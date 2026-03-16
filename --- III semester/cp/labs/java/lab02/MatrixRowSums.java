import java.util.concurrent.BrokenBarrierException;
import java.util.concurrent.CyclicBarrier;
import java.util.function.IntBinaryOperator;

public class MatrixRowSums {

    private static final int ROWS = 10;
    private static final int COLUMNS = 100;

    private static final Thread[] myThreads = new Thread[COLUMNS];
    private static int[] rowSumsConcurrent = new int[ROWS];
    private static int it = 0;
    private static int previousSum = 0;
    private static int sum;
    private static int[] values = new int[COLUMNS];

    private static final CyclicBarrier barrier = new CyclicBarrier(COLUMNS,
            MatrixRowSums::addSum);

    private static void addSum() {
        for (int i = 0; i < COLUMNS; i++) {
            sum += values[i];
        }
        rowSumsConcurrent[it] = sum - previousSum;
        previousSum = sum;
        sum = 0;
        it++;
    }

    private static class Matrix {

        private final int rows;
        private final int columns;
        private static IntBinaryOperator definition;

        public Matrix(int rows, int columns, IntBinaryOperator definition) {
            this.rows = rows;
            this.columns = columns;
            this.definition = definition;
        }

        public static int getV(int row, int column) {
            return definition.applyAsInt(row, column);
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

            for (int i = 0; i < COLUMNS; i++) {
                Thread t = new Thread(new SummingThread(i));
                myThreads[i] = t;
            }

            for (int i = 0; i < COLUMNS; ++i) {
                myThreads[i].start();
            }

            try {
                for (int i = 0; i < COLUMNS; ++i) {
                    myThreads[i].join();

                }
            } catch (InterruptedException e) {
                System.err.println("rowSumsConcurrent interrupted");
                Thread.currentThread().interrupt();
            }
        }
    }

    private static class SummingThread implements Runnable {

        private final int column;

        public SummingThread(int column) {
            this.column = column;
        }

        @Override
        public void run() {

            try {

                for (int i = 0; i < ROWS; i++) {
                    values[column] += Matrix.getV(i, column);
                    barrier.await();
                }

            } catch (InterruptedException | BrokenBarrierException e) {
                Thread t = Thread.currentThread();
                t.interrupt();
                System.err.println(t.getName() + " interrupted");
            }
        }
    }

    public static void main(String[] args) {
        Matrix matrix = new Matrix(ROWS, COLUMNS, (row, column) -> {
            int a = 2 * column + 1;
            return (row + 1) * (a % 4 - 2) * a;
        });

        int[] rowSums = matrix.rowSums();
        matrix.rowSumsConcurrent();

        System.out.println("Sequence rowSums:");
        for (int i = 0; i < rowSums.length; i++) {
            System.out.println(i + " -> " + rowSums[i]);
        }

        System.out.println("Concurrent rowSums:");
        for (int i = 0; i < rowSumsConcurrent.length; i++) {
            System.out.println(i + "-> " + rowSumsConcurrent[i]);
        }
    }

}