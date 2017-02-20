/*
* RTGS-AEAP_ALPA_improve.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP_ALAP_improve(Kernel_INFO *kernel, int Tr, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

	printf("\n||---AEAP_ALAP_IMPROVE--> ALAP IMPROVED scheduling\n");
	printf("\n||---AEAP_ALAP_IMPROVE--> TR: %d\n", Tr);

	Node *temp = *Kernel_queue;

	while (temp->KN != alap->KN && temp->next != NULL)
		temp = temp->next;


	if (temp->KN == alap->KN && temp->P_f_g == alap->Pg) {
		temp->data = temp->Tr = alap->data = Tr;
		temp->Tf = alap->Tf = Tr + kernel[temp->KN].Texe;
	}

	return Pa;
}
