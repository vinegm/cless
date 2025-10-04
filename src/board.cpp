#include "board.hpp"
#include "engine.hpp"
#include <vector>

#define title_padding line_padding
#define board_padding (title_padding + line_padding * 2)
#define next_move_padding (board_padding + board_height + line_padding)
#define board_width 18  // 8 + 8 for 2 chars per square, + 2 for borders
#define board_height 10 // 8 for squares + 2 for borders

void BoardWin::draw() {
  WINDOW *parent_win = this->handler->get_main_win();
  int parent_height, parent_width;
  getmaxyx(parent_win, parent_height, parent_width);

  box(parent_win, 0, 0);

  std::string title = "CLESS - Chess Game";
  std::vector<std::string> instructions = {
      "Arrow keys/hjkl: move cursor", "Space/Enter: select", "o: flip board",
      "q: return to menu"};

  int instructions_count = instructions.size();
  int instructions_line = parent_height - line_padding - instructions_count;

  modifier_wrapper(parent_win, A_BOLD, [&]() {
    mvwprintw_centered(parent_win, parent_width, title_padding, title);
  });

  modifier_wrapper(parent_win, A_DIM, [&]() {
    for (size_t i = 0; i < instructions_count; i++) {
      mvwprintw_centered(parent_win, parent_width, instructions_line + i,
                         instructions[i]);
    }
  });

  int board_start_x = (parent_width - board_width) / 2;
  WINDOW *board_win = derwin(parent_win, board_height, board_width,
                             board_padding, board_start_x);
  this->board_win = UniqueWindow(board_win);

  modifier_wrapper(board_win, A_DIM, [&]() {
    this->printw_rank_labels();
    this->printw_file_labels();
  });

  this->game_loop();
}

void BoardWin::game_loop() {
  WINDOW *board_win = this->board_win.get();
  WINDOW *parent_win = this->handler->get_main_win();

  int _, parent_width;
  getmaxyx(parent_win, _, parent_width);

  int pressed_key;
  bool is_white_orientation =
      (this->board_orientation == BOARD_ORIENTATION_WHITE);
  int orientation_mod = is_white_orientation ? 1 : -1;
  std::string to_move_text;

  keypad(board_win, true);
  while (true) {
    to_move_text = (this->game.to_move == PieceColor::WHITE) ? "White to move"
                                                             : "Black to move";

    modifier_wrapper(parent_win, A_BOLD, [&]() {
      mvwprintw_centered(parent_win, parent_width, next_move_padding,
                         to_move_text);
    });

    wrefresh(parent_win);

    this->printw_board();

    pressed_key = wgetch(board_win);
    switch (pressed_key) {
      case ' ':
      case 10:
        // TODO: handle piece selection and movement
        break;

      case 'k':
      case KEY_UP:
        this->highlighted_square += 8 * orientation_mod;
        this->highlighted_square %= 64;
        break;

      case 'j':
      case KEY_DOWN:
        this->highlighted_square -= 8 * orientation_mod;
        this->highlighted_square %= 64;
        break;

      case 'h':
      case KEY_LEFT: {
        const int col = this->highlighted_square % 8;
        const int left_edge = (orientation_mod == 1) ? 0 : 7;

        if (col == left_edge) {
          this->highlighted_square += 7 * orientation_mod;
          break;
        }

        this->highlighted_square -= orientation_mod;
        break;
      }

      case 'l':
      case KEY_RIGHT: {
        const int col = this->highlighted_square % 8;
        const int right_edge = (orientation_mod == 1) ? 7 : 0;

        if (col == right_edge) {
          this->highlighted_square -= 7 * orientation_mod;
          break;
        }

        this->highlighted_square += orientation_mod;
        break;
      }

      case 'o':
        orientation_mod *= -1;
        this->board_orientation = !this->board_orientation;
        this->highlighted_square = 63 - this->highlighted_square;
        modifier_wrapper(board_win, A_DIM,
                         [&]() { this->printw_rank_labels(); });
        break;

      case 'q': this->handler->next_window = this->menu_win_name; return;

      case KEY_RESIZE:
        this->handler->event = this->handler->resize_event;
        return;

      case ERR: break;

      default: break;
    }
  }
}

/**
 * @brief Render the board
 */
void BoardWin::printw_board() {
  WINDOW *board_win = this->board_win.get();
  for (int draw_rank = 0; draw_rank < 8; draw_rank++) {
    for (int draw_file = 0; draw_file < 8; draw_file++) {
      const int square =
          this->get_square_from_orientation(draw_rank, draw_file);
      const int color_pair = this->get_square_color(square);

      char piece_char = this->get_piece_char_at(static_cast<Square>(square));
      modifier_wrapper(board_win, COLOR_PAIR(color_pair), [&]() {
        mvwprintw(board_win, draw_rank + line_padding, draw_file * 2 + 1, "%c ",
                  piece_char);
      });
    }
  }
  wrefresh(board_win);
}

/**
 * @brief Render the rank labels on the board
 */
void BoardWin::printw_rank_labels() {
  WINDOW *board_win = this->board_win.get();

  switch (this->board_orientation) {
    case BOARD_ORIENTATION_WHITE:
      for (int i = 0; i < 8; i++) {
        mvwprintw(board_win, i + line_padding, 0, "%d", 8 - i);
        mvwprintw(board_win, i + line_padding, board_width - 1, "%d", 8 - i);
      }
      break;

    case BOARD_ORIENTATION_BLACK:
      for (int i = 0; i < 8; i++) {
        mvwprintw(board_win, i + line_padding, 0, "%d", i + 1);
        mvwprintw(board_win, i + line_padding, board_width - 1, "%d", i + 1);
      }
      break;
  }
}

/**
 * @brief Render the file labels on the board
 */
void BoardWin::printw_file_labels() {
  WINDOW *board_win = this->board_win.get();

  mvwprintw(board_win, 0, 1, "a b c d e f g h");
  mvwprintw(board_win, board_height - 1, 1, "a b c d e f g h");
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
  switch (this->board_orientation) {
    case BOARD_ORIENTATION_WHITE:
      rank = 7 - draw_rank;
      file = draw_file;
      return rank * 8 + file;

    case BOARD_ORIENTATION_BLACK:
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
int BoardWin::get_square_color(int square) {
  if (square == this->highlighted_square) return HIGHLIGHTED_SQUARE;

  if (square == this->selected_square) return SELECTED_SQUARE;

  int is_white_square = ((square / 8 + square % 8) % 2 == 0) ? 1 : 0;
  return is_white_square ? WHITE_SQUARE : BLACK_SQUARE;
}

/**
 * @brief Get the character representation of the piece at the given square
 *
 * @param square
 * @return char
 */
char BoardWin::get_piece_char_at(Square square) {
  Piece piece = this->game.get_piece_at(square);
  char piece_str = '?';

  switch (piece.type) {
    case PIECE_PAWN: piece_str = 'P'; break;
    case PIECE_ROOK: piece_str = 'R'; break;
    case PIECE_KNIGHT: piece_str = 'K'; break;
    case PIECE_BISHOP: piece_str = 'B'; break;
    case PIECE_QUEEN: piece_str = 'Q'; break;
    case PIECE_KING: piece_str = 'K'; break;
    case PIECE_NONE: piece_str = ' '; break;
  }

  if (piece.color == PieceColor::BLACK) piece_str = std::tolower(piece_str);

  return piece_str;
}
