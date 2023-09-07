/*
 * RTGS.h
 *      Author: Kiriti Nagesh Gowda
 */

#ifndef RTGS_H
#define RTGS_H

#if _WIN32
#include <Windows.h>
#define strcasecmp strcmp
#else
#include <x86intrin.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>
#include <strings.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_GPU_PROCESSOR 16 // Total streaming multi-processors available on the GPU -- default:16
#define MAX_JOBS 1200		 // Max jobs to be scheduled
#define PROCESSOR_LIMIT 60	 // ALAP Processor Limit -- default:60%
#define MAX_RUN_TIME 500000	 // Max run time

#define MULTIPLE_JOBS_SCHEDULED -99 // multiple jobs scheduled at a given time

// PROFILER_MODE:
//   0 - no profiling
//   1 - default profiling
#define PROFILER_MODE 0
#if PROFILER_MODE
#ifndef _WIN32
#include <inttypes.h>
#define __stdcall
#define __int64 int64_t
#endif
extern void __stdcall PROFILER_INITIALIZE();
extern void __stdcall PROFILER_SHUTDOWN();
extern void __stdcall PROFILER_FILE_INITIALIZE(int Mode, char *File);
#define PROFILER_DEFINE_EVENT(g, e) ePROFILER_EVENT_ENUM_##g##e,
enum ProfilerEventEnum
{
#include "profilerEvents.h"
	PROFILER_NUM_EVENTS
};
#define PROFILER_START(g, e) _PROFILER_START(ePROFILER_EVENT_ENUM_##g##e);
#define PROFILER_STOP(g, e) _PROFILER_STOP(ePROFILER_EVENT_ENUM_##g##e);
#else
#define PROFILER_FILE_INITIALIZE(g, e)
#define PROFILER_INITIALIZE()
#define PROFILER_SHUTDOWN()
#define PROFILER_START(g, e)
#define PROFILER_STOP(g, e)
#endif

/*! \brief A formal status type with known fixed size.
 * \see RTGS_status_e
 */
typedef int RTGS_Status;

/*! \brief The enumeration of all status codes.
 * \see rtgs_status.
 */
enum RTGS_status_e
{
	RTGS_ERROR_RECOMPILE = -10,			/*!< \brief Indicates that the compiler need to be recompiled with GPU Support Enabled. */
	RTGS_ERROR_INVALID_PARAMETERS = -9, /*!< \brief Indicates that the supplied parameter information does not match the jobAttibutesList. */
	RTGS_ERROR_NO_MEMORY = -8,			/*!< \brief Indicates that an internal or implicit allocation failed. Typically catastrophic. */
	RTGS_ERROR_NO_RESOURCES = -7,		/*!< \brief Indicates that an internal or implicit resource can not be acquired (not memory). */
	RTGS_ERROR_NOT_COMPATIBLE = -6,		/*!< \brief Indicates that the attempt to link two parameters together failed due to type incompatibilty. */
	RTGS_ERROR_NOT_ALLOCATED = -5,		/*!< \brief Indicates to the system that the parameter must be allocated by the system.  */
	RTGS_ERROR_NOT_SUFFICIENT = -4,		/*!< \brief Indicates that the given an insufficient number of required parameters. */
	RTGS_ERROR_NOT_SUPPORTED = -3,		/*!< \brief Indicates that the requested set of parameters produce a configuration that cannot be supported */
	RTGS_ERROR_NOT_IMPLEMENTED = -2,	/*!< \brief Indicates that the requested function is missing. */
	RTGS_FAILURE = -1,					/*!< \brief Indicates a generic error code, used when no other describes the error. */
	RTGS_SUCCESS = 0,					/*!< \brief No error. */
};

/*! \brief The enumeration of all scheduler methods.
 */
enum RTGS_schedule_method_e
{
	RTGS_SCHEDULE_METHOD_IMMEDIATE = 0,
	RTGS_SCHEDULE_METHOD_AEAP = 1,
	RTGS_SCHEDULE_METHOD_ALAP = 2,
	RTGS_SCHEDULE_METHOD_AEAP_ADVANCED = 3,
	RTGS_SCHEDULE_METHOD_ALAP_ADVANCED = 4,
	RTGS_SCHEDULE_METHOD_NOT_DEFINED = 99,
};

/*! \brief The enumeration of all Top Level Methods.
 */
enum RTGS_METHOD_E
{
	RTGS_METHOD_BASE = 0,
	RTGS_METHOD_ENHANCED = 1,
	RTGS_METHOD_ENHANCED_VQS = 2,
};

