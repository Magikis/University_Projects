%	Compilator jezyka Algol16 na architekture Sextium 
%
%	Kacper Kulczak 279079 
%	Compilator obsluguje: 
%	TAK: nierekurencyjne procedury bez parametrów z lokalnymi zmiennymi 3pkt
%	NIE: mozliwosc rekurencyjnego wywoływania procedur  3pkt 
%	TAK: mozliwosc przekazywania parametrów do procedur przez wartosc 2pkt
%	NIE: mozliwosc przekazywania parametrów do procedur przez nazwe 10pkt
%	TAK: mozliwosc zagniezdzania procedur 10pkt
%
%			!!!	IDEA	!!!
%	W compilatorze wystepuje stos. 
%	Wszystkie wyrazenie arytmetyczne oraz boolowskie sa zamieniane na odwrotna notacje Polska i obliczane przy pomocy stosu.
%	Niestety wiekszosc operacji np. =, <, wymaga skokow, dlatego tez podczas translacji, przesylam zmienna "Num", ktora pozwala uniknac konfilktow 
%	nazw moich etykiet skokow: przykladoweo label(true,Num)  
%	Z tej samej techniki korzysta przy while, if ifelse...
%	Przy wybranych przezemnie warunkach zadania wszystkie zmienne traktuje jako globalne (Dzieki zmiennej "Path" unikam konlfiktu nazw zmiennych)  
%	Zbieram wszystkie zmienne z programu, tworze z nich liste var_list i pod sam koniec kompilacji tworze slownik, ktory wie gdzie konczy sie kod
%	wykonywalny programu i przyporzadkowuje kazdej zmiennej adres.
%	Funkcje sa przechowywane w liscie funckji, w momencie gdy ktora kolwiek funkcja jest wywolywana w programie wyciagam jej "tresc" i tlumacze
%	na instrukcje. Jest to najprostszy sposob na unikanie konfliktow label()
%
%	Kolejnosc dzialania predykatu assembly
%		1. Zbudowanie listy zmiennych (potrzebne do zbudowania slownika)
%		2. Zbudowanie listy funkcji, ktora mozna przeszukiwac, majac sama funkcje i sciezke dostepu
%		3.	Tlumaczenie instrukcji z uzyciem pop,push,store(),load()
%		4. Inicjacja stosu
%		5.	Translacja pop i push na bardziej podstawowe
%		6.	Translacja load(), store(), ++, -- na bardziej podstawowe
%		7. Wyjmowanie zawartosci const(), formatowanie instrukcji do 4, formatowanie, aby label() zawsze mial za soba dopelnienie do 4
%		8. Zbudowaie slownika: Obliczenie "wartosci/adresu" kazdego label() umieszczonego w kodzie, i wyjecie adresu, od ktorego nie ma juz instrukcji
%		9.	Wyliczenie i dodanie do slownika adresow zmiennych
%		10.Dodanie adresu rejestrow do slownika (rsp,r1,r2)
%		11.Przetlumaczenie w kodzie wartosci pisanych na adresy (label(),var(),rsp,r1,r2)
%		12.Przetlumaczeni instrukcji i liczb na liczby w systemie dziesietnym
%
%	Lexer oraz Parser sa bardzo mocno wzorowane na pliku wykladowcy "while_parser" (czasem, wrecz przepisane fragmenty)

%	Predykaty obslugi 
algol16(Source,SextiumBin):-
	myparse(Source, TreeP),
	assembly(TreeP,ASM),
	printHex(ASM,SextiumBin),!.

myparse(CharCodeList, Absynt):-
   phrase(lexer(TokList), CharCodeList),
   phrase(program(Absynt), TokList).

%	Zamiana na dziesietny -> szestanstkowy
printHex([],[]):-!.
printHex([H|T],[A|Exit]) :-
  format(atom(A),'~|~`0t~16r~4+', H),
  printHex(T,Exit).

