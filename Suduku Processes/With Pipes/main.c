#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "main.h"

int readFromResultPipe(int fd) {
	int i, result = 0, tmp = 0;
	for (i = 0; i < NUMBER_OF_PROCESS; i++) {
		read(fd, &tmp, sizeof(int));
		printf("%d", result);
		if (tmp > 0)
			result++;
	}
	close(fd);
	return result;
}

void checkSudoku(int fd[2], char* theNameOfTheFile) {
// Read children answers
	if (readFromResultPipe(fd[READ]) == 3) {
		printf("\n%s  is legal\n", theNameOfTheFile);   //Sudoku is valid
	} else {
		printf("\n%s is not legal\n", theNameOfTheFile); //sudoku is not  valid
	}
}

void release(int **matrix) {
	int j;
	for (j = 0; j < SIZE_OF_MATRIX; j++) {
		free(matrix[j]);
	}
	free(matrix);
}

void readSudokuFromFile(char *theNameOfTheFile, int fd[2], int fd2[2],
		int fd3[2]) {
	int i, j, k = 0;
	FILE *file;
	int **mat;
	int array[81];

	mat = (int**) malloc(SIZE_OF_MATRIX * sizeof(int*));
	if (!mat)
		return;
	for (i = 0; i < SIZE_OF_MATRIX; i++)
		mat[i] = (int*) malloc(SIZE_OF_MATRIX * sizeof(int));
	file = fopen(theNameOfTheFile, "rt");
	if (file == NULL) {
		fprintf(stderr, "Cannot Open File '%s' \n", theNameOfTheFile);
	}

	for (i = 0; i < SIZE_OF_MATRIX; i++)
		for (j = 0; j < SIZE_OF_MATRIX; j++)
			fscanf(file, "%d", &mat[i][j]);

	fclose(file);

	for (i = 0; i < SIZE_OF_MATRIX; i++) {
		for (j = 0; j < SIZE_OF_MATRIX; j++) {
			array[k] = mat[i][j];
			k++;
		}
	}

	printf("\n");
	write(fd[WRITE], array, sizeof(array) + 1);
	write(fd2[WRITE], array, sizeof(array) + 1);
	write(fd3[WRITE], array, sizeof(array) + 1);

	release(mat);
}

void readSudokuFromUser(int fd[2], int fd2[2], int fd3[2]) {
	int i, j;
	int **mat;
	int array[81];
	int k = 0;
	mat = (int**) malloc(SIZE_OF_MATRIX * sizeof(int*));
	if (!mat)
		return;
	for (i = 0; i < SIZE_OF_MATRIX; i++)
		mat[i] = (int*) malloc(SIZE_OF_MATRIX * sizeof(int));

	for (i = 0; i < SIZE_OF_MATRIX; ++i) {
		printf("\nEnter row number %d (num by num):\n", i + 1);

		for (j = 0; j < SIZE_OF_MATRIX; ++j) {
			scanf("%d", &mat[i][j]);
		}
	}

	for (i = 0; i < SIZE_OF_MATRIX; i++) {
		for (j = 0; j < SIZE_OF_MATRIX; j++) {
			array[k] = mat[i][j];
			k++;
		}
	}


	printf("\n");
	write(fd[WRITE], array, sizeof(array) + 1);
	write(fd2[WRITE], array, sizeof(array) + 1);
	write(fd3[WRITE], array, sizeof(array) + 1);

	release(mat);
}

