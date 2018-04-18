/*
* RTG-mode_4.c
*      Author: Kiriti Nagesh Gowda
*/
#include"RTGS.h"
/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
static int Mode_4_ALAP_advanced
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledJobNode ** processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode 4 ALAP advanced: Job:%d is verified for AEAP advanced scheduling\n", jobNumber);
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
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode 4 ALAP advanced: The Job:%d scheduled\n", jobNumber);
				printf("Mode 4 ALAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
			GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time,
				processorReleased, jobNumber);
			Kernel_queue_handler(processorReleased, job_release_time, processor_release_time,
				schedule_method, jobNumber, jobSchdeuleQueueList);
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
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode 4 ALAP advanced: The Job:%d scheduled\n", jobNumber);
				printf("Mode 4 ALAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
			}
			GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time,
				processorReleased, jobNumber);
			Kernel_queue_handler(processorReleased, job_release_time, processor_release_time,
				schedule_method, jobNumber, jobSchdeuleQueueList);
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
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 4 ALAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
						printf("Mode 4 ALAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
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
							if (GLOBAL_RTGS_DEBUG_MSG > 1) {
								printf("Mode 4 ALAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
								printf("Mode 4 ALAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
							}
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
							if (GLOBAL_RTGS_DEBUG_MSG > 1) {
								printf("Mode 4 ALAP advanced: The Job:%d scheduled\n", jobNumber);
								printf("Mode 4 ALAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
							}
							GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time,
								processorReleased, jobNumber);
							Kernel_queue_handler(processorReleased, job_release_time, processor_release_time,
								schedule_method, jobNumber,
								jobSchdeuleQueueList);
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
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 4 ALAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 4 ALAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
		return processors_available;
	}
	return processors_available;
}

static int Mode_4_ALAP
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledJobNode ** processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{
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
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 4 ALAP: The Job:%d scheduled\n", jobNumber);
			printf("Mode 4 ALAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
		}
		GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time,
			processor_release_time, processors_allocated, jobNumber);
		Kernel_queue_handler(processorReleased, job_release_time, processor_release_time,
			schedule_method, jobNumber, jobSchdeuleQueueList);
		return processors_available;
	}
	P_Given_list = insert_ALAP_list(P_Given_list, job_release_time, processor_release_time, processors_available, jobNumber);
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
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode 4 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
				printf("Mode 4 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
			}
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
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 4 ALAP: The Job:%d scheduled\n", jobNumber);
					printf("Mode 4 ALAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
				}
				GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, job_release_time, processor_release_time,
					processors_allocated, jobNumber);
				Kernel_queue_handler(processorReleased, job_release_time, processor_release_time,
					schedule_method, jobNumber, jobSchdeuleQueueList);
				return processors_available;
			}
			else if (Pro < jobAttributesList[jobNumber].processor_req)
			{
				P_Given_list = insert_ALAP_list(P_Given_list, job_release_time, processor_release_time, temp->processors_allocated, temp->jobNumber);
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
		jobAttributesList[jobNumber].schedule_hardware = 2;
		jobAttributesList[jobNumber].rescheduled_execution = -1;
		jobAttributesList[jobNumber].completion_time = -1;
		jobAttributesList[jobNumber].scheduled_execution = -1;
		GLOBAL_CPU_JOBS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 4 ALAP: The Job:%d Cannot be scheduled\n", jobNumber);
			printf("Mode 4 ALAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
		}
	}
	return processors_available;
}

