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
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 74
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
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
# 35
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Hashtable_Table;
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
typedef void*Cyc_CfFlowInfo_root_t;struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};
# 58
typedef void*Cyc_CfFlowInfo_pathcon_t;
typedef struct Cyc_List_List*Cyc_CfFlowInfo_path_t;struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 72
typedef struct Cyc_CfFlowInfo_Place*Cyc_CfFlowInfo_place_t;
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0,Cyc_CfFlowInfo_AllIL  = 1};
# 78
typedef enum Cyc_CfFlowInfo_InitLevel Cyc_CfFlowInfo_initlevel_t;extern char Cyc_CfFlowInfo_IsZero[7];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};
# 93
typedef union Cyc_CfFlowInfo_AbsLVal Cyc_CfFlowInfo_absLval_t;
# 98
typedef void*Cyc_CfFlowInfo_absRval_t;
typedef void*Cyc_CfFlowInfo_absRval_opt_t;
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_flowdict_t;
typedef struct _dyneither_ptr Cyc_CfFlowInfo_aggrdict_t;struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};
# 106
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 137 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 152
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
# 46 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 50
({void*_tmp0=0;({unsigned int _tmp51D=loc;struct _dyneither_ptr _tmp51C=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp51D,_tmp51C,_tag_dyneither(_tmp0,sizeof(void*),0));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 54
return*topt;}}
# 63
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 67
if(topt == 0){
# 69
({void*_tmp51F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp51E=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp2;_tmp2.tag=26;_tmp2.f1=des;_tmp2;});_tmp1[0]=_tmp51E;});_tmp1;});e->r=_tmp51F;});
return;}{
# 72
void*t=*topt;
void*_tmp3=Cyc_Tcutil_compress(t);void*_tmp4=_tmp3;void*_tmp13;struct Cyc_Absyn_Tqual _tmp12;union Cyc_Absyn_AggrInfoU _tmp11;switch(*((int*)_tmp4)){case 11: _LL1: _tmp11=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1).aggr_info;_LL2:
# 75
{union Cyc_Absyn_AggrInfoU _tmp5=_tmp11;struct Cyc_Absyn_Aggrdecl*_tmpA;if((_tmp5.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp6=0;({struct _dyneither_ptr _tmp520=({const char*_tmp7="struct type not properly set";_tag_dyneither(_tmp7,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp520,_tag_dyneither(_tmp6,sizeof(void*),0));});});}else{_LLC: _tmpA=*(_tmp5.KnownAggr).val;_LLD:
({void*_tmp522=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp8=_cycalloc(sizeof(*_tmp8));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp521=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp9;_tmp9.tag=29;_tmp9.f1=_tmpA->name;_tmp9.f2=0;_tmp9.f3=des;_tmp9.f4=_tmpA;_tmp9;});_tmp8[0]=_tmp521;});_tmp8;});e->r=_tmp522;});}_LL9:;}
# 79
goto _LL0;case 8: _LL3: _tmp13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).elt_type;_tmp12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).tq;_LL4:
({void*_tmp524=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp523=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpC;_tmpC.tag=26;_tmpC.f1=des;_tmpC;});_tmpB[0]=_tmp523;});_tmpB;});e->r=_tmp524;});goto _LL0;case 12: _LL5: _LL6:
({void*_tmp526=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD=_cycalloc(sizeof(*_tmpD));({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp525=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE;_tmpE.tag=30;_tmpE.f1=t;_tmpE.f2=des;_tmpE;});_tmpD[0]=_tmp525;});_tmpD;});e->r=_tmp526;});goto _LL0;default: _LL7: _LL8:
({void*_tmp528=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp527=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp10;_tmp10.tag=26;_tmp10.f1=des;_tmp10;});_tmpF[0]=_tmp527;});_tmpF;});e->r=_tmp528;});goto _LL0;}_LL0:;};}
# 89
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 93
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 99
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp14=e->r;void*_tmp15=_tmp14;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->tag == 4){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->f2 == 0){_LLF: _LL10:
({void*_tmp16=0;({unsigned int _tmp52A=e->loc;struct _dyneither_ptr _tmp529=({const char*_tmp17="assignment in test";_tag_dyneither(_tmp17,sizeof(char),19);});Cyc_Tcutil_warn(_tmp52A,_tmp529,_tag_dyneither(_tmp16,sizeof(void*),0));});});goto _LLE;}else{goto _LL11;}}else{_LL11: _LL12:
 goto _LLE;}_LLE:;}
# 107
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;({struct _dyneither_ptr _tmp52B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp1B.f1=_tmp52B;});({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp18[2]={& _tmp1A,& _tmp1B};({unsigned int _tmp52D=e->loc;struct _dyneither_ptr _tmp52C=({const char*_tmp19="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp19,sizeof(char),59);});Cyc_Tcutil_terr(_tmp52D,_tmp52C,_tag_dyneither(_tmp18,sizeof(void*),2));});});});});
# 116
{void*_tmp1C=e->r;void*_tmp1D=_tmp1C;enum Cyc_Absyn_Primop _tmp26;struct Cyc_List_List*_tmp25;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->tag == 3){_LL14: _tmp26=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f1;_tmp25=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f2;_LL15:
# 118
 if(_tmp26 == Cyc_Absyn_Eq  || _tmp26 == Cyc_Absyn_Neq){
struct _tuple0 _tmp1E=({struct _tuple0 _tmp24;_tmp24.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25))->hd)->topt);_tmp24.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25->tl))->hd)->topt);_tmp24;});struct _tuple0 _tmp1F=_tmp1E;void*_tmp23;void*_tmp22;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->tag == 15){_LL19: _tmp23=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->f1;_tmp22=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->f1;_LL1A:
# 121
 return({struct Cyc_Tcexp_TestEnv _tmp20;({struct _tuple0*_tmp52E=({struct _tuple0*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->f1=_tmp23;_tmp21->f2=_tmp22;_tmp21;});_tmp20.eq=_tmp52E;});_tmp20.isTrue=_tmp26 == Cyc_Absyn_Eq;_tmp20;});}else{goto _LL1B;}}else{_LL1B: _LL1C:
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
{union Cyc_Absyn_Cnst _tmp28=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp29=_tmp28;struct _dyneither_ptr _tmp65;struct _dyneither_ptr _tmp64;enum Cyc_Absyn_Sign _tmp63;int _tmp62;int _tmp61;enum Cyc_Absyn_Sign _tmp60;enum Cyc_Absyn_Sign _tmp5F;switch((_tmp29.Wstring_c).tag){case 2: switch(((_tmp29.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1E: _LL1F:
 t=Cyc_Absyn_schar_typ;goto _LL1D;case Cyc_Absyn_Unsigned: _LL20: _LL21:
 t=Cyc_Absyn_uchar_typ;goto _LL1D;default: _LL22: _LL23:
 t=Cyc_Absyn_char_typ;goto _LL1D;}case 3: _LL24: _LL25:
({void*_tmp52F=Cyc_Absyn_wchar_typ();t=_tmp52F;});goto _LL1D;case 4: _LL26: _tmp5F=((_tmp29.Short_c).val).f1;_LL27:
# 165
 t=_tmp5F == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL1D;case 6: _LL28: _tmp60=((_tmp29.LongLong_c).val).f1;_LL29:
# 167
 t=_tmp60 == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL1D;case 7: _LL2A: _tmp61=((_tmp29.Float_c).val).f2;_LL2B:
({void*_tmp530=Cyc_Absyn_float_typ(_tmp61);t=_tmp530;});goto _LL1D;case 5: _LL2C: _tmp63=((_tmp29.Int_c).val).f1;_tmp62=((_tmp29.Int_c).val).f2;_LL2D:
# 170
 if(topt == 0)
t=_tmp63 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 177
void*_tmp2A=Cyc_Tcutil_compress(*topt);void*_tmp2B=_tmp2A;void*_tmp44;void*_tmp43;struct Cyc_Absyn_Tqual _tmp42;void*_tmp41;union Cyc_Absyn_Constraint*_tmp40;union Cyc_Absyn_Constraint*_tmp3F;union Cyc_Absyn_Constraint*_tmp3E;enum Cyc_Absyn_Sign _tmp3D;enum Cyc_Absyn_Sign _tmp3C;enum Cyc_Absyn_Sign _tmp3B;enum Cyc_Absyn_Sign _tmp3A;switch(*((int*)_tmp2B)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f2){case Cyc_Absyn_Char_sz: _LL35: _tmp3A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL36:
# 179
{enum Cyc_Absyn_Sign _tmp2C=_tmp3A;switch(_tmp2C){case Cyc_Absyn_Unsigned: _LL44: _LL45:
 t=Cyc_Absyn_uchar_typ;goto _LL43;case Cyc_Absyn_Signed: _LL46: _LL47:
 t=Cyc_Absyn_schar_typ;goto _LL43;default: _LL48: _LL49:
 t=Cyc_Absyn_char_typ;goto _LL43;}_LL43:;}
# 184
({union Cyc_Absyn_Cnst _tmp531=Cyc_Absyn_Char_c(_tmp3A,(char)_tmp62);*c=_tmp531;});
goto _LL34;case Cyc_Absyn_Short_sz: _LL37: _tmp3B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL38:
# 187
 t=_tmp3B == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
({union Cyc_Absyn_Cnst _tmp532=Cyc_Absyn_Short_c(_tmp3B,(short)_tmp62);*c=_tmp532;});
goto _LL34;case Cyc_Absyn_Int_sz: _LL39: _tmp3C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL3A:
# 191
 t=_tmp3C == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;case Cyc_Absyn_Long_sz: _LL3B: _tmp3D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL3C:
# 194
 t=_tmp3D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;default: goto _LL41;}case 5: _LL3D: _tmp43=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_typ;_tmp42=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_tq;_tmp41=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).rgn;_tmp40=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).nullable;_tmp3F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).bounds;_tmp3E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;if(_tmp62 == 0){_LL3E: {
# 198
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp40))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp2F;_tmp2F.tag=0;({struct _dyneither_ptr _tmp533=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));_tmp2F.f1=_tmp533;});({void*_tmp2D[1]={& _tmp2F};({unsigned int _tmp535=e->loc;struct _dyneither_ptr _tmp534=({const char*_tmp2E="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp2E,sizeof(char),53);});Cyc_Tcutil_terr(_tmp535,_tmp534,_tag_dyneither(_tmp2D,sizeof(void*),1));});});});{
struct Cyc_List_List*_tmp30=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp539=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp31=_cycalloc(sizeof(*_tmp31));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp538=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp32;_tmp32.tag=5;({struct Cyc_Absyn_PtrInfo _tmp537=({struct Cyc_Absyn_PtrInfo _tmp33;_tmp33.elt_typ=_tmp43;_tmp33.elt_tq=_tmp42;({struct Cyc_Absyn_PtrAtts _tmp536=({(_tmp33.ptr_atts).rgn=_tmp41;(_tmp33.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp33.ptr_atts).bounds=_tmp3F;(_tmp33.ptr_atts).zero_term=_tmp3E;(_tmp33.ptr_atts).ptrloc=0;_tmp33.ptr_atts;});_tmp33.ptr_atts=_tmp536;});_tmp33;});_tmp32.f1=_tmp537;});_tmp32;});_tmp31[0]=_tmp538;});_tmp31;});t=_tmp539;});
# 206
goto _LL34;};}}else{goto _LL41;}case 19: _LL3F: _tmp44=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B)->f1;_LL40: {
# 208
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp35=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp38=_cycalloc(sizeof(*_tmp38));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp53B=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp39;_tmp39.tag=18;({struct Cyc_Absyn_Exp*_tmp53A=Cyc_Absyn_uint_exp((unsigned int)_tmp62,0);_tmp39.f1=_tmp53A;});_tmp39;});_tmp38[0]=_tmp53B;});_tmp38;});
# 215
({void*_tmp53D=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp36=_cycalloc(sizeof(*_tmp36));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp53C=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp37;_tmp37.tag=19;_tmp37.f1=(void*)_tmp35;_tmp37;});_tmp36[0]=_tmp53C;});_tmp36;});t=_tmp53D;});
goto _LL34;}default: _LL41: _LL42:
# 218
 t=_tmp63 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;}_LL34:;}
# 221
goto _LL1D;case 8: _LL2E: _tmp64=(_tmp29.String_c).val;_LL2F:
# 223
 string_numelts=(int)_get_dyneither_size(_tmp64,sizeof(char));
_tmp65=_tmp64;goto _LL31;case 9: _LL30: _tmp65=(_tmp29.Wstring_c).val;_LL31:
# 226
 if(string_numelts == 0){
({int _tmp53E=Cyc_Tcexp_wchar_numelts(_tmp65);string_numelts=_tmp53E;});
({void*_tmp53F=Cyc_Absyn_wchar_typ();string_elt_typ=_tmp53F;});}{
# 230
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp540=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp540,loc);});
elen->topt=Cyc_Absyn_uint_typ;{
# 235
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp45=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp541=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp4A;_tmp4A.tag=1;_tmp4A.f1=elen;_tmp4A;});_tmp49[0]=_tmp541;});_tmp49;});
({void*_tmp545=({void*_tmp544=string_elt_typ;struct Cyc_Absyn_Tqual _tmp543=Cyc_Absyn_const_tqual(0);void*_tmp542=(void*)_tmp45;Cyc_Absyn_atb_typ(_tmp544,(void*)& Cyc_Absyn_HeapRgn_val,_tmp543,_tmp542,Cyc_Absyn_true_conref);});t=_tmp545;});
# 238
if(topt != 0){
void*_tmp46=Cyc_Tcutil_compress(*topt);void*_tmp47=_tmp46;struct Cyc_Absyn_Tqual _tmp48;switch(*((int*)_tmp47)){case 8: _LL4B: _tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47)->f1).tq;_LL4C:
# 243
 return({void*_tmp548=string_elt_typ;struct Cyc_Absyn_Tqual _tmp547=_tmp48;struct Cyc_Absyn_Exp*_tmp546=elen;Cyc_Absyn_array_typ(_tmp548,_tmp547,_tmp546,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);});case 5: _LL4D: _LL4E:
# 245
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 251
({void*_tmp54D=({void*_tmp54C=string_elt_typ;void*_tmp54B=Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));struct Cyc_Absyn_Tqual _tmp54A=
Cyc_Absyn_const_tqual(0);
# 251
void*_tmp549=(void*)_tmp45;Cyc_Absyn_atb_typ(_tmp54C,_tmp54B,_tmp54A,_tmp549,Cyc_Absyn_true_conref);});t=_tmp54D;});
# 253
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 259
goto _LL4A;default: _LL4F: _LL50:
 goto _LL4A;}_LL4A:;}
# 263
return t;};};default: _LL32: _LL33:
# 265
 if(topt != 0){
void*_tmp4B=Cyc_Tcutil_compress(*topt);void*_tmp4C=_tmp4B;void*_tmp58;struct Cyc_Absyn_Tqual _tmp57;void*_tmp56;union Cyc_Absyn_Constraint*_tmp55;union Cyc_Absyn_Constraint*_tmp54;union Cyc_Absyn_Constraint*_tmp53;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->tag == 5){_LL52: _tmp58=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_typ;_tmp57=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_tq;_tmp56=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).rgn;_tmp55=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_tmp54=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).bounds;_tmp53=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).zero_term;_LL53:
# 269
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp55))return*topt;
({struct Cyc_String_pa_PrintArg_struct _tmp4F;_tmp4F.tag=0;({struct _dyneither_ptr _tmp54E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(*topt));_tmp4F.f1=_tmp54E;});({void*_tmp4D[1]={& _tmp4F};({unsigned int _tmp550=e->loc;struct _dyneither_ptr _tmp54F=({const char*_tmp4E="Used NULL when expecting a non-NULL value of type %s";_tag_dyneither(_tmp4E,sizeof(char),53);});Cyc_Tcutil_terr(_tmp550,_tmp54F,_tag_dyneither(_tmp4D,sizeof(void*),1));});});});
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp50=_cycalloc(sizeof(*_tmp50));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp553=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp51;_tmp51.tag=5;({struct Cyc_Absyn_PtrInfo _tmp552=({struct Cyc_Absyn_PtrInfo _tmp52;_tmp52.elt_typ=_tmp58;_tmp52.elt_tq=_tmp57;({struct Cyc_Absyn_PtrAtts _tmp551=({(_tmp52.ptr_atts).rgn=_tmp56;(_tmp52.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp52.ptr_atts).bounds=_tmp54;(_tmp52.ptr_atts).zero_term=_tmp53;(_tmp52.ptr_atts).ptrloc=0;_tmp52.ptr_atts;});_tmp52.ptr_atts=_tmp551;});_tmp52;});_tmp51.f1=_tmp552;});_tmp51;});_tmp50[0]=_tmp553;});_tmp50;});}else{_LL54: _LL55:
# 274
 goto _LL51;}_LL51:;}{
# 277
struct Cyc_List_List*_tmp59=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp55C=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp55B=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp5B;_tmp5B.tag=5;({struct Cyc_Absyn_PtrInfo _tmp55A=({struct Cyc_Absyn_PtrInfo _tmp5C;({void*_tmp554=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->v=_tmp59;_tmp5E;}));_tmp5C.elt_typ=_tmp554;});({struct Cyc_Absyn_Tqual _tmp555=
Cyc_Absyn_empty_tqual(0);_tmp5C.elt_tq=_tmp555;});({struct Cyc_Absyn_PtrAtts _tmp559=({({void*_tmp556=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->v=_tmp59;_tmp5D;}));(_tmp5C.ptr_atts).rgn=_tmp556;});(_tmp5C.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmp557=
# 282
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp5C.ptr_atts).bounds=_tmp557;});({union Cyc_Absyn_Constraint*_tmp558=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp5C.ptr_atts).zero_term=_tmp558;});(_tmp5C.ptr_atts).ptrloc=0;_tmp5C.ptr_atts;});_tmp5C.ptr_atts=_tmp559;});_tmp5C;});_tmp5B.f1=_tmp55A;});_tmp5B;});_tmp5A[0]=_tmp55B;});_tmp5A;});
# 278
t=_tmp55C;});
# 284
goto _LL1D;};}_LL1D:;}
# 286
return t;}
# 290
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 295
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp66=*((void**)_check_null(b));void*_tmp67=_tmp66;struct Cyc_Absyn_Vardecl*_tmp75;struct Cyc_Absyn_Vardecl*_tmp74;struct Cyc_Absyn_Vardecl*_tmp73;struct Cyc_Absyn_Fndecl*_tmp72;struct Cyc_Absyn_Vardecl*_tmp71;struct _tuple1*_tmp70;switch(*((int*)_tmp67)){case 0: _LL57: _tmp70=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp67)->f1;_LL58:
# 298
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp68=_cycalloc(sizeof(*_tmp68));({struct Cyc_Core_Impossible_exn_struct _tmp55E=({struct Cyc_Core_Impossible_exn_struct _tmp69;_tmp69.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp55D=({const char*_tmp6A="unresolved binding in tcVar";_tag_dyneither(_tmp6A,sizeof(char),28);});_tmp69.f1=_tmp55D;});_tmp69;});_tmp68[0]=_tmp55E;});_tmp68;}));case 1: _LL59: _tmp71=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp67)->f1;_LL5A:
# 302
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp71->name,1);
return _tmp71->type;case 2: _LL5B: _tmp72=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp67)->f1;_LL5C:
# 309
 if(_tmp72->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp72->name,1);
return Cyc_Tcutil_fndecl2typ(_tmp72);case 5: _LL5D: _tmp73=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp67)->f1;_LL5E:
 _tmp74=_tmp73;goto _LL60;case 4: _LL5F: _tmp74=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp67)->f1;_LL60:
 _tmp75=_tmp74;goto _LL62;default: _LL61: _tmp75=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp67)->f1;_LL62:
# 315
 if(te->allow_valueof){
void*_tmp6B=Cyc_Tcutil_compress(_tmp75->type);void*_tmp6C=_tmp6B;void*_tmp6F;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6C)->tag == 19){_LL64: _tmp6F=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp6C)->f1;_LL65:
# 318
({void*_tmp560=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp55F=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp6E;_tmp6E.tag=39;_tmp6E.f1=_tmp6F;_tmp6E;});_tmp6D[0]=_tmp55F;});_tmp6D;});e->r=_tmp560;});
goto _LL63;}else{_LL66: _LL67:
 goto _LL63;}_LL63:;}
# 323
return _tmp75->type;}_LL56:;}
# 327
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct Cyc_List_List*(*type_getter)(struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 334
struct Cyc_List_List*desc_types;
{void*_tmp76=fmt->r;void*_tmp77=_tmp76;struct _dyneither_ptr _tmp7C;struct _dyneither_ptr _tmp7B;switch(*((int*)_tmp77)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp77)->f1).String_c).tag == 8){_LL69: _tmp7B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp77)->f1).String_c).val;_LL6A:
 _tmp7C=_tmp7B;goto _LL6C;}else{goto _LL6D;}case 14: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2)->r)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2)->r)->f1).String_c).tag == 8){_LL6B: _tmp7C=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp77)->f2)->r)->f1).String_c).val;_LL6C:
