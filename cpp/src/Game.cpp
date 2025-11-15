#include "../hdr/Game.h"



void Game:: printField() {
    Field field = state.getField();
    for (int l = 0; l < field.getHeight(); ++l) {
        for (int c = 0; c < field.getWidth(); ++c) {
            std::cout << (field.getGrid()[l][c] ? '*' : '.');
        }
        std::cout << '\n';
    }
}   
void Game:: play() {
        // Game loop
        int lines = 0;
        std::cout << "Initial Field:\n";    
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
int Game::completeLine() {
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
                field.getGrid()[r][c] = false;
            }
            removeCount++;
            //move all lines above down
            for(int row = r; row > 0; --row) {
                for(int c = 0; c < field.getWidth(); ++c) {
                    field.getGrid()[row][c] = field.getGrid()[row-1][c];
                }
            }
            //clear top line
            for(int c = 0; c < field.getWidth(); ++c) {
                field.getGrid()[0][c] = false;
            }
        }
    }
    //the state after all line removals
    if (removeCount)
    {
        std::unique_ptr<Tromino> newTromino;
        if (dynamic_cast<IPiece*>(&state.getNextTromino())) {
            newTromino = std::make_unique<IPiece>();
        } else if (dynamic_cast<LPiece*>(&state.getNextTromino())) {
            newTromino = std::make_unique<LPiece>();
        }
        state = State(field, std::move(newTromino));
        // state = State(field, std::make_unique<Tromino>(state.getNextTromino()));
    }
    return removeCount;
}
