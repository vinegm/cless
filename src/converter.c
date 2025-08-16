#include "converter.h"

ChessBoardState fenToBitboard(const char *fen);
void bitboardToFen(const ChessBoardState *board, char *fen);
static void placePiece(ChessBoardState *board, char piece, uint64_t mask);

// TODO: After implementing the engine, fix the converter

ChessBoardState fenToBitboard(const char *fen) {
  ChessBoardState board;
  // int row = 0;
  // int col = 0;

  // for (const char *piece = fen; *piece != '\0' && *piece != ' '; piece++) {
  //   if (*piece == '/') {
  //     row++;
  //     col = 0;
  //     continue;
  //   }

  //   if (*piece >= '1' && *piece <= '8') {
  //     col += (*piece - '0');
  //     continue;
  //   }

  //   int bitIndex = (7 - row) * 8 + col;
  //   uint64_t pieceMask = 1ULL << bitIndex;
  //   placePiece(&board, *piece, pieceMask);

  //   col++;
  // }

  // board.occupied = board.whitePieces | board.blackPieces;

  // return board;
  return board;
}

void bitboardToFen(const ChessBoardState *board, char *fen) {
  // const char pieceChars[12] = {'P', 'N', 'B', 'R', 'Q', 'K',
  //                              'p', 'n', 'b', 'r', 'q', 'k'};
  // const uint64_t *bitboards[12] = {
  //     &board->whitePawns, &board->whiteKnights, &board->whiteBishops,
  //     &board->whiteRooks, &board->whiteQueens,  &board->whiteKing,
  //     &board->blackPawns, &board->blackKnights, &board->blackBishops,
  //     &board->blackRooks, &board->blackQueens,  &board->blackKing};

  // int fenIndex = 0;

  // for (int rank = 7; rank >= 0; rank--) {
  //   int empty = 0;

  //   for (int file = 0; file < 8; file++) {
  //     int square = rank * 8 + file;
  //     uint64_t mask = 1ULL << square;

  //     char pieceChar = '\0';

  //     for (int i = 0; i < 12; i++) {
  //       if (*bitboards[i] & mask) {
  //         pieceChar = pieceChars[i];
  //         break;
  //       }
  //     }

  //     if (pieceChar) {
  //       if (empty > 0) {
  //         fen[fenIndex++] = '0' + empty;
  //         empty = 0;
  //       }
  //       fen[fenIndex++] = pieceChar;
  //     } else {
  //       empty++;
  //     }
  //   }

  //   if (empty > 0) { fen[fenIndex++] = '0' + empty; }

  //   if (rank > 0) { fen[fenIndex++] = '/'; }
  // }

  // fen[fenIndex] = '\0';
}

// static void placePiece(ChessBoardState *board, char piece, uint64_t mask) {
//   switch (piece) {
//     case 'p':
//       board->blackPawns |= mask;
//       board->blackPieces |= mask;
//       break;
//     case 'n':
//       board->blackKnights |= mask;
//       board->blackPieces |= mask;
//       break;
//     case 'b':
//       board->blackBishops |= mask;
//       board->blackPieces |= mask;
//       break;
//     case 'r':
//       board->blackRooks |= mask;
//       board->blackPieces |= mask;
//       break;
//     case 'q':
//       board->blackQueens |= mask;
//       board->blackPieces |= mask;
//       break;
//     case 'k':
//       board->blackKing |= mask;
//       board->blackPieces |= mask;
//       break;
//     case 'P':
//       board->whitePawns |= mask;
//       board->whitePieces |= mask;
//       break;
//     case 'N':
//       board->whiteKnights |= mask;
//       board->whitePieces |= mask;
//       break;
//     case 'B':
//       board->whiteBishops |= mask;
//       board->whitePieces |= mask;
//       break;
//     case 'R':
//       board->whiteRooks |= mask;
//       board->whitePieces |= mask;
//       break;
//     case 'Q':
//       board->whiteQueens |= mask;
//       board->whitePieces |= mask;
//       break;
//     case 'K':
//       board->whiteKing |= mask;
//       board->whitePieces |= mask;
//       break;
//     default: break;
//   }
// }
