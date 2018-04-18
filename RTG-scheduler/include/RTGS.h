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

#define MAX_GPU_PROCESSOR 14                    // Total streaming multi-processors available on the GPU
#define MAX_JOBS 200							// Max jobs to be scheduled
#define PROCESSOR_LIMIT 10                      // ALAP Processor Limit
#define MAX_RUN_TIME 1000                       // Max run time

#define MULTIPLE_JOBS_SCHEDULED -99				// multiple jobs scheduled at a given time

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
#define PROFILER_DEFINE_EVENT(g,e) ePROFILER_EVENT_ENUM_ ## g ## e,
enum ProfilerEventEnum {
#include "profilerEvents.h"	
	PROFILER_NUM_EVENTS
};
#define PROFILER_START(g,e) _PROFILER_START(ePROFILER_EVENT_ENUM_ ## g ## e);
#define PROFILER_STOP(g,e)  _PROFILER_STOP(ePROFILER_EVENT_ENUM_ ## g ## e);
#else
#define PROFILER_FILE_INITIALIZE(g,e)
#define PROFILER_INITIALIZE()
#define PROFILER_SHUTDOWN()
#define PROFILER_START(g,e)
#define PROFILER_STOP(g,e)
#endif

/*! \brief A formal status type with known fixed size.
* \see RTGS_status_e
*/
typedef int RTGS_Status;

/*! \brief The enumeration of all status codes.
* \see rtgs_status.
*/
enum RTGS_status_e {
	RTGS_ERROR_INVALID_PARAMETERS = -9,		/*!< \brief Indicates that the supplied parameter information does not match the jobAttibutesList. */
	RTGS_ERROR_NO_MEMORY = -8,				/*!< \brief Indicates that an internal or implicit allocation failed. Typically catastrophic. */
	RTGS_ERROR_NO_RESOURCES = -7,			/*!< \brief Indicates that an internal or implicit resource can not be acquired (not memory). */
	RTGS_ERROR_NOT_COMPATIBLE = -6,			/*!< \brief Indicates that the attempt to link two parameters together failed due to type incompatibilty. */
	RTGS_ERROR_NOT_ALLOCATED = -5,			/*!< \brief Indicates to the system that the parameter must be allocated by the system.  */
	RTGS_ERROR_NOT_SUFFICIENT = -4,			/*!< \brief Indicates that the given an insufficient number of required parameters. */
	RTGS_ERROR_NOT_SUPPORTED = -3,			/*!< \brief Indicates that the requested set of parameters produce a configuration that cannot be supported */
	RTGS_ERROR_NOT_IMPLEMENTED = -2,		/*!< \brief Indicates that the requested function is missing. */
	RTGS_FAILURE = -1,						/*!< \brief Indicates a generic error code, used when no other describes the error. */
	RTGS_SUCCESS = 0,						/*!< \brief No error. */
};

/*! \brief The enumeration of all schedule methods.
*/
enum RTGS_schedule_method_e {
	RTGS_SCHEDULE_METHOD_IMMEDIATE = 0,
	RTGS_SCHEDULE_METHOD_AEAP = 1,
	RTGS_SCHEDULE_METHOD_ALAP = 2,
	RTGS_SCHEDULE_METHOD_NOT_DEFINED = 99,
};

/* Job Information Structure */
struct jobInformation {
	int processor_req;              // processors needed
	int execution_time;             // execution time
	int deadline;                   // deadline
	int latest_schedulable_time;    // latest schedule time
	int release_time;               // latest schedule time
	float schedule_overhead;        // scheduler Overhead
	int data_send_time;             // data for the job sent
	int scheduled_execution;        // scheduled for GPU exec
	int completion_time;            // job completion time
	int rescheduled_execution;      // job rescheduled for gpu exe
	int schedule_hardware;          // job scheduled on 0: ERROR, 1: GPU, 2: CPU
};
//! \brief jobInformation structure
typedef struct jobInformation jobAttributes;


/* Job Release Info Structure */
struct jobReleaseTime {
	int release_time;               // job release time
	int num_job_released;           // num jobs released
};
//! \brief jobReleaseInfo structure
typedef struct jobReleaseTime jobReleaseInfo;

/* scheduled Job Info jobBackupList*/
struct jobScheduleList {
	int jobNumber;
	int job_release_time;
	int processor_release_time;
	int processors_allocated;
	int schedule_method;
	int data;
	struct jobScheduleList* next;
	struct jobScheduleList* job_next;
};
//! \brief Job scheduled info
typedef struct jobScheduleList scheduledJobNode;

