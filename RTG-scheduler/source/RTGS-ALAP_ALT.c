/*
* RTGS-ALAP_ALT.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP_advanced
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	PROFILER_START(SRTG, ALAP_advanced)
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("ALAP advanced: Job:%d is verified for ALAP advanced scheduling\n", jobNumber);
		}

	int localProcessors = 0, job_release_time = 0, processor_release_time = 0, processorsInUse = 0;
	processorsInUse = jobAttributesList[jobNumber].processor_req;
	processor_release_time = jobAttributesList[jobNumber].deadline;
	job_release_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
	int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

	scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;
	genericBackupNode *lastALAPJobScheduled = GLOBAL_preScheduleList;

	// fail case return
	if (present_time >= job_release_time) {
		jobAttributesList[jobNumber].schedule_hardware = 2;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].completion_time = -1;
		jobAttributesList[jobNumber].scheduled_execution = -1;
		GLOBAL_CPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
		return processors_available;
	}

	int processorsQueued = 0;
	int processorsQueuedRelease = 0;
	while (lastALAPJobScheduled != NULL) {
		processorsQueued += lastALAPJobScheduled->processors_allocated;
		if (processorsQueuedRelease < lastALAPJobScheduled->processor_release_time)
			processorsQueuedRelease = lastALAPJobScheduled->processor_release_time;
		if (processorsQueued >= jobAttributesList[jobNumber].processor_req)
			break;
		lastALAPJobScheduled = lastALAPJobScheduled->next;
	}
	lastALAPJobScheduled = GLOBAL_preScheduleList;

	int processorsRetrived = 0;
	int processorsReleaseTime = 0;
	while (localProcessorsAllocatedList != NULL) {
		processorsRetrived += localProcessorsAllocatedList->processors_allocated;
		if (processorsReleaseTime <localProcessorsAllocatedList->processor_release_time)
			processorsReleaseTime = localProcessorsAllocatedList->processor_release_time;
		if (processorsRetrived >= jobAttributesList[jobNumber].processor_req)
			break;
		localProcessorsAllocatedList = localProcessorsAllocatedList->next;
	}
	localProcessorsAllocatedList = *processorsAllocatedList;

	if ((processorsQueuedRelease + jobAttributesList[jobNumber].execution_time) <= jobAttributesList[jobNumber].deadline &&
		(processorsReleaseTime + jobAttributesList[jobNumber].execution_time) <= jobAttributesList[jobNumber].deadline)
	{
		// condition 3
		if (processors_available >= jobAttributesList[jobNumber].processor_req)
		{
			processorsQueued = 0;
			while (lastALAPJobScheduled != NULL) {
				processorsQueued += lastALAPJobScheduled->processors_allocated;
				if (processorsQueued >= jobAttributesList[jobNumber].processor_req) {
					lastALAPJobScheduled->processors_allocated = processorsQueued - jobAttributesList[jobNumber].processor_req;
					break;
				}
				else {
					lastALAPJobScheduled->processors_allocated = 0;
				}
				lastALAPJobScheduled = lastALAPJobScheduled->next;
			}
			lastALAPJobScheduled = GLOBAL_preScheduleList;

			jobAttributesList[jobNumber].schedule_hardware = 1;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].scheduled_execution = job_release_time;
			jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
			GLOBAL_GPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("ALAP advanced: Condition 3 pass, The Job:%d scheduled\n", jobNumber);
				printf("ALAP advanced: Condition 3 pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
			GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time,
				processorsInUse, jobNumber);
			job_queue_handler(processorsInUse, job_release_time, processor_release_time,
				schedule_method, jobNumber, jobScheduledQueueList);
			return processors_available;
		}
		// condition 4
		else if (processorsQueued >= jobAttributesList[jobNumber].processor_req)
		{
			processorsQueued = 0;
			while (lastALAPJobScheduled != NULL) {
				processorsQueued += lastALAPJobScheduled->processors_allocated;
				if (processorsQueued >= jobAttributesList[jobNumber].processor_req) {
					lastALAPJobScheduled->processors_allocated = processorsQueued - jobAttributesList[jobNumber].processor_req;
					break;
				}
				else {
					lastALAPJobScheduled->processors_allocated = 0;
				}
				lastALAPJobScheduled = lastALAPJobScheduled->next;
			}
			lastALAPJobScheduled = GLOBAL_preScheduleList;

			jobAttributesList[jobNumber].schedule_hardware = 1;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].scheduled_execution = job_release_time;
			jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
			GLOBAL_GPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("ALAP advanced: Condition 4 pass, The Job:%d scheduled\n", jobNumber);
				printf("ALAP advanced: Condition 4 pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
			GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time,
				processorsInUse, jobNumber);
			job_queue_handler(processorsInUse, job_release_time, processor_release_time,
				schedule_method, jobNumber, jobScheduledQueueList);
			return processors_available;
		}
		else
		{
			localProcessors = MAX_GPU_PROCESSOR;
			if (jobAttributesList[jobNumber].processor_req <= localProcessors)
			{
				processorsQueued = 0;
				while (lastALAPJobScheduled != NULL) {
					processorsQueued += lastALAPJobScheduled->processors_allocated;
					if (processorsQueued >= jobAttributesList[jobNumber].processor_req) {
						lastALAPJobScheduled->processors_allocated = processorsQueued - jobAttributesList[jobNumber].processor_req;
						break;
					}
					else {
						lastALAPJobScheduled->processors_allocated = 0;
					}
					lastALAPJobScheduled = lastALAPJobScheduled->next;
				}
				lastALAPJobScheduled = GLOBAL_preScheduleList;

				jobAttributesList[jobNumber].schedule_hardware = 1;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].scheduled_execution = job_release_time;
				jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
				GLOBAL_GPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("ALAP advanced: Condition 4 b pass, The Job:%d scheduled\n", jobNumber);
					printf("ALAP advanced: Condition 4 b pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
				}
				GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time,
					processorsInUse, jobNumber);
				job_queue_handler(processorsInUse, job_release_time, processor_release_time,
					schedule_method, jobNumber, jobScheduledQueueList);
				return processors_available;
			}
		}
	}
	else
	{
		jobAttributesList[jobNumber].schedule_hardware = 2;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].completion_time = -1;
		jobAttributesList[jobNumber].scheduled_execution = -1;
		GLOBAL_CPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("ALAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("ALAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
	}
	PROFILER_STOP(SRTG, ALAP_advanced)
	return processors_available;

}
