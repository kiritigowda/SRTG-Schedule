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
	scheduledResourceNode ** processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode 4 ALAP advanced: Job:%d is verified for AEAP advanced scheduling\n", jobNumber);
	}

	int localProcessors = 0, job_release_time;
	scheduledResourceNode* temp = *processorsAllocatedList;
	genericBackupNode *alap_check = GLOBAL_ALAP_LIST;

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
			job_queue_handler(processorReleased, job_release_time, processor_release_time,
				schedule_method, jobNumber, jobScheduledQueueList);
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
			job_queue_handler(processorReleased, job_release_time, processor_release_time,
				schedule_method, jobNumber, jobScheduledQueueList);
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
					scheduledResourceNode *t1 = temp;
					localProcessors = alap_check->processors_allocated;
					do {
						localProcessors = localProcessors + t1->processors_allocated;
						if (t1->next == NULL && t1->processor_release_time > alap_check->data)
							localProcessors = localProcessors + (processors_available - localProcessors);
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
						else if (localProcessors >= jobAttributesList[jobNumber].processor_req)
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
							job_queue_handler(processorReleased, job_release_time, processor_release_time,
								schedule_method, jobNumber,
								jobScheduledQueueList);
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
		job_queue_handler(processorReleased, job_release_time, processor_release_time,
			schedule_method, jobNumber, jobScheduledQueueList);
		return processors_available;
	}

	processorsDistList = insert_ALAP_list(processorsDistList, job_release_time, processor_release_time, processors_available, jobNumber);
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
				job_queue_handler(processorReleased, job_release_time, processor_release_time,
					schedule_method, jobNumber, jobScheduledQueueList);
				break;
			}
			else
			{
				processorsDistList = insert_ALAP_list(processorsDistList, job_release_time, processor_release_time, localProcessorsAllocatedList->processors_allocated, localProcessorsAllocatedList->jobNumber);
				localProcessorsAllocatedList->job_release_time = job_release_time;
			}
		}
		localProcessorsAllocatedList = localProcessorsAllocatedList->next;
	}

	return processors_available;
}

