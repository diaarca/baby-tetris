#include "State.h"
#include "Tromino.h"
#include <memory>
#include <stdexcept>

std::vector<Point> State::placementPositions() const
{
    std::vector<Point> positions;
    const auto& grid = field_.getGrid();
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

std::vector<Action> State::getAvailableActions() const
{
    std::vector<Action> possibleActions;
    std::vector<Point> placementPos = placementPositions();
    std::vector<Point> emptyPositions = field_.getEmptyPositions();

    for (const Point& p : emptyPositions)
    {
        if (!nextTromino_)
            continue;
        for (int r = 0; r < nextTromino_->rotationCount(); ++r)
        {
            if (!field_.isAvailable(*nextTromino_, p.getX(), p.getY(), r))
                continue;

            std::vector<Offset> offsets = nextTromino_->getOffsets(r);
            bool allBlocksAccessible = true;
            bool isInPlacementPos = false;

            for (const Offset& off : offsets)
            {
                Point candidate(p.getX() + off[0], p.getY() + off[1]);

                for (const Point& pp : placementPos)
                {
                    if (pp == candidate)
                    {
                        isInPlacementPos = true;
                        break;
                    }
                }

                bool columnClear = true;
                for (int l = 0; l < candidate.getX(); ++l)
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

    std::unique_ptr<Tromino> newNext;
    if ((rand() / (double)RAND_MAX) < PROBA_I_PIECE)
        newNext = std::make_unique<IPiece>();
    else
        newNext = std::make_unique<LPiece>();
    return State(std::move(newField), std::move(newNext));
}


State State::applyAction(const Action& action) const
{
    Field newField = field_.clone();
    newField.addTromino(*nextTromino_, action.getPosition().getX(),
                        action.getPosition().getY(), action.getRotation());

    std::unique_ptr<Tromino> newNext;
    if ((rand() / (double)RAND_MAX) < PROBA_I_PIECE)
        newNext = std::make_unique<IPiece>();
    else
        newNext = std::make_unique<LPiece>();
    return State(std::move(newField), std::move(newNext));
}

State State::applyActionTromino(Action action, const Tromino& t)
{
    Field newField = field_.clone();
    newField.addTromino(*nextTromino_, action.getPosition().getX(),
                        action.getPosition().getY(), action.getRotation());

    std::unique_ptr<Tromino> newTromino = t.clone();
    return State(newField, std::move(newTromino));
}

std::vector<State> State::genAllStatesFromAction(Action& action)
{
    Field newField1 = field_.clone();
    newField1.addTromino(*nextTromino_, action.getPosition().getX(),
                         action.getPosition().getY(), action.getRotation());

    Field newField2 = field_.clone();
    newField2.addTromino(*nextTromino_, action.getPosition().getX(),
                         action.getPosition().getY(), action.getRotation());

    std::unique_ptr<Tromino> next1 = std::make_unique<IPiece>();
    std::unique_ptr<Tromino> next2 = std::make_unique<LPiece>();

    std::vector<State> res;
    res.reserve(2);
    res.emplace_back(std::move(newField1), std::move(next1));
    res.emplace_back(std::move(newField2), std::move(next2));
    return res;
}


std::vector<State> State::genAllStatesFromAction(const Action& action) const
{
    Field newField1 = field_.clone();
    newField1.addTromino(*nextTromino_, action.getPosition().getX(),
                         action.getPosition().getY(), action.getRotation());

    Field newField2 = field_.clone();
    newField2.addTromino(*nextTromino_, action.getPosition().getX(),
                         action.getPosition().getY(), action.getRotation());

    std::unique_ptr<Tromino> next1 = std::make_unique<IPiece>();
    std::unique_ptr<Tromino> next2 = std::make_unique<LPiece>();

    std::vector<State> res;
    res.reserve(2);
    res.emplace_back(std::move(newField1), std::move(next1));
    res.emplace_back(std::move(newField2), std::move(next2));
    return res;
}

int State::evaluate(std::array<int, 3>& config) const
{
    int completedLines = 0;
    const auto& grid = field_.getGrid();
    int rows = field_.getHeight();
    int cols = field_.getWidth();

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

State State::completeLines()
{
    auto grid = field_.getGrid();
    bool didComplete = false;

    for (int r = 0; r < field_.getHeight(); ++r)
    {
        bool isComplete = true;
        for (int c = 0; c < field_.getWidth(); ++c)
        {
            if (!field_.getGrid()[r][c])
            {
                isComplete = false;
                break;
            }
        }
        if (isComplete)
        {
            didComplete = true;
            // clear complete line
            grid[r] = {false, false, false, false};
            // make all upper lines moving down
            for (int row = r; row > 0; --row)
            {
                grid[row] = grid[row - 1];
            }
            // clear top line
            for (int c = 0; c < field_.getWidth(); ++c)
            {
                grid[0][c] = false;
            }
        }
    }

    State newState = clone();

    // the state after all line removals
    if (didComplete)
    {
        newState.getField().setGrid(grid);
    }

    return newState;
}

State State::clone() const
{
    std::unique_ptr<Tromino> newTromino;
    if (nextTromino_)
    {
        if (nextTromino_->isIPiece())
            newTromino = std::make_unique<IPiece>();
        else if (nextTromino_->isLPiece())
            newTromino = std::make_unique<LPiece>();
    }

    return State(field_.clone(), std::move(newTromino));
}

std::ostream& operator<<(std::ostream& os, const State& s)
{
    os << "Next Piece: " << s.getNextTromino() << "\n";
    os << "Current Grid:\n" << s.getField();
    return os;
}

bool State::operator==(const State& other) const
{
    if (field_.getGrid() != other.field_.getGrid())
    {
        return false;
    }

    bool thisHasPiece = !!nextTromino_;
    bool otherHasPiece = !!other.nextTromino_;
    if (thisHasPiece != otherHasPiece)
        return false;
    if (!thisHasPiece)
        return true;

    bool thisIsIPiece = nextTromino_->isIPiece();
    bool otherIsIPiece = other.nextTromino_->isIPiece();

    return thisIsIPiece == otherIsIPiece;
}

size_t State::hash() const
{
    int width = field_.getWidth();
    int height = field_.getHeight();
    int cells = width * height;

    if (cells < 0 || cells >= 64)
    {
        return static_cast<size_t>(-1);
    }

    uint64_t mask = 0ULL;
    const auto& grid = field_.getGrid();

    if ((int)grid.size() != height)
    {
        return static_cast<size_t>(-1);
    }

    for (int r = 0; r < height; ++r)
    {
        if ((int)grid[r].size() != width)
        {
            return static_cast<size_t>(-1);
        }
        for (int c = 0; c < width; ++c)
        {
            if (grid[r][c])
            {
                mask |= (1ULL << (r * width + c));
            }
        }
    }

    size_t pieceIndex;
    if (nextTromino_)
    {
        if (nextTromino_->isIPiece())
        {
            pieceIndex = 1;
        }
        else if (nextTromino_->isLPiece())
        {
            pieceIndex = 2;
        }
        else
        {
            return static_cast<size_t>(-1); // Unknown piece
        }
    }
    else
    {
        pieceIndex = 0; // No piece
    }

    return static_cast<size_t>(mask) * 3 + pieceIndex;
}
