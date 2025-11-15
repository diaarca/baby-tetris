#include "Field.h"

Field::Field(int w, int h) : width(w), height(h)
{
    grid.assign(height, std::vector<bool>(width, false));
}

int Field::getWidth() { return width; }
int Field::getHeight() { return height; }
std::vector<std::vector<bool>> Field::getGrid() { return grid; }

bool Field::isAvailable(int line, int column)
{
    if (line < 0 || line >= height)
        return false;
    if (column < 0 || column >= width)
        return false;
    return !grid[line][column];
}

bool Field::isAvailable(const Tromino& t, int line, int column, int rotation)
{
    auto offsets = t.getOffsets(rotation);
    for (auto& off : offsets)
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
    auto offsets = t.getOffsets(rotation);
    for (auto& off : offsets)
    {
        int l = line + off[0];
        int c = column + off[1];
        grid[l][c] = true;
    }
    return true;
}

std::vector<Point> Field::getEmptyPositions()
{
    std::vector<Point> positions;
    for (int l = 0; l < height; ++l)
    {
        for (int c = 0; c < width; ++c)
        {
            if (!grid[l][c])
                positions.emplace_back(l, c);
        }
    }
    return positions;
}
