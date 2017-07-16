/*
* RTG-mode_4.c
*      Author: Kiriti Nagesh Gowda
*/
#include"RTGS.h"

/**********************************************************************************************************
RTGS Mode 4 - - AEAP/ALAP Bin Packer mode->AEAP/ALAP Pack
***********************************************************************************************************/
int RTGS_mode_4(char *kernel_file, char *Releasetime_file) {

	kernelInfo kernel_info_list[MAX_KERNELS];
	scheduledNode *processor_alloc_list = NULL; //*Pro_release_list=NULL;
	scheduledNode *kernel_queue_list = NULL;  //Kernel queued for future executions
	int processors_available = MAX_GPU_PROCESSOR;
	int c = 0, Nkr = 0;
	int rt = 0, kernel_number = 0, k1, k2;
	GLOBAL_GPU_KERNELS = 0;
	GLOBAL_CPU_KERNELS = 0;
	GLOBAL_ALAP_LIST = NULL;
	Nkr = get_kernel_information(kernel_info_list, kernel_file);                 				// Read Kernel.TXT
	rt = get_kernel_release_times(Releasetime_file);                   				// Read Release_time.TXT
#if DEBUG_MESSAGES
	printf("\nThe GPU Scheduler will Schedule %d Kernels\n\n", Nkr);				// Scheduler Begins
#endif
	int64_t stime = RTGS_GetClockCounter();
	for (int present_time = 0; present_time < rt; present_time++)
	{
		processors_available = Retrieve_processors(present_time, processors_available, &processor_alloc_list);     // Freeing-up processors
		processors_available = Dispatch_queued_kernels(present_time, processors_available, &kernel_queue_list, &processor_alloc_list);     // Freeing-up processors
		if (GLOBAL_RELEASE_TIME[present_time] == 1)
		{
#if DEBUG_MESSAGES
			printf("\n-->>Total processors Available at time %d = %d\n\n ", present_time, processors_available);
			printf("Kernels:%d has been released\n", kernel_number);
#endif
			processors_available = Mode_4_book_keeper(kernel_info_list, kernel_number, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
			kernel_number++;
		}
		else if (GLOBAL_RELEASE_TIME[present_time] == 2)
		{
			k1 = kernel_number; kernel_number++;
			k2 = kernel_number; kernel_number++;
#if DEBUG_MESSAGES
			printf("\n-->>Total processors Available at time %d = %d\n\n ", present_time, processors_available);
			printf("Kernels:%d has been released\n", k1);
			printf("Kernels:%d has been released\n", k2);
#endif
			if (kernel_info_list[k1].deadline <= kernel_info_list[k2].deadline)
			{
				processors_available = Mode_4_book_keeper(kernel_info_list, k1, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
				processors_available = Mode_4_book_keeper(kernel_info_list, k2, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
			}
			else
			{
				processors_available = Mode_4_book_keeper(kernel_info_list, k2, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
				processors_available = Mode_4_book_keeper(kernel_info_list, k1, processors_available, present_time, &processor_alloc_list, &kernel_queue_list); // handling the released kernel_info_list by the book-keeper
			}
		}
	}
	processor_alloc_list = clean_node(processor_alloc_list);
	GLOBAL_ALAP_LIST = clean_list(GLOBAL_ALAP_LIST);
	if (rt != 0)
	{
#if DEBUG_INFO
		printf("All Kernels Scheduled or Sent to CPU Successfully - Processors Available: %d Mode_2_AEAP Kernels: %d\n", processors_available, GLOBAL_GPU_KERNELS);
		printf("Kernels sent Back to CPU: %d\n", GLOBAL_CPU_KERNELS);
#endif
		for (int j = 0; j <= Nkr; j++)
		{
			kernel_info_list[j].processor_req = kernel_info_list[j].deadline = kernel_info_list[j].execution_time = kernel_info_list[j].latest_schedulable_time = 0;
		}
		Nkr = 0;
		rt = 0;
		kernel_number = 0;
		GLOBAL_GPU_KERNELS = 0;
		GLOBAL_CPU_KERNELS = 0;
	}
#if DEBUG_MESSAGES
	print(processor_alloc_list);
#endif
	processor_alloc_list = clean_node(processor_alloc_list);
	return RTGS_SUCCESS;
}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_book_keeper(kernelInfo* kernel_info_list, int kernel_number, int processors_available, int present_time, scheduledNode **processor_alloc_list, scheduledNode **kernel_queue_list)
{

	int processorReleased, processor_release_time, presentTime = present_time, schedule_method = 99;
	static int FLAG = 0, FLAG_V = 0;

#if DEBUG_MESSAGES
	printf("\n ^^ Kernel[%d].processor_req = %d, execution_time = %d, deadline = %d, latest_schedulable_time= %d ^^\n", kernel_number, kernel_info_list[kernel_number].processor_req, kernel_info_list[kernel_number].execution_time, kernel_info_list[kernel_number].deadline, kernel_info_list[kernel_number].latest_schedulable_time);
#endif

	if (kernel_info_list[kernel_number].processor_req <= processors_available) // If processors available is greater than the required processors by the kernel_info_list
	{
		if (GLOBAL_ALAP_LIST == NULL) { //ALAP not set
			FLAG = 0;
			FLAG_V = 0;

			if (kernel_info_list[kernel_number].processor_req < PROCESSOR_LIMIT) { // Processors needed lesser than the limit

				if (kernel_info_list[kernel_number].execution_time + presentTime <= kernel_info_list[kernel_number].deadline) {

					processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
					processorReleased = kernel_info_list[kernel_number].processor_req;
					processor_release_time = kernel_info_list[kernel_number].execution_time + presentTime;
					schedule_method = 0;
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
				}

				else {
#if DEBUG_MESSAGES
					printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", kernel_number);
#endif
					GLOBAL_CPU_KERNELS++;
				}
			}

			else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT) { // Processors needed greater or equal than the limit
#if DEBUG_MESSAGES
				printf("\n>>>>>Kernel:%d is compute intensive, sent for ALAP execution\n", kernel_number);
#endif
				processors_available = Mode_4_ALAP(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list, kernel_queue_list);
			}
		}
		else if (GLOBAL_ALAP_LIST != NULL) { //ALAP set
			if (FLAG_V != GLOBAL_ALAP_LIST->data) { //ALAP updated
				FLAG = 0;
				FLAG_V = 0;
			}
			int Pl = MAX_GPU_PROCESSOR - kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req;
			if (kernel_info_list[kernel_number].processor_req < PROCESSOR_LIMIT) { // Processors needed lesser than the limit

				if (kernel_info_list[kernel_number].processor_req <= Pl && (presentTime + kernel_info_list[kernel_number].execution_time) <= GLOBAL_ALAP_LIST->data) { // Condition 1
#if DEBUG_MESSAGES
					printf("\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 1 && 2", kernel_number);
#endif
					if (kernel_info_list[kernel_number].execution_time + presentTime <= kernel_info_list[kernel_number].deadline) {

						processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
						processorReleased = kernel_info_list[kernel_number].processor_req;
						processor_release_time = kernel_info_list[kernel_number].execution_time + presentTime;
						schedule_method = 0;
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}
					else {
#if DEBUG_MESSAGES
						printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", kernel_number);
#endif
					}
				}

				else if (kernel_info_list[kernel_number].processor_req > Pl
					&& (presentTime + kernel_info_list[kernel_number].execution_time) <= GLOBAL_ALAP_LIST->data) { // Condition 2
#if DEBUG_MESSAGES
					printf("\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 2", kernel_number);
#endif
					if (kernel_info_list[kernel_number].execution_time + presentTime <= kernel_info_list[kernel_number].deadline) {

						processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
						processorReleased = kernel_info_list[kernel_number].processor_req;
						processor_release_time = kernel_info_list[kernel_number].execution_time + presentTime;
						schedule_method = 0;
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}

					else {
#if DEBUG_MESSAGES
						printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", kernel_number);
#endif
						GLOBAL_CPU_KERNELS++;
					}
				}

				else if (kernel_info_list[kernel_number].processor_req <= Pl
					&& (presentTime + kernel_info_list[kernel_number].execution_time) > GLOBAL_ALAP_LIST->data && FLAG == 0) { // Condition 3
					// Control flags to not allow over budgeting of PA
					FLAG = 1;
					FLAG_V = GLOBAL_ALAP_LIST->data;
#if DEBUG_MESSAGES
					printf("\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 1 with FLAG", kernel_number);
#endif
					if (kernel_info_list[kernel_number].execution_time + presentTime <= kernel_info_list[kernel_number].deadline) {
						processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
						processorReleased = kernel_info_list[kernel_number].processor_req;
						processor_release_time = kernel_info_list[kernel_number].execution_time + presentTime;
						schedule_method = 0;
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}
					else {
#if DEBUG_MESSAGES
						printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", kernel_number);
#endif
						GLOBAL_CPU_KERNELS++;
					}
				}
				else {
					processors_available = AEAP_Flagged(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list, kernel_queue_list);
					GLOBAL_GPU_KERNELS++;
				}
			}
			else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT) { // Processors needed greater or equal than the limit
				processors_available = ALAP_Flagged(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list, kernel_queue_list);
				GLOBAL_GPU_KERNELS++;
			}
		}
	}
	else if (kernel_info_list[kernel_number].processor_req > processors_available) // If processors available is lesser than the required processors by the kernel_info_list
	{
		processors_available = Mode_4_Processors_unavailable(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list,
			kernel_queue_list); // Schedule the kernel_info_list to be released in a future time

	}

	return processors_available;

}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_Processors_unavailable(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list)
{
	//printf("\nKernel->%d sent for AEAP Scheduling at TIME: %d\n",kernel_number,present_time);
	if (kernel_info_list[kernel_number].processor_req < PROCESSOR_LIMIT) {
		processors_available = Mode_4_AEAP(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list, kernel_queue_list);
		GLOBAL_GPU_KERNELS++;
	}
	else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST == NULL) {
#if DEBUG_MESSAGES
		printf("\n>>>>>Kernel:%d sent for ALAP execution\n", kernel_number);
#endif
		processors_available = Mode_4_ALAP(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list, kernel_queue_list);
	}
	else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST != NULL) {
#if DEBUG_MESSAGES
		printf("\n>>>>>Kernel:%d sent for ALAP execution\n", kernel_number);
#endif
		processors_available = Mode_4_ALAP_Flagged(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list, kernel_queue_list);
	}
	return processors_available;
}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_AEAP_Flagged(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list)
{
#if DEBUG_MESSAGES
	printf("\n||---AEAP_FLAGGED-->Kernel->%d is verified for ALAP Flagged scheduling\n", kernel_number);
#endif

	int Pro = 0, kernel_release_time = 0;

	backup_list *P_Given_list = NULL;
	scheduledNode* temp = *processor_alloc_list;

	if (GLOBAL_ALAP_LIST->next == NULL) {
		int Pl = MAX_GPU_PROCESSOR - kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req;
		if (kernel_info_list[kernel_number].processor_req <= Pl) {
			while (temp != NULL) {
				if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {
					return processors_available;
				}
				else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->data) {
					temp = temp->next;
				}
				else if (temp->processor_release_time > GLOBAL_ALAP_LIST->data) {
					scheduledNode *t1 = temp;
					scheduledNode *t2 = temp; // Back up
					Pro = 0;

					do {

						Pro = Pro + t1->processors_allocated;
						t1->processors_allocated = 0;
						P_Given_list = insert_list(P_Given_list, t1->processors_allocated);

						if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

							scheduledNode* temp1 = t2;
							backup_list* temp2 = P_Given_list;

							while (temp2 != NULL) {

								temp1->processors_allocated = temp2->data;
								temp1 = temp1->next;
								temp2 = temp2->next;

							}

							P_Given_list = clean_list(P_Given_list);

							//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
							printf("\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!", kernel_number);
							printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", kernel_number);
#endif
							GLOBAL_CPU_KERNELS++;
							return processors_available;

						}

						else if (Pro >= kernel_info_list[kernel_number].processor_req) {

							t1->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
							kernel_release_time = t1->processor_release_time;

							int processorReleased = kernel_info_list[kernel_number].processor_req;
							int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
							int presentTime = present_time;
							int schedule_method = 1;

#if DEBUG_MESSAGES
							printf("\n||---AEAP_FLAGGED-->The Kernel:%d scheduled AEAP -->---||", kernel_number);
#endif

							P_Given_list = clean_list(P_Given_list);
							Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list);
							Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);

							return processors_available;
						}

						t1 = t1->next;

					} while (t1 != NULL);

				}

			} //End of while

		} //End of if

		else if (kernel_info_list[kernel_number].processor_req > Pl) {

			if ((GLOBAL_ALAP_LIST->processor_release_time + kernel_info_list[kernel_number].execution_time) <= kernel_info_list[kernel_number].deadline) {

				if (GLOBAL_ALAP_LIST->processors_allocated >= kernel_info_list[kernel_number].processor_req) {

					GLOBAL_ALAP_LIST->processors_allocated = GLOBAL_ALAP_LIST->processors_allocated - kernel_info_list[kernel_number].processor_req;
					kernel_release_time = GLOBAL_ALAP_LIST->processor_release_time;

					int processorReleased = kernel_info_list[kernel_number].processor_req;
					int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
					int presentTime = present_time;
					int schedule_method = 1;
#if DEBUG_MESSAGES
					printf(
						"\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||",
						kernel_number);
#endif
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list);
					Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);

					return processors_available;

				}

				else if (GLOBAL_ALAP_LIST->processors_allocated < kernel_info_list[kernel_number].processor_req) {

					while (temp != NULL) {

						if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

							return processors_available;
						}

						else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->processor_release_time) {

							temp = temp->next;

						}

						else if (temp->processor_release_time > GLOBAL_ALAP_LIST->processor_release_time) {

							scheduledNode *t1 = temp;
							scheduledNode *t2 = temp; // Back up

							Pro = GLOBAL_ALAP_LIST->processors_allocated;
							P_Given_list = insert_list(P_Given_list, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;

							do {

								Pro = Pro + t1->processors_allocated;
								t1->processors_allocated = 0;
								P_Given_list = insert_list(P_Given_list,
									t1->processors_allocated);

								if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time)
										> kernel_info_list[kernel_number].deadline) {

									int GLOBAL_GPU_KERNELS = 0;
									scheduledNode* temp1 = t2;
									backup_list* temp2 = P_Given_list;

									while (temp2 != NULL) {

										if (GLOBAL_GPU_KERNELS == 0) {
											GLOBAL_ALAP_LIST->processors_allocated = temp2->data;
											temp2 = temp2->next;
										}

										else {
											temp1->processors_allocated = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;
										}

										GLOBAL_GPU_KERNELS++;

									}

									P_Given_list = clean_list(P_Given_list);

									//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
									printf(
										"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
										kernel_number);
									printf(
										"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
										kernel_number);
#endif
									GLOBAL_CPU_KERNELS++;
									return processors_available;

								}

								else if (Pro >= kernel_info_list[kernel_number].processor_req) {

									t1->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
									kernel_release_time = t1->processor_release_time;

									int processorReleased = kernel_info_list[kernel_number].processor_req;
									int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
									int presentTime = present_time;
									int schedule_method = 1;

#if DEBUG_MESSAGES
									printf("\n||---AEAP_FLAGGED-->The Kernel:%d scheduled AEAP -->---||", kernel_number);
#endif
									P_Given_list = clean_list(P_Given_list);
									Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number,
										processor_alloc_list);
									Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number,
										kernel_queue_list);

									return processors_available;
								}

								t1 = t1->next;

							} while (t1 != NULL);

						}

					} //End of while

				}

			}

			else {

				//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
				printf(
					"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
					kernel_number);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", kernel_number);
#endif
				GLOBAL_CPU_KERNELS++;
				return processors_available;

			}
		} //End of else if
	} //End of GLOBAL_ALAP_LIST->next == NULL

	if (GLOBAL_ALAP_LIST->next != NULL) {

		backup_list* check = GLOBAL_ALAP_LIST->next;
		int Pl = MAX_GPU_PROCESSOR - kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req;

		if (kernel_info_list[kernel_number].processor_req <= Pl) {

			//printf("\n\nDeadline : %d and ALAP kernel_release_time : %d\n\n",kernel_info_list[kernel_number].deadline,check->data );

			if ((kernel_info_list[kernel_number].deadline <= check->data)) {

				while (temp != NULL) {

					if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

						return processors_available;
					}

					else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->data) {

						temp = temp->next;

					}

					else if (temp->processor_release_time > GLOBAL_ALAP_LIST->data) {

						scheduledNode *t1 = temp;

						scheduledNode *t2 = temp; // Back up

						Pro = 0;

						do {

							Pro = Pro + t1->processors_allocated;
							t1->processors_allocated = 0;
							P_Given_list = insert_list(P_Given_list, t1->processors_allocated);

							if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

								scheduledNode* temp1 = t2;
								backup_list* temp2 = P_Given_list;

								while (temp2 != NULL) {

									temp1->processors_allocated = temp2->data;
									temp1 = temp1->next;
									temp2 = temp2->next;

								}

								P_Given_list = clean_list(P_Given_list);

								//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
								printf(
									"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
									kernel_number);
								printf(
									"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
									kernel_number);
#endif
								GLOBAL_CPU_KERNELS++;
								return processors_available;

							}

							else if (Pro >= kernel_info_list[kernel_number].processor_req) {

								t1->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
								kernel_release_time = t1->processor_release_time;

								int processorReleased = kernel_info_list[kernel_number].processor_req;
								int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
								int presentTime = present_time;
								int schedule_method = 1;

								printf(
									"\n||---AEAP_FLAGGED-->The Kernel:%d scheduled AEAP -->---||",
									kernel_number);

								P_Given_list = clean_list(P_Given_list);
								Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number,
									processor_alloc_list);
								Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number,
									kernel_queue_list);

								return processors_available;
							}

							t1 = t1->next;

						} while (t1 != NULL);

					}
				} //End of while
			}

			else {

				//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
				printf(
					"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
					kernel_number);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", kernel_number);
#endif
				GLOBAL_CPU_KERNELS++;
				return processors_available;

			}
		} //End of if

		else if (kernel_info_list[kernel_number].processor_req > Pl) {

			if ((GLOBAL_ALAP_LIST->processor_release_time + kernel_info_list[kernel_number].execution_time) <= kernel_info_list[kernel_number].deadline
				&& kernel_info_list[kernel_number].deadline <= check->data) {

				if (GLOBAL_ALAP_LIST->processors_allocated >= kernel_info_list[kernel_number].processor_req) {

					GLOBAL_ALAP_LIST->processors_allocated = GLOBAL_ALAP_LIST->processors_allocated - kernel_info_list[kernel_number].processor_req;
					kernel_release_time = GLOBAL_ALAP_LIST->processor_release_time;

					int processorReleased = kernel_info_list[kernel_number].processor_req;
					int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
					int presentTime = present_time;
					int schedule_method = 1;
#if DEBUG_MESSAGES
					printf(
						"\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||",
						kernel_number);
#endif
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list);
					Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);

					return processors_available;

				}

				else if (GLOBAL_ALAP_LIST->processors_allocated < kernel_info_list[kernel_number].processor_req) {

					while (temp != NULL) {

						if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

							return processors_available;
						}

						else if (temp->processor_release_time <= GLOBAL_ALAP_LIST->processor_release_time) {

							temp = temp->next;

						}

						else if (temp->processor_release_time > GLOBAL_ALAP_LIST->processor_release_time) {

							scheduledNode *t1 = temp;
							scheduledNode *t2 = temp; // Back up

							Pro = GLOBAL_ALAP_LIST->processors_allocated;
							P_Given_list = insert_list(P_Given_list, GLOBAL_ALAP_LIST->processors_allocated);
							GLOBAL_ALAP_LIST->processors_allocated = 0;

							do {

								Pro = Pro + t1->processors_allocated;
								t1->processors_allocated = 0;
								P_Given_list = insert_list(P_Given_list,
									t1->processors_allocated);

								if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time)
										> kernel_info_list[kernel_number].deadline) {

									int GLOBAL_GPU_KERNELS = 0;
									scheduledNode* temp1 = t2;
									backup_list* temp2 = P_Given_list;

									while (temp2 != NULL) {

										if (GLOBAL_GPU_KERNELS == 0) {
											GLOBAL_ALAP_LIST->processors_allocated = temp2->data;
											temp2 = temp2->next;
										}

										else {
											temp1->processors_allocated = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;
										}

										GLOBAL_GPU_KERNELS++;

									}

									P_Given_list = clean_list(P_Given_list);

									//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
									printf(
										"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
										kernel_number);
									printf(
										"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
										kernel_number);
#endif
									GLOBAL_CPU_KERNELS++;
									return processors_available;

								}

								else if (Pro >= kernel_info_list[kernel_number].processor_req) {

									t1->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
									kernel_release_time = t1->processor_release_time;

									int processorReleased = kernel_info_list[kernel_number].processor_req;
									int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
									int presentTime = present_time;
									int schedule_method = 1;
#if DEBUG_MESSAGES
									printf(
										"\n||---AEAP_FLAGGED-->The Kernel:%d scheduled AEAP -->---||",
										kernel_number);
#endif

									P_Given_list = clean_list(P_Given_list);
									Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number,
										processor_alloc_list);
									Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number,
										kernel_queue_list);

									return processors_available;
								}

								t1 = t1->next;

							} while (t1 != NULL);

						}

					} //End of while

				}

			}

			else {

				//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
				printf(
					"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
					kernel_number);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", kernel_number);

#endif
				GLOBAL_CPU_KERNELS++;
				return processors_available;

			}
		} //End of else if

	} //End of GLOBAL_ALAP_LIST->next != NULL

	return processors_available;
}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_AEAP(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list)
{
	int Pro = 0, kernel_release_time;
	static int given = 0;
	backup_list *P_Given_list = NULL;
	backup_list *P_Given_list_t = NULL;
	scheduledNode* temp = *processor_alloc_list;

	if (GLOBAL_ALAP_LIST == NULL) {

		Pro = processors_available;
		P_Given_list = insert_list(P_Given_list, processors_available);
		processors_available = 0;

		while (temp != NULL) {

			if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

				int GLOBAL_GPU_KERNELS = 0;
				scheduledNode*temp1 = *processor_alloc_list;
				backup_list* temp2 = P_Given_list;
				while (temp2 != NULL) {

					if (GLOBAL_GPU_KERNELS == 0) {
						processors_available = temp2->data;
						temp2 = temp2->next;
					}

					else {
						temp1->processors_allocated = temp2->data;
						temp1 = temp1->next;
						temp2 = temp2->next;
					}

					GLOBAL_GPU_KERNELS++;
				}

				P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
				printf("\n\n||---AEAP-->The Kernel:%d Cannot be scheduled AEAP*****|", kernel_number);
#endif
				GLOBAL_CPU_KERNELS++;

				//Return Kernel to CPU - Function to send Kernel to CPU execution

				return processors_available;
			}

			else {

				Pro = Pro + temp->processors_allocated;

				if (Pro >= kernel_info_list[kernel_number].processor_req) {

					temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
					kernel_release_time = temp->processor_release_time;

					P_Given_list = clean_list(P_Given_list);

					int processorReleased = kernel_info_list[kernel_number].processor_req;
					int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
					int presentTime = present_time;
					int schedule_method = 1;
#if DEBUG_MESSAGES
					printf("\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||", kernel_number);
#endif

					Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list);

					Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);

					return processors_available;

				}

				else if (Pro < kernel_info_list[kernel_number].processor_req) {

					P_Given_list = insert_list(P_Given_list, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}

			}

		}
	} //End of GLOBAL_ALAP_LIST == NULL

	else if (GLOBAL_ALAP_LIST != NULL) {

		Pro = processors_available;
		P_Given_list = insert_list(P_Given_list, processors_available);
		processors_available = 0;

		while (temp != NULL) {

			if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

				int GLOBAL_GPU_KERNELS = 0;
				scheduledNode*temp1 = *processor_alloc_list;
				backup_list* temp2 = P_Given_list;
				while (temp2 != NULL) {

					if (GLOBAL_GPU_KERNELS == 0) {
						processors_available = temp2->data;
						temp2 = temp2->next;
					}

					else {
						temp1->processors_allocated = temp2->data;
						temp1 = temp1->next;
						temp2 = temp2->next;
					}

					GLOBAL_GPU_KERNELS++;
				}

				P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
				printf(
					"\n\n||---AEAP-->The Kernel:%d Cannot be scheduled AEAP*****|",
					kernel_number);
#endif

				GLOBAL_CPU_KERNELS++;
				//Return Kernel to CPU - Function to send Kernel to CPU execution

				return processors_available;
			}

			else {

				Pro = Pro + temp->processors_allocated;

				if (Pro >= kernel_info_list[kernel_number].processor_req) {

					kernel_release_time = temp->processor_release_time;

					int processorReleased = kernel_info_list[kernel_number].processor_req;
					int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
					int presentTime = present_time;
					int schedule_method = 1;

					int Pl = MAX_GPU_PROCESSOR - kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req;

					if (processor_release_time <= GLOBAL_ALAP_LIST->data) {

						temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
						P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
						printf(
							"\n||---AEAP with ALAP Condition-2 -->The Kernel:%d scheduled AEAP -->---||",
							kernel_number);
#endif
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number,
							processor_alloc_list);
						Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);

						return processors_available;
					}

					else if (kernel_info_list[kernel_number].processor_req > Pl && processor_release_time > GLOBAL_ALAP_LIST->data) {

						if (P_Given_list != NULL) {

							int GLOBAL_GPU_KERNELS = 0;
							scheduledNode*temp1 = *processor_alloc_list;
							backup_list* temp2 = P_Given_list;
							while (temp2 != NULL) {

								if (GLOBAL_GPU_KERNELS == 0) {
									processors_available = temp2->data;
									temp2 = temp2->next;
								}

								else {
									temp1->processors_allocated = temp2->data;
									temp1 = temp1->next;
									temp2 = temp2->next;
								}

								GLOBAL_GPU_KERNELS++;
							}
							P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
							printf(
								"\n||---AEAP with ALAP-->Backup processors reloaded-->\n");
#endif
						}

						processors_available = AEAP_Flagged(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list,
							kernel_queue_list);

						//******* Schedule after ALAP NEEDED ********

					}

					else if ((kernel_info_list[kernel_number].processor_req + given) <= Pl
						&& (temp->next == NULL)) {

						temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
						P_Given_list = clean_list(P_Given_list);
						given = kernel_info_list[kernel_number].processor_req;
#if DEBUG_MESSAGES
						printf(
							"\n||---AEAP with ALAP Condition-1 & A -->The Kernel:%d scheduled AEAP -->---||",
							kernel_number);
#endif
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number,
							processor_alloc_list);
						Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);
						return processors_available;
					}

					else if ((kernel_info_list[kernel_number].processor_req + given) <= Pl && (temp->next != NULL)) {
						scheduledNode* check = temp->next;
						if (check->processors_allocated + kernel_info_list[kernel_number].processor_req <= Pl) {

							temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
							P_Given_list = clean_list(P_Given_list);
							given = kernel_info_list[kernel_number].processor_req;
#if DEBUG_MESSAGES
							printf(
								"\n||---AEAP with ALAP Condition-1 & B -->The Kernel:%d scheduled AEAP -->---||",
								kernel_number);
#endif
							Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number,
								processor_alloc_list);
							Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number,
								kernel_queue_list);
							return processors_available;
						}

						else {

							if (kernel_info_list[kernel_number].processor_req <= Pl) {

								scheduledNode *t1 = temp->next;
								scheduledNode *t2 = temp->next; // Back up
								int Pro_t = 0;

								do {

									Pro_t = Pro_t + t1->processors_allocated;
									t1->processors_allocated = 0;
									P_Given_list_t = insert_list(P_Given_list_t,
										t1->processors_allocated);

									if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time)
											> kernel_info_list[kernel_number].deadline) {

										scheduledNode* temp1 = t2;
										backup_list* temp2 = P_Given_list_t;

										while (temp2 != NULL) {

											temp1->processors_allocated = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;

										}

										P_Given_list_t = clean_list(
											P_Given_list_t);

										//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
										printf(
											"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
											kernel_number);
										printf(
											"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
											kernel_number);
#endif
										break;

									}

									else if (Pro_t >= kernel_info_list[kernel_number].processor_req) {

										t1->processors_allocated = Pro_t - kernel_info_list[kernel_number].processor_req;
										kernel_release_time = t1->processor_release_time;

										P_Given_list_t = clean_list(
											P_Given_list_t);

										int processorReleased = kernel_info_list[kernel_number].processor_req;
										int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
										int presentTime = present_time;
										int schedule_method = 1;
#if DEBUG_MESSAGES
										printf(
											"\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||",
											kernel_number);
#endif
										Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method,
											kernel_number, processor_alloc_list);

										Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number,
											kernel_queue_list);
										//return processors_available;
										break;
									}

									t1 = t1->next;
								} while (t1 != NULL);

							}

							if (kernel_info_list[kernel_number].processor_req > Pl) {

								if (P_Given_list != NULL) {

									int GLOBAL_GPU_KERNELS = 0;
									scheduledNode*temp1 = *processor_alloc_list;
									backup_list* temp2 = P_Given_list;
									while (temp2 != NULL) {

										if (GLOBAL_GPU_KERNELS == 0) {
											processors_available = temp2->data;
											temp2 = temp2->next;
										}

										else {
											temp1->processors_allocated = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;
										}

										GLOBAL_GPU_KERNELS++;
									}
									P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
									printf(
										"\n||---AEAP with ALAP-->Backup processors reloaded-->\n");
#endif
								}

								processors_available = ALAP_Flagged(kernel_info_list, kernel_number, present_time, processors_available,
									processor_alloc_list, kernel_queue_list);

								//******* Schedule after ALAP NEEDED ********

							}

						} // temp->next != NULL -- Else end
					} //End temp->next != NULL

					break;

				} //End Pro >= kernel_info_list[kernel_number].processor_req

				else if (Pro < kernel_info_list[kernel_number].processor_req) {

					P_Given_list = insert_list(P_Given_list, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}

			} //End of else
		} //End of while

	} //End of GLOBAL_ALAP_LIST != NULL

	if (P_Given_list != NULL) {

		int GLOBAL_GPU_KERNELS = 0;
		scheduledNode*temp1 = *processor_alloc_list;
		backup_list* temp2 = P_Given_list;
		while (temp2 != NULL) {

			if (GLOBAL_GPU_KERNELS == 0) {
				processors_available = temp2->data;
				temp2 = temp2->next;
			}

			else {
				temp1->processors_allocated = temp2->data;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}

			GLOBAL_GPU_KERNELS++;
		}
		P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
		printf("\n||---AEAP with ALAP-->Backup processors reloaded-->\n");
#endif
	}

	return processors_available;
}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_ALAP_Flagged(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list)
{
#if DEBUG_MESSAGES
	printf("\n||---ALAP_FLAGGED-->Kernel->%d is verified for AEAP Flagged scheduling\n", kernel_number);
#endif
	int Pro = 0, kernel_release_time;
	scheduledNode* temp = *processor_alloc_list;

	backup_list *alap_check = GLOBAL_ALAP_LIST;

	while (alap_check->next != NULL)
		alap_check = alap_check->next;

	if ((alap_check->processor_release_time + kernel_info_list[kernel_number].execution_time) <= kernel_info_list[kernel_number].deadline) {

		if (processors_available == MAX_GPU_PROCESSOR) {

			kernel_release_time = kernel_info_list[kernel_number].deadline - kernel_info_list[kernel_number].execution_time;

			int processorReleased = kernel_info_list[kernel_number].processor_req;
			int processor_release_time = kernel_info_list[kernel_number].deadline;
			int schedule_method = 2;
#if DEBUG_MESSAGES
			printf("\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$",
				kernel_number);
#endif
			GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, processorReleased, kernel_number);
			Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, schedule_method, kernel_number, kernel_queue_list);

			return processors_available;

		}

		else if (alap_check->processors_allocated >= kernel_info_list[kernel_number].processor_req) {

			kernel_release_time = kernel_info_list[kernel_number].deadline - kernel_info_list[kernel_number].execution_time;

			int processorReleased = kernel_info_list[kernel_number].processor_req;
			int processor_release_time = kernel_info_list[kernel_number].deadline;
			int schedule_method = 2;
#if DEBUG_MESSAGES
			printf("\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$",
				kernel_number);
#endif
			GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, processorReleased, kernel_number);
			Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, schedule_method, kernel_number, kernel_queue_list);

			return processors_available;

		}

		else if (alap_check->processors_allocated < kernel_info_list[kernel_number].processor_req) {

			while (temp != NULL) {

				if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {
#if DEBUG_MESSAGES
					printf(
						"\n!!!---ALAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
						kernel_number);
					printf("\n!!!---KERNEL:%d SENT BACK TO CPU -->---!!!", kernel_number);
#endif
					GLOBAL_CPU_KERNELS++;
					return processors_available;
				}

				else if (temp->processor_release_time <= (kernel_info_list[kernel_number].deadline - kernel_info_list[kernel_number].execution_time)) {

					scheduledNode *t1 = temp;
					Pro = alap_check->processors_allocated;

					do {

						Pro = Pro + t1->processors_allocated;

						if (t1->next == NULL && t1->processor_release_time > alap_check->data)
							Pro = Pro + (processors_available - Pro);

						if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

							//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
							printf(
								"\n!!!---ALAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
								kernel_number);
							printf(
								"\n!!!---KERNEL:%d SENT BACK TO CPU -->---!!!",
								kernel_number);
#endif
							GLOBAL_CPU_KERNELS++;
							return processors_available;

						}

						else if (Pro >= kernel_info_list[kernel_number].processor_req) {

							kernel_release_time = kernel_info_list[kernel_number].deadline - kernel_info_list[kernel_number].execution_time;
							int processorReleased = kernel_info_list[kernel_number].processor_req;
							int processor_release_time = kernel_info_list[kernel_number].deadline;
							int schedule_method = 2;
#if DEBUG_MESSAGES
							printf(
								"\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$",
								kernel_number);
#endif

							GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, processorReleased, kernel_number);
							Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, schedule_method, kernel_number,
								kernel_queue_list);

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

	else {

		//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
		printf(
			"\n!!!---ALAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
			kernel_number);
		printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", kernel_number);
#endif
		GLOBAL_CPU_KERNELS++;
		return processors_available;

	}

	return processors_available;

}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_ALAP(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list)
{

	//printf("\n||---AEAP-->Kernel->%d is verified for AEAP scheduling\n",kernel_number);

	int Pro = 0, kernel_release_time, processor_release_time, processors_allocated;

	backup_list *P_Given_list = NULL;

	processors_allocated = kernel_info_list[kernel_number].processor_req;
	processor_release_time = kernel_info_list[kernel_number].deadline;
	kernel_release_time = processor_release_time - kernel_info_list[kernel_number].execution_time;

	Pro = processors_available;

	if (Pro >= kernel_info_list[kernel_number].processor_req) {

		int processorReleased = kernel_info_list[kernel_number].processor_req;
		int schedule_method = 2;
#if DEBUG_MESSAGES
		printf("\n$$---ALAP-->The Kernel:%d scheduled ALAP-->---$$",
			kernel_number);
#endif
		GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, processors_allocated, kernel_number);
		Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, schedule_method, kernel_number, kernel_queue_list);

		return processors_available;

	}


	P_Given_list = insert_ALAP_list(P_Given_list, kernel_release_time, processor_release_time, processors_available, kernel_number);

	scheduledNode* temp = *processor_alloc_list;

	while (temp != NULL) {

		if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) {

			int GLOBAL_GPU_KERNELS = 0;

			scheduledNode*temp1 = *processor_alloc_list;
			backup_list* temp2 = P_Given_list;

			while (temp2 != NULL) {

				if (GLOBAL_GPU_KERNELS == 0)
					temp2 = temp2->next;

				else {
					temp1->kernel_release_time = 0;
					temp1 = temp1->next;
					temp2 = temp2->next;
				}

				GLOBAL_GPU_KERNELS++;
			}

			P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
			printf(
				"\n\n$$---ALAP-->The Kernel:%d Cannot be scheduled ALAP*****$$",
				kernel_number);
#endif
			GLOBAL_CPU_KERNELS++;
			//Return Kernel to CPU - Function to send Kernel to CPU execution

			return processors_available;
		}

		else {

			Pro = Pro + temp->processors_allocated;

			if (Pro >= kernel_info_list[kernel_number].processor_req) {

				temp->kernel_release_time = kernel_release_time;

				P_Given_list = clean_list(P_Given_list);

				int processorReleased = kernel_info_list[kernel_number].processor_req;
				int schedule_method = 2;
#if DEBUG_MESSAGES
				printf("\n$$---ALAP-->The Kernel:%d scheduled ALAP-->---$$",
					kernel_number);
#endif

				GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, processors_allocated, kernel_number);
				Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, schedule_method, kernel_number, kernel_queue_list);

				return processors_available;

			}

			else if (Pro < kernel_info_list[kernel_number].processor_req) {

				P_Given_list = insert_ALAP_list(P_Given_list, kernel_release_time, processor_release_time, temp->processors_allocated, temp->kernel_number);
				temp->kernel_release_time = kernel_release_time;
				temp = temp->next;
			}

		}

	}

	if (temp == NULL && P_Given_list != NULL) {

		int GLOBAL_GPU_KERNELS = 0;
		scheduledNode*temp1 = *processor_alloc_list;
		backup_list* temp2 = P_Given_list;
		while (temp2 != NULL) {

			if (GLOBAL_GPU_KERNELS == 0) {
				temp2 = temp2->next;
			}

			else {
				temp1->kernel_release_time = 0;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}

			GLOBAL_GPU_KERNELS++;
		}

		P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
		printf("$$---ALAP-->The Kernel:%d Cannot be scheduled AEAP --$$", kernel_number);
#endif
	}

	return processors_available;
}