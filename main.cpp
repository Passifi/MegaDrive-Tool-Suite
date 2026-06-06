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

struct MouseState {
  bool mouseDown = false;
  bool dragging = false;
  SDL_FPoint startPosition;
};
int ControlState = 0;
MouseState mouseState;
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static float mouseX, mouseY = 4.0;
static int currentTile = 0;
static int screenWidth = 800;
static int screenHeight = 600;
static int horizontalTiles = screenWidth / TILE_SIZE;
static int verticalTiles = screenHeight / TILE_SIZE;
static Tilemap* map=nullptr;
static CursorSettings cursorSettings;
static bool mousedown = false;
static SDL_FRect selectionRect = {0, 0, 8, 8};
static std::ofstream logFile;
static bool isOverTileSelection = false;
static size_t metaIndex = 0;
class TileMapController {
  Tilemap* map = nullptr;
  size_t currentTile;
  int tileState= 0;
  bool isEmpty();
  void initMap() {
    map = initializeMap(screenWidth/TILE_SIZE, screenHeight/TILE_SIZE);
  }
  int getTileValueAt(int x, int y) {
    if(x + y*map->width < map->data.size()) {
      return map->data[x+y*map->width];
    }
    else {
      return NO_TILE;
    }
  }
  SDL_Texture* getSelectedTileTexture() {
    return map->tiles[this->currentTile];
  }
  
  void setTileAt(int metadata,int x, int y) {
      if(x + y*map->width < map->data.size()) {
        return;
      } 
      auto val = currentTile|metadata;
      map->data[x+y*map->width] = val;
  }

  void choseNextTile() {
    currentTile++;
    if(map->tiles.size() <= currentTile ) {
      currentTile = 0;
    }
  }
  void chosePreviousTile() {
    if(currentTile == 0) {
      currentTile = map->tiles.size() -1;
    }
    else {
      currentTile--;
    }
  }
};

