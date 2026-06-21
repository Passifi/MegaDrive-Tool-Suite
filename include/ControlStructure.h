#pragma once
enum InputMode {
  Draw,
  DrawMeta,
  Fill,
  Select, 
  SelectMeta,
  Erase
};
struct CursorSettings {
  InputMode mode = InputMode::Draw;  
};





