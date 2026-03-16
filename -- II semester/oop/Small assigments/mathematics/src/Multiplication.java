public class Multiplication extends TwoArgumentsExpression {

    private final int priority = 2;

    protected Multiplication(Expression l, Expression r) {
        super(l, r);
    }

    public static Expression create(Expression l, Expression r) {
        if (l.getClass() == Constant.class && r.getClass() == Constant.class) {
            return Constant.create(l.calculate(0) * r.calculate(0));
        }
        if (l.getClass() == Zero.class || r.getClass() == Zero.class) {
            return Zero.create();
        }
        if (l.getClass() == Constant.class && ((Constant) l).getValue() == 0) {
            return Zero.create();
        }
        if (r.getClass() == Constant.class && ((Constant) r).getValue() == 0) {
            return Zero.create();
        }
        if ((l.getClass() == Constant.class && ((Constant) l).getValue() == 1) || (l.getClass() == One.class)) {
            return r;
        }
        if ((r.getClass() == Constant.class && ((Constant) r).getValue() == 1) || (r.getClass() == One.class)) {
            return l;
        }
        return new Multiplication(l, r);
    }

    public double calculate(double x) {
        return left.calculate(x) * right.calculate(x);
    }

    public Expression derivative() {
        return new Sum(new Multiplication(derivative(left), right), new Multiplication(left, derivative(right)));
    }

    public Expression derivative(Expression derivative) {
        return derivative.derivative();
    }

    @Override
    public int Priority() {
        return priority;
    }

    @Override
    public String getOperator() {
        return "*";
    }

    @Override
    public String toString() {
        String s = "";
        if (this.priority > this.left.Priority()) {
            s += "(" + this.left.toString() + ")";
        } else {
            s += this.left.toString();
        }
        s += getOperator();
        if (this.priority > this.right.Priority()) {
            s += "(" + this.right.toString() + ")";
        } else {
            s += this.right.toString();
        }

        return s;
    }
}
