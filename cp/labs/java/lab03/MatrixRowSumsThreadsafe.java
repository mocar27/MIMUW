import java.util.ArrayList;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.ConcurrentMap;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.function.IntBinaryOperator;

public class MatrixRowSumsThreadsafe {

    private static final int ROWS = 10;
    private static final int COLUMNS = 100;
    private static int[] rowSumsConcurrent = new int[ROWS];
    private static ArrayList<Thread> myThreads = new ArrayList<>();

    private static ConcurrentMap<Integer, LinkedBlockingQueue<Integer>> mapQueues = new ConcurrentHashMap<>();

    private static class ColumnThread implements Runnable {

        private static int column;

        public ColumnThread(int column) {
            this.column = column;
        }

        @Override
        public void run() {

            try {
                for (int i = 0; i < ROWS; i++) {
                    mapQueues.get(i).put(Matrix.getV(i, column));
                }
            } catch (InterruptedException e) {
                Thread t = Thread.currentThread();
                t.interrupt();
                System.err.println(t.getName() + "interrupted column thread");
            }

        }
    }


    private static class RowThread implements Runnable {

        @Override
        public void run() {

            int it =0;
            while (it < ROWS) {

                int sum = 0;
                for (int column = 0; column < COLUMNS; column++) {
                    try {
                        sum += mapQueues.get(column).take();
                    } catch (InterruptedException e) {
                        Thread t = Thread.currentThread();
                        t.interrupt();
                        System.err.println(t.getName() + "interrupted row summing");
                    }
                }
                rowSumsConcurrent[it] = sum;
                it++;
            }

        }
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

        public static int getV(int row, int column) {
            return definition.applyAsInt(row, column);
        }

        public void rowSumsConcurrentThreadsafe() {

            for (int i = 0; i < COLUMNS; i++) {
                LinkedBlockingQueue<Integer> q = new LinkedBlockingQueue<>();
                mapQueues.putIfAbsent(i, q);
            }

            for(int i=0; i< COLUMNS; i++) {
                Thread t = new Thread(new ColumnThread(i));
                myThreads.add(t);
            }
            Thread t = new Thread(new RowThread());
            myThreads.add(t);
            for (Thread myThread : myThreads) {
                myThread.start();
            }

            try {
                for (Thread myThread : myThreads) {
                    myThread.join();
                }
            } catch (InterruptedException e) {
                System.err.println("rowSumsConcurrent interrupted");
                Thread.currentThread().interrupt();
            }

        }
    }

    public static void main(String[] args) {
        Matrix matrix = new Matrix(ROWS, COLUMNS, (row, column) -> {
            int a = 2 * column + 1;
            return (row + 1) * (a % 4 - 2) * a;
        });

        int[] rowSums = matrix.rowSums();
        matrix.rowSumsConcurrentThreadsafe();

        System.out.println("SUM SEQUENCE:");
        for (int i = 0; i < rowSums.length; i++) {
            System.out.println(i + " -> " + rowSums[i]);
        }

        System.out.println("SUM CONCURRENT THREAD SAFE:");
        for (int i = 0; i < rowSumsConcurrent.length; i++) {
            System.out.println(i + " -> " + rowSumsConcurrent[i]);
        }

    }
}
