package pl.edu.mimuw;

import pl.edu.mimuw.matrix.IDoubleMatrix;

import static pl.edu.mimuw.matrix.DoubleMatrixFactory.*;
import static pl.edu.mimuw.matrix.MatrixCellValue.cell;
import static pl.edu.mimuw.matrix.Shape.matrix;

public class Main {

    public static void main(String[] args) {
        IDoubleMatrix SPARSE_10X10 = sparse(matrix(10, 10),
                cell(0, 0, 1),
                cell(0, 2, 3),
                cell(1, 0, 2),
                cell(2, 1, 4),
                cell(6, 0, 95),
                cell(5, 4, 7),
                cell(2, 6, 12),
                cell(9, 9, 2),
                cell(5, 1, 11),
                cell(6, 0, 2137),
                cell(2, 4, 9),
                cell(9, 9, 1)

        );
        System.out.println(SPARSE_10X10);
        IDoubleMatrix FULL_10X10 = full(new double[][]{
                new double[]{1, 2, 3, 4, 5, 6, 7, 8, 9, 7},
                new double[]{9, 8, 7, 6, 5, 4, 3, 2, 1, 6},
                new double[]{2, 1, 3, 7, 4, 2, 0, 6, 9, 2},
                new double[]{8, 1, 3, 0, 1, 3, 3, 7, 0, 1},
                new double[]{6, 1, 6, 7, 2, 1, 7, 8, 0, 9},
                new double[]{0, 0, 0, 2, 2, 1, 2, 7, 0, 8},
                new double[]{2, 0, 2, 1, 3, 7, 4, 2, 0, 2},
                new double[]{2, 6, 7, 1, 2, 9, 8, 6, 2, 1},
                new double[]{6, 7, 2, 2, 2, 2, 2, 2, 2, 2},
                new double[]{1, 1, 1, 2, 2, 2, 3, 3, 3, 6},
        });
        System.out.println(FULL_10X10);

        IDoubleMatrix IDENTITY_10X10 = identity(10);
        System.out.println(IDENTITY_10X10);

        IDoubleMatrix DIAGONAL_10X10 = diagonal(10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
        System.out.println(DIAGONAL_10X10);

        IDoubleMatrix ANTIDIAGONAL_10X10 = antiDiagonal(10, 9, 8, 7, 6, 5, 4, 3, 2, 1);
        System.out.println(ANTIDIAGONAL_10X10);

        IDoubleMatrix VECTOR_10X1 = vector(2, 1, 3, 7, 4, 2, 0, 7, 7, 7);
        System.out.println(VECTOR_10X1);

        IDoubleMatrix ZERO_10X10 = zero(matrix(10, 10));
        System.out.println(ZERO_10X10);

        IDoubleMatrix CONSTANT_10X10 = constant(matrix(10, 10), 7);
        System.out.println(CONSTANT_10X10);

        IDoubleMatrix COLUMN_10X10 = column(matrix(10, 10), 1, 6, 2, 7, 2, 7, 8, 0, 2, 2);
        System.out.println(COLUMN_10X10);

        IDoubleMatrix ROW_10X10 = row(matrix(10, 10), 5, 1, 7, 2, 8, 9, 0, 2, 1, 7);
        System.out.println(ROW_10X10);
        
    }
}
