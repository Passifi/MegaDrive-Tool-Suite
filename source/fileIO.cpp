#include "../include/fileIO.h"
Palettes loadPalettes(std::string path) {
  std::ifstream file;
  file.open(path, std::ios::binary | std::ios::in);
  Palettes result;
  if (file) {
    while (true) {
      Palette currentPalette;
      file.read(reinterpret_cast<char *>(currentPalette.data()),
                PaletteSize * 2);
      for (int i = 0; i < currentPalette.size(); i++) {
        uint16_t value = currentPalette[i];
        currentPalette[i] = ((value & 0xff00) >> 8) | ((value & 0xff) << 8);
      }
      if (file.gcount() == PaletteSize * 2) {
        result.push_back(std::move(currentPalette));
      } else {
        if (file.gcount() != 0) {
          std::cout << "Insufficient palette data\n";
        }
        break;
      }
    }
  } else {
    std::cout << "Couldn't open file\n";
  }
  return result;
}

TileContainer loadTiles(std::string path) {
  std::ifstream file;
  TileContainer data;
  file.open(path, std::ios::binary | std::ios::in);
  if (file.is_open()) {
    while (true) {
      char current;
      Tile tile;
      file.read(reinterpret_cast<char *>(tile.data()), 32);
      if (file.gcount() == TileSize) {
        data.push_back(std::move(tile));
      } else {
        if (file.gcount() != 0) {
          std::cout << "Incomplete tile at the end of the file\n";
        }
        break;
      }
    }
  } else {
    std::cout << "Couldn't open file\n";
  }
  return data;
}