%	LEXER
lexer(Tokens) -->
   white_space,
   (  (  ":=",      !, { Token = tokAssgn }
      ;  ";",       !, { Token = tokSColon }
      ;  "(",       !, { Token = tokLParen }
      ;  ")",       !, { Token = tokRParen }
      ;  "+",       !, { Token = tokPlus }
      ;  "-",       !, { Token = tokMinus }
      ;  "*",       !, { Token = tokTimes }
      ;  "=",       !, { Token = tokEq }
      ;  "<>",      !, { Token = tokNeq }
      ;  "<=",      !, { Token = tokLe }
      ;  "<",       !, { Token = tokL }
      ;  ">=",      !, { Token = tokGe }
      ;  ">",       !, { Token = tokG }
      ;  ",",       !, { Token = tokComma }
      ;  digit(D),  !,
            number(D, N),
            { Token = tokNumber(N) }
      ;  letter(L), !, identifier(L, Id),
            {  member((Id, Token), [ (and, tokAnd),
                                     (div, tokDiv),
                                     (do, tokDo),
                                     (done, tokDone),
                                     (else, tokElse),
                                     (false, tokFalse),
                                     (fi, tokFi),
                                     (if, tokIf),
                                     (mod, tokMod),
                                     (not, tokNot),
                                     (or, tokOr),
                                     (skip, tokSkip),
                                     (then, tokThen),
                                     (true, tokTrue),
                                     (while, tokWhile),
                                     (call,	tokCall),
                                     (begin,	tokBegin),
                                     (end,	tokEnd),
                                     (local,	tokLocal),
                                     (procedure, tokProcedure),
                                     (program,	tokProgram),
                                     (read,	tokRead),
                                     (return,	tokReturn),
                                     (value,	tokValue),
                                     (write,	tokWrite)
                                     ]),
               !
            ;  Token = tokVar(Id)
            }
      ;  [_],
            { Token = tokUnknown }
      ),
      !,
         { Tokens = [Token | TokList] },
      lexer(TokList)
   ;  [],
         { Tokens = [] }
   ).

white_space -->
   "(*", !, endcomment, white_space
   ; [Char], { code_type(Char, space) }, !, white_space
   ; [].
endcomment -->
	"*)",!
	; [_], endcomment.
   
digit(D) -->
   [D],
      { code_type(D, digit) }.

digits([D|T]) -->
   digit(D),
   !,
   digits(T).
digits([]) -->
   [].

number(D, N) -->
   digits(Ds),
      { number_chars(N, [D|Ds]) }.

letter(L) -->
   [L], { code_type(L, alpha) }.
%	Pozwolenie zmiennym posiadanie znakow "_" oraz "'"
alphanum([A|T]) -->
   [A], { code_type(A, alnum);A is 95 ;A is 39 }, !, alphanum(T).
alphanum([]) -->
   [].

identifier(L, Id) -->
   alphanum(As),
      { atom_codes(Id, [L|As]) }.
%	PARSER
%	Zmienna Path to sciezka "dostepu" Pozwala stwierdzic z drzewa jakich funkcji pochodzi dany blok, a co za tym idzie rowniez zmienne
% 	Jest to zmienna bardz czesto kopiowana w programie
program(Start)-->
	[tokProgram], [tokVar(A)], blok(B,[A]),
	{Start = B}.

blok(Exit,Path)-->
	deklaracje(A,Path), [tokBegin], instrukcja_zlozona(B,Path), [tokEnd],
	{Exit =(A,B)}.

deklaracje(Exit,Path)-->
	 deklaracja(A,Path), deklaracje(B,Path), {Exit = [A|B]}
	;[] , {Exit = []}.
	
deklaracja(Exit,Path)-->
	deklarator(A,Path),!, {Exit = A}
	;procedura(B,Path),	{Exit = B}.

deklarator(Exit,Path) -->
	[tokLocal], zmienne(B,Path), {Exit = B}.

zmienne(Exit,Path) -->
	zmienna(A,Path), ( [tokComma],!,zmienne(B,Path), {Exit = [A|B]}
	; {Exit = [A]} ).

