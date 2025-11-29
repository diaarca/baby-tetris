#include "Action.h"
#include "Field.h"
#include "State.h"
#include "Tromino.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>
// #include <Game.h>
#include <MDP.h>

int main(int argc, char** argv)
{
    srand((time(NULL) & 0xFFFF));

    std::string configPath = (argc > 1) ? argv[1] : "config.txt";
    std::array<int, 3> config{};
    std::ifstream in(configPath);
    if (!in)
    {
        std::cerr << "Failed to open config file '" << configPath << "'\n";
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

    Field field(4, 4);
    Game game(config, field);

    // Don't move the game's internal state into the MDP — that would
    // leave `game.state` in a moved-from state with a null
    // `nextTromino_`, causing a segfault when `game.getState()` is
    // accessed later. Instead, build a fresh State copying the
    // Field and recreating the next piece type.
    Field initialField = game.getState().getField();
    std::unique_ptr<Tromino> initialPiece;
    if (dynamic_cast<IPiece*>(&game.getState().getNextTromino()) != nullptr)
        initialPiece = std::make_unique<IPiece>();
    else
        initialPiece = std::make_unique<LPiece>();
    State s0(initialField, std::move(initialPiece));

    MDP mdp(0.5, field.getWidth(), field.getHeight(), std::move(s0), config);
    std::vector<Action> policy = mdp.valueIteration(0.00000001, 1000, 0.1);

    mdp.playPolicy(game, policy);
    // game.playRandom();
    return 0;
}
