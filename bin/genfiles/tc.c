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
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;
# 205
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_region_key_t;
# 211
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_uregion_key_t;
# 216
typedef struct Cyc_Core_DynamicRegion*Cyc_Core_rcregion_key_t;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 295 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;
# 299
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 304
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 311
return 0;
return dyn.curr;};}
# 317
static unsigned int arr_prevsize(struct _dyneither_ptr arr,unsigned int elt_sz){
unsigned char*_get_arr_size_curr=arr.curr;
unsigned char*_get_arr_size_base=arr.base;
return
(_get_arr_size_curr < _get_arr_size_base  || _get_arr_size_curr >= arr.last_plus_one)?0:(_get_arr_size_curr - _get_arr_size_base)/ elt_sz;}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 319
int Cyc_List_memq(struct Cyc_List_List*l,void*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 394
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;struct Cyc_Relations_Reln;
# 69 "absyn.h"
typedef struct Cyc_Relations_Reln*Cyc_Relations_reln_t;
typedef struct Cyc_List_List*Cyc_Relations_relns_t;
# 74
typedef void*Cyc_Tcpat_decision_opt_t;
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 94
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 101
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 106
typedef enum Cyc_Absyn_Scope Cyc_Absyn_scope_t;
typedef struct Cyc_Absyn_Tqual Cyc_Absyn_tqual_t;
typedef enum Cyc_Absyn_Size_of Cyc_Absyn_size_of_t;
typedef struct Cyc_Absyn_Kind*Cyc_Absyn_kind_t;
typedef void*Cyc_Absyn_kindbound_t;
typedef struct Cyc_Absyn_Tvar*Cyc_Absyn_tvar_t;
typedef enum Cyc_Absyn_Sign Cyc_Absyn_sign_t;
typedef enum Cyc_Absyn_AggrKind Cyc_Absyn_aggr_kind_t;
typedef void*Cyc_Absyn_bounds_t;
typedef struct Cyc_Absyn_PtrAtts Cyc_Absyn_ptr_atts_t;
typedef struct Cyc_Absyn_PtrInfo Cyc_Absyn_ptr_info_t;
typedef struct Cyc_Absyn_VarargInfo Cyc_Absyn_vararg_info_t;
typedef struct Cyc_Absyn_FnInfo Cyc_Absyn_fn_info_t;
typedef struct Cyc_Absyn_DatatypeInfo Cyc_Absyn_datatype_info_t;
typedef struct Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_datatype_field_info_t;
typedef struct Cyc_Absyn_AggrInfo Cyc_Absyn_aggr_info_t;
typedef struct Cyc_Absyn_ArrayInfo Cyc_Absyn_array_info_t;
typedef void*Cyc_Absyn_type_t;typedef void*Cyc_Absyn_rgntype_t;typedef void*Cyc_Absyn_type_opt_t;
typedef union Cyc_Absyn_Cnst Cyc_Absyn_cnst_t;
typedef enum Cyc_Absyn_Primop Cyc_Absyn_primop_t;
typedef enum Cyc_Absyn_Incrementor Cyc_Absyn_incrementor_t;
typedef struct Cyc_Absyn_VarargCallInfo Cyc_Absyn_vararg_call_info_t;
typedef void*Cyc_Absyn_raw_exp_t;
typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_t;typedef struct Cyc_Absyn_Exp*Cyc_Absyn_exp_opt_t;
typedef void*Cyc_Absyn_raw_stmt_t;
typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_t;typedef struct Cyc_Absyn_Stmt*Cyc_Absyn_stmt_opt_t;
typedef void*Cyc_Absyn_raw_pat_t;
typedef struct Cyc_Absyn_Pat*Cyc_Absyn_pat_t;
typedef void*Cyc_Absyn_binding_t;
typedef struct Cyc_Absyn_Switch_clause*Cyc_Absyn_switch_clause_t;
typedef struct Cyc_Absyn_Fndecl*Cyc_Absyn_fndecl_t;
typedef struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_aggrdecl_t;
typedef struct Cyc_Absyn_Datatypefield*Cyc_Absyn_datatypefield_t;
typedef struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_datatypedecl_t;
typedef struct Cyc_Absyn_Typedefdecl*Cyc_Absyn_typedefdecl_t;
typedef struct Cyc_Absyn_Enumfield*Cyc_Absyn_enumfield_t;
typedef struct Cyc_Absyn_Enumdecl*Cyc_Absyn_enumdecl_t;
typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_t;typedef struct Cyc_Absyn_Vardecl*Cyc_Absyn_vardecl_opt_t;
typedef void*Cyc_Absyn_raw_decl_t;
typedef struct Cyc_Absyn_Decl*Cyc_Absyn_decl_t;
typedef void*Cyc_Absyn_designator_t;
typedef void*Cyc_Absyn_absyn_annot_t;
typedef void*Cyc_Absyn_attribute_t;
typedef struct Cyc_List_List*Cyc_Absyn_attributes_t;
typedef struct Cyc_Absyn_Aggrfield*Cyc_Absyn_aggrfield_t;
typedef void*Cyc_Absyn_offsetof_field_t;
typedef struct Cyc_Absyn_MallocInfo Cyc_Absyn_malloc_info_t;
typedef enum Cyc_Absyn_Coercion Cyc_Absyn_coercion_t;
typedef struct Cyc_Absyn_PtrLoc*Cyc_Absyn_ptrloc_t;
# 157
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 178
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 183
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 209
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 211
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 220
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 366
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 371
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 442 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 445
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 509
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 535
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 542
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 560
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 924
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 930
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 933
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 944
void*Cyc_Absyn_compress_kb(void*);
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 973
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 995
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1006
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1078
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1166
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
# 30 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 32
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 46
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 49
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 38 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 49
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 65
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 69
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 80
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 97 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(struct _RegionHandle*,unsigned int,struct Cyc_Absyn_Fndecl*);
# 107
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 111
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 114
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 185
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 56
void*Cyc_Tcutil_compress(void*t);
# 59
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 84
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 86
int Cyc_Tcutil_is_function_type(void*t);
# 95
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 99
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 130
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 144
void Cyc_Tcutil_explain_failure();
# 146
int Cyc_Tcutil_unify(void*,void*);
# 173
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 183
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 210 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 212
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 220
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 224
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 234
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 266
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 276
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 296
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 307
int Cyc_Tcutil_bits_only(void*t);
# 310
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 320
int Cyc_Tcutil_supports_default(void*);
# 331
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 335
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 355
int Cyc_Tcutil_is_array(void*t);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple10{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 35
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 34
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 38
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 41
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 36 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 61 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 64
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 66
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 74
void**Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 83
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);static char _tmp0[1]="";
# 38 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 42
int Cyc_Tc_aggressive_warn=0;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 44
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 50
(*_tmp1).f3=1;
return 1;}}
# 54
return 0;}
# 57
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att));({void*_tmp2[1]={& _tmp4};Cyc_Tcutil_terr(loc,({const char*_tmp3="%s has an out-of-range index";_tag_dyneither(_tmp3,sizeof(char),29);}),_tag_dyneither(_tmp2,sizeof(void*),1));});});
return 0;}
# 62
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 64
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 67
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_Int_pa_PrintArg_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(unsigned long)i;({struct Cyc_String_pa_PrintArg_struct _tmp8;_tmp8.tag=0;_tmp8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2);({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2);({void*_tmp5[3]={& _tmp7,& _tmp8,& _tmp9};Cyc_Tcutil_terr(loc,({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_dyneither(_tmp6,sizeof(char),54);}),_tag_dyneither(_tmp5,sizeof(void*),3));});});});});}
# 71
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle _tmpA=_new_region("temp");struct _RegionHandle*temp=& _tmpA;_push_region(temp);
{struct _tuple13 init_params=({struct _tuple13 _tmp3B;_tmp3B.f1=0;_tmp3B.f2=({const char*_tmp3C="initializes";_tag_dyneither(_tmp3C,sizeof(char),12);});_tmp3B;});
struct _tuple13 nolive_unique_params=({struct _tuple13 _tmp39;_tmp39.f1=0;_tmp39.f2=({const char*_tmp3A="noliveunique";_tag_dyneither(_tmp3A,sizeof(char),13);});_tmp39;});
struct _tuple13 noconsume_params=({struct _tuple13 _tmp37;_tmp37.f1=0;_tmp37.f2=({const char*_tmp38="noconsume";_tag_dyneither(_tmp38,sizeof(char),10);});_tmp37;});
void*_tmpB=Cyc_Tcutil_compress(t);void*_tmpC=_tmpB;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp35;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->tag == 9){_LL1: _tmp36=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).attributes;_tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).args;_LL2: {
# 78
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp35);
for(0;_tmp36 != 0;_tmp36=_tmp36->tl){
void*_tmpD=(void*)_tmp36->hd;void*_tmpE=_tmpD;int _tmp32;int _tmp31;int _tmp30;switch(*((int*)_tmpE)){case 20: _LL6: _tmp30=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpE)->f1;_LL7:
# 82
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp30,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,init_params,noconsume_params);{
struct _tuple8*_tmpF=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp30 - 1);struct _tuple8*_tmp10=_tmpF;void*_tmp25;_LLF: _tmp25=_tmp10->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp24="initializes attribute allowed only on";_tag_dyneither(_tmp24,sizeof(char),38);});
{void*_tmp11=Cyc_Tcutil_compress(_tmp25);void*_tmp12=_tmp11;void*_tmp22;union Cyc_Absyn_Constraint*_tmp21;union Cyc_Absyn_Constraint*_tmp20;union Cyc_Absyn_Constraint*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->tag == 5){_LL12: _tmp22=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).elt_typ;_tmp21=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).nullable;_tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).bounds;_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).zero_term;_LL13:
# 89
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp21))
({struct Cyc_String_pa_PrintArg_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp13[1]={& _tmp15};Cyc_Tcutil_terr(loc,({const char*_tmp14="%s non-null pointers";_tag_dyneither(_tmp14,sizeof(char),21);}),_tag_dyneither(_tmp13,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_bound_one(_tmp20))
({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp16[1]={& _tmp18};Cyc_Tcutil_terr(loc,({const char*_tmp17="%s pointers of size 1";_tag_dyneither(_tmp17,sizeof(char),22);}),_tag_dyneither(_tmp16,sizeof(void*),1));});});
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1F))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp19[1]={& _tmp1B};Cyc_Tcutil_terr(loc,({const char*_tmp1A="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp1A,sizeof(char),42);}),_tag_dyneither(_tmp19,sizeof(void*),1));});});
goto _LL11;}else{_LL14: _LL15:
# 97
({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1C[1]={& _tmp1E};Cyc_Tcutil_terr(loc,({const char*_tmp1D="%s pointers";_tag_dyneither(_tmp1D,sizeof(char),12);}),_tag_dyneither(_tmp1C,sizeof(void*),1));});});}_LL11:;}
# 99
init_params.f1=({struct Cyc_List_List*_tmp23=_region_malloc(temp,sizeof(*_tmp23));_tmp23->hd=(void*)_tmp30;_tmp23->tl=init_params.f1;_tmp23;});
goto _LL5;};};case 21: _LL8: _tmp31=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpE)->f1;_LL9:
# 102
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp31,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp26=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp31 - 1);struct _tuple8*_tmp27=_tmp26;void*_tmp2B;_LL17: _tmp2B=_tmp27->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2B,0))
({void*_tmp28=0;Cyc_Tcutil_terr(loc,({const char*_tmp29="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp29,sizeof(char),55);}),_tag_dyneither(_tmp28,sizeof(void*),0));});
nolive_unique_params.f1=({struct Cyc_List_List*_tmp2A=_region_malloc(temp,sizeof(*_tmp2A));_tmp2A->hd=(void*)_tmp31;_tmp2A->tl=nolive_unique_params.f1;_tmp2A;});
goto _LL5;};case 22: _LLA: _tmp32=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpE)->f1;_LLB:
# 111
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp32,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp2C=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp32 - 1);struct _tuple8*_tmp2D=_tmp2C;void*_tmp2F;_LL1A: _tmp2F=_tmp2D->f3;_LL1B:;
# 116
noconsume_params.f1=({struct Cyc_List_List*_tmp2E=_region_malloc(temp,sizeof(*_tmp2E));_tmp2E->hd=(void*)_tmp32;_tmp2E->tl=noconsume_params.f1;_tmp2E;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 121
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp33=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp34="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp34,sizeof(char),34);}),_tag_dyneither(_tmp33,sizeof(void*),0));});}_LL0:;}
# 73
;_pop_region(temp);}struct _tuple14{void*f1;int f2;};
# 127
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 129
struct Cyc_Absyn_Vardecl*_tmp3D=vd;enum Cyc_Absyn_Scope _tmp9D;struct _tuple0*_tmp9C;void*_tmp9B;struct Cyc_Absyn_Exp*_tmp9A;struct Cyc_List_List*_tmp99;_LL1D: _tmp9D=_tmp3D->sc;_tmp9C=_tmp3D->name;_tmp9B=_tmp3D->type;_tmp9A=_tmp3D->initializer;_tmp99=_tmp3D->attributes;_LL1E:;
# 136
{void*_tmp3E=Cyc_Tcutil_compress(_tmp9B);void*_tmp3F=_tmp3E;void*_tmp4B;struct Cyc_Absyn_Tqual _tmp4A;union Cyc_Absyn_Constraint*_tmp49;unsigned int _tmp48;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->tag == 8){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).num_elts == 0){_LL20: _tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).tq;_tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;_tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zt_loc;if(_tmp9A != 0){_LL21:
# 138
{void*_tmp40=_tmp9A->r;void*_tmp41=_tmp40;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct _dyneither_ptr _tmp43;struct _dyneither_ptr _tmp42;switch(*((int*)_tmp41)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).Wstring_c).tag){case 8: _LL25: _tmp42=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).String_c).val;_LL26:
# 140
 _tmp9B=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp42,sizeof(char)),0),_tmp49,_tmp48));
