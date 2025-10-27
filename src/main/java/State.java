import java.util.ArrayList;

public class State {
    public Field field;
    public Tromino nextTromino;

    public State(Field field, Tromino nextTromino) {
        this.field = field;
        this.nextTromino = nextTromino;
    }

    public Field getField() {
        return this.field;
    }

    public Tromino getNextTromino() {
        return this.nextTromino;
    }

    private ArrayList<Point> placementPositions() {
        ArrayList<Point> positions = new ArrayList<>();
        boolean[][] grid = this.field.getGrid();
        int cols = grid[0].length;
        for (int c = 0; c < cols; c++) {
            for (int l = 0; l < grid.length; l++) {
                if (grid[l][c]) {
                    if (l - 1 >= 0) {
                        positions.add(new Point(l - 1, c));
                    }
                    break;
                } if (l == grid.length - 1) {
                    positions.add(new Point(l, c));
                }
            }
        }
        return positions;
    }

    public ArrayList<Action> getAvailableActions() {
        ArrayList<Action> possibleActions = new ArrayList<Action>();
        ArrayList<Point> placementPosition = placementPositions();
        for (Point p : this.field.getEmptyPositions()) {
            for (int r = 0; r < this.nextTromino.rotationCount(); r++) {
                if (this.field.isAvailable(this.nextTromino, p.getX(), p.getY(), r)) {
                    int[][] offsets = this.nextTromino.getOffsets(r);
                    for (int i = 0; i < offsets.length; i++) {
                        if (placementPosition.contains(new Point(p.getX() + offsets[i][0], p.getY() + offsets[i][1]))) {
                            possibleActions.add(new Action(p, r));
                            break;
                        }
                    } 
                }
            }
        }
        return possibleActions;
    }

    public State applyAction(Action action) {
        Field newField = this.field.clone();

        // add the new tromino
        newField.addTromino(this.nextTromino, action.position.x, action.position.y, action.rotation);

        Tromino newNextTromino = (Math.random() < 0.5) ? new Tromino.IPiece() : new Tromino.LPiece();
        return new State(newField, newNextTromino);
    }
}