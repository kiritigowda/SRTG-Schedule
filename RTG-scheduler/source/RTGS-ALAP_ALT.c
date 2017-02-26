/*
* RTGS-ALAP_ALT.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP_Flagged(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

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
			printf("\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$", KN);
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
			printf("\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$",KN);
#endif

			alap = insert_ALAP_list(alap, Tr, Tf, Pf, KN);
			Kernel_queue_handler(Pf, Tr, Tf, SA, KN, Kernel_queue);

			return Pa;

		}

		else if (alap_check->Pg < kernel[KN].Pn) {

			while (temp != NULL) {

				if ((temp->Tf + kernel[KN].Texe) > kernel[KN].Td) {

#if DEBUG_MESSAGES
					printf(	"\n!!!---ALAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",KN);
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
							printf("\n!!!---ALAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",	KN);
							printf("\n!!!---KERNEL:%d SENT BACK TO CPU -->---!!!",KN);
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
							printf(	"\n$$---ALAP_FLAGGED-->The Kernel:%d scheduled ALAP-->---$$",KN);
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
		printf(	"\n!!!---ALAP FLAGGED with ALAP is not Possible for the Kernel:%d-->---!!!",KN);
		printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!", KN);
#endif
		CPU_Kernel++;
		return Pa;

	}

	return Pa;

}
