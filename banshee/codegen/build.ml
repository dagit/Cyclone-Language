(*
 * Copyright (c) 2000-2004
 *      The Regents of the University of California.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 *)

exception Fail of string
let _ = 
  let filename = try (Array.get Sys.argv 1) with
  | _ -> print_string "Usage : banshee <filename> [outfile] "; exit 1 in
  let outfile = ( try (Array.get Sys.argv 2) with
  | _ ->  Filename.chop_extension filename) in
(*  let _ =  
    if (Filename.check_suffix filename ".spec") 
    then ()
    else begin 
      print_string "Specification file must end with .spec"; 
      exit 1
    end in
*)
  let h_file = outfile ^ ".h" and c_file = outfile ^ ".c" in
  let input_file = try (open_in filename) with | _ -> 
    print_string "Error opening specification file"; exit 1 in 
  let lexbuf = Lexing.from_channel input_file in
  let spec = Parser.engspec Lexer.token lexbuf in
  (Spec_to_c.spec_to_c spec h_file c_file; flush stdout)
 
