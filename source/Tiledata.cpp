#include "../include/Tiledata.h"
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

  mainMap = std::make_unique<Tilemap>(width,height);
  for (int i = 0; i < mainMap->size; i++) {
    mainMap->data[i] = NO_TILE;
  }
  return mainMap.get();
}



