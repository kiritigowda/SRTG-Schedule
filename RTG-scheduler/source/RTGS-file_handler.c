/*
* RTGS-file_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

#define LINE_SIZE_MAX 1024

/* The Function is for reading the GPU Compatilble Kernel Values */
int get_kernel_information(kernelInfo *kernelInfoList, const char *kernelFilename)
{
	PROFILER_START(SRTG, get_kernel_information)

	FILE * fp = fopen(kernelFilename, "r");
	if (fp == NULL) {
		printf("ERROR::get_kernel_information - error while opening the file -- %s\n", kernelFilename);
		return RTGS_FAILURE;
	}
#if DEBUG_INFO
	printf("Kernel Info File -- %s\n", kernelFilename);
#endif

	char kernelLine[LINE_SIZE_MAX];
	char kernelInfo[5][10];
	int kernel_ID = -1, num_kernels = 0;

	while (fgets(kernelLine, LINE_SIZE_MAX, fp) != NULL)
	{
		int j = 0, ctr = 0;
		for (int i = 0; i <= (strlen(kernelLine)); i++)
		{
			if (kernelLine[i] == ',' || kernelLine[i] == '\0')
			{
				kernelInfo[ctr][j] = '\0';
				ctr++;  //for next kernel info
				j = 0;  //for next kernel info, init index to 0
			}
			else
			{
				kernelInfo[ctr][j] = kernelLine[i];
				j++;
			}

			if (ctr > 5) {
				printf("ERROR:get_kernel_information - Job Info File ERROR -- RTGS_ERROR_INVALID_PARAMETERS-- count: %d\n", ctr);
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}

		if (ctr != 5) {
			printf("ERROR::get_kernel_information - Job Info File ERROR -- RTGS_ERROR_NOT_SUFFICIENT -- count: %d\n", ctr);
			return RTGS_ERROR_NOT_SUFFICIENT;
		}

		kernel_ID = atoi(kernelInfo[0]);
		if (kernel_ID < 0) {
			printf("ERROR::get_kernel_information - KERNEL ID needs to be in the range 0 - N\n");
			return RTGS_ERROR_INVALID_PARAMETERS;
		}
		kernelInfoList[kernel_ID].processor_req = atoi(kernelInfo[1]);
		kernelInfoList[kernel_ID].execution_time = atoi(kernelInfo[2]);
		kernelInfoList[kernel_ID].deadline = atoi(kernelInfo[3]);
		kernelInfoList[kernel_ID].latest_schedulable_time = atoi(kernelInfo[4]);
		num_kernels++;
	}

	fclose(fp);
	PROFILER_STOP(SRTG, get_kernel_information)
	return num_kernels;
}


/* The Function is to read the time frames in which these Kernels are released */
int get_kernel_release_times(const char *releaseTimeFilename)
{
	PROFILER_START(SRTG, get_kernel_release_times)
	char string[LINE_SIZE_MAX];
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



