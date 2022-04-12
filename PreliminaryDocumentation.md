# UXP1A - LINDA - Dokumentacja wstępna

## Skład zespołu
* Świrta Bartosz - lider
* Wojno Maciej
* Górniak Daniel
* Zieliński Jakub

Data przekazania dokuemntacji wstępnej: dd.mm.rrrr

## Temat projektu
<p align="justify">
Napisać wieloprocesorowy system realizujący komunikację w języku komunikacyjnym Linda. W uproszczeniu Linda realizuje trzy operacje:
</p> 

* output(krotka)
* input(wzorzec-krotki, timeout)
* read(wzorzec-krotki, timeout)

<p align="justify">
Komunikacja   międzyprocesowa   w   Lindzie   realizowana   jest   poprzez   wspólną   dla   wszystkich   procesów przestrzeń krotek. 
Krotki są arbitralnymi tablicami dowolnej długości składającymi się z elementów 3 typów podstawowych: string, integer, float. 
Przykłady krotek: 
</p>

* (1, ”abc”, 3.1415, ”d”)
* (10, ”abc”, 3.1415) 
* (2,3,1, „Ala ma kota”) 

<p align="justify">
Funkcja output umieszcza krotkę w przestrzeni. Funkcja input pobiera i atomowo usuwa krotkę z przestrzeni, przy czym wybór krotki następuje poprzez dopasowanie wzorca-krotki. Wzorzec jest krotką, w której dowolne składniki mogą być niewyspecyfikowane: * (podany jest tylko typ) lub zadane warunkiem logicznym. Przyjąć warunki: ==, <, <=, > , >= . Przykład: 
</p>

* input(integer:1, string:* , float:* ,string:”d”) – pobierze pierwszą krotkę z przykładu
* input (integer:>0, string:”abc”, float:*) - pobierze drugą krotkę z przykładu

<p align="justify">
Operacja read działa tak samo jak input, lecz nie usuwa krotki z przestrzeni. Operacje read i input zawsze zwracają jedną krotkę (choć pasować może więcej niż jedna). W przypadku gdy wyspecyfikowana krotka nie istnieje operacje read i input zawieszają się do czasu pojawienia się oczekiwanej danej.
</p> 

## Wariant zadania
Realizacja przestrzeni krotek ma zostać wykonana z użyciem potoków nienazwanych z centralnym procesem koordynującym (rodzicem).

## Interpretacja treści i przyjęte założenia
<p align="justify">
Proponowane rozwiązanie to biblioteka pozwalającą na uruchomienie wielu procesów potomnych, działających pod jednym procesem rodzica. Procesy potomne mogą komunikować się ze sobą za pomocą przestrzeni krotek, która jest koordynowana przez proces rodzica. Procesy potomne wykonują operacje zdefiniowane w języku Linda komunikując się z procesem rodzica poprzez potoki nienazwane. Proces rodzica tworzy i ustawia nienazwane potoki do komunikacji przy tworzeniu procesów potomnych. Oznacza to, że proces rodzica po wykonaniu operacji inicjalizacyjnych stanie się serwerem, a procesy potomne klientami.
</p> 

