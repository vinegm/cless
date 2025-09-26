#include "tui_handler.h"
#include "utils.h"
#include <ncurses.h>

static void show_window(TuiHandler* handler, int win_id);

void run_tui(TuiHandler* handler) {
  while (1) {
    show_window(handler, handler->current_window);

    if (handler->event == handler->exit_event) break;

    if (handler->event == handler->resize_event) {
      handle_win(&handler->main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
    }
  }
}

void add_window(TuiHandler* handler, WinRef* window) {
  for (int i = 0; i < 10; i++) {
    if (handler->windows[i] != NULL) continue;

    handler->windows[i] = window;
    window->id = i;
    break;
  }
}

static void show_window(TuiHandler* handler, int win_id) {
  int current_win_id = handler->current_window;

  WinRef *window = handler->windows[win_id];

  window->draw(handler->main_win, window->data);
}
