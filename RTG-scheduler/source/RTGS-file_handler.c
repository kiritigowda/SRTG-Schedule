/*
* RTGS-file_handler.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

#define FILE_MAX_KERNELS 30

/* The Function is for reading the GPU Compatilble Kernel Values */
int get_kernel_information(Kernel_INFO* kernel, const char *File)
{
	char string[FILE_MAX_KERNELS];
	char num_processor[10], execution_time[10], deadline[10], latest_schedulable[10], KERNEL[10];
	int kernel_ID, num_kernels = 0;

	FILE * fp;
	fp = fopen(File, "r");		// read mode
	if (fp == NULL) {
		printf("ERROR::get_kernel_information - error while opening the file.\n");
		return RTGS_FAILURE;
	}

	while (fgets(string, FILE_MAX_KERNELS, fp) != NULL)
	{
		int i = 0, j = 0, flag = 0;
		for (int k = 0; k < 10; k++){ KERNEL[k] = '\0'; num_processor[k] = '\0'; execution_time[k] = '\0'; deadline[k] = '\0'; latest_schedulable[k] = '\0'; }

		while (string[i] != '\0')
		{
			if (string[i] == ' '){ flag = flag + 1; j = 0; }
			if (string[i] != ' ' && flag == 0){ KERNEL[j] = string[i]; j++; }
			if (string[i] != ' ' && flag == 1){ num_processor[j] = string[i]; j++; }
			if (string[i] != ' ' && flag == 2){ execution_time[j] = string[i]; j++; }
			if (string[i] != ' ' && flag == 3){ deadline[j] = string[i]; j++; }
			if (string[i] != ' ' && flag == 4){ latest_schedulable[j] = string[i];	j++; }
			i++;
		}
		if (string[i] == '\0'){
			kernel_ID = atoi(KERNEL);
			if (kernel_ID < 0){
				printf("ERROR::get_kernel_information - KERNEL ID needs to be in the range 0 - N\n");
				return RTGS_ERROR_INVALID_PARAMETERS;
			}
			kernel[kernel_ID].Pn = atoi(num_processor);
			kernel[kernel_ID].Texe = atoi(execution_time);
			kernel[kernel_ID].Td = atoi(deadline);
			kernel[kernel_ID].Tls = atoi(latest_schedulable);
			num_kernels++;
		}
	}
	fclose(fp);

	return num_kernels;
}


/* The Function is to read the time frames in which these Kernels are released */
int get_kernel_release_times(const char *File)
{
	char string[FILE_MAX_KERNELS];
	int i = 0;

	FILE * fp;
	fp = fopen(File, "r");		// read mode
	if (fp == NULL) {
		printf("ERROR::get_kernel_release_times - error while opening the file.\n");
		return RTGS_FAILURE;
	}

	while (fgets(string, 2, fp) != NULL)
	{
		if (string[0] == '0'){ gobalReleaseTime[i] = 0; i++; }
		else if (string[0] == '1'){ gobalReleaseTime[i] = 1; i++; }
		else if (string[0] == '2'){ gobalReleaseTime[i] = 2; i++; }
		else if (string[0] == '3' || string[0] == '4' || string[0] == '5'){
			printf("ERROR::get_kernel_release_times - release of MAX 2 Kernels Simultanously supported in this release\n");
			return RTGS_ERROR_INVALID_PARAMETERS;
		}
	}
	fclose(fp);
	int MaxRunTime = i;

	return MaxRunTime;
}