# 338
({struct Cyc_List_List*_tmp561=type_getter(te,(struct _dyneither_ptr)_tmp7C,fmt->loc);desc_types=_tmp561;});goto _LL68;}else{goto _LL6D;}}else{goto _LL6D;}default: _LL6D: _LL6E:
# 342
 if(opt_args != 0){
struct Cyc_List_List*_tmp78=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp78 != 0;_tmp78=_tmp78->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp78->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp78->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp78->hd))
({void*_tmp79=0;({unsigned int _tmp563=((struct Cyc_Absyn_Exp*)_tmp78->hd)->loc;struct _dyneither_ptr _tmp562=({const char*_tmp7A="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp7A,sizeof(char),49);});Cyc_Tcutil_terr(_tmp563,_tmp562,_tag_dyneither(_tmp79,sizeof(void*),0));});});}}
# 351
return;}_LL68:;}
# 353
if(opt_args != 0){
struct Cyc_List_List*_tmp7D=(struct Cyc_List_List*)opt_args->v;
# 356
for(0;desc_types != 0  && _tmp7D != 0;
(((desc_types=desc_types->tl,_tmp7D=_tmp7D->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp7D->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp81;_tmp81.tag=0;({struct _dyneither_ptr _tmp564=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp81.f1=_tmp564;});({struct Cyc_String_pa_PrintArg_struct _tmp80;_tmp80.tag=0;({struct _dyneither_ptr _tmp565=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp80.f1=_tmp565;});({void*_tmp7E[2]={& _tmp80,& _tmp81};({unsigned int _tmp567=e->loc;struct _dyneither_ptr _tmp566=({const char*_tmp7F="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp7F,sizeof(char),48);});Cyc_Tcutil_terr(_tmp567,_tmp566,_tag_dyneither(_tmp7E,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 367
if(alias_coercion)
({struct Cyc_List_List*_tmp568=({struct Cyc_List_List*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->hd=(void*)arg_cnt;_tmp82->tl=*alias_arg_exps;_tmp82;});*alias_arg_exps=_tmp568;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp83=0;({unsigned int _tmp56A=((struct Cyc_Absyn_Exp*)_tmp7D->hd)->loc;struct _dyneither_ptr _tmp569=({const char*_tmp84="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp84,sizeof(char),49);});Cyc_Tcutil_terr(_tmp56A,_tmp569,_tag_dyneither(_tmp83,sizeof(void*),0));});});}
# 374
if(desc_types != 0)
({void*_tmp85=0;({unsigned int _tmp56C=fmt->loc;struct _dyneither_ptr _tmp56B=({const char*_tmp86="too few arguments";_tag_dyneither(_tmp86,sizeof(char),18);});Cyc_Tcutil_terr(_tmp56C,_tmp56B,_tag_dyneither(_tmp85,sizeof(void*),0));});});
if(_tmp7D != 0){
({void*_tmp87=0;({unsigned int _tmp56E=((struct Cyc_Absyn_Exp*)_tmp7D->hd)->loc;struct _dyneither_ptr _tmp56D=({const char*_tmp88="too many arguments";_tag_dyneither(_tmp88,sizeof(char),19);});Cyc_Tcutil_terr(_tmp56E,_tmp56D,_tag_dyneither(_tmp87,sizeof(void*),0));});});
# 379
for(0;_tmp7D != 0;_tmp7D=_tmp7D->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp7D->hd);}}}}
# 384
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 386
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp89=p;switch(_tmp89){case Cyc_Absyn_Plus: _LL70: _LL71:
 goto _LL73;case Cyc_Absyn_Minus: _LL72: _LL73:
# 390
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp8C;_tmp8C.tag=0;({struct _dyneither_ptr _tmp56F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp8C.f1=_tmp56F;});({void*_tmp8A[1]={& _tmp8C};({unsigned int _tmp571=loc;struct _dyneither_ptr _tmp570=({const char*_tmp8B="expecting numeric type but found %s";_tag_dyneither(_tmp8B,sizeof(char),36);});Cyc_Tcutil_terr(_tmp571,_tmp570,_tag_dyneither(_tmp8A,sizeof(void*),1));});});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL74: _LL75:
# 394
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp8F;_tmp8F.tag=0;({struct _dyneither_ptr _tmp572=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp8F.f1=_tmp572;});({void*_tmp8D[1]={& _tmp8F};({unsigned int _tmp574=loc;struct _dyneither_ptr _tmp573=({const char*_tmp8E="expecting integral or * type but found %s";_tag_dyneither(_tmp8E,sizeof(char),42);});Cyc_Tcutil_terr(_tmp574,_tmp573,_tag_dyneither(_tmp8D,sizeof(void*),1));});});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL76: _LL77:
# 399
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp92;_tmp92.tag=0;({struct _dyneither_ptr _tmp575=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp92.f1=_tmp575;});({void*_tmp90[1]={& _tmp92};({unsigned int _tmp577=loc;struct _dyneither_ptr _tmp576=({const char*_tmp91="expecting integral type but found %s";_tag_dyneither(_tmp91,sizeof(char),37);});Cyc_Tcutil_terr(_tmp577,_tmp576,_tag_dyneither(_tmp90,sizeof(void*),1));});});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL78: _LL79:
# 403
{void*_tmp93=t;union Cyc_Absyn_Constraint*_tmp9C;switch(*((int*)_tmp93)){case 8: _LL7D: _LL7E:
 goto _LL7C;case 5: _LL7F: _tmp9C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp93)->f1).ptr_atts).bounds;_LL80:
# 406
{void*_tmp94=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9C);void*_tmp95=_tmp94;struct Cyc_Absyn_Exp*_tmp98;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp95)->tag == 0){_LL84: _LL85:
 goto _LL83;}else{_LL86: _tmp98=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp95)->f1;_LL87:
# 409
 if(!Cyc_Evexp_c_can_eval(_tmp98) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp96=0;({unsigned int _tmp579=loc;struct _dyneither_ptr _tmp578=({const char*_tmp97="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp97,sizeof(char),55);});Cyc_Tcutil_terr(_tmp579,_tmp578,_tag_dyneither(_tmp96,sizeof(void*),0));});});
goto _LL83;}_LL83:;}
# 413
goto _LL7C;default: _LL81: _LL82:
# 415
({struct Cyc_String_pa_PrintArg_struct _tmp9B;_tmp9B.tag=0;({struct _dyneither_ptr _tmp57A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp9B.f1=_tmp57A;});({void*_tmp99[1]={& _tmp9B};({unsigned int _tmp57C=loc;struct _dyneither_ptr _tmp57B=({const char*_tmp9A="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp9A,sizeof(char),47);});Cyc_Tcutil_terr(_tmp57C,_tmp57B,_tag_dyneither(_tmp99,sizeof(void*),1));});});});}_LL7C:;}
# 417
return Cyc_Absyn_uint_typ;default: _LL7A: _LL7B:
({void*_tmp9D=0;({struct _dyneither_ptr _tmp57D=({const char*_tmp9E="Non-unary primop";_tag_dyneither(_tmp9E,sizeof(char),17);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp57D,_tag_dyneither(_tmp9D,sizeof(void*),0));});});}_LL6F:;}
# 423
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 426
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmpA1;_tmpA1.tag=0;({struct _dyneither_ptr _tmp57E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmpA1.f1=_tmp57E;});({void*_tmp9F[1]={& _tmpA1};({unsigned int _tmp580=e1->loc;struct _dyneither_ptr _tmp57F=({const char*_tmpA0="type %s cannot be used here";_tag_dyneither(_tmpA0,sizeof(char),28);});Cyc_Tcutil_terr(_tmp580,_tmp57F,_tag_dyneither(_tmp9F,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 430
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpA4;_tmpA4.tag=0;({struct _dyneither_ptr _tmp581=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmpA4.f1=_tmp581;});({void*_tmpA2[1]={& _tmpA4};({unsigned int _tmp583=e2->loc;struct _dyneither_ptr _tmp582=({const char*_tmpA3="type %s cannot be used here";_tag_dyneither(_tmpA3,sizeof(char),28);});Cyc_Tcutil_terr(_tmp583,_tmp582,_tag_dyneither(_tmpA2,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 434
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 439
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmpA5=t1;void*_tmpBB;struct Cyc_Absyn_Tqual _tmpBA;void*_tmpB9;union Cyc_Absyn_Constraint*_tmpB8;union Cyc_Absyn_Constraint*_tmpB7;union Cyc_Absyn_Constraint*_tmpB6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->tag == 5){_LL89: _tmpBB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).elt_typ;_tmpBA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).elt_tq;_tmpB9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).rgn;_tmpB8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).nullable;_tmpB7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).bounds;_tmpB6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA5)->f1).ptr_atts).zero_term;_LL8A:
# 444
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpBB),& Cyc_Tcutil_tmk))
({void*_tmpA6=0;({unsigned int _tmp585=e1->loc;struct _dyneither_ptr _tmp584=({const char*_tmpA7="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpA7,sizeof(char),50);});Cyc_Tcutil_terr(_tmp585,_tmp584,_tag_dyneither(_tmpA6,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpA8=0;({unsigned int _tmp587=e1->loc;struct _dyneither_ptr _tmp586=({const char*_tmpA9="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpA9,sizeof(char),54);});Cyc_Tcutil_terr(_tmp587,_tmp586,_tag_dyneither(_tmpA8,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmpAC;_tmpAC.tag=0;({struct _dyneither_ptr _tmp588=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpAC.f1=_tmp588;});({void*_tmpAA[1]={& _tmpAC};({unsigned int _tmp58A=e2->loc;struct _dyneither_ptr _tmp589=({const char*_tmpAB="expecting int but found %s";_tag_dyneither(_tmpAB,sizeof(char),27);});Cyc_Tcutil_terr(_tmp58A,_tmp589,_tag_dyneither(_tmpAA,sizeof(void*),1));});});});{
void*_tmpAD=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB7);void*_tmpAE=_tmpAD;struct Cyc_Absyn_Exp*_tmpB5;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAE)->tag == 0){_LL8E: _LL8F:
 return t1;}else{_LL90: _tmpB5=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAE)->f1;_LL91:
# 455
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB6))
({void*_tmpAF=0;({unsigned int _tmp58C=e1->loc;struct _dyneither_ptr _tmp58B=({const char*_tmpB0="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmpB0,sizeof(char),70);});Cyc_Tcutil_warn(_tmp58C,_tmp58B,_tag_dyneither(_tmpAF,sizeof(void*),0));});});{
# 463
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB1=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp58F=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpB3;_tmpB3.tag=5;({struct Cyc_Absyn_PtrInfo _tmp58E=({struct Cyc_Absyn_PtrInfo _tmpB4;_tmpB4.elt_typ=_tmpBB;_tmpB4.elt_tq=_tmpBA;({struct Cyc_Absyn_PtrAtts _tmp58D=({(_tmpB4.ptr_atts).rgn=_tmpB9;(_tmpB4.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmpB4.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref;(_tmpB4.ptr_atts).zero_term=_tmpB6;(_tmpB4.ptr_atts).ptrloc=0;_tmpB4.ptr_atts;});_tmpB4.ptr_atts=_tmp58D;});_tmpB4;});_tmpB3.f1=_tmp58E;});_tmpB3;});_tmpB2[0]=_tmp58F;});_tmpB2;});
# 467
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpB1,Cyc_Absyn_Other_coercion);
return(void*)_tmpB1;};}_LL8D:;};}else{_LL8B: _LL8C:
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
({struct Cyc_String_pa_PrintArg_struct _tmpBF;_tmpBF.tag=0;({struct _dyneither_ptr _tmp590=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 485
Cyc_Absynpp_typ2string(t2));_tmpBF.f1=_tmp590;});({struct Cyc_String_pa_PrintArg_struct _tmpBE;_tmpBE.tag=0;({struct _dyneither_ptr _tmp591=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpBE.f1=_tmp591;});({void*_tmpBC[2]={& _tmpBE,& _tmpBF};({unsigned int _tmp593=e1->loc;struct _dyneither_ptr _tmp592=({const char*_tmpBD="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpBD,sizeof(char),59);});Cyc_Tcutil_terr(_tmp593,_tmp592,_tag_dyneither(_tmpBC,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 488
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp594=t1_elt;Cyc_Tcutil_unify(_tmp594,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpC3;_tmpC3.tag=0;({struct _dyneither_ptr _tmp595=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 493
Cyc_Absynpp_typ2string(t2));_tmpC3.f1=_tmp595;});({struct Cyc_String_pa_PrintArg_struct _tmpC2;_tmpC2.tag=0;({struct _dyneither_ptr _tmp596=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpC2.f1=_tmp596;});({void*_tmpC0[2]={& _tmpC2,& _tmpC3};({unsigned int _tmp598=e1->loc;struct _dyneither_ptr _tmp597=({const char*_tmpC1="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpC1,sizeof(char),59);});Cyc_Tcutil_terr(_tmp598,_tmp597,_tag_dyneither(_tmpC0,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 497
({void*_tmpC4=0;({unsigned int _tmp59A=e1->loc;struct _dyneither_ptr _tmp599=({const char*_tmpC5="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpC5,sizeof(char),60);});Cyc_Tcutil_warn(_tmp59A,_tmp599,_tag_dyneither(_tmpC4,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmp59F=te;struct Cyc_Absyn_Exp*_tmp59E=e1;Cyc_Tcutil_unchecked_cast(_tmp59F,_tmp59E,({void*_tmp59D=t1_elt;void*_tmp59C=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp59B=
Cyc_Absyn_empty_tqual(0);
# 498
Cyc_Absyn_star_typ(_tmp59D,_tmp59C,_tmp59B,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});
# 501
return Cyc_Absyn_sint_typ;}else{
# 503
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpC6=0;({unsigned int _tmp5A1=e1->loc;struct _dyneither_ptr _tmp5A0=({const char*_tmpC7="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpC7,sizeof(char),50);});Cyc_Tcutil_terr(_tmp5A1,_tmp5A0,_tag_dyneither(_tmpC6,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpC8=0;({unsigned int _tmp5A3=e1->loc;struct _dyneither_ptr _tmp5A2=({const char*_tmpC9="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpC9,sizeof(char),54);});Cyc_Tcutil_terr(_tmp5A3,_tmp5A2,_tag_dyneither(_tmpC8,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpCD;_tmpCD.tag=0;({struct _dyneither_ptr _tmp5A4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpCD.f1=_tmp5A4;});({struct Cyc_String_pa_PrintArg_struct _tmpCC;_tmpCC.tag=0;({struct _dyneither_ptr _tmp5A5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpCC.f1=_tmp5A5;});({void*_tmpCA[2]={& _tmpCC,& _tmpCD};({unsigned int _tmp5A7=e2->loc;struct _dyneither_ptr _tmp5A6=({const char*_tmpCB="expecting either %s or int but found %s";_tag_dyneither(_tmpCB,sizeof(char),40);});Cyc_Tcutil_terr(_tmp5A7,_tmp5A6,_tag_dyneither(_tmpCA,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 512
return t1;}}}
# 516
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp5A8=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp5A8,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpCE=0;({unsigned int _tmp5AA=e1->loc;struct _dyneither_ptr _tmp5A9=({const char*_tmpCF="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpCF,sizeof(char),60);});Cyc_Tcutil_warn(_tmp5AA,_tmp5A9,_tag_dyneither(_tmpCE,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmp5AF=te;struct Cyc_Absyn_Exp*_tmp5AE=e2;Cyc_Tcutil_unchecked_cast(_tmp5AF,_tmp5AE,({void*_tmp5AD=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5AC=
Cyc_Tcutil_pointer_region(t2);
# 521
struct Cyc_Absyn_Tqual _tmp5AB=
# 523
Cyc_Absyn_empty_tqual(0);
# 521
Cyc_Absyn_star_typ(_tmp5AD,_tmp5AC,_tmp5AB,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});}
# 526
({void*_tmpD0=0;({unsigned int _tmp5B1=e1->loc;struct _dyneither_ptr _tmp5B0=({const char*_tmpD1="thin pointer subtraction!";_tag_dyneither(_tmpD1,sizeof(char),26);});Cyc_Tcutil_warn(_tmp5B1,_tmp5B0,_tag_dyneither(_tmpD0,sizeof(void*),0));});});
return Cyc_Absyn_sint_typ;}
# 529
({void*_tmpD2=0;({unsigned int _tmp5B3=e1->loc;struct _dyneither_ptr _tmp5B2=({const char*_tmpD3="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpD3,sizeof(char),56);});Cyc_Tcutil_warn(_tmp5B3,_tmp5B2,_tag_dyneither(_tmpD2,sizeof(void*),0));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 532
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpD4=0;({unsigned int _tmp5B5=e1->loc;struct _dyneither_ptr _tmp5B4=({const char*_tmpD5="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpD5,sizeof(char),51);});Cyc_Tcutil_warn(_tmp5B5,_tmp5B4,_tag_dyneither(_tmpD4,sizeof(void*),0));});});
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
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || ({
void*_tmp5B6=t1;Cyc_Tcutil_unify(_tmp5B6,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)));})){
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
struct _tuple0 _tmpD6=({struct _tuple0 _tmpDA;({void*_tmp5B7=Cyc_Tcutil_compress(t1);_tmpDA.f1=_tmp5B7;});({void*_tmp5B8=Cyc_Tcutil_compress(t2);_tmpDA.f2=_tmp5B8;});_tmpDA;});struct _tuple0 _tmpD7=_tmpD6;void*_tmpD9;void*_tmpD8;switch(*((int*)_tmpD7.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7.f2)->tag == 5){_LL93: _tmpD9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7.f1)->f1).elt_typ;_tmpD8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD7.f2)->f1).elt_typ;_LL94:
# 570
 if(Cyc_Tcutil_unify(_tmpD9,_tmpD8))return Cyc_Absyn_sint_typ;goto _LL92;}else{goto _LL97;}case 15: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD7.f2)->tag == 15){_LL95: _LL96:
# 572
 return Cyc_Absyn_sint_typ;}else{goto _LL97;}default: _LL97: _LL98:
 goto _LL92;}_LL92:;}
# 575
({struct Cyc_String_pa_PrintArg_struct _tmpDE;_tmpDE.tag=0;({struct _dyneither_ptr _tmp5B9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpDE.f1=_tmp5B9;});({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0;({struct _dyneither_ptr _tmp5BA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpDD.f1=_tmp5BA;});({void*_tmpDB[2]={& _tmpDD,& _tmpDE};({unsigned int _tmp5BC=loc;struct _dyneither_ptr _tmp5BB=({const char*_tmpDC="comparison not allowed between %s and %s";_tag_dyneither(_tmpDC,sizeof(char),41);});Cyc_Tcutil_terr(_tmp5BC,_tmp5BB,_tag_dyneither(_tmpDB,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 582
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 584
if(({void*_tmp5BD=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5BD == Cyc_Absyn_sint_typ;}))
return Cyc_Absyn_sint_typ;{
# 590
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpDF=({struct _tuple0 _tmpE3;_tmpE3.f1=t1;_tmpE3.f2=t2;_tmpE3;});struct _tuple0 _tmpE0=_tmpDF;void*_tmpE2;void*_tmpE1;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpE0.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpE0.f2)->tag == 15){_LL9A: _tmpE2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpE0.f1)->f1;_tmpE1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpE0.f2)->f1;_LL9B:
# 595
 return Cyc_Absyn_sint_typ;}else{goto _LL9C;}}else{_LL9C: _LL9D:
 goto _LL99;}_LL99:;}
# 598
({struct Cyc_String_pa_PrintArg_struct _tmpE7;_tmpE7.tag=0;({struct _dyneither_ptr _tmp5BE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpE7.f1=_tmp5BE;});({struct Cyc_String_pa_PrintArg_struct _tmpE6;_tmpE6.tag=0;({struct _dyneither_ptr _tmp5BF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpE6.f1=_tmp5BF;});({void*_tmpE4[2]={& _tmpE6,& _tmpE7};({unsigned int _tmp5C1=loc;struct _dyneither_ptr _tmp5C0=({const char*_tmpE5="comparison not allowed between %s and %s";_tag_dyneither(_tmpE5,sizeof(char),41);});Cyc_Tcutil_terr(_tmp5C1,_tmp5C0,_tag_dyneither(_tmpE4,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 606
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 608
enum Cyc_Absyn_Primop _tmpE8=p;switch(_tmpE8){case Cyc_Absyn_Plus: _LL9F: _LLA0:
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
({void*_tmpE9=0;({struct _dyneither_ptr _tmp5C2=({const char*_tmpEA="bad binary primop";_tag_dyneither(_tmpEA,sizeof(char),18);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5C2,_tag_dyneither(_tmpE9,sizeof(void*),0));});});}_LL9E:;}
# 637
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 645
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpEB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpEC=Cyc_Tcexp_tcExp(te,topt,_tmpEB);
if(!Cyc_Tcutil_is_numeric(_tmpEB))
({struct Cyc_String_pa_PrintArg_struct _tmpEF;_tmpEF.tag=0;({struct _dyneither_ptr _tmp5C3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpEC));_tmpEF.f1=_tmp5C3;});({void*_tmpED[1]={& _tmpEF};({unsigned int _tmp5C5=_tmpEB->loc;struct _dyneither_ptr _tmp5C4=({const char*_tmpEE="expecting numeric type but found %s";_tag_dyneither(_tmpEE,sizeof(char),36);});Cyc_Tcutil_terr(_tmp5C5,_tmp5C4,_tag_dyneither(_tmpED,sizeof(void*),1));});});});
return _tmpEC;}
# 652
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpF0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpF1=_tmpF0;switch(_tmpF1){case 0: _LLC4: _LLC5:
 return({void*_tmpF2=0;({struct Cyc_Tcenv_Tenv*_tmp5C9=te;unsigned int _tmp5C8=loc;void**_tmp5C7=topt;struct _dyneither_ptr _tmp5C6=({const char*_tmpF3="primitive operator has 0 arguments";_tag_dyneither(_tmpF3,sizeof(char),35);});Cyc_Tcexp_expr_err(_tmp5C9,_tmp5C8,_tmp5C7,_tmp5C6,_tag_dyneither(_tmpF2,sizeof(void*),0));});});case 1: _LLC6: _LLC7:
({void*_tmp5CA=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp5CA;});goto _LLC3;case 2: _LLC8: _LLC9:
({void*_tmp5D0=({struct Cyc_Tcenv_Tenv*_tmp5CF=te;unsigned int _tmp5CE=loc;void**_tmp5CD=topt;enum Cyc_Absyn_Primop _tmp5CC=p;struct Cyc_Absyn_Exp*_tmp5CB=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp5CF,_tmp5CE,_tmp5CD,_tmp5CC,_tmp5CB,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp5D0;});goto _LLC3;default: _LLCA: _LLCB:
 return({void*_tmpF4=0;({struct Cyc_Tcenv_Tenv*_tmp5D4=te;unsigned int _tmp5D3=loc;void**_tmp5D2=topt;struct _dyneither_ptr _tmp5D1=({const char*_tmpF5="primitive operator has > 2 arguments";_tag_dyneither(_tmpF5,sizeof(char),37);});Cyc_Tcexp_expr_err(_tmp5D4,_tmp5D3,_tmp5D2,_tmp5D1,_tag_dyneither(_tmpF4,sizeof(void*),0));});});}_LLC3:;}
# 660
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 663
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpF6=Cyc_Tcutil_compress(t);
void*_tmpF7=_tmpF6;struct Cyc_List_List*_tmp113;void*_tmp112;struct Cyc_Absyn_Tqual _tmp111;struct Cyc_Absyn_Datatypefield*_tmp110;struct Cyc_List_List*_tmp10F;struct Cyc_Absyn_Aggrdecl*_tmp10E;switch(*((int*)_tmpF7)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF7)->f1).aggr_info).KnownAggr).tag == 2){_LLCD: _tmp10E=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF7)->f1).aggr_info).KnownAggr).val;_LLCE:
# 667
 if(_tmp10E->impl == 0){
({void*_tmpF8=0;({unsigned int _tmp5D6=loc;struct _dyneither_ptr _tmp5D5=({const char*_tmpF9="attempt to write an abstract aggregate";_tag_dyneither(_tmpF9,sizeof(char),39);});Cyc_Tcutil_terr(_tmp5D6,_tmp5D5,_tag_dyneither(_tmpF8,sizeof(void*),0));});});
return 0;}else{
# 671
_tmp10F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp10E->impl))->fields;goto _LLD0;}}else{goto _LLD7;}case 12: _LLCF: _tmp10F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF7)->f2;_LLD0:
# 673
 for(0;_tmp10F != 0;_tmp10F=_tmp10F->tl){
struct Cyc_Absyn_Aggrfield*_tmpFA=(struct Cyc_Absyn_Aggrfield*)_tmp10F->hd;
if((_tmpFA->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0;_tmpFD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpFA->name);({void*_tmpFB[1]={& _tmpFD};({unsigned int _tmp5D8=loc;struct _dyneither_ptr _tmp5D7=({const char*_tmpFC="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpFC,sizeof(char),56);});Cyc_Tcutil_terr(_tmp5D8,_tmp5D7,_tag_dyneither(_tmpFB,sizeof(void*),1));});});});
return 0;}
# 679
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpFA->type))return 0;}
# 681
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF7)->f1).field_info).KnownDatatypefield).tag == 2){_LLD1: _tmp110=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF7)->f1).field_info).KnownDatatypefield).val).f2;_LLD2:
# 683
{struct Cyc_List_List*_tmpFE=_tmp110->typs;for(0;_tmpFE != 0;_tmpFE=_tmpFE->tl){
struct _tuple17*_tmpFF=(struct _tuple17*)_tmpFE->hd;struct _tuple17*_tmp100=_tmpFF;struct Cyc_Absyn_Tqual _tmp105;void*_tmp104;_LLDA: _tmp105=_tmp100->f1;_tmp104=_tmp100->f2;_LLDB:;
if(_tmp105.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0;({struct _dyneither_ptr _tmp5D9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp110->name));_tmp103.f1=_tmp5D9;});({void*_tmp101[1]={& _tmp103};({unsigned int _tmp5DB=loc;struct _dyneither_ptr _tmp5DA=({const char*_tmp102="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmp102,sizeof(char),64);});Cyc_Tcutil_terr(_tmp5DB,_tmp5DA,_tag_dyneither(_tmp101,sizeof(void*),1));});});});
return 0;}
# 689
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp104))return 0;}}
# 691
return 1;}else{goto _LLD7;}case 8: _LLD3: _tmp112=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).elt_type;_tmp111=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF7)->f1).tq;_LLD4:
# 693
 if(_tmp111.real_const){
({void*_tmp106=0;({unsigned int _tmp5DD=loc;struct _dyneither_ptr _tmp5DC=({const char*_tmp107="attempt to over-write a const array";_tag_dyneither(_tmp107,sizeof(char),36);});Cyc_Tcutil_terr(_tmp5DD,_tmp5DC,_tag_dyneither(_tmp106,sizeof(void*),0));});});
return 0;}
# 697
return Cyc_Tcexp_check_writable_aggr(loc,_tmp112);case 10: _LLD5: _tmp113=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF7)->f1;_LLD6:
# 699
 for(0;_tmp113 != 0;_tmp113=_tmp113->tl){
struct _tuple17*_tmp108=(struct _tuple17*)_tmp113->hd;struct _tuple17*_tmp109=_tmp108;struct Cyc_Absyn_Tqual _tmp10D;void*_tmp10C;_LLDD: _tmp10D=_tmp109->f1;_tmp10C=_tmp109->f2;_LLDE:;
if(_tmp10D.real_const){
({void*_tmp10A=0;({unsigned int _tmp5DF=loc;struct _dyneither_ptr _tmp5DE=({const char*_tmp10B="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmp10B,sizeof(char),56);});Cyc_Tcutil_terr(_tmp5DF,_tmp5DE,_tag_dyneither(_tmp10A,sizeof(void*),0));});});
return 0;}
# 705
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp10C))return 0;}
# 707
return 1;default: _LLD7: _LLD8:
 return 1;}_LLCC:;}
# 715
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 718
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp114=e->r;void*_tmp115=_tmp114;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct _dyneither_ptr*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct _dyneither_ptr*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Vardecl*_tmp13B;struct Cyc_Absyn_Vardecl*_tmp13A;struct Cyc_Absyn_Vardecl*_tmp139;struct Cyc_Absyn_Vardecl*_tmp138;switch(*((int*)_tmp115)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)){case 3: _LLE0: _tmp138=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)->f1;_LLE1:
 _tmp139=_tmp138;goto _LLE3;case 4: _LLE2: _tmp139=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)->f1;_LLE3:
 _tmp13A=_tmp139;goto _LLE5;case 5: _LLE4: _tmp13A=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)->f1;_LLE5:
 _tmp13B=_tmp13A;goto _LLE7;case 1: _LLE6: _tmp13B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp115)->f1)->f1;_LLE7:
 if(!(_tmp13B->tq).real_const)return;goto _LLDF;default: goto _LLF4;}case 23: _LLE8: _tmp13D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_tmp13C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp115)->f2;_LLE9:
# 725
{void*_tmp116=Cyc_Tcutil_compress((void*)_check_null(_tmp13D->topt));void*_tmp117=_tmp116;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_Tqual _tmp127;struct Cyc_Absyn_Tqual _tmp126;switch(*((int*)_tmp117)){case 5: _LLF7: _tmp126=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp117)->f1).elt_tq;_LLF8:
 _tmp127=_tmp126;goto _LLFA;case 8: _LLF9: _tmp127=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp117)->f1).tq;_LLFA:
 if(!_tmp127.real_const)return;goto _LLF6;case 10: _LLFB: _tmp128=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp117)->f1;_LLFC: {
# 729
struct _tuple15 _tmp118=Cyc_Evexp_eval_const_uint_exp(_tmp13C);struct _tuple15 _tmp119=_tmp118;unsigned int _tmp125;int _tmp124;_LL100: _tmp125=_tmp119.f1;_tmp124=_tmp119.f2;_LL101:;
if(!_tmp124){
({void*_tmp11A=0;({unsigned int _tmp5E1=e->loc;struct _dyneither_ptr _tmp5E0=({const char*_tmp11B="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp11B,sizeof(char),47);});Cyc_Tcutil_terr(_tmp5E1,_tmp5E0,_tag_dyneither(_tmp11A,sizeof(void*),0));});});
return;}
# 734
{struct _handler_cons _tmp11C;_push_handler(& _tmp11C);{int _tmp11E=0;if(setjmp(_tmp11C.handler))_tmp11E=1;if(!_tmp11E){
{struct _tuple17*_tmp11F=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp128,(int)_tmp125);struct _tuple17*_tmp120=_tmp11F;struct Cyc_Absyn_Tqual _tmp121;_LL103: _tmp121=_tmp120->f1;_LL104:;
if(!_tmp121.real_const){_npop_handler(0);return;}}
# 735
;_pop_handler();}else{void*_tmp11D=(void*)_exn_thrown;void*_tmp122=_tmp11D;void*_tmp123;if(((struct Cyc_List_Nth_exn_struct*)_tmp122)->tag == Cyc_List_Nth){_LL106: _LL107:
# 737
 return;}else{_LL108: _tmp123=_tmp122;_LL109:(int)_rethrow(_tmp123);}_LL105:;}};}
goto _LLF6;}default: _LLFD: _LLFE:
 goto _LLF6;}_LLF6:;}
# 741
goto _LLDF;case 21: _LLEA: _tmp13F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_tmp13E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp115)->f2;_LLEB:
# 743
{void*_tmp129=Cyc_Tcutil_compress((void*)_check_null(_tmp13F->topt));void*_tmp12A=_tmp129;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_Aggrdecl**_tmp12B;switch(*((int*)_tmp12A)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12A)->f1).aggr_info).KnownAggr).tag == 2){_LL10B: _tmp12B=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12A)->f1).aggr_info).KnownAggr).val;_LL10C: {
# 745
struct Cyc_Absyn_Aggrfield*sf=
_tmp12B == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp12B,_tmp13E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}}else{goto _LL10F;}case 12: _LL10D: _tmp12C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12A)->f2;_LL10E: {
# 750
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp12C,_tmp13E);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}default: _LL10F: _LL110:
 goto _LL10A;}_LL10A:;}
# 755
goto _LLDF;case 22: _LLEC: _tmp141=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_tmp140=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp115)->f2;_LLED:
# 757
{void*_tmp12D=Cyc_Tcutil_compress((void*)_check_null(_tmp141->topt));void*_tmp12E=_tmp12D;void*_tmp133;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E)->tag == 5){_LL112: _tmp133=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E)->f1).elt_typ;_LL113:
# 759
{void*_tmp12F=Cyc_Tcutil_compress(_tmp133);void*_tmp130=_tmp12F;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_Aggrdecl**_tmp131;switch(*((int*)_tmp130)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp130)->f1).aggr_info).KnownAggr).tag == 2){_LL117: _tmp131=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp130)->f1).aggr_info).KnownAggr).val;_LL118: {
# 761
struct Cyc_Absyn_Aggrfield*sf=
_tmp131 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp131,_tmp140);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}}else{goto _LL11B;}case 12: _LL119: _tmp132=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp130)->f2;_LL11A: {
# 766
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp132,_tmp140);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}default: _LL11B: _LL11C:
 goto _LL116;}_LL116:;}
# 771
goto _LL111;}else{_LL114: _LL115:
 goto _LL111;}_LL111:;}
# 774
goto _LLDF;case 20: _LLEE: _tmp142=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_LLEF:
# 776
{void*_tmp134=Cyc_Tcutil_compress((void*)_check_null(_tmp142->topt));void*_tmp135=_tmp134;struct Cyc_Absyn_Tqual _tmp137;struct Cyc_Absyn_Tqual _tmp136;switch(*((int*)_tmp135)){case 5: _LL11E: _tmp136=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp135)->f1).elt_tq;_LL11F:
 _tmp137=_tmp136;goto _LL121;case 8: _LL120: _tmp137=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp135)->f1).tq;_LL121:
 if(!_tmp137.real_const)return;goto _LL11D;default: _LL122: _LL123:
 goto _LL11D;}_LL11D:;}
# 781
goto _LLDF;case 12: _LLF0: _tmp143=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_LLF1:
 _tmp144=_tmp143;goto _LLF3;case 13: _LLF2: _tmp144=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp115)->f1;_LLF3:
 Cyc_Tcexp_check_writable(te,_tmp144);return;default: _LLF4: _LLF5:
 goto _LLDF;}_LLDF:;}
# 786
({struct Cyc_String_pa_PrintArg_struct _tmp147;_tmp147.tag=0;({struct _dyneither_ptr _tmp5E2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp147.f1=_tmp5E2;});({void*_tmp145[1]={& _tmp147};({unsigned int _tmp5E4=e->loc;struct _dyneither_ptr _tmp5E3=({const char*_tmp146="attempt to write a const location: %s";_tag_dyneither(_tmp146,sizeof(char),38);});Cyc_Tcutil_terr(_tmp5E4,_tmp5E3,_tag_dyneither(_tmp145,sizeof(void*),1));});});});}
# 789
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 792
({struct Cyc_Tcenv_Tenv*_tmp5E5=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp5E5,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp148=0;({struct Cyc_Tcenv_Tenv*_tmp5E9=te;unsigned int _tmp5E8=loc;void**_tmp5E7=topt;struct _dyneither_ptr _tmp5E6=({const char*_tmp149="increment/decrement of non-lvalue";_tag_dyneither(_tmp149,sizeof(char),34);});Cyc_Tcexp_expr_err(_tmp5E9,_tmp5E8,_tmp5E7,_tmp5E6,_tag_dyneither(_tmp148,sizeof(void*),0));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 798
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp14A=0;({unsigned int _tmp5EB=e->loc;struct _dyneither_ptr _tmp5EA=({const char*_tmp14B="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp14B,sizeof(char),50);});Cyc_Tcutil_terr(_tmp5EB,_tmp5EA,_tag_dyneither(_tmp14A,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp14C=0;({unsigned int _tmp5ED=e->loc;struct _dyneither_ptr _tmp5EC=({const char*_tmp14D="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp14D,sizeof(char),54);});Cyc_Tcutil_terr(_tmp5ED,_tmp5EC,_tag_dyneither(_tmp14C,sizeof(void*),0));});});}else{
# 807
({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0;({struct _dyneither_ptr _tmp5EE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp150.f1=_tmp5EE;});({void*_tmp14E[1]={& _tmp150};({unsigned int _tmp5F0=e->loc;struct _dyneither_ptr _tmp5EF=({const char*_tmp14F="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp14F,sizeof(char),44);});Cyc_Tcutil_terr(_tmp5F0,_tmp5EF,_tag_dyneither(_tmp14E,sizeof(void*),1));});});});}}
# 809
return t;};}
# 813
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 815
({struct Cyc_Tcenv_Tenv*_tmp5F2=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp5F1=e1;Cyc_Tcexp_tcTest(_tmp5F2,_tmp5F1,({const char*_tmp151="conditional expression";_tag_dyneither(_tmp151,sizeof(char),23);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp5F3=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp5F3;});else{
# 822
({void*_tmp5F4=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp5F4;});}{
struct Cyc_List_List _tmp152=({struct Cyc_List_List _tmp159;_tmp159.hd=e3;_tmp159.tl=0;_tmp159;});
struct Cyc_List_List _tmp153=({struct Cyc_List_List _tmp158;_tmp158.hd=e2;_tmp158.tl=& _tmp152;_tmp158;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp153)){
({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0;({struct _dyneither_ptr _tmp5F5=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp157.f1=_tmp5F5;});({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0;({struct _dyneither_ptr _tmp5F6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp156.f1=_tmp5F6;});({void*_tmp154[2]={& _tmp156,& _tmp157};({unsigned int _tmp5F8=loc;struct _dyneither_ptr _tmp5F7=({const char*_tmp155="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp155,sizeof(char),48);});Cyc_Tcutil_terr(_tmp5F8,_tmp5F7,_tag_dyneither(_tmp154,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 830
return t;};};}
# 834
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 836
({struct Cyc_Tcenv_Tenv*_tmp5FA=te;struct Cyc_Absyn_Exp*_tmp5F9=e1;Cyc_Tcexp_tcTest(_tmp5FA,_tmp5F9,({const char*_tmp15A="logical-and expression";_tag_dyneither(_tmp15A,sizeof(char),23);}));});
({struct Cyc_Tcenv_Tenv*_tmp5FC=te;struct Cyc_Absyn_Exp*_tmp5FB=e2;Cyc_Tcexp_tcTest(_tmp5FC,_tmp5FB,({const char*_tmp15B="logical-and expression";_tag_dyneither(_tmp15B,sizeof(char),23);}));});
return Cyc_Absyn_sint_typ;}
# 842
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 844
({struct Cyc_Tcenv_Tenv*_tmp5FE=te;struct Cyc_Absyn_Exp*_tmp5FD=e1;Cyc_Tcexp_tcTest(_tmp5FE,_tmp5FD,({const char*_tmp15C="logical-or expression";_tag_dyneither(_tmp15C,sizeof(char),22);}));});
({struct Cyc_Tcenv_Tenv*_tmp600=te;struct Cyc_Absyn_Exp*_tmp5FF=e2;Cyc_Tcexp_tcTest(_tmp600,_tmp5FF,({const char*_tmp15D="logical-or expression";_tag_dyneither(_tmp15D,sizeof(char),22);}));});
return Cyc_Absyn_sint_typ;}
# 850
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 857
({struct Cyc_Tcenv_Tenv*_tmp601=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp601,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 862
{void*_tmp15E=Cyc_Tcutil_compress(t1);void*_tmp15F=_tmp15E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15F)->tag == 8){_LL125: _LL126:
({void*_tmp160=0;({unsigned int _tmp603=loc;struct _dyneither_ptr _tmp602=({const char*_tmp161="cannot assign to an array";_tag_dyneither(_tmp161,sizeof(char),26);});Cyc_Tcutil_terr(_tmp603,_tmp602,_tag_dyneither(_tmp160,sizeof(void*),0));});});goto _LL124;}else{_LL127: _LL128:
 goto _LL124;}_LL124:;}
# 867
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp162=0;({unsigned int _tmp605=loc;struct _dyneither_ptr _tmp604=({const char*_tmp163="type is abstract (can't determine size).";_tag_dyneither(_tmp163,sizeof(char),41);});Cyc_Tcutil_terr(_tmp605,_tmp604,_tag_dyneither(_tmp162,sizeof(void*),0));});});
# 871
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp164=0;({struct Cyc_Tcenv_Tenv*_tmp609=te;unsigned int _tmp608=loc;void**_tmp607=topt;struct _dyneither_ptr _tmp606=({const char*_tmp165="assignment to non-lvalue";_tag_dyneither(_tmp165,sizeof(char),25);});Cyc_Tcexp_expr_err(_tmp609,_tmp608,_tmp607,_tmp606,_tag_dyneither(_tmp164,sizeof(void*),0));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp166=0;({unsigned int _tmp60B=e2->loc;struct _dyneither_ptr _tmp60A=({const char*_tmp167="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp167,sizeof(char),49);});Cyc_Tcutil_terr(_tmp60B,_tmp60A,_tag_dyneither(_tmp166,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp168=({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0;({struct _dyneither_ptr _tmp60C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp16C.f1=_tmp60C;});({struct Cyc_String_pa_PrintArg_struct _tmp16B;_tmp16B.tag=0;({struct _dyneither_ptr _tmp60D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp16B.f1=_tmp60D;});({void*_tmp169[2]={& _tmp16B,& _tmp16C};({struct Cyc_Tcenv_Tenv*_tmp611=te;unsigned int _tmp610=loc;void**_tmp60F=topt;struct _dyneither_ptr _tmp60E=({const char*_tmp16A="type mismatch: %s != %s";_tag_dyneither(_tmp16A,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp611,_tmp610,_tmp60F,_tmp60E,_tag_dyneither(_tmp169,sizeof(void*),2));});});});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp168;}}else{
# 885
enum Cyc_Absyn_Primop _tmp16D=(enum Cyc_Absyn_Primop)po->v;
void*_tmp16E=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp16D,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp16E,t1) || Cyc_Tcutil_coerceable(_tmp16E) && Cyc_Tcutil_coerceable(t1))){
void*_tmp16F=({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0;({struct _dyneither_ptr _tmp612=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 891
Cyc_Absynpp_typ2string(t2));_tmp173.f1=_tmp612;});({struct Cyc_String_pa_PrintArg_struct _tmp172;_tmp172.tag=0;({struct _dyneither_ptr _tmp613=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 890
Cyc_Absynpp_typ2string(t1));_tmp172.f1=_tmp613;});({void*_tmp170[2]={& _tmp172,& _tmp173};({struct Cyc_Tcenv_Tenv*_tmp617=te;unsigned int _tmp616=loc;void**_tmp615=topt;struct _dyneither_ptr _tmp614=({const char*_tmp171="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp171,sizeof(char),82);});Cyc_Tcexp_expr_err(_tmp617,_tmp616,_tmp615,_tmp614,_tag_dyneither(_tmp170,sizeof(void*),2));});});});});
# 892
Cyc_Tcutil_unify(_tmp16E,t1);
Cyc_Tcutil_explain_failure();
return _tmp16F;}
# 896
return _tmp16E;}
# 898
return t1;};};}
# 902
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp618=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp618,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp61A=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp619=topt;Cyc_Tcexp_tcExp(_tmp61A,_tmp619,e2);});
return(void*)_check_null(e2->topt);}
# 909
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 912
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 915
{void*_tmp174=Cyc_Tcutil_compress(t1);void*_tmp175=_tmp174;switch(*((int*)_tmp175)){case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp175)->f1 == 0){_LL12A: _LL12B:
({struct Cyc_Tcenv_Tenv*_tmp61C=te;struct Cyc_Absyn_Exp*_tmp61B=e;Cyc_Tcutil_unchecked_cast(_tmp61C,_tmp61B,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);});({void*_tmp61D=Cyc_Absyn_float_typ(1);t1=_tmp61D;});goto _LL129;}else{goto _LL130;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp175)->f2){case Cyc_Absyn_Char_sz: _LL12C: _LL12D:
 goto _LL12F;case Cyc_Absyn_Short_sz: _LL12E: _LL12F:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL129;default: goto _LL130;}default: _LL130: _LL131:
 goto _LL129;}_LL129:;}
# 922
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp176=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp177=_tmp176;struct _tuple1*_tmp17B;struct Cyc_List_List*_tmp17A;unsigned int _tmp179;enum Cyc_Absyn_Scope _tmp178;_LL133: _tmp17B=_tmp177.name;_tmp17A=_tmp177.typs;_tmp179=_tmp177.loc;_tmp178=_tmp177.sc;_LL134:;
# 925
if(_tmp17A == 0  || _tmp17A->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp17A->hd)).f2);
# 928
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 933
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp17C=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp17D=_tmp17C;struct _tuple1*_tmp181;struct Cyc_List_List*_tmp180;unsigned int _tmp17F;enum Cyc_Absyn_Scope _tmp17E;_LL136: _tmp181=_tmp17D.name;_tmp180=_tmp17D.typs;_tmp17F=_tmp17D.loc;_tmp17E=_tmp17D.sc;_LL137:;
# 936
if(_tmp180 == 0  || _tmp180->tl != 0)continue;{
void*t2=Cyc_Tcutil_substitute(inst,(*((struct _tuple17*)_tmp180->hd)).f2);
# 940
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 945
({struct Cyc_String_pa_PrintArg_struct _tmp185;_tmp185.tag=0;({struct _dyneither_ptr _tmp61E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));_tmp185.f1=_tmp61E;});({struct Cyc_String_pa_PrintArg_struct _tmp184;_tmp184.tag=0;({struct _dyneither_ptr _tmp61F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp184.f1=_tmp61F;});({void*_tmp182[2]={& _tmp184,& _tmp185};({unsigned int _tmp621=e->loc;struct _dyneither_ptr _tmp620=({const char*_tmp183="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp183,sizeof(char),54);});Cyc_Tcutil_terr(_tmp621,_tmp620,_tag_dyneither(_tmp182,sizeof(void*),2));});});});});
return 0;}
# 951
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 957
struct Cyc_List_List*_tmp186=args;
int _tmp187=0;
struct Cyc_Tcenv_Tenv*_tmp188=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp189=Cyc_Tcenv_clear_abstract_val_ok(_tmp188);
Cyc_Tcexp_tcExp(_tmp189,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 966
void*_tmp18A=t;void*_tmp1ED;struct Cyc_Absyn_Tqual _tmp1EC;void*_tmp1EB;union Cyc_Absyn_Constraint*_tmp1EA;union Cyc_Absyn_Constraint*_tmp1E9;union Cyc_Absyn_Constraint*_tmp1E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->tag == 5){_LL139: _tmp1ED=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).elt_typ;_tmp1EC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).elt_tq;_tmp1EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).ptr_atts).rgn;_tmp1EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).ptr_atts).nullable;_tmp1E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).ptr_atts).bounds;_tmp1E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18A)->f1).ptr_atts).zero_term;_LL13A:
# 971
 Cyc_Tcenv_check_rgn_accessible(_tmp189,loc,_tmp1EB);
# 973
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1E9);{
void*_tmp18B=Cyc_Tcutil_compress(_tmp1ED);void*_tmp18C=_tmp18B;struct Cyc_List_List*_tmp1E4;void*_tmp1E3;struct Cyc_Absyn_Tqual _tmp1E2;void*_tmp1E1;struct Cyc_List_List*_tmp1E0;int _tmp1DF;struct Cyc_Absyn_VarargInfo*_tmp1DE;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1D9;struct Cyc_List_List*_tmp1D8;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->tag == 9){_LL13E: _tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).tvars;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).effect;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ret_tqual;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ret_typ;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).args;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).c_varargs;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).cyc_varargs;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).rgn_po;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).attributes;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).requires_clause;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).requires_relns;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ensures_clause;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp18C)->f1).ensures_relns;_LL13F:
# 978
 if(_tmp1E4 != 0  || _tmp1DD != 0)
({void*_tmp18D=0;({unsigned int _tmp623=e->loc;struct _dyneither_ptr _tmp622=({const char*_tmp18E="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp18E,sizeof(char),79);});Cyc_Tcutil_terr(_tmp623,_tmp622,_tag_dyneither(_tmp18D,sizeof(void*),0));});});
# 982
if(topt != 0)Cyc_Tcutil_unify(_tmp1E1,*topt);
# 984
while(_tmp186 != 0  && _tmp1E0 != 0){
# 986
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp186->hd;
void*t2=(*((struct _tuple9*)_tmp1E0->hd)).f3;
Cyc_Tcexp_tcExp(_tmp189,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp189,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp19E="actual argument has type ";_tag_dyneither(_tmp19E,sizeof(char),26);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp19D=" but formal has type ";_tag_dyneither(_tmp19D,sizeof(char),22);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _tmp626=({unsigned long _tmp625=({unsigned long _tmp624=Cyc_strlen((struct _dyneither_ptr)s0);_tmp624 + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp625 + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp626 + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70)
({void*_tmp18F=0;({unsigned int _tmp629=e1->loc;struct _dyneither_ptr _tmp628=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp195;_tmp195.tag=0;_tmp195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp194;_tmp194.tag=0;_tmp194.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp193;_tmp193.tag=0;_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp192;_tmp192.tag=0;_tmp192.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp190[4]={& _tmp192,& _tmp193,& _tmp194,& _tmp195};({struct _dyneither_ptr _tmp627=({const char*_tmp191="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp191,sizeof(char),15);});Cyc_aprintf(_tmp627,_tag_dyneither(_tmp190,sizeof(void*),4));});});});});});});Cyc_Tcutil_terr(_tmp629,_tmp628,_tag_dyneither(_tmp18F,sizeof(void*),0));});});else{
# 998
({void*_tmp196=0;({unsigned int _tmp62C=e1->loc;struct _dyneither_ptr _tmp62B=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19C;_tmp19C.tag=0;_tmp19C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp19B;_tmp19B.tag=0;_tmp19B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp19A;_tmp19A.tag=0;_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0;_tmp199.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp197[4]={& _tmp199,& _tmp19A,& _tmp19B,& _tmp19C};({struct _dyneither_ptr _tmp62A=({const char*_tmp198="%s%s%s%s.";_tag_dyneither(_tmp198,sizeof(char),10);});Cyc_aprintf(_tmp62A,_tag_dyneither(_tmp197,sizeof(void*),4));});});});});});});Cyc_Tcutil_terr(_tmp62C,_tmp62B,_tag_dyneither(_tmp196,sizeof(void*),0));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1003
if(alias_coercion)
({struct Cyc_List_List*_tmp62D=({struct Cyc_List_List*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=(void*)_tmp187;_tmp19F->tl=*alias_arg_exps;_tmp19F;});*alias_arg_exps=_tmp62D;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1A0=0;({unsigned int _tmp62F=e1->loc;struct _dyneither_ptr _tmp62E=({const char*_tmp1A1="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1A1,sizeof(char),49);});Cyc_Tcutil_terr(_tmp62F,_tmp62E,_tag_dyneither(_tmp1A0,sizeof(void*),0));});});
_tmp186=_tmp186->tl;
_tmp1E0=_tmp1E0->tl;
++ _tmp187;}{
# 1014
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1DC;for(0;a != 0;a=a->tl){
void*_tmp1A2=(void*)a->hd;void*_tmp1A3=_tmp1A2;enum Cyc_Absyn_Format_Type _tmp1B0;int _tmp1AF;int _tmp1AE;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->tag == 19){_LL143: _tmp1B0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f1;_tmp1AF=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f2;_tmp1AE=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A3)->f3;_LL144:
# 1018
{struct _handler_cons _tmp1A4;_push_handler(& _tmp1A4);{int _tmp1A6=0;if(setjmp(_tmp1A4.handler))_tmp1A6=1;if(!_tmp1A6){
# 1020
{struct Cyc_Absyn_Exp*_tmp1A7=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1AF - 1);
# 1022
struct Cyc_Core_Opt*fmt_args;
if(_tmp1AE == 0)
fmt_args=0;else{
# 1026
({struct Cyc_Core_Opt*_tmp631=({struct Cyc_Core_Opt*_tmp1A8=_cycalloc(sizeof(*_tmp1A8));({struct Cyc_List_List*_tmp630=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1AE - 1);_tmp1A8->v=_tmp630;});_tmp1A8;});fmt_args=_tmp631;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1A9=_tmp1B0;if(_tmp1A9 == Cyc_Absyn_Printf_ft){_LL148: _LL149:
# 1030
 Cyc_Tcexp_check_format_args(_tmp189,_tmp1A7,fmt_args,_tmp1AE - 1,alias_arg_exps,Cyc_Formatstr_get_format_typs);
# 1033
goto _LL147;}else{_LL14A: _LL14B:
# 1035
 Cyc_Tcexp_check_format_args(_tmp189,_tmp1A7,fmt_args,_tmp1AE - 1,alias_arg_exps,Cyc_Formatstr_get_scanf_typs);
# 1038
goto _LL147;}_LL147:;};}
# 1020
;_pop_handler();}else{void*_tmp1A5=(void*)_exn_thrown;void*_tmp1AA=_tmp1A5;void*_tmp1AD;if(((struct Cyc_List_Nth_exn_struct*)_tmp1AA)->tag == Cyc_List_Nth){_LL14D: _LL14E:
# 1040
({void*_tmp1AB=0;({unsigned int _tmp633=loc;struct _dyneither_ptr _tmp632=({const char*_tmp1AC="bad format arguments";_tag_dyneither(_tmp1AC,sizeof(char),21);});Cyc_Tcutil_terr(_tmp633,_tmp632,_tag_dyneither(_tmp1AB,sizeof(void*),0));});});goto _LL14C;}else{_LL14F: _tmp1AD=_tmp1AA;_LL150:(int)_rethrow(_tmp1AD);}_LL14C:;}};}
goto _LL142;}else{_LL145: _LL146:
 goto _LL142;}_LL142:;}}
# 1045
if(_tmp1E0 != 0)({void*_tmp1B1=0;({unsigned int _tmp635=loc;struct _dyneither_ptr _tmp634=({const char*_tmp1B2="too few arguments for function";_tag_dyneither(_tmp1B2,sizeof(char),31);});Cyc_Tcutil_terr(_tmp635,_tmp634,_tag_dyneither(_tmp1B1,sizeof(void*),0));});});else{
# 1047
if((_tmp186 != 0  || _tmp1DF) || _tmp1DE != 0){
if(_tmp1DF)
for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
Cyc_Tcexp_tcExp(_tmp189,0,(struct Cyc_Absyn_Exp*)_tmp186->hd);}else{
if(_tmp1DE == 0)
({void*_tmp1B3=0;({unsigned int _tmp637=loc;struct _dyneither_ptr _tmp636=({const char*_tmp1B4="too many arguments for function";_tag_dyneither(_tmp1B4,sizeof(char),32);});Cyc_Tcutil_terr(_tmp637,_tmp636,_tag_dyneither(_tmp1B3,sizeof(void*),0));});});else{
# 1054
struct Cyc_Absyn_VarargInfo _tmp1B5=*_tmp1DE;struct Cyc_Absyn_VarargInfo _tmp1B6=_tmp1B5;void*_tmp1D4;int _tmp1D3;_LL152: _tmp1D4=_tmp1B6.type;_tmp1D3=_tmp1B6.inject;_LL153:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1B7=({struct Cyc_Absyn_VarargCallInfo*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->num_varargs=0;_tmp1D2->injectors=0;_tmp1D2->vai=_tmp1DE;_tmp1D2;});
# 1058
*vararg_call_info=_tmp1B7;
# 1060
if(!_tmp1D3)
# 1062
for(0;_tmp186 != 0;(_tmp186=_tmp186->tl,_tmp187 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp186->hd;
++ _tmp1B7->num_varargs;
Cyc_Tcexp_tcExp(_tmp189,& _tmp1D4,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp189,e1,_tmp1D4,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1BB;_tmp1BB.tag=0;({struct _dyneither_ptr _tmp638=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp1BB.f1=_tmp638;});({struct Cyc_String_pa_PrintArg_struct _tmp1BA;_tmp1BA.tag=0;({struct _dyneither_ptr _tmp639=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D4));_tmp1BA.f1=_tmp639;});({void*_tmp1B8[2]={& _tmp1BA,& _tmp1BB};({unsigned int _tmp63B=loc;struct _dyneither_ptr _tmp63A=({const char*_tmp1B9="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1B9,sizeof(char),49);});Cyc_Tcutil_terr(_tmp63B,_tmp63A,_tag_dyneither(_tmp1B8,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 1072
if(alias_coercion)
({struct Cyc_List_List*_tmp63C=({struct Cyc_List_List*_tmp1BC=_cycalloc(sizeof(*_tmp1BC));_tmp1BC->hd=(void*)_tmp187;_tmp1BC->tl=*alias_arg_exps;_tmp1BC;});*alias_arg_exps=_tmp63C;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1D4) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1BD=0;({unsigned int _tmp63E=e1->loc;struct _dyneither_ptr _tmp63D=({const char*_tmp1BE="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1BE,sizeof(char),49);});Cyc_Tcutil_terr(_tmp63E,_tmp63D,_tag_dyneither(_tmp1BD,sizeof(void*),0));});});}else{
# 1081
void*_tmp1BF=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1D4));void*_tmp1C0=_tmp1BF;struct Cyc_Absyn_Datatypedecl*_tmp1D1;struct Cyc_List_List*_tmp1D0;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C0)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C0)->f1).datatype_info).KnownDatatype).tag == 2){_LL155: _tmp1D1=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C0)->f1).datatype_info).KnownDatatype).val;_tmp1D0=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C0)->f1).targs;_LL156: {
# 1085
struct Cyc_Absyn_Datatypedecl*_tmp1C1=*Cyc_Tcenv_lookup_datatypedecl(_tmp189,loc,_tmp1D1->name);
struct Cyc_List_List*fields=0;
if(_tmp1C1->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1C4;_tmp1C4.tag=0;({struct _dyneither_ptr _tmp63F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1D4));_tmp1C4.f1=_tmp63F;});({void*_tmp1C2[1]={& _tmp1C4};({unsigned int _tmp641=loc;struct _dyneither_ptr _tmp640=({const char*_tmp1C3="can't inject into abstract datatype %s";_tag_dyneither(_tmp1C3,sizeof(char),39);});Cyc_Tcutil_terr(_tmp641,_tmp640,_tag_dyneither(_tmp1C2,sizeof(void*),1));});});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C1->fields))->v;}
# 1096
if(!({void*_tmp642=Cyc_Tcutil_pointer_region(_tmp1D4);Cyc_Tcutil_unify(_tmp642,Cyc_Tcenv_curr_rgn(_tmp189));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1C8;_tmp1C8.tag=0;({struct _dyneither_ptr _tmp643=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp189)));_tmp1C8.f1=_tmp643;});({struct Cyc_String_pa_PrintArg_struct _tmp1C7;_tmp1C7.tag=0;({struct _dyneither_ptr _tmp644=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1D4));_tmp1C7.f1=_tmp644;});({void*_tmp1C5[2]={& _tmp1C7,& _tmp1C8};({unsigned int _tmp646=loc;struct _dyneither_ptr _tmp645=({const char*_tmp1C6="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1C6,sizeof(char),49);});Cyc_Tcutil_terr(_tmp646,_tmp645,_tag_dyneither(_tmp1C5,sizeof(void*),2));});});});});{
# 1100
struct Cyc_List_List*_tmp1C9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1C1->tvs,_tmp1D0);
for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
++ _tmp1B7->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp186->hd;
# 1105
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp189,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1CA=0;({unsigned int _tmp648=e1->loc;struct _dyneither_ptr _tmp647=({const char*_tmp1CB="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1CB,sizeof(char),49);});Cyc_Tcutil_terr(_tmp648,_tmp647,_tag_dyneither(_tmp1CA,sizeof(void*),0));});});}{
# 1111
struct Cyc_Absyn_Datatypefield*_tmp1CC=({struct Cyc_Tcenv_Tenv*_tmp64C=_tmp189;struct Cyc_Absyn_Exp*_tmp64B=e1;void*_tmp64A=Cyc_Tcutil_pointer_elt_type(_tmp1D4);struct Cyc_List_List*_tmp649=_tmp1C9;Cyc_Tcexp_tcInjection(_tmp64C,_tmp64B,_tmp64A,_tmp649,fields);});
if(_tmp1CC != 0)
({struct Cyc_List_List*_tmp64E=({
struct Cyc_List_List*_tmp64D=_tmp1B7->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp64D,({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=_tmp1CC;_tmp1CD->tl=0;_tmp1CD;}));});
# 1113
_tmp1B7->injectors=_tmp64E;});};};}
# 1117
goto _LL154;};}}else{goto _LL157;}}else{_LL157: _LL158:
({void*_tmp1CE=0;({unsigned int _tmp650=loc;struct _dyneither_ptr _tmp64F=({const char*_tmp1CF="bad inject vararg type";_tag_dyneither(_tmp1CF,sizeof(char),23);});Cyc_Tcutil_terr(_tmp650,_tmp64F,_tag_dyneither(_tmp1CE,sizeof(void*),0));});});goto _LL154;}_LL154:;}};}}}}
# 1123
if(*alias_arg_exps == 0)
# 1132 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp189,loc,(void*)_check_null(_tmp1E3));
# 1134
return _tmp1E1;};}else{_LL140: _LL141:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1D7;_tmp1D7.tag=0;({struct _dyneither_ptr _tmp651=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1D7.f1=_tmp651;});({void*_tmp1D5[1]={& _tmp1D7};({struct Cyc_Tcenv_Tenv*_tmp655=_tmp189;unsigned int _tmp654=loc;void**_tmp653=topt;struct _dyneither_ptr _tmp652=({const char*_tmp1D6="expected pointer to function but found %s";_tag_dyneither(_tmp1D6,sizeof(char),42);});Cyc_Tcexp_expr_err(_tmp655,_tmp654,_tmp653,_tmp652,_tag_dyneither(_tmp1D5,sizeof(void*),1));});});});}_LL13D:;};}else{_LL13B: _LL13C:
# 1137
 return({struct Cyc_String_pa_PrintArg_struct _tmp1E7;_tmp1E7.tag=0;({struct _dyneither_ptr _tmp656=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1E7.f1=_tmp656;});({void*_tmp1E5[1]={& _tmp1E7};({struct Cyc_Tcenv_Tenv*_tmp65A=_tmp189;unsigned int _tmp659=loc;void**_tmp658=topt;struct _dyneither_ptr _tmp657=({const char*_tmp1E6="expected pointer to function but found %s";_tag_dyneither(_tmp1E6,sizeof(char),42);});Cyc_Tcexp_expr_err(_tmp65A,_tmp659,_tmp658,_tmp657,_tag_dyneither(_tmp1E5,sizeof(void*),1));});});});}_LL138:;};}
# 1141
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1EE=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1EE,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1EE,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1F2;_tmp1F2.tag=0;({struct _dyneither_ptr _tmp65B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp1F2.f1=_tmp65B;});({struct Cyc_String_pa_PrintArg_struct _tmp1F1;_tmp1F1.tag=0;({struct _dyneither_ptr _tmp65C=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1146
Cyc_Absynpp_typ2string(_tmp1EE));_tmp1F1.f1=_tmp65C;});({void*_tmp1EF[2]={& _tmp1F1,& _tmp1F2};({unsigned int _tmp65E=loc;struct _dyneither_ptr _tmp65D=({const char*_tmp1F0="expected %s but found %s";_tag_dyneither(_tmp1F0,sizeof(char),25);});Cyc_Tcutil_terr(_tmp65E,_tmp65D,_tag_dyneither(_tmp1EF,sizeof(void*),2));});});});});
# 1148
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1153
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1155
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1157
({void*_tmp65F=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp65F;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1F3=t1;void*_tmp212;struct Cyc_Absyn_Tqual _tmp211;void*_tmp210;union Cyc_Absyn_Constraint*_tmp20F;union Cyc_Absyn_Constraint*_tmp20E;union Cyc_Absyn_Constraint*_tmp20D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->tag == 5){_LL15A: _tmp212=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).elt_typ;_tmp211=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).elt_tq;_tmp210=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).ptr_atts).rgn;_tmp20F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).ptr_atts).nullable;_tmp20E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).ptr_atts).bounds;_tmp20D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F3)->f1).ptr_atts).zero_term;_LL15B:
# 1161
{void*_tmp1F4=Cyc_Tcutil_compress(_tmp212);void*_tmp1F5=_tmp1F4;struct Cyc_List_List*_tmp20C;void*_tmp20B;struct Cyc_Absyn_Tqual _tmp20A;void*_tmp209;struct Cyc_List_List*_tmp208;int _tmp207;struct Cyc_Absyn_VarargInfo*_tmp206;struct Cyc_List_List*_tmp205;struct Cyc_List_List*_tmp204;struct Cyc_Absyn_Exp*_tmp203;struct Cyc_List_List*_tmp202;struct Cyc_Absyn_Exp*_tmp201;struct Cyc_List_List*_tmp200;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->tag == 9){_LL15F: _tmp20C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).tvars;_tmp20B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).effect;_tmp20A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).ret_tqual;_tmp209=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).ret_typ;_tmp208=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).args;_tmp207=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).c_varargs;_tmp206=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).cyc_varargs;_tmp205=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).rgn_po;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).attributes;_tmp203=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).requires_clause;_tmp202=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).requires_relns;_tmp201=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).ensures_clause;_tmp200=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1F5)->f1).ensures_relns;_LL160: {
# 1163
struct Cyc_List_List*instantiation=0;
# 1165
for(0;ts != 0  && _tmp20C != 0;(ts=ts->tl,_tmp20C=_tmp20C->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp20C->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp663=loc;struct Cyc_Tcenv_Tenv*_tmp662=te;struct Cyc_List_List*_tmp661=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp660=k;Cyc_Tcutil_check_type(_tmp663,_tmp662,_tmp661,_tmp660,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp665=({struct Cyc_List_List*_tmp1F6=_cycalloc(sizeof(*_tmp1F6));({struct _tuple13*_tmp664=({struct _tuple13*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));_tmp1F7->f1=(struct Cyc_Absyn_Tvar*)_tmp20C->hd;_tmp1F7->f2=(void*)ts->hd;_tmp1F7;});_tmp1F6->hd=_tmp664;});_tmp1F6->tl=instantiation;_tmp1F6;});instantiation=_tmp665;});}
# 1171
if(ts != 0)
return({void*_tmp1F8=0;({struct Cyc_Tcenv_Tenv*_tmp669=te;unsigned int _tmp668=loc;void**_tmp667=topt;struct _dyneither_ptr _tmp666=({const char*_tmp1F9="too many type variables in instantiation";_tag_dyneither(_tmp1F9,sizeof(char),41);});Cyc_Tcexp_expr_err(_tmp669,_tmp668,_tmp667,_tmp666,_tag_dyneither(_tmp1F8,sizeof(void*),0));});});
# 1176
if(_tmp20C == 0){
({struct Cyc_List_List*_tmp66A=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp205);_tmp205=_tmp66A;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp205);
_tmp205=0;}{
# 1181
void*new_fn_typ=({
struct Cyc_List_List*_tmp66D=instantiation;Cyc_Tcutil_substitute(_tmp66D,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1FD=_cycalloc(sizeof(*_tmp1FD));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp66C=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1FE;_tmp1FE.tag=9;({struct Cyc_Absyn_FnInfo _tmp66B=({struct Cyc_Absyn_FnInfo _tmp1FF;_tmp1FF.tvars=_tmp20C;_tmp1FF.effect=_tmp20B;_tmp1FF.ret_tqual=_tmp20A;_tmp1FF.ret_typ=_tmp209;_tmp1FF.args=_tmp208;_tmp1FF.c_varargs=_tmp207;_tmp1FF.cyc_varargs=_tmp206;_tmp1FF.rgn_po=_tmp205;_tmp1FF.attributes=_tmp204;_tmp1FF.requires_clause=_tmp203;_tmp1FF.requires_relns=_tmp202;_tmp1FF.ensures_clause=_tmp201;_tmp1FF.ensures_relns=_tmp200;_tmp1FF;});_tmp1FE.f1=_tmp66B;});_tmp1FE;});_tmp1FD[0]=_tmp66C;});_tmp1FD;}));});
# 1186
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1FA=_cycalloc(sizeof(*_tmp1FA));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp670=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1FB;_tmp1FB.tag=5;({struct Cyc_Absyn_PtrInfo _tmp66F=({struct Cyc_Absyn_PtrInfo _tmp1FC;_tmp1FC.elt_typ=new_fn_typ;_tmp1FC.elt_tq=_tmp211;({struct Cyc_Absyn_PtrAtts _tmp66E=({(_tmp1FC.ptr_atts).rgn=_tmp210;(_tmp1FC.ptr_atts).nullable=_tmp20F;(_tmp1FC.ptr_atts).bounds=_tmp20E;(_tmp1FC.ptr_atts).zero_term=_tmp20D;(_tmp1FC.ptr_atts).ptrloc=0;_tmp1FC.ptr_atts;});_tmp1FC.ptr_atts=_tmp66E;});_tmp1FC;});_tmp1FB.f1=_tmp66F;});_tmp1FB;});_tmp1FA[0]=_tmp670;});_tmp1FA;});};}}else{_LL161: _LL162:
 goto _LL15E;}_LL15E:;}
