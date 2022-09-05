package Simulation;

import com.squareup.moshi.Json;

import Players.*;

public class Rynek {

    private final Info info;
    private final Robotnik[] robotnicy;
    private final Spekulant[] spekulanci;
    @Json(ignore = true)
    private boolean isInitialize = false;

    public Rynek(Info i, Robotnik[] r, Spekulant[] s) {
        this.info = i;
        this.robotnicy = new Robotnik[r.length];
        this.spekulanci = new Spekulant[s.length];
        System.arraycopy(r, 0, robotnicy, 0, r.length);
        System.arraycopy(s, 0, spekulanci, 0, s.length);

    }

    public void symulujDzien() {

        if (!isInitialize) {
            info.infoInitialize();
            info.getGielda().gieldaInitialize();
            for (Robotnik r : robotnicy) {
                r.robotnikInitialize();
            }
            isInitialize = true;
        }

        info.getGielda().listaSprzedazyDodajDzien();
        for (Robotnik r : robotnicy) {
            if (r.getDlugoscGlodowania() < 3) {
                if (info.getDzien() % 7 == 0 && r.getZmiana().equals("rewolucjonista")) {
                    r.zmienKariere(info.getGielda());
                }
                if (r.getUczenie().czyUczySie(info, r.getZasoby().getDiamenty(), info.getDzien())) {
                    r.uczSie();
                } else {
                    r.pracuj(info, info.getGielda());
                }
            }
        }
        for (Spekulant s : spekulanci) {
            s.dodajOferty(info, info.getGielda());
        }
        info.getGielda().dopasujOferty(info, robotnicy, spekulanci);
        for (Robotnik r : robotnicy) {
            if (!r.getUczenie().czyUczySie(this.info, r.getZasoby().getDiamenty(), info.getDzien())) {
                r.modyfikujZasoby();
            }
        }
    }

    public Info getInfo() {
        return info;
    }
}