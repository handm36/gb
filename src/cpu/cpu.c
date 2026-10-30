#include "cpu.h"

// op_b3 uses the same bits as r8 as the second operand
static inline uint8_t op_b3(uint8_t inst) { return (inst >> 3) & 0x7; }

static inline uint8_t op_r16(uint8_t inst) { return (inst >> 4) & 0x3; }
static inline uint8_t op_r8(uint8_t inst) { return inst & 0x7; }
static inline uint8_t op_carry(uint8_t inst) { return (inst >> 3) & 1; }

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

static void inst_ld8(GB_CPU *cpu, GB_Memory *mem, uint8_t reg, uint8_t value) {
  write_reg8(cpu, mem, reg, value);
}

static void inst_ld16_reg(GB_CPU *cpu, uint8_t reg, uint16_t value) {
  write_reg16(cpu, reg, value);
}

static void inst_ld8_mem(GB_Memory *mem, uint16_t addr, uint8_t value) {
  write8(mem, addr, value);
}

static void inst_ld16_mem(GB_Memory *mem, uint16_t addr, uint16_t value) {
  write16(mem, addr, value);
}

static int group_0(GB_CPU *cpu, GB_Memory *mem, uint8_t inst) {
  if ((inst & 0b00001111) == 0b00000001) {

    // ld r16, imm16
    inst_ld16_reg(cpu, op_r16(inst), read16(mem, cpu->PC + 1));
    cpu->PC += 3;

  } else if ((inst & 0b00001111) == 0b00000010) {

    // ld [r16mem], a
    inst_ld16_mem(mem, read_reg16mem(cpu, op_r16(inst)),
                  read_reg8(cpu, mem, 7));
    cpu->PC += 1;

  } else if ((inst & 0b00001111) == 0b00001010) {

    // ld a, [r16mem]
    inst_ld8(cpu, mem, 7, read8(mem, read_reg16mem(cpu, op_r16(inst))));
    cpu->PC += 1;

  } else if ((inst & 0b00001111) == 0b00001000) {

    // ld [imm16], sp
    inst_ld16_mem(mem, read16(mem, cpu->PC + 1), cpu->SP & 0xFF);
    cpu->PC += 3;

  } else if ((inst & 0b00001111) == 0b00000011) {

    // inc r16
    write_reg16(cpu, op_r16(inst), read_reg16(cpu, op_r16(inst)) + 1);
    cpu->PC += 1;

  } else if ((inst & 0b00001111) == 0b00001011) {

    // dec r16
    write_reg16(cpu, op_r16(inst), read_reg16(cpu, op_r16(inst)) - 1);
    cpu->PC += 1;

  } else if ((inst & 0b00001111) == 0b00001001) {

    // add hl, r16
    uint16_t hl = read_reg16(cpu, 2);
    uint16_t r16 = read_reg16(cpu, op_r16(inst));

    write_reg16(cpu, 2, hl + r16);

    cpu->flags_reg = cpu->flags_reg & Z_FLAG;
    cpu->flags_reg |= carry_12(hl, r16) << H_FLAG_LOC;
    cpu->flags_reg |= carry_16(hl, r16) << C_FLAG_LOC;

    cpu->PC += 1;

  } else if ((inst & 0b00000111) == 0b00000100) {

    // inc r8
    uint8_t r8 = read_reg8(cpu, mem, op_b3(inst));

    write_reg8(cpu, mem, op_b3(inst), r8 + 1);

    cpu->flags_reg = cpu->flags_reg & C_FLAG;
    cpu->flags_reg |= !(uint8_t)(r8 + 1) << Z_FLAG_LOC;
    cpu->flags_reg |= carry_4(r8, 1, 0) << H_FLAG_LOC;

    cpu->PC += 1;

  } else if ((inst & 0b0000111) == 0b00000100) {

    // dec r8
    uint8_t r8 = read_reg8(cpu, mem, op_b3(inst));

    write_reg8(cpu, mem, op_b3(inst), r8 - 1);

    cpu->flags_reg = (cpu->flags_reg & C_FLAG) | N_FLAG;
    cpu->flags_reg |= !(uint8_t)(r8 - 1) << Z_FLAG_LOC;
    cpu->flags_reg |= borrow(r8 & 0xF, 1, 0) << H_FLAG_LOC;

    cpu->PC += 1;

  } else if ((inst & 0b00000111) == 0b00000110) {

    // ld r8, imm8
    inst_ld8(cpu, mem, op_b3(inst), read8(mem, cpu->PC + 1));
    cpu->PC += 2;

  } else {
    return 1;
  }

  return 0;
}

