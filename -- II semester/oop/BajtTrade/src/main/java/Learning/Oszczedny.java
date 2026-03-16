package Learning;

import Simulation.*;

public class Oszczedny extends Uczenie {

    protected int limit_diamentow;

    public boolean czyUczySie(Info info, double diamenty, int dzien) {
        return diamenty > limit_diamentow;
    }

    public int getLimit_diamentow() {
        return limit_diamentow;
    }

    public void setLimit_diamentow(int limit_diamentow) {
        this.limit_diamentow = limit_diamentow;
    }
}