# 1189
goto _LL159;}else{_LL15C: _LL15D:
 goto _LL159;}_LL159:;}
# 1192
return({struct Cyc_String_pa_PrintArg_struct _tmp215;_tmp215.tag=0;({struct _dyneither_ptr _tmp671=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));_tmp215.f1=_tmp671;});({void*_tmp213[1]={& _tmp215};({struct Cyc_Tcenv_Tenv*_tmp675=te;unsigned int _tmp674=loc;void**_tmp673=topt;struct _dyneither_ptr _tmp672=({const char*_tmp214="expecting polymorphic type but found %s";_tag_dyneither(_tmp214,sizeof(char),40);});Cyc_Tcexp_expr_err(_tmp675,_tmp674,_tmp673,_tmp672,_tag_dyneither(_tmp213,sizeof(void*),1));});});});};}
# 1197
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1199
({unsigned int _tmp679=loc;struct Cyc_Tcenv_Tenv*_tmp678=te;struct Cyc_List_List*_tmp677=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp676=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1199
Cyc_Tcutil_check_type(_tmp679,_tmp678,_tmp677,_tmp676,1,0,t);});
# 1201
Cyc_Tcutil_check_no_qual(loc,t);
# 1203
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1208
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1215
Cyc_Tcutil_unify(t2,t);{
void*_tmp216=({struct Cyc_String_pa_PrintArg_struct _tmp21A;_tmp21A.tag=0;({struct _dyneither_ptr _tmp67A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp21A.f1=_tmp67A;});({struct Cyc_String_pa_PrintArg_struct _tmp219;_tmp219.tag=0;({struct _dyneither_ptr _tmp67B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp219.f1=_tmp67B;});({void*_tmp217[2]={& _tmp219,& _tmp21A};({struct Cyc_Tcenv_Tenv*_tmp67E=te;unsigned int _tmp67D=loc;struct _dyneither_ptr _tmp67C=({const char*_tmp218="cannot cast %s to %s";_tag_dyneither(_tmp218,sizeof(char),21);});Cyc_Tcexp_expr_err(_tmp67E,_tmp67D,& t,_tmp67C,_tag_dyneither(_tmp217,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return _tmp216;};}}}
# 1225
{struct _tuple0 _tmp21B=({struct _tuple0 _tmp226;_tmp226.f1=e->r;({void*_tmp67F=Cyc_Tcutil_compress(t);_tmp226.f2=_tmp67F;});_tmp226;});struct _tuple0 _tmp21C=_tmp21B;int _tmp225;union Cyc_Absyn_Constraint*_tmp224;union Cyc_Absyn_Constraint*_tmp223;union Cyc_Absyn_Constraint*_tmp222;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21C.f1)->tag == 34){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->tag == 5){_LL164: _tmp225=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp21C.f1)->f1).fat_result;_tmp224=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).nullable;_tmp223=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).bounds;_tmp222=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21C.f2)->f1).ptr_atts).zero_term;_LL165:
# 1229
 if((_tmp225  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp222)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp224)){
void*_tmp21D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp223);void*_tmp21E=_tmp21D;struct Cyc_Absyn_Exp*_tmp221;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp21E)->tag == 1){_LL169: _tmp221=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp21E)->f1;_LL16A:
# 1232
 if((Cyc_Evexp_eval_const_uint_exp(_tmp221)).f1 == 1)
