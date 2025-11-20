#include "State.h"

std::vector<Point> State::placementPositions()
{
    std::vector<Point> positions;
    std::vector<std::vector<bool>> grid = field_.getGrid();
    int cols = field_.getWidth();
    int rows = field_.getHeight();
    for (int c = 0; c < cols; ++c)
    {
        for (int l = 0; l < rows; ++l)
        {
            if (grid[l][c])
            {
                if (l - 1 >= 0)
                    positions.emplace_back(l - 1, c);
                break;
            }
            if (l == rows - 1)
                positions.emplace_back(l, c);
        }
    }
    return positions;
}

std::vector<Action> State::getAvailableActions() // n3
{
    std::vector<Action> possibleActions;
    std::vector<Point> placementPos = placementPositions(); // n2
    std::vector<Point> emptyPositions = field_.getEmptyPositions(); // n2

    for (Point& p : emptyPositions) // n2
    {
        for (int r = 0; r < nextTromino_->rotationCount(); ++r) // cst
        {
            if (!field_.isAvailable(*nextTromino_, p.getX(), p.getY(), r)) // 1
                continue;

            std::vector<Offset> offsets = nextTromino_->getOffsets(r); // 1
            bool allBlocksAccessible = true;
            bool isInPlacementPos = false;

            for (Offset& off : offsets)
            {
                Point candidate(p.getX() + off[0], p.getY() + off[1]);

                // Check if the candidate is in placementPositions
                for (Point& pp : placementPos) // n
                {
                    if (pp == candidate)
                    {
                        isInPlacementPos = true;
                        break;
                    }
                }

                // Check if the column above the candidate is empty up to the
                // top
                bool columnClear = true;
                for (int l = 0; l < candidate.getX(); ++l) // n
                {
                    if (field_.getGrid()[l][candidate.getY()])
                    {
                        columnClear = false;
                        break;
                    }
                }
                if (!columnClear)
                {
                    allBlocksAccessible = false;
                    break;
                }
            }

            if (allBlocksAccessible && isInPlacementPos)
                possibleActions.emplace_back(p, r);
        }
    }
    return possibleActions;
}

State State::applyAction(Action& action)
{
    Field newField = field_.clone();
    newField.addTromino(*nextTromino_, action.getPosition().getX(),
                        action.getPosition().getY(), action.getRotation());

    // choose new random tromino
    std::unique_ptr<Tromino> newNext;
    if ((rand() / (double)RAND_MAX) < 0.5)
        newNext = std::make_unique<IPiece>();
    else
        newNext = std::make_unique<LPiece>();
    return State(newField, std::move(newNext));
}

int State::evaluate(std::array<int, 3>& config)
{
    int completedLines = 0;
    std::vector<std::vector<bool>> grid = field_.getGrid();
    int rows = field_.getHeight();
    int cols = field_.getWidth();

    // Count completed lines
    for (int r = 0; r < rows; ++r)
    {
        bool isComplete = true;
        for (int c = 0; c < cols; ++c)
        {
            if (!grid[r][c])
            {
                isComplete = false;
                break;
            }
        }
        if (isComplete)
            completedLines++;
    }

    int score = 0;
    switch (completedLines)
    {
    case 1:
        score += config[0];
        break;
    case 2:
        score += config[1];
        break;
    case 3:
        score += config[2];
        break;
    default:
        break;
    }

    return score;
}

std::ostream& operator<<(std::ostream& os, const State& s) {
    os << "Next Piece: " << s.getNextTromino() << "\n";
    os << "Current Grid:\n" << s.getField();
    return os;
}
