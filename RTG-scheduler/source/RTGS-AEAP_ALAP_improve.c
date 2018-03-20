/*
* RTGS-AEAP_ALPA_improve.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP_ALAP_improve
(
	kernelInfo *kernel_info_list,
	int kernel_release_time,
	int present_time,
	int processors_available,
	scheduledNode ** processor_alloc_list,
	scheduledNode **kernel_queue_list
)
{

	if (GLOBAL_RTGS_DEBUG_MSG > 2) {
		printf("AEAP_ALAP_IMPROVE:ALAP IMPROVED scheduling\n");
		printf("AEAP_ALAP_IMPROVE: Job Release Time: %d\n", kernel_release_time);
	}
	scheduledNode *temp = *kernel_queue_list;
	while (temp->kernel_number != GLOBAL_ALAP_LIST->kernel_number && temp->next != NULL)
		temp = temp->next;

	if (temp->kernel_number == GLOBAL_ALAP_LIST->kernel_number && temp->processors_allocated == GLOBAL_ALAP_LIST->processors_allocated)
	{
		temp->data = temp->kernel_release_time = GLOBAL_ALAP_LIST->data = kernel_release_time;
		temp->processor_release_time = GLOBAL_ALAP_LIST->processor_release_time = kernel_release_time + kernel_info_list[temp->kernel_number].execution_time;
	}

	return processors_available;
}
