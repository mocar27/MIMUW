package Learning;

import java.util.Random;

import Simulation.*;

public class Rozkladowy extends Uczenie {

    public boolean czyUczySie(Info info, double diamenty, int dzien) {
        int nauka = 1;
        int praca = dzien + 2;
        Random rand = new Random();
        int decyzja = rand.nextInt(nauka + praca);
        return decyzja == 0;
    }
}