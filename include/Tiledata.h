#pragma once
#include <cstdint>
#include <vector>

#include "SDL3/SDL_render.h"
#include "SDL3/SDL_surface.h"
class Tilemap {
  public: 
    Tilemap(size_t width, size_t height) : width(width), height(height),size(width*height), data(width*height) {
    
    }
  size_t width,height,size;
  std::vector<uint16_t> data;
  std::vector<SDL_Texture*> tiles;
  private:
  };


