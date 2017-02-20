/*
* RTGS-ALAP.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

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

		printf("\n$$---ALAP-->The Kernel:%d scheduled ALAP-->---$$",
			KN);

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
			printf(
				"\n\n$$---ALAP-->The Kernel:%d Cannot be scheduled ALAP*****$$",
				KN);

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

				printf("\n$$---ALAP-->The Kernel:%d scheduled ALAP-->---$$",
					KN);

				alap = insert_ALAP_list(alap, Tr, Tf, Pg, KN);
				Kernel_queue_handler(Pf, Tr, Tf, SA, KN, Kernel_queue);

				return Pa;

			}

			else if (Pro < kernel[KN].Pn) {

				P_Given_list = insert_ALAP_list(P_Given_list, Tr, Tf, temp->P_f_g, KN);
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
		printf("$$---ALAP-->The Kernel:%d Cannot be scheduled AEAP --$$", KN);
	}

	return Pa;
}

