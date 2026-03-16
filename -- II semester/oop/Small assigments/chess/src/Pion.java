import java.util.ArrayList;
import java.util.List;

public class Pion extends Bierka {

    private int pierwszyRuch;

    public Pion(int x, int y) {
        super(x, y);
        this.pierwszyRuch = 1;
    }

    @Override
    public List<Ruch> dajRuchCzarne(Szachownica sz) {

        List<Ruch> ruchy = new ArrayList<>();

        if (pierwszyRuch != 0) {
            if (sz.czyWolne(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() + 2) &&
                    sz.czyWolne(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() + 1)) {
                Ruch r = new Ruch();
                r.staraPozycja = this.getPole();
                r.nowaPozycja = new Pole(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() + 2);
                ruchy.add(r);
                pierwszyRuch = 0;
            }
        }
        if (sz.czyWolne(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() + 1)) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() + 1);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaY() + 1 < sz.getRozmiarSzachownicy()
                && this.getPole().getPozycjaX() + 1 < sz.getRozmiarSzachownicy()) {
            if (!sz.czyWolne(this.getPole().getPozycjaX() + 1, this.getPole().getPozycjaY() + 1) &&
                    sz.getPlansza()[this.getPole().getPozycjaX() + 1][this.getPole().getPozycjaY() + 1] != 5) {
                Ruch r = new Ruch();
                r.staraPozycja = this.getPole();
                r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + 1, this.getPole().getPozycjaY() + 1);
                ruchy.add(r);
            }
        }
        if (this.getPole().getPozycjaY() + 1 < sz.getRozmiarSzachownicy() && this.getPole().getPozycjaX() - 1 >= 0) {
            if (!sz.czyWolne(this.getPole().getPozycjaX() - 1, this.getPole().getPozycjaY() + 1) &&
                    sz.getPlansza()[this.getPole().getPozycjaX() - 1][this.getPole().getPozycjaY() + 1] != 5) {
                Ruch r = new Ruch();
                r.staraPozycja = this.getPole();
                r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - 1, this.getPole().getPozycjaY() + 1);
                ruchy.add(r);
            }
        }
        return ruchy;
    }

    @Override
    public List<Ruch> dajRuchBiale(Szachownica sz) {

        List<Ruch> ruchy = new ArrayList<>();

        if (pierwszyRuch != 0) {
            if (sz.czyWolne(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() - 2) &&
                    sz.czyWolne(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() - 1)) {
                Ruch r = new Ruch();
                r.staraPozycja = this.getPole();
                r.nowaPozycja = new Pole(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() - 2);
                ruchy.add(r);
                pierwszyRuch = 0;
            }
        }
        if (sz.czyWolne(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() - 1)) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX(), this.getPole().getPozycjaY() - 1);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaY() - 1 >= 0 && this.getPole().getPozycjaX() + 1 < sz.getRozmiarSzachownicy()) {
            if (!sz.czyWolne(this.getPole().getPozycjaX() + 1, this.getPole().getPozycjaY() - 1) &&
                    sz.getPlansza()[this.getPole().getPozycjaX() + 1][this.getPole().getPozycjaY() - 1] != 5) {
                Ruch r = new Ruch();
                r.staraPozycja = this.getPole();
                r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + 1, this.getPole().getPozycjaY() - 1);
                ruchy.add(r);
            }
        }
        if (this.getPole().getPozycjaY() - 1 >= 0 && this.getPole().getPozycjaX() - 1 >= 0) {
            if (!sz.czyWolne(this.getPole().getPozycjaX() - 1, this.getPole().getPozycjaY() - 1) &&
                    sz.getPlansza()[this.getPole().getPozycjaX() - 1][this.getPole().getPozycjaY() - 1] != 5) {
                Ruch r = new Ruch();
                r.staraPozycja = this.getPole();
                r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - 1, this.getPole().getPozycjaY() - 1);
                ruchy.add(r);
            }
        }
        return ruchy;
    }

    @Override
    public String toString() {
        return "P";
    }
}