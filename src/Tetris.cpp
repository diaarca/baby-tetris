#include "Action.h"
#include "State.h"
#include "MDP.h"
#include "MABTrainer.h"
#include "HeuristicPlayer.h"
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
#define NB_SIMU 100
#define MAB_TRAINING_GAMES 1000

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
              << " and number of simulation = " << NB_SIMU << std::endl
              << std::endl;

    std::cout << "Computing policies..." << std::endl;
    std::unordered_map<State, std::unique_ptr<Tromino>> minMaxTrominos =
        mdp.trominoValueIterationMinMax(EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);

    std::unordered_map<State, std::unique_ptr<Tromino>> minAvgTrominos =
        mdp.trominoValueIterationMinAvg(EPSILON, MAX_IT, TROMINO_POLICY_LAMBDA);

    std::unordered_map<State, Action> actions =
        mdp.actionValueIteration(EPSILON, MAX_IT, ACTION_POLICY_LAMBDA);
    
    std::unordered_map<State, Action> robustActions =
        mdp.robustActionValueIteration(EPSILON, MAX_IT, ACTION_POLICY_LAMBDA);
    std::cout << "Policies computed." << std::endl << std::endl;


    std::cout << "--- Running Value Iteration Policy Simulations ---" << std::endl;
    int randScore = 0;
    for (int i = 0; i < NB_SIMU; i++)
    {
        randScore += mdp.playPolicy(
            game, actions,
            std::unordered_map<State, std::unique_ptr<Tromino>>());
    }
    int minMaxScore = mdp.playPolicy(game, actions, minMaxTrominos);
    int minAvgScore = mdp.playPolicy(game, actions, minAvgTrominos);
    std::cout << "Done." << std::endl << std::endl;


    std::cout << "--- Running Robust Value Iteration Policy Simulations ---" << std::endl;
    int robustRandScore = 0;
    for (int i = 0; i < NB_SIMU; i++)
    {
        robustRandScore += mdp.playPolicy(
            game, robustActions,
            std::unordered_map<State, std::unique_ptr<Tromino>>());
    }
    int robustMinMaxScore = mdp.playPolicy(game, robustActions, minMaxTrominos);
    int robustMinAvgScore = mdp.playPolicy(game, robustActions, minAvgTrominos);
    std::cout << "Done." << std::endl << std::endl;


    std::cout << "--- Training Heuristic Player with MAB ---" << std::endl;
    std::unordered_map<State, std::unique_ptr<Tromino>> emptyAdvPolicy;
    MABTrainer trainer(game, emptyAdvPolicy);
    trainer.train(MAB_TRAINING_GAMES);
    std::vector<double> bestWeights = trainer.getBestWeights();
    std::cout << "Training complete. Best weights found: " << trainer.getBestWeightsString() << std::endl << std::endl;

    std::cout << "--- Running MAB Heuristic Player Simulations ---" << std::endl;
    HeuristicPlayer heuristicPlayer;
    int heuristicRandScore = 0;
    for (int i = 0; i < NB_SIMU; i++)
    {
        heuristicRandScore += heuristicPlayer.playGame(game, bestWeights, emptyAdvPolicy);
    }
    int heuristicMinMaxScore = heuristicPlayer.playGame(game, bestWeights, minMaxTrominos);
    int heuristicMinAvgScore = heuristicPlayer.playGame(game, bestWeights, minAvgTrominos);
    std::cout << "Done." << std::endl << std::endl;


    std::cout << "------ Average results ------" << std::endl << std::endl;
    
    std::cout << "Standard Policy:" << std::endl
              << "  Random Adversary: " << (double)randScore / (double)NB_SIMU
              << std::endl
              << "  MinMax Adversary: " << minMaxScore << std::endl
              << "  MinAvg Adversary: " << minAvgScore << std::endl << std::endl;

    std::cout << "Robust Policy:" << std::endl
              << "  Random Adversary: " << (double)robustRandScore / (double)NB_SIMU
              << std::endl
              << "  MinMax Adversary: " << robustMinMaxScore << std::endl
              << "  MinAvg Adversary: " << robustMinAvgScore << std::endl << std::endl;

    std::cout << "MAB Heuristic Policy:" << std::endl
              << "  Random Adversary: " << (double)heuristicRandScore / (double)NB_SIMU
              << std::endl
              << "  MinMax Adversary: " << heuristicMinMaxScore << std::endl
              << "  MinAvg Adversary: " << heuristicMinAvgScore << std::endl;

    return 0;
}
