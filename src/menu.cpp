#include "menu.hpp"
#include <string>
#include <vector>

#define title_padding (line_padding * 4)
#define menu_padding (title_padding + line_padding)
#define instructions_padding (menu_padding + line_padding)

void MenuWin::draw() {
  WINDOW *parent_win = this->handler->get_main_win();
  int _, parent_width;
  getmaxyx(parent_win, _, parent_width);

  box(parent_win, 0, 0);

  int title_lines_count = this->printw_title();

  std::vector<std::string> instructions = {"Arrow keys/jk: move cursor",
                                           "Space/Enter: select", "q: quit"};
  int instructions_line =
      title_lines_count + this->options_count + instructions_padding;

  modifier_wrapper(parent_win, A_DIM, [&]() {
    for (int i = 0; i < instructions.size(); i++) {
      mvwprintw_centered(parent_win, parent_width, instructions_line + i,
                         instructions[i]);
    }
  });

  WINDOW *menu_win = derwin(parent_win, this->options_count, parent_width,
                            title_lines_count + menu_padding, 0);
  this->menu_win = UniqueWindow(menu_win);
  this->printw_menu();

  navigation_loop();
}

void MenuWin::navigation_loop() {
  WINDOW *parent_win = this->handler->get_main_win();
  WINDOW *menu_win = this->menu_win.get();

  keypad(menu_win, true);
  int pressed_key;
  while (true) {
    wrefresh(parent_win);
    this->printw_menu();

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
    case 0: this->handler->next_window = this->board_win_name; return;

    case 1: this->handler->event = this->handler->exit_event; return;

    case 2: this->handler->event = this->handler->exit_event; return;
  }
}

/**
 * @brief Prints menu options and handles highlighting
 */
void MenuWin::printw_menu() {
  WINDOW *menu_win = this->menu_win.get();

  int win_width, _;
  getmaxyx(menu_win, _, win_width);

  for (int i = 0; i < this->options_count; i++) {
    if (i == this->highlight) { wattron(menu_win, A_REVERSE); }
    mvwprintw_centered(menu_win, win_width, i, this->options[i]);
    if (i == this->highlight) { wattroff(menu_win, A_REVERSE); }
  }
}

/**
 * @brief Print the title of the game
 *
 * @return int - The number of lines used by the title
 */
int MenuWin::printw_title() {
  WINDOW *parent_win = this->handler->get_main_win();
  int _, parent_width;
  getmaxyx(parent_win, _, parent_width);

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
    mvwprintw_centered(parent_win, parent_width, i + title_padding,
                       titleLines[i]);
  }

  return title_lines_count;
}
