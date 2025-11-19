#pragma once

#include "utils.hpp"

#include <memory>
#include <ncurses.h>
#include <panel.h>
#include <stdexcept>
#include <string>
#include <unordered_map>

struct BaseState {
  const int WINDOW_HEIGHT;
  const int WINDOW_WIDTH;

  std::string next_window = "";
  bool exit_tui = false;

  BaseState(int height, int width) : WINDOW_HEIGHT(height), WINDOW_WIDTH(width) {}
};

/**
 * @brief Base class for all windows managed by WinHandler.
 * draw_panel() should be implemented to set up the window and panel
 * it should draw static content, and update() should handle dynamic content.
 */
template<typename StateType>
class BaseWindow {
public:
  UniqueWindow main_win = nullptr;
  UniquePanel panel = nullptr;

  BaseWindow(StateType &state, bool std_win_size = true) : state(state) {
    if (std_win_size) {
      main_win = UniqueWindow(newwin(state.WINDOW_HEIGHT, state.WINDOW_WIDTH, 0, 0));
      panel = UniquePanel(new_panel(main_win.get()));

      hide_panel(panel.get());
    }
  }

  virtual void draw_panel() {
    throw std::runtime_error("BasePanel::draw_panel() not implemented!");
  }
  virtual void update() { throw std::runtime_error("BasePanel::update() not implemented!"); }
  virtual void handle_input(int pressed_key) {
    throw std::runtime_error("BasePanel::handle_input() not implemented!");
  }
  virtual void recenter_popups() {}

protected:
  StateType &state;
};

/*
 * @brief Manages multiple ncurses windows/panels and handles transitions.
 */
template<typename StateType>
class WinHandler {
public:
  WinHandler(StateType &state) : state(state) {}

  template<typename NewWin>
  void add_window(const std::string &name) {
    windows.emplace(name, std::make_unique<NewWin>(state));
  }

  void run(std::string start_window) {
    state.next_window = start_window;

    center_panels();

    timeout(50);
    keypad(stdscr, true);
    int pressed_key = ERR;
    while (true) {
      if (state.exit_tui) break;

      if (!check_terminal_size())
        change_panel(size_warning_win_name);
      else if (current_window != state.next_window)
        change_panel(state.next_window);

      windows.at(current_window)->update();
      update_panels();
      doupdate();

      pressed_key = getch();
      if (pressed_key == ERR) continue;
      if (pressed_key == KEY_RESIZE) {
        center_panels();
        continue;
      }

      windows.at(current_window)->handle_input(pressed_key);
    }
  }

private:
  StateType &state;
  std::string current_window = "";
  const std::string size_warning_win_name = "size_warning";

  std::unordered_map<std::string, std::unique_ptr<BaseWindow<StateType>>> windows;

  void change_panel(std::string next_win) {
    if (!current_window.empty()) {
      auto it = windows.find(current_window);
      hide_panel(it->second->panel.get());
    }

    current_window = next_win;

    auto it2 = windows.find(current_window);
    if (it2 == windows.end()) {
      throw std::runtime_error("WinHandler::change_panel() - window not found!");
    }
    show_panel(it2->second->panel.get());
  }

  void center_panels() {
    int new_y, new_x;
    int win_height, win_width;
    int scr_height, scr_width;
    getmaxyx(stdscr, scr_height, scr_width);

    for (auto &[name, window] : windows) {
      WINDOW *win_ptr = window->main_win.get();
      getmaxyx(win_ptr, win_height, win_width);

      new_y = (scr_height - win_height) / 2;
      new_x = (scr_width - win_width) / 2;
      move_panel(window->panel.get(), new_y, new_x);

      window->recenter_popups();
    }
  }

  bool check_terminal_size() {
    int scr_height, scr_width;
    getmaxyx(stdscr, scr_height, scr_width);

    bool is_wide = scr_width >= state.WINDOW_WIDTH;
    bool is_tall = scr_height >= state.WINDOW_HEIGHT;

    return (is_wide && is_tall);
  }
};
