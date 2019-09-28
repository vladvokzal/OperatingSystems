
#include "list.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

List* createList(char *data) 
{
	List* newNode = (List*)malloc( (strlen(data) + 1) * sizeof(List));
	
	if (!newNode) 
	{
		return NULL;
	}

	newNode->string = (char *)malloc((strlen(data) + 1 ) * sizeof(char));
	strcpy(newNode->string, data);
	newNode->next = NULL;
	
	return newNode;
}

List* findLast(List* head) 
{
	while (head->next)
	{
		head = head->next;
	}
	return head;
}

void freeList( List** head ) 
{
	List* prev = NULL;
	
	if (NULL == *head) 
	{
		return;
	}
	
	while ((*head)->next) 
	{
		prev = ( *head );

		(*head) = (*head)->next;
		
		free(prev->string);
		free(prev);
	}
	
	free((*head)->string);
	free(*head);
}

void printList (List* head) 
{
	while(head) 
	{
		printf("%s", head->string);
		head = head->next;
	}
}

int insertList( List** head, char *data ) 
{
	if ( NULL == (*head) ) 
	{
		*head = createList(data);
	}
	else 
	{
		List* last = findLast(*head);
		List* new_node = createList(data);
		
		if (!new_node)
		{
			return 0;
		}
		
		last->next = new_node;
	}
	
	return 1;
}
