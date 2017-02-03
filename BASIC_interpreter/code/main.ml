(*  Projekt na przedmiot "Programowanie Funkcyjne" BASIC - interpreter
    Kacper Kulczak 279079
    year 2017  *)

(***************cd *********************************************)
(*          MAIN                                            *)
(************************************************************)
open Types;;
open Lexer;;
open Parser;;
open Evaluation;;
open Execution;;

let my_read_line channel = try Some (input_line channel) with End_of_file -> None;;

(* Funkcja inicjująca. Zwraca stuture programu gotowa do interpretacji. (parsowanie) *)
let init_program file =
  let ic = open_in file in
  let imp_labels = ref [] in
  let rec aux n  =
    match my_read_line ic with
    | Some str ->
      let parsed_line = parse_line (init_lex str) in
      imp_labels := add_var parsed_line.label n (!imp_labels) ;
      ( {cmd = parsed_line.cmd} ):: (aux (n+1))
    | None -> [] in
  let arr = Array.of_list (aux 0) in
  { current = 0 ; length = Array.length arr ; code = arr;
    vars = []; labels =  !imp_labels  };;

let main () =
  let args = Array.length Sys.argv in
  if args <> 2
  then print_endline "Nie podano programu do interpretowania"
  else
    let prog = init_program Sys.argv.(1) in
    run prog;;
main ();;
