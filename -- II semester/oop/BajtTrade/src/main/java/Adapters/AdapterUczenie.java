package Adapters;

import com.squareup.moshi.FromJson;
import com.squareup.moshi.JsonDataException;
import com.squareup.moshi.ToJson;

import Learning.*;

public class AdapterUczenie {

    @ToJson
    Object uczenieToJson(Uczenie uczenie) {

        Object json;

        if (uczenie.getClass() == Student.class) {
            json = new AdaptStudent();
            ((AdaptStudent) json).typ = "student";
            ((AdaptStudent) json).zapas = ((Student) uczenie).getZapas();
            ((AdaptStudent) json).okres = ((Student) uczenie).getOkres();
        } else if (uczenie.getClass() == Okresowy.class) {
            json = new AdaptOkresowy();
            ((AdaptOkresowy) json).typ = "okresowy";
            ((AdaptOkresowy) json).okresowosc_nauki = ((Okresowy) uczenie).getOkresowosc_nauki();
        } else if (uczenie.getClass() == Oszczedny.class) {
            json = new AdaptOszczedny();
            ((AdaptOszczedny) json).typ = "oszczedny";
            ((AdaptOszczedny) json).limit_diamentow = ((Oszczedny) uczenie).getLimit_diamentow();
        } else if (uczenie.getClass() == Pracus.class) {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "pracus";
        } else {
            json = new AdaptNoAttributes();
            ((AdaptNoAttributes) json).typ = "rozkladowy";
        }
        return json;
    }

    @FromJson
    Uczenie uczenieFromJson(AdaptUczenie adaptUczenieJson) {

        Uczenie typ;
        if (adaptUczenieJson.typ.equals("pracus")) {
            return new Pracus();
        } else if (adaptUczenieJson.typ.equals("rozkladowy")) {
            return new Rozkladowy();
        } else if (adaptUczenieJson.typ.equals("student")) {
            typ = new Student();
            ((Student) typ).setZapas(adaptUczenieJson.zapas);
            ((Student) typ).setOkres(adaptUczenieJson.okres);
            return typ;
        } else if (adaptUczenieJson.typ.equals("okresowy")) {
            typ = new Okresowy();
            ((Okresowy) typ).setOkresowosc_nauki(adaptUczenieJson.okresowosc_nauki);
            return typ;
        } else if (adaptUczenieJson.typ.equals("oszczedny")) {
            typ = new Oszczedny();
            ((Oszczedny) typ).setLimit_diamentow(adaptUczenieJson.limit_diamentow);
            return typ;
        } else {
            throw new JsonDataException("Nieznany typ uczenia: " + adaptUczenieJson.typ);
        }
    }
}