package Simulation;

import java.util.List;

public class OfertaRobotnika {

    private final String typ;
    private List<Double> iloscPrzedmiotow;

    public OfertaRobotnika(String s, List<Double> p) {
        this.typ = s;
        this.iloscPrzedmiotow = p;
    }

    public void setIloscPrzedmiotow(int poziom, double nowaIlosc) {
        this.iloscPrzedmiotow.set(poziom, nowaIlosc);
    }

    public String getTyp() {
        return typ;
    }

    public double getIloscPrzedmiotow(int poziom) {
        return iloscPrzedmiotow.get(poziom);
    }

    public int getSizePrzedmiotow() {
        return iloscPrzedmiotow.size();
    }
}