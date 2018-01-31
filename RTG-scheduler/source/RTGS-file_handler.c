/*
* RTGS-file_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

#define FILE_MAX_KERNELS 30

/* The Function is for reading the GPU Compatilble Kernel Values */
int get_kernel_information(kernelInfo *kernelInfoList, const char *kernelFilename)
{
	PROFILER_START(SRTG, get_kernel_information)
	char string[FILE_MAX_KERNELS];
	char num_processor[10], execution_time[10], deadline[10], latest_schedulable[10], KERNEL[10];
	int kernel_ID, num_kernels = 0;

	FILE * fp = fopen(kernelFilename, "r");		// read mode
	if (fp == NULL) {
		printf("ERROR::get_kernel_information - error while opening the file -- %s\n", kernelFilename);
		return RTGS_FAILURE;
	}

#if DEBUG_INFO
	printf("Kernel Info File -- %s\n", kernelFilename);
#endif

	while (fgets(string, FILE_MAX_KERNELS, fp) != NULL)
	{
		int present_time = 0, j = 0, flag = 0;
		for (int k = 0; k < 10; k++){ KERNEL[k] = '\0'; num_processor[k] = '\0'; execution_time[k] = '\0'; deadline[k] = '\0'; latest_schedulable[k] = '\0'; }

		while (string[present_time] != '\0')
		{
			if (string[present_time] == ' '){ flag = flag + 1; j = 0; }
			if (string[present_time] != ' ' && flag == 0){ KERNEL[j] = string[present_time]; j++; }
			if (string[present_time] != ' ' && flag == 1){ num_processor[j] = string[present_time]; j++; }
			if (string[present_time] != ' ' && flag == 2){ execution_time[j] = string[present_time]; j++; }
			if (string[present_time] != ' ' && flag == 3){ deadline[j] = string[present_time]; j++; }
			if (string[present_time] != ' ' && flag == 4){ latest_schedulable[j] = string[present_time];	j++; }
			present_time++;
		}
		if (string[present_time] == '\0'){
			kernel_ID = atoi(KERNEL);
			if (kernel_ID < 0){
				printf("ERROR::get_kernel_information - KERNEL ID needs to be in the range 0 - N\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
            kernelInfoList[kernel_ID].processor_req = atoi(num_processor);
            kernelInfoList[kernel_ID].execution_time = atoi(execution_time);
            kernelInfoList[kernel_ID].deadline = atoi(deadline);
            kernelInfoList[kernel_ID].latest_schedulable_time = atoi(latest_schedulable);
			num_kernels++;
		}
	}
	fclose(fp);
	PROFILER_STOP(SRTG, get_kernel_information)
	return num_kernels;
}


/* The Function is to read the time frames in which these Kernels are released */
int get_kernel_release_times(const char *releaseTimeFilename)
{
	PROFILER_START(SRTG, get_kernel_release_times)
	char string[FILE_MAX_KERNELS];
	int present_time = 0;

	FILE * fp = fopen(releaseTimeFilename, "r");		// read mode
	if (fp == NULL) {
		printf("ERROR::get_kernel_release_times - error while opening the file -- %s\n", releaseTimeFilename);
		return RTGS_FAILURE;
	}

#if DEBUG_INFO
	printf("Release Times Info File -- %s\n", releaseTimeFilename);
#endif

	while (fgets(string, 2, fp) != NULL)
	{
		if (string[0] == '0'){ GLOBAL_RELEASE_TIME[present_time] = 0; present_time++; }
		else if (string[0] == '1'){ GLOBAL_RELEASE_TIME[present_time] = 1; present_time++; }
		else if (string[0] == '2'){ GLOBAL_RELEASE_TIME[present_time] = 2; present_time++; }
		else if (string[0] == '3' || string[0] == '4' || string[0] == '5'){
			printf("ERROR::get_kernel_release_times - release of MAX 2 Kernels Simultanously supported in this release\n");
			return RTGS_ERROR_INVALID_PARAMETERS;
		}
	}
	fclose(fp);
	int MaxRunTime = present_time;
	PROFILER_STOP(SRTG, get_kernel_release_times)
	return MaxRunTime;
}