({void*_tmp21F=0;({unsigned int _tmp681=loc;struct _dyneither_ptr _tmp680=({const char*_tmp220="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp220,sizeof(char),60);});Cyc_Tcutil_warn(_tmp681,_tmp680,_tag_dyneither(_tmp21F,sizeof(void*),0));});});
goto _LL168;}else{_LL16B: _LL16C:
 goto _LL168;}_LL168:;}
# 1238
goto _LL163;}else{goto _LL166;}}else{_LL166: _LL167:
 goto _LL163;}_LL163:;}
# 1241
return t;};}
# 1245
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1247
void**_tmp227=0;
struct Cyc_Absyn_Tqual _tmp228=Cyc_Absyn_empty_tqual(0);
int _tmp229=0;
if(topt != 0){
void*_tmp22A=Cyc_Tcutil_compress(*topt);void*_tmp22B=_tmp22A;void**_tmp22E;struct Cyc_Absyn_Tqual _tmp22D;union Cyc_Absyn_Constraint*_tmp22C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->tag == 5){_LL16E: _tmp22E=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_typ;_tmp22D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).elt_tq;_tmp22C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22B)->f1).ptr_atts).nullable;_LL16F:
# 1253
 _tmp227=_tmp22E;
_tmp228=_tmp22D;
({int _tmp682=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp22C);_tmp229=_tmp682;});
goto _LL16D;}else{_LL170: _LL171:
 goto _LL16D;}_LL16D:;}
# 1267
({struct Cyc_Tcenv_Tenv*_tmp684=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp683=_tmp227;Cyc_Tcexp_tcExpNoInst(_tmp684,_tmp683,e);});
# 1270
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp22F=0;({unsigned int _tmp686=e->loc;struct _dyneither_ptr _tmp685=({const char*_tmp230="Cannot take the address of an alias-free path";_tag_dyneither(_tmp230,sizeof(char),46);});Cyc_Tcutil_terr(_tmp686,_tmp685,_tag_dyneither(_tmp22F,sizeof(void*),0));});});
# 1276
{void*_tmp231=e->r;void*_tmp232=_tmp231;struct Cyc_Absyn_Exp*_tmp236;struct Cyc_Absyn_Exp*_tmp235;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->tag == 23){_LL173: _tmp236=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->f1;_tmp235=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp232)->f2;_LL174:
# 1278
{void*_tmp233=Cyc_Tcutil_compress((void*)_check_null(_tmp236->topt));void*_tmp234=_tmp233;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp234)->tag == 10){_LL178: _LL179:
 goto _LL177;}else{_LL17A: _LL17B:
# 1283
({void*_tmp687=(Cyc_Absyn_add_exp(_tmp236,_tmp235,0))->r;e0->r=_tmp687;});
return Cyc_Tcexp_tcPlus(te,_tmp236,_tmp235);}_LL177:;}
# 1286
goto _LL172;}else{_LL175: _LL176:
 goto _LL172;}_LL172:;}
# 1291
{void*_tmp237=e->r;void*_tmp238=_tmp237;switch(*((int*)_tmp238)){case 21: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp238)->f3 == 1){_LL17D: _LL17E:
 goto _LL180;}else{goto _LL181;}case 22: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp238)->f3 == 1){_LL17F: _LL180:
# 1294
({void*_tmp239=0;({unsigned int _tmp689=e->loc;struct _dyneither_ptr _tmp688=({const char*_tmp23A="cannot take the address of a @tagged union member";_tag_dyneither(_tmp23A,sizeof(char),50);});Cyc_Tcutil_terr(_tmp689,_tmp688,_tag_dyneither(_tmp239,sizeof(void*),0));});});
goto _LL17C;}else{goto _LL181;}default: _LL181: _LL182:
 goto _LL17C;}_LL17C:;}{
# 1300
struct _tuple14 _tmp23B=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp23C=_tmp23B;int _tmp240;void*_tmp23F;_LL184: _tmp240=_tmp23C.f1;_tmp23F=_tmp23C.f2;_LL185:;
# 1302
if(Cyc_Tcutil_is_noalias_region(_tmp23F,0))
({void*_tmp23D=0;({unsigned int _tmp68B=e->loc;struct _dyneither_ptr _tmp68A=({const char*_tmp23E="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp23E,sizeof(char),60);});Cyc_Tcutil_terr(_tmp68B,_tmp68A,_tag_dyneither(_tmp23D,sizeof(void*),0));});});{
# 1305
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp240){
tq.print_const=1;
tq.real_const=1;}{
# 1311
void*t=_tmp229?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp23F,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp23F,tq,Cyc_Absyn_false_conref);
return t;};};};}
# 1318
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1322
return Cyc_Absyn_uint_typ;
# 1324
({unsigned int _tmp68E=loc;struct Cyc_Tcenv_Tenv*_tmp68D=te;struct Cyc_List_List*_tmp68C=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp68E,_tmp68D,_tmp68C,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp243;_tmp243.tag=0;({struct _dyneither_ptr _tmp68F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp243.f1=_tmp68F;});({void*_tmp241[1]={& _tmp243};({unsigned int _tmp691=loc;struct _dyneither_ptr _tmp690=({const char*_tmp242="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp242,sizeof(char),55);});Cyc_Tcutil_terr(_tmp691,_tmp690,_tag_dyneither(_tmp241,sizeof(void*),1));});});});
if(topt != 0){
void*_tmp244=Cyc_Tcutil_compress(*topt);void*_tmp245=_tmp244;void*_tmp24B;void*_tmp24A;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp245)->tag == 19){_LL187: _tmp24B=_tmp245;_tmp24A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp245)->f1;_LL188: {
# 1333
struct Cyc_Absyn_Exp*_tmp246=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp247=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp248=_cycalloc(sizeof(*_tmp248));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp692=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp249;_tmp249.tag=18;_tmp249.f1=_tmp246;_tmp249;});_tmp248[0]=_tmp692;});_tmp248;});
if(Cyc_Tcutil_unify(_tmp24A,(void*)_tmp247))return _tmp24B;
goto _LL186;}}else{_LL189: _LL18A:
 goto _LL186;}_LL186:;}
# 1339
return Cyc_Absyn_uint_typ;}
# 1342
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1349
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1351
({unsigned int _tmp695=loc;struct Cyc_Tcenv_Tenv*_tmp694=te;struct Cyc_List_List*_tmp693=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp695,_tmp694,_tmp693,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp24C=fs;
void*_tmp24D=t_orig;
for(0;_tmp24C != 0;_tmp24C=_tmp24C->tl){
void*_tmp24E=(void*)_tmp24C->hd;
void*_tmp24F=_tmp24E;unsigned int _tmp289;struct _dyneither_ptr*_tmp288;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24F)->tag == 0){_LL18C: _tmp288=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp24F)->f1;_LL18D: {
# 1359
int bad_type=1;
{void*_tmp250=Cyc_Tcutil_compress(_tmp24D);void*_tmp251=_tmp250;struct Cyc_List_List*_tmp259;struct Cyc_Absyn_Aggrdecl**_tmp258;switch(*((int*)_tmp251)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).aggr_info).KnownAggr).tag == 2){_LL191: _tmp258=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp251)->f1).aggr_info).KnownAggr).val;_LL192:
# 1362
 if((*_tmp258)->impl == 0)goto _LL190;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp288,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp258)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp254;_tmp254.tag=0;_tmp254.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp288);({void*_tmp252[1]={& _tmp254};({unsigned int _tmp697=loc;struct _dyneither_ptr _tmp696=({const char*_tmp253="no field of struct/union has name %s";_tag_dyneither(_tmp253,sizeof(char),37);});Cyc_Tcutil_terr(_tmp697,_tmp696,_tag_dyneither(_tmp252,sizeof(void*),1));});});});else{
# 1367
_tmp24D=t2;}
bad_type=0;
goto _LL190;};}else{goto _LL195;}case 12: _LL193: _tmp259=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp251)->f2;_LL194: {
# 1371
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp288,_tmp259);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp257;_tmp257.tag=0;_tmp257.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp288);({void*_tmp255[1]={& _tmp257};({unsigned int _tmp699=loc;struct _dyneither_ptr _tmp698=({const char*_tmp256="no field of struct/union has name %s";_tag_dyneither(_tmp256,sizeof(char),37);});Cyc_Tcutil_terr(_tmp699,_tmp698,_tag_dyneither(_tmp255,sizeof(void*),1));});});});else{
# 1375
_tmp24D=t2;}
bad_type=0;
goto _LL190;}default: _LL195: _LL196:
 goto _LL190;}_LL190:;}
# 1380
if(bad_type){
if(_tmp24C == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp25C;_tmp25C.tag=0;({struct _dyneither_ptr _tmp69A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp25C.f1=_tmp69A;});({void*_tmp25A[1]={& _tmp25C};({unsigned int _tmp69C=loc;struct _dyneither_ptr _tmp69B=({const char*_tmp25B="%s is not a known struct/union type";_tag_dyneither(_tmp25B,sizeof(char),36);});Cyc_Tcutil_terr(_tmp69C,_tmp69B,_tag_dyneither(_tmp25A,sizeof(void*),1));});});});else{
# 1384
struct _dyneither_ptr _tmp25D=({struct Cyc_String_pa_PrintArg_struct _tmp270;_tmp270.tag=0;({struct _dyneither_ptr _tmp69D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp270.f1=_tmp69D;});({void*_tmp26E[1]={& _tmp270};({struct _dyneither_ptr _tmp69E=({const char*_tmp26F="(%s)";_tag_dyneither(_tmp26F,sizeof(char),5);});Cyc_aprintf(_tmp69E,_tag_dyneither(_tmp26E,sizeof(void*),1));});});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp24C;x=x->tl){
void*_tmp25E=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp25F=_tmp25E;unsigned int _tmp269;struct _dyneither_ptr*_tmp268;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->tag == 0){_LL198: _tmp268=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->f1;_LL199:
# 1389
({struct _dyneither_ptr _tmp6A0=({struct Cyc_String_pa_PrintArg_struct _tmp263;_tmp263.tag=0;_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp268);({struct Cyc_String_pa_PrintArg_struct _tmp262;_tmp262.tag=0;_tmp262.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp260[2]={& _tmp262,& _tmp263};({struct _dyneither_ptr _tmp69F=({const char*_tmp261="%s.%s";_tag_dyneither(_tmp261,sizeof(char),6);});Cyc_aprintf(_tmp69F,_tag_dyneither(_tmp260,sizeof(void*),2));});});});});_tmp25D=_tmp6A0;});goto _LL197;}else{_LL19A: _tmp269=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp25F)->f1;_LL19B:
# 1391
({struct _dyneither_ptr _tmp6A2=({struct Cyc_Int_pa_PrintArg_struct _tmp267;_tmp267.tag=1;_tmp267.f1=(unsigned long)((int)_tmp269);({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp264[2]={& _tmp266,& _tmp267};({struct _dyneither_ptr _tmp6A1=({const char*_tmp265="%s.%d";_tag_dyneither(_tmp265,sizeof(char),6);});Cyc_aprintf(_tmp6A1,_tag_dyneither(_tmp264,sizeof(void*),2));});});});});_tmp25D=_tmp6A2;});goto _LL197;}_LL197:;}
# 1393
({struct Cyc_String_pa_PrintArg_struct _tmp26D;_tmp26D.tag=0;({struct _dyneither_ptr _tmp6A3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp26D.f1=_tmp6A3;});({struct Cyc_String_pa_PrintArg_struct _tmp26C;_tmp26C.tag=0;_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp25D);({void*_tmp26A[2]={& _tmp26C,& _tmp26D};({unsigned int _tmp6A5=loc;struct _dyneither_ptr _tmp6A4=({const char*_tmp26B="%s == %s is not a struct/union type";_tag_dyneither(_tmp26B,sizeof(char),36);});Cyc_Tcutil_terr(_tmp6A5,_tmp6A4,_tag_dyneither(_tmp26A,sizeof(void*),2));});});});});}}
# 1396
goto _LL18B;}}else{_LL18E: _tmp289=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp24F)->f1;_LL18F: {
# 1398
int bad_type=1;
{void*_tmp271=Cyc_Tcutil_compress(_tmp24D);void*_tmp272=_tmp271;struct Cyc_Absyn_Datatypefield*_tmp284;struct Cyc_List_List*_tmp283;struct Cyc_List_List*_tmp282;struct Cyc_Absyn_Aggrdecl**_tmp281;switch(*((int*)_tmp272)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp272)->f1).aggr_info).KnownAggr).tag == 2){_LL19D: _tmp281=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp272)->f1).aggr_info).KnownAggr).val;_LL19E:
# 1401
 if((*_tmp281)->impl == 0)