// Backup jobBackupList to revert the processor actions
struct jobBackupList {
	int jobNumber;
	int data;
	int processor_release_time;
	int processors_allocated;
	struct jobBackupList* next;
};
//! \brief backup job scheduled info
typedef struct jobBackupList jobBackupNode;

// global variables
int GLOBAL_GPU_JOBS;
int GLOBAL_CPU_JOBS;
int GLOBAL_RELEASE_TIME[MAX_RUN_TIME];
jobBackupNode* GLOBAL_ALAP_LIST;

// debug message setup variable
int GLOBAL_RTGS_DEBUG_MSG;  // debug mode information: 0 - TurnOff, 1 - Basic Debug Info, 2 - Detailed Debug Info, 3 - extended Debug Information

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
int Retrieve_processors(int present_time, int processors_available,
	scheduledJobNode** processorsAllocatedList);

//! \brief function to release kernels for exection at the scheduled time
int Dispatch_queued_kernels(int present_time, int processors_available,
	scheduledJobNode** jobSchdeuleQueueList, scheduledJobNode **processorsAllocatedList);

//! \brief Function to add future jobAttibutesList releases and arrange jobAttibutesList execution times in ascending order
void Queue_kernel_execution(int processorReleased, int processorReleaseTime, int presentTime,
	int scheduleMethod, int kernelNumber, scheduledJobNode **processorAllocList);

//! \brief Function to
int Kernel_book_keeper(jobAttributes* jobAttibutesList, int jobNumber, int processors_available, int present_time,
	scheduledJobNode **processorsAllocatedList, scheduledJobNode **jobSchdeuleQueueList);

//! \brief Function to
int Processors_unavailable(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
	scheduledJobNode ** processorsAllocatedList, scheduledJobNode **jobSchdeuleQueueList);

//! \brief Function to
void Kernel_queue_handler(int processorReleased, int job_release_time, int processor_release_time,
	int schedule_method, int jobNumber, scheduledJobNode **jobSchdeuleQueueList);

//! \brief Function to
int AEAP(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
	scheduledJobNode ** processorsAllocatedList, scheduledJobNode **jobSchdeuleQueueList);

//! \brief Function to
int AEAP_ALAP_improve(jobAttributes *jobAttibutesList, int job_release_time, int present_time, int processors_available,
	scheduledJobNode ** processorsAllocatedList, scheduledJobNode **jobSchdeuleQueueList);

//! \brief Function to
int AEAP_advanced(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
	scheduledJobNode ** processorsAllocatedList, scheduledJobNode **jobSchdeuleQueueList);

//! \brief Function to
int ALAP(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
	scheduledJobNode ** processorsAllocatedList, scheduledJobNode **jobSchdeuleQueueList);

//! \brief Function to
int ALAP_advanced(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
	scheduledJobNode ** processorsAllocatedList, scheduledJobNode **jobSchdeuleQueueList);

//! \brief Function to
int ALAP_improve(jobAttributes *jobAttibutesList, int jobNumber, int present_time, int processors_available,
	scheduledJobNode ** processorsAllocatedList, scheduledJobNode **jobSchdeuleQueueList);

/***************************************************************
					helper functions
****************************************************************/
//! \brief clock counter function
int64_t RTGS_GetClockCounter();

//! \brief clock frequency function
int64_t RTGS_GetClockFrequency();

//! \brief Get ENV Variable
bool RTGS_GetEnvironmentVariable(const char * name, char * value, size_t valueSize);

//! \brief print summary
int RTGS_PrintScheduleSummary(int mode, int maxKernels, jobAttributes *jobAttributesList);

// jobBackupList functions
scheduledJobNode* insert(scheduledJobNode* head, scheduledJobNode* data);
scheduledJobNode* ascending_insert(scheduledJobNode* head, int data, int processor_release_time,
	int processorReleased, int jobNumber, int schedule_method);
scheduledJobNode* position_insert(scheduledJobNode* head, scheduledJobNode* data, int position);
scheduledJobNode* position_delete(scheduledJobNode* head, int position);
scheduledJobNode* reverse(scheduledJobNode* head);
scheduledJobNode* remove_recurring_node(scheduledJobNode* head);
scheduledJobNode* clean_node(scheduledJobNode* head);
jobBackupNode* insert_list(jobBackupNode* head, int data);
jobBackupNode* clean_list(jobBackupNode* head);
jobBackupNode* position_delete_list(jobBackupNode* head);
jobBackupNode* insert_ALAP_list(jobBackupNode* head, int job_release_time,
	int processor_release_time, int processors_allocated, int jobNumber);
void print(scheduledJobNode* head);
void Kernel_queue_print(scheduledJobNode* head);
void R_print(scheduledJobNode* head);

#endif /* RTGS_H */