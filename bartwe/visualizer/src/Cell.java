public class Cell {
    public static final byte Invalid = 0;
    public static final byte Empty = 1;
    public static final byte Wall = 2;
    public static final byte Rock = 3;
    public static final byte Lambda = 4;
    public static final byte ClosedLambdaLift = 5;
    public static final byte Earth = 6;
    public static final byte RobotLocation = 7;

    private static final byte HighestState = RobotLocation;


    public static boolean validState(byte b) {
        return b >= Empty && b <= HighestState;
    }

    public static int getColor(byte cell) {
        switch (cell) {
            case Cell.Empty:
                return 0xffffff;
            case Cell.Wall:
                return 0x000000;
            case Cell.Rock:
                return 0xff0000;
            case Cell.Lambda:
                return 0x00ffff;
            case Cell.ClosedLambdaLift:
                return 0xff00ff;
            case Cell.Earth:
                return 0x00ff00;
            case Cell.RobotLocation:
                return 0xffff00;
            default:
                assert false;
                return 0x808080;
        }
    }

    public static byte charToState(int c) {
        switch (c) {
            case 82: // R
                return Cell.RobotLocation;
            case 35: // #
                return Cell.Wall;
            case 42: // *
                return Cell.Rock;
            case 92: // \
                return Cell.Lambda;
            case 76: // L
                return Cell.ClosedLambdaLift;
            case 46: // .
                return Cell.Earth;
            case 32: // <space>
                return Cell.Empty;
            case 13:
                return Cell.Invalid;
            default:
                assert false;
                return Cell.Invalid;
        }
    }
}
