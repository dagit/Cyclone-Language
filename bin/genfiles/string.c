#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file
   lib/runtime_cyc.c
*/
#ifndef _CYC_INCLUDE_H_
#define _CYC_INCLUDE_H_

/***********************************************************************/
/* Runtime Stack routines (runtime_stack.c).                           */
/***********************************************************************/

/* Need one of these per thread (we don't have threads)
   The runtime maintains a stack that contains either _handler_cons
   structs or _RegionHandle structs.  The tag is 0 for a handler_cons
   and 1 for a region handle.  */
struct _RuntimeStack {
  int tag; /* 0 for an exception handler, 1 for a region handle */
  struct _RuntimeStack *next;
  void (*cleanup)(struct _RuntimeStack *frame);
};

// pushes a frame on the stack
void _push_frame(struct _RuntimeStack *frame);

// pop N+1 frames from the stack (error if stack_size < N+1)
void _npop_frame(unsigned int n);

// returns top frame on the stack (NULL if stack is empty)
struct _RuntimeStack * _top_frame();

// pops off frames until a frame with the given tag is reached.  This
// frame is returned, or else NULL if none found.
struct _RuntimeStack * _pop_frame_until(int tag);

/***********************************************************************/
/* Low-level representations etc.                                      */
/***********************************************************************/

#ifdef NO_CYC_PREFIX
#define ADD_PREFIX(x) x
#else
#define ADD_PREFIX(x) Cyc_##x
#endif

#ifndef offsetof
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
#endif

/* Tagged arrays */
struct _dyneither_ptr {
  unsigned char *curr; 
  unsigned char *base; 
  unsigned char *last_plus_one; 
};  

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

/* Regions */
struct _RegionPage {
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];  /*FJS: used to be size 0, but that's forbidden in ansi c*/
};

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};

struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};

// A dynamic region is just a region handle.  We have the
// wrapper struct for type abstraction reasons.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

extern struct _RegionHandle _new_region(const char *);
extern void * _region_malloc(struct _RegionHandle *, unsigned);
extern void * _region_calloc(struct _RegionHandle *, unsigned t, unsigned n);
extern void   _free_region(struct _RegionHandle *);
extern void   _reset_region(struct _RegionHandle *);
extern struct _RegionHandle *_open_dynregion(struct _DynRegionFrame *f,
                                             struct _DynRegionHandle *h);
extern void   _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
extern void _push_handler(struct _handler_cons *);
extern void _push_region(struct _RegionHandle *);
extern void _npop_handler(int);
extern void _pop_handler();
extern void _pop_region();

#ifndef _throw
extern int _throw_null_fn(const char *filename, unsigned lineno);
extern int _throw_arraybounds_fn(const char *filename, unsigned lineno);
extern int _throw_badalloc_fn(const char *filename, unsigned lineno);
extern int _throw_match_fn(const char *filename, unsigned lineno);
extern int _throw_fn(void* e, const char *filename, unsigned lineno);
extern int _rethrow(void* e);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

extern struct _xtunion_struct *_exn_thrown;

/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

/* Built-in Run-time Checks and company */
#ifdef __APPLE__
#define _INLINE_FUNCTIONS
#endif

#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#define _INLINE_FUNCTIONS
#else
#define _INLINE inline
#endif

#ifdef VC_C
#define _CYC_U_LONG_LONG_T __int64
#else
#ifdef GCC_C
#define _CYC_U_LONG_LONG_T unsigned long long
#else
#define _CYC_U_LONG_LONG_T unsigned long long
#endif
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE void *
_check_null_fn(const void *ptr, const char *filename, unsigned lineno) {
  void*_check_null_temp = (void*)(ptr);
  if (!_check_null_temp) _throw_null_fn(filename,lineno);
  return _check_null_temp;
}
#define _check_null(p) (_check_null_fn((p),__FILE__,__LINE__))
#else
#define _check_null(ptr) \
  ({ void*_check_null_temp = (void*)(ptr); \
     if (!_check_null_temp) _throw_null(); \
     _check_null_temp; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  ((char *)ptr) + (elt_sz)*(index); })
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE char *
_check_known_subscript_null_fn(void *ptr, unsigned bound, unsigned elt_sz, unsigned index, const char *filename, unsigned lineno) {
  void*_cks_ptr = (void*)(ptr);
  unsigned _cks_bound = (bound);
  unsigned _cks_elt_sz = (elt_sz);
  unsigned _cks_index = (index);
  if (!_cks_ptr) _throw_null_fn(filename,lineno);
  if (_cks_index >= _cks_bound) _throw_arraybounds_fn(filename,lineno);
  return ((char *)_cks_ptr) + _cks_elt_sz*_cks_index;
}
#define _check_known_subscript_null(p,b,e) (_check_known_subscript_null_fn(p,b,e,__FILE__,__LINE__))
#else
#define _check_known_subscript_null(ptr,bound,elt_sz,index) ({ \
  void*_cks_ptr = (void*)(ptr); \
  unsigned _cks_bound = (bound); \
  unsigned _cks_elt_sz = (elt_sz); \
  unsigned _cks_index = (index); \
  if (!_cks_ptr) _throw_null(); \
  if (_cks_index >= _cks_bound) _throw_arraybounds(); \
  ((char *)_cks_ptr) + _cks_elt_sz*_cks_index; })
#endif
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_known_subscript_notnull(bound,index) (index)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_check_known_subscript_notnull_fn(unsigned bound,unsigned index,const char *filename,unsigned lineno) { 
  unsigned _cksnn_bound = (bound); 
  unsigned _cksnn_index = (index); 
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds_fn(filename,lineno); 
  return _cksnn_index;
}
#define _check_known_subscript_notnull(b,i) (_check_known_subscript_notnull_fn(b,i,__FILE__,__LINE__))
#else
#define _check_known_subscript_notnull(bound,index) ({ \
  unsigned _cksnn_bound = (bound); \
  unsigned _cksnn_index = (index); \
  if (_cksnn_index >= _cksnn_bound) _throw_arraybounds(); \
  _cksnn_index; })
#endif
#endif

/* Add i to zero-terminated pointer x.  Checks for x being null and
   ensures that x[0..i-1] are not 0. */
