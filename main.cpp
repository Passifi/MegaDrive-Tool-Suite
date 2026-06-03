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
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#define TileSize 8

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
static int horizontalTiles = screenWidth / TileSize;
static int verticalTiles = screenHeight / TileSize;
static Tilemap* map=nullptr;
static CursorSettings cursorSettings;
static bool mousedown = false;
static SDL_FRect selectionRect = {0, 0, 8, 8};
static std::ofstream logFile;
static bool isOverTileSelection = false;
static std::vector<MetaTile> metaTiles;
static size_t metaIndex = 0;
class TileMapController {
  Tilemap* map = nullptr;
  size_t mapIndex;
  bool isEmpty();
  void initializeMap();
  Tile& getTileAt(int x, int y);
  Tile& getSelectedTile();
  void choseNextTile();
  void chosePreviousTile();
};
class MetaSelector {  // can't guratentee that the index is right if the size of tiles changes 
  // possibly consider only exposing tile changes via the class e.g. addTile removeTile, for remove maybe too options with popping and removing a specific tile then update the index accordinlgy  
  // this would also suggest that at this point MetaSelector should own the vector completly so no reference anymore...  
  public: 
  std::vector<MetaTile> tiles;
  size_t metaIndex = 0;
  MetaSelector(std::vector<MetaTile>& tiles) : tiles(tiles) {
  }
  bool isEmpty() {
    return tiles.size() == 0;
  }
  void incrementIndex() { 
    if(metaIndex < tiles.size()-1)
    this->metaIndex++;
  }
  void decerementIndex() {
    if(metaIndex > 0) 
      this->metaIndex--;
  }
  void setIndex(size_t position) {
    if(position < tiles.size()) {
      metaIndex = position;
    }
  }
  MetaTile& getTile() {
    return tiles[metaIndex];
  }
};

void fill(int startPosition);
void createMetaTile() {
  int xBase = selectionRect.x / TileSize;
  int yBase = selectionRect.y / TileSize;
  int xSteps = selectionRect.w / TileSize;
  int ySteps = selectionRect.h / TileSize;
  MetaTile currentTile; 
  for(int y = 0; y < ySteps; y++) {
    for(int x = 0; x < xSteps; x++) {
      int value = map->data[x+xBase + (y+yBase) * map->width];
      currentTile.addTile(x, y, value);
    }
  }
  metaTiles.push_back(currentTile);
}
void setMetaTile(int id,int x, int y ) {
  for(auto& el : metaTiles[id].subTiles)  {
    int currentX = (selectionRect.x/TileSize) + el.x; 
    int currentY = (selectionRect.y/TileSize) + el.y; 
    if(currentX >= map->width || currentY >= map->height || (currentX + currentY*map->width) >= map->data.size()) {
      continue;
    }
    map->data[currentX + currentY*map->width] = el.value;
  }
}

SDL_FlipMode getTileRenderdata(int value) {
  SDL_FlipMode flip = SDL_FLIP_NONE;   
  if(value&Flip_Horizontally)
        {
          flip = (SDL_FlipMode)(SDL_FLIP_HORIZONTAL|flip);
        }
        if(value&Flip_Vertically) {
          flip = (SDL_FlipMode)(SDL_FLIP_VERTICAL|flip);
        }
        int tileIndex = value&0x000003ff;
    return flip; 
}

void renderMetatile(MetaTile* tile,int x, int y) {
  SDL_FRect src_rect = {0,0,8,8};
  SDL_FRect dst_rect = {0,0,8,8};
  for(const auto& el : tile->subTiles) {
    int currentX = x + el.x; 
    int currentY = y + el.y; 
    int value = el.value; 
    SDL_FlipMode flip = getTileRenderdata(value); // extract rendering info shoudl probably be it's own function 
    int tileIndex = value&0x000003ff;
    dst_rect.x = currentX*TileSize;
    dst_rect.y = currentY*TileSize;
    SDL_RenderTextureRotated(renderer, map->tiles[tileIndex], &src_rect, &dst_rect, 0.0,  nullptr, flip);
  }
}
SDL_FRect tileSelectionRect = {(float)400, 000, 80, (float)screenHeight};
/* This function runs once at startup. */
class TileSelection {
public:
  TileContainer tiles;
  std::vector<SDL_Texture *> tileTextures;
};

static TileSelection possibleTiles;

