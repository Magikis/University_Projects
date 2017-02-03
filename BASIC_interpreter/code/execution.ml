(*  Projekt na przedmiot "Programowanie Funkcyjne" BASIC - interpreter
    Kacper Kulczak 279079
    year 2017  *)
open Types;;
open Evaluation;;
exception RunError of int;;

(* Przejdz do nastepnej linijki w programie *)
let goto_nextline (prog:program) =
  if prog.current + 1 = prog.length
  then prog.current <- (-1)
  else prog.current <- (prog.current +1)

let goto_previusline prog =
  prog.current <- (prog.current - 1)

(* Skocz do linijki *)
let goto_label prog label =
  try
    prog.current <- get_label label prog.labels
  with
  | Not_found -> prog.current <- -1


(* Skocz do poczatku petli *)
(* dziala dla zagniezdzonych petli, odkladajac liczbe minietych linijek DONE  *)
let rec find_loop_start (prog : program ) n = match prog.code.(prog.current).cmd with
  | While _ | For _->
    if n = 0
    then ()
    else (goto_previusline prog; find_loop_start prog (n-1) )
  | Done -> goto_previusline prog; find_loop_start prog (n+1)
  | _ -> goto_previusline prog; find_loop_start prog n

(* Skocz na koniec petli. Podobnie jak powyzej  *)
let rec find_loop_end prog n = match prog.code.(prog.current).cmd with
  | Done ->
    if n = 0
    then ()
    else (goto_nextline prog; find_loop_end prog (n-1) )
  | While _ | For _ -> goto_nextline prog; find_loop_end prog (n+1)
  | _ -> goto_nextline prog; find_loop_end prog n

(* Wykonaj instrukcje PRINT  *)
let execute_print value = match value with
  | Vbool v -> if v then print_endline "true" else print_endline "false"
  | Vstring s -> print_endline s
  | Vint num -> print_endline (string_of_int num)

(* Wykonaj instrukcje INPUT *)
let rec execute_input var vars =
  print_string "?: ";
  try
    add_var var (read_int ()) vars
  with
  | _ ->
    print_endline "Nie wpisano liczby.";
    execute_input var vars

(*Wykonuje komende, na ktora wskazuje pole prog.current*)
let run_cmd prog cmd =
  match cmd.cmd with
  | Goto str -> goto_label prog str
  | If (exp, str) ->
    (
    match eval_exp prog.vars exp with
    | Vbool b -> if b then goto_label prog str else goto_nextline prog
    | _ -> raise (RunError prog.current)
    )
  | Rem -> goto_nextline prog
  | Let (var,exp) ->
    (
    match eval_exp prog.vars exp with
    | Vint num -> prog.vars <- add_var var num prog.vars; goto_nextline prog
    | _ -> raise (RunError prog.current)
    )
  | Print exp -> execute_print (eval_exp  prog.vars exp); goto_nextline prog
  | Input str -> prog.vars <- execute_input str prog.vars; goto_nextline prog
  | While exp ->
    (match eval_exp prog.vars exp with
     | Vbool b ->
       if b
       then goto_nextline prog
       else (goto_nextline prog; find_loop_end prog 0 ; goto_nextline prog)
     | _ -> raise (RunError prog.current)
    )
  | Done -> goto_previusline prog;find_loop_start prog 0
  | For (var,exp1,exp2)->
    let evaled_var =
      try
        let n = (get_var var prog.vars) in
        prog.vars <- add_var var (n+1) prog.vars;
        n+1
      with
      | Not_found -> (
        (match eval_exp prog.vars exp1 with
        | Vint num -> prog.vars <- add_var var num prog.vars
        | _ -> raise (RunError prog.current)
        );
        get_var var prog.vars)  in
    match eval_exp prog.vars exp2 with
    | Vint num ->
      (
        if (evaled_var <= num )
      then goto_nextline prog
      else
        (
        goto_nextline prog;
        (
          match eval_exp prog.vars exp1 with
          | Vint num -> prog.vars <- add_var var (num-1) prog.vars
          | _ -> raise (RunError prog.current)
        );
        find_loop_end prog 0 ;
        goto_nextline prog
        )
      )
    | _ -> raise (RunError prog.current)

(* Uruchamia program, zakonczenie po Wyjsciu poza tablice komend programu *)
let rec run prog =
  if prog.current = -1
  then (print_endline "Program Exits"; exit 0)
  else
    (
    run_cmd prog (prog.code.(prog.current));
    run prog
    );;
