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
# 148
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 151
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
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple14{int f1;void*f2;};
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
({void*_tmp0=0U;({unsigned int _tmp51D=loc;struct _dyneither_ptr _tmp51C=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp51D,_tmp51C,_tag_dyneither(_tmp0,sizeof(void*),0U));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 54
return*topt;}}
# 63
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 67
if(topt == 0){
# 69
({void*_tmp51F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp51E=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp2;_tmp2.tag=26U;_tmp2.f1=des;_tmp2;});_tmp1[0]=_tmp51E;});_tmp1;});e->r=_tmp51F;});
return;}{
# 72
void*t=*topt;
void*_tmp3=Cyc_Tcutil_compress(t);void*_tmp4=_tmp3;void*_tmp13;struct Cyc_Absyn_Tqual _tmp12;union Cyc_Absyn_AggrInfoU _tmp11;switch(*((int*)_tmp4)){case 11U: _LL1: _tmp11=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1).aggr_info;_LL2:
# 75
{union Cyc_Absyn_AggrInfoU _tmp5=_tmp11;struct Cyc_Absyn_Aggrdecl*_tmpA;if((_tmp5.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp6=0U;({struct _dyneither_ptr _tmp520=({const char*_tmp7="struct type not properly set";_tag_dyneither(_tmp7,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp520,_tag_dyneither(_tmp6,sizeof(void*),0U));});});}else{_LLC: _tmpA=*(_tmp5.KnownAggr).val;_LLD:
({void*_tmp522=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp8=_cycalloc(sizeof(*_tmp8));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp521=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp9;_tmp9.tag=29U;_tmp9.f1=_tmpA->name;_tmp9.f2=0;_tmp9.f3=des;_tmp9.f4=_tmpA;_tmp9;});_tmp8[0]=_tmp521;});_tmp8;});e->r=_tmp522;});}_LL9:;}
# 79
goto _LL0;case 8U: _LL3: _tmp13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).elt_type;_tmp12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).tq;_LL4:
({void*_tmp524=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp523=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpC;_tmpC.tag=26U;_tmpC.f1=des;_tmpC;});_tmpB[0]=_tmp523;});_tmpB;});e->r=_tmp524;});goto _LL0;case 12U: _LL5: _LL6:
({void*_tmp526=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD=_cycalloc(sizeof(*_tmpD));({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp525=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE;_tmpE.tag=30U;_tmpE.f1=t;_tmpE.f2=des;_tmpE;});_tmpD[0]=_tmp525;});_tmpD;});e->r=_tmp526;});goto _LL0;default: _LL7: _LL8:
({void*_tmp528=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp527=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp10;_tmp10.tag=26U;_tmp10.f1=des;_tmp10;});_tmpF[0]=_tmp527;});_tmpF;});e->r=_tmp528;});goto _LL0;}_LL0:;};}
# 89
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 93
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 99
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp14=e->r;void*_tmp15=_tmp14;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->tag == 4U){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->f2 == 0){_LL1: _LL2:
({void*_tmp16=0U;({unsigned int _tmp52A=e->loc;struct _dyneither_ptr _tmp529=({const char*_tmp17="assignment in test";_tag_dyneither(_tmp17,sizeof(char),19U);});Cyc_Tcutil_warn(_tmp52A,_tmp529,_tag_dyneither(_tmp16,sizeof(void*),0U));});});goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 107
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0U;({struct _dyneither_ptr _tmp52B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp1B.f1=_tmp52B;});({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0U;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp18[2U]={& _tmp1A,& _tmp1B};({unsigned int _tmp52D=e->loc;struct _dyneither_ptr _tmp52C=({const char*_tmp19="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp19,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp52D,_tmp52C,_tag_dyneither(_tmp18,sizeof(void*),2U));});});});});
# 116
{void*_tmp1C=e->r;void*_tmp1D=_tmp1C;enum Cyc_Absyn_Primop _tmp26;struct Cyc_List_List*_tmp25;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->tag == 3U){_LL1: _tmp26=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f1;_tmp25=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f2;_LL2:
# 118
 if(_tmp26 == Cyc_Absyn_Eq  || _tmp26 == Cyc_Absyn_Neq){
struct _tuple0 _tmp1E=({struct _tuple0 _tmp24;_tmp24.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25))->hd)->topt);_tmp24.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25->tl))->hd)->topt);_tmp24;});struct _tuple0 _tmp1F=_tmp1E;void*_tmp23;void*_tmp22;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->tag == 15U){_LL6: _tmp23=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->f1;_tmp22=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->f1;_LL7:
# 121
 return({struct Cyc_Tcexp_TestEnv _tmp20;({struct _tuple0*_tmp52E=({struct _tuple0*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->f1=_tmp23;_tmp21->f2=_tmp22;_tmp21;});_tmp20.eq=_tmp52E;});_tmp20.isTrue=_tmp26 == Cyc_Absyn_Eq;_tmp20;});}else{goto _LL8;}}else{_LL8: _LL9:
# 123
 goto _LL5;}_LL5:;}
# 126
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
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
{union Cyc_Absyn_Cnst _tmp28=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp29=_tmp28;struct _dyneither_ptr _tmp65;struct _dyneither_ptr _tmp64;enum Cyc_Absyn_Sign _tmp63;int _tmp62;int _tmp61;enum Cyc_Absyn_Sign _tmp60;enum Cyc_Absyn_Sign _tmp5F;switch((_tmp29.Wstring_c).tag){case 2U: switch(((_tmp29.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1: _LL2:
 t=Cyc_Absyn_schar_typ;goto _LL0;case Cyc_Absyn_Unsigned: _LL3: _LL4:
 t=Cyc_Absyn_uchar_typ;goto _LL0;default: _LL5: _LL6:
 t=Cyc_Absyn_char_typ;goto _LL0;}case 3U: _LL7: _LL8:
({void*_tmp52F=Cyc_Absyn_wchar_typ();t=_tmp52F;});goto _LL0;case 4U: _LL9: _tmp5F=((_tmp29.Short_c).val).f1;_LLA:
# 165
 t=_tmp5F == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL0;case 6U: _LLB: _tmp60=((_tmp29.LongLong_c).val).f1;_LLC:
# 167
 t=_tmp60 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL0;case 7U: _LLD: _tmp61=((_tmp29.Float_c).val).f2;_LLE:
({void*_tmp530=Cyc_Absyn_float_typ(_tmp61);t=_tmp530;});goto _LL0;case 5U: _LLF: _tmp63=((_tmp29.Int_c).val).f1;_tmp62=((_tmp29.Int_c).val).f2;_LL10:
# 170
 if(topt == 0)
t=_tmp63 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 177
void*_tmp2A=Cyc_Tcutil_compress(*topt);void*_tmp2B=_tmp2A;void*_tmp44;void*_tmp43;struct Cyc_Absyn_Tqual _tmp42;void*_tmp41;union Cyc_Absyn_Constraint*_tmp40;union Cyc_Absyn_Constraint*_tmp3F;union Cyc_Absyn_Constraint*_tmp3E;enum Cyc_Absyn_Sign _tmp3D;enum Cyc_Absyn_Sign _tmp3C;enum Cyc_Absyn_Sign _tmp3B;enum Cyc_Absyn_Sign _tmp3A;switch(*((int*)_tmp2B)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f2){case Cyc_Absyn_Char_sz: _LL18: _tmp3A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL19:
# 179
{enum Cyc_Absyn_Sign _tmp2C=_tmp3A;switch(_tmp2C){case Cyc_Absyn_Unsigned: _LL27: _LL28:
 t=Cyc_Absyn_uchar_typ;goto _LL26;case Cyc_Absyn_Signed: _LL29: _LL2A:
 t=Cyc_Absyn_schar_typ;goto _LL26;default: _LL2B: _LL2C:
 t=Cyc_Absyn_char_typ;goto _LL26;}_LL26:;}
# 184
({union Cyc_Absyn_Cnst _tmp531=Cyc_Absyn_Char_c(_tmp3A,(char)_tmp62);*c=_tmp531;});
goto _LL17;case Cyc_Absyn_Short_sz: _LL1A: _tmp3B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL1B:
# 187
 t=_tmp3B == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
({union Cyc_Absyn_Cnst _tmp532=Cyc_Absyn_Short_c(_tmp3B,(short)_tmp62);*c=_tmp532;});
goto _LL17;case Cyc_Absyn_Int_sz: _LL1C: _tmp3C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL1D:
# 191
 t=_tmp3C == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 194
({union Cyc_Absyn_Cnst _tmp533=Cyc_Absyn_Int_c(_tmp3C,_tmp62);*c=_tmp533;});
goto _LL17;case Cyc_Absyn_Long_sz: _LL1E: _tmp3D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL1F:
# 197
 t=_tmp3D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
# 200
({union Cyc_Absyn_Cnst _tmp534=Cyc_Absyn_Int_c(_tmp3D,_tmp62);*c=_tmp534;});
goto _LL17;default: goto _LL24;}case 5U: _LL20: _tmp43=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_typ;_tmp42=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_tq;_tmp41=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).rgn;_tmp40=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).nullable;_tmp3F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).bounds;_tmp3E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;if(_tmp62 == 0){_LL21: {
# 204
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0U,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp40))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0U;({struct _dyneither_ptr _tmp535=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));_tmp2F.f1=_tmp535;});({void*_tmp2D[1U]={& _tmp2F};({unsigned int _tmp537=e->loc;struct _dyneither_ptr _tmp536=({const char*_tmp2E="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp2E,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp537,_tmp536,_tag_dyneither(_tmp2D,sizeof(void*),1U));});});});{
struct Cyc_List_List*_tmp30=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp53B=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31=_cycalloc(sizeof(*_tmp31));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp53A=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp32;_tmp32.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp539=({struct Cyc_Absyn_PtrInfo _tmp33;_tmp33.elt_typ=_tmp43;_tmp33.elt_tq=_tmp42;({struct Cyc_Absyn_PtrAtts _tmp538=({(_tmp33.ptr_atts).rgn=_tmp41;(_tmp33.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp33.ptr_atts).bounds=_tmp3F;(_tmp33.ptr_atts).zero_term=_tmp3E;(_tmp33.ptr_atts).ptrloc=0;_tmp33.ptr_atts;});_tmp33.ptr_atts=_tmp538;});_tmp33;});_tmp32.f1=_tmp539;});_tmp32;});_tmp31[0]=_tmp53A;});_tmp31;});t=_tmp53B;});
# 212
goto _LL17;};}}else{goto _LL24;}case 19U: _LL22: _tmp44=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B)->f1;_LL23: {
# 214
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp35=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp38=_cycalloc(sizeof(*_tmp38));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp53D=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp39;_tmp39.tag=18U;({struct Cyc_Absyn_Exp*_tmp53C=Cyc_Absyn_uint_exp((unsigned int)_tmp62,0U);_tmp39.f1=_tmp53C;});_tmp39;});_tmp38[0]=_tmp53D;});_tmp38;});
# 221
({void*_tmp53F=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp36=_cycalloc(sizeof(*_tmp36));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp53E=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp37;_tmp37.tag=19U;_tmp37.f1=(void*)_tmp35;_tmp37;});_tmp36[0]=_tmp53E;});_tmp36;});t=_tmp53F;});
goto _LL17;}default: _LL24: _LL25:
# 224
 t=_tmp63 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL17;}_LL17:;}
# 227
goto _LL0;case 8U: _LL11: _tmp64=(_tmp29.String_c).val;_LL12:
# 229
 string_numelts=(int)_get_dyneither_size(_tmp64,sizeof(char));
_tmp65=_tmp64;goto _LL14;case 9U: _LL13: _tmp65=(_tmp29.Wstring_c).val;_LL14:
# 232
 if(string_numelts == 0){
({int _tmp540=Cyc_Tcexp_wchar_numelts(_tmp65);string_numelts=_tmp540;});
({void*_tmp541=Cyc_Absyn_wchar_typ();string_elt_typ=_tmp541;});}{
# 236
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp542=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp542,loc);});
elen->topt=Cyc_Absyn_uint_typ;{
# 241
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp45=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp543=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp4A;_tmp4A.tag=1U;_tmp4A.f1=elen;_tmp4A;});_tmp49[0]=_tmp543;});_tmp49;});
({void*_tmp547=({void*_tmp546=string_elt_typ;struct Cyc_Absyn_Tqual _tmp545=Cyc_Absyn_const_tqual(0U);void*_tmp544=(void*)_tmp45;Cyc_Absyn_atb_typ(_tmp546,(void*)& Cyc_Absyn_HeapRgn_val,_tmp545,_tmp544,Cyc_Absyn_true_conref);});t=_tmp547;});
# 244
if(topt != 0){
void*_tmp46=Cyc_Tcutil_compress(*topt);void*_tmp47=_tmp46;struct Cyc_Absyn_Tqual _tmp48;switch(*((int*)_tmp47)){case 8U: _LL2E: _tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->f1).tq;_LL2F:
# 249
 return({void*_tmp54A=string_elt_typ;struct Cyc_Absyn_Tqual _tmp549=_tmp48;struct Cyc_Absyn_Exp*_tmp548=elen;Cyc_Absyn_array_typ(_tmp54A,_tmp549,_tmp548,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0U);});case 5U: _LL30: _LL31:
# 251
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 257
({void*_tmp54F=({void*_tmp54E=string_elt_typ;void*_tmp54D=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp54C=
Cyc_Absyn_const_tqual(0U);
# 257
void*_tmp54B=(void*)_tmp45;Cyc_Absyn_atb_typ(_tmp54E,_tmp54D,_tmp54C,_tmp54B,Cyc_Absyn_true_conref);});t=_tmp54F;});
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
void*_tmp4B=Cyc_Tcutil_compress(*topt);void*_tmp4C=_tmp4B;void*_tmp58;struct Cyc_Absyn_Tqual _tmp57;void*_tmp56;union Cyc_Absyn_Constraint*_tmp55;union Cyc_Absyn_Constraint*_tmp54;union Cyc_Absyn_Constraint*_tmp53;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->tag == 5U){_LL35: _tmp58=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_typ;_tmp57=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_tq;_tmp56=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).rgn;_tmp55=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).bounds;_tmp53=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).zero_term;_LL36:
# 275
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp55))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp4F;_tmp4F.tag=0U;({struct _dyneither_ptr _tmp550=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));_tmp4F.f1=_tmp550;});({void*_tmp4D[1U]={& _tmp4F};({unsigned int _tmp552=e->loc;struct _dyneither_ptr _tmp551=({const char*_tmp4E="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp4E,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp552,_tmp551,_tag_dyneither(_tmp4D,sizeof(void*),1U));});});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp50=_cycalloc(sizeof(*_tmp50));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp555=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp51;_tmp51.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp554=({struct Cyc_Absyn_PtrInfo _tmp52;_tmp52.elt_typ=_tmp58;_tmp52.elt_tq=_tmp57;({struct Cyc_Absyn_PtrAtts _tmp553=({(_tmp52.ptr_atts).rgn=_tmp56;(_tmp52.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp52.ptr_atts).bounds=_tmp54;(_tmp52.ptr_atts).zero_term=_tmp53;(_tmp52.ptr_atts).ptrloc=0;_tmp52.ptr_atts;});_tmp52.ptr_atts=_tmp553;});_tmp52;});_tmp51.f1=_tmp554;});_tmp51;});_tmp50[0]=_tmp555;});_tmp50;});}else{_LL37: _LL38:
# 280
 goto _LL34;}_LL34:;}{
# 283
struct Cyc_List_List*_tmp59=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp55E=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp55D=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp5B;_tmp5B.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp55C=({struct Cyc_Absyn_PtrInfo _tmp5C;({void*_tmp556=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->v=_tmp59;_tmp5E;}));_tmp5C.elt_typ=_tmp556;});({struct Cyc_Absyn_Tqual _tmp557=
Cyc_Absyn_empty_tqual(0U);_tmp5C.elt_tq=_tmp557;});({struct Cyc_Absyn_PtrAtts _tmp55B=({({void*_tmp558=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->v=_tmp59;_tmp5D;}));(_tmp5C.ptr_atts).rgn=_tmp558;});(_tmp5C.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmp559=
# 288
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp5C.ptr_atts).bounds=_tmp559;});({union Cyc_Absyn_Constraint*_tmp55A=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp5C.ptr_atts).zero_term=_tmp55A;});(_tmp5C.ptr_atts).ptrloc=0;_tmp5C.ptr_atts;});_tmp5C.ptr_atts=_tmp55B;});_tmp5C;});_tmp5B.f1=_tmp55C;});_tmp5B;});_tmp5A[0]=_tmp55D;});_tmp5A;});
# 284
t=_tmp55E;});
# 290
goto _LL0;};}_LL0:;}
# 292
return t;}
# 296
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 301
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp66=*((void**)_check_null(b));void*_tmp67=_tmp66;struct Cyc_Absyn_Vardecl*_tmp75;struct Cyc_Absyn_Vardecl*_tmp74;struct Cyc_Absyn_Vardecl*_tmp73;struct Cyc_Absyn_Fndecl*_tmp72;struct Cyc_Absyn_Vardecl*_tmp71;struct _tuple1*_tmp70;switch(*((int*)_tmp67)){case 0U: _LL1: _tmp70=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp67)->f1;_LL2:
# 304
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp68=_cycalloc(sizeof(*_tmp68));({struct Cyc_Core_Impossible_exn_struct _tmp560=({struct Cyc_Core_Impossible_exn_struct _tmp69;_tmp69.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp55F=({const char*_tmp6A="unresolved binding in tcVar";_tag_dyneither(_tmp6A,sizeof(char),28U);});_tmp69.f1=_tmp55F;});_tmp69;});_tmp68[0]=_tmp560;});_tmp68;}));case 1U: _LL3: _tmp71=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp67)->f1;_LL4:
# 308
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp71->name,1);
return _tmp71->type;case 2U: _LL5: _tmp72=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp67)->f1;_LL6:
# 315
 if(_tmp72->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp72->name,1);
return Cyc_Tcutil_fndecl2typ(_tmp72);case 5U: _LL7: _tmp73=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp67)->f1;_LL8:
 _tmp74=_tmp73;goto _LLA;case 4U: _LL9: _tmp74=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp67)->f1;_LLA:
 _tmp75=_tmp74;goto _LLC;default: _LLB: _tmp75=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp67)->f1;_LLC:
# 321
 if(te->allow_valueof){
void*_tmp6B=Cyc_Tcutil_compress(_tmp75->type);void*_tmp6C=_tmp6B;void*_tmp6F;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6C)->tag == 19U){_LLE: _tmp6F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6C)->f1;_LLF:
# 324
({void*_tmp562=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp561=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6E;_tmp6E.tag=39U;_tmp6E.f1=_tmp6F;_tmp6E;});_tmp6D[0]=_tmp561;});_tmp6D;});e->r=_tmp562;});
goto _LLD;}else{_LL10: _LL11:
 goto _LLD;}_LLD:;}
# 329
return _tmp75->type;}_LL0:;}
# 333
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 340
struct Cyc_List_List*desc_types;
{void*_tmp76=fmt->r;void*_tmp77=_tmp76;struct _dyneither_ptr _tmp7C;struct _dyneither_ptr _tmp7B;switch(*((int*)_tmp77)){case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp77)->f1).String_c).tag == 8){_LL1: _tmp7B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp77)->f1).String_c).val;_LL2:
 _tmp7C=_tmp7B;goto _LL4;}else{goto _LL5;}case 14U: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2)->r)->tag == 0U){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2)->r)->f1).String_c).tag == 8){_LL3: _tmp7C=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2)->r)->f1).String_c).val;_LL4:
