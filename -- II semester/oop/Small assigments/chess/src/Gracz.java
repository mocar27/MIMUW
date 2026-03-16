import java.util.ArrayList;
import java.util.List;

public abstract class Gracz {

    protected List<Bierka> bierki;
    private final boolean czyBialy;

    public Gracz(boolean kolor, Bierka[] b) {
        this.czyBialy = kolor;
        this.bierki = new ArrayList<>();
        this.bierki.addAll(List.of(b));
    }

    public abstract boolean wykonajRuch(Gracz drugiGracz, Szachownica sz);

    public boolean getKolor() {
        return czyBialy;
    }

    public void usunBierke(Bierka b) {
        for (Bierka bierka : bierki) {
            if (bierka.getPole().getPozycjaX() == b.getPole().getPozycjaX()
                    && bierka.getPole().getPozycjaY() == b.getPole().getPozycjaY()) {
                bierki.remove(b);
            }
        }
    }

    public boolean czyMaBierke(Pole p) {
        for (Bierka b : bierki) {
            if (b.getPole().getPozycjaX() == p.getPozycjaX() &&
                    b.getPole().getPozycjaY() == p.getPozycjaY()) {
                return true;
            }
        }
        return false;
    }
}