#include "board.h"
#include "engine.h"
#include "menu.h"
#include "utils.h"
#include <locale.h>
#include <ncurses.h>

#define RESIZE_EVENT -2
#define EXIT_EVENT -1

static WINDOW *g_main_win = NULL;

void main_loop();
void play_loop();

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

  main_loop();

  if (g_main_win) { delwin(g_main_win); }
  endwin();

  return 0;
}

void main_loop() {
  char *opts[] = {"Player vs Robot", "Player vs Player", "Exit"};
  enum { player_vs_robot = 0, player_vs_player, exit_game };

  MenuOptions menu_options = {.options = opts,
                              .options_count = len(opts),
                              .highlight = player_vs_robot,
                              .exit_event = EXIT_EVENT,
                              .resize_event = RESIZE_EVENT};

  handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
  while (TRUE) {
    render_menu(g_main_win, &menu_options);

    switch (menu_options.selected_option) {
      case player_vs_robot: play_loop(); break;

      case player_vs_player: break;

      case RESIZE_EVENT:
        handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
        break;

      case EXIT_EVENT:
      case exit_game: return;
    }
  };
}

void play_loop() {
  ChessBoardState board;
  GameWinState game = {.selected_square = -1,
                       .highlighted_square = 28,
                       .boardWin = &g_main_win,
                       .board_orientation = white_orientation,
                       .exit_event = EXIT_EVENT,
                       .resize_event = RESIZE_EVENT};

  init_chess_board(&board);

  handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);

  while (TRUE) {
    render_board(g_main_win, &board, &game);

    switch (game.status) {
      case EXIT_EVENT:
        handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
        return;

      case RESIZE_EVENT:
        handle_win(&g_main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
        break;
    }
  };
}
