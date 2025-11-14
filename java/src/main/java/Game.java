public class Game {

    private State state;
    private int score;

    public Game(int[] config, int height, int width) {

        Field field = new Field(width, height);
        Tromino firstTromino =
            (Math.random() < 0.5) ? new Tromino.IPiece() : new Tromino.LPiece();

        this.state = new State(field, firstTromino);
        this.score = 0;
    }
}
