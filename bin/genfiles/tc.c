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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 915 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 923
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 932
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 935
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 943
void*Cyc_Absyn_compress_kb(void*);
# 948
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 972
void*Cyc_Absyn_string_typ(void*rgn);
void*Cyc_Absyn_const_string_typ(void*rgn);
# 994
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1005
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1077
extern struct _tuple0*Cyc_Absyn_uniquergn_qvar;
# 1165
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 67
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned int,struct Cyc_Absyn_Fndecl*);
# 77
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 149
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
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
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
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
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
if(({Cyc_Absyn_qvar_cmp(x,(*_tmp1).f2)== 0;})){
# 49
(*_tmp1).f3=1;
return 1;}}
# 53
return 0;}
# 56
static int Cyc_Tc_fnTypeAttsRangeOK(unsigned int loc,int i,int nargs,void*att){
if(i < 1  || i > nargs){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;({struct _dyneither_ptr _tmp1DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string(att));_tmp4.f1=_tmp1DF;});({void*_tmp2[1]={& _tmp4};({unsigned int _tmp1E1=loc;struct _dyneither_ptr _tmp1E0=({const char*_tmp3="%s has an out-of-range index";_tag_dyneither(_tmp3,sizeof(char),29);});Cyc_Tcutil_terr(_tmp1E1,_tmp1E0,_tag_dyneither(_tmp2,sizeof(void*),1));});});});
return 0;}
# 61
return 1;}struct _tuple13{struct Cyc_List_List*f1;struct _dyneither_ptr f2;};
# 63
static void Cyc_Tc_fnTypeAttsOverlap(unsigned int loc,int i,struct _tuple13 lst1,struct _tuple13 lst2){
# 66
if(((int(*)(struct Cyc_List_List*l,int x))Cyc_List_memq)(lst2.f1,i))
({struct Cyc_Int_pa_PrintArg_struct _tmp9;_tmp9.tag=1;_tmp9.f1=(unsigned long)i;({struct Cyc_String_pa_PrintArg_struct _tmp8;_tmp8.tag=0;_tmp8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst2.f2);({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)lst1.f2);({void*_tmp5[3]={& _tmp7,& _tmp8,& _tmp9};({unsigned int _tmp1E3=loc;struct _dyneither_ptr _tmp1E2=({const char*_tmp6="incompatible %s() and %s() attributes on parameter %d";_tag_dyneither(_tmp6,sizeof(char),54);});Cyc_Tcutil_terr(_tmp1E3,_tmp1E2,_tag_dyneither(_tmp5,sizeof(void*),3));});});});});});}
# 70
static void Cyc_Tc_fnTypeAttsOK(struct Cyc_Tcenv_Tenv*te,void*t,unsigned int loc){
struct _tuple13 init_params=({struct _tuple13 _tmp3A;_tmp3A.f1=0;({struct _dyneither_ptr _tmp1E4=({const char*_tmp3B="initializes";_tag_dyneither(_tmp3B,sizeof(char),12);});_tmp3A.f2=_tmp1E4;});_tmp3A;});
struct _tuple13 nolive_unique_params=({struct _tuple13 _tmp38;_tmp38.f1=0;({struct _dyneither_ptr _tmp1E5=({const char*_tmp39="noliveunique";_tag_dyneither(_tmp39,sizeof(char),13);});_tmp38.f2=_tmp1E5;});_tmp38;});
struct _tuple13 noconsume_params=({struct _tuple13 _tmp36;_tmp36.f1=0;({struct _dyneither_ptr _tmp1E6=({const char*_tmp37="noconsume";_tag_dyneither(_tmp37,sizeof(char),10);});_tmp36.f2=_tmp1E6;});_tmp36;});
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;struct Cyc_List_List*_tmp35;struct Cyc_List_List*_tmp34;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->tag == 9){_LL1: _tmp35=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).attributes;_tmp34=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB)->f1).args;_LL2: {
# 76
int nargs=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp34);
for(0;_tmp35 != 0;_tmp35=_tmp35->tl){
void*_tmpC=(void*)_tmp35->hd;void*_tmpD=_tmpC;int _tmp31;int _tmp30;int _tmp2F;switch(*((int*)_tmpD)){case 20: _LL6: _tmp2F=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmpD)->f1;_LL7:
# 80
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp2F,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp2F,init_params,nolive_unique_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp2F,init_params,noconsume_params);{
struct _tuple8*_tmpE=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp2F - 1);struct _tuple8*_tmpF=_tmpE;void*_tmp24;_LLF: _tmp24=_tmpF->f3;_LL10:;{
struct _dyneither_ptr s=({const char*_tmp23="initializes attribute allowed only on";_tag_dyneither(_tmp23,sizeof(char),38);});
{void*_tmp10=Cyc_Tcutil_compress(_tmp24);void*_tmp11=_tmp10;void*_tmp21;union Cyc_Absyn_Constraint*_tmp20;union Cyc_Absyn_Constraint*_tmp1F;union Cyc_Absyn_Constraint*_tmp1E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->tag == 5){_LL12: _tmp21=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).elt_typ;_tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).nullable;_tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).bounds;_tmp1E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11)->f1).ptr_atts).zero_term;_LL13:
# 87
 if(((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp20))
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp12[1]={& _tmp14};({unsigned int _tmp1E8=loc;struct _dyneither_ptr _tmp1E7=({const char*_tmp13="%s non-null pointers";_tag_dyneither(_tmp13,sizeof(char),21);});Cyc_Tcutil_terr(_tmp1E8,_tmp1E7,_tag_dyneither(_tmp12,sizeof(void*),1));});});});
if(!Cyc_Tcutil_is_bound_one(_tmp1F))
({struct Cyc_String_pa_PrintArg_struct _tmp17;_tmp17.tag=0;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp15[1]={& _tmp17};({unsigned int _tmp1EA=loc;struct _dyneither_ptr _tmp1E9=({const char*_tmp16="%s pointers of size 1";_tag_dyneither(_tmp16,sizeof(char),22);});Cyc_Tcutil_terr(_tmp1EA,_tmp1E9,_tag_dyneither(_tmp15,sizeof(void*),1));});});});
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp1E))
({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp18[1]={& _tmp1A};({unsigned int _tmp1EC=loc;struct _dyneither_ptr _tmp1EB=({const char*_tmp19="%s pointers to non-zero-terminated arrays";_tag_dyneither(_tmp19,sizeof(char),42);});Cyc_Tcutil_terr(_tmp1EC,_tmp1EB,_tag_dyneither(_tmp18,sizeof(void*),1));});});});
goto _LL11;}else{_LL14: _LL15:
# 95
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s);({void*_tmp1B[1]={& _tmp1D};({unsigned int _tmp1EE=loc;struct _dyneither_ptr _tmp1ED=({const char*_tmp1C="%s pointers";_tag_dyneither(_tmp1C,sizeof(char),12);});Cyc_Tcutil_terr(_tmp1EE,_tmp1ED,_tag_dyneither(_tmp1B,sizeof(void*),1));});});});}_LL11:;}
# 97
({struct Cyc_List_List*_tmp1EF=({struct Cyc_List_List*_tmp22=_cycalloc(sizeof(*_tmp22));_tmp22->hd=(void*)_tmp2F;_tmp22->tl=init_params.f1;_tmp22;});init_params.f1=_tmp1EF;});
goto _LL5;};};case 21: _LL8: _tmp30=((struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct*)_tmpD)->f1;_LL9:
# 100
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp30,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,nolive_unique_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp30,nolive_unique_params,noconsume_params);{
struct _tuple8*_tmp25=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp30 - 1);struct _tuple8*_tmp26=_tmp25;void*_tmp2A;_LL17: _tmp2A=_tmp26->f3;_LL18:;
if(!Cyc_Tcutil_is_noalias_pointer(_tmp2A,0))
({void*_tmp27=0;({unsigned int _tmp1F1=loc;struct _dyneither_ptr _tmp1F0=({const char*_tmp28="noliveunique attribute allowed only on unique pointers";_tag_dyneither(_tmp28,sizeof(char),55);});Cyc_Tcutil_terr(_tmp1F1,_tmp1F0,_tag_dyneither(_tmp27,sizeof(void*),0));});});
({struct Cyc_List_List*_tmp1F2=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->hd=(void*)_tmp30;_tmp29->tl=nolive_unique_params.f1;_tmp29;});nolive_unique_params.f1=_tmp1F2;});
goto _LL5;};case 22: _LLA: _tmp31=((struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct*)_tmpD)->f1;_LLB:
# 109
 if(!Cyc_Tc_fnTypeAttsRangeOK(loc,_tmp31,nargs,(void*)_tmp35->hd))goto _LL5;
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,noconsume_params,init_params);
Cyc_Tc_fnTypeAttsOverlap(loc,_tmp31,noconsume_params,nolive_unique_params);{
struct _tuple8*_tmp2B=((struct _tuple8*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp34,_tmp31 - 1);struct _tuple8*_tmp2C=_tmp2B;void*_tmp2E;_LL1A: _tmp2E=_tmp2C->f3;_LL1B:;
# 114
({struct Cyc_List_List*_tmp1F3=({struct Cyc_List_List*_tmp2D=_cycalloc(sizeof(*_tmp2D));_tmp2D->hd=(void*)_tmp31;_tmp2D->tl=noconsume_params.f1;_tmp2D;});noconsume_params.f1=_tmp1F3;});
goto _LL5;};default: _LLC: _LLD:
 goto _LL5;}_LL5:;}
# 119
goto _LL0;}}else{_LL3: _LL4:
({void*_tmp32=0;({struct _dyneither_ptr _tmp1F4=({const char*_tmp33="fnTypeAttsOK: not a function type";_tag_dyneither(_tmp33,sizeof(char),34);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp1F4,_tag_dyneither(_tmp32,sizeof(void*),0));});});}_LL0:;}struct _tuple14{void*f1;int f2;};
# 124
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 126
struct Cyc_Absyn_Vardecl*_tmp3C=vd;enum Cyc_Absyn_Scope _tmp96;struct _tuple0*_tmp95;void*_tmp94;struct Cyc_Absyn_Exp*_tmp93;struct Cyc_List_List*_tmp92;_LL1D: _tmp96=_tmp3C->sc;_tmp95=_tmp3C->name;_tmp94=_tmp3C->type;_tmp93=_tmp3C->initializer;_tmp92=_tmp3C->attributes;_LL1E:;
# 133
{void*_tmp3D=Cyc_Tcutil_compress(_tmp94);void*_tmp3E=_tmp3D;void*_tmp4A;struct Cyc_Absyn_Tqual _tmp49;union Cyc_Absyn_Constraint*_tmp48;unsigned int _tmp47;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->tag == 8){if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).num_elts == 0){_LL20: _tmp4A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).elt_type;_tmp49=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).tq;_tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zero_term;_tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zt_loc;if(_tmp93 != 0){_LL21:
# 135
{void*_tmp3F=_tmp93->r;void*_tmp40=_tmp3F;struct Cyc_List_List*_tmp46;struct Cyc_List_List*_tmp45;struct Cyc_Absyn_Exp*_tmp44;struct Cyc_Absyn_Exp*_tmp43;struct _dyneither_ptr _tmp42;struct _dyneither_ptr _tmp41;switch(*((int*)_tmp40)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40)->f1).Wstring_c).tag){case 8: _LL25: _tmp41=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40)->f1).String_c).val;_LL26:
# 137
({void*_tmp1FA=({void*_tmp1F9=({void*_tmp1F8=_tmp4A;struct Cyc_Absyn_Tqual _tmp1F7=_tmp49;struct Cyc_Absyn_Exp*_tmp1F6=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp41,sizeof(char)),0);union Cyc_Absyn_Constraint*_tmp1F5=_tmp48;Cyc_Absyn_array_typ(_tmp1F8,_tmp1F7,_tmp1F6,_tmp1F5,_tmp47);});vd->type=_tmp1F9;});_tmp94=_tmp1FA;});
goto _LL24;case 9: _LL27: _tmp42=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp40)->f1).Wstring_c).val;_LL28:
# 140
({void*_tmp200=({void*_tmp1FF=({void*_tmp1FE=_tmp4A;struct Cyc_Absyn_Tqual _tmp1FD=_tmp49;struct Cyc_Absyn_Exp*_tmp1FC=Cyc_Absyn_uint_exp(1,0);union Cyc_Absyn_Constraint*_tmp1FB=_tmp48;Cyc_Absyn_array_typ(_tmp1FE,_tmp1FD,_tmp1FC,_tmp1FB,_tmp47);});vd->type=_tmp1FF;});_tmp94=_tmp200;});
goto _LL24;default: goto _LL31;}case 26: _LL29: _tmp43=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp40)->f2;_LL2A:
 _tmp44=_tmp43;goto _LL2C;case 27: _LL2B: _tmp44=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp40)->f1;_LL2C:
