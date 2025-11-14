#ifndef TETRIS_STATE_H
#define TETRIS_STATE_H

#include <vector>
#include <memory>
#include "Field.h"
#include "Action.h"
#include "Tromino.h"

struct State {
    Field field;
    std::unique_ptr<Tromino> nextTromino;

    State(const Field &f, std::unique_ptr<Tromino> next)
        : field(f), nextTromino(std::move(next)) {}

    Field getField() const { return field; }
    const Tromino& getNextTromino() const { return *nextTromino; }

private:
    std::vector<Point> placementPositions() const {
        std::vector<Point> positions;
        const auto &grid = field.getGrid();
        int cols = field.getWidth();
        int rows = field.getHeight();
        for (int c = 0; c < cols; ++c) {
            for (int l = 0; l < rows; ++l) {
                if (grid[l][c]) {
                    if (l - 1 >= 0) positions.emplace_back(l - 1, c);
                    break;
                }
                if (l == rows - 1) positions.emplace_back(l, c);
            }
        }
        return positions;
    }

public:
    std::vector<Action> getAvailableActions() const {
        std::vector<Action> possibleActions;
        auto placementPosition = placementPositions();
        auto emptyPositions = field.getEmptyPositions();
        for (const auto &p : emptyPositions) {
            for (int r = 0; r < nextTromino->rotationCount(); ++r) {
                if (field.isAvailable(*nextTromino, p.getX(), p.getY(), r)) {
                    auto offsets = nextTromino->getOffsets(r);
                    for (auto &off : offsets) {
                        Point candidate(p.getX() + off[0], p.getY() + off[1]);
                        for (auto &pp : placementPosition) {
                            if (pp == candidate) {
                                possibleActions.emplace_back(p, r);
                                goto next_position; // break out early
                            }
                        }
                    }
                }
            }
        next_position: ;
        }
        return possibleActions;
    }

    State applyAction(const Action &action) const {
        Field newField = field.clone();
        newField.addTromino(*nextTromino, action.getPosition().getX(),
                            action.getPosition().getY(), action.getRotation());
        // choose new random tromino
        std::unique_ptr<Tromino> newNext;
        if ((rand() / (double)RAND_MAX) < 0.5) newNext = std::make_unique<IPiece>();
        else newNext = std::make_unique<LPiece>();
        return State(newField, std::move(newNext));
    }

    int evaluate(const std::array<int,5> &config) const {
        int completedLines = 0;
        int gapNumber = 0;
        const auto &grid = field.getGrid();
        int rows = field.getHeight();
        int cols = field.getWidth();

        // Count completed lines
        for (int r = 0; r < rows; ++r) {
            bool isComplete = true;
            for (int c = 0; c < cols; ++c) {
                if (!grid[r][c]) { isComplete = false; break; }
            }
            if (isComplete) completedLines++;
        }

        // Count gaps
        for (int c = 0; c < cols; ++c) {
            bool foundFilled = false;
            for (int r = 0; r < rows; ++r) {
                if (grid[r][c]) foundFilled = true;
                else if (foundFilled) gapNumber++;
            }
        }

        int score = 0;
        switch (completedLines) {
        case 1: score += config[0]; break;
        case 2: score += config[1]; break;
        case 3: score += config[2]; break;
        default: break;
        }

        switch (gapNumber) {
        case 0: break;
        case 1: score += config[3]; break;
        case 2: score += config[4]; break;
        default: score += config[4]; break;
        }
        return score;
    }
};

#endif // TETRIS_STATE_H
