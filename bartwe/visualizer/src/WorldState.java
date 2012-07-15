import java.io.*;
import java.util.ArrayList;
import java.util.Arrays;

public abstract class WorldState {
    protected int steps = 0;

    protected int robotX, robotY;
    protected int exitX, exitY;
    protected int lambdaCollected;
    protected int lambdaRemaining;

    public int initWaterLevel = 0;
    public int curWaterLevel = 0;
    public int floodingFreq = 0;
    public int waterproof = 10;
    public int submergedSteps = 0;

    protected StepResult stepResult = StepResult.Ok;
    public WorldState parent;
    public RobotAction action;
    public TransporterMap map;
    boolean trampolined;

    public abstract byte get(int x, int y);

    public abstract void set(int x, int y, byte cellState);

    public abstract int getN();

    public abstract int getM();

    public void render(int[] raw) {
        int n = getN();
        int m = getM();
        int idx = 0;
        for (int y = m; y >= 1; y--) {
            for (int x = 1; x < n + 1; x++) {
                byte cell = get(x, y);
                raw[idx] = Cell.getColor(cell);
                idx++;
            }
        }
    }

    public static WorldState loadFromDisk(String filename) {
        try {
            FileInputStream fstream = new FileInputStream(filename);
            try {
                return readFromStream(fstream);
            } finally {
                fstream.close();
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }
    }

    static WorldState readFromStream(InputStream stream) {
        int capacity = 16;
        ArrayList<byte[]> rows = new ArrayList<byte[]>();
        boolean lastByteNewLine = false;
        try {
            byte[] buffer = new byte[capacity];
            int index = 0;
            while (true) {
                int c = stream.read();
                if (c == -1) {
                    if (index != 0)
                        rows.add(Arrays.copyOf(buffer, index));
                    break;
                } else if (c == 10) { // newline
                    if (lastByteNewLine) { //two newlines in a row means weather
                        break;
                    }
                    lastByteNewLine = true;
                    rows.add(Arrays.copyOf(buffer, index));
                    index = 0;
                } else {
                    if (c != 13)
                        lastByteNewLine = false;
                    byte state = Cell.charToState(c);
                    if (state != Cell.Invalid) {
                        if (buffer.length == index) {
                            capacity *= 2;
                            buffer = Arrays.copyOf(buffer, capacity);
                        }
                        buffer[index] = state;
                        index++;
                    }
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        //warning rows is in inverted order of m
        int n = 0;
        int m = rows.size();
        for (byte[] row : rows)
            n = Math.max(n, row.length);
        SolidWorldState result = new SolidWorldState(n, m);
        result.map = new TransporterMap();

        try {
            BufferedReader buf = new BufferedReader(new InputStreamReader(stream));
            String line;
            while ((line = buf.readLine()) != null) {
                String[] para = line.split(" ");
                if (para[0].equals("Water")) {
                    result.initWaterLevel = Integer.parseInt(para[1]);
                    result.curWaterLevel = Integer.parseInt(para[1]);
                }
                if (para[0].equals("Flooding")) {
                    result.floodingFreq = Integer.parseInt(para[1]);
                }
                if (para[0].equals("Waterproof")) {
                    result.waterproof = Integer.parseInt(para[1]);
                }
                if (para[0].equals("Trampoline")) {
                    assert para[1].length() == 1;
                    assert para[2].equals("targets");
                    assert para[3].length() == 1;
                    byte source = Cell.charToState(para[1].charAt(0));
                    byte target = Cell.charToState(para[3].charAt(0));
                    result.map.addMapping(source, target);
                }
            }
        } catch (IOException e) {
            throw new RuntimeException(e);
        }

        for (int i = 0; i < m; i++) {
            byte[] row = rows.get(i);
            int y = m - i;
            for (int j = 0; j < n; j++) {
                int x = j + 1;
                byte state;
                if (j >= row.length)
                    state = Cell.Empty;
                else
                    state = row[j];
                result.set(x, y, state);
            }
        }
        result.expensiveScan();
        return result;
    }

    public void copyFrom(WorldState current) {
        int n = getN();
        int m = getM();
        for (int y = 0; y < m + 2; y++) {
            for (int x = 0; x < n + 2; x++) {
                set(x, y, current.get(x, y));
            }
        }

        robotX = current.robotX;
        robotY = current.robotY;
        exitX = current.exitX;
        exitY = current.exitY;
        lambdaCollected = current.lambdaCollected;
        lambdaRemaining = current.lambdaRemaining;
        curWaterLevel = current.curWaterLevel;
        initWaterLevel = current.initWaterLevel;
        floodingFreq = current.floodingFreq;
        submergedSteps = current.submergedSteps;
        waterproof = current.waterproof;
        steps = current.steps;
        map = current.map;
        trampolined = false;
    }

    public abstract WorldState copy();

    public abstract WorldStateHash getHash();

    public abstract int score();

    public void doTransporter(byte targetTransporter) {
        int n = getN();
        int m = getM();
        boolean foundTarget = false;
        boolean[] cleanuplist = map.cleanupList(targetTransporter);
        for (int y = 1; y < m + 1; y++) {
            for (int x = 1; x < n + 1; x++) {
                byte cell = get(x, y);
                if (cell == targetTransporter) {
                    set(x, y, Cell.RobotLocation);
                    robotX = x;
                    robotY = y;
                    foundTarget = true;
                } else if (cell >= Cell.TrampolineSource1 && cell <= Cell.TrampolineSource9) {
                    if (cleanuplist[cell - Cell.TrampolineSource1])
                        set(x, y, Cell.Empty);
                }
            }
        }
        assert foundTarget;
        trampolined = true;
    }

    public boolean disallowNonGoalTransportMove(RobotAction action, int x, int y) {
        int rx = robotX + action.dx;
        int ry = robotY + action.dy;
        if ((rx < 1) || (rx > getN()) || (ry < 1) || (ry > getM()))
            return true;
        byte cell = get(rx, ry);
        if ((cell >= Cell.TrampolineSource1) && (cell <= Cell.TrampolineSource9))
        {
            if ((x == rx) && (y == ry))
                return false;
            return true;
        }
        return false;
    }
}

