#pragma once

#include "chess_types.hpp"
#include "ext_engine.hpp"
#include "move_gen.hpp"
#include "position.hpp"

#include <memory>
#include <stdexcept>

enum GameMode {
  PLAYER_VS_PLAYER,
  PLAYER_VS_ENGINE
};

class GameState {
public:
  GameState(const std::string &engine_cmd, const std::string &fen = INITIAL_POSITION_FEN) :
      pos(fen) {
    set_engine(engine_cmd);
  }
  GameState() : pos(INITIAL_POSITION_FEN) {}

  PieceColor player_color = ANY;
  GameMode current_mode = PLAYER_VS_PLAYER;
  bool ongoing_game = false;
  bool has_engine = false;

  void new_game(GameMode mode) {
    if (mode == PLAYER_VS_ENGINE && !has_engine) {
      throw std::runtime_error("GameState::new_game() - No engine available!");
    }

    pos.set_fen(INITIAL_POSITION_FEN);
    ongoing_game = true;
    current_mode = mode;
  }
  std::string get_fen() const { return pos.get_fen(); }
  void set_fen(const std::string &fen) { return pos.set_fen(fen); }

  PieceColor to_move() const { return pos.to_move; }
  Piece get_piece_at(int square) { return pos.get_piece_at(static_cast<Square>(square)); }
  Piece get_piece_at(Square square) { return pos.get_piece_at(square); }

  MoveList get_legal_moves() const;
  MoveList get_legal_moves_from(Square square) const;

  bool make_move(const Move &move);
  void undo_move();
  uint64_t perft(int depth);

  bool make_engine_move();

private:
  std::unique_ptr<ExtEngine> engine = nullptr;
  MoveGenerator generator;
  Position pos;

  bool validate_move(const Move &move) const;
  mutable bool legal_cache_valid = false;
  mutable MoveList legal_moves{};

  MoveList get_cached_moves();
  void set_engine(const std::string &engine_cmd) {
    if (engine_cmd.empty()) return;

    try {
      engine = std::make_unique<ExtEngine>(engine_cmd);
      has_engine = true;
    } catch (const std::exception &e) { has_engine = false; }
  }
};
