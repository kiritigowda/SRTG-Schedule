/*
* RTGS-mode_3.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/**********************************************************************************************************
MODE 3 FUNCTION
***********************************************************************************************************/
static int Mode_3_ALAP
(
	kernelInfo *kernel_info_list,
	int kernel_number,
	int present_time,
	int processors_available,
	scheduledNode ** processor_alloc_list,
	scheduledNode **kernel_queue_list
)
{
	int Pro = 0, kernel_release_time = 0, processor_release_time = 0, processors_allocated = 0;
	backup_list *P_Given_list = NULL;

	processors_allocated = kernel_info_list[kernel_number].processor_req;
	processor_release_time = kernel_info_list[kernel_number].deadline;
	kernel_release_time = processor_release_time - kernel_info_list[kernel_number].execution_time;
	Pro = processors_available;

	P_Given_list = insert_ALAP_list(P_Given_list, kernel_release_time, processor_release_time, processors_available, kernel_number);
	scheduledNode* temp = *processor_alloc_list;

	while (temp != NULL)
	{
		if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
		{
			int count = 0;
			scheduledNode*temp1 = *processor_alloc_list;
			backup_list* temp2 = P_Given_list;
			while (temp2 != NULL)
			{

				if (count == 0)
					temp2 = temp2->next;

				else {
					temp1->kernel_release_time = 0;
					temp1 = temp1->next;
					temp2 = temp2->next;
				}
				count++;
			}
			P_Given_list = clean_list(P_Given_list);
			kernel_info_list[kernel_number].schedule_hardware = 2;
			kernel_info_list[kernel_number].rescheduled_execution = -1;
			kernel_info_list[kernel_number].completion_time = -1;
			kernel_info_list[kernel_number].scheduled_execution = -1;
			GLOBAL_CPU_KERNELS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				// TBD:: Return Kernel to CPU - Function to send Kernel to CPU execution
				printf("Mode 3 ALAP: Kernel:%d Cannot be scheduled", kernel_number);
				printf("Mode 3 ALAP: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
			}
			return processors_available;
		}
		else
		{
			Pro = Pro + temp->processors_allocated;

			if (Pro >= kernel_info_list[kernel_number].processor_req)
			{
				temp->kernel_release_time = kernel_release_time;
				P_Given_list = clean_list(P_Given_list);

				int processorReleased = kernel_info_list[kernel_number].processor_req;
				int schedule_method = RTGS_SCHEDULE_METHOD_ALAP;

				kernel_info_list[kernel_number].schedule_hardware = 1;
				kernel_info_list[kernel_number].rescheduled_execution = -1;
				kernel_info_list[kernel_number].scheduled_execution = kernel_release_time;
				kernel_info_list[kernel_number].completion_time = kernel_info_list[kernel_number].execution_time + kernel_release_time;
				GLOBAL_GPU_KERNELS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 3 ALAP: Kernel:%d scheduled", kernel_number);
					printf("Mode 3 ALAP: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
				}
				GLOBAL_ALAP_LIST = insert_ALAP_list(GLOBAL_ALAP_LIST, kernel_release_time, processor_release_time, processors_allocated, kernel_number);
				Kernel_queue_handler(processorReleased, kernel_release_time, processor_release_time,
					schedule_method, kernel_number, kernel_queue_list);
				return processors_available;
			}
			else if (Pro < kernel_info_list[kernel_number].processor_req)
			{

				P_Given_list = insert_ALAP_list(P_Given_list, kernel_release_time, processor_release_time, temp->processors_allocated, kernel_number);
				temp->kernel_release_time = kernel_release_time;
				temp = temp->next;
			}
		}
	}
	if (temp == NULL && P_Given_list != NULL)
	{
		int count = 0;
		scheduledNode*temp1 = *processor_alloc_list;
		backup_list* temp2 = P_Given_list;
		while (temp2 != NULL)
		{
			if (count == 0) {
				temp2 = temp2->next;
			}
			else {
				temp1->kernel_release_time = 0;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}
			count++;
		}
		P_Given_list = clean_list(P_Given_list);
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 3 ALAP: Kernel:%d Cannot be scheduled AEAP --$$", kernel_number);
		}
	}
	return processors_available;
}