goto _LL24;case 9: _LL27: _tmp43=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).Wstring_c).val;_LL28:
# 143
 _tmp9B=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp(1,0),_tmp49,_tmp48));
goto _LL24;default: goto _LL31;}case 26: _LL29: _tmp44=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL2A:
 _tmp45=_tmp44;goto _LL2C;case 27: _LL2B: _tmp45=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_LL2C:
# 148
 _tmp9B=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,_tmp45,_tmp49,_tmp48));
goto _LL24;case 35: _LL2D: _tmp46=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL2E:
 _tmp47=_tmp46;goto _LL30;case 25: _LL2F: _tmp47=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_LL30:
# 152
 _tmp9B=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp47),0),_tmp49,_tmp48));
goto _LL24;default: _LL31: _LL32:
 goto _LL24;}_LL24:;}
# 156
goto _LL1F;}else{goto _LL22;}}else{goto _LL22;}}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 160
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp9B);
# 162
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp9B);
# 165
vd->escapes=!Cyc_Tcutil_is_array(_tmp9B);
# 167
if(Cyc_Tcutil_is_function_type(_tmp9B)){
_tmp99=Cyc_Tcutil_transfer_fn_type_atts(_tmp9B,_tmp99);
Cyc_Tc_fnTypeAttsOK(te,_tmp9B,loc);}
# 172
if(_tmp9D == Cyc_Absyn_Extern  || _tmp9D == Cyc_Absyn_ExternC){
if(_tmp9A != 0  && !in_cinclude)
({void*_tmp4C=0;Cyc_Tcutil_terr(loc,({const char*_tmp4D="extern declaration should not have initializer";_tag_dyneither(_tmp4D,sizeof(char),47);}),_tag_dyneither(_tmp4C,sizeof(void*),0));});}else{
if(!Cyc_Tcutil_is_function_type(_tmp9B)){
# 179
for(0;_tmp99 != 0;_tmp99=_tmp99->tl){
void*_tmp4E=(void*)_tmp99->hd;void*_tmp4F=_tmp4E;switch(*((int*)_tmp4F)){case 6: _LL34: _LL35:
 goto _LL37;case 8: _LL36: _LL37:
# 184
 goto _LL39;case 9: _LL38: _LL39:
 goto _LL3B;case 10: _LL3A: _LL3B:
 goto _LL3D;case 11: _LL3C: _LL3D:
 goto _LL3F;case 12: _LL3E: _LL3F:
 goto _LL41;case 13: _LL40: _LL41:
 goto _LL43;case 14: _LL42: _LL43:
 continue;default: _LL44: _LL45:
# 192
({struct Cyc_String_pa_PrintArg_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name));({struct Cyc_String_pa_PrintArg_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp99->hd));({void*_tmp50[2]={& _tmp52,& _tmp53};Cyc_Tcutil_terr(loc,({const char*_tmp51="bad attribute %s for variable %s";_tag_dyneither(_tmp51,sizeof(char),33);}),_tag_dyneither(_tmp50,sizeof(void*),2));});});});
goto _LL33;}_LL33:;}
# 197
if(_tmp9A == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp9B))
({struct Cyc_String_pa_PrintArg_struct _tmp57;_tmp57.tag=0;_tmp57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp9B));({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));({void*_tmp54[2]={& _tmp56,& _tmp57};Cyc_Tcutil_terr(loc,({const char*_tmp55="initializer required for variable %s of type %s";_tag_dyneither(_tmp55,sizeof(char),48);}),_tag_dyneither(_tmp54,sizeof(void*),2));});});});}else{
# 202
struct Cyc_Absyn_Exp*_tmp58=_tmp9A;
# 206
struct _handler_cons _tmp59;_push_handler(& _tmp59);{int _tmp5B=0;if(setjmp(_tmp59.handler))_tmp5B=1;if(!_tmp5B){
{void*_tmp5C=Cyc_Tcexp_tcExpInitializer(te,& _tmp9B,_tmp58);
if(!Cyc_Tcutil_coerce_assign(te,_tmp58,_tmp9B)){
struct _dyneither_ptr _tmp5D=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp5E=" declared with type ";
struct _dyneither_ptr _tmp5F=Cyc_Absynpp_typ2string(_tmp9B);
const char*_tmp60=" but initialized with type ";
struct _dyneither_ptr _tmp61=Cyc_Absynpp_typ2string(_tmp5C);
if((((Cyc_strlen((struct _dyneither_ptr)_tmp5D)+ Cyc_strlen(({const char*_tmp62=_tmp5E;_tag_dyneither(_tmp62,sizeof(char),_get_zero_arr_size_char((void*)_tmp62,21));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp5F))+ Cyc_strlen(({const char*_tmp63=_tmp60;_tag_dyneither(_tmp63,sizeof(char),_get_zero_arr_size_char((void*)_tmp63,28));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp61)> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp61);({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dyneither_ptr)({const char*_tmp6C=_tmp60;_tag_dyneither(_tmp6C,sizeof(char),_get_zero_arr_size_char((void*)_tmp6C,28));});({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);({struct Cyc_String_pa_PrintArg_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _dyneither_ptr)({const char*_tmp6B=_tmp5E;_tag_dyneither(_tmp6B,sizeof(char),_get_zero_arr_size_char((void*)_tmp6B,21));});({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);({void*_tmp64[5]={& _tmp66,& _tmp67,& _tmp68,& _tmp69,& _tmp6A};Cyc_Tcutil_terr(loc,({const char*_tmp65="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp65,sizeof(char),16);}),_tag_dyneither(_tmp64,sizeof(void*),5));});});});});});});else{
# 217
({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp61);({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)({const char*_tmp75=_tmp60;_tag_dyneither(_tmp75,sizeof(char),_get_zero_arr_size_char((void*)_tmp75,28));});({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(struct _dyneither_ptr)({const char*_tmp74=_tmp5E;_tag_dyneither(_tmp74,sizeof(char),_get_zero_arr_size_char((void*)_tmp74,21));});({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);({void*_tmp6D[5]={& _tmp6F,& _tmp70,& _tmp71,& _tmp72,& _tmp73};Cyc_Tcutil_terr(loc,({const char*_tmp6E="%s%s%s%s%s";_tag_dyneither(_tmp6E,sizeof(char),11);}),_tag_dyneither(_tmp6D,sizeof(void*),5));});});});});});});}
Cyc_Tcutil_explain_failure();
# 220
if(!Cyc_Tcutil_is_const_exp(_tmp58))
({void*_tmp76=0;Cyc_Tcutil_terr(loc,({const char*_tmp77="initializer is not a constant expression";_tag_dyneither(_tmp77,sizeof(char),41);}),_tag_dyneither(_tmp76,sizeof(void*),0));});}}
# 207
;_pop_handler();}else{void*_tmp5A=(void*)_exn_thrown;void*_tmp78=_tmp5A;void*_tmp7B;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp78)->tag == Cyc_Tcenv_Env_error){_LL47: _LL48:
# 225
({void*_tmp79=0;Cyc_Tcutil_terr(loc,({const char*_tmp7A="initializer is not a constant expression";_tag_dyneither(_tmp7A,sizeof(char),41);}),_tag_dyneither(_tmp79,sizeof(void*),0));});
goto _LL46;}else{_LL49: _tmp7B=_tmp78;_LL4A:(int)_rethrow(_tmp7B);}_LL46:;}};}}else{
# 231
for(0;_tmp99 != 0;_tmp99=_tmp99->tl){
void*_tmp7C=(void*)_tmp99->hd;void*_tmp7D=_tmp7C;switch(*((int*)_tmp7D)){case 0: _LL4C: _LL4D:
# 234
 goto _LL4F;case 1: _LL4E: _LL4F:
 goto _LL51;case 2: _LL50: _LL51:
 goto _LL53;case 3: _LL52: _LL53:
 goto _LL55;case 4: _LL54: _LL55:
 goto _LL57;case 19: _LL56: _LL57:
 goto _LL59;case 20: _LL58: _LL59:
 goto _LL5B;case 23: _LL5A: _LL5B:
 goto _LL5D;case 26: _LL5C: _LL5D:
 goto _LL5F;case 5: _LL5E: _LL5F:
({void*_tmp7E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7F="tcVardecl: fn type atts in function var decl";_tag_dyneither(_tmp7F,sizeof(char),45);}),_tag_dyneither(_tmp7E,sizeof(void*),0));});case 6: _LL60: _LL61:
# 245
 goto _LL63;case 7: _LL62: _LL63:
# 247
({struct Cyc_String_pa_PrintArg_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp99->hd));({void*_tmp80[1]={& _tmp82};Cyc_Tcutil_terr(loc,({const char*_tmp81="bad attribute %s in function declaration";_tag_dyneither(_tmp81,sizeof(char),41);}),_tag_dyneither(_tmp80,sizeof(void*),1));});});
goto _LL4B;default: _LL64: _LL65:
 continue;}_LL4B:;}}}
