#include "board.h"
#include "menu.h"
#include "win_handler.h"
#include <locale.h>

#define EXIT_EVENT -1
#define RESIZE_EVENT -2

int main() {
  setlocale(LC_ALL, "");
  initscr();   // Initialize ncurses
  noecho();    // Don't echo input characters
  curs_set(0); // Hide the cursor

  // Initialize colors if supported
  if (has_colors()) {
    start_color();
    use_default_colors();

    init_pair(white_square_color, COLOR_BLACK, COLOR_WHITE);
    init_pair(black_square_color, COLOR_WHITE, COLOR_BLACK);
    init_pair(highlighted_square_color, COLOR_YELLOW, COLOR_BLUE);
    init_pair(selected_square_color, COLOR_WHITE, COLOR_RED);
  }

  WINDOW *main_win = NULL;

  WinHandler tui = {.main_win = main_win,
                    .exit_event = EXIT_EVENT,
                    .resize_event = RESIZE_EVENT};
  BoardState board;
  MenuWinData menu_data;
  BoardWinData board_data;

  WinRef menu_win = {.draw = render_menu, .data = &menu_data};
  WinRef board_win = {.draw = render_board, .data = &board_data};

  add_window(&tui, &menu_win);
  add_window(&tui, &board_win);

  init_menu_data(&menu_data, &tui, board_win.id);
  init_board_data(&board_data, &tui, menu_win.id, &board);

  run_tui(&tui);

  delwin(main_win);
  endwin();

  return 0;
}
