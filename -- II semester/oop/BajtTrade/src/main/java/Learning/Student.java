package Learning;

import Simulation.*;

public class Student extends Uczenie {

    protected int zapas;
    protected int okres;

    public boolean czyUczySie(Info info, double diamenty, int dzien) {
        return diamenty > 100 * zapas * info.getSredniaJedzenie(dzien);
    }

    public void setZapas(int x) {
        this.zapas = x;
    }

    public void setOkres(int x) {
        this.okres = x;
    }

    public int getOkres() {
        return okres;
    }

    public int getZapas() {
        return zapas;
    }
}