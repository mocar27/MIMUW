package Buying;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;

public class Czyscioszek extends Technofob {

    @Override
    public void wystawOfertyKupna(Zasoby zasoby, double iloscWyprodukowanych, List<OfertaRobotnika> ofertyKupna) {

        super.wystawOfertyKupna(zasoby, iloscWyprodukowanych, ofertyKupna);

        double sumaUbran = 0;

        for (int i = 0; i < zasoby.getSizeUbrania(); i++) {
            sumaUbran += zasoby.getUbrania(i);
        }

        if (sumaUbran < 200) {
            List<Double> temp = new ArrayList<>();
            temp.add(200.0 - sumaUbran);

            ofertyKupna.add(new OfertaRobotnika("ubrania", temp));
        }
    }
}