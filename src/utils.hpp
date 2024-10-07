#pragma once

#include <memory>
#include <ncurses.h>
#include <string>

#define line_padding 1

struct WindowDeleter {
  void operator()(WINDOW *win) const noexcept {
    if (win) delwin(win);
  }
};

using UniqueWindow = std::unique_ptr<WINDOW, WindowDeleter>;
using SharedWindow = std::shared_ptr<WINDOW>;

UniqueWindow create_centered_window(int height, int width);
void mvwprintw_centered(WINDOW *win, int win_width, int line,
                        const std::wstring msg);
void mvwprintw_centered(WINDOW *win, int win_width, int line, std::string msg);
