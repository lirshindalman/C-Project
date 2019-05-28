/*
 * test.c
 *
 *  Created on: Apr 21, 2019
 *      Author: lir
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include "threadsWithout.h"

/////************************************************** /
// Method that determines if numbers 1-9 only appear once in a col
/////************************************************** /
void *isColumnValid(void* param) {
// Confirm that parameters indicate a valid col subsection
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
//	if (row != 0 || col > 8) {
//		fprintf(stderr,
//				"Invalid row or column for col subsection! row=%d, col=%d\n",
//				row, col);
//		pthread_exit(NULL);
//	}

// Check if numbers 1-9 only appear once in the column
	int validityArray[9] = { 0 };
	int i;
	for (i = 0; i < 9; i++) {
		int num = sudoku[i][col];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			pthread_exit(NULL);
		} else {
			validityArray[num - 1] = 1;
		}
	}
// If reached this point, col subsection is valid.
	valid[18 + col] = 1;
	pthread_exit(NULL);
}

/////************************************************** /
// Method that determines if numbers 1-9 only appear once in a row
/////************************************************** /
void *isRowValid(void* param) {
// Confirm that parameters indicate a valid row subsection
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
//	if (col != 0 || row > 8) {
//		fprintf(stderr,
//				"Invalid row or column for row subsection! row=%d, col=%d\n",
//				row, col);
//		pthread_exit(NULL);
//	}

// Check if numbers 1-9 only appear once in the row
	int validityArray[9] = { 0 };
	int i;
	for (i = 0; i < 9; i++) {
		// If the corresponding index for the number is set to 1, and the number is encountered again,
		// the valid array will not be updated and the thread will exit.
		int num = sudoku[row][i];
		if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
			pthread_exit(NULL);
		} else {
			validityArray[num - 1] = 1;
		}
	}
// If reached this point, row subsection is valid.
	valid[9 + row] = 1;
	pthread_exit(NULL);
}

// Method that determines if numbers 1-9 only appear once in a 3x3 subsection
void *is3x3Valid(void* param) {
// Confirm that parameters indicate a valid 3x3 subsection
	parameters *params = (parameters*) param;
	int row = params->row;
	int col = params->column;
//	if (row > 6 || row % 3 != 0 || col > 6 || col % 3 != 0) {
//		fprintf(stderr,
//				"Invalid row or column for subsection! row=%d, col=%d\n", row,
//				col);
//		pthread_exit(NULL);
//	}
	int validityArray[9] = { 0 };
	int i, j;
	for (i = row; i < row + 3; i++) {
		for (j = col; j < col + 3; j++) {
			int num = sudoku[i][j];
			if (num < 1 || num > 9 || validityArray[num - 1] == 1) {
				pthread_exit(NULL);
			} else {
				validityArray[num - 1] = 1;
			}
		}
	}
// If reached this point, 3x3 subsection is valid.
	valid[row + col / 3] = 1; // Maps the subsection to an index in the first 8 indices of the valid array
	pthread_exit(NULL);
}

/////************************************************** /
/* Function that read grid from file and store it in an array */
/////************************************************** /
int load_grid(int sudoku[][SIZE_COL_ROW], char *filename) {
	FILE *input_file = fopen(filename, "r");
	if (input_file != NULL) {
		for (int i = 0; i < SIZE_COL_ROW; i++)
			for (int j = 0; j < SIZE_COL_ROW; j++) {
				fscanf(input_file, "%d", &sudoku[i][j]);
			}
		fclose(input_file);
		return 1;
	}
	return 0;
}

int main(int argc, char* argv[]) {
	char theNameOfTheFile[100];
	int i, j;
	pthread_t threads[NUM_THREADS];
	int threadIndex = 0;
	// Checker if there is a name of file in the arguments
	if (argc != 2) {
		printf("Error\n");
		return 0;
	}

	sscanf(argv[1], "%s", theNameOfTheFile);
	// read sudoku from file the load it
	load_grid(sudoku, theNameOfTheFile);

// Create 9 threads for 9 3x3 subsections, 9 threads for 9 columns and 9 threads for 9 rows.
// This will end up with a total of 27 threads.
	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			if (i % 3 == 0 && j % 3 == 0) {
				parameters *data = (parameters *) malloc(sizeof(parameters));
				data->row = i;
				data->column = j;
				pthread_create(&threads[threadIndex++], NULL, is3x3Valid, data); // 3x3 subsection threads
			}
			if (i == 0) {
				parameters *columnData = (parameters *) malloc(
						sizeof(parameters));
				columnData->row = i;
				columnData->column = j;
				pthread_create(&threads[threadIndex++], NULL, isColumnValid,
						columnData);	// column threads
			}
			if (j == 0) {
				parameters *rowData = (parameters *) malloc(sizeof(parameters));
				rowData->row = i;
				rowData->column = j;
				pthread_create(&threads[threadIndex++], NULL, isRowValid,
						rowData); // row threads
			}
		}
	}

	// Wait for all threads to finish
	for (i = 0; i < NUM_THREADS; i++) {
		pthread_join(threads[i], NULL);
	}

// If any of the entries in the valid array are 0, then the sudoku solution is invalid
	for (i = 0; i < NUM_THREADS; i++) {
		if (valid[i] == 0) {
			printf("solution is not legal\n");
			return EXIT_SUCCESS;
		}
	}
	printf("solution is legal\n");
	return EXIT_SUCCESS;
}
