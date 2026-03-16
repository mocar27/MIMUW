package StockMarket;

import java.util.Arrays;
import java.util.Comparator;

import Simulation.*;
import Players.*;

public class Socjalistyczna extends Gielda {

    @Override
    public void dopasujOferty(Info info, Robotnik[] robotnicy, Spekulant[] spekulanci) {

        Arrays.sort(robotnicy, Comparator.comparing(Robotnik::getZasobyGetDiamenty)
                .thenComparing(Robotnik::getID));

        super.dopasuj(info, robotnicy, spekulanci);
    }

    @Override
    public String toString() {
        return "socjalistyczna";
    }
}