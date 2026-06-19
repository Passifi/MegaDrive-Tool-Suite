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
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>

int ControlState = 0;
MouseState mouseState;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static Renderer mainRenderer;
static float mouseX, mouseY = 4.0;
static int screenWidth = 800;
static int screenHeight = 600;
static int horizontalTiles = screenWidth / TILE_SIZE;
static int verticalTiles = screenHeight / TILE_SIZE;
TilemapController tilemapController;
static Tilemap* map=nullptr;
static SDL_FRect selectionRect = {0, 0, 8, 8};
static std::ofstream logFile;
static bool isOverTileSelection = false;
void checkSelection(float rx, float ry);
SDL_FRect processInputs(float rx, float ry);
MetaTile createMetaTile();
void setMetaTile(int id,int x, int y );
SDL_AppResult handleKeyDown(SDL_Event* event);
MetaSelector metaSelector;
SDL_FRect tileSelectionRect = {(float)400, 000, 80, (float)screenHeight};
static TileSelection possibleTiles;
Renderer mainRenderer;
TilemapBuilder tilemapBuilder;
void initializeLogging();

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  std::string filePath = "build/catwartilesreduced.bin";
  if (argc > 1) {
    filePath = argv[1];
  }
  initializeLogging();
  if (!SDL_CreateWindowAndRenderer("Hello World", screenWidth, screenHeight,
                                   SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  mainRenderer.intialize(renderer,screenWidth,screenHeight);
  tilemapController.initMap(screenWidth,screenHeight); 
  mainRenderer.intialize(renderer,screenWidth,screenHeight);
  TilemapBuilder builder(renderer,tilemapController);
  builder.addTiles(filePath);  
  builder.loadPalette("build/catwartilesreduced_palette.bin");
  builder.intializeTiles();
  tilemapController.tilePalettes = builder.tilePalette;
  tilemapController.palettes = builder.palettes;
  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  static float rx, ry;
  if (event->type == SDL_EVENT_KEY_DOWN) {
      return handleKeyDown(event);
    }
  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }
  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    SDL_GetMouseState(&mouseX, &mouseY);
    if (!mouseState.mouseDown) {
      SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
    } else {
      mouseState.dragging = true;
    }
  }
  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    
    SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
    mouseState.mouseDown = true;
    mouseState.startPosition = {rx, ry};
    selectionRect.x = (int)(rx/TILE_SIZE)*TILE_SIZE;
    selectionRect.y = (int)(ry/TILE_SIZE)*TILE_SIZE;
    }
  if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    mouseState.mouseDown = false;
    mouseState.dragging = false;
  }
  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void *appstate) {
  static float rx, ry;
  int w = 0, h = 0;
  float x, y;
  static SDL_FRect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
  auto org_dest = processInputs(rx, ry);
  auto dst_rect = org_dest;
  mainRenderer.renderTiles(tilemapController.map.get());
  //inputHandler. 
  switch(cursorSettings.mode) {
    case Draw:
    case Fill:{
      SDL_FlipMode flip = SDL_FLIP_NONE;
      if(ControlState&Flip_Horizontally) {
        flip = SDL_FLIP_HORIZONTAL;
      }
      if(ControlState&Flip_Vertically) {
        flip = (SDL_FlipMode)(SDL_FLIP_VERTICAL|flip);
      }
      SDL_RenderTextureRotated(renderer, tilemapController.getSelectedTileTexture(), &src_rect, &org_dest,0.0,nullptr,flip);
      break;
               }
    case DrawMeta: 
      //renderMetatile(renderer,map,&metaSelector.getTile() ,org_dest.x/TILE_SIZE, org_dest.y/TILE_SIZE);  
      break;
    case Select:
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0x43);
    SDL_RenderRect(renderer, &selectionRect);
    mainRenderer.renderTileSelection(renderer, possibleTiles, tileSelectionRect);
    case SelectMeta:
      //renderMetaTileSelection(renderer, metaSelector,nullptr, tileSelectionRect);
    default:
      break;
  }
  mainRenderer.renderInfo(renderer, rx, ry);
  SDL_RenderPresent(renderer);
  return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {}


