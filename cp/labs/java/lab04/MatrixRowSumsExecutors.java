import java.util.ArrayList;
import java.util.List;
import java.util.function.IntBinaryOperator;
import java.util.concurrent.*;

import static java.util.concurrent.Executors.newFixedThreadPool;

public class MatrixRowSumsExecutors {

    private static final int ROWS = 10;
    private static final int COLUMNS = 100;
    private static final int NUMOFTHREADS = 4;

    private static class Matrix {

        private final int rows;
        private final int columns;
        private static IntBinaryOperator definition;

        public Matrix(int rows, int columns, IntBinaryOperator definition) {
            this.rows = rows;
            this.columns = columns;
            this.definition = definition;
        }

        public int[] rowSums() {
            int[] rowSums = new int[rows];
            for (int row = 0; row < rows; ++row) {
                int sum = 0;
                for (int column = 0; column < columns; ++column) {
                    sum += definition.applyAsInt(row, column);
                }
                rowSums[row] = sum;
            }
            return rowSums;
        }

        public void rowSumsConcurrent() {
            ExecutorService threads = newFixedThreadPool(NUMOFTHREADS);
            List<Callable<Integer>> elements = new ArrayList<>();

            for (int i = 0; i < ROWS; i++) {
                for (int j = 0; j < COLUMNS; j++) {
                    elements.add(new Counting(i, j));
                }
            }

            try {
                List<Future<Integer>> results = threads.invokeAll(elements);
                int sumOfRow = 0;
                for (int i = 0; i < ROWS; i++) {
                    for (int j = 0; j < COLUMNS; j++) {

                        Future<Integer> element = results.get(j);
                        sumOfRow += element.get();
                    }
                    System.out.println(i + " -> " + sumOfRow);
                }

            } catch (Exception e) {
                System.out.println("Error");
            } finally {
                threads.shutdown();
            }
        }
    }

    public static class Counting implements Callable<Integer> {

        int row;
        int column;

        public Counting(int r, int c) {
            this.row = r;
            this.column = c;
        }

        @Override
        public Integer call() throws Exception {
            return Matrix.definition.applyAsInt(row, column);
        }
    }

    public static void main(String[] args) {
        Matrix matrix = new Matrix(ROWS, COLUMNS, (row, column) -> {
            int a = 2 * column + 1;
            return (row + 1) * (a % 4 - 2) * a;
        });

        int[] rowSums = matrix.rowSums();
        matrix.rowSumsConcurrent();

        System.out.println("SUM SEQUENCE:");
        for (int i = 0; i < rowSums.length; i++) {
            System.out.println(i + " -> " + rowSums[i]);
        }

        System.out.println("SUM CONCURRENT EXECUTORS:");
        matrix.rowSumsConcurrent();
    }

}