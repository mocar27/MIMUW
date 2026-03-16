package pl.edu.mimuw.matrix;

public abstract class Matrix implements IDoubleMatrix {

    protected final Shape shape;

    public Matrix(Shape s) {
        this.shape = s;
    }

    public IDoubleMatrix times(IDoubleMatrix other) {

        assert this.shape.columns == other.shape().rows;

        if (other.getClass() == MatrixIdentity.class) {
            return this;
        } else if (other.getClass() == MatrixZero.class) {
            return new MatrixZero(Shape.matrix(this.shape.rows, other.shape().columns));
        }

        double[][] temp1 = this.data();
        double[][] temp2 = other.data();
        double[][] result = new double[this.shape.rows][other.shape().columns];

        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < other.shape().columns; j++) {
                for (int k = 0; k < other.shape().rows; k++) {
                    result[i][j] += temp1[i][k] * temp2[k][j];
                }
            }
        }
        return new MatrixFull(result);

    }

    public IDoubleMatrix minus(IDoubleMatrix other) {

        assert this.shape.equals(other.shape());

        if (other.getClass() == MatrixZero.class) {
            return this;
        }

        return this.plus(other.times(-1));
    }

    public IDoubleMatrix minus(double scalar) {

        if (scalar == 0) {
            return this;
        }

        scalar *= -1;
        return this.plus(scalar);
    }

    public double get(int row, int column) {
        this.shape.assertInShape(row, column);
        return 0;
    }

    public Shape shape() {
        return shape;
    }

    public double[] getValues() {
        return new double[0];
    }
}
