import java.util.Arrays;

public class SolidWorldState extends WorldState {
    byte[][] data;
    int n, m;

    public int getN() {
        return n;
    }

    public int getM() {
        return m;
    }

    public SolidWorldState(int n, int m) {
        this.n = n;
        this.m = m;
        data = new byte[m + 2][];
        for (int i = 0; i < m + 2; i++) {
            byte[] row = new byte[n + 2];
            data[i] = row;
            if ((i == 0) || (i == m + 1)) {
                for (int j = 1; j < n + 1; j++)
                    row[j] = Cell.Wall;
            } else {
                for (int j = 1; j < n + 1; j++)
                    row[j] = Cell.Empty;
            }
            row[0] = Cell.Wall;
            row[n + 1] = Cell.Wall;
        }
    }

    public byte get(int x, int y) {
        assert Cell.validState(data[y][x]);
        return data[y][x];
    }

    public void set(int x, int y, byte state) {
        assert Cell.validState(state);
        data[y][x] = state;
    }

    public void expensiveScan() {
        lambdaCollected = 0;
        lambdaRemaining = 0;
        int n = getN();
        int m = getM();
        for (int y = 0; y < m + 2; y++) {
            for (int x = 0; x < n + 2; x++) {
                byte cell = get(x, y);
                switch (cell) {
                    case Cell.RobotLocation:
                        robotX = x;
                        robotY = y;
                        break;
                    case Cell.ClosedLambdaLift:
                        exitX = x;
                        exitY = y;
                        break;
                    case Cell.Lambda:
                        lambdaRemaining++;
                        break;
                    default:
                }
            }
        }
    }

    public WorldState copy() {
        SolidWorldState result = new SolidWorldState(n, m);
        result.data = new byte[m + 2][];
        for (int i = 0; i < m + 2; i++) {
            result.data[i] = Arrays.copyOf(data[i], data[i].length);
        }
        return result;
    }
}
