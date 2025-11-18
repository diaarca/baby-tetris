#include "Game.h"

Game::Game(const std::array<int, 3>& config, Field& field)
    : state(field, std::make_unique<IPiece>()), score(0), config(config)
{
    if ((rand() / (double)RAND_MAX) < 0.5)
    {
        state = State(field, std::make_unique<LPiece>());
    }
}

void Game::play()
{
    // Game loop
    int lines, i;
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
            score += state.evaluate(config);
            if ((lines = completeLine()) > 0)
            {
                std::cout << "Action completed " << lines << " lines\n";
                std::cout << "New board:\n" << state.getField();
            }
            std::cout << "Current score: " << score << "\n\n";
        }
        i++;
    }
    std::cout << "Game Over! Global score: " << score << "\n";
}
// returns number of completed lines
int Game::completeLine()
{
    Field field = state.getField();
    std::vector<std::vector<bool>> grid = field.getGrid();
    int removeCount = 0;
    // check if one line is full
    for (int r = 0; r < field.getHeight(); ++r)
    {
        bool isComplete = true;
        for (int c = 0; c < field.getWidth(); ++c)
        {
            if (!field.getGrid()[r][c])
            {
                isComplete = false;
                break;
            }
        }
        if (isComplete)
        {
            // std::cout << state.getField();
            // remove line
            for (int c = 0; c < field.getWidth(); ++c)
            {
                grid[r][c] = false;
            }
            removeCount++;
            // move all lines above down
            for (int row = r; row > 0; --row)
            {
                for (int c = 0; c < field.getWidth(); ++c)
                {
                    grid[row][c] = grid[row - 1][c];
                }
            }
            // clear top line
            for (int c = 0; c < field.getWidth(); ++c)
            {
                grid[0][c] = false;
            }
        }
    }
    // the state after all line removals
    if (removeCount)
    {
        std::unique_ptr<Tromino> newTromino;
        if (dynamic_cast<IPiece*>(&state.getNextTromino()))
        {
            newTromino = std::make_unique<IPiece>();
        }
        else if (dynamic_cast<LPiece*>(&state.getNextTromino()))
        {
            newTromino = std::make_unique<LPiece>();
        }
        field.setGrid(grid);
        state = State(field, std::move(newTromino));
    }
    return removeCount;
}
