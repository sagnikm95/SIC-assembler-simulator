#ifndef ARCHI_H
#define ARCHI_H

// contains the memory and registers for the simulator

typedef struct reg Reg;

struct reg {
	int no;
	int val;
};

extern char mem[];

extern Reg a;
extern Reg x;
extern Reg l;
extern Reg pc;
extern Reg sw;

#endif
