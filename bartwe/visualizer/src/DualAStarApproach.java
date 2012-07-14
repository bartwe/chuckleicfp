import javax.sound.midi.ControllerEventListener;
import java.awt.geom.Path2D;
import java.util.ArrayList;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;

public class DualAStarApproach {
/*

    A: number of nodes in the top layer that a node may branch out to
    B: radius(manhattan) of spaces

    At the top level, insert the start position as a node
    Greedily pick A neighbour lambdas within radius B
    Do multiple bottom layer A* search from the SP to the NP, limit them to C steps of depth and D steps of breadth, process the A* calculations interleaved to have an early out if say 1/3 of them have a solution


    The bottom layer a* search is a straigh pathfinding from SP to NP
    It is limited in depth and breadth, it may be stopped if other searches find results earlier.
    The open/closed set is keyed by a (sha1?) hash of the worldstates

    The point of the open/closed keying is to detect multiple paths to a point, and pick the on with the least steps.
*/

    ArrayList<WorldState> findPath(WorldState initialState) {
        final int horizon = initialState.getM() * initialState.getN();
        AStar pathfinder = new AStar(new AStar.Controller() {
            public int getAdjacent(WorldState state, WorldState[] adjacentsBuffer) {
                if (state.stepResult != StepResult.Ok)
                    return 0;
                return findInnerPaths(state, adjacentsBuffer);
            }

            public double goalHeuristic(WorldState state) {
                return (state.stepResult == StepResult.Win) ? 0 : 1; // no know goal know, return something low
            }

            public double edgeCost(WorldState from, WorldState to) {
                return -(to.score() - from.score());
            }

            public boolean isEndPoint(WorldState state) {
                return state.stepResult == StepResult.Win;
            }

            public AStar.AStarNode findBest(AStar.AStarNode best, boolean singleSolution, HashMap<WorldStateHash, AStar.AStarNode> nodes) {
                if (best.state.stepResult == StepResult.Win)
                    return best;
                double bestScore = best.state.score();
                for (AStar.AStarNode node : nodes.values()) {
                    double score = node.state.score();
                    if ((score > bestScore) || ((score == bestScore) && (node.gScore < best.gScore))) {
                        bestScore = score;
                        best = node;
                    }
                }
                return best;
            }

            public double horizon() {
                return horizon;
            }
        }, initialState);

        while (!pathfinder.run()) {
        }
        return pathfinder.getResult();
    }

    class LambdaGoal {
        public WorldState state;
        public int x;
        public int y;
        public int radius;

        public AStar pathfinder;
        public boolean done;

        public LambdaGoal(WorldState state, int x_, int y_, int radius) {
            this.state = state;
            this.x = x_;
            this.y = y_;
            this.radius = radius;
        }
    }

    private void findLambdas(WorldState state, ArrayList<LambdaGoal> goals) {
        int radius = 1;
        int n = state.getN();
        int m = state.getM();
        while (true) {

            for (int y = -radius; y <= radius; y++) {
                int y_ = y + state.robotY;
                if ((y_ < 1) || (y_ > m))
                    continue;
                for (int x = -radius; x <= radius; x++) {
                    int x_ = x + state.robotX;
                    if ((x_ < 1) || (x_ > n))
                        continue;
                    if (Math.abs(x_) + Math.abs(y) != radius)
                        continue; // zomg, expensive
                    if (state.get(x_, y_) == Cell.Lambda)
                        goals.add(new LambdaGoal(state, x_, y_, radius));
                }
            }

            if ((goals.size() >= 10) || (goals.size() == state.lambdaRemaining))
                break;
            if ((radius >= n) && (radius >= m))
                break;
            radius++;
        }
        Collections.sort(goals, new Comparator<LambdaGoal>() {
            public int compare(LambdaGoal o1, LambdaGoal o2) {
                return o1.radius - o2.radius;
            }
        });
    }

    private int findInnerPaths(WorldState initialState, WorldState[] adjacentsBuffer) {
        int results = 0;
        final int horizon = initialState.getM() * initialState.getN();
        final StepLogic logic = new StepLogic();

        ArrayList<LambdaGoal> goals = new ArrayList<LambdaGoal>();

        if (initialState.lambdaRemaining > 0)
        findLambdas(initialState, goals);
        else
            goals.add(new LambdaGoal(initialState, initialState.exitX, initialState.exitY, 0));

        //find a number of lambdas to try and reach, path find them concurrently, stop looking if you find enough of them, abandon rest.
        for (LambdaGoal goal : goals) {
            final LambdaGoal lg = goal;
            goal.pathfinder = new AStar(new AStar.Controller() {
                public int getAdjacent(WorldState state, WorldState[] adjacentsBuffer) {
                    if (state.stepResult != StepResult.Ok)
                        return 0;
                    int count = 0;
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Abort);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Left);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Right);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Up);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Down);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Wait);
                    return count;
                }

                int addToBuffer(WorldState state, int count, WorldState[] adjacentsBufffer, RobotAction action) {
                    WorldState next = state.copy();
                    logic.applyStep(state, next, action);
                    if (next.stepResult == StepResult.MoveFail)
                        return count;
                    adjacentsBufffer[count] = next;
                    return count + 1;
                }

                public double goalHeuristic(WorldState state) {
                    return Math.abs(state.robotX - lg.x) + Math.abs(state.robotY - lg.y);
                }

                public double edgeCost(WorldState from, WorldState to) {
                    return 1; // its about shortest path, ignore score
                }

                public boolean isEndPoint(WorldState state) {
                    return Math.abs(state.robotX - lg.x) + Math.abs(state.robotY - lg.y) == 0;
                }

                public AStar.AStarNode findBest(AStar.AStarNode best, boolean singleSolution, HashMap<WorldStateHash, AStar.AStarNode> nodes) {
                    if (!singleSolution)
                        return null;
                    if (isEndPoint(best.state))
                        return best;
                    return null; // no runner up solutions, we just want to go to the specified lambdas
                }

                public double horizon() {
                    return horizon;
                }
            }, goal.state);
        }

        while (true) {
            boolean active = false;
            for (LambdaGoal goal : goals) {
                if (goal.done)
                    continue;
                if (goal.pathfinder.run()) {
                    goal.done = true;
                    ArrayList<WorldState> path = goal.pathfinder.getResult();
                    if ((path != null)&&(path.size()>1) && (results < adjacentsBuffer.length))
                    {
                        adjacentsBuffer[results] = path.get(path.size()-1);
                        results++;
                    }
                } else
                    active = true;
            }
            if (!active)
                break;
            if (results > 10)
                break;
        }

        return results;
    }


}
