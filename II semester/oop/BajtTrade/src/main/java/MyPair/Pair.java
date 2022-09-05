package MyPair;

public class Pair {

    public Object obiekt;
    public int poziom;

    public Pair(Object k, int p) {
        this.obiekt = k;
        this.poziom = p;
    }

    public int getPoziom() {
        return poziom;
    }

    public Object getObiekt() {
        return obiekt;
    }

    public void setPoziom(int poziom) {
        this.poziom = poziom;
    }
}