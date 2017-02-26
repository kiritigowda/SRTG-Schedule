/*
* RTGS-mode_2.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/**********************************************************************************************************
RTGS Mode 3 - AEAP with As Late As Possible mode->AEAP/ALAP
***********************************************************************************************************/
int RTGS_mode_3(char *kernel_file, char *Releasetime_file)
{
	Kernel_INFO kernel[MAX_KERNELS];
	Node *Pro_free_list = NULL; //*Pro_release_list=NULL;
	Node *Kernel_queue = NULL;  //Kernel queued for future executions

	int Pa = MAX_GPU_PROCESSOR;
	int c, Nkr = 0;
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

			Pa = Mode_3_book_keeper(kernel, KN, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
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
				Pa = Mode_3_book_keeper(kernel, k1, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
				Pa = Mode_3_book_keeper(kernel, k2, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
			}

			else
			{
				Pa = Mode_3_book_keeper(kernel, k2, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
				Pa = Mode_3_book_keeper(kernel, k1, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
			}
		}
	}

	Pro_free_list = clean_node(Pro_free_list);
	alap = clean_list(alap);

	if (rt != 0)
	{
#if DEBUG_MESSAGES
		printf("\n\n$$$ All Kernels Scheduled or Sent to CPU Successfully - Processors Available: %d -->Mode_3_AEAP Kernels: %d $$$\n\n", Pa, count); //End of Scheduler
		printf("\n\n$$$ Kernels sent Back to CPU: %d $$$\n\n", CPU_Kernel); //End of Scheduler
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

/* Book keeping Function, the core function of the scheduler in charge of assigning processors and allocating the future releases */
int Mode_3_book_keeper(Kernel_INFO* kernel, int KN, int Pa, int i, Node **Pro_free_list, Node **Kernel_queue) {

	int Pf, Tf, Pt = i, SA = 99;
	static int Flag_Processors = 0;
#if DEBUG_MESSAGES
	printf("\n ^^ Kernel[%d].Pn = %d, Texe = %d, Td = %d, Tls= %d ^^\n", KN,kernel[KN].Pn, kernel[KN].Texe, kernel[KN].Td, kernel[KN].Tls);
#endif

	if (kernel[KN].Pn <= Pa) // If processors available is greater than the required processors by the kernel
	{

		if (alap == NULL) {

			Flag_Processors = 0;

			if (kernel[KN].Texe + Pt <= kernel[KN].Td) {

				Pa = Pa - kernel[KN].Pn;
				Pf = kernel[KN].Pn;
				Tf = kernel[KN].Texe + Pt;
				SA = 0;
				Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
			}

			else {
#if DEBUG_MESSAGES
				printf(	"\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n",	KN);
#endif
				CPU_Kernel++;
			}
		}

		else if (alap != NULL) {

			int Pl = MAX_GPU_PROCESSOR - alap->Pg;

			if ((kernel[KN].Pn + Flag_Processors) <= Pl
				|| (Pt + kernel[KN].Texe) <= alap->data) {

				Flag_Processors = Flag_Processors + kernel[KN].Pn;
#if DEBUG_MESSAGES
				printf("\n||---ALAP is set!! -->The Kernel:%d SATISFIED CONDITION 1 or 2",KN);
#endif

				if (kernel[KN].Texe + Pt <= kernel[KN].Td)
				{

					Pa = Pa - kernel[KN].Pn;
					Pf = kernel[KN].Pn;
					Tf = kernel[KN].Texe + Pt;
					SA = 0;
					Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
				}

				else
				{
#if DEBUG_MESSAGES
					printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", KN);
#endif

					CPU_Kernel++;
				}
			}

			else
			{
#if DEBUG_MESSAGES
				printf("\n\n@@ Kernel-%d REJECTED as ALAP kernel is set@@\n\n",	KN);
#endif
				CPU_Kernel++;
			}

		}
	}

	else if (kernel[KN].Pn > Pa) // If processors available is greater than the required processors by the kernel
	{

		Pa = Mode_3_Processors_Unavailable(kernel, KN, i, Pa, Pro_free_list,
			Kernel_queue); // Schedule the kernel to be released in a future time

	}

	return Pa;

}

int Mode_3_Processors_Unavailable(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

	//printf("\nKernel->%d sent for Mode_3_AEAP Scheduling at TIME: %d\n",KN,i);

	if (kernel[KN].Pn < PROCESSOR_LIMIT) {

		Pa = Mode_3_AEAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);

		count++;
	}

	else if (kernel[KN].Pn >= PROCESSOR_LIMIT && alap == NULL) {
#if DEBUG_MESSAGES
		printf("\n>>>>>Kernel:%d sent for ALAP execution\n", KN);
#endif

		Pa = Mode_3_ALAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);

	}

	else if (kernel[KN].Pn >= PROCESSOR_LIMIT && alap != NULL) {
#if DEBUG_MESSAGES
		printf("\n>>>>>Kernel:%d sent BACK TO CPU as ALAP is already set\n",KN);
#endif
		CPU_Kernel++;

	}

	return Pa;
}

int Mode_3_AEAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

	//printf("\n||---Mode_3_AEAP-->Kernel->%d is verified for Mode_3_AEAP scheduling\n",KN);
	int Pro = 0, Tr;
	static int given = 0;
	backup_list *P_Given_list = NULL;
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
				printf(	"\n\n||---Mode_3_AEAP-->The Kernel:%d Cannot be scheduled Mode_3_AEAP*****|",	KN);
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
					printf(	"\n||---Mode_3_AEAP-->The Kernel:%d scheduled Mode_3_AEAP -->---||",KN);
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
	}

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
				printf(	"\n\n||---Mode_3_AEAP-->The Kernel:%d Cannot be scheduled Mode_3_AEAP*****|",KN);
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

					int Pl = MAX_GPU_PROCESSOR - alap->Pg;

					if ((kernel[KN].Pn + given) <= Pl) {

						temp->P_f_g = Pro - kernel[KN].Pn;
						P_Given_list = clean_list(P_Given_list);
						given = kernel[KN].Pn;
#if DEBUG_MESSAGES
						printf(	"\n||---Mode_3_AEAP with ALAP Condition-1 -->The Kernel:%d scheduled Mode_3_AEAP -->---||",	KN);
#endif
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list);
						Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);
						return Pa;
					}

					else if (Tf <= alap->data) {

						temp->P_f_g = Pro - kernel[KN].Pn;
						P_Given_list = clean_list(P_Given_list);
#if DEBUG_MESSAGES
						printf(	"\n||---Mode_3_AEAP with ALAP Condition-2 -->The Kernel:%d scheduled Mode_3_AEAP -->---||",	KN);
#endif
						Queue_kernel_execution(Pf, Tf, Pt, SA, KN,
							Pro_free_list);
						Kernel_queue_handler(Pf, Tr, Pt, SA, KN, Kernel_queue);
						return Pa;
					}

					else {

#if DEBUG_MESSAGES
						printf(	"\n!!!---Mode_3_AEAP with ALAP is not Possible for the Kernel:%d-->---!!!",	KN);
						printf("\n!!!---KERNEK:%d SENT BACK TO CPU -->---!!!",	KN);
#endif
						CPU_Kernel++;
						// sent back to CPU

					}


					break;
				}

				else if (Pro < kernel[KN].Pn) {

					P_Given_list = insert_list(P_Given_list, temp->P_f_g);
					temp->P_f_g = 0;
					temp = temp->next;
				}

			}

		}

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
		printf("\n||---Mode_3_AEAP with ALAP-->Backup processors reloaded-->\n");
#endif
	}

	return Pa;
}

int Mode_3_ALAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue) {

	//printf("\n||---AEAP-->Kernel->%d is verified for AEAP scheduling\n",KN);

	int Pro = 0, Tr, Tf, Pg;

	backup_list *P_Given_list = NULL;

	Pg = kernel[KN].Pn;
	Tf = kernel[KN].Td;
	Tr = Tf - kernel[KN].Texe;

	Pro = Pa;
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
			printf(	"\n\n$$---ALAP-->The Kernel:%d Cannot be scheduled ALAP*****$$",KN);
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
				printf("\n$$---ALAP-->The Kernel:%d scheduled ALAP-->---$$",KN);
#endif
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
#if DEBUG_MESSAGES
		printf("$$---AEAP-->The Kernel:%d Cannot be scheduled AEAP --$$", KN);
#endif
	}

	return Pa;
}