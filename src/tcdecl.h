#ifndef _TCDECL_H_
#define _TCDECL_H_

#include "absyn.h"
#include "string.h"
#include "position.h"
#include "list.h"

namespace Tcdecl {
  using Absyn;
  //  using String;
  using Position;

  extern xtunion exn {extern Incompatible};

  // used by Interface
  struct Xtunionfielddecl {
    tuniondecl_t base; //no fields here
    tunionfield_t field;
  };
  typedef struct Xtunionfielddecl @ xtunionfielddecl_t;

  // if msg0 == null, don't print any error message and throw exception Incompatible
  // otherwise call terr(loc, msg) where msg is *msg0 + " " + msg1
  extern void merr(seg_t loc, string_t * `r msg0, string_t msg1);

  // all these functions work the same way :
  // _ they check compatibility between d0 (previous top-level declaration)
  //   and d1 (new top-level declaration)
  // _ if something's wrong, call merge_err and return null (or Unresolved_b for merge_binding)
  // _ don't modify d0 or d1
  // _ if d1 is included in d0 (i.e. d1 doesn't bring any new information w.r.t d0), return d0 itself
  //   hence the test for inclusion d1 <= d0 can be written : d0 == merge(d0, d1)
  // _ from a 'semantic' point of view, merge(d0, d1) = merge(d1, d0)
  // _ we assume the declarations have already been type-checked

  // t and v describe the type and the name of the object involved
  extern $(scope_t, bool) merge_scope(scope_t s0, scope_t s1,  
                                      string_t t, string_t v,
				      seg_t loc, string_t * `r msg);

  extern struct Structdecl  * merge_structdecl (structdecl_t d0,  
                                                structdecl_t d1,
						seg_t loc, 
                                                string_t * `r msg);

  extern struct Uniondecl   * merge_uniondecl  (uniondecl_t d0,   
                                                uniondecl_t d1,
						seg_t loc, 
                                                string_t * `r msg);
  extern struct Tuniondecl  * merge_tuniondecl (tuniondecl_t d0,  
                                                tuniondecl_t d1,
						seg_t loc, 
                                                string_t * `r msg);
  extern struct Enumdecl    * merge_enumdecl   (enumdecl_t d0,   
                                                enumdecl_t d1, 
						seg_t loc, 
                                                string_t * `r msg);
  extern struct Vardecl     * merge_vardecl    (vardecl_t d0,
                                                vardecl_t d1, 
						seg_t loc, 
                                                string_t * `r msg);
  extern struct Typedefdecl * merge_typedefdecl(typedefdecl_t d0, 
                                                typedefdecl_t d1,
						seg_t loc, 
                                                string_t * `r msg);

  extern binding_t            merge_binding    (binding_t d0,
                                                binding_t d1,
						seg_t loc, 
                                                string_t * `r msg);
  
  extern struct Xtunionfielddecl * 
  merge_xtunionfielddecl (xtunionfielddecl_t d0,
                          xtunionfielddecl_t d1, 
                          seg_t loc, string_t * `r msg);

  // sort the list of fields, trying to merge the duplicate
  // *res is set to false if something's wrong
  extern List::list_t<tunionfield_t> sort_xtunion_fields(List::list_t<tunionfield_t> f,
							 bool @ `r1 res,
							 var_t v, seg_t loc, string_t * `r2 msg);
}

#endif /* _TCDECL_H_ */
