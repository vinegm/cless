#include "menu.hpp"

#include "utils.hpp"

#include <ncurses.h>
#include <string>
#include <vector>

#define title_padding (line_padding * 4)
#define menu_padding (title_padding + line_padding)
#define instructions_padding (menu_padding + line_padding)

void MenuWin::draw() {
  WINDOW *parent_win = handler->get_main_win();
  int parent_height, parent_width;
  getmaxyx(parent_win, parent_height, parent_width);

  box(parent_win, 0, 0);

  int title_lines_count = printw_title();

  std::string help_hint = "?: Help";
  modifier_wrapper(parent_win, A_DIM, [&]() {
    mvwprintw_centered(parent_win, parent_width, parent_height - 3, help_hint);
  });

  std::string quit_hint = "q: Quit to main menu";
  modifier_wrapper(parent_win, A_DIM, [&]() {
    mvwprintw_centered(parent_win, parent_width, parent_height - 2, quit_hint);
  });

  WINDOW *menu_win_ptr =
      derwin(parent_win, options_count, parent_width, title_lines_count + menu_padding, 0);
  menu_win = UniqueWindow(menu_win_ptr);
  printw_menu();

  navigation_loop();
}

void MenuWin::navigation_loop() {
  WINDOW *parent_win = handler->get_main_win();
  WINDOW *menu_win_ptr = menu_win.get();

  keypad(menu_win_ptr, true);
  int pressed_key;
  while (true) {
    touchwin(parent_win);
    wrefresh(parent_win);
    printw_menu();

    pressed_key = wgetch(menu_win_ptr);

    if (handle_menu_key(pressed_key, highlight, options_count)) {
      select_option();
      return;
    }

    switch (pressed_key) {
      case '?': show_help_popup(); break;
      case 'q': handler->event = handler->exit_event; return;
      case KEY_RESIZE: handler->event = handler->resize_event; return;
      case ERR: break;
      default: break;
    }
  }
}

/**
 * @brief Show popup when a game is ongoing and handles game continuation or new game
 */
void MenuWin::ongoing_game_popup(GameMode new_game_mode) {
  std::string message = "A game is currently ongoing.";
  std::vector<std::string> popup_options = {"Return to game", "Start new game"};

  int popup_event =
      create_popup(message, popup_options, handler->exit_event, handler->resize_event);

  if (popup_event == handler->exit_event) return;
  if (popup_event == handler->resize_event) {
    handler->event = handler->resize_event;
    return;
  }

  if (popup_event == 0) {
    handler->next_window = board_win_name;
    return;
  }

  if (popup_event == 1) {
    reset_game_callback();
    start_new_game(new_game_mode);
  }
}

void MenuWin::select_option() {
  switch (highlight) {
    case 0: {
      GameMode mode = PLAYER_VS_PLAYER;

      if (ongoing_game) return ongoing_game_popup(mode);
      start_new_game(mode);
      return;
    }

    case 1: {
      if (has_engine) {
        GameMode mode = PLAYER_VS_ENGINE;

        if (ongoing_game) return ongoing_game_popup(mode);
        start_new_game(mode);
      } else {
        int pop_event = create_popup("No engine available", {}, 0, handler->resize_event);
        handler->event = pop_event;
      }
      return;
    }

    case 2: {
      if (ongoing_game) {
        int popup_event = create_popup(
            "Exiting will lose current game progress!",
            {"Cancel", "Exit Anyway"},
            1,
            handler->resize_event
        );

        if (popup_event == 1) handler->event = handler->exit_event;
        if (popup_event == handler->resize_event) handler->event = popup_event;
        return;
      }

      handler->event = handler->exit_event;
      return;
    }
  }
}

void MenuWin::start_new_game(GameMode mode) {
  ongoing_game = true;

  if (mode == PLAYER_VS_PLAYER) { playing_engine = false; }
  if (mode == PLAYER_VS_ENGINE) { playing_engine = true; }

  handler->next_window = board_win_name;
}

/**
 * @brief Prints menu options and handles highlighting
 */
void MenuWin::printw_menu() {
  WINDOW *menu_win_ptr = menu_win.get();

  int win_width, _;
  getmaxyx(menu_win_ptr, _, win_width);

  for (int i = 0; i < options_count; i++) {
    if (i == highlight) { wattron(menu_win_ptr, A_REVERSE); }
    if (i == 1 && !has_engine) { wattron(menu_win_ptr, A_DIM); }
    mvwprintw_centered(menu_win_ptr, win_width, i, options[i]);
    if (i == 1 && !has_engine) { wattroff(menu_win_ptr, A_DIM); }
    if (i == highlight) { wattroff(menu_win_ptr, A_REVERSE); }
  }
}

/**
 * @brief Print the title of the game
 *
 * @return int - The number of lines used by the title
 */
int MenuWin::printw_title() {
  WINDOW *parent_win = handler->get_main_win();
  int _, parent_width;
  getmaxyx(parent_win, _, parent_width);

  std::vector<std::wstring> titleLines = {
      L" ██████╗██╗     ███████╗███████╗███████╗",
      L"██╔════╝██║     ██╔════╝██╔════╝██╔════╝",
      L"██║     ██║     █████╗  ███████╗███████╗",
      L"██║     ██║     ██╔══╝  ╚════██║╚════██║",
      L"╚██████╗███████╗███████╗███████║███████║",
      L" ╚═════╝╚══════╝╚══════╝╚══════╝╚══════╝",
  };

  int title_lines_count = titleLines.size();
  for (int i = 0; i < title_lines_count; i++) {
    mvwprintw_centered(parent_win, parent_width, i + title_padding, titleLines[i]);
  }

  return title_lines_count;
}

/**
 * @brief Show help popup with menu navigation instructions
 */
void MenuWin::show_help_popup() {
  std::vector<std::string> help_messages = {
      "Arrow keys / jk - Move cursor",
      "Space / Enter - Select option",
      "? - Show help",
      "q - Quit the game"
  };

  create_popup(help_messages, {}, handler->exit_event, handler->resize_event);
}
