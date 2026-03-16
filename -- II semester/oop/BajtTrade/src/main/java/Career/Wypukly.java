package Career;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;
import StockMarket.*;

public class Wypukly extends Kariera {

    @Override
    public void dodajOfertyKupna(Info info, List<OfertaSpekulanta> ofertySpekulanta, Zasoby z, Gielda g) {

        if (info.getDzien() < 3) {
            return;
        }

        if (info.getCenySrednie().get(info.getDzien() - 3).getUbrania(0) <
                info.getCenySrednie().get(info.getDzien() - 2).getUbrania(0)) {

            if (info.getCenySrednie().get(info.getDzien() - 1).getUbrania(0) <
                    info.getCenySrednie().get(info.getDzien() - 2).getUbrania(0)) {
                List<Double> ubrania = new ArrayList<>();
                for (int i = 0; i < z.getSizeUbrania(); i++) {
                    ubrania.add(100.0);
                }
                ofertySpekulanta.add(new OfertaSpekulanta("ubrania",
                        (info.getCenySrednie().get(info.getDzien() - 1).getUbrania(0)) * 0.9, ubrania));
            }
        }
        if (info.getCenySrednie().get(info.getDzien() - 3).getNarzedzia(0) <
                info.getCenySrednie().get(info.getDzien() - 2).getNarzedzia(0)) {

            if (info.getCenySrednie().get(info.getDzien() - 1).getNarzedzia(0) <
                    info.getCenySrednie().get(info.getDzien() - 2).getNarzedzia(0)) {
                List<Double> narzedzia = new ArrayList<>();
                for (int i = 0; i < z.getSizeNarzedzia(); i++) {
                    narzedzia.add(100.0);
                }
                ofertySpekulanta.add(new OfertaSpekulanta("narzedzia",
                        (info.getCenySrednie().get(info.getDzien() - 1).getNarzedzia(0)) * 0.9, narzedzia));
            }
        }
        if (info.getCenySrednie().get(info.getDzien() - 3).getProgramy(0) <
                info.getCenySrednie().get(info.getDzien() - 2).getProgramy(0)) {

            if (info.getCenySrednie().get(info.getDzien() - 1).getProgramy(0) <
                    info.getCenySrednie().get(info.getDzien() - 2).getProgramy(0)) {
                List<Double> programy = new ArrayList<>();
                for (int i = 0; i < z.getSizeProgramy(); i++) {
                    programy.add(100.0);
                }
                ofertySpekulanta.add(new OfertaSpekulanta("programy",
                        (info.getCenySrednie().get(info.getDzien() - 1).getProgramy(0)) * 0.9, programy));
            }
        }
        if (info.getCenySrednie().get(info.getDzien() - 3).getJedzenie() <
                info.getCenySrednie().get(info.getDzien() - 2).getJedzenie()) {

            if (info.getCenySrednie().get(info.getDzien() - 1).getJedzenie() <
                    info.getCenySrednie().get(info.getDzien() - 2).getJedzenie()) {
                List<Double> jedzenie = new ArrayList<>();
                jedzenie.add(100.0);
                ofertySpekulanta.add(new OfertaSpekulanta("jedzenie",
                        (info.getCenySrednie().get(info.getDzien() - 1).getJedzenie()) * 0.9, jedzenie));
            }
        }
    }

