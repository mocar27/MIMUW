package pl.edu.mimuw.matrix;

public class MatrixColumn extends SpecialMatrix implements IDoubleMatrix {

    public MatrixColumn(Shape s, double... columnsVal) {
        super(s, columnsVal);
    }

    @Override
    public double get(int row, int column) {
        this.shape.assertInShape(row, column);
        return this.values[column];
    }

    @Override
    public double[][] data() {
        double[][] result = new double[this.shape.rows][this.shape.columns];
        for (int i = 0; i < this.shape.rows; i++) {
            System.arraycopy(this.values, 0, result[i], 0, this.shape.columns);
        }
        return result;
    }

    @Override
    public double normOne() {
        double result = 0;
        for (double v : values) {
            result = Math.max(Math.abs(v), result);
        }
        return result * this.shape.rows;
    }

    @Override
    public double normInfinity() {
        double result = 0;
        for (double v : values) {
            result += Math.abs(v);
        }
        return result;
    }

    @Override
    public String toString() {
        String s = "Column matrix dimensions: " + this.shape.rows + " x " + this.shape.columns + "\n";

        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                s += this.values[j] + " ";
            }
            s += "\n";
        }
        return s;
    }
}
