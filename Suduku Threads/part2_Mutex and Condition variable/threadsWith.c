/*
 * test.c
 *
 *  Created on: May 2, 2019
 *      Author: lir
 */
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "threadsWith.h"

int sudoku[SIZE][SIZE];
pthread_mutex_t mutex_current;
pthread_mutex_t mutex_done;
pthread_cond_t cond;
int success = 0;
int done = 0;
int current = 0;

task **newTesk;
/////************************************************** /
/* Function that read grid from file and store it in an array */
/////************************************************** /
int load_grid(int sudoku[][SIZE], char *filename) {
	FILE *input_file = fopen(filename, "r");
	if (input_file != NULL) {
		for (int i = 0; i < SIZE; i++)
			for (int j = 0; j < SIZE; j++)
				fscanf(input_file, "%d", &sudoku[i][j]);
		fclose(input_file);
		return 1;
	}
	return 0;
}

/////************************************************** /
/// Function that built the array of 27 tasks
/////************************************************** /
void buildlist() {
	newTesk = (task**) malloc(27 * sizeof(task*));
	int i, j;
	int x = 0;
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			if (i % 3 == 0 && j % 3 == 0) {
				parameters *data = (parameters*) malloc(sizeof(parameters));
				data->row = i;
				data->column = j;
				newTesk[x] = (task*) malloc(sizeof(task));
				newTesk[x]->func = IS3X3VALID;
				newTesk[x]->index = data;
				newTesk[x]->check = NOT_DONE_YET;
				x++;
			}
			if (i == 0) {
				parameters *columnData = (parameters *) malloc(
						sizeof(parameters));
				columnData->row = i;
				columnData->column = j;
				newTesk[x] = (task *) malloc(sizeof(task));
				newTesk[x]->func = ISCOLUMNVALID;
				newTesk[x]->index = columnData;
				newTesk[x]->check = NOT_DONE_YET;
				x++;
			}
			if (j == 0) {
				parameters *rowData = (parameters *) malloc(sizeof(parameters));
				rowData->row = i;
				rowData->column = j;
				newTesk[x] = (task *) malloc(sizeof(task));
				newTesk[x]->func = ISROWVALID;
				newTesk[x]->index = rowData;
				newTesk[x]->check = NOT_DONE_YET;
				x++;
			}
		}
	}
}

/////************************************************** /
// Method that determines if numbers 1-9 only appear once in a col
/////************************************************** /
int isColumnValid(int row, int col) {
	// Check if numbers 1-9 only appear once in the column
	int validityArray[SIZE] = { 0 };
	int i;
	for (i = 0; i < 9; i++) {
		int num = sudoku[i][col];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			pthread_mutex_lock(&mutex_done);
			done = 1;
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex_done);
			return 1;
		} else {
			validityArray[num - 1] = 1;

		}
	}
	// If reached this point, col subsection is valid.
	pthread_mutex_lock(&mutex_current);
	success++;
	pthread_mutex_unlock(&mutex_current);
	return 0;
}
/////************************************************** /
// Method that determines if numbers 1-9 only appear once in a row
/////************************************************** /
int isRowValid(int row, int col) {
	// Check if numbers 1-9 only appear once in the row
	int validityArray[SIZE] = { 0 };
	int i;
	for (i = 0; i < 9; i++) {
		int num = sudoku[row][i];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			pthread_mutex_lock(&mutex_done);
			done = 1;
			pthread_cond_signal(&cond);
			pthread_mutex_unlock(&mutex_done);
			return 1;
		} else {
			validityArray[num - 1] = 1;
		}
	}
	// If reached this point, row subsection is valid.
	pthread_mutex_lock(&mutex_current);
	success++;
	pthread_mutex_unlock(&mutex_current);
	return 0;

}

