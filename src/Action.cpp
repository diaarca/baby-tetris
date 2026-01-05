#include "Action.h"

Action Action::clone() const
{
    return Action(position_, rotation_);
}

std::ostream& operator<<(std::ostream& os, const Action& a)
{
    os << a.getPosition() << " @rot=" << a.getRotation();
    return os;
}

bool operator!=(const Action& a, const Action& b)
{
    return !(a.getPosition() == b.getPosition() &&
             a.getRotation() == b.getRotation());
}
