#include <MDP.h>
#include <cstddef>
#include <cstdlib>
#include <fstream>
#include <iostream>

#define WIDTH 4
#define HEIGHT 4
#define CONFIG_PATH "config.txt"
#define EPSILON 0.00000001
#define MAX_IT 100
#define LAMBDA 0.5

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
    // Field field1(WIDTH, HEIGHT);
    // Game game(config, field1);
    //
    // MDP mdp1(field1.getWidth(), field1.getHeight(), game.getState().clone(),
    //          config);
    //
    // std::cout << "All constants:" << std::endl
    //           << "width = " << WIDTH << ", height = " << HEIGHT
    //           << ", probaIPiece = " << PROBA_I_PIECE
    //           << ", maxGameAction = " << MAX_ACTION << std::endl
    //           << "epsilon = " << EPSILON << ", maxIteration = " << MAX_IT
    //           << ", lambda = " << LAMBDA << std::endl
    //           << std::endl;
    //
    // std::cout << "\n" << std::endl;
    //
    // // compute the optimal policy using the value iteration algorithm
    // std::vector<Action> policy1 = mdp1.valueIteration(EPSILON, MAX_IT, LAMBDA);
    //
    // // play the computed policy on the game
    // int playerOnly =
    //     mdp1.playPolicy(game, policy1, std::vector<std::unique_ptr<Tromino>>());

    Field field2(WIDTH, HEIGHT);
    Game game2(config, field2);

    MDP mdp2(field2.getWidth(), field2.getHeight(), game2.getState().clone(),
             config);

    // compute the optimal adversary policy
    std::vector<std::unique_ptr<Tromino>> trominos =
        mdp2.trominoValueIteration(EPSILON, MAX_IT, LAMBDA);

    std::cout << "\n" << std::endl;

    std::vector<Action> policy2 = mdp2.valueIteration(EPSILON, MAX_IT, LAMBDA);

    // int both = mdp2.playPolicy(game2, policy2, trominos);
    mdp2.playPolicy(game2, policy2, trominos);

    // std::cout << "playerOnly: " << playerOnly << std::endl
    //           << "both: " << both << std::endl;

    return 0;
}
