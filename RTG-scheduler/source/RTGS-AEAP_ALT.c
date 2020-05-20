/*
* RTGS-AEAP_ALT.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP_advanced
(
    jobAttributes *jobAttributesList,
    int jobNumber,
    int present_time,
    int processors_available,
    scheduledResourceNode **processorsAllocatedList,
    scheduledResourceNode **jobScheduledQueueList
)
{
    PROFILER_START(SRTG, AEAP_advanced)

    if (GLOBAL_preScheduleList == NULL) {
        printf("AEAP advanced: ERROR - Bad Entry Case\n");
        return RTGS_ERROR_NOT_SUPPORTED;
    }
    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
        printf("AEAP advanced: Job->%d is verified for AEAP advanced scheduling\n", jobNumber);
    }

    int localProcessors = 0, job_release_time = 0, startTimeALAP = 0;
    genericBackupNode *processorsDistList = NULL;
    scheduledResourceNode *localProcessorsAllocatedList = *processorsAllocatedList;
    int jobExecutionTime = jobAttributesList[jobNumber].execution_time;
    int jobProcessorReq = jobAttributesList[jobNumber].processor_req;

    // condition 5 - schedule before ALAP execution
    if (localProcessorsAllocatedList != NULL)
    {
        startTimeALAP = jobAttributesList[GLOBAL_preScheduleList->jobNumber].scheduled_execution;
        localProcessors = processors_available;
        processorsDistList = insert_node(processorsDistList, processors_available);
        processors_available = 0;

        while (localProcessorsAllocatedList != NULL)
        {
            if ((localProcessorsAllocatedList->processor_release_time + jobExecutionTime) > jobAttributesList[jobNumber].deadline ||
                    (localProcessorsAllocatedList->processor_release_time + jobExecutionTime) > startTimeALAP)
            {
                int count = 0;
                scheduledResourceNode *temp1 = *processorsAllocatedList;
                genericBackupNode *temp2 = processorsDistList;
                while (temp2 != NULL) {
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
                break;
            }
            else
            {
                localProcessors = localProcessors + localProcessorsAllocatedList->processors_allocated;
                if (localProcessors >= jobProcessorReq)
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
                        printf("AEAP Advanced: Condition-5 The Job:%d scheduled\n", jobNumber);
                        printf("AEAP Advanced: Condition-5 Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                    }
                    queue_job_execution(processorsInUse, processor_release_time, presentTime, schedule_method, jobNumber, processorsAllocatedList);
                    job_queue_handler(processorsInUse, job_release_time, presentTime, schedule_method, jobNumber, jobScheduledQueueList);
                    return processors_available;
                }
                else {
                    processorsDistList = insert_node(processorsDistList, localProcessorsAllocatedList->processors_allocated);
                    localProcessorsAllocatedList->processors_allocated = 0;
                }
                localProcessorsAllocatedList = localProcessorsAllocatedList->next;
            }
        }
    }

    // condition 6 - schedule After ALAP execution
    localProcessorsAllocatedList = *processorsAllocatedList;
    if (GLOBAL_preScheduleList->next == NULL)
    {
        localProcessors = GLOBAL_preScheduleList->processors_allocated;
        // condition 6 a - processors available after the preScheduled job release
        if (localProcessors >= jobProcessorReq)
        {
            if (jobAttributesList[jobNumber].execution_time + GLOBAL_preScheduleList->processor_release_time <= jobAttributesList[jobNumber].deadline)
            {
                GLOBAL_preScheduleList->processors_allocated = localProcessors - jobProcessorReq;
                job_release_time = GLOBAL_preScheduleList->processor_release_time;
                processorsDistList = clean_list(processorsDistList);

                int processorsInUse = jobAttributesList[jobNumber].processor_req;
                int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                jobAttributesList[jobNumber].schedule_hardware = 1;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                GLOBAL_GPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP Advanced: Condition-6 a The Job:%d scheduled\n", jobNumber);
                    printf("AEAP Advanced: Condition-6 a Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                }
                GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
            }
            else
            {
                jobAttributesList[jobNumber].schedule_hardware = 2;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].completion_time = -1;
                jobAttributesList[jobNumber].scheduled_execution = -1;
                GLOBAL_CPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP Advanced: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
                    printf("AEAP Advanced: Condition-5&6 Fail, Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                }
            }
            return processors_available;
        }
        else
        {
            // processors scheduled on other jobs scheduled AEAP
            if (localProcessorsAllocatedList != NULL)
            {
                int latestProcessorRelTime = GLOBAL_preScheduleList->processor_release_time;
                localProcessorsAllocatedList = *processorsAllocatedList;
                while (localProcessorsAllocatedList != NULL) {
                    if (latestProcessorRelTime < localProcessorsAllocatedList->processor_release_time) {
                        latestProcessorRelTime = localProcessorsAllocatedList->processor_release_time;
                    }
                    localProcessorsAllocatedList = localProcessorsAllocatedList->next;
                }

                // condition 6 b - job scheduled after all scheduled jobs are completed
                localProcessors = GLOBAL_MAX_PROCESSORS;
                if (jobAttributesList[jobNumber].execution_time + latestProcessorRelTime <= jobAttributesList[jobNumber].deadline &&
                        jobAttributesList[jobNumber].processor_req <= localProcessors)
                {
                    GLOBAL_preScheduleList->processors_allocated = 0;
                    job_release_time = latestProcessorRelTime;
                    int processorsInUse = jobAttributesList[jobNumber].processor_req;
                    int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                    int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                    jobAttributesList[jobNumber].schedule_hardware = 1;
                    jobAttributesList[jobNumber].rescheduled_execution = -1;
                    jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                    jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                    GLOBAL_GPU_JOBS++;
                    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                        printf("AEAP Advanced: Condition-6 b The Job:%d scheduled\n", jobNumber);
                        printf("AEAP Advanced: Condition-6 b Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                    }
                    GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                    job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
                }
                else
                {
                    jobAttributesList[jobNumber].schedule_hardware = 2;
                    jobAttributesList[jobNumber].rescheduled_execution = -1;
                    jobAttributesList[jobNumber].completion_time = -1;
                    jobAttributesList[jobNumber].scheduled_execution = -1;
                    GLOBAL_CPU_JOBS++;
                    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                        printf("AEAP Advanced: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
                        printf("AEAP Advanced: Condition-5&6 Fail, Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                    }
                }
                return processors_available;
            }
            // no other jobs scheduled
            else
            {
                int latestProcessorRelTime = GLOBAL_preScheduleList->processor_release_time;
                // condition 6 c - job scheduled after all Pre Scheduled jobs are completed
                localProcessors = GLOBAL_MAX_PROCESSORS;
                if (jobAttributesList[jobNumber].execution_time + latestProcessorRelTime <= jobAttributesList[jobNumber].deadline &&
                        jobAttributesList[jobNumber].processor_req <= localProcessors)
                {
                    GLOBAL_preScheduleList->processors_allocated = 0;
                    job_release_time = latestProcessorRelTime;
                    int processorsInUse = jobAttributesList[jobNumber].processor_req;
                    int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                    int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                    jobAttributesList[jobNumber].schedule_hardware = 1;
                    jobAttributesList[jobNumber].rescheduled_execution = -1;
                    jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                    jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                    GLOBAL_GPU_JOBS++;
                    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                        printf("AEAP Advanced: Condition-6 c The Job:%d scheduled\n", jobNumber);
                        printf("AEAP Advanced: Condition-6 c Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                    }
                    GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                    job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
                }
                else
                {
                    jobAttributesList[jobNumber].schedule_hardware = 2;
                    jobAttributesList[jobNumber].rescheduled_execution = -1;
                    jobAttributesList[jobNumber].completion_time = -1;
                    jobAttributesList[jobNumber].scheduled_execution = -1;
                    GLOBAL_CPU_JOBS++;
                    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                        printf("AEAP Advanced: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
                        printf("AEAP Advanced: Condition-5&6 Fail, Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                    }
                }
                return processors_available;
            }
        }
    }
    // condition 7 - Schedule with multiple pre scheduled jobs
    else
    {
        // condition 7 a & b - no AEAP jobs scheduled
        if (localProcessorsAllocatedList == NULL)
        {
            genericBackupNode *localPreScheduledList = GLOBAL_preScheduleList;
            int latestProcessorRelTime = 0, maxProcessorsReleased = 0, nextJobReleased = 0, nextJobProcessorsRequired = 0;
            while (localPreScheduledList->next != NULL)
            {
                genericBackupNode *nextScheduledJob = localPreScheduledList->next;
                nextJobReleased = nextScheduledJob->data;
                nextJobProcessorsRequired = nextScheduledJob->processors_requested;
                if (latestProcessorRelTime < localPreScheduledList->processor_release_time)
                {
                    latestProcessorRelTime = localPreScheduledList->processor_release_time;
                }
                maxProcessorsReleased = localPreScheduledList->processors_allocated;

                if (jobAttributesList[jobNumber].execution_time + latestProcessorRelTime > jobAttributesList[jobNumber].deadline)
                {
                    jobAttributesList[jobNumber].schedule_hardware = 2;
                    jobAttributesList[jobNumber].rescheduled_execution = -1;
                    jobAttributesList[jobNumber].completion_time = -1;
                    jobAttributesList[jobNumber].scheduled_execution = -1;
                    GLOBAL_CPU_JOBS++;
                    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                        printf("AEAP Advanced: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
                        printf("AEAP Advanced: Condition-5&6 Fail, Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                    }
                    return processors_available;
                }
                else
                {
                    // condition 7 a - job scheduled in between pre scheduled jobs
                    if (jobAttributesList[jobNumber].processor_req <= maxProcessorsReleased &&
                            jobAttributesList[jobNumber].execution_time + latestProcessorRelTime <= nextJobReleased)
                    {
                        localPreScheduledList->processors_allocated = maxProcessorsReleased - jobAttributesList[jobNumber].processor_req;
                        job_release_time = latestProcessorRelTime;
                        int processorsInUse = jobAttributesList[jobNumber].processor_req;
                        int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                        int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                        jobAttributesList[jobNumber].schedule_hardware = 1;
                        jobAttributesList[jobNumber].rescheduled_execution = -1;
                        jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                        jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                        GLOBAL_GPU_JOBS++;
                        if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                            printf("AEAP Advanced: Condition-7 a The Job:%d scheduled\n", jobNumber);
                            printf("AEAP Advanced: Condition-7 a Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                        }
                        GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                        job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
                        return processors_available;
                    }
                    // condition 7 b - job scheduled in between pre scheduled jobs
                    if (jobAttributesList[jobNumber].execution_time + latestProcessorRelTime <= nextJobReleased &&
                            jobAttributesList[jobNumber].processor_req <= nextJobProcessorsRequired)
                    {
                        localPreScheduledList->processors_allocated = 0;
                        job_release_time = latestProcessorRelTime;
                        int processorsInUse = jobAttributesList[jobNumber].processor_req;
                        int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                        int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                        jobAttributesList[jobNumber].schedule_hardware = 1;
                        jobAttributesList[jobNumber].rescheduled_execution = -1;
                        jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                        jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                        GLOBAL_GPU_JOBS++;
                        if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                            printf("AEAP Advanced: Condition-7 b The Job:%d scheduled\n", jobNumber);
                            printf("AEAP Advanced: Condition-7 b Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                        }
                        GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                        job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
                        // processors allocated set to zero
                        localPreScheduledList = GLOBAL_preScheduleList;
                        while (localPreScheduledList != NULL) {
                            if (localPreScheduledList->jobNumber == jobNumber) {
                                localPreScheduledList->processors_allocated = 0;
                                break;
                            }
                            localPreScheduledList = localPreScheduledList->next;
                        }
                        return processors_available;
                    }
                }
                localPreScheduledList = localPreScheduledList->next;
            }

            int processorsQueued = 0;
            int processorsQueuedRelease = 0;
            localPreScheduledList = GLOBAL_preScheduleList;
            while (localPreScheduledList != NULL) {
                processorsQueued += localPreScheduledList->processors_allocated;
                if (processorsQueuedRelease < localPreScheduledList->processor_release_time)
                {
                    processorsQueuedRelease = localPreScheduledList->processor_release_time;
                }
                if (processorsQueued >= jobAttributesList[jobNumber].processor_req)
                {
                    break;
                }
                localPreScheduledList = localPreScheduledList->next;
            }
            localPreScheduledList = GLOBAL_preScheduleList;

            // condition 7 c - job scheduled after all pre scheduled jobs are completed
            localProcessors = GLOBAL_MAX_PROCESSORS;
            if (jobAttributesList[jobNumber].execution_time + processorsQueuedRelease <= jobAttributesList[jobNumber].deadline &&
                    jobAttributesList[jobNumber].processor_req <= localProcessors)
            {
                processorsQueued = 0;
                while (localPreScheduledList != NULL) {
                    processorsQueued += localPreScheduledList->processors_allocated;
                    if (processorsQueued >= jobAttributesList[jobNumber].processor_req) {
                        localPreScheduledList->processors_allocated = processorsQueued - jobAttributesList[jobNumber].processor_req;
                        break;
                    }
                    else {
                        localPreScheduledList->processors_allocated = 0;
                    }
                    localPreScheduledList = localPreScheduledList->next;
                }
                localPreScheduledList = GLOBAL_preScheduleList;

                job_release_time = processorsQueuedRelease;
                int processorsInUse = jobAttributesList[jobNumber].processor_req;
                int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                jobAttributesList[jobNumber].schedule_hardware = 1;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                GLOBAL_GPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP Advanced: Condition-7 c The Job:%d scheduled\n", jobNumber);
                    printf("AEAP Advanced: Condition-7 c Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                }
                GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
            }
            else
            {
                jobAttributesList[jobNumber].schedule_hardware = 2;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].completion_time = -1;
                jobAttributesList[jobNumber].scheduled_execution = -1;
                GLOBAL_CPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP Advanced: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
                    printf("AEAP Advanced: Condition-5,6&7 Fail, Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                }
            }
            return processors_available;
        }
        else
        {
            int processorsRetrived = 0;
            int processorsReleaseTime = 0;
            while (localProcessorsAllocatedList != NULL) {
                processorsRetrived += localProcessorsAllocatedList->processors_allocated;
                if (processorsReleaseTime < localProcessorsAllocatedList->processor_release_time) {
                    processorsReleaseTime = localProcessorsAllocatedList->processor_release_time;
                }
                if (processorsRetrived >= jobAttributesList[jobNumber].processor_req)
                {
                    break;
                }
                localProcessorsAllocatedList = localProcessorsAllocatedList->next;
            }
            localProcessorsAllocatedList = *processorsAllocatedList;

            // fail case return
            if (jobAttributesList[jobNumber].execution_time + processorsReleaseTime > jobAttributesList[jobNumber].deadline)
            {
                jobAttributesList[jobNumber].schedule_hardware = 2;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].completion_time = -1;
                jobAttributesList[jobNumber].scheduled_execution = -1;
                GLOBAL_CPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP Advanced: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
                    printf("AEAP Advanced: Condition-5,6,7&8 Fail, Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                }
                return processors_available;
            }

            genericBackupNode *localPreScheduledList = GLOBAL_preScheduleList;
            int latestProcessorRelTime = 0, maxProcessorsReleased = 0, nextJobReleased = 0, nextJobProcessorsRequired = 0;
            while (localPreScheduledList->next != NULL)
            {
                genericBackupNode *nextScheduledJob = localPreScheduledList->next;
                nextJobReleased = nextScheduledJob->data;
                nextJobProcessorsRequired = nextScheduledJob->processors_requested;
                if (latestProcessorRelTime < localPreScheduledList->processor_release_time)
                {
                    latestProcessorRelTime = localPreScheduledList->processor_release_time;
                }
                maxProcessorsReleased = localPreScheduledList->processors_allocated;

                if (jobAttributesList[jobNumber].execution_time + latestProcessorRelTime > jobAttributesList[jobNumber].deadline)
                {
                    jobAttributesList[jobNumber].schedule_hardware = 2;
                    jobAttributesList[jobNumber].rescheduled_execution = -1;
                    jobAttributesList[jobNumber].completion_time = -1;
                    jobAttributesList[jobNumber].scheduled_execution = -1;
                    GLOBAL_CPU_JOBS++;
                    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                        printf("AEAP Advanced: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
                        printf("AEAP Advanced: Condition-5,6,7,& 8 Fail, Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                    }
                    return processors_available;
                }
                else
                {
                    // condition 8 a - job scheduled in between pre scheduled jobs
                    if (jobAttributesList[jobNumber].processor_req <= maxProcessorsReleased &&
                            jobAttributesList[jobNumber].execution_time + latestProcessorRelTime <= nextJobReleased)
                    {
                        localPreScheduledList->processors_allocated = maxProcessorsReleased - jobAttributesList[jobNumber].processor_req;
                        job_release_time = latestProcessorRelTime;
                        int processorsInUse = jobAttributesList[jobNumber].processor_req;
                        int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                        int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                        jobAttributesList[jobNumber].schedule_hardware = 1;
                        jobAttributesList[jobNumber].rescheduled_execution = -1;
                        jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                        jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                        GLOBAL_GPU_JOBS++;
                        if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                            printf("AEAP Advanced: Condition-8 a The Job:%d scheduled\n", jobNumber);
                            printf("AEAP Advanced: Condition-8 a Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                        }
                        GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                        job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
                        return processors_available;
                    }
                    // condition 8 b - job scheduled in between pre scheduled jobs
                    if (jobAttributesList[jobNumber].execution_time + latestProcessorRelTime <= nextJobReleased &&
                            jobAttributesList[jobNumber].processor_req <= nextJobProcessorsRequired &&
                            processorsReleaseTime <= latestProcessorRelTime)
                    {
                        localPreScheduledList->processors_allocated = 0;
                        job_release_time = latestProcessorRelTime;
                        int processorsInUse = jobAttributesList[jobNumber].processor_req;
                        int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                        int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                        jobAttributesList[jobNumber].schedule_hardware = 1;
                        jobAttributesList[jobNumber].rescheduled_execution = -1;
                        jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                        jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                        GLOBAL_GPU_JOBS++;
                        if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                            printf("AEAP Advanced: Condition-8 b The Job:%d scheduled\n", jobNumber);
                            printf("AEAP Advanced: Condition-8 b Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                        }
                        GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                        job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
                        // processors allocated set to zero
                        localPreScheduledList = GLOBAL_preScheduleList;
                        while (localPreScheduledList != NULL) {
                            if (localPreScheduledList->jobNumber == jobNumber) {
                                localPreScheduledList->processors_allocated = 0;
                                break;
                            }
                            localPreScheduledList = localPreScheduledList->next;
                        }
                        return processors_available;
                    }
                }
                localPreScheduledList = localPreScheduledList->next;
            }

            int processorsQueued = 0;
            int processorsQueuedRelease = processorsReleaseTime;
            localPreScheduledList = GLOBAL_preScheduleList;
            while (localPreScheduledList != NULL) {
                processorsQueued += localPreScheduledList->processors_allocated;
                if (processorsQueuedRelease < localPreScheduledList->processor_release_time)
                {
                    processorsQueuedRelease = localPreScheduledList->processor_release_time;
                }
                if (processorsQueued >= jobAttributesList[jobNumber].processor_req)
                {
                    break;
                }
                localPreScheduledList = localPreScheduledList->next;
            }
            localPreScheduledList = GLOBAL_preScheduleList;

            // condition 8 c - job scheduled after all pre scheduled jobs are completed
            localProcessors = GLOBAL_MAX_PROCESSORS;
            if (jobAttributesList[jobNumber].execution_time + processorsQueuedRelease <= jobAttributesList[jobNumber].deadline &&
                    jobAttributesList[jobNumber].processor_req <= localProcessors)
            {
                processorsQueued = 0;
                while (localPreScheduledList != NULL) {
                    processorsQueued += localPreScheduledList->processors_allocated;
                    if (processorsQueued >= jobAttributesList[jobNumber].processor_req) {
                        localPreScheduledList->processors_allocated = processorsQueued - jobAttributesList[jobNumber].processor_req;
                        break;
                    }
                    else {
                        localPreScheduledList->processors_allocated = 0;
                    }
                    localPreScheduledList = localPreScheduledList->next;
                }
                localPreScheduledList = GLOBAL_preScheduleList;

                job_release_time = processorsQueuedRelease;
                int processorsInUse = jobAttributesList[jobNumber].processor_req;
                int processor_release_time = job_release_time + jobAttributesList[jobNumber].execution_time;
                int schedule_method = RTGS_SCHEDULE_METHOD_AEAP_ADVANCED;

                jobAttributesList[jobNumber].schedule_hardware = 1;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].scheduled_execution = job_release_time;
                jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + job_release_time;
                GLOBAL_GPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP Advanced: Condition-8 c The Job:%d scheduled\n", jobNumber);
                    printf("AEAP Advanced: Condition-8 c Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
                }
                GLOBAL_preScheduleList = insert_preScheduledJob_list(GLOBAL_preScheduleList, job_release_time, processor_release_time, processorsInUse, jobNumber);
                job_queue_handler(processorsInUse, job_release_time, processor_release_time, schedule_method, jobNumber, jobScheduledQueueList);
            }
            else
            {
                jobAttributesList[jobNumber].schedule_hardware = 2;
                jobAttributesList[jobNumber].rescheduled_execution = -1;
                jobAttributesList[jobNumber].completion_time = -1;
                jobAttributesList[jobNumber].scheduled_execution = -1;
                GLOBAL_CPU_JOBS++;
                if (GLOBAL_RTGS_DEBUG_MSG > 1) {
                    printf("AEAP Advanced: Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
                    printf("AEAP Advanced: Condition-5,6,7 & 8 Fail, Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
                }
            }
            return processors_available;
        }
    }
    // exit case
    jobAttributesList[jobNumber].schedule_hardware = 2;
    jobAttributesList[jobNumber].rescheduled_execution = -1;
    jobAttributesList[jobNumber].completion_time = -1;
    jobAttributesList[jobNumber].scheduled_execution = -1;
    GLOBAL_CPU_JOBS++;
    if (GLOBAL_RTGS_DEBUG_MSG > 1) {
        printf("AEAP advanced: The Job:%d Cannot be scheduled\n", jobNumber);
        printf("AEAP advanced: Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
    }

    PROFILER_STOP(SRTG, AEAP_advanced)
    return processors_available;
}
