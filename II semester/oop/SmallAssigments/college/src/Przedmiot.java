public class Przedmiot {

    private final String nazwa;
    private final Prowadzacy prowadzacy;
    private Student[] studenci;
    private int ileZapisanych;
    private final int maxPojemnosc;
    private final int ects;

    Przedmiot(String nazwa, Prowadzacy p, int ects, int pojemnosc) {
        this.nazwa = nazwa;
        this.prowadzacy = p;
        this.studenci = new Student[pojemnosc];
        this.maxPojemnosc = pojemnosc;
        this.ileZapisanych = 0;
        this.ects = ects;
    }

    @Override
    public String toString() {
        return "Przedmiot: " + nazwa + " prowadzony przez: " + prowadzacy + ", liczba punktow: " + ects;
    }

    public String getNazwa() {
        return nazwa;
    }

    public int getEcts() {
        return ects;
    }

    public Student[] getStudenci() {
        return studenci;
    }

    public int getIleZapisanych() {
        return ileZapisanych;
    }

    public boolean dodajStudenta(Student s) {
        if (ileZapisanych < maxPojemnosc) {
            studenci[ileZapisanych] = s;
            s.iloscZajec++;
            ileZapisanych++;
            return true;
        } else
            return false;
    }

}
