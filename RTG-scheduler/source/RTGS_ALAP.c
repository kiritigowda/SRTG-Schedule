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
	scheduledJobNode ** processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{
	PROFILER_START(SRTG, ALAP)
	int Pro = 0, job_release_time, processor_release_time = 0, processors_allocated = 0;
	jobBackupNode *P_Given_list = NULL;
	processors_allocated = jobAttributesList[jobNumber].processor_req;
	processor_release_time = jobAttributesList[jobNumber].deadline;
	job_release_time = processor_release_time - jobAttributesList[jobNumber].execution_time;
	Pro = processors_available;

	if (Pro >= jobAttributesList[jobNumber].processor_req)
	{
		int processorReleased = jobAttributesList[jobNumber].processor_req;
		int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

		jobAttributesList[jobNumber].schedule_hardware = 1;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].scheduled_execution = job_release_time;
		jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
		GLOBAL_GPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("As Lata As Possible (ALAP) -- Job-%d scheduled\n", jobNumber);
			printf("ALAP -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
		}
		GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time,
			processors_allocated, jobNumber);
		Kernel_queue_handler(processorReleased, job_release_time, processor_release_time,
			schedule_method, jobNumber, jobSchdeuleQueueList);
		PROFILER_STOP(SRTG, ALAP)
			return processors_available;
	}
	P_Given_list = insert_ALAP_list(P_Given_list, job_release_time, processor_release_time,
		processors_available, jobNumber);
	scheduledJobNode* temp = *processorsAllocatedList;

	while (temp != NULL)
	{
		if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
		{
			int count = 0;
			scheduledJobNode*temp1 = *processorsAllocatedList;
			jobBackupNode* temp2 = P_Given_list;

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
			P_Given_list = clean_list(P_Given_list);
			jobAttributesList[jobNumber].schedule_hardware = 2;
			jobAttributesList[jobNumber].rescheduled_execution = -1;
			jobAttributesList[jobNumber].completion_time = -1;
			jobAttributesList[jobNumber].scheduled_execution = -1;
			GLOBAL_CPU_JOBS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("As Lata As Possible (ALAP) -- Job-%d Cannot be scheduled\n", jobNumber);
				printf("ALAP -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
			}
			PROFILER_STOP(SRTG, ALAP)
			return processors_available;
		}
		else
		{
			Pro = Pro + temp->processors_allocated;
			if (Pro >= jobAttributesList[jobNumber].processor_req)
			{
				temp->job_release_time = job_release_time;
				P_Given_list = clean_list(P_Given_list);
				int processorReleased = jobAttributesList[jobNumber].processor_req;
				int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

				jobAttributesList[jobNumber].schedule_hardware = 1;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].scheduled_execution = job_release_time;
				jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
				GLOBAL_GPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Lata As Possible (ALAP) -- Job-%d scheduled\n", jobNumber);
					printf("ALAP -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
				}
				GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time,
					processors_allocated, jobNumber);
				Kernel_queue_handler(processorReleased, job_release_time, processor_release_time,
					schedule_method, jobNumber, jobSchdeuleQueueList);
				PROFILER_STOP(SRTG, ALAP)
				return processors_available;
			}
			else if (Pro < jobAttributesList[jobNumber].processor_req)
			{
				P_Given_list = insert_ALAP_list(P_Given_list, job_release_time, processor_release_time, temp->processors_allocated, jobNumber);
				temp->job_release_time = job_release_time;
				temp = temp->next;
			}
		}
	}
	if (temp == NULL && P_Given_list != NULL)
	{
		int count = 0;
		scheduledJobNode*temp1 = *processorsAllocatedList;
		jobBackupNode* temp2 = P_Given_list;
		while (temp2 != NULL)
		{
			if (count == 0) {
				temp2 = temp2->next;
			}
			else {
				temp1->job_release_time = 0;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}
			count++;
		}
		P_Given_list = clean_list(P_Given_list);
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("As Lata As Possible (ALAP) -- Job-%d Cannot be scheduled AEAP", jobNumber);
		}
	}
	PROFILER_STOP(SRTG, ALAP)
	return processors_available;
}
