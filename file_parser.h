/*
 * file_parser.h
 *
 *  Created on: Oct 3, 2011
 *      Author: nayef
 */

#include "hash_table.h"

#ifndef FILE_PARSER_H_
#define FILE_PARSER_H_

#define MAX_LINE_LENGTH 256

void parse_file(FILE *fptr, int pass, char *instructions[], size_t inst_len, hash_table_t *hash_table, FILE *Out);
int binarySearch(char *instructions[], int low, int high, char *string);
char instruction_type(char *instruction);
char *register_address(char *registerName);
void rtype_instruction(char *instruction, char *rs, char *rt, char *rd, int shamt, FILE *Out);
void itype_instruction(char *instruction, char *rs, char *rt, int immediate, FILE *Out);
void jtype_instruction(char *instruction, int immediate, FILE *Out);
void word_rep(int binary_rep, FILE *Out);
void ascii_rep(char string[], FILE *Out);
void getBin(int num, char *str, int padding);
int getDec(char *bin);

#endif /* FILE_PARSER_H_ */
