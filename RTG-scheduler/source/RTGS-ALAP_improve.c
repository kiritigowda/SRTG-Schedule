/*
* RTGS-ALAP_improve.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP_improve(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

#if DEBUG_MESSAGES
	printf("\n||---ALAP_IMPROVE-->Kernel->%d is verified for ALAP IMPROVED scheduling\n", KN);
#endif

	Node *temp = *Kernel_queue;

	if (temp->KN == KN) {
#if DEBUG_MESSAGES
		printf("\n||---ALAP_IMPROVE-->Kernel->%d is verified for ALAP IMPROVED scheduling\n", KN);
		printf("\n||--- alap->data: %d && Kernel_queue: %d\n", alap->data, temp->data);
#endif

		if (alap->data == temp->data) {

			alap->data = temp->data = i;
			temp->Tf = i + kernel[KN].Texe;

			Pa = Dispatch_queued_kernels(i, Pa, Kernel_queue, Pro_free_list);
		}

	}


	else if (temp->KN == -99) {

		Node *head = temp;
		Node *t1, *t2;
		t1 = temp->Kernel_next;

		while (t1 != NULL) {

			t2 = t1->Kernel_next;

			if (t1->KN == KN && alap->data == t1->data) {

				head->Kernel_next = t2;

				alap->data = t1->data = i;
				t1->Tf = i + kernel[KN].Texe;

				alap = position_delete_list(alap);
				Pa = Pa - t1->P_f_g;

#if DEBUG_MESSAGES
				printf("\n||---ALAP_IMPROVE-->Kernel-> -99\n");
				printf("\n\nTIME: %d<--Dispatch-- SA:2 --Kernel -- %d sent to GPU for EXECUTION-->\n", i, t1->KN);
#endif
				Queue_kernel_execution(t1->P_f_g, t1->Tf, i, t1->SA, t1->KN,
					Pro_free_list);

				free(t1);
				return Pa;
			}

			else
				head = t1;

			t1 = t2;

		}

	}


	return Pa;
}

