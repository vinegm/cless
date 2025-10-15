cmake_minimum_required(VERSION 3.25)
project(cless LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "Generate compile_commands.json" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCES
    src/main.cpp
    src/win_handler.cpp
    src/menu.cpp
    src/board.cpp
    src/size_warning.cpp
    src/engine.cpp
    src/position.cpp
    src/move_gen.cpp
    src/utils.cpp)

add_executable(cless.out ${SOURCES})
target_link_libraries(cless.out ncurses)
target_include_directories(cless.out PRIVATE ${PROJECT_SOURCE_DIR}/include)

enable_testing()

find_library(CRITERION_LIB criterion)
include_directories(/usr/include)

set(TEST_SOURCES tests/engine.cpp src/engine.cpp src/position.cpp src/move_gen.cpp)

add_executable(tests.out ${TEST_SOURCES})
target_link_libraries(tests.out ${CRITERION_LIB})
target_include_directories(tests.out PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_test(NAME engine_tests COMMAND tests.out)
