#include "MDP.h"
#include <iostream>

std::vector<Action>
MDP::valueIteration(double eps, int maxIteration, double lambda)
{
    std::vector<State> S = generateAllStates();
    int nbState = S.size();
    std::vector<Action> A(nbState);
    std::vector<int> V(nbState); // value vector (expected value)
    int newVal;

    double delta;
    for (int i = 0; i < maxIteration; i++)
    {
        delta = 0.0;
        std::vector<int> VPrime(nbState); // value vector' (expected value)

        for (int j = 0; j < nbState; j++)
        {
            State& s = S[j];
            for (Action& a : s.getAvailableActions())
            {
                State sPrime = s.applyAction(a);
                // newVal = 1/2 * (sPrime.evaluate() + lambda *
                // V[stateIndex(sPrime)]); newVal += 1/2 * (sPrime.evaluate() +
                // lambda * V[stateIndex(sPrime)]); avoid for proba because 1/2
                // for each Piece for now
                newVal =
                    sPrime.evaluate(config_) + lambda * V[stateIndex(sPrime)];

                if (newVal > VPrime[j])
                {
                    // 2* for the 2 possibles s' from s using a (IPiece, LPiece)
                    VPrime[j] = newVal;
                    A[j] = a;
                }
            }
            delta = (VPrime[j] - V[j]) > delta ? (VPrime[j] - V[j]) : delta;
            // std::cout << "action: " << A[j] << std::endl;
        }
        V = VPrime;

        if (delta < eps)
            break;
    }
    return A;
}

std::vector<State> MDP::generateAllStates() const
{
    int cells = width_ * height_;
    if (cells < 0)
        return {};

    // Guard against shifting by >= 64 on platforms where that is UB.
    // For reasonably small grids (as used in this project) this is fine.
    uint64_t total = 1ULL;
    if (cells >= 64)
    {
        // too many states to enumerate safely with 64-bit mask
        return {};
    }
    total <<= cells; // total = 2^cells

    std::vector<State> states;
    states.reserve(static_cast<size_t>(total) * 2);

    for (uint64_t mask = 0; mask < total; ++mask)
    {
        std::vector<std::vector<bool>> grid(height_,
                                            std::vector<bool>(width_, false));
        for (int r = 0; r < height_; ++r)
        {
            for (int c = 0; c < width_; ++c)
            {
                int idx = r * width_ + c; // bit index for this cell
                grid[r][c] = ((mask >> idx) & 1ULL) != 0ULL;
            }
        }

        Field f(grid);

        // For each possible next piece type, create a State.
        states.emplace_back(f, std::make_unique<IPiece>());
        states.emplace_back(f, std::make_unique<LPiece>());
    }

    return states;
}

size_t MDP::stateIndex(const State& s) const
{
    int cells = width_ * height_;
    if (cells < 0 || cells >= 64)
        return static_cast<size_t>(-1);

    // Build mask the same way as in generateAllStates()
    uint64_t mask = 0ULL;
    std::vector<std::vector<bool>> grid = s.getField().getGrid();
    // Basic sanity checks
    if ((int)grid.size() != height_)
        return static_cast<size_t>(-1);

    for (int r = 0; r < height_; ++r)
    {
        if ((int)grid[r].size() != width_)
            return static_cast<size_t>(-1);
        for (int c = 0; c < width_; ++c)
        {
            int idx = r * width_ + c;
            if (grid[r][c])
                mask |= (1ULL << idx);
        }
    }

    // Determine piece index: 0 for IPiece, 1 for LPiece
    const Tromino* t = &s.getNextTromino();
    size_t pieceIndex = static_cast<size_t>(-1);
    if (dynamic_cast<const IPiece*>(t) != nullptr)
        pieceIndex = 0;
    else if (dynamic_cast<const LPiece*>(t) != nullptr)
        pieceIndex = 1;
    else
        return static_cast<size_t>(-1);

    return static_cast<size_t>(mask) * 2u + pieceIndex;
}

void MDP::playPolicy(Game& game, std::vector<Action> policy)
{
    int lines, i;
    int maxAction = 1000;
    lines = i = 0;
    // std::cout << "Initial State:\n" << game.getState() << std::endl;
    // std::cout << "Starting game...\n";

    while (game.getState().getAvailableActions().size() > 0)
    {
        Action a = policy[stateIndex(game.getState())];

        // std::cout << "Action number " << i << std::endl;
        std::cout << game.getState().getNextTromino();
        game.setState(game.getState().applyAction(a));
        // std::cout << game.getState();

        game.setScore(game.getScore() + game.getState().evaluate(config_));
        if ((lines = game.completeLine()) > 0)
        {
            // std::cout << "Action completed " << lines << " lines\n";
        }
        std::cout << "New board:\n" << game.getState().getField();
        // std::cout << "Current score: " << game.getScore() << "\n\n";
        i++;
        if (i > maxAction)
            break;
    }
    std::cout << game.getState().getField();
    std::cout << "Game Over! Global score: " << game.getScore() << " in " << i
              << " actions \n";
}
