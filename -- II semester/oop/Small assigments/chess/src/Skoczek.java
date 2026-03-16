import java.util.ArrayList;
import java.util.List;

public class Skoczek extends Bierka {
    public Skoczek(int x, int y) {
        super(x, y);
    }

    @Override
    public List<Ruch> dajRuchCzarne(Szachownica sz) {

        List<Ruch> ruchy = new ArrayList<>();

        if (this.getPole().getPozycjaX() + 1 < sz.getRozmiarSzachownicy() &&
                this.getPole().getPozycjaY() + 2 < sz.getRozmiarSzachownicy() &&
                sz.getPlansza()[this.getPole().getPozycjaX() + 1][this.getPole().getPozycjaY() + 2] != 5) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + 1, this.getPole().getPozycjaY() + 2);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaX() + 1 < sz.getRozmiarSzachownicy() &&
                this.getPole().getPozycjaY() - 2 >= 0 &&
                sz.getPlansza()[this.getPole().getPozycjaX() + 1][this.getPole().getPozycjaY() - 2] != 5) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + 1, this.getPole().getPozycjaY() - 2);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaX() - 1 >= 0 &&
                this.getPole().getPozycjaY() + 2 < sz.getRozmiarSzachownicy() &&
                sz.getPlansza()[this.getPole().getPozycjaX() - 1][this.getPole().getPozycjaY() + 2] != 5) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - 1, this.getPole().getPozycjaY() + 2);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaX() - 1 >= 0 &&
                this.getPole().getPozycjaY() - 2 >= 0 &&
                sz.getPlansza()[this.getPole().getPozycjaX() - 1][this.getPole().getPozycjaY() - 2] != 5) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - 1, this.getPole().getPozycjaY() - 2);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaX() + 2 < sz.getRozmiarSzachownicy() &&
                this.getPole().getPozycjaY() + 1 < sz.getRozmiarSzachownicy() &&
                sz.getPlansza()[this.getPole().getPozycjaX() + 2][this.getPole().getPozycjaY() + 1] != 5) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + 2, this.getPole().getPozycjaY() + 1);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaX() + 2 < sz.getRozmiarSzachownicy() &&
                this.getPole().getPozycjaY() - 1 >= 0 &&
                sz.getPlansza()[this.getPole().getPozycjaX() + 2][this.getPole().getPozycjaY() - 1] != 5) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX() + 2, this.getPole().getPozycjaY() - 1);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaX() - 2 >= 0 &&
                this.getPole().getPozycjaY() + 1 < sz.getRozmiarSzachownicy() &&
                sz.getPlansza()[this.getPole().getPozycjaX() -2][this.getPole().getPozycjaY() + 1] != 5) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - 2, this.getPole().getPozycjaY() + 1);
            ruchy.add(r);
        }
        if (this.getPole().getPozycjaX() - 2 >= 0 &&
                this.getPole().getPozycjaY() - 1 >= 0 &&
                sz.getPlansza()[this.getPole().getPozycjaX() - 2][this.getPole().getPozycjaY() - 1] != 5) {
            Ruch r = new Ruch();
            r.staraPozycja = this.getPole();
            r.nowaPozycja = new Pole(this.getPole().getPozycjaX() - 2, this.getPole().getPozycjaY() - 1);
            ruchy.add(r);
        }

        return ruchy;
    }

    @Override
    public String toString() {
        return "S";
    }
}