static int Mode_4_AEAP_advanced
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode ** processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode - 4 AEAP advanced: Job->%d is verified for AEAP advanced scheduling\n", jobNumber);
	}

	int localProcessors = 0, job_release_time = 0;
	genericBackupNode *processorsDistList = NULL;
	scheduledResourceNode* temp = *processorsAllocatedList;

	if (GLOBAL_ALAP_LIST->next == NULL)
	{
		int availAlapProcessors = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;
		if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors)
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
					scheduledResourceNode *t1 = temp;
					scheduledResourceNode *t2 = temp; // Back up
					localProcessors = 0;
					do {
						localProcessors = localProcessors + t1->processors_allocated;
						t1->processors_allocated = 0;
						processorsDistList = insert_node(processorsDistList, t1->processors_allocated);

						if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
						{
							scheduledResourceNode* temp1 = t2;
							genericBackupNode* temp2 = processorsDistList;

							while (temp2 != NULL)
							{
								temp1->processors_allocated = temp2->data;
								temp1 = temp1->next;
								temp2 = temp2->next;
							}
							processorsDistList = clean_list(processorsDistList);
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
						else if (localProcessors >= jobAttributesList[jobNumber].processor_req)
						{
							t1->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
							job_release_time = t1->processor_release_time;

							int processorReleased = jobAttributesList[jobNumber].processor_req;
							int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
							int presentTime = present_time;
							int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
							processorsDistList = clean_list(processorsDistList);

							jobAttributesList[jobNumber].schedule_hardware = 1;
							jobAttributesList[jobNumber].rescheduled_execution = -1;
							jobAttributesList[jobNumber].scheduled_execution = job_release_time;
							jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
							GLOBAL_GPU_JOBS++;
							if (GLOBAL_RTGS_DEBUG_MSG > 1) {
								printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
								printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
		} //End of if
		else if (jobAttributesList[jobNumber].processor_req > availAlapProcessors)
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

							localProcessors = GLOBAL_ALAP_LIST->processors_allocated;
							processorsDistList = insert_node(processorsDistList, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;
							do {
								localProcessors = localProcessors + t1->processors_allocated;
								t1->processors_allocated = 0;
								processorsDistList = insert_node(processorsDistList, t1->processors_allocated);

								if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
								{
									int count = 0;
									scheduledResourceNode* temp1 = t2;
									genericBackupNode* temp2 = processorsDistList;

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
									processorsDistList = clean_list(processorsDistList);
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
								else if (localProcessors >= jobAttributesList[jobNumber].processor_req)
								{
									t1->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
									job_release_time = t1->processor_release_time;
									int processorReleased = jobAttributesList[jobNumber].processor_req;
									int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
									int presentTime = present_time;
									int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
									processorsDistList = clean_list(processorsDistList);

									jobAttributesList[jobNumber].schedule_hardware = 1;
									jobAttributesList[jobNumber].rescheduled_execution = -1;
									jobAttributesList[jobNumber].scheduled_execution = job_release_time;
									jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
									GLOBAL_GPU_JOBS++;
									if (GLOBAL_RTGS_DEBUG_MSG > 1) {
										printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
										printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
		genericBackupNode* check = GLOBAL_ALAP_LIST->next;
		int availAlapProcessors = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;
		if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors)
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
						scheduledResourceNode *t1 = temp;
						scheduledResourceNode *t2 = temp; // Back up
						localProcessors = 0;
						do
						{
							localProcessors = localProcessors + t1->processors_allocated;
							t1->processors_allocated = 0;
							processorsDistList = insert_node(processorsDistList, t1->processors_allocated);

							if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
							{
								scheduledResourceNode* temp1 = t2;
								genericBackupNode* temp2 = processorsDistList;
								while (temp2 != NULL)
								{
									temp1->processors_allocated = temp2->data;
									temp1 = temp1->next;
									temp2 = temp2->next;
								}
								processorsDistList = clean_list(processorsDistList);

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
							else if (localProcessors >= jobAttributesList[jobNumber].processor_req)
							{
								t1->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
								job_release_time = t1->processor_release_time;
								int processorReleased = jobAttributesList[jobNumber].processor_req;
								int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
								int presentTime = present_time;
								int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
								processorsDistList = clean_list(processorsDistList);

								jobAttributesList[jobNumber].schedule_hardware = 1;
								jobAttributesList[jobNumber].rescheduled_execution = -1;
								jobAttributesList[jobNumber].scheduled_execution = job_release_time;
								jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
								GLOBAL_GPU_JOBS++;
								if (GLOBAL_RTGS_DEBUG_MSG > 1) {
									printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
									printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
		else if (jobAttributesList[jobNumber].processor_req > availAlapProcessors)
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

							localProcessors = GLOBAL_ALAP_LIST->processors_allocated;
							processorsDistList = insert_node(processorsDistList, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;
							do
							{
								localProcessors = localProcessors + t1->processors_allocated;
								t1->processors_allocated = 0;
								processorsDistList = insert_node(processorsDistList, t1->processors_allocated);
								if ((t1->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
								{
									int count = 0;
									scheduledResourceNode* temp1 = t2;
									genericBackupNode* temp2 = processorsDistList;

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
									processorsDistList = clean_list(processorsDistList);
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
								else if (localProcessors >= jobAttributesList[jobNumber].processor_req)
								{
									t1->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
									job_release_time = t1->processor_release_time;
									int processorReleased = jobAttributesList[jobNumber].processor_req;
									int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
									int presentTime = present_time;
									int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
									processorsDistList = clean_list(processorsDistList);

									jobAttributesList[jobNumber].schedule_hardware = 1;
									jobAttributesList[jobNumber].rescheduled_execution = -1;
									jobAttributesList[jobNumber].scheduled_execution = job_release_time;
									jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
									GLOBAL_GPU_JOBS++;
									if (GLOBAL_RTGS_DEBUG_MSG > 1) {
										printf("Mode 4 AEAP advanced: The Job:%d scheduled\n", jobNumber);
										printf("Mode 4 AEAP advanced: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
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
	scheduledResourceNode ** processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	int localProcessors = 0, job_release_time;
	static int given = 0;
	genericBackupNode *processorsDistList = NULL;
	genericBackupNode *P_Given_list_t = NULL;
	scheduledResourceNode* temp = *processorsAllocatedList;

	if (GLOBAL_ALAP_LIST == NULL)
	{
		localProcessors = processors_available;
		processorsDistList = insert_node(processorsDistList, processors_available);
		processors_available = 0;

		while (temp != NULL)
		{
			if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
			{
				int count = 0;
				scheduledResourceNode*temp1 = *processorsAllocatedList;
				genericBackupNode* temp2 = processorsDistList;
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
					printf("Mode 4 AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				return processors_available;
			}
			else
			{
				localProcessors = localProcessors + temp->processors_allocated;
				if (localProcessors >= jobAttributesList[jobNumber].processor_req)
				{
					temp->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
					job_release_time = temp->processor_release_time;
					processorsDistList = clean_list(processorsDistList);
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
					queue_job_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, jobNumber, processorsAllocatedList);
					job_queue_handler(processorReleased, job_release_time, presentTime,
						schedule_method, jobNumber, jobScheduledQueueList);

					return processors_available;
				}
				else if (localProcessors < jobAttributesList[jobNumber].processor_req)
				{

					processorsDistList = insert_node(processorsDistList, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}
			}
		}
	} //End of GLOBAL_ALAP_LIST == NULL
	else if (GLOBAL_ALAP_LIST != NULL)
	{
		localProcessors = processors_available;
		processorsDistList = insert_node(processorsDistList, processors_available);
		processors_available = 0;
		while (temp != NULL)
		{
			if ((temp->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
			{
				int count = 0;
				scheduledResourceNode*temp1 = *processorsAllocatedList;
				genericBackupNode* temp2 = processorsDistList;
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
					printf("Mode 4 AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
					printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
				}
				return processors_available;
			}
			else
			{
				localProcessors = localProcessors + temp->processors_allocated;
				if (localProcessors >= jobAttributesList[jobNumber].processor_req)
				{
					job_release_time = temp->processor_release_time;
					int processorReleased = jobAttributesList[jobNumber].processor_req;
					int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
					int availAlapProcessors = MAX_GPU_PROCESSOR - jobAttributesList[GLOBAL_ALAP_LIST->jobNumber].processor_req;

					if (processor_release_time <= GLOBAL_ALAP_LIST->data)
					{
						temp->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
						processorsDistList = clean_list(processorsDistList);

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
						queue_job_execution(processorReleased, processor_release_time, presentTime,
							schedule_method, jobNumber, processorsAllocatedList);
						job_queue_handler(processorReleased, job_release_time, presentTime,
							schedule_method, jobNumber, jobScheduledQueueList);

						return processors_available;
					}
					else if (jobAttributesList[jobNumber].processor_req > availAlapProcessors && processor_release_time > GLOBAL_ALAP_LIST->data)
					{
						if (processorsDistList != NULL)
						{
							int count = 0;
							scheduledResourceNode*temp1 = *processorsAllocatedList;
							genericBackupNode* temp2 = processorsDistList;
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
							if (GLOBAL_RTGS_DEBUG_MSG > 1) {
								printf("Mode 4 AEAP: AEAP with ALAP, Backup processors reloaded\n");
							}
						}
						//TBD:: Schedule after ALAP NEEDED ************************
						processors_available = Mode_4_AEAP_advanced(jobAttributesList, jobNumber, present_time,
							processors_available, processorsAllocatedList, jobScheduledQueueList);
					}
					else if ((jobAttributesList[jobNumber].processor_req + given) <= availAlapProcessors && (temp->next == NULL))
					{
						temp->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
						processorsDistList = clean_list(processorsDistList);
						given = jobAttributesList[jobNumber].processor_req;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 4 AEAP:  AEAP with ALAP Condition-1 & A, The Job:%d scheduled AEAP\n", jobNumber);
						}
						queue_job_execution(processorReleased, processor_release_time, presentTime,
							schedule_method, jobNumber, processorsAllocatedList);
						job_queue_handler(processorReleased, job_release_time, presentTime,
							schedule_method, jobNumber, jobScheduledQueueList);
						return processors_available;
					}

					else if ((jobAttributesList[jobNumber].processor_req + given) <= availAlapProcessors && (temp->next != NULL))
					{
						scheduledResourceNode* check = temp->next;
						if (check->processors_allocated + jobAttributesList[jobNumber].processor_req <= availAlapProcessors)
						{
							temp->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
							processorsDistList = clean_list(processorsDistList);
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
							queue_job_execution(processorReleased, processor_release_time, presentTime, schedule_method, jobNumber,
								processorsAllocatedList);
							job_queue_handler(processorReleased, job_release_time, presentTime, schedule_method, jobNumber,
								jobScheduledQueueList);
							return processors_available;
						}
						else
						{
							if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors)
							{
								scheduledResourceNode *t1 = temp->next;
								scheduledResourceNode *t2 = temp->next; // Back up
								int Pro_t = 0;
								do
								{
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
										queue_job_execution(processorReleased, processor_release_time, presentTime,
											schedule_method, jobNumber, processorsAllocatedList);
										job_queue_handler(processorReleased, job_release_time, presentTime,
											schedule_method, jobNumber, jobScheduledQueueList);
										break;
									}
									t1 = t1->next;
								} while (t1 != NULL);
							}
							if (jobAttributesList[jobNumber].processor_req > availAlapProcessors)
							{
								if (processorsDistList != NULL)
								{
									int count = 0;
									scheduledResourceNode*temp1 = *processorsAllocatedList;
									genericBackupNode* temp2 = processorsDistList;
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
									processors_available, processorsAllocatedList, jobScheduledQueueList);
							}
						} // temp->next != NULL -- Else end
					} //End temp->next != NULL
					break;
				} //End localProcessors >= jobAttributesList[jobNumber].processor_req
				else if (localProcessors < jobAttributesList[jobNumber].processor_req)
				{
					processorsDistList = insert_node(processorsDistList, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}
			} //End of else
		} //End of while
	} //End of GLOBAL_ALAP_LIST != NULL
	if (processorsDistList != NULL)
	{
		int count = 0;
		scheduledResourceNode*temp1 = *processorsAllocatedList;
		genericBackupNode* temp2 = processorsDistList;
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
	scheduledResourceNode ** processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("Mode 4 Processors unavailable:: Job:%d sent for AEAP Advanced execution\n", jobNumber);
		}
		processors_available = Mode_4_AEAP(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST == NULL)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("Mode 4 Processors unavailable:: Job:%d sent for ALAP execution\n", jobNumber);
		}
		processors_available = Mode_4_ALAP(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	else
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("Mode 4 Processors unavailable:: Job:%d sent for ALAP Advanced execution\n", jobNumber);
		}
		processors_available = Mode_4_ALAP_advanced(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobScheduledQueueList);
	}

	return processors_available;
}

static int Mode_4_book_keeper
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int processors_available,
	int present_time,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	int processorReleased = 0, processor_release_time = 0;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	int presentTime = present_time;

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode 4 Book Keeper:: Job::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
			jobNumber, jobAttributesList[jobNumber].processor_req, jobAttributesList[jobNumber].execution_time,
			jobAttributesList[jobNumber].deadline, jobAttributesList[jobNumber].latest_schedulable_time);
	}

	// If processors available is greater than the required processors by the jobAttributesList
	if (jobAttributesList[jobNumber].processor_req <= processors_available)
	{
		if (GLOBAL_ALAP_LIST == NULL)
		{
			// Processors needed lesser than the ALAP limit
			if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
			{
				if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
				{

					processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
					processorReleased = jobAttributesList[jobNumber].processor_req;
					processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
					schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
					// Job call for the GPU to handle the given Jobs and number of blocks
					queue_job_execution(processorReleased, processor_release_time, present_time,
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
			else
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 4 Book Keeper:: Job:%d is compute intensive, sent for ALAP execution\n", jobNumber);
				}
				processors_available = Mode_4_ALAP(jobAttributesList, jobNumber, present_time,
					processors_available, processorsAllocatedList, jobScheduledQueueList);
			}
		}
		else
		{
			int availAlapProcessors = processors_available - GLOBAL_ALAP_LIST->processors_allocated;
			// Processors needed lesser than the limit
			if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
			{
				// Condition 1
				if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors)
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
						queue_job_execution(processorReleased, processor_release_time, present_time,
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
				else if ((presentTime + jobAttributesList[jobNumber].execution_time) <= GLOBAL_ALAP_LIST->data)
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
						queue_job_execution(processorReleased, processor_release_time, present_time,
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
				else {
					processors_available = Mode_4_AEAP_advanced(jobAttributesList, jobNumber, present_time,
						processors_available, processorsAllocatedList, jobScheduledQueueList);
				}
			}
			else
			{
				processors_available = Mode_4_ALAP_advanced(jobAttributesList, jobNumber, present_time,
					processors_available, processorsAllocatedList, jobScheduledQueueList);
			}
		}
	}
	else {
		// Schedule the jobAttributesList to be released in a future time
		processors_available = Mode_4_Processors_unavailable(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobScheduledQueueList);
	}

	return processors_available;
}

/**********************************************************************************************************
RTGS Mode 4 - - AEAP/ALAP Bin Packer mode->AEAP/ALAP Pack
***********************************************************************************************************/
int RTGS_mode_4(char *jobsListFileName, char *releaseTimeFilename) {

	jobAttributes jobAttributesList[MAX_JOBS] = {{0}};
	jobReleaseInfo releaseTimeInfo[MAX_JOBS] = {{0}};
	scheduledResourceNode *processorsAllocatedList = NULL;
	scheduledResourceNode *jobScheduledQueueList = NULL;

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
		processorsAvailable = Dispatch_queued_kernels(present_time, processorsAvailable, &jobScheduledQueueList, &processorsAllocatedList);
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
					printf("\nRTGS Mode 4 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 4 -- Job-%d Released\n", k1);
					printf("RTGS Mode 4 -- Job-%d Released\n", k2);
				}
				if (jobAttributesList[k1].deadline <= jobAttributesList[k2].deadline)
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k1, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k2, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
				}
				else
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k2, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_4_book_keeper(jobAttributesList, k1, processorsAvailable, present_time, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
				}
			}
			else if (releaseTimeInfo[numReleases].num_job_released > 2) { return RTGS_ERROR_NOT_IMPLEMENTED; }

			numReleases++;
			if (numReleases > maxReleases) {
				printf("RTGS Mode 4 ERROR --  Job Release Time exceded Max Releases\n");
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

	if (processorsAllocatedList || GLOBAL_ALAP_LIST) {
		printf("\nERROR -- processorsAllocatedList/GLOBAL_ALAP_LIST Failed\n");
		return RTGS_FAILURE;
	}

	return RTGS_SUCCESS;
}
