#ifndef FILE_H
#define FILE_H

#include <fstream>
#include <string>
#include <stdexcept>
#include <vector>

#include "posn.h"


using std::string;
using std::vector;

class File {
private:
    string file_name;
    vector<string> the_file;
    int edit_mode_engaged;
    int num_lines;
public:
    File(string file_name);
    bool save();
    string get_file_name() const;
    bool is_in_bounds(Posn posn) const;
    Posn bring_in_bounds(Posn posn) const;
    void insert_char(const Posn& posn, const char c);
    void insert_str(const Posn& posn, const string& s);
    void delete_char(const Posn& posn);
    // deletes from start_posn to end_posn inclusive
    void delete_str(const Posn& start_posn, const size_t delete_length);
    void create_new_line(const int new_line_idx);
    void delete_line(const int delete_line_idx);
    int get_num_lines() const;
    string& operator[](const int line_idx);
    const string& operator[](const int line_idx) const;
    int get_edit_mode() const;
    void set_edit_mode(const int mode);

    class iterator {
    private:
        File& file;
        size_t line_idx;
        size_t char_idx;
        iterator(File& file, size_t line_idx, size_t char_idx);
    public:
        iterator& operator++();
        bool operator!=(const iterator& other);
        char operator*();
        friend class File;
    };
    class const_iterator {
    private:
        const File& file;
        size_t line_idx;
        size_t char_idx;
        const_iterator(const File& file, size_t line_idx, size_t char_idx);
    public:
        const_iterator& operator++();
        bool operator!=(const const_iterator& other);
        char operator*();
        friend class File;
    };

    iterator begin();
    iterator end();
    const_iterator begin() const;
    const_iterator end() const;
};

#endif
