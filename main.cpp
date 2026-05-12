#include "SDL3/SDL_events.h"
#include "SDL3/SDL_init.h"
#include "SDL3/SDL_mouse.h"
#include "SDL3/SDL_oldnames.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_stdinc.h"
#include "SDL3/SDL_surface.h"
#include <cstdint>
#define SDL_MAIN_USE_CALLBACKS 1  /* use the callbacks instead of main() */
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#define TileSize 8
static SDL_Window *window = NULL;
static SDL_Renderer *renderer = NULL;
static float mouseX,mouseY = 4.0;
static int screenWidth = 800; 
static int screenHeight = 600;
const size_t tilemapSize = (800/8)*(600/8);
static bool tilemap[(800/8)*(600/8)];
/* This function runs once at startup. */
SDL_Surface* createTile(int width, int height ) {
  auto surface = SDL_CreateSurface(8,8, SDL_PIXELFORMAT_RGBA8888);
  if(!surface) return NULL;
  SDL_LockSurface(surface);
  Uint32 color = SDL_MapSurfaceRGBA(surface,  0xff, 0xf2, 0x33,  0xff);
  for (int y=0; y<8; y++) {
      Uint8* row = (Uint8*)surface->pixels + y * surface->pitch;
      for (int x=0; x< 8;x++) {
        Uint32* pixel = (Uint32*)(row + x * 4);
        *pixel = color;
        
      } 
    } 
  SDL_UnlockSurface(surface);
  return surface;
}
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[])
{
    /* Create the window */
    if (!SDL_CreateWindowAndRenderer("Hello World", screenWidth, screenHeight, SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
        SDL_Log("Couldn't create window and renderer: %s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    for(int i = 0; i < tilemapSize; i++) {
      if(i % 2 == 0) {
        tilemap[i] = true;
      }
      else 
      {
        tilemap[i] = false;
      }
    }
    return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event)
{
    if (event->type == SDL_EVENT_KEY_DOWN ||
        event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  /* end the program, reporting success to the OS. */
    }
    if(event->type == SDL_EVENT_MOUSE_MOTION) {
      SDL_GetMouseState(&mouseX, &mouseY);
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
    const char *message = "Hello World!";
    int w = 0, h = 0;
    float x, y;
    const float scale = 4.0f;

    /* Center the message and scale it up */
    SDL_GetRenderOutputSize(renderer, &w, &h);
    SDL_SetRenderScale(renderer, scale, scale);
    x = ((w / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE * SDL_strlen(message)) / 2;
    y = ((h / scale) - SDL_DEBUG_TEXT_FONT_CHARACTER_SIZE) / 2;
    
    /* Draw the message */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    static float rx,ry;
    auto surface = createTile(8, 8); 
    SDL_FRect src_rect = {0,0,8,8};
    SDL_RenderCoordinatesFromWindow(renderer, mouseX, mouseY, &rx, &ry);
    auto text = SDL_CreateTextureFromSurface(renderer, surface);
    int xSet = (((int)rx)/8)*8; 
    int ySet = (((int)ry)/8)*8; 
    SDL_FRect dst_rect = {(float)xSet,(float)ySet,8,8};

    SDL_RenderTexture(renderer, text,&src_rect , &dst_rect); 
    for(int yPos = 0; yPos < screenHeight/8; yPos++) {
      dst_rect.y = (float)yPos; 

      for(int xPos = 0; xPos < screenWidth/8; xPos++) {
        if(tilemap[xPos+yPos])  {
        dst_rect.x = (float)xPos*8;
        dst_rect.y = (float)yPos*8;
        SDL_RenderTexture(renderer, text,&src_rect , &dst_rect); 
        }
      }
   }
    SDL_RenderPresent(renderer);
    SDL_DestroySurface(surface);
    return SDL_APP_CONTINUE;
}

/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result)
{
}
