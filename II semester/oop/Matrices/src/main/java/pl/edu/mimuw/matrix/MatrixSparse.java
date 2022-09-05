package pl.edu.mimuw.matrix;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Comparator;
import java.util.List;

public class MatrixSparse extends Matrix implements IDoubleMatrix {

    private final MatrixCellValue[] values;
    private final int numOfValues;

    public MatrixSparse(Shape s, MatrixCellValue... values) {
        super(s);
        this.numOfValues = values.length;
        this.values = new MatrixCellValue[values.length];
        System.arraycopy(values, 0, this.values, 0, values.length);

        Arrays.sort(values, Comparator.comparing(MatrixCellValue::getRow)
                .thenComparing(MatrixCellValue::getColumn));
    }

    @Override
    public IDoubleMatrix times(IDoubleMatrix other) {

        assert this.shape.columns == other.shape().rows;

        if (other.getClass() == MatrixZero.class) {
            return new MatrixZero(Shape.matrix(this.shape.rows, other.shape().columns));
        }

        if (other.getClass() == MatrixIdentity.class) {
            return this;
        }

        if (other.getClass() == MatrixDiagonal.class) {

            MatrixCellValue[] result = new MatrixCellValue[numOfValues];
            double[] temp = other.getValues();
            int size = 0;

            for (int i = 0; i < temp.length; i++) {
                for (MatrixCellValue m : values) {
                    if (m.column == i) {
                        result[size] = new MatrixCellValue(m.row, m.column, m.value * temp[i]);
                        size++;
                    }
                }
            }
            return new MatrixSparse(Shape.matrix(this.shape.rows, other.shape().columns), result);
        }

        if (other.getClass() == MatrixConstant.class) {

            double[] rowsVal = new double[this.shape.rows];
            double val = other.get(0, 0);
            double temp = 0;
            int actualRow = values[0].row;

            for (MatrixCellValue m : values) {
                if (m.row != actualRow) {
                    rowsVal[actualRow] = temp;
                    actualRow = m.row;
                    temp = 0;
                }
                temp += m.value * val;
            }
            rowsVal[actualRow] = temp;
            return new MatrixRow(Shape.matrix(this.shape.rows, other.shape().columns), rowsVal);
        }

        if (other.getClass() == MatrixSparse.class) {

            MatrixCellValue[] otherVal = ((MatrixSparse) other).getMatrixCellsValues();
            List<MatrixCellValue> temp = new ArrayList<>();

            for (MatrixCellValue m : this.values) {
                for (MatrixCellValue o : otherVal) {
                    if (o.row == m.column) {
                        temp.add(new MatrixCellValue(m.row, o.column, m.value * o.value));
                    }
                }
            }
            MatrixCellValue[] temp2 = new MatrixCellValue[temp.size()];
            for (int i = 0; i < temp.size(); i++) {
                temp2[i] = temp.get(i);
            }
            Arrays.sort(temp2, Comparator.comparing(MatrixCellValue::getRow)
                    .thenComparing(MatrixCellValue::getColumn));

            int counter = 0;
            for (int i = 0; i < temp2.length; i++) {
                if (i != temp2.length - 1) {
                    if (temp2[i].row != temp2[i + 1].row || temp2[i].column != temp2[i + 1].column) {
                        counter++;
                    }
                } else {
                    counter++;
                }
            }

            MatrixCellValue[] result = new MatrixCellValue[counter];
            int iterator = 0;
            double v;

            for (int i = 0; i < temp2.length; i++) {
                if (i != temp2.length - 1) {
                    if (temp2[i].row != temp2[i + 1].row || temp2[i].column != temp2[i + 1].column) {
                        result[iterator] = new MatrixCellValue(temp2[i].row, temp2[i].column, temp2[i].value);
                    } else {
                        v = 0;
                        while (i < temp2.length - 1 && temp2[i].row == temp2[i + 1].row && temp2[i].column == temp2[i + 1].column) {
                            v += temp2[i].value;
                            i++;
                        }
                        if (i != temp2.length) {
                            v += temp2[i].value;
                        }
                        result[iterator] = new MatrixCellValue(temp2[i].row, temp2[i].column, v);
                    }
                } else {
                    result[iterator] = new MatrixCellValue(temp2[i].row, temp2[i].column, temp2[i].value);
                }
                iterator++;
            }
            return new MatrixSparse(Shape.matrix(this.shape.rows, other.shape().columns), result);
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

    @Override
    public IDoubleMatrix times(double scalar) {

        if (scalar == 0) {
            return new MatrixZero(this.shape);
        }
        if (scalar == 1) {
            return this;
        }

        MatrixCellValue[] result = new MatrixCellValue[values.length];

        for (int i = 0; i < values.length; i++) {
            result[i] = new MatrixCellValue(values[i].row, values[i].column, values[i].value * scalar);
        }

        return new MatrixSparse(this.shape, result);
    }

    @Override
    public IDoubleMatrix plus(IDoubleMatrix other) {

        assert this.shape.equals(other.shape());

        if (other.getClass() == MatrixZero.class) {
            return this;
        }

        if (other.getClass() == MatrixSparse.class) {

            int size = 0;
            boolean found;
            MatrixCellValue[] temp = new MatrixCellValue[numOfValues + ((MatrixSparse) other).getNumOfValues()];
            MatrixCellValue[] v = ((MatrixSparse) other).getMatrixCellsValues();
            boolean[] checked = new boolean[numOfValues];

            for (MatrixCellValue m : v) {
                found = false;
                for (int j = 0; j < values.length; j++) {
                    MatrixCellValue m1 = values[j];
                    if (m.row == m1.row && m.column == m1.column) {
                        temp[size] = new MatrixCellValue(m.row, m.column, m.value + m1.value);
                        size++;
                        checked[j] = true;
                        found = true;
                    }
                }
                if (!found) {
                    temp[size] = new MatrixCellValue(m.row, m.column, m.value);
                    size++;
                }
            }

            for (int i = 0; i < values.length; i++) {
                if (!checked[i]) {
                    temp[size] = new MatrixCellValue(values[i].row, values[i].column, values[i].value);
                    size++;
                }
            }

            MatrixCellValue[] result = new MatrixCellValue[size];
            System.arraycopy(temp, 0, result, 0, size);
            return new MatrixSparse(this.shape, result);
        }

        double[][] result = new double[this.shape.rows][this.shape.columns];
        double[][] temp1 = this.data();
        double[][] temp2 = other.data();

        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                result[i][j] = temp1[i][j] + temp2[i][j];
            }
        }
        return new MatrixFull(result);
    }

