#pragma once

#include "engine.h"
#include <ncurses.h>

#define white_orientation 0
#define black_orientation 1

#define white_square_color 1
#define black_square_color 2
#define highlighted_square_color 3
#define selected_square_color 4

typedef struct {
  WINDOW **boardWin;
  int status;
  int selected_square;
  int highlighted_square;
  int board_orientation;
  int exit_event;
  int resize_event;
} GameWinState;

void render_board(WINDOW *parentWin, ChessBoardState *board,
                  GameWinState *game);
