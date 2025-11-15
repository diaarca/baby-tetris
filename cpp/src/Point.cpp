#include "Point.h"

bool Point::operator==(const Point& p)
{
    return x_ == p.getX() && y_ == p.getY();
}

std::ostream& Point::operator<<(std::ostream& os)
{
    os << "Point{" << "x=" << x_ << ", y=" << y_ << "}";
    return os;
}
