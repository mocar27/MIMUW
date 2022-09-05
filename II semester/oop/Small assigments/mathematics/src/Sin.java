public class Sin extends OneArgumentExpression {

    public Sin(Expression expression) {
        super(expression);
    }

    public double calculate(double x) {
        return Math.sin(expression.calculate(x));
    }

    public Expression derivative() {
        return new Multiplication(new Cos(expression), expression.derivative());
    }

    public Expression derivative(Expression derivative) {
        return null;
    }

    @Override
    public String toString() {
        return "sin(" + this.expression + ")";
    }
}
