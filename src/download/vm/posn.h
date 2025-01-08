#ifndef POSN_H
#define POSN_H

#include <iostream>

class Posn {
public:
    int row;
    int col;
    Posn();
    Posn(const int row, const int col);
    Posn operator+(const Posn& other) const;
    Posn operator-(const Posn& other) const;
    Posn& operator+=(const Posn& other);
    Posn& operator-=(const Posn& other);
    bool operator==(const Posn& other) const;
    bool operator<(const Posn& other) const;
    bool operator>(const Posn& other) const;
    bool operator<=(const Posn& other) const;
    bool operator>=(const Posn& other) const;
};







#endif
