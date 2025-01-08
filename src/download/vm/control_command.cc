#include "command.h"
#include "control_command.h"
#include "move_command.h"
#include "colon_command.h"
#include "model.h"
#include "posn.h"

Abstract_Control_Command::Abstract_Control_Command(const int multiplier, Editor& editor):
    Abstract_Command{multiplier, editor} {}

bool Abstract_Control_Command::undo() { return false; }

Half_Scroll_Up_Control_Command::Half_Scroll_Up_Control_Command(const int multiplier, Editor& editor):
    Abstract_Control_Command{multiplier, editor} {}

void Half_Scroll_Up_Control_Command::execute() {
    Page& page = editor.get_page();
    Posn screen_size = editor.get_display().get_screen_size();
    int new_top_line_idx = page.get_top_line_idx() - (screen_size.row - 1)/2;
    if (new_top_line_idx < 0 || new_top_line_idx >= editor.get_file().get_num_lines() - 1) return;
    page.set_top_line_idx(new_top_line_idx);
}

Half_Scroll_Down_Control_Command::Half_Scroll_Down_Control_Command(const int multiplier, Editor& editor):
    Abstract_Control_Command{multiplier, editor} {}

void Half_Scroll_Down_Control_Command::execute() {
    Page& page = editor.get_page();
    Posn screen_size = editor.get_display().get_screen_size();
    int new_top_line_idx = page.get_top_line_idx() + (screen_size.row - 1)/2;
    if (new_top_line_idx < 0 || new_top_line_idx >= editor.get_file().get_num_lines() - 1) return;
    page.set_top_line_idx(new_top_line_idx);
}

Full_Scroll_Up_Control_Command::Full_Scroll_Up_Control_Command(const int multiplier, Editor& editor):
    Abstract_Control_Command{multiplier, editor} {}

void Full_Scroll_Up_Control_Command::execute() {
    Page& page = editor.get_page();
    Posn screen_size = editor.get_display().get_screen_size();
    int new_top_line_idx = page.get_top_line_idx() - (screen_size.row - 1);
    if (new_top_line_idx < 0 || new_top_line_idx >= editor.get_file().get_num_lines() - 1) return;
    page.set_top_line_idx(new_top_line_idx);
}

Full_Scroll_Down_Control_Command::Full_Scroll_Down_Control_Command(const int multiplier, Editor& editor):
    Abstract_Control_Command{multiplier, editor} {}

void Full_Scroll_Down_Control_Command::execute() {
    Page& page = editor.get_page();
    Posn screen_size = editor.get_display().get_screen_size();
    int new_top_line_idx = page.get_top_line_idx() + screen_size.row - 1;
    if (new_top_line_idx < 0 || new_top_line_idx >= editor.get_file().get_num_lines() - 1) return;
    page.set_top_line_idx(new_top_line_idx);
}


