package StockMarket;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;
import Career.*;
import Players.*;

public abstract class Gielda {

    protected List<Zasoby> listaSprzedazy;
    private double cenaJedzenie;
    private double iloscJedzenie;
    private double minJedzenie;
    private double maxJedzenie;
    private double cenaUbrania;
    private double iloscUbrania;
    private double minUbrania;
    private double maxUbrania;
    private double cenaNarzedzia;
    private double iloscNarzedzia;
    private double minNarzedzia;
    private double maxNarzedzia;
    private double cenaProgramy;
    private double iloscProgramy;
    private double minProgramy;
    private double maxProgramy;

    public Kariera wybierzKariere(int modulo) {

        double ubrania = 0;
        double narzedzia = 0;
        double jedzenie = 0;
        double programy = 0;
        int iterator = listaSprzedazy.size() - 1;
        while (modulo > 0 && iterator >= 0) {
            for (int i = 0; i < listaSprzedazy.get(iterator).getSizeUbrania(); i++) {
                ubrania += listaSprzedazy.get(iterator).getUbrania(i);
            }
            for (int i = 0; i < listaSprzedazy.get(iterator).getSizeNarzedzia(); i++) {
                narzedzia += listaSprzedazy.get(iterator).getNarzedzia(i);
            }
            for (int i = 0; i < listaSprzedazy.get(iterator).getSizeProgramy(); i++) {
                programy += listaSprzedazy.get(iterator).getProgramy(i);
            }
            jedzenie += listaSprzedazy.get(iterator).getJedzenie();
            iterator--;
            modulo--;
        }

        double maks1 = Math.max(ubrania, narzedzia);
        double maks2 = Math.max(jedzenie, programy);
        double maks3 = Math.max(maks1, maks2);

        if (maks3 == 0) {
            return new Programista();
        } else {
            if (maks3 == programy) {
                return new Programista();
            } else if (maks3 == narzedzia) {
                return new Inzynier();
            } else if (maks3 == ubrania) {
                return new Rzemieslnik();
            } else {
                return new Rolnik();
            }
        }
    }

    public void gieldaInitialize() {
        this.listaSprzedazy = new ArrayList<>();
        this.listaSprzedazy.add(new Zasoby(0, 0, 0, 0, 0));
    }

    public List<Zasoby> getListaSprzedazy() {
        return listaSprzedazy;
    }

    public void listaSprzedazyDodajDzien() {
        listaSprzedazy.add(new Zasoby(0, 0, 0, 0, 0));
    }

    public void modyfikujListeSprzedazy(int dzien, String typ, List<Double> ilosci) {

        if (typ.equals("jedzenie")) {
            listaSprzedazy.get(dzien).modyfikujJedzenie(
                    listaSprzedazy.get(dzien).getJedzenie() + ilosci.get(0));
        } else if (typ.equals("ubrania")) {
            double suma = 0;
            for (Double u : ilosci) {
                suma += u;
            }
            listaSprzedazy.get(dzien).modyfikujUbrania(0,
                    listaSprzedazy.get(dzien).getUbrania(0) + suma);
        } else if (typ.equals("narzedzia")) {
            double suma = 0;
            for (Double n : ilosci) {
                suma += n;
            }
            listaSprzedazy.get(dzien).modyfikujNarzedzia(0,
                    listaSprzedazy.get(dzien).getNarzedzia(0) + suma);
        } else if (typ.equals("programy")) {
            double suma = 0;
            for (Double p : ilosci) {
                suma += p;
            }
            listaSprzedazy.get(dzien).modyfikujProgramy(0,
                    listaSprzedazy.get(dzien).getProgramy(0) + suma);
        }
    }


    private void zrealizujOferteSprzedazy(Robotnik r, Spekulant s, OfertaRobotnika ofertaR,
                                          OfertaSpekulanta ofertaS, int poziom, String typ) {
        if (ofertaS.getIloscPrzedmiotow(0) >
                ofertaR.getIloscPrzedmiotow(poziom)) {

            r.getZasoby().modyfikujDiamenty(r.getZasoby().getDiamenty() +
                    ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom));
            s.getZasoby().modyfikujDiamenty(s.getZasoby().getDiamenty() -
                    ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom));

