package StockMarket;

import java.util.Arrays;
import java.util.Comparator;

import static java.util.Collections.reverseOrder;

import Simulation.*;
import Players.*;

public class Zrownowazona extends Gielda {

    private boolean czyKapitalistyczna = false;

    @Override
    public void dopasujOferty(Info info, Robotnik[] robotnicy, Spekulant[] spekulanci) {

        if (czyKapitalistyczna) {
            Arrays.sort(robotnicy, Comparator.comparing(Robotnik::getZasobyGetDiamenty)
                    .thenComparing(Robotnik::getID));
            Arrays.sort(robotnicy, reverseOrder());
            czyKapitalistyczna = false;
        } else {
            Arrays.sort(robotnicy, Comparator.comparing(Robotnik::getZasobyGetDiamenty)
                    .thenComparing(Robotnik::getID));
            czyKapitalistyczna = true;
        }

        super.dopasuj(info, robotnicy, spekulanci);
    }

    @Override
    public String toString() {
        return "zrownowazona";
    }
}