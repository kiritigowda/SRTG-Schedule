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
	scheduledJobNode** processorsAllocatedList
)
{
	PROFILER_START(SRTG, Retrieve_processors)
	scheduledJobNode* temp;
	temp = *processorsAllocatedList;

	if (temp != NULL)
	{
		if (temp->data == present_time)
		{
			processors_available = processors_available + temp->processors_allocated;
			if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				// TBD:: Data return handling needed
				printf("\nRetrieve Processors -- Present Time:%d, Processors Available:%d\n", present_time, processors_available);
				printf("Retrieve Processors -- GPU Execution Completed for Job-%d Processors Retrieved:%d\n", temp->jobNumber, temp->processors_allocated);
			}
			if (temp->job_next != NULL)
			{
				scheduledJobNode *t1, *t2;
				t1 = temp->job_next;
				while (t1 != NULL)
				{
					t2 = t1->job_next;
					free(t1);
					t1 = t2;

				}
			}
			else
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					// TBD:: Data return handling needed
					printf("Retrieve Processors -- Job-%d data sent back\n", temp->jobNumber);
				}
			}
			temp = position_delete(*processorsAllocatedList, 1);
			*processorsAllocatedList = temp;
		}
	}
	PROFILER_STOP(SRTG, Retrieve_processors)
	return processors_available;
}

/* Function to dispatch jobs for execution  */
int Dispatch_queued_kernels
(
	int present_time,
	int processors_available,
	scheduledJobNode** jobSchdeuleQueueList,
	scheduledJobNode **processorsAllocatedList
)
{
	PROFILER_START(SRTG, Dispatch_queued_kernels)
	scheduledJobNode* temp;
	temp = *jobSchdeuleQueueList;

	if (temp != NULL)
	{
		if (temp->data == present_time)
		{
			if (temp->job_next != NULL)
			{
				scheduledJobNode *t1, *t2;
				t1 = temp->job_next;
				while (t1 != NULL)
				{
					t2 = t1->job_next;
					if (t1->schedule_method == RTGS_SCHEDULE_METHOD_ALAP)
					{
						int ALAP_Pg = 0;
						if (GLOBAL_ALAP_LIST != NULL)
						{
							jobBackupNode* temp1 = GLOBAL_ALAP_LIST;
							if (temp1->data == present_time)
							{
								ALAP_Pg = temp1->processors_allocated;
								GLOBAL_ALAP_LIST = position_delete_list(GLOBAL_ALAP_LIST);
							}
							else
							{
								if (GLOBAL_RTGS_DEBUG_MSG > 2) {
									printf("Dispatch Queued Jobs -- ERROR At TIME: %d while dispatching Job-%d\n", present_time, t1->jobNumber);
								}
								return processors_available;
							}
						}
						processors_available = processors_available - t1->processors_allocated;
						if (GLOBAL_RTGS_DEBUG_MSG > 2) {
							printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_ALAP Job-%d ProcAlloc:%d for GPU EXECUTION\n",
								present_time, t1->jobNumber, t1->processors_allocated);
						}
						Queue_kernel_execution(ALAP_Pg, t1->processor_release_time, present_time,
							t1->schedule_method, t1->jobNumber, processorsAllocatedList);
					}
					else if (t1->schedule_method == RTGS_SCHEDULE_METHOD_AEAP)
					{
						if (GLOBAL_RTGS_DEBUG_MSG > 2) {
							printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_AEAP Job-%d ProcAlloc:%d for GPU EXECUTION\n",
								present_time, t1->jobNumber, t1->processors_allocated);
						}
					}
					else { 
						printf("Dispatch Queued Jobs -- ERROR NOT IMPLEMENTED"); return RTGS_ERROR_NOT_IMPLEMENTED; 
					}

					free(t1);
					t1 = t2;
				}
			}
			else
			{
				if (temp->schedule_method == RTGS_SCHEDULE_METHOD_ALAP)
				{
					int ALAP_Pg = 0;
					if (GLOBAL_ALAP_LIST != NULL)
					{
						jobBackupNode* temp1 = GLOBAL_ALAP_LIST;
						if (temp1->data == present_time)
						{
							ALAP_Pg = temp1->processors_allocated;
							GLOBAL_ALAP_LIST = position_delete_list(GLOBAL_ALAP_LIST);
						}
						else
						{
							if (GLOBAL_RTGS_DEBUG_MSG > 2) {
								printf("Dispatch Queued Jobs -- ERROR At TIME:%d Dispatch Job-%d \n", present_time, temp->jobNumber);
							}
							return processors_available;
						}
					}
					processors_available = processors_available - temp->processors_allocated;
					if (GLOBAL_RTGS_DEBUG_MSG > 2) {
						printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_ALAP Job-%d ProcAlloc:%d for GPU EXECUTION\n",
							present_time, temp->jobNumber, temp->processors_allocated);
					}
					Queue_kernel_execution(ALAP_Pg, temp->processor_release_time, present_time,
						temp->schedule_method, temp->jobNumber, processorsAllocatedList);
				}
				else if (temp->schedule_method == RTGS_SCHEDULE_METHOD_AEAP)
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 2) {
						printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_AEAP Job-%d ProcAlloc:%d for GPU EXECUTION\n",
							present_time, temp->jobNumber, temp->processors_allocated);
					}
				}
				else { 
					printf("Dispatch Queued Jobs -- ERROR NOT IMPLEMENTED"); return RTGS_ERROR_NOT_IMPLEMENTED; 
				}
			}
			temp = position_delete(*jobSchdeuleQueueList, 1);
			*jobSchdeuleQueueList = temp;
		}
	}
	PROFILER_STOP(SRTG, Dispatch_queued_kernels)
	return processors_available;
}

