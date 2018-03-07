/*
* RTGS-mode_1.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/***********************************************************************************************************
MODE 1 FUNCTIONS
**********************************************************************************************************/
static int Mode_1_book_keeper
(
	kernelInfo* kernel_info_list,
	int kernel_number,
	int processors_available,
	int present_time,
	scheduledNode** processor_alloc_list
)
{
	int processorReleased = 0, processorReleaseTime = 0;
	int presentTime = present_time;
	int scheduleMethod = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode-1 Book Keeper:: Kernel::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n", kernel_number, kernel_info_list[kernel_number].processor_req, kernel_info_list[kernel_number].execution_time, kernel_info_list[kernel_number].deadline, kernel_info_list[kernel_number].latest_schedulable_time);
	}
	// If processors available is greater than the required processors by the kernel_info_list
	if (kernel_info_list[kernel_number].processor_req <= processors_available)
	{
		if (kernel_info_list[kernel_number].execution_time + presentTime <= kernel_info_list[kernel_number].deadline) 
		{
			processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
			processorReleased = kernel_info_list[kernel_number].processor_req;
			processorReleaseTime = kernel_info_list[kernel_number].execution_time + presentTime;
			scheduleMethod = RTGS_SCHEDULE_METHOD_IMMEDIATE;
			// Kernel call for the GPU to handle the given Kernels and number of blocks
			Queue_kernel_execution(processorReleased, processorReleaseTime, presentTime, scheduleMethod, kernel_number, processor_alloc_list);

            kernel_info_list[kernel_number].schedule_hardware = 1;
            kernel_info_list[kernel_number].rescheduled_execution = -1;
            kernel_info_list[kernel_number].scheduled_execution = present_time;
            kernel_info_list[kernel_number].completion_time = kernel_info_list[kernel_number].execution_time + present_time;
			GLOBAL_GPU_KERNELS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode-1 Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
			}
		}
		else 
		{
            kernel_info_list[kernel_number].schedule_hardware = 2;
            kernel_info_list[kernel_number].rescheduled_execution = -1;
            kernel_info_list[kernel_number].completion_time = -1;
            kernel_info_list[kernel_number].scheduled_execution = -1;
			GLOBAL_CPU_KERNELS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode-1 Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
				printf("Mode-1 Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
			}
		}
	}
	else 
	{
        kernel_info_list[kernel_number].schedule_hardware = 2;
        kernel_info_list[kernel_number].rescheduled_execution = -1;
        kernel_info_list[kernel_number].completion_time = -1;
        kernel_info_list[kernel_number].scheduled_execution = -1;
		GLOBAL_CPU_KERNELS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode-1 Book Keeper:: No Processors Available for Kernel-%d, Job REJECTED\n", kernel_number);
			printf("Mode-1 Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
		}
	}

	return processors_available;
}

