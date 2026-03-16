package Production;

import Simulation.*;
import Career.*;

public class Perspektywiczny extends Produkcja {

    public int historia_perspektywy;

    @Override
    public String coProdukuje(Info info, Zasoby produktywnosc, int bonus, Kariera k) {

        double programy = info.getCenySrednie().get(info.getCenySrednie().size() - 1).getProgramy(0)
                - info.getCenySrednie().get(historia_perspektywy).getProgramy(0);
        double jedzenie = info.getCenySrednie().get(info.getCenySrednie().size() - 1).getJedzenie()
                - info.getCenySrednie().get(historia_perspektywy).getJedzenie();
        double ubrania = info.getCenySrednie().get(info.getCenySrednie().size() - 1).getUbrania(0)
                - info.getCenySrednie().get(historia_perspektywy).getUbrania(0);
        double narzedzia = info.getCenySrednie().get(info.getCenySrednie().size() - 1).getNarzedzia(0)
                - info.getCenySrednie().get(historia_perspektywy).getNarzedzia(0);

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