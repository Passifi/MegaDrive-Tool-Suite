#pragma once
#include <array>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

struct TilemapHeader {
  int numberofTiles;
  int tilemapSize;
  int noOfPalettes;
  int verticalMapSize;
  int horizontalMapSize;
};
const unsigned int PaletteSize = 16;
const unsigned int TileSize = 32;
// PaletteSize * numberOfPalettes => start of tileData, tileSize * numberOfTiles
// => startof tilemap

using std::vector, std::array;
using Tile = std::array<uint8_t, TileSize>;
using Palette = std::array<uint16_t, PaletteSize>;
using TileContainer = std::vector<Tile>;
using Palettes = std::vector<Palette>;
class Tiledata {
public:
  std::vector<vector<uint16_t>> palettes;
  TileContainer tiles;
};

Palettes loadPalettes(std::string path);
TileContainer loadTiles(std::string path);
