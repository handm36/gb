#define SDL_MAIN_USE_CALLBACKS 1
#include "audio/audio.h"
#include "display/display.h"
#include "gb.h"
#include "input/input.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)) {
    printf("Couldn't initialize SDL: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  SDL_SetAppMetadata("GameBoy", "1.0", "com.gb");

  if (init_display() == SDL_APP_FAILURE) {
    return SDL_APP_FAILURE;
  }

  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS;
  }
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void *appstate) { return SDL_APP_CONTINUE; }

void SDL_AppQuit(void *appstate, SDL_AppResult result) {}
