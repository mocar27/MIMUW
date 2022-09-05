package pl.edu.mimuw.matrix;

public class MatrixZero extends MatrixConstant implements IDoubleMatrix {

    public MatrixZero(Shape s) {
        super(s, 0);
    }

    @Override
    public IDoubleMatrix times(IDoubleMatrix other) {
        assert this.shape.columns == other.shape().rows;
        return new MatrixZero(Shape.matrix(this.shape.rows, other.shape().columns));
    }

    @Override
    public double[][] data() {
        return new double[this.shape.rows][this.shape.columns];
    }

}
