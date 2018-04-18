/*
* RTGS-AEAP_ALT.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP_advanced
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode ** processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	PROFILER_START(SRTG, AEAP_advanced)
	if (GLOBAL_RTGS_DEBUG_MSG > 2) {
		printf("As Early As Possible Advanced (AEAP-A) -- Job-%d is verified for AEAP Advanced scheduling\n", jobNumber);
	}

	int Pro = 0, job_release_time;
	genericBackupNode *P_Given_list = NULL;
	scheduledResourceNode* temp = *processorsAllocatedList;

	if (GLOBAL_ALAP_LIST->next == NULL)
	{
		int Pl = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;

		if (jobAttributesList[jobNumber].processor_req <= Pl)
		{
			while (temp != NULL)
			{
				if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
				{
					PROFILER_STOP(SRTG, AEAP_advanced)
					// TBD:: Job has to be sent to CPU
					jobAttributesList[jobNumber].schedule_hardware = 2;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].completion_time = -1;
					jobAttributesList[jobNumber].scheduled_execution = -1;
					GLOBAL_CPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 2) {
						printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, can not be scheduled before deadline\n", jobNumber);
						printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
					return processors_available;
				}
				else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->data)
				{
					temp = temp->next;
				}
				else if (temp->processor_release_time > GLOBAL_ALAP_LIST->data)
				{
					scheduledResourceNode *t1 = temp;
					scheduledResourceNode *t2 = temp; // Back up
					Pro = 0;
					do
					{
						Pro = Pro + t1->processors_allocated;
						t1->processors_allocated = 0;
						P_Given_list = insert_node(P_Given_list, t1->processors_allocated);

						if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
						{
							scheduledResourceNode* temp1 = t2;
							genericBackupNode* temp2 = P_Given_list;
							while (temp2 != NULL)
							{
								temp1->processors_allocated = temp2->data;
								temp1 = temp1->next;
								temp2 = temp2->next;
							}
							P_Given_list = clean_list(P_Given_list);
							// TBD:: Job has to be sent to CPU
							jobAttributesList[jobNumber].schedule_hardware = 2;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].completion_time = -1;
							jobAttributesList[jobNumber].scheduled_execution = -1;
							GLOBAL_CPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
								printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
							}
							PROFILER_STOP(SRTG, AEAP_advanced)
							return processors_available;
						}
						else if (Pro >= jobAttributesList[jobNumber].processor_req)
						{
							t1->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
							job_release_time = t1->processor_release_time;
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
								printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", jobNumber);
								printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
							}
							queue_job_execution(processorReleased, processor_release_time, presentTime,
								schedule_method, jobNumber, processorsAllocatedList);
							job_queue_handler(processorReleased, job_release_time, presentTime,
								schedule_method, jobNumber, jobScheduledQueueList);
							PROFILER_STOP(SRTG, AEAP_advanced)
							return processors_available;
						}
						t1 = t1->next;
					} while (t1 != NULL);
				}
			} //End of while
		} //End of if
		else if (jobAttributesList[jobNumber].processor_req > Pl)
		{
			if ((GLOBAL_ALAP_LIST->processor_release_time + jobAttributesList[jobNumber].execution_time) <= jobAttributesList[jobNumber].deadline)
			{
				if (GLOBAL_ALAP_LIST->processors_allocated >= jobAttributesList[jobNumber].processor_req)
				{
					GLOBAL_ALAP_LIST->processors_allocated = GLOBAL_ALAP_LIST->processors_allocated - jobAttributesList[jobNumber].processor_req;
					job_release_time = GLOBAL_ALAP_LIST->processor_release_time;
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
						printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", jobNumber);
						printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					queue_job_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, jobNumber, processorsAllocatedList);
					job_queue_handler(processorReleased, job_release_time, presentTime,
						schedule_method, jobNumber, jobScheduledQueueList);
					PROFILER_STOP(SRTG, AEAP_advanced)
					return processors_available;
				}
				else if (GLOBAL_ALAP_LIST->processors_allocated < jobAttributesList[jobNumber].processor_req)
				{
					while (temp != NULL) {
						if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
						{
							PROFILER_STOP(SRTG, AEAP_advanced)
							// TBD:: Job has to be sent to CPU
							jobAttributesList[jobNumber].schedule_hardware = 2;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].completion_time = -1;
							jobAttributesList[jobNumber].scheduled_execution = -1;
							GLOBAL_CPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, can not be scheduled before deadline\n", jobNumber);
								printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
							}
							return processors_available;
						}
						else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->processor_release_time)
						{
							temp = temp->next;
						}
						else if (temp->processor_release_time > GLOBAL_ALAP_LIST->processor_release_time)
						{
							scheduledResourceNode *t1 = temp;
							scheduledResourceNode *t2 = temp; // Back up
							Pro = GLOBAL_ALAP_LIST->processors_allocated;
							P_Given_list = insert_node(P_Given_list, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;
							do
							{
								Pro = Pro + t1->processors_allocated;
								t1->processors_allocated = 0;
								P_Given_list = insert_node(P_Given_list, t1->processors_allocated);

								if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
								{
									int count = 0;
									scheduledResourceNode* temp1 = t2;
									genericBackupNode* temp2 = P_Given_list;

									while (temp2 != NULL)
									{
										if (count == 0) {
											GLOBAL_ALAP_LIST->processors_allocated = temp2->data;
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
										printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
										printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
									}
									PROFILER_STOP(SRTG, AEAP_advanced)
									return processors_available;
								}
								else if (Pro >= jobAttributesList[jobNumber].processor_req)
								{
									t1->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
									job_release_time = t1->processor_release_time;

									int processorReleased = jobAttributesList[jobNumber].processor_req;
									int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
									int presentTime = present_time;
									int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
									P_Given_list = clean_list(P_Given_list);

									jobAttributesList[jobNumber].schedule_hardware = 1;
									jobAttributesList[jobNumber].rescheduled_execution = -1;
									jobAttributesList[jobNumber].scheduled_execution = job_release_time;
									jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
									GLOBAL_GPU_JOBS++;
									if (GLOBAL_RTGS_DEBUG_MSG > 2) {
										printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", jobNumber);
										printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
									}
									queue_job_execution(processorReleased, processor_release_time, presentTime,
										schedule_method, jobNumber, processorsAllocatedList);
									job_queue_handler(processorReleased, job_release_time, presentTime,
										schedule_method, jobNumber, jobScheduledQueueList);
									PROFILER_STOP(SRTG, AEAP_advanced)
									return processors_available;
								}
								t1 = t1->next;
							} while (t1 != NULL);
						}
					} //End of while
				}
			}
			else {
				// TBD:: Job has to be sent to CPU
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
					printf("AEAP_A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				PROFILER_STOP(SRTG, AEAP_advanced)
				return processors_available;
			}
		} //End of else if
		else {
			printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, ODD CASE\n", jobNumber);
		}
	} //End of GLOBAL_ALAP_LIST->next == NULL
	if (GLOBAL_ALAP_LIST->next != NULL)
	{
		genericBackupNode* check = GLOBAL_ALAP_LIST->next;
		int Pl = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;

		if (jobAttributesList[jobNumber].processor_req <= Pl)
		{
			if ((jobAttributesList[jobNumber].deadline <= check->data))
			{
				while (temp != NULL)
				{
					if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
					{
						PROFILER_STOP(SRTG, AEAP_advanced)
						// TBD:: Job has to be sent to CPU
						jobAttributesList[jobNumber].schedule_hardware = 2;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].completion_time = -1;
						jobAttributesList[jobNumber].scheduled_execution = -1;
						GLOBAL_CPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 2) {
							printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, can not be scheduled before deadline\n", jobNumber);
							printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
						return processors_available;
					}
					else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->data)
					{
						temp = temp->next;
					}
					else if (temp->processor_release_time > GLOBAL_ALAP_LIST->data)
					{
						scheduledResourceNode *t1 = temp;
						scheduledResourceNode *t2 = temp; // Back up
						Pro = 0;
						do
						{
							Pro = Pro + t1->processors_allocated;
							t1->processors_allocated = 0;
							P_Given_list = insert_node(P_Given_list, t1->processors_allocated);

							if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
							{
								scheduledResourceNode* temp1 = t2;
								genericBackupNode* temp2 = P_Given_list;

								while (temp2 != NULL)
								{
									temp1->processors_allocated = temp2->data;
									temp1 = temp1->next;
									temp2 = temp2->next;
								}
								P_Given_list = clean_list(P_Given_list);
								// TBD:: Job has to be sent to CPU
								jobAttributesList[jobNumber].schedule_hardware = 2;
								jobAttributesList[jobNumber].rescheduled_execution = -1;
								jobAttributesList[jobNumber].completion_time = -1;
								jobAttributesList[jobNumber].scheduled_execution = -1;
								GLOBAL_CPU_JOBS++;
								if (GLOBAL_RTGS_DEBUG_MSG > 2) {
									printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
									printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
								}
								PROFILER_STOP(SRTG, AEAP_advanced)
								return processors_available;
							}
							else if (Pro >= jobAttributesList[jobNumber].processor_req)
							{
								t1->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
								job_release_time = t1->processor_release_time;
								int processorReleased = jobAttributesList[jobNumber].processor_req;
								int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
								int presentTime = present_time;
								int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
								P_Given_list = clean_list(P_Given_list);

								jobAttributesList[jobNumber].schedule_hardware = 1;
								jobAttributesList[jobNumber].rescheduled_execution = -1;
								jobAttributesList[jobNumber].scheduled_execution = job_release_time;
								jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
								GLOBAL_GPU_JOBS++;
								if (GLOBAL_RTGS_DEBUG_MSG > 2) {
									printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", jobNumber);
									printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
								}
								queue_job_execution(processorReleased, processor_release_time, presentTime,
									schedule_method, jobNumber, processorsAllocatedList);
								job_queue_handler(processorReleased, job_release_time, presentTime,
									schedule_method, jobNumber, jobScheduledQueueList);
								PROFILER_STOP(SRTG, AEAP_advanced)
								return processors_available;
							}
							t1 = t1->next;
						} while (t1 != NULL);
					}
				} //End of while
			}
			else
			{
				// TBD:: Job has to be sent to CPU
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
					printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				PROFILER_STOP(SRTG, AEAP_advanced)
				return processors_available;
			}
		} //End of if
		else if (jobAttributesList[jobNumber].processor_req > Pl)
		{
			if ((GLOBAL_ALAP_LIST->processor_release_time + jobAttributesList[jobNumber].execution_time) <= jobAttributesList[jobNumber].deadline	&&
				jobAttributesList[jobNumber].deadline <= check->data)
			{
				if (GLOBAL_ALAP_LIST->processors_allocated >= jobAttributesList[jobNumber].processor_req)
				{
					GLOBAL_ALAP_LIST->processors_allocated = GLOBAL_ALAP_LIST->processors_allocated - jobAttributesList[jobNumber].processor_req;
					job_release_time = GLOBAL_ALAP_LIST->processor_release_time;

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
						printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", jobNumber);
						printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					queue_job_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, jobNumber, processorsAllocatedList);
					job_queue_handler(processorReleased, job_release_time, presentTime,
						schedule_method, jobNumber, jobScheduledQueueList);
					return processors_available;
				}
				else if (GLOBAL_ALAP_LIST->processors_allocated < jobAttributesList[jobNumber].processor_req)
				{
					while (temp != NULL)
					{
						if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
						{
							// TBD:: Job has to be sent to CPU
							jobAttributesList[jobNumber].schedule_hardware = 2;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].completion_time = -1;
							jobAttributesList[jobNumber].scheduled_execution = -1;
							GLOBAL_CPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, can not be scheduled before deadline\n", jobNumber);
								printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
							}
							return processors_available;
						}
						else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->processor_release_time)
						{
							temp = temp->next;
						}
						else if (temp->processor_release_time > GLOBAL_ALAP_LIST->processor_release_time)
						{
							scheduledResourceNode *t1 = temp;
							scheduledResourceNode *t2 = temp; // Back up

							Pro = GLOBAL_ALAP_LIST->processors_allocated;
							P_Given_list = insert_node(P_Given_list, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;
							do
							{
								Pro = Pro + t1->processors_allocated;
								t1->processors_allocated = 0;
								P_Given_list = insert_node(P_Given_list, t1->processors_allocated);

								if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
								{
									int count = 0;
									scheduledResourceNode* temp1 = t2;
									genericBackupNode* temp2 = P_Given_list;

									while (temp2 != NULL)
									{
										if (count == 0) {
											GLOBAL_ALAP_LIST->processors_allocated = temp2->data;
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
										printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
										printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
									}
									return processors_available;
								}
								else if (Pro >= jobAttributesList[jobNumber].processor_req)
								{
									t1->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
									job_release_time = t1->processor_release_time;

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
										printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", jobNumber);
										printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
									}
									queue_job_execution(processorReleased, processor_release_time, presentTime,
										schedule_method, jobNumber, processorsAllocatedList);
									job_queue_handler(processorReleased, job_release_time, presentTime,
										schedule_method, jobNumber, jobScheduledQueueList);
									return processors_available;
								}
								t1 = t1->next;
							} while (t1 != NULL);
						}
					} //End of while
				}
			}
			else
			{
				// TBD:: Job has to be sent to CPU
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, Condition 1 & 2 Fail\n", jobNumber);
					printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				PROFILER_STOP(SRTG, AEAP_advanced)
				return processors_available;
			}
		} //End of else if
		else {
			printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, ODD CASE\n", jobNumber);
		}
	} //End of GLOBAL_ALAP_LIST->next != NULL
	PROFILER_STOP(SRTG, AEAP_advanced)
	printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, ODD CASE FINAL\n", jobNumber);
	return processors_available;
}

