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
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 49
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 944
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 1165
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[13];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 34 "tcdecl.h"
typedef struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_xdatatypefielddecl_t;
# 38
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple10{enum Cyc_Absyn_Scope f1;int f2;};
# 54 "tcdecl.h"
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int allow_externC_extern_merge);
# 59
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 62
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 64
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 66
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 68
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 72
void**Cyc_Tcdecl_merge_binding(void*d0,void*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 75
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg);
# 81
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
# 30 "rgnorder.h"
typedef struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_rgn_po_t;
# 32
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_initial_fn_po(struct _RegionHandle*,struct Cyc_List_List*tvs,struct Cyc_List_List*po,void*effect,struct Cyc_Absyn_Tvar*fst_rgn,unsigned int);
# 39
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_outlives_constraint(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn,unsigned int loc);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 86
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 130 "tcutil.h"
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 143
void Cyc_Tcutil_explain_failure();
# 145
int Cyc_Tcutil_unify(void*,void*);
# 148
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 235 "tcutil.h"
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 302
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[13]="Incompatible";
# 36 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};
# 38
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 42
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _dyneither_ptr fmt2;
if(({Cyc_strlen((struct _dyneither_ptr)*msg1)== 0;}))
({struct _dyneither_ptr _tmp17E=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp17D=({const char*_tmp1="%s";_tag_dyneither(_tmp1,sizeof(char),3);});Cyc_strconcat(_tmp17D,(struct _dyneither_ptr)fmt);});fmt2=_tmp17E;});else{
# 48
({struct _dyneither_ptr _tmp180=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp17F=({const char*_tmp2="%s ";_tag_dyneither(_tmp2,sizeof(char),4);});Cyc_strconcat(_tmp17F,(struct _dyneither_ptr)fmt);});fmt2=_tmp180;});}{
struct _dyneither_ptr ap2=({unsigned int _tmp3=_get_dyneither_size(ap,sizeof(void*))+ 1;void**_tmp4=(void**)_cycalloc(_check_times(sizeof(void*),_tmp3));struct _dyneither_ptr _tmp8=_tag_dyneither(_tmp4,sizeof(void*),_tmp3);{unsigned int _tmp5=_tmp3;unsigned int i;for(i=0;i < _tmp5;i ++){({typeof((void*)(
i == 0?(void*)({struct Cyc_String_pa_PrintArg_struct*_tmp6=_cycalloc(sizeof(*_tmp6));({struct Cyc_String_pa_PrintArg_struct _tmp181=({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*msg1);_tmp7;});_tmp6[0]=_tmp181;});_tmp6;}):*((void**)_check_dyneither_subscript(ap,sizeof(void*),(int)(i - 1)))))_tmp182=(void*)(i == 0?(void*)({struct Cyc_String_pa_PrintArg_struct*_tmp6=_cycalloc(sizeof(*_tmp6));({struct Cyc_String_pa_PrintArg_struct _tmp181=({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*msg1);_tmp7;});_tmp6[0]=_tmp181;});_tmp6;}):*((void**)_check_dyneither_subscript(ap,sizeof(void*),(int)(i - 1))));_tmp4[i]=_tmp182;});}}_tmp8;});
Cyc_Position_post_error(({unsigned int _tmp183=loc;Cyc_Position_mk_err(_tmp183,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2));}));};};}
# 56
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 58
struct _dyneither_ptr s0_str=s0 == Cyc_Absyn_Public?({const char*_tmp10="public ";_tag_dyneither(_tmp10,sizeof(char),8);}): Cyc_Absynpp_scope2string(s0);
struct _dyneither_ptr s1_str=s1 == Cyc_Absyn_Public?({const char*_tmpF="public ";_tag_dyneither(_tmpF,sizeof(char),8);}): Cyc_Absynpp_scope2string(s1);
({struct Cyc_String_pa_PrintArg_struct _tmpE;_tmpE.tag=0;_tmpE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0_str);({struct Cyc_String_pa_PrintArg_struct _tmpD;_tmpD.tag=0;_tmpD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1_str);({struct Cyc_String_pa_PrintArg_struct _tmpC;_tmpC.tag=0;_tmpC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpB;_tmpB.tag=0;_tmpB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp9[4]={& _tmpB,& _tmpC,& _tmpD,& _tmpE};({unsigned int _tmp186=loc;struct _dyneither_ptr*_tmp185=msg;struct _dyneither_ptr _tmp184=({const char*_tmpA="%s %s is %sbut expected scope is %s";_tag_dyneither(_tmpA,sizeof(char),36);});Cyc_Tcdecl_merr(_tmp186,_tmp185,_tmp184,_tag_dyneither(_tmp9,sizeof(void*),4));});});});});});});}struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 64
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int externCmerge){
# 66
{struct _tuple11 _tmp11=({struct _tuple11 _tmp15;_tmp15.f1=s0;_tmp15.f2=s1;_tmp15;});struct _tuple11 _tmp12=_tmp11;switch(_tmp12.f1){case Cyc_Absyn_ExternC: switch(_tmp12.f2){case Cyc_Absyn_ExternC: _LL1: _LL2:
# 68
 goto _LL0;case Cyc_Absyn_Public: _LL3: _LL4:
# 70
 goto _LL6;case Cyc_Absyn_Extern: _LL5: _LL6:
 goto _LL8;default: _LLB: _LLC:
# 75
 goto _LLE;}case Cyc_Absyn_Public: switch(_tmp12.f2){case Cyc_Absyn_ExternC: _LL7: _LL8:
# 72
 goto _LLA;case Cyc_Absyn_Extern: goto _LLF;case Cyc_Absyn_Public: _LL15: _LL16:
# 82
 goto _LL18;default: goto _LL1B;}case Cyc_Absyn_Extern: switch(_tmp12.f2){case Cyc_Absyn_ExternC: _LL9: _LLA:
# 73
 if(externCmerge)goto _LL0;
goto _LLC;case Cyc_Absyn_Extern: goto _LLF;default: _LL11: _LL12:
# 79
 goto _LL0;}default: switch(_tmp12.f2){case Cyc_Absyn_ExternC: _LLD: _LLE:
# 76
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple10 _tmp13;_tmp13.f1=s1;_tmp13.f2=0;_tmp13;});case Cyc_Absyn_Extern: _LLF: _LL10:
# 78
 s1=s0;goto _LL0;default: switch(_tmp12.f1){case Cyc_Absyn_Static: if(_tmp12.f2 == Cyc_Absyn_Static){_LL13: _LL14:
# 81
 goto _LL16;}else{goto _LL1B;}case Cyc_Absyn_Abstract: if(_tmp12.f2 == Cyc_Absyn_Abstract){_LL17: _LL18:
# 83
 goto _LL0;}else{goto _LL1B;}default: if(_tmp12.f2 == Cyc_Absyn_Register){_LL19: _LL1A:
 goto _LL0;}else{_LL1B: _LL1C:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple10 _tmp14;_tmp14.f1=s1;_tmp14.f2=0;_tmp14;});}}}}_LL0:;}
# 87
return({struct _tuple10 _tmp16;_tmp16.f1=s1;_tmp16.f2=1;_tmp16;});}
# 90
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 92
return Cyc_Tcutil_unify(t0,t1);}
# 96
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_tmp17=e->r;void*_tmp18=_tmp17;int _tmp1C;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18)->f1).Int_c).tag == 5){_LL1E: _tmp1C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp18)->f1).Int_c).val).f2;_LL1F:
 return(unsigned int)_tmp1C;}else{goto _LL20;}}else{_LL20: _LL21:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp19=_cycalloc(sizeof(*_tmp19));({struct Cyc_Core_Invalid_argument_exn_struct _tmp188=({struct Cyc_Core_Invalid_argument_exn_struct _tmp1A;_tmp1A.tag=Cyc_Core_Invalid_argument;({struct _dyneither_ptr _tmp187=({const char*_tmp1B="Tcdecl::get_uint_const_value";_tag_dyneither(_tmp1B,sizeof(char),29);});_tmp1A.f1=_tmp187;});_tmp1A;});_tmp19[0]=_tmp188;});_tmp19;}));}_LL1D:;}
