#pragma once

#include "win_handler.h"
#include <ncurses.h>

typedef struct {
  char **options;
  WinHandler *tui;
  int board_win_id;
  int options_count;
  int highlight;
} MenuWinData;

void init_menu_data(MenuWinData *menu_data, WinHandler *tui, int board_win_id);
void render_menu(WINDOW *parentWin, void *menu_data);
