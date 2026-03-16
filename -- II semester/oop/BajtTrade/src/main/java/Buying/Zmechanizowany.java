package Buying;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;

public class Zmechanizowany extends Czyscioszek {

    public double liczba_narzedzi;

    @Override
    public void wystawOfertyKupna(Zasoby zasoby, double iloscWyprodukowanych, List<OfertaRobotnika> ofertyKupna) {

        super.wystawOfertyKupna(zasoby, iloscWyprodukowanych, ofertyKupna);

        List<Double> temp = new ArrayList<>();
        temp.add(liczba_narzedzi);

        ofertyKupna.add(new OfertaRobotnika("narzedzia", temp));
    }
}