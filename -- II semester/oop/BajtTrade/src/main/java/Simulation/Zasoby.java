package Simulation;

import java.util.ArrayList;
import java.util.List;

public class Zasoby {

    private double diamenty;
    private List<Double> ubrania;
    private List<Double> narzedzia;
    private double jedzenie;
    private List<Double> programy;

    public Zasoby(double d, double u, double n, double j, double p) {
        this.diamenty = d;
        this.ubrania = new ArrayList<>();
        this.ubrania.add(u);
        this.narzedzia = new ArrayList<>();
        this.narzedzia.add(n);
        this.jedzenie = j;
        this.programy = new ArrayList<>();
        this.programy.add(p);
    }

    public double getDiamenty() {
        return diamenty;
    }

    public double getUbrania(int poziom) {
        return ubrania.get(poziom);
    }

    public double getNarzedzia(int poziom) {
        return narzedzia.get(poziom);
    }

    public double getJedzenie() {
        return jedzenie;
    }

    public double getProgramy(int poziom) {
        return programy.get(poziom);
    }

    public List<Double> getProgramyList() {
        return programy;
    }

    public double getSizeUbrania() {
        return ubrania.size();
    }

    public double getSizeNarzedzia() {
        return narzedzia.size();
    }

    public double getSizeProgramy() {
        return programy.size();
    }

    public void modyfikujDiamenty(double wartosc) {
        diamenty = wartosc;
    }

    public void modyfikujUbrania(int poziom, double wartosc) {
        ubrania.set(poziom, wartosc);
    }

    public void modyfikujNarzedzia(int poziom, double wartosc) {
        narzedzia.set(poziom, wartosc);
    }

    public void modyfikujJedzenie(double wartosc) {
        jedzenie = wartosc;
    }

    public void modyfikujProgramy(int poziom, double wartosc) {
        programy.set(poziom, wartosc);
    }
}