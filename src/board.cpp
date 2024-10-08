#include "board.hpp"

#define title_padding line_padding
#define board_padding (title_padding + line_padding * 2)
#define next_move_padding (board_padding + board_height + line_padding)
#define board_width 18  // 8 + 8 for 2 chars per square, + 2 for borders
#define board_height 10 // 8 for squares + 2 for borders

void BoardWin::draw() {
  WINDOW *board_win;

  WINDOW *raw_parent_win = this->parent_win.get()->get();
  int parent_height, parent_width;
  getmaxyx(raw_parent_win, parent_height, parent_width);

  werase(raw_parent_win);
  box(raw_parent_win, 0, 0);

  wattron(raw_parent_win, A_BOLD);
  mvwprintw_centered(raw_parent_win, parent_width, title_padding,
                     "♛ CLESS - Chess Game ♛");
  wattroff(raw_parent_win, A_BOLD);

  std::vector<std::string> instructions = {
      "Arrow keys/hjkl: move cursor", "Space/Enter: select", "o: flip board",
      "q: return to menu"};
  int instructions_count = instructions.size();
  int instructions_line = parent_height - line_padding - instructions_count;

  wattron(raw_parent_win, A_DIM);
  for (int i = 0; i < instructions_count; i++) {
    mvwprintw_centered(raw_parent_win, parent_width, instructions_line + i,
                       instructions[i]);
  }
  wattroff(raw_parent_win, A_DIM);

  int board_start_x = (parent_width - board_width) / 2;
  board_win = derwin(raw_parent_win, board_height, board_width, board_padding,
                     board_start_x);

  this->printw_rank_labels();
  this->printw_file_labels();

  game_loop();

  werase(board_win);
  delwin(board_win);
}

void BoardWin::game_loop() {
  WINDOW *raw_parent_win = this->parent_win.get()->get();
  WINDOW *raw_board_win = this->board_win.get();

  int pressed_key;
  std::string to_move_text;
  keypad(raw_board_win, true);

  int parent_height, parent_width;
  getmaxyx(raw_parent_win, parent_height, parent_width);

  int orientation_dir =
      (this->board_orientation == BOARD_ORIENTATION_WHITE) ? 1 : -1;
  while (true) {
    // to_move_text = (this->game->to_move == PieceColor::WHITE)
    //                    ? "♔ White to move"
    //                    : "♚ Black to move";
    to_move_text = "♔ White to move";

    wattron(raw_parent_win, A_BOLD);
    mvwprintw_centered(raw_parent_win, parent_width, next_move_padding,
                       to_move_text);
    wattroff(raw_parent_win, A_BOLD);

    wrefresh(raw_parent_win);

    printw_board();

    pressed_key = wgetch(raw_board_win);
    switch (pressed_key) {
      case ' ':
      case 10:
        // TODO: handle piece selection and movement
        break;

      case 'k':
      case KEY_UP:
        this->highlighted_square += 8 * orientation_dir;
        if (this->highlighted_square < 0) this->highlighted_square += 64;
        if (this->highlighted_square > 63) this->highlighted_square -= 64;
        break;

      case 'j':
      case KEY_DOWN:
        this->highlighted_square -= 8 * orientation_dir;
        if (this->highlighted_square < 0) this->highlighted_square += 64;
        if (this->highlighted_square > 63) this->highlighted_square -= 64;
        break;

      case 'h':
      case KEY_LEFT: {
        const int col = this->highlighted_square % 8;
        const int left_edge = (orientation_dir == 1) ? 0 : 7;

        if (col == left_edge) {
          this->highlighted_square += 7 * orientation_dir;
          break;
        }

        this->highlighted_square -= orientation_dir;
        break;
      }

      case 'l':
      case KEY_RIGHT: {
        const int col = this->highlighted_square % 8;
        const int right_edge = (orientation_dir == 1) ? 7 : 0;

        if (col == right_edge) {
          this->highlighted_square -= 7 * orientation_dir;
          break;
        }

        this->highlighted_square += orientation_dir;
        break;
      }

      case 'o':
        orientation_dir *= -1;
        this->board_orientation = !this->board_orientation;
        this->highlighted_square = 63 - this->highlighted_square;
        this->printw_rank_labels();
        break;

      case 'q': this->handler->current_window = this->menu_win_id; return;

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
 *
 * @param board_win
 * @param board
 * @param game
 */
void BoardWin::printw_board() {
  WINDOW *raw_board_win = this->board_win.get();
  for (int draw_rank = 0; draw_rank < 8; draw_rank++) {
    for (int draw_file = 0; draw_file < 8; draw_file++) {
      const int square = get_square_from_orientation(draw_rank, draw_file);
      const int color_pair = get_square_color(square);

      if (has_colors()) wattron(raw_board_win, COLOR_PAIR(color_pair));

      // char piece_char =
      //     this->game.get_piece_char_at(static_cast<Square>(square));
      mvwprintw(raw_board_win, draw_rank + line_padding, draw_file * 2 + 1,
                "%c ", 'T');

      if (has_colors()) wattroff(raw_board_win, COLOR_PAIR(color_pair));
    }
  }
  wrefresh(raw_board_win);
}

/**
 * @brief Render the rank labels on the board
 *
 * @param board_win
 * @param game - Game state to determine orientation
 */
void BoardWin::printw_rank_labels() {
  WINDOW *raw_board_win = this->board_win.get();

  switch (this->board_orientation) {
    case BOARD_ORIENTATION_WHITE:
      for (int i = 0; i < 8; i++) {
        mvwprintw(raw_board_win, i + line_padding, 0, "%d", 8 - i);
        mvwprintw(raw_board_win, i + line_padding, board_width - 1, "%d",
                  8 - i);
      }
      break;

    case BOARD_ORIENTATION_BLACK:
      for (int i = 0; i < 8; i++) {
        mvwprintw(raw_board_win, i + line_padding, 0, "%d", i + 1);
        mvwprintw(raw_board_win, i + line_padding, board_width - 1, "%d",
                  i + 1);
      }
      break;
  }
}

/**
 * @brief Render the file labels on the board
 *
 * @param board_win
 */
void BoardWin::printw_file_labels() {
  WINDOW *raw_board_win = this->board_win.get();

  mvwprintw(raw_board_win, 0, 1, "a b c d e f g h");
  mvwprintw(raw_board_win, board_height - 1, 1, "a b c d e f g h");
}

/**
 * @brief Get the square based on the board orientation
 *
 * @param game - Game state to determine orientation
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
 * @param square - Square needed to color
 * @return int
 */
int BoardWin::get_square_color(int square) {
  if (square == this->highlighted_square) return HIGHLIGHTED_SQUARE;

  if (square == this->selected_square) return SELECTED_SQUARE;

  int is_white_square = ((square / 8 + square % 8) % 2 == 0) ? 1 : 0;
  return is_white_square ? WHITE_SQUARE : BLACK_SQUARE;
}