static int Mode_4_AEAP_advanced
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledJobNode ** processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode - 4 AEAP advanced: Job->%d is verified for AEAP advanced scheduling\n", jobNumber);
	}

	int Pro = 0, job_release_time = 0;
	jobBackupNode *P_Given_list = NULL;
	scheduledJobNode* temp = *processorsAllocatedList;

	if (GLOBAL_ALAP_LIST->next == NULL)
	{
		int Pl = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;
		if (jobAttributesList[jobNumber].processor_req <= Pl)
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
						printf("Mode - 4 AEAP advanced -- Job-%d Cannot be scheduled, can not be scheduled before deadline\n", jobNumber);
						printf("Mode - 4 AEAP advanced -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
					return processors_available;
				}
				else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->data)
				{
					temp = temp->next;
				}
				else if (temp->processor_release_time > GLOBAL_ALAP_LIST->data)
				{
					scheduledJobNode *t1 = temp;
					scheduledJobNode *t2 = temp; // Back up
					Pro = 0;
					do {
						Pro = Pro + t1->processors_allocated;
						t1->processors_allocated = 0;
						P_Given_list = insert_list(P_Given_list, t1->processors_allocated);

						if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
						{
							scheduledJobNode* temp1 = t2;
							jobBackupNode* temp2 = P_Given_list;

							while (temp2 != NULL)
							{
								temp1->processors_allocated = temp2->data;
								temp1 = temp1->next;
								temp2 = temp2->next;
							}
							P_Given_list = clean_list(P_Given_list);
							jobAttributesList[jobNumber].schedule_hardware = 2;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].completion_time = -1;
							jobAttributesList[jobNumber].scheduled_execution = -1;
							GLOBAL_CPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 1) {
								printf("Mode 4 AEAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
								printf("Mode 4 AEAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
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
							P_Given_list = clean_list(P_Given_list);

							jobAttributesList[jobNumber].schedule_hardware = 1;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].scheduled_execution = job_release_time;
							jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
							GLOBAL_GPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 1) {
								printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
								printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
							}
							Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
								schedule_method, jobNumber, processorsAllocatedList);
							Kernel_queue_handler(processorReleased, job_release_time, presentTime,
								schedule_method, jobNumber, jobSchdeuleQueueList);

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
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
						printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, jobNumber, processorsAllocatedList);
					Kernel_queue_handler(processorReleased, job_release_time, presentTime,
						schedule_method, jobNumber, jobSchdeuleQueueList);
					return processors_available;
				}
				else if (GLOBAL_ALAP_LIST->processors_allocated < jobAttributesList[jobNumber].processor_req)
				{
					while (temp != NULL)
					{
						if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
						{
							return processors_available;
						}
						else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->processor_release_time)
						{
							temp = temp->next;
						}
						else if (temp->processor_release_time > GLOBAL_ALAP_LIST->processor_release_time)
						{
							scheduledJobNode *t1 = temp;
							scheduledJobNode *t2 = temp; // Back up

							Pro = GLOBAL_ALAP_LIST->processors_allocated;
							P_Given_list = insert_list(P_Given_list, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;
							do {
								Pro = Pro + t1->processors_allocated;
								t1->processors_allocated = 0;
								P_Given_list = insert_list(P_Given_list, t1->processors_allocated);

								if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
								{
									int count = 0;
									scheduledJobNode* temp1 = t2;
									jobBackupNode* temp2 = P_Given_list;

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
									jobAttributesList[jobNumber].schedule_hardware = 2;
									jobAttributesList[jobNumber].rescheduled_execution = -1;
									jobAttributesList[jobNumber].completion_time = -1;
									jobAttributesList[jobNumber].scheduled_execution = -1;
									GLOBAL_CPU_JOBS++;
									if (GLOBAL_RTGS_DEBUG_MSG > 1) {
										printf("Mode 4 AEAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
										printf("Mode 4 AEAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
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
									P_Given_list = clean_list(P_Given_list);

									jobAttributesList[jobNumber].schedule_hardware = 1;
									jobAttributesList[jobNumber].rescheduled_execution = -1;
									jobAttributesList[jobNumber].scheduled_execution = job_release_time;
									jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
									GLOBAL_GPU_JOBS++;
									if (GLOBAL_RTGS_DEBUG_MSG > 1) {
										printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
										printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
									}
									Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
										schedule_method, jobNumber, processorsAllocatedList);
									Kernel_queue_handler(processorReleased, job_release_time, presentTime,
										schedule_method, jobNumber, jobSchdeuleQueueList);

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
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 4 AEAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("Mode 4 AEAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				return processors_available;
			}
		} //End of else if
		else {
			printf("Mode 4 AEAP advanced: The Job:%d Cannot be scheduled --ODD CASE\n", jobNumber);
		}
	} //End of GLOBAL_ALAP_LIST->next == NULL
	if (GLOBAL_ALAP_LIST->next != NULL)
	{
		jobBackupNode* check = GLOBAL_ALAP_LIST->next;
		int Pl = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;
		if (jobAttributesList[jobNumber].processor_req <= Pl)
		{
			if ((jobAttributesList[jobNumber].deadline <= check->data))
			{
				while (temp != NULL)
				{
					if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
					{
						return processors_available;
					}
					else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->data)
					{
						temp = temp->next;
					}
					else if (temp->processor_release_time > GLOBAL_ALAP_LIST->data)
					{
						scheduledJobNode *t1 = temp;
						scheduledJobNode *t2 = temp; // Back up
						Pro = 0;
						do
						{
							Pro = Pro + t1->processors_allocated;
							t1->processors_allocated = 0;
							P_Given_list = insert_list(P_Given_list, t1->processors_allocated);

							if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
							{
								scheduledJobNode* temp1 = t2;
								jobBackupNode* temp2 = P_Given_list;
								while (temp2 != NULL)
								{
									temp1->processors_allocated = temp2->data;
									temp1 = temp1->next;
									temp2 = temp2->next;
								}
								P_Given_list = clean_list(P_Given_list);

								//Job has to be sent to CPU
								jobAttributesList[jobNumber].schedule_hardware = 2;
								jobAttributesList[jobNumber].rescheduled_execution = -1;
								jobAttributesList[jobNumber].completion_time = -1;
								jobAttributesList[jobNumber].scheduled_execution = -1;
								GLOBAL_CPU_JOBS++;
								if (GLOBAL_RTGS_DEBUG_MSG > 1) {
									printf("Mode 4 AEAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
									printf("Mode 4 AEAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
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
								P_Given_list = clean_list(P_Given_list);

								jobAttributesList[jobNumber].schedule_hardware = 1;
								jobAttributesList[jobNumber].rescheduled_execution = -1;
								jobAttributesList[jobNumber].scheduled_execution = job_release_time;
								jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
								GLOBAL_GPU_JOBS++;
								if (GLOBAL_RTGS_DEBUG_MSG > 1) {
									printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
									printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
								}

								Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
									schedule_method, jobNumber, processorsAllocatedList);
								Kernel_queue_handler(processorReleased, job_release_time, presentTime,
									schedule_method, jobNumber, jobSchdeuleQueueList);

								return processors_available;
							}
							t1 = t1->next;
						} while (t1 != NULL);
					}
				} //End of while
			}
			else
			{
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 4 AEAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("Mode 4 AEAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
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
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
						printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, jobNumber, processorsAllocatedList);
					Kernel_queue_handler(processorReleased, job_release_time, presentTime,
						schedule_method, jobNumber, jobSchdeuleQueueList);

					return processors_available;
				}
				else if (GLOBAL_ALAP_LIST->processors_allocated < jobAttributesList[jobNumber].processor_req)
				{
					while (temp != NULL)
					{
						if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
						{
							return processors_available;
						}
						else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->processor_release_time)
						{
							temp = temp->next;
						}
						else if (temp->processor_release_time > GLOBAL_ALAP_LIST->processor_release_time)
						{
							scheduledJobNode *t1 = temp;
							scheduledJobNode *t2 = temp; // Back up

							Pro = GLOBAL_ALAP_LIST->processors_allocated;
							P_Given_list = insert_list(P_Given_list, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;
							do
							{
								Pro = Pro + t1->processors_allocated;
								t1->processors_allocated = 0;
								P_Given_list = insert_list(P_Given_list, t1->processors_allocated);
								if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
								{
									int count = 0;
									scheduledJobNode* temp1 = t2;
									jobBackupNode* temp2 = P_Given_list;

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
									jobAttributesList[jobNumber].schedule_hardware = 2;
									jobAttributesList[jobNumber].rescheduled_execution = -1;
									jobAttributesList[jobNumber].completion_time = -1;
									jobAttributesList[jobNumber].scheduled_execution = -1;
									GLOBAL_CPU_JOBS++;
									if (GLOBAL_RTGS_DEBUG_MSG > 1) {
										printf("Mode 4 AEAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
										printf("Mode 4 AEAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
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
									P_Given_list = clean_list(P_Given_list);

									jobAttributesList[jobNumber].schedule_hardware = 1;
									jobAttributesList[jobNumber].rescheduled_execution = -1;
									jobAttributesList[jobNumber].scheduled_execution = job_release_time;
									jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
									GLOBAL_GPU_JOBS++;
									if (GLOBAL_RTGS_DEBUG_MSG > 1) {
										printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
										printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
									}
									Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
										schedule_method, jobNumber, processorsAllocatedList);
									Kernel_queue_handler(processorReleased, job_release_time, presentTime,
										schedule_method, jobNumber, jobSchdeuleQueueList);

									return processors_available;
								}
								t1 = t1->next;
							} while (t1 != NULL);
						}
					} //End of while
				}
			}
			else {
				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 4 AEAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("Mode 4 AEAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				return processors_available;
			}
		} //End of else if
	} //End of GLOBAL_ALAP_LIST->next != NULL

	return processors_available;
}

static int Mode_4_AEAP
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledJobNode ** processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{
	int Pro = 0, job_release_time;
	static int given = 0;
	jobBackupNode *P_Given_list = NULL;
	jobBackupNode *P_Given_list_t = NULL;
	scheduledJobNode* temp = *processorsAllocatedList;

	if (GLOBAL_ALAP_LIST == NULL)
	{
		Pro = processors_available;
		P_Given_list = insert_list(P_Given_list, processors_available);
		processors_available = 0;

		while (temp != NULL)
		{
			if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
			{
				int count = 0;
				scheduledJobNode*temp1 = *processorsAllocatedList;
				jobBackupNode* temp2 = P_Given_list;
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

				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 4 AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
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
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 4 AEAP: The Job:%d scheduled\n", jobNumber);
						printf("Mode 4 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, jobNumber, processorsAllocatedList);
					Kernel_queue_handler(processorReleased, job_release_time, presentTime,
						schedule_method, jobNumber, jobSchdeuleQueueList);

					return processors_available;
				}
				else if (Pro < jobAttributesList[jobNumber].processor_req)
				{

					P_Given_list = insert_list(P_Given_list, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}
			}
		}
	} //End of GLOBAL_ALAP_LIST == NULL
	else if (GLOBAL_ALAP_LIST != NULL)
	{
		Pro = processors_available;
		P_Given_list = insert_list(P_Given_list, processors_available);
		processors_available = 0;
		while (temp != NULL)
		{
			if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
			{
				int count = 0;
				scheduledJobNode*temp1 = *processorsAllocatedList;
				jobBackupNode* temp2 = P_Given_list;
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

				jobAttributesList[jobNumber].schedule_hardware = 2;
				jobAttributesList[jobNumber].rescheduled_execution = -1;
				jobAttributesList[jobNumber].completion_time = -1;
				jobAttributesList[jobNumber].scheduled_execution = -1;
				GLOBAL_CPU_JOBS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 4 AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
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
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 4 AEAP: AEAP with ALAP Condition-2\n");
							printf("Mode 4 AEAP: The Job:%d scheduled\n", jobNumber);
							printf("Mode 4 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
							schedule_method, jobNumber, processorsAllocatedList);
						Kernel_queue_handler(processorReleased, job_release_time, presentTime,
							schedule_method, jobNumber, jobSchdeuleQueueList);

						return processors_available;
					}
					else if (jobAttributesList[jobNumber].processor_req > Pl && processor_release_time > GLOBAL_ALAP_LIST->data)
					{
						if (P_Given_list != NULL)
						{
							int count = 0;
							scheduledJobNode*temp1 = *processorsAllocatedList;
							jobBackupNode* temp2 = P_Given_list;
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
							if (GLOBAL_RTGS_DEBUG_MSG > 1) {
								printf("Mode 4 AEAP: AEAP with ALAP, Backup processors reloaded\n");
							}
						}
						//TBD:: Schedule after ALAP NEEDED ************************
						processors_available = Mode_4_AEAP_advanced(jobAttributesList, jobNumber, present_time,
							processors_available, processorsAllocatedList, jobSchdeuleQueueList);
					}
					else if ((jobAttributesList[jobNumber].processor_req + given) <= Pl && (temp->next == NULL))
					{
						temp->processors_allocated = Pro - jobAttributesList[jobNumber].processor_req;
						P_Given_list = clean_list(P_Given_list);
						given = jobAttributesList[jobNumber].processor_req;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 4 AEAP:  AEAP with ALAP Condition-1 & A, The Job:%d scheduled AEAP\n", jobNumber);
						}
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
							schedule_method, jobNumber, processorsAllocatedList);
						Kernel_queue_handler(processorReleased, job_release_time, presentTime,
							schedule_method, jobNumber, jobSchdeuleQueueList);
						return processors_available;
					}

					else if ((jobAttributesList[jobNumber].processor_req + given) <= Pl && (temp->next != NULL))
					{
						scheduledJobNode* check = temp->next;
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
							if (GLOBAL_RTGS_DEBUG_MSG > 1) {
								printf("Mode 4 AEAP: AEAP with ALAP Condition-1 & B\n");
								printf("Mode 4 AEAP: The Job:%d scheduled\n", jobNumber);
								printf("Mode 4 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
							}
							Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, jobNumber,
								processorsAllocatedList);
							Kernel_queue_handler(processorReleased, job_release_time, presentTime, schedule_method, jobNumber,
								jobSchdeuleQueueList);
							return processors_available;
						}
						else
						{
							if (jobAttributesList[jobNumber].processor_req <= Pl)
							{
								scheduledJobNode *t1 = temp->next;
								scheduledJobNode *t2 = temp->next; // Back up
								int Pro_t = 0;
								do
								{
									Pro_t = Pro_t + t1->processors_allocated;
									t1->processors_allocated = 0;
									P_Given_list_t = insert_list(P_Given_list_t,
										t1->processors_allocated);

									if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
									{
										scheduledJobNode* temp1 = t2;
										jobBackupNode* temp2 = P_Given_list_t;
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
										if (GLOBAL_RTGS_DEBUG_MSG > 1) {
											printf("Mode 4 AEAP: The Job:%d Cannot be scheduled Condition 1 & 2 Fail\n", jobNumber);
											printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
										}
										break;

									}
									else if (Pro_t >= jobAttributesList[jobNumber].processor_req)
									{
										t1->processors_allocated = Pro_t - jobAttributesList[jobNumber].processor_req;
										job_release_time = t1->processor_release_time;

										P_Given_list_t = clean_list(
											P_Given_list_t);

										int processorReleased = jobAttributesList[jobNumber].processor_req;
										int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
										int presentTime = present_time;
										int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

										jobAttributesList[jobNumber].schedule_hardware = 1;
										jobAttributesList[jobNumber].rescheduled_execution = -1;
										jobAttributesList[jobNumber].scheduled_execution = job_release_time;
										jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
										GLOBAL_GPU_JOBS++;
										if (GLOBAL_RTGS_DEBUG_MSG > 1) {
											printf("Mode 4 AEAP: Condition-1 The Job:%d scheduled\n", jobNumber);
											printf("Mode 4 AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
										}
										Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
											schedule_method, jobNumber, processorsAllocatedList);
										Kernel_queue_handler(processorReleased, job_release_time, presentTime,
											schedule_method, jobNumber, jobSchdeuleQueueList);
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
									scheduledJobNode*temp1 = *processorsAllocatedList;
									jobBackupNode* temp2 = P_Given_list;
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
									if (GLOBAL_RTGS_DEBUG_MSG > 1) {
										printf("Mode 4 AEAP: The Job:%d Cannot be scheduled Condition 1 & 2 Fail\n", jobNumber);
										printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
									}
								}

								//TBD:: Schedule after ALAP NEEDED ********
								processors_available = Mode_4_ALAP_advanced(jobAttributesList, jobNumber, present_time,
									processors_available, processorsAllocatedList, jobSchdeuleQueueList);
							}
						} // temp->next != NULL -- Else end
					} //End temp->next != NULL
					break;
				} //End Pro >= jobAttributesList[jobNumber].processor_req
				else if (Pro < jobAttributesList[jobNumber].processor_req)
				{
					P_Given_list = insert_list(P_Given_list, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}
			} //End of else
		} //End of while
	} //End of GLOBAL_ALAP_LIST != NULL
	if (P_Given_list != NULL)
	{
		int count = 0;
		scheduledJobNode*temp1 = *processorsAllocatedList;
		jobBackupNode* temp2 = P_Given_list;
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
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("MODE 4 AEAP: Backup processors reloaded\n");
		}
	}
	return processors_available;
}

static int Mode_4_Processors_unavailable
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledJobNode ** processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{
	if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
	{
		processors_available = Mode_4_AEAP(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobSchdeuleQueueList);
	}
	else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST == NULL)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 4 Processors unavailable:: Job:%d sent for ALAP execution\n", jobNumber);
		}
		processors_available = Mode_4_ALAP(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobSchdeuleQueueList);
	}
	else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST != NULL)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 4 Processors unavailable:: Job:%d sent for ALAP execution\n", jobNumber);
		}
		processors_available = Mode_4_ALAP_advanced(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobSchdeuleQueueList);
	}

	return processors_available;
}

