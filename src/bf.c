/* BF in C (http://github.com/zabertooth/cbrainfuck)
 * Copyright (c) 2016 Mike Dupuis
 */

#include "bf.h"

GBL_STR gbl = {0};

static void bf_printf(char *fmt, ...) {
	if (gbl.debug) {
		va_list args;
		va_start(args, fmt);
		vprintf(fmt, args);
		va_end(args);
	}
}

static RESULT read_bf(void)
{
	RESULT result = FAILURE;
	FILE *fp = NULL;
	char c;

	/* Open the source file */
	if (NULL == (fp = fopen(gbl.file_name, "r"))) {
		fprintf(stderr, "ERROR: Failed to open source file '%s'\n", gbl.file_name);
		goto END;
	}

	/* Clear the program's memory */
	memset(gbl.program, 0, PROGRAM_SIZE);
	gbl.program_length = 0;

	/* Copy the input to the program's memory */
	while (EOF != (c = (char)getc(fp))) {
		if (gbl.program_length == PROGRAM_SIZE) {
			fprintf(stderr, "ERROR: Program exceeds maximum size %d\n", PROGRAM_SIZE);
			goto END;
		}

		gbl.program[gbl.program_length] = c;
		gbl.program_length++;
	}

	result = SUCCESS;

END:
	if (NULL != fp)
		fclose(fp);

	return result;
}

// Safely increment an index
static RESULT s_increment_index(unsigned int *index, unsigned int maximum)
{
	bf_printf(" s_increment_index incrementing data at address %x from %d to %d\n", index, *index, *index + 1);
	(*index)++;

	return (*index > maximum) ? FAILURE : SUCCESS;
}

// Safely decrement an index
static RESULT s_decrement_index(unsigned int *index)
{
	bf_printf(" s_decrement_index decrementing data at address %x from %d to %d\n", index, *index, *index - 1);

	// Cannot decrement beyond 0
	if (*index == 0)
		return FAILURE;

	(*index)--;
	return SUCCESS;
}

#define MOVE_INSTRUCTION_POINTER_ONE(DIRECTION) \
	do { \
		switch (DIRECTION) { \
			case DECREMENT: \
				if (instruction_index == 0) { \
					fprintf(stderr, "Error: Instruction pointer out-of-bounds while decrementing\n"); \
					return FAILURE; \
				} \
				instruction_index--; \
				break; \
			case INCREMENT: \
				if (instruction_index >= gbl.program_length) { \
					fprintf(stderr, "Error: Instruction pointer out-of-bounds while incrementing\n"); \
					return FAILURE; \
				} \
				instruction_index++; \
				break; \
			default: \
				fprintf(stderr, "Error: Invalid pointer movement direction %d\n", DIRECTION); \
				return FAILURE; \
		} \
	} while (0)

