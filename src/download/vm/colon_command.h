#ifndef COLON_COMMAND_H
#define COLON_COMMAND_H

#include "command.h"
#include "model.h"
#include <string>

using std::string;

class Editor;

class Abstract_Colon_Command: public Abstract_Command {
private:
public:
    Abstract_Colon_Command(Editor& editor);
    void execute() = 0;
    bool undo() override;
};

class Force_Quit_Colon_Command: public Abstract_Colon_Command {
private:
public:
    Force_Quit_Colon_Command(Editor& editor);
    void execute() override;
};

class Save_Colon_Command: public Abstract_Colon_Command {
private:
public:
    Save_Colon_Command(Editor& editor);
    void execute() override;
};

class Safe_Quit_Colon_Command: public Abstract_Colon_Command {
private:
public:
    Safe_Quit_Colon_Command(Editor& editor);
    void execute() override;
};

class Save_Quit_Colon_Command: public Abstract_Colon_Command {
private:
public:
    Save_Quit_Colon_Command(Editor& editor);
    void execute() override;
};

class Line_Num_Colon_Command: public Abstract_Colon_Command {
private:
    const int line_num;
public:
    Line_Num_Colon_Command(const int line_num, Editor& editor);
    void execute() override;
};

class Begin_File_Colon_Command: public Abstract_Colon_Command {
private:
public:
    Begin_File_Colon_Command(Editor& editor);
    void execute() override;
};

class End_File_Colon_Command: public Abstract_Colon_Command {
private:
public:
    End_File_Colon_Command(Editor& editor);
    void execute() override;
};


#endif
