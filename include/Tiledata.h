#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <queue> 
#include <map>
#include <set>
#include <assert.h>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include <memory>
#define NO_TILE -1
#define TILE_SIZE 8
#define ExtractRed(val) (val & 0b111) * 36
#define ExtractGreen(val) ((val & 0b11100000) >> 5) * 36
#define ExtractBlue(val) ((val & 0b111000000000) >> 9) * 36
#define SetPalette(val,palette) (val & 0x0fff) | palette

const unsigned int PaletteSize = 16;
const unsigned int TileSize = 32;

const int Flip_Horizontally = 0x0800;
const int Flip_Vertically = 0x1000;
const int PAL0 = 0x0000;
const int PAL1 = 0x2000;
const int PAL2 = 0x4000;
const int PAL3 = 0x6000;
const int Low_Priority = 0x0000 ;
const int High_Priority = 0x8000 ;
using Tile = std::array<uint8_t, TileSize>;
using Palette = std::array<uint16_t, PaletteSize>;
using TileContainer = std::vector<Tile>;
using Palettes = std::vector<Palette>;
struct TileMetaData {
  SDL_FlipMode flipped = SDL_FLIP_NONE;
  uint8_t paletteNo;
  bool priority;
  int tileIndex;
};



class TileSelection {
public:
  TileContainer tiles;
  std::vector<SDL_Texture *> tileTextures;
};

class Tilemap {
  public: 
    Tilemap(size_t width, size_t height) : width(width), height(height),size(width*height), data(width*height) {
    
    }
    ~Tilemap() {
    }
  size_t width,height,size;
  std::vector<int> data;
  std::vector<SDL_Texture*> tiles;
  private:
  };

class SubTile {
  public:
    SubTile(int x, int y, int value) : x(x), y(y), value(value) {}
  int x;
  int y;
  int value;
};
class MetaTile {
  public:
    size_t width,height;
    std::vector<SubTile> subTiles; 
    void addTile(int x, int y, int value);
    SDL_Texture* texture = nullptr;
};
class MetaSelector { 
  public: 
  std::vector<MetaTile> tiles;
  size_t metaIndex = 0;
  MetaSelector(std::vector<MetaTile>& tiles) : tiles(tiles) {
  }
  MetaSelector() {
        
  }
  bool isEmpty() {
    return tiles.size() == 0;
  }
  void incrementIndex() { 
    if(metaIndex < tiles.size()-1)
    this->metaIndex++;
    else 
      this->metaIndex = 0;
  }
  void decerementIndex() {
    if(metaIndex > 0) 
      this->metaIndex--;
    else 
      this->metaIndex = tiles.size()-1;
  }
  void setIndex(size_t position) {
    if(position < tiles.size()) {
      metaIndex = position;
    }
  }
  MetaTile& getTile() {
    return tiles[metaIndex];
  } 
  MetaTile* getTileAt(int index) {
    return &tiles[index];
  }

  void addTile(MetaTile& tile) {
    tiles.push_back(tile);
  }
 };

Tilemap* initializeMap(size_t width, size_t height);
class TileMapController {
  public: 
  Tilemap* map = nullptr;
  size_t currentTile;
  int tileState= 0;
  int controlState; 
  int horizontalTiles,verticalTiles;
  bool isEmpty();
  void initMap(int screenWidth, int screenHeight) {
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
    map->data[currentPosition] = currentTile|controlState;
    add(currentPosition + 1);
    add(currentPosition - 1);
    add(currentPosition + horizontalTiles);
    add(currentPosition - horizontalTiles);
  }
}
  void setFlipHorizontal() {
    controlState ^= Flip_Horizontally;
  }
  
  void setFlipVertical() {

    controlState ^= Flip_Vertically;
  }

  
};

TileMetaData getTileRenderdata(int value);

void fill(int startPosition,Tilemap* map);
SDL_Surface *createTileFromBinaryData(Tile data, Palette palette);
