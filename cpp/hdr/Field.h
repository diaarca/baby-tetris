#ifndef TETRIS_FIELD_H
#define TETRIS_FIELD_H

#include "Point.h"
#include "Tromino.h"
#include <memory>
#include <vector>

class Field
{
  private:
    int width;
    int height;
    std::vector<std::vector<bool>> grid;

  public:
    Field(int w, int h);
    ~Field();
    int getWidth();
    int getHeight();
    bool isAvailable(int line, int column);
    bool isAvailable(const Tromino& t, int line, int column, int rotation);
    bool addTromino(const Tromino& t, int line, int column, int rotation);
    std::vector<Point> getEmptyPositions();
    Field clone();
};

#endif // TETRIS_FIELD_H
