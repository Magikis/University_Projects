(*  Projekt na przedmiot "Programowanie Funkcyjne" BASIC - interpreter
    Kacper Kulczak 279079
    year 2017  *)
(************************************************************)
(************************************************************)
(*                      EWALUACJA                              *)
(************************************************************)
(************************************************************)
open Types;;
exception EvalError;;

(* Wylicz wartosc zmiennej *)
let get_var v vars =   List.assoc v vars ;;

(*  Dodaj/zmien wartosc zmiennej*)
let rec add_var var value vars = match vars with
  | (k,v)::xs ->
    if k = var
    then (var,value)::xs
    else (k,v)::(add_var var value xs)
  | [] -> (var,value)::[]

(* Podaj indeks etykeity skoku *)
let get_label = get_var ;;

(* Obliczanie wartosci wyrazenia *)
let rec eval_exp vars exp = match exp with
  | ExpInt num -> Vint num
  | ExpStr str -> Vstring str
  | ExpVar v -> Vint (get_var v vars)
  | ExpBin (exp1, op, exp2)->
    (
      match eval_exp vars exp1 , eval_exp vars exp2 with
      | Vint e1, Vint e2 ->
        (
          match op with
          | PLUS -> Vint(e1 + e2)
          | MINUS -> Vint(e1 - e2)
          | MULT -> Vint(e1 *e2)
          | DIV ->  Vint(e1 / e2)
          | MOD -> Vint(e1 mod e2)
          | LS -> Vbool(e1 < e2)
          | LSEQ -> Vbool(e1 <= e2)
          | GT -> Vbool(e1 > e2)
          | GTEQ -> Vbool(e1 >= e2)
          | EQ -> Vbool(e1 = e2)
          | NEQ -> Vbool(e1 <> e2)
          | _ -> raise EvalError
        )
      | Vbool e1, Vbool e2 ->
        (
          match op with
          | AND -> Vbool(e1 && e2)
          | OR -> Vbool(e1 || e2)
          | _ -> raise EvalError
        )
      | _ -> raise EvalError
    )
  | ExpUnr (op,exp1) -> (match eval_exp vars exp1 with
      | Vbool e1 -> Vbool (not e1 )
      | Vint e1 -> Vint (- e1)
      | _ -> raise EvalError
    );;
