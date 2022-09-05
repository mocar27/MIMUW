package Career;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;
import StockMarket.*;

public class RegulujacyRynek extends Kariera {

    @Override
    public void dodajOfertyKupna(Info info, List<OfertaSpekulanta> ofertySpekulanta, Zasoby z, Gielda g) {
        if (info.getDzien() == 1) {
            return;
        }

        List<Double> temp = new ArrayList<>();
        temp.add(100.0);
        double srednia = info.getCenySrednie().get(info.getDzien() - 1).getJedzenie();
        double iloscDniaPoprzedniego = g.getListaSprzedazy().get(info.getDzien() - 1).getJedzenie();
        double iloscTegoDnia = g.getListaSprzedazy().get(info.getDzien()).getJedzenie();
        ofertySpekulanta.add(new OfertaSpekulanta("jedzenie",
                0.9 * srednia * (iloscTegoDnia / Math.max(iloscDniaPoprzedniego, 1)), temp));

        srednia = info.getCenySrednie().get(info.getDzien() - 1).getUbrania(0);
        iloscDniaPoprzedniego = g.getListaSprzedazy().get(info.getDzien() - 1).getUbrania(0);
        iloscTegoDnia = g.getListaSprzedazy().get(info.getDzien()).getUbrania(0);
        ofertySpekulanta.add(new OfertaSpekulanta("ubrania",
                0.9 * srednia * (iloscTegoDnia / Math.max(iloscDniaPoprzedniego, 1)), temp));

        srednia = info.getCenySrednie().get(info.getDzien() - 1).getNarzedzia(0);
        iloscDniaPoprzedniego = g.getListaSprzedazy().get(info.getDzien() - 1).getNarzedzia(0);
        iloscTegoDnia = g.getListaSprzedazy().get(info.getDzien()).getNarzedzia(0);
        ofertySpekulanta.add(new OfertaSpekulanta("narzedzia",
                0.9 * srednia * (iloscTegoDnia / Math.max(iloscDniaPoprzedniego, 1)), temp));

        srednia = info.getCenySrednie().get(info.getDzien() - 1).getProgramy(0);
        iloscDniaPoprzedniego = g.getListaSprzedazy().get(info.getDzien() - 1).getProgramy(0);
        iloscTegoDnia = g.getListaSprzedazy().get(info.getDzien()).getProgramy(0);
        ofertySpekulanta.add(new OfertaSpekulanta("programy",
                0.9 * srednia * (iloscTegoDnia / Math.max(iloscDniaPoprzedniego, 1)), temp));
    }

    @Override
    public void dodajOfertySprzedazy(Info info, List<OfertaSpekulanta> ofertySpekulanta, Zasoby z, Gielda g) {
        if (info.getDzien() == 1) {
            return;
        }

        if (z.getJedzenie() != 0) {
            List<Double> jedzenie = new ArrayList<>();
            jedzenie.add(z.getJedzenie());
            double srednia = info.getCenySrednie().get(info.getDzien() - 1).getJedzenie();
            double iloscDniaPoprzedniego = g.getListaSprzedazy().get(info.getDzien() - 1).getJedzenie();
            double iloscTegoDnia = g.getListaSprzedazy().get(info.getDzien()).getJedzenie();
            ofertySpekulanta.add(new OfertaSpekulanta("jedzenie",
                    1.1 * srednia * (iloscTegoDnia / Math.max(iloscDniaPoprzedniego, 1)), jedzenie));
            g.modyfikujListeSprzedazy(info.getDzien(), "jedzenie", jedzenie);

        }

        double suma = 0;
        for (int i = 0; i < z.getSizeUbrania(); i++) {
            suma += z.getUbrania(i);
        }

        if (suma != 0) {
            List<Double> ubrania = new ArrayList<>();
            for (int i = 0; i < z.getSizeUbrania(); i++) {
                ubrania.add(z.getUbrania(i));
            }
            double srednia = info.getCenySrednie().get(info.getDzien() - 1).getUbrania(0);
            double iloscDniaPoprzedniego = g.getListaSprzedazy().get(info.getDzien() - 1).getUbrania(0);
            double iloscTegoDnia = g.getListaSprzedazy().get(info.getDzien()).getUbrania(0);
            ofertySpekulanta.add(new OfertaSpekulanta("ubrania",
                    1.1 * srednia * (iloscTegoDnia / Math.max(iloscDniaPoprzedniego, 1)), ubrania));
            g.modyfikujListeSprzedazy(info.getDzien(), "ubrania", ubrania);

        }

        suma = 0;
        for (int i = 0; i < z.getSizeNarzedzia(); i++) {
            suma += z.getNarzedzia(i);
        }

        if (suma != 0) {
            List<Double> narzedzia = new ArrayList<>();
            for (int i = 0; i < z.getSizeNarzedzia(); i++) {
                narzedzia.add(z.getNarzedzia(i));
            }
            double srednia = info.getCenySrednie().get(info.getDzien() - 1).getNarzedzia(0);
            double iloscDniaPoprzedniego = g.getListaSprzedazy().get(info.getDzien() - 1).getNarzedzia(0);
            double iloscTegoDnia = g.getListaSprzedazy().get(info.getDzien()).getNarzedzia(0);
            ofertySpekulanta.add(new OfertaSpekulanta("narzedzia",
                    1.1 * srednia * (iloscTegoDnia / Math.max(iloscDniaPoprzedniego, 1)), narzedzia));
            g.modyfikujListeSprzedazy(info.getDzien(), "narzedzia", narzedzia);

        }

        suma = 0;
        for (int i = 0; i < z.getSizeProgramy(); i++) {
            suma += z.getProgramy(i);
        }

        if (suma != 0) {
            List<Double> programy = new ArrayList<>();
            for (int i = 0; i < z.getSizeProgramy(); i++) {
                programy.add(z.getProgramy(i));
            }
            double srednia = info.getCenySrednie().get(info.getDzien() - 1).getProgramy(0);
            double iloscDniaPoprzedniego = g.getListaSprzedazy().get(info.getDzien() - 1).getProgramy(0);
            double iloscTegoDnia = g.getListaSprzedazy().get(info.getDzien()).getProgramy(0);
            ofertySpekulanta.add(new OfertaSpekulanta("programy",
                    1.1 * srednia * (iloscTegoDnia / Math.max(iloscDniaPoprzedniego, 1)), programy));
            g.modyfikujListeSprzedazy(info.getDzien(), "programy", programy);

        }
    }
}