static int group_2(GB_CPU *cpu, GB_Memory *mem, uint8_t inst) {
  if ((inst & 0b00110000) == 0) {

    // add/adc a, r8
    inst_add_adc8(cpu, read_reg8(cpu, mem, op_r8(inst)), op_carry(inst));
    cpu->PC += 1;

  } else if ((inst & 0b00110000) == 0b00010000) {

    // sub/sbc a, r8
    inst_sub_sbc8(cpu, read_reg8(cpu, mem, op_r8(inst)), op_carry(inst));
    cpu->PC += 1;

  } else if ((inst & 0b00111000) == 0b00100000) {

    // and a, r8
    inst_and8(cpu, read_reg8(cpu, mem, op_r8(inst)));
    cpu->PC += 1;

  } else if ((inst & 0b00111000) == 0b00101000) {

    // xor a, r8
    inst_xor8(cpu, read_reg8(cpu, mem, op_r8(inst)));
    cpu->PC += 1;

  } else if ((inst & 0b00111000) == 0b00110000) {

    // or a, r8
    inst_or8(cpu, read_reg8(cpu, mem, op_r8(inst)));
    cpu->PC += 1;

  } else if ((inst & 0b00111000) == 0b00111000) {

    // cp a, r8
    inst_cp8(cpu, read_reg8(cpu, mem, op_r8(inst)));
    cpu->PC += 1;

  } else {
    return 1;
  }

  return 0;
}

static int group_3(GB_CPU *cpu, GB_Memory *mem, uint8_t inst) {
  if ((inst & 0b00110111) == 0b00000110) {

    // add/adc a, imm8
    inst_add_adc8(cpu, read8(mem, cpu->PC + 1), op_carry(inst));

    cpu->PC += 2;
  } else if ((inst & 0b00110111) == 0b00010110) {

    // sub/sbc a, imm8
    inst_sub_sbc8(cpu, read8(mem, cpu->PC + 1), op_carry(inst));
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00100110) {

    // and a, imm8
    inst_and8(cpu, read8(mem, cpu->PC + 1));
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00101110) {

    // xor a, imm8
    inst_xor8(cpu, read8(mem, cpu->PC + 1));
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00110110) {

    // or a, imm8
    inst_or8(cpu, read8(mem, cpu->PC + 1));
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00111110) {

    // cp a, imm8
    inst_cp8(cpu, read8(mem, cpu->PC + 1));
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00100010) {

    // ldh [c], a
    inst_ld8_mem(mem, (uint16_t)0xFF00 | read_reg8(cpu, mem, 1),
                 read_reg8(cpu, mem, 7));
    cpu->PC += 1;

  } else if ((inst & 0b00111111) == 0b00100000) {

    // ldh [imm8], a
    inst_ld8_mem(mem, (uint16_t)0xFF00 | read8(mem, cpu->PC + 1),
                 read_reg8(cpu, mem, 7));
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00101010) {

    // ld [imm16], a
    inst_ld8_mem(mem, read16(mem, cpu->PC + 1), read_reg8(cpu, mem, 7));
    cpu->PC += 3;

  } else if ((inst & 0b00111111) == 0b00110010) {

    // ldh a, [c]
    inst_ld8(cpu, mem, 7,
             read8(mem, (uint16_t)0xFF00 | read_reg8(cpu, mem, 1)));
    cpu->PC += 1;

  } else if ((inst & 0b00111111) == 0b00110000) {

    // ldh a, [imm8]
    inst_ld8(cpu, mem, 7,
             read8(mem, (uint16_t)0xFF00 | read8(mem, cpu->PC + 1)));
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00111010) {

    // ld a, [imm16]
    inst_ld8(cpu, mem, 7, read8(mem, read16(mem, cpu->PC + 1)));
    cpu->PC += 3;

  } else if ((inst & 0b00111111) == 0b00101000) {

    // add sp, imm8
    int16_t num = (int16_t)(int8_t)read8(mem, cpu->PC + 1);

    cpu->flags_reg = carry_4(cpu->SP & 0xF, num & 0xF, 0) << H_FLAG_LOC;
    cpu->flags_reg |= carry_8(cpu->SP & 0xFF, num & 0xFF, 0) << C_FLAG_LOC;

    cpu->SP += num;
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00111000) {

    // ld hl, sp + imm8
    inst_ld16_reg(cpu, 2, cpu->SP + ((int16_t)(int8_t)read8(mem, cpu->PC + 1)));
    cpu->PC += 2;

  } else if ((inst & 0b00111111) == 0b00111001) {

    // ld sp, hl
    inst_ld16_reg(cpu, 3, read_reg16(cpu, 2));
    cpu->PC += 1;

  } else {
    return 1;
  }

  return 0;
}

// 0 for continue and 1 for unknown opcode
int run_cpu(GB_CPU *cpu, GB_Memory *mem) {
  uint8_t inst = read8(mem, cpu->PC);

  switch (inst >> 6) {
  case 0b00:
    // Group 0
    return group_0(cpu, mem, inst);
  case 0b01:
    // Group 1
    if (inst == 0b01110110) {
      // halt
      return 0;
    } else {
      // ld r8, r8
      inst_ld8(cpu, mem, op_b3(inst), read_reg8(cpu, mem, op_r8(inst)));
      cpu->PC += 1;
      return 0;
    }
  case 0b10:
    // Group 2
    return group_2(cpu, mem, inst);
  case 0b11:
    // Group 3
    return group_3(cpu, mem, inst);
  default:
    return 1;
  }

  return 0;
}
