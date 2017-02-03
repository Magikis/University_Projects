(*  Projekt na przedmiot "Programowanie Funkcyjne" BASIC - interpreter
    Kacper Kulczak 279079
    year 2017  *)
(************************************************************)
(************************************************************)
(*                      PARSER                              *)
(************************************************************)
(************************************************************)
open Types;;
open Lexer;;
exception ParseError;;
(*Funkcje podajace priorytety operatorow*)
let uop_priority = function
  |  UNMINUS -> 6
  |  NEG -> 1;;
let bop_priority = function
  | PLUS | MINUS ->4
  | MULT | DIV | MOD ->5
  |  LS  | LSEQ  | GT | GTEQ  | EQ  | NEQ  ->2
  | AND | OR -> 3;;

(* Konwersja tokena symbolu na faktyczny symbol *)
let unr_symb = function
   "!" -> NEG | "-" -> UNMINUS | _ -> raise ParseError;;
let bin_symb = function
   "+" -> PLUS | "-" -> MINUS | "*" -> MULT | "/" -> DIV | "%" -> MOD
 | "=" -> EQ | "<" -> LS | "<=" -> LSEQ | ">" -> GT
 | ">=" -> GTEQ | "<>" -> NEQ | "&" -> AND | "|" -> OR
 | _ -> raise ParseError;;
let tsymb s =
    try
      Tbin ( bin_symb s)
    with
    | ParseError -> Tunr (unr_symb s);;

(*redukcja stosu do kombinowania drzew wyrazen*)
(*Wyowalne z z p_op=0 po prostu redukuje stos wyrazen*)
let reduce_stack p_op stack = match stack with
  | (Texp e)::(Tunr op)::st when (uop_priority op) >= p_op
    -> (Texp (ExpUnr (op,e)))::st
  | (Texp e1)::(Tbin op)::(Texp e2)::st when (bop_priority op) >= p_op
    -> (Texp (ExpBin (e2,op,e1)))::st
  | _ -> raise ParseError

(* Wpisywanie kolejnych tokenów wyrażeń na stos i redukcja w odpowiednich momentach *)
let rec stack_or_reduce tok stack = match tok,stack with
  | Tokint n, _ ->  (Texp (ExpInt n))::stack
  | Tokstring s, _ -> (Texp (ExpStr s))::stack
  | Tokident s, _  -> (Texp (ExpVar s))::stack
  | Toksym "(",_ -> Tlp::stack
  | Toksym ")", ( Texp (e))::Tlp::stack_2  ->   (Texp e)::stack_2
  | Toksym ")",_ -> stack_or_reduce tok (reduce_stack 0 stack)
  | Toksym s,_ ->
      let symbol =
        if s = "-"
        (*rozróżnienie czy minsa na binarny i unarny*)
        then
          match stack with
              | (Texp e)::_ -> Tbin MINUS
              | _ -> Tunr UNMINUS
        else
          tsymb s
      in ( match symbol with
          | Tunr op -> symbol::stack
          | Tbin op ->
            (
              try
                stack_or_reduce tok (reduce_stack (bop_priority op) stack)
              with
              | ParseError -> symbol::stack
            )
          | _ -> raise ParseError
        )
  | _ -> raise ParseError
(* Końcowe zredukowanie stosu do jengeo elementu *)
let rec reduce_all = function
  | [] -> raise ParseError
  | [Texp x] -> x
  | stack -> reduce_all (reduce_stack 0 stack)


(* Parsowanie wyrazenie, za pomoca stack_or_reduce *)
let parse_exp (cl : string_lexer) stop=
  let last_curr = ref 0 in
  let rec parse_exp_aux stack =
    last_curr := cl.current;
    let tok = lexer cl in
    if (stop tok)
    then
      (cl.current <- !last_curr;
       reduce_all stack)
    else
      parse_exp_aux (stack_or_reduce tok stack)
  in parse_exp_aux [];;

(*Typy komend*)






(*Parosowanie lini (komendy)  *)
let parse_line cl =
  let first_word = lexer cl in
  let (label,comm_word) = match first_word with
    | Toklabel x -> (Toklabel x, lexer cl)
    | Tokident x -> (Toklabel "",Tokident x)
    | _ -> raise ParseError
  in
  let comm =
    match comm_word with
      | Tokident "REM"  ->  Rem
      | Tokident "GOTO" ->
          (match lexer cl with
          | Toklabel str -> Goto (str)
          | _ -> raise ParseError)
      | Tokident "IF" ->
        let exp = parse_exp cl ((=) (Tokident "THEN")) in
        (match ignore (lexer cl); lexer cl with
         | Toklabel s -> If (exp,s)
         | _ -> raise ParseError)
      | Tokident "LET" ->
        let tok1 = lexer cl and tok2 = lexer cl in
        (match tok1,tok2 with
         | Tokident v, Toksym "=" -> Let (v,parse_exp cl ((=) Tokend) )
         | _ -> raise ParseError )
      | Tokident "INPUT" ->
          (match lexer cl with
          | Tokident v -> Input v
          | _ -> raise ParseError)
      | Tokident "PRINT" -> Print (parse_exp cl ((=) Tokend)  )
      | Tokident "WHILE" -> While (parse_exp cl ((=) Tokend)  )
      | Tokident "DONE" -> Done
      | Tokident "FOR" ->
        let tok1 = lexer cl and tok2 = lexer cl in
        (match tok1,tok2 with
         | Tokident v, Toksym "=" ->
           (
             let exp1 = parse_exp cl ((=) (Tokident "TO")) in
             ignore (lexer cl);
             For (v,exp1, parse_exp cl ((=) Tokend))
           )
          | _ -> raise ParseError
        )
      | _ -> raise ParseError
  in
  match label with
  | Toklabel s -> {label = s ; cmd = comm}
  | _ -> raise ParseError;;