static int Mode_4_book_keeper
(
	jobAttributes* jobAttributesList,
	int jobNumber,
	int processors_available,
	int present_time,
	scheduledJobNode **processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{
	int processorReleased = 0, processor_release_time = 0;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	static int FLAG, FLAG_V;

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode 4 Book Keeper:: Job::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
			jobNumber, jobAttributesList[jobNumber].processor_req, jobAttributesList[jobNumber].execution_time,
			jobAttributesList[jobNumber].deadline, jobAttributesList[jobNumber].latest_schedulable_time);
	}
	// If processors available is greater than the required processors by the jobAttributesList
	if (jobAttributesList[jobNumber].processor_req <= processors_available)
	{
		//ALAP not set
		if (GLOBAL_ALAP_LIST == NULL)
		{
			FLAG = 0;
			FLAG_V = 0;
			// Processors needed lesser than the limit
			if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
			{
				if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
				{

					processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
					processorReleased = jobAttributesList[jobNumber].processor_req;
					processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
					schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
					// Job call for the GPU to handle the given Jobs and number of blocks
					Queue_kernel_execution(processorReleased, processor_release_time, present_time,
						schedule_method, jobNumber, processorsAllocatedList);

					jobAttributesList[jobNumber].schedule_hardware = 1;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].scheduled_execution = present_time;
					jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
					GLOBAL_GPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
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
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 4 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
						printf("Mode 4 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
				}
			}
			// Processors needed greater or equal than the limit
			else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT)
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 4 Book Keeper:: Job:%d is compute intensive, sent for ALAP execution\n", jobNumber);
				}
				processors_available = Mode_4_ALAP(jobAttributesList, jobNumber, present_time,
					processors_available, processorsAllocatedList, jobSchdeuleQueueList);
			}
		}
		//ALAP Job Dispateched
		else if (GLOBAL_ALAP_LIST != NULL)
		{
			//ALAP updated
			if (FLAG_V != GLOBAL_ALAP_LIST->data)
			{
				FLAG = 0;
				FLAG_V = 0;
			}

			int Pl = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;
			// Processors needed lesser than the limit
			if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
			{
				// Condition 1
				if (jobAttributesList[jobNumber].processor_req <= Pl && (present_time + jobAttributesList[jobNumber].execution_time) <= GLOBAL_ALAP_LIST->data)
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 4 Book Keeper::  ALAP is set, Job:%d SATISFIED CONDITION 1", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
					{

						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorReleased = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						// Job call for the GPU to handle the given Jobs and number of blocks
						Queue_kernel_execution(processorReleased, processor_release_time, present_time,
							schedule_method, jobNumber, processorsAllocatedList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = present_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
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
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 4 Book Keeper:: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Mode 4 Book Keeper:: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				// Condition 2
				else if (jobAttributesList[jobNumber].processor_req > Pl
					&& (present_time + jobAttributesList[jobNumber].execution_time) <= GLOBAL_ALAP_LIST->data)
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 4 Book Keeper::  ALAP is set, Job:%d SATISFIED CONDITION 2", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
					{
						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorReleased = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						// Job call for the GPU to handle the given Jobs and number of blocks
						Queue_kernel_execution(processorReleased, processor_release_time, present_time,
							schedule_method, jobNumber, processorsAllocatedList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = present_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 4 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
					}
					else
					{
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 4 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
					}
				}
				// Condition 3
				else if (jobAttributesList[jobNumber].processor_req <= Pl
					&& (present_time + jobAttributesList[jobNumber].execution_time) > GLOBAL_ALAP_LIST->data && FLAG == 0)
				{
					// Control flags to not allow over budgeting of PA
					FLAG = 1;
					FLAG_V = GLOBAL_ALAP_LIST->data;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 4 Book Keeper:: ALAP is set, Job:%d SATISFIED CONDITION 1 with FLAG", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
					{
						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorReleased = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						// Job call for the GPU to handle the given Jobs and number of blocks
						Queue_kernel_execution(processorReleased, processor_release_time, present_time,
							schedule_method, jobNumber, processorsAllocatedList);

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = present_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 4 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
					}
					else
					{
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 4 Book Keeper:: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
					}
				}
				else
				{
					processors_available = Mode_4_AEAP_advanced(jobAttributesList, jobNumber, present_time,
						processors_available, processorsAllocatedList, jobSchdeuleQueueList);
				}
			}
			// Processors needed greater or equal than the limit
			else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT)
			{
				processors_available = Mode_4_ALAP_advanced(jobAttributesList, jobNumber, present_time,
					processors_available, processorsAllocatedList, jobSchdeuleQueueList);
			}
		}
	}
	// If processors available is lesser than the required processors by the jobAttributesList
	else if (jobAttributesList[jobNumber].processor_req > processors_available)
	{
		// Schedule the jobAttributesList to be released in a future time
		processors_available = Mode_4_Processors_unavailable(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobSchdeuleQueueList);
	}

	return processors_available;
}