#ifdef NO_CYC_BOUNDS_CHECK
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_short_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_int_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_float_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_double_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_longdouble_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_voidstar_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#else
static _INLINE char *
_zero_arr_plus_char_fn(char *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE short *
_zero_arr_plus_short_fn(short *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE int *
_zero_arr_plus_int_fn(int *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE float *
_zero_arr_plus_float_fn(float *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE double *
_zero_arr_plus_double_fn(double *orig_x, unsigned int orig_sz, int orig_i,const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE long double *
_zero_arr_plus_longdouble_fn(long double *orig_x, unsigned int orig_sz, int orig_i, const char *filename, unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
static _INLINE void *
_zero_arr_plus_voidstar_fn(void **orig_x, unsigned int orig_sz, int orig_i,const char *filename,unsigned lineno) {
  unsigned int _czs_temp;
  if ((orig_x) == 0) _throw_null_fn(filename,lineno);
  if (orig_i < 0 || orig_sz == 0) _throw_arraybounds_fn(filename,lineno);
  for (_czs_temp=orig_sz-1; _czs_temp < orig_i; _czs_temp++)
    if (orig_x[_czs_temp] == 0) _throw_arraybounds_fn(filename,lineno);
  return orig_x + orig_i;
}
#endif

#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))


/* Calculates the number of elements in a zero-terminated, thin array.
   If non-null, the array is guaranteed to have orig_offset elements. */
static _INLINE int
_get_zero_arr_size_char(const char *orig_x, unsigned int orig_offset) {
  const char *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_short(const short *orig_x, unsigned int orig_offset) {
  const short *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_int(const int *orig_x, unsigned int orig_offset) {
  const int *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_float(const float *orig_x, unsigned int orig_offset) {
  const float *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_double(const double *orig_x, unsigned int orig_offset) {
  const double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_longdouble(const long double *orig_x, unsigned int orig_offset) {
  const long double *_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}
static _INLINE int
_get_zero_arr_size_voidstar(const void **orig_x, unsigned int orig_offset) {
  const void **_gres_x = orig_x;
  unsigned int _gres = 0;
  if (_gres_x != 0) {
     _gres = orig_offset;
     _gres_x += orig_offset - 1;
     while (*_gres_x != 0) { _gres_x++; _gres++; }
  }
  return _gres; 
}


/* Does in-place addition of a zero-terminated pointer (x += e and ++x).  
   Note that this expands to call _zero_arr_plus_<type>_fn. */
static _INLINE char *
_zero_arr_inplace_plus_char_fn(char **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_char_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_short_fn(short **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_short_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_int(int **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_int_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_float_fn(float **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_float_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_double_fn(double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_double_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_longdouble_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
static _INLINE void *
_zero_arr_inplace_plus_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno) {
  *x = _zero_arr_plus_voidstar_fn(*x,1,orig_i,filename,lineno);
  return *x;
}
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)

/* Does in-place increment of a zero-terminated pointer (e.g., x++). */
static _INLINE char *
_zero_arr_inplace_plus_post_char_fn(char **x, int orig_i,const char *filename,unsigned lineno){
  char * _zap_res = *x;
  *x = _zero_arr_plus_char_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
static _INLINE short *
_zero_arr_inplace_plus_post_short_fn(short **x, int orig_i,const char *filename,unsigned lineno){
  short * _zap_res = *x;
  *x = _zero_arr_plus_short_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
static _INLINE int *
_zero_arr_inplace_plus_post_int_fn(int **x, int orig_i,const char *filename, unsigned lineno){
  int * _zap_res = *x;
  *x = _zero_arr_plus_int_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
static _INLINE float *
_zero_arr_inplace_plus_post_float_fn(float **x, int orig_i,const char *filename, unsigned lineno){
  float * _zap_res = *x;
  *x = _zero_arr_plus_float_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
static _INLINE double *
_zero_arr_inplace_plus_post_double_fn(double **x, int orig_i,const char *filename,unsigned lineno){
  double * _zap_res = *x;
  *x = _zero_arr_plus_double_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
static _INLINE long double *
_zero_arr_inplace_plus_post_longdouble_fn(long double **x, int orig_i,const char *filename,unsigned lineno){
  long double * _zap_res = *x;
  *x = _zero_arr_plus_longdouble_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
static _INLINE void **
_zero_arr_inplace_plus_post_voidstar_fn(void ***x, int orig_i,const char *filename,unsigned lineno){
  void ** _zap_res = *x;
  *x = _zero_arr_plus_voidstar_fn(_zap_res,1,orig_i,filename,lineno);
  return _zap_res;
}
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

/* functions for dealing with dynamically sized pointers */
#ifdef NO_CYC_BOUNDS_CHECKS
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  return _cus_ans;
}
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  _cus_ans; })
#endif
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_check_dyneither_subscript_fn(struct _dyneither_ptr arr,unsigned elt_sz,unsigned index,const char *filename, unsigned lineno) {
  struct _dyneither_ptr _cus_arr = (arr);
  unsigned _cus_elt_sz = (elt_sz);
  unsigned _cus_index = (index);
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index;
  /* JGM: not needed! if (!_cus_arr.base) _throw_null(); */ 
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one)
    _throw_arraybounds_fn(filename,lineno);
  return _cus_ans;
}
#define _check_dyneither_subscript(a,s,i) \
  _check_dyneither_subscript_fn(a,s,i,__FILE__,__LINE__)
#else
#define _check_dyneither_subscript(arr,elt_sz,index) ({ \
  struct _dyneither_ptr _cus_arr = (arr); \
  unsigned _cus_elt_sz = (elt_sz); \
  unsigned _cus_index = (index); \
  unsigned char *_cus_ans = _cus_arr.curr + _cus_elt_sz * _cus_index; \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_tag_dyneither(const void *tcurr,unsigned elt_sz,unsigned num_elts) {
  struct _dyneither_ptr _tag_arr_ans;
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr);
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts);
  return _tag_arr_ans;
}
#else
#define _tag_dyneither(tcurr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _tag_arr_ans; \
  _tag_arr_ans.base = _tag_arr_ans.curr = (void*)(tcurr); \
  _tag_arr_ans.last_plus_one = _tag_arr_ans.base + (elt_sz) * (num_elts); \
  _tag_arr_ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr *
_init_dyneither_ptr(struct _dyneither_ptr *arr_ptr,
                    void *arr, unsigned elt_sz, unsigned num_elts) {
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr);
  void* _itarr = (arr);
  _itarr_ptr->base = _itarr_ptr->curr = _itarr;
  _itarr_ptr->last_plus_one = ((unsigned char *)_itarr) + (elt_sz) * (num_elts);
  return _itarr_ptr;
}
#else
#define _init_dyneither_ptr(arr_ptr,arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr *_itarr_ptr = (arr_ptr); \
  void* _itarr = (arr); \
  _itarr_ptr->base = _itarr_ptr->curr = _itarr; \
  _itarr_ptr->last_plus_one = ((char *)_itarr) + (elt_sz) * (num_elts); \
  _itarr_ptr; })
#endif

#ifdef NO_CYC_BOUNDS_CHECKS
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ((arr).curr)
#else
#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned char *
_untag_dyneither_ptr_fn(struct _dyneither_ptr arr, 
                        unsigned elt_sz,unsigned num_elts,
                        const char *filename, unsigned lineno) {
  struct _dyneither_ptr _arr = (arr);
  unsigned char *_curr = _arr.curr;
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&
      _curr != (unsigned char *)0)
    _throw_arraybounds_fn(filename,lineno);
  return _curr;
}
#define _untag_dyneither_ptr(a,s,e) \
  _untag_dyneither_ptr_fn(a,s,e,__FILE__,__LINE__)
#else
#define _untag_dyneither_ptr(arr,elt_sz,num_elts) ({ \
  struct _dyneither_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
#endif
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE unsigned
_get_dyneither_size(struct _dyneither_ptr arr,unsigned elt_sz) {
  struct _dyneither_ptr _get_arr_size_temp = (arr);
  unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr;
  unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one;
  return (_get_arr_size_curr < _get_arr_size_temp.base ||
          _get_arr_size_curr >= _get_arr_size_last) ? 0 :
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));
}
#else
#define _get_dyneither_size(arr,elt_sz) \
  ({struct _dyneither_ptr _get_arr_size_temp = (arr); \
    unsigned char *_get_arr_size_curr=_get_arr_size_temp.curr; \
    unsigned char *_get_arr_size_last=_get_arr_size_temp.last_plus_one; \
    (_get_arr_size_curr < _get_arr_size_temp.base || \
     _get_arr_size_curr >= _get_arr_size_last) ? 0 : \
    ((_get_arr_size_last - _get_arr_size_curr) / (elt_sz));})
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_plus(struct _dyneither_ptr arr,unsigned elt_sz,int change) {
  struct _dyneither_ptr _ans = (arr);
  _ans.curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_plus(arr,elt_sz,change) ({ \
  struct _dyneither_ptr _ans = (arr); \
  _ans.curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,
                            int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return *_arr_ptr;
}
#else
#define _dyneither_ptr_inplace_plus(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  *_arr_ptr; })
#endif

#ifdef _INLINE_FUNCTIONS
static _INLINE struct _dyneither_ptr
_dyneither_ptr_inplace_plus_post(struct _dyneither_ptr *arr_ptr,unsigned elt_sz,int change) {
  struct _dyneither_ptr * _arr_ptr = (arr_ptr);
  struct _dyneither_ptr _ans = *_arr_ptr;
  _arr_ptr->curr += ((int)(elt_sz))*(change);
  return _ans;
}
#else
#define _dyneither_ptr_inplace_plus_post(arr_ptr,elt_sz,change) ({ \
  struct _dyneither_ptr * _arr_ptr = (arr_ptr); \
  struct _dyneither_ptr _ans = *_arr_ptr; \
  _arr_ptr->curr += ((int)(elt_sz))*(change); \
  _ans; })
#endif

/* Decrease the upper bound on a fat pointer by numelts where sz is
   the size of the pointer's type.  Note that this can't be a macro
   if we're to get initializers right. */
static struct 
_dyneither_ptr _dyneither_ptr_decrease_size(struct _dyneither_ptr x,
                                            unsigned int sz,
                                            unsigned int numelts) {
  x.last_plus_one -= sz * numelts; 
  return x; 
}

/* Allocation */

extern void* GC_malloc(int);
extern void* GC_malloc_atomic(int);
extern void* GC_calloc(unsigned,unsigned);
extern void* GC_calloc_atomic(unsigned,unsigned);

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
#ifdef CYC_NOALIGN
    unsigned s =  orig_s;
#else
    unsigned s =  (orig_s + _CYC_MIN_ALIGNMENT - 1) & (~(_CYC_MIN_ALIGNMENT -1)); 
#endif
    char *result; 
    result = r->offset; 
    if (s <= (r->last_plus_one - result)) {
      r->offset = result + s; 
#ifdef CYC_REGION_PROFILE
    r->curr->free_bytes = r->curr->free_bytes - s;
    rgn_total_bytes += s;
#endif
      return result;
    }
  } 
  return _region_malloc(r,orig_s); 
}

/* FIX?  Not sure if we want to pass filename and lineno in here... */
static _INLINE void* _cycalloc(int n) {
  void * ans = (void *)GC_malloc(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cycalloc_atomic(int n) {
  void * ans = (void *)GC_malloc_atomic(n);
  if(!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
static _INLINE void* _cyccalloc_atomic(unsigned n, unsigned s) {
  void* ans = (void*)GC_calloc_atomic(n,s);
  if (!ans)
    _throw_badalloc();
  return ans;
}
#define MAX_MALLOC_SIZE (1 << 28)
static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
  _CYC_U_LONG_LONG_T whole_ans = 
    ((_CYC_U_LONG_LONG_T)x)*((_CYC_U_LONG_LONG_T)y);
  unsigned word_ans = (unsigned)whole_ans;
  if(word_ans < whole_ans || word_ans > MAX_MALLOC_SIZE)
    _throw_badalloc();
  return word_ans;
}

#if defined(CYC_REGION_PROFILE) 
extern void* _profile_GC_malloc(int,const char *file,const char *func,
                                int lineno);
extern void* _profile_GC_malloc_atomic(int,const char *file,
                                       const char *func,int lineno);
extern void* _profile_region_malloc(struct _RegionHandle *, unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern void* _profile_region_calloc(struct _RegionHandle *, unsigned,
                                    unsigned,
                                    const char *file,
                                    const char *func,
                                    int lineno);
extern struct _RegionHandle _profile_new_region(const char *rgn_name,
						const char *file,
						const char *func,
                                                int lineno);
extern void _profile_free_region(struct _RegionHandle *,
				 const char *file,
                                 const char *func,
                                 int lineno);
#  if !defined(RUNTIME_CYC)
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif

# 35 "core.h"
 typedef char*Cyc_Cstring;
typedef char*Cyc_CstringNN;
typedef struct _dyneither_ptr Cyc_string_t;
# 40
typedef struct _dyneither_ptr Cyc_mstring_t;
# 43
typedef struct _dyneither_ptr*Cyc_stringptr_t;
# 47
typedef struct _dyneither_ptr*Cyc_mstringptr_t;
# 50
typedef char*Cyc_Cbuffer_t;
# 52
typedef char*Cyc_CbufferNN_t;
# 54
typedef struct _dyneither_ptr Cyc_buffer_t;
# 56
typedef struct _dyneither_ptr Cyc_mbuffer_t;
# 59
typedef int Cyc_bool;
# 26 "cycboot.h"
typedef unsigned long Cyc_size_t;
# 33
typedef unsigned short Cyc_mode_t;struct Cyc___cycFILE;
# 49
typedef struct Cyc___cycFILE Cyc_FILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 310 "cycboot.h"
int toupper(int);
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;
# 97 "core.h"
struct _dyneither_ptr Cyc_Core_new_string(unsigned int);
# 101
struct _dyneither_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 290 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef void*Cyc_Core___nn_cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;
# 301
struct _dyneither_ptr Cyc_Core_mkfat(void*arr,unsigned int s,unsigned int n);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
int Cyc_zstrcmp(struct _dyneither_ptr,struct _dyneither_ptr);
int Cyc_zstrncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n);
int Cyc_zstrptrcmp(struct _dyneither_ptr*,struct _dyneither_ptr*);
int Cyc_strcasecmp(struct _dyneither_ptr,struct _dyneither_ptr);
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 60
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);
struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*,struct Cyc_List_List*);
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);
# 71
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr,struct _dyneither_ptr,unsigned long);
# 76
struct _dyneither_ptr Cyc_realloc(struct _dyneither_ptr,unsigned long);
struct _dyneither_ptr Cyc_rrealloc(struct _RegionHandle*,struct _dyneither_ptr s,unsigned long sz);
# 81
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int);
struct _dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long,unsigned int);
int Cyc_memcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n);
struct _dyneither_ptr Cyc_memchr(struct _dyneither_ptr s,char c,unsigned long n);
struct _dyneither_ptr Cyc_mmemchr(struct _dyneither_ptr s,char c,unsigned long n);
struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,unsigned long n);
void Cyc_bzero(struct _dyneither_ptr s,unsigned long n);
void Cyc__bcopy(struct _dyneither_ptr src,struct _dyneither_ptr dst,unsigned long n,unsigned int sz);
# 99 "string.h"
struct _dyneither_ptr Cyc_expand(struct _dyneither_ptr s,unsigned long sz);
struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*,struct _dyneither_ptr s,unsigned long sz);
struct _dyneither_ptr Cyc_realloc_str(struct _dyneither_ptr str,unsigned long sz);
struct _dyneither_ptr Cyc_rrealloc_str(struct _RegionHandle*r,struct _dyneither_ptr str,unsigned long sz);
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src);
struct _dyneither_ptr Cyc_rstrdup(struct _RegionHandle*,struct _dyneither_ptr src);
# 108
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr,int ofs,unsigned long n);
struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*,struct _dyneither_ptr,int ofs,unsigned long n);
# 113
struct _dyneither_ptr Cyc_replace_suffix(struct _dyneither_ptr,struct _dyneither_ptr,struct _dyneither_ptr);
struct _dyneither_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix);
# 119
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_mstrrchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_strrchr(struct _dyneither_ptr s,char c);
struct _dyneither_ptr Cyc_mstrstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle);
struct _dyneither_ptr Cyc_strstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle);
struct _dyneither_ptr Cyc_strpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept);
struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept);
unsigned long Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept);
unsigned long Cyc_strcspn(struct _dyneither_ptr s,struct _dyneither_ptr accept);
struct _dyneither_ptr Cyc_strtok(struct _dyneither_ptr s,struct _dyneither_ptr delim);
# 133
struct Cyc_List_List*Cyc_explode(struct _dyneither_ptr s);
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*,struct _dyneither_ptr s);
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*c);
# 25 "strings.h"
int Cyc_strcasecmp(struct _dyneither_ptr,struct _dyneither_ptr);
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long len);
# 29 "assert.h"
void*Cyc___assert_fail(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line);
# 39 "string.cyc"
unsigned long Cyc_strlen(struct _dyneither_ptr s){
unsigned long i;
unsigned int sz=_get_dyneither_size(s,sizeof(char));
for(i=0;i < sz;++ i){
if(((const char*)s.curr)[(int)i]== '\000')
return i;}
# 46
return i;}
# 52
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
int i=0;
unsigned int sz1=_get_dyneither_size(s1,sizeof(char));
unsigned int sz2=_get_dyneither_size(s2,sizeof(char));
unsigned int minsz=sz1 < sz2?sz1: sz2;
minsz <= _get_dyneither_size(s1,sizeof(char)) && minsz <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp0="minsz <= numelts(s1) && minsz <= numelts(s2)";_tag_dyneither(_tmp0,sizeof(char),45);}),({const char*_tmp1="string.cyc";_tag_dyneither(_tmp1,sizeof(char),11);}),59);
while(i < minsz){
char c1=((const char*)s1.curr)[i];
char c2=((const char*)s2.curr)[i];
if(c1 == '\000'){
if(c2 == '\000')return 0;else{
return - 1;}}else{
if(c2 == '\000')return 1;else{
# 68
int diff=c1 - c2;
if(diff != 0)return diff;}}
# 71
++ i;}
# 73
if(sz1 == sz2)return 0;
if(minsz < sz2){
if(*((const char*)_check_dyneither_subscript(s2,sizeof(char),i))== '\000')return 0;else{
return - 1;}}else{
# 78
if(*((const char*)_check_dyneither_subscript(s1,sizeof(char),i))== '\000')return 0;else{
return 1;}}};}
# 83
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2){
return Cyc_strcmp((struct _dyneither_ptr)*s1,(struct _dyneither_ptr)*s2);}
# 87
inline static int Cyc_ncmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2,unsigned long n){
# 90
if(n <= 0)return 0;{
# 92
unsigned long min_len=len1 > len2?len2: len1;
unsigned long bound=min_len > n?n: min_len;
# 95
bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp2="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp2,sizeof(char),45);}),({const char*_tmp3="string.cyc";_tag_dyneither(_tmp3,sizeof(char),11);}),95);
# 97
{int i=0;for(0;i < bound;++ i){
int retc;
if((retc=((const char*)s1.curr)[i]- ((const char*)s2.curr)[i])!= 0)
return retc;}}
# 102
if(len1 < n  || len2 < n)
return(int)len1 - (int)len2;
return 0;};}
# 109
int Cyc_strncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_ncmp(s1,len1,s2,len2,n);}
# 119
int Cyc_zstrcmp(struct _dyneither_ptr a,struct _dyneither_ptr b){
if((char*)a.curr == (char*)b.curr)
return 0;{
unsigned long as=_get_dyneither_size(a,sizeof(char));
unsigned long bs=_get_dyneither_size(b,sizeof(char));
unsigned long min_length=as < bs?as: bs;
int i=-1;
# 127
min_length <= _get_dyneither_size(a,sizeof(char)) && min_length <= _get_dyneither_size(b,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp4="min_length <= numelts(a) && min_length <= numelts(b)";_tag_dyneither(_tmp4,sizeof(char),53);}),({const char*_tmp5="string.cyc";_tag_dyneither(_tmp5,sizeof(char),11);}),127);
# 129
while((++ i,i < min_length)){
int diff=(int)((const char*)a.curr)[i]- (int)((const char*)b.curr)[i];
if(diff != 0)
return diff;}
# 134
return(int)as - (int)bs;};}
# 137
int Cyc_zstrncmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
if(n <= 0)return 0;{
# 140
unsigned long s1size=_get_dyneither_size(s1,sizeof(char));
unsigned long s2size=_get_dyneither_size(s2,sizeof(char));
unsigned long min_size=s1size > s2size?s2size: s1size;
unsigned long bound=min_size > n?n: min_size;
# 145
bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp6="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp6,sizeof(char),45);}),({const char*_tmp7="string.cyc";_tag_dyneither(_tmp7,sizeof(char),11);}),145);
# 147
{int i=0;for(0;i < bound;++ i){
if(((const char*)s1.curr)[i]< ((const char*)s2.curr)[i])
return - 1;else{
if(((const char*)s2.curr)[i]< ((const char*)s1.curr)[i])
return 1;}}}
# 153
if(min_size <= bound)
return 0;
if(s1size < s2size)
return - 1;else{
# 158
return 1;}};}
# 162
int Cyc_zstrptrcmp(struct _dyneither_ptr*a,struct _dyneither_ptr*b){
if((int)a == (int)b)return 0;
return Cyc_zstrcmp((struct _dyneither_ptr)*a,(struct _dyneither_ptr)*b);}
# 172
inline static struct _dyneither_ptr Cyc_int_strcato(struct _dyneither_ptr dest,struct _dyneither_ptr src){
int i;
unsigned long dsize;unsigned long slen;unsigned long dlen;
# 176
dsize=_get_dyneither_size(dest,sizeof(char));
dlen=Cyc_strlen((struct _dyneither_ptr)dest);
slen=Cyc_strlen(src);
# 180
if(slen + dlen <= dsize){
slen <= _get_dyneither_size(src,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp8="slen <= numelts(src)";_tag_dyneither(_tmp8,sizeof(char),21);}),({const char*_tmp9="string.cyc";_tag_dyneither(_tmp9,sizeof(char),11);}),181);
for(i=0;i < slen;++ i){
({struct _dyneither_ptr _tmpA=_dyneither_ptr_plus(dest,sizeof(char),(int)(i + dlen));char _tmpB=*((char*)_check_dyneither_subscript(_tmpA,sizeof(char),0));char _tmpC=((const char*)src.curr)[i];if(_get_dyneither_size(_tmpA,sizeof(char))== 1  && (_tmpB == '\000'  && _tmpC != '\000'))_throw_arraybounds();*((char*)_tmpA.curr)=_tmpC;});}
# 185
if(i != dsize)
({struct _dyneither_ptr _tmpD=_dyneither_ptr_plus(dest,sizeof(char),(int)(i + dlen));char _tmpE=*((char*)_check_dyneither_subscript(_tmpD,sizeof(char),0));char _tmpF='\000';if(_get_dyneither_size(_tmpD,sizeof(char))== 1  && (_tmpE == '\000'  && _tmpF != '\000'))_throw_arraybounds();*((char*)_tmpD.curr)=_tmpF;});}else{
# 189
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp11;_tmp11.tag=Cyc_Core_Invalid_argument;_tmp11.f1=({const char*_tmp12="strcat";_tag_dyneither(_tmp12,sizeof(char),7);});_tmp11;});_tmp10;}));}
return dest;}
# 195
struct _dyneither_ptr Cyc_strcat(struct _dyneither_ptr dest,struct _dyneither_ptr src){
return Cyc_int_strcato(dest,src);}
# 200
struct _dyneither_ptr Cyc_rstrconcat(struct _RegionHandle*r,struct _dyneither_ptr a,struct _dyneither_ptr b){
unsigned long _tmp13=Cyc_strlen(a);
unsigned long _tmp14=Cyc_strlen(b);
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,(_tmp13 + _tmp14)+ 1);
int i;int j;
_tmp13 <= _get_dyneither_size(ans,sizeof(char)) && _tmp13 <= _get_dyneither_size(a,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp15="alen <= numelts(ans) && alen <= numelts(a)";_tag_dyneither(_tmp15,sizeof(char),43);}),({const char*_tmp16="string.cyc";_tag_dyneither(_tmp16,sizeof(char),11);}),205);
for(i=0;i < _tmp13;++ i){({struct _dyneither_ptr _tmp17=_dyneither_ptr_plus(ans,sizeof(char),i);char _tmp18=*((char*)_check_dyneither_subscript(_tmp17,sizeof(char),0));char _tmp19=((const char*)a.curr)[i];if(_get_dyneither_size(_tmp17,sizeof(char))== 1  && (_tmp18 == '\000'  && _tmp19 != '\000'))_throw_arraybounds();*((char*)_tmp17.curr)=_tmp19;});}
_tmp14 <= _get_dyneither_size(b,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp1A="blen <= numelts(b)";_tag_dyneither(_tmp1A,sizeof(char),19);}),({const char*_tmp1B="string.cyc";_tag_dyneither(_tmp1B,sizeof(char),11);}),207);
for(j=0;j < _tmp14;++ j){({struct _dyneither_ptr _tmp1C=_dyneither_ptr_plus(ans,sizeof(char),i + j);char _tmp1D=*((char*)_check_dyneither_subscript(_tmp1C,sizeof(char),0));char _tmp1E=((const char*)b.curr)[j];if(_get_dyneither_size(_tmp1C,sizeof(char))== 1  && (_tmp1D == '\000'  && _tmp1E != '\000'))_throw_arraybounds();*((char*)_tmp1C.curr)=_tmp1E;});}
return ans;}
# 212
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr a,struct _dyneither_ptr b){
return Cyc_rstrconcat(Cyc_Core_heap_region,a,b);}
# 217
struct _dyneither_ptr Cyc_rstrconcat_l(struct _RegionHandle*r,struct Cyc_List_List*strs){
# 219
unsigned long len;
unsigned long total_len=0;
struct _dyneither_ptr ans;
struct _RegionHandle _tmp1F=_new_region("temp");struct _RegionHandle*temp=& _tmp1F;_push_region(temp);{
struct Cyc_List_List*lens=({struct Cyc_List_List*_tmp22=_region_malloc(temp,sizeof(*_tmp22));_tmp22->hd=(void*)0;_tmp22->tl=0;_tmp22;});
# 226
struct Cyc_List_List*end=lens;
{struct Cyc_List_List*p=strs;for(0;p != 0;p=p->tl){
len=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)p->hd));
total_len +=len;
((struct Cyc_List_List*)_check_null(end))->tl=({struct Cyc_List_List*_tmp20=_region_malloc(temp,sizeof(*_tmp20));_tmp20->hd=(void*)len;_tmp20->tl=0;_tmp20;});
end=end->tl;}}
# 233
lens=lens->tl;
ans=Cyc_Core_rnew_string(r,total_len + 1);{
unsigned long i=0;
while(strs != 0){
struct _dyneither_ptr _tmp21=*((struct _dyneither_ptr*)strs->hd);
len=(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd;
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),(struct _dyneither_ptr)_tmp21,len);
i +=len;
strs=strs->tl;
lens=lens->tl;}};}{
# 245
struct _dyneither_ptr _tmp23=ans;_npop_handler(0);return _tmp23;};
# 222
;_pop_region(temp);}
# 248
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*strs){
return Cyc_rstrconcat_l(Cyc_Core_heap_region,strs);}
# 254
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*r,struct Cyc_List_List*strs,struct _dyneither_ptr separator){
if(strs == 0)return Cyc_Core_rnew_string(r,0);
if(strs->tl == 0)return Cyc_rstrdup(r,(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd));{
struct Cyc_List_List*_tmp24=strs;
struct _RegionHandle _tmp25=_new_region("temp");struct _RegionHandle*temp=& _tmp25;_push_region(temp);
{struct Cyc_List_List*lens=({struct Cyc_List_List*_tmp29=_region_malloc(temp,sizeof(*_tmp29));_tmp29->hd=(void*)0;_tmp29->tl=0;_tmp29;});
struct Cyc_List_List*end=lens;
unsigned long len;
unsigned long total_len=0;
unsigned long list_len=0;
for(0;_tmp24 != 0;_tmp24=_tmp24->tl){
len=Cyc_strlen((struct _dyneither_ptr)*((struct _dyneither_ptr*)_tmp24->hd));
total_len +=len;
((struct Cyc_List_List*)_check_null(end))->tl=({struct Cyc_List_List*_tmp26=_region_malloc(temp,sizeof(*_tmp26));_tmp26->hd=(void*)len;_tmp26->tl=0;_tmp26;});
end=end->tl;
++ list_len;}
# 271
lens=lens->tl;{
unsigned long seplen=Cyc_strlen(separator);
total_len +=(list_len - 1)* seplen;{
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,total_len + 1);
unsigned long i=0;
while(strs->tl != 0){
struct _dyneither_ptr _tmp27=*((struct _dyneither_ptr*)strs->hd);
len=(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd;
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),(struct _dyneither_ptr)_tmp27,len);
i +=len;
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),separator,seplen);
i +=seplen;
strs=strs->tl;
lens=lens->tl;}
# 286
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)i),sizeof(char),1),(struct _dyneither_ptr)*((struct _dyneither_ptr*)strs->hd),(unsigned long)((struct Cyc_List_List*)_check_null(lens))->hd);{
struct _dyneither_ptr _tmp28=ans;_npop_handler(0);return _tmp28;};};};}
# 259
;_pop_region(temp);};}
# 291
struct _dyneither_ptr Cyc_str_sepstr(struct Cyc_List_List*strs,struct _dyneither_ptr separator){
return Cyc_rstr_sepstr(Cyc_Core_heap_region,strs,separator);}
# 296
struct _dyneither_ptr Cyc_strncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned long n){
int i;
n <= _get_dyneither_size(src,sizeof(char)) && n <= _get_dyneither_size(dest,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp2A="n <= numelts(src) && n <= numelts(dest)";_tag_dyneither(_tmp2A,sizeof(char),40);}),({const char*_tmp2B="string.cyc";_tag_dyneither(_tmp2B,sizeof(char),11);}),298);
for(i=0;i < n;++ i){
char _tmp2C=((const char*)src.curr)[i];
if(_tmp2C == '\000')break;
((char*)dest.curr)[i]=_tmp2C;}
# 304
for(0;i < n;++ i){
((char*)dest.curr)[i]='\000';}
# 307
return dest;}
# 311
struct _dyneither_ptr Cyc_zstrncpy(struct _dyneither_ptr dest,struct _dyneither_ptr src,unsigned long n){
n <= _get_dyneither_size(dest,sizeof(char)) && n <= _get_dyneither_size(src,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp2D="n <= numelts(dest) && n <= numelts(src)";_tag_dyneither(_tmp2D,sizeof(char),40);}),({const char*_tmp2E="string.cyc";_tag_dyneither(_tmp2E,sizeof(char),11);}),312);{
int i;
for(i=0;i < n;++ i){
((char*)dest.curr)[i]=((const char*)src.curr)[i];}
return dest;};}
# 319
struct _dyneither_ptr Cyc_strcpy(struct _dyneither_ptr dest,struct _dyneither_ptr src){
unsigned int ssz=_get_dyneither_size(src,sizeof(char));
unsigned int dsz=_get_dyneither_size(dest,sizeof(char));
if(ssz <= dsz){
unsigned int i;
for(i=0;i < ssz;++ i){
char _tmp2F=((const char*)src.curr)[(int)i];
({struct _dyneither_ptr _tmp30=_dyneither_ptr_plus(dest,sizeof(char),(int)i);char _tmp31=*((char*)_check_dyneither_subscript(_tmp30,sizeof(char),0));char _tmp32=_tmp2F;if(_get_dyneither_size(_tmp30,sizeof(char))== 1  && (_tmp31 == '\000'  && _tmp32 != '\000'))_throw_arraybounds();*((char*)_tmp30.curr)=_tmp32;});
if(_tmp2F == '\000')break;}}else{
# 331
unsigned long len=Cyc_strlen(src);
Cyc_strncpy(_dyneither_ptr_decrease_size(dest,sizeof(char),1),src,len);
if(len < _get_dyneither_size(dest,sizeof(char)))
({struct _dyneither_ptr _tmp33=_dyneither_ptr_plus(dest,sizeof(char),(int)len);char _tmp34=*((char*)_check_dyneither_subscript(_tmp33,sizeof(char),0));char _tmp35='\000';if(_get_dyneither_size(_tmp33,sizeof(char))== 1  && (_tmp34 == '\000'  && _tmp35 != '\000'))_throw_arraybounds();*((char*)_tmp33.curr)=_tmp35;});}
# 336
return dest;}
# 342
struct _dyneither_ptr Cyc_rstrdup(struct _RegionHandle*r,struct _dyneither_ptr src){
unsigned long len;
struct _dyneither_ptr temp;
# 346
len=Cyc_strlen(src);
temp=Cyc_Core_rnew_string(r,len + 1);
{struct _dyneither_ptr _tmp36=_dyneither_ptr_decrease_size(temp,sizeof(char),1);
struct _dyneither_ptr _tmp37=_tmp36;struct _dyneither_ptr _tmp38;_LL1: _tmp38=_tmp37;_LL2:;
Cyc_strncpy(_tmp38,src,len);}
# 352
return temp;}
# 355
struct _dyneither_ptr Cyc_strdup(struct _dyneither_ptr src){
return Cyc_rstrdup(Cyc_Core_heap_region,src);}
# 359
struct _dyneither_ptr Cyc_rrealloc(struct _RegionHandle*r,struct _dyneither_ptr s,unsigned long sz){
struct _dyneither_ptr temp;
unsigned long slen;
# 363
slen=_get_dyneither_size(s,sizeof(char));
sz=sz > slen?sz: slen;
temp=({unsigned int _tmp39=sz;char*_tmp3A=_region_calloc(r,sizeof(char),_tmp39);_tag_dyneither(_tmp3A,sizeof(char),_tmp39);});
# 367
{struct _dyneither_ptr _tmp3B=temp;struct _dyneither_ptr _tmp3C;_LL4: _tmp3C=_tmp3B;_LL5:;
Cyc_strncpy((struct _dyneither_ptr)_tmp3C,(struct _dyneither_ptr)s,slen);}
# 371
return temp;}
# 374
struct _dyneither_ptr Cyc_rexpand(struct _RegionHandle*r,struct _dyneither_ptr s,unsigned long sz){
struct _dyneither_ptr temp;
unsigned long slen;
# 378
slen=Cyc_strlen(s);
sz=sz > slen?sz: slen;
temp=Cyc_Core_rnew_string(r,sz);
# 382
{struct _dyneither_ptr _tmp3D=temp;struct _dyneither_ptr _tmp3E;_LL7: _tmp3E=_tmp3D;_LL8:;
Cyc_strncpy((struct _dyneither_ptr)_tmp3E,s,slen);}
# 386
if(slen != _get_dyneither_size(s,sizeof(char)))
({struct _dyneither_ptr _tmp3F=_dyneither_ptr_plus(temp,sizeof(char),(int)slen);char _tmp40=*((char*)_check_dyneither_subscript(_tmp3F,sizeof(char),0));char _tmp41='\000';if(_get_dyneither_size(_tmp3F,sizeof(char))== 1  && (_tmp40 == '\000'  && _tmp41 != '\000'))_throw_arraybounds();*((char*)_tmp3F.curr)=_tmp41;});
# 389
return temp;}
# 392
struct _dyneither_ptr Cyc_expand(struct _dyneither_ptr s,unsigned long sz){
return Cyc_rexpand(Cyc_Core_heap_region,s,sz);}
# 396
struct _dyneither_ptr Cyc_rrealloc_str(struct _RegionHandle*r,struct _dyneither_ptr str,unsigned long sz){
# 398
unsigned long maxsizeP=_get_dyneither_size(str,sizeof(char));
# 400
if(maxsizeP == 0){
maxsizeP=30 > sz?30: sz;
str=Cyc_Core_rnew_string(r,maxsizeP);
({struct _dyneither_ptr _tmp42=_dyneither_ptr_plus(str,sizeof(char),0);char _tmp43=*((char*)_check_dyneither_subscript(_tmp42,sizeof(char),0));char _tmp44='\000';if(_get_dyneither_size(_tmp42,sizeof(char))== 1  && (_tmp43 == '\000'  && _tmp44 != '\000'))_throw_arraybounds();*((char*)_tmp42.curr)=_tmp44;});}else{
# 405
if(sz > maxsizeP){
maxsizeP=maxsizeP * 2 > (sz * 5)/ 4?maxsizeP * 2:(sz * 5)/ 4;{
struct _dyneither_ptr _tmp45=str;struct _dyneither_ptr _tmp46;_LLA: _tmp46=_tmp45;_LLB:;
_tmp46=_dyneither_ptr_decrease_size(Cyc_rexpand(r,(struct _dyneither_ptr)_tmp46,maxsizeP),sizeof(char),1);};}}
# 411
return str;}
# 414
struct _dyneither_ptr Cyc_realloc_str(struct _dyneither_ptr str,unsigned long sz){
return Cyc_rrealloc_str(Cyc_Core_heap_region,str,sz);}
# 423
struct _dyneither_ptr Cyc_rsubstring(struct _RegionHandle*r,struct _dyneither_ptr s,int start,unsigned long amt){
# 427
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,amt + 1);
s=_dyneither_ptr_plus(s,sizeof(char),start);
amt < _get_dyneither_size(ans,sizeof(char)) && amt <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp47="amt < numelts(ans) && amt <= numelts(s)";_tag_dyneither(_tmp47,sizeof(char),40);}),({const char*_tmp48="string.cyc";_tag_dyneither(_tmp48,sizeof(char),11);}),429);
{unsigned long i=0;for(0;i < amt;++ i){
({struct _dyneither_ptr _tmp49=_dyneither_ptr_plus(ans,sizeof(char),(int)i);char _tmp4A=*((char*)_check_dyneither_subscript(_tmp49,sizeof(char),0));char _tmp4B=((const char*)s.curr)[(int)i];if(_get_dyneither_size(_tmp49,sizeof(char))== 1  && (_tmp4A == '\000'  && _tmp4B != '\000'))_throw_arraybounds();*((char*)_tmp49.curr)=_tmp4B;});}}
({struct _dyneither_ptr _tmp4C=_dyneither_ptr_plus(ans,sizeof(char),(int)amt);char _tmp4D=*((char*)_check_dyneither_subscript(_tmp4C,sizeof(char),0));char _tmp4E='\000';if(_get_dyneither_size(_tmp4C,sizeof(char))== 1  && (_tmp4D == '\000'  && _tmp4E != '\000'))_throw_arraybounds();*((char*)_tmp4C.curr)=_tmp4E;});
return ans;}
# 436
struct _dyneither_ptr Cyc_substring(struct _dyneither_ptr s,int start,unsigned long amt){
return Cyc_rsubstring(Cyc_Core_heap_region,s,start,amt);}
# 442
struct _dyneither_ptr Cyc_rreplace_suffix(struct _RegionHandle*r,struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix){
unsigned long m=_get_dyneither_size(src,sizeof(char));
unsigned long n=_get_dyneither_size(curr_suffix,sizeof(char));
struct _dyneither_ptr err=({const char*_tmp53="replace_suffix";_tag_dyneither(_tmp53,sizeof(char),15);});
if(m < n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp50;_tmp50.tag=Cyc_Core_Invalid_argument;_tmp50.f1=err;_tmp50;});_tmp4F;}));
{unsigned long i=1;for(0;i <= n;++ i){
if(*((const char*)_check_dyneither_subscript(src,sizeof(char),(int)(m - i)))!= *((const char*)_check_dyneither_subscript(curr_suffix,sizeof(char),(int)(n - i))))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp52;_tmp52.tag=Cyc_Core_Invalid_argument;_tmp52.f1=err;_tmp52;});_tmp51;}));}}{
struct _dyneither_ptr ans=Cyc_Core_rnew_string(r,((m - n)+ _get_dyneither_size(new_suffix,sizeof(char)))+ 1);
Cyc_strncpy(_dyneither_ptr_decrease_size(ans,sizeof(char),1),src,m - n);
Cyc_strncpy(_dyneither_ptr_decrease_size(_dyneither_ptr_plus(ans,sizeof(char),(int)(m - n)),sizeof(char),1),new_suffix,_get_dyneither_size(new_suffix,sizeof(char)));
return ans;};}
# 457
struct _dyneither_ptr Cyc_replace_suffix(struct _dyneither_ptr src,struct _dyneither_ptr curr_suffix,struct _dyneither_ptr new_suffix){
return Cyc_rreplace_suffix(Cyc_Core_heap_region,src,curr_suffix,new_suffix);}
# 464
struct _dyneither_ptr Cyc_strpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept){
int len=(int)_get_dyneither_size(s,sizeof(char));
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
char c;
unsigned int i;
for(i=0;i < len  && (c=((const char*)s.curr)[(int)i])!= 0;++ i){
unsigned int j=0;for(0;j < asize;++ j){
if(c == ((const char*)accept.curr)[(int)j])
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}
# 474
return _tag_dyneither(0,0,0);}
# 477
struct _dyneither_ptr Cyc_mstrpbrk(struct _dyneither_ptr s,struct _dyneither_ptr accept){
int len=(int)_get_dyneither_size(s,sizeof(char));
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
char c;
unsigned int i;
for(i=0;i < len  && (c=((char*)s.curr)[(int)i])!= 0;++ i){
unsigned int j=0;for(0;j < asize;++ j){
if(c == ((const char*)accept.curr)[(int)j])
return _dyneither_ptr_plus(s,sizeof(char),(int)i);}}
# 487
return _tag_dyneither(0,0,0);}
# 492
struct _dyneither_ptr Cyc_mstrchr(struct _dyneither_ptr s,char c){
int len=(int)_get_dyneither_size(s,sizeof(char));
char c2;
unsigned int i;
# 497
for(i=0;i < len  && (c2=((char*)s.curr)[(int)i])!= 0;++ i){
if(c2 == c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}
# 500
return _tag_dyneither(0,0,0);}
# 503
struct _dyneither_ptr Cyc_strchr(struct _dyneither_ptr s,char c){
int len=(int)_get_dyneither_size(s,sizeof(char));
char c2;
unsigned int i;
# 508
for(i=0;i < len  && (c2=((const char*)s.curr)[(int)i])!= 0;++ i){
if(c2 == c)return _dyneither_ptr_plus(s,sizeof(char),(int)i);}
# 511
return _tag_dyneither(0,0,0);}
# 516
struct _dyneither_ptr Cyc_strrchr(struct _dyneither_ptr s0,char c){
int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
int i=len - 1;
struct _dyneither_ptr s=s0;
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);
# 522
for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){
if(*((const char*)_check_dyneither_subscript(s,sizeof(char),0))== c)
return(struct _dyneither_ptr)s;}
# 526
return _tag_dyneither(0,0,0);}
# 529
struct _dyneither_ptr Cyc_mstrrchr(struct _dyneither_ptr s0,char c){
int len=(int)Cyc_strlen((struct _dyneither_ptr)s0);
int i=len - 1;
struct _dyneither_ptr s=s0;
_dyneither_ptr_inplace_plus(& s,sizeof(char),i);
# 535
for(0;i >= 0;(i --,_dyneither_ptr_inplace_plus_post(& s,sizeof(char),-1))){
if(*((char*)_check_dyneither_subscript(s,sizeof(char),0))== c)
return(struct _dyneither_ptr)s;}
# 539
return _tag_dyneither(0,0,0);}
# 544
struct _dyneither_ptr Cyc_strstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle){
if(!((unsigned int)haystack.curr) || !((unsigned int)needle.curr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp55;_tmp55.tag=Cyc_Core_Invalid_argument;_tmp55.f1=({const char*_tmp56="strstr";_tag_dyneither(_tmp56,sizeof(char),7);});_tmp55;});_tmp54;}));
if(*((const char*)_check_dyneither_subscript(needle,sizeof(char),0))== '\000')return haystack;{
# 548
int len=(int)Cyc_strlen((struct _dyneither_ptr)needle);
{struct _dyneither_ptr start=haystack;for(0;(char*)(start=
Cyc_strchr(start,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))).curr != (char*)(_tag_dyneither(0,0,0)).curr;start=
Cyc_strchr(_dyneither_ptr_plus(start,sizeof(char),1),*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))){
if(Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned long)len)== 0)
return start;}}
# 555
return _tag_dyneither(0,0,0);};}
# 558
struct _dyneither_ptr Cyc_mstrstr(struct _dyneither_ptr haystack,struct _dyneither_ptr needle){
if(!((unsigned int)haystack.curr) || !((unsigned int)needle.curr))(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp58;_tmp58.tag=Cyc_Core_Invalid_argument;_tmp58.f1=({const char*_tmp59="mstrstr";_tag_dyneither(_tmp59,sizeof(char),8);});_tmp58;});_tmp57;}));
if(*((const char*)_check_dyneither_subscript(needle,sizeof(char),0))== '\000')return haystack;{
# 562
int len=(int)Cyc_strlen((struct _dyneither_ptr)needle);
{struct _dyneither_ptr start=haystack;for(0;(char*)(start=
Cyc_mstrchr(start,*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))).curr != (char*)(_tag_dyneither(0,0,0)).curr;start=
Cyc_mstrchr(_dyneither_ptr_plus(start,sizeof(char),1),*((const char*)_check_dyneither_subscript(needle,sizeof(char),0)))){
if(Cyc_strncmp((struct _dyneither_ptr)start,(struct _dyneither_ptr)needle,(unsigned long)len)== 0)
return start;}}
# 569
return _tag_dyneither(0,0,0);};}
# 575
unsigned long Cyc_strspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
unsigned long len=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
# 579
len <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp5A="len <= numelts(s)";_tag_dyneither(_tmp5A,sizeof(char),18);}),({const char*_tmp5B="string.cyc";_tag_dyneither(_tmp5B,sizeof(char),11);}),579);
{unsigned long i=0;for(0;i < len;++ i){
int j;
for(j=0;j < asize;++ j){
if(((const char*)s.curr)[(int)i]== ((const char*)accept.curr)[j])
break;}
if(j == asize)
return i;}}
# 589
return len;}
# 595
unsigned long Cyc_strcspn(struct _dyneither_ptr s,struct _dyneither_ptr accept){
unsigned long len=Cyc_strlen((struct _dyneither_ptr)s);
unsigned int asize=_get_dyneither_size(accept,sizeof(char));
# 599
len <= _get_dyneither_size(s,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp5C="len <= numelts(s)";_tag_dyneither(_tmp5C,sizeof(char),18);}),({const char*_tmp5D="string.cyc";_tag_dyneither(_tmp5D,sizeof(char),11);}),599);
{unsigned long i=0;for(0;i < len;++ i){
int j;
for(j=0;j < asize;++ j){
if(((const char*)s.curr)[(int)i]!= ((const char*)accept.curr)[j])
break;}
if(j == asize)
return i;}}
# 609
return len;}
# 616
struct _dyneither_ptr Cyc_strtok(struct _dyneither_ptr s,struct _dyneither_ptr delim){
# 618
static struct _dyneither_ptr olds={(void*)0,(void*)0,(void*)(0 + 0)};
struct _dyneither_ptr token;
# 621
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr){
if((char*)olds.curr == (char*)(_tag_dyneither(0,0,0)).curr)
return _tag_dyneither(0,0,0);
s=olds;}{
# 628
unsigned long inc=Cyc_strspn((struct _dyneither_ptr)s,delim);
if(inc >= _get_dyneither_size(s,sizeof(char)) || *((char*)_check_dyneither_subscript(_dyneither_ptr_plus(s,sizeof(char),(int)inc),sizeof(char),0))== '\000'){
# 631
olds=_tag_dyneither(0,0,0);
return _tag_dyneither(0,0,0);}else{
# 635
_dyneither_ptr_inplace_plus(& s,sizeof(char),(int)inc);}
# 638
token=s;
s=Cyc_mstrpbrk(token,(struct _dyneither_ptr)delim);
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr)
# 642
olds=_tag_dyneither(0,0,0);else{
# 646
({struct _dyneither_ptr _tmp5E=s;char _tmp5F=*((char*)_check_dyneither_subscript(_tmp5E,sizeof(char),0));char _tmp60='\000';if(_get_dyneither_size(_tmp5E,sizeof(char))== 1  && (_tmp5F == '\000'  && _tmp60 != '\000'))_throw_arraybounds();*((char*)_tmp5E.curr)=_tmp60;});
olds=_dyneither_ptr_plus(s,sizeof(char),1);}
# 649
return token;};}
# 653
struct Cyc_List_List*Cyc_rexplode(struct _RegionHandle*r,struct _dyneither_ptr s){
struct Cyc_List_List*result=0;
{int i=(int)(Cyc_strlen(s)- 1);for(0;i >= 0;-- i){
result=({struct Cyc_List_List*_tmp61=_region_malloc(r,sizeof(*_tmp61));_tmp61->hd=(void*)((int)*((const char*)_check_dyneither_subscript(s,sizeof(char),i)));_tmp61->tl=result;_tmp61;});}}
return result;}
# 660
struct Cyc_List_List*Cyc_explode(struct _dyneither_ptr s){
return Cyc_rexplode(Cyc_Core_heap_region,s);}
# 664
struct _dyneither_ptr Cyc_implode(struct Cyc_List_List*chars){
struct _dyneither_ptr s=Cyc_Core_new_string((unsigned int)(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(chars)+ 1));
unsigned long i=0;
while(chars != 0){
({struct _dyneither_ptr _tmp62=_dyneither_ptr_plus(s,sizeof(char),(int)i ++);char _tmp63=*((char*)_check_dyneither_subscript(_tmp62,sizeof(char),0));char _tmp64=(char)((int)chars->hd);if(_get_dyneither_size(_tmp62,sizeof(char))== 1  && (_tmp63 == '\000'  && _tmp64 != '\000'))_throw_arraybounds();*((char*)_tmp62.curr)=_tmp64;});
chars=chars->tl;}
# 671
return s;}
# 675
inline static int Cyc_casecmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2){
# 678
unsigned long min_length=len1 < len2?len1: len2;
# 680
min_length <= _get_dyneither_size(s1,sizeof(char)) && min_length <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp65="min_length <= numelts(s1) && min_length <= numelts(s2)";_tag_dyneither(_tmp65,sizeof(char),55);}),({const char*_tmp66="string.cyc";_tag_dyneither(_tmp66,sizeof(char),11);}),680);{
# 682
int i=-1;
while((++ i,i < min_length)){
int diff= toupper((int)((const char*)s1.curr)[i])-  toupper((int)((const char*)s2.curr)[i]);
if(diff != 0)
return diff;}
# 688
return(int)len1 - (int)len2;};}
# 691
int Cyc_strcasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2){
if((char*)s1.curr == (char*)s2.curr)
return 0;{
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_casecmp(s1,len1,s2,len2);};}
# 699
inline static int Cyc_caseless_ncmp(struct _dyneither_ptr s1,unsigned long len1,struct _dyneither_ptr s2,unsigned long len2,unsigned long n){
# 702
if(n <= 0)return 0;{
# 704
unsigned long min_len=len1 > len2?len2: len1;
unsigned long bound=min_len > n?n: min_len;
# 707
bound <= _get_dyneither_size(s1,sizeof(char)) && bound <= _get_dyneither_size(s2,sizeof(char))?0:((int(*)(struct _dyneither_ptr assertion,struct _dyneither_ptr file,unsigned int line))Cyc___assert_fail)(({const char*_tmp67="bound <= numelts(s1) && bound <= numelts(s2)";_tag_dyneither(_tmp67,sizeof(char),45);}),({const char*_tmp68="string.cyc";_tag_dyneither(_tmp68,sizeof(char),11);}),707);
# 709
{int i=0;for(0;i < bound;++ i){
int retc;
if((retc= toupper((int)((const char*)s1.curr)[i])-  toupper((int)((const char*)s2.curr)[i]))!= 0)
return retc;}}
# 714
if(len1 < n  || len2 < n)
return(int)len1 - (int)len2;
return 0;};}
# 720
int Cyc_strncasecmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
unsigned long len1=Cyc_strlen(s1);
unsigned long len2=Cyc_strlen(s2);
return Cyc_caseless_ncmp(s1,len1,s2,len2,n);}
# 727
typedef char*Cyc_Cstr___cyclone_buf;
# 729
void* memcpy(void*,const void*,unsigned long n);
void* memmove(void*,const void*,unsigned long n);
int memcmp(const void*,const void*,unsigned long n);
char* memchr(const char*,char c,unsigned long n);
void* memset(void*,int c,unsigned long n);
void bcopy(const void*src,void*dest,unsigned long n);
void bzero(void*s,unsigned long n);
char* GC_realloc(char*,unsigned int n);
# 741
struct _dyneither_ptr Cyc_realloc(struct _dyneither_ptr s,unsigned long n){
unsigned long _tmp69=n;unsigned int _tmp6B;_LLD: _tmp6B=_tmp69;_LLE:;{
char*_tmp6A= GC_realloc((char*)_untag_dyneither_ptr(s,sizeof(char),1),_tmp6B);
return((struct _dyneither_ptr(*)(char*arr,unsigned int s,unsigned int n))Cyc_Core_mkfat)((char*)_check_null(_tmp6A),sizeof(char),_tmp6B);};}
# 747
struct _dyneither_ptr Cyc__memcpy(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long n,unsigned int sz){
if((((void*)d.curr == (void*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(d,sizeof(void))< n) || (void*)s.curr == (void*)(_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(s,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp6D;_tmp6D.tag=Cyc_Core_Invalid_argument;_tmp6D.f1=({const char*_tmp6E="memcpy";_tag_dyneither(_tmp6E,sizeof(char),7);});_tmp6D;});_tmp6C;}));
 memcpy((void*)_untag_dyneither_ptr(d,sizeof(void),1),(const void*)_untag_dyneither_ptr(s,sizeof(void),1),n * sz);
return d;}
# 754
struct _dyneither_ptr Cyc__memmove(struct _dyneither_ptr d,struct _dyneither_ptr s,unsigned long n,unsigned int sz){
if((((void*)d.curr == (void*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(d,sizeof(void))< n) || (void*)s.curr == (void*)(_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(s,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp70;_tmp70.tag=Cyc_Core_Invalid_argument;_tmp70.f1=({const char*_tmp71="memove";_tag_dyneither(_tmp71,sizeof(char),7);});_tmp70;});_tmp6F;}));
 memmove((void*)_untag_dyneither_ptr(d,sizeof(void),1),(const void*)_untag_dyneither_ptr(s,sizeof(void),1),n * sz);
return d;}
# 761
int Cyc_memcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2,unsigned long n){
if((((char*)s1.curr == (char*)(_tag_dyneither(0,0,0)).curr  || (char*)s2.curr == (char*)(_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(s1,sizeof(char))< n) || _get_dyneither_size(s2,sizeof(char))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp73;_tmp73.tag=Cyc_Core_Invalid_argument;_tmp73.f1=({const char*_tmp74="memcmp";_tag_dyneither(_tmp74,sizeof(char),7);});_tmp73;});_tmp72;}));
return memcmp((const void*)_untag_dyneither_ptr(s1,sizeof(char),1),(const void*)_untag_dyneither_ptr(s2,sizeof(char),1),n);}
# 767
struct _dyneither_ptr Cyc_memchr(struct _dyneither_ptr s,char c,unsigned long n){
unsigned int sz=_get_dyneither_size(s,sizeof(char));
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || n > sz)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp76;_tmp76.tag=Cyc_Core_Invalid_argument;_tmp76.f1=({const char*_tmp77="memchr";_tag_dyneither(_tmp77,sizeof(char),7);});_tmp76;});_tmp75;}));{
char*_tmp78= memchr((const char*)_untag_dyneither_ptr(s,sizeof(char),1),c,n);
if(_tmp78 == 0)return _tag_dyneither(0,0,0);{
unsigned int _tmp79=(unsigned int)((const char*)_untag_dyneither_ptr(s,sizeof(char),1));
unsigned int _tmp7A=(unsigned int)((const char*)_tmp78);
unsigned int _tmp7B=_tmp7A - _tmp79;
return _dyneither_ptr_plus(s,sizeof(char),(int)_tmp7B);};};}
# 779
struct _dyneither_ptr Cyc_mmemchr(struct _dyneither_ptr s,char c,unsigned long n){
unsigned int sz=_get_dyneither_size(s,sizeof(char));
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || n > sz)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp7D;_tmp7D.tag=Cyc_Core_Invalid_argument;_tmp7D.f1=({const char*_tmp7E="mmemchr";_tag_dyneither(_tmp7E,sizeof(char),8);});_tmp7D;});_tmp7C;}));{
char*_tmp7F= memchr((const char*)_untag_dyneither_ptr(s,sizeof(char),1),c,n);
if(_tmp7F == 0)return _tag_dyneither(0,0,0);{
unsigned int _tmp80=(unsigned int)((const char*)_untag_dyneither_ptr(s,sizeof(char),1));
unsigned int _tmp81=(unsigned int)_tmp7F;
unsigned int _tmp82=_tmp81 - _tmp80;
return _dyneither_ptr_plus(s,sizeof(char),(int)_tmp82);};};}
# 791
struct _dyneither_ptr Cyc_memset(struct _dyneither_ptr s,char c,unsigned long n){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || n > _get_dyneither_size(s,sizeof(char)))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp84;_tmp84.tag=Cyc_Core_Invalid_argument;_tmp84.f1=({const char*_tmp85="memset";_tag_dyneither(_tmp85,sizeof(char),7);});_tmp84;});_tmp83;}));
 memset((void*)((char*)_untag_dyneither_ptr(s,sizeof(char),1)),(int)c,n);
