/*
* RTGS-mode_3.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"


/**********************************************************************************************************
Mode 3 FUNCTION
***********************************************************************************************************/
static int Mode_3_ALAP_advanced
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode 3 ALAP advanced: Job:%d is verified for ALAP advanced scheduling\n", jobNumber);
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
			printf("Mode 3 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 3 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
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
				printf("Mode 3 ALAP advanced: Condition 3 pass, The Job:%d scheduled\n", jobNumber);
				printf("Mode 3 ALAP advanced: Condition 3 pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
				printf("Mode 3 ALAP advanced: Condition 4 pass, The Job:%d scheduled\n", jobNumber);
				printf("Mode 3 ALAP advanced: Condition 4 pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
					printf("Mode 3 ALAP advanced: Condition 4 b pass, The Job:%d scheduled\n", jobNumber);
					printf("Mode 3 ALAP advanced: Condition 4 b pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
			printf("Mode 3 ALAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 3 ALAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
	}
	return processors_available;
}


/**********************************************************************************************************
MODE 3 FUNCTION
***********************************************************************************************************/
static int Mode_3_ALAP
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	int localProcessors = 0, job_release_time = 0, processor_release_time = 0, processors_allocated = 0;
	genericBackupNode *processorsDistList = NULL;
	processors_allocated = jobAttributesList[jobNumber].processor_req;
	processor_release_time = jobAttributesList[jobNumber].deadline;
	job_release_time = processor_release_time - jobAttributesList[jobNumber].execution_time;
	localProcessors = processors_available;

	// fail case return
	if (present_time >= job_release_time) {
		jobAttributesList[jobNumber].schedule_hardware = 2;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].completion_time = -1;
		jobAttributesList[jobNumber].scheduled_execution = -1;
		GLOBAL_CPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 3 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 3 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
		return processors_available;
	}

	if (localProcessors >= jobAttributesList[jobNumber].processor_req)
	{
		int processorsInUse = jobAttributesList[jobNumber].processor_req;
		int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

		jobAttributesList[jobNumber].schedule_hardware = 1;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].scheduled_execution = job_release_time;
		jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
		GLOBAL_GPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 3 ALAP: The Job:%d scheduled\n", jobNumber);
			printf("Mode 3 ALAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
		}
		GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time,
			processor_release_time, processors_allocated, jobNumber);
		job_queue_handler(processorsInUse, job_release_time, processor_release_time,
			schedule_method, jobNumber, jobScheduledQueueList);
		return processors_available;
	}

	processorsDistList = insert_preScheduledJob_list(processorsDistList, job_release_time, processor_release_time, processors_available, jobNumber);
	scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;

	while (localProcessorsAllocatedList != NULL)
	{
		if ((localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
		{
			int count = 0;
			scheduledResourceNode*temp1 = *processorsAllocatedList;
			genericBackupNode* temp2 = processorsDistList;

			while (temp2 != NULL)
			{
				if (count == 0)
					temp2 = temp2->next;
				else {
					temp1->job_release_time = 0;
					temp1 = temp1->next;
					temp2 = temp2->next;
				}
				count++;
			}
			processorsDistList = clean_list(processorsDistList);
			jobAttributesList[jobNumber].schedule_hardware = 2;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].completion_time = -1;
			jobAttributesList[jobNumber].scheduled_execution = -1;
			GLOBAL_CPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode 3 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
				printf("Mode 3 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
			}
			break;
		}
		else
		{
			localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
			if (localProcessors >= jobAttributesList[jobNumber].processor_req)
			{
				localProcessorsAllocatedList->job_release_time = job_release_time;
				processorsDistList = clean_list(processorsDistList);
				int processorsInUse = jobAttributesList[jobNumber].processor_req;
				int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

				jobAttributesList[jobNumber].schedule_hardware = 1;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].scheduled_execution = job_release_time;
				jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
				GLOBAL_GPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 3 ALAP: The Job:%d scheduled\n", jobNumber);
					printf("Mode 3 ALAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
				}
				GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time,
					processors_allocated, jobNumber);
				job_queue_handler(processorsInUse, job_release_time, processor_release_time,
					schedule_method, jobNumber, jobScheduledQueueList);
				break;
			}
			else
			{
				processorsDistList = insert_preScheduledJob_list(processorsDistList, job_release_time, processor_release_time, localProcessorsAllocatedList->processors_allocated, localProcessorsAllocatedList->jobNumber);
				localProcessorsAllocatedList->job_release_time = job_release_time;
			}
		}
		localProcessorsAllocatedList = localProcessorsAllocatedList->next;
	}
	return processors_available;
}

