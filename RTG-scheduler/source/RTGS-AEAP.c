/*
* RTGS-AEAP.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP
(
    jobAttributes *jobAttributesList,
    int jobNumber,
    int present_time,
    int processors_available,
    scheduledResourceNode **processorsAllocatedList,
    scheduledResourceNode **jobScheduledQueueList
)
{
    PROFILER_START(SRTG, AEAP)
    int localProcessors = 0, job_release_time;
    genericBackupNode *processorsDistList = NULL;
    scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;

    localProcessors = processors_available;
    processorsDistList = insert_node(processorsDistList, processors_available);
    processors_available = 0;

    if (GLOBAL_preScheduleList == NULL)
    {
        while (localProcessorsAllocatedList != NULL)
        {
            if ((localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
            {
                int count = 0;
                scheduledResourceNode *temp1 = *processorsAllocatedList;
                genericBackupNode *temp2 = processorsDistList;
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
                processorsDistList = clean_list(processorsDistList);

                jobAttributesList[jobNumber].schedule_hardware = 2;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].completion_time = -1;
                jobAttributesList[jobNumber].scheduled_execution = -1;
                GLOBAL_CPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
                    printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                }
                break;
            }
            else
            {
                localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
                if (localProcessors >= jobAttributesList[jobNumber].processor_req)
                {
                    localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
                    job_release_time = localProcessorsAllocatedList->processor_release_time;
                    processorsDistList = clean_list(processorsDistList);
                    int processorsInUse = jobAttributesList[jobNumber].processor_req;
                    int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                    int presentTime = present_time;
                    int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

                    jobAttributesList[jobNumber].schedule_hardware = 1;
                    jobAttributesList[jobNumber].rescheduled_execution = -1;
                    jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                    jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                    GLOBAL_GPU_JOBS++;
                    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                        printf("AEAP: The Job:%d scheduled\n", jobNumber);
                        printf("AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                    }
                    queue_job_execution(processorsInUse, processor_release_time, presentTime,
                                        schedule_method, jobNumber, processorsAllocatedList);
                    job_queue_handler(processorsInUse, job_release_time, presentTime,
                                      schedule_method, jobNumber, jobScheduledQueueList);

                    break;
                }
                else
                {
                    processorsDistList = insert_node(processorsDistList, localProcessorsAllocatedList->processors_allocated);
                    localProcessorsAllocatedList->processors_allocated = 0;
                }
            }
            localProcessorsAllocatedList = localProcessorsAllocatedList->next;
        }
    }
    else
    {
        while (localProcessorsAllocatedList != NULL)
        {
            if ((localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) > jobAttributesList[jobNumber].deadline)
            {
                int count = 0;
                scheduledResourceNode *temp1 = *processorsAllocatedList;
                genericBackupNode *temp2 = processorsDistList;
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
                processorsDistList = clean_list(processorsDistList);

                jobAttributesList[jobNumber].schedule_hardware = 2;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].completion_time = -1;
                jobAttributesList[jobNumber].scheduled_execution = -1;
                GLOBAL_CPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP: The Job:%d Cannot be scheduled\n", jobNumber);
                    printf("MODE 4 AEAP: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                }
                break;
            }
            else
            {
                localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
                if (localProcessors >= jobAttributesList[jobNumber].processor_req)
                {
                    int availAlapProcessors = localProcessors - jobAttributesList[GLOBAL_preScheduleList->jobNumber].processor_req;
                    int futureRelease = 0;
                    if (GLOBAL_preScheduleList->next != NULL) {
                        genericBackupNode *future_preScheduleList = GLOBAL_preScheduleList->next;
                        futureRelease = future_preScheduleList->data;
                    }
                    // condition 1
                    if (jobAttributesList[jobNumber].processor_req <= availAlapProcessors &&
                            localProcessorsAllocatedList->processor_release_time <= GLOBAL_preScheduleList->data &&
                            (futureRelease == 0 || (localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time) <= futureRelease))
                    {
                        localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
                        processorsDistList = clean_list(processorsDistList);

                        job_release_time = localProcessorsAllocatedList->processor_release_time;
                        int processorsInUse = jobAttributesList[jobNumber].processor_req;
                        int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                        int presentTime = present_time;
                        int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

                        jobAttributesList[jobNumber].schedule_hardware = 1;
                        jobAttributesList[jobNumber].rescheduled_execution = -1;
                        jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                        jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                        GLOBAL_GPU_JOBS++;
                        if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                            printf("AEAP: Condition-1 The Job:%d scheduled\n", jobNumber);
                            printf("AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                        }
                        queue_job_execution(processorsInUse, processor_release_time, presentTime,
                                            schedule_method, jobNumber, processorsAllocatedList);
                        job_queue_handler(processorsInUse, job_release_time, presentTime,
                                          schedule_method, jobNumber, jobScheduledQueueList);
                        break;
                    }
                    // condition 2
                    else if (localProcessorsAllocatedList->processor_release_time + jobAttributesList[jobNumber].execution_time <= GLOBAL_preScheduleList->data)
                    {
                        localProcessorsAllocatedList->processors_allocated = localProcessors - jobAttributesList[jobNumber].processor_req;
                        processorsDistList = clean_list(processorsDistList);

                        job_release_time = localProcessorsAllocatedList->processor_release_time;
                        int processorsInUse = jobAttributesList[jobNumber].processor_req;
                        int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                        int presentTime = present_time;
                        int schedule_method = RTGS_SCHEDULE_METHOD_AEAP;

                        jobAttributesList[jobNumber].schedule_hardware = 1;
                        jobAttributesList[jobNumber].rescheduled_execution = -1;
                        jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                        jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                        GLOBAL_GPU_JOBS++;
                        if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                            printf("AEAP: Condition-2 The Job:%d scheduled\n", jobNumber);
                            printf("AEAP: Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                        }
                        queue_job_execution(processorsInUse, processor_release_time, presentTime,
                                            schedule_method, jobNumber, processorsAllocatedList);
                        job_queue_handler(processorsInUse, job_release_time, presentTime,
                                          schedule_method, jobNumber, jobScheduledQueueList);
                        break;
                    }
                    else
                    {
                        int count = 0;
                        scheduledResourceNode *temp1 = *processorsAllocatedList;
                        genericBackupNode *temp2 = processorsDistList;
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
                        processorsDistList = clean_list(processorsDistList);

                        if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                            printf("AEAP: AEAP with ALAP, Backup processors reloaded\n");
                        }
                        //TBD:: Schedule after AEAP Advanced NEEDED && AEAP_ALAP_IMPROVE_HERE
                        processors_available = AEAP_advanced(jobAttributesList, jobNumber, present_time,
                                                             processors_available, processorsAllocatedList, jobScheduledQueueList);
                        break;
                    }
                }
                else
                {
                    processorsDistList = insert_node(processorsDistList, localProcessorsAllocatedList->processors_allocated);
                    localProcessorsAllocatedList->processors_allocated = 0;
                }
            }
            localProcessorsAllocatedList = localProcessorsAllocatedList->next;
        }
    }
    PROFILER_STOP(SRTG, AEAP)
    return processors_available;
}
