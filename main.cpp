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
#include <cstdlib>
#include <stdio.h>
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
const size_t tilemapSize = (800/8)*(600/8);
static Tilemap map(screenWidth/TileSize,screenHeight/TileSize);
static bool tilemap[(800/8)*(600/8)];
static CursorSettings cursorSettings;
static bool mousedown = false;
static std::ofstream logFile;
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
 SDL_FRect src_rect = {0,0,8,8};
 SDL_SetRenderDrawColor(renderer, 0 ,  0,  0,  0);
 SDL_RenderFillRect(renderer,&dimensions); 
 for (size_t y = 0; y < numberOfVerticalTiles && tileIndex < selection.tiles.size() ; y++) {
  for (size_t x = 0; x < numberOfHorizontalTiles && tileIndex < selection.tiles.size(); x++) {
   
    SDL_FRect position = {(float)(x*8 + dimensions.x),(float)(y*numberOfHorizontalTiles*8)+dimensions.y,8,8}; 
    SDL_RenderTexture(renderer,selection.tileTextures[tileIndex],  &src_rect, &position); 
    tileIndex++;
  } 
 }
 SDL_SetRenderDrawColor(renderer,255,255,255,0);
 SDL_RenderRect(renderer,&dimensions);
}

SDL_Surface* createTileFromBinaryData(Tile data,Palette palette) {
       
  auto surface = SDL_CreateSurface(8,8, SDL_PIXELFORMAT_RGBA8888);
  for(int y = 0; y < 8; y++) {
   
    Uint8* row = (Uint8*)surface->pixels + y * surface->pitch;
  for(int x = 0; x < 4; x++) {
   auto byte = (uint8_t)data[y*4+x]; 
   auto colorData1 =  palette[(byte&0xf0)>>4]; 
   auto colorData2 =  palette[(byte&0xf)]; 
   uint32_t color1 = SDL_MapSurfaceRGBA(surface, ((colorData1&0b111000000)>>6)*36, ((colorData1&0b111000)>>3)*36,(colorData1&0b111)*36,0xff);
   uint32_t color2 = SDL_MapSurfaceRGBA(surface, ((colorData2&0b111000000)>>6)*36, ((colorData2&0b111000)>>3)*36,(colorData2&0b111)*36,0xff);
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

SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    logFile.open("log.txt",std::ios::out | std::ios::app);
    auto* old_cout = std::cout.rdbuf(logFile.rdbuf());
    auto* old_clog = std::clog.rdbuf(logFile.rdbuf());
    if (!SDL_CreateWindowAndRenderer("Hello World", screenWidth, screenHeight, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    initializeMap(); 
    TileContainer container = loadTiles("build\\mouse.bin");
    Palettes palettes = loadPalettes("build\\mouse_palette.bin");
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
    }
    if(event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
      mousedown = true;
    }
    if(event->type == SDL_EVENT_MOUSE_BUTTON_UP) {

      mousedown = false;
    }
    return SDL_APP_CONTINUE;
}

SDL_AppResult drawTile(SDL_Renderer* renderer, SDL_Point coord) {
 for (int yVal=0; yVal<8; yVal++) {
      for (int xVal=0; xVal< 8;xVal++) {
        SDL_SetRenderDrawColor(renderer, xVal*16, yVal*16, (xVal+yVal)*8, 255);
        SDL_RenderPoint(renderer,coord.x*TileSize+xVal, coord.y*TileSize+yVal); 
      } 
    } 
  return SDL_APP_SUCCESS; 
}
/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate)
{
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;
    
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    static float rx,ry;
    SDL_SetRenderScale(renderer,  4.0, 4.0); 
    SDL_FRect src_rect = {0,0,8,8};
    SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
    int xSet = (((int)rx)/8) ;
    int ySet = (((int)ry)/8);
    char xStr[20];
    char yStr[20];
    sprintf(xStr,"%d",xSet);
    sprintf(yStr,"%d",ySet);

    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderDebugText(renderer, 400.0f, 20.0f, xStr );
    SDL_RenderDebugText(renderer, 400.0f, 40.0f, yStr );
    
   if(mousedown ) {
     switch(cursorSettings.mode) {
        case Draw:
        map.data[xSet+ySet*100] = currentTile;
        break; 
        case Select:
        default:
        break;     
     }
    }
    xSet*=8;
    ySet*=8;

    SDL_FRect dst_rect = {(float)xSet,(float)ySet,8,8};
    for(int yPos = 0; yPos < screenHeight/8; yPos++) {
      dst_rect.y = (float)yPos; 
      for(int xPos = 0; xPos < screenWidth/8; xPos++) {
        auto currentPos = map.data[xPos+yPos*100];
        if(currentPos != NO_TILE)  {
        dst_rect.x = (float)xPos*8;
        dst_rect.y = (float)yPos*8;
        SDL_RenderTexture(renderer, map.tiles[currentPos],&src_rect , &dst_rect); 
        }
      }
    }
    dst_rect.x = (float)xSet;
    dst_rect.y = (float)ySet;
    renderTileSelection(renderer,possibleTiles,{(float)400,000,80,80});
    SDL_RenderTexture(renderer, map.tiles[currentTile],&src_rect , &dst_rect); 
    SDL_RenderPresent(renderer);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
