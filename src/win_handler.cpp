#include "win_handler.hpp"

void WinHandler::run(std::string start_window) {
  current_window = start_window;

  while (true) {
    if (next_window != "") {
      current_window = next_window;
      next_window = "";
    }

    if (event == exit_event) break;
    if (event == resize_event) refresh_win();
    event = 0;

    werase(get_main_win());
    if (!check_terminal_size() && size_warning_win_name != "")
      show_window(size_warning_win_name);
    else
      show_window(current_window);
  }
}

void WinHandler::show_window(std::string name) {
  if (windows.find(name) == windows.end()) throw std::out_of_range("Invalid window: " + name);

  windows.at(name)->draw();
}

void WinHandler::refresh_win() { main_win = create_centered_window(WINDOW_HEIGHT, WINDOW_WIDTH); }

bool WinHandler::check_terminal_size() {
  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  bool is_wide = scr_width >= WINDOW_WIDTH;
  bool is_tall = scr_height >= WINDOW_HEIGHT;

  return (is_wide && is_tall);
}
