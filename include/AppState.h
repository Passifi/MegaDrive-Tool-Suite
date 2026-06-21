#pragma once 
#include "ControlStructure.h"
#include "fileIO.h"
#include "input.h"
#include "render.h"
#include "Tiledata.h" 
#include "TilemapController.h"
int screenWidth = 800; 
int screenHeight = 600;
class App {
  public: 
  Renderer renderer;
  TilemapController tilemapController;
  InputHandler inputHandler;
  EventHandler eventHandler; 
  InputMode mode; 
  void initalize(); 
  void iterate();
  };
