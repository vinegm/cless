#pragma once

#include "tui_handler.h"
#include <ncurses.h>

typedef struct {
  char **options;
  TuiHandler *tui;
  int board_win_id;
  int options_count;
  int highlight;
} MenuData;

void init_menu_data(MenuData *menu_data, TuiHandler *tui, int board_win_id);
void render_menu(WINDOW *parentWin, void *menu_data);
