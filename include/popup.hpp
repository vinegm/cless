#pragma once

#include "utils.hpp"

#include <functional>
#include <ncurses.h>
#include <panel.h>
#include <string>
#include <unordered_map>
#include <vector>

class Popup {
public:
  Popup(const std::vector<std::string> &messages, const std::vector<std::string> &options = {});

  void update();
  void show();
  void hide();
  void recenter();
  bool is_visible() const { return visible; }

  int handle_input(int pressed_key);
  int get_selected_option() const { return selected_option; }
  void reset_selection() {
    if (options.size() == 0) {
      selected_option = -1;
      return;
    }
    selected_option = 0;
  }

private:
  UniqueWindow popup_win;
  UniquePanel popup_panel;
  bool visible = false;

  int options_start_line = -1;
  int selected_option = -1;
  std::vector<std::string> options;
};

class PopupHandler {
public:
  using InputHandler = std::function<bool(int, int)>;

  void add_popup(const std::string &name, Popup &popup, InputHandler handler = nullptr);
  void update();
  int handle_input(int pressed_key);
  bool any_visible() const;
  void show_popup(const std::string &name);
  void hide_all();
  void recenter_popups();

private:
  struct PopupEntry {
    std::string name;
    Popup &popup;
    InputHandler handler;
  };

  std::unordered_map<std::string, PopupEntry> popups;

  mutable PopupEntry *active_popup = nullptr;
  PopupEntry *get_active() const;
};
