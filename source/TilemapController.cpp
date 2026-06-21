#include "../include/TilemapController.h"

  
void TilemapController::initMap(int screenWidth, int screenHeight) {
    map = std::make_unique<Tilemap>(screenWidth,screenHeight);
  }
  
  int TilemapController::getTileValueAt(int x, int y) {
    if(x + y*map->width < map->data.size()) {
      return map->data[x+y*map->width];
    }
    else {
      return NO_TILE;
    }
  }
  SDL_Texture* TilemapController::getSelectedTileTexture() {
    return map->tiles[this->currentTile];
  }

  void TilemapController::setTileAt(int x, int y) {
      if(x + y*map->width < map->data.size()) {
        return;
      } 
      auto val = currentTile|controlState;
      map->data[x+y*map->width] = val;
  }
  void TilemapController::setIndex(int x,int y ) {
    auto nextIndex = x + y* this->horizontalTiles;
    if(nextIndex < map->size) {
      currentTile = nextIndex;
    }
  }
  void TilemapController::setIndex(int index) {
    if(index < map->size) {
      currentTile = index; 
    }
  }
  void TilemapController::nextTile() {
    currentTile++;
    if(map->tiles.size() <= currentTile ) {
      currentTile = 0;
    }
  }
  void TilemapController::previousTile() {
    if(currentTile == 0) {
      currentTile = map->tiles.size() -1;
    }
    else {
      currentTile--;
    }
  }


  void TilemapController::fill(int startPosition) {
  assert(startPosition >= 0 && startPosition < map->data.size());
  std::queue<int> to_visit;
  std::set<int> visited;
  to_visit.push(startPosition);
  visited.insert(startPosition);
  int originalTile = map->data[startPosition];
  auto add = [&](int element) {
    if ((element >= 0) && (element < map->data.size()) &&
        (map->data[element] == originalTile)
        && (visited.count(element) == 0)) {
      to_visit.push(element);
      visited.insert(element);
    }
  };
  while (!to_visit.empty()) {
    int currentPosition = to_visit.front();
    to_visit.pop();
    map->data[currentPosition] = currentTile|controlState;
    add(currentPosition + 1);
    add(currentPosition - 1);
    add(currentPosition + horizontalTiles);
    add(currentPosition - horizontalTiles);
  }
}
  void TilemapController::setFlipHorizontal() {
    controlState ^= Flip_Horizontally;
  }
  
  void TilemapController::setFlipVertical() {

    controlState ^= Flip_Vertically;
  }

  
