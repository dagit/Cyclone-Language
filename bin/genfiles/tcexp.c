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
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
typedef unsigned int Cyc_Core___cyclone_internal_singleton;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 39 "list.h"
typedef struct Cyc_List_List*Cyc_List_list_t;
# 49 "list.h"
typedef struct Cyc_List_List*Cyc_List_List_t;
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 923 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 932
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 935
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 937
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 948
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 950
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 953
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 955
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 957
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 967
void*Cyc_Absyn_exn_typ();
# 978
extern void*Cyc_Absyn_bounds_one;
# 983
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 986
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 989
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 994
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1005
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1078
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1082
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1154
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1156
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1159
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1161
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 76
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 86
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 90
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 96
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct Cyc_Tcenv_Tenv*);
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 114
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 119
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 127
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
# 134
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 138
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 140
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 145
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 42
void*Cyc_Tcutil_copy_type(void*t);
# 45
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 64
int Cyc_Tcutil_coerceable(void*);
# 67
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 69
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 75
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 83
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 86
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
# 90
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 95
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 111
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 121
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 137
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 139
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 143
void Cyc_Tcutil_explain_failure();
# 145
int Cyc_Tcutil_unify(void*,void*);
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 177
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 219 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 229
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 231
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 233
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 237
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 243
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 245
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 247
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 257
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 262
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 265
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 270
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 279
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 338
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 354
int Cyc_Tcutil_is_array(void*t);
# 358
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 361
int Cyc_Tcutil_zeroable_type(void*t);struct _tuple15{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple15 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 48
int Cyc_Evexp_okay_szofarg(void*t);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 30 "formatstr.h"
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 33
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 35
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 38
extern int Cyc_Tcexp_in_stmt_exp;struct Cyc_Hashtable_Table;
# 35 "hashtable.h"
typedef struct Cyc_Hashtable_Table*Cyc_Hashtable_table_t;
# 29 "jump_analysis.h"
typedef struct Cyc_Hashtable_Table*Cyc_JumpAnalysis_table_t;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 44 "jump_analysis.h"
typedef struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_anal_res_t;
# 46
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};
# 52
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*fields;};
# 63
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 65
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_ThisIL  = 1,Cyc_CfFlowInfo_AllIL  = 2};
# 70
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 85
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 90
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 98
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 131 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 149
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
# 46 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 50
({void*_tmp0=0;({unsigned int _tmp52E=loc;struct _dyneither_ptr _tmp52D=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp52E,_tmp52D,_tag_dyneither(_tmp0,sizeof(void*),0));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 54
return*topt;}}
# 63
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 67
if(topt == 0){
# 69
({void*_tmp530=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp52F=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp2;_tmp2.tag=25;_tmp2.f1=des;_tmp2;});_tmp1[0]=_tmp52F;});_tmp1;});e->r=_tmp530;});
return;}{
# 72
void*t=*topt;
void*_tmp3=Cyc_Tcutil_compress(t);void*_tmp4=_tmp3;void*_tmp13;struct Cyc_Absyn_Tqual _tmp12;union Cyc_Absyn_AggrInfoU _tmp11;switch(*((int*)_tmp4)){case 11: _LL1: _tmp11=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1).aggr_info;_LL2:
# 75
{union Cyc_Absyn_AggrInfoU _tmp5=_tmp11;struct Cyc_Absyn_Aggrdecl*_tmpA;if((_tmp5.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp6=0;({struct _dyneither_ptr _tmp531=({const char*_tmp7="struct type not properly set";_tag_dyneither(_tmp7,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp531,_tag_dyneither(_tmp6,sizeof(void*),0));});});}else{_LLC: _tmpA=*(_tmp5.KnownAggr).val;_LLD:
({void*_tmp533=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp8=_cycalloc(sizeof(*_tmp8));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp532=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp9;_tmp9.tag=28;_tmp9.f1=_tmpA->name;_tmp9.f2=0;_tmp9.f3=des;_tmp9.f4=_tmpA;_tmp9;});_tmp8[0]=_tmp532;});_tmp8;});e->r=_tmp533;});}_LL9:;}
# 79
goto _LL0;case 8: _LL3: _tmp13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).elt_type;_tmp12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).tq;_LL4:
({void*_tmp535=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp534=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpC;_tmpC.tag=25;_tmpC.f1=des;_tmpC;});_tmpB[0]=_tmp534;});_tmpB;});e->r=_tmp535;});goto _LL0;case 12: _LL5: _LL6:
({void*_tmp537=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD=_cycalloc(sizeof(*_tmpD));({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp536=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE;_tmpE.tag=29;_tmpE.f1=t;_tmpE.f2=des;_tmpE;});_tmpD[0]=_tmp536;});_tmpD;});e->r=_tmp537;});goto _LL0;default: _LL7: _LL8:
({void*_tmp539=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp538=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp10;_tmp10.tag=25;_tmp10.f1=des;_tmp10;});_tmpF[0]=_tmp538;});_tmpF;});e->r=_tmp539;});goto _LL0;}_LL0:;};}
# 89
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 93
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 99
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp14=e->r;void*_tmp15=_tmp14;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->tag == 3){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->f2 == 0){_LLF: _LL10:
({void*_tmp16=0;({unsigned int _tmp53B=e->loc;struct _dyneither_ptr _tmp53A=({const char*_tmp17="assignment in test";_tag_dyneither(_tmp17,sizeof(char),19);});Cyc_Tcutil_warn(_tmp53B,_tmp53A,_tag_dyneither(_tmp16,sizeof(void*),0));});});goto _LLE;}else{goto _LL11;}}else{_LL11: _LL12:
 goto _LLE;}_LLE:;}
# 107
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;({struct _dyneither_ptr _tmp53C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp1B.f1=_tmp53C;});({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp18[2]={& _tmp1A,& _tmp1B};({unsigned int _tmp53E=e->loc;struct _dyneither_ptr _tmp53D=({const char*_tmp19="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp19,sizeof(char),59);});Cyc_Tcutil_terr(_tmp53E,_tmp53D,_tag_dyneither(_tmp18,sizeof(void*),2));});});});});
# 116
{void*_tmp1C=e->r;void*_tmp1D=_tmp1C;enum Cyc_Absyn_Primop _tmp26;struct Cyc_List_List*_tmp25;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->tag == 2){_LL14: _tmp26=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f1;_tmp25=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f2;_LL15:
# 118
 if(_tmp26 == Cyc_Absyn_Eq  || _tmp26 == Cyc_Absyn_Neq){
struct _tuple0 _tmp1E=({struct _tuple0 _tmp24;_tmp24.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25))->hd)->topt);_tmp24.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25->tl))->hd)->topt);_tmp24;});struct _tuple0 _tmp1F=_tmp1E;void*_tmp23;void*_tmp22;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->tag == 15){_LL19: _tmp23=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->f1;_tmp22=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->f1;_LL1A:
# 121
 return({struct Cyc_Tcexp_TestEnv _tmp20;({struct _tuple0*_tmp53F=({struct _tuple0*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->f1=_tmp23;_tmp21->f2=_tmp22;_tmp21;});_tmp20.eq=_tmp53F;});_tmp20.isTrue=_tmp26 == Cyc_Absyn_Eq;_tmp20;});}else{goto _LL1B;}}else{_LL1B: _LL1C:
# 123
 goto _LL18;}_LL18:;}
# 126
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 129
return({struct Cyc_Tcexp_TestEnv _tmp27;_tmp27.eq=0;_tmp27.isTrue=0;_tmp27;});}
# 150 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 155
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp28=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp29=_tmp28;struct _dyneither_ptr _tmp5F;struct _dyneither_ptr _tmp5E;enum Cyc_Absyn_Sign _tmp5D;int _tmp5C;int _tmp5B;enum Cyc_Absyn_Sign _tmp5A;enum Cyc_Absyn_Sign _tmp59;switch((_tmp29.Wstring_c).tag){case 2: switch(((_tmp29.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1E: _LL1F:
 t=Cyc_Absyn_schar_typ;goto _LL1D;case Cyc_Absyn_Unsigned: _LL20: _LL21:
 t=Cyc_Absyn_uchar_typ;goto _LL1D;default: _LL22: _LL23:
 t=Cyc_Absyn_char_typ;goto _LL1D;}case 3: _LL24: _LL25:
({void*_tmp540=Cyc_Absyn_wchar_typ();t=_tmp540;});goto _LL1D;case 4: _LL26: _tmp59=((_tmp29.Short_c).val).f1;_LL27:
# 165
 t=_tmp59 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL1D;case 6: _LL28: _tmp5A=((_tmp29.LongLong_c).val).f1;_LL29:
# 167
 t=_tmp5A == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL1D;case 7: _LL2A: _tmp5B=((_tmp29.Float_c).val).f2;_LL2B:
({void*_tmp541=Cyc_Absyn_float_typ(_tmp5B);t=_tmp541;});goto _LL1D;case 5: _LL2C: _tmp5D=((_tmp29.Int_c).val).f1;_tmp5C=((_tmp29.Int_c).val).f2;_LL2D:
# 170
 if(topt == 0)
t=_tmp5D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 177
void*_tmp2A=Cyc_Tcutil_compress(*topt);void*_tmp2B=_tmp2A;void*_tmp41;void*_tmp40;struct Cyc_Absyn_Tqual _tmp3F;void*_tmp3E;union Cyc_Absyn_Constraint*_tmp3D;union Cyc_Absyn_Constraint*_tmp3C;union Cyc_Absyn_Constraint*_tmp3B;enum Cyc_Absyn_Sign _tmp3A;enum Cyc_Absyn_Sign _tmp39;enum Cyc_Absyn_Sign _tmp38;enum Cyc_Absyn_Sign _tmp37;switch(*((int*)_tmp2B)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f2){case Cyc_Absyn_Char_sz: _LL35: _tmp37=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL36:
# 179
{enum Cyc_Absyn_Sign _tmp2C=_tmp37;switch(_tmp2C){case Cyc_Absyn_Unsigned: _LL44: _LL45:
 t=Cyc_Absyn_uchar_typ;goto _LL43;case Cyc_Absyn_Signed: _LL46: _LL47:
 t=Cyc_Absyn_schar_typ;goto _LL43;default: _LL48: _LL49:
 t=Cyc_Absyn_char_typ;goto _LL43;}_LL43:;}
# 184
({union Cyc_Absyn_Cnst _tmp542=Cyc_Absyn_Char_c(_tmp37,(char)_tmp5C);*c=_tmp542;});
goto _LL34;case Cyc_Absyn_Short_sz: _LL37: _tmp38=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL38:
# 187
 t=_tmp38 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
({union Cyc_Absyn_Cnst _tmp543=Cyc_Absyn_Short_c(_tmp38,(short)_tmp5C);*c=_tmp543;});
goto _LL34;case Cyc_Absyn_Int_sz: _LL39: _tmp39=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL3A:
# 191
 t=_tmp39 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;case Cyc_Absyn_Long_sz: _LL3B: _tmp3A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL3C:
# 194
 t=_tmp3A == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;default: goto _LL41;}case 5: _LL3D: _tmp40=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_typ;_tmp3F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_tq;_tmp3E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).rgn;_tmp3D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).nullable;_tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).bounds;_tmp3B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;if(_tmp5C == 0){_LL3E: {
# 198
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp3D))return*topt;{
struct Cyc_List_List*_tmp2D=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp547=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp546=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp2F;_tmp2F.tag=5;({struct Cyc_Absyn_PtrInfo _tmp545=({struct Cyc_Absyn_PtrInfo _tmp30;_tmp30.elt_typ=_tmp40;_tmp30.elt_tq=_tmp3F;({struct Cyc_Absyn_PtrAtts _tmp544=({(_tmp30.ptr_atts).rgn=_tmp3E;(_tmp30.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp30.ptr_atts).bounds=_tmp3C;(_tmp30.ptr_atts).zero_term=_tmp3B;(_tmp30.ptr_atts).ptrloc=0;_tmp30.ptr_atts;});_tmp30.ptr_atts=_tmp544;});_tmp30;});_tmp2F.f1=_tmp545;});_tmp2F;});_tmp2E[0]=_tmp546;});_tmp2E;});t=_tmp547;});
# 204
goto _LL34;};}}else{goto _LL41;}case 19: _LL3F: _tmp41=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B)->f1;_LL40: {
# 206
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp32=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp549=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp36;_tmp36.tag=18;({struct Cyc_Absyn_Exp*_tmp548=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0);_tmp36.f1=_tmp548;});_tmp36;});_tmp35[0]=_tmp549;});_tmp35;});
# 213
({void*_tmp54B=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp33=_cycalloc(sizeof(*_tmp33));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp54A=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp34;_tmp34.tag=19;_tmp34.f1=(void*)_tmp32;_tmp34;});_tmp33[0]=_tmp54A;});_tmp33;});t=_tmp54B;});
goto _LL34;}default: _LL41: _LL42:
# 216
 t=_tmp5D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;}_LL34:;}
# 219
goto _LL1D;case 8: _LL2E: _tmp5E=(_tmp29.String_c).val;_LL2F:
# 221
 string_numelts=(int)_get_dyneither_size(_tmp5E,sizeof(char));
_tmp5F=_tmp5E;goto _LL31;case 9: _LL30: _tmp5F=(_tmp29.Wstring_c).val;_LL31:
# 224
 if(string_numelts == 0){
({int _tmp54C=Cyc_Tcexp_wchar_numelts(_tmp5F);string_numelts=_tmp54C;});
({void*_tmp54D=Cyc_Absyn_wchar_typ();string_elt_typ=_tmp54D;});}{
# 228
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp54E=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp54E,loc);});
elen->topt=Cyc_Absyn_uint_typ;{
# 233
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp42=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp46=_cycalloc(sizeof(*_tmp46));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp54F=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp47;_tmp47.tag=1;_tmp47.f1=elen;_tmp47;});_tmp46[0]=_tmp54F;});_tmp46;});
({void*_tmp553=({void*_tmp552=string_elt_typ;struct Cyc_Absyn_Tqual _tmp551=Cyc_Absyn_const_tqual(0);void*_tmp550=(void*)_tmp42;Cyc_Absyn_atb_typ(_tmp552,(void*)& Cyc_Absyn_HeapRgn_val,_tmp551,_tmp550,Cyc_Absyn_true_conref);});t=_tmp553;});
# 236
if(topt != 0){
void*_tmp43=Cyc_Tcutil_compress(*topt);void*_tmp44=_tmp43;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 8: _LL4B: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL4C:
# 241
 return({void*_tmp556=string_elt_typ;struct Cyc_Absyn_Tqual _tmp555=_tmp45;struct Cyc_Absyn_Exp*_tmp554=elen;Cyc_Absyn_array_typ(_tmp556,_tmp555,_tmp554,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);});case 5: _LL4D: _LL4E:
# 243
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 249
({void*_tmp55B=({void*_tmp55A=string_elt_typ;void*_tmp559=({Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));});struct Cyc_Absyn_Tqual _tmp558=
Cyc_Absyn_const_tqual(0);
# 249
void*_tmp557=(void*)_tmp42;Cyc_Absyn_atb_typ(_tmp55A,_tmp559,_tmp558,_tmp557,Cyc_Absyn_true_conref);});t=_tmp55B;});
# 251
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 257
goto _LL4A;default: _LL4F: _LL50:
 goto _LL4A;}_LL4A:;}
# 261
return t;};};default: _LL32: _LL33:
# 263
 if(topt != 0){
void*_tmp48=Cyc_Tcutil_compress(*topt);void*_tmp49=_tmp48;void*_tmp52;struct Cyc_Absyn_Tqual _tmp51;void*_tmp50;union Cyc_Absyn_Constraint*_tmp4F;union Cyc_Absyn_Constraint*_tmp4E;union Cyc_Absyn_Constraint*_tmp4D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->tag == 5){_LL52: _tmp52=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).elt_typ;_tmp51=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).elt_tq;_tmp50=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).rgn;_tmp4F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).nullable;_tmp4E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).bounds;_tmp4D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).zero_term;_LL53:
# 267
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp4F))return*topt;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp55E=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4B;_tmp4B.tag=5;({struct Cyc_Absyn_PtrInfo _tmp55D=({struct Cyc_Absyn_PtrInfo _tmp4C;_tmp4C.elt_typ=_tmp52;_tmp4C.elt_tq=_tmp51;({struct Cyc_Absyn_PtrAtts _tmp55C=({(_tmp4C.ptr_atts).rgn=_tmp50;(_tmp4C.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4C.ptr_atts).bounds=_tmp4E;(_tmp4C.ptr_atts).zero_term=_tmp4D;(_tmp4C.ptr_atts).ptrloc=0;_tmp4C.ptr_atts;});_tmp4C.ptr_atts=_tmp55C;});_tmp4C;});_tmp4B.f1=_tmp55D;});_tmp4B;});_tmp4A[0]=_tmp55E;});_tmp4A;});}else{_LL54: _LL55:
# 270
 goto _LL51;}_LL51:;}{
# 273
struct Cyc_List_List*_tmp53=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp567=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp54=_cycalloc(sizeof(*_tmp54));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp566=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp55;_tmp55.tag=5;({struct Cyc_Absyn_PtrInfo _tmp565=({struct Cyc_Absyn_PtrInfo _tmp56;({void*_tmp55F=({Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->v=_tmp53;_tmp58;}));});_tmp56.elt_typ=_tmp55F;});({struct Cyc_Absyn_Tqual _tmp560=
Cyc_Absyn_empty_tqual(0);_tmp56.elt_tq=_tmp560;});({struct Cyc_Absyn_PtrAtts _tmp564=({({void*_tmp561=({
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->v=_tmp53;_tmp57;}));});(_tmp56.ptr_atts).rgn=_tmp561;});(_tmp56.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmp562=
# 278
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp56.ptr_atts).bounds=_tmp562;});({union Cyc_Absyn_Constraint*_tmp563=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp56.ptr_atts).zero_term=_tmp563;});(_tmp56.ptr_atts).ptrloc=0;_tmp56.ptr_atts;});_tmp56.ptr_atts=_tmp564;});_tmp56;});_tmp55.f1=_tmp565;});_tmp55;});_tmp54[0]=_tmp566;});_tmp54;});
# 274
t=_tmp567;});
# 280
goto _LL1D;};}_LL1D:;}
# 282
return t;}
# 286
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 291
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp60=*((void**)_check_null(b));void*_tmp61=_tmp60;struct Cyc_Absyn_Vardecl*_tmp6F;struct Cyc_Absyn_Vardecl*_tmp6E;struct Cyc_Absyn_Vardecl*_tmp6D;struct Cyc_Absyn_Fndecl*_tmp6C;struct Cyc_Absyn_Vardecl*_tmp6B;struct _tuple1*_tmp6A;switch(*((int*)_tmp61)){case 0: _LL57: _tmp6A=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp61)->f1;_LL58:
# 294
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp62=_cycalloc(sizeof(*_tmp62));({struct Cyc_Core_Impossible_exn_struct _tmp569=({struct Cyc_Core_Impossible_exn_struct _tmp63;_tmp63.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp568=({const char*_tmp64="unresolved binding in tcVar";_tag_dyneither(_tmp64,sizeof(char),28);});_tmp63.f1=_tmp568;});_tmp63;});_tmp62[0]=_tmp569;});_tmp62;}));case 1: _LL59: _tmp6B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5A:
# 298
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp6B->name,1);
return _tmp6B->type;case 2: _LL5B: _tmp6C=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5C:
# 305
 if(_tmp6C->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp6C->name,1);
return Cyc_Tcutil_fndecl2typ(_tmp6C);case 5: _LL5D: _tmp6D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5E:
 _tmp6E=_tmp6D;goto _LL60;case 4: _LL5F: _tmp6E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp61)->f1;_LL60:
 _tmp6F=_tmp6E;goto _LL62;default: _LL61: _tmp6F=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp61)->f1;_LL62:
# 311
 if(te->allow_valueof){
void*_tmp65=Cyc_Tcutil_compress(_tmp6F->type);void*_tmp66=_tmp65;void*_tmp69;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66)->tag == 19){_LL64: _tmp69=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66)->f1;_LL65:
# 314
({void*_tmp56B=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp56A=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp68;_tmp68.tag=38;_tmp68.f1=_tmp69;_tmp68;});_tmp67[0]=_tmp56A;});_tmp67;});e->r=_tmp56B;});
goto _LL63;}else{_LL66: _LL67:
 goto _LL63;}_LL63:;}
# 319
return _tmp6F->type;}_LL56:;}
# 323
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 332
struct Cyc_List_List*desc_types;
{void*_tmp70=fmt->r;void*_tmp71=_tmp70;struct _dyneither_ptr _tmp77;struct _dyneither_ptr _tmp76;switch(*((int*)_tmp71)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp71)->f1).String_c).tag == 8){_LL69: _tmp76=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp71)->f1).String_c).val;_LL6A:
 _tmp77=_tmp76;goto _LL6C;}else{goto _LL6D;}case 13: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->f1).String_c).tag == 8){_LL6B: _tmp77=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->f1).String_c).val;_LL6C:
