#include "game_logic.hpp"

#include "chess_types.hpp"
#include "ext_engine.hpp"
#include "position.hpp"

#include <algorithm>
#include <cstdint>

MoveList GameState::get_legal_moves() const {
  if (legal_cache_valid) return legal_moves;

  legal_moves = generator.generate_legal_moves(pos);
  legal_cache_valid = true;

  return legal_moves;
};

MoveList GameState::get_legal_moves_from(Square square) const {
  if (!legal_cache_valid) {
    legal_moves = generator.generate_legal_moves(pos);
    legal_cache_valid = true;
  }

  MoveList filtered_moves;
  for (int i = 0; i < legal_moves.count; i++) {
    if (legal_moves.moves[i].from == square) { filtered_moves.add_move(legal_moves.moves[i]); }
  }

  return filtered_moves;
};

bool GameState::validate_move(const Move &move) const {
  if (!legal_cache_valid) {
    legal_moves = generator.generate_legal_moves(pos);
    legal_cache_valid = true;
  }

  return std::find(legal_moves.begin(), legal_moves.end(), move) != legal_moves.end();
}

MoveList GameState::get_cached_moves() {
  if (!legal_cache_valid) {
    legal_moves = generator.generate_legal_moves(pos);
    legal_cache_valid = true;
  }

  return legal_moves;
};

bool GameState::make_move(const Move &move) {
  if (!validate_move(move)) return false;

  legal_cache_valid = false;
  pos.make_move(move);
  return true;
}

void GameState::undo_move() {
  legal_cache_valid = false;
  pos.undo_move();
}

uint64_t GameState::perft(int depth) {
  if (depth == 0) return 1;

  MoveList moves = get_legal_moves();
  if (depth == 1) return moves.count;

  uint64_t nodes = 0;
  for (int i = 0; i < moves.count; i++) {
    make_move(moves[i]);
    nodes += perft(depth - 1);
    undo_move();
  }

  return nodes;
}

bool GameState::make_engine_move() {
  if (!engine) return false;

  std::string fen = get_fen();
  engine->set_position(fen);

  std::string uci_move = engine->get_best_move(15, 2000);

  if (uci_move.empty()) return false;

  if (uci_move.length() < 4) return false;

  int from_file = uci_move[0] - 'a';
  int from_rank = uci_move[1] - '1';
  int to_file = uci_move[2] - 'a';
  int to_rank = uci_move[3] - '1';

  Square from = static_cast<Square>(from_rank * 8 + from_file);
  Square to = static_cast<Square>(to_rank * 8 + to_file);

  MoveList legal_moves = get_legal_moves_from(from);

  Move *valid_move = nullptr;
  for (int i = 0; i < legal_moves.count; i++) {
    if (legal_moves[i].to == to) {
      if (uci_move.length() == 5) {
        char promotion_char = uci_move[4];
        PieceType expected_promotion = PIECE_NONE;
        switch (promotion_char) {
          case 'q': expected_promotion = PIECE_QUEEN; break;
          case 'r': expected_promotion = PIECE_ROOK; break;
          case 'b': expected_promotion = PIECE_BISHOP; break;
          case 'n': expected_promotion = PIECE_KNIGHT; break;
        }

        if (legal_moves[i].is_promotion() && legal_moves[i].promotion_piece == expected_promotion) {
          valid_move = &legal_moves[i];
          break;
        }
      } else {
        valid_move = &legal_moves[i];
        break;
      }
    }
  }

  if (valid_move) { return make_move(*valid_move); }

  return false;
}
