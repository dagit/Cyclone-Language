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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178 "list.h"
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 50 "string.h"
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 940
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 960
extern void*Cyc_Absyn_empty_effect;
# 962
extern struct _tuple0*Cyc_Absyn_exn_name;
extern struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud;
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);struct Cyc_RgnOrder_RgnPO;
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
# 64 "tcenv.h"
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg);
# 66
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init();
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned int,struct Cyc_Absyn_Fndecl*);
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 70
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args);
# 74
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple0*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 88
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 92
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 98
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 102
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 110
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te);
# 119
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple10{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 123
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 126
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned int);struct _tuple11{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc);
# 133
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 137
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 139
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 141
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 144
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 147
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 94
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 129
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 132
struct _tuple11 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 166
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 170
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 223 "tcutil.h"
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 290
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 292
int Cyc_Tcutil_new_tvar_id();
# 294
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);char Cyc_Tcenv_Env_error[10U]="Env_error";
# 50 "tcenv.cyc"
struct Cyc_Tcenv_Env_error_exn_struct Cyc_Tcenv_Env_error_val={Cyc_Tcenv_Env_error};
# 52
void*Cyc_Tcenv_env_err(struct _dyneither_ptr msg){
(int)_throw((void*)& Cyc_Tcenv_Env_error_val);}struct Cyc_Tcenv_Tenv;struct Cyc_Tcenv_Genv;struct Cyc_Tcenv_SharedFenv{void*return_typ;struct Cyc_List_List*delayed_effect_checks;struct Cyc_List_List*delayed_constraint_checks;};struct Cyc_Tcenv_FenvFlags{enum Cyc_Tcenv_NewStatus in_new;int in_notreadctxt: 1;int in_lhs: 1;int abstract_ok: 1;int in_stmt_exp: 1;};struct Cyc_Tcenv_Fenv{struct Cyc_Tcenv_SharedFenv*shared;struct Cyc_List_List*type_vars;struct Cyc_RgnOrder_RgnPO*region_order;const struct _tuple10*ctrl_env;void*capability;void*curr_rgn;struct Cyc_Tcenv_FenvFlags flags;};
# 110
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init(){
# 112
struct Cyc_Tcenv_Genv*_tmp1=({struct Cyc_Tcenv_Genv*_tmp4=_cycalloc(sizeof(*_tmp4));({struct Cyc_Dict_Dict _tmpFD=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp4->aggrdecls=_tmpFD;});({struct Cyc_Dict_Dict _tmpFE=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp4->datatypedecls=_tmpFE;});({struct Cyc_Dict_Dict _tmpFF=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp4->enumdecls=_tmpFF;});({struct Cyc_Dict_Dict _tmp100=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp4->typedefs=_tmp100;});({struct Cyc_Dict_Dict _tmp101=
((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _tuple0*,struct _tuple0*)))Cyc_Dict_empty)(Cyc_Absyn_qvar_cmp);_tmp4->ordinaries=_tmp101;});_tmp4;});
({struct Cyc_Dict_Dict _tmp104=({struct Cyc_Dict_Dict _tmp103=_tmp1->datatypedecls;struct _tuple0*_tmp102=Cyc_Absyn_exn_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple0*k,struct Cyc_Absyn_Datatypedecl**v))Cyc_Dict_insert)(_tmp103,_tmp102,({struct Cyc_Absyn_Datatypedecl**_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2[0]=Cyc_Absyn_exn_tud;_tmp2;}));});_tmp1->datatypedecls=_tmp104;});
return({struct Cyc_Tcenv_Tenv*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->ns=0;_tmp3->ae=_tmp1;_tmp3->le=0;_tmp3->allow_valueof=0;_tmp3->in_extern_c_include=0;_tmp3;});}struct _tuple12{void*f1;int f2;};
# 120
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q,int is_use){
struct _tuple12*_tmp5=((struct _tuple12*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->ordinaries,q);
if(is_use)
(*_tmp5).f2=1;
return(*_tmp5).f1;}
# 126
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Aggrdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->aggrdecls,q);}
# 129
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);}
# 132
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return({struct Cyc_Absyn_Datatypedecl***_tmp6=_region_malloc(r,sizeof(*_tmp6));({struct Cyc_Absyn_Datatypedecl**_tmp105=((struct Cyc_Absyn_Datatypedecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->datatypedecls,q);_tmp6[0]=_tmp105;});_tmp6;});}
# 135
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->enumdecls,q);}
# 138
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct _tuple0*q){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _tuple0*k))Cyc_Dict_lookup)((te->ae)->typedefs,q);}
# 144
static struct Cyc_Tcenv_Fenv*Cyc_Tcenv_get_fenv(struct Cyc_Tcenv_Tenv*te,struct _dyneither_ptr err_msg){
struct Cyc_Tcenv_Fenv*_tmp7=te->le;
if(_tmp7 == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(err_msg);
return _tmp7;}
# 149
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_fenv(struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcenv_Fenv*fe){
if(te->le == 0)((int(*)(struct _dyneither_ptr msg))Cyc_Tcenv_env_err)(({const char*_tmp8="put_fenv";_tag_dyneither(_tmp8,sizeof(char),9U);}));{
struct Cyc_Tcenv_Tenv*_tmp9=({struct Cyc_Tcenv_Tenv*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA[0]=*te;_tmpA;});
_tmp9->le=fe;
return _tmp9;};}
# 155
static struct Cyc_Tcenv_Tenv*Cyc_Tcenv_put_emptyfenv(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Tenv*_tmpB=({struct Cyc_Tcenv_Tenv*_tmpC=_cycalloc(sizeof(*_tmpC));_tmpC[0]=*te;_tmpC;});
_tmpB->le=0;
return _tmpB;}
# 161
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*te){
return(({struct Cyc_Tcenv_Tenv*_tmp106=te;Cyc_Tcenv_get_fenv(_tmp106,({const char*_tmpD="return_typ";_tag_dyneither(_tmpD,sizeof(char),11U);}));})->shared)->return_typ;}
# 165
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmpE=te->le;
if(te->le == 0)return 0;
return((struct Cyc_Tcenv_Fenv*)_check_null(_tmpE))->type_vars;}
# 171
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te){
return({struct Cyc_Core_Opt*_tmpF=_cycalloc(sizeof(*_tmpF));({struct Cyc_List_List*_tmp107=Cyc_Tcenv_lookup_type_vars(te);_tmpF->v=_tmp107;});_tmpF;});}
# 175
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tvs){
struct Cyc_Tcenv_Fenv*_tmp10=({struct Cyc_Tcenv_Fenv*_tmp12=_cycalloc(sizeof(*_tmp12));({struct Cyc_Tcenv_Fenv _tmp109=*({struct Cyc_Tcenv_Tenv*_tmp108=te;Cyc_Tcenv_get_fenv(_tmp108,({const char*_tmp13="add_type_vars";_tag_dyneither(_tmp13,sizeof(char),14U);}));});_tmp12[0]=_tmp109;});_tmp12;});
Cyc_Tcutil_add_tvar_identities(tvs);{
struct Cyc_List_List*_tmp11=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(tvs,_tmp10->type_vars);
Cyc_Tcutil_check_unique_tvars(loc,_tmp11);
_tmp10->type_vars=_tmp11;
return Cyc_Tcenv_put_fenv(te,_tmp10);};}
# 184
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp14=te->le;
if(_tmp14 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp15=({struct Cyc_Tcenv_Fenv*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16[0]=*_tmp14;_tmp16;});
(_tmp15->flags).in_new=status;
return Cyc_Tcenv_put_fenv(te,_tmp15);};}
# 191
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp17=te->le;
if(_tmp17 == 0)return Cyc_Tcenv_NoneNew;
return(_tmp17->flags).in_new;}
# 196
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp18=te->le;
if(_tmp18 == 0)return 0;
return(_tmp18->flags).abstract_ok;}
# 201
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp19=te->le;
if(_tmp19 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1A=({struct Cyc_Tcenv_Fenv*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B[0]=*_tmp19;_tmp1B;});
(_tmp1A->flags).abstract_ok=1;
return Cyc_Tcenv_put_fenv(te,_tmp1A);};}
# 208
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1C=te->le;
if(_tmp1C == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp1D=({struct Cyc_Tcenv_Fenv*_tmp1E=_cycalloc(sizeof(*_tmp1E));_tmp1E[0]=*_tmp1C;_tmp1E;});
(_tmp1D->flags).abstract_ok=0;
return Cyc_Tcenv_put_fenv(te,_tmp1D);};}
# 215
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp1F=te->le;
if(_tmp1F == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp20=({struct Cyc_Tcenv_Fenv*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21[0]=*_tmp1F;_tmp21;});
(_tmp20->flags).in_notreadctxt=1;
return Cyc_Tcenv_put_fenv(te,_tmp20);};}
# 222
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp22=te->le;
if(_tmp22 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp23=({struct Cyc_Tcenv_Fenv*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24[0]=*_tmp22;_tmp24;});
(_tmp23->flags).in_notreadctxt=0;
return Cyc_Tcenv_put_fenv(te,_tmp23);};}
# 229
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp25=te->le;
if(_tmp25 == 0)return 0;
return(_tmp25->flags).in_notreadctxt;}
# 234
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp26=te->le;
if(_tmp26 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp27=({struct Cyc_Tcenv_Fenv*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28[0]=*_tmp26;_tmp28;});
(_tmp27->flags).in_lhs=1;
return Cyc_Tcenv_put_fenv(te,_tmp27);};}
# 241
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp29=te->le;
if(_tmp29 == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp2A=({struct Cyc_Tcenv_Fenv*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B[0]=*_tmp29;_tmp2B;});
(_tmp2A->flags).in_lhs=0;
return Cyc_Tcenv_put_fenv(te,_tmp2A);};}
# 248
int Cyc_Tcenv_in_lhs(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2C=te->le;
if(_tmp2C == 0)return 0;
return(_tmp2C->flags).in_lhs;}
# 253
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp2D=te->le;
if(_tmp2D == 0)return Cyc_Tcenv_put_emptyfenv(te);{
struct Cyc_Tcenv_Fenv*_tmp2E=({struct Cyc_Tcenv_Fenv*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F[0]=*_tmp2D;_tmp2F;});
(_tmp2E->flags).in_stmt_exp=1;
return Cyc_Tcenv_put_fenv(te,_tmp2E);};}
# 260
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp30=te->le;
if(_tmp30 == 0)return 0;
return(_tmp30->flags).in_stmt_exp;}
# 267
const struct _tuple10*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Switch_clause***clauseopt){
# 269
const struct _tuple10*_tmp31=({struct Cyc_Tcenv_Tenv*_tmp10A=te;Cyc_Tcenv_get_fenv(_tmp10A,({const char*_tmp33="process_fallthru";_tag_dyneither(_tmp33,sizeof(char),17U);}));})->ctrl_env;
if(_tmp31 != (const struct _tuple10*)0)
({struct Cyc_Absyn_Switch_clause**_tmp10B=({struct Cyc_Absyn_Switch_clause**_tmp32=_cycalloc(sizeof(*_tmp32));_tmp32[0]=(*_tmp31).f1;_tmp32;});*clauseopt=_tmp10B;});
return _tmp31;}
# 275
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause){
# 277
struct Cyc_List_List*ft_typ=0;
for(0;vds != 0;vds=vds->tl){
({struct Cyc_List_List*_tmp10C=({struct Cyc_List_List*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->hd=((struct Cyc_Absyn_Vardecl*)vds->hd)->type;_tmp34->tl=ft_typ;_tmp34;});ft_typ=_tmp10C;});}{
struct _tuple10*_tmp35=({struct _tuple10*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->f1=clause;_tmp39->f2=new_tvs;({struct Cyc_List_List*_tmp10D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ft_typ);_tmp39->f3=_tmp10D;});_tmp39;});
struct Cyc_Tcenv_Fenv*_tmp36=({struct Cyc_Tcenv_Fenv*_tmp37=_cycalloc(sizeof(*_tmp37));({struct Cyc_Tcenv_Fenv _tmp10F=*({struct Cyc_Tcenv_Tenv*_tmp10E=te;Cyc_Tcenv_get_fenv(_tmp10E,({const char*_tmp38="set_fallthru";_tag_dyneither(_tmp38,sizeof(char),13U);}));});_tmp37[0]=_tmp10F;});_tmp37;});
_tmp36->ctrl_env=(const struct _tuple10*)_tmp35;
return Cyc_Tcenv_put_fenv(te,_tmp36);};}
# 286
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Fenv*_tmp3A=_cycalloc(sizeof(*_tmp3A));({struct Cyc_Tcenv_Fenv _tmp111=*({struct Cyc_Tcenv_Tenv*_tmp110=te;Cyc_Tcenv_get_fenv(_tmp110,({const char*_tmp3B="clear_fallthru";_tag_dyneither(_tmp3B,sizeof(char),15U);}));});_tmp3A[0]=_tmp111;});_tmp3A;});
fe->ctrl_env=0;
return Cyc_Tcenv_put_fenv(te,fe);}
# 292
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp3C=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F[0]=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp3F;});
struct Cyc_Tcenv_Tenv*_tmp3D=({struct Cyc_Tcenv_Tenv*_tmp3E=_cycalloc(sizeof(*_tmp3E));_tmp3E[0]=*te;_tmp3E;});
_tmp3D->allow_valueof=1;
return _tmp3D;}
# 298
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp40=te->le == 0?0:({struct Cyc_Tcenv_Fenv*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43[0]=*((struct Cyc_Tcenv_Fenv*)_check_null(te->le));_tmp43;});
struct Cyc_Tcenv_Tenv*_tmp41=({struct Cyc_Tcenv_Tenv*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42[0]=*te;_tmp42;});
_tmp41->in_extern_c_include=1;
return _tmp41;}
# 305
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*te,void*rgn,int opened){
struct Cyc_Tcenv_Fenv*_tmp44=({struct Cyc_Tcenv_Fenv*_tmp4F=_cycalloc(sizeof(*_tmp4F));({struct Cyc_Tcenv_Fenv _tmp113=*({struct Cyc_Tcenv_Tenv*_tmp112=te;Cyc_Tcenv_get_fenv(_tmp112,({const char*_tmp50="add_region";_tag_dyneither(_tmp50,sizeof(char),11U);}));});_tmp4F[0]=_tmp113;});_tmp4F;});
struct Cyc_RgnOrder_RgnPO*_tmp45=_tmp44->region_order;
{void*_tmp46=Cyc_Tcutil_compress(rgn);void*_tmp47=_tmp46;struct Cyc_Absyn_Tvar*_tmp48;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp47)->tag == 2U){_LL1: _tmp48=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp47)->f1;_LL2:
# 310
({struct Cyc_RgnOrder_RgnPO*_tmp114=Cyc_RgnOrder_add_youngest(_tmp45,_tmp48,opened);_tmp45=_tmp114;});
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 314
_tmp44->region_order=_tmp45;
({void*_tmp11A=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp119=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp4A;_tmp4A.tag=24U;({struct Cyc_List_List*_tmp118=({struct Cyc_List_List*_tmp4B=_cycalloc(sizeof(*_tmp4B));({void*_tmp116=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp115=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp4E;_tmp4E.tag=23U;_tmp4E.f1=rgn;_tmp4E;});_tmp4D[0]=_tmp115;});_tmp4D;});_tmp4B->hd=_tmp116;});({struct Cyc_List_List*_tmp117=({struct Cyc_List_List*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->hd=_tmp44->capability;_tmp4C->tl=0;_tmp4C;});_tmp4B->tl=_tmp117;});_tmp4B;});_tmp4A.f1=_tmp118;});_tmp4A;});_tmp49[0]=_tmp119;});_tmp49;});_tmp44->capability=_tmp11A;});
# 317
return Cyc_Tcenv_put_fenv(te,_tmp44);}
# 320
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Tvar*block_rgn){
struct Cyc_Tcenv_Fenv*_tmp51=({struct Cyc_Tcenv_Fenv*_tmp5C=_cycalloc(sizeof(*_tmp5C));({struct Cyc_Tcenv_Fenv _tmp11C=*({struct Cyc_Tcenv_Tenv*_tmp11B=te;Cyc_Tcenv_get_fenv(_tmp11B,({const char*_tmp5D="new_named_block";_tag_dyneither(_tmp5D,sizeof(char),16U);}));});_tmp5C[0]=_tmp11C;});_tmp5C;});
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp52=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp11D=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp5B;_tmp5B.tag=2U;_tmp5B.f1=block_rgn;_tmp5B;});_tmp5A[0]=_tmp11D;});_tmp5A;});
({struct Cyc_List_List*_tmp11E=({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=block_rgn;_tmp53->tl=_tmp51->type_vars;_tmp53;});_tmp51->type_vars=_tmp11E;});
Cyc_Tcutil_check_unique_tvars(loc,_tmp51->type_vars);
({struct Cyc_RgnOrder_RgnPO*_tmp11F=Cyc_RgnOrder_add_youngest(_tmp51->region_order,block_rgn,0);_tmp51->region_order=_tmp11F;});
({void*_tmp125=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp54=_cycalloc(sizeof(*_tmp54));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp124=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp55;_tmp55.tag=24U;({struct Cyc_List_List*_tmp123=({struct Cyc_List_List*_tmp56=_cycalloc(sizeof(*_tmp56));({void*_tmp121=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp58=_cycalloc(sizeof(*_tmp58));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp120=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp59;_tmp59.tag=23U;_tmp59.f1=(void*)_tmp52;_tmp59;});_tmp58[0]=_tmp120;});_tmp58;});_tmp56->hd=_tmp121;});({struct Cyc_List_List*_tmp122=({struct Cyc_List_List*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->hd=_tmp51->capability;_tmp57->tl=0;_tmp57;});_tmp56->tl=_tmp122;});_tmp56;});_tmp55.f1=_tmp123;});_tmp55;});_tmp54[0]=_tmp124;});_tmp54;});_tmp51->capability=_tmp125;});
# 328
_tmp51->curr_rgn=(void*)_tmp52;
return Cyc_Tcenv_put_fenv(te,_tmp51);}
# 332
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct Cyc_Tcenv_rgn_kb={0U,& Cyc_Tcutil_rk};
# 334
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int loc,struct Cyc_Tcenv_Tenv*te){
struct Cyc_Absyn_Tvar*t=Cyc_Tcutil_new_tvar((void*)& Cyc_Tcenv_rgn_kb);
Cyc_Tcutil_add_tvar_identity(t);
return Cyc_Tcenv_new_named_block(loc,te,t);}struct _tuple13{void*f1;void*f2;};
# 340
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*cs,unsigned int loc){
# 342
struct Cyc_Tcenv_Fenv*_tmp5F=({struct Cyc_Tcenv_Fenv*_tmp60=_cycalloc(sizeof(*_tmp60));({struct Cyc_Tcenv_Fenv _tmp127=*({struct Cyc_Tcenv_Tenv*_tmp126=te;Cyc_Tcenv_get_fenv(_tmp126,({const char*_tmp61="new_outlives_constraints";_tag_dyneither(_tmp61,sizeof(char),25U);}));});_tmp60[0]=_tmp127;});_tmp60;});
for(0;cs != 0;cs=cs->tl){
({struct Cyc_RgnOrder_RgnPO*_tmp128=
Cyc_RgnOrder_add_outlives_constraint(_tmp5F->region_order,(*((struct _tuple13*)cs->hd)).f1,(*((struct _tuple13*)cs->hd)).f2,loc);
# 344
_tmp5F->region_order=_tmp128;});}
# 346
return Cyc_Tcenv_put_fenv(te,_tmp5F);}
# 349
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple11**oldtv,unsigned int loc){
# 353
void*_tmp62=Cyc_Tcutil_compress(r1);
void*_tmp63=Cyc_Tcutil_compress(r2);
struct Cyc_Absyn_Kind*_tmp64=Cyc_Tcutil_typ_kind(_tmp62);
struct Cyc_Absyn_Kind*_tmp65=Cyc_Tcutil_typ_kind(_tmp63);
# 359
int r1_le_r2=Cyc_Tcutil_kind_leq(_tmp64,_tmp65);
int r2_le_r1=Cyc_Tcutil_kind_leq(_tmp65,_tmp64);
if(!r1_le_r2  && !r2_le_r1){
({struct Cyc_String_pa_PrintArg_struct _tmp6B;_tmp6B.tag=0U;({struct _dyneither_ptr _tmp129=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(_tmp65));_tmp6B.f1=_tmp129;});({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0U;({struct _dyneither_ptr _tmp12A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(_tmp64));_tmp6A.f1=_tmp12A;});({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0U;({struct _dyneither_ptr _tmp12B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp63));_tmp69.f1=_tmp12B;});({struct Cyc_String_pa_PrintArg_struct _tmp68;_tmp68.tag=0U;({struct _dyneither_ptr _tmp12C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp62));_tmp68.f1=_tmp12C;});({void*_tmp66[4U]={& _tmp68,& _tmp69,& _tmp6A,& _tmp6B};({unsigned int _tmp12E=loc;struct _dyneither_ptr _tmp12D=({const char*_tmp67="Cannot compare region handles for %s and %s\n  kinds %s and %s are not compatible\n";_tag_dyneither(_tmp67,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp12E,_tmp12D,_tag_dyneither(_tmp66,sizeof(void*),4U));});});});});});});
return Cyc_Tcenv_new_outlives_constraints(te,0,loc);}else{
# 366
if(r1_le_r2  && !r2_le_r1)
({struct _tuple11*_tmp131=({struct _tuple11*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct _tuple11 _tmp130=({void*_tmp12F=_tmp63;Cyc_Tcutil_swap_kind(_tmp12F,Cyc_Tcutil_kind_to_bound(_tmp64));});_tmp6C[0]=_tmp130;});_tmp6C;});*oldtv=_tmp131;});else{
if(!r1_le_r2  && r2_le_r1)
({struct _tuple11*_tmp134=({struct _tuple11*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct _tuple11 _tmp133=({void*_tmp132=_tmp62;Cyc_Tcutil_swap_kind(_tmp132,Cyc_Tcutil_kind_to_bound(_tmp65));});_tmp6D[0]=_tmp133;});_tmp6D;});*oldtv=_tmp134;});}}{
# 372
struct Cyc_List_List*_tmp6E=0;
if((_tmp62 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp62 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp62 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff1=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp135=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp72;_tmp72.tag=23U;_tmp72.f1=_tmp62;_tmp72;});_tmp71[0]=_tmp135;});_tmp71;});
({struct Cyc_List_List*_tmp137=({struct Cyc_List_List*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct _tuple13*_tmp136=({struct _tuple13*_tmp70=_cycalloc(sizeof(*_tmp70));_tmp70->f1=eff1;_tmp70->f2=_tmp63;_tmp70;});_tmp6F->hd=_tmp136;});_tmp6F->tl=_tmp6E;_tmp6F;});_tmp6E=_tmp137;});}
# 377
if((_tmp63 != (void*)& Cyc_Absyn_HeapRgn_val  && _tmp63 != (void*)& Cyc_Absyn_UniqueRgn_val) && _tmp63 != (void*)& Cyc_Absyn_RefCntRgn_val){
void*eff2=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp75=_cycalloc(sizeof(*_tmp75));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp138=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp76;_tmp76.tag=23U;_tmp76.f1=_tmp63;_tmp76;});_tmp75[0]=_tmp138;});_tmp75;});
({struct Cyc_List_List*_tmp13A=({struct Cyc_List_List*_tmp73=_cycalloc(sizeof(*_tmp73));({struct _tuple13*_tmp139=({struct _tuple13*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74->f1=eff2;_tmp74->f2=_tmp62;_tmp74;});_tmp73->hd=_tmp139;});_tmp73->tl=_tmp6E;_tmp73;});_tmp6E=_tmp13A;});}
# 381
return Cyc_Tcenv_new_outlives_constraints(te,_tmp6E,loc);};}
# 384
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*te){
struct Cyc_Tcenv_Fenv*_tmp77=te->le;
if(_tmp77 == 0)return(void*)& Cyc_Absyn_HeapRgn_val;
return _tmp77->curr_rgn;}
# 393
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*rgn){
struct Cyc_Tcenv_Fenv*fe=({struct Cyc_Tcenv_Tenv*_tmp13B=te;Cyc_Tcenv_get_fenv(_tmp13B,({const char*_tmp80="check_rgn_accessible";_tag_dyneither(_tmp80,sizeof(char),21U);}));});
struct Cyc_Tcenv_Fenv*_tmp78=fe;void*_tmp7F;struct Cyc_RgnOrder_RgnPO*_tmp7E;_LL1: _tmp7F=_tmp78->capability;_tmp7E=_tmp78->region_order;_LL2:;
if(Cyc_Tcutil_region_in_effect(0,rgn,_tmp7F) || 
Cyc_Tcutil_region_in_effect(1,rgn,_tmp7F))
return;
if(({struct Cyc_RgnOrder_RgnPO*_tmp13E=_tmp7E;void*_tmp13D=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp79=_cycalloc(sizeof(*_tmp79));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp13C=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp7A;_tmp7A.tag=23U;_tmp7A.f1=rgn;_tmp7A;});_tmp79[0]=_tmp13C;});_tmp79;});Cyc_RgnOrder_eff_outlives_eff(_tmp13E,_tmp13D,_tmp7F);}))
return;
({struct Cyc_String_pa_PrintArg_struct _tmp7D;_tmp7D.tag=0U;({struct _dyneither_ptr _tmp13F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(rgn));_tmp7D.f1=_tmp13F;});({void*_tmp7B[1U]={& _tmp7D};({unsigned int _tmp141=loc;struct _dyneither_ptr _tmp140=({const char*_tmp7C="Expression accesses unavailable region %s";_tag_dyneither(_tmp7C,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp141,_tmp140,_tag_dyneither(_tmp7B,sizeof(void*),1U));});});});}
# 407
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*te,void*rt_a,void*rt_b){
struct Cyc_Tcenv_Fenv*_tmp81=te->le;
({void*_tmp142=Cyc_Tcutil_compress(rt_a);rt_a=_tmp142;});
({void*_tmp143=Cyc_Tcutil_compress(rt_b);rt_b=_tmp143;});
# 412
if(_tmp81 == 0){
void*_tmp82=rt_a;switch(*((int*)_tmp82)){case 22U: _LL1: _LL2:
 return rt_b != (void*)& Cyc_Absyn_UniqueRgn_val;case 21U: _LL3: _LL4:
 return rt_b != (void*)& Cyc_Absyn_RefCntRgn_val;case 20U: _LL5: _LL6:
 return rt_b == (void*)& Cyc_Absyn_HeapRgn_val;default: _LL7: _LL8:
 return 0;}_LL0:;}{
# 420
int _tmp83=({struct Cyc_RgnOrder_RgnPO*_tmp146=_tmp81->region_order;void*_tmp145=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp84=_cycalloc(sizeof(*_tmp84));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp144=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp85;_tmp85.tag=23U;_tmp85.f1=rt_a;_tmp85;});_tmp84[0]=_tmp144;});_tmp84;});Cyc_RgnOrder_effect_outlives(_tmp146,_tmp145,rt_b);});
# 424
return _tmp83;};}struct _tuple14{void*f1;void*f2;struct Cyc_RgnOrder_RgnPO*f3;unsigned int f4;};
# 429
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff){
struct Cyc_Tcenv_Fenv*_tmp86=({
struct Cyc_Tcenv_Tenv*_tmp147=te;Cyc_Tcenv_get_fenv(_tmp147,({const char*_tmp8D="check_effect_accessible";_tag_dyneither(_tmp8D,sizeof(char),24U);}));});
# 430
struct Cyc_Tcenv_Fenv*_tmp87=_tmp86;void*_tmp8C;struct Cyc_RgnOrder_RgnPO*_tmp8B;struct Cyc_Tcenv_SharedFenv*_tmp8A;_LL1: _tmp8C=_tmp87->capability;_tmp8B=_tmp87->region_order;_tmp8A=_tmp87->shared;_LL2:;
# 432
if(Cyc_Tcutil_subset_effect(0,eff,_tmp8C))
return;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp8B,eff,_tmp8C))
return;
({struct Cyc_List_List*_tmp149=({struct Cyc_List_List*_tmp88=_cycalloc(sizeof(*_tmp88));({struct _tuple14*_tmp148=({struct _tuple14*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->f1=_tmp8C;_tmp89->f2=eff;_tmp89->f3=_tmp8B;_tmp89->f4=loc;_tmp89;});_tmp88->hd=_tmp148;});_tmp88->tl=_tmp8A->delayed_effect_checks;_tmp88;});_tmp8A->delayed_effect_checks=_tmp149;});}
# 439
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmp8E=(({
struct Cyc_Tcenv_Tenv*_tmp14A=te;Cyc_Tcenv_get_fenv(_tmp14A,({const char*_tmp99="check_delayed_constraints";_tag_dyneither(_tmp99,sizeof(char),26U);}));})->shared)->delayed_effect_checks;
for(0;_tmp8E != 0;_tmp8E=_tmp8E->tl){
struct _tuple14*_tmp8F=(struct _tuple14*)_tmp8E->hd;struct _tuple14*_tmp90=_tmp8F;void*_tmp98;void*_tmp97;struct Cyc_RgnOrder_RgnPO*_tmp96;unsigned int _tmp95;_LL1: _tmp98=_tmp90->f1;_tmp97=_tmp90->f2;_tmp96=_tmp90->f3;_tmp95=_tmp90->f4;_LL2:;
if(Cyc_Tcutil_subset_effect(1,_tmp97,_tmp98))
continue;
if(Cyc_RgnOrder_eff_outlives_eff(_tmp96,_tmp97,_tmp98))
continue;
({struct Cyc_String_pa_PrintArg_struct _tmp94;_tmp94.tag=0U;({struct _dyneither_ptr _tmp14B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp97));_tmp94.f1=_tmp14B;});({struct Cyc_String_pa_PrintArg_struct _tmp93;_tmp93.tag=0U;({struct _dyneither_ptr _tmp14C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp98));_tmp93.f1=_tmp14C;});({void*_tmp91[2U]={& _tmp93,& _tmp94};({unsigned int _tmp14E=_tmp95;struct _dyneither_ptr _tmp14D=({const char*_tmp92="Capability \n%s\ndoes not cover function's effect\n%s";_tag_dyneither(_tmp92,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp14E,_tmp14D,_tag_dyneither(_tmp91,sizeof(void*),2U));});});});});}}struct _tuple15{struct Cyc_RgnOrder_RgnPO*f1;struct Cyc_List_List*f2;unsigned int f3;};
# 457
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po){
# 459
struct Cyc_Tcenv_Fenv*_tmp9A=te->le;
if(_tmp9A == 0){
# 462
for(0;po != 0;po=po->tl){
if(!Cyc_Tcutil_subset_effect(1,(*((struct _tuple13*)po->hd)).f1,Cyc_Absyn_empty_effect) || !({
void*_tmp150=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp14F=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp9C;_tmp9C.tag=23U;_tmp9C.f1=(*((struct _tuple13*)po->hd)).f2;_tmp9C;});_tmp9B[0]=_tmp14F;});_tmp9B;});Cyc_Tcutil_subset_effect(1,_tmp150,Cyc_Absyn_empty_effect);}))
({void*_tmp9D=0U;({unsigned int _tmp152=loc;struct _dyneither_ptr _tmp151=({const char*_tmp9E="the required region ordering is not satisfied here";_tag_dyneither(_tmp9E,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp152,_tmp151,_tag_dyneither(_tmp9D,sizeof(void*),0U));});});}
return;}{
# 468
struct Cyc_Tcenv_Fenv*_tmp9F=_tmp9A;struct Cyc_Tcenv_Fenv*_tmpA0=_tmp9F;struct Cyc_RgnOrder_RgnPO*_tmpA4;struct Cyc_Tcenv_SharedFenv*_tmpA3;_LL1: _tmpA4=_tmpA0->region_order;_tmpA3=_tmpA0->shared;_LL2:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmpA4,po,(void*)& Cyc_Absyn_HeapRgn_val,0))
({struct Cyc_List_List*_tmp154=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*_tmpA1));({struct _tuple15*_tmp153=({struct _tuple15*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->f1=_tmpA4;_tmpA2->f2=po;_tmpA2->f3=loc;_tmpA2;});_tmpA1->hd=_tmp153;});_tmpA1->tl=_tmpA3->delayed_constraint_checks;_tmpA1;});_tmpA3->delayed_constraint_checks=_tmp154;});};}
# 474
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te){
struct Cyc_List_List*_tmpA5=(({
struct Cyc_Tcenv_Tenv*_tmp155=te;Cyc_Tcenv_get_fenv(_tmp155,({const char*_tmpAD="check_delayed_constraints";_tag_dyneither(_tmpAD,sizeof(char),26U);}));})->shared)->delayed_constraint_checks;
for(0;_tmpA5 != 0;_tmpA5=_tmpA5->tl){
struct _tuple15*_tmpA6=(struct _tuple15*)_tmpA5->hd;struct _tuple15*_tmpA7=_tmpA6;struct Cyc_RgnOrder_RgnPO*_tmpAC;struct Cyc_List_List*_tmpAB;unsigned int _tmpAA;_LL1: _tmpAC=_tmpA7->f1;_tmpAB=_tmpA7->f2;_tmpAA=_tmpA7->f3;_LL2:;
if(!Cyc_RgnOrder_satisfies_constraints(_tmpAC,_tmpAB,(void*)& Cyc_Absyn_HeapRgn_val,1))
({void*_tmpA8=0U;({unsigned int _tmp157=_tmpAA;struct _dyneither_ptr _tmp156=({const char*_tmpA9="the required region ordering is not satisfied here";_tag_dyneither(_tmpA9,sizeof(char),51U);});Cyc_Tcutil_terr(_tmp157,_tmp156,_tag_dyneither(_tmpA8,sizeof(void*),0U));});});}}
# 484
static struct Cyc_Tcenv_SharedFenv*Cyc_Tcenv_new_shared_fenv(void*ret){
return({struct Cyc_Tcenv_SharedFenv*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->return_typ=ret;_tmpAE->delayed_effect_checks=0;_tmpAE->delayed_constraint_checks=0;_tmpAE;});}
# 490
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned int loc,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpC4=_cycalloc(sizeof(*_tmpC4));({struct _dyneither_ptr*_tmp15A=({struct _dyneither_ptr*_tmpC5=_cycalloc(sizeof(*_tmpC5));({struct _dyneither_ptr _tmp159=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC8;_tmpC8.tag=0U;_tmpC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmpC6[1U]={& _tmpC8};({struct _dyneither_ptr _tmp158=({const char*_tmpC7="`%s";_tag_dyneither(_tmpC7,sizeof(char),4U);});Cyc_aprintf(_tmp158,_tag_dyneither(_tmpC6,sizeof(void*),1U));});});});_tmpC5[0]=_tmp159;});_tmpC5;});_tmpC4->name=_tmp15A;});({int _tmp15B=
# 493
Cyc_Tcutil_new_tvar_id();_tmpC4->identity=_tmp15B;});_tmpC4->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpC4;});
struct Cyc_List_List*_tmpAF=({struct Cyc_List_List*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->hd=rgn0;_tmpC3->tl=fd->tvs;_tmpC3;});
Cyc_Tcutil_check_unique_tvars(loc,_tmpAF);{
# 497
struct Cyc_RgnOrder_RgnPO*_tmpB0=Cyc_RgnOrder_initial_fn_po(fd->tvs,fd->rgn_po,(void*)_check_null(fd->effect),rgn0,loc);
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp15C=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC2;_tmpC2.tag=2U;_tmpC2.f1=rgn0;_tmpC2;});_tmpC1[0]=_tmp15C;});_tmpC1;});
{struct Cyc_List_List*_tmpB1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpB1 != 0;_tmpB1=_tmpB1->tl){
((struct Cyc_Absyn_Vardecl*)_tmpB1->hd)->rgn=param_rgn;}}
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpB2=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmpB3=_tmpB2;struct _dyneither_ptr*_tmpB9;struct Cyc_Absyn_Tqual _tmpB8;void*_tmpB7;int _tmpB6;_LL1: _tmpB9=_tmpB3.name;_tmpB8=_tmpB3.tq;_tmpB7=_tmpB3.type;_tmpB6=_tmpB3.inject;_LL2:;{
# 504
struct Cyc_List_List*_tmpB4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpB4 != 0;_tmpB4=_tmpB4->tl){
struct Cyc_Absyn_Vardecl*_tmpB5=(struct Cyc_Absyn_Vardecl*)_tmpB4->hd;
if(Cyc_strptrcmp((*_tmpB5->name).f2,(struct _dyneither_ptr*)_check_null(_tmpB9))== 0){
({void*_tmp15D=Cyc_Absyn_dyneither_typ(_tmpB7,param_rgn,_tmpB8,Cyc_Absyn_false_conref);_tmpB5->type=_tmp15D;});
break;}}};}
# 512
return({struct Cyc_Tcenv_Fenv*_tmpBA=_cycalloc(sizeof(*_tmpBA));({struct Cyc_Tcenv_SharedFenv*_tmp15E=
Cyc_Tcenv_new_shared_fenv(fd->ret_type);_tmpBA->shared=_tmp15E;});_tmpBA->type_vars=_tmpAF;_tmpBA->region_order=_tmpB0;_tmpBA->ctrl_env=0;({void*_tmp164=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpBB=_cycalloc(sizeof(*_tmpBB));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp163=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpBC;_tmpBC.tag=24U;({struct Cyc_List_List*_tmp162=({struct Cyc_List_List*_tmpBD=_cycalloc(sizeof(*_tmpBD));({void*_tmp160=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp15F=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC0;_tmpC0.tag=23U;_tmpC0.f1=param_rgn;_tmpC0;});_tmpBF[0]=_tmp15F;});_tmpBF;});_tmpBD->hd=_tmp160;});({struct Cyc_List_List*_tmp161=({struct Cyc_List_List*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->hd=(void*)_check_null(fd->effect);_tmpBE->tl=0;_tmpBE;});_tmpBD->tl=_tmp161;});_tmpBD;});_tmpBC.f1=_tmp162;});_tmpBC;});_tmpBB[0]=_tmp163;});_tmpBB;});_tmpBA->capability=_tmp164;});_tmpBA->curr_rgn=param_rgn;({struct Cyc_Tcenv_FenvFlags _tmp165=({(_tmpBA->flags).in_new=Cyc_Tcenv_NoneNew;(_tmpBA->flags).in_notreadctxt=0;(_tmpBA->flags).in_lhs=0;(_tmpBA->flags).abstract_ok=0;(_tmpBA->flags).in_stmt_exp=0;_tmpBA->flags;});_tmpBA->flags=_tmp165;});_tmpBA;});};}
# 524
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int loc,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*fd){
struct Cyc_Tcenv_Fenv*_tmpC9=old_fenv;struct Cyc_RgnOrder_RgnPO*_tmpED;struct Cyc_List_List*_tmpEC;struct Cyc_Tcenv_SharedFenv*_tmpEB;_LL1: _tmpED=_tmpC9->region_order;_tmpEC=_tmpC9->type_vars;_tmpEB=_tmpC9->shared;_LL2:;{
# 527
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpE6=_cycalloc(sizeof(*_tmpE6));({struct _dyneither_ptr*_tmp168=({struct _dyneither_ptr*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct _dyneither_ptr _tmp167=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpEA;_tmpEA.tag=0U;_tmpEA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*fd->name).f2);({void*_tmpE8[1U]={& _tmpEA};({struct _dyneither_ptr _tmp166=({const char*_tmpE9="`%s";_tag_dyneither(_tmpE9,sizeof(char),4U);});Cyc_aprintf(_tmp166,_tag_dyneither(_tmpE8,sizeof(void*),1U));});});});_tmpE7[0]=_tmp167;});_tmpE7;});_tmpE6->name=_tmp168;});({int _tmp169=
# 529
Cyc_Tcutil_new_tvar_id();_tmpE6->identity=_tmp169;});_tmpE6->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpE6;});
{struct Cyc_List_List*_tmpCA=fd->tvs;for(0;_tmpCA != 0;_tmpCA=_tmpCA->tl){
struct Cyc_Absyn_Kind*_tmpCB=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmpCA->hd,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpCC=_tmpCB;enum Cyc_Absyn_KindQual _tmpD0;enum Cyc_Absyn_AliasQual _tmpCF;_LL4: _tmpD0=_tmpCC->kind;_tmpCF=_tmpCC->aliasqual;_LL5:;
if(_tmpD0 == Cyc_Absyn_RgnKind){
if(_tmpCF == Cyc_Absyn_Aliasable)
({struct Cyc_RgnOrder_RgnPO*_tmp16A=Cyc_RgnOrder_add_unordered(_tmpED,(struct Cyc_Absyn_Tvar*)_tmpCA->hd);_tmpED=_tmp16A;});else{
# 536
({void*_tmpCD=0U;({struct _dyneither_ptr _tmp16B=({const char*_tmpCE="non-intuitionistic tvar in nested_fenv";_tag_dyneither(_tmpCE,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp16B,_tag_dyneither(_tmpCD,sizeof(void*),0U));});});}}}}
# 538
({struct Cyc_RgnOrder_RgnPO*_tmp16C=Cyc_RgnOrder_add_youngest(_tmpED,rgn0,0);_tmpED=_tmp16C;});
{struct Cyc_List_List*_tmpD1=fd->rgn_po;for(0;_tmpD1 != 0;_tmpD1=_tmpD1->tl){
({struct Cyc_RgnOrder_RgnPO*_tmp16D=Cyc_RgnOrder_add_outlives_constraint(_tmpED,(*((struct _tuple13*)_tmpD1->hd)).f1,(*((struct _tuple13*)_tmpD1->hd)).f2,loc);_tmpED=_tmp16D;});}}{
struct Cyc_List_List*_tmpD2=({struct Cyc_List_List*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->hd=rgn0;({struct Cyc_List_List*_tmp16E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(fd->tvs,_tmpEC);_tmpE5->tl=_tmp16E;});_tmpE5;});
Cyc_Tcutil_check_unique_tvars(loc,_tmpD2);{
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp16F=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE4;_tmpE4.tag=2U;_tmpE4.f1=rgn0;_tmpE4;});_tmpE3[0]=_tmp16F;});_tmpE3;});
{struct Cyc_List_List*_tmpD3=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpD3 != 0;_tmpD3=_tmpD3->tl){
((struct Cyc_Absyn_Vardecl*)_tmpD3->hd)->rgn=param_rgn;}}
if(fd->cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmpD4=*((struct Cyc_Absyn_VarargInfo*)_check_null(fd->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmpD5=_tmpD4;struct _dyneither_ptr*_tmpDB;struct Cyc_Absyn_Tqual _tmpDA;void*_tmpD9;int _tmpD8;_LL7: _tmpDB=_tmpD5.name;_tmpDA=_tmpD5.tq;_tmpD9=_tmpD5.type;_tmpD8=_tmpD5.inject;_LL8:;{
# 549
struct Cyc_List_List*_tmpD6=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(fd->param_vardecls))->v;for(0;_tmpD6 != 0;_tmpD6=_tmpD6->tl){
struct Cyc_Absyn_Vardecl*_tmpD7=(struct Cyc_Absyn_Vardecl*)_tmpD6->hd;
if(Cyc_strptrcmp((*_tmpD7->name).f2,(struct _dyneither_ptr*)_check_null(_tmpDB))== 0){
({void*_tmp170=Cyc_Absyn_dyneither_typ(_tmpD9,param_rgn,_tmpDA,Cyc_Absyn_false_conref);_tmpD7->type=_tmp170;});
break;}}};}
# 557
return({struct Cyc_Tcenv_Fenv*_tmpDC=_cycalloc(sizeof(*_tmpDC));({struct Cyc_Tcenv_SharedFenv*_tmp171=
Cyc_Tcenv_new_shared_fenv(fd->ret_type);_tmpDC->shared=_tmp171;});_tmpDC->type_vars=_tmpD2;_tmpDC->region_order=_tmpED;_tmpDC->ctrl_env=0;({void*_tmp177=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp176=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpDE;_tmpDE.tag=24U;({struct Cyc_List_List*_tmp175=({struct Cyc_List_List*_tmpDF=_cycalloc(sizeof(*_tmpDF));({void*_tmp173=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp172=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE2;_tmpE2.tag=23U;_tmpE2.f1=param_rgn;_tmpE2;});_tmpE1[0]=_tmp172;});_tmpE1;});_tmpDF->hd=_tmp173;});({struct Cyc_List_List*_tmp174=({struct Cyc_List_List*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->hd=(void*)_check_null(fd->effect);_tmpE0->tl=0;_tmpE0;});_tmpDF->tl=_tmp174;});_tmpDF;});_tmpDE.f1=_tmp175;});_tmpDE;});_tmpDD[0]=_tmp176;});_tmpDD;});_tmpDC->capability=_tmp177;});_tmpDC->curr_rgn=param_rgn;({struct Cyc_Tcenv_FenvFlags _tmp178=({(_tmpDC->flags).in_new=Cyc_Tcenv_NoneNew;(_tmpDC->flags).in_notreadctxt=0;(_tmpDC->flags).in_lhs=0;(_tmpDC->flags).abstract_ok=0;(_tmpDC->flags).in_stmt_exp=0;_tmpDC->flags;});_tmpDC->flags=_tmp178;});_tmpDC;});};};};}
# 569
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args){
# 571
struct Cyc_Absyn_Tvar*rgn0=({struct Cyc_Absyn_Tvar*_tmpFB=_cycalloc(sizeof(*_tmpFB));({struct _dyneither_ptr*_tmp179=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"bogus",sizeof(char),6U);_tmpFB->name=_tmp179;});({int _tmp17A=Cyc_Tcutil_new_tvar_id();_tmpFB->identity=_tmp17A;});_tmpFB->kind=(void*)& Cyc_Tcenv_rgn_kb;_tmpFB;});
struct Cyc_List_List*_tmpEE=({struct Cyc_List_List*_tmpFA=_cycalloc(sizeof(*_tmpFA));_tmpFA->hd=rgn0;_tmpFA->tl=0;_tmpFA;});
struct Cyc_RgnOrder_RgnPO*_tmpEF=({void*_tmp17C=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF8=_cycalloc(sizeof(*_tmpF8));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp17B=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF9;_tmpF9.tag=24U;_tmpF9.f1=0;_tmpF9;});_tmpF8[0]=_tmp17B;});_tmpF8;});Cyc_RgnOrder_initial_fn_po(0,0,_tmp17C,rgn0,0U);});
void*param_rgn=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp17D=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpF7;_tmpF7.tag=2U;_tmpF7.f1=rgn0;_tmpF7;});_tmpF6[0]=_tmp17D;});_tmpF6;});
return({struct Cyc_Tcenv_Fenv*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Tcenv_SharedFenv*_tmp17E=
Cyc_Tcenv_new_shared_fenv(ret_type);_tmpF0->shared=_tmp17E;});_tmpF0->type_vars=_tmpEE;_tmpF0->region_order=_tmpEF;_tmpF0->ctrl_env=0;({void*_tmp183=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmpF1=_cycalloc(sizeof(*_tmpF1));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp182=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpF2;_tmpF2.tag=24U;({struct Cyc_List_List*_tmp181=({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(*_tmpF3));({void*_tmp180=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp17F=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpF5;_tmpF5.tag=23U;_tmpF5.f1=param_rgn;_tmpF5;});_tmpF4[0]=_tmp17F;});_tmpF4;});_tmpF3->hd=_tmp180;});_tmpF3->tl=0;_tmpF3;});_tmpF2.f1=_tmp181;});_tmpF2;});_tmpF1[0]=_tmp182;});_tmpF1;});_tmpF0->capability=_tmp183;});_tmpF0->curr_rgn=param_rgn;({struct Cyc_Tcenv_FenvFlags _tmp184=({(_tmpF0->flags).in_new=Cyc_Tcenv_NoneNew;(_tmpF0->flags).in_notreadctxt=0;(_tmpF0->flags).in_lhs=0;(_tmpF0->flags).abstract_ok=0;(_tmpF0->flags).in_stmt_exp=0;_tmpF0->flags;});_tmpF0->flags=_tmp184;});_tmpF0;});}
