#include "menu.h"
#include "common.h"
#include "utils.h"

static void navigation_loop(WINDOW *menu_win, MenuOptions *menu_opts);
static void printw_menu(WINDOW *win, MenuOptions *menu_opts);

void render_menu(WINDOW *parent_win, MenuOptions *menu_opts) {
  WINDOW *menu_win;
  int parent_height, parent_width;
  getmaxyx(parent_win, parent_height, parent_width);

  werase(parent_win);
  box(parent_win, 0, 0);

  // TODO: Make the centering dynamic
  // due to wide characters it does not work with the title
  int titleStartX = (parent_width - 40) / 2;
  if (titleStartX < 1) titleStartX = 1;

  const char *titleLines[] = {"", // Padding
                              "", // Padding
                              " ██████╗██╗     ███████╗███████╗███████╗",
                              "██╔════╝██║     ██╔════╝██╔════╝██╔════╝",
                              "██║     ██║     █████╗  ███████╗███████╗",
                              "██║     ██║     ██╔══╝  ╚════██║╚════██║",
                              "╚██████╗███████╗███████╗███████║███████║",
                              " ╚═════╝╚══════╝╚══════╝╚══════╝╚══════╝",
                              "", // Padding
                              NULL};

  int title_lines_count = 0;
  for (int i = 0; titleLines[i] != NULL; i++) {
    mvwprintw(parent_win, i, titleStartX, "%s", titleLines[i]);
    title_lines_count++;
  }

  const char *instructions[] = {"", "Arrow keys/jk: move cursor",
                                "Space/Enter: select", "q: quit", NULL};
  int instructions_y = title_lines_count + menu_opts->optionsCount;

  wattron(parent_win, A_DIM);
  for (int i = 0; instructions[i] != NULL; i++) {
    mvwprintw_centered(parent_win, parent_width, instructions_y + i,
                       instructions[i]);
  }
  wattroff(parent_win, A_DIM);

  wrefresh(parent_win);

  menu_win = derwin(parent_win, menu_opts->optionsCount + 1, parent_width,
                    title_lines_count, 0);
  navigation_loop(menu_win, menu_opts);

  werase(menu_win);
  delwin(menu_win);
}

static void navigation_loop(WINDOW *menu_win, MenuOptions *menu_opts) {
  int pressed_key;
  keypad(menu_win, TRUE);

  while (TRUE) {
    printw_menu(menu_win, menu_opts);

    pressed_key = wgetch(menu_win);
    switch (pressed_key) {
      case ' ':
      case 10: menu_opts->selectedOption = menu_opts->highlight; return;

      case 'k':
      case KEY_UP:
        menu_opts->highlight--;
        if (menu_opts->highlight < 0)
          menu_opts->highlight = menu_opts->optionsCount - 1;
        break;

      case 'j':
      case KEY_DOWN:
        menu_opts->highlight++;
        if (menu_opts->highlight >= menu_opts->optionsCount)
          menu_opts->highlight = 0;
        break;

      case 'q': menu_opts->selectedOption = menu_opts->exitOptionIndex; return;

      case KEY_RESIZE: menu_opts->selectedOption = RESIZE_EVENT; return;

      case ERR: break;

      default: break;
    }
  }
}

static void printw_menu(WINDOW *win, MenuOptions *menu_opts) {
  int win_width, _;
  getmaxyx(win, _, win_width);

  for (int i = 0; i < menu_opts->optionsCount; i++) {
    if (i == menu_opts->highlight) { wattron(win, A_REVERSE); }
    mvwprintw_centered(win, win_width, i, menu_opts->options[i]);
    if (i == menu_opts->highlight) { wattroff(win, A_REVERSE); }
  }
}
