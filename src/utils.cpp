#include "utils.hpp"

#include <ncurses.h>
#include <string>

/**
 * @brief Create a centered window object, does not refresh the window.
 *
 * @param height The height of the window.
 * @param width The width of the window.
 */
UniqueWindow create_centered_window(int height, int width) {
  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  int y_start = (scr_height - height) / 2;
  int x_start = (scr_width - width) / 2;

  if (y_start < 0) y_start = 0;
  if (x_start < 0) x_start = 0;
  if (height > scr_height) height = scr_height;
  if (width > scr_width) width = scr_width;

  return UniqueWindow(newwin(height, width, y_start, x_start));
}

/**
 * @brief Handle a single menu navigation key press
 *
 * @param key The key that was pressed
 * @param current_selection Reference to the current selection index (will be modified)
 * @param options_count Total number of options
 * @return bool true if selection was confirmed (Enter/Space), false otherwise
 */
bool handle_menu_key(int key, int &current_selection, int options_count) {
  switch (key) {
    case ' ':
    case 10: return true;

    case 'k':
    case KEY_UP:
      current_selection--;
      if (current_selection < 0) current_selection = options_count - 1;
      break;

    case 'j':
    case KEY_DOWN:
      current_selection++;
      if (current_selection >= options_count) current_selection = 0;
      break;

    default: break;
  }

  return false;
}

/**
 * @brief Wrapper function to apply a modifier to a window and call a callback.
 *
 * @param win
 * @param modifier
 * @param callaback
 */
void modifier_wrapper(WINDOW *win, unsigned int modifier, std::function<void()> callaback) {
  wattron(win, modifier);
  callaback();
  wattroff(win, modifier);
}

/**
 * @brief Print a formatted string centered in a window at a specific line.
 *
 * @param win The window to print to.
 * @param win_width The width of the window.
 * @param line The line number to print the string on.
 * @param msg The formatted string to print.
 */
void mvwprintw_centered(WINDOW *win, int win_width, int line, const std::wstring msg) {
  int str_width = wcswidth(msg.c_str(), msg.size());
  if (str_width < 0) str_width = 0;

  int x_start = (win_width - str_width) / 2;
  if (x_start < 0) x_start = 0;

  mvwprintw(win, line, x_start, "%ls", msg.c_str());
}

// --- UTF-8 string version (for std::string) ---
void mvwprintw_centered(WINDOW *win, int win_width, int line, const std::string msg) {
  std::wstring wmsg(msg.begin(), msg.end()); // Convert to wide string
  mvwprintw_centered(win, win_width, line, wmsg);
}
