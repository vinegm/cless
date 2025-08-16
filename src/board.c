#include "board.h"
#include "common.h"
#include "utils.h"
#include <ncurses.h>
#include <string.h>

void render_board(WINDOW *parent_win, ChessBoardState *board,
                  BoardDisplay *display) {
  WINDOW *board_win;
  int parent_height, parent_width;
  getmaxyx(parent_win, parent_height, parent_width);

  werase(parent_win);
  box(parent_win, 0, 0);

  wattron(parent_win, A_BOLD);
  mvwprintw_centered(parent_win, parent_width, 1, "♛ CLESS - Chess Game ♛");
  wattroff(parent_win, A_BOLD);

  int board_start_y = 4;
  int board_start_x = (parent_width - 18) / 2;
  if (board_start_x < 3) board_start_x = 3;

  mvwprintw(parent_win, board_start_y - 1, board_start_x + 2,
            " a b c d e f g h");

  for (int rank = 7; rank >= 0; rank--) {
    int y = board_start_y + (7 - rank);

    mvwprintw(parent_win, y, board_start_x - 1, "%d", rank + 1);

    mvwprintw(parent_win, y, board_start_x + 18, "%d", rank + 1);
  }

  mvwprintw(parent_win, board_start_y + 8, board_start_x + 2,
            " a b c d e f g h");

  // TODO: use mvwprintw_centered, currently unable to pass the formating
  const char *to_move_text =
      (board->to_move == WHITE) ? "♔ White to move" : "♚ Black to move";
  int info_y = board_start_y + 10;

  wattron(parent_win, A_BOLD);
  mvwprintw(parent_win, info_y, (parent_width - strlen(to_move_text)) / 2, "%s",
            to_move_text);
  wattroff(parent_win, A_BOLD);

  char *instructions[] = {"Arrow keys/hjkl: move cursor", "Space/Enter: select",
                          "q: return to menu", NULL};
  int instructions_y = parent_height - 4;

  wattron(parent_win, A_DIM);
  for (int i = 0; instructions[i] != NULL; i++) {
    mvwprintw_centered(parent_win, parent_width, instructions_y + i,
                       instructions[i]);
  }
  wattroff(parent_win, A_DIM);
}

void handle_board_input(BoardDisplay *display, ChessBoardState *board,
                        int *game_status) {
  int pressed_key;
  keypad(*display->boardWin, TRUE);

  while (TRUE) {
    // TODO: re-render only the board squares, not the whole window
    render_board(*display->boardWin, board, display);

    pressed_key = wgetch(*display->boardWin);

    int current_rank = display->highlightedSquare / 8;
    int current_file = display->highlightedSquare % 8;

    switch (pressed_key) {
      case ' ':
      case 10:
        // TODO: handle piece selection and movement
        break;

      case 'k':
      case KEY_UP:
        // Placeholder
        break;

      case 'j':
      case KEY_DOWN:
        // Placeholder
        break;

      case 'h':
      case KEY_LEFT:
        // Placeholder
        break;

      case 'l':
      case KEY_RIGHT:
        // Placeholder
        break;

      // TODO: Remove this magic number
      case 'q': *game_status = -1; return;

      case KEY_RESIZE: *game_status = RESIZE_EVENT; return;

      case ERR: break;

      default: break;
    }
  }
}
