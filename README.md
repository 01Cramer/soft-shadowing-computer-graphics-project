# Renderer gładkich cieni z wykorzystaniem C++ i OpenGL

## Opis projektu
Projekt polega na zaimplementowaniu renderera gładkich cieni (soft shadows) z wykorzystaniem języka C++ i biblioteki OpenGL.
Możliwe jest dynamiczne regulowanie stopnia gładkości cieni za pomocą klawiszy F i G, gdzie F zwiększa, a G zmniejsza rozmiar filtra odpowiedzialnego za wygładzanie cieni, z wykorzystaniem techniki PCF (Percentage Closer Filtering).

Dodatkowo, zaimplementowany został podstawowy ruch kamerą za pomocą klawiszy WSAD oraz myszki, co ułatwia przeglądanie projektu i dostrzeganie zmian w cieniach.
Wyrenderowane obiekty w postaci domku w stylu RPG oraz drzew ukazują wszechstronne cienie, które znacząco różnią się strukturą.

## Uruchamianie projektu
Aby uruchomić projekt wymagane jest posiadanie Visual Studio oraz system operacyjny Windows.

Następnie należy:
- Sklonować repozytorium
- Wejść w folder projektu
- Wewnątrz folderu projektu wejść w folder Windows
- Wejść w folder soft-shadowing_vs_2022
- Uruchomić solucję soft-shadowing_vs_2022.sln
- Uruchomić projekt (Ctrl + F5)

Plik wykonywalny (soft-shadowing.exe) znajduje się w następującym miejscu:
soft-shadowing-computer-graphics-project\Windows\softshadowing_vs_2022\x64\Release

## Przykładowy Efekt - Cień komina budynku
Przed wygładzaniem:
![](https://github.com/01Cramer/soft-shadowing-computer-graphics-project/blob/main/before_softing.png)

Po wygładzaniu:
![](https://github.com/01Cramer/soft-shadowing-computer-graphics-project/blob/main/after_softing.png)

## Struktura Projektu
Pliki nagłówkowe zostały umieszczone w folderze Include.

Pliki cpp implementujące pliki nagłówkowe zostały umieszczone w folderze Common.

Plik main znajduje się w folderze soft-shadowing.

W folderze Content znajdują się potrzebne tekstury.
