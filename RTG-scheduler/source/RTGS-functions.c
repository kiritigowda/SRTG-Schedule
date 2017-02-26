/*
* RTGS-functions.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"


/* Book keeping Function, the core function of the scheduler in charge of assigning processors and allocating the future releases */
int Kernel_book_keeper(Kernel_INFO* kernel, int KN, int Pa, int i, Node **Pro_free_list, Node **Kernel_queue) {

	int Pf, Tf, Pt = i, SA = 99;
	static int FLAG = 0, FLAG_V = 0, given = 0;

#if DEBUG_MESSAGES
	printf("\n ^^ Kernel[%d].Pn = %d, Texe = %d, Td = %d, Tls= %d ^^\n", KN, kernel[KN].Pn, kernel[KN].Texe, kernel[KN].Td, kernel[KN].Tls);
#endif

	if (kernel[KN].Pn <= Pa) // If processors available is greater than the required processors by the kernel
	{

		if (alap == NULL) { //ALAP not set
			FLAG = 0;
			FLAG_V = 0;
			given = 0;

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
					printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED Send to CPU @@\n\n", KN);
#endif

					CPU_Kernel++;


				}
			}

			else if (kernel[KN].Pn >= PROCESSOR_LIMIT) { // Processors needed greater or equal than the limit
#if DEBUG_MESSAGES
				printf(	"\n>>>>>Kernel:%d is compute intensive, sent for ALAP execution\n",	KN);
#endif
				Pa = ALAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);

			}
		}

		else if (alap != NULL) { //ALAP set

			if (FLAG_V != alap->data) { //ALAP updated
				FLAG = 0;
				FLAG_V = 0;
				given = 0;
			}
			int Pl = MAX_GPU_PROCESSOR - alap->Pg;

			if (kernel[KN].Pn < PROCESSOR_LIMIT) { // Processors needed lesser than the limit

				if (kernel[KN].Pn <= Pl && (Pt + kernel[KN].Texe) <= alap->data) { // Condition 1

#if DEBUG_MESSAGES
					printf(	"\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 1 && 2",	KN);
#endif

					if (kernel[KN].Texe + Pt <= kernel[KN].Td) {

						Pa = Pa - kernel[KN].Pn;
						Pf = kernel[KN].Pn;
						Tf = kernel[KN].Texe + Pt;
						SA = 0;
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}
					else {
#if DEBUG_MESSAGES
						printf(	"\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n",	KN);
#endif
						CPU_Kernel++;
					}
				}

				else if (kernel[KN].Pn > Pl && (Pt + kernel[KN].Texe) <= alap->data) { // Condition 2

#if DEBUG_MESSAGES
					printf(	"\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 2",	KN);
#endif

					if (kernel[KN].Texe + Pt <= kernel[KN].Td) {

						Pa = Pa - kernel[KN].Pn;
						Pf = kernel[KN].Pn;
						Tf = kernel[KN].Texe + Pt;
						SA = 0;
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}

					else {
#if DEBUG_MESSAGES
						printf(	"\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n",	KN);
#endif
						CPU_Kernel++;
					}
				}

				else if ((kernel[KN].Pn + given) <= Pl && (Pt + kernel[KN].Texe) > alap->data && FLAG == 0) { // Condition 3


					given = given + kernel[KN].Pn;


					// Control flags to not allow over budgeting of PA
					if (given == Pl) {
						FLAG = 1;
						FLAG_V = alap->data;
						given = 0;
					}

#if DEBUG_MESSAGES
					printf(	"\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 1 with FLAG",KN);
#endif

					if (kernel[KN].Texe + Pt <= kernel[KN].Td) {

						Pa = Pa - kernel[KN].Pn;
						Pf = kernel[KN].Pn;
						Tf = kernel[KN].Texe + Pt;
						SA = 0;
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
					}

					else {
#if DEBUG_MESSAGES
						printf(	"\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n",	KN);
#endif
						CPU_Kernel++;
					}
				}



				else if (Pa >= kernel[alap->KN].Pn) {

					/// NEW FUNCTION NEEDED
#if DEBUG_MESSAGES
					printf("\n\n\n RELEASE ALAP KERNEL NOW TO INCREASE SYSTEM TIME\n\n");
					printf("\nPA : %d   ALAP_Pg : %d\n", Pa, kernel[alap->KN].Pn);
#endif
					Pa = ALAP_improve(kernel, alap->KN, i, Pa, Pro_free_list, Kernel_queue);
					Pa = AEAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);
					count++;

				}


				else
				{
					Pa = AEAP_Flagged(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);
					count++;
				}

			}//Processors lesses than the limit

			else if (kernel[KN].Pn >= PROCESSOR_LIMIT) { // Processors needed greater or equal than the limit

				Pa = ALAP_Flagged(kernel, KN, i, Pa, Pro_free_list,
					Kernel_queue);
				count++;

			}

		}//ALAP != NULL end if
	}//End Pa available

	else if (kernel[KN].Pn > Pa) // If processors available is lesser than the required processors by the kernel
	{

		Pa = Processors_unavailable(kernel, KN, i, Pa, Pro_free_list,
			Kernel_queue); // Schedule the kernel to be released in a future time

	}

	return Pa;

}

int Processors_unavailable(Kernel_INFO *kernel, int KN, int i, int Pa,
	Node ** Pro_free_list, Node **Kernel_queue) {

	//printf("\nKernel->%d sent for AEAP Scheduling at TIME: %d\n",KN,i);

	if (kernel[KN].Pn < PROCESSOR_LIMIT) {

		Pa = AEAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);

		count++;
	}

	else if (kernel[KN].Pn >= PROCESSOR_LIMIT && alap == NULL) {

#if DEBUG_MESSAGES
		printf("\n>>>>>Kernel:%d sent for ALAP execution\n", KN);
#endif
		Pa = ALAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);

	}


	else if (kernel[KN].Pn >= PROCESSOR_LIMIT && alap != NULL) {
#if DEBUG_MESSAGES
		printf("\n>>>>>Kernel:%d sent for ALAP execution\n", KN);
#endif

		Pa = ALAP_Flagged(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);

	}

	return Pa;
}
