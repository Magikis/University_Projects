(*  Projekt na przedmiot "Programowanie Funkcyjne" BASIC - interpreter
    Kacper Kulczak 279079
    year 2017  *)
(************************************************************)
(*          LEXER                                            *)
(************************************************************)
open Types;;
exception LexerError;;

(* Załadowanie stringa (linijki kodu) do struktury *)
let init_lex s =   {str = s ; current = 0 ; size = String.length s}  ;;

(* funkcje imperatyrwe: Chodzenie po linijce kodu *)
let foward (s: string_lexer) = s.current <- s.current +1;;
let foward_n (s :string_lexer) n = s.current <- s.current+n;;

let is_int = function
  | '0'..'9' -> true
  | _ ->  false;;
let is_string = function
  | '0'..'9' | 'a'..'z' | 'A'..'Z' -> true
  | _ -> false;;

(* rozpakowuje stringa, sprawdzajac zakonczenie za pomoca funkcji boolowskiej [check_fun] *)
let extract check_fun prog =
  let str = prog.str and pos = prog.current in
  let rec ext n = if (n < prog.size) && (check_fun str.[n]) then ext (n+1) else n in
  let last = ext pos
  in prog.current <- last ; String.sub str pos (last-pos);;

(*Dodtakowe przesuniecie w celu wyjęcia ostatnioego '"' z tokena *)
let extract_for_string f line =
  let str = extract f line
  in
  foward line;
  str;;


(* Funkcja ktora ze struktory linijki kodu wyciaga jeden token i
   przestawia wskaznik miejsca w strukturze na nastepny token*)
let rec lexer ( line : string_lexer ) =
  let  lexer_char c =
    match c with
    | ' ' | '\t' | '\n' -> foward line; lexer line
    | '+' | '-' | '*' | '/' | '%' | '(' | ')' | '=' | '&' | '|' | '!' -> foward line; Toksym (String.make 1 c)
    (*Sprytne rozpatrywanie <= itp. *)
    | '<' | '>' ->
      foward line;
      (match c , line.str.[line.current] with
       | '<', '>' -> foward line; Toksym "<>"
       | '<', '=' -> foward line; Toksym "<="
       | '>', '=' -> foward line; Toksym ">="
       | _ -> Toksym (String.make 1 c))
    | '_' -> foward line; Toklabel ((String.make 1 c) ^(extract is_string  line))
    | '0'..'9' ->  Tokint (int_of_string @@ extract is_int line)
    | 'a'..'z' | 'A'..'Z' ->  Tokident (extract is_string line)
    | '"' -> foward line; Tokstring (extract_for_string (function x -> x <> '"') line)
    | _ -> raise LexerError
  in
  if line.current >= line.size
  then  Tokend
  else  lexer_char line.str.[line.current]
