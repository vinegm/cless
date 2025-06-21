#include <stdint.h>

typedef struct {
  uint64_t occupied;
  uint64_t whitePieces;
  uint64_t blackPieces;

  uint64_t blackPawns;
  uint64_t blackKnights;
  uint64_t blackBishops;
  uint64_t blackRooks;
  uint64_t blackQueens;
  uint64_t blackKing;

  uint64_t whitePawns;
  uint64_t whiteKnights;
  uint64_t whiteBishops;
  uint64_t whiteRooks;
  uint64_t whiteQueens;
  uint64_t whiteKing;
} boardState;

// clang-format off
typedef enum {
  A8 = 56, B8, C8, D8, E8, F8, G8, H8,
  A7 = 48, B7, C7, D7, E7, F7, G7, H7,
  A6 = 32, B6, C6, D6, E6, F6, G6, H6,
  A5 = 40, B5, C5, D5, E5, F5, G5, H5,
  A4 = 24, B4, C4, D4, E4, F4, G4, H4,
  A3 = 16, B3, C3, D3, E3, F3, G3, H3,
  A2 = 8, B2, C2, D2, E2, F2, G2, H2,
  A1 = 0, B1, C1, D1, E1, F1, G1, H1,
} Square;
// clang-format on
