public abstract class TwoArgumentsExpression extends Expression {

    protected Expression left;
    protected Expression right;
    protected int priority;

    public TwoArgumentsExpression(Expression l, Expression r) {
        this.left = l;
        this.right = r;
    }

    public abstract String getOperator();

}
