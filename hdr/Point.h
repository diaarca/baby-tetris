#pragma once
#include <ostream>

class Point
{
  private:
    int x_;
    int y_;

  public:
    Point(int x, int y) : x_(x), y_(y) {};

    int getX() const { return x_; };
    int getY() const { return y_; };

    bool operator==(const Point& o);
    friend std::ostream& operator<<(std::ostream& os, const Point& p);
};
