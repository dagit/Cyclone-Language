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
# 167 "core.h"
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
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
# 81
typedef unsigned int Cyc_Absyn_seg_t;
# 83
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple0*Cyc_Absyn_qvar_t;typedef struct _tuple0*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_t;
typedef struct _tuple0*Cyc_Absyn_typedef_name_opt_t;
# 107
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
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 210
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 212
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 221
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443 "absyn.h"
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 917 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 927
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 931
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 934
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 937
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 945
void*Cyc_Absyn_compress_kb(void*);
# 950
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 974
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 996
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1007
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1079
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1167
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 69
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned int,struct Cyc_Absyn_Fndecl*);
# 79
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
# 86
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 150
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
# 94
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 98
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 129
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 143
void Cyc_Tcutil_explain_failure();
# 145
int Cyc_Tcutil_unify(void*,void*);
# 172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 182
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 209 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 211
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 219
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 223
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 233
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 265
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
# 295
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 319
int Cyc_Tcutil_supports_default(void*);
# 330
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 334
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 354
int Cyc_Tcutil_is_array(void*t);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct _tuple10{void*f1;void*f2;};struct Cyc_Tcexp_TestEnv{struct _tuple10*eq;int isTrue;};
# 35
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);struct _tuple11{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 33
void Cyc_Tc_tc(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 37
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);
# 40
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 34 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
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
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);static char _tmp0[1]="";
# 37 "tc.cyc"
static struct _dyneither_ptr Cyc_Tc_tc_msg_c={_tmp0,_tmp0,_tmp0 + 1};
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
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att));({void*_tmp2[1]={& _tmp4};Cyc_Tcutil_terr(loc,({const char*_tmp3="%s has an out-of-range index";_tag_dyneither(_tmp3,sizeof(char),29);}),_tag_dyneither(_tmp2,sizeof(void*),1));});});
return 0;}
# 61
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 63
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 66
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_Int_pa_PrintArg_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(unsigned long)i;({struct Cyc_String_pa_PrintArg_struct _tmp8;_tmp8.tag=0;_tmp8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2);({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2);({void*_tmp5[3]={& _tmp7,& _tmp8,& _tmp9};Cyc_Tcutil_terr(loc,({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_dyneither(_tmp6,sizeof(char),54);}),_tag_dyneither(_tmp5,sizeof(void*),3));});});});});}
# 70
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _RegionHandle*_tmpA=Cyc_Core_heap_region;
struct _tuple13 init_params=({struct _tuple13 _tmp3B;_tmp3B.f1=0;_tmp3B.f2=({const char*_tmp3C="initializes";_tag_dyneither(_tmp3C,sizeof(char),12);});_tmp3B;});
struct _tuple13 nolive_unique_params=({struct _tuple13 _tmp39;_tmp39.f1=0;_tmp39.f2=({const char*_tmp3A="noliveunique";_tag_dyneither(_tmp3A,sizeof(char),13);});_tmp39;});
struct _tuple13 noconsume_params=({struct _tuple13 _tmp37;_tmp37.f1=0;_tmp37.f2=({const char*_tmp38="noconsume";_tag_dyneither(_tmp38,sizeof(char),10);});_tmp37;});
void*_tmpB=Cyc_Tcutil_compress(t);void*_tmpC=_tmpB;struct Cyc_List_List*_tmp36;struct Cyc_List_List*_tmp35;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->tag == 9){_LL1: _tmp36=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).attributes;_tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpC)->f1).args;_LL2: {
# 77
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp35);
for(0;_tmp36 != 0;_tmp36=_tmp36->tl){
void*_tmpD=(void*)_tmp36->hd;void*_tmpE=_tmpD;int _tmp32;int _tmp31;int _tmp30;switch(*((int*)_tmpE)){case 20: _LL6: _tmp30=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpE)->f1;_LL7:
# 81
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp30,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,init_params,noconsume_params);{
struct _tuple8*_tmpF=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp30 - 1);struct _tuple8*_tmp10=_tmpF;void*_tmp25;_LLF: _tmp25=_tmp10->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp24="initializes attribute allowed only on";_tag_dyneither(_tmp24,sizeof(char),38);});
{void*_tmp11=Cyc_Tcutil_compress(_tmp25);void*_tmp12=_tmp11;void*_tmp22;union Cyc_Absyn_Constraint*_tmp21;union Cyc_Absyn_Constraint*_tmp20;union Cyc_Absyn_Constraint*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->tag == 5){_LL12: _tmp22=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).elt_typ;_tmp21=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).nullable;_tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).bounds;_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12)->f1).ptr_atts).zero_term;_LL13:
# 88
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp21))
({struct Cyc_String_pa_PrintArg_struct _tmp15;_tmp15.tag=0;_tmp15.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp13[1]={& _tmp15};Cyc_Tcutil_terr(loc,({const char*_tmp14="%s non-null pointers";_tag_dyneither(_tmp14,sizeof(char),21);}),_tag_dyneither(_tmp13,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_bound_one(_tmp20))
({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0;_tmp18.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp16[1]={& _tmp18};Cyc_Tcutil_terr(loc,({const char*_tmp17="%s pointers of size 1";_tag_dyneither(_tmp17,sizeof(char),22);}),_tag_dyneither(_tmp16,sizeof(void*),1));});});
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1F))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp19[1]={& _tmp1B};Cyc_Tcutil_terr(loc,({const char*_tmp1A="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp1A,sizeof(char),42);}),_tag_dyneither(_tmp19,sizeof(void*),1));});});
goto _LL11;}else{_LL14: _LL15:
# 96
({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1C[1]={& _tmp1E};Cyc_Tcutil_terr(loc,({const char*_tmp1D="%s pointers";_tag_dyneither(_tmp1D,sizeof(char),12);}),_tag_dyneither(_tmp1C,sizeof(void*),1));});});}_LL11:;}
# 98
init_params.f1=({struct Cyc_List_List*_tmp23=_region_malloc(_tmpA,sizeof(*_tmp23));_tmp23->hd=(void*)_tmp30;_tmp23->tl=init_params.f1;_tmp23;});
goto _LL5;};};case 21: _LL8: _tmp31=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpE)->f1;_LL9:
# 101
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp31,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp26=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp31 - 1);struct _tuple8*_tmp27=_tmp26;void*_tmp2B;_LL17: _tmp2B=_tmp27->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2B,0))
({void*_tmp28=0;Cyc_Tcutil_terr(loc,({const char*_tmp29="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp29,sizeof(char),55);}),_tag_dyneither(_tmp28,sizeof(void*),0));});
nolive_unique_params.f1=({struct Cyc_List_List*_tmp2A=_region_malloc(_tmpA,sizeof(*_tmp2A));_tmp2A->hd=(void*)_tmp31;_tmp2A->tl=nolive_unique_params.f1;_tmp2A;});
goto _LL5;};case 22: _LLA: _tmp32=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpE)->f1;_LLB:
# 110
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp32,nargs,(void*)_tmp36->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp32,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp2C=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp35,_tmp32 - 1);struct _tuple8*_tmp2D=_tmp2C;void*_tmp2F;_LL1A: _tmp2F=_tmp2D->f3;_LL1B:;
# 115
noconsume_params.f1=({struct Cyc_List_List*_tmp2E=_region_malloc(_tmpA,sizeof(*_tmp2E));_tmp2E->hd=(void*)_tmp32;_tmp2E->tl=noconsume_params.f1;_tmp2E;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 120
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp33=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp34="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp34,sizeof(char),34);}),_tag_dyneither(_tmp33,sizeof(void*),0));});}_LL0:;}struct _tuple14{void*f1;int f2;};
# 126
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 128
struct Cyc_Absyn_Vardecl*_tmp3D=vd;enum Cyc_Absyn_Scope _tmp97;struct _tuple0*_tmp96;void*_tmp95;struct Cyc_Absyn_Exp*_tmp94;struct Cyc_List_List*_tmp93;_LL1D: _tmp97=_tmp3D->sc;_tmp96=_tmp3D->name;_tmp95=_tmp3D->type;_tmp94=_tmp3D->initializer;_tmp93=_tmp3D->attributes;_LL1E:;
# 135
{void*_tmp3E=Cyc_Tcutil_compress(_tmp95);void*_tmp3F=_tmp3E;void*_tmp4B;struct Cyc_Absyn_Tqual _tmp4A;union Cyc_Absyn_Constraint*_tmp49;unsigned int _tmp48;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->tag == 8){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).num_elts == 0){_LL20: _tmp4B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).tq;_tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;_tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zt_loc;if(_tmp94 != 0){_LL21:
# 137
{void*_tmp40=_tmp94->r;void*_tmp41=_tmp40;struct Cyc_List_List*_tmp47;struct Cyc_List_List*_tmp46;struct Cyc_Absyn_Exp*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct _dyneither_ptr _tmp43;struct _dyneither_ptr _tmp42;switch(*((int*)_tmp41)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).Wstring_c).tag){case 8: _LL25: _tmp42=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).String_c).val;_LL26:
# 139
 _tmp95=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp42,sizeof(char)),0),_tmp49,_tmp48));