zmienna(Exit,Path)-->
	[tokVar(A)], {Exit = var(A,Path)}.

procedura(Exit,Path)-->
	[tokProcedure], [tokVar(A)],[tokLParen],argumenty_formalne(F,[A|Path])
	,[tokRParen], blok(B,[A|Path]),
	{Exit = proc([A|Path],F,B)}.

argumenty_formalne(Exit,Path)-->
	zmienna(A,Path),!, 
	( [tokComma],!,argumenty_formalne(B,Path), {Exit = [A|B]}	; {Exit = [A]} )
	; [], {Exit = []}.

instrukcja_zlozona(Exit,Path)-->
	instrukcja(A,Path), (
	[tokSColon],!, instrukcja_zlozona(B,Path), {Exit = [A|B]}
	;[],{Exit = [A]}
	).
	
instrukcja(Exit,Path)-->
	 zmienna(A,Path),!, [tokAssgn], wyrazenie_arytm(B,Path), {Exit=assign(A,B)}
	;[tokIf],wyrazenie_log(A,Path),[tokThen],instrukcja_zlozona(B,Path),
	(
		[tokFi],!,{Exit = if(A,B)} 
		;[tokElse],!,instrukcja_zlozona(C,Path),[tokFi],{Exit = ifelse(A,B,C)}
	)
	;[tokWhile],!,wyrazenie_log(A,Path),[tokDo],instrukcja_zlozona(B,Path),[tokDone],{Exit = while(A,B)}	
	;[tokCall],!,wywolanie_procedury(A,Path), {Exit = call(A)}
	;[tokReturn],!, wyrazenie_arytm(A,Path), {Exit = return(A)}
	;[tokRead],!,zmienna(A,Path), {Exit = ioRead(A)}
	;[tokWrite],!, wyrazenie_arytm(A,Path), {Exit = ioWrite(A)}.




wyrazenie_arytm(Exit,Path) -->
    skladnik(A,Path), wyrazenie_arytm(A,Exit,Path).
wyrazenie_arytm(Acc, Exit,Path)-->
   additive_op(A),!,skladnik(B,Path),
   {Acc1 = [Acc,B,A]},
   wyrazenie_arytm(Acc1, Exit,Path).
wyrazenie_arytm(Acc, Acc1,_)-->[],{flatten(Acc,Acc1)}.

skladnik(Exit,Path) -->
    czynnik(A,Path), skladnik(A,Exit,Path).
skladnik(Acc, Exit,Path)-->
   multiplicative_op(A),!,czynnik(B,Path),
   {Acc1 = [Acc,B,A]},
   skladnik(Acc1, Exit,Path).
skladnik(Acc, Acc,_)-->[].


czynnik(Exit,Path)-->
	 [tokMinus],wyrazenie_proste(A,Path), {Exit = [A,neg]} 
	;wyrazenie_proste(B,Path), {Exit = B }.

wyrazenie_proste(Exit,Path)-->
	 [tokLParen],wyrazenie_arytm(A,Path),[tokRParen],{Exit = A}
	;wyrazenie_atomowe(B,Path), {Exit = B}.

wyrazenie_atomowe(Exit,Path)-->
	 wywolanie_procedury(A,Path),{Exit = A}
	;zmienna(B,Path), {Exit=B}
	;liczba(C), {Exit = C}.

wywolanie_procedury(Exit,Path)-->
	zmienna(A,Path),[tokLParen],argumenty_funkcji(B,Path),
	[tokRParen], {Exit=function(A,B)}.

argumenty_funkcji(Exit,Path)-->
	wyrazenie_arytm(A,Path),!,
	(
		[tokComma],!, argumenty_funkcji(B,Path), {Exit = [A|B]}
		; {Exit = [A]}
	)
	;[],{Exit=[]}. 
	
liczba(Exit)-->
	[tokNumber(A)],{Exit=num(A)}.

