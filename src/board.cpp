#include "board.hpp"

#include "chess_types.hpp"
#include "game_logic.hpp"
#include "utils.hpp"

#include <ncurses.h>
#include <string>

#define title_padding padding
#define board_padding (title_padding + padding * 2)
#define next_move_padding (board_padding + board_height + padding)
#define board_width 18  // 8 + 8 for 2 chars per square, + 2 for borders
#define board_height 10 // 8 for squares + 2 for borders

void BoardWin::draw_panel() {
  popup_handler.add_popup("help", help_popup);
  popup_handler.add_popup("promote", promote_popup, [this](int pressed_key, int popup_result) {
    static const PieceType promotion_pieces[] =
        {PIECE_QUEEN, PIECE_ROOK, PIECE_BISHOP, PIECE_KNIGHT};
    PieceType chosen_piece = promotion_pieces[popup_result];

    if (promotion_move.has_value()) {
      Move move = promotion_move.value();
      move.promotion_piece = chosen_piece;
      state.game.make_move(move);
    }

    promotion_move = std::nullopt;
    selected_square = std::nullopt;

    return true;
  });

  WINDOW *main_win_ptr = main_win.get();
  int main_win_height, main_win_width;
  getmaxyx(main_win_ptr, main_win_height, main_win_width);

  box(main_win_ptr, 0, 0);

  std::string title;
  if (state.game.current_mode == PLAYER_VS_ENGINE) {
    title = "CLESS - Player vs Engine";
  } else {
    title = "CLESS - Player vs Player";
  }

  modifier_wrapper(main_win_ptr, A_BOLD, [&]() {
    mvwprintw_centered(main_win_ptr, main_win_width, title_padding, title);
  });

  std::string help_hint = "?: Help";
  modifier_wrapper(main_win_ptr, A_DIM, [&]() {
    mvwprintw_centered(main_win_ptr, main_win_width, main_win_height - 3, help_hint);
  });

  std::string quit_hint = "q: Quit to main menu";
  modifier_wrapper(main_win_ptr, A_DIM, [&]() {
    mvwprintw_centered(main_win_ptr, main_win_width, main_win_height - 2, quit_hint);
  });

  int board_start_x = (main_win_width - board_width) / 2;
  WINDOW *board_win_ptr =
      derwin(main_win_ptr, board_height, board_width, board_padding, board_start_x);
  board_win = UniqueWindow(board_win_ptr);

  update(); // Initial draw of interractive elements
}

void BoardWin::update() {
  if (popup_handler.any_visible()) {
    popup_handler.update();
    return;
  }

  WINDOW *board_win_ptr = board_win.get();
  WINDOW *main_win_ptr = main_win.get();
  int main_win_height, main_win_width;
  getmaxyx(main_win_ptr, main_win_height, main_win_width);

  std::string to_move_text =
      (state.game.to_move() == PieceColor::WHITE) ? "White to move" : "Black to move";

  modifier_wrapper(main_win_ptr, A_BOLD, [&]() {
    mvwprintw_centered(main_win_ptr, main_win_width, next_move_padding, to_move_text);
  });

  modifier_wrapper(board_win_ptr, A_DIM, [&]() {
    printw_rank_labels();
    printw_file_labels();
  });

  printw_board();

  touchwin(main_win_ptr);
  wrefresh(main_win_ptr);
}

void BoardWin::handle_input(int pressed_key) {
  if (popup_handler.any_visible()) {
    popup_handler.handle_input(pressed_key);
    return;
  }

  bool is_white_orientation = (board_orientation == BoardOrientation::WHITE);
  int orientation_mod = is_white_orientation ? 1 : -1;

  switch (pressed_key) {
    case ' ':
    case 10: handle_piece_selection(); break;

    case 'k':
    case KEY_UP:
      highlighted_square += NORTH * orientation_mod;
      highlighted_square %= 64;
      break;

    case 'j':
    case KEY_DOWN:
      highlighted_square -= NORTH * orientation_mod;
      highlighted_square %= 64;
      break;

    case 'h':
    case KEY_LEFT: {
      const int col = highlighted_square % 8;
      const int left_edge = (orientation_mod == 1) ? 0 : 7;

      if (col == left_edge) {
        highlighted_square += 7 * orientation_mod;
        break;
      }

      highlighted_square -= EAST * orientation_mod;
      break;
    }

    case 'l':
    case KEY_RIGHT: {
      const int col = highlighted_square % 8;
      const int right_edge = (orientation_mod == 1) ? 7 : 0;

      if (col == right_edge) {
        highlighted_square -= 7 * orientation_mod;
        break;
      }

      highlighted_square += EAST * orientation_mod;
      break;
    }

    case 'o':
      board_orientation = is_white_orientation ? BoardOrientation::BLACK : BoardOrientation::WHITE;
      highlighted_square = 63 - highlighted_square;
      break;

    case '?': popup_handler.show_popup("help"); break;
    case 'q': state.next_window = state.menu_win_name; return;
    default: break;
  }
}

/**
 * @brief Handle piece selection and movement logic
 */
