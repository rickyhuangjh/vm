#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H

#include "command.h"
#include "cursor.h"
#include "model.h"


class Abstract_Move_Command: public Abstract_Command {
protected:
    Posn start_posn;
    Posn end_posn;
public:
    Abstract_Move_Command(Editor& editor);
    Abstract_Move_Command(const int multiplier, Editor& editor);
    bool undo() override;
};


class Single_Move_Command: public Abstract_Move_Command {
private:
    const int downward;
    const int rightward;
public:
    void execute() override;
    Single_Move_Command(const int multiplier, Editor& editor,
    const int downward, const int rightward);
};

class Begin_Next_Word_Move_Command: public Abstract_Move_Command {
private:
public:
    void execute() override;
    Begin_Next_Word_Move_Command(const int multiplier, Editor& editor);
};

class Begin_Prev_Word_Move_Command: public Abstract_Move_Command {
private:
public:
    void execute() override;
    Begin_Prev_Word_Move_Command(const int multiplier, Editor& editor);
};

class End_Cur_Word_Move_Command: public Abstract_Move_Command {
private:
public:
    void execute() override;
    End_Cur_Word_Move_Command(const int multiplier, Editor& editor);
};

class Begin_Line_Move_Command: public Abstract_Move_Command {
private:
public:
    void execute() override;
    Begin_Line_Move_Command(Editor& editor);
};

class End_Line_Move_Command: public Abstract_Move_Command {
private:
public:
    void execute() override;
    End_Line_Move_Command(Editor& editor);
};

class First_Non_Blank_Char_Line_Move_Command: public Abstract_Move_Command {
private:
public:
    First_Non_Blank_Char_Line_Move_Command(Editor& editor);
    void execute() override;
};

class Begin_File_Move_Command: public Abstract_Move_Command {
private:
public:
    Begin_File_Move_Command(Editor& editor);
    void execute() override;
};

class End_File_Move_Command: public Abstract_Move_Command {
private:
public:
    End_File_Move_Command(Editor& editor);
    void execute() override;
};

class Line_Num_Move_Command: public Abstract_Move_Command {
private:
    const int line_num;
public:
    Line_Num_Move_Command(const int line_num, Editor& editor);
    void execute() override;
};

class Next_Char_Line_Move_Command: public Abstract_Move_Command {
private:
    char ch;
    bool valid;
public:
    Next_Char_Line_Move_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Prev_Char_Line_Move_Command: public Abstract_Move_Command {
private:
    char ch;
    bool valid;
public:
    Prev_Char_Line_Move_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Before_Next_Char_Line_Move_Command: public Abstract_Move_Command {
private:
    char ch;
    bool valid;
public:
    Before_Next_Char_Line_Move_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Before_Prev_Char_Line_Move_Command: public Abstract_Move_Command {
private:
    char ch;
    bool valid;
public:
    Before_Prev_Char_Line_Move_Command(const int multiplier, Editor& editor);
    void execute() override;
};

#endif