# 344
({struct Cyc_List_List*_tmp563=type_getter(te,(struct _dyneither_ptr)_tmp7C,fmt->loc);desc_types=_tmp563;});goto _LL0;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
# 348
 if(opt_args != 0){
struct Cyc_List_List*_tmp78=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp78 != 0;_tmp78=_tmp78->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp78->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp78->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp78->hd))
({void*_tmp79=0U;({unsigned int _tmp565=((struct Cyc_Absyn_Exp*)_tmp78->hd)->loc;struct _dyneither_ptr _tmp564=({const char*_tmp7A="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp7A,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp565,_tmp564,_tag_dyneither(_tmp79,sizeof(void*),0U));});});}}
# 357
return;}_LL0:;}
# 359
if(opt_args != 0){
struct Cyc_List_List*_tmp7D=(struct Cyc_List_List*)opt_args->v;
# 362
for(0;desc_types != 0  && _tmp7D != 0;
(((desc_types=desc_types->tl,_tmp7D=_tmp7D->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp7D->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0U;({struct _dyneither_ptr _tmp566=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp81.f1=_tmp566;});({struct Cyc_String_pa_PrintArg_struct _tmp80;_tmp80.tag=0U;({struct _dyneither_ptr _tmp567=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp80.f1=_tmp567;});({void*_tmp7E[2U]={& _tmp80,& _tmp81};({unsigned int _tmp569=e->loc;struct _dyneither_ptr _tmp568=({const char*_tmp7F="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp7F,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp569,_tmp568,_tag_dyneither(_tmp7E,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}
# 373
if(alias_coercion)
({struct Cyc_List_List*_tmp56A=({struct Cyc_List_List*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->hd=(void*)arg_cnt;_tmp82->tl=*alias_arg_exps;_tmp82;});*alias_arg_exps=_tmp56A;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp83=0U;({unsigned int _tmp56C=((struct Cyc_Absyn_Exp*)_tmp7D->hd)->loc;struct _dyneither_ptr _tmp56B=({const char*_tmp84="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp84,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp56C,_tmp56B,_tag_dyneither(_tmp83,sizeof(void*),0U));});});}
# 380
if(desc_types != 0)
({void*_tmp85=0U;({unsigned int _tmp56E=fmt->loc;struct _dyneither_ptr _tmp56D=({const char*_tmp86="too few arguments";_tag_dyneither(_tmp86,sizeof(char),18U);});Cyc_Tcutil_terr(_tmp56E,_tmp56D,_tag_dyneither(_tmp85,sizeof(void*),0U));});});
if(_tmp7D != 0){
({void*_tmp87=0U;({unsigned int _tmp570=((struct Cyc_Absyn_Exp*)_tmp7D->hd)->loc;struct _dyneither_ptr _tmp56F=({const char*_tmp88="too many arguments";_tag_dyneither(_tmp88,sizeof(char),19U);});Cyc_Tcutil_terr(_tmp570,_tmp56F,_tag_dyneither(_tmp87,sizeof(void*),0U));});});
# 385
for(0;_tmp7D != 0;_tmp7D=_tmp7D->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp7D->hd);}}}}
# 390
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 392
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp89=p;switch(_tmp89){case Cyc_Absyn_Plus: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_Minus: _LL3: _LL4:
# 396
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp8C;_tmp8C.tag=0U;({struct _dyneither_ptr _tmp571=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp8C.f1=_tmp571;});({void*_tmp8A[1U]={& _tmp8C};({unsigned int _tmp573=loc;struct _dyneither_ptr _tmp572=({const char*_tmp8B="expecting numeric type but found %s";_tag_dyneither(_tmp8B,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp573,_tmp572,_tag_dyneither(_tmp8A,sizeof(void*),1U));});});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL5: _LL6:
# 400
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp8F;_tmp8F.tag=0U;({struct _dyneither_ptr _tmp574=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp8F.f1=_tmp574;});({void*_tmp8D[1U]={& _tmp8F};({unsigned int _tmp576=loc;struct _dyneither_ptr _tmp575=({const char*_tmp8E="expecting integral or * type but found %s";_tag_dyneither(_tmp8E,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp576,_tmp575,_tag_dyneither(_tmp8D,sizeof(void*),1U));});});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL7: _LL8:
# 405
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp92;_tmp92.tag=0U;({struct _dyneither_ptr _tmp577=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp92.f1=_tmp577;});({void*_tmp90[1U]={& _tmp92};({unsigned int _tmp579=loc;struct _dyneither_ptr _tmp578=({const char*_tmp91="expecting integral type but found %s";_tag_dyneither(_tmp91,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp579,_tmp578,_tag_dyneither(_tmp90,sizeof(void*),1U));});});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL9: _LLA:
# 409
{void*_tmp93=t;union Cyc_Absyn_Constraint*_tmp9C;switch(*((int*)_tmp93)){case 8U: _LLE: _LLF:
 goto _LLD;case 5U: _LL10: _tmp9C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).bounds;_LL11:
# 412
{void*_tmp94=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9C);void*_tmp95=_tmp94;struct Cyc_Absyn_Exp*_tmp98;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp95)->tag == 0U){_LL15: _LL16:
 goto _LL14;}else{_LL17: _tmp98=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp95)->f1;_LL18:
# 415
 if(!Cyc_Evexp_c_can_eval(_tmp98) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp96=0U;({unsigned int _tmp57B=loc;struct _dyneither_ptr _tmp57A=({const char*_tmp97="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp97,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp57B,_tmp57A,_tag_dyneither(_tmp96,sizeof(void*),0U));});});
goto _LL14;}_LL14:;}
# 419
goto _LLD;default: _LL12: _LL13:
# 421
({struct Cyc_String_pa_PrintArg_struct _tmp9B;_tmp9B.tag=0U;({struct _dyneither_ptr _tmp57C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9B.f1=_tmp57C;});({void*_tmp99[1U]={& _tmp9B};({unsigned int _tmp57E=loc;struct _dyneither_ptr _tmp57D=({const char*_tmp9A="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp9A,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp57E,_tmp57D,_tag_dyneither(_tmp99,sizeof(void*),1U));});});});}_LLD:;}
# 423
return Cyc_Absyn_uint_typ;default: _LLB: _LLC:
({void*_tmp9D=0U;({struct _dyneither_ptr _tmp57F=({const char*_tmp9E="Non-unary primop";_tag_dyneither(_tmp9E,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp57F,_tag_dyneither(_tmp9D,sizeof(void*),0U));});});}_LL0:;}
# 429
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 432
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0U;({struct _dyneither_ptr _tmp580=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmpA1.f1=_tmp580;});({void*_tmp9F[1U]={& _tmpA1};({unsigned int _tmp582=e1->loc;struct _dyneither_ptr _tmp581=({const char*_tmpA0="type %s cannot be used here";_tag_dyneither(_tmpA0,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp582,_tmp581,_tag_dyneither(_tmp9F,sizeof(void*),1U));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 436
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpA4;_tmpA4.tag=0U;({struct _dyneither_ptr _tmp583=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmpA4.f1=_tmp583;});({void*_tmpA2[1U]={& _tmpA4};({unsigned int _tmp585=e2->loc;struct _dyneither_ptr _tmp584=({const char*_tmpA3="type %s cannot be used here";_tag_dyneither(_tmpA3,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp585,_tmp584,_tag_dyneither(_tmpA2,sizeof(void*),1U));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 440
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 445
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmpA5=t1;void*_tmpBB;struct Cyc_Absyn_Tqual _tmpBA;void*_tmpB9;union Cyc_Absyn_Constraint*_tmpB8;union Cyc_Absyn_Constraint*_tmpB7;union Cyc_Absyn_Constraint*_tmpB6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->tag == 5U){_LL1: _tmpBB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).elt_typ;_tmpBA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).elt_tq;_tmpB9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).rgn;_tmpB8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).nullable;_tmpB7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).bounds;_tmpB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).zero_term;_LL2:
# 450
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpBB),& Cyc_Tcutil_tmk))
({void*_tmpA6=0U;({unsigned int _tmp587=e1->loc;struct _dyneither_ptr _tmp586=({const char*_tmpA7="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpA7,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp587,_tmp586,_tag_dyneither(_tmpA6,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpA8=0U;({unsigned int _tmp589=e1->loc;struct _dyneither_ptr _tmp588=({const char*_tmpA9="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpA9,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp589,_tmp588,_tag_dyneither(_tmpA8,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmpAC;_tmpAC.tag=0U;({struct _dyneither_ptr _tmp58A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpAC.f1=_tmp58A;});({void*_tmpAA[1U]={& _tmpAC};({unsigned int _tmp58C=e2->loc;struct _dyneither_ptr _tmp58B=({const char*_tmpAB="expecting int but found %s";_tag_dyneither(_tmpAB,sizeof(char),27U);});Cyc_Tcutil_terr(_tmp58C,_tmp58B,_tag_dyneither(_tmpAA,sizeof(void*),1U));});});});{
void*_tmpAD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB7);void*_tmpAE=_tmpAD;struct Cyc_Absyn_Exp*_tmpB5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAE)->tag == 0U){_LL6: _LL7:
 return t1;}else{_LL8: _tmpB5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAE)->f1;_LL9:
# 461
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB6))
({void*_tmpAF=0U;({unsigned int _tmp58E=e1->loc;struct _dyneither_ptr _tmp58D=({const char*_tmpB0="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmpB0,sizeof(char),70U);});Cyc_Tcutil_warn(_tmp58E,_tmp58D,_tag_dyneither(_tmpAF,sizeof(void*),0U));});});{
# 469
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB1=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp591=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpB3;_tmpB3.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp590=({struct Cyc_Absyn_PtrInfo _tmpB4;_tmpB4.elt_typ=_tmpBB;_tmpB4.elt_tq=_tmpBA;({struct Cyc_Absyn_PtrAtts _tmp58F=({(_tmpB4.ptr_atts).rgn=_tmpB9;(_tmpB4.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmpB4.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref;(_tmpB4.ptr_atts).zero_term=_tmpB6;(_tmpB4.ptr_atts).ptrloc=0;_tmpB4.ptr_atts;});_tmpB4.ptr_atts=_tmp58F;});_tmpB4;});_tmpB3.f1=_tmp590;});_tmpB3;});_tmpB2[0]=_tmp591;});_tmpB2;});
# 473
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpB1,Cyc_Absyn_Other_coercion);
return(void*)_tmpB1;};}_LL5:;};}else{_LL3: _LL4:
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
({struct Cyc_String_pa_PrintArg_struct _tmpBF;_tmpBF.tag=0U;({struct _dyneither_ptr _tmp592=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 491
Cyc_Absynpp_typ2string(t2));_tmpBF.f1=_tmp592;});({struct Cyc_String_pa_PrintArg_struct _tmpBE;_tmpBE.tag=0U;({struct _dyneither_ptr _tmp593=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpBE.f1=_tmp593;});({void*_tmpBC[2U]={& _tmpBE,& _tmpBF};({unsigned int _tmp595=e1->loc;struct _dyneither_ptr _tmp594=({const char*_tmpBD="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpBD,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp595,_tmp594,_tag_dyneither(_tmpBC,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}
# 494
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp596=t1_elt;Cyc_Tcutil_unify(_tmp596,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpC3;_tmpC3.tag=0U;({struct _dyneither_ptr _tmp597=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 499
Cyc_Absynpp_typ2string(t2));_tmpC3.f1=_tmp597;});({struct Cyc_String_pa_PrintArg_struct _tmpC2;_tmpC2.tag=0U;({struct _dyneither_ptr _tmp598=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpC2.f1=_tmp598;});({void*_tmpC0[2U]={& _tmpC2,& _tmpC3};({unsigned int _tmp59A=e1->loc;struct _dyneither_ptr _tmp599=({const char*_tmpC1="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpC1,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp59A,_tmp599,_tag_dyneither(_tmpC0,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}
# 503
({void*_tmpC4=0U;({unsigned int _tmp59C=e1->loc;struct _dyneither_ptr _tmp59B=({const char*_tmpC5="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpC5,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp59C,_tmp59B,_tag_dyneither(_tmpC4,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5A1=te;struct Cyc_Absyn_Exp*_tmp5A0=e1;Cyc_Tcutil_unchecked_cast(_tmp5A1,_tmp5A0,({void*_tmp59F=t1_elt;void*_tmp59E=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp59D=
Cyc_Absyn_empty_tqual(0U);
# 504
Cyc_Absyn_star_typ(_tmp59F,_tmp59E,_tmp59D,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});
# 507
return Cyc_Absyn_sint_typ;}else{
# 509
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpC6=0U;({unsigned int _tmp5A3=e1->loc;struct _dyneither_ptr _tmp5A2=({const char*_tmpC7="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpC7,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5A3,_tmp5A2,_tag_dyneither(_tmpC6,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpC8=0U;({unsigned int _tmp5A5=e1->loc;struct _dyneither_ptr _tmp5A4=({const char*_tmpC9="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpC9,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5A5,_tmp5A4,_tag_dyneither(_tmpC8,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpCD;_tmpCD.tag=0U;({struct _dyneither_ptr _tmp5A6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpCD.f1=_tmp5A6;});({struct Cyc_String_pa_PrintArg_struct _tmpCC;_tmpCC.tag=0U;({struct _dyneither_ptr _tmp5A7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpCC.f1=_tmp5A7;});({void*_tmpCA[2U]={& _tmpCC,& _tmpCD};({unsigned int _tmp5A9=e2->loc;struct _dyneither_ptr _tmp5A8=({const char*_tmpCB="expecting either %s or int but found %s";_tag_dyneither(_tmpCB,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp5A9,_tmp5A8,_tag_dyneither(_tmpCA,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}
# 518
return t1;}}}
# 522
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5AA=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5AA,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpCE=0U;({unsigned int _tmp5AC=e1->loc;struct _dyneither_ptr _tmp5AB=({const char*_tmpCF="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpCF,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp5AC,_tmp5AB,_tag_dyneither(_tmpCE,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmp5B1=te;struct Cyc_Absyn_Exp*_tmp5B0=e2;Cyc_Tcutil_unchecked_cast(_tmp5B1,_tmp5B0,({void*_tmp5AF=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5AE=
Cyc_Tcutil_pointer_region(t2);
# 527
struct Cyc_Absyn_Tqual _tmp5AD=
# 529
Cyc_Absyn_empty_tqual(0U);
# 527
Cyc_Absyn_star_typ(_tmp5AF,_tmp5AE,_tmp5AD,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});}
# 532
({void*_tmpD0=0U;({unsigned int _tmp5B3=e1->loc;struct _dyneither_ptr _tmp5B2=({const char*_tmpD1="thin pointer subtraction!";_tag_dyneither(_tmpD1,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp5B3,_tmp5B2,_tag_dyneither(_tmpD0,sizeof(void*),0U));});});
return Cyc_Absyn_sint_typ;}
# 535
({void*_tmpD2=0U;({unsigned int _tmp5B5=e1->loc;struct _dyneither_ptr _tmp5B4=({const char*_tmpD3="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpD3,sizeof(char),56U);});Cyc_Tcutil_warn(_tmp5B5,_tmp5B4,_tag_dyneither(_tmpD2,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 538
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpD4=0U;({unsigned int _tmp5B7=e1->loc;struct _dyneither_ptr _tmp5B6=({const char*_tmpD5="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpD5,sizeof(char),51U);});Cyc_Tcutil_warn(_tmp5B7,_tmp5B6,_tag_dyneither(_tmpD4,sizeof(void*),0U));});});
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
void*_tmp5B8=t1;Cyc_Tcutil_unify(_tmp5B8,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
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
struct _tuple0 _tmpD6=({struct _tuple0 _tmpDA;({void*_tmp5B9=Cyc_Tcutil_compress(t1);_tmpDA.f1=_tmp5B9;});({void*_tmp5BA=Cyc_Tcutil_compress(t2);_tmpDA.f2=_tmp5BA;});_tmpDA;});struct _tuple0 _tmpD7=_tmpD6;void*_tmpD9;void*_tmpD8;switch(*((int*)_tmpD7.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7.f2)->tag == 5U){_LL1: _tmpD9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7.f1)->f1).elt_typ;_tmpD8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7.f2)->f1).elt_typ;_LL2:
# 576
 if(Cyc_Tcutil_unify(_tmpD9,_tmpD8))return Cyc_Absyn_sint_typ;goto _LL0;}else{goto _LL5;}case 15U: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD7.f2)->tag == 15U){_LL3: _LL4:
# 578
 return Cyc_Absyn_sint_typ;}else{goto _LL5;}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 581
({struct Cyc_String_pa_PrintArg_struct _tmpDE;_tmpDE.tag=0U;({struct _dyneither_ptr _tmp5BB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpDE.f1=_tmp5BB;});({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0U;({struct _dyneither_ptr _tmp5BC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpDD.f1=_tmp5BC;});({void*_tmpDB[2U]={& _tmpDD,& _tmpDE};({unsigned int _tmp5BE=loc;struct _dyneither_ptr _tmp5BD=({const char*_tmpDC="comparison not allowed between %s and %s";_tag_dyneither(_tmpDC,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5BE,_tmp5BD,_tag_dyneither(_tmpDB,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 588
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 590
if(({void*_tmp5BF=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5BF == Cyc_Absyn_sint_typ;}))
return Cyc_Absyn_sint_typ;{
# 596
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpDF=({struct _tuple0 _tmpE3;_tmpE3.f1=t1;_tmpE3.f2=t2;_tmpE3;});struct _tuple0 _tmpE0=_tmpDF;void*_tmpE2;void*_tmpE1;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpE0.f1)->tag == 15U){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpE0.f2)->tag == 15U){_LL1: _tmpE2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpE0.f1)->f1;_tmpE1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpE0.f2)->f1;_LL2:
# 601
 return Cyc_Absyn_sint_typ;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 604
({struct Cyc_String_pa_PrintArg_struct _tmpE7;_tmpE7.tag=0U;({struct _dyneither_ptr _tmp5C0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpE7.f1=_tmp5C0;});({struct Cyc_String_pa_PrintArg_struct _tmpE6;_tmpE6.tag=0U;({struct _dyneither_ptr _tmp5C1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpE6.f1=_tmp5C1;});({void*_tmpE4[2U]={& _tmpE6,& _tmpE7};({unsigned int _tmp5C3=loc;struct _dyneither_ptr _tmp5C2=({const char*_tmpE5="comparison not allowed between %s and %s";_tag_dyneither(_tmpE5,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp5C3,_tmp5C2,_tag_dyneither(_tmpE4,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 612
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 614
enum Cyc_Absyn_Primop _tmpE8=p;switch(_tmpE8){case Cyc_Absyn_Plus: _LL1: _LL2:
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
({void*_tmpE9=0U;({struct _dyneither_ptr _tmp5C4=({const char*_tmpEA="bad binary primop";_tag_dyneither(_tmpEA,sizeof(char),18U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5C4,_tag_dyneither(_tmpE9,sizeof(void*),0U));});});}_LL0:;}
# 643
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 651
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpEB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpEC=Cyc_Tcexp_tcExp(te,topt,_tmpEB);
if(!Cyc_Tcutil_is_numeric(_tmpEB))
({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0U;({struct _dyneither_ptr _tmp5C5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpEC));_tmpEF.f1=_tmp5C5;});({void*_tmpED[1U]={& _tmpEF};({unsigned int _tmp5C7=_tmpEB->loc;struct _dyneither_ptr _tmp5C6=({const char*_tmpEE="expecting numeric type but found %s";_tag_dyneither(_tmpEE,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5C7,_tmp5C6,_tag_dyneither(_tmpED,sizeof(void*),1U));});});});
return _tmpEC;}
# 658
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpF0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpF1=_tmpF0;switch(_tmpF1){case 0U: _LL1: _LL2:
 return({void*_tmpF2=0U;({struct Cyc_Tcenv_Tenv*_tmp5CB=te;unsigned int _tmp5CA=loc;void**_tmp5C9=topt;struct _dyneither_ptr _tmp5C8=({const char*_tmpF3="primitive operator has 0 arguments";_tag_dyneither(_tmpF3,sizeof(char),35U);});Cyc_Tcexp_expr_err(_tmp5CB,_tmp5CA,_tmp5C9,_tmp5C8,_tag_dyneither(_tmpF2,sizeof(void*),0U));});});case 1U: _LL3: _LL4:
({void*_tmp5CC=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp5CC;});goto _LL0;case 2U: _LL5: _LL6:
({void*_tmp5D2=({struct Cyc_Tcenv_Tenv*_tmp5D1=te;unsigned int _tmp5D0=loc;void**_tmp5CF=topt;enum Cyc_Absyn_Primop _tmp5CE=p;struct Cyc_Absyn_Exp*_tmp5CD=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp5D1,_tmp5D0,_tmp5CF,_tmp5CE,_tmp5CD,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp5D2;});goto _LL0;default: _LL7: _LL8:
 return({void*_tmpF4=0U;({struct Cyc_Tcenv_Tenv*_tmp5D6=te;unsigned int _tmp5D5=loc;void**_tmp5D4=topt;struct _dyneither_ptr _tmp5D3=({const char*_tmpF5="primitive operator has > 2 arguments";_tag_dyneither(_tmpF5,sizeof(char),37U);});Cyc_Tcexp_expr_err(_tmp5D6,_tmp5D5,_tmp5D4,_tmp5D3,_tag_dyneither(_tmpF4,sizeof(void*),0U));});});}_LL0:;}
# 666
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 669
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);
void*_tmpF7=_tmpF6;struct Cyc_List_List*_tmp113;void*_tmp112;struct Cyc_Absyn_Tqual _tmp111;struct Cyc_Absyn_Datatypefield*_tmp110;struct Cyc_List_List*_tmp10F;struct Cyc_Absyn_Aggrdecl*_tmp10E;switch(*((int*)_tmpF7)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF7)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp10E=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF7)->f1).aggr_info).KnownAggr).val;_LL2:
# 673
 if(_tmp10E->impl == 0){
({void*_tmpF8=0U;({unsigned int _tmp5D8=loc;struct _dyneither_ptr _tmp5D7=({const char*_tmpF9="attempt to write an abstract aggregate";_tag_dyneither(_tmpF9,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp5D8,_tmp5D7,_tag_dyneither(_tmpF8,sizeof(void*),0U));});});
return 0;}else{
# 677
_tmp10F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp10E->impl))->fields;goto _LL4;}}else{goto _LLB;}case 12U: _LL3: _tmp10F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF7)->f2;_LL4:
# 679
 for(0;_tmp10F != 0;_tmp10F=_tmp10F->tl){
struct Cyc_Absyn_Aggrfield*_tmpFA=(struct Cyc_Absyn_Aggrfield*)_tmp10F->hd;
if((_tmpFA->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0U;_tmpFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFA->name);({void*_tmpFB[1U]={& _tmpFD};({unsigned int _tmp5DA=loc;struct _dyneither_ptr _tmp5D9=({const char*_tmpFC="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpFC,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp5DA,_tmp5D9,_tag_dyneither(_tmpFB,sizeof(void*),1U));});});});
return 0;}
# 685
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpFA->type))return 0;}
# 687
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF7)->f1).field_info).KnownDatatypefield).tag == 2){_LL5: _tmp110=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF7)->f1).field_info).KnownDatatypefield).val).f2;_LL6:
# 689
{struct Cyc_List_List*_tmpFE=_tmp110->typs;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct _tuple17*_tmpFF=(struct _tuple17*)_tmpFE->hd;struct _tuple17*_tmp100=_tmpFF;struct Cyc_Absyn_Tqual _tmp105;void*_tmp104;_LLE: _tmp105=_tmp100->f1;_tmp104=_tmp100->f2;_LLF:;
if(_tmp105.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0U;({struct _dyneither_ptr _tmp5DB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp110->name));_tmp103.f1=_tmp5DB;});({void*_tmp101[1U]={& _tmp103};({unsigned int _tmp5DD=loc;struct _dyneither_ptr _tmp5DC=({const char*_tmp102="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmp102,sizeof(char),64U);});Cyc_Tcutil_terr(_tmp5DD,_tmp5DC,_tag_dyneither(_tmp101,sizeof(void*),1U));});});});
return 0;}
# 695
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp104))return 0;}}
# 697
return 1;}else{goto _LLB;}case 8U: _LL7: _tmp112=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).elt_type;_tmp111=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).tq;_LL8:
# 699
 if(_tmp111.real_const){
({void*_tmp106=0U;({unsigned int _tmp5DF=loc;struct _dyneither_ptr _tmp5DE=({const char*_tmp107="attempt to over-write a const array";_tag_dyneither(_tmp107,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp5DF,_tmp5DE,_tag_dyneither(_tmp106,sizeof(void*),0U));});});
return 0;}
# 703
return Cyc_Tcexp_check_writable_aggr(loc,_tmp112);case 10U: _LL9: _tmp113=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF7)->f1;_LLA:
# 705
 for(0;_tmp113 != 0;_tmp113=_tmp113->tl){
struct _tuple17*_tmp108=(struct _tuple17*)_tmp113->hd;struct _tuple17*_tmp109=_tmp108;struct Cyc_Absyn_Tqual _tmp10D;void*_tmp10C;_LL11: _tmp10D=_tmp109->f1;_tmp10C=_tmp109->f2;_LL12:;
if(_tmp10D.real_const){
({void*_tmp10A=0U;({unsigned int _tmp5E1=loc;struct _dyneither_ptr _tmp5E0=({const char*_tmp10B="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmp10B,sizeof(char),56U);});Cyc_Tcutil_terr(_tmp5E1,_tmp5E0,_tag_dyneither(_tmp10A,sizeof(void*),0U));});});
return 0;}
# 711
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp10C))return 0;}
# 713
return 1;default: _LLB: _LLC:
 return 1;}_LL0:;}
# 721
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 724
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp114=e->r;void*_tmp115=_tmp114;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct _dyneither_ptr*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct _dyneither_ptr*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Vardecl*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13A;struct Cyc_Absyn_Vardecl*_tmp139;struct Cyc_Absyn_Vardecl*_tmp138;switch(*((int*)_tmp115)){case 1U: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)){case 3U: _LL1: _tmp138=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)->f1;_LL2:
 _tmp139=_tmp138;goto _LL4;case 4U: _LL3: _tmp139=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)->f1;_LL4:
 _tmp13A=_tmp139;goto _LL6;case 5U: _LL5: _tmp13A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)->f1;_LL6:
 _tmp13B=_tmp13A;goto _LL8;case 1U: _LL7: _tmp13B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)->f1;_LL8:
 if(!(_tmp13B->tq).real_const)return;goto _LL0;default: goto _LL15;}case 23U: _LL9: _tmp13D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_tmp13C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp115)->f2;_LLA:
# 731
{void*_tmp116=Cyc_Tcutil_compress((void*)_check_null(_tmp13D->topt));void*_tmp117=_tmp116;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_Tqual _tmp127;struct Cyc_Absyn_Tqual _tmp126;switch(*((int*)_tmp117)){case 5U: _LL18: _tmp126=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp117)->f1).elt_tq;_LL19:
 _tmp127=_tmp126;goto _LL1B;case 8U: _LL1A: _tmp127=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp117)->f1).tq;_LL1B:
 if(!_tmp127.real_const)return;goto _LL17;case 10U: _LL1C: _tmp128=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp117)->f1;_LL1D: {
# 735
struct _tuple15 _tmp118=Cyc_Evexp_eval_const_uint_exp(_tmp13C);struct _tuple15 _tmp119=_tmp118;unsigned int _tmp125;int _tmp124;_LL21: _tmp125=_tmp119.f1;_tmp124=_tmp119.f2;_LL22:;
if(!_tmp124){
({void*_tmp11A=0U;({unsigned int _tmp5E3=e->loc;struct _dyneither_ptr _tmp5E2=({const char*_tmp11B="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp11B,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp5E3,_tmp5E2,_tag_dyneither(_tmp11A,sizeof(void*),0U));});});
return;}
# 740
{struct _handler_cons _tmp11C;_push_handler(& _tmp11C);{int _tmp11E=0;if(setjmp(_tmp11C.handler))_tmp11E=1;if(!_tmp11E){
{struct _tuple17*_tmp11F=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp128,(int)_tmp125);struct _tuple17*_tmp120=_tmp11F;struct Cyc_Absyn_Tqual _tmp121;_LL24: _tmp121=_tmp120->f1;_LL25:;
if(!_tmp121.real_const){_npop_handler(0U);return;}}
# 741
;_pop_handler();}else{void*_tmp11D=(void*)_exn_thrown;void*_tmp122=_tmp11D;void*_tmp123;if(((struct Cyc_List_Nth_exn_struct*)_tmp122)->tag == Cyc_List_Nth){_LL27: _LL28:
# 743
 return;}else{_LL29: _tmp123=_tmp122;_LL2A:(int)_rethrow(_tmp123);}_LL26:;}};}
goto _LL17;}default: _LL1E: _LL1F:
 goto _LL17;}_LL17:;}
# 747
goto _LL0;case 21U: _LLB: _tmp13F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_tmp13E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp115)->f2;_LLC:
# 749
{void*_tmp129=Cyc_Tcutil_compress((void*)_check_null(_tmp13F->topt));void*_tmp12A=_tmp129;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_Aggrdecl**_tmp12B;switch(*((int*)_tmp12A)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12A)->f1).aggr_info).KnownAggr).tag == 2){_LL2C: _tmp12B=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12A)->f1).aggr_info).KnownAggr).val;_LL2D: {
# 751
struct Cyc_Absyn_Aggrfield*sf=
_tmp12B == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp12B,_tmp13E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}}else{goto _LL30;}case 12U: _LL2E: _tmp12C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12A)->f2;_LL2F: {
# 756
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp12C,_tmp13E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL2B;}default: _LL30: _LL31:
 goto _LL2B;}_LL2B:;}
# 761
goto _LL0;case 22U: _LLD: _tmp141=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_tmp140=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp115)->f2;_LLE:
# 763
{void*_tmp12D=Cyc_Tcutil_compress((void*)_check_null(_tmp141->topt));void*_tmp12E=_tmp12D;void*_tmp133;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E)->tag == 5U){_LL33: _tmp133=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E)->f1).elt_typ;_LL34:
# 765
{void*_tmp12F=Cyc_Tcutil_compress(_tmp133);void*_tmp130=_tmp12F;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_Aggrdecl**_tmp131;switch(*((int*)_tmp130)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp130)->f1).aggr_info).KnownAggr).tag == 2){_LL38: _tmp131=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp130)->f1).aggr_info).KnownAggr).val;_LL39: {
# 767
struct Cyc_Absyn_Aggrfield*sf=
_tmp131 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp131,_tmp140);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}}else{goto _LL3C;}case 12U: _LL3A: _tmp132=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp130)->f2;_LL3B: {
# 772
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp132,_tmp140);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL37;}default: _LL3C: _LL3D:
 goto _LL37;}_LL37:;}
# 777
goto _LL32;}else{_LL35: _LL36:
 goto _LL32;}_LL32:;}
# 780
goto _LL0;case 20U: _LLF: _tmp142=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_LL10:
# 782
{void*_tmp134=Cyc_Tcutil_compress((void*)_check_null(_tmp142->topt));void*_tmp135=_tmp134;struct Cyc_Absyn_Tqual _tmp137;struct Cyc_Absyn_Tqual _tmp136;switch(*((int*)_tmp135)){case 5U: _LL3F: _tmp136=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135)->f1).elt_tq;_LL40:
 _tmp137=_tmp136;goto _LL42;case 8U: _LL41: _tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).tq;_LL42:
 if(!_tmp137.real_const)return;goto _LL3E;default: _LL43: _LL44:
 goto _LL3E;}_LL3E:;}
# 787
goto _LL0;case 12U: _LL11: _tmp143=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_LL12:
 _tmp144=_tmp143;goto _LL14;case 13U: _LL13: _tmp144=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_LL14:
 Cyc_Tcexp_check_writable(te,_tmp144);return;default: _LL15: _LL16:
 goto _LL0;}_LL0:;}
# 792
({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0U;({struct _dyneither_ptr _tmp5E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp147.f1=_tmp5E4;});({void*_tmp145[1U]={& _tmp147};({unsigned int _tmp5E6=e->loc;struct _dyneither_ptr _tmp5E5=({const char*_tmp146="attempt to write a const location: %s";_tag_dyneither(_tmp146,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp5E6,_tmp5E5,_tag_dyneither(_tmp145,sizeof(void*),1U));});});});}
# 795
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 798
({struct Cyc_Tcenv_Tenv*_tmp5E7=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp5E7,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp148=0U;({struct Cyc_Tcenv_Tenv*_tmp5EB=te;unsigned int _tmp5EA=loc;void**_tmp5E9=topt;struct _dyneither_ptr _tmp5E8=({const char*_tmp149="increment/decrement of non-lvalue";_tag_dyneither(_tmp149,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp5EB,_tmp5EA,_tmp5E9,_tmp5E8,_tag_dyneither(_tmp148,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 804
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp14A=0U;({unsigned int _tmp5ED=e->loc;struct _dyneither_ptr _tmp5EC=({const char*_tmp14B="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp14B,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp5ED,_tmp5EC,_tag_dyneither(_tmp14A,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp14C=0U;({unsigned int _tmp5EF=e->loc;struct _dyneither_ptr _tmp5EE=({const char*_tmp14D="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp14D,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp5EF,_tmp5EE,_tag_dyneither(_tmp14C,sizeof(void*),0U));});});}else{
# 813
({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0U;({struct _dyneither_ptr _tmp5F0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp150.f1=_tmp5F0;});({void*_tmp14E[1U]={& _tmp150};({unsigned int _tmp5F2=e->loc;struct _dyneither_ptr _tmp5F1=({const char*_tmp14F="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp14F,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp5F2,_tmp5F1,_tag_dyneither(_tmp14E,sizeof(void*),1U));});});});}}
# 815
return t;};}
# 819
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 821
({struct Cyc_Tcenv_Tenv*_tmp5F4=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp5F3=e1;Cyc_Tcexp_tcTest(_tmp5F4,_tmp5F3,({const char*_tmp151="conditional expression";_tag_dyneither(_tmp151,sizeof(char),23U);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp5F5=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp5F5;});else{
# 828
({void*_tmp5F6=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp5F6;});}{
struct Cyc_List_List _tmp152=({struct Cyc_List_List _tmp159;_tmp159.hd=e3;_tmp159.tl=0;_tmp159;});
struct Cyc_List_List _tmp153=({struct Cyc_List_List _tmp158;_tmp158.hd=e2;_tmp158.tl=& _tmp152;_tmp158;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp153)){
({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0U;({struct _dyneither_ptr _tmp5F7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp157.f1=_tmp5F7;});({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U;({struct _dyneither_ptr _tmp5F8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp156.f1=_tmp5F8;});({void*_tmp154[2U]={& _tmp156,& _tmp157};({unsigned int _tmp5FA=loc;struct _dyneither_ptr _tmp5F9=({const char*_tmp155="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp155,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp5FA,_tmp5F9,_tag_dyneither(_tmp154,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}
# 836
return t;};};}
# 840
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 842
({struct Cyc_Tcenv_Tenv*_tmp5FC=te;struct Cyc_Absyn_Exp*_tmp5FB=e1;Cyc_Tcexp_tcTest(_tmp5FC,_tmp5FB,({const char*_tmp15A="logical-and expression";_tag_dyneither(_tmp15A,sizeof(char),23U);}));});
({struct Cyc_Tcenv_Tenv*_tmp5FE=te;struct Cyc_Absyn_Exp*_tmp5FD=e2;Cyc_Tcexp_tcTest(_tmp5FE,_tmp5FD,({const char*_tmp15B="logical-and expression";_tag_dyneither(_tmp15B,sizeof(char),23U);}));});
return Cyc_Absyn_sint_typ;}
# 848
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 850
({struct Cyc_Tcenv_Tenv*_tmp600=te;struct Cyc_Absyn_Exp*_tmp5FF=e1;Cyc_Tcexp_tcTest(_tmp600,_tmp5FF,({const char*_tmp15C="logical-or expression";_tag_dyneither(_tmp15C,sizeof(char),22U);}));});
({struct Cyc_Tcenv_Tenv*_tmp602=te;struct Cyc_Absyn_Exp*_tmp601=e2;Cyc_Tcexp_tcTest(_tmp602,_tmp601,({const char*_tmp15D="logical-or expression";_tag_dyneither(_tmp15D,sizeof(char),22U);}));});
return Cyc_Absyn_sint_typ;}
# 856
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 863
({struct Cyc_Tcenv_Tenv*_tmp603=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp603,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 868
{void*_tmp15E=Cyc_Tcutil_compress(t1);void*_tmp15F=_tmp15E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15F)->tag == 8U){_LL1: _LL2:
({void*_tmp160=0U;({unsigned int _tmp605=loc;struct _dyneither_ptr _tmp604=({const char*_tmp161="cannot assign to an array";_tag_dyneither(_tmp161,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp605,_tmp604,_tag_dyneither(_tmp160,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 873
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp162=0U;({unsigned int _tmp607=loc;struct _dyneither_ptr _tmp606=({const char*_tmp163="type is abstract (can't determine size).";_tag_dyneither(_tmp163,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp607,_tmp606,_tag_dyneither(_tmp162,sizeof(void*),0U));});});
# 877
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp164=0U;({struct Cyc_Tcenv_Tenv*_tmp60B=te;unsigned int _tmp60A=loc;void**_tmp609=topt;struct _dyneither_ptr _tmp608=({const char*_tmp165="assignment to non-lvalue";_tag_dyneither(_tmp165,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp60B,_tmp60A,_tmp609,_tmp608,_tag_dyneither(_tmp164,sizeof(void*),0U));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp166=0U;({unsigned int _tmp60D=e2->loc;struct _dyneither_ptr _tmp60C=({const char*_tmp167="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp167,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp60D,_tmp60C,_tag_dyneither(_tmp166,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0U;({struct _dyneither_ptr _tmp60E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp16C.f1=_tmp60E;});({struct Cyc_String_pa_PrintArg_struct _tmp16B;_tmp16B.tag=0U;({struct _dyneither_ptr _tmp60F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp16B.f1=_tmp60F;});({void*_tmp169[2U]={& _tmp16B,& _tmp16C};({struct Cyc_Tcenv_Tenv*_tmp613=te;unsigned int _tmp612=loc;void**_tmp611=topt;struct _dyneither_ptr _tmp610=({const char*_tmp16A="type mismatch: %s != %s";_tag_dyneither(_tmp16A,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp613,_tmp612,_tmp611,_tmp610,_tag_dyneither(_tmp169,sizeof(void*),2U));});});});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp168;}}else{
# 891
enum Cyc_Absyn_Primop _tmp16D=(enum Cyc_Absyn_Primop)po->v;
void*_tmp16E=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp16D,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp16E,t1) || Cyc_Tcutil_coerceable(_tmp16E) && Cyc_Tcutil_coerceable(t1))){
void*_tmp16F=({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0U;({struct _dyneither_ptr _tmp614=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 897
Cyc_Absynpp_typ2string(t2));_tmp173.f1=_tmp614;});({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0U;({struct _dyneither_ptr _tmp615=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 896
Cyc_Absynpp_typ2string(t1));_tmp172.f1=_tmp615;});({void*_tmp170[2U]={& _tmp172,& _tmp173};({struct Cyc_Tcenv_Tenv*_tmp619=te;unsigned int _tmp618=loc;void**_tmp617=topt;struct _dyneither_ptr _tmp616=({const char*_tmp171="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp171,sizeof(char),82U);});Cyc_Tcexp_expr_err(_tmp619,_tmp618,_tmp617,_tmp616,_tag_dyneither(_tmp170,sizeof(void*),2U));});});});});
# 898
Cyc_Tcutil_unify(_tmp16E,t1);
Cyc_Tcutil_explain_failure();
return _tmp16F;}
# 902
return _tmp16E;}
# 904
return t1;};};}
# 908
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp61A=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp61A,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp61C=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp61B=topt;Cyc_Tcexp_tcExp(_tmp61C,_tmp61B,e2);});
return(void*)_check_null(e2->topt);}
# 915
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 918
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 921
{void*_tmp174=Cyc_Tcutil_compress(t1);void*_tmp175=_tmp174;switch(*((int*)_tmp175)){case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp175)->f1 == 0){_LL1: _LL2:
({struct Cyc_Tcenv_Tenv*_tmp61E=te;struct Cyc_Absyn_Exp*_tmp61D=e;Cyc_Tcutil_unchecked_cast(_tmp61E,_tmp61D,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);});({void*_tmp61F=Cyc_Absyn_float_typ(1);t1=_tmp61F;});goto _LL0;}else{goto _LL7;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp175)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 goto _LL6;case Cyc_Absyn_Short_sz: _LL5: _LL6:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL0;default: goto _LL7;}default: _LL7: _LL8:
 goto _LL0;}_LL0:;}
# 928
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp176=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp177=_tmp176;struct _tuple1*_tmp17B;struct Cyc_List_List*_tmp17A;unsigned int _tmp179;enum Cyc_Absyn_Scope _tmp178;_LLA: _tmp17B=_tmp177.name;_tmp17A=_tmp177.typs;_tmp179=_tmp177.loc;_tmp178=_tmp177.sc;_LLB:;
# 931
if(_tmp17A == 0  || _tmp17A->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp17A->hd)).f2);
# 934
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 939
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp17C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp17D=_tmp17C;struct _tuple1*_tmp181;struct Cyc_List_List*_tmp180;unsigned int _tmp17F;enum Cyc_Absyn_Scope _tmp17E;_LLD: _tmp181=_tmp17D.name;_tmp180=_tmp17D.typs;_tmp17F=_tmp17D.loc;_tmp17E=_tmp17D.sc;_LLE:;
# 942
if(_tmp180 == 0  || _tmp180->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp180->hd)).f2);
# 946
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 951
({struct Cyc_String_pa_PrintArg_struct _tmp185;_tmp185.tag=0U;({struct _dyneither_ptr _tmp620=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));_tmp185.f1=_tmp620;});({struct Cyc_String_pa_PrintArg_struct _tmp184;_tmp184.tag=0U;({struct _dyneither_ptr _tmp621=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp184.f1=_tmp621;});({void*_tmp182[2U]={& _tmp184,& _tmp185};({unsigned int _tmp623=e->loc;struct _dyneither_ptr _tmp622=({const char*_tmp183="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp183,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp623,_tmp622,_tag_dyneither(_tmp182,sizeof(void*),2U));});});});});
return 0;}
# 957
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 963
struct Cyc_List_List*_tmp186=args;
int _tmp187=0;
struct Cyc_Tcenv_Tenv*_tmp188=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp189=Cyc_Tcenv_clear_abstract_val_ok(_tmp188);
Cyc_Tcexp_tcExp(_tmp189,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 972
void*_tmp18A=t;void*_tmp1ED;struct Cyc_Absyn_Tqual _tmp1EC;void*_tmp1EB;union Cyc_Absyn_Constraint*_tmp1EA;union Cyc_Absyn_Constraint*_tmp1E9;union Cyc_Absyn_Constraint*_tmp1E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->tag == 5U){_LL1: _tmp1ED=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).elt_typ;_tmp1EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).elt_tq;_tmp1EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).ptr_atts).rgn;_tmp1EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).ptr_atts).nullable;_tmp1E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).ptr_atts).bounds;_tmp1E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).ptr_atts).zero_term;_LL2:
# 977
 Cyc_Tcenv_check_rgn_accessible(_tmp189,loc,_tmp1EB);
# 979
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1E9);{
void*_tmp18B=Cyc_Tcutil_compress(_tmp1ED);void*_tmp18C=_tmp18B;struct Cyc_List_List*_tmp1E4;void*_tmp1E3;struct Cyc_Absyn_Tqual _tmp1E2;void*_tmp1E1;struct Cyc_List_List*_tmp1E0;int _tmp1DF;struct Cyc_Absyn_VarargInfo*_tmp1DE;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1D9;struct Cyc_List_List*_tmp1D8;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->tag == 9U){_LL6: _tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).tvars;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).effect;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ret_tqual;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ret_typ;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).args;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).c_varargs;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).cyc_varargs;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).rgn_po;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).attributes;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).requires_clause;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).requires_relns;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ensures_clause;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ensures_relns;_LL7:
# 984
 if(_tmp1E4 != 0  || _tmp1DD != 0)
({void*_tmp18D=0U;({unsigned int _tmp625=e->loc;struct _dyneither_ptr _tmp624=({const char*_tmp18E="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp18E,sizeof(char),79U);});Cyc_Tcutil_terr(_tmp625,_tmp624,_tag_dyneither(_tmp18D,sizeof(void*),0U));});});
# 988
if(topt != 0)Cyc_Tcutil_unify(_tmp1E1,*topt);
# 990
while(_tmp186 != 0  && _tmp1E0 != 0){
# 992
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp186->hd;
void*t2=(*((struct _tuple9*)_tmp1E0->hd)).f3;
Cyc_Tcexp_tcExp(_tmp189,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp189,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp19E="actual argument has type ";_tag_dyneither(_tmp19E,sizeof(char),26U);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp19D=" but formal has type ";_tag_dyneither(_tmp19D,sizeof(char),22U);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp628=({unsigned long _tmp627=({unsigned long _tmp626=Cyc_strlen((struct _dyneither_ptr)s0);_tmp626 + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp627 + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp628 + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70)
({void*_tmp18F=0U;({unsigned int _tmp62B=e1->loc;struct _dyneither_ptr _tmp62A=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp195;_tmp195.tag=0U;_tmp195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp194;_tmp194.tag=0U;_tmp194.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp193;_tmp193.tag=0U;_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp192;_tmp192.tag=0U;_tmp192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp190[4U]={& _tmp192,& _tmp193,& _tmp194,& _tmp195};({struct _dyneither_ptr _tmp629=({const char*_tmp191="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp191,sizeof(char),15U);});Cyc_aprintf(_tmp629,_tag_dyneither(_tmp190,sizeof(void*),4U));});});});});});});Cyc_Tcutil_terr(_tmp62B,_tmp62A,_tag_dyneither(_tmp18F,sizeof(void*),0U));});});else{
# 1004
({void*_tmp196=0U;({unsigned int _tmp62E=e1->loc;struct _dyneither_ptr _tmp62D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19C;_tmp19C.tag=0U;_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp19B;_tmp19B.tag=0U;_tmp19B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp19A;_tmp19A.tag=0U;_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0U;_tmp199.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp197[4U]={& _tmp199,& _tmp19A,& _tmp19B,& _tmp19C};({struct _dyneither_ptr _tmp62C=({const char*_tmp198="%s%s%s%s.";_tag_dyneither(_tmp198,sizeof(char),10U);});Cyc_aprintf(_tmp62C,_tag_dyneither(_tmp197,sizeof(void*),4U));});});});});});});Cyc_Tcutil_terr(_tmp62E,_tmp62D,_tag_dyneither(_tmp196,sizeof(void*),0U));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1009
if(alias_coercion)
({struct Cyc_List_List*_tmp62F=({struct Cyc_List_List*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=(void*)_tmp187;_tmp19F->tl=*alias_arg_exps;_tmp19F;});*alias_arg_exps=_tmp62F;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1A0=0U;({unsigned int _tmp631=e1->loc;struct _dyneither_ptr _tmp630=({const char*_tmp1A1="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1A1,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp631,_tmp630,_tag_dyneither(_tmp1A0,sizeof(void*),0U));});});
_tmp186=_tmp186->tl;
_tmp1E0=_tmp1E0->tl;
++ _tmp187;}{
# 1020
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1DC;for(0;a != 0;a=a->tl){
void*_tmp1A2=(void*)a->hd;void*_tmp1A3=_tmp1A2;enum Cyc_Absyn_Format_Type _tmp1B0;int _tmp1AF;int _tmp1AE;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->tag == 19U){_LLB: _tmp1B0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f1;_tmp1AF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f2;_tmp1AE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f3;_LLC:
# 1024
{struct _handler_cons _tmp1A4;_push_handler(& _tmp1A4);{int _tmp1A6=0;if(setjmp(_tmp1A4.handler))_tmp1A6=1;if(!_tmp1A6){
# 1026
{struct Cyc_Absyn_Exp*_tmp1A7=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1AF - 1);
# 1028
struct Cyc_Core_Opt*fmt_args;
if(_tmp1AE == 0)
fmt_args=0;else{
# 1032
({struct Cyc_Core_Opt*_tmp633=({struct Cyc_Core_Opt*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));({struct Cyc_List_List*_tmp632=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1AE - 1);_tmp1A8->v=_tmp632;});_tmp1A8;});fmt_args=_tmp633;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1A9=_tmp1B0;if(_tmp1A9 == Cyc_Absyn_Printf_ft){_LL10: _LL11:
# 1036
 Cyc_Tcexp_check_format_args(_tmp189,_tmp1A7,fmt_args,_tmp1AE - 1,alias_arg_exps,Cyc_Formatstr_get_format_typs);
# 1039
goto _LLF;}else{_LL12: _LL13:
# 1041
 Cyc_Tcexp_check_format_args(_tmp189,_tmp1A7,fmt_args,_tmp1AE - 1,alias_arg_exps,Cyc_Formatstr_get_scanf_typs);
# 1044
goto _LLF;}_LLF:;};}
# 1026
;_pop_handler();}else{void*_tmp1A5=(void*)_exn_thrown;void*_tmp1AA=_tmp1A5;void*_tmp1AD;if(((struct Cyc_List_Nth_exn_struct*)_tmp1AA)->tag == Cyc_List_Nth){_LL15: _LL16:
# 1046
({void*_tmp1AB=0U;({unsigned int _tmp635=loc;struct _dyneither_ptr _tmp634=({const char*_tmp1AC="bad format arguments";_tag_dyneither(_tmp1AC,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp635,_tmp634,_tag_dyneither(_tmp1AB,sizeof(void*),0U));});});goto _LL14;}else{_LL17: _tmp1AD=_tmp1AA;_LL18:(int)_rethrow(_tmp1AD);}_LL14:;}};}
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}}
# 1051
if(_tmp1E0 != 0)({void*_tmp1B1=0U;({unsigned int _tmp637=loc;struct _dyneither_ptr _tmp636=({const char*_tmp1B2="too few arguments for function";_tag_dyneither(_tmp1B2,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp637,_tmp636,_tag_dyneither(_tmp1B1,sizeof(void*),0U));});});else{
# 1053
if((_tmp186 != 0  || _tmp1DF) || _tmp1DE != 0){
if(_tmp1DF)
for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
Cyc_Tcexp_tcExp(_tmp189,0,(struct Cyc_Absyn_Exp*)_tmp186->hd);}else{
if(_tmp1DE == 0)
({void*_tmp1B3=0U;({unsigned int _tmp639=loc;struct _dyneither_ptr _tmp638=({const char*_tmp1B4="too many arguments for function";_tag_dyneither(_tmp1B4,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp639,_tmp638,_tag_dyneither(_tmp1B3,sizeof(void*),0U));});});else{
# 1060
struct Cyc_Absyn_VarargInfo _tmp1B5=*_tmp1DE;struct Cyc_Absyn_VarargInfo _tmp1B6=_tmp1B5;void*_tmp1D4;int _tmp1D3;_LL1A: _tmp1D4=_tmp1B6.type;_tmp1D3=_tmp1B6.inject;_LL1B:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1B7=({struct Cyc_Absyn_VarargCallInfo*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->num_varargs=0;_tmp1D2->injectors=0;_tmp1D2->vai=_tmp1DE;_tmp1D2;});
# 1064
*vararg_call_info=_tmp1B7;
# 1066
if(!_tmp1D3)
# 1068
for(0;_tmp186 != 0;(_tmp186=_tmp186->tl,_tmp187 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp186->hd;
++ _tmp1B7->num_varargs;
Cyc_Tcexp_tcExp(_tmp189,& _tmp1D4,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp189,e1,_tmp1D4,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=0U;({struct _dyneither_ptr _tmp63A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp1BB.f1=_tmp63A;});({struct Cyc_String_pa_PrintArg_struct _tmp1BA;_tmp1BA.tag=0U;({struct _dyneither_ptr _tmp63B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D4));_tmp1BA.f1=_tmp63B;});({void*_tmp1B8[2U]={& _tmp1BA,& _tmp1BB};({unsigned int _tmp63D=loc;struct _dyneither_ptr _tmp63C=({const char*_tmp1B9="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1B9,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp63D,_tmp63C,_tag_dyneither(_tmp1B8,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}
# 1078
if(alias_coercion)
({struct Cyc_List_List*_tmp63E=({struct Cyc_List_List*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->hd=(void*)_tmp187;_tmp1BC->tl=*alias_arg_exps;_tmp1BC;});*alias_arg_exps=_tmp63E;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1D4) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1BD=0U;({unsigned int _tmp640=e1->loc;struct _dyneither_ptr _tmp63F=({const char*_tmp1BE="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1BE,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp640,_tmp63F,_tag_dyneither(_tmp1BD,sizeof(void*),0U));});});}else{
# 1087
void*_tmp1BF=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1D4));void*_tmp1C0=_tmp1BF;struct Cyc_Absyn_Datatypedecl*_tmp1D1;struct Cyc_List_List*_tmp1D0;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C0)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C0)->f1).datatype_info).KnownDatatype).tag == 2){_LL1D: _tmp1D1=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C0)->f1).datatype_info).KnownDatatype).val;_tmp1D0=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C0)->f1).targs;_LL1E: {
# 1091
struct Cyc_Absyn_Datatypedecl*_tmp1C1=*Cyc_Tcenv_lookup_datatypedecl(_tmp189,loc,_tmp1D1->name);
struct Cyc_List_List*fields=0;
if(_tmp1C1->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1C4;_tmp1C4.tag=0U;({struct _dyneither_ptr _tmp641=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1D4));_tmp1C4.f1=_tmp641;});({void*_tmp1C2[1U]={& _tmp1C4};({unsigned int _tmp643=loc;struct _dyneither_ptr _tmp642=({const char*_tmp1C3="can't inject into abstract datatype %s";_tag_dyneither(_tmp1C3,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp643,_tmp642,_tag_dyneither(_tmp1C2,sizeof(void*),1U));});});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C1->fields))->v;}
# 1102
if(!({void*_tmp644=Cyc_Tcutil_pointer_region(_tmp1D4);Cyc_Tcutil_unify(_tmp644,Cyc_Tcenv_curr_rgn(_tmp189));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1C8;_tmp1C8.tag=0U;({struct _dyneither_ptr _tmp645=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp189)));_tmp1C8.f1=_tmp645;});({struct Cyc_String_pa_PrintArg_struct _tmp1C7;_tmp1C7.tag=0U;({struct _dyneither_ptr _tmp646=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D4));_tmp1C7.f1=_tmp646;});({void*_tmp1C5[2U]={& _tmp1C7,& _tmp1C8};({unsigned int _tmp648=loc;struct _dyneither_ptr _tmp647=({const char*_tmp1C6="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1C6,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp648,_tmp647,_tag_dyneither(_tmp1C5,sizeof(void*),2U));});});});});{
# 1106
struct Cyc_List_List*_tmp1C9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1C1->tvs,_tmp1D0);
for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
++ _tmp1B7->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp186->hd;
# 1111
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp189,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1CA=0U;({unsigned int _tmp64A=e1->loc;struct _dyneither_ptr _tmp649=({const char*_tmp1CB="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1CB,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp64A,_tmp649,_tag_dyneither(_tmp1CA,sizeof(void*),0U));});});}{
# 1117
struct Cyc_Absyn_Datatypefield*_tmp1CC=({struct Cyc_Tcenv_Tenv*_tmp64E=_tmp189;struct Cyc_Absyn_Exp*_tmp64D=e1;void*_tmp64C=Cyc_Tcutil_pointer_elt_type(_tmp1D4);struct Cyc_List_List*_tmp64B=_tmp1C9;Cyc_Tcexp_tcInjection(_tmp64E,_tmp64D,_tmp64C,_tmp64B,fields);});
if(_tmp1CC != 0)
({struct Cyc_List_List*_tmp650=({
struct Cyc_List_List*_tmp64F=_tmp1B7->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp64F,({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=_tmp1CC;_tmp1CD->tl=0;_tmp1CD;}));});
# 1119
_tmp1B7->injectors=_tmp650;});};};}
# 1123
goto _LL1C;};}}else{goto _LL1F;}}else{_LL1F: _LL20:
({void*_tmp1CE=0U;({unsigned int _tmp652=loc;struct _dyneither_ptr _tmp651=({const char*_tmp1CF="bad inject vararg type";_tag_dyneither(_tmp1CF,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp652,_tmp651,_tag_dyneither(_tmp1CE,sizeof(void*),0U));});});goto _LL1C;}_LL1C:;}};}}}}
# 1129
if(*alias_arg_exps == 0)
# 1138 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp189,loc,(void*)_check_null(_tmp1E3));
# 1140
return _tmp1E1;};}else{_LL8: _LL9:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1D7;_tmp1D7.tag=0U;({struct _dyneither_ptr _tmp653=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1D7.f1=_tmp653;});({void*_tmp1D5[1U]={& _tmp1D7};({struct Cyc_Tcenv_Tenv*_tmp657=_tmp189;unsigned int _tmp656=loc;void**_tmp655=topt;struct _dyneither_ptr _tmp654=({const char*_tmp1D6="expected pointer to function but found %s";_tag_dyneither(_tmp1D6,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp657,_tmp656,_tmp655,_tmp654,_tag_dyneither(_tmp1D5,sizeof(void*),1U));});});});}_LL5:;};}else{_LL3: _LL4:
# 1143
 return({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0U;({struct _dyneither_ptr _tmp658=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1E7.f1=_tmp658;});({void*_tmp1E5[1U]={& _tmp1E7};({struct Cyc_Tcenv_Tenv*_tmp65C=_tmp189;unsigned int _tmp65B=loc;void**_tmp65A=topt;struct _dyneither_ptr _tmp659=({const char*_tmp1E6="expected pointer to function but found %s";_tag_dyneither(_tmp1E6,sizeof(char),42U);});Cyc_Tcexp_expr_err(_tmp65C,_tmp65B,_tmp65A,_tmp659,_tag_dyneither(_tmp1E5,sizeof(void*),1U));});});});}_LL0:;};}
# 1147
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1EE=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1EE,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1EE,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0U;({struct _dyneither_ptr _tmp65D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp1F2.f1=_tmp65D;});({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0U;({struct _dyneither_ptr _tmp65E=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1152
Cyc_Absynpp_typ2string(_tmp1EE));_tmp1F1.f1=_tmp65E;});({void*_tmp1EF[2U]={& _tmp1F1,& _tmp1F2};({unsigned int _tmp660=loc;struct _dyneither_ptr _tmp65F=({const char*_tmp1F0="expected %s but found %s";_tag_dyneither(_tmp1F0,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp660,_tmp65F,_tag_dyneither(_tmp1EF,sizeof(void*),2U));});});});});
# 1154
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1159
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1161
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1163
({void*_tmp661=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp661;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1F3=t1;void*_tmp212;struct Cyc_Absyn_Tqual _tmp211;void*_tmp210;union Cyc_Absyn_Constraint*_tmp20F;union Cyc_Absyn_Constraint*_tmp20E;union Cyc_Absyn_Constraint*_tmp20D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->tag == 5U){_LL1: _tmp212=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).elt_typ;_tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).elt_tq;_tmp210=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).ptr_atts).rgn;_tmp20F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).ptr_atts).nullable;_tmp20E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).ptr_atts).bounds;_tmp20D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).ptr_atts).zero_term;_LL2:
# 1167
{void*_tmp1F4=Cyc_Tcutil_compress(_tmp212);void*_tmp1F5=_tmp1F4;struct Cyc_List_List*_tmp20C;void*_tmp20B;struct Cyc_Absyn_Tqual _tmp20A;void*_tmp209;struct Cyc_List_List*_tmp208;int _tmp207;struct Cyc_Absyn_VarargInfo*_tmp206;struct Cyc_List_List*_tmp205;struct Cyc_List_List*_tmp204;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_List_List*_tmp202;struct Cyc_Absyn_Exp*_tmp201;struct Cyc_List_List*_tmp200;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->tag == 9U){_LL6: _tmp20C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).tvars;_tmp20B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).effect;_tmp20A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).ret_tqual;_tmp209=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).ret_typ;_tmp208=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).args;_tmp207=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).c_varargs;_tmp206=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).cyc_varargs;_tmp205=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).rgn_po;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).attributes;_tmp203=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).requires_clause;_tmp202=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).requires_relns;_tmp201=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).ensures_clause;_tmp200=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).ensures_relns;_LL7: {
# 1169
struct Cyc_List_List*instantiation=0;
# 1171
for(0;ts != 0  && _tmp20C != 0;(ts=ts->tl,_tmp20C=_tmp20C->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp20C->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp665=loc;struct Cyc_Tcenv_Tenv*_tmp664=te;struct Cyc_List_List*_tmp663=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp662=k;Cyc_Tcutil_check_type(_tmp665,_tmp664,_tmp663,_tmp662,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp667=({struct Cyc_List_List*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));({struct _tuple13*_tmp666=({struct _tuple13*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->f1=(struct Cyc_Absyn_Tvar*)_tmp20C->hd;_tmp1F7->f2=(void*)ts->hd;_tmp1F7;});_tmp1F6->hd=_tmp666;});_tmp1F6->tl=instantiation;_tmp1F6;});instantiation=_tmp667;});}
# 1177
if(ts != 0)
return({void*_tmp1F8=0U;({struct Cyc_Tcenv_Tenv*_tmp66B=te;unsigned int _tmp66A=loc;void**_tmp669=topt;struct _dyneither_ptr _tmp668=({const char*_tmp1F9="too many type variables in instantiation";_tag_dyneither(_tmp1F9,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp66B,_tmp66A,_tmp669,_tmp668,_tag_dyneither(_tmp1F8,sizeof(void*),0U));});});
# 1182
if(_tmp20C == 0){
({struct Cyc_List_List*_tmp66C=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp205);_tmp205=_tmp66C;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp205);
_tmp205=0;}{
# 1187
void*new_fn_typ=({
struct Cyc_List_List*_tmp66F=instantiation;Cyc_Tcutil_substitute(_tmp66F,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp66E=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1FE;_tmp1FE.tag=9U;({struct Cyc_Absyn_FnInfo _tmp66D=({struct Cyc_Absyn_FnInfo _tmp1FF;_tmp1FF.tvars=_tmp20C;_tmp1FF.effect=_tmp20B;_tmp1FF.ret_tqual=_tmp20A;_tmp1FF.ret_typ=_tmp209;_tmp1FF.args=_tmp208;_tmp1FF.c_varargs=_tmp207;_tmp1FF.cyc_varargs=_tmp206;_tmp1FF.rgn_po=_tmp205;_tmp1FF.attributes=_tmp204;_tmp1FF.requires_clause=_tmp203;_tmp1FF.requires_relns=_tmp202;_tmp1FF.ensures_clause=_tmp201;_tmp1FF.ensures_relns=_tmp200;_tmp1FF;});_tmp1FE.f1=_tmp66D;});_tmp1FE;});_tmp1FD[0]=_tmp66E;});_tmp1FD;}));});
# 1192
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp672=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1FB;_tmp1FB.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp671=({struct Cyc_Absyn_PtrInfo _tmp1FC;_tmp1FC.elt_typ=new_fn_typ;_tmp1FC.elt_tq=_tmp211;({struct Cyc_Absyn_PtrAtts _tmp670=({(_tmp1FC.ptr_atts).rgn=_tmp210;(_tmp1FC.ptr_atts).nullable=_tmp20F;(_tmp1FC.ptr_atts).bounds=_tmp20E;(_tmp1FC.ptr_atts).zero_term=_tmp20D;(_tmp1FC.ptr_atts).ptrloc=0;_tmp1FC.ptr_atts;});_tmp1FC.ptr_atts=_tmp670;});_tmp1FC;});_tmp1FB.f1=_tmp671;});_tmp1FB;});_tmp1FA[0]=_tmp672;});_tmp1FA;});};}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1195
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1198
return({struct Cyc_String_pa_PrintArg_struct _tmp215;_tmp215.tag=0U;({struct _dyneither_ptr _tmp673=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));_tmp215.f1=_tmp673;});({void*_tmp213[1U]={& _tmp215};({struct Cyc_Tcenv_Tenv*_tmp677=te;unsigned int _tmp676=loc;void**_tmp675=topt;struct _dyneither_ptr _tmp674=({const char*_tmp214="expecting polymorphic type but found %s";_tag_dyneither(_tmp214,sizeof(char),40U);});Cyc_Tcexp_expr_err(_tmp677,_tmp676,_tmp675,_tmp674,_tag_dyneither(_tmp213,sizeof(void*),1U));});});});};}
# 1203
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1205
({unsigned int _tmp67B=loc;struct Cyc_Tcenv_Tenv*_tmp67A=te;struct Cyc_List_List*_tmp679=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp678=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1205
Cyc_Tcutil_check_type(_tmp67B,_tmp67A,_tmp679,_tmp678,1,0,t);});
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
void*_tmp216=({struct Cyc_String_pa_PrintArg_struct _tmp21A;_tmp21A.tag=0U;({struct _dyneither_ptr _tmp67C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp21A.f1=_tmp67C;});({struct Cyc_String_pa_PrintArg_struct _tmp219;_tmp219.tag=0U;({struct _dyneither_ptr _tmp67D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp219.f1=_tmp67D;});({void*_tmp217[2U]={& _tmp219,& _tmp21A};({struct Cyc_Tcenv_Tenv*_tmp680=te;unsigned int _tmp67F=loc;struct _dyneither_ptr _tmp67E=({const char*_tmp218="cannot cast %s to %s";_tag_dyneither(_tmp218,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp680,_tmp67F,& t,_tmp67E,_tag_dyneither(_tmp217,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();
return _tmp216;};}}}
# 1231
{struct _tuple0 _tmp21B=({struct _tuple0 _tmp226;_tmp226.f1=e->r;({void*_tmp681=Cyc_Tcutil_compress(t);_tmp226.f2=_tmp681;});_tmp226;});struct _tuple0 _tmp21C=_tmp21B;int _tmp225;union Cyc_Absyn_Constraint*_tmp224;union Cyc_Absyn_Constraint*_tmp223;union Cyc_Absyn_Constraint*_tmp222;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21C.f1)->tag == 34U){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->tag == 5U){_LL1: _tmp225=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21C.f1)->f1).fat_result;_tmp224=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).nullable;_tmp223=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).bounds;_tmp222=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).zero_term;_LL2:
# 1235
 if((_tmp225  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp222)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp224)){
void*_tmp21D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp223);void*_tmp21E=_tmp21D;struct Cyc_Absyn_Exp*_tmp221;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp21E)->tag == 1U){_LL6: _tmp221=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp21E)->f1;_LL7:
# 1238
 if((Cyc_Evexp_eval_const_uint_exp(_tmp221)).f1 == 1)
({void*_tmp21F=0U;({unsigned int _tmp683=loc;struct _dyneither_ptr _tmp682=({const char*_tmp220="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp220,sizeof(char),60U);});Cyc_Tcutil_warn(_tmp683,_tmp682,_tag_dyneither(_tmp21F,sizeof(void*),0U));});});
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1244
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1247
return t;};}
# 1251
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1253
void**_tmp227=0;
struct Cyc_Absyn_Tqual _tmp228=Cyc_Absyn_empty_tqual(0U);
int _tmp229=0;
if(topt != 0){
void*_tmp22A=Cyc_Tcutil_compress(*topt);void*_tmp22B=_tmp22A;void**_tmp22E;struct Cyc_Absyn_Tqual _tmp22D;union Cyc_Absyn_Constraint*_tmp22C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->tag == 5U){_LL1: _tmp22E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_typ;_tmp22D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_tq;_tmp22C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).ptr_atts).nullable;_LL2:
# 1259
 _tmp227=_tmp22E;
_tmp228=_tmp22D;
({int _tmp684=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp22C);_tmp229=_tmp684;});
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1273
({struct Cyc_Tcenv_Tenv*_tmp686=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp685=_tmp227;Cyc_Tcexp_tcExpNoInst(_tmp686,_tmp685,e);});
# 1276
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp22F=0U;({unsigned int _tmp688=e->loc;struct _dyneither_ptr _tmp687=({const char*_tmp230="Cannot take the address of an alias-free path";_tag_dyneither(_tmp230,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp688,_tmp687,_tag_dyneither(_tmp22F,sizeof(void*),0U));});});
# 1282
{void*_tmp231=e->r;void*_tmp232=_tmp231;struct Cyc_Absyn_Exp*_tmp236;struct Cyc_Absyn_Exp*_tmp235;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->tag == 23U){_LL6: _tmp236=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->f1;_tmp235=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->f2;_LL7:
# 1284
{void*_tmp233=Cyc_Tcutil_compress((void*)_check_null(_tmp236->topt));void*_tmp234=_tmp233;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp234)->tag == 10U){_LLB: _LLC:
 goto _LLA;}else{_LLD: _LLE:
# 1289
({void*_tmp689=(Cyc_Absyn_add_exp(_tmp236,_tmp235,0U))->r;e0->r=_tmp689;});
return Cyc_Tcexp_tcPlus(te,_tmp236,_tmp235);}_LLA:;}
# 1292
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1297
{void*_tmp237=e->r;void*_tmp238=_tmp237;switch(*((int*)_tmp238)){case 21U: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp238)->f3 == 1){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case 22U: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp238)->f3 == 1){_LL12: _LL13:
# 1300
({void*_tmp239=0U;({unsigned int _tmp68B=e->loc;struct _dyneither_ptr _tmp68A=({const char*_tmp23A="cannot take the address of a @tagged union member";_tag_dyneither(_tmp23A,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp68B,_tmp68A,_tag_dyneither(_tmp239,sizeof(void*),0U));});});
goto _LLF;}else{goto _LL14;}default: _LL14: _LL15:
 goto _LLF;}_LLF:;}{
# 1306
struct _tuple14 _tmp23B=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp23C=_tmp23B;int _tmp240;void*_tmp23F;_LL17: _tmp240=_tmp23C.f1;_tmp23F=_tmp23C.f2;_LL18:;
# 1308
if(Cyc_Tcutil_is_noalias_region(_tmp23F,0))
({void*_tmp23D=0U;({unsigned int _tmp68D=e->loc;struct _dyneither_ptr _tmp68C=({const char*_tmp23E="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp23E,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp68D,_tmp68C,_tag_dyneither(_tmp23D,sizeof(void*),0U));});});{
# 1311
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0U);
if(_tmp240){
tq.print_const=1;
tq.real_const=1;}{
# 1317
void*t=_tmp229?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp23F,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp23F,tq,Cyc_Absyn_false_conref);
return t;};};};}
# 1324
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1328
return Cyc_Absyn_uint_typ;
# 1330
({unsigned int _tmp690=loc;struct Cyc_Tcenv_Tenv*_tmp68F=te;struct Cyc_List_List*_tmp68E=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp690,_tmp68F,_tmp68E,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp243;_tmp243.tag=0U;({struct _dyneither_ptr _tmp691=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp243.f1=_tmp691;});({void*_tmp241[1U]={& _tmp243};({unsigned int _tmp693=loc;struct _dyneither_ptr _tmp692=({const char*_tmp242="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp242,sizeof(char),55U);});Cyc_Tcutil_terr(_tmp693,_tmp692,_tag_dyneither(_tmp241,sizeof(void*),1U));});});});
if(topt != 0){
void*_tmp244=Cyc_Tcutil_compress(*topt);void*_tmp245=_tmp244;void*_tmp24B;void*_tmp24A;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp245)->tag == 19U){_LL1: _tmp24B=_tmp245;_tmp24A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp245)->f1;_LL2: {
# 1339
struct Cyc_Absyn_Exp*_tmp246=Cyc_Absyn_sizeoftyp_exp(t,0U);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp247=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp248=_cycalloc(sizeof(*_tmp248));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp694=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp249;_tmp249.tag=18U;_tmp249.f1=_tmp246;_tmp249;});_tmp248[0]=_tmp694;});_tmp248;});
if(Cyc_Tcutil_unify(_tmp24A,(void*)_tmp247))return _tmp24B;
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1345
return Cyc_Absyn_uint_typ;}
# 1348
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1355
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1357
({unsigned int _tmp697=loc;struct Cyc_Tcenv_Tenv*_tmp696=te;struct Cyc_List_List*_tmp695=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp697,_tmp696,_tmp695,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp24C=fs;
void*_tmp24D=t_orig;
for(0;_tmp24C != 0;_tmp24C=_tmp24C->tl){
void*_tmp24E=(void*)_tmp24C->hd;
void*_tmp24F=_tmp24E;unsigned int _tmp289;struct _dyneither_ptr*_tmp288;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24F)->tag == 0U){_LL1: _tmp288=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24F)->f1;_LL2: {
# 1365
int bad_type=1;
{void*_tmp250=Cyc_Tcutil_compress(_tmp24D);void*_tmp251=_tmp250;struct Cyc_List_List*_tmp259;struct Cyc_Absyn_Aggrdecl**_tmp258;switch(*((int*)_tmp251)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp258=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).aggr_info).KnownAggr).val;_LL7:
# 1368
 if((*_tmp258)->impl == 0)goto _LL5;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp288,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp258)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp254;_tmp254.tag=0U;_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp288);({void*_tmp252[1U]={& _tmp254};({unsigned int _tmp699=loc;struct _dyneither_ptr _tmp698=({const char*_tmp253="no field of struct/union has name %s";_tag_dyneither(_tmp253,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp699,_tmp698,_tag_dyneither(_tmp252,sizeof(void*),1U));});});});else{
# 1373
_tmp24D=t2;}
bad_type=0;
goto _LL5;};}else{goto _LLA;}case 12U: _LL8: _tmp259=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp251)->f2;_LL9: {
# 1377
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp288,_tmp259);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp257;_tmp257.tag=0U;_tmp257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp288);({void*_tmp255[1U]={& _tmp257};({unsigned int _tmp69B=loc;struct _dyneither_ptr _tmp69A=({const char*_tmp256="no field of struct/union has name %s";_tag_dyneither(_tmp256,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp69B,_tmp69A,_tag_dyneither(_tmp255,sizeof(void*),1U));});});});else{
# 1381
_tmp24D=t2;}
bad_type=0;
goto _LL5;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1386
if(bad_type){
if(_tmp24C == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp25C;_tmp25C.tag=0U;({struct _dyneither_ptr _tmp69C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp25C.f1=_tmp69C;});({void*_tmp25A[1U]={& _tmp25C};({unsigned int _tmp69E=loc;struct _dyneither_ptr _tmp69D=({const char*_tmp25B="%s is not a known struct/union type";_tag_dyneither(_tmp25B,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp69E,_tmp69D,_tag_dyneither(_tmp25A,sizeof(void*),1U));});});});else{
# 1390
struct _dyneither_ptr _tmp25D=({struct Cyc_String_pa_PrintArg_struct _tmp270;_tmp270.tag=0U;({struct _dyneither_ptr _tmp69F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp270.f1=_tmp69F;});({void*_tmp26E[1U]={& _tmp270};({struct _dyneither_ptr _tmp6A0=({const char*_tmp26F="(%s)";_tag_dyneither(_tmp26F,sizeof(char),5U);});Cyc_aprintf(_tmp6A0,_tag_dyneither(_tmp26E,sizeof(void*),1U));});});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp24C;x=x->tl){
void*_tmp25E=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp25F=_tmp25E;unsigned int _tmp269;struct _dyneither_ptr*_tmp268;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->tag == 0U){_LLD: _tmp268=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->f1;_LLE:
# 1395
({struct _dyneither_ptr _tmp6A2=({struct Cyc_String_pa_PrintArg_struct _tmp263;_tmp263.tag=0U;_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp268);({struct Cyc_String_pa_PrintArg_struct _tmp262;_tmp262.tag=0U;_tmp262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp260[2U]={& _tmp262,& _tmp263};({struct _dyneither_ptr _tmp6A1=({const char*_tmp261="%s.%s";_tag_dyneither(_tmp261,sizeof(char),6U);});Cyc_aprintf(_tmp6A1,_tag_dyneither(_tmp260,sizeof(void*),2U));});});});});_tmp25D=_tmp6A2;});goto _LLC;}else{_LLF: _tmp269=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp25F)->f1;_LL10:
# 1397
({struct _dyneither_ptr _tmp6A4=({struct Cyc_Int_pa_PrintArg_struct _tmp267;_tmp267.tag=1U;_tmp267.f1=(unsigned long)((int)_tmp269);({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0U;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp264[2U]={& _tmp266,& _tmp267};({struct _dyneither_ptr _tmp6A3=({const char*_tmp265="%s.%d";_tag_dyneither(_tmp265,sizeof(char),6U);});Cyc_aprintf(_tmp6A3,_tag_dyneither(_tmp264,sizeof(void*),2U));});});});});_tmp25D=_tmp6A4;});goto _LLC;}_LLC:;}
# 1399
({struct Cyc_String_pa_PrintArg_struct _tmp26D;_tmp26D.tag=0U;({struct _dyneither_ptr _tmp6A5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp26D.f1=_tmp6A5;});({struct Cyc_String_pa_PrintArg_struct _tmp26C;_tmp26C.tag=0U;_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp26A[2U]={& _tmp26C,& _tmp26D};({unsigned int _tmp6A7=loc;struct _dyneither_ptr _tmp6A6=({const char*_tmp26B="%s == %s is not a struct/union type";_tag_dyneither(_tmp26B,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp6A7,_tmp6A6,_tag_dyneither(_tmp26A,sizeof(void*),2U));});});});});}}
# 1402
goto _LL0;}}else{_LL3: _tmp289=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp24F)->f1;_LL4: {
# 1404
int bad_type=1;
{void*_tmp271=Cyc_Tcutil_compress(_tmp24D);void*_tmp272=_tmp271;struct Cyc_Absyn_Datatypefield*_tmp284;struct Cyc_List_List*_tmp283;struct Cyc_List_List*_tmp282;struct Cyc_Absyn_Aggrdecl**_tmp281;switch(*((int*)_tmp272)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp272)->f1).aggr_info).KnownAggr).tag == 2){_LL12: _tmp281=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp272)->f1).aggr_info).KnownAggr).val;_LL13:
# 1407
 if((*_tmp281)->impl == 0)
goto _LL11;
_tmp282=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp281)->impl))->fields;goto _LL15;}else{goto _LL1A;}case 12U: _LL14: _tmp282=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp272)->f2;_LL15:
# 1411
 if(({int _tmp6A8=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp282);_tmp6A8 <= _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp276;_tmp276.tag=1U;_tmp276.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp275;_tmp275.tag=1U;({unsigned long _tmp6A9=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp282);_tmp275.f1=_tmp6A9;});({void*_tmp273[2U]={& _tmp275,& _tmp276};({unsigned int _tmp6AB=loc;struct _dyneither_ptr _tmp6AA=({const char*_tmp274="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp274,sizeof(char),46U);});Cyc_Tcutil_terr(_tmp6AB,_tmp6AA,_tag_dyneither(_tmp273,sizeof(void*),2U));});});});});else{
# 1415
({void*_tmp6AC=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp282,(int)_tmp289))->type;_tmp24D=_tmp6AC;});}
bad_type=0;
goto _LL11;case 10U: _LL16: _tmp283=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp272)->f1;_LL17:
# 1419
 if(({int _tmp6AD=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp283);_tmp6AD <= _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp27A;_tmp27A.tag=1U;_tmp27A.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp279;_tmp279.tag=1U;({unsigned long _tmp6AE=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp283);_tmp279.f1=_tmp6AE;});({void*_tmp277[2U]={& _tmp279,& _tmp27A};({unsigned int _tmp6B0=loc;struct _dyneither_ptr _tmp6AF=({const char*_tmp278="tuple has too few components: %d <= %d";_tag_dyneither(_tmp278,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp6B0,_tmp6AF,_tag_dyneither(_tmp277,sizeof(void*),2U));});});});});else{
# 1423
({void*_tmp6B1=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp283,(int)_tmp289)).f2;_tmp24D=_tmp6B1;});}
bad_type=0;
goto _LL11;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp272)->f1).field_info).KnownDatatypefield).tag == 2){_LL18: _tmp284=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp272)->f1).field_info).KnownDatatypefield).val).f2;_LL19:
# 1427
 if(({int _tmp6B2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp284->typs);_tmp6B2 < _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp27E;_tmp27E.tag=1U;_tmp27E.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp27D;_tmp27D.tag=1U;({unsigned long _tmp6B3=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp284->typs);_tmp27D.f1=_tmp6B3;});({void*_tmp27B[2U]={& _tmp27D,& _tmp27E};({unsigned int _tmp6B5=loc;struct _dyneither_ptr _tmp6B4=({const char*_tmp27C="datatype field has too few components: %d < %d";_tag_dyneither(_tmp27C,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp6B5,_tmp6B4,_tag_dyneither(_tmp27B,sizeof(void*),2U));});});});});else{
# 1431
if(_tmp289 != 0)
({void*_tmp6B6=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp284->typs,(int)(_tmp289 - 1))).f2;_tmp24D=_tmp6B6;});else{
if(_tmp24C->tl != 0)
({void*_tmp27F=0U;({unsigned int _tmp6B8=loc;struct _dyneither_ptr _tmp6B7=({const char*_tmp280="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp280,sizeof(char),68U);});Cyc_Tcutil_terr(_tmp6B8,_tmp6B7,_tag_dyneither(_tmp27F,sizeof(void*),0U));});});}}
# 1436
bad_type=0;
goto _LL11;}else{goto _LL1A;}default: _LL1A: _LL1B:
 goto _LL11;}_LL11:;}
# 1440
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp287;_tmp287.tag=0U;({struct _dyneither_ptr _tmp6B9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp287.f1=_tmp6B9;});({void*_tmp285[1U]={& _tmp287};({unsigned int _tmp6BB=loc;struct _dyneither_ptr _tmp6BA=({const char*_tmp286="%s is not a known type";_tag_dyneither(_tmp286,sizeof(char),23U);});Cyc_Tcutil_terr(_tmp6BB,_tmp6BA,_tag_dyneither(_tmp285,sizeof(void*),1U));});});});
goto _LL0;}}_LL0:;}
# 1445
return Cyc_Absyn_uint_typ;};}
# 1449
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp28A=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp28A,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp28B=t;void*_tmp2A1;void*_tmp2A0;union Cyc_Absyn_Constraint*_tmp29F;union Cyc_Absyn_Constraint*_tmp29E;switch(*((int*)_tmp28B)){case 1U: _LL1: _LL2: {
# 1455
struct Cyc_List_List*_tmp28C=Cyc_Tcenv_lookup_type_vars(_tmp28A);
void*_tmp28D=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->v=_tmp28C;_tmp298;}));
void*_tmp28E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->v=_tmp28C;_tmp297;}));
union Cyc_Absyn_Constraint*_tmp28F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp290=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp291=({struct Cyc_Absyn_PtrAtts _tmp296;_tmp296.rgn=_tmp28E;({union Cyc_Absyn_Constraint*_tmp6BC=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp296.nullable=_tmp6BC;});_tmp296.bounds=_tmp28F;_tmp296.zero_term=_tmp290;_tmp296.ptrloc=0;_tmp296;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp292=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp293=_cycalloc(sizeof(*_tmp293));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6BF=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp294;_tmp294.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp6BE=({struct Cyc_Absyn_PtrInfo _tmp295;_tmp295.elt_typ=_tmp28D;({struct Cyc_Absyn_Tqual _tmp6BD=Cyc_Absyn_empty_tqual(0U);_tmp295.elt_tq=_tmp6BD;});_tmp295.ptr_atts=_tmp291;_tmp295;});_tmp294.f1=_tmp6BE;});_tmp294;});_tmp293[0]=_tmp6BF;});_tmp293;});
Cyc_Tcutil_unify(t,(void*)_tmp292);
_tmp2A1=_tmp28D;_tmp2A0=_tmp28E;_tmp29F=_tmp28F;_tmp29E=_tmp290;goto _LL4;}case 5U: _LL3: _tmp2A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B)->f1).elt_typ;_tmp2A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B)->f1).ptr_atts).rgn;_tmp29F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B)->f1).ptr_atts).bounds;_tmp29E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B)->f1).ptr_atts).zero_term;_LL4:
# 1465
 Cyc_Tcenv_check_rgn_accessible(_tmp28A,loc,_tmp2A0);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp29F);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2A1),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp28A))
