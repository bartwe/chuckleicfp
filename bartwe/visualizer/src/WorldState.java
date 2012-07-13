import java.io.*;
import java.lang.reflect.Array;
import java.util.ArrayList;
import java.util.Arrays;

public abstract class WorldState {
    public abstract byte get(int x, int y);

    public abstract void set(int x, int y, byte cellState);

    public abstract int getN();

    public abstract int getM();

    public void render(int[] raw) {
        int n = getN();
        int m = getM();
        int idx = 0;
        for (int y = 0; y < m + 2; y++) {
            for (int x = 0; x < n + 2; x++) {
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

    private static WorldState readFromStream(InputStream stream) {
        int capacity = 16;
        ArrayList<byte[]> rows = new ArrayList<byte[]>();
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
                    rows.add(Arrays.copyOf(buffer, index));
                    index = 0;
                } else {
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
        for (int i = 0; i < m; i++)
            n = Math.max(n, rows.get(i).length);
        SolidWorldState result = new SolidWorldState(n, m);
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
}
