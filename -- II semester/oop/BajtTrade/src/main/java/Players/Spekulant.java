package Players;

import com.squareup.moshi.Json;

import java.util.ArrayList;
import java.util.List;

import Simulation.*;
import Career.*;
import StockMarket.*;

public class Spekulant extends Agent {

    @Json(ignore = true)
    private List<OfertaSpekulanta> ofertyKupna;
    @Json(ignore = true)
    private List<OfertaSpekulanta> ofertySprzedazy;

    public Spekulant(int id, Kariera k, Zasoby z) {
        super(id, k, z);
    }

    public void dodajOferty(Info info, Gielda g) {

        this.ofertyKupna = new ArrayList<>();
        this.ofertySprzedazy = new ArrayList<>();

        this.kariera.dodajOfertyKupna(info, ofertyKupna, this.zasoby, g);
        this.kariera.dodajOfertySprzedazy(info, ofertySprzedazy, this.zasoby, g);
    }

    public List<OfertaSpekulanta> getOfertyKupna() {
        return ofertyKupna;
    }

    public List<OfertaSpekulanta> getOfertySprzedazy() {
        return ofertySprzedazy;
    }
}