#define INCREMENT_INSTRUCTION_POINTER() MOVE_INSTRUCTION_POINTER_ONE(INCREMENT)
#define DECREMENT_INSTRUCTION_POINTER() MOVE_INSTRUCTION_POINTER_ONE(DECREMENT)
static RESULT run_bf(void)
{
	char data[DATA_SIZE] = {0};
	unsigned int data_index = 0;
	char data_string[2] = {0};
	unsigned int instruction_index = 0;

	char instruction;
	while ('\0' != (instruction = gbl.program[instruction_index])) {
#if 0
		if (gbl.debug)
			sleep(1);
#endif
		switch (instruction) { // Comments describe the language specification
			case '+': // Increment the value of the data pointer
				bf_printf("Processing instruction %c\n", instruction);
				bf_printf(" incrementing data at index %d from %d to %d\n", data_index, data[data_index], data[data_index] + 1);
				data[data_index]++; 
				break;

			case '-': // Decrement the value of the data pointer
				bf_printf("Processing instruction %c\n", instruction);
				bf_printf(" decrementing data at index %d from %d to %d\n", data_index, data[data_index], data[data_index] - 1);
				data[data_index]--;
				break;

			case '.': // Print the value of the data pointer as a character
				bf_printf("Processing instruction c\n", instruction);
				if (gbl.debug) {
					memset(data_string, 0, 2);
					snprintf(data_string, 2, "%c", data[data_index]);
					strncat(gbl.output, data_string, 1024);
				}
				else {
					fprintf(stdout, "%c", data[data_index]);
				}
				break;

			case ',': // Accept one byte of input and store it in the data pointer
				bf_printf("Processing instruction c\n", instruction);
				data[data_index] = (char)getchar();
				break;

			case '>': // Increment the data pointer to the next address
				bf_printf("Processing instruction c\n", instruction);
				if (SUCCESS != s_increment_index(&data_index, DATA_SIZE)) {
					fprintf(stderr, "ERROR: Data out-of-bounds while incrementing\n");
					return FAILURE;
				}
				break;

			case '<': // Decrement the data pointer to the previous address
				bf_printf("Processing instruction c\n", instruction);
				if (SUCCESS != s_decrement_index(&data_index)) {
					fprintf(stderr, "ERROR: Data out-of-bounds while decrementing\n");
					return FAILURE;
				}
				break;

			case '[': // If the value of the data pointer is 0, seek the corresponding ] instruction
				bf_printf("Processing instruction c\n", instruction);
				if (data[data_index] == 0) {
					unsigned int loop_depth = 1; // The number of ]s to find
					while (loop_depth > 0) {
						INCREMENT_INSTRUCTION_POINTER();
						
						if (gbl.program[instruction_index] == '[')
							loop_depth++;

						if (gbl.program[instruction_index] == ']')
							loop_depth--;
					}
				}

				// NOTE: at this instant, the instruction pointer will point to the ] instruction we sought out
				//   the end of the loop body will handle advancing the pointer to the next instruction
				break;

			case ']': // If the value of the data pointer is not 0, seek the corresponding ] instruction
				bf_printf("Processing instruction c\n", instruction);
				if (data[data_index] != 0) {
					unsigned int loop_depth = 1; // The number of [s to find
					while (loop_depth > 0) {
						DECREMENT_INSTRUCTION_POINTER();
						
						if (gbl.program[instruction_index] == '[')
							loop_depth--;

						if (gbl.program[instruction_index] == ']')
							loop_depth++;
					}
				}

				// NOTE: at this instant, the instruction pointer will point to the ] instruction we sought out
				//   the end of the loop body will handle advancing the pointer to the next instruction
				break;

			default:
				bf_printf("Skipping non-instruction c\n", instruction);
				break;

		}

		INCREMENT_INSTRUCTION_POINTER();
	}

	// Print a newline to flush the stdout buffer.
	printf("\n");

	return SUCCESS;
}

static void usage(const char *app_name)
{
	fprintf(stderr, "Usage: %s [-d] filename\n", app_name);
	fprintf(stderr, "  -d    gbl.debug\n");
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
			gbl.debug = 1;
		}

		else {
			if (0 != strcmp("", gbl.file_name)) {
				usage(argv[0]);
				exit(EXIT_FAILURE);
			}
			snprintf(gbl.file_name, 1024, "%s", current_arg);
		}
	}

	if (0 == strcmp(gbl.file_name, "")) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}
}

int main(int argc, const char **argv)
{
	RESULT status;

	/* Parse the command line arguments */
	parse_args(argc, argv);

	/* Process the source file */
	bf_printf("reading bf source file\n");
	if (SUCCESS != (status = read_bf())) {
		fprintf(stderr, "Failed to read instructions, exiting!\n");
		exit(EXIT_FAILURE);
	}

	/* Run the program */
	bf_printf("running bf program\n");
	if (SUCCESS != (status = run_bf())) {
		fprintf(stderr, "Failed to run program, exiting!\n");
		exit(EXIT_FAILURE);
	}

	if (gbl.debug)
		printf("%s\n", gbl.output);

	return status;
}
