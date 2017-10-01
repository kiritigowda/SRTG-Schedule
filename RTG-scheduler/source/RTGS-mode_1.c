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
#if DEBUG_MESSAGES
	printf("Mode-1 Book Keeper:: Kernel::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n", kernel_number, kernel_info_list[kernel_number].processor_req, kernel_info_list[kernel_number].execution_time, kernel_info_list[kernel_number].deadline, kernel_info_list[kernel_number].latest_schedulable_time);
#endif
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
			GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
			printf("Mode-1 Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
		}
		else 
		{
			GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
			printf("Mode-1 Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
			printf("Mode-1 Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
		}
	}
	else 
	{
		GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
		printf("Mode-1 Book Keeper:: No Processors Available for Kernel-%d, Job REJECTED\n", kernel_number);
		printf("Mode-1 Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
	}

	return processors_available;
}

/**********************************************************************************************************
RTGS Mode 1 - - Simple GPU Schedulers
***********************************************************************************************************/
int RTGS_mode_1(char *kernelFilename, char *releaseTimeFilename)
{
	kernelInfo kernel_info_list[MAX_KERNELS] = { 0 };
	scheduledNode *processor_alloc_list = NULL;
	scheduledNode *kernel_queue_list = NULL;

	// global variables initialized
	GLOBAL_GPU_KERNELS = 0;
	GLOBAL_CPU_KERNELS = 0;
	GLOBAL_ALAP_LIST = NULL;

	int processorsAvailable = MAX_GPU_PROCESSOR;
	int kernel_number = 0;

	int kernelMax = get_kernel_information(kernel_info_list, kernelFilename);// Read Kernel.TXT
	if (kernelMax <= RTGS_FAILURE) { return  RTGS_FAILURE; }
	int runTimeMax = get_kernel_release_times(releaseTimeFilename);// Read Release_time.TXT
	if (runTimeMax <= RTGS_FAILURE) { return  RTGS_FAILURE; }
#if DEBUG_MESSAGES
	printf("\nThe GPU Scheduler will Schedule %d Kernels\n", kernelMax);			    // Scheduler Begins
#endif

	int64_t stime = RTGS_GetClockCounter();
	for (int present_time = 0; present_time < runTimeMax; present_time++)
	{
		// Freeing-up processors
		processorsAvailable = Retrieve_processors(present_time, processorsAvailable, &processor_alloc_list);
		processorsAvailable = Dispatch_queued_kernels(present_time, processorsAvailable, &kernel_queue_list, &processor_alloc_list);

		if (GLOBAL_RELEASE_TIME[present_time] == 1)
		{
#if DEBUG_MESSAGES
			printf("RTGS Mode 1:: Total processors Available at time %d = %d\n", present_time, processorsAvailable);
			printf("RTGS Mode 1:: Kernels:%d Released\n", kernel_number);
#endif
			processorsAvailable = Mode_1_book_keeper(kernel_info_list, kernel_number, processorsAvailable, present_time, &processor_alloc_list); // handling the released kernel_info_list by the book-keeper
			kernel_number++;
		}
		else if (GLOBAL_RELEASE_TIME[present_time] == 2)
		{
			int kernel_1 = kernel_number; kernel_number++;
			int kernel_2 = kernel_number; kernel_number++;
#if DEBUG_MESSAGES
			printf("RTGS Mode 1:: Total processors Available at time %d = %d\n", present_time, processorsAvailable);
			printf("RTGS Mode 1:: Kernels:%d Released\n", kernel_1);
			printf("RTGS Mode 1:: Kernels:%d Released\n", kernel_2);
#endif

			if (kernel_info_list[kernel_1].deadline <= kernel_info_list[kernel_2].deadline) {
				// handling the released kernel_info_list by the book-keeper
				processorsAvailable = Mode_1_book_keeper(kernel_info_list, kernel_1, processorsAvailable, present_time, &processor_alloc_list);
				processorsAvailable = Mode_1_book_keeper(kernel_info_list, kernel_2, processorsAvailable, present_time, &processor_alloc_list);
			}
			else {
				// handling the released kernel_info_list by the book-keeper
				processorsAvailable = Mode_1_book_keeper(kernel_info_list, kernel_2, processorsAvailable, present_time, &processor_alloc_list);
				processorsAvailable = Mode_1_book_keeper(kernel_info_list, kernel_1, processorsAvailable, present_time, &processor_alloc_list);

			}
		}
		else if (GLOBAL_RELEASE_TIME[present_time] > 2) { return RTGS_ERROR_NOT_IMPLEMENTED; }
	}

	if (runTimeMax != 0)
	{
#if DEBUG_INFO
		printf("\n******* Scheduler Mode 1 *******\n");
		printf("Processors Available -- %d\n", processorsAvailable);
		printf("Total Kernels Scheduled -- %d\n", kernelMax);
		printf("	GPU Scheduled Kernels -- %d\n", GLOBAL_GPU_KERNELS);
		printf("	CPU Scheduled Kernels -- %d\n", GLOBAL_CPU_KERNELS);
#endif
		for (int j = 0; j <= kernelMax; j++)
		{
			kernel_info_list[j].processor_req = 0;
			kernel_info_list[j].deadline = 0;
			kernel_info_list[j].execution_time = 0;
			kernel_info_list[j].latest_schedulable_time = 0;
		}
		kernelMax = 0; runTimeMax = 0; kernel_number = 0; GLOBAL_GPU_KERNELS = 0; GLOBAL_CPU_KERNELS = 0;
	}

#if DEBUG_MESSAGES
	print(processor_alloc_list);
#endif
	processor_alloc_list = clean_node(processor_alloc_list);
	GLOBAL_ALAP_LIST = clean_list(GLOBAL_ALAP_LIST);

	return RTGS_SUCCESS;
}