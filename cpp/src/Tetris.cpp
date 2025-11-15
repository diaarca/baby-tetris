#include "Action.h"
#include "Field.h"
#include "State.h"
#include "Tromino.h"
#include <array>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <vector>

int main(int argc, char** argv)
{
    std::string configPath = (argc > 1) ? argv[1] : "config.txt";
    std::array<int, 5> config{};
    std::ifstream in(configPath);
    if (!in)
    {
        std::cerr << "Failed to open config file '" << configPath << "'\n";
        return 1;
    }
    for (int i = 0; i < 5; ++i)
    {
        if (!(in >> config[i]))
        {
            std::cerr << "Config must contain at least 5 ints\n";
            return 1;
        }
    }
    std::cout << "Loaded config: [";
    for (int i = 0; i < 5; ++i)
    {
        std::cout << config[i] << (i + 1 < 5 ? ", " : "");
    }
    std::cout << "]\n";

    Field field(4, 4);
    State state(field, std::make_unique<IPiece>());
    field = state.getField();
    for (int l = 0; l < field.getHeight(); ++l)
    {
        for (int c = 0; c < field.getWidth(); ++c)
        {
            std::cout << (field.getGrid()[l][c] ? '*' : '.');
        }
        std::cout << '\n';
    }
    auto actions = state.getAvailableActions();
    std::cout << actions.size() << " possible actions\n";
    for (auto& a : actions)
    {
        std::cout << "possible action at (" << a.getPosition().getX() << ", "
                  << a.getPosition().getY() << ") with rotation "
                  << a.getRotation() << "\n";
    }
    if (!actions.empty())
    {
        state = state.applyAction(actions[0]);
        field = state.getField();
        for (int l = 0; l < field.getHeight(); ++l)
        {
            for (int c = 0; c < field.getWidth(); ++c)
            {
                std::cout << (field.getGrid()[l][c] ? '*' : '.');
            }
            std::cout << '\n';
        }
        std::cout << "State evaluation: " << state.evaluate(config) << '\n';
    }
    return 0;
}
