
#ifndef _LEXING_H_
#define _LEXING_H_

#include "core.h"
#include "stdio.h"

namespace Lexing {
using Core;
using Stdio;

extern xenum exn { Error(string) };

extern struct lexbuf<`a> { /* use 'a for state that refill_buff might need */
 
  void   (@refill_buff)(struct lexbuf<`a> @);
  `a     refill_state;

  string lex_buffer;
  int    lex_buffer_len;
  int    lex_abs_pos;
  int    lex_start_pos;
  int    lex_curr_pos;
  int    lex_last_pos;
  int    lex_last_action;
  bool   lex_eof_reached;
};
typedef struct lexbuf<`a> @Lexbuf<`a>;

extern struct function_lexbuf_state<`b> { 
// instantiation for using function to read
  int (@read_fun)(string,int,`b);
  `b read_fun_state;
};
typedef struct function_lexbuf_state<`b> @Function_lexbuf_state<`b>;

extern struct lex_tables { 
// for space this should probably be shorts, but who cares
  int ?lex_base;
  int ?lex_backtrk;
  int ?lex_default;
  int ?lex_trans;
  int ?lex_check;
};
typedef struct lex_tables @LexTables;


extern Lexbuf<Function_lexbuf_state<`b>>
  from_function<`b>(int read_fun(string,int,`b), `b);

extern Lexbuf<Function_lexbuf_state<FILE@>> from_file(FILE@);
extern Lexbuf<bool> from_string(string);

extern string lexeme      <`a>(Lexbuf<`a>);
extern char   lexeme_char <`a>(Lexbuf<`a>, int);
extern int    lexeme_start<`a>(Lexbuf<`a>);
extern int    lexeme_end  <`a>(Lexbuf<`a>);

extern int lex_engine<`a>(LexTables,int,Lexbuf<`a>);

}

#endif
