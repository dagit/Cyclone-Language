#ifndef OUTPUT_H
#define OUTPUT_H

#include "core.h"
#include "stdio.h"
#include "list.h"
#include "syntax.h"
#include "lexgen.h"
#include "compact.h"

namespace Output {
using Stdio;

extern void output_lexdef(string sourcename, 
			  FILE @ infile, 
			  FILE @ outfile,
			  Syntax::location_t    header,
			  Compact::lex_tables_t tables,
			  List::list_t<Lexgen::automata_entry_t> entry_points,
			  Syntax::location_t    trailer);

extern xenum exn { Table_overflow };

}
#endif
