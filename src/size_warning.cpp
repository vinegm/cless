#include "size_warning.hpp"

#include "utils.hpp"

#include <ncurses.h>

void SizeWarningWin::draw_panel() {
  WINDOW *main_win_ptr = main_win.get();
  int warning_height, warning_width;
  getmaxyx(main_win_ptr, warning_height, warning_width);

  std::string expected_width = std::to_string(state.WINDOW_WIDTH);
  std::string expected_height = std::to_string(state.WINDOW_HEIGHT);

  box(main_win_ptr, 0, 0);

  std::string title = "TERMINAL SIZE WARNING";
  std::string req_size = "Required size: " + expected_width + "x" + expected_height + " (minimum)";
  std::string info_msg = "Your terminal is too small to display CLESS properly.";
  std::string resize_msg = "Please resize your terminal or use a smaller font.";
  std::string exit_msg = "Press any key to continue or 'q' to quit...";

  auto print_centered_warning = [main_win_ptr, warning_width](int line, const std::string &msg) {
    mvwprintw_centered(main_win_ptr, warning_width, line, msg);
  };

  wattron(main_win_ptr, A_BOLD | A_REVERSE);
  print_centered_warning(1, title);
  wattroff(main_win_ptr, A_BOLD | A_REVERSE);

  print_centered_warning(2, info_msg);
  print_centered_warning(5, req_size);
  print_centered_warning(7, resize_msg);

  wattron(main_win_ptr, A_BOLD);
  print_centered_warning(8, exit_msg);
  wattroff(main_win_ptr, A_BOLD);
}

void SizeWarningWin::update() {
  WINDOW *main_win_ptr = main_win.get();
  int _, main_width;
  getmaxyx(main_win_ptr, _, main_width);
  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  std::string curr_size =
      "Current size: " + std::to_string(scr_width) + "x" + std::to_string(scr_height);

  mvwprintw_centered(main_win_ptr, main_width, 4, curr_size);
}

void SizeWarningWin::handle_input(int key) {
  if (key == 'q') state.exit_tui = true;
}
