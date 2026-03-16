package pl.edu.mimuw.matrix;

public abstract class Diagonals extends Matrix implements IDoubleMatrix {

    protected final double[] diagonalValues;

    // Constructor of a diagonal / antidiagonal matrix
    public Diagonals(double... diagonalVal) {
        super(Shape.matrix(diagonalVal.length, diagonalVal.length));
        this.diagonalValues = new double[diagonalVal.length];

        int i = 0;
        for (double d : diagonalVal) {
            this.diagonalValues[i] = d;
            i++;
        }
    }

    // Constructor of the identity matrix
    public Diagonals(int s) {
        super(Shape.matrix(s, s));
        this.diagonalValues = new double[s];

        for (int i = 0; i < s; i++) {
            this.diagonalValues[i] = 1;
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

        double[] values = new double[this.shape.rows];
        System.arraycopy(diagonalValues, 0, values, 0, diagonalValues.length);

        for (int i = 0; i < this.shape.rows; i++) {
            values[i] *= scalar;
        }

        if (this.getClass() == MatrixDiagonal.class || this.getClass() == MatrixIdentity.class) {
            return new MatrixDiagonal(values);
        } else {
            return new MatrixAntidiagonal(values);
        }
    }

    @Override
    public double normOne() {
        double result = 0;
        for (double diagonalValue : diagonalValues) {
            result = Math.max(result, Math.abs(diagonalValue));
        }
        return result;
    }

    @Override
    public double normInfinity() {
        return this.normOne();
    }

    @Override
    public double frobeniusNorm() {
        double result = 0;
        for (double diagonalValue : diagonalValues) {
            result += (diagonalValue * diagonalValue);
        }
        return Math.sqrt(result);
    }

    @Override
    public double[] getValues() {
        return diagonalValues;
    }

    @Override
    public String toString() {
        String s = "Dimensions of the diagonal matrix: " + this.shape.rows + " x " + this.shape.columns + "\n";
        if (this.shape.columns < 4) {
            for (int i = 0; i < this.shape.rows; i++) {
                for (int j = 0; j < this.shape.columns; j++) {
                    if (i == j) {
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
                    if (i == j) {
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