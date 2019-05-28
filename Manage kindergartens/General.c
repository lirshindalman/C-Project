#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include "General.h"
#include "City.h"
#include "Kindergarten.h"

const char* optionStr[NofOptions] = { "Exit", "Read City information from file",
		"Show all Kindergartens", "Show a specific Kindergarten",
		"Save City information to file", "Add a Kindergarten", "Add a Child",
		"Birthday to a Child", "Count Hova childres",
		"Sort kindergartens by name", "Sort children by ID",
		"Sort kindergartens by type and number of childrn",
		"Create kindergartens linked list" };

/**************************************************/
int menu()
/**************************************************/
/**************************************************/
{
	int option, i;

	printf("\n==========================");
	printf("\nSelect:\n");
	for (i = 0; i < NofOptions; i++)
		printf("\n\t%d. %s.", i, optionStr[i]);
	printf("\n");
	scanf("%d", &option);
	return option;
}

char* getStrExactLength(char* inpStr) {
	char* theStr = NULL;
	size_t len;
	len = strlen(inpStr) + 1;
	theStr = (char*) malloc(len * sizeof(char));
	if (theStr != NULL)
		strcpy(theStr, inpStr);

	return theStr;
}

int checkAllocation(const void* p) {
	if (!p) {
		printf("ERROR! Not enough memory!");
		return 0;
	}
	return 1;
}

void insertionSort(void* arr, int size, int typeSize,
		int (*compare)(const void*, const void*)) {

	char  *i, *j;
	char* key=(char*)malloc(typeSize);

	for (i = (char*)arr+typeSize ; i < (char*)arr + size * typeSize ; i += typeSize) {
		memcpy(key, i, typeSize);
		for (j = i - typeSize ; j >= (char*) arr && compare(j, key) > 0 ; j -= typeSize) {
			memcpy(j + typeSize, j, typeSize);
			}
		memcpy(j + typeSize, key, typeSize);
		}
	free(key);
	}


void Variadic_Function(char* str, int num, ...) {
	va_list list;
	va_start(list, num);
	while (str != NULL) {
		printf("%s : %d", str, num);
		str = va_arg(list, char*);
		num = va_arg(list, int);
	}
	va_end(list);

}

