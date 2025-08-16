#pragma once

#include <ncurses.h>

typedef struct {
  char **options;
  int optionsCount;
  int selectedOption;
  int highlight;
  int exitOptionIndex;
} MenuOptions;

void render_menu(WINDOW *parentWin, MenuOptions *menuOptions);
