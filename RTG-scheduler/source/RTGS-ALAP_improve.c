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
	
	if (GLOBAL_preScheduleList == NULL) {
		printf("As Late As Possible Improved (ALAP-I) -- Job:%d BAD ENTRY\n", jobNumber);
		return RTGS_FAILURE;
	}
	if (GLOBAL_RTGS_DEBUG_MSG > 2) {
		printf("As Late As Possible Improved (ALAP-I) -- Job-%d is verified, ALAP IMPROVED called\n", jobNumber);
	}

	scheduledResourceNode *localJobScheduledQueueList = *jobScheduledQueueList;
	genericBackupNode *localPreScheduleList = GLOBAL_preScheduleList;


	if (localJobScheduledQueueList->jobNumber != MULTIPLE_JOBS_SCHEDULED)
	{
		if ((localPreScheduleList->data == localJobScheduledQueueList->data) && (localPreScheduleList->jobNumber == localJobScheduledQueueList->jobNumber) &&
			processors_available >= localPreScheduleList->processors_requested)
		{
			int jobImproveID = localJobScheduledQueueList->jobNumber;
			if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("As Late Aa Possible Improved (ALAP-I) -- Job-%d is verified for ALAP IMPROVED scheduling\n", jobImproveID);
				printf("ALAP-I -- GLOBAL_preScheduleList->data: %d && jobScheduledQueueList: %d\n", localPreScheduleList->data, localJobScheduledQueueList->data);
			}
			int completionTime = present_time + jobAttributesList[jobImproveID].execution_time;
			// job attribute updates
			jobAttributesList[jobImproveID].rescheduled_execution = present_time;
			jobAttributesList[jobImproveID].completion_time = completionTime;
			// change job release and completion time
			localPreScheduleList->data = localJobScheduledQueueList->data = present_time;
			localJobScheduledQueueList->job_release_time = present_time;
			localPreScheduleList->processor_release_time = localJobScheduledQueueList->processor_release_time = completionTime;
			// dispatch job for execution
			processors_available = Dispatch_queued_kernels(present_time, processors_available, jobScheduledQueueList, processorsAllocatedList);
		}
	}
	else
	{
		if (localPreScheduleList->data == localJobScheduledQueueList->data && 
			processors_available >= localJobScheduledQueueList->processors_allocated)
		{
			scheduledResourceNode *t1, *t2;
			t1 = localJobScheduledQueueList->job_next;
			while (t1 != NULL)
			{
				t2 = t1->job_next;
				int jobImproveID = t1->jobNumber;

				genericBackupNode *local_pre = localPreScheduleList;
				while (local_pre->jobNumber != jobImproveID)
					local_pre = local_pre->next;

				if (jobImproveID == local_pre->jobNumber &&
					local_pre->data == t1->data)
				{
					if (GLOBAL_RTGS_DEBUG_MSG > 2) {
						printf("As Late Aa Possible Improved (ALAP-I) -- Job-%d is verified for ALAP IMPROVED scheduling\n", jobImproveID);
						printf("ALAP-I -- GLOBAL_preScheduleList->data: %d && jobScheduledQueueList: %d\n", localPreScheduleList->data, t1->data);
					}
					int completionTime = present_time + jobAttributesList[jobImproveID].execution_time;
					// job attribute updates
					jobAttributesList[jobImproveID].rescheduled_execution = present_time;
					jobAttributesList[jobImproveID].completion_time = completionTime;
					// change job release and completion time
					local_pre->data = t1->data = present_time;
					t1->job_release_time = present_time;
					local_pre->processor_release_time = t1->processor_release_time = completionTime;
				}
				t1 = t2;
			}
			localJobScheduledQueueList->job_release_time = localJobScheduledQueueList->data = present_time;
			// dispatch job for execution
			processors_available = Dispatch_queued_kernels(present_time, processors_available, jobScheduledQueueList, processorsAllocatedList);
		}
	}
	PROFILER_STOP(SRTG, ALAP_improve)
	return processors_available;
}
