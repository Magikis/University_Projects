(*  Projekt na przedmiot "Programowanie Funkcyjne" BASIC - interpreter
    Kacper Kulczak 279079
    year 2017  *)
(******************************)
        (* Types *)
(******************************)
type tokens =
  | Tokint of int
  | Tokstring of string
  | Toksym of string
  | Tokident of string
  | Toklabel of string
  | Tokend
  (*Linijka programu w BASIC gotowa do zczytywania tokenow*)
type string_lexer = {str : string ; mutable current : int ; size : int }


(******************************)
(*********** Parser ***********)
(******************************)
type unr_op = UNMINUS | NEG;;
type bin_op =
  | PLUS | MINUS | MULT | DIV | MOD
  | LS  | LSEQ  | GT | GTEQ  | EQ  | NEQ | AND | OR;;

type expression =
   | ExpInt of int
   | ExpVar of string
   | ExpStr of string
   | ExpUnr of unr_op * expression
   | ExpBin of expression * bin_op * expression;;

type exp_elem =
     Texp of expression
   | Tbin of bin_op
   | Tunr of unr_op
   | Tlp;;

type command =
      | Goto of string
      | If of expression * string
      | Rem
      | Let of string * expression
      | Print of expression
      | Input of string
      | While of expression
      | For of string * expression * expression
      | Done ;;
(*Sparsowana linia programu (etykietka skoku, komenda wykonywana)*)
type line = {label : string ; cmd : command};;


(******************************)
        (*Ewaluacja*)
(******************************)
type eval_line = {cmd:command}
type code = eval_line array
type variables =  (string * int) list
type labels = (string * int) list
(*Stan programu*)
type program = {mutable current : int ; length:int; code : code;
                mutable vars : variables; labels : labels  }
type value =
  |  Vint of int
  |  Vstring of string
  |  Vbool of bool
