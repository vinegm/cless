#pragma once

#include <stdbool.h>
#include <stdint.h>

#define encode_piece(color, type) ((color + 1) | (1 << (type + 1)))
#define decode_piece(piece, color, type)                                       \
  do {                                                                         \
    *color = ((piece) & 1) ? WHITE : BLACK;                                    \
    *type = (piece) ? (31 - __builtin_clz(piece)) - 1 : PIECE_NONE;            \
  } while (0)

#define RANK_1 0xFFULL
#define RANK_2 (RANK_1 << (8 * 1))
#define RANK_3 (RANK_1 << (8 * 2))
#define RANK_4 (RANK_1 << (8 * 3))
#define RANK_5 (RANK_1 << (8 * 4))
#define RANK_6 (RANK_1 << (8 * 5))
#define RANK_7 (RANK_1 << (8 * 6))
#define RANK_8 (RANK_1 << (8 * 7))

#define FILE_A 0x0101010101010101ULL
#define FILE_B 0x0202020202020202ULL
#define FILE_C 0x0404040404040404ULL
#define FILE_D 0x0808080808080808ULL
#define FILE_E 0x1010101010101010ULL
#define FILE_F 0x2020202020202020ULL
#define FILE_G 0x4040404040404040ULL
#define FILE_H 0x8080808080808080ULL

// enums for indexing pieces
typedef enum {
  PIECE_NONE,
  PIECE_PAWN,
  PIECE_KNIGHT,
  PIECE_BISHOP,
  PIECE_ROOK,
  PIECE_QUEEN,
  PIECE_KING
} PieceType;

// enums for indexing colors
typedef enum { WHITE, BLACK } PieceColor;

typedef struct {
  uint64_t bitboards[2][7]; // [PieceColor][PieceType]
  uint8_t lookup_table[64]; // Encoded pieces

  // uint8_t castling_rights; // Will implement later
  uint8_t *en_passant_square;
  int halfmove_clock;
  int fullmove_counter;
  PieceColor to_move;
} BoardState;

// clang-format off
// Little-endian rank-file
typedef enum {
  A8 = 7 * 8, B8, C8, D8, E8, F8, G8, H8,
  A7 = 6 * 8, B7, C7, D7, E7, F7, G7, H7,
  A6 = 5 * 8, B6, C6, D6, E6, F6, G6, H6,
  A5 = 4 * 8, B5, C5, D5, E5, F5, G5, H5,
  A4 = 3 * 8, B4, C4, D4, E4, F4, G4, H4,
  A3 = 2 * 8, B3, C3, D3, E3, F3, G3, H3,
  A2 = 1 * 8, B2, C2, D2, E2, F2, G2, H2,
  A1 = 0 * 8, B1, C1, D1, E1, F1, G1, H1,
} Square;
// clang-format on

void init_chess_board(BoardState *board);
char get_piece_char_at(BoardState *board, Square square);
bool move_piece(BoardState *board, Square from, Square to);
