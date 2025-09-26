cmake_minimum_required(VERSION 3.25)
project(cless)

set(SOURCES src/main.c src/menu.c src/board.c src/engine.c src/utils.c
            src/win_handler.c)

add_executable(cless.out ${SOURCES})
target_link_libraries(cless.out ncurses)
target_include_directories(cless.out PRIVATE ${PROJECT_SOURCE_DIR}/src)

enable_testing()

find_library(CRITERION_LIB criterion)
include_directories(/usr/include)

set(TEST_SOURCES tests/engine.c src/engine.c)

add_executable(tests.out ${TEST_SOURCES})
target_link_libraries(tests.out ${CRITERION_LIB})
target_include_directories(tests.out PRIVATE ${PROJECT_SOURCE_DIR}/src)

add_test(NAME engine_tests COMMAND tests.out)
