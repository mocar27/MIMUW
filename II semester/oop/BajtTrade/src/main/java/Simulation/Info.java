package Simulation;

import java.util.ArrayList;
import java.util.List;

import StockMarket.*;

public class Info {

    private final int kara_za_brak_ubran;
    private final int dlugosc;
    private final Gielda gielda;
    private final Zasoby ceny;

    private int dzien;
    private List<Zasoby> ceny_srednie;
    private List<Zasoby> ceny_min;
    private List<Zasoby> ceny_max;

    public Info(int dlugosc, Gielda g, int kara, Zasoby c) {
        this.dlugosc = dlugosc;
        this.gielda = g;
        this.kara_za_brak_ubran = kara;
        this.ceny = c;
    }

    public void infoInitialize() {
        this.ceny_srednie = new ArrayList<>();
        this.ceny_min = new ArrayList<>();
        this.ceny_max = new ArrayList<>();
        this.ceny_srednie.add(ceny);
        this.ceny_min.add(ceny);
        this.ceny_max.add(ceny);
        this.dzien = 1;
    }

    public double getSredniaJedzenie(int iloscDni) {
        int it = ceny_srednie.size() - 1;
        double wynik = 0;
        while (it >= 0) {
            wynik += ceny_srednie.get(it).getJedzenie();
            it--;
        }
        if (iloscDni > dzien) {
            wynik += ceny.getJedzenie();
        }
        return wynik / iloscDni;
    }

    public int getDlugosc() {
        return dlugosc;
    }

    public int getDzien() {
        return dzien;
    }

    public void modyfikujDzien() {
        this.dzien++;
    }

    public Gielda getGielda() {
        return gielda;
    }

    public int getKara() {
        return kara_za_brak_ubran;
    }

    public void dodajCenySrednie(Zasoby z) {
        this.ceny_srednie.add(z);
    }

    public void dodajCenyMin(Zasoby z) {
        this.ceny_min.add(z);
    }

    public void dodajCenyMax(Zasoby z) {
        this.ceny_max.add(z);
    }

    public List<Zasoby> getCenySrednie() {
        return this.ceny_srednie;
    }

    public List<Zasoby> getCenyMin() {
        return this.ceny_min;
    }

    public List<Zasoby> getCenyMax() {
        return this.ceny_max;
    }

}