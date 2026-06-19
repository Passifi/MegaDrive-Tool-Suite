#pragma once
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Tiledata.h"
using std::vector, std::array;
struct TilemapHeader {
  uint32_t numberofTiles;
  uint32_t tilemapSize;
  uint32_t noOfPalettes;
  uint32_t verticalMapSize;
  uint32_t horizontalMapSize;
  uint32_t metaTiledataSize;
  uint32_t extraHeaderInfoSize;
};
class Tiledata {
public:
  std::vector<vector<uint16_t>> palettes;
  TileContainer tiles;
};

Palettes loadPalettes(std::string path);
TileContainer loadTiles(std::string path);
void saveTilemap(Tilemap& tilemap,std::string path);
void saveTilemap(Tilemap& tilemap,std::vector<MetaTile>& metaTiles,std::string path);
void loadTilemap(Tilemap& tilemap,std::string path);
