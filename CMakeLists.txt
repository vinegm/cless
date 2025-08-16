cmake_minimum_required(VERSION 3.25)
project(cless C)

set(SOURCES src/main.c src/menu.c src/board.c src/engine.c src/utils.c)

add_executable(cless.out ${SOURCES})
target_link_libraries(cless.out ncurses)
target_include_directories(cless.out PRIVATE ${PROJECT_SOURCE_DIR}/src)

enable_testing()

find_package(PkgConfig REQUIRED)
pkg_check_modules(CHECK REQUIRED check)

include_directories(${CHECK_INCLUDE_DIRS})
link_directories(${CHECK_LIBRARY_DIRS})

set(TEST_SOURCES tests/test_converter.c src/converter.c)
add_executable(test_converter ${TEST_SOURCES})
target_link_libraries(test_converter ${CHECK_LIBRARIES})
target_include_directories(test_converter PRIVATE ${PROJECT_SOURCE_DIR}/src)

add_test(NAME ConverterTests COMMAND test_converter)
