import java.util.ArrayList;
import java.util.Collections;

public class DasaConsole {

    public static void main(String[] args)
    {
        try
        {
            WorldState initialState = WorldState.readFromStream(System.in);
            DualAStarApproach dasa = new DualAStarApproach();
            ArrayList<WorldState> highLevelPath = dasa.findPath(initialState);
            if (highLevelPath.size() > 0) {
                WorldState cursor = highLevelPath.get(highLevelPath.size() - 1);
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
            }
            else
                System.out.println("A");
        }
        catch (Exception e)
        {
            if (args.length > 0)
                e.printStackTrace();
            System.out.println("A");
        }
    }
}
