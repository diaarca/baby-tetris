#ifndef TETRIS_ACTION_H
#define TETRIS_ACTION_H

#include "Point.h"

struct Action {
    Point position;
    int rotation;
    Action() : position(), rotation(0) {}
    Action(Point p, int r) : position(p), rotation(r) {}
    Point getPosition() const { return position; }
    int getRotation() const { return rotation; }
};

#endif // TETRIS_ACTION_H
