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

// PaletteSize * numberOfPalettes => start of tileData, tileSize * numberOfTiles => startof tilemap  

using std::vector,std::array;
class Tiledata {
  public:
  
  private:
  std::vector<vector<uint16_t>> palettes; 
  vector<array<uint8_t,32>> tiles; 
};

vector<array<uint8_t,32>> loadTiles(std::string path) {
  std::ifstream file; 
  vector<array<uint8_t,32>> data;
  file.open(path,std::ios::binary | std::ios::in);
  if(file.is_open()) {
    while(true) {
      char current; 
      array<uint8_t,32> tile;
      file.read(reinterpret_cast<char*>(tile.data()),32);
      if(file.gcount() == 32) {
        data.push_back(std::move(tile)); 

      } else {
        if(file.gcount() != 0) {
          std::cout << "INcomplete tile at the end of the file\n";
        }
        break;
      }
    }
  }
  return data;
}
