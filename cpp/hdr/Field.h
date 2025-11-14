#ifndef TETRIS_FIELD_H
#define TETRIS_FIELD_H

#include <vector>
#include <memory>
#include "Tromino.h"
#include "Point.h"

struct Field {
    int width;
    int height;
    std::vector<std::vector<bool>> grid;

    Field(int width_, int height_) : width(width_), height(height_) {
        grid.assign(height, std::vector<bool>(width, false));
    }

    const std::vector<std::vector<bool>>& getGrid() const { return grid; }
    int getWidth() const { return width; }
    int getHeight() const { return height; }

    bool isAvailable(int line, int column) const {
        if (line < 0 || line >= height) return false;
        if (column < 0 || column >= width) return false;
        return !grid[line][column];
    }

    bool isAvailable(const Tromino &t, int line, int column, int rotation) const {
        auto offsets = t.getOffsets(rotation);
        for (auto &off : offsets) {
            int l = line + off[0];
            int c = column + off[1];
            if (!isAvailable(l, c)) return false;
        }
        return true;
    }

    bool addTromino(const Tromino &t, int line, int column, int rotation) {
        if (!isAvailable(t, line, column, rotation)) return false;
        auto offsets = t.getOffsets(rotation);
        for (auto &off : offsets) {
            int l = line + off[0];
            int c = column + off[1];
            grid[l][c] = true;
        }
        return true;
    }

    std::vector<Point> getEmptyPositions() const {
        std::vector<Point> positions;
        for (int l = 0; l < height; ++l) {
            for (int c = 0; c < width; ++c) {
                if (!grid[l][c]) positions.emplace_back(l, c);
            }
        }
        return positions;
    }

    Field clone() const {
        Field f(width, height);
        f.grid = grid;
        return f;
    }
};

#endif // TETRIS_FIELD_H
