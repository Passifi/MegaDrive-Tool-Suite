#pragma once 
#include "Tiledata.h"
#include "input.h"
class TilemapController {
  public: 
    std::unique_ptr<Tilemap> map = nullptr;
  size_t currentTile;
  int tileState= 0;
  int controlState; 
  int horizontalTiles,verticalTiles;
  Palettes palettes; 
  TilePalette tilePalettes; 
  bool isEmpty();
  void initMap(int screenWidth, int screenHeight);
  int getTileValueAt(int x, int y);
  SDL_Texture* getSelectedTileTexture();  
  void command(EventHandler* eventHandler); 
  void setTileAt(int x, int y);  
  void setIndex(int x,int y ); 
  void setIndex(int index); 
  void nextTile(); 
  void previousTile();
  void fill(int startPosition); 
  void setFlipHorizontal(); 
  void setFlipVertical();
};
