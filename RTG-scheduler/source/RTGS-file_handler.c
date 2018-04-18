/*
* RTGS-file_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

#define LINE_SIZE_MAX 1024

/* The Function is for reading the GPU Compatilble Job Values -- jobs & jobs used interchangeably*/
int get_job_information(jobAttributes *kernelInfoList, const char *jobsListFileName)
{
	PROFILER_START(SRTG, get_job_information)

	FILE * fp = fopen(jobsListFileName, "r");
	if (fp == NULL) {
		printf("ERROR::get_job_information - error while opening the file -- %s\n", jobsListFileName);
		return RTGS_FAILURE;
	}
	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Jobs Info File -- %s\n", jobsListFileName);
	}

	char kernelLine[LINE_SIZE_MAX];
	char jobAttributes[5][10];
	int kernel_ID = -1, num_kernels = 0;

	while (fgets(kernelLine, LINE_SIZE_MAX, fp) != NULL)
	{
		int j = 0, ctr = 0;
		for (int i = 0; i <= (strlen(kernelLine)); i++)
		{
			if (kernelLine[i] == ',' || kernelLine[i] == '\0')
			{
				jobAttributes[ctr][j] = '\0';
				ctr++;  //for next job info
				j = 0;  //for next job info, init index to 0
			}
			else
			{
				jobAttributes[ctr][j] = kernelLine[i];
				j++;
			}

			if (ctr > 5) {
				printf("ERROR:get_job_information - Job Info File ERROR -- RTGS_ERROR_INVALID_PARAMETERS-- count: %d\n", ctr);
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}

		if (ctr != 5) {
			printf("ERROR::get_job_information - Job Info File ERROR -- RTGS_ERROR_NOT_SUFFICIENT -- count: %d\n", ctr);
			return RTGS_ERROR_NOT_SUFFICIENT;
		}

		kernel_ID = atoi(jobAttributes[0]);
		if (kernel_ID < 0) {
			printf("ERROR::get_job_information - KERNEL ID needs to be in the range 0 - N\n");
			return RTGS_ERROR_INVALID_PARAMETERS;
		}
		kernelInfoList[kernel_ID].processor_req = atoi(jobAttributes[1]);
		kernelInfoList[kernel_ID].execution_time = atoi(jobAttributes[2]);
		kernelInfoList[kernel_ID].deadline = atoi(jobAttributes[3]);
		kernelInfoList[kernel_ID].latest_schedulable_time = atoi(jobAttributes[4]);
		num_kernels++;
	}

	fclose(fp);
	PROFILER_STOP(SRTG, get_job_information)
	return num_kernels;
}


/* The Function is to read the time frames in which these Jobs are released */
int get_job_release_times(jobReleaseInfo *releaseTimeInfo, const char *releaseTimeFilename)
{
	PROFILER_START(SRTG, get_job_release_times)

	FILE * fp = fopen(releaseTimeFilename, "r");
	if (fp == NULL) {
		printf("ERROR::get_job_release_times - error while opening the file -- %s\n", releaseTimeFilename);
		return RTGS_FAILURE;
	}

	if (GLOBAL_RTGS_DEBUG_MSG > 1) {
		printf("Release Times Info File -- %s\n", releaseTimeFilename);
	}

	char kernelLine[LINE_SIZE_MAX];
	char jobAttributes[2][10];
	int releaseTime = -1, numReleases = 0;

	while (fgets(kernelLine, LINE_SIZE_MAX, fp) != NULL)
	{
		int j = 0, ctr = 0;
		for (int i = 0; i <= (strlen(kernelLine)); i++)
		{
			if (kernelLine[i] == ',' || kernelLine[i] == '\0')
			{
				jobAttributes[ctr][j] = '\0';
				ctr++;  //for next job release info
				j = 0;
			}
			else
			{
				jobAttributes[ctr][j] = kernelLine[i];
				j++;
			}

			if (ctr > 2) {
				printf("ERROR:get_job_release_times - Job Release Info File ERROR -- RTGS_ERROR_INVALID_PARAMETERS-- count: %d\n", ctr);
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
		}

		if (ctr != 2) {
			printf("ERROR::get_job_release_times - Job Release Info File ERROR -- RTGS_ERROR_NOT_SUFFICIENT -- count: %d\n", ctr);
			return RTGS_ERROR_NOT_SUFFICIENT;
		}

		releaseTime = atoi(jobAttributes[0]);
		if (releaseTime < 0) {
			printf("ERROR::get_job_release_times - KERNEL Release time needs to be in the range 0 - N\n");
			return RTGS_ERROR_INVALID_PARAMETERS;
		}

		releaseTimeInfo[numReleases].release_time = atoi(jobAttributes[0]);
		releaseTimeInfo[numReleases].num_job_released = atoi(jobAttributes[1]);

		numReleases++;
	}

	fclose(fp);
	PROFILER_STOP(SRTG, get_job_release_times)
	return numReleases;
}