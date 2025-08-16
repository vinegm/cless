#pragma once

#include <ncurses.h>

#define WINDOW_HEIGHT 20
#define WINDOW_WIDTH 50

#define MIN_TERMINAL_HEIGHT 26
#define MIN_TERMINAL_WIDTH 52

void handle_win(WINDOW **win, int height, int width);
void mvwprintw_centered(WINDOW *win, int width, int line, const char *str);
