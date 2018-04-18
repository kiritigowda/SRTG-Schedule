/*
* RTGS-AEAP.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode ** processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	PROFILER_START(SRTG, AEAP)
	int Pro = 0, job_release_time = 0;
	static int given = 0;
	genericBackupNode *P_Given_list = NULL;
	genericBackupNode *P_Given_list_t = NULL;
	scheduledResourceNode* temp = *processorsAllocatedList;

	if (GLOBAL_ALAP_LIST == NULL)
	{
		Pro = processors_available;
		P_Given_list = insert_node(P_Given_list, processors_available);
		processors_available = 0;

		while (temp != NULL)
		{
			if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
			{
				int count = 0;
				scheduledResourceNode*temp1 = *processorsAllocatedList;
				genericBackupNode* temp2 = P_Given_list;
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
				P_Given_list = clean_list(P_Given_list);
				// TBD:: Job has to be sent to CPU
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Early As Possible (AEAP) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
					printf("AEAP -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				PROFILER_STOP(SRTG, AEAP)
				return processors_available;
			}
			else
			{
				Pro = Pro + temp->processors_allocated;
				if (Pro >= jobAttributesList[jobNumber].processor_req)
				{
					temp->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
					job_release_time = temp->processor_release_time;
					P_Given_list = clean_list(P_Given_list);
					int processorReleased = jobAttributesList[jobNumber].processor_req;
					int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

					jobAttributesList[jobNumber].schedule_hardware = 1;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].scheduled_execution = job_release_time;
					jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
					GLOBAL_GPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 2) {
						printf("As Early As Possible (AEAP) -- Job-%d scheduled\n", jobNumber);
						printf("AEAP -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					queue_job_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, jobNumber, processorsAllocatedList);
					job_queue_handler(processorReleased, job_release_time, presentTime,
						schedule_method, jobNumber, jobScheduledQueueList);
					PROFILER_STOP(SRTG, AEAP)
						return processors_available;
				}
				else if (Pro < jobAttributesList[jobNumber].processor_req)
				{
					P_Given_list = insert_node(P_Given_list, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}
			}
		}
	} //End of GLOBAL_ALAP_LIST == NULL
	else if (GLOBAL_ALAP_LIST != NULL)
	{
		Pro = processors_available;
		P_Given_list = insert_node(P_Given_list, processors_available);
		processors_available = 0;
		while (temp != NULL)
		{
			if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
			{
				int count = 0;
				scheduledResourceNode*temp1 = *processorsAllocatedList;
				genericBackupNode* temp2 = P_Given_list;
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
				P_Given_list = clean_list(P_Given_list);
				// TBD:: Job has to be sent to CPU
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Early As Possible (AEAP) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
					printf("AEAP -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				PROFILER_STOP(SRTG, AEAP)
				return processors_available;
			}
			else
			{
				Pro = Pro + temp->processors_allocated;
				if (Pro >= jobAttributesList[jobNumber].processor_req)
				{
					job_release_time = temp->processor_release_time;
					int processorReleased = jobAttributesList[jobNumber].processor_req;
					int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
					int Pl = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;
					if (processor_release_time <= GLOBAL_ALAP_LIST->data)
					{
						temp->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
						P_Given_list = clean_list(P_Given_list);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = job_release_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 2) {
							printf("As Early As Possible (AEAP) -- Job-%d scheduled\n", jobNumber);
							printf("AEAP -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						queue_job_execution(processorReleased, processor_release_time, presentTime,
							schedule_method, jobNumber, processorsAllocatedList);
						job_queue_handler(processorReleased, job_release_time, presentTime,
							schedule_method, jobNumber, jobScheduledQueueList);
						PROFILER_STOP(SRTG, AEAP)
						return processors_available;
					}
					else if (jobAttributesList[jobNumber].processor_req > Pl && processor_release_time > GLOBAL_ALAP_LIST->data)
					{
						if (Pro >= jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req && job_release_time < GLOBAL_ALAP_LIST->data)
						{
							//Improve ALAP release time
							processors_available = AEAP_ALAP_improve(jobAttributesList, job_release_time, present_time,
								processors_available, processorsAllocatedList, jobScheduledQueueList);
						}
						if (P_Given_list != NULL)
						{
							int count = 0;
							scheduledResourceNode*temp1 = *processorsAllocatedList;
							genericBackupNode* temp2 = P_Given_list;
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
							P_Given_list = clean_list(P_Given_list);
							if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Early As Possible (AEAP) -- AEAP with ALAP-->Backup processors reloaded\n");
							}
						}
						processors_available = AEAP_advanced(jobAttributesList, jobNumber, present_time,
							processors_available, processorsAllocatedList, jobScheduledQueueList);
					}

					else if ((jobAttributesList[jobNumber].processor_req + given) <= Pl && (temp->next == NULL))
					{
						temp->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
						P_Given_list = clean_list(P_Given_list);
						given = jobAttributesList[jobNumber].processor_req;

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = job_release_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 2) {
							printf("As Early As Possible (AEAP) -- AEAP with ALAP Condition-1, Job-%d scheduled\n", jobNumber);
							printf("AEAP -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						queue_job_execution(processorReleased, processor_release_time, presentTime,
							schedule_method, jobNumber, processorsAllocatedList);
						job_queue_handler(processorReleased, job_release_time, presentTime,
							schedule_method, jobNumber, jobScheduledQueueList);
						PROFILER_STOP(SRTG, AEAP)
						return processors_available;
					}
					else if ((jobAttributesList[jobNumber].processor_req + given) <= Pl && (temp->next != NULL))
					{
						scheduledResourceNode* check = temp->next;
						if (check->processors_allocated + jobAttributesList[jobNumber].processor_req <= Pl)
						{
							temp->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
							P_Given_list = clean_list(P_Given_list);
							given = jobAttributesList[jobNumber].processor_req;

							jobAttributesList[jobNumber].schedule_hardware = 1;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].scheduled_execution = job_release_time;
							jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
							GLOBAL_GPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Early As Possible (AEAP) -- AEAP with ALAP Condition-1, Job-%d scheduled\n", jobNumber);
								printf("AEAP -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
							}
							queue_job_execution(processorReleased, processor_release_time, presentTime, schedule_method, jobNumber, processorsAllocatedList);
							job_queue_handler(processorReleased, job_release_time, presentTime, schedule_method, jobNumber, jobScheduledQueueList);
							PROFILER_STOP(SRTG, AEAP)
							return processors_available;
						}
						else
						{
							if (jobAttributesList[jobNumber].processor_req <= Pl)
							{
								scheduledResourceNode *t1 = temp->next;
								scheduledResourceNode *t2 = temp->next; // Back up
								int Pro_t = 0;

								do {
									Pro_t = Pro_t + t1->processors_allocated;
									t1->processors_allocated = 0;
									P_Given_list_t = insert_node(P_Given_list_t,
										t1->processors_allocated);

									if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
									{
										scheduledResourceNode* temp1 = t2;
										genericBackupNode* temp2 = P_Given_list_t;

										while (temp2 != NULL)
										{
											temp1->processors_allocated = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;
										}
										P_Given_list_t = clean_list(P_Given_list_t);
										// TBD:: Job has to be sent to CPU
										jobAttributesList[jobNumber].schedule_hardware = 2;
										jobAttributesList[jobNumber].rescheduled_execution = -1;
										jobAttributesList[jobNumber].completion_time = -1;
										jobAttributesList[jobNumber].scheduled_execution = -1;
										GLOBAL_CPU_JOBS++;
										if (GLOBAL_RTGS_DEBUG_MSG > 2) {
											printf("As Early As Possible (AEAP) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
											printf("AEAP -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
										}
										break;
									}
									else if (Pro_t >= jobAttributesList[jobNumber].processor_req)
									{
										t1->processors_allocated = Pro_t - jobAttributesList[jobNumber].processor_req;
										job_release_time = t1->processor_release_time;
										P_Given_list_t = clean_list(P_Given_list_t);
										int processorReleased = jobAttributesList[jobNumber].processor_req;
										int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
										int presentTime = present_time;
										int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

										jobAttributesList[jobNumber].schedule_hardware = 1;
										jobAttributesList[jobNumber].rescheduled_execution = -1;
										jobAttributesList[jobNumber].scheduled_execution = job_release_time;
										jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
										GLOBAL_GPU_JOBS++;
										if (GLOBAL_RTGS_DEBUG_MSG > 2) {
											printf("As Early As Possible (AEAP) -- Job-%d scheduled\n", jobNumber);
											printf("AEAP -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
										}
										queue_job_execution(processorReleased, processor_release_time, presentTime,
											schedule_method, jobNumber, processorsAllocatedList);
										job_queue_handler(processorReleased, job_release_time, presentTime,
											schedule_method, jobNumber, jobScheduledQueueList);
										break;
									}
									t1 = t1->next;
								} while (t1 != NULL);
							}
							if (jobAttributesList[jobNumber].processor_req > Pl)
							{
								if (P_Given_list != NULL)
								{
									int count = 0;
									scheduledResourceNode*temp1 = *processorsAllocatedList;
									genericBackupNode* temp2 = P_Given_list;
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
									P_Given_list = clean_list(P_Given_list);
									if (GLOBAL_RTGS_DEBUG_MSG > 2) {
										printf("As Early As Possible (AEAP) -- Backup processors reloaded\n");
									}
								}
								//******* Schedule after ALAP NEEDED ********
								processors_available = ALAP_advanced(jobAttributesList, jobNumber, present_time,
									processors_available, processorsAllocatedList, jobScheduledQueueList);
							}
						} // temp->next != NULL -- Else end
					} //End temp->next != NULL
					break;
				} //End Pro >= jobAttributesList[jobNumber].processor_req
				else if (Pro < jobAttributesList[jobNumber].processor_req)
				{
					P_Given_list = insert_node(P_Given_list, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}
			} //End of else
		} //End of while
	} //End of GLOBAL_ALAP_LIST != NULL
	if (P_Given_list != NULL)
	{
		int count = 0;
		scheduledResourceNode*temp1 = *processorsAllocatedList;
		genericBackupNode* temp2 = P_Given_list;
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
		P_Given_list = clean_list(P_Given_list);
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("As Early As Possible (AEAP) -- Backup processors reloaded-->\n");
		}
	}
	PROFILER_STOP(SRTG, AEAP)
	return processors_available;
}
