#pragma once

#include <ncurses.h>

typedef struct WinHandler WinHandler;
typedef struct WinRef WinRef;

struct WinRef {
  int id;
  void *data;
  void (*draw)(WINDOW *parent_win, void *data);
};

struct WinHandler {
  WinRef *windows[2];
  int current_window;

  WINDOW *main_win;
  int event;
  int exit_event;
  int resize_event;
};

void run_tui(WinHandler *handler);
void add_window(WinHandler *handler, WinRef *window);