goto _LL24;case 9: _LL27: _tmp43=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41)->f1).Wstring_c).val;_LL28:
# 142
 _tmp95=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp(1,0),_tmp49,_tmp48));
goto _LL24;default: goto _LL31;}case 26: _LL29: _tmp44=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL2A:
 _tmp45=_tmp44;goto _LL2C;case 27: _LL2B: _tmp45=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_LL2C:
# 147
 _tmp95=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,_tmp45,_tmp49,_tmp48));
goto _LL24;case 35: _LL2D: _tmp46=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp41)->f2;_LL2E:
 _tmp47=_tmp46;goto _LL30;case 25: _LL2F: _tmp47=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp41)->f1;_LL30:
# 151
 _tmp95=(vd->type=Cyc_Absyn_array_typ(_tmp4B,_tmp4A,Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp47),0),_tmp49,_tmp48));
goto _LL24;default: _LL31: _LL32:
 goto _LL24;}_LL24:;}
# 155
goto _LL1F;}else{goto _LL22;}}else{goto _LL22;}}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 159
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp95);
# 161
(vd->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp95);
# 164
vd->escapes=!Cyc_Tcutil_is_array(_tmp95);
# 166
if(Cyc_Tcutil_is_function_type(_tmp95)){
_tmp93=Cyc_Tcutil_transfer_fn_type_atts(_tmp95,_tmp93);
Cyc_Tc_fnTypeAttsOK(te,_tmp95,loc);}
# 171
if(_tmp97 == Cyc_Absyn_Extern  || _tmp97 == Cyc_Absyn_ExternC){
if(_tmp94 != 0  && !in_cinclude)
({void*_tmp4C=0;Cyc_Tcutil_terr(loc,({const char*_tmp4D="extern declaration should not have initializer";_tag_dyneither(_tmp4D,sizeof(char),47);}),_tag_dyneither(_tmp4C,sizeof(void*),0));});}else{
if(!Cyc_Tcutil_is_function_type(_tmp95)){
# 178
for(0;_tmp93 != 0;_tmp93=_tmp93->tl){
void*_tmp4E=(void*)_tmp93->hd;void*_tmp4F=_tmp4E;switch(*((int*)_tmp4F)){case 6: _LL34: _LL35:
 goto _LL37;case 8: _LL36: _LL37:
# 183
 goto _LL39;case 9: _LL38: _LL39:
 goto _LL3B;case 10: _LL3A: _LL3B:
 goto _LL3D;case 11: _LL3C: _LL3D:
 goto _LL3F;case 12: _LL3E: _LL3F:
 goto _LL41;case 13: _LL40: _LL41:
 goto _LL43;case 14: _LL42: _LL43:
 continue;default: _LL44: _LL45:
# 191
({struct Cyc_String_pa_PrintArg_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name));({struct Cyc_String_pa_PrintArg_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp93->hd));({void*_tmp50[2]={& _tmp52,& _tmp53};Cyc_Tcutil_terr(loc,({const char*_tmp51="bad attribute %s for variable %s";_tag_dyneither(_tmp51,sizeof(char),33);}),_tag_dyneither(_tmp50,sizeof(void*),2));});});});
goto _LL33;}_LL33:;}
# 196
if(_tmp94 == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp95))
({struct Cyc_String_pa_PrintArg_struct _tmp57;_tmp57.tag=0;_tmp57.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp95));({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0;_tmp56.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));({void*_tmp54[2]={& _tmp56,& _tmp57};Cyc_Tcutil_terr(loc,({const char*_tmp55="initializer required for variable %s of type %s";_tag_dyneither(_tmp55,sizeof(char),48);}),_tag_dyneither(_tmp54,sizeof(void*),2));});});});}else{
# 201
struct Cyc_Absyn_Exp*_tmp58=_tmp94;
# 205
struct _handler_cons _tmp59;_push_handler(& _tmp59);{int _tmp5B=0;if(setjmp(_tmp59.handler))_tmp5B=1;if(!_tmp5B){
{void*_tmp5C=Cyc_Tcexp_tcExpInitializer(te,& _tmp95,_tmp58);
if(!Cyc_Tcutil_coerce_assign(te,_tmp58,_tmp95)){
struct _dyneither_ptr _tmp5D=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp5E=" declared with type ";
struct _dyneither_ptr _tmp5F=Cyc_Absynpp_typ2string(_tmp95);
const char*_tmp60=" but initialized with type ";
struct _dyneither_ptr _tmp61=Cyc_Absynpp_typ2string(_tmp5C);
if((((Cyc_strlen((struct _dyneither_ptr)_tmp5D)+ Cyc_strlen(({const char*_tmp62=_tmp5E;_tag_dyneither(_tmp62,sizeof(char),_get_zero_arr_size_char((void*)_tmp62,21));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp5F))+ Cyc_strlen(({const char*_tmp63=_tmp60;_tag_dyneither(_tmp63,sizeof(char),_get_zero_arr_size_char((void*)_tmp63,28));})))+ Cyc_strlen((struct _dyneither_ptr)_tmp61)> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp61);({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dyneither_ptr)({const char*_tmp6C=_tmp60;_tag_dyneither(_tmp6C,sizeof(char),_get_zero_arr_size_char((void*)_tmp6C,28));});({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0;_tmp68.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);({struct Cyc_String_pa_PrintArg_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _dyneither_ptr)({const char*_tmp6B=_tmp5E;_tag_dyneither(_tmp6B,sizeof(char),_get_zero_arr_size_char((void*)_tmp6B,21));});({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);({void*_tmp64[5]={& _tmp66,& _tmp67,& _tmp68,& _tmp69,& _tmp6A};Cyc_Tcutil_terr(loc,({const char*_tmp65="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp65,sizeof(char),16);}),_tag_dyneither(_tmp64,sizeof(void*),5));});});});});});});else{
# 216
({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp61);({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)({const char*_tmp75=_tmp60;_tag_dyneither(_tmp75,sizeof(char),_get_zero_arr_size_char((void*)_tmp75,28));});({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;_tmp71.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5F);({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(struct _dyneither_ptr)({const char*_tmp74=_tmp5E;_tag_dyneither(_tmp74,sizeof(char),_get_zero_arr_size_char((void*)_tmp74,21));});({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0;_tmp6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5D);({void*_tmp6D[5]={& _tmp6F,& _tmp70,& _tmp71,& _tmp72,& _tmp73};Cyc_Tcutil_terr(loc,({const char*_tmp6E="%s%s%s%s%s";_tag_dyneither(_tmp6E,sizeof(char),11);}),_tag_dyneither(_tmp6D,sizeof(void*),5));});});});});});});}
Cyc_Tcutil_explain_failure();
# 219
if(!Cyc_Tcutil_is_const_exp(_tmp58))
({void*_tmp76=0;Cyc_Tcutil_terr(loc,({const char*_tmp77="initializer is not a constant expression";_tag_dyneither(_tmp77,sizeof(char),41);}),_tag_dyneither(_tmp76,sizeof(void*),0));});}}
# 206
;_pop_handler();}else{void*_tmp5A=(void*)_exn_thrown;void*_tmp78=_tmp5A;void*_tmp7B;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp78)->tag == Cyc_Tcenv_Env_error){_LL47: _LL48:
# 224
({void*_tmp79=0;Cyc_Tcutil_terr(loc,({const char*_tmp7A="initializer is not a constant expression";_tag_dyneither(_tmp7A,sizeof(char),41);}),_tag_dyneither(_tmp79,sizeof(void*),0));});
goto _LL46;}else{_LL49: _tmp7B=_tmp78;_LL4A:(int)_rethrow(_tmp7B);}_LL46:;}};}}else{
# 230
for(0;_tmp93 != 0;_tmp93=_tmp93->tl){
void*_tmp7C=(void*)_tmp93->hd;void*_tmp7D=_tmp7C;switch(*((int*)_tmp7D)){case 0: _LL4C: _LL4D:
# 233
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
# 244
 goto _LL63;case 7: _LL62: _LL63:
# 246
({struct Cyc_String_pa_PrintArg_struct _tmp82;_tmp82.tag=0;_tmp82.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp93->hd));({void*_tmp80[1]={& _tmp82};Cyc_Tcutil_terr(loc,({const char*_tmp81="bad attribute %s in function declaration";_tag_dyneither(_tmp81,sizeof(char),41);}),_tag_dyneither(_tmp80,sizeof(void*),1));});});
goto _LL4B;default: _LL64: _LL65:
 continue;}_LL4B:;}}}
# 254
{struct _handler_cons _tmp83;_push_handler(& _tmp83);{int _tmp85=0;if(setjmp(_tmp83.handler))_tmp85=1;if(!_tmp85){
{struct _tuple14*_tmp86=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,_tmp96);
void*_tmp87=(*_tmp86).f1;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp88=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp8D;_tmp8D.tag=1;_tmp8D.f1=vd;_tmp8D;});_tmp8C;});
void**_tmp89=Cyc_Tcdecl_merge_binding(_tmp87,(void*)_tmp88,loc,Cyc_Tc_tc_msg);
if(_tmp89 == 0){_npop_handler(0);return;}{
void*_tmp8A=*_tmp89;
# 262
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp8A != _tmp87  || (*_tmp86).f2)
# 265
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp96,({struct _tuple14*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->f1=_tmp8A;_tmp8B->f2=1;_tmp8B;}));}
_npop_handler(0);return;};}
# 255
;_pop_handler();}else{void*_tmp84=(void*)_exn_thrown;void*_tmp8E=_tmp84;void*_tmp8F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8E)->tag == Cyc_Dict_Absent){_LL67: _LL68:
# 267
 goto _LL66;}else{_LL69: _tmp8F=_tmp8E;_LL6A:(int)_rethrow(_tmp8F);}_LL66:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp96,({struct _tuple14*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->f1=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp92;_tmp92.tag=1;_tmp92.f1=vd;_tmp92;});_tmp91;});_tmp90->f2=0;_tmp90;}));}
