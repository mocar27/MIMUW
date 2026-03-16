package Players;

import com.squareup.moshi.Json;
import com.squareup.moshi.JsonDataException;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;
import MyPair.*;
import Career.*;
import Buying.*;
import Production.*;
import Learning.*;
import StockMarket.*;

public class Robotnik extends Agent {

    private final String zmiana;
    @Json(ignore = true)
    private Pair[] poziomyKarier;
    private int poziom;
    private final Zasoby produktywnosc;
    @Json(ignore = true)
    private int dlugoscGlodowania;
    @Json(ignore = true)
    private List<Integer> dlugoscNoszeniaUbran;
    @Json(ignore = true)
    private List<OfertaRobotnika> ofertyKupna;
    @Json(ignore = true)
    private List<OfertaRobotnika> ofertySprzedazy;

    public int getID() {
        return this.id;
    }

    public double getZasobyGetDiamenty() {
        return this.zasoby.getDiamenty();
    }

    public List<OfertaRobotnika> getOfertyKupna() {
        return ofertyKupna;
    }

    public List<OfertaRobotnika> getOfertySprzedazy() {
        return ofertySprzedazy;
    }

    public Robotnik(int id, int poziom, Kariera kariera, Kupowanie kupowanie,
                    Produkcja produkcja, Uczenie uczenie,
                    String zmiana, Zasoby z, Zasoby prod) {
        super(id, kariera, kupowanie, produkcja, uczenie, z);


        this.poziom = poziom;
        this.zmiana = zmiana;
        if (prod.getProgramy(0) % 100 != 0 || prod.getJedzenie() % 100 != 0
                || prod.getNarzedzia(0) % 100 != 0 || prod.getDiamenty() % 100 != 0
                || prod.getSizeUbrania() % 100 != 0) {
            throw new JsonDataException("Nieprawidlowa liczba produktywnosci");
        }
        this.produktywnosc = prod;
        this.dlugoscGlodowania = 0;
    }

    public void robotnikInitialize() {
        this.poziomyKarier = new Pair[5];
        this.poziomyKarier[0] = new Pair(new Rolnik(), 1);
        this.poziomyKarier[1] = new Pair(new Gornik(), 1);
        this.poziomyKarier[2] = new Pair(new Rzemieslnik(), 1);
        this.poziomyKarier[3] = new Pair(new Inzynier(), 1);
        this.poziomyKarier[4] = new Pair(new Programista(), 1);

        this.dlugoscNoszeniaUbran = new ArrayList<>();
        this.dlugoscNoszeniaUbran.add(0);
        this.dlugoscNoszeniaUbran.add(0);
    }

    public String getZmiana() {
        return zmiana;
    }

    public void zmienKariere(Gielda g) {

        int modulo = Math.max(this.id % 17, 1);

        Kariera nowaKariera = g.wybierzKariere(modulo);

        if (this.kariera.getClass() == nowaKariera.getClass()) {
            poziom++;
        } else {
            for (int i = 0; i < 5; i++) {
                if (this.kariera.getClass() == poziomyKarier[i].getObiekt().getClass()) {
                    poziomyKarier[i].setPoziom(poziom);
                }
            }
            for (int i = 0; i < 5; i++) {
                if (nowaKariera.getClass() == poziomyKarier[i].getObiekt().getClass()) {
                    this.poziom = poziomyKarier[i].getPoziom();
                    this.kariera = (Kariera) poziomyKarier[i].getObiekt();
                }
            }
        }
    }

    public void uczSie() {
        dlugoscGlodowania = 0;
        poziom++;
    }

