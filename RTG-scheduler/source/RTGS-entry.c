/*
* RTGS-entry.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"
#include"RTGS_Global.h"

// version
#define RTGS_VERSION "1.0.0"

/**********************************************************************************************************
usage information
***********************************************************************************************************/
static void show_usage()
{
	printf("\n*************************************************************************************************************************************\n");
	printf("\n                                              Real Time GPU Scheduler -- RTGS - %s\n", RTGS_VERSION);
	printf("\n*************************************************************************************************************************************\n");
	printf("\n");
	printf("Usage:\n\n");
	printf("\tWindows:\n");
	printf("\t\tRTG-scheduler.exe [options] --j <jobs_file.txt> --rt <Release_Time_file.txt> --m <option> --p <option> --d <option>\n\n");
	printf("\tLinux:\n");
	printf("\t\t./RTG-scheduler [options] --j <jobs_file.txt> --rt <Release_Time_file.txt> --m <option> --p <option> --d <option>\n\n");
	printf("\n");
	printf("\nScheduler Options Supported\n\n");
	printf("\t--h/--help\t-- Show full help\n");
	printf("\t--v/--verbose\t-- Show detailed messages\n");
	printf("\nScheduler Parameters\n\n");
	printf("\t--j/--jobs \t\t\t -- Jobs to be scheduled [required]\n");
	printf("\t--rt/--releaseTimes \t\t -- Release times for the jobs [required]\n");
	printf("\t--m/--mode \t\t\t -- Mode options [optional - deafult:5]\n");
	printf("\t--p/--maxProcessors \t\t -- Max processors available on the GPU [optional - default:16]\n");
	printf("\t--d/--delayLimitPercentage \t -- Delay Schedule processor limit in percentage [optional - default:60]\n");
	printf("\n");
	printf("The Jobs File format - Jobs to be scheduled: <jobs_file.txt>\n");
	printf("\tThe arguments:\n");
	printf("			Jid - Job Number\n");
	printf("			Pn - Processors Needed\n");
	printf("			Texe - Execution Time\n");
	printf("			Td - Deadline\n");
	printf("			Tlts - Lastest Time Schedulable on the GPU\n\n");
	printf("			\"Jid, Pn, Texe, Td, Tlts\"\n\n");
	printf("\n");
	printf("The Release Time File Format - Release times of jobs: <Release_Time_file.txt>\n");
	printf("\tThe arguments:\n");
	printf("			Tr - Release Time\n");
	printf("			Jr - Number of jobs released\n\n");
	printf("			\"Tr, Jr\"\n");
	printf("\n");
	printf("The Modes Supported: <options>\n");
	printf("\tThe arguments:\n");
	printf("			1 - Greedy Schedule\n");
	printf("			2 - Event Aware Scheduler\n");
	printf("			3 - Event Aware Scheduler with Bias\n");
	printf("			4 - Event Aware Scheduler with Bias and Bias Prediction\n");
	printf("			5 - Event Aware Scheduler with Bias and Improved Bias Prediction\n");
	printf("			N - Extended in the next release\n");
	printf("\n");

}

