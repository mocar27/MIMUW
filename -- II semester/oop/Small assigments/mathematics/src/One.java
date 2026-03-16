public class One extends Constant {

    private static One one = new One();

    private One() {
        super(1);
    }

    public static One create() {
        return one;
    }

    public Expression tryAddingLtoP(Expression exp) {
        return One.create();
    }

    public Expression tryAddingPtoL(Expression exp) {
        return One.create();
    }

    public Expression tryMultiplyingLtoP(Expression exp) {
        return exp;
    }

    public Expression tryMultiplyingPtoL(Expression exp) {
        return exp;
    }

}
