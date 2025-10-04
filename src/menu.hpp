#pragma once

#include "win_handler.hpp"
#include <ncurses.h>
#include <vector>

class MenuWin : public BaseWindow {
public:
  MenuWin(std::string board_win_name) : board_win_name(board_win_name) {}
  void draw() override;

private:
  UniqueWindow menu_win;

  const std::string board_win_name;
  std::vector<std::string> options = {"Player vs Robot", "Player vs Player",
                                      "Exit"};
  int options_count = options.size();
  int highlight = 0;

  void navigation_loop();
  void select_option();
  void printw_menu();
  int printw_title();
};