/*! \brief The enumeration of all Method GCU Level.
 */
enum RTGS_LEVEL_E
{
	RTGS_LEVEL_HIGH = 0,
	RTGS_LEVEL_MEDIUM = 1,
	RTGS_LEVEL_LOW = 2,
};

/*! \brief The enumeration of all Schedulable Hardware.
 */
enum RTGS_HARDWARE_E
{
	RTGS_HARDWARE_UNKNOWN = 0,
	RTGS_HARDWARE_GPU = 1,
	RTGS_HARDWARE_CPU = 2,
};

/* Job Information Structure */
struct jobInformation
{
	int processor_req;			 // processors needed
	int processor_req_h;		 // processors needed high
	int processor_req_m;		 // processors needed medium
	int processor_req_l;		 // processors needed low
	int execution_time;			 // execution time
	int execution_time_h;		 // execution time high
	int execution_time_m;		 // execution time medium
	int execution_time_l;		 // execution time medium
	int deadline;				 // deadline
	int latest_schedulable_time; // latest schedule time
	int release_time;			 // latest schedule time
	int data_send_time;			 // data for the job sent
	int scheduled_execution;	 // scheduled for GPU exec
	int completion_time;		 // job completion time
	int rescheduled_execution;	 // job rescheduled for gpu exe
	int schedule_hardware;		 // job scheduled on -- 0: ERROR, 1: GPU, 2: CPU
	int rtgs_method;			 // rtgs method -- 0: Base, 1: Enhanced, 2: Enhanced with varying quality of services
	int rtgs_level;				 // rtgs GCU level -- 0: high, 1: medium, 2: low
	int deadline_flexibility;	 // deadline flexibilty percentage
	float schedule_overhead;	 // scheduler Overhead
};
//! \brief jobInformation structure
typedef struct jobInformation jobAttributes;

/* Job Release Info Structure */
struct jobReleaseTime
{
	int release_time;	  // job release time
	int num_job_released; // num jobs released
};
//! \brief jobReleaseInfo structure
typedef struct jobReleaseTime jobReleaseInfo;

/* scheduled Job Info jobBackupList*/
struct jobScheduleList
{
	int jobNumber;
	int job_release_time;
	int processor_release_time;
	int processors_allocated;
	int schedule_method;
	int data;
	struct jobScheduleList *next;
	struct jobScheduleList *job_next;
};
//! \brief Job scheduled info
typedef struct jobScheduleList scheduledResourceNode;

// Backup jobBackupList to revert the processor actions
struct jobBackupList
{
	int jobNumber;
	int data;
	int processor_release_time;
	int processors_allocated;
	int processors_requested;
	struct jobBackupList *next;
};
//! \brief backup job scheduled info
typedef struct jobBackupList genericBackupNode;

// global variables
int GLOBAL_RTGS_METHOD;
int GLOBAL_GPU_JOBS;
int GLOBAL_CPU_JOBS;
int GLOBAL_RELEASE_TIME[MAX_RUN_TIME];
int GLOBAL_MAX_PROCESSORS;
int GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT;
genericBackupNode *GLOBAL_preScheduleList;

// debug message setup variable
int GLOBAL_RTGS_DEBUG_MSG; // debug mode information: 0 - TurnOff, 1 - Basic Debug Info, 2 - Detailed Debug Info, 3 - extended Debug Information

//! \brief RTG-scheduler main function
int scheduler_main(char *jobsListFileName, char *releaseTimeFilename, int schedulerMode);

//! \brief RTG-scheduler Mode-1 function
int RTGS_mode_1(char *jobsListFileName, char *releaseTimeFilename);
//! \brief RTG-scheduler Mode-2 function
int RTGS_mode_2(char *jobsListFileName, char *releaseTimeFilename);
//! \brief RTG-scheduler Mode-3 function
int RTGS_mode_3(char *jobsListFileName, char *releaseTimeFilename);
//! \brief RTG-scheduler Mode-4 function
int RTGS_mode_4(char *jobsListFileName, char *releaseTimeFilename);
//! \brief RTG-scheduler Mode-4 function
int RTGS_mode_5(char *jobsListFileName, char *releaseTimeFilename);

//! \brief function to get job info from the input file
int get_job_information(jobAttributes *jobAttributesList, const char *jobsListFileName);

//! \brief function to get job release time info from the input file
int get_job_release_times(jobReleaseInfo *releaseTimeInfo, const char *releaseTimeFilename);

