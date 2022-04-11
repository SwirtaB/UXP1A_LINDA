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
* Tutaj przyjęte założenia

## Opis funkcjonalny
### Ujęcie ogólne
Tutaj opis funkcjonalny

### Realizacja przestrzeni krotek
Tutaj o tym jak proces rodzic tworzy procesy potomne i przechodzi w tryb serwera, oraz jak w trybie serwera obsługuje żądania

### Struktury
Tutaj struktury komunikatów i struktury Tupli

## Analiza rozwiązania
* Na 100% analiza poll albo select - w zależności od tego co wybierzemy
* Analiza przypadku z timeoutem

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