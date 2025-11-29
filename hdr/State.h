#pragma once

#include "Action.h"
#include "Field.h"
#include "Tromino.h"
#include <array>
#include <iostream>
#include <memory>
#include <vector>

class State
{
  private:
    Field field_;
    std::unique_ptr<Tromino> nextTromino_;

    std::vector<Point> placementPositions();

  public:
    // Take Field by value so callers can pass temporaries (e.g. clones).
    State(Field field, std::unique_ptr<Tromino> nextTromino)
      : field_(std::move(field)), nextTromino_(std::move(nextTromino)) {};

    // Return a deep copy / clone of this State. The returned State has its
    // own Field and a newly-constructed next tromino of the same runtime
    // type (IPiece or LPiece).
    State clone() const;

    Field getField() const { return field_; }
    // non-const getter to allow modifying the field of a State
    Field& getField() { return field_; }
    Tromino& getNextTromino() const { return *nextTromino_; }
    std::vector<Action> getAvailableActions();
    State applyAction(Action& action);
    std::vector<State> genAllStatesFromAction(Action& action);
    int evaluate(std::array<int, 3>& config);
    State completeLines();
    State clone();
    friend std::ostream& operator<<(std::ostream& os, const State& s);
};
