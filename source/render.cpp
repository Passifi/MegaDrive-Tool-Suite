#include "../include/render.h"
#include "SDL3/SDL_render.h"
static int screenWidth; 
static int screenHeight;
static int verticalTiles; 
static int horizontalTiles;

void intializeRender(int screenW, int screenH) {
  screenWidth = screenW;
  screenHeight = screenH;
  verticalTiles = screenW/TILE_SIZE;
  horizontalTiles = screenH/TILE_SIZE;
}
void renderTiles(SDL_Renderer *renderer,Tilemap* map) {
  try {
  static SDL_FRect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};
  SDL_FRect dst_rect = {0, 0, TILE_SIZE, TILE_SIZE};
  for (int y = 0; y < verticalTiles; y++) {
    for (int x = 0; x < horizontalTiles; x++) {
      auto currentPos = map->data[x + y * horizontalTiles];
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


void renderTileSelection(SDL_Renderer *renderer, TileSelection &selection,
                         SDL_FRect dimensions) {
  int numberOfHorizontalTiles = dimensions.w / TILE_SIZE;
  int numberOfVerticalTiles = dimensions.h / TILE_SIZE;
  int tileIndex = 0;
  SDL_FRect src_rect = {0, 0, TILE_SIZE, TILE_SIZE};

  SDL_SetRenderDrawColor(renderer, 0xff, 0xff, 0xff,0x23 );
  SDL_RenderFillRect(renderer, &dimensions);
  for (size_t y = 0;
       y < numberOfVerticalTiles && tileIndex < selection.tiles.size(); y++) {

    for (size_t x = 0;
         x < numberOfHorizontalTiles && tileIndex < selection.tiles.size();
         x++) {
      SDL_FRect position = {(float)(x * TILE_SIZE + dimensions.x),
                            (float)(y * TILE_SIZE) + dimensions.y, TILE_SIZE,
                            TILE_SIZE};
      SDL_RenderTexture(renderer, selection.tileTextures[tileIndex], &src_rect,
                        &position);
      tileIndex++;
    }
  }
  // draw Outline
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 0);
  SDL_RenderRect(renderer, &dimensions);
}
// best approach would probably be to get an SDL_texture from the metatile and dispaly that, this way you can actually scale stuffs

void renderMetaTileSelection(SDL_Renderer *renderer,MetaSelector& selector ,SDL_FRect dimensions) {
  SDL_SetRenderDrawColor(renderer, 0xf, 0xf, 0xf, 0x33);
  SDL_RenderFillRect(renderer,&dimensions );
  int currentX = 0;
  int currentY = 0;
  for(auto& el : selector.tiles) {
    auto widht = el.width;
    auto height = el.height; 
    for(auto& subTile : el.subTiles) {
      SDL_FRect position = {(float)(currentX + subTile.x),(float)(currentY + subTile.y),TILE_SIZE,TILE_SIZE};
    }
  }
}

void renderMetatile(SDL_Renderer* renderer,Tilemap* map,MetaTile* tile,int x, int y) {
  SDL_FRect src_rect = {0,0,8,8};
  SDL_FRect dst_rect = {0,0,8,8};
  for(const auto& el : tile->subTiles) {

    int value = el.value; 
    
    if(value == NO_TILE) continue;  
    int currentX = x + el.x; 
    int currentY = y + el.y; 
    auto metadata = getTileRenderdata(value); // extract rendering info shoudl probably be it's own function 
    dst_rect.x = currentX*TILE_SIZE;
    dst_rect.y = currentY*TILE_SIZE;
    SDL_RenderTextureRotated(renderer, map->tiles[metadata.tileIndex], &src_rect, &dst_rect, 0.0,  nullptr, metadata.flipped);
  }
}
void renderInfo(SDL_Renderer *renderer, int x, int y) {
  int selectedTileNumber = 0;
  std::string xStr = std::to_string(x);
  std::string yStr = std::to_string(y);
  SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
  SDL_RenderDebugText(renderer, 10.0f, 20.0f, xStr.data());
  SDL_RenderDebugText(renderer, 10.0f, 40.0f, yStr.data());
}

