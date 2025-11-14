/**
 * Polymorphic representation of a 3-block tromino.
 *
 * Subclasses provide rotation-aware footprints. Offsets are given as
 * (rowOffset, columnOffset) pairs relative to a placement origin.
 */
public abstract class Tromino {
    /**
     * Return the occupied offsets for the given rotation index.
     * Rotation indices normalize internally (e.g. IPiece has 2 unique
     * rotations). Each offset is an int[]{rowOffset, columnOffset}.
     */
    public abstract int[][] getOffsets(int rotation);

    /** Number of unique rotations (2 or 4). Default is 4. */
    public int rotationCount() { return 4; }

    @Override
    public String toString() {
        return this.getClass().getSimpleName();
    }

    // Concrete pieces
    public static class IPiece extends Tromino {
        @Override
        public int rotationCount() {
            return 2;
        }

        @Override
        public int[][] getOffsets(int rotation) {
            int r = ((rotation % 2) + 2) % 2;
            if (r == 0) {
                // horizontal: 1 x 3
                return new int[][] {{0, 0}, {0, 1}, {0, 2}};
            } else {
                // vertical: 3 x 1
                return new int[][] {{0, 0}, {1, 0}, {2, 0}};
            }
        }
    }

    public static class LPiece extends Tromino {
        @Override
        public int rotationCount() {
            return 4;
        }

        @Override
        public int[][] getOffsets(int rotation) {
            int r = ((rotation % 4) + 4) % 4;
            // Represent the L-shaped tromino as one of the four 2x2 boxes with
            // one missing cell.
            switch (r) {
            // missing top-left
            case 0:
                return new int[][] {{0, 1}, {1, 0}, {1, 1}};
            // missing top-right
            case 1:
                return new int[][] {{0, 0}, {1, 0}, {1, 1}};
            // missing bottom-right
            case 2:
                return new int[][] {{0, 0}, {0, 1}, {1, 0}};
            // missing bottom-left
            default:
                return new int[][] {{0, 0}, {0, 1}, {1, 1}};
            }
        }
    }
}
