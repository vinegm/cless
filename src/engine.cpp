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
  if (validate_moves && !validate_move(move)) return false;

  legal_cache_valid = false;
  pos.make_move(move);
  return true;
}

void ClessEngine::undo_move() {
  legal_cache_valid = false;
  pos.undo_move();
}

int ClessEngine::perft(int depth) {
  if (depth == 0) return 1;

  std::vector<Move> moves = get_legal_moves();
  if (depth == 1) return moves.size();

  int nodes = 0;
  for (const Move &move : moves) {
    make_move(move);
    nodes += perft(depth - 1);
    undo_move();
  }

  return nodes;
}

std::vector<std::pair<Move, int>> ClessEngine::perft_divide(int depth) {
  std::vector<std::pair<Move, int>> results;

  if (depth == 0) return results;

  std::vector<Move> moves = get_legal_moves();

  for (const Move &move : moves) {
    make_move(move);
    int nodes = (depth == 1) ? 1 : perft(depth - 1);
    results.emplace_back(move, nodes);
    undo_move();
  }

  return results;
}
