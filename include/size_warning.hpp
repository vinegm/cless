#pragma once

#include "tui_state.hpp"
#include "win_handler.hpp"

#include <panel.h>

class SizeWarningWin : public BaseWindow<TuiState> {
public:
  SizeWarningWin(TuiState &state) : BaseWindow<TuiState>(state, false) {
    const int warning_height = 10;
    const int warning_width = 60;

    main_win = UniqueWindow(newwin(warning_height, warning_width, 0, 0));
    panel = UniquePanel(new_panel(main_win.get()));

    hide_panel(panel.get());
    draw_panel();
  }

  void draw_panel() override;
  void update() override;
  void handle_input(int pressed_key) override;
};
