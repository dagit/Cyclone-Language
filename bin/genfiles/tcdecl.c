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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Position_Error;
# 43 "position.h"
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 49
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 945
struct Cyc_Absyn_Kind*Cyc_Absyn_force_kb(void*kb);
# 1161
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);extern char Cyc_Tcdecl_Incompatible[13U];struct Cyc_Tcdecl_Incompatible_exn_struct{char*tag;};struct Cyc_Tcdecl_Xdatatypefielddecl{struct Cyc_Absyn_Datatypedecl*base;struct Cyc_Absyn_Datatypefield*field;};
# 38 "tcdecl.h"
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
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 50 "string.h"
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 132 "tcutil.h"
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 145
void Cyc_Tcutil_explain_failure();
# 147
int Cyc_Tcutil_unify(void*,void*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 239 "tcutil.h"
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 306
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _dyneither_ptr Cyc_Absynpp_scope2string(enum Cyc_Absyn_Scope sc);char Cyc_Tcdecl_Incompatible[13U]="Incompatible";
# 36 "tcdecl.cyc"
struct Cyc_Tcdecl_Incompatible_exn_struct Cyc_Tcdecl_Incompatible_val={Cyc_Tcdecl_Incompatible};
# 38
void Cyc_Tcdecl_merr(unsigned int loc,struct _dyneither_ptr*msg1,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 42
if(msg1 == 0)
(int)_throw((void*)& Cyc_Tcdecl_Incompatible_val);{
struct _dyneither_ptr fmt2;
if(Cyc_strlen((struct _dyneither_ptr)*msg1)== 0)
({struct _dyneither_ptr _tmp176=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp175=({const char*_tmp0="%s";_tag_dyneither(_tmp0,sizeof(char),3U);});Cyc_strconcat(_tmp175,(struct _dyneither_ptr)fmt);});fmt2=_tmp176;});else{
# 48
({struct _dyneither_ptr _tmp178=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp177=({const char*_tmp1="%s ";_tag_dyneither(_tmp1,sizeof(char),4U);});Cyc_strconcat(_tmp177,(struct _dyneither_ptr)fmt);});fmt2=_tmp178;});}{
struct _dyneither_ptr ap2=({unsigned int _tmp4=_get_dyneither_size(ap,sizeof(void*))+ 1;void**_tmp3=_cycalloc(_check_times(_tmp4,sizeof(void*)));({{unsigned int _tmp168=_get_dyneither_size(ap,sizeof(void*))+ 1;unsigned int i;for(i=0;i < _tmp168;++ i){
i == 0?({void*_tmp179=(void*)({struct Cyc_String_pa_PrintArg_struct*_tmp2=_cycalloc(sizeof(*_tmp2));((*_tmp2).tag=0U,(*_tmp2).f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*msg1));_tmp2;});_tmp3[i]=_tmp179;}):(_tmp3[i]=*((void**)_check_dyneither_subscript(ap,sizeof(void*),(int)(i - 1))));}}0;});_tag_dyneither(_tmp3,sizeof(void*),_tmp4);});
Cyc_Position_post_error(({unsigned int _tmp17A=loc;Cyc_Position_mk_err(_tmp17A,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt2,ap2));}));};};}
# 56
static void Cyc_Tcdecl_merge_scope_err(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 58
struct _dyneither_ptr s0_str=s0 == Cyc_Absyn_Public?({const char*_tmpC="public ";_tag_dyneither(_tmpC,sizeof(char),8U);}): Cyc_Absynpp_scope2string(s0);
struct _dyneither_ptr s1_str=s1 == Cyc_Absyn_Public?({const char*_tmpB="public ";_tag_dyneither(_tmpB,sizeof(char),8U);}): Cyc_Absynpp_scope2string(s1);
({struct Cyc_String_pa_PrintArg_struct _tmpA;_tmpA.tag=0U;_tmpA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0_str);({struct Cyc_String_pa_PrintArg_struct _tmp9;_tmp9.tag=0U;_tmp9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1_str);({struct Cyc_String_pa_PrintArg_struct _tmp8;_tmp8.tag=0U;_tmp8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp7;_tmp7.tag=0U;_tmp7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp5[4U];(((((_tmp5[0]=& _tmp7,_tmp5[1]=& _tmp8)),_tmp5[2]=& _tmp9)),_tmp5[3]=& _tmpA);({unsigned int _tmp17D=loc;struct _dyneither_ptr*_tmp17C=msg;struct _dyneither_ptr _tmp17B=({const char*_tmp6="%s %s is %sbut expected scope is %s";_tag_dyneither(_tmp6,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp17D,_tmp17C,_tmp17B,_tag_dyneither(_tmp5,sizeof(void*),4U));});});});});});});}struct _tuple11{enum Cyc_Absyn_Scope f1;enum Cyc_Absyn_Scope f2;};
# 64
struct _tuple10 Cyc_Tcdecl_merge_scope(enum Cyc_Absyn_Scope s0,enum Cyc_Absyn_Scope s1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg,int externCmerge){
# 66
{struct _tuple11 _tmpD=({struct _tuple11 _tmp16B;(_tmp16B.f1=s0,_tmp16B.f2=s1);_tmp16B;});struct _tuple11 _tmpE=_tmpD;switch(_tmpE.f1){case Cyc_Absyn_ExternC: switch(_tmpE.f2){case Cyc_Absyn_ExternC: _LL1: _LL2:
# 68
 goto _LL0;case Cyc_Absyn_Public: _LL3: _LL4:
# 70
 goto _LL6;case Cyc_Absyn_Extern: _LL5: _LL6:
 goto _LL8;default: _LLB: _LLC:
# 75
 goto _LLE;}case Cyc_Absyn_Public: switch(_tmpE.f2){case Cyc_Absyn_ExternC: _LL7: _LL8:
# 72
 goto _LLA;case Cyc_Absyn_Extern: goto _LLF;case Cyc_Absyn_Public: _LL15: _LL16:
# 82
 goto _LL18;default: goto _LL1B;}case Cyc_Absyn_Extern: switch(_tmpE.f2){case Cyc_Absyn_ExternC: _LL9: _LLA:
# 73
 if(externCmerge)goto _LL0;
goto _LLC;case Cyc_Absyn_Extern: goto _LLF;default: _LL11: _LL12:
# 79
 goto _LL0;}default: switch(_tmpE.f2){case Cyc_Absyn_ExternC: _LLD: _LLE:
# 76
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple10 _tmp169;(_tmp169.f1=s1,_tmp169.f2=0);_tmp169;});case Cyc_Absyn_Extern: _LLF: _LL10:
# 78
 s1=s0;goto _LL0;default: switch(_tmpE.f1){case Cyc_Absyn_Static: if(_tmpE.f2 == Cyc_Absyn_Static){_LL13: _LL14:
# 81
 goto _LL16;}else{goto _LL1B;}case Cyc_Absyn_Abstract: if(_tmpE.f2 == Cyc_Absyn_Abstract){_LL17: _LL18:
# 83
 goto _LL0;}else{goto _LL1B;}default: if(_tmpE.f2 == Cyc_Absyn_Register){_LL19: _LL1A:
 goto _LL0;}else{_LL1B: _LL1C:
 Cyc_Tcdecl_merge_scope_err(s0,s1,t,v,loc,msg);return({struct _tuple10 _tmp16A;(_tmp16A.f1=s1,_tmp16A.f2=0);_tmp16A;});}}}}_LL0:;}
# 87
return({struct _tuple10 _tmp16C;(_tmp16C.f1=s1,_tmp16C.f2=1);_tmp16C;});}
# 90
static int Cyc_Tcdecl_check_type(void*t0,void*t1){
# 92
return Cyc_Tcutil_unify(t0,t1);}
# 96
static unsigned int Cyc_Tcdecl_get_uint_const_value(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_tmp10=_tmpF;int _tmp13;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Int_c).tag == 5){_LL1: _tmp13=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp10)->f1).Int_c).val).f2;_LL2:
 return(unsigned int)_tmp13;}else{goto _LL3;}}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp12=_cycalloc(sizeof(*_tmp12));((*_tmp12).tag=Cyc_Core_Invalid_argument,({struct _dyneither_ptr _tmp17E=({const char*_tmp11="Tcdecl::get_uint_const_value";_tag_dyneither(_tmp11,sizeof(char),29U);});(*_tmp12).f1=_tmp17E;}));_tmp12;}));}_LL0:;}
