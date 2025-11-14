#ifndef TETRIS_TROMINO_H
#define TETRIS_TROMINO_H

#include <vector>
#include <array>

using Offset = std::array<int,2>;

struct Tromino {
    virtual ~Tromino() = default;
    virtual std::vector<Offset> getOffsets(int rotation) const = 0;
    virtual int rotationCount() const { return 4; }
};

struct IPiece : Tromino {
    int rotationCount() const override { return 2; }
    std::vector<Offset> getOffsets(int rotation) const override {
        int r = ((rotation % 2) + 2) % 2;
        if (r == 0) {
            return {{0,0},{0,1},{0,2}}; // horizontal
        } else {
            return {{0,0},{1,0},{2,0}}; // vertical
        }
    }
};

struct LPiece : Tromino {
    int rotationCount() const override { return 4; }
    std::vector<Offset> getOffsets(int rotation) const override {
        int r = ((rotation % 4) + 4) % 4;
        switch (r) {
        case 0: return {{0,1},{1,0},{1,1}}; // missing top-left
        case 1: return {{0,0},{1,0},{1,1}}; // missing top-right
        case 2: return {{0,0},{0,1},{1,0}}; // missing bottom-right
        default: return {{0,0},{0,1},{1,1}}; // missing bottom-left
        }
    }
};

#endif // TETRIS_TROMINO_H