({void*_tmp299=0U;({unsigned int _tmp6C1=loc;struct _dyneither_ptr _tmp6C0=({const char*_tmp29A="can't dereference abstract pointer type";_tag_dyneither(_tmp29A,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp6C1,_tmp6C0,_tag_dyneither(_tmp299,sizeof(void*),0U));});});
return _tmp2A1;default: _LL5: _LL6:
# 1471
 return({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0U;({struct _dyneither_ptr _tmp6C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp29D.f1=_tmp6C2;});({void*_tmp29B[1U]={& _tmp29D};({struct Cyc_Tcenv_Tenv*_tmp6C6=_tmp28A;unsigned int _tmp6C5=loc;void**_tmp6C4=topt;struct _dyneither_ptr _tmp6C3=({const char*_tmp29C="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp29C,sizeof(char),39U);});Cyc_Tcexp_expr_err(_tmp6C6,_tmp6C5,_tmp6C4,_tmp6C3,_tag_dyneither(_tmp29B,sizeof(void*),1U));});});});}_LL0:;};}
# 1476
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1480
({struct Cyc_Tcenv_Tenv*_tmp6C7=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6C7,0,e);});
# 1482
({int _tmp6C8=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6C8;});{
void*_tmp2A2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A3=_tmp2A2;enum Cyc_Absyn_AggrKind _tmp2BD;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_Aggrdecl*_tmp2BB;struct Cyc_List_List*_tmp2BA;switch(*((int*)_tmp2A3)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A3)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp2BB=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A3)->f1).aggr_info).KnownAggr).val;_tmp2BA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A3)->f1).targs;_LL2: {
# 1485
struct Cyc_Absyn_Aggrfield*_tmp2A4=Cyc_Absyn_lookup_decl_field(_tmp2BB,f);
if(_tmp2A4 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0U;_tmp2A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0U;({struct _dyneither_ptr _tmp6C9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2BB->name));_tmp2A7.f1=_tmp6C9;});({void*_tmp2A5[2U]={& _tmp2A7,& _tmp2A8};({struct Cyc_Tcenv_Tenv*_tmp6CD=te;unsigned int _tmp6CC=loc;void**_tmp6CB=topt;struct _dyneither_ptr _tmp6CA=({const char*_tmp2A6="%s has no %s member";_tag_dyneither(_tmp2A6,sizeof(char),20U);});Cyc_Tcexp_expr_err(_tmp6CD,_tmp6CC,_tmp6CB,_tmp6CA,_tag_dyneither(_tmp2A5,sizeof(void*),2U));});});});});
# 1490
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BB->impl))->tagged)*is_tagged=1;{
void*t2=_tmp2A4->type;
if(_tmp2BA != 0){
struct Cyc_List_List*_tmp2A9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2BB->tvs,_tmp2BA);
({void*_tmp6CE=Cyc_Tcutil_substitute(_tmp2A9,_tmp2A4->type);t2=_tmp6CE;});}
# 1498
if((((_tmp2BB->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BB->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2A4->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0U;_tmp2AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2AA[1U]={& _tmp2AC};({struct Cyc_Tcenv_Tenv*_tmp6D2=te;unsigned int _tmp6D1=loc;void**_tmp6D0=topt;struct _dyneither_ptr _tmp6CF=({const char*_tmp2AB="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2AB,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6D2,_tmp6D1,_tmp6D0,_tmp6CF,_tag_dyneither(_tmp2AA,sizeof(void*),1U));});});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BB->impl))->exist_vars != 0){
# 1503
if(!({void*_tmp6D3=t2;Cyc_Tcutil_unify(_tmp6D3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0U;_tmp2AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2AD[1U]={& _tmp2AF};({struct Cyc_Tcenv_Tenv*_tmp6D7=te;unsigned int _tmp6D6=loc;void**_tmp6D5=topt;struct _dyneither_ptr _tmp6D4=({const char*_tmp2AE="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp2AE,sizeof(char),81U);});Cyc_Tcexp_expr_err(_tmp6D7,_tmp6D6,_tmp6D5,_tmp6D4,_tag_dyneither(_tmp2AD,sizeof(void*),1U));});});});}
# 1506
return t2;};}}else{goto _LL5;}case 12U: _LL3: _tmp2BD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A3)->f1;_tmp2BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A3)->f2;_LL4: {
# 1508
struct Cyc_Absyn_Aggrfield*_tmp2B0=Cyc_Absyn_lookup_field(_tmp2BC,f);
if(_tmp2B0 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0U;_tmp2B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2B1[1U]={& _tmp2B3};({struct Cyc_Tcenv_Tenv*_tmp6DB=te;unsigned int _tmp6DA=loc;void**_tmp6D9=topt;struct _dyneither_ptr _tmp6D8=({const char*_tmp2B2="type has no %s member";_tag_dyneither(_tmp2B2,sizeof(char),22U);});Cyc_Tcexp_expr_err(_tmp6DB,_tmp6DA,_tmp6D9,_tmp6D8,_tag_dyneither(_tmp2B1,sizeof(void*),1U));});});});
# 1513
if(((_tmp2BD == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2B0->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2B0->requires_clause == 0)
# 1515
return({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0U;_tmp2B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2B4[1U]={& _tmp2B6};({struct Cyc_Tcenv_Tenv*_tmp6DF=te;unsigned int _tmp6DE=loc;void**_tmp6DD=topt;struct _dyneither_ptr _tmp6DC=({const char*_tmp2B5="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2B5,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6DF,_tmp6DE,_tmp6DD,_tmp6DC,_tag_dyneither(_tmp2B4,sizeof(void*),1U));});});});
return _tmp2B0->type;}default: _LL5: _LL6:
# 1518
 return({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0U;({struct _dyneither_ptr _tmp6E0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp2B9.f1=_tmp6E0;});({void*_tmp2B7[1U]={& _tmp2B9};({struct Cyc_Tcenv_Tenv*_tmp6E4=te;unsigned int _tmp6E3=loc;void**_tmp6E2=topt;struct _dyneither_ptr _tmp6E1=({const char*_tmp2B8="expecting struct or union, found %s";_tag_dyneither(_tmp2B8,sizeof(char),36U);});Cyc_Tcexp_expr_err(_tmp6E4,_tmp6E3,_tmp6E2,_tmp6E1,_tag_dyneither(_tmp2B7,sizeof(void*),1U));});});});}_LL0:;};}
# 1524
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1527
({struct Cyc_Tcenv_Tenv*_tmp6E5=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp6E5,0,e);});
# 1529
({int _tmp6E6=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6E6;});
{void*_tmp2BE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2BF=_tmp2BE;void*_tmp2E2;void*_tmp2E1;union Cyc_Absyn_Constraint*_tmp2E0;union Cyc_Absyn_Constraint*_tmp2DF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->tag == 5U){_LL1: _tmp2E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).elt_typ;_tmp2E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).rgn;_tmp2E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).bounds;_tmp2DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).zero_term;_LL2:
# 1532
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2E0);
{void*_tmp2C0=Cyc_Tcutil_compress(_tmp2E2);void*_tmp2C1=_tmp2C0;enum Cyc_Absyn_AggrKind _tmp2DE;struct Cyc_List_List*_tmp2DD;struct Cyc_Absyn_Aggrdecl*_tmp2DC;struct Cyc_List_List*_tmp2DB;switch(*((int*)_tmp2C1)){case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C1)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp2DC=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C1)->f1).aggr_info).KnownAggr).val;_tmp2DB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C1)->f1).targs;_LL7: {
# 1535
struct Cyc_Absyn_Aggrfield*_tmp2C2=Cyc_Absyn_lookup_decl_field(_tmp2DC,f);
if(_tmp2C2 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0U;_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=0U;({struct _dyneither_ptr _tmp6E7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2DC->name));_tmp2C5.f1=_tmp6E7;});({void*_tmp2C3[2U]={& _tmp2C5,& _tmp2C6};({struct Cyc_Tcenv_Tenv*_tmp6EB=te;unsigned int _tmp6EA=loc;void**_tmp6E9=topt;struct _dyneither_ptr _tmp6E8=({const char*_tmp2C4="type %s has no %s member";_tag_dyneither(_tmp2C4,sizeof(char),25U);});Cyc_Tcexp_expr_err(_tmp6EB,_tmp6EA,_tmp6E9,_tmp6E8,_tag_dyneither(_tmp2C3,sizeof(void*),2U));});});});});
# 1540
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DC->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2C2->type;
if(_tmp2DB != 0){
struct Cyc_List_List*_tmp2C7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2DC->tvs,_tmp2DB);
({void*_tmp6EC=Cyc_Tcutil_substitute(_tmp2C7,_tmp2C2->type);t3=_tmp6EC;});}{
# 1546
struct Cyc_Absyn_Kind*_tmp2C8=Cyc_Tcutil_typ_kind(t3);
# 1549
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2C8) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2C9=Cyc_Tcutil_compress(t3);void*_tmp2CA=_tmp2C9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CA)->tag == 8U){_LLD: _LLE:
 goto _LLC;}else{_LLF: _LL10:
# 1553
 return({struct Cyc_String_pa_PrintArg_struct _tmp2CD;_tmp2CD.tag=0U;_tmp2CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CB[1U]={& _tmp2CD};({struct Cyc_Tcenv_Tenv*_tmp6F0=te;unsigned int _tmp6EF=loc;void**_tmp6EE=topt;struct _dyneither_ptr _tmp6ED=({const char*_tmp2CC="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2CC,sizeof(char),48U);});Cyc_Tcexp_expr_err(_tmp6F0,_tmp6EF,_tmp6EE,_tmp6ED,_tag_dyneither(_tmp2CB,sizeof(void*),1U));});});});}_LLC:;}
# 1558
if((((_tmp2DC->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DC->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2C2->requires_clause == 0)
# 1561
return({struct Cyc_String_pa_PrintArg_struct _tmp2D0;_tmp2D0.tag=0U;_tmp2D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CE[1U]={& _tmp2D0};({struct Cyc_Tcenv_Tenv*_tmp6F4=te;unsigned int _tmp6F3=loc;void**_tmp6F2=topt;struct _dyneither_ptr _tmp6F1=({const char*_tmp2CF="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2CF,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp6F4,_tmp6F3,_tmp6F2,_tmp6F1,_tag_dyneither(_tmp2CE,sizeof(void*),1U));});});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DC->impl))->exist_vars != 0){
if(!({void*_tmp6F5=t3;Cyc_Tcutil_unify(_tmp6F5,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2D3;_tmp2D3.tag=0U;_tmp2D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D1[1U]={& _tmp2D3};({struct Cyc_Tcenv_Tenv*_tmp6F9=te;unsigned int _tmp6F8=loc;void**_tmp6F7=topt;struct _dyneither_ptr _tmp6F6=({const char*_tmp2D2="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2D2,sizeof(char),72U);});Cyc_Tcexp_expr_err(_tmp6F9,_tmp6F8,_tmp6F7,_tmp6F6,_tag_dyneither(_tmp2D1,sizeof(void*),1U));});});});}
# 1568
return t3;};};}}else{goto _LLA;}case 12U: _LL8: _tmp2DE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1)->f1;_tmp2DD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1)->f2;_LL9: {
# 1570
struct Cyc_Absyn_Aggrfield*_tmp2D4=Cyc_Absyn_lookup_field(_tmp2DD,f);
if(_tmp2D4 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2D7;_tmp2D7.tag=0U;_tmp2D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D5[1U]={& _tmp2D7};({struct Cyc_Tcenv_Tenv*_tmp6FD=te;unsigned int _tmp6FC=loc;void**_tmp6FB=topt;struct _dyneither_ptr _tmp6FA=({const char*_tmp2D6="type has no %s field";_tag_dyneither(_tmp2D6,sizeof(char),21U);});Cyc_Tcexp_expr_err(_tmp6FD,_tmp6FC,_tmp6FB,_tmp6FA,_tag_dyneither(_tmp2D5,sizeof(void*),1U));});});});
# 1575
if((_tmp2DE == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2D4->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2DA;_tmp2DA.tag=0U;_tmp2DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D8[1U]={& _tmp2DA};({struct Cyc_Tcenv_Tenv*_tmp701=te;unsigned int _tmp700=loc;void**_tmp6FF=topt;struct _dyneither_ptr _tmp6FE=({const char*_tmp2D9="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2D9,sizeof(char),56U);});Cyc_Tcexp_expr_err(_tmp701,_tmp700,_tmp6FF,_tmp6FE,_tag_dyneither(_tmp2D8,sizeof(void*),1U));});});});
return _tmp2D4->type;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1580
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1583
return({struct Cyc_String_pa_PrintArg_struct _tmp2E5;_tmp2E5.tag=0U;({struct _dyneither_ptr _tmp702=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp2E5.f1=_tmp702;});({void*_tmp2E3[1U]={& _tmp2E5};({struct Cyc_Tcenv_Tenv*_tmp706=te;unsigned int _tmp705=loc;void**_tmp704=topt;struct _dyneither_ptr _tmp703=({const char*_tmp2E4="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2E4,sizeof(char),44U);});Cyc_Tcexp_expr_err(_tmp706,_tmp705,_tmp704,_tmp703,_tag_dyneither(_tmp2E3,sizeof(void*),1U));});});});}
# 1588
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1590
struct _tuple15 _tmp2E6=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2E7=_tmp2E6;unsigned int _tmp2F5;int _tmp2F4;_LL1: _tmp2F5=_tmp2E7.f1;_tmp2F4=_tmp2E7.f2;_LL2:;
if(!_tmp2F4)
return({void*_tmp2E8=0U;({struct Cyc_Tcenv_Tenv*_tmp709=te;unsigned int _tmp708=loc;struct _dyneither_ptr _tmp707=({const char*_tmp2E9="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2E9,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp709,_tmp708,0,_tmp707,_tag_dyneither(_tmp2E8,sizeof(void*),0U));});});{
# 1594
struct _handler_cons _tmp2EA;_push_handler(& _tmp2EA);{int _tmp2EC=0;if(setjmp(_tmp2EA.handler))_tmp2EC=1;if(!_tmp2EC){
{void*_tmp2ED=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2F5)).f2;_npop_handler(0U);return _tmp2ED;};_pop_handler();}else{void*_tmp2EB=(void*)_exn_thrown;void*_tmp2EE=_tmp2EB;void*_tmp2F3;if(((struct Cyc_List_Nth_exn_struct*)_tmp2EE)->tag == Cyc_List_Nth){_LL4: _LL5:
# 1597
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2F2;_tmp2F2.tag=1U;({unsigned long _tmp70A=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp2F2.f1=_tmp70A;});({struct Cyc_Int_pa_PrintArg_struct _tmp2F1;_tmp2F1.tag=1U;_tmp2F1.f1=(unsigned long)((int)_tmp2F5);({void*_tmp2EF[2U]={& _tmp2F1,& _tmp2F2};({struct Cyc_Tcenv_Tenv*_tmp70D=te;unsigned int _tmp70C=loc;struct _dyneither_ptr _tmp70B=({const char*_tmp2F0="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2F0,sizeof(char),41U);});Cyc_Tcexp_expr_err(_tmp70D,_tmp70C,0,_tmp70B,_tag_dyneither(_tmp2EF,sizeof(void*),2U));});});});});}else{_LL6: _tmp2F3=_tmp2EE;_LL7:(int)_rethrow(_tmp2F3);}_LL3:;}};};}
# 1602
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1604
struct Cyc_Tcenv_Tenv*_tmp2F6=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp70E=Cyc_Tcenv_clear_abstract_val_ok(_tmp2F6);Cyc_Tcexp_tcExp(_tmp70E,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp70F=Cyc_Tcenv_clear_abstract_val_ok(_tmp2F6);Cyc_Tcexp_tcExp(_tmp70F,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2F6,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2F9;_tmp2F9.tag=0U;({struct _dyneither_ptr _tmp710=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp2F9.f1=_tmp710;});({void*_tmp2F7[1U]={& _tmp2F9};({struct Cyc_Tcenv_Tenv*_tmp714=_tmp2F6;unsigned int _tmp713=e2->loc;void**_tmp712=topt;struct _dyneither_ptr _tmp711=({const char*_tmp2F8="expecting int subscript, found %s";_tag_dyneither(_tmp2F8,sizeof(char),34U);});Cyc_Tcexp_expr_err(_tmp714,_tmp713,_tmp712,_tmp711,_tag_dyneither(_tmp2F7,sizeof(void*),1U));});});});{
# 1614
void*_tmp2FA=t1;struct Cyc_List_List*_tmp318;void*_tmp317;struct Cyc_Absyn_Tqual _tmp316;void*_tmp315;union Cyc_Absyn_Constraint*_tmp314;union Cyc_Absyn_Constraint*_tmp313;switch(*((int*)_tmp2FA)){case 5U: _LL1: _tmp317=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).elt_typ;_tmp316=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).elt_tq;_tmp315=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).rgn;_tmp314=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).bounds;_tmp313=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).zero_term;_LL2:
# 1618
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp313)){
int emit_warning=1;
{void*_tmp2FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp314);void*_tmp2FC=_tmp2FB;struct Cyc_Absyn_Exp*_tmp305;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FC)->tag == 1U){_LL8: _tmp305=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FC)->f1;_LL9:
# 1622
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2FD=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2FE=_tmp2FD;unsigned int _tmp304;int _tmp303;_LLD: _tmp304=_tmp2FE.f1;_tmp303=_tmp2FE.f2;_LLE:;
if(_tmp303){
struct _tuple15 _tmp2FF=Cyc_Evexp_eval_const_uint_exp(_tmp305);struct _tuple15 _tmp300=_tmp2FF;unsigned int _tmp302;int _tmp301;_LL10: _tmp302=_tmp300.f1;_tmp301=_tmp300.f2;_LL11:;
if(_tmp301  && _tmp302 > _tmp304)emit_warning=0;}}
# 1629
goto _LL7;}else{_LLA: _LLB:
 emit_warning=0;goto _LL7;}_LL7:;}
# 1632
if(emit_warning)
({void*_tmp306=0U;({unsigned int _tmp716=e2->loc;struct _dyneither_ptr _tmp715=({const char*_tmp307="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp307,sizeof(char),63U);});Cyc_Tcutil_warn(_tmp716,_tmp715,_tag_dyneither(_tmp306,sizeof(void*),0U));});});}else{
# 1636
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp308=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp309=_tmp308;unsigned int _tmp30B;int _tmp30A;_LL13: _tmp30B=_tmp309.f1;_tmp30A=_tmp309.f2;_LL14:;
if(_tmp30A)
Cyc_Tcutil_check_bound(loc,_tmp30B,_tmp314);}else{
# 1642
if(Cyc_Tcutil_is_bound_one(_tmp314) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp313))
({void*_tmp30C=0U;({unsigned int _tmp718=e1->loc;struct _dyneither_ptr _tmp717=({const char*_tmp30D="subscript applied to pointer to one object";_tag_dyneither(_tmp30D,sizeof(char),43U);});Cyc_Tcutil_warn(_tmp718,_tmp717,_tag_dyneither(_tmp30C,sizeof(void*),0U));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp314);}}
# 1647
Cyc_Tcenv_check_rgn_accessible(_tmp2F6,loc,_tmp315);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp317),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2F6))
({void*_tmp30E=0U;({unsigned int _tmp71A=e1->loc;struct _dyneither_ptr _tmp719=({const char*_tmp30F="can't subscript an abstract pointer";_tag_dyneither(_tmp30F,sizeof(char),36U);});Cyc_Tcutil_terr(_tmp71A,_tmp719,_tag_dyneither(_tmp30E,sizeof(void*),0U));});});
return _tmp317;case 10U: _LL3: _tmp318=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FA)->f1;_LL4:
 return Cyc_Tcexp_ithTupleType(_tmp2F6,loc,_tmp318,e2);default: _LL5: _LL6:
 return({struct Cyc_String_pa_PrintArg_struct _tmp312;_tmp312.tag=0U;({struct _dyneither_ptr _tmp71B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp312.f1=_tmp71B;});({void*_tmp310[1U]={& _tmp312};({struct Cyc_Tcenv_Tenv*_tmp71F=_tmp2F6;unsigned int _tmp71E=loc;void**_tmp71D=topt;struct _dyneither_ptr _tmp71C=({const char*_tmp311="subscript applied to %s";_tag_dyneither(_tmp311,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp71F,_tmp71E,_tmp71D,_tmp71C,_tag_dyneither(_tmp310,sizeof(void*),1U));});});});}_LL0:;};};}
# 1657
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp319=Cyc_Tcutil_compress(*topt);void*_tmp31A=_tmp319;struct Cyc_List_List*_tmp31E;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp31A)->tag == 10U){_LL1: _tmp31E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp31A)->f1;_LL2:
# 1663
 if(({int _tmp720=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp31E);_tmp720 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1666
goto _LL0;
# 1668
for(0;es != 0;(es=es->tl,_tmp31E=_tmp31E->tl)){
int bogus=0;
void*_tmp31B=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp31E))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp721=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp721,& _tmp31B,(struct Cyc_Absyn_Exp*)es->hd);});
# 1673
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp31E->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp723=({struct Cyc_List_List*_tmp31C=_cycalloc(sizeof(*_tmp31C));({struct _tuple17*_tmp722=({struct _tuple17*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->f1=(*((struct _tuple17*)_tmp31E->hd)).f1;_tmp31D->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp31D;});_tmp31C->hd=_tmp722;});_tmp31C->tl=fields;_tmp31C;});fields=_tmp723;});}
# 1676
done=1;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1680
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp724=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp724,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp727=({struct Cyc_List_List*_tmp31F=_cycalloc(sizeof(*_tmp31F));({struct _tuple17*_tmp726=({struct _tuple17*_tmp320=_cycalloc(sizeof(*_tmp320));({struct Cyc_Absyn_Tqual _tmp725=Cyc_Absyn_empty_tqual(0U);_tmp320->f1=_tmp725;});_tmp320->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp320;});_tmp31F->hd=_tmp726;});_tmp31F->tl=fields;_tmp31F;});fields=_tmp727;});}
# 1685
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp321=_cycalloc(sizeof(*_tmp321));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp729=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp322;_tmp322.tag=10U;({struct Cyc_List_List*_tmp728=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp322.f1=_tmp728;});_tmp322;});_tmp321[0]=_tmp729;});_tmp321;});}
# 1689
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1692
return({void*_tmp323=0U;({struct Cyc_Tcenv_Tenv*_tmp72D=te;unsigned int _tmp72C=loc;void**_tmp72B=topt;struct _dyneither_ptr _tmp72A=({const char*_tmp324="tcCompoundLit";_tag_dyneither(_tmp324,sizeof(char),14U);});Cyc_Tcexp_expr_err(_tmp72D,_tmp72C,_tmp72B,_tmp72A,_tag_dyneither(_tmp323,sizeof(void*),0U));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1702 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1705
void*res_t2;
int _tmp325=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp326=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp33C=_cycalloc(sizeof(*_tmp33C));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp72F=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp33D;_tmp33D.tag=0U;({union Cyc_Absyn_Cnst _tmp72E=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp325);_tmp33D.f1=_tmp72E;});_tmp33D;});_tmp33C[0]=_tmp72F;});_tmp33C;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp326,loc);
# 1713
if(zero_term){
struct Cyc_Absyn_Exp*_tmp327=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp325 - 1);
if(!Cyc_Tcutil_is_zero(_tmp327))
({void*_tmp328=0U;({unsigned int _tmp731=_tmp327->loc;struct _dyneither_ptr _tmp730=({const char*_tmp329="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp329,sizeof(char),45U);});Cyc_Tcutil_terr(_tmp731,_tmp730,_tag_dyneither(_tmp328,sizeof(void*),0U));});});}
# 1718
sz_exp->topt=Cyc_Absyn_uint_typ;
({void*_tmp735=({void*_tmp734=res;struct Cyc_Absyn_Tqual _tmp733=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0U);
# 1719
struct Cyc_Absyn_Exp*_tmp732=sz_exp;Cyc_Absyn_array_typ(_tmp734,_tmp733,_tmp732,
# 1721
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0U);});
# 1719
res_t2=_tmp735;});
# 1723
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1726
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1728
({struct Cyc_String_pa_PrintArg_struct _tmp32C;_tmp32C.tag=0U;({struct _dyneither_ptr _tmp736=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res));_tmp32C.f1=_tmp736;});({void*_tmp32A[1U]={& _tmp32C};({unsigned int _tmp738=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp737=({const char*_tmp32B="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp32B,sizeof(char),53U);});Cyc_Tcutil_terr(_tmp738,_tmp737,_tag_dyneither(_tmp32A,sizeof(void*),1U));});});});
# 1731
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1736
void*_tmp32D=(void*)ds->hd;void*_tmp32E=_tmp32D;struct Cyc_Absyn_Exp*_tmp33B;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32E)->tag == 1U){_LL1: _LL2:
# 1738
({void*_tmp32F=0U;({unsigned int _tmp73A=loc;struct _dyneither_ptr _tmp739=({const char*_tmp330="only array index designators are supported";_tag_dyneither(_tmp330,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp73A,_tmp739,_tag_dyneither(_tmp32F,sizeof(void*),0U));});});
goto _LL0;}else{_LL3: _tmp33B=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp32E)->f1;_LL4:
# 1741
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp33B);{
struct _tuple15 _tmp331=Cyc_Evexp_eval_const_uint_exp(_tmp33B);struct _tuple15 _tmp332=_tmp331;unsigned int _tmp33A;int _tmp339;_LL6: _tmp33A=_tmp332.f1;_tmp339=_tmp332.f2;_LL7:;
if(!_tmp339)
({void*_tmp333=0U;({unsigned int _tmp73C=_tmp33B->loc;struct _dyneither_ptr _tmp73B=({const char*_tmp334="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp334,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp73C,_tmp73B,_tag_dyneither(_tmp333,sizeof(void*),0U));});});else{
if(_tmp33A != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp338;_tmp338.tag=1U;_tmp338.f1=(unsigned long)((int)_tmp33A);({struct Cyc_Int_pa_PrintArg_struct _tmp337;_tmp337.tag=1U;_tmp337.f1=(unsigned long)offset;({void*_tmp335[2U]={& _tmp337,& _tmp338};({unsigned int _tmp73E=_tmp33B->loc;struct _dyneither_ptr _tmp73D=({const char*_tmp336="expecting index designator %d but found %d";_tag_dyneither(_tmp336,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp73E,_tmp73D,_tag_dyneither(_tmp335,sizeof(void*),2U));});});});});}
# 1748
goto _LL0;};}_LL0:;}}}
# 1752
return res_t2;}
# 1756
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1759
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp33E=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp33F=_tmp33E;void*_tmp344;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp33F)->tag == 19U){_LL1: _tmp344=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp33F)->f1;_LL2:
# 1764
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp340=({void*_tmp73F=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp73F,Cyc_Absyn_valueof_exp(_tmp344,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp340->topt=bound->topt;
# 1769
bound=_tmp340;}
# 1771
goto _LL0;}else{_LL3: _LL4:
# 1773
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp343;_tmp343.tag=0U;({struct _dyneither_ptr _tmp740=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp343.f1=_tmp740;});({void*_tmp341[1U]={& _tmp343};({unsigned int _tmp742=bound->loc;struct _dyneither_ptr _tmp741=({const char*_tmp342="expecting unsigned int, found %s";_tag_dyneither(_tmp342,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp742,_tmp741,_tag_dyneither(_tmp341,sizeof(void*),1U));});});});}_LL0:;}
# 1778
if(!(vd->tq).real_const)
({void*_tmp345=0U;({struct _dyneither_ptr _tmp743=({const char*_tmp346="comprehension index variable is not declared const!";_tag_dyneither(_tmp346,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp743,_tag_dyneither(_tmp345,sizeof(void*),0U));});});
# 1781
if(te->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp744=Cyc_Tcenv_new_block(loc,te);te=_tmp744;});{
void**_tmp347=0;
struct Cyc_Absyn_Tqual*_tmp348=0;
union Cyc_Absyn_Constraint**_tmp349=0;
# 1787
if(topt != 0){
void*_tmp34A=Cyc_Tcutil_compress(*topt);void*_tmp34B=_tmp34A;void*_tmp356;struct Cyc_Absyn_Tqual _tmp355;struct Cyc_Absyn_Exp*_tmp354;union Cyc_Absyn_Constraint*_tmp353;struct Cyc_Absyn_PtrInfo _tmp352;switch(*((int*)_tmp34B)){case 5U: _LL6: _tmp352=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34B)->f1;_LL7:
# 1790
({void**_tmp745=({void**_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=_tmp352.elt_typ;_tmp34C;});_tmp347=_tmp745;});
({struct Cyc_Absyn_Tqual*_tmp746=({struct Cyc_Absyn_Tqual*_tmp34D=_cycalloc_atomic(sizeof(*_tmp34D));_tmp34D[0]=_tmp352.elt_tq;_tmp34D;});_tmp348=_tmp746;});
({union Cyc_Absyn_Constraint**_tmp747=({union Cyc_Absyn_Constraint**_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[0]=(_tmp352.ptr_atts).zero_term;_tmp34E;});_tmp349=_tmp747;});
goto _LL5;case 8U: _LL8: _tmp356=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34B)->f1).elt_type;_tmp355=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34B)->f1).tq;_tmp354=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34B)->f1).num_elts;_tmp353=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34B)->f1).zero_term;_LL9:
# 1795
({void**_tmp748=({void**_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F[0]=_tmp356;_tmp34F;});_tmp347=_tmp748;});
({struct Cyc_Absyn_Tqual*_tmp749=({struct Cyc_Absyn_Tqual*_tmp350=_cycalloc_atomic(sizeof(*_tmp350));_tmp350[0]=_tmp355;_tmp350;});_tmp348=_tmp749;});
({union Cyc_Absyn_Constraint**_tmp74A=({union Cyc_Absyn_Constraint**_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351[0]=_tmp353;_tmp351;});_tmp349=_tmp74A;});
goto _LL5;default: _LLA: _LLB:
# 1800
 goto _LL5;}_LL5:;}{
# 1803
void*t=Cyc_Tcexp_tcExp(te,_tmp347,body);
# 1805
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp357=0U;({unsigned int _tmp74C=body->loc;struct _dyneither_ptr _tmp74B=({const char*_tmp358="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp358,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp74C,_tmp74B,_tag_dyneither(_tmp357,sizeof(void*),0U));});});
if(te->le == 0){
# 1809
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp359=0U;({unsigned int _tmp74E=bound->loc;struct _dyneither_ptr _tmp74D=({const char*_tmp35A="bound is not constant";_tag_dyneither(_tmp35A,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp74E,_tmp74D,_tag_dyneither(_tmp359,sizeof(void*),0U));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp35B=0U;({unsigned int _tmp750=bound->loc;struct _dyneither_ptr _tmp74F=({const char*_tmp35C="body is not constant";_tag_dyneither(_tmp35C,sizeof(char),21U);});Cyc_Tcutil_terr(_tmp750,_tmp74F,_tag_dyneither(_tmp35B,sizeof(void*),0U));});});}
# 1814
if(_tmp349 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp349)){
# 1817
struct Cyc_Absyn_Exp*_tmp35D=Cyc_Absyn_uint_exp(1U,0U);_tmp35D->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp751=Cyc_Absyn_add_exp(bound,_tmp35D,0U);bound=_tmp751;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}
# 1821
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp35E=0U;({unsigned int _tmp753=body->loc;struct _dyneither_ptr _tmp752=({const char*_tmp35F="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp35F,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp753,_tmp752,_tag_dyneither(_tmp35E,sizeof(void*),0U));});});{
# 1825
void*_tmp360=({void*_tmp756=t;struct Cyc_Absyn_Tqual _tmp755=_tmp348 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp348;struct Cyc_Absyn_Exp*_tmp754=bound;Cyc_Absyn_array_typ(_tmp756,_tmp755,_tmp754,
_tmp349 == 0?Cyc_Absyn_false_conref:*_tmp349,0U);});
return _tmp360;};};};}
# 1831
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1834
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp361=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp362=_tmp361;void*_tmp367;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp362)->tag == 19U){_LL1: _tmp367=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp362)->f1;_LL2:
# 1839
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp363=({void*_tmp757=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp757,Cyc_Absyn_valueof_exp(_tmp367,0U),0,Cyc_Absyn_No_coercion,0U);});
_tmp363->topt=bound->topt;
# 1844
bound=_tmp363;}
# 1846
goto _LL0;}else{_LL3: _LL4:
# 1848
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp366;_tmp366.tag=0U;({struct _dyneither_ptr _tmp758=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp366.f1=_tmp758;});({void*_tmp364[1U]={& _tmp366};({unsigned int _tmp75A=bound->loc;struct _dyneither_ptr _tmp759=({const char*_tmp365="expecting unsigned int, found %s";_tag_dyneither(_tmp365,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp75A,_tmp759,_tag_dyneither(_tmp364,sizeof(void*),1U));});});});}_LL0:;}{
# 1853
void**_tmp368=0;
struct Cyc_Absyn_Tqual*_tmp369=0;
union Cyc_Absyn_Constraint**_tmp36A=0;
# 1857
if(topt != 0){
void*_tmp36B=Cyc_Tcutil_compress(*topt);void*_tmp36C=_tmp36B;void*_tmp377;struct Cyc_Absyn_Tqual _tmp376;struct Cyc_Absyn_Exp*_tmp375;union Cyc_Absyn_Constraint*_tmp374;struct Cyc_Absyn_PtrInfo _tmp373;switch(*((int*)_tmp36C)){case 5U: _LL6: _tmp373=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36C)->f1;_LL7:
# 1860
({void**_tmp75B=({void**_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=_tmp373.elt_typ;_tmp36D;});_tmp368=_tmp75B;});
({struct Cyc_Absyn_Tqual*_tmp75C=({struct Cyc_Absyn_Tqual*_tmp36E=_cycalloc_atomic(sizeof(*_tmp36E));_tmp36E[0]=_tmp373.elt_tq;_tmp36E;});_tmp369=_tmp75C;});
({union Cyc_Absyn_Constraint**_tmp75D=({union Cyc_Absyn_Constraint**_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F[0]=(_tmp373.ptr_atts).zero_term;_tmp36F;});_tmp36A=_tmp75D;});
goto _LL5;case 8U: _LL8: _tmp377=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).elt_type;_tmp376=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).tq;_tmp375=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).num_elts;_tmp374=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).zero_term;_LL9:
# 1865
({void**_tmp75E=({void**_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370[0]=_tmp377;_tmp370;});_tmp368=_tmp75E;});
({struct Cyc_Absyn_Tqual*_tmp75F=({struct Cyc_Absyn_Tqual*_tmp371=_cycalloc_atomic(sizeof(*_tmp371));_tmp371[0]=_tmp376;_tmp371;});_tmp369=_tmp75F;});
({union Cyc_Absyn_Constraint**_tmp760=({union Cyc_Absyn_Constraint**_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372[0]=_tmp374;_tmp372;});_tmp36A=_tmp760;});
goto _LL5;default: _LLA: _LLB:
# 1870
 goto _LL5;}_LL5:;}
# 1873
({unsigned int _tmp763=loc;struct Cyc_Tcenv_Tenv*_tmp762=te;struct Cyc_List_List*_tmp761=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp763,_tmp762,_tmp761,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp368 != 0)Cyc_Tcutil_unify(*_tmp368,t);
# 1876
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp378=0U;({unsigned int _tmp765=bound->loc;struct _dyneither_ptr _tmp764=({const char*_tmp379="bound is not constant";_tag_dyneither(_tmp379,sizeof(char),22U);});Cyc_Tcutil_terr(_tmp765,_tmp764,_tag_dyneither(_tmp378,sizeof(void*),0U));});});}
# 1880
if(_tmp36A != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp36A)){
# 1883
struct Cyc_Absyn_Exp*_tmp37A=Cyc_Absyn_uint_exp(1U,0U);_tmp37A->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp766=Cyc_Absyn_add_exp(bound,_tmp37A,0U);bound=_tmp766;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1887
({void*_tmp37B=0U;({unsigned int _tmp768=loc;struct _dyneither_ptr _tmp767=({const char*_tmp37C="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp37C,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp768,_tmp767,_tag_dyneither(_tmp37B,sizeof(void*),0U));});});}{
# 1890
void*_tmp37D=({void*_tmp76B=t;struct Cyc_Absyn_Tqual _tmp76A=_tmp369 == 0?Cyc_Absyn_empty_tqual(0U):*_tmp369;struct Cyc_Absyn_Exp*_tmp769=bound;Cyc_Absyn_array_typ(_tmp76B,_tmp76A,_tmp769,
_tmp36A == 0?Cyc_Absyn_false_conref:*_tmp36A,0U);});
return _tmp37D;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1905 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1909
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp76C=({struct Cyc_Absyn_Aggrdecl**_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=ad;_tmp37E;});adptr=_tmp76C;});}else{
# 1915
{struct _handler_cons _tmp37F;_push_handler(& _tmp37F);{int _tmp381=0;if(setjmp(_tmp37F.handler))_tmp381=1;if(!_tmp381){({struct Cyc_Absyn_Aggrdecl**_tmp76D=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp76D;});
ad=*adptr;
# 1915
;_pop_handler();}else{void*_tmp380=(void*)_exn_thrown;void*_tmp382=_tmp380;void*_tmp386;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp382)->tag == Cyc_Dict_Absent){_LL1: _LL2:
# 1918
({struct Cyc_String_pa_PrintArg_struct _tmp385;_tmp385.tag=0U;({struct _dyneither_ptr _tmp76E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp385.f1=_tmp76E;});({void*_tmp383[1U]={& _tmp385};({unsigned int _tmp770=loc;struct _dyneither_ptr _tmp76F=({const char*_tmp384="unbound struct/union name %s";_tag_dyneither(_tmp384,sizeof(char),29U);});Cyc_Tcutil_terr(_tmp770,_tmp76F,_tag_dyneither(_tmp383,sizeof(void*),1U));});});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL3: _tmp386=_tmp382;_LL4:(int)_rethrow(_tmp386);}_LL0:;}};}
# 1921
*ad_opt=ad;
*tn=ad->name;}
# 1924
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp389;_tmp389.tag=0U;({struct _dyneither_ptr _tmp771=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp38A="struct";_tag_dyneither(_tmp38A,sizeof(char),7U);}):({const char*_tmp38B="union";_tag_dyneither(_tmp38B,sizeof(char),6U);}));_tmp389.f1=_tmp771;});({void*_tmp387[1U]={& _tmp389};({unsigned int _tmp773=loc;struct _dyneither_ptr _tmp772=({const char*_tmp388="can't construct abstract %s";_tag_dyneither(_tmp388,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp773,_tmp772,_tag_dyneither(_tmp387,sizeof(void*),1U));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1932
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp38C=Cyc_Tcenv_new_status(te);
if(_tmp38C == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp774=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp774;});else{
# 1941
({struct Cyc_Tcenv_Tenv*_tmp775=Cyc_Tcenv_set_new_status(_tmp38C,te);te2=_tmp775;});}{
# 1943
struct _tuple12 _tmp38D=({struct _tuple12 _tmp3B4;({struct Cyc_List_List*_tmp776=Cyc_Tcenv_lookup_type_vars(te2);_tmp3B4.f1=_tmp776;});_tmp3B4.f2=Cyc_Core_heap_region;_tmp3B4;});
struct Cyc_List_List*_tmp38E=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp38D,ad->tvs);
struct Cyc_List_List*_tmp38F=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp38D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp390=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp38E);
struct Cyc_List_List*_tmp391=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp38F);
struct Cyc_List_List*_tmp392=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp38E,_tmp38F);
void*res_typ;
# 1954
if(topt != 0){
void*_tmp393=Cyc_Tcutil_compress(*topt);void*_tmp394=_tmp393;struct Cyc_Absyn_Aggrdecl**_tmp39A;struct Cyc_List_List*_tmp399;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp394)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp394)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp39A=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp394)->f1).aggr_info).KnownAggr).val;_tmp399=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp394)->f1).targs;_LL7:
# 1957
 if(*_tmp39A == *adptr){
{struct Cyc_List_List*_tmp395=_tmp390;for(0;_tmp395 != 0  && _tmp399 != 0;
(_tmp395=_tmp395->tl,_tmp399=_tmp399->tl)){
Cyc_Tcutil_unify((void*)_tmp395->hd,(void*)_tmp399->hd);}}
# 1962
res_typ=*topt;
goto _LL5;}
# 1965
goto _LL9;}else{goto _LL8;}}else{_LL8: _LL9:
# 1967
({void*_tmp77A=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp396=_cycalloc(sizeof(*_tmp396));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp779=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp397;_tmp397.tag=11U;({struct Cyc_Absyn_AggrInfo _tmp778=({struct Cyc_Absyn_AggrInfo _tmp398;({union Cyc_Absyn_AggrInfoU _tmp777=Cyc_Absyn_KnownAggr(adptr);_tmp398.aggr_info=_tmp777;});_tmp398.targs=_tmp390;_tmp398;});_tmp397.f1=_tmp778;});_tmp397;});_tmp396[0]=_tmp779;});_tmp396;});res_typ=_tmp77A;});}_LL5:;}else{
# 1970
({void*_tmp77E=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp39B=_cycalloc(sizeof(*_tmp39B));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp77D=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp39C;_tmp39C.tag=11U;({struct Cyc_Absyn_AggrInfo _tmp77C=({struct Cyc_Absyn_AggrInfo _tmp39D;({union Cyc_Absyn_AggrInfoU _tmp77B=Cyc_Absyn_KnownAggr(adptr);_tmp39D.aggr_info=_tmp77B;});_tmp39D.targs=_tmp390;_tmp39D;});_tmp39C.f1=_tmp77C;});_tmp39C;});_tmp39B[0]=_tmp77D;});_tmp39B;});res_typ=_tmp77E;});}{
# 1973
struct Cyc_List_List*_tmp39E=*ts;
struct Cyc_List_List*_tmp39F=_tmp391;
while(_tmp39E != 0  && _tmp39F != 0){
# 1977
({unsigned int _tmp781=loc;struct Cyc_Tcenv_Tenv*_tmp780=te2;struct Cyc_List_List*_tmp77F=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp781,_tmp780,_tmp77F,& Cyc_Tcutil_ak,1,0,(void*)_tmp39E->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp39E->hd);
Cyc_Tcutil_unify((void*)_tmp39E->hd,(void*)_tmp39F->hd);
_tmp39E=_tmp39E->tl;
_tmp39F=_tmp39F->tl;}
# 1983
if(_tmp39E != 0)
({void*_tmp3A0=0U;({unsigned int _tmp783=loc;struct _dyneither_ptr _tmp782=({const char*_tmp3A1="too many explicit witness types";_tag_dyneither(_tmp3A1,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp783,_tmp782,_tag_dyneither(_tmp3A0,sizeof(void*),0U));});});
# 1986
*ts=_tmp391;{
# 1989
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3A2=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3A3=_tmp3A2;struct Cyc_Absyn_Aggrfield*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3AE;_LLB: _tmp3AF=_tmp3A3->f1;_tmp3AE=_tmp3A3->f2;_LLC:;{
void*_tmp3A4=Cyc_Tcutil_substitute(_tmp392,_tmp3AF->type);
({struct Cyc_Tcenv_Tenv*_tmp784=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp784,& _tmp3A4,_tmp3AE);});
# 2000
if(!Cyc_Tcutil_coerce_arg(te2,_tmp3AE,_tmp3A4,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0U;({struct _dyneither_ptr _tmp785=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2004
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3AE->topt)));_tmp3AB.f1=_tmp785;});({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0U;({struct _dyneither_ptr _tmp786=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2003
Cyc_Absynpp_typ2string(_tmp3A4));_tmp3AA.f1=_tmp786;});({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0U;({struct _dyneither_ptr _tmp787=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp3A9.f1=_tmp787;});({struct Cyc_String_pa_PrintArg_struct _tmp3A8;_tmp3A8.tag=0U;({struct _dyneither_ptr _tmp788=(struct _dyneither_ptr)(
# 2002
ad->kind == Cyc_Absyn_StructA?({const char*_tmp3AC="struct";_tag_dyneither(_tmp3AC,sizeof(char),7U);}):({const char*_tmp3AD="union";_tag_dyneither(_tmp3AD,sizeof(char),6U);}));_tmp3A8.f1=_tmp788;});({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0U;_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3AF->name);({void*_tmp3A5[5U]={& _tmp3A7,& _tmp3A8,& _tmp3A9,& _tmp3AA,& _tmp3AB};({unsigned int _tmp78A=_tmp3AE->loc;struct _dyneither_ptr _tmp789=({const char*_tmp3A6="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp3A6,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp78A,_tmp789,_tag_dyneither(_tmp3A5,sizeof(void*),5U));});});});});});});});
# 2005
Cyc_Tcutil_explain_failure();}};}{
# 2009
struct Cyc_List_List*_tmp3B0=0;
{struct Cyc_List_List*_tmp3B1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp3B1 != 0;_tmp3B1=_tmp3B1->tl){
({struct Cyc_List_List*_tmp78E=({struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({struct _tuple0*_tmp78D=({struct _tuple0*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));({void*_tmp78B=Cyc_Tcutil_substitute(_tmp392,(*((struct _tuple0*)_tmp3B1->hd)).f1);_tmp3B3->f1=_tmp78B;});({void*_tmp78C=
Cyc_Tcutil_substitute(_tmp392,(*((struct _tuple0*)_tmp3B1->hd)).f2);_tmp3B3->f2=_tmp78C;});_tmp3B3;});_tmp3B2->hd=_tmp78D;});_tmp3B2->tl=_tmp3B0;_tmp3B2;});
# 2011
_tmp3B0=_tmp78E;});}}
# 2014
({struct Cyc_List_List*_tmp78F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3B0);_tmp3B0=_tmp78F;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp3B0);
return res_typ;};};};};};}
# 2020
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2022
{void*_tmp3B5=Cyc_Tcutil_compress(ts);void*_tmp3B6=_tmp3B5;enum Cyc_Absyn_AggrKind _tmp3C5;struct Cyc_List_List*_tmp3C4;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B6)->tag == 12U){_LL1: _tmp3C5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B6)->f1;_tmp3C4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B6)->f2;_LL2:
# 2024
 if(_tmp3C5 == Cyc_Absyn_UnionA)
({void*_tmp3B7=0U;({unsigned int _tmp791=loc;struct _dyneither_ptr _tmp790=({const char*_tmp3B8="expecting struct but found union";_tag_dyneither(_tmp3B8,sizeof(char),33U);});Cyc_Tcutil_terr(_tmp791,_tmp790,_tag_dyneither(_tmp3B7,sizeof(void*),0U));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp3C4);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3B9=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3BA=_tmp3B9;struct Cyc_Absyn_Aggrfield*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C0;_LL6: _tmp3C1=_tmp3BA->f1;_tmp3C0=_tmp3BA->f2;_LL7:;
({struct Cyc_Tcenv_Tenv*_tmp793=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp792=& _tmp3C1->type;Cyc_Tcexp_tcExpInitializer(_tmp793,_tmp792,_tmp3C0);});
# 2033
if(!Cyc_Tcutil_coerce_arg(te,_tmp3C0,_tmp3C1->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0U;({struct _dyneither_ptr _tmp794=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2036
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3C0->topt)));_tmp3BF.f1=_tmp794;});({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0U;({struct _dyneither_ptr _tmp795=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2035
Cyc_Absynpp_typ2string(_tmp3C1->type));_tmp3BE.f1=_tmp795;});({struct Cyc_String_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=0U;_tmp3BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3C1->name);({void*_tmp3BB[3U]={& _tmp3BD,& _tmp3BE,& _tmp3BF};({unsigned int _tmp797=_tmp3C0->loc;struct _dyneither_ptr _tmp796=({const char*_tmp3BC="field %s of struct expects type %s != %s";_tag_dyneither(_tmp3BC,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp797,_tmp796,_tag_dyneither(_tmp3BB,sizeof(void*),3U));});});});});});
# 2037
Cyc_Tcutil_explain_failure();}}
# 2040
goto _LL0;};}else{_LL3: _LL4:
({void*_tmp3C2=0U;({struct _dyneither_ptr _tmp798=({const char*_tmp3C3="tcAnonStruct: wrong type";_tag_dyneither(_tmp3C3,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp798,_tag_dyneither(_tmp3C2,sizeof(void*),0U));});});}_LL0:;}
# 2043
return ts;}
# 2047
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2050
struct _tuple12 _tmp3C6=({struct _tuple12 _tmp3DA;({struct Cyc_List_List*_tmp799=Cyc_Tcenv_lookup_type_vars(te);_tmp3DA.f1=_tmp799;});_tmp3DA.f2=Cyc_Core_heap_region;_tmp3DA;});
struct Cyc_List_List*_tmp3C7=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp3C6,tud->tvs);
struct Cyc_List_List*_tmp3C8=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3C7);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp79C=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp3D8;_tmp3D8.tag=4U;({struct Cyc_Absyn_DatatypeFieldInfo _tmp79B=({struct Cyc_Absyn_DatatypeFieldInfo _tmp3D9;({union Cyc_Absyn_DatatypeFieldInfoU _tmp79A=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp3D9.field_info=_tmp79A;});_tmp3D9.targs=_tmp3C8;_tmp3D9;});_tmp3D8.f1=_tmp79B;});_tmp3D8;});_tmp3D7[0]=_tmp79C;});_tmp3D7;});
# 2057
if(topt != 0){
void*_tmp3C9=Cyc_Tcutil_compress(*topt);void*_tmp3CA=_tmp3C9;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3CA)->tag == 4U){_LL1: _LL2:
 Cyc_Tcutil_unify(*topt,res);goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}{
# 2063
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp3C7 != 0)({void*_tmp79D=Cyc_Tcutil_substitute(_tmp3C7,t);t=_tmp79D;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0U;({struct _dyneither_ptr _tmp79E=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2074
e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3D0="?";_tag_dyneither(_tmp3D0,sizeof(char),2U);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp3CF.f1=_tmp79E;});({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0U;({struct _dyneither_ptr _tmp79F=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2073
Cyc_Absynpp_typ2string(t));_tmp3CE.f1=_tmp79F;});({struct Cyc_String_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=0U;({struct _dyneither_ptr _tmp7A0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp3CD.f1=_tmp7A0;});({void*_tmp3CB[3U]={& _tmp3CD,& _tmp3CE,& _tmp3CF};({unsigned int _tmp7A2=e->loc;struct _dyneither_ptr _tmp7A1=({const char*_tmp3CC="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3CC,sizeof(char),82U);});Cyc_Tcutil_terr(_tmp7A2,_tmp7A1,_tag_dyneither(_tmp3CB,sizeof(void*),3U));});});});});});
# 2075
Cyc_Tcutil_explain_failure();}}
# 2078
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0U;({struct _dyneither_ptr _tmp7A3=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2081
Cyc_Absynpp_qvar2string(tuf->name));_tmp3D3.f1=_tmp7A3;});({void*_tmp3D1[1U]={& _tmp3D3};({struct Cyc_Tcenv_Tenv*_tmp7A7=te;unsigned int _tmp7A6=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7A5=topt;struct _dyneither_ptr _tmp7A4=({const char*_tmp3D2="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3D2,sizeof(char),47U);});Cyc_Tcexp_expr_err(_tmp7A7,_tmp7A6,_tmp7A5,_tmp7A4,_tag_dyneither(_tmp3D1,sizeof(void*),1U));});});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=0U;({struct _dyneither_ptr _tmp7A8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));_tmp3D6.f1=_tmp7A8;});({void*_tmp3D4[1U]={& _tmp3D6};({struct Cyc_Tcenv_Tenv*_tmp7AC=te;unsigned int _tmp7AB=loc;void**_tmp7AA=topt;struct _dyneither_ptr _tmp7A9=({const char*_tmp3D5="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3D5,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7AC,_tmp7AB,_tmp7AA,_tmp7A9,_tag_dyneither(_tmp3D4,sizeof(void*),1U));});});});
return res;};}
# 2088
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2090
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2092
if(topt != 0){
void*_tmp3DB=Cyc_Tcutil_compress(*topt);void*_tmp3DC=_tmp3DB;void*_tmp3DD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DC)->tag == 5U){_LL1: _tmp3DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DC)->f1).elt_typ;_LL2:
# 2095
 Cyc_Tcutil_unify(_tmp3DD,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2101
({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0U;({struct _dyneither_ptr _tmp7AD=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp3E1.f1=_tmp7AD;});({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0U;({struct _dyneither_ptr _tmp7AE=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3E2="calloc";_tag_dyneither(_tmp3E2,sizeof(char),7U);}):(struct _dyneither_ptr)({const char*_tmp3E3="malloc";_tag_dyneither(_tmp3E3,sizeof(char),7U);})));_tmp3E0.f1=_tmp7AE;});({void*_tmp3DE[2U]={& _tmp3E0,& _tmp3E1};({unsigned int _tmp7B0=loc;struct _dyneither_ptr _tmp7AF=({const char*_tmp3DF="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3DF,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp7B0,_tmp7AF,_tag_dyneither(_tmp3DE,sizeof(void*),2U));});});});});}
# 2105
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2107
enum Cyc_Absyn_AliasQual _tmp3E4=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3E5=_tmp3E4;if(_tmp3E5 == Cyc_Absyn_Unique){_LL1: _LL2:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL3: _LL4:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL0:;}
# 2113
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2118
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2121
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp7B2=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp3ED;_tmp3ED.tag=15U;({void*_tmp7B1=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3ED.f1=_tmp7B1;});_tmp3ED;});_tmp3EC[0]=_tmp7B2;});_tmp3EC;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3E6=Cyc_Tcutil_compress(handle_type);void*_tmp3E7=_tmp3E6;void*_tmp3EB;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E7)->tag == 15U){_LL1: _tmp3EB=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E7)->f1;_LL2:
# 2126
 rgn=_tmp3EB;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2130
({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0U;({struct _dyneither_ptr _tmp7B3=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));_tmp3EA.f1=_tmp7B3;});({void*_tmp3E8[1U]={& _tmp3EA};({unsigned int _tmp7B5=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7B4=({const char*_tmp3E9="expecting region_t type but found %s";_tag_dyneither(_tmp3E9,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7B5,_tmp7B4,_tag_dyneither(_tmp3E8,sizeof(void*),1U));});});});
goto _LL0;}_LL0:;}else{
# 2137
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7B6=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7B6;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2144
({struct Cyc_Tcenv_Tenv*_tmp7B7=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7B7,& Cyc_Absyn_uint_typ,*e);});{
# 2153 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3EE=0U;({struct _dyneither_ptr _tmp7B8=({const char*_tmp3EF="calloc with empty type";_tag_dyneither(_tmp3EF,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7B8,_tag_dyneither(_tmp3EE,sizeof(void*),0U));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7BB=loc;struct Cyc_Tcenv_Tenv*_tmp7BA=te;struct Cyc_List_List*_tmp7B9=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7BB,_tmp7BA,_tmp7B9,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2165
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3F0=er;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp405;void*_tmp404;switch(*((int*)_tmp3F0)){case 17U: _LL6: _tmp404=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F0)->f1;_LL7:
# 2169
 elt_type=_tmp404;
({void**_tmp7BC=({void**_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1[0]=elt_type;_tmp3F1;});*t=_tmp7BC;});
({struct Cyc_Absyn_Exp*_tmp7BD=Cyc_Absyn_uint_exp(1U,0U);num_elts=_tmp7BD;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL5;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2)->tl)->tl == 0){_LL8: _tmp406=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2)->hd;_tmp405=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2)->tl)->hd;_LL9:
# 2176
{struct _tuple0 _tmp3F2=({struct _tuple0 _tmp3F8;_tmp3F8.f1=_tmp406->r;_tmp3F8.f2=_tmp405->r;_tmp3F8;});struct _tuple0 _tmp3F3=_tmp3F2;void*_tmp3F7;void*_tmp3F6;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F3.f1)->tag == 17U){_LLD: _tmp3F6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F3.f1)->f1;_LLE:
# 2178
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3F6);
elt_type=_tmp3F6;
({void**_tmp7BE=({void**_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4[0]=elt_type;_tmp3F4;});*t=_tmp7BE;});
num_elts=_tmp405;
one_elt=0;
goto _LLC;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F3.f2)->tag == 17U){_LLF: _tmp3F7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F3.f2)->f1;_LL10:
# 2185
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3F7);
elt_type=_tmp3F7;
({void**_tmp7BF=({void**_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5[0]=elt_type;_tmp3F5;});*t=_tmp7BF;});
num_elts=_tmp406;
one_elt=0;
goto _LLC;}else{_LL11: _LL12:
 goto No_sizeof;}}_LLC:;}
