import java.util.ArrayList;
import java.util.List;
import java.util.Random;

public class Drapiezny extends Gracz {

    public Drapiezny(boolean kolor, Bierka[] b) {
        super(kolor, b);
    }

    @Override
    public boolean wykonajRuch(Gracz drugiGracz, Szachownica sz) {

        List<Ruch> ruchy = new ArrayList<>();
        if (!this.getKolor()) {
            for (Bierka b : bierki) {
                List<Ruch> temp;
                temp = b.dajRuchCzarne(sz);
                if (temp != null) {
                    ruchy.addAll(temp);
                }
            }
        } else {
            for (Bierka b : bierki) {
                List<Ruch> temp;
                temp = b.dajRuchBiale(sz);
                if (temp != null) {
                    ruchy.addAll(temp);
                }
            }
        }

        ruchy.removeIf(r -> this.czyMaBierke(r.nowaPozycja));

        if (ruchy.size() == 0) {
            return false;
        }

        List<Ruch> mozliwosciBicia = new ArrayList<>();
        for (Ruch r : ruchy) {
            for (Bierka b : drugiGracz.bierki) {
                if (r.nowaPozycja.getPozycjaX() == b.getPole().getPozycjaX() &&
                        r.nowaPozycja.getPozycjaY() == b.getPole().getPozycjaY()) {
                    mozliwosciBicia.add(r);
                }
            }
        }

        Pole starePole;
        Pole nowePole;

        if (mozliwosciBicia.size() > 0) {
            Random rand = new Random();
            int limitGorny = mozliwosciBicia.size();
            int wylosowanyRuch = rand.nextInt(limitGorny);

            starePole = mozliwosciBicia.get(wylosowanyRuch).getStaraPozycja();
            nowePole = mozliwosciBicia.get(wylosowanyRuch).getNowaPozycja();
        } else {
            Random rand = new Random();
            int limitGorny = ruchy.size();
            int wylosowanyRuch = rand.nextInt(limitGorny);

            starePole = ruchy.get(wylosowanyRuch).getStaraPozycja();
            nowePole = ruchy.get(wylosowanyRuch).getNowaPozycja();
        }


        if (!sz.czyWolne(nowePole.getPozycjaX(), nowePole.getPozycjaY())) {
            Bierka doZbicia = null;
            switch (sz.getPlansza()[nowePole.getPozycjaX()][nowePole.getPozycjaY()]) {
                case 1 -> doZbicia = new Pion(nowePole.getPozycjaX(), nowePole.getPozycjaY());
                case 2 -> doZbicia = new Wieza(nowePole.getPozycjaX(), nowePole.getPozycjaY());
                case 3 -> doZbicia = new Skoczek(nowePole.getPozycjaX(), nowePole.getPozycjaY());
                case 4 -> doZbicia = new Goniec(nowePole.getPozycjaX(), nowePole.getPozycjaY());
                case 6 -> doZbicia = new Hetman(nowePole.getPozycjaX(), nowePole.getPozycjaY());
            }
            if(this.getKolor()) {
                System.out.println("NASTAPILO BICIE CZARNEGO " + doZbicia);
            } else {
                System.out.println("NASTAPILO BICIE BIALEGO " + doZbicia);
            }
            drugiGracz.usunBierke(doZbicia);
        }
        for (Bierka b : this.bierki) {
            if (b.getPole().getPozycjaX() == starePole.getPozycjaX() &&
                    b.getPole().getPozycjaY() == starePole.getPozycjaY()) {
                b.zmienPozycje(nowePole);
            }
            if(this.getKolor()) {
                System.out.println("RUCH BIALEGO " + b + " z pola: " + starePole + " na pole: " + nowePole);
            } else {
                System.out.println("RUCH CZARNEGO " + b + " z pola: " + starePole + " na pole: " + nowePole);
            }
        }
        sz.modyfikujPole(starePole, nowePole);
        return true;
    }

    @Override
    public String toString() {
        return "DRAPIEZNY";
    }
}
