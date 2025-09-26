#include "engine.h"
#include <ncurses.h>
#include <stdint.h>

#define square_to_bit(square) (1ULL << (square))
#define pass_turn(board)                                                       \
  board->to_move = (board->to_move == WHITE) ? BLACK : WHITE;

char get_piece_char_at(BoardState *board, Square square);

static char get_piece_char(BoardState *board, PieceColor color,
                           PieceType piece);
static bool captures(BoardState *board, Square at, int *captured_color,
                     int *captured_piece);
static void get_piece_at(BoardState *board, uint64_t square_bit, int *color,
                         int *type);
static bool validate_move(BoardState *board, PieceColor piece_color,
                          PieceType piece_type, uint64_t from, uint64_t to);
static void add_piece(BoardState *board, PieceColor color, PieceType piece,
                      uint64_t square_bit);
static void remove_piece(BoardState *board, PieceColor color, PieceType piece,
                         uint64_t square_bit);

void init_chess_board(BoardState *board) {
  board->bitboards[WHITE][PIECE_PAWN] = RANK_2;
  board->bitboards[WHITE][PIECE_KNIGHT] = square_to_bit(B1) | square_to_bit(G1);
  board->bitboards[WHITE][PIECE_BISHOP] = square_to_bit(C1) | square_to_bit(F1);
  board->bitboards[WHITE][PIECE_ROOK] = square_to_bit(A1) | square_to_bit(H1);
  board->bitboards[WHITE][PIECE_QUEEN] = square_to_bit(D1);
  board->bitboards[WHITE][PIECE_KING] = square_to_bit(E1);

  board->bitboards[BLACK][PIECE_PAWN] = RANK_7;
  board->bitboards[BLACK][PIECE_KNIGHT] = square_to_bit(B8) | square_to_bit(G8);
  board->bitboards[BLACK][PIECE_BISHOP] = square_to_bit(C8) | square_to_bit(F8);
  board->bitboards[BLACK][PIECE_ROOK] = square_to_bit(A8) | square_to_bit(H8);
  board->bitboards[BLACK][PIECE_QUEEN] = square_to_bit(D8);
  board->bitboards[BLACK][PIECE_KING] = square_to_bit(E8);

  board->bitboards[WHITE][PIECE_NONE] = ~(RANK_1 | RANK_2);
  board->bitboards[BLACK][PIECE_NONE] = ~(RANK_7 | RANK_8);

  board->to_move = WHITE;
}

char get_piece_char_at(BoardState *board, Square square) {
  int color, piece = -1;
  get_piece_at(board, square_to_bit(square), &color, &piece);
  return get_piece_char(board, color, piece);
}

static char get_piece_char(BoardState *board, PieceColor color,
                           PieceType piece) {
  if (piece == PIECE_NONE) return ' ';

  const char piece_char[2][6] = {{'P', 'N', 'B', 'R', 'Q', 'K'},
                                 {'p', 'n', 'b', 'r', 'q', 'k'}};

  return piece_char[color][piece - 1];
}

bool move_piece(BoardState *board, Square from, Square to) {
  uint64_t from_bit = square_to_bit(from);
  uint64_t to_bit = square_to_bit(to);

  int piece_color, piece_type;
  get_piece_at(board, from_bit, &piece_color, &piece_type);

  if (!validate_move(board, piece_color, piece_type, from_bit, to_bit))
    return false;

  remove_piece(board, piece_color, piece_type, from_bit);
  add_piece(board, piece_color, piece_type, to_bit);

  pass_turn(board);
  return true;
}

static void get_piece_at(BoardState *board, uint64_t square_bit, int *color,
                         int *type) {
  if ((~board->bitboards[WHITE][PIECE_NONE]) & square_bit) {
    *color = WHITE;
  } else if ((~board->bitboards[BLACK][PIECE_NONE]) & square_bit) {
    *color = BLACK;
  } else {
    *type = PIECE_NONE;
    return;
  }

  for (int p = 0; p <= 12; p++) {
    if (board->bitboards[*color][p] & square_bit) {
      *type = p;
      return;
    }
  }

  *type = PIECE_NONE;
}

static bool validate_move(BoardState *board, PieceColor piece_color,
                          PieceType piece_type, uint64_t from, uint64_t to) {
  // No piece at the source square
  if (piece_type == PIECE_NONE) return false;

  // Not the player's turn
  if (piece_color != board->to_move) return false;

  return true;
}

static void add_piece(BoardState *board, PieceColor color, PieceType piece,
                      uint64_t square_bit) {
  board->bitboards[color][piece] |= square_bit;
  board->bitboards[color][PIECE_NONE] &= ~square_bit;
}

static void remove_piece(BoardState *board, PieceColor color, PieceType piece,
                         uint64_t square_bit) {
  board->bitboards[color][piece] &= ~square_bit;
  board->bitboards[color][PIECE_NONE] |= square_bit;
}
