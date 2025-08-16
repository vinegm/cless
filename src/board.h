#pragma once

#include "engine.h"
#include <ncurses.h>

// Board display structure
typedef struct {
  WINDOW **boardWin;
  int selectedSquare;    // -1 if no selection, 0-63 for square index
  int highlightedSquare; // -1 if no highlight, 0-63 for square index
} BoardDisplay;

// Function declarations for UI and interaction
void render_board(WINDOW *parentWin, ChessBoardState *board,
                  BoardDisplay *display);
void create_centered_board_window(WINDOW **boardWin);
void handle_board_input(BoardDisplay *display, ChessBoardState *board,
                        int *game_result);
