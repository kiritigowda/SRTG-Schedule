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
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{

	if (GLOBAL_RTGS_DEBUG_MSG > 2) {
		printf("AEAP_ALAP_IMPROVE: ALAP IMPROVED scheduling\n");
		printf("AEAP_ALAP_IMPROVE: Job Release Time: %d\n", job_release_time);
	}
	scheduledResourceNode *temp = *jobScheduledQueueList;
	while (temp->jobNumber != GLOBAL_preScheduleList->jobNumber && temp->next != NULL)
		temp = temp->next;

	if (temp->jobNumber == GLOBAL_preScheduleList->jobNumber && temp->processors_allocated == GLOBAL_preScheduleList->processors_allocated)
	{
		temp->data = temp->job_release_time = GLOBAL_preScheduleList->data = job_release_time;
		temp->processor_release_time = GLOBAL_preScheduleList->processor_release_time = job_release_time + jobAttributesList[temp->jobNumber].execution_time;
	}

	return processors_available;
}
