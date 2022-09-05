package pl.edu.mimuw.matrix;

public class MatrixFull extends Matrix implements IDoubleMatrix {

    private final double[][] values;

    public MatrixFull(double[][] v) {

        super(Shape.matrix(v.length, v[0].length));
        values = new double[v.length][];

        for (int i = 0; i < v.length; i++) {
            values[i] = new double[v[i].length];
            System.arraycopy(v[i], 0, values[i], 0, v[i].length);
        }
    }

    @Override
    public IDoubleMatrix times(double scalar) {

        if (scalar == 0) {
            return new MatrixZero(this.shape);
        }
        if (scalar == 1) {
            return this;
        }

        double[][] result = new double[this.shape.rows][this.shape.columns];
        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                result[i][j] = values[i][j] * scalar;
            }
        }
        return new MatrixFull(result);
    }

    @Override
    public IDoubleMatrix plus(IDoubleMatrix other) {

        assert this.shape.equals(other.shape());

        if (other.getClass() == MatrixZero.class) {
            return this;
        }

        double[][] result = new double[this.shape.rows][this.shape.columns];
        double[][] temp = other.data();

        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                result[i][j] = values[i][j] + temp[i][j];
            }
        }
        return new MatrixFull(result);
    }

    @Override
    public IDoubleMatrix plus(double scalar) {

        if (scalar == 0) {
            return this;
        }

        double[][] result = new double[this.shape.rows][this.shape.columns];

        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                result[i][j] = values[i][j] + scalar;
            }
        }
        return new MatrixFull(result);
    }

    @Override
    public double get(int row, int column) {
        this.shape.assertInShape(row, column);
        return values[row][column];
    }

    @Override
    public double[][] data() {
        return this.values;
    }

    @Override
    public double normOne() {

        double result = 0;
        double temp;
        for (int i = 0; i < this.shape.columns; i++) {

            temp = 0;
            for (int j = 0; j < this.shape.rows; j++) {
                temp += Math.abs(values[j][i]);
            }
            result = Math.max(result, temp);
        }
        return result;
    }

    @Override
    public double normInfinity() {
        double result = 0;
        double temp;
        for (int i = 0; i < this.shape.rows; i++) {

            temp = 0;
            for (int j = 0; j < this.shape.columns; j++) {
                temp += Math.abs(values[i][j]);
            }
            result = Math.max(result, temp);
        }
        return result;
    }

    @Override
    public double frobeniusNorm() {

        double result = 0;
        for (int i = 0; i < this.shape.rows; i++) {

            for (int j = 0; j < this.shape.columns; j++) {
                result += values[i][j] * values[i][j];
            }
        }
        return Math.sqrt(result);
    }

    public String toString() {
        String s = "Full matrix dimensions: " + this.shape.rows + " x " + this.shape.columns + "\n";
        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                s += values[i][j] + " ";
            }
            s += "\n";
        }
        return s;
    }
}
