#ifndef CONTROLLER_H
#define CONTROLLER_H

#include <memory>
#include <string>
#include <unordered_map>
#include "command.h"
#include "model.h"
#include "trie.h"

using std::string;
using std::unique_ptr;
using std::unordered_map;

class Editor;
class Abstract_Command;

class Controller {
private:
    Editor& editor;
    Trie command_trie;
    unordered_map<string, int> command_lookup;
    int get_num() const;
public:
    Controller(Editor& editor);
    void init();
    char get_input() const;
    unique_ptr<Abstract_Command> take_command(string existing_buffer="");
};

#endif
