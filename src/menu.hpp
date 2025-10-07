#pragma once

#include "win_handler.hpp"
#include <ncurses.h>

class MenuWin : public BaseWindow {
public:
  void draw() override;
  void set_board_win_id(int id) { this->board_win_id = id; }

private:
  int board_win_id = -1;
  int highlight = 0;
  std::vector<std::string> options = {"Player vs Robot", "Player vs Player",
                                      "Exit"};
  int options_count = options.size();

  void navigation_loop(WINDOW *menu_win);
  void select_option();
  void printw_menu(WINDOW *win);
  int printw_title(WINDOW *win, int win_width);
};
