#include<stdio.h>
#include<stdlib.h>

#include "archi.h"
#include "decode.h"

#define MAX_RECORD_LENGTH 69
#define HEADER_RECORD_LENGTH 19
#define TEXT_RECORD_LENGTH 69
#define END_RECORD_LENGTH 7
#define EXIT_PROG (-1)

void ReadHeaderRecord(char *line,char *progname,int *strtadd,int *objlen){
	int i,cnt = 0;
    for(i = 1; i < 7; ++i) //col 2-7 is program name
    {
        progname[cnt++] = line[i];
    }
    progname[cnt] = '\0';
    
    //reading starting address of objet program
	int num = 0;
    while(i < 13)
    {
        char ch = line[i++];
        int temp=0;
        if(ch >= 'A')
        {
           temp=ch-'A';
           temp+=10;
        }

        else
        	temp=ch-'0';		 
			 
        num += temp;
        num = num << 4;
    }
    num = num >> 4;
    *strtadd = num;
    
    num = 0;
    while(i<19){
	char ch = line[i++];
        int temp=0;
        if(ch >= 'A')
        {
           temp=ch-'A';
           temp+=10;
        }

        else
        	temp=ch-'0';		 
			 
        num += temp;
        num = num << 4;
	}
    num = num >> 4;
    *objlen = num;
}



void ReadTextRecord(char *line,int *RecordStartAdd,int *RecordLength)
{
    int i = 1,num = 0;
    //read starting address
    while(i < 7)
    {
       char ch = line[i++];
        int temp=0;
        if(ch >= 'A')
        {
           temp=ch-'A';
           temp+=10;
        }

        else
        	temp=ch-'0';		 
			 
        num += temp;
        num = num << 4;
	}
    num = num >> 4;
    *RecordStartAdd = num;
    
    //read length of object code in this record
    num = 0;
    while(i < 9)
    {
        char ch = line[i++];
        int temp=0;
        if(ch >= 'A')
        {
           temp=ch-'A';
           temp+=10;
        }

        else
        	temp=ch-'0';		 
			 
        num += temp;
        num = num << 4;
	}
    num = num >> 4;
    *RecordLength = num;
}


void ReadEndRecord(char *line,int *FirstExecIns)
{
    int i = 1,num = 0;
    //read address of first executable instruction in object program
    while(i < 7)
    {
        char ch = line[i++];
        int temp=0;
        if(ch >= 'A')
        {
           temp=ch-'A';
           temp+=10;
        }

        else
        	temp=ch-'0';		 
			 
        num += temp;
        num = num << 4;
	}
    *FirstExecIns = num >> 4;
}

void LoadtoMemory(char *line,int StartAddress,int Length)
{
//	printf("In Load to Memory\n");
    int i = 9,num = 0,BytesRead = 0,index = StartAddress;
    while(BytesRead < Length)
    {   num=0; 
        char ch = line[i++];
      //  printf("h %c \n",ch);
        int temp=0;
        if(ch >= 'A')
        {
           temp=ch-'A';
           temp+=10;
        }

        else
        	temp=ch-'0';		 
			 
        num += temp;
        num = num << 4;
	

        ch = line[i++];
       // printf("h %c \n",ch);
        if(ch >= 'A')
        {
           temp=ch-'A';
           temp+=10;
        }
        else
        	temp=ch-'0';
   
        num += temp;
        BytesRead++;
       // printf("%d : %02x \n",index,(char)num);
       mem[index++]=(char)num;
    }
}

void DecodeInstruction(int instruction,int* opcode,int* address){

  //check if indexing present
    *address = instruction & 0x7FFF;
    if(instruction & 0x8000)
    {
        *address += x.val;
    }
    
    *opcode = instruction & 0xFF0000;
    *opcode = *opcode >> 16;
}


int exec_instruc(int opcode, int addr)
{
	if (opcode == 0xff) {
		return EXIT_PROG;
	}
	functions[opcode](addr);
	return 0;
}

void exec_prog()
{
	functions_init();
	int address,opcode;
	do {
		int instruction =readWord(pc.val);
		pc.val += 3;
		
		DecodeInstruction(instruction,&opcode,&address);
    } while (exec_instruc(opcode,address) != EXIT_PROG);
	// implement ExecuteInstruction using the decode.h and hash table of fn pointers EXIT_PROGRAM similar to halt
	
}

void loader(char* objFileName){
	char ProgramName[7];
    int StartingAddress = 0,ObjcodeLength = 0,RecordStartAddress = 0,RecordLength = 0,FirstExecIns = 0;
    FILE* file = fopen(objFileName, "r");
    if(!file){
    	printf("Error in Opening File\n");
    	return;
	}
	char line[MAX_RECORD_LENGTH + 1];
	fgets(line, sizeof(line), file);
	ReadHeaderRecord(line,ProgramName,&StartingAddress,&ObjcodeLength);
//	printf("Starting address : %d\n",StartingAddress);
    fgets(line, sizeof(line), file);
  
	 while(line[0] != 'E')
    {       
        ReadTextRecord(line,&RecordStartAddress,&RecordLength); 
	//	printf("%s\n",line);
	//	printf("Starting address : %d\n",RecordStartAddress);
        
        LoadtoMemory(line,RecordStartAddress,RecordLength);
        fgets(line, sizeof(line), file);
	}

   /* int i; 
     for(i=1000;i<=1010;i++)
	printf("%d : %02x \n",i,mem[i]);*/


	
	ReadEndRecord(line,&FirstExecIns);
	pc.val=FirstExecIns;
	fclose(file);
}
