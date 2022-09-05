package pl.edu.mimuw.matrix;

public class MatrixConstant extends Matrix implements IDoubleMatrix {

    private final double value;

    public MatrixConstant(Shape s, double v) {
        super(s);
        this.value = v;
    }

    @Override
    public IDoubleMatrix times(double scalar) {

        if (scalar == 0) {
            return new MatrixZero(this.shape);
        }
        if (scalar == 1) {
            return this;
        }
        return new MatrixConstant(this.shape, value * scalar);
    }

    @Override
    public IDoubleMatrix plus(IDoubleMatrix other) {

        assert this.shape.equals(other.shape());

        if (other.getClass() == MatrixZero.class) {
            return this;
        }
        return other.plus(value);
    }

    @Override
    public IDoubleMatrix plus(double scalar) {
        return new MatrixConstant(this.shape, value + scalar);
    }

    @Override
    public double get(int row, int column) {
        this.shape.assertInShape(row, column);
        return value;
    }

    @Override
    public double[][] data() {
        double[][] values = new double[this.shape.rows][this.shape.columns];
        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                values[i][j] = value;
            }
        }
        return values;
    }

    @Override
    public double normOne() {
        return this.shape.columns * Math.abs(value);
    }

    @Override
    public double normInfinity() {
        return this.shape.rows * Math.abs(value);
    }

    @Override
    public double frobeniusNorm() {
        double x = (value * value) * (this.shape.rows * this.shape.columns);
        return Math.sqrt(x);
    }

    @Override
    public String toString() {
        String s = "The dimensions of the constant matrix: " + this.shape.rows + " x " + this.shape.columns + "\n";

        if (this.shape.columns > 2) {
            for (int i = 0; i < this.shape.rows; i++) {
                s += value + " ... " + value + "\n";
            }
        } else {
            for (int i = 0; i < this.shape.rows; i++) {
                s += value + " " + value + "\n";
            }

        }
        return s;
    }

}