# 2193
goto _LL5;}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
# 2195
 No_sizeof: {
# 2198
struct Cyc_Absyn_Exp*_tmp3F9=*e;
{void*_tmp3FA=_tmp3F9->r;void*_tmp3FB=_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FC;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FB)->tag == 14U){_LL14: _tmp3FC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FB)->f2;_LL15:
 _tmp3F9=_tmp3FC;goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 2203
{void*_tmp3FD=Cyc_Tcutil_compress((void*)_check_null(_tmp3F9->topt));void*_tmp3FE=_tmp3FD;void*_tmp402;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3FE)->tag == 19U){_LL19: _tmp402=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3FE)->f1;_LL1A:
# 2205
{void*_tmp3FF=Cyc_Tcutil_compress(_tmp402);void*_tmp400=_tmp3FF;struct Cyc_Absyn_Exp*_tmp401;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp400)->tag == 18U){_LL1E: _tmp401=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp400)->f1;_LL1F:
# 2207
 er=_tmp401->r;goto retry_sizeof;}else{_LL20: _LL21:
 goto _LL1D;}_LL1D:;}
# 2210
goto _LL18;}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2213
elt_type=Cyc_Absyn_char_typ;
({void**_tmp7C0=({void**_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403[0]=elt_type;_tmp403;});*t=_tmp7C0;});
num_elts=*e;
one_elt=0;}
# 2218
goto _LL5;}_LL5:;}}
# 2222
*is_fat=!one_elt;
# 2225
{void*_tmp407=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp40A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp407)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp407)->f1).aggr_info).KnownAggr).tag == 2){_LL23: _tmp40A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp407)->f1).aggr_info).KnownAggr).val;_LL24:
# 2227
 if(_tmp40A->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp40A->impl))->exist_vars != 0)
