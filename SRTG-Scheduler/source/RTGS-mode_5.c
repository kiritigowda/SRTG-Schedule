/*
 * RTG-mode_5.c
 *      Author: Kiriti Nagesh Gowda
 */
#include "RTGS.h"

/***********************************************************************************************************
MODE 5 Enhanced Method FUNCTIONS
**********************************************************************************************************/
static int mode_5_enhanced_method(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int processorsAvailable,
	int presentTime,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList)
{
	if (GLOBAL_RTGS_DEBUG_MSG > 2)
	{
		printf("Mode-5 Enhanced Method\n");
	}

	if (GLOBAL_RTGS_METHOD >= RTGS_METHOD_ENHANCED && jobAttributesList[jobNumber].processor_req_m > 0)
	{
		// schedule with medium processor option
		GLOBAL_CPU_JOBS--;
		jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_m;
		jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_m;
		jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
		processorsAvailable = Kernel_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
												 processorsAllocatedList, jobScheduledQueueList);
		if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
		{
			jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED;
			jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_MEDIUM;
		}
		else if (jobAttributesList[jobNumber].processor_req_l > 0)
		{
			// schedule with low processor option
			GLOBAL_CPU_JOBS--;
			jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_l;
			jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_l;
			jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
			processorsAvailable = Kernel_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
													 processorsAllocatedList, jobScheduledQueueList);
			if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
			{
				jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED;
				jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_LOW;
			}
		}
	}
	if (GLOBAL_RTGS_METHOD == RTGS_METHOD_ENHANCED_VQS && jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_CPU)
	{
		if (jobAttributesList[jobNumber].deadline_flexibility != 0)
		{
			float deadlineFlexibility = (float)(jobAttributesList[jobNumber].deadline_flexibility);
			deadlineFlexibility = deadlineFlexibility / 100; // percentage value
			int deadlineExtended = ceil((float)((jobAttributesList[jobNumber].deadline - presentTime) * deadlineFlexibility));
			jobAttributesList[jobNumber].deadline = jobAttributesList[jobNumber].deadline + deadlineExtended;
			// schedule with high processor option with deadline flexibilty
			GLOBAL_CPU_JOBS--;
			jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_h;
			jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_h;
			jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
			processorsAvailable = Kernel_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
													 processorsAllocatedList, jobScheduledQueueList);
			if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
			{
				jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED_VQS;
				jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_HIGH;
			}
			else if (jobAttributesList[jobNumber].processor_req_m > 0)
			{
				// schedule with medium processor option with deadline flexibilty
				GLOBAL_CPU_JOBS--;
				jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_m;
				jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_m;
				jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
				processorsAvailable = Kernel_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
														 processorsAllocatedList, jobScheduledQueueList);
				if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
				{
					jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED_VQS;
					jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_MEDIUM;
				}
				else if (jobAttributesList[jobNumber].processor_req_l > 0)
				{
					// schedule with low processor option with deadline flexibilty
					GLOBAL_CPU_JOBS--;
					jobAttributesList[jobNumber].processor_req = jobAttributesList[jobNumber].processor_req_l;
					jobAttributesList[jobNumber].execution_time = jobAttributesList[jobNumber].execution_time_l;
					jobAttributesList[jobNumber].latest_schedulable_time = jobAttributesList[jobNumber].deadline - jobAttributesList[jobNumber].execution_time;
					processorsAvailable = Kernel_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
															 processorsAllocatedList, jobScheduledQueueList);
					if (jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_GPU)
					{
						jobAttributesList[jobNumber].rtgs_method = RTGS_METHOD_ENHANCED_VQS;
						jobAttributesList[jobNumber].rtgs_level = RTGS_LEVEL_LOW;
					}
				}
			}
		}
		else
		{
			if (GLOBAL_RTGS_DEBUG_MSG > 1)
			{
				printf("Jod:%d -- Schedule Method:5 Can not be used as deadline is not flexible\n", jobNumber);
			}
		}
	}
	return processorsAvailable;
}

