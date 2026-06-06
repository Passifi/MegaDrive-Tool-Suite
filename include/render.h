#pragma once 
#include "SDL3/SDL.h"
#include <iostream>
#include <string>
#include "Tiledata.h"
class Renderer {
  int screenWidth;
  int screenHeight;
  float scaleX,scaleY;
  std::unique_ptr<SDL_Renderer> renderer = nullptr; 

  void intialize();
  SDL_Renderer* getCurrentRenderer();
};
void intializeRender(int screenW, int screenH);
void renderTiles(SDL_Renderer *renderer,Tilemap* map);
void renderTileSelection(SDL_Renderer *renderer, TileSelection &selection,
                         SDL_FRect dimensions);

void renderMetaTileSelection(SDL_Renderer *renderer,MetaSelector& selector ,SDL_FRect dimensions);
void renderMetatile(SDL_Renderer* renderer,Tilemap* map,MetaTile* tile,int x, int y);
void renderInfo(SDL_Renderer *renderer, int x, int y);
