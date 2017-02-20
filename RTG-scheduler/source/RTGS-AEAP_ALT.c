/*
* RTGS-AEAP_ALT.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP_Flagged(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

	printf(
		"\n||---AEAP_FLAGGED-->Kernel->%d is verified for ALAP Flagged scheduling\n",
		KN);

	int Pro = 0, Tr;

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
							printf(
								"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
								KN);
							printf(
								"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
								KN);
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

					printf(
						"\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||",
						KN);
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
									printf(
										"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
										KN);
									printf(
										"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
										KN);
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

			}

			else {

				//Kernel has to be sent to CPU
				printf(
					"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
					KN);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
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
								printf(
									"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
									KN);
								printf(
									"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
									KN);
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
				printf(
					"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
					KN);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
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

					printf(
						"\n||---AEAP-->The Kernel:%d scheduled AEAP -->---||",
						KN);
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
									printf(
										"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
										KN);
									printf(
										"\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",
										KN);
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

			}

			else {

				//Kernel has to be sent to CPU
				printf(
					"\n!!!---AEAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",
					KN);
				printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
				CPU_Kernel++;
				return Pa;

			}
		} //End of else if

	} //End of alap->next != NULL

	return Pa;
}