//! \brief function to retrieve processors from kernels which complete thier execution
int Retrieve_processors(int present_time, int processors_available, scheduledResourceNode **processorsAllocatedList);

//! \brief function to release kernels for exection at the scheduled time
int Dispatch_queued_kernels(int present_time, int processors_available,
							scheduledResourceNode **jobScheduledQueueList, scheduledResourceNode **processorsAllocatedList);

//! \brief Function to add future jobAttibutesList releases and arrange jobAttibutesList execution times in ascending order
void queue_job_execution(int processorReleased, int processorReleaseTime, int presentTime,
						 int scheduleMethod, int kernelNumber, scheduledResourceNode **processorAllocList);

//! \brief Function to
int Kernel_book_keeper(jobAttributes *jobAttibutesList, int jobNumber, int processors_available, int present_time,
					   scheduledResourceNode **processorsAllocatedList, scheduledResourceNode **jobScheduledQueueList);

//! \brief Function to
int Processors_unavailable(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
						   scheduledResourceNode **processorsAllocatedList, scheduledResourceNode **jobScheduledQueueList);

//! \brief Function to
void job_queue_handler(int processorReleased, int job_release_time, int processor_release_time,
					   int schedule_method, int jobNumber, scheduledResourceNode **jobScheduledQueueList);

//! \brief Function to
int AEAP(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
		 scheduledResourceNode **processorsAllocatedList, scheduledResourceNode **jobScheduledQueueList);

//! \brief Function to
int AEAP_ALAP_improve(jobAttributes *jobAttibutesList, int job_release_time, int present_time, int processors_available,
					  scheduledResourceNode **processorsAllocatedList, scheduledResourceNode **jobScheduledQueueList);

//! \brief Function to
int AEAP_advanced(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
				  scheduledResourceNode **processorsAllocatedList, scheduledResourceNode **jobScheduledQueueList);

//! \brief Function to
int ALAP(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
		 scheduledResourceNode **processorsAllocatedList, scheduledResourceNode **jobScheduledQueueList);

//! \brief Function to
int ALAP_advanced(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
				  scheduledResourceNode **processorsAllocatedList, scheduledResourceNode **jobScheduledQueueList);

//! \brief Function to
int ALAP_improve(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
				 scheduledResourceNode **processorsAllocatedList, scheduledResourceNode **jobScheduledQueueList);

/***************************************************************
					helper functions
****************************************************************/
//! \brief clock counter function
int64_t RTGS_GetClockCounter();

//! \brief clock frequency function
int64_t RTGS_GetClockFrequency();

//! \brief Get ENV Variable
bool RTGS_GetEnvironmentVariable(const char *name, char *value, size_t valueSize);

//! \brief print_processorsAllocated_list summary
int RTGS_PrintScheduleSummary(int mode, int maxKernels, jobAttributes *jobAttributesList);

//! \brief Check GPU Offload readiness
int RTGS_checkGPUReadiness();

// jobBackupList functions
scheduledResourceNode *insert(scheduledResourceNode *head, scheduledResourceNode *data);
scheduledResourceNode *ascending_insert(scheduledResourceNode *head, int data, int processor_release_time,
										int processorReleased, int jobNumber, int schedule_method);
scheduledResourceNode *position_insert(scheduledResourceNode *head, scheduledResourceNode *data, int position);
scheduledResourceNode *position_delete(scheduledResourceNode *head, int position);
scheduledResourceNode *remove_recurring_node(scheduledResourceNode *head);
genericBackupNode *insert_node(genericBackupNode *head, int data);
genericBackupNode *clean_list(genericBackupNode *head);
genericBackupNode *insert_preScheduledJob_list(genericBackupNode *head, int job_release_time,
											   int processor_release_time, int processors_allocated, int jobNumber);
genericBackupNode *position_insert_preScheduledJobs(genericBackupNode *head, genericBackupNode *positionInsertVariable, int position);
genericBackupNode *insert_preScheduledJobs(genericBackupNode *head, genericBackupNode *insertVariable);
genericBackupNode *ascending_insert_preScheduledJobs(genericBackupNode *head, int job_release_time, int processor_release_time,
													 int processors_allocated, int jobNumber);
genericBackupNode *position_delete_preScheduledJob(genericBackupNode *head, int position);
void print_processorsAllocated_list(scheduledResourceNode *head);
void print_preScheduledJob_list(scheduledResourceNode *head);
void print_preQueuedJob_list(genericBackupNode *head);

#endif /* RTGS_H */