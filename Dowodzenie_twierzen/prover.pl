/* Funktory do budowania klauzul */
:- op(200, fx, ~).
:- op(500, xfy, v).
/* Główny program: main/1. Argumentem jest atom będący nazwą pliku
* z zadaniem. Przykład uruchomienia:
*
?- main('zad125.txt').
* Plik z zadaniem powinien być plikiem tekstowym, na którego
* początku znajduje się lista klauzul zbudowanych za pomocą funktorów
* v/2 i ~/1 (szczegóły znajdują się w opisie zadania). Listę zapisujemy
* w notacji prologowej, tj. rozdzielając elementy przecinkami
* i otaczając listę nawiasami [ i ]. Za nawiasem ] należy postawić
* kropkę. Wszelkie inne znaki umieszczone w pliku są pomijane przez
* program (można tam umieścić np. komentarz do zadania).
*/
main(FileName) :-
	readClauses(FileName, Clauses),
	prove(Clauses, Proof),
	writeProof(Proof).
/*
*
*
*
*
Silnik programu: predykat prove/2 do napisania w ramach zadania.
Predykat umieszczony poniżej nie rozwiązuje zadania. Najpierw
wypisuje klauzule wczytane z pliku, a po nawrocie przykładowy dowód
jednego z zadań. Dziewięć wierszy następujących po tym komentarzu
należy zastąpić własnym rozwiązaniem. */
prove(Clauses, Proof):-
	convertionCtoL(Clauses, List,1 ,Number),
	find_empty(List, ProofTemp,Number),
	convertionLtoC(ProofTemp, Proof).
	
	

find_empty([[[] | Y] | X], [[[] | Y] | X],_ ).
find_empty(List, Proof,Number):-
	member([Clause1,Id1,Temp1,Temp2], List),
	select([Clause1,Id1,Temp1,Temp2], List, SmallerList),
	member([Clause2,Id2,_,_], SmallerList),
	rezolution(Clause1, Clause2, Res),
	\+ member([Res,_,_,_], List),
	Result = [Res,Number, Id1, Id2],
	NewNumber is Number +1,!, 
	find_empty([Result | List], Proof, NewNumber).

rezolution(Clause1, Clause2, Res):-
	member( Elem, Clause1),
	elimination(Elem, Clause2, Result1),
	select( Elem, Clause1, Result2),
	append(Result1, Result2, Result3),
	sort(Result3,Res).


elimination(Elem, Clause, Result):-
	elimination2(Elem, Clause, Result).
elimination2( ~Elem, Clause, Result ):-!,
	member(Elem, Clause ),
	select(Elem, Clause, Result).
elimination2( Elem, Clause, Result ):-
	member(~Elem, Clause ),
	select(~Elem, Clause, Result).

convertionLtoC(List, Clauses):-
	convertionLtoC(List, Clauses, []).
convertionLtoC([], X, X).
convertionLtoC([[List,_,From1,From2]|T], Clauses, Acc):-
	create_id(From1, From2, Id),
	list_to_claus(List, C),
	convertionLtoC(T,Clauses, [(C, ( Id ) ) | Acc]). 

create_id(axiom, axiom, axiom).
create_id(X,Y, (X,Y) ):-
	number(X).

list_to_claus( [], []).
list_to_claus( [H | T], Claus):-
	list_to_claus( T, Claus ,H).
list_to_claus([],X,X).
list_to_claus([H | T], Res, Acc):-
	list_to_claus(T, Res, H v Acc).

convertionCtoL(Clauses, List, Number, ExitNumber):-
	convertionCtoL(Clauses, List, [], Number, ExitNumber)
	.
convertionCtoL([],X,X,Num, Num).
convertionCtoL([H|T], Result, Acc, Number,ExitNumber):-
	claus_to_list(H,Res),
	ExactRes = [Res,Number,axiom, axiom],
	NewNumber is Number +1,
	convertionCtoL(T, Result, [ExactRes | Acc], NewNumber, ExitNumber).

claus_to_list(Claus, List):-
	claus_to_list(Claus, [], Res),
	sort(Res, List).
claus_to_list( X v Y , Acc, List ):-!,
	claus_to_list( Y, [X | Acc], List ).
claus_to_list( X , Acc, [X | Acc] ).

%make_axioms(List, Result).


/* Pozostała część pliku zawiera definicje predykatów wczytujących listę
* klauzul i wypisujących rozwiązanie. Wykorzystane predykaty
* biblioteczne SWI-Prologu (wersja kompilatora: 6.6.6):
3*
*
close/1
*
format/2
*
length/2
*
maplist/3
*
max_list/2
*
nl/0
*
open/3
*
read/2
*
write_length/3
*
* Dokumentację tych predykatów można uzyskać wpisując powyższe napisy
* na końcu następującego URL-a w przeglądarce WWW:
*
http://www.swi-prolog.org/pldoc/doc_for?object=
* np.
*
http://www.swi-prolog.org/pldoc/doc_for?object=write_length/3
* lub jako argument predykatu help/1 w konsoli interpretera SWI
* Prologu, np.
*
?- help(write_length/3).
*/
readClauses(FileName, Clauses) :-
	open(FileName, read, Fd),
	read(Fd, Clauses),
	close(Fd).
/* Wypisywanie dowodu */
writeProof(Proof) :-
	maplist(clause_width, Proof, Sizes),
	max_list(Sizes, ClauseWidth),
	length(Proof, MaxNum),
	write_length(MaxNum, NumWidth, []),
	nl,
	writeClauses(Proof, 1, NumWidth, ClauseWidth),
	nl.
clause_width((Clause, _), Size) :-
	write_length(Clause, Size, []).
writeClauses([], _, _, _).
writeClauses([(Clause,Origin) | Clauses], Num, NumWidth, ClauseWidth) :-
	format('~t~d~*|. ~|~w~t~*+ (~w)~n',[Num,NumWidth,Clause,ClauseWidth,Origin]),
	Num1 is Num + 1,
	writeClauses(Clauses, Num1, NumWidth, ClauseWidth).
/* twi 2016/03/13 vim: set filetype=prolog fileencoding=utf-8 : */
