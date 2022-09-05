public class Zero extends Constant {

    private static Zero zero = new Zero();

    private Zero() {
        super(0);
    }

    public static Zero create() {
        return zero;
    }

    public Expression tryAddingLtoP(Expression exp) {
        return exp;
    }

    public Expression tryAddingPtoL(Expression exp) {
        return exp;
    }

    public Expression tryMultiplyingLtoP(Expression exp) {
        return Zero.create();
    }

    public Expression tryMultiplyingPtoL(Expression exp) {
        return Zero.create();
    }
}