# 145
({void*_tmp202=({void*_tmp201=Cyc_Absyn_array_typ(_tmp4A,_tmp49,_tmp44,_tmp48,_tmp47);vd->type=_tmp201;});_tmp94=_tmp202;});
goto _LL24;case 35: _LL2D: _tmp45=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp40)->f2;_LL2E:
 _tmp46=_tmp45;goto _LL30;case 25: _LL2F: _tmp46=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp40)->f1;_LL30:
# 149
({void*_tmp208=({void*_tmp207=({void*_tmp206=_tmp4A;struct Cyc_Absyn_Tqual _tmp205=_tmp49;struct Cyc_Absyn_Exp*_tmp204=({Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp46),0);});union Cyc_Absyn_Constraint*_tmp203=_tmp48;Cyc_Absyn_array_typ(_tmp206,_tmp205,_tmp204,_tmp203,_tmp47);});vd->type=_tmp207;});_tmp94=_tmp208;});
goto _LL24;default: _LL31: _LL32:
 goto _LL24;}_LL24:;}
# 153
goto _LL1F;}else{goto _LL22;}}else{goto _LL22;}}else{_LL22: _LL23:
 goto _LL1F;}_LL1F:;}
# 157
Cyc_Tcutil_check_valid_toplevel_type(loc,te,_tmp94);
# 159
({int _tmp209=Cyc_Tcutil_extract_const_from_typedef(loc,(vd->tq).print_const,_tmp94);(vd->tq).real_const=_tmp209;});
# 162
({int _tmp20A=!Cyc_Tcutil_is_array(_tmp94);vd->escapes=_tmp20A;});
# 164
if(Cyc_Tcutil_is_function_type(_tmp94)){
({struct Cyc_List_List*_tmp20B=Cyc_Tcutil_transfer_fn_type_atts(_tmp94,_tmp92);_tmp92=_tmp20B;});
Cyc_Tc_fnTypeAttsOK(te,_tmp94,loc);}
# 169
if(_tmp96 == Cyc_Absyn_Extern  || _tmp96 == Cyc_Absyn_ExternC){
if(_tmp93 != 0  && !in_cinclude)
({void*_tmp4B=0;({unsigned int _tmp20D=loc;struct _dyneither_ptr _tmp20C=({const char*_tmp4C="extern declaration should not have initializer";_tag_dyneither(_tmp4C,sizeof(char),47);});Cyc_Tcutil_terr(_tmp20D,_tmp20C,_tag_dyneither(_tmp4B,sizeof(void*),0));});});}else{
if(!Cyc_Tcutil_is_function_type(_tmp94)){
# 176
for(0;_tmp92 != 0;_tmp92=_tmp92->tl){
void*_tmp4D=(void*)_tmp92->hd;void*_tmp4E=_tmp4D;switch(*((int*)_tmp4E)){case 6: _LL34: _LL35:
 goto _LL37;case 8: _LL36: _LL37:
# 181
 goto _LL39;case 9: _LL38: _LL39:
 goto _LL3B;case 10: _LL3A: _LL3B:
 goto _LL3D;case 11: _LL3C: _LL3D:
 goto _LL3F;case 12: _LL3E: _LL3F:
 goto _LL41;case 13: _LL40: _LL41:
 goto _LL43;case 14: _LL42: _LL43:
 continue;default: _LL44: _LL45:
# 189
({struct Cyc_String_pa_PrintArg_struct _tmp52;_tmp52.tag=0;({struct _dyneither_ptr _tmp20E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(vd->name));_tmp52.f1=_tmp20E;});({struct Cyc_String_pa_PrintArg_struct _tmp51;_tmp51.tag=0;({struct _dyneither_ptr _tmp20F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp92->hd));_tmp51.f1=_tmp20F;});({void*_tmp4F[2]={& _tmp51,& _tmp52};({unsigned int _tmp211=loc;struct _dyneither_ptr _tmp210=({const char*_tmp50="bad attribute %s for variable %s";_tag_dyneither(_tmp50,sizeof(char),33);});Cyc_Tcutil_terr(_tmp211,_tmp210,_tag_dyneither(_tmp4F,sizeof(void*),2));});});});});
goto _LL33;}_LL33:;}
# 194
if(_tmp93 == 0  || in_cinclude){
if((check_var_init  && !in_cinclude) && !Cyc_Tcutil_supports_default(_tmp94))
({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0;({struct _dyneither_ptr _tmp212=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp94));_tmp56.f1=_tmp212;});({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0;({struct _dyneither_ptr _tmp213=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp55.f1=_tmp213;});({void*_tmp53[2]={& _tmp55,& _tmp56};({unsigned int _tmp215=loc;struct _dyneither_ptr _tmp214=({const char*_tmp54="initializer required for variable %s of type %s";_tag_dyneither(_tmp54,sizeof(char),48);});Cyc_Tcutil_terr(_tmp215,_tmp214,_tag_dyneither(_tmp53,sizeof(void*),2));});});});});}else{
# 199
struct Cyc_Absyn_Exp*_tmp57=_tmp93;
# 203
struct _handler_cons _tmp58;_push_handler(& _tmp58);{int _tmp5A=0;if(setjmp(_tmp58.handler))_tmp5A=1;if(!_tmp5A){
{void*_tmp5B=Cyc_Tcexp_tcExpInitializer(te,& _tmp94,_tmp57);
if(!Cyc_Tcutil_coerce_assign(te,_tmp57,_tmp94)){
struct _dyneither_ptr _tmp5C=Cyc_Absynpp_qvar2string(vd->name);
const char*_tmp5D=" declared with type ";
struct _dyneither_ptr _tmp5E=Cyc_Absynpp_typ2string(_tmp94);
const char*_tmp5F=" but initialized with type ";
struct _dyneither_ptr _tmp60=Cyc_Absynpp_typ2string(_tmp5B);
if(({({unsigned long _tmp219=({unsigned long _tmp218=({unsigned long _tmp217=({unsigned long _tmp216=Cyc_strlen((struct _dyneither_ptr)_tmp5C);_tmp216 + Cyc_strlen(({const char*_tmp61=_tmp5D;_tag_dyneither(_tmp61,sizeof(char),_get_zero_arr_size_char((void*)_tmp61,21));}));});_tmp217 + Cyc_strlen((struct _dyneither_ptr)_tmp5E);});_tmp218 + Cyc_strlen(({const char*_tmp62=_tmp5F;_tag_dyneither(_tmp62,sizeof(char),_get_zero_arr_size_char((void*)_tmp62,28));}));});_tmp219 + Cyc_strlen((struct _dyneither_ptr)_tmp60);})> 70;}))
({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp60);({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0;({struct _dyneither_ptr _tmp21A=(struct _dyneither_ptr)({const char*_tmp6B=_tmp5F;_tag_dyneither(_tmp6B,sizeof(char),_get_zero_arr_size_char((void*)_tmp6B,28));});_tmp68.f1=_tmp21A;});({struct Cyc_String_pa_PrintArg_struct _tmp67;_tmp67.tag=0;_tmp67.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5E);({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0;({struct _dyneither_ptr _tmp21B=(struct _dyneither_ptr)({const char*_tmp6A=_tmp5D;_tag_dyneither(_tmp6A,sizeof(char),_get_zero_arr_size_char((void*)_tmp6A,21));});_tmp66.f1=_tmp21B;});({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0;_tmp65.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5C);({void*_tmp63[5]={& _tmp65,& _tmp66,& _tmp67,& _tmp68,& _tmp69};({unsigned int _tmp21D=loc;struct _dyneither_ptr _tmp21C=({const char*_tmp64="%s%s\n\t%s\n%s\n\t%s";_tag_dyneither(_tmp64,sizeof(char),16);});Cyc_Tcutil_terr(_tmp21D,_tmp21C,_tag_dyneither(_tmp63,sizeof(void*),5));});});});});});});});else{
# 214
({struct Cyc_String_pa_PrintArg_struct _tmp72;_tmp72.tag=0;_tmp72.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp60);({struct Cyc_String_pa_PrintArg_struct _tmp71;_tmp71.tag=0;({struct _dyneither_ptr _tmp21E=(struct _dyneither_ptr)({const char*_tmp74=_tmp5F;_tag_dyneither(_tmp74,sizeof(char),_get_zero_arr_size_char((void*)_tmp74,28));});_tmp71.f1=_tmp21E;});({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0;_tmp70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5E);({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0;({struct _dyneither_ptr _tmp21F=(struct _dyneither_ptr)({const char*_tmp73=_tmp5D;_tag_dyneither(_tmp73,sizeof(char),_get_zero_arr_size_char((void*)_tmp73,21));});_tmp6F.f1=_tmp21F;});({struct Cyc_String_pa_PrintArg_struct _tmp6E;_tmp6E.tag=0;_tmp6E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp5C);({void*_tmp6C[5]={& _tmp6E,& _tmp6F,& _tmp70,& _tmp71,& _tmp72};({unsigned int _tmp221=loc;struct _dyneither_ptr _tmp220=({const char*_tmp6D="%s%s%s%s%s";_tag_dyneither(_tmp6D,sizeof(char),11);});Cyc_Tcutil_terr(_tmp221,_tmp220,_tag_dyneither(_tmp6C,sizeof(void*),5));});});});});});});});}
Cyc_Tcutil_explain_failure();
# 217
if(!Cyc_Tcutil_is_const_exp(_tmp57))
({void*_tmp75=0;({unsigned int _tmp223=loc;struct _dyneither_ptr _tmp222=({const char*_tmp76="initializer is not a constant expression";_tag_dyneither(_tmp76,sizeof(char),41);});Cyc_Tcutil_terr(_tmp223,_tmp222,_tag_dyneither(_tmp75,sizeof(void*),0));});});}}
# 204
;_pop_handler();}else{void*_tmp59=(void*)_exn_thrown;void*_tmp77=_tmp59;void*_tmp7A;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_tmp77)->tag == Cyc_Tcenv_Env_error){_LL47: _LL48:
# 222
({void*_tmp78=0;({unsigned int _tmp225=loc;struct _dyneither_ptr _tmp224=({const char*_tmp79="initializer is not a constant expression";_tag_dyneither(_tmp79,sizeof(char),41);});Cyc_Tcutil_terr(_tmp225,_tmp224,_tag_dyneither(_tmp78,sizeof(void*),0));});});
goto _LL46;}else{_LL49: _tmp7A=_tmp77;_LL4A:(int)_rethrow(_tmp7A);}_LL46:;}};}}else{
# 228
for(0;_tmp92 != 0;_tmp92=_tmp92->tl){
void*_tmp7B=(void*)_tmp92->hd;void*_tmp7C=_tmp7B;switch(*((int*)_tmp7C)){case 0: _LL4C: _LL4D:
# 231
 goto _LL4F;case 1: _LL4E: _LL4F:
 goto _LL51;case 2: _LL50: _LL51:
 goto _LL53;case 3: _LL52: _LL53:
 goto _LL55;case 4: _LL54: _LL55:
 goto _LL57;case 19: _LL56: _LL57:
 goto _LL59;case 20: _LL58: _LL59:
 goto _LL5B;case 23: _LL5A: _LL5B:
 goto _LL5D;case 26: _LL5C: _LL5D:
 goto _LL5F;case 5: _LL5E: _LL5F:
({void*_tmp7D=0;({struct _dyneither_ptr _tmp226=({const char*_tmp7E="tcVardecl: fn type atts in function var decl";_tag_dyneither(_tmp7E,sizeof(char),45);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp226,_tag_dyneither(_tmp7D,sizeof(void*),0));});});case 6: _LL60: _LL61:
# 242
 goto _LL63;case 7: _LL62: _LL63:
# 244
({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0;({struct _dyneither_ptr _tmp227=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)_tmp92->hd));_tmp81.f1=_tmp227;});({void*_tmp7F[1]={& _tmp81};({unsigned int _tmp229=loc;struct _dyneither_ptr _tmp228=({const char*_tmp80="bad attribute %s in function declaration";_tag_dyneither(_tmp80,sizeof(char),41);});Cyc_Tcutil_terr(_tmp229,_tmp228,_tag_dyneither(_tmp7F,sizeof(void*),1));});});});
goto _LL4B;default: _LL64: _LL65:
 continue;}_LL4B:;}}}
