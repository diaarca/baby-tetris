#pragma once

#include "Action.h"
#include "Field.h"
#include "Tromino.h"
#include <algorithm>
#include <array>
#include <functional>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#define PROBA_I_PIECE 0.5

class State
{
  private:
    Field field_;
    std::unique_ptr<Tromino> nextTromino_;

    std::vector<Point> placementPositions() const;

  public:
    State(Field field, std::unique_ptr<Tromino> nextTromino)
        : field_(std::move(field)), nextTromino_(std::move(nextTromino)) {};

    State(const State&) = delete;
    State& operator=(const State&) = delete;
    State(State&&) = default;
    State& operator=(State&&) = default;

    State clone() const;

    void setNextTromino(const Tromino& t) { nextTromino_ = t.clone(); };
    Field& getField() { return field_; }
    const Field& getField() const { return field_; }
    const Tromino& getNextTromino() const { return *nextTromino_; }

    std::vector<Action> getAvailableActions() const;

    State applyAction(Action& action);
    State applyAction(const Action& action) const;

    State applyActionTromino(Action action, const Tromino& t);

    std::vector<State> genAllStatesFromAction(Action& action);
    std::vector<State> genAllStatesFromAction(const Action& action) const;

    int evaluate(const std::array<int, 3>& config) const;
    State completeLines() const;
    int gapCheck() const;

    bool operator==(const State& other) const;
    size_t hash() const;

    friend std::ostream& operator<<(std::ostream& os, const State& s);
};

namespace std
{
template <> struct hash<State>
{
    size_t operator()(const State& s) const { return s.hash(); }
};
} // namespace std
