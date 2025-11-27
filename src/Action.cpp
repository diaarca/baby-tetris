#include "Action.h"

#include <ostream>

std::ostream& operator<<(std::ostream& os, const Action& a)
{
	os << a.getPosition() << " @rot=" << a.getRotation();
	return os;
}
