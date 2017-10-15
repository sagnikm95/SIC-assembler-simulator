#include "archi.h"
#include "decode.h"
#include "execute.h"
#include "assembler.h"
#include <stdio.h>
#include <string.h>


#define INPUT_LEN 128

int main()
{ 
	mem[0x1009] = 0x01;
	mem[0x100C] = 0x02;
	char option[INPUT_LEN + 1];
	do {
		printf("assemble[a], load[l], run[r], view_mem[vm], view_reg[vr]\n");
		printf("write_mem[wm], write_reg[wr], step[s], quit[q]\n");

		fgets(option, INPUT_LEN, stdin);
		option[strcspn(option, "\n")] = '\0';
		if (!strcmp(option, "a")) {
			printf("Enter input file: ");
			char input_file_name[INPUT_LEN + 1];
			fgets(input_file_name, INPUT_LEN, stdin);
			input_file_name[strcspn(input_file_name, "\n")] = '\0';
			FILE *input_file = fopen(input_file_name, "r");
			FILE *output_file = fopen("obj.txt", "w+");
			assemble(input_file, output_file);
			fclose(input_file);
			fclose(output_file);
		} else if (!strcmp(option, "l")) {
			char* c="obj.txt";
			loader(c);
		   //	load_interface();
		} else if (!strcmp(option, "r")) {
			exec_prog();
		} else if (!strcmp(option, "vm")) {
			printf("Enter address(in hex): ");
			char addr_str[INPUT_LEN + 1];
			fgets(addr_str, INPUT_LEN, stdin);
			int addr = strtol(addr_str, NULL, 16);
			printf("Memory[%04x] = %02x\n", addr, mem[addr]);
		} else if (!strcmp(option, "vr")) {
		} else if (!strcmp(option, "wm")) {
		} else if (!strcmp(option, "wr")) {
		} else if (!strcmp(option, "s")) {
		} else if (!strcmp(option, "q")) {
		} else {
			printf("Invalid input");
		}
	} while (strcmp(option, "q"));
	return 0;
}
