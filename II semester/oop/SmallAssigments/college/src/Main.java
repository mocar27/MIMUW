import java.util.Random;

public class Main {

    public static void main(String[] args) {

        // Random przyda nam sie do generowania numerow indeksow i punktow ECTS
        Random r = new Random();
        int liczbaStudentow = 5;
        int liczbaPrzedmiotow = 4;

        // tu bedziemy przetrzymywac obiekty do testowania
        Student[] studenciUczelni = new Student[liczbaStudentow];
        String[] imiona = {"A.B.","C.D.","E.F.","G.H.","I.J."};
        Przedmiot[] przedmiotyUczelni = new Przedmiot[liczbaPrzedmiotow];

        // tworzymy tablice studentow
        for(int i = 0; i < 5; i++) {
            studenciUczelni[i] = new Student(imiona[i], r.nextInt(99999), 10 + r.nextInt(30));
        }

        // wprowadzamy dwoch prowadzacych
        Prowadzacy prowadzacy = new Prowadzacy("X. Y.", 5);
        Prowadzacy prowadzacy2 = new Prowadzacy("P. Q.", 5);

        // tworzymy cztery przedmioty trzy z nich prowadzone przez prowadzacy, ostatni przez prowadzacy2
        String[] nazwyPrzedmiotow = {"PO", "ASD", "AM", "BD"};
        for(int i = 0; i < 3; i++) {
            przedmiotyUczelni[i] =  new Przedmiot(nazwyPrzedmiotow[i], prowadzacy, r.nextInt(5) + 3, 10);
            prowadzacy.dodajPrzedmiot(przedmiotyUczelni[i]);
        }
        for(int i = 3; i < liczbaPrzedmiotow; i++) {
            przedmiotyUczelni[i] =  new Przedmiot(nazwyPrzedmiotow[i], prowadzacy2, r.nextInt(5) + 3, 10);
            prowadzacy2.dodajPrzedmiot(przedmiotyUczelni[i]);
        }

        // zapisujemy studentow na przedmioty u prowadzacych
        for(int i = 0; i < 3; i++) {
            Przedmiot p = przedmiotyUczelni[i];
            for(Student s : studenciUczelni) {
                if(!prowadzacy.zapiszNaPrzedmiot(s, p)) {
                    System.out.println("Student " + s + " nie został zapisany na " + p.getNazwa());
                }
            }
        }

        for(int i = 0; i < 3; i++) {
            prowadzacy2.zapiszNaPrzedmiot(studenciUczelni[i], przedmiotyUczelni[3]);
        }

        // zapisz na przedmiot jest przeladowane, tj. moze byc zarowno wywolane jako:
        // (Student s, Przedmiot p) oraz (Student s, String nazwaPrzedmiotu)
        // Tego studenta nie uda sie zarejestrowac bo prowadzacy nie ma na liscie przedmiotow "AM"
        // zapiszNaPrzedmiot ma odpowiedziec przez false
        boolean bRes = prowadzacy.zapiszNaPrzedmiot(studenciUczelni[4], "BD");
        String res = "\n[INFO] " + (bRes ? "Udało " : "Nie udało ")  +
                "zarejestrować się studenta " + studenciUczelni[4];
        System.out.println(res);

        // Podsumowanie tworzenia obiektow
        System.out.println("\n>>>     STAN SYSTEMU     <<<");
        System.out.println("----------------------------");
        System.out.println("Studenci w systemie: ");
        for(Student s : studenciUczelni) {
            System.out.println(s);
        }
        System.out.println("----------------------------\n");

        System.out.println("----------------------------");
        System.out.println("Przedmioty w systemie: ");
        for(Przedmiot p : przedmiotyUczelni) {
            System.out.println(p);
            System.out.println("   Lista studentów:");
            for(int i = 0; i< p.getIleZapisanych(); i++)
                System.out.println("    " + p.getStudenci()[i]);
        }
        System.out.println("----------------------------");

        // rozliczamy przedmioty, kazdy student otrzymuje punkty ECTS ktore przysluguja za przedmiot
        prowadzacy.rozliczPrzedmiot(przedmiotyUczelni[0]);
        prowadzacy.rozliczPrzedmiot(przedmiotyUczelni[1]);
        prowadzacy.rozliczPrzedmiot(przedmiotyUczelni[2]);
        prowadzacy2.rozliczPrzedmiot(przedmiotyUczelni[3]);

        // Ktorzy studenci zaliczyli etap?
        System.out.println(">>>  PODSUMOWANIE ETAPU  <<<");
        System.out.println("----------------------------");
        System.out.println("Studenci w systemie:");
        for(Student s : studenciUczelni) {
            if(s.czyZaliczylEtap()) {
                System.out.println("Student "+ s.getImie() + " ZALICZYL etap uzyskujac " +
                        s.ileZdobytychPunktowECTS() + "/"+
                        s.ileWymaganychPunktowECTS() + "pkt. ECTS.");
            } else {
                System.out.println("Student "+ s.getImie() + " NIE ZALICZYL etapu uzyskujac " +
                        s.ileZdobytychPunktowECTS() + "/"+
                        s.ileWymaganychPunktowECTS() + "pkt. ECTS.");
            }
        }

    }

}
