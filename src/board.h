#pragma once

#include "engine.h"
#include "win_handler.h"
#include <ncurses.h>

#define white_orientation 0
#define black_orientation 1

#define white_square_color 1
#define black_square_color 2
#define highlighted_square_color 3
#define selected_square_color 4

typedef struct {
  BoardState *board;
  WinHandler *tui;
  int menu_win_id;
  int status;
  int selected_square;
  int highlighted_square;
  int board_orientation;
} BoardWinData;

void init_board_data(BoardWinData *board_data, WinHandler *tui, int menu_win_id,
                     BoardState *gameState);
void render_board(WINDOW *parentWin, void *board_data);
