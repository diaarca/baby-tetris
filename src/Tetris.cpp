#include "Action.h"
#include "MDP.h"
#include "State.h"
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <vector>

#define WIDTH 4
#define HEIGHT 4
#define CONFIG_PATH "config.txt"
#define EPSILON 0.00000001
#define MAX_IT 1000
#define ACTION_POLICY_LAMBDA 0.9
#define TROMINO_POLICY_LAMBDA 0.1
#define NB_SIMU 100
#define LINE_WEIGHT 10.0  // High reward for clearing lines
#define HEIGHT_WEIGHT 0.5 // Penalty for height

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
            std::cerr << "Config must contain at least 3 ints\n";
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
              << ", number of simulation = " << NB_SIMU << std::endl
              << std::endl;

    // --- Policy Computations ---
    std::cout << "Computing policies..." << std::endl;

    std::unordered_map<State, Action> vi_policy =
        mdp.actionValueIteration(EPSILON, MAX_IT, ACTION_POLICY_LAMBDA);

    std::unordered_map<State, Action> lh_policy = mdp.lineAndHeightPolicy(
        ACTION_POLICY_LAMBDA, LINE_WEIGHT, HEIGHT_WEIGHT, EPSILON, MAX_IT);

    std::unordered_map<State, std::unique_ptr<Tromino>> rand_tromino;
    std::unordered_map<State, std::unique_ptr<Tromino>> minmax_tromino =
        mdp.trominoValueIterationMinMax(EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);
    std::unordered_map<State, std::unique_ptr<Tromino>> minavg_tromino =
        mdp.trominoValueIterationMinAvg(EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);

    std::cout << "Policies computed." << std::endl << std::endl;

    // --- Simulations ---
    std::cout << "Running simulations..." << std::endl;

    double vi_rand_score = 0, lh_rand_score = 0;
    for (int i = 0; i < NB_SIMU; ++i)
    {
        vi_rand_score += mdp.playPolicy(game, vi_policy, rand_tromino);
        lh_rand_score += mdp.playPolicy(game, lh_policy, rand_tromino);
    }

    double vi_minmax_score = mdp.playPolicy(game, vi_policy, minmax_tromino);
    double lh_minmax_score = mdp.playPolicy(game, lh_policy, minmax_tromino);

    double vi_minavg_score = mdp.playPolicy(game, vi_policy, minavg_tromino);
    double lh_minavg_score = mdp.playPolicy(game, lh_policy, minavg_tromino);

    std::cout << "Simulations complete." << std::endl << std::endl;

    // --- Results ---
    std::cout << "------ Average Results ------" << std::endl << std::endl;

    std::cout << "--- Basic Value Iteration Policy ---" << std::endl;
    std::cout << "Random Adversary: " << vi_rand_score / NB_SIMU << std::endl;
    std::cout << "MinMax Adversary: " << vi_minmax_score << std::endl;
    std::cout << "MinAvg Adversary: " << vi_minavg_score << std::endl
              << std::endl;

    std::cout << "--- Line & Height Policy ---" << std::endl;
    std::cout << "Random Adversary: " << lh_rand_score / NB_SIMU << std::endl;
    std::cout << "MinMax Adversary: " << lh_minmax_score << std::endl;
    std::cout << "MinAvg Adversary: " << lh_minavg_score << std::endl;

    return 0;
}