({void*_tmp408=0U;({unsigned int _tmp7C2=loc;struct _dyneither_ptr _tmp7C1=({const char*_tmp409="malloc with existential types not yet implemented";_tag_dyneither(_tmp409,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp7C2,_tmp7C1,_tag_dyneither(_tmp408,sizeof(void*),0U));});});
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}{
# 2234
void*(*_tmp40B)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp40C=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp40D=Cyc_Tcutil_compress(*topt);void*_tmp40E=_tmp40D;union Cyc_Absyn_Constraint*_tmp41B;union Cyc_Absyn_Constraint*_tmp41A;union Cyc_Absyn_Constraint*_tmp419;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->tag == 5U){_LL28: _tmp41B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->f1).ptr_atts).nullable;_tmp41A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->f1).ptr_atts).bounds;_tmp419=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->f1).ptr_atts).zero_term;_LL29:
# 2239
 _tmp40C=_tmp419;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp41B))
_tmp40B=Cyc_Absyn_star_typ;
# 2244
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp419) && !(*is_calloc)){
({void*_tmp40F=0U;({unsigned int _tmp7C4=loc;struct _dyneither_ptr _tmp7C3=({const char*_tmp410="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp410,sizeof(char),55U);});Cyc_Tcutil_warn(_tmp7C4,_tmp7C3,_tag_dyneither(_tmp40F,sizeof(void*),0U));});});
*is_calloc=1;}
# 2250
{void*_tmp411=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp41A);void*_tmp412=_tmp411;struct Cyc_Absyn_Exp*_tmp418;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp412)->tag == 0U){_LL2D: _LL2E:
 goto _LL2C;}else{_LL2F: _tmp418=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp412)->f1;if(!one_elt){_LL30: {
# 2253
int _tmp413=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp413  && Cyc_Evexp_same_const_exp(_tmp418,num_elts)){
*is_fat=0;
return({void*_tmp7C8=elt_type;void*_tmp7C7=rgn;struct Cyc_Absyn_Tqual _tmp7C6=Cyc_Absyn_empty_tqual(0U);void*_tmp7C5=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp41A);Cyc_Absyn_atb_typ(_tmp7C8,_tmp7C7,_tmp7C6,_tmp7C5,_tmp40C);});}
# 2259
{void*_tmp414=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp415=_tmp414;void*_tmp417;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp415)->tag == 19U){_LL34: _tmp417=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp415)->f1;_LL35: {
# 2261
struct Cyc_Absyn_Exp*_tmp416=({void*_tmp7C9=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp7C9,Cyc_Absyn_valueof_exp(_tmp417,0U),0,Cyc_Absyn_No_coercion,0U);});
# 2263
if(Cyc_Evexp_same_const_exp(_tmp416,_tmp418)){
*is_fat=0;
return({void*_tmp7CD=elt_type;void*_tmp7CC=rgn;struct Cyc_Absyn_Tqual _tmp7CB=Cyc_Absyn_empty_tqual(0U);void*_tmp7CA=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp41A);Cyc_Absyn_atb_typ(_tmp7CD,_tmp7CC,_tmp7CB,_tmp7CA,_tmp40C);});}
# 2268
goto _LL33;}}else{_LL36: _LL37:
 goto _LL33;}_LL33:;}
