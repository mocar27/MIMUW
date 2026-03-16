package Buying;

import java.util.List;

import Simulation.*;

public abstract class Kupowanie {

    public abstract void wystawOfertyKupna(Zasoby zasoby, double iloscWyprodukowanych,
                                           List<OfertaRobotnika> ofertyKupna);
}