goto _LL19C;
_tmp282=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp281)->impl))->fields;goto _LL1A0;}else{goto _LL1A5;}case 12: _LL19F: _tmp282=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp272)->f2;_LL1A0:
# 1405
 if(({int _tmp6A6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp282);_tmp6A6 <= _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp276;_tmp276.tag=1;_tmp276.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp275;_tmp275.tag=1;({unsigned long _tmp6A7=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp282);_tmp275.f1=_tmp6A7;});({void*_tmp273[2]={& _tmp275,& _tmp276};({unsigned int _tmp6A9=loc;struct _dyneither_ptr _tmp6A8=({const char*_tmp274="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp274,sizeof(char),46);});Cyc_Tcutil_terr(_tmp6A9,_tmp6A8,_tag_dyneither(_tmp273,sizeof(void*),2));});});});});else{
# 1409
({void*_tmp6AA=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp282,(int)_tmp289))->type;_tmp24D=_tmp6AA;});}
bad_type=0;
goto _LL19C;case 10: _LL1A1: _tmp283=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp272)->f1;_LL1A2:
# 1413
 if(({int _tmp6AB=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp283);_tmp6AB <= _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp27A;_tmp27A.tag=1;_tmp27A.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp279;_tmp279.tag=1;({unsigned long _tmp6AC=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp283);_tmp279.f1=_tmp6AC;});({void*_tmp277[2]={& _tmp279,& _tmp27A};({unsigned int _tmp6AE=loc;struct _dyneither_ptr _tmp6AD=({const char*_tmp278="tuple has too few components: %d <= %d";_tag_dyneither(_tmp278,sizeof(char),39);});Cyc_Tcutil_terr(_tmp6AE,_tmp6AD,_tag_dyneither(_tmp277,sizeof(void*),2));});});});});else{
# 1417
({void*_tmp6AF=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp283,(int)_tmp289)).f2;_tmp24D=_tmp6AF;});}
bad_type=0;
goto _LL19C;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp272)->f1).field_info).KnownDatatypefield).tag == 2){_LL1A3: _tmp284=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp272)->f1).field_info).KnownDatatypefield).val).f2;_LL1A4:
# 1421
 if(({int _tmp6B0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp284->typs);_tmp6B0 < _tmp289;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp27E;_tmp27E.tag=1;_tmp27E.f1=(unsigned long)((int)_tmp289);({struct Cyc_Int_pa_PrintArg_struct _tmp27D;_tmp27D.tag=1;({unsigned long _tmp6B1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp284->typs);_tmp27D.f1=_tmp6B1;});({void*_tmp27B[2]={& _tmp27D,& _tmp27E};({unsigned int _tmp6B3=loc;struct _dyneither_ptr _tmp6B2=({const char*_tmp27C="datatype field has too few components: %d < %d";_tag_dyneither(_tmp27C,sizeof(char),47);});Cyc_Tcutil_terr(_tmp6B3,_tmp6B2,_tag_dyneither(_tmp27B,sizeof(void*),2));});});});});else{
# 1425
if(_tmp289 != 0)
({void*_tmp6B4=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp284->typs,(int)(_tmp289 - 1))).f2;_tmp24D=_tmp6B4;});else{
if(_tmp24C->tl != 0)
({void*_tmp27F=0;({unsigned int _tmp6B6=loc;struct _dyneither_ptr _tmp6B5=({const char*_tmp280="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp280,sizeof(char),68);});Cyc_Tcutil_terr(_tmp6B6,_tmp6B5,_tag_dyneither(_tmp27F,sizeof(void*),0));});});}}
# 1430
bad_type=0;
goto _LL19C;}else{goto _LL1A5;}default: _LL1A5: _LL1A6:
 goto _LL19C;}_LL19C:;}
# 1434
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp287;_tmp287.tag=0;({struct _dyneither_ptr _tmp6B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp24D));_tmp287.f1=_tmp6B7;});({void*_tmp285[1]={& _tmp287};({unsigned int _tmp6B9=loc;struct _dyneither_ptr _tmp6B8=({const char*_tmp286="%s is not a known type";_tag_dyneither(_tmp286,sizeof(char),23);});Cyc_Tcutil_terr(_tmp6B9,_tmp6B8,_tag_dyneither(_tmp285,sizeof(void*),1));});});});
goto _LL18B;}}_LL18B:;}
# 1439
return Cyc_Absyn_uint_typ;};}
# 1443
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp28A=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp28A,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp28B=t;void*_tmp2A1;void*_tmp2A0;union Cyc_Absyn_Constraint*_tmp29F;union Cyc_Absyn_Constraint*_tmp29E;switch(*((int*)_tmp28B)){case 1: _LL1A8: _LL1A9: {
# 1449
struct Cyc_List_List*_tmp28C=Cyc_Tcenv_lookup_type_vars(_tmp28A);
void*_tmp28D=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp298=_cycalloc(sizeof(*_tmp298));_tmp298->v=_tmp28C;_tmp298;}));
void*_tmp28E=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp297=_cycalloc(sizeof(*_tmp297));_tmp297->v=_tmp28C;_tmp297;}));
union Cyc_Absyn_Constraint*_tmp28F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp290=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp291=({struct Cyc_Absyn_PtrAtts _tmp296;_tmp296.rgn=_tmp28E;({union Cyc_Absyn_Constraint*_tmp6BA=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp296.nullable=_tmp6BA;});_tmp296.bounds=_tmp28F;_tmp296.zero_term=_tmp290;_tmp296.ptrloc=0;_tmp296;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp292=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp293=_cycalloc(sizeof(*_tmp293));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6BD=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp294;_tmp294.tag=5;({struct Cyc_Absyn_PtrInfo _tmp6BC=({struct Cyc_Absyn_PtrInfo _tmp295;_tmp295.elt_typ=_tmp28D;({struct Cyc_Absyn_Tqual _tmp6BB=Cyc_Absyn_empty_tqual(0);_tmp295.elt_tq=_tmp6BB;});_tmp295.ptr_atts=_tmp291;_tmp295;});_tmp294.f1=_tmp6BC;});_tmp294;});_tmp293[0]=_tmp6BD;});_tmp293;});
Cyc_Tcutil_unify(t,(void*)_tmp292);
_tmp2A1=_tmp28D;_tmp2A0=_tmp28E;_tmp29F=_tmp28F;_tmp29E=_tmp290;goto _LL1AB;}case 5: _LL1AA: _tmp2A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B)->f1).elt_typ;_tmp2A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B)->f1).ptr_atts).rgn;_tmp29F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B)->f1).ptr_atts).bounds;_tmp29E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp28B)->f1).ptr_atts).zero_term;_LL1AB:
# 1459
 Cyc_Tcenv_check_rgn_accessible(_tmp28A,loc,_tmp2A0);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp29F);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2A1),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp28A))
({void*_tmp299=0;({unsigned int _tmp6BF=loc;struct _dyneither_ptr _tmp6BE=({const char*_tmp29A="can't dereference abstract pointer type";_tag_dyneither(_tmp29A,sizeof(char),40);});Cyc_Tcutil_terr(_tmp6BF,_tmp6BE,_tag_dyneither(_tmp299,sizeof(void*),0));});});
return _tmp2A1;default: _LL1AC: _LL1AD:
# 1465
 return({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0;({struct _dyneither_ptr _tmp6C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp29D.f1=_tmp6C0;});({void*_tmp29B[1]={& _tmp29D};({struct Cyc_Tcenv_Tenv*_tmp6C4=_tmp28A;unsigned int _tmp6C3=loc;void**_tmp6C2=topt;struct _dyneither_ptr _tmp6C1=({const char*_tmp29C="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp29C,sizeof(char),39);});Cyc_Tcexp_expr_err(_tmp6C4,_tmp6C3,_tmp6C2,_tmp6C1,_tag_dyneither(_tmp29B,sizeof(void*),1));});});});}_LL1A7:;};}
# 1470
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1474
({struct Cyc_Tcenv_Tenv*_tmp6C5=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6C5,0,e);});
# 1476
({int _tmp6C6=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6C6;});{
void*_tmp2A2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A3=_tmp2A2;enum Cyc_Absyn_AggrKind _tmp2BD;struct Cyc_List_List*_tmp2BC;struct Cyc_Absyn_Aggrdecl*_tmp2BB;struct Cyc_List_List*_tmp2BA;switch(*((int*)_tmp2A3)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A3)->f1).aggr_info).KnownAggr).tag == 2){_LL1AF: _tmp2BB=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A3)->f1).aggr_info).KnownAggr).val;_tmp2BA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2A3)->f1).targs;_LL1B0: {
# 1479
struct Cyc_Absyn_Aggrfield*_tmp2A4=Cyc_Absyn_lookup_decl_field(_tmp2BB,f);
if(_tmp2A4 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0;_tmp2A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0;({struct _dyneither_ptr _tmp6C7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2BB->name));_tmp2A7.f1=_tmp6C7;});({void*_tmp2A5[2]={& _tmp2A7,& _tmp2A8};({struct Cyc_Tcenv_Tenv*_tmp6CB=te;unsigned int _tmp6CA=loc;void**_tmp6C9=topt;struct _dyneither_ptr _tmp6C8=({const char*_tmp2A6="%s has no %s member";_tag_dyneither(_tmp2A6,sizeof(char),20);});Cyc_Tcexp_expr_err(_tmp6CB,_tmp6CA,_tmp6C9,_tmp6C8,_tag_dyneither(_tmp2A5,sizeof(void*),2));});});});});
# 1484
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BB->impl))->tagged)*is_tagged=1;{
void*t2=_tmp2A4->type;
if(_tmp2BA != 0){
struct Cyc_List_List*_tmp2A9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2BB->tvs,_tmp2BA);
({void*_tmp6CC=Cyc_Tcutil_substitute(_tmp2A9,_tmp2A4->type);t2=_tmp6CC;});}
# 1492
if((((_tmp2BB->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BB->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2A4->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0;_tmp2AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2AA[1]={& _tmp2AC};({struct Cyc_Tcenv_Tenv*_tmp6D0=te;unsigned int _tmp6CF=loc;void**_tmp6CE=topt;struct _dyneither_ptr _tmp6CD=({const char*_tmp2AB="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2AB,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6D0,_tmp6CF,_tmp6CE,_tmp6CD,_tag_dyneither(_tmp2AA,sizeof(void*),1));});});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2BB->impl))->exist_vars != 0){
# 1497
if(!({void*_tmp6D1=t2;Cyc_Tcutil_unify(_tmp6D1,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2AD[1]={& _tmp2AF};({struct Cyc_Tcenv_Tenv*_tmp6D5=te;unsigned int _tmp6D4=loc;void**_tmp6D3=topt;struct _dyneither_ptr _tmp6D2=({const char*_tmp2AE="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp2AE,sizeof(char),81);});Cyc_Tcexp_expr_err(_tmp6D5,_tmp6D4,_tmp6D3,_tmp6D2,_tag_dyneither(_tmp2AD,sizeof(void*),1));});});});}
# 1500
return t2;};}}else{goto _LL1B3;}case 12: _LL1B1: _tmp2BD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A3)->f1;_tmp2BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2A3)->f2;_LL1B2: {
# 1502
struct Cyc_Absyn_Aggrfield*_tmp2B0=Cyc_Absyn_lookup_field(_tmp2BC,f);
if(_tmp2B0 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0;_tmp2B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2B1[1]={& _tmp2B3};({struct Cyc_Tcenv_Tenv*_tmp6D9=te;unsigned int _tmp6D8=loc;void**_tmp6D7=topt;struct _dyneither_ptr _tmp6D6=({const char*_tmp2B2="type has no %s member";_tag_dyneither(_tmp2B2,sizeof(char),22);});Cyc_Tcexp_expr_err(_tmp6D9,_tmp6D8,_tmp6D7,_tmp6D6,_tag_dyneither(_tmp2B1,sizeof(void*),1));});});});
# 1507
if(((_tmp2BD == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2B0->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2B0->requires_clause == 0)
# 1509
return({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0;_tmp2B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2B4[1]={& _tmp2B6};({struct Cyc_Tcenv_Tenv*_tmp6DD=te;unsigned int _tmp6DC=loc;void**_tmp6DB=topt;struct _dyneither_ptr _tmp6DA=({const char*_tmp2B5="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2B5,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6DD,_tmp6DC,_tmp6DB,_tmp6DA,_tag_dyneither(_tmp2B4,sizeof(void*),1));});});});
return _tmp2B0->type;}default: _LL1B3: _LL1B4:
# 1512
 return({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0;({struct _dyneither_ptr _tmp6DE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp2B9.f1=_tmp6DE;});({void*_tmp2B7[1]={& _tmp2B9};({struct Cyc_Tcenv_Tenv*_tmp6E2=te;unsigned int _tmp6E1=loc;void**_tmp6E0=topt;struct _dyneither_ptr _tmp6DF=({const char*_tmp2B8="expecting struct or union, found %s";_tag_dyneither(_tmp2B8,sizeof(char),36);});Cyc_Tcexp_expr_err(_tmp6E2,_tmp6E1,_tmp6E0,_tmp6DF,_tag_dyneither(_tmp2B7,sizeof(void*),1));});});});}_LL1AE:;};}
# 1518
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1521
({struct Cyc_Tcenv_Tenv*_tmp6E3=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp6E3,0,e);});
# 1523
({int _tmp6E4=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6E4;});
{void*_tmp2BE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2BF=_tmp2BE;void*_tmp2E2;void*_tmp2E1;union Cyc_Absyn_Constraint*_tmp2E0;union Cyc_Absyn_Constraint*_tmp2DF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->tag == 5){_LL1B6: _tmp2E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).elt_typ;_tmp2E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).rgn;_tmp2E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).bounds;_tmp2DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).zero_term;_LL1B7:
# 1526
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2E0);
{void*_tmp2C0=Cyc_Tcutil_compress(_tmp2E2);void*_tmp2C1=_tmp2C0;enum Cyc_Absyn_AggrKind _tmp2DE;struct Cyc_List_List*_tmp2DD;struct Cyc_Absyn_Aggrdecl*_tmp2DC;struct Cyc_List_List*_tmp2DB;switch(*((int*)_tmp2C1)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C1)->f1).aggr_info).KnownAggr).tag == 2){_LL1BB: _tmp2DC=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C1)->f1).aggr_info).KnownAggr).val;_tmp2DB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2C1)->f1).targs;_LL1BC: {
# 1529
struct Cyc_Absyn_Aggrfield*_tmp2C2=Cyc_Absyn_lookup_decl_field(_tmp2DC,f);
if(_tmp2C2 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=0;({struct _dyneither_ptr _tmp6E5=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2DC->name));_tmp2C5.f1=_tmp6E5;});({void*_tmp2C3[2]={& _tmp2C5,& _tmp2C6};({struct Cyc_Tcenv_Tenv*_tmp6E9=te;unsigned int _tmp6E8=loc;void**_tmp6E7=topt;struct _dyneither_ptr _tmp6E6=({const char*_tmp2C4="type %s has no %s member";_tag_dyneither(_tmp2C4,sizeof(char),25);});Cyc_Tcexp_expr_err(_tmp6E9,_tmp6E8,_tmp6E7,_tmp6E6,_tag_dyneither(_tmp2C3,sizeof(void*),2));});});});});
# 1534
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DC->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2C2->type;
if(_tmp2DB != 0){
struct Cyc_List_List*_tmp2C7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2DC->tvs,_tmp2DB);
({void*_tmp6EA=Cyc_Tcutil_substitute(_tmp2C7,_tmp2C2->type);t3=_tmp6EA;});}{
# 1540
struct Cyc_Absyn_Kind*_tmp2C8=Cyc_Tcutil_typ_kind(t3);
# 1543
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2C8) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2C9=Cyc_Tcutil_compress(t3);void*_tmp2CA=_tmp2C9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2CA)->tag == 8){_LL1C2: _LL1C3:
 goto _LL1C1;}else{_LL1C4: _LL1C5:
# 1547
 return({struct Cyc_String_pa_PrintArg_struct _tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CB[1]={& _tmp2CD};({struct Cyc_Tcenv_Tenv*_tmp6EE=te;unsigned int _tmp6ED=loc;void**_tmp6EC=topt;struct _dyneither_ptr _tmp6EB=({const char*_tmp2CC="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2CC,sizeof(char),48);});Cyc_Tcexp_expr_err(_tmp6EE,_tmp6ED,_tmp6EC,_tmp6EB,_tag_dyneither(_tmp2CB,sizeof(void*),1));});});});}_LL1C1:;}
# 1552
if((((_tmp2DC->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DC->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2C2->requires_clause == 0)
# 1555
return({struct Cyc_String_pa_PrintArg_struct _tmp2D0;_tmp2D0.tag=0;_tmp2D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CE[1]={& _tmp2D0};({struct Cyc_Tcenv_Tenv*_tmp6F2=te;unsigned int _tmp6F1=loc;void**_tmp6F0=topt;struct _dyneither_ptr _tmp6EF=({const char*_tmp2CF="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2CF,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6F2,_tmp6F1,_tmp6F0,_tmp6EF,_tag_dyneither(_tmp2CE,sizeof(void*),1));});});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2DC->impl))->exist_vars != 0){
if(!({void*_tmp6F3=t3;Cyc_Tcutil_unify(_tmp6F3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D1[1]={& _tmp2D3};({struct Cyc_Tcenv_Tenv*_tmp6F7=te;unsigned int _tmp6F6=loc;void**_tmp6F5=topt;struct _dyneither_ptr _tmp6F4=({const char*_tmp2D2="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2D2,sizeof(char),72);});Cyc_Tcexp_expr_err(_tmp6F7,_tmp6F6,_tmp6F5,_tmp6F4,_tag_dyneither(_tmp2D1,sizeof(void*),1));});});});}
# 1562
return t3;};};}}else{goto _LL1BF;}case 12: _LL1BD: _tmp2DE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1)->f1;_tmp2DD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2C1)->f2;_LL1BE: {
# 1564
struct Cyc_Absyn_Aggrfield*_tmp2D4=Cyc_Absyn_lookup_field(_tmp2DD,f);
if(_tmp2D4 == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2D7;_tmp2D7.tag=0;_tmp2D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D5[1]={& _tmp2D7};({struct Cyc_Tcenv_Tenv*_tmp6FB=te;unsigned int _tmp6FA=loc;void**_tmp6F9=topt;struct _dyneither_ptr _tmp6F8=({const char*_tmp2D6="type has no %s field";_tag_dyneither(_tmp2D6,sizeof(char),21);});Cyc_Tcexp_expr_err(_tmp6FB,_tmp6FA,_tmp6F9,_tmp6F8,_tag_dyneither(_tmp2D5,sizeof(void*),1));});});});
# 1569
if((_tmp2DE == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2D4->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2DA;_tmp2DA.tag=0;_tmp2DA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D8[1]={& _tmp2DA};({struct Cyc_Tcenv_Tenv*_tmp6FF=te;unsigned int _tmp6FE=loc;void**_tmp6FD=topt;struct _dyneither_ptr _tmp6FC=({const char*_tmp2D9="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2D9,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6FF,_tmp6FE,_tmp6FD,_tmp6FC,_tag_dyneither(_tmp2D8,sizeof(void*),1));});});});
return _tmp2D4->type;}default: _LL1BF: _LL1C0:
 goto _LL1BA;}_LL1BA:;}
# 1574
goto _LL1B5;}else{_LL1B8: _LL1B9:
 goto _LL1B5;}_LL1B5:;}
# 1577
return({struct Cyc_String_pa_PrintArg_struct _tmp2E5;_tmp2E5.tag=0;({struct _dyneither_ptr _tmp700=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp2E5.f1=_tmp700;});({void*_tmp2E3[1]={& _tmp2E5};({struct Cyc_Tcenv_Tenv*_tmp704=te;unsigned int _tmp703=loc;void**_tmp702=topt;struct _dyneither_ptr _tmp701=({const char*_tmp2E4="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2E4,sizeof(char),44);});Cyc_Tcexp_expr_err(_tmp704,_tmp703,_tmp702,_tmp701,_tag_dyneither(_tmp2E3,sizeof(void*),1));});});});}
# 1582
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1584
struct _tuple15 _tmp2E6=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2E7=_tmp2E6;unsigned int _tmp2F5;int _tmp2F4;_LL1C7: _tmp2F5=_tmp2E7.f1;_tmp2F4=_tmp2E7.f2;_LL1C8:;
if(!_tmp2F4)
return({void*_tmp2E8=0;({struct Cyc_Tcenv_Tenv*_tmp707=te;unsigned int _tmp706=loc;struct _dyneither_ptr _tmp705=({const char*_tmp2E9="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2E9,sizeof(char),47);});Cyc_Tcexp_expr_err(_tmp707,_tmp706,0,_tmp705,_tag_dyneither(_tmp2E8,sizeof(void*),0));});});{
# 1588
struct _handler_cons _tmp2EA;_push_handler(& _tmp2EA);{int _tmp2EC=0;if(setjmp(_tmp2EA.handler))_tmp2EC=1;if(!_tmp2EC){
{void*_tmp2ED=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2F5)).f2;_npop_handler(0);return _tmp2ED;};_pop_handler();}else{void*_tmp2EB=(void*)_exn_thrown;void*_tmp2EE=_tmp2EB;void*_tmp2F3;if(((struct Cyc_List_Nth_exn_struct*)_tmp2EE)->tag == Cyc_List_Nth){_LL1CA: _LL1CB:
# 1591
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2F2;_tmp2F2.tag=1;({unsigned long _tmp708=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp2F2.f1=_tmp708;});({struct Cyc_Int_pa_PrintArg_struct _tmp2F1;_tmp2F1.tag=1;_tmp2F1.f1=(unsigned long)((int)_tmp2F5);({void*_tmp2EF[2]={& _tmp2F1,& _tmp2F2};({struct Cyc_Tcenv_Tenv*_tmp70B=te;unsigned int _tmp70A=loc;struct _dyneither_ptr _tmp709=({const char*_tmp2F0="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2F0,sizeof(char),41);});Cyc_Tcexp_expr_err(_tmp70B,_tmp70A,0,_tmp709,_tag_dyneither(_tmp2EF,sizeof(void*),2));});});});});}else{_LL1CC: _tmp2F3=_tmp2EE;_LL1CD:(int)_rethrow(_tmp2F3);}_LL1C9:;}};};}
# 1596
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1598
struct Cyc_Tcenv_Tenv*_tmp2F6=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp70C=Cyc_Tcenv_clear_abstract_val_ok(_tmp2F6);Cyc_Tcexp_tcExp(_tmp70C,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp70D=Cyc_Tcenv_clear_abstract_val_ok(_tmp2F6);Cyc_Tcexp_tcExp(_tmp70D,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2F6,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2F9;_tmp2F9.tag=0;({struct _dyneither_ptr _tmp70E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp2F9.f1=_tmp70E;});({void*_tmp2F7[1]={& _tmp2F9};({struct Cyc_Tcenv_Tenv*_tmp712=_tmp2F6;unsigned int _tmp711=e2->loc;void**_tmp710=topt;struct _dyneither_ptr _tmp70F=({const char*_tmp2F8="expecting int subscript, found %s";_tag_dyneither(_tmp2F8,sizeof(char),34);});Cyc_Tcexp_expr_err(_tmp712,_tmp711,_tmp710,_tmp70F,_tag_dyneither(_tmp2F7,sizeof(void*),1));});});});{
# 1608
void*_tmp2FA=t1;struct Cyc_List_List*_tmp318;void*_tmp317;struct Cyc_Absyn_Tqual _tmp316;void*_tmp315;union Cyc_Absyn_Constraint*_tmp314;union Cyc_Absyn_Constraint*_tmp313;switch(*((int*)_tmp2FA)){case 5: _LL1CF: _tmp317=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).elt_typ;_tmp316=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).elt_tq;_tmp315=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).rgn;_tmp314=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).bounds;_tmp313=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).zero_term;_LL1D0:
# 1612
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp313)){
int emit_warning=1;
{void*_tmp2FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp314);void*_tmp2FC=_tmp2FB;struct Cyc_Absyn_Exp*_tmp305;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FC)->tag == 1){_LL1D6: _tmp305=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2FC)->f1;_LL1D7:
# 1616
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2FD=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2FE=_tmp2FD;unsigned int _tmp304;int _tmp303;_LL1DB: _tmp304=_tmp2FE.f1;_tmp303=_tmp2FE.f2;_LL1DC:;
if(_tmp303){
struct _tuple15 _tmp2FF=Cyc_Evexp_eval_const_uint_exp(_tmp305);struct _tuple15 _tmp300=_tmp2FF;unsigned int _tmp302;int _tmp301;_LL1DE: _tmp302=_tmp300.f1;_tmp301=_tmp300.f2;_LL1DF:;
if(_tmp301  && _tmp302 > _tmp304)emit_warning=0;}}
# 1623
goto _LL1D5;}else{_LL1D8: _LL1D9:
 emit_warning=0;goto _LL1D5;}_LL1D5:;}
# 1626
if(emit_warning)
({void*_tmp306=0;({unsigned int _tmp714=e2->loc;struct _dyneither_ptr _tmp713=({const char*_tmp307="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp307,sizeof(char),63);});Cyc_Tcutil_warn(_tmp714,_tmp713,_tag_dyneither(_tmp306,sizeof(void*),0));});});}else{
# 1630
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp308=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp309=_tmp308;unsigned int _tmp30B;int _tmp30A;_LL1E1: _tmp30B=_tmp309.f1;_tmp30A=_tmp309.f2;_LL1E2:;
if(_tmp30A)
Cyc_Tcutil_check_bound(loc,_tmp30B,_tmp314);}else{
# 1636
if(Cyc_Tcutil_is_bound_one(_tmp314) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp313))
({void*_tmp30C=0;({unsigned int _tmp716=e1->loc;struct _dyneither_ptr _tmp715=({const char*_tmp30D="subscript applied to pointer to one object";_tag_dyneither(_tmp30D,sizeof(char),43);});Cyc_Tcutil_warn(_tmp716,_tmp715,_tag_dyneither(_tmp30C,sizeof(void*),0));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp314);}}
# 1641
Cyc_Tcenv_check_rgn_accessible(_tmp2F6,loc,_tmp315);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp317),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp2F6))
({void*_tmp30E=0;({unsigned int _tmp718=e1->loc;struct _dyneither_ptr _tmp717=({const char*_tmp30F="can't subscript an abstract pointer";_tag_dyneither(_tmp30F,sizeof(char),36);});Cyc_Tcutil_terr(_tmp718,_tmp717,_tag_dyneither(_tmp30E,sizeof(void*),0));});});
return _tmp317;case 10: _LL1D1: _tmp318=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2FA)->f1;_LL1D2:
 return Cyc_Tcexp_ithTupleType(_tmp2F6,loc,_tmp318,e2);default: _LL1D3: _LL1D4:
 return({struct Cyc_String_pa_PrintArg_struct _tmp312;_tmp312.tag=0;({struct _dyneither_ptr _tmp719=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp312.f1=_tmp719;});({void*_tmp310[1]={& _tmp312};({struct Cyc_Tcenv_Tenv*_tmp71D=_tmp2F6;unsigned int _tmp71C=loc;void**_tmp71B=topt;struct _dyneither_ptr _tmp71A=({const char*_tmp311="subscript applied to %s";_tag_dyneither(_tmp311,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp71D,_tmp71C,_tmp71B,_tmp71A,_tag_dyneither(_tmp310,sizeof(void*),1));});});});}_LL1CE:;};};}
# 1651
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp319=Cyc_Tcutil_compress(*topt);void*_tmp31A=_tmp319;struct Cyc_List_List*_tmp31E;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp31A)->tag == 10){_LL1E4: _tmp31E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp31A)->f1;_LL1E5:
# 1657
 if(({int _tmp71E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp31E);_tmp71E != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1660
goto _LL1E3;
# 1662
for(0;es != 0;(es=es->tl,_tmp31E=_tmp31E->tl)){
int bogus=0;
void*_tmp31B=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp31E))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp71F=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp71F,& _tmp31B,(struct Cyc_Absyn_Exp*)es->hd);});
# 1667
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp31E->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp721=({struct Cyc_List_List*_tmp31C=_cycalloc(sizeof(*_tmp31C));({struct _tuple17*_tmp720=({struct _tuple17*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->f1=(*((struct _tuple17*)_tmp31E->hd)).f1;_tmp31D->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp31D;});_tmp31C->hd=_tmp720;});_tmp31C->tl=fields;_tmp31C;});fields=_tmp721;});}
# 1670
done=1;
goto _LL1E3;}else{_LL1E6: _LL1E7:
 goto _LL1E3;}_LL1E3:;}
# 1674
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp722=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp722,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp725=({struct Cyc_List_List*_tmp31F=_cycalloc(sizeof(*_tmp31F));({struct _tuple17*_tmp724=({struct _tuple17*_tmp320=_cycalloc(sizeof(*_tmp320));({struct Cyc_Absyn_Tqual _tmp723=Cyc_Absyn_empty_tqual(0);_tmp320->f1=_tmp723;});_tmp320->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp320;});_tmp31F->hd=_tmp724;});_tmp31F->tl=fields;_tmp31F;});fields=_tmp725;});}
# 1679
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp321=_cycalloc(sizeof(*_tmp321));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp727=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp322;_tmp322.tag=10;({struct Cyc_List_List*_tmp726=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp322.f1=_tmp726;});_tmp322;});_tmp321[0]=_tmp727;});_tmp321;});}
# 1683
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1686
return({void*_tmp323=0;({struct Cyc_Tcenv_Tenv*_tmp72B=te;unsigned int _tmp72A=loc;void**_tmp729=topt;struct _dyneither_ptr _tmp728=({const char*_tmp324="tcCompoundLit";_tag_dyneither(_tmp324,sizeof(char),14);});Cyc_Tcexp_expr_err(_tmp72B,_tmp72A,_tmp729,_tmp728,_tag_dyneither(_tmp323,sizeof(void*),0));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1696 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1699
void*res_t2;
int _tmp325=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp326=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp33C=_cycalloc(sizeof(*_tmp33C));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp72D=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp33D;_tmp33D.tag=0;({union Cyc_Absyn_Cnst _tmp72C=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp325);_tmp33D.f1=_tmp72C;});_tmp33D;});_tmp33C[0]=_tmp72D;});_tmp33C;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp326,loc);
# 1707
if(zero_term){
struct Cyc_Absyn_Exp*_tmp327=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp325 - 1);
if(!Cyc_Tcutil_is_zero(_tmp327))
({void*_tmp328=0;({unsigned int _tmp72F=_tmp327->loc;struct _dyneither_ptr _tmp72E=({const char*_tmp329="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp329,sizeof(char),45);});Cyc_Tcutil_terr(_tmp72F,_tmp72E,_tag_dyneither(_tmp328,sizeof(void*),0));});});}
# 1712
sz_exp->topt=Cyc_Absyn_uint_typ;
({void*_tmp733=({void*_tmp732=res;struct Cyc_Absyn_Tqual _tmp731=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0);
# 1713
struct Cyc_Absyn_Exp*_tmp730=sz_exp;Cyc_Absyn_array_typ(_tmp732,_tmp731,_tmp730,
# 1715
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);});
# 1713
res_t2=_tmp733;});
# 1717
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1720
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1722
({struct Cyc_String_pa_PrintArg_struct _tmp32C;_tmp32C.tag=0;({struct _dyneither_ptr _tmp734=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res));_tmp32C.f1=_tmp734;});({void*_tmp32A[1]={& _tmp32C};({unsigned int _tmp736=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp735=({const char*_tmp32B="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp32B,sizeof(char),53);});Cyc_Tcutil_terr(_tmp736,_tmp735,_tag_dyneither(_tmp32A,sizeof(void*),1));});});});
# 1725
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1730
void*_tmp32D=(void*)ds->hd;void*_tmp32E=_tmp32D;struct Cyc_Absyn_Exp*_tmp33B;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp32E)->tag == 1){_LL1E9: _LL1EA:
# 1732
({void*_tmp32F=0;({unsigned int _tmp738=loc;struct _dyneither_ptr _tmp737=({const char*_tmp330="only array index designators are supported";_tag_dyneither(_tmp330,sizeof(char),43);});Cyc_Tcutil_terr(_tmp738,_tmp737,_tag_dyneither(_tmp32F,sizeof(void*),0));});});
goto _LL1E8;}else{_LL1EB: _tmp33B=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp32E)->f1;_LL1EC:
# 1735
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp33B);{
struct _tuple15 _tmp331=Cyc_Evexp_eval_const_uint_exp(_tmp33B);struct _tuple15 _tmp332=_tmp331;unsigned int _tmp33A;int _tmp339;_LL1EE: _tmp33A=_tmp332.f1;_tmp339=_tmp332.f2;_LL1EF:;
if(!_tmp339)
({void*_tmp333=0;({unsigned int _tmp73A=_tmp33B->loc;struct _dyneither_ptr _tmp739=({const char*_tmp334="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp334,sizeof(char),47);});Cyc_Tcutil_terr(_tmp73A,_tmp739,_tag_dyneither(_tmp333,sizeof(void*),0));});});else{
if(_tmp33A != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp338;_tmp338.tag=1;_tmp338.f1=(unsigned long)((int)_tmp33A);({struct Cyc_Int_pa_PrintArg_struct _tmp337;_tmp337.tag=1;_tmp337.f1=(unsigned long)offset;({void*_tmp335[2]={& _tmp337,& _tmp338};({unsigned int _tmp73C=_tmp33B->loc;struct _dyneither_ptr _tmp73B=({const char*_tmp336="expecting index designator %d but found %d";_tag_dyneither(_tmp336,sizeof(char),43);});Cyc_Tcutil_terr(_tmp73C,_tmp73B,_tag_dyneither(_tmp335,sizeof(void*),2));});});});});}
# 1742
goto _LL1E8;};}_LL1E8:;}}}
# 1746
return res_t2;}
# 1750
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1753
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp33E=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp33F=_tmp33E;void*_tmp344;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp33F)->tag == 19){_LL1F1: _tmp344=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp33F)->f1;_LL1F2:
# 1758
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp340=({void*_tmp73D=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp73D,Cyc_Absyn_valueof_exp(_tmp344,0),0,Cyc_Absyn_No_coercion,0);});
_tmp340->topt=bound->topt;
# 1763
bound=_tmp340;}
# 1765
goto _LL1F0;}else{_LL1F3: _LL1F4:
# 1767
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp343;_tmp343.tag=0;({struct _dyneither_ptr _tmp73E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp343.f1=_tmp73E;});({void*_tmp341[1]={& _tmp343};({unsigned int _tmp740=bound->loc;struct _dyneither_ptr _tmp73F=({const char*_tmp342="expecting unsigned int, found %s";_tag_dyneither(_tmp342,sizeof(char),33);});Cyc_Tcutil_terr(_tmp740,_tmp73F,_tag_dyneither(_tmp341,sizeof(void*),1));});});});}_LL1F0:;}
# 1772
if(!(vd->tq).real_const)
({void*_tmp345=0;({struct _dyneither_ptr _tmp741=({const char*_tmp346="comprehension index variable is not declared const!";_tag_dyneither(_tmp346,sizeof(char),52);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp741,_tag_dyneither(_tmp345,sizeof(void*),0));});});
# 1775
if(te->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp742=Cyc_Tcenv_new_block(loc,te);te=_tmp742;});{
void**_tmp347=0;
struct Cyc_Absyn_Tqual*_tmp348=0;
union Cyc_Absyn_Constraint**_tmp349=0;
# 1781
if(topt != 0){
void*_tmp34A=Cyc_Tcutil_compress(*topt);void*_tmp34B=_tmp34A;void*_tmp356;struct Cyc_Absyn_Tqual _tmp355;struct Cyc_Absyn_Exp*_tmp354;union Cyc_Absyn_Constraint*_tmp353;struct Cyc_Absyn_PtrInfo _tmp352;switch(*((int*)_tmp34B)){case 5: _LL1F6: _tmp352=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34B)->f1;_LL1F7:
# 1784
({void**_tmp743=({void**_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=_tmp352.elt_typ;_tmp34C;});_tmp347=_tmp743;});
({struct Cyc_Absyn_Tqual*_tmp744=({struct Cyc_Absyn_Tqual*_tmp34D=_cycalloc_atomic(sizeof(*_tmp34D));_tmp34D[0]=_tmp352.elt_tq;_tmp34D;});_tmp348=_tmp744;});
({union Cyc_Absyn_Constraint**_tmp745=({union Cyc_Absyn_Constraint**_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E[0]=(_tmp352.ptr_atts).zero_term;_tmp34E;});_tmp349=_tmp745;});
goto _LL1F5;case 8: _LL1F8: _tmp356=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34B)->f1).elt_type;_tmp355=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34B)->f1).tq;_tmp354=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34B)->f1).num_elts;_tmp353=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp34B)->f1).zero_term;_LL1F9:
# 1789
({void**_tmp746=({void**_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F[0]=_tmp356;_tmp34F;});_tmp347=_tmp746;});
({struct Cyc_Absyn_Tqual*_tmp747=({struct Cyc_Absyn_Tqual*_tmp350=_cycalloc_atomic(sizeof(*_tmp350));_tmp350[0]=_tmp355;_tmp350;});_tmp348=_tmp747;});
({union Cyc_Absyn_Constraint**_tmp748=({union Cyc_Absyn_Constraint**_tmp351=_cycalloc(sizeof(*_tmp351));_tmp351[0]=_tmp353;_tmp351;});_tmp349=_tmp748;});
goto _LL1F5;default: _LL1FA: _LL1FB:
# 1794
 goto _LL1F5;}_LL1F5:;}{
# 1797
void*t=Cyc_Tcexp_tcExp(te,_tmp347,body);
# 1799
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp357=0;({unsigned int _tmp74A=body->loc;struct _dyneither_ptr _tmp749=({const char*_tmp358="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp358,sizeof(char),49);});Cyc_Tcutil_terr(_tmp74A,_tmp749,_tag_dyneither(_tmp357,sizeof(void*),0));});});
if(te->le == 0){
# 1803
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp359=0;({unsigned int _tmp74C=bound->loc;struct _dyneither_ptr _tmp74B=({const char*_tmp35A="bound is not constant";_tag_dyneither(_tmp35A,sizeof(char),22);});Cyc_Tcutil_terr(_tmp74C,_tmp74B,_tag_dyneither(_tmp359,sizeof(void*),0));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp35B=0;({unsigned int _tmp74E=bound->loc;struct _dyneither_ptr _tmp74D=({const char*_tmp35C="body is not constant";_tag_dyneither(_tmp35C,sizeof(char),21);});Cyc_Tcutil_terr(_tmp74E,_tmp74D,_tag_dyneither(_tmp35B,sizeof(void*),0));});});}
# 1808
if(_tmp349 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp349)){
# 1811
struct Cyc_Absyn_Exp*_tmp35D=Cyc_Absyn_uint_exp(1,0);_tmp35D->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp74F=Cyc_Absyn_add_exp(bound,_tmp35D,0);bound=_tmp74F;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}
# 1815
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp35E=0;({unsigned int _tmp751=body->loc;struct _dyneither_ptr _tmp750=({const char*_tmp35F="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp35F,sizeof(char),49);});Cyc_Tcutil_terr(_tmp751,_tmp750,_tag_dyneither(_tmp35E,sizeof(void*),0));});});{
# 1819
void*_tmp360=({void*_tmp754=t;struct Cyc_Absyn_Tqual _tmp753=_tmp348 == 0?Cyc_Absyn_empty_tqual(0):*_tmp348;struct Cyc_Absyn_Exp*_tmp752=bound;Cyc_Absyn_array_typ(_tmp754,_tmp753,_tmp752,
_tmp349 == 0?Cyc_Absyn_false_conref:*_tmp349,0);});
return _tmp360;};};};}
# 1825
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1828
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp361=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp362=_tmp361;void*_tmp367;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp362)->tag == 19){_LL1FD: _tmp367=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp362)->f1;_LL1FE:
# 1833
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp363=({void*_tmp755=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp755,Cyc_Absyn_valueof_exp(_tmp367,0),0,Cyc_Absyn_No_coercion,0);});
_tmp363->topt=bound->topt;
# 1838
bound=_tmp363;}
# 1840
goto _LL1FC;}else{_LL1FF: _LL200:
# 1842
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp366;_tmp366.tag=0;({struct _dyneither_ptr _tmp756=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp366.f1=_tmp756;});({void*_tmp364[1]={& _tmp366};({unsigned int _tmp758=bound->loc;struct _dyneither_ptr _tmp757=({const char*_tmp365="expecting unsigned int, found %s";_tag_dyneither(_tmp365,sizeof(char),33);});Cyc_Tcutil_terr(_tmp758,_tmp757,_tag_dyneither(_tmp364,sizeof(void*),1));});});});}_LL1FC:;}{
# 1847
void**_tmp368=0;
struct Cyc_Absyn_Tqual*_tmp369=0;
union Cyc_Absyn_Constraint**_tmp36A=0;
# 1851
if(topt != 0){
void*_tmp36B=Cyc_Tcutil_compress(*topt);void*_tmp36C=_tmp36B;void*_tmp377;struct Cyc_Absyn_Tqual _tmp376;struct Cyc_Absyn_Exp*_tmp375;union Cyc_Absyn_Constraint*_tmp374;struct Cyc_Absyn_PtrInfo _tmp373;switch(*((int*)_tmp36C)){case 5: _LL202: _tmp373=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36C)->f1;_LL203:
# 1854
({void**_tmp759=({void**_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=_tmp373.elt_typ;_tmp36D;});_tmp368=_tmp759;});
({struct Cyc_Absyn_Tqual*_tmp75A=({struct Cyc_Absyn_Tqual*_tmp36E=_cycalloc_atomic(sizeof(*_tmp36E));_tmp36E[0]=_tmp373.elt_tq;_tmp36E;});_tmp369=_tmp75A;});
({union Cyc_Absyn_Constraint**_tmp75B=({union Cyc_Absyn_Constraint**_tmp36F=_cycalloc(sizeof(*_tmp36F));_tmp36F[0]=(_tmp373.ptr_atts).zero_term;_tmp36F;});_tmp36A=_tmp75B;});
goto _LL201;case 8: _LL204: _tmp377=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).elt_type;_tmp376=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).tq;_tmp375=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).num_elts;_tmp374=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp36C)->f1).zero_term;_LL205:
# 1859
({void**_tmp75C=({void**_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370[0]=_tmp377;_tmp370;});_tmp368=_tmp75C;});
({struct Cyc_Absyn_Tqual*_tmp75D=({struct Cyc_Absyn_Tqual*_tmp371=_cycalloc_atomic(sizeof(*_tmp371));_tmp371[0]=_tmp376;_tmp371;});_tmp369=_tmp75D;});
({union Cyc_Absyn_Constraint**_tmp75E=({union Cyc_Absyn_Constraint**_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372[0]=_tmp374;_tmp372;});_tmp36A=_tmp75E;});
goto _LL201;default: _LL206: _LL207:
# 1864
 goto _LL201;}_LL201:;}