# 103
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 106
if(({int _tmp189=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0);_tmp189 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp20;_tmp20.tag=0;_tmp20.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp1F;_tmp1F.tag=0;_tmp1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp1D[2]={& _tmp1F,& _tmp20};({unsigned int _tmp18C=loc;struct _dyneither_ptr*_tmp18B=msg;struct _dyneither_ptr _tmp18A=({const char*_tmp1E="%s %s has a different number of type parameters";_tag_dyneither(_tmp1E,sizeof(char),48);});Cyc_Tcdecl_merr(_tmp18C,_tmp18B,_tmp18A,_tag_dyneither(_tmp1D,sizeof(void*),2));});});});});
return 0;}{
# 110
struct Cyc_List_List*_tmp21=tvs0;
struct Cyc_List_List*_tmp22=tvs1;
for(0;_tmp21 != 0;(_tmp21=_tmp21->tl,_tmp22=_tmp22->tl)){
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp21->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp22))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp23=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp21->hd)->kind);
struct Cyc_Absyn_Kind*_tmp24=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp22->hd)->kind);
if(_tmp23 == _tmp24)continue;
({struct Cyc_String_pa_PrintArg_struct _tmp2B;_tmp2B.tag=0;({struct _dyneither_ptr _tmp18D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp24));_tmp2B.f1=_tmp18D;});({struct Cyc_String_pa_PrintArg_struct _tmp2A;_tmp2A.tag=0;_tmp2A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp21->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp29;_tmp29.tag=0;({struct _dyneither_ptr _tmp18E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp23));_tmp29.f1=_tmp18E;});({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp25[5]={& _tmp27,& _tmp28,& _tmp29,& _tmp2A,& _tmp2B};({unsigned int _tmp191=loc;struct _dyneither_ptr*_tmp190=msg;struct _dyneither_ptr _tmp18F=({const char*_tmp26="%s %s has a different kind (%s) for type parameter %s (%s)";_tag_dyneither(_tmp26,sizeof(char),59);});Cyc_Tcdecl_merr(_tmp191,_tmp190,_tmp18F,_tag_dyneither(_tmp25,sizeof(void*),5));});});});});});});});
return 0;};}
# 121
return 1;};}
# 124
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 126
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0;_tmp2F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp2E;_tmp2E.tag=0;_tmp2E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp2C[2]={& _tmp2E,& _tmp2F};({unsigned int _tmp194=loc;struct _dyneither_ptr*_tmp193=msg;struct _dyneither_ptr _tmp192=({const char*_tmp2D="%s %s has different attributes";_tag_dyneither(_tmp2D,sizeof(char),31);});Cyc_Tcdecl_merr(_tmp194,_tmp193,_tmp192,_tag_dyneither(_tmp2C,sizeof(void*),2));});});});});
return 0;}
# 130
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 133
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 135
struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
({struct Cyc_List_List*_tmp198=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _tuple12*_tmp197=({struct _tuple12*_tmp31=_cycalloc(sizeof(*_tmp31));_tmp31->f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd;({void*_tmp196=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp32=_cycalloc(sizeof(*_tmp32));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp195=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp33;_tmp33.tag=2;_tmp33.f1=(struct Cyc_Absyn_Tvar*)tvs0->hd;_tmp33;});_tmp32[0]=_tmp195;});_tmp32;});_tmp31->f2=_tmp196;});_tmp31;});_tmp30->hd=_tmp197;});_tmp30->tl=inst;_tmp30;});inst=_tmp198;});}
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple14{void*f1;void*f2;};
# 141
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 143
struct _dyneither_ptr _tmp34=Cyc_Absynpp_qvar2string(d0->name);
int _tmp35=1;
# 149
if(!(d0->kind == d1->kind))return 0;
# 152
if(!({struct Cyc_List_List*_tmp19D=d0->tvs;struct Cyc_List_List*_tmp19C=d1->tvs;struct _dyneither_ptr _tmp19B=({const char*_tmp36="type";_tag_dyneither(_tmp36,sizeof(char),5);});struct _dyneither_ptr _tmp19A=_tmp34;unsigned int _tmp199=loc;Cyc_Tcdecl_check_tvs(_tmp19D,_tmp19C,_tmp19B,_tmp19A,_tmp199,msg);}))return 0;{
# 155
struct _tuple10 _tmp37=({
enum Cyc_Absyn_Scope _tmp1A2=d0->sc;enum Cyc_Absyn_Scope _tmp1A1=d1->sc;struct _dyneither_ptr _tmp1A0=({const char*_tmp90="type";_tag_dyneither(_tmp90,sizeof(char),5);});struct _dyneither_ptr _tmp19F=_tmp34;unsigned int _tmp19E=loc;Cyc_Tcdecl_merge_scope(_tmp1A2,_tmp1A1,_tmp1A0,_tmp19F,_tmp19E,msg,1);});
# 155
struct _tuple10 _tmp38=_tmp37;enum Cyc_Absyn_Scope _tmp8F;int _tmp8E;_LL23: _tmp8F=_tmp38.f1;_tmp8E=_tmp38.f2;_LL24:;
# 157
if(!_tmp8E)_tmp35=0;
# 160
if(!({struct Cyc_List_List*_tmp1A7=d0->attributes;struct Cyc_List_List*_tmp1A6=d1->attributes;struct _dyneither_ptr _tmp1A5=({const char*_tmp39="type";_tag_dyneither(_tmp39,sizeof(char),5);});struct _dyneither_ptr _tmp1A4=_tmp34;unsigned int _tmp1A3=loc;Cyc_Tcdecl_check_atts(_tmp1A7,_tmp1A6,_tmp1A5,_tmp1A4,_tmp1A3,msg);}))_tmp35=0;{
# 162
struct Cyc_Absyn_Aggrdecl*d2;
# 164
{struct _tuple13 _tmp3A=({struct _tuple13 _tmp8C;_tmp8C.f1=d0->impl;_tmp8C.f2=d1->impl;_tmp8C;});struct _tuple13 _tmp3B=_tmp3A;struct Cyc_List_List*_tmp8B;struct Cyc_List_List*_tmp8A;struct Cyc_List_List*_tmp89;int _tmp88;struct Cyc_List_List*_tmp87;struct Cyc_List_List*_tmp86;struct Cyc_List_List*_tmp85;int _tmp84;if(_tmp3B.f2 == 0){_LL26: _LL27:
 d2=d0;goto _LL25;}else{if(_tmp3B.f1 == 0){_LL28: _LL29:
 d2=d1;goto _LL25;}else{_LL2A: _tmp8B=(_tmp3B.f1)->exist_vars;_tmp8A=(_tmp3B.f1)->rgn_po;_tmp89=(_tmp3B.f1)->fields;_tmp88=(_tmp3B.f1)->tagged;_tmp87=(_tmp3B.f2)->exist_vars;_tmp86=(_tmp3B.f2)->rgn_po;_tmp85=(_tmp3B.f2)->fields;_tmp84=(_tmp3B.f2)->tagged;_LL2B:
# 171
 if(!({struct Cyc_List_List*_tmp1AC=_tmp8B;struct Cyc_List_List*_tmp1AB=_tmp87;struct _dyneither_ptr _tmp1AA=({const char*_tmp3C="type";_tag_dyneither(_tmp3C,sizeof(char),5);});struct _dyneither_ptr _tmp1A9=_tmp34;unsigned int _tmp1A8=loc;Cyc_Tcdecl_check_tvs(_tmp1AC,_tmp1AB,_tmp1AA,_tmp1A9,_tmp1A8,msg);}))
return 0;{
# 175
struct Cyc_List_List*_tmp3D=({struct Cyc_List_List*_tmp1AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp8B);Cyc_Tcdecl_build_tvs_map(_tmp1AD,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp87));});
# 180
for(0;_tmp8A != 0  && _tmp86 != 0;
(_tmp8A=_tmp8A->tl,_tmp86=_tmp86->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp8A->hd)).f1,(*((struct _tuple14*)_tmp86->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp8A->hd)).f2,(*((struct _tuple14*)_tmp86->hd)).f2);}
# 187
for(0;_tmp89 != 0  && _tmp85 != 0;(_tmp89=_tmp89->tl,_tmp85=_tmp85->tl)){
struct Cyc_Absyn_Aggrfield*_tmp3E=(struct Cyc_Absyn_Aggrfield*)_tmp89->hd;struct Cyc_Absyn_Aggrfield*_tmp3F=_tmp3E;struct _dyneither_ptr*_tmp78;struct Cyc_Absyn_Tqual _tmp77;void*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_Exp*_tmp73;_LL2D: _tmp78=_tmp3F->name;_tmp77=_tmp3F->tq;_tmp76=_tmp3F->type;_tmp75=_tmp3F->width;_tmp74=_tmp3F->attributes;_tmp73=_tmp3F->requires_clause;_LL2E:;{
struct Cyc_Absyn_Aggrfield*_tmp40=(struct Cyc_Absyn_Aggrfield*)_tmp85->hd;struct Cyc_Absyn_Aggrfield*_tmp41=_tmp40;struct _dyneither_ptr*_tmp72;struct Cyc_Absyn_Tqual _tmp71;void*_tmp70;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_List_List*_tmp6E;struct Cyc_Absyn_Exp*_tmp6D;_LL30: _tmp72=_tmp41->name;_tmp71=_tmp41->tq;_tmp70=_tmp41->type;_tmp6F=_tmp41->width;_tmp6E=_tmp41->attributes;_tmp6D=_tmp41->requires_clause;_LL31:;
# 191
if(({Cyc_strptrcmp(_tmp78,_tmp72)!= 0;})){
({struct Cyc_String_pa_PrintArg_struct _tmp47;_tmp47.tag=0;_tmp47.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp72);({struct Cyc_String_pa_PrintArg_struct _tmp46;_tmp46.tag=0;_tmp46.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp78);({struct Cyc_String_pa_PrintArg_struct _tmp45;_tmp45.tag=0;_tmp45.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp44;_tmp44.tag=0;({struct _dyneither_ptr _tmp1AE=(struct _dyneither_ptr)({const char*_tmp48="type";_tag_dyneither(_tmp48,sizeof(char),5);});_tmp44.f1=_tmp1AE;});({void*_tmp42[4]={& _tmp44,& _tmp45,& _tmp46,& _tmp47};({unsigned int _tmp1B1=loc;struct _dyneither_ptr*_tmp1B0=msg;struct _dyneither_ptr _tmp1AF=({const char*_tmp43="%s %s : field name mismatch %s != %s";_tag_dyneither(_tmp43,sizeof(char),37);});Cyc_Tcdecl_merr(_tmp1B1,_tmp1B0,_tmp1AF,_tag_dyneither(_tmp42,sizeof(void*),4));});});});});});});
# 194
return 0;}
# 196
if(!Cyc_Tcutil_same_atts(_tmp74,_tmp6E)){
({struct Cyc_String_pa_PrintArg_struct _tmp4D;_tmp4D.tag=0;_tmp4D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp78);({struct Cyc_String_pa_PrintArg_struct _tmp4C;_tmp4C.tag=0;_tmp4C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp4B;_tmp4B.tag=0;({struct _dyneither_ptr _tmp1B2=(struct _dyneither_ptr)({const char*_tmp4E="type";_tag_dyneither(_tmp4E,sizeof(char),5);});_tmp4B.f1=_tmp1B2;});({void*_tmp49[3]={& _tmp4B,& _tmp4C,& _tmp4D};({unsigned int _tmp1B5=loc;struct _dyneither_ptr*_tmp1B4=msg;struct _dyneither_ptr _tmp1B3=({const char*_tmp4A="%s %s : attribute mismatch on field %s";_tag_dyneither(_tmp4A,sizeof(char),39);});Cyc_Tcdecl_merr(_tmp1B5,_tmp1B4,_tmp1B3,_tag_dyneither(_tmp49,sizeof(void*),3));});});});});});
_tmp35=0;}
# 200
if(!Cyc_Tcutil_equal_tqual(_tmp77,_tmp71)){
({struct Cyc_String_pa_PrintArg_struct _tmp53;_tmp53.tag=0;_tmp53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp78);({struct Cyc_String_pa_PrintArg_struct _tmp52;_tmp52.tag=0;_tmp52.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp51;_tmp51.tag=0;({struct _dyneither_ptr _tmp1B6=(struct _dyneither_ptr)({const char*_tmp54="type";_tag_dyneither(_tmp54,sizeof(char),5);});_tmp51.f1=_tmp1B6;});({void*_tmp4F[3]={& _tmp51,& _tmp52,& _tmp53};({unsigned int _tmp1B9=loc;struct _dyneither_ptr*_tmp1B8=msg;struct _dyneither_ptr _tmp1B7=({const char*_tmp50="%s %s : qualifier mismatch on field %s";_tag_dyneither(_tmp50,sizeof(char),39);});Cyc_Tcdecl_merr(_tmp1B9,_tmp1B8,_tmp1B7,_tag_dyneither(_tmp4F,sizeof(void*),3));});});});});});
_tmp35=0;}
# 204
if(((_tmp75 != 0  && _tmp6F != 0) && ({
unsigned int _tmp1BA=Cyc_Tcdecl_get_uint_const_value(_tmp75);_tmp1BA != 
Cyc_Tcdecl_get_uint_const_value(_tmp6F);}) || 
_tmp75 == 0  && _tmp6F != 0) || 
_tmp75 != 0  && _tmp6F == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp59;_tmp59.tag=0;_tmp59.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp78);({struct Cyc_String_pa_PrintArg_struct _tmp58;_tmp58.tag=0;_tmp58.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({struct Cyc_String_pa_PrintArg_struct _tmp57;_tmp57.tag=0;({struct _dyneither_ptr _tmp1BB=(struct _dyneither_ptr)({const char*_tmp5A="type";_tag_dyneither(_tmp5A,sizeof(char),5);});_tmp57.f1=_tmp1BB;});({void*_tmp55[3]={& _tmp57,& _tmp58,& _tmp59};({unsigned int _tmp1BE=loc;struct _dyneither_ptr*_tmp1BD=msg;struct _dyneither_ptr _tmp1BC=({const char*_tmp56="%s %s : bitfield mismatch on field %s";_tag_dyneither(_tmp56,sizeof(char),38);});Cyc_Tcdecl_merr(_tmp1BE,_tmp1BD,_tmp1BC,_tag_dyneither(_tmp55,sizeof(void*),3));});});});});});
_tmp35=0;}
# 212
if(_tmp73 != 0  && _tmp6D != 0){
void*subst_w1=({struct Cyc_List_List*_tmp1C0=_tmp3D;Cyc_Tcutil_substitute(_tmp1C0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp62=_cycalloc(sizeof(*_tmp62));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1BF=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp63;_tmp63.tag=18;_tmp63.f1=_tmp6D;_tmp63;});_tmp62[0]=_tmp1BF;});_tmp62;}));});
if(!({void*_tmp1C2=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp1C1=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp5C;_tmp5C.tag=18;_tmp5C.f1=_tmp73;_tmp5C;});_tmp5B[0]=_tmp1C1;});_tmp5B;});Cyc_Tcdecl_check_type(_tmp1C2,subst_w1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0;({struct _dyneither_ptr _tmp1C3=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp6D));_tmp61.f1=_tmp1C3;});({struct Cyc_String_pa_PrintArg_struct _tmp60;_tmp60.tag=0;({struct _dyneither_ptr _tmp1C4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp73));_tmp60.f1=_tmp1C4;});({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0;_tmp5F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp5D[3]={& _tmp5F,& _tmp60,& _tmp61};({unsigned int _tmp1C7=loc;struct _dyneither_ptr*_tmp1C6=msg;struct _dyneither_ptr _tmp1C5=({const char*_tmp5E="type %s : mismatch on @requires clauses: %s != %s";_tag_dyneither(_tmp5E,sizeof(char),50);});Cyc_Tcdecl_merr(_tmp1C7,_tmp1C6,_tmp1C5,_tag_dyneither(_tmp5D,sizeof(void*),3));});});});});});
_tmp35=0;}}else{
# 219
if(_tmp73 == 0  && _tmp6D != 0  || 
_tmp73 != 0  && _tmp6D == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp66;_tmp66.tag=0;_tmp66.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp64[1]={& _tmp66};({unsigned int _tmp1CA=loc;struct _dyneither_ptr*_tmp1C9=msg;struct _dyneither_ptr _tmp1C8=({const char*_tmp65="type %s : mismatch on @requires clauses";_tag_dyneither(_tmp65,sizeof(char),40);});Cyc_Tcdecl_merr(_tmp1CA,_tmp1C9,_tmp1C8,_tag_dyneither(_tmp64,sizeof(void*),1));});});});
_tmp35=0;}}{
# 225
void*subst_t1=Cyc_Tcutil_substitute(_tmp3D,_tmp70);
if(!Cyc_Tcdecl_check_type(_tmp76,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmp6C;_tmp6C.tag=0;({struct _dyneither_ptr _tmp1CB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1));_tmp6C.f1=_tmp1CB;});({struct Cyc_String_pa_PrintArg_struct _tmp6B;_tmp6B.tag=0;({struct _dyneither_ptr _tmp1CC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp76));_tmp6B.f1=_tmp1CC;});({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0;_tmp6A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp78);({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0;_tmp69.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp67[4]={& _tmp69,& _tmp6A,& _tmp6B,& _tmp6C};({unsigned int _tmp1CF=loc;struct _dyneither_ptr*_tmp1CE=msg;struct _dyneither_ptr _tmp1CD=({const char*_tmp68="type %s : type mismatch on field %s: %s != %s";_tag_dyneither(_tmp68,sizeof(char),46);});Cyc_Tcdecl_merr(_tmp1CF,_tmp1CE,_tmp1CD,_tag_dyneither(_tmp67,sizeof(void*),4));});});});});});});
Cyc_Tcutil_explain_failure();
_tmp35=0;}};};}
# 234
if(_tmp89 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp89->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp79[2]={& _tmp7B,& _tmp7C};({unsigned int _tmp1D2=loc;struct _dyneither_ptr*_tmp1D1=msg;struct _dyneither_ptr _tmp1D0=({const char*_tmp7A="type %s is missing field %s";_tag_dyneither(_tmp7A,sizeof(char),28);});Cyc_Tcdecl_merr(_tmp1D2,_tmp1D1,_tmp1D0,_tag_dyneither(_tmp79,sizeof(void*),2));});});});});
_tmp35=0;}
# 238
if(_tmp85 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp80;_tmp80.tag=0;_tmp80.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp85->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp7F;_tmp7F.tag=0;_tmp7F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp7D[2]={& _tmp7F,& _tmp80};({unsigned int _tmp1D5=loc;struct _dyneither_ptr*_tmp1D4=msg;struct _dyneither_ptr _tmp1D3=({const char*_tmp7E="type %s has extra field %s";_tag_dyneither(_tmp7E,sizeof(char),27);});Cyc_Tcdecl_merr(_tmp1D5,_tmp1D4,_tmp1D3,_tag_dyneither(_tmp7D,sizeof(void*),2));});});});});
_tmp35=0;}
# 243
if(_tmp88 != _tmp84){
({struct Cyc_String_pa_PrintArg_struct _tmp83;_tmp83.tag=0;_tmp83.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp34);({void*_tmp81[1]={& _tmp83};({unsigned int _tmp1D8=loc;struct _dyneither_ptr*_tmp1D7=msg;struct _dyneither_ptr _tmp1D6=({const char*_tmp82="%s : previous declaration disagrees with @tagged qualifier";_tag_dyneither(_tmp82,sizeof(char),59);});Cyc_Tcdecl_merr(_tmp1D8,_tmp1D7,_tmp1D6,_tag_dyneither(_tmp81,sizeof(void*),1));});});});
_tmp35=0;}
# 248
d2=d0;
goto _LL25;};}}_LL25:;}
# 252
if(!_tmp35)return 0;
# 254
if(_tmp8F == d2->sc)
return d2;else{
# 257
({struct Cyc_Absyn_Aggrdecl*_tmp1D9=({struct Cyc_Absyn_Aggrdecl*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=*d2;_tmp8D;});d2=_tmp1D9;});
d2->sc=_tmp8F;
return d2;}};};}
# 263
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
return is_x?({const char*_tmp91="@extensible datatype";_tag_dyneither(_tmp91,sizeof(char),21);}):({const char*_tmp92="datatype";_tag_dyneither(_tmp92,sizeof(char),9);});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 269
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 273
unsigned int loc=f1->loc;
# 276
if(({Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0;})){
({struct Cyc_String_pa_PrintArg_struct _tmp98;_tmp98.tag=0;_tmp98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp97;_tmp97.tag=0;_tmp97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp96;_tmp96.tag=0;_tmp96.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp95;_tmp95.tag=0;_tmp95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp93[4]={& _tmp95,& _tmp96,& _tmp97,& _tmp98};({unsigned int _tmp1DC=loc;struct _dyneither_ptr*_tmp1DB=msg;struct _dyneither_ptr _tmp1DA=({const char*_tmp94="%s %s: field name mismatch %s != %s";_tag_dyneither(_tmp94,sizeof(char),36);});Cyc_Tcdecl_merr(_tmp1DC,_tmp1DB,_tmp1DA,_tag_dyneither(_tmp93,sizeof(void*),4));});});});});});});
# 279
return 0;}{
# 282
struct _dyneither_ptr _tmp99=*(*f0->name).f2;
# 285
struct _tuple10 _tmp9A=({
enum Cyc_Absyn_Scope _tmp1E2=f0->sc;enum Cyc_Absyn_Scope _tmp1E1=f1->sc;struct _dyneither_ptr _tmp1E0=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpB6;_tmpB6.tag=0;_tmpB6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpB3[2]={& _tmpB5,& _tmpB6};({struct _dyneither_ptr _tmp1DD=({const char*_tmpB4="in %s %s, field";_tag_dyneither(_tmpB4,sizeof(char),16);});Cyc_aprintf(_tmp1DD,_tag_dyneither(_tmpB3,sizeof(void*),2));});});});});struct _dyneither_ptr _tmp1DF=_tmp99;unsigned int _tmp1DE=loc;Cyc_Tcdecl_merge_scope(_tmp1E2,_tmp1E1,_tmp1E0,_tmp1DF,_tmp1DE,msg,0);});
# 285
struct _tuple10 _tmp9B=_tmp9A;enum Cyc_Absyn_Scope _tmpB2;int _tmpB1;_LL33: _tmpB2=_tmp9B.f1;_tmpB1=_tmp9B.f2;_LL34:;{
# 289
struct Cyc_List_List*_tmp9C=f0->typs;
struct Cyc_List_List*_tmp9D=f1->typs;
if(({int _tmp1E3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9C);_tmp1E3 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9D);})){
({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0;_tmpA1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp9E[3]={& _tmpA0,& _tmpA1,& _tmpA2};({unsigned int _tmp1E6=loc;struct _dyneither_ptr*_tmp1E5=msg;struct _dyneither_ptr _tmp1E4=({const char*_tmp9F="%s %s, field %s: parameter number mismatch";_tag_dyneither(_tmp9F,sizeof(char),43);});Cyc_Tcdecl_merr(_tmp1E6,_tmp1E5,_tmp1E4,_tag_dyneither(_tmp9E,sizeof(void*),3));});});});});});
_tmpB1=0;}
# 296
for(0;_tmp9C != 0  && _tmp9D != 0;(_tmp9C=_tmp9C->tl,_tmp9D=_tmp9D->tl)){
# 298
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmp9C->hd)).f1,(*((struct _tuple15*)_tmp9D->hd)).f1)){
({struct Cyc_String_pa_PrintArg_struct _tmpA7;_tmpA7.tag=0;_tmpA7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({struct Cyc_String_pa_PrintArg_struct _tmpA6;_tmpA6.tag=0;_tmpA6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpA3[3]={& _tmpA5,& _tmpA6,& _tmpA7};({unsigned int _tmp1E9=loc;struct _dyneither_ptr*_tmp1E8=msg;struct _dyneither_ptr _tmp1E7=({const char*_tmpA4="%s %s, field %s: parameter qualifier";_tag_dyneither(_tmpA4,sizeof(char),37);});Cyc_Tcdecl_merr(_tmp1E9,_tmp1E8,_tmp1E7,_tag_dyneither(_tmpA3,sizeof(void*),3));});});});});});
_tmpB1=0;}{
# 302
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmp9D->hd)).f2);
# 304
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmp9C->hd)).f2,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmpAE;_tmpAE.tag=0;({struct _dyneither_ptr _tmp1EA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1));_tmpAE.f1=_tmp1EA;});({struct Cyc_String_pa_PrintArg_struct _tmpAD;_tmpAD.tag=0;({struct _dyneither_ptr _tmp1EB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)_tmp9C->hd)).f2));_tmpAD.f1=_tmp1EB;});({struct Cyc_String_pa_PrintArg_struct _tmpAC;_tmpAC.tag=0;_tmpAC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({struct Cyc_String_pa_PrintArg_struct _tmpAB;_tmpAB.tag=0;_tmpAB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpA8[5]={& _tmpAA,& _tmpAB,& _tmpAC,& _tmpAD,& _tmpAE};({unsigned int _tmp1EE=loc;struct _dyneither_ptr*_tmp1ED=msg;struct _dyneither_ptr _tmp1EC=({const char*_tmpA9="%s %s, field %s: parameter type mismatch %s != %s";_tag_dyneither(_tmpA9,sizeof(char),50);});Cyc_Tcdecl_merr(_tmp1EE,_tmp1ED,_tmp1EC,_tag_dyneither(_tmpA8,sizeof(void*),5));});});});});});});});
Cyc_Tcutil_explain_failure();
_tmpB1=0;}};}
# 312
if(!_tmpB1)return 0;
if(f0->sc != _tmpB2){
struct Cyc_Absyn_Datatypefield*_tmpAF=({struct Cyc_Absyn_Datatypefield*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0[0]=*f0;_tmpB0;});
_tmpAF->sc=_tmpB2;
return _tmpAF;}else{
return f0;}};};}
# 320
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){
# 323
struct _tuple15*_tmpB7=x;struct Cyc_Absyn_Tqual _tmpBA;void*_tmpB9;_LL36: _tmpBA=_tmpB7->f1;_tmpB9=_tmpB7->f2;_LL37:;
return({struct _tuple15*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->f1=_tmpBA;({void*_tmp1EF=Cyc_Tcutil_substitute(inst,_tmpB9);_tmpB8->f2=_tmp1EF;});_tmpB8;});}
# 329
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 331
struct Cyc_Absyn_Datatypefield*_tmpBB=({struct Cyc_Absyn_Datatypefield*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC[0]=*f1;_tmpBC;});
({struct Cyc_List_List*_tmp1F0=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);_tmpBB->typs=_tmp1F0;});
# 334
return _tmpBB;}
# 341
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 350
struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5[0]=0;_tmpC5;});
struct Cyc_List_List**_tmpBD=f2sp;
int cmp=-1;
# 354
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
while(f0s != 0  && ({({int _tmp1F1=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name);cmp=_tmp1F1;})< 0;})){
struct Cyc_List_List*_tmpBE=({struct Cyc_List_List*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd;_tmpBF->tl=0;_tmpBF;});
*_tmpBD=_tmpBE;
_tmpBD=& _tmpBE->tl;
f0s=f0s->tl;}
# 361
if(f0s == 0  || cmp > 0){
# 363
*incl=0;{
struct Cyc_List_List*_tmpC0=({struct Cyc_List_List*_tmpC1=_cycalloc(sizeof(*_tmpC1));({struct Cyc_Absyn_Datatypefield*_tmp1F2=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);_tmpC1->hd=_tmp1F2;});_tmpC1->tl=0;_tmpC1;});
*_tmpBD=_tmpC0;
_tmpBD=& _tmpC0->tl;};}else{
# 369
struct Cyc_Absyn_Datatypefield*_tmpC2=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(_tmpC2 != 0){
if(_tmpC2 != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmpC3=({struct Cyc_List_List*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->hd=_tmpC2;_tmpC4->tl=0;_tmpC4;});
*_tmpBD=_tmpC3;
_tmpBD=& _tmpC3->tl;};}else{
# 376
*res=0;}
# 378
f0s=f0s->tl;}}
# 383
if(f1s != 0){
*incl=0;
*_tmpBD=f1s;}else{
# 387
*_tmpBD=f0s;}
# 390
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 394
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0)return({struct _tuple16 _tmpC6;_tmpC6.f1=0;_tmpC6.f2=0;_tmpC6;});
if(f->tl == 0)return({struct _tuple16 _tmpC7;_tmpC7.f1=f;_tmpC7.f2=0;_tmpC7;});{
struct _tuple16 _tmpC8=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);struct _tuple16 _tmpC9=_tmpC8;struct Cyc_List_List*_tmpCE;struct Cyc_List_List*_tmpCD;_LL39: _tmpCE=_tmpC9.f1;_tmpCD=_tmpC9.f2;_LL3A:;
return({struct _tuple16 _tmpCA;({struct Cyc_List_List*_tmp1F3=({struct Cyc_List_List*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->hd=f->hd;_tmpCC->tl=_tmpCE;_tmpCC;});_tmpCA.f1=_tmp1F3;});({struct Cyc_List_List*_tmp1F4=({struct Cyc_List_List*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->hd=((struct Cyc_List_List*)_check_null(f->tl))->hd;_tmpCB->tl=_tmpCD;_tmpCB;});_tmpCA.f2=_tmp1F4;});_tmpCA;});};}
# 403
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 406
struct _tuple16 _tmpCF=((struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);struct _tuple16 _tmpD0=_tmpCF;struct Cyc_List_List*_tmpD4;struct Cyc_List_List*_tmpD3;_LL3C: _tmpD4=_tmpD0.f1;_tmpD3=_tmpD0.f2;_LL3D:;
if(_tmpD4 != 0  && _tmpD4->tl != 0)
({struct Cyc_List_List*_tmp1F5=Cyc_Tcdecl_sort_xdatatype_fields(_tmpD4,res,v,loc,msg);_tmpD4=_tmp1F5;});
if(_tmpD3 != 0  && _tmpD3->tl != 0)
({struct Cyc_List_List*_tmp1F6=Cyc_Tcdecl_sort_xdatatype_fields(_tmpD3,res,v,loc,msg);_tmpD3=_tmp1F6;});
return({struct Cyc_List_List*_tmp1FD=_tmpD4;struct Cyc_List_List*_tmp1FC=_tmpD3;int*_tmp1FB=res;int*_tmp1FA=({int*_tmpD1=_cycalloc_atomic(sizeof(*_tmpD1));_tmpD1[0]=1;_tmpD1;});struct _dyneither_ptr _tmp1F9=({const char*_tmpD2="@extensible datatype";_tag_dyneither(_tmpD2,sizeof(char),21);});struct _dyneither_ptr _tmp1F8=*v;unsigned int _tmp1F7=loc;Cyc_Tcdecl_merge_xdatatype_fields(_tmp1FD,_tmp1FC,0,0,0,_tmp1FB,_tmp1FA,_tmp1F9,_tmp1F8,_tmp1F7,msg);});}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 415
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 417
struct _dyneither_ptr _tmpD5=Cyc_Absynpp_qvar2string(d0->name);
struct _dyneither_ptr t=({const char*_tmpF5="datatype";_tag_dyneither(_tmpF5,sizeof(char),9);});
int _tmpD6=1;
# 424
if(d0->is_extensible != d1->is_extensible){
({struct Cyc_String_pa_PrintArg_struct _tmpDB;_tmpDB.tag=0;({struct _dyneither_ptr _tmp1FE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcdecl_is_x2string(d1->is_extensible));_tmpDB.f1=_tmp1FE;});({struct Cyc_String_pa_PrintArg_struct _tmpDA;_tmpDA.tag=0;({struct _dyneither_ptr _tmp1FF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible));_tmpDA.f1=_tmp1FF;});({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5);({void*_tmpD7[3]={& _tmpD9,& _tmpDA,& _tmpDB};({unsigned int _tmp202=loc;struct _dyneither_ptr*_tmp201=msg;struct _dyneither_ptr _tmp200=({const char*_tmpD8="expected %s to be a %s instead of a %s";_tag_dyneither(_tmpD8,sizeof(char),39);});Cyc_Tcdecl_merr(_tmp202,_tmp201,_tmp200,_tag_dyneither(_tmpD7,sizeof(void*),3));});});});});});
_tmpD6=0;}else{
# 429
({struct _dyneither_ptr _tmp203=Cyc_Tcdecl_is_x2string(d0->is_extensible);t=_tmp203;});}
# 433
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpD5,loc,msg))return 0;{
# 436
struct _tuple10 _tmpDC=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpD5,loc,msg,0);struct _tuple10 _tmpDD=_tmpDC;enum Cyc_Absyn_Scope _tmpF4;int _tmpF3;_LL3F: _tmpF4=_tmpDD.f1;_tmpF3=_tmpDD.f2;_LL40:;
if(!_tmpF3)_tmpD6=0;{
# 439
struct Cyc_Absyn_Datatypedecl*d2;
# 442
{struct _tuple17 _tmpDE=({struct _tuple17 _tmpF1;_tmpF1.f1=d0->fields;_tmpF1.f2=d1->fields;_tmpF1;});struct _tuple17 _tmpDF=_tmpDE;struct Cyc_List_List*_tmpF0;struct Cyc_List_List*_tmpEF;if(_tmpDF.f2 == 0){_LL42: _LL43:
 d2=d0;goto _LL41;}else{if(_tmpDF.f1 == 0){_LL44: _LL45:
 d2=d1;goto _LL41;}else{_LL46: _tmpF0=(struct Cyc_List_List*)(_tmpDF.f1)->v;_tmpEF=(struct Cyc_List_List*)(_tmpDF.f2)->v;_LL47: {
# 456 "tcdecl.cyc"
struct Cyc_List_List*_tmpE0=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 459
if(d0->is_extensible){
# 463
int _tmpE1=1;
struct Cyc_List_List*_tmpE2=Cyc_Tcdecl_merge_xdatatype_fields(_tmpF0,_tmpEF,_tmpE0,d0->tvs,d1->tvs,& _tmpD6,& _tmpE1,t,_tmpD5,loc,msg);
# 467
if(_tmpE1)
d2=d0;else{
# 470
({struct Cyc_Absyn_Datatypedecl*_tmp204=({struct Cyc_Absyn_Datatypedecl*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3[0]=*d0;_tmpE3;});d2=_tmp204;});
d2->sc=_tmpF4;
({struct Cyc_Core_Opt*_tmp205=({struct Cyc_Core_Opt*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->v=_tmpE2;_tmpE4;});d2->fields=_tmp205;});}}else{
# 476
for(0;_tmpF0 != 0  && _tmpEF != 0;(_tmpF0=_tmpF0->tl,_tmpEF=_tmpEF->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpF0->hd,(struct Cyc_Absyn_Datatypefield*)_tmpEF->hd,_tmpE0,t,_tmpD5,msg);}
# 479
if(_tmpF0 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpE9;_tmpE9.tag=0;_tmpE9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpF0->hd)->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmpE8;_tmpE8.tag=0;_tmpE8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5);({struct Cyc_String_pa_PrintArg_struct _tmpE7;_tmpE7.tag=0;_tmpE7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpE5[3]={& _tmpE7,& _tmpE8,& _tmpE9};({unsigned int _tmp208=loc;struct _dyneither_ptr*_tmp207=msg;struct _dyneither_ptr _tmp206=({const char*_tmpE6="%s %s is missing field %s";_tag_dyneither(_tmpE6,sizeof(char),26);});Cyc_Tcdecl_merr(_tmp208,_tmp207,_tmp206,_tag_dyneither(_tmpE5,sizeof(void*),3));});});});});});
_tmpD6=0;}
# 483
if(_tmpEF != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpEE;_tmpEE.tag=0;_tmpEE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpEF->hd)->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmpED;_tmpED.tag=0;_tmpED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpD5);({struct Cyc_String_pa_PrintArg_struct _tmpEC;_tmpEC.tag=0;_tmpEC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpEA[3]={& _tmpEC,& _tmpED,& _tmpEE};({unsigned int _tmp20B=loc;struct _dyneither_ptr*_tmp20A=msg;struct _dyneither_ptr _tmp209=({const char*_tmpEB="%s %s has extra field %s";_tag_dyneither(_tmpEB,sizeof(char),25);});Cyc_Tcdecl_merr(_tmp20B,_tmp20A,_tmp209,_tag_dyneither(_tmpEA,sizeof(void*),3));});});});});});
_tmpD6=0;}
# 487
d2=d0;}
# 489
goto _LL41;}}}_LL41:;}
# 492
if(!_tmpD6)return 0;
# 494
if(_tmpF4 == d2->sc)
return d2;else{
# 497
({struct Cyc_Absyn_Datatypedecl*_tmp20C=({struct Cyc_Absyn_Datatypedecl*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=*d2;_tmpF2;});d2=_tmp20C;});
d2->sc=_tmpF4;
return d2;}};};}
# 503
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 505
struct _dyneither_ptr _tmpF6=Cyc_Absynpp_qvar2string(d0->name);
int _tmpF7=1;
# 509
struct _tuple10 _tmpF8=({enum Cyc_Absyn_Scope _tmp211=d0->sc;enum Cyc_Absyn_Scope _tmp210=d1->sc;struct _dyneither_ptr _tmp20F=({const char*_tmp115="enum";_tag_dyneither(_tmp115,sizeof(char),5);});struct _dyneither_ptr _tmp20E=_tmpF6;unsigned int _tmp20D=loc;Cyc_Tcdecl_merge_scope(_tmp211,_tmp210,_tmp20F,_tmp20E,_tmp20D,msg,1);});struct _tuple10 _tmpF9=_tmpF8;enum Cyc_Absyn_Scope _tmp114;int _tmp113;_LL49: _tmp114=_tmpF9.f1;_tmp113=_tmpF9.f2;_LL4A:;
if(!_tmp113)_tmpF7=0;{
# 512
struct Cyc_Absyn_Enumdecl*d2;
# 515
{struct _tuple17 _tmpFA=({struct _tuple17 _tmp111;_tmp111.f1=d0->fields;_tmp111.f2=d1->fields;_tmp111;});struct _tuple17 _tmpFB=_tmpFA;struct Cyc_List_List*_tmp110;struct Cyc_List_List*_tmp10F;if(_tmpFB.f2 == 0){_LL4C: _LL4D:
 d2=d0;goto _LL4B;}else{if(_tmpFB.f1 == 0){_LL4E: _LL4F:
 d2=d1;goto _LL4B;}else{_LL50: _tmp110=(struct Cyc_List_List*)(_tmpFB.f1)->v;_tmp10F=(struct Cyc_List_List*)(_tmpFB.f2)->v;_LL51:
# 520
 for(0;_tmp110 != 0  && _tmp10F != 0;(_tmp110=_tmp110->tl,_tmp10F=_tmp10F->tl)){
struct Cyc_Absyn_Enumfield*_tmpFC=(struct Cyc_Absyn_Enumfield*)_tmp110->hd;struct Cyc_Absyn_Enumfield*_tmpFD=_tmpFC;struct _dyneither_ptr*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;unsigned int _tmp10C;_LL53: _tmp10E=(_tmpFD->name)->f2;_tmp10D=_tmpFD->tag;_tmp10C=_tmpFD->loc;_LL54:;{
struct Cyc_Absyn_Enumfield*_tmpFE=(struct Cyc_Absyn_Enumfield*)_tmp10F->hd;struct Cyc_Absyn_Enumfield*_tmpFF=_tmpFE;struct _dyneither_ptr*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;unsigned int _tmp109;_LL56: _tmp10B=(_tmpFF->name)->f2;_tmp10A=_tmpFF->tag;_tmp109=_tmpFF->loc;_LL57:;
# 525
if(({Cyc_strptrcmp(_tmp10E,_tmp10B)!= 0;})){
({struct Cyc_String_pa_PrintArg_struct _tmp104;_tmp104.tag=0;_tmp104.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp10B);({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0;_tmp103.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp10E);({struct Cyc_String_pa_PrintArg_struct _tmp102;_tmp102.tag=0;_tmp102.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF6);({void*_tmp100[3]={& _tmp102,& _tmp103,& _tmp104};({unsigned int _tmp214=loc;struct _dyneither_ptr*_tmp213=msg;struct _dyneither_ptr _tmp212=({const char*_tmp101="enum %s: field name mismatch %s != %s";_tag_dyneither(_tmp101,sizeof(char),38);});Cyc_Tcdecl_merr(_tmp214,_tmp213,_tmp212,_tag_dyneither(_tmp100,sizeof(void*),3));});});});});});
_tmpF7=0;}
# 530
if(({unsigned int _tmp215=Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp10D));_tmp215 != Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmp10A));})){
({struct Cyc_String_pa_PrintArg_struct _tmp108;_tmp108.tag=0;_tmp108.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp10B);({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0;_tmp107.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpF6);({void*_tmp105[2]={& _tmp107,& _tmp108};({unsigned int _tmp218=loc;struct _dyneither_ptr*_tmp217=msg;struct _dyneither_ptr _tmp216=({const char*_tmp106="enum %s, field %s, value mismatch";_tag_dyneither(_tmp106,sizeof(char),34);});Cyc_Tcdecl_merr(_tmp218,_tmp217,_tmp216,_tag_dyneither(_tmp105,sizeof(void*),2));});});});});
_tmpF7=0;}};}
# 535
d2=d0;
goto _LL4B;}}_LL4B:;}
# 539
if(!_tmpF7)return 0;
# 541
if(d2->sc == _tmp114)
return d2;else{
# 544
({struct Cyc_Absyn_Enumdecl*_tmp219=({struct Cyc_Absyn_Enumdecl*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=*d2;_tmp112;});d2=_tmp219;});
d2->sc=_tmp114;
return d2;}};}
# 550
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 554
int _tmp116=1;
# 557
struct _tuple10 _tmp117=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);struct _tuple10 _tmp118=_tmp117;enum Cyc_Absyn_Scope _tmp13E;int _tmp13D;_LL59: _tmp13E=_tmp118.f1;_tmp13D=_tmp118.f2;_LL5A:;
if(!_tmp13D)_tmp116=0;
# 561
if(!Cyc_Tcdecl_check_type(t0,t1)){
struct _dyneither_ptr _tmp119=Cyc_Absynpp_typ2string(t0);
struct _dyneither_ptr _tmp11A=Cyc_Absynpp_typ2string(t1);
if(({({unsigned long _tmp21D=({unsigned long _tmp21C=({unsigned long _tmp21B=({unsigned long _tmp21A=Cyc_strlen((struct _dyneither_ptr)_tmp119);_tmp21A + Cyc_strlen((struct _dyneither_ptr)_tmp11A);});_tmp21B + Cyc_strlen((struct _dyneither_ptr)t);});_tmp21C + Cyc_strlen((struct _dyneither_ptr)v);});_tmp21D + 
Cyc_strlen(({const char*_tmp11B="  was declared  instead of ";_tag_dyneither(_tmp11B,sizeof(char),28);}));})> 70;}))
({struct Cyc_String_pa_PrintArg_struct _tmp121;_tmp121.tag=0;_tmp121.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp119);({struct Cyc_String_pa_PrintArg_struct _tmp120;_tmp120.tag=0;_tmp120.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11A);({struct Cyc_String_pa_PrintArg_struct _tmp11F;_tmp11F.tag=0;_tmp11F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp11E;_tmp11E.tag=0;_tmp11E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp11C[4]={& _tmp11E,& _tmp11F,& _tmp120,& _tmp121};({unsigned int _tmp220=loc;struct _dyneither_ptr*_tmp21F=msg;struct _dyneither_ptr _tmp21E=({const char*_tmp11D="%s %s was declared \n\t%s\n instead of \n\t%s";_tag_dyneither(_tmp11D,sizeof(char),41);});Cyc_Tcdecl_merr(_tmp220,_tmp21F,_tmp21E,_tag_dyneither(_tmp11C,sizeof(void*),4));});});});});});});else{
# 569
({struct Cyc_String_pa_PrintArg_struct _tmp127;_tmp127.tag=0;_tmp127.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp119);({struct Cyc_String_pa_PrintArg_struct _tmp126;_tmp126.tag=0;_tmp126.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp11A);({struct Cyc_String_pa_PrintArg_struct _tmp125;_tmp125.tag=0;_tmp125.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp124;_tmp124.tag=0;_tmp124.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp122[4]={& _tmp124,& _tmp125,& _tmp126,& _tmp127};({unsigned int _tmp223=loc;struct _dyneither_ptr*_tmp222=msg;struct _dyneither_ptr _tmp221=({const char*_tmp123="%s %s was declared %s instead of %s";_tag_dyneither(_tmp123,sizeof(char),36);});Cyc_Tcdecl_merr(_tmp223,_tmp222,_tmp221,_tag_dyneither(_tmp122,sizeof(void*),4));});});});});});});}
# 571
Cyc_Tcutil_explain_failure();
_tmp116=0;}
# 576
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
({struct Cyc_String_pa_PrintArg_struct _tmp12B;_tmp12B.tag=0;_tmp12B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp12A;_tmp12A.tag=0;_tmp12A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp128[2]={& _tmp12A,& _tmp12B};({unsigned int _tmp226=loc;struct _dyneither_ptr*_tmp225=msg;struct _dyneither_ptr _tmp224=({const char*_tmp129="%s %s has different type qualifiers";_tag_dyneither(_tmp129,sizeof(char),36);});Cyc_Tcdecl_merr(_tmp226,_tmp225,_tmp224,_tag_dyneither(_tmp128,sizeof(void*),2));});});});});
_tmp116=0;}
# 582
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp12F;_tmp12F.tag=0;_tmp12F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp12E;_tmp12E.tag=0;_tmp12E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp12C[2]={& _tmp12E,& _tmp12F};({unsigned int _tmp229=loc;struct _dyneither_ptr*_tmp228=msg;struct _dyneither_ptr _tmp227=({const char*_tmp12D="%s %s has different attributes";_tag_dyneither(_tmp12D,sizeof(char),31);});Cyc_Tcdecl_merr(_tmp229,_tmp228,_tmp227,_tag_dyneither(_tmp12C,sizeof(void*),2));});});});});
({void*_tmp130=0;({struct Cyc___cycFILE*_tmp22B=Cyc_stderr;struct _dyneither_ptr _tmp22A=({const char*_tmp131="\tprevious attributes: ";_tag_dyneither(_tmp131,sizeof(char),23);});Cyc_fprintf(_tmp22B,_tmp22A,_tag_dyneither(_tmp130,sizeof(void*),0));});});
for(0;atts0 != 0;atts0=atts0->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp134;_tmp134.tag=0;({struct _dyneither_ptr _tmp22C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd));_tmp134.f1=_tmp22C;});({void*_tmp132[1]={& _tmp134};({struct Cyc___cycFILE*_tmp22E=Cyc_stderr;struct _dyneither_ptr _tmp22D=({const char*_tmp133="%s ";_tag_dyneither(_tmp133,sizeof(char),4);});Cyc_fprintf(_tmp22E,_tmp22D,_tag_dyneither(_tmp132,sizeof(void*),1));});});});}
({void*_tmp135=0;({struct Cyc___cycFILE*_tmp230=Cyc_stderr;struct _dyneither_ptr _tmp22F=({const char*_tmp136="\n\tcurrent attributes: ";_tag_dyneither(_tmp136,sizeof(char),23);});Cyc_fprintf(_tmp230,_tmp22F,_tag_dyneither(_tmp135,sizeof(void*),0));});});
for(0;atts1 != 0;atts1=atts1->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp139;_tmp139.tag=0;({struct _dyneither_ptr _tmp231=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd));_tmp139.f1=_tmp231;});({void*_tmp137[1]={& _tmp139};({struct Cyc___cycFILE*_tmp233=Cyc_stderr;struct _dyneither_ptr _tmp232=({const char*_tmp138="%s ";_tag_dyneither(_tmp138,sizeof(char),4);});Cyc_fprintf(_tmp233,_tmp232,_tag_dyneither(_tmp137,sizeof(void*),1));});});});}
({void*_tmp13A=0;({struct Cyc___cycFILE*_tmp235=Cyc_stderr;struct _dyneither_ptr _tmp234=({const char*_tmp13B="\n";_tag_dyneither(_tmp13B,sizeof(char),2);});Cyc_fprintf(_tmp235,_tmp234,_tag_dyneither(_tmp13A,sizeof(void*),0));});});
_tmp116=0;}
# 594
return({struct _tuple10 _tmp13C;_tmp13C.f1=_tmp13E;_tmp13C.f2=_tmp116;_tmp13C;});}
# 597
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 599
struct _dyneither_ptr _tmp13F=Cyc_Absynpp_qvar2string(d0->name);
# 602
struct _tuple10 _tmp140=({
enum Cyc_Absyn_Scope _tmp240=d0->sc;void*_tmp23F=d0->type;struct Cyc_Absyn_Tqual _tmp23E=d0->tq;struct Cyc_List_List*_tmp23D=d0->attributes;enum Cyc_Absyn_Scope _tmp23C=d1->sc;void*_tmp23B=d1->type;struct Cyc_Absyn_Tqual _tmp23A=d1->tq;struct Cyc_List_List*_tmp239=d1->attributes;struct _dyneither_ptr _tmp238=({const char*_tmp146="variable";_tag_dyneither(_tmp146,sizeof(char),9);});struct _dyneither_ptr _tmp237=_tmp13F;unsigned int _tmp236=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp240,_tmp23F,_tmp23E,_tmp23D,_tmp23C,_tmp23B,_tmp23A,_tmp239,_tmp238,_tmp237,_tmp236,msg);});
# 602
struct _tuple10 _tmp141=_tmp140;enum Cyc_Absyn_Scope _tmp145;int _tmp144;_LL5C: _tmp145=_tmp141.f1;_tmp144=_tmp141.f2;_LL5D:;
# 606
if(!_tmp144)return 0;
# 608
if(d0->sc == _tmp145)
return d0;else{
# 611
struct Cyc_Absyn_Vardecl*_tmp142=({struct Cyc_Absyn_Vardecl*_tmp143=_cycalloc(sizeof(*_tmp143));_tmp143[0]=*d0;_tmp143;});
_tmp142->sc=_tmp145;
return _tmp142;}}
# 617
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 619
struct _dyneither_ptr _tmp147=Cyc_Absynpp_qvar2string(d0->name);
# 622
if(!({struct Cyc_List_List*_tmp245=d0->tvs;struct Cyc_List_List*_tmp244=d1->tvs;struct _dyneither_ptr _tmp243=({const char*_tmp148="typedef";_tag_dyneither(_tmp148,sizeof(char),8);});struct _dyneither_ptr _tmp242=_tmp147;unsigned int _tmp241=loc;Cyc_Tcdecl_check_tvs(_tmp245,_tmp244,_tmp243,_tmp242,_tmp241,msg);}))return 0;{
# 625
struct Cyc_List_List*_tmp149=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 628
if(d0->defn != 0  && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(_tmp149,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0;({struct _dyneither_ptr _tmp246=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(d0->defn)));_tmp14E.f1=_tmp246;});({struct Cyc_String_pa_PrintArg_struct _tmp14D;_tmp14D.tag=0;({struct _dyneither_ptr _tmp247=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1));_tmp14D.f1=_tmp247;});({struct Cyc_String_pa_PrintArg_struct _tmp14C;_tmp14C.tag=0;_tmp14C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp147);({void*_tmp14A[3]={& _tmp14C,& _tmp14D,& _tmp14E};({unsigned int _tmp24A=loc;struct _dyneither_ptr*_tmp249=msg;struct _dyneither_ptr _tmp248=({const char*_tmp14B="typedef %s does not refer to the same type: %s != %s";_tag_dyneither(_tmp14B,sizeof(char),53);});Cyc_Tcdecl_merr(_tmp24A,_tmp249,_tmp248,_tag_dyneither(_tmp14A,sizeof(void*),3));});});});});});
return 0;}}
# 636
return d0;};}
# 639
void**Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct _dyneither_ptr*msg){
struct _tuple14 _tmp14F=({struct _tuple14 _tmp16B;_tmp16B.f1=b0;_tmp16B.f2=b1;_tmp16B;});struct _tuple14 _tmp150=_tmp14F;struct Cyc_Absyn_Fndecl*_tmp16A;struct Cyc_Absyn_Vardecl*_tmp169;struct Cyc_Absyn_Fndecl*_tmp168;struct Cyc_Absyn_Vardecl*_tmp167;struct Cyc_Absyn_Fndecl*_tmp166;struct Cyc_Absyn_Vardecl*_tmp165;struct Cyc_Absyn_Vardecl*_tmp164;switch(*((int*)_tmp150.f1)){case 1: switch(*((int*)_tmp150.f2)){case 1: _LL5F: _tmp165=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp150.f1)->f1;_tmp164=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp150.f2)->f1;_LL60: {
# 642
struct Cyc_Absyn_Vardecl*_tmp151=Cyc_Tcdecl_merge_vardecl(_tmp165,_tmp164,loc,msg);
if(_tmp151 == 0)return 0;
if(_tmp151 == _tmp165)return({void**_tmp152=_cycalloc(sizeof(*_tmp152));_tmp152[0]=b0;_tmp152;});
if(_tmp151 == _tmp164)return({void**_tmp153=_cycalloc(sizeof(*_tmp153));_tmp153[0]=b1;_tmp153;});
return({void**_tmp154=_cycalloc(sizeof(*_tmp154));({void*_tmp24C=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp155=_cycalloc(sizeof(*_tmp155));({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp24B=({struct Cyc_Absyn_Global_b_Absyn_Binding_struct _tmp156;_tmp156.tag=1;_tmp156.f1=_tmp151;_tmp156;});_tmp155[0]=_tmp24B;});_tmp155;});_tmp154[0]=_tmp24C;});_tmp154;});}case 2: _LL61: _tmp167=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp150.f1)->f1;_tmp166=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp150.f2)->f1;_LL62: {
# 648
struct _tuple10 _tmp157=({enum Cyc_Absyn_Scope _tmp257=_tmp167->sc;void*_tmp256=_tmp167->type;struct Cyc_Absyn_Tqual _tmp255=_tmp167->tq;struct Cyc_List_List*_tmp254=_tmp167->attributes;enum Cyc_Absyn_Scope _tmp253=_tmp166->sc;void*_tmp252=(void*)_check_null(_tmp166->cached_typ);struct Cyc_Absyn_Tqual _tmp251=
Cyc_Absyn_empty_tqual(0);
# 648
struct Cyc_List_List*_tmp250=_tmp166->attributes;struct _dyneither_ptr _tmp24F=({const char*_tmp15B="function";_tag_dyneither(_tmp15B,sizeof(char),9);});struct _dyneither_ptr _tmp24E=
# 651
Cyc_Absynpp_qvar2string(_tmp167->name);
# 648
unsigned int _tmp24D=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp257,_tmp256,_tmp255,_tmp254,_tmp253,_tmp252,_tmp251,_tmp250,_tmp24F,_tmp24E,_tmp24D,msg);});struct _tuple10 _tmp158=_tmp157;int _tmp15A;_LL6A: _tmp15A=_tmp158.f2;_LL6B:;
# 652
return _tmp15A?({void**_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159[0]=b1;_tmp159;}): 0;}default: goto _LL67;}case 2: switch(*((int*)_tmp150.f2)){case 2: _LL63: _tmp168=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp150.f2)->f1;_LL64:
# 654
({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0;({struct _dyneither_ptr _tmp258=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp168->name));_tmp15E.f1=_tmp258;});({void*_tmp15C[1]={& _tmp15E};({unsigned int _tmp25B=loc;struct _dyneither_ptr*_tmp25A=msg;struct _dyneither_ptr _tmp259=({const char*_tmp15D="redefinition of function %s";_tag_dyneither(_tmp15D,sizeof(char),28);});Cyc_Tcdecl_merr(_tmp25B,_tmp25A,_tmp259,_tag_dyneither(_tmp15C,sizeof(void*),1));});});});
return 0;case 1: _LL65: _tmp16A=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp150.f1)->f1;_tmp169=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp150.f2)->f1;_LL66: {
# 657
struct _tuple10 _tmp15F=({enum Cyc_Absyn_Scope _tmp266=_tmp16A->sc;void*_tmp265=(void*)_check_null(_tmp16A->cached_typ);struct Cyc_Absyn_Tqual _tmp264=Cyc_Absyn_empty_tqual(0);struct Cyc_List_List*_tmp263=_tmp16A->attributes;enum Cyc_Absyn_Scope _tmp262=_tmp169->sc;void*_tmp261=_tmp169->type;struct Cyc_Absyn_Tqual _tmp260=_tmp169->tq;struct Cyc_List_List*_tmp25F=_tmp169->attributes;struct _dyneither_ptr _tmp25E=({const char*_tmp163="variable";_tag_dyneither(_tmp163,sizeof(char),9);});struct _dyneither_ptr _tmp25D=
# 660
Cyc_Absynpp_qvar2string(_tmp16A->name);
# 657
unsigned int _tmp25C=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp266,_tmp265,_tmp264,_tmp263,_tmp262,_tmp261,_tmp260,_tmp25F,_tmp25E,_tmp25D,_tmp25C,msg);});struct _tuple10 _tmp160=_tmp15F;int _tmp162;_LL6D: _tmp162=_tmp160.f2;_LL6E:;
# 661
return _tmp162?({void**_tmp161=_cycalloc(sizeof(*_tmp161));_tmp161[0]=b0;_tmp161;}): 0;}default: goto _LL67;}default: _LL67: _LL68:
 return 0;}_LL5E:;}