# 252
{struct _handler_cons _tmp82;_push_handler(& _tmp82);{int _tmp84=0;if(setjmp(_tmp82.handler))_tmp84=1;if(!_tmp84){
{struct _tuple14*_tmp85=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,_tmp95);
void*_tmp86=(*_tmp85).f1;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp87=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp22A=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp8C;_tmp8C.tag=1;_tmp8C.f1=vd;_tmp8C;});_tmp8B[0]=_tmp22A;});_tmp8B;});
void**_tmp88=Cyc_Tcdecl_merge_binding(_tmp86,(void*)_tmp87,loc,Cyc_Tc_tc_msg);
if(_tmp88 == 0){_npop_handler(0);return;}{
void*_tmp89=*_tmp88;
# 260
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports)){
if(_tmp89 != _tmp86  || (*_tmp85).f2)
# 263
({struct Cyc_Dict_Dict _tmp22D=({struct Cyc_Dict_Dict _tmp22C=(te->ae)->ordinaries;struct _tuple0*_tmp22B=_tmp95;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp22C,_tmp22B,({struct _tuple14*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=_tmp89;_tmp8A->f2=1;_tmp8A;}));});(te->ae)->ordinaries=_tmp22D;});}
_npop_handler(0);return;};}
# 253
;_pop_handler();}else{void*_tmp83=(void*)_exn_thrown;void*_tmp8D=_tmp83;void*_tmp8E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8D)->tag == Cyc_Dict_Absent){_LL67: _LL68:
# 265
 goto _LL66;}else{_LL69: _tmp8E=_tmp8D;_LL6A:(int)_rethrow(_tmp8E);}_LL66:;}};}
if(exports == 0  || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _tmp232=({struct Cyc_Dict_Dict _tmp231=(te->ae)->ordinaries;struct _tuple0*_tmp230=_tmp95;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp231,_tmp230,({struct _tuple14*_tmp8F=_cycalloc(sizeof(*_tmp8F));({void*_tmp22F=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp90=_cycalloc(sizeof(*_tmp90));({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp22E=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp91;_tmp91.tag=1;_tmp91.f1=vd;_tmp91;});_tmp90[0]=_tmp22E;});_tmp90;});_tmp8F->f1=_tmp22F;});_tmp8F->f2=0;_tmp8F;}));});(te->ae)->ordinaries=_tmp232;});}
# 271
static int Cyc_Tc_is_main(struct _tuple0*n){
struct _tuple0*_tmp97=n;union Cyc_Absyn_Nmspace _tmp9B;struct _dyneither_ptr*_tmp9A;_LL6C: _tmp9B=_tmp97->f1;_tmp9A=_tmp97->f2;_LL6D:;{
union Cyc_Absyn_Nmspace _tmp98=_tmp9B;if((_tmp98.Abs_n).tag == 2){if((_tmp98.Abs_n).val == 0){_LL6F: _LL70:
# 275
 return({({struct _dyneither_ptr _tmp233=(struct _dyneither_ptr)*_tmp9A;Cyc_strcmp(_tmp233,({const char*_tmp99="main";_tag_dyneither(_tmp99,sizeof(char),5);}));})== 0;});}else{goto _LL71;}}else{_LL71: _LL72:
 return 0;}_LL6E:;};}
# 280
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 282
struct _tuple0*q=fd->name;
# 286
if(fd->sc == Cyc_Absyn_ExternC  && !te->in_extern_c_include)
({void*_tmp9C=0;({unsigned int _tmp235=loc;struct _dyneither_ptr _tmp234=({const char*_tmp9D="extern \"C\" functions cannot be implemented in Cyclone";_tag_dyneither(_tmp9D,sizeof(char),54);});Cyc_Tcutil_terr(_tmp235,_tmp234,_tag_dyneither(_tmp9C,sizeof(void*),0));});});
# 290
Cyc_Tcutil_check_fndecl_valid_type(loc,te,fd);{
# 292
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 294
({struct Cyc_List_List*_tmp236=Cyc_Tcutil_transfer_fn_type_atts(t,fd->attributes);fd->attributes=_tmp236;});
Cyc_Tc_fnTypeAttsOK(te,t,loc);
# 297
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp9E=(void*)atts->hd;void*_tmp9F=_tmp9E;switch(*((int*)_tmp9F)){case 7: _LL74: _LL75:
 goto _LL77;case 6: _LL76: _LL77:
# 301
({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;({struct _dyneither_ptr _tmp237=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmpA2.f1=_tmp237;});({void*_tmpA0[1]={& _tmpA2};({unsigned int _tmp239=loc;struct _dyneither_ptr _tmp238=({const char*_tmpA1="bad attribute %s for function";_tag_dyneither(_tmpA1,sizeof(char),30);});Cyc_Tcutil_terr(_tmp239,_tmp238,_tag_dyneither(_tmpA0,sizeof(void*),1));});});});
goto _LL73;default: _LL78: _LL79:
 goto _LL73;}_LL73:;}}
