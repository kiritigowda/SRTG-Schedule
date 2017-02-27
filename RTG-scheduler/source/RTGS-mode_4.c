/*
* RTG-mode_4.c
*      Author: Kiriti Nagesh Gowda
*/
#include"RTGS.h"

/**********************************************************************************************************
RTGS Mode 4 - - AEAP/ALAP Bin Packer mode->AEAP/ALAP Pack
***********************************************************************************************************/
int RTGS_mode_4(char *kernel_file, char *Releasetime_file) {

	Kernel_INFO kernel[MAX_KERNELS];
	Node *Pro_free_list = NULL; //*Pro_release_list=NULL;
	Node *Kernel_queue = NULL;  //Kernel queued for future executions
	int Pa = MAX_GPU_PROCESSOR;
	int c = 0, Nkr = 0;
	int rt = 0, KN = 0, k1, k2;
	count = 0;
	CPU_Kernel = 0;
	alap = NULL;
	Nkr = Get_kernel_information(kernel, kernel_file);                 				// Read Kernel.TXT
	rt = Get_kernel_release_times(Releasetime_file);                   				// Read Release_time.TXT
#if DEBUG_MESSAGES
	printf("\nThe GPU Scheduler will Schedule %d Kernels\n\n", Nkr);				// Scheduler Begins
#endif
	int64_t stime = RTGS_GetClockCounter();
	for (int i = 0; i < rt; i++)
	{
		Pa = Retrieve_processors(i, Pa, &Pro_free_list);     // Freeing-up processors
		Pa = Dispatch_queued_kernels(i, Pa, &Kernel_queue, &Pro_free_list);     // Freeing-up processors
		if (RT[i] == 1)
		{
#if DEBUG_MESSAGES
			printf("\n-->>Total processors Available at time %d = %d\n\n ", i, Pa);
			printf("Kernels:%d has been released\n", KN);
#endif
			Pa = Mode_4_book_keeper(kernel, KN, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
			KN++;
		}
		else if (RT[i] == 2)
		{
			k1 = KN;KN++;
			k2 = KN;KN++;
#if DEBUG_MESSAGES
			printf("\n-->>Total processors Available at time %d = %d\n\n ", i, Pa)
				printf("Kernels:%d has been released\n", k1);
			printf("Kernels:%d has been released\n", k2);
#endif
			if (kernel[k1].Td <= kernel[k2].Td)
			{
				Pa = Mode_4_book_keeper(kernel, k1, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
				Pa = Mode_4_book_keeper(kernel, k2, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
			}
			else
			{
				Pa = Mode_4_book_keeper(kernel, k2, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
				Pa = Mode_4_book_keeper(kernel, k1, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
			}
		}
	}
	Pro_free_list = clean_node(Pro_free_list);
	alap = clean_list(alap);
	if (rt != 0)
	{
#if DEBUG_INFO
		printf("All Kernels Scheduled or Sent to CPU Successfully - Processors Available: %d Mode_2_AEAP Kernels: %d\n", Pa, count);
		printf("Kernels sent Back to CPU: %d\n", CPU_Kernel);
#endif
		for (int j = 0; j <= Nkr; j++)
		{
			kernel[j].Pn = kernel[j].Td = kernel[j].Texe = kernel[j].Tls = 0;
		}
		Nkr = 0;
		rt = 0;
		KN = 0;
		count = 0;
		CPU_Kernel = 0;
	}
#if DEBUG_MESSAGES
	print(Pro_free_list);
#endif
	Pro_free_list = clean_node(Pro_free_list);
	return RTGS_SUCCESS;
}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_book_keeper(Kernel_INFO* kernel, int KN, int Pa, int i, Node **Pro_free_list, Node **Kernel_queue)
{

	int Pf, Tf, Pt = i, SA = 99;
	static int FLAG = 0, FLAG_V = 0;

#if DEBUG_MESSAGES
	printf("\n ^^ Kernel[%d].Pn = %d, Texe = %d, Td = %d, Tls= %d ^^\n", KN, kernel[KN].Pn, kernel[KN].Texe, kernel[KN].Td, kernel[KN].Tls);
#endif

	if (kernel[KN].Pn <= Pa) // If processors available is greater than the required processors by the kernel
	{
		if (alap == NULL) { //ALAP not set
			FLAG = 0;
			FLAG_V = 0;

			if (kernel[KN].Pn < PROCESSOR_LIMIT) { // Processors needed lesser than the limit

				if (kernel[KN].Texe + Pt <= kernel[KN].Td) {

					Pa = Pa - kernel[KN].Pn;
					Pf = kernel[KN].Pn;
					Tf = kernel[KN].Texe + Pt;
					SA = 0;
					Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
				}

				else {
#if DEBUG_MESSAGES
					printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", KN);
#endif
					CPU_Kernel++;
				}
			}

			else if (kernel[KN].Pn >= PROCESSOR_LIMIT) { // Processors needed greater or equal than the limit
#if DEBUG_MESSAGES
				printf("\n>>>>>Kernel:%d is compute intensive, sent for ALAP execution\n", KN);
#endif
				Pa = Mode_4_ALAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);
			}
		}
		else if (alap != NULL) { //ALAP set
			if (FLAG_V != alap->data) { //ALAP updated
				FLAG = 0;
				FLAG_V = 0;
			}
			int Pl = MAX_GPU_PROCESSOR - kernel[alap->KN].Pn;
			if (kernel[KN].Pn < PROCESSOR_LIMIT) { // Processors needed lesser than the limit

				if (kernel[KN].Pn <= Pl && (Pt + kernel[KN].Texe) <= alap->data) { // Condition 1
#if DEBUG_MESSAGES
					printf("\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 1 && 2", KN);
#endif
					if (kernel[KN].Texe + Pt <= kernel[KN].Td) {

						Pa = Pa - kernel[KN].Pn;
						Pf = kernel[KN].Pn;
						Tf = kernel[KN].Texe + Pt;
						SA = 0;
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}
					else {
#if DEBUG_MESSAGES
						printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", KN);
#endif
					}
				}

				else if (kernel[KN].Pn > Pl
					&& (Pt + kernel[KN].Texe) <= alap->data) { // Condition 2
#if DEBUG_MESSAGES
					printf("\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 2", KN);
#endif
					if (kernel[KN].Texe + Pt <= kernel[KN].Td) {

						Pa = Pa - kernel[KN].Pn;
						Pf = kernel[KN].Pn;
						Tf = kernel[KN].Texe + Pt;
						SA = 0;
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}

					else {
#if DEBUG_MESSAGES
						printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", KN);
#endif
						CPU_Kernel++;
					}
				}

				else if (kernel[KN].Pn <= Pl
					&& (Pt + kernel[KN].Texe) > alap->data && FLAG == 0) { // Condition 3
				   // Control flags to not allow over budgeting of PA
					FLAG = 1;
					FLAG_V = alap->data;
#if DEBUG_MESSAGES
					printf("\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 1 with FLAG", KN);
#endif
					if (kernel[KN].Texe + Pt <= kernel[KN].Td) {
						Pa = Pa - kernel[KN].Pn;
						Pf = kernel[KN].Pn;
						Tf = kernel[KN].Texe + Pt;
						SA = 0;
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}
					else {
#if DEBUG_MESSAGES
						printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", KN);
#endif
						CPU_Kernel++;
					}
				}
				else {
					Pa = AEAP_Flagged(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);
					count++;
				}
			}
			else if (kernel[KN].Pn >= PROCESSOR_LIMIT) { // Processors needed greater or equal than the limit
				Pa = ALAP_Flagged(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);
				count++;
			}
		}
	}
	else if (kernel[KN].Pn > Pa) // If processors available is lesser than the required processors by the kernel
	{
		Pa = Mode_4_Processors_unavailable(kernel, KN, i, Pa, Pro_free_list,
			Kernel_queue); // Schedule the kernel to be released in a future time

	}

	return Pa;

}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_Processors_unavailable(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue)
{
	//printf("\nKernel->%d sent for AEAP Scheduling at TIME: %d\n",KN,i);
	if (kernel[KN].Pn < PROCESSOR_LIMIT) {
		Pa = Mode_4_AEAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);
		count++;
	}
	else if (kernel[KN].Pn >= PROCESSOR_LIMIT && alap == NULL) {
#if DEBUG_MESSAGES
		printf("\n>>>>>Kernel:%d sent for ALAP execution\n", KN);
#endif
		Pa = Mode_4_ALAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);
	}
	else if (kernel[KN].Pn >= PROCESSOR_LIMIT && alap != NULL) {
#if DEBUG_MESSAGES
		printf("\n>>>>>Kernel:%d sent for ALAP execution\n", KN);
#endif
		Pa = Mode_4_ALAP_Flagged(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);
	}
	return Pa;
}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_AEAP_Flagged(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue)
{
#if DEBUG_MESSAGES
	printf("\n||---AEAP_FLAGGED-->Kernel->%d is verified for ALAP Flagged scheduling\n", KN);
#endif

	int Pro = 0, Tr = 0;

	backup_list *P_Given_list = NULL;
	Node* temp = *Pro_free_list;

	if (alap->next == NULL) {
		int Pl = MAX_GPU_PROCESSOR - kernel[alap->KN].Pn;
		if (kernel[KN].Pn <= Pl) {
			while (temp != NULL) {
				if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {
					return Pa;
				}
				else if (temp->Tf <= alap->data) {
					temp = temp->next;
				}
				else if (temp->Tf > alap->data) {
					Node *t1 = temp;
					Node *t2 = temp; // Back up
					Pro = 0;

					do {

						Pro = Pro + t1->P_f_g;
						t1->P_f_g = 0;
						P_Given_list = insert_list(P_Given_list, t1->P_f_g);

						if ((t1->Tf + kernel[KN].Texe) > kernel[KN].Td) {

							Node* temp1 = t2;
							backup_list* temp2 = P_Given_list;

							while (temp2 != NULL) {

								temp1->P_f_g = temp2->data;
								temp1 = temp1->next;
								temp2 = temp2->next;

							}

							P_Given_list = clean_list(P_Given_list);

							//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
							printf("\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!", KN);
							printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
#endif
							CPU_Kernel++;
							return Pa;

						}

						else if (Pro >= kernel[KN].Pn) {

							t1->P_f_g = Pro - kernel[KN].Pn;
							Tr = t1->Tf;

							int Pf = kernel[KN].Pn;
							int Tf = Tr + kernel[KN].Texe;
							int Pt = i;
							int SA = 1;

#if DEBUG_MESSAGES
							printf("\n||---AEAP_FLAGGED-->The Kernel:%d scheduled AEAP -->---||", KN);
#endif

							P_Given_list = clean_list(P_Given_list);
							Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list);
							Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);

							return Pa;
						}

						t1 = t1->next;

					} while (t1 != NULL);

				}

			} //End of while

		} //End of if

		else if (kernel[KN].Pn > Pl) {

			if ((alap->Tf + kernel[KN].Texe) <= kernel[KN].Td) {

				if (alap->Pg >= kernel[KN].Pn) {

					alap->Pg = alap->Pg - kernel[KN].Pn;
					Tr = alap->Tf;

					int Pf = kernel[KN].Pn;
					int Tf = Tr + kernel[KN].Texe;
					int Pt = i;
					int SA = 1;
#if DEBUG_MESSAGES
					printf(
						"\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||",
						KN);
#endif
					Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list);
					Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);

					return Pa;

				}

				else if (alap->Pg < kernel[KN].Pn) {

					while (temp != NULL) {

						if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {

							return Pa;
						}

						else if (temp->Tf <= alap->Tf) {

							temp = temp->next;

						}

						else if (temp->Tf > alap->Tf) {

							Node *t1 = temp;
							Node *t2 = temp; // Back up

							Pro = alap->Pg;
							P_Given_list = insert_list(P_Given_list, alap->Pg);
							alap->Pg = 0;

							do {

								Pro = Pro + t1->P_f_g;
								t1->P_f_g = 0;
								P_Given_list = insert_list(P_Given_list,
									t1->P_f_g);

								if ((t1->Tf + kernel[KN].Texe)
										> kernel[KN].Td) {

									int count = 0;
									Node* temp1 = t2;
									backup_list* temp2 = P_Given_list;

									while (temp2 != NULL) {

										if (count == 0) {
											alap->Pg = temp2->data;
											temp2 = temp2->next;
										}

										else {
											temp1->P_f_g = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;
										}

										count++;

									}

									P_Given_list = clean_list(P_Given_list);

									//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
									printf(
										"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
										KN);
									printf(
										"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
										KN);
#endif
									CPU_Kernel++;
									return Pa;

								}

								else if (Pro >= kernel[KN].Pn) {

									t1->P_f_g = Pro - kernel[KN].Pn;
									Tr = t1->Tf;

									int Pf = kernel[KN].Pn;
									int Tf = Tr + kernel[KN].Texe;
									int Pt = i;
									int SA = 1;

#if DEBUG_MESSAGES
									printf("\n||---AEAP_FLAGGED-->The Kernel:%d scheduled AEAP -->---||", KN);
#endif
									P_Given_list = clean_list(P_Given_list);
									Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
										Pro_free_list);
									Kernel_queue_handler(Pf, Tr, Pt, SA, KN,
										Kernel_queue);

									return Pa;
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
					KN);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
#endif
				CPU_Kernel++;
				return Pa;

			}
		} //End of else if
	} //End of alap->next == NULL

	if (alap->next != NULL) {

		backup_list* check = alap->next;
		int Pl = MAX_GPU_PROCESSOR - kernel[alap->KN].Pn;

		if (kernel[KN].Pn <= Pl) {

			//printf("\n\nDeadline : %d and ALAP Tr : %d\n\n",kernel[KN].Td,check->data );

			if ((kernel[KN].Td <= check->data)) {

				while (temp != NULL) {

					if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {

						return Pa;
					}

					else if (temp->Tf <= alap->data) {

						temp = temp->next;

					}

					else if (temp->Tf > alap->data) {

						Node *t1 = temp;

						Node *t2 = temp; // Back up

						Pro = 0;

						do {

							Pro = Pro + t1->P_f_g;
							t1->P_f_g = 0;
							P_Given_list = insert_list(P_Given_list, t1->P_f_g);

							if ((t1->Tf + kernel[KN].Texe) > kernel[KN].Td) {

								Node* temp1 = t2;
								backup_list* temp2 = P_Given_list;

								while (temp2 != NULL) {

									temp1->P_f_g = temp2->data;
									temp1 = temp1->next;
									temp2 = temp2->next;

								}

								P_Given_list = clean_list(P_Given_list);

								//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
								printf(
									"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
									KN);
								printf(
									"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
									KN);
#endif
								CPU_Kernel++;
								return Pa;

							}

							else if (Pro >= kernel[KN].Pn) {

								t1->P_f_g = Pro - kernel[KN].Pn;
								Tr = t1->Tf;

								int Pf = kernel[KN].Pn;
								int Tf = Tr + kernel[KN].Texe;
								int Pt = i;
								int SA = 1;

								printf(
									"\n||---AEAP_FLAGGED-->The Kernel:%d scheduled AEAP -->---||",
									KN);

								P_Given_list = clean_list(P_Given_list);
								Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
									Pro_free_list);
								Kernel_queue_handler(Pf, Tr, Pt, SA, KN,
									Kernel_queue);

								return Pa;
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
					KN);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
#endif
				CPU_Kernel++;
				return Pa;

			}
		} //End of if

		else if (kernel[KN].Pn > Pl) {

			if ((alap->Tf + kernel[KN].Texe) <= kernel[KN].Td
				&& kernel[KN].Td <= check->data) {

				if (alap->Pg >= kernel[KN].Pn) {

					alap->Pg = alap->Pg - kernel[KN].Pn;
					Tr = alap->Tf;

					int Pf = kernel[KN].Pn;
					int Tf = Tr + kernel[KN].Texe;
					int Pt = i;
					int SA = 1;
#if DEBUG_MESSAGES
					printf(
						"\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||",
						KN);
#endif
					Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list);
					Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);

					return Pa;

				}

				else if (alap->Pg < kernel[KN].Pn) {

					while (temp != NULL) {

						if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {

							return Pa;
						}

						else if (temp->Tf <= alap->Tf) {

							temp = temp->next;

						}

						else if (temp->Tf > alap->Tf) {

							Node *t1 = temp;
							Node *t2 = temp; // Back up

							Pro = alap->Pg;
							P_Given_list = insert_list(P_Given_list, alap->Pg);
							alap->Pg = 0;

							do {

								Pro = Pro + t1->P_f_g;
								t1->P_f_g = 0;
								P_Given_list = insert_list(P_Given_list,
									t1->P_f_g);

								if ((t1->Tf + kernel[KN].Texe)
										> kernel[KN].Td) {

									int count = 0;
									Node* temp1 = t2;
									backup_list* temp2 = P_Given_list;

									while (temp2 != NULL) {

										if (count == 0) {
											alap->Pg = temp2->data;
											temp2 = temp2->next;
										}

										else {
											temp1->P_f_g = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;
										}

										count++;

									}

									P_Given_list = clean_list(P_Given_list);

									//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
									printf(
										"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
										KN);
									printf(
										"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
										KN);
#endif
									CPU_Kernel++;
									return Pa;

								}

								else if (Pro >= kernel[KN].Pn) {

									t1->P_f_g = Pro - kernel[KN].Pn;
									Tr = t1->Tf;

									int Pf = kernel[KN].Pn;
									int Tf = Tr + kernel[KN].Texe;
									int Pt = i;
									int SA = 1;
#if DEBUG_MESSAGES
									printf(
										"\n||---AEAP_FLAGGED-->The Kernel:%d scheduled AEAP -->---||",
										KN);
#endif

									P_Given_list = clean_list(P_Given_list);
									Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
										Pro_free_list);
									Kernel_queue_handler(Pf, Tr, Pt, SA, KN,
										Kernel_queue);

									return Pa;
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
					KN);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);

#endif
				CPU_Kernel++;
				return Pa;

			}
		} //End of else if

	} //End of alap->next != NULL

	return Pa;
}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_AEAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue)
{
	int Pro = 0, Tr;
	static int given = 0;
	backup_list *P_Given_list = NULL;
	backup_list *P_Given_list_t = NULL;
	Node* temp = *Pro_free_list;

	if (alap == NULL) {

		Pro = Pa;
		P_Given_list = insert_list(P_Given_list, Pa);
		Pa = 0;

		while (temp != NULL) {

			if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {

				int count = 0;
				Node*temp1 = *Pro_free_list;
				backup_list* temp2 = P_Given_list;
				while (temp2 != NULL) {

					if (count == 0) {
						Pa = temp2->data;
						temp2 = temp2->next;
					}

					else {
						temp1->P_f_g = temp2->data;
						temp1 = temp1->next;
						temp2 = temp2->next;
					}

					count++;
				}

				P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
				printf("\n\n||---AEAP-->The Kernel:%d Cannot be scheduled AEAP*****|", KN);
#endif
				CPU_Kernel++;

				//Return Kernel to CPU - Function to send Kernel to CPU execution

				return Pa;
			}

			else {

				Pro = Pro + temp->P_f_g;

				if (Pro >= kernel[KN].Pn) {

					temp->P_f_g = Pro - kernel[KN].Pn;
					Tr = temp->Tf;

					P_Given_list = clean_list(P_Given_list);

					int Pf = kernel[KN].Pn;
					int Tf = Tr + kernel[KN].Texe;
					int Pt = i;
					int SA = 1;
#if DEBUG_MESSAGES
					printf("\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||", KN);
#endif

					Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list);

					Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);

					return Pa;

				}

				else if (Pro < kernel[KN].Pn) {

					P_Given_list = insert_list(P_Given_list, temp->P_f_g);
					temp->P_f_g = 0;
					temp = temp->next;
				}

			}

		}
	} //End of alap == NULL

	else if (alap != NULL) {

		Pro = Pa;
		P_Given_list = insert_list(P_Given_list, Pa);
		Pa = 0;

		while (temp != NULL) {

			if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {

				int count = 0;
				Node*temp1 = *Pro_free_list;
				backup_list* temp2 = P_Given_list;
				while (temp2 != NULL) {

					if (count == 0) {
						Pa = temp2->data;
						temp2 = temp2->next;
					}

					else {
						temp1->P_f_g = temp2->data;
						temp1 = temp1->next;
						temp2 = temp2->next;
					}

					count++;
				}

				P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
				printf(
					"\n\n||---AEAP-->The Kernel:%d Cannot be scheduled AEAP*****|",
					KN);
#endif

				CPU_Kernel++;
				//Return Kernel to CPU - Function to send Kernel to CPU execution

				return Pa;
			}

			else {

				Pro = Pro + temp->P_f_g;

				if (Pro >= kernel[KN].Pn) {

					Tr = temp->Tf;

					int Pf = kernel[KN].Pn;
					int Tf = Tr + kernel[KN].Texe;
					int Pt = i;
					int SA = 1;

					int Pl = MAX_GPU_PROCESSOR - kernel[alap->KN].Pn;

					if (Tf <= alap->data) {

						temp->P_f_g = Pro - kernel[KN].Pn;
						P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
						printf(
							"\n||---AEAP with ALAP Condition-2 -->The Kernel:%d scheduled AEAP -->---||",
							KN);
#endif
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list);
						Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);

						return Pa;
					}

					else if (kernel[KN].Pn > Pl && Tf > alap->data) {

						if (P_Given_list != NULL) {

							int count = 0;
							Node*temp1 = *Pro_free_list;
							backup_list* temp2 = P_Given_list;
							while (temp2 != NULL) {

								if (count == 0) {
									Pa = temp2->data;
									temp2 = temp2->next;
								}

								else {
									temp1->P_f_g = temp2->data;
									temp1 = temp1->next;
									temp2 = temp2->next;
								}

								count++;
							}
							P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
							printf(
								"\n||---AEAP with ALAP-->Backup processors reloaded-->\n");
#endif
						}

						Pa = AEAP_Flagged(kernel, KN, i, Pa, Pro_free_list,
							Kernel_queue);

						//******* Schedule after ALAP NEEDED ********

					}

					else if ((kernel[KN].Pn + given) <= Pl
						&& (temp->next == NULL)) {

						temp->P_f_g = Pro - kernel[KN].Pn;
						P_Given_list = clean_list(P_Given_list);
						given = kernel[KN].Pn;
#if DEBUG_MESSAGES
						printf(
							"\n||---AEAP with ALAP Condition-1 & A -->The Kernel:%d scheduled AEAP -->---||",
							KN);
#endif
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list);
						Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);
						return Pa;
					}

					else if ((kernel[KN].Pn + given) <= Pl && (temp->next != NULL)) {
						Node* check = temp->next;
						if (check->P_f_g + kernel[KN].Pn <= Pl) {

							temp->P_f_g = Pro - kernel[KN].Pn;
							P_Given_list = clean_list(P_Given_list);
							given = kernel[KN].Pn;
#if DEBUG_MESSAGES
							printf(
								"\n||---AEAP with ALAP Condition-1 & B -->The Kernel:%d scheduled AEAP -->---||",
								KN);
#endif
							Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
								Pro_free_list);
							Kernel_queue_handler(Pf, Tr, Pt, SA, KN,
								Kernel_queue);
							return Pa;
						}

						else {

							if (kernel[KN].Pn <= Pl) {

								Node *t1 = temp->next;
								Node *t2 = temp->next; // Back up
								int Pro_t = 0;

								do {

									Pro_t = Pro_t + t1->P_f_g;
									t1->P_f_g = 0;
									P_Given_list_t = insert_list(P_Given_list_t,
										t1->P_f_g);

									if ((t1->Tf + kernel[KN].Texe)
											> kernel[KN].Td) {

										Node* temp1 = t2;
										backup_list* temp2 = P_Given_list_t;

										while (temp2 != NULL) {

											temp1->P_f_g = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;

										}

										P_Given_list_t = clean_list(
											P_Given_list_t);

										//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
										printf(
											"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
											KN);
										printf(
											"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
											KN);
#endif
										break;

									}

									else if (Pro_t >= kernel[KN].Pn) {

										t1->P_f_g = Pro_t - kernel[KN].Pn;
										Tr = t1->Tf;

										P_Given_list_t = clean_list(
											P_Given_list_t);

										int Pf = kernel[KN].Pn;
										int Tf = Tr + kernel[KN].Texe;
										int Pt = i;
										int SA = 1;
#if DEBUG_MESSAGES
										printf(
											"\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||",
											KN);
#endif
										Queue_kernel_execution(Pf, Tf, Pt, SA,
											KN, Pro_free_list);

										Kernel_queue_handler(Pf, Tr, Pt, SA, KN,
											Kernel_queue);
										//return Pa;
										break;
									}

									t1 = t1->next;
								} while (t1 != NULL);

							}

							if (kernel[KN].Pn > Pl) {

								if (P_Given_list != NULL) {

									int count = 0;
									Node*temp1 = *Pro_free_list;
									backup_list* temp2 = P_Given_list;
									while (temp2 != NULL) {

										if (count == 0) {
											Pa = temp2->data;
											temp2 = temp2->next;
										}

										else {
											temp1->P_f_g = temp2->data;
											temp1 = temp1->next;
											temp2 = temp2->next;
										}

										count++;
									}
									P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
									printf(
										"\n||---AEAP with ALAP-->Backup processors reloaded-->\n");
#endif
								}

								Pa = ALAP_Flagged(kernel, KN, i, Pa,
									Pro_free_list, Kernel_queue);

								//******* Schedule after ALAP NEEDED ********

							}

						} // temp->next != NULL -- Else end
					} //End temp->next != NULL

					break;

				} //End Pro >= kernel[KN].Pn

				else if (Pro < kernel[KN].Pn) {

					P_Given_list = insert_list(P_Given_list, temp->P_f_g);
					temp->P_f_g = 0;
					temp = temp->next;
				}

			} //End of else
		} //End of while

	} //End of alap != NULL

	if (P_Given_list != NULL) {

		int count = 0;
		Node*temp1 = *Pro_free_list;
		backup_list* temp2 = P_Given_list;
		while (temp2 != NULL) {

			if (count == 0) {
				Pa = temp2->data;
				temp2 = temp2->next;
			}

			else {
				temp1->P_f_g = temp2->data;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}

			count++;
		}
		P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
		printf("\n||---AEAP with ALAP-->Backup processors reloaded-->\n");
#endif
	}

	return Pa;
}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_ALAP_Flagged(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue)
{
#if DEBUG_MESSAGES
	printf("\n||---ALAP_FLAGGED-->Kernel->%d is verified for AEAP Flagged scheduling\n", KN);
#endif
	int Pro = 0, Tr;
	Node* temp = *Pro_free_list;

	backup_list *alap_check = alap;

	while (alap_check->next != NULL)
		alap_check = alap_check->next;

	if ((alap_check->Tf + kernel[KN].Texe) <= kernel[KN].Td) {

		if (Pa == MAX_GPU_PROCESSOR) {

			Tr = kernel[KN].Td - kernel[KN].Texe;

			int Pf = kernel[KN].Pn;
			int Tf = kernel[KN].Td;
			int SA = 2;
#if DEBUG_MESSAGES
			printf("\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$",
				KN);
#endif
			alap = insert_ALAP_list(alap, Tr, Tf, Pf, KN);
			Kernel_queue_handler(Pf, Tr, Tf, SA, KN, Kernel_queue);

			return Pa;

		}

		else if (alap_check->Pg >= kernel[KN].Pn) {

			Tr = kernel[KN].Td - kernel[KN].Texe;

			int Pf = kernel[KN].Pn;
			int Tf = kernel[KN].Td;
			int SA = 2;
#if DEBUG_MESSAGES
			printf("\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$",
				KN);
#endif
			alap = insert_ALAP_list(alap, Tr, Tf, Pf, KN);
			Kernel_queue_handler(Pf, Tr, Tf, SA, KN, Kernel_queue);

			return Pa;

		}

		else if (alap_check->Pg < kernel[KN].Pn) {

			while (temp != NULL) {

				if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {
#if DEBUG_MESSAGES
					printf(
						"\n!!!---ALAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
						KN);
					printf("\n!!!---KERNEL:%d SENT BACK TO CPU -->---!!!", KN);
#endif
					CPU_Kernel++;
					return Pa;
				}

				else if (temp->Tf <= (kernel[KN].Td - kernel[KN].Texe)) {

					Node *t1 = temp;
					Pro = alap_check->Pg;

					do {

						Pro = Pro + t1->P_f_g;

						if (t1->next == NULL && t1->Tf > alap_check->data)
							Pro = Pro + (Pa - Pro);

						if ((t1->Tf + kernel[KN].Texe) > kernel[KN].Td) {

							//Kernel has to be sent to CPU
#if DEBUG_MESSAGES
							printf(
								"\n!!!---ALAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
								KN);
							printf(
								"\n!!!---KERNEL:%d SENT BACK TO CPU -->---!!!",
								KN);
#endif
							CPU_Kernel++;
							return Pa;

						}

						else if (Pro >= kernel[KN].Pn) {

							Tr = kernel[KN].Td - kernel[KN].Texe;
							int Pf = kernel[KN].Pn;
							int Tf = kernel[KN].Td;
							int SA = 2;
#if DEBUG_MESSAGES
							printf(
								"\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$",
								KN);
#endif

							alap = insert_ALAP_list(alap, Tr, Tf, Pf, KN);
							Kernel_queue_handler(Pf, Tr, Tf, SA, KN,
								Kernel_queue);

							return Pa;
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
			KN);
		printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
#endif
		CPU_Kernel++;
		return Pa;

	}

	return Pa;

}

/**********************************************************************************************************
MODE 4 FUNCTION
***********************************************************************************************************/
int Mode_4_ALAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue)
{

	//printf("\n||---AEAP-->Kernel->%d is verified for AEAP scheduling\n",KN);

	int Pro = 0, Tr, Tf, Pg;

	backup_list *P_Given_list = NULL;

	Pg = kernel[KN].Pn;
	Tf = kernel[KN].Td;
	Tr = Tf - kernel[KN].Texe;

	Pro = Pa;

	if (Pro >= kernel[KN].Pn) {

		int Pf = kernel[KN].Pn;
		int SA = 2;
#if DEBUG_MESSAGES
		printf("\n$$---ALAP-->The Kernel:%d scheduled ALAP-->---$$",
			KN);
#endif
		alap = insert_ALAP_list(alap, Tr, Tf, Pg, KN);
		Kernel_queue_handler(Pf, Tr, Tf, SA, KN, Kernel_queue);

		return Pa;

	}


	P_Given_list = insert_ALAP_list(P_Given_list, Tr, Tf, Pa, KN);

	Node* temp = *Pro_free_list;

	while (temp != NULL) {

		if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {

			int count = 0;

			Node*temp1 = *Pro_free_list;
			backup_list* temp2 = P_Given_list;

			while (temp2 != NULL) {

				if (count == 0)
					temp2 = temp2->next;

				else {
					temp1->Tr = 0;
					temp1 = temp1->next;
					temp2 = temp2->next;
				}

				count++;
			}

			P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
			printf(
				"\n\n$$---ALAP-->The Kernel:%d Cannot be scheduled ALAP*****$$",
				KN);
#endif
			CPU_Kernel++;
			//Return Kernel to CPU - Function to send Kernel to CPU execution

			return Pa;
		}

		else {

			Pro = Pro + temp->P_f_g;

			if (Pro >= kernel[KN].Pn) {

				temp->Tr = Tr;

				P_Given_list = clean_list(P_Given_list);

				int Pf = kernel[KN].Pn;
				int SA = 2;
#if DEBUG_MESSAGES
				printf("\n$$---ALAP-->The Kernel:%d scheduled ALAP-->---$$",
					KN);
#endif

				alap = insert_ALAP_list(alap, Tr, Tf, Pg, KN);
				Kernel_queue_handler(Pf, Tr, Tf, SA, KN, Kernel_queue);

				return Pa;

			}

			else if (Pro < kernel[KN].Pn) {

				P_Given_list = insert_ALAP_list(P_Given_list, Tr, Tf, temp->P_f_g, temp->KN);
				temp->Tr = Tr;
				temp = temp->next;
			}

		}

	}

	if (temp == NULL && P_Given_list != NULL) {

		int count = 0;
		Node*temp1 = *Pro_free_list;
		backup_list* temp2 = P_Given_list;
		while (temp2 != NULL) {

			if (count == 0) {
				temp2 = temp2->next;
			}

			else {
				temp1->Tr = 0;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}

			count++;
		}

		P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
		printf("$$---ALAP-->The Kernel:%d Cannot be scheduled AEAP --$$", KN);
#endif
	}

	return Pa;
}