public class Main {

    public static void main(String[] args) {

        Bierka[] bierki1 = new Bierka[16];
        bierki1[0] = new Wieza(0, 0);
        bierki1[1] = new Skoczek(1, 0);
        bierki1[2] = new Goniec(2, 0);
        bierki1[3] = new Krol(3, 0);
        bierki1[4] = new Hetman(4, 0);
        bierki1[5] = new Goniec(5, 0);
        bierki1[6] = new Skoczek(6, 0);
        bierki1[7] = new Wieza(7, 0);
        bierki1[8] = new Pion(0, 1);
        bierki1[9] = new Pion(1, 1);
        bierki1[10] = new Pion(2, 1);
        bierki1[11] = new Pion(3, 1);
        bierki1[12] = new Pion(4, 1);
        bierki1[13] = new Pion(5, 1);
        bierki1[14] = new Pion(6, 1);
        bierki1[15] = new Pion(7, 1);

        Bierka[] bierki2 = new Bierka[16];
        bierki2[0] = new Wieza(0, 7);
        bierki2[1] = new Skoczek(1, 7);
        bierki2[2] = new Goniec(2, 7);
        bierki2[3] = new Krol(3, 7);
        bierki2[4] = new Hetman(4, 7);
        bierki2[5] = new Goniec(5, 7);
        bierki2[6] = new Skoczek(6, 7);
        bierki2[7] = new Wieza(7, 7);
        bierki2[8] = new Pion(0, 6);
        bierki2[9] = new Pion(1, 6);
        bierki2[10] = new Pion(2, 6);
        bierki2[11] = new Pion(3, 6);
        bierki2[12] = new Pion(4, 6);
        bierki2[13] = new Pion(5, 6);
        bierki2[14] = new Pion(6, 6);
        bierki2[15] = new Pion(7, 6);

        Gracz gracz1 = new Losowy(true, bierki1);
        Gracz gracz2 = new Losowy(false, bierki2);
        Szachownica szachownica = new Szachownica(8);
        Gra gra = new Gra(gracz1, gracz2, szachownica);
        System.out.println("GRACZ 1: " + gracz1 + ", BIALE BIERKI" + " vs. " + "GRACZ 2: "+ gracz2 + ", CZARNE BIERKI");
        System.out.println("STAN POCZATKOWY SZACHOWNICY:");
        szachownica.wypiszPlansze();
        gra.rozegrajPartie();
    }
}
