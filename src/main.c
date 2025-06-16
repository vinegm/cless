#include "menu.h"
#include <locale.h>
#include <ncurses.h>

void loop();

int main() {
  setlocale(LC_ALL, "");

  // Initialize ncurses
  initscr();
  noecho();
  curs_set(0);
  cbreak();
  // raw();

  WINDOW *menuWin;
  int scrMaxY, scrMaxX;
  int selected;
  char choices[3][20] = {"choice1", "choice2", "choice3"};
  int numChoices = sizeof(choices) / sizeof(choices[0]);

  getmaxyx(stdscr, scrMaxY, scrMaxX);

  menuWin = newwin(26, scrMaxX / 2, scrMaxY / 2, 25);
  box(menuWin, 0, 0);
  refresh();
  keypad(menuWin, TRUE);

  char *title = " ██████╗██╗     ███████╗███████╗███████╗\n"
                "██╔════╝██║     ██╔════╝██╔════╝██╔════╝\n"
                "██║     ██║     █████╗  ███████╗███████╗\n"
                "██║     ██║     ██╔══╝  ╚════██║╚════██║\n"
                "╚██████╗███████╗███████╗███████║███████║\n"
                " ╚═════╝╚══════╝╚══════╝╚══════╝╚══════╝\n";
  mvwprintw(menuWin, 1, 0, "%s", title);

  selected = menu_loop(menuWin, choices, numChoices);

  printw("you chose: %s", choices[selected]);

  getch();
  endwin();

  return 0;
}

void loop() {}
