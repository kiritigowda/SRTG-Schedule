/*
*
* RTGS-queue_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"


/* Function to add future jobAttributesList releases and arrange jobAttributesList execution times in ascending order */
void Queue_kernel_execution
(
	int processorReleased,
	int processor_release_time,
	int present_time,
	int schedule_method,
	int jobNumber,
	scheduledJobNode **processorsAllocatedList
)
{
	PROFILER_START(SRTG, Queue_kernel_execution)
	// TBD:: Sending Data and Jobs to GPU
	*processorsAllocatedList = ascending_insert(*processorsAllocatedList, processor_release_time, processor_release_time,
		processorReleased, jobNumber, schedule_method);
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		print(*processorsAllocatedList);
	}
	PROFILER_STOP(SRTG, Queue_kernel_execution)
	return;
}

/* Function to add future jobAttributesList releases and arrange jobAttributesList execution times in ascending order */
void Kernel_queue_handler
(
	int processorReleased,
	int job_release_time,
	int processor_release_time,
	int schedule_method,
	int jobNumber,
	scheduledJobNode **jobSchdeuleQueueList
)
{
	PROFILER_START(SRTG, Kernel_queue_handler)
	// TBD:: Sending Data and Jobs
	if (schedule_method == RTGS_SCHEDULE_METHOD_ALAP)
	{
		*jobSchdeuleQueueList = ascending_insert(*jobSchdeuleQueueList, job_release_time, processor_release_time,
			processorReleased, jobNumber, schedule_method);
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			Kernel_queue_print(*jobSchdeuleQueueList);
		}
	}
	else if (schedule_method == RTGS_SCHEDULE_METHOD_AEAP)
	{
		*jobSchdeuleQueueList = ascending_insert(*jobSchdeuleQueueList, job_release_time, job_release_time,
			processorReleased, jobNumber, schedule_method);
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			Kernel_queue_print(*jobSchdeuleQueueList);
		}
	}
	else
	{
		printf("Kernel_queue_handler ERROR NOT IMPLEMENTED");
	}
	PROFILER_STOP(SRTG, Kernel_queue_handler)
	return;
}
