#ifndef OUTPUT_H
#define OUTPUT_H

#include "core.h"
#include "stdio.h"
#include "list.h"
#include "syntax.h"
#include "lexgen.h"
#include "compact.h"

namespace Output {

extern void output_lexdef(string_t sourcename, 
			  FILE @ infile, 
			  FILE @ outfile,
			  Syntax::location_t    header,
			  Compact::lex_tables_t tables,
			  List::list_t<Lexgen::automata_entry_t> entry_points,
			  Syntax::location_t    trailer);

extern datatype exn { extern Table_overflow(int) };

}
#endif
