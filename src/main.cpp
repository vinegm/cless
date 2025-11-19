#include "board.hpp"
#include "game_logic.hpp"
#include "menu.hpp"
#include "size_warning.hpp"
#include "win_handler.hpp"

#include <ncurses.h>
#include <sys/types.h>

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

    init_pair(static_cast<int>(SquareColor::WHITE), COLOR_BLACK, COLOR_WHITE);
    init_pair(static_cast<int>(SquareColor::BLACK), COLOR_WHITE, COLOR_BLACK);
    init_pair(static_cast<int>(SquareColor::HIGHLIGHTED), COLOR_RED, COLOR_BLUE);
    init_pair(static_cast<int>(SquareColor::SELECTED), COLOR_WHITE, COLOR_RED);
    init_pair(static_cast<int>(SquareColor::LEGAL_MOVE), COLOR_GREEN, COLOR_YELLOW);
  }

  GameState game_state = GameState(args.engine_cmd);

  TuiState tui_state(19, 46, game_state);
  tui_state.menu_win_name = "menu";
  tui_state.board_win_name = "board";

  WinHandler<TuiState> handler(tui_state);
  handler.add_window<MenuWin>("menu");
  handler.add_window<BoardWin>("board");
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
