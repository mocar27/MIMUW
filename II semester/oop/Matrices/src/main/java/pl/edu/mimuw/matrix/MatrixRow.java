package pl.edu.mimuw.matrix;

public class MatrixRow extends SpecialMatrix implements IDoubleMatrix {

    public MatrixRow(Shape s, double... rowsVal) {
        super(s, rowsVal);
    }

    @Override
    public double get(int row, int column) {
        this.shape.assertInShape(row, column);
        return this.values[row];
    }

    @Override
    public double[][] data() {
        double[][] result = new double[this.shape.rows][this.shape.columns];
        for (int i = 0; i < this.shape.columns; i++) {
            System.arraycopy(this.values, 0, result[i], 0, this.shape.columns);
        }
        return result;
    }

    @Override
    public double normOne() {
        double result = 0;
        for (double v : values) {
            result += Math.abs(v);
        }
        return result;
    }

    @Override
    public double normInfinity() {
        double result = 0;
        for (double v : values) {
            result = Math.max(Math.abs(v), result);
        }
        return result * this.shape.columns;
    }

    @Override
    public String toString() {
        String s = "Row matrix dimensions: " + this.shape.rows + " x " + this.shape.columns + "\n";

        switch (this.shape.columns) {
            case 1:
                for (int i = 0; i < this.shape.rows; i++) {
                    s += this.values[i] + "\n";
                }
                break;
            case 2:
                for (int i = 0; i < this.shape.rows; i++) {
                    s += this.values[i] + "  " + this.values[i] + "\n";
                }
                break;
            default:
                for (int i = 0; i < this.shape.rows; i++) {
                    s += this.values[i] + " ... " + this.values[i] + "\n";
                }
                break;
        }
        return s;
    }

}