# 336
({struct Cyc_List_List*_tmp56C=type_getter(temp,te,(struct _dyneither_ptr)_tmp77,fmt->loc);desc_types=_tmp56C;});goto _LL68;}else{goto _LL6D;}}else{goto _LL6D;}default: _LL6D: _LL6E:
# 340
 if(opt_args != 0){
struct Cyc_List_List*_tmp72=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp72 != 0;_tmp72=_tmp72->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp72->hd);{
struct _RegionHandle*_tmp73=Cyc_Core_heap_region;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp73,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp72->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(_tmp73,(struct Cyc_Absyn_Exp*)_tmp72->hd))
({void*_tmp74=0;({unsigned int _tmp56E=((struct Cyc_Absyn_Exp*)_tmp72->hd)->loc;struct _dyneither_ptr _tmp56D=({const char*_tmp75="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp75,sizeof(char),49);});Cyc_Tcutil_terr(_tmp56E,_tmp56D,_tag_dyneither(_tmp74,sizeof(void*),0));});});};}}
# 351
return;}_LL68:;}
# 353
if(opt_args != 0){
struct Cyc_List_List*_tmp78=(struct Cyc_List_List*)opt_args->v;
# 356
for(0;desc_types != 0  && _tmp78 != 0;
(((desc_types=desc_types->tl,_tmp78=_tmp78->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp78->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0;({struct _dyneither_ptr _tmp56F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp7C.f1=_tmp56F;});({struct Cyc_String_pa_PrintArg_struct _tmp7B;_tmp7B.tag=0;({struct _dyneither_ptr _tmp570=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp7B.f1=_tmp570;});({void*_tmp79[2]={& _tmp7B,& _tmp7C};({unsigned int _tmp572=e->loc;struct _dyneither_ptr _tmp571=({const char*_tmp7A="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp7A,sizeof(char),48);});Cyc_Tcutil_terr(_tmp572,_tmp571,_tag_dyneither(_tmp79,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 367
if(alias_coercion)
({struct Cyc_List_List*_tmp573=({struct Cyc_List_List*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->hd=(void*)arg_cnt;_tmp7D->tl=*alias_arg_exps;_tmp7D;});*alias_arg_exps=_tmp573;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Core_heap_region,t) && !
Cyc_Tcutil_is_noalias_path(Cyc_Core_heap_region,e))
({void*_tmp7E=0;({unsigned int _tmp575=((struct Cyc_Absyn_Exp*)_tmp78->hd)->loc;struct _dyneither_ptr _tmp574=({const char*_tmp7F="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp7F,sizeof(char),49);});Cyc_Tcutil_terr(_tmp575,_tmp574,_tag_dyneither(_tmp7E,sizeof(void*),0));});});}
# 374
if(desc_types != 0)
({void*_tmp80=0;({unsigned int _tmp577=fmt->loc;struct _dyneither_ptr _tmp576=({const char*_tmp81="too few arguments";_tag_dyneither(_tmp81,sizeof(char),18);});Cyc_Tcutil_terr(_tmp577,_tmp576,_tag_dyneither(_tmp80,sizeof(void*),0));});});
if(_tmp78 != 0){
({void*_tmp82=0;({unsigned int _tmp579=((struct Cyc_Absyn_Exp*)_tmp78->hd)->loc;struct _dyneither_ptr _tmp578=({const char*_tmp83="too many arguments";_tag_dyneither(_tmp83,sizeof(char),19);});Cyc_Tcutil_terr(_tmp579,_tmp578,_tag_dyneither(_tmp82,sizeof(void*),0));});});
# 379
for(0;_tmp78 != 0;_tmp78=_tmp78->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp78->hd);}}}}
# 384
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 386
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp84=p;switch(_tmp84){case Cyc_Absyn_Plus: _LL70: _LL71:
 goto _LL73;case Cyc_Absyn_Minus: _LL72: _LL73:
# 390
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp87;_tmp87.tag=0;({struct _dyneither_ptr _tmp57A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp87.f1=_tmp57A;});({void*_tmp85[1]={& _tmp87};({unsigned int _tmp57C=loc;struct _dyneither_ptr _tmp57B=({const char*_tmp86="expecting numeric type but found %s";_tag_dyneither(_tmp86,sizeof(char),36);});Cyc_Tcutil_terr(_tmp57C,_tmp57B,_tag_dyneither(_tmp85,sizeof(void*),1));});});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL74: _LL75:
# 394
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp8A;_tmp8A.tag=0;({struct _dyneither_ptr _tmp57D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp8A.f1=_tmp57D;});({void*_tmp88[1]={& _tmp8A};({unsigned int _tmp57F=loc;struct _dyneither_ptr _tmp57E=({const char*_tmp89="expecting integral or * type but found %s";_tag_dyneither(_tmp89,sizeof(char),42);});Cyc_Tcutil_terr(_tmp57F,_tmp57E,_tag_dyneither(_tmp88,sizeof(void*),1));});});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL76: _LL77:
# 399
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp8D;_tmp8D.tag=0;({struct _dyneither_ptr _tmp580=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp8D.f1=_tmp580;});({void*_tmp8B[1]={& _tmp8D};({unsigned int _tmp582=loc;struct _dyneither_ptr _tmp581=({const char*_tmp8C="expecting integral type but found %s";_tag_dyneither(_tmp8C,sizeof(char),37);});Cyc_Tcutil_terr(_tmp582,_tmp581,_tag_dyneither(_tmp8B,sizeof(void*),1));});});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL78: _LL79:
# 403
{void*_tmp8E=t;union Cyc_Absyn_Constraint*_tmp97;switch(*((int*)_tmp8E)){case 8: _LL7D: _LL7E:
 goto _LL7C;case 5: _LL7F: _tmp97=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8E)->f1).ptr_atts).bounds;_LL80:
# 406
{void*_tmp8F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp97);void*_tmp90=_tmp8F;struct Cyc_Absyn_Exp*_tmp93;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp90)->tag == 0){_LL84: _LL85:
 goto _LL83;}else{_LL86: _tmp93=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp90)->f1;_LL87:
# 409
 if(!Cyc_Evexp_c_can_eval(_tmp93) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp91=0;({unsigned int _tmp584=loc;struct _dyneither_ptr _tmp583=({const char*_tmp92="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp92,sizeof(char),55);});Cyc_Tcutil_terr(_tmp584,_tmp583,_tag_dyneither(_tmp91,sizeof(void*),0));});});
goto _LL83;}_LL83:;}
# 413
goto _LL7C;default: _LL81: _LL82:
# 415
({struct Cyc_String_pa_PrintArg_struct _tmp96;_tmp96.tag=0;({struct _dyneither_ptr _tmp585=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp96.f1=_tmp585;});({void*_tmp94[1]={& _tmp96};({unsigned int _tmp587=loc;struct _dyneither_ptr _tmp586=({const char*_tmp95="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp95,sizeof(char),47);});Cyc_Tcutil_terr(_tmp587,_tmp586,_tag_dyneither(_tmp94,sizeof(void*),1));});});});}_LL7C:;}
# 417
return Cyc_Absyn_uint_typ;default: _LL7A: _LL7B:
({void*_tmp98=0;({struct _dyneither_ptr _tmp588=({const char*_tmp99="Non-unary primop";_tag_dyneither(_tmp99,sizeof(char),17);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp588,_tag_dyneither(_tmp98,sizeof(void*),0));});});}_LL6F:;}
# 423
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 426
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp9C;_tmp9C.tag=0;({struct _dyneither_ptr _tmp589=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp9C.f1=_tmp589;});({void*_tmp9A[1]={& _tmp9C};({unsigned int _tmp58B=e1->loc;struct _dyneither_ptr _tmp58A=({const char*_tmp9B="type %s cannot be used here";_tag_dyneither(_tmp9B,sizeof(char),28);});Cyc_Tcutil_terr(_tmp58B,_tmp58A,_tag_dyneither(_tmp9A,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 430
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp9F;_tmp9F.tag=0;({struct _dyneither_ptr _tmp58C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp9F.f1=_tmp58C;});({void*_tmp9D[1]={& _tmp9F};({unsigned int _tmp58E=e2->loc;struct _dyneither_ptr _tmp58D=({const char*_tmp9E="type %s cannot be used here";_tag_dyneither(_tmp9E,sizeof(char),28);});Cyc_Tcutil_terr(_tmp58E,_tmp58D,_tag_dyneither(_tmp9D,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 434
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 439
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmpA0=t1;void*_tmpB6;struct Cyc_Absyn_Tqual _tmpB5;void*_tmpB4;union Cyc_Absyn_Constraint*_tmpB3;union Cyc_Absyn_Constraint*_tmpB2;union Cyc_Absyn_Constraint*_tmpB1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0)->tag == 5){_LL89: _tmpB6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0)->f1).elt_typ;_tmpB5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0)->f1).elt_tq;_tmpB4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0)->f1).ptr_atts).rgn;_tmpB3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0)->f1).ptr_atts).nullable;_tmpB2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0)->f1).ptr_atts).bounds;_tmpB1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA0)->f1).ptr_atts).zero_term;_LL8A:
# 444
 if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpB6),& Cyc_Tcutil_tmk);}))
({void*_tmpA1=0;({unsigned int _tmp590=e1->loc;struct _dyneither_ptr _tmp58F=({const char*_tmpA2="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpA2,sizeof(char),50);});Cyc_Tcutil_terr(_tmp590,_tmp58F,_tag_dyneither(_tmpA1,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpA3=0;({unsigned int _tmp592=e1->loc;struct _dyneither_ptr _tmp591=({const char*_tmpA4="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpA4,sizeof(char),54);});Cyc_Tcutil_terr(_tmp592,_tmp591,_tag_dyneither(_tmpA3,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmpA7;_tmpA7.tag=0;({struct _dyneither_ptr _tmp593=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA7.f1=_tmp593;});({void*_tmpA5[1]={& _tmpA7};({unsigned int _tmp595=e2->loc;struct _dyneither_ptr _tmp594=({const char*_tmpA6="expecting int but found %s";_tag_dyneither(_tmpA6,sizeof(char),27);});Cyc_Tcutil_terr(_tmp595,_tmp594,_tag_dyneither(_tmpA5,sizeof(void*),1));});});});{
void*_tmpA8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB2);void*_tmpA9=_tmpA8;struct Cyc_Absyn_Exp*_tmpB0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA9)->tag == 0){_LL8E: _LL8F:
 return t1;}else{_LL90: _tmpB0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA9)->f1;_LL91:
# 455
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB1))
({void*_tmpAA=0;({unsigned int _tmp597=e1->loc;struct _dyneither_ptr _tmp596=({const char*_tmpAB="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmpAB,sizeof(char),70);});Cyc_Tcutil_warn(_tmp597,_tmp596,_tag_dyneither(_tmpAA,sizeof(void*),0));});});{
# 463
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAC=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp59A=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpAE;_tmpAE.tag=5;({struct Cyc_Absyn_PtrInfo _tmp599=({struct Cyc_Absyn_PtrInfo _tmpAF;_tmpAF.elt_typ=_tmpB6;_tmpAF.elt_tq=_tmpB5;({struct Cyc_Absyn_PtrAtts _tmp598=({(_tmpAF.ptr_atts).rgn=_tmpB4;(_tmpAF.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmpAF.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref;(_tmpAF.ptr_atts).zero_term=_tmpB1;(_tmpAF.ptr_atts).ptrloc=0;_tmpAF.ptr_atts;});_tmpAF.ptr_atts=_tmp598;});_tmpAF;});_tmpAE.f1=_tmp599;});_tmpAE;});_tmpAD[0]=_tmp59A;});_tmpAD;});
# 467
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpAC,Cyc_Absyn_Other_coercion);
return(void*)_tmpAC;};}_LL8D:;};}else{_LL8B: _LL8C:
# 470
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL88:;}
# 475
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpBA;_tmpBA.tag=0;({struct _dyneither_ptr _tmp59B=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 485
Cyc_Absynpp_typ2string(t2));_tmpBA.f1=_tmp59B;});({struct Cyc_String_pa_PrintArg_struct _tmpB9;_tmpB9.tag=0;({struct _dyneither_ptr _tmp59C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpB9.f1=_tmp59C;});({void*_tmpB7[2]={& _tmpB9,& _tmpBA};({unsigned int _tmp59E=e1->loc;struct _dyneither_ptr _tmp59D=({const char*_tmpB8="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpB8,sizeof(char),59);});Cyc_Tcutil_terr(_tmp59E,_tmp59D,_tag_dyneither(_tmpB7,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 488
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp59F=t1_elt;Cyc_Tcutil_unify(_tmp59F,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpBE;_tmpBE.tag=0;({struct _dyneither_ptr _tmp5A0=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 493
Cyc_Absynpp_typ2string(t2));_tmpBE.f1=_tmp5A0;});({struct Cyc_String_pa_PrintArg_struct _tmpBD;_tmpBD.tag=0;({struct _dyneither_ptr _tmp5A1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpBD.f1=_tmp5A1;});({void*_tmpBB[2]={& _tmpBD,& _tmpBE};({unsigned int _tmp5A3=e1->loc;struct _dyneither_ptr _tmp5A2=({const char*_tmpBC="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpBC,sizeof(char),59);});Cyc_Tcutil_terr(_tmp5A3,_tmp5A2,_tag_dyneither(_tmpBB,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 497
({void*_tmpBF=0;({unsigned int _tmp5A5=e1->loc;struct _dyneither_ptr _tmp5A4=({const char*_tmpC0="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpC0,sizeof(char),60);});Cyc_Tcutil_warn(_tmp5A5,_tmp5A4,_tag_dyneither(_tmpBF,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmp5AA=te;struct Cyc_Absyn_Exp*_tmp5A9=e1;Cyc_Tcutil_unchecked_cast(_tmp5AA,_tmp5A9,({void*_tmp5A8=t1_elt;void*_tmp5A7=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp5A6=
Cyc_Absyn_empty_tqual(0);
# 498
Cyc_Absyn_star_typ(_tmp5A8,_tmp5A7,_tmp5A6,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});
# 501
return Cyc_Absyn_sint_typ;}else{
# 503
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk);}))
({void*_tmpC1=0;({unsigned int _tmp5AC=e1->loc;struct _dyneither_ptr _tmp5AB=({const char*_tmpC2="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpC2,sizeof(char),50);});Cyc_Tcutil_terr(_tmp5AC,_tmp5AB,_tag_dyneither(_tmpC1,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpC3=0;({unsigned int _tmp5AE=e1->loc;struct _dyneither_ptr _tmp5AD=({const char*_tmpC4="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpC4,sizeof(char),54);});Cyc_Tcutil_terr(_tmp5AE,_tmp5AD,_tag_dyneither(_tmpC3,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpC8;_tmpC8.tag=0;({struct _dyneither_ptr _tmp5AF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpC8.f1=_tmp5AF;});({struct Cyc_String_pa_PrintArg_struct _tmpC7;_tmpC7.tag=0;({struct _dyneither_ptr _tmp5B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpC7.f1=_tmp5B0;});({void*_tmpC5[2]={& _tmpC7,& _tmpC8};({unsigned int _tmp5B2=e2->loc;struct _dyneither_ptr _tmp5B1=({const char*_tmpC6="expecting either %s or int but found %s";_tag_dyneither(_tmpC6,sizeof(char),40);});Cyc_Tcutil_terr(_tmp5B2,_tmp5B1,_tag_dyneither(_tmpC5,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 512
return t1;}}}
# 516
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5B3=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5B3,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpC9=0;({unsigned int _tmp5B5=e1->loc;struct _dyneither_ptr _tmp5B4=({const char*_tmpCA="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpCA,sizeof(char),60);});Cyc_Tcutil_warn(_tmp5B5,_tmp5B4,_tag_dyneither(_tmpC9,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmp5BA=te;struct Cyc_Absyn_Exp*_tmp5B9=e2;Cyc_Tcutil_unchecked_cast(_tmp5BA,_tmp5B9,({void*_tmp5B8=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5B7=
Cyc_Tcutil_pointer_region(t2);
# 521
struct Cyc_Absyn_Tqual _tmp5B6=
# 523
Cyc_Absyn_empty_tqual(0);
# 521
Cyc_Absyn_star_typ(_tmp5B8,_tmp5B7,_tmp5B6,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});}
# 526
({void*_tmpCB=0;({unsigned int _tmp5BC=e1->loc;struct _dyneither_ptr _tmp5BB=({const char*_tmpCC="thin pointer subtraction!";_tag_dyneither(_tmpCC,sizeof(char),26);});Cyc_Tcutil_warn(_tmp5BC,_tmp5BB,_tag_dyneither(_tmpCB,sizeof(void*),0));});});
return Cyc_Absyn_sint_typ;}
# 529
({void*_tmpCD=0;({unsigned int _tmp5BE=e1->loc;struct _dyneither_ptr _tmp5BD=({const char*_tmpCE="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpCE,sizeof(char),56);});Cyc_Tcutil_warn(_tmp5BE,_tmp5BD,_tag_dyneither(_tmpCD,sizeof(void*),0));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 532
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpCF=0;({unsigned int _tmp5C0=e1->loc;struct _dyneither_ptr _tmp5BF=({const char*_tmpD0="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpD0,sizeof(char),51);});Cyc_Tcutil_warn(_tmp5C0,_tmp5BF,_tag_dyneither(_tmpCF,sizeof(void*),0));});});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 537
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 540
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 548
if(({(Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind;}) || ({
void*_tmp5C1=t1;Cyc_Tcutil_unify(_tmp5C1,({Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te));}));})){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 553
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 566
pointer_cmp: {
struct _tuple0 _tmpD1=({struct _tuple0 _tmpD5;({void*_tmp5C2=Cyc_Tcutil_compress(t1);_tmpD5.f1=_tmp5C2;});({void*_tmp5C3=Cyc_Tcutil_compress(t2);_tmpD5.f2=_tmp5C3;});_tmpD5;});struct _tuple0 _tmpD2=_tmpD1;void*_tmpD4;void*_tmpD3;switch(*((int*)_tmpD2.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD2.f2)->tag == 5){_LL93: _tmpD4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD2.f1)->f1).elt_typ;_tmpD3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD2.f2)->f1).elt_typ;_LL94:
# 570
 if(Cyc_Tcutil_unify(_tmpD4,_tmpD3))return Cyc_Absyn_sint_typ;goto _LL92;}else{goto _LL97;}case 15: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD2.f2)->tag == 15){_LL95: _LL96:
# 572
 return Cyc_Absyn_sint_typ;}else{goto _LL97;}default: _LL97: _LL98:
 goto _LL92;}_LL92:;}
# 575
({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0;({struct _dyneither_ptr _tmp5C4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpD9.f1=_tmp5C4;});({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0;({struct _dyneither_ptr _tmp5C5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpD8.f1=_tmp5C5;});({void*_tmpD6[2]={& _tmpD8,& _tmpD9};({unsigned int _tmp5C7=loc;struct _dyneither_ptr _tmp5C6=({const char*_tmpD7="comparison not allowed between %s and %s";_tag_dyneither(_tmpD7,sizeof(char),41);});Cyc_Tcutil_terr(_tmp5C7,_tmp5C6,_tag_dyneither(_tmpD6,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 582
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 584
if(({void*_tmp5C8=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5C8 == Cyc_Absyn_sint_typ;}))
return Cyc_Absyn_sint_typ;{
# 590
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpDA=({struct _tuple0 _tmpDE;_tmpDE.f1=t1;_tmpDE.f2=t2;_tmpDE;});struct _tuple0 _tmpDB=_tmpDA;void*_tmpDD;void*_tmpDC;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDB.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDB.f2)->tag == 15){_LL9A: _tmpDD=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDB.f1)->f1;_tmpDC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDB.f2)->f1;_LL9B:
# 595
 return Cyc_Absyn_sint_typ;}else{goto _LL9C;}}else{_LL9C: _LL9D:
 goto _LL99;}_LL99:;}
# 598
({struct Cyc_String_pa_PrintArg_struct _tmpE2;_tmpE2.tag=0;({struct _dyneither_ptr _tmp5C9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpE2.f1=_tmp5C9;});({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;({struct _dyneither_ptr _tmp5CA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpE1.f1=_tmp5CA;});({void*_tmpDF[2]={& _tmpE1,& _tmpE2};({unsigned int _tmp5CC=loc;struct _dyneither_ptr _tmp5CB=({const char*_tmpE0="comparison not allowed between %s and %s";_tag_dyneither(_tmpE0,sizeof(char),41);});Cyc_Tcutil_terr(_tmp5CC,_tmp5CB,_tag_dyneither(_tmpDF,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 606
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 608
enum Cyc_Absyn_Primop _tmpE3=p;switch(_tmpE3){case Cyc_Absyn_Plus: _LL9F: _LLA0:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLA1: _LLA2:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLA3: _LLA4:
# 612
 goto _LLA6;case Cyc_Absyn_Div: _LLA5: _LLA6:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLA7: _LLA8:
# 615
 goto _LLAA;case Cyc_Absyn_Bitand: _LLA9: _LLAA:
 goto _LLAC;case Cyc_Absyn_Bitor: _LLAB: _LLAC:
 goto _LLAE;case Cyc_Absyn_Bitxor: _LLAD: _LLAE:
 goto _LLB0;case Cyc_Absyn_Bitlshift: _LLAF: _LLB0:
 goto _LLB2;case Cyc_Absyn_Bitlrshift: _LLB1: _LLB2:
 goto _LLB4;case Cyc_Absyn_Bitarshift: _LLB3: _LLB4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLB5: _LLB6:
# 625
 goto _LLB8;case Cyc_Absyn_Neq: _LLB7: _LLB8:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLB9: _LLBA:
# 628
 goto _LLBC;case Cyc_Absyn_Lt: _LLBB: _LLBC:
 goto _LLBE;case Cyc_Absyn_Gte: _LLBD: _LLBE:
 goto _LLC0;case Cyc_Absyn_Lte: _LLBF: _LLC0:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLC1: _LLC2:
# 633
({void*_tmpE4=0;({struct _dyneither_ptr _tmp5CD=({const char*_tmpE5="bad binary primop";_tag_dyneither(_tmpE5,sizeof(char),18);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5CD,_tag_dyneither(_tmpE4,sizeof(void*),0));});});}_LL9E:;}
# 637
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 645
if(p == Cyc_Absyn_Minus  && ({((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1;})){
struct Cyc_Absyn_Exp*_tmpE6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpE7=Cyc_Tcexp_tcExp(te,topt,_tmpE6);
if(!Cyc_Tcutil_is_numeric(_tmpE6))
({struct Cyc_String_pa_PrintArg_struct _tmpEA;_tmpEA.tag=0;({struct _dyneither_ptr _tmp5CE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpE7));_tmpEA.f1=_tmp5CE;});({void*_tmpE8[1]={& _tmpEA};({unsigned int _tmp5D0=_tmpE6->loc;struct _dyneither_ptr _tmp5CF=({const char*_tmpE9="expecting numeric type but found %s";_tag_dyneither(_tmpE9,sizeof(char),36);});Cyc_Tcutil_terr(_tmp5D0,_tmp5CF,_tag_dyneither(_tmpE8,sizeof(void*),1));});});});
return _tmpE7;}
# 652
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpEB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpEC=_tmpEB;switch(_tmpEC){case 0: _LLC4: _LLC5:
 return({void*_tmpED=0;({struct Cyc_Tcenv_Tenv*_tmp5D4=te;unsigned int _tmp5D3=loc;void**_tmp5D2=topt;struct _dyneither_ptr _tmp5D1=({const char*_tmpEE="primitive operator has 0 arguments";_tag_dyneither(_tmpEE,sizeof(char),35);});Cyc_Tcexp_expr_err(_tmp5D4,_tmp5D3,_tmp5D2,_tmp5D1,_tag_dyneither(_tmpED,sizeof(void*),0));});});case 1: _LLC6: _LLC7:
({void*_tmp5D5=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp5D5;});goto _LLC3;case 2: _LLC8: _LLC9:
({void*_tmp5DB=({struct Cyc_Tcenv_Tenv*_tmp5DA=te;unsigned int _tmp5D9=loc;void**_tmp5D8=topt;enum Cyc_Absyn_Primop _tmp5D7=p;struct Cyc_Absyn_Exp*_tmp5D6=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp5DA,_tmp5D9,_tmp5D8,_tmp5D7,_tmp5D6,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp5DB;});goto _LLC3;default: _LLCA: _LLCB:
 return({void*_tmpEF=0;({struct Cyc_Tcenv_Tenv*_tmp5DF=te;unsigned int _tmp5DE=loc;void**_tmp5DD=topt;struct _dyneither_ptr _tmp5DC=({const char*_tmpF0="primitive operator has > 2 arguments";_tag_dyneither(_tmpF0,sizeof(char),37);});Cyc_Tcexp_expr_err(_tmp5DF,_tmp5DE,_tmp5DD,_tmp5DC,_tag_dyneither(_tmpEF,sizeof(void*),0));});});}_LLC3:;}
# 660
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 663
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpF1=Cyc_Tcutil_compress(t);
void*_tmpF2=_tmpF1;struct Cyc_List_List*_tmp10E;void*_tmp10D;struct Cyc_Absyn_Tqual _tmp10C;struct Cyc_Absyn_Datatypefield*_tmp10B;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Aggrdecl*_tmp109;switch(*((int*)_tmpF2)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF2)->f1).aggr_info).KnownAggr).tag == 2){_LLCD: _tmp109=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF2)->f1).aggr_info).KnownAggr).val;_LLCE:
# 667
 if(_tmp109->impl == 0){
({void*_tmpF3=0;({unsigned int _tmp5E1=loc;struct _dyneither_ptr _tmp5E0=({const char*_tmpF4="attempt to write an abstract aggregate";_tag_dyneither(_tmpF4,sizeof(char),39);});Cyc_Tcutil_terr(_tmp5E1,_tmp5E0,_tag_dyneither(_tmpF3,sizeof(void*),0));});});
return 0;}else{
# 671
_tmp10A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp109->impl))->fields;goto _LLD0;}}else{goto _LLD7;}case 12: _LLCF: _tmp10A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF2)->f2;_LLD0:
# 673
 for(0;_tmp10A != 0;_tmp10A=_tmp10A->tl){
struct Cyc_Absyn_Aggrfield*_tmpF5=(struct Cyc_Absyn_Aggrfield*)_tmp10A->hd;
if((_tmpF5->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpF8;_tmpF8.tag=0;_tmpF8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF5->name);({void*_tmpF6[1]={& _tmpF8};({unsigned int _tmp5E3=loc;struct _dyneither_ptr _tmp5E2=({const char*_tmpF7="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpF7,sizeof(char),56);});Cyc_Tcutil_terr(_tmp5E3,_tmp5E2,_tag_dyneither(_tmpF6,sizeof(void*),1));});});});
return 0;}
# 679
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF5->type))return 0;}
# 681
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF2)->f1).field_info).KnownDatatypefield).tag == 2){_LLD1: _tmp10B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF2)->f1).field_info).KnownDatatypefield).val).f2;_LLD2:
# 683
{struct Cyc_List_List*_tmpF9=_tmp10B->typs;for(0;_tmpF9 != 0;_tmpF9=_tmpF9->tl){
struct _tuple17*_tmpFA=(struct _tuple17*)_tmpF9->hd;struct _tuple17*_tmpFB=_tmpFA;struct Cyc_Absyn_Tqual _tmp100;void*_tmpFF;_LLDA: _tmp100=_tmpFB->f1;_tmpFF=_tmpFB->f2;_LLDB:;
if(_tmp100.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0;({struct _dyneither_ptr _tmp5E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp10B->name));_tmpFE.f1=_tmp5E4;});({void*_tmpFC[1]={& _tmpFE};({unsigned int _tmp5E6=loc;struct _dyneither_ptr _tmp5E5=({const char*_tmpFD="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpFD,sizeof(char),64);});Cyc_Tcutil_terr(_tmp5E6,_tmp5E5,_tag_dyneither(_tmpFC,sizeof(void*),1));});});});
return 0;}
# 689
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpFF))return 0;}}
# 691
return 1;}else{goto _LLD7;}case 8: _LLD3: _tmp10D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF2)->f1).elt_type;_tmp10C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF2)->f1).tq;_LLD4:
# 693
 if(_tmp10C.real_const){
({void*_tmp101=0;({unsigned int _tmp5E8=loc;struct _dyneither_ptr _tmp5E7=({const char*_tmp102="attempt to over-write a const array";_tag_dyneither(_tmp102,sizeof(char),36);});Cyc_Tcutil_terr(_tmp5E8,_tmp5E7,_tag_dyneither(_tmp101,sizeof(void*),0));});});
return 0;}
# 697
return Cyc_Tcexp_check_writable_aggr(loc,_tmp10D);case 10: _LLD5: _tmp10E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF2)->f1;_LLD6:
# 699
 for(0;_tmp10E != 0;_tmp10E=_tmp10E->tl){
struct _tuple17*_tmp103=(struct _tuple17*)_tmp10E->hd;struct _tuple17*_tmp104=_tmp103;struct Cyc_Absyn_Tqual _tmp108;void*_tmp107;_LLDD: _tmp108=_tmp104->f1;_tmp107=_tmp104->f2;_LLDE:;
if(_tmp108.real_const){
({void*_tmp105=0;({unsigned int _tmp5EA=loc;struct _dyneither_ptr _tmp5E9=({const char*_tmp106="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmp106,sizeof(char),56);});Cyc_Tcutil_terr(_tmp5EA,_tmp5E9,_tag_dyneither(_tmp105,sizeof(void*),0));});});
return 0;}
# 705
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp107))return 0;}
# 707
return 1;default: _LLD7: _LLD8:
 return 1;}_LLCC:;}
# 715
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 718
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp10F=e->r;void*_tmp110=_tmp10F;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct _dyneither_ptr*_tmp13B;struct Cyc_Absyn_Exp*_tmp13A;struct _dyneither_ptr*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Vardecl*_tmp136;struct Cyc_Absyn_Vardecl*_tmp135;struct Cyc_Absyn_Vardecl*_tmp134;struct Cyc_Absyn_Vardecl*_tmp133;switch(*((int*)_tmp110)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp110)->f1)){case 3: _LLE0: _tmp133=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp110)->f1)->f1;_LLE1:
 _tmp134=_tmp133;goto _LLE3;case 4: _LLE2: _tmp134=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp110)->f1)->f1;_LLE3:
 _tmp135=_tmp134;goto _LLE5;case 5: _LLE4: _tmp135=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp110)->f1)->f1;_LLE5:
 _tmp136=_tmp135;goto _LLE7;case 1: _LLE6: _tmp136=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp110)->f1)->f1;_LLE7:
 if(!(_tmp136->tq).real_const)return;goto _LLDF;default: goto _LLF4;}case 22: _LLE8: _tmp138=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp137=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_LLE9:
# 725
{void*_tmp111=Cyc_Tcutil_compress((void*)_check_null(_tmp138->topt));void*_tmp112=_tmp111;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_Tqual _tmp122;struct Cyc_Absyn_Tqual _tmp121;switch(*((int*)_tmp112)){case 5: _LLF7: _tmp121=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp112)->f1).elt_tq;_LLF8:
 _tmp122=_tmp121;goto _LLFA;case 8: _LLF9: _tmp122=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp112)->f1).tq;_LLFA:
 if(!_tmp122.real_const)return;goto _LLF6;case 10: _LLFB: _tmp123=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp112)->f1;_LLFC: {
# 729
struct _tuple15 _tmp113=Cyc_Evexp_eval_const_uint_exp(_tmp137);struct _tuple15 _tmp114=_tmp113;unsigned int _tmp120;int _tmp11F;_LL100: _tmp120=_tmp114.f1;_tmp11F=_tmp114.f2;_LL101:;
if(!_tmp11F){
({void*_tmp115=0;({unsigned int _tmp5EC=e->loc;struct _dyneither_ptr _tmp5EB=({const char*_tmp116="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp116,sizeof(char),47);});Cyc_Tcutil_terr(_tmp5EC,_tmp5EB,_tag_dyneither(_tmp115,sizeof(void*),0));});});
return;}
# 734
{struct _handler_cons _tmp117;_push_handler(& _tmp117);{int _tmp119=0;if(setjmp(_tmp117.handler))_tmp119=1;if(!_tmp119){
{struct _tuple17*_tmp11A=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp123,(int)_tmp120);struct _tuple17*_tmp11B=_tmp11A;struct Cyc_Absyn_Tqual _tmp11C;_LL103: _tmp11C=_tmp11B->f1;_LL104:;
if(!_tmp11C.real_const){_npop_handler(0);return;}}
# 735
;_pop_handler();}else{void*_tmp118=(void*)_exn_thrown;void*_tmp11D=_tmp118;void*_tmp11E;if(((struct Cyc_List_Nth_exn_struct*)_tmp11D)->tag == Cyc_List_Nth){_LL106: _LL107:
# 737
 return;}else{_LL108: _tmp11E=_tmp11D;_LL109:(int)_rethrow(_tmp11E);}_LL105:;}};}
goto _LLF6;}default: _LLFD: _LLFE:
 goto _LLF6;}_LLF6:;}
# 741
goto _LLDF;case 20: _LLEA: _tmp13A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp139=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_LLEB:
# 743
{void*_tmp124=Cyc_Tcutil_compress((void*)_check_null(_tmp13A->topt));void*_tmp125=_tmp124;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Aggrdecl**_tmp126;switch(*((int*)_tmp125)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp125)->f1).aggr_info).KnownAggr).tag == 2){_LL10B: _tmp126=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp125)->f1).aggr_info).KnownAggr).val;_LL10C: {
# 745
struct Cyc_Absyn_Aggrfield*sf=
_tmp126 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp126,_tmp139);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}}else{goto _LL10F;}case 12: _LL10D: _tmp127=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp125)->f2;_LL10E: {
# 750
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp127,_tmp139);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}default: _LL10F: _LL110:
 goto _LL10A;}_LL10A:;}
# 755
goto _LLDF;case 21: _LLEC: _tmp13C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_tmp13B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp110)->f2;_LLED:
# 757
{void*_tmp128=Cyc_Tcutil_compress((void*)_check_null(_tmp13C->topt));void*_tmp129=_tmp128;void*_tmp12E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp129)->tag == 5){_LL112: _tmp12E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp129)->f1).elt_typ;_LL113:
# 759
{void*_tmp12A=Cyc_Tcutil_compress(_tmp12E);void*_tmp12B=_tmp12A;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_Aggrdecl**_tmp12C;switch(*((int*)_tmp12B)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12B)->f1).aggr_info).KnownAggr).tag == 2){_LL117: _tmp12C=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12B)->f1).aggr_info).KnownAggr).val;_LL118: {
# 761
struct Cyc_Absyn_Aggrfield*sf=
_tmp12C == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp12C,_tmp13B);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}}else{goto _LL11B;}case 12: _LL119: _tmp12D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12B)->f2;_LL11A: {
# 766
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp12D,_tmp13B);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}default: _LL11B: _LL11C:
 goto _LL116;}_LL116:;}
# 771
goto _LL111;}else{_LL114: _LL115:
 goto _LL111;}_LL111:;}
# 774
goto _LLDF;case 19: _LLEE: _tmp13D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_LLEF:
# 776
{void*_tmp12F=Cyc_Tcutil_compress((void*)_check_null(_tmp13D->topt));void*_tmp130=_tmp12F;struct Cyc_Absyn_Tqual _tmp132;struct Cyc_Absyn_Tqual _tmp131;switch(*((int*)_tmp130)){case 5: _LL11E: _tmp131=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp130)->f1).elt_tq;_LL11F:
 _tmp132=_tmp131;goto _LL121;case 8: _LL120: _tmp132=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp130)->f1).tq;_LL121:
 if(!_tmp132.real_const)return;goto _LL11D;default: _LL122: _LL123:
 goto _LL11D;}_LL11D:;}
