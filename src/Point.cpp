#include "Point.h"

bool Point::operator==(const Point& p) const
{
    return x_ == p.getX() && y_ == p.getY();
}

std::ostream& operator<<(std::ostream& os, const Point& p)
{
    os << "(" << p.x_ << ", " << p.y_ << ")";
    return os;
}
