import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;

public class PathingGUI implements KeyListener {
    DisplayWindow window;
    WorldState state;
    int[] raw;
    private ArrayList<WorldState> path;

    public static void main(String[] args) {
        PathingGUI manual = new PathingGUI(WorldState.loadFromDisk(args[0]));
    }

    public PathingGUI(WorldState state) {
        this.state = state;
        window = new DisplayWindow(state.getN(), state.getM(), calcPixelShift(state.getN(), state.getM()));
        raw = new int[window.height * window.width];
        state.render(raw);
        window.draw(raw);
        window.setKeyListener(this);
    }

    private static int calcPixelShift(int a, int b) {
        if ((a == b) && (a == 0))
            return 0;
        int shift = 0;
        while (true) {
            if ((a << (shift + 1)) > 1000)
                return shift;
            if ((b << (shift + 1)) > 700)
                return shift;
            shift++;
        }
    }

    public void keyTyped(KeyEvent e) {
    }

    public void keyPressed(KeyEvent e) {
        switch (e.getKeyCode()) {
            case KeyEvent.VK_SPACE:
                DualAStarApproach dasa = new DualAStarApproach();
                path = dasa.findPath(state);
                if (path.size() > 0)
                    state = path.remove(0);
                else
                    Toolkit.getDefaultToolkit().beep();
                break;
            case KeyEvent.VK_ENTER:
                if (path.size() > 0)
                    state = path.remove(0);
                else
                    Toolkit.getDefaultToolkit().beep();
                break;
            default:
                Toolkit.getDefaultToolkit().beep();
                return;
        }

        state.render(raw);
        window.draw(raw);
    }

    public void keyReleased(KeyEvent e) {
    }
}