# 307
{struct _handler_cons _tmpA3;_push_handler(& _tmpA3);{int _tmpA5=0;if(setjmp(_tmpA3.handler))_tmpA5=1;if(!_tmpA5){
{struct _tuple14*_tmpA6=((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
void*_tmpA7=(*_tmpA6).f1;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpA8=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp23A=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpAD;_tmpAD.tag=2;_tmpAD.f1=fd;_tmpAD;});_tmpAC[0]=_tmp23A;});_tmpAC;});
void**_tmpA9=Cyc_Tcdecl_merge_binding(_tmpA7,(void*)_tmpA8,loc,Cyc_Tc_tc_msg);
if(_tmpA9 != 0){
void*_tmpAA=*_tmpA9;
# 315
if(exports == 0  || Cyc_Tc_export_member(q,*exports)){
if(!(_tmpAA == _tmpA7  && (*_tmpA6).f2))
({struct Cyc_Dict_Dict _tmp23D=({struct Cyc_Dict_Dict _tmp23C=(te->ae)->ordinaries;struct _tuple0*_tmp23B=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp23C,_tmp23B,({struct _tuple14*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->f1=_tmpAA;_tmpAB->f2=1;_tmpAB;}));});(te->ae)->ordinaries=_tmp23D;});}}}
# 308
;_pop_handler();}else{void*_tmpA4=(void*)_exn_thrown;void*_tmpAE=_tmpA4;void*_tmpB5;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpAE)->tag == Cyc_Dict_Absent){_LL7B: _LL7C:
# 321
 if(exports == 0  || Cyc_Tc_export_member(fd->name,*exports)){
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpAF=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp23E=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpB4;_tmpB4.tag=2;_tmpB4.f1=fd;_tmpB4;});_tmpB3[0]=_tmp23E;});_tmpB3;});
({struct Cyc_Dict_Dict _tmp243=({struct Cyc_Dict_Dict _tmp242=(te->ae)->ordinaries;struct _tuple0*_tmp241=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct _tuple14*v))Cyc_Dict_insert)(_tmp242,_tmp241,({struct _tuple14*_tmpB0=_cycalloc(sizeof(*_tmpB0));({void*_tmp240=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmp23F=({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct _tmpB2;_tmpB2.tag=2;_tmpB2.f1=fd;_tmpB2;});_tmpB1[0]=_tmp23F;});_tmpB1;});_tmpB0->f1=_tmp240;});_tmpB0->f2=0;_tmpB0;}));});(te->ae)->ordinaries=_tmp243;});}
# 326
goto _LL7A;}else{_LL7D: _tmpB5=_tmpAE;_LL7E:(int)_rethrow(_tmpB5);}_LL7A:;}};}
# 330
if(te->in_extern_c_include)return;{
# 335
struct Cyc_Tcenv_Fenv*_tmpB6=Cyc_Tcenv_new_fenv(loc,fd);
struct Cyc_Tcenv_Tenv*_tmpB7=({struct Cyc_Tcenv_Tenv*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->ns=te->ns;_tmpDD->ae=te->ae;_tmpDD->le=_tmpB6;_tmpDD->allow_valueof=0;_tmpDD->in_extern_c_include=0;_tmpDD;});
# 338
Cyc_Tcstmt_tcStmt(_tmpB7,fd->body,0);
# 341
Cyc_Tcenv_check_delayed_effects(_tmpB7);
Cyc_Tcenv_check_delayed_constraints(_tmpB7);
# 344
if(Cyc_Tc_is_main(q)){
# 346
{void*_tmpB8=Cyc_Tcutil_compress(fd->ret_type);void*_tmpB9=_tmpB8;enum Cyc_Absyn_Size_of _tmpBF;switch(*((int*)_tmpB9)){case 0: _LL80: _LL81:
({void*_tmpBA=0;({unsigned int _tmp245=loc;struct _dyneither_ptr _tmp244=({const char*_tmpBB="main declared with return type void";_tag_dyneither(_tmpBB,sizeof(char),36);});Cyc_Tcutil_warn(_tmp245,_tmp244,_tag_dyneither(_tmpBA,sizeof(void*),0));});});goto _LL7F;case 6: _LL82: _tmpBF=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpB9)->f2;_LL83:
 goto _LL7F;default: _LL84: _LL85:
# 350
({struct Cyc_String_pa_PrintArg_struct _tmpBE;_tmpBE.tag=0;({struct _dyneither_ptr _tmp246=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(fd->ret_type));_tmpBE.f1=_tmp246;});({void*_tmpBC[1]={& _tmpBE};({unsigned int _tmp248=loc;struct _dyneither_ptr _tmp247=({const char*_tmpBD="main declared with return type %s instead of int or void";_tag_dyneither(_tmpBD,sizeof(char),57);});Cyc_Tcutil_terr(_tmp248,_tmp247,_tag_dyneither(_tmpBC,sizeof(void*),1));});});});
goto _LL7F;}_LL7F:;}
# 354
if(fd->c_varargs  || fd->cyc_varargs != 0)
({void*_tmpC0=0;({unsigned int _tmp24A=loc;struct _dyneither_ptr _tmp249=({const char*_tmpC1="main declared with varargs";_tag_dyneither(_tmpC1,sizeof(char),27);});Cyc_Tcutil_terr(_tmp24A,_tmp249,_tag_dyneither(_tmpC0,sizeof(void*),0));});});{
struct Cyc_List_List*_tmpC2=fd->args;
if(_tmpC2 != 0){
struct _tuple8*_tmpC3=(struct _tuple8*)_tmpC2->hd;struct _tuple8*_tmpC4=_tmpC3;void*_tmpDC;_LL87: _tmpDC=_tmpC4->f3;_LL88:;
{void*_tmpC5=Cyc_Tcutil_compress(_tmpDC);void*_tmpC6=_tmpC5;enum Cyc_Absyn_Size_of _tmpCA;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC6)->tag == 6){_LL8A: _tmpCA=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmpC6)->f2;_LL8B:
 goto _LL89;}else{_LL8C: _LL8D:
# 362
({struct Cyc_String_pa_PrintArg_struct _tmpC9;_tmpC9.tag=0;({struct _dyneither_ptr _tmp24B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpDC));_tmpC9.f1=_tmp24B;});({void*_tmpC7[1]={& _tmpC9};({unsigned int _tmp24D=loc;struct _dyneither_ptr _tmp24C=({const char*_tmpC8="main declared with first argument of type %s instead of int";_tag_dyneither(_tmpC8,sizeof(char),60);});Cyc_Tcutil_terr(_tmp24D,_tmp24C,_tag_dyneither(_tmpC7,sizeof(void*),1));});});});
goto _LL89;}_LL89:;}
# 366
_tmpC2=_tmpC2->tl;
if(_tmpC2 != 0){
struct _tuple8*_tmpCB=(struct _tuple8*)_tmpC2->hd;struct _tuple8*_tmpCC=_tmpCB;void*_tmpDB;_LL8F: _tmpDB=_tmpCC->f3;_LL90:;
_tmpC2=_tmpC2->tl;
if(_tmpC2 != 0)
({void*_tmpCD=0;({unsigned int _tmp24F=loc;struct _dyneither_ptr _tmp24E=({const char*_tmpCE="main declared with too many arguments";_tag_dyneither(_tmpCE,sizeof(char),38);});Cyc_Tcutil_terr(_tmp24F,_tmp24E,_tag_dyneither(_tmpCD,sizeof(void*),0));});});{
struct Cyc_Core_Opt*tvs=({struct Cyc_Core_Opt*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA->v=fd->tvs;_tmpDA;});
if(((!({void*_tmp267=_tmpDB;Cyc_Tcutil_unify(_tmp267,({void*_tmp266=Cyc_Absyn_string_typ(({struct Cyc_Core_Opt*_tmp262=({struct Cyc_Core_Opt*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->v=& Cyc_Tcutil_rk;_tmpCF;});Cyc_Absyn_new_evar(_tmp262,tvs);}));void*_tmp265=({
struct Cyc_Core_Opt*_tmp263=({struct Cyc_Core_Opt*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->v=& Cyc_Tcutil_rk;_tmpD0;});Cyc_Absyn_new_evar(_tmp263,tvs);});
# 373
struct Cyc_Absyn_Tqual _tmp264=
# 375
Cyc_Absyn_empty_tqual(0);
# 373
Cyc_Absyn_dyneither_typ(_tmp266,_tmp265,_tmp264,
# 375
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());}));}) && !({
void*_tmp261=_tmpDB;Cyc_Tcutil_unify(_tmp261,({void*_tmp260=Cyc_Absyn_const_string_typ(({struct Cyc_Core_Opt*_tmp25C=({struct Cyc_Core_Opt*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->v=& Cyc_Tcutil_rk;_tmpD1;});Cyc_Absyn_new_evar(_tmp25C,tvs);}));void*_tmp25F=({
# 378
struct Cyc_Core_Opt*_tmp25D=({struct Cyc_Core_Opt*_tmpD2=_cycalloc(sizeof(*_tmpD2));_tmpD2->v=& Cyc_Tcutil_rk;_tmpD2;});Cyc_Absyn_new_evar(_tmp25D,tvs);});
# 376
struct Cyc_Absyn_Tqual _tmp25E=
# 379
Cyc_Absyn_empty_tqual(0);
# 376
Cyc_Absyn_dyneither_typ(_tmp260,_tmp25F,_tmp25E,
# 379
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());}));})) && !({
void*_tmp25B=_tmpDB;Cyc_Tcutil_unify(_tmp25B,({void*_tmp25A=Cyc_Absyn_string_typ(({struct Cyc_Core_Opt*_tmp256=({struct Cyc_Core_Opt*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->v=& Cyc_Tcutil_rk;_tmpD3;});Cyc_Absyn_new_evar(_tmp256,tvs);}));void*_tmp259=({
struct Cyc_Core_Opt*_tmp257=({struct Cyc_Core_Opt*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->v=& Cyc_Tcutil_rk;_tmpD4;});Cyc_Absyn_new_evar(_tmp257,tvs);});
# 380
struct Cyc_Absyn_Tqual _tmp258=
# 382
Cyc_Absyn_const_tqual(0);
# 380
Cyc_Absyn_dyneither_typ(_tmp25A,_tmp259,_tmp258,
# 382
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());}));})) && !({
void*_tmp255=_tmpDB;Cyc_Tcutil_unify(_tmp255,({void*_tmp254=Cyc_Absyn_const_string_typ(({struct Cyc_Core_Opt*_tmp250=({struct Cyc_Core_Opt*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->v=& Cyc_Tcutil_rk;_tmpD5;});Cyc_Absyn_new_evar(_tmp250,tvs);}));void*_tmp253=({
# 385
struct Cyc_Core_Opt*_tmp251=({struct Cyc_Core_Opt*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->v=& Cyc_Tcutil_rk;_tmpD6;});Cyc_Absyn_new_evar(_tmp251,tvs);});
# 383
struct Cyc_Absyn_Tqual _tmp252=
# 386
Cyc_Absyn_const_tqual(0);
# 383
Cyc_Absyn_dyneither_typ(_tmp254,_tmp253,_tmp252,
# 386
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)());}));}))
({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0;({struct _dyneither_ptr _tmp268=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpDB));_tmpD9.f1=_tmp268;});({void*_tmpD7[1]={& _tmpD9};({unsigned int _tmp26A=loc;struct _dyneither_ptr _tmp269=({const char*_tmpD8="second argument of main has type %s instead of char??";_tag_dyneither(_tmpD8,sizeof(char),54);});Cyc_Tcutil_terr(_tmp26A,_tmp269,_tag_dyneither(_tmpD7,sizeof(void*),1));});});});};}}};}};};}
# 395
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple0*q=td->name;
# 402
if(((int(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_member)((te->ae)->typedefs,q)){
({struct Cyc_String_pa_PrintArg_struct _tmpE0;_tmpE0.tag=0;_tmpE0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmpDE[1]={& _tmpE0};({unsigned int _tmp26C=loc;struct _dyneither_ptr _tmp26B=({const char*_tmpDF="redeclaration of typedef %s";_tag_dyneither(_tmpDF,sizeof(char),28);});Cyc_Tcutil_terr(_tmp26C,_tmp26B,_tag_dyneither(_tmpDE,sizeof(void*),1));});});});
return;}
# 407
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn != 0){
Cyc_Tcutil_check_type(loc,te,td->tvs,& Cyc_Tcutil_tak,0,1,(void*)_check_null(td->defn));
({int _tmp26D=
Cyc_Tcutil_extract_const_from_typedef(loc,(td->tq).print_const,(void*)_check_null(td->defn));
# 411
(td->tq).real_const=_tmp26D;});}
# 416
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs != 0;tvs=tvs->tl){
void*_tmpE1=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_tmpE2=_tmpE1;struct Cyc_Core_Opt**_tmpEA;struct Cyc_Absyn_Kind*_tmpE9;struct Cyc_Core_Opt**_tmpE8;switch(*((int*)_tmpE2)){case 1: _LL92: _tmpE8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpE2)->f1;_LL93:
# 419
 if(td->defn != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpE5;_tmpE5.tag=0;_tmpE5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs->hd)->name);({void*_tmpE3[1]={& _tmpE5};({unsigned int _tmp26F=loc;struct _dyneither_ptr _tmp26E=({const char*_tmpE4="type variable %s is not used in typedef definition";_tag_dyneither(_tmpE4,sizeof(char),51);});Cyc_Tcutil_warn(_tmp26F,_tmp26E,_tag_dyneither(_tmpE3,sizeof(void*),1));});});});
# 422
({struct Cyc_Core_Opt*_tmp271=({struct Cyc_Core_Opt*_tmpE6=_cycalloc(sizeof(*_tmpE6));({void*_tmp270=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_tbk);_tmpE6->v=_tmp270;});_tmpE6;});*_tmpE8=_tmp271;});goto _LL91;case 2: _LL94: _tmpEA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE2)->f1;_tmpE9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpE2)->f2;_LL95:
# 429
({struct Cyc_Core_Opt*_tmp273=({struct Cyc_Core_Opt*_tmpE7=_cycalloc(sizeof(*_tmpE7));({void*_tmp272=Cyc_Tcutil_kind_to_bound(_tmpE9);_tmpE7->v=_tmp272;});_tmpE7;});*_tmpEA=_tmp273;});
goto _LL91;default: _LL96: _LL97:
 continue;}_LL91:;}}
