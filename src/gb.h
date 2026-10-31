#pragma once
#include <stdint.h>
#include <stdio.h>

#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 144

typedef struct {
  size_t game_size;
  size_t bios_size;
  uint8_t *bios_ROM;
  uint8_t *game_ROM;
  uint8_t VRAM[0x2000];
  uint8_t ERAM[0x2000];
  uint8_t WRAM[2][0x1000];
  uint8_t OAM[0xA0];
  uint8_t IO_regs[0x80];
  uint8_t HRAM[0x7F];
  uint8_t IE;
} GB_Memory;

typedef struct {
  uint16_t SP;
  uint16_t PC;
  uint8_t IR;
  uint8_t registers[8]; // in the order of B, C, D, E, H, L, [HL], A
  uint8_t flags_reg;
  uint8_t IME;
} GB_CPU;
