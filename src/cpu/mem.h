#pragma once
#include "gb.h"
#include <stdint.h>
#include <string.h>

uint8_t *resolve_addr(GB_Memory *mem, uint16_t addr);

uint8_t read8(GB_Memory *mem, uint16_t addr);
void write8(GB_Memory *mem, uint16_t addr, uint8_t data);
uint16_t read16(GB_Memory *mem, uint16_t addr);
void write16(GB_Memory *mem, uint16_t addr, uint16_t data);
