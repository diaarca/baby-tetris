#pragma once

#include "Point.h"
#include "Tromino.h"
#include <memory>
#include <vector>
#include <ostream>

class Field
{
  private:
    int width_;
    int height_;
    std::vector<std::vector<bool>> grid_;

  public:
    // constructors
    Field(int width, int height) : width_(width), height_(height)
    {
        grid_.assign(height_, std::vector<bool>(width_, false));
    };
    Field(std::vector<std::vector<bool>> g)
        : width_(g[0].size()), height_(g.size()), grid_(g) {};

    // getters
    int getWidth() const { return width_; };
    int getHeight() const { return height_; };
    std::vector<std::vector<bool>> getGrid() const { return grid_; };

    // other methods
    bool isAvailable(int line, int column);
    bool isAvailable(const Tromino& t, int line, int column, int rotation);
    bool addTromino(const Tromino& t, int line, int column, int rotation);
    void setGrid(const std::vector<std::vector<bool>>& g) { grid_ = g; }
    std::vector<Point> getEmptyPositions();
    Field clone();
    friend std::ostream& operator<<(std::ostream& os, const Field& f);
};
