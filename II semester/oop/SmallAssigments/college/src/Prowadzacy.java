public class Prowadzacy extends MajacyZajecia {

    private final int maxPrzedmiotow;

    Prowadzacy(String imie, int maxLiczbaPrzedmiotow) {
        super(imie);
        this.maxPrzedmiotow = maxLiczbaPrzedmiotow;
        przedmioty = new Przedmiot[maxLiczbaPrzedmiotow];
    }

    @Override
    public String toString() {
        return getImie();
    }

    public void dodajPrzedmiot(Przedmiot p) {
        przedmioty[iloscZajec] = p;
        iloscZajec++;
    }

    public boolean zapiszNaPrzedmiot(Student s, Przedmiot p) {
        for (int i = 0; i < iloscZajec; i++) {
            if (przedmioty[i] == p)
                return p.dodajStudenta(s);
        }
        return false;
    }

    public boolean zapiszNaPrzedmiot(Student s, String p) {
        for (int i = 0; i < iloscZajec; i++) {
            if (przedmioty[i].getNazwa() == p)
                return przedmioty[i].dodajStudenta(s);
        }
        return false;
    }

    public void rozliczPrzedmiot(Przedmiot p) {
        for (int i = 0; i < p.getIleZapisanych(); i++) {
            Student s = p.getStudenci()[i];
            s.przyznajECTS(p.getEcts());
        }
    }
}
