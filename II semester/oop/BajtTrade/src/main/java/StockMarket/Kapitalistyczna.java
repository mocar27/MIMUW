package StockMarket;

import java.util.Arrays;
import java.util.Comparator;

import static java.util.Collections.reverseOrder;

import Simulation.*;
import Players.*;

public class Kapitalistyczna extends Gielda {

    @Override
    public void dopasujOferty(Info info, Robotnik[] robotnicy, Spekulant[] spekulanci) {
        Arrays.sort(robotnicy, Comparator.comparing(Robotnik::getZasobyGetDiamenty)
                .thenComparing(Robotnik::getID));
        Arrays.sort(robotnicy, reverseOrder());

        super.dopasuj(info, robotnicy, spekulanci);
    }

    @Override
    public String toString() {
        return "kapitalistyczna";
    }
}