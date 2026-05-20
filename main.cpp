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
#include <cassert>
#include <cstdlib>
#include <stdio.h>
#include <queue> 
#include <set>
#include <vector>
#include <cstdint>
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include "include/Tiledata.h"
#include "include/fileIO.h"
#include "include/ControlStructure.h"
#define TileSize 8
#define NO_TILE -1

static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static float mouseX,mouseY = 4.0;
static int currentTile = 0;
static int screenWidth = 800; 
static int screenHeight = 600;
static int horizontalTiles = screenWidth / TileSize;
static int verticalTiles = screenHeight / TileSize;
static Tilemap map(horizontalTiles,verticalTiles);
static CursorSettings cursorSettings;
static bool mousedown = false;
static SDL_FRect selectionRect = {0,0,8,8};
static std::ofstream logFile;
static bool isOverTileSelection = false;
void fill(int startPosition);
SDL_FRect tileSelectionRect = {(float)400,000,80,(float)screenHeight};
/* This function runs once at startup. */
class TileSelection {
  public:
   TileContainer tiles;
   std::vector<SDL_Texture*> tileTextures;
};

static TileSelection possibleTiles;

void renderTileSelection(SDL_Renderer* renderer, TileSelection& selection,SDL_FRect dimensions) {
 int numberOfHorizontalTiles = dimensions.w / TileSize;
 int numberOfVerticalTiles = dimensions.h / TileSize;
 int tileIndex = 0;
 SDL_FRect src_rect = {0,0,TileSize,TileSize};
 
 SDL_SetRenderDrawColor(renderer, 0 ,  0,  0,  0);
 SDL_RenderFillRect(renderer,&dimensions); 
 for (size_t y = 0; y < numberOfVerticalTiles && tileIndex < selection.tiles.size() ; y++) {
    
    for (size_t x = 0; x < numberOfHorizontalTiles && tileIndex < selection.tiles.size(); x++) {
      SDL_FRect position = {(float)(x*TileSize + dimensions.x),(float)(y*TileSize)+dimensions.y,TileSize,TileSize}; 
      SDL_RenderTexture(renderer,selection.tileTextures[tileIndex],  &src_rect, &position); 
      tileIndex++;
    }

 }
 // draw Outline
 SDL_SetRenderDrawColor(renderer,255,255,255,0);
 SDL_RenderRect(renderer,&dimensions);
}

#define ExtractRed(val) (val&0b111)*36
#define ExtractGreen(val) ((val&0b11100000)>>5)*36
#define ExtractBlue(val) ((val&0b111000000000)>>9)*36

SDL_Surface* createTileFromBinaryData(Tile data,Palette palette) {
       
  auto surface = SDL_CreateSurface(8,8, SDL_PIXELFORMAT_RGBA8888);
  for(int y = 0; y < 8; y++) {
   
    Uint8* row = (Uint8*)surface->pixels + y * surface->pitch;
    
    for(int x = 0; x < 4; x++) {
      auto byte = (uint8_t)data[y*4+x]; 
      auto colorData1 =  palette[(byte&0xf0)>>4]; 
      auto colorData2 =  palette[(byte&0xf)]; 
      uint8_t red,green,blue; 
      red =  ExtractRed(colorData1);
      green =  ExtractGreen(colorData1);
      blue =  ExtractBlue(colorData1);
      uint32_t color1 = SDL_MapSurfaceRGBA(surface, red,green,blue,0xff);
      red =  ExtractRed(colorData2);
      green =  ExtractGreen(colorData2);
      blue =  ExtractBlue(colorData2);
       
      uint32_t color2 = SDL_MapSurfaceRGBA(surface, red, green,blue,0xff);
      Uint32* pixel1 = (Uint32*)(row + x*2*4);  
      Uint32* pixel2 = (Uint32*)(row + (x*2+1)*4);  
      *pixel1 = color1; 
      *pixel2 = color2; 
      }
  } 

  return surface;

}

void initializeMap() {
    
  for(int i = 0; i < map.size; i++) {
        map.data[i] = NO_TILE; 
  }

}