void BoardWin::handle_piece_selection() {
  // Select
  if (!selected_square.has_value()) {
    Piece piece = state.game.get_piece_at(highlighted_square);
    if (piece.type != PIECE_NONE && piece.color == state.game.to_move())
      selected_square = highlighted_square;
    return;
  }

  // Deselect
  if (highlighted_square == selected_square) {
    selected_square = std::nullopt;
    return;
  }

  // Make move
  MoveList legal_moves =
      state.game.get_legal_moves_from(static_cast<Square>(selected_square.value()));

  Move *found_move = nullptr;
  bool has_promotion_moves = false;

  for (int i = 0; i < legal_moves.count; i++) {
    if (legal_moves[i].to == highlighted_square) {
      found_move = &legal_moves[i];
      if (legal_moves[i].is_promotion()) { has_promotion_moves = true; }
      break;
    }
  }

  bool move_successful = false;
  if (found_move) {
    Move *selected_move = found_move;

    if (has_promotion_moves) {
      promotion_move = *selected_move;
      popup_handler.show_popup("promote");
      return;

      // if (choice == 0) {
      //   selected_square = std::nullopt;
      //   return;
      // }

      // PieceType promotion_pieces[] = {PIECE_QUEEN, PIECE_ROOK, PIECE_BISHOP, PIECE_KNIGHT};
      // PieceType chosen_piece = promotion_pieces[choice];

      // for (int i = 0; i < legal_moves.count; i++) {
      //   if (legal_moves[i].to == highlighted_square && legal_moves[i].is_promotion()
      //       && legal_moves[i].promotion_piece == chosen_piece) {
      //     selected_move = &legal_moves[i];
      //     break;
      //   }
      // }
    }

    move_successful = state.game.make_move(*selected_move);
  }

  // Deselect after move
  if (move_successful) {
    selected_square = std::nullopt;

    if (state.game.current_mode == PLAYER_VS_ENGINE) { state.game.make_engine_move(); }
    return;
  }

  // Failed move, select new highlighted square
  selected_square = std::nullopt;
  handle_piece_selection();
}

/**
 * @brief Render the board
 */
void BoardWin::printw_board() {
  WINDOW *board_win_ptr = board_win.get();
  for (int draw_rank = 0; draw_rank < 8; draw_rank++) {
    for (int draw_file = 0; draw_file < 8; draw_file++) {
      const int square = get_square_from_orientation(draw_rank, draw_file);
      const SquareColor sq_color = get_square_color(square);

      const char piece_char = get_piece_char(state.game.get_piece_at(square));
      modifier_wrapper(board_win_ptr, COLOR_PAIR(sq_color), [&]() {
        mvwprintw(board_win_ptr, draw_rank + padding, draw_file * 2 + 1, "%c ", piece_char);
      });
    }
  }
}

/**
 * @brief Render the rank labels on the board
 */
void BoardWin::printw_rank_labels() {
  WINDOW *board_win_ptr = board_win.get();

  switch (board_orientation) {
    case BoardOrientation::WHITE:
      for (int i = 0; i < 8; i++) {
        mvwprintw(board_win_ptr, i + padding, 0, "%d", 8 - i);
        mvwprintw(board_win_ptr, i + padding, board_width - 1, "%d", 8 - i);
      }
      break;

    case BoardOrientation::BLACK:
      for (int i = 0; i < 8; i++) {
        mvwprintw(board_win_ptr, i + padding, 0, "%d", i + 1);
        mvwprintw(board_win_ptr, i + padding, board_width - 1, "%d", i + 1);
      }
      break;
  }
}

/**
 * @brief Render the file labels on the board
 */
void BoardWin::printw_file_labels() {
  WINDOW *board_win_ptr = board_win.get();

  if (board_orientation == BoardOrientation::WHITE) {
    mvwprintw(board_win_ptr, 0, 1, "a b c d e f g h");
    mvwprintw(board_win_ptr, board_height - 1, 1, "a b c d e f g h");
    return;
  }

  mvwprintw(board_win_ptr, 0, 1, "h g f e d c b a");
  mvwprintw(board_win_ptr, board_height - 1, 1, "h g f e d c b a");
}

/**
 * @brief Get the square based on the board orientation
 *
 * @param draw_rank
 * @param draw_file
 * @return int
 */
int BoardWin::get_square_from_orientation(int draw_rank, int draw_file) {
  int rank, file;
  switch (board_orientation) {
    case BoardOrientation::WHITE:
      rank = 7 - draw_rank;
      file = draw_file;
      return rank * 8 + file;

    case BoardOrientation::BLACK:
      rank = draw_rank;
      file = 7 - draw_file;
      return rank * 8 + file;

    default: return -1; // Error case, should not happen
  }
}

/**
 * @brief Get the color of the square
 *
 * @param square
 * @return int
 */
SquareColor BoardWin::get_square_color(int square) {
  if (square == highlighted_square) return SquareColor::HIGHLIGHTED;

  if (square == selected_square) return SquareColor::SELECTED;

  if (selected_square.has_value()) {
    MoveList legal_moves =
        state.game.get_legal_moves_from(static_cast<Square>(selected_square.value()));
    for (int i = 0; i < legal_moves.count; i++) {
      if (legal_moves[i].to == square) return SquareColor::LEGAL_MOVE;
    }
  }

  int is_white_square = ((square / 8 + square % 8) % 2 == 0) ? 1 : 0;
  return is_white_square ? SquareColor::WHITE : SquareColor::BLACK;
}

/**
 * @brief Get the character representation of a piece at a square
 */
char BoardWin::get_piece_char(Piece piece) {
  char piece_char;
  switch (piece.type) {
    case PIECE_PAWN: piece_char = 'P'; break;
    case PIECE_ROOK: piece_char = 'R'; break;
    case PIECE_KNIGHT: piece_char = 'N'; break;
    case PIECE_BISHOP: piece_char = 'B'; break;
    case PIECE_QUEEN: piece_char = 'Q'; break;
    case PIECE_KING: piece_char = 'K'; break;
    default: piece_char = ' '; break;
  }

  // Convert to lowercase for black pieces
  if (piece.color == BLACK) { piece_char = tolower(piece_char); }

  return piece_char;
}
