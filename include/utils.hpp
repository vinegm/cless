#pragma once

#include <functional>
#include <memory>
#include <ncurses.h>
#include <string>
#include <vector>

#define line_padding 1

struct WindowDeleter {
  void operator()(WINDOW *win) const noexcept {
    if (win) delwin(win);
  }
};

using UniqueWindow = std::unique_ptr<WINDOW, WindowDeleter>;
using SharedWindow = std::shared_ptr<WINDOW>;

UniqueWindow create_centered_window(int height, int width);
UniqueWindow create_centered_overlay(int height, int width);
int create_popup(
    const std::vector<std::string> &messages,
    const std::vector<std::string> &options,
    int exit_status,
    int resize_status
);
int create_popup(
    const std::string &message,
    const std::vector<std::string> &options,
    int exit_status,
    int resize_status
);
bool handle_menu_key(int key, int &current_selection, int options_count);
void modifier_wrapper(WINDOW *win, unsigned int modifier, std::function<void()> callaback);
void mvwprintw_centered(WINDOW *win, int win_width, int line, const std::wstring msg);
void mvwprintw_centered(WINDOW *win, int win_width, int line, const std::string msg);
