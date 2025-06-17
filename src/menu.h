#include <ncurses.h>

typedef struct {
  char **options;
  int optionsCount;
  int selectedOption;
  int highlight;
} MenuOptions;

void menu(WINDOW *parentWin, MenuOptions *menuOptions);
