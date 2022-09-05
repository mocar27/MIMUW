package Production;

import Simulation.*;
import Career.*;

public class Sredniak extends Produkcja {

    public int historia_sredniej_produkcji;

    @Override
    public String coProdukuje(Info info, Zasoby produktywnosc, int bonus, Kariera k) {

        double programy = 0;
        double jedzenie = 0;
        double ubrania = 0;
        double narzedzia = 0;

        int iterator = info.getCenySrednie().size() - 1;
        for (int i = 0; i < historia_sredniej_produkcji; i++) {
            programy += info.getCenySrednie().get(iterator).getProgramy(0);
            jedzenie += info.getCenySrednie().get(iterator).getJedzenie();
            ubrania += info.getCenySrednie().get(iterator).getUbrania(0);
            narzedzia += info.getCenySrednie().get(iterator).getNarzedzia(0);
            iterator--;
        }
        programy /= historia_sredniej_produkcji;
        jedzenie /= historia_sredniej_produkcji;
        ubrania /= historia_sredniej_produkcji;
        narzedzia /= historia_sredniej_produkcji;

        double maks1 = Math.max(programy, jedzenie);
        double maks2 = Math.max(ubrania, narzedzia);
        double maks3 = Math.max(maks1, maks2);

        if (maks3 == programy) {
            return "programy";
        } else if (maks3 == narzedzia) {
            return "narzedzia";
        } else if (maks3 == ubrania) {
            return "ubrania";
        } else {
            return "jedzenie";
        }
    }
}