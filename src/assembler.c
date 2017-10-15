#include "assembler.h"
#include "strmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

// maximum length of a string required in a program
#define MAX_LEN (256)

// length of a SIC instruction in bytes
#define INSTRCTN_LEN_BYTES 3

// hash table for the opcode table
StrMap *optab;
const char *optab_file_path = "data/optab.txt";
const int N_OPTAB_SLOTS = 1 << 8;

// hash table for the symbol table
StrMap *symtab;
const int N_SYMTAB_SLOTS = 1 << 8;

int prog_len = 0;
char prog_name[MAX_LEN + 1];
int start_addr = 0;

char* strrev(char *str)
{
    /* skip null */
    if (str == 0) {
		return str;
    }

    /* skip empty string */
    if (*str == 0) {
		return str;
    }

    /* get range */
    char *start = str;
    char *end = start + strlen(str) - 1; /* -1 for \0 */
    char temp;

    /* reverse */
    while (end > start) {
		/* swap */
		temp = *start;
		*start = *end;
		*end = temp;

		/* move */
		++start;
		--end;
    }
	return str;
}

char *strtok_single(char *str, char const *delims)
{
	static char *src = NULL;
	char  *p,  *ret = 0;

	if (str != NULL)
		src = str;

	if (src == NULL)
		return NULL;

	if ((p = strpbrk(src, delims)) != NULL) {
		*p  = 0;
		ret = src;
		src = ++p;

	} else if (*src) {
		ret = src;
		src = NULL;
	}

	return ret;
}

// checks if a source file line is a comment
bool is_comment(char *line)
{
	return line[0] == '.';
}

void parse_src_line(char *line, char *label, char *opcode, char *operand)
{
	if (is_comment(line)) {
		strcpy(label, "");
		strcpy(opcode, "");
		strcpy(operand, "");
		return;
	}

	char line_copy[MAX_LEN + 1] = "";
	char *token = strtok_single(strcpy(line_copy, line), ":\n");
	strcpy(label, token);
	token = strtok_single(NULL, ":\n");
	strcpy(opcode, token);
	token = strtok_single(NULL, ":\n");
	strcpy(operand, token);
}

void write_inter_line(FILE *file, const char *label,
		const char *opcode, const char *operand, int addr)
{
	char line[MAX_LEN + 1] = "";
	strcat(line, label);
	strcat(line, ":");
	strcat(line, opcode);
	strcat(line, ":");
	strcat(line, operand);
	strcat(line, ":");
	sprintf(line + strlen(line), "%04X", addr);
	strcat(line, "\n");
	fputs(line, file);
}

char *parse_constant_hex(char *constant, char *obj_code)
{
	strcpy(obj_code, constant);
	return obj_code;
}

char *parse_constant_char(char *constant, char *obj_code)
{
	(void)constant;
	return obj_code;
}

char *parse_constant(char *constant, char *obj_code)
{
	char constant_cpy[strlen(constant) + 1];

	char *type = strtok_single(strcpy(constant_cpy, constant), "'");
	char *val = strtok_single(NULL, "'");

	if (!strcmp(type, "C")) {
		return parse_constant_char(val, obj_code);
	} else if (!strcmp(type, "X")) {
		return parse_constant_hex(val, obj_code);
	}

	return obj_code;
}