static int Mode_3_AEAP
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	int localProcessors = 0, job_release_time = 0;
	genericBackupNode *processorsDistList = NULL;
	scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;

	localProcessors = processors_available;
	processorsDistList = insert_node(processorsDistList, processors_available);
	processors_available = 0;

	if (GLOBAL_preScheduleList == NULL)
	{
		while (localProcessorsAllocatedList != NULL)
		{
			if ((localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
			{
				int count = 0;
				scheduledResourceNode *temp1 = *processorsAllocatedList;
				genericBackupNode *temp2 = processorsDistList;
				while (temp2 != NULL)
				{
					if (count == 0) {
						processors_available = temp2->data;
						temp2 = temp2->next;
					}
					else {
						temp1->processors_allocated = temp2->data;
						temp1 = temp1->next;
						temp2 = temp2->next;
					}
					count++;
				}
				processorsDistList = clean_list(processorsDistList);

				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 3 AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("Mode 3 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				break;
			}
			else
			{
				localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
				if (localProcessors >= jobAttributesList[jobNumber].processor_req)
				{
					localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
					job_release_time = localProcessorsAllocatedList->processor_release_time;
					processorsDistList = clean_list(processorsDistList);

					int processorsInUse = jobAttributesList[jobNumber].processor_req;
					int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

					jobAttributesList[jobNumber].schedule_hardware = 1;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].scheduled_execution = job_release_time;
					jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
					GLOBAL_GPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 3 AEAP: The Job:%d scheduled\n", jobNumber);
						printf("Mode 3 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					queue_job_execution(processorsInUse, processor_release_time, presentTime, schedule_method, jobNumber, processorsAllocatedList);
					job_queue_handler(processorsInUse, job_release_time, presentTime, schedule_method, jobNumber, jobScheduledQueueList);
					break;
				}
				else
				{
					processorsDistList = insert_node(processorsDistList, localProcessorsAllocatedList->processors_allocated);
					localProcessorsAllocatedList->processors_allocated = 0;
				}
			}
			localProcessorsAllocatedList = localProcessorsAllocatedList->next;
		}
	}
	else
	{
		while (localProcessorsAllocatedList != NULL)
		{
			if ((localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
			{
				int count = 0;
				scheduledResourceNode *temp1 = *processorsAllocatedList;
				genericBackupNode *temp2 = processorsDistList;
				while (temp2 != NULL) {
					if (count == 0) {
						processors_available = temp2->data;
						temp2 = temp2->next;
					}
					else {
						temp1->processors_allocated = temp2->data;
						temp1 = temp1->next;
						temp2 = temp2->next;
					}
					count++;
				}
				// TBD:: Return Job to CPU - Function to send Job to CPU execution
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				processorsDistList = clean_list(processorsDistList);
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 3 AEAP: The Job:%d Cannot be scheduled AEAP\n", jobNumber);
					printf("Mode 3 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				break;
			}
			else
			{
				localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
				if (localProcessors >= jobAttributesList[jobNumber].processor_req)
				{
					job_release_time = localProcessorsAllocatedList->processor_release_time;
					int processorsInUse = jobAttributesList[jobNumber].processor_req;
					int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

					int availAlapProcessors = localProcessors - jobAttributesList[GLOBAL_preScheduleList->jobNumber].processor_req;
					int futureRelease = 0;
					if (GLOBAL_preScheduleList->next != NULL) {
						genericBackupNode *future_preScheduleList = GLOBAL_preScheduleList->next;
						futureRelease = future_preScheduleList->data;
					}
					// condition 3
					if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors &&
						(futureRelease == 0 || (job_release_time + jobAttributesList[jobNumber].execution_time) <= futureRelease))
					{
						localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
						processorsDistList = clean_list(processorsDistList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = job_release_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 3 AEAP: Condition-3 The Job:%d scheduled\n", jobNumber);
							printf("Mode 3 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						queue_job_execution(processorsInUse, processor_release_time, presentTime,
							schedule_method, jobNumber, processorsAllocatedList);
						job_queue_handler(processorsInUse, job_release_time, presentTime,
							schedule_method, jobNumber, jobScheduledQueueList);
						break;
					}
					// condition 2
					else if (processor_release_time <= GLOBAL_preScheduleList->data)
					{
						localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
						processorsDistList = clean_list(processorsDistList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = job_release_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 3 AEAP: Condition-2 The Job:%d scheduled\n", jobNumber);
							printf("Mode 3 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						queue_job_execution(processorsInUse, processor_release_time, presentTime,
							schedule_method, jobNumber, processorsAllocatedList);
						job_queue_handler(processorsInUse, job_release_time, presentTime,
							schedule_method, jobNumber, jobScheduledQueueList);
						break;
					}
					else
					{
						int count = 0;
						scheduledResourceNode *temp1 = *processorsAllocatedList;
						genericBackupNode *temp2 = processorsDistList;
						while (temp2 != NULL) {
							if (count == 0) {
								processors_available = temp2->data;
								temp2 = temp2->next;
							}
							else {
								temp1->processors_allocated = temp2->data;
								temp1 = temp1->next;
								temp2 = temp2->next;
							}
							count++;
						}
						// TBD:: Return Job to CPU - Function to send Job to CPU execution
						jobAttributesList[jobNumber].schedule_hardware = 2;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].completion_time = -1;
						jobAttributesList[jobNumber].scheduled_execution = -1;
						GLOBAL_CPU_JOBS++;
						processorsDistList = clean_list(processorsDistList);
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 3 AEAP: The Job:%d Cannot be scheduled AEAP\n", jobNumber);
							printf("Mode 3 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
						break;
					}
				}
				else
				{
					processorsDistList = insert_node(processorsDistList, localProcessorsAllocatedList->processors_allocated);
					localProcessorsAllocatedList->processors_allocated = 0;
				}
			}
			localProcessorsAllocatedList = localProcessorsAllocatedList->next;
		}
	}
	return processors_available;
}

static int Mode_3_Processors_Unavailable
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
	{
		processors_available = Mode_3_AEAP(jobAttributesList, jobNumber, present_time, processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT && GLOBAL_preScheduleList == NULL)
	{
		processors_available = Mode_3_ALAP(jobAttributesList, jobNumber, present_time, processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	else
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("Mode 3 Processors unavailable:: Job:%d sent for ALAP Advanced execution\n", jobNumber);
		}
		processors_available = Mode_3_ALAP_advanced(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	return processors_available;
}

static int Mode_3_book_keeper
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int processors_available,
	int present_time,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	int processorsInUse = 0, processor_release_time = 0;
	int presentTime = present_time;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode-3 Book Keeper:: Job::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
			jobNumber, jobAttributesList[jobNumber].processor_req, jobAttributesList[jobNumber].execution_time,
			jobAttributesList[jobNumber].deadline, jobAttributesList[jobNumber].latest_schedulable_time);
	}

	// If processors available is greater than the required processors by the jobAttributesList
	if (jobAttributesList[jobNumber].processor_req <= processors_available)
	{
		if (GLOBAL_preScheduleList == NULL)
		{
			// Processors needed lesser than the ALAP limit
			if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
			{
				if (jobAttributesList[jobNumber].execution_time + presentTime <= jobAttributesList[jobNumber].deadline)
				{
					processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
					processorsInUse = jobAttributesList[jobNumber].processor_req;
					processor_release_time = jobAttributesList[jobNumber].execution_time + presentTime;
					schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
					// Job call for the GPU to handle the given Jobs and number of blocks//
					queue_job_execution(processorsInUse, processor_release_time, presentTime, schedule_method, jobNumber, processorsAllocatedList);
					jobAttributesList[jobNumber].schedule_hardware = 1;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].scheduled_execution = presentTime;
					jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + presentTime;
					GLOBAL_GPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode-3 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
						printf("Mode-3 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
						printf("Mode-3 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
				}
			}
			else {
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 3 Book Keeper:: Job:%d is compute intensive, sent for ALAP execution\n", jobNumber);
				}
				processors_available = Mode_3_ALAP(jobAttributesList, jobNumber, present_time,
					processors_available, processorsAllocatedList, jobScheduledQueueList);
			}
		}
		else
		{
			// Processors needed lesser than the limit
			if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
			{
				int availAlapProcessors = processors_available - jobAttributesList[GLOBAL_preScheduleList->jobNumber].processor_req;
				int futureRelease = 0;
				if (GLOBAL_preScheduleList->next != NULL) {
					genericBackupNode *future_preScheduleList = GLOBAL_preScheduleList->next;
					futureRelease = future_preScheduleList->data;
				}
				// condition 1
				if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors &&
					(futureRelease == 0 || (presentTime + jobAttributesList[jobNumber].execution_time) <= futureRelease))
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode-3 Book Keeper:: Job:%d SATISFIED CONDITION 1\n", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + presentTime <= jobAttributesList[jobNumber].deadline)
					{

						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorsInUse = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + presentTime;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						queue_job_execution(processorsInUse, processor_release_time, presentTime, schedule_method, jobNumber, processorsAllocatedList);
						// TBD:: Job call for the GPU to handle the given Jobs and number of blocks//
						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = presentTime;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + presentTime;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode-3 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
							printf("Mode-3 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Mode-3 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				// condition 2
				else if ((presentTime + jobAttributesList[jobNumber].execution_time) <= GLOBAL_preScheduleList->data)
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode-3 Book Keeper:: Job:%d SATISFIED CONDITION 2\n", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + presentTime <= jobAttributesList[jobNumber].deadline)
					{

						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorsInUse = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + presentTime;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						queue_job_execution(processorsInUse, processor_release_time, presentTime, schedule_method, jobNumber, processorsAllocatedList);
						// TBD:: Job call for the GPU to handle the given Jobs and number of blocks//
						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = presentTime;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + presentTime;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode-3 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
							printf("Mode-3 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Mode-3 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
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
						printf("Mode-3 Book Keeper:: Job-%d REJECTED ALAP job is set\n", jobNumber);
						printf("Mode-3 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
				}
			}
			else {
				processors_available = Mode_3_ALAP_advanced(jobAttributesList, jobNumber, present_time,
					processors_available, processorsAllocatedList, jobScheduledQueueList);
			}
		}
	}
	else
	{
		// Schedule the jobAttributesList to be released in a future time
		processors_available = Mode_3_Processors_Unavailable(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	return processors_available;
}

/**********************************************************************************************************
RTGS Mode 3 - AEAP with As Late As Possible mode->AEAP/ALAP
***********************************************************************************************************/
int RTGS_mode_3(char *jobsListFileName, char *releaseTimeFilename)
{
	jobAttributes jobAttributesList[MAX_JOBS] = {{0}};
	jobReleaseInfo releaseTimeInfo[MAX_JOBS] = {{0}};
	scheduledResourceNode *processorsAllocatedList = NULL;
	scheduledResourceNode *jobScheduledQueueList = NULL;		//Job queued for future executions

	// global variable initialize
	GLOBAL_GPU_JOBS = 0;
	GLOBAL_CPU_JOBS = 0;
	GLOBAL_preScheduleList = NULL;

	int processorsAvailable = MAX_GPU_PROCESSOR;
	int jobNumber = 0;

	int kernelMax = get_job_information(jobAttributesList, jobsListFileName);
	if (kernelMax <= RTGS_FAILURE) { return  RTGS_FAILURE; }
	int maxReleases = get_job_release_times(releaseTimeInfo, releaseTimeFilename);
	if (maxReleases <= RTGS_FAILURE) { return  RTGS_FAILURE; }

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("\n**************** The GPU Scheduler will Schedule %d Jobs ****************\n", kernelMax);
	}

	int numReleases = 0;
	for (int present_time = 0; present_time < MAX_RUN_TIME; present_time++)
	{
		// Freeing-up processors
		processorsAvailable = Retrieve_processors(present_time, processorsAvailable, &processorsAllocatedList);
		if (processorsAvailable < 0) { printf("Retrieve_processors ERROR - PresentTime:%d Processors Available:%d\n",present_time, processorsAvailable); return RTGS_ERROR_NOT_IMPLEMENTED; }
		processorsAvailable = Dispatch_queued_kernels(present_time, processorsAvailable, &jobScheduledQueueList, &processorsAllocatedList);
		if (processorsAvailable < 0) { printf("Dispatch_queued_kernels ERROR - PresentTime:%d Processors Available:%d\n",present_time, processorsAvailable); return RTGS_ERROR_NOT_IMPLEMENTED; }

		if (releaseTimeInfo[numReleases].release_time == present_time) {

			if (releaseTimeInfo[numReleases].num_job_released == 1)
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("\nRTGS Mode 3 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 3 -- Job-%d Released\n", jobNumber);
				}
				jobAttributesList[jobNumber].release_time = present_time;
				// handling the released jobAttributesList by the book-keeper
				int64_t start_t = RTGS_GetClockCounter();
				processorsAvailable = Mode_3_book_keeper(jobAttributesList, jobNumber, processorsAvailable, present_time,
					&processorsAllocatedList, &jobScheduledQueueList);
				int64_t end_t = RTGS_GetClockCounter();
				int64_t freq = RTGS_GetClockFrequency();
				float factor = 1000.0f / (float)freq; // to convert clock counter to ms
				float SchedulerOverhead = (float)((end_t - start_t) * factor);
				jobAttributesList[jobNumber].schedule_overhead = SchedulerOverhead;
				jobNumber++;
			}
			else if (releaseTimeInfo[numReleases].num_job_released == 2)
			{
				int k1 = jobNumber; jobNumber++;
				int k2 = jobNumber; jobNumber++;
				jobAttributesList[k1].release_time = present_time;
				jobAttributesList[k2].release_time = present_time;

				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("\nRTGS Mode 3 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 3 -- Job-%d Released\n", k1);
					printf("RTGS Mode 3 -- Job-%d Released\n", k2);
				}
				if (jobAttributesList[k1].deadline <= jobAttributesList[k2].deadline)
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_3_book_keeper(jobAttributesList, k1, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_3_book_keeper(jobAttributesList, k2, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
				}
				else
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_3_book_keeper(jobAttributesList, k2, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_3_book_keeper(jobAttributesList, k1, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
				}
			}
			else { printf("RTGS Mode 3 ERROR --  RTGS_ERROR_NOT_IMPLEMENTED\n"); return RTGS_ERROR_NOT_IMPLEMENTED; }

			numReleases++;
			if (numReleases > maxReleases) {
				printf("RTGS Mode 3 ERROR --  Job Release Time exceded Max Releases\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	if (maxReleases != 0) {

		if (GLOBAL_RTGS_DEBUG_MSG) {
			printf("\n******* Scheduler Mode 3 *******\n");
			printf("Processors Available -- %d\n", processorsAvailable);
			printf("Total Jobs Scheduled -- %d\n", kernelMax);
			printf("	GPU Scheduled Jobs    -- %d\n", GLOBAL_GPU_JOBS);
			printf("	Jobs Sent Back To CPU -- %d\n", GLOBAL_CPU_JOBS);
		}

		if (RTGS_PrintScheduleSummary(3, kernelMax, jobAttributesList)) {
			printf("\nSummary Failed\n");
		}

		if ((kernelMax != (GLOBAL_GPU_JOBS + GLOBAL_CPU_JOBS)) || processorsAvailable != MAX_GPU_PROCESSOR) {
			return RTGS_FAILURE;
		}

		for (int j = 0; j <= kernelMax; j++) {
			jobAttributesList[j].processor_req = jobAttributesList[j].deadline = jobAttributesList[j].execution_time = jobAttributesList[j].latest_schedulable_time = 0;
		}
		kernelMax = 0; maxReleases = 0; jobNumber = 0; GLOBAL_GPU_JOBS = 0; GLOBAL_CPU_JOBS = 0;
	}

	if (processorsAllocatedList || GLOBAL_preScheduleList) {
		printf("\nERROR -- processorsAllocatedList/GLOBAL_preScheduleList Failed %d/%d\n", processorsAllocatedList == NULL ? 0 : 1, GLOBAL_preScheduleList == NULL ? 0 : 1);
		return RTGS_FAILURE;
	}

	return RTGS_SUCCESS;
}