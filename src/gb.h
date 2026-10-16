#pragma once
#include <stdint.h>

#define DISPLAY_WIDTH 160
#define DISPLAY_HEIGHT 144

typedef struct {
  uint8_t ROM[2][0x4000];
  uint8_t VRAM[0x2000];
  uint8_t ERAM[0x2000];
  uint8_t WRAM[2][0x1000];
  uint8_t OAM[0xA0];
  uint8_t IO_regs[0x80];
  uint8_t HRAM[0x7F];
  uint8_t IE;
} GBA_Memory;
