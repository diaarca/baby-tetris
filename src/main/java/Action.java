public class Action {
    private Point position;
    private int rotation;

    public Action(Point position, int rotation) {
        this.position = position;
        this.rotation = rotation;
    }

    public Point getPosition() { return this.position; }

    public int getRotation() { return this.rotation; }
}
