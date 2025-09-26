
#include "board.h"
#include "engine.h"
#include "utils.h"
#include <ncurses.h>

static void game_loop(WINDOW *parent_win, WINDOW *board_win,
                      ChessBoardState *board, BoardData *game);
static void printw_board(WINDOW *board_win, ChessBoardState *board,
                         BoardData *game);
static void printw_rank_labels(WINDOW *board_win, BoardData *game);
static void printw_file_labels(WINDOW *board_win);
static int get_square_from_orientation(BoardData *game, int draw_rank,
                                       int draw_file);
static int get_square_color(int square, BoardData *game);

#define title_padding line_padding
#define board_padding (title_padding + line_padding * 2)
#define next_move_padding (board_padding + board_height + line_padding)
#define board_width 18  // 8 + 8 for 2 chars per square, + 2 for borders
#define board_height 10 // 8 for squares + 2 for borders

void init_board_data(BoardData *board_data, TuiHandler *tui, int menu_win_id, ChessBoardState *gameState) {
  init_chess_board(gameState);

  board_data->tui = tui;
  board_data->menu_win_id = menu_win_id;
  board_data->board = gameState;

  board_data->selected_square = -1;
  board_data->highlighted_square = E4;
  board_data->board_orientation = white_orientation;
  board_data->status = 0;

}

void render_board(WINDOW *parent_win, void *board_data) {
  BoardData *game = (BoardData *)board_data;
  ChessBoardState *board = game->board;
  WINDOW *board_win;
  int parent_height, parent_width;
  getmaxyx(parent_win, parent_height, parent_width);

  werase(parent_win);
  box(parent_win, 0, 0);

  wattron(parent_win, A_BOLD);
  mvwprintw_centered(parent_win, parent_width, title_padding,
                     "♛ CLESS - Chess Game ♛");
  wattroff(parent_win, A_BOLD);

  char *instructions[] = {"Arrow keys/hjkl: move cursor", "Space/Enter: select",
                          "o: flip board", "q: return to menu"};
  int instructions_count = len(instructions);
  int instructions_line = parent_height - line_padding - instructions_count;

  wattron(parent_win, A_DIM);
  for (int i = 0; i < instructions_count; i++) {
    mvwprintw_centered(parent_win, parent_width, instructions_line + i,
                       instructions[i]);
  }
  wattroff(parent_win, A_DIM);

  int board_start_x = (parent_width - board_width) / 2;
  board_win = derwin(parent_win, board_height, board_width, board_padding,
                     board_start_x);

  printw_rank_labels(board_win, game);
  printw_file_labels(board_win);

  game_loop(parent_win, board_win, board, game);

  werase(board_win);
  delwin(board_win);
}

