#include "riscv.h"

void test_simple() {
  CPU *cpu = cpu_init();

  uint32_t program[] = {0x00A00293, 0x02000313, 0x006283B3, 0x00100073};

  memcpy(cpu->memory, program, sizeof(program));
  run_emulator(cpu);

  assert(cpu->regs[7] == 42);

  free(cpu);
}

void test_fibonacci() {
  CPU *cpu = cpu_init();

  uint32_t program_fib[] = {0x00000293, 0x00100313, 0x00A00393, 0x00038C63,
                            0x00628433, 0x00030293, 0x00040313, 0xFFF38393,
                            0xFEDFF06F, 0x00100073};

  memcpy(cpu->memory, program_fib, sizeof(program_fib));
  run_emulator(cpu);

  free(cpu);
}

int main() {
  test_simple();
  test_fibonacci();
  return 0;
}
