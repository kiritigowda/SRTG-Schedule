/*
* RTGS-mode_1.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/**********************************************************************************************************
Main function to execute the Program
***********************************************************************************************************/
int Scheduler_main_mode_1(char *kernel_file, char *Releasetime_file)
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

	while (1)
	{
		printf("\nEnter your choice for the Scheduler:\n 0. Exit Mode 1\n 1. Load the Kernels and the Release Times\n 2. Schedule the Kernels\n\n");
		scanf_s("%d", &c);

		switch (c)
		{
		case 0:
			return RTGS_SUCCESS;

		case 1:
			Nkr = Get_kernel_information(kernel, kernel_file);                 				// Read Kernel.TXT
			rt = Get_kernel_release_times(Releasetime_file);                   				// Read Release_time.TXT
			printf("\nThe GPU Scheduler will Schedule %d Kernels\n\n", Nkr);				// Scheduler Begins
			printf("Please press 2 to schedule these Kernels\n\n");
			break;

		case 2:
		{


			int64_t stime = RTGS_GetClockCounter();
			for (int i = 0; i < rt; i++)
			{
				Pa = Retrieve_processors(i, Pa, &Pro_free_list);     // Freeing-up processors
				Pa = Dispatch_queued_kernels(i, Pa, &Kernel_queue, &Pro_free_list);     // Freeing-up processors

				if (RT[i] == 1)
				{
					printf("\n-->>Total processors Available at time %d = %d\n\n ", i, Pa);
					printf("Kernels:%d has been released\n", KN);

					Pa = Mode_1_book_keeper(kernel, KN, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
					KN++;
				}

				else if (RT[i] == 2)
				{
					printf("\n-->>Total processors Available at time %d = %d\n\n ", i, Pa);
					k1 = KN;
					printf("Kernels:%d has been released\n", KN);
					KN++;
					k2 = KN;
					printf("Kernels:%d has been released\n", KN);
					KN++;


					if (kernel[k1].Td <= kernel[k2].Td)
					{
						Pa = Mode_1_book_keeper(kernel, k1, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
						Pa = Mode_1_book_keeper(kernel, k2, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
					}

					else
					{
						Pa = Mode_1_book_keeper(kernel, k2, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
						Pa = Mode_1_book_keeper(kernel, k1, Pa, i, &Pro_free_list, &Kernel_queue); // handling the released kernel by the book-keeper
					}
				}
			}

			Pro_free_list = clean_node(Pro_free_list);
			alap = clean_list(alap);

			if (rt != 0)
			{
				printf("\n\n$$$ All Kernels Scheduled or Sent to CPU Successfully - Processors Available: %d -->AEAP Kernels: %d $$$\n\n", Pa, count); //End of Scheduler
				printf("\n\n$$$ Kernels sent Back to CPU: %d $$$\n\n", CPU_Kernel); //End of Scheduler
				printf("$$$ Please press 0 to exit the scheduler $$$\n\n");

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

			else
				printf("Please press 1 to load Kernels to be scheduled\n\n");

			print(Pro_free_list);
			break;

		}
		default: printf("\n\nPlease enter a valid choice for the scheduler\n\n");

		}

	}
	Pro_free_list = clean_node(Pro_free_list);
	printf("\n\n"); //End of Scheduler
	return RTGS_SUCCESS;
}

int Mode_1_book_keeper(struct Kernels* kernel, int KN, int Pa, int i, Node** Pro_free_list) {


	int Pf, Tf, Pt = i, SA = 99;

	printf("\n ^^ Kernel[%d].Pn = %d, Texe = %d, Td = %d, Tls= %d ^^\n", KN, kernel[KN].Pn, kernel[KN].Texe, kernel[KN].Td, kernel[KN].Tls);

	if (kernel[KN].Pn <= Pa) // If processors available is greater than the required processors by the kernel
	{


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
			printf("\n\n@@ Kernel-%d will not complete before it's deadline, Job REJECTED @@\n\n", KN);
			CPU_Kernel++;
		}


	}

	else
	{
		printf("\n\n!!@@ Kernel-%d is sent to CPU for execution @@!!\n\n", KN);
		CPU_Kernel++;
	}


	return Pa;

}