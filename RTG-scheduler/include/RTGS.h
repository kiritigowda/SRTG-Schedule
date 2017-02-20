/*
* RTGS.h
*      Author: Kiriti Nagesh Gowda
*/

#ifndef RTGS_H
#define RTGS_H

#if _WIN32
#include<Windows.h>
#endif

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <stdint.h>

#define DEBUG_MESSAGES 1 // debug messages

#define MAX_GPU_PROCESSOR 14        // Total streaming multi-processors available on the GPU
#define MAX_KERNELS 100				// Max Kernels needed to be scheduled
#define PROCESSOR_LIMIT 10			// ALAP Processor Limit

/*! \brief Sets the standard enumeration type size to be a fixed quantity.
* \details All enumerable fields must use this type as the container to
* enforce enumeration ranges and sizeof() operations.
* \ingroup group_basic_features
*/
typedef int RTGS_ENUM;

/*! \brief The enumeration of all status codes.
* \see rtgs_status.
* \ingroup group_basic_features
*/
enum RTGS_status_e {
	RTGS_ERROR_INVALID_MODULE = -11,		/*!< \brief This is returned from <tt>\ref vxLoadKernels</tt> when the module does not contain the entry point. */
	RTGS_ERROR_INVALID_PARAMETERS = -10,	/*!< \brief Indicates that the supplied parameter information does not match the kernel contract. */
	RTGS_ERROR_OPTIMIZED_AWAY = -9,			/*!< \brief Indicates that the object refered to has been optimized out of existence. */
	RTGS_ERROR_NO_MEMORY = -8,				/*!< \brief Indicates that an internal or implicit allocation failed. Typically catastrophic. After detection, deconstruct the context. \see vxVerifyGraph. */
	RTGS_ERROR_NO_RESOURCES = -7,			/*!< \brief Indicates that an internal or implicit resource can not be acquired (not memory). This is typically catastrophic. After detection, deconstruct the context. \see vxVerifyGraph. */
	RTGS_ERROR_NOT_COMPATIBLE = -6,			/*!< \brief Indicates that the attempt to link two parameters together failed due to type incompatibilty. */
	RTGS_ERROR_NOT_ALLOCATED = -5,			/*!< \brief Indicates to the system that the parameter must be allocated by the system.  */
	RTGS_ERROR_NOT_SUFFICIENT = -4,			/*!< \brief Indicates that the given graph has failed verification due to an insufficient number of required parameters, which cannot be automatically created. Typically this indicates required atomic parameters. \see vxVerifyGraph. */
	RTGS_ERROR_NOT_SUPPORTED = -3,			/*!< \brief Indicates that the requested set of parameters produce a configuration that cannot be supported. Refer to the supplied documentation on the configured kernels. \see vx_kernel_e. */
	RTGS_ERROR_NOT_IMPLEMENTED = -2,		/*!< \brief Indicates that the requested kernel is missing. \see vx_kernel_e vxGetKernelByName. */
	RTGS_FAILURE = -1,						/*!< \brief Indicates a generic error code, used when no other describes the error. */
	RTGS_SUCCESS = 0,						/*!< \brief No error. */
};

/*! \brief A formal status type with known fixed size.
* \see vx_status_e
* \ingroup group_basic_features
*/
typedef RTGS_ENUM RTGS_Status;


// Backup list to revert the processor actions
struct list {
	int KN;
	int data;
	int Tf;
	int Pg;
	struct list* next;
};

struct Linked_list {
	int KN;
	int Tr;
	int Tf;
	int P_f_g;
	int SA;
	int data;
	struct Linked_list* next;
	struct Linked_list* Kernel_next;
};

/* Kernel Structure */
struct Kernels {
	int Pn;                                        //Processors needed
	int Texe;                                      //Execution time
	int Td;                                        //Deadline
	int Tls;                                       //Latest schedule time
};


typedef struct list backup_list;
typedef struct Linked_list Node;
typedef struct Kernels Kernel_INFO;

int RT[MAX_KERNELS];
int count;
int CPU_Kernel;
backup_list* alap;


int Scheduler_main_mode_1(char *kernel, char *Releasetime);
int Mode_1_book_keeper(struct Kernels* kernel, int KN, int Pa, int i, Node** Pro_free_list);

int Scheduler_main_mode_2(char *kernel, char *Releasetime);
int Mode_2_book_keeper(Kernel_INFO* kernel, int KN, int Pa, int i, Node **Pro_free_list, Node **Kernel_queue);
int Mode_2_Processors_Unavailable(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue);
int Mode_2_AEAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue);

int Scheduler_main_mode_3(char *kernel, char *Releasetime);
int Mode_3_book_keeper(Kernel_INFO* kernel, int KN, int Pa, int i, Node **Pro_free_list, Node **Kernel_queue);
int Mode_3_Processors_Unavailable(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue);
int Mode_3_AEAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue);
int Mode_3_ALAP(Kernel_INFO *kernel, int KN, int i, int Pa, Node ** Pro_free_list, Node **Kernel_queue);

int scheduler_main(char *kernel, char *Releasetime, int mode);

int Get_kernel_information(Kernel_INFO*, char *File);
int Get_kernel_release_times(char *File);

int Kernel_book_keeper(Kernel_INFO*, int, int, int, Node**, Node**);
int Processors_unavailable(Kernel_INFO*, int, int, int, Node**, Node**);

int AEAP(Kernel_INFO*, int, int, int, Node**, Node**);
int ALAP(Kernel_INFO*, int, int, int, Node**, Node**);

int AEAP_Flagged(Kernel_INFO*, int, int, int, Node **, Node **);
int ALAP_Flagged(Kernel_INFO*, int, int, int, Node **, Node **);

int ALAP_improve(Kernel_INFO *, int, int, int, Node **, Node **);
int AEAP_ALAP_improve(Kernel_INFO *, int, int, int, Node **, Node **);

void Queue_kernel_execution(int, int, int, int, int, Node**);
void Kernel_queue_handler(int, int, int, int, int, Node**);

int Retrieve_processors(int, int, Node **);
int Dispatch_queued_kernels(int, int, Node **, Node**);

//Function Prototyping
Node* insert(Node* head, Node* x);
Node* ascending_insert(Node* head, int x, int Tf, int Pf, int KN, int SA);
Node* position_insert(Node* head, Node* x, int p);
Node* position_delete(Node* head, int p);
Node* reverse(Node* head);
Node* remove_recurring_node(Node* head);
Node* clean_node(Node* head);

backup_list* insert_list(backup_list*, int);
backup_list* clean_list(backup_list*);
backup_list* position_delete_list(backup_list*);
backup_list* insert_ALAP_list(backup_list*, int, int, int, int);

void print(Node* head);
void Kernel_queue_print(Node* head);
void R_print(Node* p);

int64_t RTGS_GetClockCounter();
int64_t RTGS_GetClockFrequency();

#endif /* RTGS_H */
