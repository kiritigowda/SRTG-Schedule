/*
* RTGS-helper_functions.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"
#ifndef _WIN32
#include <x86intrin.h>
#define __int64 int64_t
static inline __int64 my_rdtsc(){ return __rdtsc(); }
#endif

//! \brief clock counter function
int64_t RTGS_GetClockCounter()
{
#if _WIN32
	LARGE_INTEGER v;
	QueryPerformanceCounter(&v);
	return v.QuadPart;
#else
	//return std::chrono::high_resolution_clock::now().time_since_epoch().GLOBAL_GPU_KERNELS();
	return my_rdtsc();
#endif
}

//! \brief clock frequency function
int64_t RTGS_GetClockFrequency()
{
#if _WIN32
	LARGE_INTEGER v;
	QueryPerformanceFrequency(&v);
	return v.QuadPart;
#else
	//return std::chrono::high_resolution_clock::period::den / std::chrono::high_resolution_clock::period::num;
	return 100000000;
#endif
}

//! \brief environment variable function
bool RTGS_GetEnvironmentVariable(const char * name, char * value, size_t valueSize)
{
#if _WIN32
	DWORD len = GetEnvironmentVariableA(name, value, (DWORD)valueSize);
	value[valueSize - 1] = 0;
	return (len > 0) ? true : false;
#else
	const char * v = getenv(name);
	if (v) {
		strncpy(value, v, valueSize);
		value[valueSize - 1] = 0;
	}
	return v ? true : false;
#endif
}

int RTGS_PrintScheduleSummary(int mode, int maxKernels, kernelInfo *kernelInfoList)
{
#if _WIN32
	CreateDirectory("RTGS-Summary", NULL);
#else
	struct stat st = { 0 };
	if (stat("RTGS-Summary", &st) == -1) { mkdir("RTGS-Summary", 0700); }
#endif

	char profiler[1024] = "RTGS-Summary/RTGS";

	char pCSVfile[1024]; sprintf(pCSVfile, "%s-Mode-%d-Job-Summary.csv", profiler, mode);

	FILE * fp = fopen(pCSVfile, "w"); if (!fp) { printf("ERROR: unable to create '%s'\n", pCSVfile); return RTGS_ERROR_NO_RESOURCES; }

    fprintf(fp, "Job,Processors,Execution Time,Deadline,Release Time,Scheduler Overhead,Scheduled At,Rescheduled to,Completion Time,Scheduled Hardware\n");
	for (int i = 0; i < maxKernels; i++) {
		fprintf(fp, "%d,%d,%d,%d,%d,%f,%d,%d,%d,%d\n",i,
			kernelInfoList[i].processor_req,
			kernelInfoList[i].execution_time,
			kernelInfoList[i].deadline,
			kernelInfoList[i].release_time,
			kernelInfoList[i].schedule_overhead,
			kernelInfoList[i].scheduled_execution,
			kernelInfoList[i].rescheduled_execution,
			kernelInfoList[i].completion_time,
			kernelInfoList[i].schedule_hardware
			);
	}
	fclose(fp);
	return RTGS_SUCCESS;
}

// Backup processor list
backup_list* insert_ALAP_list
(
	backup_list* head, 
	int kernel_release_time, 
	int processor_release_time, 
	int processors_allocated, 
	int kernel_number
) 
{

	backup_list* temp = (backup_list*)malloc(sizeof(backup_list));
	temp->data = kernel_release_time;
	temp->processor_release_time = processor_release_time;
	temp->processors_allocated = processors_allocated;
	temp->next = NULL;
	temp->kernel_number = kernel_number;

	if (head == NULL)	head = temp;
	else {
		backup_list* temp1;
		temp1 = head;

		while (temp1 != NULL) 
		{
			if (temp1->next == NULL) {
				temp1->next = temp;
				return head;
			}
			temp1 = temp1->next;
		}
	}
	return head;
}

// Backup  list delete
backup_list* position_delete_list(backup_list* head) 
{
	backup_list* temp;
	temp = head;
	if (temp == NULL) {
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("The List is empty\n");
		}
		return head;
	}
	head = temp->next;
	free(temp);
	return head;
}

// Backup processor list
backup_list* insert_list(backup_list* head, int x) 
{
	backup_list* temp = (backup_list*)malloc(sizeof(backup_list));

	temp->data = x;
	temp->processor_release_time = 0;
	temp->processors_allocated = 0;
	temp->next = NULL;

	if (head == NULL)	head = temp;
	else 
	{
		backup_list* temp1;
		temp1 = head;
		while (temp1 != NULL) 
		{
			if (temp1->next == NULL) {
				temp1->next = temp;
				return head;
			}
			temp1 = temp1->next;
		}
	}
	return head;
}

//clean list
backup_list *clean_list(backup_list * head) 
{
	backup_list  *temp1;

	while (head != NULL) {
		temp1 = head->next;
		free(head);
		head = temp1;
	}
	return head;
}

//Ascending insert function
scheduledNode* ascending_insert
(
scheduledNode* head,
int x,
int processor_release_time,
int processorReleased,
int kernel_number,
int schedule_method
)
{
	int count = 1, flag = 0;
	scheduledNode* temp = head;
	scheduledNode* temp1 = (scheduledNode*)malloc(sizeof(scheduledNode));
	//Values into the variable
	temp1->kernel_release_time = 0;
	temp1->data = x;
	temp1->processor_release_time = processor_release_time;
	temp1->processors_allocated = processorReleased;
	temp1->schedule_method = schedule_method;
	temp1->kernel_number = kernel_number;
	temp1->next = NULL;
	temp1->kernel_next = NULL;

	if (head == NULL) {
		head = temp1;
		flag = 1;
	}
	else if (x <= temp->data)
	{
		temp1->next = head;
		head = temp1;
		head = remove_recurring_node(head);
		flag = 1;
	}
	else
	{
		while (temp != NULL)
		{
			if (x <= temp->data)
			{
				head = position_insert(head, temp1, count);
				head = remove_recurring_node(head);
				flag = 0;
				return head;
			}
			temp = temp->next;
			count++;
		}
	}

	if (flag == 0)
	{
		head = insert(head, temp1);
		head = remove_recurring_node(head);
	}
	return head;
}

//Remove recurring variables function
scheduledNode* remove_recurring_node(scheduledNode* head)
{
	scheduledNode *temp, *kernel_check;
	scheduledNode *temp1;

	temp = head;
	while (temp->next != NULL)
	{
		temp1 = temp->next;
		if (temp->data == temp1->data)
		{
			scheduledNode* t1 = (scheduledNode*)malloc(sizeof(scheduledNode));
			scheduledNode* t2 = (scheduledNode*)malloc(sizeof(scheduledNode));

			t1->data = temp->data;
			t1->kernel_release_time = temp->kernel_release_time;
			t1->processor_release_time = temp->processor_release_time;
			t1->processors_allocated = temp->processors_allocated;
			t1->schedule_method = temp->schedule_method;
			t1->kernel_number = temp->kernel_number;
			t1->next = NULL;
			t1->kernel_next = NULL;

			t2->data = temp1->data;
			t2->kernel_release_time = temp1->kernel_release_time;
			t2->processor_release_time = temp1->processor_release_time;
			t2->processors_allocated = temp1->processors_allocated;
			t2->schedule_method = temp1->schedule_method;
			t2->kernel_number = temp1->kernel_number;
			t2->next = NULL;
			t2->kernel_next = temp1->kernel_next;

			if (t2->kernel_next == NULL)
			{
				temp->kernel_next = t2;
				t2->kernel_next = t1;
			}
			else
			{
				free(t2);
				temp->kernel_next = temp1->kernel_next;
				kernel_check = temp1->kernel_next;
				while (kernel_check->kernel_next != NULL)
					kernel_check = kernel_check->kernel_next;

				if (kernel_check->kernel_next == NULL)
					kernel_check->kernel_next = t1;
			}
			temp->processors_allocated = temp->processors_allocated + temp1->processors_allocated;
			temp->kernel_number = MULTIPLE_KERNELS_SCHEDULED;
			temp->kernel_release_time = MULTIPLE_KERNELS_SCHEDULED;
			temp->next = temp1->next;

			free(temp1);

			if (temp->next == NULL)		return head;
		}
		temp = temp->next;
	}
	return head;
}

//Insert a variable function
scheduledNode* insert(scheduledNode* head, scheduledNode* x)
{
	scheduledNode* temp;
	scheduledNode* temp1 = x;

	if (head == NULL) head = temp1;
	else
	{
		temp = head;
		while (temp->next != NULL) {
			temp = temp->next;
		}
		temp->next = temp1;
	}
	return head;
}

//Insert a variable in a given position
scheduledNode* position_insert(scheduledNode* head, scheduledNode* x, int p) 
{
	scheduledNode* temp;
	scheduledNode* temp1;
	int count = 1;
	temp = head;
	temp1 = x;

	if (p == 1) 
	{
		temp1->next = head;
		head = temp1;
		return head;
	}

	if (temp == NULL) 
	{
		head = temp1;
		return head;
	}

	while (temp->next != NULL)
	{
		if (count == (p - 1))
		{
			temp1->next = temp->next;
			temp->next = temp1;
			return head;
		}
		temp = temp->next;
		++count;
	}
	if (count == p + 1) {
		head = insert(head, x);
		return head;
	}

	return head;
}

//Delete a node from the list
scheduledNode* position_delete(scheduledNode* head, int p) 
{
	scheduledNode* temp;
	scheduledNode* temp1;
	int count = 1;
	temp = head;

	if (temp == NULL) 
	{
		if (GLOBAL_RTGS_DEBUG_MSG > 1) {
			printf("The List is empty\n");
		}
		return head;
	}

	if (p == 1)
	{
		head = temp->next;
		free(temp);
		return head;
	}

	while (temp->next != NULL) 
	{
		if (count == (p - 1))
		{
			temp1 = temp->next;
			temp->next = temp1->next;

			free(temp1);
			return head;
		}

		temp = temp->next;
		++count;
	}
	return head;
}

//clean node
scheduledNode *clean_node(scheduledNode * head) 
{
	scheduledNode  *temp1;
	while (head != NULL) 
	{
		temp1 = head->next;
		free(head);
		head = temp1;
	}
	return head;
}

//Reverese a list
scheduledNode* reverse(scheduledNode* head)
{
	scheduledNode *current, *prev, *next;
	current = head;
	prev = NULL;

	while (current != NULL)
	{
		next = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}
	head = prev;
	return head;
}

//Print the list
void print(scheduledNode* head)
{
	scheduledNode* temp;
	temp = head;
	printf("Scheduled Job List\n");
	while (temp != NULL) {
		if (temp->kernel_number != MULTIPLE_KERNELS_SCHEDULED){
			printf("	Job-%d	-- Completion Time:%d,	Processors Retrived:%d\n", temp->kernel_number, temp->data, temp->processors_allocated);
		}
		else{
			scheduledNode* temp1 = temp->kernel_next;
			while (temp1 != NULL){
				printf("	Job-%d	-- Completion Time:%d,	Processors Retrived:%d\n", temp1->kernel_number, temp1->data, temp1->processors_allocated);
				temp1 = temp1->kernel_next;
			}
		}
		temp = temp->next;
	}
	return;
}

//Print the list in reverse order
void R_print(scheduledNode *p)
{
	if (p == NULL)return;
	R_print(p->next);
	printf("%d->", p->data);
	return;
}

//Print the list
void Kernel_queue_print(scheduledNode* head) 
{
	scheduledNode* temp;
	temp = head;
	printf("Jobs Scheduled for GPU Execution\n");
	while (temp != NULL) {
		if (temp->kernel_number != MULTIPLE_KERNELS_SCHEDULED){
			printf("	Job-%d	-- Job Release Time:%d,	Processor Allocated:%d\n", temp->kernel_number, temp->data, temp->processors_allocated);
		}
		else{
			scheduledNode* temp1 = temp->kernel_next;
			while (temp1 != NULL){
				printf("	Job-%d	-- Job Release Time:%d,	Processor Allocated:%d\n", temp1->kernel_number, temp1->data, temp1->processors_allocated);
				temp1 = temp1->kernel_next;
			}
		}
		temp = temp->next;
	}
	return;
}



