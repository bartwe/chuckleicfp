import com.sun.org.apache.xerces.internal.impl.xpath.XPath;

import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;

public class Manual implements KeyListener {
    DisplayWindow window;
    WorldState state;
    WorldState nextState;
    int[] raw;

    public static void main(String[] args) {
        Manual manual = new Manual(WorldState.loadFromDisk(args[0]));
    }

    public Manual(WorldState state) {
        this.state = state;
        nextState = state.copy();
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
        RobotAction action;
        switch (e.getKeyCode()) {
            case KeyEvent.VK_W:
            case KeyEvent.VK_UP:
                action = RobotAction.Up;
                break;
            case KeyEvent.VK_S:
            case KeyEvent.VK_DOWN:
                action = RobotAction.Down;
                break;
            case KeyEvent.VK_A:
            case KeyEvent.VK_LEFT:
                action = RobotAction.Left;
                break;
            case KeyEvent.VK_D:
            case KeyEvent.VK_RIGHT:
                action = RobotAction.Right;
                break;
            case KeyEvent.VK_SPACE:
                action = RobotAction.Wait;
                break;
            case KeyEvent.VK_ENTER:
                action = RobotAction.Abort;
                break;
            default:
                Toolkit.getDefaultToolkit().beep();
                return;
        }

        StepLogic sl = new StepLogic();
        sl.applyStep(state, nextState, action);
        WorldState flip = state;
        state = nextState;
        nextState = flip;
        state.render(raw);
        window.draw(raw);
    }

    public void keyReleased(KeyEvent e) {
    }
}