# 435
({struct Cyc_Dict_Dict _tmp274=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)((te->ae)->typedefs,q,td);(te->ae)->typedefs=_tmp274;});}
# 438
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*tvs,struct Cyc_List_List*rpo,struct Cyc_List_List*fields){
# 445
struct _RegionHandle _tmpEB=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmpEB;_push_region(uprev_rgn);
# 447
for(0;rpo != 0;rpo=rpo->tl){
struct _tuple10*_tmpEC=(struct _tuple10*)rpo->hd;struct _tuple10*_tmpED=_tmpEC;void*_tmpEF;void*_tmpEE;_LL99: _tmpEF=_tmpED->f1;_tmpEE=_tmpED->f2;_LL9A:;
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_ek,0,0,_tmpEF);
Cyc_Tcutil_check_type(loc,te,tvs,& Cyc_Tcutil_trk,0,0,_tmpEE);}{
# 453
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 456
struct Cyc_List_List*_tmpF0=fields;for(0;_tmpF0 != 0;_tmpF0=_tmpF0->tl){
struct Cyc_Absyn_Aggrfield*_tmpF1=(struct Cyc_Absyn_Aggrfield*)_tmpF0->hd;struct Cyc_Absyn_Aggrfield*_tmpF2=_tmpF1;struct _dyneither_ptr*_tmp10E;struct Cyc_Absyn_Tqual _tmp10D;void*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;_LL9C: _tmp10E=_tmpF2->name;_tmp10D=_tmpF2->tq;_tmp10C=_tmpF2->type;_tmp10B=_tmpF2->width;_tmp10A=_tmpF2->attributes;_tmp109=_tmpF2->requires_clause;_LL9D:;
# 459
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp10E))
({struct Cyc_String_pa_PrintArg_struct _tmpF5;_tmpF5.tag=0;_tmpF5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp10E);({void*_tmpF3[1]={& _tmpF5};({unsigned int _tmp276=loc;struct _dyneither_ptr _tmp275=({const char*_tmpF4="duplicate member %s";_tag_dyneither(_tmpF4,sizeof(char),20);});Cyc_Tcutil_terr(_tmp276,_tmp275,_tag_dyneither(_tmpF3,sizeof(void*),1));});});});
# 463
if(({({struct _dyneither_ptr _tmp277=(struct _dyneither_ptr)*_tmp10E;Cyc_strcmp(_tmp277,({const char*_tmpF6="";_tag_dyneither(_tmpF6,sizeof(char),1);}));})!= 0;}))
({struct Cyc_List_List*_tmp278=({struct Cyc_List_List*_tmpF7=_region_malloc(uprev_rgn,sizeof(*_tmpF7));_tmpF7->hd=_tmp10E;_tmpF7->tl=prev_fields;_tmpF7;});prev_fields=_tmp278;});{
# 466
struct Cyc_Absyn_Kind*field_kind=& Cyc_Tcutil_tmk;
# 470
if(str_or_union == Cyc_Absyn_UnionA  || 
_tmpF0->tl == 0  && str_or_union == Cyc_Absyn_StructA)
field_kind=& Cyc_Tcutil_tak;
Cyc_Tcutil_check_type(loc,te,tvs,field_kind,0,0,_tmp10C);
# 475
({int _tmp279=Cyc_Tcutil_extract_const_from_typedef(loc,(((struct Cyc_Absyn_Aggrfield*)_tmpF0->hd)->tq).print_const,_tmp10C);(((struct Cyc_Absyn_Aggrfield*)_tmpF0->hd)->tq).real_const=_tmp279;});
# 478
Cyc_Tcutil_check_bitfield(loc,te,_tmp10C,_tmp10B,_tmp10E);
# 480
if((unsigned int)_tmp109){
if(str_or_union != Cyc_Absyn_UnionA)
({void*_tmpF8=0;({unsigned int _tmp27B=loc;struct _dyneither_ptr _tmp27A=({const char*_tmpF9="@requires clauses are only allowed on union members";_tag_dyneither(_tmpF9,sizeof(char),52);});Cyc_Tcutil_terr(_tmp27B,_tmp27A,_tag_dyneither(_tmpF8,sizeof(void*),0));});});{
struct Cyc_Tcenv_Tenv*_tmpFA=Cyc_Tcenv_allow_valueof(te);
Cyc_Tcexp_tcExp(_tmpFA,0,_tmp109);
if(!Cyc_Tcutil_is_integral(_tmp109))
({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0;({struct _dyneither_ptr _tmp27C=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 488
Cyc_Absynpp_typ2string((void*)_check_null(_tmp109->topt)));_tmpFD.f1=_tmp27C;});({void*_tmpFB[1]={& _tmpFD};({unsigned int _tmp27E=_tmp109->loc;struct _dyneither_ptr _tmp27D=({const char*_tmpFC="@requires clause has type %s instead of integral type";_tag_dyneither(_tmpFC,sizeof(char),54);});Cyc_Tcutil_terr(_tmp27E,_tmp27D,_tag_dyneither(_tmpFB,sizeof(void*),1));});});});else{
# 490
({unsigned int _tmp282=_tmp109->loc;struct Cyc_Tcenv_Tenv*_tmp281=te;struct Cyc_List_List*_tmp280=tvs;Cyc_Tcutil_check_type(_tmp282,_tmp281,_tmp280,& Cyc_Tcutil_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpFE=_cycalloc(sizeof(*_tmpFE));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp27F=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpFF;_tmpFF.tag=18;_tmpFF.f1=_tmp109;_tmpFF;});_tmpFE[0]=_tmp27F;});_tmpFE;}));});{
# 492
struct Cyc_List_List*_tmp100=Cyc_Relations_exp2relns(uprev_rgn,_tmp109);
# 499
if(!Cyc_Relations_consistent_relations(_tmp100))
({void*_tmp101=0;({unsigned int _tmp284=_tmp109->loc;struct _dyneither_ptr _tmp283=({const char*_tmp102="@requires clause may be unsatisfiable";_tag_dyneither(_tmp102,sizeof(char),38);});Cyc_Tcutil_terr(_tmp284,_tmp283,_tag_dyneither(_tmp101,sizeof(void*),0));});});
# 505
{struct Cyc_List_List*_tmp103=prev_relations;for(0;_tmp103 != 0;_tmp103=_tmp103->tl){
if(Cyc_Relations_consistent_relations(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(uprev_rgn,_tmp100,(struct Cyc_List_List*)_tmp103->hd)))
({void*_tmp104=0;({unsigned int _tmp286=_tmp109->loc;struct _dyneither_ptr _tmp285=({const char*_tmp105="@requires clause may overlap with previous clauses";_tag_dyneither(_tmp105,sizeof(char),51);});Cyc_Tcutil_terr(_tmp286,_tmp285,_tag_dyneither(_tmp104,sizeof(void*),0));});});}}
# 510
({struct Cyc_List_List*_tmp287=({struct Cyc_List_List*_tmp106=_region_malloc(uprev_rgn,sizeof(*_tmp106));_tmp106->hd=_tmp100;_tmp106->tl=prev_relations;_tmp106;});prev_relations=_tmp287;});};}};}else{
# 513
if(prev_relations != 0)
({void*_tmp107=0;({unsigned int _tmp289=loc;struct _dyneither_ptr _tmp288=({const char*_tmp108="if one field has a @requires clause, they all must";_tag_dyneither(_tmp108,sizeof(char),51);});Cyc_Tcutil_terr(_tmp289,_tmp288,_tag_dyneither(_tmp107,sizeof(void*),0));});});}};}};
# 447
;_pop_region(uprev_rgn);}
# 519
static void Cyc_Tc_rule_out_memkind(unsigned int loc,struct _tuple0*n,struct Cyc_List_List*tvs,int constrain_top_kind){
# 521
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp10F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp110=_tmp10F;enum Cyc_Absyn_AliasQual _tmp125;struct Cyc_Core_Opt**_tmp124;struct Cyc_Absyn_Kind*_tmp123;struct Cyc_Core_Opt**_tmp122;struct Cyc_Core_Opt**_tmp121;enum Cyc_Absyn_AliasQual _tmp120;struct Cyc_Core_Opt**_tmp11F;switch(*((int*)_tmp110)){case 1: _LL9F: _tmp11F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp110)->f1;_LLA0:
# 524
({struct Cyc_Core_Opt*_tmp28B=({struct Cyc_Core_Opt*_tmp111=_cycalloc(sizeof(*_tmp111));({void*_tmp28A=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp111->v=_tmp28A;});_tmp111;});*_tmp11F=_tmp28B;});continue;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110)->f2)->kind){case Cyc_Absyn_MemKind: _LLA1: _tmp121=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110)->f1;_tmp120=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110)->f2)->aliasqual;_LLA2:
# 526
 if(constrain_top_kind  && _tmp120 == Cyc_Absyn_Top)
