#include <iostream>
#include <memory>
#include <ncurses.h>
#include <stdexcept>
#include <string>
#include "command.h"
#include "controller.h"
#include "move_command.h"
#include "edit_command.h"
#include "colon_command.h"
#include "control_command.h"
#include "model.h"
#include "trie.h"
#include "clip.h"

using std::string;
using std::make_unique;


const char ESCAPE_CHAR = 27;

Controller::Controller(Editor& editor): editor{editor} {
    timeout(50); // milliseconds wait for other characters after ESC entered
    init();
}

void Controller::init() {
    // 100 = move commands, multiplier available
    command_lookup["h"] = 100;
    command_lookup["j"] = 101;
    command_lookup["k"] = 102;
    command_lookup["l"] = 103;
    command_lookup["w"] = 104;
    command_lookup["b"] = 105;
    command_lookup["e"] = 106;
    command_lookup["0"] = 107;
    command_lookup["$"] = 108;
    command_lookup["^"] = 109;
    command_lookup["gg"] = 110;
    command_lookup["G"] = 111; 
    command_lookup["[LINE]G"] = 112;
    command_lookup["f"] = 113;
    command_lookup["F"] = 114;
    command_lookup["t"] = 115;
    command_lookup["T"] = 116;

    // 200 = edit commands, 
    command_lookup["i"] = 200;
    command_lookup["I"] = 201;
    command_lookup["a"] = 202;
    command_lookup["A"] = 203;
    command_lookup["o"] = 204;
    command_lookup["O"] = 205;
    command_lookup["x"] = 206;
    command_lookup["X"] = 207;
    command_lookup["r"] = 208;
    command_lookup["R"] = 209;
    command_lookup["d"] = 210;
    command_lookup["c"] = 211;
    command_lookup["p"] = 212;
    command_lookup["P"] = 213;

    // 300 other commands
    command_lookup["u"] = 300;
    command_lookup["y"] = 301;

    // 400 colon commands
    command_lookup[":q!\n"] = 400;
    command_lookup[":w\n"] = 401;
    command_lookup[":q\n"] = 402;
    command_lookup[":wq\n"] = 403;
    command_lookup[":$\n"] = 404;

    // 500 control commands
    command_lookup["\2"] = 500;
    command_lookup["\4"] = 501;
    command_lookup["\6"] = 502;
    
    for (auto& itr : command_lookup) command_trie.add_word(itr.first);
}

char Controller::get_input() const {
    int ch;
    while ((ch = getch()) == KEY_RESIZE) {
        editor.update_page_cursor();
        editor.refresh_display();
    }
    return ch;
}