wyrazenie_log(Exit,Path)-->
	 koniunkcja(A,Path), wyrazenie_log(A, Exit,Path).
wyrazenie_log(Acc, Exit,Path)-->
	[tokOr],!,koniunkcja(A,Path),
	{Acc1 = [Acc,A,or]},
	wyrazenie_log(Acc1, Exit,Path).
wyrazenie_log(Acc, Acc1,_)-->[],{flatten(Acc,Acc1)}.	

koniunkcja(Exit,Path)-->
	 warunek(A,Path), koniunkcja(A, Exit,Path).
koniunkcja(Acc, Exit,Path)-->
	[tokAnd],!,warunek(A,Path),
	{Acc1 = [Acc,A,and]},
	koniunkcja(Acc1, Exit,Path).
koniunkcja(Acc, Acc,_)-->[].	


warunek(Exit,Path)-->
	 [tokNot],!, wyrazenie_relacyjne(A,Path), {Exit = [A|not]}
	;wyrazenie_relacyjne(B,Path), {Exit = B}.

wyrazenie_relacyjne(Exit,Path)-->
	 [tokLParen],wyrazenie_log(A,Path),[tokRParen],{Exit = A}
	;wyrazenie_arytm(B,Path), rel_op(C), wyrazenie_arytm(D,Path), {Exit= [B,D,C]}.

additive_op(add) -->
   [tokPlus], !.
additive_op(sub) -->
   [tokMinus].

multiplicative_op(mul) -->
   [tokTimes], !.
multiplicative_op(div) -->
   [tokDiv], !.
multiplicative_op(mod) -->
   [tokMod].

rel_op(equal) -->
   [tokEq], !.
rel_op(not_equal) -->
   [tokNeq], !.
rel_op(less) -->
   [tokL], !.
rel_op(less_equal) -->
   [tokLe], !.
rel_op(greater) -->
   [tokG], !.
rel_op(greater_equal) -->
   [tokGe].	

%Compiler

% predykat glowny
assembly( (Declarations,Main), ASM_2):-
	make_var_list(Declarations, UglyVarList),
	flatten(UglyVarList,VarList),
	make_functions_blocks(Declarations, UglyFuncList),
	flatten(UglyFuncList,FunctionsList),
	trans_instructions(Main, UglyProlog_List_1, 0, _, FunctionsList),
	Stack_plus_UglyProlog_List_1=[[const(num(65531)),store(rsp)]|UglyProlog_List_1],
	flatten(Stack_plus_UglyProlog_List_1,Prolog_List_1),
	trans_stack(Prolog_List_1,UglyProlog_List_2),
	flatten(UglyProlog_List_2,Prolog_List_2),
	trans_registers(Prolog_List_2,UglyProlog_List_3),
	flatten(UglyProlog_List_3,Prolog_List_3),
	trans_const(Prolog_List_3,[],0,UglyProlog_List_4),
	flatten(UglyProlog_List_4,Prolog_List_4),
	make_dictionary(Prolog_List_4,Prolog_List_5,0,LastInstrAd,LabelDictionary), 
	complement_dictionary(VarList,Dictionary,LabelDictionary,LastInstrAd ),	
	append([(rsp,65534),(r1,65533),(r2,65532)],Dictionary,OneDictionary),
	trans_adresses(Prolog_List_5,OneDictionary,ASM_List_1),
	trans_bin(ASM_List_1,ASM_2).
	

%Tworzenie listy zmiennych: zbieranie rozny deklarowan "local" w jedno oraz poieranie zmiennych lokalnych z kazdej funkcji (z uwzglednieniem Path)
% W miejsce Exit tworzona jest lista wszystkich zmiennych w programie 
make_var_list([],[]):-!.
make_var_list([H|  T],[New|Exit]):-
	(
	H = proc(_,Args,(Dec,_)),!,make_var_list(Dec,NewTemp), New = [Args | NewTemp]
	;H = [var(A,B)|T1],!, make_var_list(T1,TEMP), New=[var(A,B)|TEMP]
	;H = var(C,D),!, New=[var(C,D)]
	;H = [], New = []
	), make_var_list(T,Exit).

