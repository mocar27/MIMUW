public class Pole {

    protected int pozycjaX;
    protected int pozycjaY;

    public Pole(int x, int y) {
        this.pozycjaX = x;
        this.pozycjaY = y;
    }

    public int getPozycjaX() {
        return pozycjaX;
    }

    public int getPozycjaY() {
        return pozycjaY;
    }

    @Override
    public String toString() {
        return "(x,y) = (" + (pozycjaX + 1) + "," + (pozycjaY + 1) + ")";
    }
}
