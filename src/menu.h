#pragma once

#include <ncurses.h>

typedef struct {
  char **options;
  int options_count;
  int selected_option;
  int highlight;
  int exit_event;
  int resize_event;
} MenuOptions;

void render_menu(WINDOW *parentWin, MenuOptions *menuOptions);
