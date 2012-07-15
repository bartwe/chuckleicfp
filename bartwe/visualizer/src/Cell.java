public class Cell {
    public static final byte Invalid = 0;
    public static final byte Empty = 1;
    public static final byte Wall = 2;
    public static final byte Rock = 3;
    public static final byte Lambda = 4;
    public static final byte ClosedLambdaLift = 5;
    public static final byte Earth = 6;
    public static final byte RobotLocation = 7;
    public static final byte Beard = 8;
    public static final byte Razor = 9;
    public static final byte HighOrderRock = 10;

    public static final byte TrampolineTarget1 = 11;
    public static final byte TrampolineTarget2 = 12;
    public static final byte TrampolineTarget3 = 13;
    public static final byte TrampolineTarget4 = 14;
    public static final byte TrampolineTarget5 = 15;
    public static final byte TrampolineTarget6 = 16;
    public static final byte TrampolineTarget7 = 17;
    public static final byte TrampolineTarget8 = 18;
    public static final byte TrampolineTarget9 = 19;

    public static final byte TrampolineSource1 = 21;
    public static final byte TrampolineSource2 = 22;
    public static final byte TrampolineSource3 = 23;
    public static final byte TrampolineSource4 = 24;
    public static final byte TrampolineSource5 = 25;
    public static final byte TrampolineSource6 = 26;
    public static final byte TrampolineSource7 = 27;
    public static final byte TrampolineSource8 = 28;
    public static final byte TrampolineSource9 = 29;

    private static final byte HighestState = TrampolineSource9;


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
            case Cell.TrampolineSource1:
            case Cell.TrampolineSource2:
            case Cell.TrampolineSource3:
            case Cell.TrampolineSource4:
            case Cell.TrampolineSource5:
            case Cell.TrampolineSource6:
            case Cell.TrampolineSource7:
            case Cell.TrampolineSource8:
            case Cell.TrampolineSource9:
                return 0x0000ff;
            case Cell.TrampolineTarget1:
            case Cell.TrampolineTarget2:
            case Cell.TrampolineTarget3:
            case Cell.TrampolineTarget4:
            case Cell.TrampolineTarget5:
            case Cell.TrampolineTarget6:
            case Cell.TrampolineTarget7:
            case Cell.TrampolineTarget8:
            case Cell.TrampolineTarget9:
                return 0x8000ff;
            case Cell.Razor:
                return 0x00ff80;
            case Cell.Beard:
                return 0x80ff00;
            case Cell.HighOrderRock:
                return 0xff8080;
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
            case 79: // O
                return Cell.ClosedLambdaLift; // open lambda, but i don't distinguish
            case 46: // .
                return Cell.Earth;
            case 32: // <space>
                return Cell.Empty;

            case 49: // 1
                return Cell.TrampolineTarget1;
            case 50: // 2
                return Cell.TrampolineTarget2;
            case 51: // 3
                return Cell.TrampolineTarget3;
            case 52: // 4
                return Cell.TrampolineTarget4;
            case 53: // 5
                return Cell.TrampolineTarget5;
            case 54: // 6
                return Cell.TrampolineTarget6;
            case 55: // 7
                return Cell.TrampolineTarget7;
            case 56: // 8
                return Cell.TrampolineTarget8;
            case 57: // 9
                return Cell.TrampolineTarget9;

            case 65: // A
                return Cell.TrampolineSource1;
            case 66: // B
                return Cell.TrampolineSource2;
            case 67: // C
                return Cell.TrampolineSource3;
            case 68: // D
                return Cell.TrampolineSource4;
            case 69: // E
                return Cell.TrampolineSource5;
            case 70: // F
                return Cell.TrampolineSource6;
            case 71: // G
                return Cell.TrampolineSource7;
            case 72: // H
                return Cell.TrampolineSource8;
            case 73: // I
                return Cell.TrampolineSource9;

            case 87: // W
                return Cell.Beard;
            case 33: // !
                return Cell.Razor;
            case 64: // @
                return Cell.HighOrderRock;

            case 13:
                return Cell.Invalid;
            default:
                assert false;
                return Cell.Invalid;
        }
    }

    public static boolean isEmptyEarthLambdaLiftTransporterRazor(byte cell) {
        switch (cell) {
            case Cell.Empty:
                return true;
            case Cell.Earth:
                return true;
            case Cell.Lambda:
                return true;
            case Cell.ClosedLambdaLift:
                return true;
            case Cell.TrampolineSource1:
                return true;
            case Cell.TrampolineSource2:
                return true;
            case Cell.TrampolineSource3:
                return true;
            case Cell.TrampolineSource4:
                return true;
            case Cell.TrampolineSource5:
                return true;
            case Cell.TrampolineSource6:
                return true;
            case Cell.TrampolineSource7:
                return true;
            case Cell.TrampolineSource8:
                return true;
            case Cell.TrampolineSource9:
                return true;
            case Cell.Razor:
                return true;
            default:
                return false;
        }
    }

    public static boolean isEmpty(byte cell) {
        return cell == Cell.Empty;
    }

    public static boolean isLambda(byte cell) {
        return cell == Cell.Lambda;
    }

    public static boolean isTransporter(byte cell) {
        switch (cell) {
            case Cell.TrampolineSource1:
                return true;
            case Cell.TrampolineSource2:
                return true;
            case Cell.TrampolineSource3:
                return true;
            case Cell.TrampolineSource4:
                return true;
            case Cell.TrampolineSource5:
                return true;
            case Cell.TrampolineSource6:
                return true;
            case Cell.TrampolineSource7:
                return true;
            case Cell.TrampolineSource8:
                return true;
            case Cell.TrampolineSource9:
                return true;
            default:
                return false;
        }
    }

    public static boolean isRockish(byte b) {
        return (b == Cell.Rock) || (b == Cell.HighOrderRock);
    }

    public static boolean notEmptyNorRobot(byte b) {
        return (b != Cell.Empty) && (b != Cell.RobotLocation);
    }
}