void fill(int startPosition);
void checkSelection(float rx, float ry);
SDL_FRect processInputs(float rx, float ry);
MetaTile createMetaTile();
void setMetaTile(int id,int x, int y );
MetaSelector metaSelector;
SDL_FRect tileSelectionRect = {(float)400, 000, 80, (float)screenHeight};
static TileSelection possibleTiles;

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {
  std::string filePath = "build/catwartilesreduced.bin";
  if (argc > 1) {
    filePath = argv[1];
  }
  logFile.open("log.txt", std::ios::out | std::ios::app);
  logFile << std::unitbuf;
  auto *old_cout = std::cout.rdbuf(logFile.rdbuf());
  auto *old_clog = std::clog.rdbuf(logFile.rdbuf());
  auto *old_cerr = std::cerr.rdbuf(logFile.rdbuf());
  std::cout << std::unitbuf;
  std::clog << std::unitbuf;
  std::cerr << std::unitbuf;
  if (!SDL_CreateWindowAndRenderer("Hello World", screenWidth, screenHeight,
                                   SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
    SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  map =  initializeMap(screenWidth/8,screenHeight/8);
  
  intializeRender(screenWidth, screenHeight);
  TileContainer container = loadTiles(filePath);

  Palettes palettes = loadPalettes("build/catwartilesreduced_palette.bin");

  for (auto el : container) {
    auto sur = createTileFromBinaryData(el, palettes.front());
    map->tiles.push_back(SDL_CreateTextureFromSurface(renderer, sur));
    possibleTiles.tileTextures.push_back(map->tiles.back());
  }
  possibleTiles.tiles = container;
  return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {
  static float rx, ry;
  if (event->type == SDL_EVENT_KEY_DOWN) {
    if (event->key.key == SDLK_ESCAPE) {
      return SDL_APP_SUCCESS;
    } else if (event->key.key == SDLK_LEFT) {
      if(cursorSettings.mode == DrawMeta) {
        metaSelector.incrementIndex();
      } 
      else {
      if (currentTile > 0)
        currentTile--;
      }
    } else if (event->key.key == SDLK_RIGHT) {
      if(cursorSettings.mode == DrawMeta) {
        metaSelector.decerementIndex();
      }
      else {
      currentTile++;
      if (currentTile >= map->tiles.size()) {
        currentTile = map->tiles.size() - 1;
      }}
    } else if (event->key.key == SDLK_F) {
        cursorSettings.mode = Fill; 
      
    } else if (event->key.key == SDLK_F1) {
      cursorSettings.mode = Draw;
    } else if (event->key.key == SDLK_F2) {
      cursorSettings.mode = Select;
    }
    else if(event->key.key == SDLK_F3) {
      cursorSettings.mode = DrawMeta;
    }
    else if (event->key.key == SDLK_X) {
      ControlState ^= Flip_Horizontally;
    }
    else if (event->key.key == SDLK_Z) {
      ControlState ^= Flip_Vertically;
    }
    else if(event->key.key == SDLK_S) {
      saveTilemap(*map,"maptest.bin" );
    }
    else if(event->key.key == SDLK_M) {
      createMetaTile();
    }
    else if(event->key.key == SDLK_T) {
      setMetaTile(0, 0, 0);
    }
  }

  if (event->type == SDL_EVENT_QUIT) {
    return SDL_APP_SUCCESS; /* end the program, reporting success to the OS. */
  }
  if (event->type == SDL_EVENT_MOUSE_MOTION) {
    SDL_GetMouseState(&mouseX, &mouseY);
    if (!mousedown) {
      SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
    } else {
      mouseState.dragging = true;
    }
  }
  if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
    mousedown = true;
    mouseState.mouseDown = true;
    SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
    mouseState.startPosition = {rx, ry};
    selectionRect.x = (int)(rx/TILE_SIZE)*TILE_SIZE;
    selectionRect.y = (int)(ry/TILE_SIZE)*TILE_SIZE;
    }
  if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    mousedown = false;
    mouseState.mouseDown = false;
    mouseState.dragging = false;
  }
  return SDL_APP_CONTINUE;
}


SDL_AppResult SDL_AppIterate(void *appstate) {
  int w = 0, h = 0;
  float x, y;
  const float scale = 4.0f;
  static float rx, ry;
  static SDL_FRect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_SetRenderScale(renderer, 4.0, 4.0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
  auto org_dest = processInputs(rx, ry);
  auto dst_rect = org_dest;

  renderTiles(renderer,map);
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
      SDL_RenderTextureRotated(renderer, map->tiles[currentTile], &src_rect, &org_dest,0.0,nullptr,flip);
      break;
               }
    case DrawMeta: 
      renderMetatile(renderer,map,&metaSelector.getTile() ,org_dest.x/TILE_SIZE, org_dest.y/TILE_SIZE);  
      break;
    case Select:
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0x43);
    SDL_RenderRect(renderer, &selectionRect);
    renderTileSelection(renderer, possibleTiles, tileSelectionRect);
    default:
      break;
  }
  renderInfo(renderer, rx, ry);
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
    currentTile = xValue + yValue * selectionHorizontalTiles;
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
      map->data[xSet + ySet * horizontalTiles] = (currentTile|ControlState);
      break;
    case Select:
      checkSelection(rx, ry);
      break;
    case DrawMeta:
      setMetaTile(metaIndex, alignedX, alignedY);
      break;
    case Fill:
      int xPos, yPos;
      xPos = selectionRect.x / TILE_SIZE;
      yPos = selectionRect.y / TILE_SIZE;
      fill(xPos + yPos * horizontalTiles);
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
void fill(int startPosition) {
  assert(startPosition >= 0 && startPosition < map->data.size());
  std::queue<int> to_visit;
  std::set<int> visited;
  to_visit.push(startPosition);
  visited.insert(startPosition);
  int originalTile = map->data[startPosition];
  auto add = [&](int element) {
    if ((element >= 0) && (element < map->data.size()) &&
        (map->data[element] == originalTile)
        && (visited.count(element) == 0)) {
      to_visit.push(element);
      visited.insert(element);
    }
  };
  while (!to_visit.empty()) {
    int currentPosition = to_visit.front();
    to_visit.pop();
    map->data[currentPosition] = currentTile|ControlState;
    add(currentPosition + 1);
    add(currentPosition - 1);
    add(currentPosition + horizontalTiles);
    add(currentPosition - horizontalTiles);
  }
}



