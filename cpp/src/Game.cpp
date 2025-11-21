#include "Game.h"
#include <vector>

Game::Game(const std::array<int, 3>& config, Field& field)
    : state(field, std::make_unique<IPiece>()), score(0), config(config)
{
    if ((rand() / (double)RAND_MAX) < 0.5)
    {
        state = State(field, std::make_unique<LPiece>());
    }
}

void Game::playRandom()
{
    // Game loop
    int lines, i, gain;
    std::vector<Action> actions;
    lines = i = 0;
    std::cout << "Initial State:\n" << state << std::endl;
    std::cout << "Starting game...\n";

    while ((actions = state.getAvailableActions()).size() > 0)
    {
        if (!actions.empty())
        {
            // random action for now
            Action a = actions[rand() % actions.size()];
            std::cout << "Action number " << i << std::endl;
            state = state.applyAction(a);
            std::cout << state;
            gain = state.evaluate(config);

            auto it = std::find(std::begin(config), std::end(config), gain);
            lines = std::distance(std::begin(config), it) + 1;
            score += gain;
            if (lines > 0)
            {
                std::cout << "Action completed " << lines << " lines\n";
                std::cout << "New board:\n" << state.getField();
            }
            state = state.completeLines();
            std::cout << "Current score: " << score << "\n\n";
        }
        i++;
    }
    std::cout << "Game Over! Global score: " << score << "\n";
}
