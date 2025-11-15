#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include "Field.h"
#include "State.h"
#include "Tromino.h"
#include <memory>

// class Game
// {
//   private:
//     State state;
//     int score;
//
//   public:
//     Game(const std::array<int, 5>& config, int height, int width) : score(0)
//     {
//
//         // choose first tromino randomly
//         Tromino* t;
//         if ((rand() / (double)RAND_MAX) < 0.5)
//         {
//             t = std::make_unique<IPiece>();
//         }
//         else
//         {
//             t = std::make_unique<LPiece>();
//         }
//         state(new State(Field(width, height), t));
//     }
// };

#endif // TETRIS_GAME_H
