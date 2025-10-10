#include "../src/engine.hpp"
#include <criterion/criterion.h>
#include <criterion/internal/test.h>
#include <criterion/logging.h>

struct expected_result {
  bool result;
  Square from;
  Square to;

  Piece piece_at_from;
  Piece piece_at_to;
  PieceColor next_turn;
};

std::string piece_to_string(const Piece &piece) {
  std::string piece_str;

  switch (piece.color) {
    case WHITE: piece_str += "White "; break;
    case BLACK: piece_str += "Black "; break;
    default: piece_str += "Unknown "; break;
  }

  switch (piece.type) {
    case PIECE_PAWN: piece_str += "Pawn"; break;
    case PIECE_ROOK: piece_str += "Rook"; break;
    case PIECE_KNIGHT: piece_str += "Knight"; break;
    case PIECE_BISHOP: piece_str += "Bishop"; break;
    case PIECE_QUEEN: piece_str += "Queen"; break;
    case PIECE_KING: piece_str += "King"; break;
    case PIECE_NONE: piece_str = "Empty"; break;
    default: piece_str = "Unknown"; break;
  }

  return piece_str;
}

void check_made_move(BoardState &board, bool move_result,
                     expected_result &expected) {
  Piece piece_at_from = board.get_piece_at(expected.from);
  Piece piece_at_to = board.get_piece_at(expected.to);

  cr_assert_eq(piece_at_from, expected.piece_at_from,
               "Piece at 'from' mismatch: got %s, expected %s",
               piece_to_string(piece_at_from).c_str(),
               piece_to_string(expected.piece_at_from).c_str());

  cr_assert_eq(piece_at_to, expected.piece_at_to,
               "Piece at 'to' mismatch: got %s, expected %s",
               piece_to_string(piece_at_to).c_str(),
               piece_to_string(expected.piece_at_to).c_str());

  cr_assert_eq(move_result, expected.result, "Move result mismatch");
  cr_assert_eq(board.to_move, expected.next_turn,
               "Expected turn to switch correctly after move");
}

Test(engine, pawn_e2_to_e4) {
  BoardState board;
  expected_result expected = {.result = true,
                              .from = E2,
                              .to = E4,
                              .piece_at_from = {WHITE, PIECE_NONE},
                              .piece_at_to = {WHITE, PIECE_PAWN},
                              .next_turn = BLACK};

  bool result = board.move_piece(expected.from, expected.to);
  check_made_move(board, result, expected);
}

Test(engine, incorrect_piece_color) {
  BoardState board;
  expected_result expected = {.result = false,
                              .from = E7,
                              .to = E5,
                              .piece_at_from = {BLACK, PIECE_PAWN},
                              .piece_at_to = {WHITE, PIECE_NONE},
                              .next_turn = WHITE};

  bool result = board.move_piece(expected.from, expected.to);
  check_made_move(board, result, expected);
}

Test(engine, move_invalid_piece) {
  BoardState board;
  expected_result expected = {.result = false,
                              .from = E3,
                              .to = E4,
                              .piece_at_from = {WHITE, PIECE_NONE},
                              .piece_at_to = {WHITE, PIECE_NONE},
                              .next_turn = WHITE};

  bool result = board.move_piece(expected.from, expected.to);
  check_made_move(board, result, expected);
}