/**********************************************************************************************************
RTGS Mode 5 - - AEAP/ALAP BP with APLAP improver mode->AEAP/ALAP BP Improve
***********************************************************************************************************/
int RTGS_mode_5(char *jobsListFileName, char *releaseTimeFilename)
{
	PROFILER_START(SRTG, RTGS_mode_5)
	jobAttributes jobAttributesList[MAX_JOBS] = {{0}};
	jobReleaseInfo releaseTimeInfo[MAX_JOBS] = {{0}};
	scheduledResourceNode *processorsAllocatedList = NULL;
	scheduledResourceNode *jobScheduledQueueList = NULL; // Job queued for future executions

	// global variable initialize
	GLOBAL_GPU_JOBS = 0;
	GLOBAL_CPU_JOBS = 0;
	GLOBAL_preScheduleList = NULL;

	int processorsAvailable = GLOBAL_MAX_PROCESSORS;
	int jobNumber = 0;

	int kernelMax = get_job_information(jobAttributesList, jobsListFileName);
	if (kernelMax <= RTGS_FAILURE)
	{
		return RTGS_FAILURE;
	}
	int maxReleases = get_job_release_times(releaseTimeInfo, releaseTimeFilename);
	if (maxReleases <= RTGS_FAILURE)
	{
		return RTGS_FAILURE;
	}

	if (GLOBAL_RTGS_DEBUG_MSG > 1)
	{
		printf("\n**************** The GPU Scheduler will Schedule %d Jobs ****************\n", kernelMax);
	}

	int numReleases = 0;
	for (int presentTime = 0; presentTime < MAX_RUN_TIME; presentTime++)
	{
		// Freeing-up processors
		processorsAvailable = Retrieve_processors(presentTime, processorsAvailable, &processorsAllocatedList);
		if (processorsAvailable < 0)
		{
			printf("Retrieve_processors ERROR- GCUs Available:%d\n", processorsAvailable);
			return RTGS_ERROR_NOT_IMPLEMENTED;
		}
		processorsAvailable = Dispatch_queued_kernels(presentTime, processorsAvailable, &jobScheduledQueueList, &processorsAllocatedList);
		if (processorsAvailable < 0)
		{
			printf("Dispatch_queued_kernels ERROR - GCUs Available:%d\n", processorsAvailable);
			return RTGS_ERROR_NOT_IMPLEMENTED;
		}

		if (releaseTimeInfo[numReleases].release_time == presentTime)
		{

			if (releaseTimeInfo[numReleases].num_job_released == 1)
			{
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("\nRTGS Mode 5 -- Total GCUs Available at time %d = %d\n", presentTime, processorsAvailable);
					printf("RTGS Mode 5 -- Job-%d Released\n", jobNumber);
				}
				jobAttributesList[jobNumber].release_time = presentTime;
				// handling the released jobAttributesList by the book-keeper
				int64_t start_t = RTGS_GetClockCounter();
				processorsAvailable = Kernel_book_keeper(jobAttributesList, jobNumber, processorsAvailable, presentTime,
														 &processorsAllocatedList, &jobScheduledQueueList);
				int64_t end_t = RTGS_GetClockCounter();
				int64_t freq = RTGS_GetClockFrequency();
				float factor = 1000.0f / (float)freq; // to convert clock counter to ms
				float SchedulerOverhead = (float)((end_t - start_t) * factor);
				jobAttributesList[jobNumber].schedule_overhead = SchedulerOverhead;
				// Enhanced Scheduler Methods
				if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[jobNumber].schedule_hardware == RTGS_HARDWARE_CPU)
				{
					int64_t start_t_e = RTGS_GetClockCounter();
					processorsAvailable = mode_5_enhanced_method(jobAttributesList, jobNumber, processorsAvailable, presentTime,
																 &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t_e = RTGS_GetClockCounter();
					float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
					jobAttributesList[jobNumber].schedule_overhead = jobAttributesList[jobNumber].schedule_overhead + SchedulerOverhead_e;
				}
				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", jobNumber, jobAttributesList[jobNumber].rtgs_method, jobAttributesList[jobNumber].rtgs_level);
				}
				jobNumber++;
			}
			else if (releaseTimeInfo[numReleases].num_job_released == 2)
			{
				int k1 = jobNumber;
				jobNumber++;
				int k2 = jobNumber;
				jobNumber++;
				jobAttributesList[k1].release_time = presentTime;
				jobAttributesList[k2].release_time = presentTime;

				if (GLOBAL_RTGS_DEBUG_MSG > 1)
				{
					printf("\nRTGS Mode 5 -- Total GCUs Available at time %d = %d\n", presentTime, processorsAvailable);
					printf("RTGS Mode 5 -- Job-%d Released\n", k1);
					printf("RTGS Mode 5 -- Job-%d Released\n", k2);
				}
				if (jobAttributesList[k1].deadline <= jobAttributesList[k2].deadline)
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Kernel_book_keeper(jobAttributesList, k1, processorsAvailable, presentTime, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
					// Enhanced Scheduler Methods
					if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[k1].schedule_hardware == RTGS_HARDWARE_CPU)
					{
						int64_t start_t_e = RTGS_GetClockCounter();
						processorsAvailable = mode_5_enhanced_method(jobAttributesList, k1, processorsAvailable, presentTime,
																	 &processorsAllocatedList, &jobScheduledQueueList);
						int64_t end_t_e = RTGS_GetClockCounter();
						float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
						jobAttributesList[k1].schedule_overhead = jobAttributesList[k1].schedule_overhead + SchedulerOverhead_e;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", k1, jobAttributesList[k1].rtgs_method, jobAttributesList[k1].rtgs_level);
					}
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Kernel_book_keeper(jobAttributesList, k2, processorsAvailable, presentTime, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
					// Enhanced Scheduler Methods
					if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[k2].schedule_hardware == RTGS_HARDWARE_CPU)
					{
						int64_t start_t_e = RTGS_GetClockCounter();
						processorsAvailable = mode_5_enhanced_method(jobAttributesList, k2, processorsAvailable, presentTime,
																	 &processorsAllocatedList, &jobScheduledQueueList);
						int64_t end_t_e = RTGS_GetClockCounter();
						float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
						jobAttributesList[k2].schedule_overhead = jobAttributesList[k2].schedule_overhead + SchedulerOverhead_e;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", k2, jobAttributesList[k2].rtgs_method, jobAttributesList[k2].rtgs_level);
					}
				}
				else
				{
					// handling the released jobAttributesList by the book-keeper
					int64_t start_t = RTGS_GetClockCounter();
					processorsAvailable = Kernel_book_keeper(jobAttributesList, k2, processorsAvailable, presentTime, &processorsAllocatedList, &jobScheduledQueueList);
					int64_t end_t = RTGS_GetClockCounter();
					int64_t freq = RTGS_GetClockFrequency();
					float factor = 1000.0f / (float)freq; // to convert clock counter to ms
					float SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k2].schedule_overhead = SchedulerOverhead;
					// Enhanced Scheduler Methods
					if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[k2].schedule_hardware == RTGS_HARDWARE_CPU)
					{
						int64_t start_t_e = RTGS_GetClockCounter();
						processorsAvailable = mode_5_enhanced_method(jobAttributesList, k2, processorsAvailable, presentTime,
																	 &processorsAllocatedList, &jobScheduledQueueList);
						int64_t end_t_e = RTGS_GetClockCounter();
						float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
						jobAttributesList[k2].schedule_overhead = jobAttributesList[k2].schedule_overhead + SchedulerOverhead_e;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", k2, jobAttributesList[k2].rtgs_method, jobAttributesList[k2].rtgs_level);
					}
					start_t = RTGS_GetClockCounter();
					processorsAvailable = Kernel_book_keeper(jobAttributesList, k1, processorsAvailable, presentTime, &processorsAllocatedList, &jobScheduledQueueList);
					end_t = RTGS_GetClockCounter();
					SchedulerOverhead = (float)((end_t - start_t) * factor);
					jobAttributesList[k1].schedule_overhead = SchedulerOverhead;
					// Enhanced Scheduler Methods
					if (GLOBAL_RTGS_METHOD > RTGS_METHOD_BASE && jobAttributesList[k1].schedule_hardware == RTGS_HARDWARE_CPU)
					{
						int64_t start_t_e = RTGS_GetClockCounter();
						processorsAvailable = mode_5_enhanced_method(jobAttributesList, k1, processorsAvailable, presentTime,
																	 &processorsAllocatedList, &jobScheduledQueueList);
						int64_t end_t_e = RTGS_GetClockCounter();
						float SchedulerOverhead_e = (float)((end_t_e - start_t_e) * factor);
						jobAttributesList[k1].schedule_overhead = jobAttributesList[k1].schedule_overhead + SchedulerOverhead_e;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1)
					{
						printf("Jod:%d -- Scheduled Method:%d Scheduled Level:%d\n", k1, jobAttributesList[k1].rtgs_method, jobAttributesList[k1].rtgs_level);
					}
				}
			}
			else if (releaseTimeInfo[numReleases].num_job_released > 2)
			{
				return RTGS_ERROR_NOT_IMPLEMENTED;
			}

			numReleases++;
			if (numReleases > maxReleases)
			{
				printf("RTGS Mode 5 ERROR --  Job Release Time exceded Max Releases\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}
	}

	if (maxReleases != 0)
	{

		if (GLOBAL_RTGS_DEBUG_MSG)
		{
			printf("******* Scheduler Results *******\n");
			printf("RTGS Method:%d RTGS Mode:5\n", GLOBAL_RTGS_METHOD);
			printf("GCUs Available -- %d\n", processorsAvailable);
			printf("Total Jobs Scheduled -- %d\n", kernelMax);
			printf("	GPU Scheduled Jobs    -- %d\n", GLOBAL_GPU_JOBS);
			printf("	Jobs Sent Back To CPU -- %d\n", GLOBAL_CPU_JOBS);
		}

		if (RTGS_PrintScheduleSummary(5, kernelMax, jobAttributesList))
		{
			printf("\nSummary Failed\n");
		}

		if ((kernelMax != (GLOBAL_GPU_JOBS + GLOBAL_CPU_JOBS)) || processorsAvailable != GLOBAL_MAX_PROCESSORS)
		{
			return RTGS_FAILURE;
		}

		for (int j = 0; j <= kernelMax; j++)
		{
			jobAttributesList[j].processor_req = jobAttributesList[j].deadline = jobAttributesList[j].execution_time = jobAttributesList[j].latest_schedulable_time = 0;
		}
		kernelMax = 0;
		maxReleases = 0;
		jobNumber = 0;
		GLOBAL_GPU_JOBS = 0;
		GLOBAL_CPU_JOBS = 0;
	}

	if (processorsAllocatedList || GLOBAL_preScheduleList)
	{
		printf("\nERROR -- processorsAllocatedList/GLOBAL_preScheduleList Failed %d/%d\n", processorsAllocatedList == NULL ? 0 : 1, GLOBAL_preScheduleList == NULL ? 0 : 1);
		return RTGS_FAILURE;
	}
	PROFILER_STOP(SRTG, RTGS_mode_5)
	return RTGS_SUCCESS;
}