%	Tworzenie listy funkcji w programie, jako jednej listy do przegladania przy kodowaniu odwolan do 
make_functions_blocks([],[]):-!.
make_functions_blocks([H|T],[New|Exit]):-
	(
	H =proc(Name,Args,(Dec,Instr)),!, make_functions_blocks(Dec, Temp), New = [(Name,Args,Instr)|Temp]
	; New = []
	),make_functions_blocks(T,Exit).

%	Num i Enum to taki iterator, ktory dolaczany do adresow skokow "label()" unika konfilktow nazw adresow skokow
%
% FunctionList to kopiowana zawartoscwszystkich funkcji
%	Gdy dana funkcja jest wolana, z FunctionList wyszukiwana jest jej tresc i na biezaco jest ona tlumaczona na komendy (w celu braku konfliktu numeracji "label()" czyli adresow skokow)
trans_instructions([],[], X, X,_).
trans_instructions([H|T],[Next|Exit], Num, Enum,FunctionsList):-(
	 H= assign(A,B),!,trans_expr(B,AssExit,Num,NumEx,FunctionsList), 
	 Next=[AssExit,pop,store(A)]
	;H= ioRead(C),!, Next = [const(num(1)),syscall,store(C)],
	NumEx is Num
	;H= ioWrite(D),!,trans_expr(D,ExitWrite,Num,NumEx,FunctionsList),
	 Next = [ExitWrite,pop,swapd,const(num(2)),syscall]
	;H= if(E,F),!,trans_expr(E,IfBool,Num,Num1,FunctionsList),
	trans_instructions(F,IfInstr,Num1,Num2,FunctionsList),
	NumEx is Num2+1,	Next=
	[IfBool,pop,swapa,const(label(false,Num2)),swapa,branchz,IfInstr,
	label(false,Num2)]
	;H=ifelse(G,I,J),!,
	trans_expr(G,IfelseBool,Num,Num1,FunctionsList),
	trans_instructions(I,IfelseTrue,Num1,Num2,FunctionsList),
	trans_instructions(J,IfelseFalse,Num2,Num3,FunctionsList),
	NumEx is Num3+1,	Next=
	[IfelseBool,pop,swapa,const(label(false,Num3)),swapa,branchz,
	IfelseTrue,const(label(end,Num3)),jump,label(false,Num3),
	IfelseFalse,label(end,Num3)]
	;H = while(K,L),!,
	trans_expr(K,WhileBool,Num,Num1,FunctionsList),
	trans_instructions(L,WhileInstr,Num1, Num2,FunctionsList),
	NumEx is Num2 +1,
	Next = [label(start,Num2),WhileBool,pop,swapa,const(label(end,Num2)),
	swapa,branchz,WhileInstr,const(label(start,Num2)),jump,
	label(end,Num2)]
	;H= call(function(var(M,Path),ArgsExprs)),!,
	find_function(FunctionsList,[M|Path],FUNC,FunctionsList,Args), 
	call_arguments(ArgsExprs,Args,Num,Num1,ASMinitArgs,FunctionsList),
	trans_instructions(FUNC,ASMbodyFunc,Num1,NumEx,FunctionsList),
	 append([ASMinitArgs | ASMbodyFunc],pop,Next)
	;H = return(M),!,trans_expr(M,Next,Num,NumEx,FunctionsList)
	),
	trans_instructions(T,Exit,NumEx,Enum,FunctionsList).

%	Wyjecie listy argumentow i tresci funkcji po nazwie
%	Przeszukiwanie roznych sciezek
find_function(_,[_],_,_,_):-!,write('Error, Function name, not found').
find_function([],[TrueName,_|TName],Exit,List,Args):-!,
	find_function(List, [TrueName|TName], Exit, List,Args).
