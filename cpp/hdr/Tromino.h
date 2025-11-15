#pragma once

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
    int rotationCount() const override { return 2; };
    std::vector<Offset> getOffsets(int rotation) const override;
};

class LPiece : public Tromino
{
  public:
    int rotationCount() const override { return 4; };
    std::vector<Offset> getOffsets(int rotation) const override;
};
