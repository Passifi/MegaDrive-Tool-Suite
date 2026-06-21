#include "../include/AppState.h"

void App::initalize() {
  renderer.intialize(screenWidth,screenHeight);
  tilemapController.initMap(screenWidth,screenHeight);
}
void App::iterate() {
    eventHandler.handleEvents(&inputHandler.events);
    renderer.render(); 
  }
  