# 2271
goto _LL2C;}}else{_LL31: _LL32:
 goto _LL2C;}}_LL2C:;}
# 2274
goto _LL27;}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2277
if(!one_elt)_tmp40B=Cyc_Absyn_dyneither_typ;
return({void*(*_tmp7D1)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=_tmp40B;void*_tmp7D0=elt_type;void*_tmp7CF=rgn;struct Cyc_Absyn_Tqual _tmp7CE=Cyc_Absyn_empty_tqual(0U);_tmp7D1(_tmp7D0,_tmp7CF,_tmp7CE,_tmp40C);});};};}
# 2282
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2284
struct Cyc_Tcenv_Tenv*_tmp41C=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp41C,0,e1);{
void*_tmp41D=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp41C,& _tmp41D,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2291
{void*_tmp41E=Cyc_Tcutil_compress(t1);void*_tmp41F=_tmp41E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp41F)->tag == 8U){_LL1: _LL2:
({void*_tmp420=0U;({unsigned int _tmp7D3=loc;struct _dyneither_ptr _tmp7D2=({const char*_tmp421="cannot assign to an array";_tag_dyneither(_tmp421,sizeof(char),26U);});Cyc_Tcutil_terr(_tmp7D3,_tmp7D2,_tag_dyneither(_tmp420,sizeof(void*),0U));});});goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2296
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp422=0U;({unsigned int _tmp7D5=loc;struct _dyneither_ptr _tmp7D4=({const char*_tmp423="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp423,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp7D5,_tmp7D4,_tag_dyneither(_tmp422,sizeof(void*),0U));});});
# 2300
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp424=0U;({struct Cyc_Tcenv_Tenv*_tmp7D9=te;unsigned int _tmp7D8=e1->loc;void**_tmp7D7=topt;struct _dyneither_ptr _tmp7D6=({const char*_tmp425="swap non-lvalue";_tag_dyneither(_tmp425,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7D9,_tmp7D8,_tmp7D7,_tmp7D6,_tag_dyneither(_tmp424,sizeof(void*),0U));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp426=0U;({struct Cyc_Tcenv_Tenv*_tmp7DD=te;unsigned int _tmp7DC=e2->loc;void**_tmp7DB=topt;struct _dyneither_ptr _tmp7DA=({const char*_tmp427="swap non-lvalue";_tag_dyneither(_tmp427,sizeof(char),16U);});Cyc_Tcexp_expr_err(_tmp7DD,_tmp7DC,_tmp7DB,_tmp7DA,_tag_dyneither(_tmp426,sizeof(void*),0U));});});{
# 2305
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp428=0U;({struct Cyc_Tcenv_Tenv*_tmp7E1=te;unsigned int _tmp7E0=e1->loc;void**_tmp7DF=topt;struct _dyneither_ptr _tmp7DE=({const char*_tmp429="swap value in zeroterm array";_tag_dyneither(_tmp429,sizeof(char),29U);});Cyc_Tcexp_expr_err(_tmp7E1,_tmp7E0,_tmp7DF,_tmp7DE,_tag_dyneither(_tmp428,sizeof(void*),0U));});});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp42A=0U;({struct Cyc_Tcenv_Tenv*_tmp7E5=te;unsigned int _tmp7E4=e2->loc;void**_tmp7E3=topt;struct _dyneither_ptr _tmp7E2=({const char*_tmp42B="swap value in zeroterm array";_tag_dyneither(_tmp42B,sizeof(char),29U);});Cyc_Tcexp_expr_err(_tmp7E5,_tmp7E4,_tmp7E3,_tmp7E2,_tag_dyneither(_tmp42A,sizeof(void*),0U));});});
# 2312
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp42C=({struct Cyc_String_pa_PrintArg_struct _tmp430;_tmp430.tag=0U;({struct _dyneither_ptr _tmp7E6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp430.f1=_tmp7E6;});({struct Cyc_String_pa_PrintArg_struct _tmp42F;_tmp42F.tag=0U;({struct _dyneither_ptr _tmp7E7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp42F.f1=_tmp7E7;});({void*_tmp42D[2U]={& _tmp42F,& _tmp430};({struct Cyc_Tcenv_Tenv*_tmp7EB=te;unsigned int _tmp7EA=loc;void**_tmp7E9=topt;struct _dyneither_ptr _tmp7E8=({const char*_tmp42E="type mismatch: %s != %s";_tag_dyneither(_tmp42E,sizeof(char),24U);});Cyc_Tcexp_expr_err(_tmp7EB,_tmp7EA,_tmp7E9,_tmp7E8,_tag_dyneither(_tmp42D,sizeof(void*),2U));});});});});
return _tmp42C;}
# 2319
return(void*)& Cyc_Absyn_VoidType_val;};};};}
# 2324
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp7EC=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp7EC,s,1);});
# 2327
while(1){
void*_tmp431=s->r;void*_tmp432=_tmp431;struct Cyc_Absyn_Decl*_tmp43D;struct Cyc_Absyn_Stmt*_tmp43C;struct Cyc_Absyn_Stmt*_tmp43B;struct Cyc_Absyn_Stmt*_tmp43A;struct Cyc_Absyn_Exp*_tmp439;switch(*((int*)_tmp432)){case 1U: _LL1: _tmp439=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp432)->f1;_LL2: {
# 2331
void*_tmp433=(void*)_check_null(_tmp439->topt);
if(!({void*_tmp7ED=_tmp433;Cyc_Tcutil_unify(_tmp7ED,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp436;_tmp436.tag=0U;({struct _dyneither_ptr _tmp7EE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp433));_tmp436.f1=_tmp7EE;});({void*_tmp434[1U]={& _tmp436};({unsigned int _tmp7F0=loc;struct _dyneither_ptr _tmp7EF=({const char*_tmp435="statement expression returns type %s";_tag_dyneither(_tmp435,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7F0,_tmp7EF,_tag_dyneither(_tmp434,sizeof(void*),1U));});});});
Cyc_Tcutil_explain_failure();}
# 2337
return _tmp433;}case 2U: _LL3: _tmp43B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp432)->f1;_tmp43A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp432)->f2;_LL4:
 s=_tmp43A;continue;case 12U: _LL5: _tmp43D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp432)->f1;_tmp43C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp432)->f2;_LL6:
 s=_tmp43C;continue;default: _LL7: _LL8:
# 2341
 return({void*_tmp437=0U;({struct Cyc_Tcenv_Tenv*_tmp7F4=te;unsigned int _tmp7F3=loc;void**_tmp7F2=topt;struct _dyneither_ptr _tmp7F1=({const char*_tmp438="statement expression must end with expression";_tag_dyneither(_tmp438,sizeof(char),46U);});Cyc_Tcexp_expr_err(_tmp7F4,_tmp7F3,_tmp7F2,_tmp7F1,_tag_dyneither(_tmp437,sizeof(void*),0U));});});}_LL0:;}}
# 2346
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp7F5=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F5,0,e);}));
{void*_tmp43E=t;struct Cyc_Absyn_Aggrdecl*_tmp442;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43E)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43E)->f1).aggr_info).KnownAggr).tag == 2){_LL1: _tmp442=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43E)->f1).aggr_info).KnownAggr).val;_LL2:
# 2350
 if((_tmp442->kind == Cyc_Absyn_UnionA  && _tmp442->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp442->impl))->tagged)goto _LL0;
goto _LL4;}else{goto _LL3;}}else{_LL3: _LL4:
# 2353
({struct Cyc_String_pa_PrintArg_struct _tmp441;_tmp441.tag=0U;({struct _dyneither_ptr _tmp7F6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp441.f1=_tmp7F6;});({void*_tmp43F[1U]={& _tmp441};({unsigned int _tmp7F8=loc;struct _dyneither_ptr _tmp7F7=({const char*_tmp440="expecting @tagged union but found %s";_tag_dyneither(_tmp440,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7F8,_tmp7F7,_tag_dyneither(_tmp43F,sizeof(void*),1U));});});});
goto _LL0;}_LL0:;}
# 2356
return Cyc_Absyn_uint_typ;}
# 2360
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2364
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct Cyc_Tcenv_Tenv*_tmp443=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2368
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp7FA=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp44B;_tmp44B.tag=15U;({void*_tmp7F9=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp443));_tmp44B.f1=_tmp7F9;});_tmp44B;});_tmp44A[0]=_tmp7FA;});_tmp44A;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp443,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp444=Cyc_Tcutil_compress(handle_type);void*_tmp445=_tmp444;void*_tmp449;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp445)->tag == 15U){_LL1: _tmp449=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp445)->f1;_LL2:
# 2373
 rgn=_tmp449;
Cyc_Tcenv_check_rgn_accessible(_tmp443,loc,rgn);
goto _LL0;}else{_LL3: _LL4:
# 2377
({struct Cyc_String_pa_PrintArg_struct _tmp448;_tmp448.tag=0U;({struct _dyneither_ptr _tmp7FB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));_tmp448.f1=_tmp7FB;});({void*_tmp446[1U]={& _tmp448};({unsigned int _tmp7FD=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp7FC=({const char*_tmp447="expecting region_t type but found %s";_tag_dyneither(_tmp447,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp7FD,_tmp7FC,_tag_dyneither(_tmp446,sizeof(void*),1U));});});});
goto _LL0;}_LL0:;}else{
# 2384
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7FE=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7FE;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2392
void*_tmp44C=e1->r;void*_tmp44D=_tmp44C;struct Cyc_List_List*_tmp46C;struct Cyc_Core_Opt*_tmp46B;struct Cyc_List_List*_tmp46A;switch(*((int*)_tmp44D)){case 27U: _LL6: _LL7:
 goto _LL9;case 28U: _LL8: _LL9: {
# 2397
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp443,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp44E=0U;({struct _dyneither_ptr _tmp7FF=({const char*_tmp44F="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp44F,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7FF,_tag_dyneither(_tmp44E,sizeof(void*),0U));});});
({void*_tmp800=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp800;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp443,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp443,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2408
return res_typ;}case 36U: _LLA: _tmp46B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp44D)->f1;_tmp46A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp44D)->f2;_LLB:
# 2410
({void*_tmp802=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp450=_cycalloc(sizeof(*_tmp450));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp801=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp451;_tmp451.tag=26U;_tmp451.f1=_tmp46A;_tmp451;});_tmp450[0]=_tmp801;});_tmp450;});e1->r=_tmp802;});
_tmp46C=_tmp46A;goto _LLD;case 26U: _LLC: _tmp46C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp44D)->f1;_LLD: {
# 2413
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp452=Cyc_Tcutil_compress(*topt);void*_tmp453=_tmp452;void**_tmp456;struct Cyc_Absyn_Tqual _tmp455;union Cyc_Absyn_Constraint*_tmp454;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp453)->tag == 5U){_LL15: _tmp456=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp453)->f1).elt_typ;_tmp455=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp453)->f1).elt_tq;_tmp454=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp453)->f1).ptr_atts).zero_term;_LL16:
# 2419
 elt_typ_opt=_tmp456;
