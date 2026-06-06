#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
#include <memory>
#define NO_TILE -1
#define TILE_SIZE 8
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


#define ExtractRed(val) (val & 0b111) * 36
#define ExtractGreen(val) ((val & 0b11100000) >> 5) * 36
#define ExtractBlue(val) ((val & 0b111000000000) >> 9) * 36
#define SetPalette(val,palette) (val & 0x0fff) | palette
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

  void addTile(MetaTile& tile) {
    tiles.push_back(tile);
  }

};
TileMetaData getTileRenderdata(int value);

SDL_Surface *createTileFromBinaryData(Tile data, Palette palette);
Tilemap* initializeMap(size_t width, size_t height);
