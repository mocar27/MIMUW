package Adapters;

import com.squareup.moshi.FromJson;
import com.squareup.moshi.JsonDataException;
import com.squareup.moshi.ToJson;

import StockMarket.*;

public class AdapterGielda {

    @ToJson
    String toJson(Gielda gielda) {
        return gielda.toString();
    }

    @FromJson
    Gielda fromJson(String gielda) {
        if (gielda.equals("kapitalistyczna")) {
            return new Kapitalistyczna();
        } else if (gielda.equals("socjalistyczna")) {
            return new Socjalistyczna();
        } else if (gielda.equals("zrownowazona")) {
            return new Zrownowazona();
        } else {
            throw new JsonDataException("Nieznany typ gieldy: " + gielda);
        }
    }

}