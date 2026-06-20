#include "include/input.h"
void InputHandler::registerInputs(SDL_Event* event)  {
  
    Event e; 
  switch(event->type) {
    case SDL_EVENT_KEY_DOWN: 
      e.type = KeyDown;
      events.push(e);
    
    break;
    case SDL_EVENT_MOUSE_MOTION: 
      SDL_GetMouseState(&e.mousePosition.x,&e.mousePosition.y);
      e.type = MouseMove;
      break;
    
    case SDL_EVENT_MOUSE_BUTTON_DOWN: 
      e.type = MouseDown;
      break;
    case SDL_EVENT_KEY_UP: 
      e.type = KeyUp;
      break;
    case SDL_EVENT_MOUSE_BUTTON_UP:
      e.type = MouseUp;
      break;
  }

}