# 781
goto _LLDF;case 11: _LLF0: _tmp13E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_LLF1:
 _tmp13F=_tmp13E;goto _LLF3;case 12: _LLF2: _tmp13F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp110)->f1;_LLF3:
 Cyc_Tcexp_check_writable(te,_tmp13F);return;default: _LLF4: _LLF5:
 goto _LLDF;}_LLDF:;}
# 786
({struct Cyc_String_pa_PrintArg_struct _tmp142;_tmp142.tag=0;({struct _dyneither_ptr _tmp5ED=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp142.f1=_tmp5ED;});({void*_tmp140[1]={& _tmp142};({unsigned int _tmp5EF=e->loc;struct _dyneither_ptr _tmp5EE=({const char*_tmp141="attempt to write a const location: %s";_tag_dyneither(_tmp141,sizeof(char),38);});Cyc_Tcutil_terr(_tmp5EF,_tmp5EE,_tag_dyneither(_tmp140,sizeof(void*),1));});});});}
# 789
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 792
({struct Cyc_Tcenv_Tenv*_tmp5F0=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp5F0,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp143=0;({struct Cyc_Tcenv_Tenv*_tmp5F4=te;unsigned int _tmp5F3=loc;void**_tmp5F2=topt;struct _dyneither_ptr _tmp5F1=({const char*_tmp144="increment/decrement of non-lvalue";_tag_dyneither(_tmp144,sizeof(char),34);});Cyc_Tcexp_expr_err(_tmp5F4,_tmp5F3,_tmp5F2,_tmp5F1,_tag_dyneither(_tmp143,sizeof(void*),0));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 798
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk);}))
({void*_tmp145=0;({unsigned int _tmp5F6=e->loc;struct _dyneither_ptr _tmp5F5=({const char*_tmp146="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp146,sizeof(char),50);});Cyc_Tcutil_terr(_tmp5F6,_tmp5F5,_tag_dyneither(_tmp145,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp147=0;({unsigned int _tmp5F8=e->loc;struct _dyneither_ptr _tmp5F7=({const char*_tmp148="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp148,sizeof(char),54);});Cyc_Tcutil_terr(_tmp5F8,_tmp5F7,_tag_dyneither(_tmp147,sizeof(void*),0));});});}else{
# 807
({struct Cyc_String_pa_PrintArg_struct _tmp14B;_tmp14B.tag=0;({struct _dyneither_ptr _tmp5F9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp14B.f1=_tmp5F9;});({void*_tmp149[1]={& _tmp14B};({unsigned int _tmp5FB=e->loc;struct _dyneither_ptr _tmp5FA=({const char*_tmp14A="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp14A,sizeof(char),44);});Cyc_Tcutil_terr(_tmp5FB,_tmp5FA,_tag_dyneither(_tmp149,sizeof(void*),1));});});});}}
# 809
return t;};}
# 813
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 815
({struct Cyc_Tcenv_Tenv*_tmp5FD=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp5FC=e1;Cyc_Tcexp_tcTest(_tmp5FD,_tmp5FC,({const char*_tmp14C="conditional expression";_tag_dyneither(_tmp14C,sizeof(char),23);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp5FE=({Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));});t=_tmp5FE;});else{
# 822
({void*_tmp5FF=({Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));});t=_tmp5FF;});}{
struct Cyc_List_List _tmp14D=({struct Cyc_List_List _tmp154;_tmp154.hd=e3;_tmp154.tl=0;_tmp154;});
struct Cyc_List_List _tmp14E=({struct Cyc_List_List _tmp153;_tmp153.hd=e2;_tmp153.tl=& _tmp14D;_tmp153;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp14E)){
({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0;({struct _dyneither_ptr _tmp600=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp152.f1=_tmp600;});({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0;({struct _dyneither_ptr _tmp601=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp151.f1=_tmp601;});({void*_tmp14F[2]={& _tmp151,& _tmp152};({unsigned int _tmp603=loc;struct _dyneither_ptr _tmp602=({const char*_tmp150="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp150,sizeof(char),48);});Cyc_Tcutil_terr(_tmp603,_tmp602,_tag_dyneither(_tmp14F,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 830
return t;};};}
# 834
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 836
({struct Cyc_Tcenv_Tenv*_tmp605=te;struct Cyc_Absyn_Exp*_tmp604=e1;Cyc_Tcexp_tcTest(_tmp605,_tmp604,({const char*_tmp155="logical-and expression";_tag_dyneither(_tmp155,sizeof(char),23);}));});
({struct Cyc_Tcenv_Tenv*_tmp607=te;struct Cyc_Absyn_Exp*_tmp606=e2;Cyc_Tcexp_tcTest(_tmp607,_tmp606,({const char*_tmp156="logical-and expression";_tag_dyneither(_tmp156,sizeof(char),23);}));});
return Cyc_Absyn_sint_typ;}
# 842
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 844
({struct Cyc_Tcenv_Tenv*_tmp609=te;struct Cyc_Absyn_Exp*_tmp608=e1;Cyc_Tcexp_tcTest(_tmp609,_tmp608,({const char*_tmp157="logical-or expression";_tag_dyneither(_tmp157,sizeof(char),22);}));});
({struct Cyc_Tcenv_Tenv*_tmp60B=te;struct Cyc_Absyn_Exp*_tmp60A=e2;Cyc_Tcexp_tcTest(_tmp60B,_tmp60A,({const char*_tmp158="logical-or expression";_tag_dyneither(_tmp158,sizeof(char),22);}));});
return Cyc_Absyn_sint_typ;}
# 850
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 857
({struct Cyc_Tcenv_Tenv*_tmp60C=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp60C,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 862
{void*_tmp159=Cyc_Tcutil_compress(t1);void*_tmp15A=_tmp159;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15A)->tag == 8){_LL125: _LL126:
({void*_tmp15B=0;({unsigned int _tmp60E=loc;struct _dyneither_ptr _tmp60D=({const char*_tmp15C="cannot assign to an array";_tag_dyneither(_tmp15C,sizeof(char),26);});Cyc_Tcutil_terr(_tmp60E,_tmp60D,_tag_dyneither(_tmp15B,sizeof(void*),0));});});goto _LL124;}else{_LL127: _LL128:
 goto _LL124;}_LL124:;}
# 867
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk);}))
({void*_tmp15D=0;({unsigned int _tmp610=loc;struct _dyneither_ptr _tmp60F=({const char*_tmp15E="type is abstract (can't determine size).";_tag_dyneither(_tmp15E,sizeof(char),41);});Cyc_Tcutil_terr(_tmp610,_tmp60F,_tag_dyneither(_tmp15D,sizeof(void*),0));});});
# 871
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp15F=0;({struct Cyc_Tcenv_Tenv*_tmp614=te;unsigned int _tmp613=loc;void**_tmp612=topt;struct _dyneither_ptr _tmp611=({const char*_tmp160="assignment to non-lvalue";_tag_dyneither(_tmp160,sizeof(char),25);});Cyc_Tcexp_expr_err(_tmp614,_tmp613,_tmp612,_tmp611,_tag_dyneither(_tmp15F,sizeof(void*),0));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
{struct _RegionHandle*_tmp161=Cyc_Core_heap_region;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp161,t2) && !Cyc_Tcutil_is_noalias_path(_tmp161,e2))
({void*_tmp162=0;({unsigned int _tmp616=e2->loc;struct _dyneither_ptr _tmp615=({const char*_tmp163="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp163,sizeof(char),49);});Cyc_Tcutil_terr(_tmp616,_tmp615,_tag_dyneither(_tmp162,sizeof(void*),0));});});}
# 879
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp164=({struct Cyc_String_pa_PrintArg_struct _tmp168;_tmp168.tag=0;({struct _dyneither_ptr _tmp617=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp168.f1=_tmp617;});({struct Cyc_String_pa_PrintArg_struct _tmp167;_tmp167.tag=0;({struct _dyneither_ptr _tmp618=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp167.f1=_tmp618;});({void*_tmp165[2]={& _tmp167,& _tmp168};({struct Cyc_Tcenv_Tenv*_tmp61C=te;unsigned int _tmp61B=loc;void**_tmp61A=topt;struct _dyneither_ptr _tmp619=({const char*_tmp166="type mismatch: %s != %s";_tag_dyneither(_tmp166,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp61C,_tmp61B,_tmp61A,_tmp619,_tag_dyneither(_tmp165,sizeof(void*),2));});});});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp164;}}else{
# 887
enum Cyc_Absyn_Primop _tmp169=(enum Cyc_Absyn_Primop)po->v;
void*_tmp16A=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp169,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp16A,t1) || Cyc_Tcutil_coerceable(_tmp16A) && Cyc_Tcutil_coerceable(t1))){
void*_tmp16B=({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0;({struct _dyneither_ptr _tmp61D=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 893
Cyc_Absynpp_typ2string(t2));_tmp16F.f1=_tmp61D;});({struct Cyc_String_pa_PrintArg_struct _tmp16E;_tmp16E.tag=0;({struct _dyneither_ptr _tmp61E=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 892
Cyc_Absynpp_typ2string(t1));_tmp16E.f1=_tmp61E;});({void*_tmp16C[2]={& _tmp16E,& _tmp16F};({struct Cyc_Tcenv_Tenv*_tmp622=te;unsigned int _tmp621=loc;void**_tmp620=topt;struct _dyneither_ptr _tmp61F=({const char*_tmp16D="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp16D,sizeof(char),82);});Cyc_Tcexp_expr_err(_tmp622,_tmp621,_tmp620,_tmp61F,_tag_dyneither(_tmp16C,sizeof(void*),2));});});});});
# 894
Cyc_Tcutil_unify(_tmp16A,t1);
Cyc_Tcutil_explain_failure();
return _tmp16B;}
# 898
return _tmp16A;}
# 900
return t1;};};}
# 904
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp623=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp623,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp625=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp624=topt;Cyc_Tcexp_tcExp(_tmp625,_tmp624,e2);});
return(void*)_check_null(e2->topt);}
# 911
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 915
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 918
{void*_tmp170=Cyc_Tcutil_compress(t1);void*_tmp171=_tmp170;switch(*((int*)_tmp171)){case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp171)->f1 == 0){_LL12A: _LL12B:
({struct Cyc_Tcenv_Tenv*_tmp627=te;struct Cyc_Absyn_Exp*_tmp626=e;Cyc_Tcutil_unchecked_cast(_tmp627,_tmp626,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);});({void*_tmp628=Cyc_Absyn_float_typ(1);t1=_tmp628;});goto _LL129;}else{goto _LL130;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp171)->f2){case Cyc_Absyn_Char_sz: _LL12C: _LL12D:
 goto _LL12F;case Cyc_Absyn_Short_sz: _LL12E: _LL12F:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL129;default: goto _LL130;}default: _LL130: _LL131:
 goto _LL129;}_LL129:;}
# 925
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp172=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp173=_tmp172;struct _tuple1*_tmp177;struct Cyc_List_List*_tmp176;unsigned int _tmp175;enum Cyc_Absyn_Scope _tmp174;_LL133: _tmp177=_tmp173.name;_tmp176=_tmp173.typs;_tmp175=_tmp173.loc;_tmp174=_tmp173.sc;_LL134:;
# 928
if(_tmp176 == 0  || _tmp176->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp176->hd)).f2);
# 931
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 936
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp178=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp179=_tmp178;struct _tuple1*_tmp17D;struct Cyc_List_List*_tmp17C;unsigned int _tmp17B;enum Cyc_Absyn_Scope _tmp17A;_LL136: _tmp17D=_tmp179.name;_tmp17C=_tmp179.typs;_tmp17B=_tmp179.loc;_tmp17A=_tmp179.sc;_LL137:;
# 939
if(_tmp17C == 0  || _tmp17C->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp17C->hd)).f2);
# 943
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 948
({struct Cyc_String_pa_PrintArg_struct _tmp181;_tmp181.tag=0;({struct _dyneither_ptr _tmp629=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));_tmp181.f1=_tmp629;});({struct Cyc_String_pa_PrintArg_struct _tmp180;_tmp180.tag=0;({struct _dyneither_ptr _tmp62A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp180.f1=_tmp62A;});({void*_tmp17E[2]={& _tmp180,& _tmp181};({unsigned int _tmp62C=e->loc;struct _dyneither_ptr _tmp62B=({const char*_tmp17F="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp17F,sizeof(char),54);});Cyc_Tcutil_terr(_tmp62C,_tmp62B,_tag_dyneither(_tmp17E,sizeof(void*),2));});});});});
return 0;}
# 954
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 960
struct Cyc_List_List*_tmp182=args;
int _tmp183=0;
struct _RegionHandle*_tmp184=Cyc_Core_heap_region;
struct Cyc_Tcenv_Tenv*_tmp185=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp186=Cyc_Tcenv_clear_abstract_val_ok(_tmp185);
Cyc_Tcexp_tcExp(_tmp186,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 970
void*_tmp187=t;void*_tmp1EC;struct Cyc_Absyn_Tqual _tmp1EB;void*_tmp1EA;union Cyc_Absyn_Constraint*_tmp1E9;union Cyc_Absyn_Constraint*_tmp1E8;union Cyc_Absyn_Constraint*_tmp1E7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187)->tag == 5){_LL139: _tmp1EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187)->f1).elt_typ;_tmp1EB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187)->f1).elt_tq;_tmp1EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187)->f1).ptr_atts).rgn;_tmp1E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187)->f1).ptr_atts).nullable;_tmp1E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187)->f1).ptr_atts).bounds;_tmp1E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp187)->f1).ptr_atts).zero_term;_LL13A:
# 975
 Cyc_Tcenv_check_rgn_accessible(_tmp186,loc,_tmp1EA);
# 977
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1E8);{
void*_tmp188=Cyc_Tcutil_compress(_tmp1EC);void*_tmp189=_tmp188;struct Cyc_List_List*_tmp1E3;void*_tmp1E2;struct Cyc_Absyn_Tqual _tmp1E1;void*_tmp1E0;struct Cyc_List_List*_tmp1DF;int _tmp1DE;struct Cyc_Absyn_VarargInfo*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_List_List*_tmp1D7;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->tag == 9){_LL13E: _tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).tvars;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).effect;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).ret_tqual;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).ret_typ;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).args;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).c_varargs;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).cyc_varargs;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).rgn_po;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).attributes;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).requires_clause;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).requires_relns;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).ensures_clause;_tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp189)->f1).ensures_relns;_LL13F:
# 982
 if(_tmp1E3 != 0  || _tmp1DC != 0)
({void*_tmp18A=0;({unsigned int _tmp62E=e->loc;struct _dyneither_ptr _tmp62D=({const char*_tmp18B="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp18B,sizeof(char),79);});Cyc_Tcutil_terr(_tmp62E,_tmp62D,_tag_dyneither(_tmp18A,sizeof(void*),0));});});
# 986
if(topt != 0)Cyc_Tcutil_unify(_tmp1E0,*topt);
# 988
while(_tmp182 != 0  && _tmp1DF != 0){
# 990
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp182->hd;
void*t2=(*((struct _tuple9*)_tmp1DF->hd)).f3;
Cyc_Tcexp_tcExp(_tmp186,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp186,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp19B="actual argument has type ";_tag_dyneither(_tmp19B,sizeof(char),26);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp19A=" but formal has type ";_tag_dyneither(_tmp19A,sizeof(char),22);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({({unsigned long _tmp631=({unsigned long _tmp630=({unsigned long _tmp62F=Cyc_strlen((struct _dyneither_ptr)s0);_tmp62F + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp630 + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp631 + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70;}))
({void*_tmp18C=0;({unsigned int _tmp634=e1->loc;struct _dyneither_ptr _tmp633=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp192;_tmp192.tag=0;_tmp192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp191;_tmp191.tag=0;_tmp191.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp190;_tmp190.tag=0;_tmp190.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp18D[4]={& _tmp18F,& _tmp190,& _tmp191,& _tmp192};({struct _dyneither_ptr _tmp632=({const char*_tmp18E="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp18E,sizeof(char),15);});Cyc_aprintf(_tmp632,_tag_dyneither(_tmp18D,sizeof(void*),4));});});});});});});Cyc_Tcutil_terr(_tmp634,_tmp633,_tag_dyneither(_tmp18C,sizeof(void*),0));});});else{
# 1002
({void*_tmp193=0;({unsigned int _tmp637=e1->loc;struct _dyneither_ptr _tmp636=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0;_tmp199.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0;_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp197;_tmp197.tag=0;_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp196;_tmp196.tag=0;_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp194[4]={& _tmp196,& _tmp197,& _tmp198,& _tmp199};({struct _dyneither_ptr _tmp635=({const char*_tmp195="%s%s%s%s.";_tag_dyneither(_tmp195,sizeof(char),10);});Cyc_aprintf(_tmp635,_tag_dyneither(_tmp194,sizeof(void*),4));});});});});});});Cyc_Tcutil_terr(_tmp637,_tmp636,_tag_dyneither(_tmp193,sizeof(void*),0));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1007
if(alias_coercion)
({struct Cyc_List_List*_tmp638=({struct Cyc_List_List*_tmp19C=_cycalloc(sizeof(*_tmp19C));_tmp19C->hd=(void*)_tmp183;_tmp19C->tl=*alias_arg_exps;_tmp19C;});*alias_arg_exps=_tmp638;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp184,t2) && !Cyc_Tcutil_is_noalias_path(_tmp184,e1))
({void*_tmp19D=0;({unsigned int _tmp63A=e1->loc;struct _dyneither_ptr _tmp639=({const char*_tmp19E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp19E,sizeof(char),49);});Cyc_Tcutil_terr(_tmp63A,_tmp639,_tag_dyneither(_tmp19D,sizeof(void*),0));});});
_tmp182=_tmp182->tl;
_tmp1DF=_tmp1DF->tl;
++ _tmp183;}{
# 1018
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1DB;for(0;a != 0;a=a->tl){
void*_tmp19F=(void*)a->hd;void*_tmp1A0=_tmp19F;enum Cyc_Absyn_Format_Type _tmp1AE;int _tmp1AD;int _tmp1AC;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A0)->tag == 19){_LL143: _tmp1AE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A0)->f1;_tmp1AD=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A0)->f2;_tmp1AC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A0)->f3;_LL144:
# 1022
{struct _handler_cons _tmp1A1;_push_handler(& _tmp1A1);{int _tmp1A3=0;if(setjmp(_tmp1A1.handler))_tmp1A3=1;if(!_tmp1A3){
# 1024
{struct Cyc_Absyn_Exp*_tmp1A4=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1AD - 1);
# 1026
struct Cyc_Core_Opt*fmt_args;
if(_tmp1AC == 0)
fmt_args=0;else{
# 1030
({struct Cyc_Core_Opt*_tmp63C=({struct Cyc_Core_Opt*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));({struct Cyc_List_List*_tmp63B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1AC - 1);_tmp1A5->v=_tmp63B;});_tmp1A5;});fmt_args=_tmp63C;});}
args_already_checked=1;{
struct _RegionHandle*_tmp1A6=Cyc_Core_heap_region;
enum Cyc_Absyn_Format_Type _tmp1A7=_tmp1AE;if(_tmp1A7 == Cyc_Absyn_Printf_ft){_LL148: _LL149:
# 1035
 Cyc_Tcexp_check_format_args(_tmp186,_tmp1A4,fmt_args,_tmp1AC - 1,alias_arg_exps,_tmp1A6,Cyc_Formatstr_get_format_typs);
# 1038
goto _LL147;}else{_LL14A: _LL14B:
# 1040
 Cyc_Tcexp_check_format_args(_tmp186,_tmp1A4,fmt_args,_tmp1AC - 1,alias_arg_exps,_tmp1A6,Cyc_Formatstr_get_scanf_typs);
# 1043
goto _LL147;}_LL147:;};}
# 1024
;_pop_handler();}else{void*_tmp1A2=(void*)_exn_thrown;void*_tmp1A8=_tmp1A2;void*_tmp1AB;if(((struct Cyc_List_Nth_exn_struct*)_tmp1A8)->tag == Cyc_List_Nth){_LL14D: _LL14E:
# 1047
({void*_tmp1A9=0;({unsigned int _tmp63E=loc;struct _dyneither_ptr _tmp63D=({const char*_tmp1AA="bad format arguments";_tag_dyneither(_tmp1AA,sizeof(char),21);});Cyc_Tcutil_terr(_tmp63E,_tmp63D,_tag_dyneither(_tmp1A9,sizeof(void*),0));});});goto _LL14C;}else{_LL14F: _tmp1AB=_tmp1A8;_LL150:(int)_rethrow(_tmp1AB);}_LL14C:;}};}
# 1049
goto _LL142;}else{_LL145: _LL146:
 goto _LL142;}_LL142:;}}
# 1053
if(_tmp1DF != 0)({void*_tmp1AF=0;({unsigned int _tmp640=loc;struct _dyneither_ptr _tmp63F=({const char*_tmp1B0="too few arguments for function";_tag_dyneither(_tmp1B0,sizeof(char),31);});Cyc_Tcutil_terr(_tmp640,_tmp63F,_tag_dyneither(_tmp1AF,sizeof(void*),0));});});else{
# 1055
if((_tmp182 != 0  || _tmp1DE) || _tmp1DD != 0){
if(_tmp1DE)
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
Cyc_Tcexp_tcExp(_tmp186,0,(struct Cyc_Absyn_Exp*)_tmp182->hd);}else{
if(_tmp1DD == 0)
({void*_tmp1B1=0;({unsigned int _tmp642=loc;struct _dyneither_ptr _tmp641=({const char*_tmp1B2="too many arguments for function";_tag_dyneither(_tmp1B2,sizeof(char),32);});Cyc_Tcutil_terr(_tmp642,_tmp641,_tag_dyneither(_tmp1B1,sizeof(void*),0));});});else{
# 1062
struct Cyc_Absyn_VarargInfo _tmp1B3=*_tmp1DD;struct Cyc_Absyn_VarargInfo _tmp1B4=_tmp1B3;void*_tmp1D3;int _tmp1D2;_LL152: _tmp1D3=_tmp1B4.type;_tmp1D2=_tmp1B4.inject;_LL153:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1B5=({struct Cyc_Absyn_VarargCallInfo*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));_tmp1D1->num_varargs=0;_tmp1D1->injectors=0;_tmp1D1->vai=_tmp1DD;_tmp1D1;});
# 1066
*vararg_call_info=_tmp1B5;
# 1068
if(!_tmp1D2)
# 1070
for(0;_tmp182 != 0;(_tmp182=_tmp182->tl,_tmp183 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp182->hd;
++ _tmp1B5->num_varargs;
Cyc_Tcexp_tcExp(_tmp186,& _tmp1D3,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp186,e1,_tmp1D3,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1B9;_tmp1B9.tag=0;({struct _dyneither_ptr _tmp643=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp1B9.f1=_tmp643;});({struct Cyc_String_pa_PrintArg_struct _tmp1B8;_tmp1B8.tag=0;({struct _dyneither_ptr _tmp644=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D3));_tmp1B8.f1=_tmp644;});({void*_tmp1B6[2]={& _tmp1B8,& _tmp1B9};({unsigned int _tmp646=loc;struct _dyneither_ptr _tmp645=({const char*_tmp1B7="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1B7,sizeof(char),49);});Cyc_Tcutil_terr(_tmp646,_tmp645,_tag_dyneither(_tmp1B6,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 1080
if(alias_coercion)
({struct Cyc_List_List*_tmp647=({struct Cyc_List_List*_tmp1BA=_cycalloc(sizeof(*_tmp1BA));_tmp1BA->hd=(void*)_tmp183;_tmp1BA->tl=*alias_arg_exps;_tmp1BA;});*alias_arg_exps=_tmp647;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp184,_tmp1D3) && !
Cyc_Tcutil_is_noalias_path(_tmp184,e1))
({void*_tmp1BB=0;({unsigned int _tmp649=e1->loc;struct _dyneither_ptr _tmp648=({const char*_tmp1BC="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1BC,sizeof(char),49);});Cyc_Tcutil_terr(_tmp649,_tmp648,_tag_dyneither(_tmp1BB,sizeof(void*),0));});});}else{
# 1089
void*_tmp1BD=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1D3));void*_tmp1BE=_tmp1BD;struct Cyc_Absyn_Datatypedecl*_tmp1D0;struct Cyc_List_List*_tmp1CF;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BE)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BE)->f1).datatype_info).KnownDatatype).tag == 2){_LL155: _tmp1D0=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BE)->f1).datatype_info).KnownDatatype).val;_tmp1CF=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BE)->f1).targs;_LL156: {
# 1093
struct Cyc_Absyn_Datatypedecl*_tmp1BF=*Cyc_Tcenv_lookup_datatypedecl(_tmp186,loc,_tmp1D0->name);
struct Cyc_List_List*fields=0;
if(_tmp1BF->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1C2;_tmp1C2.tag=0;({struct _dyneither_ptr _tmp64A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1D3));_tmp1C2.f1=_tmp64A;});({void*_tmp1C0[1]={& _tmp1C2};({unsigned int _tmp64C=loc;struct _dyneither_ptr _tmp64B=({const char*_tmp1C1="can't inject into abstract datatype %s";_tag_dyneither(_tmp1C1,sizeof(char),39);});Cyc_Tcutil_terr(_tmp64C,_tmp64B,_tag_dyneither(_tmp1C0,sizeof(void*),1));});});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1BF->fields))->v;}
# 1105
if(!({void*_tmp64D=Cyc_Tcutil_pointer_region(_tmp1D3);Cyc_Tcutil_unify(_tmp64D,Cyc_Tcenv_curr_rgn(_tmp186));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1C6;_tmp1C6.tag=0;({struct _dyneither_ptr _tmp64E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp186)));_tmp1C6.f1=_tmp64E;});({struct Cyc_String_pa_PrintArg_struct _tmp1C5;_tmp1C5.tag=0;({struct _dyneither_ptr _tmp64F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D3));_tmp1C5.f1=_tmp64F;});({void*_tmp1C3[2]={& _tmp1C5,& _tmp1C6};({unsigned int _tmp651=loc;struct _dyneither_ptr _tmp650=({const char*_tmp1C4="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1C4,sizeof(char),49);});Cyc_Tcutil_terr(_tmp651,_tmp650,_tag_dyneither(_tmp1C3,sizeof(void*),2));});});});});{
# 1109
struct _RegionHandle*_tmp1C7=Cyc_Core_heap_region;{
struct Cyc_List_List*_tmp1C8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp1C7,_tmp1C7,_tmp1BF->tvs,_tmp1CF);
for(0;_tmp182 != 0;_tmp182=_tmp182->tl){
++ _tmp1B5->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp182->hd;
# 1115
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp186,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1C7,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(_tmp1C7,e1))
({void*_tmp1C9=0;({unsigned int _tmp653=e1->loc;struct _dyneither_ptr _tmp652=({const char*_tmp1CA="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1CA,sizeof(char),49);});Cyc_Tcutil_terr(_tmp653,_tmp652,_tag_dyneither(_tmp1C9,sizeof(void*),0));});});}{
# 1121
struct Cyc_Absyn_Datatypefield*_tmp1CB=({struct Cyc_Tcenv_Tenv*_tmp658=_tmp186;struct Cyc_Absyn_Exp*_tmp657=e1;void*_tmp656=Cyc_Tcutil_pointer_elt_type(_tmp1D3);struct _RegionHandle*_tmp655=_tmp1C7;struct Cyc_List_List*_tmp654=_tmp1C8;Cyc_Tcexp_tcInjection(_tmp658,_tmp657,_tmp656,_tmp655,_tmp654,fields);});
# 1123
if(_tmp1CB != 0)
({struct Cyc_List_List*_tmp65A=({
struct Cyc_List_List*_tmp659=_tmp1B5->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp659,({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=_tmp1CB;_tmp1CC->tl=0;_tmp1CC;}));});
# 1124
_tmp1B5->injectors=_tmp65A;});};};}}
# 1129
goto _LL154;};}}else{goto _LL157;}}else{_LL157: _LL158:
({void*_tmp1CD=0;({unsigned int _tmp65C=loc;struct _dyneither_ptr _tmp65B=({const char*_tmp1CE="bad inject vararg type";_tag_dyneither(_tmp1CE,sizeof(char),23);});Cyc_Tcutil_terr(_tmp65C,_tmp65B,_tag_dyneither(_tmp1CD,sizeof(void*),0));});});goto _LL154;}_LL154:;}};}}}}
# 1135
if(*alias_arg_exps == 0)
# 1144 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp186,loc,(void*)_check_null(_tmp1E2));
# 1146
return _tmp1E0;};}else{_LL140: _LL141:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1D6;_tmp1D6.tag=0;({struct _dyneither_ptr _tmp65D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1D6.f1=_tmp65D;});({void*_tmp1D4[1]={& _tmp1D6};({struct Cyc_Tcenv_Tenv*_tmp661=_tmp186;unsigned int _tmp660=loc;void**_tmp65F=topt;struct _dyneither_ptr _tmp65E=({const char*_tmp1D5="expected pointer to function but found %s";_tag_dyneither(_tmp1D5,sizeof(char),42);});Cyc_Tcexp_expr_err(_tmp661,_tmp660,_tmp65F,_tmp65E,_tag_dyneither(_tmp1D4,sizeof(void*),1));});});});}_LL13D:;};}else{_LL13B: _LL13C:
# 1149
 return({struct Cyc_String_pa_PrintArg_struct _tmp1E6;_tmp1E6.tag=0;({struct _dyneither_ptr _tmp662=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1E6.f1=_tmp662;});({void*_tmp1E4[1]={& _tmp1E6};({struct Cyc_Tcenv_Tenv*_tmp666=_tmp186;unsigned int _tmp665=loc;void**_tmp664=topt;struct _dyneither_ptr _tmp663=({const char*_tmp1E5="expected pointer to function but found %s";_tag_dyneither(_tmp1E5,sizeof(char),42);});Cyc_Tcexp_expr_err(_tmp666,_tmp665,_tmp664,_tmp663,_tag_dyneither(_tmp1E4,sizeof(void*),1));});});});}_LL138:;};}
