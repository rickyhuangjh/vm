#ifndef CURSOR_H
#define CURSOR_H

#include "posn.h"

class Cursor {
private:
    int ideal_col_idx;
    Posn actual_posn;
public:
    Cursor(): ideal_col_idx{0}, actual_posn{Posn{0, 0}} {}
    int get_ideal_col_idx() const { return ideal_col_idx; }
    void set_ideal_col_idx(const int new_ideal_col_idx) { ideal_col_idx = new_ideal_col_idx; }
    Posn get_actual_posn() const { return actual_posn; }
    void set_actual_posn(Posn new_actual_posn) { actual_posn = new_actual_posn; }
};

#endif
