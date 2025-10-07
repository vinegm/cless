#include "win_handler.hpp"

void WinHandler::run() {
  this->refresh_win();

  while (true) {
    this->show_window(this->current_window);

    if (this->event == this->exit_event) break;

    if (this->event == this->resize_event) this->refresh_win();
  }
}

void WinHandler::add_window(std::unique_ptr<BaseWindow> window) {
  if (!window) throw std::invalid_argument("Window pointer is null");

  window->setInternals(this->windows.size(), main_win, this);
  windows.push_back(std::move(window));
}

void WinHandler::show_window(int win_id) {
  if (win_id < 0 || win_id >= windows.size())
    throw std::out_of_range("Invalid window ID");

  int current_win_id = this->current_window;
  BaseWindow *window = this->windows[win_id].get();

  window->draw();
}

template <typename T, typename... Args>
T *WinHandler::add_window(Args &&...args) {
  static_assert(std::is_base_of_v<BaseWindow, T>,
                "T must derive from BaseWindow");

  auto win = std::make_unique<T>(std::forward<Args>(args)...);
  win->setInternals(windows.size(), main_win, this);

  T *raw_ptr = win.get(); // return pointer if caller wants direct access
  windows.push_back(std::move(win));
  return raw_ptr;
}

void WinHandler::refresh_win() {
  while (!check_terminal_size()) {
    *(this->main_win) = create_centered_window(WINDOW_HEIGHT, WINDOW_WIDTH);
    show_window(1);
  }

  *(this->main_win) = create_centered_window(WINDOW_HEIGHT, WINDOW_WIDTH);
}

int WinHandler::check_terminal_size() {
  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  int is_wide = scr_width >= this->WINDOW_WIDTH;
  int is_tall = scr_height >= this->WINDOW_HEIGHT;

  return (is_wide && is_tall);
}
