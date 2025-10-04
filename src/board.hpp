#pragma once

#include "engine.hpp"
#include "win_handler.hpp"
#include <cstdint>
#include <ncurses.h>

enum BoardOrientation { BOARD_ORIENTATION_WHITE, BOARD_ORIENTATION_BLACK };

enum SquareColor {
  WHITE_SQUARE = 1,
  BLACK_SQUARE,
  HIGHLIGHTED_SQUARE,
  SELECTED_SQUARE
};

class BoardWin : public BaseWindow {
public:
  BoardWin(std::string menu_win_name) : menu_win_name(menu_win_name) {}
  void draw() override;

private:
  UniqueWindow board_win;

  const std::string menu_win_name;
  uint8_t highlighted_square = E4;
  int selected_square = -1;

  int board_orientation = BOARD_ORIENTATION_WHITE;
  BoardState game;

  void game_loop();
  int handle_input(int ch);
  void printw_board();
  void printw_rank_labels();
  void printw_file_labels();
  int get_square_from_orientation(int draw_rank, int draw_file);
  int get_square_color(int square);
  char get_piece_char_at(Square square);
};
