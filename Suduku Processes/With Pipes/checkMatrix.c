#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "checkMatrix.h"

static int outCome = 1;

int main(int argc, char *argv[]) {
	int array[81];
	int sudoku[COL_ROW_SIZE][COL_ROW_SIZE];
	int fdRead = atoi(argv[2]);
	int fdWrite = atoi(argv[3]);

	read(fdRead, array, sizeof(int) * 81);

	int t;
	for (t = 0; t < 81; t++) {
		printf("%d", array[t]);
	}

	close(fdRead);

	int i, j;
	int k = 0;
	for (i = 0; i < COL_ROW_SIZE; i++) {
		for (j = 0; j < COL_ROW_SIZE; j++) {
			sudoku[i][j] = array[k];
			k++;
		}
	}

	for (i = 0; i < 9; i++) {
		for (j = 0; j < 9; j++) {
			printf("%d", sudoku[i][j]);
		}
		printf("\n");

	}

// Select the appropriate method to execute
	if (strcmp(argv[1], "rows") == 0) {
		isRowValid(sudoku);

	} else if (strcmp(argv[1], "cols") == 0) {
		isColumnValid(sudoku);

	} else if (strcmp(argv[1], "sub") == 0) {
		is3x3Valid(sudoku);

	} else {
		exit(1);
	}

	// write result to pipe
	write(fdWrite, &outCome, sizeof(int));
	close(fdWrite);

	return 0;
}

/////**************************************************************** /
// Method that determines if numbers 1-9 only appear once in a row
/////*********************************************************** ***** /
void isRowValid(int sudoku[COL_ROW_SIZE][COL_ROW_SIZE]) {
	// Check if numbers 1-9 only appear once in the row
	int validityArray[COL_ROW_SIZE] = { 0 };
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
}

/////************************************************** /
// Method that determines if numbers 1-9 only appear once in a column
/////************************************************** /
void isColumnValid(int sudoku[COL_ROW_SIZE][COL_ROW_SIZE]) {
	// Check if numbers 1-9 only appear once in the row
	int validityArray[COL_ROW_SIZE] = { 0 };
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

}

/////************************************************** /
// Method that determines if numbers 1-9 only appear once in a 3x3 subsection
/////************************************************** /
void is3x3Valid(int sudoku[COL_ROW_SIZE][COL_ROW_SIZE]) {
	// Check if numbers 1-9 only appear once in the row
	int validityArray[COL_ROW_SIZE] = { 0 };
	int x_offset = 0;
	int y_offset = 0;
	while (y_offset <= 6) {
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				if (sudoku[i + y_offset][j + x_offset] < 10
						&& sudoku[i + y_offset][j + x_offset] > 0) {
					++validityArray[sudoku[i + y_offset][j + x_offset] - 1];
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
		for (int k = 0; k < COL_ROW_SIZE; ++k) {
			if (validityArray[k] != 1) {
				outCome = 0;
				break;
			}
			validityArray[k] = 0;
		}
	}
}
