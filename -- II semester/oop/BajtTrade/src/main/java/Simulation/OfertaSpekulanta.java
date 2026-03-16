package Simulation;

import java.util.List;

public class OfertaSpekulanta {

    private final String typ;
    private final double cena;
    private List<Double> iloscPrzedmiotow;

    public OfertaSpekulanta(String s, double c, List<Double> i) {
        this.typ = s;
        this.cena = c;
        this.iloscPrzedmiotow = i;
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

    public double getCena() {
        return cena;
    }
}