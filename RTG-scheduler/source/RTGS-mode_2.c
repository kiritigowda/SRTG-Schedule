/*
* RTGS-mode_2.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/**********************************************************************************************************
MODE 2 Helper Functions
***********************************************************************************************************/
static int Mode_2_AEAP
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
	backup_list *P_Given_list = NULL;
	Pro = processors_available;
	P_Given_list = insert_list(P_Given_list, processors_available);
	processors_available = 0;
	scheduledNode* temp = *processor_alloc_list;

	while (temp != NULL)
	{
		if ((temp->processor_release_time + kernel_info_list[kernel_number].execution_time) > kernel_info_list[kernel_number].deadline)
		{
			int AEAP_count = 0;
			scheduledNode*temp1 = *processor_alloc_list;
			backup_list* temp2 = P_Given_list;
			while (temp2 != NULL)
			{
				if (AEAP_count == 0) {
					processors_available = temp2->data;
					temp2 = temp2->next;
				}
				else {
					temp1->processors_allocated = temp2->data;
					temp1 = temp1->next;
					temp2 = temp2->next;
				}
				AEAP_count++;
			}
			// TBD:: Return Kernel to CPU - Function to send Kernel to CPU execution 
			kernel_info_list[kernel_number].schedule_hardware = 2;
			kernel_info_list[kernel_number].rescheduled_execution = -1;
			kernel_info_list[kernel_number].completion_time = -1;
			kernel_info_list[kernel_number].scheduled_execution = -1;
			GLOBAL_CPU_KERNELS++;
			P_Given_list = clean_list(P_Given_list);
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("MODE-2 AEAP:: The Kernel:%d Cannot be scheduled AEAP\n", kernel_number);
				printf("MODE-2 AEAP:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
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
					printf("MODE-2 AEAP:: The Kernel:%d scheduled AEAP\n", kernel_number);
					printf("MODE-2 AEAP:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
				}
				Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
					schedule_method, kernel_number, processor_alloc_list);
				Kernel_queue_handler(processorReleased, kernel_release_time, presentTime,
					schedule_method, kernel_number, kernel_queue_list);
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

	if (temp == NULL && P_Given_list != NULL)
	{
		int AEAP_count = 0;
		scheduledNode* temp1 = *processor_alloc_list;
		backup_list* temp2 = P_Given_list;
		while (temp2 != NULL)
		{
			if (AEAP_count == 0) {
				processors_available = temp2->data;
				temp2 = temp2->next;
			}
			else {
				temp1->processors_allocated = temp2->data;
				temp1 = temp1->next;
				temp2 = temp2->next;
			}
			AEAP_count++;
		}
		P_Given_list = clean_list(P_Given_list);
		kernel_info_list[kernel_number].schedule_hardware = 2;
		kernel_info_list[kernel_number].rescheduled_execution = -1;
		kernel_info_list[kernel_number].completion_time = -1;
		kernel_info_list[kernel_number].scheduled_execution = -1;
		GLOBAL_CPU_KERNELS++;
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("MODE-2 AEAP:: The Kernel:%d Cannot be scheduled AEAP\n", kernel_number);
			printf("MODE-2 AEAP:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
		}

	}

	return processors_available;
}

static int Mode_2_Processors_Unavailable
(
	kernelInfo *kernel_info_list,
	int kernel_number,
	int present_time,
	int processors_available,
	scheduledNode ** processor_alloc_list,
	scheduledNode **kernel_queue_list
)
{
	processors_available = Mode_2_AEAP(kernel_info_list, kernel_number,
		present_time, processors_available,
		processor_alloc_list, kernel_queue_list);
	return processors_available;
}

static int Mode_2_book_keeper
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
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Mode-2 Book Keeper:: Kernel::%d --> processor_req:%d execution_time:%d, deadline:%d, latest_schedulable_time:%d\n",
			kernel_number, kernel_info_list[kernel_number].processor_req, kernel_info_list[kernel_number].execution_time,
			kernel_info_list[kernel_number].deadline, kernel_info_list[kernel_number].latest_schedulable_time);
	}
	// If processors available is greater than the required processors by the kernel_info_list
	if (kernel_info_list[kernel_number].processor_req <= processors_available)
	{
		if (kernel_info_list[kernel_number].execution_time + presentTime <= kernel_info_list[kernel_number].deadline) {
			processors_available = processors_available - kernel_info_list[kernel_number].processor_req;
			processorReleased = kernel_info_list[kernel_number].processor_req;
			processor_release_time = kernel_info_list[kernel_number].execution_time + presentTime;
			schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
			// Kernel call for the GPU to handle the given Kernels and number of blocks
			Queue_kernel_execution(processorReleased, processor_release_time, presentTime,
				schedule_method, kernel_number, processor_alloc_list);
			kernel_info_list[kernel_number].schedule_hardware = 1;
			kernel_info_list[kernel_number].rescheduled_execution = -1;
			kernel_info_list[kernel_number].scheduled_execution = present_time;
			kernel_info_list[kernel_number].completion_time = kernel_info_list[kernel_number].execution_time + present_time;
			GLOBAL_GPU_KERNELS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode-2 Book Keeper:: Kernels ACCEPTED count --> %d\n", GLOBAL_GPU_KERNELS);
			}
		}
		else {
			kernel_info_list[kernel_number].schedule_hardware = 2;
			kernel_info_list[kernel_number].rescheduled_execution = -1;
			kernel_info_list[kernel_number].completion_time = -1;
			kernel_info_list[kernel_number].scheduled_execution = -1;
			GLOBAL_CPU_KERNELS++;
			if (GLOBAL_RTGS_DEBUG_MSG > 1) {
				printf("Mode-2 Book Keeper:: Kernel-%d will not complete before it's deadline, Job REJECTED\n", kernel_number);
				printf("Mode-2 Book Keeper:: Kernels REJECTED count --> %d\n", GLOBAL_CPU_KERNELS);
			}
		}
	}
	// If processors available is greater than the required processors by the kernel_info_list
	else if (kernel_info_list[kernel_number].processor_req > processors_available)
	{
		// Schedule the kernel to be released in a future time
		processors_available = Mode_2_Processors_Unavailable(kernel_info_list, kernel_number,
			present_time, processors_available,
			processor_alloc_list, kernel_queue_list);
	}

	return processors_available;
}

