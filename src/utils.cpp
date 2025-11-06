#include "utils.hpp"

#include <algorithm>
#include <ncurses.h>
#include <string>
#include <vector>

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
 * @brief Create a popup with a message, also allows a list of options
 *
 * @param message Message to display in the popup
 * @param options Vector of option strings to display, empty for just an OK popup
 * @param exit_status The return value if the popup is cancelled (no options)
 * @param resize_status The return value if the popup is resized
 * @return int The index of the chosen option, exit_status if cancelled
 */
int create_popup(
    const std::string &message,
    const std::vector<std::string> &options,
    int exit_status,
    int resize_status
) {
  int message_width = message.length();
  int max_option_width = 0;
  for (const auto &option : options) {
    max_option_width = std::max(max_option_width, (int)option.length());
  }

  int window_width =
      std::max(message_width, max_option_width) + 4; // 2 for padding and borders on each side
  int window_height = line_padding * 5; // 2 for borders + 1 for message + 2 for padding

  if (options.size() != 0) { window_height += options.size() + 1; } // 1 for padding

  UniqueWindow popup_win = create_centered_window(window_height, window_width);
  WINDOW *popup_win_ptr = popup_win.get();
  box(popup_win_ptr, 0, 0);

  int options_count = options.size();
  int pressed_key;
  if (options_count == 0) {
    mvwprintw_centered(popup_win_ptr, window_width, 2, message);

    wrefresh(popup_win_ptr);
    pressed_key = wgetch(popup_win_ptr);

    if (pressed_key == KEY_RESIZE) return resize_status;
    return exit_status;
  }

  keypad(popup_win_ptr, true);

  int current_selection = 0;
  while (true) {
    wrefresh(popup_win_ptr);

    mvwprintw_centered(popup_win_ptr, window_width, line_padding * 2, message);
    for (int i = 0; i < options_count; i++) {
      int line = (line_padding * 4) + i;

      if (i != current_selection) {
        mvwprintw_centered(popup_win_ptr, window_width, line, options[i]);
        continue;
      }

      modifier_wrapper(popup_win_ptr, A_REVERSE, [&]() {
        mvwprintw_centered(popup_win_ptr, window_width, line, options[i]);
      });
    }

    pressed_key = wgetch(popup_win_ptr);
    if (handle_menu_key(pressed_key, current_selection, options_count)) {
      return current_selection;
    }

    switch (pressed_key) {
      case 'q': return exit_status;
      case KEY_RESIZE: return resize_status;
      case ERR: break;
      default: break;
    }
  }
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
