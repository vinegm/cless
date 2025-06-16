#include <ncurses.h>

void highlight_selected(WINDOW *win, char choices[][20], int selectedOption) {
  for (int i = 0; i < 3; i++) {
    if (i == selectedOption)
      wattron(win, A_REVERSE);
    mvwprintw(win, i + 7, 1, "%s", choices[i]);
    wattroff(win, A_REVERSE);
  }
}

int menu_loop(WINDOW *menuWin, char choices[][20], int choicesCount) {
  int choice;
  int highlight = 0;

  while (1) {
    highlight_selected(menuWin, choices, highlight);
    choice = wgetch(menuWin);
    switch (choice) {
    case 'k':
      highlight--;
      if (highlight == -1)
        highlight = 0;
      break;

    case 'j':
      highlight++;
      if (highlight == choicesCount)
        highlight = 2;
      break;

    default:
      break;
    }

    if (choice == 10)
      return highlight;
  }
}
