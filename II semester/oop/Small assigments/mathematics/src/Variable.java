public class Variable extends Expression {

    private String variable;

    public Variable(String z) {
        this.variable = z;
    }

    public double calculate(double x) {
        return x;
    }

    public Expression derivative() {
        return new Constant(1);
    }

    public Expression derivative(Expression derivative) {
        return derivative.derivative();
    }

    @Override
    public String toString() {
        return this.variable;
    }
}
