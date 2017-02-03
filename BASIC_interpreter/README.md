# BASIC-interpreter

Projekt na przedmiot "Programowanie Funkcyjne" semestr zimowy 2016/17
Kacper Kulczak 279079

1. Kompilacja

wymagania: 
	Ocaml 4.04.0
	ocamlopt

budowanie:
	Wejdz do folderu "code" i użyj komendy "make"
	komenda "make clean" czyści pliki

2. Używanie
	Aby interpretować program w BASIC, należy skompilować ten interpreter, a nastepnie uruchomic go z pierszym argumentem będącym ścieżką do pliku w BASIC

3. Obsługiwany podzbiór jezyka:
	Interpreter obsługuję tylko liczby calkowite. Wyświetla również napisy.	
	Każda linijka może zaczynać się etykietą skoku ( etykietka zaczyna się od znaku '_')
	
	Obsługiwane komendy:
		- PRINT <wyrazenie / string >			//wypisywanie
		- INPUT <zmienna> = <wyrazenie>			//przypisanie
		- GOTO <etykieta>				//Skos
		- IF <wyrazenie> THEN <etykieta>		//Skok warunkowy
		- REM <dowolny tekst>				// komentarz
		- WHILE <wyrazenie>				//petla while
		- FOR <zmienna> = <wyrazenie> TO <wyrazenie>	//petla for (mozna uzywac tej zmiennej w petli)
		- DONE 						//koniec petli

4. Testy mozna znaleźć w folderze testy


