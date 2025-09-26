#include "menu.h"
#include "utils.h"

static void navigation_loop(WINDOW *menu_win, MenuData *menu_opts);
static void select_option(MenuData *menu_opts);
static void printw_menu(WINDOW *win, MenuData *menu_opts);
static int printw_title(WINDOW *win, int win_width);

#define title_padding (line_padding * 4)
#define menu_padding (title_padding + line_padding)
#define instructions_padding (menu_padding + line_padding)

void init_menu_data(MenuData *menu_data, TuiHandler *tui, int board_win_id) {
  menu_data->tui = tui;
  menu_data->board_win_id = board_win_id;

  static char *options[] = {"Player vs Robot", "Player vs Player", "Exit"};
  menu_data->options = options;
  menu_data->options_count = len(options);

  menu_data->highlight = 0;
}

void render_menu(WINDOW *parent_win, void *menu_data) {
  MenuData *menu_opts = (MenuData *)menu_data;
  WINDOW *menu_win;
  int parent_height, parent_width;
  getmaxyx(parent_win, parent_height, parent_width);

  werase(parent_win);
  box(parent_win, 0, 0);

  int title_lines_count = printw_title(parent_win, parent_width);

  const char *instructions[] = {"Arrow keys/jk: move cursor",
                                "Space/Enter: select", "q: quit"};
  int instructions_line =
      title_lines_count + menu_opts->options_count + instructions_padding;

  wattron(parent_win, A_DIM);
  for (int i = 0; i < len(instructions); i++) {
    mvwprintw_centered(parent_win, parent_width, instructions_line + i,
                       instructions[i]);
  }
  wattroff(parent_win, A_DIM);

  wrefresh(parent_win);

  menu_win = derwin(parent_win, menu_opts->options_count, parent_width,
                    title_lines_count + menu_padding, 0);
  navigation_loop(menu_win, menu_opts);

  werase(menu_win);
  delwin(menu_win);
}

static void navigation_loop(WINDOW *menu_win, MenuData *menu_opts) {
  int pressed_key;
  keypad(menu_win, TRUE);

  while (TRUE) {
    printw_menu(menu_win, menu_opts);

    pressed_key = wgetch(menu_win);
    switch (pressed_key) {
      case ' ':
      case 10: select_option(menu_opts); return;

      case 'k':
      case KEY_UP:
        menu_opts->highlight--;
        if (menu_opts->highlight < 0)
          menu_opts->highlight = menu_opts->options_count - 1;
        break;

      case 'j':
      case KEY_DOWN:
        menu_opts->highlight++;
        if (menu_opts->highlight >= menu_opts->options_count)
          menu_opts->highlight = 0;
        break;

      case 'q': menu_opts->tui->event = menu_opts->tui->exit_event; return;

      case KEY_RESIZE:
        menu_opts->tui->event = menu_opts->tui->resize_event;
        return;

      case ERR: break;

      default: break;
    }
  }
}

static void select_option(MenuData *menu_opts) {
  switch (menu_opts->highlight) {
    case 0: menu_opts->tui->current_window = menu_opts->board_win_id; return;

    case 1: menu_opts->tui->event = menu_opts->tui->exit_event; return;

    case 2: menu_opts->tui->event = menu_opts->tui->exit_event; return;
  }
}

/**
 * @brief Print the menu options, also handles the highlighting
 *
 * @param win
 * @param menu_opts
 */
static void printw_menu(WINDOW *win, MenuData *menu_opts) {
  int win_width, _;
  getmaxyx(win, _, win_width);

  for (int i = 0; i < menu_opts->options_count; i++) {
    if (i == menu_opts->highlight) { wattron(win, A_REVERSE); }
    mvwprintw_centered(win, win_width, i, menu_opts->options[i]);
    if (i == menu_opts->highlight) { wattroff(win, A_REVERSE); }
  }
}

// TODO: handle if the terminal does not support UTF-8
/**
 * @brief Print the title of the game
 *
 * @param win
 * @param win_width
 * @return int - The number of lines used by the title
 */
static int printw_title(WINDOW *win, int win_width) {
  const char *titleLines[] = {
      " ██████╗██╗     ███████╗███████╗███████╗",
      "██╔════╝██║     ██╔════╝██╔════╝██╔════╝",
      "██║     ██║     █████╗  ███████╗███████╗",
      "██║     ██║     ██╔══╝  ╚════██║╚════██║",
      "╚██████╗███████╗███████╗███████║███████║",
      " ╚═════╝╚══════╝╚══════╝╚══════╝╚══════╝",
  };

  int title_lines_count = len(titleLines);
  for (int i = 0; i < title_lines_count; i++) {
    mvwprintw_centered(win, win_width, i + title_padding, "%s", titleLines[i]);
  }

  return title_lines_count;
}
