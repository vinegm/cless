#pragma once

#include "chess_types.hpp"
#include "move_gen.hpp"
#include "position.hpp"
#include <string>

#define INITIAL_POSITION_FEN                                                   \
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"

class ClessEngine {
public:
  ClessEngine() : pos(INITIAL_POSITION_FEN) {}
  ClessEngine(const std::string &fen) : pos(fen) {}

  std::string get_fen() const { return pos.get_fen(); }
  PieceColor to_move() const { return pos.to_move; }
  Piece get_piece_at(Square square) { return pos.get_piece_at(square); }

  std::vector<Move> get_legal_moves() const;
  std::vector<Move> get_legal_moves_from(Square square) const;

  bool make_move(const Move &move);
  void undo_move();

private:
  MoveGenerator generator{};
  Position pos;

  bool validate_move(const Move &move) const;
  mutable bool legal_cache_valid = false;
  mutable std::vector<Move> legal_moves;
};
