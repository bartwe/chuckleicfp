import java.util.*;

public class AStar {
    interface Controller {
        int getAdjacent(WorldState state, WorldState[] adjacentsBuffer);
        double goalHeuristic(WorldState state);
        double edgeCost(WorldState from, WorldState to);
        boolean isEndPoint(WorldState state);
        AStarNode findBest(AStarNode best, boolean singleSolution, HashMap<WorldStateHash, AStarNode> nodes);
        double horizon();
    }

    final Controller controller;
    int sequence;

    final HashSet<WorldStateHash> closedSet;
    final PriorityQueue<AStarEvent> openSet;
    final HashMap<WorldStateHash, AStarNode> nodes;
    AStarNode bestNode;

    int limit = 1000;
    final double horizon;

    final WorldState[] adjacentsBuffer;
    boolean completed;

    public AStar(Controller controller, WorldState start) {
        this.controller = controller;
        sequence = 0;

        closedSet = new HashSet<WorldStateHash>();
        openSet = new PriorityQueue<AStarEvent>();
        nodes = new HashMap<WorldStateHash, AStarNode>();
        bestNode = new AStarNode(null, controller.goalHeuristic(start), 0, start);
        nodes.put(bestNode.state.getHash(), bestNode);
        openSet.add(new AStarEvent(bestNode.state, bestNode.fScore, sequence++));

        horizon = controller.horizon();

        adjacentsBuffer = new WorldState[32];
    }

    public boolean run() {
        if (completed)
            return true;
        int batchSize = 100;
        while (!openSet.isEmpty()) {
            AStarEvent nodePos = openSet.poll();
            AStarNode node = nodes.get(nodePos.state.getHash());
            closedSet.add(node.state.getHash());
            if (node.fScore != nodePos.fScore)
                continue;
            bestNode = node;
            if (controller.isEndPoint(node.state))
            {
                openSet.clear();
                break;
            }
            if (node.gScore > horizon) {
                break;
            }

            int count = controller.getAdjacent(node.state, adjacentsBuffer);

            for (int i = 0; i < count; i++) {
                WorldState t = adjacentsBuffer[i];
                if (closedSet.contains(t.getHash()))
                    continue;
                double hScore = controller.goalHeuristic(t);
                double gScore = node.gScore + controller.edgeCost(t, node.state);
                if ((hScore >= Double.MAX_VALUE) || (gScore >= Double.MAX_VALUE))
                    continue;
                AStarNode slot = nodes.get(t.getHash());
                if (slot == null) {
                    slot = new AStarNode(node, hScore, gScore, t);
                    nodes.put(slot.state.getHash(), slot);
                    openSet.add(new AStarEvent(slot.state, slot.fScore, sequence++));
                } else {
                    if (gScore < slot.gScore) {
                        slot.parent = node;
                        slot.gScore = gScore;
                        slot.hScore = hScore;
                        slot.fScore = slot.gScore + slot.hScore;
                        openSet.add(new AStarEvent(slot.state, slot.fScore, sequence++));
                    }
                }
            }
            limit--;
            if (limit <= 0)
                break;
            if (batchSize-- < 0)
                return false;
        }
        completed = true;
        return true;
    }

    public ArrayList<WorldState> getResult() {
        if (!completed)
            throw new RuntimeException("!");
        if (!controller.isEndPoint(bestNode.state))
            bestNode = controller.findBest(bestNode, openSet.size() == 0, nodes);
        if (bestNode == null)
            return null;

        ArrayList<WorldState> path = new ArrayList<WorldState>();
        while (bestNode != null) {
            path.add(bestNode.state);
            bestNode = bestNode.parent;
        }
        Collections.reverse(path);
        return path;
    }

    private class AStarEvent implements Comparable<AStarEvent> {
        public final WorldState state;
        public final double fScore;
        public final int sequence;

        public AStarEvent(WorldState state, double fScore, int sequence) {
            this.state = state;
            this.fScore = fScore;
            this.sequence = sequence;
        }

        public int compareTo(AStarEvent o) {
            if (o == this)
                return 0;
            if (fScore < o.fScore)
                return -1;
            if (fScore > o.fScore)
                return 1;
            if (sequence > o.sequence)
                return -1;
            if (sequence < o.sequence)
                return 1;
            throw new RuntimeException("meh");
        }
    }

    class AStarNode {
        public AStarNode parent;
        public double hScore;
        public double gScore;
        public double fScore;
        public final WorldState state;

        public AStarNode(AStarNode node, double hScore, double gScore, WorldState state) {
            this.parent = node;
            this.hScore = hScore;
            this.gScore = gScore;
            this.fScore = hScore + gScore;
            this.state = state;
        }

        @Override
        public int hashCode() {
            return state.hashCode();
        }

        @Override
        public boolean equals(Object obj) {
            return state.equals(obj);
        }
    }
}
