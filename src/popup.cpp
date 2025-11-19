#include "popup.hpp"

#include "utils.hpp"

#include <ncurses.h>
#include <panel.h>

#define side_padding padding * 2 // border + empty line
#define sides_padding side_padding * 2
#define message_padding padding * 2
#define options_padding message_padding + padding
#define instructions_padding padding * 2

Popup::Popup(const std::vector<std::string> &messages, const std::vector<std::string> &options) :
    options(options) {
  int max_message_width = 0;
  for (const auto &message : messages) {
    max_message_width = std::max(max_message_width, (int)message.length());
  }

  int max_option_width = 0;
  for (const auto &option : options) {
    max_option_width = std::max(max_option_width, (int)option.length());
  }

  int window_width = std::max(max_message_width, max_option_width) + sides_padding;
  int window_height = sides_padding + messages.size();

  if (options.size() > 0) {
    selected_option = 0;
    window_height += options.size() + padding * 2; // empty line + instructions
    options_start_line = messages.size() + options_padding;
  }

  popup_win = create_centered_window(window_height, window_width);
  WINDOW *popup_win_ptr = popup_win.get();
  box(popup_win_ptr, 0, 0);

  popup_panel = UniquePanel(new_panel(popup_win_ptr));

  for (size_t i = 0; i < messages.size(); i++) {
    mvwprintw_centered(popup_win_ptr, window_width, message_padding + i, messages[i]);
  }

  if (options.size() > 0) {
    modifier_wrapper(popup_win_ptr, A_DIM, [&]() {
      mvwprintw_centered(
          popup_win_ptr,
          window_width,
          window_height - instructions_padding,
          "Press 'q' to cancel"
      );
    });
  }

  update(); // Initial draw of interractive elements
  hide_panel(popup_panel.get());
}

void Popup::update() {
  WINDOW *popup_win_ptr = popup_win.get();
  int _, window_width;
  getmaxyx(popup_win_ptr, _, window_width);

  if (options.size() > 0) {
    for (size_t i = 0; i < options.size(); i++) {
      int line = options_start_line + i;
      if (i == selected_option) wattron(popup_win_ptr, A_REVERSE);
      mvwprintw_centered(popup_win_ptr, window_width, line, options[i]);
      if (i == selected_option) wattroff(popup_win_ptr, A_REVERSE);
    }
  }

  wrefresh(popup_win_ptr);
}

void Popup::show() {
  show_panel(popup_panel.get());
  visible = true;

  update_panels();
  doupdate();
}

void Popup::hide() {
  hide_panel(popup_panel.get());
  reset_selection();
  visible = false;

  update_panels();
  doupdate();
}

void Popup::recenter() {
  WINDOW *popup_win_ptr = popup_win.get();
  int popup_height, popup_width;
  getmaxyx(popup_win_ptr, popup_height, popup_width);

  int scr_height, scr_width;
  getmaxyx(stdscr, scr_height, scr_width);

  int new_y = (scr_height - popup_height) / 2;
  int new_x = (scr_width - popup_width) / 2;

  if (new_y < 0) new_y = 0;
  if (new_x < 0) new_x = 0;

  move_panel(popup_panel.get(), new_y, new_x);
  update_panels();
  doupdate();
}

int Popup::handle_input(int pressed_key) {
  if (options.size() == 0) {
    hide();
    return -1;
  }

  if (pressed_key == 'q') {
    hide();
    return -1;
  }

  if (handle_menu_key(pressed_key, selected_option, options.size())) {
    int result = selected_option;
    hide();
    return result;
  }

  update();
  return -1;
}

void PopupHandler::add_popup(const std::string &name, Popup &popup, InputHandler handler) {
  popups.emplace(name, PopupEntry{name, popup, handler});
}

void PopupHandler::update() {
  PopupEntry *active = get_active();
  if (active) active->popup.update();
}

int PopupHandler::handle_input(int pressed_key) {
  PopupEntry *active = get_active();
  if (!active) return -1;

  int popup_result = active->popup.handle_input(pressed_key);

  if (active->handler && popup_result != -1) {
    active->handler(pressed_key, popup_result);
    return popup_result;
  }

  return popup_result;
}

bool PopupHandler::any_visible() const { return get_active() != nullptr; }

void PopupHandler::show_popup(const std::string &name) {
  if (active_popup) {
    active_popup->popup.hide();
    active_popup = nullptr;
  }

  auto it = popups.find(name);
  if (it != popups.end()) {
    it->second.popup.show();
    active_popup = &it->second;
  }
}

void PopupHandler::hide_all() {
  PopupEntry *active = get_active();

  if (active) {
    active->popup.hide();
    active_popup = nullptr;
  }
}

void PopupHandler::recenter_popups() {
  for (auto &pair : popups) {
    pair.second.popup.recenter();
  }
}

PopupHandler::PopupEntry *PopupHandler::get_active() const {
  if (active_popup && active_popup->popup.is_visible()) { return active_popup; }

  active_popup = nullptr;

  for (auto &pair : popups) {
    if (pair.second.popup.is_visible()) {
      active_popup = const_cast<PopupEntry *>(&pair.second);
      return active_popup;
    }
  }

  return nullptr;
}