({struct Cyc_Core_Opt*_tmp28D=({struct Cyc_Core_Opt*_tmp112=_cycalloc(sizeof(*_tmp112));({void*_tmp28C=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp113=_cycalloc_atomic(sizeof(*_tmp113));_tmp113->kind=Cyc_Absyn_BoxKind;_tmp113->aliasqual=Cyc_Absyn_Aliasable;_tmp113;}));_tmp112->v=_tmp28C;});_tmp112;});*_tmp121=_tmp28D;});else{
# 529
({struct Cyc_Core_Opt*_tmp28F=({struct Cyc_Core_Opt*_tmp114=_cycalloc(sizeof(*_tmp114));({void*_tmp28E=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp115=_cycalloc_atomic(sizeof(*_tmp115));_tmp115->kind=Cyc_Absyn_BoxKind;_tmp115->aliasqual=_tmp120;_tmp115;}));_tmp114->v=_tmp28E;});_tmp114;});*_tmp121=_tmp28F;});}
continue;case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110)->f2)->aliasqual == Cyc_Absyn_Top){_LLA3: _tmp122=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110)->f1;if(constrain_top_kind){_LLA4:
# 532
({struct Cyc_Core_Opt*_tmp291=({struct Cyc_Core_Opt*_tmp116=_cycalloc(sizeof(*_tmp116));({void*_tmp290=Cyc_Tcutil_kind_to_bound(({struct Cyc_Absyn_Kind*_tmp117=_cycalloc_atomic(sizeof(*_tmp117));_tmp117->kind=Cyc_Absyn_BoxKind;_tmp117->aliasqual=Cyc_Absyn_Aliasable;_tmp117;}));_tmp116->v=_tmp290;});_tmp116;});*_tmp122=_tmp291;});
continue;}else{goto _LLA5;}}else{goto _LLA5;}default: _LLA5: _tmp124=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110)->f1;_tmp123=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp110)->f2;_LLA6:
# 535
({struct Cyc_Core_Opt*_tmp293=({struct Cyc_Core_Opt*_tmp118=_cycalloc(sizeof(*_tmp118));({void*_tmp292=Cyc_Tcutil_kind_to_bound(_tmp123);_tmp118->v=_tmp292;});_tmp118;});*_tmp124=_tmp293;});continue;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp110)->f1)->kind == Cyc_Absyn_MemKind){_LLA7: _tmp125=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp110)->f1)->aliasqual;_LLA8:
# 537
({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0;({struct _dyneither_ptr _tmp294=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp11E=_cycalloc_atomic(sizeof(*_tmp11E));_tmp11E->kind=Cyc_Absyn_MemKind;_tmp11E->aliasqual=_tmp125;_tmp11E;})));_tmp11D.f1=_tmp294;});({struct Cyc_String_pa_PrintArg_struct _tmp11C;_tmp11C.tag=0;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp11B;_tmp11B.tag=0;_tmp11B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*n).f2);({void*_tmp119[3]={& _tmp11B,& _tmp11C,& _tmp11D};({unsigned int _tmp296=loc;struct _dyneither_ptr _tmp295=({const char*_tmp11A="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp11A,sizeof(char),57);});Cyc_Tcutil_terr(_tmp296,_tmp295,_tag_dyneither(_tmp119,sizeof(void*),3));});});});});});
continue;}else{_LLA9: _LLAA:
 continue;}}_LL9E:;}}struct _tuple15{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_Aggrdecl***f2;};
# 545
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple0*_tmp126=ad->name;
# 552
{struct Cyc_List_List*atts=ad->attributes;for(0;atts != 0;atts=atts->tl){
void*_tmp127=(void*)atts->hd;void*_tmp128=_tmp127;switch(*((int*)_tmp128)){case 7: _LLAC: _LLAD:
 goto _LLAF;case 6: _LLAE: _LLAF:
 continue;default: _LLB0: _LLB1:
# 557
({struct Cyc_String_pa_PrintArg_struct _tmp12C;_tmp12C.tag=0;_tmp12C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp126).f2);({struct Cyc_String_pa_PrintArg_struct _tmp12B;_tmp12B.tag=0;({struct _dyneither_ptr _tmp297=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));_tmp12B.f1=_tmp297;});({void*_tmp129[2]={& _tmp12B,& _tmp12C};({unsigned int _tmp299=loc;struct _dyneither_ptr _tmp298=({const char*_tmp12A="bad attribute %s in %s definition";_tag_dyneither(_tmp12A,sizeof(char),34);});Cyc_Tcutil_terr(_tmp299,_tmp298,_tag_dyneither(_tmp129,sizeof(void*),2));});});});});
goto _LLAB;}_LLAB:;}}{
# 562
struct Cyc_List_List*_tmp12D=ad->tvs;
# 565
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 570
struct _tuple15 _tmp12E=({struct _tuple15 _tmp14C;_tmp14C.f1=ad->impl;({struct Cyc_Absyn_Aggrdecl***_tmp29A=((struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->aggrdecls,_tmp126);_tmp14C.f2=_tmp29A;});_tmp14C;});struct _tuple15 _tmp12F=_tmp12E;struct Cyc_List_List*_tmp14B;struct Cyc_List_List*_tmp14A;struct Cyc_List_List*_tmp149;int _tmp148;struct Cyc_Absyn_Aggrdecl**_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_List_List*_tmp145;struct Cyc_List_List*_tmp144;int _tmp143;struct Cyc_Absyn_Aggrdecl**_tmp142;if(_tmp12F.f1 == 0){if(_tmp12F.f2 == 0){_LLB3: _LLB4:
# 573
 Cyc_Tc_rule_out_memkind(loc,_tmp126,_tmp12D,0);
# 575
({struct Cyc_Dict_Dict _tmp29D=({struct Cyc_Dict_Dict _tmp29C=(te->ae)->aggrdecls;struct _tuple0*_tmp29B=_tmp126;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)(_tmp29C,_tmp29B,({struct Cyc_Absyn_Aggrdecl**_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130[0]=ad;_tmp130;}));});(te->ae)->aggrdecls=_tmp29D;});
goto _LLB2;}else{_LLB9: _tmp142=*_tmp12F.f2;_LLBA: {
# 635
struct Cyc_Absyn_Aggrdecl*_tmp141=Cyc_Tcdecl_merge_aggrdecl(*_tmp142,ad,loc,Cyc_Tc_tc_msg);
if(_tmp141 == 0)
return;else{
# 639
Cyc_Tc_rule_out_memkind(loc,_tmp126,_tmp12D,0);
# 642
if(ad->impl != 0)
Cyc_Tc_rule_out_memkind(loc,_tmp126,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars,1);
# 645
*_tmp142=_tmp141;
ad=_tmp141;
goto _LLB2;}}}}else{if(_tmp12F.f2 == 0){_LLB5: _tmp146=(_tmp12F.f1)->exist_vars;_tmp145=(_tmp12F.f1)->rgn_po;_tmp144=(_tmp12F.f1)->fields;_tmp143=(_tmp12F.f1)->tagged;_LLB6: {
# 580
struct Cyc_Absyn_Aggrdecl**_tmp131=({struct Cyc_Absyn_Aggrdecl**_tmp139=_cycalloc(sizeof(*_tmp139));({struct Cyc_Absyn_Aggrdecl*_tmp29E=({struct Cyc_Absyn_Aggrdecl*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->kind=ad->kind;_tmp13A->sc=Cyc_Absyn_Extern;_tmp13A->name=ad->name;_tmp13A->tvs=_tmp12D;_tmp13A->impl=0;_tmp13A->attributes=ad->attributes;_tmp13A->expected_mem_kind=0;_tmp13A;});_tmp139[0]=_tmp29E;});_tmp139;});
# 582
({struct Cyc_Dict_Dict _tmp29F=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Aggrdecl**v))Cyc_Dict_insert)((te->ae)->aggrdecls,_tmp126,_tmp131);(te->ae)->aggrdecls=_tmp29F;});
# 587
Cyc_Tcutil_check_unique_tvars(loc,_tmp146);
Cyc_Tcutil_add_tvar_identities(_tmp146);
# 591
if(_tmp143  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp132=0;({unsigned int _tmp2A1=loc;struct _dyneither_ptr _tmp2A0=({const char*_tmp133="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp133,sizeof(char),56);});Cyc_Tcutil_terr(_tmp2A1,_tmp2A0,_tag_dyneither(_tmp132,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmp2A6=te;unsigned int _tmp2A5=loc;enum Cyc_Absyn_AggrKind _tmp2A4=ad->kind;struct Cyc_List_List*_tmp2A3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp12D,_tmp146);struct Cyc_List_List*_tmp2A2=_tmp145;Cyc_Tc_tcAggrImpl(_tmp2A6,_tmp2A5,_tmp2A4,_tmp2A3,_tmp2A2,_tmp144);});
# 595
Cyc_Tc_rule_out_memkind(loc,_tmp126,_tmp12D,0);
# 598
Cyc_Tc_rule_out_memkind(loc,_tmp126,_tmp146,1);
# 600
if(ad->kind == Cyc_Absyn_UnionA  && !_tmp143){
# 603
struct Cyc_List_List*f=_tmp144;for(0;f != 0;f=f->tl){
if((Cyc_Tc_aggressive_warn  && !
Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)f->hd)->type)) && ((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp138;_tmp138.tag=0;({struct _dyneither_ptr _tmp2A7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(((struct Cyc_Absyn_Aggrfield*)f->hd)->type));_tmp138.f1=_tmp2A7;});({struct Cyc_String_pa_PrintArg_struct _tmp137;_tmp137.tag=0;_tmp137.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp126).f2);({struct Cyc_String_pa_PrintArg_struct _tmp136;_tmp136.tag=0;_tmp136.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)f->hd)->name);({void*_tmp134[3]={& _tmp136,& _tmp137,& _tmp138};({unsigned int _tmp2A9=loc;struct _dyneither_ptr _tmp2A8=({const char*_tmp135="member %s of union %s has type %s which is not `bits-only' so it can only be written and not read";_tag_dyneither(_tmp135,sizeof(char),98);});Cyc_Tcutil_warn(_tmp2A9,_tmp2A8,_tag_dyneither(_tmp134,sizeof(void*),3));});});});});});}}
# 609
*_tmp131=ad;
goto _LLB2;}}else{_LLB7: _tmp14B=(_tmp12F.f1)->exist_vars;_tmp14A=(_tmp12F.f1)->rgn_po;_tmp149=(_tmp12F.f1)->fields;_tmp148=(_tmp12F.f1)->tagged;_tmp147=*_tmp12F.f2;_LLB8:
# 613
 if(ad->kind != (*_tmp147)->kind)
({void*_tmp13B=0;({unsigned int _tmp2AB=loc;struct _dyneither_ptr _tmp2AA=({const char*_tmp13C="cannot reuse struct names for unions and vice-versa";_tag_dyneither(_tmp13C,sizeof(char),52);});Cyc_Tcutil_terr(_tmp2AB,_tmp2AA,_tag_dyneither(_tmp13B,sizeof(void*),0));});});{
# 616
struct Cyc_Absyn_Aggrdecl*_tmp13D=*_tmp147;
# 618
({struct Cyc_Absyn_Aggrdecl*_tmp2AC=({struct Cyc_Absyn_Aggrdecl*_tmp13E=_cycalloc(sizeof(*_tmp13E));_tmp13E->kind=ad->kind;_tmp13E->sc=Cyc_Absyn_Extern;_tmp13E->name=ad->name;_tmp13E->tvs=_tmp12D;_tmp13E->impl=0;_tmp13E->attributes=ad->attributes;_tmp13E->expected_mem_kind=0;_tmp13E;});*_tmp147=_tmp2AC;});
# 624
Cyc_Tcutil_check_unique_tvars(loc,_tmp14B);
Cyc_Tcutil_add_tvar_identities(_tmp14B);
# 627
if(_tmp148  && ad->kind == Cyc_Absyn_StructA)
({void*_tmp13F=0;({unsigned int _tmp2AE=loc;struct _dyneither_ptr _tmp2AD=({const char*_tmp140="@tagged qualifier is only allowed on union declarations";_tag_dyneither(_tmp140,sizeof(char),56);});Cyc_Tcutil_terr(_tmp2AE,_tmp2AD,_tag_dyneither(_tmp13F,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmp2B3=te;unsigned int _tmp2B2=loc;enum Cyc_Absyn_AggrKind _tmp2B1=ad->kind;struct Cyc_List_List*_tmp2B0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp12D,_tmp14B);struct Cyc_List_List*_tmp2AF=_tmp14A;Cyc_Tc_tcAggrImpl(_tmp2B3,_tmp2B2,_tmp2B1,_tmp2B0,_tmp2AF,_tmp149);});
# 631
*_tmp147=_tmp13D;
_tmp142=_tmp147;goto _LLBA;};}}_LLB2:;};};}
# 652
static void Cyc_Tc_rule_out_mem_and_unique(unsigned int loc,struct _tuple0*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
void*_tmp14D=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_tmp14E=_tmp14D;enum Cyc_Absyn_AliasQual _tmp16B;enum Cyc_Absyn_KindQual _tmp16A;struct Cyc_Core_Opt**_tmp169;enum Cyc_Absyn_KindQual _tmp168;struct Cyc_Core_Opt**_tmp167;struct Cyc_Core_Opt**_tmp166;struct Cyc_Core_Opt**_tmp165;struct Cyc_Core_Opt**_tmp164;struct Cyc_Core_Opt**_tmp163;struct Cyc_Core_Opt**_tmp162;switch(*((int*)_tmp14E)){case 1: _LLBC: _tmp162=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp14E)->f1;_LLBD:
 _tmp163=_tmp162;goto _LLBF;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f2)->kind){case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f2)->aliasqual){case Cyc_Absyn_Top: _LLBE: _tmp163=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f1;_LLBF:
# 657
 _tmp164=_tmp163;goto _LLC1;case Cyc_Absyn_Aliasable: _LLC0: _tmp164=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f1;_LLC1:
# 659
({struct Cyc_Core_Opt*_tmp2B5=({struct Cyc_Core_Opt*_tmp14F=_cycalloc(sizeof(*_tmp14F));({void*_tmp2B4=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_bk);_tmp14F->v=_tmp2B4;});_tmp14F;});*_tmp164=_tmp2B5;});goto _LLBB;default: goto _LLCA;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f2)->aliasqual){case Cyc_Absyn_Top: _LLC2: _tmp165=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f1;_LLC3:
 _tmp166=_tmp165;goto _LLC5;case Cyc_Absyn_Aliasable: _LLC4: _tmp166=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f1;_LLC5:
# 662
({struct Cyc_Core_Opt*_tmp2B7=({struct Cyc_Core_Opt*_tmp150=_cycalloc(sizeof(*_tmp150));({void*_tmp2B6=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_ak);_tmp150->v=_tmp2B6;});_tmp150;});*_tmp166=_tmp2B7;});goto _LLBB;default: goto _LLCA;}case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f2)->aliasqual){case Cyc_Absyn_Top: _LLC6: _tmp167=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f1;_LLC7:
# 664
({struct Cyc_Core_Opt*_tmp2B9=({struct Cyc_Core_Opt*_tmp151=_cycalloc(sizeof(*_tmp151));({void*_tmp2B8=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp151->v=_tmp2B8;});_tmp151;});*_tmp167=_tmp2B9;});goto _LLBB;case Cyc_Absyn_Unique: goto _LLCA;default: goto _LLD0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f2)->aliasqual == Cyc_Absyn_Unique){_LLCA: _tmp169=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f1;_tmp168=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp14E)->f2)->kind;_LLCB:
# 668
 _tmp16A=_tmp168;goto _LLCD;}else{goto _LLD0;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14E)->f1)->kind == Cyc_Absyn_RgnKind)switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14E)->f1)->aliasqual){case Cyc_Absyn_Top: _LLC8: _LLC9:
# 666
({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0;_tmp155.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0;_tmp154.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp152[2]={& _tmp154,& _tmp155};({unsigned int _tmp2BB=loc;struct _dyneither_ptr _tmp2BA=({const char*_tmp153="type %s attempts to abstract type variable %s of kind TR";_tag_dyneither(_tmp153,sizeof(char),57);});Cyc_Tcutil_terr(_tmp2BB,_tmp2BA,_tag_dyneither(_tmp152,sizeof(void*),2));});});});});
goto _LLBB;case Cyc_Absyn_Unique: goto _LLCC;default: goto _LLD0;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14E)->f1)->aliasqual == Cyc_Absyn_Unique){_LLCC: _tmp16A=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14E)->f1)->kind;_LLCD:
# 670
({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0;({struct _dyneither_ptr _tmp2BC=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 672
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp15B=_cycalloc_atomic(sizeof(*_tmp15B));_tmp15B->kind=_tmp16A;_tmp15B->aliasqual=Cyc_Absyn_Unique;_tmp15B;})));_tmp15A.f1=_tmp2BC;});({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0;_tmp159.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0;_tmp158.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp156[3]={& _tmp158,& _tmp159,& _tmp15A};({unsigned int _tmp2BE=loc;struct _dyneither_ptr _tmp2BD=({const char*_tmp157="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp157,sizeof(char),57);});Cyc_Tcutil_terr(_tmp2BE,_tmp2BD,_tag_dyneither(_tmp156,sizeof(void*),3));});});});});});goto _LLBB;}else{if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14E)->f1)->kind == Cyc_Absyn_MemKind){_LLCE: _tmp16B=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp14E)->f1)->aliasqual;_LLCF:
# 674
({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0;({struct _dyneither_ptr _tmp2BF=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 676
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp161=_cycalloc_atomic(sizeof(*_tmp161));_tmp161->kind=Cyc_Absyn_MemKind;_tmp161->aliasqual=_tmp16B;_tmp161;})));_tmp160.f1=_tmp2BF;});({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0;_tmp15F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)tvs2->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0;_tmp15E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp15C[3]={& _tmp15E,& _tmp15F,& _tmp160};({unsigned int _tmp2C1=loc;struct _dyneither_ptr _tmp2C0=({const char*_tmp15D="type %s attempts to abstract type variable %s of kind %s";_tag_dyneither(_tmp15D,sizeof(char),57);});Cyc_Tcutil_terr(_tmp2C1,_tmp2C0,_tag_dyneither(_tmp15C,sizeof(void*),3));});});});});});goto _LLBB;}else{_LLD0: _LLD1:
 goto _LLBB;}}}}_LLBB:;}}struct _tuple16{struct Cyc_Absyn_Tqual f1;void*f2;};