void fill(int startPosition) {
  assert(startPosition >= 0 && startPosition < map.data.size());
  std::queue<int> to_visit;
  std::set<int> visited;
  to_visit.push(startPosition);
  visited.insert(startPosition);
  int originalTile = map.data[startPosition];
  auto add = [&](int element) {
    if(
                 (element >= 0 )
        && (element < map.data.size()) && 
      (map.data[element] == originalTile)

        && (visited.count(element) == 0)) 
    {
      to_visit.push(element);
      visited.insert(element);
    }
  };
  while(!to_visit.empty() ) {
  int currentPosition = to_visit.front(); 
    to_visit.pop();
    map.data[currentPosition] = currentTile;
    add(currentPosition+1);
    add(currentPosition-1);
    add(currentPosition+horizontalTiles);
    add(currentPosition-horizontalTiles);
  }
  
  
}

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
  std::string filePath = "build\\catwartilesreduced.bin";
    if(argc > 1) {
      filePath = argv[1];
    }
    logFile.open("log.txt",std::ios::out | std::ios::app);
    auto* old_cout = std::cout.rdbuf(logFile.rdbuf());
    auto* old_clog = std::clog.rdbuf(logFile.rdbuf());

    if (!SDL_CreateWindowAndRenderer("Hello World", screenWidth, screenHeight, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    initializeMap(); 
    
    TileContainer container = loadTiles(filePath);

    Palettes palettes = loadPalettes("build\\catwartilesreduced_palette.bin");
    for(auto entry : palettes) {
      for(auto c : entry) {
        std::cout << c << std::endl;
       }
    }
    for(auto el : container) {
      auto sur = createTileFromBinaryData(el,palettes.front());
      map.tiles.push_back(SDL_CreateTextureFromSurface(renderer, sur));
      possibleTiles.tileTextures.push_back(map.tiles.back());
    
    }
    possibleTiles.tiles = container;
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    static float rx,ry;
    if(event->type == SDL_EVENT_KEY_DOWN) {
      if(event->key.key == SDLK_ESCAPE) {
        return SDL_APP_SUCCESS;
      } 
      else if(event->key.key == SDLK_LEFT) {
        if(currentTile > 0) 
        currentTile--; 
      }
      else if(event->key.key == SDLK_RIGHT) {
        currentTile++;
        if(currentTile >= map.tiles.size() ) {
          currentTile = map.tiles.size()-1;
        }
      }
      else if(event->key.key == SDLK_F) {
        int xPos,yPos;
        xPos = selectionRect.x/8;
        yPos = selectionRect.y/8;
        fill(xPos + yPos*horizontalTiles);
      }
      else if(event->key.key == SDLK_F1) {
        cursorSettings.mode = Draw;
      }
      else if(event->key.key == SDLK_F2) {
        cursorSettings.mode = Select;
      }

    }

    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    if(event->type == SDL_EVENT_MOUSE_MOTION) {
      SDL_GetMouseState(&mouseX, &mouseY);
      if(!mousedown) {
        SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
      }

    }
    if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      mousedown = true;
      SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
      selectionRect.x = rx;
      selectionRect.y = ry;

    }
    if(event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
      mousedown = false;
    }
    return SDL_APP_CONTINUE;
}
void renderInfo(SDL_Renderer* renderer,int x, int y) {
  // actually want a struct or some data structure that contains all relevant info in the form of 
  // name as string, and value as pairs  
    int selectedTileNumber = 0;
    std::string xStr = std::to_string(x);
    std::string yStr = std::to_string(y);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 10.0f, 20.0f, xStr.data() );
    SDL_RenderDebugText(renderer, 10.0f, 40.0f, yStr.data() );
}

void checkSelection(float rx,float ry) {
          isOverTileSelection =
            rx > tileSelectionRect.x 
            && rx < tileSelectionRect.x + tileSelectionRect.w 
            && ry > tileSelectionRect.y 
            && ry < tileSelectionRect.y + tileSelectionRect.h;
          if(isOverTileSelection) {
            auto selectionHorizontalTiles = tileSelectionRect.w/ TileSize; 
            auto xOffset = rx - tileSelectionRect.x;
            auto yOffset = ry - tileSelectionRect.y;
            int xValue = xOffset/8;
            int yValue = yOffset/8;
            currentTile = xValue + yValue * selectionHorizontalTiles;
            cursorSettings.mode = Draw;
            mousedown = false;
            return;
          }   
          
          if(selectionRect.x + selectionRect.w < rx) 
          {
            selectionRect.w += 8;
          } 
          else if(selectionRect.x + selectionRect.w - 8 > rx) {
            if(selectionRect.w > 8) {
              selectionRect.w -= 8;
            }
          }
          else if(selectionRect.y + selectionRect.h < ry) {
            selectionRect.h += 8;
          }
          else if(selectionRect.y + selectionRect.h - 8 > ry ) {
            if(selectionRect.h > 8) {
              selectionRect.h -= 8;
            }
          }
 
}

SDL_FRect processInputs(float rx,float ry) {
    int alignedX = static_cast<int>(rx) / 8;
    float xSet = static_cast<float>(alignedX);
    int alignedY = static_cast<int>(ry) / 8;
    float ySet = static_cast<float>(alignedY);
    
    SDL_FRect result = {xSet,ySet,8,8};
    if(mousedown) {
     switch(cursorSettings.mode) {
        case Draw:
        map.data[xSet+ySet*horizontalTiles] = currentTile;
        break; 
        case Select:
          checkSelection(rx, ry);
          break;
       default:
        break;     
     }
    }
    result.x *= TileSize; 
    result.y *= TileSize; 
    return result;

}

void renderTiles(SDL_Renderer* renderer) {
   static SDL_FRect src_rect = {0,0,TileSize,TileSize};
   SDL_FRect dst_rect = {0,0,TileSize,TileSize};
   for(int y = 0; y < verticalTiles; y++) {
      
      dst_rect.y = y; 
      
      for(int x = 0; x < horizontalTiles; x++) {
        
        auto currentPos = map.data[x+y*horizontalTiles];
        if(currentPos != NO_TILE)  {
        dst_rect.x = x*TileSize;
        dst_rect.y = y*TileSize;
        SDL_RenderTexture(renderer, map.tiles[currentPos],&src_rect , &dst_rect); 
        }
      }
    }
}
SDL_AppResult SDL_AppIterate(void *appstate)
{
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;
    static float rx,ry;
    static SDL_FRect src_rect = {0,0,TileSize,TileSize};
    SDL_SetRenderScale(renderer,  4.0, 4.0); 
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    
    SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
    auto org_dest = processInputs(rx,ry);
    auto dst_rect = org_dest;

    renderTiles(renderer); 
    if(cursorSettings.mode == Draw) {
      SDL_RenderTexture(renderer, map.tiles[currentTile],&src_rect , &org_dest); 
    }
    else {

      SDL_SetRenderDrawColor(renderer, 255 ,  255,  255,  0);
      SDL_RenderRect(renderer,&selectionRect);
      renderTileSelection(renderer,possibleTiles,tileSelectionRect); 
    } 
    renderInfo(renderer,rx,ry);
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
