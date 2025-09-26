#include "engine.h"
#include <criterion/criterion.h>
#include <criterion/internal/test.h>

void check_made_move(BoardState *board, bool result, Square from, Square to,
                     char expected_piece, char expected_target,
                     PieceColor expected_next_turn) {
  char piece_at_from = get_piece_char_at(board, from);
  char piece_at_to = get_piece_char_at(board, to);

  if (!result) {
    cr_assert_eq(piece_at_from, expected_piece,
                 "Expected source square '%c' to still have the piece after "
                 "invalid move",
                 expected_piece);
    cr_assert_eq(
        piece_at_to, expected_target,
        "Expected target square '%c' to be unchanged after invalid move",
        expected_target);
    return;
  }

  cr_assert_eq(piece_at_from, ' ',
               "Expected source square to be empty after move");
  cr_assert_eq(piece_at_to, expected_piece,
               "Expected target square to have moved piece '%c' after move",
               expected_piece);
  cr_assert_eq(board->to_move, expected_next_turn,
               "Expected turn to switch correctly after move");
}

Test(engine, pawn_e2_to_e4) {
  BoardState board;
  init_chess_board(&board);

  bool result = move_piece(&board, E2, E4);
  cr_assert_eq(result, true, "Expected move to be valid");

  check_made_move(&board, result, E2, E4, 'P', ' ', BLACK);
}

Test(engine, incorrect_piece_color) {
  BoardState board;
  init_chess_board(&board);

  bool result = move_piece(&board, E7, E5);
  cr_assert_eq(result, false, "Expected move to be invalid");

  check_made_move(&board, result, E7, E5, 'p', ' ', WHITE);
}

Test(engine, move_invalid_piece) {
  BoardState board;
  init_chess_board(&board);

  bool result = move_piece(&board, E3, E4);
  cr_assert_eq(result, false, "Expected move to be invalid");

  check_made_move(&board, result, E3, E4, ' ', ' ', WHITE);
}
