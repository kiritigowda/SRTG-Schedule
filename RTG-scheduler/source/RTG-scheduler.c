/*
* RTG-scheduler.c
*      Author: Kiriti Nagesh Gowda
*/
#include"RTGS.h"

/**********************************************************************************************************
Scheduler Main - Mode Selector
***********************************************************************************************************/
int scheduler_main(char *kernel_file, char *Releasetime_file, int mode)
{
	RTGS_Status status = RTGS_SUCCESS;
	switch (mode) {
	case 1:
		status = RTGS_mode_1(kernel_file, Releasetime_file);
			break;
	case 2:
		status = RTGS_mode_2(kernel_file, Releasetime_file);
			break;
	case 3:
		status = RTGS_mode_3(kernel_file, Releasetime_file);
			break;
	case 4:
		status = RTGS_mode_4(kernel_file, Releasetime_file);
			break;
	case 5:
		status = RTGS_mode_5(kernel_file, Releasetime_file);
			break;

	default:
		printf("\nMode Not Specified/Mode %d not implemented, switched to default mode\n", mode);
		printf("Mode 5::AEAP / ALAP BP with APLAP improver mode->AEAP / ALAP BP Improve\n");
		status = RTGS_mode_5(kernel_file, Releasetime_file);
			break;
	}

	return status;
}
