#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include "Field.h"
#include "State.h"
#include "Tromino.h"

struct Game {
    State state;
    int score;
    Game(const std::array<int,5> &config, int height, int width)
        : state(Field(width, height), std::make_unique<IPiece>()), score(0) {
        // choose first tromino randomly
        if ((rand() / (double)RAND_MAX) < 0.5) {
            state = State(Field(width, height), std::make_unique<IPiece>());
        } else {
            state = State(Field(width, height), std::make_unique<LPiece>());
        }
    }
};

#endif // TETRIS_GAME_H