/**********************************************************************************************************
RTGS Mode 1 - - Simple GPU Schedulers
***********************************************************************************************************/
int RTGS_mode_1(char *kernelFilename, char *releaseTimeFilename)
{
	kernelInfo kernel_info_list[MAX_KERNELS] = { 0 };
	kernelReleaseInfo releaseTimeInfo[MAX_KERNELS] = { 0 };
	scheduledNode *processor_alloc_list = NULL;
	scheduledNode *kernel_queue_list = NULL;

	// global variables initialized
	GLOBAL_GPU_KERNELS = 0;
	GLOBAL_CPU_KERNELS = 0;
	GLOBAL_ALAP_LIST = NULL;

    int processorsAvailable = MAX_GPU_PROCESSOR;
    int kernel_number = 0;

    int kernelMax = get_kernel_information(kernel_info_list, kernelFilename);
    if (kernelMax <= RTGS_FAILURE) { return  RTGS_FAILURE; }
    int maxReleases = get_kernel_release_times(releaseTimeInfo, releaseTimeFilename);
    if (maxReleases <= RTGS_FAILURE) { return  RTGS_FAILURE; }

    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
        printf("\n**************** The GPU Scheduler will Schedule %d Jobs ****************\n", kernelMax);
    }

    int numReleases = 0;
    for (int present_time = 0; present_time < MAX_RUN_TIME; present_time++)
    {
        // Freeing-up processors
        processorsAvailable = Retrieve_processors(present_time, processorsAvailable, &processor_alloc_list);
        processorsAvailable = Dispatch_queued_kernels(present_time, processorsAvailable, &kernel_queue_list, &processor_alloc_list);

        if (releaseTimeInfo[numReleases].release_time == present_time) {

            if (releaseTimeInfo[numReleases].num_kernel_released == 1)
            {
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("\nRTGS Mode 1 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
                    printf("RTGS Mode 1 -- Job-%d Released\n", kernel_number);
                }
                kernel_info_list[kernel_number].release_time = present_time;
                // handling the released kernel_info_list by the book-keeper
                int64_t start_t = RTGS_GetClockCounter();
                processorsAvailable = Mode_1_book_keeper(kernel_info_list, kernel_number, processorsAvailable, present_time,
                    &processor_alloc_list);
                int64_t end_t = RTGS_GetClockCounter();
                int64_t freq = RTGS_GetClockFrequency();
                float factor = 1000.0f / (float)freq; // to convert clock counter to ms
                float SchedulerOverhead = (float)((end_t - start_t) * factor);
                kernel_info_list[kernel_number].schedule_overhead = SchedulerOverhead;
                kernel_number++;
            }
            else if (releaseTimeInfo[numReleases].num_kernel_released == 2)
            {
                int k1 = kernel_number; kernel_number++;
                int k2 = kernel_number; kernel_number++;
                kernel_info_list[k1].release_time = present_time;
                kernel_info_list[k2].release_time = present_time;

                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("\nRTGS Mode 1 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
                    printf("RTGS Mode 1 -- Job-%d Released\n", k1);
                    printf("RTGS Mode 1 -- Job-%d Released\n", k2);
                }
                if (kernel_info_list[k1].deadline <= kernel_info_list[k2].deadline)
                {
                    // handling the released kernel_info_list by the book-keeper
                    int64_t start_t = RTGS_GetClockCounter();
                    processorsAvailable = Mode_1_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list);
                    int64_t end_t = RTGS_GetClockCounter();
                    int64_t freq = RTGS_GetClockFrequency();
                    float factor = 1000.0f / (float)freq; // to convert clock counter to ms
                    float SchedulerOverhead = (float)((end_t - start_t) * factor);
                    kernel_info_list[k1].schedule_overhead = SchedulerOverhead;
                    start_t = RTGS_GetClockCounter();
                    processorsAvailable = Mode_1_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list);
                    end_t = RTGS_GetClockCounter();
                    SchedulerOverhead = (float)((end_t - start_t) * factor);
                    kernel_info_list[k2].schedule_overhead = SchedulerOverhead;
                }
                else
                {
                    // handling the released kernel_info_list by the book-keeper
                    int64_t start_t = RTGS_GetClockCounter();
                    processorsAvailable = Mode_1_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list);
                    int64_t end_t = RTGS_GetClockCounter();
                    int64_t freq = RTGS_GetClockFrequency();
                    float factor = 1000.0f / (float)freq; // to convert clock counter to ms
                    float SchedulerOverhead = (float)((end_t - start_t) * factor);
                    kernel_info_list[k2].schedule_overhead = SchedulerOverhead;
                    start_t = RTGS_GetClockCounter();
                    processorsAvailable = Mode_1_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list);
                    end_t = RTGS_GetClockCounter();
                    SchedulerOverhead = (float)((end_t - start_t) * factor);
                    kernel_info_list[k1].schedule_overhead = SchedulerOverhead;
                }
            }
            else if (releaseTimeInfo[numReleases].num_kernel_released > 2) { return RTGS_ERROR_NOT_IMPLEMENTED; }

            numReleases++;
            if (numReleases > maxReleases) {
                printf("RTGS Mode 1 ERROR --  KERNEL Release Time exceded Max Releases\n");
                return RTGS_ERROR_INVALID_PARAMETERS;
            }
        }
    }

    if (maxReleases != 0) {

        if (GLOBAL_RTGS_DEBUG_MSG) {
            printf("\n******* Scheduler Mode 1 *******\n");
            printf("Processors Available -- %d\n", processorsAvailable);
            printf("Total Jobs Scheduled -- %d\n", kernelMax);
            printf("	GPU Scheduled Jobs -- %d\n", GLOBAL_GPU_KERNELS);
            printf("	CPU Scheduled Jobs -- %d\n", GLOBAL_CPU_KERNELS);
        }

        if (RTGS_PrintScheduleSummary(1, kernelMax, kernel_info_list)) {
            printf("\nSummary Failed\n");
        }

        for (int j = 0; j <= kernelMax; j++) {
            kernel_info_list[j].processor_req = kernel_info_list[j].deadline = kernel_info_list[j].execution_time = kernel_info_list[j].latest_schedulable_time = 0;
        }
        kernelMax = 0; maxReleases = 0; kernel_number = 0; GLOBAL_GPU_KERNELS = 0; GLOBAL_CPU_KERNELS = 0;
    }

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		print(processor_alloc_list);
	}
	processor_alloc_list = clean_node(processor_alloc_list);
	GLOBAL_ALAP_LIST = clean_list(GLOBAL_ALAP_LIST);

	return RTGS_SUCCESS;
}
