#ifndef LIST_H
#define LIST_H


typedef struct List 
{
    char *string;
    struct List *next;
}List;

List* createList ( char *data );

List* findLast( List *head );

void freeList( List **head );

void printList ( List *head );

int insertList( List **head, char *data );

#endif //LIST_H
