/*
 * RTG-mode_4.c
 *      Author: Kiriti Nagesh Gowda
 */
#include "RTGS.h"
/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
static int Mode_4_ALAP_advanced(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int presentTime,
	int processorsAvailable,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	if (GLOBAL_RTGS_DEBUG_MSG > 1)
	{
		printf("Mode 4 ALAP advanced: Job:%d is verified for ALAP advanced scheduling\n", jobNumber);
	}

	int localProcessors = 0, jobReleaseTime = 0, processorReleaseTime = 0, processorsInUse = 0;
	processorsInUse = jobAttributesList[jobNumber].processor_req;
	processorReleaseTime = jobAttributesList[jobNumber].deadline;
	jobReleaseTime = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
	int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

	scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;
	genericBackupNode *lastALAPJobScheduled = GLOBAL_preScheduleList;

	// fail case return
	if (presentTime >= jobReleaseTime)
	{
		jobAttributesList[jobNumber].schedule_hardware = 2;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].completion_time = -1;
		jobAttributesList[jobNumber].scheduled_execution = -1;
		GLOBAL_CPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1)
		{
			printf("Mode 4 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 4 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
		return processorsAvailable;
	}

	int processorsQueued = 0;
	int processorsQueuedRelease = 0;
	while (lastALAPJobScheduled != NULL)
	{
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
	while (localProcessorsAllocatedList != NULL)
	{
		processorsRetrived += localProcessorsAllocatedList->processors_allocated;
		if (processorsReleaseTime < localProcessorsAllocatedList->processor_release_time)
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
		if (processorsAvailable >= jobAttributesList[jobNumber].processor_req)
		{
			processorsQueued = 0;
			while (lastALAPJobScheduled != NULL)
			{
				processorsQueued += lastALAPJobScheduled->processors_allocated;
				if (processorsQueued >= jobAttributesList[jobNumber].processor_req)
				{
					lastALAPJobScheduled->processors_allocated = processorsQueued - jobAttributesList[jobNumber].processor_req;
					break;
				}
				else
				{
					lastALAPJobScheduled->processors_allocated = 0;
				}
				lastALAPJobScheduled = lastALAPJobScheduled->next;
			}
			lastALAPJobScheduled = GLOBAL_preScheduleList;

			jobAttributesList[jobNumber].schedule_hardware = 1;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].scheduled_execution = jobReleaseTime;
			jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + jobReleaseTime;
			GLOBAL_GPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1)
			{
				printf("Mode 4 ALAP advanced: Condition 3 pass, The Job:%d scheduled\n", jobNumber);
				printf("Mode 4 ALAP advanced: Condition 3 pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
			GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, jobReleaseTime, processorReleaseTime,
																 processorsInUse, jobNumber);
			job_queue_handler(processorsInUse, jobReleaseTime, processorReleaseTime,
							  schedule_method, jobNumber, jobScheduledQueueList);
			return processorsAvailable;
		}
		// condition 4
		else if (processorsQueued >= jobAttributesList[jobNumber].processor_req)
		{
			processorsQueued = 0;
			while (lastALAPJobScheduled != NULL)
			{
				processorsQueued += lastALAPJobScheduled->processors_allocated;
				if (processorsQueued >= jobAttributesList[jobNumber].processor_req)
				{
					lastALAPJobScheduled->processors_allocated = processorsQueued - jobAttributesList[jobNumber].processor_req;
					break;
				}
				else
				{
					lastALAPJobScheduled->processors_allocated = 0;
				}
				lastALAPJobScheduled = lastALAPJobScheduled->next;
			}
			lastALAPJobScheduled = GLOBAL_preScheduleList;

			jobAttributesList[jobNumber].schedule_hardware = 1;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].scheduled_execution = jobReleaseTime;
			jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + jobReleaseTime;
			GLOBAL_GPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1)
			{
				printf("Mode 4 ALAP advanced: Condition 4 pass, The Job:%d scheduled\n", jobNumber);
				printf("Mode 4 ALAP advanced: Condition 4 pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
			GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, jobReleaseTime, processorReleaseTime,
																 processorsInUse, jobNumber);
			job_queue_handler(processorsInUse, jobReleaseTime, processorReleaseTime,
							  schedule_method, jobNumber, jobScheduledQueueList);
			return processorsAvailable;
		}
		else
		{
			localProcessors = GLOBAL_MAX_PROCESSORS;
			if (jobAttributesList[jobNumber].processor_req <= localProcessors)
			{
				processorsQueued = 0;
				while (lastALAPJobScheduled != NULL)
				{
					processorsQueued += lastALAPJobScheduled->processors_allocated;
					if (processorsQueued >= jobAttributesList[jobNumber].processor_req)
					{
						lastALAPJobScheduled->processors_allocated = processorsQueued - jobAttributesList[jobNumber].processor_req;
						break;
					}
					else
					{
						lastALAPJobScheduled->processors_allocated = 0;
					}
					lastALAPJobScheduled = lastALAPJobScheduled->next;
				}
				lastALAPJobScheduled = GLOBAL_preScheduleList;

				jobAttributesList[jobNumber].schedule_hardware = 1;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].scheduled_execution = jobReleaseTime;
				jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + jobReleaseTime;
				GLOBAL_GPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("Mode 4 ALAP advanced: Condition 4 b pass, The Job:%d scheduled\n", jobNumber);
					printf("Mode 4 ALAP advanced: Condition 4 b pass, Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
				}
				GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, jobReleaseTime, processorReleaseTime,
																	 processorsInUse, jobNumber);
				job_queue_handler(processorsInUse, jobReleaseTime, processorReleaseTime,
								  schedule_method, jobNumber, jobScheduledQueueList);
				return processorsAvailable;
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
		if (GLOBAL_RTGS_DEBUG_MSG > 1)
		{
			printf("Mode 4 ALAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 4 ALAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
	}
	return processorsAvailable;
}

static int Mode_4_ALAP(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int presentTime,
	int processorsAvailable,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	int localProcessors = 0, jobReleaseTime, processorReleaseTime = 0, processorsAllocated = 0;
	genericBackupNode *processorsDistList = NULL;
	processorsAllocated = jobAttributesList[jobNumber].processor_req;
	processorReleaseTime = jobAttributesList[jobNumber].deadline;
	jobReleaseTime = processorReleaseTime - jobAttributesList[jobNumber].execution_time;
	localProcessors = processorsAvailable;

	// fail case return
	if (presentTime >= jobReleaseTime)
	{
		jobAttributesList[jobNumber].schedule_hardware = 2;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].completion_time = -1;
		jobAttributesList[jobNumber].scheduled_execution = -1;
		GLOBAL_CPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1)
		{
			printf("Mode 4 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 4 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
		return processorsAvailable;
	}

	if (localProcessors >= jobAttributesList[jobNumber].processor_req)
	{
		int processorsInUse = jobAttributesList[jobNumber].processor_req;
		int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

		jobAttributesList[jobNumber].schedule_hardware = 1;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].scheduled_execution = jobReleaseTime;
		jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + jobReleaseTime;
		GLOBAL_GPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1)
		{
			printf("Mode 4 ALAP: The Job:%d scheduled\n", jobNumber);
			printf("Mode 4 ALAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
		}
		GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, jobReleaseTime,
															 processorReleaseTime, processorsAllocated, jobNumber);
		job_queue_handler(processorsInUse, jobReleaseTime, processorReleaseTime,
						  schedule_method, jobNumber, jobScheduledQueueList);
		return processorsAvailable;
	}

	processorsDistList = insert_preScheduledJob_list(processorsDistList, jobReleaseTime, processorReleaseTime, processorsAvailable, jobNumber);
	scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;

	while (localProcessorsAllocatedList != NULL)
	{
		if ((localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
		{
			int count = 0;
			scheduledResourceNode *temp1 = *processorsAllocatedList;
			genericBackupNode *temp2 = processorsDistList;

			while (temp2 != NULL)
			{
				if (count == 0)
					temp2 = temp2->next;
				else
				{
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
			if (GLOBAL_RTGS_DEBUG_MSG > 1)
			{
				printf("Mode 4 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
				printf("Mode 4 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
			}
			break;
		}
		else
		{
			localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
			if (localProcessors >= jobAttributesList[jobNumber].processor_req)
			{
				localProcessorsAllocatedList->job_release_time = jobReleaseTime;
				processorsDistList = clean_list(processorsDistList);
				int processorsInUse = jobAttributesList[jobNumber].processor_req;
				int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

				jobAttributesList[jobNumber].schedule_hardware = 1;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].scheduled_execution = jobReleaseTime;
				jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + jobReleaseTime;
				GLOBAL_GPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("Mode 4 ALAP: The Job:%d scheduled\n", jobNumber);
					printf("Mode 4 ALAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
				}
				GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, jobReleaseTime, processorReleaseTime,
																	 processorsAllocated, jobNumber);
				job_queue_handler(processorsInUse, jobReleaseTime, processorReleaseTime,
								  schedule_method, jobNumber, jobScheduledQueueList);
				break;
			}
			else
			{
				processorsDistList = insert_preScheduledJob_list(processorsDistList, jobReleaseTime, processorReleaseTime, localProcessorsAllocatedList->processors_allocated, localProcessorsAllocatedList->jobNumber);
				localProcessorsAllocatedList->job_release_time = jobReleaseTime;
			}
		}
		localProcessorsAllocatedList = localProcessorsAllocatedList->next;
	}
	return processorsAvailable;
}

static int Mode_4_AEAP(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int presentTime,
	int processorsAvailable,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	int localProcessors = 0, jobReleaseTime;
	genericBackupNode *processorsDistList = NULL;
	scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;

	localProcessors = processorsAvailable;
	processorsDistList = insert_node(processorsDistList, processorsAvailable);
	processorsAvailable = 0;

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
					if (count == 0)
					{
						processorsAvailable = temp2->data;
						temp2 = temp2->next;
					}
					else
					{
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
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("Mode 4 AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				break;
			}
			else
			{
				localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
				if (localProcessors >= jobAttributesList[jobNumber].processor_req)
				{
					localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
					jobReleaseTime = localProcessorsAllocatedList->processor_release_time;
					processorsDistList = clean_list(processorsDistList);
					int processorsInUse = jobAttributesList[jobNumber].processor_req;
					int processorReleaseTime = jobReleaseTime + jobAttributesList[jobNumber].execution_time;
					int presentTimeL = presentTime;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

					jobAttributesList[jobNumber].schedule_hardware = 1;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].scheduled_execution = jobReleaseTime;
					jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + jobReleaseTime;
					GLOBAL_GPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Mode 4 AEAP: The Job:%d scheduled\n", jobNumber);
						printf("Mode 4 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					queue_job_execution(processorsInUse, processorReleaseTime, presentTimeL,
										schedule_method, jobNumber, processorsAllocatedList);
					job_queue_handler(processorsInUse, jobReleaseTime, presentTimeL,
									  schedule_method, jobNumber, jobScheduledQueueList);

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
				while (temp2 != NULL)
				{
					if (count == 0)
					{
						processorsAvailable = temp2->data;
						temp2 = temp2->next;
					}
					else
					{
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
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("Mode 4 AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				break;
			}
			else
			{
				localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
				if (localProcessors >= jobAttributesList[jobNumber].processor_req)
				{
					int availAlapProcessors = localProcessors - jobAttributesList[GLOBAL_preScheduleList->jobNumber].processor_req;
					int futureRelease = 0;
					if (GLOBAL_preScheduleList->next != NULL)
					{
						genericBackupNode *future_preScheduleList = GLOBAL_preScheduleList->next;
						futureRelease = future_preScheduleList->data;
					}
					// condition 1
					if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors &&
						localProcessorsAllocatedList->processor_release_time <= GLOBAL_preScheduleList->data &&
						(futureRelease == 0 || (localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) <= futureRelease))
					{
						localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
						processorsDistList = clean_list(processorsDistList);

						jobReleaseTime = localProcessorsAllocatedList->processor_release_time;
						int processorsInUse = jobAttributesList[jobNumber].processor_req;
						int processorReleaseTime = jobReleaseTime + jobAttributesList[jobNumber].execution_time;
						int presentTimeL = presentTime;
						int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = jobReleaseTime;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + jobReleaseTime;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Mode 4 AEAP: Condition-1 The Job:%d scheduled\n", jobNumber);
							printf("Mode 4 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						queue_job_execution(processorsInUse, processorReleaseTime, presentTimeL,
											schedule_method, jobNumber, processorsAllocatedList);
						job_queue_handler(processorsInUse, jobReleaseTime, presentTimeL,
										  schedule_method, jobNumber, jobScheduledQueueList);
						break;
					}
					// condition 2
					else if (localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time <= GLOBAL_preScheduleList->data)
					{
						localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
						processorsDistList = clean_list(processorsDistList);

						jobReleaseTime = localProcessorsAllocatedList->processor_release_time;
						int processorsInUse = jobAttributesList[jobNumber].processor_req;
						int processorReleaseTime = jobReleaseTime + jobAttributesList[jobNumber].execution_time;
						int presentTimeL = presentTime;
						int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = jobReleaseTime;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + jobReleaseTime;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Mode 4 AEAP: Condition-2 The Job:%d scheduled\n", jobNumber);
							printf("Mode 4 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						queue_job_execution(processorsInUse, processorReleaseTime, presentTimeL,
											schedule_method, jobNumber, processorsAllocatedList);
						job_queue_handler(processorsInUse, jobReleaseTime, presentTimeL,
										  schedule_method, jobNumber, jobScheduledQueueList);
						break;
					}
					else
					{
						int count = 0;
						scheduledResourceNode *temp1 = *processorsAllocatedList;
						genericBackupNode *temp2 = processorsDistList;
						while (temp2 != NULL)
						{
							if (count == 0)
							{
								processorsAvailable = temp2->data;
								temp2 = temp2->next;
							}
							else
							{
								temp1->processors_allocated = temp2->data;
								temp1 = temp1->next;
								temp2 = temp2->next;
							}
							count++;
						}
						processorsDistList = clean_list(processorsDistList);

						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Mode 4 AEAP: AEAP with ALAP, Backup processors reloaded\n");
						}
						// TBD:: Schedule after AEAP Advanced NEEDED
						processorsAvailable = AEAP_advanced(jobAttributesList, jobNumber, presentTime,
															processorsAvailable, processorsAllocatedList, jobScheduledQueueList);
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
	return processorsAvailable;
}

static int Mode_4_Processors_unavailable(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int presentTime,
	int processorsAvailable,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	if (jobAttributesList[jobNumber].processor_req < GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2)
		{
			printf("Mode 4 GCUs unavailable:: Job:%d sent for AEAP execution\n", jobNumber);
		}
		processorsAvailable = Mode_4_AEAP(jobAttributesList, jobNumber, presentTime,
										  processorsAvailable, processorsAllocatedList, jobScheduledQueueList);
	}
	else if (jobAttributesList[jobNumber].processor_req >= GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT && GLOBAL_preScheduleList == NULL)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2)
		{
			printf("Mode 4 GCUs unavailable:: Job:%d sent for ALAP execution\n", jobNumber);
		}
		processorsAvailable = Mode_4_ALAP(jobAttributesList, jobNumber, presentTime,
										  processorsAvailable, processorsAllocatedList, jobScheduledQueueList);
	}
	else
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2)
		{
			printf("Mode 4 GCUs unavailable:: Job:%d sent for ALAP Advanced execution\n", jobNumber);
		}
		processorsAvailable = Mode_4_ALAP_advanced(jobAttributesList, jobNumber, presentTime,
												   processorsAvailable, processorsAllocatedList, jobScheduledQueueList);
	}

	return processorsAvailable;
}

static int Mode_4_book_keeper(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int processorsAvailable,
	int presentTime,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	int processorsInUse = 0, processorReleaseTime = 0;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	int presentTimeL = presentTime;

	if (GLOBAL_RTGS_DEBUG_MSG > 1)
	{
		printf("Mode 4 Book Keeper:: Job::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
			   jobNumber, jobAttributesList[jobNumber].processor_req, jobAttributesList[jobNumber].execution_time,
			   jobAttributesList[jobNumber].deadline, jobAttributesList[jobNumber].latest_schedulable_time);
	}

	// If processors available is greater than the required processors by the jobAttributesList
	if (jobAttributesList[jobNumber].processor_req <= processorsAvailable)
	{
		if (GLOBAL_preScheduleList == NULL)
		{
			// GCUs needed lesser than the ALAP limit
			if (jobAttributesList[jobNumber].processor_req < GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT)
			{
				if (jobAttributesList[jobNumber].execution_time + presentTime <= jobAttributesList[jobNumber].deadline)
				{

					processorsAvailable = processorsAvailable - jobAttributesList[jobNumber].processor_req;
					processorsInUse = jobAttributesList[jobNumber].processor_req;
					processorReleaseTime = jobAttributesList[jobNumber].execution_time + presentTime;
					schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
					// Job call for the GPU to handle the given Jobs and number of blocks
					queue_job_execution(processorsInUse, processorReleaseTime, presentTime,
										schedule_method, jobNumber, processorsAllocatedList);

					jobAttributesList[jobNumber].schedule_hardware = 1;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].scheduled_execution = presentTime;
					jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + presentTime;
					GLOBAL_GPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Mode 4 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
				}
				else
				{
					jobAttributesList[jobNumber].schedule_hardware = 2;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].completion_time = -1;
					jobAttributesList[jobNumber].scheduled_execution = -1;
					GLOBAL_CPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Mode 4 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
						printf("Mode 4 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
				}
			}
			else
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("Mode 4 Book Keeper:: Job:%d is compute intensive, sent for ALAP execution\n", jobNumber);
				}
				processorsAvailable = Mode_4_ALAP(jobAttributesList, jobNumber, presentTime,
												  processorsAvailable, processorsAllocatedList, jobScheduledQueueList);
			}
		}
		else
		{
			int availAlapProcessors = processorsAvailable - jobAttributesList[GLOBAL_preScheduleList->jobNumber].processor_req;
			int futureRelease = 0;
			if (GLOBAL_preScheduleList->next != NULL)
			{
				genericBackupNode *future_preScheduleList = GLOBAL_preScheduleList->next;
				futureRelease = future_preScheduleList->data;
			}
			// GCUs needed lesser than the limit
			if (jobAttributesList[jobNumber].processor_req < GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT)
			{
				// Condition 1
				if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors &&
					(futureRelease == 0 || (presentTimeL + jobAttributesList[jobNumber].execution_time) <= futureRelease))
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Mode 4 Book Keeper::  ALAP is set, Job:%d SATISFIED CONDITION 1\n", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + presentTime <= jobAttributesList[jobNumber].deadline)
					{
						processorsAvailable = processorsAvailable - jobAttributesList[jobNumber].processor_req;
						processorsInUse = jobAttributesList[jobNumber].processor_req;
						processorReleaseTime = jobAttributesList[jobNumber].execution_time + presentTime;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						// Job call for the GPU to handle the given Jobs and number of blocks
						queue_job_execution(processorsInUse, processorReleaseTime, presentTime,
											schedule_method, jobNumber, processorsAllocatedList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = presentTime;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + presentTime;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Mode 4 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
					}
					else
					{
						jobAttributesList[jobNumber].schedule_hardware = 2;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].completion_time = -1;
						jobAttributesList[jobNumber].scheduled_execution = -1;
						GLOBAL_CPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Mode 4 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Mode 4 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				// Condition 2
				else if ((presentTimeL + jobAttributesList[jobNumber].execution_time) <= GLOBAL_preScheduleList->data)
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Mode 4 Book Keeper::  ALAP is set, Job:%d SATISFIED CONDITION 2\n", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + presentTime <= jobAttributesList[jobNumber].deadline)
					{
						processorsAvailable = processorsAvailable - jobAttributesList[jobNumber].processor_req;
						processorsInUse = jobAttributesList[jobNumber].processor_req;
						processorReleaseTime = jobAttributesList[jobNumber].execution_time + presentTime;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						// Job call for the GPU to handle the given Jobs and number of blocks
						queue_job_execution(processorsInUse, processorReleaseTime, presentTime,
											schedule_method, jobNumber, processorsAllocatedList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = presentTime;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + presentTime;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Mode 4 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
					}
					else
					{
						jobAttributesList[jobNumber].schedule_hardware = 2;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].completion_time = -1;
						jobAttributesList[jobNumber].scheduled_execution = -1;
						GLOBAL_CPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Mode 4 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Mode 4 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				else
				{
					processorsAvailable = AEAP_advanced(jobAttributesList, jobNumber, presentTime,
														processorsAvailable, processorsAllocatedList, jobScheduledQueueList);
				}
			}
			else
			{
				processorsAvailable = Mode_4_ALAP_advanced(jobAttributesList, jobNumber, presentTime,
														   processorsAvailable, processorsAllocatedList, jobScheduledQueueList);
			}
		}
	}
	else
	{
		// Schedule the jobAttributesList to be released in a future time
		processorsAvailable = Mode_4_Processors_unavailable(jobAttributesList, jobNumber, presentTime,
															processorsAvailable, processorsAllocatedList, jobScheduledQueueList);
	}

	return processorsAvailable;
}

/***********************************************************************************************************
MODE 4 Enhanced Method FUNCTIONS
**********************************************************************************************************/
static int mode_4_enhanced_method(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int processorsAvailable,
	int presentTime,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	if (GLOBAL_RTGS_DEBUG_MSG > 2)
	{
		printf("Mode-4 Enhanced Method\n");
	}

	if (GLOBAL_RTGS_METHOD >= RTGS_METHOD_ENHANCED && jobAttributesList[jobNumber].processor_req_m > 0)
	{
		// schedule with medium processor option
		GLOBAL_CPU_JOBS--;
		jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_m;
		jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_m;
		jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
		processorsAvailable = Mode_4_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
												 processorsAllocatedList, jobScheduledQueueList);
		if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
		{
			jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED;
			jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_MEDIUM;
		}
		else if (jobAttributesList[jobNumber].processor_req_l > 0)
		{
			// schedule with low processor option
			GLOBAL_CPU_JOBS--;
			jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_l;
			jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_l;
			jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
			processorsAvailable = Mode_4_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
													 processorsAllocatedList, jobScheduledQueueList);
			if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
			{
				jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED;
				jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_LOW;
			}
		}
	}
	if (GLOBAL_RTGS_METHOD == RTGS_METHOD_ENHANCED_VQS && jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_CPU)
	{
		if (jobAttributesList[jobNumber].deadline_flexibility != 0)
		{
			float deadlineFlexibility = (float)(jobAttributesList[jobNumber].deadline_flexibility);
			deadlineFlexibility = deadlineFlexibility / 100; // percentage value
			int deadlineExtended = ceil((float)((jobAttributesList[jobNumber].deadline - presentTime) * deadlineFlexibility));
			jobAttributesList[jobNumber].deadline = jobAttributesList[jobNumber].deadline + deadlineExtended;
			// schedule with high processor option with deadline flexibilty
			GLOBAL_CPU_JOBS--;
			jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_h;
			jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_h;
			jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
			processorsAvailable = Mode_4_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
													 processorsAllocatedList, jobScheduledQueueList);
			if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
			{
				jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED_VQS;
				jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_HIGH;
			}
			else if (jobAttributesList[jobNumber].processor_req_m > 0)
			{
				// schedule with medium processor option with deadline flexibilty
				GLOBAL_CPU_JOBS--;
				jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_m;
				jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_m;
				jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
				processorsAvailable = Mode_4_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
														 processorsAllocatedList, jobScheduledQueueList);
				if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
				{
					jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED_VQS;
					jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_MEDIUM;
				}
				else if (jobAttributesList[jobNumber].processor_req_l > 0)
				{
					// schedule with low processor option with deadline flexibilty
					GLOBAL_CPU_JOBS--;
					jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_l;
					jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_l;
					jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
															 processorsAllocatedList, jobScheduledQueueList);
					if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
					{
						jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED_VQS;
						jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_LOW;
					}
				}
			}
		}
		else
		{
			if (GLOBAL_RTGS_DEBUG_MSG > 1)
			{
				printf("Jod:%d -- Schedule Method:4 Can not be used as deadline is not flexible\n", jobNumber);
			}
		}
	}
	return processorsAvailable;
}