# 273
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp98=n;union Cyc_Absyn_Nmspace _tmp9C;struct _dyneither_ptr*_tmp9B;_LL6C: _tmp9C=_tmp98->f1;_tmp9B=_tmp98->f2;_LL6D:;{
union Cyc_Absyn_Nmspace _tmp99=_tmp9C;if((_tmp99.Abs_n).tag == 2){if((_tmp99.Abs_n).val == 0){_LL6F: _LL70:
# 277
 return Cyc_strcmp((struct _dyneither_ptr)*_tmp9B,({const char*_tmp9A="main";_tag_dyneither(_tmp9A,sizeof(char),5);}))== 0;}else{goto _LL71;}}else{_LL71: _LL72:
 return 0;}_LL6E:;};}
# 282
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 284
struct _tuple0*q=fd->name;
# 288
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include)
({void*_tmp9D=0;Cyc_Tcutil_terr(loc,({const char*_tmp9E="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmp9E,sizeof(char),54);}),_tag_dyneither(_tmp9D,sizeof(void*),0));});
# 292
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 294
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 296
fd->attributes=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 299
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp9F=(void*)atts->hd;void*_tmpA0=_tmp9F;switch(*((int*)_tmpA0)){case 7: _LL74: _LL75:
 goto _LL77;case 6: _LL76: _LL77:
# 303
({struct Cyc_String_pa_PrintArg_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmpA1[1]={& _tmpA3};Cyc_Tcutil_terr(loc,({const char*_tmpA2="bad attribute %s for function";_tag_dyneither(_tmpA2,sizeof(char),30);}),_tag_dyneither(_tmpA1,sizeof(void*),1));});});
goto _LL73;default: _LL78: _LL79:
 goto _LL73;}_LL73:;}}
# 309
{struct _handler_cons _tmpA4;_push_handler(& _tmpA4);{int _tmpA6=0;if(setjmp(_tmpA4.handler))_tmpA6=1;if(!_tmpA6){
{struct _tuple14*_tmpA7=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
void*_tmpA8=(*_tmpA7).f1;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA9=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpAE;_tmpAE.tag=2;_tmpAE.f1=fd;_tmpAE;});_tmpAD;});
void**_tmpAA=Cyc_Tcdecl_merge_binding(_tmpA8,(void*)_tmpA9,loc,Cyc_Tc_tc_msg);
if(_tmpAA != 0){
void*_tmpAB=*_tmpAA;
# 317
if(exports == 0  || Cyc_Tc_export_member(q,*exports)){
if(!(_tmpAB == _tmpA8  && (*_tmpA7).f2))
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,q,({struct _tuple14*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->f1=_tmpAB;_tmpAC->f2=1;_tmpAC;}));}}}
# 310
;_pop_handler();}else{void*_tmpA5=(void*)_exn_thrown;void*_tmpAF=_tmpA5;void*_tmpB6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpAF)->tag == Cyc_Dict_Absent){_LL7B: _LL7C:
# 323
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB0=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpB5;_tmpB5.tag=2;_tmpB5.f1=fd;_tmpB5;});_tmpB4;});
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)((te->ae)->ordinaries,q,({struct _tuple14*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->f1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2[0]=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpB3;_tmpB3.tag=2;_tmpB3.f1=fd;_tmpB3;});_tmpB2;});_tmpB1->f2=0;_tmpB1;}));}
# 328
goto _LL7A;}else{_LL7D: _tmpB6=_tmpAF;_LL7E:(int)_rethrow(_tmpB6);}_LL7A:;}};}
# 332
if(te->in_extern_c_include)return;{
# 337
struct Cyc_Tcenv_Fenv*_tmpB7=Cyc_Tcenv_new_fenv(loc,fd);
struct Cyc_Tcenv_Tenv*_tmpB8=({struct Cyc_Tcenv_Tenv*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->ns=te->ns;_tmpDE->ae=te->ae;_tmpDE->le=_tmpB7;_tmpDE->allow_valueof=0;_tmpDE->in_extern_c_include=0;_tmpDE;});
# 340
Cyc_Tcstmt_tcStmt(_tmpB8,fd->body,0);
# 343
Cyc_Tcenv_check_delayed_effects(_tmpB8);
Cyc_Tcenv_check_delayed_constraints(_tmpB8);
# 346
if(Cyc_Tc_is_main(q)){
# 348
{void*_tmpB9=Cyc_Tcutil_compress(fd->ret_type);void*_tmpBA=_tmpB9;enum Cyc_Absyn_Size_of _tmpC0;switch(*((int*)_tmpBA)){case 0: _LL80: _LL81:
({void*_tmpBB=0;Cyc_Tcutil_warn(loc,({const char*_tmpBC="main declared with return type void";_tag_dyneither(_tmpBC,sizeof(char),36);}),_tag_dyneither(_tmpBB,sizeof(void*),0));});goto _LL7F;case 6: _LL82: _tmpC0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpBA)->f2;_LL83:
 goto _LL7F;default: _LL84: _LL85:
# 352
({struct Cyc_String_pa_PrintArg_struct _tmpBF;_tmpBF.tag=0;_tmpBF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type));({void*_tmpBD[1]={& _tmpBF};Cyc_Tcutil_terr(loc,({const char*_tmpBE="main declared with return type %s instead of int or void";_tag_dyneither(_tmpBE,sizeof(char),57);}),_tag_dyneither(_tmpBD,sizeof(void*),1));});});
goto _LL7F;}_LL7F:;}
# 356
if(fd->c_varargs  || fd->cyc_varargs != 0)
({void*_tmpC1=0;Cyc_Tcutil_terr(loc,({const char*_tmpC2="main declared with varargs";_tag_dyneither(_tmpC2,sizeof(char),27);}),_tag_dyneither(_tmpC1,sizeof(void*),0));});{
struct Cyc_List_List*_tmpC3=fd->args;
if(_tmpC3 != 0){
struct _tuple8*_tmpC4=(struct _tuple8*)_tmpC3->hd;struct _tuple8*_tmpC5=_tmpC4;void*_tmpDD;_LL87: _tmpDD=_tmpC5->f3;_LL88:;
{void*_tmpC6=Cyc_Tcutil_compress(_tmpDD);void*_tmpC7=_tmpC6;enum Cyc_Absyn_Size_of _tmpCB;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC7)->tag == 6){_LL8A: _tmpCB=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC7)->f2;_LL8B:
 goto _LL89;}else{_LL8C: _LL8D:
# 364
({struct Cyc_String_pa_PrintArg_struct _tmpCA;_tmpCA.tag=0;_tmpCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpDD));({void*_tmpC8[1]={& _tmpCA};Cyc_Tcutil_terr(loc,({const char*_tmpC9="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpC9,sizeof(char),60);}),_tag_dyneither(_tmpC8,sizeof(void*),1));});});
goto _LL89;}_LL89:;}
# 368
_tmpC3=_tmpC3->tl;
if(_tmpC3 != 0){
struct _tuple8*_tmpCC=(struct _tuple8*)_tmpC3->hd;struct _tuple8*_tmpCD=_tmpCC;void*_tmpDC;_LL8F: _tmpDC=_tmpCD->f3;_LL90:;
_tmpC3=_tmpC3->tl;
if(_tmpC3 != 0)
({void*_tmpCE=0;Cyc_Tcutil_terr(loc,({const char*_tmpCF="main declared with too many arguments";_tag_dyneither(_tmpCF,sizeof(char),38);}),_tag_dyneither(_tmpCE,sizeof(void*),0));});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpDB=_cycalloc(sizeof(*_tmpDB));_tmpDB->v=fd->tvs;_tmpDB;});
if(((!Cyc_Tcutil_unify(_tmpDC,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=& Cyc_Tcutil_rk;_tmpD0;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->v=& Cyc_Tcutil_rk;_tmpD1;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())) && !
Cyc_Tcutil_unify(_tmpDC,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->v=& Cyc_Tcutil_rk;_tmpD2;}),tvs)),
# 380
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->v=& Cyc_Tcutil_rk;_tmpD3;}),tvs),
Cyc_Absyn_empty_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpDC,Cyc_Absyn_dyneither_typ(Cyc_Absyn_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->v=& Cyc_Tcutil_rk;_tmpD4;}),tvs)),
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->v=& Cyc_Tcutil_rk;_tmpD5;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)()))) && !
Cyc_Tcutil_unify(_tmpDC,Cyc_Absyn_dyneither_typ(Cyc_Absyn_const_string_typ(Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->v=& Cyc_Tcutil_rk;_tmpD6;}),tvs)),
# 387
Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD7=_cycalloc(sizeof(*_tmpD7));_tmpD7->v=& Cyc_Tcutil_rk;_tmpD7;}),tvs),
Cyc_Absyn_const_tqual(0),((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)())))
({struct Cyc_String_pa_PrintArg_struct _tmpDA;_tmpDA.tag=0;_tmpDA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpDC));({void*_tmpD8[1]={& _tmpDA};Cyc_Tcutil_terr(loc,({const char*_tmpD9="second argument of main has type %s instead of char??";_tag_dyneither(_tmpD9,sizeof(char),54);}),_tag_dyneither(_tmpD8,sizeof(void*),1));});});};}}};}};};}
# 397
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 404
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;_tmpE1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmpDF[1]={& _tmpE1};Cyc_Tcutil_terr(loc,({const char*_tmpE0="redeclaration of typedef %s";_tag_dyneither(_tmpE0,sizeof(char),28);}),_tag_dyneither(_tmpDF,sizeof(void*),1));});});
return;}
# 409
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
(td->tq).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));}
# 418
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpE2=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmpE3=_tmpE2;struct Cyc_Core_Opt**_tmpEB;struct Cyc_Absyn_Kind*_tmpEA;struct Cyc_Core_Opt**_tmpE9;switch(*((int*)_tmpE3)){case 1: _LL92: _tmpE9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpE3)->f1;_LL93:
# 421
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpE6;_tmpE6.tag=0;_tmpE6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);({void*_tmpE4[1]={& _tmpE6};Cyc_Tcutil_warn(loc,({const char*_tmpE5="type variable %s is not used in typedef definition";_tag_dyneither(_tmpE5,sizeof(char),51);}),_tag_dyneither(_tmpE4,sizeof(void*),1));});});
# 424
*_tmpE9=({struct Cyc_Core_Opt*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpE7;});goto _LL91;case 2: _LL94: _tmpEB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE3)->f1;_tmpEA=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE3)->f2;_LL95:
# 431
*_tmpEB=({struct Cyc_Core_Opt*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->v=Cyc_Tcutil_kind_to_bound(_tmpEA);_tmpE8;});
goto _LL91;default: _LL96: _LL97:
 continue;}_LL91:;}}
