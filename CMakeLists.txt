cmake_minimum_required(VERSION 3.25)
project(cless LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "Generate compile_commands.json" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CORE_SOURCES src/engine.cpp src/position.cpp src/move_gen.cpp)

set(TUI_SOURCES src/main.cpp src/win_handler.cpp src/menu.cpp src/board.cpp
                src/size_warning.cpp src/utils.cpp)

set(UCI_SOURCES src/uci_main.cpp src/uci_interface.cpp)

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(cless.out ${TUI_SOURCES})
target_link_libraries(cless.out ncurses core)
target_include_directories(cless.out PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_executable(cless-uci.out ${UCI_SOURCES})
target_link_libraries(cless-uci.out core)
target_include_directories(cless-uci.out PRIVATE ${PROJECT_SOURCE_DIR}/include)

enable_testing()

find_library(CRITERION_LIB criterion)
include_directories(/usr/include)

set(TEST_SOURCES tests/perft_tests.cpp tests/unique_moves.cpp)

add_executable(tests.out ${TEST_SOURCES})
target_link_libraries(tests.out ${CRITERION_LIB} core)
target_include_directories(tests.out PRIVATE ${PROJECT_SOURCE_DIR}/include)

add_test(NAME engine_tests COMMAND tests.out)
