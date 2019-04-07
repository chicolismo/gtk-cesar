#include <stdlib.h>
#include <string.h>
#include "simulator.h"

CPU *cpu_new() {
    CPU *cpu = malloc(sizeof(CPU));
    cpu_clear_registers(cpu);
    return cpu;
}

void cpu_destroy(CPU *cpu) {
    /*
    if (cpu->memory != NULL) {
        free(cpu->memory);
    }
    */
}

void cpu_clear_registers(CPU *cpu) {
    for (int i = 0; i < N_REGISTERS; ++i) {
        cpu->memory[i] = 0;
    }
}

void cpu_set_memory(CPU *cpu, byte *memory, long size) {
    // Somente copia os últimos 2^16 bytes para a memória do simulador.
    //
    // As primeiras instruções podem ser sobre o início de pilha, etc.
    // Essas instruções não fazem parte do simulador.
    int offset = 0;
    if (size > MEM_SIZE) {
        offset = size - MEM_SIZE;
    }
    memcpy((void *) &cpu->memory, (const void *) (memory + offset), MEM_SIZE);
}


byte cpu_get_byte(CPU *cpu, word address) {
    return cpu->memory[address];
}


word cpu_get_word(CPU *cpu, word address) {
    // TODO: Testar se o endereço está na área do cursor
    // Caso esteja, retornar apenas o byte como sendo
    // os bits menos significativos da palavra.

    byte a = cpu->memory[address];
    byte b = cpu->memory[address + 1];
    word w = a << 8;
    w |= b;
    return w;
}

