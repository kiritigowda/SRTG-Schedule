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
	scheduledJobNode ** processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{
	PROFILER_START(SRTG, ALAP_advanced)
	if (GLOBAL_RTGS_DEBUG_MSG > 2) {
		printf("As Late As Possible Advanced (ALAP-A) -- Job-%d is verified for AEAP advanced scheduling\n", jobNumber);
	}
	int Pro = 0, job_release_time;
	scheduledJobNode* temp = *processorsAllocatedList;
	jobBackupNode *alap_check = GLOBAL_ALAP_LIST;

	while (alap_check->next != NULL)
		alap_check = alap_check->next;

	if ((alap_check->processor_release_time + jobAttributesList[jobNumber].execution_time) <= jobAttributesList[jobNumber].deadline)
	{
		if (processors_available == MAX_GPU_PROCESSOR)
		{
			job_release_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
			int processorReleased = jobAttributesList[jobNumber].processor_req;
			int processor_release_time = jobAttributesList[jobNumber].deadline;
			int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

			jobAttributesList[jobNumber].schedule_hardware = 1;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].scheduled_execution = job_release_time;
			jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
			GLOBAL_GPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("As Late As Possible Advanced (ALAP-A) -- Job-%d scheduled\n", jobNumber);
				printf("ALAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
			GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time,
				processorReleased, jobNumber);
			Kernel_queue_handler(processorReleased, job_release_time, processor_release_time, schedule_method,
				jobNumber, jobSchdeuleQueueList);
			PROFILER_STOP(SRTG, ALAP_advanced)
			return processors_available;
		}
		else if (alap_check->processors_allocated >= jobAttributesList[jobNumber].processor_req)
		{
			job_release_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
			int processorReleased = jobAttributesList[jobNumber].processor_req;
			int processor_release_time = jobAttributesList[jobNumber].deadline;
			int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

			jobAttributesList[jobNumber].schedule_hardware = 1;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].scheduled_execution = job_release_time;
			jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
			GLOBAL_GPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("As Late As Possible Advanced (ALAP-A) -- Job-%d scheduled\n", jobNumber);
				printf("ALAP-A -- Jos ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
			GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time, processorReleased, jobNumber);
			Kernel_queue_handler(processorReleased, job_release_time, processor_release_time, schedule_method, jobNumber, jobSchdeuleQueueList);
			PROFILER_STOP(SRTG, ALAP_advanced)
			return processors_available;
		}
		else if (alap_check->processors_allocated < jobAttributesList[jobNumber].processor_req)
		{
			while (temp != NULL)
			{
				if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
				{
					jobAttributesList[jobNumber].schedule_hardware = 2;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].completion_time = -1;
					jobAttributesList[jobNumber].scheduled_execution = -1;
					GLOBAL_CPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 2) {
						printf("As Late As Possible Advanced (ALAP-A) -- Job-%d cannot be scheduled\n", jobNumber);
						printf("ALAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
					PROFILER_STOP(SRTG, ALAP_advanced)
					return processors_available;
				}
				else if (temp->processor_release_time <= (jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time))
				{
					scheduledJobNode *t1 = temp;
					Pro = alap_check->processors_allocated;
					do {
						Pro = Pro + t1->processors_allocated;

						if (t1->next == NULL && t1->processor_release_time > alap_check->data)
							Pro = Pro + (processors_available - Pro);

						if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
						{
							jobAttributesList[jobNumber].schedule_hardware = 2;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].completion_time = -1;
							jobAttributesList[jobNumber].scheduled_execution = -1;
							GLOBAL_CPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Late As Possible Advanced (ALAP-A) -- Job-%d cannot be scheduled\n", jobNumber);
								printf("ALAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
							}
							PROFILER_STOP(SRTG, ALAP_advanced)
							return processors_available;
						}
						else if (Pro >= jobAttributesList[jobNumber].processor_req)
						{
							job_release_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
							int processorReleased = jobAttributesList[jobNumber].processor_req;
							int processor_release_time = jobAttributesList[jobNumber].deadline;
							int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

							jobAttributesList[jobNumber].schedule_hardware = 1;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].scheduled_execution = job_release_time;
							jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
							GLOBAL_GPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Late As Possible Advanced (ALAP-A) -- Job-%d scheduled\n", jobNumber);
								printf("ALAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
							}
							GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time,
								processorReleased, jobNumber);
							Kernel_queue_handler(processorReleased, job_release_time, processor_release_time,
								schedule_method, jobNumber, jobSchdeuleQueueList);
							PROFILER_STOP(SRTG, ALAP_advanced)
							return processors_available;
						}
						t1 = t1->next;
					} while (t1 != NULL);
					//break;
				}
				temp = temp->next;
			} //End of while
		} //End of else if
	} //End if
	else
	{
		jobAttributesList[jobNumber].schedule_hardware = 2;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].completion_time = -1;
		jobAttributesList[jobNumber].scheduled_execution = -1;
		GLOBAL_CPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("As Late As Possible Advanced (ALAP-A) -- Job-%d Cannot be scheduled\n", jobNumber);
			printf("ALAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
		PROFILER_STOP(SRTG, ALAP_advanced)
		return processors_available;
	}
	PROFILER_STOP(SRTG, ALAP_advanced)
	return processors_available;

}
