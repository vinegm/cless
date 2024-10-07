#include "utils.hpp"
#include <ncurses.h>
#include <string>

/**
 * @brief Create a centered window object
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

  clear();
  refresh();
  return UniqueWindow(newwin(height, width, y_start, x_start));
}

/**
 * @brief Print a formatted string centered in a window at a specific line.
 *
 * @param win The window to print to.
 * @param win_width The width of the window.
 * @param line The line number to print the string on.
 * @param msg The formatted string to print.
 */
void mvwprintw_centered(WINDOW *win, int win_width, int line,
                        const std::wstring msg) {
  int str_width = wcswidth(msg.c_str(), msg.size());
  if (str_width < 0) str_width = 0;

  int x_start = (win_width - str_width) / 2;
  if (x_start < 0) x_start = 0;

  mvwprintw(win, line, x_start, "%ls", msg.c_str());
}

// --- UTF-8 string version (for std::string) ---
void mvwprintw_centered(WINDOW *win, int win_width, int line,
                        const std::string msg) {
  std::wstring wmsg(msg.begin(), msg.end()); // Convert to wide string
  mvwprintw_centered(win, win_width, line, wmsg);
}
