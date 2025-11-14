#ifndef TETRIS_POINT_H
#define TETRIS_POINT_H

#include <ostream>

struct Point {
    int x;
    int y;
    Point() : x(0), y(0) {}
    Point(int x_, int y_) : x(x_), y(y_) {}
    int getX() const { return x; }
    void setX(int v) { x = v; }
    int getY() const { return y; }
    void setY(int v) { y = v; }
    bool operator==(const Point &o) const { return x == o.x && y == o.y; }
};

inline std::ostream &operator<<(std::ostream &os, const Point &p) {
    os << "Point{" << "x=" << p.x << ", y=" << p.y << "}";
    return os;
}

#endif // TETRIS_POINT_H
