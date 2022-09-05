package Adapters;

import com.squareup.moshi.FromJson;
import com.squareup.moshi.JsonDataException;
import com.squareup.moshi.ToJson;

import Production.*;

public class AdapterProdukcja {

    @ToJson
    Object produkcjaToJson(Produkcja produkcja) {

        Object json;

        if (produkcja.getClass() == Sredniak.class) {
            json = new AdaptSredniak();
            ((AdaptSredniak) json).typ = "sredniak";
            ((AdaptSredniak) json).historia_sredniej_produkcji =
                    ((Sredniak) produkcja).historia_sredniej_produkcji;
        } else if (produkcja.getClass() == Perspektywiczny.class) {
            json = new AdaptPerspektywiczny();
            ((AdaptPerspektywiczny) json).typ = "perspektywiczny";
            ((AdaptPerspektywiczny) json).historia_perspektywy =
                    ((Perspektywiczny) produkcja).historia_perspektywy;
        } else if (produkcja.getClass() == Krotkowzroczny.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "krotkowzroczny";
        } else if (produkcja.getClass() == Chciwy.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "chciwy";
        } else {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "losowy";
        }
        return json;
    }

    @FromJson
    Produkcja produkcjaFromJson(AdaptProdukcja adaptProdukcjaJson) {

        Produkcja typ;
        if (adaptProdukcjaJson.typ.equals("krotkowzroczny")) {
            return new Krotkowzroczny();
        } else if (adaptProdukcjaJson.typ.equals("chciwy")) {
            return new Chciwy();
        } else if (adaptProdukcjaJson.typ.equals("losowy")) {
            return new Losowy();
        } else if (adaptProdukcjaJson.typ.equals("sredniak")) {
            typ = new Sredniak();
            ((Sredniak) typ).historia_sredniej_produkcji =
                    adaptProdukcjaJson.historia_sredniej_produkcji;
            return typ;
        } else if (adaptProdukcjaJson.typ.equals("perspektywiczny")) {
            typ = new Perspektywiczny();
            ((Perspektywiczny) typ).historia_perspektywy =
                    adaptProdukcjaJson.historia_perspektywy;
            return typ;
        } else {
            throw new JsonDataException("Nieznany typ producji: " + adaptProdukcjaJson.typ);
        }
    }
}