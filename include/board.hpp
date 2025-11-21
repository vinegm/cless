#pragma once

#include "chess_types.hpp"
#include "popup.hpp"
#include "tui_state.hpp"
#include "win_handler.hpp"

#include <cstdint>
#include <ncurses.h>
#include <optional>

enum class BoardOrientation {
  WHITE,
  BLACK
};

enum class SquareColor {
  WHITE = 1,
  BLACK,
  HIGHLIGHTED,
  SELECTED,
  LEGAL_MOVE
};

class BoardWin : public BaseWindow<TuiState> {
public:
  BoardWin(TuiState &state) : BaseWindow<TuiState>(state) { draw_panel(); }

  void draw_panel() override;
  void update() override;
  void handle_input(int pressed_key) override;
  void recenter_popups() override { popup_handler.recenter_popups(); };

private:
  const std::string menu_win_name;
  uint8_t highlighted_square = E4;
  BoardOrientation board_orientation = BoardOrientation::WHITE;
  std::optional<int> selected_square = std::nullopt;
  std::optional<Move> promotion_move = std::nullopt;

  UniqueWindow board_win;

  PopupHandler popup_handler;
  Popup help_popup{
      {"Arrow keys / hjkl - Move cursor",
       "Space / Enter - Select piece / Move piece",
       "o - Invert board orientation",
       "? - Show help",
       "q - Quit to main menu"}
  };
  Popup promote_popup{{"Choose promotion piece:"}, {"Queen", "Rook", "Bishop", "Knight"}};

  void handle_piece_selection();
  void printw_board();
  void printw_rank_labels();
  void printw_file_labels();
  int get_square_from_orientation(int draw_rank, int draw_file);
  SquareColor get_square_color(int square);
  char get_piece_char(Piece piece);
};
