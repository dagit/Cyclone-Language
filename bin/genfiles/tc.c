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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
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
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0U,Cyc_Absyn_Abstract  = 1U,Cyc_Absyn_Public  = 2U,Cyc_Absyn_Extern  = 3U,Cyc_Absyn_ExternC  = 4U,Cyc_Absyn_Register  = 5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0U,Cyc_Absyn_Short_sz  = 1U,Cyc_Absyn_Int_sz  = 2U,Cyc_Absyn_Long_sz  = 3U,Cyc_Absyn_LongLong_sz  = 4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0U,Cyc_Absyn_Unique  = 1U,Cyc_Absyn_Top  = 2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0U,Cyc_Absyn_MemKind  = 1U,Cyc_Absyn_BoxKind  = 2U,Cyc_Absyn_RgnKind  = 3U,Cyc_Absyn_EffKind  = 4U,Cyc_Absyn_IntKind  = 5U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0U,Cyc_Absyn_Unsigned  = 1U,Cyc_Absyn_None  = 2U};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 969
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1073
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1161
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
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 83 "dict.h"
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
# 32 "rgnorder.h"
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn,unsigned int);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 67 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned int,struct Cyc_Absyn_Fndecl*);
# 77
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 147
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 58
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 83
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 85
int Cyc_Tcutil_is_function_type(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 100
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 131
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 145
void Cyc_Tcutil_explain_failure();
# 147
int Cyc_Tcutil_unify(void*,void*);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 186
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 213 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 215
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 223
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 227
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 237
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 269
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 299
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 310
int Cyc_Tcutil_bits_only(void*t);
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 323
int Cyc_Tcutil_supports_default(void*);
# 334
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 338
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 358
int Cyc_Tcutil_is_array(void*t);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple10{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 36
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 33
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 37
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 40
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 59 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 62
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 64
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 72
void**Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 81
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);static char _tmp0[1U]="";
# 37 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1U};
static struct _dyneither_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;
# 41
int Cyc_Tc_aggressive_warn=0;struct _tuple12{unsigned int f1;struct _tuple0*f2;int f3;};
# 43
static int Cyc_Tc_export_member(struct _tuple0*x,struct Cyc_List_List*exports){
for(0;exports != 0;exports=exports->tl){
struct _tuple12*_tmp1=(struct _tuple12*)exports->hd;
if(Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0){
# 49
(*_tmp1).f3=1;
return 1;}}
# 53
return 0;}
# 56
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0U;({struct _dyneither_ptr _tmp1D8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att));_tmp4.f1=_tmp1D8;});({void*_tmp2[1U];_tmp2[0]=& _tmp4;({unsigned int _tmp1DA=loc;struct _dyneither_ptr _tmp1D9=({const char*_tmp3="%s has an out-of-range index";_tag_dyneither(_tmp3,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp1DA,_tmp1D9,_tag_dyneither(_tmp2,sizeof(void*),1U));});});});
return 0;}
# 61
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 63
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 66
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_Int_pa_PrintArg_struct _tmp9;_tmp9.tag=1U;_tmp9.f1=(unsigned long)i;({struct Cyc_String_pa_PrintArg_struct _tmp8;_tmp8.tag=0U;_tmp8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2);({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0U;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2);({void*_tmp5[3U];(((_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8)),_tmp5[2]=& _tmp9);({unsigned int _tmp1DC=loc;struct _dyneither_ptr _tmp1DB=({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_dyneither(_tmp6,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp1DC,_tmp1DB,_tag_dyneither(_tmp5,sizeof(void*),3U));});});});});});}
# 70
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _tuple13 init_params=({struct _tuple13 _tmp1D4;(_tmp1D4.f1=0,({struct _dyneither_ptr _tmp1DD=({const char*_tmp38="initializes";_tag_dyneither(_tmp38,sizeof(char),12U);});_tmp1D4.f2=_tmp1DD;}));_tmp1D4;});
struct _tuple13 nolive_unique_params=({struct _tuple13 _tmp1D3;(_tmp1D3.f1=0,({struct _dyneither_ptr _tmp1DE=({const char*_tmp37="noliveunique";_tag_dyneither(_tmp37,sizeof(char),13U);});_tmp1D3.f2=_tmp1DE;}));_tmp1D3;});
struct _tuple13 noconsume_params=({struct _tuple13 _tmp1D2;(_tmp1D2.f1=0,({struct _dyneither_ptr _tmp1DF=({const char*_tmp36="noconsume";_tag_dyneither(_tmp36,sizeof(char),10U);});_tmp1D2.f2=_tmp1DF;}));_tmp1D2;});
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp34;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->tag == 9U){_LL1: _tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).attributes;_tmp34=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).args;_LL2: {
# 76
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp34);
for(0;_tmp35 != 0;_tmp35=_tmp35->tl){
void*_tmpC=(void*)_tmp35->hd;void*_tmpD=_tmpC;int _tmp31;int _tmp30;int _tmp2F;switch(*((int*)_tmpD)){case 20U: _LL6: _tmp2F=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpD)->f1;_LL7:
# 80
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp2F,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp2F,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp2F,init_params,noconsume_params);{
struct _tuple8*_tmpE=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp2F - 1);struct _tuple8*_tmpF=_tmpE;void*_tmp24;_LLF: _tmp24=_tmpF->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp23="initializes attribute allowed only on";_tag_dyneither(_tmp23,sizeof(char),38U);});
{void*_tmp10=Cyc_Tcutil_compress(_tmp24);void*_tmp11=_tmp10;void*_tmp21;union Cyc_Absyn_Constraint*_tmp20;union Cyc_Absyn_Constraint*_tmp1F;union Cyc_Absyn_Constraint*_tmp1E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->tag == 5U){_LL12: _tmp21=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).elt_typ;_tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).nullable;_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).bounds;_tmp1E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).zero_term;_LL13:
# 87
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp20))
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0U;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp12[1U];_tmp12[0]=& _tmp14;({unsigned int _tmp1E1=loc;struct _dyneither_ptr _tmp1E0=({const char*_tmp13="%s non-null pointers";_tag_dyneither(_tmp13,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp1E1,_tmp1E0,_tag_dyneither(_tmp12,sizeof(void*),1U));});});});
if(!Cyc_Tcutil_is_bound_one(_tmp1F))
({struct Cyc_String_pa_PrintArg_struct _tmp17;_tmp17.tag=0U;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp15[1U];_tmp15[0]=& _tmp17;({unsigned int _tmp1E3=loc;struct _dyneither_ptr _tmp1E2=({const char*_tmp16="%s pointers of size 1";_tag_dyneither(_tmp16,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp1E3,_tmp1E2,_tag_dyneither(_tmp15,sizeof(void*),1U));});});});
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1E))
({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0U;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp18[1U];_tmp18[0]=& _tmp1A;({unsigned int _tmp1E5=loc;struct _dyneither_ptr _tmp1E4=({const char*_tmp19="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp19,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp1E5,_tmp1E4,_tag_dyneither(_tmp18,sizeof(void*),1U));});});});
goto _LL11;}else{_LL14: _LL15:
# 95
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0U;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1B[1U];_tmp1B[0]=& _tmp1D;({unsigned int _tmp1E7=loc;struct _dyneither_ptr _tmp1E6=({const char*_tmp1C="%s pointers";_tag_dyneither(_tmp1C,sizeof(char),12U);});Cyc_Tcutil_terr(_tmp1E7,_tmp1E6,_tag_dyneither(_tmp1B,sizeof(void*),1U));});});});}_LL11:;}
# 97
({struct Cyc_List_List*_tmp1E8=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));((*_tmp22).hd=(void*)_tmp2F,(*_tmp22).tl=init_params.f1);_tmp22;});init_params.f1=_tmp1E8;});
goto _LL5;};};case 21U: _LL8: _tmp30=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpD)->f1;_LL9:
# 100
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp30,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp25=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp30 - 1);struct _tuple8*_tmp26=_tmp25;void*_tmp2A;_LL17: _tmp2A=_tmp26->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2A,0))
({void*_tmp27=0U;({unsigned int _tmp1EA=loc;struct _dyneither_ptr _tmp1E9=({const char*_tmp28="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp28,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp1EA,_tmp1E9,_tag_dyneither(_tmp27,sizeof(void*),0U));});});
({struct Cyc_List_List*_tmp1EB=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));((*_tmp29).hd=(void*)_tmp30,(*_tmp29).tl=nolive_unique_params.f1);_tmp29;});nolive_unique_params.f1=_tmp1EB;});
goto _LL5;};case 22U: _LLA: _tmp31=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpD)->f1;_LLB:
# 109
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp31,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp2B=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp31 - 1);struct _tuple8*_tmp2C=_tmp2B;void*_tmp2E;_LL1A: _tmp2E=_tmp2C->f3;_LL1B:;
# 114
({struct Cyc_List_List*_tmp1EC=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));((*_tmp2D).hd=(void*)_tmp31,(*_tmp2D).tl=noconsume_params.f1);_tmp2D;});noconsume_params.f1=_tmp1EC;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 119
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp32=0U;({struct _dyneither_ptr _tmp1ED=({const char*_tmp33="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp33,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp1ED,_tag_dyneither(_tmp32,sizeof(void*),0U));});});}_LL0:;}struct _tuple14{void*f1;int f2;};
# 124
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 126
struct Cyc_Absyn_Vardecl*_tmp39=vd;enum Cyc_Absyn_Scope _tmp91;struct _tuple0*_tmp90;void*_tmp8F;struct Cyc_Absyn_Exp*_tmp8E;struct Cyc_List_List*_tmp8D;_LL1: _tmp91=_tmp39->sc;_tmp90=_tmp39->name;_tmp8F=_tmp39->type;_tmp8E=_tmp39->initializer;_tmp8D=_tmp39->attributes;_LL2:;
# 133
{void*_tmp3A=Cyc_Tcutil_compress(_tmp8F);void*_tmp3B=_tmp3A;void*_tmp47;struct Cyc_Absyn_Tqual _tmp46;union Cyc_Absyn_Constraint*_tmp45;unsigned int _tmp44;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->tag == 8U){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).num_elts == 0){_LL4: _tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).elt_type;_tmp46=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).tq;_tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).zero_term;_tmp44=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B)->f1).zt_loc;if(_tmp8E != 0){_LL5:
# 135
{void*_tmp3C=_tmp8E->r;void*_tmp3D=_tmp3C;struct Cyc_List_List*_tmp43;struct Cyc_List_List*_tmp42;struct Cyc_Absyn_Exp*_tmp41;struct Cyc_Absyn_Exp*_tmp40;struct _dyneither_ptr _tmp3F;struct _dyneither_ptr _tmp3E;switch(*((int*)_tmp3D)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D)->f1).Wstring_c).tag){case 8U: _LL9: _tmp3E=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D)->f1).String_c).val;_LLA:
# 137
({void*_tmp1F3=({void*_tmp1F2=({void*_tmp1F1=_tmp47;struct Cyc_Absyn_Tqual _tmp1F0=_tmp46;struct Cyc_Absyn_Exp*_tmp1EF=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp3E,sizeof(char)),0U);union Cyc_Absyn_Constraint*_tmp1EE=_tmp45;Cyc_Absyn_array_typ(_tmp1F1,_tmp1F0,_tmp1EF,_tmp1EE,_tmp44);});vd->type=_tmp1F2;});_tmp8F=_tmp1F3;});
goto _LL8;case 9U: _LLB: _tmp3F=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3D)->f1).Wstring_c).val;_LLC:
# 140
({void*_tmp1F9=({void*_tmp1F8=({void*_tmp1F7=_tmp47;struct Cyc_Absyn_Tqual _tmp1F6=_tmp46;struct Cyc_Absyn_Exp*_tmp1F5=Cyc_Absyn_uint_exp(1U,0U);union Cyc_Absyn_Constraint*_tmp1F4=_tmp45;Cyc_Absyn_array_typ(_tmp1F7,_tmp1F6,_tmp1F5,_tmp1F4,_tmp44);});vd->type=_tmp1F8;});_tmp8F=_tmp1F9;});
goto _LL8;default: goto _LL15;}case 27U: _LLD: _tmp40=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp3D)->f2;_LLE:
 _tmp41=_tmp40;goto _LL10;case 28U: _LLF: _tmp41=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp3D)->f1;_LL10:
# 145
({void*_tmp1FB=({void*_tmp1FA=Cyc_Absyn_array_typ(_tmp47,_tmp46,_tmp41,_tmp45,_tmp44);vd->type=_tmp1FA;});_tmp8F=_tmp1FB;});
goto _LL8;case 36U: _LL11: _tmp42=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp3D)->f2;_LL12:
 _tmp43=_tmp42;goto _LL14;case 26U: _LL13: _tmp43=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp3D)->f1;_LL14:
# 149
({void*_tmp201=({void*_tmp200=({void*_tmp1FF=_tmp47;struct Cyc_Absyn_Tqual _tmp1FE=_tmp46;struct Cyc_Absyn_Exp*_tmp1FD=Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp43),0U);union Cyc_Absyn_Constraint*_tmp1FC=_tmp45;Cyc_Absyn_array_typ(_tmp1FF,_tmp1FE,_tmp1FD,_tmp1FC,_tmp44);});vd->type=_tmp200;});_tmp8F=_tmp201;});
goto _LL8;default: _LL15: _LL16:
 goto _LL8;}_LL8:;}
# 153
goto _LL3;}else{goto _LL6;}}else{goto _LL6;}}else{_LL6: _LL7:
 goto _LL3;}_LL3:;}
# 157
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp8F);
# 159
({int _tmp202=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp8F);(vd->tq).real_const=_tmp202;});
# 162
({int _tmp203=!Cyc_Tcutil_is_array(_tmp8F);vd->escapes=_tmp203;});
# 164
if(Cyc_Tcutil_is_function_type(_tmp8F)){
({struct Cyc_List_List*_tmp204=Cyc_Tcutil_transfer_fn_type_atts(_tmp8F,_tmp8D);_tmp8D=_tmp204;});
Cyc_Tc_fnTypeAttsOK(te,_tmp8F,loc);}
# 169
if(_tmp91 == Cyc_Absyn_Extern  || _tmp91 == Cyc_Absyn_ExternC){
if(_tmp8E != 0  && !in_cinclude)
({void*_tmp48=0U;({unsigned int _tmp206=loc;struct _dyneither_ptr _tmp205=({const char*_tmp49="extern declaration should not have initializer";_tag_dyneither(_tmp49,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp206,_tmp205,_tag_dyneither(_tmp48,sizeof(void*),0U));});});}else{
if(!Cyc_Tcutil_is_function_type(_tmp8F)){
# 176
for(0;_tmp8D != 0;_tmp8D=_tmp8D->tl){
void*_tmp4A=(void*)_tmp8D->hd;void*_tmp4B=_tmp4A;switch(*((int*)_tmp4B)){case 6U: _LL18: _LL19:
 goto _LL1B;case 8U: _LL1A: _LL1B:
# 181
 goto _LL1D;case 9U: _LL1C: _LL1D:
 goto _LL1F;case 10U: _LL1E: _LL1F:
 goto _LL21;case 11U: _LL20: _LL21:
 goto _LL23;case 12U: _LL22: _LL23:
 goto _LL25;case 13U: _LL24: _LL25:
 goto _LL27;case 14U: _LL26: _LL27:
 continue;default: _LL28: _LL29:
# 189
({struct Cyc_String_pa_PrintArg_struct _tmp4F;_tmp4F.tag=0U;({struct _dyneither_ptr _tmp207=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name));_tmp4F.f1=_tmp207;});({struct Cyc_String_pa_PrintArg_struct _tmp4E;_tmp4E.tag=0U;({struct _dyneither_ptr _tmp208=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8D->hd));_tmp4E.f1=_tmp208;});({void*_tmp4C[2U];(_tmp4C[0]=& _tmp4E,_tmp4C[1]=& _tmp4F);({unsigned int _tmp20A=loc;struct _dyneither_ptr _tmp209=({const char*_tmp4D="bad attribute %s for variable %s";_tag_dyneither(_tmp4D,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp20A,_tmp209,_tag_dyneither(_tmp4C,sizeof(void*),2U));});});});});
goto _LL17;}_LL17:;}
# 194
if(_tmp8E == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp8F))
({struct Cyc_String_pa_PrintArg_struct _tmp53;_tmp53.tag=0U;({struct _dyneither_ptr _tmp20B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8F));_tmp53.f1=_tmp20B;});({struct Cyc_String_pa_PrintArg_struct _tmp52;_tmp52.tag=0U;({struct _dyneither_ptr _tmp20C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp52.f1=_tmp20C;});({void*_tmp50[2U];(_tmp50[0]=& _tmp52,_tmp50[1]=& _tmp53);({unsigned int _tmp20E=loc;struct _dyneither_ptr _tmp20D=({const char*_tmp51="initializer required for variable %s of type %s";_tag_dyneither(_tmp51,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp20E,_tmp20D,_tag_dyneither(_tmp50,sizeof(void*),2U));});});});});}else{
# 199
struct Cyc_Absyn_Exp*_tmp54=_tmp8E;
# 203
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){
{void*_tmp58=Cyc_Tcexp_tcExpInitializer(te,& _tmp8F,_tmp54);
if(!Cyc_Tcutil_coerce_assign(te,_tmp54,_tmp8F)){
struct _dyneither_ptr _tmp59=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp5A=" declared with type ";
struct _dyneither_ptr _tmp5B=Cyc_Absynpp_typ2string(_tmp8F);
const char*_tmp5C=" but initialized with type ";
struct _dyneither_ptr _tmp5D=Cyc_Absynpp_typ2string(_tmp58);
if(({unsigned long _tmp212=({unsigned long _tmp211=({unsigned long _tmp210=({unsigned long _tmp20F=Cyc_strlen((struct _dyneither_ptr)_tmp59);_tmp20F + Cyc_strlen(({const char*_tmp5E=_tmp5A;_tag_dyneither(_tmp5E,sizeof(char),_get_zero_arr_size_char((void*)_tmp5E,21U));}));});_tmp210 + Cyc_strlen((struct _dyneither_ptr)_tmp5B);});_tmp211 + Cyc_strlen(({const char*_tmp5F=_tmp5C;_tag_dyneither(_tmp5F,sizeof(char),_get_zero_arr_size_char((void*)_tmp5F,28U));}));});_tmp212 + Cyc_strlen((struct _dyneither_ptr)_tmp5D);})> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0U;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0U;({struct _dyneither_ptr _tmp213=(struct _dyneither_ptr)({const char*_tmp68=_tmp5C;_tag_dyneither(_tmp68,sizeof(char),_get_zero_arr_size_char((void*)_tmp68,28U));});_tmp65.f1=_tmp213;});({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0U;_tmp64.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5B);({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0U;({struct _dyneither_ptr _tmp214=(struct _dyneither_ptr)({const char*_tmp67=_tmp5A;_tag_dyneither(_tmp67,sizeof(char),_get_zero_arr_size_char((void*)_tmp67,21U));});_tmp63.f1=_tmp214;});({struct Cyc_String_pa_PrintArg_struct _tmp62;_tmp62.tag=0U;_tmp62.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp59);({void*_tmp60[5U];(((((((_tmp60[0]=& _tmp62,_tmp60[1]=& _tmp63)),_tmp60[2]=& _tmp64)),_tmp60[3]=& _tmp65)),_tmp60[4]=& _tmp66);({unsigned int _tmp216=loc;struct _dyneither_ptr _tmp215=({const char*_tmp61="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp61,sizeof(char),16U);});Cyc_Tcutil_terr(_tmp216,_tmp215,_tag_dyneither(_tmp60,sizeof(void*),5U));});});});});});});});else{
# 214
({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0U;_tmp6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);({struct Cyc_String_pa_PrintArg_struct _tmp6E;_tmp6E.tag=0U;({struct _dyneither_ptr _tmp217=(struct _dyneither_ptr)({const char*_tmp71=_tmp5C;_tag_dyneither(_tmp71,sizeof(char),_get_zero_arr_size_char((void*)_tmp71,28U));});_tmp6E.f1=_tmp217;});({struct Cyc_String_pa_PrintArg_struct _tmp6D;_tmp6D.tag=0U;_tmp6D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5B);({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0U;({struct _dyneither_ptr _tmp218=(struct _dyneither_ptr)({const char*_tmp70=_tmp5A;_tag_dyneither(_tmp70,sizeof(char),_get_zero_arr_size_char((void*)_tmp70,21U));});_tmp6C.f1=_tmp218;});({struct Cyc_String_pa_PrintArg_struct _tmp6B;_tmp6B.tag=0U;_tmp6B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp59);({void*_tmp69[5U];(((((((_tmp69[0]=& _tmp6B,_tmp69[1]=& _tmp6C)),_tmp69[2]=& _tmp6D)),_tmp69[3]=& _tmp6E)),_tmp69[4]=& _tmp6F);({unsigned int _tmp21A=loc;struct _dyneither_ptr _tmp219=({const char*_tmp6A="%s%s%s%s%s";_tag_dyneither(_tmp6A,sizeof(char),11U);});Cyc_Tcutil_terr(_tmp21A,_tmp219,_tag_dyneither(_tmp69,sizeof(void*),5U));});});});});});});});}
Cyc_Tcutil_explain_failure();
# 217
if(!Cyc_Tcutil_is_const_exp(_tmp54))
({void*_tmp72=0U;({unsigned int _tmp21C=loc;struct _dyneither_ptr _tmp21B=({const char*_tmp73="initializer is not a constant expression";_tag_dyneither(_tmp73,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp21C,_tmp21B,_tag_dyneither(_tmp72,sizeof(void*),0U));});});}}
# 204
;_pop_handler();}else{void*_tmp56=(void*)_exn_thrown;void*_tmp74=_tmp56;void*_tmp77;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp74)->tag == Cyc_Tcenv_Env_error){_LL2B: _LL2C:
# 222
({void*_tmp75=0U;({unsigned int _tmp21E=loc;struct _dyneither_ptr _tmp21D=({const char*_tmp76="initializer is not a constant expression";_tag_dyneither(_tmp76,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp21E,_tmp21D,_tag_dyneither(_tmp75,sizeof(void*),0U));});});
goto _LL2A;}else{_LL2D: _tmp77=_tmp74;_LL2E:(int)_rethrow(_tmp77);}_LL2A:;}};}}else{
# 228
for(0;_tmp8D != 0;_tmp8D=_tmp8D->tl){
void*_tmp78=(void*)_tmp8D->hd;void*_tmp79=_tmp78;switch(*((int*)_tmp79)){case 0U: _LL30: _LL31:
# 231
 goto _LL33;case 1U: _LL32: _LL33:
 goto _LL35;case 2U: _LL34: _LL35:
 goto _LL37;case 3U: _LL36: _LL37:
 goto _LL39;case 4U: _LL38: _LL39:
 goto _LL3B;case 19U: _LL3A: _LL3B:
 goto _LL3D;case 20U: _LL3C: _LL3D:
 goto _LL3F;case 23U: _LL3E: _LL3F:
 goto _LL41;case 26U: _LL40: _LL41:
 goto _LL43;case 5U: _LL42: _LL43:
({void*_tmp7A=0U;({struct _dyneither_ptr _tmp21F=({const char*_tmp7B="tcVardecl: fn type atts in function var decl";_tag_dyneither(_tmp7B,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp21F,_tag_dyneither(_tmp7A,sizeof(void*),0U));});});case 6U: _LL44: _LL45:
# 242
 goto _LL47;case 7U: _LL46: _LL47:
# 244
({struct Cyc_String_pa_PrintArg_struct _tmp7E;_tmp7E.tag=0U;({struct _dyneither_ptr _tmp220=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp8D->hd));_tmp7E.f1=_tmp220;});({void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp222=loc;struct _dyneither_ptr _tmp221=({const char*_tmp7D="bad attribute %s in function declaration";_tag_dyneither(_tmp7D,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp222,_tmp221,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});});
goto _LL2F;default: _LL48: _LL49:
 continue;}_LL2F:;}}}
# 252
{struct _handler_cons _tmp7F;_push_handler(& _tmp7F);{int _tmp81=0;if(setjmp(_tmp7F.handler))_tmp81=1;if(!_tmp81){
{struct _tuple14*_tmp82=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,_tmp90);
void*_tmp83=(*_tmp82).f1;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp84=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp88=_cycalloc(sizeof(*_tmp88));((*_tmp88).tag=1U,(*_tmp88).f1=vd);_tmp88;});
void**_tmp85=Cyc_Tcdecl_merge_binding(_tmp83,(void*)_tmp84,loc,Cyc_Tc_tc_msg);
if(_tmp85 == 0){_npop_handler(0U);return;}{
void*_tmp86=*_tmp85;
# 260
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp86 != _tmp83  || (*_tmp82).f2)
# 263
({struct Cyc_Dict_Dict _tmp225=({struct Cyc_Dict_Dict _tmp224=(te->ae)->ordinaries;struct _tuple0*_tmp223=_tmp90;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp224,_tmp223,({struct _tuple14*_tmp87=_cycalloc(sizeof(*_tmp87));((*_tmp87).f1=_tmp86,(*_tmp87).f2=1);_tmp87;}));});(te->ae)->ordinaries=_tmp225;});}
_npop_handler(0U);return;};}
# 253
;_pop_handler();}else{void*_tmp80=(void*)_exn_thrown;void*_tmp89=_tmp80;void*_tmp8A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp89)->tag == Cyc_Dict_Absent){_LL4B: _LL4C:
# 265
 goto _LL4A;}else{_LL4D: _tmp8A=_tmp89;_LL4E:(int)_rethrow(_tmp8A);}_LL4A:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp229=({struct Cyc_Dict_Dict _tmp228=(te->ae)->ordinaries;struct _tuple0*_tmp227=_tmp90;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp228,_tmp227,({struct _tuple14*_tmp8C=_cycalloc(sizeof(*_tmp8C));
(({void*_tmp226=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));((*_tmp8B).tag=1U,(*_tmp8B).f1=vd);_tmp8B;});(*_tmp8C).f1=_tmp226;}),(*_tmp8C).f2=0);_tmp8C;}));});
# 267
(te->ae)->ordinaries=_tmp229;});}
# 271
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp92=n;union Cyc_Absyn_Nmspace _tmp96;struct _dyneither_ptr*_tmp95;_LL1: _tmp96=_tmp92->f1;_tmp95=_tmp92->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmp93=_tmp96;if((_tmp93.Abs_n).tag == 2){if((_tmp93.Abs_n).val == 0){_LL4: _LL5:
# 275
 return({struct _dyneither_ptr _tmp22A=(struct _dyneither_ptr)*_tmp95;Cyc_strcmp(_tmp22A,({const char*_tmp94="main";_tag_dyneither(_tmp94,sizeof(char),5U);}));})== 0;}else{goto _LL6;}}else{_LL6: _LL7:
 return 0;}_LL3:;};}
