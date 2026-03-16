package pl.edu.mimuw.matrix;

public class MatrixDiagonal extends Diagonals implements IDoubleMatrix {

    public MatrixDiagonal(double... diagonalVal) {
        super(diagonalVal);
    }

    public MatrixDiagonal(int s) {
        super(s);
    }

    @Override
    public IDoubleMatrix plus(IDoubleMatrix other) {

        assert this.shape.equals(other.shape());

        if (other.getClass() == MatrixZero.class) {
            return this;
        }
        if (other.getClass() == MatrixDiagonal.class || other.getClass() == MatrixIdentity.class) {

            double[] result = new double[diagonalValues.length];
            double[] temp2 = other.getValues();

            for (int i = 0; i < diagonalValues.length; i++) {
                result[i] = diagonalValues[i] + temp2[i];
            }
            return new MatrixDiagonal(result);
        } else if (other.getClass() == MatrixSparse.class) {

            int size = 0;
            MatrixCellValue[] temp = new MatrixCellValue[((MatrixSparse) other).getNumOfValues() + diagonalValues.length];
            MatrixCellValue[] v = ((MatrixSparse) other).getMatrixCellsValues();
            boolean[] checked = new boolean[diagonalValues.length];

            for (MatrixCellValue m : v) {

                if (m.row == m.column) {
                    temp[size] = new MatrixCellValue(m.row, m.column, m.value + diagonalValues[m.row]);
                    checked[m.row] = true;
                } else {
                    temp[size] = new MatrixCellValue(m.row, m.column, m.value);
                }
                size++;
            }
            for (int i = 0; i < diagonalValues.length; i++) {

                if (!checked[i]) {
                    temp[size] = new MatrixCellValue(i, i, diagonalValues[i]);
                    size++;
                }
            }
            MatrixCellValue[] result = new MatrixCellValue[size];
            System.arraycopy(temp, 0, result, 0, size);
            return new MatrixSparse(this.shape, result);
        }
        double[][] result = new double[this.shape.columns][];
        for (int i = 0; i < this.shape.columns; i++) {
            result[i] = new double[this.shape.rows];
        }

        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                if (i == j) {
                    result[i][j] = diagonalValues[i] + other.get(i, j);
                } else {
                    result[i][j] += other.get(i, j);
                }
            }
        }
        return new MatrixFull(result);

    }

    @Override
    public IDoubleMatrix plus(double scalar) {

        if (scalar == 0) {
            return this;
        }

        double[][] values = new double[this.shape.rows][this.shape.columns];
        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                if (i == j) {
                    values[i][j] = this.diagonalValues[i] + scalar;
                } else {
                    values[i][j] += scalar;
                }
            }
        }
        return new MatrixFull(values);
    }

    @Override
    public double get(int row, int column) {
        this.shape.assertInShape(row, column);
        if (row == column) {
            return diagonalValues[row];
        } else {
            return 0.0;
        }
    }

    @Override
    public double[][] data() {
        double[][] values = new double[this.shape.rows][this.shape.columns];
        for (int i = 0; i < this.shape.rows; i++) {
            values[i][i] = diagonalValues[i];
        }
        return values;
    }
}