/**********************************************************************************************************
RTGS Mode 4 - - AEAP/ALAP Bin Packer mode->AEAP/ALAP Pack
***********************************************************************************************************/
int RTGS_mode_4(char *jobsListFileName, char *releaseTimeFilename)
{

	jobAttributes jobAttributesList[MAX_JOBS] = {{0}};
	jobReleaseInfo releaseTimeInfo[MAX_JOBS] = {{0}};
	scheduledResourceNode *processorsAllocatedList = NULL;
	scheduledResourceNode *jobScheduledQueueList = NULL;

	// global variable initialize
	GLOBAL_GPU_JOBS = 0;
	GLOBAL_CPU_JOBS = 0;
	GLOBAL_preScheduleList = NULL;

	int processorsAvailable = GLOBAL_MAX_PROCESSORS;
	int jobNumber = 0;

	int kernelMax = get_job_information(jobAttributesList, jobsListFileName);
	if (kernelMax <= RTGS_FAILURE)
	{
		return RTGS_FAILURE;
	}
	int maxReleases = get_job_release_times(releaseTimeInfo, releaseTimeFilename);
	if (maxReleases <= RTGS_FAILURE)
	{
		return RTGS_FAILURE;
	}

	if (GLOBAL_RTGS_DEBUG_MSG > 1)
	{
		printf("\n**************** The GPU Scheduler will Schedule %d Jobs ****************\n", kernelMax);
	}

	int numReleases = 0;
	for (int presentTime = 0; presentTime < MAX_RUN_TIME; presentTime++)
	{
		// Freeing-up processors
		processorsAvailable = Retrieve_processors(presentTime, processorsAvailable, &processorsAllocatedList);
		if (processorsAvailable < 0)
		{
			printf("Retrieve_processors ERROR - Time:%d GCUs Available:%d\n", presentTime, processorsAvailable);
			return RTGS_ERROR_NOT_IMPLEMENTED;
		}
		processorsAvailable = Dispatch_queued_kernels(presentTime, processorsAvailable, &jobScheduledQueueList, &processorsAllocatedList);
		if (processorsAvailable < 0)
		{
			printf("Dispatch_queued_kernels ERROR - Time:%d GCUs Available:%d\n", presentTime, processorsAvailable);
			return RTGS_ERROR_NOT_IMPLEMENTED;
		}

		if (releaseTimeInfo[numReleases].release_time == presentTime)
		{

			if (releaseTimeInfo[numReleases].num_job_released == 1)
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("\nRTGS Mode 4 -- Total GCUs Available at time %d = %d\n", presentTime, processorsAvailable);
					printf("RTGS Mode 4 -- Job-%d Released\n", jobNumber);
				}
				jobAttributesList[jobNumber].release_time = presentTime;
				// handling the released jobAttributesList by the book-keeper
				int64_t start_t = RTGS_GetClockCounter();
				processorsAvailable = Mode_4_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
														 &processorsAllocatedList, &jobScheduledQueueList);
				int64_t end_t = RTGS_GetClockCounter();
				int64_t freq = RTGS_GetClockFrequency();
				float factor = 1000.0f / (float)freq; // to convert clock counter to ms
				float SchedulerOverhead = (float)((end_t - start_t) * factor);
				jobAttributesList[jobNumber].schedule_overhead = SchedulerOverhead;
				// Enhanced Scheduler Methods
				if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_CPU)
				{
					int64_t start_t_e = RTGS_GetClockCounter();
					processorsAvailable = mode_4_enhanced_method(jobAttributesList, jobNumber, processorsAvailable, presentTime,
																 &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t_e = RTGS_GetClockCounter();
					float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
					jobAttributesList[jobNumber].schedule_overhead = jobAttributesList[jobNumber].schedule_overhead + SchedulerOverhead_e;
				}
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", jobNumber, jobAttributesList[jobNumber].rtgs_method, jobAttributesList[jobNumber].rtgs_level);
				}
				jobNumber++;
			}
			else if (releaseTimeInfo[numReleases].num_job_released == 2)
			{
				int k1 = jobNumber;
				jobNumber++;
				int k2 = jobNumber;
				jobNumber++;
				jobAttributesList[k1].release_time = presentTime;
				jobAttributesList[k2].release_time = presentTime;

				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("\nRTGS Mode 4 -- Total GCUs Available at time %d = %d\n", presentTime, processorsAvailable);
					printf("RTGS Mode 4 -- Job-%d Released\n", k1);
					printf("RTGS Mode 4 -- Job-%d Released\n", k2);
				}
				if (jobAttributesList[k1].deadline <= jobAttributesList[k2].deadline)
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k1, processorsAvailable, presentTime, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
					// Enhanced Scheduler Methods
					if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[k1].schedule_hardware == RTGS_HARDWARE_CPU)
					{
						int64_t start_t_e = RTGS_GetClockCounter();
						processorsAvailable = mode_4_enhanced_method(jobAttributesList, k1, processorsAvailable, presentTime,
																	 &processorsAllocatedList, &jobScheduledQueueList);
						int64_t end_t_e = RTGS_GetClockCounter();
						float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
						jobAttributesList[k1].schedule_overhead = jobAttributesList[k1].schedule_overhead + SchedulerOverhead_e;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", k1, jobAttributesList[k1].rtgs_method, jobAttributesList[k1].rtgs_level);
					}
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k2, processorsAvailable, presentTime, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
					// Enhanced Scheduler Methods
					if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[k2].schedule_hardware == RTGS_HARDWARE_CPU)
					{
						int64_t start_t_e = RTGS_GetClockCounter();
						processorsAvailable = mode_4_enhanced_method(jobAttributesList, k2, processorsAvailable, presentTime,
																	 &processorsAllocatedList, &jobScheduledQueueList);
						int64_t end_t_e = RTGS_GetClockCounter();
						float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
						jobAttributesList[k2].schedule_overhead = jobAttributesList[k2].schedule_overhead + SchedulerOverhead_e;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", k2, jobAttributesList[k2].rtgs_method, jobAttributesList[k2].rtgs_level);
					}
				}
				else
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k2, processorsAvailable, presentTime, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
					// Enhanced Scheduler Methods
					if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[k2].schedule_hardware == RTGS_HARDWARE_CPU)
					{
						int64_t start_t_e = RTGS_GetClockCounter();
						processorsAvailable = mode_4_enhanced_method(jobAttributesList, k2, processorsAvailable, presentTime,
																	 &processorsAllocatedList, &jobScheduledQueueList);
						int64_t end_t_e = RTGS_GetClockCounter();
						float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
						jobAttributesList[k2].schedule_overhead = jobAttributesList[k2].schedule_overhead + SchedulerOverhead_e;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", k2, jobAttributesList[k2].rtgs_method, jobAttributesList[k2].rtgs_level);
					}
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k1, processorsAvailable, presentTime, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
					// Enhanced Scheduler Methods
					if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[k1].schedule_hardware == RTGS_HARDWARE_CPU)
					{
						int64_t start_t_e = RTGS_GetClockCounter();
						processorsAvailable = mode_4_enhanced_method(jobAttributesList, k1, processorsAvailable, presentTime,
																	 &processorsAllocatedList, &jobScheduledQueueList);
						int64_t end_t_e = RTGS_GetClockCounter();
						float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
						jobAttributesList[k1].schedule_overhead = jobAttributesList[k1].schedule_overhead + SchedulerOverhead_e;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", k1, jobAttributesList[k1].rtgs_method, jobAttributesList[k1].rtgs_level);
					}
				}
			}
			else if (releaseTimeInfo[numReleases].num_job_released > 2)
			{
				return RTGS_ERROR_NOT_IMPLEMENTED;
			}

			numReleases++;
			if (numReleases > maxReleases)
			{
				printf("RTGS Mode 4 ERROR --  Job Release Time exceded Max Releases\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	if (maxReleases != 0)
	{
		if (GLOBAL_RTGS_DEBUG_MSG)
		{
			printf("******* Scheduler Results *******\n");
			printf("RTGS Method:%d RTGS Mode:4\n", GLOBAL_RTGS_METHOD);
			printf("GCUs Available -- %d\n", processorsAvailable);
			printf("Total Jobs Scheduled -- %d\n", kernelMax);
			printf("	GPU Scheduled Jobs    -- %d\n", GLOBAL_GPU_JOBS);
			printf("	Jobs Sent Back To CPU -- %d\n", GLOBAL_CPU_JOBS);
		}

		if (RTGS_PrintScheduleSummary(4, kernelMax, jobAttributesList))
		{
			printf("\nSummary Failed\n");
		}

		if ((kernelMax != (GLOBAL_GPU_JOBS + GLOBAL_CPU_JOBS)) || processorsAvailable != GLOBAL_MAX_PROCESSORS)
		{
			return RTGS_FAILURE;
		}

		for (int j = 0; j <= kernelMax; j++)
		{
			jobAttributesList[j].processor_req = jobAttributesList[j].deadline = jobAttributesList[j].execution_time = jobAttributesList[j].latest_schedulable_time = 0;
		}
		kernelMax = 0;
		maxReleases = 0;
		jobNumber = 0;
		GLOBAL_GPU_JOBS = 0;
		GLOBAL_CPU_JOBS = 0;
	}

	if (processorsAllocatedList || GLOBAL_preScheduleList)
	{
		printf("\nERROR -- processorsAllocatedList/GLOBAL_preScheduleList Failed %d/%d\n", processorsAllocatedList == NULL ? 0 : 1, GLOBAL_preScheduleList == NULL ? 0 : 1);
		return RTGS_FAILURE;
	}

	return RTGS_SUCCESS;
}
