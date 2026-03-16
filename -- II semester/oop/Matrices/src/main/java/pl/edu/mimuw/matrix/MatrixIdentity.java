package pl.edu.mimuw.matrix;

public class MatrixIdentity extends MatrixDiagonal implements IDoubleMatrix {

    public MatrixIdentity(int s) {
        super(s);
    }

    @Override
    public IDoubleMatrix times(IDoubleMatrix other) {

        assert this.shape.columns == other.shape().rows;

        if (other.getClass() == MatrixZero.class) {
            return new MatrixZero(Shape.matrix(this.shape.rows, other.shape().columns));
        }

        return other;
    }

    @Override
    public double normOne() {
        return 1;
    }

    @Override
    public double normInfinity() {
        return 1;
    }

    @Override
    public double frobeniusNorm() {
        return Math.sqrt(this.shape.rows);
    }

}
