#include "board.hpp"
#include "menu.hpp"
#include "size_warning.hpp"
#include "win_handler.hpp"
#include <clocale>

#define WINDOW_HEIGHT 21
#define WINDOW_WIDTH 50

int main() {
  setlocale(LC_ALL, "");
  initscr();   // Initialize ncurses
  noecho();    // Don't echo input characters
  curs_set(0); // Hide the cursor

  // Initialize colors if supported
  if (has_colors()) {
    start_color();
    use_default_colors();

    init_pair(WHITE_SQUARE, COLOR_BLACK, COLOR_WHITE);
    init_pair(BLACK_SQUARE, COLOR_WHITE, COLOR_BLACK);
    init_pair(HIGHLIGHTED_SQUARE, COLOR_YELLOW, COLOR_BLUE);
    init_pair(SELECTED_SQUARE, COLOR_WHITE, COLOR_RED);
  }

  WinHandler handler(WINDOW_HEIGHT, WINDOW_WIDTH);
  handler.add_window(std::make_unique<MenuWin>());
  handler.add_window(std::make_unique<SizeWarningWin>());
  handler.add_window(std::make_unique<BoardWin>());

  handler.run();

  endwin();

  return 0;
}