({int _tmp803=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp454);zero_term=_tmp803;});
goto _LL14;}else{_LL17: _LL18:
 goto _LL14;}_LL14:;}{
# 2425
void*res_typ=Cyc_Tcexp_tcArray(_tmp443,e1->loc,elt_typ_opt,0,zero_term,_tmp46C);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp457=0U;({struct _dyneither_ptr _tmp804=({const char*_tmp458="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp458,sizeof(char),50U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp804,_tag_dyneither(_tmp457,sizeof(void*),0U));});});
({void*_tmp805=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp805;});
if(topt != 0){
# 2434
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp443,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp443,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2440
return res_typ;};}case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp44D)->f1).Wstring_c).tag){case 8U: _LLE: _LLF: {
# 2445
void*_tmp459=({void*_tmp808=Cyc_Absyn_char_typ;void*_tmp807=rgn;struct Cyc_Absyn_Tqual _tmp806=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp808,_tmp807,_tmp806,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2447
void*_tmp45A=Cyc_Tcexp_tcExp(_tmp443,& _tmp459,e1);
return({void*_tmp80E=_tmp45A;void*_tmp80D=rgn;struct Cyc_Absyn_Tqual _tmp80C=Cyc_Absyn_empty_tqual(0U);void*_tmp80B=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp80A=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp45C;_tmp45C.tag=1U;({struct Cyc_Absyn_Exp*_tmp809=Cyc_Absyn_uint_exp(1U,0U);_tmp45C.f1=_tmp809;});_tmp45C;});_tmp45B[0]=_tmp80A;});_tmp45B;});Cyc_Absyn_atb_typ(_tmp80E,_tmp80D,_tmp80C,_tmp80B,Cyc_Absyn_false_conref);});}case 9U: _LL10: _LL11: {
# 2452
void*_tmp45D=({void*_tmp811=Cyc_Absyn_wchar_typ();void*_tmp810=rgn;struct Cyc_Absyn_Tqual _tmp80F=Cyc_Absyn_const_tqual(0U);Cyc_Absyn_atb_typ(_tmp811,_tmp810,_tmp80F,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2454
void*_tmp45E=Cyc_Tcexp_tcExp(_tmp443,& _tmp45D,e1);
return({void*_tmp817=_tmp45E;void*_tmp816=rgn;struct Cyc_Absyn_Tqual _tmp815=Cyc_Absyn_empty_tqual(0U);void*_tmp814=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp813=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp460;_tmp460.tag=1U;({struct Cyc_Absyn_Exp*_tmp812=Cyc_Absyn_uint_exp(1U,0U);_tmp460.f1=_tmp812;});_tmp460;});_tmp45F[0]=_tmp813;});_tmp45F;});Cyc_Absyn_atb_typ(_tmp817,_tmp816,_tmp815,_tmp814,Cyc_Absyn_false_conref);});}default: goto _LL12;}default: _LL12: _LL13:
# 2461
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp461=Cyc_Tcutil_compress(*topt);void*_tmp462=_tmp461;void**_tmp464;struct Cyc_Absyn_Tqual _tmp463;switch(*((int*)_tmp462)){case 5U: _LL1A: _tmp464=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp462)->f1).elt_typ;_tmp463=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp462)->f1).elt_tq;_LL1B:
# 2467
 topt2=_tmp464;goto _LL19;case 3U: _LL1C: _LL1D:
# 2471
 bogus=*topt;
topt2=& bogus;
goto _LL19;default: _LL1E: _LL1F:
 goto _LL19;}_LL19:;}{
# 2477
void*telt=Cyc_Tcexp_tcExp(_tmp443,topt2,e1);
# 2479
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp465=0U;({unsigned int _tmp819=e1->loc;struct _dyneither_ptr _tmp818=({const char*_tmp466="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp466,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp819,_tmp818,_tag_dyneither(_tmp465,sizeof(void*),0U));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp81E=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp468;_tmp468.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp81D=({struct Cyc_Absyn_PtrInfo _tmp469;_tmp469.elt_typ=telt;({struct Cyc_Absyn_Tqual _tmp81A=
Cyc_Absyn_empty_tqual(0U);_tmp469.elt_tq=_tmp81A;});({struct Cyc_Absyn_PtrAtts _tmp81C=({(_tmp469.ptr_atts).rgn=rgn;({union Cyc_Absyn_Constraint*_tmp81B=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp469.ptr_atts).nullable=_tmp81B;});(_tmp469.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp469.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp469.ptr_atts).ptrloc=0;_tmp469.ptr_atts;});_tmp469.ptr_atts=_tmp81C;});_tmp469;});_tmp468.f1=_tmp81D;});_tmp468;});_tmp467[0]=_tmp81E;});_tmp467;});
# 2486
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp443,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp443,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2493
return res_typ;};};}}_LL5:;};}
# 2499
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
({void*_tmp821=({void*_tmp820=({void*_tmp81F=t;Cyc_Tcutil_promote_array(_tmp81F,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp820;});e->topt=_tmp821;});
return t;}
# 2509
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2513
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2518
({void*_tmp46D=0U;({unsigned int _tmp823=e->loc;struct _dyneither_ptr _tmp822=({const char*_tmp46E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp46E,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp823,_tmp822,_tag_dyneither(_tmp46D,sizeof(void*),0U));});});{
# 2520
void*_tmp46F=e->r;void*_tmp470=_tmp46F;switch(*((int*)_tmp470)){case 26U: _LL1: _LL2:
 goto _LL4;case 27U: _LL3: _LL4:
 goto _LL6;case 28U: _LL5: _LL6:
 goto _LL8;case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp470)->f1).String_c).tag){case 9U: _LL7: _LL8:
 goto _LLA;case 8U: _LL9: _LLA:
 return t;default: goto _LLB;}default: _LLB: _LLC:
# 2527
({void*_tmp824=Cyc_Tcutil_compress(t);t=_tmp824;});
if(Cyc_Tcutil_is_array(t)){
({void*_tmp826=({void*_tmp825=t;Cyc_Tcutil_promote_array(_tmp825,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp826;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2532
return t;}_LL0:;};}
# 2543 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp471=e->r;void*_tmp472=_tmp471;struct Cyc_Absyn_Exp*_tmp496;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp472)->tag == 12U){_LL1: _tmp496=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp472)->f1;_LL2:
# 2547
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp496);
({void*_tmp827=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp496->topt),0);_tmp496->topt=_tmp827;});
e->topt=_tmp496->topt;
goto _LL0;}else{_LL3: _LL4:
# 2553
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp828=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp828;});
# 2556
{void*_tmp473=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp474=_tmp473;void*_tmp495;struct Cyc_Absyn_Tqual _tmp494;void*_tmp493;union Cyc_Absyn_Constraint*_tmp492;union Cyc_Absyn_Constraint*_tmp491;union Cyc_Absyn_Constraint*_tmp490;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->tag == 5U){_LL6: _tmp495=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).elt_typ;_tmp494=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).elt_tq;_tmp493=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).rgn;_tmp492=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).nullable;_tmp491=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).bounds;_tmp490=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).zero_term;_LL7:
# 2558
{void*_tmp475=Cyc_Tcutil_compress(_tmp495);void*_tmp476=_tmp475;struct Cyc_List_List*_tmp48F;void*_tmp48E;struct Cyc_Absyn_Tqual _tmp48D;void*_tmp48C;struct Cyc_List_List*_tmp48B;int _tmp48A;struct Cyc_Absyn_VarargInfo*_tmp489;struct Cyc_List_List*_tmp488;struct Cyc_List_List*_tmp487;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_List_List*_tmp485;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_List_List*_tmp483;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->tag == 9U){_LLB: _tmp48F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).tvars;_tmp48E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).effect;_tmp48D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).ret_tqual;_tmp48C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).ret_typ;_tmp48B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).args;_tmp48A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).c_varargs;_tmp489=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).cyc_varargs;_tmp488=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).rgn_po;_tmp487=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).attributes;_tmp486=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).requires_clause;_tmp485=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).requires_relns;_tmp484=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).ensures_clause;_tmp483=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).ensures_relns;_LLC:
# 2560
 if(_tmp48F != 0){
struct _tuple12 _tmp477=({struct _tuple12 _tmp482;({struct Cyc_List_List*_tmp829=Cyc_Tcenv_lookup_type_vars(te);_tmp482.f1=_tmp829;});_tmp482.f2=Cyc_Core_heap_region;_tmp482;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp477,_tmp48F);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2568
({struct Cyc_List_List*_tmp82A=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp488);_tmp488=_tmp82A;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp488);{
void*ftyp=({struct Cyc_List_List*_tmp82D=inst;Cyc_Tcutil_substitute(_tmp82D,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp47F=_cycalloc(sizeof(*_tmp47F));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp82C=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp480;_tmp480.tag=9U;({struct Cyc_Absyn_FnInfo _tmp82B=({struct Cyc_Absyn_FnInfo _tmp481;_tmp481.tvars=0;_tmp481.effect=_tmp48E;_tmp481.ret_tqual=_tmp48D;_tmp481.ret_typ=_tmp48C;_tmp481.args=_tmp48B;_tmp481.c_varargs=_tmp48A;_tmp481.cyc_varargs=_tmp489;_tmp481.rgn_po=0;_tmp481.attributes=_tmp487;_tmp481.requires_clause=_tmp486;_tmp481.requires_relns=_tmp485;_tmp481.ensures_clause=_tmp484;_tmp481.ensures_relns=_tmp483;_tmp481;});_tmp480.f1=_tmp82B;});_tmp480;});_tmp47F[0]=_tmp82C;});_tmp47F;}));});
# 2575
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp478=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp830=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp47D;_tmp47D.tag=5U;({struct Cyc_Absyn_PtrInfo _tmp82F=({struct Cyc_Absyn_PtrInfo _tmp47E;_tmp47E.elt_typ=ftyp;_tmp47E.elt_tq=_tmp494;({struct Cyc_Absyn_PtrAtts _tmp82E=({(_tmp47E.ptr_atts).rgn=_tmp493;(_tmp47E.ptr_atts).nullable=_tmp492;(_tmp47E.ptr_atts).bounds=_tmp491;(_tmp47E.ptr_atts).zero_term=_tmp490;(_tmp47E.ptr_atts).ptrloc=0;_tmp47E.ptr_atts;});_tmp47E.ptr_atts=_tmp82E;});_tmp47E;});_tmp47D.f1=_tmp82F;});_tmp47D;});_tmp47C[0]=_tmp830;});_tmp47C;});
# 2577
struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_copy_exp(e);
({void*_tmp832=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp831=({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp47B;_tmp47B.tag=13U;_tmp47B.f1=_tmp479;_tmp47B.f2=ts;_tmp47B;});_tmp47A[0]=_tmp831;});_tmp47A;});e->r=_tmp832;});
e->topt=(void*)_tmp478;};}
# 2581
goto _LLA;}else{_LLD: _LLE:
 goto _LLA;}_LLA:;}
# 2584
goto _LL5;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2587
goto _LL0;}_LL0:;}
# 2589
return(void*)_check_null(e->topt);}
# 2597
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2599
struct Cyc_List_List*_tmp497=0;
# 2601
{void*_tmp498=fn_exp->r;void*_tmp499=_tmp498;struct Cyc_List_List*_tmp4AD;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp499)->tag == 10U){_LL1: _tmp4AD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL2:
# 2603
{void*_tmp49A=e->r;void*_tmp49B=_tmp49A;struct Cyc_List_List*_tmp4AA;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp49B)->tag == 10U){_LL6: _tmp4AA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp49B)->f2;_LL7: {
# 2605
struct Cyc_List_List*_tmp49C=alias_arg_exps;
int _tmp49D=0;
while(_tmp49C != 0){
while(_tmp49D != (int)_tmp49C->hd){
if(_tmp4AD == 0)
({void*_tmp49E=0U;({struct _dyneither_ptr _tmp834=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4A2;_tmp4A2.tag=1U;_tmp4A2.f1=(unsigned long)((int)_tmp49C->hd);({struct Cyc_Int_pa_PrintArg_struct _tmp4A1;_tmp4A1.tag=1U;_tmp4A1.f1=(unsigned long)_tmp49D;({void*_tmp49F[2U]={& _tmp4A1,& _tmp4A2};({struct _dyneither_ptr _tmp833=({const char*_tmp4A0="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp4A0,sizeof(char),36U);});Cyc_aprintf(_tmp833,_tag_dyneither(_tmp49F,sizeof(void*),2U));});});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp834,_tag_dyneither(_tmp49E,sizeof(void*),0U));});});
# 2612
++ _tmp49D;
_tmp4AD=_tmp4AD->tl;
_tmp4AA=((struct Cyc_List_List*)_check_null(_tmp4AA))->tl;}{
# 2617
struct _tuple11 _tmp4A3=({struct Cyc_Absyn_Exp*_tmp835=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AD))->hd;Cyc_Tcutil_insert_alias(_tmp835,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AA))->hd)->topt)));});struct _tuple11 _tmp4A4=_tmp4A3;struct Cyc_Absyn_Decl*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;_LLB: _tmp4A7=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;_LLC:;
_tmp4AD->hd=(void*)_tmp4A6;
({struct Cyc_List_List*_tmp836=({struct Cyc_List_List*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5->hd=_tmp4A7;_tmp4A5->tl=_tmp497;_tmp4A5;});_tmp497=_tmp836;});
_tmp49C=_tmp49C->tl;};}
# 2622
goto _LL5;}}else{_LL8: _LL9:
({void*_tmp4A8=0U;({struct _dyneither_ptr _tmp837=({const char*_tmp4A9="not a function call!";_tag_dyneither(_tmp4A9,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp837,_tag_dyneither(_tmp4A8,sizeof(void*),0U));});});}_LL5:;}
# 2625
goto _LL0;}else{_LL3: _LL4:
({void*_tmp4AB=0U;({struct _dyneither_ptr _tmp838=({const char*_tmp4AC="not a function call!";_tag_dyneither(_tmp4AC,sizeof(char),21U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp838,_tag_dyneither(_tmp4AB,sizeof(void*),0U));});});}_LL0:;}
# 2630
while(_tmp497 != 0){
struct Cyc_Absyn_Decl*_tmp4AE=(struct Cyc_Absyn_Decl*)_tmp497->hd;
({struct Cyc_Absyn_Exp*_tmp83C=({struct Cyc_Absyn_Stmt*_tmp83B=({struct Cyc_Absyn_Decl*_tmp83A=_tmp4AE;struct Cyc_Absyn_Stmt*_tmp839=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp83A,_tmp839,e->loc);});Cyc_Absyn_stmt_exp(_tmp83B,e->loc);});fn_exp=_tmp83C;});
_tmp497=_tmp497->tl;}
# 2637
e->topt=0;
e->r=fn_exp->r;}
# 2643
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2647
{void*_tmp4AF=e->r;void*_tmp4B0=_tmp4AF;void*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;struct _dyneither_ptr*_tmp519;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp517;int*_tmp516;struct Cyc_Absyn_Exp**_tmp515;void***_tmp514;struct Cyc_Absyn_Exp**_tmp513;int*_tmp512;void*_tmp511;struct Cyc_Absyn_Enumfield*_tmp510;struct Cyc_Absyn_Enumdecl*_tmp50F;struct Cyc_Absyn_Enumfield*_tmp50E;struct Cyc_List_List*_tmp50D;struct Cyc_Absyn_Datatypedecl*_tmp50C;struct Cyc_Absyn_Datatypefield*_tmp50B;void*_tmp50A;struct Cyc_List_List*_tmp509;struct _tuple1**_tmp508;struct Cyc_List_List**_tmp507;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_Aggrdecl**_tmp505;struct Cyc_Absyn_Exp*_tmp504;void*_tmp503;int*_tmp502;struct Cyc_Absyn_Vardecl*_tmp501;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;int*_tmp4FE;struct Cyc_Absyn_Stmt*_tmp4FD;struct Cyc_List_List*_tmp4FC;struct _tuple9*_tmp4FB;struct Cyc_List_List*_tmp4FA;struct Cyc_List_List*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;struct _dyneither_ptr*_tmp4F5;int*_tmp4F4;int*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct _dyneither_ptr*_tmp4F1;int*_tmp4F0;int*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;void*_tmp4ED;struct Cyc_List_List*_tmp4EC;void*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp**_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;void*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;enum Cyc_Absyn_Coercion*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_List_List*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Core_Opt*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D4;enum Cyc_Absyn_Incrementor _tmp4D3;enum Cyc_Absyn_Primop _tmp4D2;struct Cyc_List_List*_tmp4D1;void**_tmp4D0;union Cyc_Absyn_Cnst*_tmp4CF;struct Cyc_Core_Opt*_tmp4CE;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_List_List*_tmp4CB;struct Cyc_Absyn_VarargCallInfo**_tmp4CA;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_List_List*_tmp4C8;struct Cyc_Absyn_VarargCallInfo**_tmp4C7;int*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C5;switch(*((int*)_tmp4B0)){case 12U: _LL1: _tmp4C5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2:
# 2652
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp4C5);
return;case 10U: _LL3: _tmp4C9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4C8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4C7=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp4C6=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;if(!(*_tmp4C6)){_LL4:
# 2656
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));({struct Cyc_Core_Impossible_exn_struct _tmp83E=({struct Cyc_Core_Impossible_exn_struct _tmp4B2;_tmp4B2.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp83D=({const char*_tmp4B3="unresolved function in tcExpNoInst";_tag_dyneither(_tmp4B3,sizeof(char),35U);});_tmp4B2.f1=_tmp83D;});_tmp4B2;});_tmp4B1[0]=_tmp83E;});_tmp4B1;}));}else{_LL1B: _tmp4CC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4CB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4CA=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL1C: {
# 2689
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp4B4;_push_handler(& _tmp4B4);{int _tmp4B6=0;if(setjmp(_tmp4B4.handler))_tmp4B6=1;if(!_tmp4B6){
({struct Cyc_Absyn_Exp*_tmp83F=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp83F;});;_pop_handler();}else{void*_tmp4B5=(void*)_exn_thrown;void*_tmp4B7=_tmp4B5;void*_tmp4B8;if(((struct Cyc_Core_Failure_exn_struct*)_tmp4B7)->tag == Cyc_Core_Failure){_LL56: _LL57:
# 2696
 ok=0;
fn_exp=e;
goto _LL55;}else{_LL58: _tmp4B8=_tmp4B7;_LL59:(int)_rethrow(_tmp4B8);}_LL55:;}};}
# 2700
({void*_tmp840=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4CC,_tmp4CB,_tmp4CA,& alias_arg_exps);t=_tmp840;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp841=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp841;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2707
goto _LL0;}}case 36U: _LL5: _tmp4CE=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4CD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL6:
# 2660
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp4CD);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0U: _LL7: _tmp4CF=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL8:
# 2665
({void*_tmp842=Cyc_Tcexp_tcConst(te,loc,topt,_tmp4CF,e);t=_tmp842;});goto _LL0;case 1U: _LL9: _tmp4D0=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LLA:
# 2667
({void*_tmp843=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp4D0);t=_tmp843;});goto _LL0;case 2U: _LLB: _LLC:
# 2669
 t=Cyc_Absyn_sint_typ;goto _LL0;case 3U: _LLD: _tmp4D2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4D1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LLE:
# 2671
({void*_tmp844=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4D2,_tmp4D1);t=_tmp844;});goto _LL0;case 5U: _LLF: _tmp4D4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4D3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL10:
# 2673
({void*_tmp845=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp4D4,_tmp4D3);t=_tmp845;});goto _LL0;case 4U: _LL11: _tmp4D7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4D6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4D5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL12:
# 2675
({void*_tmp846=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4D7,_tmp4D6,_tmp4D5);t=_tmp846;});goto _LL0;case 6U: _LL13: _tmp4DA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4D9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4D8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL14:
# 2677
({void*_tmp847=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp4DA,_tmp4D9,_tmp4D8);t=_tmp847;});goto _LL0;case 7U: _LL15: _tmp4DC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4DB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL16:
# 2679
({void*_tmp848=Cyc_Tcexp_tcAnd(te,loc,_tmp4DC,_tmp4DB);t=_tmp848;});goto _LL0;case 8U: _LL17: _tmp4DE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4DD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL18:
# 2681
({void*_tmp849=Cyc_Tcexp_tcOr(te,loc,_tmp4DE,_tmp4DD);t=_tmp849;});goto _LL0;case 9U: _LL19: _tmp4E0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4DF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL1A:
# 2683
({void*_tmp84A=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4E0,_tmp4DF);t=_tmp84A;});goto _LL0;case 11U: _LL1D: _tmp4E1=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL1E:
# 2709
({void*_tmp84B=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4E1);t=_tmp84B;});goto _LL0;case 13U: _LL1F: _tmp4E3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4E2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL20:
# 2711
({void*_tmp84C=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4E3,_tmp4E2);t=_tmp84C;});goto _LL0;case 14U: _LL21: _tmp4E6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4E5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4E4=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL22:
# 2713
({void*_tmp84D=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4E6,_tmp4E5,_tmp4E4);t=_tmp84D;});goto _LL0;case 15U: _LL23: _tmp4E7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL24:
# 2715
({void*_tmp84E=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4E7);t=_tmp84E;});goto _LL0;case 16U: _LL25: _tmp4E9=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4E8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL26:
# 2717
({void*_tmp84F=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4E9,e,_tmp4E8);t=_tmp84F;});goto _LL0;case 18U: _LL27: _tmp4EA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL28: {
# 2719
void*_tmp4B9=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4EA);
({void*_tmp850=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4B9);t=_tmp850;});goto _LL0;}case 17U: _LL29: _tmp4EB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2A:
# 2722
({void*_tmp851=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4EB);t=_tmp851;});goto _LL0;case 19U: _LL2B: _tmp4ED=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4EC=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2C:
# 2724
({void*_tmp852=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4ED,_tmp4EC);t=_tmp852;});goto _LL0;case 20U: _LL2D: _tmp4EE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2E:
# 2726
({void*_tmp853=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4EE);t=_tmp853;});goto _LL0;case 21U: _LL2F: _tmp4F2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4F1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4F0=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp4EF=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL30:
# 2728
({void*_tmp854=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4F2,_tmp4F1,_tmp4F0,_tmp4EF);t=_tmp854;});goto _LL0;case 22U: _LL31: _tmp4F6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4F5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4F4=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp4F3=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL32:
# 2730
({void*_tmp855=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4F6,_tmp4F5,_tmp4F4,_tmp4F3);t=_tmp855;});goto _LL0;case 23U: _LL33: _tmp4F8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4F7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL34:
# 2732
({void*_tmp856=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4F8,_tmp4F7);t=_tmp856;});goto _LL0;case 24U: _LL35: _tmp4F9=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL36:
# 2734
({void*_tmp857=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4F9);t=_tmp857;});goto _LL0;case 25U: _LL37: _tmp4FB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4FA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL38:
# 2736
({void*_tmp858=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4FB,_tmp4FA);t=_tmp858;});goto _LL0;case 26U: _LL39: _tmp4FC=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL3A: {
# 2740
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4BA=Cyc_Tcutil_compress(*topt);void*_tmp4BB=_tmp4BA;void**_tmp4BE;struct Cyc_Absyn_Tqual*_tmp4BD;union Cyc_Absyn_Constraint*_tmp4BC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BB)->tag == 8U){_LL5B: _tmp4BE=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BB)->f1).elt_type;_tmp4BD=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BB)->f1).tq;_tmp4BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BB)->f1).zero_term;_LL5C:
# 2746
 elt_topt=_tmp4BE;
elt_tqopt=_tmp4BD;
({int _tmp859=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4BC);zero_term=_tmp859;});
goto _LL5A;}else{_LL5D: _LL5E:
 goto _LL5A;}_LL5A:;}
# 2753
({void*_tmp85A=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4FC);t=_tmp85A;});goto _LL0;}case 37U: _LL3B: _tmp4FD=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL3C:
# 2755
({void*_tmp85B=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4FD);t=_tmp85B;});goto _LL0;case 27U: _LL3D: _tmp501=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp500=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4FF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp4FE=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL3E:
# 2757
({void*_tmp85C=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp501,_tmp500,_tmp4FF,_tmp4FE);t=_tmp85C;});goto _LL0;case 28U: _LL3F: _tmp504=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp503=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp502=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL40:
# 2759
({void*_tmp85D=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp504,_tmp503,_tmp502);t=_tmp85D;});goto _LL0;case 29U: _LL41: _tmp508=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp507=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp506=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp505=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL42:
# 2761
({void*_tmp85E=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp508,_tmp507,_tmp506,_tmp505);t=_tmp85E;});goto _LL0;case 30U: _LL43: _tmp50A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp509=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL44:
# 2763
({void*_tmp85F=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp50A,_tmp509);t=_tmp85F;});goto _LL0;case 31U: _LL45: _tmp50D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp50C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp50B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL46:
# 2765
({void*_tmp860=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp50D,_tmp50C,_tmp50B);t=_tmp860;});goto _LL0;case 32U: _LL47: _tmp50F=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp50E=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL48:
# 2767
({void*_tmp862=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp861=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp4C0;_tmp4C0.tag=13U;_tmp4C0.f1=_tmp50F->name;_tmp4C0.f2=_tmp50F;_tmp4C0;});_tmp4BF[0]=_tmp861;});_tmp4BF;});t=_tmp862;});goto _LL0;case 33U: _LL49: _tmp511=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp510=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL4A:
# 2769
 t=_tmp511;goto _LL0;case 34U: _LL4B: _tmp516=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).is_calloc;_tmp515=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).rgn;_tmp514=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).elt_type;_tmp513=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).num_elts;_tmp512=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).fat_result;_LL4C:
# 2771
({void*_tmp863=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp515,_tmp514,_tmp513,_tmp516,_tmp512);t=_tmp863;});goto _LL0;case 35U: _LL4D: _tmp518=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp517=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL4E:
# 2773
({void*_tmp864=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp518,_tmp517);t=_tmp864;});goto _LL0;case 38U: _LL4F: _tmp51A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp519=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL50:
# 2775
({void*_tmp865=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp51A,_tmp519);t=_tmp865;});goto _LL0;case 39U: _LL51: _tmp51B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL52:
# 2777
 if(!te->allow_valueof)
({void*_tmp4C1=0U;({unsigned int _tmp867=e->loc;struct _dyneither_ptr _tmp866=({const char*_tmp4C2="valueof(-) can only occur within types";_tag_dyneither(_tmp4C2,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp867,_tmp866,_tag_dyneither(_tmp4C1,sizeof(void*),0U));});});
# 2785
t=Cyc_Absyn_sint_typ;
goto _LL0;default: _LL53: _LL54:
# 2788
({void*_tmp4C3=0U;({unsigned int _tmp869=e->loc;struct _dyneither_ptr _tmp868=({const char*_tmp4C4="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp4C4,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp869,_tmp868,_tag_dyneither(_tmp4C3,sizeof(void*),0U));});});
({void*_tmp86A=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp86A;});}_LL0:;}
# 2791
e->topt=t;}
