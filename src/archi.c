#include "archi.h"

#define MEM_SIZE (1 << 15)

char mem[MEM_SIZE];

Reg a = { 0, 0 };
Reg x = { 1, 0 };
Reg l = { 2, 0 };
Reg pc = { 8, 0 };
Reg sw = { 9, 0 };
