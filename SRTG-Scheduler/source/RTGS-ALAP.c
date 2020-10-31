/*
* RTGS-ALAP.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	int localProcessors = 0, job_release_time, processor_release_time = 0, processors_allocated = 0;
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
			printf("Mode 4 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 4 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
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
			printf("Mode 4 ALAP: The Job:%d scheduled\n", jobNumber);
			printf("Mode 4 ALAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
					printf("Mode 4 ALAP: The Job:%d scheduled\n", jobNumber);
					printf("Mode 4 ALAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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