/*
* RTGS-linked_list.c
*      Author: Kiriti Nagesh Gowda
*/

#include"RTGS.h"

int64_t RTGS_GetClockCounter()
{
#if _WIN32
	LARGE_INTEGER v;
	QueryPerformanceCounter(&v);
	return v.QuadPart;
#else
	return chrono::high_resolution_clock::now().time_since_epoch().count();
#endif
}

int64_t RTGS_GetClockFrequency()
{
#if _WIN32
	LARGE_INTEGER v;
	QueryPerformanceFrequency(&v);
	return v.QuadPart;
#else
	return chrono::high_resolution_clock::period::den / chrono::high_resolution_clock::period::num;
#endif
}

// Backup processor list
backup_list* insert_ALAP_list(backup_list* head, int Tr, int Tf, int Pg, int KN) {

	backup_list* temp = (backup_list*)malloc(sizeof(backup_list));
	temp->data = Tr;
	temp->Tf = Tf;
	temp->Pg = Pg;
	temp->next = NULL;
	temp->KN = KN;

	if (head == NULL)
		head = temp;

	else {
		backup_list* temp1;
		temp1 = head;

		while (temp1 != NULL) {
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
backup_list* position_delete_list(backup_list* head) {

	backup_list* temp;
	temp = head;

	if (temp == NULL) {
#if DEBUG_MESSAGES
		printf("The List is empty\n");
#endif
		return (head);
	}

	head = temp->next;
	free(temp);

	return (head);
}


// Backup processor list
backup_list* insert_list(backup_list* head, int x) {

	backup_list* temp = (backup_list*)malloc(sizeof(backup_list));

	temp->data = x;
	temp->Tf = 0;
	temp->Pg = 0;
	temp->next = NULL;

	if (head == NULL)
		head = temp;

	else {
		backup_list* temp1;

		temp1 = head;

		while (temp1 != NULL) {

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
backup_list *clean_list(backup_list * head) {

	backup_list  *temp1;

	while (head != NULL) {

		temp1 = head->next;

		free(head);

		head = temp1;

	}
	return(head);
}


//Ascending insert function

Node* ascending_insert(Node* head, int x, int Tf, int Pf, int KN, int SA) {

	Node* temp;
	int count = 1;
	int flag = 0;

	temp = head;

	Node* temp1 = (Node*)malloc(sizeof(Node));


	//Values into the variable
	temp1->Tr = 0;

	temp1->data = x;


	temp1->Tf = Tf;
	temp1->P_f_g = Pf;

	temp1->SA = SA;
	temp1->KN = KN;

	temp1->next = NULL;
	temp1->Kernel_next = NULL;



	if (head == NULL) {
		head = temp1;
		flag = 1;
	}


	else if (x <= temp->data) {
		temp1->next = head;
		head = temp1;
		head = remove_recurring_node(head);

		flag = 1;
	}

	else {

		while (temp != NULL) {

			if (x <= temp->data) {
				head = position_insert(head, temp1, count);
				head = remove_recurring_node(head);
				flag = 0;
				return head;
			}

			temp = temp->next;
			count++;
		}

	}

	if (flag == 0) {
		head = insert(head, temp1);
		head = remove_recurring_node(head);
	}
	return (head);

}


//Remove recurring variables function

Node* remove_recurring_node(Node* head) {

	Node *temp, *kernel_check;

	Node *temp1;

	temp = head;

	while (temp->next != NULL) {
		temp1 = temp->next;

		if (temp->data == temp1->data) {
			//printf("%d is removed from the list\n",temp->data);
			//printf("Processors Available before removing:1-- %d 2-- %d \n\n",temp->P_f_g, temp1->P_f_g);

			Node* t1 = (Node*)malloc(sizeof(Node));
			Node* t2 = (Node*)malloc(sizeof(Node));

			t1->data = temp->data;
			t1->Tr = temp->Tr;
			t1->Tf = temp->Tf;
			t1->P_f_g = temp->P_f_g;
			t1->SA = temp->SA;
			t1->KN = temp->KN;
			t1->next = NULL;
			t1->Kernel_next = NULL;

			t2->data = temp1->data;
			t2->Tr = temp1->Tr;
			t2->Tf = temp1->Tf;
			t2->P_f_g = temp1->P_f_g;
			t2->SA = temp1->SA;
			t2->KN = temp1->KN;
			t2->next = NULL;
			t2->Kernel_next = temp1->Kernel_next;


			if (t2->Kernel_next == NULL) {
				temp->Kernel_next = t2;
				t2->Kernel_next = t1;
			}

			else {

				free(t2);
				temp->Kernel_next = temp1->Kernel_next;
				kernel_check = temp1->Kernel_next;
				while (kernel_check->Kernel_next != NULL)
					kernel_check = kernel_check->Kernel_next;

				if (kernel_check->Kernel_next == NULL)
					kernel_check->Kernel_next = t1;
			}


			temp->P_f_g = temp->P_f_g + temp1->P_f_g;
			temp->KN = -99;
			temp->Tr = -99;

			//printf("Processors Available after removing: %d\n\n",temp->P_f_g );

			temp->next = temp1->next;

			free(temp1);

			if (temp->next == NULL)
				return head;

		}

		temp = temp->next;
	}



	return (head);

}




//Insert a variable function

Node* insert(Node* head, Node* x) {

	Node* temp;

	Node* temp1 = x;

	if (head == NULL)
		head = temp1;


	else {

		temp = head;

		while (temp->next != NULL) {
			temp = temp->next;
		}

		temp->next = temp1;

	}

	return (head);

}



//Insert a variable in a given position

Node* position_insert(Node* head, Node* x, int p) {


	Node* temp;
	Node* temp1;
	int count = 1;
	temp = head;

	temp1 = x;

	if (p == 1) {
		//printf("The Node should be inserted after %d\n",head->data);

		temp1->next = head;
		head = temp1;

		return (head);

	}



	if (temp == NULL) {
		head = temp1;
		return (head);

	}



	while (temp->next != NULL) {


		if (count == (p - 1))
		{

			//printf("The Node should be inserted after %d\n",temp->data);


			temp1->next = temp->next;
			temp->next = temp1;
			return (head);
		}

		temp = temp->next;
		++count;

	}

	if (count == p + 1) {
		head = insert(head, x);
		return (head);
	}

	return (head);
}


//Delete a node from the list

Node* position_delete(Node* head, int p) {


	Node* temp;
	Node* temp1;
	int count = 1;



	temp = head;


	if (temp == NULL) {
#if DEBUG_MESSAGES
		printf("The List is empty\n");
#endif
		return (head);
	}



	if (p == 1) {


		head = temp->next;
		free(temp);
		return (head);
	}

	while (temp->next != NULL) {


		if (count == (p - 1))
		{
			//printf("The Node after %d should be deleted\n",temp->data);
			temp1 = temp->next;
			temp->next = temp1->next;

			free(temp1);
			//printf("The Node %d should be connected\n",temp->data);

			//temp1 = temp2;
			return (head);
		}

		temp = temp->next;
		++count;

	}

	return head;

}





//clean node
Node *clean_node(Node * head) {

	Node  *temp1;

	while (head != NULL) {

		temp1 = head->next;

		free(head);

		head = temp1;

	}

	return(head);
}



//Reverese a list

Node* reverse(Node* head) {

	Node *current, *prev, *next;

	current = head;

	prev = NULL;


	while (current != NULL) {

		next = current->next;
		current->next = prev;
		prev = current;
		current = next;
	}

	head = prev;

	return (head);

}



//Print the list
void print(Node* head) {

	Node* temp;
	temp = head;

	printf("\nThe List::->");
	while (temp != NULL) {
		printf("(T-%d--P-%d)->", temp->data, temp->P_f_g);
		temp = temp->next;
	}
	printf("NULL\n\n");

	return;
}

//Print the list in reverse order
void R_print(Node *p) {
	if (p == NULL)return;

	R_print(p->next);
	printf("%d->", p->data);
	return;
}


//Print the list
void Kernel_queue_print(Node* head) {
	Node* temp;
	temp = head;
	printf("\n\nKernels Scheduled for EXE::->");
	while (temp != NULL) {
		printf("(T-%d--P-%d--KN-%d)->", temp->data, temp->P_f_g, temp->KN);
		temp = temp->next;
	}
	printf("NULL\n");

	return;
}



