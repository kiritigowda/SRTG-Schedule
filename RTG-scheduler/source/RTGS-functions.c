/*
* RTGS-functions.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

/* Book keeping Function, the core function of the scheduler in charge of assigning processors and allocating the future releases */
int Kernel_book_keeper
(
	jobAttributes* jobAttributesList,
	int jobNumber,
	int processors_available,
	int present_time,
	scheduledResourceNode **processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	PROFILER_START(SRTG, Kernel_book_keeper)
		int processorReleased = 0, processor_release_time = 0;
	int schedule_method = RTGS_SCHEDULE_METHOD_NOT_DEFINED;
	static int FLAG, FLAG_V, given;

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Job Book Keeper -- Job-%d --> processor required:%d, execution time:%d, deadline:%d, latest schedulable time:%d\n",
			jobNumber, jobAttributesList[jobNumber].processor_req, jobAttributesList[jobNumber].execution_time,
			jobAttributesList[jobNumber].deadline, jobAttributesList[jobNumber].latest_schedulable_time);
	}
	// If processors available is greater than the required processors by the jobAttributesList
	if (jobAttributesList[jobNumber].processor_req <= processors_available)
	{
		if (GLOBAL_preScheduleList == NULL)
		{ //ALAP not set
			FLAG = 0;
			FLAG_V = 0;
			given = 0;

			if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
			{ // Processors needed lesser than the limit

				if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
				{
					processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
					processorReleased = jobAttributesList[jobNumber].processor_req;
					processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
					schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;

					jobAttributesList[jobNumber].schedule_hardware = 1;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].scheduled_execution = present_time;
					jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
					GLOBAL_GPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Job Book Keeper -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
					}
					// Job call for the GPU to handle the given Jobs and number of blocks
					queue_job_execution(processorReleased, processor_release_time, present_time,
						schedule_method, jobNumber, processorsAllocatedList);

				}
				else
				{
					jobAttributesList[jobNumber].schedule_hardware = 2;
					jobAttributesList[jobNumber].rescheduled_execution = -1;
					jobAttributesList[jobNumber].completion_time = -1;
					jobAttributesList[jobNumber].scheduled_execution = -1;
					GLOBAL_CPU_JOBS++;
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Job Book Keeper -- Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
						printf("Job Book Keeper -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
					}
				}
			}
			else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT)
			{ // Processors needed greater or equal than the limit
				if (GLOBAL_RTGS_DEBUG_MSG > 1) {
					printf("Job Book Keeper -- Job-%d is compute intensive, sent for ALAP execution\n", jobNumber);
				}
				processors_available = ALAP(jobAttributesList, jobNumber, present_time,
					processors_available, processorsAllocatedList, jobScheduledQueueList);
			}
		}
		else if (GLOBAL_preScheduleList != NULL)
		{ //ALAP set
			if (FLAG_V != GLOBAL_preScheduleList->data)
			{ //ALAP updated
				FLAG = 0;
				FLAG_V = 0;
				given = 0;
			}
			int Pl = MAX_GPU_PROCESSOR - GLOBAL_preScheduleList->processors_allocated;

			if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
			{ // Processors needed lesser than the limit
				if (jobAttributesList[jobNumber].processor_req <= Pl && (present_time + jobAttributesList[jobNumber].execution_time) <= GLOBAL_preScheduleList->data)
				{ // Condition 1
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Job Book Keeper -- ALAP is set, Job-%d SATISFIED CONDITION 1", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline) {

						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorReleased = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;
						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = present_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Job Book Keeper -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						// Job call for the GPU to handle the given Jobs and number of blocks//
						queue_job_execution(processorReleased, processor_release_time, present_time,
							schedule_method, jobNumber, processorsAllocatedList);
					}
					else
					{
						jobAttributesList[jobNumber].schedule_hardware = 2;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].completion_time = -1;
						jobAttributesList[jobNumber].scheduled_execution = -1;
						GLOBAL_CPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Job Book Keeper -- Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Job Book Keeper -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				else if (jobAttributesList[jobNumber].processor_req > Pl && (present_time + jobAttributesList[jobNumber].execution_time) <= GLOBAL_preScheduleList->data)
				{ // Condition 2
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Job Book Keeper -- ALAP is set, Job-%d SATISFIED CONDITION 2", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline) {

						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorReleased = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = present_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Job Book Keeper -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						// Job call for the GPU to handle the given Jobs and number of blocks//
						queue_job_execution(processorReleased, processor_release_time, present_time,
							schedule_method, jobNumber, processorsAllocatedList);
					}
					else
					{
						jobAttributesList[jobNumber].schedule_hardware = 2;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].completion_time = -1;
						jobAttributesList[jobNumber].scheduled_execution = -1;
						GLOBAL_CPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Job Book Keeper -- Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Job Book Keeper -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				else if ((jobAttributesList[jobNumber].processor_req + given) <= Pl && (present_time + jobAttributesList[jobNumber].execution_time) > GLOBAL_preScheduleList->data &&
					FLAG == 0)
				{ // Condition 3
					given = given + jobAttributesList[jobNumber].processor_req;
					// Control flags to not allow over budgeting of PA
					if (given == Pl) {
						FLAG = 1;
						FLAG_V = GLOBAL_preScheduleList->data;
						given = 0;
					}
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Job Book Keeper -- ALAP is set, Job-%d SATISFIED CONDITION 1 with FLAG", jobNumber);
					}
					if (jobAttributesList[jobNumber].execution_time + present_time <= jobAttributesList[jobNumber].deadline)
					{

						processors_available = processors_available - jobAttributesList[jobNumber].processor_req;
						processorReleased = jobAttributesList[jobNumber].processor_req;
						processor_release_time = jobAttributesList[jobNumber].execution_time + present_time;
						schedule_method = RTGS_SCHEDULE_METHOD_IMMEDIATE;

						jobAttributesList[jobNumber].schedule_hardware = 1;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].scheduled_execution = present_time;
						jobAttributesList[jobNumber].completion_time = jobAttributesList[jobNumber].execution_time + present_time;
						GLOBAL_GPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Job Book Keeper -- Jobs ACCEPTED count --> %d\n", GLOBAL_GPU_JOBS);
						}
						// Job call for the GPU to handle the given Jobs and number of blocks//
						queue_job_execution(processorReleased, processor_release_time, present_time,
							schedule_method, jobNumber, processorsAllocatedList);
					}
					else
					{
						jobAttributesList[jobNumber].schedule_hardware = 2;
						jobAttributesList[jobNumber].rescheduled_execution = -1;
						jobAttributesList[jobNumber].completion_time = -1;
						jobAttributesList[jobNumber].scheduled_execution = -1;
						GLOBAL_CPU_JOBS++;
						if (GLOBAL_RTGS_DEBUG_MSG > 1) {
							printf("Job Book Keeper -- Job-%d will not complete before it's deadline, Job REJECTED\n", jobNumber);
							printf("Job Book Keeper -- Jobs REJECTED count --> %d\n", GLOBAL_CPU_JOBS);
						}
					}
				}
				else if (processors_available >= jobAttributesList[GLOBAL_preScheduleList->jobNumber].processor_req)
				{
					/// NEW FUNCTION NEEDED
					if (GLOBAL_RTGS_DEBUG_MSG > 1) {
						printf("Job Book Keeper -- RELEASE ALAP Job NOW TO INCREASE SYSTEM TIME\n\n");
						printf("\nPA : %d   ALAP_Pg : %d\n", processors_available, jobAttributesList[GLOBAL_preScheduleList->jobNumber].processor_req);
					}
					processors_available = ALAP_improve(jobAttributesList, GLOBAL_preScheduleList->jobNumber, present_time,
						processors_available, processorsAllocatedList, jobScheduledQueueList);
					processors_available = AEAP(jobAttributesList, jobNumber, present_time,
						processors_available, processorsAllocatedList, jobScheduledQueueList);
				}
				else
				{
					processors_available = AEAP_advanced(jobAttributesList, jobNumber, present_time,
						processors_available, processorsAllocatedList, jobScheduledQueueList);
				}
			}//Processors lesses than the limit
			else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT)
			{ // Processors needed greater or equal than the limit
				processors_available = ALAP_advanced(jobAttributesList, jobNumber, present_time,
					processors_available, processorsAllocatedList, jobScheduledQueueList);
			}
		}//ALAP != NULL end if
	}//End processors_available available
	else if (jobAttributesList[jobNumber].processor_req > processors_available)
	{// If processors available is lesser than the required processors by the jobAttributesList
		// Schedule the jobAttributesList to be released in a future time
		processors_available = Processors_unavailable(jobAttributesList, jobNumber, present_time,
			processors_available, processorsAllocatedList, jobScheduledQueueList);
	}
	PROFILER_STOP(SRTG, Kernel_book_keeper)
		return processors_available;
}

