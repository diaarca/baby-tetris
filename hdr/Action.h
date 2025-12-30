#pragma once

#include "Point.h"

class Action
{
  private:
    Point position_;
    int rotation_;

  public:
    Action() : position_(-1, -1), rotation_(-1) {};
    Action(Point position, int rotation)
        : position_(position), rotation_(rotation) {};

    Point getPosition() const { return position_; };
    int getRotation() const { return rotation_; };
    Action clone() const;

    friend std::ostream& operator<<(std::ostream& os, const Action& a);
    friend bool operator!=(const Action& a, const Action& b);
};
