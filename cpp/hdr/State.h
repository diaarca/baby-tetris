#pragma once

#include "Action.h"
#include "Field.h"
#include "Tromino.h"
#include <array>
#include <memory>
#include <vector>
#include <iostream>

class State
{
  private:
    Field field_;
    std::unique_ptr<Tromino> nextTromino_;
    std::vector<Point> placementPositions();

  public:
    State(Field& field, std::unique_ptr<Tromino> nextTromino)
        : field_(field), nextTromino_(std::move(nextTromino)) {};

    Field getField() const { return field_; }
    Tromino& getNextTromino() const { return *nextTromino_; }
    std::vector<Action> getAvailableActions();
    State applyAction(Action& action);
    int evaluate(std::array<int, 3>& config);
    friend std::ostream& operator<<(std::ostream& os, const State& s);
};
