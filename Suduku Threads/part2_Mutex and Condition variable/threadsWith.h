#ifndef THREADSWITH_H_
#define THREADSWITH_H_

#define SIZE 9
#define TASKS 27
#define IS3X3VALID 0
#define ISCOLUMNVALID 1
#define ISROWVALID 2
#define DONE 1
#define NOT_DONE_YET 0

typedef struct {
	int row;
	int column;
} parameters;

typedef struct {
	parameters *index;
	int func;
	int check;
} task;

int load_grid(int sudoku[][SIZE], char *filename);
void buildlist();
int isColumnValid(int row, int col);
int isRowValid(int row, int col);
int is3x3Valid(int row, int col);
void main_loop(int index);
void release(task **newTesk);

#endif
