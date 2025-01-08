#include "command.h"
#include "model.h"

class Editor;
class Invoker;

Abstract_Command::Abstract_Command(Editor& editor):
    multiplier(1), editor(editor) {}
Abstract_Command::Abstract_Command(const int multiplier, Editor& editor):
    multiplier(multiplier), editor(editor) {}

Abstract_Command::~Abstract_Command() {}

int Abstract_Command::get_multiplier() const { return multiplier; }

Undo_Command::Undo_Command(Editor& editor):
    Abstract_Command{1, editor} {}

void Undo_Command::execute() {
    Invoker& invoker = editor.get_invoker();
    invoker.undo_prev_command();
}

bool Undo_Command::undo() { return false; }

Copy_Command::Copy_Command(Editor& editor):
    Abstract_Command{1, editor} {}

void Copy_Command::execute() {
    File& file = editor.get_file();
    Posn posn = editor.get_cursor_posn();
    string line_copy = file[posn.row];
    line_copy.pop_back();
    editor.set_clip(Clip(line_copy, true));
}

bool Copy_Command::undo() { return false; }





