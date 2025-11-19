#pragma once

#include "game_logic.hpp"
#include "popup.hpp"
#include "tui_state.hpp"
#include "win_handler.hpp"

#include <ncurses.h>
#include <optional>
#include <vector>

class MenuWin : public BaseWindow<TuiState> {
public:
  MenuWin(TuiState &state) : BaseWindow<TuiState>(state) { draw_panel(); }

  void draw_panel() override;
  void update() override;
  void handle_input(int pressed_key) override;
  void recenter_popups() override { popup_handler.recenter_popups(); };

private:
  std::string board_win_name;
  std::vector<std::string> options = {"Player vs Player", "Player vs Engine", "Exit"};
  std::optional<GameMode> selected_mode = std::nullopt;
  int highlight = 0;

  UniqueWindow menu_win;

  PopupHandler popup_handler;
  Popup help_popup{
      {"Arrow keys / jk - Move cursor",
       "Space / Enter - Select option",
       "? - Show help",
       "q - Quit the game"}
  };
  Popup no_engine_popup{{"Engine is not available."}};
  Popup new_game_popup{
      {"A game is currently in progress. Start a new game?"},
      {"Return to game", "Start new game"}
  };
  Popup confirm_exit_popup{{"Exiting will lose current game progress!"}, {"Cancel", "Exit anyway"}};

  void select_option();
  void start_new_game();
  void printw_menu();
  int printw_title();
  void show_help_popup();
};
