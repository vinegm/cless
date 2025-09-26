#define _XOPEN_SOURCE 700
#include "utils.h"
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

static void show_size_warning();
static int check_terminal_size();
static void create_centered_window(WINDOW **win, int height, int width);

/**
 * @brief Create a centered window object
 *
 * @param win A pointer to the window pointer to create.
 * @param height The height of the window.
 * @param width The width of the window.
 */
static void create_centered_window(WINDOW **win, int height, int width) {
  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  int y_start = (scr_height - height) / 2;
  int x_start = (scr_width - width) / 2;

  if (y_start < 0) y_start = 0;
  if (x_start < 0) x_start = 0;
  if (height > scr_height) height = scr_height;
  if (width > scr_width) width = scr_width;

  if (*win) {
    delwin(*win);
    *win = NULL;
  }

  clear();
  refresh();
  *win = newwin(height, width, y_start, x_start);
}

/**
 * @brief Show a warning message if the terminal size is insufficient.
 */
static void show_size_warning() {
  WINDOW *warning_win = NULL;
  int warning_height = 10;
  int warning_width = 60;

  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  create_centered_window(&warning_win, warning_height, warning_width);
  box(warning_win, 0, 0);

  wattron(warning_win, A_BOLD | A_REVERSE);
  mvwprintw_centered(warning_win, warning_width, 1, "TERMINAL SIZE WARNING");
  wattroff(warning_win, A_BOLD | A_REVERSE);

  mvwprintw_centered(warning_win, warning_width, 2,
                     "Your terminal is too small to display CLESS properly.");

  mvwprintw_centered(warning_win, warning_width, 4, "Current size: %dx%d",
                     scr_width, scr_height);
  mvwprintw_centered(warning_win, warning_width, 5,
                     "Required size: %dx%d (minimum)", MIN_TERMINAL_WIDTH,
                     MIN_TERMINAL_HEIGHT);

  mvwprintw_centered(warning_win, warning_width, 7,
                     "Please resize your terminal or use a smaller font.");

  wattron(warning_win, A_BOLD);
  mvwprintw_centered(warning_win, warning_width, 8,
                     "Press any key to continue or 'q' to quit...");
  wattroff(warning_win, A_BOLD);

  wrefresh(warning_win);

  int pressed_key = wgetch(warning_win);

  delwin(warning_win);

  if (pressed_key == 'q' || pressed_key == 'Q') {
    endwin();
    exit(0);
  }
}

/**
 * @brief Check if the terminal size is sufficient for the application.
 *
 * @return int 1 if the terminal size is sufficient, 0 otherwise.
 */
static int check_terminal_size() {
  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  int is_wide = scr_width >= MIN_TERMINAL_WIDTH;
  int is_tall = scr_height >= MIN_TERMINAL_HEIGHT;

  return (is_wide && is_tall);
}

/**
 * @brief Create a centered window with the specified dimensions.
 *
 * @param win A pointer to the window pointer to create.
 * @param height The height of the window.
 * @param width The width of the window.
 */
void refresh_win(WINDOW **win, int height, int width) {
  while (!check_terminal_size()) {
    if (*win) {
      delwin(*win);
      *win = NULL;
    }
    show_size_warning();
  }

  create_centered_window(win, height, width);
  return;
}

/**
 * @brief Print a formatted string centered in a window at a specific line.
 *
 * @param win The window to print to.
 * @param win_width The width of the window.
 * @param line The line number to print the string on.
 * @param fmt The printf-style format string.
 * @param ... Additional arguments for the format string. Buffer limit of 1024
 * bytes.
 */
void mvwprintw_centered(WINDOW *win, int win_width, int line, const char *fmt,
                        ...) {
  char buffer[1024];

  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, sizeof(buffer), fmt, args);
  va_end(args);

  wchar_t wbuffer[1024];
  mbstowcs(wbuffer, buffer, 1024);

  int str_width = wcswidth(wbuffer, wcslen(wbuffer));
  if (str_width < 0) str_width = strlen(buffer); // fallback

  int x_start = (win_width - str_width) / 2;
  if (x_start < 0) x_start = 0;

  mvwprintw(win, line, x_start, "%s", buffer);
}
