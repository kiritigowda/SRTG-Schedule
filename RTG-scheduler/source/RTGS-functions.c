/*
* RTGS-functions.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/* Book keeping Function, the core function of the scheduler in charge of assigning processors and allocating the future releases */
int Kernel_book_keeper
(
	kernelInfo* kernel_info_list, 
	int kernel_number, 
	int processors_available, 
	int present_time, 
	scheduledNode **processor_alloc_list, 
	scheduledNode **kernel_queue_list
) 
{
	int processorReleased = 0, processor_release_time = 0;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	static int FLAG , FLAG_V, given;

#if DEBUG_MESSAGES
	printf("Kernel Book Keeper:: Kernel::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
		kernel_number, kernel_info_list[kernel_number].processor_req, kernel_info_list[kernel_number].execution_time,
		kernel_info_list[kernel_number].deadline, kernel_info_list[kernel_number].latest_schedulable_time);
#endif
	// If processors available is greater than the required processors by the kernel_info_list
	if (kernel_info_list[kernel_number].processor_req <= processors_available) 
	{
		if (GLOBAL_ALAP_LIST == NULL) 
		{ //ALAP not set
			FLAG = 0;
			FLAG_V = 0;
			given = 0;

			if (kernel_info_list[kernel_number].processor_req < PROCESSOR_LIMIT)
			{ // Processors needed lesser than the limit

				if (kernel_info_list[kernel_number].execution_time + present_time <= kernel_info_list[kernel_number].deadline)
				{
					processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
					processorReleased = kernel_info_list[kernel_number].processor_req;
					processor_release_time = kernel_info_list[kernel_number].execution_time + present_time;
					schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
					GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
					printf("Kernel Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
					// Kernel call for the GPU to handle the given Kernels and number of blocks
					Queue_kernel_execution(processorReleased, processor_release_time, present_time, 
						schedule_method, kernel_number, processor_alloc_list); 
					
				}
				else 
				{
					GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
					printf("Kernel Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
					printf("Kernel Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
				}
			}
			else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT) 
			{ // Processors needed greater or equal than the limit
#if DEBUG_MESSAGES
				printf("Kernel Book Keeper:: Kernel:%d is compute intensive, sent for ALAP execution\n", kernel_number);
#endif
				processors_available = ALAP(kernel_info_list, kernel_number, present_time, 
					processors_available, processor_alloc_list, kernel_queue_list);
			}
		}
		else if (GLOBAL_ALAP_LIST != NULL) 
		{ //ALAP set
			if (FLAG_V != GLOBAL_ALAP_LIST->data) 
			{ //ALAP updated
				FLAG = 0;
				FLAG_V = 0;
				given = 0;
			}
			int Pl = MAX_GPU_PROCESSOR - GLOBAL_ALAP_LIST->processors_allocated;

			if (kernel_info_list[kernel_number].processor_req < PROCESSOR_LIMIT) 
			{ // Processors needed lesser than the limit
				if (kernel_info_list[kernel_number].processor_req <= Pl && (present_time + kernel_info_list[kernel_number].execution_time) <= GLOBAL_ALAP_LIST->data) 
				{ // Condition 1
#if DEBUG_MESSAGES
					printf("Kernel Book Keeper:: ALAP is set, Kernel:%d SATISFIED CONDITION 1", kernel_number);
#endif
					if (kernel_info_list[kernel_number].execution_time + present_time <= kernel_info_list[kernel_number].deadline) {

						processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
						processorReleased = kernel_info_list[kernel_number].processor_req;
						processor_release_time = kernel_info_list[kernel_number].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
						printf("Kernel Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
						// Kernel call for the GPU to handle the given Kernels and number of blocks//
						Queue_kernel_execution(processorReleased, processor_release_time, present_time, 
							schedule_method, kernel_number,	processor_alloc_list); 
					}
					else 
					{
						GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
						printf("Kernel Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
						printf("Kernel Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
					}
				}
				else if (kernel_info_list[kernel_number].processor_req > Pl && (present_time + kernel_info_list[kernel_number].execution_time) <= GLOBAL_ALAP_LIST->data) 
				{ // Condition 2
#if DEBUG_MESSAGES
					printf("Kernel Book Keeper:: ALAP is set, Kernel:%d SATISFIED CONDITION 2", kernel_number);
#endif
					if (kernel_info_list[kernel_number].execution_time + present_time <= kernel_info_list[kernel_number].deadline) {

						processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
						processorReleased = kernel_info_list[kernel_number].processor_req;
						processor_release_time = kernel_info_list[kernel_number].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
						printf("Kernel Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
						// Kernel call for the GPU to handle the given Kernels and number of blocks//
						Queue_kernel_execution(processorReleased, processor_release_time, present_time,
							schedule_method, kernel_number, processor_alloc_list);
					}
					else 
					{
						GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
						printf("Kernel Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
						printf("Kernel Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
					}
				}
				else if ((kernel_info_list[kernel_number].processor_req + given) <= Pl && (present_time + kernel_info_list[kernel_number].execution_time) > GLOBAL_ALAP_LIST->data &&
					FLAG == 0) 
				{ // Condition 3
					given = given + kernel_info_list[kernel_number].processor_req;
					// Control flags to not allow over budgeting of PA
					if (given == Pl) {
						FLAG = 1;
						FLAG_V = GLOBAL_ALAP_LIST->data;
						given = 0;
					}
#if DEBUG_MESSAGES
					printf("Kernel Book Keeper::  ALAP is set, Kernel:%d SATISFIED CONDITION 1 with FLAG", kernel_number);
#endif
					if (kernel_info_list[kernel_number].execution_time + present_time <= kernel_info_list[kernel_number].deadline)
					{

						processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
						processorReleased = kernel_info_list[kernel_number].processor_req;
						processor_release_time = kernel_info_list[kernel_number].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						GLOBAL_GPU_KERNELS++;
#if DEBUG_MESSAGES
						printf("Kernel Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
#endif
						// Kernel call for the GPU to handle the given Kernels and number of blocks//
						Queue_kernel_execution(processorReleased, processor_release_time, present_time,
							schedule_method, kernel_number, processor_alloc_list);
					}
					else
					{
						GLOBAL_CPU_KERNELS++;
#if DEBUG_MESSAGES
						printf("Kernel Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
						printf("Kernel Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
#endif
					}
				}
				else if (processors_available >= kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req) 
				{
					/// NEW FUNCTION NEEDED
#if DEBUG_MESSAGES
					printf("Kernel Book Keeper:: RELEASE ALAP KERNEL NOW TO INCREASE SYSTEM TIME\n\n");
					printf("\nPA : %d   ALAP_Pg : %d\n", processors_available, kernel_info_list[GLOBAL_ALAP_LIST->kernel_number].processor_req);
#endif
					processors_available = ALAP_improve(kernel_info_list, GLOBAL_ALAP_LIST->kernel_number, present_time,
						processors_available, processor_alloc_list, kernel_queue_list);
					processors_available = AEAP(kernel_info_list, kernel_number, present_time,
						processors_available, processor_alloc_list, kernel_queue_list);
				}
				else
				{
					processors_available = AEAP_advanced(kernel_info_list, kernel_number, present_time, 
						processors_available, processor_alloc_list, kernel_queue_list);
				}
			}//Processors lesses than the limit
			else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT)
			{ // Processors needed greater or equal than the limit
				processors_available = ALAP_advanced(kernel_info_list, kernel_number, present_time, 
					processors_available, processor_alloc_list,	kernel_queue_list);
			}
		}//ALAP != NULL end if
	}//End processors_available available
	else if (kernel_info_list[kernel_number].processor_req > processors_available) 
	{// If processors available is lesser than the required processors by the kernel_info_list
		// Schedule the kernel_info_list to be released in a future time
		processors_available = Processors_unavailable(kernel_info_list, kernel_number, present_time, 
			processors_available, processor_alloc_list,kernel_queue_list); 
	}
	return processors_available;
}

int Processors_unavailable
(
	kernelInfo *kernel_info_list, 
	int kernel_number, 
	int present_time, 
	int processors_available,
	scheduledNode ** processor_alloc_list, 
	scheduledNode **kernel_queue_list
)
{
	if (kernel_info_list[kernel_number].processor_req < PROCESSOR_LIMIT)
	{
		processors_available = AEAP(kernel_info_list, kernel_number, present_time, 
			processors_available, processor_alloc_list, kernel_queue_list);
	}
	else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST == NULL) 
	{
		processors_available = ALAP(kernel_info_list, kernel_number, present_time, 
			processors_available, processor_alloc_list, kernel_queue_list);
	}
	else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST != NULL) 
	{
		processors_available = ALAP_advanced(kernel_info_list, kernel_number, present_time,
			processors_available, processor_alloc_list, kernel_queue_list);
	}

	return processors_available;
}
