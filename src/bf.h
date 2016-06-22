#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#define PROGRAM_SIZE    4096
#define DATA_SIZE       65535

typedef enum _BF_RESULT {
	BF_R_SUCCESS,
	BF_R_FAILURE
} BF_RESULT;
