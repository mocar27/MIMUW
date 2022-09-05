package pl.edu.mimuw.matrix;

public class DoubleMatrixFactory {

    private DoubleMatrixFactory() {
    }

    public static IDoubleMatrix sparse(Shape shape, MatrixCellValue... values) {
        assert values != null;
        assert shape.rows > 0;
        assert shape.columns > 0;
        for (MatrixCellValue m : values) {
            shape.assertInShape(m.row, m.column);
        }
        return new MatrixSparse(shape, values);
    }

    public static IDoubleMatrix full(double[][] values) {
        assert values != null;
        assert values.length > 0;
        assert values[0].length > 0;
        int rows = values[0].length;
        for (double[] value : values) {
            assert value.length == rows;
        }
        return new MatrixFull(values);
    }

    public static IDoubleMatrix identity(int size) {
        assert size > 0;
        return new MatrixIdentity(size);
    }

    public static IDoubleMatrix diagonal(double... diagonalValues) {
        assert diagonalValues != null;
        return new MatrixDiagonal(diagonalValues);
    }

    public static IDoubleMatrix antiDiagonal(double... antiDiagonalValues) {
        assert antiDiagonalValues != null;
        return new MatrixAntidiagonal(antiDiagonalValues);
    }

    public static IDoubleMatrix vector(double... values) {
        assert values != null;
        return new Vector(values);
    }

    public static IDoubleMatrix zero(Shape shape) {
        assert shape.rows > 0;
        assert shape.columns > 0;
        return new MatrixZero(shape);
    }

    public static IDoubleMatrix constant(Shape shape, double value) {
        assert shape.rows > 0;
        assert shape.columns > 0;
        assert value >= 0;
        return new MatrixConstant(shape, value);
    }

    public static IDoubleMatrix column(Shape shape, double... columnsVal) {
        assert shape.rows > 0;
        assert shape.columns > 0;
        assert columnsVal != null;
        return new MatrixColumn(shape, columnsVal);
    }

    public static IDoubleMatrix row(Shape shape, double... rowsVal) {
        assert shape.rows > 0;
        assert shape.columns > 0;
        assert rowsVal != null;
        return new MatrixRow(shape, rowsVal);
    }

}
