#pragma once

#include <stdint.h>

typedef enum {
  PIECE_NONE = 0,
  PIECE_PAWN,
  PIECE_ROOK,
  PIECE_KNIGHT,
  PIECE_BISHOP,
  PIECE_QUEEN,
  PIECE_KING
} PieceType;

typedef enum { WHITE = 0, BLACK = 1 } PieceColor;

typedef struct {
  uint64_t pieces[2][7];
  uint64_t occupied;
  uint64_t white_pieces;
  uint64_t black_pieces;
  int to_move;
} ChessBoardState;

// clang-format off
typedef enum {
  A8 = 56, B8, C8, D8, E8, F8, G8, H8,
  A7 = 48, B7, C7, D7, E7, F7, G7, H7,
  A6 = 32, B6, C6, D6, E6, F6, G6, H6,
  A5 = 40, B5, C5, D5, E5, F5, G5, H5,
  A4 = 24, B4, C4, D4, E4, F4, G4, H4,
  A3 = 16, B3, C3, D3, E3, F3, G3, H3,
  A2 = 8, B2, C2, D2, E2, F2, G2, H2,
  A1 = 0, B1, C1, D1, E1, F1, G1, H1,
} Square;
// clang-format on

void init_chess_board(ChessBoardState *board);