# 1155
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1ED=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1ED,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1ED,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0;({struct _dyneither_ptr _tmp667=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp1F1.f1=_tmp667;});({struct Cyc_String_pa_PrintArg_struct _tmp1F0;_tmp1F0.tag=0;({struct _dyneither_ptr _tmp668=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1160
Cyc_Absynpp_typ2string(_tmp1ED));_tmp1F0.f1=_tmp668;});({void*_tmp1EE[2]={& _tmp1F0,& _tmp1F1};({unsigned int _tmp66A=loc;struct _dyneither_ptr _tmp669=({const char*_tmp1EF="expected %s but found %s";_tag_dyneither(_tmp1EF,sizeof(char),25);});Cyc_Tcutil_terr(_tmp66A,_tmp669,_tag_dyneither(_tmp1EE,sizeof(void*),2));});});});});
# 1162
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1167
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1169
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1171
({void*_tmp66B=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp66B;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1F2=t1;void*_tmp212;struct Cyc_Absyn_Tqual _tmp211;void*_tmp210;union Cyc_Absyn_Constraint*_tmp20F;union Cyc_Absyn_Constraint*_tmp20E;union Cyc_Absyn_Constraint*_tmp20D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F2)->tag == 5){_LL15A: _tmp212=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F2)->f1).elt_typ;_tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F2)->f1).elt_tq;_tmp210=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F2)->f1).ptr_atts).rgn;_tmp20F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F2)->f1).ptr_atts).nullable;_tmp20E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F2)->f1).ptr_atts).bounds;_tmp20D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F2)->f1).ptr_atts).zero_term;_LL15B:
# 1175
{void*_tmp1F3=Cyc_Tcutil_compress(_tmp212);void*_tmp1F4=_tmp1F3;struct Cyc_List_List*_tmp20C;void*_tmp20B;struct Cyc_Absyn_Tqual _tmp20A;void*_tmp209;struct Cyc_List_List*_tmp208;int _tmp207;struct Cyc_Absyn_VarargInfo*_tmp206;struct Cyc_List_List*_tmp205;struct Cyc_List_List*_tmp204;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_List_List*_tmp202;struct Cyc_Absyn_Exp*_tmp201;struct Cyc_List_List*_tmp200;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->tag == 9){_LL15F: _tmp20C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).tvars;_tmp20B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).effect;_tmp20A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).ret_tqual;_tmp209=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).ret_typ;_tmp208=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).args;_tmp207=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).c_varargs;_tmp206=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).cyc_varargs;_tmp205=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).rgn_po;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).attributes;_tmp203=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).requires_clause;_tmp202=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).requires_relns;_tmp201=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).ensures_clause;_tmp200=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F4)->f1).ensures_relns;_LL160: {
# 1177
struct _RegionHandle*_tmp1F5=Cyc_Core_heap_region;
struct Cyc_List_List*instantiation=0;
# 1180
for(0;ts != 0  && _tmp20C != 0;(ts=ts->tl,_tmp20C=_tmp20C->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp20C->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp66F=loc;struct Cyc_Tcenv_Tenv*_tmp66E=te;struct Cyc_List_List*_tmp66D=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp66C=k;Cyc_Tcutil_check_type(_tmp66F,_tmp66E,_tmp66D,_tmp66C,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp671=({struct Cyc_List_List*_tmp1F6=_region_malloc(_tmp1F5,sizeof(*_tmp1F6));({struct _tuple13*_tmp670=({struct _tuple13*_tmp1F7=_region_malloc(_tmp1F5,sizeof(*_tmp1F7));_tmp1F7->f1=(struct Cyc_Absyn_Tvar*)_tmp20C->hd;_tmp1F7->f2=(void*)ts->hd;_tmp1F7;});_tmp1F6->hd=_tmp670;});_tmp1F6->tl=instantiation;_tmp1F6;});instantiation=_tmp671;});}
# 1187
if(ts != 0)
return({void*_tmp1F8=0;({struct Cyc_Tcenv_Tenv*_tmp675=te;unsigned int _tmp674=loc;void**_tmp673=topt;struct _dyneither_ptr _tmp672=({const char*_tmp1F9="too many type variables in instantiation";_tag_dyneither(_tmp1F9,sizeof(char),41);});Cyc_Tcexp_expr_err(_tmp675,_tmp674,_tmp673,_tmp672,_tag_dyneither(_tmp1F8,sizeof(void*),0));});});
# 1193
if(_tmp20C == 0){
({struct Cyc_List_List*_tmp676=Cyc_Tcutil_rsubst_rgnpo(_tmp1F5,instantiation,_tmp205);_tmp205=_tmp676;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp205);
_tmp205=0;}{
# 1198
void*new_fn_typ=({
struct _RegionHandle*_tmp67A=_tmp1F5;struct Cyc_List_List*_tmp679=instantiation;Cyc_Tcutil_rsubstitute(_tmp67A,_tmp679,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp678=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1FE;_tmp1FE.tag=9;({struct Cyc_Absyn_FnInfo _tmp677=({struct Cyc_Absyn_FnInfo _tmp1FF;_tmp1FF.tvars=_tmp20C;_tmp1FF.effect=_tmp20B;_tmp1FF.ret_tqual=_tmp20A;_tmp1FF.ret_typ=_tmp209;_tmp1FF.args=_tmp208;_tmp1FF.c_varargs=_tmp207;_tmp1FF.cyc_varargs=_tmp206;_tmp1FF.rgn_po=_tmp205;_tmp1FF.attributes=_tmp204;_tmp1FF.requires_clause=_tmp203;_tmp1FF.requires_relns=_tmp202;_tmp1FF.ensures_clause=_tmp201;_tmp1FF.ensures_relns=_tmp200;_tmp1FF;});_tmp1FE.f1=_tmp677;});_tmp1FE;});_tmp1FD[0]=_tmp678;});_tmp1FD;}));});
# 1203
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp67D=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1FB;_tmp1FB.tag=5;({struct Cyc_Absyn_PtrInfo _tmp67C=({struct Cyc_Absyn_PtrInfo _tmp1FC;_tmp1FC.elt_typ=new_fn_typ;_tmp1FC.elt_tq=_tmp211;({struct Cyc_Absyn_PtrAtts _tmp67B=({(_tmp1FC.ptr_atts).rgn=_tmp210;(_tmp1FC.ptr_atts).nullable=_tmp20F;(_tmp1FC.ptr_atts).bounds=_tmp20E;(_tmp1FC.ptr_atts).zero_term=_tmp20D;(_tmp1FC.ptr_atts).ptrloc=0;_tmp1FC.ptr_atts;});_tmp1FC.ptr_atts=_tmp67B;});_tmp1FC;});_tmp1FB.f1=_tmp67C;});_tmp1FB;});_tmp1FA[0]=_tmp67D;});_tmp1FA;});};}}else{_LL161: _LL162:
# 1205
 goto _LL15E;}_LL15E:;}
# 1207
goto _LL159;}else{_LL15C: _LL15D:
 goto _LL159;}_LL159:;}
# 1210
return({struct Cyc_String_pa_PrintArg_struct _tmp215;_tmp215.tag=0;({struct _dyneither_ptr _tmp67E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));_tmp215.f1=_tmp67E;});({void*_tmp213[1]={& _tmp215};({struct Cyc_Tcenv_Tenv*_tmp682=te;unsigned int _tmp681=loc;void**_tmp680=topt;struct _dyneither_ptr _tmp67F=({const char*_tmp214="expecting polymorphic type but found %s";_tag_dyneither(_tmp214,sizeof(char),40);});Cyc_Tcexp_expr_err(_tmp682,_tmp681,_tmp680,_tmp67F,_tag_dyneither(_tmp213,sizeof(void*),1));});});});};}
# 1215
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1217
({unsigned int _tmp686=loc;struct Cyc_Tcenv_Tenv*_tmp685=te;struct Cyc_List_List*_tmp684=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp683=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1217
Cyc_Tcutil_check_type(_tmp686,_tmp685,_tmp684,_tmp683,1,0,t);});
# 1219
Cyc_Tcutil_check_no_qual(loc,t);
# 1221
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1226
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1233
Cyc_Tcutil_unify(t2,t);{
void*_tmp216=({struct Cyc_String_pa_PrintArg_struct _tmp21A;_tmp21A.tag=0;({struct _dyneither_ptr _tmp687=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp21A.f1=_tmp687;});({struct Cyc_String_pa_PrintArg_struct _tmp219;_tmp219.tag=0;({struct _dyneither_ptr _tmp688=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp219.f1=_tmp688;});({void*_tmp217[2]={& _tmp219,& _tmp21A};({struct Cyc_Tcenv_Tenv*_tmp68B=te;unsigned int _tmp68A=loc;struct _dyneither_ptr _tmp689=({const char*_tmp218="cannot cast %s to %s";_tag_dyneither(_tmp218,sizeof(char),21);});Cyc_Tcexp_expr_err(_tmp68B,_tmp68A,& t,_tmp689,_tag_dyneither(_tmp217,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return _tmp216;};}}}
# 1243
{struct _tuple0 _tmp21B=({struct _tuple0 _tmp226;_tmp226.f1=e->r;({void*_tmp68C=Cyc_Tcutil_compress(t);_tmp226.f2=_tmp68C;});_tmp226;});struct _tuple0 _tmp21C=_tmp21B;int _tmp225;union Cyc_Absyn_Constraint*_tmp224;union Cyc_Absyn_Constraint*_tmp223;union Cyc_Absyn_Constraint*_tmp222;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21C.f1)->tag == 33){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->tag == 5){_LL164: _tmp225=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21C.f1)->f1).fat_result;_tmp224=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).nullable;_tmp223=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).bounds;_tmp222=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).zero_term;_LL165:
# 1247
 if((_tmp225  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp222)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp224)){
void*_tmp21D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp223);void*_tmp21E=_tmp21D;struct Cyc_Absyn_Exp*_tmp221;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp21E)->tag == 1){_LL169: _tmp221=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp21E)->f1;_LL16A:
# 1250
 if(({(Cyc_Evexp_eval_const_uint_exp(_tmp221)).f1 == 1;}))
({void*_tmp21F=0;({unsigned int _tmp68E=loc;struct _dyneither_ptr _tmp68D=({const char*_tmp220="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp220,sizeof(char),60);});Cyc_Tcutil_warn(_tmp68E,_tmp68D,_tag_dyneither(_tmp21F,sizeof(void*),0));});});
goto _LL168;}else{_LL16B: _LL16C:
 goto _LL168;}_LL168:;}
# 1256
goto _LL163;}else{goto _LL166;}}else{_LL166: _LL167:
 goto _LL163;}_LL163:;}
# 1259
return t;};}
# 1263
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1265
void**_tmp227=0;
struct Cyc_Absyn_Tqual _tmp228=Cyc_Absyn_empty_tqual(0);
int _tmp229=0;
if(topt != 0){
void*_tmp22A=Cyc_Tcutil_compress(*topt);void*_tmp22B=_tmp22A;void**_tmp22E;struct Cyc_Absyn_Tqual _tmp22D;union Cyc_Absyn_Constraint*_tmp22C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->tag == 5){_LL16E: _tmp22E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_typ;_tmp22D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_tq;_tmp22C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).ptr_atts).nullable;_LL16F:
# 1271
 _tmp227=_tmp22E;
_tmp228=_tmp22D;
({int _tmp68F=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp22C);_tmp229=_tmp68F;});
goto _LL16D;}else{_LL170: _LL171:
 goto _LL16D;}_LL16D:;}
# 1285
({struct Cyc_Tcenv_Tenv*_tmp691=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp690=_tmp227;Cyc_Tcexp_tcExpNoInst(_tmp691,_tmp690,e);});
# 1288
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Core_heap_region,(void*)_check_null(e->topt)))
({void*_tmp22F=0;({unsigned int _tmp693=e->loc;struct _dyneither_ptr _tmp692=({const char*_tmp230="Cannot take the address of an alias-free path";_tag_dyneither(_tmp230,sizeof(char),46);});Cyc_Tcutil_terr(_tmp693,_tmp692,_tag_dyneither(_tmp22F,sizeof(void*),0));});});
# 1294
{void*_tmp231=e->r;void*_tmp232=_tmp231;struct Cyc_Absyn_Exp*_tmp236;struct Cyc_Absyn_Exp*_tmp235;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->tag == 22){_LL173: _tmp236=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->f1;_tmp235=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->f2;_LL174:
# 1296
{void*_tmp233=Cyc_Tcutil_compress((void*)_check_null(_tmp236->topt));void*_tmp234=_tmp233;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp234)->tag == 10){_LL178: _LL179:
 goto _LL177;}else{_LL17A: _LL17B:
# 1301
({void*_tmp694=(Cyc_Absyn_add_exp(_tmp236,_tmp235,0))->r;e0->r=_tmp694;});
return Cyc_Tcexp_tcPlus(te,_tmp236,_tmp235);}_LL177:;}
# 1304
goto _LL172;}else{_LL175: _LL176:
 goto _LL172;}_LL172:;}
# 1309
{void*_tmp237=e->r;void*_tmp238=_tmp237;switch(*((int*)_tmp238)){case 20: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp238)->f3 == 1){_LL17D: _LL17E:
 goto _LL180;}else{goto _LL181;}case 21: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp238)->f3 == 1){_LL17F: _LL180:
# 1312
({void*_tmp239=0;({unsigned int _tmp696=e->loc;struct _dyneither_ptr _tmp695=({const char*_tmp23A="cannot take the address of a @tagged union member";_tag_dyneither(_tmp23A,sizeof(char),50);});Cyc_Tcutil_terr(_tmp696,_tmp695,_tag_dyneither(_tmp239,sizeof(void*),0));});});
goto _LL17C;}else{goto _LL181;}default: _LL181: _LL182:
 goto _LL17C;}_LL17C:;}{
# 1318
struct _tuple14 _tmp23B=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp23C=_tmp23B;int _tmp240;void*_tmp23F;_LL184: _tmp240=_tmp23C.f1;_tmp23F=_tmp23C.f2;_LL185:;
# 1320
if(Cyc_Tcutil_is_noalias_region(_tmp23F,0))
({void*_tmp23D=0;({unsigned int _tmp698=e->loc;struct _dyneither_ptr _tmp697=({const char*_tmp23E="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp23E,sizeof(char),60);});Cyc_Tcutil_terr(_tmp698,_tmp697,_tag_dyneither(_tmp23D,sizeof(void*),0));});});{
# 1323
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp240){
tq.print_const=1;
tq.real_const=1;}{
# 1329
void*t=_tmp229?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp23F,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp23F,tq,Cyc_Absyn_false_conref);
return t;};};};}
# 1336
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1340
return Cyc_Absyn_uint_typ;
# 1342
({unsigned int _tmp69B=loc;struct Cyc_Tcenv_Tenv*_tmp69A=te;struct Cyc_List_List*_tmp699=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp69B,_tmp69A,_tmp699,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp243;_tmp243.tag=0;({struct _dyneither_ptr _tmp69C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp243.f1=_tmp69C;});({void*_tmp241[1]={& _tmp243};({unsigned int _tmp69E=loc;struct _dyneither_ptr _tmp69D=({const char*_tmp242="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp242,sizeof(char),55);});Cyc_Tcutil_terr(_tmp69E,_tmp69D,_tag_dyneither(_tmp241,sizeof(void*),1));});});});
if(topt != 0){
void*_tmp244=Cyc_Tcutil_compress(*topt);void*_tmp245=_tmp244;void*_tmp24B;void*_tmp24A;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp245)->tag == 19){_LL187: _tmp24B=_tmp245;_tmp24A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp245)->f1;_LL188: {
# 1351
struct Cyc_Absyn_Exp*_tmp246=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp247=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp248=_cycalloc(sizeof(*_tmp248));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp69F=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp249;_tmp249.tag=18;_tmp249.f1=_tmp246;_tmp249;});_tmp248[0]=_tmp69F;});_tmp248;});
if(Cyc_Tcutil_unify(_tmp24A,(void*)_tmp247))return _tmp24B;
goto _LL186;}}else{_LL189: _LL18A:
 goto _LL186;}_LL186:;}
# 1357
return Cyc_Absyn_uint_typ;}
# 1360
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(({Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}))return sf->type;else{
return 0;}}
# 1367
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1369
({unsigned int _tmp6A2=loc;struct Cyc_Tcenv_Tenv*_tmp6A1=te;struct Cyc_List_List*_tmp6A0=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6A2,_tmp6A1,_tmp6A0,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp24C=fs;
void*_tmp24D=t_orig;
for(0;_tmp24C != 0;_tmp24C=_tmp24C->tl){
void*_tmp24E=(void*)_tmp24C->hd;
void*_tmp24F=_tmp24E;unsigned int _tmp289;struct _dyneither_ptr*_tmp288;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24F)->tag == 0){_LL18C: _tmp288=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24F)->f1;_LL18D: {
# 1377
int bad_type=1;
{void*_tmp250=Cyc_Tcutil_compress(_tmp24D);void*_tmp251=_tmp250;struct Cyc_List_List*_tmp259;struct Cyc_Absyn_Aggrdecl**_tmp258;switch(*((int*)_tmp251)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).aggr_info).KnownAggr).tag == 2){_LL191: _tmp258=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).aggr_info).KnownAggr).val;_LL192:
# 1380
 if((*_tmp258)->impl == 0)goto _LL190;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp288,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp258)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp254;_tmp254.tag=0;_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp288);({void*_tmp252[1]={& _tmp254};({unsigned int _tmp6A4=loc;struct _dyneither_ptr _tmp6A3=({const char*_tmp253="no field of struct/union has name %s";_tag_dyneither(_tmp253,sizeof(char),37);});Cyc_Tcutil_terr(_tmp6A4,_tmp6A3,_tag_dyneither(_tmp252,sizeof(void*),1));});});});else{
# 1385
_tmp24D=t2;}
bad_type=0;
goto _LL190;};}else{goto _LL195;}case 12: _LL193: _tmp259=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp251)->f2;_LL194: {
# 1389
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp288,_tmp259);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp257;_tmp257.tag=0;_tmp257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp288);({void*_tmp255[1]={& _tmp257};({unsigned int _tmp6A6=loc;struct _dyneither_ptr _tmp6A5=({const char*_tmp256="no field of struct/union has name %s";_tag_dyneither(_tmp256,sizeof(char),37);});Cyc_Tcutil_terr(_tmp6A6,_tmp6A5,_tag_dyneither(_tmp255,sizeof(void*),1));});});});else{
# 1393
_tmp24D=t2;}
bad_type=0;
goto _LL190;}default: _LL195: _LL196:
 goto _LL190;}_LL190:;}
# 1398
if(bad_type){
if(_tmp24C == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp25C;_tmp25C.tag=0;({struct _dyneither_ptr _tmp6A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp25C.f1=_tmp6A7;});({void*_tmp25A[1]={& _tmp25C};({unsigned int _tmp6A9=loc;struct _dyneither_ptr _tmp6A8=({const char*_tmp25B="%s is not a known struct/union type";_tag_dyneither(_tmp25B,sizeof(char),36);});Cyc_Tcutil_terr(_tmp6A9,_tmp6A8,_tag_dyneither(_tmp25A,sizeof(void*),1));});});});else{
# 1402
struct _dyneither_ptr _tmp25D=({struct Cyc_String_pa_PrintArg_struct _tmp270;_tmp270.tag=0;({struct _dyneither_ptr _tmp6AA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp270.f1=_tmp6AA;});({void*_tmp26E[1]={& _tmp270};({struct _dyneither_ptr _tmp6AB=({const char*_tmp26F="(%s)";_tag_dyneither(_tmp26F,sizeof(char),5);});Cyc_aprintf(_tmp6AB,_tag_dyneither(_tmp26E,sizeof(void*),1));});});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp24C;x=x->tl){
void*_tmp25E=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp25F=_tmp25E;unsigned int _tmp269;struct _dyneither_ptr*_tmp268;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->tag == 0){_LL198: _tmp268=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->f1;_LL199:
# 1407
({struct _dyneither_ptr _tmp6AD=({struct Cyc_String_pa_PrintArg_struct _tmp263;_tmp263.tag=0;_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp268);({struct Cyc_String_pa_PrintArg_struct _tmp262;_tmp262.tag=0;_tmp262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp260[2]={& _tmp262,& _tmp263};({struct _dyneither_ptr _tmp6AC=({const char*_tmp261="%s.%s";_tag_dyneither(_tmp261,sizeof(char),6);});Cyc_aprintf(_tmp6AC,_tag_dyneither(_tmp260,sizeof(void*),2));});});});});_tmp25D=_tmp6AD;});goto _LL197;}else{_LL19A: _tmp269=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp25F)->f1;_LL19B:
# 1409
({struct _dyneither_ptr _tmp6AF=({struct Cyc_Int_pa_PrintArg_struct _tmp267;_tmp267.tag=1;_tmp267.f1=(unsigned long)((int)_tmp269);({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp264[2]={& _tmp266,& _tmp267};({struct _dyneither_ptr _tmp6AE=({const char*_tmp265="%s.%d";_tag_dyneither(_tmp265,sizeof(char),6);});Cyc_aprintf(_tmp6AE,_tag_dyneither(_tmp264,sizeof(void*),2));});});});});_tmp25D=_tmp6AF;});goto _LL197;}_LL197:;}
# 1411
({struct Cyc_String_pa_PrintArg_struct _tmp26D;_tmp26D.tag=0;({struct _dyneither_ptr _tmp6B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp26D.f1=_tmp6B0;});({struct Cyc_String_pa_PrintArg_struct _tmp26C;_tmp26C.tag=0;_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp26A[2]={& _tmp26C,& _tmp26D};({unsigned int _tmp6B2=loc;struct _dyneither_ptr _tmp6B1=({const char*_tmp26B="%s == %s is not a struct/union type";_tag_dyneither(_tmp26B,sizeof(char),36);});Cyc_Tcutil_terr(_tmp6B2,_tmp6B1,_tag_dyneither(_tmp26A,sizeof(void*),2));});});});});}}
# 1414
goto _LL18B;}}else{_LL18E: _tmp289=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp24F)->f1;_LL18F: {
# 1416
int bad_type=1;
{void*_tmp271=Cyc_Tcutil_compress(_tmp24D);void*_tmp272=_tmp271;struct Cyc_Absyn_Datatypefield*_tmp284;struct Cyc_List_List*_tmp283;struct Cyc_List_List*_tmp282;struct Cyc_Absyn_Aggrdecl**_tmp281;switch(*((int*)_tmp272)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp272)->f1).aggr_info).KnownAggr).tag == 2){_LL19D: _tmp281=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp272)->f1).aggr_info).KnownAggr).val;_LL19E:
# 1419
 if((*_tmp281)->impl == 0)
