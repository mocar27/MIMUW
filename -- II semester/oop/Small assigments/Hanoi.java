import java.util.Scanner;

public class Hanoi {
    public static void hanoi(int ring, int start, int end) {
        if (ring > 0) {
            hanoi(ring - 1, start, 6 - start - end);
            System.out.println(ring + " " + start + " > " + end);
            hanoi(ring - 1, 6 - start - end, end);
        }
    }

    public static void main(String[] args) {
        Scanner scanner = new Scanner(System.in);
        int n = scanner.nextInt();
        hanoi(n, 1, 3);
    }
}
