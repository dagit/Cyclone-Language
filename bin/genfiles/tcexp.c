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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 113 "core.h"
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i);
# 265
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 924 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 930
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 933
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 938
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_dyneither_conref;
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 951
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_ushort_typ;extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 956
extern void*Cyc_Absyn_schar_typ;extern void*Cyc_Absyn_sshort_typ;extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slonglong_typ;
# 958
void*Cyc_Absyn_float_typ(int);void*Cyc_Absyn_wchar_typ();
# 964
void*Cyc_Absyn_exn_typ();
# 973
extern void*Cyc_Absyn_bounds_one;
# 978
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 981
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 984
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1056
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1074
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1078
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1150
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1152
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1157
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 74 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(enum Cyc_Tcenv_NewStatus,struct Cyc_Tcenv_Tenv*);
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*);
# 88
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct Cyc_Tcenv_Tenv*);
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 110
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 115
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 119
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_stmt_exp(struct Cyc_Tcenv_Tenv*);
# 126
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
# 133
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 137
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 139
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 144
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
# 89
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
# 92
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 97
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 113
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 139
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 141
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 145
void Cyc_Tcutil_explain_failure();
# 147
int Cyc_Tcutil_unify(void*,void*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 155
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 181
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 223 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 233
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 235
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 237
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 241
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 249
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 261
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 266
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 269
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 274
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple14{int f1;void*f2;};
# 283
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 310
int Cyc_Tcutil_bits_only(void*t);
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 342
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 345
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 358
int Cyc_Tcutil_is_array(void*t);
# 362
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 365
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
struct Cyc_List_List*Cyc_Formatstr_get_format_typs(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 33
struct Cyc_List_List*Cyc_Formatstr_get_scanf_typs(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 36
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 46 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 50
({void*_tmp0=0U;({unsigned int _tmp569=loc;struct _dyneither_ptr _tmp568=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp569,_tmp568,_tag_dyneither(_tmp0,sizeof(void*),0U));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 54
return*topt;}}
# 63
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 67
if(topt == 0){
# 69
({void*_tmp56A=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=26U,_tmp1->f1=des;_tmp1;});e->r=_tmp56A;});
return;}{
# 72
void*t=*topt;
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;void*_tmpE;struct Cyc_Absyn_Tqual _tmpD;union Cyc_Absyn_AggrInfoU _tmpC;switch(*((int*)_tmp3)){case 11U: _LL1: _tmpC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3)->f1).aggr_info;_LL2:
# 75
{union Cyc_Absyn_AggrInfoU _tmp4=_tmpC;struct Cyc_Absyn_Aggrdecl*_tmp8;if((_tmp4.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp5=0U;({struct _dyneither_ptr _tmp56B=({const char*_tmp6="struct type not properly set";_tag_dyneither(_tmp6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp56B,_tag_dyneither(_tmp5,sizeof(void*),0U));});});}else{_LLC: _tmp8=*(_tmp4.KnownAggr).val;_LLD:
({void*_tmp56C=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=29U,_tmp7->f1=_tmp8->name,_tmp7->f2=0,_tmp7->f3=des,_tmp7->f4=_tmp8;_tmp7;});e->r=_tmp56C;});}_LL9:;}
# 79
goto _LL0;case 8U: _LL3: _tmpE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).elt_type;_tmpD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->f1).tq;_LL4:
({void*_tmp56D=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=26U,_tmp9->f1=des;_tmp9;});e->r=_tmp56D;});goto _LL0;case 12U: _LL5: _LL6:
({void*_tmp56E=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpA=_cycalloc(sizeof(*_tmpA));_tmpA->tag=30U,_tmpA->f1=t,_tmpA->f2=des;_tmpA;});e->r=_tmp56E;});goto _LL0;default: _LL7: _LL8:
({void*_tmp56F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=26U,_tmpB->f1=des;_tmpB;});e->r=_tmp56F;});goto _LL0;}_LL0:;};}
# 89
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 93
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 99
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmpF=e->r;void*_tmp10=_tmpF;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp10)->f2 == 0){_LL1: _LL2:
({void*_tmp11=0U;({unsigned int _tmp571=e->loc;struct _dyneither_ptr _tmp570=({const char*_tmp12="assignment in test";_tag_dyneither(_tmp12,sizeof(char),19U);});Cyc_Tcutil_warn(_tmp571,_tmp570,_tag_dyneither(_tmp11,sizeof(void*),0U));});});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 107
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp4E0;_tmp4E0.tag=0U,_tmp4E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);_tmp4E0;});struct Cyc_String_pa_PrintArg_struct _tmp16=({struct Cyc_String_pa_PrintArg_struct _tmp4DF;_tmp4DF.tag=0U,({
struct _dyneither_ptr _tmp572=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4DF.f1=_tmp572;});_tmp4DF;});void*_tmp13[2U];_tmp13[0]=& _tmp15,_tmp13[1]=& _tmp16;({unsigned int _tmp574=e->loc;struct _dyneither_ptr _tmp573=({const char*_tmp14="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp14,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp574,_tmp573,_tag_dyneither(_tmp13,sizeof(void*),2U));});});
# 116
{void*_tmp17=e->r;void*_tmp18=_tmp17;enum Cyc_Absyn_Primop _tmp1F;struct Cyc_List_List*_tmp1E;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->tag == 3U){_LL1: _tmp1F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->f1;_tmp1E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp18)->f2;_LL2:
# 118
 if(_tmp1F == Cyc_Absyn_Eq  || _tmp1F == Cyc_Absyn_Neq){
struct _tuple0 _tmp19=({struct _tuple0 _tmp4E2;_tmp4E2.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E))->hd)->topt),_tmp4E2.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp1E->tl))->hd)->topt);_tmp4E2;});struct _tuple0 _tmp1A=_tmp19;void*_tmp1D;void*_tmp1C;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f2)->tag == 15U){_LL6: _tmp1D=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f1)->f1;_tmp1C=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1A.f2)->f1;_LL7:
# 121
 return({struct Cyc_Tcexp_TestEnv _tmp4E1;({struct _tuple0*_tmp575=({struct _tuple0*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->f1=_tmp1D,_tmp1B->f2=_tmp1C;_tmp1B;});_tmp4E1.eq=_tmp575;}),_tmp4E1.isTrue=_tmp1F == Cyc_Absyn_Eq;_tmp4E1;});}else{goto _LL8;}}else{_LL8: _LL9:
# 123
 goto _LL5;}_LL5:;}
# 126
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 129
return({struct Cyc_Tcexp_TestEnv _tmp4E3;_tmp4E3.eq=0,_tmp4E3.isTrue=0;_tmp4E3;});}
# 150 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 155
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp20=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp21=_tmp20;struct _dyneither_ptr _tmp53;struct _dyneither_ptr _tmp52;enum Cyc_Absyn_Sign _tmp51;int _tmp50;int _tmp4F;enum Cyc_Absyn_Sign _tmp4E;enum Cyc_Absyn_Sign _tmp4D;switch((_tmp21.Wstring_c).tag){case 2U: switch(((_tmp21.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 t=Cyc_Absyn_schar_typ;goto _LL0;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 t=Cyc_Absyn_uchar_typ;goto _LL0;default: _LL5: _LL6:
 t=Cyc_Absyn_char_typ;goto _LL0;}case 3U: _LL7: _LL8:
({void*_tmp576=Cyc_Absyn_wchar_typ();t=_tmp576;});goto _LL0;case 4U: _LL9: _tmp4D=((_tmp21.Short_c).val).f1;_LLA:
# 165
 t=_tmp4D == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL0;case 6U: _LLB: _tmp4E=((_tmp21.LongLong_c).val).f1;_LLC:
# 167
 t=_tmp4E == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL0;case 7U: _LLD: _tmp4F=((_tmp21.Float_c).val).f2;_LLE:
({void*_tmp577=Cyc_Absyn_float_typ(_tmp4F);t=_tmp577;});goto _LL0;case 5U: _LLF: _tmp51=((_tmp21.Int_c).val).f1;_tmp50=((_tmp21.Int_c).val).f2;_LL10:
# 170
 if(topt == 0)
t=_tmp51 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 177
void*_tmp22=Cyc_Tcutil_compress(*topt);void*_tmp23=_tmp22;void*_tmp37;void*_tmp36;struct Cyc_Absyn_Tqual _tmp35;void*_tmp34;union Cyc_Absyn_Constraint*_tmp33;union Cyc_Absyn_Constraint*_tmp32;union Cyc_Absyn_Constraint*_tmp31;enum Cyc_Absyn_Sign _tmp30;enum Cyc_Absyn_Sign _tmp2F;enum Cyc_Absyn_Sign _tmp2E;enum Cyc_Absyn_Sign _tmp2D;switch(*((int*)_tmp23)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f2){case Cyc_Absyn_Char_sz: _LL18: _tmp2D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f1;_LL19:
# 179
{enum Cyc_Absyn_Sign _tmp24=_tmp2D;switch(_tmp24){case Cyc_Absyn_Unsigned: _LL27: _LL28:
 t=Cyc_Absyn_uchar_typ;goto _LL26;case Cyc_Absyn_Signed: _LL29: _LL2A:
 t=Cyc_Absyn_schar_typ;goto _LL26;default: _LL2B: _LL2C:
 t=Cyc_Absyn_char_typ;goto _LL26;}_LL26:;}
# 184
({union Cyc_Absyn_Cnst _tmp578=Cyc_Absyn_Char_c(_tmp2D,(char)_tmp50);*c=_tmp578;});
goto _LL17;case Cyc_Absyn_Short_sz: _LL1A: _tmp2E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f1;_LL1B:
# 187
 t=_tmp2E == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
({union Cyc_Absyn_Cnst _tmp579=Cyc_Absyn_Short_c(_tmp2E,(short)_tmp50);*c=_tmp579;});
goto _LL17;case Cyc_Absyn_Int_sz: _LL1C: _tmp2F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f1;_LL1D:
# 191
 t=_tmp2F == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 194
({union Cyc_Absyn_Cnst _tmp57A=Cyc_Absyn_Int_c(_tmp2F,_tmp50);*c=_tmp57A;});
goto _LL17;case Cyc_Absyn_Long_sz: _LL1E: _tmp30=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp23)->f1;_LL1F:
# 197
 t=_tmp30 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 200
({union Cyc_Absyn_Cnst _tmp57B=Cyc_Absyn_Int_c(_tmp30,_tmp50);*c=_tmp57B;});
goto _LL17;default: goto _LL24;}case 5U: _LL20: _tmp36=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).elt_typ;_tmp35=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).elt_tq;_tmp34=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).rgn;_tmp33=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).nullable;_tmp32=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).bounds;_tmp31=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23)->f1).ptr_atts).zero_term;if(_tmp50 == 0){_LL21: {
# 204
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp33))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp4E4;_tmp4E4.tag=0U,({
struct _dyneither_ptr _tmp57C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4E4.f1=_tmp57C;});_tmp4E4;});void*_tmp25[1U];_tmp25[0]=& _tmp27;({unsigned int _tmp57E=e->loc;struct _dyneither_ptr _tmp57D=({const char*_tmp26="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp26,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp57E,_tmp57D,_tag_dyneither(_tmp25,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp28=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp57F=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=5U,(_tmp29->f1).elt_typ=_tmp36,(_tmp29->f1).elt_tq=_tmp35,
((_tmp29->f1).ptr_atts).rgn=_tmp34,((_tmp29->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp29->f1).ptr_atts).bounds=_tmp32,((_tmp29->f1).ptr_atts).zero_term=_tmp31,((_tmp29->f1).ptr_atts).ptrloc=0;_tmp29;});
# 210
t=_tmp57F;});
# 212
goto _LL17;};}}else{goto _LL24;}case 19U: _LL22: _tmp37=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp23)->f1;_LL23: {
# 214
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2A=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp2C=_cycalloc(sizeof(*_tmp2C));_tmp2C->tag=18U,({struct Cyc_Absyn_Exp*_tmp580=Cyc_Absyn_uint_exp((unsigned int)_tmp50,0U);_tmp2C->f1=_tmp580;});_tmp2C;});
# 221
({void*_tmp581=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=19U,_tmp2B->f1=(void*)_tmp2A;_tmp2B;});t=_tmp581;});
goto _LL17;}default: _LL24: _LL25:
# 224
 t=_tmp51 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL17;}_LL17:;}
# 227
goto _LL0;case 8U: _LL11: _tmp52=(_tmp21.String_c).val;_LL12:
# 229
 string_numelts=(int)_get_dyneither_size(_tmp52,sizeof(char));
_tmp53=_tmp52;goto _LL14;case 9U: _LL13: _tmp53=(_tmp21.Wstring_c).val;_LL14:
# 232
 if(string_numelts == 0){
({int _tmp582=Cyc_Tcexp_wchar_numelts(_tmp53);string_numelts=_tmp582;});
({void*_tmp583=Cyc_Absyn_wchar_typ();string_elt_typ=_tmp583;});}{
# 236
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp584=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp584,loc);});
elen->topt=Cyc_Absyn_uint_typ;{
# 241
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp38=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C->tag=1U,_tmp3C->f1=elen;_tmp3C;});
({void*_tmp588=({void*_tmp587=string_elt_typ;struct Cyc_Absyn_Tqual _tmp586=Cyc_Absyn_const_tqual(0U);void*_tmp585=(void*)_tmp38;Cyc_Absyn_atb_typ(_tmp587,(void*)& Cyc_Absyn_HeapRgn_val,_tmp586,_tmp585,Cyc_Absyn_true_conref);});t=_tmp588;});
# 244
if(topt != 0){
void*_tmp39=Cyc_Tcutil_compress(*topt);void*_tmp3A=_tmp39;struct Cyc_Absyn_Tqual _tmp3B;switch(*((int*)_tmp3A)){case 8U: _LL2E: _tmp3B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A)->f1).tq;_LL2F:
# 249
 return({void*_tmp58B=string_elt_typ;struct Cyc_Absyn_Tqual _tmp58A=_tmp3B;struct Cyc_Absyn_Exp*_tmp589=elen;Cyc_Absyn_array_typ(_tmp58B,_tmp58A,_tmp589,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0U);});case 5U: _LL30: _LL31:
# 251
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 257
({void*_tmp590=({void*_tmp58F=string_elt_typ;void*_tmp58E=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp58D=
Cyc_Absyn_const_tqual(0U);
# 257
void*_tmp58C=(void*)_tmp38;Cyc_Absyn_atb_typ(_tmp58F,_tmp58E,_tmp58D,_tmp58C,Cyc_Absyn_true_conref);});t=_tmp590;});
# 259
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 265
goto _LL2D;default: _LL32: _LL33:
 goto _LL2D;}_LL2D:;}
# 269
return t;};};default: _LL15: _LL16:
# 271
 if(topt != 0){
void*_tmp3D=Cyc_Tcutil_compress(*topt);void*_tmp3E=_tmp3D;void*_tmp48;struct Cyc_Absyn_Tqual _tmp47;void*_tmp46;union Cyc_Absyn_Constraint*_tmp45;union Cyc_Absyn_Constraint*_tmp44;union Cyc_Absyn_Constraint*_tmp43;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->tag == 5U){_LL35: _tmp48=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).elt_typ;_tmp47=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).elt_tq;_tmp46=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).ptr_atts).rgn;_tmp45=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).ptr_atts).nullable;_tmp44=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).ptr_atts).bounds;_tmp43=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3E)->f1).ptr_atts).zero_term;_LL36:
# 275
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp45))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp41=({struct Cyc_String_pa_PrintArg_struct _tmp4E5;_tmp4E5.tag=0U,({
struct _dyneither_ptr _tmp591=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(*topt));_tmp4E5.f1=_tmp591;});_tmp4E5;});void*_tmp3F[1U];_tmp3F[0]=& _tmp41;({unsigned int _tmp593=e->loc;struct _dyneither_ptr _tmp592=({const char*_tmp40="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp40,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp593,_tmp592,_tag_dyneither(_tmp3F,sizeof(void*),1U));});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->tag=5U,(_tmp42->f1).elt_typ=_tmp48,(_tmp42->f1).elt_tq=_tmp47,((_tmp42->f1).ptr_atts).rgn=_tmp46,((_tmp42->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp42->f1).ptr_atts).bounds=_tmp44,((_tmp42->f1).ptr_atts).zero_term=_tmp43,((_tmp42->f1).ptr_atts).ptrloc=0;_tmp42;});}else{_LL37: _LL38:
# 280
 goto _LL34;}_LL34:;}{
# 283
struct Cyc_List_List*_tmp49=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp599=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->tag=5U,({void*_tmp598=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->v=_tmp49;_tmp4A;}));(_tmp4C->f1).elt_typ=_tmp598;}),({
struct Cyc_Absyn_Tqual _tmp597=Cyc_Absyn_empty_tqual(0U);(_tmp4C->f1).elt_tq=_tmp597;}),
({void*_tmp596=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->v=_tmp49;_tmp4B;}));((_tmp4C->f1).ptr_atts).rgn=_tmp596;}),((_tmp4C->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,({
# 288
union Cyc_Absyn_Constraint*_tmp595=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp4C->f1).ptr_atts).bounds=_tmp595;}),({union Cyc_Absyn_Constraint*_tmp594=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp4C->f1).ptr_atts).zero_term=_tmp594;}),((_tmp4C->f1).ptr_atts).ptrloc=0;_tmp4C;});
# 284
t=_tmp599;});
# 290
goto _LL0;};}_LL0:;}
# 292
return t;}
# 296
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 301
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp54=*((void**)_check_null(b));void*_tmp55=_tmp54;struct Cyc_Absyn_Vardecl*_tmp61;struct Cyc_Absyn_Vardecl*_tmp60;struct Cyc_Absyn_Vardecl*_tmp5F;struct Cyc_Absyn_Fndecl*_tmp5E;struct Cyc_Absyn_Vardecl*_tmp5D;struct _tuple1*_tmp5C;switch(*((int*)_tmp55)){case 0U: _LL1: _tmp5C=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp55)->f1;_LL2:
# 304
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp59A=({const char*_tmp56="unresolved binding in tcVar";_tag_dyneither(_tmp56,sizeof(char),28U);});_tmp57->f1=_tmp59A;});_tmp57;}));case 1U: _LL3: _tmp5D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp55)->f1;_LL4:
# 308
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp5D->name,1);
return _tmp5D->type;case 2U: _LL5: _tmp5E=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp55)->f1;_LL6:
# 315
 if(_tmp5E->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp5E->name,1);
return Cyc_Tcutil_fndecl2typ(_tmp5E);case 5U: _LL7: _tmp5F=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp55)->f1;_LL8:
 _tmp60=_tmp5F;goto _LLA;case 4U: _LL9: _tmp60=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp55)->f1;_LLA:
 _tmp61=_tmp60;goto _LLC;default: _LLB: _tmp61=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp55)->f1;_LLC:
# 321
 if(te->allow_valueof){
void*_tmp58=Cyc_Tcutil_compress(_tmp61->type);void*_tmp59=_tmp58;void*_tmp5B;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59)->tag == 19U){_LLE: _tmp5B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp59)->f1;_LLF:
# 324
({void*_tmp59B=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=39U,_tmp5A->f1=_tmp5B;_tmp5A;});e->r=_tmp59B;});
goto _LLD;}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 329
return _tmp61->type;}_LL0:;}
# 333
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 340
struct Cyc_List_List*desc_types;
{void*_tmp62=fmt->r;void*_tmp63=_tmp62;struct _dyneither_ptr _tmp68;struct _dyneither_ptr _tmp67;switch(*((int*)_tmp63)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp63)->f1).String_c).tag == 8){_LL1: _tmp67=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp63)->f1).String_c).val;_LL2:
 _tmp68=_tmp67;goto _LL4;}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp63)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp63)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp68=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp63)->f2)->r)->f1).String_c).val;_LL4:
# 344
({struct Cyc_List_List*_tmp59C=type_getter(te,(struct _dyneither_ptr)_tmp68,fmt->loc);desc_types=_tmp59C;});goto _LL0;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 348
 if(opt_args != 0){
struct Cyc_List_List*_tmp64=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp64 != 0;_tmp64=_tmp64->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp64->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp64->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp64->hd))
({void*_tmp65=0U;({unsigned int _tmp59E=((struct Cyc_Absyn_Exp*)_tmp64->hd)->loc;struct _dyneither_ptr _tmp59D=({const char*_tmp66="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp66,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp59E,_tmp59D,_tag_dyneither(_tmp65,sizeof(void*),0U));});});}}
# 357
return;}_LL0:;}
# 359
if(opt_args != 0){
struct Cyc_List_List*_tmp69=(struct Cyc_List_List*)opt_args->v;
# 362
for(0;desc_types != 0  && _tmp69 != 0;(
desc_types=desc_types->tl,_tmp69=_tmp69->tl,arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp69->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp4E7;_tmp4E7.tag=0U,({
struct _dyneither_ptr _tmp59F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4E7.f1=_tmp59F;});_tmp4E7;});struct Cyc_String_pa_PrintArg_struct _tmp6D=({struct Cyc_String_pa_PrintArg_struct _tmp4E6;_tmp4E6.tag=0U,({struct _dyneither_ptr _tmp5A0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp4E6.f1=_tmp5A0;});_tmp4E6;});void*_tmp6A[2U];_tmp6A[0]=& _tmp6C,_tmp6A[1]=& _tmp6D;({unsigned int _tmp5A2=e->loc;struct _dyneither_ptr _tmp5A1=({const char*_tmp6B="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp6B,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp5A2,_tmp5A1,_tag_dyneither(_tmp6A,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 373
if(alias_coercion)
({struct Cyc_List_List*_tmp5A3=({struct Cyc_List_List*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->hd=(void*)arg_cnt,_tmp6E->tl=*alias_arg_exps;_tmp6E;});*alias_arg_exps=_tmp5A3;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp6F=0U;({unsigned int _tmp5A5=((struct Cyc_Absyn_Exp*)_tmp69->hd)->loc;struct _dyneither_ptr _tmp5A4=({const char*_tmp70="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp70,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp5A5,_tmp5A4,_tag_dyneither(_tmp6F,sizeof(void*),0U));});});}
# 380
if(desc_types != 0)
({void*_tmp71=0U;({unsigned int _tmp5A7=fmt->loc;struct _dyneither_ptr _tmp5A6=({const char*_tmp72="too few arguments";_tag_dyneither(_tmp72,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp5A7,_tmp5A6,_tag_dyneither(_tmp71,sizeof(void*),0U));});});
if(_tmp69 != 0){
({void*_tmp73=0U;({unsigned int _tmp5A9=((struct Cyc_Absyn_Exp*)_tmp69->hd)->loc;struct _dyneither_ptr _tmp5A8=({const char*_tmp74="too many arguments";_tag_dyneither(_tmp74,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp5A9,_tmp5A8,_tag_dyneither(_tmp73,sizeof(void*),0U));});});
# 385
for(0;_tmp69 != 0;_tmp69=_tmp69->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp69->hd);}}}}
# 390
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 392
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp75=p;switch(_tmp75){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 396
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp78=({struct Cyc_String_pa_PrintArg_struct _tmp4E8;_tmp4E8.tag=0U,({struct _dyneither_ptr _tmp5AA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4E8.f1=_tmp5AA;});_tmp4E8;});void*_tmp76[1U];_tmp76[0]=& _tmp78;({unsigned int _tmp5AC=loc;struct _dyneither_ptr _tmp5AB=({const char*_tmp77="expecting numeric type but found %s";_tag_dyneither(_tmp77,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5AC,_tmp5AB,_tag_dyneither(_tmp76,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 400
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp7B=({struct Cyc_String_pa_PrintArg_struct _tmp4E9;_tmp4E9.tag=0U,({struct _dyneither_ptr _tmp5AD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4E9.f1=_tmp5AD;});_tmp4E9;});void*_tmp79[1U];_tmp79[0]=& _tmp7B;({unsigned int _tmp5AF=loc;struct _dyneither_ptr _tmp5AE=({const char*_tmp7A="expecting integral or * type but found %s";_tag_dyneither(_tmp7A,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp5AF,_tmp5AE,_tag_dyneither(_tmp79,sizeof(void*),1U));});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 405
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp7E=({struct Cyc_String_pa_PrintArg_struct _tmp4EA;_tmp4EA.tag=0U,({struct _dyneither_ptr _tmp5B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4EA.f1=_tmp5B0;});_tmp4EA;});void*_tmp7C[1U];_tmp7C[0]=& _tmp7E;({unsigned int _tmp5B2=loc;struct _dyneither_ptr _tmp5B1=({const char*_tmp7D="expecting integral type but found %s";_tag_dyneither(_tmp7D,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp5B2,_tmp5B1,_tag_dyneither(_tmp7C,sizeof(void*),1U));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 409
{void*_tmp7F=t;union Cyc_Absyn_Constraint*_tmp88;switch(*((int*)_tmp7F)){case 8U: _LLE: _LLF:
 goto _LLD;case 5U: _LL10: _tmp88=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7F)->f1).ptr_atts).bounds;_LL11:
# 412
{void*_tmp80=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp88);void*_tmp81=_tmp80;struct Cyc_Absyn_Exp*_tmp84;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp81)->tag == 0U){_LL15: _LL16:
 goto _LL14;}else{_LL17: _tmp84=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp81)->f1;_LL18:
# 415
 if(!Cyc_Evexp_c_can_eval(_tmp84) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp82=0U;({unsigned int _tmp5B4=loc;struct _dyneither_ptr _tmp5B3=({const char*_tmp83="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp83,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp5B4,_tmp5B3,_tag_dyneither(_tmp82,sizeof(void*),0U));});});
goto _LL14;}_LL14:;}
# 419
goto _LLD;default: _LL12: _LL13:
# 421
({struct Cyc_String_pa_PrintArg_struct _tmp87=({struct Cyc_String_pa_PrintArg_struct _tmp4EB;_tmp4EB.tag=0U,({struct _dyneither_ptr _tmp5B5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4EB.f1=_tmp5B5;});_tmp4EB;});void*_tmp85[1U];_tmp85[0]=& _tmp87;({unsigned int _tmp5B7=loc;struct _dyneither_ptr _tmp5B6=({const char*_tmp86="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp86,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp5B7,_tmp5B6,_tag_dyneither(_tmp85,sizeof(void*),1U));});});}_LLD:;}
# 423
return Cyc_Absyn_uint_typ;default: _LLB: _LLC:
({void*_tmp89=0U;({struct _dyneither_ptr _tmp5B8=({const char*_tmp8A="Non-unary primop";_tag_dyneither(_tmp8A,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5B8,_tag_dyneither(_tmp89,sizeof(void*),0U));});});}_LL0:;}
# 429
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 432
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp8D=({struct Cyc_String_pa_PrintArg_struct _tmp4EC;_tmp4EC.tag=0U,({struct _dyneither_ptr _tmp5B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp4EC.f1=_tmp5B9;});_tmp4EC;});void*_tmp8B[1U];_tmp8B[0]=& _tmp8D;({unsigned int _tmp5BB=e1->loc;struct _dyneither_ptr _tmp5BA=({const char*_tmp8C="type %s cannot be used here";_tag_dyneither(_tmp8C,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5BB,_tmp5BA,_tag_dyneither(_tmp8B,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 436
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp90=({struct Cyc_String_pa_PrintArg_struct _tmp4ED;_tmp4ED.tag=0U,({struct _dyneither_ptr _tmp5BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp4ED.f1=_tmp5BC;});_tmp4ED;});void*_tmp8E[1U];_tmp8E[0]=& _tmp90;({unsigned int _tmp5BE=e2->loc;struct _dyneither_ptr _tmp5BD=({const char*_tmp8F="type %s cannot be used here";_tag_dyneither(_tmp8F,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp5BE,_tmp5BD,_tag_dyneither(_tmp8E,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 440
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 445
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp91=t1;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;union Cyc_Absyn_Constraint*_tmpA2;union Cyc_Absyn_Constraint*_tmpA1;union Cyc_Absyn_Constraint*_tmpA0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->tag == 5U){_LL1: _tmpA5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).elt_typ;_tmpA4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).elt_tq;_tmpA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).rgn;_tmpA2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).nullable;_tmpA1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).bounds;_tmpA0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).ptr_atts).zero_term;_LL2:
# 450
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpA5),& Cyc_Tcutil_tmk))
({void*_tmp92=0U;({unsigned int _tmp5C0=e1->loc;struct _dyneither_ptr _tmp5BF=({const char*_tmp93="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp93,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5C0,_tmp5BF,_tag_dyneither(_tmp92,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmp94=0U;({unsigned int _tmp5C2=e1->loc;struct _dyneither_ptr _tmp5C1=({const char*_tmp95="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp95,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5C2,_tmp5C1,_tag_dyneither(_tmp94,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmp98=({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U,({struct _dyneither_ptr _tmp5C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4EE.f1=_tmp5C3;});_tmp4EE;});void*_tmp96[1U];_tmp96[0]=& _tmp98;({unsigned int _tmp5C5=e2->loc;struct _dyneither_ptr _tmp5C4=({const char*_tmp97="expecting int but found %s";_tag_dyneither(_tmp97,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp5C5,_tmp5C4,_tag_dyneither(_tmp96,sizeof(void*),1U));});});{
void*_tmp99=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpA1);void*_tmp9A=_tmp99;struct Cyc_Absyn_Exp*_tmp9F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9A)->tag == 0U){_LL6: _LL7:
 return t1;}else{_LL8: _tmp9F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp9A)->f1;_LL9:
# 461
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA0))
({void*_tmp9B=0U;({unsigned int _tmp5C7=e1->loc;struct _dyneither_ptr _tmp5C6=({const char*_tmp9C="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmp9C,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp5C7,_tmp5C6,_tag_dyneither(_tmp9B,sizeof(void*),0U));});});{
# 469
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9D=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=5U,(_tmp9E->f1).elt_typ=_tmpA5,(_tmp9E->f1).elt_tq=_tmpA4,
((_tmp9E->f1).ptr_atts).rgn=_tmpA3,((_tmp9E->f1).ptr_atts).nullable=Cyc_Absyn_true_conref,((_tmp9E->f1).ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref,((_tmp9E->f1).ptr_atts).zero_term=_tmpA0,((_tmp9E->f1).ptr_atts).ptrloc=0;_tmp9E;});
# 473
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmp9D,Cyc_Absyn_Other_coercion);
return(void*)_tmp9D;};}_LL5:;};}else{_LL3: _LL4:
# 476
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL0:;}
# 481
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpA8=({struct Cyc_String_pa_PrintArg_struct _tmp4F0;_tmp4F0.tag=0U,({
# 491
struct _dyneither_ptr _tmp5C8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F0.f1=_tmp5C8;});_tmp4F0;});struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=0U,({struct _dyneither_ptr _tmp5C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4EF.f1=_tmp5C9;});_tmp4EF;});void*_tmpA6[2U];_tmpA6[0]=& _tmpA8,_tmpA6[1]=& _tmpA9;({unsigned int _tmp5CB=e1->loc;struct _dyneither_ptr _tmp5CA=({const char*_tmpA7="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpA7,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5CB,_tmp5CA,_tag_dyneither(_tmpA6,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 494
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp5CC=t1_elt;Cyc_Tcutil_unify(_tmp5CC,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpAC=({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0U,({
# 499
struct _dyneither_ptr _tmp5CD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F2.f1=_tmp5CD;});_tmp4F2;});struct Cyc_String_pa_PrintArg_struct _tmpAD=({struct Cyc_String_pa_PrintArg_struct _tmp4F1;_tmp4F1.tag=0U,({struct _dyneither_ptr _tmp5CE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F1.f1=_tmp5CE;});_tmp4F1;});void*_tmpAA[2U];_tmpAA[0]=& _tmpAC,_tmpAA[1]=& _tmpAD;({unsigned int _tmp5D0=e1->loc;struct _dyneither_ptr _tmp5CF=({const char*_tmpAB="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpAB,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp5D0,_tmp5CF,_tag_dyneither(_tmpAA,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 503
({void*_tmpAE=0U;({unsigned int _tmp5D2=e1->loc;struct _dyneither_ptr _tmp5D1=({const char*_tmpAF="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpAF,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5D2,_tmp5D1,_tag_dyneither(_tmpAE,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5D7=te;struct Cyc_Absyn_Exp*_tmp5D6=e1;Cyc_Tcutil_unchecked_cast(_tmp5D7,_tmp5D6,({void*_tmp5D5=t1_elt;void*_tmp5D4=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp5D3=
Cyc_Absyn_empty_tqual(0U);
# 504
Cyc_Absyn_star_typ(_tmp5D5,_tmp5D4,_tmp5D3,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});
# 507
return Cyc_Absyn_sint_typ;}else{
# 509
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpB0=0U;({unsigned int _tmp5D9=e1->loc;struct _dyneither_ptr _tmp5D8=({const char*_tmpB1="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpB1,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5D9,_tmp5D8,_tag_dyneither(_tmpB0,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpB2=0U;({unsigned int _tmp5DB=e1->loc;struct _dyneither_ptr _tmp5DA=({const char*_tmpB3="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpB3,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5DB,_tmp5DA,_tag_dyneither(_tmpB2,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpB6=({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U,({
struct _dyneither_ptr _tmp5DC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F4.f1=_tmp5DC;});_tmp4F4;});struct Cyc_String_pa_PrintArg_struct _tmpB7=({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U,({struct _dyneither_ptr _tmp5DD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F3.f1=_tmp5DD;});_tmp4F3;});void*_tmpB4[2U];_tmpB4[0]=& _tmpB6,_tmpB4[1]=& _tmpB7;({unsigned int _tmp5DF=e2->loc;struct _dyneither_ptr _tmp5DE=({const char*_tmpB5="expecting either %s or int but found %s";_tag_dyneither(_tmpB5,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp5DF,_tmp5DE,_tag_dyneither(_tmpB4,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 518
return t1;}}}
# 522
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5E0=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5E0,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpB8=0U;({unsigned int _tmp5E2=e1->loc;struct _dyneither_ptr _tmp5E1=({const char*_tmpB9="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpB9,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5E2,_tmp5E1,_tag_dyneither(_tmpB8,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5E7=te;struct Cyc_Absyn_Exp*_tmp5E6=e2;Cyc_Tcutil_unchecked_cast(_tmp5E7,_tmp5E6,({void*_tmp5E5=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5E4=
Cyc_Tcutil_pointer_region(t2);
# 527
struct Cyc_Absyn_Tqual _tmp5E3=
# 529
Cyc_Absyn_empty_tqual(0U);
# 527
Cyc_Absyn_star_typ(_tmp5E5,_tmp5E4,_tmp5E3,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});}
# 532
({void*_tmpBA=0U;({unsigned int _tmp5E9=e1->loc;struct _dyneither_ptr _tmp5E8=({const char*_tmpBB="thin pointer subtraction!";_tag_dyneither(_tmpBB,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp5E9,_tmp5E8,_tag_dyneither(_tmpBA,sizeof(void*),0U));});});
return Cyc_Absyn_sint_typ;}
# 535
({void*_tmpBC=0U;({unsigned int _tmp5EB=e1->loc;struct _dyneither_ptr _tmp5EA=({const char*_tmpBD="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpBD,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp5EB,_tmp5EA,_tag_dyneither(_tmpBC,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 538
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpBE=0U;({unsigned int _tmp5ED=e1->loc;struct _dyneither_ptr _tmp5EC=({const char*_tmpBF="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpBF,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp5ED,_tmp5EC,_tag_dyneither(_tmpBE,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 543
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 546
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 554
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || ({
void*_tmp5EE=t1;Cyc_Tcutil_unify(_tmp5EE,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 559
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 572
pointer_cmp: {
struct _tuple0 _tmpC0=({struct _tuple0 _tmp4F5;({void*_tmp5F0=Cyc_Tcutil_compress(t1);_tmp4F5.f1=_tmp5F0;}),({void*_tmp5EF=Cyc_Tcutil_compress(t2);_tmp4F5.f2=_tmp5EF;});_tmp4F5;});struct _tuple0 _tmpC1=_tmpC0;void*_tmpC3;void*_tmpC2;switch(*((int*)_tmpC1.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC1.f2)->tag == 5U){_LL1: _tmpC3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC1.f1)->f1).elt_typ;_tmpC2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpC1.f2)->f1).elt_typ;_LL2:
# 576
 if(Cyc_Tcutil_unify(_tmpC3,_tmpC2))return Cyc_Absyn_sint_typ;goto _LL0;}else{goto _LL5;}case 15U: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpC1.f2)->tag == 15U){_LL3: _LL4:
# 578
 return Cyc_Absyn_sint_typ;}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 581
({struct Cyc_String_pa_PrintArg_struct _tmpC6=({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0U,({
struct _dyneither_ptr _tmp5F1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F7.f1=_tmp5F1;});_tmp4F7;});struct Cyc_String_pa_PrintArg_struct _tmpC7=({struct Cyc_String_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=0U,({struct _dyneither_ptr _tmp5F2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F6.f1=_tmp5F2;});_tmp4F6;});void*_tmpC4[2U];_tmpC4[0]=& _tmpC6,_tmpC4[1]=& _tmpC7;({unsigned int _tmp5F4=loc;struct _dyneither_ptr _tmp5F3=({const char*_tmpC5="comparison not allowed between %s and %s";_tag_dyneither(_tmpC5,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5F4,_tmp5F3,_tag_dyneither(_tmpC4,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 588
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 590
if(({void*_tmp5F5=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5F5 == Cyc_Absyn_sint_typ;}))
return Cyc_Absyn_sint_typ;{
# 596
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpC8=({struct _tuple0 _tmp4F8;_tmp4F8.f1=t1,_tmp4F8.f2=t2;_tmp4F8;});struct _tuple0 _tmpC9=_tmpC8;void*_tmpCB;void*_tmpCA;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpC9.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpC9.f2)->tag == 15U){_LL1: _tmpCB=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpC9.f1)->f1;_tmpCA=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpC9.f2)->f1;_LL2:
# 601
 return Cyc_Absyn_sint_typ;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 604
({struct Cyc_String_pa_PrintArg_struct _tmpCE=({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U,({
struct _dyneither_ptr _tmp5F6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FA.f1=_tmp5F6;});_tmp4FA;});struct Cyc_String_pa_PrintArg_struct _tmpCF=({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U,({struct _dyneither_ptr _tmp5F7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp4F9.f1=_tmp5F7;});_tmp4F9;});void*_tmpCC[2U];_tmpCC[0]=& _tmpCE,_tmpCC[1]=& _tmpCF;({unsigned int _tmp5F9=loc;struct _dyneither_ptr _tmp5F8=({const char*_tmpCD="comparison not allowed between %s and %s";_tag_dyneither(_tmpCD,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5F9,_tmp5F8,_tag_dyneither(_tmpCC,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 612
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 614
enum Cyc_Absyn_Primop _tmpD0=p;switch(_tmpD0){case Cyc_Absyn_Plus: _LL1: _LL2:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LL3: _LL4:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LL5: _LL6:
# 618
 goto _LL8;case Cyc_Absyn_Div: _LL7: _LL8:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LL9: _LLA:
# 621
 goto _LLC;case Cyc_Absyn_Bitand: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_Bitor: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Bitxor: _LLF: _LL10:
 goto _LL12;case Cyc_Absyn_Bitlshift: _LL11: _LL12:
 goto _LL14;case Cyc_Absyn_Bitlrshift: _LL13: _LL14:
 goto _LL16;case Cyc_Absyn_Bitarshift: _LL15: _LL16:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LL17: _LL18:
# 631
 goto _LL1A;case Cyc_Absyn_Neq: _LL19: _LL1A:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LL1B: _LL1C:
# 634
 goto _LL1E;case Cyc_Absyn_Lt: _LL1D: _LL1E:
 goto _LL20;case Cyc_Absyn_Gte: _LL1F: _LL20:
 goto _LL22;case Cyc_Absyn_Lte: _LL21: _LL22:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LL23: _LL24:
# 639
({void*_tmpD1=0U;({struct _dyneither_ptr _tmp5FA=({const char*_tmpD2="bad binary primop";_tag_dyneither(_tmpD2,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5FA,_tag_dyneither(_tmpD1,sizeof(void*),0U));});});}_LL0:;}
# 643
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 651
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpD3=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpD4=Cyc_Tcexp_tcExp(te,topt,_tmpD3);
if(!Cyc_Tcutil_is_numeric(_tmpD3))
({struct Cyc_String_pa_PrintArg_struct _tmpD7=({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0U,({struct _dyneither_ptr _tmp5FB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD4));_tmp4FB.f1=_tmp5FB;});_tmp4FB;});void*_tmpD5[1U];_tmpD5[0]=& _tmpD7;({unsigned int _tmp5FD=_tmpD3->loc;struct _dyneither_ptr _tmp5FC=({const char*_tmpD6="expecting numeric type but found %s";_tag_dyneither(_tmpD6,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5FD,_tmp5FC,_tag_dyneither(_tmpD5,sizeof(void*),1U));});});
return _tmpD4;}
# 658
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpD8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpD9=_tmpD8;switch(_tmpD9){case 0U: _LL1: _LL2:
 return({void*_tmpDA=0U;({struct Cyc_Tcenv_Tenv*_tmp601=te;unsigned int _tmp600=loc;void**_tmp5FF=topt;struct _dyneither_ptr _tmp5FE=({const char*_tmpDB="primitive operator has 0 arguments";_tag_dyneither(_tmpDB,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp601,_tmp600,_tmp5FF,_tmp5FE,_tag_dyneither(_tmpDA,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
({void*_tmp602=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp602;});goto _LL0;case 2U: _LL5: _LL6:
({void*_tmp608=({struct Cyc_Tcenv_Tenv*_tmp607=te;unsigned int _tmp606=loc;void**_tmp605=topt;enum Cyc_Absyn_Primop _tmp604=p;struct Cyc_Absyn_Exp*_tmp603=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp607,_tmp606,_tmp605,_tmp604,_tmp603,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp608;});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpDC=0U;({struct Cyc_Tcenv_Tenv*_tmp60C=te;unsigned int _tmp60B=loc;void**_tmp60A=topt;struct _dyneither_ptr _tmp609=({const char*_tmpDD="primitive operator has > 2 arguments";_tag_dyneither(_tmpDD,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp60C,_tmp60B,_tmp60A,_tmp609,_tag_dyneither(_tmpDC,sizeof(void*),0U));});});}_LL0:;}
# 666
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 669
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpDE=Cyc_Tcutil_compress(t);
void*_tmpDF=_tmpDE;struct Cyc_List_List*_tmpFB;void*_tmpFA;struct Cyc_Absyn_Tqual _tmpF9;struct Cyc_Absyn_Datatypefield*_tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_Absyn_Aggrdecl*_tmpF6;switch(*((int*)_tmpDF)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDF)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmpF6=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpDF)->f1).aggr_info).KnownAggr).val;_LL2:
# 673
 if(_tmpF6->impl == 0){
({void*_tmpE0=0U;({unsigned int _tmp60E=loc;struct _dyneither_ptr _tmp60D=({const char*_tmpE1="attempt to write an abstract aggregate";_tag_dyneither(_tmpE1,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp60E,_tmp60D,_tag_dyneither(_tmpE0,sizeof(void*),0U));});});
return 0;}else{
# 677
_tmpF7=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpF6->impl))->fields;goto _LL4;}}else{goto _LLB;}case 12U: _LL3: _tmpF7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpDF)->f2;_LL4:
# 679
 for(0;_tmpF7 != 0;_tmpF7=_tmpF7->tl){
struct Cyc_Absyn_Aggrfield*_tmpE2=(struct Cyc_Absyn_Aggrfield*)_tmpF7->hd;
if((_tmpE2->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpE5=({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0U,_tmp4FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE2->name);_tmp4FC;});void*_tmpE3[1U];_tmpE3[0]=& _tmpE5;({unsigned int _tmp610=loc;struct _dyneither_ptr _tmp60F=({const char*_tmpE4="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpE4,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp610,_tmp60F,_tag_dyneither(_tmpE3,sizeof(void*),1U));});});
return 0;}
# 685
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpE2->type))return 0;}
# 687
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpDF)->f1).field_info).KnownDatatypefield).tag == 2){_LL5: _tmpF8=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpDF)->f1).field_info).KnownDatatypefield).val).f2;_LL6:
# 689
{struct Cyc_List_List*_tmpE6=_tmpF8->typs;for(0;_tmpE6 != 0;_tmpE6=_tmpE6->tl){
struct _tuple17*_tmpE7=(struct _tuple17*)_tmpE6->hd;struct _tuple17*_tmpE8=_tmpE7;struct Cyc_Absyn_Tqual _tmpED;void*_tmpEC;_LLE: _tmpED=_tmpE8->f1;_tmpEC=_tmpE8->f2;_LLF:;
if(_tmpED.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpEB=({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U,({struct _dyneither_ptr _tmp611=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmpF8->name));_tmp4FD.f1=_tmp611;});_tmp4FD;});void*_tmpE9[1U];_tmpE9[0]=& _tmpEB;({unsigned int _tmp613=loc;struct _dyneither_ptr _tmp612=({const char*_tmpEA="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpEA,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp613,_tmp612,_tag_dyneither(_tmpE9,sizeof(void*),1U));});});
return 0;}
# 695
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpEC))return 0;}}
# 697
return 1;}else{goto _LLB;}case 8U: _LL7: _tmpFA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDF)->f1).elt_type;_tmpF9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpDF)->f1).tq;_LL8:
# 699
 if(_tmpF9.real_const){
({void*_tmpEE=0U;({unsigned int _tmp615=loc;struct _dyneither_ptr _tmp614=({const char*_tmpEF="attempt to over-write a const array";_tag_dyneither(_tmpEF,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp615,_tmp614,_tag_dyneither(_tmpEE,sizeof(void*),0U));});});
return 0;}
# 703
return Cyc_Tcexp_check_writable_aggr(loc,_tmpFA);case 10U: _LL9: _tmpFB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpDF)->f1;_LLA:
# 705
 for(0;_tmpFB != 0;_tmpFB=_tmpFB->tl){
struct _tuple17*_tmpF0=(struct _tuple17*)_tmpFB->hd;struct _tuple17*_tmpF1=_tmpF0;struct Cyc_Absyn_Tqual _tmpF5;void*_tmpF4;_LL11: _tmpF5=_tmpF1->f1;_tmpF4=_tmpF1->f2;_LL12:;
if(_tmpF5.real_const){
({void*_tmpF2=0U;({unsigned int _tmp617=loc;struct _dyneither_ptr _tmp616=({const char*_tmpF3="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmpF3,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp617,_tmp616,_tag_dyneither(_tmpF2,sizeof(void*),0U));});});
return 0;}
# 711
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF4))return 0;}
# 713
return 1;default: _LLB: _LLC:
 return 1;}_LL0:;}
# 721
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 724
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmpFC=e->r;void*_tmpFD=_tmpFC;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct _dyneither_ptr*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct _dyneither_ptr*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Vardecl*_tmp123;struct Cyc_Absyn_Vardecl*_tmp122;struct Cyc_Absyn_Vardecl*_tmp121;struct Cyc_Absyn_Vardecl*_tmp120;switch(*((int*)_tmpFD)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFD)->f1)){case 3U: _LL1: _tmp120=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFD)->f1)->f1;_LL2:
 _tmp121=_tmp120;goto _LL4;case 4U: _LL3: _tmp121=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFD)->f1)->f1;_LL4:
 _tmp122=_tmp121;goto _LL6;case 5U: _LL5: _tmp122=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFD)->f1)->f1;_LL6:
 _tmp123=_tmp122;goto _LL8;case 1U: _LL7: _tmp123=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpFD)->f1)->f1;_LL8:
 if(!(_tmp123->tq).real_const)return;goto _LL0;default: goto _LL15;}case 23U: _LL9: _tmp125=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmp124=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_LLA:
# 731
{void*_tmpFE=Cyc_Tcutil_compress((void*)_check_null(_tmp125->topt));void*_tmpFF=_tmpFE;struct Cyc_List_List*_tmp110;struct Cyc_Absyn_Tqual _tmp10F;struct Cyc_Absyn_Tqual _tmp10E;switch(*((int*)_tmpFF)){case 5U: _LL18: _tmp10E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpFF)->f1).elt_tq;_LL19:
 _tmp10F=_tmp10E;goto _LL1B;case 8U: _LL1A: _tmp10F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpFF)->f1).tq;_LL1B:
 if(!_tmp10F.real_const)return;goto _LL17;case 10U: _LL1C: _tmp110=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpFF)->f1;_LL1D: {
# 735
struct _tuple15 _tmp100=Cyc_Evexp_eval_const_uint_exp(_tmp124);struct _tuple15 _tmp101=_tmp100;unsigned int _tmp10D;int _tmp10C;_LL21: _tmp10D=_tmp101.f1;_tmp10C=_tmp101.f2;_LL22:;
if(!_tmp10C){
({void*_tmp102=0U;({unsigned int _tmp619=e->loc;struct _dyneither_ptr _tmp618=({const char*_tmp103="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp103,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp619,_tmp618,_tag_dyneither(_tmp102,sizeof(void*),0U));});});
return;}
# 740
{struct _handler_cons _tmp104;_push_handler(& _tmp104);{int _tmp106=0;if(setjmp(_tmp104.handler))_tmp106=1;if(!_tmp106){
{struct _tuple17*_tmp107=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp110,(int)_tmp10D);struct _tuple17*_tmp108=_tmp107;struct Cyc_Absyn_Tqual _tmp109;_LL24: _tmp109=_tmp108->f1;_LL25:;
if(!_tmp109.real_const){_npop_handler(0U);return;}}
# 741
;_pop_handler();}else{void*_tmp105=(void*)_exn_thrown;void*_tmp10A=_tmp105;void*_tmp10B;if(((struct Cyc_List_Nth_exn_struct*)_tmp10A)->tag == Cyc_List_Nth){_LL27: _LL28:
# 743
 return;}else{_LL29: _tmp10B=_tmp10A;_LL2A:(int)_rethrow(_tmp10B);}_LL26:;}};}
goto _LL17;}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 747
goto _LL0;case 21U: _LLB: _tmp127=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmp126=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_LLC:
# 749
{void*_tmp111=Cyc_Tcutil_compress((void*)_check_null(_tmp127->topt));void*_tmp112=_tmp111;struct Cyc_List_List*_tmp114;struct Cyc_Absyn_Aggrdecl**_tmp113;switch(*((int*)_tmp112)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp112)->f1).aggr_info).KnownAggr).tag == 2){_LL2C: _tmp113=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp112)->f1).aggr_info).KnownAggr).val;_LL2D: {
# 751
struct Cyc_Absyn_Aggrfield*sf=
_tmp113 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp113,_tmp126);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}case 12U: _LL2E: _tmp114=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp112)->f2;_LL2F: {
# 756
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp114,_tmp126);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 761
goto _LL0;case 22U: _LLD: _tmp129=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_tmp128=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpFD)->f2;_LLE:
# 763
{void*_tmp115=Cyc_Tcutil_compress((void*)_check_null(_tmp129->topt));void*_tmp116=_tmp115;void*_tmp11B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp116)->tag == 5U){_LL33: _tmp11B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp116)->f1).elt_typ;_LL34:
# 765
{void*_tmp117=Cyc_Tcutil_compress(_tmp11B);void*_tmp118=_tmp117;struct Cyc_List_List*_tmp11A;struct Cyc_Absyn_Aggrdecl**_tmp119;switch(*((int*)_tmp118)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp118)->f1).aggr_info).KnownAggr).tag == 2){_LL38: _tmp119=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp118)->f1).aggr_info).KnownAggr).val;_LL39: {
# 767
struct Cyc_Absyn_Aggrfield*sf=
_tmp119 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp119,_tmp128);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}case 12U: _LL3A: _tmp11A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp118)->f2;_LL3B: {
# 772
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp11A,_tmp128);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 777
goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 780
goto _LL0;case 20U: _LLF: _tmp12A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LL10:
# 782
{void*_tmp11C=Cyc_Tcutil_compress((void*)_check_null(_tmp12A->topt));void*_tmp11D=_tmp11C;struct Cyc_Absyn_Tqual _tmp11F;struct Cyc_Absyn_Tqual _tmp11E;switch(*((int*)_tmp11D)){case 5U: _LL3F: _tmp11E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp11D)->f1).elt_tq;_LL40:
 _tmp11F=_tmp11E;goto _LL42;case 8U: _LL41: _tmp11F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp11D)->f1).tq;_LL42:
 if(!_tmp11F.real_const)return;goto _LL3E;default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 787
goto _LL0;case 12U: _LL11: _tmp12B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LL12:
 _tmp12C=_tmp12B;goto _LL14;case 13U: _LL13: _tmp12C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpFD)->f1;_LL14:
 Cyc_Tcexp_check_writable(te,_tmp12C);return;default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 792
({struct Cyc_String_pa_PrintArg_struct _tmp12F=({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U,({struct _dyneither_ptr _tmp61A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4FE.f1=_tmp61A;});_tmp4FE;});void*_tmp12D[1U];_tmp12D[0]=& _tmp12F;({unsigned int _tmp61C=e->loc;struct _dyneither_ptr _tmp61B=({const char*_tmp12E="attempt to write a const location: %s";_tag_dyneither(_tmp12E,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp61C,_tmp61B,_tag_dyneither(_tmp12D,sizeof(void*),1U));});});}
# 795
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 798
({struct Cyc_Tcenv_Tenv*_tmp61D=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp61D,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp130=0U;({struct Cyc_Tcenv_Tenv*_tmp621=te;unsigned int _tmp620=loc;void**_tmp61F=topt;struct _dyneither_ptr _tmp61E=({const char*_tmp131="increment/decrement of non-lvalue";_tag_dyneither(_tmp131,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp621,_tmp620,_tmp61F,_tmp61E,_tag_dyneither(_tmp130,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 804
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp132=0U;({unsigned int _tmp623=e->loc;struct _dyneither_ptr _tmp622=({const char*_tmp133="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp133,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp623,_tmp622,_tag_dyneither(_tmp132,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp134=0U;({unsigned int _tmp625=e->loc;struct _dyneither_ptr _tmp624=({const char*_tmp135="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp135,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp625,_tmp624,_tag_dyneither(_tmp134,sizeof(void*),0U));});});}else{
# 813
({struct Cyc_String_pa_PrintArg_struct _tmp138=({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0U,({struct _dyneither_ptr _tmp626=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4FF.f1=_tmp626;});_tmp4FF;});void*_tmp136[1U];_tmp136[0]=& _tmp138;({unsigned int _tmp628=e->loc;struct _dyneither_ptr _tmp627=({const char*_tmp137="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp137,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp628,_tmp627,_tag_dyneither(_tmp136,sizeof(void*),1U));});});}}
# 815
return t;};}
# 819
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 821
({struct Cyc_Tcenv_Tenv*_tmp62A=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp629=e1;Cyc_Tcexp_tcTest(_tmp62A,_tmp629,({const char*_tmp139="conditional expression";_tag_dyneither(_tmp139,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp62B=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp62B;});else{
# 828
({void*_tmp62C=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp62C;});}{
struct Cyc_List_List _tmp13A=({struct Cyc_List_List _tmp503;_tmp503.hd=e3,_tmp503.tl=0;_tmp503;});
struct Cyc_List_List _tmp13B=({struct Cyc_List_List _tmp502;_tmp502.hd=e2,_tmp502.tl=& _tmp13A;_tmp502;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp13B)){
({struct Cyc_String_pa_PrintArg_struct _tmp13E=({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0U,({
struct _dyneither_ptr _tmp62D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp501.f1=_tmp62D;});_tmp501;});struct Cyc_String_pa_PrintArg_struct _tmp13F=({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0U,({struct _dyneither_ptr _tmp62E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp500.f1=_tmp62E;});_tmp500;});void*_tmp13C[2U];_tmp13C[0]=& _tmp13E,_tmp13C[1]=& _tmp13F;({unsigned int _tmp630=loc;struct _dyneither_ptr _tmp62F=({const char*_tmp13D="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp13D,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp630,_tmp62F,_tag_dyneither(_tmp13C,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 836
return t;};};}
# 840
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 842
({struct Cyc_Tcenv_Tenv*_tmp632=te;struct Cyc_Absyn_Exp*_tmp631=e1;Cyc_Tcexp_tcTest(_tmp632,_tmp631,({const char*_tmp140="logical-and expression";_tag_dyneither(_tmp140,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp634=te;struct Cyc_Absyn_Exp*_tmp633=e2;Cyc_Tcexp_tcTest(_tmp634,_tmp633,({const char*_tmp141="logical-and expression";_tag_dyneither(_tmp141,sizeof(char),23U);}));});
return Cyc_Absyn_sint_typ;}
# 848
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 850
({struct Cyc_Tcenv_Tenv*_tmp636=te;struct Cyc_Absyn_Exp*_tmp635=e1;Cyc_Tcexp_tcTest(_tmp636,_tmp635,({const char*_tmp142="logical-or expression";_tag_dyneither(_tmp142,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp638=te;struct Cyc_Absyn_Exp*_tmp637=e2;Cyc_Tcexp_tcTest(_tmp638,_tmp637,({const char*_tmp143="logical-or expression";_tag_dyneither(_tmp143,sizeof(char),22U);}));});
return Cyc_Absyn_sint_typ;}
# 856
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 863
({struct Cyc_Tcenv_Tenv*_tmp639=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp639,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 868
{void*_tmp144=Cyc_Tcutil_compress(t1);void*_tmp145=_tmp144;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp145)->tag == 8U){_LL1: _LL2:
({void*_tmp146=0U;({unsigned int _tmp63B=loc;struct _dyneither_ptr _tmp63A=({const char*_tmp147="cannot assign to an array";_tag_dyneither(_tmp147,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp63B,_tmp63A,_tag_dyneither(_tmp146,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 873
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp148=0U;({unsigned int _tmp63D=loc;struct _dyneither_ptr _tmp63C=({const char*_tmp149="type is abstract (can't determine size).";_tag_dyneither(_tmp149,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp63D,_tmp63C,_tag_dyneither(_tmp148,sizeof(void*),0U));});});
# 877
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp14A=0U;({struct Cyc_Tcenv_Tenv*_tmp641=te;unsigned int _tmp640=loc;void**_tmp63F=topt;struct _dyneither_ptr _tmp63E=({const char*_tmp14B="assignment to non-lvalue";_tag_dyneither(_tmp14B,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp641,_tmp640,_tmp63F,_tmp63E,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp14C=0U;({unsigned int _tmp643=e2->loc;struct _dyneither_ptr _tmp642=({const char*_tmp14D="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp14D,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp643,_tmp642,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp14E=({struct Cyc_String_pa_PrintArg_struct _tmp151=({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0U,({
struct _dyneither_ptr _tmp644=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp505.f1=_tmp644;});_tmp505;});struct Cyc_String_pa_PrintArg_struct _tmp152=({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0U,({struct _dyneither_ptr _tmp645=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp504.f1=_tmp645;});_tmp504;});void*_tmp14F[2U];_tmp14F[0]=& _tmp151,_tmp14F[1]=& _tmp152;({struct Cyc_Tcenv_Tenv*_tmp649=te;unsigned int _tmp648=loc;void**_tmp647=topt;struct _dyneither_ptr _tmp646=({const char*_tmp150="type mismatch: %s != %s";_tag_dyneither(_tmp150,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp649,_tmp648,_tmp647,_tmp646,_tag_dyneither(_tmp14F,sizeof(void*),2U));});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp14E;}}else{
# 891
enum Cyc_Absyn_Primop _tmp153=(enum Cyc_Absyn_Primop)po->v;
void*_tmp154=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp153,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp154,t1) || Cyc_Tcutil_coerceable(_tmp154) && Cyc_Tcutil_coerceable(t1))){
void*_tmp155=({struct Cyc_String_pa_PrintArg_struct _tmp158=({struct Cyc_String_pa_PrintArg_struct _tmp507;_tmp507.tag=0U,({
# 896
struct _dyneither_ptr _tmp64A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp507.f1=_tmp64A;});_tmp507;});struct Cyc_String_pa_PrintArg_struct _tmp159=({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0U,({
struct _dyneither_ptr _tmp64B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp506.f1=_tmp64B;});_tmp506;});void*_tmp156[2U];_tmp156[0]=& _tmp158,_tmp156[1]=& _tmp159;({struct Cyc_Tcenv_Tenv*_tmp64F=te;unsigned int _tmp64E=loc;void**_tmp64D=topt;struct _dyneither_ptr _tmp64C=({const char*_tmp157="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp157,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp64F,_tmp64E,_tmp64D,_tmp64C,_tag_dyneither(_tmp156,sizeof(void*),2U));});});
Cyc_Tcutil_unify(_tmp154,t1);
Cyc_Tcutil_explain_failure();
return _tmp155;}
# 902
return _tmp154;}
# 904
return t1;};};}
# 908
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp650=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp650,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp652=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp651=topt;Cyc_Tcexp_tcExp(_tmp652,_tmp651,e2);});
return(void*)_check_null(e2->topt);}
# 915
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 918
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 921
{void*_tmp15A=Cyc_Tcutil_compress(t1);void*_tmp15B=_tmp15A;switch(*((int*)_tmp15B)){case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp15B)->f1 == 0){_LL1: _LL2:
({struct Cyc_Tcenv_Tenv*_tmp654=te;struct Cyc_Absyn_Exp*_tmp653=e;Cyc_Tcutil_unchecked_cast(_tmp654,_tmp653,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);});({void*_tmp655=Cyc_Absyn_float_typ(1);t1=_tmp655;});goto _LL0;}else{goto _LL7;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp15B)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL0;default: goto _LL7;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 928
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp15C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp15D=_tmp15C;struct _tuple1*_tmp161;struct Cyc_List_List*_tmp160;unsigned int _tmp15F;enum Cyc_Absyn_Scope _tmp15E;_LLA: _tmp161=_tmp15D.name;_tmp160=_tmp15D.typs;_tmp15F=_tmp15D.loc;_tmp15E=_tmp15D.sc;_LLB:;
# 931
if(_tmp160 == 0  || _tmp160->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp160->hd)).f2);
# 934
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 939
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp162=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp163=_tmp162;struct _tuple1*_tmp167;struct Cyc_List_List*_tmp166;unsigned int _tmp165;enum Cyc_Absyn_Scope _tmp164;_LLD: _tmp167=_tmp163.name;_tmp166=_tmp163.typs;_tmp165=_tmp163.loc;_tmp164=_tmp163.sc;_LLE:;
# 942
if(_tmp166 == 0  || _tmp166->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp166->hd)).f2);
# 946
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 951
({struct Cyc_String_pa_PrintArg_struct _tmp16A=({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0U,({
struct _dyneither_ptr _tmp656=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp509.f1=_tmp656;});_tmp509;});struct Cyc_String_pa_PrintArg_struct _tmp16B=({struct Cyc_String_pa_PrintArg_struct _tmp508;_tmp508.tag=0U,({struct _dyneither_ptr _tmp657=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp508.f1=_tmp657;});_tmp508;});void*_tmp168[2U];_tmp168[0]=& _tmp16A,_tmp168[1]=& _tmp16B;({unsigned int _tmp659=e->loc;struct _dyneither_ptr _tmp658=({const char*_tmp169="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp169,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp659,_tmp658,_tag_dyneither(_tmp168,sizeof(void*),2U));});});
return 0;}
# 957
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 963
struct Cyc_List_List*_tmp16C=args;
int _tmp16D=0;
struct Cyc_Tcenv_Tenv*_tmp16E=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp16F=Cyc_Tcenv_clear_abstract_val_ok(_tmp16E);
Cyc_Tcexp_tcExp(_tmp16F,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 972
void*_tmp170=t;void*_tmp1D3;struct Cyc_Absyn_Tqual _tmp1D2;void*_tmp1D1;union Cyc_Absyn_Constraint*_tmp1D0;union Cyc_Absyn_Constraint*_tmp1CF;union Cyc_Absyn_Constraint*_tmp1CE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->tag == 5U){_LL1: _tmp1D3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).elt_typ;_tmp1D2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).elt_tq;_tmp1D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).ptr_atts).rgn;_tmp1D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).ptr_atts).nullable;_tmp1CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).ptr_atts).bounds;_tmp1CE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp170)->f1).ptr_atts).zero_term;_LL2:
# 977
 Cyc_Tcenv_check_rgn_accessible(_tmp16F,loc,_tmp1D1);
# 979
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1CF);{
void*_tmp171=Cyc_Tcutil_compress(_tmp1D3);void*_tmp172=_tmp171;struct Cyc_List_List*_tmp1CA;void*_tmp1C9;struct Cyc_Absyn_Tqual _tmp1C8;void*_tmp1C7;struct Cyc_List_List*_tmp1C6;int _tmp1C5;struct Cyc_Absyn_VarargInfo*_tmp1C4;struct Cyc_List_List*_tmp1C3;struct Cyc_List_List*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Exp*_tmp1BF;struct Cyc_List_List*_tmp1BE;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->tag == 9U){_LL6: _tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).tvars;_tmp1C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).effect;_tmp1C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).ret_tqual;_tmp1C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).ret_typ;_tmp1C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).args;_tmp1C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).c_varargs;_tmp1C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).cyc_varargs;_tmp1C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).rgn_po;_tmp1C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).attributes;_tmp1C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).requires_clause;_tmp1C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).requires_relns;_tmp1BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).ensures_clause;_tmp1BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp172)->f1).ensures_relns;_LL7:
# 984
 if(_tmp1CA != 0  || _tmp1C3 != 0)
({void*_tmp173=0U;({unsigned int _tmp65B=e->loc;struct _dyneither_ptr _tmp65A=({const char*_tmp174="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp174,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp65B,_tmp65A,_tag_dyneither(_tmp173,sizeof(void*),0U));});});
# 988
if(topt != 0)Cyc_Tcutil_unify(_tmp1C7,*topt);
# 990
while(_tmp16C != 0  && _tmp1C6 != 0){
# 992
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp16C->hd;
void*t2=(*((struct _tuple9*)_tmp1C6->hd)).f3;
Cyc_Tcexp_tcExp(_tmp16F,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp16F,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp184="actual argument has type ";_tag_dyneither(_tmp184,sizeof(char),26U);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp183=" but formal has type ";_tag_dyneither(_tmp183,sizeof(char),22U);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp65E=({unsigned long _tmp65D=({unsigned long _tmp65C=Cyc_strlen((struct _dyneither_ptr)s0);_tmp65C + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp65D + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp65E + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70)
({void*_tmp175=0U;({unsigned int _tmp661=e1->loc;struct _dyneither_ptr _tmp660=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp178=({struct Cyc_String_pa_PrintArg_struct _tmp50D;_tmp50D.tag=0U,_tmp50D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp50D;});struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp50C;_tmp50C.tag=0U,_tmp50C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp50C;});struct Cyc_String_pa_PrintArg_struct _tmp17A=({struct Cyc_String_pa_PrintArg_struct _tmp50B;_tmp50B.tag=0U,_tmp50B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp50B;});struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0U,_tmp50A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp50A;});void*_tmp176[4U];_tmp176[0]=& _tmp178,_tmp176[1]=& _tmp179,_tmp176[2]=& _tmp17A,_tmp176[3]=& _tmp17B;({struct _dyneither_ptr _tmp65F=({const char*_tmp177="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp177,sizeof(char),15U);});Cyc_aprintf(_tmp65F,_tag_dyneither(_tmp176,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp661,_tmp660,_tag_dyneither(_tmp175,sizeof(void*),0U));});});else{
# 1004
({void*_tmp17C=0U;({unsigned int _tmp664=e1->loc;struct _dyneither_ptr _tmp663=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17F=({struct Cyc_String_pa_PrintArg_struct _tmp511;_tmp511.tag=0U,_tmp511.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);_tmp511;});struct Cyc_String_pa_PrintArg_struct _tmp180=({struct Cyc_String_pa_PrintArg_struct _tmp510;_tmp510.tag=0U,_tmp510.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);_tmp510;});struct Cyc_String_pa_PrintArg_struct _tmp181=({struct Cyc_String_pa_PrintArg_struct _tmp50F;_tmp50F.tag=0U,_tmp50F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);_tmp50F;});struct Cyc_String_pa_PrintArg_struct _tmp182=({struct Cyc_String_pa_PrintArg_struct _tmp50E;_tmp50E.tag=0U,_tmp50E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);_tmp50E;});void*_tmp17D[4U];_tmp17D[0]=& _tmp17F,_tmp17D[1]=& _tmp180,_tmp17D[2]=& _tmp181,_tmp17D[3]=& _tmp182;({struct _dyneither_ptr _tmp662=({const char*_tmp17E="%s%s%s%s.";_tag_dyneither(_tmp17E,sizeof(char),10U);});Cyc_aprintf(_tmp662,_tag_dyneither(_tmp17D,sizeof(void*),4U));});});Cyc_Tcutil_terr(_tmp664,_tmp663,_tag_dyneither(_tmp17C,sizeof(void*),0U));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1009
if(alias_coercion)
({struct Cyc_List_List*_tmp665=({struct Cyc_List_List*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->hd=(void*)_tmp16D,_tmp185->tl=*alias_arg_exps;_tmp185;});*alias_arg_exps=_tmp665;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp186=0U;({unsigned int _tmp667=e1->loc;struct _dyneither_ptr _tmp666=({const char*_tmp187="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp187,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp667,_tmp666,_tag_dyneither(_tmp186,sizeof(void*),0U));});});
_tmp16C=_tmp16C->tl;
_tmp1C6=_tmp1C6->tl;
++ _tmp16D;}{
# 1020
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1C2;for(0;a != 0;a=a->tl){
void*_tmp188=(void*)a->hd;void*_tmp189=_tmp188;enum Cyc_Absyn_Format_Type _tmp196;int _tmp195;int _tmp194;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp189)->tag == 19U){_LLB: _tmp196=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp189)->f1;_tmp195=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp189)->f2;_tmp194=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp189)->f3;_LLC:
# 1024
{struct _handler_cons _tmp18A;_push_handler(& _tmp18A);{int _tmp18C=0;if(setjmp(_tmp18A.handler))_tmp18C=1;if(!_tmp18C){
# 1026
{struct Cyc_Absyn_Exp*_tmp18D=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp195 - 1);
# 1028
struct Cyc_Core_Opt*fmt_args;
if(_tmp194 == 0)
fmt_args=0;else{
# 1032
({struct Cyc_Core_Opt*_tmp669=({struct Cyc_Core_Opt*_tmp18E=_cycalloc(sizeof(*_tmp18E));({struct Cyc_List_List*_tmp668=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp194 - 1);_tmp18E->v=_tmp668;});_tmp18E;});fmt_args=_tmp669;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp18F=_tmp196;if(_tmp18F == Cyc_Absyn_Printf_ft){_LL10: _LL11:
# 1036
 Cyc_Tcexp_check_format_args(_tmp16F,_tmp18D,fmt_args,_tmp194 - 1,alias_arg_exps,Cyc_Formatstr_get_format_typs);
# 1039
goto _LLF;}else{_LL12: _LL13:
# 1041
 Cyc_Tcexp_check_format_args(_tmp16F,_tmp18D,fmt_args,_tmp194 - 1,alias_arg_exps,Cyc_Formatstr_get_scanf_typs);
# 1044
goto _LLF;}_LLF:;};}
# 1026
;_pop_handler();}else{void*_tmp18B=(void*)_exn_thrown;void*_tmp190=_tmp18B;void*_tmp193;if(((struct Cyc_List_Nth_exn_struct*)_tmp190)->tag == Cyc_List_Nth){_LL15: _LL16:
# 1046
({void*_tmp191=0U;({unsigned int _tmp66B=loc;struct _dyneither_ptr _tmp66A=({const char*_tmp192="bad format arguments";_tag_dyneither(_tmp192,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp66B,_tmp66A,_tag_dyneither(_tmp191,sizeof(void*),0U));});});goto _LL14;}else{_LL17: _tmp193=_tmp190;_LL18:(int)_rethrow(_tmp193);}_LL14:;}};}
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1051
if(_tmp1C6 != 0)({void*_tmp197=0U;({unsigned int _tmp66D=loc;struct _dyneither_ptr _tmp66C=({const char*_tmp198="too few arguments for function";_tag_dyneither(_tmp198,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp66D,_tmp66C,_tag_dyneither(_tmp197,sizeof(void*),0U));});});else{
# 1053
if((_tmp16C != 0  || _tmp1C5) || _tmp1C4 != 0){
if(_tmp1C5)
for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
Cyc_Tcexp_tcExp(_tmp16F,0,(struct Cyc_Absyn_Exp*)_tmp16C->hd);}else{
if(_tmp1C4 == 0)
({void*_tmp199=0U;({unsigned int _tmp66F=loc;struct _dyneither_ptr _tmp66E=({const char*_tmp19A="too many arguments for function";_tag_dyneither(_tmp19A,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp66F,_tmp66E,_tag_dyneither(_tmp199,sizeof(void*),0U));});});else{
# 1060
struct Cyc_Absyn_VarargInfo _tmp19B=*_tmp1C4;struct Cyc_Absyn_VarargInfo _tmp19C=_tmp19B;void*_tmp1BA;int _tmp1B9;_LL1A: _tmp1BA=_tmp19C.type;_tmp1B9=_tmp19C.inject;_LL1B:;{
struct Cyc_Absyn_VarargCallInfo*_tmp19D=({struct Cyc_Absyn_VarargCallInfo*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->num_varargs=0,_tmp1B8->injectors=0,_tmp1B8->vai=_tmp1C4;_tmp1B8;});
# 1064
*vararg_call_info=_tmp19D;
# 1066
if(!_tmp1B9)
# 1068
for(0;_tmp16C != 0;(_tmp16C=_tmp16C->tl,_tmp16D ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp16C->hd;
++ _tmp19D->num_varargs;
Cyc_Tcexp_tcExp(_tmp16F,& _tmp1BA,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp16F,e1,_tmp1BA,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1A0=({struct Cyc_String_pa_PrintArg_struct _tmp513;_tmp513.tag=0U,({
struct _dyneither_ptr _tmp670=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1BA));_tmp513.f1=_tmp670;});_tmp513;});struct Cyc_String_pa_PrintArg_struct _tmp1A1=({struct Cyc_String_pa_PrintArg_struct _tmp512;_tmp512.tag=0U,({struct _dyneither_ptr _tmp671=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp512.f1=_tmp671;});_tmp512;});void*_tmp19E[2U];_tmp19E[0]=& _tmp1A0,_tmp19E[1]=& _tmp1A1;({unsigned int _tmp673=loc;struct _dyneither_ptr _tmp672=({const char*_tmp19F="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp19F,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp673,_tmp672,_tag_dyneither(_tmp19E,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 1078
if(alias_coercion)
({struct Cyc_List_List*_tmp674=({struct Cyc_List_List*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->hd=(void*)_tmp16D,_tmp1A2->tl=*alias_arg_exps;_tmp1A2;});*alias_arg_exps=_tmp674;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1BA) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1A3=0U;({unsigned int _tmp676=e1->loc;struct _dyneither_ptr _tmp675=({const char*_tmp1A4="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1A4,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp676,_tmp675,_tag_dyneither(_tmp1A3,sizeof(void*),0U));});});}else{
# 1087
void*_tmp1A5=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1BA));void*_tmp1A6=_tmp1A5;struct Cyc_Absyn_Datatypedecl*_tmp1B7;struct Cyc_List_List*_tmp1B6;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1A6)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1A6)->f1).datatype_info).KnownDatatype).tag == 2){_LL1D: _tmp1B7=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1A6)->f1).datatype_info).KnownDatatype).val;_tmp1B6=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1A6)->f1).targs;_LL1E: {
# 1091
struct Cyc_Absyn_Datatypedecl*_tmp1A7=*Cyc_Tcenv_lookup_datatypedecl(_tmp16F,loc,_tmp1B7->name);
struct Cyc_List_List*fields=0;
if(_tmp1A7->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1AA=({struct Cyc_String_pa_PrintArg_struct _tmp514;_tmp514.tag=0U,({
struct _dyneither_ptr _tmp677=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1BA));_tmp514.f1=_tmp677;});_tmp514;});void*_tmp1A8[1U];_tmp1A8[0]=& _tmp1AA;({unsigned int _tmp679=loc;struct _dyneither_ptr _tmp678=({const char*_tmp1A9="can't inject into abstract datatype %s";_tag_dyneither(_tmp1A9,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp679,_tmp678,_tag_dyneither(_tmp1A8,sizeof(void*),1U));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1A7->fields))->v;}
# 1102
if(!({void*_tmp67A=Cyc_Tcutil_pointer_region(_tmp1BA);Cyc_Tcutil_unify(_tmp67A,Cyc_Tcenv_curr_rgn(_tmp16F));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1AD=({struct Cyc_String_pa_PrintArg_struct _tmp516;_tmp516.tag=0U,({
struct _dyneither_ptr _tmp67B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1BA));_tmp516.f1=_tmp67B;});_tmp516;});struct Cyc_String_pa_PrintArg_struct _tmp1AE=({struct Cyc_String_pa_PrintArg_struct _tmp515;_tmp515.tag=0U,({struct _dyneither_ptr _tmp67C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp16F)));_tmp515.f1=_tmp67C;});_tmp515;});void*_tmp1AB[2U];_tmp1AB[0]=& _tmp1AD,_tmp1AB[1]=& _tmp1AE;({unsigned int _tmp67E=loc;struct _dyneither_ptr _tmp67D=({const char*_tmp1AC="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1AC,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp67E,_tmp67D,_tag_dyneither(_tmp1AB,sizeof(void*),2U));});});{
# 1106
struct Cyc_List_List*_tmp1AF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1A7->tvs,_tmp1B6);
for(0;_tmp16C != 0;_tmp16C=_tmp16C->tl){
++ _tmp19D->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp16C->hd;
# 1111
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp16F,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1B0=0U;({unsigned int _tmp680=e1->loc;struct _dyneither_ptr _tmp67F=({const char*_tmp1B1="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1B1,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp680,_tmp67F,_tag_dyneither(_tmp1B0,sizeof(void*),0U));});});}{
# 1117
struct Cyc_Absyn_Datatypefield*_tmp1B2=({struct Cyc_Tcenv_Tenv*_tmp684=_tmp16F;struct Cyc_Absyn_Exp*_tmp683=e1;void*_tmp682=Cyc_Tcutil_pointer_elt_type(_tmp1BA);struct Cyc_List_List*_tmp681=_tmp1AF;Cyc_Tcexp_tcInjection(_tmp684,_tmp683,_tmp682,_tmp681,fields);});
if(_tmp1B2 != 0)
({struct Cyc_List_List*_tmp686=({
struct Cyc_List_List*_tmp685=_tmp19D->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp685,({struct Cyc_List_List*_tmp1B3=_cycalloc(sizeof(*_tmp1B3));
_tmp1B3->hd=_tmp1B2,_tmp1B3->tl=0;_tmp1B3;}));});
# 1119
_tmp19D->injectors=_tmp686;});};};}
# 1123
goto _LL1C;};}}else{goto _LL1F;}}else{_LL1F: _LL20:
({void*_tmp1B4=0U;({unsigned int _tmp688=loc;struct _dyneither_ptr _tmp687=({const char*_tmp1B5="bad inject vararg type";_tag_dyneither(_tmp1B5,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp688,_tmp687,_tag_dyneither(_tmp1B4,sizeof(void*),0U));});});goto _LL1C;}_LL1C:;}};}}}}
# 1129
if(*alias_arg_exps == 0)
# 1138 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp16F,loc,(void*)_check_null(_tmp1C9));
# 1140
return _tmp1C7;};}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1BD=({struct Cyc_String_pa_PrintArg_struct _tmp517;_tmp517.tag=0U,({struct _dyneither_ptr _tmp689=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp517.f1=_tmp689;});_tmp517;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BD;({struct Cyc_Tcenv_Tenv*_tmp68D=_tmp16F;unsigned int _tmp68C=loc;void**_tmp68B=topt;struct _dyneither_ptr _tmp68A=({const char*_tmp1BC="expected pointer to function but found %s";_tag_dyneither(_tmp1BC,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp68D,_tmp68C,_tmp68B,_tmp68A,_tag_dyneither(_tmp1BB,sizeof(void*),1U));});});}_LL5:;};}else{_LL3: _LL4:
# 1143
 return({struct Cyc_String_pa_PrintArg_struct _tmp1CD=({struct Cyc_String_pa_PrintArg_struct _tmp518;_tmp518.tag=0U,({struct _dyneither_ptr _tmp68E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp518.f1=_tmp68E;});_tmp518;});void*_tmp1CB[1U];_tmp1CB[0]=& _tmp1CD;({struct Cyc_Tcenv_Tenv*_tmp692=_tmp16F;unsigned int _tmp691=loc;void**_tmp690=topt;struct _dyneither_ptr _tmp68F=({const char*_tmp1CC="expected pointer to function but found %s";_tag_dyneither(_tmp1CC,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp692,_tmp691,_tmp690,_tmp68F,_tag_dyneither(_tmp1CB,sizeof(void*),1U));});});}_LL0:;};}
# 1147
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1D4=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1D4,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1D4,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1D7=({struct Cyc_String_pa_PrintArg_struct _tmp51A;_tmp51A.tag=0U,({struct _dyneither_ptr _tmp693=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D4));_tmp51A.f1=_tmp693;});_tmp51A;});struct Cyc_String_pa_PrintArg_struct _tmp1D8=({struct Cyc_String_pa_PrintArg_struct _tmp519;_tmp519.tag=0U,({
struct _dyneither_ptr _tmp694=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp519.f1=_tmp694;});_tmp519;});void*_tmp1D5[2U];_tmp1D5[0]=& _tmp1D7,_tmp1D5[1]=& _tmp1D8;({unsigned int _tmp696=loc;struct _dyneither_ptr _tmp695=({const char*_tmp1D6="expected %s but found %s";_tag_dyneither(_tmp1D6,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp696,_tmp695,_tag_dyneither(_tmp1D5,sizeof(void*),2U));});});
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1159
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1161
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1163
({void*_tmp697=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp697;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1D9=t1;void*_tmp1F4;struct Cyc_Absyn_Tqual _tmp1F3;void*_tmp1F2;union Cyc_Absyn_Constraint*_tmp1F1;union Cyc_Absyn_Constraint*_tmp1F0;union Cyc_Absyn_Constraint*_tmp1EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9)->tag == 5U){_LL1: _tmp1F4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9)->f1).elt_typ;_tmp1F3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9)->f1).elt_tq;_tmp1F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9)->f1).ptr_atts).rgn;_tmp1F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9)->f1).ptr_atts).nullable;_tmp1F0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9)->f1).ptr_atts).bounds;_tmp1EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1D9)->f1).ptr_atts).zero_term;_LL2:
# 1167
{void*_tmp1DA=Cyc_Tcutil_compress(_tmp1F4);void*_tmp1DB=_tmp1DA;struct Cyc_List_List*_tmp1EE;void*_tmp1ED;struct Cyc_Absyn_Tqual _tmp1EC;void*_tmp1EB;struct Cyc_List_List*_tmp1EA;int _tmp1E9;struct Cyc_Absyn_VarargInfo*_tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E6;struct Cyc_Absyn_Exp*_tmp1E5;struct Cyc_List_List*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_List_List*_tmp1E2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->tag == 9U){_LL6: _tmp1EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).tvars;_tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).effect;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).ret_tqual;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).ret_typ;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).args;_tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).c_varargs;_tmp1E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).cyc_varargs;_tmp1E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).rgn_po;_tmp1E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).attributes;_tmp1E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).requires_clause;_tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).requires_relns;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).ensures_clause;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1DB)->f1).ensures_relns;_LL7: {
# 1169
struct Cyc_List_List*instantiation=0;
# 1171
for(0;ts != 0  && _tmp1EE != 0;(ts=ts->tl,_tmp1EE=_tmp1EE->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp1EE->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp69B=loc;struct Cyc_Tcenv_Tenv*_tmp69A=te;struct Cyc_List_List*_tmp699=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp698=k;Cyc_Tcutil_check_type(_tmp69B,_tmp69A,_tmp699,_tmp698,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp69D=({struct Cyc_List_List*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));({struct _tuple13*_tmp69C=({struct _tuple13*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));_tmp1DC->f1=(struct Cyc_Absyn_Tvar*)_tmp1EE->hd,_tmp1DC->f2=(void*)ts->hd;_tmp1DC;});_tmp1DD->hd=_tmp69C;}),_tmp1DD->tl=instantiation;_tmp1DD;});instantiation=_tmp69D;});}
# 1177
if(ts != 0)
return({void*_tmp1DE=0U;({struct Cyc_Tcenv_Tenv*_tmp6A1=te;unsigned int _tmp6A0=loc;void**_tmp69F=topt;struct _dyneither_ptr _tmp69E=({const char*_tmp1DF="too many type variables in instantiation";_tag_dyneither(_tmp1DF,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp6A1,_tmp6A0,_tmp69F,_tmp69E,_tag_dyneither(_tmp1DE,sizeof(void*),0U));});});
# 1182
if(_tmp1EE == 0){
({struct Cyc_List_List*_tmp6A2=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp1E7);_tmp1E7=_tmp6A2;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp1E7);
_tmp1E7=0;}{
# 1187
void*new_fn_typ=({
struct Cyc_List_List*_tmp6A3=instantiation;Cyc_Tcutil_substitute(_tmp6A3,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));
_tmp1E1->tag=9U,(_tmp1E1->f1).tvars=_tmp1EE,(_tmp1E1->f1).effect=_tmp1ED,(_tmp1E1->f1).ret_tqual=_tmp1EC,(_tmp1E1->f1).ret_typ=_tmp1EB,(_tmp1E1->f1).args=_tmp1EA,(_tmp1E1->f1).c_varargs=_tmp1E9,(_tmp1E1->f1).cyc_varargs=_tmp1E8,(_tmp1E1->f1).rgn_po=_tmp1E7,(_tmp1E1->f1).attributes=_tmp1E6,(_tmp1E1->f1).requires_clause=_tmp1E5,(_tmp1E1->f1).requires_relns=_tmp1E4,(_tmp1E1->f1).ensures_clause=_tmp1E3,(_tmp1E1->f1).ensures_relns=_tmp1E2;_tmp1E1;}));});
# 1192
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));_tmp1E0->tag=5U,(_tmp1E0->f1).elt_typ=new_fn_typ,(_tmp1E0->f1).elt_tq=_tmp1F3,((_tmp1E0->f1).ptr_atts).rgn=_tmp1F2,((_tmp1E0->f1).ptr_atts).nullable=_tmp1F1,((_tmp1E0->f1).ptr_atts).bounds=_tmp1F0,((_tmp1E0->f1).ptr_atts).zero_term=_tmp1EF,((_tmp1E0->f1).ptr_atts).ptrloc=0;_tmp1E0;});};}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1195
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1198
return({struct Cyc_String_pa_PrintArg_struct _tmp1F7=({struct Cyc_String_pa_PrintArg_struct _tmp51B;_tmp51B.tag=0U,({
struct _dyneither_ptr _tmp6A4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp51B.f1=_tmp6A4;});_tmp51B;});void*_tmp1F5[1U];_tmp1F5[0]=& _tmp1F7;({struct Cyc_Tcenv_Tenv*_tmp6A8=te;unsigned int _tmp6A7=loc;void**_tmp6A6=topt;struct _dyneither_ptr _tmp6A5=({const char*_tmp1F6="expecting polymorphic type but found %s";_tag_dyneither(_tmp1F6,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp6A8,_tmp6A7,_tmp6A6,_tmp6A5,_tag_dyneither(_tmp1F5,sizeof(void*),1U));});});};}
# 1203
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1205
({unsigned int _tmp6AC=loc;struct Cyc_Tcenv_Tenv*_tmp6AB=te;struct Cyc_List_List*_tmp6AA=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp6A9=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1205
Cyc_Tcutil_check_type(_tmp6AC,_tmp6AB,_tmp6AA,_tmp6A9,1,0,t);});
# 1207
Cyc_Tcutil_check_no_qual(loc,t);
# 1209
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1214
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1221
Cyc_Tcutil_unify(t2,t);{
void*_tmp1F8=({struct Cyc_String_pa_PrintArg_struct _tmp1FB=({struct Cyc_String_pa_PrintArg_struct _tmp51D;_tmp51D.tag=0U,({
struct _dyneither_ptr _tmp6AD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp51D.f1=_tmp6AD;});_tmp51D;});struct Cyc_String_pa_PrintArg_struct _tmp1FC=({struct Cyc_String_pa_PrintArg_struct _tmp51C;_tmp51C.tag=0U,({struct _dyneither_ptr _tmp6AE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp51C.f1=_tmp6AE;});_tmp51C;});void*_tmp1F9[2U];_tmp1F9[0]=& _tmp1FB,_tmp1F9[1]=& _tmp1FC;({struct Cyc_Tcenv_Tenv*_tmp6B1=te;unsigned int _tmp6B0=loc;struct _dyneither_ptr _tmp6AF=({const char*_tmp1FA="cannot cast %s to %s";_tag_dyneither(_tmp1FA,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp6B1,_tmp6B0,& t,_tmp6AF,_tag_dyneither(_tmp1F9,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();
return _tmp1F8;};}}}
# 1231
{struct _tuple0 _tmp1FD=({struct _tuple0 _tmp51E;_tmp51E.f1=e->r,({void*_tmp6B2=Cyc_Tcutil_compress(t);_tmp51E.f2=_tmp6B2;});_tmp51E;});struct _tuple0 _tmp1FE=_tmp1FD;int _tmp207;union Cyc_Absyn_Constraint*_tmp206;union Cyc_Absyn_Constraint*_tmp205;union Cyc_Absyn_Constraint*_tmp204;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1FE.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE.f2)->tag == 5U){_LL1: _tmp207=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp1FE.f1)->f1).fat_result;_tmp206=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE.f2)->f1).ptr_atts).nullable;_tmp205=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE.f2)->f1).ptr_atts).bounds;_tmp204=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FE.f2)->f1).ptr_atts).zero_term;_LL2:
# 1235
 if((_tmp207  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp204)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp206)){
void*_tmp1FF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp205);void*_tmp200=_tmp1FF;struct Cyc_Absyn_Exp*_tmp203;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp200)->tag == 1U){_LL6: _tmp203=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp200)->f1;_LL7:
# 1238
 if((Cyc_Evexp_eval_const_uint_exp(_tmp203)).f1 == 1)
({void*_tmp201=0U;({unsigned int _tmp6B4=loc;struct _dyneither_ptr _tmp6B3=({const char*_tmp202="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp202,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp6B4,_tmp6B3,_tag_dyneither(_tmp201,sizeof(void*),0U));});});
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1244
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1247
return t;};}
# 1251
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
void**_tmp208=0;
struct Cyc_Absyn_Tqual _tmp209=Cyc_Absyn_empty_tqual(0U);
int _tmp20A=0;
if(topt != 0){
void*_tmp20B=Cyc_Tcutil_compress(*topt);void*_tmp20C=_tmp20B;void**_tmp20F;struct Cyc_Absyn_Tqual _tmp20E;union Cyc_Absyn_Constraint*_tmp20D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->tag == 5U){_LL1: _tmp20F=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->f1).elt_typ;_tmp20E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->f1).elt_tq;_tmp20D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp20C)->f1).ptr_atts).nullable;_LL2:
# 1258
 _tmp208=_tmp20F;
_tmp209=_tmp20E;
({int _tmp6B5=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp20D);_tmp20A=_tmp6B5;});
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1272
({struct Cyc_Tcenv_Tenv*_tmp6B7=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp6B6=_tmp208;Cyc_Tcexp_tcExpNoInst(_tmp6B7,_tmp6B6,e);});
# 1275
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp210=0U;({unsigned int _tmp6B9=e->loc;struct _dyneither_ptr _tmp6B8=({const char*_tmp211="Cannot take the address of an alias-free path";_tag_dyneither(_tmp211,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6B9,_tmp6B8,_tag_dyneither(_tmp210,sizeof(void*),0U));});});
# 1281
{void*_tmp212=e->r;void*_tmp213=_tmp212;struct Cyc_Absyn_Exp*_tmp217;struct Cyc_Absyn_Exp*_tmp216;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp213)->tag == 23U){_LL6: _tmp217=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp213)->f1;_tmp216=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp213)->f2;_LL7:
# 1283
{void*_tmp214=Cyc_Tcutil_compress((void*)_check_null(_tmp217->topt));void*_tmp215=_tmp214;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp215)->tag == 10U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1288
({void*_tmp6BA=(Cyc_Absyn_add_exp(_tmp217,_tmp216,0U))->r;e0->r=_tmp6BA;});
return Cyc_Tcexp_tcPlus(te,_tmp217,_tmp216);}_LLA:;}
# 1291
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1296
{void*_tmp218=e->r;void*_tmp219=_tmp218;switch(*((int*)_tmp219)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp219)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp219)->f3 == 1){_LL12: _LL13:
# 1299
({void*_tmp21A=0U;({unsigned int _tmp6BC=e->loc;struct _dyneither_ptr _tmp6BB=({const char*_tmp21B="cannot take the address of a @tagged union member";_tag_dyneither(_tmp21B,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp6BC,_tmp6BB,_tag_dyneither(_tmp21A,sizeof(void*),0U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1305
struct _tuple14 _tmp21C=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp21D=_tmp21C;int _tmp221;void*_tmp220;_LL17: _tmp221=_tmp21D.f1;_tmp220=_tmp21D.f2;_LL18:;
# 1307
if(Cyc_Tcutil_is_noalias_region(_tmp220,0))
({void*_tmp21E=0U;({unsigned int _tmp6BE=e->loc;struct _dyneither_ptr _tmp6BD=({const char*_tmp21F="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp21F,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp6BE,_tmp6BD,_tag_dyneither(_tmp21E,sizeof(void*),0U));});});{
# 1310
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(_tmp221){
tq.print_const=1;
tq.real_const=1;}{
# 1316
void*t=_tmp20A?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp220,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp220,tq,Cyc_Absyn_false_conref);
return t;};};};}
# 1323
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1327
return Cyc_Absyn_uint_typ;
# 1329
({unsigned int _tmp6C1=loc;struct Cyc_Tcenv_Tenv*_tmp6C0=te;struct Cyc_List_List*_tmp6BF=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6C1,_tmp6C0,_tmp6BF,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp224=({struct Cyc_String_pa_PrintArg_struct _tmp51F;_tmp51F.tag=0U,({
struct _dyneither_ptr _tmp6C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp51F.f1=_tmp6C2;});_tmp51F;});void*_tmp222[1U];_tmp222[0]=& _tmp224;({unsigned int _tmp6C4=loc;struct _dyneither_ptr _tmp6C3=({const char*_tmp223="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp223,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp6C4,_tmp6C3,_tag_dyneither(_tmp222,sizeof(void*),1U));});});
if(topt != 0){
void*_tmp225=Cyc_Tcutil_compress(*topt);void*_tmp226=_tmp225;void*_tmp22B;void*_tmp22A;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp226)->tag == 19U){_LL1: _tmp22B=_tmp226;_tmp22A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp226)->f1;_LL2: {
# 1338
struct Cyc_Absyn_Exp*_tmp227=Cyc_Absyn_sizeoftyp_exp(t,0U);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp228=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp229=_cycalloc(sizeof(*_tmp229));_tmp229->tag=18U,_tmp229->f1=_tmp227;_tmp229;});
if(Cyc_Tcutil_unify(_tmp22A,(void*)_tmp228))return _tmp22B;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1344
return Cyc_Absyn_uint_typ;}
# 1347
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1354
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1356
({unsigned int _tmp6C7=loc;struct Cyc_Tcenv_Tenv*_tmp6C6=te;struct Cyc_List_List*_tmp6C5=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp6C7,_tmp6C6,_tmp6C5,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp22C=fs;
void*_tmp22D=t_orig;
for(0;_tmp22C != 0;_tmp22C=_tmp22C->tl){
void*_tmp22E=(void*)_tmp22C->hd;
void*_tmp22F=_tmp22E;unsigned int _tmp269;struct _dyneither_ptr*_tmp268;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp22F)->tag == 0U){_LL1: _tmp268=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp22F)->f1;_LL2: {
# 1364
int bad_type=1;
{void*_tmp230=Cyc_Tcutil_compress(_tmp22D);void*_tmp231=_tmp230;struct Cyc_List_List*_tmp239;struct Cyc_Absyn_Aggrdecl**_tmp238;switch(*((int*)_tmp231)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp231)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp238=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp231)->f1).aggr_info).KnownAggr).val;_LL7:
# 1367
 if((*_tmp238)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp268,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp238)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp234=({struct Cyc_String_pa_PrintArg_struct _tmp520;_tmp520.tag=0U,_tmp520.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp268);_tmp520;});void*_tmp232[1U];_tmp232[0]=& _tmp234;({unsigned int _tmp6C9=loc;struct _dyneither_ptr _tmp6C8=({const char*_tmp233="no field of struct/union has name %s";_tag_dyneither(_tmp233,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6C9,_tmp6C8,_tag_dyneither(_tmp232,sizeof(void*),1U));});});else{
# 1372
_tmp22D=t2;}
bad_type=0;
goto _LL5;};}else{goto _LLA;}case 12U: _LL8: _tmp239=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp231)->f2;_LL9: {
# 1376
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp268,_tmp239);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp237=({struct Cyc_String_pa_PrintArg_struct _tmp521;_tmp521.tag=0U,_tmp521.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp268);_tmp521;});void*_tmp235[1U];_tmp235[0]=& _tmp237;({unsigned int _tmp6CB=loc;struct _dyneither_ptr _tmp6CA=({const char*_tmp236="no field of struct/union has name %s";_tag_dyneither(_tmp236,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp6CB,_tmp6CA,_tag_dyneither(_tmp235,sizeof(void*),1U));});});else{
# 1380
_tmp22D=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1385
if(bad_type){
if(_tmp22C == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp23C=({struct Cyc_String_pa_PrintArg_struct _tmp522;_tmp522.tag=0U,({struct _dyneither_ptr _tmp6CC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp22D));_tmp522.f1=_tmp6CC;});_tmp522;});void*_tmp23A[1U];_tmp23A[0]=& _tmp23C;({unsigned int _tmp6CE=loc;struct _dyneither_ptr _tmp6CD=({const char*_tmp23B="%s is not a known struct/union type";_tag_dyneither(_tmp23B,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6CE,_tmp6CD,_tag_dyneither(_tmp23A,sizeof(void*),1U));});});else{
# 1389
struct _dyneither_ptr _tmp23D=({struct Cyc_String_pa_PrintArg_struct _tmp250=({struct Cyc_String_pa_PrintArg_struct _tmp529;_tmp529.tag=0U,({struct _dyneither_ptr _tmp6CF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp529.f1=_tmp6CF;});_tmp529;});void*_tmp24E[1U];_tmp24E[0]=& _tmp250;({struct _dyneither_ptr _tmp6D0=({const char*_tmp24F="(%s)";_tag_dyneither(_tmp24F,sizeof(char),5U);});Cyc_aprintf(_tmp6D0,_tag_dyneither(_tmp24E,sizeof(void*),1U));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp22C;x=x->tl){
void*_tmp23E=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp23F=_tmp23E;unsigned int _tmp249;struct _dyneither_ptr*_tmp248;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp23F)->tag == 0U){_LLD: _tmp248=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp23F)->f1;_LLE:
# 1394
({struct _dyneither_ptr _tmp6D2=({struct Cyc_String_pa_PrintArg_struct _tmp242=({struct Cyc_String_pa_PrintArg_struct _tmp524;_tmp524.tag=0U,_tmp524.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp23D);_tmp524;});struct Cyc_String_pa_PrintArg_struct _tmp243=({struct Cyc_String_pa_PrintArg_struct _tmp523;_tmp523.tag=0U,_tmp523.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp248);_tmp523;});void*_tmp240[2U];_tmp240[0]=& _tmp242,_tmp240[1]=& _tmp243;({struct _dyneither_ptr _tmp6D1=({const char*_tmp241="%s.%s";_tag_dyneither(_tmp241,sizeof(char),6U);});Cyc_aprintf(_tmp6D1,_tag_dyneither(_tmp240,sizeof(void*),2U));});});_tmp23D=_tmp6D2;});goto _LLC;}else{_LLF: _tmp249=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp23F)->f1;_LL10:
# 1396
({struct _dyneither_ptr _tmp6D4=({struct Cyc_String_pa_PrintArg_struct _tmp246=({struct Cyc_String_pa_PrintArg_struct _tmp526;_tmp526.tag=0U,_tmp526.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp23D);_tmp526;});struct Cyc_Int_pa_PrintArg_struct _tmp247=({struct Cyc_Int_pa_PrintArg_struct _tmp525;_tmp525.tag=1U,_tmp525.f1=(unsigned long)((int)_tmp249);_tmp525;});void*_tmp244[2U];_tmp244[0]=& _tmp246,_tmp244[1]=& _tmp247;({struct _dyneither_ptr _tmp6D3=({const char*_tmp245="%s.%d";_tag_dyneither(_tmp245,sizeof(char),6U);});Cyc_aprintf(_tmp6D3,_tag_dyneither(_tmp244,sizeof(void*),2U));});});_tmp23D=_tmp6D4;});goto _LLC;}_LLC:;}
# 1398
({struct Cyc_String_pa_PrintArg_struct _tmp24C=({struct Cyc_String_pa_PrintArg_struct _tmp528;_tmp528.tag=0U,_tmp528.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp23D);_tmp528;});struct Cyc_String_pa_PrintArg_struct _tmp24D=({struct Cyc_String_pa_PrintArg_struct _tmp527;_tmp527.tag=0U,({struct _dyneither_ptr _tmp6D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp22D));_tmp527.f1=_tmp6D5;});_tmp527;});void*_tmp24A[2U];_tmp24A[0]=& _tmp24C,_tmp24A[1]=& _tmp24D;({unsigned int _tmp6D7=loc;struct _dyneither_ptr _tmp6D6=({const char*_tmp24B="%s == %s is not a struct/union type";_tag_dyneither(_tmp24B,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6D7,_tmp6D6,_tag_dyneither(_tmp24A,sizeof(void*),2U));});});}}
# 1401
goto _LL0;}}else{_LL3: _tmp269=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp22F)->f1;_LL4: {
# 1403
int bad_type=1;
{void*_tmp251=Cyc_Tcutil_compress(_tmp22D);void*_tmp252=_tmp251;struct Cyc_Absyn_Datatypefield*_tmp264;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp262;struct Cyc_Absyn_Aggrdecl**_tmp261;switch(*((int*)_tmp252)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp252)->f1).aggr_info).KnownAggr).tag == 2){_LL12: _tmp261=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp252)->f1).aggr_info).KnownAggr).val;_LL13:
# 1406
 if((*_tmp261)->impl == 0)
goto _LL11;
_tmp262=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp261)->impl))->fields;goto _LL15;}else{goto _LL1A;}case 12U: _LL14: _tmp262=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp252)->f2;_LL15:
# 1410
 if(({int _tmp6D8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp262);_tmp6D8 <= _tmp269;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp255=({struct Cyc_Int_pa_PrintArg_struct _tmp52B;_tmp52B.tag=1U,({
unsigned long _tmp6D9=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp262);_tmp52B.f1=_tmp6D9;});_tmp52B;});struct Cyc_Int_pa_PrintArg_struct _tmp256=({struct Cyc_Int_pa_PrintArg_struct _tmp52A;_tmp52A.tag=1U,_tmp52A.f1=(unsigned long)((int)_tmp269);_tmp52A;});void*_tmp253[2U];_tmp253[0]=& _tmp255,_tmp253[1]=& _tmp256;({unsigned int _tmp6DB=loc;struct _dyneither_ptr _tmp6DA=({const char*_tmp254="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp254,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6DB,_tmp6DA,_tag_dyneither(_tmp253,sizeof(void*),2U));});});else{
# 1414
({void*_tmp6DC=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp262,(int)_tmp269))->type;_tmp22D=_tmp6DC;});}
bad_type=0;
goto _LL11;case 10U: _LL16: _tmp263=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp252)->f1;_LL17:
# 1418
 if(({int _tmp6DD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp263);_tmp6DD <= _tmp269;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp259=({struct Cyc_Int_pa_PrintArg_struct _tmp52D;_tmp52D.tag=1U,({
unsigned long _tmp6DE=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp263);_tmp52D.f1=_tmp6DE;});_tmp52D;});struct Cyc_Int_pa_PrintArg_struct _tmp25A=({struct Cyc_Int_pa_PrintArg_struct _tmp52C;_tmp52C.tag=1U,_tmp52C.f1=(unsigned long)((int)_tmp269);_tmp52C;});void*_tmp257[2U];_tmp257[0]=& _tmp259,_tmp257[1]=& _tmp25A;({unsigned int _tmp6E0=loc;struct _dyneither_ptr _tmp6DF=({const char*_tmp258="tuple has too few components: %d <= %d";_tag_dyneither(_tmp258,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6E0,_tmp6DF,_tag_dyneither(_tmp257,sizeof(void*),2U));});});else{
# 1422
({void*_tmp6E1=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp263,(int)_tmp269)).f2;_tmp22D=_tmp6E1;});}
bad_type=0;
goto _LL11;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp252)->f1).field_info).KnownDatatypefield).tag == 2){_LL18: _tmp264=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp252)->f1).field_info).KnownDatatypefield).val).f2;_LL19:
# 1426
 if(({int _tmp6E2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp264->typs);_tmp6E2 < _tmp269;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp25D=({struct Cyc_Int_pa_PrintArg_struct _tmp52F;_tmp52F.tag=1U,({
unsigned long _tmp6E3=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp264->typs);_tmp52F.f1=_tmp6E3;});_tmp52F;});struct Cyc_Int_pa_PrintArg_struct _tmp25E=({struct Cyc_Int_pa_PrintArg_struct _tmp52E;_tmp52E.tag=1U,_tmp52E.f1=(unsigned long)((int)_tmp269);_tmp52E;});void*_tmp25B[2U];_tmp25B[0]=& _tmp25D,_tmp25B[1]=& _tmp25E;({unsigned int _tmp6E5=loc;struct _dyneither_ptr _tmp6E4=({const char*_tmp25C="datatype field has too few components: %d < %d";_tag_dyneither(_tmp25C,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6E5,_tmp6E4,_tag_dyneither(_tmp25B,sizeof(void*),2U));});});else{
# 1430
if(_tmp269 != 0)
({void*_tmp6E6=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp264->typs,(int)(_tmp269 - 1))).f2;_tmp22D=_tmp6E6;});else{
if(_tmp22C->tl != 0)
({void*_tmp25F=0U;({unsigned int _tmp6E8=loc;struct _dyneither_ptr _tmp6E7=({const char*_tmp260="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp260,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6E8,_tmp6E7,_tag_dyneither(_tmp25F,sizeof(void*),0U));});});}}
# 1435
bad_type=0;
goto _LL11;}else{goto _LL1A;}default: _LL1A: _LL1B:
 goto _LL11;}_LL11:;}
# 1439
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp267=({struct Cyc_String_pa_PrintArg_struct _tmp530;_tmp530.tag=0U,({struct _dyneither_ptr _tmp6E9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp22D));_tmp530.f1=_tmp6E9;});_tmp530;});void*_tmp265[1U];_tmp265[0]=& _tmp267;({unsigned int _tmp6EB=loc;struct _dyneither_ptr _tmp6EA=({const char*_tmp266="%s is not a known type";_tag_dyneither(_tmp266,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6EB,_tmp6EA,_tag_dyneither(_tmp265,sizeof(void*),1U));});});
goto _LL0;}}_LL0:;}
# 1444
return Cyc_Absyn_uint_typ;};}
# 1448
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp26A=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp26A,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp26B=t;void*_tmp27E;void*_tmp27D;union Cyc_Absyn_Constraint*_tmp27C;union Cyc_Absyn_Constraint*_tmp27B;switch(*((int*)_tmp26B)){case 1U: _LL1: _LL2: {
# 1454
struct Cyc_List_List*_tmp26C=Cyc_Tcenv_lookup_type_vars(_tmp26A);
void*_tmp26D=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp275=_cycalloc(sizeof(*_tmp275));_tmp275->v=_tmp26C;_tmp275;}));
void*_tmp26E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->v=_tmp26C;_tmp274;}));
union Cyc_Absyn_Constraint*_tmp26F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp270=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp271=({struct Cyc_Absyn_PtrAtts _tmp531;_tmp531.rgn=_tmp26E,({union Cyc_Absyn_Constraint*_tmp6EC=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp531.nullable=_tmp6EC;}),_tmp531.bounds=_tmp26F,_tmp531.zero_term=_tmp270,_tmp531.ptrloc=0;_tmp531;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp272=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->tag=5U,(_tmp273->f1).elt_typ=_tmp26D,({struct Cyc_Absyn_Tqual _tmp6ED=Cyc_Absyn_empty_tqual(0U);(_tmp273->f1).elt_tq=_tmp6ED;}),(_tmp273->f1).ptr_atts=_tmp271;_tmp273;});
Cyc_Tcutil_unify(t,(void*)_tmp272);
_tmp27E=_tmp26D;_tmp27D=_tmp26E;_tmp27C=_tmp26F;_tmp27B=_tmp270;goto _LL4;}case 5U: _LL3: _tmp27E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26B)->f1).elt_typ;_tmp27D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26B)->f1).ptr_atts).rgn;_tmp27C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26B)->f1).ptr_atts).bounds;_tmp27B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp26B)->f1).ptr_atts).zero_term;_LL4:
# 1464
 Cyc_Tcenv_check_rgn_accessible(_tmp26A,loc,_tmp27D);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp27C);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp27E),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp26A))