static int Mode_3_AEAP
(
	kernelInfo *kernel_info_list,
	int kernel_number,
	int present_time,
	int processors_available,
	scheduledNode ** processor_alloc_list,
	scheduledNode **kernel_queue_list
)
{
	int Pro = 0, kernel_release_time = 0;
	static int given = 0;
	backup_list *P_Given_list = NULL;
	scheduledNode* temp = *processor_alloc_list;

	if (GLOBAL_ALAP_LIST == NULL)
	{
		Pro = processors_available;
		P_Given_list = insert_list(P_Given_list, processors_available);
		processors_available = 0;

		while (temp != NULL)
		{
			if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
			{
				int count = 0;
				scheduledNode*temp1 = *processor_alloc_list;
				backup_list* temp2 = P_Given_list;
				while (temp2 != NULL)
				{

					if (count == 0) {
						processors_available = temp2->data;
						temp2 = temp2->next;
					}
					else {
						temp1->processors_allocated = temp2->data;
						temp1 = temp1->next;
						temp2 = temp2->next;
					}
					count++;
				}
				P_Given_list = clean_list(P_Given_list);

				kernel_info_list[kernel_number].schedule_hardware = 2;
				kernel_info_list[kernel_number].rescheduled_execution = -1;
				kernel_info_list[kernel_number].completion_time = -1;
				kernel_info_list[kernel_number].scheduled_execution = -1;
				GLOBAL_CPU_KERNELS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 3 AEAP: The Kernel:%d Cannot be scheduled\n", kernel_number);
					printf("Mode 3 AEAP: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
				}
				return processors_available;
			}
			else
			{
				Pro = Pro + temp->processors_allocated;
				if (Pro >= kernel_info_list[kernel_number].processor_req)
				{
					temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
					kernel_release_time = temp->processor_release_time;
					P_Given_list = clean_list(P_Given_list);

					int processorReleased = kernel_info_list[kernel_number].processor_req;
					int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

					kernel_info_list[kernel_number].schedule_hardware = 1;
					kernel_info_list[kernel_number].rescheduled_execution = -1;
					kernel_info_list[kernel_number].scheduled_execution = kernel_release_time;
					kernel_info_list[kernel_number].completion_time = kernel_info_list[kernel_number].execution_time + kernel_release_time;
					GLOBAL_GPU_KERNELS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode 3 AEAP: The Kernel:%d scheduled\n", kernel_number);
						printf("Mode 3 AEAP: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
					}
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list);
					Kernel_queue_handler(processorReleased, kernel_release_time, presentTime, schedule_method, kernel_number, kernel_queue_list);
					return processors_available;
				}
				else if (Pro < kernel_info_list[kernel_number].processor_req)
				{
					P_Given_list = insert_list(P_Given_list, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}
			}
		}
	}
	else if (GLOBAL_ALAP_LIST != NULL)
	{
		Pro = processors_available;
		P_Given_list = insert_list(P_Given_list, processors_available);
		processors_available = 0;
		while (temp != NULL)
		{
			if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
			{
				int count = 0;
				scheduledNode*temp1 = *processor_alloc_list;
				backup_list* temp2 = P_Given_list;
				while (temp2 != NULL) {

					if (count == 0) {
						processors_available = temp2->data;
						temp2 = temp2->next;
					}
					else {
						temp1->processors_allocated = temp2->data;
						temp1 = temp1->next;
						temp2 = temp2->next;
					}
					count++;
				}
				// TBD:: Return Kernel to CPU - Function to send Kernel to CPU execution
				kernel_info_list[kernel_number].schedule_hardware = 2;
				kernel_info_list[kernel_number].rescheduled_execution = -1;
				kernel_info_list[kernel_number].completion_time = -1;
				kernel_info_list[kernel_number].scheduled_execution = -1;
				GLOBAL_CPU_KERNELS++;
				P_Given_list = clean_list(P_Given_list);
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode 3 AEAP: The Kernel:%d Cannot be scheduled AEAP\n", kernel_number);
					printf("Mode 3 AEAP: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
				}
				return processors_available;
			}
			else
			{
				Pro = Pro + temp->processors_allocated;
				if (Pro >= kernel_info_list[kernel_number].processor_req)
				{
					kernel_release_time = temp->processor_release_time;
					int processorReleased = kernel_info_list[kernel_number].processor_req;
					int processor_release_time = kernel_release_time + kernel_info_list[kernel_number].execution_time;
					int presentTime = present_time;
					int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;
					int Pl = MAX_GPU_PROCESSOR - GLOBAL_ALAP_LIST->processors_allocated;
					if ((kernel_info_list[kernel_number].processor_req + given) <= Pl)
					{
						temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
						P_Given_list = clean_list(P_Given_list);
						given = kernel_info_list[kernel_number].processor_req;

						kernel_info_list[kernel_number].schedule_hardware = 1;
						kernel_info_list[kernel_number].rescheduled_execution = -1;
						kernel_info_list[kernel_number].scheduled_execution = kernel_release_time;
						kernel_info_list[kernel_number].completion_time = kernel_info_list[kernel_number].execution_time + kernel_release_time;
						GLOBAL_GPU_KERNELS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 3 AEAP: Condition-1 The Kernel:%d scheduled\n", kernel_number);
							printf("Mode 3 AEAP: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
						}
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
							schedule_method, kernel_number, processor_alloc_list);
						Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
							schedule_method, kernel_number, kernel_queue_list);
						return processors_available;
					}
					else if (processor_release_time <= GLOBAL_ALAP_LIST->data)
					{
						temp->processors_allocated = Pro - kernel_info_list[kernel_number].processor_req;
						P_Given_list = clean_list(P_Given_list);

						kernel_info_list[kernel_number].schedule_hardware = 1;
						kernel_info_list[kernel_number].rescheduled_execution = -1;
						kernel_info_list[kernel_number].scheduled_execution = kernel_release_time;
						kernel_info_list[kernel_number].completion_time = kernel_info_list[kernel_number].execution_time + kernel_release_time;
						GLOBAL_GPU_KERNELS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 3 AEAP: Condition-2 The Kernel:%d scheduled\n", kernel_number);
							printf("Mode 3 AEAP: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
						}
						Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
							schedule_method, kernel_number, processor_alloc_list);
						Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
							schedule_method, kernel_number, kernel_queue_list);
						return processors_available;
					}
					else
					{
						kernel_info_list[kernel_number].schedule_hardware = 2;
						kernel_info_list[kernel_number].rescheduled_execution = -1;
						kernel_info_list[kernel_number].completion_time = -1;
						kernel_info_list[kernel_number].scheduled_execution = -1;
						GLOBAL_CPU_KERNELS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Mode 3 AEAP: The Kernel:%d Cannot be scheduled Condition 1 & 2 Fail\n", kernel_number);
							printf("MODE 3 AEAP: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
						}
					}
					break;
				}
				else if (Pro < kernel_info_list[kernel_number].processor_req) {
					P_Given_list = insert_list(P_Given_list, temp->processors_allocated);
					temp->processors_allocated = 0;
					temp = temp->next;
				}
			}
		}
	}

	if (P_Given_list != NULL)
	{
		int count = 0;
		scheduledNode*temp1 = *processor_alloc_list;
		backup_list* temp2 = P_Given_list;
		while (temp2 != NULL)
		{
			if (count == 0) {
				processors_available = temp2->data;
				temp2 = temp2->next;
			}
			else {
				temp1->processors_allocated = temp2->data;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}
			count++;
		}
		P_Given_list = clean_list(P_Given_list);
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode 3 AEAP with ALAP:Backup processors reloaded\n");
		}
	}
	return processors_available;
}

