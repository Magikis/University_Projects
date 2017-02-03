(*  Projekt na przedmiot "Programowanie Funkcyjne" BASIC - interpreter
    Kacper Kulczak 279079
    year 2017  *)
(* Progeaming temporary functions *)
open Types;;

let print_vars xs =
  let rec aux = function
    | [] -> ()
    | x::xs ->
      let (a,b) = x in
      print_string ( "(" ^ a ^ "," ^ (string_of_int b) ^ ") ; " ) ;
      aux xs
  in
  print_string "[ ";
  aux xs;
  print_string "]\n";;

  let rec debug prog =
    match read_line () with
    | _ -> print_endline ("PC:" ^ (string_of_int (prog.current+1) )) ;;
