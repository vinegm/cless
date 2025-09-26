cmake_minimum_required(VERSION 3.25)
project(cless)

set(SOURCES src/main.c src/menu.c src/board.c src/engine.c src/utils.c
            src/tui_handler.c)

add_executable(cless.out ${SOURCES})
target_link_libraries(cless.out ncurses)
target_include_directories(cless.out PRIVATE ${PROJECT_SOURCE_DIR}/src)
