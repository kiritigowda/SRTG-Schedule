/*
* RTGS-dispatche.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"
/* Function to free processors at times their execution is complete  */

int Retrieve_processors(int i, int Pa, Node** Pro_free_list) {

	Node* temp;
	temp = *Pro_free_list;

	if (temp != NULL) {

		if (temp->data == i) {
			//printf("\n\n\n--Dispatch----->TIME: %d\n",i);
			Pa = Pa + temp->P_f_g;
#if DEBUG_MESSAGES
			printf("\n\n\n--Dispatch----->TIME: %d  -- PA = %d\n", i, Pa);
#endif
			if (temp->Kernel_next != NULL) {
				Node *t1, *t2;
				t1 = temp->Kernel_next;

				while (t1 != NULL) {

					t2 = t1->Kernel_next;
#if DEBUG_MESSAGES
					printf(	"\n\n--TIME: %d Dispatch--## Kernel -- %d data sent back ##\n",	i, t1->KN);
#endif
					free(t1);
					t1 = t2;

				}
			}

			else {
#if DEBUG_MESSAGES
				printf("\n\n--TIME: %d Dispatch--## Kernel -- %d data sent back ##\n", i, temp->KN);
#endif
			}


			temp = position_delete(*Pro_free_list, 1);
			*Pro_free_list = temp;
		}
	}

	return Pa;
}

int Dispatch_queued_kernels(int i, int Pa, Node** Kernel_queue,
	Node **Pro_free_list) {

	Node* temp;
	temp = *Kernel_queue;

	if (temp != NULL) {

		if (temp->data == i) {
			//printf("\n\n\n--Dispatch Kernel:%d----->TIME:%d\n",temp->KN,i);


			if (temp->Kernel_next != NULL) {

				Node *t1, *t2;
				t1 = temp->Kernel_next;

				while (t1 != NULL) {
					t2 = t1->Kernel_next;

					if (t1->SA == 2) {
						int ALAP_Pg = 0;
						if (alap != NULL) {

							backup_list* temp1 = alap;

							if (temp1->data == i) {
								ALAP_Pg = temp1->Pg;
								alap = position_delete_list(alap);
							}

							else {
#if DEBUG_MESSAGES
								printf("\n\n!!!!ERROR At TIME: %d<--Dispatch--Kernel -- %d-->!!!\n",i, t1->KN);
#endif
								return Pa;
							}

						}
						Pa = Pa - t1->P_f_g;
#if DEBUG_MESSAGES
						printf(	"\n\nTIME: %d<--Dispatch-- SA:2 --Kernel -- %d sent to GPU for EXECUTION-->\n",	i, t1->KN);
#endif
						Queue_kernel_execution(ALAP_Pg, t1->Tf, i, t1->SA,
							t1->KN, Pro_free_list);

					}

					else {
#if DEBUG_MESSAGES
						printf(	"\n\nTIME: %d<--Dispatch-- SA:1 --Kernel -- %d sent to GPU for EXECUTION-->\n",	i, t1->KN);
#endif
					}

					free(t1);
					t1 = t2;

				}

			}

			else {

				if (temp->SA == 2) {
					int ALAP_Pg = 0;

					//printf("\n\n\nALAP-MAX_GPU_PROCESSOR:%d\n",alap->Pg);
					if (alap != NULL) {
						backup_list* temp1 = alap;
						if (temp1->data == i) {
							ALAP_Pg = temp1->Pg;
							alap = position_delete_list(alap);
						}
						else {
#if DEBUG_MESSAGES
							printf("\n\n!!!!ERROR At TIME: %d<--Dispatch--Kernel -- %d-->!!!\n",i, temp->KN);
#endif
							return Pa;
						}
					}

					Pa = Pa - temp->P_f_g;
#if DEBUG_MESSAGES
					printf(	"\n\nTIME: %d<--Dispatch-- SA:2 --Kernel -- %d sent to GPU for EXECUTION-->\n",	i, temp->KN);
#endif
					Queue_kernel_execution(ALAP_Pg, temp->Tf, i, temp->SA,
						temp->KN, Pro_free_list);
				}

				else {
#if DEBUG_MESSAGES
					printf("\n\nTIME: %d<--Dispatch-- SA:1 --Kernel -- %d sent to GPU for EXECUTION-->\n",i, temp->KN);
#endif
				}

			}

			temp = position_delete(*Kernel_queue, 1);
			*Kernel_queue = temp;
		}

	}

	return Pa;

}

