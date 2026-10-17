#include "mem.h"
#include "gb.h"

uint8_t *resolve_addr(GB_Memory *mem, uint16_t addr) {
  if (addr < mem->game_size) {
    return &mem->game_ROM[addr];
  } else if (addr >= 0x8000 && addr <= 0x9FFF) {
    return &mem->VRAM[addr - 0x8000];
  } else if (addr >= 0xC000 && addr <= 0xCFFF) {
    return &mem->WRAM[0][addr - 0xC000];
  } else if (addr >= 0xD000 && addr <= 0xDFFF) {
    return &mem->WRAM[1][addr - 0xD000];
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