# 437
(te->ae)->typedefs=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);}
# 440
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 447
struct _RegionHandle _tmpEC=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpEC;_push_region(uprev_rgn);
# 449
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmpED=(struct _tuple10*)rpo->hd;struct _tuple10*_tmpEE=_tmpED;void*_tmpF0;void*_tmpEF;_LL99: _tmpF0=_tmpEE->f1;_tmpEF=_tmpEE->f2;_LL9A:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmpF0);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmpEF);}{
# 455
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 458
struct Cyc_List_List*_tmpF1=fields;for(0;_tmpF1 != 0;_tmpF1=_tmpF1->tl){
struct Cyc_Absyn_Aggrfield*_tmpF2=(struct Cyc_Absyn_Aggrfield*)_tmpF1->hd;struct Cyc_Absyn_Aggrfield*_tmpF3=_tmpF2;struct _dyneither_ptr*_tmp10F;struct Cyc_Absyn_Tqual _tmp10E;void*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;_LL9C: _tmp10F=_tmpF3->name;_tmp10E=_tmpF3->tq;_tmp10D=_tmpF3->type;_tmp10C=_tmpF3->width;_tmp10B=_tmpF3->attributes;_tmp10A=_tmpF3->requires_clause;_LL9D:;
# 461
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp10F))
({struct Cyc_String_pa_PrintArg_struct _tmpF6;_tmpF6.tag=0;_tmpF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp10F);({void*_tmpF4[1]={& _tmpF6};Cyc_Tcutil_terr(loc,({const char*_tmpF5="duplicate member %s";_tag_dyneither(_tmpF5,sizeof(char),20);}),_tag_dyneither(_tmpF4,sizeof(void*),1));});});
# 465
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp10F,({const char*_tmpF7="";_tag_dyneither(_tmpF7,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmpF8=_region_malloc(uprev_rgn,sizeof(*_tmpF8));_tmpF8->hd=_tmp10F;_tmpF8->tl=prev_fields;_tmpF8;});{
# 468
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 472
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmpF1->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp10D);
# 477
(((struct Cyc_Absyn_Aggrfield*)_tmpF1->hd)->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpF1->hd)->tq).print_const,_tmp10D);
# 480
Cyc_Tcutil_check_bitfield(loc,te,_tmp10D,_tmp10C,_tmp10F);
# 482
if((unsigned int)_tmp10A){
if(str_or_union != Cyc_Absyn_UnionA)
({void*_tmpF9=0;Cyc_Tcutil_terr(loc,({const char*_tmpFA="@requires clauses are only allowed on union members";_tag_dyneither(_tmpFA,sizeof(char),52);}),_tag_dyneither(_tmpF9,sizeof(void*),0));});{
struct Cyc_Tcenv_Tenv*_tmpFB=Cyc_Tcenv_allow_valueof(te);
Cyc_Tcexp_tcExp(_tmpFB,0,_tmp10A);
if(!Cyc_Tcutil_is_integral(_tmp10A))
({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0;_tmpFE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 490
Cyc_Absynpp_typ2string((void*)_check_null(_tmp10A->topt)));({void*_tmpFC[1]={& _tmpFE};Cyc_Tcutil_terr(_tmp10A->loc,({const char*_tmpFD="@requires clause has type %s instead of integral type";_tag_dyneither(_tmpFD,sizeof(char),54);}),_tag_dyneither(_tmpFC,sizeof(void*),1));});});else{
# 492
Cyc_Tcutil_check_type(_tmp10A->loc,te,tvs,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp100;_tmp100.tag=18;_tmp100.f1=_tmp10A;_tmp100;});_tmpFF;}));{
# 494
struct Cyc_List_List*_tmp101=Cyc_Relations_exp2relns(uprev_rgn,_tmp10A);
# 501
if(!Cyc_Relations_consistent_relations(_tmp101))
({void*_tmp102=0;Cyc_Tcutil_terr(_tmp10A->loc,({const char*_tmp103="@requires clause may be unsatisfiable";_tag_dyneither(_tmp103,sizeof(char),38);}),_tag_dyneither(_tmp102,sizeof(void*),0));});
# 507
{struct Cyc_List_List*_tmp104=prev_relations;for(0;_tmp104 != 0;_tmp104=_tmp104->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp101,(struct Cyc_List_List*)_tmp104->hd)))
({void*_tmp105=0;Cyc_Tcutil_terr(_tmp10A->loc,({const char*_tmp106="@requires clause may overlap with previous clauses";_tag_dyneither(_tmp106,sizeof(char),51);}),_tag_dyneither(_tmp105,sizeof(void*),0));});}}
# 512
prev_relations=({struct Cyc_List_List*_tmp107=_region_malloc(uprev_rgn,sizeof(*_tmp107));_tmp107->hd=_tmp101;_tmp107->tl=prev_relations;_tmp107;});};}};}else{
# 515
if(prev_relations != 0)
({void*_tmp108=0;Cyc_Tcutil_terr(loc,({const char*_tmp109="if one field has a @requires clause, they all must";_tag_dyneither(_tmp109,sizeof(char),51);}),_tag_dyneither(_tmp108,sizeof(void*),0));});}};}};
# 449
;_pop_region(uprev_rgn);}
# 521
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 523
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp110=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp111=_tmp110;enum Cyc_Absyn_AliasQual _tmp126;struct Cyc_Core_Opt**_tmp125;struct Cyc_Absyn_Kind*_tmp124;struct Cyc_Core_Opt**_tmp123;struct Cyc_Core_Opt**_tmp122;enum Cyc_Absyn_AliasQual _tmp121;struct Cyc_Core_Opt**_tmp120;switch(*((int*)_tmp111)){case 1: _LL9F: _tmp120=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp111)->f1;_LLA0:
# 526
*_tmp120=({struct Cyc_Core_Opt*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp112;});continue;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp111)->f2)->kind){case Cyc_Absyn_MemKind: _LLA1: _tmp122=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp111)->f1;_tmp121=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp111)->f2)->aliasqual;_LLA2:
# 528
 if(constrain_top_kind  && _tmp121 == Cyc_Absyn_Top)