    @Override
    public IDoubleMatrix plus(double scalar) {

        if (scalar == 0) {
            return this;
        }

        double[][] result = this.data();
        for (int i = 0; i < this.shape.rows; i++) {
            for (int j = 0; j < this.shape.columns; j++) {
                result[i][j] += scalar;
            }
        }

        return new MatrixFull(result);
    }

    @Override
    public double get(int row, int column) {
        this.shape.assertInShape(row, column);

        for (MatrixCellValue m : values) {
            if (m.row == row && m.column == column)
                return m.value;
        }
        return 0;
    }

    @Override
    public double[][] data() {
        double[][] result = new double[this.shape.rows][this.shape.columns];

        for (MatrixCellValue m : this.values) {
            result[m.row][m.column] = m.value;
        }

        return result;
    }

    @Override
    public double normOne() {
        double result = 0;
        double temp;
        for (int i = 0; i < this.shape.columns; i++) {
            temp = 0;
            for (MatrixCellValue m : values) {
                if (m.column == i) {
                    temp += Math.abs(m.value);
                }
            }
            result = Math.max(result, temp);
        }
        return result;
    }

    @Override
    public double normInfinity() {
        double result = 0;
        double temp = 0;
        int actualRow = 0;
        for (MatrixCellValue m : values) {
            if (m.row == actualRow) {
                temp += Math.abs(m.value);
            } else {
                result = Math.max(result, temp);
                temp = Math.abs(m.value);
                actualRow = m.row;
            }
        }
        result = Math.max(result, temp);
        return result;
    }

    @Override
    public double frobeniusNorm() {
        double result = 0;

        for (MatrixCellValue m : values) {
            result += m.value * m.value;
        }
        return Math.sqrt(result);
    }

    @Override
    public String toString() {
        String s = "Dimensions of an irregular sparse matrix: " + this.shape.rows + " x " + this.shape.columns + "\n";

        int zeros;
        double[][] temp = this.data();

        if (this.shape.columns > 2) {
            for (int i = 0; i < this.shape.rows; i++) {
                zeros = 0;
                for (int j = 0; j < this.shape.columns; j++) {
                    if (temp[i][j] == 0) {
                        zeros++;
                    }
                }
                if (zeros == this.shape.columns) {
                    s += "0.0 ... 0.0";
                } else {
                    for (int j = 0; j < this.shape.columns; j++) {
                        s += temp[i][j] + " ";
                    }
                }
                s += "\n";
            }
        } else {
            for (int i = 0; i < this.shape.rows; i++) {
                for (int j = 0; j < this.shape.columns; j++) {
                    s += temp[i][j] + " ";
                }
                s += "\n";
            }
        }
        return s;
    }

    public int getNumOfValues() {
        return numOfValues;
    }

    public MatrixCellValue[] getMatrixCellsValues() {
        return values;
    }

}