            if (typ.equals("jedzenie")) {
                this.cenaJedzenie += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscJedzenie += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minJedzenie) {
                    this.minJedzenie = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxJedzenie) {
                    this.maxJedzenie = ofertaS.getCena();
                }
            } else if (typ.equals("ubrania")) {
                this.cenaUbrania += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscUbrania += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minUbrania) {
                    this.minUbrania = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxUbrania) {
                    this.maxUbrania = ofertaS.getCena();
                }
            } else if (typ.equals("narzedzia")) {
                this.cenaNarzedzia += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscNarzedzia += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minNarzedzia) {
                    this.minNarzedzia = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxNarzedzia) {
                    this.maxNarzedzia = ofertaS.getCena();
                }
            } else if (typ.equals("programy")) {
                this.cenaProgramy += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscProgramy += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minProgramy) {
                    this.minProgramy = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxProgramy) {
                    this.maxProgramy = ofertaS.getCena();
                }
            }

            ofertaR.setIloscPrzedmiotow(poziom, 0);
            ofertaS.setIloscPrzedmiotow(0,
                    ofertaS.getIloscPrzedmiotow(0) - ofertaR.getIloscPrzedmiotow(poziom));
        } else {
            r.getZasoby().modyfikujDiamenty(r.getZasoby().getDiamenty() +
                    ofertaS.getCena() * ofertaS.getIloscPrzedmiotow(0));
            s.getZasoby().modyfikujDiamenty(s.getZasoby().getDiamenty() -
                    ofertaS.getCena() * ofertaS.getIloscPrzedmiotow(0));

            if (typ.equals("jedzenie")) {
                this.cenaJedzenie += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscJedzenie += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minJedzenie) {
                    this.minJedzenie = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxJedzenie) {
                    this.maxJedzenie = ofertaS.getCena();
                }
            } else if (typ.equals("ubrania")) {
                this.cenaUbrania += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscUbrania += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minUbrania) {
                    this.minUbrania = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxUbrania) {
                    this.maxUbrania = ofertaS.getCena();
                }
            } else if (typ.equals("narzedzia")) {
                this.cenaNarzedzia += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscNarzedzia += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minNarzedzia) {
                    this.minNarzedzia = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxNarzedzia) {
                    this.maxNarzedzia = ofertaS.getCena();
                }
            } else if (typ.equals("programy")) {
                this.cenaProgramy += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscProgramy += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minProgramy) {
                    this.minProgramy = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxProgramy) {
                    this.maxProgramy = ofertaS.getCena();
                }
            }

            ofertaR.setIloscPrzedmiotow(poziom, ofertaR.getIloscPrzedmiotow(poziom)
                    - ofertaS.getIloscPrzedmiotow(0));
            ofertaS.setIloscPrzedmiotow(0, 0);

        }
    }

    private void zrealizujOferteKupna(Robotnik r, Spekulant s, OfertaRobotnika ofertaR,
                                      OfertaSpekulanta ofertaS, int poziom, String typ) {
        if (ofertaS.getIloscPrzedmiotow(0) >
                ofertaR.getIloscPrzedmiotow(poziom)) {

            r.getZasoby().modyfikujDiamenty(r.getZasoby().getDiamenty() -
                    ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom));
            s.getZasoby().modyfikujDiamenty((s.getZasoby().getDiamenty() +
                    ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom)));

            if (typ.equals("jedzenie")) {
                this.cenaJedzenie += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscJedzenie += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minJedzenie) {
                    this.minJedzenie = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxJedzenie) {
                    this.maxJedzenie = ofertaS.getCena();
                }
            } else if (typ.equals("ubrania")) {
                this.cenaUbrania += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscUbrania += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minUbrania) {
                    this.minUbrania = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxUbrania) {
                    this.maxUbrania = ofertaS.getCena();
                }
            } else if (typ.equals("narzedzia")) {
                this.cenaNarzedzia += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscNarzedzia += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minNarzedzia) {
                    this.minNarzedzia = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxNarzedzia) {
                    this.maxNarzedzia = ofertaS.getCena();
                }
            } else if (typ.equals("programy")) {
                this.cenaProgramy += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscProgramy += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minProgramy) {
                    this.minProgramy = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxProgramy) {
                    this.maxProgramy = ofertaS.getCena();
                }
            }


            ofertaR.setIloscPrzedmiotow(poziom, 0);
            ofertaS.setIloscPrzedmiotow(0,
                    ofertaS.getIloscPrzedmiotow(0) - ofertaR.getIloscPrzedmiotow(poziom));
        } else {
            r.getZasoby().modyfikujDiamenty(r.getZasoby().getDiamenty() -
                    ofertaS.getCena() * ofertaS.getIloscPrzedmiotow(0));
            s.getZasoby().modyfikujDiamenty((s.getZasoby().getDiamenty() +
                    ofertaS.getCena() * ofertaS.getIloscPrzedmiotow(0)));

            if (typ.equals("jedzenie")) {
                this.cenaJedzenie += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscJedzenie += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minJedzenie) {
                    this.minJedzenie = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxJedzenie) {
                    this.maxJedzenie = ofertaS.getCena();
                }
            } else if (typ.equals("ubrania")) {
                this.cenaUbrania += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscUbrania += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minUbrania) {
                    this.minUbrania = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxUbrania) {
                    this.maxUbrania = ofertaS.getCena();
                }
            } else if (typ.equals("narzedzia")) {
                this.cenaNarzedzia += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscNarzedzia += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minNarzedzia) {
                    this.minNarzedzia = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxNarzedzia) {
                    this.maxNarzedzia = ofertaS.getCena();
                }
            } else if (typ.equals("programy")) {
                this.cenaProgramy += ofertaS.getCena() * ofertaR.getIloscPrzedmiotow(poziom);
                this.iloscProgramy += ofertaR.getIloscPrzedmiotow(poziom);
                if (ofertaS.getCena() < this.minProgramy) {
                    this.minProgramy = ofertaS.getCena();
                }
                if (ofertaS.getCena() < this.maxProgramy) {
                    this.maxProgramy = ofertaS.getCena();
                }
            }

            ofertaR.setIloscPrzedmiotow(poziom,
                    ofertaR.getIloscPrzedmiotow(poziom) - ofertaS.getIloscPrzedmiotow(0));
            ofertaS.setIloscPrzedmiotow(0, 0);

        }
    }

    public abstract void dopasujOferty(Info info, Robotnik[] robotnicy, Spekulant[] spekulanci); // bedzie sortowac i bedzie dopasowywac

    public void dopasuj(Info info, Robotnik[] robotnicy, Spekulant[] spekulanci) {

        this.cenaJedzenie = 0;
        this.iloscJedzenie = 1;
        this.minJedzenie = Double.MAX_VALUE;
        this.maxJedzenie = 0;
        this.cenaUbrania = 0;
        this.iloscUbrania = 1;
        this.minUbrania = Double.MAX_VALUE;
        this.maxUbrania = 0;
        this.cenaNarzedzia = 0;
        this.iloscNarzedzia = 1;
        this.minNarzedzia = Double.MAX_VALUE;
        this.maxNarzedzia = 0;
        this.cenaProgramy = 0;
        this.iloscProgramy = 1;
        this.minProgramy = Double.MAX_VALUE;
        this.maxProgramy = 0;

        for (Robotnik r : robotnicy) {

            for (OfertaRobotnika ofertaRobotnika : r.getOfertySprzedazy()) {

                if (ofertaRobotnika.getTyp().equals("jedzenie")) {
                    int i = 0;
                    while (ofertaRobotnika.getIloscPrzedmiotow(0) > 0 && i < spekulanci.length) {
                        List<OfertaSpekulanta> ofertySpekulanta = spekulanci[i].getOfertyKupna();
                        for (int j = 0; j < ofertySpekulanta.size(); j++) {
                            if (ofertySpekulanta.get(i).getTyp().equals("jedzenie")) {
                                zrealizujOferteSprzedazy(r, spekulanci[i], ofertaRobotnika,
                                        ofertySpekulanta.get(i), 0, "jedzenie");
                            }
                        }
                        i++;
                    }
                } else if (ofertaRobotnika.getTyp().equals("ubrania")) {
                    int i = 0;
                    for (int poziom = 0; poziom < ofertaRobotnika.getSizePrzedmiotow(); poziom++) {

                        while (ofertaRobotnika.getIloscPrzedmiotow(poziom) > 0 && i < spekulanci.length) {
                            List<OfertaSpekulanta> ofertySpekulanta = spekulanci[i].getOfertyKupna();
                            for (int j = 0; j < ofertySpekulanta.size(); j++) {
                                zrealizujOferteSprzedazy(r, spekulanci[i], ofertaRobotnika,
                                        ofertySpekulanta.get(i), poziom, "ubrania");
                            }
                            i++;
                        }
                    }
                } else if (ofertaRobotnika.getTyp().equals("narzedzia")) {
                    int i = 0;
                    for (int poziom = 0; poziom < ofertaRobotnika.getSizePrzedmiotow(); poziom++) {
                        while (ofertaRobotnika.getIloscPrzedmiotow(poziom) > 0 && i < spekulanci.length) {
                            List<OfertaSpekulanta> ofertySpekulanta = spekulanci[i].getOfertyKupna();
                            for (int j = 0; j < ofertySpekulanta.size(); j++) {
                                if (ofertySpekulanta.get(i).getTyp().equals("narzedzia")) {
                                    zrealizujOferteSprzedazy(r, spekulanci[i], ofertaRobotnika,
                                            ofertySpekulanta.get(i), poziom, "narzedzia");
                                }
                            }
                            i++;
                        }
                    }
                } else if (ofertaRobotnika.getTyp().equals("programy")) {
                    int i = 0;
                    for (int poziom = 0; poziom < ofertaRobotnika.getSizePrzedmiotow(); poziom++) {
                        while (ofertaRobotnika.getIloscPrzedmiotow(poziom) > 0 && i < spekulanci.length) {
                            List<OfertaSpekulanta> ofertySpekulanta = spekulanci[i].getOfertyKupna();
                            for (int j = 0; j < ofertySpekulanta.size(); j++) {
                                if (ofertySpekulanta.get(i).getTyp().equals("programy")) {
                                    zrealizujOferteSprzedazy(r, spekulanci[i], ofertaRobotnika,
                                            ofertySpekulanta.get(i), poziom, "programy");
                                }
                            }
                            i++;
                        }
                    }
                }
            }

            for (OfertaRobotnika ofertaRobotnika : r.getOfertyKupna()) {

                if (ofertaRobotnika.getTyp().equals("jedzenie")) {
                    int i = 0;
                    while (ofertaRobotnika.getIloscPrzedmiotow(0) > 0 && i < spekulanci.length) {
                        List<OfertaSpekulanta> ofertySpekulanta = spekulanci[i].getOfertySprzedazy();
                        for (int j = 0; j < ofertySpekulanta.size(); j++) {
                            if (ofertySpekulanta.get(i).getTyp().equals("jedzenie")) {
                                zrealizujOferteKupna(r, spekulanci[i], ofertaRobotnika,
                                        ofertySpekulanta.get(i), 0, "jedzenie");
                            }
                        }
                        i++;
                    }
                } else if (ofertaRobotnika.getTyp().equals("ubrania")) {
                    int i = 0;
                    for (int poziom = 0; poziom < ofertaRobotnika.getSizePrzedmiotow(); poziom++) {

                        while (ofertaRobotnika.getIloscPrzedmiotow(poziom) > 0 && i < spekulanci.length) {
                            List<OfertaSpekulanta> ofertySpekulanta = spekulanci[i].getOfertySprzedazy();
                            for (int j = 0; j < ofertySpekulanta.size(); j++) {
                                zrealizujOferteKupna(r, spekulanci[i], ofertaRobotnika,
                                        ofertySpekulanta.get(i), poziom, "ubrania");
                            }
                            i++;
                        }
                    }
                } else if (ofertaRobotnika.getTyp().equals("narzedzia")) {
                    int i = 0;
                    for (int poziom = 0; poziom < ofertaRobotnika.getSizePrzedmiotow(); poziom++) {
                        while (ofertaRobotnika.getIloscPrzedmiotow(poziom) > 0 && i < spekulanci.length) {
                            List<OfertaSpekulanta> ofertySpekulanta = spekulanci[i].getOfertySprzedazy();
                            for (int j = 0; j < ofertySpekulanta.size(); j++) {
                                if (ofertySpekulanta.get(i).getTyp().equals("narzedzia")) {
                                    zrealizujOferteKupna(r, spekulanci[i], ofertaRobotnika,
                                            ofertySpekulanta.get(i), poziom, "narzedzia");
                                }
                            }
                            i++;
                        }
                    }
                } else if (ofertaRobotnika.getTyp().equals("programy")) {
                    int i = 0;
                    for (int poziom = 0; poziom < ofertaRobotnika.getSizePrzedmiotow(); poziom++) {
                        while (ofertaRobotnika.getIloscPrzedmiotow(poziom) > 0 && i < spekulanci.length) {
                            List<OfertaSpekulanta> ofertySpekulanta = spekulanci[i].getOfertySprzedazy();
                            for (int j = 0; j < ofertySpekulanta.size(); j++) {
                                if (ofertySpekulanta.get(i).getTyp().equals("programy")) {
                                    zrealizujOferteKupna(r, spekulanci[i], ofertaRobotnika,
                                            ofertySpekulanta.get(i), poziom, "programy");
                                }
                            }
                            i++;
                        }
                    }
                }
            }
        }

        Zasoby cenySrednie = new Zasoby(0, cenaUbrania / iloscUbrania, cenaNarzedzia / iloscNarzedzia,
                cenaJedzenie / iloscJedzenie, cenaProgramy / iloscProgramy);
        Zasoby cenyMinimalne = new Zasoby(0, minUbrania, minNarzedzia, minJedzenie, minProgramy);
        Zasoby cenyMaksymalne = new Zasoby(0, maxUbrania, maxNarzedzia, maxJedzenie, maxProgramy);

        info.dodajCenySrednie(cenySrednie);
        info.dodajCenyMin(cenyMinimalne);
        info.dodajCenyMax(cenyMaksymalne);

    }
}