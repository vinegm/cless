#pragma once

#include <functional>
#include <memory>
#include <ncurses.h>
#include <panel.h>
#include <string>

#define padding 1 // General padding constant, considered as 1 line/column

struct WindowDeleter {
  void operator()(WINDOW *win) const noexcept {
    if (win) delwin(win);
  }
};

struct PanelDeleter {
  void operator()(PANEL *panel) const noexcept {
    if (panel) {
      // Check if the panel is still valid by checking if it's in the panel stack
      // hide_panel returns ERR if panel is invalid, OK otherwise
      if (hide_panel(panel) != ERR) { del_panel(panel); }
    }
  }
};

using UniquePanel = std::unique_ptr<PANEL, PanelDeleter>;
using UniqueWindow = std::unique_ptr<WINDOW, WindowDeleter>;

UniqueWindow create_centered_window(int height, int width);
bool handle_menu_key(int key, int &current_selection, int options_count);
void modifier_wrapper(WINDOW *win, unsigned int modifier, std::function<void()> callaback);
void mvwprintw_centered(WINDOW *win, int win_width, int line, const std::wstring msg);
void mvwprintw_centered(WINDOW *win, int win_width, int line, const std::string msg);
