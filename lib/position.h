#ifndef POSITION_H
#define POSITION_H
// operations for tracking positions (locations) within source code
#include "core.h"
#include "list.h"
#include "lineno.h"

namespace Position {
  using Core;
  using List;
  using Lineno;

  extern xenum exn {Exit};

  extern void   reset_position(string);
  extern void   set_position_file(string);
  extern string get_position_file();

  //////// Location Tracking /////////
  extern struct Segment;
  typedef struct Segment *segment;
  extern struct Segment dummy_seg;
  extern segment segment_of_abs(int,int);
  extern segment segment_join(segment,segment);
  extern string string_of_loc(int);
  extern string string_of_segment(segment);

  //////// Errors ////////////////////
  extern enum Error_kind { Lex, Parse, Elab };
  typedef enum Error_kind error_kind;
  extern struct Error {
    string     source;
    segment    seg;
    error_kind kind;
    string     desc;
  };
  typedef struct Error @error;
  extern error mk_err_lex(segment,string);
  extern error mk_err_parse(segment,string);
  extern error mk_err_elab(segment,string);

  //////// Error Reporting ///////////
  extern xenum exn {Nocontext};
  extern bool print_context;
  extern void post_error(error);
  extern bool error_p();
}
#endif 