static int Mode_3_Processors_Unavailable
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
		processors_available = Mode_3_AEAP(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list, kernel_queue_list);
	}
	else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST == NULL)
	{
		processors_available = Mode_3_ALAP(kernel_info_list, kernel_number, present_time, processors_available, processor_alloc_list, kernel_queue_list);
	}
	else if (kernel_info_list[kernel_number].processor_req >= PROCESSOR_LIMIT && GLOBAL_ALAP_LIST != NULL)
	{
		kernel_info_list[kernel_number].schedule_hardware = 2;
		kernel_info_list[kernel_number].rescheduled_execution = -1;
		kernel_info_list[kernel_number].completion_time = -1;
		kernel_info_list[kernel_number].scheduled_execution = -1;
		GLOBAL_CPU_KERNELS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("Mode-3 Processors Unavailable: Kernel:%d sent BACK TO CPU as ALAP is already set\n", kernel_number);
		}
	}

	return processors_available;
}

static int Mode_3_book_keeper
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
	int presentTime = present_time;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	static int Flag_Processors = 0;
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode-3 Book Keeper:: Kernel::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
			kernel_number, kernel_info_list[kernel_number].processor_req, kernel_info_list[kernel_number].execution_time,
			kernel_info_list[kernel_number].deadline, kernel_info_list[kernel_number].latest_schedulable_time);
	}

	// If processors available is greater than the required processors by the kernel_info_list
	if (kernel_info_list[kernel_number].processor_req <= processors_available)
	{
		if (GLOBAL_ALAP_LIST == NULL) {

			Flag_Processors = 0;

			if (kernel_info_list[kernel_number].execution_time + presentTime <= kernel_info_list[kernel_number].deadline)
			{

				processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
				processorReleased = kernel_info_list[kernel_number].processor_req;
				processor_release_time = kernel_info_list[kernel_number].execution_time + presentTime;
				schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
				// Kernel call for the GPU to handle the given Kernels and number of blocks//
				Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list);
				kernel_info_list[kernel_number].schedule_hardware = 1;
				kernel_info_list[kernel_number].rescheduled_execution = -1;
				kernel_info_list[kernel_number].scheduled_execution = presentTime;
				kernel_info_list[kernel_number].completion_time = kernel_info_list[kernel_number].execution_time + presentTime;
				GLOBAL_GPU_KERNELS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode-3 Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
				}
			}
			else
			{
				kernel_info_list[kernel_number].schedule_hardware = 2;
				kernel_info_list[kernel_number].rescheduled_execution = -1;
				kernel_info_list[kernel_number].completion_time = -1;
				kernel_info_list[kernel_number].scheduled_execution = -1;
				GLOBAL_CPU_KERNELS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode-3 Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
					printf("Mode-3 Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
				}
			}
		}
		else if (GLOBAL_ALAP_LIST != NULL)
		{
			int Pl = MAX_GPU_PROCESSOR - GLOBAL_ALAP_LIST->processors_allocated;
			if ((kernel_info_list[kernel_number].processor_req + Flag_Processors) <= Pl || (presentTime + kernel_info_list[kernel_number].execution_time) <= GLOBAL_ALAP_LIST->data)
			{
				Flag_Processors = Flag_Processors + kernel_info_list[kernel_number].processor_req;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode-3 Book Keeper:: Kernel:%d SATISFIED CONDITION 1 or 2", kernel_number);
				}
				if (kernel_info_list[kernel_number].execution_time + presentTime <= kernel_info_list[kernel_number].deadline)
				{

					processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
					processorReleased = kernel_info_list[kernel_number].processor_req;
					processor_release_time = kernel_info_list[kernel_number].execution_time + presentTime;
					schedule_method = 0;
					Queue_kernel_execution(processorReleased, processor_release_time, presentTime, schedule_method, kernel_number, processor_alloc_list);
					// TBD:: Kernel call for the GPU to handle the given Kernels and number of blocks//
					kernel_info_list[kernel_number].schedule_hardware = 1;
					kernel_info_list[kernel_number].rescheduled_execution = -1;
					kernel_info_list[kernel_number].scheduled_execution = presentTime;
					kernel_info_list[kernel_number].completion_time = kernel_info_list[kernel_number].execution_time + presentTime;
					GLOBAL_GPU_KERNELS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode-3 Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
					}
				}
				else
				{
					kernel_info_list[kernel_number].schedule_hardware = 2;
					kernel_info_list[kernel_number].rescheduled_execution = -1;
					kernel_info_list[kernel_number].completion_time = -1;
					kernel_info_list[kernel_number].scheduled_execution = -1;
					GLOBAL_CPU_KERNELS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Mode-3 Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
						printf("Mode-3 Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
					}
				}
			}
			else
			{
				kernel_info_list[kernel_number].schedule_hardware = 2;
				kernel_info_list[kernel_number].rescheduled_execution = -1;
				kernel_info_list[kernel_number].completion_time = -1;
				kernel_info_list[kernel_number].scheduled_execution = -1;
				GLOBAL_CPU_KERNELS++;
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Mode-3 Book Keeper:: Kernel-%d REJECTED ALAP kernel is set\n", kernel_number);
					printf("Mode-3 Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
				}
			}
		}
	}
	// If processors required is greater than the available processors by the kernel
	else if (kernel_info_list[kernel_number].processor_req > processors_available)
	{
		// Schedule the kernel_info_list to be released in a future time
		processors_available = Mode_3_Processors_Unavailable(kernel_info_list, kernel_number, present_time,
			processors_available, processor_alloc_list, kernel_queue_list);
	}

	return processors_available;
}