find_function([(Name,Args,Exit)|_],Name,Exit,_,Args):-!.
find_function( [_|T],Name,Exit,List,Args ):-
	find_function(T,Name,Exit,List,Args).


%	Translacja instrukcji wywolania, zapisanie wyr_artym do Argumentow wywolania
call_arguments([],[],Num,Num,[],_).
call_arguments([H|T],[HArg|TArg],Num, ExNum,[Exit|InitArgs],FunctionList ):-
	 trans_expr(H, Expr, Num, Num1, FunctionList), 
	 Exit = [Expr,pop,store(HArg)],
	 call_arguments(T,TArg,Num1,ExNum, InitArgs, FunctionList).

%	Translacja wyrazenie boolowskiego i arytmetycznego       
%	Niektore operacje tlumacze, na zbior innych operacji, wiec nie zawsze zwiekszam Num
trans_expr([],[],X,X,_).
trans_expr([H|T], [New|Exit], Num, Enum,FunctionsList):-
	H= or,!, 
	trans_expr([add,num(0),greater|T], Exit,Num,Enum,FunctionsList),
	New=[]
	;H= and,!,
	trans_expr([add,num(2),sub,num(0),equal|T], Exit, Num, Enum, FunctionsList),
	New=[]
	;H=less_equal,!,trans_expr([greater,not|T], Exit, Num, Enum, FunctionsList), New=[]
	;H=greater_equal,!,trans_expr([less,not|T], Exit, Num, Enum, FunctionsList),New=[]
	;H=not_equal,!,trans_expr([equal,not|T], Exit, Num, Enum, FunctionsList),New=[]
	;H=neg,!,trans_expr([num(-1),mul|T],Exit,Num,Enum,FunctionsList),
	New=[]
	;H = function(var(Name,Path),ArgsExprs),!,
	find_function(FunctionsList,[Name|Path],Instr,FunctionsList,Args),
	call_arguments(ArgsExprs,Args,Num,Num1,ASMinitArgs,FunctionsList),
	trans_instructions(Instr,FuncASM, Num1, Num2,FunctionsList),
	New = [ASMinitArgs|FuncASM], trans_expr(T,Exit, Num2, Enum, FunctionsList)
	;Num1 is Num + 1,
	(
	H=var(A,AP),!,New=[const(var(A,AP)),swapa,load,push]
	;H=num(B),!,New=[const(num(B)),push]
	;action_op(H),!,New = [pop, store(r1), pop, load(r1), swapd, H, push]
	;H = mod,!, New=[pop, store(r1), pop, load(r1), swapd, div,swapd,
	const(num(-16)),swapd,shift, push] 
	;H = equal, !, New=
	[pop,store(r1),pop,load(r1),sub,swapa,const( label(true,Num) ),
	swapa,branchz,const(num(0)),swapd,const( label(end,Num ) ),jump,
	label(true,Num), const(num(1)),swapd, label(end,Num),swapd,push]
	;H = less,!, commands_cmp(less, New, Num)
	;H = greater,!, commands_cmp(greater, New, Num)
	;H = not,!,New= 
	[pop,swapa,const( label(false,Num) ), swapa,branchz,const(num(0)),
	swapd,const(label(end,Num)),jump,label(false,Num),const(num(1)),swapd,
	label(end,Num),swapd,push]
	),
	trans_expr(T,Exit, Num1, Enum, FunctionsList).

