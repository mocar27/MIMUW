import java.util.Scanner;

public class Sito {
        public static void sito (int tab[], int size) {
                for(int k=2; k<size; k++) {
                    int x =k*k;
                    if(tab[k]!=0)
                    while(x < size) {
                        tab[x] = 0;
                        x+=k;
                    }

                }
        }
        public static void main(String[] args){
            Scanner scanner = new Scanner(System.in);
            int n = scanner.nextInt();
            int numbers[];
            numbers = new int[n+1];
            for(int i=1; i<=n; i++)
                numbers[i] = i;
            sito(numbers, n);
            for(int i=2; i<n;i++) {
                if(numbers[i] != 0)
                    System.out.print(numbers[i] + " ");
            }
        }
}
