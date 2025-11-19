#pragma once

#include "win_handler.hpp"

class GameState;

struct TuiState : BaseState {
  std::string menu_win_name;
  std::string board_win_name;

  GameState &game;

  TuiState(int height, int width, GameState &game) : BaseState(height, width), game(game) {}
};