# 682
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _dyneither_ptr obj,int is_extensible,struct _tuple0*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 690
{struct Cyc_List_List*_tmp16C=fields;for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
struct Cyc_Absyn_Datatypefield*_tmp16D=(struct Cyc_Absyn_Datatypefield*)_tmp16C->hd;
struct Cyc_List_List*typs=_tmp16D->typs;for(0;typs != 0;typs=typs->tl){
Cyc_Tcutil_check_type(_tmp16D->loc,te,tvs,& Cyc_Tcutil_tmk,0,0,(*((struct _tuple16*)typs->hd)).f2);
# 695
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple16*)typs->hd)).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0;({struct _dyneither_ptr _tmp2C2=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 698
Cyc_Absynpp_qvar2string(_tmp16D->name));_tmp170.f1=_tmp2C2;});({void*_tmp16E[1]={& _tmp170};({unsigned int _tmp2C4=_tmp16D->loc;struct _dyneither_ptr _tmp2C3=({const char*_tmp16F="noalias pointers in datatypes are not allowed (%s)";_tag_dyneither(_tmp16F,sizeof(char),51);});Cyc_Tcutil_terr(_tmp2C4,_tmp2C3,_tag_dyneither(_tmp16E,sizeof(void*),1));});});});
({int _tmp2C5=
Cyc_Tcutil_extract_const_from_typedef(_tmp16D->loc,((*((struct _tuple16*)typs->hd)).f1).print_const,(*((struct _tuple16*)typs->hd)).f2);
# 699
((*((struct _tuple16*)typs->hd)).f1).real_const=_tmp2C5;});}}}{
# 705
struct Cyc_List_List*fields2;
if(is_extensible){
# 708
int _tmp171=1;
struct Cyc_List_List*_tmp172=Cyc_Tcdecl_sort_xdatatype_fields(fields,& _tmp171,(*name).f2,loc,Cyc_Tc_tc_msg);
if(_tmp171)
fields2=_tmp172;else{
# 713
fields2=0;}}else{
# 715
struct _RegionHandle _tmp173=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp173;_push_region(uprev_rgn);
# 717
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp174=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp174->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp178;_tmp178.tag=0;_tmp178.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)obj);({struct Cyc_String_pa_PrintArg_struct _tmp177;_tmp177.tag=0;_tmp177.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp174->name).f2);({void*_tmp175[2]={& _tmp177,& _tmp178};({unsigned int _tmp2C7=_tmp174->loc;struct _dyneither_ptr _tmp2C6=({const char*_tmp176="duplicate field name %s in %s";_tag_dyneither(_tmp176,sizeof(char),30);});Cyc_Tcutil_terr(_tmp2C7,_tmp2C6,_tag_dyneither(_tmp175,sizeof(void*),2));});});});});else{
# 723
({struct Cyc_List_List*_tmp2C8=({struct Cyc_List_List*_tmp179=_region_malloc(uprev_rgn,sizeof(*_tmp179));_tmp179->hd=(*_tmp174->name).f2;_tmp179->tl=prev_fields;_tmp179;});prev_fields=_tmp2C8;});}
# 725
if(_tmp174->sc != Cyc_Absyn_Public){
({struct Cyc_String_pa_PrintArg_struct _tmp17C;_tmp17C.tag=0;_tmp17C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp174->name).f2);({void*_tmp17A[1]={& _tmp17C};({unsigned int _tmp2CA=loc;struct _dyneither_ptr _tmp2C9=({const char*_tmp17B="ignoring scope of field %s";_tag_dyneither(_tmp17B,sizeof(char),27);});Cyc_Tcutil_warn(_tmp2CA,_tmp2C9,_tag_dyneither(_tmp17A,sizeof(void*),1));});});});
_tmp174->sc=Cyc_Absyn_Public;}}}
# 730
fields2=fields;}
# 717
;_pop_region(uprev_rgn);}
# 732
return fields2;};}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Datatypedecl***f2;};
# 735
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple0*q=tud->name;
struct _dyneither_ptr obj=tud->is_extensible?({const char*_tmp194="@extensible datatype";_tag_dyneither(_tmp194,sizeof(char),21);}):({const char*_tmp195="datatype";_tag_dyneither(_tmp195,sizeof(char),9);});
# 742
struct Cyc_List_List*tvs=tud->tvs;
# 745
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 751
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 761 "tc.cyc"
{struct _handler_cons _tmp17D;_push_handler(& _tmp17D);{int _tmp17F=0;if(setjmp(_tmp17D.handler))_tmp17F=1;if(!_tmp17F){
({struct Cyc_Absyn_Datatypedecl***_tmp2CB=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);tud_opt=_tmp2CB;});
if(tud_opt == 0  && !tud->is_extensible)(int)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_tmp180=_cycalloc_atomic(sizeof(*_tmp180));({struct Cyc_Dict_Absent_exn_struct _tmp2CC=({struct Cyc_Dict_Absent_exn_struct _tmp181;_tmp181.tag=Cyc_Dict_Absent;_tmp181;});_tmp180[0]=_tmp2CC;});_tmp180;}));
if(tud_opt != 0)
tud->name=(*(*tud_opt))->name;else{
# 767
({union Cyc_Absyn_Nmspace _tmp2CD=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f1=_tmp2CD;});}
# 762
;_pop_handler();}else{void*_tmp17E=(void*)_exn_thrown;void*_tmp182=_tmp17E;void*_tmp185;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp182)->tag == Cyc_Dict_Absent){_LLD3: _LLD4: {
# 771
struct Cyc_Absyn_Datatypedecl***_tmp183=((struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->datatypedecls,q);
({struct Cyc_Absyn_Datatypedecl***_tmp2CE=(unsigned int)_tmp183?({struct Cyc_Absyn_Datatypedecl***_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184[0]=*_tmp183;_tmp184;}): 0;tud_opt=_tmp2CE;});
goto _LLD2;}}else{_LLD5: _tmp185=_tmp182;_LLD6:(int)_rethrow(_tmp185);}_LLD2:;}};}{
# 778
struct _tuple17 _tmp186=({struct _tuple17 _tmp193;_tmp193.f1=tud->fields;_tmp193.f2=tud_opt;_tmp193;});struct _tuple17 _tmp187=_tmp186;struct Cyc_List_List**_tmp192;struct Cyc_Absyn_Datatypedecl**_tmp191;struct Cyc_List_List**_tmp190;struct Cyc_Absyn_Datatypedecl**_tmp18F;if(_tmp187.f1 == 0){if(_tmp187.f2 == 0){_LLD8: _LLD9:
# 781
 Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
({struct Cyc_Dict_Dict _tmp2D1=({struct Cyc_Dict_Dict _tmp2D0=(te->ae)->datatypedecls;struct _tuple0*_tmp2CF=q;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp2D0,_tmp2CF,({struct Cyc_Absyn_Datatypedecl**_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188[0]=tud;_tmp188;}));});(te->ae)->datatypedecls=_tmp2D1;});
goto _LLD7;}else{_LLDE: _tmp18F=*_tmp187.f2;_LLDF: {
# 814
struct Cyc_Absyn_Datatypedecl*_tmp18E=Cyc_Tcdecl_merge_datatypedecl(*_tmp18F,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
if(_tmp18E == 0)
return;else{
# 819
*_tmp18F=_tmp18E;
goto _LLD7;}}}}else{if(_tmp187.f2 == 0){_LLDA: _tmp190=(struct Cyc_List_List**)&(_tmp187.f1)->v;_LLDB: {
# 786
struct Cyc_Absyn_Datatypedecl**_tmp189=({struct Cyc_Absyn_Datatypedecl**_tmp18A=_cycalloc(sizeof(*_tmp18A));({struct Cyc_Absyn_Datatypedecl*_tmp2D2=({struct Cyc_Absyn_Datatypedecl*_tmp18B=_cycalloc(sizeof(*_tmp18B));_tmp18B->sc=Cyc_Absyn_Extern;_tmp18B->name=tud->name;_tmp18B->tvs=tvs;_tmp18B->fields=0;_tmp18B->is_extensible=tud->is_extensible;_tmp18B;});_tmp18A[0]=_tmp2D2;});_tmp18A;});
# 788
({struct Cyc_Dict_Dict _tmp2D3=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)((te->ae)->datatypedecls,q,_tmp189);(te->ae)->datatypedecls=_tmp2D3;});
# 791
({struct Cyc_List_List*_tmp2D4=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp190,tvs,tud);*_tmp190=_tmp2D4;});
Cyc_Tc_rule_out_mem_and_unique(loc,q,tvs);
*_tmp189=tud;
goto _LLD7;}}else{_LLDC: _tmp192=(struct Cyc_List_List**)&(_tmp187.f1)->v;_tmp191=*_tmp187.f2;_LLDD: {
# 796
struct Cyc_Absyn_Datatypedecl*_tmp18C=*_tmp191;
# 799
if((!tud->is_extensible  && (unsigned int)_tmp18C) && _tmp18C->is_extensible)
tud->is_extensible=1;
# 802
({struct Cyc_Absyn_Datatypedecl*_tmp2D5=({struct Cyc_Absyn_Datatypedecl*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->sc=Cyc_Absyn_Extern;_tmp18D->name=tud->name;_tmp18D->tvs=tvs;_tmp18D->fields=0;_tmp18D->is_extensible=tud->is_extensible;_tmp18D;});*_tmp191=_tmp2D5;});
# 806
({struct Cyc_List_List*_tmp2D6=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*_tmp192,tvs,tud);*_tmp192=_tmp2D6;});
# 809
*_tmp191=_tmp18C;
_tmp18F=_tmp191;goto _LLDF;}}}_LLD7:;};};}
# 825
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple0*q=ed->name;
# 832
if(ed->fields != 0){struct _RegionHandle _tmp196=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp196;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp197=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 838
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp197->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp19A;_tmp19A.tag=0;_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp197->name).f2);({void*_tmp198[1]={& _tmp19A};({unsigned int _tmp2D8=_tmp197->loc;struct _dyneither_ptr _tmp2D7=({const char*_tmp199="duplicate enum constructor %s";_tag_dyneither(_tmp199,sizeof(char),30);});Cyc_Tcutil_terr(_tmp2D8,_tmp2D7,_tag_dyneither(_tmp198,sizeof(void*),1));});});});else{
# 841
({struct Cyc_List_List*_tmp2D9=({struct Cyc_List_List*_tmp19B=_region_malloc(uprev_rgn,sizeof(*_tmp19B));_tmp19B->hd=(*_tmp197->name).f2;_tmp19B->tl=prev_fields;_tmp19B;});prev_fields=_tmp2D9;});}
# 843
if(({((struct _tuple14**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup_opt)((te->ae)->ordinaries,_tmp197->name)!= 0;}))
({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp197->name).f2);({void*_tmp19C[1]={& _tmp19E};({unsigned int _tmp2DB=_tmp197->loc;struct _dyneither_ptr _tmp2DA=({const char*_tmp19D="enum field name %s shadows global name";_tag_dyneither(_tmp19D,sizeof(char),39);});Cyc_Tcutil_terr(_tmp2DB,_tmp2DA,_tag_dyneither(_tmp19C,sizeof(void*),1));});});});
# 846
if(_tmp197->tag == 0){
({struct Cyc_Absyn_Exp*_tmp2DC=Cyc_Absyn_uint_exp(tag_count,_tmp197->loc);_tmp197->tag=_tmp2DC;});
++ tag_count;}else{
# 851
if(Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp197->tag))){
struct _tuple11 _tmp19F=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp197->tag));struct _tuple11 _tmp1A0=_tmp19F;unsigned int _tmp1A2;int _tmp1A1;_LLE1: _tmp1A2=_tmp1A0.f1;_tmp1A1=_tmp1A0.f2;_LLE2:;
if(_tmp1A1)tag_count=_tmp1A2 + 1;}}}}
# 833
;_pop_region(uprev_rgn);}
# 859
{struct _handler_cons _tmp1A3;_push_handler(& _tmp1A3);{int _tmp1A5=0;if(setjmp(_tmp1A3.handler))_tmp1A5=1;if(!_tmp1A5){
{struct Cyc_Absyn_Enumdecl**_tmp1A6=((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);
struct Cyc_Absyn_Enumdecl*_tmp1A7=Cyc_Tcdecl_merge_enumdecl(*_tmp1A6,ed,loc,Cyc_Tc_tc_msg);
if(_tmp1A7 == 0){_npop_handler(0);return;}
*_tmp1A6=_tmp1A7;}
# 860
;_pop_handler();}else{void*_tmp1A4=(void*)_exn_thrown;void*_tmp1A8=_tmp1A4;void*_tmp1AB;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp1A8)->tag == Cyc_Dict_Absent){_LLE4: _LLE5: {
# 865
struct Cyc_Absyn_Enumdecl**_tmp1A9=({struct Cyc_Absyn_Enumdecl**_tmp1AA=_cycalloc(sizeof(*_tmp1AA));_tmp1AA[0]=ed;_tmp1AA;});
({struct Cyc_Dict_Dict _tmp2DD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Enumdecl**v))Cyc_Dict_insert)((te->ae)->enumdecls,q,_tmp1A9);(te->ae)->enumdecls=_tmp2DD;});
goto _LLE3;}}else{_LLE6: _tmp1AB=_tmp1A8;_LLE7:(int)_rethrow(_tmp1AB);}_LLE3:;}};}
# 871
if(ed->fields != 0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1AC=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_check_null(_tmp1AC->tag));
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp1AC->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp1B0;_tmp1B0.tag=0;_tmp1B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp1AC->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp1AF;_tmp1AF.tag=0;_tmp1AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*q).f2);({void*_tmp1AD[2]={& _tmp1AF,& _tmp1B0};({unsigned int _tmp2DF=loc;struct _dyneither_ptr _tmp2DE=({const char*_tmp1AE="enum %s, field %s: expression is not constant";_tag_dyneither(_tmp1AE,sizeof(char),46);});Cyc_Tcutil_terr(_tmp2DF,_tmp2DE,_tag_dyneither(_tmp1AD,sizeof(void*),2));});});});});}}}
# 881
static int Cyc_Tc_okay_externC(unsigned int loc,enum Cyc_Absyn_Scope sc,int in_include){
enum Cyc_Absyn_Scope _tmp1B1=sc;switch(_tmp1B1){case Cyc_Absyn_Static: _LLE9: _LLEA:
# 884
 if(!in_include)
({void*_tmp1B2=0;({unsigned int _tmp2E1=loc;struct _dyneither_ptr _tmp2E0=({const char*_tmp1B3="static declaration nested within extern \"C\"";_tag_dyneither(_tmp1B3,sizeof(char),44);});Cyc_Tcutil_warn(_tmp2E1,_tmp2E0,_tag_dyneither(_tmp1B2,sizeof(void*),0));});});
return 0;case Cyc_Absyn_Abstract: _LLEB: _LLEC:
# 888
({void*_tmp1B4=0;({unsigned int _tmp2E3=loc;struct _dyneither_ptr _tmp2E2=({const char*_tmp1B5="abstract declaration nested within extern \"C\"";_tag_dyneither(_tmp1B5,sizeof(char),46);});Cyc_Tcutil_warn(_tmp2E3,_tmp2E2,_tag_dyneither(_tmp1B4,sizeof(void*),0));});});
return 0;case Cyc_Absyn_Public: _LLED: _LLEE:
 goto _LLF0;case Cyc_Absyn_Register: _LLEF: _LLF0:
 goto _LLF2;case Cyc_Absyn_Extern: _LLF1: _LLF2:
 return 1;default: _LLF3: _LLF4:
# 894
({void*_tmp1B6=0;({unsigned int _tmp2E5=loc;struct _dyneither_ptr _tmp2E4=({const char*_tmp1B7="nested extern \"C\" declaration";_tag_dyneither(_tmp1B7,sizeof(char),30);});Cyc_Tcutil_warn(_tmp2E5,_tmp2E4,_tag_dyneither(_tmp1B6,sizeof(void*),0));});});
return 1;}_LLE8:;}
# 904
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 909
struct Cyc_List_List*last=0;
# 911
struct Cyc_List_List*_tmp1B8=ds0;for(0;_tmp1B8 != 0;(last=_tmp1B8,_tmp1B8=_tmp1B8->tl)){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_tmp1B8->hd;
unsigned int loc=d->loc;
# 915
void*_tmp1B9=d->r;void*_tmp1BA=_tmp1B9;struct Cyc_List_List*_tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D3;union Cyc_Absyn_Nmspace _tmp1D2;struct _dyneither_ptr*_tmp1D1;struct Cyc_List_List*_tmp1D0;struct _dyneither_ptr*_tmp1CF;struct Cyc_List_List*_tmp1CE;struct Cyc_Absyn_Enumdecl*_tmp1CD;struct Cyc_Absyn_Datatypedecl*_tmp1CC;struct Cyc_Absyn_Aggrdecl*_tmp1CB;struct Cyc_Absyn_Typedefdecl*_tmp1CA;struct Cyc_Absyn_Fndecl*_tmp1C9;struct Cyc_Absyn_Vardecl*_tmp1C8;switch(*((int*)_tmp1BA)){case 2: _LLF6: _LLF7:
 goto _LLF9;case 3: _LLF8: _LLF9:
# 918
({void*_tmp1BB=0;({unsigned int _tmp2E7=loc;struct _dyneither_ptr _tmp2E6=({const char*_tmp1BC="top level let-declarations are not implemented";_tag_dyneither(_tmp1BC,sizeof(char),47);});Cyc_Tcutil_terr(_tmp2E7,_tmp2E6,_tag_dyneither(_tmp1BB,sizeof(void*),0));});});
goto _LLF5;case 4: _LLFA: _LLFB:
# 921
({void*_tmp1BD=0;({unsigned int _tmp2E9=loc;struct _dyneither_ptr _tmp2E8=({const char*_tmp1BE="top level region declarations are not implemented";_tag_dyneither(_tmp1BE,sizeof(char),50);});Cyc_Tcutil_terr(_tmp2E9,_tmp2E8,_tag_dyneither(_tmp1BD,sizeof(void*),0));});});
goto _LLF5;case 0: _LLFC: _tmp1C8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_LLFD:
# 924
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1C8->sc,te->in_extern_c_include))
_tmp1C8->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcVardecl(te,loc,_tmp1C8,check_var_init,te->in_extern_c_include,exports);
goto _LLF5;case 1: _LLFE: _tmp1C9=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_LLFF:
# 929
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1C9->sc,te->in_extern_c_include))
_tmp1C9->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcFndecl(te,loc,_tmp1C9,exports);
goto _LLF5;case 8: _LL100: _tmp1CA=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_LL101:
# 934
 _tmp1CA->extern_c=te->in_extern_c_include;
