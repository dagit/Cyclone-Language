(***********************************************************************)
(*                                                                     *)
(*                           Objective Caml                            *)
(*                                                                     *)
(*            Xavier Leroy, projet Cristal, INRIA Rocquencourt         *)
(*                                                                     *)
(*  Copyright 1996 Institut National de Recherche en Informatique et   *)
(*  en Automatique.  All rights reserved.  This file is distributed    *)
(*  under the terms of the Q Public License version 1.0.               *)
(*                                                                     *)
(***********************************************************************)

(* $Id: main.ml,v 1.2 2000-11-06 16:41:31 danieljg Exp $ *)

(* The lexer generator. Command-line parsing. *)

open Syntax
open Lexgen
open Output

let main () =
  let arglen = Array.length Sys.argv in
  if arglen < 2 || arglen > 3 then begin
    prerr_endline "Usage: cycocamllex <input file> [<output file>]";
    exit 2
  end;
  let source_name = Sys.argv.(1) in
  let dest_name =
    if arglen == 3
    then Sys.argv.(2)
    else if Filename.check_suffix source_name ".cyl" then
      Filename.chop_suffix source_name ".cyl" ^ ".cyc"
    else
      source_name ^ ".cyc" in
  let ic = open_in_bin source_name in
  let oc = open_out dest_name in
  let lexbuf = Lexing.from_channel ic in
  try
    let def = Parser.lexer_definition Lexer.main lexbuf in
    let (entries, transitions) = Lexgen.make_dfa def in
    let tables = Compact.compact_tables transitions in
    Output.output_lexdef source_name ic oc
                         def.header tables entries def.trailer;
    close_in ic;
    close_out oc
  with exn ->
    close_in ic;
    close_out oc;
    Sys.remove dest_name;
    begin match exn with
      Parsing.Parse_error ->
        Printf.fprintf stderr
          "File \"%s\", line %d, character %d: syntax error.\n"
          source_name !Lexer.line_num
          (Lexing.lexeme_start lexbuf - !Lexer.line_start_pos)
    | Lexer.Lexical_error(msg, line, col) ->
        Printf.fprintf stderr
          "File \"%s\", line %d, character %d: %s.\n"
          source_name line col msg
    | Output.Table_overflow ->
        Printf.fprintf stderr
          "File \"%s\":\ntransition table overflow, automaton is too big\n"
          source_name
    | _ ->
        raise exn
    end;
    exit 3

let _ = Printexc.catch main (); exit 0