int Processors_unavailable
(
	jobAttributes *jobAttributesList,
	int jobNumber,
	int present_time,
	int processors_available,
	scheduledResourceNode ** processorsAllocatedList,
	scheduledResourceNode **jobScheduledQueueList
)
{
	PROFILER_START(SRTG, Processors_unavailable)
		if (jobAttributesList[jobNumber].processor_req < PROCESSOR_LIMIT)
		{
			if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("Processors unavailable:: Job:%d sent for AEAP execution\n", jobNumber);
			}
			processors_available = AEAP(jobAttributesList, jobNumber, present_time,
				processors_available, processorsAllocatedList, jobScheduledQueueList);
		}
		else if (jobAttributesList[jobNumber].processor_req >= PROCESSOR_LIMIT && GLOBAL_preScheduleList == NULL)
		{
			if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("Processors unavailable:: Job:%d sent for ALAP execution\n", jobNumber);
			}
			processors_available = ALAP(jobAttributesList, jobNumber, present_time,
				processors_available, processorsAllocatedList, jobScheduledQueueList);
		}
		else
		{
			if (GLOBAL_RTGS_DEBUG_MSG > 2) {
				printf("Processors unavailable:: Job:%d sent for ALAP Advanced execution\n", jobNumber);
			}
			processors_available = ALAP_advanced(jobAttributesList, jobNumber, present_time,
				processors_available, processorsAllocatedList, jobScheduledQueueList);
		}
	PROFILER_STOP(SRTG, Processors_unavailable)
		return processors_available;
}