# 255
{struct _handler_cons _tmp83;_push_handler(& _tmp83);{int _tmp85=0;if(setjmp(_tmp83.handler))_tmp85=1;if(!_tmp85){
{struct _tuple14*_tmp86=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,_tmp9C);
void*_tmp87=(*_tmp86).f1;void*_tmp88=_tmp87;void*_tmp91;switch(*((int*)_tmp88)){case 0: _LL67: _tmp91=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmp88)->f1;_LL68: {
# 259
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp89=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp90;_tmp90.tag=1;_tmp90.f1=vd;_tmp90;});_tmp8F;});
void**_tmp8A=Cyc_Tcdecl_merge_binding(_tmp91,(void*)_tmp89,loc,Cyc_Tc_tc_msg);
if(_tmp8A == 0){_npop_handler(0);return;}{
void*_tmp8B=*_tmp8A;
# 264
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp8B != _tmp91  || (*_tmp86).f2)
# 267
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp9C,({struct _tuple14*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp8E;_tmp8E.tag=0;_tmp8E.f1=_tmp8B;_tmp8E;});_tmp8D;});_tmp8C->f2=1;_tmp8C;}));}
# 269
_npop_handler(0);return;};}case 1: _LL69: _LL6A:
 goto _LL6C;case 2: _LL6B: _LL6C:
 goto _LL6E;case 4: _LL6D: _LL6E:
 goto _LL70;default: _LL6F: _LL70:
 goto _LL66;}_LL66:;}
# 256
;_pop_handler();}else{void*_tmp84=(void*)_exn_thrown;void*_tmp92=_tmp84;void*_tmp93;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp92)->tag == Cyc_Dict_Absent){_LL72: _LL73:
# 275
 goto _LL71;}else{_LL74: _tmp93=_tmp92;_LL75:(int)_rethrow(_tmp93);}_LL71:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp9C,({struct _tuple14*_tmp94=_cycalloc(sizeof(*_tmp94));_tmp94->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmp95=_cycalloc(sizeof(*_tmp95));_tmp95[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmp96;_tmp96.tag=0;_tmp96.f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp98;_tmp98.tag=1;_tmp98.f1=vd;_tmp98;});_tmp97;});_tmp96;});_tmp95;});_tmp94->f2=0;_tmp94;}));}
# 281
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp9E=n;union Cyc_Absyn_Nmspace _tmpA2;struct _dyneither_ptr*_tmpA1;_LL77: _tmpA2=_tmp9E->f1;_tmpA1=_tmp9E->f2;_LL78:;{
union Cyc_Absyn_Nmspace _tmp9F=_tmpA2;if((_tmp9F.Abs_n).tag == 2){if((_tmp9F.Abs_n).val == 0){_LL7A: _LL7B:
# 285
 return Cyc_strcmp((struct _dyneither_ptr)*_tmpA1,({const char*_tmpA0="main";_tag_dyneither(_tmpA0,sizeof(char),5);}))== 0;}else{goto _LL7C;}}else{_LL7C: _LL7D:
 return 0;}_LL79:;};}
# 290
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 292
struct _tuple0*q=fd->name;
# 296
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include)
({void*_tmpA3=0;Cyc_Tcutil_terr(loc,({const char*_tmpA4="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmpA4,sizeof(char),54);}),_tag_dyneither(_tmpA3,sizeof(void*),0));});
# 300
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 302
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 304
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 307
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmpA5=(void*)atts->hd;void*_tmpA6=_tmpA5;switch(*((int*)_tmpA6)){case 7: _LL7F: _LL80:
 goto _LL82;case 6: _LL81: _LL82:
# 311
({struct Cyc_String_pa_PrintArg_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmpA7[1]={& _tmpA9};Cyc_Tcutil_terr(loc,({const char*_tmpA8="bad attribute %s for function";_tag_dyneither(_tmpA8,sizeof(char),30);}),_tag_dyneither(_tmpA7,sizeof(void*),1));});});
goto _LL7E;default: _LL83: _LL84:
 goto _LL7E;}_LL7E:;}}
# 317
{struct _handler_cons _tmpAA;_push_handler(& _tmpAA);{int _tmpAC=0;if(setjmp(_tmpAA.handler))_tmpAC=1;if(!_tmpAC){
{struct _tuple14*_tmpAD=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
void*_tmpAE=(*_tmpAD).f1;void*_tmpAF=_tmpAE;void*_tmpB8;switch(*((int*)_tmpAF)){case 0: _LL86: _tmpB8=(void*)((struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*)_tmpAF)->f1;_LL87: {
# 321
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB0=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB6=_cycalloc(sizeof(*_tmpB6));_tmpB6[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpB7;_tmpB7.tag=2;_tmpB7.f1=fd;_tmpB7;});_tmpB6;});
void**_tmpB1=Cyc_Tcdecl_merge_binding(_tmpB8,(void*)_tmpB0,loc,Cyc_Tc_tc_msg);
if(_tmpB1 == 0)goto _LL85;{
void*_tmpB2=*_tmpB1;
# 326
if(exports == 0  || Cyc_Tc_export_member(q,*exports)){
if(!(_tmpB2 == _tmpB8  && (*_tmpAD).f2))
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,q,({struct _tuple14*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=_tmpB2;_tmpB5;});_tmpB4;});_tmpB3->f2=1;_tmpB3;}));}
# 330
goto _LL85;};}case 1: _LL88: _LL89:
 goto _LL8B;case 2: _LL8A: _LL8B:
 goto _LL8D;case 4: _LL8C: _LL8D:
 goto _LL8F;default: _LL8E: _LL8F:
 goto _LL85;}_LL85:;}
