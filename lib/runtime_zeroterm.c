/* This file is part of the Cyclone Library.
   Copyright (C) 2004 Dan Grossman, AT&T

   This library is free software; you can redistribute it and/or it
   under the terms of the GNU Lesser General Public License as
   published by the Free Software Foundation; either version 2.1 of
   the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public
   License along with this library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place, Suite
   330, Boston, MA 02111-1307 USA. */

#if defined(__ARMEL__) || defined(__FreeBSD__)
/* There's no stdint.h on arm-elf for Gameboy -- same for FreeBSD */
typedef char      int8_t;
typedef short     int16_t;
typedef int       int32_t;
typedef long long int64_t;
#else
#include <stdint.h>
#endif
#include "runtime_internal.h"

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
// FIX: throws a NULL exception if we don't support a type-size, but
//      that is very uninformative as to what actually happened.
// Note: This used to be more portable, but long-winded.  Now assumes
//  sizeof(int)=sizeof(float)=sizeof(void*) and all three zero's are the
//  same.  Even the old way is a hack because we don't support structs, etc.
#ifndef NO_CYC_BOUNDS_CHECKS
#define DO_PLUS_FN(t)\
  {t * __orig_x = (t*)(orig_x);\
  unsigned int _czs_temp;\
  if ((__orig_x) == 0) _throw_null_fn(filename,lineno);\
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);\
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)\
    if (__orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);\
  return __orig_x + orig_i;}

char* _zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,
			      const char *filename, unsigned lineno) {
  DO_PLUS_FN(char);
}
void* _zero_arr_plus_other_fn(unsigned t,void* orig_x,unsigned orig_sz,int orig_i,const char*filename,unsigned lineno) {
  switch(t) {
  case 1: DO_PLUS_FN(int8_t);
  case 2: DO_PLUS_FN(int16_t);
  case 4: DO_PLUS_FN(int32_t);
  case 8: DO_PLUS_FN(int64_t);
  default: 
    if(t==sizeof(long double))
      DO_PLUS_FN(long double);
    _throw_null_fn("runtime_zeroterm.c",__LINE__);
  }
}
#endif

/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
#define DO_GET_SIZE(t)\
  {const t *_gres_x = (const t *)orig_x;\
  unsigned _gres = 0;\
  if (_gres_x != 0) {\
     _gres = orig_offset;\
     _gres_x += orig_offset - 1;\
     while (*_gres_x != 0) { _gres_x++; _gres++; }\
  }\
  return _gres;}

unsigned _get_zero_arr_size_char(const char *orig_x, unsigned orig_offset) {
  DO_GET_SIZE(char)
}
unsigned _get_zero_arr_size_other(unsigned t,const void* orig_x,unsigned orig_offset) {
  switch(t) {
  case 1: DO_GET_SIZE(int8_t);
  case 2: DO_GET_SIZE(int16_t);
  case 4: DO_GET_SIZE(int32_t);
  case 8: DO_GET_SIZE(int64_t);
  default: 
    if(t==sizeof(long double))
      DO_GET_SIZE(long double);
    _throw_null_fn("runtime_zeroterm.c",__LINE__);
  }
}

/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus_<type>_fn. */
char* _zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
void* _zero_arr_inplace_plus_other_fn(unsigned t, void **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_other_fn(t,*x,1,orig_i,filename,lineno);
  return *x;
}

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
char* _zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  char * _zap_res = *x;
  *x = _zero_arr_plus_char_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
void* _zero_arr_inplace_plus_post_voidstar_fn(unsigned t,void **x, int orig_i,const char *filename,unsigned lineno){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_other_fn(t,_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
struct _fat_ptr _fat_ptr_decrease_size(struct _fat_ptr x,
				       unsigned int sz, unsigned int numelts) {
  unsigned delta = sz * numelts;
  /* Don't let someone decrease the size so much that it wraps around.
   * This is crucial to get NULL right. */
  if (x.last_plus_one - x.base >= delta)
    x.last_plus_one -= delta;
  else x.last_plus_one = x.base;
  return x; 
}
