#include "MDP.h"

std::unordered_map<State, Action>
MDP::actionValueIteration(double epsilon, int maxIteration, double lambda)
{
    std::cout << "Exploration Value Iteration" << std::endl;
    std::unordered_map<State, double> V = generateReachableStates(s0_.clone());

    std::unordered_map<State, Action> A;

    double vAfter, vPrime, delta;
    delta = DBL_MAX;

    for (int i = 0; i < maxIteration && delta > epsilon; i++)
    {
        delta = 0.0;
        for (auto& [currState, currValue] : V)
        {
            std::vector<Action> actions = currState.getAvailableActions();

            int nbActions = actions.size();

            std::vector<double> rewards(nbActions);

            if (nbActions == 0)
                continue;

            for (int k = 0; k < nbActions; k++)
            {
                rewards[k] = 0.0;
                for (State& placedState :
                     currState.genAllStatesFromAction(actions[k]))
                {
                    State afterState = placedState.completeLines();

                    auto it = V.find(afterState);
                    if (it == V.end())
                    {
                        std::cerr << "ERROR (actionValueIterationExpl): the "
                                     "state cannot be derived into "
                                     "a non-reachable state"
                                  << std::endl;
                        exit(1);
                    }
                    vAfter = it->second;

                    rewards[k] +=
                        PROBA_I_PIECE *
                        (placedState.evaluate(config_) + lambda * vAfter);
                }
            }
            vPrime = *std::max_element(rewards.begin(), rewards.end());

            for (int k = 0; k < nbActions; k++)
            {
                if (vPrime == rewards[k])
                {
                    A.insert_or_assign(currState.clone(), actions[k]);
                    break;
                }
            }

            delta = std::max(delta, std::abs(vPrime - currValue));
            V.insert_or_assign(currState.clone(), vPrime);
        }

        std::cout << "i = " << i << " and delta = " << delta << std::endl;
    }

    double sum = 0;
    for (std::pair<const State, double>& item : V)
    {
        sum += item.second;
    }
    std::cout << "\naverage over final V " << sum / V.size() << std::endl;

    return A;
}

std::unordered_map<State, std::unique_ptr<Tromino>>
MDP::trominoValueIterationMinMax(double epsilon,
                                 int maxIteration,
                                 double lambda)
{
    std::cout << "Min Max Tromino Value Iteration" << std::endl;
    std::unordered_map<State, double> V = generateReachableStates(s0_.clone());

    std::unordered_map<State, std::unique_ptr<Tromino>> T;

    double delta, maxI, maxL, reward, vPrime, vAfter;
    delta = DBL_MAX;

    for (int i = 0; i < maxIteration && delta > epsilon; i++)
    {
        delta = 0.0;
        for (auto& [currState, currValue] : V)
        {
            std::vector<Action> actions = currState.getAvailableActions();

            int nbActions = actions.size();

            if (nbActions == 0)
                continue;

            maxI = maxL = 0.0;
            for (int k = 0; k < nbActions; k++)
            {
                for (State& placedState :
                     currState.genAllStatesFromAction(actions[k]))
                {
                    State afterState = placedState.completeLines();

                    auto it = V.find(afterState);
                    if (it == V.end())
                    {
                        std::cerr << "ERROR (trominoValueIteration): the "
                                     "state cannot be derived into "
                                     "a non-reachable state"
                                  << std::endl;
                        exit(1);
                    }
                    vAfter = it->second;

                    reward = PROBA_I_PIECE *
                             (placedState.evaluate(config_) + lambda * vAfter);

                    const Tromino* t = &afterState.getNextTromino();
                    if (dynamic_cast<const LPiece*>(t) != nullptr)
                    {
                        maxL = reward > maxL ? reward : maxL;
                    }
                    else if (dynamic_cast<const IPiece*>(t) != nullptr)
                    {
                        maxI = reward > maxI ? reward : maxI;
                    }
                }
            }
            if (maxL < maxI)
            {
                T.insert_or_assign(currState.clone(),
                                   std::make_unique<LPiece>());
                vPrime = maxL;
            }
            else
            {
                T.insert_or_assign(currState.clone(),
                                   std::make_unique<IPiece>());
                vPrime = maxI;
            }

            delta = std::max(delta, std::abs(vPrime - currValue));

            V.insert_or_assign(currState.clone(), vPrime);
        }

        std::cout << "i = " << i << " and delta = " << delta << std::endl;
    }

    double sum = 0;
    for (std::pair<const State, double>& item : V)
    {
        sum += item.second;
    }
    std::cout << "\naverage over final V " << sum / V.size() << std::endl;

    return T;
}

