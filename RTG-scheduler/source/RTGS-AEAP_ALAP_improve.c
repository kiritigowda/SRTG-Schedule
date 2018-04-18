/*
* RTGS-AEAP_ALPA_improve.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP_ALAP_improve
(
	jobAttributes *jobAttributesList,
	int job_release_time,
	int present_time,
	int processors_available,
	scheduledJobNode ** processorsAllocatedList,
	scheduledJobNode **jobSchdeuleQueueList
)
{

	if (GLOBAL_RTGS_DEBUG_MSG > 2) {
		printf("AEAP_ALAP_IMPROVE: ALAP IMPROVED scheduling\n");
		printf("AEAP_ALAP_IMPROVE: Job Release Time: %d\n", job_release_time);
	}
	scheduledJobNode *temp = *jobSchdeuleQueueList;
	while (temp->jobNumber != GLOBAL_ALAP_LIST->jobNumber && temp->next != NULL)
		temp = temp->next;

	if (temp->jobNumber == GLOBAL_ALAP_LIST->jobNumber && temp->processors_allocated == GLOBAL_ALAP_LIST->processors_allocated)
	{
		temp->data = temp->job_release_time = GLOBAL_ALAP_LIST->data = job_release_time;
		temp->processor_release_time = GLOBAL_ALAP_LIST->processor_release_time = job_release_time + jobAttributesList[temp->jobNumber].execution_time;
	}

	return processors_available;
}