# 280
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 282
struct _tuple0*q=fd->name;
# 286
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include)
({void*_tmp97=0U;({unsigned int _tmp22C=loc;struct _dyneither_ptr _tmp22B=({const char*_tmp98="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmp98,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp22C,_tmp22B,_tag_dyneither(_tmp97,sizeof(void*),0U));});});
# 290
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 292
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 294
({struct Cyc_List_List*_tmp22D=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);fd->attributes=_tmp22D;});
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 297
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp99=(void*)atts->hd;void*_tmp9A=_tmp99;switch(*((int*)_tmp9A)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
# 301
({struct Cyc_String_pa_PrintArg_struct _tmp9D;_tmp9D.tag=0U;({struct _dyneither_ptr _tmp22E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp9D.f1=_tmp22E;});({void*_tmp9B[1U];_tmp9B[0]=& _tmp9D;({unsigned int _tmp230=loc;struct _dyneither_ptr _tmp22F=({const char*_tmp9C="bad attribute %s for function";_tag_dyneither(_tmp9C,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp230,_tmp22F,_tag_dyneither(_tmp9B,sizeof(void*),1U));});});});
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}}
# 307
{struct _handler_cons _tmp9E;_push_handler(& _tmp9E);{int _tmpA0=0;if(setjmp(_tmp9E.handler))_tmpA0=1;if(!_tmpA0){
{struct _tuple14*_tmpA1=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
void*_tmpA2=(*_tmpA1).f1;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA3=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));((*_tmpA7).tag=2U,(*_tmpA7).f1=fd);_tmpA7;});
void**_tmpA4=Cyc_Tcdecl_merge_binding(_tmpA2,(void*)_tmpA3,loc,Cyc_Tc_tc_msg);
if(_tmpA4 != 0){
void*_tmpA5=*_tmpA4;
# 315
if(exports == 0  || Cyc_Tc_export_member(q,*exports)){
if(!(_tmpA5 == _tmpA2  && (*_tmpA1).f2))
({struct Cyc_Dict_Dict _tmp233=({struct Cyc_Dict_Dict _tmp232=(te->ae)->ordinaries;struct _tuple0*_tmp231=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp232,_tmp231,({struct _tuple14*_tmpA6=_cycalloc(sizeof(*_tmpA6));
((*_tmpA6).f1=_tmpA5,(*_tmpA6).f2=1);_tmpA6;}));});
# 317
(te->ae)->ordinaries=_tmp233;});}}}
# 308
;_pop_handler();}else{void*_tmp9F=(void*)_exn_thrown;void*_tmpA8=_tmp9F;void*_tmpAD;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpA8)->tag == Cyc_Dict_Absent){_LL8: _LL9:
# 321
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA9=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));((*_tmpAC).tag=2U,(*_tmpAC).f1=fd);_tmpAC;});
({struct Cyc_Dict_Dict _tmp237=({struct Cyc_Dict_Dict _tmp236=(te->ae)->ordinaries;struct _tuple0*_tmp235=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp236,_tmp235,({struct _tuple14*_tmpAB=_cycalloc(sizeof(*_tmpAB));
(({void*_tmp234=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));((*_tmpAA).tag=2U,(*_tmpAA).f1=fd);_tmpAA;});(*_tmpAB).f1=_tmp234;}),(*_tmpAB).f2=0);_tmpAB;}));});
# 323
(te->ae)->ordinaries=_tmp237;});}
# 326
goto _LL7;}else{_LLA: _tmpAD=_tmpA8;_LLB:(int)_rethrow(_tmpAD);}_LL7:;}};}
# 330
if(te->in_extern_c_include)return;{
# 335
struct Cyc_Tcenv_Fenv*_tmpAE=Cyc_Tcenv_new_fenv(loc,fd);
struct Cyc_Tcenv_Tenv*_tmpAF=({struct Cyc_Tcenv_Tenv*_tmpD5=_cycalloc(sizeof(*_tmpD5));((((((((*_tmpD5).ns=te->ns,(*_tmpD5).ae=te->ae)),(*_tmpD5).le=_tmpAE)),(*_tmpD5).allow_valueof=0)),(*_tmpD5).in_extern_c_include=0);_tmpD5;});
# 338
Cyc_Tcstmt_tcStmt(_tmpAF,fd->body,0);
# 341
Cyc_Tcenv_check_delayed_effects(_tmpAF);
Cyc_Tcenv_check_delayed_constraints(_tmpAF);
# 344
if(Cyc_Tc_is_main(q)){
# 346
{void*_tmpB0=Cyc_Tcutil_compress(fd->ret_type);void*_tmpB1=_tmpB0;enum Cyc_Absyn_Size_of _tmpB7;switch(*((int*)_tmpB1)){case 0U: _LLD: _LLE:
({void*_tmpB2=0U;({unsigned int _tmp239=loc;struct _dyneither_ptr _tmp238=({const char*_tmpB3="main declared with return type void";_tag_dyneither(_tmpB3,sizeof(char),36U);});Cyc_Tcutil_warn(_tmp239,_tmp238,_tag_dyneither(_tmpB2,sizeof(void*),0U));});});goto _LLC;case 6U: _LLF: _tmpB7=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB1)->f2;_LL10:
 goto _LLC;default: _LL11: _LL12:
# 350
({struct Cyc_String_pa_PrintArg_struct _tmpB6;_tmpB6.tag=0U;({struct _dyneither_ptr _tmp23A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type));_tmpB6.f1=_tmp23A;});({void*_tmpB4[1U];_tmpB4[0]=& _tmpB6;({unsigned int _tmp23C=loc;struct _dyneither_ptr _tmp23B=({const char*_tmpB5="main declared with return type %s instead of int or void";_tag_dyneither(_tmpB5,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp23C,_tmp23B,_tag_dyneither(_tmpB4,sizeof(void*),1U));});});});
goto _LLC;}_LLC:;}
# 354
if(fd->c_varargs  || fd->cyc_varargs != 0)
({void*_tmpB8=0U;({unsigned int _tmp23E=loc;struct _dyneither_ptr _tmp23D=({const char*_tmpB9="main declared with varargs";_tag_dyneither(_tmpB9,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp23E,_tmp23D,_tag_dyneither(_tmpB8,sizeof(void*),0U));});});{
struct Cyc_List_List*_tmpBA=fd->args;
if(_tmpBA != 0){
struct _tuple8*_tmpBB=(struct _tuple8*)_tmpBA->hd;struct _tuple8*_tmpBC=_tmpBB;void*_tmpD4;_LL14: _tmpD4=_tmpBC->f3;_LL15:;
{void*_tmpBD=Cyc_Tcutil_compress(_tmpD4);void*_tmpBE=_tmpBD;enum Cyc_Absyn_Size_of _tmpC2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpBE)->tag == 6U){_LL17: _tmpC2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpBE)->f2;_LL18:
 goto _LL16;}else{_LL19: _LL1A:
# 362
({struct Cyc_String_pa_PrintArg_struct _tmpC1;_tmpC1.tag=0U;({struct _dyneither_ptr _tmp23F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpD4));_tmpC1.f1=_tmp23F;});({void*_tmpBF[1U];_tmpBF[0]=& _tmpC1;({unsigned int _tmp241=loc;struct _dyneither_ptr _tmp240=({const char*_tmpC0="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpC0,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp241,_tmp240,_tag_dyneither(_tmpBF,sizeof(void*),1U));});});});
goto _LL16;}_LL16:;}
# 366
_tmpBA=_tmpBA->tl;
if(_tmpBA != 0){
struct _tuple8*_tmpC3=(struct _tuple8*)_tmpBA->hd;struct _tuple8*_tmpC4=_tmpC3;void*_tmpD3;_LL1C: _tmpD3=_tmpC4->f3;_LL1D:;
_tmpBA=_tmpBA->tl;
if(_tmpBA != 0)
({void*_tmpC5=0U;({unsigned int _tmp243=loc;struct _dyneither_ptr _tmp242=({const char*_tmpC6="main declared with too many arguments";_tag_dyneither(_tmpC6,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp243,_tmp242,_tag_dyneither(_tmpC5,sizeof(void*),0U));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(*_tmpD2));(*_tmpD2).v=fd->tvs;_tmpD2;});
if(((!({void*_tmp25B=_tmpD3;Cyc_Tcutil_unify(_tmp25B,({void*_tmp25A=Cyc_Absyn_string_typ(({struct Cyc_Core_Opt*_tmp256=({struct Cyc_Core_Opt*_tmpC7=_cycalloc(sizeof(*_tmpC7));(*_tmpC7).v=& Cyc_Tcutil_rk;_tmpC7;});Cyc_Absyn_new_evar(_tmp256,tvs);}));void*_tmp259=({
struct Cyc_Core_Opt*_tmp257=({struct Cyc_Core_Opt*_tmpC8=_cycalloc(sizeof(*_tmpC8));(*_tmpC8).v=& Cyc_Tcutil_rk;_tmpC8;});Cyc_Absyn_new_evar(_tmp257,tvs);});
# 373
struct Cyc_Absyn_Tqual _tmp258=
# 375
Cyc_Absyn_empty_tqual(0U);
# 373
Cyc_Absyn_dyneither_typ(_tmp25A,_tmp259,_tmp258,
# 375
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());}));}) && !({
void*_tmp255=_tmpD3;Cyc_Tcutil_unify(_tmp255,({void*_tmp254=Cyc_Absyn_const_string_typ(({struct Cyc_Core_Opt*_tmp250=({struct Cyc_Core_Opt*_tmpC9=_cycalloc(sizeof(*_tmpC9));(*_tmpC9).v=& Cyc_Tcutil_rk;_tmpC9;});Cyc_Absyn_new_evar(_tmp250,tvs);}));void*_tmp253=({
# 378
struct Cyc_Core_Opt*_tmp251=({struct Cyc_Core_Opt*_tmpCA=_cycalloc(sizeof(*_tmpCA));(*_tmpCA).v=& Cyc_Tcutil_rk;_tmpCA;});Cyc_Absyn_new_evar(_tmp251,tvs);});
# 376
struct Cyc_Absyn_Tqual _tmp252=
# 379
Cyc_Absyn_empty_tqual(0U);
# 376
Cyc_Absyn_dyneither_typ(_tmp254,_tmp253,_tmp252,
# 379
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());}));})) && !({
void*_tmp24F=_tmpD3;Cyc_Tcutil_unify(_tmp24F,({void*_tmp24E=Cyc_Absyn_string_typ(({struct Cyc_Core_Opt*_tmp24A=({struct Cyc_Core_Opt*_tmpCB=_cycalloc(sizeof(*_tmpCB));(*_tmpCB).v=& Cyc_Tcutil_rk;_tmpCB;});Cyc_Absyn_new_evar(_tmp24A,tvs);}));void*_tmp24D=({
struct Cyc_Core_Opt*_tmp24B=({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));(*_tmpCC).v=& Cyc_Tcutil_rk;_tmpCC;});Cyc_Absyn_new_evar(_tmp24B,tvs);});
# 380
struct Cyc_Absyn_Tqual _tmp24C=
# 382
Cyc_Absyn_const_tqual(0U);
# 380
Cyc_Absyn_dyneither_typ(_tmp24E,_tmp24D,_tmp24C,
# 382
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());}));})) && !({
void*_tmp249=_tmpD3;Cyc_Tcutil_unify(_tmp249,({void*_tmp248=Cyc_Absyn_const_string_typ(({struct Cyc_Core_Opt*_tmp244=({struct Cyc_Core_Opt*_tmpCD=_cycalloc(sizeof(*_tmpCD));(*_tmpCD).v=& Cyc_Tcutil_rk;_tmpCD;});Cyc_Absyn_new_evar(_tmp244,tvs);}));void*_tmp247=({
# 385
struct Cyc_Core_Opt*_tmp245=({struct Cyc_Core_Opt*_tmpCE=_cycalloc(sizeof(*_tmpCE));(*_tmpCE).v=& Cyc_Tcutil_rk;_tmpCE;});Cyc_Absyn_new_evar(_tmp245,tvs);});
# 383
struct Cyc_Absyn_Tqual _tmp246=
# 386
Cyc_Absyn_const_tqual(0U);
# 383
Cyc_Absyn_dyneither_typ(_tmp248,_tmp247,_tmp246,
# 386
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());}));}))
({struct Cyc_String_pa_PrintArg_struct _tmpD1;_tmpD1.tag=0U;({struct _dyneither_ptr _tmp25C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpD3));_tmpD1.f1=_tmp25C;});({void*_tmpCF[1U];_tmpCF[0]=& _tmpD1;({unsigned int _tmp25E=loc;struct _dyneither_ptr _tmp25D=({const char*_tmpD0="second argument of main has type %s instead of char??";_tag_dyneither(_tmpD0,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp25E,_tmp25D,_tag_dyneither(_tmpCF,sizeof(void*),1U));});});});};}}};}};};}
# 395
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 402
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0U;_tmpD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmpD6[1U];_tmpD6[0]=& _tmpD8;({unsigned int _tmp260=loc;struct _dyneither_ptr _tmp25F=({const char*_tmpD7="redeclaration of typedef %s";_tag_dyneither(_tmpD7,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp260,_tmp25F,_tag_dyneither(_tmpD6,sizeof(void*),1U));});});});
return;}
# 407
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
({int _tmp261=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 411
(td->tq).real_const=_tmp261;});}
# 416
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpD9=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmpDA=_tmpD9;struct Cyc_Core_Opt**_tmpE2;struct Cyc_Absyn_Kind*_tmpE1;struct Cyc_Core_Opt**_tmpE0;switch(*((int*)_tmpDA)){case 1U: _LL1: _tmpE0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpDA)->f1;_LL2:
# 419
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0U;_tmpDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);({void*_tmpDB[1U];_tmpDB[0]=& _tmpDD;({unsigned int _tmp263=loc;struct _dyneither_ptr _tmp262=({const char*_tmpDC="type variable %s is not used in typedef definition";_tag_dyneither(_tmpDC,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp263,_tmp262,_tag_dyneither(_tmpDB,sizeof(void*),1U));});});});
# 422
({struct Cyc_Core_Opt*_tmp265=({struct Cyc_Core_Opt*_tmpDE=_cycalloc(sizeof(*_tmpDE));({void*_tmp264=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);(*_tmpDE).v=_tmp264;});_tmpDE;});*_tmpE0=_tmp265;});goto _LL0;case 2U: _LL3: _tmpE2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDA)->f1;_tmpE1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpDA)->f2;_LL4:
# 429
({struct Cyc_Core_Opt*_tmp267=({struct Cyc_Core_Opt*_tmpDF=_cycalloc(sizeof(*_tmpDF));({void*_tmp266=Cyc_Tcutil_kind_to_bound(_tmpE1);(*_tmpDF).v=_tmp266;});_tmpDF;});*_tmpE2=_tmp267;});
goto _LL0;default: _LL5: _LL6:
 continue;}_LL0:;}}
