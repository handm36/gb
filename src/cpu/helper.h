#pragma once
#include "gb.h"

uint8_t carry_8(uint8_t x, uint8_t y, uint8_t z);
uint8_t carry_4(uint8_t x, uint8_t y, uint8_t z);
uint8_t carry_16(uint16_t x, uint16_t y);
uint8_t carry_12(uint16_t x, uint16_t y);
uint8_t borrow(uint8_t x, uint8_t y, uint8_t z);
uint8_t read_reg8(GB_CPU *cpu, GB_Memory *mem, uint8_t pos);
uint16_t read_reg16(GB_CPU *cpu, uint8_t pos);
uint16_t read_reg16mem(GB_CPU *cpu, uint8_t pos);
void write_reg8(GB_CPU *cpu, GB_Memory *mem, uint8_t pos, uint8_t value);
void write_reg16(GB_CPU *cpu, uint8_t pos, uint16_t value);
