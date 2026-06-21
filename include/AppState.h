#pragma once 
#include "ControlStructure.h"
#include "fileIO.h"
#include "input.h"
#include "render.h"
#include "Tiledata.h" 
#include "TilemapController.h"

class App {
  public: 
  Renderer renderer;
  TilemapController tilemapController;
  InputHandler inputHandler;
  EventHandler eventHandler; 
  InputMode mode; 
  void iterate() {
    eventHandler.handleEvents(&inputHandler.events);
    float x,y; 
    switch(mode) {
      case Draw: 
        SDL_RenderCoordinatesFromWindow(renderer.renderer,eventHandler.mousestate.position.x,eventHandler.mousestate.position.y,&x,&y);
        if(eventHandler.mousestate.mouseDown) {
          tilemapController.setTileAt(x,y);
        }
        break;
      case Select:
        break;
      case Erase:
        if(eventHandler.mousestate.mouseDown) {
          //tilemapController.deleteTileIDat(x,y);
        }
      break;
    }
    if(eventHandler.keyMap[SDLK_LEFT]) {
      tilemapController.previousTile();
    }
    if(eventHandler.keyMap[SDLK_RIGHT]) {
      tilemapController.nextTile();
    }
    if(eventHandler.keyMap[SDLK_S]) {
      //save tileMap
    }
    if(eventHandler.keyMap[SDLK_F1]) {
      mode = InputMode::Draw;
    }
    if(eventHandler.keyMap[SDLK_F2]) {
      mode = InputMode::Select;
    }
    if(eventHandler.keyMap[SDLK_F3]) {
      mode = InputMode::Erase;
    }
  }
  
};
