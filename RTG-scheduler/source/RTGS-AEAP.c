/*
* RTGS-AEAP.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP(kernelInfo *kernel_info_list, int kernel_number, int present_time, int processors_available, scheduledNode ** processor_alloc_list, scheduledNode **kernel_queue_list) {

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
				printf("\n\n||---AEAP-->The Kernel:%d Cannot be scheduled AEAP*****|", kernel_number);
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
						printf("\n||---AEAP with ALAP Condition-2 -->The Kernel:%d scheduled AEAP -->---||", kernel_number);
#endif
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number,
							processor_alloc_list);
						Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);

						return processors_available;
					}

					else if (kernel_info_list[kernel_number].processor_req > Pl && processor_release_time > GLOBAL_ALAP_LIST->data) {


						if (Pro >= kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req && kernel_release_time < GLOBAL_ALAP_LIST->data) {

							//Improve ALAP release time

							processors_available = AEAP_ALAP_improve(kernel_info_list, kernel_release_time, present_time, processors_available, processor_alloc_list, kernel_queue_list);


						}



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


						processors_available = AEAP_Flagged(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list,
							kernel_queue_list);



					}

					else if ((kernel_info_list[kernel_number].processor_req + given) <= Pl
						&& (temp->next == NULL)) {

						temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
						P_Given_list = clean_list(P_Given_list);
						given = kernel_info_list[kernel_number].processor_req;

#if DEBUG_MESSAGES
						printf("\n||---AEAP with ALAP Condition-1 &  -->The Kernel:%d scheduled AEAP -->---||", kernel_number);
#endif
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number,
							processor_alloc_list);
						Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);
						return processors_available;
					}

					else if ((kernel_info_list[kernel_number].processor_req + given) <= Pl
						&& (temp->next != NULL)) {
						scheduledNode* check = temp->next;
						if (check->processors_allocated + kernel_info_list[kernel_number].processor_req <= Pl) {

							temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
							P_Given_list = clean_list(P_Given_list);
							given = kernel_info_list[kernel_number].processor_req;

#if DEBUG_MESSAGES
							printf("\n||---AEAP with ALAP Condition-1 &  -->The Kernel:%d scheduled AEAP -->---||", kernel_number);
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
										printf("\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!", kernel_number);
										printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", kernel_number);
#endif

										GLOBAL_CPU_KERNELS++;
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
										printf("\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||", kernel_number);
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
									printf("\n||---AEAP with ALAP-->Backup processors reloaded-->\n");
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
