#pragma once

#include "win_handler.hpp"

#include <ncurses.h>
#include <vector>

struct MenuWinArgs {
  std::string board_win_name;
  const bool &ongoing_game;
  const bool &has_engine;
  bool &playing_engine;
};

class MenuWin : public BaseWindow {
public:
  MenuWin(const MenuWinArgs &args) :
      board_win_name(args.board_win_name), ongoing_game(args.ongoing_game),
      has_engine(args.has_engine), playing_engine(args.playing_engine) {}
  void draw() override;

private:
  UniqueWindow menu_win;

  const std::string board_win_name;
  const bool &ongoing_game;
  const bool &has_engine;
  bool &playing_engine;
  std::vector<std::string> options = {"Player vs Player", "Player vs Engine", "Exit"};
  int options_count = options.size();
  int highlight = 0;

  void navigation_loop();
  void select_option();
  void printw_menu();
  int printw_title();
};
