#include "converter.h"
#include <check.h>

START_TEST(test_starting_position) {
  const char *fenIn = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  ChessBoardState board = fenToBitboard(fenIn);

  ck_assert_msg(board.occupied == 18446462598732906495ULL,
                "Expected 0x%016llx but got 0x%016lx", 18446462598732906495ULL,
                board.occupied);

  char fenOut[100];
  bitboardToFen(&board, fenOut);

  const char *expectedFen = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";
  ck_assert_str_eq(expectedFen, fenOut);
}
END_TEST

START_TEST(test_empty_board) {
  const char *fenIn = "8/8/8/8/8/8/8/8";
  ChessBoardState board = fenToBitboard(fenIn);

  ck_assert_msg(board.occupied == 0ULL, "Expected 0x%016llx but got 0x%016lx",
                0ULL, board.occupied);

  char fenOut[100];
  bitboardToFen(&board, fenOut);

  ck_assert_str_eq(fenIn, fenOut);
}
END_TEST

Suite *engine_suite(void) {
  Suite *suite;
  TCase *core;

  suite = suite_create("Converter");
  core = tcase_create("Core");

  tcase_add_test(core, test_starting_position);
  tcase_add_test(core, test_empty_board);

  suite_add_tcase(suite, core);

  return suite;
}

int main(void) {
  int failed_count;
  Suite *suite;
  SRunner *runner;

  suite = engine_suite();
  runner = srunner_create(suite);

  srunner_run_all(runner, CK_NORMAL);
  failed_count = srunner_ntests_failed(runner);
  srunner_free(runner);

  return (failed_count == 0) ? 0 : 1;
}
