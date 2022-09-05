public class Osoba {

    private final String imie;

    Osoba(String imie) {
        this.imie = imie;
    }

    @Override
    public String toString() {
        return "\n " + imie + " ";
    }

    public String getImie() {
        return imie;
    }
}
