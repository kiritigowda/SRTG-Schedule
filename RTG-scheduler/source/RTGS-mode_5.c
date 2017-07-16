/*
* RTG-mode_5.c
*      Author: Kiriti Nagesh Gowda
*/
#include"RTGS.h"


/**********************************************************************************************************
RTGS Mode 5 - - AEAP/ALAP BP with APLAP improver mode->AEAP/ALAP BP Improve
***********************************************************************************************************/
int  RTGS_mode_5(char *kernel_file, char *Releasetime_file)
{
	kernelInfo kernel_info_list[MAX_KERNELS];
	scheduledNode *processor_alloc_list = NULL; // Processor release list;
	scheduledNode *kernel_queue_list = NULL;  // Kernel queued for future executions

	int processors_available = MAX_GPU_PROCESSOR;
	int Nkr = 0;
	int rt = 0, kernel_number = 0, k1, k2;
	GLOBAL_GPU_KERNELS = 0;
	GLOBAL_CPU_KERNELS = 0;
	GLOBAL_ALAP_LIST = NULL;

	Nkr = get_kernel_information(kernel_info_list, kernel_file);                 				// Read Kernel.TXT
	rt = get_kernel_release_times(Releasetime_file);                   				// Read Release_time.TXT

#if DEBUG_MESSAGES
	printf("\nThe GPU Scheduler will Schedule %d Kernels\n\n", Nkr);				// Scheduler Begins
#endif

	for (int present_time = 0; present_time < rt; present_time++)
	{
		processors_available = Retrieve_processors(present_time, processors_available, &processor_alloc_list);						// Freeing-up processors
		processors_available = Dispatch_queued_kernels(present_time, processors_available, &kernel_queue_list, &processor_alloc_list);     // Freeing-up processors

		if (GLOBAL_RELEASE_TIME[present_time] == 1)
		{
#if DEBUG_MESSAGES
			printf("\n-->>Total processors Available at time %d = %d\n\n ", present_time, processors_available);
			printf("Kernels:%d has been released\n", kernel_number);
#endif
			processors_available = Kernel_book_keeper(kernel_info_list, kernel_number, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
			kernel_number++;
		}

		else if (GLOBAL_RELEASE_TIME[present_time] == 2)
		{
			k1 = kernel_number; kernel_number++;
			k2 = kernel_number; kernel_number++;
#if DEBUG_MESSAGES
			printf("\n-->>Total processors Available at time %d = %d\n\n ", present_time, processors_available);
			printf("Kernels:%d has been released\n", k1);
			printf("Kernels:%d has been released\n", k2);
#endif
			if (kernel_info_list[k1].deadline <= kernel_info_list[k2].deadline)
			{
				processors_available = Kernel_book_keeper(kernel_info_list, k1, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
				processors_available = Kernel_book_keeper(kernel_info_list, k2, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
			}
			else
			{
				processors_available = Kernel_book_keeper(kernel_info_list, k2, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
				processors_available = Kernel_book_keeper(kernel_info_list, k1, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
			}
		}
	}
	processor_alloc_list = clean_node(processor_alloc_list);
	GLOBAL_ALAP_LIST = clean_list(GLOBAL_ALAP_LIST);

	if (rt != 0)
	{
#if DEBUG_INFO
		printf("All Kernels Scheduled or Sent to CPU Successfully - Processors Available: %d Mode_2_AEAP Kernels: %d\n", processors_available, GLOBAL_GPU_KERNELS);
		printf("Kernels sent Back to CPU: %d\n", GLOBAL_CPU_KERNELS);
#endif
		for (int j = 0; j <= Nkr; j++)
		{
			kernel_info_list[j].processor_req = kernel_info_list[j].deadline = kernel_info_list[j].execution_time = kernel_info_list[j].latest_schedulable_time = 0;
		}
		Nkr = 0;
		rt = 0;
		kernel_number = 0;
		GLOBAL_GPU_KERNELS = 0;
		GLOBAL_CPU_KERNELS = 0;
	}

#if DEBUG_MESSAGES
	print(processor_alloc_list);
#endif
	processor_alloc_list = clean_node(processor_alloc_list);

	return RTGS_SUCCESS;
}