({void*_tmp276=0U;({unsigned int _tmp6EF=loc;struct _dyneither_ptr _tmp6EE=({const char*_tmp277="can't dereference abstract pointer type";_tag_dyneither(_tmp277,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6EF,_tmp6EE,_tag_dyneither(_tmp276,sizeof(void*),0U));});});
return _tmp27E;default: _LL5: _LL6:
# 1470
 return({struct Cyc_String_pa_PrintArg_struct _tmp27A=({struct Cyc_String_pa_PrintArg_struct _tmp532;_tmp532.tag=0U,({struct _dyneither_ptr _tmp6F0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp532.f1=_tmp6F0;});_tmp532;});void*_tmp278[1U];_tmp278[0]=& _tmp27A;({struct Cyc_Tcenv_Tenv*_tmp6F4=_tmp26A;unsigned int _tmp6F3=loc;void**_tmp6F2=topt;struct _dyneither_ptr _tmp6F1=({const char*_tmp279="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp279,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp6F4,_tmp6F3,_tmp6F2,_tmp6F1,_tag_dyneither(_tmp278,sizeof(void*),1U));});});}_LL0:;};}
# 1475
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1479
({struct Cyc_Tcenv_Tenv*_tmp6F5=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6F5,0,e);});
# 1481
({int _tmp6F6=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6F6;});{
void*_tmp27F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp280=_tmp27F;enum Cyc_Absyn_AggrKind _tmp29A;struct Cyc_List_List*_tmp299;struct Cyc_Absyn_Aggrdecl*_tmp298;struct Cyc_List_List*_tmp297;switch(*((int*)_tmp280)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp280)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp298=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp280)->f1).aggr_info).KnownAggr).val;_tmp297=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp280)->f1).targs;_LL2: {
# 1484
struct Cyc_Absyn_Aggrfield*_tmp281=Cyc_Absyn_lookup_decl_field(_tmp298,f);
if(_tmp281 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp284=({struct Cyc_String_pa_PrintArg_struct _tmp534;_tmp534.tag=0U,({
struct _dyneither_ptr _tmp6F7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp298->name));_tmp534.f1=_tmp6F7;});_tmp534;});struct Cyc_String_pa_PrintArg_struct _tmp285=({struct Cyc_String_pa_PrintArg_struct _tmp533;_tmp533.tag=0U,_tmp533.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp533;});void*_tmp282[2U];_tmp282[0]=& _tmp284,_tmp282[1]=& _tmp285;({struct Cyc_Tcenv_Tenv*_tmp6FB=te;unsigned int _tmp6FA=loc;void**_tmp6F9=topt;struct _dyneither_ptr _tmp6F8=({const char*_tmp283="%s has no %s member";_tag_dyneither(_tmp283,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp6FB,_tmp6FA,_tmp6F9,_tmp6F8,_tag_dyneither(_tmp282,sizeof(void*),2U));});});
# 1489
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp298->impl))->tagged)*is_tagged=1;{
void*t2=_tmp281->type;
if(_tmp297 != 0){
struct Cyc_List_List*_tmp286=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp298->tvs,_tmp297);
({void*_tmp6FC=Cyc_Tcutil_substitute(_tmp286,_tmp281->type);t2=_tmp6FC;});}
# 1497
if((((_tmp298->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp298->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp281->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp535;_tmp535.tag=0U,_tmp535.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp535;});void*_tmp287[1U];_tmp287[0]=& _tmp289;({struct Cyc_Tcenv_Tenv*_tmp700=te;unsigned int _tmp6FF=loc;void**_tmp6FE=topt;struct _dyneither_ptr _tmp6FD=({const char*_tmp288="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp288,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp700,_tmp6FF,_tmp6FE,_tmp6FD,_tag_dyneither(_tmp287,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp298->impl))->exist_vars != 0){
# 1502
if(!({void*_tmp701=t2;Cyc_Tcutil_unify(_tmp701,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp28C=({struct Cyc_String_pa_PrintArg_struct _tmp536;_tmp536.tag=0U,_tmp536.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp536;});void*_tmp28A[1U];_tmp28A[0]=& _tmp28C;({struct Cyc_Tcenv_Tenv*_tmp705=te;unsigned int _tmp704=loc;void**_tmp703=topt;struct _dyneither_ptr _tmp702=({const char*_tmp28B="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp28B,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp705,_tmp704,_tmp703,_tmp702,_tag_dyneither(_tmp28A,sizeof(void*),1U));});});}
# 1505
return t2;};}}else{goto _LL5;}case 12U: _LL3: _tmp29A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp280)->f1;_tmp299=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp280)->f2;_LL4: {
# 1507
struct Cyc_Absyn_Aggrfield*_tmp28D=Cyc_Absyn_lookup_field(_tmp299,f);
if(_tmp28D == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp290=({struct Cyc_String_pa_PrintArg_struct _tmp537;_tmp537.tag=0U,_tmp537.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp537;});void*_tmp28E[1U];_tmp28E[0]=& _tmp290;({struct Cyc_Tcenv_Tenv*_tmp709=te;unsigned int _tmp708=loc;void**_tmp707=topt;struct _dyneither_ptr _tmp706=({const char*_tmp28F="type has no %s member";_tag_dyneither(_tmp28F,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp709,_tmp708,_tmp707,_tmp706,_tag_dyneither(_tmp28E,sizeof(void*),1U));});});
# 1512
if(((_tmp29A == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp28D->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp28D->requires_clause == 0)
# 1514
return({struct Cyc_String_pa_PrintArg_struct _tmp293=({struct Cyc_String_pa_PrintArg_struct _tmp538;_tmp538.tag=0U,_tmp538.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp538;});void*_tmp291[1U];_tmp291[0]=& _tmp293;({struct Cyc_Tcenv_Tenv*_tmp70D=te;unsigned int _tmp70C=loc;void**_tmp70B=topt;struct _dyneither_ptr _tmp70A=({const char*_tmp292="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp292,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp70D,_tmp70C,_tmp70B,_tmp70A,_tag_dyneither(_tmp291,sizeof(void*),1U));});});
return _tmp28D->type;}default: _LL5: _LL6:
# 1517
 return({struct Cyc_String_pa_PrintArg_struct _tmp296=({struct Cyc_String_pa_PrintArg_struct _tmp539;_tmp539.tag=0U,({
struct _dyneither_ptr _tmp70E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp539.f1=_tmp70E;});_tmp539;});void*_tmp294[1U];_tmp294[0]=& _tmp296;({struct Cyc_Tcenv_Tenv*_tmp712=te;unsigned int _tmp711=loc;void**_tmp710=topt;struct _dyneither_ptr _tmp70F=({const char*_tmp295="expecting struct or union, found %s";_tag_dyneither(_tmp295,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp712,_tmp711,_tmp710,_tmp70F,_tag_dyneither(_tmp294,sizeof(void*),1U));});});}_LL0:;};}
# 1523
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1526
({struct Cyc_Tcenv_Tenv*_tmp713=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp713,0,e);});
# 1528
({int _tmp714=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp714;});
{void*_tmp29B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp29C=_tmp29B;void*_tmp2BF;void*_tmp2BE;union Cyc_Absyn_Constraint*_tmp2BD;union Cyc_Absyn_Constraint*_tmp2BC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->tag == 5U){_LL1: _tmp2BF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->f1).elt_typ;_tmp2BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->f1).ptr_atts).rgn;_tmp2BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->f1).ptr_atts).bounds;_tmp2BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->f1).ptr_atts).zero_term;_LL2:
# 1531
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2BD);
{void*_tmp29D=Cyc_Tcutil_compress(_tmp2BF);void*_tmp29E=_tmp29D;enum Cyc_Absyn_AggrKind _tmp2BB;struct Cyc_List_List*_tmp2BA;struct Cyc_Absyn_Aggrdecl*_tmp2B9;struct Cyc_List_List*_tmp2B8;switch(*((int*)_tmp29E)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp29E)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp2B9=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp29E)->f1).aggr_info).KnownAggr).val;_tmp2B8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp29E)->f1).targs;_LL7: {
# 1534
struct Cyc_Absyn_Aggrfield*_tmp29F=Cyc_Absyn_lookup_decl_field(_tmp2B9,f);
if(_tmp29F == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2A2=({struct Cyc_String_pa_PrintArg_struct _tmp53B;_tmp53B.tag=0U,({
struct _dyneither_ptr _tmp715=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp2B9->name));_tmp53B.f1=_tmp715;});_tmp53B;});struct Cyc_String_pa_PrintArg_struct _tmp2A3=({struct Cyc_String_pa_PrintArg_struct _tmp53A;_tmp53A.tag=0U,_tmp53A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp53A;});void*_tmp2A0[2U];_tmp2A0[0]=& _tmp2A2,_tmp2A0[1]=& _tmp2A3;({struct Cyc_Tcenv_Tenv*_tmp719=te;unsigned int _tmp718=loc;void**_tmp717=topt;struct _dyneither_ptr _tmp716=({const char*_tmp2A1="type %s has no %s member";_tag_dyneither(_tmp2A1,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp719,_tmp718,_tmp717,_tmp716,_tag_dyneither(_tmp2A0,sizeof(void*),2U));});});
# 1539
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B9->impl))->tagged)*is_tagged=1;{
void*t3=_tmp29F->type;
if(_tmp2B8 != 0){
struct Cyc_List_List*_tmp2A4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2B9->tvs,_tmp2B8);
({void*_tmp71A=Cyc_Tcutil_substitute(_tmp2A4,_tmp29F->type);t3=_tmp71A;});}{
# 1545
struct Cyc_Absyn_Kind*_tmp2A5=Cyc_Tcutil_typ_kind(t3);
# 1548
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2A5) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2A6=Cyc_Tcutil_compress(t3);void*_tmp2A7=_tmp2A6;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2A7)->tag == 8U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1552
 return({struct Cyc_String_pa_PrintArg_struct _tmp2AA=({struct Cyc_String_pa_PrintArg_struct _tmp53C;_tmp53C.tag=0U,_tmp53C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp53C;});void*_tmp2A8[1U];_tmp2A8[0]=& _tmp2AA;({struct Cyc_Tcenv_Tenv*_tmp71E=te;unsigned int _tmp71D=loc;void**_tmp71C=topt;struct _dyneither_ptr _tmp71B=({const char*_tmp2A9="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2A9,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp71E,_tmp71D,_tmp71C,_tmp71B,_tag_dyneither(_tmp2A8,sizeof(void*),1U));});});}_LLC:;}
# 1557
if((((_tmp2B9->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B9->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp29F->requires_clause == 0)
# 1560
return({struct Cyc_String_pa_PrintArg_struct _tmp2AD=({struct Cyc_String_pa_PrintArg_struct _tmp53D;_tmp53D.tag=0U,_tmp53D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp53D;});void*_tmp2AB[1U];_tmp2AB[0]=& _tmp2AD;({struct Cyc_Tcenv_Tenv*_tmp722=te;unsigned int _tmp721=loc;void**_tmp720=topt;struct _dyneither_ptr _tmp71F=({const char*_tmp2AC="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2AC,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp722,_tmp721,_tmp720,_tmp71F,_tag_dyneither(_tmp2AB,sizeof(void*),1U));});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B9->impl))->exist_vars != 0){
if(!({void*_tmp723=t3;Cyc_Tcutil_unify(_tmp723,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2B0=({struct Cyc_String_pa_PrintArg_struct _tmp53E;_tmp53E.tag=0U,_tmp53E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp53E;});void*_tmp2AE[1U];_tmp2AE[0]=& _tmp2B0;({struct Cyc_Tcenv_Tenv*_tmp727=te;unsigned int _tmp726=loc;void**_tmp725=topt;struct _dyneither_ptr _tmp724=({const char*_tmp2AF="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2AF,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp727,_tmp726,_tmp725,_tmp724,_tag_dyneither(_tmp2AE,sizeof(void*),1U));});});}
# 1567
return t3;};};}}else{goto _LLA;}case 12U: _LL8: _tmp2BB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29E)->f1;_tmp2BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29E)->f2;_LL9: {
# 1569
struct Cyc_Absyn_Aggrfield*_tmp2B1=Cyc_Absyn_lookup_field(_tmp2BA,f);
if(_tmp2B1 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2B4=({struct Cyc_String_pa_PrintArg_struct _tmp53F;_tmp53F.tag=0U,_tmp53F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp53F;});void*_tmp2B2[1U];_tmp2B2[0]=& _tmp2B4;({struct Cyc_Tcenv_Tenv*_tmp72B=te;unsigned int _tmp72A=loc;void**_tmp729=topt;struct _dyneither_ptr _tmp728=({const char*_tmp2B3="type has no %s field";_tag_dyneither(_tmp2B3,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp72B,_tmp72A,_tmp729,_tmp728,_tag_dyneither(_tmp2B2,sizeof(void*),1U));});});
# 1574
if((_tmp2BB == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2B1->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2B7=({struct Cyc_String_pa_PrintArg_struct _tmp540;_tmp540.tag=0U,_tmp540.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp540;});void*_tmp2B5[1U];_tmp2B5[0]=& _tmp2B7;({struct Cyc_Tcenv_Tenv*_tmp72F=te;unsigned int _tmp72E=loc;void**_tmp72D=topt;struct _dyneither_ptr _tmp72C=({const char*_tmp2B6="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2B6,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp72F,_tmp72E,_tmp72D,_tmp72C,_tag_dyneither(_tmp2B5,sizeof(void*),1U));});});
return _tmp2B1->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1579
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1582
return({struct Cyc_String_pa_PrintArg_struct _tmp2C2=({struct Cyc_String_pa_PrintArg_struct _tmp541;_tmp541.tag=0U,({
struct _dyneither_ptr _tmp730=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp541.f1=_tmp730;});_tmp541;});void*_tmp2C0[1U];_tmp2C0[0]=& _tmp2C2;({struct Cyc_Tcenv_Tenv*_tmp734=te;unsigned int _tmp733=loc;void**_tmp732=topt;struct _dyneither_ptr _tmp731=({const char*_tmp2C1="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2C1,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp734,_tmp733,_tmp732,_tmp731,_tag_dyneither(_tmp2C0,sizeof(void*),1U));});});}
# 1587
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1589
struct _tuple15 _tmp2C3=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2C4=_tmp2C3;unsigned int _tmp2D2;int _tmp2D1;_LL1: _tmp2D2=_tmp2C4.f1;_tmp2D1=_tmp2C4.f2;_LL2:;
if(!_tmp2D1)
return({void*_tmp2C5=0U;({struct Cyc_Tcenv_Tenv*_tmp737=te;unsigned int _tmp736=loc;struct _dyneither_ptr _tmp735=({const char*_tmp2C6="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2C6,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp737,_tmp736,0,_tmp735,_tag_dyneither(_tmp2C5,sizeof(void*),0U));});});{
# 1593
struct _handler_cons _tmp2C7;_push_handler(& _tmp2C7);{int _tmp2C9=0;if(setjmp(_tmp2C7.handler))_tmp2C9=1;if(!_tmp2C9){
{void*_tmp2CA=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2D2)).f2;_npop_handler(0U);return _tmp2CA;};_pop_handler();}else{void*_tmp2C8=(void*)_exn_thrown;void*_tmp2CB=_tmp2C8;void*_tmp2D0;if(((struct Cyc_List_Nth_exn_struct*)_tmp2CB)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1596
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2CE=({struct Cyc_Int_pa_PrintArg_struct _tmp543;_tmp543.tag=1U,_tmp543.f1=(unsigned long)((int)_tmp2D2);_tmp543;});struct Cyc_Int_pa_PrintArg_struct _tmp2CF=({struct Cyc_Int_pa_PrintArg_struct _tmp542;_tmp542.tag=1U,({
unsigned long _tmp738=(unsigned long)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp542.f1=_tmp738;});_tmp542;});void*_tmp2CC[2U];_tmp2CC[0]=& _tmp2CE,_tmp2CC[1]=& _tmp2CF;({struct Cyc_Tcenv_Tenv*_tmp73B=te;unsigned int _tmp73A=loc;struct _dyneither_ptr _tmp739=({const char*_tmp2CD="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2CD,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp73B,_tmp73A,0,_tmp739,_tag_dyneither(_tmp2CC,sizeof(void*),2U));});});}else{_LL6: _tmp2D0=_tmp2CB;_LL7:(int)_rethrow(_tmp2D0);}_LL3:;}};};}
# 1601
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1603
struct Cyc_Tcenv_Tenv*_tmp2D3=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp73C=Cyc_Tcenv_clear_abstract_val_ok(_tmp2D3);Cyc_Tcexp_tcExp(_tmp73C,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp73D=Cyc_Tcenv_clear_abstract_val_ok(_tmp2D3);Cyc_Tcexp_tcExp(_tmp73D,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2D3,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2D6=({struct Cyc_String_pa_PrintArg_struct _tmp544;_tmp544.tag=0U,({
struct _dyneither_ptr _tmp73E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp544.f1=_tmp73E;});_tmp544;});void*_tmp2D4[1U];_tmp2D4[0]=& _tmp2D6;({struct Cyc_Tcenv_Tenv*_tmp742=_tmp2D3;unsigned int _tmp741=e2->loc;void**_tmp740=topt;struct _dyneither_ptr _tmp73F=({const char*_tmp2D5="expecting int subscript, found %s";_tag_dyneither(_tmp2D5,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp742,_tmp741,_tmp740,_tmp73F,_tag_dyneither(_tmp2D4,sizeof(void*),1U));});});{
# 1613
void*_tmp2D7=t1;struct Cyc_List_List*_tmp2F5;void*_tmp2F4;struct Cyc_Absyn_Tqual _tmp2F3;void*_tmp2F2;union Cyc_Absyn_Constraint*_tmp2F1;union Cyc_Absyn_Constraint*_tmp2F0;switch(*((int*)_tmp2D7)){case 5U: _LL1: _tmp2F4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).elt_typ;_tmp2F3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).elt_tq;_tmp2F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).ptr_atts).rgn;_tmp2F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).ptr_atts).bounds;_tmp2F0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).ptr_atts).zero_term;_LL2:
# 1617
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2F0)){
int emit_warning=1;
{void*_tmp2D8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp2F1);void*_tmp2D9=_tmp2D8;struct Cyc_Absyn_Exp*_tmp2E2;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D9)->tag == 1U){_LL8: _tmp2E2=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2D9)->f1;_LL9:
# 1621
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2DA=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2DB=_tmp2DA;unsigned int _tmp2E1;int _tmp2E0;_LLD: _tmp2E1=_tmp2DB.f1;_tmp2E0=_tmp2DB.f2;_LLE:;
if(_tmp2E0){
struct _tuple15 _tmp2DC=Cyc_Evexp_eval_const_uint_exp(_tmp2E2);struct _tuple15 _tmp2DD=_tmp2DC;unsigned int _tmp2DF;int _tmp2DE;_LL10: _tmp2DF=_tmp2DD.f1;_tmp2DE=_tmp2DD.f2;_LL11:;
if(_tmp2DE  && _tmp2DF > _tmp2E1)emit_warning=0;}}
# 1628
goto _LL7;}else{_LLA: _LLB:
 emit_warning=0;goto _LL7;}_LL7:;}
# 1631
if(emit_warning)
({void*_tmp2E3=0U;({unsigned int _tmp744=e2->loc;struct _dyneither_ptr _tmp743=({const char*_tmp2E4="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp2E4,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp744,_tmp743,_tag_dyneither(_tmp2E3,sizeof(void*),0U));});});}else{
# 1635
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2E5=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2E6=_tmp2E5;unsigned int _tmp2E8;int _tmp2E7;_LL13: _tmp2E8=_tmp2E6.f1;_tmp2E7=_tmp2E6.f2;_LL14:;
if(_tmp2E7)
Cyc_Tcutil_check_bound(loc,_tmp2E8,_tmp2F1);}else{
# 1641
if(Cyc_Tcutil_is_bound_one(_tmp2F1) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp2F0))
({void*_tmp2E9=0U;({unsigned int _tmp746=e1->loc;struct _dyneither_ptr _tmp745=({const char*_tmp2EA="subscript applied to pointer to one object";_tag_dyneither(_tmp2EA,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp746,_tmp745,_tag_dyneither(_tmp2E9,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2F1);}}
# 1646
Cyc_Tcenv_check_rgn_accessible(_tmp2D3,loc,_tmp2F2);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2F4),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2D3))
({void*_tmp2EB=0U;({unsigned int _tmp748=e1->loc;struct _dyneither_ptr _tmp747=({const char*_tmp2EC="can't subscript an abstract pointer";_tag_dyneither(_tmp2EC,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp748,_tmp747,_tag_dyneither(_tmp2EB,sizeof(void*),0U));});});
return _tmp2F4;case 10U: _LL3: _tmp2F5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2D7)->f1;_LL4:
 return Cyc_Tcexp_ithTupleType(_tmp2D3,loc,_tmp2F5,e2);default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp2EF=({struct Cyc_String_pa_PrintArg_struct _tmp545;_tmp545.tag=0U,({struct _dyneither_ptr _tmp749=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp545.f1=_tmp749;});_tmp545;});void*_tmp2ED[1U];_tmp2ED[0]=& _tmp2EF;({struct Cyc_Tcenv_Tenv*_tmp74D=_tmp2D3;unsigned int _tmp74C=loc;void**_tmp74B=topt;struct _dyneither_ptr _tmp74A=({const char*_tmp2EE="subscript applied to %s";_tag_dyneither(_tmp2EE,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp74D,_tmp74C,_tmp74B,_tmp74A,_tag_dyneither(_tmp2ED,sizeof(void*),1U));});});}_LL0:;};};}
# 1656
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp2F6=Cyc_Tcutil_compress(*topt);void*_tmp2F7=_tmp2F6;struct Cyc_List_List*_tmp2FB;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F7)->tag == 10U){_LL1: _tmp2FB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F7)->f1;_LL2:
# 1662
 if(({int _tmp74E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp2FB);_tmp74E != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1665
goto _LL0;
# 1667
for(0;es != 0;(es=es->tl,_tmp2FB=_tmp2FB->tl)){
int bogus=0;
void*_tmp2F8=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp2FB))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp74F=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp74F,& _tmp2F8,(struct Cyc_Absyn_Exp*)es->hd);});
# 1672
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp2FB->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp751=({struct Cyc_List_List*_tmp2FA=_cycalloc(sizeof(*_tmp2FA));({struct _tuple17*_tmp750=({struct _tuple17*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));_tmp2F9->f1=(*((struct _tuple17*)_tmp2FB->hd)).f1,_tmp2F9->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2F9;});_tmp2FA->hd=_tmp750;}),_tmp2FA->tl=fields;_tmp2FA;});fields=_tmp751;});}
# 1675
done=1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1679
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp752=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp752,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp755=({struct Cyc_List_List*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));({struct _tuple17*_tmp754=({struct _tuple17*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));({struct Cyc_Absyn_Tqual _tmp753=Cyc_Absyn_empty_tqual(0U);_tmp2FC->f1=_tmp753;}),_tmp2FC->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp2FC;});_tmp2FD->hd=_tmp754;}),_tmp2FD->tl=fields;_tmp2FD;});fields=_tmp755;});}
# 1684
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE->tag=10U,({struct Cyc_List_List*_tmp756=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp2FE->f1=_tmp756;});_tmp2FE;});}
# 1688
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1691
return({void*_tmp2FF=0U;({struct Cyc_Tcenv_Tenv*_tmp75A=te;unsigned int _tmp759=loc;void**_tmp758=topt;struct _dyneither_ptr _tmp757=({const char*_tmp300="tcCompoundLit";_tag_dyneither(_tmp300,sizeof(char),14U);});Cyc_Tcexp_expr_err(_tmp75A,_tmp759,_tmp758,_tmp757,_tag_dyneither(_tmp2FF,sizeof(void*),0U));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1701 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1704
void*res_t2;
int _tmp301=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp302=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->tag=0U,({union Cyc_Absyn_Cnst _tmp75B=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp301);_tmp318->f1=_tmp75B;});_tmp318;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp302,loc);
# 1712
if(zero_term){
struct Cyc_Absyn_Exp*_tmp303=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp301 - 1);
if(!Cyc_Tcutil_is_zero(_tmp303))
({void*_tmp304=0U;({unsigned int _tmp75D=_tmp303->loc;struct _dyneither_ptr _tmp75C=({const char*_tmp305="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp305,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp75D,_tmp75C,_tag_dyneither(_tmp304,sizeof(void*),0U));});});}
# 1717
sz_exp->topt=Cyc_Absyn_uint_typ;
({void*_tmp761=({void*_tmp760=res;struct Cyc_Absyn_Tqual _tmp75F=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1718
struct Cyc_Absyn_Exp*_tmp75E=sz_exp;Cyc_Absyn_array_typ(_tmp760,_tmp75F,_tmp75E,
# 1720
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0U);});
# 1718
res_t2=_tmp761;});
# 1722
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1725
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1727
({struct Cyc_String_pa_PrintArg_struct _tmp308=({struct Cyc_String_pa_PrintArg_struct _tmp546;_tmp546.tag=0U,({
struct _dyneither_ptr _tmp762=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(res));_tmp546.f1=_tmp762;});_tmp546;});void*_tmp306[1U];_tmp306[0]=& _tmp308;({unsigned int _tmp764=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp763=({const char*_tmp307="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp307,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp764,_tmp763,_tag_dyneither(_tmp306,sizeof(void*),1U));});});
# 1730
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1735
void*_tmp309=(void*)ds->hd;void*_tmp30A=_tmp309;struct Cyc_Absyn_Exp*_tmp317;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp30A)->tag == 1U){_LL1: _LL2:
# 1737
({void*_tmp30B=0U;({unsigned int _tmp766=loc;struct _dyneither_ptr _tmp765=({const char*_tmp30C="only array index designators are supported";_tag_dyneither(_tmp30C,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp766,_tmp765,_tag_dyneither(_tmp30B,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp317=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp30A)->f1;_LL4:
# 1740
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp317);{
struct _tuple15 _tmp30D=Cyc_Evexp_eval_const_uint_exp(_tmp317);struct _tuple15 _tmp30E=_tmp30D;unsigned int _tmp316;int _tmp315;_LL6: _tmp316=_tmp30E.f1;_tmp315=_tmp30E.f2;_LL7:;
if(!_tmp315)
({void*_tmp30F=0U;({unsigned int _tmp768=_tmp317->loc;struct _dyneither_ptr _tmp767=({const char*_tmp310="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp310,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp768,_tmp767,_tag_dyneither(_tmp30F,sizeof(void*),0U));});});else{
if(_tmp316 != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp313=({struct Cyc_Int_pa_PrintArg_struct _tmp548;_tmp548.tag=1U,_tmp548.f1=(unsigned long)offset;_tmp548;});struct Cyc_Int_pa_PrintArg_struct _tmp314=({struct Cyc_Int_pa_PrintArg_struct _tmp547;_tmp547.tag=1U,_tmp547.f1=(unsigned long)((int)_tmp316);_tmp547;});void*_tmp311[2U];_tmp311[0]=& _tmp313,_tmp311[1]=& _tmp314;({unsigned int _tmp76A=_tmp317->loc;struct _dyneither_ptr _tmp769=({const char*_tmp312="expecting index designator %d but found %d";_tag_dyneither(_tmp312,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp76A,_tmp769,_tag_dyneither(_tmp311,sizeof(void*),2U));});});}
# 1747
goto _LL0;};}_LL0:;}}}
# 1751
return res_t2;}
# 1755
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1758
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp319=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp31A=_tmp319;void*_tmp31F;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp31A)->tag == 19U){_LL1: _tmp31F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp31A)->f1;_LL2:
# 1763
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp31B=({void*_tmp76B=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp76B,Cyc_Absyn_valueof_exp(_tmp31F,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp31B->topt=bound->topt;
# 1768
bound=_tmp31B;}
# 1770
goto _LL0;}else{_LL3: _LL4:
# 1772
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp549;_tmp549.tag=0U,({
struct _dyneither_ptr _tmp76C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp549.f1=_tmp76C;});_tmp549;});void*_tmp31C[1U];_tmp31C[0]=& _tmp31E;({unsigned int _tmp76E=bound->loc;struct _dyneither_ptr _tmp76D=({const char*_tmp31D="expecting unsigned int, found %s";_tag_dyneither(_tmp31D,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp76E,_tmp76D,_tag_dyneither(_tmp31C,sizeof(void*),1U));});});}_LL0:;}
# 1777
if(!(vd->tq).real_const)
({void*_tmp320=0U;({struct _dyneither_ptr _tmp76F=({const char*_tmp321="comprehension index variable is not declared const!";_tag_dyneither(_tmp321,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp76F,_tag_dyneither(_tmp320,sizeof(void*),0U));});});
# 1780
if(te->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp770=Cyc_Tcenv_new_block(loc,te);te=_tmp770;});{
void**_tmp322=0;
struct Cyc_Absyn_Tqual*_tmp323=0;
union Cyc_Absyn_Constraint**_tmp324=0;
# 1786
if(topt != 0){
void*_tmp325=Cyc_Tcutil_compress(*topt);void*_tmp326=_tmp325;void*_tmp331;struct Cyc_Absyn_Tqual _tmp330;struct Cyc_Absyn_Exp*_tmp32F;union Cyc_Absyn_Constraint*_tmp32E;struct Cyc_Absyn_PtrInfo _tmp32D;switch(*((int*)_tmp326)){case 5U: _LL6: _tmp32D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp326)->f1;_LL7:
# 1789
({void**_tmp771=({void**_tmp327=_cycalloc(sizeof(*_tmp327));*_tmp327=_tmp32D.elt_typ;_tmp327;});_tmp322=_tmp771;});
({struct Cyc_Absyn_Tqual*_tmp772=({struct Cyc_Absyn_Tqual*_tmp328=_cycalloc(sizeof(*_tmp328));*_tmp328=_tmp32D.elt_tq;_tmp328;});_tmp323=_tmp772;});
({union Cyc_Absyn_Constraint**_tmp773=({union Cyc_Absyn_Constraint**_tmp329=_cycalloc(sizeof(*_tmp329));*_tmp329=(_tmp32D.ptr_atts).zero_term;_tmp329;});_tmp324=_tmp773;});
goto _LL5;case 8U: _LL8: _tmp331=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).elt_type;_tmp330=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).tq;_tmp32F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).num_elts;_tmp32E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp326)->f1).zero_term;_LL9:
# 1794
({void**_tmp774=({void**_tmp32A=_cycalloc(sizeof(*_tmp32A));*_tmp32A=_tmp331;_tmp32A;});_tmp322=_tmp774;});
({struct Cyc_Absyn_Tqual*_tmp775=({struct Cyc_Absyn_Tqual*_tmp32B=_cycalloc(sizeof(*_tmp32B));*_tmp32B=_tmp330;_tmp32B;});_tmp323=_tmp775;});
({union Cyc_Absyn_Constraint**_tmp776=({union Cyc_Absyn_Constraint**_tmp32C=_cycalloc(sizeof(*_tmp32C));*_tmp32C=_tmp32E;_tmp32C;});_tmp324=_tmp776;});
goto _LL5;default: _LLA: _LLB:
# 1799
 goto _LL5;}_LL5:;}{
# 1802
void*t=Cyc_Tcexp_tcExp(te,_tmp322,body);
# 1804
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp332=0U;({unsigned int _tmp778=body->loc;struct _dyneither_ptr _tmp777=({const char*_tmp333="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp333,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp778,_tmp777,_tag_dyneither(_tmp332,sizeof(void*),0U));});});
if(te->le == 0){
# 1808
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp334=0U;({unsigned int _tmp77A=bound->loc;struct _dyneither_ptr _tmp779=({const char*_tmp335="bound is not constant";_tag_dyneither(_tmp335,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_dyneither(_tmp334,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp336=0U;({unsigned int _tmp77C=bound->loc;struct _dyneither_ptr _tmp77B=({const char*_tmp337="body is not constant";_tag_dyneither(_tmp337,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp77C,_tmp77B,_tag_dyneither(_tmp336,sizeof(void*),0U));});});}
# 1813
if(_tmp324 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp324)){
# 1816
struct Cyc_Absyn_Exp*_tmp338=Cyc_Absyn_uint_exp(1U,0U);_tmp338->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp77D=Cyc_Absyn_add_exp(bound,_tmp338,0U);bound=_tmp77D;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}
# 1820
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp339=0U;({unsigned int _tmp77F=body->loc;struct _dyneither_ptr _tmp77E=({const char*_tmp33A="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp33A,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp77F,_tmp77E,_tag_dyneither(_tmp339,sizeof(void*),0U));});});{
# 1824
void*_tmp33B=({void*_tmp782=t;struct Cyc_Absyn_Tqual _tmp781=_tmp323 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp323;struct Cyc_Absyn_Exp*_tmp780=bound;Cyc_Absyn_array_typ(_tmp782,_tmp781,_tmp780,
_tmp324 == 0?Cyc_Absyn_false_conref:*_tmp324,0U);});
return _tmp33B;};};};}
# 1830
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1833
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp33C=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp33D=_tmp33C;void*_tmp342;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp33D)->tag == 19U){_LL1: _tmp342=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp33D)->f1;_LL2:
# 1838
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp33E=({void*_tmp783=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp783,Cyc_Absyn_valueof_exp(_tmp342,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp33E->topt=bound->topt;
# 1843
bound=_tmp33E;}
# 1845
goto _LL0;}else{_LL3: _LL4:
# 1847
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp54A;_tmp54A.tag=0U,({
struct _dyneither_ptr _tmp784=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp54A.f1=_tmp784;});_tmp54A;});void*_tmp33F[1U];_tmp33F[0]=& _tmp341;({unsigned int _tmp786=bound->loc;struct _dyneither_ptr _tmp785=({const char*_tmp340="expecting unsigned int, found %s";_tag_dyneither(_tmp340,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp786,_tmp785,_tag_dyneither(_tmp33F,sizeof(void*),1U));});});}_LL0:;}{
# 1852
void**_tmp343=0;
struct Cyc_Absyn_Tqual*_tmp344=0;
union Cyc_Absyn_Constraint**_tmp345=0;
# 1856
if(topt != 0){
void*_tmp346=Cyc_Tcutil_compress(*topt);void*_tmp347=_tmp346;void*_tmp352;struct Cyc_Absyn_Tqual _tmp351;struct Cyc_Absyn_Exp*_tmp350;union Cyc_Absyn_Constraint*_tmp34F;struct Cyc_Absyn_PtrInfo _tmp34E;switch(*((int*)_tmp347)){case 5U: _LL6: _tmp34E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp347)->f1;_LL7:
# 1859
({void**_tmp787=({void**_tmp348=_cycalloc(sizeof(*_tmp348));*_tmp348=_tmp34E.elt_typ;_tmp348;});_tmp343=_tmp787;});
({struct Cyc_Absyn_Tqual*_tmp788=({struct Cyc_Absyn_Tqual*_tmp349=_cycalloc(sizeof(*_tmp349));*_tmp349=_tmp34E.elt_tq;_tmp349;});_tmp344=_tmp788;});
({union Cyc_Absyn_Constraint**_tmp789=({union Cyc_Absyn_Constraint**_tmp34A=_cycalloc(sizeof(*_tmp34A));*_tmp34A=(_tmp34E.ptr_atts).zero_term;_tmp34A;});_tmp345=_tmp789;});
goto _LL5;case 8U: _LL8: _tmp352=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).elt_type;_tmp351=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).tq;_tmp350=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).num_elts;_tmp34F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp347)->f1).zero_term;_LL9:
# 1864
({void**_tmp78A=({void**_tmp34B=_cycalloc(sizeof(*_tmp34B));*_tmp34B=_tmp352;_tmp34B;});_tmp343=_tmp78A;});
({struct Cyc_Absyn_Tqual*_tmp78B=({struct Cyc_Absyn_Tqual*_tmp34C=_cycalloc(sizeof(*_tmp34C));*_tmp34C=_tmp351;_tmp34C;});_tmp344=_tmp78B;});
({union Cyc_Absyn_Constraint**_tmp78C=({union Cyc_Absyn_Constraint**_tmp34D=_cycalloc(sizeof(*_tmp34D));*_tmp34D=_tmp34F;_tmp34D;});_tmp345=_tmp78C;});
goto _LL5;default: _LLA: _LLB:
# 1869
 goto _LL5;}_LL5:;}
# 1872
({unsigned int _tmp78F=loc;struct Cyc_Tcenv_Tenv*_tmp78E=te;struct Cyc_List_List*_tmp78D=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp78F,_tmp78E,_tmp78D,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp343 != 0)Cyc_Tcutil_unify(*_tmp343,t);
# 1875
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp353=0U;({unsigned int _tmp791=bound->loc;struct _dyneither_ptr _tmp790=({const char*_tmp354="bound is not constant";_tag_dyneither(_tmp354,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp791,_tmp790,_tag_dyneither(_tmp353,sizeof(void*),0U));});});}
# 1879
if(_tmp345 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp345)){
# 1882
struct Cyc_Absyn_Exp*_tmp355=Cyc_Absyn_uint_exp(1U,0U);_tmp355->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp792=Cyc_Absyn_add_exp(bound,_tmp355,0U);bound=_tmp792;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1886
({void*_tmp356=0U;({unsigned int _tmp794=loc;struct _dyneither_ptr _tmp793=({const char*_tmp357="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp357,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp794,_tmp793,_tag_dyneither(_tmp356,sizeof(void*),0U));});});}{
# 1889
void*_tmp358=({void*_tmp797=t;struct Cyc_Absyn_Tqual _tmp796=_tmp344 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp344;struct Cyc_Absyn_Exp*_tmp795=bound;Cyc_Absyn_array_typ(_tmp797,_tmp796,_tmp795,
_tmp345 == 0?Cyc_Absyn_false_conref:*_tmp345,0U);});
return _tmp358;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1904 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1908
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp798=({struct Cyc_Absyn_Aggrdecl**_tmp359=_cycalloc(sizeof(*_tmp359));*_tmp359=ad;_tmp359;});adptr=_tmp798;});}else{
# 1914
{struct _handler_cons _tmp35A;_push_handler(& _tmp35A);{int _tmp35C=0;if(setjmp(_tmp35A.handler))_tmp35C=1;if(!_tmp35C){({struct Cyc_Absyn_Aggrdecl**_tmp799=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp799;});
ad=*adptr;
# 1914
;_pop_handler();}else{void*_tmp35B=(void*)_exn_thrown;void*_tmp35D=_tmp35B;void*_tmp361;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp35D)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1917
({struct Cyc_String_pa_PrintArg_struct _tmp360=({struct Cyc_String_pa_PrintArg_struct _tmp54B;_tmp54B.tag=0U,({struct _dyneither_ptr _tmp79A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp54B.f1=_tmp79A;});_tmp54B;});void*_tmp35E[1U];_tmp35E[0]=& _tmp360;({unsigned int _tmp79C=loc;struct _dyneither_ptr _tmp79B=({const char*_tmp35F="unbound struct/union name %s";_tag_dyneither(_tmp35F,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp79C,_tmp79B,_tag_dyneither(_tmp35E,sizeof(void*),1U));});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL3: _tmp361=_tmp35D;_LL4:(int)_rethrow(_tmp361);}_LL0:;}};}
# 1920
*ad_opt=ad;
*tn=ad->name;}
# 1923
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp364=({struct Cyc_String_pa_PrintArg_struct _tmp54C;_tmp54C.tag=0U,({struct _dyneither_ptr _tmp79D=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp365="struct";_tag_dyneither(_tmp365,sizeof(char),7U);}):({const char*_tmp366="union";_tag_dyneither(_tmp366,sizeof(char),6U);}));_tmp54C.f1=_tmp79D;});_tmp54C;});void*_tmp362[1U];_tmp362[0]=& _tmp364;({unsigned int _tmp79F=loc;struct _dyneither_ptr _tmp79E=({const char*_tmp363="can't construct abstract %s";_tag_dyneither(_tmp363,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp79F,_tmp79E,_tag_dyneither(_tmp362,sizeof(void*),1U));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1931
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp367=Cyc_Tcenv_new_status(te);
if(_tmp367 == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp7A0=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp7A0;});else{
# 1940
({struct Cyc_Tcenv_Tenv*_tmp7A1=Cyc_Tcenv_set_new_status(_tmp367,te);te2=_tmp7A1;});}{
# 1942
struct _tuple12 _tmp368=({struct _tuple12 _tmp552;({struct Cyc_List_List*_tmp7A2=Cyc_Tcenv_lookup_type_vars(te2);_tmp552.f1=_tmp7A2;}),_tmp552.f2=Cyc_Core_heap_region;_tmp552;});
struct Cyc_List_List*_tmp369=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp368,ad->tvs);
struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp368,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp369);
struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp36A);
struct Cyc_List_List*_tmp36D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp369,_tmp36A);
void*res_typ;
# 1953
if(topt != 0){
void*_tmp36E=Cyc_Tcutil_compress(*topt);void*_tmp36F=_tmp36E;struct Cyc_Absyn_Aggrdecl**_tmp373;struct Cyc_List_List*_tmp372;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36F)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36F)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp373=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36F)->f1).aggr_info).KnownAggr).val;_tmp372=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp36F)->f1).targs;_LL7:
# 1956
 if(*_tmp373 == *adptr){
{struct Cyc_List_List*_tmp370=_tmp36B;for(0;_tmp370 != 0  && _tmp372 != 0;(
_tmp370=_tmp370->tl,_tmp372=_tmp372->tl)){
Cyc_Tcutil_unify((void*)_tmp370->hd,(void*)_tmp372->hd);}}
# 1961
res_typ=*topt;
goto _LL5;}
# 1964
goto _LL9;}else{goto _LL8;}}else{_LL8: _LL9:
# 1966
({void*_tmp7A4=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp371=_cycalloc(sizeof(*_tmp371));_tmp371->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7A3=Cyc_Absyn_KnownAggr(adptr);(_tmp371->f1).aggr_info=_tmp7A3;}),(_tmp371->f1).targs=_tmp36B;_tmp371;});res_typ=_tmp7A4;});}_LL5:;}else{
# 1969
({void*_tmp7A6=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7A5=Cyc_Absyn_KnownAggr(adptr);(_tmp374->f1).aggr_info=_tmp7A5;}),(_tmp374->f1).targs=_tmp36B;_tmp374;});res_typ=_tmp7A6;});}{
# 1972
struct Cyc_List_List*_tmp375=*ts;
struct Cyc_List_List*_tmp376=_tmp36C;
while(_tmp375 != 0  && _tmp376 != 0){
# 1976
({unsigned int _tmp7A9=loc;struct Cyc_Tcenv_Tenv*_tmp7A8=te2;struct Cyc_List_List*_tmp7A7=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp7A9,_tmp7A8,_tmp7A7,& Cyc_Tcutil_ak,1,0,(void*)_tmp375->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp375->hd);
Cyc_Tcutil_unify((void*)_tmp375->hd,(void*)_tmp376->hd);
_tmp375=_tmp375->tl;
_tmp376=_tmp376->tl;}
# 1982
if(_tmp375 != 0)
({void*_tmp377=0U;({unsigned int _tmp7AB=loc;struct _dyneither_ptr _tmp7AA=({const char*_tmp378="too many explicit witness types";_tag_dyneither(_tmp378,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp7AB,_tmp7AA,_tag_dyneither(_tmp377,sizeof(void*),0U));});});
# 1985
*ts=_tmp36C;{
# 1988
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp379=(struct _tuple19*)fields->hd;struct _tuple19*_tmp37A=_tmp379;struct Cyc_Absyn_Aggrfield*_tmp386;struct Cyc_Absyn_Exp*_tmp385;_LLB: _tmp386=_tmp37A->f1;_tmp385=_tmp37A->f2;_LLC:;{
void*_tmp37B=Cyc_Tcutil_substitute(_tmp36D,_tmp386->type);
({struct Cyc_Tcenv_Tenv*_tmp7AC=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp7AC,& _tmp37B,_tmp385);});
# 1999
if(!Cyc_Tcutil_coerce_arg(te2,_tmp385,_tmp37B,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp37E=({struct Cyc_String_pa_PrintArg_struct _tmp551;_tmp551.tag=0U,_tmp551.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp386->name);_tmp551;});struct Cyc_String_pa_PrintArg_struct _tmp37F=({struct Cyc_String_pa_PrintArg_struct _tmp550;_tmp550.tag=0U,({
struct _dyneither_ptr _tmp7AD=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp383="struct";_tag_dyneither(_tmp383,sizeof(char),7U);}):({const char*_tmp384="union";_tag_dyneither(_tmp384,sizeof(char),6U);}));_tmp550.f1=_tmp7AD;});_tmp550;});struct Cyc_String_pa_PrintArg_struct _tmp380=({struct Cyc_String_pa_PrintArg_struct _tmp54F;_tmp54F.tag=0U,({
struct _dyneither_ptr _tmp7AE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp54F.f1=_tmp7AE;});_tmp54F;});struct Cyc_String_pa_PrintArg_struct _tmp381=({struct Cyc_String_pa_PrintArg_struct _tmp54E;_tmp54E.tag=0U,({struct _dyneither_ptr _tmp7AF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp37B));_tmp54E.f1=_tmp7AF;});_tmp54E;});struct Cyc_String_pa_PrintArg_struct _tmp382=({struct Cyc_String_pa_PrintArg_struct _tmp54D;_tmp54D.tag=0U,({
struct _dyneither_ptr _tmp7B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp385->topt)));_tmp54D.f1=_tmp7B0;});_tmp54D;});void*_tmp37C[5U];_tmp37C[0]=& _tmp37E,_tmp37C[1]=& _tmp37F,_tmp37C[2]=& _tmp380,_tmp37C[3]=& _tmp381,_tmp37C[4]=& _tmp382;({unsigned int _tmp7B2=_tmp385->loc;struct _dyneither_ptr _tmp7B1=({const char*_tmp37D="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp37D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp7B2,_tmp7B1,_tag_dyneither(_tmp37C,sizeof(void*),5U));});});
Cyc_Tcutil_explain_failure();}};}{
# 2008
struct Cyc_List_List*_tmp387=0;
{struct Cyc_List_List*_tmp388=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp388 != 0;_tmp388=_tmp388->tl){
({struct Cyc_List_List*_tmp7B6=({struct Cyc_List_List*_tmp38A=_cycalloc(sizeof(*_tmp38A));({struct _tuple0*_tmp7B5=({struct _tuple0*_tmp389=_cycalloc(sizeof(*_tmp389));({void*_tmp7B4=Cyc_Tcutil_substitute(_tmp36D,(*((struct _tuple0*)_tmp388->hd)).f1);_tmp389->f1=_tmp7B4;}),({
void*_tmp7B3=Cyc_Tcutil_substitute(_tmp36D,(*((struct _tuple0*)_tmp388->hd)).f2);_tmp389->f2=_tmp7B3;});_tmp389;});
# 2010
_tmp38A->hd=_tmp7B5;}),_tmp38A->tl=_tmp387;_tmp38A;});_tmp387=_tmp7B6;});}}
# 2013
({struct Cyc_List_List*_tmp7B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp387);_tmp387=_tmp7B7;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp387);
return res_typ;};};};};};}
# 2019
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2021
{void*_tmp38B=Cyc_Tcutil_compress(ts);void*_tmp38C=_tmp38B;enum Cyc_Absyn_AggrKind _tmp39B;struct Cyc_List_List*_tmp39A;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C)->tag == 12U){_LL1: _tmp39B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C)->f1;_tmp39A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp38C)->f2;_LL2:
# 2023
 if(_tmp39B == Cyc_Absyn_UnionA)
({void*_tmp38D=0U;({unsigned int _tmp7B9=loc;struct _dyneither_ptr _tmp7B8=({const char*_tmp38E="expecting struct but found union";_tag_dyneither(_tmp38E,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp7B9,_tmp7B8,_tag_dyneither(_tmp38D,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp39A);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp38F=(struct _tuple19*)fields->hd;struct _tuple19*_tmp390=_tmp38F;struct Cyc_Absyn_Aggrfield*_tmp397;struct Cyc_Absyn_Exp*_tmp396;_LL6: _tmp397=_tmp390->f1;_tmp396=_tmp390->f2;_LL7:;
({struct Cyc_Tcenv_Tenv*_tmp7BB=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp7BA=& _tmp397->type;Cyc_Tcexp_tcExpInitializer(_tmp7BB,_tmp7BA,_tmp396);});
# 2032
if(!Cyc_Tcutil_coerce_arg(te,_tmp396,_tmp397->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp393=({struct Cyc_String_pa_PrintArg_struct _tmp555;_tmp555.tag=0U,_tmp555.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp397->name);_tmp555;});struct Cyc_String_pa_PrintArg_struct _tmp394=({struct Cyc_String_pa_PrintArg_struct _tmp554;_tmp554.tag=0U,({
struct _dyneither_ptr _tmp7BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp397->type));_tmp554.f1=_tmp7BC;});_tmp554;});struct Cyc_String_pa_PrintArg_struct _tmp395=({struct Cyc_String_pa_PrintArg_struct _tmp553;_tmp553.tag=0U,({
struct _dyneither_ptr _tmp7BD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp396->topt)));_tmp553.f1=_tmp7BD;});_tmp553;});void*_tmp391[3U];_tmp391[0]=& _tmp393,_tmp391[1]=& _tmp394,_tmp391[2]=& _tmp395;({unsigned int _tmp7BF=_tmp396->loc;struct _dyneither_ptr _tmp7BE=({const char*_tmp392="field %s of struct expects type %s != %s";_tag_dyneither(_tmp392,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp7BF,_tmp7BE,_tag_dyneither(_tmp391,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2039
goto _LL0;};}else{_LL3: _LL4:
({void*_tmp398=0U;({struct _dyneither_ptr _tmp7C0=({const char*_tmp399="tcAnonStruct: wrong type";_tag_dyneither(_tmp399,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7C0,_tag_dyneither(_tmp398,sizeof(void*),0U));});});}_LL0:;}
# 2042
return ts;}
# 2046
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2049
struct _tuple12 _tmp39C=({struct _tuple12 _tmp55B;({struct Cyc_List_List*_tmp7C1=Cyc_Tcenv_lookup_type_vars(te);_tmp55B.f1=_tmp7C1;}),_tmp55B.f2=Cyc_Core_heap_region;_tmp55B;});
struct Cyc_List_List*_tmp39D=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp39C,tud->tvs);
struct Cyc_List_List*_tmp39E=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp39D);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));
_tmp3AD->tag=4U,({union Cyc_Absyn_DatatypeFieldInfoU _tmp7C2=Cyc_Absyn_KnownDatatypefield(tud,tuf);(_tmp3AD->f1).field_info=_tmp7C2;}),(_tmp3AD->f1).targs=_tmp39E;_tmp3AD;});
# 2056
if(topt != 0){
void*_tmp39F=Cyc_Tcutil_compress(*topt);void*_tmp3A0=_tmp39F;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3A0)->tag == 4U){_LL1: _LL2:
 Cyc_Tcutil_unify(*topt,res);goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2062
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp39D != 0)({void*_tmp7C3=Cyc_Tcutil_substitute(_tmp39D,t);t=_tmp7C3;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3A3=({struct Cyc_String_pa_PrintArg_struct _tmp558;_tmp558.tag=0U,({
# 2072
struct _dyneither_ptr _tmp7C4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp558.f1=_tmp7C4;});_tmp558;});struct Cyc_String_pa_PrintArg_struct _tmp3A4=({struct Cyc_String_pa_PrintArg_struct _tmp557;_tmp557.tag=0U,({struct _dyneither_ptr _tmp7C5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp557.f1=_tmp7C5;});_tmp557;});struct Cyc_String_pa_PrintArg_struct _tmp3A5=({struct Cyc_String_pa_PrintArg_struct _tmp556;_tmp556.tag=0U,({
struct _dyneither_ptr _tmp7C6=(struct _dyneither_ptr)((struct _dyneither_ptr)(e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3A6="?";_tag_dyneither(_tmp3A6,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp556.f1=_tmp7C6;});_tmp556;});void*_tmp3A1[3U];_tmp3A1[0]=& _tmp3A3,_tmp3A1[1]=& _tmp3A4,_tmp3A1[2]=& _tmp3A5;({unsigned int _tmp7C8=e->loc;struct _dyneither_ptr _tmp7C7=({const char*_tmp3A2="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3A2,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp7C8,_tmp7C7,_tag_dyneither(_tmp3A1,sizeof(void*),3U));});});
Cyc_Tcutil_explain_failure();}}
# 2077
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3A9=({struct Cyc_String_pa_PrintArg_struct _tmp559;_tmp559.tag=0U,({
# 2080
struct _dyneither_ptr _tmp7C9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp559.f1=_tmp7C9;});_tmp559;});void*_tmp3A7[1U];_tmp3A7[0]=& _tmp3A9;({struct Cyc_Tcenv_Tenv*_tmp7CD=te;unsigned int _tmp7CC=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7CB=topt;struct _dyneither_ptr _tmp7CA=({const char*_tmp3A8="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3A8,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp7CD,_tmp7CC,_tmp7CB,_tmp7CA,_tag_dyneither(_tmp3A7,sizeof(void*),1U));});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3AC=({struct Cyc_String_pa_PrintArg_struct _tmp55A;_tmp55A.tag=0U,({
struct _dyneither_ptr _tmp7CE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp55A.f1=_tmp7CE;});_tmp55A;});void*_tmp3AA[1U];_tmp3AA[0]=& _tmp3AC;({struct Cyc_Tcenv_Tenv*_tmp7D2=te;unsigned int _tmp7D1=loc;void**_tmp7D0=topt;struct _dyneither_ptr _tmp7CF=({const char*_tmp3AB="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3AB,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7D2,_tmp7D1,_tmp7D0,_tmp7CF,_tag_dyneither(_tmp3AA,sizeof(void*),1U));});});
return res;};}
# 2087
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2089
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2091
if(topt != 0){
void*_tmp3AE=Cyc_Tcutil_compress(*topt);void*_tmp3AF=_tmp3AE;void*_tmp3B0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AF)->tag == 5U){_LL1: _tmp3B0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AF)->f1).elt_typ;_LL2:
# 2094
 Cyc_Tcutil_unify(_tmp3B0,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2100
({struct Cyc_String_pa_PrintArg_struct _tmp3B3=({struct Cyc_String_pa_PrintArg_struct _tmp55D;_tmp55D.tag=0U,({
struct _dyneither_ptr _tmp7D3=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3B5="calloc";_tag_dyneither(_tmp3B5,sizeof(char),7U);}):(struct _dyneither_ptr)({const char*_tmp3B6="malloc";_tag_dyneither(_tmp3B6,sizeof(char),7U);})));_tmp55D.f1=_tmp7D3;});_tmp55D;});struct Cyc_String_pa_PrintArg_struct _tmp3B4=({struct Cyc_String_pa_PrintArg_struct _tmp55C;_tmp55C.tag=0U,({struct _dyneither_ptr _tmp7D4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp55C.f1=_tmp7D4;});_tmp55C;});void*_tmp3B1[2U];_tmp3B1[0]=& _tmp3B3,_tmp3B1[1]=& _tmp3B4;({unsigned int _tmp7D6=loc;struct _dyneither_ptr _tmp7D5=({const char*_tmp3B2="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3B2,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp7D6,_tmp7D5,_tag_dyneither(_tmp3B1,sizeof(void*),2U));});});}
# 2104
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2106
enum Cyc_Absyn_AliasQual _tmp3B7=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3B8=_tmp3B7;if(_tmp3B8 == Cyc_Absyn_Unique){_LL1: _LL2:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL3: _LL4:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL0:;}
# 2112
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2117
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2120
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3BF=_cycalloc(sizeof(*_tmp3BF));
_tmp3BF->tag=15U,({void*_tmp7D7=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3BF->f1=_tmp7D7;});_tmp3BF;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3B9=Cyc_Tcutil_compress(handle_type);void*_tmp3BA=_tmp3B9;void*_tmp3BE;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3BA)->tag == 15U){_LL1: _tmp3BE=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3BA)->f1;_LL2:
# 2125
 rgn=_tmp3BE;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2129
({struct Cyc_String_pa_PrintArg_struct _tmp3BD=({struct Cyc_String_pa_PrintArg_struct _tmp55E;_tmp55E.tag=0U,({
struct _dyneither_ptr _tmp7D8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp55E.f1=_tmp7D8;});_tmp55E;});void*_tmp3BB[1U];_tmp3BB[0]=& _tmp3BD;({unsigned int _tmp7DA=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7D9=({const char*_tmp3BC="expecting region_t type but found %s";_tag_dyneither(_tmp3BC,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7DA,_tmp7D9,_tag_dyneither(_tmp3BB,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2136
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7DB=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7DB;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2143
({struct Cyc_Tcenv_Tenv*_tmp7DC=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7DC,& Cyc_Absyn_uint_typ,*e);});{
# 2152 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3C0=0U;({struct _dyneither_ptr _tmp7DD=({const char*_tmp3C1="calloc with empty type";_tag_dyneither(_tmp3C1,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7DD,_tag_dyneither(_tmp3C0,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7E0=loc;struct Cyc_Tcenv_Tenv*_tmp7DF=te;struct Cyc_List_List*_tmp7DE=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7E0,_tmp7DF,_tmp7DE,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2164
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3C2=er;struct Cyc_Absyn_Exp*_tmp3D7;struct Cyc_Absyn_Exp*_tmp3D6;void*_tmp3D5;switch(*((int*)_tmp3C2)){case 17U: _LL6: _tmp3D5=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1;_LL7:
# 2168
 elt_type=_tmp3D5;
({void**_tmp7E1=({void**_tmp3C3=_cycalloc(sizeof(*_tmp3C3));*_tmp3C3=elt_type;_tmp3C3;});*t=_tmp7E1;});
({struct Cyc_Absyn_Exp*_tmp7E2=Cyc_Absyn_uint_exp(1U,0U);num_elts=_tmp7E2;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL5;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->tl)->tl == 0){_LL8: _tmp3D7=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->hd;_tmp3D6=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3C2)->f2)->tl)->hd;_LL9:
# 2175
{struct _tuple0 _tmp3C4=({struct _tuple0 _tmp55F;_tmp55F.f1=_tmp3D7->r,_tmp55F.f2=_tmp3D6->r;_tmp55F;});struct _tuple0 _tmp3C5=_tmp3C4;void*_tmp3C9;void*_tmp3C8;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3C5.f1)->tag == 17U){_LLD: _tmp3C8=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3C5.f1)->f1;_LLE:
# 2177
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C8);
elt_type=_tmp3C8;
({void**_tmp7E3=({void**_tmp3C6=_cycalloc(sizeof(*_tmp3C6));*_tmp3C6=elt_type;_tmp3C6;});*t=_tmp7E3;});
num_elts=_tmp3D6;
one_elt=0;
goto _LLC;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3C5.f2)->tag == 17U){_LLF: _tmp3C9=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3C5.f2)->f1;_LL10:
# 2184
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3C9);
elt_type=_tmp3C9;
({void**_tmp7E4=({void**_tmp3C7=_cycalloc(sizeof(*_tmp3C7));*_tmp3C7=elt_type;_tmp3C7;});*t=_tmp7E4;});
num_elts=_tmp3D7;
one_elt=0;
goto _LLC;}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2192
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2194
 No_sizeof: {
# 2197
struct Cyc_Absyn_Exp*_tmp3CA=*e;
{void*_tmp3CB=_tmp3CA->r;void*_tmp3CC=_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CD;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CC)->tag == 14U){_LL14: _tmp3CD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3CC)->f2;_LL15:
 _tmp3CA=_tmp3CD;goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2202
{void*_tmp3CE=Cyc_Tcutil_compress((void*)_check_null(_tmp3CA->topt));void*_tmp3CF=_tmp3CE;void*_tmp3D3;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3CF)->tag == 19U){_LL19: _tmp3D3=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3CF)->f1;_LL1A:
# 2204
{void*_tmp3D0=Cyc_Tcutil_compress(_tmp3D3);void*_tmp3D1=_tmp3D0;struct Cyc_Absyn_Exp*_tmp3D2;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D1)->tag == 18U){_LL1E: _tmp3D2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3D1)->f1;_LL1F:
# 2206
 er=_tmp3D2->r;goto retry_sizeof;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2209
goto _LL18;}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2212
elt_type=Cyc_Absyn_char_typ;
({void**_tmp7E5=({void**_tmp3D4=_cycalloc(sizeof(*_tmp3D4));*_tmp3D4=elt_type;_tmp3D4;});*t=_tmp7E5;});
num_elts=*e;
one_elt=0;}
# 2217
goto _LL5;}_LL5:;}}
# 2221
*is_fat=!one_elt;
# 2224
{void*_tmp3D8=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp3DB;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3D8)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3D8)->f1).aggr_info).KnownAggr).tag == 2){_LL23: _tmp3DB=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3D8)->f1).aggr_info).KnownAggr).val;_LL24:
# 2226
 if(_tmp3DB->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3DB->impl))->exist_vars != 0)
