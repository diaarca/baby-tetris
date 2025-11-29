#include <MDP.h>
#include <cstdlib>
#include <fstream>
#include <iostream>

#define WIDTH 4
#define HEIGHT 4
#define CONFIG_PATH "config.txt"
#define EPSILON 0.00000001
#define MAX_IT 100
#define LAMBDA 0.1

using namespace std;

int main()
{
    srand((time(NULL) & 0xFFFF));

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

    Field field(WIDTH, HEIGHT);
    Game game(config, field);

    Field initialField = game.getState().getField();
    std::unique_ptr<Tromino> initialPiece;

    if (dynamic_cast<IPiece*>(&game.getState().getNextTromino()) != nullptr)
        initialPiece = std::make_unique<IPiece>();
    else
        initialPiece = std::make_unique<LPiece>();

    State s0(initialField, std::move(initialPiece));

    MDP mdp(field.getWidth(), field.getHeight(), std::move(s0),
            config);
    std::vector<Action> policy = mdp.valueIteration(EPSILON, MAX_IT, LAMBDA);

    mdp.playPolicy(game, policy);

    return 0;
}
