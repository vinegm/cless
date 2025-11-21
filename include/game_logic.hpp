#pragma once

#include "chess_types.hpp"
#include "ext_engine.hpp"
#include "move_gen.hpp"
#include "position.hpp"

#include <memory>

enum GameMode {
  PLAYER_VS_PLAYER,
  PLAYER_VS_ENGINE
};

enum GameResult {
  GAME_ONGOING,
  CHECKMATE,
  STALEMATE,
  DRAW_INSUFFICIENT_MATERIAL,
  DRAW_FIFTY_MOVE_RULE,
  DRAW_OTHER
};

class GameState {
public:
  GameState(const std::string &engine_cmd, const std::string &fen = INITIAL_POSITION_FEN) :
      pos(fen) {
    set_engine(engine_cmd);
  }
  GameState() : pos(INITIAL_POSITION_FEN) {}

  void new_game(GameMode mode, PieceColor player_color = ANY);
  void end_game() { ongoing_game = false; }
  bool is_game_ongoing() const { return ongoing_game && get_game_result() == GAME_ONGOING; }
  bool has_engine_available() const { return has_engine; }
  GameMode get_current_mode() const { return current_mode; }
  PieceColor get_player_color() const { return player_color; }

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

  GameResult get_game_result() const;

private:
  PieceColor player_color = ANY;
  GameMode current_mode = PLAYER_VS_PLAYER;
  bool ongoing_game = false;
  bool has_engine = false;

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
