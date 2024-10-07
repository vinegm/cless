#include "menu.hpp"
#include "utils.hpp"
#include <string>
#include <vector>

#define title_padding (line_padding * 4)
#define menu_padding (title_padding + line_padding)
#define instructions_padding (menu_padding + line_padding)

void MenuWin::draw() {
  WINDOW *menu_win;

  WINDOW *raw_parent_win = this->parent_win.get()->get();
  int parent_height, parent_width;
  getmaxyx(raw_parent_win, parent_height, parent_width);

  werase(raw_parent_win);
  box(raw_parent_win, 0, 0);

  int title_lines_count = this->printw_title(raw_parent_win, parent_width);

  std::vector<std::string> instructions = {"Arrow keys/jk: move cursor",
                                           "Space/Enter: select", "q: quit"};
  int instructions_line =
      title_lines_count + this->options_count + instructions_padding;

  wattroff(raw_parent_win, A_DIM);
  for (int i = 0; i < instructions.size(); i++) {
    mvwprintw_centered(raw_parent_win, parent_width, instructions_line + i,
                       instructions[i]);
  }
  wattroff(raw_parent_win, A_DIM);

  wrefresh(raw_parent_win);

  menu_win = derwin(raw_parent_win, this->options_count, parent_width,
                    title_lines_count + menu_padding, 0);
  navigation_loop(menu_win);

  werase(menu_win);
  delwin(menu_win);
}

void MenuWin::navigation_loop(WINDOW *menu_win) {
  int pressed_key;
  keypad(menu_win, true);

  while (true) {
    printw_menu(menu_win);

    pressed_key = wgetch(menu_win);
    switch (pressed_key) {
      case ' ':
      case 10: select_option(); return;

      case 'k':
      case KEY_UP:
        this->highlight--;
        if (this->highlight < 0) this->highlight = this->options_count - 1;
        break;

      case 'j':
      case KEY_DOWN:
        this->highlight++;
        if (this->highlight >= this->options_count) this->highlight = 0;
        break;

      case 'q': this->handler->event = this->handler->exit_event; return;

      case KEY_RESIZE:
        this->handler->event = this->handler->resize_event;
        return;

      case ERR: break;

      default: break;
    }
  }
}

void MenuWin::select_option() {
  switch (this->highlight) {
    case 0: this->handler->current_window = this->board_win_id; return;

    case 1: this->handler->event = this->handler->exit_event; return;

    case 2: this->handler->event = this->handler->exit_event; return;
  }
}

/**
 * @brief Print the menu options, also handles the highlighting
 *
 * @param win
 * @param this
 */
void MenuWin::printw_menu(WINDOW *win) {
  int win_width, _;
  getmaxyx(win, _, win_width);

  for (int i = 0; i < this->options_count; i++) {
    if (i == this->highlight) { wattron(win, A_REVERSE); }
    mvwprintw_centered(win, win_width, i, this->options[i]);
    if (i == this->highlight) { wattroff(win, A_REVERSE); }
  }
}

/**
 * @brief Print the title of the game
 *
 * @param win
 * @param win_width
 * @return int - The number of lines used by the title
 */
int MenuWin::printw_title(WINDOW *win, int win_width) {
  std::vector<std::wstring> titleLines = {
      L" ██████╗██╗     ███████╗███████╗███████╗",
      L"██╔════╝██║     ██╔════╝██╔════╝██╔════╝",
      L"██║     ██║     █████╗  ███████╗███████╗",
      L"██║     ██║     ██╔══╝  ╚════██║╚════██║",
      L"╚██████╗███████╗███████╗███████║███████║",
      L" ╚═════╝╚══════╝╚══════╝╚══════╝╚══════╝",
  };

  int title_lines_count = titleLines.size();
  for (int i = 0; i < title_lines_count; i++) {
    mvwprintw_centered(win, win_width, i + title_padding, titleLines[i]);
  }

  return title_lines_count;
}
