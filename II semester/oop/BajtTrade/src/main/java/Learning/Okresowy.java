package Learning;

import Simulation.*;

public class Okresowy extends Uczenie {

    protected int okresowosc_nauki;

    public boolean czyUczySie(Info info, double diamenty, int dzien) {
        return okresowosc_nauki % dzien == 0;
    }

    public int getOkresowosc_nauki() {
        return okresowosc_nauki;
    }

    public void setOkresowosc_nauki(int okresowosc_nauki) {
        this.okresowosc_nauki = okresowosc_nauki;
    }
}