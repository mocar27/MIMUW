public class Cos extends OneArgumentExpression {

    public Cos(Expression expression) {
        super(expression);
    }

    public double calculate(double x) {
        return Math.cos(expression.calculate(x));
    }

    public Expression derivative() {
        return new Multiplication(new Multiplication(new Constant(-1), new Sin(expression)), expression.derivative());
    }

    public Expression derivative(Expression derivative) {
        return null;
    }

    @Override
    public String toString() {
        return "cos(" + this.expression + ")";
    }
}
