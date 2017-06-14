/*
* RTGS-entry.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"
// version
#define RTGS_VERSION "0.9.2"

/**********************************************************************************************************
Help Function and usage information
***********************************************************************************************************/
void show_usage()
{
	printf("\n*********************************************************************************************************\n");
	printf("\n				Real Time GPU Scheduler -> Scheduler.EXE-%s\n", RTGS_VERSION);
	printf("\n*********************************************************************************************************\n");
	printf("\n");
	printf("Usage:\n\n");
	printf("Windows:\n");
	printf("SCHEDULER.EXE [options] -K <Kernel_file.txt> -RT <Release_Time_file.txt>  -M <Option>\n");
	printf("Linux:\n");
	printf("./SCHEDULER [options] -K <Kernel_file.txt> -RT <Release_Time_file.txt>  -M <Option>\n");
	printf("\n");
	printf("\n\nScheduler [options] Supported\n\n");
	printf("  -h/-help\n");
	printf("   Show full help.\n");
	printf("\n");
	printf("The Kernel File is the list of Kernels to be scheduled.\n");
	printf("The arguments:\n");
	printf("			Kernel Number\n");
	printf("			Processors Needed\n");
	printf("			Execution Time\n");
	printf("			Deadline\n");
	printf("			Lastest Time Schedulable on the GPU\n");
	printf("\n");
	printf("The Release Time File has the list of release times of the kernels:\n");
	printf("The arguments:\n");
	printf("			0 - No Kernel Released\n");
	printf("			1 - One Kernel Released at the time marked by location\n");
	printf("			2 - Two Kernel Released at the time marked by location\n");
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
	char *Kernel = NULL, *ReleaseTime = NULL;
	int Mode = 0, error = 0;

	for (int arg = 1; arg < argc; arg++)
	{
		if (!_stricmp(argv[arg], "-h") || !_stricmp(argv[arg], "-help"))
		{
			show_usage();
			exit(status);
		}
		else if (!_stricmp(argv[arg], "Kernels") || !_stricmp(argv[arg], "-K"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nERROR: missing kernel file name on command-line (see help for details)\n\n\n");
				show_usage();
				status = RTGS_ERROR_NOT_SUFFICIENT;
				exit(status);
			}
			arg++;
			Kernel = (argv[arg]);
			error++;
		}
		else if (!_stricmp(argv[arg], "ReleaseTime") || !_stricmp(argv[arg], "-RT"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nERROR: missing Release Time file name on command-line (see help for details)\n\n\n");
				show_usage();
				status = RTGS_ERROR_NOT_SUFFICIENT;
				exit(status);

			}
			arg++;
			ReleaseTime = (argv[arg]);
			error++;
		}
		else if (!_stricmp(argv[arg], "Mode") || !_stricmp(argv[arg], "-M"))
		{
			if ((arg + 1) == argc)
			{
				printf("\n\nMissing Mode Value on command-line. Default Mode will be Executed\n");
				printf("Mode 5::AEAP ALAP BP with APLAP improver mode->AEAP/ALAP BP Improve\n");
				Mode = 5;
			}
			else {
				arg++;
				Mode = atoi(argv[arg]);
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

	int64_t start_t = RTGS_GetClockCounter();
	status = scheduler_main(Kernel, ReleaseTime, Mode); // calling the scheduler
	int64_t end_t = RTGS_GetClockCounter();

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