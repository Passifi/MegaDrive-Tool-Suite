#include "../include/Tiledata.h"

void MetaTile::addTile(int x, int y, int value) {
  this->subTiles.push_back({x,y,value});
}
static std::unique_ptr<Tilemap>  mainMap = nullptr;
SDL_Surface *createTileFromBinaryData(Tile data, Palette palette) {
  auto surface = SDL_CreateSurface(8, 8, SDL_PIXELFORMAT_RGBA8888);
  for (int y = 0; y < 8; y++) {
    Uint8 *row = (Uint8 *)surface->pixels + y * surface->pitch;
    for (int x = 0; x < 4; x++) {
      auto byte = (uint8_t)data[y * 4 + x];
      auto colorData1 = palette[(byte & 0xf0) >> 4];
      auto colorData2 = palette[(byte & 0xf)];
      uint8_t red, green, blue;
      red = ExtractRed(colorData1);
      green = ExtractGreen(colorData1);
      blue = ExtractBlue(colorData1);
      uint32_t color1 = SDL_MapSurfaceRGBA(surface, red, green, blue, 0xff);
      red = ExtractRed(colorData2);
      green = ExtractGreen(colorData2);
      blue = ExtractBlue(colorData2);
      uint32_t color2 = SDL_MapSurfaceRGBA(surface, red, green, blue, 0xff);
      Uint32 *pixel1 = (Uint32 *)(row + x * 2 * 4);
      Uint32 *pixel2 = (Uint32 *)(row + (x * 2 + 1) * 4);
      *pixel1 = color1;
      *pixel2 = color2;
    }
  }
  return surface;
}

Tilemap* initializeMap(size_t width, size_t height) {
}

TileMetaData getTileRenderdata(int value) { // change so that all extracted info is returned
  TileMetaData metaData; 
  SDL_FlipMode flip = SDL_FLIP_NONE;   
  if(value&Flip_Horizontally)
        {
          flip = (SDL_FlipMode)(SDL_FLIP_HORIZONTAL|flip);
        }
        if(value&Flip_Vertically) {
          flip = (SDL_FlipMode)(SDL_FLIP_VERTICAL|flip);
        }
    int tileIndex = value&0x000003ff;
    metaData.tileIndex = tileIndex;
    metaData.flipped = flip;
    metaData.priority = value&0x8000;
    metaData.paletteNo = (value&0x6000)>>13;
    return metaData; 
}

 void TilemapBuilder::addTiles(std::string path) {
    this->container = loadTiles(path);
}
  void TilemapBuilder::loadPalette(std::string path) {
    this->palettes = loadPalettes(path);
  }
  void TilemapBuilder::intializeTiles() {
     for (auto el : this->container) { // refactor into create TilePalette, ColorPalette
    auto sur = createTileFromBinaryData(el, palettes.front());
    map->tiles.push_back(SDL_CreateTextureFromSurface(renderer, sur)); 
    tilePalette.tileTextures.push_back(map->tiles.back());
  }    
  }
