/*
* RTGS-AEAP.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

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
				printf("\n\n||---AEAP-->The Kernel:%d Cannot be scheduled AEAP*****|", KN);
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
						printf("\n||---AEAP with ALAP Condition-2 -->The Kernel:%d scheduled AEAP -->---||", KN);
#endif
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list);
						Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);

						return Pa;
					}

					else if (kernel[KN].Pn > Pl && Tf > alap->data) {


						if (Pro >= kernel[alap->KN].Pn && Tr < alap->data) {

							//Improve ALAP release time

							Pa = AEAP_ALAP_improve(kernel, Tr, i, Pa, Pro_free_list, Kernel_queue);


						}



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


						Pa = AEAP_Flagged(kernel, KN, i, Pa, Pro_free_list,
							Kernel_queue);



						}

					else if ((kernel[KN].Pn + given) <= Pl
						&& (temp->next == NULL)) {

						temp->P_f_g = Pro - kernel[KN].Pn;
						P_Given_list = clean_list(P_Given_list);
						given = kernel[KN].Pn;

#if DEBUG_MESSAGES
						printf("\n||---AEAP with ALAP Condition-1 &  -->The Kernel:%d scheduled AEAP -->---||", KN);
#endif
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list);
						Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);
						return Pa;
					}

					else if ((kernel[KN].Pn + given) <= Pl
						&& (temp->next != NULL)) {
						Node* check = temp->next;
						if (check->P_f_g + kernel[KN].Pn <= Pl) {

							temp->P_f_g = Pro - kernel[KN].Pn;
							P_Given_list = clean_list(P_Given_list);
							given = kernel[KN].Pn;

#if DEBUG_MESSAGES
							printf("\n||---AEAP with ALAP Condition-1 &  -->The Kernel:%d scheduled AEAP -->---||", KN);
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
										printf("\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!", KN);
										printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
#endif

										CPU_Kernel++;
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
										printf("\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||", KN);
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
									printf("\n||---AEAP with ALAP-->Backup processors reloaded-->\n");
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
