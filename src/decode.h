#ifndef DECODE_H
#define DECODE_H

extern void (*functions[1 << 8]) (int addr);

void functions_init();

int readWord(int);
void writeWord(int,int);
void lda(int);
void ldch(int);
void ldl(int);
void ldx(int);
void sta(int);
void stch(int);
void stl(int);
void stx(int);
void add(int);
void sub(int);
void div_asm(int);
void mul(int);
void and_asm(int);
void or_asm(int);
void comp(int);
void j(int);
void jeq(int);
void jgt(int);
void jlt(int);
void jsub(int);
void rsub(int);
void tix(int);
void hlt(int);

#endif
