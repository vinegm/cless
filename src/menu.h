#include <ncurses.h>

void highlight_selected(WINDOW *win, char choices[20], int selectedOption);
int menu_loop(WINDOW *menuWin, char choices[][20], int choicesCount);