({void*_tmp3D9=0U;({unsigned int _tmp7E7=loc;struct _dyneither_ptr _tmp7E6=({const char*_tmp3DA="malloc with existential types not yet implemented";_tag_dyneither(_tmp3DA,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp7E7,_tmp7E6,_tag_dyneither(_tmp3D9,sizeof(void*),0U));});});
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2233
void*(*_tmp3DC)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp3DD=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp3DE=Cyc_Tcutil_compress(*topt);void*_tmp3DF=_tmp3DE;union Cyc_Absyn_Constraint*_tmp3EC;union Cyc_Absyn_Constraint*_tmp3EB;union Cyc_Absyn_Constraint*_tmp3EA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DF)->tag == 5U){_LL28: _tmp3EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DF)->f1).ptr_atts).nullable;_tmp3EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DF)->f1).ptr_atts).bounds;_tmp3EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DF)->f1).ptr_atts).zero_term;_LL29:
# 2238
 _tmp3DD=_tmp3EA;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3EC))
_tmp3DC=Cyc_Absyn_star_typ;
# 2243
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3EA) && !(*is_calloc)){
({void*_tmp3E0=0U;({unsigned int _tmp7E9=loc;struct _dyneither_ptr _tmp7E8=({const char*_tmp3E1="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp3E1,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp7E9,_tmp7E8,_tag_dyneither(_tmp3E0,sizeof(void*),0U));});});
*is_calloc=1;}
# 2249
{void*_tmp3E2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3EB);void*_tmp3E3=_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E9;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3E3)->tag == 0U){_LL2D: _LL2E:
 goto _LL2C;}else{_LL2F: _tmp3E9=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3E3)->f1;if(!one_elt){_LL30: {
# 2252
int _tmp3E4=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp3E4  && Cyc_Evexp_same_const_exp(_tmp3E9,num_elts)){
*is_fat=0;
return({void*_tmp7ED=elt_type;void*_tmp7EC=rgn;struct Cyc_Absyn_Tqual _tmp7EB=Cyc_Absyn_empty_tqual(0U);void*_tmp7EA=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp3EB);Cyc_Absyn_atb_typ(_tmp7ED,_tmp7EC,_tmp7EB,_tmp7EA,_tmp3DD);});}
# 2258
{void*_tmp3E5=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp3E6=_tmp3E5;void*_tmp3E8;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E6)->tag == 19U){_LL34: _tmp3E8=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3E6)->f1;_LL35: {
# 2260
struct Cyc_Absyn_Exp*_tmp3E7=({void*_tmp7EE=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp7EE,Cyc_Absyn_valueof_exp(_tmp3E8,0U),0,Cyc_Absyn_No_coercion,0U);});
# 2262
if(Cyc_Evexp_same_const_exp(_tmp3E7,_tmp3E9)){
*is_fat=0;
return({void*_tmp7F2=elt_type;void*_tmp7F1=rgn;struct Cyc_Absyn_Tqual _tmp7F0=Cyc_Absyn_empty_tqual(0U);void*_tmp7EF=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp3EB);Cyc_Absyn_atb_typ(_tmp7F2,_tmp7F1,_tmp7F0,_tmp7EF,_tmp3DD);});}
# 2267
goto _LL33;}}else{_LL36: _LL37:
 goto _LL33;}_LL33:;}