# 1867
({unsigned int _tmp761=loc;struct Cyc_Tcenv_Tenv*_tmp760=te;struct Cyc_List_List*_tmp75F=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp761,_tmp760,_tmp75F,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp368 != 0)Cyc_Tcutil_unify(*_tmp368,t);
# 1870
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp378=0;({unsigned int _tmp763=bound->loc;struct _dyneither_ptr _tmp762=({const char*_tmp379="bound is not constant";_tag_dyneither(_tmp379,sizeof(char),22);});Cyc_Tcutil_terr(_tmp763,_tmp762,_tag_dyneither(_tmp378,sizeof(void*),0));});});}
# 1874
if(_tmp36A != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp36A)){
# 1877
struct Cyc_Absyn_Exp*_tmp37A=Cyc_Absyn_uint_exp(1,0);_tmp37A->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp764=Cyc_Absyn_add_exp(bound,_tmp37A,0);bound=_tmp764;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1881
({void*_tmp37B=0;({unsigned int _tmp766=loc;struct _dyneither_ptr _tmp765=({const char*_tmp37C="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp37C,sizeof(char),74);});Cyc_Tcutil_terr(_tmp766,_tmp765,_tag_dyneither(_tmp37B,sizeof(void*),0));});});}{
# 1884
void*_tmp37D=({void*_tmp769=t;struct Cyc_Absyn_Tqual _tmp768=_tmp369 == 0?Cyc_Absyn_empty_tqual(0):*_tmp369;struct Cyc_Absyn_Exp*_tmp767=bound;Cyc_Absyn_array_typ(_tmp769,_tmp768,_tmp767,
_tmp36A == 0?Cyc_Absyn_false_conref:*_tmp36A,0);});
return _tmp37D;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1899 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1903
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp76A=({struct Cyc_Absyn_Aggrdecl**_tmp37E=_cycalloc(sizeof(*_tmp37E));_tmp37E[0]=ad;_tmp37E;});adptr=_tmp76A;});}else{
# 1909
{struct _handler_cons _tmp37F;_push_handler(& _tmp37F);{int _tmp381=0;if(setjmp(_tmp37F.handler))_tmp381=1;if(!_tmp381){({struct Cyc_Absyn_Aggrdecl**_tmp76B=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp76B;});
ad=*adptr;
# 1909
;_pop_handler();}else{void*_tmp380=(void*)_exn_thrown;void*_tmp382=_tmp380;void*_tmp386;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp382)->tag == Cyc_Dict_Absent){_LL209: _LL20A:
# 1912
({struct Cyc_String_pa_PrintArg_struct _tmp385;_tmp385.tag=0;({struct _dyneither_ptr _tmp76C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp385.f1=_tmp76C;});({void*_tmp383[1]={& _tmp385};({unsigned int _tmp76E=loc;struct _dyneither_ptr _tmp76D=({const char*_tmp384="unbound struct/union name %s";_tag_dyneither(_tmp384,sizeof(char),29);});Cyc_Tcutil_terr(_tmp76E,_tmp76D,_tag_dyneither(_tmp383,sizeof(void*),1));});});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL20B: _tmp386=_tmp382;_LL20C:(int)_rethrow(_tmp386);}_LL208:;}};}
# 1915
*ad_opt=ad;
*tn=ad->name;}
# 1918
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp389;_tmp389.tag=0;({struct _dyneither_ptr _tmp76F=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp38A="struct";_tag_dyneither(_tmp38A,sizeof(char),7);}):({const char*_tmp38B="union";_tag_dyneither(_tmp38B,sizeof(char),6);}));_tmp389.f1=_tmp76F;});({void*_tmp387[1]={& _tmp389};({unsigned int _tmp771=loc;struct _dyneither_ptr _tmp770=({const char*_tmp388="can't construct abstract %s";_tag_dyneither(_tmp388,sizeof(char),28);});Cyc_Tcutil_terr(_tmp771,_tmp770,_tag_dyneither(_tmp387,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1926
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp38C=Cyc_Tcenv_new_status(te);
if(_tmp38C == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp772=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp772;});else{
# 1935
({struct Cyc_Tcenv_Tenv*_tmp773=Cyc_Tcenv_set_new_status(_tmp38C,te);te2=_tmp773;});}{
# 1937
struct _tuple12 _tmp38D=({struct _tuple12 _tmp3B4;({struct Cyc_List_List*_tmp774=Cyc_Tcenv_lookup_type_vars(te2);_tmp3B4.f1=_tmp774;});_tmp3B4.f2=Cyc_Core_heap_region;_tmp3B4;});
struct Cyc_List_List*_tmp38E=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp38D,ad->tvs);
struct Cyc_List_List*_tmp38F=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp38D,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp390=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp38E);
struct Cyc_List_List*_tmp391=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp38F);
struct Cyc_List_List*_tmp392=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp38E,_tmp38F);
void*res_typ;
# 1948
if(topt != 0){
void*_tmp393=Cyc_Tcutil_compress(*topt);void*_tmp394=_tmp393;struct Cyc_Absyn_Aggrdecl**_tmp39A;struct Cyc_List_List*_tmp399;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp394)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp394)->f1).aggr_info).KnownAggr).tag == 2){_LL20E: _tmp39A=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp394)->f1).aggr_info).KnownAggr).val;_tmp399=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp394)->f1).targs;_LL20F:
# 1951
 if(*_tmp39A == *adptr){
{struct Cyc_List_List*_tmp395=_tmp390;for(0;_tmp395 != 0  && _tmp399 != 0;
(_tmp395=_tmp395->tl,_tmp399=_tmp399->tl)){
Cyc_Tcutil_unify((void*)_tmp395->hd,(void*)_tmp399->hd);}}
# 1956
res_typ=*topt;
goto _LL20D;}
# 1959
goto _LL211;}else{goto _LL210;}}else{_LL210: _LL211:
# 1961
({void*_tmp778=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp396=_cycalloc(sizeof(*_tmp396));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp777=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp397;_tmp397.tag=11;({struct Cyc_Absyn_AggrInfo _tmp776=({struct Cyc_Absyn_AggrInfo _tmp398;({union Cyc_Absyn_AggrInfoU _tmp775=Cyc_Absyn_KnownAggr(adptr);_tmp398.aggr_info=_tmp775;});_tmp398.targs=_tmp390;_tmp398;});_tmp397.f1=_tmp776;});_tmp397;});_tmp396[0]=_tmp777;});_tmp396;});res_typ=_tmp778;});}_LL20D:;}else{
# 1964
({void*_tmp77C=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp39B=_cycalloc(sizeof(*_tmp39B));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp77B=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp39C;_tmp39C.tag=11;({struct Cyc_Absyn_AggrInfo _tmp77A=({struct Cyc_Absyn_AggrInfo _tmp39D;({union Cyc_Absyn_AggrInfoU _tmp779=Cyc_Absyn_KnownAggr(adptr);_tmp39D.aggr_info=_tmp779;});_tmp39D.targs=_tmp390;_tmp39D;});_tmp39C.f1=_tmp77A;});_tmp39C;});_tmp39B[0]=_tmp77B;});_tmp39B;});res_typ=_tmp77C;});}{
# 1967
struct Cyc_List_List*_tmp39E=*ts;
struct Cyc_List_List*_tmp39F=_tmp391;
while(_tmp39E != 0  && _tmp39F != 0){
# 1971
({unsigned int _tmp77F=loc;struct Cyc_Tcenv_Tenv*_tmp77E=te2;struct Cyc_List_List*_tmp77D=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp77F,_tmp77E,_tmp77D,& Cyc_Tcutil_ak,1,0,(void*)_tmp39E->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp39E->hd);
Cyc_Tcutil_unify((void*)_tmp39E->hd,(void*)_tmp39F->hd);
_tmp39E=_tmp39E->tl;
_tmp39F=_tmp39F->tl;}
# 1977
if(_tmp39E != 0)
({void*_tmp3A0=0;({unsigned int _tmp781=loc;struct _dyneither_ptr _tmp780=({const char*_tmp3A1="too many explicit witness types";_tag_dyneither(_tmp3A1,sizeof(char),32);});Cyc_Tcutil_terr(_tmp781,_tmp780,_tag_dyneither(_tmp3A0,sizeof(void*),0));});});
# 1980
*ts=_tmp391;{
# 1983
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3A2=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3A3=_tmp3A2;struct Cyc_Absyn_Aggrfield*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3AE;_LL213: _tmp3AF=_tmp3A3->f1;_tmp3AE=_tmp3A3->f2;_LL214:;{
void*_tmp3A4=Cyc_Tcutil_substitute(_tmp392,_tmp3AF->type);
({struct Cyc_Tcenv_Tenv*_tmp782=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp782,& _tmp3A4,_tmp3AE);});
# 1994
if(!Cyc_Tcutil_coerce_arg(te2,_tmp3AE,_tmp3A4,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3AB;_tmp3AB.tag=0;({struct _dyneither_ptr _tmp783=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1998
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3AE->topt)));_tmp3AB.f1=_tmp783;});({struct Cyc_String_pa_PrintArg_struct _tmp3AA;_tmp3AA.tag=0;({struct _dyneither_ptr _tmp784=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1997
Cyc_Absynpp_typ2string(_tmp3A4));_tmp3AA.f1=_tmp784;});({struct Cyc_String_pa_PrintArg_struct _tmp3A9;_tmp3A9.tag=0;({struct _dyneither_ptr _tmp785=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp3A9.f1=_tmp785;});({struct Cyc_String_pa_PrintArg_struct _tmp3A8;_tmp3A8.tag=0;({struct _dyneither_ptr _tmp786=(struct _dyneither_ptr)(
# 1996
ad->kind == Cyc_Absyn_StructA?({const char*_tmp3AC="struct";_tag_dyneither(_tmp3AC,sizeof(char),7);}):({const char*_tmp3AD="union";_tag_dyneither(_tmp3AD,sizeof(char),6);}));_tmp3A8.f1=_tmp786;});({struct Cyc_String_pa_PrintArg_struct _tmp3A7;_tmp3A7.tag=0;_tmp3A7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3AF->name);({void*_tmp3A5[5]={& _tmp3A7,& _tmp3A8,& _tmp3A9,& _tmp3AA,& _tmp3AB};({unsigned int _tmp788=_tmp3AE->loc;struct _dyneither_ptr _tmp787=({const char*_tmp3A6="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp3A6,sizeof(char),40);});Cyc_Tcutil_terr(_tmp788,_tmp787,_tag_dyneither(_tmp3A5,sizeof(void*),5));});});});});});});});
# 1999
Cyc_Tcutil_explain_failure();}};}{
# 2003
struct Cyc_List_List*_tmp3B0=0;
{struct Cyc_List_List*_tmp3B1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp3B1 != 0;_tmp3B1=_tmp3B1->tl){
({struct Cyc_List_List*_tmp78C=({struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({struct _tuple0*_tmp78B=({struct _tuple0*_tmp3B3=_cycalloc(sizeof(*_tmp3B3));({void*_tmp789=Cyc_Tcutil_substitute(_tmp392,(*((struct _tuple0*)_tmp3B1->hd)).f1);_tmp3B3->f1=_tmp789;});({void*_tmp78A=
Cyc_Tcutil_substitute(_tmp392,(*((struct _tuple0*)_tmp3B1->hd)).f2);_tmp3B3->f2=_tmp78A;});_tmp3B3;});_tmp3B2->hd=_tmp78B;});_tmp3B2->tl=_tmp3B0;_tmp3B2;});
# 2005
_tmp3B0=_tmp78C;});}}
# 2008
({struct Cyc_List_List*_tmp78D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3B0);_tmp3B0=_tmp78D;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp3B0);
return res_typ;};};};};};}
# 2014
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2016
{void*_tmp3B5=Cyc_Tcutil_compress(ts);void*_tmp3B6=_tmp3B5;enum Cyc_Absyn_AggrKind _tmp3C5;struct Cyc_List_List*_tmp3C4;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B6)->tag == 12){_LL216: _tmp3C5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B6)->f1;_tmp3C4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B6)->f2;_LL217:
# 2018
 if(_tmp3C5 == Cyc_Absyn_UnionA)
({void*_tmp3B7=0;({unsigned int _tmp78F=loc;struct _dyneither_ptr _tmp78E=({const char*_tmp3B8="expecting struct but found union";_tag_dyneither(_tmp3B8,sizeof(char),33);});Cyc_Tcutil_terr(_tmp78F,_tmp78E,_tag_dyneither(_tmp3B7,sizeof(void*),0));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp3C4);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3B9=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3BA=_tmp3B9;struct Cyc_Absyn_Aggrfield*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C0;_LL21B: _tmp3C1=_tmp3BA->f1;_tmp3C0=_tmp3BA->f2;_LL21C:;
({struct Cyc_Tcenv_Tenv*_tmp791=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp790=& _tmp3C1->type;Cyc_Tcexp_tcExpInitializer(_tmp791,_tmp790,_tmp3C0);});
# 2027
if(!Cyc_Tcutil_coerce_arg(te,_tmp3C0,_tmp3C1->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3BF;_tmp3BF.tag=0;({struct _dyneither_ptr _tmp792=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2030
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3C0->topt)));_tmp3BF.f1=_tmp792;});({struct Cyc_String_pa_PrintArg_struct _tmp3BE;_tmp3BE.tag=0;({struct _dyneither_ptr _tmp793=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2029
Cyc_Absynpp_typ2string(_tmp3C1->type));_tmp3BE.f1=_tmp793;});({struct Cyc_String_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=0;_tmp3BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3C1->name);({void*_tmp3BB[3]={& _tmp3BD,& _tmp3BE,& _tmp3BF};({unsigned int _tmp795=_tmp3C0->loc;struct _dyneither_ptr _tmp794=({const char*_tmp3BC="field %s of struct expects type %s != %s";_tag_dyneither(_tmp3BC,sizeof(char),41);});Cyc_Tcutil_terr(_tmp795,_tmp794,_tag_dyneither(_tmp3BB,sizeof(void*),3));});});});});});
# 2031
Cyc_Tcutil_explain_failure();}}
# 2034
goto _LL215;};}else{_LL218: _LL219:
({void*_tmp3C2=0;({struct _dyneither_ptr _tmp796=({const char*_tmp3C3="tcAnonStruct: wrong type";_tag_dyneither(_tmp3C3,sizeof(char),25);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp796,_tag_dyneither(_tmp3C2,sizeof(void*),0));});});}_LL215:;}
# 2037
return ts;}
# 2041
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2044
struct _tuple12 _tmp3C6=({struct _tuple12 _tmp3DA;({struct Cyc_List_List*_tmp797=Cyc_Tcenv_lookup_type_vars(te);_tmp3DA.f1=_tmp797;});_tmp3DA.f2=Cyc_Core_heap_region;_tmp3DA;});
struct Cyc_List_List*_tmp3C7=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp3C6,tud->tvs);
struct Cyc_List_List*_tmp3C8=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3C7);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp79A=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp3D8;_tmp3D8.tag=4;({struct Cyc_Absyn_DatatypeFieldInfo _tmp799=({struct Cyc_Absyn_DatatypeFieldInfo _tmp3D9;({union Cyc_Absyn_DatatypeFieldInfoU _tmp798=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp3D9.field_info=_tmp798;});_tmp3D9.targs=_tmp3C8;_tmp3D9;});_tmp3D8.f1=_tmp799;});_tmp3D8;});_tmp3D7[0]=_tmp79A;});_tmp3D7;});
# 2051
if(topt != 0){
void*_tmp3C9=Cyc_Tcutil_compress(*topt);void*_tmp3CA=_tmp3C9;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3CA)->tag == 4){_LL21E: _LL21F:
 Cyc_Tcutil_unify(*topt,res);goto _LL21D;}else{_LL220: _LL221:
 goto _LL21D;}_LL21D:;}{
# 2057
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp3C7 != 0)({void*_tmp79B=Cyc_Tcutil_substitute(_tmp3C7,t);t=_tmp79B;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3CF;_tmp3CF.tag=0;({struct _dyneither_ptr _tmp79C=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2068
e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3D0="?";_tag_dyneither(_tmp3D0,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp3CF.f1=_tmp79C;});({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0;({struct _dyneither_ptr _tmp79D=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2067
Cyc_Absynpp_typ2string(t));_tmp3CE.f1=_tmp79D;});({struct Cyc_String_pa_PrintArg_struct _tmp3CD;_tmp3CD.tag=0;({struct _dyneither_ptr _tmp79E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp3CD.f1=_tmp79E;});({void*_tmp3CB[3]={& _tmp3CD,& _tmp3CE,& _tmp3CF};({unsigned int _tmp7A0=e->loc;struct _dyneither_ptr _tmp79F=({const char*_tmp3CC="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3CC,sizeof(char),82);});Cyc_Tcutil_terr(_tmp7A0,_tmp79F,_tag_dyneither(_tmp3CB,sizeof(void*),3));});});});});});
# 2069
Cyc_Tcutil_explain_failure();}}
# 2072
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3D3;_tmp3D3.tag=0;({struct _dyneither_ptr _tmp7A1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2075
Cyc_Absynpp_qvar2string(tuf->name));_tmp3D3.f1=_tmp7A1;});({void*_tmp3D1[1]={& _tmp3D3};({struct Cyc_Tcenv_Tenv*_tmp7A5=te;unsigned int _tmp7A4=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp7A3=topt;struct _dyneither_ptr _tmp7A2=({const char*_tmp3D2="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3D2,sizeof(char),47);});Cyc_Tcexp_expr_err(_tmp7A5,_tmp7A4,_tmp7A3,_tmp7A2,_tag_dyneither(_tmp3D1,sizeof(void*),1));});});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3D6;_tmp3D6.tag=0;({struct _dyneither_ptr _tmp7A6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));_tmp3D6.f1=_tmp7A6;});({void*_tmp3D4[1]={& _tmp3D6};({struct Cyc_Tcenv_Tenv*_tmp7AA=te;unsigned int _tmp7A9=loc;void**_tmp7A8=topt;struct _dyneither_ptr _tmp7A7=({const char*_tmp3D5="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3D5,sizeof(char),46);});Cyc_Tcexp_expr_err(_tmp7AA,_tmp7A9,_tmp7A8,_tmp7A7,_tag_dyneither(_tmp3D4,sizeof(void*),1));});});});
return res;};}
# 2082
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2084
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2086
if(topt != 0){
void*_tmp3DB=Cyc_Tcutil_compress(*topt);void*_tmp3DC=_tmp3DB;void*_tmp3DD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DC)->tag == 5){_LL223: _tmp3DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DC)->f1).elt_typ;_LL224:
# 2089
 Cyc_Tcutil_unify(_tmp3DD,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL222;}else{_LL225: _LL226:
 goto _LL222;}_LL222:;}
# 2095
({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0;({struct _dyneither_ptr _tmp7AB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp3E1.f1=_tmp7AB;});({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0;({struct _dyneither_ptr _tmp7AC=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3E2="calloc";_tag_dyneither(_tmp3E2,sizeof(char),7);}):(struct _dyneither_ptr)({const char*_tmp3E3="malloc";_tag_dyneither(_tmp3E3,sizeof(char),7);})));_tmp3E0.f1=_tmp7AC;});({void*_tmp3DE[2]={& _tmp3E0,& _tmp3E1};({unsigned int _tmp7AE=loc;struct _dyneither_ptr _tmp7AD=({const char*_tmp3DF="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3DF,sizeof(char),60);});Cyc_Tcutil_terr(_tmp7AE,_tmp7AD,_tag_dyneither(_tmp3DE,sizeof(void*),2));});});});});}
# 2099
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2101
enum Cyc_Absyn_AliasQual _tmp3E4=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3E5=_tmp3E4;if(_tmp3E5 == Cyc_Absyn_Unique){_LL228: _LL229:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL22A: _LL22B:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL227:;}
# 2107
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2112
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2115
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp7B0=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp3ED;_tmp3ED.tag=15;({void*_tmp7AF=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp3ED.f1=_tmp7AF;});_tmp3ED;});_tmp3EC[0]=_tmp7B0;});_tmp3EC;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3E6=Cyc_Tcutil_compress(handle_type);void*_tmp3E7=_tmp3E6;void*_tmp3EB;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E7)->tag == 15){_LL22D: _tmp3EB=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E7)->f1;_LL22E:
# 2120
 rgn=_tmp3EB;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22C;}else{_LL22F: _LL230:
# 2124
({struct Cyc_String_pa_PrintArg_struct _tmp3EA;_tmp3EA.tag=0;({struct _dyneither_ptr _tmp7B1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));_tmp3EA.f1=_tmp7B1;});({void*_tmp3E8[1]={& _tmp3EA};({unsigned int _tmp7B3=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7B2=({const char*_tmp3E9="expecting region_t type but found %s";_tag_dyneither(_tmp3E9,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7B3,_tmp7B2,_tag_dyneither(_tmp3E8,sizeof(void*),1));});});});
goto _LL22C;}_LL22C:;}else{
# 2131
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7B4=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7B4;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2138
({struct Cyc_Tcenv_Tenv*_tmp7B5=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7B5,& Cyc_Absyn_uint_typ,*e);});{
# 2147 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3EE=0;({struct _dyneither_ptr _tmp7B6=({const char*_tmp3EF="calloc with empty type";_tag_dyneither(_tmp3EF,sizeof(char),23);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7B6,_tag_dyneither(_tmp3EE,sizeof(void*),0));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7B9=loc;struct Cyc_Tcenv_Tenv*_tmp7B8=te;struct Cyc_List_List*_tmp7B7=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7B9,_tmp7B8,_tmp7B7,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2159
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3F0=er;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp405;void*_tmp404;switch(*((int*)_tmp3F0)){case 17: _LL232: _tmp404=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F0)->f1;_LL233:
# 2163
 elt_type=_tmp404;
({void**_tmp7BA=({void**_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1[0]=elt_type;_tmp3F1;});*t=_tmp7BA;});
({struct Cyc_Absyn_Exp*_tmp7BB=Cyc_Absyn_uint_exp(1,0);num_elts=_tmp7BB;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL231;case 3: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2)->tl)->tl == 0){_LL234: _tmp406=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2)->hd;_tmp405=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F0)->f2)->tl)->hd;_LL235:
# 2170
{struct _tuple0 _tmp3F2=({struct _tuple0 _tmp3F8;_tmp3F8.f1=_tmp406->r;_tmp3F8.f2=_tmp405->r;_tmp3F8;});struct _tuple0 _tmp3F3=_tmp3F2;void*_tmp3F7;void*_tmp3F6;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F3.f1)->tag == 17){_LL239: _tmp3F6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F3.f1)->f1;_LL23A:
# 2172
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3F6);
elt_type=_tmp3F6;
({void**_tmp7BC=({void**_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4[0]=elt_type;_tmp3F4;});*t=_tmp7BC;});
num_elts=_tmp405;
one_elt=0;
goto _LL238;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F3.f2)->tag == 17){_LL23B: _tmp3F7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3F3.f2)->f1;_LL23C:
# 2179
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3F7);
elt_type=_tmp3F7;
({void**_tmp7BD=({void**_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5[0]=elt_type;_tmp3F5;});*t=_tmp7BD;});
num_elts=_tmp406;
one_elt=0;
goto _LL238;}else{_LL23D: _LL23E:
 goto No_sizeof;}}_LL238:;}
