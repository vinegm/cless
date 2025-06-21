#include "menu.h"

static const char *title = " ██████╗██╗     ███████╗███████╗███████╗\n"
                           "██╔════╝██║     ██╔════╝██╔════╝██╔════╝\n"
                           "██║     ██║     █████╗  ███████╗███████╗\n"
                           "██║     ██║     ██╔══╝  ╚════██║╚════██║\n"
                           "╚██████╗███████╗███████╗███████║███████║\n"
                           " ╚═════╝╚══════╝╚══════╝╚══════╝╚══════╝\n";

static void highlight_selected(WINDOW *win, MenuOptions *menuOptions);
static void navigation_loop(WINDOW *menuWin, MenuOptions *menuOptions);

void menu(WINDOW *parentWin, MenuOptions *menuOptions) {
  WINDOW *menuWin;
  int highlight = 0;
  int mainMaxY, mainMaxX;

  mvwprintw(parentWin, 0, 0, "%s", title);
  wrefresh(parentWin);

  getmaxyx(parentWin, mainMaxY, mainMaxX);
  menuWin = derwin(parentWin, 5, 20, 8, 10);

  navigation_loop(menuWin, menuOptions);

  werase(menuWin);
  delwin(menuWin);
}

static void navigation_loop(WINDOW *menuWin, MenuOptions *menuOptions) {
  int choice;
  while (1) {
    highlight_selected(menuWin, menuOptions);

    choice = wgetch(menuWin);
    switch (choice) {
      case 'k':
        menuOptions->highlight--;
        if (menuOptions->highlight == -1) menuOptions->highlight = 0;
        break;

      case 'j':
        menuOptions->highlight++;
        if (menuOptions->highlight >= menuOptions->optionsCount)
          menuOptions->highlight = menuOptions->optionsCount - 1;
        break;

      default: break;
    }

    if (choice == 10) {
      menuOptions->selectedOption = menuOptions->highlight;
      break;
    }
  }
}

static void highlight_selected(WINDOW *win, MenuOptions *menuOptions) {
  for (int i = 0; i < menuOptions->optionsCount; i++) {
    if (i == menuOptions->highlight) wattron(win, A_REVERSE);
    mvwprintw(win, i, 0, "%s", menuOptions->options[i]);
    wattroff(win, A_REVERSE);
  }
}
