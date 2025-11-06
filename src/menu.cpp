#include "menu.hpp"

#include "utils.hpp"

#include <ncurses.h>
#include <string>
#include <vector>

#define title_padding (line_padding * 4)
#define menu_padding (title_padding + line_padding)
#define instructions_padding (menu_padding + line_padding)

void MenuWin::draw() {
  WINDOW *parent_win = handler->get_main_win();
  int _, parent_width;
  getmaxyx(parent_win, _, parent_width);

  box(parent_win, 0, 0);

  int title_lines_count = printw_title();

  std::vector<std::string> instructions =
      {"Arrow keys/jk: move cursor", "Space/Enter: select", "q: quit"};
  int instructions_line = title_lines_count + options_count + instructions_padding;

  modifier_wrapper(parent_win, A_DIM, [&]() {
    for (int i = 0; i < instructions.size(); i++) {
      mvwprintw_centered(parent_win, parent_width, instructions_line + i, instructions[i]);
    }
  });

  WINDOW *menu_win_ptr =
      derwin(parent_win, options_count, parent_width, title_lines_count + menu_padding, 0);
  menu_win = UniqueWindow(menu_win_ptr);
  printw_menu();

  navigation_loop();
}

void MenuWin::navigation_loop() {
  WINDOW *parent_win = handler->get_main_win();
  WINDOW *menu_win_ptr = menu_win.get();

  keypad(menu_win_ptr, true);
  int pressed_key;
  while (true) {
    wrefresh(parent_win);
    printw_menu();

    pressed_key = wgetch(menu_win_ptr);

    if (handle_menu_key(pressed_key, highlight, options_count)) {
      select_option();
      return;
    }

    switch (pressed_key) {
      case 'q': handler->event = handler->exit_event; return;
      case KEY_RESIZE: handler->event = handler->resize_event; return;
      case ERR: break;
      default: break;
    }
  }
}

void MenuWin::select_option() {
  switch (highlight) {
    case 0:
      playing_engine = false;
      handler->next_window = board_win_name;
      return;

    case 1:
      if (has_engine) {
        playing_engine = true;
        handler->next_window = board_win_name;
      } else {
        int pop_event = create_popup("No engine available", {}, 0, handler->resize_event);
        handler->event = pop_event;
      }
      return;

    case 2: handler->event = handler->exit_event; return;
  }
}

/**
 * @brief Prints menu options and handles highlighting
 */
void MenuWin::printw_menu() {
  WINDOW *menu_win_ptr = menu_win.get();

  int win_width, _;
  getmaxyx(menu_win_ptr, _, win_width);

  for (int i = 0; i < options_count; i++) {
    if (i == highlight) { wattron(menu_win_ptr, A_REVERSE); }
    if (i == 1 && !has_engine) { wattron(menu_win_ptr, A_DIM); }
    mvwprintw_centered(menu_win_ptr, win_width, i, options[i]);
    if (i == 1 && !has_engine) { wattroff(menu_win_ptr, A_DIM); }
    if (i == highlight) { wattroff(menu_win_ptr, A_REVERSE); }
  }
}

/**
 * @brief Print the title of the game
 *
 * @return int - The number of lines used by the title
 */
int MenuWin::printw_title() {
  WINDOW *parent_win = handler->get_main_win();
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
    mvwprintw_centered(parent_win, parent_width, i + title_padding, titleLines[i]);
  }

  return title_lines_count;
}
