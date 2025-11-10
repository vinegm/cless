#pragma once

#include "game_logic.hpp"
#include "win_handler.hpp"

#include <cstdint>
#include <ncurses.h>
#include <optional>

enum BoardOrientation {
  BOARD_ORIENTATION_WHITE,
  BOARD_ORIENTATION_BLACK
};

enum SquareColor {
  WHITE_SQUARE = 1,
  BLACK_SQUARE,
  HIGHLIGHTED_SQUARE,
  SELECTED_SQUARE,
  LEGAL_MOVE_SQUARE
};

struct BoardWinArgs {
  std::string menu_win_name = "Undefined";
  GameState &game_state;
};

class BoardWin : public BaseWindow {
public:
  BoardWin(const BoardWinArgs &args) :
      menu_win_name(args.menu_win_name), game_state(args.game_state) {}
  void draw() override;

private:
  const std::string menu_win_name;
  GameState &game_state;

  UniqueWindow board_win;
  uint8_t highlighted_square = E4;

  std::optional<int> selected_square = std::nullopt;

  int board_orientation = BOARD_ORIENTATION_WHITE;

  void game_loop();
  void handle_piece_selection();
  void printw_board();
  void printw_rank_labels();
  void printw_file_labels();
  void show_help_popup();
  int get_square_from_orientation(int draw_rank, int draw_file);
  int get_square_color(int square);
  char get_piece_char(Piece piece);
};
