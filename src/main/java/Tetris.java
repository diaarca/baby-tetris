import java.util.ArrayList;
import java.io.IOException;
import java.nio.file.Files;
import java.nio.file.Path;
import java.util.Scanner;

class Tetris {
    public static void main(String[] args) {
        // Read 5 ints from config file (first arg path or ./config.txt)
        String configPath = (args.length > 0) ? args[0] : "config.txt";
        int[] config = new int[5];
        try {
            String content = Files.readString(Path.of(configPath));
            Scanner sc = new Scanner(content);
            for (int i = 0; i < 5; i++) {
                if (!sc.hasNextInt()) throw new IllegalArgumentException("Config must contain at least 5 ints");
                config[i] = sc.nextInt();
            }
            sc.close();
        } catch (IOException | IllegalArgumentException e) {
            System.err.println("Failed to read config file '" + configPath + "': " + e.getMessage());
            return;
        }

        System.out.print("Loaded config: [");
        for (int i = 0; i < config.length; i++) {
            System.out.print(config[i] + (i + 1 < config.length ? ", " : ""));
        }
        System.out.println("]");

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
        System.out.println("State evaluation: " + state.evaluate(config));
    }
}