std::unordered_map<State, std::unique_ptr<Tromino>>
MDP::trominoValueIterationMinAvg(double epsilon,
                                 int maxIteration,
                                 double lambda)
{
    std::cout << "Min Average Tromino Value Iteration" << std::endl;
    std::unordered_map<State, double> V = generateReachableStates(s0_.clone());

    std::unordered_map<State, std::unique_ptr<Tromino>> T;

    double delta, reward, vPrime, vAfter, avgI, avgL;
    delta = DBL_MAX;

    for (int i = 0; i < maxIteration && delta > epsilon; i++)
    {
        delta = 0.0;
        for (auto& [currState, currValue] : V)
        {
            std::vector<Action> actions = currState.getAvailableActions();

            int nbActions = actions.size();

            if (nbActions == 0)
                continue;

            avgI = avgL = 0.0;
            for (int k = 0; k < nbActions; k++)
            {
                for (State& placedState :
                     currState.genAllStatesFromAction(actions[k]))
                {
                    State afterState = placedState.completeLines();

                    auto it = V.find(afterState);
                    if (it == V.end())
                    {
                        std::cerr << "ERROR (trominoValueIteration): the "
                                     "state cannot be derived into "
                                     "a non-reachable state"
                                  << std::endl;
                        exit(1);
                    }
                    vAfter = it->second;

                    reward = PROBA_I_PIECE *
                             (placedState.evaluate(config_) + lambda * vAfter);

                    const Tromino* t = &afterState.getNextTromino();
                    if (dynamic_cast<const LPiece*>(t) != nullptr)
                    {
                        avgL += reward;
                    }
                    else if (dynamic_cast<const IPiece*>(t) != nullptr)
                    {
                        avgI += reward;
                    }
                }
            }
            avgI /= nbActions;
            avgL /= nbActions;
            if (avgL < avgI)
            {
                T.insert_or_assign(currState.clone(),
                                   std::make_unique<LPiece>());
                vPrime = avgL;
            }
            else
            {
                T.insert_or_assign(currState.clone(),
                                   std::make_unique<IPiece>());
                vPrime = avgI;
            }

            delta = std::max(delta, std::abs(vPrime - currValue));

            V.insert_or_assign(currState.clone(), vPrime);
        }

        std::cout << "i = " << i << " and delta = " << delta << std::endl;
    }

    double sum = 0;
    for (std::pair<const State, double>& item : V)
    {
        sum += item.second;
    }
    std::cout << "\naverage over final V " << sum / V.size() << std::endl;

    return T;
}

std::unordered_map<State, double> MDP::generateReachableStates(State s0)
{
    std::unordered_map<State, double> map;
    std::vector<State> q;
    size_t q_head = 0;

    q.push_back(s0.clone());
    map.emplace(std::move(s0), 0);

    while (q_head < q.size())
    {
        State currState = std::move(q[q_head]);
        q_head++;

        for (const Action& a : currState.getAvailableActions())
        {
            for (auto& nextState : currState.genAllStatesFromAction(a))
            {
                State finalNextState = nextState.completeLines();
                if (map.find(finalNextState) == map.end())
                {
                    q.push_back(finalNextState.clone());
                    map.emplace(std::move(finalNextState), 0.0);
                }
            }
        }
    }
    return map;
}

int MDP::playPolicy(
    Game& game,
    const std::unordered_map<State, Action>& policy,
    const std::unordered_map<State, std::unique_ptr<Tromino>>& advPolicy)
{
    // maybe use the tromino policy to fix the very first Tromino
    game.setState(s0_.clone());
    game.setScore(0);

    State& curr = game.getState();
    const std::unique_ptr<Tromino>* t_ptr;
    std::unique_ptr<Tromino> t_owned;

    auto itTromino = advPolicy.find(curr);
    if (itTromino == advPolicy.end())
    {
        if ((rand() / (double)RAND_MAX) < PROBA_I_PIECE)
        {
            t_owned = std::make_unique<IPiece>();
        }
        else
        {
            t_owned = std::make_unique<LPiece>();
        }
        t_ptr = &t_owned;
    }
    else
    {
        t_ptr = &itTromino->second;
    }
    const std::unique_ptr<Tromino>& t = *t_ptr;
    game.getState().setNextTromino(*t);

    int nbAction = 0, gain;

    while (game.getState().getAvailableActions().size() > 0 &&
           nbAction < MAX_ACTION)
    {
        State& curr = game.getState();
        auto itAction = policy.find(curr);
        if (itAction == policy.end())
        {
            std::cerr << "ERROR the state:\n"
                      << curr << std::endl
                      << "haven't any associated action in the provided policy"
                      << std::endl;
            exit(1);
        }
        const Action& a = itAction->second;

        const std::unique_ptr<Tromino>* t_ptr;
        std::unique_ptr<Tromino> t_owned;

        auto itTromino = advPolicy.find(curr);
        if (itTromino == advPolicy.end())
        {
            if ((rand() / (double)RAND_MAX) < PROBA_I_PIECE)
            {
                t_owned = std::make_unique<IPiece>();
            }
            else
            {
                t_owned = std::make_unique<LPiece>();
            }
            t_ptr = &t_owned;
        }
        else
        {
            t_ptr = &itTromino->second;
        }
        const std::unique_ptr<Tromino>& t = *t_ptr;

        // compute deterministic preview states (placed and after completion)
        State placed = curr.applyActionTromino(a.clone(), *t);
        State after = placed.completeLines();

        // prettyPrint(curr, placed.clone(), after.clone());

        gain = placed.evaluate(config_);

        game.setScore(game.getScore() + gain);

        game.setState(std::move(after));
        // std::cout << "Current score: " << game.getScore() << "\n\n";

        nbAction++;
    }

    // std::cout << game.getState().getField();
    // std::cout << "\nGame Over! Global score: " << game.getScore() << " in "
    //           << nbAction << " actions \n";

    return game.getScore();
}

void MDP::prettyPrint(State& curr, State placed, State after)
{
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
        std::string l =
            (r < (int)left.size()) ? left[r] : std::string(left[0].size(), ' ');
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
}
