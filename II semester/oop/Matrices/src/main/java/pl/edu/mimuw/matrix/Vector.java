package pl.edu.mimuw.matrix;

public class Vector extends SpecialMatrix implements IDoubleMatrix {

    public Vector(double... val) {
        super(Shape.vector(val.length), val);
    }

    @Override
    public IDoubleMatrix plus(IDoubleMatrix other) {

        assert this.shape.equals(other.shape());

        if (other.getClass() == MatrixZero.class) {
            return this;
        }

        double[] result = new double[this.shape.rows];
        double[] temp = other.getValues();

        for (int i = 0; i < this.shape.rows; i++) {
            result[i] = this.values[i] + temp[i];
        }
        return new Vector(result);

    }

    @Override
    public double get(int row, int column) {
        this.shape.assertInShape(row, column);
        return this.values[row];
    }

    @Override
    public double[][] data() {

        double[][] result = new double[this.values.length][1];
        for (int i = 0; i < this.values.length; i++) {
            result[i][0] = this.values[i];
        }
        return result;
    }

    @Override
    public double normOne() {
        double result = 0;
        for (double value : values) {
            result += Math.abs(value);
        }
        return result;
    }

    @Override
    public double normInfinity() {
        double result = 0;
        for (double value : values) {
            result = Math.max(result, Math.abs(value));
        }
        return result;
    }

    @Override
    public double frobeniusNorm() {
        double result = 0;
        for (double value : values) {
            result += (value * value);
        }
        return Math.sqrt(result);
    }

    @Override
    public String toString() {
        String s = "Vector dimensions: " + this.shape.rows + " x " + this.shape.columns + "\n";
        for (int i = 0; i < this.shape.rows; i++) {
            s += this.values[i] + "\n";
        }
        return s;
    }

}