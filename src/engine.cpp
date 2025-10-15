#include "engine.hpp"
#include "chess_types.hpp"
#include "position.hpp"
#include <algorithm>

std::vector<Move> ClessEngine::get_legal_moves() const {
  if (legal_cache_valid) return legal_moves;

  legal_moves = generator.generate_legal_moves(pos);
  legal_cache_valid = true;

  return legal_moves;
};

std::vector<Move> ClessEngine::get_legal_moves_from(Square square) const {
  if (!legal_cache_valid) {
    legal_moves = generator.generate_legal_moves(pos);
    legal_cache_valid = true;
  }
  std::vector<Move> all_moves = legal_moves;

  for (auto it = all_moves.begin(); it != all_moves.end();) {
    if (it->from != square) {
      it = all_moves.erase(it);
    } else {
      ++it;
    }
  }

  return all_moves;
};

bool ClessEngine::validate_move(const Move &move) const {
  if (!legal_cache_valid) {
    legal_moves = generator.generate_legal_moves(pos);
    legal_cache_valid = true;
  }

  return std::find(legal_moves.begin(), legal_moves.end(), move) !=
         legal_moves.end();
}

bool ClessEngine::make_move(const Move &move) {
  if (!validate_move(move)) return false;

  legal_cache_valid = false;
  pos.make_move(move);
  return true;
}

void ClessEngine::undo_move() {
  legal_cache_valid = false;
  pos.undo_move();
}
