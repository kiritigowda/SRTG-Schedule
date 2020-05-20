/*
* RTGS-AEAP_ALPA_improve.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int AEAP_ALAP_improve
(
    jobAttributes *jobAttributesList,
    int job_release_time,
    int present_time,
    int processors_available,
    scheduledResourceNode **processorsAllocatedList,
    scheduledResourceNode **jobScheduledQueueList
)
{
    printf("AEAP_ALAP_IMPROVE: ALAP IMPROVED scheduling Not Implemented\n");
    return processors_available;
}