    @Override
    public void dodajOfertySprzedazy(Info info, List<OfertaSpekulanta> ofertySpekulanta, Zasoby z, Gielda g) {

        if (info.getDzien() < 3) {
            return;
        }

        if (info.getCenySrednie().get(info.getDzien() - 3).getUbrania(0) >
                info.getCenySrednie().get(info.getDzien() - 2).getUbrania(0)) {

            if (info.getCenySrednie().get(info.getDzien() - 1).getUbrania(0) >
                    info.getCenySrednie().get(info.getDzien() - 2).getUbrania(0)) {
                List<Double> ubrania = new ArrayList<>();
                double suma = 0;
                for (int i = 0; i < z.getSizeUbrania(); i++) {
                    suma += z.getUbrania(i);
                }
                if (suma != 0) {
                    for (int i = 0; i < z.getSizeUbrania(); i++) {
                        if (z.getUbrania(i) != 0) {
                            ubrania.add(z.getUbrania(i));
                            z.modyfikujUbrania(i, 0);
                        }
                    }
                }
                ofertySpekulanta.add(new OfertaSpekulanta("ubrania",
                        (info.getCenySrednie().get(info.getDzien() - 1).getUbrania(0)) * 1.1, ubrania));
                g.modyfikujListeSprzedazy(info.getDzien(), "ubrania", ubrania);
            }
        }
        if (info.getCenySrednie().get(info.getDzien() - 3).getNarzedzia(0) >
                info.getCenySrednie().get(info.getDzien() - 2).getNarzedzia(0)) {

            if (info.getCenySrednie().get(info.getDzien() - 1).getNarzedzia(0) >
                    info.getCenySrednie().get(info.getDzien() - 2).getNarzedzia(0)) {
                List<Double> narzedzia = new ArrayList<>();
                double suma = 0;
                for (int i = 0; i < z.getSizeNarzedzia(); i++) {
                    suma += z.getNarzedzia(i);
                }
                if (suma != 0) {
                    for (int i = 0; i < z.getSizeNarzedzia(); i++) {
                        narzedzia.add(z.getNarzedzia(i));
                        z.modyfikujNarzedzia(i, 0);
                    }
                }
                ofertySpekulanta.add(new OfertaSpekulanta("narzedzia",
                        (info.getCenySrednie().get(info.getDzien() - 1).getNarzedzia(0)) * 1.1, narzedzia));
                g.modyfikujListeSprzedazy(info.getDzien(), "narzedzia", narzedzia);
            }
        }
        if (info.getCenySrednie().get(info.getDzien() - 3).getProgramy(0) >
                info.getCenySrednie().get(info.getDzien() - 2).getProgramy(0)) {

            if (info.getCenySrednie().get(info.getDzien() - 1).getProgramy(0) >
                    info.getCenySrednie().get(info.getDzien() - 2).getProgramy(0)) {
                List<Double> programy = new ArrayList<>();
                double suma = 0;
                for (int i = 0; i < z.getSizeProgramy(); i++) {
                    suma += z.getProgramy(i);
                }
                if (suma != 0) {
                    for (int i = 0; i < z.getSizeProgramy(); i++) {
                        if (z.getProgramy(i) != 0) {
                            programy.add(z.getProgramy(i));
                            z.modyfikujProgramy(i, 0);
                        }
                    }
                }
                ofertySpekulanta.add(new OfertaSpekulanta("programy",
                        (info.getCenySrednie().get(info.getDzien() - 1).getProgramy(0)) * 1.1, programy));
                g.modyfikujListeSprzedazy(info.getDzien(), "programy", programy);
            }

        }
        if (info.getCenySrednie().get(info.getDzien() - 3).getJedzenie() >
                info.getCenySrednie().get(info.getDzien() - 2).getJedzenie()) {

            if (info.getCenySrednie().get(info.getDzien() - 1).getJedzenie() >
                    info.getCenySrednie().get(info.getDzien() - 2).getJedzenie()) {
                List<Double> jedzenie = new ArrayList<>();
                if (z.getJedzenie() != 0) {
                    jedzenie.add(z.getJedzenie());
                    z.modyfikujJedzenie(0);
                    ofertySpekulanta.add(new OfertaSpekulanta("jedzenie",
                            (info.getCenySrednie().get(info.getDzien() - 1).getJedzenie()) * 1.1, jedzenie));
                    g.modyfikujListeSprzedazy(info.getDzien(), "jedzenie", jedzenie);
                }
            }
        }
    }
}