# 938
Cyc_Tc_tcTypedefdecl(te,loc,_tmp1CA);
goto _LLF5;case 5: _LL102: _tmp1CB=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_LL103:
# 941
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1CB->sc,te->in_extern_c_include))
_tmp1CB->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp1CB);
goto _LLF5;case 6: _LL104: _tmp1CC=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_LL105:
# 946
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1CC->sc,te->in_extern_c_include))
_tmp1CC->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcDatatypedecl(te,loc,_tmp1CC);
goto _LLF5;case 7: _LL106: _tmp1CD=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_LL107:
# 951
 if(in_externC  && Cyc_Tc_okay_externC(d->loc,_tmp1CD->sc,te->in_extern_c_include))
_tmp1CD->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp1CD);
goto _LLF5;case 13: _LL108: _LL109:
# 956
({void*_tmp1BF=0;({unsigned int _tmp2EB=d->loc;struct _dyneither_ptr _tmp2EA=({const char*_tmp1C0="spurious __cyclone_port_on__";_tag_dyneither(_tmp1C0,sizeof(char),29);});Cyc_Tcutil_warn(_tmp2EB,_tmp2EA,_tag_dyneither(_tmp1BF,sizeof(void*),0));});});
goto _LLF5;case 14: _LL10A: _LL10B:
# 959
 goto _LLF5;case 9: _LL10C: _tmp1CF=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_tmp1CE=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1BA)->f2;_LL10D: {
# 962
struct Cyc_List_List*_tmp1C1=te->ns;
({struct Cyc_List_List*_tmp2ED=({struct Cyc_List_List*_tmp2EC=_tmp1C1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp2EC,({struct Cyc_List_List*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->hd=_tmp1CF;_tmp1C2->tl=0;_tmp1C2;}));});te->ns=_tmp2ED;});
Cyc_Tc_tc_decls(te,_tmp1CE,in_externC,check_var_init,exports);
te->ns=_tmp1C1;
goto _LLF5;}case 10: _LL10E: _tmp1D2=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1)->f1;_tmp1D1=(((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1)->f2;_tmp1D0=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1BA)->f2;_LL10F:
# 970
 Cyc_Tc_tc_decls(te,_tmp1D0,in_externC,check_var_init,exports);
goto _LLF5;case 11: _LL110: _tmp1D3=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_LL111:
# 974
 Cyc_Tc_tc_decls(te,_tmp1D3,1,check_var_init,exports);
goto _LLF5;default: _LL112: _tmp1D5=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BA)->f1;_tmp1D4=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1BA)->f2;_LL113: {
# 980
struct Cyc_List_List*newexs=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1D4,(unsigned int)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*_tmp1C3=Cyc_Tcenv_enter_extern_c_include(te);
Cyc_Tc_tc_decls(_tmp1C3,_tmp1D5,1,check_var_init,& newexs);
# 985
for(0;_tmp1D4 != 0;_tmp1D4=_tmp1D4->tl){
struct _tuple12*_tmp1C4=(struct _tuple12*)_tmp1D4->hd;
if(!(*_tmp1C4).f3)
({struct Cyc_String_pa_PrintArg_struct _tmp1C7;_tmp1C7.tag=0;({struct _dyneither_ptr _tmp2EE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string((*_tmp1C4).f2));_tmp1C7.f1=_tmp2EE;});({void*_tmp1C5[1]={& _tmp1C7};({unsigned int _tmp2F0=(*_tmp1C4).f1;struct _dyneither_ptr _tmp2EF=({const char*_tmp1C6="%s is exported but not defined";_tag_dyneither(_tmp1C6,sizeof(char),31);});Cyc_Tcutil_warn(_tmp2F0,_tmp2EF,_tag_dyneither(_tmp1C5,sizeof(void*),1));});});});}
# 991
goto _LLF5;}}_LLF5:;}}
# 996
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 1014 "tc.cyc"
typedef struct Cyc_Tc_TreeshakeEnv*Cyc_Tc_treeshake_env_t;
# 1016
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d);
# 1018
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return((struct Cyc_List_List*(*)(int(*f)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*x))Cyc_List_filter_c)(Cyc_Tc_vardecl_needed,env,ds);}
# 1022
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_tmp1D6=d->r;void*_tmp1D7=_tmp1D6;struct Cyc_List_List**_tmp1DC;struct Cyc_List_List**_tmp1DB;struct Cyc_List_List**_tmp1DA;struct Cyc_List_List**_tmp1D9;struct Cyc_Absyn_Vardecl*_tmp1D8;switch(*((int*)_tmp1D7)){case 0: _LL115: _tmp1D8=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL116:
# 1027
 if((env->in_cinclude  || _tmp1D8->sc != Cyc_Absyn_Extern  && _tmp1D8->sc != Cyc_Absyn_ExternC) || !
Cyc_Absyn_qvar_cmp(_tmp1D8->name,Cyc_Absyn_uniquergn_qvar))
# 1030
return 1;
# 1032
return(*((struct _tuple14*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)(env->ordinaries,_tmp1D8->name)).f2;case 11: _LL117: _tmp1D9=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL118:
 _tmp1DA=_tmp1D9;goto _LL11A;case 10: _LL119: _tmp1DA=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp1D7)->f2;_LL11A:
 _tmp1DB=_tmp1DA;goto _LL11C;case 9: _LL11B: _tmp1DB=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp1D7)->f2;_LL11C:
# 1036
({struct Cyc_List_List*_tmp2F1=Cyc_Tc_treeshake_f(env,*_tmp1DB);*_tmp1DB=_tmp2F1;});
return 1;case 12: _LL11D: _tmp1DC=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp1D7)->f1;_LL11E: {
# 1039
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_tmp2F2=Cyc_Tc_treeshake_f(env,*_tmp1DC);*_tmp1DC=_tmp2F2;});
env->in_cinclude=in_cinclude;
return 1;}default: _LL11F: _LL120:
 return 1;}_LL114:;}
# 1048
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv _tmp1DD=({struct Cyc_Tc_TreeshakeEnv _tmp1DE;_tmp1DE.in_cinclude=0;_tmp1DE.ordinaries=(te->ae)->ordinaries;_tmp1DE;});
return Cyc_Tc_treeshake_f(& _tmp1DD,ds);}