/**********************************************************************************************************
RTGS Mode 2 - As Early As Possible mode->AEAP
***********************************************************************************************************/
int RTGS_mode_2(char *kernelFilename, char *releaseTimeFilename)
{
	kernelInfo kernel_info_list[MAX_KERNELS] = {{0}};
	kernelReleaseInfo releaseTimeInfo[MAX_KERNELS] = {{0}};
	scheduledNode *processor_alloc_list = NULL;
	scheduledNode *kernel_queue_list = NULL;

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
					printf("\nRTGS Mode 2 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 2 -- Job-%d Released\n", kernel_number);
				}
				kernel_info_list[kernel_number].release_time = present_time;
				// handling the released kernel_info_list by the book-keeper
				int64_t start_t = RTGS_GetClockCounter();
				processorsAvailable = Mode_2_book_keeper(kernel_info_list, kernel_number, processorsAvailable, present_time,
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
					printf("\nRTGS Mode 2 -- Total Processors Available at time %d = %d\n", present_time, processorsAvailable);
					printf("RTGS Mode 2 -- Job-%d Released\n", k1);
					printf("RTGS Mode 2 -- Job-%d Released\n", k2);
				}
				if (kernel_info_list[k1].deadline <= kernel_info_list[k2].deadline)
				{
					// handling the released kernel_info_list by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_2_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k1].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_2_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k2].schedule_overhead = SchedulerOverhead;
				}
				else
				{
					// handling the released kernel_info_list by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_2_book_keeper(kernel_info_list, k2, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k2].schedule_overhead = SchedulerOverhead;
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Mode_2_book_keeper(kernel_info_list, k1, processorsAvailable, present_time, &processor_alloc_list, &kernel_queue_list);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					kernel_info_list[k1].schedule_overhead = SchedulerOverhead;
				}
			}
			else if (releaseTimeInfo[numReleases].num_kernel_released > 2) { return RTGS_ERROR_NOT_IMPLEMENTED; }

			numReleases++;
			if (numReleases > maxReleases) {
				printf("RTGS Mode 2 ERROR --  KERNEL Release Time exceded Max Releases\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	if (maxReleases != 0) {

		if (GLOBAL_RTGS_DEBUG_MSG) {
			printf("\n******* Scheduler Mode 2 *******\n");
			printf("Processors Available -- %d\n", processorsAvailable);
			printf("Total Jobs Scheduled -- %d\n", kernelMax);
			printf("	GPU Scheduled Jobs -- %d\n", GLOBAL_GPU_KERNELS);
			printf("	CPU Scheduled Jobs -- %d\n", GLOBAL_CPU_KERNELS);
		}

		if (RTGS_PrintScheduleSummary(2, kernelMax, kernel_info_list)) {
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
