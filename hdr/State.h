#pragma once

#include "Action.h"
#include "Field.h"
#include "Tromino.h"
#include <array>
#include <iostream>
#include <memory>
#include <vector>
#include <algorithm>

#define PROBA_I_PIECE 0.5

class State
{
  private:
    Field field_;
    std::unique_ptr<Tromino> nextTromino_;

    std::vector<Point> placementPositions();

  public:
    State(Field field, std::unique_ptr<Tromino> nextTromino)
        : field_(std::move(field)), nextTromino_(std::move(nextTromino)) {};

    State clone() const;
    State clone();

    Field& getField() { return field_; }
    Tromino& getNextTromino() const { return *nextTromino_; }

    std::vector<Action> getAvailableActions();

    State applyAction(Action& action);
    std::vector<State> genAllStatesFromAction(Action& action);

    int evaluate(std::array<int, 3>& config);
    State completeLines();

    friend std::ostream& operator<<(std::ostream& os, State& s);
};
