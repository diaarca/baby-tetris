#include "HeuristicPlayer.h"
#include "Game.h"
#include "State.h"
#include "Field.h"
#include "Tromino.h"
#include <vector>
#include <limits>
#include <iostream>

#define MAX_GAME_MOVES 10000

int HeuristicPlayer::playGame(Game& game, const std::vector<double>& weights, const std::unordered_map<State, std::unique_ptr<Tromino>>& advPolicy)
{
    game.getState().getField() = Field(game.getState().getField().getWidth(), game.getState().getField().getHeight());
    game.setScore(0);

    // Set initial piece
    if ((rand() / (double)RAND_MAX) < PROBA_I_PIECE)
    {
        game.getState().setNextTromino(IPiece());
    }
    else
    {
        game.getState().setNextTromino(LPiece());
    }


    for (int move = 0; move < MAX_GAME_MOVES; ++move)
    {
        State& curr = game.getState();
        const auto& actions = curr.getAvailableActions();

        if (actions.empty())
        {
            break; // Game over
        }

        Action bestAction;
        double bestScore = -std::numeric_limits<double>::infinity();

        // The piece to be placed is the "nextTromino" from the current state
        const Tromino& pieceToPlace = curr.getNextTromino();

        for (const auto& action : actions)
        {
            State tempState = curr.clone();
            tempState.setNextTromino(pieceToPlace); // Ensure correct piece is considered
            Field potentialField = tempState.applyAction(action).getField();
            
            double score = 0.0;
            score += weights[0] * potentialField.getAggregateHeight();
            score += weights[1] * potentialField.getCompleteLines();
            score += weights[2] * potentialField.getHoles();
            score += weights[3] * potentialField.getBumpiness();

            if (score > bestScore)
            {
                bestScore = score;
                bestAction = action;
            }
        }
        
        // Determine adversary's choice for the *next* piece
        const std::unique_ptr<Tromino>* t_ptr;
        std::unique_ptr<Tromino> t_owned;
        auto itTromino = advPolicy.find(curr);
        if (itTromino == advPolicy.end())
        {
            if ((rand() / (double)RAND_MAX) < PROBA_I_PIECE)
            {
                t_owned = std::make_unique<IPiece>();
            }
            else
            {
                t_owned = std::make_unique<LPiece>();
            }
            t_ptr = &t_owned;
        }
        else
        {
            t_ptr = &itTromino->second;
        }
        const std::unique_ptr<Tromino>& nextPiece = *t_ptr;

        // Apply the chosen action and the current piece
        State placed = curr.applyActionTromino(bestAction, *nextPiece);
        
        int gain = placed.evaluate(game.getConfig());
        game.setScore(game.getScore() + gain);
        
        State after = placed.completeLines();
        
        game.setState(std::move(after));
    }

    return game.getScore();
}
