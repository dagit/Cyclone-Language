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

{
open Parser
let find_exprid' s = 
  let from = String.index s '(' + 1 in
  let to_ = String.index s ')' - from in 
  String.sub s from to_
} 

let ident = ['A'-'Z' 'a'-'z'] (['A'-'Z' 'a'-'z' '0'-'9' '_'] *)
rule token = parse
  [' ' '\t' '\n' '\r']          { token lexbuf }     (* skip blanks *)
| eof                           { EOF }  
| "specification"               { SPECIFICATION }
| "spec"                        { SPEC }
| "end"                         { END }
| "data"                        { DATA }
| "of"                          { OF }
| "and"                         { AND }
| "row" '(' ident ')'           { SORT((new Row_sort.row_sort_gen 
					  (find_exprid' 
					     (Lexing.lexeme lexbuf)))
					 :> Engspec.sort_gen) }
| "set"                         { SORT(Set_sort.setsort) }
| "setIF"                       { SORT(Set_sort.setsort) }
| "setST"                       { SORT(Setst_sort.setstsort) }
| "term"                        { SORT(Term_sort.termsort) } 
| '+'                           { POS }
| '-'                           { NEG }
| '|'                           { OR }
| ':'                           { COLON }
| '='                           { EQ }
| '('                           { LPAREN }
| ')'                           { RPAREN }
| '*'                           { CROSS }
| '%'                           { comment lexbuf }
| ident                         { IDENT(Lexing.lexeme lexbuf) }  
and comment = parse
  ['\n']              { token lexbuf }
| eof                           { EOF }
| [^'\n']*            { comment lexbuf }
