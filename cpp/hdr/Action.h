#ifndef TETRIS_ACTION_H
#define TETRIS_ACTION_H

#include "Point.h"

class Action
{
  private:
    Point position;
    int rotation;

  public:
    Action(Point p, int r);
    Point getPosition();
    int getRotation();
};

#endif // TETRIS_ACTION_H
