#include "Field.h"

bool Field::isAvailable(int line, int column)
{
    if (line < 0 || line >= height_)
        return false;
    if (column < 0 || column >= width_)
        return false;
    return !grid_[line][column];
}

bool Field::isAvailable(const Tromino& t, int line, int column, int rotation)
{
    std::vector<Offset> offsets = t.getOffsets(rotation);
    for (Offset& off : offsets)
    {
        int l = line + off[0];
        int c = column + off[1];
        if (!isAvailable(l, c))
            return false;
    }
    return true;
}

bool Field::addTromino(const Tromino& t, int line, int column, int rotation)
{
    if (!isAvailable(t, line, column, rotation))
        return false;
    std::vector<Offset> offsets = t.getOffsets(rotation);
    for (Offset& off : offsets)
    {
        int l = line + off[0];
        int c = column + off[1];
        grid_[l][c] = true;
    }
    return true;
}

std::vector<Point> Field::getEmptyPositions()
{
    std::vector<Point> positions;
    for (int l = 0; l < height_; ++l)
    {
        for (int c = 0; c < width_; ++c)
        {
            if (!grid_[l][c])
                positions.emplace_back(l, c);
        }
    }
    return positions;
}

Field Field::clone()
{
    Field clone(grid_);
    return clone;
}

std::ostream& operator<<(std::ostream& os, const Field& f)
{

    for (int l = 0; l < f.getHeight(); ++l)
    {
        for (int c = 0; c < f.getWidth(); ++c)
        {
            os << (f.getGrid()[l][c] ? '*' : '.');
        }
        os << '\n';
    }
    return os;
}
