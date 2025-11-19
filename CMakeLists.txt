cmake_minimum_required(VERSION 3.25)
project(cless LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS
    ON
    CACHE BOOL "Generate compile_commands.json" FORCE)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

if(DEFINED ENV{CXXFLAGS})
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} $ENV{CXXFLAGS}")
endif()

if(DEFINED ENV{LDFLAGS})
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} $ENV{LDFLAGS}")
endif()

set(CORE_SOURCES src/game_logic.cpp src/position.cpp src/move_gen.cpp
                 src/attacks.cpp src/ext_engine.cpp)

set(TUI_SOURCES src/main.cpp src/menu.cpp src/board.cpp src/popup.cpp
                src/size_warning.cpp src/utils.cpp)

add_library(core STATIC ${CORE_SOURCES})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)

add_executable(cless ${TUI_SOURCES})
target_link_libraries(cless ncurses panel core)
target_include_directories(cless PRIVATE ${PROJECT_SOURCE_DIR}/include)

option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
  enable_testing()

  find_library(CRITERION_LIB criterion)
  include_directories(/usr/include)

  set(TEST_SOURCES tests/perft_tests.cpp tests/unique_moves.cpp)

  add_executable(tests ${TEST_SOURCES})
  target_link_libraries(tests ${CRITERION_LIB} core)
  target_include_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/include)

  add_test(NAME game_logic_tests COMMAND tests)
endif()
