# SCZR Projekt

# Temat
System czasu rzeczywistego wizualizujący rytm muzyki na diodach LED działający na RPI.

Projekt realizowany w ramach przdmiotu SCZR (realizacja 20Z) przez zespół:
- Maciej Dmowski
- Jakub Strawa
- Adam Szałowski

# Opis projektu
Projekt jest systemem czasu rzeczywistego realizującym obsługę oraz anlizę plików audio. Poszczególne procesy działają zgodnie z diagramem oraz szczegółowym opisem poniżej. Projekt przy pomocy RPI odtwarza oraz analizuje na bieżąco plik audio, wyświetlając przy pomocy diód LED tempo (BPM) oraz spektrogram. Techniczne szczegóły implementacji opisane są w dalszych sekcjach tego pliku.

# Diagram procesów
* Trzeba dodać głośnik do procesu 1 i jeszcze tam dopisać: Odtwarzanie pliku audio
* Dobrze by było żeby całość otoczyć ramką i podpisać, że to jest RPI
![Diagram](https://codimd.s3.shivering-isles.com/demo/uploads/upload_7ab58c37e6247058c8606d481e01cddb.png)

# Opis procesów

## Zarządca procesów
Odpowiada za odpowiednie uruchomienie i zamknięcie procesów oraz inicializację i terminację kolejek wiadomości. Dodatkowo, proces ten będzie zapisywał otrzymane komunikaty z logami na dysk w celu analizy skuteczności rozwiązania oraz jakości synchronizacji procesów.

## Proces 1
Ma za zadanie wczytać odpowiedni plik audio, odtworzyć go oraz przesłać pakiety danych do kolejnego procesu. Wczytanie i obsługa plików odbywać się będzie przez bibliotekę [miniaudio](https://github.com/mackron/miniaudio). 

## Proces 2
Ma za zadanie przeanalizować otrzymane dane aby wykryć rytm muzyki. Następnie proces przesła dane do kolejnego procesu. Analiza wykonywana jest przy pomocy biblioteki [BTrack](https://github.com/adamstark/BTrack).

## Proces 3
Ma za zadanie wyświetlić otrzymane dane o rytmie na diodzie LED.

# Synchronizacja
Proces 2 i 3 oczekują na komunikaty od procesów 1 i 2. Procesy są zawieszane do czasu pojawienia się komunikatu.
Proces 1 jest głównym producentem danych. Zawieszenie następuje w momencie przepełnienia kolejki.
Synchronizacja nieblokująca między procesem zarządzającym a kolejkami logów.

# Komunikacja
Komunikacja między procesami odbywać się będzie za pomocą kolejek wiadomości.

1. message_queue_12
    Przesył buforu próbek dzwiękowych.
    Struktura wiadomości:
    ```c
        struct msg_buf_12 {
            unsigned int id;
            data;
        } 
    ```
2. message_queue_23
    Przesył wykrytego rytmu.
    Struktura wiadomości:
    ```c
        struct msg_buf_23 {
            unsigned int id;
            data;
        }
 
    ```
3. log_msg_queue_d1, log_msg_queue_d2, log_msg_queue_d3
    Przesył wiadomości o czasie otrzymania oraz wysłania wiadomości.
    Struktura wiadomości:
    ```c
        struct log_msg_buf{
            unsigned int msg_id;     
            struct timeval tv;
            enum MsgType {RECEIVED, SENT} type;    
        } 
    ```
    
# Podział obowiązków
#### Maciej Dmowski
* Utworzenie zarządcy procesów
* Utworzenie procesu 1
* Utworzenie kolejek wiadomośći
* Wdrożenie mechanizmów synchronizacji
#### Jakub Strawa
* Utworzenie procesu 2
* Określenie sposobu przesyłania pakietów
* Utworzenie dokumentacji oraz analiza eksperymentów
#### Adam Szałowski
* Utworzenie procesu 3
* Przygotowanie RaspberryPi do działania
* Przeprowadzenie eksperymentów

# Konfiguracja RPI0w

# Planowane eksprymenty
Początkowo zakładamy brak jakiejkolwiek ingerencji w sposób działania systemu. Pierwszym eksperymentem będzie zatem porównanie działania diody do rzeczywistego słyszalnego tempa. Zmierzone opóźnienia wykorzystamy do określenia jakości synchronizacji. Uzyskane w ten sposób dane porównamy z danymi uzyskanymi z logów otrzymywanych przez zarządcę procesów. Następnym krokiem będzie próba dostrojenia systemu (zmniejszenia opóźnień) poprzez dobór priorytetów dla procesów metodą eksperymentalną. Dla każdej takiej próby wykonany zostanie eksperyment mający na celu jakość dostrojenia. Jeśli zaś ta metoda. zawiedzie, kolejnym planowanym eksperymentem będzie przydzielenie każdemu procesowi własny rdzeń procesora. Następnie powtórzony zostanie eksperyment polegający na porównaniu działania diody do słyszalnego tempa.
