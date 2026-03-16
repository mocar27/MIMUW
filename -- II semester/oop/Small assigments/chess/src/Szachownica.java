public class Szachownica {

    private final int rozmiar;
    private int[][] plansza;

    public Szachownica(int r) {
        this.rozmiar = r;
        plansza = new int[r][r];
        for (int j = 0; j < r; j++) {
            plansza[j][1] = 1;
        }
        for (int j = 0; j < r; j++) {
            plansza[j][6] = 1;
        }
        plansza[0][0] = 2;
        plansza[0][7] = 2;
        plansza[7][0] = 2;
        plansza[7][7] = 2;
        plansza[1][0] = 3;
        plansza[6][0] = 3;
        plansza[1][7] = 3;
        plansza[6][7] = 3;
        plansza[2][7] = 4;
        plansza[5][7] = 4;
        plansza[5][0] = 4;
        plansza[2][0] = 4;
        plansza[3][0] = 5;
        plansza[4][0] = 6;
        plansza[3][7] = 5;
        plansza[4][7] = 6;
    }

    public int getRozmiarSzachownicy() {
        return rozmiar;
    }

    public int[][] getPlansza() {
        return plansza;
    }

    public boolean czyWolne(int x, int y) {
        if (x >= 0 && x < rozmiar && y >= 0 && y < rozmiar) {
            return plansza[x][y] == 0;
        } else {
            return false;
        }
    }

    // modyfikujPole() - to tez funkcja zbijBierke();
    public void modyfikujPole(Pole starePole, Pole nowePole) {
        int pionek = plansza[starePole.getPozycjaX()][starePole.getPozycjaY()];
        plansza[starePole.getPozycjaX()][starePole.getPozycjaY()] = 0;
        plansza[nowePole.getPozycjaX()][nowePole.getPozycjaY()] = pionek;
    }

    public void wypiszPlansze() {
        for (int i = 0; i < rozmiar; i++) {
            System.out.println(" ");
            System.out.println("---------------------------------");

            for (int j = 0; j < rozmiar; j++) {
                switch (plansza[j][i]) {
                    case 1 -> System.out.print("| " + "P" + " ");
                    case 2 -> System.out.print("| " + "W" + " ");
                    case 3 -> System.out.print("| " + "S" + " ");
                    case 4 -> System.out.print("| " + "G" + " ");
                    case 5 -> System.out.print("| " + "K" + " ");
                    case 6 -> System.out.print("| " + "H" + " ");
                    default -> System.out.print("| " + " " + " ");
                }
            }
            System.out.print("|");
        }
        System.out.println(" ");
        System.out.println("---------------------------------");
    }
}