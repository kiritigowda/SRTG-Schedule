/*
* RTGS-ALAP_improve.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP_improve
(
	kernelInfo *kernel_info_list,
	int kernel_number,
	int present_time,
	int processors_available,
	scheduledNode ** processor_alloc_list,
	scheduledNode **kernel_queue_list
)
{
	PROFILER_START(SRTG, ALAP_improve)
	if (GLOBAL_RTGS_DEBUG_MSG > 2) {
		printf("As Late Aa Possible Improved (ALAP-I) -- Job-%d is verified for ALAP IMPROVED scheduling\n", kernel_number);
	}
	scheduledNode *temp = *kernel_queue_list;

	if (temp->kernel_number == kernel_number)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("As Late Aa Possible Improved (ALAP-I) -- Job-%d is verified for ALAP IMPROVED scheduling\n", kernel_number);
			printf("ALAP-I -- GLOBAL_ALAP_LIST->data: %d && kernel_queue_list: %d\n", GLOBAL_ALAP_LIST->data, temp->data);
		}
		if (GLOBAL_ALAP_LIST->data == temp->data)
		{
			GLOBAL_ALAP_LIST->data = temp->data = present_time;
			temp->processor_release_time = present_time + kernel_info_list[kernel_number].execution_time;
			processors_available = Dispatch_queued_kernels(present_time, processors_available, kernel_queue_list, processor_alloc_list);
		}
	}
	else if (temp->kernel_number == MULTIPLE_KERNELS_SCHEDULED)
	{
		scheduledNode *head = temp;
		scheduledNode *t1, *t2;
		t1 = temp->kernel_next;
		while (t1 != NULL)
		{
			t2 = t1->kernel_next;
			if (t1->kernel_number == kernel_number && GLOBAL_ALAP_LIST->data == t1->data)
			{
				head->kernel_next = t2;
				GLOBAL_ALAP_LIST->data = t1->data = present_time;
				t1->processor_release_time = present_time + kernel_info_list[kernel_number].execution_time;
				GLOBAL_ALAP_LIST = position_delete_list(GLOBAL_ALAP_LIST);
				processors_available = processors_available - t1->processors_allocated;
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Late Aa Possible Improved (ALAP-I) -- MULTIPLE_KERNELS_SCHEDULED\n");
					printf("ALAP-I --  TIME: %d<--Dispatch-- schedule_method:RTGS_SCHEDULE_METHOD_ALAP  Job-%d sent to GPU for EXECUTION-->\n", present_time, t1->kernel_number);
				}
				Queue_kernel_execution(t1->processors_allocated, t1->processor_release_time, present_time,
					t1->schedule_method, t1->kernel_number, processor_alloc_list);

				free(t1);
				PROFILER_STOP(SRTG, ALAP_improve)
				return processors_available;
			}
			else
				head = t1;

			t1 = t2;
		}
	}
	PROFILER_STOP(SRTG, ALAP_improve)
	return processors_available;
}
