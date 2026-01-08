#include "Field.h"

bool Field::isAvailable(int line, int column) const
{
    if (line < 0 || line >= height_)
        return false;
    if (column < 0 || column >= width_)
        return false;
    return !grid_[line][column];
}

bool Field::isAvailable(const Tromino& t, int line, int column, int rotation) const
{
    std::vector<Offset> offsets = t.getOffsets(rotation);
    for (const Offset& off : offsets)
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
    for (const Offset& off : offsets)
    {
        int l = line + off[0];
        int c = column + off[1];
        grid_[l][c] = true;
    }
    return true;
}

std::vector<Point> Field::getEmptyPositions() const
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

Field Field::clone() const
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

std::vector<int> Field::getColumnHeights() const
{
    std::vector<int> heights(width_, 0);
    for (int c = 0; c < width_; ++c)
    {
        for (int r = 0; r < height_; ++r)
        {
            if (grid_[r][c])
            {
                heights[c] = height_ - r;
                break;
            }
        }
    }
    return heights;
}

int Field::getAggregateHeight() const
{
    std::vector<int> heights = getColumnHeights();
    int totalHeight = 0;
    for (int h : heights)
    {
        totalHeight += h;
    }
    return totalHeight;
}

int Field::getCompleteLines() const
{
    int lines = 0;
    for (int r = 0; r < height_; ++r)
    {
        bool isComplete = true;
        for (int c = 0; c < width_; ++c)
        {
            if (!grid_[r][c])
            {
                isComplete = false;
                break;
            }
        }
        if (isComplete)
        {
            lines++;
        }
    }
    return lines;
}

int Field::getHoles() const
{
    int holes = 0;
    for (int c = 0; c < width_; ++c)
    {
        bool roof = false;
        for (int r = 0; r < height_; ++r)
        {
            if (grid_[r][c])
            {
                roof = true;
            }
            else if (roof)
            {
                holes++;
            }
        }
    }
    return holes;
}

int Field::getBumpiness() const
{
    std::vector<int> heights = getColumnHeights();
    int bumpiness = 0;
    for (int c = 0; c < width_ - 1; ++c)
    {
        bumpiness += std::abs(heights[c] - heights[c + 1]);
    }
    return bumpiness;
}
