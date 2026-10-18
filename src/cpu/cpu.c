#include "cpu.h"
#include <stdint.h>

static void inst_add_adc8(GB_CPU *cpu, uint8_t value, uint8_t is_adc) {
  uint8_t temp = cpu->registers[7];
  uint8_t carry = ((cpu->flags_reg >> C_FLAG_LOC) & !!is_adc);
  cpu->registers[7] += value + carry;

  cpu->flags_reg = !cpu->registers[7] << Z_FLAG_LOC;
  cpu->flags_reg |= carry_4(temp & 0xF, value & 0xF, carry) << H_FLAG_LOC;
  cpu->flags_reg |= carry_8(temp, value, carry) << C_FLAG_LOC;
}

static void inst_sub_sbc8(GB_CPU *cpu, uint8_t value, uint8_t is_sbc) {
  uint8_t temp = cpu->registers[7];
  uint8_t carry = ((cpu->flags_reg >> C_FLAG_LOC) & !!is_sbc);
  cpu->registers[7] -= value + carry;

  cpu->flags_reg = (!cpu->registers[7] << Z_FLAG_LOC) | N_FLAG;
  cpu->flags_reg |= borrow(temp & 0xF, value & 0xF, carry) << H_FLAG_LOC;
  cpu->flags_reg |= borrow(temp, value, carry) << C_FLAG_LOC;
}

static void inst_and8(GB_CPU *cpu, uint8_t value) {
  cpu->registers[7] &= value;
  cpu->flags_reg = (!cpu->registers[7] << Z_FLAG_LOC) | H_FLAG;
}

static void inst_xor8(GB_CPU *cpu, uint8_t value) {
  cpu->registers[7] ^= value;
  cpu->flags_reg = (!cpu->registers[7] << Z_FLAG_LOC);
}

static void inst_or8(GB_CPU *cpu, uint8_t value) {
  cpu->registers[7] |= value;
  cpu->flags_reg = (!cpu->registers[7] << Z_FLAG_LOC);
}

static void inst_cp8(GB_CPU *cpu, uint8_t value) {
  uint8_t temp = cpu->registers[7] - value;
  cpu->flags_reg = (!cpu->registers[7] << Z_FLAG_LOC) | N_FLAG;
  cpu->flags_reg |= borrow(temp & 0xF, value & 0xF, 0) << H_FLAG_LOC;
  cpu->flags_reg |= borrow(temp, value, 0) << C_FLAG_LOC;
}

// 0 for continue and 1 for exit
int run_cpu(GB_CPU *cpu, GB_Memory *mem) {
  uint8_t inst = read8(mem, cpu->PC);

  switch (inst >> 6) {
  case 0b00:
    // Group 0
    break;
  case 0b01:
    // Group 1
    break;
  case 0b10:
    // Group 2
    if ((inst & 0b00110000) == 0) {
      // Add / Adc
      inst_add_adc8(cpu, get_reg_val(cpu, mem, inst & 0x7), ((inst >> 3) & 1));
      cpu->PC += 1;
    }

    if ((inst & 0b00110000) == 0b00010000) {
      // Sub / Sbc
      inst_sub_sbc8(cpu, get_reg_val(cpu, mem, inst & 0x7), ((inst >> 3) & 1));
      cpu->PC += 1;
    }

    if ((inst & 0b00111000) == 0b00100000) {
      // And
      inst_and8(cpu, get_reg_val(cpu, mem, inst & 0x7));
      cpu->PC += 1;
    }

    if ((inst & 0b00111000) == 0b00101000) {
      // Xor
      inst_xor8(cpu, get_reg_val(cpu, mem, inst & 0x7));
      cpu->PC += 1;
    }

    if ((inst & 0b00111000) == 0b00110000) {
      // Or
      inst_or8(cpu, get_reg_val(cpu, mem, inst & 0x7));
      cpu->PC += 1;
    }

    if ((inst & 0b00111000) == 0b00111000) {
      // Cp
      inst_cp8(cpu, get_reg_val(cpu, mem, inst & 0x7));
      cpu->PC += 1;
    }

    break;
  case 0b11:
    // Group 3
    if ((inst & 0b00110111) == 0b00000110) {
      // Add / Adc
      inst_add_adc8(cpu, read8(mem, cpu->PC + 1), ((inst >> 3) & 1));
      cpu->PC += 2;
    }
    if ((inst & 0b00110111) == 0b00010110) {
      // Sub / Sbc
      inst_sub_sbc8(cpu, read8(mem, cpu->PC + 1), ((inst >> 3) & 1));
      cpu->PC += 2;
    }

    if ((inst & 0b00111111) == 0b00100110) {
      // And
      inst_and8(cpu, read8(mem, cpu->PC + 1));
      cpu->PC += 2;
    }

    if ((inst & 0b00111111) == 0b00101110) {
      // Xor
      inst_xor8(cpu, read8(mem, cpu->PC + 1));
      cpu->PC += 2;
    }

    if ((inst & 0b00111111) == 0b00110110) {
      // Or
      inst_or8(cpu, read8(mem, cpu->PC + 1));
      cpu->PC += 2;
    }

    if ((inst & 0b00111111) == 0b00111110) {
      // Cp
      inst_cp8(cpu, read8(mem, cpu->PC + 1));
      cpu->PC += 2;
    }

    break;
  }

  return 0;
}
