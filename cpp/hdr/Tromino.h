#pragma once

#include <array>
#include <ostream>
#include <vector>

using Offset = std::array<int, 2>;

class Tromino
{
  public:
    virtual ~Tromino() = default;
    virtual std::vector<Offset> getOffsets(int rotation) const = 0;
    virtual int rotationCount() const { return 4; }
    virtual void print(std::ostream& os) const = 0; // pure virtual print method
    virtual bool isIPiece() const { return false; }
    virtual bool isLPiece() const { return false; }
};

// Declare operator<< for Tromino
std::ostream& operator<<(std::ostream& os, const Tromino& piece);

class IPiece : public Tromino
{
  public:
    int rotationCount() const override { return 2; };
    std::vector<Offset> getOffsets(int rotation) const override;
    void print(std::ostream& os) const override;
    bool isIPiece() const override { return true; }
};

class LPiece : public Tromino
{
  public:
    int rotationCount() const override { return 4; };
    std::vector<Offset> getOffsets(int rotation) const override;
    void print(std::ostream& os) const override;
    bool isLPiece() const override { return true; }
};
