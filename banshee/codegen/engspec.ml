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

type strid = string
type sigid = string
type exprid = string
type conid = string
      
type variance = NEGvariance | NOvariance | POSvariance
type bconsig = exprid * variance
      
type consig = bconsig list 
type databody = (conid * consig option) list
   
exception Fixed_sort of string

class type sort_gen = 
  object
    method get_name : string
    method gen_sort_ops : Cgen.file -> Cgen.header -> exprid -> unit
    method gen_con_ops : Cgen.file -> Cgen.header -> exprid * databody -> unit
    method init : exprid -> Cgen.statement list
    method reset : exprid -> Cgen.statement list
  end

class virtual fixed_sort_gen = 
  object (this)
    method private error e = "Sort " ^ e ^ " does not permit constructors."
    method virtual get_name : string
    method virtual gen_sort_ops : Cgen.file -> Cgen.header -> exprid -> unit
    method gen_con_ops (_ :Cgen.file) (_:Cgen.header) ((_,b): exprid * databody) = 
      match b with 
      |	[] -> ()
      |	_ ->
	(raise (Fixed_sort (this#error this#get_name)); ())
    method virtual init : exprid -> Cgen.statement list
    method virtual reset : exprid -> Cgen.statement list
  end
	        
type dataspec = (exprid * sort_gen * databody) list
type engspec = strid * sigid * dataspec list
      
