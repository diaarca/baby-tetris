#include "MDP.h"

std::vector<Action>
MDP::valueIteration(double epsilon, int maxIteration, double lambda)
{
    std::cout << "Action Value Iteration" << std::endl;

    std::vector<State> S = generateAllStates();
    int nbState = S.size();
    std::vector<Action> A(nbState); // policy
    std::vector<double> V(nbState); // value vector (expected value)
    std::vector<double> VPrime(nbState);
    double delta = DBL_MAX;

    for (int i = 0; i < maxIteration && delta > epsilon; i++)
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

            delta = std::max(delta, std::abs(VPrime[j] - V[j]));

            V[j] = VPrime[j];
        }

        std::cout << "i = " << i << " and delta = " << delta << std::endl;
    }

    double sum = 0;
    for (int i = 0; i < nbState; i++)
    {
        sum += V[i];
    }
    std::cout << "\naverage over final V " << sum / nbState << std::endl;

    return A;
}

std::vector<std::unique_ptr<Tromino>>
MDP::trominoValueIteration(double epsilon, int maxIteration, double lambda)
{
    std::cout << "Tromino Value Iteration" << std::endl;
    std::vector<State> S = generateAllStates();
    int nbState = S.size();
    std::vector<std::unique_ptr<Tromino>> T(nbState); // policyTromino
    std::vector<double> V(nbState);  // value vector (expected value)
    std::vector<double> VPrime(nbState);
    double delta = DBL_MAX, maxI, maxL, reward;
    // std::vector<double> lPieceRewards;
    // std::vector<double> iPieceRewards;

    for (int i = 0; i < maxIteration && delta > epsilon; i++)
    {
        // std::cerr << "tromino iteration : " << i << std::endl;
        delta = 0.0;
        for (int j = 0; j < nbState; j++)
        {
            // std::cerr << "state : " << j << std::endl;
            VPrime[j] = 0;
            State& s = S[j];
            std::vector<Action> actions = s.getAvailableActions();

            int nbActions = actions.size();

            std::vector<double> rewards(nbActions);

            if (nbActions == 0)
                continue;

            for (int k = 0; k < nbActions; k++) // for each action of each state
            {
                // rewards[k] = 
                maxI = maxL = 0.0;
                for (State& sPrime : s.genAllStatesFromAction(actions[k]))
                {
                    State afterState = sPrime.completeLines();
                    reward = PROBA_I_PIECE * (afterState.evaluate(config_) +
                                                lambda * V[stateIndex(afterState)]);
                    // rewards[k] += r; // maybe we don't care we'll see
                    const Tromino* t = &afterState.getNextTromino();
                    if (dynamic_cast<const LPiece*>(t) != nullptr)
                    {
                        maxL = reward >= maxL ? reward : maxL;
                        // lPieceRewards.push_back(r);
                    }
                    else if (dynamic_cast<const IPiece*>(t) != nullptr)
                    {
                        maxI = reward >= maxI ? reward : maxI;
                        // iPieceRewards.push_back(r);
                    }
                }
            }
            // picking the min reward tromino is slippery cuz it can lead to the
            // min reward and to max reward min of the max rewards for each
            // piece type
            // double maxL = 0.0;
            // double maxI = 0.0;
            // if (!lPieceRewards.empty())
            // {
            //     maxL = *std::max_element(lPieceRewards.begin(),
            //                              lPieceRewards.end());
            // }
            // if (!iPieceRewards.empty())
            // {
            //     maxI = *std::max_element(iPieceRewards.begin(),
            //                              iPieceRewards.end());
            // }
            // VPrime[j] = maxI >= maxL;
            // TODO : if execo than min avg
            if (maxI >= maxL)
            {
                T[j] = std::make_unique<LPiece>();
                VPrime[j] = maxL;
            }
            else
            {
                T[j] = std::make_unique<IPiece>();
                VPrime[j] = maxI;
            }

            delta = std::max(delta, std::abs(VPrime[j] - V[j]));

            V[j] = VPrime[j];
        }

        std::cout << "i = " << i << " and delta = " << delta << std::endl;
    }

    double sum = 0;
    for (int i = 0; i < nbState; i++)
    {
        sum += V[i];
    }
    std::cout << "\naverage over final V " << sum / nbState << std::endl;

    return T;
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

void MDP::playPolicy(Game& game,
                     std::vector<Action> policy,
                     const std::vector<std::unique_ptr<Tromino>>& advPolicy)
{
    int i, gain;
    i = 0;

    while (game.getState().getAvailableActions().size() > 0 && i < MAX_ACTION)
    {
        State& curr = game.getState();
        Action a = policy[stateIndex(curr)];
        const std::unique_ptr<Tromino>& t = advPolicy[stateIndex(curr)];

        // compute deterministic preview states (placed and after completion)
        State placed = curr.applyActionTromino(a, *t);
        State after = placed.completeLines();

        // pretty-print three fields side-by-side with connectors
        auto fieldToLines = [](const Field& f)
        {
            std::vector<std::string> out;
            for (int r = 0; r < f.getHeight(); ++r)
            {
                std::string line;
                for (int c = 0; c < f.getWidth(); ++c)
                    line.push_back(f.getGrid()[r][c] ? '*' : '.');
                out.push_back(line);
            }
            return out;
        };

        std::vector<std::string> left = fieldToLines(curr.getField());
        std::vector<std::string> middle = fieldToLines(placed.getField());
        std::vector<std::string> right = fieldToLines(after.getField());

        // piece name for connector
        std::ostringstream oss;
        oss << curr.getNextTromino();
        std::string pieceName = oss.str();

        // prepare connectors and align vertically
        int rows =
            std::max({(int)left.size(), (int)middle.size(), (int)right.size()});
        int midRow = rows / 2; // place arrows in middle row
        std::string arrow1 = " --- ";
        arrow1 += pieceName + " --> ";
        std::string arrow2 = " -- Completion -> ";

        for (int r = 0; r < rows; ++r)
        {
            std::string l = (r < (int)left.size())
                                ? left[r]
                                : std::string(left[0].size(), ' ');
            std::string m = (r < (int)middle.size())
                                ? middle[r]
                                : std::string(middle[0].size(), ' ');
            std::string rg = (r < (int)right.size())
                                 ? right[r]
                                 : std::string(right[0].size(), ' ');

            std::string conn1 =
                (r == midRow) ? arrow1 : std::string(arrow1.size(), ' ');
            std::string conn2 =
                (r == midRow) ? arrow2 : std::string(arrow2.size(), ' ');

            std::cout << l << conn1 << m << conn2 << rg << '\n';
        }

        gain = placed.evaluate(config_);

        game.setScore(game.getScore() + gain);

        game.setState(std::move(after));
        std::cout << "Current score: " << game.getScore() << "\n\n";

        i++;
    }

    std::cout << game.getState().getField();
    std::cout << "\nGame Over! Global score: " << game.getScore() << " in " << i
              << " actions \n";
}
