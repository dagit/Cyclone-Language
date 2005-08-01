/* Compiler flags
   Copyright (C) 2004 Dan Grossman, AT&T
   This file is part of the Cyclone compiler.

   The Cyclone compiler is free software; you can redistribute it
   and/or modify it under the terms of the GNU General Public License
   as published by the Free Software Foundation; either version 2 of
   the License, or (at your option) any later version.

   The Cyclone compiler is distributed in the hope that it will be
   useful, but WITHOUT ANY WARRANTY; without even the implied warranty
   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with the Cyclone compiler; see the file COPYING. If not,
   write to the Free Software Foundation, Inc., 59 Temple Place -
   Suite 330, Boston, MA 02111-1307, USA. */
#ifndef _FLAGS_H_
#define _FLAGS_H_
#include <core.h>
namespace Flags {

  // print diagnostic information during compilation including
  // results of insert_checks.
extern bool verbose;

  // true for we're running the porting tool (not compiling Cyclone),
  // so use code in port.cyc and have parser include more location info
extern bool porting_c_code;

  // true for generate code that does not use regions (just stack or
  // heap allocates everything)
extern bool no_regions;

extern bool no_reaps;

  // if true, the parser ignores the "register" scope, simply translating
  // it as public (the default)
extern bool no_register;

  // true for printing a warning when a declaration shadows an outer binding
extern bool warn_override;

  // true for getting more warnings from the type-checker:
  // * read-only union fields (becasuse untagged and not-bits-only)
  // * assignments in tests (often means = should be ==)
  // * unnecessary (and meaningless) dereferences of function pointers
extern bool tc_aggressive_warn;

  // true for printing a warning when an alias coercion is implicitly
  // introduced (when coercing a function argument)
extern bool warn_alias_coerce;

  // true for printing a warning when a region-outlives relationship
  // is used for implicitly coercing an expression
extern bool warn_region_coerce;

  // true for printing when we lose a unique pointer
extern bool warn_lose_unique;

  // true for printing a warning when a bounds-check (for a subscript)
  // is inserted into the generated C code
extern bool warn_bounds_checks; 

  // true for printing a warning when a null-check (for a dereference)
  // is inserted into the generated C code
extern bool warn_all_null_deref;

  // true for do not expand typedefs to their definitions when generating
  // C code
extern bool noexpand_r;

  // When set to true, causes vcgen to look at all paths in functions
  // unless overriden by the __cyclone_pragma__(one_path).
  // The default is false.
extern bool allpaths;

  // Used to print out more verbose information in assertions.
extern bool debug_assn;

  // Used to print out unprovable assertions
extern bool print_unprovable;

  // true for producing output compatible with MSVC and not using
  // gcc extensions (outdated; surely does NOT work); corresponds
  // mostly to inserting the compiler pass defined in tovc.cyc.
  // the next flag is similar (FIX: use these consistently)
extern bool tovc_r;
enum C_Compilers {
  Gcc_c,  // gcc (default)
  Vc_c    // visual c (surely does NOT work anymore)
};
extern enum C_Compilers c_compiler;

  // Note: this is just for "stopping early". There are several
  // other issues for passes:
  //  * do we treeshake
  //  * do we generate interfaces
  //  * what printer do we use (or do we not print at all)
  //  * do we call the linker
  //  * do we skip vcgen
  //  * do we skip toseqc
enum Cyclone_Passes {
  Cpp,
  Parsing,
  Binding,
  CurrentRegion,
  TypeChecking,
  Jumps,
  FlowAnalysis,
  VCGen,
  CheckInsertion,
  Toc,
  AggregateRemoval,
  LabelRemoval,
  TempRename,
  CastRemoval,
  EvalOrder,
  CCompiler,
  AllPasses // must be last -- means do everything (the default)
};
extern enum Cyclone_Passes stop_after_pass;

void set_cpponly();
void set_parseonly();
void set_crgnonly();
void set_tconly();
void set_cfonly();
void set_toconly(); // somewhat misnamed, just doesn't call C compiler!

  // true for building the bootstrapped compiler; lexer strips out
  // directory-paths from #line directives so the repository doesn't
  // end up referring to the compiler-hacker's working directory.
  // Also effects how we #include setjmp.h and include libraries
extern bool compile_for_boot;

  // when set, we just print out the state and token for a parse
  // error and then exit immediately.  Used for generating the message table.
extern bool print_parser_state_and_token;

  // controls the maximum size of a post-condition that will be inlined by
  // vcgen without weakening.
extern unsigned int max_vc_summary;  

  // controls (roughly) how many distinct paths the constraint solver will
  // explore before giving up when trying to prove a verification condition.
extern unsigned int max_vc_paths;

  // sets every flag above with warn in its name
void set_all_warnings();
}

#endif
