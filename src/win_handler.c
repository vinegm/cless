#include "win_handler.h"
#include "utils.h"

static void show_window(WinHandler *handler, int win_id);

void run_tui(WinHandler *handler) {
  refresh_win(&handler->main_win, WINDOW_HEIGHT, WINDOW_WIDTH);

  while (true) {
    show_window(handler, handler->current_window);

    if (handler->event == handler->exit_event) break;

    if (handler->event == handler->resize_event)
      refresh_win(&handler->main_win, WINDOW_HEIGHT, WINDOW_WIDTH);
  }
}

void add_window(WinHandler *handler, WinRef *window) {
  for (int i = 0; i < 10; i++) {
    if (handler->windows[i] != NULL) continue;

    handler->windows[i] = window;
    window->id = i;
    break;
  }
}

static void show_window(WinHandler *handler, int win_id) {
  int current_win_id = handler->current_window;

  WinRef *window = handler->windows[win_id];

  window->draw(handler->main_win, window->data);
}