void pass1(FILE *input_file, FILE *inter_file)
{
	char label[MAX_LEN + 1] = "";
	char opcode[MAX_LEN + 1] = "";
	char operand[MAX_LEN + 1] = "";
	char line[MAX_LEN + 1] = "";
	fgets(line, sizeof line, input_file);
	parse_src_line(line, label, opcode, operand);

	int locctr = 0;
	int instrctn_addr;
	if (!strcmp(opcode, "START")) {
		start_addr = strtol(operand, NULL, 16);
		locctr = start_addr;
		instrctn_addr = locctr;
		strcpy(prog_name, label);

		write_inter_line(inter_file, label, opcode, operand, instrctn_addr);
		fgets(line, sizeof line, input_file);
		parse_src_line(line, label, opcode, operand);
	} else {
		locctr = 0;
	}

	while (strcmp(opcode, "END")) {
		if (!is_comment(line)) {
			instrctn_addr = locctr;
			if (strcmp(label, "")) {
				if (sm_exists(symtab, label)) {
					// duplicate symbol error
				} else {
					char locctr_str[4 + 1];
					sprintf(locctr_str, "%04X", locctr);
					sm_put(symtab, label, locctr_str);
				}
			}

			if (sm_exists(optab, opcode)) {
				locctr += 3;
			} else if (!strcmp(opcode, "WORD")) {
				locctr += 3;
			} else if (!strcmp(opcode, "RESW")) {
				int n_words = strtol(operand, NULL, 10);
				locctr += 3 * n_words;
			} else if (!strcmp(opcode, "RESB")) {
				int n_bytes = strtol(operand, NULL, 10);
				locctr += n_bytes;
			} else if (!strcmp(opcode, "BYTE")) {
				char obj_code[MAX_LEN + 1];
				locctr += strlen(parse_constant(operand, obj_code)) / 2;
			} else {
				// opcode not found
			}
			write_inter_line(inter_file, label, opcode, operand, instrctn_addr);
		} else {
			fputs(line, inter_file);
		}
		fgets(line, sizeof line, input_file);
		parse_src_line(line, label, opcode, operand);
	}
	instrctn_addr = locctr;
	write_inter_line(inter_file, label, opcode, operand, instrctn_addr);

	prog_len = locctr - start_addr;
}

void parse_inter_line(char *line,
		char *label, char *opcode, char *operand, int *addr)
{
	if (is_comment(line)) {
		strcpy(label, "");
		strcpy(opcode, "");
		strcpy(operand, "");
		return;
	}

	char line_copy[MAX_LEN + 1] = "";
	char *token = strtok_single(strcpy(line_copy, line), ":\n");
	strcpy(label, token);
	token = strtok_single(NULL, ":\n");
	strcpy(opcode, token);
	token = strtok_single(NULL, ":\n");
	strcpy(operand, token);
	token = strtok_single(NULL, ":\n");
	*addr = strtol(token, NULL, 16);
}

char *get_header_rec(char *rec)
{
	strcpy(rec, "");
	strcat(rec, "H");
	// strcat(rec, "^");
	strncat(rec, prog_name, 6);
	int len = strlen(prog_name);
	while (len < 6) {
		strcat(rec, " ");
		len++;
	}
	// strcat(rec, "^");

	sprintf(rec + strlen(rec), "%06X", start_addr);
	// strcat(rec, "^");
	sprintf(rec + strlen(rec), "%06X", prog_len);
	strcat(rec, "\n");
	return rec;
}

char *get_text_rec(char *rec, char *obj_code, int curr_addr)
{
	strcpy(rec, "");

	if (!strcmp(obj_code, "")) {
		return rec;
	}

	strcat(rec, "T");
	// strcat(rec, "^");
	strcat(rec, "00");
	sprintf(rec + strlen(rec), "%04X", curr_addr);
	// strcat(rec, "^");
	sprintf(rec + strlen(rec), "%02X", (int)strlen(obj_code) / 2);
	// strcat(rec, "^");
	strcat(rec, obj_code);
	strcat(rec, "\n");

	return rec;
}

char *get_end_rec(char *rec)
{
	strcpy(rec, "");
	strcat(rec, "E");
	// strcat(rec, "^");
	sprintf(rec + strlen(rec), "%06X", start_addr);
	strcat(rec, "\n");
	return rec;
}

