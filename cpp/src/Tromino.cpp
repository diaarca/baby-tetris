#include "Tromino.h"

int IPiece::rotationCount() const { return 2; }

std::vector<Offset> IPiece::getOffsets(int rotation) const
{
    int r = ((rotation % 2) + 2) % 2;
    if (r == 0)
    {
        return {{0, 0}, {0, 1}, {0, 2}}; // horizontal
    }
    else
    {
        return {{0, 0}, {1, 0}, {2, 0}}; // vertical
    }
}

int LPiece::rotationCount() const { return 4; }

std::vector<Offset> LPiece::getOffsets(int rotation) const
{
    int r = ((rotation % 4) + 4) % 4;
    switch (r)
    {
    case 0:
        return {{0, 1}, {1, 0}, {1, 1}}; // missing top-left
    case 1:
        return {{0, 0}, {1, 0}, {1, 1}}; // missing top-right
    case 2:
        return {{0, 0}, {0, 1}, {1, 0}}; // missing bottom-right
    default:
        return {{0, 0}, {0, 1}, {1, 1}}; // missing bottom-left
    }
}
