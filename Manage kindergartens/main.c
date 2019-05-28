#include <stdio.h>
#include <stdlib.h>

#include "General.h"
#include "Kindergarten.h"
#include "Child.h"
#include "City.h"

int main() {
	City utz = { NULL, 0 };
	int uReq, num;
	printf("for text file press 0 \n for bin file press 1\n");
	scanf("%d", &num);

	//first time read
	readCity(&utz, num);
	do {
		uReq = menu();
		switch (uReq) {
		case READ_CITY:
			readCity(&utz, num);
			break;

		case SHOW_CITY:
			showCityGardens(&utz);
			break;

		case SHOW_GARDEN:
			showSpecificGardenInCity(&utz);
			break;

		case WRITE_CITY:
			saveCity(&utz, num);
			break;

		case ADD_GARDEN:
			cityAddGarden(&utz);
			break;

		case ADD_CHILD:
			addChildToSpecificGardenInCity(&utz);
			break;

		case CHILD_BIRTHDAY:
			birthdayToChild(&utz);
			break;

		case COUNT_GRADUATE:
			printf("There are %d children going to school next year\n",
					countChova(&utz));
			break;

		case SORT_GARDENS_BY_NAME:
			sortGardenByName(&utz);
			break;

		case SORT_CHILDRENS_BY_ID:
			sortChildrenByID(&utz);
			break;

		case SORT_GARDENS_BY_TYPE_AND_NUMBER:
			sortGardenByTypeAndNumberOfChildren(&utz);
			break;

		case CREATE_KINDERGARTENS_LINKED_LIST:
			kindergartensLinkedList(&utz);
			break;

		}
	} while (uReq != EXIT);

	releaseCity(&utz); //free all allocations

	return EXIT_SUCCESS;
}