static void game_loop(WINDOW *parent_win, WINDOW *board_win,
                      ChessBoardState *board, BoardData *game) {
  int pressed_key;
  char *to_move_text;
  keypad(board_win, TRUE);

  int parent_height, parent_width;
  getmaxyx(parent_win, parent_height, parent_width);

  int orientation_dir = (game->board_orientation == white_orientation) ? 1 : -1;
  while (TRUE) {
    to_move_text =
        (board->to_move == WHITE) ? "♔ White to move" : "♚ Black to move";

    wattron(parent_win, A_BOLD);
    mvwprintw_centered(parent_win, parent_width, next_move_padding,
                       to_move_text);
    wattroff(parent_win, A_BOLD);

    wrefresh(parent_win);

    printw_board(board_win, board, game);

    pressed_key = wgetch(board_win);
    switch (pressed_key) {
      case ' ':
      case 10:
        // TODO: handle piece selection and movement
        break;

      case 'k':
      case KEY_UP:
        game->highlighted_square += 8 * orientation_dir;
        if (game->highlighted_square < 0) game->highlighted_square += 64;
        if (game->highlighted_square > 63) game->highlighted_square -= 64;
        break;

      case 'j':
      case KEY_DOWN:
        game->highlighted_square -= 8 * orientation_dir;
        if (game->highlighted_square < 0) game->highlighted_square += 64;
        if (game->highlighted_square > 63) game->highlighted_square -= 64;
        break;

      case 'h':
      case KEY_LEFT: {
        const int col = game->highlighted_square % 8;
        const int left_edge = (orientation_dir == 1) ? 0 : 7;

        if (col == left_edge) {
          game->highlighted_square += 7 * orientation_dir;
          break;
        }

        game->highlighted_square -= orientation_dir;
        break;
      }

      case 'l':
      case KEY_RIGHT: {
        const int col = game->highlighted_square % 8;
        const int right_edge = (orientation_dir == 1) ? 7 : 0;

        if (col == right_edge) {
          game->highlighted_square -= 7 * orientation_dir;
          break;
        }

        game->highlighted_square += orientation_dir;
        break;
      }

      case 'o':
        orientation_dir *= -1;
        game->board_orientation = !game->board_orientation;
        game->highlighted_square = 63 - game->highlighted_square;
        printw_rank_labels(board_win, game);
        break;

      case 'q': game->tui->current_window = game->menu_win_id; return;

      case KEY_RESIZE: game->tui->event = game->tui->resize_event; return;

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
static void printw_board(WINDOW *board_win, ChessBoardState *board,
                         BoardData *game) {
  for (int draw_rank = 0; draw_rank < 8; draw_rank++) {
    for (int draw_file = 0; draw_file < 8; draw_file++) {
      const int square =
          get_square_from_orientation(game, draw_rank, draw_file);
      const int color_pair = get_square_color(square, game);

      if (has_colors()) wattron(board_win, COLOR_PAIR(color_pair));

      char piece_char = board->square_piece[square];
      mvwprintw(board_win, draw_rank + line_padding, draw_file * 2 + 1, "%c ",
                piece_char);

      if (has_colors()) wattroff(board_win, COLOR_PAIR(color_pair));
    }
  }
  wrefresh(board_win);
}

/**
 * @brief Render the rank labels on the board
 *
 * @param board_win
 * @param game - Game state to determine orientation
 */
static void printw_rank_labels(WINDOW *board_win, BoardData *game) {
  switch (game->board_orientation) {
    case white_orientation:
      for (int i = 0; i < 8; i++) {
        mvwprintw(board_win, i + line_padding, 0, "%d", 8 - i);
        mvwprintw(board_win, i + line_padding, board_width - 1, "%d", 8 - i);
      }
      break;

    case black_orientation:
      for (int i = 0; i < 8; i++) {
        mvwprintw(board_win, i + line_padding, 0, "%d", i + 1);
        mvwprintw(board_win, i + line_padding, board_width - 1, "%d", i + 1);
      }
      break;
  }
}

/**
 * @brief Render the file labels on the board
 *
 * @param board_win
 */
static void printw_file_labels(WINDOW *board_win) {
  mvwprintw(board_win, 0, 1, "a b c d e f g h");
  mvwprintw(board_win, board_height - 1, 1, "a b c d e f g h");
}

/**
 * @brief Get the square based on the board orientation
 *
 * @param game - Game state to determine orientation
 * @param draw_rank
 * @param draw_file
 * @return int
 */
static int get_square_from_orientation(BoardData *game, int draw_rank,
                                       int draw_file) {
  int rank, file;
  switch (game->board_orientation) {
    case white_orientation:
      rank = 7 - draw_rank;
      file = draw_file;
      return rank * 8 + file;

    case black_orientation:
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
 * @param game - Game state to determine special squares
 * @return int
 */
static int get_square_color(int square, BoardData *game) {
  if (square == game->highlighted_square) return highlighted_square_color;

  if (square == game->selected_square) return selected_square_color;

  return ((square / 8 + square % 8) % 2 == 0) ? white_square_color
                                              : black_square_color;
}
