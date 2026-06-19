#pragma once
#include <SDL_rect.h>

struct MouseState {
  bool mouseDown = false;
  bool dragging = false;
  SDL_FPoint startPosition;
  SDL_FPoint position;
};

class InputHandler {
    InputMode inputMode = Draw;
    SDL_Renderer* renderer = nullptr;
    MouseState state;
    void calculateViewportPosition();
    void setMousedown();
    void releaseMousedown();
    void calculateViewportCoordinates();
    

    
};  



