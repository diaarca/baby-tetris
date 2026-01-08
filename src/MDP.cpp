#include "MDP.h"
#include "State.h"
#include "Tromino.h"
#include <algorithm>
#include <cstddef>
#include <limits>

std::unordered_map<State, Action>
MDP::actionValueIteration(double epsilon, int maxIteration, double lambda)
{
    if (DEBUG)
    {
        std::cout << "Exploration Value Iteration" << std::endl;
    }
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
            {
                continue;
            }

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
                        std::cerr << "ERROR (actionValueIteration): the "
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

        if (DEBUG)
        {
            std::cout << "i = " << i << " and delta = " << delta << std::endl;
        }
    }

    double sum = 0;
    for (std::pair<const State, double>& item : V)
    {
        sum += item.second;
    }
    if (DEBUG)
    {
        std::cout << "\naverage over final V " << sum / V.size() << std::endl;
    }

    return A;
}

std::unordered_map<State, std::unique_ptr<Tromino>>
MDP::trominoValueIterationMinMax(double epsilon,
                                 int maxIteration,
                                 double lambda)
{
    if (DEBUG)
    {
        std::cout << "Min Max Tromino Value Iteration" << std::endl;
    }
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
            {
                continue;
            }

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

                    reward = placedState.evaluate(config_) + lambda * vAfter;

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

        if (DEBUG)
        {
            std::cout << "i = " << i << " and delta = " << delta << std::endl;
        }
    }

    double sum = 0;
    for (std::pair<const State, double>& item : V)
    {
        sum += item.second;
    }
    if (DEBUG)
    {
        std::cout << "\naverage over final V " << sum / V.size() << std::endl;
    }

    return T;
}

std::unordered_map<State, std::unique_ptr<Tromino>>
MDP::trominoValueIterationMinAvg(double epsilon,
                                 int maxIteration,
                                 double lambda)
{
    if (DEBUG)
    {
        std::cout << "Min Average Tromino Value Iteration" << std::endl;
    }
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
            {
                continue;
            }

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

                    reward = placedState.evaluate(config_) + lambda * vAfter;

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

        if (DEBUG)
        {
            std::cout << "i = " << i << " and delta = " << delta << std::endl;
        }
    }

    double sum = 0;
    for (std::pair<const State, double>& item : V)
    {
        sum += item.second;
    }
    if (DEBUG)
    {
        std::cout << "\naverage over final V " << sum / V.size() << std::endl;
    }

    return T;
}

std::unordered_map<State, double> MDP::generateReachableStates(State s0)
{
    std::unordered_map<State, double> map;
    std::vector<State> q;
    size_t q_head = 0;

    State s0_other = s0.clone();
    const Tromino& current_tromino = s0_other.getNextTromino();
    if (dynamic_cast<const IPiece*>(&current_tromino))
    {
        s0_other.setNextTromino(LPiece());
    }
    else
    {
        s0_other.setNextTromino(IPiece());
    }

    q.push_back(s0.clone());
    map.emplace(std::move(s0), 0);

    if (map.find(s0_other) == map.end())
    {
        q.push_back(s0_other.clone());
        map.emplace(std::move(s0_other), 0);
    }

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

int MDP::getMaxHeight(const Field& field) const
{
    int maxHeight = 0;
    for (int c = 0; c < field.getWidth(); ++c)
    {
        for (int r = 0; r < field.getHeight(); ++r)
        {
            if (field.getGrid()[r][c])
            {
                int height = field.getHeight() - r;
                if (height > maxHeight)
                {
                    maxHeight = height;
                }
                break;
            }
        }
    }
    return maxHeight;
}

// New value iteration policy with a custom reward function
std::unordered_map<State, Action> MDP::lineAndHeightPolicy(double lambda,
                                                           double line_weight,
                                                           double height_weight,
                                                           double epsilon,
                                                           int maxIteration)
{
    if (DEBUG)
    {
        std::cout << "Line and Height Policy Value Iteration" << std::endl;
    }
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
            if (actions.empty())
            {
                continue;
            }

            std::vector<double> rewards(actions.size());

            for (size_t k = 0; k < actions.size(); k++)
            {
                rewards[k] = 0.0;
                for (State& placedState :
                     currState.genAllStatesFromAction(actions[k]))
                {
                    State afterState = placedState.completeLines();

                    auto it = V.find(afterState);
                    if (it == V.end())
                    {
                        std::cerr << "ERROR (lineAndHeightPolicy): the "
                                     "state cannot be derived into "
                                     "a non-reachable state"
                                  << std::endl;
                        exit(1);
                    }
                    vAfter = it->second;

                    double immediate_reward =
                        (line_weight * placedState.nbCompleteLines()) -
                        (height_weight * getMaxHeight(placedState.getField()));

                    rewards[k] +=
                        PROBA_I_PIECE * (immediate_reward + lambda * vAfter);
                }
            }
            vPrime = *std::max_element(rewards.begin(), rewards.end());

            for (size_t k = 0; k < actions.size(); k++)
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

        if (DEBUG)
        {
            std::cout << "i = " << i << " and delta = " << delta << std::endl;
        }
    }

    if (DEBUG)
    {
        double sum = 0;
        for (std::pair<const State, double>& item : V)
        {
            sum += item.second;
        }
        std::cout << "\naverage over final V " << sum / V.size() << std::endl;
    }

    return A;
}
