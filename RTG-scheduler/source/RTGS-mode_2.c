/*
* RTGS-mode_2.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/**********************************************************************************************************
RTGS Mode 2 - As Early As Possible mode->AEAP
***********************************************************************************************************/
int RTGS_mode_2(char *kernel_file, char *Releasetime_file)
{
	Kernel_INFO kernel[MAX_KERNELS];
	Node *Pro_free_list = NULL; //*Pro_release_list=NULL;
	Node *Kernel_queue = NULL;  //Kernel queued for future executions

	int Pa = MAX_GPU_PROCESSOR;
	int c = 0, Nkr = 0;
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

			Pa = Mode_2_book_keeper(kernel, KN, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
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
				Pa = Mode_2_book_keeper(kernel, k1, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
				Pa = Mode_2_book_keeper(kernel, k2, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
			}

			else
			{
				Pa = Mode_2_book_keeper(kernel, k2, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
				Pa = Mode_2_book_keeper(kernel, k1, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
			}
		}
	}

	Pro_free_list = clean_node(Pro_free_list);
	alap = clean_list(alap);

	if (rt != 0)
	{
#if DEBUG_MESSAGES
		printf("\n\n$$$ All Kernels Scheduled or Sent to CPU Successfully - Processors Available: %d -->Mode_2_AEAP Kernels: %d $$$\n\n", Pa, count); //End of Scheduler
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

int Mode_2_book_keeper(Kernel_INFO* kernel, int KN, int Pa, int i, Node **Pro_free_list, Node **Kernel_queue) {


	int Pf, Tf, Pt = i, SA = 99;
#if DEBUG_MESSAGES
	printf("\n ^^ Kernel[%d].Pn = %d, Texe = %d, Td = %d, Tls= %d ^^\n", KN, kernel[KN].Pn, kernel[KN].Texe, kernel[KN].Td, kernel[KN].Tls);
#endif

	if (kernel[KN].Pn <= Pa) // If processors available is greater than the required processors by the kernel
	{


		if (kernel[KN].Texe + Pt <= kernel[KN].Td) {

			Pa = Pa - kernel[KN].Pn;
			Pf = kernel[KN].Pn;
			Tf = kernel[KN].Texe + Pt;
			SA = 0;
			Queue_kernel_execution(Pf, Tf, Pt, SA, KN, Pro_free_list); // Kernel call for the GPU to handle the given Kernels and number of blocks//
		}

		else {
#if DEBUG_MESSAGES
			printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", KN);
#endif
			CPU_Kernel++;
		}

	}

	else if (kernel[KN].Pn > Pa) // If processors available is greater than the required processors by the kernel
	{

		Pa = Mode_2_Processors_Unavailable(kernel, KN, i, Pa, Pro_free_list, Kernel_queue); // Schedule the kernel to be released in a future time

	}


	return Pa;

}

int Mode_2_Processors_Unavailable(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue)
{

	//printf("\nKernel->%d sent for Mode_2_AEAP Scheduling at TIME: %d\n",KN,i);

	Pa = Mode_2_AEAP(kernel, KN, i, Pa, Pro_free_list, Kernel_queue);

	count++;


	return Pa;
}

int Mode_2_AEAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue)
{
	//printf("\n||---Mode_2_AEAP-->Kernel->%d is verified for Mode_2_AEAP scheduling\n",KN);

	int Pro = 0, Tr;
	backup_list *P_Given_list = NULL;

	Pro = Pa;

	P_Given_list = insert_list(P_Given_list, Pa);
	Pa = 0;

	Node* temp = *Pro_free_list;

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
			printf("\n\n||---Mode_2_AEAP-->The Kernel:%d Cannot be scheduled Mode_2_AEAP*****|", KN);
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
				int SA = 2;

#if DEBUG_MESSAGES
				printf("\n||---Mode_2_AEAP-->The Kernel:%d scheduled Mode_2_AEAP -->---||", KN);
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

	if (temp == NULL && P_Given_list != NULL) {

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
		printf("||---Mode_2_AEAP-->The Kernel:%d Cannot be scheduled Mode_2_AEAP -->", KN);
#endif
		CPU_Kernel++;
	}

	return Pa;
}