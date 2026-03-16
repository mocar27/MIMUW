package Production;

import java.util.Random;

import Simulation.*;
import Career.*;

public class Losowy extends Produkcja {

    @Override
    public String coProdukuje(Info info, Zasoby produktywnosc, int bonus, Kariera k) {

        Random rand = new Random();
        int produkt = rand.nextInt(5);

        if (produkt == 0) {
            return "jedzenie";
        }
        if (produkt == 1) {
            return "diamenty";
        }
        if (produkt == 2) {
            return "ubrania";
        }
        if (produkt == 3) {
            return "narzedzia";
        }

        return "programy";
    }
}