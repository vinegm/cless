#include "win_handler.hpp"

void WinHandler::run(std::string start_window) {
  this->current_window = start_window;

  while (true) {
    if (this->next_window != "") {
      this->current_window = this->next_window;
      this->next_window = "";
    }

    if (this->event == this->exit_event) break;
    if (this->event == this->resize_event) this->refresh_win();
    this->event = 0;

    werase(this->get_main_win());
    if (!check_terminal_size())
      this->show_window("size_warning");
    else
      this->show_window(this->current_window);
  }
}

void WinHandler::show_window(std::string name) {
  if (windows.find(name) == windows.end())
    throw std::out_of_range("Invalid window: " + name);

  windows.at(name)->draw();
}

void WinHandler::refresh_win() {
  this->main_win = create_centered_window(WINDOW_HEIGHT, WINDOW_WIDTH);
}

bool WinHandler::check_terminal_size() {
  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  bool is_wide = scr_width >= this->WINDOW_WIDTH;
  bool is_tall = scr_height >= this->WINDOW_HEIGHT;

  return (is_wide && is_tall);
}
