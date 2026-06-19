#pragma once
#include "ControlStructure.h"
#include <SDL_rect.h>
#include <stack>
enum EventType {
  MouseDown, 
  MouseUp,
  MouseMove, 
  KeyDown,
  KeyUp
};

enum Keys {
  F1,
  F2,
  F3,
  F4, 
  Space, 
  Enter,
  S,
  ALT,
  MaxKeys
};
struct MouseState {
  bool mouseDown = false;
  bool dragging = false;
  SDL_FPoint startPosition;
  SDL_FPoint position;
};

struct KeyState {
  bool keyDown = false;
  Keys key; 
};

struct Event {
  SDL_FPoint mousePosition; 
  KeyState keystate; 
  EventType type;

};

class InputHandler {
    public:
    InputMode inputMode = Draw;
    SDL_Renderer* renderer = nullptr;
    std::stack<Event> events;
    MouseState state;
    void registerInputs(SDL_Event event); 

    
};  


void drawMouseDown() {
  InputMode state; 
  switch(state) {
    case Draw: 
      //calcualte logical position
      // issue drawing Command
    case Select: 
      // claculate logical position
      // if on selectionArea -> select corrosponding tile 
      // else set selectionRect start and logical selection start 
    default: 
      break;
  }
}

// LogicalSelectionRect
// OpticalSelectionRect

void draggingMouse(MouseState mouseState) {
  
  InputMode state; 
  switch(state) {
    case Draw: 
      // calcualte lgocail postion
      // issue drawing Command 
    case Select: 
      // calculate effective position 
      // reisze selectoinRect 
      break; 
    default: 
      break;
  }
}

void mouseUpCommand(MouseState mouseState) {
  
  InputMode state; 
  switch(state) {
    case Draw:
      break; 
    case Select: 
        // finalize logic selection;
      break; 
    default: 
      break;
  }
}


class MainController {
  //ControlState state; 
  MouseState mousestate;
  std::array<bool,MaxKeys> keys;
  void drawCommand();
  void (* mouseDown)();
  void (* mouseDrag)();
  void handleEvents(std::stack<Event>* events) {
    while(!events->empty())   {
      auto current = events->top();
      switch(current.type) {
        case MouseDown:
          mousestate.mouseDown = true;
          mouseDown();
          // call Mousedown event
          break; 
        case MouseUp: 
          mousestate.mouseDown = false; 
          break; 
        case MouseMove:
          mousestate.dragging = mousestate.mouseDown;
          mousestate.position = current.mousePosition; 
          break; 
        case KeyDown:
          keys[current.keystate.key] = true;
          // call key event
        case KeyUp:
          keys[current.keystate.key] = false;
          // call keyStroke event
      }
    }
  }
  }
};



