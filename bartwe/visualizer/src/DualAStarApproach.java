import javax.sound.midi.ControllerEventListener;

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

    void findPath(WorldState initialState) {
        final int horizon = initialState.getM() * initialState.getN();
        AStar pathfinder = new AStar(new AStar.Controller() {
            public int getAdjacent(WorldState state, WorldState[] adjacentsBuffer) {
                return findInnerPaths(state, adjacentsBuffer);
            }

            public double goalHeuristic(WorldState state) {
                return 0; // no know goal know, return something low
            }

            public double edgeCost(WorldState from, WorldState to) {
                return -(to.score() - from.score());
            }

            public boolean isEndPoint(WorldState state) {
                return state.stepResult != StepResult.Ok;
            }

            public double horizon() {
                return horizon;
            }
        }, initialState);

        while (!pathfinder.run()) {
        }
        pathfinder.getResult();

    }

    private int findInnerPaths(WorldState initialState, WorldState[] adjacentsBuffer) {

        //find a number of lambdas to try and reach, path find them concurrently, stop looking if you find enough of them, abandon rest.
        {

            final int horizon = initialState.getM() * initialState.getN();
            final StepLogic logic = new StepLogic();
            final int goalX = -1;
            final int goalY = -1;
            AStar pathfinder = new AStar(new AStar.Controller() {
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
                    return Math.abs(state.robotX - goalX) + Math.abs(state.robotY - goalY);
                }

                public double edgeCost(WorldState from, WorldState to) {
                    return -(to.score() - from.score());
                }

                public boolean isEndPoint(WorldState state) {
                    return Math.abs(state.robotX - goalX) + Math.abs(state.robotY - goalY) == 0;
                }

                public double horizon() {
                    return horizon;
                }
            }, initialState);

            while (!pathfinder.run()) {
            }
            pathfinder.getResult();
        }

        return 0;
    }


}
