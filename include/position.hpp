#pragma once

#include "chess_types.hpp"
#include <optional>
#include <string>

struct UndoInfo {
  Square from_square{};
  Square to_square{};
  Square captured_square{};

  uint8_t moved_piece_encoded{};
  uint8_t captured_piece_encoded{};

  uint8_t castling_rights{};
  std::optional<Square> en_passant_square{};
  int halfmove_clock{};
};

class Position {
public:
  Position(const std::string &fen);

  PieceColor to_move{};
  uint64_t bitboards[12]{};   // [BitboardIndex]
  uint64_t occupancy[3]{};    // [PieceColor]
  uint8_t lookup_table[64]{}; // Encoded pieces

  uint8_t castling_rights{};
  std::optional<Square> en_passant_square{};
  int halfmove_clock{};
  int fullmove_counter{};

  std::string get_fen() const;
  Piece get_piece_at(Square square);
  void make_move(const Move &move);
  void undo_move();

private:
  UndoInfo undo_info{};

  Square get_captured_square(const Move &move) const;
  void add_piece(PieceColor color, PieceType piece, Square square);
  void remove_piece(PieceColor color, PieceType piece, Square square);
  void pass_turn();

  void set_undo_info(Square from, Square to, Square captured_square,
                     uint8_t moved_piece_encoded,
                     uint8_t captured_piece_encoded);
};
