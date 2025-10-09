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
  int from;
  int to;
  PieceType piece;
  PieceType captured;
  int is_castle;
  int is_en_passant;
  PieceType promotion;
} Move;

typedef struct {
  Move moves[256];
  int count;
} MoveList;

typedef struct {
  uint64_t pieces[2][7];
  uint64_t occupied;
  uint64_t white_pieces;
  uint64_t black_pieces;
  int to_move;
  char square_piece[64];
  int castle_rights; // Bits: 0=white kingside, 1=white queenside, 2=black kingside, 3=black queenside
  int en_passant_square; // -1 if no en passant
  int halfmove_clock;
  int fullmove_number;
} ChessBoardState;

// clang-format off
// Little-endian rank-file
typedef enum {
  A8 = 56, B8, C8, D8, E8, F8, G8, H8,
  A7 = 48, B7, C7, D7, E7, F7, G7, H7,
  A6 = 40, B6, C6, D6, E6, F6, G6, H6,
  A5 = 32, B5, C5, D5, E5, F5, G5, H5,
  A4 = 24, B4, C4, D4, E4, F4, G4, H4,
  A3 = 16, B3, C3, D3, E3, F3, G3, H3,
  A2 = 8,  B2, C2, D2, E2, F2, G2, H2,
  A1 = 0,  B1, C1, D1, E1, F1, G1, H1,
} Square;
// clang-format on

void init_chess_board(ChessBoardState *board);
void update_board_state(ChessBoardState *board);
void generate_moves(ChessBoardState *board, MoveList *move_list);
int is_square_attacked(ChessBoardState *board, int square, int by_color);
int make_move(ChessBoardState *board, Move move);
void unmake_move(ChessBoardState *board, Move move);

// Utility functions for bitboards
uint64_t get_rook_attacks(int square, uint64_t occupied);
uint64_t get_bishop_attacks(int square, uint64_t occupied);
uint64_t get_queen_attacks(int square, uint64_t occupied);
uint64_t get_knight_attacks(int square);
uint64_t get_king_attacks(int square);
uint64_t get_pawn_attacks(int square, int color);
uint64_t get_pawn_moves(int square, int color, uint64_t occupied);

// Square utilities
int square_to_rank(int square);
int square_to_file(int square);
int rank_file_to_square(int rank, int file);
