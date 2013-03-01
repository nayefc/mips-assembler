/*
 * assembler.c
 *
 *  Created on: Oct 3, 2011
 *      Author: nayef
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "file_parser.h"
#include "hash_table.h"

int search(char *instruction);

// Array that holds the supported instructions
char *instructions[] = {
		"la",	// 0
		"lui",	// 1
		"lw",	// 2
		"sw",	// 3
		"add",	// 4
		"sub",	// 5
		"addi",	// 6
		"or",	// 7
		"and",	// 8
		"ori",	// 9
		"andi",	// 10
		"slt",	// 11
		"slti",	// 12
		"sll",	// 13
		"srl",	// 14
		"beq",	// 15
		"j",	// 16
		"jr",	// 17
		"jal"	// 18
	};

// Size of array
size_t inst_len = sizeof(instructions)/sizeof(char *);

int search(char *instruction) {

	int found = 0;

	for (int i = 0; i < inst_len; i++) {

		if (strcmp(instruction, instructions[i]) == 0) {
			found = 1;
			return i;
		}
	}

	if (found == 0)
		return -1;
}

// Quick Sort String Comparison Function
int string_comp(const void *a, const void *b) {
	return strcmp(*(char **)a, *(char **)b);
}

int main (int argc, char *argv[]) {

	// Make sure correct number of arguments input
	if (argc != 3) {
		printf("Incorrect number of arguments");
	}

	else {

		// Open I/O files
		// Check that files opened properly
		FILE *In;
		In = fopen(argv[1], "r");
		if (In == NULL) {
			printf("Input file could not be opened.");
			exit(1);
		}

		FILE *Out;
		Out = fopen(argv[2], "w");
		if (Out == NULL) {
			printf("Output file could not opened.");
			exit(1);
		}

		// Sort the array using qsort for faster search
		qsort(instructions, inst_len, sizeof(char *), string_comp);

		// Create a hash table of size 127
		hash_table_t *hash_table = create_hash_table(127);

		// Parse in passes

		int passNumber = 1;
		parse_file(In, passNumber, instructions, inst_len, hash_table, Out);

		// Rewind input file & start pass 2
		rewind(In);
		passNumber = 2;
		parse_file(In, passNumber, instructions, inst_len, hash_table, Out);

		// Close files
		fclose(In);
		fclose(Out);

		return 0;
	}
}
