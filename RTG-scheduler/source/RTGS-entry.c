/*
* RTGS-entry.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"
#include"RTGS_Global.h"

// version
#define RTGS_VERSION "0.9.7"

/**********************************************************************************************************
usage information
***********************************************************************************************************/
static void show_usage()
{
	printf("\n*********************************************************************************************************\n");
	printf("\n				Real Time GPU Scheduler -> RTG-scheduler-%s\n", RTGS_VERSION);
	printf("\n*********************************************************************************************************\n");
	printf("\n");
	printf("Usage:\n\n");
	printf("Windows:\n");
	printf("RTG-scheduler.exe [options] --jobs <jobs_file.txt> --releaseTimes <Release_Time_file.txt> --mode <Option>\n");
	printf("Linux:\n");
	printf("./RTG-scheduler [options] --jobs <jobs_file.txt> --releaseTimes <Release_Time_file.txt> --mode <Option>\n");
	printf("\n");
	printf("\n\nScheduler [options] Supported\n\n");
	printf("  --h/--help\n");
	printf("   Show full help.\n");
	printf("\n");
	printf("The Jobs file is the list of jobs to be scheduled.\n");
	printf("The arguments:\n");
	printf("			Job Number\n");
	printf("			Processors Needed\n");
	printf("			Execution Time\n");
	printf("			Deadline\n");
	printf("			Lastest Time Schedulable on the GPU\n");
	printf("\n");
	printf("The Release Time File has the list of release times of the Jobs:\n");
	printf("The arguments:\n");
	printf("			0 - No Jobs Released\n");
	printf("			1 - One Job Released at the time marked by location\n");
	printf("			2 - Two Jobs Released at the time marked by location\n");
	printf("			N - Extended in the next release\n");
	printf("\n");
	printf("The Modes Supported:\n");
	printf("The arguments:\n");
	printf("			1 - Simple GPU Scheduler\n");
	printf("			2 - As Early As Possible mode->AEAP\n");
	printf("			3 - AEAP with As Late As Possible mode->AEAP/ALAP\n");
	printf("			4 - AEAP/ALAP Bin Packer mode->AEAP/ALAP Pack\n");
	printf("			5 - AEAP/ALAP BP with APLAP improver mode->AEAP/ALAP BP Improve\n");
	printf("			N - Extended in the next release\n");
	printf("\n");

}

/**********************************************************************************************************
Program Entry
***********************************************************************************************************/
int main(int argc, char * argv[])
{
	RTGS_Status status = RTGS_SUCCESS;
	char *kernelFilename = NULL, *releaseTimeFilename = NULL;
	int schedulerMode = 0;
	int error = 0;
	
	// global vaiable intitialize 
	GLOBAL_RTGS_MODE = -1;
	GLOBAL_KERNEL_FILE_NAME = NULL;

	for (int arg = 1; arg < argc; arg++)
	{
        if (!strcasecmp(argv[arg], "--h") || !strcasecmp(argv[arg], "--help"))
		{
			show_usage();
			exit(status);
		}
        else if (!strcasecmp(argv[arg], "--jobs") || !strcasecmp(argv[arg], "--J"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nERROR: missing jobs_info_list file name on command-line (see help for details)\n\n\n");
				show_usage();
				status = RTGS_ERROR_NOT_SUFFICIENT;
				exit(status);
			}
			arg++;
			kernelFilename = (argv[arg]);
			error++;
		}
        else if (!strcasecmp(argv[arg], "--releaseTimes") || !strcasecmp(argv[arg], "--RT"))
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
        else if (!strcasecmp(argv[arg], "--mode") || !strcasecmp(argv[arg], "--M"))
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
	GLOBAL_KERNEL_FILE_NAME = kernelFilename;
	PROFILER_FILE_INITIALIZE(schedulerMode, kernelFilename);
	PROFILER_INITIALIZE();
	//PROFILER_START(SRTG, RTG_Schedule)

	int64_t start_t = RTGS_GetClockCounter();
	status = scheduler_main(kernelFilename, releaseTimeFilename, schedulerMode); // scheduler call
	int64_t end_t = RTGS_GetClockCounter();

	//PROFILER_STOP(SRTG, RTG_Schedule)
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
