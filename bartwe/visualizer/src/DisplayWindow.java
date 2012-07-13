import javax.swing.*;
import java.awt.*;
import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import java.awt.image.*;

public class DisplayWindow {
    static final long serialVersionUID = 1;

    DisplayFrame frame;

    public int width;
    public int height;

    public DisplayWindow(int width, int height, int scaleShift) {
        this.width = width;
        this.height = height;
        frame = new DisplayFrame(width, height, scaleShift);
    }

    void draw(int[] raw) {
        frame.draw(raw);
    }

    private class DisplayFrame extends JFrame implements KeyListener {
        static final long serialVersionUID = 1;

        private BufferedImage shownImage;
        private int width;
        private int height;
        private int scaleShift;

        public DisplayFrame(int width, int height, int scaleShift) {

            this.width = width << scaleShift;
            this.height = height << scaleShift;
            this.scaleShift = scaleShift;

            setTitle("Graphics Test");

            shownImage = new BufferedImage(this.width, this.height,
                    BufferedImage.TYPE_INT_RGB);

            setLayout(new FlowLayout());
            add(new DisplayPanel());
            pack();
            setVisible(true);

            setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);

            addKeyListener(this);

            drawShownImage();
        }

        void draw(int[] raw) {
            // teh slowness
            if (scaleShift == 0) {
                for (int y = 0; y < height; y++) {
                    int ly = y * width;
                    for (int x = 0; x < width; x++) {
                        int c = raw[ly + x];
                        shownImage.setRGB(x, y, c);
                    }
                }
            } else {
                for (int y = 0; y < height; y++) {
                    int ly = (y >> scaleShift) * (width  >> scaleShift);
                    for (int x = 0; x < width; x++) {
                        int c = raw[ly + (x >> scaleShift)];
                        shownImage.setRGB(x, y, c);
                    }
                }
            }
            drawShownImage();
        }

        public void drawShownImage() {
            repaint();
        }

        public void keyPressed(KeyEvent e) {
            if (e.getKeyCode() == KeyEvent.VK_DOWN) {
            }
            if (e.getKeyCode() == KeyEvent.VK_UP) {
            }
            if (e.getKeyCode() == KeyEvent.VK_END) {
            }
            if (e.getKeyCode() == KeyEvent.VK_RIGHT) {
            }
            if (e.getKeyCode() == KeyEvent.VK_LEFT) {
            }
            drawShownImage();
        }

        public void keyTyped(KeyEvent e) {

        }

        public void keyReleased(KeyEvent e) {

        }

        class DisplayPanel extends JPanel {
            static final long serialVersionUID = -1;

            public Dimension getPreferredSize() {
                return new Dimension(width, height);
            }

            public void paintComponent(Graphics g) {
                super.paintComponent(g);
                g.drawImage(shownImage, 0, 0, width, height, null);
            }
        }
    }
}