import com.squareup.moshi.JsonAdapter;
import com.squareup.moshi.Moshi;

import java.io.IOException;

import Adapters.*;
import Simulation.*;

public class Main {
    public static void main(String[] args) throws IOException {
        String JString =
                "{"
                        + "    \"info\": "
                        + "        {"
                        + "                \"dlugosc\": \"5\","
                        + "                \"gielda\": \"socjalistyczna\","
                        + "                \"kara_za_brak_ubran\" : \"2\","
                        + "                \"ceny\": "
                        + "                 {"
                        + "                   \"programy\": ["
                        + "                    \"10\" ],"
                        + "                   \"jedzenie\": \"125.27\","
                        + "                   \"ubrania\": ["
                        + "                    \"10\" ],"
                        + "                   \"narzedzia\": ["
                        + "                    \"105\" ]"
                        + "                   }"
                        + "        },"
                        + "                   \"robotnicy\": ["
                        + "        {"
                        + "                \"id\": \"1\","
                        + "                \"poziom\": \"1\","
                        + "                \"kariera\":"
                        + "                    {"
                        + "                    \"typ\": \"programista\""
                        + "                    },"
                        + "                \"kupowanie\" : "
                        + "                    {"
                        + "                    \"typ\": \"gadzeciarz\","
                        + "                    \"liczba_narzedzi\": \"100\""
                        + "                    },"
                        + "                \"produkcja\" : "
                        + "                    {"
                        + "                    \"typ\": \"chciwy\""
                        + "                    },"
                        + "                   \"uczenie\": "
                        + "                    {"
                        + "                     \"typ\": \"student\","
                        + "                     \"zapas\": \"2\","
                        + "                     \"okres\": \"5\""
                        + "                     },"
                        + "                   \"zmiana\": \"rewolucjonista\","
                        + "                   \"produktywnosc\":"
                        + "                   {"
                        + "                   \"programy\": ["
                        + "                    \"100\" ],"
                        + "                   \"jedzenie\": \"100\","
                        + "                   \"diamenty\": \"100\","
                        + "                   \"ubrania\": ["
                        + "                    \"100\" ],"
                        + "                   \"narzedzia\": ["
                        + "                    \"100\" ]"
                        + "                   },"
                        + "                   \"zasoby\":"
                        + "                   {"
                        + "                   \"programy\": ["
                        + "                    \"100\" ],"
                        + "                   \"jedzenie\": \"100\","
                        + "                   \"diamenty\": \"100\","
                        + "                   \"ubrania\": ["
                        + "                    \"100\" ],"
                        + "                   \"narzedzia\": ["
                        + "                    \"100\" ]"
                        + "                   }"
                        + "         }"
                        + "], "
                        + "                   \"spekulanci\": [{"
                        + "                \"id\": \"12\","
                        + "                \"kariera\":"
                        + "                    {"
                        + "                    \"typ\": \"wypukly\""
                        + "                    },"
                        + "                   \"zasoby\":"
                        + "                   {"
                        + "                   \"programy\": ["
                        + "                    \"100\" ],"
                        + "                   \"jedzenie\": \"100\","
                        + "                   \"diamenty\": \"100\","
                        + "                   \"ubrania\": ["
                        + "                    \"100\" ],"
                        + "                   \"narzedzia\": [100]"
                        + "                   }"
                        + " }]"
                        + "}";

        Moshi moshi = new Moshi.Builder()
                .add(new AdapterKariera())
                .add(new AdapterKupowanie())
                .add(new AdapterProdukcja())
                .add(new AdapterUczenie())
                .add(new AdapterGielda())
                .build();

        JsonAdapter<Rynek> jsonAdapter = moshi.adapter(Rynek.class);
        Rynek rynek = jsonAdapter.fromJson(JString);

        if (rynek != null) {
            for (int i = 1; i <= rynek.getInfo().getDlugosc(); i++) {
                rynek.symulujDzien();
                String json = jsonAdapter.indent("  ").toJson(rynek);
                System.out.println(json);
                rynek.getInfo().modyfikujDzien();
            }
        }
    }
}