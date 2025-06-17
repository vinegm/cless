#include "menu.h"
#include <locale.h>
#include <ncurses.h>

void main_loop(WINDOW *mainWin);

int main() {
  WINDOW *mainWin;
  int scrMaxY, scrMaxX;

  setlocale(LC_ALL, "");
  initscr();   // Initialize ncurses
  noecho();    // Don't echo input characters
  curs_set(0); // Hide the cursor

  getmaxyx(stdscr, scrMaxY, scrMaxX);
  mainWin = newwin(15, 41, scrMaxY / 4, scrMaxX / 4);

  main_loop(mainWin);

  // Clean up
  delwin(mainWin);
  endwin();

  return 0;
}

void main_loop(WINDOW *mainWin) {
  int scrMaxY, scrMaxX;
  char *opts[] = {"Player vs Robot", "Player vs Player", "Options", "Exit"};

  MenuOptions menuOptions = {.options = opts,
                             .optionsCount = sizeof(opts) / sizeof((opts)[0])};

  getmaxyx(mainWin, scrMaxY, scrMaxX);

  do {
    menu(mainWin, &menuOptions);
  } while (menuOptions.selectedOption != menuOptions.optionsCount - 1);
}
