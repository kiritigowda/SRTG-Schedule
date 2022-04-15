/*
* RTGS-entry.c
*      Author: Kiriti Nagesh Gowda
*/

#include "RTGS.h"
#include "RTGS_Global.h"

/**********************************************************************************************************
usage information
***********************************************************************************************************/
static void show_usage()
{
	printf("\n**************************************************************************************************\n");
	printf("\n                  Soft-Real-Time GPU Scheduler -- SRTG-Scheduler - %s\n", RTGS_VERSION);
	printf("\n**************************************************************************************************\n");
	printf("\n");
	printf("Usage:\n\n");
	printf("\tWindows:\n");
	printf("\t\tSRTG-Scheduler.exe [options]\t--j <jobs_file.csv>\n");
	printf("\t\t\t\t\t\t--r <Release_Time_file.csv>\n");
	printf("\t\t\t\t\t\t--m <option>\n");
	printf("\t\t\t\t\t\t--p <option>\n");
	printf("\t\t\t\t\t\t--d <option>\n");
	printf("\t\t\t\t\t\t--simulation <option> \n");
	printf("\t\t\t\t\t\t--gpu <AMD/NVIDIA> \n");
	printf("\t\t\t\t\t\t--method <option> \n");
	printf("\tLinux:\n");
	printf("\t\t./SRTG-Scheduler [options]\t--j <jobs_file.csv>\n");
	printf("\t\t\t\t\t\t--r <Release_Time_file.csv>\n");
	printf("\t\t\t\t\t\t--m <option>\n");
	printf("\t\t\t\t\t\t--p <option>\n");
	printf("\t\t\t\t\t\t--d <option>\n");
	printf("\t\t\t\t\t\t--simulation <option> \n");
	printf("\t\t\t\t\t\t--gpu <AMD/NVIDIA> \n");
	printf("\t\t\t\t\t\t--method <option> \n");
	printf("\n");
	printf("\nScheduler Options Supported\n\n");
	printf("\t--h/--help\t-- Show full help\n");
	printf("\t--v/--verbose\t-- Show detailed messages\n");
	printf("\nScheduler Parameters\n\n");
	printf("\t--j/--jobs \t\t\t -- Jobs to be scheduled [required]\n");
	printf("\t--r/--releaseTimes \t\t -- Release times for the jobs [required]\n");
	printf("\t--m/--mode \t\t\t -- Scheduler Mode [optional - default:5]\n");
	printf("\t--p/--maxProcessors \t\t -- Max GCUs available on the GPU [optional - default:16]\n");
	printf("\t--d/--delayLimitPercentage \t -- Scheduler bias value in percentage [optional - default:60]\n");
	printf("\t--s/--simulation \t\t -- Simulation mode turn ON/OFF (1/0) [optional - default:1]\n");
	printf("\t--g/--gpu \t\t\t -- Jobs Scheduled on Graphics Hardware <AMD/NVIDIA> - [optional - default:OFF]\n");
	printf("\t--method \t\t\t -- Scheduler Methods [optional - default:0]\n");
	printf("\n");
	printf("The Jobs File format - Jobs to be scheduled: <jobs_file.csv>\n");
	printf("\tThe arguments:\n");
	printf("			Jid\t- Job ID\n");
	printf("			GCUs\t- Job GCU Requirement\n");
	printf("			Texe\t- Execution Time\n");
	printf("			Td\t- Deadline\n");
	printf("			Tlts\t- Lastest Time Schedulable on the GPU\n\n");
	printf("			\"Jid, GCUs, Texe, Td, Tlts\"\n\n");
	printf("\n");
	printf("The Release Time File Format - Release times of jobs: <Release_Time_file.csv>\n");
	printf("\tThe arguments:\n");
	printf("			Tr - Job Release Time\n");
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
	printf("\n");
	printf("The Methods Supported: <options>\n");
	printf("\tThe arguments:\n");
	printf("			0 - Base Scheduler Method\n");
	printf("			1 - Enhanced Scheduler Method\n");
	printf("			2 - Enhanced with varying quality of services Scheduler Method\n");
	printf("\n");
}

