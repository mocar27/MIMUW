package Players;

import Simulation.*;
import Career.*;
import Learning.*;
import Buying.*;
import Production.*;

public abstract class Agent {

    protected final int id;
    protected Zasoby zasoby;
    protected Kariera kariera;
    protected Uczenie uczenie;
    protected Kupowanie kupowanie;
    protected Produkcja produkcja;

    public Agent(int i, Kariera k, Zasoby z) {
        this.id = i;
        this.kariera = k;
        this.zasoby = z;
    }

    public Agent(int id, Kariera kariera, Kupowanie kupowanie, Produkcja produkcja, Uczenie uczenie, Zasoby z) {
        this.id = id;
        this.kariera = kariera;
        this.kupowanie = kupowanie;
        this.produkcja = produkcja;
        this.uczenie = uczenie;
        this.zasoby = z;
    }

    public Zasoby getZasoby() {
        return zasoby;
    }

    public Kariera getKariera() {
        return kariera;
    }

    public Uczenie getUczenie() {
        return uczenie;
    }
}