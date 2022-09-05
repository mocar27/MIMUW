package Adapters;

import com.squareup.moshi.FromJson;
import com.squareup.moshi.JsonDataException;
import com.squareup.moshi.ToJson;

import Buying.*;

public class AdapterKupowanie {

    @ToJson
    Object kupowanieToJson(Kupowanie kupowanie) {

        Object json;

        if (kupowanie.getClass() == Zmechanizowany.class) {
            json = new AdaptKupowanie();
            ((AdaptKupowanie) json).typ = "zmechanizowany";
            ((AdaptKupowanie) json).liczba_narzedzi =
                    ((Zmechanizowany) kupowanie).liczba_narzedzi;
        } else if (kupowanie.getClass() == Gadzeciarz.class) {
            json = new AdaptKupowanie();
            ((AdaptKupowanie) json).typ = "gadzeciarz";
            ((AdaptKupowanie) json).liczba_narzedzi =
                    ((Gadzeciarz) kupowanie).liczba_narzedzi;
        } else if (kupowanie.getClass() == Technofob.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "technofob";
        } else {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "czyscioszek";
        }
        return json;
    }

    @FromJson
    Kupowanie kupowanieFromJson(AdaptKupowanie adaptKupowanieJson) {

        Zmechanizowany typ;
        if (adaptKupowanieJson.typ.equals("technofob")) {
            return new Technofob();
        } else if (adaptKupowanieJson.typ.equals("czyscioszek")) {
            return new Czyscioszek();
        } else if (adaptKupowanieJson.typ.equals("zmechanizowany")) {
            typ = new Zmechanizowany();
            typ.liczba_narzedzi =
                    adaptKupowanieJson.liczba_narzedzi;
            return typ;
        } else if (adaptKupowanieJson.typ.equals("gadzeciarz")) {
            typ = new Gadzeciarz();
            typ.liczba_narzedzi =
                    adaptKupowanieJson.liczba_narzedzi;
            return typ;
        } else {
            throw new JsonDataException("Nieznany typ kupowania: " + adaptKupowanieJson.typ);
        }
    }
}