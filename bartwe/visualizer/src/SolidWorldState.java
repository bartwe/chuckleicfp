import java.nio.ByteBuffer;
import java.security.MessageDigest;

public class SolidWorldState extends WorldState {
    byte[][] data;
    final int n;
    final int m;
    private WorldStateHash hash;


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
        if (get(x, y) == 11 && state != 11)
            data[y][x] = state;
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
                    case Cell.HighOrderRock:
                        lambdaRemaining++;
                        break;
                    default:
                }
                if (Cell.isTransporter(cell))
                    numberOfTransporters++;
            }
        }
    }

    public WorldState copy() {
        SolidWorldState result = new SolidWorldState(n, m);
        result.data = new byte[m + 2][];
        for (int i = 0; i < m + 2; i++) {
            result.data[i] = new byte[n+2];
        }

        result.robotX = robotX;
        result.robotY = robotY;
        result.exitX = exitX;
        result.exitY = exitY;
        result.lambdaCollected = lambdaCollected;
        result.lambdaRemaining = lambdaRemaining;
        result.numberOfTransporters = numberOfTransporters;
        result.curWaterLevel = curWaterLevel;
        result.initWaterLevel = initWaterLevel;
        result.floodingFreq = floodingFreq;
        result.submergedSteps = submergedSteps;
        result.waterproof = waterproof;
        result.steps = steps;
        result.map = map;
        result.trampolined = false; //is carried over manually
        result.razors = razors;
        result.growth = growth;
        return result;
    }

    public void copyFrom(WorldState current) {
        int n = getN();
        int m = getM();
        if (current instanceof SolidWorldState) {
            SolidWorldState sws = (SolidWorldState) current;
            for (int i = 0; i < m + 2; i++) {
                System.arraycopy(sws.data[i], 0, data[i], 0, data[i].length);
            }
        } else {
            for (int y = 0; y < m + 2; y++) {
                for (int x = 0; x < n + 2; x++) {
                    set(x, y, current.get(x, y));
                }
            }
        }

        robotX = current.robotX;
        robotY = current.robotY;
        exitX = current.exitX;
        exitY = current.exitY;
        lambdaCollected = current.lambdaCollected;
        lambdaRemaining = current.lambdaRemaining;
        numberOfTransporters = current.numberOfTransporters;
        curWaterLevel = current.curWaterLevel;
        initWaterLevel = current.initWaterLevel;
        floodingFreq = current.floodingFreq;
        submergedSteps = current.submergedSteps;
        steps = current.steps;
        map = current.map;
        trampolined = false;
        razors = current.razors;
        growth = current.growth;
    }

    static MessageDigest md = null;

    @Override
    public WorldStateHash getHash() {
        if (hash != null)
            return hash;
        if (md == null) {
            // not thread safe, yay
            try {
                md = MessageDigest.getInstance("SHA-1");
            } catch (Exception e) {
                throw new RuntimeException(e);
            }
        }
        md.reset();
        ByteBuffer bb = ByteBuffer.allocate(3 * 4);
        bb.putInt(curWaterLevel);
        bb.putInt(submergedSteps);
        bb.putInt(razors);
        bb.flip();
        md.update(bb);
        for (byte[] row : data)
            md.update(row);
        hash = new WorldStateHash(md.digest());
        return hash;
    }

    public int score() {
        return lambdaCollected * 25 + ((stepResult == StepResult.Abort) ? (lambdaCollected * 25) : 0) + ((stepResult == StepResult.Win) ? (lambdaCollected * 50) : 0) - steps;
    }
}