%	Dlugie porownanie w korym nie wsytepuje problem przekrecenia sie
commands_cmp(Type, New, Num):-
	(Type = less,!, Start = [pop, store(r2), pop, store(r1)]
	;Type = greater,!, Start = [pop,store(r1),pop,store(r2)]
	), New = 
	[Start, load(r1),swapa,const(label(ygreater0,Num)),swapa,branchn,
label(secondif,Num),
	load(r1),swapd,const(num(-1)),mul,swapa,const(label(yless0,Num)),swapa,branchn,
	const(label(thirdif,Num)),jump,
label(ygreater0,Num),
	load(r2),swapd,const(num(-1)),mul,swapa,const(label(true,Num)),swapa,branchn,
	const(label(secondif,Num)),jump,
label(yless0,Num),
	load(r2),swapa,const(label(false,Num)),swapa,branchn,
label(thirdif,Num),
	load(r2),load(r1),sub,swapa,const(label(true,Num)),swapa,branchn,
label(false,Num),
	const(num(0)),push,const(label(end,Num)),jump,
label(true,Num),
	const(num(1)),push,
label(end,Num)].
	
	
action_op(add).
action_op(sub).
action_op(mul).
action_op(div).

%	Translacja komend posrednich
trans_stack([],[]):-!.
trans_stack([H|T],[New|Exit]):-
	(
	 H=pop,!,New=[load(rsp),++,store(rsp),swapa,load]
	;H=push,!,New=[load(rsp),swapa,swapd,store,swapa,--,store(rsp)]
	;New = H
	),trans_stack(T,Exit).

%	Translacja komend posrednich
trans_registers([],[const(num(0)),syscall]):-!.
trans_registers([H|T],[New|Exit]):-
	(
	 H=load(A),!,New=[swapd,const(A),swapa,load]
	;H=store(B),!,New=[swapa,const(B),swapa,store]
	;H= --,!,New=[swapd,const(num(1)),swapd,sub]
	;H= ++,!,New=[swapd,const(num(1)),swapd,add]
	;New = H
	),
	trans_registers(T,Exit).

%	Grupowanie komend po 4 i eksport zawartosci const poza te czworki
%	Nopowanie komend do 4 przed kazdym label() //adresem skoku
trans_const([],ValList,Counter,New):-!,
	noping(Counter,Nops),append(Nops,ValList,New).
trans_const([H|T],ValList,Counter ,[New|Exit]):-
	H=label(B,C),!,noping(Counter,Nops),append(Nops,ValList,Coms),
	append(Coms,[label(B,C)],New),
	trans_const(T,[],0,Exit)
	;
	(H=const(A),!,append(ValList,[val(A)],NewValList),ValHead=const
	;H =ValHead,	NewValList = ValList
	),
	(Counter == 3,!,append([ValHead],NewValList,New),
	trans_const(T,[],0,Exit)
	;New = ValHead,NewCounter is Counter + 1,
	trans_const(T,NewValList,NewCounter,Exit)
	).
noping(0,[]).	
noping(1,[nop,nop,nop]).
noping(2,[nop,nop]).
noping(3,[nop]).

%	Budowa slownika, pierwszy etap, przegladniecie calego pliku w celu ustawienia adresow, [ (label(),adres) , ... ] 
%	Wyjecie label() z listy kodu
make_dictionary([],[],X,X,[]):-!.
make_dictionary([val(A)|T],[val(A)|Exit],AdCounter, AdEx,Dict):-!,
	PlusOne is AdCounter+1,make_dictionary(T,Exit,PlusOne,AdEx,Dict).
make_dictionary([label(A,B)|T],Exit,AdCounter,AdEx,
	[(label(A,B),AdCounter)|Dict]):-!,
	make_dictionary(T,Exit,AdCounter,AdEx,Dict).
make_dictionary([A,B,C,D|T],[A,B,C,D|Exit],AdCounter,AdEx,Dict):-
	PlusOne is AdCounter+1,make_dictionary(T,Exit,PlusOne,AdEx,Dict).

% Po kodzie programu umieszczam zmienne. Dodanie do slownika [ (zmienna, adres), ... ]
complement_dictionary([],X,X,_).
complement_dictionary([H|T],[(H,LastInstrAd)|Exit],Dictionary,LastInstrAd):-
	PlusOne is LastInstrAd+1,
	complement_dictionary(T,Exit,Dictionary,PlusOne).

%	Zdjecie num() z liczb w kodzie
trans_adresses([],_,[]):-!.
trans_adresses([val(num(A))|T],Dict,[num(A)|Exit]):-!,
	trans_adresses(T,Dict,Exit).
