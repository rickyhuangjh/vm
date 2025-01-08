#include <algorithm>
#include <fstream>
#include "command.h"
#include "model.h"

using std::max;
using std::shared_ptr;
using std::unique_ptr;

const string EMPTY_LINE_STRING = "~";

Editor::Editor(string file_name): file{file_name}, invoker{*this}, page{*this} {}

void Editor::init(shared_ptr<NCurses_Display> dp, shared_ptr<Controller> ctrl) {
    display = dp;
    controller = ctrl;

}

void Editor::run() {
    running = true;
    display->open_display();
    while (running) {
        // controller->get_
        bool got_command = false;
        unique_ptr<Abstract_Command> command;
        while (!got_command) {
            try {
                command = controller->take_command();
                got_command = true;
            } catch (std::invalid_argument const &e) {}
        }
        
        invoker.add_command(std::move(command));
        invoker.invoke_cur_command();
        refresh_display();
    }
    display->close_display();
}

void Editor::set_running(bool new_running) { running = new_running; }


void Editor::refresh_display() const {
    display->refresh_display();
}

File& Editor::get_file() { return file; }
const File& Editor::get_file() const { return file; }

Posn Editor::get_cursor_posn() const { return cursor.get_actual_posn(); }
void Editor::set_cursor_posn(Posn new_cursor_posn) {
    if (new_cursor_posn.row < 0) new_cursor_posn.row = 0;
    else if (new_cursor_posn.row >= file.get_num_lines()) new_cursor_posn.row = file.get_num_lines() - 1;
    Posn cursor_prev_posn = cursor.get_actual_posn();
    if (cursor_prev_posn.col != new_cursor_posn.col ||
        new_cursor_posn.col > cursor.get_ideal_col_idx()) cursor.set_ideal_col_idx(new_cursor_posn.col);
    Posn cursor_ideal_posn{new_cursor_posn.row, cursor.get_ideal_col_idx()};

    
    if (file.is_in_bounds(cursor_ideal_posn)) cursor.set_actual_posn(cursor_ideal_posn);
    else cursor.set_actual_posn(file.bring_in_bounds(new_cursor_posn));
}

Clip Editor::get_clip() const { return clip; }
void Editor::set_clip(Clip new_clip) { clip = new_clip; }

Invoker& Editor::get_invoker() { return invoker; }


NCurses_Display& Editor::get_display() { return *display; }


Controller& Editor::get_controller() { return *controller; }
const Controller& Editor::get_controller() const { return *controller; }

Page& Editor::get_page() { return page; }

const Page& Editor::get_page() const { return page; }

void Editor::update_page_cursor() {
    Posn screen_size = display->get_screen_size();
    Posn cursor_posn = get_cursor_posn();
    if (page.get_top_line_idx() > cursor_posn.row - screen_size.row / 3) {
        page.set_top_line_idx(max(0, cursor_posn.row - screen_size.row / 3));
    }
    if (page.get_top_line_idx() < cursor_posn.row - screen_size.row + 1 + screen_size.row / 3) {
        page.set_top_line_idx(max(0, cursor_posn.row - screen_size.row + 1 + screen_size.row / 3));
    }
    refresh_display();
}

int Editor::get_edit_mode() const { return file.get_edit_mode(); }

void Editor::set_edit_mode(const int mode) {
    file.set_edit_mode(mode);
    refresh_display();
}



Invoker::Invoker(Editor& editor): editor{editor} {}

void Invoker::add_command(unique_ptr<Abstract_Command>&& command) {
    cur_command = std::move(command);
}



void Invoker::invoke_cur_command() {
    int multiplier = cur_command->get_multiplier();
    // MAKE EXCEPTION SAFE
    for (int i=0; i<multiplier; ++i) cur_command->execute();
    if (dynamic_cast<Undo_Command*>(cur_command.get()) == nullptr) {
        history.push_back(std::move(cur_command));
    }
    editor.refresh_display();
}

void Invoker::undo_prev_command() {
    bool status = false;
    while (!history.empty() && !status) {
        int multiplier = history.back()->get_multiplier();
        for (int i=0; i<multiplier; ++i) status = history.back()->undo();
        history.pop_back();
    }
}

Page::Page(Editor& editor): editor{editor}, top_line_idx{0} {}

const string& Page::operator[](const int page_line_idx) const {
    if (top_line_idx + page_line_idx < editor.get_file().get_num_lines()) {
        return editor.get_file()[top_line_idx + page_line_idx];
    }
    return EMPTY_LINE_STRING;
}

const int Page::get_top_line_idx() const { return top_line_idx; }

void Page::set_top_line_idx(const int new_top_line_idx) { top_line_idx = new_top_line_idx; }

Posn Page::get_page_cursor_posn() const { return editor.get_cursor_posn() - Posn{top_line_idx, 0}; }