void forking(char *theNameOfTheFile, int rows[2], int cols[2], int sub[2],
		int result[2]) {
	char * execFile = "./ofir";
	int p;
	int cid1, cid2, cid3;
	cid1 = fork();
	if (cid1 < 0) {
		perror("FORK ERROR"); // the fork did not succeed
		exit(EXIT_FAILURE);
	}
	if (cid1 == 0) {
		//child code
		//	printf("chaild1");
		close(rows[WRITE]);
		char* str_pd1 = (char*) malloc(sizeof(char) * 2);
		char* fdWrite = (char*) malloc(sizeof(char) * 2);
		sprintf(str_pd1, "%d", rows[READ]);
		sprintf(fdWrite, "%d", result[WRITE]);
		char* args[] = { execFile, "rows", str_pd1, fdWrite, NULL };
		execvp("./ofir", args);
		perror("error");
		exit(6);
	}

	cid2 = fork();

	if (cid2 < 0) {
		perror("FORK ERROR"); // the fork did not succeed
		exit(EXIT_FAILURE);
	}
	if (cid2 == 0) {
		//child code
		//	printf("chaild2");
		close(cols[WRITE]);
		char* str_pd2 = (char*) malloc(sizeof(char) * 2);
		char* fdWrite = (char*) malloc(sizeof(char) * 2);
		sprintf(str_pd2, "%d", cols[READ]);
		sprintf(fdWrite, "%d", result[WRITE]);
		char* args[] = { execFile, "cols", str_pd2, fdWrite, NULL };
		execvp("./ofir", args);
		perror("error");

		exit(6);
	}

	cid3 = fork();

	if (cid3 < 0) {
		perror("FORK ERROR"); // the fork did not succeed
		exit(EXIT_FAILURE);
	}
	if (cid3 == 0) {
		//child code
		//	printf("chaild3");

		close(sub[WRITE]);
		char* str_pd2 = (char*) malloc(sizeof(char) * 2);
		char* fdWrite = (char*) malloc(sizeof(char) * 2);
		sprintf(str_pd2, "%d", sub[READ]);
		sprintf(fdWrite, "%d", result[WRITE]);
		char* args[] = { execFile, "sub", str_pd2, fdWrite, NULL };
		execvp("./ofir", args);
		perror("error");
		exit(6);
	}

	p = wait(NULL);
	while (p != -1) {
		p = wait(NULL);
	}
	checkSudoku(result, theNameOfTheFile);

	//exit(7);

}
int main(int argc, char *argv[]) {

	char* theNameOfTheFile;
	int rows[2], cols[2], sub[2], result[2];
//	char * execFile = "./ofir";
//	int p;
//	int cid1, cid2, cid3;
	// Init pipes
	if (pipe(rows) == -1 || pipe(cols) == -1 || pipe(sub) == -1
			|| pipe(result) == -1) {
		//perror("pipe");
		fprintf(stderr, "pipe failed");
		exit(EXIT_FAILURE);
	}
	if (argc < 2) {
		readSudokuFromUser(rows, cols, sub);
		theNameOfTheFile = "No file";
		forking(theNameOfTheFile, rows, cols, sub, result);
	} else {
		// read from program arguments
		for (int argvi = 1; argvi < argc; argvi++) {
			theNameOfTheFile = argv[argvi];
			readSudokuFromFile(theNameOfTheFile, rows, cols, sub);
			forking(theNameOfTheFile, rows, cols, sub, result);
			//checkSudoku(result, theNameOfTheFile);
		}
	}
//	cid1 = fork();
//	if (cid1 < 0) {
//		perror("FORK ERROR"); // the fork did not succeed
//		exit(EXIT_FAILURE);
//	}
//	if (cid1 == 0) {
//		//child code
//	//	printf("chaild1");
//		close(rows[WRITE]);
//		char* str_pd1 = (char*) malloc(sizeof(char) * 2);
//		char* fdWrite = (char*) malloc(sizeof(char) * 2);
//		sprintf(str_pd1, "%d", rows[READ]);
//		sprintf(fdWrite, "%d", result[WRITE]);
//		char* args[] = {execFile, "rows", str_pd1, fdWrite, NULL };
//		execvp(args[0], args);
//		perror("error");
//		exit(6);
//	}
//
//	cid2 = fork();
//
//	if (cid2 < 0) {
//		perror("FORK ERROR"); // the fork did not succeed
//		exit(EXIT_FAILURE);
//	}
//	if (cid2 == 0) {
//		//child code
//	//	printf("chaild2");
//		close(cols[WRITE]);
//		char* str_pd2 = (char*) malloc(sizeof(char) * 2);
//		char* fdWrite = (char*) malloc(sizeof(char) * 2);
//		sprintf(str_pd2, "%d", cols[READ]);
//		sprintf(fdWrite, "%d", result[WRITE]);
//		char* args[] = { execFile, "cols", str_pd2, fdWrite, NULL };
//		execvp(args[0], args);
//		perror("error");
//
//		exit(6);
//	}
//
//	cid3 = fork();
//
//	if (cid3 < 0) {
//		perror("FORK ERROR"); // the fork did not succeed
//		exit(EXIT_FAILURE);
//	}
//	if (cid3 == 0) {
//		//child code
//	//	printf("chaild3");
//
//		close(sub[WRITE]);
//		char* str_pd2 = (char*) malloc(sizeof(char) * 2);
//		char* fdWrite = (char*) malloc(sizeof(char) * 2);
//		sprintf(str_pd2, "%d", sub[READ]);
//		sprintf(fdWrite, "%d", result[WRITE]);
//		char* args[] = { execFile, "sub", str_pd2, fdWrite, NULL };
//		execvp(args[0], args);
//		perror("error");
//		exit(6);
//	}
//
//	p = wait(NULL);
//	while (p != -1) {
//		p = wait(NULL);
//	}
//	checkSudoku(result, theNameOfTheFile);
//
//	exit(7);

}
