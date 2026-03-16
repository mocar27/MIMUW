package pl.edu.mimuw.matrix;

public class MatrixAntidiagonal extends Diagonals implements IDoubleMatrix {


    public MatrixAntidiagonal(double... antiDiagonalVal) {
        super(antiDiagonalVal);
    }

    @Override
    public IDoubleMatrix plus(IDoubleMatrix other) {

        assert this.shape.columns == other.shape().rows;

        if (other.getClass() == MatrixZero.class) {
            return this;
        }
        if (other.getClass() == MatrixAntidiagonal.class) {

            double[] result = new double[diagonalValues.length];
            double[] temp2 = other.getValues();

            for (int i = 0; i < diagonalValues.length; i++) {
                result[i] = diagonalValues[i] + temp2[i];
            }
            return new MatrixAntidiagonal(result);
        } else if (other.getClass() == MatrixSparse.class) {

            int size = 0;
            MatrixCellValue[] temp = new MatrixCellValue[((MatrixSparse) other).getNumOfValues() + diagonalValues.length];
            MatrixCellValue[] v = ((MatrixSparse) other).getMatrixCellsValues();
            int[] checked = new int[diagonalValues.length];
            Shape helper = other.shape();

            for (MatrixCellValue m : v) {

                if (m.row == helper.columns - m.column - 1) {
                    temp[size] = new MatrixCellValue(m.row, m.column, m.value + diagonalValues[m.row]);
                    checked[m.row] = 1;
                } else {
                    temp[size] = new MatrixCellValue(m.row, m.column, m.value);
                }
                size++;
            }
            for (int i = 0; i < diagonalValues.length; i++) {

                if (checked[i] == 0) {
                    temp[size] = new MatrixCellValue(i, helper.columns - i - 1, diagonalValues[i]);
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
                if (j == this.shape.columns - i - 1) {
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
                if (j == this.shape.columns - i - 1) {
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
        if (row == this.shape.columns - column) {
            return diagonalValues[row];
        } else {
            return 0.0;
        }
    }

    @Override
    public double[][] data() {
        double[][] values = new double[this.shape.rows][this.shape.columns];
        for (int i = this.shape.rows - 1; i >= 0; i--) {
            values[i][this.shape.columns - i - 1] = diagonalValues[i];
        }
        return values;
    }

    @Override
    public String toString() {
        String s = "Dimensions of the anti-diagonal matrix: " + this.shape.rows + " x " + this.shape.columns + "\n";
        if (this.shape.columns < 4) {
            for (int i = 0; i < this.shape.rows; i++) {
                for (int j = 0; j < this.shape.columns; j++) {
                    if (j == this.shape.columns - i - 1) {
                        s += diagonalValues[i] + " ";
                    } else {
                        s += "0.0 ";
                    }
                }
                s += "\n";
            }
        } else {
            int counter;
            for (int i = 0; i < this.shape.rows; i++) {

                counter = 0;
                for (int j = 0; j < this.shape.columns; j++) {
                    if (j == this.shape.columns - i - 1) {
                        switch (counter) {
                            case 0:
                                s += "";
                                break;
                            case 1:
                                s += "0.0 ";
                                break;
                            case 2:
                                s += "0.0 0.0 ";
                                break;
                            default:
                                s += "0.0 ... 0.0 ";
                                break;
                        }
                        counter = 0;
                        s += diagonalValues[i] + " ";
                    } else {
                        counter++;
                    }
                }
                switch (counter) {
                    case 0:
                        s += "";
                        break;
                    case 1:
                        s += "0.0 ";
                        break;
                    case 2:
                        s += "0.0 0.0 ";
                        break;
                    default:
                        s += "0.0 ... 0.0 ";
                        break;
                }
                s += "\n";
            }
        }
        return s;
    }
}