/////************************************************** /
/// // Method that determines if numbers 1-9 only appear once in a 3x3 subsection
/////************************************************** /
int is3x3Valid(int row, int col) {
	int validityArray[SIZE] = { 0 };
	int i, j;
	for (i = row; i < row + 3; i++) {
		for (j = col; j < col + 3; j++) {
			int num = sudoku[i][j];
			if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
				pthread_mutex_lock(&mutex_done);
				done = 1;
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex_done);
				return 1;
			} else {
				validityArray[num - 1] = 1;

			}
		}
	}
	// If reached this point, 3x3 subsection is valid.
	pthread_mutex_lock(&mutex_current);
	success++;
	pthread_mutex_unlock(&mutex_current);
	return 0;

}

/////************************************************** /
// Method that determines according to the task which function to do
/////************************************************** /
void main_loop(int index) {
	// according to the row/col and the function the thread perform the task
	switch (newTesk[index]->func) {
	case IS3X3VALID:
		is3x3Valid(newTesk[index]->index->row, newTesk[index]->index->column);
		break;

	case ISCOLUMNVALID:
		isColumnValid(newTesk[index]->index->row,
				newTesk[index]->index->column);
		break;

	case ISROWVALID:
		isRowValid(newTesk[index]->index->row, newTesk[index]->index->column);
		break;
	}

}

/////************************************************** /
// The threads are sent to this function - the func check that a task is not being performed twice
/////************************************************** /
void* get_task(void* param) {
	// if done == 0 the sudoku is still valid in the checking
	while (done == 0) {
		int index;
		while (current <= TASKS) {
			pthread_mutex_lock(&mutex_current);
			if (current == 27) {
				pthread_mutex_lock(&mutex_done);
				done = 1;
				pthread_cond_signal(&cond);
				pthread_mutex_unlock(&mutex_done);
				pthread_mutex_unlock(&mutex_current);
				pthread_exit(NULL);
			} // check whether the task already done
			if (newTesk[current]->check == NOT_DONE_YET) {
				newTesk[current]->check = DONE; // mark that the task done
				index = current;
				current++;
				pthread_mutex_unlock(&mutex_current);
				main_loop(index); // in order to perform the task
			}

		}
		pthread_mutex_unlock(&mutex_current);
		pthread_exit(NULL);
	}
	return NULL;
}

void release(task **newTesk) {
	int i;
	for (i = 0; i < TASKS; i++) {
		free(newTesk[i]->index);
		free(newTesk[i]);
	}
	free(newTesk);
}

int main(int argc, char *argv[]) {
	char name[100];
	int i;
	int max_threads;
	// Checker if there is a name of file in the arguments
	if (argc != 2) {
		printf("Error\n");
		return 0;
	}
	sscanf(argv[1], "%s", name);
	// Enter the number of thread
	printf("Please Enter NUmber oF Threads:");
	scanf("%d", &max_threads);
	pthread_t threads[max_threads];

	// Build the array of 27 tasks
	buildlist();
	// read sudoku from file the load it
	load_grid(sudoku, name);
	// initialize locks
	pthread_mutex_init(&mutex_current, NULL);
	pthread_mutex_init(&mutex_done, NULL);
	// initialize cond
	pthread_cond_init(&cond, NULL);
	// Create the number of threads requested
	for (i = 0; i < max_threads; ++i) {
		pthread_create(&threads[i], NULL, get_task, NULL);
	}

	while (done == 0) {
		pthread_cond_wait(&cond, &mutex_done);
	}

	pthread_mutex_unlock(&mutex_done);

	//printf("suc %d \n", success);
	if (success == 27) {
		printf("solution is legal\n"); // sudoku is valid
	} else
		printf("solution is not legal \n"); // sudoku is not valid

	// Wait for all threads to finish
	for (int i = 0; i < max_threads; ++i) {
		pthread_join(threads[i], NULL);
	}

	// destroy mutex
	pthread_mutex_destroy(&mutex_current);
	pthread_cond_destroy(&cond);
	return 0;
}
