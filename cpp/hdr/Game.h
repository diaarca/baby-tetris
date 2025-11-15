#include "Field.h"
#include "State.h"
#include "Tromino.h"
#include <iostream>
#include <cmath>

class Game{
    private:
        State state;
        int score;
        std::array<int, 5> config;
    public:

    // Game(const std::array<int,5> &config, int height, int width)
    //     : config(config), state(Field(width, height), std::make_unique<IPiece>()), score(0) {
    //     // choose first tromino randomly
    //     if ((rand() / (double)RAND_MAX) < 0.5) {
    //         state = State(Field(width, height), std::make_unique<IPiece>());
    //     } else {
    //         state = State(Field(width, height), std::make_unique<LPiece>());
    //     }
    // }

    //     Game(const std::array<int, 5>& config, int height, int width)
    //         : field(width, height),
    //         state(field, std::make_unique<IPiece>()),
    //         config(config), score(0) {
    //             if ((rand() / (double)RAND_MAX) < 0.5) {
    //                 state = State(field, std::make_unique<LPiece>());
    //             }
    //         };

        Game (const std::array<int,5> &config, Field &field)
            :state(field, std::make_unique<IPiece>()),
            config(config), score(0) {
                if ((rand() / (double)RAND_MAX) < 0.5) {
                    state = State(field, std::make_unique<LPiece>());
                }
            };
        void play();
        int completeLine();
        void printField();

};
