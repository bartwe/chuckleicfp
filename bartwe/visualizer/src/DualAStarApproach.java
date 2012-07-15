import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;

public class DualAStarApproach {
    public static boolean stop;
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
                if (stop)
                    return 0;
                if (state.stepResult != StepResult.Ok)
                    return 0;
                return findInnerPaths(state, adjacentsBuffer);
            }

            public double goalHeuristic(WorldState state) {
                return state.lambdaRemaining;
            }

            public double edgeCost(WorldState from, WorldState to) {
                assert to.steps - from.steps > 0;
                return Math.sqrt(to.steps - from.steps);
            }

            public boolean isEndPoint(WorldState state) {
                return false;
            }

            public AStar.AStarNode findBest(AStar.AStarNode best, boolean singleSolution, HashMap<WorldStateHash, AStar.AStarNode> nodes) {
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
        ArrayList<WorldState> path = pathfinder.getResult();
        if (path != null && path.size() > 0) {
            WorldState tail = path.get(path.size() - 1);
            if (tail.stepResult == StepResult.Ok) {
                WorldState state = tail.copy();
                state.copyFrom(tail);
                state.parent = tail;
                state.action = RobotAction.Abort;
                state.stepResult = StepResult.Abort;
                path.add(state);
            }
        }
        return path;
    }

    public static void stop() {
        stop = true;
    }

    class Goal {
        public final WorldState state;
        public final int x;
        public final int y;

        public AStar pathfinder;
        public boolean done;
        public final boolean trampoline;

        public Goal(WorldState state, int x_, int y_, boolean trampoline) {
            this.state = state;
            this.x = x_;
            this.y = y_;
            this.trampoline = trampoline;
        }
    }

    static int[] radiusScanX;
    static int[] radiusScanY;

    // maybe cull for non square regions ?
    private static void buildRadiusScan(int n, int m) {
        radiusScanX = new int[4225];
        radiusScanY = new int[4225];
        int ix = 0;

        int radius = 1;
        while (radius < 32) {
            if (stop)
                break;
            for (int y = -radius; y <= radius; y++) {
                if ((y < -m) || (y > m))
                    continue;
                for (int x = -radius; x <= radius; x++) {
                    if ((x < -n) || (x > n))
                        continue;
                    if (Math.abs(x) + Math.abs(y) != radius)
                        continue; // zomg, expensive
                    radiusScanX[ix] = x;
                    radiusScanY[ix] = y;
                    ix++;
                }
            }
            if ((radius >= n) && (radius >= m))
                break;
            radius++;
        }

        if (ix < radiusScanX.length) {
            radiusScanX = Arrays.copyOf(radiusScanX, ix);
        }
    }


    private void findLambdas(WorldState state, ArrayList<Goal> goals) {
        int n = state.getN();
        int m = state.getM();
        if (radiusScanX == null)
            buildRadiusScan(n, m);
        int limit = state.lambdaRemaining;
        if (limit > 10)
            limit = 10;
        for (int idx = 0; idx < radiusScanX.length; ++idx) {
            if (stop)
                break;
            int x = radiusScanX[idx];
            int y = radiusScanY[idx];
            int y_ = y + state.robotY;
            if ((y_ < 1) || (y_ > m))
                continue;
            int x_ = x + state.robotX;
            if ((x_ < 1) || (x_ > n))
                continue;
            if (state.get(x_, y_) == Cell.Lambda)
                goals.add(new Goal(state, x_, y_, false));
            if (state.get(x_, y_) == Cell.HighOrderRock)
                goals.add(new Goal(state, x_, y_, false));

            if (goals.size() >= limit)
                break;
        }
    }


    private void findTransporters(WorldState state, ArrayList<Goal> goals) {
        int n = state.getN();
        int m = state.getM();
        if (radiusScanX == null)
            buildRadiusScan(n, m);
        int startSize = goals.size();
        int limit = state.numberOfTransporters;
        if (limit > 10)
            limit = 10;
        for (int idx = 0; idx < radiusScanX.length; ++idx) {
            if (stop)
                break;
            int x = radiusScanX[idx];
            int y = radiusScanY[idx];
            int y_ = y + state.robotY;
            if ((y_ < 1) || (y_ > m))
                continue;
            int x_ = x + state.robotX;
            if ((x_ < 1) || (x_ > n))
                continue;
            if (Cell.isTransporter(state.get(x_, y_)))
                goals.add(new Goal(state, x_, y_, true));

            if (goals.size() - startSize >= limit)
                break;
        }
    }

    private int findInnerPaths(WorldState initialState, WorldState[] adjacentsBuffer) {
        int results = 0;
        final int horizon = initialState.getM() * initialState.getN();
        final StepLogic logic = new StepLogic();

        ArrayList<Goal> goals = new ArrayList<Goal>();

        if (initialState.lambdaRemaining > 0) {
            findLambdas(initialState, goals);
        }
        else
            goals.add(new Goal(initialState, initialState.exitX, initialState.exitY, false));

        findTransporters(initialState, goals);

        //find a number of lambdas to try and reach, path find them concurrently, stop looking if you find enough of them, abandon rest.
        for (Goal goal : goals) {
            final Goal lg = goal;
            goal.pathfinder = new AStar(new AStar.Controller() {
                public int getAdjacent(WorldState state, WorldState[] adjacentsBuffer) {
                    if (stop)
                        return 0;
                    if (state.stepResult != StepResult.Ok)
                        return 0;
                    int count = 0;
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Left);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Right);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Up);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Down);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Wait);
                    count = addToBuffer(state, count, adjacentsBuffer, RobotAction.Shave);
                    return count;
                }

                int addToBuffer(WorldState state, int count, WorldState[] adjacentsBufffer, RobotAction action) {
                    if (state.disallowNonGoalTransportMove(action, lg.x, lg.y))
                        return count;
                    WorldState next = state.copy();
                    logic.applyStep(state, next, action);
                    if (next.stepResult == StepResult.MoveFail)
                        return count;
                    if (state.trampolined && !lg.trampoline)
                        state.disallowNonGoalTransportMove(action, lg.x, lg.y);
                    adjacentsBufffer[count] = next;
                    return count + 1;
                }

                public double goalHeuristic(WorldState state) {
                    if (state.trampolined) {
                        assert lg.trampoline;
                        return 0;
                    }
                    return Math.abs(state.robotX - lg.x) + Math.abs(state.robotY - lg.y);
                }

                public double edgeCost(WorldState from, WorldState to) {
                    assert to.steps - from.steps == 1;
                    return 1;
                }

                public boolean isEndPoint(WorldState state) {
                    if (state.trampolined) {
                        assert lg.trampoline;
                        return true;
                    }
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
            for (Goal goal : goals) {
                if (goal.done)
                    continue;
                if (goal.pathfinder.run()) {
                    goal.done = true;
                    ArrayList<WorldState> path = goal.pathfinder.getResult();
                    if ((path != null) && (path.size() > 1) && (results < adjacentsBuffer.length)) {
                        adjacentsBuffer[results] = path.get(path.size() - 1);
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
