#include <exception>
#include <array>
#include <fstream> 
#include <iostream> 
#include <string>
#include <vector> 
#include <cstdint>

struct TilemapHeader {
  int numberofTiles;
  int tilemapSize;
  int noOfPalettes; 
  int verticalMapSize;
  int horizontalMapSize;
};
const unsigned int PaletteSize = 16;
const unsigned int TileSize = 32;
// PaletteSize * numberOfPalettes => start of tileData, tileSize * numberOfTiles => startof tilemap  

using std::vector,std::array; 
using Tile = std::array<uint8_t,TileSize>;
using Palette = std::array<uint32_t,PaletteSize>;
using TileContainer = std::vector<Tile>;
using Palettes = std::vector<Palette>;
class Tiledata {
  public:
   std::vector<vector<uint16_t>> palettes; 
  TileContainer tiles; 
};

Palettes loadPalettes(std::string path) {
  std::ifstream file;
  file.open(path,std::ios::binary | std::ios::in);
  Palettes result;
  if(file) {
    while(true) {
      Palette currentPalette;
      file.read(reinterpret_cast<char*>(currentPalette.data()),PaletteSize);
      if(file.gcount() == PaletteSize) {
        result.push_back(std::move(currentPalette));
      } 
      else {
        if(file.gcount() != 0) {
          std::cout << "Insufficient palette data\n";
        }
        break;
      }
    }
  }
  else {
    std::cout << "Couldn't open file\n";
  }
  return result;
}

TileContainer loadTiles(std::string path) {
  std::ifstream file; 
  TileContainer data;
  file.open(path,std::ios::binary | std::ios::in);
  if(file.is_open()) {
    while(true) {
      char current; 
      Tile tile;
      file.read(reinterpret_cast<char*>(tile.data()),32);
      if(file.gcount() == TileSize) {
        data.push_back(std::move(tile)); 
      } else {
        if(file.gcount() != 0) {
          std::cout << "Incomplete tile at the end of the file\n";
        }
        break;
      }
    }
  }
  else {
    std::cout << "Couldn't open file\n";
  }
  return data;
}
