#include "decode.h"
#include "archi.h"
#include <stdio.h>

void (*functions[1 << 8]) (int addr);

void functions_init()
{
	functions[0x00] = lda;
	functions[0x50] = ldch;
	functions[0x08] = ldl;
	functions[0x04] = ldx;
	functions[0x0C] = sta;
	functions[0x54] = stch;
	functions[0x14] = stl;
	functions[0x10] = stx;
	functions[0x18] = add;
	functions[0x1C] = sub;
	functions[0x24] = div_asm;
	functions[0x20] = mul;
	functions[0x40] = and_asm;
	functions[0x44] = or_asm;
	functions[0x28] = comp;
	functions[0x3C] = j;
	functions[0x30] = jeq;
	functions[0x34] = jgt;
	functions[0x38] = jlt;
	functions[0x48] = jsub;
	functions[0x4C] = rsub;
	functions[0xFF] = hlt;
	functions[0x2C] = tix; 
}

//read a 24 bit word from memory 
int readWord(int address){
int num1 = 0,num2 = 0,i;

    for( i = 0; i < 3; ++i)
    {
        char ch = mem[address++];
        num2 = (int)ch;
        num2 = num2 & 0x000000FF;
        num1 = num1 | num2;
        num1 = num1 << 8;
    }
    
    //making the numbers behave as signed 24 bit if MSB as 1 maintain as 2's complement
    num1 = num1 >> 8;
    if(num1 & 0x800000)
    {
        num1 = num1 | 0xFF000000;
    }
	return num1;	
}

//store a 24 bit word to memory 
void writeWord(int address, int data){
	char ch;
	int i;
    for( i = 0; i <= 2; ++i)
    {
        ch = data & 0xFF;
        mem[address + i] = ch;
        data = data >> 8;
    }
	
}

//implementing lda : load to accumulator
void lda(int address){  
    a.val=readWord(address);
}

//implementing ldch : A[rightmost 8 bit]->m
void ldch(int address){
		char word = mem[address];
        int tmpch = (int)word;
        tmpch = tmpch & 0xFF;
        a.val= a.val & 0xFFFFFF00;
        a.val = a.val | tmpch;
	
}

//implementing ldl load to L register
void ldl(int address){  
    a.val=readWord(address);
}

//implementing ldx load to X register
void ldx(int address){  
    x.val=readWord(address);
}
//implementing sta : store from accumulator
void sta(int address){
	writeWord(address,a.val);
}

//implement stch m->A[rightmost 8 bit]
void stch(int address){
	int tmp=a.val & 0xFF;
	mem[address]=(char)tmp;
}

//implement stl : store L register
void stl(int address){
	writeWord(address,l.val);
}

//implement stx : store X register
void stx(int address){
	writeWord(address,x.val);
}


//implementing ADD
void add(int address){
	int word=readWord(address);
	a.val=a.val+word;
}

//implementing SUB : Subtraction
void sub(int address){
	int word=readWord(address);
	a.val=a.val-word;
}

//implementing DIV : division
void div_asm(int address){
	int word=readWord(address);
	if(!word){
        printf("Divide by Zero Error\n");
		return;
	}
	a.val=a.val/word;
}

//implementing MUL : Multiplication
void mul(int address){
	int word=readWord(address);
	a.val=a.val*word;
}

//implementing AND

void and_asm(int address){
	int word=readWord(address);
	a.val=a.val & word;
}

//implementing OR

void or_asm(int address){
	int word=readWord(address);
	a.val=a.val | word;
}

//comparator function
void comp(int address){
	int word=readWord(address);
	if(a.val==word) sw.val=0;
	else if(a.val<word)sw.val=-1;
	else sw.val=1;
	 /***
        CC = 0 -> Equal
        CC = -1 -> Less than
        CC = 1 -> Greater than
        ***/
}

//implementing j LABEL : jump
void j(int address){
	pc.val=address;
}

//implement jeq : jump if equal
void jeq(int address){
	if(sw.val==0)
	pc.val=address;
}

//implement jump on greater than jgt
void jgt(int address){
	if(sw.val==1)
	pc.val=address;
}

//implement JLT jump on lower than
void jlt(int address){
	if(sw.val==-1)
	pc.val=address;
}

//implement jsub
void jsub(int address){
	l.val=pc.val;
	pc.val=address;
}

//implement rsub
void rsub(int address){
	(void)address;
	pc.val=l.val;
}

void hlt(int address)
{
	(void)address;
	return;
}

//implementing TIX
void tix(int address){
	//increase index register by 1
	x.val=x.val+1;
	int word=readWord(address);
	if(x.val==word) sw.val=0;
	else if(x.val<word)sw.val=-1;
	else x.val=1;
	 /***
        CC = 0 -> Equal
        CC = -1 -> Less than
        CC = 1 -> Greater than
        ***/
	
}