package pl.edu.mimuw.matrix;

public abstract class SpecialMatrix extends Matrix implements IDoubleMatrix {

    protected double[] values;

    // Constructor of row, column and vector matrix
    public SpecialMatrix(Shape s, double... val) {
        super(s);
        this.values = new double[val.length];
        System.arraycopy(val, 0, this.values, 0, val.length);
    }

    @Override
    public IDoubleMatrix times(double scalar) {

        if (scalar == 0) {
            return new MatrixZero(this.shape);
        }
        if (scalar == 1) {
            return this;
        }

        double[] result = new double[values.length];
        for (int i = 0; i < values.length; i++) {
            result[i] = values[i] * scalar;
        }
        if (this.getClass() == MatrixColumn.class) {
            return new MatrixColumn(this.shape, result);
        } else if (this.getClass() == MatrixRow.class) {
            return new MatrixRow(this.shape, result);
        } else {
            return new Vector(result);
        }
    }

    @Override
    public IDoubleMatrix plus(IDoubleMatrix other) {

        assert this.shape.equals(other.shape());

        if (other.getClass() == MatrixZero.class) {
            return this;
        }

        if (other.getClass() == MatrixColumn.class && this.getClass() == MatrixColumn.class) {
            double[] temp = other.getValues();
            double[] result = new double[temp.length];
            for (int i = 0; i < temp.length; i++) {
                result[i] = temp[i] + values[i];
            }
            return new MatrixColumn(this.shape, result);
        } else if (other.getClass() == MatrixRow.class && this.getClass() == MatrixRow.class) {
            double[] temp = other.getValues();
            double[] result = new double[temp.length];
            for (int i = 0; i < temp.length; i++) {
                result[i] = temp[i] + values[i];
            }
            return new MatrixRow(this.shape, result);
        }

        double[][] result = new double[this.shape.rows][this.shape.columns];
        double[][] temp = other.data();

        if (this.getClass() == MatrixColumn.class) {
            for (int i = 0; i < this.shape.columns; i++) {
                for (int j = 0; j < this.shape.rows; j++) {
                    result[j][i] = temp[j][i] + values[i];
                }
            }
        } else {
            for (int i = 0; i < this.shape.rows; i++) {
                for (int j = 0; j < this.shape.columns; j++) {
                    result[i][j] = temp[i][j] + values[i];
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

        double[] result = new double[values.length];
        for (int i = 0; i < values.length; i++) {
            result[i] = values[i] + scalar;
        }
        if (this.getClass() == MatrixColumn.class) {
            return new MatrixColumn(this.shape, result);
        } else if (this.getClass() == MatrixRow.class) {
            return new MatrixRow(this.shape, result);
        } else {
            return new Vector(result);
        }
    }

    @Override
    public double frobeniusNorm() {
        double result = 0;
        for (double v : values) {
            result += (v * v);
            if (this.getClass() == MatrixColumn.class) {
                result *= this.shape.rows;
            } else {
                result *= this.shape.columns;
            }
        }
        return Math.sqrt(result);
    }

    @Override
    public double[] getValues() {
        return values;
    }

}