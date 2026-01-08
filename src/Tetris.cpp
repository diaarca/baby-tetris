#include "Action.h"
#include "MDP.h"
#include "State.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>

#define WIDTH 4
#define HEIGHT 4
#define CONFIG_PATH "config.txt"
#define EPSILON 0.00000001
#define MAX_IT 1000
#define ACTION_POLICY_LAMBDA 0.9
#define TROMINO_POLICY_LAMBDA 0.1
#define NB_SIMU 1

int main()
{
    // initialize pseudo random generator
    srand((time(NULL) & 0xFFFF));

    // parsing the "config.txt" file in order to parametrize the reward function
    std::array<int, 3> config{};
    std::ifstream in(CONFIG_PATH);
    if (!in)
    {
        std::cerr << "Failed to open config file '" << CONFIG_PATH << "'\n";
        return 1;
    }
    for (int i = 0; i < 3; ++i)
    {
        if (!(in >> config[i]))
        {
            std::cerr << "Config must contain at least 5 ints\n";
            return 1;
        }
    }
    std::cout << "Loaded config: [";
    for (int i = 0; i < 3; ++i)
    {
        std::cout << config[i] << (i + 1 < 3 ? ", " : "");
    }
    std::cout << "]\n\n";

    // initializing the game and the MDP to compute the optimal policy
    Field field(WIDTH, HEIGHT);
    Game game(config, field);

    MDP mdp(field.getWidth(), field.getHeight(), game.getState().clone(),
            config);

    std::cout << "All constants:" << std::endl
              << "width = " << WIDTH << ", height = " << HEIGHT
              << ", probaIPiece = " << PROBA_I_PIECE
              << ", maxGameAction = " << MAX_ACTION << ", epsilon = " << EPSILON
              << ", maxIteration = " << MAX_IT
              << ", action policy lambda = " << ACTION_POLICY_LAMBDA
              << ", tromino policy lambda = " << TROMINO_POLICY_LAMBDA
              << " and number of simulation = " << NB_SIMU << std::endl
              << std::endl;

    std::unordered_map<State, std::unique_ptr<Tromino>> minMaxTrominos =
        mdp.trominoValueIterationMinMax(EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);

    if (DEBUG)
    {
        std::cout << std::endl << std::endl;
    }

    std::unordered_map<State, std::unique_ptr<Tromino>> minAvgTrominos =
        mdp.trominoValueIterationMinAvg(EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);

    if (DEBUG)
    {
        std::cout << std::endl << std::endl;
    }

    // compute the optimal policy using the value iteration algorithm
    std::unordered_map<State, Action> actions =
        mdp.actionValueIteration(EPSILON, MAX_IT, ACTION_POLICY_LAMBDA);

    if (DEBUG)
    {
        std::cout << std::endl << std::endl;
    }

    int rand = 0;

    for (int i = 0; i < NB_SIMU; i++)
    {
        rand += mdp.playPolicy(
            game, actions,
            std::unordered_map<State, std::unique_ptr<Tromino>>());
    }

    int minMax = mdp.playPolicy(game, actions, minMaxTrominos);
    int minAvg = mdp.playPolicy(game, actions, minAvgTrominos);

    std::cout << "Average results:" << std::endl
              << "Random Adversary moves: " << (double)rand / (double)NB_SIMU
              << std::endl
              << "Min Max Adversary Moves: " << minMax << std::endl
              << "Min Avg Adversary Moves: " << minAvg << std::endl;

    return 0;
}
