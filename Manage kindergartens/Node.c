/*
 * Node.c

 *
 *  Created on: Jan 2, 2019
 *      Author: lir
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "Node.h"
#include "Kindergarten.h"

int  L_init ( LIST*  pList )
{
	if ( pList == NULL ) return 0;

	pList->head.next = NULL;
	return 1;
}

NODE* L_insert(NODE* pNode, DATA Value)
{
	NODE* temp;
	if (!pNode ) return NULL;
	temp = (NODE*)malloc(sizeof(NODE));
	if ( temp != NULL )  {
		temp->key = Value;
		temp->next = pNode->next;
		pNode->next = temp;
	}
	return temp;
}

int L_delete(NODE* pNode)
{
	NODE* tmp;

	if ( !pNode || !(tmp = pNode->next) ) return 0;

	pNode->next = tmp->next;
	free(tmp);
	return 1;
}


int L_free(LIST* pList)
{
	NODE *tmp;

	if ( !pList ) return 0;

	for ( tmp = &(pList->head);  L_delete(tmp);  ) ;
	return 0;
}

int L_printGardenList(LIST* pList)
{
	NODE *tmp;
	int	c = 0;

	if ( !pList ) return 0;

	printf("\n");
	for (tmp = pList->head.next; tmp; tmp = tmp->next, c++)
	{
		showGarden(&tmp->key);
		printf("\n-------------\n");
	}
	printf("\n");
	return c;
}




