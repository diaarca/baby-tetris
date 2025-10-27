import java.util.ArrayList;

public class Field {
    public boolean[][] grid;
    public int width;
    public int height;

    public Field(int width, int height) {
        this.width = width;
        this.height = height;
        this.grid = new boolean[height][width];
    }

    public boolean[][] getGrid() { return this.grid; }

    private boolean isAvailable(int line, int column) {
        if (line < 0 || line >= this.grid.length) return false;
        if (column < 0 || column >= this.grid[0].length) return false;
        return !this.grid[line][column];
    }
    
    public boolean isAvailable(Tromino t, int line, int column, int rotation) {
        int[][] offsets = t.getOffsets(rotation);
        for (int[] off : offsets) {
            int l = line + off[0];
            int c = column + off[1];
            if (!isAvailable(l, c)) return false;
        }
        return true;
    }
    /**
     * Place a tromino at (line, column) using the given rotation.
     * Rotation semantics are provided by each Tromino subclass via getOffsets(rotation).
     * Returns true if placement succeeded, false on collision or out-of-bounds.
     */
    public boolean addTromino(Tromino t, int line, int column, int rotation) {
        int[][] offsets = t.getOffsets(rotation);
        // validate all offsets first
        if (!isAvailable(t, line, column, rotation)) {
            return false;
        }
        // commit placement
        for (int[] off : offsets) {
            int l = line + off[0];
            int c = column + off[1];
            this.grid[l][c] = true;
        }
        return true;
    }

    public ArrayList<Point> getEmptyPositions() {
        ArrayList<Point> positions = new ArrayList<>();
        for (int l = 0; l < this.grid.length; l++) {
            for (int c = 0; c < this.grid[0].length; c++) {
                if (!this.grid[l][c]) {
                    positions.add(new Point(l, c));
                }
            }
        }
        return positions;
    }

    @Override
    public Field clone() {
        Field newField = new Field(this.grid[0].length, this.grid.length);
        for (int l = 0; l < this.grid.length; l++) {
            for (int c = 0; c < this.grid[0].length; c++) {
                newField.grid[l][c] = this.grid[l][c];
            }
        }
        return newField;
    }
}
