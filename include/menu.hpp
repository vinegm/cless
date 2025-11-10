#pragma once

#include "win_handler.hpp"

#include <functional>
#include <ncurses.h>
#include <vector>

enum GameMode {
  PLAYER_VS_PLAYER,
  PLAYER_VS_ENGINE
};

struct MenuWinArgs {
  std::string board_win_name;
  const bool &has_engine;
  bool &ongoing_game;
  bool &playing_engine;
  std::function<void()> reset_game_callback;
};

class MenuWin : public BaseWindow {
public:
  MenuWin(const MenuWinArgs &args) :
      board_win_name(args.board_win_name), ongoing_game(args.ongoing_game),
      has_engine(args.has_engine), playing_engine(args.playing_engine),
      reset_game_callback(args.reset_game_callback) {}
  void draw() override;

private:
  UniqueWindow menu_win;

  const std::string board_win_name;
  const bool &has_engine;
  bool &ongoing_game;
  bool &playing_engine;
  std::function<void()> reset_game_callback;
  std::vector<std::string> options = {"Player vs Player", "Player vs Engine", "Exit"};
  int options_count = options.size();
  int highlight = 0;

  void navigation_loop();
  void ongoing_game_popup(GameMode new_game_mode);
  void select_option();
  void start_new_game(GameMode new_game_mode);
  void printw_menu();
  int printw_title();
  void show_help_popup();
};
