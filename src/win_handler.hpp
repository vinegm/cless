#pragma once

#include <memory>
#include <ncurses.h>
#include <stdexcept>
#include <vector>

#include "utils.hpp"

#define EXIT_EVENT -1
#define RESIZE_EVENT -2

class BaseWindow;
class WinHandler;

class BaseWindow {
public:
  BaseWindow() : id(-1) {}

  void setInternals(int new_id, std::shared_ptr<UniqueWindow> parent,
                    WinHandler *handler) {
    this->id = new_id;
    this->parent_win = parent;
    this->handler = handler;
  }
  virtual void draw() {
    throw std::runtime_error("BaseWindow::draw() not implemented!");
  }

protected:
  int id;
  std::shared_ptr<UniqueWindow> parent_win;
  WinHandler *handler;
};

class WinHandler {
public:
  const int WINDOW_HEIGHT;
  const int WINDOW_WIDTH;

  std::vector<std::unique_ptr<BaseWindow>> windows;
  int current_window = 0;

  int event = 0;
  int exit_event = EXIT_EVENT;
  int resize_event = RESIZE_EVENT;

  WinHandler(int height, int width)
      : WINDOW_HEIGHT(height), WINDOW_WIDTH(width) {}
  void run();
  void add_window(std::unique_ptr<BaseWindow> window);
  template <typename T, typename... Args> T *add_window(Args &&...args);

private:
  std::shared_ptr<UniqueWindow> main_win =
      std::make_shared<UniqueWindow>(nullptr);

  void show_window(int win_id);
  void refresh_win();
  int check_terminal_size();
};