# 318
;_pop_handler();}else{void*_tmpAB=(void*)_exn_thrown;void*_tmpB9=_tmpAB;void*_tmpC2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpB9)->tag == Cyc_Dict_Absent){_LL91: _LL92:
# 337
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBA=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpC1;_tmpC1.tag=2;_tmpC1.f1=fd;_tmpC1;});_tmpC0;});
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,q,({struct _tuple14*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->f1=(void*)({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=({struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct _tmpBD;_tmpBD.tag=0;_tmpBD.f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpBF;_tmpBF.tag=2;_tmpBF.f1=fd;_tmpBF;});_tmpBE;});_tmpBD;});_tmpBC;});_tmpBB->f2=0;_tmpBB;}));}
# 342
goto _LL90;}else{_LL93: _tmpC2=_tmpB9;_LL94:(int)_rethrow(_tmpC2);}_LL90:;}};}
# 346
if(te->in_extern_c_include)return;
# 351
{struct _RegionHandle _tmpC3=_new_region("fnrgn");struct _RegionHandle*fnrgn=& _tmpC3;_push_region(fnrgn);
{struct Cyc_Tcenv_Fenv*_tmpC4=Cyc_Tcenv_new_fenv(fnrgn,loc,fd);
struct Cyc_Tcenv_Tenv*_tmpC5=({struct Cyc_Tcenv_Tenv*_tmpC6=_region_malloc(fnrgn,sizeof(*_tmpC6));_tmpC6->ns=te->ns;_tmpC6->ae=te->ae;_tmpC6->le=_tmpC4;_tmpC6->allow_valueof=0;_tmpC6->in_extern_c_include=0;_tmpC6;});
# 355
Cyc_Tcstmt_tcStmt(_tmpC5,fd->body,0);
# 358
Cyc_Tcenv_check_delayed_effects(_tmpC5);
Cyc_Tcenv_check_delayed_constraints(_tmpC5);}
# 352
;_pop_region(fnrgn);}
# 362
if(Cyc_Tc_is_main(q)){
# 364
{void*_tmpC7=Cyc_Tcutil_compress(fd->ret_type);void*_tmpC8=_tmpC7;enum Cyc_Absyn_Size_of _tmpCE;switch(*((int*)_tmpC8)){case 0: _LL96: _LL97:
({void*_tmpC9=0;Cyc_Tcutil_warn(loc,({const char*_tmpCA="main declared with return type void";_tag_dyneither(_tmpCA,sizeof(char),36);}),_tag_dyneither(_tmpC9,sizeof(void*),0));});goto _LL95;case 6: _LL98: _tmpCE=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC8)->f2;_LL99:
 goto _LL95;default: _LL9A: _LL9B:
# 368
({struct Cyc_String_pa_PrintArg_struct _tmpCD;_tmpCD.tag=0;_tmpCD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type));({void*_tmpCB[1]={& _tmpCD};Cyc_Tcutil_terr(loc,({const char*_tmpCC="main declared with return type %s instead of int or void";_tag_dyneither(_tmpCC,sizeof(char),57);}),_tag_dyneither(_tmpCB,sizeof(void*),1));});});
goto _LL95;}_LL95:;}
# 372
if(fd->c_varargs  || fd->cyc_varargs != 0)
({void*_tmpCF=0;Cyc_Tcutil_terr(loc,({const char*_tmpD0="main declared with varargs";_tag_dyneither(_tmpD0,sizeof(char),27);}),_tag_dyneither(_tmpCF,sizeof(void*),0));});{
struct Cyc_List_List*_tmpD1=fd->args;
if(_tmpD1 != 0){
struct _tuple8*_tmpD2=(struct _tuple8*)_tmpD1->hd;struct _tuple8*_tmpD3=_tmpD2;void*_tmpEB;_LL9D: _tmpEB=_tmpD3->f3;_LL9E:;
{void*_tmpD4=Cyc_Tcutil_compress(_tmpEB);void*_tmpD5=_tmpD4;enum Cyc_Absyn_Size_of _tmpD9;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD5)->tag == 6){_LLA0: _tmpD9=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpD5)->f2;_LLA1:
 goto _LL9F;}else{_LLA2: _LLA3:
# 380
({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0;_tmpD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpEB));({void*_tmpD6[1]={& _tmpD8};Cyc_Tcutil_terr(loc,({const char*_tmpD7="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpD7,sizeof(char),60);}),_tag_dyneither(_tmpD6,sizeof(void*),1));});});
goto _LL9F;}_LL9F:;}
# 384
_tmpD1=_tmpD1->tl;
if(_tmpD1 != 0){
struct _tuple8*_tmpDA=(struct _tuple8*)_tmpD1->hd;struct _tuple8*_tmpDB=_tmpDA;void*_tmpEA;_LLA5: _tmpEA=_tmpDB->f3;_LLA6:;
_tmpD1=_tmpD1->tl;
if(_tmpD1 != 0)
({void*_tmpDC=0;Cyc_Tcutil_terr(loc,({const char*_tmpDD="main declared with too many arguments";_tag_dyneither(_tmpDD,sizeof(char),38);}),_tag_dyneither(_tmpDC,sizeof(void*),0));});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->v=fd->tvs;_tmpE9;});
if(((!Cyc_Tcutil_unify(_tmpEA,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->v=& Cyc_Tcutil_rk;_tmpDE;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->v=& Cyc_Tcutil_rk;_tmpDF;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmpEA,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->v=& Cyc_Tcutil_rk;_tmpE0;}),tvs)),
# 396
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=& Cyc_Tcutil_rk;_tmpE1;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpEA,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->v=& Cyc_Tcutil_rk;_tmpE2;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->v=& Cyc_Tcutil_rk;_tmpE3;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpEA,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->v=& Cyc_Tcutil_rk;_tmpE4;}),tvs)),
# 403
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->v=& Cyc_Tcutil_rk;_tmpE5;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
({struct Cyc_String_pa_PrintArg_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpEA));({void*_tmpE6[1]={& _tmpE8};Cyc_Tcutil_terr(loc,({const char*_tmpE7="second argument of main has type %s instead of char??";_tag_dyneither(_tmpE7,sizeof(char),54);}),_tag_dyneither(_tmpE6,sizeof(void*),1));});});};}}};}};}
# 413
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 420
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmpEC[1]={& _tmpEE};Cyc_Tcutil_terr(loc,({const char*_tmpED="redeclaration of typedef %s";_tag_dyneither(_tmpED,sizeof(char),28);}),_tag_dyneither(_tmpEC,sizeof(void*),1));});});
return;}
# 425
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 434
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpEF=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmpF0=_tmpEF;struct Cyc_Core_Opt**_tmpF8;struct Cyc_Absyn_Kind*_tmpF7;struct Cyc_Core_Opt**_tmpF6;switch(*((int*)_tmpF0)){case 1: _LLA8: _tmpF6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpF0)->f1;_LLA9:
# 437
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpF3;_tmpF3.tag=0;_tmpF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);({void*_tmpF1[1]={& _tmpF3};Cyc_Tcutil_warn(loc,({const char*_tmpF2="type variable %s is not used in typedef definition";_tag_dyneither(_tmpF2,sizeof(char),51);}),_tag_dyneither(_tmpF1,sizeof(void*),1));});});
# 440
*_tmpF6=({struct Cyc_Core_Opt*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpF4;});goto _LLA7;case 2: _LLAA: _tmpF8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF0)->f1;_tmpF7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpF0)->f2;_LLAB:
# 447
*_tmpF8=({struct Cyc_Core_Opt*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5->v=Cyc_Tcutil_kind_to_bound(_tmpF7);_tmpF5;});
goto _LLA7;default: _LLAC: _LLAD:
 continue;}_LLA7:;}}
