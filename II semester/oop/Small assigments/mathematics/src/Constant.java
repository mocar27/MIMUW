public class Constant extends Expression {

    private final double value;

    protected Constant(double value) {
        this.value = value;
    }

    public static Constant create(double x) {
        return new Constant(x);
    }

    public Expression tryAdding(Constant v) {
        double temp = this.value + v.getValue();

        return switch ((int) temp) {
            case 0 -> Zero.create();
            case 1 -> One.create();
            default -> Constant.create(temp);
        };
    }


    public double calculate(double x) {
        return this.value;
    }

    public Expression derivative() {
        return new Constant(0);
    }

    public Expression derivative(Expression derivative) {
        return derivative.derivative();
    }

    public double getValue() {
        return value;
    }

    @Override
    public String toString() {
        return this.value + "";
    }
}
