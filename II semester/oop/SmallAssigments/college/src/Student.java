public class Student extends MajacyZajecia {
    private final int nrInd;
    private int wymaganeECTS;
    private int zebraneECTS = 0;

    Student(String imie, int numer) {
        super(imie);
        this.nrInd = numer;
        this.wymaganeECTS = 30;
    }

    Student(String imie, int numer, int wymagane) {
        super(imie);
        this.nrInd = numer;
        this.wymaganeECTS = wymagane;
    }

    @Override
    public String toString() {
        return "- Student " + getImie() + ", numer indeksu: " + nrInd + ", zebrane punkty ECTS: " + zebraneECTS + "/" + wymaganeECTS + ".";
    }

    public int getNrInd() {
        return nrInd;
    }

    public int ileWymaganychPunktowECTS() {
        return wymaganeECTS;
    }

    public int ileZdobytychPunktowECTS() {
        return zebraneECTS;
    }

    public void przyznajECTS(int ilosc) {
        zebraneECTS += ilosc;
    }

    public boolean czyZaliczylEtap() {
        return zebraneECTS >= wymaganeECTS;
    }

}
