#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include "sudoku.h"

int main(int argc, char *argv[]) {
	pid_t process[NUMBER_OF_PROCESS]; // Init 3 process
	char* theNameOfTheFile;
	SHARED *shared_struct_obj;
	if (argc < 2) {
		shared_struct_obj = mmap(NULL, COL_ROW_SIZE,
		PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
		if (shared_struct_obj == MAP_FAILED) {
			perror("Mmap failed\n");
			exit(EXIT_FAILURE);
		}
		// enter manually the sudoku
		readSudokuFromUser(shared_struct_obj); // get sudoku from user
		forking(process, shared_struct_obj); // forking
		waitForChildren();
		checkSudoku(shared_struct_obj, "no file-"); /// final decision if the sudoku is valid or not
		munmap(shared_struct_obj, COL_ROW_SIZE);
	} else {
		// read from program arguments
		for (int argvi = 1; argvi < argc; argvi++) {
			shared_struct_obj = mmap(NULL, COL_ROW_SIZE,
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS, -1, 0);
			if (shared_struct_obj == MAP_FAILED) {
				perror("Mmap failed\n");
				exit(EXIT_FAILURE);
			}
			theNameOfTheFile = argv[argvi];
			readSudokuFromFile(theNameOfTheFile, shared_struct_obj); // read sudoku from file
			forking(process, shared_struct_obj); // forking
			waitForChildren();
			checkSudoku(shared_struct_obj, theNameOfTheFile); /// final decision if the sudoku is valid or not
			munmap(shared_struct_obj, COL_ROW_SIZE);
		}

	}
	return 0;
}

void forking(pid_t *process, SHARED *shared_struct_obj) {
	for (int i = 0; i < NUMBER_OF_PROCESS; i++) {
		process[i] = fork();
		if (process[i] == 0) { // init each process
			initiateProcess(i, shared_struct_obj);

		} else if (process[i] < 0) {
			perror("FORK ERROR"); // the fork did not succeed
			exit(EXIT_FAILURE);

		} else {
			continue;
		}
	}
}

/////************************************************** /
/// initiate each process
/////************************************************** /
void initiateProcess(int i, SHARED *shared_struct_obj) {
	int** matrix = (int**) malloc(COL_ROW_SIZE * sizeof(int*));
	int result = 0;
	if (!matrix)
		fprintf(stderr, "allocation faild");
	// copy sudoku to shared memory
	memcpy(matrix, shared_struct_obj->solution, SOLUTION_SIZE);

	// Choose the appropriate method to execute
	switch (i) {
	case 0:
		result = isRowValid(matrix); // checking rows
		break;
	case 1:
		result = isColumnValid(matrix); // Checking columns
		break;
	default:
		result = is3x3Valid(matrix); // Checking sub matrix
	}

	// copy result to shared memory
	shared_struct_obj->status[i] = result;
	exit(EXIT_SUCCESS);
}

/////************************************************** /
/// Read sudoku table from file and write matrix
/////************************************************** /
void readSudokuFromFile(char *theNameOfTheFile, SHARED *shared_struct_obj) {
	int i, j;
	FILE *file;
	int** matrix = (int**) malloc(COL_ROW_SIZE * sizeof(int*));
	if (!matrix)
		fprintf(stderr, "allocation faild");
	for (i = 0; i < COL_ROW_SIZE; i++)
		matrix[i] = (int*) malloc(COL_ROW_SIZE * sizeof(int));
	file = fopen(theNameOfTheFile, "r");
	if (file == NULL) { // error open file
		fprintf(stderr, "Cannot Open File '%s' \n", theNameOfTheFile);
	}

	for (i = 0; i < COL_ROW_SIZE; ++i) {
//		matrix[i] = (int*) malloc(COL_ROW_SIZE * sizeof(int));
//		if (!matrix[i])
//			fprintf(stderr, "allocation faild");
		for (j = 0; j < COL_ROW_SIZE; ++j) {
			fscanf(file, "%d", &matrix[i][j]);
		}
	}

	shared_struct_obj->solution = (int**) malloc(COL_ROW_SIZE * sizeof(int*));
	if (!shared_struct_obj->solution)
		fprintf(stderr, "allocation faild");
	for (i = 0; i < COL_ROW_SIZE; i++) {
		shared_struct_obj->solution[i] = (int*) malloc(
		COL_ROW_SIZE * sizeof(int));
		if (!shared_struct_obj->solution[i])
			fprintf(stderr, "allocation faild");
		for (j = 0; j < COL_ROW_SIZE; j++) {
			shared_struct_obj->solution[i][j] = matrix[i][j];
		}
	}

	//memcpy(shared_struct_obj->solution, matrix, SOLUTION_SIZE);
	//cpyMat(matrix, shared_struct_obj->solution);
	fclose(file);
	freeMatrix(matrix);
}

/////************************************************** /
///Read sudoku table from user and write matrix
/////************************************************** /
void readSudokuFromUser(SHARED *shared_struct_obj) {
	int i, j;
	int** matrix = (int**) malloc(COL_ROW_SIZE * sizeof(int*));
	if (!matrix)
		fprintf(stderr, "allocation faild");

	for (i = 0; i < COL_ROW_SIZE; ++i) {
		printf("\nEnter row number %d (num by num):\n", i + 1);
		matrix[i] = (int*) malloc(COL_ROW_SIZE * sizeof(int));
		if (!matrix[i])
			fprintf(stderr, "allocation faild");
		for (j = 0; j < COL_ROW_SIZE; ++j) {
			scanf("%d", &matrix[i][j]);
		}
	}

	shared_struct_obj->solution = (int**) malloc(COL_ROW_SIZE * sizeof(int*));
	if (!shared_struct_obj->solution)
		fprintf(stderr, "allocation faild");
	for (i = 0; i < COL_ROW_SIZE; i++) {
		shared_struct_obj->solution[i] = (int*) malloc(
		COL_ROW_SIZE * sizeof(int));
		if (!shared_struct_obj->solution[i])
			fprintf(stderr, "allocation faild");
		for (j = 0; j < COL_ROW_SIZE; j++) {
			shared_struct_obj->solution[i][j] = matrix[i][j];
		}
	}
	//memcpy(shared_struct_obj->solution, matrix, SOLUTION_SIZE);
	//cpyMat(matrix, shared_struct_obj->solution);
	freeMatrix(matrix);
}


void cpyMat(int** from, int** to) {
	int i, j;
	to = (int**) malloc(COL_ROW_SIZE * sizeof(int*));
	if (!to)
		fprintf(stderr, "allocation faild");

	for (i = 0; i < COL_ROW_SIZE; i++) {
		to[i] = (int*) malloc(
		COL_ROW_SIZE * sizeof(int));
		if (!to[i])
			fprintf(stderr, "allocation faild");
		for (j = 0; j < COL_ROW_SIZE; j++) {
			to[i][j] = from[i][j];
		}
	}
}

/////************************************************** /
/// print matrix
/////************************************************** /
void printMat(int** mat) {
	int i, j;
	printf("\n");
	for (i = 0; i < COL_ROW_SIZE; i++) {
		for (j = 0; j < COL_ROW_SIZE; j++) {
			printf("%d ", mat[i][j]);
		}
		printf("\n");
	}
}


/////************************************************** /
/// waiting to all children
/////************************************************** /
void waitForChildren() {
	for (int i = 0; i < NUMBER_OF_PROCESS; i++) {
		wait(NULL);
	}
}

/////************************************************** /
/// checking the final result - the the sudoku is valid or not
/////************************************************** /
void checkSudoku(SHARED *shared_struct_obj, char* theNameOfTheFile) {
	int payOff = 0;
	for (int i = 0; i < NUMBER_OF_PROCESS; ++i) {
		payOff += shared_struct_obj->status[i];
	}

	if (payOff == 3) {
		printf("\n%s is legal\n", theNameOfTheFile); // the sudoku is valid
	} else {
		printf("\n%s is not legal\n", theNameOfTheFile); // the sudoku is not valid
	}
	freeMatrix(shared_struct_obj->solution);
}

/////************************************************** /
/// free 2d array
/////************************************************** /
void freeMatrix(int** matrix) {
	int i;
	for (i = 0; i < COL_ROW_SIZE; i++) {
		free(matrix[i]);
	}
	free(matrix);
}

/////************************************************** /
// Method that determines if numbers 1-9 only appear once in a 3x3 subsection
/////************************************************** /
int is3x3Valid(int** sudoku) {
	int outCome = 1;
	int i, j, k;
// Check if numbers 1-9 only appear once in the row
	int validityArray[COL_ROW_SIZE] = { 0 };
	int x_offset = 0;
	int y_offset = 0;
	while (y_offset <= 6) {
		for (i = 0; i < 3; ++i) {
			for (j = 0; j < 3; ++j) {
				if (sudoku[i + y_offset][j + x_offset] < 10
						&& sudoku[i + y_offset][j + x_offset] > 0) {
					validityArray[sudoku[i + y_offset][j + x_offset] - 1] = 1;
				} else {
					outCome = 0;
					break;
				}
			}
		}
		x_offset += 3;
		if (x_offset > 6) {
			y_offset += 3;
			x_offset = 0;
		}
		// If after iterating through each subMat we find any index in the validityArray that it's not 1 we disqualify the sudoku.
		for (k = 0; k < COL_ROW_SIZE; ++k) {
			if (validityArray[k] != 1) {
				outCome = 0;
				break;
			}
			validityArray[k] = 0;
		}
	}
	return outCome;
}

/////**************************************************************** /
// Method that determines if numbers 1-9 only appear once in a row
/////*********************************************************** ***** /
int isRowValid(int** sudoku) {
	// Check if numbers 1-9 only appear once in the row
	int validityArray[COL_ROW_SIZE] = { 0 };
	int outCome = 1;
	for (int i = 0; i < COL_ROW_SIZE; ++i) {
		for (int j = 0; j < COL_ROW_SIZE; ++j) {
			if (sudoku[i][j] <= 9 && sudoku[i][j] >= 1) {
				++validityArray[sudoku[i][j] - 1];
			} else {
				outCome = 0;
				break;
			}
		}
		// If after iterating through each row we find any index in the validityArray that it's not 1 we disqualify the sudoku.

		for (int k = 0; k < COL_ROW_SIZE; ++k) {
			if (validityArray[k] != 1) {
				outCome = 0;
				break;
			}
			validityArray[k] = 0;
		}
	}
	return outCome;

}

/////************************************************** /
// Method that determines if numbers 1-9 only appear once in a column
/////************************************************** /
int isColumnValid(int** sudoku) {
	int validityArray[COL_ROW_SIZE] = { 0 };
	int outCome = 1;
	for (int j = 0; j < COL_ROW_SIZE; ++j) {
		for (int i = 0; i < COL_ROW_SIZE; ++i) {
			if (sudoku[i][j] <= 9 && sudoku[i][j] >= 1) {
				++validityArray[sudoku[i][j] - 1];
			} else {
				outCome = 0;
				break;
			}
		}
		// If after iterating through each col we find any index in the validityArray that it's not 1 we disqualify the sudoku.
		for (int k = 0; k < COL_ROW_SIZE; ++k) {
			if (validityArray[k] != 1) {
				outCome = 0;
				break;
			}
			validityArray[k] = 0;
		}
	}
	return outCome;

}

