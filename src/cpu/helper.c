#include "helper.h"
#include "cpu/mem.h"
#include <stdint.h>

uint8_t carry_8(uint8_t x, uint8_t y, uint8_t z) {
  if ((uint16_t)x + (uint16_t)y + (uint16_t)z > UINT8_MAX)
    return 1;
  return 0;
}

uint8_t carry_4(uint8_t x, uint8_t y, uint8_t z) {
  if (x + y + z > 0xF)
    return 1;
  return 0;
}

uint8_t carry_16(uint16_t x, uint16_t y) {
  if ((uint32_t)x + (uint32_t)y > UINT16_MAX)
    return 1;
  return 0;
}

uint8_t carry_12(uint16_t x, uint16_t y) {
  if (x + y > 0xFFF)
    return 1;
  return 0;
}

uint8_t borrow(uint8_t x, uint8_t y, uint8_t z) {
  if (x < y + z)
    return 1;
  return 0;
}

uint8_t read_reg8(GB_CPU *cpu, GB_Memory *mem, uint8_t pos) {
  if (pos == 6) {
    uint16_t hl = (cpu->registers[4] << 8) | cpu->registers[5];
    return read8(mem, hl);
  }

  return cpu->registers[pos];
}

uint16_t read_reg16(GB_CPU *cpu, uint8_t pos) {
  uint16_t val = 0;
  switch (pos) {
  case 0:
    val = (cpu->registers[0] << 8) | cpu->registers[1];
    break;
  case 1:
    val = (cpu->registers[2] << 8) | cpu->registers[3];
    break;
  case 2:
    val = (cpu->registers[4] << 8) | cpu->registers[5];
    break;
  case 3:
    val = cpu->SP;
    break;
  default:
    val = 0;
    break;
  }

  return val;
}

uint16_t read_reg16mem(GB_CPU *cpu, uint8_t pos) {
  uint16_t val = 0;
  switch (pos) {
  case 0:
    val = (cpu->registers[0] << 8) | cpu->registers[1];
    break;
  case 1:
    val = (cpu->registers[2] << 8) | cpu->registers[3];
    break;
  case 2:
    // hl+
    val = (cpu->registers[4] << 8) | cpu->registers[5];
    write_reg16(cpu, 2, read_reg16(cpu, 2) + 1);
    break;
  case 3:
    // hl-
    val = (cpu->registers[4] << 8) | cpu->registers[5];
    write_reg16(cpu, 2, read_reg16(cpu, 2) - 1);
    break;
  default:
    val = 0;
    break;
  }

  return val;
}

void write_reg8(GB_CPU *cpu, GB_Memory *mem, uint8_t pos, uint8_t value) {
  if (pos == 6) {
    uint16_t hl = (cpu->registers[4] << 8) | cpu->registers[5];
    write8(mem, hl, value);
  } else {
    cpu->registers[pos] = value;
  }
}

void write_reg16(GB_CPU *cpu, uint8_t pos, uint16_t value) {
  uint8_t hval = value >> 8;
  uint8_t lval = value & 0xFF;

  switch (pos) {
  case 0:
    cpu->registers[0] = hval;
    cpu->registers[1] = lval;
    break;
  case 1:
    cpu->registers[2] = hval;
    cpu->registers[3] = lval;
    break;
  case 2:
    cpu->registers[4] = hval;
    cpu->registers[5] = lval;
    break;
  case 3:
    cpu->SP = value;
    break;
  }
}