# 103
inline static int Cyc_Tcdecl_check_tvs(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 106
if(({int _tmp17F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs0);_tmp17F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp17;_tmp17.tag=0U;_tmp17.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp16;_tmp16.tag=0U;_tmp16.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp14[2U];(_tmp14[0]=& _tmp16,_tmp14[1]=& _tmp17);({unsigned int _tmp182=loc;struct _dyneither_ptr*_tmp181=msg;struct _dyneither_ptr _tmp180=({const char*_tmp15="%s %s has a different number of type parameters";_tag_dyneither(_tmp15,sizeof(char),48U);});Cyc_Tcdecl_merr(_tmp182,_tmp181,_tmp180,_tag_dyneither(_tmp14,sizeof(void*),2U));});});});});
return 0;}{
# 110
struct Cyc_List_List*_tmp18=tvs0;
struct Cyc_List_List*_tmp19=tvs1;
for(0;_tmp18 != 0;(_tmp18=_tmp18->tl,_tmp19=_tmp19->tl)){
Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp18->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp19))->hd)->kind);{
struct Cyc_Absyn_Kind*_tmp1A=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp18->hd)->kind);
struct Cyc_Absyn_Kind*_tmp1B=Cyc_Absyn_force_kb(((struct Cyc_Absyn_Tvar*)_tmp19->hd)->kind);
if(_tmp1A == _tmp1B)continue;
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0U;({struct _dyneither_ptr _tmp183=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp1B));_tmp22.f1=_tmp183;});({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0U;_tmp21.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp18->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp20;_tmp20.tag=0U;({struct _dyneither_ptr _tmp184=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp1A));_tmp20.f1=_tmp184;});({struct Cyc_String_pa_PrintArg_struct _tmp1F;_tmp1F.tag=0U;_tmp1F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp1E;_tmp1E.tag=0U;_tmp1E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp1C[5U];(((((((_tmp1C[0]=& _tmp1E,_tmp1C[1]=& _tmp1F)),_tmp1C[2]=& _tmp20)),_tmp1C[3]=& _tmp21)),_tmp1C[4]=& _tmp22);({unsigned int _tmp187=loc;struct _dyneither_ptr*_tmp186=msg;struct _dyneither_ptr _tmp185=({const char*_tmp1D="%s %s has a different kind (%s) for type parameter %s (%s)";_tag_dyneither(_tmp1D,sizeof(char),59U);});Cyc_Tcdecl_merr(_tmp187,_tmp186,_tmp185,_tag_dyneither(_tmp1C,sizeof(void*),5U));});});});});});});});
return 0;};}
# 121
return 1;};}
# 124
static int Cyc_Tcdecl_check_atts(struct Cyc_List_List*atts0,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 126
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp26;_tmp26.tag=0U;_tmp26.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp25;_tmp25.tag=0U;_tmp25.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp23[2U];(_tmp23[0]=& _tmp25,_tmp23[1]=& _tmp26);({unsigned int _tmp18A=loc;struct _dyneither_ptr*_tmp189=msg;struct _dyneither_ptr _tmp188=({const char*_tmp24="%s %s has different attributes";_tag_dyneither(_tmp24,sizeof(char),31U);});Cyc_Tcdecl_merr(_tmp18A,_tmp189,_tmp188,_tag_dyneither(_tmp23,sizeof(void*),2U));});});});});
return 0;}
# 130
return 1;}struct _tuple12{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 133
static struct Cyc_List_List*Cyc_Tcdecl_build_tvs_map(struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1){
# 135
struct Cyc_List_List*inst=0;
for(0;tvs0 != 0;(tvs0=tvs0->tl,tvs1=tvs1->tl)){
({struct Cyc_List_List*_tmp18D=({struct Cyc_List_List*_tmp29=_cycalloc(sizeof(*_tmp29));(({struct _tuple12*_tmp18C=({struct _tuple12*_tmp28=_cycalloc(sizeof(*_tmp28));((*_tmp28).f1=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs1))->hd,({void*_tmp18B=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp27=_cycalloc(sizeof(*_tmp27));((*_tmp27).tag=2U,(*_tmp27).f1=(struct Cyc_Absyn_Tvar*)tvs0->hd);_tmp27;});(*_tmp28).f2=_tmp18B;}));_tmp28;});(*_tmp29).hd=_tmp18C;}),(*_tmp29).tl=inst);_tmp29;});inst=_tmp18D;});}
return inst;}struct _tuple13{struct Cyc_Absyn_AggrdeclImpl*f1;struct Cyc_Absyn_AggrdeclImpl*f2;};struct _tuple14{void*f1;void*f2;};
# 141
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*d0,struct Cyc_Absyn_Aggrdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 143
struct _dyneither_ptr _tmp2A=Cyc_Absynpp_qvar2string(d0->name);
int _tmp2B=1;
# 149
if(!(d0->kind == d1->kind))return 0;
# 152
if(!({struct Cyc_List_List*_tmp192=d0->tvs;struct Cyc_List_List*_tmp191=d1->tvs;struct _dyneither_ptr _tmp190=({const char*_tmp2C="type";_tag_dyneither(_tmp2C,sizeof(char),5U);});struct _dyneither_ptr _tmp18F=_tmp2A;unsigned int _tmp18E=loc;Cyc_Tcdecl_check_tvs(_tmp192,_tmp191,_tmp190,_tmp18F,_tmp18E,msg);}))return 0;{
# 155
struct _tuple10 _tmp2D=({
enum Cyc_Absyn_Scope _tmp197=d0->sc;enum Cyc_Absyn_Scope _tmp196=d1->sc;struct _dyneither_ptr _tmp195=({const char*_tmp83="type";_tag_dyneither(_tmp83,sizeof(char),5U);});struct _dyneither_ptr _tmp194=_tmp2A;unsigned int _tmp193=loc;Cyc_Tcdecl_merge_scope(_tmp197,_tmp196,_tmp195,_tmp194,_tmp193,msg,1);});
# 155
struct _tuple10 _tmp2E=_tmp2D;enum Cyc_Absyn_Scope _tmp82;int _tmp81;_LL1: _tmp82=_tmp2E.f1;_tmp81=_tmp2E.f2;_LL2:;
# 157
if(!_tmp81)_tmp2B=0;
# 160
if(!({struct Cyc_List_List*_tmp19C=d0->attributes;struct Cyc_List_List*_tmp19B=d1->attributes;struct _dyneither_ptr _tmp19A=({const char*_tmp2F="type";_tag_dyneither(_tmp2F,sizeof(char),5U);});struct _dyneither_ptr _tmp199=_tmp2A;unsigned int _tmp198=loc;Cyc_Tcdecl_check_atts(_tmp19C,_tmp19B,_tmp19A,_tmp199,_tmp198,msg);}))_tmp2B=0;{
# 162
struct Cyc_Absyn_Aggrdecl*d2;
# 164
{struct _tuple13 _tmp30=({struct _tuple13 _tmp16D;(_tmp16D.f1=d0->impl,_tmp16D.f2=d1->impl);_tmp16D;});struct _tuple13 _tmp31=_tmp30;struct Cyc_List_List*_tmp7F;struct Cyc_List_List*_tmp7E;struct Cyc_List_List*_tmp7D;int _tmp7C;struct Cyc_List_List*_tmp7B;struct Cyc_List_List*_tmp7A;struct Cyc_List_List*_tmp79;int _tmp78;if(_tmp31.f2 == 0){_LL4: _LL5:
 d2=d0;goto _LL3;}else{if(_tmp31.f1 == 0){_LL6: _LL7:
 d2=d1;goto _LL3;}else{_LL8: _tmp7F=(_tmp31.f1)->exist_vars;_tmp7E=(_tmp31.f1)->rgn_po;_tmp7D=(_tmp31.f1)->fields;_tmp7C=(_tmp31.f1)->tagged;_tmp7B=(_tmp31.f2)->exist_vars;_tmp7A=(_tmp31.f2)->rgn_po;_tmp79=(_tmp31.f2)->fields;_tmp78=(_tmp31.f2)->tagged;_LL9:
# 171
 if(!({struct Cyc_List_List*_tmp1A1=_tmp7F;struct Cyc_List_List*_tmp1A0=_tmp7B;struct _dyneither_ptr _tmp19F=({const char*_tmp32="type";_tag_dyneither(_tmp32,sizeof(char),5U);});struct _dyneither_ptr _tmp19E=_tmp2A;unsigned int _tmp19D=loc;Cyc_Tcdecl_check_tvs(_tmp1A1,_tmp1A0,_tmp19F,_tmp19E,_tmp19D,msg);}))
return 0;{
# 175
struct Cyc_List_List*_tmp33=({struct Cyc_List_List*_tmp1A2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d0->tvs,_tmp7F);Cyc_Tcdecl_build_tvs_map(_tmp1A2,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(d1->tvs,_tmp7B));});
# 180
for(0;_tmp7E != 0  && _tmp7A != 0;
(_tmp7E=_tmp7E->tl,_tmp7A=_tmp7A->tl)){
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp7E->hd)).f1,(*((struct _tuple14*)_tmp7A->hd)).f1);
Cyc_Tcdecl_check_type((*((struct _tuple14*)_tmp7E->hd)).f2,(*((struct _tuple14*)_tmp7A->hd)).f2);}
# 187
for(0;_tmp7D != 0  && _tmp79 != 0;(_tmp7D=_tmp7D->tl,_tmp79=_tmp79->tl)){
struct Cyc_Absyn_Aggrfield*_tmp34=(struct Cyc_Absyn_Aggrfield*)_tmp7D->hd;struct Cyc_Absyn_Aggrfield*_tmp35=_tmp34;struct _dyneither_ptr*_tmp6C;struct Cyc_Absyn_Tqual _tmp6B;void*_tmp6A;struct Cyc_Absyn_Exp*_tmp69;struct Cyc_List_List*_tmp68;struct Cyc_Absyn_Exp*_tmp67;_LLB: _tmp6C=_tmp35->name;_tmp6B=_tmp35->tq;_tmp6A=_tmp35->type;_tmp69=_tmp35->width;_tmp68=_tmp35->attributes;_tmp67=_tmp35->requires_clause;_LLC:;{
struct Cyc_Absyn_Aggrfield*_tmp36=(struct Cyc_Absyn_Aggrfield*)_tmp79->hd;struct Cyc_Absyn_Aggrfield*_tmp37=_tmp36;struct _dyneither_ptr*_tmp66;struct Cyc_Absyn_Tqual _tmp65;void*_tmp64;struct Cyc_Absyn_Exp*_tmp63;struct Cyc_List_List*_tmp62;struct Cyc_Absyn_Exp*_tmp61;_LLE: _tmp66=_tmp37->name;_tmp65=_tmp37->tq;_tmp64=_tmp37->type;_tmp63=_tmp37->width;_tmp62=_tmp37->attributes;_tmp61=_tmp37->requires_clause;_LLF:;
# 191
if(Cyc_strptrcmp(_tmp6C,_tmp66)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3D;_tmp3D.tag=0U;_tmp3D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp66);({struct Cyc_String_pa_PrintArg_struct _tmp3C;_tmp3C.tag=0U;_tmp3C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);({struct Cyc_String_pa_PrintArg_struct _tmp3B;_tmp3B.tag=0U;_tmp3B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({struct Cyc_String_pa_PrintArg_struct _tmp3A;_tmp3A.tag=0U;({struct _dyneither_ptr _tmp1A3=(struct _dyneither_ptr)({const char*_tmp3E="type";_tag_dyneither(_tmp3E,sizeof(char),5U);});_tmp3A.f1=_tmp1A3;});({void*_tmp38[4U];(((((_tmp38[0]=& _tmp3A,_tmp38[1]=& _tmp3B)),_tmp38[2]=& _tmp3C)),_tmp38[3]=& _tmp3D);({unsigned int _tmp1A6=loc;struct _dyneither_ptr*_tmp1A5=msg;struct _dyneither_ptr _tmp1A4=({const char*_tmp39="%s %s : field name mismatch %s != %s";_tag_dyneither(_tmp39,sizeof(char),37U);});Cyc_Tcdecl_merr(_tmp1A6,_tmp1A5,_tmp1A4,_tag_dyneither(_tmp38,sizeof(void*),4U));});});});});});});
# 194
return 0;}
# 196
if(!Cyc_Tcutil_same_atts(_tmp68,_tmp62)){
({struct Cyc_String_pa_PrintArg_struct _tmp43;_tmp43.tag=0U;_tmp43.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);({struct Cyc_String_pa_PrintArg_struct _tmp42;_tmp42.tag=0U;_tmp42.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({struct Cyc_String_pa_PrintArg_struct _tmp41;_tmp41.tag=0U;({struct _dyneither_ptr _tmp1A7=(struct _dyneither_ptr)({const char*_tmp44="type";_tag_dyneither(_tmp44,sizeof(char),5U);});_tmp41.f1=_tmp1A7;});({void*_tmp3F[3U];(((_tmp3F[0]=& _tmp41,_tmp3F[1]=& _tmp42)),_tmp3F[2]=& _tmp43);({unsigned int _tmp1AA=loc;struct _dyneither_ptr*_tmp1A9=msg;struct _dyneither_ptr _tmp1A8=({const char*_tmp40="%s %s : attribute mismatch on field %s";_tag_dyneither(_tmp40,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp1AA,_tmp1A9,_tmp1A8,_tag_dyneither(_tmp3F,sizeof(void*),3U));});});});});});
_tmp2B=0;}
# 200
if(!Cyc_Tcutil_equal_tqual(_tmp6B,_tmp65)){
({struct Cyc_String_pa_PrintArg_struct _tmp49;_tmp49.tag=0U;_tmp49.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);({struct Cyc_String_pa_PrintArg_struct _tmp48;_tmp48.tag=0U;_tmp48.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({struct Cyc_String_pa_PrintArg_struct _tmp47;_tmp47.tag=0U;({struct _dyneither_ptr _tmp1AB=(struct _dyneither_ptr)({const char*_tmp4A="type";_tag_dyneither(_tmp4A,sizeof(char),5U);});_tmp47.f1=_tmp1AB;});({void*_tmp45[3U];(((_tmp45[0]=& _tmp47,_tmp45[1]=& _tmp48)),_tmp45[2]=& _tmp49);({unsigned int _tmp1AE=loc;struct _dyneither_ptr*_tmp1AD=msg;struct _dyneither_ptr _tmp1AC=({const char*_tmp46="%s %s : qualifier mismatch on field %s";_tag_dyneither(_tmp46,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp1AE,_tmp1AD,_tmp1AC,_tag_dyneither(_tmp45,sizeof(void*),3U));});});});});});
_tmp2B=0;}
# 204
if(((_tmp69 != 0  && _tmp63 != 0) && ({
unsigned int _tmp1AF=Cyc_Tcdecl_get_uint_const_value(_tmp69);_tmp1AF != 
Cyc_Tcdecl_get_uint_const_value(_tmp63);}) || 
_tmp69 == 0  && _tmp63 != 0) || 
_tmp69 != 0  && _tmp63 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp4F;_tmp4F.tag=0U;_tmp4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);({struct Cyc_String_pa_PrintArg_struct _tmp4E;_tmp4E.tag=0U;_tmp4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({struct Cyc_String_pa_PrintArg_struct _tmp4D;_tmp4D.tag=0U;({struct _dyneither_ptr _tmp1B0=(struct _dyneither_ptr)({const char*_tmp50="type";_tag_dyneither(_tmp50,sizeof(char),5U);});_tmp4D.f1=_tmp1B0;});({void*_tmp4B[3U];(((_tmp4B[0]=& _tmp4D,_tmp4B[1]=& _tmp4E)),_tmp4B[2]=& _tmp4F);({unsigned int _tmp1B3=loc;struct _dyneither_ptr*_tmp1B2=msg;struct _dyneither_ptr _tmp1B1=({const char*_tmp4C="%s %s : bitfield mismatch on field %s";_tag_dyneither(_tmp4C,sizeof(char),38U);});Cyc_Tcdecl_merr(_tmp1B3,_tmp1B2,_tmp1B1,_tag_dyneither(_tmp4B,sizeof(void*),3U));});});});});});
_tmp2B=0;}
# 212
if(_tmp67 != 0  && _tmp61 != 0){
void*subst_w1=({struct Cyc_List_List*_tmp1B4=_tmp33;Cyc_Tcutil_substitute(_tmp1B4,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp57=_cycalloc(sizeof(*_tmp57));((*_tmp57).tag=18U,(*_tmp57).f1=_tmp61);_tmp57;}));});
if(!({void*_tmp1B5=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp51=_cycalloc(sizeof(*_tmp51));((*_tmp51).tag=18U,(*_tmp51).f1=_tmp67);_tmp51;});Cyc_Tcdecl_check_type(_tmp1B5,subst_w1);})){
({struct Cyc_String_pa_PrintArg_struct _tmp56;_tmp56.tag=0U;({struct _dyneither_ptr _tmp1B6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(_tmp61));_tmp56.f1=_tmp1B6;});({struct Cyc_String_pa_PrintArg_struct _tmp55;_tmp55.tag=0U;({struct _dyneither_ptr _tmp1B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(_tmp67));_tmp55.f1=_tmp1B7;});({struct Cyc_String_pa_PrintArg_struct _tmp54;_tmp54.tag=0U;_tmp54.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({void*_tmp52[3U];(((_tmp52[0]=& _tmp54,_tmp52[1]=& _tmp55)),_tmp52[2]=& _tmp56);({unsigned int _tmp1BA=loc;struct _dyneither_ptr*_tmp1B9=msg;struct _dyneither_ptr _tmp1B8=({const char*_tmp53="type %s : mismatch on @requires clauses: %s != %s";_tag_dyneither(_tmp53,sizeof(char),50U);});Cyc_Tcdecl_merr(_tmp1BA,_tmp1B9,_tmp1B8,_tag_dyneither(_tmp52,sizeof(void*),3U));});});});});});
_tmp2B=0;}}else{
# 219
if(_tmp67 == 0  && _tmp61 != 0  || 
_tmp67 != 0  && _tmp61 == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0U;_tmp5A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({void*_tmp58[1U];_tmp58[0]=& _tmp5A;({unsigned int _tmp1BD=loc;struct _dyneither_ptr*_tmp1BC=msg;struct _dyneither_ptr _tmp1BB=({const char*_tmp59="type %s : mismatch on @requires clauses";_tag_dyneither(_tmp59,sizeof(char),40U);});Cyc_Tcdecl_merr(_tmp1BD,_tmp1BC,_tmp1BB,_tag_dyneither(_tmp58,sizeof(void*),1U));});});});
_tmp2B=0;}}{
# 225
void*subst_t1=Cyc_Tcutil_substitute(_tmp33,_tmp64);
if(!Cyc_Tcdecl_check_type(_tmp6A,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmp60;_tmp60.tag=0U;({struct _dyneither_ptr _tmp1BE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1));_tmp60.f1=_tmp1BE;});({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0U;({struct _dyneither_ptr _tmp1BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp6A));_tmp5F.f1=_tmp1BF;});({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0U;_tmp5E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp6C);({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0U;_tmp5D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({void*_tmp5B[4U];(((((_tmp5B[0]=& _tmp5D,_tmp5B[1]=& _tmp5E)),_tmp5B[2]=& _tmp5F)),_tmp5B[3]=& _tmp60);({unsigned int _tmp1C2=loc;struct _dyneither_ptr*_tmp1C1=msg;struct _dyneither_ptr _tmp1C0=({const char*_tmp5C="type %s : type mismatch on field %s: %s != %s";_tag_dyneither(_tmp5C,sizeof(char),46U);});Cyc_Tcdecl_merr(_tmp1C2,_tmp1C1,_tmp1C0,_tag_dyneither(_tmp5B,sizeof(void*),4U));});});});});});});
Cyc_Tcutil_explain_failure();
_tmp2B=0;}};};}
# 234
if(_tmp7D != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0U;_tmp70.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp7D->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp6F;_tmp6F.tag=0U;_tmp6F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({void*_tmp6D[2U];(_tmp6D[0]=& _tmp6F,_tmp6D[1]=& _tmp70);({unsigned int _tmp1C5=loc;struct _dyneither_ptr*_tmp1C4=msg;struct _dyneither_ptr _tmp1C3=({const char*_tmp6E="type %s is missing field %s";_tag_dyneither(_tmp6E,sizeof(char),28U);});Cyc_Tcdecl_merr(_tmp1C5,_tmp1C4,_tmp1C3,_tag_dyneither(_tmp6D,sizeof(void*),2U));});});});});
_tmp2B=0;}
# 238
if(_tmp79 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp74;_tmp74.tag=0U;_tmp74.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)_tmp79->hd)->name);({struct Cyc_String_pa_PrintArg_struct _tmp73;_tmp73.tag=0U;_tmp73.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({void*_tmp71[2U];(_tmp71[0]=& _tmp73,_tmp71[1]=& _tmp74);({unsigned int _tmp1C8=loc;struct _dyneither_ptr*_tmp1C7=msg;struct _dyneither_ptr _tmp1C6=({const char*_tmp72="type %s has extra field %s";_tag_dyneither(_tmp72,sizeof(char),27U);});Cyc_Tcdecl_merr(_tmp1C8,_tmp1C7,_tmp1C6,_tag_dyneither(_tmp71,sizeof(void*),2U));});});});});
_tmp2B=0;}
# 243
if(_tmp7C != _tmp78){
({struct Cyc_String_pa_PrintArg_struct _tmp77;_tmp77.tag=0U;_tmp77.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp2A);({void*_tmp75[1U];_tmp75[0]=& _tmp77;({unsigned int _tmp1CB=loc;struct _dyneither_ptr*_tmp1CA=msg;struct _dyneither_ptr _tmp1C9=({const char*_tmp76="%s : previous declaration disagrees with @tagged qualifier";_tag_dyneither(_tmp76,sizeof(char),59U);});Cyc_Tcdecl_merr(_tmp1CB,_tmp1CA,_tmp1C9,_tag_dyneither(_tmp75,sizeof(void*),1U));});});});
_tmp2B=0;}
# 248
d2=d0;
goto _LL3;};}}_LL3:;}
# 252
if(!_tmp2B)return 0;
# 254
if(_tmp82 == d2->sc)
return d2;else{
# 257
({struct Cyc_Absyn_Aggrdecl*_tmp1CC=({struct Cyc_Absyn_Aggrdecl*_tmp80=_cycalloc(sizeof(*_tmp80));*_tmp80=*d2;_tmp80;});d2=_tmp1CC;});
d2->sc=_tmp82;
return d2;}};};}
# 263
inline static struct _dyneither_ptr Cyc_Tcdecl_is_x2string(int is_x){
return is_x?({const char*_tmp84="@extensible datatype";_tag_dyneither(_tmp84,sizeof(char),21U);}):({const char*_tmp85="datatype";_tag_dyneither(_tmp85,sizeof(char),9U);});}struct _tuple15{struct Cyc_Absyn_Tqual f1;void*f2;};
# 269
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_merge_datatypefield(struct Cyc_Absyn_Datatypefield*f0,struct Cyc_Absyn_Datatypefield*f1,struct Cyc_List_List*inst,struct _dyneither_ptr t,struct _dyneither_ptr v,struct _dyneither_ptr*msg){
# 273
unsigned int loc=f1->loc;
# 276
if(Cyc_strptrcmp((*f0->name).f2,(*f1->name).f2)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0U;_tmp8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f0->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp8A;_tmp8A.tag=0U;_tmp8A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*f1->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0U;_tmp89.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp88;_tmp88.tag=0U;_tmp88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp86[4U];(((((_tmp86[0]=& _tmp88,_tmp86[1]=& _tmp89)),_tmp86[2]=& _tmp8A)),_tmp86[3]=& _tmp8B);({unsigned int _tmp1CF=loc;struct _dyneither_ptr*_tmp1CE=msg;struct _dyneither_ptr _tmp1CD=({const char*_tmp87="%s %s: field name mismatch %s != %s";_tag_dyneither(_tmp87,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp1CF,_tmp1CE,_tmp1CD,_tag_dyneither(_tmp86,sizeof(void*),4U));});});});});});});
# 279
return 0;}{
# 282
struct _dyneither_ptr _tmp8C=*(*f0->name).f2;
# 285
struct _tuple10 _tmp8D=({
enum Cyc_Absyn_Scope _tmp1D5=f0->sc;enum Cyc_Absyn_Scope _tmp1D4=f1->sc;struct _dyneither_ptr _tmp1D3=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9;_tmpA9.tag=0U;_tmpA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmpA8;_tmpA8.tag=0U;_tmpA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpA6[2U];(_tmpA6[0]=& _tmpA8,_tmpA6[1]=& _tmpA9);({struct _dyneither_ptr _tmp1D0=({const char*_tmpA7="in %s %s, field";_tag_dyneither(_tmpA7,sizeof(char),16U);});Cyc_aprintf(_tmp1D0,_tag_dyneither(_tmpA6,sizeof(void*),2U));});});});});struct _dyneither_ptr _tmp1D2=_tmp8C;unsigned int _tmp1D1=loc;Cyc_Tcdecl_merge_scope(_tmp1D5,_tmp1D4,_tmp1D3,_tmp1D2,_tmp1D1,msg,0);});
# 285
struct _tuple10 _tmp8E=_tmp8D;enum Cyc_Absyn_Scope _tmpA5;int _tmpA4;_LL1: _tmpA5=_tmp8E.f1;_tmpA4=_tmp8E.f2;_LL2:;{
# 289
struct Cyc_List_List*_tmp8F=f0->typs;
struct Cyc_List_List*_tmp90=f1->typs;
if(({int _tmp1D6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8F);_tmp1D6 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp90);})){
({struct Cyc_String_pa_PrintArg_struct _tmp95;_tmp95.tag=0U;_tmp95.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8C);({struct Cyc_String_pa_PrintArg_struct _tmp94;_tmp94.tag=0U;_tmp94.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp93;_tmp93.tag=0U;_tmp93.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp91[3U];(((_tmp91[0]=& _tmp93,_tmp91[1]=& _tmp94)),_tmp91[2]=& _tmp95);({unsigned int _tmp1D9=loc;struct _dyneither_ptr*_tmp1D8=msg;struct _dyneither_ptr _tmp1D7=({const char*_tmp92="%s %s, field %s: parameter number mismatch";_tag_dyneither(_tmp92,sizeof(char),43U);});Cyc_Tcdecl_merr(_tmp1D9,_tmp1D8,_tmp1D7,_tag_dyneither(_tmp91,sizeof(void*),3U));});});});});});
_tmpA4=0;}
# 296
for(0;_tmp8F != 0  && _tmp90 != 0;(_tmp8F=_tmp8F->tl,_tmp90=_tmp90->tl)){
# 298
if(!Cyc_Tcutil_equal_tqual((*((struct _tuple15*)_tmp8F->hd)).f1,(*((struct _tuple15*)_tmp90->hd)).f1)){
({struct Cyc_String_pa_PrintArg_struct _tmp9A;_tmp9A.tag=0U;_tmp9A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8C);({struct Cyc_String_pa_PrintArg_struct _tmp99;_tmp99.tag=0U;_tmp99.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp98;_tmp98.tag=0U;_tmp98.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp96[3U];(((_tmp96[0]=& _tmp98,_tmp96[1]=& _tmp99)),_tmp96[2]=& _tmp9A);({unsigned int _tmp1DC=loc;struct _dyneither_ptr*_tmp1DB=msg;struct _dyneither_ptr _tmp1DA=({const char*_tmp97="%s %s, field %s: parameter qualifier";_tag_dyneither(_tmp97,sizeof(char),37U);});Cyc_Tcdecl_merr(_tmp1DC,_tmp1DB,_tmp1DA,_tag_dyneither(_tmp96,sizeof(void*),3U));});});});});});
_tmpA4=0;}{
# 302
void*subst_t1=Cyc_Tcutil_substitute(inst,(*((struct _tuple15*)_tmp90->hd)).f2);
# 304
if(!Cyc_Tcdecl_check_type((*((struct _tuple15*)_tmp8F->hd)).f2,subst_t1)){
({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0U;({struct _dyneither_ptr _tmp1DD=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(subst_t1));_tmpA1.f1=_tmp1DD;});({struct Cyc_String_pa_PrintArg_struct _tmpA0;_tmpA0.tag=0U;({struct _dyneither_ptr _tmp1DE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((*((struct _tuple15*)_tmp8F->hd)).f2));_tmpA0.f1=_tmp1DE;});({struct Cyc_String_pa_PrintArg_struct _tmp9F;_tmp9F.tag=0U;_tmp9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp8C);({struct Cyc_String_pa_PrintArg_struct _tmp9E;_tmp9E.tag=0U;_tmp9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp9D;_tmp9D.tag=0U;_tmp9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp9B[5U];(((((((_tmp9B[0]=& _tmp9D,_tmp9B[1]=& _tmp9E)),_tmp9B[2]=& _tmp9F)),_tmp9B[3]=& _tmpA0)),_tmp9B[4]=& _tmpA1);({unsigned int _tmp1E1=loc;struct _dyneither_ptr*_tmp1E0=msg;struct _dyneither_ptr _tmp1DF=({const char*_tmp9C="%s %s, field %s: parameter type mismatch %s != %s";_tag_dyneither(_tmp9C,sizeof(char),50U);});Cyc_Tcdecl_merr(_tmp1E1,_tmp1E0,_tmp1DF,_tag_dyneither(_tmp9B,sizeof(void*),5U));});});});});});});});
Cyc_Tcutil_explain_failure();
_tmpA4=0;}};}
# 312
if(!_tmpA4)return 0;
if(f0->sc != _tmpA5){
struct Cyc_Absyn_Datatypefield*_tmpA2=({struct Cyc_Absyn_Datatypefield*_tmpA3=_cycalloc(sizeof(*_tmpA3));*_tmpA3=*f0;_tmpA3;});
_tmpA2->sc=_tmpA5;
return _tmpA2;}else{
return f0;}};};}
# 320
static struct _tuple15*Cyc_Tcdecl_substitute_datatypefield_f2(struct Cyc_List_List*inst,struct _tuple15*x){
# 323
struct _tuple15*_tmpAA=x;struct Cyc_Absyn_Tqual _tmpAD;void*_tmpAC;_LL1: _tmpAD=_tmpAA->f1;_tmpAC=_tmpAA->f2;_LL2:;
return({struct _tuple15*_tmpAB=_cycalloc(sizeof(*_tmpAB));((*_tmpAB).f1=_tmpAD,({void*_tmp1E2=Cyc_Tcutil_substitute(inst,_tmpAC);(*_tmpAB).f2=_tmp1E2;}));_tmpAB;});}
# 329
static struct Cyc_Absyn_Datatypefield*Cyc_Tcdecl_substitute_datatypefield(struct Cyc_List_List*inst1,struct Cyc_Absyn_Datatypefield*f1){
# 331
struct Cyc_Absyn_Datatypefield*_tmpAE=({struct Cyc_Absyn_Datatypefield*_tmpAF=_cycalloc(sizeof(*_tmpAF));*_tmpAF=*f1;_tmpAF;});
({struct Cyc_List_List*_tmp1E3=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct Cyc_List_List*,struct _tuple15*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcdecl_substitute_datatypefield_f2,inst1,f1->typs);_tmpAE->typs=_tmp1E3;});
# 334
return _tmpAE;}
# 341
static struct Cyc_List_List*Cyc_Tcdecl_merge_xdatatype_fields(struct Cyc_List_List*f0s,struct Cyc_List_List*f1s,struct Cyc_List_List*inst,struct Cyc_List_List*tvs0,struct Cyc_List_List*tvs1,int*res,int*incl,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 350
struct Cyc_List_List**f2sp=({struct Cyc_List_List**_tmpB8=_cycalloc(sizeof(*_tmpB8));*_tmpB8=0;_tmpB8;});
struct Cyc_List_List**_tmpB0=f2sp;
int cmp=-1;
# 354
for(0;f0s != 0  && f1s != 0;f1s=f1s->tl){
while(f0s != 0  && ({int _tmp1E4=Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)f0s->hd)->name,((struct Cyc_Absyn_Datatypefield*)f1s->hd)->name);cmp=_tmp1E4;})< 0){
struct Cyc_List_List*_tmpB1=({struct Cyc_List_List*_tmpB2=_cycalloc(sizeof(*_tmpB2));((*_tmpB2).hd=(struct Cyc_Absyn_Datatypefield*)f0s->hd,(*_tmpB2).tl=0);_tmpB2;});
*_tmpB0=_tmpB1;
_tmpB0=& _tmpB1->tl;
f0s=f0s->tl;}
# 361
if(f0s == 0  || cmp > 0){
# 363
*incl=0;{
struct Cyc_List_List*_tmpB3=({struct Cyc_List_List*_tmpB4=_cycalloc(sizeof(*_tmpB4));(({struct Cyc_Absyn_Datatypefield*_tmp1E5=Cyc_Tcdecl_substitute_datatypefield(inst,(struct Cyc_Absyn_Datatypefield*)f1s->hd);(*_tmpB4).hd=_tmp1E5;}),(*_tmpB4).tl=0);_tmpB4;});
*_tmpB0=_tmpB3;
_tmpB0=& _tmpB3->tl;};}else{
# 369
struct Cyc_Absyn_Datatypefield*_tmpB5=Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)f0s->hd,(struct Cyc_Absyn_Datatypefield*)f1s->hd,inst,t,v,msg);
if(_tmpB5 != 0){
if(_tmpB5 != (struct Cyc_Absyn_Datatypefield*)f0s->hd)*incl=0;{
struct Cyc_List_List*_tmpB6=({struct Cyc_List_List*_tmpB7=_cycalloc(sizeof(*_tmpB7));((*_tmpB7).hd=_tmpB5,(*_tmpB7).tl=0);_tmpB7;});
*_tmpB0=_tmpB6;
_tmpB0=& _tmpB6->tl;};}else{
# 376
*res=0;}
# 378
f0s=f0s->tl;}}
# 383
if(f1s != 0){
*incl=0;
*_tmpB0=f1s;}else{
# 387
*_tmpB0=f0s;}
# 390
return*f2sp;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 394
static struct _tuple16 Cyc_Tcdecl_split(struct Cyc_List_List*f){
if(f == 0)return({struct _tuple16 _tmp16E;(_tmp16E.f1=0,_tmp16E.f2=0);_tmp16E;});
if(f->tl == 0)return({struct _tuple16 _tmp16F;(_tmp16F.f1=f,_tmp16F.f2=0);_tmp16F;});{
struct _tuple16 _tmpB9=Cyc_Tcdecl_split(((struct Cyc_List_List*)_check_null(f->tl))->tl);struct _tuple16 _tmpBA=_tmpB9;struct Cyc_List_List*_tmpBE;struct Cyc_List_List*_tmpBD;_LL1: _tmpBE=_tmpBA.f1;_tmpBD=_tmpBA.f2;_LL2:;
return({struct _tuple16 _tmp170;(({struct Cyc_List_List*_tmp1E7=({struct Cyc_List_List*_tmpBB=_cycalloc(sizeof(*_tmpBB));((*_tmpBB).hd=f->hd,(*_tmpBB).tl=_tmpBE);_tmpBB;});_tmp170.f1=_tmp1E7;}),({struct Cyc_List_List*_tmp1E6=({struct Cyc_List_List*_tmpBC=_cycalloc(sizeof(*_tmpBC));((*_tmpBC).hd=((struct Cyc_List_List*)_check_null(f->tl))->hd,(*_tmpBC).tl=_tmpBD);_tmpBC;});_tmp170.f2=_tmp1E6;}));_tmp170;});};}
# 403
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*f,int*res,struct _dyneither_ptr*v,unsigned int loc,struct _dyneither_ptr*msg){
# 406
struct _tuple16 _tmpBF=((struct _tuple16(*)(struct Cyc_List_List*f))Cyc_Tcdecl_split)(f);struct _tuple16 _tmpC0=_tmpBF;struct Cyc_List_List*_tmpC4;struct Cyc_List_List*_tmpC3;_LL1: _tmpC4=_tmpC0.f1;_tmpC3=_tmpC0.f2;_LL2:;
if(_tmpC4 != 0  && _tmpC4->tl != 0)
({struct Cyc_List_List*_tmp1E8=Cyc_Tcdecl_sort_xdatatype_fields(_tmpC4,res,v,loc,msg);_tmpC4=_tmp1E8;});
if(_tmpC3 != 0  && _tmpC3->tl != 0)
({struct Cyc_List_List*_tmp1E9=Cyc_Tcdecl_sort_xdatatype_fields(_tmpC3,res,v,loc,msg);_tmpC3=_tmp1E9;});
return({struct Cyc_List_List*_tmp1F0=_tmpC4;struct Cyc_List_List*_tmp1EF=_tmpC3;int*_tmp1EE=res;int*_tmp1ED=({int*_tmpC1=_cycalloc_atomic(sizeof(*_tmpC1));*_tmpC1=1;_tmpC1;});struct _dyneither_ptr _tmp1EC=({const char*_tmpC2="@extensible datatype";_tag_dyneither(_tmpC2,sizeof(char),21U);});struct _dyneither_ptr _tmp1EB=*v;unsigned int _tmp1EA=loc;Cyc_Tcdecl_merge_xdatatype_fields(_tmp1F0,_tmp1EF,0,0,0,_tmp1EE,_tmp1ED,_tmp1EC,_tmp1EB,_tmp1EA,msg);});}struct _tuple17{struct Cyc_Core_Opt*f1;struct Cyc_Core_Opt*f2;};
# 415
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*d0,struct Cyc_Absyn_Datatypedecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 417
struct _dyneither_ptr _tmpC5=Cyc_Absynpp_qvar2string(d0->name);
struct _dyneither_ptr t=({const char*_tmpE4="datatype";_tag_dyneither(_tmpE4,sizeof(char),9U);});
int _tmpC6=1;
# 424
if(d0->is_extensible != d1->is_extensible){
({struct Cyc_String_pa_PrintArg_struct _tmpCB;_tmpCB.tag=0U;({struct _dyneither_ptr _tmp1F1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcdecl_is_x2string(d1->is_extensible));_tmpCB.f1=_tmp1F1;});({struct Cyc_String_pa_PrintArg_struct _tmpCA;_tmpCA.tag=0U;({struct _dyneither_ptr _tmp1F2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcdecl_is_x2string(d0->is_extensible));_tmpCA.f1=_tmp1F2;});({struct Cyc_String_pa_PrintArg_struct _tmpC9;_tmpC9.tag=0U;_tmpC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);({void*_tmpC7[3U];(((_tmpC7[0]=& _tmpC9,_tmpC7[1]=& _tmpCA)),_tmpC7[2]=& _tmpCB);({unsigned int _tmp1F5=loc;struct _dyneither_ptr*_tmp1F4=msg;struct _dyneither_ptr _tmp1F3=({const char*_tmpC8="expected %s to be a %s instead of a %s";_tag_dyneither(_tmpC8,sizeof(char),39U);});Cyc_Tcdecl_merr(_tmp1F5,_tmp1F4,_tmp1F3,_tag_dyneither(_tmpC7,sizeof(void*),3U));});});});});});
_tmpC6=0;}else{
# 429
({struct _dyneither_ptr _tmp1F6=Cyc_Tcdecl_is_x2string(d0->is_extensible);t=_tmp1F6;});}
# 433
if(!Cyc_Tcdecl_check_tvs(d0->tvs,d1->tvs,t,_tmpC5,loc,msg))return 0;{
# 436
struct _tuple10 _tmpCC=Cyc_Tcdecl_merge_scope(d0->sc,d1->sc,t,_tmpC5,loc,msg,0);struct _tuple10 _tmpCD=_tmpCC;enum Cyc_Absyn_Scope _tmpE3;int _tmpE2;_LL1: _tmpE3=_tmpCD.f1;_tmpE2=_tmpCD.f2;_LL2:;
if(!_tmpE2)_tmpC6=0;{
# 439
struct Cyc_Absyn_Datatypedecl*d2;
# 442
{struct _tuple17 _tmpCE=({struct _tuple17 _tmp171;(_tmp171.f1=d0->fields,_tmp171.f2=d1->fields);_tmp171;});struct _tuple17 _tmpCF=_tmpCE;struct Cyc_List_List*_tmpE0;struct Cyc_List_List*_tmpDF;if(_tmpCF.f2 == 0){_LL4: _LL5:
 d2=d0;goto _LL3;}else{if(_tmpCF.f1 == 0){_LL6: _LL7:
 d2=d1;goto _LL3;}else{_LL8: _tmpE0=(struct Cyc_List_List*)(_tmpCF.f1)->v;_tmpDF=(struct Cyc_List_List*)(_tmpCF.f2)->v;_LL9: {
# 456 "tcdecl.cyc"
struct Cyc_List_List*_tmpD0=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 459
if(d0->is_extensible){
# 463
int _tmpD1=1;
struct Cyc_List_List*_tmpD2=Cyc_Tcdecl_merge_xdatatype_fields(_tmpE0,_tmpDF,_tmpD0,d0->tvs,d1->tvs,& _tmpC6,& _tmpD1,t,_tmpC5,loc,msg);
# 467
if(_tmpD1)
d2=d0;else{
# 470
({struct Cyc_Absyn_Datatypedecl*_tmp1F7=({struct Cyc_Absyn_Datatypedecl*_tmpD3=_cycalloc(sizeof(*_tmpD3));*_tmpD3=*d0;_tmpD3;});d2=_tmp1F7;});
d2->sc=_tmpE3;
({struct Cyc_Core_Opt*_tmp1F8=({struct Cyc_Core_Opt*_tmpD4=_cycalloc(sizeof(*_tmpD4));(*_tmpD4).v=_tmpD2;_tmpD4;});d2->fields=_tmp1F8;});}}else{
# 476
for(0;_tmpE0 != 0  && _tmpDF != 0;(_tmpE0=_tmpE0->tl,_tmpDF=_tmpDF->tl)){
Cyc_Tcdecl_merge_datatypefield((struct Cyc_Absyn_Datatypefield*)_tmpE0->hd,(struct Cyc_Absyn_Datatypefield*)_tmpDF->hd,_tmpD0,t,_tmpC5,msg);}
# 479
if(_tmpE0 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0U;_tmpD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpE0->hd)->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0U;_tmpD8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);({struct Cyc_String_pa_PrintArg_struct _tmpD7;_tmpD7.tag=0U;_tmpD7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpD5[3U];(((_tmpD5[0]=& _tmpD7,_tmpD5[1]=& _tmpD8)),_tmpD5[2]=& _tmpD9);({unsigned int _tmp1FB=loc;struct _dyneither_ptr*_tmp1FA=msg;struct _dyneither_ptr _tmp1F9=({const char*_tmpD6="%s %s is missing field %s";_tag_dyneither(_tmpD6,sizeof(char),26U);});Cyc_Tcdecl_merr(_tmp1FB,_tmp1FA,_tmp1F9,_tag_dyneither(_tmpD5,sizeof(void*),3U));});});});});});
_tmpC6=0;}
# 483
if(_tmpDF != 0){
({struct Cyc_String_pa_PrintArg_struct _tmpDE;_tmpDE.tag=0U;_tmpDE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*((struct Cyc_Absyn_Datatypefield*)_tmpDF->hd)->name).f2);({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0U;_tmpDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);({struct Cyc_String_pa_PrintArg_struct _tmpDC;_tmpDC.tag=0U;_tmpDC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmpDA[3U];(((_tmpDA[0]=& _tmpDC,_tmpDA[1]=& _tmpDD)),_tmpDA[2]=& _tmpDE);({unsigned int _tmp1FE=loc;struct _dyneither_ptr*_tmp1FD=msg;struct _dyneither_ptr _tmp1FC=({const char*_tmpDB="%s %s has extra field %s";_tag_dyneither(_tmpDB,sizeof(char),25U);});Cyc_Tcdecl_merr(_tmp1FE,_tmp1FD,_tmp1FC,_tag_dyneither(_tmpDA,sizeof(void*),3U));});});});});});
_tmpC6=0;}
# 487
d2=d0;}
# 489
goto _LL3;}}}_LL3:;}
# 492
if(!_tmpC6)return 0;
# 494
if(_tmpE3 == d2->sc)
return d2;else{
# 497
({struct Cyc_Absyn_Datatypedecl*_tmp1FF=({struct Cyc_Absyn_Datatypedecl*_tmpE1=_cycalloc(sizeof(*_tmpE1));*_tmpE1=*d2;_tmpE1;});d2=_tmp1FF;});
d2->sc=_tmpE3;
return d2;}};};}
# 503
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*d0,struct Cyc_Absyn_Enumdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 505
struct _dyneither_ptr _tmpE5=Cyc_Absynpp_qvar2string(d0->name);
int _tmpE6=1;
# 509
struct _tuple10 _tmpE7=({enum Cyc_Absyn_Scope _tmp204=d0->sc;enum Cyc_Absyn_Scope _tmp203=d1->sc;struct _dyneither_ptr _tmp202=({const char*_tmp103="enum";_tag_dyneither(_tmp103,sizeof(char),5U);});struct _dyneither_ptr _tmp201=_tmpE5;unsigned int _tmp200=loc;Cyc_Tcdecl_merge_scope(_tmp204,_tmp203,_tmp202,_tmp201,_tmp200,msg,1);});struct _tuple10 _tmpE8=_tmpE7;enum Cyc_Absyn_Scope _tmp102;int _tmp101;_LL1: _tmp102=_tmpE8.f1;_tmp101=_tmpE8.f2;_LL2:;
if(!_tmp101)_tmpE6=0;{
# 512
struct Cyc_Absyn_Enumdecl*d2;
# 515
{struct _tuple17 _tmpE9=({struct _tuple17 _tmp172;(_tmp172.f1=d0->fields,_tmp172.f2=d1->fields);_tmp172;});struct _tuple17 _tmpEA=_tmpE9;struct Cyc_List_List*_tmpFF;struct Cyc_List_List*_tmpFE;if(_tmpEA.f2 == 0){_LL4: _LL5:
 d2=d0;goto _LL3;}else{if(_tmpEA.f1 == 0){_LL6: _LL7:
 d2=d1;goto _LL3;}else{_LL8: _tmpFF=(struct Cyc_List_List*)(_tmpEA.f1)->v;_tmpFE=(struct Cyc_List_List*)(_tmpEA.f2)->v;_LL9:
# 520
 for(0;_tmpFF != 0  && _tmpFE != 0;(_tmpFF=_tmpFF->tl,_tmpFE=_tmpFE->tl)){
struct Cyc_Absyn_Enumfield*_tmpEB=(struct Cyc_Absyn_Enumfield*)_tmpFF->hd;struct Cyc_Absyn_Enumfield*_tmpEC=_tmpEB;struct _dyneither_ptr*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;unsigned int _tmpFB;_LLB: _tmpFD=(_tmpEC->name)->f2;_tmpFC=_tmpEC->tag;_tmpFB=_tmpEC->loc;_LLC:;{
struct Cyc_Absyn_Enumfield*_tmpED=(struct Cyc_Absyn_Enumfield*)_tmpFE->hd;struct Cyc_Absyn_Enumfield*_tmpEE=_tmpED;struct _dyneither_ptr*_tmpFA;struct Cyc_Absyn_Exp*_tmpF9;unsigned int _tmpF8;_LLE: _tmpFA=(_tmpEE->name)->f2;_tmpF9=_tmpEE->tag;_tmpF8=_tmpEE->loc;_LLF:;
# 525
if(Cyc_strptrcmp(_tmpFD,_tmpFA)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF3;_tmpF3.tag=0U;_tmpF3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFA);({struct Cyc_String_pa_PrintArg_struct _tmpF2;_tmpF2.tag=0U;_tmpF2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFD);({struct Cyc_String_pa_PrintArg_struct _tmpF1;_tmpF1.tag=0U;_tmpF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE5);({void*_tmpEF[3U];(((_tmpEF[0]=& _tmpF1,_tmpEF[1]=& _tmpF2)),_tmpEF[2]=& _tmpF3);({unsigned int _tmp207=loc;struct _dyneither_ptr*_tmp206=msg;struct _dyneither_ptr _tmp205=({const char*_tmpF0="enum %s: field name mismatch %s != %s";_tag_dyneither(_tmpF0,sizeof(char),38U);});Cyc_Tcdecl_merr(_tmp207,_tmp206,_tmp205,_tag_dyneither(_tmpEF,sizeof(void*),3U));});});});});});
_tmpE6=0;}
# 530
if(({unsigned int _tmp208=Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpFC));_tmp208 != Cyc_Tcdecl_get_uint_const_value((struct Cyc_Absyn_Exp*)_check_null(_tmpF9));})){
({struct Cyc_String_pa_PrintArg_struct _tmpF7;_tmpF7.tag=0U;_tmpF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFA);({struct Cyc_String_pa_PrintArg_struct _tmpF6;_tmpF6.tag=0U;_tmpF6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpE5);({void*_tmpF4[2U];(_tmpF4[0]=& _tmpF6,_tmpF4[1]=& _tmpF7);({unsigned int _tmp20B=loc;struct _dyneither_ptr*_tmp20A=msg;struct _dyneither_ptr _tmp209=({const char*_tmpF5="enum %s, field %s, value mismatch";_tag_dyneither(_tmpF5,sizeof(char),34U);});Cyc_Tcdecl_merr(_tmp20B,_tmp20A,_tmp209,_tag_dyneither(_tmpF4,sizeof(void*),2U));});});});});
_tmpE6=0;}};}
# 535
d2=d0;
goto _LL3;}}_LL3:;}
# 539
if(!_tmpE6)return 0;
# 541
if(d2->sc == _tmp102)
return d2;else{
# 544
({struct Cyc_Absyn_Enumdecl*_tmp20C=({struct Cyc_Absyn_Enumdecl*_tmp100=_cycalloc(sizeof(*_tmp100));*_tmp100=*d2;_tmp100;});d2=_tmp20C;});
d2->sc=_tmp102;
return d2;}};}
# 550
static struct _tuple10 Cyc_Tcdecl_check_var_or_fn_decl(enum Cyc_Absyn_Scope sc0,void*t0,struct Cyc_Absyn_Tqual tq0,struct Cyc_List_List*atts0,enum Cyc_Absyn_Scope sc1,void*t1,struct Cyc_Absyn_Tqual tq1,struct Cyc_List_List*atts1,struct _dyneither_ptr t,struct _dyneither_ptr v,unsigned int loc,struct _dyneither_ptr*msg){
# 554
int _tmp104=1;
# 557
struct _tuple10 _tmp105=Cyc_Tcdecl_merge_scope(sc0,sc1,t,v,loc,msg,0);struct _tuple10 _tmp106=_tmp105;enum Cyc_Absyn_Scope _tmp12B;int _tmp12A;_LL1: _tmp12B=_tmp106.f1;_tmp12A=_tmp106.f2;_LL2:;
if(!_tmp12A)_tmp104=0;
# 561
if(!Cyc_Tcdecl_check_type(t0,t1)){
struct _dyneither_ptr _tmp107=Cyc_Absynpp_typ2string(t0);
struct _dyneither_ptr _tmp108=Cyc_Absynpp_typ2string(t1);
if(({unsigned long _tmp210=({unsigned long _tmp20F=({unsigned long _tmp20E=({unsigned long _tmp20D=Cyc_strlen((struct _dyneither_ptr)_tmp107);_tmp20D + Cyc_strlen((struct _dyneither_ptr)_tmp108);});_tmp20E + Cyc_strlen((struct _dyneither_ptr)t);});_tmp20F + Cyc_strlen((struct _dyneither_ptr)v);});_tmp210 + 
Cyc_strlen(({const char*_tmp109="  was declared  instead of ";_tag_dyneither(_tmp109,sizeof(char),28U);}));})> 70)
({struct Cyc_String_pa_PrintArg_struct _tmp10F;_tmp10F.tag=0U;_tmp10F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107);({struct Cyc_String_pa_PrintArg_struct _tmp10E;_tmp10E.tag=0U;_tmp10E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108);({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0U;_tmp10D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0U;_tmp10C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp10A[4U];(((((_tmp10A[0]=& _tmp10C,_tmp10A[1]=& _tmp10D)),_tmp10A[2]=& _tmp10E)),_tmp10A[3]=& _tmp10F);({unsigned int _tmp213=loc;struct _dyneither_ptr*_tmp212=msg;struct _dyneither_ptr _tmp211=({const char*_tmp10B="%s %s was declared \n\t%s\n instead of \n\t%s";_tag_dyneither(_tmp10B,sizeof(char),41U);});Cyc_Tcdecl_merr(_tmp213,_tmp212,_tmp211,_tag_dyneither(_tmp10A,sizeof(void*),4U));});});});});});});else{
# 569
({struct Cyc_String_pa_PrintArg_struct _tmp115;_tmp115.tag=0U;_tmp115.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp107);({struct Cyc_String_pa_PrintArg_struct _tmp114;_tmp114.tag=0U;_tmp114.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp108);({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0U;_tmp113.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp112;_tmp112.tag=0U;_tmp112.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp110[4U];(((((_tmp110[0]=& _tmp112,_tmp110[1]=& _tmp113)),_tmp110[2]=& _tmp114)),_tmp110[3]=& _tmp115);({unsigned int _tmp216=loc;struct _dyneither_ptr*_tmp215=msg;struct _dyneither_ptr _tmp214=({const char*_tmp111="%s %s was declared %s instead of %s";_tag_dyneither(_tmp111,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp216,_tmp215,_tmp214,_tag_dyneither(_tmp110,sizeof(void*),4U));});});});});});});}
# 571
Cyc_Tcutil_explain_failure();
_tmp104=0;}
# 576
if(!Cyc_Tcutil_equal_tqual(tq0,tq1)){
({struct Cyc_String_pa_PrintArg_struct _tmp119;_tmp119.tag=0U;_tmp119.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp118;_tmp118.tag=0U;_tmp118.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp116[2U];(_tmp116[0]=& _tmp118,_tmp116[1]=& _tmp119);({unsigned int _tmp219=loc;struct _dyneither_ptr*_tmp218=msg;struct _dyneither_ptr _tmp217=({const char*_tmp117="%s %s has different type qualifiers";_tag_dyneither(_tmp117,sizeof(char),36U);});Cyc_Tcdecl_merr(_tmp219,_tmp218,_tmp217,_tag_dyneither(_tmp116,sizeof(void*),2U));});});});});
_tmp104=0;}
# 582
if(!Cyc_Tcutil_same_atts(atts0,atts1)){
({struct Cyc_String_pa_PrintArg_struct _tmp11D;_tmp11D.tag=0U;_tmp11D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)v);({struct Cyc_String_pa_PrintArg_struct _tmp11C;_tmp11C.tag=0U;_tmp11C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)t);({void*_tmp11A[2U];(_tmp11A[0]=& _tmp11C,_tmp11A[1]=& _tmp11D);({unsigned int _tmp21C=loc;struct _dyneither_ptr*_tmp21B=msg;struct _dyneither_ptr _tmp21A=({const char*_tmp11B="%s %s has different attributes";_tag_dyneither(_tmp11B,sizeof(char),31U);});Cyc_Tcdecl_merr(_tmp21C,_tmp21B,_tmp21A,_tag_dyneither(_tmp11A,sizeof(void*),2U));});});});});
({void*_tmp11E=0U;({struct Cyc___cycFILE*_tmp21E=Cyc_stderr;struct _dyneither_ptr _tmp21D=({const char*_tmp11F="\tprevious attributes: ";_tag_dyneither(_tmp11F,sizeof(char),23U);});Cyc_fprintf(_tmp21E,_tmp21D,_tag_dyneither(_tmp11E,sizeof(void*),0U));});});
for(0;atts0 != 0;atts0=atts0->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0U;({struct _dyneither_ptr _tmp21F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts0->hd));_tmp122.f1=_tmp21F;});({void*_tmp120[1U];_tmp120[0]=& _tmp122;({struct Cyc___cycFILE*_tmp221=Cyc_stderr;struct _dyneither_ptr _tmp220=({const char*_tmp121="%s ";_tag_dyneither(_tmp121,sizeof(char),4U);});Cyc_fprintf(_tmp221,_tmp220,_tag_dyneither(_tmp120,sizeof(void*),1U));});});});}
({void*_tmp123=0U;({struct Cyc___cycFILE*_tmp223=Cyc_stderr;struct _dyneither_ptr _tmp222=({const char*_tmp124="\n\tcurrent attributes: ";_tag_dyneither(_tmp124,sizeof(char),23U);});Cyc_fprintf(_tmp223,_tmp222,_tag_dyneither(_tmp123,sizeof(void*),0U));});});
for(0;atts1 != 0;atts1=atts1->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp127;_tmp127.tag=0U;({struct _dyneither_ptr _tmp224=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts1->hd));_tmp127.f1=_tmp224;});({void*_tmp125[1U];_tmp125[0]=& _tmp127;({struct Cyc___cycFILE*_tmp226=Cyc_stderr;struct _dyneither_ptr _tmp225=({const char*_tmp126="%s ";_tag_dyneither(_tmp126,sizeof(char),4U);});Cyc_fprintf(_tmp226,_tmp225,_tag_dyneither(_tmp125,sizeof(void*),1U));});});});}
({void*_tmp128=0U;({struct Cyc___cycFILE*_tmp228=Cyc_stderr;struct _dyneither_ptr _tmp227=({const char*_tmp129="\n";_tag_dyneither(_tmp129,sizeof(char),2U);});Cyc_fprintf(_tmp228,_tmp227,_tag_dyneither(_tmp128,sizeof(void*),0U));});});
_tmp104=0;}
# 594
return({struct _tuple10 _tmp173;(_tmp173.f1=_tmp12B,_tmp173.f2=_tmp104);_tmp173;});}
# 597
struct Cyc_Absyn_Vardecl*Cyc_Tcdecl_merge_vardecl(struct Cyc_Absyn_Vardecl*d0,struct Cyc_Absyn_Vardecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 599
struct _dyneither_ptr _tmp12C=Cyc_Absynpp_qvar2string(d0->name);
# 602
struct _tuple10 _tmp12D=({
enum Cyc_Absyn_Scope _tmp233=d0->sc;void*_tmp232=d0->type;struct Cyc_Absyn_Tqual _tmp231=d0->tq;struct Cyc_List_List*_tmp230=d0->attributes;enum Cyc_Absyn_Scope _tmp22F=d1->sc;void*_tmp22E=d1->type;struct Cyc_Absyn_Tqual _tmp22D=d1->tq;struct Cyc_List_List*_tmp22C=d1->attributes;struct _dyneither_ptr _tmp22B=({const char*_tmp133="variable";_tag_dyneither(_tmp133,sizeof(char),9U);});struct _dyneither_ptr _tmp22A=_tmp12C;unsigned int _tmp229=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp233,_tmp232,_tmp231,_tmp230,_tmp22F,_tmp22E,_tmp22D,_tmp22C,_tmp22B,_tmp22A,_tmp229,msg);});
# 602
struct _tuple10 _tmp12E=_tmp12D;enum Cyc_Absyn_Scope _tmp132;int _tmp131;_LL1: _tmp132=_tmp12E.f1;_tmp131=_tmp12E.f2;_LL2:;
# 606
if(!_tmp131)return 0;
# 608
if(d0->sc == _tmp132)
return d0;else{
# 611
struct Cyc_Absyn_Vardecl*_tmp12F=({struct Cyc_Absyn_Vardecl*_tmp130=_cycalloc(sizeof(*_tmp130));*_tmp130=*d0;_tmp130;});
_tmp12F->sc=_tmp132;
return _tmp12F;}}
# 617
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*d0,struct Cyc_Absyn_Typedefdecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 619
struct _dyneither_ptr _tmp134=Cyc_Absynpp_qvar2string(d0->name);
# 622
if(!({struct Cyc_List_List*_tmp238=d0->tvs;struct Cyc_List_List*_tmp237=d1->tvs;struct _dyneither_ptr _tmp236=({const char*_tmp135="typedef";_tag_dyneither(_tmp135,sizeof(char),8U);});struct _dyneither_ptr _tmp235=_tmp134;unsigned int _tmp234=loc;Cyc_Tcdecl_check_tvs(_tmp238,_tmp237,_tmp236,_tmp235,_tmp234,msg);}))return 0;{
# 625
struct Cyc_List_List*_tmp136=Cyc_Tcdecl_build_tvs_map(d0->tvs,d1->tvs);
# 628
if(d0->defn != 0  && d1->defn != 0){
void*subst_defn1=Cyc_Tcutil_substitute(_tmp136,(void*)_check_null(d1->defn));
if(!Cyc_Tcdecl_check_type((void*)_check_null(d0->defn),subst_defn1)){
({struct Cyc_String_pa_PrintArg_struct _tmp13B;_tmp13B.tag=0U;({struct _dyneither_ptr _tmp239=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(d0->defn)));_tmp13B.f1=_tmp239;});({struct Cyc_String_pa_PrintArg_struct _tmp13A;_tmp13A.tag=0U;({struct _dyneither_ptr _tmp23A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(subst_defn1));_tmp13A.f1=_tmp23A;});({struct Cyc_String_pa_PrintArg_struct _tmp139;_tmp139.tag=0U;_tmp139.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp134);({void*_tmp137[3U];(((_tmp137[0]=& _tmp139,_tmp137[1]=& _tmp13A)),_tmp137[2]=& _tmp13B);({unsigned int _tmp23D=loc;struct _dyneither_ptr*_tmp23C=msg;struct _dyneither_ptr _tmp23B=({const char*_tmp138="typedef %s does not refer to the same type: %s != %s";_tag_dyneither(_tmp138,sizeof(char),53U);});Cyc_Tcdecl_merr(_tmp23D,_tmp23C,_tmp23B,_tag_dyneither(_tmp137,sizeof(void*),3U));});});});});});
return 0;}}
# 636
return d0;};}
# 639
void**Cyc_Tcdecl_merge_binding(void*b0,void*b1,unsigned int loc,struct _dyneither_ptr*msg){
struct _tuple14 _tmp13C=({struct _tuple14 _tmp174;(_tmp174.f1=b0,_tmp174.f2=b1);_tmp174;});struct _tuple14 _tmp13D=_tmp13C;struct Cyc_Absyn_Fndecl*_tmp156;struct Cyc_Absyn_Vardecl*_tmp155;struct Cyc_Absyn_Fndecl*_tmp154;struct Cyc_Absyn_Vardecl*_tmp153;struct Cyc_Absyn_Fndecl*_tmp152;struct Cyc_Absyn_Vardecl*_tmp151;struct Cyc_Absyn_Vardecl*_tmp150;switch(*((int*)_tmp13D.f1)){case 1U: switch(*((int*)_tmp13D.f2)){case 1U: _LL1: _tmp151=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13D.f1)->f1;_tmp150=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13D.f2)->f1;_LL2: {
# 642
struct Cyc_Absyn_Vardecl*_tmp13E=Cyc_Tcdecl_merge_vardecl(_tmp151,_tmp150,loc,msg);
if(_tmp13E == 0)return 0;
if(_tmp13E == _tmp151)return({void**_tmp13F=_cycalloc(sizeof(*_tmp13F));*_tmp13F=b0;_tmp13F;});
if(_tmp13E == _tmp150)return({void**_tmp140=_cycalloc(sizeof(*_tmp140));*_tmp140=b1;_tmp140;});
return({void**_tmp142=_cycalloc(sizeof(*_tmp142));({void*_tmp23E=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp141=_cycalloc(sizeof(*_tmp141));((*_tmp141).tag=1U,(*_tmp141).f1=_tmp13E);_tmp141;});*_tmp142=_tmp23E;});_tmp142;});}case 2U: _LL3: _tmp153=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13D.f1)->f1;_tmp152=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13D.f2)->f1;_LL4: {
# 648
struct _tuple10 _tmp143=({enum Cyc_Absyn_Scope _tmp249=_tmp153->sc;void*_tmp248=_tmp153->type;struct Cyc_Absyn_Tqual _tmp247=_tmp153->tq;struct Cyc_List_List*_tmp246=_tmp153->attributes;enum Cyc_Absyn_Scope _tmp245=_tmp152->sc;void*_tmp244=(void*)_check_null(_tmp152->cached_typ);struct Cyc_Absyn_Tqual _tmp243=
Cyc_Absyn_empty_tqual(0U);
# 648
struct Cyc_List_List*_tmp242=_tmp152->attributes;struct _dyneither_ptr _tmp241=({const char*_tmp147="function";_tag_dyneither(_tmp147,sizeof(char),9U);});struct _dyneither_ptr _tmp240=
# 651
Cyc_Absynpp_qvar2string(_tmp153->name);
# 648
unsigned int _tmp23F=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp249,_tmp248,_tmp247,_tmp246,_tmp245,_tmp244,_tmp243,_tmp242,_tmp241,_tmp240,_tmp23F,msg);});struct _tuple10 _tmp144=_tmp143;int _tmp146;_LLC: _tmp146=_tmp144.f2;_LLD:;
# 652
return _tmp146?({void**_tmp145=_cycalloc(sizeof(*_tmp145));*_tmp145=b1;_tmp145;}): 0;}default: goto _LL9;}case 2U: switch(*((int*)_tmp13D.f2)){case 2U: _LL5: _tmp154=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13D.f2)->f1;_LL6:
# 654
({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0U;({struct _dyneither_ptr _tmp24A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp154->name));_tmp14A.f1=_tmp24A;});({void*_tmp148[1U];_tmp148[0]=& _tmp14A;({unsigned int _tmp24D=loc;struct _dyneither_ptr*_tmp24C=msg;struct _dyneither_ptr _tmp24B=({const char*_tmp149="redefinition of function %s";_tag_dyneither(_tmp149,sizeof(char),28U);});Cyc_Tcdecl_merr(_tmp24D,_tmp24C,_tmp24B,_tag_dyneither(_tmp148,sizeof(void*),1U));});});});
return 0;case 1U: _LL7: _tmp156=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp13D.f1)->f1;_tmp155=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp13D.f2)->f1;_LL8: {
# 657
struct _tuple10 _tmp14B=({enum Cyc_Absyn_Scope _tmp258=_tmp156->sc;void*_tmp257=(void*)_check_null(_tmp156->cached_typ);struct Cyc_Absyn_Tqual _tmp256=Cyc_Absyn_empty_tqual(0U);struct Cyc_List_List*_tmp255=_tmp156->attributes;enum Cyc_Absyn_Scope _tmp254=_tmp155->sc;void*_tmp253=_tmp155->type;struct Cyc_Absyn_Tqual _tmp252=_tmp155->tq;struct Cyc_List_List*_tmp251=_tmp155->attributes;struct _dyneither_ptr _tmp250=({const char*_tmp14F="variable";_tag_dyneither(_tmp14F,sizeof(char),9U);});struct _dyneither_ptr _tmp24F=
# 660
Cyc_Absynpp_qvar2string(_tmp156->name);
# 657
unsigned int _tmp24E=loc;Cyc_Tcdecl_check_var_or_fn_decl(_tmp258,_tmp257,_tmp256,_tmp255,_tmp254,_tmp253,_tmp252,_tmp251,_tmp250,_tmp24F,_tmp24E,msg);});struct _tuple10 _tmp14C=_tmp14B;int _tmp14E;_LLF: _tmp14E=_tmp14C.f2;_LL10:;
# 661
return _tmp14E?({void**_tmp14D=_cycalloc(sizeof(*_tmp14D));*_tmp14D=b0;_tmp14D;}): 0;}default: goto _LL9;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 666
struct Cyc_Tcdecl_Xdatatypefielddecl*Cyc_Tcdecl_merge_xdatatypefielddecl(struct Cyc_Tcdecl_Xdatatypefielddecl*d0,struct Cyc_Tcdecl_Xdatatypefielddecl*d1,unsigned int loc,struct _dyneither_ptr*msg){
# 669
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp157=d0;struct Cyc_Absyn_Datatypedecl*_tmp167;struct Cyc_Absyn_Datatypefield*_tmp166;_LL1: _tmp167=_tmp157->base;_tmp166=_tmp157->field;_LL2:;{
struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp158=d1;struct Cyc_Absyn_Datatypedecl*_tmp165;struct Cyc_Absyn_Datatypefield*_tmp164;_LL4: _tmp165=_tmp158->base;_tmp164=_tmp158->field;_LL5:;{
struct _dyneither_ptr _tmp159=Cyc_Absynpp_qvar2string(_tmp166->name);
# 674
if(Cyc_Absyn_qvar_cmp(_tmp167->name,_tmp165->name)!= 0){
({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0U;_tmp15E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp159);({struct Cyc_String_pa_PrintArg_struct _tmp15D;_tmp15D.tag=0U;({struct _dyneither_ptr _tmp259=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp165->name));_tmp15D.f1=_tmp259;});({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0U;({struct _dyneither_ptr _tmp25A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp167->name));_tmp15C.f1=_tmp25A;});({void*_tmp15A[3U];(((_tmp15A[0]=& _tmp15C,_tmp15A[1]=& _tmp15D)),_tmp15A[2]=& _tmp15E);({unsigned int _tmp25D=loc;struct _dyneither_ptr*_tmp25C=msg;struct _dyneither_ptr _tmp25B=({const char*_tmp15B="@extensible datatype %s and %s have both a field named %s";_tag_dyneither(_tmp15B,sizeof(char),58U);});Cyc_Tcdecl_merr(_tmp25D,_tmp25C,_tmp25B,_tag_dyneither(_tmp15A,sizeof(void*),3U));});});});});});
return 0;}
# 681
if(!({struct Cyc_List_List*_tmp262=_tmp167->tvs;struct Cyc_List_List*_tmp261=_tmp165->tvs;struct _dyneither_ptr _tmp260=({const char*_tmp15F="@extensible datatype";_tag_dyneither(_tmp15F,sizeof(char),21U);});struct _dyneither_ptr _tmp25F=Cyc_Absynpp_qvar2string(_tmp167->name);unsigned int _tmp25E=loc;Cyc_Tcdecl_check_tvs(_tmp262,_tmp261,_tmp260,_tmp25F,_tmp25E,msg);}))
return 0;{
# 684
struct Cyc_List_List*_tmp160=Cyc_Tcdecl_build_tvs_map(_tmp167->tvs,_tmp165->tvs);
# 687
struct Cyc_Absyn_Datatypefield*_tmp161=({struct Cyc_Absyn_Datatypefield*_tmp267=_tmp166;struct Cyc_Absyn_Datatypefield*_tmp266=_tmp164;struct Cyc_List_List*_tmp265=_tmp160;struct _dyneither_ptr _tmp264=({const char*_tmp163="@extensible datatype field";_tag_dyneither(_tmp163,sizeof(char),27U);});struct _dyneither_ptr _tmp263=_tmp159;Cyc_Tcdecl_merge_datatypefield(_tmp267,_tmp266,_tmp265,_tmp264,_tmp263,msg);});
if(_tmp161 == 0)return 0;
if(_tmp161 == _tmp166)return d0;
return({struct Cyc_Tcdecl_Xdatatypefielddecl*_tmp162=_cycalloc(sizeof(*_tmp162));((*_tmp162).base=_tmp167,(*_tmp162).field=_tmp161);_tmp162;});};};};}
