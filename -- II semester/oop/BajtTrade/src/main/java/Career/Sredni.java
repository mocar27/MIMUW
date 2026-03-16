package Career;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;
import StockMarket.*;

public class Sredni extends Kariera {

    protected int historia_spekulanta_sredniego;

    private double obliczSrednia(List<Zasoby> cenySrednie, String typ) {
        double wynik = 0;
        int iterator = cenySrednie.size() - 1;

        switch (typ) {
            case "ubrania":
                for (int i = 0; i < historia_spekulanta_sredniego; i++) {
                    wynik += cenySrednie.get(iterator).getUbrania(0);
                    iterator--;
                }
                break;
            case "narzedzia":
                for (int i = 0; i < historia_spekulanta_sredniego; i++) {
                    wynik += cenySrednie.get(iterator).getNarzedzia(0);
                    iterator--;
                }
                break;
            case "programy":
                for (int i = 0; i < historia_spekulanta_sredniego; i++) {
                    wynik += cenySrednie.get(iterator).getProgramy(0);
                    iterator--;
                }
                break;
            default:
                for (int i = 0; i < historia_spekulanta_sredniego; i++) {
                    wynik += cenySrednie.get(iterator).getJedzenie();
                    iterator--;
                }
                break;
        }

        return wynik / historia_spekulanta_sredniego;
    }

    private boolean czyMa(Zasoby z, String typ) {
        switch (typ) {
            case "ubrania":
                for (int i = 0; i < z.getSizeUbrania(); i++) {
                    if (z.getUbrania(i) != 0) {
                        return true;
                    }
                }
                return false;
            case "narzedzia":
                for (int i = 0; i < z.getSizeNarzedzia(); i++) {
                    if (z.getNarzedzia(i) != 0) {
                        return true;
                    }
                }
                return false;
            case "programy":
                for (int i = 0; i < historia_spekulanta_sredniego; i++) {
                    if (z.getProgramy(i) != 0) {
                        return true;
                    }
                }
                return false;
            case "jedzenie":
                return z.getJedzenie() != 0;
            default:
                return false;
        }

    }

    @Override
    public void dodajOfertyKupna(Info info, List<OfertaSpekulanta> ofertySpekulanta, Zasoby z, Gielda g) {

        double sredniaJedzenie = obliczSrednia(info.getCenySrednie(), "jedzenie");
        double sredniaUbrania = obliczSrednia(info.getCenySrednie(), "ubrania");
        double sredniaNarzedzia = obliczSrednia(info.getCenySrednie(), "narzedzia");
        double sredniaProgramy = obliczSrednia(info.getCenySrednie(), "programy");
        List<Double> temp = new ArrayList<>();
        temp.add(100.0);

        if (!czyMa(z, "jedzenie")) {
            ofertySpekulanta.add(new OfertaSpekulanta("jedzenie", 0.95 * sredniaJedzenie, temp));
        } else {
            ofertySpekulanta.add(new OfertaSpekulanta("jedzenie", 0.9 * sredniaJedzenie, temp));
        }

        if (!czyMa(z, "ubrania")) {
            ofertySpekulanta.add(new OfertaSpekulanta("ubrania", 0.95 * sredniaUbrania, temp));
        } else {
            ofertySpekulanta.add(new OfertaSpekulanta("ubrania", 0.9 * sredniaUbrania, temp));
        }

        if (!czyMa(z, "narzedzia")) {
            ofertySpekulanta.add(new OfertaSpekulanta("narzedzia", 0.95 * sredniaNarzedzia, temp));
        } else {
            ofertySpekulanta.add(new OfertaSpekulanta("narzedzia", 0.9 * sredniaNarzedzia, temp));
        }

        if (!czyMa(z, "programy")) {
            ofertySpekulanta.add(new OfertaSpekulanta("programy", 0.95 * sredniaProgramy, temp));
        } else {
            ofertySpekulanta.add(new OfertaSpekulanta("programy", 0.9 * sredniaProgramy, temp));
        }
    }

    @Override
    public void dodajOfertySprzedazy(Info info, List<OfertaSpekulanta> ofertySpekulanta, Zasoby z, Gielda g) {

        double sredniaJedzenie = obliczSrednia(info.getCenySrednie(), "jedzenie");
        double sredniaUbrania = obliczSrednia(info.getCenySrednie(), "ubrania");
        double sredniaNarzedzia = obliczSrednia(info.getCenySrednie(), "narzedzia");
        double sredniaProgramy = obliczSrednia(info.getCenySrednie(), "programy");


        if (czyMa(z, "jedzenie")) {
            List<Double> jedzenie = new ArrayList<>();
            if (z.getJedzenie() != 0) {
                jedzenie.add(z.getJedzenie());
                z.modyfikujJedzenie(0);
                ofertySpekulanta.add(new OfertaSpekulanta("jedzenie", 1.1 * sredniaJedzenie, jedzenie));
                g.modyfikujListeSprzedazy(info.getDzien(), "jedzenie", jedzenie);
            }
        }

        if (czyMa(z, "ubrania")) {
            List<Double> ubrania = new ArrayList<>();
            for (int i = 0; i < z.getSizeUbrania(); i++) {
                ubrania.add(z.getUbrania(i));
                z.modyfikujUbrania(i, 0);
            }
            ofertySpekulanta.add(new OfertaSpekulanta("ubrania", 1.1 * sredniaUbrania, ubrania));
            g.modyfikujListeSprzedazy(info.getDzien(), "ubrania", ubrania);
        }

        if (czyMa(z, "narzedzia")) {
            List<Double> narzedzia = new ArrayList<>();
            for (int i = 0; i < z.getSizeNarzedzia(); i++) {
                narzedzia.add(z.getNarzedzia(i));
                z.modyfikujNarzedzia(i, 0);
            }
            ofertySpekulanta.add(new OfertaSpekulanta("narzedzia", 1.1 * sredniaNarzedzia, narzedzia));
            g.modyfikujListeSprzedazy(info.getDzien(), "narzedzia", narzedzia);
        }

        if (czyMa(z, "programy")) {
            List<Double> programy = new ArrayList<>();
            for (int i = 0; i < z.getSizeProgramy(); i++) {
                programy.add(z.getProgramy(i));
                z.modyfikujProgramy(i, 0);
            }
            ofertySpekulanta.add(new OfertaSpekulanta("programy", 1.1 * sredniaProgramy, programy));
            g.modyfikujListeSprzedazy(info.getDzien(), "programy", programy);
        }
    }

    public int getHistoria_spekulanta_sredniego() {
        return historia_spekulanta_sredniego;
    }

    public void setHistoria_spekulanta_sredniego(int historia_spekulanta_sredniego) {
        this.historia_spekulanta_sredniego = historia_spekulanta_sredniego;
    }
}