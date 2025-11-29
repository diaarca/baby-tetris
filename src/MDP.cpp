#include "MDP.h"

std::vector<Action>
MDP::valueIteration(double eps, int maxIteration, double lambda)
{
    std::vector<State> S = generateAllStates();
    int nbState = S.size();
    std::vector<Action> A(nbState); // policy
    std::vector<double> V(nbState); // value vector (expected value)
    std::vector<double> VPrime(nbState);
    double delta;

    for (int i = 0; i < maxIteration; i++)
    {
        delta = 0.0;
        for (int j = 0; j < nbState; j++)
        {
            VPrime[j] = 0;
            State& s = S[j];
            std::vector<Action> actions = s.getAvailableActions();

            int nbActions = actions.size();

            std::vector<double> rewards(nbActions);

            if (nbActions == 0)
                continue;

            for (int k = 0; k < nbActions; k++)
            {
                rewards[k] = 0;
                for (State& sPrime : s.genAllStatesFromAction(actions[k]))
                {
                    rewards[k] +=
                        PROBA_I_PIECE * (sPrime.evaluate(config_) +
                                         lambda * V[stateIndex(sPrime)]);
                }
            }
            VPrime[j] = *std::max_element(rewards.begin(), rewards.end());

            for (int k = 0; k < nbActions; k++)
            {
                if (VPrime[j] == rewards[k])
                {
                    A[j] = actions[k];
                    break;
                }
            }

            delta = std::max(delta, abs(VPrime[j] - V[j]));

            V[j] = VPrime[j];
        }

        std::cout << "Iteration i = " << i << " and delta = " << delta
                  << std::endl;

        if (delta < eps)
            break;
    }

    double sum = 0;
    for (int i = 0; i < nbState; i++)
    {
        sum += V[i];
    }
    std::cout << "\naverage over final V " << sum / nbState << std::endl;

    return A;
}

std::vector<State> MDP::generateAllStates()
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

size_t MDP::stateIndex(State& s)
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
    int lines, i, gain;
    lines = i = 0;
    // std::cout << "Initial State:\n" << game.getState() << std::endl;
    // std::cout << "Starting game...\n";

    while (game.getState().getAvailableActions().size() > 0 && i < MAX_ACTION)
    {
        Action a = policy[stateIndex(game.getState())];

        // std::cout << "Action number " << i << std::endl;
        // std::cout << game.getState().getNextTromino();
        game.setState(game.getState().applyAction(a));
        // std::cout << game.getState();

        gain = game.getState().evaluate(config_);
        auto it = std::find(std::begin(config_), std::end(config_), gain);
        lines = std::distance(std::begin(config_), it) + 1;

        game.setScore(game.getScore() + gain);
        if (lines > 0)
        {
            // std::cout << "Action completed " << lines << " lines\n";
        }
        game.setState(game.getState().completeLines());
        // std::cout << "New board:\n" << game.getState().getField();
        // std::cout << "Current score: " << game.getScore() << "\n\n";

        i++;
    }
    std::cout << game.getState().getField();
    std::cout << "\nGame Over! Global score: " << game.getScore() << " in " << i
              << " actions \n";
}
