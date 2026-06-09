#include "../include/fileIO.h"
#include <cstdint>
const uint16_t versionNo = 1;
struct TilemapFile {
  char tiles[4] = {'t','i','l','e'};
  uint16_t versionNo; 
  uint32_t size_referenceTiles;
  uint32_t size_palettes;
  uint32_t tilemapWidth;
  uint32_t tilemapHeight;
  uint32_t metatiledata;
  uint32_t extraHeaderInfo;
};


void loadTilemap(Tilemap& tilemap,std::string path) {
  TilemapFile metaData;
  std::ifstream file;
  file.open(path,std::ios::binary | std::ios::in);
  if(file) {
    std::string basicData;
    basicData.resize(5);
    file.read(&basicData[0],4);
    basicData[4] = '\0';
    if(basicData != "Tile") return;
    file.read(reinterpret_cast<char*>(&metaData.versionNo),sizeof(metaData.versionNo));    
    file.read(reinterpret_cast<char*>(&metaData.size_referenceTiles),sizeof(metaData.size_referenceTiles));    
    file.read(reinterpret_cast<char*>(&metaData.size_palettes),sizeof(metaData.size_palettes));    
    file.read(reinterpret_cast<char*>(&metaData.tilemapWidth),sizeof(metaData.tilemapWidth));    
    file.read(reinterpret_cast<char*>(&metaData.tilemapHeight),sizeof(metaData.tilemapHeight));    
    file.read(reinterpret_cast<char*>(&metaData.metatiledata),sizeof(metaData.metatiledata));    
    file.read(reinterpret_cast<char*>(&metaData.extraHeaderInfo),sizeof(metaData.extraHeaderInfo));    
    
    if(metaData.size_referenceTiles > 0) {
      std::vector<Tile> tiles;  
      tiles.resize(sizeof(Tile)*(metaData.size_referenceTiles/TileSize));
     
      int index = 0;
      while(index*TileSize < metaData.size_referenceTiles)  {
        file.read(reinterpret_cast<char*>(tiles[index].data()),tiles[index].size());
        index++; 
      }
    }

  }
}

void saveTilemap(Tilemap& tilemap, std::string path) {
  std::ofstream file;
  file.open(path,std::ios::binary |std::ios::out);
  if(file) {
    char chardata[] = "Tile";
    uint32_t sizeTiles = 0;
    uint32_t sizePalettes = 0;
    uint32_t width = static_cast<uint32_t>(tilemap.width);
    uint32_t height = static_cast<uint32_t>(tilemap.height);
    uint32_t metatiledata= 0;
    uint32_t extraHeaderInfo = 0;

    file.write(reinterpret_cast<const char*>(&chardata),5);
    file.write(reinterpret_cast<const char*>(&versionNo),sizeof(versionNo));    
    file.write(reinterpret_cast<const char*>(&sizeTiles),sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&sizePalettes),sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&width),sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&height),sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&metatiledata),sizeof(uint32_t));
    file.write(reinterpret_cast<const char*>(&extraHeaderInfo),sizeof(uint32_t));
    for(auto& el : tilemap.data) {
      uint16_t tileID = static_cast<uint16_t>(el+1);
      file.write(reinterpret_cast<const char*>(&tileID),2);
    }  
  }
  file.close();
}
void saveTilemap(Tilemap& tilemap,std::vector<MetaTile>& metaTiles, std::string path) {
  std::ofstream file;
  file.open(path,std::ios::binary |std::ios::out);
  if(file) {
    char chardata[] = "Tile";
    uint32_t sizeTiles = 0;
    uint32_t sizePalettes = 0;
    uint32_t width = static_cast<uint32_t>(tilemap.width);
    uint32_t height = static_cast<uint32_t>(tilemap.height);
    uint32_t metatiledata= metaTiles.size();
    uint32_t extraHeaderInfo = 0;
    file.write(reinterpret_cast<const char*>(&chardata),5);
    file.write(reinterpret_cast<const char*>(&sizeTiles),4);
    file.write(reinterpret_cast<const char*>(&sizePalettes),4);
    file.write(reinterpret_cast<const char*>(&width),4);
    file.write(reinterpret_cast<const char*>(&height),4);
    file.write(reinterpret_cast<const char*>(&metatiledata),4);
    file.write(reinterpret_cast<const char*>(&extraHeaderInfo),4);
    for(auto& el : tilemap.data) {
      uint16_t tileID = static_cast<uint16_t>(el+1);
      file.write(reinterpret_cast<const char*>(&tileID),2);
    }  
    for(auto& el : metaTiles) {
    for(auto& subEl: el.subTiles) {

    
      uint32_t value = static_cast<uint16_t>(subEl.value);
      uint16_t x,y;
      x = subEl.x;
      y = subEl.y;
      file.write(reinterpret_cast<const char*>(&value),4);
      file.write(reinterpret_cast<const char*>(&x),2);
      file.write(reinterpret_cast<const char*>(&y),2);

    }
    } 
  }
  file.close();
}

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
