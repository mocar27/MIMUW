public class Gra {

    private final Szachownica szachownica;
    private final Gracz gracz1;
    private final Gracz gracz2;

    public Gra(Gracz g1, Gracz g2, Szachownica sz) {
        this.gracz1 = g1;
        this.gracz2 = g2;
        this.szachownica = sz;
    }

    //  Gracz1 to bierki biale - przy wypisywaniu
    //  sa na gorze szachownicy
    public void rozegrajPartie() {
        for (int i = 0; i < 50; i++) {
            System.out.println("RUCH NUMER: " + (i + 1));
            if (!gracz1.wykonajRuch(gracz2, szachownica)) {
                System.out.println("GRACZ 1 PRZEGRYWA");
                return;
            }
            szachownica.wypiszPlansze();
            if (!gracz2.wykonajRuch(gracz1, szachownica)) {
                System.out.println("GRACZ 2 PRZEGRYWA");
                return;
            }
            szachownica.wypiszPlansze();
            if (gracz1.bierki.size() == 1 && gracz2.bierki.size() == 1) {
                System.out.println("REMIS");
                return;
            }
        }
    }
}