Przyjmujemy następujące założenia:
* Nie zakładamy maksymalnego rozmiaru krotki, jest on ograniczony jedynie przez docelowy system.
* Funkcje, które użytkownik przekazuje do uruchomienia jako pierwszą w procesie potomnym musi być odpowiedniej postaci:
  patrz [Wymagania funkcji użytkownika](#wymagania-funkcji-użytkownika)

## Opis funkcjonalny
### Ujęcie ogólne
Użytkownikowi biblioteki udostępnione są poniższe funkcjonalności:
* utworzenie instancji obiektu, który realizuje wieloprocesowy dostęp do przestrzeni krotek
* uruchomienie systemu komunikacji
* dokonywanie operacji zdefiniowanych przez język Linda:
    * wstawienie krotki do przestrzeni
    * odczytanie krotki z przestrzeni
    * odczytanie i usinięcie krotki z przestrzeni

### linda::Server
<p align="justify">
Biblioteka udostępnia programiście klasę <b><i>linda::Server</i></b>, która to realizuje wieloprocesową komunikację. Posiada wewnętrznie instancję przestrzeni krotek i podczas konstrukcji przyjmuje wskazania na funkcje, które zostaną uruchomione jako osobne procesy.
W momencie uruchomienia komunikacji, nastąpi utworzenie procesów potomnych, a proces wywołujący przejdzie w tryb pracy serwera. Korzystając z funkcji <a href="https://man7.org/linux/man-pages/man2/poll.2.html">poll(2)</a>, serwer oczekuje na komunikaty od procesów potomnych i realizuje ich żądania w przestrzeni krotek. 
Klasa udostępnia tylko jedną publiczną metodę:
</p> 

* ```void start()```

<p align="justify">
Metoda ta uruchamia system komunikacyjny. Użytkownik biblioteki może jedynie utworzyć instancję serwera i ją uruchomić.
</p>

### linda::Handle
<p align="justify">
Uchwyt do przestrzeni krotek, implementowany przez klasę, która opakowuje komunikację z procesem koordynującym przy użyciu potoków nienazwanych. Obiekt tej klasy zostanie utworzony
przez <b><i>linda::Server</i></b> i przekazany procesowi potomnemu. Klasa udostępnia trzy metody implementujące instrukcje języka Linda:
</p>

* ```std::optional<Tuple> read(TuplePattern &pattern, int timeout_ms = 0)```
* ```std::optional<Tuple> in(TuplePattern &pattern, int timeout_ms = 0)```
* ```void                 out(Tuple &tuple)```

### Wymagania funkcji użytkownika
Jedyne ograniczenie dotyczy postaci funkcji, które są przekazywane podczas kosntrukcji obiektu <b><i>linda::Server</i></b>:
```
void function(linda::Handle handle) {
    ...
}
```

### Struktury
Reprezentację danych podajemy w formie struktur języka C. Nie muszą one odzwierciedlać faktycznej implementacji (np. przedstawiona struktora mogła zostać zaimplementowana jako klasa), a służą jedynie zarysowaniu ogólnej koncepcji organizacji danych i struktury komunikatów.


## Analiza rozwiązania
Poniżej przedstawiamy analizę istonych naszym zdaniem kwiestii proponowanego rozwiązania.
### Realizacja przestrzeni krotek
Tutaj o tym jak proces rodzic tworzy procesy potomne i przechodzi w tryb serwera

### Przetwarzanie żądań przez linda::Server
Tutaj o poll

### Obsługa timeout'ów
Jak wygląda obsługa timeout'ów. Przypadki skrajne.

## Podział na moduły
* LindaServer - realizuje koordynatora przestrzeni krotek. Tworzy przestrzeń i uruchamia procesy, które chcą z niej korzystać. 
Implementuje serwer obsługujący żądania utworzony procesów.
* LindaHandle - realizuje obiekt dostępowy do przestrzeni krotek. Obiekt ten posiada interfejs języka Linda (ale go nie implementuje) i jest wykorzystywany przez procesy komunikujące się poprzez przestrzeń krotek.
* LindaTuple - zawiera struktury krotek i operacje jakie można na nich przeprowadzić.
* LindaTupleSpace - realizuje kontener na krotki, odpowiada za realizację funkcji językja Linda
* Moduł testowy - zawiera testy jednostkowe i przykładowe programy korzystające z biblioteki.

## Struktura komunikacji między modułami


## Szczegóły implementacji i używane biblioteki
Język implementacji: __C++17__  
Kompilator: __clang 13__  
Narzędzie budowania: __CMake__  
Formater kodu: __clang-format__ - format własny

Wykorzystane biblioteki: 
* GoogleTest - bibliotek testów jednostkowych
* CTest - runner testów jednostkowych