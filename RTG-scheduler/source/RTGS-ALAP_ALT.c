/*
* RTGS-ALAP_ALT.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP_advanced
(
kernelInfo *kernel_info_list,
int kernel_number, 
int present_time,
int processors_available, 
scheduledNode ** processor_alloc_list, 
scheduledNode **kernel_queue_list
) 
{
#if DEBUG_MESSAGES
	printf("ALAP Advanced::Kernel->%d is verified for AEAP advanced scheduling\n", kernel_number);
#endif
	int Pro = 0, kernel_release_time;
	scheduledNode* temp = *processor_alloc_list;
	backup_list *alap_check = GLOBAL_ALAP_LIST;

	while (alap_check->next != NULL)
		alap_check = alap_check->next;

	if ((alap_check->processor_release_time + kernel_info_list[kernel_number].execution_time) <= kernel_info_list[kernel_number].deadline) 
	{
		if (processors_available == MAX_GPU_PROCESSOR) 
		{
			kernel_release_time = kernel_info_list[kernel_number].deadline - kernel_info_list[kernel_number].execution_time;
			int processorReleased = kernel_info_list[kernel_number].processor_req;
			int processor_release_time = kernel_info_list[kernel_number].deadline;
			int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;
			GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
			printf("ALAP Advanced: Kernel:%d scheduled\n", kernel_number);
			printf("ALAP Advanced: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
			GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, 
				processorReleased, kernel_number);
			Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, schedule_method,
				kernel_number, kernel_queue_list);
			return processors_available;
		}
		else if (alap_check->processors_allocated >= kernel_info_list[kernel_number].processor_req)
		{
			kernel_release_time = kernel_info_list[kernel_number].deadline - kernel_info_list[kernel_number].execution_time;
			int processorReleased = kernel_info_list[kernel_number].processor_req;
			int processor_release_time = kernel_info_list[kernel_number].deadline;
			int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;
			GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
			printf("ALAP Advanced: Kernel:%d scheduled\n", kernel_number);
			printf("ALAP Advanced: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
			GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, processorReleased, kernel_number);
			Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, schedule_method, kernel_number, kernel_queue_list);
			return processors_available;
		}
		else if (alap_check->processors_allocated < kernel_info_list[kernel_number].processor_req)
		{
			while (temp != NULL)
			{
				if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
				{
					GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
					printf("ALAP Advanced: The Kernel:%d Cannot be scheduled\n", kernel_number);
					printf("ALAP Advanced: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
					return processors_available;
				}
				else if (temp->processor_release_time <= (kernel_info_list[kernel_number].deadline - kernel_info_list[kernel_number].execution_time)) 
				{
					scheduledNode *t1 = temp;
					Pro = alap_check->processors_allocated;
					do {
						Pro = Pro + t1->processors_allocated;
						
						if (t1->next == NULL && t1->processor_release_time > alap_check->data)
							Pro = Pro + (processors_available - Pro);

						if ((t1->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline) 
						{
							GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
							printf("ALAP Advanced: The Kernel:%d Cannot be scheduled\n", kernel_number);
							printf("ALAP Advanced: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
							return processors_available;
						}
						else if (Pro >= kernel_info_list[kernel_number].processor_req) 
						{
							kernel_release_time = kernel_info_list[kernel_number].deadline - kernel_info_list[kernel_number].execution_time;
							int processorReleased = kernel_info_list[kernel_number].processor_req;
							int processor_release_time = kernel_info_list[kernel_number].deadline;
							int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;
							GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
							printf("ALAP Advanced: Kernel:%d scheduled\n", kernel_number);
							printf("ALAP Advanced: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
							GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time,
								processorReleased, kernel_number);
							Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time, 
								schedule_method, kernel_number,	kernel_queue_list);

							return processors_available;
						}
						t1 = t1->next;
					} while (t1 != NULL);
					//break;
				}
				temp = temp->next;
			} //End of while
		} //End of else if
	} //End if
	else 
	{
		GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
		printf("ALAP Advanced: The Kernel:%d Cannot be scheduled\n", kernel_number);
		printf("ALAP Advanced: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
		return processors_available;
	}
	return processors_available;

}
