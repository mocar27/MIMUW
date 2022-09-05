package Production;

import Simulation.*;
import Career.*;

public class Chciwy extends Produkcja {

    @Override
    public String coProdukuje(Info info, Zasoby produktywnosc, int bonus, Kariera k) {

        double programy = 0;
        double jedzenie = 0;
        double ubrania = 0;
        double narzedzia = 0;

        if (k.getClass() == Rolnik.class) {
            programy = produktywnosc.getProgramy(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getProgramy(0);
            jedzenie = (produktywnosc.getJedzenie() + produktywnosc.getJedzenie() * bonus) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getJedzenie();
            ubrania = produktywnosc.getUbrania(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getUbrania(0);
            narzedzia = produktywnosc.getNarzedzia(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getNarzedzia(0);

        } else if (k.getClass() == Rzemieslnik.class) {
            programy = produktywnosc.getProgramy(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getProgramy(0);
            jedzenie = produktywnosc.getJedzenie() *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getJedzenie();
            ubrania = (produktywnosc.getUbrania(0) + produktywnosc.getUbrania(0) * bonus) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getUbrania(0);
            narzedzia = produktywnosc.getNarzedzia(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getNarzedzia(0);

        } else if (k.getClass() == Inzynier.class) {
            programy = produktywnosc.getProgramy(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getProgramy(0);
            jedzenie = produktywnosc.getJedzenie() *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getJedzenie();
            ubrania = produktywnosc.getUbrania(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getUbrania(0);
            narzedzia = (produktywnosc.getNarzedzia(0) + produktywnosc.getNarzedzia(0) * bonus) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getNarzedzia(0);

        } else if (k.getClass() == Programista.class) {
            programy = (produktywnosc.getProgramy(0) + produktywnosc.getProgramy(0) * bonus) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getProgramy(0);
            jedzenie = produktywnosc.getJedzenie() *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getJedzenie();
            ubrania = produktywnosc.getUbrania(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getUbrania(0);
            narzedzia = produktywnosc.getNarzedzia(0) *
                    info.getCenySrednie().get(info.getCenySrednie().size() - 1).getNarzedzia(0);

        }

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