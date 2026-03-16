package Production;

import Simulation.*;
import Career.*;

public class Krotkowzroczny extends Produkcja {

    @Override
    public String coProdukuje(Info info, Zasoby produktywnosc, int bonus, Kariera k) {

        double maks1 = Math.max(info.getCenyMax().get((info.getCenyMax().size() - 1)).getUbrania(0),
                info.getCenyMax().get((info.getCenyMax().size() - 1)).getNarzedzia(0));
        double maks2 = Math.max(info.getCenyMax().get((info.getCenyMax().size() - 1)).getJedzenie(),
                info.getCenyMax().get((info.getCenyMax().size() - 1)).getProgramy(0));

        double maks3 = Math.max(maks1, maks2);

        if (maks3 == info.getCenyMax().get((info.getCenyMax().size() - 1)).getUbrania(0)) {
            return "ubrania";
        } else if (maks3 == info.getCenyMax().get((info.getCenyMax().size() - 1)).getJedzenie()) {
            return "jedzenie";
        } else if (maks3 == info.getCenyMax().get((info.getCenyMax().size() - 1)).getProgramy(0)) {
            return "programy";
        } else {
            return "narzedzia";
        }
    }
}