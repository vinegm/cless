#pragma once

#include "chess_types.hpp"
#include "position.hpp"
#include <array>
#include <cstdint>
#include <vector>

constexpr std::array<std::array<uint64_t, 64>, 2> init_pawn_attacks() {
  std::array<std::array<uint64_t, 64>, 2> pawn_attacks{};
  for (int square = A2; square < H7; square++) {
    int rank = square_rank(square);
    int file = square_file(square);

    uint64_t black_attacks = 0ULL;
    uint64_t white_attacks = 0ULL;

    if (file > 0) {
      white_attacks |= (1ULL << (indexes_to_square(rank + 1, file - 1)));
      black_attacks |= (1ULL << (indexes_to_square(rank - 1, file - 1)));
    }

    if (file < 7) {
      black_attacks |= (1ULL << (indexes_to_square(rank - 1, file + 1)));
      white_attacks |= (1ULL << (indexes_to_square(rank + 1, file + 1)));
    }

    pawn_attacks[WHITE][square] = white_attacks;
    pawn_attacks[BLACK][square] = black_attacks;
  }

  return pawn_attacks;
}

constexpr std::array<uint64_t, 64> init_knight_attacks() {
  std::array<uint64_t, 64> knight_attacks{};
  for (int square = 0; square < 64; square++) {
    int rank = square_rank(square);
    int file = square_file(square);
    uint64_t attacks = 0ULL;

    int knight_moves[8][2] = {{-2, -1}, {-2, 1}, {-1, -2}, {-1, 2},
                              {1, -2},  {1, 2},  {2, -1},  {2, 1}};

    for (int i = 0; i < 8; i++) {
      int new_rank = rank + knight_moves[i][0];
      int new_file = file + knight_moves[i][1];

      if (new_rank >= 0 && new_rank < 8 && new_file >= 0 && new_file < 8) {
        int target_square = new_rank * 8 + new_file;
        attacks |= (1ULL << target_square);
      }
    }

    knight_attacks[square] = attacks;
  }

  return knight_attacks;
}

constexpr std::array<uint64_t, 64> init_king_attacks() {
  std::array<uint64_t, 64> king_attacks{};
  for (int square = 0; square < 64; square++) {
    int rank = square_rank(square);
    int file = square_file(square);
    uint64_t attacks = 0ULL;

    int king_moves[8][2] = {{-1, -1}, {-1, 0}, {-1, 1}, {0, -1},
                            {0, 1},   {1, -1}, {1, 0},  {1, 1}};

    for (int i = 0; i < 8; i++) {
      int new_rank = rank + king_moves[i][0];
      int new_file = file + king_moves[i][1];

      if (new_rank >= 0 && new_rank < 8 && new_file >= 0 && new_file < 8) {
        int target_square = new_rank * 8 + new_file;
        attacks |= (1ULL << target_square);
      }
    }

    king_attacks[square] = attacks;
  }

  return king_attacks;
}

class MoveGenerator {
public:
  std::vector<Move> generate_pseudo_legal_moves(const Position &position) const;
  std::vector<Move> generate_legal_moves(const Position &position) const;

private:
  void generate_pawn_moves(const Position &position,
                           std::vector<Move> &moves) const;
  void generate_knight_moves(const Position &position,
                             std::vector<Move> &moves) const;
  void generate_bishop_moves(const Position &position,
                             std::vector<Move> &moves) const;
  void generate_rook_moves(const Position &position,
                           std::vector<Move> &moves) const;
  void generate_queen_moves(const Position &position,
                            std::vector<Move> &moves) const;
  void generate_king_moves(const Position &position,
                           std::vector<Move> &moves) const;
  void generate_castling_moves(const Position &position,
                               std::vector<Move> &moves) const;

  bool is_square_attacked(const Position &position, Square square,
                          PieceColor by_color) const;
  bool is_in_check(const Position &position, PieceColor color) const;
  bool is_legal_move(const Position &position, const Move &move) const;
  Square find_king(const Position &position, PieceColor color) const;

  const std::array<uint64_t, 64> KNIGHT_ATTACKS = init_knight_attacks();
  const std::array<uint64_t, 64> KING_ATTACKS = init_king_attacks();
  const std::array<std::array<uint64_t, 64>, 2> PAWN_ATTACKS =
      init_pawn_attacks();
};