    public void pracuj(Info info, Gielda g) {

        int bonus = 0;

        if (poziom == 1) {
            bonus += 50;
        } else if (poziom == 2) {
            bonus += 150;
        } else if (poziom == 3) {
            bonus += 300;
        } else {
            bonus += 300;
            bonus += 25 * (poziom - 3);
        }

        if (this.dlugoscGlodowania == 1) {
            bonus -= 100;
        } else if (this.dlugoscGlodowania == 2) {
            bonus -= 300;
        }

        int sumaUbran = 0;
        for (int i = 0; i < this.getZasoby().getSizeUbrania(); i++) {
            sumaUbran += this.getZasoby().getUbrania(i);
        }

        if (sumaUbran < 100) {
            bonus -= info.getKara();
        }

        int sumaNarzedzi = 0;
        for (int i = 0; i < this.getZasoby().getSizeNarzedzia(); i++) {
            sumaNarzedzi += this.getZasoby().getNarzedzia(i) * i;
        }

        bonus += sumaNarzedzi;

        String typ = this.produkcja.coProdukuje(info, produktywnosc, bonus, this.kariera);

        if (typ.equals("diamenty") && this.kariera.getClass() != Gornik.class) {
            bonus = 1;
        } else if (typ.equals("jedzenie") && this.kariera.getClass() != Rolnik.class) {
            bonus = 1;
        } else if (typ.equals("ubrania") && this.kariera.getClass() != Rzemieslnik.class) {
            bonus = 1;
        } else if (typ.equals("narzedzia") && this.kariera.getClass() != Inzynier.class) {
            bonus = 1;
        } else if (typ.equals("programy") && this.kariera.getClass() != Programista.class) {
            bonus = 1;
        }

        List<Double> wyprodukowaneProdukty;
        if (this.kariera.getClass() == Programista.class) {
            wyprodukowaneProdukty = this.produkcja.produkuj(produktywnosc, bonus,
                    typ, poziom, this.getZasoby().getProgramyList());
        } else {
            wyprodukowaneProdukty = this.produkcja.produkuj(produktywnosc, bonus,
                    typ, 1, this.getZasoby().getProgramyList());
        }

        double iloscWyprodukowanych = 0;
        for (Double ilosc : wyprodukowaneProdukty) {
            iloscWyprodukowanych += ilosc;
        }


        if (typ.equals("diamenty")) {
            this.zasoby.modyfikujDiamenty(this.zasoby.getDiamenty() + wyprodukowaneProdukty.get(0));
        } else {
            this.ofertySprzedazy = new ArrayList<>();
            this.ofertySprzedazy.add(new OfertaRobotnika(typ, wyprodukowaneProdukty));
            g.modyfikujListeSprzedazy(info.getDzien(), typ, wyprodukowaneProdukty);
        }
        this.ofertyKupna = new ArrayList<>();
        this.kupowanie.wystawOfertyKupna(this.zasoby, iloscWyprodukowanych, ofertyKupna);
    }

    public int getDlugoscGlodowania() {
        return dlugoscGlodowania;
    }

    public void modyfikujZasoby() {
        if (this.getZasoby().getJedzenie() < 100) {
            this.dlugoscGlodowania++;
            this.getZasoby().modyfikujJedzenie(0);
        } else {
            this.getZasoby().modyfikujJedzenie(this.getZasoby().getJedzenie() - 100);
        }

        for (int i = 0; i < this.getZasoby().getSizeNarzedzia(); i++) {
            this.getZasoby().modyfikujNarzedzia(i, 0);
        }

        boolean czyNoszone = false;
        for (int i = 0; i < this.getZasoby().getSizeUbrania(); i++) {
            if (this.dlugoscNoszeniaUbran.get(i + 1) == (i + 1) * (i + 1)) {
                if (this.getZasoby().getUbrania(i) >= 100) {
                    this.getZasoby().modyfikujUbrania(i, this.getZasoby().getUbrania(i) - 100);

                    if (this.getZasoby().getUbrania(i) >= 100) {
                        this.dlugoscNoszeniaUbran.set(i + 1, 1);
                        czyNoszone = true;
                        break;
                    } else {
                        this.dlugoscNoszeniaUbran.set(i + 1, 0);
                    }
                } else {
                    this.getZasoby().modyfikujUbrania(i, 0);
                    this.dlugoscNoszeniaUbran.set(i + 1, 0);
                }
            }
        }

        if (!czyNoszone) {
            int sumaUbran = 0;
            for (int i = 0; i < this.getZasoby().getSizeUbrania(); i++) {
                sumaUbran += this.getZasoby().getUbrania(i);
            }

            if (sumaUbran < 100) {
                for (int i = 0; i < this.getZasoby().getSizeUbrania(); i++) {
                    if (this.getZasoby().getUbrania(i) > 0) {
                        this.dlugoscNoszeniaUbran.set(i + 1, this.dlugoscNoszeniaUbran.get(i + 1) + 1);
                    }
                }
            } else {
                for (int i = 0; this.getZasoby().getSizeUbrania() > 0; i++) {
                    if (this.getZasoby().getUbrania(i) >= 100) {
                        this.dlugoscNoszeniaUbran.set(i + 1, this.dlugoscNoszeniaUbran.get(i + 1) + 1);
                        czyNoszone = true;
                        break;
                    }
                }
                if (!czyNoszone) {
                    int odjeto = 100;
                    for (int i = 0; odjeto > 0; i++) {
                        this.dlugoscNoszeniaUbran.set(i + 1, this.dlugoscNoszeniaUbran.get(i + 1) + 1);
                        odjeto -= this.getZasoby().getUbrania(i);
                    }
                }
            }
        }
    }
}