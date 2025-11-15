#pragma once

#include "Point.h"

class Action
{
  private:
    Point position_;
    int rotation_;

  public:
    Action(Point position, int rotation)
        : position_(position), rotation_(rotation) {};

    Point getPosition() const { return position_; };
    int getRotation() const { return rotation_; };
};
