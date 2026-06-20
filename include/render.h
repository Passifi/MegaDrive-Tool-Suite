#pragma once 
#include "SDL3/SDL.h"
#include "SDL3/SDL_render.h"
#include <iostream>
#include <string>
#include "Tiledata.h"

class Renderer {
  public: 
  int screenWidth; 
  int screenHeight;
  int verticalTiles; 
  int horizontalTiles;

  float scaleX,scaleY;
  SDL_Renderer* renderer= nullptr ;
  SDL_Window* window = nullptr;
  bool intialize(int screenWidth, int screenHeight);
  void render();
  void renderScreen(SDL_Renderer* renderer, Tilemap* map);
  void intializeRender(int screenW, int screenH);
  void clearScreen();
  void renderTiles(Tilemap* map);
  void renderTileSelection(SDL_Renderer *renderer, TileSelection &selection,
                           SDL_FRect dimensions);
  void renderMetaTileSelection(SDL_Renderer *renderer,MetaSelector& selector,Tilemap*map ,SDL_FRect dimensions);
  void renderMetatile(SDL_Renderer* renderer,Tilemap* map,MetaTile* tile,int x, int y);
  void renderMetatile(SDL_Renderer* renderer,Tilemap* map,MetaTile* tile,int x, int y,float scale);
  void renderInfo(SDL_Renderer *renderer, int x, int y);
  SDL_Renderer* getCurrentRenderer();
};

