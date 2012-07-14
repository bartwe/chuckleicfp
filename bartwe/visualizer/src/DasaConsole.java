import java.util.ArrayList;
import java.util.Collections;

public class DasaConsole {

    static DualAStarApproach dasa;

    public static void main(String[] args) {
        Runtime.getRuntime().addShutdownHook(new Thread() {
            @Override
            public void run() {
                dasa.stop();
            }
        });

        try {
            WorldState initialState = WorldState.readFromStream(System.in);
            dasa = new DualAStarApproach();
            ArrayList<WorldState> highLevelPath = dasa.findPath(initialState);
            printPath(highLevelPath);
        } catch (Exception e) {
            if (args.length > 0)
                e.printStackTrace();
            System.out.println("A");
        }
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
