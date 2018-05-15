/*
* RTGS-ALAP_improve.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int ALAP_improve
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	PROFILER_START(SRTG, ALAP_improve)
	if (GLOBAL_RTGS_DEBUG_MSG > 2) {
		printf("As Late Aa Possible Improved (ALAP-I) -- Job-%d is verified for ALAP IMPROVED scheduling\n", jobNumber);
	}
	scheduledResourceNode *temp = *jobScheduledQueueList;

	if (temp->jobNumber == jobNumber)
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 2) {
			printf("As Late Aa Possible Improved (ALAP-I) -- Job-%d is verified for ALAP IMPROVED scheduling\n", jobNumber);
			printf("ALAP-I -- GLOBAL_preScheduleList->data: %d && jobScheduledQueueList: %d\n", GLOBAL_preScheduleList->data, temp->data);
		}
		if (GLOBAL_preScheduleList->data == temp->data)
		{
			GLOBAL_preScheduleList->data = temp->data = present_time;
			temp->processor_release_time = present_time + jobAttributesList[jobNumber].execution_time;
			processors_available = Dispatch_queued_kernels(present_time, processors_available, jobScheduledQueueList, processorsAllocatedList);
		}
	}
	else if (temp->jobNumber == MULTIPLE_JOBS_SCHEDULED)
	{
		scheduledResourceNode *head = temp;
		scheduledResourceNode *t1, *t2;
		t1 = temp->job_next;
		while (t1 != NULL)
		{
			t2 = t1->job_next;
			if (t1->jobNumber == jobNumber && GLOBAL_preScheduleList->data == t1->data)
			{
				head->job_next = t2;
				GLOBAL_preScheduleList->data = t1->data = present_time;
				t1->processor_release_time = present_time + jobAttributesList[jobNumber].execution_time;
				GLOBAL_preScheduleList = position_delete_list(GLOBAL_preScheduleList);
				processors_available = processors_available - t1->processors_allocated;
				if (GLOBAL_RTGS_DEBUG_MSG > 2) {
					printf("As Late Aa Possible Improved (ALAP-I) -- MULTIPLE_JOBS_SCHEDULED\n");
					printf("ALAP-I --  TIME: %d<--Dispatch-- schedule_method:RTGS_SCHEDULE_METHOD_ALAP  Job-%d sent to GPU for EXECUTION-->\n", present_time, t1->jobNumber);
				}
				queue_job_execution(t1->processors_allocated, t1->processor_release_time, present_time,
					t1->schedule_method, t1->jobNumber, processorsAllocatedList);

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
