/*
* RTGS-AEAP_ALT.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP_advanced
(
	kernelInfo *kernel_info_list, 
	int kernel_number, 
	int present_time, 
	int processors_available, 
	scheduledNode ** processor_alloc_list,
	scheduledNode **kernel_queue_list
)
{
	PROFILER_START(SRTG, AEAP_advanced)
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
	printf("As Early As Possible Advanced (AEAP-A) -- Job-%d is verified for ALAP Advanced scheduling\n", kernel_number);
}

	int Pro = 0, kernel_release_time;
	backup_list *P_Given_list = NULL;
	scheduledNode* temp = *processor_alloc_list;

	if (GLOBAL_ALAP_LIST->next == NULL) 
	{
		int Pl = MAX_GPU_PROCESSOR - kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req;

		if (kernel_info_list[kernel_number].processor_req <= Pl)
		{
			while (temp != NULL)
			{
				if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
				{
					PROFILER_STOP(SRTG, AEAP_advanced)
					return processors_available;
				}
				else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->data)
				{
					temp = temp->next;
				}
				else if (temp->processor_release_time > GLOBAL_ALAP_LIST->data) 
				{
					scheduledNode *t1 = temp;
					scheduledNode *t2 = temp; // Back up
					Pro = 0;
					do 
					{
						Pro = Pro + t1->processors_allocated;
						t1->processors_allocated = 0;
						P_Given_list = insert_list(P_Given_list, t1->processors_allocated);

						if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
						{
							scheduledNode* temp1 = t2;
							backup_list* temp2 = P_Given_list;
							while (temp2 != NULL)
							{
								temp1->processors_allocated = temp2->data;
								temp1 = temp1->next;
								temp2 = temp2->next;
							}
							P_Given_list = clean_list(P_Given_list);
							// TBD:: Kernel has to be sent to CPU
							GLOBAL_CPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
							printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", kernel_number);
							printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
}
							PROFILER_STOP(SRTG, AEAP_advanced)
							return processors_available;
						}
						else if (Pro >= kernel_info_list[kernel_number].processor_req)
						{
							t1->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
							kernel_release_time = t1->processor_release_time;
							int processorReleased = kernel_info_list[kernel_number].processor_req;
							int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
							int presentTime = present_time;
							int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
							GLOBAL_GPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
							printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", kernel_number);
							printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
}
							Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
								schedule_method, kernel_number, processor_alloc_list);
							Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
								schedule_method, kernel_number, kernel_queue_list);
							PROFILER_STOP(SRTG, AEAP_advanced)
							return processors_available;
						}
						t1 = t1->next;
					} while (t1 != NULL);
				}
			} //End of while
		} //End of if
		else if (kernel_info_list[kernel_number].processor_req > Pl)
		{
			if ((GLOBAL_ALAP_LIST->processor_release_time + kernel_info_list[kernel_number].execution_time) <= kernel_info_list[kernel_number].deadline)
			{
				if (GLOBAL_ALAP_LIST->processors_allocated >= kernel_info_list[kernel_number].processor_req) 
				{
					GLOBAL_ALAP_LIST->processors_allocated = GLOBAL_ALAP_LIST->processors_allocated - kernel_info_list[kernel_number].processor_req;
					kernel_release_time = GLOBAL_ALAP_LIST->processor_release_time;
					int processorReleased = kernel_info_list[kernel_number].processor_req;
					int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
					GLOBAL_GPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", kernel_number);
					printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
}
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, kernel_number, processor_alloc_list);
					Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
						schedule_method, kernel_number, kernel_queue_list);
					PROFILER_STOP(SRTG, AEAP_advanced)
					return processors_available;
				}
				else if (GLOBAL_ALAP_LIST->processors_allocated < kernel_info_list[kernel_number].processor_req) 
				{
					while (temp != NULL) {
						if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
						{
							PROFILER_STOP(SRTG, AEAP_advanced)
							return processors_available;
						}
						else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->processor_release_time)
						{
							temp = temp->next;
						}
						else if (temp->processor_release_time > GLOBAL_ALAP_LIST->processor_release_time) 
						{
							scheduledNode *t1 = temp;
							scheduledNode *t2 = temp; // Back up
							Pro = GLOBAL_ALAP_LIST->processors_allocated;
							P_Given_list = insert_list(P_Given_list, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;
							do 
							{
								Pro = Pro + t1->processors_allocated;
								t1->processors_allocated = 0;
								P_Given_list = insert_list(P_Given_list,t1->processors_allocated);

								if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
								{
									int count = 0;
									scheduledNode* temp1 = t2;
									backup_list* temp2 = P_Given_list;

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
									// TBD:: Kernel has to be sent to CPU
									GLOBAL_CPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
									printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", kernel_number);
									printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
}
									PROFILER_STOP(SRTG, AEAP_advanced)
									return processors_available;
								}
								else if (Pro >= kernel_info_list[kernel_number].processor_req) 
								{
									t1->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
									kernel_release_time = t1->processor_release_time;

									int processorReleased = kernel_info_list[kernel_number].processor_req;
									int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
									int presentTime = present_time;
									int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
									P_Given_list = clean_list(P_Given_list);
									GLOBAL_GPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
									printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", kernel_number);
									printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
}
									Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
										schedule_method, kernel_number, processor_alloc_list);
									Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
										schedule_method, kernel_number, kernel_queue_list);
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
				// TBD:: Kernel has to be sent to CPU
				GLOBAL_CPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, Condition 1 & 2 Fail\n", kernel_number);
				printf("AEAP_A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
}
				PROFILER_STOP(SRTG, AEAP_advanced)
				return processors_available;
			}
		} //End of else if
	} //End of GLOBAL_ALAP_LIST->next == NULL
	if (GLOBAL_ALAP_LIST->next != NULL) 
	{
		backup_list* check = GLOBAL_ALAP_LIST->next;
		int Pl = MAX_GPU_PROCESSOR - kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req;

		if (kernel_info_list[kernel_number].processor_req <= Pl) 
		{
			if ((kernel_info_list[kernel_number].deadline <= check->data)) 
			{
				while (temp != NULL) 
				{
					if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) 
					{
						PROFILER_STOP(SRTG, AEAP_advanced)
						return processors_available;
					}
					else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->data) 
					{
						temp = temp->next;
					}
					else if (temp->processor_release_time > GLOBAL_ALAP_LIST->data)
					{
						scheduledNode *t1 = temp;
						scheduledNode *t2 = temp; // Back up
						Pro = 0;
						do
						{
							Pro = Pro + t1->processors_allocated;
							t1->processors_allocated = 0;
							P_Given_list = insert_list(P_Given_list, t1->processors_allocated);

							if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
							{
								scheduledNode* temp1 = t2;
								backup_list* temp2 = P_Given_list;

								while (temp2 != NULL)
								{
									temp1->processors_allocated = temp2->data;
									temp1 = temp1->next;
									temp2 = temp2->next;
								}
								P_Given_list = clean_list(P_Given_list);
								// TBD:: Kernel has to be sent to CPU
								GLOBAL_CPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, Condition 1 & 2 Fail\n", kernel_number);
								printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
}
								PROFILER_STOP(SRTG, AEAP_advanced)
								return processors_available;
							}
							else if (Pro >= kernel_info_list[kernel_number].processor_req)
							{
								t1->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
								kernel_release_time = t1->processor_release_time;
								int processorReleased = kernel_info_list[kernel_number].processor_req;
								int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
								int presentTime = present_time;
								int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
								P_Given_list = clean_list(P_Given_list);
								GLOBAL_GPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", kernel_number);
								printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
}
								Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
									schedule_method, kernel_number, processor_alloc_list);
								Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
									schedule_method, kernel_number, kernel_queue_list);
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
				// TBD:: Kernel has to be sent to CPU
				GLOBAL_CPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, Condition 1 & 2 Fail\n", kernel_number);
				printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
}
				PROFILER_STOP(SRTG, AEAP_advanced)
				return processors_available;
			}
		} //End of if
		else if (kernel_info_list[kernel_number].processor_req > Pl) 
		{
			if ((GLOBAL_ALAP_LIST->processor_release_time + kernel_info_list[kernel_number].execution_time) <= kernel_info_list[kernel_number].deadline	&&
				kernel_info_list[kernel_number].deadline <= check->data) 
			{
				if (GLOBAL_ALAP_LIST->processors_allocated >= kernel_info_list[kernel_number].processor_req) 
				{
					GLOBAL_ALAP_LIST->processors_allocated = GLOBAL_ALAP_LIST->processors_allocated - kernel_info_list[kernel_number].processor_req;
					kernel_release_time = GLOBAL_ALAP_LIST->processor_release_time;

					int processorReleased = kernel_info_list[kernel_number].processor_req;
					int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
					GLOBAL_GPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", kernel_number);
					printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
}
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
						schedule_method, kernel_number, processor_alloc_list);
					Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
						schedule_method, kernel_number, kernel_queue_list);
					return processors_available;
				}
				else if (GLOBAL_ALAP_LIST->processors_allocated < kernel_info_list[kernel_number].processor_req)
				{
					while (temp != NULL) 
					{
						if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) 
						{
							return processors_available;
						}
						else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->processor_release_time) 
						{
							temp = temp->next;
						}
						else if (temp->processor_release_time > GLOBAL_ALAP_LIST->processor_release_time) 
						{
							scheduledNode *t1 = temp;
							scheduledNode *t2 = temp; // Back up

							Pro = GLOBAL_ALAP_LIST->processors_allocated;
							P_Given_list = insert_list(P_Given_list, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;
							do 
							{
								Pro = Pro + t1->processors_allocated;
								t1->processors_allocated = 0;
								P_Given_list = insert_list(P_Given_list,t1->processors_allocated);

								if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
								{
									int count = 0;
									scheduledNode* temp1 = t2;
									backup_list* temp2 = P_Given_list;

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
									// TBD:: Kernel has to be sent to CPU
									GLOBAL_CPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
									printf("As Early As Possible Advanced (AEAP-A) -- Job-%d Cannot be scheduled, Condition 1 & 2 Fail\n", kernel_number);
									printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
}
									return processors_available;
								}
								else if (Pro >= kernel_info_list[kernel_number].processor_req)
								{
									t1->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
									kernel_release_time = t1->processor_release_time;

									int processorReleased = kernel_info_list[kernel_number].processor_req;
									int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
									int presentTime = present_time;
									int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
									GLOBAL_GPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
									printf("As Early As Possible Advanced (AEAP-A) -- Job-%d scheduled\n", kernel_number);
									printf("AEAP-A -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
}
									Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
										schedule_method, kernel_number, processor_alloc_list);
									Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
										schedule_method, kernel_number, kernel_queue_list);
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
				// TBD:: Kernel has to be sent to CPU
				GLOBAL_CPU_KERNELS++;
if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("As Early As Possible Advanced (AEAP-A) -- Job-%d cannot be scheduled, Condition 1 & 2 Fail\n", kernel_number);
				printf("AEAP-A -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
}
				PROFILER_STOP(SRTG, AEAP_advanced)
				return processors_available;
			}
		} //End of else if
	} //End of GLOBAL_ALAP_LIST->next != NULL
	PROFILER_STOP(SRTG, AEAP_advanced)
	return processors_available;
}

