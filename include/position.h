#ifndef _POSITION_H_
#define _POSITION_H_
// operations for tracking positions (locations) within source code
#include "core.h"
#include "list.h"
#include "lineno.h"

namespace Position {
  using Core;
  using List;
  using Lineno;

  extern xtunion exn {extern Exit};

  extern void   reset_position(string);
  extern void   set_position_file(string);
  extern string get_position_file();

  //////// Location Tracking /////////
  extern struct Segment;
  typedef struct Segment *seg_t; // same as segment, but better emacs coloring
  extern seg_t  segment_of_abs(int,int);
  extern seg_t  segment_join(seg_t,seg_t);
  extern string string_of_loc(int);
  extern string string_of_segment(seg_t);
  extern list_t<stringptr> strings_of_segments(glist_t<seg_t,`r>);

  //////// Errors ////////////////////
  extern tunion Error_kind { Lex, Parse, Elab };
  typedef tunion Error_kind error_kind_t;
  extern struct Error {
    string       source;
    seg_t        seg;
    error_kind_t kind;
    string       desc;
  };
  typedef struct Error @error_t;
  extern error_t mk_err_lex(seg_t,string);
  extern error_t mk_err_parse(seg_t,string);
  extern error_t mk_err_elab(seg_t,string);

  //////// Error Reporting ///////////
  extern xtunion exn {extern Nocontext};
  extern bool print_context;
  extern void post_error(error_t);
  extern bool error_p();
}
#endif 
