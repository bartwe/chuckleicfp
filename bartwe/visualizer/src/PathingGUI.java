import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.util.ArrayList;
import java.util.Collections;

public class PathingGUI implements KeyListener {
    final DisplayWindow window;
    WorldState state;
    final int[] raw;
    private final ArrayList<WorldState> path = new ArrayList<WorldState>();

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
                path.clear();
                DualAStarApproach.stop = false;
                Thread stopper = new Thread(new Runnable() {
                    public void run() {
                        try {
                            Thread.sleep(50000);
                        } catch (InterruptedException e1) {
                        }
                        DualAStarApproach.tryHarder = true;
                        try {
                            Thread.sleep(50000);
                        } catch (InterruptedException e1) {
                        }
                        DualAStarApproach.stop = true;
                    }
                });
                stopper.setDaemon(true);
                stopper.start();
                DualAStarApproach dasa = new DualAStarApproach();
                ArrayList<WorldState> highLevelPath = dasa.findPath(state);
                System.err.println("Path found.");
                System.err.println("Top layer size: " + highLevelPath.size());
                if (highLevelPath.size() > 0) {
                    WorldState cursor = highLevelPath.get(highLevelPath.size() - 1);
                    System.err.println("Final state score: " + cursor.score() + "  Reason:" + cursor.stepResult.toString() + " FinishOnAbort: " + (cursor.action == RobotAction.Abort));

                    while (cursor != null) {
                        path.add(cursor);
                        cursor = cursor.parent;
                    }
                    System.err.println("Base path size: " + path.size());
                    Collections.reverse(path);
                    System.err.println("Path:");
                    for (WorldState s : path) {
                        if (s.action != null)
                            System.err.print(s.action.letter);
                    }
                    System.err.println();
                    state = path.remove(0);
                } else
                    Toolkit.getDefaultToolkit().beep();
                break;
            case KeyEvent.VK_ENTER:
                if (path.size() > 0) {
                    state = path.remove(0);
                    if (path.size() == 0)
                        System.err.print("Done.");
                    else
                        System.err.print("Step.");
                    System.err.println("step: "+state.steps+" score: " + state.score() + "  Reason:" + state.stepResult.toString() + " waterLevel: " + state.curWaterLevel + " submergedSteps:" + state.submergedSteps);
                } else
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
