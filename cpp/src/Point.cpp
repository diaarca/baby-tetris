#include "Point.h"

Point::Point(int x_, int y_) : x(x_), y(y_) {}

int Point::getX() const
{
    return x;
}

int Point::getY() const
{
    return y;
}

bool Point::operator==(const Point& p)
{
    return x == p.getX() && y == p.getY();
}

std::ostream& Point::operator<<(std::ostream& os)
{
    os << "Point{" << "x=" << x << ", y=" << y << "}";
    return os;
}
