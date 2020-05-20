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
    scheduledResourceNode **processorsAllocatedList
)
{
    PROFILER_START(SRTG, Retrieve_processors)
    scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;

    if (localProcessorsAllocatedList != NULL)
    {
        if (localProcessorsAllocatedList->data == present_time)
        {
            processors_available = processors_available + localProcessorsAllocatedList->processors_allocated;
            if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                // TBD:: Data return handling needed
                printf("\nRetrieve Processors - Present Time:%d, Processors Available:%d\n", present_time, processors_available);
                printf("Retrieve Processors - GPU Execution Completed for Job-%d Processors Retrieved:%d\n", localProcessorsAllocatedList->jobNumber, localProcessorsAllocatedList->processors_allocated);
            }

            if (localProcessorsAllocatedList->job_next != NULL)
            {
                scheduledResourceNode *t1, *t2;
                t1 = localProcessorsAllocatedList->job_next;
                while (t1 != NULL)
                {
                    if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                        printf("Retrieve Processors -- Job-%d data sent back\n", t1->jobNumber);
                    }
                    t2 = t1->job_next;
                    free(t1);
                    t1 = t2;
                }
            }
            else {
                if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                    printf("Retrieve Processors -- Job-%d data sent back\n", localProcessorsAllocatedList->jobNumber);
                }
            }

            localProcessorsAllocatedList = position_delete(*processorsAllocatedList, 1);
            *processorsAllocatedList = localProcessorsAllocatedList;
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
    scheduledResourceNode **jobScheduledQueueList,
    scheduledResourceNode **processorsAllocatedList
)
{
    PROFILER_START(SRTG, Dispatch_queued_kernels)
    scheduledResourceNode *localJobScheduledQueueList = *jobScheduledQueueList;

    if (localJobScheduledQueueList != NULL)
    {
        if (localJobScheduledQueueList->data == present_time)
        {
            if (localJobScheduledQueueList->job_next != NULL)
            {
                scheduledResourceNode *t1, *t2;
                t1 = localJobScheduledQueueList->job_next;
                while (t1 != NULL)
                {
                    t2 = t1->job_next;
                    if (t1->schedule_method == RTGS_SCHEDULE_METHOD_ALAP)
                    {
                        int processorsAllocatedALAP = 0;
                        if (GLOBAL_preScheduleList != NULL) {
                            genericBackupNode *localListALAP = GLOBAL_preScheduleList;
                            int jobCount = 1;
                            while (localListALAP != NULL) {
                                if (t1->jobNumber == localListALAP->jobNumber && localListALAP->data == present_time) {
                                    processorsAllocatedALAP = localListALAP->processors_requested;
                                    GLOBAL_preScheduleList = position_delete_preScheduledJob(GLOBAL_preScheduleList, jobCount);
                                    break;
                                }
                                jobCount++;
                                localListALAP = localListALAP->next;
                            }
                        }
                        else {
                            printf("Dispatch Queued Jobs -- RTGS_SCHEDULE_METHOD_ALAP -- ERROR At TIME: %d while dispatching Job-%d\n", present_time, t1->jobNumber);
                            return RTGS_FAILURE;
                        }
                        processors_available = processors_available - processorsAllocatedALAP;
                        queue_job_execution(processorsAllocatedALAP, t1->processor_release_time, present_time,
                                            t1->schedule_method, t1->jobNumber, processorsAllocatedList);

                        if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                            printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_ALAP Job-%d ProcAlloc:%d for GPU EXECUTION\n",
                                   present_time, t1->jobNumber, processorsAllocatedALAP);
                        }
                    }
                    else if (t1->schedule_method == RTGS_SCHEDULE_METHOD_AEAP_ADVANCED)
                    {
                        int processorsAllocatedAEAP_ADV = 0;
                        if (GLOBAL_preScheduleList != NULL) {
                            genericBackupNode *localListAEAP_ADV = GLOBAL_preScheduleList;
                            int jobCount = 1;
                            while (localListAEAP_ADV != NULL) {
                                if (t1->jobNumber == localListAEAP_ADV->jobNumber && localListAEAP_ADV->data == present_time) {
                                    processorsAllocatedAEAP_ADV = localListAEAP_ADV->processors_requested;
                                    GLOBAL_preScheduleList = position_delete_preScheduledJob(GLOBAL_preScheduleList, jobCount);
                                    break;
                                }
                                jobCount++;
                                localListAEAP_ADV = localListAEAP_ADV->next;
                            }
                        }
                        else {
                            printf("Dispatch Queued Jobs -- RTGS_SCHEDULE_METHOD_AEAP_ADVANCED -- ERROR At TIME: %d while dispatching Job-%d\n", present_time, t1->jobNumber);
                            return RTGS_FAILURE;
                        }

                        processors_available = processors_available - processorsAllocatedAEAP_ADV;
                        queue_job_execution(processorsAllocatedAEAP_ADV, t1->processor_release_time, present_time,
                                            t1->schedule_method, t1->jobNumber, processorsAllocatedList);

                        if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                            printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_AEAP_ADVANCED Job-%d ProcAlloc:%d for GPU EXECUTION\n",
                                   present_time, t1->jobNumber, t1->processors_allocated);
                        }
                    }
                    else if (t1->schedule_method == RTGS_SCHEDULE_METHOD_AEAP) {
                        if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                            printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_AEAP Job-%d ProcAlloc:%d for GPU EXECUTION\n",
                                   present_time, t1->jobNumber, t1->processors_allocated);
                        }
                    }
                    else {
                        printf("Dispatch Queued Jobs -- ERROR NOT IMPLEMENTED");
                        return RTGS_ERROR_NOT_IMPLEMENTED;
                    }
                    free(t1);
                    t1 = t2;
                }
            }
            else
            {
                if (localJobScheduledQueueList->schedule_method == RTGS_SCHEDULE_METHOD_ALAP)
                {
                    int processorsAllocatedALAP = 0;
                    if (GLOBAL_preScheduleList != NULL) {
                        genericBackupNode *localListALAP = GLOBAL_preScheduleList;
                        int jobCount = 1;
                        while (localListALAP != NULL) {
                            if (localJobScheduledQueueList->jobNumber == localListALAP->jobNumber && localListALAP->data == present_time) {
                                processorsAllocatedALAP = localListALAP->processors_requested;
                                GLOBAL_preScheduleList = position_delete_preScheduledJob(GLOBAL_preScheduleList, jobCount);
                                break;
                            }
                            jobCount++;
                            localListALAP = localListALAP->next;
                        }
                    }
                    else {
                        printf("Dispatch Queued Jobs -- RTGS_SCHEDULE_METHOD_ALAP -- ERROR At TIME: %d while dispatching Job-%d\n", present_time, localJobScheduledQueueList->jobNumber);
                        return RTGS_FAILURE;
                    }
                    processors_available = processors_available - processorsAllocatedALAP;
                    queue_job_execution(processorsAllocatedALAP, localJobScheduledQueueList->processor_release_time, present_time,
                                        localJobScheduledQueueList->schedule_method, localJobScheduledQueueList->jobNumber, processorsAllocatedList);

                    if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                        printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_ALAP Job-%d ProcAlloc:%d for GPU EXECUTION\n",
                               present_time, localJobScheduledQueueList->jobNumber, processorsAllocatedALAP);
                    }
                }
                else if (localJobScheduledQueueList->schedule_method == RTGS_SCHEDULE_METHOD_AEAP_ADVANCED)
                {
                    int processorsAllocatedAEAP_ADV = 0;
                    if (GLOBAL_preScheduleList != NULL) {
                        genericBackupNode *localListAEAP_ADV = GLOBAL_preScheduleList;
                        int jobCount = 1;
                        while (localListAEAP_ADV != NULL) {
                            if (localJobScheduledQueueList->jobNumber == localListAEAP_ADV->jobNumber && localListAEAP_ADV->data == present_time) {
                                processorsAllocatedAEAP_ADV = localListAEAP_ADV->processors_requested;
                                GLOBAL_preScheduleList = position_delete_preScheduledJob(GLOBAL_preScheduleList, jobCount);
                                break;
                            }
                            jobCount++;
                            localListAEAP_ADV = localListAEAP_ADV->next;
                        }
                    }
                    else {
                        printf("Dispatch Queued Jobs -- RTGS_SCHEDULE_METHOD_AEAP_ADVANCED -- ERROR At TIME: %d while dispatching Job-%d\n", present_time, localJobScheduledQueueList->jobNumber);
                        return RTGS_FAILURE;
                    }
                    processors_available = processors_available - processorsAllocatedAEAP_ADV;
                    queue_job_execution(processorsAllocatedAEAP_ADV, localJobScheduledQueueList->processor_release_time, present_time,
                                        localJobScheduledQueueList->schedule_method, localJobScheduledQueueList->jobNumber, processorsAllocatedList);

                    if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                        printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_AEAP_ADVANCED Job-%d ProcAlloc:%d for GPU EXECUTION\n",
                               present_time, localJobScheduledQueueList->jobNumber, processorsAllocatedAEAP_ADV);
                    }
                }
                else if (localJobScheduledQueueList->schedule_method == RTGS_SCHEDULE_METHOD_AEAP) {
                    if (GLOBAL_RTGS_DEBUG_MSG > 2) {
                        printf("Dispatch Queued Jobs -- Present Time:%d Dispatched RTGS_SCHEDULE_METHOD_AEAP Job-%d ProcAlloc:%d for GPU EXECUTION\n",
                               present_time, localJobScheduledQueueList->jobNumber, localJobScheduledQueueList->processors_allocated);
                    }
                }
                else {
                    printf("Dispatch Queued Jobs -- ERROR NOT IMPLEMENTED");
                    return RTGS_ERROR_NOT_IMPLEMENTED;
                }
            }
            localJobScheduledQueueList = position_delete(*jobScheduledQueueList, 1);
            *jobScheduledQueueList = localJobScheduledQueueList;
        }
    }
    PROFILER_STOP(SRTG, Dispatch_queued_kernels)
    return processors_available;
}