# 435
({struct Cyc_Dict_Dict _tmp268=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp268;});}
# 438
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 445
struct _RegionHandle _tmpE3=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpE3;_push_region(uprev_rgn);
# 447
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmpE4=(struct _tuple10*)rpo->hd;struct _tuple10*_tmpE5=_tmpE4;void*_tmpE7;void*_tmpE6;_LL1: _tmpE7=_tmpE5->f1;_tmpE6=_tmpE5->f2;_LL2:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmpE7);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmpE6);}{
# 453
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 456
struct Cyc_List_List*_tmpE8=fields;for(0;_tmpE8 != 0;_tmpE8=_tmpE8->tl){
struct Cyc_Absyn_Aggrfield*_tmpE9=(struct Cyc_Absyn_Aggrfield*)_tmpE8->hd;struct Cyc_Absyn_Aggrfield*_tmpEA=_tmpE9;struct _dyneither_ptr*_tmp105;struct Cyc_Absyn_Tqual _tmp104;void*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp100;_LL4: _tmp105=_tmpEA->name;_tmp104=_tmpEA->tq;_tmp103=_tmpEA->type;_tmp102=_tmpEA->width;_tmp101=_tmpEA->attributes;_tmp100=_tmpEA->requires_clause;_LL5:;
# 459
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp105))
({struct Cyc_String_pa_PrintArg_struct _tmpED;_tmpED.tag=0U;_tmpED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp105);({void*_tmpEB[1U];_tmpEB[0]=& _tmpED;({unsigned int _tmp26A=loc;struct _dyneither_ptr _tmp269=({const char*_tmpEC="duplicate member %s";_tag_dyneither(_tmpEC,sizeof(char),20U);});Cyc_Tcutil_terr(_tmp26A,_tmp269,_tag_dyneither(_tmpEB,sizeof(void*),1U));});});});
# 463
if(({struct _dyneither_ptr _tmp26B=(struct _dyneither_ptr)*_tmp105;Cyc_strcmp(_tmp26B,({const char*_tmpEE="";_tag_dyneither(_tmpEE,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmp26C=({struct Cyc_List_List*_tmpEF=_region_malloc(uprev_rgn,sizeof(*_tmpEF));((*_tmpEF).hd=_tmp105,(*_tmpEF).tl=prev_fields);_tmpEF;});prev_fields=_tmp26C;});{
# 466
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 470
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmpE8->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp103);
# 475
({int _tmp26D=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpE8->hd)->tq).print_const,_tmp103);(((struct Cyc_Absyn_Aggrfield*)_tmpE8->hd)->tq).real_const=_tmp26D;});
# 478
Cyc_Tcutil_check_bitfield(loc,te,_tmp103,_tmp102,_tmp105);
# 480
if((unsigned int)_tmp100){
if(str_or_union != Cyc_Absyn_UnionA)
({void*_tmpF0=0U;({unsigned int _tmp26F=loc;struct _dyneither_ptr _tmp26E=({const char*_tmpF1="@requires clauses are only allowed on union members";_tag_dyneither(_tmpF1,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp26F,_tmp26E,_tag_dyneither(_tmpF0,sizeof(void*),0U));});});{
struct Cyc_Tcenv_Tenv*_tmpF2=Cyc_Tcenv_allow_valueof(te);
Cyc_Tcexp_tcExp(_tmpF2,0,_tmp100);
if(!Cyc_Tcutil_is_integral(_tmp100))
({struct Cyc_String_pa_PrintArg_struct _tmpF5;_tmpF5.tag=0U;({struct _dyneither_ptr _tmp270=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 488
Cyc_Absynpp_typ2string((void*)_check_null(_tmp100->topt)));_tmpF5.f1=_tmp270;});({void*_tmpF3[1U];_tmpF3[0]=& _tmpF5;({unsigned int _tmp272=_tmp100->loc;struct _dyneither_ptr _tmp271=({const char*_tmpF4="@requires clause has type %s instead of integral type";_tag_dyneither(_tmpF4,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp272,_tmp271,_tag_dyneither(_tmpF3,sizeof(void*),1U));});});});else{
# 490
({unsigned int _tmp275=_tmp100->loc;struct Cyc_Tcenv_Tenv*_tmp274=te;struct Cyc_List_List*_tmp273=tvs;Cyc_Tcutil_check_type(_tmp275,_tmp274,_tmp273,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));((*_tmpF6).tag=18U,(*_tmpF6).f1=_tmp100);_tmpF6;}));});{
# 492
struct Cyc_List_List*_tmpF7=Cyc_Relations_exp2relns(uprev_rgn,_tmp100);
# 499
if(!Cyc_Relations_consistent_relations(_tmpF7))
({void*_tmpF8=0U;({unsigned int _tmp277=_tmp100->loc;struct _dyneither_ptr _tmp276=({const char*_tmpF9="@requires clause may be unsatisfiable";_tag_dyneither(_tmpF9,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp277,_tmp276,_tag_dyneither(_tmpF8,sizeof(void*),0U));});});
# 505
{struct Cyc_List_List*_tmpFA=prev_relations;for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmpF7,(struct Cyc_List_List*)_tmpFA->hd)))
({void*_tmpFB=0U;({unsigned int _tmp279=_tmp100->loc;struct _dyneither_ptr _tmp278=({const char*_tmpFC="@requires clause may overlap with previous clauses";_tag_dyneither(_tmpFC,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp279,_tmp278,_tag_dyneither(_tmpFB,sizeof(void*),0U));});});}}
# 510
({struct Cyc_List_List*_tmp27A=({struct Cyc_List_List*_tmpFD=_region_malloc(uprev_rgn,sizeof(*_tmpFD));((*_tmpFD).hd=_tmpF7,(*_tmpFD).tl=prev_relations);_tmpFD;});prev_relations=_tmp27A;});};}};}else{
# 513
if(prev_relations != 0)
({void*_tmpFE=0U;({unsigned int _tmp27C=loc;struct _dyneither_ptr _tmp27B=({const char*_tmpFF="if one field has a @requires clause, they all must";_tag_dyneither(_tmpFF,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp27C,_tmp27B,_tag_dyneither(_tmpFE,sizeof(void*),0U));});});}};}};
# 447
;_pop_region(uprev_rgn);}
# 519
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 521
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp106=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp107=_tmp106;enum Cyc_Absyn_AliasQual _tmp11C;struct Cyc_Core_Opt**_tmp11B;struct Cyc_Absyn_Kind*_tmp11A;struct Cyc_Core_Opt**_tmp119;struct Cyc_Core_Opt**_tmp118;enum Cyc_Absyn_AliasQual _tmp117;struct Cyc_Core_Opt**_tmp116;switch(*((int*)_tmp107)){case 1U: _LL1: _tmp116=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp107)->f1;_LL2:
# 524
({struct Cyc_Core_Opt*_tmp27E=({struct Cyc_Core_Opt*_tmp108=_cycalloc(sizeof(*_tmp108));({void*_tmp27D=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);(*_tmp108).v=_tmp27D;});_tmp108;});*_tmp116=_tmp27E;});continue;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f2)->kind){case Cyc_Absyn_MemKind: _LL3: _tmp118=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f1;_tmp117=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f2)->aliasqual;_LL4:
# 526
 if(constrain_top_kind  && _tmp117 == Cyc_Absyn_Top)
