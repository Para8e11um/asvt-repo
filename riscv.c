#include "riscv.h"

#define MEMORY_SIZE (1024 * 1024)
#define REG_COUNT 32

CPU *cpu_init(void) {
  CPU *cpu = (CPU *)malloc(sizeof(CPU));
  memset(cpu, 0, sizeof(CPU));
  cpu->is_running = true;
  return cpu;
}

uint32_t mem_read_u32(CPU *cpu, uint32_t addr) {
  return *((uint32_t *)&cpu->memory[addr]);
}

void execute(CPU *cpu, uint32_t instr) {
  uint8_t opcode = instr & 0x7F;
  uint8_t rd = (instr >> 7) & 0x1F;
  uint8_t funct3 = (instr >> 12) & 0x07;
  uint8_t rs1 = (instr >> 15) & 0x1F;
  uint8_t rs2 = (instr >> 20) & 0x1F;
  uint8_t funct7 = (instr >> 25) & 0x7F;

  uint32_t current_pc = cpu->pc - 4;

  switch (opcode) {
  case 0x33: {
    if (funct3 == 0x0 && funct7 == 0x00) { // ADD
      if (rd != 0)
        cpu->regs[rd] = cpu->regs[rs1] + cpu->regs[rs2];
    } else if (funct3 == 0x0 && funct7 == 0x20) { // SUB
      if (rd != 0)
        cpu->regs[rd] = cpu->regs[rs1] - cpu->regs[rs2];
    }
    break;
  }
  case 0x13: {
    int32_t imm = ((int32_t)instr) >> 20;
    if (funct3 == 0x0) { // ADDI
      if (rd != 0)
        cpu->regs[rd] = cpu->regs[rs1] + imm;
    }
    break;
  }
  case 0x03: {
    int32_t imm = ((int32_t)instr) >> 20;
    if (funct3 == 0x2) { // LW
      uint32_t addr = cpu->regs[rs1] + imm;
      if (rd != 0)
        cpu->regs[rd] = mem_read_u32(cpu, addr);
    }
    break;
  }
  case 0x23: {
    int32_t imm_s =
        ((int32_t)(instr & 0xFE000000) >> 20) | ((instr >> 7) & 0x1F);
    if (funct3 == 0x2) { // SW
      uint32_t addr = cpu->regs[rs1] + imm_s;
      *((uint32_t *)&cpu->memory[addr]) = cpu->regs[rs2];
    }
    break;
  }
  case 0x63: {
    int32_t imm_b = ((int32_t)(instr & 0x80000000) >> 19) |
                    ((instr & 0x80) << 4) | ((instr >> 20) & 0x7E0) |
                    ((instr >> 7) & 0x01E);
    if (funct3 == 0x0) { // BEQ
      if (cpu->regs[rs1] == cpu->regs[rs2]) {
        cpu->pc = current_pc + imm_b;
      }
    }
    break;
  }
  case 0x6F: {
    int32_t imm_j = ((int32_t)(instr & 0x80000000) >> 11) |
                    (instr & 0x000FF000) | ((instr >> 9) & 0x0800) |
                    ((instr >> 20) & 0x07FE);
    if (rd != 0) {

      cpu->regs[rd] = cpu->pc;
    }
    cpu->pc = current_pc + imm_j; // Прыжок
    break;
  }
  case 0x73: {
    cpu->is_running = false;
    break;
  }
  default:
    printf("Неизвестная инструкция: 0x%08X на адресе 0x%08X\n", instr,
           current_pc);
    cpu->is_running = false;
    break;
  }
}

void run_emulator(CPU *cpu) {
  while (cpu->is_running) {
    uint32_t instr = mem_read_u32(cpu, cpu->pc);

    cpu->pc += 4;

    execute(cpu, instr);
  }
}