*_tmp122=({struct Cyc_Core_Opt*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp114=_cycalloc_atomic(sizeof(*_tmp114));_tmp114->kind=Cyc_Absyn_BoxKind;_tmp114->aliasqual=Cyc_Absyn_Aliasable;_tmp114;}));_tmp113;});else{
# 531
*_tmp122=({struct Cyc_Core_Opt*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp116=_cycalloc_atomic(sizeof(*_tmp116));_tmp116->kind=Cyc_Absyn_BoxKind;_tmp116->aliasqual=_tmp121;_tmp116;}));_tmp115;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp111)->f2)->aliasqual == Cyc_Absyn_Top){_LLA3: _tmp123=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp111)->f1;if(constrain_top_kind){_LLA4:
# 534
*_tmp123=({struct Cyc_Core_Opt*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->v=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp118=_cycalloc_atomic(sizeof(*_tmp118));_tmp118->kind=Cyc_Absyn_BoxKind;_tmp118->aliasqual=Cyc_Absyn_Aliasable;_tmp118;}));_tmp117;});
continue;}else{goto _LLA5;}}else{goto _LLA5;}default: _LLA5: _tmp125=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp111)->f1;_tmp124=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp111)->f2;_LLA6:
# 537
*_tmp125=({struct Cyc_Core_Opt*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->v=Cyc_Tcutil_kind_to_bound(_tmp124);_tmp119;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp111)->f1)->kind == Cyc_Absyn_MemKind){_LLA7: _tmp126=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp111)->f1)->aliasqual;_LLA8:
# 539
({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp11F=_cycalloc_atomic(sizeof(*_tmp11F));_tmp11F->kind=Cyc_Absyn_MemKind;_tmp11F->aliasqual=_tmp126;_tmp11F;})));({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0;_tmp11D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*n).f2);({void*_tmp11A[3]={& _tmp11C,& _tmp11D,& _tmp11E};Cyc_Tcutil_terr(loc,({const char*_tmp11B="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp11B,sizeof(char),57);}),_tag_dyneither(_tmp11A,sizeof(void*),3));});});});});
continue;}else{_LLA9: _LLAA:
 continue;}}_LL9E:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 547
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp127=ad->name;
# 554
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp128=(void*)atts->hd;void*_tmp129=_tmp128;switch(*((int*)_tmp129)){case 7: _LLAC: _LLAD:
 goto _LLAF;case 6: _LLAE: _LLAF:
 continue;default: _LLB0: _LLB1:
# 559
({struct Cyc_String_pa_PrintArg_struct _tmp12D;_tmp12D.tag=0;_tmp12D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp127).f2);({struct Cyc_String_pa_PrintArg_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp12A[2]={& _tmp12C,& _tmp12D};Cyc_Tcutil_terr(loc,({const char*_tmp12B="bad attribute %s in %s definition";_tag_dyneither(_tmp12B,sizeof(char),34);}),_tag_dyneither(_tmp12A,sizeof(void*),2));});});});
goto _LLAB;}_LLAB:;}}{
# 564
struct Cyc_List_List*_tmp12E=ad->tvs;
# 567
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 572
struct _tuple15 _tmp12F=({struct _tuple15 _tmp14D;_tmp14D.f1=ad->impl;_tmp14D.f2=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,_tmp127);_tmp14D;});struct _tuple15 _tmp130=_tmp12F;struct Cyc_List_List*_tmp14C;struct Cyc_List_List*_tmp14B;struct Cyc_List_List*_tmp14A;int _tmp149;struct Cyc_Absyn_Aggrdecl**_tmp148;struct Cyc_List_List*_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_List_List*_tmp145;int _tmp144;struct Cyc_Absyn_Aggrdecl**_tmp143;if(_tmp130.f1 == 0){if(_tmp130.f2 == 0){_LLB3: _LLB4:
# 575
 Cyc_Tc_rule_out_memkind(loc,_tmp127,_tmp12E,0);
# 577
(te->ae)->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp127,({struct Cyc_Absyn_Aggrdecl**_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131[0]=ad;_tmp131;}));
goto _LLB2;}else{_LLB9: _tmp143=*_tmp130.f2;_LLBA: {
# 637
struct Cyc_Absyn_Aggrdecl*_tmp142=Cyc_Tcdecl_merge_aggrdecl(*_tmp143,ad,loc,Cyc_Tc_tc_msg);
if(_tmp142 == 0)
return;else{
# 641
Cyc_Tc_rule_out_memkind(loc,_tmp127,_tmp12E,0);
# 644
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp127,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 647
*_tmp143=_tmp142;
ad=_tmp142;
goto _LLB2;}}}}else{if(_tmp130.f2 == 0){_LLB5: _tmp147=(_tmp130.f1)->exist_vars;_tmp146=(_tmp130.f1)->rgn_po;_tmp145=(_tmp130.f1)->fields;_tmp144=(_tmp130.f1)->tagged;_LLB6: {
# 582
struct Cyc_Absyn_Aggrdecl**_tmp132=({struct Cyc_Absyn_Aggrdecl**_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A[0]=({struct Cyc_Absyn_Aggrdecl*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->kind=ad->kind;_tmp13B->sc=Cyc_Absyn_Extern;_tmp13B->name=ad->name;_tmp13B->tvs=_tmp12E;_tmp13B->impl=0;_tmp13B->attributes=ad->attributes;_tmp13B->expected_mem_kind=0;_tmp13B;});_tmp13A;});
# 584
(te->ae)->aggrdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp127,_tmp132);
# 589
Cyc_Tcutil_check_unique_tvars(loc,_tmp147);
Cyc_Tcutil_add_tvar_identities(_tmp147);
# 593
if(_tmp144  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp133=0;Cyc_Tcutil_terr(loc,({const char*_tmp134="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp134,sizeof(char),56);}),_tag_dyneither(_tmp133,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp12E,_tmp147),_tmp146,_tmp145);
# 597
Cyc_Tc_rule_out_memkind(loc,_tmp127,_tmp12E,0);
# 600
Cyc_Tc_rule_out_memkind(loc,_tmp127,_tmp147,1);
# 602
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp144){
# 605
struct Cyc_List_List*f=_tmp145;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp139;_tmp139.tag=0;_tmp139.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));({struct Cyc_String_pa_PrintArg_struct _tmp138;_tmp138.tag=0;_tmp138.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp127).f2);({struct Cyc_String_pa_PrintArg_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);({void*_tmp135[3]={& _tmp137,& _tmp138,& _tmp139};Cyc_Tcutil_warn(loc,({const char*_tmp136="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp136,sizeof(char),98);}),_tag_dyneither(_tmp135,sizeof(void*),3));});});});});}}
# 611
*_tmp132=ad;
goto _LLB2;}}else{_LLB7: _tmp14C=(_tmp130.f1)->exist_vars;_tmp14B=(_tmp130.f1)->rgn_po;_tmp14A=(_tmp130.f1)->fields;_tmp149=(_tmp130.f1)->tagged;_tmp148=*_tmp130.f2;_LLB8:
# 615
 if(ad->kind != (*_tmp148)->kind)
({void*_tmp13C=0;Cyc_Tcutil_terr(loc,({const char*_tmp13D="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp13D,sizeof(char),52);}),_tag_dyneither(_tmp13C,sizeof(void*),0));});{
# 618
struct Cyc_Absyn_Aggrdecl*_tmp13E=*_tmp148;
# 620
*_tmp148=({struct Cyc_Absyn_Aggrdecl*_tmp13F=_cycalloc(sizeof(*_tmp13F));_tmp13F->kind=ad->kind;_tmp13F->sc=Cyc_Absyn_Extern;_tmp13F->name=ad->name;_tmp13F->tvs=_tmp12E;_tmp13F->impl=0;_tmp13F->attributes=ad->attributes;_tmp13F->expected_mem_kind=0;_tmp13F;});
# 626
Cyc_Tcutil_check_unique_tvars(loc,_tmp14C);
Cyc_Tcutil_add_tvar_identities(_tmp14C);
# 629
if(_tmp149  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp140=0;Cyc_Tcutil_terr(loc,({const char*_tmp141="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp141,sizeof(char),56);}),_tag_dyneither(_tmp140,sizeof(void*),0));});
Cyc_Tc_tcAggrImpl(te,loc,ad->kind,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp12E,_tmp14C),_tmp14B,_tmp14A);
# 633
*_tmp148=_tmp13E;
_tmp143=_tmp148;goto _LLBA;};}}_LLB2:;};};}
# 654
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp14E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp14F=_tmp14E;enum Cyc_Absyn_AliasQual _tmp16C;enum Cyc_Absyn_KindQual _tmp16B;struct Cyc_Core_Opt**_tmp16A;enum Cyc_Absyn_KindQual _tmp169;struct Cyc_Core_Opt**_tmp168;struct Cyc_Core_Opt**_tmp167;struct Cyc_Core_Opt**_tmp166;struct Cyc_Core_Opt**_tmp165;struct Cyc_Core_Opt**_tmp164;struct Cyc_Core_Opt**_tmp163;switch(*((int*)_tmp14F)){case 1: _LLBC: _tmp163=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp14F)->f1;_LLBD:
 _tmp164=_tmp163;goto _LLBF;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f2)->aliasqual){case Cyc_Absyn_Top: _LLBE: _tmp164=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f1;_LLBF:
# 659
 _tmp165=_tmp164;goto _LLC1;case Cyc_Absyn_Aliasable: _LLC0: _tmp165=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f1;_LLC1:
# 661
*_tmp165=({struct Cyc_Core_Opt*_tmp150=_cycalloc(sizeof(*_tmp150));_tmp150->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp150;});goto _LLBB;default: goto _LLCA;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f2)->aliasqual){case Cyc_Absyn_Top: _LLC2: _tmp166=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f1;_LLC3:
 _tmp167=_tmp166;goto _LLC5;case Cyc_Absyn_Aliasable: _LLC4: _tmp167=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f1;_LLC5:
# 664
*_tmp167=({struct Cyc_Core_Opt*_tmp151=_cycalloc(sizeof(*_tmp151));_tmp151->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp151;});goto _LLBB;default: goto _LLCA;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f2)->aliasqual){case Cyc_Absyn_Top: _LLC6: _tmp168=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f1;_LLC7:
# 666
*_tmp168=({struct Cyc_Core_Opt*_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152->v=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp152;});goto _LLBB;case Cyc_Absyn_Unique: goto _LLCA;default: goto _LLD0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f2)->aliasqual == Cyc_Absyn_Unique){_LLCA: _tmp16A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f1;_tmp169=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14F)->f2)->kind;_LLCB:
# 670
 _tmp16B=_tmp169;goto _LLCD;}else{goto _LLD0;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14F)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14F)->f1)->aliasqual){case Cyc_Absyn_Top: _LLC8: _LLC9:
# 668
({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0;_tmp156.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp153[2]={& _tmp155,& _tmp156};Cyc_Tcutil_terr(loc,({const char*_tmp154="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp154,sizeof(char),57);}),_tag_dyneither(_tmp153,sizeof(void*),2));});});});
goto _LLBB;case Cyc_Absyn_Unique: goto _LLCC;default: goto _LLD0;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14F)->f1)->aliasqual == Cyc_Absyn_Unique){_LLCC: _tmp16B=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14F)->f1)->kind;_LLCD:
# 672
({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0;_tmp15B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 674
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp15C=_cycalloc_atomic(sizeof(*_tmp15C));_tmp15C->kind=_tmp16B;_tmp15C->aliasqual=Cyc_Absyn_Unique;_tmp15C;})));({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0;_tmp15A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp157[3]={& _tmp159,& _tmp15A,& _tmp15B};Cyc_Tcutil_terr(loc,({const char*_tmp158="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp158,sizeof(char),57);}),_tag_dyneither(_tmp157,sizeof(void*),3));});});});});goto _LLBB;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14F)->f1)->kind == Cyc_Absyn_MemKind){_LLCE: _tmp16C=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14F)->f1)->aliasqual;_LLCF:
# 676
({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0;_tmp161.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 678
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp162=_cycalloc_atomic(sizeof(*_tmp162));_tmp162->kind=Cyc_Absyn_MemKind;_tmp162->aliasqual=_tmp16C;_tmp162;})));({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0;_tmp160.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp15D[3]={& _tmp15F,& _tmp160,& _tmp161};Cyc_Tcutil_terr(loc,({const char*_tmp15E="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp15E,sizeof(char),57);}),_tag_dyneither(_tmp15D,sizeof(void*),3));});});});});goto _LLBB;}else{_LLD0: _LLD1:
 goto _LLBB;}}}}_LLBB:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 684
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 692
{struct Cyc_List_List*_tmp16D=fields;for(0;_tmp16D != 0;_tmp16D=_tmp16D->tl){
struct Cyc_Absyn_Datatypefield*_tmp16E=(struct Cyc_Absyn_Datatypefield*)_tmp16D->hd;
struct Cyc_List_List*typs=_tmp16E->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp16E->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);{
# 697
struct _RegionHandle*_tmp16F=Cyc_Core_heap_region;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp16F,(*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0;_tmp172.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 701
Cyc_Absynpp_qvar2string(_tmp16E->name));({void*_tmp170[1]={& _tmp172};Cyc_Tcutil_terr(_tmp16E->loc,({const char*_tmp171="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp171,sizeof(char),51);}),_tag_dyneither(_tmp170,sizeof(void*),1));});});
# 703
((*((struct _tuple16*)typs->hd)).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(_tmp16E->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);};}}}{
# 709
struct Cyc_List_List*fields2;
if(is_extensible){
# 712
int _tmp173=1;
struct Cyc_List_List*_tmp174=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp173,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp173)
fields2=_tmp174;else{
# 717
fields2=0;}}else{
# 719
struct _RegionHandle _tmp175=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp175;_push_region(uprev_rgn);
# 721
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp176=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp176->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp17A;_tmp17A.tag=0;_tmp17A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);({struct Cyc_String_pa_PrintArg_struct _tmp179;_tmp179.tag=0;_tmp179.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp176->name).f2);({void*_tmp177[2]={& _tmp179,& _tmp17A};Cyc_Tcutil_terr(_tmp176->loc,({const char*_tmp178="duplicate field name %s in %s";_tag_dyneither(_tmp178,sizeof(char),30);}),_tag_dyneither(_tmp177,sizeof(void*),2));});});});else{
# 727
prev_fields=({struct Cyc_List_List*_tmp17B=_region_malloc(uprev_rgn,sizeof(*_tmp17B));_tmp17B->hd=(*_tmp176->name).f2;_tmp17B->tl=prev_fields;_tmp17B;});}
# 729
if(_tmp176->sc != Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp17E;_tmp17E.tag=0;_tmp17E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp176->name).f2);({void*_tmp17C[1]={& _tmp17E};Cyc_Tcutil_warn(loc,({const char*_tmp17D="ignoring scope of field %s";_tag_dyneither(_tmp17D,sizeof(char),27);}),_tag_dyneither(_tmp17C,sizeof(void*),1));});});
_tmp176->sc=Cyc_Absyn_Public;}}}
# 734
fields2=fields;}
# 721
;_pop_region(uprev_rgn);}
# 736
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 739
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp197="@extensible datatype";_tag_dyneither(_tmp197,sizeof(char),21);}):({const char*_tmp198="datatype";_tag_dyneither(_tmp198,sizeof(char),9);});
# 746
struct Cyc_List_List*tvs=tud->tvs;
# 749
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 755
struct _RegionHandle _tmp17F=_new_region("temp");struct _RegionHandle*temp=& _tmp17F;_push_region(temp);
{struct Cyc_Absyn_Datatypedecl***tud_opt;
# 766 "tc.cyc"
{struct _handler_cons _tmp180;_push_handler(& _tmp180);{int _tmp182=0;if(setjmp(_tmp180.handler))_tmp182=1;if(!_tmp182){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(temp,te,loc,tud->name);
if(tud_opt == 0  && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp183=_cycalloc_atomic(sizeof(*_tmp183));_tmp183[0]=({struct Cyc_Dict_Absent_exn_struct _tmp184;_tmp184.tag=Cyc_Dict_Absent;_tmp184;});_tmp183;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 772
(*tud->name).f1=Cyc_Absyn_Abs_n(te->ns,0);}
# 767
;_pop_handler();}else{void*_tmp181=(void*)_exn_thrown;void*_tmp185=_tmp181;void*_tmp188;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp185)->tag == Cyc_Dict_Absent){_LLD3: _LLD4: {
# 776
struct Cyc_Absyn_Datatypedecl***_tmp186=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);
tud_opt=(unsigned int)_tmp186?({struct Cyc_Absyn_Datatypedecl***_tmp187=_region_malloc(temp,sizeof(*_tmp187));_tmp187[0]=*_tmp186;_tmp187;}): 0;
goto _LLD2;}}else{_LLD5: _tmp188=_tmp185;_LLD6:(int)_rethrow(_tmp188);}_LLD2:;}};}{
# 783
struct _tuple17 _tmp189=({struct _tuple17 _tmp196;_tmp196.f1=tud->fields;_tmp196.f2=tud_opt;_tmp196;});struct _tuple17 _tmp18A=_tmp189;struct Cyc_List_List**_tmp195;struct Cyc_Absyn_Datatypedecl**_tmp194;struct Cyc_List_List**_tmp193;struct Cyc_Absyn_Datatypedecl**_tmp192;if(_tmp18A.f1 == 0){if(_tmp18A.f2 == 0){_LLD8: _LLD9:
# 786
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
(te->ae)->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,({struct Cyc_Absyn_Datatypedecl**_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B[0]=tud;_tmp18B;}));
goto _LLD7;}else{_LLDE: _tmp192=*_tmp18A.f2;_LLDF: {
# 819
struct Cyc_Absyn_Datatypedecl*_tmp191=Cyc_Tcdecl_merge_datatypedecl(*_tmp192,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(_tmp191 == 0){
_npop_handler(0);return;}else{
# 824
*_tmp192=_tmp191;
goto _LLD7;}}}}else{if(_tmp18A.f2 == 0){_LLDA: _tmp193=(struct Cyc_List_List**)&(_tmp18A.f1)->v;_LLDB: {
# 791
struct Cyc_Absyn_Datatypedecl**_tmp18C=({struct Cyc_Absyn_Datatypedecl**_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D[0]=({struct Cyc_Absyn_Datatypedecl*_tmp18E=_cycalloc(sizeof(*_tmp18E));_tmp18E->sc=Cyc_Absyn_Extern;_tmp18E->name=tud->name;_tmp18E->tvs=tvs;_tmp18E->fields=0;_tmp18E->is_extensible=tud->is_extensible;_tmp18E;});_tmp18D;});
# 793
(te->ae)->datatypedecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,_tmp18C);
# 796
*_tmp193=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp193,tvs,tud);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*_tmp18C=tud;
goto _LLD7;}}else{_LLDC: _tmp195=(struct Cyc_List_List**)&(_tmp18A.f1)->v;_tmp194=*_tmp18A.f2;_LLDD: {
# 801
struct Cyc_Absyn_Datatypedecl*_tmp18F=*_tmp194;
# 804
if((!tud->is_extensible  && (unsigned int)_tmp18F) && _tmp18F->is_extensible)
tud->is_extensible=1;
# 807
*_tmp194=({struct Cyc_Absyn_Datatypedecl*_tmp190=_cycalloc(sizeof(*_tmp190));_tmp190->sc=Cyc_Absyn_Extern;_tmp190->name=tud->name;_tmp190->tvs=tvs;_tmp190->fields=0;_tmp190->is_extensible=tud->is_extensible;_tmp190;});
# 811
*_tmp195=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp195,tvs,tud);
# 815
*_tmp194=_tmp18F;
_tmp192=_tmp194;goto _LLDF;}}}_LLD7:;};}
# 756 "tc.cyc"
;_pop_region(temp);};}
# 831 "tc.cyc"
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 838
if(ed->fields != 0){struct _RegionHandle _tmp199=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp199;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp19A=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 844
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp19A->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp19D;_tmp19D.tag=0;_tmp19D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp19A->name).f2);({void*_tmp19B[1]={& _tmp19D};Cyc_Tcutil_terr(_tmp19A->loc,({const char*_tmp19C="duplicate enum constructor %s";_tag_dyneither(_tmp19C,sizeof(char),30);}),_tag_dyneither(_tmp19B,sizeof(void*),1));});});else{
# 847
prev_fields=({struct Cyc_List_List*_tmp19E=_region_malloc(uprev_rgn,sizeof(*_tmp19E));_tmp19E->hd=(*_tmp19A->name).f2;_tmp19E->tl=prev_fields;_tmp19E;});}
# 849
if(((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,_tmp19A->name)!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1A1;_tmp1A1.tag=0;_tmp1A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp19A->name).f2);({void*_tmp19F[1]={& _tmp1A1};Cyc_Tcutil_terr(_tmp19A->loc,({const char*_tmp1A0="enum field name %s shadows global name";_tag_dyneither(_tmp1A0,sizeof(char),39);}),_tag_dyneither(_tmp19F,sizeof(void*),1));});});
# 852
if(_tmp19A->tag == 0){
_tmp19A->tag=Cyc_Absyn_uint_exp(tag_count,_tmp19A->loc);
++ tag_count;}else{
# 857
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp19A->tag))){
struct _tuple11 _tmp1A2=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp19A->tag));struct _tuple11 _tmp1A3=_tmp1A2;unsigned int _tmp1A5;int _tmp1A4;_LLE1: _tmp1A5=_tmp1A3.f1;_tmp1A4=_tmp1A3.f2;_LLE2:;
if(_tmp1A4)tag_count=_tmp1A5 + 1;}}}}
# 839
;_pop_region(uprev_rgn);}
# 865
{struct _handler_cons _tmp1A6;_push_handler(& _tmp1A6);{int _tmp1A8=0;if(setjmp(_tmp1A6.handler))_tmp1A8=1;if(!_tmp1A8){
{struct Cyc_Absyn_Enumdecl**_tmp1A9=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);
struct Cyc_Absyn_Enumdecl*_tmp1AA=Cyc_Tcdecl_merge_enumdecl(*_tmp1A9,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1AA == 0){_npop_handler(0);return;}
*_tmp1A9=_tmp1AA;}
# 866
;_pop_handler();}else{void*_tmp1A7=(void*)_exn_thrown;void*_tmp1AB=_tmp1A7;void*_tmp1AE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1AB)->tag == Cyc_Dict_Absent){_LLE4: _LLE5: {
# 871
struct Cyc_Absyn_Enumdecl**_tmp1AC=({struct Cyc_Absyn_Enumdecl**_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD[0]=ed;_tmp1AD;});
(te->ae)->enumdecls=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,_tmp1AC);
goto _LLE3;}}else{_LLE6: _tmp1AE=_tmp1AB;_LLE7:(int)_rethrow(_tmp1AE);}_LLE3:;}};}
# 877
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1AF=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1AF->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1AF->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1AF->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1B2;_tmp1B2.tag=0;_tmp1B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp1B0[2]={& _tmp1B2,& _tmp1B3};Cyc_Tcutil_terr(loc,({const char*_tmp1B1="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1B1,sizeof(char),46);}),_tag_dyneither(_tmp1B0,sizeof(void*),2));});});});}}}
# 887
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1B4=sc;switch(_tmp1B4){case Cyc_Absyn_Static: _LLE9: _LLEA:
# 890
 if(!in_include)
({void*_tmp1B5=0;Cyc_Tcutil_warn(loc,({const char*_tmp1B6="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1B6,sizeof(char),44);}),_tag_dyneither(_tmp1B5,sizeof(void*),0));});
return 0;case Cyc_Absyn_Abstract: _LLEB: _LLEC:
# 894
({void*_tmp1B7=0;Cyc_Tcutil_warn(loc,({const char*_tmp1B8="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1B8,sizeof(char),46);}),_tag_dyneither(_tmp1B7,sizeof(void*),0));});
return 0;case Cyc_Absyn_Public: _LLED: _LLEE:
 goto _LLF0;case Cyc_Absyn_Register: _LLEF: _LLF0:
 goto _LLF2;case Cyc_Absyn_Extern: _LLF1: _LLF2:
 return 1;default: _LLF3: _LLF4:
# 900
({void*_tmp1B9=0;Cyc_Tcutil_warn(loc,({const char*_tmp1BA="nested extern \"C\" declaration";_tag_dyneither(_tmp1BA,sizeof(char),30);}),_tag_dyneither(_tmp1B9,sizeof(void*),0));});
return 1;}_LLE8:;}
# 910
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct _RegionHandle*grgn,struct Cyc_List_List**exports){
# 916
struct Cyc_List_List*last=0;
# 918
struct Cyc_List_List*_tmp1BB=ds0;for(0;_tmp1BB != 0;(last=_tmp1BB,_tmp1BB=_tmp1BB->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1BB->hd;
unsigned int loc=d->loc;
# 922
void*_tmp1BC=d->r;void*_tmp1BD=_tmp1BC;struct Cyc_List_List*_tmp1D9;struct Cyc_List_List*_tmp1D8;struct Cyc_List_List*_tmp1D7;union Cyc_Absyn_Nmspace _tmp1D6;struct _dyneither_ptr*_tmp1D5;struct Cyc_List_List*_tmp1D4;struct _dyneither_ptr*_tmp1D3;struct Cyc_List_List*_tmp1D2;struct Cyc_Absyn_Enumdecl*_tmp1D1;struct Cyc_Absyn_Datatypedecl*_tmp1D0;struct Cyc_Absyn_Aggrdecl*_tmp1CF;struct Cyc_Absyn_Typedefdecl*_tmp1CE;struct Cyc_Absyn_Fndecl*_tmp1CD;struct Cyc_Absyn_Vardecl*_tmp1CC;switch(*((int*)_tmp1BD)){case 2: _LLF6: _LLF7:
 goto _LLF9;case 3: _LLF8: _LLF9:
# 925
({void*_tmp1BE=0;Cyc_Tcutil_terr(loc,({const char*_tmp1BF="top level let-declarations are not implemented";_tag_dyneither(_tmp1BF,sizeof(char),47);}),_tag_dyneither(_tmp1BE,sizeof(void*),0));});
goto _LLF5;case 4: _LLFA: _LLFB:
# 928
({void*_tmp1C0=0;Cyc_Tcutil_terr(loc,({const char*_tmp1C1="top level region declarations are not implemented";_tag_dyneither(_tmp1C1,sizeof(char),50);}),_tag_dyneither(_tmp1C0,sizeof(void*),0));});
goto _LLF5;case 0: _LLFC: _tmp1CC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_LLFD:
# 931
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1CC->sc,te->in_extern_c_include))
_tmp1CC->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,_tmp1CC,check_var_init,te->in_extern_c_include,exports);
goto _LLF5;case 1: _LLFE: _tmp1CD=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_LLFF:
# 936
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1CD->sc,te->in_extern_c_include))
_tmp1CD->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,_tmp1CD,exports);
goto _LLF5;case 8: _LL100: _tmp1CE=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_LL101:
# 941
 _tmp1CE->extern_c=te->in_extern_c_include;
# 945
Cyc_Tc_tcTypedefdecl(te,loc,_tmp1CE);
goto _LLF5;case 5: _LL102: _tmp1CF=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_LL103:
# 948
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1CF->sc,te->in_extern_c_include))
_tmp1CF->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp1CF);
goto _LLF5;case 6: _LL104: _tmp1D0=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_LL105:
# 953
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1D0->sc,te->in_extern_c_include))
_tmp1D0->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,_tmp1D0);
goto _LLF5;case 7: _LL106: _tmp1D1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_LL107:
# 958
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1D1->sc,te->in_extern_c_include))
_tmp1D1->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp1D1);
goto _LLF5;case 13: _LL108: _LL109:
# 963
({void*_tmp1C2=0;Cyc_Tcutil_warn(d->loc,({const char*_tmp1C3="spurious __cyclone_port_on__";_tag_dyneither(_tmp1C3,sizeof(char),29);}),_tag_dyneither(_tmp1C2,sizeof(void*),0));});
goto _LLF5;case 14: _LL10A: _LL10B:
# 966
 goto _LLF5;case 9: _LL10C: _tmp1D3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_tmp1D2=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BD)->f2;_LL10D: {
# 969
struct Cyc_List_List*_tmp1C4=te->ns;
te->ns=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1C4,({struct Cyc_List_List*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));_tmp1C5->hd=_tmp1D3;_tmp1C5->tl=0;_tmp1C5;}));
Cyc_Tc_tc_decls(te,_tmp1D2,in_externC,check_var_init,grgn,exports);
te->ns=_tmp1C4;
goto _LLF5;}case 10: _LL10E: _tmp1D6=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1)->f1;_tmp1D5=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1)->f2;_tmp1D4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BD)->f2;_LL10F:
# 977
 Cyc_Tc_tc_decls(te,_tmp1D4,in_externC,check_var_init,grgn,exports);
