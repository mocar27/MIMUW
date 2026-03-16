import java.util.ArrayList;
import java.util.List;

public class Goniec extends Bierka {
    public Goniec(int x, int y) {
        super(x, y);
    }

    @Override
    public List<Ruch> dajRuchCzarne(Szachownica sz) {

        List<Ruch> ruchy = new ArrayList<>();

        for (int i = 1; i < sz.getRozmiarSzachownicy(); i++) {
            if (this.getPole().getPozycjaX() + i < sz.getRozmiarSzachownicy() &&
                    this.getPole().getPozycjaY() + i < sz.getRozmiarSzachownicy()) {
                if (sz.czyWolne(this.getPole().getPozycjaX() + i, this.getPole().getPozycjaY() + i)) {
                    Ruch r = new Ruch();
                    r.staraPozycja = this.getPole();
                    r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + i, this.getPole().getPozycjaY() + i);
                    ruchy.add(r);
                } else if (sz.getPlansza()[this.getPole().getPozycjaX() + i][this.getPole().getPozycjaY() + i] != 5) {
                    Ruch r = new Ruch();
                    r.staraPozycja = this.getPole();
                    r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + i, this.getPole().getPozycjaY() + i);
                    ruchy.add(r);
                    break;
                } else {
                    break;
                }
            }
        }
        for (int i = 1; i < sz.getRozmiarSzachownicy(); i++) {
            if (this.getPole().getPozycjaX() + i < sz.getRozmiarSzachownicy() &&
                    this.getPole().getPozycjaY() - i >= 0) {
                if (sz.czyWolne(this.getPole().getPozycjaX() + i, this.getPole().getPozycjaY() - i)) {
                    Ruch r = new Ruch();
                    r.staraPozycja = this.getPole();
                    r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + i, this.getPole().getPozycjaY() - i);
                    ruchy.add(r);
                } else if (sz.getPlansza()[this.getPole().getPozycjaX() + i][this.getPole().getPozycjaY() - i] != 5) {
                    Ruch r = new Ruch();
                    r.staraPozycja = this.getPole();
                    r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + i, this.getPole().getPozycjaY() - i);
                    ruchy.add(r);
                    break;
                } else {
                    break;
                }
            }
        }
        for (int i = 1; i < sz.getRozmiarSzachownicy(); i++) {
            if (this.getPole().getPozycjaX() - i >= 0 &&
                    this.getPole().getPozycjaY() + i < sz.getRozmiarSzachownicy()) {
                if (sz.czyWolne(this.getPole().getPozycjaX() - i, this.getPole().getPozycjaY() + i)) {
                    Ruch r = new Ruch();
                    r.staraPozycja = this.getPole();
                    r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - i, this.getPole().getPozycjaY() + i);
                    ruchy.add(r);
                } else if (sz.getPlansza()[this.getPole().getPozycjaX() - i][this.getPole().getPozycjaY() + i] != 5) {
                    Ruch r = new Ruch();
                    r.staraPozycja = this.getPole();
                    r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - i, this.getPole().getPozycjaY() + i);
                    ruchy.add(r);
                    break;
                } else {
                    break;
                }
            }
        }
        for (int i = 1; i < sz.getRozmiarSzachownicy(); i++) {
            if (this.getPole().getPozycjaX() - i >= 0 && this.getPole().getPozycjaY() - i >= 0) {
                if (sz.czyWolne(this.getPole().getPozycjaX() - i, this.getPole().getPozycjaY() - i)) {
                    Ruch r = new Ruch();
                    r.staraPozycja = this.getPole();
                    r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - i, this.getPole().getPozycjaY() - i);
                    ruchy.add(r);
                } else if (sz.getPlansza()[this.getPole().getPozycjaX() - i][this.getPole().getPozycjaY() - i] != 5) {
                    Ruch r = new Ruch();
                    r.staraPozycja = this.getPole();
                    r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - i, this.getPole().getPozycjaY() - i);
                    ruchy.add(r);
                    break;
                } else {
                    break;
                }
            }
        }
        return ruchy;
    }

    @Override
    public String toString() {
        return "G";
    }
}
