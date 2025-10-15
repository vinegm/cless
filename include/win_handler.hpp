#pragma once

#include "utils.hpp"
#include <ncurses.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

#define EXIT_EVENT -1
#define RESIZE_EVENT -2

class BaseWindow;
class WinHandler;

class BaseWindow {
public:
  void set_handler(WinHandler *handler) { this->handler = handler; }

  virtual void draw() {
    throw std::runtime_error("BaseWindow::draw() not implemented!");
  }

protected:
  WinHandler *handler;
};

/*
 * @brief Manages multiple ncurses windows and handles window transitions.
 */
class WinHandler {
public:
  std::string next_window = "";
  std::string size_warning_win_name = "";
  const int WINDOW_HEIGHT;
  const int WINDOW_WIDTH;

  int event = 0;
  const int exit_event = EXIT_EVENT;
  const int resize_event = RESIZE_EVENT;

  WinHandler(int height, int width, const std::string &name)
      : WinHandler(height, width) {
    size_warning_win_name = name;
  }
  WinHandler(int height, int width)
      : WINDOW_HEIGHT(height), WINDOW_WIDTH(width) {
    refresh_win();
  }

  template <typename NewWin> void add_window(const std::string &name) {
    auto window = std::make_unique<NewWin>();
    window->set_handler(this);
    windows.emplace(name, std::move(window));
  }

  template <typename NewWin, typename Arg>
  void add_window(const std::string &name, const Arg &arg) {
    auto window = std::make_unique<NewWin>(arg);
    window->set_handler(this);
    windows.emplace(name, std::move(window));
  }

  void run(std::string start_window);
  WINDOW *get_main_win() { return main_win.get(); }

private:
  std::string current_window = "";
  UniqueWindow main_win = UniqueWindow(nullptr);
  std::unordered_map<std::string, std::unique_ptr<BaseWindow>> windows;

  void show_window(std::string name);
  void refresh_win();
  bool check_terminal_size();
};
