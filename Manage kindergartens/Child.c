#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "Child.h"
#include "General.h"

/**************************************************/
/*             Read a Child from a file           */
/**************************************************/
void readChild(FILE* fp, Child* pChild, int num) {

	unsigned char* temp;
	temp= (unsigned char*)malloc (2*sizeof(unsigned char));
	if (num == 0) {
		fscanf(fp, "%d", &pChild->id);
		fscanf(fp, "%d", &pChild->age);
	} else if (num == 1) {

		fread(temp, sizeof(unsigned char), 2, fp);
		pChild->id=   (temp[1] & 0x1F)<< 8 | temp[0];
		pChild->age=temp[1] >> 5;

	}
}

/**************************************************/
/*            show a Child				          */
/**************************************************/
void showChild(const Child* pChild) {
	printf("\nID:%d  ", pChild->id);
	printf("Age:%d  ", pChild->age);
}

/**************************************************/
void getChildFromUser(Child* pChild, int id)

{
	pChild->id = id;

	puts("\nAge:\t");
	scanf("%d", &pChild->age);
}

/**************************************************/
/*Write a Child to the open file				*/
/**************************************************/
void writeChild(FILE* fp, const Child* pChild, int num) {
	unsigned char* buffer;
	buffer = (unsigned char*)malloc (2*sizeof(unsigned char));

	if (num == 0)
		fprintf(fp, "%d %d\n", pChild->id, pChild->age);
	else if (num == 1)
	{
		buffer[0]=pChild->id;
		buffer[1]= pChild->id >> 8 | pChild->age << 5;
		fwrite(buffer, sizeof(unsigned char), 2, fp);


		//pChild->id=   (temp[1] & 0x1F) << 8 | temp[0];
		//pChild->age=temp[1] >> 5;

	}
}

//linear search
int findChildById(Child** pChildList, int count, int id) {

	Child** child=NULL;
	Child serch= {id, 0};
	Child* pSerch =&serch;

	qsort(pChildList, count, sizeof(Child*), compareChildByID);
	child = (Child**)bsearch(&pSerch, pChildList,count,sizeof(Child*),compareChildByID);

	if (child)
		return child-pChildList;

	return -1;

}

void birthday(Child* pChild) {
	pChild->age++;
}

int compareChildByID(const void* st1, const void* st2) {
	Child* pS1 = *(Child**) st1;
	Child* pS2 = *(Child**) st2;
	return ((pS1->id) - (pS2->id));
}
