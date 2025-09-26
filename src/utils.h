#pragma once

#include <ncurses.h>

#define WINDOW_HEIGHT 21
#define WINDOW_WIDTH 50

#define MIN_TERMINAL_HEIGHT 26
#define MIN_TERMINAL_WIDTH 52

#define line_padding 1

#define len(x) (sizeof(x) / sizeof((x)[0]))

void refresh_win(WINDOW **win, int height, int width);
void mvwprintw_centered(WINDOW *win, int win_width, int line, const char *fmt,
                        ...);
