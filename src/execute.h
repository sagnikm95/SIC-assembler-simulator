#ifndef EXECUTE_H
#define EXECUTE_H
#include<stdio.h>
#include<stdlib.h>

#include "archi.h"
#include "decode.h"

#define MAX_RECORD_LENGTH 69
#define HEADER_RECORD_LENGTH 19
#define TEXT_RECORD_LENGTH 69
#define END_RECORD_LENGTH 7

void ReadHeaderRecord(char *line,char *progname,int *strtadd,int *objlen);
void ReadTextRecord(char *line,int *RecordStartAdd,int *RecordLength);
void ReadEndRecord(char *line,int *FirstExecIns);
void LoadtoMemory(char *line,int StartAddress,int Length);
void DecodeInstruction(int instruction,int* opcode,int* address);
void loader(char* objFileName);
void exec_prog();

#endif
