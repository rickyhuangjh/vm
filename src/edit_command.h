#ifndef EDIT_COMMAND_H
#define EDIT_COMMAND_H

#include <memory>
#include <string>
#include <tuple>
#include <vector>
#include "command.h"
#include "move_command.h"
#include "posn.h"
#include "clip.h"

using std::unique_ptr;
using std::string;
using std::tuple;
using std::vector;


class Abstract_Edit_Command: public Abstract_Command {
protected:
    void engage_insert_mode();
    void engage_replace_mode();
    vector<tuple<Posn, int, char>> changed;
public:
    Abstract_Edit_Command(Editor& editor);
    Abstract_Edit_Command(const int multiplier, Editor& editor);
    bool undo() override;
};

class Before_Cursor_Insert_Edit_Command: public Abstract_Edit_Command {
private:
public:
    Before_Cursor_Insert_Edit_Command(Editor& editor);
    void execute() override;
};

class Begin_Line_Insert_Edit_Command: public Abstract_Edit_Command {
private:
public:
    Begin_Line_Insert_Edit_Command(Editor& editor);
    void execute() override;
};

class After_Cursor_Insert_Edit_Command: public Abstract_Edit_Command {
private:
public:
    After_Cursor_Insert_Edit_Command(Editor& editor);
    void execute() override;
};

class End_Line_Insert_Edit_Command: public Abstract_Edit_Command {
private:
public:
    End_Line_Insert_Edit_Command(Editor& editor);
    void execute() override;
};

class New_Line_Below_Insert_Edit_Command: public Abstract_Edit_Command {
private:
public:
    New_Line_Below_Insert_Edit_Command(Editor& editor);
    void execute() override;
};

class New_Line_Above_Insert_Edit_Command: public Abstract_Edit_Command {
private:
public:
    New_Line_Above_Insert_Edit_Command(Editor& editor);
    void execute() override;
};

class Char_Under_Cursor_Delete_Edit_Command: public Abstract_Edit_Command {
private:
public:
    Char_Under_Cursor_Delete_Edit_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Char_Before_Cursor_Delete_Edit_Command: public Abstract_Edit_Command {
private:
public:
    Char_Before_Cursor_Delete_Edit_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Char_Under_Cursor_Replace_Edit_Command: public Abstract_Edit_Command {
private:
    char replacement;
public:
    Char_Under_Cursor_Replace_Edit_Command(const int multiplier, Editor& editor, char ch);
    void execute() override;
};

class Replace_Mode_Edit_Command: public Abstract_Edit_Command {
private:
public:
    Replace_Mode_Edit_Command(Editor& editor);
    void execute() override;
};

class Direction_Motion_Delete_Edit_Command: public Abstract_Edit_Command {
private:
    unique_ptr<Abstract_Move_Command> move_command;
public:
    Direction_Motion_Delete_Edit_Command(const int multiplier, Editor& editor, unique_ptr<Abstract_Move_Command> move_command);
    void execute() override;
};

class Line_Delete_Edit_Command: public Abstract_Edit_Command {
private:
public:
    Line_Delete_Edit_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Direction_Motion_Replace_Edit_Command: public Abstract_Edit_Command {
private:
    Direction_Motion_Delete_Edit_Command delete_command;
public:
    Direction_Motion_Replace_Edit_Command(const int multiplier, Editor& editor, unique_ptr<Abstract_Move_Command> move_command);
    void execute() override;
    bool undo() override;
};

class Line_Replace_Edit_Command: public Abstract_Edit_Command {
private:
    Line_Delete_Edit_Command delete_command;
public:
    Line_Replace_Edit_Command(const int multiplier, Editor& editor);
    void execute() override;
    bool undo() override;
};

class Paste_After_Cursor_Insert_Edit_Command: public Abstract_Edit_Command {
private:
    Clip clip;
public:
    Paste_After_Cursor_Insert_Edit_Command(const int multiplier, Editor& editor);
    void execute();
};

class Paste_Before_Cursor_Insert_Edit_Command: public Abstract_Edit_Command {
private:
    Clip clip;
public:
    Paste_Before_Cursor_Insert_Edit_Command(const int multiplier, Editor& editor);
    void execute();
};



#endif
