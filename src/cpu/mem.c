#include "mem.h"
#include "gb.h"
#include <stdint.h>
#include <string.h>

uint8_t *resolve_addr(GB_Memory *mem, uint16_t addr) {
  if (addr <= 0x7FFF && addr < mem->game_size) {
    return &mem->game_ROM[addr];
  } else if (addr >= 0x8000 && addr <= 0x9FFF) {
    return &mem->VRAM[addr - 0x8000];
  } else if (addr >= 0xA000 && addr <= 0xBFFF) {
    return &mem->ERAM[addr - 0xA000];
  } else if (addr >= 0xC000 && addr <= 0xCFFF) {
    return &mem->WRAM[0][addr - 0xC000];
  } else if (addr >= 0xD000 && addr <= 0xDFFF) {
    return &mem->WRAM[1][addr - 0xD000];
  } else if (addr >= 0xE000 && addr <= 0xEFFF) {
    return &mem->WRAM[0][addr - 0xE000];
  } else if (addr >= 0xF000 && addr <= 0xFDFF) {
    return &mem->WRAM[1][addr - 0xF000];
  } else if (addr >= 0xFE00 && addr <= 0xFE9F) {
    return &mem->OAM[addr - 0xFE00];
  } else if (addr >= 0xFF00 && addr <= 0xFF7F) {
    return &mem->IO_regs[addr - 0xFF00];
  } else if (addr >= 0xFF80 && addr <= 0xFFFE) {
    return &mem->HRAM[addr - 0xFF80];
  } else if (addr == 0xFFFF) {
    return &mem->IE;
  } else {
    return NULL;
  }
}

uint8_t read8(GB_Memory *mem, uint16_t addr) {
  uint8_t *result = resolve_addr(mem, addr);
  if (result == NULL) {
    return 0xFF;
  }

  return *result;
}

void write8(GB_Memory *mem, uint16_t addr, uint8_t data) {
  if (addr < 0x8000) {
    return;
  }

  uint8_t *result = resolve_addr(mem, addr);
  if (result == NULL) {
    return;
  }

  memcpy(result, &data, 1);
}

uint16_t read16(GB_Memory *mem, uint16_t addr) {
  return (read8(mem, addr + 1) << 8) | read8(mem, addr);
}

void write16(GB_Memory *mem, uint16_t addr, uint16_t data) {
  write8(mem, addr, (data & 0xFF));
  write8(mem, addr + 1, (data & 0xFF00) >> 8);
}