# 2270
goto _LL2C;}}else{_LL31: _LL32:
 goto _LL2C;}}_LL2C:;}
# 2273
goto _LL27;}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2276
if(!one_elt)_tmp3DC=Cyc_Absyn_dyneither_typ;
return({void*(*_tmp7F6)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=_tmp3DC;void*_tmp7F5=elt_type;void*_tmp7F4=rgn;struct Cyc_Absyn_Tqual _tmp7F3=Cyc_Absyn_empty_tqual(0U);_tmp7F6(_tmp7F5,_tmp7F4,_tmp7F3,_tmp3DD);});};};}
# 2281
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2283
struct Cyc_Tcenv_Tenv*_tmp3ED=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp3ED,0,e1);{
void*_tmp3EE=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp3ED,& _tmp3EE,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2290
{void*_tmp3EF=Cyc_Tcutil_compress(t1);void*_tmp3F0=_tmp3EF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0)->tag == 8U){_LL1: _LL2:
({void*_tmp3F1=0U;({unsigned int _tmp7F8=loc;struct _dyneither_ptr _tmp7F7=({const char*_tmp3F2="cannot assign to an array";_tag_dyneither(_tmp3F2,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp7F8,_tmp7F7,_tag_dyneither(_tmp3F1,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2295
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp3F3=0U;({unsigned int _tmp7FA=loc;struct _dyneither_ptr _tmp7F9=({const char*_tmp3F4="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp3F4,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp7FA,_tmp7F9,_tag_dyneither(_tmp3F3,sizeof(void*),0U));});});
# 2299
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp3F5=0U;({struct Cyc_Tcenv_Tenv*_tmp7FE=te;unsigned int _tmp7FD=e1->loc;void**_tmp7FC=topt;struct _dyneither_ptr _tmp7FB=({const char*_tmp3F6="swap non-lvalue";_tag_dyneither(_tmp3F6,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7FE,_tmp7FD,_tmp7FC,_tmp7FB,_tag_dyneither(_tmp3F5,sizeof(void*),0U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp3F7=0U;({struct Cyc_Tcenv_Tenv*_tmp802=te;unsigned int _tmp801=e2->loc;void**_tmp800=topt;struct _dyneither_ptr _tmp7FF=({const char*_tmp3F8="swap non-lvalue";_tag_dyneither(_tmp3F8,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp802,_tmp801,_tmp800,_tmp7FF,_tag_dyneither(_tmp3F7,sizeof(void*),0U));});});{
# 2304
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp3F9=0U;({struct Cyc_Tcenv_Tenv*_tmp806=te;unsigned int _tmp805=e1->loc;void**_tmp804=topt;struct _dyneither_ptr _tmp803=({const char*_tmp3FA="swap value in zeroterm array";_tag_dyneither(_tmp3FA,sizeof(char),29U);});Cyc_Tcexp_expr_err(_tmp806,_tmp805,_tmp804,_tmp803,_tag_dyneither(_tmp3F9,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp3FB=0U;({struct Cyc_Tcenv_Tenv*_tmp80A=te;unsigned int _tmp809=e2->loc;void**_tmp808=topt;struct _dyneither_ptr _tmp807=({const char*_tmp3FC="swap value in zeroterm array";_tag_dyneither(_tmp3FC,sizeof(char),29U);});Cyc_Tcexp_expr_err(_tmp80A,_tmp809,_tmp808,_tmp807,_tag_dyneither(_tmp3FB,sizeof(void*),0U));});});
# 2311
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp3FD=({struct Cyc_String_pa_PrintArg_struct _tmp400=({struct Cyc_String_pa_PrintArg_struct _tmp561;_tmp561.tag=0U,({
struct _dyneither_ptr _tmp80B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp561.f1=_tmp80B;});_tmp561;});struct Cyc_String_pa_PrintArg_struct _tmp401=({struct Cyc_String_pa_PrintArg_struct _tmp560;_tmp560.tag=0U,({struct _dyneither_ptr _tmp80C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp560.f1=_tmp80C;});_tmp560;});void*_tmp3FE[2U];_tmp3FE[0]=& _tmp400,_tmp3FE[1]=& _tmp401;({struct Cyc_Tcenv_Tenv*_tmp810=te;unsigned int _tmp80F=loc;void**_tmp80E=topt;struct _dyneither_ptr _tmp80D=({const char*_tmp3FF="type mismatch: %s != %s";_tag_dyneither(_tmp3FF,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp810,_tmp80F,_tmp80E,_tmp80D,_tag_dyneither(_tmp3FE,sizeof(void*),2U));});});
return _tmp3FD;}
# 2318
return(void*)& Cyc_Absyn_VoidType_val;};};};}
# 2323
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp811=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp811,s,1);});
# 2326
while(1){
void*_tmp402=s->r;void*_tmp403=_tmp402;struct Cyc_Absyn_Decl*_tmp40E;struct Cyc_Absyn_Stmt*_tmp40D;struct Cyc_Absyn_Stmt*_tmp40C;struct Cyc_Absyn_Stmt*_tmp40B;struct Cyc_Absyn_Exp*_tmp40A;switch(*((int*)_tmp403)){case 1U: _LL1: _tmp40A=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp403)->f1;_LL2: {
# 2330
void*_tmp404=(void*)_check_null(_tmp40A->topt);
if(!({void*_tmp812=_tmp404;Cyc_Tcutil_unify(_tmp812,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp407=({struct Cyc_String_pa_PrintArg_struct _tmp562;_tmp562.tag=0U,({
struct _dyneither_ptr _tmp813=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp404));_tmp562.f1=_tmp813;});_tmp562;});void*_tmp405[1U];_tmp405[0]=& _tmp407;({unsigned int _tmp815=loc;struct _dyneither_ptr _tmp814=({const char*_tmp406="statement expression returns type %s";_tag_dyneither(_tmp406,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp815,_tmp814,_tag_dyneither(_tmp405,sizeof(void*),1U));});});
Cyc_Tcutil_explain_failure();}
# 2336
return _tmp404;}case 2U: _LL3: _tmp40C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp403)->f1;_tmp40B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp403)->f2;_LL4:
 s=_tmp40B;continue;case 12U: _LL5: _tmp40E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp403)->f1;_tmp40D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp403)->f2;_LL6:
 s=_tmp40D;continue;default: _LL7: _LL8:
# 2340
 return({void*_tmp408=0U;({struct Cyc_Tcenv_Tenv*_tmp819=te;unsigned int _tmp818=loc;void**_tmp817=topt;struct _dyneither_ptr _tmp816=({const char*_tmp409="statement expression must end with expression";_tag_dyneither(_tmp409,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp819,_tmp818,_tmp817,_tmp816,_tag_dyneither(_tmp408,sizeof(void*),0U));});});}_LL0:;}}
# 2345
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp81A=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp81A,0,e);}));
{void*_tmp40F=t;struct Cyc_Absyn_Aggrdecl*_tmp413;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp40F)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp40F)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp413=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp40F)->f1).aggr_info).KnownAggr).val;_LL2:
# 2349
 if((_tmp413->kind == Cyc_Absyn_UnionA  && _tmp413->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp413->impl))->tagged)goto _LL0;
goto _LL4;}else{goto _LL3;}}else{_LL3: _LL4:
# 2352
({struct Cyc_String_pa_PrintArg_struct _tmp412=({struct Cyc_String_pa_PrintArg_struct _tmp563;_tmp563.tag=0U,({struct _dyneither_ptr _tmp81B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp563.f1=_tmp81B;});_tmp563;});void*_tmp410[1U];_tmp410[0]=& _tmp412;({unsigned int _tmp81D=loc;struct _dyneither_ptr _tmp81C=({const char*_tmp411="expecting @tagged union but found %s";_tag_dyneither(_tmp411,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp81D,_tmp81C,_tag_dyneither(_tmp410,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}
# 2355
return Cyc_Absyn_uint_typ;}
# 2359
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2363
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct Cyc_Tcenv_Tenv*_tmp414=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2367
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));
_tmp41B->tag=15U,({void*_tmp81E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp414));_tmp41B->f1=_tmp81E;});_tmp41B;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp414,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp415=Cyc_Tcutil_compress(handle_type);void*_tmp416=_tmp415;void*_tmp41A;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp416)->tag == 15U){_LL1: _tmp41A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp416)->f1;_LL2:
# 2372
 rgn=_tmp41A;
Cyc_Tcenv_check_rgn_accessible(_tmp414,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2376
({struct Cyc_String_pa_PrintArg_struct _tmp419=({struct Cyc_String_pa_PrintArg_struct _tmp564;_tmp564.tag=0U,({
struct _dyneither_ptr _tmp81F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(handle_type));_tmp564.f1=_tmp81F;});_tmp564;});void*_tmp417[1U];_tmp417[0]=& _tmp419;({unsigned int _tmp821=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp820=({const char*_tmp418="expecting region_t type but found %s";_tag_dyneither(_tmp418,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp821,_tmp820,_tag_dyneither(_tmp417,sizeof(void*),1U));});});
goto _LL0;}_LL0:;}else{
# 2383
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp822=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp822;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2391
void*_tmp41C=e1->r;void*_tmp41D=_tmp41C;struct Cyc_List_List*_tmp437;struct Cyc_Core_Opt*_tmp436;struct Cyc_List_List*_tmp435;switch(*((int*)_tmp41D)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2396
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp414,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp41E=0U;({struct _dyneither_ptr _tmp823=({const char*_tmp41F="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp41F,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp823,_tag_dyneither(_tmp41E,sizeof(void*),0U));});});
({void*_tmp824=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp824;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp414,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp414,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2407
return res_typ;}case 36U: _LLA: _tmp436=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp41D)->f1;_tmp435=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp41D)->f2;_LLB:
# 2409
({void*_tmp825=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->tag=26U,_tmp420->f1=_tmp435;_tmp420;});e1->r=_tmp825;});
_tmp437=_tmp435;goto _LLD;case 26U: _LLC: _tmp437=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp41D)->f1;_LLD: {
# 2412
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp421=Cyc_Tcutil_compress(*topt);void*_tmp422=_tmp421;void**_tmp425;struct Cyc_Absyn_Tqual _tmp424;union Cyc_Absyn_Constraint*_tmp423;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp422)->tag == 5U){_LL15: _tmp425=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp422)->f1).elt_typ;_tmp424=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp422)->f1).elt_tq;_tmp423=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp422)->f1).ptr_atts).zero_term;_LL16:
# 2418
 elt_typ_opt=_tmp425;
