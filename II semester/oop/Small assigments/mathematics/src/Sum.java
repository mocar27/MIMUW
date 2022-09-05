public class Sum extends TwoArgumentsExpression {

    private final int priorytet = 1;

    protected Sum(Expression l, Expression r) {
        super(l, r);
    }

    public static Expression create(Expression l, Expression r) {
        if (l.getClass() == Constant.class && r.getClass() == Constant.class) {
            return Constant.create(l.calculate(0) + r.calculate(0));
        }
        if ((l.getClass() == Constant.class && ((Constant) l).getValue() == 0) || (l.getClass() == Zero.class)) {
            return r;
        }
        if ((r.getClass() == Constant.class && ((Constant) r).getValue() == 0) || (r.getClass() == Zero.class)) {
            return l;
        }
        return new Sum(l, r);
    }

    public double calculate(double x) {
        return left.calculate(x) + right.calculate(x);
    }

    @Override
    public String getOperator() {
        return "+";
    }

    public Expression derivative() {
        return new Sum(left.derivative(), right.derivative());
    }

    public Expression derivative(Expression derivative) {
        return derivative.derivative();
    }

    @Override
    public int Priority() {
        return priorytet;
    }

    @Override
    public String toString() {
        String s = "";
        if (this.priorytet > this.left.Priority()) {
            s += "(" + this.left.toString() + ")";
        } else {
            s += this.left.toString();
        }
        s += getOperator();
        if (this.priorytet > this.right.Priority()) {
            s += "(" + this.right.toString() + ")";
        } else {
            s += this.right.toString();
        }
        return s;
    }
}
