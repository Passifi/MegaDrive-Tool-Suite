#include "../include/render.h"
#include "SDL3/SDL_pixels.h"
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
bool Renderer::intialize(int screenWidth, int screenHeight) {
  
  if(!SDL_CreateWindowAndRenderer("Hello World", screenWidth, screenHeight,
                                   SDL_WINDOW_FULLSCREEN, &window, &renderer)) {
                                    return false;
                                   }
  this->screenWidth = screenWidth;
  this->screenHeight = screenHeight;
  this->verticalTiles = screenHeight/TILE_SIZE;
  this->horizontalTiles = screenWidth/TILE_SIZE;
  const float scale = 4.0f;
  SDL_SetRenderScale(this->renderer, 4.0, 4.0);
  return true;
}

void Renderer::intializeRender(int screenW, int screenH) {
  
}


  void Renderer::render() {
      this->clearScreen();
      this->renderTiles(nullptr);
      SDL_RenderPresent(renderer);
  }

void Renderer::clearScreen() {
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
 
}
void Renderer::renderScreen(SDL_Renderer* renderer,TilemapController* tilemapController) {
  int w = 0, h = 0;
  float x, y;
  const float scale = 4.0f;
  static SDL_FRect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_SetRenderScale(renderer, 4.0, 4.0);
  SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
  SDL_RenderClear(renderer);
}

void Renderer::renderTiles(TilemapController* tilemapController) { // interpreteing and processing all this shoudln't be the renderes job. rather 
  // have  the  tilemapController spit out a list of tiles to render and then just iteratore over that?
   
  try {
  static SDL_FRect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_FRect dst_rect = {0, 0, TILE_SIZE, TILE_SIZE};
  for (int y = 0; y < verticalTiles; y++) {
    for (int x = 0; x < horizontalTiles; x++) {
      auto currentPos = tilemapController->map.get()->data[x + y * horizontalTiles];
      if (currentPos != NO_TILE) {
        dst_rect.x = x * TILE_SIZE;
        dst_rect.y = y * TILE_SIZE;
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
        SDL_RenderTextureRotated(renderer, tilemapController->getTextureOfTileAtIndex(tileIndex), &src_rect,
                          &dst_rect,0.0,nullptr,flip);
      }
    }
  }
  }
  catch(const std::exception& ex) {
    std::cout << "Something went wrong in render Tiles" << std::endl;
  }
}


void Renderer::renderInfo(SDL_Renderer *renderer, int x, int y) {
  int selectedTileNumber = 0;
  std::string xStr = std::to_string(x);
  std::string yStr = std::to_string(y);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, 10.0f, 20.0f, xStr.data());
  SDL_RenderDebugText(renderer, 10.0f, 40.0f, yStr.data());
}

