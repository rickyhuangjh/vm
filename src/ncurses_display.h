#ifndef NCURSES_DISPLAY_H
#define NCURSES_DISPLAY_H

#include <ncurses.h>
#include <memory>
#include <string>
#include <vector>

#include "model.h"

using std::vector;
using std::string;

class Editor;

class NCurses_Display {
private:
    const Editor& editor;
    string command_string;
    void refresh_status_bar();
public:
    NCurses_Display(const Editor& editor);
    void open_display();
    void close_display();
    void refresh_display();
    Posn get_screen_size() const;
    void set_command_string(const string& new_command_string);
};

#endif
