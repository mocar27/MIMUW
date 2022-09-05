package Production;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;
import Career.*;

public abstract class Produkcja {

    public List<Double> produkuj(Zasoby produktywnosc, int bonusProdukcji,
                                 String typ, int poziom, List<Double> programy) {

        List<Double> produkty = new ArrayList<>();

        if (typ.equals("jedzenie")) {
            produkty.add(produktywnosc.getJedzenie() + produktywnosc.getJedzenie() * bonusProdukcji);
        } else if (typ.equals("diamenty")) {
            produkty.add(produktywnosc.getDiamenty() + produktywnosc.getDiamenty() * bonusProdukcji);
        } else {
            for (int i = 0; i < poziom; i++) {
                produkty.add(0.0);
            }
            double iloscDoWyprodukowania;
            if (typ.equals("narzedzia")) {
                iloscDoWyprodukowania = produktywnosc.getNarzedzia(0) +
                        produktywnosc.getNarzedzia(0) * bonusProdukcji;
            } else if (typ.equals("ubrania")) {
                iloscDoWyprodukowania = produktywnosc.getUbrania(0) +
                        produktywnosc.getUbrania(0) * bonusProdukcji;
            } else {
                iloscDoWyprodukowania = produktywnosc.getProgramy(0) +
                        produktywnosc.getProgramy(0) * bonusProdukcji;
            }

            produkty.set(0, iloscDoWyprodukowania);
            if (poziom != 1) {
                for (int i = Math.min(poziom - 1, programy.size() - 1); i >= 0 && produkty.get(0) != 0; i--) {
                    produkty.set(i, Math.min(iloscDoWyprodukowania, programy.get(i)));
                    programy.set(i, programy.get(i) - Math.min(iloscDoWyprodukowania, programy.get(i)));
                    produkty.set(0, iloscDoWyprodukowania - Math.min(iloscDoWyprodukowania, programy.get(i)));
                }
            } else {
                programy.set(0, programy.get(0) - Math.min(iloscDoWyprodukowania, programy.get(0)));
            }
        }
        return produkty;
    }

    public abstract String coProdukuje(Info info, Zasoby produktywnosc, int bonus, Kariera k);
}