trans_adresses([val(A)|T],Dict,[num(Adress)|Exit]):-!,
	find_adress(A,Dict,Adress,Dict),
	trans_adresses(T,Dict,Exit).
trans_adresses([A,B,C,D|T],Dict,[A,B,C,D|Exit]):-
	trans_adresses(T,Dict,Exit).

%	Podstawienie pod kazda zmienna, konkretnego adresu liczbowego
find_adress(El,[(El,Adress)|_],Adress,_):-!.
find_adress(var(Name,[_|TPath]),[],Adress,DictCP):-!,
	find_adress(var(Name,TPath),DictCP,Adress,DictCP).
find_adress(var(_,[]),_,_,_):-!.
find_adress(El,[_|T],Adress,DictCP):-
	find_adress(El,T,Adress,DictCP).

%	Zamiana liczb ujemnych na odpowiednikiw systemie uzupelnien do dwoch
%	oraz 4 insturkcji na liczbe
%	Wszystko dzesietnie
trans_bin([],[]):-!.
trans_bin([num(H)|T],[New|Exit]):-
	H < 0,!,New is  32768 + (32768 + H),
	trans_bin(T,Exit).
trans_bin([num(H)|T],[H|Exit]):-!,
	trans_bin(T,Exit).
trans_bin([A,B,C,D|T],[New|Exit]):-
	trans_rozkaz(A,A1),
	trans_rozkaz(B,B1),
	trans_rozkaz(C,C1),
	trans_rozkaz(D,D1),
	New is A1*16*16*16+B1*16*16+C1*16+D1,
	trans_bin(T,Exit).
	
trans_rozkaz(nop,0):-!.
trans_rozkaz(syscall,1):-!.
trans_rozkaz(load,2):-!.
trans_rozkaz(store,3):-!.
trans_rozkaz(swapa,4):-!.
trans_rozkaz(swapd,5):-!.
trans_rozkaz(branchz,6):-!.
trans_rozkaz(branchn,7):-!.
trans_rozkaz(jump,8):-!.
trans_rozkaz(const,9):-!.
trans_rozkaz(add,10):-!.
trans_rozkaz(sub,11):-!.
trans_rozkaz(mul,12):-!.
trans_rozkaz(div,13):-!.
trans_rozkaz(shift,14):-!. 
	
% Predykaty uzywkowe ktore nie sa czescia zadania
%	Bardzo wygony jest predykat main()
%	Wystarczy pisac nazwe pliku z programem algol16
%	A wynik compilacji zostanie binarnie zapisany w 'result'


main(FileName):-!,
	readfile(FileName, CharCodeList),
	myparse(CharCodeList, TreeP),
	assembly(TreeP,ASM),
	printHex(ASM,Exit),
	writefile('result',Exit),!.

writefile(FileName,List):-
	open(FileName,write,Stream),
	loop(Stream,List),
	close(Stream).

loop(_,[]):-!.
loop(Stream,[H|T]):-
	write(Stream,H),
	write(Stream,' '),
	loop(Stream, T).

writefile2(FileName,List):-
	open(FileName,write,Stream),
	loop2(Stream,List),
	close(Stream).

loop2(_,[]):-!.
loop2(Stream,[num(A)|T]):-!,
	write(Stream,num(A)),
	write(Stream,'\n'),
	loop2(Stream, T).
loop2(Stream,[A,B,C,D|T]):-
	loop(Stream,[A,B,C,D]),
	write(Stream,'\n'),
	loop2(Stream,T).
	
readfile(FileName, List) :-
	open(FileName, read, Stream),
	createlist(Stream,List),
	close(Stream).

createlist(Stream, T):-
	get_code(Stream, H),
	coss(H,T,Stream).
coss(H,T,_):-
	H = -1,!,
	T = [].
coss(H, [H|T],Stream):-
	createlist(Stream,T).
