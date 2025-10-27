import java.util.ArrayList;

class Tetris {
    public static void main(String[] args) {
        System.out.println("Tetris");
        Field field = new Field(3, 4);
        State state = new State(field, new Tromino.IPiece());
        field = state.getField();
        for (int l = 0; l < field.height; l++) {
            for (int c = 0; c < field.width; c++) {
                System.out.print(field.getGrid()[l][c] ? "*" : ".");
            }
            System.out.println();
        }
        ArrayList<Action> actions = state.getAvailableActions();
        System.out.println(actions.size() + " possible actions");
        for (Action a : actions) {
            System.out.println("possible action at (" + a.position.getX() + ", " + a.position.getY() + ") with rotation " + a.rotation);
        }
        state = state.applyAction(actions.get(0));
        field = state.getField();
        for (int l = 0; l < field.height; l++) {
            for (int c = 0; c < field.width; c++) {
                System.out.print(field.getGrid()[l][c] ? "*" : ".");
            }
            System.out.println();
        }
    }
}
