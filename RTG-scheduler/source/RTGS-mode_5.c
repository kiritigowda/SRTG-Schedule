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
	//PROFILER_START(SRTG, RTGS_mode_5)
	kernelInfo kernel_info_list[MAX_KERNELS];
	scheduledNode *processor_alloc_list = NULL;
	scheduledNode *kernel_queue_list = NULL;		//Kernel queued for future executions

	// global variable initialize
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
	printf("\n**************** The GPU Scheduler will Schedule %d Jobs ****************\n", kernelMax);
#endif

	for (int present_time = 0; present_time < runTimeMax; present_time++)
	{
		// Freeing-up processors
		processorsAvailable = Retrieve_processors(present_time, processorsAvailable, &processor_alloc_list);
		processorsAvailable = Dispatch_queued_kernels(present_time, processorsAvailable, &kernel_queue_list, &processor_alloc_list);
		if (GLOBAL_RELEASE_TIME[present_time] == 1)
		{
#if DEBUG_MESSAGES
			printf("\nRTGS Mode 5 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
			printf("RTGS Mode 5 -- Job-%d Released\n", kernel_number);
#endif
			// handling the released kernel_info_list by the book-keeper
			processorsAvailable = Kernel_book_keeper(kernel_info_list, kernel_number, processorsAvailable, present_time,
				&processor_alloc_list, &kernel_queue_list);
			kernel_number++;
		}
		else if (GLOBAL_RELEASE_TIME[present_time] == 2)
		{
			int k1 = kernel_number; kernel_number++;
			int k2 = kernel_number; kernel_number++;
#if DEBUG_MESSAGES
			printf("\nRTGS Mode 5 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
			printf("RTGS Mode 5 -- Job-%d Released\n", k1);
			printf("RTGS Mode 5 -- Job-%d Released\n", k2);
#endif
			if (kernel_info_list[k1].deadline <= kernel_info_list[k2].deadline)
			{
				// handling the released kernel_info_list by the book-keeper
				processorsAvailable = Kernel_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
				processorsAvailable = Kernel_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
			}
			else
			{
				// handling the released kernel_info_list by the book-keeper
				processorsAvailable = Kernel_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
				processorsAvailable = Kernel_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
			}
		}
		else if (GLOBAL_RELEASE_TIME[present_time] > 2) { return RTGS_ERROR_NOT_IMPLEMENTED; }
	}

	if (runTimeMax != 0) {
#if DEBUG_INFO
		printf("\n******* Scheduler Mode 5 *******\n");
		printf("Processors Available -- %d\n", processorsAvailable);
		printf("Total Jobs Scheduled -- %d\n", kernelMax);
		printf("	GPU Scheduled Jobs -- %d\n", GLOBAL_GPU_KERNELS);
		printf("	CPU Scheduled Jobs -- %d\n", GLOBAL_CPU_KERNELS);
#endif
		for (int j = 0; j <= kernelMax; j++) {
			kernel_info_list[j].processor_req = kernel_info_list[j].deadline = kernel_info_list[j].execution_time = kernel_info_list[j].latest_schedulable_time = 0;
		}
		kernelMax = 0; runTimeMax = 0; kernel_number = 0; GLOBAL_GPU_KERNELS = 0; GLOBAL_CPU_KERNELS = 0;
	}

#if DEBUG_MESSAGES
	print(processor_alloc_list);
#endif
	processor_alloc_list = clean_node(processor_alloc_list);
	GLOBAL_ALAP_LIST = clean_list(GLOBAL_ALAP_LIST);
	//PROFILER_STOP(SRTG, RTGS_mode_5)
	return RTGS_SUCCESS;
}