/**********************************************************************************************************
RTGS Mode 4 - - AEAP/ALAP Bin Packer mode->AEAP/ALAP Pack
***********************************************************************************************************/
int RTGS_mode_4(char *jobsListFileName, char *releaseTimeFilename) {

	jobAttributes jobAttributesList[MAX_JOBS] = {{0}};
	jobReleaseInfo releaseTimeInfo[MAX_JOBS] = {{0}};
	scheduledJobNode *processorsAllocatedList = NULL;
	scheduledJobNode *jobSchdeuleQueueList = NULL;		//Job queued for future executions

	// global variable initialize
	GLOBAL_GPU_JOBS = 0;
	GLOBAL_CPU_JOBS = 0;
	GLOBAL_ALAP_LIST = NULL;

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
		if (processorsAvailable < 0) { printf("Retrieve_processors ERROR- Processors Available:%d\n", processorsAvailable); return RTGS_ERROR_NOT_IMPLEMENTED; }
		processorsAvailable = Dispatch_queued_kernels(present_time, processorsAvailable, &jobSchdeuleQueueList, &processorsAllocatedList);
		if (processorsAvailable < 0) { printf("Dispatch_queued_kernels ERROR - Processors Available:%d\n", processorsAvailable); return RTGS_ERROR_NOT_IMPLEMENTED; }

		if (releaseTimeInfo[numReleases].release_time == present_time) {

			if (releaseTimeInfo[numReleases].num_job_released == 1)
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("\nRTGS Mode 4 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 4 -- Job-%d Released\n", jobNumber);
				}
				jobAttributesList[jobNumber].release_time = present_time;
				// handling the released jobAttributesList by the book-keeper
				int64_t start_t = RTGS_GetClockCounter();
				processorsAvailable = Mode_4_book_keeper(jobAttributesList, jobNumber, processorsAvailable, present_time,
					&processorsAllocatedList, &jobSchdeuleQueueList);
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
					printf("\nRTGS Mode 4 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 4 -- Job-%d Released\n", k1);
					printf("RTGS Mode 4 -- Job-%d Released\n", k2);
				}
				if (jobAttributesList[k1].deadline <= jobAttributesList[k2].deadline)
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k1, processorsAvailable, present_time, &processorsAllocatedList, &jobSchdeuleQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k2, processorsAvailable, present_time, &processorsAllocatedList, &jobSchdeuleQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
				}
				else
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k2, processorsAvailable, present_time, &processorsAllocatedList, &jobSchdeuleQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k1, processorsAvailable, present_time, &processorsAllocatedList, &jobSchdeuleQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
				}
			}
			else if (releaseTimeInfo[numReleases].num_job_released > 2) { return RTGS_ERROR_NOT_IMPLEMENTED; }

			numReleases++;
			if (numReleases > maxReleases) {
				printf("RTGS Mode 4 ERROR --  KERNEL Release Time exceded Max Releases\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	if (maxReleases != 0) {

		if (GLOBAL_RTGS_DEBUG_MSG) {
			printf("\n******* Scheduler Mode 4 *******\n");
			printf("Processors Available -- %d\n", processorsAvailable);
			printf("Total Jobs Scheduled -- %d\n", kernelMax);
			printf("	GPU Scheduled Jobs -- %d\n", GLOBAL_GPU_JOBS);
			printf("	CPU Scheduled Jobs -- %d\n", GLOBAL_CPU_JOBS);
		}

		if (RTGS_PrintScheduleSummary(4, kernelMax, jobAttributesList)) {
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
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		print(processorsAllocatedList);
	}
	processorsAllocatedList = clean_node(processorsAllocatedList);
	GLOBAL_ALAP_LIST = clean_list(GLOBAL_ALAP_LIST);

	return RTGS_SUCCESS;
}
