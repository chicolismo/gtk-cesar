#ifndef __SIMULATOR_H__
#define __SIMULATOR_H__

#include "common.h"

#define N_REGISTERS 8

typedef struct CPU {
    word registers[N_REGISTERS];
    byte memory[MEM_SIZE];
} CPU;

CPU *cpu_new();
void cpu_destroy(CPU *cpu);
void cpu_clear_registers(CPU *cpu);
byte cpu_get_byte(CPU *cpu, word address);
word cpu_get_word(CPU *cpu, word address);

void cpu_set_memory(CPU *cpu, byte *memory, long size);

#endif
