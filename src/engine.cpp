#include "engine.hpp"
#include <cstdint>
#include <ncurses.h>

BoardState::BoardState() : bitboards{}, lookup_table{} {
  this->bitboards[WHITE][PIECE_PAWN] = RANK_2;
  this->bitboards[WHITE][PIECE_KNIGHT] = square_to_bit(B1) | square_to_bit(G1);
  this->bitboards[WHITE][PIECE_BISHOP] = square_to_bit(C1) | square_to_bit(F1);
  this->bitboards[WHITE][PIECE_ROOK] = square_to_bit(A1) | square_to_bit(H1);
  this->bitboards[WHITE][PIECE_QUEEN] = square_to_bit(D1);
  this->bitboards[WHITE][PIECE_KING] = square_to_bit(E1);

  this->bitboards[BLACK][PIECE_PAWN] = RANK_7;
  this->bitboards[BLACK][PIECE_KNIGHT] = square_to_bit(B8) | square_to_bit(G8);
  this->bitboards[BLACK][PIECE_BISHOP] = square_to_bit(C8) | square_to_bit(F8);
  this->bitboards[BLACK][PIECE_ROOK] = square_to_bit(A8) | square_to_bit(H8);
  this->bitboards[BLACK][PIECE_QUEEN] = square_to_bit(D8);
  this->bitboards[BLACK][PIECE_KING] = square_to_bit(E8);

  this->bitboards[WHITE][PIECE_NONE] = ~(RANK_1 | RANK_2);
  this->bitboards[BLACK][PIECE_NONE] = ~(RANK_7 | RANK_8);

  this->lookup_table[A1] = this->encode_piece(WHITE, PIECE_ROOK);
  this->lookup_table[B1] = this->encode_piece(WHITE, PIECE_KNIGHT);
  this->lookup_table[C1] = this->encode_piece(WHITE, PIECE_BISHOP);
  this->lookup_table[D1] = this->encode_piece(WHITE, PIECE_QUEEN);
  this->lookup_table[E1] = this->encode_piece(WHITE, PIECE_KING);
  this->lookup_table[F1] = this->encode_piece(WHITE, PIECE_BISHOP);
  this->lookup_table[G1] = this->encode_piece(WHITE, PIECE_KNIGHT);
  this->lookup_table[H1] = this->encode_piece(WHITE, PIECE_ROOK);

  for (int sq = A2; sq <= H2; sq++) {
    this->lookup_table[sq] = this->encode_piece(WHITE, PIECE_PAWN);
  }

  this->lookup_table[A8] = this->encode_piece(BLACK, PIECE_ROOK);
  this->lookup_table[B8] = this->encode_piece(BLACK, PIECE_KNIGHT);
  this->lookup_table[C8] = this->encode_piece(BLACK, PIECE_BISHOP);
  this->lookup_table[D8] = this->encode_piece(BLACK, PIECE_QUEEN);
  this->lookup_table[E8] = this->encode_piece(BLACK, PIECE_KING);
  this->lookup_table[F8] = this->encode_piece(BLACK, PIECE_BISHOP);
  this->lookup_table[G8] = this->encode_piece(BLACK, PIECE_KNIGHT);
  this->lookup_table[H8] = this->encode_piece(BLACK, PIECE_ROOK);

  for (int sq = A7; sq <= H7; sq++) {
    this->lookup_table[sq] = this->encode_piece(BLACK, PIECE_PAWN);
  }
}

Piece BoardState::get_piece_at(Square square) {
  uint8_t encoded_piece = this->lookup_table[square];
  if (encoded_piece == 0) return Piece{WHITE, PIECE_NONE};

  PieceColor color = this->decode_color(encoded_piece);
  PieceType type = this->decode_type(encoded_piece);
  return Piece{color, type};
}

bool BoardState::move_piece(Square from, Square to) {
  uint8_t encoded_piece = this->lookup_table[from];
  if (encoded_piece == 0) return false;

  PieceColor piece_color = this->decode_color(encoded_piece);
  PieceType piece_type = this->decode_type(encoded_piece);

  if (!validate_move(piece_color, piece_type)) return false;
  if (piece_type == PIECE_PAWN) this->halfmove_clock = 0;

  this->remove_piece(piece_color, piece_type, from);
  this->add_piece(piece_color, piece_type, to);

  this->pass_turn();
  return true;
}

bool BoardState::validate_move(PieceColor piece_color, PieceType piece_type) {
  // No piece at the source square
  if (piece_type == PIECE_NONE) return false;

  // Not the player's turn
  if (piece_color != this->to_move) return false;

  return true;
}

void BoardState::add_piece(PieceColor color, PieceType piece, Square square) {
  uint64_t square_bit = square_to_bit(square);
  this->bitboards[color][piece] |= square_bit;
  this->bitboards[color][PIECE_NONE] &= ~square_bit;

  this->lookup_table[square] = this->encode_piece(color, piece);
}

void BoardState::remove_piece(PieceColor color, PieceType piece,
                              Square square) {
  uint64_t square_bit = square_to_bit(square);
  this->bitboards[color][piece] &= ~square_bit;
  this->bitboards[color][PIECE_NONE] |= square_bit;

  this->lookup_table[square] = 0;
}

void BoardState::pass_turn() {
  bool black_played = this->to_move == BLACK;
  if (black_played) this->fullmove_counter++;

  this->to_move = black_played ? WHITE : BLACK;
}
