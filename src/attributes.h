/* Functions concerning attributes
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
#ifndef _ATTRIBUTES_H_
#define _ATTRIBUTES_H_
#include "absyn.h"
namespace Atts {
  using Absyn;
// does the function (or fn pointer) type have the "noreturn" attribute?
bool is_noreturn_fn_type(type_t);
// are the lists of attributes the same?  doesn't require the same order
bool same_atts(attributes_t, attributes_t);
// are the lists of attributes equivalent? regparm(0) is optional, since this is the gcc default
// but if the attribute -mregparm=n is used to override the default then this is not sound
// places in the linux kernel are lax with this attribute; need this to compile some headers
bool equiv_fn_atts(attributes_t, attributes_t);

// Transfer any function type attributes from the given list to the
// function type.  
attributes_t transfer_fn_type_atts(type_t, attributes_t);

// Returns true when first attribute list a1 can be safely cast to second
bool sub_attributes(attributes_t, attributes_t);

// returns true when a is an attribute for function types
bool fntype_att(attribute_t);

void check_fndecl_atts(seg_t,attributes_t,bool is_defn);
void check_variable_atts(seg_t, vardecl_t, attributes_t);
void check_field_atts(seg_t, stringptr_t<`H> fn, attributes_t);

void fnTypeAttsOK(seg_t, type_t);

// returns true when a1 is equal to a2
bool equal_att(attribute_t, attribute_t);
// like strcmp but for attributes
int attribute_cmp(attribute_t,attribute_t);

// Merge two attribute lists into a freshly-allocated list.  
attributes_t merge_attributes(attributes_t, attributes_t);

attributes_t atts2c(attributes_t);
}

#endif
