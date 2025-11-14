#ifndef TETRIS_GAME_H
#define TETRIS_GAME_H

#include "Field.h"
#include "State.h"
#include "Tromino.h"
#include <iostream>
#include <cmath>

struct Game {
    State state;
    int score;
    std::array<int, 5> config;

    Game(const std::array<int,5> &config, int height, int width)
        : config(config), state(Field(width, height), std::make_unique<IPiece>()), score(0) {
        // choose first tromino randomly
        if ((rand() / (double)RAND_MAX) < 0.5) {
            state = State(Field(width, height), std::make_unique<IPiece>());
        } else {
            state = State(Field(width, height), std::make_unique<LPiece>());
        }
    }

    private:
        void printField() {
            Field field = state.getField();
            for (int l = 0; l < field.getHeight(); ++l) {
                for (int c = 0; c < field.getWidth(); ++c) {
                    std::cout << (field.getGrid()[l][c] ? '*' : '.');
                }
                std::cout << '\n';
            }
        }
            

    public:
        void play() {
            // Game loop
            int lines = 0;
            printField();
            std::cout << "Starting game...\n";
            while (state.getAvailableActions().size() > 0) {
                auto actions = state.getAvailableActions();
                if (! actions.empty()) {
                    // first action for now
                    state = state.applyAction(actions[0]);
                    score = state.evaluate(config);
                    lines = completeLine();
                    if (lines > 0) {
                        score += pow(lines, 2); // for now; after pass the lines to eval
                    }
                    printField();
                    std::cout << "Current score: " << score << "\n";
                }
            }
            std::cout << "Game Over! Completed lines: " <<  lines <<  "\n";
        }
        //returns number of completed lines
        int completeLine() {
            Field field = state.getField();
            int removeCount = 0;
            //check if one line is full
            for (int r = 0; r < field.getHeight(); ++r) {
                bool isComplete = true;
                for (int c = 0; c < field.getWidth(); ++c) {
                    if (!field.getGrid()[r][c]) { isComplete = false; break; }
                }
                if (isComplete) {
                    printField();
                    std::cout << "Completed line: " << r << "\n";
                    //remove line
                    for(int c = 0; c < field.getWidth(); ++c) {
                        field.grid[r][c] = false;
                    }
                    removeCount++;
                    //move all lines above down
                    for(int row = r; row > 0; --row) {
                        for(int c = 0; c < field.getWidth(); ++c) {
                            field.grid[row][c] = field.grid[row-1][c];
                        }
                    }
                    //clear top line
                    for(int c = 0; c < field.getWidth(); ++c) {
                        field.grid[0][c] = false;
                    }
                }
            }
            //the state after all line removals
            if (removeCount)
            {
                state = State(field, std::move(state.nextTromino));
            }
            return removeCount;
        }

};


#endif // TETRIS_GAME_H
