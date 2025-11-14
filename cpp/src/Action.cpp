#include "Action.h"

Action::Action(Point p, int r) : position(p), rotation(r) {}

Point Action::getPosition() { return position; }

int Action::getRotation() { return rotation; }