return s;}
# 798
void Cyc_bzero(struct _dyneither_ptr s,unsigned long n){
if((char*)s.curr == (char*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(s,sizeof(char))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp87;_tmp87.tag=Cyc_Core_Invalid_argument;_tmp87.f1=({const char*_tmp88="bzero";_tag_dyneither(_tmp88,sizeof(char),6);});_tmp87;});_tmp86;}));
((void(*)(char*s,unsigned long n)) bzero)((char*)_untag_dyneither_ptr(s,sizeof(char),1),n);}
# 804
void Cyc__bcopy(struct _dyneither_ptr src,struct _dyneither_ptr dst,unsigned long n,unsigned int sz){
if((((void*)src.curr == (void*)(_tag_dyneither(0,0,0)).curr  || _get_dyneither_size(src,sizeof(void))< n) || (void*)dst.curr == (void*)(_tag_dyneither(0,0,0)).curr) || _get_dyneither_size(dst,sizeof(void))< n)
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Core_Invalid_argument_exn_struct _tmp8A;_tmp8A.tag=Cyc_Core_Invalid_argument;_tmp8A.f1=({const char*_tmp8B="bcopy";_tag_dyneither(_tmp8B,sizeof(char),6);});_tmp8A;});_tmp89;}));
 bcopy((const void*)_untag_dyneither_ptr(src,sizeof(void),1),(void*)_untag_dyneither_ptr(dst,sizeof(void),1),n * sz);}
