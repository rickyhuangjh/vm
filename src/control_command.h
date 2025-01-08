#ifndef CONTROL_COMMAND_H
#define CONTROL_COMMAND_H

#include "command.h"
#include "model.h"

class Abstract_Control_Command: public Abstract_Command {
private:
public:
    Abstract_Control_Command(const int multiplier, Editor& editor);
    void execute() = 0;
    bool undo() override;
};

class Half_Scroll_Up_Control_Command: public Abstract_Control_Command {
private:
public:
    Half_Scroll_Up_Control_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Half_Scroll_Down_Control_Command: public Abstract_Control_Command {
private:
public:
    Half_Scroll_Down_Control_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Full_Scroll_Up_Control_Command: public Abstract_Control_Command {
private:
public:
    Full_Scroll_Up_Control_Command(const int multiplier, Editor& editor);
    void execute() override;
};

class Full_Scroll_Down_Control_Command: public Abstract_Control_Command {
private:
public:
    Full_Scroll_Down_Control_Command(const int multiplier, Editor& editor);
    void execute() override;
};



#endif
