#ifndef TETRIS_POINT_H
#define TETRIS_POINT_H

#include <ostream>

class Point
{
  private:
    int x;
    int y;

  public:
    Point(int x_, int y_);
    int getX() const;
    int getY() const;
    bool operator==(const Point& o);
    std::ostream& operator<<(std::ostream& os);
};

#endif // TETRIS_POINT_H
