#include <iostream>
#include "posn.h"

Posn::Posn(): row(0), col(0) {}
Posn::Posn(const int row, const int col): row(row), col(col) {}

Posn Posn::operator+(const Posn& other) const { return Posn(row + other.row, col + other.col); }
Posn Posn::operator-(const Posn& other) const { return Posn(row - other.row, col - other.col); }
Posn& Posn::operator+=(const Posn& other) {
    row += other.row;
    col += other.col;
    return *this;
}
Posn& Posn::operator-=(const Posn& other) {
    row -= other.row;
    col -= other.col;
    return *this;
}
bool Posn::operator==(const Posn& other) const { return row == other.row && col == other.col; }

bool Posn::operator<(const Posn& other) const {
    if (row < other.row) return true;
    if (row > other.row) return false;
    if (col < other.col) return true;
    return false;
}

bool Posn::operator>(const Posn& other) const { return *this != other && *this >= other; }

bool Posn::operator<=(const Posn& other) const {
    if (row < other.row) return true;
    if (row > other.row) return false;
    if (col <= other.col) return true;
    return false;
}

bool Posn::operator>=(const Posn& other) const { return !(*this < other); }



std::ostream& operator<<(std::ostream& out, const Posn& pos) {
    out << "(" << pos.row << ", " << pos.col << ")";
    return out;
}
