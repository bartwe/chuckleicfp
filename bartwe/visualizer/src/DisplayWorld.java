// Created: bartwe 13-7-12 15:53
public class DisplayWorld {


    public static void main(String[] args) {
        WorldState state = WorldState.loadFromDisk(args[0]);
        DisplayWindow window = new DisplayWindow(state.getN() + 2, state.getM() + 2, calcPixelShift(state.getN() + 2, state.getM() + 2));
        int[] raw = new int[window.height * window.width];

        state.render(raw);

        window.draw(raw);
    }

    private static int calcPixelShift(int a, int b) {
        if ((a == b) && (a == 0))
            return 0;
        int shift = 0;
        while (true) {
            if ((a << (shift+1)) > 1000)
                return shift;
            if ((b << (shift+1)) > 700)
                return shift;
            shift++;
        }
    }
}
