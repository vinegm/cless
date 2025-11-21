#include "menu.hpp"

#include "game_logic.hpp"
#include "utils.hpp"

#include <cstdlib>
#include <ncurses.h>
#include <panel.h>
#include <string>
#include <vector>

#define title_padding (padding * 4)
#define menu_padding (title_padding + padding)
#define instructions_padding (menu_padding + padding)

void MenuWin::draw_panel() {
  popup_handler.add_popup("help", help_popup);
  popup_handler.add_popup("no_engine", no_engine_popup);
  popup_handler.add_popup("new_game", new_game_popup, [this](int pressed_key, int popup_result) {
    if (popup_result == 0) state.next_window = state.board_win_name;

    if (popup_result == 1) {
      if (selected_mode.value() == PLAYER_VS_ENGINE) {
        popup_handler.show_popup("select_color");
        return true;
      }

      state.game.new_game(selected_mode.value());
      state.next_window = state.board_win_name;
    }

    return true;
  });
  popup_handler
      .add_popup("confirm_exit", confirm_exit_popup, [this](int pressed_key, int popup_result) {
        if (popup_result == 1) state.exit_tui = true;
        return true;
      });
  popup_handler
      .add_popup("select_color", select_color_popup, [this](int pressed_key, int popup_result) {
        PieceColor selected_color;
        switch (popup_result) {
          case 0: selected_color = WHITE; break;
          case 1: selected_color = BLACK; break;
          case 2: selected_color = (rand() % 2 == 0) ? WHITE : BLACK; break;
          default: return false;
        }

        state.game.new_game(selected_mode.value(), selected_color);
        state.next_window = state.board_win_name;
        return true;
      });

  WINDOW *main_win_ptr = main_win.get();
  int main_win_height, main_win_width;
  getmaxyx(main_win_ptr, main_win_height, main_win_width);

  box(main_win_ptr, 0, 0);

  int title_lines_count = printw_title();

  std::string help_hint = "?: Help";
  modifier_wrapper(main_win_ptr, A_DIM, [&]() {
    mvwprintw_centered(main_win_ptr, main_win_width, main_win_height - 3, help_hint);
  });

  std::string quit_hint = "q: Quit to main menu";
  modifier_wrapper(main_win_ptr, A_DIM, [&]() {
    mvwprintw_centered(main_win_ptr, main_win_width, main_win_height - 2, quit_hint);
  });

  WINDOW *menu_win_ptr =
      derwin(main_win_ptr, options.size(), main_win_width, title_lines_count + menu_padding, 0);
  menu_win = UniqueWindow(menu_win_ptr);

  update(); // Initial draw of interractive elements
}

void MenuWin::update() {
  if (popup_handler.any_visible()) {
    popup_handler.update();
    return;
  }

  WINDOW *main_win_ptr = main_win.get();

  printw_menu();

  touchwin(main_win_ptr);
  wrefresh(main_win_ptr);
}

void MenuWin::handle_input(int pressed_key) {
  if (popup_handler.any_visible()) {
    popup_handler.handle_input(pressed_key);
    return;
  }

  if (handle_menu_key(pressed_key, highlight, options.size())) {
    select_option();
    return;
  }

  switch (pressed_key) {
    case '?': popup_handler.show_popup("help"); break;

    case 'q': {
      if (state.game.is_game_ongoing()) return popup_handler.show_popup("confirm_exit");

      state.exit_tui = true;
      return;
    }

    default: break;
  }
}

void MenuWin::select_option() {
  switch (highlight) {
    case 0: {
      selected_mode = PLAYER_VS_PLAYER;
      start_new_game();
      return;
    }

    case 1: {
      if (state.game.has_engine_available()) {
        selected_mode = PLAYER_VS_ENGINE;
        start_new_game();
      } else {
        popup_handler.show_popup("no_engine");
      }
      return;
    }

    case 2: {
      if (state.game.is_game_ongoing()) return popup_handler.show_popup("confirm_exit");

      state.exit_tui = true;
      return;
    }
  }
}

void MenuWin::start_new_game() {
  if (!selected_mode.has_value()) return;

  if (state.game.is_game_ongoing()) {
    popup_handler.show_popup("new_game");
    return;
  }

  if (selected_mode.value() == PLAYER_VS_ENGINE) {
    popup_handler.show_popup("select_color");
    return;
  }

  state.game.new_game(selected_mode.value());
  state.next_window = state.board_win_name;
}

/**
 * @brief Prints menu options and handles highlighting
 */
void MenuWin::printw_menu() {
  WINDOW *menu_win_ptr = menu_win.get();

  int win_width, _;
  getmaxyx(menu_win_ptr, _, win_width);

  for (int i = 0; i < options.size(); i++) {
    if (i == highlight) { wattron(menu_win_ptr, A_REVERSE); }
    if (i == 1 && !state.game.has_engine_available()) { wattron(menu_win_ptr, A_DIM); }
    mvwprintw_centered(menu_win_ptr, win_width, i, options[i]);
    if (i == 1 && !state.game.has_engine_available()) { wattroff(menu_win_ptr, A_DIM); }
    if (i == highlight) { wattroff(menu_win_ptr, A_REVERSE); }
  }
}

/**
 * @brief Print the title of the game
 *
 * @return int - The number of lines used by the title
 */
int MenuWin::printw_title() {
  WINDOW *main_win_ptr = main_win.get();
  int _, main_win_width;
  getmaxyx(main_win_ptr, _, main_win_width);

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
    mvwprintw_centered(main_win_ptr, main_win_width, i + title_padding, titleLines[i]);
  }

  return title_lines_count;
}
