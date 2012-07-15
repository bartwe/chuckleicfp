import sun.misc.Signal;
import sun.misc.SignalHandler;

import java.util.ArrayList;
import java.util.Collections;

public class DasaConsole {

    static DualAStarApproach dasa;

    public static void main(String[] args) {
        sun.misc.Signal.handle(new Signal("INT"), new SignalHandler() {
            public void handle(Signal signal) {
                DualAStarApproach.stop();
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                }
            }
        });
        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                DualAStarApproach.stop();
                try {
                    Thread.sleep(1000);
                } catch (InterruptedException e) {
                }
            }
        });

        Thread oomWatcher = new Thread(new Runnable() {
            public void run() {
                try {
                    while (true) {
                        long memInUse = Runtime.getRuntime().totalMemory() - Runtime.getRuntime().freeMemory();
                        if (memInUse > 600 * 1000 * 1000) {
                            DualAStarApproach.stop();

                            Runtime.getRuntime().gc();
                            Runtime.getRuntime().gc();
                            return;
                        }
                        Thread.sleep(100);
                    }
                } catch (InterruptedException e) {
                }
            }
        });
        oomWatcher.setDaemon(true);
        oomWatcher.start();

        try {
            String file = null;
            for (String arg : args)
                if (!arg.startsWith("-"))
                    file = arg;

            WorldState initialState;

            if (file == null)
                initialState = WorldState.readFromStream(System.in);
            else
                initialState = WorldState.loadFromDisk(file);
            dasa = new DualAStarApproach();
            ArrayList<WorldState> highLevelPath = dasa.findPath(initialState);
            printPath(highLevelPath);
        } catch (Throwable e) {
            for (String arg : args)
                if (arg.equals("-e"))
                    e.printStackTrace();
            System.out.println("A");
        }
        System.out.flush();
    }

    static void printPath(ArrayList<WorldState> rawPath) {
        if (rawPath.size() > 0) {
            WorldState cursor = rawPath.get(rawPath.size() - 1);
			System.out.println("# score: " + cursor.score());
            ArrayList<WorldState> path = new ArrayList<WorldState>();
            while (cursor != null) {
                path.add(cursor);
                cursor = cursor.parent;
            }
            Collections.reverse(path);
            while (path.size() > 0) {
                WorldState state = path.remove(0);
                if (state.action != null)
                    System.out.print(state.action.letter);
            }
            System.out.println();
        } else
            System.out.println("A");
    }
}
