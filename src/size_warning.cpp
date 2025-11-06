#include "size_warning.hpp"

#include <ncurses.h>

void SizeWarningWin::draw() {
  const int warning_height = 10;
  const int warning_width = 60;

  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  UniqueWindow warning_win = create_centered_window(warning_height, warning_width);
  WINDOW *raw_warning_win = warning_win.get();
  box(raw_warning_win, 0, 0);

  std::string expected_width = std::to_string(handler->WINDOW_WIDTH);
  std::string expected_height = std::to_string(handler->WINDOW_HEIGHT);

  std::string title = "TERMINAL SIZE WARNING";
  std::string curr_size =
      "Current size: " + std::to_string(scr_width) + "x" + std::to_string(scr_height);
  std::string req_size = "Required size: " + expected_width + "x" + expected_height + " (minimum)";
  std::string info_msg = "Your terminal is too small to display CLESS properly.";
  std::string resize_msg = "Please resize your terminal or use a smaller font.";
  std::string exit_msg = "Press any key to continue or 'q' to quit...";

  auto print_centered_warning = [raw_warning_win, warning_width](int line, const std::string &msg) {
    mvwprintw_centered(raw_warning_win, warning_width, line, msg);
  };

  wattron(raw_warning_win, A_BOLD | A_REVERSE);
  print_centered_warning(1, title);
  wattroff(raw_warning_win, A_BOLD | A_REVERSE);

  print_centered_warning(2, info_msg);
  print_centered_warning(4, curr_size);
  print_centered_warning(5, req_size);
  print_centered_warning(7, resize_msg);

  wattron(raw_warning_win, A_BOLD);
  print_centered_warning(8, exit_msg);
  wattroff(raw_warning_win, A_BOLD);

  int pressed_key = wgetch(raw_warning_win);
  if (std::tolower(pressed_key) == 'q') handler->event = handler->exit_event;
}
