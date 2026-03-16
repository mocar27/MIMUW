package Buying;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;

public class Gadzeciarz extends Zmechanizowany {

    public double liczba_narzedzi;

    @Override
    public void wystawOfertyKupna(Zasoby zasoby, double iloscWyprodukowanych, List<OfertaRobotnika> ofertyKupna) {

        super.wystawOfertyKupna(zasoby, iloscWyprodukowanych, ofertyKupna);

        double ileZostalo = 0;
        for (int i = 0; i < zasoby.getProgramyList().size(); i++) {
            ileZostalo += zasoby.getProgramy(i);
        }

        List<Double> temp = new ArrayList<>();
        temp.add(iloscWyprodukowanych - ileZostalo);
        ofertyKupna.add(new OfertaRobotnika("programy", temp));

    }
}