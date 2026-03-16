public abstract class OneArgumentExpression extends Expression {

    protected Expression expression;

    public OneArgumentExpression(Expression expression) {
        this.expression = expression;
    }

}
