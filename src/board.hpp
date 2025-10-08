#pragma once

#include "engine.hpp"
#include "utils.hpp"
#include "win_handler.hpp"
#include <ncurses.h>

enum BoardOrientation { BOARD_ORIENTATION_WHITE, BOARD_ORIENTATION_BLACK };

enum SquareColor {
  WHITE_SQUARE,
  BLACK_SQUARE,
  HIGHLIGHTED_SQUARE,
  SELECTED_SQUARE
};

class BoardWin : public BaseWindow {
public:
  void draw() override;

private:
  UniqueWindow board_win;

  int menu_win_id;
  int highlighted_square;
  int selected_square;

  int board_orientation;
  BoardState game;

  void game_loop();
  int handle_input(int ch);
  void printw_board();
  void printw_rank_labels();
  void printw_file_labels();
  int get_square_from_orientation(int draw_rank, int draw_file);
  int get_square_color(int square);
};
