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
	PROFILER_START(SRTG, Queue_kernel_execution)
	// TBD:: Sending Data and Kernels
	*processor_alloc_list = ascending_insert(*processor_alloc_list, processor_release_time, processor_release_time, 
											processorReleased, kernel_number, schedule_method);
#if DEBUG_MESSAGES
	print(*processor_alloc_list);
#endif
	PROFILER_STOP(SRTG, Queue_kernel_execution)
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
	PROFILER_START(SRTG, Kernel_queue_handler)
	// TBD:: Sending Data and Kernels
	if (schedule_method == RTGS_SCHEDULE_METHOD_ALAP)
	{
		*kernel_queue_list = ascending_insert(*kernel_queue_list, kernel_release_time, processor_release_time,
			processorReleased, kernel_number, schedule_method);
#if DEBUG_MESSAGES
		Kernel_queue_print(*kernel_queue_list);
#endif
	}
	else if (schedule_method == RTGS_SCHEDULE_METHOD_AEAP)
	{
		*kernel_queue_list = ascending_insert(*kernel_queue_list, kernel_release_time, kernel_release_time,
			processorReleased, kernel_number, schedule_method);
#if DEBUG_MESSAGES
		Kernel_queue_print(*kernel_queue_list);
#endif
	}
	else
	{
		printf("Kernel_queue_handler ERROR NOT IMPLEMENTED");
	}
	PROFILER_STOP(SRTG, Kernel_queue_handler)
	return;
}