goto _LL19C;
_tmp282=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp281)->impl))->fields;goto _LL1A0;}else{goto _LL1A5;}case 12: _LL19F: _tmp282=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp272)->f2;_LL1A0:
# 1423
 if(({int _tmp6B3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp282);_tmp6B3 <= _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp276;_tmp276.tag=1;_tmp276.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp275;_tmp275.tag=1;({unsigned long _tmp6B4=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp282);_tmp275.f1=_tmp6B4;});({void*_tmp273[2]={& _tmp275,& _tmp276};({unsigned int _tmp6B6=loc;struct _dyneither_ptr _tmp6B5=({const char*_tmp274="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp274,sizeof(char),46);});Cyc_Tcutil_terr(_tmp6B6,_tmp6B5,_tag_dyneither(_tmp273,sizeof(void*),2));});});});});else{
# 1427
({void*_tmp6B7=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp282,(int)_tmp289))->type;_tmp24D=_tmp6B7;});}
bad_type=0;
goto _LL19C;case 10: _LL1A1: _tmp283=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp272)->f1;_LL1A2:
# 1431
 if(({int _tmp6B8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp283);_tmp6B8 <= _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp27A;_tmp27A.tag=1;_tmp27A.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp279;_tmp279.tag=1;({unsigned long _tmp6B9=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp283);_tmp279.f1=_tmp6B9;});({void*_tmp277[2]={& _tmp279,& _tmp27A};({unsigned int _tmp6BB=loc;struct _dyneither_ptr _tmp6BA=({const char*_tmp278="tuple has too few components: %d <= %d";_tag_dyneither(_tmp278,sizeof(char),39);});Cyc_Tcutil_terr(_tmp6BB,_tmp6BA,_tag_dyneither(_tmp277,sizeof(void*),2));});});});});else{
# 1435
({void*_tmp6BC=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp283,(int)_tmp289)).f2;_tmp24D=_tmp6BC;});}
bad_type=0;
goto _LL19C;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp272)->f1).field_info).KnownDatatypefield).tag == 2){_LL1A3: _tmp284=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp272)->f1).field_info).KnownDatatypefield).val).f2;_LL1A4:
# 1439
 if(({int _tmp6BD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp284->typs);_tmp6BD < _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp27E;_tmp27E.tag=1;_tmp27E.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp27D;_tmp27D.tag=1;({unsigned long _tmp6BE=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp284->typs);_tmp27D.f1=_tmp6BE;});({void*_tmp27B[2]={& _tmp27D,& _tmp27E};({unsigned int _tmp6C0=loc;struct _dyneither_ptr _tmp6BF=({const char*_tmp27C="datatype field has too few components: %d < %d";_tag_dyneither(_tmp27C,sizeof(char),47);});Cyc_Tcutil_terr(_tmp6C0,_tmp6BF,_tag_dyneither(_tmp27B,sizeof(void*),2));});});});});else{
# 1443
if(_tmp289 != 0)
({void*_tmp6C1=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp284->typs,(int)(_tmp289 - 1))).f2;_tmp24D=_tmp6C1;});else{
if(_tmp24C->tl != 0)
({void*_tmp27F=0;({unsigned int _tmp6C3=loc;struct _dyneither_ptr _tmp6C2=({const char*_tmp280="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp280,sizeof(char),68);});Cyc_Tcutil_terr(_tmp6C3,_tmp6C2,_tag_dyneither(_tmp27F,sizeof(void*),0));});});}}
# 1448
bad_type=0;
goto _LL19C;}else{goto _LL1A5;}default: _LL1A5: _LL1A6:
 goto _LL19C;}_LL19C:;}
# 1452
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp287;_tmp287.tag=0;({struct _dyneither_ptr _tmp6C4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp287.f1=_tmp6C4;});({void*_tmp285[1]={& _tmp287};({unsigned int _tmp6C6=loc;struct _dyneither_ptr _tmp6C5=({const char*_tmp286="%s is not a known type";_tag_dyneither(_tmp286,sizeof(char),23);});Cyc_Tcutil_terr(_tmp6C6,_tmp6C5,_tag_dyneither(_tmp285,sizeof(void*),1));});});});
goto _LL18B;}}_LL18B:;}
# 1457
return Cyc_Absyn_uint_typ;};}
# 1461
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle*_tmp28A=Cyc_Core_heap_region;
struct Cyc_Tcenv_Tenv*_tmp28B=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp28B,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp28C=t;void*_tmp2A2;void*_tmp2A1;union Cyc_Absyn_Constraint*_tmp2A0;union Cyc_Absyn_Constraint*_tmp29F;switch(*((int*)_tmp28C)){case 1: _LL1A8: _LL1A9: {
# 1468
struct Cyc_List_List*_tmp28D=Cyc_Tcenv_lookup_type_vars(_tmp28B);
void*_tmp28E=({Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299->v=_tmp28D;_tmp299;}));});
void*_tmp28F=({Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->v=_tmp28D;_tmp298;}));});
union Cyc_Absyn_Constraint*_tmp290=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp291=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp292=({struct Cyc_Absyn_PtrAtts _tmp297;_tmp297.rgn=_tmp28F;({union Cyc_Absyn_Constraint*_tmp6C7=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp297.nullable=_tmp6C7;});_tmp297.bounds=_tmp290;_tmp297.zero_term=_tmp291;_tmp297.ptrloc=0;_tmp297;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp293=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp294=_cycalloc(sizeof(*_tmp294));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6CA=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp295;_tmp295.tag=5;({struct Cyc_Absyn_PtrInfo _tmp6C9=({struct Cyc_Absyn_PtrInfo _tmp296;_tmp296.elt_typ=_tmp28E;({struct Cyc_Absyn_Tqual _tmp6C8=Cyc_Absyn_empty_tqual(0);_tmp296.elt_tq=_tmp6C8;});_tmp296.ptr_atts=_tmp292;_tmp296;});_tmp295.f1=_tmp6C9;});_tmp295;});_tmp294[0]=_tmp6CA;});_tmp294;});
Cyc_Tcutil_unify(t,(void*)_tmp293);
_tmp2A2=_tmp28E;_tmp2A1=_tmp28F;_tmp2A0=_tmp290;_tmp29F=_tmp291;goto _LL1AB;}case 5: _LL1AA: _tmp2A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).elt_typ;_tmp2A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).rgn;_tmp2A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).bounds;_tmp29F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28C)->f1).ptr_atts).zero_term;_LL1AB:
# 1478
 Cyc_Tcenv_check_rgn_accessible(_tmp28B,loc,_tmp2A1);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2A0);
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2A2),& Cyc_Tcutil_tmk);}) && !Cyc_Tcenv_abstract_val_ok(_tmp28B))
({void*_tmp29A=0;({unsigned int _tmp6CC=loc;struct _dyneither_ptr _tmp6CB=({const char*_tmp29B="can't dereference abstract pointer type";_tag_dyneither(_tmp29B,sizeof(char),40);});Cyc_Tcutil_terr(_tmp6CC,_tmp6CB,_tag_dyneither(_tmp29A,sizeof(void*),0));});});
return _tmp2A2;default: _LL1AC: _LL1AD:
# 1484
 return({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0;({struct _dyneither_ptr _tmp6CD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp29E.f1=_tmp6CD;});({void*_tmp29C[1]={& _tmp29E};({struct Cyc_Tcenv_Tenv*_tmp6D1=_tmp28B;unsigned int _tmp6D0=loc;void**_tmp6CF=topt;struct _dyneither_ptr _tmp6CE=({const char*_tmp29D="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp29D,sizeof(char),39);});Cyc_Tcexp_expr_err(_tmp6D1,_tmp6D0,_tmp6CF,_tmp6CE,_tag_dyneither(_tmp29C,sizeof(void*),1));});});});}_LL1A7:;};}
# 1490
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1494
({struct Cyc_Tcenv_Tenv*_tmp6D2=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6D2,0,e);});
# 1496
({int _tmp6D3=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6D3;});{
void*_tmp2A3=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A4=_tmp2A3;enum Cyc_Absyn_AggrKind _tmp2BF;struct Cyc_List_List*_tmp2BE;struct Cyc_Absyn_Aggrdecl*_tmp2BD;struct Cyc_List_List*_tmp2BC;switch(*((int*)_tmp2A4)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A4)->f1).aggr_info).KnownAggr).tag == 2){_LL1AF: _tmp2BD=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A4)->f1).aggr_info).KnownAggr).val;_tmp2BC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A4)->f1).targs;_LL1B0: {
# 1499
struct Cyc_Absyn_Aggrfield*_tmp2A5=Cyc_Absyn_lookup_decl_field(_tmp2BD,f);
if(_tmp2A5 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0;({struct _dyneither_ptr _tmp6D4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2BD->name));_tmp2A8.f1=_tmp6D4;});({void*_tmp2A6[2]={& _tmp2A8,& _tmp2A9};({struct Cyc_Tcenv_Tenv*_tmp6D8=te;unsigned int _tmp6D7=loc;void**_tmp6D6=topt;struct _dyneither_ptr _tmp6D5=({const char*_tmp2A7="%s has no %s member";_tag_dyneither(_tmp2A7,sizeof(char),20);});Cyc_Tcexp_expr_err(_tmp6D8,_tmp6D7,_tmp6D6,_tmp6D5,_tag_dyneither(_tmp2A6,sizeof(void*),2));});});});});
# 1504
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BD->impl))->tagged)*is_tagged=1;{
void*t2=_tmp2A5->type;
if(_tmp2BC != 0){
struct _RegionHandle*_tmp2AA=Cyc_Core_heap_region;
struct Cyc_List_List*_tmp2AB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2AA,_tmp2AA,_tmp2BD->tvs,_tmp2BC);
({void*_tmp6D9=Cyc_Tcutil_rsubstitute(_tmp2AA,_tmp2AB,_tmp2A5->type);t2=_tmp6D9;});}
# 1514
if((((_tmp2BD->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BD->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2A5->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0;_tmp2AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2AC[1]={& _tmp2AE};({struct Cyc_Tcenv_Tenv*_tmp6DD=te;unsigned int _tmp6DC=loc;void**_tmp6DB=topt;struct _dyneither_ptr _tmp6DA=({const char*_tmp2AD="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2AD,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6DD,_tmp6DC,_tmp6DB,_tmp6DA,_tag_dyneither(_tmp2AC,sizeof(void*),1));});});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BD->impl))->exist_vars != 0){
# 1519
if(!({void*_tmp6DE=t2;Cyc_Tcutil_unify(_tmp6DE,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=0;_tmp2B1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2AF[1]={& _tmp2B1};({struct Cyc_Tcenv_Tenv*_tmp6E2=te;unsigned int _tmp6E1=loc;void**_tmp6E0=topt;struct _dyneither_ptr _tmp6DF=({const char*_tmp2B0="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp2B0,sizeof(char),81);});Cyc_Tcexp_expr_err(_tmp6E2,_tmp6E1,_tmp6E0,_tmp6DF,_tag_dyneither(_tmp2AF,sizeof(void*),1));});});});}
# 1522
return t2;};}}else{goto _LL1B3;}case 12: _LL1B1: _tmp2BF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A4)->f1;_tmp2BE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A4)->f2;_LL1B2: {
# 1524
struct Cyc_Absyn_Aggrfield*_tmp2B2=Cyc_Absyn_lookup_field(_tmp2BE,f);
if(_tmp2B2 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2B5;_tmp2B5.tag=0;_tmp2B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2B3[1]={& _tmp2B5};({struct Cyc_Tcenv_Tenv*_tmp6E6=te;unsigned int _tmp6E5=loc;void**_tmp6E4=topt;struct _dyneither_ptr _tmp6E3=({const char*_tmp2B4="type has no %s member";_tag_dyneither(_tmp2B4,sizeof(char),22);});Cyc_Tcexp_expr_err(_tmp6E6,_tmp6E5,_tmp6E4,_tmp6E3,_tag_dyneither(_tmp2B3,sizeof(void*),1));});});});
# 1529
if(((_tmp2BF == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2B2->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2B2->requires_clause == 0)
# 1531
return({struct Cyc_String_pa_PrintArg_struct _tmp2B8;_tmp2B8.tag=0;_tmp2B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2B6[1]={& _tmp2B8};({struct Cyc_Tcenv_Tenv*_tmp6EA=te;unsigned int _tmp6E9=loc;void**_tmp6E8=topt;struct _dyneither_ptr _tmp6E7=({const char*_tmp2B7="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2B7,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6EA,_tmp6E9,_tmp6E8,_tmp6E7,_tag_dyneither(_tmp2B6,sizeof(void*),1));});});});
return _tmp2B2->type;}default: _LL1B3: _LL1B4:
# 1534
 return({struct Cyc_String_pa_PrintArg_struct _tmp2BB;_tmp2BB.tag=0;({struct _dyneither_ptr _tmp6EB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp2BB.f1=_tmp6EB;});({void*_tmp2B9[1]={& _tmp2BB};({struct Cyc_Tcenv_Tenv*_tmp6EF=te;unsigned int _tmp6EE=loc;void**_tmp6ED=topt;struct _dyneither_ptr _tmp6EC=({const char*_tmp2BA="expecting struct or union, found %s";_tag_dyneither(_tmp2BA,sizeof(char),36);});Cyc_Tcexp_expr_err(_tmp6EF,_tmp6EE,_tmp6ED,_tmp6EC,_tag_dyneither(_tmp2B9,sizeof(void*),1));});});});}_LL1AE:;};}
# 1540
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1543
({struct Cyc_Tcenv_Tenv*_tmp6F0=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp6F0,0,e);});
# 1545
({int _tmp6F1=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6F1;});
{void*_tmp2C0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2C1=_tmp2C0;void*_tmp2E5;void*_tmp2E4;union Cyc_Absyn_Constraint*_tmp2E3;union Cyc_Absyn_Constraint*_tmp2E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1)->tag == 5){_LL1B6: _tmp2E5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1)->f1).elt_typ;_tmp2E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1)->f1).ptr_atts).rgn;_tmp2E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1)->f1).ptr_atts).bounds;_tmp2E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1)->f1).ptr_atts).zero_term;_LL1B7:
# 1548
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2E3);
{void*_tmp2C2=Cyc_Tcutil_compress(_tmp2E5);void*_tmp2C3=_tmp2C2;enum Cyc_Absyn_AggrKind _tmp2E1;struct Cyc_List_List*_tmp2E0;struct Cyc_Absyn_Aggrdecl*_tmp2DF;struct Cyc_List_List*_tmp2DE;switch(*((int*)_tmp2C3)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C3)->f1).aggr_info).KnownAggr).tag == 2){_LL1BB: _tmp2DF=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C3)->f1).aggr_info).KnownAggr).val;_tmp2DE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C3)->f1).targs;_LL1BC: {
# 1551
struct Cyc_Absyn_Aggrfield*_tmp2C4=Cyc_Absyn_lookup_decl_field(_tmp2DF,f);
if(_tmp2C4 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2C8;_tmp2C8.tag=0;_tmp2C8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2C7;_tmp2C7.tag=0;({struct _dyneither_ptr _tmp6F2=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2DF->name));_tmp2C7.f1=_tmp6F2;});({void*_tmp2C5[2]={& _tmp2C7,& _tmp2C8};({struct Cyc_Tcenv_Tenv*_tmp6F6=te;unsigned int _tmp6F5=loc;void**_tmp6F4=topt;struct _dyneither_ptr _tmp6F3=({const char*_tmp2C6="type %s has no %s member";_tag_dyneither(_tmp2C6,sizeof(char),25);});Cyc_Tcexp_expr_err(_tmp6F6,_tmp6F5,_tmp6F4,_tmp6F3,_tag_dyneither(_tmp2C5,sizeof(void*),2));});});});});
# 1556
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DF->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2C4->type;
if(_tmp2DE != 0){
struct _RegionHandle*_tmp2C9=Cyc_Core_heap_region;
struct Cyc_List_List*_tmp2CA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(_tmp2C9,_tmp2C9,_tmp2DF->tvs,_tmp2DE);
({void*_tmp6F7=Cyc_Tcutil_rsubstitute(_tmp2C9,_tmp2CA,_tmp2C4->type);t3=_tmp6F7;});}{
# 1564
struct Cyc_Absyn_Kind*_tmp2CB=Cyc_Tcutil_typ_kind(t3);
# 1567
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2CB) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2CC=Cyc_Tcutil_compress(t3);void*_tmp2CD=_tmp2CC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CD)->tag == 8){_LL1C2: _LL1C3:
 goto _LL1C1;}else{_LL1C4: _LL1C5:
# 1571
 return({struct Cyc_String_pa_PrintArg_struct _tmp2D0;_tmp2D0.tag=0;_tmp2D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CE[1]={& _tmp2D0};({struct Cyc_Tcenv_Tenv*_tmp6FB=te;unsigned int _tmp6FA=loc;void**_tmp6F9=topt;struct _dyneither_ptr _tmp6F8=({const char*_tmp2CF="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2CF,sizeof(char),48);});Cyc_Tcexp_expr_err(_tmp6FB,_tmp6FA,_tmp6F9,_tmp6F8,_tag_dyneither(_tmp2CE,sizeof(void*),1));});});});}_LL1C1:;}
# 1576
if((((_tmp2DF->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DF->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2C4->requires_clause == 0)
# 1579
return({struct Cyc_String_pa_PrintArg_struct _tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D1[1]={& _tmp2D3};({struct Cyc_Tcenv_Tenv*_tmp6FF=te;unsigned int _tmp6FE=loc;void**_tmp6FD=topt;struct _dyneither_ptr _tmp6FC=({const char*_tmp2D2="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2D2,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6FF,_tmp6FE,_tmp6FD,_tmp6FC,_tag_dyneither(_tmp2D1,sizeof(void*),1));});});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DF->impl))->exist_vars != 0){
if(!({void*_tmp700=t3;Cyc_Tcutil_unify(_tmp700,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2D6;_tmp2D6.tag=0;_tmp2D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D4[1]={& _tmp2D6};({struct Cyc_Tcenv_Tenv*_tmp704=te;unsigned int _tmp703=loc;void**_tmp702=topt;struct _dyneither_ptr _tmp701=({const char*_tmp2D5="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2D5,sizeof(char),72);});Cyc_Tcexp_expr_err(_tmp704,_tmp703,_tmp702,_tmp701,_tag_dyneither(_tmp2D4,sizeof(void*),1));});});});}
# 1586
return t3;};};}}else{goto _LL1BF;}case 12: _LL1BD: _tmp2E1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C3)->f1;_tmp2E0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C3)->f2;_LL1BE: {
# 1588
struct Cyc_Absyn_Aggrfield*_tmp2D7=Cyc_Absyn_lookup_field(_tmp2E0,f);
if(_tmp2D7 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2DA;_tmp2DA.tag=0;_tmp2DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D8[1]={& _tmp2DA};({struct Cyc_Tcenv_Tenv*_tmp708=te;unsigned int _tmp707=loc;void**_tmp706=topt;struct _dyneither_ptr _tmp705=({const char*_tmp2D9="type has no %s field";_tag_dyneither(_tmp2D9,sizeof(char),21);});Cyc_Tcexp_expr_err(_tmp708,_tmp707,_tmp706,_tmp705,_tag_dyneither(_tmp2D8,sizeof(void*),1));});});});
# 1593
if((_tmp2E1 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2D7->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2DD;_tmp2DD.tag=0;_tmp2DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2DB[1]={& _tmp2DD};({struct Cyc_Tcenv_Tenv*_tmp70C=te;unsigned int _tmp70B=loc;void**_tmp70A=topt;struct _dyneither_ptr _tmp709=({const char*_tmp2DC="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2DC,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp70C,_tmp70B,_tmp70A,_tmp709,_tag_dyneither(_tmp2DB,sizeof(void*),1));});});});
return _tmp2D7->type;}default: _LL1BF: _LL1C0:
 goto _LL1BA;}_LL1BA:;}
# 1598
goto _LL1B5;}else{_LL1B8: _LL1B9:
 goto _LL1B5;}_LL1B5:;}
# 1601
return({struct Cyc_String_pa_PrintArg_struct _tmp2E8;_tmp2E8.tag=0;({struct _dyneither_ptr _tmp70D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp2E8.f1=_tmp70D;});({void*_tmp2E6[1]={& _tmp2E8};({struct Cyc_Tcenv_Tenv*_tmp711=te;unsigned int _tmp710=loc;void**_tmp70F=topt;struct _dyneither_ptr _tmp70E=({const char*_tmp2E7="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2E7,sizeof(char),44);});Cyc_Tcexp_expr_err(_tmp711,_tmp710,_tmp70F,_tmp70E,_tag_dyneither(_tmp2E6,sizeof(void*),1));});});});}
# 1606
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1608
struct _tuple15 _tmp2E9=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2EA=_tmp2E9;unsigned int _tmp2F8;int _tmp2F7;_LL1C7: _tmp2F8=_tmp2EA.f1;_tmp2F7=_tmp2EA.f2;_LL1C8:;
if(!_tmp2F7)
return({void*_tmp2EB=0;({struct Cyc_Tcenv_Tenv*_tmp714=te;unsigned int _tmp713=loc;struct _dyneither_ptr _tmp712=({const char*_tmp2EC="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2EC,sizeof(char),47);});Cyc_Tcexp_expr_err(_tmp714,_tmp713,0,_tmp712,_tag_dyneither(_tmp2EB,sizeof(void*),0));});});{
# 1612
struct _handler_cons _tmp2ED;_push_handler(& _tmp2ED);{int _tmp2EF=0;if(setjmp(_tmp2ED.handler))_tmp2EF=1;if(!_tmp2EF){
{void*_tmp2F0=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2F8)).f2;_npop_handler(0);return _tmp2F0;};_pop_handler();}else{void*_tmp2EE=(void*)_exn_thrown;void*_tmp2F1=_tmp2EE;void*_tmp2F6;if(((struct Cyc_List_Nth_exn_struct*)_tmp2F1)->tag == Cyc_List_Nth){_LL1CA: _LL1CB:
# 1615
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=1;({unsigned long _tmp715=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp2F5.f1=_tmp715;});({struct Cyc_Int_pa_PrintArg_struct _tmp2F4;_tmp2F4.tag=1;_tmp2F4.f1=(unsigned long)((int)_tmp2F8);({void*_tmp2F2[2]={& _tmp2F4,& _tmp2F5};({struct Cyc_Tcenv_Tenv*_tmp718=te;unsigned int _tmp717=loc;struct _dyneither_ptr _tmp716=({const char*_tmp2F3="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2F3,sizeof(char),41);});Cyc_Tcexp_expr_err(_tmp718,_tmp717,0,_tmp716,_tag_dyneither(_tmp2F2,sizeof(void*),2));});});});});}else{_LL1CC: _tmp2F6=_tmp2F1;_LL1CD:(int)_rethrow(_tmp2F6);}_LL1C9:;}};};}
# 1620
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1622
struct _RegionHandle*_tmp2F9=Cyc_Core_heap_region;
struct Cyc_Tcenv_Tenv*_tmp2FA=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp719=Cyc_Tcenv_clear_abstract_val_ok(_tmp2FA);Cyc_Tcexp_tcExp(_tmp719,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp71A=Cyc_Tcenv_clear_abstract_val_ok(_tmp2FA);Cyc_Tcexp_tcExp(_tmp71A,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2FA,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2FD;_tmp2FD.tag=0;({struct _dyneither_ptr _tmp71B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp2FD.f1=_tmp71B;});({void*_tmp2FB[1]={& _tmp2FD};({struct Cyc_Tcenv_Tenv*_tmp71F=_tmp2FA;unsigned int _tmp71E=e2->loc;void**_tmp71D=topt;struct _dyneither_ptr _tmp71C=({const char*_tmp2FC="expecting int subscript, found %s";_tag_dyneither(_tmp2FC,sizeof(char),34);});Cyc_Tcexp_expr_err(_tmp71F,_tmp71E,_tmp71D,_tmp71C,_tag_dyneither(_tmp2FB,sizeof(void*),1));});});});{
# 1633
void*_tmp2FE=t1;struct Cyc_List_List*_tmp31C;void*_tmp31B;struct Cyc_Absyn_Tqual _tmp31A;void*_tmp319;union Cyc_Absyn_Constraint*_tmp318;union Cyc_Absyn_Constraint*_tmp317;switch(*((int*)_tmp2FE)){case 5: _LL1CF: _tmp31B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).elt_typ;_tmp31A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).elt_tq;_tmp319=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).rgn;_tmp318=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).bounds;_tmp317=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).zero_term;_LL1D0:
# 1637
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp317)){
int emit_warning=1;
{void*_tmp2FF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp318);void*_tmp300=_tmp2FF;struct Cyc_Absyn_Exp*_tmp309;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp300)->tag == 1){_LL1D6: _tmp309=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp300)->f1;_LL1D7:
# 1641
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp301=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp302=_tmp301;unsigned int _tmp308;int _tmp307;_LL1DB: _tmp308=_tmp302.f1;_tmp307=_tmp302.f2;_LL1DC:;
if(_tmp307){
struct _tuple15 _tmp303=Cyc_Evexp_eval_const_uint_exp(_tmp309);struct _tuple15 _tmp304=_tmp303;unsigned int _tmp306;int _tmp305;_LL1DE: _tmp306=_tmp304.f1;_tmp305=_tmp304.f2;_LL1DF:;
if(_tmp305  && _tmp306 > _tmp308)emit_warning=0;}}
# 1648
goto _LL1D5;}else{_LL1D8: _LL1D9:
 emit_warning=0;goto _LL1D5;}_LL1D5:;}
# 1651
if(emit_warning)
({void*_tmp30A=0;({unsigned int _tmp721=e2->loc;struct _dyneither_ptr _tmp720=({const char*_tmp30B="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp30B,sizeof(char),63);});Cyc_Tcutil_warn(_tmp721,_tmp720,_tag_dyneither(_tmp30A,sizeof(void*),0));});});}else{
# 1655
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp30C=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp30D=_tmp30C;unsigned int _tmp30F;int _tmp30E;_LL1E1: _tmp30F=_tmp30D.f1;_tmp30E=_tmp30D.f2;_LL1E2:;
if(_tmp30E)
Cyc_Tcutil_check_bound(loc,_tmp30F,_tmp318);}else{
# 1661
if(Cyc_Tcutil_is_bound_one(_tmp318) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp317))
({void*_tmp310=0;({unsigned int _tmp723=e1->loc;struct _dyneither_ptr _tmp722=({const char*_tmp311="subscript applied to pointer to one object";_tag_dyneither(_tmp311,sizeof(char),43);});Cyc_Tcutil_warn(_tmp723,_tmp722,_tag_dyneither(_tmp310,sizeof(void*),0));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp318);}}
# 1666
Cyc_Tcenv_check_rgn_accessible(_tmp2FA,loc,_tmp319);
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp31B),& Cyc_Tcutil_tmk);}) && !Cyc_Tcenv_abstract_val_ok(_tmp2FA))
({void*_tmp312=0;({unsigned int _tmp725=e1->loc;struct _dyneither_ptr _tmp724=({const char*_tmp313="can't subscript an abstract pointer";_tag_dyneither(_tmp313,sizeof(char),36);});Cyc_Tcutil_terr(_tmp725,_tmp724,_tag_dyneither(_tmp312,sizeof(void*),0));});});
return _tmp31B;case 10: _LL1D1: _tmp31C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FE)->f1;_LL1D2:
 return Cyc_Tcexp_ithTupleType(_tmp2FA,loc,_tmp31C,e2);default: _LL1D3: _LL1D4:
 return({struct Cyc_String_pa_PrintArg_struct _tmp316;_tmp316.tag=0;({struct _dyneither_ptr _tmp726=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp316.f1=_tmp726;});({void*_tmp314[1]={& _tmp316};({struct Cyc_Tcenv_Tenv*_tmp72A=_tmp2FA;unsigned int _tmp729=loc;void**_tmp728=topt;struct _dyneither_ptr _tmp727=({const char*_tmp315="subscript applied to %s";_tag_dyneither(_tmp315,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp72A,_tmp729,_tmp728,_tmp727,_tag_dyneither(_tmp314,sizeof(void*),1));});});});}_LL1CE:;};};}
# 1677
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp31D=Cyc_Tcutil_compress(*topt);void*_tmp31E=_tmp31D;struct Cyc_List_List*_tmp322;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp31E)->tag == 10){_LL1E4: _tmp322=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp31E)->f1;_LL1E5:
# 1683
 if(({int _tmp72B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp322);_tmp72B != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1686
goto _LL1E3;
# 1688
for(0;es != 0;(es=es->tl,_tmp322=_tmp322->tl)){
int bogus=0;
void*_tmp31F=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp322))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp72C=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp72C,& _tmp31F,(struct Cyc_Absyn_Exp*)es->hd);});
# 1693
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp322->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp72E=({struct Cyc_List_List*_tmp320=_cycalloc(sizeof(*_tmp320));({struct _tuple17*_tmp72D=({struct _tuple17*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->f1=(*((struct _tuple17*)_tmp322->hd)).f1;_tmp321->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp321;});_tmp320->hd=_tmp72D;});_tmp320->tl=fields;_tmp320;});fields=_tmp72E;});}
# 1696
done=1;
goto _LL1E3;}else{_LL1E6: _LL1E7:
 goto _LL1E3;}_LL1E3:;}
# 1700
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp72F=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp72F,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp732=({struct Cyc_List_List*_tmp323=_cycalloc(sizeof(*_tmp323));({struct _tuple17*_tmp731=({struct _tuple17*_tmp324=_cycalloc(sizeof(*_tmp324));({struct Cyc_Absyn_Tqual _tmp730=Cyc_Absyn_empty_tqual(0);_tmp324->f1=_tmp730;});_tmp324->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp324;});_tmp323->hd=_tmp731;});_tmp323->tl=fields;_tmp323;});fields=_tmp732;});}
# 1705
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp325=_cycalloc(sizeof(*_tmp325));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp734=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp326;_tmp326.tag=10;({struct Cyc_List_List*_tmp733=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp326.f1=_tmp733;});_tmp326;});_tmp325[0]=_tmp734;});_tmp325;});}
# 1709
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1712
return({void*_tmp327=0;({struct Cyc_Tcenv_Tenv*_tmp738=te;unsigned int _tmp737=loc;void**_tmp736=topt;struct _dyneither_ptr _tmp735=({const char*_tmp328="tcCompoundLit";_tag_dyneither(_tmp328,sizeof(char),14);});Cyc_Tcexp_expr_err(_tmp738,_tmp737,_tmp736,_tmp735,_tag_dyneither(_tmp327,sizeof(void*),0));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1722 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1725
void*res_t2;
struct _RegionHandle*_tmp329=Cyc_Core_heap_region;{
int _tmp32A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp329,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=({Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));});
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp32B=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp332=_cycalloc(sizeof(*_tmp332));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp73A=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp333;_tmp333.tag=0;({union Cyc_Absyn_Cnst _tmp739=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp32A);_tmp333.f1=_tmp739;});_tmp333;});_tmp332[0]=_tmp73A;});_tmp332;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp32B,loc);
# 1734
if(zero_term){
struct Cyc_Absyn_Exp*_tmp32C=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp32A - 1);
if(!Cyc_Tcutil_is_zero(_tmp32C))
({void*_tmp32D=0;({unsigned int _tmp73C=_tmp32C->loc;struct _dyneither_ptr _tmp73B=({const char*_tmp32E="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp32E,sizeof(char),45);});Cyc_Tcutil_terr(_tmp73C,_tmp73B,_tag_dyneither(_tmp32D,sizeof(void*),0));});});}
# 1739
sz_exp->topt=Cyc_Absyn_uint_typ;
({void*_tmp740=({void*_tmp73F=res;struct Cyc_Absyn_Tqual _tmp73E=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0);
# 1740
struct Cyc_Absyn_Exp*_tmp73D=sz_exp;Cyc_Absyn_array_typ(_tmp73F,_tmp73E,_tmp73D,
# 1742
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);});
# 1740
res_t2=_tmp740;});
# 1744
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1747
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1749
({struct Cyc_String_pa_PrintArg_struct _tmp331;_tmp331.tag=0;({struct _dyneither_ptr _tmp741=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res));_tmp331.f1=_tmp741;});({void*_tmp32F[1]={& _tmp331};({unsigned int _tmp743=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp742=({const char*_tmp330="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp330,sizeof(char),53);});Cyc_Tcutil_terr(_tmp743,_tmp742,_tag_dyneither(_tmp32F,sizeof(void*),1));});});});}
# 1753
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1758
void*_tmp334=(void*)ds->hd;void*_tmp335=_tmp334;struct Cyc_Absyn_Exp*_tmp342;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp335)->tag == 1){_LL1E9: _LL1EA:
# 1760
({void*_tmp336=0;({unsigned int _tmp745=loc;struct _dyneither_ptr _tmp744=({const char*_tmp337="only array index designators are supported";_tag_dyneither(_tmp337,sizeof(char),43);});Cyc_Tcutil_terr(_tmp745,_tmp744,_tag_dyneither(_tmp336,sizeof(void*),0));});});
goto _LL1E8;}else{_LL1EB: _tmp342=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp335)->f1;_LL1EC:
# 1763
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp342);{
struct _tuple15 _tmp338=Cyc_Evexp_eval_const_uint_exp(_tmp342);struct _tuple15 _tmp339=_tmp338;unsigned int _tmp341;int _tmp340;_LL1EE: _tmp341=_tmp339.f1;_tmp340=_tmp339.f2;_LL1EF:;
if(!_tmp340)
({void*_tmp33A=0;({unsigned int _tmp747=_tmp342->loc;struct _dyneither_ptr _tmp746=({const char*_tmp33B="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp33B,sizeof(char),47);});Cyc_Tcutil_terr(_tmp747,_tmp746,_tag_dyneither(_tmp33A,sizeof(void*),0));});});else{
if(_tmp341 != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp33F;_tmp33F.tag=1;_tmp33F.f1=(unsigned long)((int)_tmp341);({struct Cyc_Int_pa_PrintArg_struct _tmp33E;_tmp33E.tag=1;_tmp33E.f1=(unsigned long)offset;({void*_tmp33C[2]={& _tmp33E,& _tmp33F};({unsigned int _tmp749=_tmp342->loc;struct _dyneither_ptr _tmp748=({const char*_tmp33D="expecting index designator %d but found %d";_tag_dyneither(_tmp33D,sizeof(char),43);});Cyc_Tcutil_terr(_tmp749,_tmp748,_tag_dyneither(_tmp33C,sizeof(void*),2));});});});});}
# 1770
goto _LL1E8;};}_LL1E8:;}}}
# 1774
return res_t2;}
# 1778
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1781
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp343=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp344=_tmp343;void*_tmp349;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp344)->tag == 19){_LL1F1: _tmp349=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp344)->f1;_LL1F2:
# 1786
 if(({Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr;})){
struct Cyc_Absyn_Exp*_tmp345=({void*_tmp74A=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp74A,Cyc_Absyn_valueof_exp(_tmp349,0),0,Cyc_Absyn_No_coercion,0);});
_tmp345->topt=bound->topt;
# 1791
bound=_tmp345;}
# 1793
goto _LL1F0;}else{_LL1F3: _LL1F4:
# 1795
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp348;_tmp348.tag=0;({struct _dyneither_ptr _tmp74B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp348.f1=_tmp74B;});({void*_tmp346[1]={& _tmp348};({unsigned int _tmp74D=bound->loc;struct _dyneither_ptr _tmp74C=({const char*_tmp347="expecting unsigned int, found %s";_tag_dyneither(_tmp347,sizeof(char),33);});Cyc_Tcutil_terr(_tmp74D,_tmp74C,_tag_dyneither(_tmp346,sizeof(void*),1));});});});}_LL1F0:;}
# 1800
if(!(vd->tq).real_const)
({void*_tmp34A=0;({struct _dyneither_ptr _tmp74E=({const char*_tmp34B="comprehension index variable is not declared const!";_tag_dyneither(_tmp34B,sizeof(char),52);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp74E,_tag_dyneither(_tmp34A,sizeof(void*),0));});});{
# 1803
struct _RegionHandle*_tmp34C=Cyc_Core_heap_region;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(te);
if(te2->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp74F=Cyc_Tcenv_new_block(loc,te2);te2=_tmp74F;});{
# 1809
struct Cyc_Tcenv_Tenv*_tmp34D=te2;
void**_tmp34E=0;
struct Cyc_Absyn_Tqual*_tmp34F=0;
union Cyc_Absyn_Constraint**_tmp350=0;
# 1814
if(topt != 0){
void*_tmp351=Cyc_Tcutil_compress(*topt);void*_tmp352=_tmp351;void*_tmp35D;struct Cyc_Absyn_Tqual _tmp35C;struct Cyc_Absyn_Exp*_tmp35B;union Cyc_Absyn_Constraint*_tmp35A;struct Cyc_Absyn_PtrInfo _tmp359;switch(*((int*)_tmp352)){case 5: _LL1F6: _tmp359=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp352)->f1;_LL1F7:
# 1817
({void**_tmp750=({void**_tmp353=_region_malloc(_tmp34C,sizeof(*_tmp353));_tmp353[0]=_tmp359.elt_typ;_tmp353;});_tmp34E=_tmp750;});
({struct Cyc_Absyn_Tqual*_tmp751=({struct Cyc_Absyn_Tqual*_tmp354=_region_malloc(_tmp34C,sizeof(*_tmp354));_tmp354[0]=_tmp359.elt_tq;_tmp354;});_tmp34F=_tmp751;});
({union Cyc_Absyn_Constraint**_tmp752=({union Cyc_Absyn_Constraint**_tmp355=_region_malloc(_tmp34C,sizeof(*_tmp355));_tmp355[0]=(_tmp359.ptr_atts).zero_term;_tmp355;});_tmp350=_tmp752;});
goto _LL1F5;case 8: _LL1F8: _tmp35D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp352)->f1).elt_type;_tmp35C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp352)->f1).tq;_tmp35B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp352)->f1).num_elts;_tmp35A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp352)->f1).zero_term;_LL1F9:
# 1822
({void**_tmp753=({void**_tmp356=_region_malloc(_tmp34C,sizeof(*_tmp356));_tmp356[0]=_tmp35D;_tmp356;});_tmp34E=_tmp753;});
({struct Cyc_Absyn_Tqual*_tmp754=({struct Cyc_Absyn_Tqual*_tmp357=_region_malloc(_tmp34C,sizeof(*_tmp357));_tmp357[0]=_tmp35C;_tmp357;});_tmp34F=_tmp754;});
({union Cyc_Absyn_Constraint**_tmp755=({union Cyc_Absyn_Constraint**_tmp358=_region_malloc(_tmp34C,sizeof(*_tmp358));_tmp358[0]=_tmp35A;_tmp358;});_tmp350=_tmp755;});
goto _LL1F5;default: _LL1FA: _LL1FB:
# 1827
 goto _LL1F5;}_LL1F5:;}{
# 1830
void*t=Cyc_Tcexp_tcExp(_tmp34D,_tmp34E,body);
# 1832
struct _RegionHandle*_tmp35E=Cyc_Core_heap_region;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp35E,t) && !Cyc_Tcutil_is_noalias_path(_tmp35E,body))
({void*_tmp35F=0;({unsigned int _tmp757=body->loc;struct _dyneither_ptr _tmp756=({const char*_tmp360="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp360,sizeof(char),49);});Cyc_Tcutil_terr(_tmp757,_tmp756,_tag_dyneither(_tmp35F,sizeof(void*),0));});});
# 1836
if(_tmp34D->le == 0){
# 1838
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp361=0;({unsigned int _tmp759=bound->loc;struct _dyneither_ptr _tmp758=({const char*_tmp362="bound is not constant";_tag_dyneither(_tmp362,sizeof(char),22);});Cyc_Tcutil_terr(_tmp759,_tmp758,_tag_dyneither(_tmp361,sizeof(void*),0));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp363=0;({unsigned int _tmp75B=bound->loc;struct _dyneither_ptr _tmp75A=({const char*_tmp364="body is not constant";_tag_dyneither(_tmp364,sizeof(char),21);});Cyc_Tcutil_terr(_tmp75B,_tmp75A,_tag_dyneither(_tmp363,sizeof(void*),0));});});}
# 1843
if(_tmp350 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp350)){
# 1846
struct Cyc_Absyn_Exp*_tmp365=Cyc_Absyn_uint_exp(1,0);_tmp365->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp75C=Cyc_Absyn_add_exp(bound,_tmp365,0);bound=_tmp75C;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1850
struct _RegionHandle*_tmp366=Cyc_Core_heap_region;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp366,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(_tmp366,body))
({void*_tmp367=0;({unsigned int _tmp75E=body->loc;struct _dyneither_ptr _tmp75D=({const char*_tmp368="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp368,sizeof(char),49);});Cyc_Tcutil_terr(_tmp75E,_tmp75D,_tag_dyneither(_tmp367,sizeof(void*),0));});});{
# 1856
void*_tmp369=({void*_tmp761=t;struct Cyc_Absyn_Tqual _tmp760=_tmp34F == 0?Cyc_Absyn_empty_tqual(0):*_tmp34F;struct Cyc_Absyn_Exp*_tmp75F=bound;Cyc_Absyn_array_typ(_tmp761,_tmp760,_tmp75F,
_tmp350 == 0?Cyc_Absyn_false_conref:*_tmp350,0);});
return _tmp369;};};};};};}
# 1863
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1866
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp36A=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp36B=_tmp36A;void*_tmp370;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp36B)->tag == 19){_LL1FD: _tmp370=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp36B)->f1;_LL1FE:
# 1871
 if(({Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr;})){
struct Cyc_Absyn_Exp*_tmp36C=({void*_tmp762=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp762,Cyc_Absyn_valueof_exp(_tmp370,0),0,Cyc_Absyn_No_coercion,0);});
_tmp36C->topt=bound->topt;
# 1876
bound=_tmp36C;}
# 1878
goto _LL1FC;}else{_LL1FF: _LL200:
# 1880
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp36F;_tmp36F.tag=0;({struct _dyneither_ptr _tmp763=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp36F.f1=_tmp763;});({void*_tmp36D[1]={& _tmp36F};({unsigned int _tmp765=bound->loc;struct _dyneither_ptr _tmp764=({const char*_tmp36E="expecting unsigned int, found %s";_tag_dyneither(_tmp36E,sizeof(char),33);});Cyc_Tcutil_terr(_tmp765,_tmp764,_tag_dyneither(_tmp36D,sizeof(void*),1));});});});}_LL1FC:;}{
# 1885
struct _RegionHandle*_tmp371=Cyc_Core_heap_region;
void**_tmp372=0;
struct Cyc_Absyn_Tqual*_tmp373=0;
union Cyc_Absyn_Constraint**_tmp374=0;
# 1890
if(topt != 0){
void*_tmp375=Cyc_Tcutil_compress(*topt);void*_tmp376=_tmp375;void*_tmp381;struct Cyc_Absyn_Tqual _tmp380;struct Cyc_Absyn_Exp*_tmp37F;union Cyc_Absyn_Constraint*_tmp37E;struct Cyc_Absyn_PtrInfo _tmp37D;switch(*((int*)_tmp376)){case 5: _LL202: _tmp37D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376)->f1;_LL203:
# 1893
({void**_tmp766=({void**_tmp377=_region_malloc(_tmp371,sizeof(*_tmp377));_tmp377[0]=_tmp37D.elt_typ;_tmp377;});_tmp372=_tmp766;});
({struct Cyc_Absyn_Tqual*_tmp767=({struct Cyc_Absyn_Tqual*_tmp378=_region_malloc(_tmp371,sizeof(*_tmp378));_tmp378[0]=_tmp37D.elt_tq;_tmp378;});_tmp373=_tmp767;});
({union Cyc_Absyn_Constraint**_tmp768=({union Cyc_Absyn_Constraint**_tmp379=_region_malloc(_tmp371,sizeof(*_tmp379));_tmp379[0]=(_tmp37D.ptr_atts).zero_term;_tmp379;});_tmp374=_tmp768;});
goto _LL201;case 8: _LL204: _tmp381=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376)->f1).elt_type;_tmp380=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376)->f1).tq;_tmp37F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376)->f1).num_elts;_tmp37E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp376)->f1).zero_term;_LL205:
# 1898
({void**_tmp769=({void**_tmp37A=_region_malloc(_tmp371,sizeof(*_tmp37A));_tmp37A[0]=_tmp381;_tmp37A;});_tmp372=_tmp769;});
({struct Cyc_Absyn_Tqual*_tmp76A=({struct Cyc_Absyn_Tqual*_tmp37B=_region_malloc(_tmp371,sizeof(*_tmp37B));_tmp37B[0]=_tmp380;_tmp37B;});_tmp373=_tmp76A;});
({union Cyc_Absyn_Constraint**_tmp76B=({union Cyc_Absyn_Constraint**_tmp37C=_region_malloc(_tmp371,sizeof(*_tmp37C));_tmp37C[0]=_tmp37E;_tmp37C;});_tmp374=_tmp76B;});
goto _LL201;default: _LL206: _LL207:
# 1903
 goto _LL201;}_LL201:;}
