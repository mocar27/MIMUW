package Buying;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;

public class Technofob extends Kupowanie {

    @Override
    public void wystawOfertyKupna(Zasoby zasoby, double iloscWyprodukowanych, List<OfertaRobotnika> ofertyKupna) {

        List<Double> temp = new ArrayList<>();
        temp.add(100.0);

        ofertyKupna.add(new OfertaRobotnika("jedzenie", temp));
    }
}