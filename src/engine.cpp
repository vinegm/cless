#include "engine.hpp"
#include <cstdint>
#include <cstring>
#include <ncurses.h>

#define square_to_bit(square) (1ULL << (square))

char get_piece_char_at(BoardState *board, Square square);

static char get_piece_char(BoardState *board, PieceColor color,
                           PieceType piece);
static bool captures(BoardState *board, Square at, int *captured_color,
                     int *captured_piece);
static void get_piece_at(BoardState *board, int square, int *color, int *type);
static bool validate_move(BoardState *board, PieceColor piece_color,
                          PieceType piece_type);
static void pass_turn(BoardState *board);
static void add_piece(BoardState *board, PieceColor color, PieceType piece,
                      Square square);
static void remove_piece(BoardState *board, PieceColor color, PieceType piece,
                         Square square);

void init_chess_board(BoardState *board) {
  memset(board, 0, sizeof(BoardState));

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

  board->lookup_table[A1] = encode_piece(WHITE, PIECE_ROOK);
  board->lookup_table[B1] = encode_piece(WHITE, PIECE_KNIGHT);
  board->lookup_table[C1] = encode_piece(WHITE, PIECE_BISHOP);
  board->lookup_table[D1] = encode_piece(WHITE, PIECE_QUEEN);
  board->lookup_table[E1] = encode_piece(WHITE, PIECE_KING);
  board->lookup_table[F1] = encode_piece(WHITE, PIECE_BISHOP);
  board->lookup_table[G1] = encode_piece(WHITE, PIECE_KNIGHT);
  board->lookup_table[H1] = encode_piece(WHITE, PIECE_ROOK);

  for (int sq = A2; sq <= H2; sq++) {
    board->lookup_table[sq] = encode_piece(WHITE, PIECE_PAWN);
  }

  board->lookup_table[A8] = encode_piece(BLACK, PIECE_ROOK);
  board->lookup_table[B8] = encode_piece(BLACK, PIECE_KNIGHT);
  board->lookup_table[C8] = encode_piece(BLACK, PIECE_BISHOP);
  board->lookup_table[D8] = encode_piece(BLACK, PIECE_QUEEN);
  board->lookup_table[E8] = encode_piece(BLACK, PIECE_KING);
  board->lookup_table[F8] = encode_piece(BLACK, PIECE_BISHOP);
  board->lookup_table[G8] = encode_piece(BLACK, PIECE_KNIGHT);
  board->lookup_table[H8] = encode_piece(BLACK, PIECE_ROOK);

  for (int sq = A7; sq <= H7; sq++) {
    board->lookup_table[sq] = encode_piece(BLACK, PIECE_PAWN);
  }
}

char get_piece_char_at(BoardState *board, Square square) {
  int color, piece = -1;
  get_piece_at(board, square, &color, &piece);
  return get_piece_char(board, static_cast<PieceColor>(color),
                        static_cast<PieceType>(piece));
}

static char get_piece_char(BoardState *board, PieceColor color,
                           PieceType piece) {
  if (piece == PIECE_NONE) return ' ';

  const char piece_char[2][6] = {{'P', 'N', 'B', 'R', 'Q', 'K'},
                                 {'p', 'n', 'b', 'r', 'q', 'k'}};

  return piece_char[color][piece - 1];
}

bool move_piece(BoardState *board, Square from, Square to) {
  int piece_color, piece_type = -1;
  get_piece_at(board, from, &piece_color, &piece_type);

  if (!validate_move(board, static_cast<PieceColor>(piece_color),
                     static_cast<PieceType>(piece_type)))
    return false;
  if (piece_type == PIECE_PAWN) board->halfmove_clock = 0;

  remove_piece(board, static_cast<PieceColor>(piece_color),
               static_cast<PieceType>(piece_type), from);
  add_piece(board, static_cast<PieceColor>(piece_color),
            static_cast<PieceType>(piece_type), to);

  pass_turn(board);
  return true;
}

static void get_piece_at(BoardState *board, int square, int *color, int *type) {
  uint8_t piece = board->lookup_table[square];

  if (piece)
    decode_piece(piece, color, type);
  else
    *type = PIECE_NONE;
}

static bool validate_move(BoardState *board, PieceColor piece_color,
                          PieceType piece_type) {
  // No piece at the source square
  if (piece_type == PIECE_NONE) return false;

  // Not the player's turn
  if (piece_color != board->to_move) return false;

  return true;
}

static void pass_turn(BoardState *board) {
  int past_turn = board->to_move;
  if (past_turn == BLACK) board->fullmove_counter++;

  board->to_move = (past_turn == WHITE) ? BLACK : WHITE;
}

static void add_piece(BoardState *board, PieceColor color, PieceType piece,
                      Square square) {
  uint64_t square_bit = square_to_bit(square);
  board->bitboards[color][piece] |= square_bit;
  board->bitboards[color][PIECE_NONE] &= ~square_bit;

  // Update lookup table
  board->lookup_table[square] = encode_piece(color, piece);
}

static void remove_piece(BoardState *board, PieceColor color, PieceType piece,
                         Square square) {
  uint64_t square_bit = square_to_bit(square);
  board->bitboards[color][piece] &= ~square_bit;
  board->bitboards[color][PIECE_NONE] |= square_bit;

  // Update lookup table
  board->lookup_table[square] = 0; // Empty square
}