# 1906
({unsigned int _tmp76E=loc;struct Cyc_Tcenv_Tenv*_tmp76D=te;struct Cyc_List_List*_tmp76C=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp76E,_tmp76D,_tmp76C,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp372 != 0)Cyc_Tcutil_unify(*_tmp372,t);
# 1909
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp382=0;({unsigned int _tmp770=bound->loc;struct _dyneither_ptr _tmp76F=({const char*_tmp383="bound is not constant";_tag_dyneither(_tmp383,sizeof(char),22);});Cyc_Tcutil_terr(_tmp770,_tmp76F,_tag_dyneither(_tmp382,sizeof(void*),0));});});}
# 1913
if(_tmp374 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp374)){
# 1916
struct Cyc_Absyn_Exp*_tmp384=Cyc_Absyn_uint_exp(1,0);_tmp384->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp771=Cyc_Absyn_add_exp(bound,_tmp384,0);bound=_tmp771;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1920
({void*_tmp385=0;({unsigned int _tmp773=loc;struct _dyneither_ptr _tmp772=({const char*_tmp386="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp386,sizeof(char),74);});Cyc_Tcutil_terr(_tmp773,_tmp772,_tag_dyneither(_tmp385,sizeof(void*),0));});});}{
# 1923
void*_tmp387=({void*_tmp776=t;struct Cyc_Absyn_Tqual _tmp775=_tmp373 == 0?Cyc_Absyn_empty_tqual(0):*_tmp373;struct Cyc_Absyn_Exp*_tmp774=bound;Cyc_Absyn_array_typ(_tmp776,_tmp775,_tmp774,
_tmp374 == 0?Cyc_Absyn_false_conref:*_tmp374,0);});
return _tmp387;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1939 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1943
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp777=({struct Cyc_Absyn_Aggrdecl**_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388[0]=ad;_tmp388;});adptr=_tmp777;});}else{
# 1949
{struct _handler_cons _tmp389;_push_handler(& _tmp389);{int _tmp38B=0;if(setjmp(_tmp389.handler))_tmp38B=1;if(!_tmp38B){({struct Cyc_Absyn_Aggrdecl**_tmp778=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp778;});
ad=*adptr;
# 1949
;_pop_handler();}else{void*_tmp38A=(void*)_exn_thrown;void*_tmp38C=_tmp38A;void*_tmp390;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp38C)->tag == Cyc_Dict_Absent){_LL209: _LL20A:
# 1952
({struct Cyc_String_pa_PrintArg_struct _tmp38F;_tmp38F.tag=0;({struct _dyneither_ptr _tmp779=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp38F.f1=_tmp779;});({void*_tmp38D[1]={& _tmp38F};({unsigned int _tmp77B=loc;struct _dyneither_ptr _tmp77A=({const char*_tmp38E="unbound struct/union name %s";_tag_dyneither(_tmp38E,sizeof(char),29);});Cyc_Tcutil_terr(_tmp77B,_tmp77A,_tag_dyneither(_tmp38D,sizeof(void*),1));});});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL20B: _tmp390=_tmp38C;_LL20C:(int)_rethrow(_tmp390);}_LL208:;}};}
# 1955
*ad_opt=ad;
*tn=ad->name;}
# 1958
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp393;_tmp393.tag=0;({struct _dyneither_ptr _tmp77C=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp394="struct";_tag_dyneither(_tmp394,sizeof(char),7);}):({const char*_tmp395="union";_tag_dyneither(_tmp395,sizeof(char),6);}));_tmp393.f1=_tmp77C;});({void*_tmp391[1]={& _tmp393};({unsigned int _tmp77E=loc;struct _dyneither_ptr _tmp77D=({const char*_tmp392="can't construct abstract %s";_tag_dyneither(_tmp392,sizeof(char),28);});Cyc_Tcutil_terr(_tmp77E,_tmp77D,_tag_dyneither(_tmp391,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1962
struct _RegionHandle*_tmp396=Cyc_Core_heap_region;
# 1967
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp397=Cyc_Tcenv_new_status(te);
if(_tmp397 == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp77F=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp77F;});else{
# 1976
({struct Cyc_Tcenv_Tenv*_tmp780=Cyc_Tcenv_set_new_status(_tmp397,te);te2=_tmp780;});}{
# 1978
struct _tuple12 _tmp398=({struct _tuple12 _tmp3BF;({struct Cyc_List_List*_tmp781=Cyc_Tcenv_lookup_type_vars(te2);_tmp3BF.f1=_tmp781;});_tmp3BF.f2=_tmp396;_tmp3BF;});
struct Cyc_List_List*_tmp399=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp396,Cyc_Tcutil_r_make_inst_var,& _tmp398,ad->tvs);
struct Cyc_List_List*_tmp39A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp396,Cyc_Tcutil_r_make_inst_var,& _tmp398,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp39B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp399);
struct Cyc_List_List*_tmp39C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp39A);
struct Cyc_List_List*_tmp39D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(_tmp396,_tmp399,_tmp39A);
void*res_typ;
# 1989
if(topt != 0){
void*_tmp39E=Cyc_Tcutil_compress(*topt);void*_tmp39F=_tmp39E;struct Cyc_Absyn_Aggrdecl**_tmp3A5;struct Cyc_List_List*_tmp3A4;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39F)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39F)->f1).aggr_info).KnownAggr).tag == 2){_LL20E: _tmp3A5=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39F)->f1).aggr_info).KnownAggr).val;_tmp3A4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp39F)->f1).targs;_LL20F:
# 1992
 if(*_tmp3A5 == *adptr){
{struct Cyc_List_List*_tmp3A0=_tmp39B;for(0;_tmp3A0 != 0  && _tmp3A4 != 0;
(_tmp3A0=_tmp3A0->tl,_tmp3A4=_tmp3A4->tl)){
Cyc_Tcutil_unify((void*)_tmp3A0->hd,(void*)_tmp3A4->hd);}}
# 1997
res_typ=*topt;
goto _LL20D;}
# 2000
goto _LL211;}else{goto _LL210;}}else{_LL210: _LL211:
# 2002
({void*_tmp785=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp784=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3A2;_tmp3A2.tag=11;({struct Cyc_Absyn_AggrInfo _tmp783=({struct Cyc_Absyn_AggrInfo _tmp3A3;({union Cyc_Absyn_AggrInfoU _tmp782=Cyc_Absyn_KnownAggr(adptr);_tmp3A3.aggr_info=_tmp782;});_tmp3A3.targs=_tmp39B;_tmp3A3;});_tmp3A2.f1=_tmp783;});_tmp3A2;});_tmp3A1[0]=_tmp784;});_tmp3A1;});res_typ=_tmp785;});}_LL20D:;}else{
# 2005
({void*_tmp789=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp788=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3A7;_tmp3A7.tag=11;({struct Cyc_Absyn_AggrInfo _tmp787=({struct Cyc_Absyn_AggrInfo _tmp3A8;({union Cyc_Absyn_AggrInfoU _tmp786=Cyc_Absyn_KnownAggr(adptr);_tmp3A8.aggr_info=_tmp786;});_tmp3A8.targs=_tmp39B;_tmp3A8;});_tmp3A7.f1=_tmp787;});_tmp3A7;});_tmp3A6[0]=_tmp788;});_tmp3A6;});res_typ=_tmp789;});}{
# 2008
struct Cyc_List_List*_tmp3A9=*ts;
struct Cyc_List_List*_tmp3AA=_tmp39C;
while(_tmp3A9 != 0  && _tmp3AA != 0){
# 2012
({unsigned int _tmp78C=loc;struct Cyc_Tcenv_Tenv*_tmp78B=te2;struct Cyc_List_List*_tmp78A=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp78C,_tmp78B,_tmp78A,& Cyc_Tcutil_ak,1,0,(void*)_tmp3A9->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp3A9->hd);
Cyc_Tcutil_unify((void*)_tmp3A9->hd,(void*)_tmp3AA->hd);
_tmp3A9=_tmp3A9->tl;
_tmp3AA=_tmp3AA->tl;}
# 2018
if(_tmp3A9 != 0)
({void*_tmp3AB=0;({unsigned int _tmp78E=loc;struct _dyneither_ptr _tmp78D=({const char*_tmp3AC="too many explicit witness types";_tag_dyneither(_tmp3AC,sizeof(char),32);});Cyc_Tcutil_terr(_tmp78E,_tmp78D,_tag_dyneither(_tmp3AB,sizeof(void*),0));});});
# 2021
*ts=_tmp39C;{
# 2024
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp396,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3AD=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3AE=_tmp3AD;struct Cyc_Absyn_Aggrfield*_tmp3BA;struct Cyc_Absyn_Exp*_tmp3B9;_LL213: _tmp3BA=_tmp3AE->f1;_tmp3B9=_tmp3AE->f2;_LL214:;{
void*_tmp3AF=Cyc_Tcutil_rsubstitute(_tmp396,_tmp39D,_tmp3BA->type);
({struct Cyc_Tcenv_Tenv*_tmp78F=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp78F,& _tmp3AF,_tmp3B9);});
# 2035
if(!Cyc_Tcutil_coerce_arg(te2,_tmp3B9,_tmp3AF,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B6;_tmp3B6.tag=0;({struct _dyneither_ptr _tmp790=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2039
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3B9->topt)));_tmp3B6.f1=_tmp790;});({struct Cyc_String_pa_PrintArg_struct _tmp3B5;_tmp3B5.tag=0;({struct _dyneither_ptr _tmp791=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2038
Cyc_Absynpp_typ2string(_tmp3AF));_tmp3B5.f1=_tmp791;});({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0;({struct _dyneither_ptr _tmp792=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp3B4.f1=_tmp792;});({struct Cyc_String_pa_PrintArg_struct _tmp3B3;_tmp3B3.tag=0;({struct _dyneither_ptr _tmp793=(struct _dyneither_ptr)(
# 2037
ad->kind == Cyc_Absyn_StructA?({const char*_tmp3B7="struct";_tag_dyneither(_tmp3B7,sizeof(char),7);}):({const char*_tmp3B8="union";_tag_dyneither(_tmp3B8,sizeof(char),6);}));_tmp3B3.f1=_tmp793;});({struct Cyc_String_pa_PrintArg_struct _tmp3B2;_tmp3B2.tag=0;_tmp3B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3BA->name);({void*_tmp3B0[5]={& _tmp3B2,& _tmp3B3,& _tmp3B4,& _tmp3B5,& _tmp3B6};({unsigned int _tmp795=_tmp3B9->loc;struct _dyneither_ptr _tmp794=({const char*_tmp3B1="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp3B1,sizeof(char),40);});Cyc_Tcutil_terr(_tmp795,_tmp794,_tag_dyneither(_tmp3B0,sizeof(void*),5));});});});});});});});
# 2040
Cyc_Tcutil_explain_failure();}};}{
# 2044
struct Cyc_List_List*_tmp3BB=0;
{struct Cyc_List_List*_tmp3BC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp3BC != 0;_tmp3BC=_tmp3BC->tl){
({struct Cyc_List_List*_tmp799=({struct Cyc_List_List*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));({struct _tuple0*_tmp798=({struct _tuple0*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));({void*_tmp796=Cyc_Tcutil_rsubstitute(_tmp396,_tmp39D,(*((struct _tuple0*)_tmp3BC->hd)).f1);_tmp3BE->f1=_tmp796;});({void*_tmp797=
Cyc_Tcutil_rsubstitute(_tmp396,_tmp39D,(*((struct _tuple0*)_tmp3BC->hd)).f2);_tmp3BE->f2=_tmp797;});_tmp3BE;});_tmp3BD->hd=_tmp798;});_tmp3BD->tl=_tmp3BB;_tmp3BD;});
# 2046
_tmp3BB=_tmp799;});}}
# 2049
({struct Cyc_List_List*_tmp79A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3BB);_tmp3BB=_tmp79A;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp3BB);
return res_typ;};};};};};}
# 2056
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2058
struct _RegionHandle*_tmp3C0=Cyc_Core_heap_region;{
void*_tmp3C1=Cyc_Tcutil_compress(ts);void*_tmp3C2=_tmp3C1;enum Cyc_Absyn_AggrKind _tmp3D1;struct Cyc_List_List*_tmp3D0;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C2)->tag == 12){_LL216: _tmp3D1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C2)->f1;_tmp3D0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C2)->f2;_LL217:
# 2061
 if(_tmp3D1 == Cyc_Absyn_UnionA)
