#include "MDP.h"

std::unordered_map<State, Action>
MDP::valueIteration(double epsilon, int maxIteration, double lambda)
{
    std::unordered_map<State, double> V =
        generateReachableStates(std::move(s0_));

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
                        std::cerr << "ERROR: the state cannot be derived into "
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

void MDP::playPolicy(Game& game,
                     const std::unordered_map<State, Action>& policy)
{
    int i = 0, gain;

    while (game.getState().getAvailableActions().size() > 0 && i < MAX_ACTION)
    {
        const State& curr = game.getState();
        auto it = policy.find(curr);
        if (it == policy.end())
        {
            std::cerr
                << "ERROR the state:\n"
                << curr
                << "\n haven't any associated action in the provided policy";
            exit(1);
        }
        const Action& a = policy.find(curr)->second;

        // compute deterministic preview states (placed and after completion)
        State placed = curr.applyAction(a);
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