void renderTileSelection(SDL_Renderer *renderer, TileSelection &selection,
                         SDL_FRect dimensions) {
  int numberOfHorizontalTiles = dimensions.w / TileSize;
  int numberOfVerticalTiles = dimensions.h / TileSize;
  int tileIndex = 0;
  SDL_FRect src_rect = {0, 0, TileSize, TileSize};

  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
  SDL_RenderFillRect(renderer, &dimensions);
  for (size_t y = 0;
       y < numberOfVerticalTiles && tileIndex < selection.tiles.size(); y++) {

    for (size_t x = 0;
         x < numberOfHorizontalTiles && tileIndex < selection.tiles.size();
         x++) {
      SDL_FRect position = {(float)(x * TileSize + dimensions.x),
                            (float)(y * TileSize) + dimensions.y, TileSize,
                            TileSize};
      SDL_RenderTexture(renderer, selection.tileTextures[tileIndex], &src_rect,
                        &position);
      tileIndex++;
    }
  }
  // draw Outline
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
  SDL_RenderRect(renderer, &dimensions);
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
      if (currentTile > 0)
        currentTile--;
    } else if (event->key.key == SDLK_RIGHT) {
      currentTile++;
      if (currentTile >= map->tiles.size()) {
        currentTile = map->tiles.size() - 1;
      }
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
    selectionRect.x = (int)(rx/TileSize)*8.0f;
    selectionRect.y = (int)(ry/TileSize)*8.0f;
    }
  if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
    mousedown = false;
    mouseState.mouseDown = false;
    mouseState.dragging = false;
  }
  return SDL_APP_CONTINUE;
}
void renderInfo(SDL_Renderer *renderer, int x, int y) {
  int selectedTileNumber = 0;
  std::string xStr = std::to_string(x);
  std::string yStr = std::to_string(y);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, 10.0f, 20.0f, xStr.data());
  SDL_RenderDebugText(renderer, 10.0f, 40.0f, yStr.data());
}

void checkSelection(float rx, float ry) {
  isOverTileSelection = rx > tileSelectionRect.x &&
                        rx < tileSelectionRect.x + tileSelectionRect.w &&
                        ry > tileSelectionRect.y &&
                        ry < tileSelectionRect.y + tileSelectionRect.h;
  if (isOverTileSelection) {
    auto selectionHorizontalTiles = tileSelectionRect.w / TileSize;
    auto xOffset = rx - tileSelectionRect.x;
    auto yOffset = ry - tileSelectionRect.y;
    int xValue = xOffset / 8;
    int yValue = yOffset / 8;
    currentTile = xValue + yValue * selectionHorizontalTiles;
    cursorSettings.mode = Draw;
    return;
  }

  if (selectionRect.x + selectionRect.w < rx) {
    selectionRect.w += 8;
  } else if (selectionRect.x + selectionRect.w - 8 > rx) {
    if (selectionRect.w > 8) {
      selectionRect.w -= 8;
    }
  } else if (selectionRect.y + selectionRect.h < ry) {
    selectionRect.h += 8;
  } else if (selectionRect.y + selectionRect.h - 8 > ry) {
    if (selectionRect.h > 8) {
      selectionRect.h -= 8;
    }
  }
}

SDL_FRect processInputs(float rx, float ry) {
  int alignedX = static_cast<int>(rx) / 8;
  float xSet = static_cast<float>(alignedX);
  int alignedY = static_cast<int>(ry) / 8;
  float ySet = static_cast<float>(alignedY);

  SDL_FRect result = {xSet, ySet, 8, 8};
  if (mouseState.mouseDown) {
    switch (cursorSettings.mode) {
    case Draw:
      map->data[xSet + ySet * horizontalTiles] = (currentTile|ControlState);
      break;
    case Select:
      checkSelection(rx, ry);
      break;
    case Fill:
      int xPos, yPos;
      xPos = selectionRect.x / TileSize;
      yPos = selectionRect.y / TileSize;
      fill(xPos + yPos * horizontalTiles);
    default:
      break;
    }
  }
  result.x *= TileSize;
  result.y *= TileSize;
  return result;
}

void renderTiles(SDL_Renderer *renderer) {
  try {
  static SDL_FRect src_rect = {0, 0, TileSize, TileSize};
  SDL_FRect dst_rect = {0, 0, TileSize, TileSize};
  for (int y = 0; y < verticalTiles; y++) {
    for (int x = 0; x < horizontalTiles; x++) {
      auto currentPos = map->data[x + y * horizontalTiles];
      if (currentPos != NO_TILE) {
        dst_rect.x = x * TileSize;
        dst_rect.y = y * TileSize;
        int flipped = currentPos&0x18;
        SDL_FlipMode flip = SDL_FLIP_NONE;
        if(currentPos&Flip_Horizontally)
        {
          flip = (SDL_FlipMode)(SDL_FLIP_HORIZONTAL|flip);
        }
        if(currentPos&Flip_Vertically) {
          flip = (SDL_FlipMode)(SDL_FLIP_VERTICAL|flip);
        }
        int tileIndex = currentPos&0x000003ff;
        SDL_RenderTextureRotated(renderer, map->tiles[tileIndex], &src_rect,
                          &dst_rect,0.0,nullptr,flip);
      }
    }
  }
  }
  catch(const std::exception& ex) {
    std::cout << "Something went wrong in render Tiles" << std::endl;
  }
}
SDL_AppResult SDL_AppIterate(void *appstate) {
  int w = 0, h = 0;
  float x, y;
  const float scale = 4.0f;
  static float rx, ry;
  static SDL_FRect src_rect = {0, 0, TileSize, TileSize};
  SDL_SetRenderScale(renderer, 4.0, 4.0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
  SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
  auto org_dest = processInputs(rx, ry);
  auto dst_rect = org_dest;

  renderTiles(renderer);
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
      // renderMetatile(MetaTile* tile,int x, int y); -> just push a render command into the render pipeline and let the renderer execute it 

      break;
    case Select:
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
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