void checkSelection(float rx, float ry) {
  isOverTileSelection = rx > tileSelectionRect.x &&
                        rx < tileSelectionRect.x + tileSelectionRect.w &&
                        ry > tileSelectionRect.y &&
                        ry < tileSelectionRect.y + tileSelectionRect.h;
  if (isOverTileSelection) {
    auto selectionHorizontalTiles = tileSelectionRect.w / TILE_SIZE;
    auto xOffset = rx - tileSelectionRect.x;
    auto yOffset = ry - tileSelectionRect.y;
    int xValue = xOffset / TILE_SIZE;
    int yValue = yOffset / TILE_SIZE;
    tilemapController.setTileAt(xValue, yValue);  
    cursorSettings.mode = Draw;
    return;
  }

  if (selectionRect.x + selectionRect.w < rx) {
    selectionRect.w += TILE_SIZE;
  } else if (selectionRect.x + selectionRect.w - TILE_SIZE > rx) {
    if (selectionRect.w > TILE_SIZE) {
      selectionRect.w -= TILE_SIZE;
    }
  } else if (selectionRect.y + selectionRect.h < ry) {
    selectionRect.h += TILE_SIZE;
  } else if (selectionRect.y + selectionRect.h - TILE_SIZE > ry) {
    if (selectionRect.h > TILE_SIZE) {
      selectionRect.h -= TILE_SIZE;
    }
  }
}

SDL_FRect processInputs(float rx, float ry) {
  int alignedX = static_cast<int>(rx) / TILE_SIZE;
  float xSet = static_cast<float>(alignedX);
  int alignedY = static_cast<int>(ry) / TILE_SIZE;
  float ySet = static_cast<float>(alignedY);

  SDL_FRect result = {xSet, ySet, TILE_SIZE, TILE_SIZE};
  if (mouseState.mouseDown) {
    switch (cursorSettings.mode) {
    case Draw:
      tilemapController.setTileAt(xSet,ySet);
      break;
    case Select:
      checkSelection(rx, ry);
      break;
    case DrawMeta:
      //setMetaTile(metaIndex, alignedX, alignedY);
      break;
    case Fill:
      int xPos, yPos;
      xPos = selectionRect.x / TILE_SIZE;
      yPos = selectionRect.y / TILE_SIZE;
      tilemapController.fill(xPos + yPos * horizontalTiles);
    default:
      break;
    }
  }
  result.x *= TILE_SIZE;
  result.y *= TILE_SIZE;
  return result;
}
MetaTile createMetaTile() {
  int xBase = selectionRect.x / TILE_SIZE;
  int yBase = selectionRect.y / TILE_SIZE;
  int xSteps = selectionRect.w / TILE_SIZE;
  int ySteps = selectionRect.h / TILE_SIZE;
  MetaTile currentTile; 
  for(int y = 0; y < ySteps; y++) {
    for(int x = 0; x < xSteps; x++) {
      int value = map->data[x+xBase + (y+yBase) * map->width];
      currentTile.addTile(x, y, value);
    }
  }
  metaSelector.addTile(currentTile);
  return currentTile;
}
void setMetaTile(int id,int x, int y ) {
  auto tile = metaSelector.getTile();
  for(auto& el : tile.subTiles)  {
    if(el.value == NO_TILE) {
      continue;
    }
    int currentX = (selectionRect.x/TILE_SIZE) + el.x; 
    int currentY = (selectionRect.y/TILE_SIZE) + el.y; 
    if(currentX >= map->width || currentY >= map->height || (currentX + currentY*map->width) >= map->data.size()) {
      continue;
    }
    map->data[currentX + currentY*map->width] = el.value;
  }
}

SDL_AppResult handleKeyDown(SDL_Event* event) {
  
    if (event->key.key == SDLK_ESCAPE) {
      return SDL_APP_SUCCESS;
    } 
    else if (event->key.key == SDLK_LEFT) {
      if(cursorSettings.mode == DrawMeta) {
          metaSelector.decerementIndex();
      } 
      else {
        tilemapController.previousTile(); 
      }
    }
    else if (event->key.key == SDLK_RIGHT) {
      if(cursorSettings.mode == DrawMeta) {

        metaSelector.incrementIndex();
      } 
      else {
        tilemapController.nextTile();
      }
    
    } else if (event->key.key == SDLK_F) {
        cursorSettings.mode = Fill; 
      
    } else if (event->key.key == SDLK_F1) {
      cursorSettings.mode = Draw;
    } else if (event->key.key == SDLK_F2) {
      cursorSettings.mode = Select;
    } else if(event->key.key == SDLK_F3) {
      cursorSettings.mode = DrawMeta;
    } else if(event->key.key == SDLK_F4) {
      cursorSettings.mode = SelectMeta;
    } else if (event->key.key == SDLK_X) {
      tilemapController.setFlipVertical();
    } else if (event->key.key == SDLK_Z) {
      tilemapController.setFlipHorizontal();
    } else if(event->key.key == SDLK_S) {
      saveTilemap(*map,"maptest.bin" );
    } else if(event->key.key == SDLK_M) {
      createMetaTile();
    } else if(event->key.key == SDLK_T) {
      setMetaTile(0, 0, 0);
    }
    return SDL_APP_SUCCESS;
}

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



