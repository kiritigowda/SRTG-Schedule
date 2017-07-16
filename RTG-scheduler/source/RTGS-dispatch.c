/*
* RTGS-dispatche.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/* Function to free processors at times their execution is complete  */
int Retrieve_processors
(
	int present_time, 
	int processors_available, 
	scheduledNode** processor_alloc_list
) 
{
	scheduledNode* temp;
	temp = *processor_alloc_list;

	if (temp != NULL) 
	{
		if (temp->data == present_time) 
		{
			processors_available = processors_available + temp->processors_allocated;
#if DEBUG_MESSAGES
			printf("\n\n\n--Dispatch----->TIME: %d  -- PA = %d\n", present_time, processors_available);
#endif
			if (temp->kernel_next != NULL) 
			{
				scheduledNode *t1, *t2;
				t1 = temp->kernel_next;
				while (t1 != NULL) 
				{
					t2 = t1->kernel_next;
#if DEBUG_MESSAGES
					printf(	"\n\n--TIME: %d Dispatch--## Kernel -- %d data sent back ##\n",	present_time, t1->kernel_number);
#endif
					free(t1);
					t1 = t2;

				}
			}
			else 
			{
#if DEBUG_MESSAGES
				printf("\n\n--TIME: %d Dispatch--## Kernel -- %d data sent back ##\n", present_time, temp->kernel_number);
#endif
			}

			temp = position_delete(*processor_alloc_list, 1);
			*processor_alloc_list = temp;
		}
	}

	return processors_available;
}

/* Function to dispatch kernels for execution  */
int Dispatch_queued_kernels
(
	int present_time, 
	int processors_available, 
	scheduledNode** kernel_queue_list,
	scheduledNode **processor_alloc_list
) 
{
	scheduledNode* temp;
	temp = *kernel_queue_list;

	if (temp != NULL) 
	{
		if (temp->data == present_time) 
		{
			if (temp->kernel_next != NULL) 
			{
				scheduledNode *t1, *t2;
				t1 = temp->kernel_next;

				while (t1 != NULL) 
				{
					t2 = t1->kernel_next;

					if (t1->schedule_method == 2) 
					{
						int ALAP_Pg = 0;
						if (GLOBAL_ALAP_LIST != NULL) 
						{
							backup_list* temp1 = GLOBAL_ALAP_LIST;

							if (temp1->data == present_time) 
							{
								ALAP_Pg = temp1->processors_allocated;
								GLOBAL_ALAP_LIST = position_delete_list(GLOBAL_ALAP_LIST);
							}

							else 
							{
#if DEBUG_MESSAGES
								printf("\n\n!!!!ERROR At TIME: %d<--Dispatch--Kernel -- %d-->!!!\n",
									present_time, t1->kernel_number);
#endif
								return processors_available;
							}
						}
						processors_available = processors_available - t1->processors_allocated;
#if DEBUG_MESSAGES
						printf(	"\n\nTIME: %d<--Dispatch-- schedule_method:2 --Kernel -- %d sent to GPU for EXECUTION-->\n",	
							present_time, t1->kernel_number);
#endif
						Queue_kernel_execution(ALAP_Pg, t1->processor_release_time, present_time, 
											t1->schedule_method, t1->kernel_number, processor_alloc_list);
					}
					else 
					{
#if DEBUG_MESSAGES
						printf(	"\n\nTIME: %d<--Dispatch-- schedule_method:1 --Kernel -- %d sent to GPU for EXECUTION-->\n",	
							present_time, t1->kernel_number);
#endif
					}

					free(t1);
					t1 = t2;
				}
			}
			else 
			{
				if (temp->schedule_method == 2) 
				{
					int ALAP_Pg = 0;
					if (GLOBAL_ALAP_LIST != NULL) 
					{
						backup_list* temp1 = GLOBAL_ALAP_LIST;
						if (temp1->data == present_time) 
						{
							ALAP_Pg = temp1->processors_allocated;
							GLOBAL_ALAP_LIST = position_delete_list(GLOBAL_ALAP_LIST);
						}
						else 
						{
#if DEBUG_MESSAGES
							printf("\n\n!!!!ERROR At TIME: %d<--Dispatch--Kernel -- %d-->!!!\n",
								present_time, temp->kernel_number);
#endif
							return processors_available;
						}
					}
					processors_available = processors_available - temp->processors_allocated;
#if DEBUG_MESSAGES
					printf(	"\n\nTIME: %d<--Dispatch-- schedule_method:2 --Kernel -- %d sent to GPU for EXECUTION-->\n",	
						present_time, temp->kernel_number);
#endif
					Queue_kernel_execution(ALAP_Pg, temp->processor_release_time, present_time, 
										temp->schedule_method,temp->kernel_number, processor_alloc_list);
				}
				else 
				{
#if DEBUG_MESSAGES
					printf("\n\nTIME: %d<--Dispatch-- schedule_method:1 --Kernel -- %d sent to GPU for EXECUTION-->\n",
						present_time, temp->kernel_number);
#endif
				}
			}
			temp = position_delete(*kernel_queue_list, 1);
			*kernel_queue_list = temp;
		}
	}
	return processors_available;
}