# 666
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 669
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp16C=d0;struct Cyc_Absyn_Datatypedecl*_tmp17C;struct Cyc_Absyn_Datatypefield*_tmp17B;_LL70: _tmp17C=_tmp16C->base;_tmp17B=_tmp16C->field;_LL71:;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp16D=d1;struct Cyc_Absyn_Datatypedecl*_tmp17A;struct Cyc_Absyn_Datatypefield*_tmp179;_LL73: _tmp17A=_tmp16D->base;_tmp179=_tmp16D->field;_LL74:;{
struct _dyneither_ptr _tmp16E=Cyc_Absynpp_qvar2string(_tmp17B->name);
# 674
if(({Cyc_Absyn_qvar_cmp(_tmp17C->name,_tmp17A->name)!= 0;})){
({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0;_tmp173.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp16E);({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0;({struct _dyneither_ptr _tmp267=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp17A->name));_tmp172.f1=_tmp267;});({struct Cyc_String_pa_PrintArg_struct _tmp171;_tmp171.tag=0;({struct _dyneither_ptr _tmp268=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp17C->name));_tmp171.f1=_tmp268;});({void*_tmp16F[3]={& _tmp171,& _tmp172,& _tmp173};({unsigned int _tmp26B=loc;struct _dyneither_ptr*_tmp26A=msg;struct _dyneither_ptr _tmp269=({const char*_tmp170="@extensible datatype %s and %s have both a field named %s";_tag_dyneither(_tmp170,sizeof(char),58);});Cyc_Tcdecl_merr(_tmp26B,_tmp26A,_tmp269,_tag_dyneither(_tmp16F,sizeof(void*),3));});});});});});
return 0;}
# 681
if(!({struct Cyc_List_List*_tmp270=_tmp17C->tvs;struct Cyc_List_List*_tmp26F=_tmp17A->tvs;struct _dyneither_ptr _tmp26E=({const char*_tmp174="@extensible datatype";_tag_dyneither(_tmp174,sizeof(char),21);});struct _dyneither_ptr _tmp26D=Cyc_Absynpp_qvar2string(_tmp17C->name);unsigned int _tmp26C=loc;Cyc_Tcdecl_check_tvs(_tmp270,_tmp26F,_tmp26E,_tmp26D,_tmp26C,msg);}))
return 0;{
# 684
struct Cyc_List_List*_tmp175=Cyc_Tcdecl_build_tvs_map(_tmp17C->tvs,_tmp17A->tvs);
# 687
struct Cyc_Absyn_Datatypefield*_tmp176=({struct Cyc_Absyn_Datatypefield*_tmp275=_tmp17B;struct Cyc_Absyn_Datatypefield*_tmp274=_tmp179;struct Cyc_List_List*_tmp273=_tmp175;struct _dyneither_ptr _tmp272=({const char*_tmp178="@extensible datatype field";_tag_dyneither(_tmp178,sizeof(char),27);});struct _dyneither_ptr _tmp271=_tmp16E;Cyc_Tcdecl_merge_datatypefield(_tmp275,_tmp274,_tmp273,_tmp272,_tmp271,msg);});
if(_tmp176 == 0)return 0;
if(_tmp176 == _tmp17B)return d0;
return({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->base=_tmp17C;_tmp177->field=_tmp176;_tmp177;});};};};}
