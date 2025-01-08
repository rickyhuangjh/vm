#include <fstream>
#include "command.h"
#include "move_command.h"
#include "colon_command.h"
#include "file.h"

Abstract_Colon_Command::Abstract_Colon_Command(Editor& editor):
    Abstract_Command{editor} {}

bool Abstract_Colon_Command::undo() { return false; }

Force_Quit_Colon_Command::Force_Quit_Colon_Command(Editor& editor):
    Abstract_Colon_Command{editor} {}

void Force_Quit_Colon_Command::execute() {
    editor.set_running(false);
}

Save_Colon_Command::Save_Colon_Command(Editor& editor):
    Abstract_Colon_Command{editor} {}

void Save_Colon_Command::execute() { editor.get_file().save(); }

Safe_Quit_Colon_Command::Safe_Quit_Colon_Command(Editor& editor):
    Abstract_Colon_Command{editor} {}

void Safe_Quit_Colon_Command::execute() {
    File& file = editor.get_file();
    std::ifstream f(file.get_file_name(), std::ios::in);
    if (!f.is_open()) return;
    
    string actual_line;
    int i = 0;
    while (std::getline(f, actual_line)) {
        actual_line.push_back('\n');
        string& vm_line = file[i];
        if (vm_line != actual_line) return;
        ++i;
    }
    Force_Quit_Colon_Command(editor).execute();
}

Save_Quit_Colon_Command::Save_Quit_Colon_Command(Editor& editor):
    Abstract_Colon_Command{editor} {}

void Save_Quit_Colon_Command::execute() {
    if (editor.get_file().save()) Safe_Quit_Colon_Command(editor).execute();
}

Line_Num_Colon_Command::Line_Num_Colon_Command(const int line_num, Editor& editor):
    Abstract_Colon_Command{editor}, line_num{line_num} {}

void Line_Num_Colon_Command::execute() {
    Line_Num_Move_Command(line_num, editor).execute();
}

Begin_File_Colon_Command::Begin_File_Colon_Command(Editor& editor):
    Abstract_Colon_Command{editor} {}

void Begin_File_Colon_Command::execute() {
    Begin_File_Move_Command(editor).execute();
}

End_File_Colon_Command::End_File_Colon_Command(Editor& editor):
    Abstract_Colon_Command{editor} {}

void End_File_Colon_Command::execute() {
    End_File_Move_Command(editor).execute();
}


