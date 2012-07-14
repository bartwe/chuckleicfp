public class StepLogic {
    WorldState scratch;

    public StepResult applyStep(WorldState current, WorldState next, RobotAction action) {
        if (current.stepResult != StepResult.Ok)
            throw new RuntimeException("Cannot move out of final step.");
        if (scratch == null)
            scratch = current.copy();
        else
            scratch.copyFrom(current);
        StepResult result = applyRobotAction(current, scratch, action);
        next.copyFrom(scratch);
        next.steps++;
        next.parent = current;
        next.action = action;
        if (result != StepResult.Abort)
            applyWorldStep(scratch, next);
        if ((next.robotX == next.exitX) && (next.robotY == next.exitY))
            result = StepResult.Win;
        else
        if ((next.get(next.robotX, next.robotY + 1) == Cell.Rock) && (current.get(next.robotX, next.robotY + 1) != Cell.Rock))
           result = StepResult.Lose;
        next.stepResult = result;
        return result;
    }

    private StepResult applyRobotAction(WorldState current, WorldState next, RobotAction action) {
        if (action == RobotAction.Wait)
            return StepResult.Ok;
        if (action == RobotAction.Abort)
            return StepResult.Abort;
        byte target = current.get(current.robotX + action.dx, current.robotY + action.dy);
        if (Cell.isEmptyEarthLambdaLift(target)) {
            if (Cell.isLambda(target)) {
                next.lambdaCollected++;
                next.lambdaRemaining--;
            } else if (target == Cell.ClosedLambdaLift && next.lambdaRemaining != 0)
                return StepResult.MoveFail;
            next.set(current.robotX, current.robotY, Cell.Empty);
            next.robotX += action.dx;
            next.robotY += action.dy;
            next.set(next.robotX, next.robotY, Cell.RobotLocation);
            return StepResult.Ok;
        } else if (action.dy != 0) {
            return StepResult.MoveFail;
        } else {
            if (action.dx == -1) {
                if (Cell.isRock(current.get(current.robotX - 1, current.robotY)) && Cell.isEmpty(current.get(current.robotX - 2, current.robotY))) {
                    next.set(current.robotX, current.robotY, Cell.Empty);
                    next.robotX += -1;
                    next.set(next.robotX, next.robotY, Cell.RobotLocation);
                    next.set(current.robotX - 2, current.robotY, Cell.Rock);
                    return StepResult.Ok;
                } else
                    return StepResult.MoveFail;
            } else {
                assert action.dx == 1;
                if (Cell.isRock(current.get(current.robotX + 1, current.robotY)) && Cell.isEmpty(current.get(current.robotX + 2, current.robotY))) {
                    next.set(current.robotX, current.robotY, Cell.Empty);
                    next.robotX += 1;
                    next.set(next.robotX, next.robotY, Cell.RobotLocation);
                    next.set(current.robotX + 2, current.robotY, Cell.Rock);
                    return StepResult.Ok;
                } else
                    return StepResult.MoveFail;
            }
        }
    }

    private void applyWorldStep(WorldState current, WorldState next) {
        int width = current.getN() + 2;
        int height = current.getM() + 2;
        for (int y = 0; y < height; ++y) {
            for (int x = 0; x < width; ++x) {
                if (current.get(x, y) == Cell.Rock && current.get(x, y - 1) == Cell.Empty) {
                    next.set(x, y, Cell.Empty);
                    next.set(x, y - 1, Cell.Rock);
                } else if (current.get(x, y) == Cell.Rock && current.get(x, y - 1) == Cell.Rock && current.get(x + 1, y) == Cell.Empty && current.get(x + 1, y - 1) == Cell.Empty) {
                    next.set(x, y, Cell.Empty);
                    next.set(x + 1, y - 1, Cell.Rock);
                } else if (current.get(x, y) == Cell.Rock && current.get(x, y - 1) == Cell.Rock && current.get(x - 1, y) == Cell.Empty && current.get(x - 1, y - 1) == Cell.Empty) {
                    next.set(x, y, Cell.Empty);
                    next.set(x - 1, y - 1, Cell.Rock);
                } else if (current.get(x, y) == Cell.Rock && current.get(x, y - 1) == Cell.Lambda && current.get(x + 1, y) == Cell.Empty && current.get(x + 1, y - 1) == Cell.Empty) {
                    next.set(x, y, Cell.Empty);
                    next.set(x + 1, y - 1, Cell.Rock);
                }
            }
        }
    }
}