({void*_tmp3C3=0;({unsigned int _tmp79C=loc;struct _dyneither_ptr _tmp79B=({const char*_tmp3C4="expecting struct but found union";_tag_dyneither(_tmp3C4,sizeof(char),33);});Cyc_Tcutil_terr(_tmp79C,_tmp79B,_tag_dyneither(_tmp3C3,sizeof(void*),0));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(_tmp3C0,loc,args,Cyc_Absyn_StructA,_tmp3D0);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3C5=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3C6=_tmp3C5;struct Cyc_Absyn_Aggrfield*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CC;_LL21B: _tmp3CD=_tmp3C6->f1;_tmp3CC=_tmp3C6->f2;_LL21C:;
({struct Cyc_Tcenv_Tenv*_tmp79E=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp79D=& _tmp3CD->type;Cyc_Tcexp_tcExpInitializer(_tmp79E,_tmp79D,_tmp3CC);});
# 2070
if(!Cyc_Tcutil_coerce_arg(te,_tmp3CC,_tmp3CD->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3CB;_tmp3CB.tag=0;({struct _dyneither_ptr _tmp79F=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2073
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3CC->topt)));_tmp3CB.f1=_tmp79F;});({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0;({struct _dyneither_ptr _tmp7A0=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2072
Cyc_Absynpp_typ2string(_tmp3CD->type));_tmp3CA.f1=_tmp7A0;});({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0;_tmp3C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3CD->name);({void*_tmp3C7[3]={& _tmp3C9,& _tmp3CA,& _tmp3CB};({unsigned int _tmp7A2=_tmp3CC->loc;struct _dyneither_ptr _tmp7A1=({const char*_tmp3C8="field %s of struct expects type %s != %s";_tag_dyneither(_tmp3C8,sizeof(char),41);});Cyc_Tcutil_terr(_tmp7A2,_tmp7A1,_tag_dyneither(_tmp3C7,sizeof(void*),3));});});});});});
# 2074
Cyc_Tcutil_explain_failure();}}
# 2077
goto _LL215;};}else{_LL218: _LL219:
({void*_tmp3CE=0;({struct _dyneither_ptr _tmp7A3=({const char*_tmp3CF="tcAnonStruct: wrong type";_tag_dyneither(_tmp3CF,sizeof(char),25);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7A3,_tag_dyneither(_tmp3CE,sizeof(void*),0));});});}_LL215:;}
# 2081
return ts;}
# 2085
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2088
struct _RegionHandle*_tmp3D2=Cyc_Core_heap_region;
struct _tuple12 _tmp3D3=({struct _tuple12 _tmp3E7;({struct Cyc_List_List*_tmp7A4=Cyc_Tcenv_lookup_type_vars(te);_tmp3E7.f1=_tmp7A4;});_tmp3E7.f2=_tmp3D2;_tmp3E7;});
struct Cyc_List_List*_tmp3D4=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp3D2,Cyc_Tcutil_r_make_inst_var,& _tmp3D3,tud->tvs);
struct Cyc_List_List*_tmp3D5=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3D4);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3E4=_cycalloc(sizeof(*_tmp3E4));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp7A7=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp3E5;_tmp3E5.tag=4;({struct Cyc_Absyn_DatatypeFieldInfo _tmp7A6=({struct Cyc_Absyn_DatatypeFieldInfo _tmp3E6;({union Cyc_Absyn_DatatypeFieldInfoU _tmp7A5=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp3E6.field_info=_tmp7A5;});_tmp3E6.targs=_tmp3D5;_tmp3E6;});_tmp3E5.f1=_tmp7A6;});_tmp3E5;});_tmp3E4[0]=_tmp7A7;});_tmp3E4;});
# 2096
if(topt != 0){
void*_tmp3D6=Cyc_Tcutil_compress(*topt);void*_tmp3D7=_tmp3D6;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3D7)->tag == 4){_LL21E: _LL21F:
 Cyc_Tcutil_unify(*topt,res);goto _LL21D;}else{_LL220: _LL221:
 goto _LL21D;}_LL21D:;}{
# 2102
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp3D4 != 0)({void*_tmp7A8=Cyc_Tcutil_rsubstitute(_tmp3D2,_tmp3D4,t);t=_tmp7A8;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=0;({struct _dyneither_ptr _tmp7A9=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2113
e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3DD="?";_tag_dyneither(_tmp3DD,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp3DC.f1=_tmp7A9;});({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0;({struct _dyneither_ptr _tmp7AA=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2112
Cyc_Absynpp_typ2string(t));_tmp3DB.f1=_tmp7AA;});({struct Cyc_String_pa_PrintArg_struct _tmp3DA;_tmp3DA.tag=0;({struct _dyneither_ptr _tmp7AB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp3DA.f1=_tmp7AB;});({void*_tmp3D8[3]={& _tmp3DA,& _tmp3DB,& _tmp3DC};({unsigned int _tmp7AD=e->loc;struct _dyneither_ptr _tmp7AC=({const char*_tmp3D9="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3D9,sizeof(char),82);});Cyc_Tcutil_terr(_tmp7AD,_tmp7AC,_tag_dyneither(_tmp3D8,sizeof(void*),3));});});});});});
# 2114
Cyc_Tcutil_explain_failure();}}
# 2117
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0;({struct _dyneither_ptr _tmp7AE=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2120
Cyc_Absynpp_qvar2string(tuf->name));_tmp3E0.f1=_tmp7AE;});({void*_tmp3DE[1]={& _tmp3E0};({struct Cyc_Tcenv_Tenv*_tmp7B2=te;unsigned int _tmp7B1=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7B0=topt;struct _dyneither_ptr _tmp7AF=({const char*_tmp3DF="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3DF,sizeof(char),47);});Cyc_Tcexp_expr_err(_tmp7B2,_tmp7B1,_tmp7B0,_tmp7AF,_tag_dyneither(_tmp3DE,sizeof(void*),1));});});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0;({struct _dyneither_ptr _tmp7B3=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));_tmp3E3.f1=_tmp7B3;});({void*_tmp3E1[1]={& _tmp3E3};({struct Cyc_Tcenv_Tenv*_tmp7B7=te;unsigned int _tmp7B6=loc;void**_tmp7B5=topt;struct _dyneither_ptr _tmp7B4=({const char*_tmp3E2="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3E2,sizeof(char),46);});Cyc_Tcexp_expr_err(_tmp7B7,_tmp7B6,_tmp7B5,_tmp7B4,_tag_dyneither(_tmp3E1,sizeof(void*),1));});});});
return res;};}
# 2128
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2130
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2132
if(topt != 0){
void*_tmp3E8=Cyc_Tcutil_compress(*topt);void*_tmp3E9=_tmp3E8;void*_tmp3EA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9)->tag == 5){_LL223: _tmp3EA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E9)->f1).elt_typ;_LL224:
# 2135
 Cyc_Tcutil_unify(_tmp3EA,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL222;}else{_LL225: _LL226:
 goto _LL222;}_LL222:;}
# 2141
({struct Cyc_String_pa_PrintArg_struct _tmp3EE;_tmp3EE.tag=0;({struct _dyneither_ptr _tmp7B8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp3EE.f1=_tmp7B8;});({struct Cyc_String_pa_PrintArg_struct _tmp3ED;_tmp3ED.tag=0;({struct _dyneither_ptr _tmp7B9=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3EF="calloc";_tag_dyneither(_tmp3EF,sizeof(char),7);}):(struct _dyneither_ptr)({const char*_tmp3F0="malloc";_tag_dyneither(_tmp3F0,sizeof(char),7);})));_tmp3ED.f1=_tmp7B9;});({void*_tmp3EB[2]={& _tmp3ED,& _tmp3EE};({unsigned int _tmp7BB=loc;struct _dyneither_ptr _tmp7BA=({const char*_tmp3EC="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3EC,sizeof(char),60);});Cyc_Tcutil_terr(_tmp7BB,_tmp7BA,_tag_dyneither(_tmp3EB,sizeof(void*),2));});});});});}
# 2145
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2147
enum Cyc_Absyn_AliasQual _tmp3F1=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3F2=_tmp3F1;if(_tmp3F2 == Cyc_Absyn_Unique){_LL228: _LL229:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL22A: _LL22B:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL227:;}
# 2153
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2158
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2161
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3F9=_cycalloc(sizeof(*_tmp3F9));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp7BD=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp3FA;_tmp3FA.tag=15;({void*_tmp7BC=({
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));});_tmp3FA.f1=_tmp7BC;});_tmp3FA;});_tmp3F9[0]=_tmp7BD;});_tmp3F9;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3F3=Cyc_Tcutil_compress(handle_type);void*_tmp3F4=_tmp3F3;void*_tmp3F8;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F4)->tag == 15){_LL22D: _tmp3F8=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F4)->f1;_LL22E:
# 2166
 rgn=_tmp3F8;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22C;}else{_LL22F: _LL230:
# 2170
({struct Cyc_String_pa_PrintArg_struct _tmp3F7;_tmp3F7.tag=0;({struct _dyneither_ptr _tmp7BE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));_tmp3F7.f1=_tmp7BE;});({void*_tmp3F5[1]={& _tmp3F7};({unsigned int _tmp7C0=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7BF=({const char*_tmp3F6="expecting region_t type but found %s";_tag_dyneither(_tmp3F6,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7C0,_tmp7BF,_tag_dyneither(_tmp3F5,sizeof(void*),1));});});});
goto _LL22C;}_LL22C:;}else{
# 2177
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7C1=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7C1;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2184
({struct Cyc_Tcenv_Tenv*_tmp7C2=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7C2,& Cyc_Absyn_uint_typ,*e);});{
# 2193 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3FB=0;({struct _dyneither_ptr _tmp7C3=({const char*_tmp3FC="calloc with empty type";_tag_dyneither(_tmp3FC,sizeof(char),23);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7C3,_tag_dyneither(_tmp3FB,sizeof(void*),0));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7C6=loc;struct Cyc_Tcenv_Tenv*_tmp7C5=te;struct Cyc_List_List*_tmp7C4=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7C6,_tmp7C5,_tmp7C4,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2205
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3FD=er;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Exp*_tmp412;void*_tmp411;switch(*((int*)_tmp3FD)){case 16: _LL232: _tmp411=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_LL233:
# 2209
 elt_type=_tmp411;
({void**_tmp7C7=({void**_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE[0]=elt_type;_tmp3FE;});*t=_tmp7C7;});
({struct Cyc_Absyn_Exp*_tmp7C8=Cyc_Absyn_uint_exp(1,0);num_elts=_tmp7C8;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL231;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2)->tl)->tl == 0){_LL234: _tmp413=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2)->hd;_tmp412=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2)->tl)->hd;_LL235:
# 2216
{struct _tuple0 _tmp3FF=({struct _tuple0 _tmp405;_tmp405.f1=_tmp413->r;_tmp405.f2=_tmp412->r;_tmp405;});struct _tuple0 _tmp400=_tmp3FF;void*_tmp404;void*_tmp403;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp400.f1)->tag == 16){_LL239: _tmp403=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp400.f1)->f1;_LL23A:
# 2218
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp403);
elt_type=_tmp403;
({void**_tmp7C9=({void**_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401[0]=elt_type;_tmp401;});*t=_tmp7C9;});
num_elts=_tmp412;
one_elt=0;
goto _LL238;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp400.f2)->tag == 16){_LL23B: _tmp404=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp400.f2)->f1;_LL23C:
# 2225
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp404);
elt_type=_tmp404;
({void**_tmp7CA=({void**_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402[0]=elt_type;_tmp402;});*t=_tmp7CA;});
num_elts=_tmp413;
one_elt=0;
goto _LL238;}else{_LL23D: _LL23E:
 goto No_sizeof;}}_LL238:;}
# 2233
goto _LL231;}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}default: _LL236: _LL237:
# 2235
 No_sizeof: {
# 2238
struct Cyc_Absyn_Exp*_tmp406=*e;
{void*_tmp407=_tmp406->r;void*_tmp408=_tmp407;struct Cyc_Absyn_Exp*_tmp409;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp408)->tag == 13){_LL240: _tmp409=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp408)->f2;_LL241:
 _tmp406=_tmp409;goto _LL23F;}else{_LL242: _LL243:
 goto _LL23F;}_LL23F:;}
# 2243
{void*_tmp40A=Cyc_Tcutil_compress((void*)_check_null(_tmp406->topt));void*_tmp40B=_tmp40A;void*_tmp40F;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp40B)->tag == 19){_LL245: _tmp40F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp40B)->f1;_LL246:
# 2245
{void*_tmp40C=Cyc_Tcutil_compress(_tmp40F);void*_tmp40D=_tmp40C;struct Cyc_Absyn_Exp*_tmp40E;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp40D)->tag == 18){_LL24A: _tmp40E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp40D)->f1;_LL24B:
# 2247
 er=_tmp40E->r;goto retry_sizeof;}else{_LL24C: _LL24D:
 goto _LL249;}_LL249:;}
# 2250
goto _LL244;}else{_LL247: _LL248:
 goto _LL244;}_LL244:;}
# 2253
elt_type=Cyc_Absyn_char_typ;
({void**_tmp7CB=({void**_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410[0]=elt_type;_tmp410;});*t=_tmp7CB;});
num_elts=*e;
one_elt=0;}
# 2258
goto _LL231;}_LL231:;}}
# 2262
*is_fat=!one_elt;
# 2265
{void*_tmp414=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp417;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp414)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp414)->f1).aggr_info).KnownAggr).tag == 2){_LL24F: _tmp417=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp414)->f1).aggr_info).KnownAggr).val;_LL250:
# 2267
 if(_tmp417->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp417->impl))->exist_vars != 0)
({void*_tmp415=0;({unsigned int _tmp7CD=loc;struct _dyneither_ptr _tmp7CC=({const char*_tmp416="malloc with existential types not yet implemented";_tag_dyneither(_tmp416,sizeof(char),50);});Cyc_Tcutil_terr(_tmp7CD,_tmp7CC,_tag_dyneither(_tmp415,sizeof(void*),0));});});
goto _LL24E;}else{goto _LL251;}}else{_LL251: _LL252:
 goto _LL24E;}_LL24E:;}{
# 2274
void*(*_tmp418)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp419=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp41A=Cyc_Tcutil_compress(*topt);void*_tmp41B=_tmp41A;union Cyc_Absyn_Constraint*_tmp428;union Cyc_Absyn_Constraint*_tmp427;union Cyc_Absyn_Constraint*_tmp426;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->tag == 5){_LL254: _tmp428=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->f1).ptr_atts).nullable;_tmp427=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->f1).ptr_atts).bounds;_tmp426=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41B)->f1).ptr_atts).zero_term;_LL255:
# 2279
 _tmp419=_tmp426;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp428))
_tmp418=Cyc_Absyn_star_typ;
# 2284
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp426) && !(*is_calloc)){
({void*_tmp41C=0;({unsigned int _tmp7CF=loc;struct _dyneither_ptr _tmp7CE=({const char*_tmp41D="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp41D,sizeof(char),55);});Cyc_Tcutil_warn(_tmp7CF,_tmp7CE,_tag_dyneither(_tmp41C,sizeof(void*),0));});});
*is_calloc=1;}
# 2290
{void*_tmp41E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp427);void*_tmp41F=_tmp41E;struct Cyc_Absyn_Exp*_tmp425;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp41F)->tag == 0){_LL259: _LL25A:
 goto _LL258;}else{_LL25B: _tmp425=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp41F)->f1;if(!one_elt){_LL25C: {
# 2293
int _tmp420=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp420  && Cyc_Evexp_same_const_exp(_tmp425,num_elts)){
*is_fat=0;
return({void*_tmp7D3=elt_type;void*_tmp7D2=rgn;struct Cyc_Absyn_Tqual _tmp7D1=Cyc_Absyn_empty_tqual(0);void*_tmp7D0=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp427);Cyc_Absyn_atb_typ(_tmp7D3,_tmp7D2,_tmp7D1,_tmp7D0,_tmp419);});}
# 2299
{void*_tmp421=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp422=_tmp421;void*_tmp424;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp422)->tag == 19){_LL260: _tmp424=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp422)->f1;_LL261: {
# 2301
struct Cyc_Absyn_Exp*_tmp423=({void*_tmp7D4=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp7D4,Cyc_Absyn_valueof_exp(_tmp424,0),0,Cyc_Absyn_No_coercion,0);});
# 2303
if(Cyc_Evexp_same_const_exp(_tmp423,_tmp425)){
*is_fat=0;
return({void*_tmp7D8=elt_type;void*_tmp7D7=rgn;struct Cyc_Absyn_Tqual _tmp7D6=Cyc_Absyn_empty_tqual(0);void*_tmp7D5=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp427);Cyc_Absyn_atb_typ(_tmp7D8,_tmp7D7,_tmp7D6,_tmp7D5,_tmp419);});}
# 2308
goto _LL25F;}}else{_LL262: _LL263:
 goto _LL25F;}_LL25F:;}
# 2311
goto _LL258;}}else{_LL25D: _LL25E:
 goto _LL258;}}_LL258:;}
# 2314
goto _LL253;}else{_LL256: _LL257:
 goto _LL253;}_LL253:;}
# 2317
if(!one_elt)_tmp418=Cyc_Absyn_dyneither_typ;
return({void*_tmp7DB=elt_type;void*_tmp7DA=rgn;struct Cyc_Absyn_Tqual _tmp7D9=Cyc_Absyn_empty_tqual(0);_tmp418(_tmp7DB,_tmp7DA,_tmp7D9,_tmp419);});};};}
# 2322
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2324
struct Cyc_Tcenv_Tenv*_tmp429=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp429,0,e1);{
void*_tmp42A=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp429,& _tmp42A,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2331
{void*_tmp42B=Cyc_Tcutil_compress(t1);void*_tmp42C=_tmp42B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42C)->tag == 8){_LL265: _LL266:
({void*_tmp42D=0;({unsigned int _tmp7DD=loc;struct _dyneither_ptr _tmp7DC=({const char*_tmp42E="cannot assign to an array";_tag_dyneither(_tmp42E,sizeof(char),26);});Cyc_Tcutil_terr(_tmp7DD,_tmp7DC,_tag_dyneither(_tmp42D,sizeof(void*),0));});});goto _LL264;}else{_LL267: _LL268:
 goto _LL264;}_LL264:;}
# 2336
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp42F=0;({unsigned int _tmp7DF=loc;struct _dyneither_ptr _tmp7DE=({const char*_tmp430="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp430,sizeof(char),58);});Cyc_Tcutil_terr(_tmp7DF,_tmp7DE,_tag_dyneither(_tmp42F,sizeof(void*),0));});});
# 2340
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp431=0;({struct Cyc_Tcenv_Tenv*_tmp7E3=te;unsigned int _tmp7E2=e1->loc;void**_tmp7E1=topt;struct _dyneither_ptr _tmp7E0=({const char*_tmp432="swap non-lvalue";_tag_dyneither(_tmp432,sizeof(char),16);});Cyc_Tcexp_expr_err(_tmp7E3,_tmp7E2,_tmp7E1,_tmp7E0,_tag_dyneither(_tmp431,sizeof(void*),0));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp433=0;({struct Cyc_Tcenv_Tenv*_tmp7E7=te;unsigned int _tmp7E6=e2->loc;void**_tmp7E5=topt;struct _dyneither_ptr _tmp7E4=({const char*_tmp434="swap non-lvalue";_tag_dyneither(_tmp434,sizeof(char),16);});Cyc_Tcexp_expr_err(_tmp7E7,_tmp7E6,_tmp7E5,_tmp7E4,_tag_dyneither(_tmp433,sizeof(void*),0));});});{
# 2345
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp435=0;({struct Cyc_Tcenv_Tenv*_tmp7EB=te;unsigned int _tmp7EA=e1->loc;void**_tmp7E9=topt;struct _dyneither_ptr _tmp7E8=({const char*_tmp436="swap value in zeroterm array";_tag_dyneither(_tmp436,sizeof(char),29);});Cyc_Tcexp_expr_err(_tmp7EB,_tmp7EA,_tmp7E9,_tmp7E8,_tag_dyneither(_tmp435,sizeof(void*),0));});});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp437=0;({struct Cyc_Tcenv_Tenv*_tmp7EF=te;unsigned int _tmp7EE=e2->loc;void**_tmp7ED=topt;struct _dyneither_ptr _tmp7EC=({const char*_tmp438="swap value in zeroterm array";_tag_dyneither(_tmp438,sizeof(char),29);});Cyc_Tcexp_expr_err(_tmp7EF,_tmp7EE,_tmp7ED,_tmp7EC,_tag_dyneither(_tmp437,sizeof(void*),0));});});
# 2352
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp439=({struct Cyc_String_pa_PrintArg_struct _tmp43D;_tmp43D.tag=0;({struct _dyneither_ptr _tmp7F0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp43D.f1=_tmp7F0;});({struct Cyc_String_pa_PrintArg_struct _tmp43C;_tmp43C.tag=0;({struct _dyneither_ptr _tmp7F1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp43C.f1=_tmp7F1;});({void*_tmp43A[2]={& _tmp43C,& _tmp43D};({struct Cyc_Tcenv_Tenv*_tmp7F5=te;unsigned int _tmp7F4=loc;void**_tmp7F3=topt;struct _dyneither_ptr _tmp7F2=({const char*_tmp43B="type mismatch: %s != %s";_tag_dyneither(_tmp43B,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp7F5,_tmp7F4,_tmp7F3,_tmp7F2,_tag_dyneither(_tmp43A,sizeof(void*),2));});});});});
return _tmp439;}
# 2359
return(void*)& Cyc_Absyn_VoidType_val;};};};}
# 2363
int Cyc_Tcexp_in_stmt_exp=0;
# 2365
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
({struct Cyc_Tcenv_Tenv*_tmp7F6=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcstmt_tcStmt(_tmp7F6,s,1);});
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;
# 2370
while(1){
void*_tmp43E=s->r;void*_tmp43F=_tmp43E;struct Cyc_Absyn_Decl*_tmp44A;struct Cyc_Absyn_Stmt*_tmp449;struct Cyc_Absyn_Stmt*_tmp448;struct Cyc_Absyn_Stmt*_tmp447;struct Cyc_Absyn_Exp*_tmp446;switch(*((int*)_tmp43F)){case 1: _LL26A: _tmp446=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp43F)->f1;_LL26B: {
# 2374
void*_tmp440=(void*)_check_null(_tmp446->topt);
if(!({void*_tmp7F7=_tmp440;Cyc_Tcutil_unify(_tmp7F7,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp443;_tmp443.tag=0;({struct _dyneither_ptr _tmp7F8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp440));_tmp443.f1=_tmp7F8;});({void*_tmp441[1]={& _tmp443};({unsigned int _tmp7FA=loc;struct _dyneither_ptr _tmp7F9=({const char*_tmp442="statement expression returns type %s";_tag_dyneither(_tmp442,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7FA,_tmp7F9,_tag_dyneither(_tmp441,sizeof(void*),1));});});});
Cyc_Tcutil_explain_failure();}
# 2380
return _tmp440;}case 2: _LL26C: _tmp448=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp43F)->f1;_tmp447=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp43F)->f2;_LL26D:
 s=_tmp447;continue;case 12: _LL26E: _tmp44A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43F)->f1;_tmp449=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp43F)->f2;_LL26F:
 s=_tmp449;continue;default: _LL270: _LL271:
# 2384
 return({void*_tmp444=0;({struct Cyc_Tcenv_Tenv*_tmp7FE=te;unsigned int _tmp7FD=loc;void**_tmp7FC=topt;struct _dyneither_ptr _tmp7FB=({const char*_tmp445="statement expression must end with expression";_tag_dyneither(_tmp445,sizeof(char),46);});Cyc_Tcexp_expr_err(_tmp7FE,_tmp7FD,_tmp7FC,_tmp7FB,_tag_dyneither(_tmp444,sizeof(void*),0));});});}_LL269:;}}
# 2389
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp7FF=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7FF,0,e);}));
{void*_tmp44B=t;struct Cyc_Absyn_Aggrdecl*_tmp44F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44B)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44B)->f1).aggr_info).KnownAggr).tag == 2){_LL273: _tmp44F=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp44B)->f1).aggr_info).KnownAggr).val;_LL274:
# 2393
 if((_tmp44F->kind == Cyc_Absyn_UnionA  && _tmp44F->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44F->impl))->tagged)goto _LL272;
goto _LL276;}else{goto _LL275;}}else{_LL275: _LL276:
# 2396
({struct Cyc_String_pa_PrintArg_struct _tmp44E;_tmp44E.tag=0;({struct _dyneither_ptr _tmp800=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp44E.f1=_tmp800;});({void*_tmp44C[1]={& _tmp44E};({unsigned int _tmp802=loc;struct _dyneither_ptr _tmp801=({const char*_tmp44D="expecting @tagged union but found %s";_tag_dyneither(_tmp44D,sizeof(char),37);});Cyc_Tcutil_terr(_tmp802,_tmp801,_tag_dyneither(_tmp44C,sizeof(void*),1));});});});
goto _LL272;}_LL272:;}
# 2399
return Cyc_Absyn_uint_typ;}
# 2403
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2407
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle*_tmp450=Cyc_Core_heap_region;
struct Cyc_Tcenv_Tenv*_tmp451=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2412
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp458=_cycalloc(sizeof(*_tmp458));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp804=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp459;_tmp459.tag=15;({void*_tmp803=({
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp451));});_tmp459.f1=_tmp803;});_tmp459;});_tmp458[0]=_tmp804;});_tmp458;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp451,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp452=Cyc_Tcutil_compress(handle_type);void*_tmp453=_tmp452;void*_tmp457;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp453)->tag == 15){_LL278: _tmp457=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp453)->f1;_LL279:
# 2417
 rgn=_tmp457;
Cyc_Tcenv_check_rgn_accessible(_tmp451,loc,rgn);
goto _LL277;}else{_LL27A: _LL27B:
# 2421
({struct Cyc_String_pa_PrintArg_struct _tmp456;_tmp456.tag=0;({struct _dyneither_ptr _tmp805=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));_tmp456.f1=_tmp805;});({void*_tmp454[1]={& _tmp456};({unsigned int _tmp807=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp806=({const char*_tmp455="expecting region_t type but found %s";_tag_dyneither(_tmp455,sizeof(char),37);});Cyc_Tcutil_terr(_tmp807,_tmp806,_tag_dyneither(_tmp454,sizeof(void*),1));});});});
goto _LL277;}_LL277:;}else{
# 2428
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp808=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp808;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2436
void*_tmp45A=e1->r;void*_tmp45B=_tmp45A;struct Cyc_List_List*_tmp47B;struct Cyc_Core_Opt*_tmp47A;struct Cyc_List_List*_tmp479;switch(*((int*)_tmp45B)){case 26: _LL27D: _LL27E:
 goto _LL280;case 27: _LL27F: _LL280: {
# 2441
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp451,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp45C=0;({struct _dyneither_ptr _tmp809=({const char*_tmp45D="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp45D,sizeof(char),45);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp809,_tag_dyneither(_tmp45C,sizeof(void*),0));});});
({void*_tmp80A=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp80A;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp451,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp451,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2452
return res_typ;}case 35: _LL281: _tmp47A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_tmp479=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp45B)->f2;_LL282:
# 2454
({void*_tmp80C=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp45E=_cycalloc(sizeof(*_tmp45E));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp80B=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp45F;_tmp45F.tag=25;_tmp45F.f1=_tmp479;_tmp45F;});_tmp45E[0]=_tmp80B;});_tmp45E;});e1->r=_tmp80C;});
_tmp47B=_tmp479;goto _LL284;case 25: _LL283: _tmp47B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp45B)->f1;_LL284: {
# 2457
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp460=Cyc_Tcutil_compress(*topt);void*_tmp461=_tmp460;void**_tmp464;struct Cyc_Absyn_Tqual _tmp463;union Cyc_Absyn_Constraint*_tmp462;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp461)->tag == 5){_LL28C: _tmp464=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp461)->f1).elt_typ;_tmp463=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp461)->f1).elt_tq;_tmp462=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp461)->f1).ptr_atts).zero_term;_LL28D:
# 2463
 elt_typ_opt=_tmp464;
({int _tmp80D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp462);zero_term=_tmp80D;});
goto _LL28B;}else{_LL28E: _LL28F:
 goto _LL28B;}_LL28B:;}{
# 2469
void*res_typ=Cyc_Tcexp_tcArray(_tmp451,e1->loc,elt_typ_opt,0,zero_term,_tmp47B);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp465=0;({struct _dyneither_ptr _tmp80E=({const char*_tmp466="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp466,sizeof(char),50);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp80E,_tag_dyneither(_tmp465,sizeof(void*),0));});});
({void*_tmp80F=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp80F;});
if(topt != 0){
# 2478
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp451,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp451,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2484
return res_typ;};}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp45B)->f1).Wstring_c).tag){case 8: _LL285: _LL286: {
# 2489
void*_tmp467=({void*_tmp812=Cyc_Absyn_char_typ;void*_tmp811=rgn;struct Cyc_Absyn_Tqual _tmp810=Cyc_Absyn_const_tqual(0);Cyc_Absyn_atb_typ(_tmp812,_tmp811,_tmp810,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2491
void*_tmp468=Cyc_Tcexp_tcExp(_tmp451,& _tmp467,e1);
return({void*_tmp818=_tmp468;void*_tmp817=rgn;struct Cyc_Absyn_Tqual _tmp816=Cyc_Absyn_empty_tqual(0);void*_tmp815=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp469=_cycalloc(sizeof(*_tmp469));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp814=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp46A;_tmp46A.tag=1;({struct Cyc_Absyn_Exp*_tmp813=Cyc_Absyn_uint_exp(1,0);_tmp46A.f1=_tmp813;});_tmp46A;});_tmp469[0]=_tmp814;});_tmp469;});Cyc_Absyn_atb_typ(_tmp818,_tmp817,_tmp816,_tmp815,Cyc_Absyn_false_conref);});}case 9: _LL287: _LL288: {
# 2496
void*_tmp46B=({void*_tmp81B=Cyc_Absyn_wchar_typ();void*_tmp81A=rgn;struct Cyc_Absyn_Tqual _tmp819=Cyc_Absyn_const_tqual(0);Cyc_Absyn_atb_typ(_tmp81B,_tmp81A,_tmp819,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2498
void*_tmp46C=Cyc_Tcexp_tcExp(_tmp451,& _tmp46B,e1);
return({void*_tmp821=_tmp46C;void*_tmp820=rgn;struct Cyc_Absyn_Tqual _tmp81F=Cyc_Absyn_empty_tqual(0);void*_tmp81E=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp46D=_cycalloc(sizeof(*_tmp46D));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp81D=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp46E;_tmp46E.tag=1;({struct Cyc_Absyn_Exp*_tmp81C=Cyc_Absyn_uint_exp(1,0);_tmp46E.f1=_tmp81C;});_tmp46E;});_tmp46D[0]=_tmp81D;});_tmp46D;});Cyc_Absyn_atb_typ(_tmp821,_tmp820,_tmp81F,_tmp81E,Cyc_Absyn_false_conref);});}default: goto _LL289;}default: _LL289: _LL28A:
# 2505
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp46F=Cyc_Tcutil_compress(*topt);void*_tmp470=_tmp46F;void**_tmp472;struct Cyc_Absyn_Tqual _tmp471;switch(*((int*)_tmp470)){case 5: _LL291: _tmp472=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp470)->f1).elt_typ;_tmp471=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp470)->f1).elt_tq;_LL292:
# 2511
 topt2=_tmp472;goto _LL290;case 3: _LL293: _LL294:
# 2515
 bogus=*topt;
topt2=& bogus;
goto _LL290;default: _LL295: _LL296:
 goto _LL290;}_LL290:;}{
# 2521
void*telt=Cyc_Tcexp_tcExp(_tmp451,topt2,e1);
# 2523
struct _RegionHandle*_tmp473=Cyc_Core_heap_region;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp473,telt) && !
Cyc_Tcutil_is_noalias_path(_tmp473,e1))
({void*_tmp474=0;({unsigned int _tmp823=e1->loc;struct _dyneither_ptr _tmp822=({const char*_tmp475="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp475,sizeof(char),49);});Cyc_Tcutil_terr(_tmp823,_tmp822,_tag_dyneither(_tmp474,sizeof(void*),0));});});{
# 2528
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp476=_cycalloc(sizeof(*_tmp476));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp828=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp477;_tmp477.tag=5;({struct Cyc_Absyn_PtrInfo _tmp827=({struct Cyc_Absyn_PtrInfo _tmp478;_tmp478.elt_typ=telt;({struct Cyc_Absyn_Tqual _tmp824=
Cyc_Absyn_empty_tqual(0);_tmp478.elt_tq=_tmp824;});({struct Cyc_Absyn_PtrAtts _tmp826=({(_tmp478.ptr_atts).rgn=rgn;({union Cyc_Absyn_Constraint*_tmp825=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp478.ptr_atts).nullable=_tmp825;});(_tmp478.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp478.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp478.ptr_atts).ptrloc=0;_tmp478.ptr_atts;});_tmp478.ptr_atts=_tmp826;});_tmp478;});_tmp477.f1=_tmp827;});_tmp477;});_tmp476[0]=_tmp828;});_tmp476;});
# 2533
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp451,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp451,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2540
return res_typ;};};}}_LL27C:;};}
# 2547
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
({void*_tmp82B=({void*_tmp82A=({void*_tmp829=t;Cyc_Tcutil_promote_array(_tmp829,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp82A;});e->topt=_tmp82B;});
return t;}
# 2557
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2561
{struct _RegionHandle*_tmp47C=Cyc_Core_heap_region;
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp47C,t) && !Cyc_Tcutil_is_noalias_path(_tmp47C,e))
# 2567
({void*_tmp47D=0;({unsigned int _tmp82D=e->loc;struct _dyneither_ptr _tmp82C=({const char*_tmp47E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp47E,sizeof(char),49);});Cyc_Tcutil_terr(_tmp82D,_tmp82C,_tag_dyneither(_tmp47D,sizeof(void*),0));});});}{
# 2570
void*_tmp47F=e->r;void*_tmp480=_tmp47F;switch(*((int*)_tmp480)){case 25: _LL298: _LL299:
 goto _LL29B;case 26: _LL29A: _LL29B:
 goto _LL29D;case 27: _LL29C: _LL29D:
 goto _LL29F;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp480)->f1).String_c).tag){case 9: _LL29E: _LL29F:
 goto _LL2A1;case 8: _LL2A0: _LL2A1:
 return t;default: goto _LL2A2;}default: _LL2A2: _LL2A3:
# 2577
({void*_tmp82E=Cyc_Tcutil_compress(t);t=_tmp82E;});
if(Cyc_Tcutil_is_array(t)){
# 2580
({void*_tmp830=({void*_tmp82F=t;Cyc_Tcutil_promote_array(_tmp82F,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp830;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2583
return t;}_LL297:;};}
# 2594 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp481=e->r;void*_tmp482=_tmp481;struct Cyc_Absyn_Exp*_tmp4A7;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp482)->tag == 11){_LL2A5: _tmp4A7=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp482)->f1;_LL2A6:
# 2598
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp4A7);
({void*_tmp831=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp4A7->topt),0);_tmp4A7->topt=_tmp831;});
e->topt=_tmp4A7->topt;
goto _LL2A4;}else{_LL2A7: _LL2A8:
# 2604
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp832=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp832;});
# 2607
{void*_tmp483=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp484=_tmp483;void*_tmp4A6;struct Cyc_Absyn_Tqual _tmp4A5;void*_tmp4A4;union Cyc_Absyn_Constraint*_tmp4A3;union Cyc_Absyn_Constraint*_tmp4A2;union Cyc_Absyn_Constraint*_tmp4A1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->tag == 5){_LL2AA: _tmp4A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->f1).elt_typ;_tmp4A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->f1).elt_tq;_tmp4A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->f1).ptr_atts).rgn;_tmp4A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->f1).ptr_atts).nullable;_tmp4A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->f1).ptr_atts).bounds;_tmp4A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->f1).ptr_atts).zero_term;_LL2AB:
# 2609
{void*_tmp485=Cyc_Tcutil_compress(_tmp4A6);void*_tmp486=_tmp485;struct Cyc_List_List*_tmp4A0;void*_tmp49F;struct Cyc_Absyn_Tqual _tmp49E;void*_tmp49D;struct Cyc_List_List*_tmp49C;int _tmp49B;struct Cyc_Absyn_VarargInfo*_tmp49A;struct Cyc_List_List*_tmp499;struct Cyc_List_List*_tmp498;struct Cyc_Absyn_Exp*_tmp497;struct Cyc_List_List*_tmp496;struct Cyc_Absyn_Exp*_tmp495;struct Cyc_List_List*_tmp494;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->tag == 9){_LL2AF: _tmp4A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).tvars;_tmp49F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).effect;_tmp49E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).ret_tqual;_tmp49D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).ret_typ;_tmp49C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).args;_tmp49B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).c_varargs;_tmp49A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).cyc_varargs;_tmp499=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).rgn_po;_tmp498=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).attributes;_tmp497=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).requires_clause;_tmp496=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).requires_relns;_tmp495=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).ensures_clause;_tmp494=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp486)->f1).ensures_relns;_LL2B0:
# 2611
 if(_tmp4A0 != 0){
struct _RegionHandle*_tmp487=Cyc_Core_heap_region;
struct _tuple12 _tmp488=({struct _tuple12 _tmp493;({struct Cyc_List_List*_tmp833=Cyc_Tcenv_lookup_type_vars(te);_tmp493.f1=_tmp833;});_tmp493.f2=_tmp487;_tmp493;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmp487,Cyc_Tcutil_r_make_inst_var,& _tmp488,_tmp4A0);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2620
({struct Cyc_List_List*_tmp834=Cyc_Tcutil_rsubst_rgnpo(_tmp487,inst,_tmp499);_tmp499=_tmp834;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp499);{
void*ftyp=({struct _RegionHandle*_tmp838=_tmp487;struct Cyc_List_List*_tmp837=inst;Cyc_Tcutil_rsubstitute(_tmp838,_tmp837,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp490=_cycalloc(sizeof(*_tmp490));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp836=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp491;_tmp491.tag=9;({struct Cyc_Absyn_FnInfo _tmp835=({struct Cyc_Absyn_FnInfo _tmp492;_tmp492.tvars=0;_tmp492.effect=_tmp49F;_tmp492.ret_tqual=_tmp49E;_tmp492.ret_typ=_tmp49D;_tmp492.args=_tmp49C;_tmp492.c_varargs=_tmp49B;_tmp492.cyc_varargs=_tmp49A;_tmp492.rgn_po=0;_tmp492.attributes=_tmp498;_tmp492.requires_clause=_tmp497;_tmp492.requires_relns=_tmp496;_tmp492.ensures_clause=_tmp495;_tmp492.ensures_relns=_tmp494;_tmp492;});_tmp491.f1=_tmp835;});_tmp491;});_tmp490[0]=_tmp836;});_tmp490;}));});
# 2627
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp489=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp83B=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp48E;_tmp48E.tag=5;({struct Cyc_Absyn_PtrInfo _tmp83A=({struct Cyc_Absyn_PtrInfo _tmp48F;_tmp48F.elt_typ=ftyp;_tmp48F.elt_tq=_tmp4A5;({struct Cyc_Absyn_PtrAtts _tmp839=({(_tmp48F.ptr_atts).rgn=_tmp4A4;(_tmp48F.ptr_atts).nullable=_tmp4A3;(_tmp48F.ptr_atts).bounds=_tmp4A2;(_tmp48F.ptr_atts).zero_term=_tmp4A1;(_tmp48F.ptr_atts).ptrloc=0;_tmp48F.ptr_atts;});_tmp48F.ptr_atts=_tmp839;});_tmp48F;});_tmp48E.f1=_tmp83A;});_tmp48E;});_tmp48D[0]=_tmp83B;});_tmp48D;});
# 2629
struct Cyc_Absyn_Exp*_tmp48A=Cyc_Absyn_copy_exp(e);
({void*_tmp83D=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp48B=_cycalloc(sizeof(*_tmp48B));({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp83C=({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp48C;_tmp48C.tag=12;_tmp48C.f1=_tmp48A;_tmp48C.f2=ts;_tmp48C;});_tmp48B[0]=_tmp83C;});_tmp48B;});e->r=_tmp83D;});
e->topt=(void*)_tmp489;};}
# 2634
goto _LL2AE;}else{_LL2B1: _LL2B2:
 goto _LL2AE;}_LL2AE:;}
# 2637
goto _LL2A9;}else{_LL2AC: _LL2AD:
 goto _LL2A9;}_LL2A9:;}
# 2640
goto _LL2A4;}_LL2A4:;}
# 2642
return(void*)_check_null(e->topt);}
# 2650
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2652
struct Cyc_List_List*_tmp4A8=0;
# 2654
{void*_tmp4A9=fn_exp->r;void*_tmp4AA=_tmp4A9;struct Cyc_List_List*_tmp4BE;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AA)->tag == 9){_LL2B4: _tmp4BE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AA)->f2;_LL2B5:
# 2656
{void*_tmp4AB=e->r;void*_tmp4AC=_tmp4AB;struct Cyc_List_List*_tmp4BB;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AC)->tag == 9){_LL2B9: _tmp4BB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AC)->f2;_LL2BA: {
# 2658
struct Cyc_List_List*_tmp4AD=alias_arg_exps;
int _tmp4AE=0;
while(_tmp4AD != 0){
while(_tmp4AE != (int)_tmp4AD->hd){
if(_tmp4BE == 0)
({void*_tmp4AF=0;({struct _dyneither_ptr _tmp83F=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4B3;_tmp4B3.tag=1;_tmp4B3.f1=(unsigned long)((int)_tmp4AD->hd);({struct Cyc_Int_pa_PrintArg_struct _tmp4B2;_tmp4B2.tag=1;_tmp4B2.f1=(unsigned long)_tmp4AE;({void*_tmp4B0[2]={& _tmp4B2,& _tmp4B3};({struct _dyneither_ptr _tmp83E=({const char*_tmp4B1="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp4B1,sizeof(char),36);});Cyc_aprintf(_tmp83E,_tag_dyneither(_tmp4B0,sizeof(void*),2));});});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp83F,_tag_dyneither(_tmp4AF,sizeof(void*),0));});});
# 2665
++ _tmp4AE;
_tmp4BE=_tmp4BE->tl;
_tmp4BB=((struct Cyc_List_List*)_check_null(_tmp4BB))->tl;}{
# 2670
struct _tuple11 _tmp4B4=({struct Cyc_Absyn_Exp*_tmp840=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BE))->hd;Cyc_Tcutil_insert_alias(_tmp840,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4BB))->hd)->topt)));});struct _tuple11 _tmp4B5=_tmp4B4;struct Cyc_Absyn_Decl*_tmp4B8;struct Cyc_Absyn_Exp*_tmp4B7;_LL2BE: _tmp4B8=_tmp4B5.f1;_tmp4B7=_tmp4B5.f2;_LL2BF:;
_tmp4BE->hd=(void*)_tmp4B7;
({struct Cyc_List_List*_tmp841=({struct Cyc_List_List*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->hd=_tmp4B8;_tmp4B6->tl=_tmp4A8;_tmp4B6;});_tmp4A8=_tmp841;});
_tmp4AD=_tmp4AD->tl;};}
# 2675
goto _LL2B8;}}else{_LL2BB: _LL2BC:
({void*_tmp4B9=0;({struct _dyneither_ptr _tmp842=({const char*_tmp4BA="not a function call!";_tag_dyneither(_tmp4BA,sizeof(char),21);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp842,_tag_dyneither(_tmp4B9,sizeof(void*),0));});});}_LL2B8:;}
# 2678
goto _LL2B3;}else{_LL2B6: _LL2B7:
({void*_tmp4BC=0;({struct _dyneither_ptr _tmp843=({const char*_tmp4BD="not a function call!";_tag_dyneither(_tmp4BD,sizeof(char),21);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp843,_tag_dyneither(_tmp4BC,sizeof(void*),0));});});}_LL2B3:;}
# 2683
while(_tmp4A8 != 0){
struct Cyc_Absyn_Decl*_tmp4BF=(struct Cyc_Absyn_Decl*)_tmp4A8->hd;
({struct Cyc_Absyn_Exp*_tmp847=({struct Cyc_Absyn_Stmt*_tmp846=({struct Cyc_Absyn_Decl*_tmp845=_tmp4BF;struct Cyc_Absyn_Stmt*_tmp844=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp845,_tmp844,e->loc);});Cyc_Absyn_stmt_exp(_tmp846,e->loc);});fn_exp=_tmp847;});
_tmp4A8=_tmp4A8->tl;}
# 2690
e->topt=0;
e->r=fn_exp->r;}
# 2696
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2700
{void*_tmp4C0=e->r;void*_tmp4C1=_tmp4C0;void*_tmp52C;struct Cyc_Absyn_Exp*_tmp52B;struct _dyneither_ptr*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;struct Cyc_Absyn_Exp*_tmp528;int*_tmp527;struct Cyc_Absyn_Exp**_tmp526;void***_tmp525;struct Cyc_Absyn_Exp**_tmp524;int*_tmp523;void*_tmp522;struct Cyc_Absyn_Enumfield*_tmp521;struct Cyc_Absyn_Enumdecl*_tmp520;struct Cyc_Absyn_Enumfield*_tmp51F;struct Cyc_List_List*_tmp51E;struct Cyc_Absyn_Datatypedecl*_tmp51D;struct Cyc_Absyn_Datatypefield*_tmp51C;void*_tmp51B;struct Cyc_List_List*_tmp51A;struct _tuple1**_tmp519;struct Cyc_List_List**_tmp518;struct Cyc_List_List*_tmp517;struct Cyc_Absyn_Aggrdecl**_tmp516;struct Cyc_Absyn_Exp*_tmp515;void*_tmp514;int*_tmp513;struct Cyc_Absyn_Vardecl*_tmp512;struct Cyc_Absyn_Exp*_tmp511;struct Cyc_Absyn_Exp*_tmp510;int*_tmp50F;struct Cyc_Absyn_Stmt*_tmp50E;struct Cyc_List_List*_tmp50D;struct _tuple9*_tmp50C;struct Cyc_List_List*_tmp50B;struct Cyc_List_List*_tmp50A;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp*_tmp508;struct Cyc_Absyn_Exp*_tmp507;struct _dyneither_ptr*_tmp506;int*_tmp505;int*_tmp504;struct Cyc_Absyn_Exp*_tmp503;struct _dyneither_ptr*_tmp502;int*_tmp501;int*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;void*_tmp4FE;struct Cyc_List_List*_tmp4FD;void*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp**_tmp4FA;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4F8;void*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;enum Cyc_Absyn_Coercion*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_List_List*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Core_Opt*_tmp4E7;struct Cyc_Absyn_Exp*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;enum Cyc_Absyn_Incrementor _tmp4E4;enum Cyc_Absyn_Primop _tmp4E3;struct Cyc_List_List*_tmp4E2;void**_tmp4E1;union Cyc_Absyn_Cnst*_tmp4E0;struct Cyc_Core_Opt*_tmp4DF;struct Cyc_List_List*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_List_List*_tmp4DC;struct Cyc_Absyn_VarargCallInfo**_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_List_List*_tmp4D9;struct Cyc_Absyn_VarargCallInfo**_tmp4D8;int*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D6;switch(*((int*)_tmp4C1)){case 11: _LL2C1: _tmp4D6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2C2:
# 2705
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp4D6);
return;case 9: _LL2C3: _tmp4DA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4D9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp4D8=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_tmp4D7=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C1)->f4;if(!(*_tmp4D7)){_LL2C4:
# 2709
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));({struct Cyc_Core_Impossible_exn_struct _tmp849=({struct Cyc_Core_Impossible_exn_struct _tmp4C3;_tmp4C3.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp848=({const char*_tmp4C4="unresolved function in tcExpNoInst";_tag_dyneither(_tmp4C4,sizeof(char),35);});_tmp4C3.f1=_tmp848;});_tmp4C3;});_tmp4C2[0]=_tmp849;});_tmp4C2;}));}else{_LL2D9: _tmp4DD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4DC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp4DB=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_LL2DA: {
# 2740
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp4C5;_push_handler(& _tmp4C5);{int _tmp4C7=0;if(setjmp(_tmp4C5.handler))_tmp4C7=1;if(!_tmp4C7){
({struct Cyc_Absyn_Exp*_tmp84A=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp84A;});;_pop_handler();}else{void*_tmp4C6=(void*)_exn_thrown;void*_tmp4C8=_tmp4C6;void*_tmp4C9;if(((struct Cyc_Core_Failure_exn_struct*)_tmp4C8)->tag == Cyc_Core_Failure){_LL314: _LL315:
# 2747
 ok=0;
fn_exp=e;
goto _LL313;}else{_LL316: _tmp4C9=_tmp4C8;_LL317:(int)_rethrow(_tmp4C9);}_LL313:;}};}
# 2751
({void*_tmp84B=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4DD,_tmp4DC,_tmp4DB,& alias_arg_exps);t=_tmp84B;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp84C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp84C;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2758
goto _LL2C0;}}case 35: _LL2C5: _tmp4DF=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4DE=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2C6:
# 2713
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp4DE);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0: _LL2C7: _tmp4E0=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2C8:
# 2718
({void*_tmp84D=Cyc_Tcexp_tcConst(te,loc,topt,_tmp4E0,e);t=_tmp84D;});goto _LL2C0;case 1: _LL2C9: _tmp4E1=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2CA:
# 2720
({void*_tmp84E=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp4E1);t=_tmp84E;});goto _LL2C0;case 2: _LL2CB: _tmp4E3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4E2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2CC:
# 2722
({void*_tmp84F=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4E3,_tmp4E2);t=_tmp84F;});goto _LL2C0;case 4: _LL2CD: _tmp4E5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4E4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2CE:
# 2724
({void*_tmp850=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp4E5,_tmp4E4);t=_tmp850;});goto _LL2C0;case 3: _LL2CF: _tmp4E8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4E7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp4E6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_LL2D0:
# 2726
({void*_tmp851=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4E8,_tmp4E7,_tmp4E6);t=_tmp851;});goto _LL2C0;case 5: _LL2D1: _tmp4EB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4EA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp4E9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_LL2D2:
# 2728
({void*_tmp852=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp4EB,_tmp4EA,_tmp4E9);t=_tmp852;});goto _LL2C0;case 6: _LL2D3: _tmp4ED=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4EC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2D4:
# 2730
({void*_tmp853=Cyc_Tcexp_tcAnd(te,loc,_tmp4ED,_tmp4EC);t=_tmp853;});goto _LL2C0;case 7: _LL2D5: _tmp4EF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4EE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2D6:
# 2732
({void*_tmp854=Cyc_Tcexp_tcOr(te,loc,_tmp4EF,_tmp4EE);t=_tmp854;});goto _LL2C0;case 8: _LL2D7: _tmp4F1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4F0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2D8:
# 2734
({void*_tmp855=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4F1,_tmp4F0);t=_tmp855;});goto _LL2C0;case 10: _LL2DB: _tmp4F2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2DC:
# 2760
({void*_tmp856=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4F2);t=_tmp856;});goto _LL2C0;case 12: _LL2DD: _tmp4F4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4F3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2DE:
# 2762
({void*_tmp857=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4F4,_tmp4F3);t=_tmp857;});goto _LL2C0;case 13: _LL2DF: _tmp4F7=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4F6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp4F5=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4C1)->f4;_LL2E0:
# 2764
({void*_tmp858=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4F7,_tmp4F6,_tmp4F5);t=_tmp858;});goto _LL2C0;case 14: _LL2E1: _tmp4F8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2E2:
# 2766
({void*_tmp859=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4F8);t=_tmp859;});goto _LL2C0;case 15: _LL2E3: _tmp4FA=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4F9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2E4:
# 2768
({void*_tmp85A=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4FA,e,_tmp4F9);t=_tmp85A;});goto _LL2C0;case 17: _LL2E5: _tmp4FB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2E6: {
# 2770
void*_tmp4CA=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4FB);
({void*_tmp85B=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4CA);t=_tmp85B;});goto _LL2C0;}case 16: _LL2E7: _tmp4FC=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2E8:
# 2773
({void*_tmp85C=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4FC);t=_tmp85C;});goto _LL2C0;case 18: _LL2E9: _tmp4FE=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp4FD=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2EA:
# 2775
({void*_tmp85D=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4FE,_tmp4FD);t=_tmp85D;});goto _LL2C0;case 19: _LL2EB: _tmp4FF=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2EC:
# 2777
({void*_tmp85E=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4FF);t=_tmp85E;});goto _LL2C0;case 20: _LL2ED: _tmp503=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp502=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp501=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_tmp500=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4C1)->f4;_LL2EE:
# 2779
({void*_tmp85F=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp503,_tmp502,_tmp501,_tmp500);t=_tmp85F;});goto _LL2C0;case 21: _LL2EF: _tmp507=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp506=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp505=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_tmp504=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4C1)->f4;_LL2F0:
# 2781
({void*_tmp860=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp507,_tmp506,_tmp505,_tmp504);t=_tmp860;});goto _LL2C0;case 22: _LL2F1: _tmp509=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp508=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2F2:
# 2783
({void*_tmp861=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp509,_tmp508);t=_tmp861;});goto _LL2C0;case 23: _LL2F3: _tmp50A=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2F4:
# 2785
({void*_tmp862=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp50A);t=_tmp862;});goto _LL2C0;case 24: _LL2F5: _tmp50C=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp50B=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL2F6:
# 2787
({void*_tmp863=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp50C,_tmp50B);t=_tmp863;});goto _LL2C0;case 25: _LL2F7: _tmp50D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2F8: {
# 2791
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4CB=Cyc_Tcutil_compress(*topt);void*_tmp4CC=_tmp4CB;void**_tmp4CF;struct Cyc_Absyn_Tqual*_tmp4CE;union Cyc_Absyn_Constraint*_tmp4CD;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CC)->tag == 8){_LL319: _tmp4CF=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CC)->f1).elt_type;_tmp4CE=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CC)->f1).tq;_tmp4CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4CC)->f1).zero_term;_LL31A:
# 2797
 elt_topt=_tmp4CF;
elt_tqopt=_tmp4CE;
({int _tmp864=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4CD);zero_term=_tmp864;});
goto _LL318;}else{_LL31B: _LL31C:
 goto _LL318;}_LL318:;}
# 2804
({void*_tmp865=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp50D);t=_tmp865;});goto _LL2C0;}case 36: _LL2F9: _tmp50E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL2FA:
# 2806
({void*_tmp866=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp50E);t=_tmp866;});goto _LL2C0;case 26: _LL2FB: _tmp512=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp511=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp510=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_tmp50F=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4C1)->f4;_LL2FC:
# 2808
({void*_tmp867=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp512,_tmp511,_tmp510,_tmp50F);t=_tmp867;});goto _LL2C0;case 27: _LL2FD: _tmp515=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp514=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp513=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_LL2FE:
# 2810
({void*_tmp868=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp515,_tmp514,_tmp513);t=_tmp868;});goto _LL2C0;case 28: _LL2FF: _tmp519=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp518=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp517=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_tmp516=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4C1)->f4;_LL300:
# 2812
({void*_tmp869=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp519,_tmp518,_tmp517,_tmp516);t=_tmp869;});goto _LL2C0;case 29: _LL301: _tmp51B=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp51A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL302:
# 2814
({void*_tmp86A=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp51B,_tmp51A);t=_tmp86A;});goto _LL2C0;case 30: _LL303: _tmp51E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp51D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_tmp51C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4C1)->f3;_LL304:
# 2816
({void*_tmp86B=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp51E,_tmp51D,_tmp51C);t=_tmp86B;});goto _LL2C0;case 31: _LL305: _tmp520=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp51F=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL306:
# 2818
({void*_tmp86D=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp86C=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp4D1;_tmp4D1.tag=13;_tmp4D1.f1=_tmp520->name;_tmp4D1.f2=_tmp520;_tmp4D1;});_tmp4D0[0]=_tmp86C;});_tmp4D0;});t=_tmp86D;});goto _LL2C0;case 32: _LL307: _tmp522=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp521=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL308:
# 2820
 t=_tmp522;goto _LL2C0;case 33: _LL309: _tmp527=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1).is_calloc;_tmp526=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1).rgn;_tmp525=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1).elt_type;_tmp524=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1).num_elts;_tmp523=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1).fat_result;_LL30A:
# 2822
({void*_tmp86E=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp526,_tmp525,_tmp524,_tmp527,_tmp523);t=_tmp86E;});goto _LL2C0;case 34: _LL30B: _tmp529=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp528=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL30C:
# 2824
({void*_tmp86F=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp529,_tmp528);t=_tmp86F;});goto _LL2C0;case 37: _LL30D: _tmp52B=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_tmp52A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4C1)->f2;_LL30E:
# 2826
({void*_tmp870=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp52B,_tmp52A);t=_tmp870;});goto _LL2C0;case 38: _LL30F: _tmp52C=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4C1)->f1;_LL310:
# 2828
 if(!te->allow_valueof)
({void*_tmp4D2=0;({unsigned int _tmp872=e->loc;struct _dyneither_ptr _tmp871=({const char*_tmp4D3="valueof(-) can only occur within types";_tag_dyneither(_tmp4D3,sizeof(char),39);});Cyc_Tcutil_terr(_tmp872,_tmp871,_tag_dyneither(_tmp4D2,sizeof(void*),0));});});
# 2836
t=Cyc_Absyn_sint_typ;
goto _LL2C0;default: _LL311: _LL312:
# 2839
({void*_tmp4D4=0;({unsigned int _tmp874=e->loc;struct _dyneither_ptr _tmp873=({const char*_tmp4D5="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp4D5,sizeof(char),50);});Cyc_Tcutil_terr(_tmp874,_tmp873,_tag_dyneither(_tmp4D4,sizeof(void*),0));});});
({void*_tmp875=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp875;});}_LL2C0:;}
# 2842
e->topt=t;}
