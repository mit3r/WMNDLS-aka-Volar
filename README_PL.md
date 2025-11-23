# WMNDLS - Wireless Mandala LED System

Projekt realizujący bezprzewodowe sterowanie i synchronizację taśm LED WS2812B przy użyciu protokołu ESP-NOW. System składa się z jednego nadajnika (Server) oraz wielu odbiorników (Client).

## Opis Projektu

System pozwala na centralne sterowanie efektami świetlnymi na wielu oddalonych od siebie urządzeniach bez konieczności łączenia ich przewodami ani korzystania z zewnętrznej sieci Wi-Fi. Komunikacja odbywa się bezpośrednio między układami ESP.

## Wymagania Sprzętowe

### Nadajnik (Server)

- **Mikrokontroler:** Dowolny moduł oparty o ESP8266 (możliwość użycia ESP32, ale należy dostosować biblioteki)
- **Rola:** Transmituje dane do wszystkich odbiorników

### Odbiornik (Client)

- **Mikrokontroler:** ESP-01s (ze względu na mały rozmiar)
- **Elementy wykonawcze:** Taśma LED adresowalna (np. WS2812B)
- **Zasilanie:** Odpowiednie dla taśmy LED i modułu ESP (zazwyczaj 5V z regulatorem 3.3V dla ESP)

## Konfiguracja Pinów (Odbiornik ESP-01s)

W pliku `src/client/main_client.cpp`:

- **LED_PIN (GPIO 2):** Pin danych taśmy LED
- **BUTTON_PIN (GPIO 3 / RX):** Opcjonalny przycisk (np. do zmiany ID urządzenia)

## Protokół Komunikacji

Komunikacja opiera się na protokole **ESP-NOW**, który jest szybkim, bezpołączeniowym protokołem komunikacji radiowej opracowanym przez Espressif. Jednak aby znieść ograniczenie 20 połączeń one-one, rezygnujemy z szyfrowania - nadajnik wykorzystuje adres rozgłoszeniowy.

### Struktura Pakietu Danych

Dane przesyłane są w strukturze `structMessage`

| Pole        | Typ        | Opis                                                                                    |
| ----------- | ---------- | --------------------------------------------------------------------------------------- |
| `networkId` | `uint16_t` | Identyfikator sieci (`0x308B`). Zabezpiecza przed odbieraniem danych z innych systemów. |
| `deviceId`  | `uint8_t`  | Adres urządzenia docelowego. `0` oznacza broadcast (do wszystkich).                     |
| `type`      | `uint8_t`  | Typ komendy (np. `MSG_TYPE_UPDATE_LEDS`).                                               |
| `order`     | `uint32_t` | Numer sekwencyjny pakietu. Służy do ignorowania starych lub zduplikowanych pakietów.    |
| `ledCount`  | `uint16_t` | Liczba diod w pakiecie.                                                                 |
| `leds`      | `CRGB[]`   | Tablica kolorów dla każdej diody.                                                       |

### Działanie Protokołu

1.  **Nadawanie:** Serwer oblicza klatkę animacji, inkrementuje licznik `order` i wysyła całą tablicę kolorów `leds` na adres rozgłoszeniowy.
2.  **Odbiór:** Klient nasłuchuje pakietów. Po odebraniu sprawdza:
    - Czy `networkId` się zgadza.
    - Czy pakiet jest skierowany do niego lub jest broadcastem.
    - Czy `order` jest większy od ostatnio odebranego (zabezpieczenie przed kolejnością pakietów).
3.  **Wyświetlanie:** Jeśli weryfikacja przebiegnie pomyślnie, klient natychmiastowo wyświetla przesłane dane.

## Struktura Katalogów

- `src/client/` - Kod źródłowy dla odbiorników (ESP-01s).
- `src/server/` - Kod źródłowy dla nadajnika.
- `src/shared/` - Współdzielone pliki nagłówkowe i definicje (protokół).

## Zastosowanie

Projekt idealnie nadaje się do:

- Oświetlenia dekoracyjnego rozproszonego w pomieszczeniu.
- Instalacji artystycznych, gdzie wiele elementów musi zsychornizować swoje oświetlenie bez plątaniny kabli.
- Oświetlenia scenicznego i eventowego.
