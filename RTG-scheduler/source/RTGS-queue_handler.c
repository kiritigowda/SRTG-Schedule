/*
/*
* RTGS-queue_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"


/* Function to add future kernel releases and arrange kernel execution times in ascending order */
void Queue_kernel_execution(int Pf, int Tf, int i, int SA, int KN, Node **Pro_free_list) {
	//printf("\n *** The Processors given :: %d ***\n\n", Pf);
	*Pro_free_list = ascending_insert(*Pro_free_list, Tf, Tf, Pf, KN, SA);
	print(*Pro_free_list);

	// Sending Data and Kernels

	return;
}


/* Function to add future kernel releases and arrange kernel execution times in ascending order */
void Kernel_queue_handler(int Pf, int Tr, int Tf, int SA, int KN, Node **Kernel_queue) {
	//printf("\n *** The Processors given :: %d ***\n\n", Pf);
	if (SA == 2) {
		*Kernel_queue = ascending_insert(*Kernel_queue, Tr, Tf, Pf, KN, SA);
		Kernel_queue_print(*Kernel_queue);
	}
	else {
		*Kernel_queue = ascending_insert(*Kernel_queue, Tr, Tr, Pf, KN, SA);
		Kernel_queue_print(*Kernel_queue);
	}

	// Sending Data and Kernels

	return;
}
