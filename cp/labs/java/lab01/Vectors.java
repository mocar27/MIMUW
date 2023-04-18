import java.util.ArrayList;
import java.util.Random;

public class Vectors {

    private final int[] values;
    private static ArrayList<Thread> additiveThreads = new ArrayList<>();
    private static ArrayList<Thread> multiplicationThreads = new ArrayList<>();

    private Vectors(int size) {
        values = new int[size];
        Random rand = new Random();

        int bound = 21;

        for (int i = 0; i < size; i++) {
            values[i] = rand.nextInt(bound);
        }
    }


    private static class Additive implements Runnable {

        private final int a;
        private final int b;
        private final Vectors first;
        private final Vectors second;
        private final Vectors res;

        Additive(Vectors first, Vectors second, Vectors res, int a, int b) {
            this.a = a;
            this.b = b;
            this.first = first;
            this.second = second;
            this.res = res;
        }

        @Override
        public void run() {
            for (int i = a; i < first.values.length && i < b; i++) {
                res.values[i] = first.values[i] + second.values[i];
            }
        }
    }

    private static class Multiplier implements Runnable {

        private final int a;
        private final int b;
        private final Vectors first;
        private final Vectors second;
        private final Vectors res;

        Multiplier(Vectors first, Vectors second, Vectors res, int a, int b) {
            this.a = a;
            this.b = b;
            this.first = first;
            this.second = second;
            this.res = res;
        }

        @Override
        public void run() {
            for (int i = a; i < first.values.length && i < b; i++) {
                res.values[i] = first.values[i] * second.values[i];
            }
        }
    }

    public void printVector() {

        for (int value : values) {
            System.out.println(value);
        }
    }

    public static Vectors sumSeq(Vectors first, Vectors second) {

        Vectors res = new Vectors(first.values.length);

        for (int i = 0; i < first.values.length; i++) {
            res.values[i] = first.values[i] + second.values[i];
        }

        return res;
    }

    public static int dotSeq(Vectors first, Vectors second) {

        int res = 0;

        for (int i = 0; i < first.values.length; i++) {
            res += (first.values[i] * second.values[i]);
        }

        return res;
    }

    public static Vectors sum(Vectors first, Vectors second) {

        Vectors res = new Vectors(first.values.length);

        for (int i = 0; i < first.values.length; i += 10) {
            Thread t = new Thread(new Additive(first, second, res, i, Math.min(first.values.length, i + 10)));
            additiveThreads.add(t);
        }

        for (Thread t : additiveThreads) {
            try {
                t.start();
            } catch (Exception e) {
                System.out.println("Cos sie nie udalo w dodawaniu / startowanie");
            }
        }
        for (Thread t : additiveThreads) {
            try {
                t.join();
            } catch (Exception e) {
                System.out.println("Cos sie nie udalo w dodawaniu / joinowanie");
            }
        }

        return res;
    }

    public static int dot(Vectors first, Vectors second) {

        Vectors res = new Vectors(first.values.length);

        for (int i = 0; i < first.values.length; i += 10) {
            Thread t = new Thread(new Multiplier(first, second, res, i, Math.min(first.values.length, i + 10)));
            multiplicationThreads.add(t);
        }
        for (Thread t : multiplicationThreads) {
            try {
                t.start();
            } catch (Exception e) {
                System.out.println("Cos sie nie udalo w mnozeniu / startowanie");
            }
        }
        for (Thread t : multiplicationThreads) {
            try {
                t.join();
            } catch (Exception e) {
                System.out.println("Cos sie nie udalo w mnozeniu / joinowanie");
            }
        }
        int result = 0;

        for (
                int i = 0;
                i < res.values.length; i++) {
            result += res.values[i];
        }
        return result;
    }

    public static void main(String[] args) {

        Random rand = new Random();
        int bound = 21;
        int n = rand.nextInt(bound);

        Vectors v1 = new Vectors(n);
        Vectors v2 = new Vectors(n);

        System.out.println("Wektor 1: ");
        v1.printVector();
        System.out.println("Wektor 2: ");
        v2.printVector();

        System.out.println("Suma wektorow sekwencyjnie: ");
        sumSeq(v1, v2).printVector();
        System.out.println("Suma wektorow wielowatkowo: ");
        sum(v1, v2).printVector();
        System.out.println("Iloczyn skalarny sekwencyjnie: ");
        System.out.println(dotSeq(v1, v2));
        System.out.println("Iloczyn skalarny wielowatkowo: ");
        System.out.println(dot(v1, v2));
    }
}
