#include "board.hpp"
#include "menu.hpp"
#include "size_warning.hpp"
#include "win_handler.hpp"

#include <ncurses.h>
#include <sys/types.h>

#define WINDOW_HEIGHT 21
#define WINDOW_WIDTH 50

struct Args {
  std::string engine_cmd = "";
};

Args parse_args(int argc, char *argv[]);

int main(int argc, char *argv[]) {
  Args args = parse_args(argc, argv);
  setlocale(LC_ALL, "");
  initscr();   // Initialize ncurses
  noecho();    // Don't echo input characters
  curs_set(0); // Hide the cursor

  if (has_colors()) {
    start_color();
    use_default_colors();

    init_pair(WHITE_SQUARE, COLOR_BLACK, COLOR_WHITE);
    init_pair(BLACK_SQUARE, COLOR_WHITE, COLOR_BLACK);
    init_pair(HIGHLIGHTED_SQUARE, COLOR_RED, COLOR_BLUE);
    init_pair(SELECTED_SQUARE, COLOR_WHITE, COLOR_RED);
    init_pair(LEGAL_MOVE_SQUARE, COLOR_GREEN, COLOR_YELLOW);
  }

  GameState game_state = GameState(args.engine_cmd);

  WinHandler handler(WINDOW_HEIGHT, WINDOW_WIDTH, "size_warning");
  handler.add_window<MenuWin>(
      "menu",
      MenuWinArgs{
          "board",
          game_state.ongoing_game,
          game_state.has_engine(),
          game_state.playing_engine
      }
  );
  handler.add_window<BoardWin>("board", BoardWinArgs{"menu", game_state});
  handler.add_window<SizeWarningWin>("size_warning");
  handler.run("menu");

  endwin();

  return 0;
}

Args parse_args(int argc, char *argv[]) {
  Args args{};

  for (int i = 1; i < argc; i++) {
    std::string arg = argv[i];
    if (arg == "--engine" && i + 1 < argc) {
      i++;
      args.engine_cmd = argv[i];
      continue;
    }
  }

  return args;
}
