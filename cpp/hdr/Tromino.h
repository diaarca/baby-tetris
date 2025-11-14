#ifndef TETRIS_TROMINO_H
#define TETRIS_TROMINO_H

#include <array>
#include <vector>

using Offset = std::array<int, 2>;

class Tromino
{
  public:
    virtual ~Tromino() = default;
    virtual std::vector<Offset> getOffsets(int rotation) const = 0;
    virtual int rotationCount() const { return 4; }
};

class IPiece : public Tromino
{
  public:
    int rotationCount() const override;
    std::vector<Offset> getOffsets(int rotation) const override;
};

class LPiece : public Tromino
{
  public:
    int rotationCount() const override;
    std::vector<Offset> getOffsets(int rotation) const override;
};

#endif // TETRIS_TROMINO_H