({int _tmp826=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp423);zero_term=_tmp826;});
goto _LL14;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2424
void*res_typ=Cyc_Tcexp_tcArray(_tmp414,e1->loc,elt_typ_opt,0,zero_term,_tmp437);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp426=0U;({struct _dyneither_ptr _tmp827=({const char*_tmp427="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp427,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp827,_tag_dyneither(_tmp426,sizeof(void*),0U));});});
({void*_tmp828=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp828;});
if(topt != 0){
# 2433
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp414,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp414,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2439
return res_typ;};}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp41D)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2444
void*_tmp428=({void*_tmp82B=Cyc_Absyn_char_typ;void*_tmp82A=rgn;struct Cyc_Absyn_Tqual _tmp829=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp82B,_tmp82A,_tmp829,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2446
void*_tmp429=Cyc_Tcexp_tcExp(_tmp414,& _tmp428,e1);
return({void*_tmp830=_tmp429;void*_tmp82F=rgn;struct Cyc_Absyn_Tqual _tmp82E=Cyc_Absyn_empty_tqual(0U);void*_tmp82D=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A->tag=1U,({struct Cyc_Absyn_Exp*_tmp82C=Cyc_Absyn_uint_exp(1U,0U);_tmp42A->f1=_tmp82C;});_tmp42A;});Cyc_Absyn_atb_typ(_tmp830,_tmp82F,_tmp82E,_tmp82D,Cyc_Absyn_false_conref);});}case 9U: _LL10: _LL11: {
# 2451
void*_tmp42B=({void*_tmp833=Cyc_Absyn_wchar_typ();void*_tmp832=rgn;struct Cyc_Absyn_Tqual _tmp831=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp833,_tmp832,_tmp831,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2453
void*_tmp42C=Cyc_Tcexp_tcExp(_tmp414,& _tmp42B,e1);
return({void*_tmp838=_tmp42C;void*_tmp837=rgn;struct Cyc_Absyn_Tqual _tmp836=Cyc_Absyn_empty_tqual(0U);void*_tmp835=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp42D=_cycalloc(sizeof(*_tmp42D));_tmp42D->tag=1U,({struct Cyc_Absyn_Exp*_tmp834=Cyc_Absyn_uint_exp(1U,0U);_tmp42D->f1=_tmp834;});_tmp42D;});Cyc_Absyn_atb_typ(_tmp838,_tmp837,_tmp836,_tmp835,Cyc_Absyn_false_conref);});}default: goto _LL12;}default: _LL12: _LL13:
# 2460
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp42E=Cyc_Tcutil_compress(*topt);void*_tmp42F=_tmp42E;void**_tmp431;struct Cyc_Absyn_Tqual _tmp430;switch(*((int*)_tmp42F)){case 5U: _LL1A: _tmp431=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42F)->f1).elt_typ;_tmp430=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42F)->f1).elt_tq;_LL1B:
# 2466
 topt2=_tmp431;goto _LL19;case 3U: _LL1C: _LL1D:
# 2470
 bogus=*topt;
topt2=& bogus;
goto _LL19;default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2476
void*telt=Cyc_Tcexp_tcExp(_tmp414,topt2,e1);
# 2478
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp432=0U;({unsigned int _tmp83A=e1->loc;struct _dyneither_ptr _tmp839=({const char*_tmp433="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp433,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp83A,_tmp839,_tag_dyneither(_tmp432,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp434=_cycalloc(sizeof(*_tmp434));
_tmp434->tag=5U,(_tmp434->f1).elt_typ=telt,({struct Cyc_Absyn_Tqual _tmp83C=Cyc_Absyn_empty_tqual(0U);(_tmp434->f1).elt_tq=_tmp83C;}),
((_tmp434->f1).ptr_atts).rgn=rgn,({union Cyc_Absyn_Constraint*_tmp83B=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();((_tmp434->f1).ptr_atts).nullable=_tmp83B;}),((_tmp434->f1).ptr_atts).bounds=Cyc_Absyn_bounds_one_conref,((_tmp434->f1).ptr_atts).zero_term=Cyc_Absyn_false_conref,((_tmp434->f1).ptr_atts).ptrloc=0;_tmp434;});
# 2485
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp414,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp414,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2492
return res_typ;};};}}_LL5:;};}
# 2498
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
({void*_tmp83F=({void*_tmp83E=({void*_tmp83D=t;Cyc_Tcutil_promote_array(_tmp83D,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp83E;});e->topt=_tmp83F;});
return t;}
# 2508
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2512
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2517
({void*_tmp438=0U;({unsigned int _tmp841=e->loc;struct _dyneither_ptr _tmp840=({const char*_tmp439="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp439,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp841,_tmp840,_tag_dyneither(_tmp438,sizeof(void*),0U));});});{
# 2519
void*_tmp43A=e->r;void*_tmp43B=_tmp43A;switch(*((int*)_tmp43B)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp43B)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2526
({void*_tmp842=Cyc_Tcutil_compress(t);t=_tmp842;});
if(Cyc_Tcutil_is_array(t)){
({void*_tmp844=({void*_tmp843=t;Cyc_Tcutil_promote_array(_tmp843,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp844;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2531
return t;}_LL0:;};}
# 2542 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp43C=e->r;void*_tmp43D=_tmp43C;struct Cyc_Absyn_Exp*_tmp45B;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp43D)->tag == 12U){_LL1: _tmp45B=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp43D)->f1;_LL2:
# 2546
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp45B);
({void*_tmp845=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp45B->topt),0);_tmp45B->topt=_tmp845;});
e->topt=_tmp45B->topt;
goto _LL0;}else{_LL3: _LL4:
# 2552
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp846=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp846;});
# 2555
{void*_tmp43E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp43F=_tmp43E;void*_tmp45A;struct Cyc_Absyn_Tqual _tmp459;void*_tmp458;union Cyc_Absyn_Constraint*_tmp457;union Cyc_Absyn_Constraint*_tmp456;union Cyc_Absyn_Constraint*_tmp455;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->tag == 5U){_LL6: _tmp45A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).elt_typ;_tmp459=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).elt_tq;_tmp458=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).rgn;_tmp457=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).nullable;_tmp456=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).bounds;_tmp455=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).zero_term;_LL7:
# 2557
{void*_tmp440=Cyc_Tcutil_compress(_tmp45A);void*_tmp441=_tmp440;struct Cyc_List_List*_tmp454;void*_tmp453;struct Cyc_Absyn_Tqual _tmp452;void*_tmp451;struct Cyc_List_List*_tmp450;int _tmp44F;struct Cyc_Absyn_VarargInfo*_tmp44E;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44C;struct Cyc_Absyn_Exp*_tmp44B;struct Cyc_List_List*_tmp44A;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_List_List*_tmp448;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->tag == 9U){_LLB: _tmp454=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).tvars;_tmp453=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).effect;_tmp452=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).ret_tqual;_tmp451=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).ret_typ;_tmp450=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).args;_tmp44F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).c_varargs;_tmp44E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).cyc_varargs;_tmp44D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).rgn_po;_tmp44C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).attributes;_tmp44B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).requires_clause;_tmp44A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).requires_relns;_tmp449=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).ensures_clause;_tmp448=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp441)->f1).ensures_relns;_LLC:
# 2559
 if(_tmp454 != 0){
struct _tuple12 _tmp442=({struct _tuple12 _tmp565;({struct Cyc_List_List*_tmp847=Cyc_Tcenv_lookup_type_vars(te);_tmp565.f1=_tmp847;}),_tmp565.f2=Cyc_Core_heap_region;_tmp565;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp442,_tmp454);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2567
({struct Cyc_List_List*_tmp848=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp44D);_tmp44D=_tmp848;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp44D);{
void*ftyp=({struct Cyc_List_List*_tmp849=inst;Cyc_Tcutil_substitute(_tmp849,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp447=_cycalloc(sizeof(*_tmp447));
_tmp447->tag=9U,(_tmp447->f1).tvars=0,(_tmp447->f1).effect=_tmp453,(_tmp447->f1).ret_tqual=_tmp452,(_tmp447->f1).ret_typ=_tmp451,(_tmp447->f1).args=_tmp450,(_tmp447->f1).c_varargs=_tmp44F,(_tmp447->f1).cyc_varargs=_tmp44E,(_tmp447->f1).rgn_po=0,(_tmp447->f1).attributes=_tmp44C,(_tmp447->f1).requires_clause=_tmp44B,(_tmp447->f1).requires_relns=_tmp44A,(_tmp447->f1).ensures_clause=_tmp449,(_tmp447->f1).ensures_relns=_tmp448;_tmp447;}));});
# 2574
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp443=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446->tag=5U,(_tmp446->f1).elt_typ=ftyp,(_tmp446->f1).elt_tq=_tmp459,((_tmp446->f1).ptr_atts).rgn=_tmp458,((_tmp446->f1).ptr_atts).nullable=_tmp457,((_tmp446->f1).ptr_atts).bounds=_tmp456,((_tmp446->f1).ptr_atts).zero_term=_tmp455,((_tmp446->f1).ptr_atts).ptrloc=0;_tmp446;});
# 2576
struct Cyc_Absyn_Exp*_tmp444=Cyc_Absyn_copy_exp(e);
({void*_tmp84A=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->tag=13U,_tmp445->f1=_tmp444,_tmp445->f2=ts;_tmp445;});e->r=_tmp84A;});
e->topt=(void*)_tmp443;};}
# 2580
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2583
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2586
goto _LL0;}_LL0:;}
# 2588
return(void*)_check_null(e->topt);}
# 2596
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2598
struct Cyc_List_List*_tmp45C=0;
# 2600
{void*_tmp45D=fn_exp->r;void*_tmp45E=_tmp45D;struct Cyc_List_List*_tmp472;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp45E)->tag == 10U){_LL1: _tmp472=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp45E)->f2;_LL2:
# 2602
{void*_tmp45F=e->r;void*_tmp460=_tmp45F;struct Cyc_List_List*_tmp46F;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp460)->tag == 10U){_LL6: _tmp46F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp460)->f2;_LL7: {
# 2604
struct Cyc_List_List*_tmp461=alias_arg_exps;
int _tmp462=0;
while(_tmp461 != 0){
while(_tmp462 != (int)_tmp461->hd){
if(_tmp472 == 0)
({void*_tmp463=0U;({struct _dyneither_ptr _tmp84C=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp466=({struct Cyc_Int_pa_PrintArg_struct _tmp567;_tmp567.tag=1U,_tmp567.f1=(unsigned long)_tmp462;_tmp567;});struct Cyc_Int_pa_PrintArg_struct _tmp467=({struct Cyc_Int_pa_PrintArg_struct _tmp566;_tmp566.tag=1U,_tmp566.f1=(unsigned long)((int)_tmp461->hd);_tmp566;});void*_tmp464[2U];_tmp464[0]=& _tmp466,_tmp464[1]=& _tmp467;({struct _dyneither_ptr _tmp84B=({const char*_tmp465="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp465,sizeof(char),36U);});Cyc_aprintf(_tmp84B,_tag_dyneither(_tmp464,sizeof(void*),2U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp84C,_tag_dyneither(_tmp463,sizeof(void*),0U));});});
# 2611
++ _tmp462;
_tmp472=_tmp472->tl;
_tmp46F=((struct Cyc_List_List*)_check_null(_tmp46F))->tl;}{
# 2616
struct _tuple11 _tmp468=({struct Cyc_Absyn_Exp*_tmp84D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp472))->hd;Cyc_Tcutil_insert_alias(_tmp84D,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp46F))->hd)->topt)));});struct _tuple11 _tmp469=_tmp468;struct Cyc_Absyn_Decl*_tmp46C;struct Cyc_Absyn_Exp*_tmp46B;_LLB: _tmp46C=_tmp469.f1;_tmp46B=_tmp469.f2;_LLC:;
_tmp472->hd=(void*)_tmp46B;
({struct Cyc_List_List*_tmp84E=({struct Cyc_List_List*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->hd=_tmp46C,_tmp46A->tl=_tmp45C;_tmp46A;});_tmp45C=_tmp84E;});
_tmp461=_tmp461->tl;};}
# 2621
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp46D=0U;({struct _dyneither_ptr _tmp84F=({const char*_tmp46E="not a function call!";_tag_dyneither(_tmp46E,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp84F,_tag_dyneither(_tmp46D,sizeof(void*),0U));});});}_LL5:;}
# 2624
goto _LL0;}else{_LL3: _LL4:
({void*_tmp470=0U;({struct _dyneither_ptr _tmp850=({const char*_tmp471="not a function call!";_tag_dyneither(_tmp471,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp850,_tag_dyneither(_tmp470,sizeof(void*),0U));});});}_LL0:;}
# 2629
while(_tmp45C != 0){
struct Cyc_Absyn_Decl*_tmp473=(struct Cyc_Absyn_Decl*)_tmp45C->hd;
({struct Cyc_Absyn_Exp*_tmp854=({struct Cyc_Absyn_Stmt*_tmp853=({struct Cyc_Absyn_Decl*_tmp852=_tmp473;struct Cyc_Absyn_Stmt*_tmp851=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp852,_tmp851,e->loc);});Cyc_Absyn_stmt_exp(_tmp853,e->loc);});fn_exp=_tmp854;});
_tmp45C=_tmp45C->tl;}
# 2636
e->topt=0;
e->r=fn_exp->r;}
# 2642
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2646
{void*_tmp474=e->r;void*_tmp475=_tmp474;void*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct _dyneither_ptr*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;int*_tmp4D9;struct Cyc_Absyn_Exp**_tmp4D8;void***_tmp4D7;struct Cyc_Absyn_Exp**_tmp4D6;int*_tmp4D5;void*_tmp4D4;struct Cyc_Absyn_Enumfield*_tmp4D3;struct Cyc_Absyn_Enumdecl*_tmp4D2;struct Cyc_Absyn_Enumfield*_tmp4D1;struct Cyc_List_List*_tmp4D0;struct Cyc_Absyn_Datatypedecl*_tmp4CF;struct Cyc_Absyn_Datatypefield*_tmp4CE;void*_tmp4CD;struct Cyc_List_List*_tmp4CC;struct _tuple1**_tmp4CB;struct Cyc_List_List**_tmp4CA;struct Cyc_List_List*_tmp4C9;struct Cyc_Absyn_Aggrdecl**_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C7;void*_tmp4C6;int*_tmp4C5;struct Cyc_Absyn_Vardecl*_tmp4C4;struct Cyc_Absyn_Exp*_tmp4C3;struct Cyc_Absyn_Exp*_tmp4C2;int*_tmp4C1;struct Cyc_Absyn_Stmt*_tmp4C0;struct Cyc_List_List*_tmp4BF;struct _tuple9*_tmp4BE;struct Cyc_List_List*_tmp4BD;struct Cyc_List_List*_tmp4BC;struct Cyc_Absyn_Exp*_tmp4BB;struct Cyc_Absyn_Exp*_tmp4BA;struct Cyc_Absyn_Exp*_tmp4B9;struct _dyneither_ptr*_tmp4B8;int*_tmp4B7;int*_tmp4B6;struct Cyc_Absyn_Exp*_tmp4B5;struct _dyneither_ptr*_tmp4B4;int*_tmp4B3;int*_tmp4B2;struct Cyc_Absyn_Exp*_tmp4B1;void*_tmp4B0;struct Cyc_List_List*_tmp4AF;void*_tmp4AE;struct Cyc_Absyn_Exp*_tmp4AD;struct Cyc_Absyn_Exp**_tmp4AC;struct Cyc_Absyn_Exp*_tmp4AB;struct Cyc_Absyn_Exp*_tmp4AA;void*_tmp4A9;struct Cyc_Absyn_Exp*_tmp4A8;enum Cyc_Absyn_Coercion*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_List_List*_tmp4A5;struct Cyc_Absyn_Exp*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;struct Cyc_Absyn_Exp*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;struct Cyc_Absyn_Exp*_tmp4A0;struct Cyc_Absyn_Exp*_tmp49F;struct Cyc_Absyn_Exp*_tmp49E;struct Cyc_Absyn_Exp*_tmp49D;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49B;struct Cyc_Absyn_Exp*_tmp49A;struct Cyc_Core_Opt*_tmp499;struct Cyc_Absyn_Exp*_tmp498;struct Cyc_Absyn_Exp*_tmp497;enum Cyc_Absyn_Incrementor _tmp496;enum Cyc_Absyn_Primop _tmp495;struct Cyc_List_List*_tmp494;void**_tmp493;union Cyc_Absyn_Cnst*_tmp492;struct Cyc_Core_Opt*_tmp491;struct Cyc_List_List*_tmp490;struct Cyc_Absyn_Exp*_tmp48F;struct Cyc_List_List*_tmp48E;struct Cyc_Absyn_VarargCallInfo**_tmp48D;struct Cyc_Absyn_Exp*_tmp48C;struct Cyc_List_List*_tmp48B;struct Cyc_Absyn_VarargCallInfo**_tmp48A;int*_tmp489;struct Cyc_Absyn_Exp*_tmp488;switch(*((int*)_tmp475)){case 12U: _LL1: _tmp488=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL2:
# 2651
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp488);
return;case 10U: _LL3: _tmp48C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp48B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp48A=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_tmp489=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp475)->f4;if(!(*_tmp489)){_LL4:
# 2655
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp477=_cycalloc(sizeof(*_tmp477));_tmp477->tag=Cyc_Core_Impossible,({struct _dyneither_ptr _tmp855=({const char*_tmp476="unresolved function in tcExpNoInst";_tag_dyneither(_tmp476,sizeof(char),35U);});_tmp477->f1=_tmp855;});_tmp477;}));}else{_LL1B: _tmp48F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp48E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp48D=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_LL1C: {
# 2688
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp478;_push_handler(& _tmp478);{int _tmp47A=0;if(setjmp(_tmp478.handler))_tmp47A=1;if(!_tmp47A){
({struct Cyc_Absyn_Exp*_tmp856=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp856;});;_pop_handler();}else{void*_tmp479=(void*)_exn_thrown;void*_tmp47B=_tmp479;void*_tmp47C;if(((struct Cyc_Core_Failure_exn_struct*)_tmp47B)->tag == Cyc_Core_Failure){_LL56: _LL57:
# 2695
 ok=0;
fn_exp=e;
goto _LL55;}else{_LL58: _tmp47C=_tmp47B;_LL59:(int)_rethrow(_tmp47C);}_LL55:;}};}
# 2699
({void*_tmp857=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp48F,_tmp48E,_tmp48D,& alias_arg_exps);t=_tmp857;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp858=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp858;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2706
goto _LL0;}}case 36U: _LL5: _tmp491=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp490=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL6:
# 2659
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp490);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0U: _LL7: _tmp492=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL8:
# 2664
({void*_tmp859=Cyc_Tcexp_tcConst(te,loc,topt,_tmp492,e);t=_tmp859;});goto _LL0;case 1U: _LL9: _tmp493=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LLA:
# 2666
({void*_tmp85A=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp493);t=_tmp85A;});goto _LL0;case 2U: _LLB: _LLC:
# 2668
 t=Cyc_Absyn_sint_typ;goto _LL0;case 3U: _LLD: _tmp495=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp494=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LLE:
# 2670
({void*_tmp85B=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp495,_tmp494);t=_tmp85B;});goto _LL0;case 5U: _LLF: _tmp497=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp496=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL10:
# 2672
({void*_tmp85C=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp497,_tmp496);t=_tmp85C;});goto _LL0;case 4U: _LL11: _tmp49A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp499=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp498=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_LL12:
# 2674
({void*_tmp85D=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp49A,_tmp499,_tmp498);t=_tmp85D;});goto _LL0;case 6U: _LL13: _tmp49D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp49C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp49B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_LL14:
# 2676
({void*_tmp85E=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp49D,_tmp49C,_tmp49B);t=_tmp85E;});goto _LL0;case 7U: _LL15: _tmp49F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp49E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL16:
# 2678
({void*_tmp85F=Cyc_Tcexp_tcAnd(te,loc,_tmp49F,_tmp49E);t=_tmp85F;});goto _LL0;case 8U: _LL17: _tmp4A1=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4A0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL18:
# 2680
({void*_tmp860=Cyc_Tcexp_tcOr(te,loc,_tmp4A1,_tmp4A0);t=_tmp860;});goto _LL0;case 9U: _LL19: _tmp4A3=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4A2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL1A:
# 2682
({void*_tmp861=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4A3,_tmp4A2);t=_tmp861;});goto _LL0;case 11U: _LL1D: _tmp4A4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL1E:
# 2708
({void*_tmp862=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4A4);t=_tmp862;});goto _LL0;case 13U: _LL1F: _tmp4A6=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4A5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL20:
# 2710
({void*_tmp863=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4A6,_tmp4A5);t=_tmp863;});goto _LL0;case 14U: _LL21: _tmp4A9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4A8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp4A7=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp475)->f4;_LL22:
# 2712
({void*_tmp864=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4A9,_tmp4A8,_tmp4A7);t=_tmp864;});goto _LL0;case 15U: _LL23: _tmp4AA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL24:
# 2714
({void*_tmp865=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4AA);t=_tmp865;});goto _LL0;case 16U: _LL25: _tmp4AC=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4AB=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL26:
# 2716
({void*_tmp866=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4AC,e,_tmp4AB);t=_tmp866;});goto _LL0;case 18U: _LL27: _tmp4AD=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL28: {
# 2718
void*_tmp47D=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4AD);
({void*_tmp867=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp47D);t=_tmp867;});goto _LL0;}case 17U: _LL29: _tmp4AE=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL2A:
# 2721
({void*_tmp868=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4AE);t=_tmp868;});goto _LL0;case 19U: _LL2B: _tmp4B0=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4AF=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL2C:
# 2723
({void*_tmp869=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4B0,_tmp4AF);t=_tmp869;});goto _LL0;case 20U: _LL2D: _tmp4B1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL2E:
# 2725
({void*_tmp86A=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4B1);t=_tmp86A;});goto _LL0;case 21U: _LL2F: _tmp4B5=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4B4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp4B3=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_tmp4B2=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp475)->f4;_LL30:
# 2727
({void*_tmp86B=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4B5,_tmp4B4,_tmp4B3,_tmp4B2);t=_tmp86B;});goto _LL0;case 22U: _LL31: _tmp4B9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4B8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp4B7=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_tmp4B6=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp475)->f4;_LL32:
# 2729
({void*_tmp86C=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4B9,_tmp4B8,_tmp4B7,_tmp4B6);t=_tmp86C;});goto _LL0;case 23U: _LL33: _tmp4BB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4BA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL34:
# 2731
({void*_tmp86D=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4BB,_tmp4BA);t=_tmp86D;});goto _LL0;case 24U: _LL35: _tmp4BC=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL36:
# 2733
({void*_tmp86E=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4BC);t=_tmp86E;});goto _LL0;case 25U: _LL37: _tmp4BE=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4BD=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL38:
# 2735
({void*_tmp86F=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4BE,_tmp4BD);t=_tmp86F;});goto _LL0;case 26U: _LL39: _tmp4BF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL3A: {
# 2739
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp47E=Cyc_Tcutil_compress(*topt);void*_tmp47F=_tmp47E;void**_tmp482;struct Cyc_Absyn_Tqual*_tmp481;union Cyc_Absyn_Constraint*_tmp480;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47F)->tag == 8U){_LL5B: _tmp482=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47F)->f1).elt_type;_tmp481=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47F)->f1).tq;_tmp480=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47F)->f1).zero_term;_LL5C:
# 2745
 elt_topt=_tmp482;
elt_tqopt=_tmp481;
({int _tmp870=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp480);zero_term=_tmp870;});
goto _LL5A;}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}
# 2752
({void*_tmp871=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4BF);t=_tmp871;});goto _LL0;}case 37U: _LL3B: _tmp4C0=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL3C:
# 2754
({void*_tmp872=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4C0);t=_tmp872;});goto _LL0;case 27U: _LL3D: _tmp4C4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4C3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp4C2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_tmp4C1=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp475)->f4;_LL3E:
# 2756
({void*_tmp873=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4C4,_tmp4C3,_tmp4C2,_tmp4C1);t=_tmp873;});goto _LL0;case 28U: _LL3F: _tmp4C7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4C6=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp4C5=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_LL40:
# 2758
({void*_tmp874=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp4C7,_tmp4C6,_tmp4C5);t=_tmp874;});goto _LL0;case 29U: _LL41: _tmp4CB=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4CA=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp4C9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_tmp4C8=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp475)->f4;_LL42:
# 2760
({void*_tmp875=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp4CB,_tmp4CA,_tmp4C9,_tmp4C8);t=_tmp875;});goto _LL0;case 30U: _LL43: _tmp4CD=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4CC=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL44:
# 2762
({void*_tmp876=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp4CD,_tmp4CC);t=_tmp876;});goto _LL0;case 31U: _LL45: _tmp4D0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4CF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_tmp4CE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp475)->f3;_LL46:
# 2764
({void*_tmp877=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp4D0,_tmp4CF,_tmp4CE);t=_tmp877;});goto _LL0;case 32U: _LL47: _tmp4D2=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4D1=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL48:
# 2766
({void*_tmp878=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->tag=13U,_tmp483->f1=_tmp4D2->name,_tmp483->f2=_tmp4D2;_tmp483;});t=_tmp878;});goto _LL0;case 33U: _LL49: _tmp4D4=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4D3=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL4A:
# 2768
 t=_tmp4D4;goto _LL0;case 34U: _LL4B: _tmp4D9=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp475)->f1).is_calloc;_tmp4D8=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp475)->f1).rgn;_tmp4D7=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp475)->f1).elt_type;_tmp4D6=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp475)->f1).num_elts;_tmp4D5=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp475)->f1).fat_result;_LL4C:
# 2770
({void*_tmp879=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp4D8,_tmp4D7,_tmp4D6,_tmp4D9,_tmp4D5);t=_tmp879;});goto _LL0;case 35U: _LL4D: _tmp4DB=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4DA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL4E:
# 2772
({void*_tmp87A=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp4DB,_tmp4DA);t=_tmp87A;});goto _LL0;case 38U: _LL4F: _tmp4DD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_tmp4DC=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp475)->f2;_LL50:
# 2774
({void*_tmp87B=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp4DD,_tmp4DC);t=_tmp87B;});goto _LL0;case 39U: _LL51: _tmp4DE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp475)->f1;_LL52:
# 2776
 if(!te->allow_valueof)
({void*_tmp484=0U;({unsigned int _tmp87D=e->loc;struct _dyneither_ptr _tmp87C=({const char*_tmp485="valueof(-) can only occur within types";_tag_dyneither(_tmp485,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp87D,_tmp87C,_tag_dyneither(_tmp484,sizeof(void*),0U));});});
# 2784
t=Cyc_Absyn_sint_typ;
goto _LL0;default: _LL53: _LL54:
# 2787
({void*_tmp486=0U;({unsigned int _tmp87F=e->loc;struct _dyneither_ptr _tmp87E=({const char*_tmp487="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp487,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp87F,_tmp87E,_tag_dyneither(_tmp486,sizeof(void*),0U));});});
({void*_tmp880=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp880;});}_LL0:;}
# 2790
e->topt=t;}
