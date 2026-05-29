#pragma once
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Tiledata.h"
struct TilemapHeader {
  int numberofTiles;
  int tilemapSize;
  int noOfPalettes;
  int verticalMapSize;
  int horizontalMapSize;
};
// PaletteSize * numberOfPalettes => start of tileData, tileSize * numberOfTiles
// => startof tilemap

using std::vector, std::array;
class Tiledata {
public:
  std::vector<vector<uint16_t>> palettes;
  TileContainer tiles;
};

Palettes loadPalettes(std::string path);
TileContainer loadTiles(std::string path);
void saveTilemap(Tilemap& tilemap,std::string path);
void loadTilemap(Tilemap& tilemap,std::string path);
