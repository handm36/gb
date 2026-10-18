#include "helper.h"

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

uint8_t borrow(uint8_t x, uint8_t y, uint8_t z) {
  if (x < y + z)
    return 1;
  return 0;
}

uint8_t get_reg_val(GB_CPU *cpu, GB_Memory *mem, uint8_t pos) {
  if (pos == 6) {
    uint16_t hl = (cpu->registers[4] << 8) | cpu->registers[5];
    return read8(mem, hl);
  }

  return cpu->registers[pos];
}
