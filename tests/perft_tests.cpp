#include "engine.hpp"
#include <criterion/criterion.h>

Test(perft, initial_position) {
  ClessEngine board(true);

  unsigned long long expected_num_moves[] = {20, 400, 8902, 197281, 4865609ULL};

  for (int depth = 1; depth <= 5; depth++) {
    unsigned long long nodes = board.perft(depth);
    cr_assert_eq(nodes, expected_num_moves[depth - 1],
                 "Perft mismatch at depth %d: got %llu, expected %llu", depth,
                 nodes, expected_num_moves[depth - 1]);
  }
}
