public class StepResult {
    public static final StepResult Ok = new StepResult("Ok");
    public static final StepResult Win = new StepResult("Win");
    public static final StepResult Abort = new StepResult("Abort");
    public static final StepResult Lose = new StepResult("Lose");
    public static final StepResult MoveFail = new StepResult("MoveFail");
    private final String txt;

    public StepResult(String txt) {
        this.txt = txt;
    }

    @Override
    public String toString() {
        return txt;
    }
}
