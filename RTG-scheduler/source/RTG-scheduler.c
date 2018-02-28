/*
* RTG-scheduler.c
*      Author: Kiriti Nagesh Gowda
*/
#include"RTGS.h"

/**********************************************************************************************************
Scheduler Main - Mode Selector
***********************************************************************************************************/
int scheduler_main(char *kernelFilename, char *releaseTimeFilename, int schedulerMode)
{
	//PROFILER_START(SRTG, scheduler_main)
	RTGS_Status status = RTGS_SUCCESS;
	switch (schedulerMode) {
	case 1:
		status = RTGS_mode_1(kernelFilename, releaseTimeFilename);
		break;
	case 2:
		status = RTGS_mode_2(kernelFilename, releaseTimeFilename);
		break;
	case 3:
		status = RTGS_mode_3(kernelFilename, releaseTimeFilename);
		break;
	case 4:
		status = RTGS_mode_4(kernelFilename, releaseTimeFilename);
		break;
	case 5:
		status = RTGS_mode_5(kernelFilename, releaseTimeFilename);
		break;

	default:
		printf("\nMode  Not Specified/Not implemented, switched to default mode\n");
		printf("Mode 5:: AEAP/ALAP BP with APLAP improver mode->AEAP/ALAP BP Improve\n");
		status = RTGS_mode_5(kernelFilename, releaseTimeFilename);
		break;
	}
	//PROFILER_STOP(SRTG, scheduler_main)
	return status;
}