({struct Cyc_Core_Opt*_tmp280=({struct Cyc_Core_Opt*_tmp10A=_cycalloc(sizeof(*_tmp10A));({void*_tmp27F=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp109=_cycalloc(sizeof(*_tmp109));((*_tmp109).kind=Cyc_Absyn_BoxKind,(*_tmp109).aliasqual=Cyc_Absyn_Aliasable);_tmp109;}));(*_tmp10A).v=_tmp27F;});_tmp10A;});*_tmp118=_tmp280;});else{
# 529
({struct Cyc_Core_Opt*_tmp282=({struct Cyc_Core_Opt*_tmp10C=_cycalloc(sizeof(*_tmp10C));({void*_tmp281=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp10B=_cycalloc(sizeof(*_tmp10B));((*_tmp10B).kind=Cyc_Absyn_BoxKind,(*_tmp10B).aliasqual=_tmp117);_tmp10B;}));(*_tmp10C).v=_tmp281;});_tmp10C;});*_tmp118=_tmp282;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f2)->aliasqual == Cyc_Absyn_Top){_LL5: _tmp119=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f1;if(constrain_top_kind){_LL6:
# 532
({struct Cyc_Core_Opt*_tmp284=({struct Cyc_Core_Opt*_tmp10E=_cycalloc(sizeof(*_tmp10E));({void*_tmp283=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp10D=_cycalloc(sizeof(*_tmp10D));((*_tmp10D).kind=Cyc_Absyn_BoxKind,(*_tmp10D).aliasqual=Cyc_Absyn_Aliasable);_tmp10D;}));(*_tmp10E).v=_tmp283;});_tmp10E;});*_tmp119=_tmp284;});
continue;}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _tmp11B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f1;_tmp11A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp107)->f2;_LL8:
# 535
({struct Cyc_Core_Opt*_tmp286=({struct Cyc_Core_Opt*_tmp10F=_cycalloc(sizeof(*_tmp10F));({void*_tmp285=Cyc_Tcutil_kind_to_bound(_tmp11A);(*_tmp10F).v=_tmp285;});_tmp10F;});*_tmp11B=_tmp286;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp107)->f1)->kind == Cyc_Absyn_MemKind){_LL9: _tmp11C=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp107)->f1)->aliasqual;_LLA:
# 537
({struct Cyc_String_pa_PrintArg_struct _tmp114;_tmp114.tag=0U;({struct _dyneither_ptr _tmp287=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp115=_cycalloc(sizeof(*_tmp115));((*_tmp115).kind=Cyc_Absyn_MemKind,(*_tmp115).aliasqual=_tmp11C);_tmp115;})));_tmp114.f1=_tmp287;});({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0U;_tmp113.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp112;_tmp112.tag=0U;_tmp112.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*n).f2);({void*_tmp110[3U];(((_tmp110[0]=& _tmp112,_tmp110[1]=& _tmp113)),_tmp110[2]=& _tmp114);({unsigned int _tmp289=loc;struct _dyneither_ptr _tmp288=({const char*_tmp111="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp111,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp289,_tmp288,_tag_dyneither(_tmp110,sizeof(void*),3U));});});});});});
continue;}else{_LLB: _LLC:
 continue;}}_LL0:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 545
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp11D=ad->name;
# 552
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp11E=(void*)atts->hd;void*_tmp11F=_tmp11E;switch(*((int*)_tmp11F)){case 7U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
# 557
({struct Cyc_String_pa_PrintArg_struct _tmp123;_tmp123.tag=0U;_tmp123.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp11D).f2);({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0U;({struct _dyneither_ptr _tmp28A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));_tmp122.f1=_tmp28A;});({void*_tmp120[2U];(_tmp120[0]=& _tmp122,_tmp120[1]=& _tmp123);({unsigned int _tmp28C=loc;struct _dyneither_ptr _tmp28B=({const char*_tmp121="bad attribute %s in %s definition";_tag_dyneither(_tmp121,sizeof(char),34U);});Cyc_Tcutil_terr(_tmp28C,_tmp28B,_tag_dyneither(_tmp120,sizeof(void*),2U));});});});});
goto _LL0;}_LL0:;}}{
# 562
struct Cyc_List_List*_tmp124=ad->tvs;
# 565
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 570
struct _tuple15 _tmp125=({struct _tuple15 _tmp1D5;(_tmp1D5.f1=ad->impl,({struct Cyc_Absyn_Aggrdecl***_tmp28D=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,_tmp11D);_tmp1D5.f2=_tmp28D;}));_tmp1D5;});struct _tuple15 _tmp126=_tmp125;struct Cyc_List_List*_tmp142;struct Cyc_List_List*_tmp141;struct Cyc_List_List*_tmp140;int _tmp13F;struct Cyc_Absyn_Aggrdecl**_tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_List_List*_tmp13C;struct Cyc_List_List*_tmp13B;int _tmp13A;struct Cyc_Absyn_Aggrdecl**_tmp139;if(_tmp126.f1 == 0){if(_tmp126.f2 == 0){_LL8: _LL9:
# 573
 Cyc_Tc_rule_out_memkind(loc,_tmp11D,_tmp124,0);
# 575
({struct Cyc_Dict_Dict _tmp290=({struct Cyc_Dict_Dict _tmp28F=(te->ae)->aggrdecls;struct _tuple0*_tmp28E=_tmp11D;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(_tmp28F,_tmp28E,({struct Cyc_Absyn_Aggrdecl**_tmp127=_cycalloc(sizeof(*_tmp127));*_tmp127=ad;_tmp127;}));});(te->ae)->aggrdecls=_tmp290;});
goto _LL7;}else{_LLE: _tmp139=*_tmp126.f2;_LLF: {
# 635
struct Cyc_Absyn_Aggrdecl*_tmp138=Cyc_Tcdecl_merge_aggrdecl(*_tmp139,ad,loc,Cyc_Tc_tc_msg);
if(_tmp138 == 0)
return;else{
# 639
Cyc_Tc_rule_out_memkind(loc,_tmp11D,_tmp124,0);
# 642
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp11D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 645
*_tmp139=_tmp138;
ad=_tmp138;
goto _LL7;}}}}else{if(_tmp126.f2 == 0){_LLA: _tmp13D=(_tmp126.f1)->exist_vars;_tmp13C=(_tmp126.f1)->rgn_po;_tmp13B=(_tmp126.f1)->fields;_tmp13A=(_tmp126.f1)->tagged;_LLB: {
# 580
struct Cyc_Absyn_Aggrdecl**_tmp128=({struct Cyc_Absyn_Aggrdecl**_tmp131=_cycalloc(sizeof(*_tmp131));({struct Cyc_Absyn_Aggrdecl*_tmp291=({struct Cyc_Absyn_Aggrdecl*_tmp130=_cycalloc(sizeof(*_tmp130));((((((((((((*_tmp130).kind=ad->kind,(*_tmp130).sc=Cyc_Absyn_Extern)),(*_tmp130).name=ad->name)),(*_tmp130).tvs=_tmp124)),(*_tmp130).impl=0)),(*_tmp130).attributes=ad->attributes)),(*_tmp130).expected_mem_kind=0);_tmp130;});*_tmp131=_tmp291;});_tmp131;});
# 582
({struct Cyc_Dict_Dict _tmp292=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp11D,_tmp128);(te->ae)->aggrdecls=_tmp292;});
# 587
Cyc_Tcutil_check_unique_tvars(loc,_tmp13D);
Cyc_Tcutil_add_tvar_identities(_tmp13D);
# 591
if(_tmp13A  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp129=0U;({unsigned int _tmp294=loc;struct _dyneither_ptr _tmp293=({const char*_tmp12A="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp12A,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp294,_tmp293,_tag_dyneither(_tmp129,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp299=te;unsigned int _tmp298=loc;enum Cyc_Absyn_AggrKind _tmp297=ad->kind;struct Cyc_List_List*_tmp296=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp124,_tmp13D);struct Cyc_List_List*_tmp295=_tmp13C;Cyc_Tc_tcAggrImpl(_tmp299,_tmp298,_tmp297,_tmp296,_tmp295,_tmp13B);});
# 595
Cyc_Tc_rule_out_memkind(loc,_tmp11D,_tmp124,0);
# 598
Cyc_Tc_rule_out_memkind(loc,_tmp11D,_tmp13D,1);
# 600
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp13A){
# 603
struct Cyc_List_List*f=_tmp13B;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp12F;_tmp12F.tag=0U;({struct _dyneither_ptr _tmp29A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));_tmp12F.f1=_tmp29A;});({struct Cyc_String_pa_PrintArg_struct _tmp12E;_tmp12E.tag=0U;_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp11D).f2);({struct Cyc_String_pa_PrintArg_struct _tmp12D;_tmp12D.tag=0U;_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);({void*_tmp12B[3U];(((_tmp12B[0]=& _tmp12D,_tmp12B[1]=& _tmp12E)),_tmp12B[2]=& _tmp12F);({unsigned int _tmp29C=loc;struct _dyneither_ptr _tmp29B=({const char*_tmp12C="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp12C,sizeof(char),98U);});Cyc_Tcutil_warn(_tmp29C,_tmp29B,_tag_dyneither(_tmp12B,sizeof(void*),3U));});});});});});}}
# 609
*_tmp128=ad;
goto _LL7;}}else{_LLC: _tmp142=(_tmp126.f1)->exist_vars;_tmp141=(_tmp126.f1)->rgn_po;_tmp140=(_tmp126.f1)->fields;_tmp13F=(_tmp126.f1)->tagged;_tmp13E=*_tmp126.f2;_LLD:
# 613
 if(ad->kind != (*_tmp13E)->kind)
({void*_tmp132=0U;({unsigned int _tmp29E=loc;struct _dyneither_ptr _tmp29D=({const char*_tmp133="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp133,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp29E,_tmp29D,_tag_dyneither(_tmp132,sizeof(void*),0U));});});{
# 616
struct Cyc_Absyn_Aggrdecl*_tmp134=*_tmp13E;
# 618
({struct Cyc_Absyn_Aggrdecl*_tmp29F=({struct Cyc_Absyn_Aggrdecl*_tmp135=_cycalloc(sizeof(*_tmp135));((((((((((((*_tmp135).kind=ad->kind,(*_tmp135).sc=Cyc_Absyn_Extern)),(*_tmp135).name=ad->name)),(*_tmp135).tvs=_tmp124)),(*_tmp135).impl=0)),(*_tmp135).attributes=ad->attributes)),(*_tmp135).expected_mem_kind=0);_tmp135;});*_tmp13E=_tmp29F;});
# 624
Cyc_Tcutil_check_unique_tvars(loc,_tmp142);
Cyc_Tcutil_add_tvar_identities(_tmp142);
# 627
if(_tmp13F  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp136=0U;({unsigned int _tmp2A1=loc;struct _dyneither_ptr _tmp2A0=({const char*_tmp137="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp137,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp2A1,_tmp2A0,_tag_dyneither(_tmp136,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp2A6=te;unsigned int _tmp2A5=loc;enum Cyc_Absyn_AggrKind _tmp2A4=ad->kind;struct Cyc_List_List*_tmp2A3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp124,_tmp142);struct Cyc_List_List*_tmp2A2=_tmp141;Cyc_Tc_tcAggrImpl(_tmp2A6,_tmp2A5,_tmp2A4,_tmp2A3,_tmp2A2,_tmp140);});
# 631
*_tmp13E=_tmp134;
_tmp139=_tmp13E;goto _LLF;};}}_LL7:;};};}
# 652
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp143=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp144=_tmp143;enum Cyc_Absyn_AliasQual _tmp161;enum Cyc_Absyn_KindQual _tmp160;struct Cyc_Core_Opt**_tmp15F;enum Cyc_Absyn_KindQual _tmp15E;struct Cyc_Core_Opt**_tmp15D;struct Cyc_Core_Opt**_tmp15C;struct Cyc_Core_Opt**_tmp15B;struct Cyc_Core_Opt**_tmp15A;struct Cyc_Core_Opt**_tmp159;struct Cyc_Core_Opt**_tmp158;switch(*((int*)_tmp144)){case 1U: _LL1: _tmp158=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL2:
 _tmp159=_tmp158;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->aliasqual){case Cyc_Absyn_Top: _LL3: _tmp159=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL4:
# 657
 _tmp15A=_tmp159;goto _LL6;case Cyc_Absyn_Aliasable: _LL5: _tmp15A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL6:
# 659
({struct Cyc_Core_Opt*_tmp2A8=({struct Cyc_Core_Opt*_tmp145=_cycalloc(sizeof(*_tmp145));({void*_tmp2A7=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);(*_tmp145).v=_tmp2A7;});_tmp145;});*_tmp15A=_tmp2A8;});goto _LL0;default: goto _LLF;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7: _tmp15B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL8:
 _tmp15C=_tmp15B;goto _LLA;case Cyc_Absyn_Aliasable: _LL9: _tmp15C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LLA:
# 662
({struct Cyc_Core_Opt*_tmp2AA=({struct Cyc_Core_Opt*_tmp146=_cycalloc(sizeof(*_tmp146));({void*_tmp2A9=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);(*_tmp146).v=_tmp2A9;});_tmp146;});*_tmp15C=_tmp2AA;});goto _LL0;default: goto _LLF;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->aliasqual){case Cyc_Absyn_Top: _LLB: _tmp15D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LLC:
# 664
({struct Cyc_Core_Opt*_tmp2AC=({struct Cyc_Core_Opt*_tmp147=_cycalloc(sizeof(*_tmp147));({void*_tmp2AB=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);(*_tmp147).v=_tmp2AB;});_tmp147;});*_tmp15D=_tmp2AC;});goto _LL0;case Cyc_Absyn_Unique: goto _LLF;default: goto _LL15;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->aliasqual == Cyc_Absyn_Unique){_LLF: _tmp15F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f1;_tmp15E=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2)->kind;_LL10:
# 668
 _tmp160=_tmp15E;goto _LL12;}else{goto _LL15;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->aliasqual){case Cyc_Absyn_Top: _LLD: _LLE:
# 666
({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0U;_tmp14B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0U;_tmp14A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp148[2U];(_tmp148[0]=& _tmp14A,_tmp148[1]=& _tmp14B);({unsigned int _tmp2AE=loc;struct _dyneither_ptr _tmp2AD=({const char*_tmp149="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp149,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2AE,_tmp2AD,_tag_dyneither(_tmp148,sizeof(void*),2U));});});});});
goto _LL0;case Cyc_Absyn_Unique: goto _LL11;default: goto _LL15;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->aliasqual == Cyc_Absyn_Unique){_LL11: _tmp160=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->kind;_LL12:
# 670
({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0U;({struct _dyneither_ptr _tmp2AF=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 672
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp151=_cycalloc(sizeof(*_tmp151));((*_tmp151).kind=_tmp160,(*_tmp151).aliasqual=Cyc_Absyn_Unique);_tmp151;})));_tmp150.f1=_tmp2AF;});({struct Cyc_String_pa_PrintArg_struct _tmp14F;_tmp14F.tag=0U;_tmp14F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0U;_tmp14E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp14C[3U];(((_tmp14C[0]=& _tmp14E,_tmp14C[1]=& _tmp14F)),_tmp14C[2]=& _tmp150);({unsigned int _tmp2B1=loc;struct _dyneither_ptr _tmp2B0=({const char*_tmp14D="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp14D,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2B1,_tmp2B0,_tag_dyneither(_tmp14C,sizeof(void*),3U));});});});});});goto _LL0;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->kind == Cyc_Absyn_MemKind){_LL13: _tmp161=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1)->aliasqual;_LL14:
# 674
({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U;({struct _dyneither_ptr _tmp2B2=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 676
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp157=_cycalloc(sizeof(*_tmp157));((*_tmp157).kind=Cyc_Absyn_MemKind,(*_tmp157).aliasqual=_tmp161);_tmp157;})));_tmp156.f1=_tmp2B2;});({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0U;_tmp155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U;_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp152[3U];(((_tmp152[0]=& _tmp154,_tmp152[1]=& _tmp155)),_tmp152[2]=& _tmp156);({unsigned int _tmp2B4=loc;struct _dyneither_ptr _tmp2B3=({const char*_tmp153="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp153,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp2B4,_tmp2B3,_tag_dyneither(_tmp152,sizeof(void*),3U));});});});});});goto _LL0;}else{_LL15: _LL16:
 goto _LL0;}}}}_LL0:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 682
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 690
{struct Cyc_List_List*_tmp162=fields;for(0;_tmp162 != 0;_tmp162=_tmp162->tl){
struct Cyc_Absyn_Datatypefield*_tmp163=(struct Cyc_Absyn_Datatypefield*)_tmp162->hd;
struct Cyc_List_List*typs=_tmp163->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp163->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);
# 695
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp166;_tmp166.tag=0U;({struct _dyneither_ptr _tmp2B5=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 698
Cyc_Absynpp_qvar2string(_tmp163->name));_tmp166.f1=_tmp2B5;});({void*_tmp164[1U];_tmp164[0]=& _tmp166;({unsigned int _tmp2B7=_tmp163->loc;struct _dyneither_ptr _tmp2B6=({const char*_tmp165="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp165,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp2B7,_tmp2B6,_tag_dyneither(_tmp164,sizeof(void*),1U));});});});
({int _tmp2B8=
Cyc_Tcutil_extract_const_from_typedef(_tmp163->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 699
((*((struct _tuple16*)typs->hd)).f1).real_const=_tmp2B8;});}}}{
# 705
struct Cyc_List_List*fields2;
if(is_extensible){
# 708
int _tmp167=1;
struct Cyc_List_List*_tmp168=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp167,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp167)
fields2=_tmp168;else{
# 713
fields2=0;}}else{
# 715
struct _RegionHandle _tmp169=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp169;_push_region(uprev_rgn);
# 717
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp16A=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp16A->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp16E;_tmp16E.tag=0U;_tmp16E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);({struct Cyc_String_pa_PrintArg_struct _tmp16D;_tmp16D.tag=0U;_tmp16D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp16A->name).f2);({void*_tmp16B[2U];(_tmp16B[0]=& _tmp16D,_tmp16B[1]=& _tmp16E);({unsigned int _tmp2BA=_tmp16A->loc;struct _dyneither_ptr _tmp2B9=({const char*_tmp16C="duplicate field name %s in %s";_tag_dyneither(_tmp16C,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp2BA,_tmp2B9,_tag_dyneither(_tmp16B,sizeof(void*),2U));});});});});else{
# 723
({struct Cyc_List_List*_tmp2BB=({struct Cyc_List_List*_tmp16F=_region_malloc(uprev_rgn,sizeof(*_tmp16F));((*_tmp16F).hd=(*_tmp16A->name).f2,(*_tmp16F).tl=prev_fields);_tmp16F;});prev_fields=_tmp2BB;});}
# 725
if(_tmp16A->sc != Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0U;_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp16A->name).f2);({void*_tmp170[1U];_tmp170[0]=& _tmp172;({unsigned int _tmp2BD=loc;struct _dyneither_ptr _tmp2BC=({const char*_tmp171="ignoring scope of field %s";_tag_dyneither(_tmp171,sizeof(char),27U);});Cyc_Tcutil_warn(_tmp2BD,_tmp2BC,_tag_dyneither(_tmp170,sizeof(void*),1U));});});});
_tmp16A->sc=Cyc_Absyn_Public;}}}
# 730
fields2=fields;}
# 717
;_pop_region(uprev_rgn);}
# 732
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 735
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp188="@extensible datatype";_tag_dyneither(_tmp188,sizeof(char),21U);}):({const char*_tmp189="datatype";_tag_dyneither(_tmp189,sizeof(char),9U);});
# 742
struct Cyc_List_List*tvs=tud->tvs;
# 745
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 751
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 761 "tc.cyc"
{struct _handler_cons _tmp173;_push_handler(& _tmp173);{int _tmp175=0;if(setjmp(_tmp173.handler))_tmp175=1;if(!_tmp175){
({struct Cyc_Absyn_Datatypedecl***_tmp2BE=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);tud_opt=_tmp2BE;});
if(tud_opt == 0  && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp176=_cycalloc(sizeof(*_tmp176));(*_tmp176).tag=Cyc_Dict_Absent;_tmp176;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 767
({union Cyc_Absyn_Nmspace _tmp2BF=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp2BF;});}
# 762
;_pop_handler();}else{void*_tmp174=(void*)_exn_thrown;void*_tmp177=_tmp174;void*_tmp17A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp177)->tag == Cyc_Dict_Absent){_LL1: _LL2: {
# 771
struct Cyc_Absyn_Datatypedecl***_tmp178=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);
({struct Cyc_Absyn_Datatypedecl***_tmp2C0=(unsigned int)_tmp178?({struct Cyc_Absyn_Datatypedecl***_tmp179=_cycalloc(sizeof(*_tmp179));*_tmp179=*_tmp178;_tmp179;}): 0;tud_opt=_tmp2C0;});
goto _LL0;}}else{_LL3: _tmp17A=_tmp177;_LL4:(int)_rethrow(_tmp17A);}_LL0:;}};}{
# 778
struct _tuple17 _tmp17B=({struct _tuple17 _tmp1D6;(_tmp1D6.f1=tud->fields,_tmp1D6.f2=tud_opt);_tmp1D6;});struct _tuple17 _tmp17C=_tmp17B;struct Cyc_List_List**_tmp187;struct Cyc_Absyn_Datatypedecl**_tmp186;struct Cyc_List_List**_tmp185;struct Cyc_Absyn_Datatypedecl**_tmp184;if(_tmp17C.f1 == 0){if(_tmp17C.f2 == 0){_LL6: _LL7:
# 781
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp2C3=({struct Cyc_Dict_Dict _tmp2C2=(te->ae)->datatypedecls;struct _tuple0*_tmp2C1=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp2C2,_tmp2C1,({struct Cyc_Absyn_Datatypedecl**_tmp17D=_cycalloc(sizeof(*_tmp17D));*_tmp17D=tud;_tmp17D;}));});(te->ae)->datatypedecls=_tmp2C3;});
goto _LL5;}else{_LLC: _tmp184=*_tmp17C.f2;_LLD: {
# 814
struct Cyc_Absyn_Datatypedecl*_tmp183=Cyc_Tcdecl_merge_datatypedecl(*_tmp184,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(_tmp183 == 0)
return;else{
# 819
*_tmp184=_tmp183;
goto _LL5;}}}}else{if(_tmp17C.f2 == 0){_LL8: _tmp185=(struct Cyc_List_List**)&(_tmp17C.f1)->v;_LL9: {
# 786
struct Cyc_Absyn_Datatypedecl**_tmp17E=({struct Cyc_Absyn_Datatypedecl**_tmp180=_cycalloc(sizeof(*_tmp180));({struct Cyc_Absyn_Datatypedecl*_tmp2C4=({struct Cyc_Absyn_Datatypedecl*_tmp17F=_cycalloc(sizeof(*_tmp17F));((((((((*_tmp17F).sc=Cyc_Absyn_Extern,(*_tmp17F).name=tud->name)),(*_tmp17F).tvs=tvs)),(*_tmp17F).fields=0)),(*_tmp17F).is_extensible=tud->is_extensible);_tmp17F;});*_tmp180=_tmp2C4;});_tmp180;});
# 788
({struct Cyc_Dict_Dict _tmp2C5=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,_tmp17E);(te->ae)->datatypedecls=_tmp2C5;});
# 791
({struct Cyc_List_List*_tmp2C6=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp185,tvs,tud);*_tmp185=_tmp2C6;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*_tmp17E=tud;
goto _LL5;}}else{_LLA: _tmp187=(struct Cyc_List_List**)&(_tmp17C.f1)->v;_tmp186=*_tmp17C.f2;_LLB: {
# 796
struct Cyc_Absyn_Datatypedecl*_tmp181=*_tmp186;
# 799
if((!tud->is_extensible  && (unsigned int)_tmp181) && _tmp181->is_extensible)
tud->is_extensible=1;
# 802
({struct Cyc_Absyn_Datatypedecl*_tmp2C7=({struct Cyc_Absyn_Datatypedecl*_tmp182=_cycalloc(sizeof(*_tmp182));((((((((*_tmp182).sc=Cyc_Absyn_Extern,(*_tmp182).name=tud->name)),(*_tmp182).tvs=tvs)),(*_tmp182).fields=0)),(*_tmp182).is_extensible=tud->is_extensible);_tmp182;});*_tmp186=_tmp2C7;});
# 806
({struct Cyc_List_List*_tmp2C8=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp187,tvs,tud);*_tmp187=_tmp2C8;});
# 809
*_tmp186=_tmp181;
_tmp184=_tmp186;goto _LLD;}}}_LL5:;};};}
# 825
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 832
if(ed->fields != 0){struct _RegionHandle _tmp18A=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp18A;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp18B=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 838
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp18B->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp18E;_tmp18E.tag=0U;_tmp18E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp18B->name).f2);({void*_tmp18C[1U];_tmp18C[0]=& _tmp18E;({unsigned int _tmp2CA=_tmp18B->loc;struct _dyneither_ptr _tmp2C9=({const char*_tmp18D="duplicate enum constructor %s";_tag_dyneither(_tmp18D,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp2CA,_tmp2C9,_tag_dyneither(_tmp18C,sizeof(void*),1U));});});});else{
# 841
({struct Cyc_List_List*_tmp2CB=({struct Cyc_List_List*_tmp18F=_region_malloc(uprev_rgn,sizeof(*_tmp18F));((*_tmp18F).hd=(*_tmp18B->name).f2,(*_tmp18F).tl=prev_fields);_tmp18F;});prev_fields=_tmp2CB;});}
# 843
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,_tmp18B->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp192;_tmp192.tag=0U;_tmp192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp18B->name).f2);({void*_tmp190[1U];_tmp190[0]=& _tmp192;({unsigned int _tmp2CD=_tmp18B->loc;struct _dyneither_ptr _tmp2CC=({const char*_tmp191="enum field name %s shadows global name";_tag_dyneither(_tmp191,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp2CD,_tmp2CC,_tag_dyneither(_tmp190,sizeof(void*),1U));});});});
# 846
if(_tmp18B->tag == 0){
({struct Cyc_Absyn_Exp*_tmp2CE=Cyc_Absyn_uint_exp(tag_count,_tmp18B->loc);_tmp18B->tag=_tmp2CE;});
++ tag_count;}else{
# 851
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp18B->tag))){
struct _tuple11 _tmp193=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp18B->tag));struct _tuple11 _tmp194=_tmp193;unsigned int _tmp196;int _tmp195;_LL1: _tmp196=_tmp194.f1;_tmp195=_tmp194.f2;_LL2:;
if(_tmp195)tag_count=_tmp196 + 1;}}}}
# 833
;_pop_region(uprev_rgn);}
# 859
{struct _handler_cons _tmp197;_push_handler(& _tmp197);{int _tmp199=0;if(setjmp(_tmp197.handler))_tmp199=1;if(!_tmp199){
{struct Cyc_Absyn_Enumdecl**_tmp19A=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);
struct Cyc_Absyn_Enumdecl*_tmp19B=Cyc_Tcdecl_merge_enumdecl(*_tmp19A,ed,loc,Cyc_Tc_tc_msg);
if(_tmp19B == 0){_npop_handler(0U);return;}
*_tmp19A=_tmp19B;}
# 860
;_pop_handler();}else{void*_tmp198=(void*)_exn_thrown;void*_tmp19C=_tmp198;void*_tmp19F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp19C)->tag == Cyc_Dict_Absent){_LL4: _LL5: {
# 865
struct Cyc_Absyn_Enumdecl**_tmp19D=({struct Cyc_Absyn_Enumdecl**_tmp19E=_cycalloc(sizeof(*_tmp19E));*_tmp19E=ed;_tmp19E;});
({struct Cyc_Dict_Dict _tmp2CF=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,_tmp19D);(te->ae)->enumdecls=_tmp2CF;});
goto _LL3;}}else{_LL6: _tmp19F=_tmp19C;_LL7:(int)_rethrow(_tmp19F);}_LL3:;}};}
# 871
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1A0=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1A0->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1A0->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1A4;_tmp1A4.tag=0U;_tmp1A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1A0->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1A3;_tmp1A3.tag=0U;_tmp1A3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp1A1[2U];(_tmp1A1[0]=& _tmp1A3,_tmp1A1[1]=& _tmp1A4);({unsigned int _tmp2D1=loc;struct _dyneither_ptr _tmp2D0=({const char*_tmp1A2="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1A2,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp2D1,_tmp2D0,_tag_dyneither(_tmp1A1,sizeof(void*),2U));});});});});}}}
# 881
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1A5=sc;switch(_tmp1A5){case Cyc_Absyn_Static: _LL1: _LL2:
# 884
 if(!in_include)
({void*_tmp1A6=0U;({unsigned int _tmp2D3=loc;struct _dyneither_ptr _tmp2D2=({const char*_tmp1A7="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1A7,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp2D3,_tmp2D2,_tag_dyneither(_tmp1A6,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Abstract: _LL3: _LL4:
# 888
({void*_tmp1A8=0U;({unsigned int _tmp2D5=loc;struct _dyneither_ptr _tmp2D4=({const char*_tmp1A9="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1A9,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp2D5,_tmp2D4,_tag_dyneither(_tmp1A8,sizeof(void*),0U));});});
return 0;case Cyc_Absyn_Public: _LL5: _LL6:
 goto _LL8;case Cyc_Absyn_Register: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LL9: _LLA:
 return 1;default: _LLB: _LLC:
# 894
({void*_tmp1AA=0U;({unsigned int _tmp2D7=loc;struct _dyneither_ptr _tmp2D6=({const char*_tmp1AB="nested extern \"C\" declaration";_tag_dyneither(_tmp1AB,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp2D7,_tmp2D6,_tag_dyneither(_tmp1AA,sizeof(void*),0U));});});
return 1;}_LL0:;}
# 904
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 909
struct Cyc_List_List*last=0;
# 911
struct Cyc_List_List*_tmp1AC=ds0;for(0;_tmp1AC != 0;(last=_tmp1AC,_tmp1AC=_tmp1AC->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1AC->hd;
unsigned int loc=d->loc;
# 915
void*_tmp1AD=d->r;void*_tmp1AE=_tmp1AD;struct Cyc_List_List*_tmp1C9;struct Cyc_List_List*_tmp1C8;struct Cyc_List_List*_tmp1C7;union Cyc_Absyn_Nmspace _tmp1C6;struct _dyneither_ptr*_tmp1C5;struct Cyc_List_List*_tmp1C4;struct _dyneither_ptr*_tmp1C3;struct Cyc_List_List*_tmp1C2;struct Cyc_Absyn_Enumdecl*_tmp1C1;struct Cyc_Absyn_Datatypedecl*_tmp1C0;struct Cyc_Absyn_Aggrdecl*_tmp1BF;struct Cyc_Absyn_Typedefdecl*_tmp1BE;struct Cyc_Absyn_Fndecl*_tmp1BD;struct Cyc_Absyn_Vardecl*_tmp1BC;switch(*((int*)_tmp1AE)){case 2U: _LL1: _LL2:
 goto _LL4;case 3U: _LL3: _LL4:
# 918
({void*_tmp1AF=0U;({unsigned int _tmp2D9=loc;struct _dyneither_ptr _tmp2D8=({const char*_tmp1B0="top level let-declarations are not implemented";_tag_dyneither(_tmp1B0,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp2D9,_tmp2D8,_tag_dyneither(_tmp1AF,sizeof(void*),0U));});});
goto _LL0;case 4U: _LL5: _LL6:
# 921
({void*_tmp1B1=0U;({unsigned int _tmp2DB=loc;struct _dyneither_ptr _tmp2DA=({const char*_tmp1B2="top level region declarations are not implemented";_tag_dyneither(_tmp1B2,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp2DB,_tmp2DA,_tag_dyneither(_tmp1B1,sizeof(void*),0U));});});
goto _LL0;case 0U: _LL7: _tmp1BC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LL8:
# 924
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1BC->sc,te->in_extern_c_include))
_tmp1BC->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,_tmp1BC,check_var_init,te->in_extern_c_include,exports);
goto _LL0;case 1U: _LL9: _tmp1BD=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LLA:
# 929
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1BD->sc,te->in_extern_c_include))
_tmp1BD->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,_tmp1BD,exports);
goto _LL0;case 8U: _LLB: _tmp1BE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LLC:
# 934
 _tmp1BE->extern_c=te->in_extern_c_include;
# 938
Cyc_Tc_tcTypedefdecl(te,loc,_tmp1BE);
goto _LL0;case 5U: _LLD: _tmp1BF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LLE:
# 941
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1BF->sc,te->in_extern_c_include))
_tmp1BF->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp1BF);
goto _LL0;case 6U: _LLF: _tmp1C0=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LL10:
# 946
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1C0->sc,te->in_extern_c_include))
_tmp1C0->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,_tmp1C0);
goto _LL0;case 7U: _LL11: _tmp1C1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LL12:
# 951
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1C1->sc,te->in_extern_c_include))
_tmp1C1->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp1C1);
goto _LL0;case 13U: _LL13: _LL14:
# 956
({void*_tmp1B3=0U;({unsigned int _tmp2DD=d->loc;struct _dyneither_ptr _tmp2DC=({const char*_tmp1B4="spurious __cyclone_port_on__";_tag_dyneither(_tmp1B4,sizeof(char),29U);});Cyc_Tcutil_warn(_tmp2DD,_tmp2DC,_tag_dyneither(_tmp1B3,sizeof(void*),0U));});});
goto _LL0;case 14U: _LL15: _LL16:
# 959
 goto _LL0;case 9U: _LL17: _tmp1C3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_tmp1C2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1AE)->f2;_LL18: {
# 962
struct Cyc_List_List*_tmp1B5=te->ns;
({struct Cyc_List_List*_tmp2DF=({struct Cyc_List_List*_tmp2DE=_tmp1B5;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2DE,({struct Cyc_List_List*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));((*_tmp1B6).hd=_tmp1C3,(*_tmp1B6).tl=0);_tmp1B6;}));});te->ns=_tmp2DF;});
Cyc_Tc_tc_decls(te,_tmp1C2,in_externC,check_var_init,exports);
te->ns=_tmp1B5;
goto _LL0;}case 10U: _LL19: _tmp1C6=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1)->f1;_tmp1C5=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1)->f2;_tmp1C4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1AE)->f2;_LL1A:
# 970
 Cyc_Tc_tc_decls(te,_tmp1C4,in_externC,check_var_init,exports);
goto _LL0;case 11U: _LL1B: _tmp1C7=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_LL1C:
# 974
 Cyc_Tc_tc_decls(te,_tmp1C7,1,check_var_init,exports);
goto _LL0;default: _LL1D: _tmp1C9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AE)->f1;_tmp1C8=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1AE)->f2;_LL1E: {
# 980
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C8,(unsigned int)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1B7=Cyc_Tcenv_enter_extern_c_include(te);
Cyc_Tc_tc_decls(_tmp1B7,_tmp1C9,1,check_var_init,& newexs);
# 985
for(0;_tmp1C8 != 0;_tmp1C8=_tmp1C8->tl){
struct _tuple12*_tmp1B8=(struct _tuple12*)_tmp1C8->hd;
if(!(*_tmp1B8).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=0U;({struct _dyneither_ptr _tmp2E0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1B8).f2));_tmp1BB.f1=_tmp2E0;});({void*_tmp1B9[1U];_tmp1B9[0]=& _tmp1BB;({unsigned int _tmp2E2=(*_tmp1B8).f1;struct _dyneither_ptr _tmp2E1=({const char*_tmp1BA="%s is exported but not defined";_tag_dyneither(_tmp1BA,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp2E2,_tmp2E1,_tag_dyneither(_tmp1B9,sizeof(void*),1U));});});});}
# 991
goto _LL0;}}_LL0:;}}
# 996
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1016 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1018
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1022
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1CA=d->r;void*_tmp1CB=_tmp1CA;struct Cyc_List_List**_tmp1D0;struct Cyc_List_List**_tmp1CF;struct Cyc_List_List**_tmp1CE;struct Cyc_List_List**_tmp1CD;struct Cyc_Absyn_Vardecl*_tmp1CC;switch(*((int*)_tmp1CB)){case 0U: _LL1: _tmp1CC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1CB)->f1;_LL2:
# 1027
 if((env->in_cinclude  || _tmp1CC->sc != Cyc_Absyn_Extern  && _tmp1CC->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp1CC->name,Cyc_Absyn_uniquergn_qvar))