/**********************************************************************************************************
Program Entry
***********************************************************************************************************/
int main(int argc, char *argv[])
{
	RTGS_Status status = RTGS_SUCCESS;
	bool simulation = true, hardwareSupport = false;
	char *hardwareMode = NULL;
	char *jobsListFileName = NULL, *releaseTimeFilename = NULL;
	int schedulerMode = 0;
	int error = 0;
	int schedulerMethod = -1;

	// global vaiable intitialize
	GLOBAL_RTGS_METHOD = -1;
	GLOBAL_MAX_PROCESSORS = -1;
	GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT = -1;

	// get default debug msg control
	GLOBAL_RTGS_DEBUG_MSG = 1;
	char textBuffer[1024];
	if (RTGS_GetEnvironmentVariable("RTGS_DEBUG_MSG", textBuffer, sizeof(textBuffer)))
	{
		GLOBAL_RTGS_DEBUG_MSG = atoi(textBuffer);
	}

	for (int arg = 1; arg < argc; arg++)
	{
		if (!strcasecmp(argv[arg], "--help") || !strcasecmp(argv[arg], "--H") || !strcasecmp(argv[arg], "--h"))
		{
			show_usage();
			exit(status);
		}
		else if (!strcasecmp(argv[arg], "--simulation") || !strcasecmp(argv[arg], "--S") || !strcasecmp(argv[arg], "--s"))
		{
			if ((arg + 1) == argc)
			{
				printf("\nMissing Simulation Value on command-line. Scheduler will run Simulation by Default\n");
			}
			else
			{
				arg++;
				int value = atoi(argv[arg]);
				if (!value)
				{
					simulation = false;
					printf("\nSimulation Turned Off. Make sure the scheduler is compiled with the right hardware options\n\n");
				}
			}
		}
		else if (!strcasecmp(argv[arg], "--gpu") || !strcasecmp(argv[arg], "--G") || !strcasecmp(argv[arg], "--g"))
		{
			if ((arg + 1) == argc)
			{
				printf("\nMissing Hardware option on command-line. Scheduler will run Simulation by Default\n");
			}
			else
			{
				arg++;
				hardwareMode = argv[arg];
				if (!strcasecmp(hardwareMode, "AMD") || !strcasecmp(hardwareMode, "NVIDIA"))
				{
					printf("\nScheduler Offloading GPU Compatible Jobs to - %s GPU\n", hardwareMode);
					hardwareSupport = true;
				}
				else
				{
					printf("\nERROR: Scheduler GPU Hardware Support only extended to 'AMD' / 'NVIDIA'\n\n");
					status = RTGS_ERROR_NOT_SUPPORTED;
					exit(status);
				}
			}
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
		else if (!strcasecmp(argv[arg], "--releaseTimes") || !strcasecmp(argv[arg], "--R") || !strcasecmp(argv[arg], "--r") || !strcasecmp(argv[arg], "--rt"))
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
				printf("Mode 5: Event Aware Schedule with Bias and Improved Bias Prediction\n");
				schedulerMode = 5;
			}
			else
			{
				arg++;
				schedulerMode = atoi(argv[arg]);
			}
		}
		else if (!strcasecmp(argv[arg], "--method"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nMissing Scheduler Method Value on command-line. Default Method will be Executed\n");
				printf("Method 0 -- Base Scheduler Method\n");
				schedulerMethod = 0;
			}
			else
			{
				arg++;
				schedulerMethod = atoi(argv[arg]);
			}
		}
		else if (!strcasecmp(argv[arg], "--maxProcessors") || !strcasecmp(argv[arg], "--P") || !strcasecmp(argv[arg], "--p"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nMissing Max GCUs Value on command-line. Default Max GCUs will be used\n");
				printf("Default Max GCUs: %d\n", MAX_GPU_PROCESSOR);
				GLOBAL_MAX_PROCESSORS = MAX_GPU_PROCESSOR;
			}
			else
			{
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
				if (GLOBAL_MAX_PROCESSORS != -1)
				{
					maxProcessorsLocal = GLOBAL_MAX_PROCESSORS;
				}
				else
				{
					maxProcessorsLocal = MAX_GPU_PROCESSOR;
				}
				GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT = (int)floor((maxProcessorsLocal * PROCESSOR_LIMIT) / 100);
			}
			else
			{
				arg++;
				int Percentage = atoi(argv[arg]);
				float processorsAvailable = 0;
				if (GLOBAL_MAX_PROCESSORS != -1)
				{
					processorsAvailable = (float)GLOBAL_MAX_PROCESSORS;
				}
				else
				{
					processorsAvailable = MAX_GPU_PROCESSOR;
				}
				float delaylimitProcessors = (float)floor((processorsAvailable * Percentage) / 100);
				GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT = (int)delaylimitProcessors;
			}
		}
	}
	// check if all the files needed was passed
	if (error != 2)
	{
		show_usage();
		status = RTGS_ERROR_NOT_SUFFICIENT;
		printf("\nMissing 'Jobs' / 'Release Times' in command-line. Please check help for details\n");
		exit(status);
	}
	// Check opertion modes - Simulation or Hardware Dependent
	if (hardwareSupport && simulation)
	{
		show_usage();
		status = RTGS_ERROR_NOT_COMPATIBLE;
		printf("\nTurn OFF Simulation to enter GPU offloading mode. Please check help for details\n");
		exit(status);
	}
	else if (hardwareSupport && !simulation)
	{
		status = RTGS_checkGPUReadiness();
		if (status)
		{
			printf("The Scheduler exited with error code: %d\n", status);
			exit(status);
		}
	}
	else if (!hardwareSupport && !simulation)
	{
		status = RTGS_ERROR_NOT_COMPATIBLE;
		printf("GPU Support Options need to be specified. The Scheduler exited with error code: %d\n", status);
		exit(status);
	}
	else
	{
		printf("\nSRTG-Scheduler Simulation\n");
	}

	// Schedule Method  - Verify
	if (schedulerMethod < 0 && schedulerMethod > 2)
	{
		printf("SRTG-Scheduler Methods Supported -- 0/1/2 \n");
		printf("Default Method Set: %d\n", RTGS_METHOD_BASE);
		GLOBAL_RTGS_METHOD = RTGS_METHOD_BASE;
	}
	else{
		GLOBAL_RTGS_METHOD = schedulerMethod;
	}

	if (GLOBAL_MAX_PROCESSORS == -1)
	{
		GLOBAL_MAX_PROCESSORS = MAX_GPU_PROCESSOR;
	}
	if (GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT == -1)
	{
		GLOBAL_DELAY_SCHEDULE_PROCESSOR_LIMIT = (int)floor(GLOBAL_MAX_PROCESSORS * 0.75);
	}

	PROFILER_FILE_INITIALIZE(schedulerMode, jobsListFileName);
	PROFILER_INITIALIZE();
	PROFILER_START(SRTG, RTG_Schedule)

	int64_t start_t = RTGS_GetClockCounter();
	status = scheduler_main(jobsListFileName, releaseTimeFilename, schedulerMode); // scheduler call
	int64_t end_t = RTGS_GetClockCounter();

	PROFILER_STOP(SRTG, RTG_Schedule)
	PROFILER_SHUTDOWN();

	if (status != RTGS_SUCCESS)
	{
		printf("The Scheduler Failed with error code: %d\n", status);
	}
	else
	{
		int64_t freq = RTGS_GetClockFrequency();
		float factor = 1000.0f / (float)freq; // to convert clock counter to ms
		float Scheduler_time = (float)((end_t - start_t) * factor);
		printf("SRTG-Scheduler Sucessful\n");
		printf("Total Time Taken by SRTG Scheduler  - %0.2f ms\n", Scheduler_time);
	}

	return status;
}