/**********************************************************************************************************
Program Entry
***********************************************************************************************************/
int main(int argc, char * argv[])
{
	RTGS_Status status = RTGS_SUCCESS;
	char *jobsListFileName = NULL, *releaseTimeFilename = NULL;
	int schedulerMode = 0;
	int error = 0;

	// global vaiable intitialize 
	GLOBAL_RTGS_MODE = -1;
	GLOBAL_KERNEL_FILE_NAME = NULL;
	GLOBAL_MAX_PROCESSORS = -1;
	GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT = -1;

	// get default debug msg control
	GLOBAL_RTGS_DEBUG_MSG = 1;
	char textBuffer[1024];
	if (RTGS_GetEnvironmentVariable("RTGS_DEBUG_MSG", textBuffer, sizeof(textBuffer))) {
		GLOBAL_RTGS_DEBUG_MSG = atoi(textBuffer);
	}

	for (int arg = 1; arg < argc; arg++)
	{
		if (!strcasecmp(argv[arg], "--help") || !strcasecmp(argv[arg], "--H") || !strcasecmp(argv[arg], "--h"))
		{
			show_usage();
			exit(status);
		}
		else if (!strcasecmp(argv[arg], "--jobs") || !strcasecmp(argv[arg], "--J") || !strcasecmp(argv[arg], "--j"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nERROR: missing jobs_info_list file name on command-line (see help for details)\n\n\n");
				show_usage();
				status = RTGS_ERROR_NOT_SUFFICIENT;
				exit(status);
			}
			arg++;
			jobsListFileName = (argv[arg]);
			error++;
		}
		else if (!strcasecmp(argv[arg], "--releaseTimes") || !strcasecmp(argv[arg], "--RT") || !strcasecmp(argv[arg], "--rt"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nERROR: missing Release Time file name on command-line (see help for details)\n\n\n");
				show_usage();
				status = RTGS_ERROR_NOT_SUFFICIENT;
				exit(status);
			}
			arg++;
			releaseTimeFilename = (argv[arg]);
			error++;
		}
		else if (!strcasecmp(argv[arg], "--mode") || !strcasecmp(argv[arg], "--M") || !strcasecmp(argv[arg], "--m"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nMissing Mode Value on command-line. Default Mode will be Executed\n");
				printf("Mode 5::AEAP ALAP BP with APLAP improver mode->AEAP/ALAP BP Improve\n");
				schedulerMode = 5;
			}
			else {
				arg++;
				schedulerMode = atoi(argv[arg]);
			}
		}
		else if (!strcasecmp(argv[arg], "--maxProcessors") || !strcasecmp(argv[arg], "--P") || !strcasecmp(argv[arg], "--p"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nMissing Max Processors Value on command-line. Default Max Processors will be used\n");
				printf("Default Max Processors: %d\n", MAX_GPU_PROCESSOR);
				GLOBAL_MAX_PROCESSORS = MAX_GPU_PROCESSOR;
			}
			else {
				arg++;
				GLOBAL_MAX_PROCESSORS = atoi(argv[arg]);
			}
		}
		else if (!strcasecmp(argv[arg], "--delayLimitPercentage") || !strcasecmp(argv[arg], "--D") || !strcasecmp(argv[arg], "--d"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nMissing delay limit percentage for processors Value on command-line. Default Delay Limit Processors will be used\n");
				printf("Default Delay Schedule Processor limit percentage: %d %%\n", PROCESSOR_LIMIT);
				int maxProcessorsLocal = 0;
				if (GLOBAL_MAX_PROCESSORS != -1) maxProcessorsLocal = GLOBAL_MAX_PROCESSORS;
				else maxProcessorsLocal = MAX_GPU_PROCESSOR;
				GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT = (int) floor((maxProcessorsLocal * PROCESSOR_LIMIT)/100);
			}
			else {
				arg++;
				int Percentage = atoi(argv[arg]);
				float processorsAvailable = 0;
				if (GLOBAL_MAX_PROCESSORS != -1) { processorsAvailable = (float)GLOBAL_MAX_PROCESSORS; }
				else{ processorsAvailable = MAX_GPU_PROCESSOR; }
				float delaylimitProcessors = (float)floor((processorsAvailable * Percentage)/100);
				GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT = (int)delaylimitProcessors;
			}
		}
	}
	// check if all the files needed was passed
	if (error != 2)
	{
		printf("\nMissing Files in command-line. Please check help for details\n");
		show_usage();
		status = RTGS_ERROR_NOT_SUFFICIENT;
		exit(status);
	}

	// profiler  - output name initialize, profiler initialize and shutdown
	GLOBAL_RTGS_MODE = schedulerMode;
	GLOBAL_KERNEL_FILE_NAME = jobsListFileName;
	if(GLOBAL_MAX_PROCESSORS == -1){ GLOBAL_MAX_PROCESSORS = MAX_GPU_PROCESSOR; }
	if(GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT == -1){ 
		GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT = (int) floor(GLOBAL_MAX_PROCESSORS * 0.75);
	}
	PROFILER_FILE_INITIALIZE(schedulerMode, jobsListFileName);
	PROFILER_INITIALIZE();
	PROFILER_START(SRTG, RTG_Schedule)

	int64_t start_t = RTGS_GetClockCounter();
	status = scheduler_main(jobsListFileName, releaseTimeFilename, schedulerMode); // scheduler call
	int64_t end_t = RTGS_GetClockCounter();

	PROFILER_STOP(SRTG, RTG_Schedule)
	PROFILER_SHUTDOWN();

	if (status != RTGS_SUCCESS) {
		printf("The Scheduler Failed with error code ->%d\n", status);
	}
	else {
		int64_t freq = RTGS_GetClockFrequency();
		float factor = 1000.0f / (float)freq; // to convert clock counter to ms
		float Scheduler_time = (float)((end_t - start_t) * factor);
		printf("RTG-Scheduler Sucessful\n");
		printf("Total Time Taken to Schedule  -> %0.2f ms\n", Scheduler_time);
	}

	return status;
}
