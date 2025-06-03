#include <ncurses.h>
#include <string>

int main() {
    initscr();

    int height = getmaxy(stdscr);
    int width = getmaxx(stdscr);

    std::string text = "centered text";

    mvprintw(height / 2, (width - text.length()) / 2, text.c_str());

    refresh();

    getch();
    endwin();
    return 0;
}