# 453
(te->ae)->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);}
# 456
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 463
struct _RegionHandle _tmpF9=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpF9;_push_region(uprev_rgn);
# 465
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmpFA=(struct _tuple10*)rpo->hd;struct _tuple10*_tmpFB=_tmpFA;void*_tmpFD;void*_tmpFC;_LLAF: _tmpFD=_tmpFB->f1;_tmpFC=_tmpFB->f2;_LLB0:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmpFD);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmpFC);}{
# 471
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 474
struct Cyc_List_List*_tmpFE=fields;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct Cyc_Absyn_Aggrfield*_tmpFF=(struct Cyc_Absyn_Aggrfield*)_tmpFE->hd;struct Cyc_Absyn_Aggrfield*_tmp100=_tmpFF;struct _dyneither_ptr*_tmp11D;struct Cyc_Absyn_Tqual _tmp11C;void*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_List_List*_tmp119;struct Cyc_Absyn_Exp*_tmp118;_LLB2: _tmp11D=_tmp100->name;_tmp11C=_tmp100->tq;_tmp11B=_tmp100->type;_tmp11A=_tmp100->width;_tmp119=_tmp100->attributes;_tmp118=_tmp100->requires_clause;_LLB3:;
# 477
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp11D))
({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0;_tmp103.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp11D);({void*_tmp101[1]={& _tmp103};Cyc_Tcutil_terr(loc,({const char*_tmp102="duplicate member %s";_tag_dyneither(_tmp102,sizeof(char),20);}),_tag_dyneither(_tmp101,sizeof(void*),1));});});
# 481
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp11D,({const char*_tmp104="";_tag_dyneither(_tmp104,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp105=_region_malloc(uprev_rgn,sizeof(*_tmp105));_tmp105->hd=_tmp11D;_tmp105->tl=prev_fields;_tmp105;});{
# 484
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 488
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmpFE->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp11B);
# 493
(((struct Cyc_Absyn_Aggrfield*)_tmpFE->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpFE->hd)->tq).print_const,_tmp11B);
# 496
Cyc_Tcutil_check_bitfield(loc,te,_tmp11B,_tmp11A,_tmp11D);
# 498
if((unsigned int)_tmp118){
if(str_or_union != Cyc_Absyn_UnionA)
({void*_tmp106=0;Cyc_Tcutil_terr(loc,({const char*_tmp107="@requires clauses are only allowed on union members";_tag_dyneither(_tmp107,sizeof(char),52);}),_tag_dyneither(_tmp106,sizeof(void*),0));});
{struct _RegionHandle _tmp108=_new_region("temp");struct _RegionHandle*temp=& _tmp108;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp109=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp109,0,_tmp118);}
# 502
;_pop_region(temp);}
# 505
if(!Cyc_Tcutil_is_integral(_tmp118))
({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0;_tmp10C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 508
Cyc_Absynpp_typ2string((void*)_check_null(_tmp118->topt)));({void*_tmp10A[1]={& _tmp10C};Cyc_Tcutil_terr(_tmp118->loc,({const char*_tmp10B="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp10B,sizeof(char),54);}),_tag_dyneither(_tmp10A,sizeof(void*),1));});});else{
# 510
Cyc_Tcutil_check_type(_tmp118->loc,te,tvs,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp10E;_tmp10E.tag=18;_tmp10E.f1=_tmp118;_tmp10E;});_tmp10D;}));{
# 512
struct Cyc_List_List*_tmp10F=Cyc_Relations_exp2relns(uprev_rgn,_tmp118);
# 519
if(!Cyc_Relations_consistent_relations(_tmp10F))
({void*_tmp110=0;Cyc_Tcutil_terr(_tmp118->loc,({const char*_tmp111="@requires clause may be unsatisfiable";_tag_dyneither(_tmp111,sizeof(char),38);}),_tag_dyneither(_tmp110,sizeof(void*),0));});
# 525
{struct Cyc_List_List*_tmp112=prev_relations;for(0;_tmp112 != 0;_tmp112=_tmp112->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp10F,(struct Cyc_List_List*)_tmp112->hd)))
({void*_tmp113=0;Cyc_Tcutil_terr(_tmp118->loc,({const char*_tmp114="@requires clause may overlap with previous clauses";_tag_dyneither(_tmp114,sizeof(char),51);}),_tag_dyneither(_tmp113,sizeof(void*),0));});}}
# 530
prev_relations=({struct Cyc_List_List*_tmp115=_region_malloc(uprev_rgn,sizeof(*_tmp115));_tmp115->hd=_tmp10F;_tmp115->tl=prev_relations;_tmp115;});};}}else{
# 533
if(prev_relations != 0)
({void*_tmp116=0;Cyc_Tcutil_terr(loc,({const char*_tmp117="if one field has a @requires clause, they all must";_tag_dyneither(_tmp117,sizeof(char),51);}),_tag_dyneither(_tmp116,sizeof(void*),0));});}};}};
# 465
;_pop_region(uprev_rgn);}
# 539
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 541
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp11E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp11F=_tmp11E;enum Cyc_Absyn_AliasQual _tmp134;struct Cyc_Core_Opt**_tmp133;struct Cyc_Absyn_Kind*_tmp132;struct Cyc_Core_Opt**_tmp131;struct Cyc_Core_Opt**_tmp130;enum Cyc_Absyn_AliasQual _tmp12F;struct Cyc_Core_Opt**_tmp12E;switch(*((int*)_tmp11F)){case 1: _LLB5: _tmp12E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp11F)->f1;_LLB6:
# 544
*_tmp12E=({struct Cyc_Core_Opt*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp120;});continue;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F)->f2)->kind){case Cyc_Absyn_MemKind: _LLB7: _tmp130=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F)->f1;_tmp12F=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F)->f2)->aliasqual;_LLB8:
# 546
 if(constrain_top_kind  && _tmp12F == Cyc_Absyn_Top)
