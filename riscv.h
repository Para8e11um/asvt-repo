#include <assert.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MEMORY_SIZE (1024 * 1024)
#define REG_COUNT 32

typedef struct {
  uint32_t regs[REG_COUNT];
  uint32_t pc;
  uint8_t memory[MEMORY_SIZE];
  bool is_running;
} CPU;

CPU *cpu_init(void);

uint32_t mem_read_u32(CPU *cpu, uint32_t addr);

void execute(CPU *cpu, uint32_t instr);

void run_emulator(CPU *cpu);
