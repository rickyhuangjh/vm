#include "ncurses_display.h"
#include <ncurses.h>
#include <string>
#include <vector>
#include "posn.h"
#include "file.h"

NCurses_Display::NCurses_Display(const Editor& editor): editor{editor} {}


void NCurses_Display::open_display() {
    initscr();
    raw();
    noecho();
    keypad(stdscr, TRUE);
    refresh_display();
}

void NCurses_Display::close_display() {
    endwin();
}

void NCurses_Display::refresh_status_bar() {
    Posn screen_size = get_screen_size();
    Posn cursor_posn = editor.get_cursor_posn();
    int file_length = editor.get_file().get_num_lines();
    move(screen_size.row - 1, 0);
    clrtoeol();
    mvprintw(screen_size.row - 1, 0, "COMMAND: ");
    mvprintw(screen_size.row - 1, 12, "%s", command_string.c_str());

    if (editor.get_edit_mode() == 1) mvprintw(screen_size.row - 1, 18, "%s", "[INSERT]");
    else if (editor.get_edit_mode() == 2) mvprintw(screen_size.row - 1, 18, "%s", "[REPLACE]");
    else mvprintw(screen_size.row - 1, 18, "%s", "         ");

    mvprintw(screen_size.row - 1, screen_size.col - 24, "(%d, %d)", cursor_posn.row + 1, cursor_posn.col + 1);
    
    if (cursor_posn.row == 0) mvprintw(screen_size.row - 1, screen_size.col - 8, "%s", "TOP");
    else if (cursor_posn.row == file_length - 1) {
        mvprintw(screen_size.row - 1, screen_size.col - 8, "%s", "BOTTOM");
    } else {
        int cursor_percent = (cursor_posn.row + 1) * 100 / file_length;
        mvprintw(screen_size.row - 1, screen_size.col - 8, "%d%%", cursor_percent);
    }
    refresh();
}

void NCurses_Display::refresh_display() {
    const Page& page = editor.get_page();
    Posn screen_size = get_screen_size();
    Posn cursor_row_adjusted = page.get_page_cursor_posn();


    // display the text
    int screen_row_idx = 0;
    int page_line_idx = 0;
    size_t line_start_idx = 0;
    int prev_extra_rows = 0;
    while (screen_row_idx < screen_size.row - 1) {
        const string& line = page[page_line_idx];
        move(screen_row_idx, 0); // Move to the beginning of the line
        clrtoeol();  
        mvprintw(screen_row_idx, 0, "%s", line.substr(line_start_idx, screen_size.col).c_str());
        line_start_idx += screen_size.col;
        ++screen_row_idx;
        if (line_start_idx >= line.length()){
            ++page_line_idx;
            line_start_idx = 0;
        } else if (page_line_idx < cursor_row_adjusted.row) ++prev_extra_rows;
    }
    Posn cursor_screen_posn{cursor_row_adjusted.row + prev_extra_rows + cursor_row_adjusted.col / screen_size.col,
        cursor_row_adjusted.col % screen_size.col};

    // display status bar
    refresh_status_bar();

    // display the cursor
    move(cursor_screen_posn.row, cursor_screen_posn.col);
    refresh();
}

Posn NCurses_Display::get_screen_size() const {
    Posn screen_size;
    getmaxyx(stdscr, screen_size.row, screen_size.col);
    return screen_size;
}

void NCurses_Display::set_command_string(const string& new_command_string) {
    command_string = new_command_string;
}


