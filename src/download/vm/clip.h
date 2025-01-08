#ifndef CLIP_H
#define CLIP_H

#include <string>

using std::string;

class Clip {
private:
    string the_clip; // should not incluce new_line char
    bool is_line;
public:
    Clip(): the_clip{""}, is_line{false} {}
    Clip(string s, bool is_line): the_clip{s}, is_line{is_line} {}
    const string get_clip() const { return the_clip; }
    void set_clip(string new_clip) { the_clip = new_clip; }
    bool get_is_line() const { return is_line; }
    void set_is_line(bool new_is_line) { is_line = new_is_line; }
};

#endif