/**********************************************************************************************************
RTGS Mode 3 - AEAP with As Late As Possible mode->AEAP/ALAP
***********************************************************************************************************/
int RTGS_mode_3(char *kernelFilename, char *releaseTimeFilename)
{
	kernelInfo kernel_info_list[MAX_KERNELS] = {{0}};
	kernelReleaseInfo releaseTimeInfo[MAX_KERNELS] = {{0}};
	scheduledNode *processor_alloc_list = NULL;
	scheduledNode *kernel_queue_list = NULL;		//Kernel queued for future executions

	// global variable initialize
	GLOBAL_GPU_KERNELS = 0;
	GLOBAL_CPU_KERNELS = 0;
	GLOBAL_ALAP_LIST = NULL;

	int processorsAvailable = MAX_GPU_PROCESSOR;
	int kernel_number = 0;

	int kernelMax = get_kernel_information(kernel_info_list, kernelFilename);
	if (kernelMax <= RTGS_FAILURE) { return  RTGS_FAILURE; }
	int maxReleases = get_kernel_release_times(releaseTimeInfo, releaseTimeFilename);
	if (maxReleases <= RTGS_FAILURE) { return  RTGS_FAILURE; }

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("\n**************** The GPU Scheduler will Schedule %d Jobs ****************\n", kernelMax);
	}

	int numReleases = 0;
	for (int present_time = 0; present_time < MAX_RUN_TIME; present_time++)
	{
		// Freeing-up processors
		processorsAvailable = Retrieve_processors(present_time, processorsAvailable, &processor_alloc_list);
		if (processorsAvailable < 0) { printf("Retrieve_processors ERROR- Processors Available:%d\n", processorsAvailable); return RTGS_ERROR_NOT_IMPLEMENTED; }
		processorsAvailable = Dispatch_queued_kernels(present_time, processorsAvailable, &kernel_queue_list, &processor_alloc_list);
		if (processorsAvailable < 0) { printf("Dispatch_queued_kernels ERROR - Processors Available:%d\n", processorsAvailable); return RTGS_ERROR_NOT_IMPLEMENTED; }

		if (releaseTimeInfo[numReleases].release_time == present_time) {

			if (releaseTimeInfo[numReleases].num_kernel_released == 1)
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("\nRTGS Mode 3 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 3 -- Job-%d Released\n", kernel_number);
				}
				kernel_info_list[kernel_number].release_time = present_time;
				// handling the released kernel_info_list by the book-keeper
				int64_t start_t = RTGS_GetClockCounter();
				processorsAvailable = Mode_3_book_keeper(kernel_info_list, kernel_number, processorsAvailable, present_time,
					&processor_alloc_list, &kernel_queue_list);
				int64_t end_t = RTGS_GetClockCounter();
				int64_t freq = RTGS_GetClockFrequency();
				float factor = 1000.0f / (float)freq; // to convert clock counter to ms
				float SchedulerOverhead = (float)((end_t - start_t) * factor);
				kernel_info_list[kernel_number].schedule_overhead = SchedulerOverhead;
				kernel_number++;
			}
			else if (releaseTimeInfo[numReleases].num_kernel_released == 2)
			{
				int k1 = kernel_number; kernel_number++;
				int k2 = kernel_number; kernel_number++;
				kernel_info_list[k1].release_time = present_time;
				kernel_info_list[k2].release_time = present_time;

				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("\nRTGS Mode 3 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 3 -- Job-%d Released\n", k1);
					printf("RTGS Mode 3 -- Job-%d Released\n", k2);
				}
				if (kernel_info_list[k1].deadline <= kernel_info_list[k2].deadline)
				{
					// handling the released kernel_info_list by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_3_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k1].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_3_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k2].schedule_overhead = SchedulerOverhead;
				}
				else
				{
					// handling the released kernel_info_list by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_3_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k2].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_3_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k1].schedule_overhead = SchedulerOverhead;
				}
			}
			else if (releaseTimeInfo[numReleases].num_kernel_released > 2) { return RTGS_ERROR_NOT_IMPLEMENTED; }

			numReleases++;
			if (numReleases > maxReleases) {
				printf("RTGS Mode 3 ERROR --  KERNEL Release Time exceded Max Releases\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	if (maxReleases != 0) {

		if (GLOBAL_RTGS_DEBUG_MSG) {
			printf("\n******* Scheduler Mode 3 *******\n");
			printf("Processors Available -- %d\n", processorsAvailable);
			printf("Total Jobs Scheduled -- %d\n", kernelMax);
			printf("	GPU Scheduled Jobs -- %d\n", GLOBAL_GPU_KERNELS);
			printf("	CPU Scheduled Jobs -- %d\n", GLOBAL_CPU_KERNELS);
		}

		if (RTGS_PrintScheduleSummary(3, kernelMax, kernel_info_list)) {
			printf("\nSummary Failed\n");
		}

		if ((kernelMax != (GLOBAL_GPU_KERNELS + GLOBAL_CPU_KERNELS)) || processorsAvailable != MAX_GPU_PROCESSOR) {
			return RTGS_FAILURE;
		}

		for (int j = 0; j <= kernelMax; j++) {
			kernel_info_list[j].processor_req = kernel_info_list[j].deadline = kernel_info_list[j].execution_time = kernel_info_list[j].latest_schedulable_time = 0;
		}
		kernelMax = 0; maxReleases = 0; kernel_number = 0; GLOBAL_GPU_KERNELS = 0; GLOBAL_CPU_KERNELS = 0;
	}

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		print(processor_alloc_list);
	}
	processor_alloc_list = clean_node(processor_alloc_list);
	GLOBAL_ALAP_LIST = clean_list(GLOBAL_ALAP_LIST);

	return RTGS_SUCCESS;
}
