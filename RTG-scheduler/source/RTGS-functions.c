/*
* RTGS-functions.c
*      Author: Kiriti Nagesh Gowda
*/

#include "RTGS.h"

/* Book keeping Function, the core function of the scheduler in charge of assigning processors and allocating the future releases */
int Kernel_book_keeper(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int processors_available,
	int present_time,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	PROFILER_START(SRTG, Kernel_book_keeper)
	int processorsInUse = 0, processor_release_time = 0;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	int presentTime = present_time;

	if (GLOBAL_RTGS_DEBUG_MSG > 1)
	{
		printf("Book Keeper:: Job::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
			   jobNumber, jobAttributesList[jobNumber].processor_req, jobAttributesList[jobNumber].execution_time,
			   jobAttributesList[jobNumber].deadline, jobAttributesList[jobNumber].latest_schedulable_time);
	}

	// If processors available is greater than the required processors by the jobAttributesList
	if (jobAttributesList[jobNumber].processor_req <= processors_available)
	{
		if (GLOBAL_preScheduleList == NULL)
		{
			// Processors needed lesser than the ALAP limit
			if (jobAttributesList[jobNumber].processor_req < GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT)
			{
				if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
				{

					processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
					processorsInUse = jobAttributesList[jobNumber].processor_req;
					processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
					schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
					// Job call for the GPU to handle the given Jobs and number of blocks
					queue_job_execution(processorsInUse, processor_release_time, present_time,
										schedule_method, jobNumber, processorsAllocatedList);

					jobAttributesList[jobNumber].schedule_hardware = 1;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].scheduled_execution = present_time;
					jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
					GLOBAL_GPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
						printf("Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
						printf("Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
				}
			}
			else
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("Book Keeper:: Job:%d is compute intensive, sent for ALAP execution\n", jobNumber);
				}
				processors_available = ALAP(jobAttributesList, jobNumber, present_time,
											processors_available, processorsAllocatedList, jobScheduledQueueList);
			}
		}
		else
		{
			int availAlapProcessors = processors_available - jobAttributesList[GLOBAL_preScheduleList->jobNumber].processor_req;
			int futureRelease = 0;
			if (GLOBAL_preScheduleList->next != NULL)
			{
				genericBackupNode *future_preScheduleList = GLOBAL_preScheduleList->next;
				futureRelease = future_preScheduleList->data;
			}
			// Processors needed lesser than the limit
			if (jobAttributesList[jobNumber].processor_req < GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT)
			{
				// Condition 1
				if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors &&
					(futureRelease == 0 || (presentTime + jobAttributesList[jobNumber].execution_time) <= futureRelease))
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Book Keeper::  ALAP is set, Job:%d SATISFIED CONDITION 1\n", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
					{
						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorsInUse = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						// Job call for the GPU to handle the given Jobs and number of blocks
						queue_job_execution(processorsInUse, processor_release_time, present_time,
											schedule_method, jobNumber, processorsAllocatedList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = present_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
							printf("Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				// Condition 2
				else if ((presentTime + jobAttributesList[jobNumber].execution_time) <= GLOBAL_preScheduleList->data)
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Book Keeper::  ALAP is set, Job:%d SATISFIED CONDITION 2\n", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
					{
						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorsInUse = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						// Job call for the GPU to handle the given Jobs and number of blocks
						queue_job_execution(processorsInUse, processor_release_time, present_time,
											schedule_method, jobNumber, processorsAllocatedList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = present_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1)
						{
							printf("Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
							printf("Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				else
				{
					// ALAP Improve function
					processors_available = ALAP_improve(jobAttributesList, jobNumber, present_time,
														processors_available, processorsAllocatedList, jobScheduledQueueList);
					if (GLOBAL_preScheduleList == NULL)
					{
						processors_available = AEAP(jobAttributesList, jobNumber, present_time,
													processors_available, processorsAllocatedList, jobScheduledQueueList);
					}
					else
					{
						processors_available = AEAP_advanced(jobAttributesList, jobNumber, present_time,
															 processors_available, processorsAllocatedList, jobScheduledQueueList);
					}
				}
			}
			else
			{
				processors_available = ALAP_advanced(jobAttributesList, jobNumber, present_time,
													 processors_available, processorsAllocatedList, jobScheduledQueueList);
			}
		}
	}
	else
	{
		// Schedule the jobAttributesList to be released in a future time
		processors_available = Processors_unavailable(jobAttributesList, jobNumber, present_time,
													  processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	PROFILER_STOP(SRTG, Kernel_book_keeper)
	return processors_available;
}

int Processors_unavailable(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	PROFILER_START(SRTG, Processors_unavailable)
	if (jobAttributesList[jobNumber].processor_req < GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2)
		{
			printf("Processors unavailable:: Job:%d sent for AEAP execution\n", jobNumber);
		}
		processors_available = AEAP(jobAttributesList, jobNumber, present_time,
									processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	else if (jobAttributesList[jobNumber].processor_req >= GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT && GLOBAL_preScheduleList == NULL)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2)
		{
			printf("Processors unavailable:: Job:%d sent for ALAP execution\n", jobNumber);
		}
		processors_available = ALAP(jobAttributesList, jobNumber, present_time,
									processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	else
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2)
		{
			printf("Processors unavailable:: Job:%d sent for ALAP Advanced execution\n", jobNumber);
		}
		processors_available = ALAP_advanced(jobAttributesList, jobNumber, present_time,
											 processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	PROFILER_STOP(SRTG, Processors_unavailable)
	return processors_available;
}
