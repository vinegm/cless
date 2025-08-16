#include "engine.h"
#include <string.h>

static const char piece_ascii[2][7] = {{' ', 'P', 'R', 'N', 'B', 'Q', 'K'},
                                       {' ', 'p', 'r', 'n', 'b', 'q', 'k'}};

void init_chess_board(ChessBoardState *board) {
  memset(board, 0, sizeof(ChessBoardState));

  board->pieces[WHITE][PIECE_PAWN] = 0x000000000000FF00ULL;
  board->pieces[WHITE][PIECE_ROOK] = 0x0000000000000081ULL;
  board->pieces[WHITE][PIECE_KNIGHT] = 0x0000000000000042ULL;
  board->pieces[WHITE][PIECE_BISHOP] = 0x0000000000000024ULL;
  board->pieces[WHITE][PIECE_QUEEN] = 0x0000000000000008ULL;
  board->pieces[WHITE][PIECE_KING] = 0x0000000000000010ULL;

  board->pieces[BLACK][PIECE_PAWN] = 0x00FF000000000000ULL;
  board->pieces[BLACK][PIECE_ROOK] = 0x8100000000000000ULL;
  board->pieces[BLACK][PIECE_KNIGHT] = 0x4200000000000000ULL;
  board->pieces[BLACK][PIECE_BISHOP] = 0x2400000000000000ULL;
  board->pieces[BLACK][PIECE_QUEEN] = 0x0800000000000000ULL;
  board->pieces[BLACK][PIECE_KING] = 0x1000000000000000ULL;

  board->to_move = WHITE;
}
