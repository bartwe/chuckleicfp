public class RobotAction {
    public static final RobotAction Left = new RobotAction('L', -1, 0);
    public static final RobotAction Right = new RobotAction('R', 1, 0);
    public static final RobotAction Up = new RobotAction('U', 0, 1);
    public static final RobotAction Down = new RobotAction('D', 0, -1);
    public static final RobotAction Wait = new RobotAction('W', 0, 0);
    public static final RobotAction Abort = new RobotAction('A', 0, 0);

    char letter;
    int dx;
    int dy;

    private RobotAction(char letter, int dx, int dy) {
        this.letter = letter;
        this.dx = dx;
        this.dy = dy;
    }
}
