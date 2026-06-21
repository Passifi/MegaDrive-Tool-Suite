#pragma once
#include "ControlStructure.h"
#include "SDL3/SDL_rect.h"
#include "SDL3/SDL_events.h"
#include "SDL3/SDL_render.h"
#include <stack>
#include <map>
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
  CTRL,
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
    InputMode inputMode = InputMode::Draw;
    SDL_Renderer* renderer = nullptr;
    std::stack<Event> events;
    MouseState state;
    void registerInputs(SDL_Event* event); 

    
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


class EventHandler {
  public:
  //ControlState state; 
  MouseState mousestate;
  std::map<SDL_Keycode,bool> keyMap {
  {SDLK_ESCAPE,false},
  {SDLK_F1,false},
  {SDLK_F2,false},
  {SDLK_SPACE,false},
  {SDLK_RETURN,false},
  {SDLK_LALT,false},
  {SDLK_S,false},
  {SDLK_LCTRL,false},

};
  void drawCommand();
  void (* mouseDown)();
  void (* mouseDrag)();
  void handleEvents(std::stack<Event>* events) {
    while(!events->empty())   {
      auto current = events->top();
      events->pop();
      switch(current.type) {
        case MouseDown:
          mousestate.mouseDown = true;
          // call MousedownCommand based on button
          break; 
        case MouseUp: 
          mousestate.mouseDown = false; 
          break; 
        case MouseMove:
          mousestate.dragging = mousestate.mouseDown;
          mousestate.position = current.mousePosition; 
          break; 
        case KeyDown:
          keyMap[current.keystate.key] = true;
          // check whether combo is active
          if(keyMap[SDLK_LALT] || keyMap[SDLK_LCTRL]) {

          }else {
            // call eky command 
          }
        case KeyUp:
          keyMap[current.keystate.key] = false;
          // call keyStroke event
      }
    }
  }
  };




