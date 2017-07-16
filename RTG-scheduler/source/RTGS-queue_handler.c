/*
/*
* RTGS-queue_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"


/* Function to add future kernel_info_list releases and arrange kernel_info_list execution times in ascending order */
void Queue_kernel_execution
(
	int processorReleased, 
	int processor_release_time,
	int present_time, 
	int schedule_method, 
	int kernel_number, 
	scheduledNode **processor_alloc_list
)
{
	//printf("\n *** The Processors given :: %d ***\n\n", processorReleased);
	*processor_alloc_list = ascending_insert(*processor_alloc_list, processor_release_time, 
											processor_release_time, processorReleased, kernel_number, schedule_method);
#if DEBUG_MESSAGES
	print(*processor_alloc_list);
#endif
	// TBD:: Sending Data and Kernels
	return;
}

/* Function to add future kernel_info_list releases and arrange kernel_info_list execution times in ascending order */
void Kernel_queue_handler
(
	int processorReleased,
	int kernel_release_time, 
	int processor_release_time, 
	int schedule_method, 
	int kernel_number, 
	scheduledNode **kernel_queue_list
)
{
	if (schedule_method == 2) 
	{
		*kernel_queue_list = ascending_insert(*kernel_queue_list, kernel_release_time, processor_release_time, 
											processorReleased, kernel_number, schedule_method);
#if DEBUG_MESSAGES
		Kernel_queue_print(*kernel_queue_list);
#endif
	}
	else 
	{
		*kernel_queue_list = ascending_insert(*kernel_queue_list, kernel_release_time, kernel_release_time, 
											processorReleased, kernel_number, schedule_method);
#if DEBUG_MESSAGES
		Kernel_queue_print(*kernel_queue_list);
#endif
	}
	// TBD:: Sending Data and Kernels
	return;
}
