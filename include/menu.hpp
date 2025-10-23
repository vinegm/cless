#pragma once

#include "win_handler.hpp"

#include <ncurses.h>
#include <vector>

struct MenuWinArgs {
  std::string board_win_name;
};

class MenuWin : public BaseWindow {
public:
  MenuWin(MenuWinArgs args) : board_win_name(args.board_win_name) {}
  void draw() override;

private:
  UniqueWindow menu_win;

  const std::string board_win_name;
  std::vector<std::string> options = {"Player vs Player", "Player vs Robot", "Exit"};
  int options_count = options.size();
  int highlight = 0;

  void navigation_loop();
  void select_option();
  void printw_menu();
  int printw_title();
};
