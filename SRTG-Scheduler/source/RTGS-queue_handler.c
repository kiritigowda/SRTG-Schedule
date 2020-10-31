/*
*
* RTGS-queue_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"


/* Function to add future jobAttributesList releases and arrange jobAttributesList execution times in ascending order */
void queue_job_execution
(
	int processorReleased,
	int processor_release_time,
	int present_time,
	int schedule_method,
	int jobNumber,
	scheduledResourceNode **processorsAllocatedList
)
{
	PROFILER_START(SRTG, queue_job_execution)
	// TBD:: Sending Data and Jobs to GPU
	*processorsAllocatedList = ascending_insert(*processorsAllocatedList, processor_release_time, processor_release_time,
		processorReleased, jobNumber, schedule_method);
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		print_processorsAllocated_list(*processorsAllocatedList);
	}
	PROFILER_STOP(SRTG, queue_job_execution)
	return;
}

/* Function to add future jobAttributesList releases and arrange jobAttributesList execution times in ascending order */
void job_queue_handler
(
	int processorReleased,
	int job_release_time,
	int processor_release_time,
	int schedule_method,
	int jobNumber,
	scheduledResourceNode **jobScheduledQueueList
)
{
	PROFILER_START(SRTG, job_queue_handler)
	// TBD:: Sending Data and Jobs
	if (schedule_method == RTGS_SCHEDULE_METHOD_ALAP){
		*jobScheduledQueueList = ascending_insert(*jobScheduledQueueList, job_release_time, processor_release_time,
			processorReleased, jobNumber, schedule_method);
	}
	else if (schedule_method == RTGS_SCHEDULE_METHOD_AEAP_ADVANCED) {
		*jobScheduledQueueList = ascending_insert(*jobScheduledQueueList, job_release_time, processor_release_time,
			processorReleased, jobNumber, schedule_method);
	}
	else if (schedule_method == RTGS_SCHEDULE_METHOD_AEAP) {
		*jobScheduledQueueList = ascending_insert(*jobScheduledQueueList, job_release_time, job_release_time,
			processorReleased, jobNumber, schedule_method);
	}
	else {
		printf("job_queue_handler ERROR NOT IMPLEMENTED\n");
	}

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		print_preScheduledJob_list(*jobScheduledQueueList);
		if(GLOBAL_preScheduleList != NULL)
			print_preQueuedJob_list(GLOBAL_preScheduleList);
	}
	PROFILER_STOP(SRTG, job_queue_handler)
	return;
}
