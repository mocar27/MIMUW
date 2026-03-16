public abstract class Expression {

    private int priority;

    public abstract Expression derivative();

    public abstract Expression derivative(Expression derivative);

    public abstract double calculate(double x);

    public double integral(Expression e, double a, double b) {

        double x = 0;
        for (double i = a; i < b; i += 0.1) {
            x += e.calculate(i);
        }
        return x;
    }

    public int Priority() {
        return 2000;
    }

    public Expression tryAdding(Expression e) {
        return e.tryAdding2(this);
    }

    public Expression tryAdding2(Expression e) {
        return Sum.create(e, Zero.create());
    }
}