# 1030
return 1;
# 1032
return(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,_tmp1CC->name)).f2;case 11U: _LL3: _tmp1CD=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1CB)->f1;_LL4:
 _tmp1CE=_tmp1CD;goto _LL6;case 10U: _LL5: _tmp1CE=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1CB)->f2;_LL6:
 _tmp1CF=_tmp1CE;goto _LL8;case 9U: _LL7: _tmp1CF=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1CB)->f2;_LL8:
# 1036
({struct Cyc_List_List*_tmp2E3=Cyc_Tc_treeshake_f(env,*_tmp1CF);*_tmp1CF=_tmp2E3;});
return 1;case 12U: _LL9: _tmp1D0=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1CB)->f1;_LLA: {
# 1039
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp2E4=Cyc_Tc_treeshake_f(env,*_tmp1D0);*_tmp1D0=_tmp2E4;});
env->in_cinclude=in_cinclude;
return 1;}default: _LLB: _LLC:
 return 1;}_LL0:;}
# 1048
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1D1=({struct Cyc_Tc_TreeshakeEnv _tmp1D7;(_tmp1D7.in_cinclude=0,_tmp1D7.ordinaries=(te->ae)->ordinaries);_tmp1D7;});
return Cyc_Tc_treeshake_f(& _tmp1D1,ds);}
