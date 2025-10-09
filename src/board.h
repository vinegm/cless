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
#define legal_move_color 5
#define capture_move_color 6

typedef struct {
  ChessBoardState *board;
  WinHandler *tui;
  int menu_win_id;
  int status;
  int selected_square;
  int highlighted_square;
  int board_orientation;
  MoveList legal_moves;
  MoveList selected_piece_moves;
  int show_legal_moves;
} BoardData;

void init_board_data(BoardData *board_data, WinHandler *tui, int menu_win_id,
                     ChessBoardState *gameState);
void render_board(WINDOW *parentWin, void *board_data);