std::unique_ptr<Abstract_Command> Controller::take_command(string existing_buffer) {
    char c;
    int multiplier = 0;
    int digit = 0;
    string buffer = existing_buffer;
    bool is_taking_input = true;
    if (existing_buffer.length()) is_taking_input = false;
    while (true) {
        if (is_taking_input) {
            c = get_input();
            if (c == ESCAPE_CHAR) throw std::invalid_argument("");

            buffer += c;
        } else is_taking_input = true;

        if (buffer.length() && buffer[0] == ':') {
            editor.get_display().set_command_string(buffer);
            editor.get_display().refresh_display();
            // check for colon line_num 
            if (buffer.back() == '\n') try {
                const int line_num = std::stoi(buffer.substr(1, buffer.length() - 2));
                
                if (line_num == 0) return make_unique<Begin_File_Colon_Command>(editor);
                return make_unique<Line_Num_Colon_Command>(line_num, editor);
            } catch (std::invalid_argument const &e) {}
            try {
                std::stoi(buffer.substr(1));
                continue;
            } catch (std::invalid_argument const &e) {}
        }

        
        if (!(multiplier && c == '0') && command_lookup.contains(buffer)) {

            // edge case [line num]G
            if (multiplier && buffer == "G") buffer = "[LINE]G";

            if (multiplier == 0) multiplier = 1;
            string command_string = (multiplier > 1? std::to_string(multiplier): "") + buffer;
            editor.get_display().set_command_string(command_string);

            int command_code = command_lookup[buffer];
            switch (command_code) {
                case 100: { return make_unique<Single_Move_Command>(multiplier, editor, 0, -1); }
                case 101: { return make_unique<Single_Move_Command>(multiplier, editor, 1, 0); }
                case 102: { return make_unique<Single_Move_Command>(multiplier, editor, -1, 0);  }
                case 103: { return make_unique<Single_Move_Command>(multiplier, editor, 0, 1); }
                case 104: { return make_unique<Begin_Next_Word_Move_Command>(multiplier, editor); }
                case 105: { return make_unique<Begin_Prev_Word_Move_Command>(multiplier, editor); }
                case 106: { return make_unique<End_Cur_Word_Move_Command>(multiplier, editor); }
                case 107: { return make_unique<Begin_Line_Move_Command>(editor); }
                case 108: { return make_unique<End_Line_Move_Command>(editor); }
                case 109: { return make_unique<First_Non_Blank_Char_Line_Move_Command>(editor); }
                case 110: { return make_unique<Begin_File_Move_Command>(editor); }
                case 111: { return make_unique<End_File_Move_Command>(editor); }
                case 112: { return make_unique<Line_Num_Move_Command>(multiplier, editor); }
                case 113: { return make_unique<Next_Char_Line_Move_Command>(multiplier, editor); }
                case 114: { return make_unique<Prev_Char_Line_Move_Command>(multiplier, editor); }
                case 115: { return make_unique<Before_Next_Char_Line_Move_Command>(multiplier, editor); }
                case 116: { return make_unique<Before_Prev_Char_Line_Move_Command>(multiplier, editor); }
                case 200: { return make_unique<Before_Cursor_Insert_Edit_Command>(editor); }
                case 201: { return make_unique<Begin_Line_Insert_Edit_Command>(editor); }
                case 202: { return make_unique<After_Cursor_Insert_Edit_Command>(editor); }
                case 203: { return make_unique<End_Line_Insert_Edit_Command>(editor); }
                case 204: { return make_unique<New_Line_Below_Insert_Edit_Command>(editor); }
                case 205: { return make_unique<New_Line_Above_Insert_Edit_Command>(editor); }
                case 206: { return make_unique<Char_Under_Cursor_Delete_Edit_Command>(multiplier, editor); }
                case 207: { return make_unique<Char_Before_Cursor_Delete_Edit_Command>(multiplier, editor); }
                case 208: {
                    char ch_replace = get_input();
                    if (!isalnum(ch_replace)) throw std::invalid_argument("");
                    return make_unique<Char_Under_Cursor_Replace_Edit_Command>(multiplier, editor, ch_replace);
                }
                case 209: { return make_unique<Replace_Mode_Edit_Command>(editor); }
                case 210: {
                    char ch = get_input();
                    if (ch == 'd') return make_unique<Line_Delete_Edit_Command>(multiplier, editor);

                    Abstract_Command* command_raw = take_command(string(1, ch)).release();
                    if (!dynamic_cast<Abstract_Move_Command*>(command_raw)) throw std::invalid_argument("");
                    return make_unique<Direction_Motion_Delete_Edit_Command>(multiplier, editor,
                        unique_ptr<Abstract_Move_Command>(dynamic_cast<Abstract_Move_Command*>(command_raw)));
                }
                case 211: {
                    char ch = get_input();

                    if (ch == 'c') return make_unique<Line_Replace_Edit_Command>(multiplier, editor);

                    Abstract_Command* command_raw = take_command(string(1, ch)).release();
                    if (!dynamic_cast<Abstract_Move_Command*>(command_raw)) throw std::invalid_argument("");
                    return make_unique<Direction_Motion_Replace_Edit_Command>(multiplier, editor,
                        unique_ptr<Abstract_Move_Command>(dynamic_cast<Abstract_Move_Command*>(command_raw)));
                }
                case 212: { return make_unique<Paste_After_Cursor_Insert_Edit_Command>(multiplier, editor); }
                case 213: { return make_unique<Paste_Before_Cursor_Insert_Edit_Command>(multiplier, editor); }
                case 300: { return make_unique<Undo_Command>(editor); }
                case 301: {
                    char ch = get_input();

                    if (ch == 'y') return make_unique<Copy_Command>(editor);
                    /*
                    Abstract_Command* command_raw = take_command(string(1, ch)).release();
                    if (!dynamic_cast<Abstract_Move_Command*>(command_raw)) throw std::invalid_argument("");
                    return make_unique<Copy>(multiplier, editor,
                        unique_ptr<Abstract_Move_Command>(dynamic_cast<Abstract_Move_Command*>(command_raw)));
                    */
                }
                case 400: { return make_unique<Force_Quit_Colon_Command>(editor); }
                case 401: { return make_unique<Save_Colon_Command>(editor); }
                case 402: { return make_unique<Safe_Quit_Colon_Command>(editor); }
                case 403: { return make_unique<Save_Quit_Colon_Command>(editor); }
                case 404: { return make_unique<End_File_Colon_Command>(editor); }
                case 500: { return make_unique<Full_Scroll_Up_Control_Command>(multiplier, editor); }
                case 501: { return make_unique<Half_Scroll_Down_Control_Command>(multiplier, editor); }
                case 502: { return make_unique<Full_Scroll_Down_Control_Command>(multiplier, editor); }
                default: throw std::invalid_argument("");   
            }
        } else {
            try {
                digit = std::stoi(buffer);
                multiplier = 10 * multiplier + digit;
            } catch (std::invalid_argument const &e) {}
            if (multiplier > 1) {
                editor.get_display().set_command_string(std::to_string(multiplier));
                editor.refresh_display();
            }
            if (!command_trie.contains_prefix(buffer)) buffer = "";
        }
    }
    
}
