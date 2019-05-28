#include <stdio.h>

#include "City.h"
#include "Kindergarten.h"
#include "Node.h"

void readCity(City* pCity, int num) {
	if (pCity->pGardenList != NULL) {
		releaseCity(pCity);
		pCity->count = 0;
	}
	if (num == 0)
		pCity->pGardenList = readAllGardensFromFile(DATA_FILE, &pCity->count,
				num);
	else if (num == 1)
		pCity->pGardenList = readAllGardensFromFile(BIN_FILE, &pCity->count,
				num);

	if (pCity->pGardenList == NULL)
		printf("Error reading city information\n");
}

void showCityGardens(City* pCity) {
	showAllGardens(pCity->pGardenList, pCity->count);
}

void showSpecificGardenInCity(City* pCity) {
	showGardenMenu(pCity->pGardenList, pCity->count);
}

void saveCity(City* pCity, int num) {
	if (num == 0)
		writeGardensToFile(pCity->pGardenList, pCity->count, DATA_FILE, num);

	else if (num == 1)
		writeGardensToFile(pCity->pGardenList, pCity->count, BIN_FILE, num);

}

void cityAddGarden(City* pCity) {
	pCity->pGardenList = addGarden(pCity->pGardenList, &pCity->count);
	if (pCity->pGardenList == NULL) //Allocation error
		printf("Error adding kindergarten\n");
}

void addChildToSpecificGardenInCity(City* pCity) {
	addChildToGarden(pCity->pGardenList, pCity->count);
}

void birthdayToChild(City* pCity) {
	handleBirthdayToChild(pCity->pGardenList, pCity->count);
}

int countChova(City* pCity) {
	int i;
	int count = 0;
	for (i = 0; i < pCity->count; i++) {
		if (pCity->pGardenList[i]->type == Chova)
			count += pCity->pGardenList[i]->childCount;
	}
	return count;
}

void releaseCity(City* pCity) {
	release(pCity->pGardenList, pCity->count);
}

void sortGardenByTypeAndNumberOfChildren(City* pCity) {
	insertionSort(pCity->pGardenList, pCity->count,sizeof(Garden*), conpareByNumberOfChildren);
	insertionSort(pCity->pGardenList, pCity->count,sizeof(Garden*), conpareByGardenType);
}
void sortChildrenByID(City* pCity) {
	char name[100];
	printf("Enter garden name \n");
	scanf("%s", name);
	sortGardenByID(findGardenByName(pCity->pGardenList, pCity->count, name));

}

void sortGardenByName(City* pCity) {
	insertionSort(pCity->pGardenList, pCity->count, sizeof(Garden*), conpareByGardenName);
}

LIST* createLinkedListForKindergartenType(City* pCity, int type) {
	int i;
	NODE* pNode;
	LIST list;
	LIST *pList = &list;
	if(L_init(pList))
		pNode = &(*pList).head;


	for (i = 0; i < pCity->count; i++) {
		if (pCity->pGardenList[i]->type == type) {
			pNode = L_insert(pNode, *pCity->pGardenList[i]);
		}
	}
	return pList;
}

void displayKindergartensForList(LIST list) {
	L_printGardenList(&list);
}

LIST* kindergartensLinkedList(City* pCity) {
	LIST *list;
	int type = getTypeOption();

	list = createLinkedListForKindergartenType(pCity, type);
	displayKindergartensForList(*list);

	return list;

}
