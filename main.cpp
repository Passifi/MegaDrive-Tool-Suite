#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_keycode.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include "input.h"
#include "fileIO.h"
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <queue>
#include <set>
#include <stdio.h>
#include <vector>
#define SDL_MAIN_USE_CALLBACKS 1 /* use the callbacks instead of main() */
#include "include/ControlStructure.h"
#include "include/Tiledata.h"
#include "include/render.h"
#include "include/input.h"
#include "include/AppState.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

static Renderer mainRenderer;
static int screenWidth = 800;
static int screenHeight = 600;
InputHandler inputHandler;
EventHandler eventHandler;
TilemapController tilemapController;
static std::ofstream logFile;
SDL_AppResult handleKeyDown(SDL_Event* event);
SDL_FRect tileSelectionRect = {(float)400, 000, 80, (float)screenHeight};
Renderer mainRenderer;
TilemapBuilder tilemapBuilder;
void initializeLogging();

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  std::string filePath = "build/catwartilesreduced.bin";
  if (argc > 1) {
    filePath = argv[1];
  }
  initializeLogging();
  if(!mainRenderer.intialize(screenWidth,screenHeight)) {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;

  }
  tilemapController.initMap(screenWidth,screenHeight); 
  TilemapBuilder builder(mainRenderer.renderer,tilemapController);
  builder.addTiles(filePath);  
  builder.loadPalette("build/catwartilesreduced_palette.bin");
  builder.intializeTiles();
  tilemapController.tilePalettes = builder.tilePalette;
  tilemapController.palettes = builder.palettes;
  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {

  inputHandler.registerInputs(event);
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }
  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void *appstate) {
  eventHandler.handleEvents(&inputHandler.events);
  tilemapController.command(&eventHandler);

  mainRenderer.render(); 
  return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {}


void initializeLogging() {
  logFile.open("log.txt", std::ios::out | std::ios::app);
  logFile << std::unitbuf;
  auto *old_cout = std::cout.rdbuf(logFile.rdbuf());
  auto *old_clog = std::clog.rdbuf(logFile.rdbuf());
  auto *old_cerr = std::cerr.rdbuf(logFile.rdbuf());
  std::cout << std::unitbuf;
  std::clog << std::unitbuf;
  std::cerr << std::unitbuf;
 
}



