import java.util.List;

public abstract class Bierka {

    private Pole pole;

    public Bierka(int x, int y) {
        pole = new Pole(x, y);
    }

    public abstract List<Ruch> dajRuchCzarne(Szachownica sz);

    public List<Ruch> dajRuchBiale(Szachownica sz) {
        return dajRuchCzarne(sz);
    }

    public Pole getPole() {
        return pole;
    }

    public void zmienPozycje(Pole p) {
        this.pole = p;
    }
}
