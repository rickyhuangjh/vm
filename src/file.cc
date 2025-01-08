#include <algorithm>
#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>
#include "file.h"

using std::string;
using std::to_string;

File::File(string file_name): file_name{file_name}, edit_mode_engaged{false}, num_lines{0} {
    std::ifstream f(file_name, std::ios::in);
    string line;
    if (f.is_open()) {
        while (std::getline(f, line)) {
            line.push_back('\n');
            the_file.push_back(line);
            ++num_lines;
        }
    }
}

bool File::save() {
    std::ofstream f(file_name, std::ios::trunc | std::ios::out);
    if (!f.is_open()) {
       return false;
    }
    for (const auto &line : the_file) {
        f << line; // Strings already contain newlines
    }
    return true;
}

string File::get_file_name() const {
    return file_name;
}

bool File::is_in_bounds(Posn posn) const {
    const int row_idx = posn.row;
    const int col_idx = posn.col;
    const int line_length = the_file[row_idx].length();
    if (row_idx < 0) return false;
    if (row_idx >= num_lines) return false;
    if (col_idx < 0) return false;
    if (col_idx >= line_length) return false;
    return true;
}

Posn File::bring_in_bounds(Posn posn) const {
    int in_bounds_row_idx = posn.row;
    int in_bounds_col_idx = posn.col;
    if (in_bounds_row_idx < 0) in_bounds_row_idx = 0;
    else if (in_bounds_row_idx >= num_lines) in_bounds_row_idx = num_lines - 1;

    int line_length = (*this)[in_bounds_row_idx].length();
    if (in_bounds_col_idx < 0) in_bounds_col_idx = 0;
    else if (in_bounds_col_idx >= line_length) in_bounds_col_idx = line_length - 1;
    return Posn(in_bounds_row_idx, in_bounds_col_idx);
}

void File::insert_char(const Posn& posn, const char c) {
    if (!is_in_bounds(posn)) {
        string message = "Insert char - Out of bounds at (" + to_string(posn.row) + "," + to_string(posn.col) + ")";
        throw std::invalid_argument(message);
    }
    the_file[posn.row].insert(posn.col, 1, c);
}

void File::insert_str(const Posn& posn, const string& s) {
    if (!is_in_bounds(posn)) throw std::invalid_argument("Out of bounds");
    the_file[posn.row].insert(posn.col, s);
}

void File::delete_char(const Posn& posn) {
    if (!is_in_bounds(posn)) {
        string message = "Delete char - Out of bounds at (" + to_string(posn.row) + "," + to_string(posn.col) + ")";
        throw std::invalid_argument(message);
    }
    if (num_lines == 1 && the_file[0][0] == '\n') return;
    the_file[posn.row].erase(posn.col, 1);
}

void File::delete_str(const Posn& start_posn, const size_t delete_length) {
    if (!is_in_bounds(start_posn) ||  !is_in_bounds(start_posn + Posn{0, delete_length})) {
        throw std::invalid_argument("Out of bounds");
    }
    the_file[start_posn.row].erase(start_posn.col, delete_length);
}

void File::create_new_line(const int new_line_idx) {
    if (new_line_idx < 0 || new_line_idx > get_num_lines()) throw std::invalid_argument("Out of bounds");
    the_file.insert(the_file.begin() + new_line_idx, "\n");
    ++num_lines;
}

void File::delete_line(const int delete_line_idx) {
    if (delete_line_idx < 0 || delete_line_idx >= get_num_lines()) throw std::invalid_argument("Out of bounds");
    if (num_lines == 1) return;
    the_file.erase(the_file.begin() + delete_line_idx);
    --num_lines;
}

string& File::operator[](const int line_idx) {
    if (line_idx >= num_lines) {
        string message = "Line access - Out of bounds at line " + to_string(line_idx);
        throw std::invalid_argument(message);
    }
    return the_file[line_idx];
}

const string& File::operator[](const int line_idx) const {
    if (line_idx >= num_lines) {
        string message = "Line access - Out of bounds at line " + to_string(line_idx);
        throw std::invalid_argument(message);
    }
    return the_file[line_idx];
}

int File::get_edit_mode() const { return edit_mode_engaged; }

void File::set_edit_mode(const int mode) { edit_mode_engaged = mode; }

int File::get_num_lines() const { return num_lines; }

File::iterator::iterator(File& file, size_t line_idx, size_t char_idx):
    file{file}, line_idx{line_idx}, char_idx{char_idx} {}


File::iterator& File::iterator::operator++() {
    if (char_idx == file[line_idx].length() - 1) {
        ++line_idx;
        char_idx = 0;
    } else ++char_idx;
    return *this;
}

bool File::iterator::operator!=(const iterator& other) {
    return &file != &other.file || line_idx != other.line_idx || char_idx != other.char_idx;
}

char File::iterator::operator*() { return file[line_idx][char_idx]; }


File::iterator File::begin() { return iterator(*this, 0, 0); }

File::iterator File::end() { return iterator(*this, num_lines, 0); }

File::const_iterator::const_iterator(const File& file, size_t line_idx, size_t char_idx):
    file{file}, line_idx{line_idx}, char_idx{char_idx} {}


File::const_iterator& File::const_iterator::operator++() {
    if (char_idx == file[line_idx].length() - 1) {
        ++line_idx;
        char_idx = 0;
    } else ++char_idx;
    return *this;
}

bool File::const_iterator::operator!=(const const_iterator& other) {
    return &file != &other.file || line_idx != other.line_idx || char_idx != other.char_idx;
}

char File::const_iterator::operator*() { return file[line_idx][char_idx]; }

File::const_iterator File::begin() const { return const_iterator(*this, 0, 0); }

File::const_iterator File::end() const { return const_iterator(*this, num_lines, 0); }