*_tmp130=({struct Cyc_Core_Opt*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp122=_cycalloc_atomic(sizeof(*_tmp122));_tmp122->kind=Cyc_Absyn_BoxKind;_tmp122->aliasqual=Cyc_Absyn_Aliasable;_tmp122;}));_tmp121;});else{
# 549
*_tmp130=({struct Cyc_Core_Opt*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp124=_cycalloc_atomic(sizeof(*_tmp124));_tmp124->kind=Cyc_Absyn_BoxKind;_tmp124->aliasqual=_tmp12F;_tmp124;}));_tmp123;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F)->f2)->aliasqual == Cyc_Absyn_Top){_LLB9: _tmp131=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F)->f1;if(constrain_top_kind){_LLBA:
# 552
*_tmp131=({struct Cyc_Core_Opt*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp126=_cycalloc_atomic(sizeof(*_tmp126));_tmp126->kind=Cyc_Absyn_BoxKind;_tmp126->aliasqual=Cyc_Absyn_Aliasable;_tmp126;}));_tmp125;});
continue;}else{goto _LLBB;}}else{goto _LLBB;}default: _LLBB: _tmp133=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F)->f1;_tmp132=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp11F)->f2;_LLBC:
# 555
*_tmp133=({struct Cyc_Core_Opt*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->v=Cyc_Tcutil_kind_to_bound(_tmp132);_tmp127;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11F)->f1)->kind == Cyc_Absyn_MemKind){_LLBD: _tmp134=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp11F)->f1)->aliasqual;_LLBE:
# 557
({struct Cyc_String_pa_PrintArg_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp12D=_cycalloc_atomic(sizeof(*_tmp12D));_tmp12D->kind=Cyc_Absyn_MemKind;_tmp12D->aliasqual=_tmp134;_tmp12D;})));({struct Cyc_String_pa_PrintArg_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*n).f2);({void*_tmp128[3]={& _tmp12A,& _tmp12B,& _tmp12C};Cyc_Tcutil_terr(loc,({const char*_tmp129="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp129,sizeof(char),57);}),_tag_dyneither(_tmp128,sizeof(void*),3));});});});});
continue;}else{_LLBF: _LLC0:
 continue;}}_LLB4:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 565
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp135=ad->name;
# 572
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp136=(void*)atts->hd;void*_tmp137=_tmp136;switch(*((int*)_tmp137)){case 7: _LLC2: _LLC3:
 goto _LLC5;case 6: _LLC4: _LLC5:
 continue;default: _LLC6: _LLC7:
# 577
({struct Cyc_String_pa_PrintArg_struct _tmp13B;_tmp13B.tag=0;_tmp13B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp135).f2);({struct Cyc_String_pa_PrintArg_struct _tmp13A;_tmp13A.tag=0;_tmp13A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp138[2]={& _tmp13A,& _tmp13B};Cyc_Tcutil_terr(loc,({const char*_tmp139="bad attribute %s in %s definition";_tag_dyneither(_tmp139,sizeof(char),34);}),_tag_dyneither(_tmp138,sizeof(void*),2));});});});
goto _LLC1;}_LLC1:;}}{
# 582
struct Cyc_List_List*_tmp13C=ad->tvs;
# 585
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);
# 590
{struct _tuple15 _tmp13D=({struct _tuple15 _tmp15B;_tmp15B.f1=ad->impl;_tmp15B.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,_tmp135);_tmp15B;});struct _tuple15 _tmp13E=_tmp13D;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp159;struct Cyc_List_List*_tmp158;int _tmp157;struct Cyc_Absyn_Aggrdecl**_tmp156;struct Cyc_List_List*_tmp155;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp153;int _tmp152;struct Cyc_Absyn_Aggrdecl**_tmp151;if(_tmp13E.f1 == 0){if(_tmp13E.f2 == 0){_LLC9: _LLCA:
# 593
 Cyc_Tc_rule_out_memkind(loc,_tmp135,_tmp13C,0);
# 595
(te->ae)->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp135,({struct Cyc_Absyn_Aggrdecl**_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F[0]=ad;_tmp13F;}));
goto _LLC8;}else{_LLCF: _tmp151=*_tmp13E.f2;_LLD0: {
# 655
struct Cyc_Absyn_Aggrdecl*_tmp150=Cyc_Tcdecl_merge_aggrdecl(*_tmp151,ad,loc,Cyc_Tc_tc_msg);
if(_tmp150 == 0)
return;else{
# 659
Cyc_Tc_rule_out_memkind(loc,_tmp135,_tmp13C,0);
# 662
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp135,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 665
*_tmp151=_tmp150;
ad=_tmp150;
goto _LLC8;}}}}else{if(_tmp13E.f2 == 0){_LLCB: _tmp155=(_tmp13E.f1)->exist_vars;_tmp154=(_tmp13E.f1)->rgn_po;_tmp153=(_tmp13E.f1)->fields;_tmp152=(_tmp13E.f1)->tagged;_LLCC: {
# 600
struct Cyc_Absyn_Aggrdecl**_tmp140=({struct Cyc_Absyn_Aggrdecl**_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148[0]=({struct Cyc_Absyn_Aggrdecl*_tmp149=_cycalloc(sizeof(*_tmp149));_tmp149->kind=ad->kind;_tmp149->sc=Cyc_Absyn_Extern;_tmp149->name=ad->name;_tmp149->tvs=_tmp13C;_tmp149->impl=0;_tmp149->attributes=ad->attributes;_tmp149->expected_mem_kind=0;_tmp149;});_tmp148;});
# 602
(te->ae)->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp135,_tmp140);
# 607
Cyc_Tcutil_check_unique_tvars(loc,_tmp155);
Cyc_Tcutil_add_tvar_identities(_tmp155);
# 611
if(_tmp152  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp141=0;Cyc_Tcutil_terr(loc,({const char*_tmp142="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp142,sizeof(char),56);}),_tag_dyneither(_tmp141,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp13C,_tmp155),_tmp154,_tmp153);
# 615
Cyc_Tc_rule_out_memkind(loc,_tmp135,_tmp13C,0);
# 618
Cyc_Tc_rule_out_memkind(loc,_tmp135,_tmp155,1);
# 620
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp152){
# 623
struct Cyc_List_List*f=_tmp153;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0;_tmp147.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));({struct Cyc_String_pa_PrintArg_struct _tmp146;_tmp146.tag=0;_tmp146.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp135).f2);({struct Cyc_String_pa_PrintArg_struct _tmp145;_tmp145.tag=0;_tmp145.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);({void*_tmp143[3]={& _tmp145,& _tmp146,& _tmp147};Cyc_Tcutil_warn(loc,({const char*_tmp144="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp144,sizeof(char),98);}),_tag_dyneither(_tmp143,sizeof(void*),3));});});});});}}
# 629
*_tmp140=ad;
goto _LLC8;}}else{_LLCD: _tmp15A=(_tmp13E.f1)->exist_vars;_tmp159=(_tmp13E.f1)->rgn_po;_tmp158=(_tmp13E.f1)->fields;_tmp157=(_tmp13E.f1)->tagged;_tmp156=*_tmp13E.f2;_LLCE:
# 633
 if(ad->kind != (*_tmp156)->kind)
({void*_tmp14A=0;Cyc_Tcutil_terr(loc,({const char*_tmp14B="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp14B,sizeof(char),52);}),_tag_dyneither(_tmp14A,sizeof(void*),0));});{
# 636
struct Cyc_Absyn_Aggrdecl*_tmp14C=*_tmp156;
# 638
*_tmp156=({struct Cyc_Absyn_Aggrdecl*_tmp14D=_cycalloc(sizeof(*_tmp14D));_tmp14D->kind=ad->kind;_tmp14D->sc=Cyc_Absyn_Extern;_tmp14D->name=ad->name;_tmp14D->tvs=_tmp13C;_tmp14D->impl=0;_tmp14D->attributes=ad->attributes;_tmp14D->expected_mem_kind=0;_tmp14D;});
# 644
Cyc_Tcutil_check_unique_tvars(loc,_tmp15A);
Cyc_Tcutil_add_tvar_identities(_tmp15A);
# 647
if(_tmp157  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp14E=0;Cyc_Tcutil_terr(loc,({const char*_tmp14F="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp14F,sizeof(char),56);}),_tag_dyneither(_tmp14E,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp13C,_tmp15A),_tmp159,_tmp158);
# 651
*_tmp156=_tmp14C;
_tmp151=_tmp156;goto _LLD0;};}}_LLC8:;}
# 671
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp135,({struct _tuple14*_tmp15C=_cycalloc(sizeof(*_tmp15C));_tmp15C->f1=(void*)({struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D[0]=({struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct _tmp15E;_tmp15E.tag=1;_tmp15E.f1=ad;_tmp15E;});_tmp15D;});_tmp15C->f2=1;_tmp15C;}));};}
# 675
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp15F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp160=_tmp15F;enum Cyc_Absyn_AliasQual _tmp17D;enum Cyc_Absyn_KindQual _tmp17C;struct Cyc_Core_Opt**_tmp17B;enum Cyc_Absyn_KindQual _tmp17A;struct Cyc_Core_Opt**_tmp179;struct Cyc_Core_Opt**_tmp178;struct Cyc_Core_Opt**_tmp177;struct Cyc_Core_Opt**_tmp176;struct Cyc_Core_Opt**_tmp175;struct Cyc_Core_Opt**_tmp174;switch(*((int*)_tmp160)){case 1: _LLD2: _tmp174=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp160)->f1;_LLD3:
 _tmp175=_tmp174;goto _LLD5;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f2)->aliasqual){case Cyc_Absyn_Top: _LLD4: _tmp175=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f1;_LLD5:
# 680
 _tmp176=_tmp175;goto _LLD7;case Cyc_Absyn_Aliasable: _LLD6: _tmp176=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f1;_LLD7:
# 682
*_tmp176=({struct Cyc_Core_Opt*_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp161;});goto _LLD1;default: goto _LLE0;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f2)->aliasqual){case Cyc_Absyn_Top: _LLD8: _tmp177=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f1;_LLD9:
 _tmp178=_tmp177;goto _LLDB;case Cyc_Absyn_Aliasable: _LLDA: _tmp178=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f1;_LLDB:
# 685
*_tmp178=({struct Cyc_Core_Opt*_tmp162=_cycalloc(sizeof(*_tmp162));_tmp162->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp162;});goto _LLD1;default: goto _LLE0;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f2)->aliasqual){case Cyc_Absyn_Top: _LLDC: _tmp179=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f1;_LLDD:
# 687
*_tmp179=({struct Cyc_Core_Opt*_tmp163=_cycalloc(sizeof(*_tmp163));_tmp163->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp163;});goto _LLD1;case Cyc_Absyn_Unique: goto _LLE0;default: goto _LLE6;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f2)->aliasqual == Cyc_Absyn_Unique){_LLE0: _tmp17B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f1;_tmp17A=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp160)->f2)->kind;_LLE1:
# 691
 _tmp17C=_tmp17A;goto _LLE3;}else{goto _LLE6;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp160)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp160)->f1)->aliasqual){case Cyc_Absyn_Top: _LLDE: _LLDF:
# 689
({struct Cyc_String_pa_PrintArg_struct _tmp167;_tmp167.tag=0;_tmp167.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp166;_tmp166.tag=0;_tmp166.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp164[2]={& _tmp166,& _tmp167};Cyc_Tcutil_terr(loc,({const char*_tmp165="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp165,sizeof(char),57);}),_tag_dyneither(_tmp164,sizeof(void*),2));});});});
goto _LLD1;case Cyc_Absyn_Unique: goto _LLE2;default: goto _LLE6;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp160)->f1)->aliasqual == Cyc_Absyn_Unique){_LLE2: _tmp17C=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp160)->f1)->kind;_LLE3:
# 693
({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0;_tmp16C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 695
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp16D=_cycalloc_atomic(sizeof(*_tmp16D));_tmp16D->kind=_tmp17C;_tmp16D->aliasqual=Cyc_Absyn_Unique;_tmp16D;})));({struct Cyc_String_pa_PrintArg_struct _tmp16B;_tmp16B.tag=0;_tmp16B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp16A;_tmp16A.tag=0;_tmp16A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp168[3]={& _tmp16A,& _tmp16B,& _tmp16C};Cyc_Tcutil_terr(loc,({const char*_tmp169="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp169,sizeof(char),57);}),_tag_dyneither(_tmp168,sizeof(void*),3));});});});});goto _LLD1;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp160)->f1)->kind == Cyc_Absyn_MemKind){_LLE4: _tmp17D=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp160)->f1)->aliasqual;_LLE5:
# 697
({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0;_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 699
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp173=_cycalloc_atomic(sizeof(*_tmp173));_tmp173->kind=Cyc_Absyn_MemKind;_tmp173->aliasqual=_tmp17D;_tmp173;})));({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0;_tmp171.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0;_tmp170.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp16E[3]={& _tmp170,& _tmp171,& _tmp172};Cyc_Tcutil_terr(loc,({const char*_tmp16F="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp16F,sizeof(char),57);}),_tag_dyneither(_tmp16E,sizeof(void*),3));});});});});goto _LLD1;}else{_LLE6: _LLE7:
 goto _LLD1;}}}}_LLD1:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 705
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 713
{struct Cyc_List_List*_tmp17E=fields;for(0;_tmp17E != 0;_tmp17E=_tmp17E->tl){
struct Cyc_Absyn_Datatypefield*_tmp17F=(struct Cyc_Absyn_Datatypefield*)_tmp17E->hd;
struct Cyc_List_List*typs=_tmp17F->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp17F->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);{
# 718
struct _RegionHandle _tmp180=_new_region("temp");struct _RegionHandle*temp=& _tmp180;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp183;_tmp183.tag=0;_tmp183.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 722
Cyc_Absynpp_qvar2string(_tmp17F->name));({void*_tmp181[1]={& _tmp183};Cyc_Tcutil_terr(_tmp17F->loc,({const char*_tmp182="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp182,sizeof(char),51);}),_tag_dyneither(_tmp181,sizeof(void*),1));});});
# 724
((*((struct _tuple16*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp17F->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 718
;_pop_region(temp);};}}}{
# 730
struct Cyc_List_List*fields2;
if(is_extensible){
# 733
int _tmp184=1;
struct Cyc_List_List*_tmp185=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp184,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp184)
fields2=_tmp185;else{
# 738
fields2=0;}}else{
# 740
struct _RegionHandle _tmp186=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp186;_push_region(uprev_rgn);
# 742
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp187=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp187->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp18B;_tmp18B.tag=0;_tmp18B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);({struct Cyc_String_pa_PrintArg_struct _tmp18A;_tmp18A.tag=0;_tmp18A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp187->name).f2);({void*_tmp188[2]={& _tmp18A,& _tmp18B};Cyc_Tcutil_terr(_tmp187->loc,({const char*_tmp189="duplicate field name %s in %s";_tag_dyneither(_tmp189,sizeof(char),30);}),_tag_dyneither(_tmp188,sizeof(void*),2));});});});else{
# 748
prev_fields=({struct Cyc_List_List*_tmp18C=_region_malloc(uprev_rgn,sizeof(*_tmp18C));_tmp18C->hd=(*_tmp187->name).f2;_tmp18C->tl=prev_fields;_tmp18C;});}
# 750
if(_tmp187->sc != Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp187->name).f2);({void*_tmp18D[1]={& _tmp18F};Cyc_Tcutil_warn(loc,({const char*_tmp18E="ignoring scope of field %s";_tag_dyneither(_tmp18E,sizeof(char),27);}),_tag_dyneither(_tmp18D,sizeof(void*),1));});});
_tmp187->sc=Cyc_Absyn_Public;}}}
# 755
fields2=fields;}
# 742
;_pop_region(uprev_rgn);}
# 761
{struct Cyc_List_List*_tmp190=fields;for(0;_tmp190 != 0;_tmp190=_tmp190->tl){
struct Cyc_Absyn_Datatypefield*_tmp191=(struct Cyc_Absyn_Datatypefield*)_tmp190->hd;
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp191->name,({struct _tuple14*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->f1=(void*)({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*_tmp193=_cycalloc(sizeof(*_tmp193));_tmp193[0]=({struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct _tmp194;_tmp194.tag=2;_tmp194.f1=tudres;_tmp194.f2=_tmp191;_tmp194;});_tmp193;});_tmp192->f2=1;_tmp192;}));}}
# 767
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 770
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp1AD="@extensible datatype";_tag_dyneither(_tmp1AD,sizeof(char),21);}):({const char*_tmp1AE="datatype";_tag_dyneither(_tmp1AE,sizeof(char),9);});
# 777
struct Cyc_List_List*tvs=tud->tvs;
# 780
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 786
struct _RegionHandle _tmp195=_new_region("temp");struct _RegionHandle*temp=& _tmp195;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 797 "tc.cyc"
{struct _handler_cons _tmp196;_push_handler(& _tmp196);{int _tmp198=0;if(setjmp(_tmp196.handler))_tmp198=1;if(!_tmp198){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp199=_cycalloc_atomic(sizeof(*_tmp199));_tmp199[0]=({struct Cyc_Dict_Absent_exn_struct _tmp19A;_tmp19A.tag=Cyc_Dict_Absent;_tmp19A;});_tmp199;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 803
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 798
;_pop_handler();}else{void*_tmp197=(void*)_exn_thrown;void*_tmp19B=_tmp197;void*_tmp19E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp19B)->tag == Cyc_Dict_Absent){_LLE9: _LLEA: {
# 807
struct Cyc_Absyn_Datatypedecl***_tmp19C=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);
tud_opt=(unsigned int)_tmp19C?({struct Cyc_Absyn_Datatypedecl***_tmp19D=_region_malloc(temp,sizeof(*_tmp19D));_tmp19D[0]=*_tmp19C;_tmp19D;}): 0;
goto _LLE8;}}else{_LLEB: _tmp19E=_tmp19B;_LLEC:(int)_rethrow(_tmp19E);}_LLE8:;}};}{
# 814
struct _tuple17 _tmp19F=({struct _tuple17 _tmp1AC;_tmp1AC.f1=tud->fields;_tmp1AC.f2=tud_opt;_tmp1AC;});struct _tuple17 _tmp1A0=_tmp19F;struct Cyc_List_List**_tmp1AB;struct Cyc_Absyn_Datatypedecl**_tmp1AA;struct Cyc_List_List**_tmp1A9;struct Cyc_Absyn_Datatypedecl**_tmp1A8;if(_tmp1A0.f1 == 0){if(_tmp1A0.f2 == 0){_LLEE: _LLEF:
# 817
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
(te->ae)->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,({struct Cyc_Absyn_Datatypedecl**_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1[0]=tud;_tmp1A1;}));
goto _LLED;}else{_LLF4: _tmp1A8=*_tmp1A0.f2;_LLF5: {
# 850
struct Cyc_Absyn_Datatypedecl*_tmp1A7=Cyc_Tcdecl_merge_datatypedecl(*_tmp1A8,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(_tmp1A7 == 0){
_npop_handler(0);return;}else{
# 855
*_tmp1A8=_tmp1A7;
goto _LLED;}}}}else{if(_tmp1A0.f2 == 0){_LLF0: _tmp1A9=(struct Cyc_List_List**)&(_tmp1A0.f1)->v;_LLF1: {
# 822
struct Cyc_Absyn_Datatypedecl**_tmp1A2=({struct Cyc_Absyn_Datatypedecl**_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3[0]=({struct Cyc_Absyn_Datatypedecl*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->sc=Cyc_Absyn_Extern;_tmp1A4->name=tud->name;_tmp1A4->tvs=tvs;_tmp1A4->fields=0;_tmp1A4->is_extensible=tud->is_extensible;_tmp1A4;});_tmp1A3;});
# 824
(te->ae)->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,_tmp1A2);
# 827
*_tmp1A9=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp1A9,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*_tmp1A2=tud;
goto _LLED;}}else{_LLF2: _tmp1AB=(struct Cyc_List_List**)&(_tmp1A0.f1)->v;_tmp1AA=*_tmp1A0.f2;_LLF3: {
# 832
struct Cyc_Absyn_Datatypedecl*_tmp1A5=*_tmp1AA;
# 835
if((!tud->is_extensible  && (unsigned int)_tmp1A5) && _tmp1A5->is_extensible)
tud->is_extensible=1;
# 838
*_tmp1AA=({struct Cyc_Absyn_Datatypedecl*_tmp1A6=_cycalloc(sizeof(*_tmp1A6));_tmp1A6->sc=Cyc_Absyn_Extern;_tmp1A6->name=tud->name;_tmp1A6->tvs=tvs;_tmp1A6->fields=0;_tmp1A6->is_extensible=tud->is_extensible;_tmp1A6;});
# 842
*_tmp1AB=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp1AB,tvs,tud);
# 846
*_tmp1AA=_tmp1A5;
_tmp1A8=_tmp1AA;goto _LLF5;}}}_LLED:;};}
# 787 "tc.cyc"
;_pop_region(temp);};}
# 862 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 869
if(ed->fields != 0){struct _RegionHandle _tmp1AF=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp1AF;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1B0=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 875
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp1B0->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1B0->name).f2);({void*_tmp1B1[1]={& _tmp1B3};Cyc_Tcutil_terr(_tmp1B0->loc,({const char*_tmp1B2="duplicate enum constructor %s";_tag_dyneither(_tmp1B2,sizeof(char),30);}),_tag_dyneither(_tmp1B1,sizeof(void*),1));});});else{
# 878
prev_fields=({struct Cyc_List_List*_tmp1B4=_region_malloc(uprev_rgn,sizeof(*_tmp1B4));_tmp1B4->hd=(*_tmp1B0->name).f2;_tmp1B4->tl=prev_fields;_tmp1B4;});}
# 880
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,_tmp1B0->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B7;_tmp1B7.tag=0;_tmp1B7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1B0->name).f2);({void*_tmp1B5[1]={& _tmp1B7};Cyc_Tcutil_terr(_tmp1B0->loc,({const char*_tmp1B6="enum field name %s shadows global name";_tag_dyneither(_tmp1B6,sizeof(char),39);}),_tag_dyneither(_tmp1B5,sizeof(void*),1));});});
# 883
if(_tmp1B0->tag == 0){
_tmp1B0->tag=Cyc_Absyn_uint_exp(tag_count,_tmp1B0->loc);
++ tag_count;}else{
# 888
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1B0->tag))){
struct _tuple11 _tmp1B8=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1B0->tag));struct _tuple11 _tmp1B9=_tmp1B8;unsigned int _tmp1BB;int _tmp1BA;_LLF7: _tmp1BB=_tmp1B9.f1;_tmp1BA=_tmp1B9.f2;_LLF8:;
if(_tmp1BA)tag_count=_tmp1BB + 1;}}}}
# 870
;_pop_region(uprev_rgn);}
# 896
{struct _handler_cons _tmp1BC;_push_handler(& _tmp1BC);{int _tmp1BE=0;if(setjmp(_tmp1BC.handler))_tmp1BE=1;if(!_tmp1BE){
{struct Cyc_Absyn_Enumdecl**_tmp1BF=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);
struct Cyc_Absyn_Enumdecl*_tmp1C0=Cyc_Tcdecl_merge_enumdecl(*_tmp1BF,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1C0 == 0){_npop_handler(0);return;}
*_tmp1BF=_tmp1C0;}
# 897
;_pop_handler();}else{void*_tmp1BD=(void*)_exn_thrown;void*_tmp1C1=_tmp1BD;void*_tmp1C4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1C1)->tag == Cyc_Dict_Absent){_LLFA: _LLFB: {
# 902
struct Cyc_Absyn_Enumdecl**_tmp1C2=({struct Cyc_Absyn_Enumdecl**_tmp1C3=_cycalloc(sizeof(*_tmp1C3));_tmp1C3[0]=ed;_tmp1C3;});
(te->ae)->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,_tmp1C2);
goto _LLF9;}}else{_LLFC: _tmp1C4=_tmp1C1;_LLFD:(int)_rethrow(_tmp1C4);}_LLF9:;}};}
# 906
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1C5=(struct Cyc_Absyn_Enumfield*)fs->hd;
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp1C5->name,({struct _tuple14*_tmp1C6=_cycalloc(sizeof(*_tmp1C6));_tmp1C6->f1=(void*)({struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7[0]=({struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct _tmp1C8;_tmp1C8.tag=3;_tmp1C8.f1=ed;_tmp1C8.f2=_tmp1C5;_tmp1C8;});_tmp1C7;});_tmp1C6->f2=1;_tmp1C6;}));}}
# 915
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1C9=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1C9->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1C9->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1CD;_tmp1CD.tag=0;_tmp1CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1C9->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1CC;_tmp1CC.tag=0;_tmp1CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp1CA[2]={& _tmp1CC,& _tmp1CD};Cyc_Tcutil_terr(loc,({const char*_tmp1CB="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1CB,sizeof(char),46);}),_tag_dyneither(_tmp1CA,sizeof(void*),2));});});});}}}
# 925
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1CE=sc;switch(_tmp1CE){case Cyc_Absyn_Static: _LLFF: _LL100:
# 928
 if(!in_include)
({void*_tmp1CF=0;Cyc_Tcutil_warn(loc,({const char*_tmp1D0="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1D0,sizeof(char),44);}),_tag_dyneither(_tmp1CF,sizeof(void*),0));});
return 0;case Cyc_Absyn_Abstract: _LL101: _LL102:
# 932
({void*_tmp1D1=0;Cyc_Tcutil_warn(loc,({const char*_tmp1D2="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1D2,sizeof(char),46);}),_tag_dyneither(_tmp1D1,sizeof(void*),0));});
return 0;case Cyc_Absyn_Public: _LL103: _LL104:
 goto _LL106;case Cyc_Absyn_Register: _LL105: _LL106:
 goto _LL108;case Cyc_Absyn_Extern: _LL107: _LL108:
 return 1;default: _LL109: _LL10A:
# 938
({void*_tmp1D3=0;Cyc_Tcutil_warn(loc,({const char*_tmp1D4="nested extern \"C\" declaration";_tag_dyneither(_tmp1D4,sizeof(char),30);}),_tag_dyneither(_tmp1D3,sizeof(void*),0));});
return 1;}_LLFE:;}
# 948
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 954
struct Cyc_List_List*last=0;
# 956
struct Cyc_List_List*_tmp1D5=ds0;for(0;_tmp1D5 != 0;(last=_tmp1D5,_tmp1D5=_tmp1D5->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1D5->hd;
unsigned int loc=d->loc;
# 960
void*_tmp1D6=d->r;void*_tmp1D7=_tmp1D6;struct Cyc_List_List*_tmp1F3;struct Cyc_List_List*_tmp1F2;struct Cyc_List_List*_tmp1F1;union Cyc_Absyn_Nmspace _tmp1F0;struct _dyneither_ptr*_tmp1EF;struct Cyc_List_List*_tmp1EE;struct _dyneither_ptr*_tmp1ED;struct Cyc_List_List*_tmp1EC;struct Cyc_Absyn_Enumdecl*_tmp1EB;struct Cyc_Absyn_Datatypedecl*_tmp1EA;struct Cyc_Absyn_Aggrdecl*_tmp1E9;struct Cyc_Absyn_Typedefdecl*_tmp1E8;struct Cyc_Absyn_Fndecl*_tmp1E7;struct Cyc_Absyn_Vardecl*_tmp1E6;switch(*((int*)_tmp1D7)){case 2: _LL10C: _LL10D:
 goto _LL10F;case 3: _LL10E: _LL10F:
# 963
({void*_tmp1D8=0;Cyc_Tcutil_terr(loc,({const char*_tmp1D9="top level let-declarations are not implemented";_tag_dyneither(_tmp1D9,sizeof(char),47);}),_tag_dyneither(_tmp1D8,sizeof(void*),0));});
goto _LL10B;case 4: _LL110: _LL111:
# 966
({void*_tmp1DA=0;Cyc_Tcutil_terr(loc,({const char*_tmp1DB="top level region declarations are not implemented";_tag_dyneither(_tmp1DB,sizeof(char),50);}),_tag_dyneither(_tmp1DA,sizeof(void*),0));});
goto _LL10B;case 0: _LL112: _tmp1E6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL113:
# 969
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1E6->sc,te->in_extern_c_include))
_tmp1E6->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,_tmp1E6,check_var_init,te->in_extern_c_include,exports);
goto _LL10B;case 1: _LL114: _tmp1E7=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL115:
# 974
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1E7->sc,te->in_extern_c_include))
_tmp1E7->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,_tmp1E7,exports);
goto _LL10B;case 8: _LL116: _tmp1E8=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL117:
# 979
 _tmp1E8->extern_c=te->in_extern_c_include;
# 983
Cyc_Tc_tcTypedefdecl(te,loc,_tmp1E8);
goto _LL10B;case 5: _LL118: _tmp1E9=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL119:
# 986
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1E9->sc,te->in_extern_c_include))
_tmp1E9->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp1E9);
goto _LL10B;case 6: _LL11A: _tmp1EA=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL11B:
# 991
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1EA->sc,te->in_extern_c_include))
_tmp1EA->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,_tmp1EA);
goto _LL10B;case 7: _LL11C: _tmp1EB=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL11D:
# 996
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1EB->sc,te->in_extern_c_include))
_tmp1EB->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp1EB);
goto _LL10B;case 13: _LL11E: _LL11F:
# 1001
({void*_tmp1DC=0;Cyc_Tcutil_warn(d->loc,({const char*_tmp1DD="spurious __cyclone_port_on__";_tag_dyneither(_tmp1DD,sizeof(char),29);}),_tag_dyneither(_tmp1DC,sizeof(void*),0));});
goto _LL10B;case 14: _LL120: _LL121:
# 1004
 goto _LL10B;case 9: _LL122: _tmp1ED=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_tmp1EC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1D7)->f2;_LL123: {
# 1007
struct Cyc_List_List*_tmp1DE=te->ns;
te->ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1DE,({struct Cyc_List_List*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));_tmp1DF->hd=_tmp1ED;_tmp1DF->tl=0;_tmp1DF;}));
Cyc_Tc_tc_decls(te,_tmp1EC,in_externC,check_var_init,grgn,exports);
te->ns=_tmp1DE;
goto _LL10B;}case 10: _LL124: _tmp1F0=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1)->f1;_tmp1EF=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1)->f2;_tmp1EE=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1D7)->f2;_LL125:
# 1015
 Cyc_Tc_tc_decls(te,_tmp1EE,in_externC,check_var_init,grgn,exports);