goto _LLF5;case 11: _LL110: _tmp1D7=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_LL111:
# 981
 Cyc_Tc_tc_decls(te,_tmp1D7,1,check_var_init,grgn,exports);
goto _LLF5;default: _LL112: _tmp1D9=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BD)->f1;_tmp1D8=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BD)->f2;_LL113: {
# 987
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1D8,(unsigned int)exports?*exports: 0);
{struct _RegionHandle*_tmp1C6=Cyc_Core_heap_region;
struct Cyc_Tcenv_Tenv*_tmp1C7=Cyc_Tcenv_enter_extern_c_include(te);
Cyc_Tc_tc_decls(_tmp1C7,_tmp1D9,1,check_var_init,grgn,& newexs);
# 993
for(0;_tmp1D8 != 0;_tmp1D8=_tmp1D8->tl){
struct _tuple12*_tmp1C8=(struct _tuple12*)_tmp1D8->hd;
if(!(*_tmp1C8).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1CB;_tmp1CB.tag=0;_tmp1CB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1C8).f2));({void*_tmp1C9[1]={& _tmp1CB};Cyc_Tcutil_warn((*_tmp1C8).f1,({const char*_tmp1CA="%s is exported but not defined";_tag_dyneither(_tmp1CA,sizeof(char),31);}),_tag_dyneither(_tmp1C9,sizeof(void*),1));});});}}
# 1000
goto _LLF5;}}_LLF5:;}}
# 1005
void Cyc_Tc_tc(struct _RegionHandle*g,struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
# 1007
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,g,0);}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1024 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1026
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1028
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1032
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1DA=d->r;void*_tmp1DB=_tmp1DA;struct Cyc_List_List**_tmp1E0;struct Cyc_List_List**_tmp1DF;struct Cyc_List_List**_tmp1DE;struct Cyc_List_List**_tmp1DD;struct Cyc_Absyn_Vardecl*_tmp1DC;switch(*((int*)_tmp1DB)){case 0: _LL115: _tmp1DC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1DB)->f1;_LL116:
# 1037
 if((env->in_cinclude  || _tmp1DC->sc != Cyc_Absyn_Extern  && _tmp1DC->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp1DC->name,Cyc_Absyn_uniquergn_qvar))
# 1040
return 1;
# 1042
return(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,_tmp1DC->name)).f2;case 11: _LL117: _tmp1DD=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1DB)->f1;_LL118:
 _tmp1DE=_tmp1DD;goto _LL11A;case 10: _LL119: _tmp1DE=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1DB)->f2;_LL11A:
 _tmp1DF=_tmp1DE;goto _LL11C;case 9: _LL11B: _tmp1DF=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1DB)->f2;_LL11C:
# 1046
*_tmp1DF=Cyc_Tc_treeshake_f(env,*_tmp1DF);
return 1;case 12: _LL11D: _tmp1E0=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1DB)->f1;_LL11E: {
# 1049
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
*_tmp1E0=Cyc_Tc_treeshake_f(env,*_tmp1E0);
env->in_cinclude=in_cinclude;
return 1;}default: _LL11F: _LL120:
 return 1;}_LL114:;}
# 1058
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1E1=({struct Cyc_Tc_TreeshakeEnv _tmp1E2;_tmp1E2.in_cinclude=0;_tmp1E2.ordinaries=(te->ae)->ordinaries;_tmp1E2;});
return Cyc_Tc_treeshake_f(& _tmp1E1,ds);}
