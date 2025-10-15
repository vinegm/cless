#include "engine.hpp"
#include "chess_types.hpp"
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

void check_made_move(ClessEngine &board, bool move_result,
                     expected_result &expected) {
  Piece piece_at_from = board.get_piece_at(expected.from);
  Piece piece_at_to = board.get_piece_at(expected.to);

  cr_assert_eq(move_result, expected.result, "Move result mismatch");
  cr_assert_eq(board.to_move(), expected.next_turn,
               "Expected turn to switch correctly after move");

  cr_assert_eq(piece_at_from, expected.piece_at_from,
               "Piece at 'from' mismatch: got %s, expected %s",
               piece_to_string(piece_at_from).c_str(),
               piece_to_string(expected.piece_at_from).c_str());

  cr_assert_eq(piece_at_to, expected.piece_at_to,
               "Piece at 'to' mismatch: got %s, expected %s",
               piece_to_string(piece_at_to).c_str(),
               piece_to_string(expected.piece_at_to).c_str());
}

Test(engine, pawn_e2_to_e4) {
  ClessEngine board;
  expected_result expected = {.result = true,
                              .from = E2,
                              .to = E4,
                              .piece_at_from = {WHITE, PIECE_NONE},
                              .piece_at_to = {WHITE, PIECE_PAWN},
                              .next_turn = BLACK};

  bool result = board.make_move({expected.from, expected.to});
  check_made_move(board, result, expected);
}

Test(engine, e6_en_passant) {
  ClessEngine board(
      "rnbqkbnr/ppp2ppp/8/3Pp3/8/8/PPPP1PPP/RNBQKBNR w KQkq e6 0 1");
  expected_result expected = {.result = true,
                              .from = D5,
                              .to = E6,
                              .piece_at_from = {WHITE, PIECE_NONE},
                              .piece_at_to = {WHITE, PIECE_PAWN},
                              .next_turn = BLACK};

  bool result = board.make_move({expected.from, expected.to});
  check_made_move(board, result, expected);
}

Test(engine, invalid_en_passant) {
  ClessEngine board(
      "rnbqkbnr/ppp2ppp/8/3Pp3/8/8/PPPP1PPP/RNBQKBNR w KQkq - 0 1");
  expected_result expected = {.result = false,
                              .from = D5,
                              .to = E6,
                              .piece_at_from = {WHITE, PIECE_PAWN},
                              .piece_at_to = {WHITE, PIECE_NONE},
                              .next_turn = WHITE};

  bool result = board.make_move({expected.from, expected.to});
  check_made_move(board, result, expected);
}

Test(engine, incorrect_piece_color) {
  ClessEngine board;
  expected_result expected = {.result = false,
                              .from = E7,
                              .to = E5,
                              .piece_at_from = {BLACK, PIECE_PAWN},
                              .piece_at_to = {WHITE, PIECE_NONE},
                              .next_turn = WHITE};

  bool result = board.make_move({expected.from, expected.to});
  check_made_move(board, result, expected);
}

Test(engine, move_invalid_piece) {
  ClessEngine board;
  expected_result expected = {.result = false,
                              .from = E3,
                              .to = E4,
                              .piece_at_from = {WHITE, PIECE_NONE},
                              .piece_at_to = {WHITE, PIECE_NONE},
                              .next_turn = WHITE};

  bool result = board.make_move({expected.from, expected.to});
  check_made_move(board, result, expected);
}

Test(engine, fen_initial_position) {
  std::string expected_fen =
      "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  ClessEngine board;

  std::string fen = board.get_fen();
  cr_assert_eq(fen, expected_fen, "FEN mismatch:\ngot: %s\nexpected: %s",
               fen.c_str(), expected_fen.c_str());
}

Test(engine, fen_random_position) {
  std::string expected_fen =
      "r1bq1rk1/pp1n1ppp/2p1pn2/3p4/3P4/2NBPN2/PP3PPP/R1BQ1RK1 w - - 0 10";
  ClessEngine board(expected_fen);

  std::string fen = board.get_fen();
  cr_assert_eq(fen, expected_fen, "FEN mismatch:\ngot: %s\nexpected: %s",
               fen.c_str(), expected_fen.c_str());
}