goto _LL10B;case 11: _LL126: _tmp1F1=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL127:
# 1019
 Cyc_Tc_tc_decls(te,_tmp1F1,1,check_var_init,grgn,exports);
goto _LL10B;default: _LL128: _tmp1F3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_tmp1F2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1D7)->f2;_LL129: {
# 1025
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1F2,(unsigned int)exports?*exports: 0);
{struct _RegionHandle _tmp1E0=_new_region("temp");struct _RegionHandle*temp=& _tmp1E0;_push_region(temp);
{struct Cyc_Tcenv_Tenv*_tmp1E1=Cyc_Tcenv_enter_extern_c_include(temp,te);
Cyc_Tc_tc_decls(_tmp1E1,_tmp1F3,1,check_var_init,grgn,& newexs);
# 1031
for(0;_tmp1F2 != 0;_tmp1F2=_tmp1F2->tl){
struct _tuple12*_tmp1E2=(struct _tuple12*)_tmp1F2->hd;
if(!(*_tmp1E2).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1E5;_tmp1E5.tag=0;_tmp1E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1E2).f2));({void*_tmp1E3[1]={& _tmp1E5};Cyc_Tcutil_warn((*_tmp1E2).f1,({const char*_tmp1E4="%s is exported but not defined";_tag_dyneither(_tmp1E4,sizeof(char),31);}),_tag_dyneither(_tmp1E3,sizeof(void*),1));});});}}
# 1028
;_pop_region(temp);}
# 1038
goto _LL10B;}}_LL10B:;}}
# 1043
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1045
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1062 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1064
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1066
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1070
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1F4=d->r;void*_tmp1F5=_tmp1F4;struct Cyc_List_List**_tmp1FA;struct Cyc_List_List**_tmp1F9;struct Cyc_List_List**_tmp1F8;struct Cyc_List_List**_tmp1F7;struct Cyc_Absyn_Vardecl*_tmp1F6;switch(*((int*)_tmp1F5)){case 0: _LL12B: _tmp1F6=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1F5)->f1;_LL12C:
# 1075
 if((env->in_cinclude  || _tmp1F6->sc != Cyc_Absyn_Extern  && _tmp1F6->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp1F6->name,Cyc_Absyn_uniquergn_qvar))
# 1078
return 1;
# 1080
return(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,_tmp1F6->name)).f2;case 11: _LL12D: _tmp1F7=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1F5)->f1;_LL12E:
 _tmp1F8=_tmp1F7;goto _LL130;case 10: _LL12F: _tmp1F8=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1F5)->f2;_LL130:
 _tmp1F9=_tmp1F8;goto _LL132;case 9: _LL131: _tmp1F9=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1F5)->f2;_LL132:
# 1084
*_tmp1F9=Cyc_Tc_treeshake_f(env,*_tmp1F9);
return 1;case 12: _LL133: _tmp1FA=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1F5)->f1;_LL134: {
# 1087
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp1FA=Cyc_Tc_treeshake_f(env,*_tmp1FA);
env->in_cinclude=in_cinclude;
return 1;}default: _LL135: _LL136:
 return 1;}_LL12A:;}
# 1096
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1FB=({struct Cyc_Tc_TreeshakeEnv _tmp1FC;_tmp1FC.in_cinclude=0;_tmp1FC.ordinaries=(te->ae)->ordinaries;_tmp1FC;});
return Cyc_Tc_treeshake_f(& _tmp1FB,ds);}