int fill_remaining_mem(int curr_addr, int written_addr, FILE *output_file)
{
	char rec[MAX_LEN + 1] = "";
	char obj_code[6 + 1] = "";
	while (written_addr < curr_addr) {
		strcat(obj_code, "00");
		written_addr++;

		if (strlen(obj_code) == 6 || written_addr >= curr_addr) {
			get_text_rec(rec, obj_code,
					written_addr - (int)strlen(obj_code) / 2);
			fputs(rec, output_file);
			strcpy(obj_code, "");
		}
	}
	return written_addr;
}

void pass2(FILE *inter_file, FILE *output_file)
{
	char line[MAX_LEN + 1] = "";
	fgets(line, sizeof line, inter_file);

	char label[MAX_LEN + 1] = "";
	char opcode[MAX_LEN + 1] = "";
	char operand[MAX_LEN + 1] = "";
	int curr_addr = 0;
	parse_inter_line(line, label, opcode, operand, &curr_addr);

	if (!strcmp(opcode, "START")) {
		fgets(line, sizeof line, inter_file);
		parse_inter_line(line, label, opcode, operand, &curr_addr);
	}

	char rec[MAX_LEN + 1] = "";
	get_header_rec(rec);
	fputs(rec, output_file);

	int written_addr = start_addr;
	while (strcmp(opcode, "END")) {
		if (!is_comment(line)) {
			char obj_code[MAX_LEN + 1] = "";

			if (sm_exists(optab, opcode)) {
				char address[4 + 1] = "";
				bool is_indexed = false;
				if (strcmp(operand, "")) {
					char *operand_label = strtok_single(operand, ",");
					char *indexed = strtok_single(NULL, ",");
					if (indexed == NULL) {
						is_indexed = false;
					} else {
						is_indexed = true;
					}

					if (sm_exists(symtab, operand_label)) {
						sm_get(symtab, operand_label, address, 5);
					} else {
						strcpy(address, "0000");
						// error handling undefined symbol
					}
				} else {
					strcpy(address, "0000");
				}

				char opcode_val[2 + 1] = "";
				sm_get(optab, opcode, opcode_val, 3);
				strcat(obj_code, opcode_val);

				int last_2_bytes = strtol(address, NULL, 16);
				if (is_indexed) {
					last_2_bytes = 0x80 | last_2_bytes;
				} else {
					last_2_bytes = 0x00 | last_2_bytes;
				}

				assert(strlen(obj_code) == 2);

				sprintf(obj_code + strlen(obj_code), "%04X", last_2_bytes);
			} else if (!strcmp(opcode, "BYTE")) {
				parse_constant(operand, obj_code);
			} else if (!strcmp(opcode, "WORD")) {
				sprintf(obj_code, "%06X", (int)strtol(operand, NULL, 10));
			}
			get_text_rec(rec, obj_code, curr_addr);
			fputs(rec, output_file);
			written_addr += (strlen(obj_code) / 2);
		}
		fgets(line, sizeof line, inter_file);
		parse_inter_line(line, label, opcode, operand, &curr_addr);
		written_addr =
			fill_remaining_mem(curr_addr, written_addr, output_file);
	}

	get_end_rec(rec);
	fputs(rec, output_file);
}

void read_optab()
{
	FILE *fp = fopen(optab_file_path, "r");
	char line[MAX_LEN + 1];

	while (fgets(line, sizeof line, fp)) {
		char *key = strtok(line, "\t\n");
		char *val = strtok(NULL, "\t\n");
		sm_put(optab, key, val);
	}
	fclose(fp);
}

void assemble(FILE *input_file, FILE *output_file)
{
	optab = sm_new(N_OPTAB_SLOTS);
	read_optab();

	symtab = sm_new(N_SYMTAB_SLOTS);

	FILE *inter_file = fopen("output/intermediate.txt", "w+");
	pass1(input_file, inter_file);
	fclose(inter_file);

	inter_file = fopen("output/intermediate.txt", "r");
	pass2(inter_file, output_file);
	fclose(inter_file);

	sm_delete(optab);
	sm_delete(symtab);
}
