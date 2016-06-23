/* bf.h
 * Copyright (c) Mike Dupuis 2016
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

#define PROGRAM_SIZE    65535
#define DATA_SIZE       65535
#define OUTPUT_SIZE      8192
#define FILE_NAME_SIZE   1024

typedef enum _VALUE_CHANGE_DIRECTIONS {
	INCREMENT,
	DECREMENT
} VALUE_CHANGE_DIRECTIONS;

typedef enum _RESULT {
	SUCCESS,
	FAILURE
} RESULT;

typedef struct _GBL_STR {
	bool debug;
	char program[PROGRAM_SIZE];
	char output[OUTPUT_SIZE];
	unsigned long program_length;
	char file_name[FILE_NAME_SIZE];
} GBL_STR;
