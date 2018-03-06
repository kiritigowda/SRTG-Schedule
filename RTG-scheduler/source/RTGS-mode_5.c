/*
* RTG-mode_5.c
*      Author: Kiriti Nagesh Gowda
*/
#include"RTGS.h"


/**********************************************************************************************************
RTGS Mode 5 - - AEAP/ALAP BP with APLAP improver mode->AEAP/ALAP BP Improve
***********************************************************************************************************/
int  RTGS_mode_5(char *kernelFilename, char *releaseTimeFilename)
{
	PROFILER_START(SRTG, RTGS_mode_5)
	kernelInfo kernel_info_list[MAX_KERNELS] = { 0 };
	kernelReleaseInfo releaseTimeInfo[MAX_KERNELS] = { 0 };
	scheduledNode *processor_alloc_list = NULL;
	scheduledNode *kernel_queue_list = NULL;		//Kernel queued for future executions

	// global variable initialize
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
					printf("\nRTGS Mode 5 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 5 -- Job-%d Released\n", kernel_number);
				}
				kernel_info_list[kernel_number].release_time = present_time;
				// handling the released kernel_info_list by the book-keeper
				int64_t start_t = RTGS_GetClockCounter();
				processorsAvailable = Kernel_book_keeper(kernel_info_list, kernel_number, processorsAvailable, present_time,
					&processor_alloc_list, &kernel_queue_list);
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
					printf("\nRTGS Mode 5 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 5 -- Job-%d Released\n", k1);
					printf("RTGS Mode 5 -- Job-%d Released\n", k2);
				}
				if (kernel_info_list[k1].deadline <= kernel_info_list[k2].deadline)
				{
					// handling the released kernel_info_list by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Kernel_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k1].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Kernel_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k2].schedule_overhead = SchedulerOverhead;
				}
				else
				{
					// handling the released kernel_info_list by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Kernel_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k2].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Kernel_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k1].schedule_overhead = SchedulerOverhead;
				}
			}
			else if (releaseTimeInfo[numReleases].num_kernel_released > 2) { return RTGS_ERROR_NOT_IMPLEMENTED; }

			numReleases++;
			if (numReleases > maxReleases) {
				printf("RTGS Mode 5 ERROR --  KERNEL Release Time exceded Max Releases\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	if (maxReleases != 0) {

		if (GLOBAL_RTGS_DEBUG_MSG) {
			printf("\n******* Scheduler Mode 5 *******\n");
			printf("Processors Available -- %d\n", processorsAvailable);
			printf("Total Jobs Scheduled -- %d\n", kernelMax);
			printf("	GPU Scheduled Jobs -- %d\n", GLOBAL_GPU_KERNELS);
			printf("	CPU Scheduled Jobs -- %d\n", GLOBAL_CPU_KERNELS);
		}

		if (RTGS_PrintScheduleSummary(5, kernelMax, kernel_info_list)) {
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
	PROFILER_STOP(SRTG, RTGS_mode_5)
	return RTGS_SUCCESS;
}