# 2187
goto _LL231;}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}default: _LL236: _LL237:
# 2189
 No_sizeof: {
# 2192
struct Cyc_Absyn_Exp*_tmp3F9=*e;
{void*_tmp3FA=_tmp3F9->r;void*_tmp3FB=_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FC;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FB)->tag == 14){_LL240: _tmp3FC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FB)->f2;_LL241:
 _tmp3F9=_tmp3FC;goto _LL23F;}else{_LL242: _LL243:
 goto _LL23F;}_LL23F:;}
# 2197
{void*_tmp3FD=Cyc_Tcutil_compress((void*)_check_null(_tmp3F9->topt));void*_tmp3FE=_tmp3FD;void*_tmp402;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3FE)->tag == 19){_LL245: _tmp402=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3FE)->f1;_LL246:
# 2199
{void*_tmp3FF=Cyc_Tcutil_compress(_tmp402);void*_tmp400=_tmp3FF;struct Cyc_Absyn_Exp*_tmp401;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp400)->tag == 18){_LL24A: _tmp401=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp400)->f1;_LL24B:
# 2201
 er=_tmp401->r;goto retry_sizeof;}else{_LL24C: _LL24D:
 goto _LL249;}_LL249:;}
# 2204
goto _LL244;}else{_LL247: _LL248:
 goto _LL244;}_LL244:;}
# 2207
elt_type=Cyc_Absyn_char_typ;
({void**_tmp7BE=({void**_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403[0]=elt_type;_tmp403;});*t=_tmp7BE;});
num_elts=*e;
one_elt=0;}
# 2212
goto _LL231;}_LL231:;}}
# 2216
*is_fat=!one_elt;
# 2219
{void*_tmp407=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp40A;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp407)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp407)->f1).aggr_info).KnownAggr).tag == 2){_LL24F: _tmp40A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp407)->f1).aggr_info).KnownAggr).val;_LL250:
# 2221
 if(_tmp40A->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp40A->impl))->exist_vars != 0)
({void*_tmp408=0;({unsigned int _tmp7C0=loc;struct _dyneither_ptr _tmp7BF=({const char*_tmp409="malloc with existential types not yet implemented";_tag_dyneither(_tmp409,sizeof(char),50);});Cyc_Tcutil_terr(_tmp7C0,_tmp7BF,_tag_dyneither(_tmp408,sizeof(void*),0));});});
goto _LL24E;}else{goto _LL251;}}else{_LL251: _LL252:
 goto _LL24E;}_LL24E:;}{
# 2228
void*(*_tmp40B)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp40C=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp40D=Cyc_Tcutil_compress(*topt);void*_tmp40E=_tmp40D;union Cyc_Absyn_Constraint*_tmp41B;union Cyc_Absyn_Constraint*_tmp41A;union Cyc_Absyn_Constraint*_tmp419;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->tag == 5){_LL254: _tmp41B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->f1).ptr_atts).nullable;_tmp41A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->f1).ptr_atts).bounds;_tmp419=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40E)->f1).ptr_atts).zero_term;_LL255:
# 2233
 _tmp40C=_tmp419;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp41B))
_tmp40B=Cyc_Absyn_star_typ;
# 2238
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp419) && !(*is_calloc)){
({void*_tmp40F=0;({unsigned int _tmp7C2=loc;struct _dyneither_ptr _tmp7C1=({const char*_tmp410="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp410,sizeof(char),55);});Cyc_Tcutil_warn(_tmp7C2,_tmp7C1,_tag_dyneither(_tmp40F,sizeof(void*),0));});});
*is_calloc=1;}
# 2244
{void*_tmp411=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp41A);void*_tmp412=_tmp411;struct Cyc_Absyn_Exp*_tmp418;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp412)->tag == 0){_LL259: _LL25A:
 goto _LL258;}else{_LL25B: _tmp418=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp412)->f1;if(!one_elt){_LL25C: {
# 2247
int _tmp413=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp413  && Cyc_Evexp_same_const_exp(_tmp418,num_elts)){
*is_fat=0;
return({void*_tmp7C6=elt_type;void*_tmp7C5=rgn;struct Cyc_Absyn_Tqual _tmp7C4=Cyc_Absyn_empty_tqual(0);void*_tmp7C3=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp41A);Cyc_Absyn_atb_typ(_tmp7C6,_tmp7C5,_tmp7C4,_tmp7C3,_tmp40C);});}
# 2253
{void*_tmp414=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp415=_tmp414;void*_tmp417;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp415)->tag == 19){_LL260: _tmp417=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp415)->f1;_LL261: {
# 2255
struct Cyc_Absyn_Exp*_tmp416=({void*_tmp7C7=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp7C7,Cyc_Absyn_valueof_exp(_tmp417,0),0,Cyc_Absyn_No_coercion,0);});
# 2257
if(Cyc_Evexp_same_const_exp(_tmp416,_tmp418)){
*is_fat=0;
return({void*_tmp7CB=elt_type;void*_tmp7CA=rgn;struct Cyc_Absyn_Tqual _tmp7C9=Cyc_Absyn_empty_tqual(0);void*_tmp7C8=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp41A);Cyc_Absyn_atb_typ(_tmp7CB,_tmp7CA,_tmp7C9,_tmp7C8,_tmp40C);});}
# 2262
goto _LL25F;}}else{_LL262: _LL263:
 goto _LL25F;}_LL25F:;}
# 2265
goto _LL258;}}else{_LL25D: _LL25E:
 goto _LL258;}}_LL258:;}
# 2268
goto _LL253;}else{_LL256: _LL257:
 goto _LL253;}_LL253:;}
# 2271
if(!one_elt)_tmp40B=Cyc_Absyn_dyneither_typ;
return({void*(*_tmp7CF)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=_tmp40B;void*_tmp7CE=elt_type;void*_tmp7CD=rgn;struct Cyc_Absyn_Tqual _tmp7CC=Cyc_Absyn_empty_tqual(0);_tmp7CF(_tmp7CE,_tmp7CD,_tmp7CC,_tmp40C);});};};}
# 2276
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2278
struct Cyc_Tcenv_Tenv*_tmp41C=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp41C,0,e1);{
void*_tmp41D=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp41C,& _tmp41D,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2285
{void*_tmp41E=Cyc_Tcutil_compress(t1);void*_tmp41F=_tmp41E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp41F)->tag == 8){_LL265: _LL266:
({void*_tmp420=0;({unsigned int _tmp7D1=loc;struct _dyneither_ptr _tmp7D0=({const char*_tmp421="cannot assign to an array";_tag_dyneither(_tmp421,sizeof(char),26);});Cyc_Tcutil_terr(_tmp7D1,_tmp7D0,_tag_dyneither(_tmp420,sizeof(void*),0));});});goto _LL264;}else{_LL267: _LL268:
 goto _LL264;}_LL264:;}
# 2290
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp422=0;({unsigned int _tmp7D3=loc;struct _dyneither_ptr _tmp7D2=({const char*_tmp423="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp423,sizeof(char),58);});Cyc_Tcutil_terr(_tmp7D3,_tmp7D2,_tag_dyneither(_tmp422,sizeof(void*),0));});});
# 2294
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp424=0;({struct Cyc_Tcenv_Tenv*_tmp7D7=te;unsigned int _tmp7D6=e1->loc;void**_tmp7D5=topt;struct _dyneither_ptr _tmp7D4=({const char*_tmp425="swap non-lvalue";_tag_dyneither(_tmp425,sizeof(char),16);});Cyc_Tcexp_expr_err(_tmp7D7,_tmp7D6,_tmp7D5,_tmp7D4,_tag_dyneither(_tmp424,sizeof(void*),0));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp426=0;({struct Cyc_Tcenv_Tenv*_tmp7DB=te;unsigned int _tmp7DA=e2->loc;void**_tmp7D9=topt;struct _dyneither_ptr _tmp7D8=({const char*_tmp427="swap non-lvalue";_tag_dyneither(_tmp427,sizeof(char),16);});Cyc_Tcexp_expr_err(_tmp7DB,_tmp7DA,_tmp7D9,_tmp7D8,_tag_dyneither(_tmp426,sizeof(void*),0));});});{
# 2299
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp428=0;({struct Cyc_Tcenv_Tenv*_tmp7DF=te;unsigned int _tmp7DE=e1->loc;void**_tmp7DD=topt;struct _dyneither_ptr _tmp7DC=({const char*_tmp429="swap value in zeroterm array";_tag_dyneither(_tmp429,sizeof(char),29);});Cyc_Tcexp_expr_err(_tmp7DF,_tmp7DE,_tmp7DD,_tmp7DC,_tag_dyneither(_tmp428,sizeof(void*),0));});});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp42A=0;({struct Cyc_Tcenv_Tenv*_tmp7E3=te;unsigned int _tmp7E2=e2->loc;void**_tmp7E1=topt;struct _dyneither_ptr _tmp7E0=({const char*_tmp42B="swap value in zeroterm array";_tag_dyneither(_tmp42B,sizeof(char),29);});Cyc_Tcexp_expr_err(_tmp7E3,_tmp7E2,_tmp7E1,_tmp7E0,_tag_dyneither(_tmp42A,sizeof(void*),0));});});
# 2306
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp42C=({struct Cyc_String_pa_PrintArg_struct _tmp430;_tmp430.tag=0;({struct _dyneither_ptr _tmp7E4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp430.f1=_tmp7E4;});({struct Cyc_String_pa_PrintArg_struct _tmp42F;_tmp42F.tag=0;({struct _dyneither_ptr _tmp7E5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp42F.f1=_tmp7E5;});({void*_tmp42D[2]={& _tmp42F,& _tmp430};({struct Cyc_Tcenv_Tenv*_tmp7E9=te;unsigned int _tmp7E8=loc;void**_tmp7E7=topt;struct _dyneither_ptr _tmp7E6=({const char*_tmp42E="type mismatch: %s != %s";_tag_dyneither(_tmp42E,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp7E9,_tmp7E8,_tmp7E7,_tmp7E6,_tag_dyneither(_tmp42D,sizeof(void*),2));});});});});
return _tmp42C;}
# 2313
return(void*)& Cyc_Absyn_VoidType_val;};};};}
# 2318
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
({struct Cyc_Tcenv_Tenv*_tmp7EA=Cyc_Tcenv_enter_stmt_exp(Cyc_Tcenv_clear_abstract_val_ok(te));Cyc_Tcstmt_tcStmt(_tmp7EA,s,1);});
# 2321
while(1){
void*_tmp431=s->r;void*_tmp432=_tmp431;struct Cyc_Absyn_Decl*_tmp43D;struct Cyc_Absyn_Stmt*_tmp43C;struct Cyc_Absyn_Stmt*_tmp43B;struct Cyc_Absyn_Stmt*_tmp43A;struct Cyc_Absyn_Exp*_tmp439;switch(*((int*)_tmp432)){case 1: _LL26A: _tmp439=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp432)->f1;_LL26B: {
# 2325
void*_tmp433=(void*)_check_null(_tmp439->topt);
if(!({void*_tmp7EB=_tmp433;Cyc_Tcutil_unify(_tmp7EB,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp436;_tmp436.tag=0;({struct _dyneither_ptr _tmp7EC=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp433));_tmp436.f1=_tmp7EC;});({void*_tmp434[1]={& _tmp436};({unsigned int _tmp7EE=loc;struct _dyneither_ptr _tmp7ED=({const char*_tmp435="statement expression returns type %s";_tag_dyneither(_tmp435,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7EE,_tmp7ED,_tag_dyneither(_tmp434,sizeof(void*),1));});});});
Cyc_Tcutil_explain_failure();}
# 2331
return _tmp433;}case 2: _LL26C: _tmp43B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp432)->f1;_tmp43A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp432)->f2;_LL26D:
 s=_tmp43A;continue;case 12: _LL26E: _tmp43D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp432)->f1;_tmp43C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp432)->f2;_LL26F:
 s=_tmp43C;continue;default: _LL270: _LL271:
# 2335
 return({void*_tmp437=0;({struct Cyc_Tcenv_Tenv*_tmp7F2=te;unsigned int _tmp7F1=loc;void**_tmp7F0=topt;struct _dyneither_ptr _tmp7EF=({const char*_tmp438="statement expression must end with expression";_tag_dyneither(_tmp438,sizeof(char),46);});Cyc_Tcexp_expr_err(_tmp7F2,_tmp7F1,_tmp7F0,_tmp7EF,_tag_dyneither(_tmp437,sizeof(void*),0));});});}_LL269:;}}
# 2340
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp7F3=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7F3,0,e);}));
{void*_tmp43E=t;struct Cyc_Absyn_Aggrdecl*_tmp442;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43E)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43E)->f1).aggr_info).KnownAggr).tag == 2){_LL273: _tmp442=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp43E)->f1).aggr_info).KnownAggr).val;_LL274:
# 2344
 if((_tmp442->kind == Cyc_Absyn_UnionA  && _tmp442->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp442->impl))->tagged)goto _LL272;
goto _LL276;}else{goto _LL275;}}else{_LL275: _LL276:
# 2347
({struct Cyc_String_pa_PrintArg_struct _tmp441;_tmp441.tag=0;({struct _dyneither_ptr _tmp7F4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp441.f1=_tmp7F4;});({void*_tmp43F[1]={& _tmp441};({unsigned int _tmp7F6=loc;struct _dyneither_ptr _tmp7F5=({const char*_tmp440="expecting @tagged union but found %s";_tag_dyneither(_tmp440,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7F6,_tmp7F5,_tag_dyneither(_tmp43F,sizeof(void*),1));});});});
goto _LL272;}_LL272:;}
# 2350
return Cyc_Absyn_uint_typ;}
# 2354
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2358
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct Cyc_Tcenv_Tenv*_tmp443=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2362
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp44A=_cycalloc(sizeof(*_tmp44A));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp7F8=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp44B;_tmp44B.tag=15;({void*_tmp7F7=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp443));_tmp44B.f1=_tmp7F7;});_tmp44B;});_tmp44A[0]=_tmp7F8;});_tmp44A;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp443,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp444=Cyc_Tcutil_compress(handle_type);void*_tmp445=_tmp444;void*_tmp449;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp445)->tag == 15){_LL278: _tmp449=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp445)->f1;_LL279:
# 2367
 rgn=_tmp449;
Cyc_Tcenv_check_rgn_accessible(_tmp443,loc,rgn);
goto _LL277;}else{_LL27A: _LL27B:
# 2371
({struct Cyc_String_pa_PrintArg_struct _tmp448;_tmp448.tag=0;({struct _dyneither_ptr _tmp7F9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));_tmp448.f1=_tmp7F9;});({void*_tmp446[1]={& _tmp448};({unsigned int _tmp7FB=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp7FA=({const char*_tmp447="expecting region_t type but found %s";_tag_dyneither(_tmp447,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7FB,_tmp7FA,_tag_dyneither(_tmp446,sizeof(void*),1));});});});
goto _LL277;}_LL277:;}else{
# 2378
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7FC=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7FC;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2386
void*_tmp44C=e1->r;void*_tmp44D=_tmp44C;struct Cyc_List_List*_tmp46C;struct Cyc_Core_Opt*_tmp46B;struct Cyc_List_List*_tmp46A;switch(*((int*)_tmp44D)){case 27: _LL27D: _LL27E:
 goto _LL280;case 28: _LL27F: _LL280: {
# 2391
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp443,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp44E=0;({struct _dyneither_ptr _tmp7FD=({const char*_tmp44F="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp44F,sizeof(char),45);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7FD,_tag_dyneither(_tmp44E,sizeof(void*),0));});});
({void*_tmp7FE=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp7FE;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp443,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp443,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2402
return res_typ;}case 36: _LL281: _tmp46B=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp44D)->f1;_tmp46A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp44D)->f2;_LL282:
# 2404
({void*_tmp800=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp450=_cycalloc(sizeof(*_tmp450));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp7FF=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp451;_tmp451.tag=26;_tmp451.f1=_tmp46A;_tmp451;});_tmp450[0]=_tmp7FF;});_tmp450;});e1->r=_tmp800;});
_tmp46C=_tmp46A;goto _LL284;case 26: _LL283: _tmp46C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp44D)->f1;_LL284: {
# 2407
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp452=Cyc_Tcutil_compress(*topt);void*_tmp453=_tmp452;void**_tmp456;struct Cyc_Absyn_Tqual _tmp455;union Cyc_Absyn_Constraint*_tmp454;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp453)->tag == 5){_LL28C: _tmp456=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp453)->f1).elt_typ;_tmp455=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp453)->f1).elt_tq;_tmp454=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp453)->f1).ptr_atts).zero_term;_LL28D:
# 2413
 elt_typ_opt=_tmp456;
({int _tmp801=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp454);zero_term=_tmp801;});
goto _LL28B;}else{_LL28E: _LL28F:
 goto _LL28B;}_LL28B:;}{
# 2419
void*res_typ=Cyc_Tcexp_tcArray(_tmp443,e1->loc,elt_typ_opt,0,zero_term,_tmp46C);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp457=0;({struct _dyneither_ptr _tmp802=({const char*_tmp458="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp458,sizeof(char),50);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp802,_tag_dyneither(_tmp457,sizeof(void*),0));});});
({void*_tmp803=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp803;});
if(topt != 0){
# 2428
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp443,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp443,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2434
return res_typ;};}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp44D)->f1).Wstring_c).tag){case 8: _LL285: _LL286: {
# 2439
void*_tmp459=({void*_tmp806=Cyc_Absyn_char_typ;void*_tmp805=rgn;struct Cyc_Absyn_Tqual _tmp804=Cyc_Absyn_const_tqual(0);Cyc_Absyn_atb_typ(_tmp806,_tmp805,_tmp804,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2441
void*_tmp45A=Cyc_Tcexp_tcExp(_tmp443,& _tmp459,e1);
return({void*_tmp80C=_tmp45A;void*_tmp80B=rgn;struct Cyc_Absyn_Tqual _tmp80A=Cyc_Absyn_empty_tqual(0);void*_tmp809=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp808=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp45C;_tmp45C.tag=1;({struct Cyc_Absyn_Exp*_tmp807=Cyc_Absyn_uint_exp(1,0);_tmp45C.f1=_tmp807;});_tmp45C;});_tmp45B[0]=_tmp808;});_tmp45B;});Cyc_Absyn_atb_typ(_tmp80C,_tmp80B,_tmp80A,_tmp809,Cyc_Absyn_false_conref);});}case 9: _LL287: _LL288: {
# 2446
void*_tmp45D=({void*_tmp80F=Cyc_Absyn_wchar_typ();void*_tmp80E=rgn;struct Cyc_Absyn_Tqual _tmp80D=Cyc_Absyn_const_tqual(0);Cyc_Absyn_atb_typ(_tmp80F,_tmp80E,_tmp80D,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2448
void*_tmp45E=Cyc_Tcexp_tcExp(_tmp443,& _tmp45D,e1);
return({void*_tmp815=_tmp45E;void*_tmp814=rgn;struct Cyc_Absyn_Tqual _tmp813=Cyc_Absyn_empty_tqual(0);void*_tmp812=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp45F=_cycalloc(sizeof(*_tmp45F));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp811=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp460;_tmp460.tag=1;({struct Cyc_Absyn_Exp*_tmp810=Cyc_Absyn_uint_exp(1,0);_tmp460.f1=_tmp810;});_tmp460;});_tmp45F[0]=_tmp811;});_tmp45F;});Cyc_Absyn_atb_typ(_tmp815,_tmp814,_tmp813,_tmp812,Cyc_Absyn_false_conref);});}default: goto _LL289;}default: _LL289: _LL28A:
# 2455
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp461=Cyc_Tcutil_compress(*topt);void*_tmp462=_tmp461;void**_tmp464;struct Cyc_Absyn_Tqual _tmp463;switch(*((int*)_tmp462)){case 5: _LL291: _tmp464=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp462)->f1).elt_typ;_tmp463=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp462)->f1).elt_tq;_LL292:
# 2461
 topt2=_tmp464;goto _LL290;case 3: _LL293: _LL294:
