package Adapters;

import com.squareup.moshi.FromJson;
import com.squareup.moshi.JsonDataException;
import com.squareup.moshi.ToJson;

import Career.*;

public class AdapterKariera {

    @ToJson
    Object karieraToJson(Kariera kariera) {

        Object json;

        if (kariera.getClass() == Sredni.class) {
            json = new AdaptSredni();
            ((AdaptSredni) json).typ = "sredni";
            ((AdaptSredni) json).historia_spekulanta_sredniego =
                    ((Sredni) kariera).getHistoria_spekulanta_sredniego();
        } else if (kariera.getClass() == Rolnik.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "rolnik";
        } else if (kariera.getClass() == Rzemieslnik.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "rzemieslnik";
        } else if (kariera.getClass() == Inzynier.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "inzynier";
        } else if (kariera.getClass() == Programista.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "programista";
        } else if (kariera.getClass() == Gornik.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "gornik";
        } else if (kariera.getClass() == RegulujacyRynek.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "regulujacy_rynek";
        } else {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "wypukly";
        }
        return json;
    }

    @FromJson
    Kariera karieraFromJson(AdaptSredni adaptKarieraJson) {

        Sredni typ;
        if (adaptKarieraJson.typ.equals("rolnik")) {
            return new Rolnik();
        } else if (adaptKarieraJson.typ.equals("rzemielsnik")) {
            return new Rzemieslnik();
        } else if (adaptKarieraJson.typ.equals("inzynier")) {
            return new Inzynier();
        } else if (adaptKarieraJson.typ.equals("gornik")) {
            return new Gornik();
        } else if (adaptKarieraJson.typ.equals("programista")) {
            return new Programista();
        } else if (adaptKarieraJson.typ.equals("regulujacy_rynek")) {
            return new RegulujacyRynek();
        } else if (adaptKarieraJson.typ.equals("wypukly")) {
            return new Wypukly();
        } else if (adaptKarieraJson.typ.equals("sredni")) {
            typ = new Sredni();
            typ.setHistoria_spekulanta_sredniego(adaptKarieraJson.historia_spekulanta_sredniego);
            return typ;
        } else {
            throw new JsonDataException("Nieznany typ kariery: " + adaptKarieraJson.typ);
        }
    }

}