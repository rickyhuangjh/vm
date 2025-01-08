#ifndef COMMAND_H
#define COMMAND_H

#include "controller.h"
#include "cursor.h"
#include "file.h"
#include "model.h"
#include "ncurses_display.h"

class Controller;
class Editor;
class Invoker;
class NCurses_Display;

class Abstract_Command {
protected:
    int multiplier;
    Editor& editor;
public:
    Abstract_Command(Editor& editor);
    Abstract_Command(const int multiplier, Editor& editor);
    virtual ~Abstract_Command();
    virtual void execute() = 0;
    virtual bool undo() = 0;
    int get_multiplier() const;
};

class Undo_Command: public Abstract_Command {
public:
    Undo_Command(Editor& editor);
    void execute() override;
    bool undo() override;
};

class Copy_Command: public Abstract_Command {
public:
    Copy_Command(Editor& editor);
    void execute() override;
    bool undo() override;
};


#endif
