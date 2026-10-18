#pragma once
#include "gb.h"
#include "mem.h"
#include <stdint.h>

uint8_t carry_8(uint8_t x, uint8_t y, uint8_t z);
uint8_t carry_4(uint8_t x, uint8_t y, uint8_t z);
uint8_t borrow(uint8_t x, uint8_t y, uint8_t z);
uint8_t get_reg_val(GB_CPU *cpu, GB_Memory *mem, uint8_t pos);
