#include "cpu/cpu.h"
#include "cpu/helper.h"
#define SDL_MAIN_USE_CALLBACKS 1
#include "cpu/mem.h"
#include "display/display.h"
#include "gb.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

GB_Memory mem = {0};
GB_CPU cpu = {.SP = 0xFFFE,
              .PC = 0x100,
              .registers = {0, 0x13, 0, 0xD8, 0x1, 0x4D, 0, 0x1},
              .flags_reg = 0b10000000};

void free_items(GB_Memory *mem) {
  free(mem->bios_ROM);
  free(mem->game_ROM);
}

static SDL_AppResult parse_file(uint8_t **rom_ptr, size_t *size_ptr,
                                char *path) {
  FILE *file_ptr = fopen(path, "rb");

  if (file_ptr == NULL) {
    printf("No file specified \"%s\"\n", path);
    return SDL_APP_FAILURE;
  }

  if (fseek(file_ptr, 0, SEEK_END) != 0) {
    fclose(file_ptr);
    printf("Failed to read file \"%s\"\n", path);
    return SDL_APP_FAILURE;
  }

  long size = ftell(file_ptr);
  if (size == -1) {
    fclose(file_ptr);
    printf("Failed to get file size \"%s\"\n", path);
    return SDL_APP_FAILURE;
  }

  if (size == 0) {
    fclose(file_ptr);
    printf("Invalid file \"%s\"\n", path);
    return SDL_APP_FAILURE;
  }
  fseek(file_ptr, 0, SEEK_SET);

  uint8_t *rom = malloc(size);
  if (rom == NULL) {
    fclose(file_ptr);
    perror("Couldn't allocate memory");
    return SDL_APP_FAILURE;
  }

  size_t ret = fread(rom, 1, size, file_ptr);

  if (ret != (unsigned long)size) {
    fclose(file_ptr);
    free(rom);
    printf("Failed to read file\n");
    return SDL_APP_FAILURE;
  }

  *rom_ptr = rom;
  *size_ptr = size;

  fclose(file_ptr);
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Usage: %s <game file path> <bios file path>\n", argv[0]);
    return SDL_APP_FAILURE;
  }

  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    printf("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_SetAppMetadata("GameBoy", "1.0", "com.gameboy");

  if (init_display() == SDL_APP_FAILURE) {
    return SDL_APP_FAILURE;
  }

  if (parse_file(&mem.game_ROM, &mem.game_size, argv[1]) == SDL_APP_FAILURE) {
    return SDL_APP_FAILURE;
  }

  if (parse_file(&mem.bios_ROM, &mem.bios_size, argv[2]) == SDL_APP_FAILURE) {
    return SDL_APP_FAILURE;
  }

  if (read8(&mem, 0x014D) != 0) {
    cpu.flags_reg |= 0b00110000;
  }

  uint8_t checksum = 0;
  for (uint16_t address = 0x0134; address <= 0x014C; address++) {
    checksum = checksum - read8(&mem, address) - 1;
  }

  if (checksum != 0) {
    cpu.flags_reg |= H_FLAG | C_FLAG;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) {

  // Logging

  printf("A: %02X ", cpu.registers[7]);
  printf("F: %02X ", cpu.flags_reg);
  printf("B: %02X ", cpu.registers[0]);
  printf("C: %02X ", cpu.registers[1]);
  printf("D: %02X ", cpu.registers[2]);
  printf("E: %02X ", cpu.registers[3]);
  printf("H: %02X ", cpu.registers[4]);
  printf("L: %02X ", cpu.registers[5]);
  printf("SP: %04X ", cpu.SP);
  printf("PC: 00:%04X ", cpu.PC);
  printf("(%02X ", read8(&mem, cpu.PC));
  printf("%02X ", read8(&mem, cpu.PC + 1));
  printf("%02X ", read8(&mem, cpu.PC + 2));
  printf("%02X)\n", read8(&mem, cpu.PC + 3));

  render_frame();
  if (run_cpu(&cpu, &mem) == 1) {
    // printf("Unknown instruction: 0x%02X\nat: 0x%04X\n", read8(&mem, cpu.PC),
    //       cpu.PC);
    return SDL_APP_SUCCESS;
  }

  return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void *appstate, SDL_AppResult result) { free_items(&mem); }
