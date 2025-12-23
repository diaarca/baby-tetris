#include "Tromino.h"

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

// Override print for IPiece
void IPiece::print(std::ostream& os) const { os << "IPiece"; }

// Override print for LPiece
void LPiece::print(std::ostream& os) const { os << "LPiece"; }

// Implement clone for pieces
std::unique_ptr<Tromino> IPiece::clone() const { return std::make_unique<IPiece>(*this); }
std::unique_ptr<Tromino> LPiece::clone() const { return std::make_unique<LPiece>(*this); }

// Implement operator<< for Tromino
std::ostream& operator<<(std::ostream& os, const Tromino& piece)
{
    piece.print(os);
    return os;
}
