public class Action {
    public Point position;
    public int rotation;

    public Action(Point position, int rotation) {
        this.position = position;
        this.rotation = rotation;
    }

    public Point getPosition() {
        return this.position;
    }

    public int getRotation() {
        return this.rotation;
    }
}