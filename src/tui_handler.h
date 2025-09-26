#pragma once

#include <ncurses.h>

typedef struct TuiHandler TuiHandler;
typedef struct WinRef WinRef;

struct WinRef {
  int id;
  void *data;
  void (*draw)(WINDOW *parent_win, void *data);
  void (*handle_input)(int key, void *data);
};

struct TuiHandler {
  WinRef *windows[10]; // static array, can switch to dynamic later
  int current_window;

  WINDOW *main_win;
  int event;
  int exit_event;
  int resize_event;
};

void run_tui(TuiHandler* handler);
void add_window(TuiHandler* handler, WinRef* window);