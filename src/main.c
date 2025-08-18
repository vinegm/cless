#include "board.h"
#include "common.h"
#include "engine.h"
#include "menu.h"
#include "utils.h"
#include <locale.h>
#include <ncurses.h>

static WINDOW *g_main_win = NULL;

void main_loop();
void play_chess_game();

int main() {
  setlocale(LC_ALL, "");
  initscr();   // Initialize ncurses
  noecho();    // Don't echo input characters
  curs_set(0); // Hide the cursor

  // Initialize colors if supported
  if (has_colors()) {
    start_color();
    use_default_colors();

    init_pair(1, COLOR_BLACK, COLOR_WHITE); // Light squares
    init_pair(2, COLOR_YELLOW, COLOR_BLUE); // Highlighted square
    init_pair(3, COLOR_WHITE, COLOR_RED);   // Selected square
    init_pair(4, COLOR_WHITE, COLOR_BLACK); // Dark squares
  }

  main_loop();

  if (g_main_win) { delwin(g_main_win); }
  endwin();

  return 0;
}

void main_loop() {
  char *opts[] = {"Player vs Robot", "Player vs Player", "Exit"};
  enum { player_vs_robot = 0, player_vs_player = 1, exit_game = 2 };

  MenuOptions menu_options = {.options = opts,
                              .optionsCount = len(opts),
                              .highlight = player_vs_robot,
                              .exitOptionIndex = exit_game};

  handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
  while (TRUE) {
    render_menu(g_main_win, &menu_options);

    switch (menu_options.selectedOption) {
      case player_vs_robot: play_chess_game(); break;

      case player_vs_player: break;

      case RESIZE_EVENT:
        handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
        break;

      case exit_game: return; break;
    }
  };
}

void play_chess_game() {
  int game_status = 0;
  ChessBoardState board;
  BoardDisplay display = {
      .selectedSquare = -1, .highlightedSquare = 28, .boardWin = &g_main_win};

  init_chess_board(&board);

  handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);

  while (TRUE) {
    handle_board_input(&display, &board, &game_status);

    switch (game_status) {
      // TODO: Remove this magic number
      case -1: handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH); return;

      case RESIZE_EVENT:
        handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
        break;
    }
  };
}