# 2465
 bogus=*topt;
topt2=& bogus;
goto _LL290;default: _LL295: _LL296:
 goto _LL290;}_LL290:;}{
# 2471
void*telt=Cyc_Tcexp_tcExp(_tmp443,topt2,e1);
# 2473
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp465=0;({unsigned int _tmp817=e1->loc;struct _dyneither_ptr _tmp816=({const char*_tmp466="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp466,sizeof(char),49);});Cyc_Tcutil_terr(_tmp817,_tmp816,_tag_dyneither(_tmp465,sizeof(void*),0));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp81C=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp468;_tmp468.tag=5;({struct Cyc_Absyn_PtrInfo _tmp81B=({struct Cyc_Absyn_PtrInfo _tmp469;_tmp469.elt_typ=telt;({struct Cyc_Absyn_Tqual _tmp818=
Cyc_Absyn_empty_tqual(0);_tmp469.elt_tq=_tmp818;});({struct Cyc_Absyn_PtrAtts _tmp81A=({(_tmp469.ptr_atts).rgn=rgn;({union Cyc_Absyn_Constraint*_tmp819=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp469.ptr_atts).nullable=_tmp819;});(_tmp469.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp469.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp469.ptr_atts).ptrloc=0;_tmp469.ptr_atts;});_tmp469.ptr_atts=_tmp81A;});_tmp469;});_tmp468.f1=_tmp81B;});_tmp468;});_tmp467[0]=_tmp81C;});_tmp467;});
# 2480
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp443,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp443,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2487
return res_typ;};};}}_LL27C:;};}
# 2493
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
({void*_tmp81F=({void*_tmp81E=({void*_tmp81D=t;Cyc_Tcutil_promote_array(_tmp81D,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp81E;});e->topt=_tmp81F;});
return t;}
# 2503
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2507
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2512
({void*_tmp46D=0;({unsigned int _tmp821=e->loc;struct _dyneither_ptr _tmp820=({const char*_tmp46E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp46E,sizeof(char),49);});Cyc_Tcutil_terr(_tmp821,_tmp820,_tag_dyneither(_tmp46D,sizeof(void*),0));});});{
# 2514
void*_tmp46F=e->r;void*_tmp470=_tmp46F;switch(*((int*)_tmp470)){case 26: _LL298: _LL299:
 goto _LL29B;case 27: _LL29A: _LL29B:
 goto _LL29D;case 28: _LL29C: _LL29D:
 goto _LL29F;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp470)->f1).String_c).tag){case 9: _LL29E: _LL29F:
 goto _LL2A1;case 8: _LL2A0: _LL2A1:
 return t;default: goto _LL2A2;}default: _LL2A2: _LL2A3:
# 2521
({void*_tmp822=Cyc_Tcutil_compress(t);t=_tmp822;});
if(Cyc_Tcutil_is_array(t)){
({void*_tmp824=({void*_tmp823=t;Cyc_Tcutil_promote_array(_tmp823,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp824;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2526
return t;}_LL297:;};}
# 2537 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp471=e->r;void*_tmp472=_tmp471;struct Cyc_Absyn_Exp*_tmp496;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp472)->tag == 12){_LL2A5: _tmp496=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp472)->f1;_LL2A6:
# 2541
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp496);
({void*_tmp825=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp496->topt),0);_tmp496->topt=_tmp825;});
e->topt=_tmp496->topt;
goto _LL2A4;}else{_LL2A7: _LL2A8:
# 2547
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp826=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp826;});
# 2550
{void*_tmp473=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp474=_tmp473;void*_tmp495;struct Cyc_Absyn_Tqual _tmp494;void*_tmp493;union Cyc_Absyn_Constraint*_tmp492;union Cyc_Absyn_Constraint*_tmp491;union Cyc_Absyn_Constraint*_tmp490;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->tag == 5){_LL2AA: _tmp495=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).elt_typ;_tmp494=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).elt_tq;_tmp493=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).rgn;_tmp492=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).nullable;_tmp491=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).bounds;_tmp490=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp474)->f1).ptr_atts).zero_term;_LL2AB:
# 2552
{void*_tmp475=Cyc_Tcutil_compress(_tmp495);void*_tmp476=_tmp475;struct Cyc_List_List*_tmp48F;void*_tmp48E;struct Cyc_Absyn_Tqual _tmp48D;void*_tmp48C;struct Cyc_List_List*_tmp48B;int _tmp48A;struct Cyc_Absyn_VarargInfo*_tmp489;struct Cyc_List_List*_tmp488;struct Cyc_List_List*_tmp487;struct Cyc_Absyn_Exp*_tmp486;struct Cyc_List_List*_tmp485;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_List_List*_tmp483;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->tag == 9){_LL2AF: _tmp48F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).tvars;_tmp48E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).effect;_tmp48D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).ret_tqual;_tmp48C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).ret_typ;_tmp48B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).args;_tmp48A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).c_varargs;_tmp489=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).cyc_varargs;_tmp488=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).rgn_po;_tmp487=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).attributes;_tmp486=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).requires_clause;_tmp485=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).requires_relns;_tmp484=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).ensures_clause;_tmp483=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp476)->f1).ensures_relns;_LL2B0:
# 2554
 if(_tmp48F != 0){
struct _tuple12 _tmp477=({struct _tuple12 _tmp482;({struct Cyc_List_List*_tmp827=Cyc_Tcenv_lookup_type_vars(te);_tmp482.f1=_tmp827;});_tmp482.f2=Cyc_Core_heap_region;_tmp482;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp477,_tmp48F);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2562
({struct Cyc_List_List*_tmp828=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp488);_tmp488=_tmp828;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp488);{
void*ftyp=({struct Cyc_List_List*_tmp82B=inst;Cyc_Tcutil_substitute(_tmp82B,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp47F=_cycalloc(sizeof(*_tmp47F));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp82A=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp480;_tmp480.tag=9;({struct Cyc_Absyn_FnInfo _tmp829=({struct Cyc_Absyn_FnInfo _tmp481;_tmp481.tvars=0;_tmp481.effect=_tmp48E;_tmp481.ret_tqual=_tmp48D;_tmp481.ret_typ=_tmp48C;_tmp481.args=_tmp48B;_tmp481.c_varargs=_tmp48A;_tmp481.cyc_varargs=_tmp489;_tmp481.rgn_po=0;_tmp481.attributes=_tmp487;_tmp481.requires_clause=_tmp486;_tmp481.requires_relns=_tmp485;_tmp481.ensures_clause=_tmp484;_tmp481.ensures_relns=_tmp483;_tmp481;});_tmp480.f1=_tmp829;});_tmp480;});_tmp47F[0]=_tmp82A;});_tmp47F;}));});
# 2569
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp478=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp47C=_cycalloc(sizeof(*_tmp47C));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp82E=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp47D;_tmp47D.tag=5;({struct Cyc_Absyn_PtrInfo _tmp82D=({struct Cyc_Absyn_PtrInfo _tmp47E;_tmp47E.elt_typ=ftyp;_tmp47E.elt_tq=_tmp494;({struct Cyc_Absyn_PtrAtts _tmp82C=({(_tmp47E.ptr_atts).rgn=_tmp493;(_tmp47E.ptr_atts).nullable=_tmp492;(_tmp47E.ptr_atts).bounds=_tmp491;(_tmp47E.ptr_atts).zero_term=_tmp490;(_tmp47E.ptr_atts).ptrloc=0;_tmp47E.ptr_atts;});_tmp47E.ptr_atts=_tmp82C;});_tmp47E;});_tmp47D.f1=_tmp82D;});_tmp47D;});_tmp47C[0]=_tmp82E;});_tmp47C;});
# 2571
struct Cyc_Absyn_Exp*_tmp479=Cyc_Absyn_copy_exp(e);
({void*_tmp830=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp82F=({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp47B;_tmp47B.tag=13;_tmp47B.f1=_tmp479;_tmp47B.f2=ts;_tmp47B;});_tmp47A[0]=_tmp82F;});_tmp47A;});e->r=_tmp830;});
e->topt=(void*)_tmp478;};}
# 2575
goto _LL2AE;}else{_LL2B1: _LL2B2:
 goto _LL2AE;}_LL2AE:;}
# 2578
goto _LL2A9;}else{_LL2AC: _LL2AD:
 goto _LL2A9;}_LL2A9:;}
# 2581
goto _LL2A4;}_LL2A4:;}
# 2583
return(void*)_check_null(e->topt);}
# 2591
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2593
struct Cyc_List_List*_tmp497=0;
# 2595
{void*_tmp498=fn_exp->r;void*_tmp499=_tmp498;struct Cyc_List_List*_tmp4AD;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp499)->tag == 10){_LL2B4: _tmp4AD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp499)->f2;_LL2B5:
# 2597
{void*_tmp49A=e->r;void*_tmp49B=_tmp49A;struct Cyc_List_List*_tmp4AA;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp49B)->tag == 10){_LL2B9: _tmp4AA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp49B)->f2;_LL2BA: {
# 2599
struct Cyc_List_List*_tmp49C=alias_arg_exps;
int _tmp49D=0;
while(_tmp49C != 0){
while(_tmp49D != (int)_tmp49C->hd){
if(_tmp4AD == 0)
({void*_tmp49E=0;({struct _dyneither_ptr _tmp832=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4A2;_tmp4A2.tag=1;_tmp4A2.f1=(unsigned long)((int)_tmp49C->hd);({struct Cyc_Int_pa_PrintArg_struct _tmp4A1;_tmp4A1.tag=1;_tmp4A1.f1=(unsigned long)_tmp49D;({void*_tmp49F[2]={& _tmp4A1,& _tmp4A2};({struct _dyneither_ptr _tmp831=({const char*_tmp4A0="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp4A0,sizeof(char),36);});Cyc_aprintf(_tmp831,_tag_dyneither(_tmp49F,sizeof(void*),2));});});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp832,_tag_dyneither(_tmp49E,sizeof(void*),0));});});
# 2606
++ _tmp49D;
_tmp4AD=_tmp4AD->tl;
_tmp4AA=((struct Cyc_List_List*)_check_null(_tmp4AA))->tl;}{
# 2611
struct _tuple11 _tmp4A3=({struct Cyc_Absyn_Exp*_tmp833=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AD))->hd;Cyc_Tcutil_insert_alias(_tmp833,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4AA))->hd)->topt)));});struct _tuple11 _tmp4A4=_tmp4A3;struct Cyc_Absyn_Decl*_tmp4A7;struct Cyc_Absyn_Exp*_tmp4A6;_LL2BE: _tmp4A7=_tmp4A4.f1;_tmp4A6=_tmp4A4.f2;_LL2BF:;
_tmp4AD->hd=(void*)_tmp4A6;
({struct Cyc_List_List*_tmp834=({struct Cyc_List_List*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5->hd=_tmp4A7;_tmp4A5->tl=_tmp497;_tmp4A5;});_tmp497=_tmp834;});
_tmp49C=_tmp49C->tl;};}
# 2616
goto _LL2B8;}}else{_LL2BB: _LL2BC:
({void*_tmp4A8=0;({struct _dyneither_ptr _tmp835=({const char*_tmp4A9="not a function call!";_tag_dyneither(_tmp4A9,sizeof(char),21);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp835,_tag_dyneither(_tmp4A8,sizeof(void*),0));});});}_LL2B8:;}
# 2619
goto _LL2B3;}else{_LL2B6: _LL2B7:
({void*_tmp4AB=0;({struct _dyneither_ptr _tmp836=({const char*_tmp4AC="not a function call!";_tag_dyneither(_tmp4AC,sizeof(char),21);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp836,_tag_dyneither(_tmp4AB,sizeof(void*),0));});});}_LL2B3:;}
# 2624
while(_tmp497 != 0){
struct Cyc_Absyn_Decl*_tmp4AE=(struct Cyc_Absyn_Decl*)_tmp497->hd;
({struct Cyc_Absyn_Exp*_tmp83A=({struct Cyc_Absyn_Stmt*_tmp839=({struct Cyc_Absyn_Decl*_tmp838=_tmp4AE;struct Cyc_Absyn_Stmt*_tmp837=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp838,_tmp837,e->loc);});Cyc_Absyn_stmt_exp(_tmp839,e->loc);});fn_exp=_tmp83A;});
_tmp497=_tmp497->tl;}
# 2631
e->topt=0;
e->r=fn_exp->r;}
# 2637
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2641
{void*_tmp4AF=e->r;void*_tmp4B0=_tmp4AF;void*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;struct _dyneither_ptr*_tmp519;struct Cyc_Absyn_Exp*_tmp518;struct Cyc_Absyn_Exp*_tmp517;int*_tmp516;struct Cyc_Absyn_Exp**_tmp515;void***_tmp514;struct Cyc_Absyn_Exp**_tmp513;int*_tmp512;void*_tmp511;struct Cyc_Absyn_Enumfield*_tmp510;struct Cyc_Absyn_Enumdecl*_tmp50F;struct Cyc_Absyn_Enumfield*_tmp50E;struct Cyc_List_List*_tmp50D;struct Cyc_Absyn_Datatypedecl*_tmp50C;struct Cyc_Absyn_Datatypefield*_tmp50B;void*_tmp50A;struct Cyc_List_List*_tmp509;struct _tuple1**_tmp508;struct Cyc_List_List**_tmp507;struct Cyc_List_List*_tmp506;struct Cyc_Absyn_Aggrdecl**_tmp505;struct Cyc_Absyn_Exp*_tmp504;void*_tmp503;int*_tmp502;struct Cyc_Absyn_Vardecl*_tmp501;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;int*_tmp4FE;struct Cyc_Absyn_Stmt*_tmp4FD;struct Cyc_List_List*_tmp4FC;struct _tuple9*_tmp4FB;struct Cyc_List_List*_tmp4FA;struct Cyc_List_List*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;struct _dyneither_ptr*_tmp4F5;int*_tmp4F4;int*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct _dyneither_ptr*_tmp4F1;int*_tmp4F0;int*_tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;void*_tmp4ED;struct Cyc_List_List*_tmp4EC;void*_tmp4EB;struct Cyc_Absyn_Exp*_tmp4EA;struct Cyc_Absyn_Exp**_tmp4E9;struct Cyc_Absyn_Exp*_tmp4E8;struct Cyc_Absyn_Exp*_tmp4E7;void*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;enum Cyc_Absyn_Coercion*_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_List_List*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Core_Opt*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D4;enum Cyc_Absyn_Incrementor _tmp4D3;enum Cyc_Absyn_Primop _tmp4D2;struct Cyc_List_List*_tmp4D1;void**_tmp4D0;union Cyc_Absyn_Cnst*_tmp4CF;struct Cyc_Core_Opt*_tmp4CE;struct Cyc_List_List*_tmp4CD;struct Cyc_Absyn_Exp*_tmp4CC;struct Cyc_List_List*_tmp4CB;struct Cyc_Absyn_VarargCallInfo**_tmp4CA;struct Cyc_Absyn_Exp*_tmp4C9;struct Cyc_List_List*_tmp4C8;struct Cyc_Absyn_VarargCallInfo**_tmp4C7;int*_tmp4C6;struct Cyc_Absyn_Exp*_tmp4C5;switch(*((int*)_tmp4B0)){case 12: _LL2C1: _tmp4C5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2C2:
# 2646
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp4C5);
return;case 10: _LL2C3: _tmp4C9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4C8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4C7=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp4C6=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;if(!(*_tmp4C6)){_LL2C4:
# 2650
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));({struct Cyc_Core_Impossible_exn_struct _tmp83C=({struct Cyc_Core_Impossible_exn_struct _tmp4B2;_tmp4B2.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp83B=({const char*_tmp4B3="unresolved function in tcExpNoInst";_tag_dyneither(_tmp4B3,sizeof(char),35);});_tmp4B2.f1=_tmp83B;});_tmp4B2;});_tmp4B1[0]=_tmp83C;});_tmp4B1;}));}else{_LL2DB: _tmp4CC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4CB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4CA=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL2DC: {
# 2683
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp4B4;_push_handler(& _tmp4B4);{int _tmp4B6=0;if(setjmp(_tmp4B4.handler))_tmp4B6=1;if(!_tmp4B6){
({struct Cyc_Absyn_Exp*_tmp83D=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp83D;});;_pop_handler();}else{void*_tmp4B5=(void*)_exn_thrown;void*_tmp4B7=_tmp4B5;void*_tmp4B8;if(((struct Cyc_Core_Failure_exn_struct*)_tmp4B7)->tag == Cyc_Core_Failure){_LL316: _LL317:
# 2690
 ok=0;
fn_exp=e;
goto _LL315;}else{_LL318: _tmp4B8=_tmp4B7;_LL319:(int)_rethrow(_tmp4B8);}_LL315:;}};}
# 2694
({void*_tmp83E=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4CC,_tmp4CB,_tmp4CA,& alias_arg_exps);t=_tmp83E;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp83F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp83F;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2701
goto _LL2C0;}}case 36: _LL2C5: _tmp4CE=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4CD=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2C6:
# 2654
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp4CD);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0: _LL2C7: _tmp4CF=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2C8:
# 2659
({void*_tmp840=Cyc_Tcexp_tcConst(te,loc,topt,_tmp4CF,e);t=_tmp840;});goto _LL2C0;case 1: _LL2C9: _tmp4D0=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2CA:
# 2661
({void*_tmp841=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp4D0);t=_tmp841;});goto _LL2C0;case 2: _LL2CB: _LL2CC:
# 2663
 t=Cyc_Absyn_sint_typ;goto _LL2C0;case 3: _LL2CD: _tmp4D2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4D1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2CE:
# 2665
({void*_tmp842=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4D2,_tmp4D1);t=_tmp842;});goto _LL2C0;case 5: _LL2CF: _tmp4D4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4D3=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2D0:
# 2667
({void*_tmp843=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp4D4,_tmp4D3);t=_tmp843;});goto _LL2C0;case 4: _LL2D1: _tmp4D7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4D6=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4D5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL2D2:
# 2669
({void*_tmp844=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4D7,_tmp4D6,_tmp4D5);t=_tmp844;});goto _LL2C0;case 6: _LL2D3: _tmp4DA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4D9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4D8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL2D4:
# 2671
({void*_tmp845=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp4DA,_tmp4D9,_tmp4D8);t=_tmp845;});goto _LL2C0;case 7: _LL2D5: _tmp4DC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4DB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2D6:
# 2673
({void*_tmp846=Cyc_Tcexp_tcAnd(te,loc,_tmp4DC,_tmp4DB);t=_tmp846;});goto _LL2C0;case 8: _LL2D7: _tmp4DE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4DD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2D8:
# 2675
({void*_tmp847=Cyc_Tcexp_tcOr(te,loc,_tmp4DE,_tmp4DD);t=_tmp847;});goto _LL2C0;case 9: _LL2D9: _tmp4E0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4DF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2DA:
# 2677
({void*_tmp848=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4E0,_tmp4DF);t=_tmp848;});goto _LL2C0;case 11: _LL2DD: _tmp4E1=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2DE:
# 2703
({void*_tmp849=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4E1);t=_tmp849;});goto _LL2C0;case 13: _LL2DF: _tmp4E3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4E2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2E0:
# 2705
({void*_tmp84A=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4E3,_tmp4E2);t=_tmp84A;});goto _LL2C0;case 14: _LL2E1: _tmp4E6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4E5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4E4=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL2E2:
# 2707
({void*_tmp84B=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4E6,_tmp4E5,_tmp4E4);t=_tmp84B;});goto _LL2C0;case 15: _LL2E3: _tmp4E7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2E4:
# 2709
({void*_tmp84C=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4E7);t=_tmp84C;});goto _LL2C0;case 16: _LL2E5: _tmp4E9=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4E8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2E6:
# 2711
({void*_tmp84D=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4E9,e,_tmp4E8);t=_tmp84D;});goto _LL2C0;case 18: _LL2E7: _tmp4EA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2E8: {
# 2713
void*_tmp4B9=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4EA);
({void*_tmp84E=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4B9);t=_tmp84E;});goto _LL2C0;}case 17: _LL2E9: _tmp4EB=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2EA:
# 2716
({void*_tmp84F=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4EB);t=_tmp84F;});goto _LL2C0;case 19: _LL2EB: _tmp4ED=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4EC=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2EC:
# 2718
({void*_tmp850=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4ED,_tmp4EC);t=_tmp850;});goto _LL2C0;case 20: _LL2ED: _tmp4EE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2EE:
# 2720
({void*_tmp851=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4EE);t=_tmp851;});goto _LL2C0;case 21: _LL2EF: _tmp4F2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4F1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4F0=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp4EF=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL2F0:
# 2722
({void*_tmp852=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4F2,_tmp4F1,_tmp4F0,_tmp4EF);t=_tmp852;});goto _LL2C0;case 22: _LL2F1: _tmp4F6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4F5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4F4=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp4F3=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL2F2:
# 2724
({void*_tmp853=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4F6,_tmp4F5,_tmp4F4,_tmp4F3);t=_tmp853;});goto _LL2C0;case 23: _LL2F3: _tmp4F8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4F7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2F4:
# 2726
({void*_tmp854=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4F8,_tmp4F7);t=_tmp854;});goto _LL2C0;case 24: _LL2F5: _tmp4F9=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2F6:
# 2728
({void*_tmp855=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4F9);t=_tmp855;});goto _LL2C0;case 25: _LL2F7: _tmp4FB=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp4FA=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL2F8:
# 2730
({void*_tmp856=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4FB,_tmp4FA);t=_tmp856;});goto _LL2C0;case 26: _LL2F9: _tmp4FC=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2FA: {
# 2734
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4BA=Cyc_Tcutil_compress(*topt);void*_tmp4BB=_tmp4BA;void**_tmp4BE;struct Cyc_Absyn_Tqual*_tmp4BD;union Cyc_Absyn_Constraint*_tmp4BC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BB)->tag == 8){_LL31B: _tmp4BE=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BB)->f1).elt_type;_tmp4BD=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BB)->f1).tq;_tmp4BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BB)->f1).zero_term;_LL31C:
# 2740
 elt_topt=_tmp4BE;
elt_tqopt=_tmp4BD;
({int _tmp857=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4BC);zero_term=_tmp857;});
goto _LL31A;}else{_LL31D: _LL31E:
 goto _LL31A;}_LL31A:;}
# 2747
({void*_tmp858=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4FC);t=_tmp858;});goto _LL2C0;}case 37: _LL2FB: _tmp4FD=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL2FC:
# 2749
({void*_tmp859=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4FD);t=_tmp859;});goto _LL2C0;case 27: _LL2FD: _tmp501=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp500=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp4FF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp4FE=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL2FE:
# 2751
({void*_tmp85A=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp501,_tmp500,_tmp4FF,_tmp4FE);t=_tmp85A;});goto _LL2C0;case 28: _LL2FF: _tmp504=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp503=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp502=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL300:
# 2753
({void*_tmp85B=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp504,_tmp503,_tmp502);t=_tmp85B;});goto _LL2C0;case 29: _LL301: _tmp508=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp507=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp506=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_tmp505=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4B0)->f4;_LL302:
# 2755
({void*_tmp85C=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp508,_tmp507,_tmp506,_tmp505);t=_tmp85C;});goto _LL2C0;case 30: _LL303: _tmp50A=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp509=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL304:
# 2757
({void*_tmp85D=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp50A,_tmp509);t=_tmp85D;});goto _LL2C0;case 31: _LL305: _tmp50D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp50C=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_tmp50B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4B0)->f3;_LL306:
# 2759
({void*_tmp85E=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp50D,_tmp50C,_tmp50B);t=_tmp85E;});goto _LL2C0;case 32: _LL307: _tmp50F=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp50E=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL308:
# 2761
({void*_tmp860=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp85F=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp4C0;_tmp4C0.tag=13;_tmp4C0.f1=_tmp50F->name;_tmp4C0.f2=_tmp50F;_tmp4C0;});_tmp4BF[0]=_tmp85F;});_tmp4BF;});t=_tmp860;});goto _LL2C0;case 33: _LL309: _tmp511=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp510=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL30A:
# 2763
 t=_tmp511;goto _LL2C0;case 34: _LL30B: _tmp516=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).is_calloc;_tmp515=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).rgn;_tmp514=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).elt_type;_tmp513=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).num_elts;_tmp512=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1).fat_result;_LL30C:
# 2765
({void*_tmp861=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp515,_tmp514,_tmp513,_tmp516,_tmp512);t=_tmp861;});goto _LL2C0;case 35: _LL30D: _tmp518=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp517=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL30E:
# 2767
({void*_tmp862=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp518,_tmp517);t=_tmp862;});goto _LL2C0;case 38: _LL30F: _tmp51A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_tmp519=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4B0)->f2;_LL310:
# 2769
({void*_tmp863=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp51A,_tmp519);t=_tmp863;});goto _LL2C0;case 39: _LL311: _tmp51B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4B0)->f1;_LL312:
# 2771
 if(!te->allow_valueof)
({void*_tmp4C1=0;({unsigned int _tmp865=e->loc;struct _dyneither_ptr _tmp864=({const char*_tmp4C2="valueof(-) can only occur within types";_tag_dyneither(_tmp4C2,sizeof(char),39);});Cyc_Tcutil_terr(_tmp865,_tmp864,_tag_dyneither(_tmp4C1,sizeof(void*),0));});});
# 2779
t=Cyc_Absyn_sint_typ;
goto _LL2C0;default: _LL313: _LL314:
# 2782
({void*_tmp4C3=0;({unsigned int _tmp867=e->loc;struct _dyneither_ptr _tmp866=({const char*_tmp4C4="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp4C4,sizeof(char),50);});Cyc_Tcutil_terr(_tmp867,_tmp866,_tag_dyneither(_tmp4C3,sizeof(void*),0));});});
({void*_tmp868=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp868;});}_LL2C0:;}
# 2785
e->topt=t;}
