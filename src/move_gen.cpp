#include "move_gen.hpp"
#include "chess_types.hpp"
#include <cstdint>
#include <vector>

std::vector<Move>
MoveGenerator::generate_pseudo_legal_moves(const Position &position) const {
  std::vector<Move> moves;

  generate_pawn_moves(position, moves);
  generate_knight_moves(position, moves);
  // generate_bishop_moves(position, moves);
  // generate_rook_moves(position, moves);
  // generate_queen_moves(position, moves);
  generate_king_moves(position, moves);

  return moves;
}

std::vector<Move>
MoveGenerator::generate_legal_moves(const Position &position) const {
  std::vector<Move> pseudo_legal = generate_pseudo_legal_moves(position);
  std::vector<Move> legal_moves;

  for (const Move &move : pseudo_legal) {
    if (is_legal_move(position, move)) { legal_moves.push_back(move); }
  }

  return legal_moves;
}

void MoveGenerator::generate_pawn_moves(const Position &position,
                                        std::vector<Move> &moves) const {
  PieceColor us = position.to_move;
  PieceColor them = opposite_color(us);

  uint64_t our_pawns = position.bitboards[bitboard_index(us, PIECE_PAWN)];
  uint64_t enemy_pieces = position.occupancy[them];
  uint64_t all_pieces = position.occupancy[WHITE] | position.occupancy[BLACK];
  uint64_t empty_squares = ~all_pieces;

  int forward_direction = (us == WHITE) ? 8 : -8;

  uint64_t starting_rank = (us == WHITE) ? RANK_2 : RANK_7;
  uint64_t promotion_rank = (us == WHITE) ? RANK_8 : RANK_1;

  // Single pushes
  uint64_t single_pushes;
  if (us == WHITE) {
    single_pushes = (our_pawns << 8) & empty_squares;
  } else {
    single_pushes = (our_pawns >> 8) & empty_squares;
  }

  while (single_pushes) {
    Square to = static_cast<Square>(pop_lsb(single_pushes));
    Square from = static_cast<Square>(to - forward_direction);

    if (square_to_bit(to) & promotion_rank) {
      moves.push_back(
          {.from = from, .to = to, .promotion_piece = PIECE_KNIGHT});
      moves.push_back(
          {.from = from, .to = to, .promotion_piece = PIECE_BISHOP});
      moves.push_back({.from = from, .to = to, .promotion_piece = PIECE_ROOK});
      moves.push_back({.from = from, .to = to, .promotion_piece = PIECE_QUEEN});
    } else {
      moves.push_back({.from = from, .to = to});
    }
  }

  // Double pushes
  uint64_t double_pushes;
  if (us == WHITE) {
    double_pushes = ((our_pawns & starting_rank) << 16) & empty_squares;
  } else {
    double_pushes = ((our_pawns & starting_rank) >> 16) & empty_squares;
  }

  while (double_pushes) {
    Square to = static_cast<Square>(pop_lsb(double_pushes));
    Square from = static_cast<Square>(to - 2 * forward_direction);

    moves.push_back({from, to});
  }

  // Regular captures
  uint64_t our_pawns_copy = our_pawns;
  while (our_pawns_copy) {
    Square from = static_cast<Square>(pop_lsb(our_pawns_copy));
    uint64_t attacks = PAWN_ATTACKS[us][from] & enemy_pieces;

    while (attacks) {
      Square to = static_cast<Square>(pop_lsb(attacks));

      if (square_to_bit(to) & promotion_rank) {
        moves.push_back(
            {.from = from, .to = to, .promotion_piece = PIECE_KNIGHT});
        moves.push_back(
            {.from = from, .to = to, .promotion_piece = PIECE_BISHOP});
        moves.push_back(
            {.from = from, .to = to, .promotion_piece = PIECE_ROOK});
        moves.push_back(
            {.from = from, .to = to, .promotion_piece = PIECE_QUEEN});
      } else {
        moves.push_back({from, to});
      }
    }
  }

  // En passant captures
  if (position.en_passant_square.has_value()) {
    Square en_passant_square = position.en_passant_square.value();

    our_pawns_copy = our_pawns;
    while (our_pawns_copy) {
      Square from = static_cast<Square>(pop_lsb(our_pawns_copy));
      uint64_t attacks = PAWN_ATTACKS[us][from];

      // Check if this pawn can attack the en passant square
      if (attacks & square_to_bit(en_passant_square)) {
        moves.push_back({from, en_passant_square});
      }
    }
  }
}

void MoveGenerator::generate_knight_moves(const Position &position,
                                          std::vector<Move> &moves) const {
  PieceColor us = position.to_move;
  uint64_t our_knights = position.bitboards[bitboard_index(us, PIECE_KNIGHT)];
  uint64_t our_pieces = position.occupancy[us];

  while (our_knights) {
    Square from = static_cast<Square>(pop_lsb(our_knights));
    uint64_t attacks = KNIGHT_ATTACKS[from] & ~our_pieces;

    while (attacks) {
      Square to = static_cast<Square>(pop_lsb(attacks));
      moves.push_back({from, to});
    }
  }
}

void MoveGenerator::generate_bishop_moves(const Position &position,
                                          std::vector<Move> &moves) const {}

void MoveGenerator::generate_rook_moves(const Position &position,
                                        std::vector<Move> &moves) const {}

void MoveGenerator::generate_queen_moves(const Position &position,
                                         std::vector<Move> &moves) const {}

void MoveGenerator::generate_king_moves(const Position &position,
                                        std::vector<Move> &moves) const {
  PieceColor us = position.to_move;
  Square king_square = find_king(position, us);
  uint64_t attacks = KING_ATTACKS[king_square] & ~position.occupancy[us];

  while (attacks) {
    Square to = static_cast<Square>(pop_lsb(attacks));
    moves.push_back({king_square, to});
  }
}

bool MoveGenerator::is_square_attacked(const Position &position, Square square,
                                       PieceColor enemy_color) const {
  return false;
}

bool MoveGenerator::is_in_check(const Position &position,
                                PieceColor color) const {
  Square king_square = find_king(position, color);
  return is_square_attacked(position, king_square, opposite_color(color));
}

bool MoveGenerator::is_legal_move(const Position &position,
                                  const Move &move) const {
  uint8_t moved_piece_encoded = position.lookup_table[move.from];
  Piece moved_piece = decode_piece(moved_piece_encoded);

  if (moved_piece.type == PIECE_NONE) return false;
  if (position.to_move != moved_piece.color) return false;

  Position temp_position = position;
  temp_position.make_move(move);

  if (is_in_check(temp_position, position.to_move)) return false;

  return true;
}

Square MoveGenerator::find_king(const Position &position,
                                PieceColor color) const {
  uint64_t king = position.bitboards[bitboard_index(color, PIECE_KING)];
  return static_cast<Square>(lsb_index(king));
}
