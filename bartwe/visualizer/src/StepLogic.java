public class StepLogic {
    WorldState scratch;

    public void applyStep(WorldState current, WorldState next, RobotAction action) {
        if (current.stepResult != StepResult.Ok)
            throw new RuntimeException("Cannot move out of final step.");
        if (scratch == null)
            scratch = current.copy();
        scratch.copyFrom(current);
        scratch.trampolined = false;
        StepResult result = applyRobotAction(current, scratch, action);
        next.copyFrom(scratch);
        next.steps++;
        next.parent = current;
        next.action = action;
        next.trampolined = scratch.trampolined;
        if (result != StepResult.Abort) {
            result = applyWorldStep(result, scratch, next);
            if ((next.robotX == next.exitX) && (next.robotY == next.exitY))
                result = StepResult.Win;
            else if (Cell.isRockish(next.get(next.robotX, next.robotY + 1)) && !Cell.isRockish(current.get(next.robotX, next.robotY + 1)))
                result = StepResult.Lose;
            else if (Cell.isLambda(next.get(next.robotX, next.robotY + 1)) && Cell.isEmpty(current.get(next.robotX, next.robotY + 1)))
                result = StepResult.Lose;
        } else
            next.stepResult = result;
        next.stepResult = result;
    }

    private StepResult applyRobotAction(WorldState current, WorldState next, RobotAction action) {
        if (action == RobotAction.Wait)
            return StepResult.Ok;
        if (action == RobotAction.Abort)
            return StepResult.Abort;
        if (action == RobotAction.Shave) {
            if (next.razors == 0)
                return StepResult.MoveFail;
            boolean beard = false;
            for (int y = -1; y <= 1; y++)
                for (int x = -1; x <= 1; x++) {
                    byte cell = current.get(current.robotX + x, current.robotY + y);
                    if (cell == Cell.Beard) {
                        beard = true;
                        next.set(current.robotX + x, current.robotY + y, Cell.Empty);
                    }
                }
            if (!beard)
                return StepResult.MoveFail;
            next.razors--;
            return StepResult.Ok;
        }
        byte target = current.get(current.robotX + action.dx, current.robotY + action.dy);
        if (Cell.isEmptyEarthLambdaLiftTransporterRazor(target)) {
            if (target == Cell.Razor) {
                next.razors++;
            } else if (Cell.isLambda(target)) {
                next.lambdaCollected++;
                next.lambdaRemaining--;
            } else if (target == Cell.ClosedLambdaLift && next.lambdaRemaining != 0)
                return StepResult.MoveFail;
            else if (Cell.isTransporter(target)) {
                byte targetTransporter = current.map.getTargetTransporter(target);
                next.set(current.robotX, current.robotY, Cell.Empty);
                next.doTransporter(targetTransporter);
                return StepResult.Ok;
            }
            next.set(current.robotX, current.robotY, Cell.Empty);
            next.robotX += action.dx;
            next.robotY += action.dy;
            next.set(next.robotX, next.robotY, Cell.RobotLocation);
            return StepResult.Ok;
        } else if (action.dy != 0) {
            return StepResult.MoveFail;
        } else {
            if (action.dx == -1) {
                if (Cell.isRockish(current.get(current.robotX - 1, current.robotY)) && Cell.isEmpty(current.get(current.robotX - 2, current.robotY))) {
                    next.set(current.robotX, current.robotY, Cell.Empty);
                    next.robotX += -1;
                    next.set(next.robotX, next.robotY, Cell.RobotLocation);
                    next.set(current.robotX - 2, current.robotY, current.get(current.robotX - 1, current.robotY));
                    return StepResult.Ok;
                } else
                    return StepResult.MoveFail;
            } else {
                assert action.dx == 1;
                if (Cell.isRockish(current.get(current.robotX + 1, current.robotY)) && Cell.isEmpty(current.get(current.robotX + 2, current.robotY))) {
                    next.set(current.robotX, current.robotY, Cell.Empty);
                    next.robotX += 1;
                    next.set(next.robotX, next.robotY, Cell.RobotLocation);
                    next.set(current.robotX + 2, current.robotY, current.get(current.robotX + 1, current.robotY));
                    return StepResult.Ok;
                } else
                    return StepResult.MoveFail;
            }
        }
    }

    private StepResult applyWorldStep(StepResult result, WorldState current, WorldState next) {
        int n = current.getN();
        int m = current.getM();

        if (next.curWaterLevel >= next.robotY) {
            next.submergedSteps++;
        } else {
            next.submergedSteps = 0;
        }

        if (next.floodingFreq != 0 && (next.steps) % next.floodingFreq == 0) {
            next.curWaterLevel++;
        }

        if (next.submergedSteps > next.waterproof) {
            result = StepResult.Lose;
        }

        boolean beardy = next.steps % next.growth == 0;

        for (int y = 1; y <= m; ++y) {
            for (int x = 1; x <= n; ++x) {
                byte c = current.get(x, y);
                if (Cell.isRockish(c)) {
                    byte goal = Cell.Rock;
                    if (c == Cell.HighOrderRock)
                        goal = Cell.Lambda;
                    if (current.get(x, y - 1) == Cell.Empty) {
                        next.set(x, y, Cell.Empty);
                        next.set(x, y - 1, goal);
                    } else if (Cell.isRockish(current.get(x, y - 1)) && current.get(x + 1, y) == Cell.Empty && current.get(x + 1, y - 1) == Cell.Empty) {
                        next.set(x, y, Cell.Empty);
                        next.set(x + 1, y - 1, goal);
                    } else if (Cell.isRockish(current.get(x, y - 1)) && current.get(x - 1, y) == Cell.Empty && current.get(x - 1, y - 1) == Cell.Empty) {
                        next.set(x, y, Cell.Empty);
                        next.set(x - 1, y - 1, goal);
                    } else if (current.get(x, y - 1) == Cell.Lambda && current.get(x + 1, y) == Cell.Empty && current.get(x + 1, y - 1) == Cell.Empty) {
                        next.set(x, y, Cell.Empty);
                        next.set(x + 1, y - 1, goal);
                    }
                }
                if (beardy && c == Cell.Beard) {
                    for (int y_ = -1; y_ <= 1; y_++)
                        for (int x_ = -1; x_ <= 1; x_++) {
                            byte cell = current.get(x + x_, y + y_);
                            if (cell == Cell.Empty) {
                                next.set(x + x_, y + y_, Cell.Beard);
                            }
                        }
                }
            }
        }
        return result;
    }
}
