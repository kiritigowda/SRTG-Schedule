/*
* RTGS-file_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

#define LINE_SIZE_MAX 1024

/* The Function is for reading the GPU Compatilble Kernel Values -- jobs & kernels used interchangeably*/
int get_kernel_information(kernelInfo *kernelInfoList, const char *kernelFilename)
{
	PROFILER_START(SRTG, get_kernel_information)

	FILE * fp = fopen(kernelFilename, "r");
	if (fp == NULL) {
		printf("ERROR::get_kernel_information - error while opening the file -- %s\n", kernelFilename);
		return RTGS_FAILURE;
	}
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Jobs Info File -- %s\n", kernelFilename);
	}

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
				ctr++;  //for next job info
				j = 0;  //for next job info, init index to 0
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
int get_kernel_release_times(kernelReleaseInfo *releaseTimeInfo, const char *releaseTimeFilename)
{
	PROFILER_START(SRTG, get_kernel_release_times)

	FILE * fp = fopen(releaseTimeFilename, "r");
	if (fp == NULL) {
		printf("ERROR::get_kernel_release_times - error while opening the file -- %s\n", releaseTimeFilename);
		return RTGS_FAILURE;
	}

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Release Times Info File -- %s\n", releaseTimeFilename);
	}

	char kernelLine[LINE_SIZE_MAX];
	char kernelInfo[2][10];
	int releaseTime = -1, numReleases = 0;

	while (fgets(kernelLine, LINE_SIZE_MAX, fp) != NULL)
	{
		int j = 0, ctr = 0;
		for (int i = 0; i <= (strlen(kernelLine)); i++)
		{
			if (kernelLine[i] == ',' || kernelLine[i] == '\0')
			{
				kernelInfo[ctr][j] = '\0';
				ctr++;  //for next job release info
				j = 0;
			}
			else
			{
				kernelInfo[ctr][j] = kernelLine[i];
				j++;
			}

			if (ctr > 2) {
				printf("ERROR:get_kernel_release_times - Job Release Info File ERROR -- RTGS_ERROR_INVALID_PARAMETERS-- count: %d\n", ctr);
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}

		if (ctr != 2) {
			printf("ERROR::get_kernel_release_times - Job Release Info File ERROR -- RTGS_ERROR_NOT_SUFFICIENT -- count: %d\n", ctr);
			return RTGS_ERROR_NOT_SUFFICIENT;
		}

		releaseTime = atoi(kernelInfo[0]);
		if (releaseTime < 0) {
			printf("ERROR::get_kernel_release_times - KERNEL Release time needs to be in the range 0 - N\n");
			return RTGS_ERROR_INVALID_PARAMETERS;
		}

		releaseTimeInfo[numReleases].release_time = atoi(kernelInfo[0]);
		releaseTimeInfo[numReleases].num_kernel_released = atoi(kernelInfo[1]);

		numReleases++;
	}

	fclose(fp);
	PROFILER_STOP(SRTG, get_kernel_release_times)
	return numReleases;
}