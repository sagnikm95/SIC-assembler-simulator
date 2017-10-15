#include "assembler.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
	if (argc == 3) {
		FILE *input_file = fopen(argv[1], "r");
		FILE *output_file = fopen(argv[2], "w+");
		assemble(input_file, output_file);
		fclose(input_file);
		fclose(output_file);
	}
	return 0;
}
