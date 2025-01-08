#ifndef MODEL_H
#define MODEL_H

#include <fstream>
#include <memory>
#include <string>
#include <vector>
#include "command.h"
#include "cursor.h"
#include "file.h"
#include "ncurses_display.h"
#include "clip.h"

using std::unique_ptr;
using std::vector;
using std::shared_ptr;

class Abstract_Command;
class Controller;
class Cursor;
class Editor;
class File;
class NCurses_Display;
class Page;

class Invoker {
private:
    vector<unique_ptr<Abstract_Command>> history;
    unique_ptr<Abstract_Command> cur_command;
    Editor& editor;
public:
    Invoker(Editor& editor);
    void add_command(unique_ptr<Abstract_Command>&& command);
    void invoke_cur_command();
    void undo_prev_command();
};

class Page {
    Editor& editor;
    int top_line_idx;
    
public:
    Page(Editor& editor);
    const string& operator[](const int page_line_idx) const;
    const int get_top_line_idx() const;
    void set_top_line_idx(const int new_top_line_idx);
    Posn get_page_cursor_posn() const;
};

class Editor {
private:
    File file;
    Cursor cursor;
    Invoker invoker;
    Clip clip;
    shared_ptr<NCurses_Display> display;
    shared_ptr<Controller> controller;
    Page page;
    bool running;
public:
    Editor(string file_name);
    void run();
    void set_running(bool new_running);
    void init(shared_ptr<NCurses_Display> dp, shared_ptr<Controller> ctrl);

    void refresh_display() const;

    File& get_file();
    const File& get_file() const;

    Posn get_cursor_posn() const;
    void set_cursor_posn(Posn new_cursor_posn);
    void offset_cursor_posn(Posn new_cursor_offset);

    Clip get_clip() const;
    void set_clip(Clip new_clip);

    int get_edit_mode() const;
    void set_edit_mode(const int mode);


    Invoker& get_invoker();

    NCurses_Display& get_display();

    Controller& get_controller();
    const Controller& get_controller() const;

    Page& get_page();
    const Page& get_page() const;
    void update_page_cursor();
};





#endif
