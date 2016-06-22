/* BF in C (http://github.com/zabertooth/brainfuck-c)
 * Copyright (c) 2016 Mike Dupuis
 */

#include "bf.h"

static char program[PROGRAM_SIZE];
static char output[1024];
static unsigned int program_length;
static char debug;
static char bf_file_name[1024] = {0};

static void bfprintf(char *fmt, ...) {
	if (debug) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}

static BF_RESULT read_bf(void)
{
	BF_RESULT result = BF_R_FAILURE;
	FILE *fp = NULL;
	char c;

	/* Open the source file */
	if (NULL == (fp = fopen(bf_file_name, "r"))) {
		fprintf(stderr, "ERROR: Failed to open source file '%s'\n", bf_file_name);
		goto END;
	}

	/* Clear the program's memory */
	memset(program, 0, PROGRAM_SIZE);
	program_length = 0;

	/* Copy the input to the program's memory */
	while (EOF != (c = (char)getc(fp))) {
		if (program_length == PROGRAM_SIZE) {
			fprintf(stderr, "ERROR: Program exceeds maximum size %d\n", PROGRAM_SIZE);
			goto END;
		}

		program[program_length] = c;
		program_length++;
	}

	result = BF_R_SUCCESS;

END:
	if (NULL != fp)
		fclose(fp);

	return result;
}

static BF_RESULT s_increment_index(unsigned int *index, unsigned int maximum)
{
	bfprintf("s_increment_index incrementing data at address %x from %d to %d\n", index, *index, *index + 1);
	(*index)++;

	return (*index > maximum) ? BF_R_FAILURE : BF_R_SUCCESS;
}

static BF_RESULT s_decrement_index(unsigned int *index, unsigned int minimum)
{
	(*index)--;

	return (*index < minimum) ? BF_R_FAILURE : BF_R_SUCCESS;
}

static BF_RESULT run_bf(void)
{
	unsigned short data[DATA_SIZE] = {0};
	unsigned int data_index = 0;
	char data_string[2];
	unsigned int instruction_index = 0;
	char c;

	for (instruction_index = 0; instruction_index < program_length; instruction_index++) {
		if (debug)
			sleep(1);
		c = program[instruction_index];
		bfprintf("Processing instruction %c\n", c);
		switch (c) {
			case '+': 
				bfprintf(" incrementing data at index %d from %d to %d\n", data_index, data[data_index], data[data_index] + 1);
				data[data_index]++; 
				break;

			case '-':
				bfprintf(" decrementing data at index %d from %d to %d\n", data_index, data[data_index], data[data_index] - 1);
				data[data_index]--;
				break;

			case '.':
				if (debug) {
					memset(data_string, 0, 2);
					snprintf(data_string, 2, "%c", data[data_index]);
					strncat(output, data_string, 1024);
				}
				else {
					putchar(data[data_index]);
				}
				break;

			case ',':
				data[data_index] = (char)getchar();
				break;

			case '>':
				if (BF_R_SUCCESS != s_increment_index(&data_index, DATA_SIZE)) {
					fprintf(stderr, "ERROR: Data out-of-bounds while incrementing\n");
					return BF_R_FAILURE;
				}
				break;

			case '<':
				if (BF_R_SUCCESS != s_decrement_index(&data_index, 0)) {
					fprintf(stderr, "ERROR: Data out-of-bounds while decrementing\n");
					return BF_R_FAILURE;
				}
				break;

			case '[':
				if (data[data_index] == 0) { /* If the loop is done, advance to the matching ] instruction */
					unsigned int loop_depth = 1; /* The number of ]s to find */
					while (loop_depth > 0) {
						if (BF_R_SUCCESS != s_increment_index(&instruction_index, PROGRAM_SIZE)) {
							fprintf(stderr, "Instruction: Data out-of-bounds while incrementing\n");
							return BF_R_FAILURE;
						}
						
						if (program[instruction_index] == '[')
							loop_depth++;

						if (program[instruction_index] == ']')
							loop_depth--;
					}
					
					// Advance to the instruction BEYOND the ']'
					if (BF_R_SUCCESS != s_increment_index(&instruction_index, PROGRAM_SIZE)) {
						fprintf(stderr, "Instruction: Data out-of-bounds while incrementing\n");
						return BF_R_FAILURE;
					}
				}
				break;

			case ']':
				// Return to the matching [ instruction
				{
					unsigned int loop_depth = 1; // The number of [s to find
					while (loop_depth > 0) {
						if (BF_R_SUCCESS != s_decrement_index(&instruction_index, 0)) {
							fprintf(stderr, "Instruction: Data out-of-bounds while decrementing\n");
							return BF_R_FAILURE;
						}
						
						if (program[instruction_index] == '[')
							loop_depth--;

						if (program[instruction_index] == ']')
							loop_depth++;
					}
				}

				// Since we want to process the [ instruction on the next iteration, return to one element BEFORE that instruction because the loop iterator increments instruction_index
				instruction_index--; // do this without regard for safety because the loop iterator increments this value
				break;

		}
	}

	return BF_R_SUCCESS;
}

static void usage(const char *app_name)
{
	fprintf(stderr, "Usage: %s filename\n", app_name);
}

static void parse_args(int argc, const char **argv)
{
	int current_arg_index = 0;
	const char *current_arg = NULL;

	if (argc < 2) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	for (current_arg_index = 1; current_arg_index < argc; current_arg_index++) {
		current_arg = argv[current_arg_index];
		if (0 == strcmp(current_arg, "-d")) {
			debug = 1;
		}

		else {
			if (0 != strcmp("", bf_file_name)) {
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			snprintf(bf_file_name, 1024, current_arg);
		}
	}

	if (0 == strcmp(bf_file_name, "")) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, const char **argv)
{
	BF_RESULT status;

	/* Parse the command line arguments */
	parse_args(argc, argv);

	/* Process the source file */
	bfprintf("reading bf source file\n");
	if (BF_R_SUCCESS != (status = read_bf())) {
		fprintf(stderr, "Failed to read instructions, exiting!");
		exit(EXIT_FAILURE);
	}

	/* Run the program */
	bfprintf("running bf program\n");
	if (BF_R_SUCCESS != (status = run_bf())) {
		fprintf(stderr, "Failed to run program, exiting!");
		exit(EXIT_FAILURE);
	}

	if (debug)
		printf(output);

	return status;
}
