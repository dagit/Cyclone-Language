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
# 295 "core.h"
typedef void*Cyc_Core___cyclone_internal_array_t;
typedef unsigned int Cyc_Core___cyclone_internal_singleton;
# 299
inline static void* arrcast(struct _dyneither_ptr dyn,unsigned int bd,unsigned int sz){
# 304
if(bd >> 20  || sz >> 12)
return 0;{
unsigned char*ptrbd=dyn.curr + bd * sz;
if(((ptrbd < dyn.curr  || dyn.curr == 0) || dyn.curr < dyn.base) || ptrbd > dyn.last_plus_one)
# 311
return 0;
return dyn.curr;};}
# 317
static unsigned int arr_prevsize(struct _dyneither_ptr arr,unsigned int elt_sz){
unsigned char*_get_arr_size_curr=arr.curr;
unsigned char*_get_arr_size_base=arr.base;
return
(_get_arr_size_curr < _get_arr_size_base  || _get_arr_size_curr >= arr.last_plus_one)?0:(_get_arr_size_curr - _get_arr_size_base)/ elt_sz;}struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
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
# 82
typedef struct _dyneither_ptr*Cyc_Absyn_field_name_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_t;
typedef struct _dyneither_ptr*Cyc_Absyn_tvarname_t;
typedef struct _dyneither_ptr*Cyc_Absyn_var_opt_t;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 94
typedef union Cyc_Absyn_Nmspace Cyc_Absyn_nmspace_t;
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 101
typedef struct _tuple1*Cyc_Absyn_qvar_t;typedef struct _tuple1*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_t;
typedef struct _tuple1*Cyc_Absyn_typedef_name_opt_t;
# 106
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
# 157
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned int loc;};
# 178
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 183
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 209
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 211
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 220
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 246
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 333
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 366
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 371
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 426 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 429
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 442
typedef void*Cyc_Absyn_funcparams_t;
# 445
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 509
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 525
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 527
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
# 535
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 542
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 560
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 968
void*Cyc_Absyn_exn_typ();
# 979
extern void*Cyc_Absyn_bounds_one;
# 984
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 987
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 990
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 995
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1006
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
# 1061
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
# 1079
extern struct Cyc_Absyn_Exp*Cyc_Absyn_uniquergn_exp;
# 1083
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1094
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1155
void*Cyc_Absyn_pointer_expand(void*,int fresh_evar);
# 1157
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1160
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1162
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
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_youngest(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn,int resetable,int opened);
struct Cyc_RgnOrder_RgnPO*Cyc_RgnOrder_add_unordered(struct _RegionHandle*,struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*rgn);
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 46
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 49
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_CList{void*hd;const struct Cyc_Tcenv_CList*tl;};
# 38 "tcenv.h"
typedef const struct Cyc_Tcenv_CList*Cyc_Tcenv_mclist_t;
typedef const struct Cyc_Tcenv_CList*const Cyc_Tcenv_clist_t;struct Cyc_Tcenv_VarRes_Tcenv_Resolved_struct{int tag;void*f1;};struct Cyc_Tcenv_AggrRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcenv_EnumRes_Tcenv_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};
# 49
typedef void*Cyc_Tcenv_resolved_t;struct Cyc_Tcenv_Genv{struct _RegionHandle*grgn;struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 65
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 69
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 80
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 84
struct _RegionHandle*Cyc_Tcenv_get_fnrgn(struct Cyc_Tcenv_Tenv*);
# 104 "tcenv.h"
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple1*);
# 111
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 114
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_new_status(struct _RegionHandle*,enum Cyc_Tcenv_NewStatus status,struct Cyc_Tcenv_Tenv*);
# 117
enum Cyc_Tcenv_NewStatus Cyc_Tcenv_new_status(struct Cyc_Tcenv_Tenv*te);
# 119
int Cyc_Tcenv_abstract_val_ok(struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 122
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_abstract_val_ok(struct _RegionHandle*r,struct Cyc_Tcenv_Tenv*te);
# 127
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 131
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 148
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*te);
# 153
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*te);
# 161
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(struct _RegionHandle*,unsigned int,struct Cyc_Tcenv_Tenv*);
# 169
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 173
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 177
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 182
void Cyc_Tcenv_check_rgn_partial_order(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*po);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 43
void*Cyc_Tcutil_copy_type(void*t);
# 46
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 49
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 53
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 56
void*Cyc_Tcutil_compress(void*t);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_List_List*);
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 65
int Cyc_Tcutil_coerceable(void*);
# 68
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 70
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);struct _tuple11{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 76
struct _tuple11 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 84
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
# 87
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
# 91
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 112
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
# 122
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 138
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 140
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 144
void Cyc_Tcutil_explain_failure();
# 146
int Cyc_Tcutil_unify(void*,void*);
# 151
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 173
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 178
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 220 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 230
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 232
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 234
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 238
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 244
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 246
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 248
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 258
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 263
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 266
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 271
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 276
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple14{int f1;void*f2;};
# 280
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 307
int Cyc_Tcutil_bits_only(void*t);
# 310
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 339
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 355
int Cyc_Tcutil_is_array(void*t);
# 359
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 362
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
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s,int new_block);
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 119
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_dict_set_t;
# 126
typedef struct Cyc_Dict_Dict Cyc_CfFlowInfo_place_set_t;struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple16{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple16 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 142 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_Dict_Dict mt_place_set;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 161
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;struct Cyc_CfFlowInfo_UniqueRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_RefCntRgn_k_CfFlowInfo_KillRgn_struct{int tag;};struct Cyc_CfFlowInfo_Region_k_CfFlowInfo_KillRgn_struct{int tag;struct Cyc_Absyn_Tvar*f1;};
# 233 "cf_flowinfo.h"
typedef void*Cyc_CfFlowInfo_killrgn_t;
# 47 "tcexp.cyc"
static void*Cyc_Tcexp_expr_err(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _dyneither_ptr msg,struct _dyneither_ptr ap){
# 51
({void*_tmp0=0;Cyc_Tcutil_terr(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap),_tag_dyneither(_tmp0,sizeof(void*),0));});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 55
return*topt;}}
# 64
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 68
if(topt == 0){
# 70
e->r=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp2;_tmp2.tag=25;_tmp2.f1=des;_tmp2;});_tmp1;});
return;}{
# 73
void*t=*topt;
void*_tmp3=Cyc_Tcutil_compress(t);void*_tmp4=_tmp3;void*_tmp13;struct Cyc_Absyn_Tqual _tmp12;union Cyc_Absyn_AggrInfoU _tmp11;switch(*((int*)_tmp4)){case 11: _LL1: _tmp11=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1).aggr_info;_LL2:
# 76
{union Cyc_Absyn_AggrInfoU _tmp5=_tmp11;struct Cyc_Absyn_Aggrdecl*_tmpA;if((_tmp5.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7="struct type not properly set";_tag_dyneither(_tmp7,sizeof(char),29);}),_tag_dyneither(_tmp6,sizeof(void*),0));});}else{_LLC: _tmpA=*(_tmp5.KnownAggr).val;_LLD:
 e->r=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp8=_cycalloc(sizeof(*_tmp8));_tmp8[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp9;_tmp9.tag=28;_tmp9.f1=_tmpA->name;_tmp9.f2=0;_tmp9.f3=des;_tmp9.f4=_tmpA;_tmp9;});_tmp8;});}_LL9:;}
# 80
goto _LL0;case 8: _LL3: _tmp13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).elt_type;_tmp12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).tq;_LL4:
 e->r=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpC;_tmpC.tag=25;_tmpC.f1=des;_tmpC;});_tmpB;});goto _LL0;case 12: _LL5: _LL6:
 e->r=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE;_tmpE.tag=29;_tmpE.f1=t;_tmpE.f2=des;_tmpE;});_tmpD;});goto _LL0;default: _LL7: _LL8:
 e->r=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp10;_tmp10.tag=25;_tmp10.f1=des;_tmp10;});_tmpF;});goto _LL0;}_LL0:;};}
# 90
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e);
# 94
static void Cyc_Tcexp_tcExpList(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*es){
for(0;es != 0;es=es->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);}}
# 100
static void Cyc_Tcexp_check_contains_assign(struct Cyc_Absyn_Exp*e){
void*_tmp14=e->r;void*_tmp15=_tmp14;if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->tag == 3){if(((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp15)->f2 == 0){_LLF: _LL10:
({void*_tmp16=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp17="assignment in test";_tag_dyneither(_tmp17,sizeof(char),19);}),_tag_dyneither(_tmp16,sizeof(void*),0));});goto _LLE;}else{goto _LL11;}}else{_LL11: _LL12:
 goto _LLE;}_LLE:;}
# 108
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;_tmp1B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp18[2]={& _tmp1A,& _tmp1B};Cyc_Tcutil_terr(e->loc,({const char*_tmp19="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp19,sizeof(char),59);}),_tag_dyneither(_tmp18,sizeof(void*),2));});});});
# 117
{void*_tmp1C=e->r;void*_tmp1D=_tmp1C;enum Cyc_Absyn_Primop _tmp26;struct Cyc_List_List*_tmp25;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->tag == 2){_LL14: _tmp26=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f1;_tmp25=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f2;_LL15:
# 119
 if(_tmp26 == Cyc_Absyn_Eq  || _tmp26 == Cyc_Absyn_Neq){
struct _tuple0 _tmp1E=({struct _tuple0 _tmp24;_tmp24.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25))->hd)->topt);_tmp24.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(_tmp25))->tl))->hd)->topt);_tmp24;});struct _tuple0 _tmp1F=_tmp1E;void*_tmp23;void*_tmp22;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->tag == 15){_LL19: _tmp23=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->f1;_tmp22=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->f1;_LL1A:
# 122
 return({struct Cyc_Tcexp_TestEnv _tmp20;_tmp20.eq=({struct _tuple0*_tmp21=_region_malloc(Cyc_Tcenv_get_fnrgn(te),sizeof(*_tmp21));_tmp21->f1=_tmp23;_tmp21->f2=_tmp22;_tmp21;});_tmp20.isTrue=_tmp26 == Cyc_Absyn_Eq;_tmp20;});}else{goto _LL1B;}}else{_LL1B: _LL1C:
# 124
 goto _LL18;}_LL18:;}
# 127
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 130
return({struct Cyc_Tcexp_TestEnv _tmp27;_tmp27.eq=0;_tmp27.isTrue=0;_tmp27;});}
# 151 "tcexp.cyc"
static int Cyc_Tcexp_wchar_numelts(struct _dyneither_ptr s){
return 1;}
# 156
static void*Cyc_Tcexp_tcConst(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,union Cyc_Absyn_Cnst*c,struct Cyc_Absyn_Exp*e){
void*t;
void*string_elt_typ=Cyc_Absyn_char_typ;
int string_numelts=0;
{union Cyc_Absyn_Cnst _tmp28=*((union Cyc_Absyn_Cnst*)_check_null(c));union Cyc_Absyn_Cnst _tmp29=_tmp28;struct _dyneither_ptr _tmp5F;struct _dyneither_ptr _tmp5E;enum Cyc_Absyn_Sign _tmp5D;int _tmp5C;int _tmp5B;enum Cyc_Absyn_Sign _tmp5A;enum Cyc_Absyn_Sign _tmp59;switch((_tmp29.Wstring_c).tag){case 2: switch(((_tmp29.Char_c).val).f1){case Cyc_Absyn_Signed: _LL1E: _LL1F:
 t=Cyc_Absyn_schar_typ;goto _LL1D;case Cyc_Absyn_Unsigned: _LL20: _LL21:
 t=Cyc_Absyn_uchar_typ;goto _LL1D;default: _LL22: _LL23:
 t=Cyc_Absyn_char_typ;goto _LL1D;}case 3: _LL24: _LL25:
 t=Cyc_Absyn_wchar_typ();goto _LL1D;case 4: _LL26: _tmp59=((_tmp29.Short_c).val).f1;_LL27:
# 166
 t=_tmp59 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL1D;case 6: _LL28: _tmp5A=((_tmp29.LongLong_c).val).f1;_LL29:
# 168
 t=_tmp5A == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL1D;case 7: _LL2A: _tmp5B=((_tmp29.Float_c).val).f2;_LL2B:
 t=Cyc_Absyn_float_typ(_tmp5B);goto _LL1D;case 5: _LL2C: _tmp5D=((_tmp29.Int_c).val).f1;_tmp5C=((_tmp29.Int_c).val).f2;_LL2D:
# 171
 if(topt == 0)
t=_tmp5D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;else{
# 178
void*_tmp2A=Cyc_Tcutil_compress(*topt);void*_tmp2B=_tmp2A;void*_tmp41;void*_tmp40;struct Cyc_Absyn_Tqual _tmp3F;void*_tmp3E;union Cyc_Absyn_Constraint*_tmp3D;union Cyc_Absyn_Constraint*_tmp3C;union Cyc_Absyn_Constraint*_tmp3B;enum Cyc_Absyn_Sign _tmp3A;enum Cyc_Absyn_Sign _tmp39;enum Cyc_Absyn_Sign _tmp38;enum Cyc_Absyn_Sign _tmp37;switch(*((int*)_tmp2B)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f2){case Cyc_Absyn_Char_sz: _LL35: _tmp37=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL36:
# 180
{enum Cyc_Absyn_Sign _tmp2C=_tmp37;switch(_tmp2C){case Cyc_Absyn_Unsigned: _LL44: _LL45:
 t=Cyc_Absyn_uchar_typ;goto _LL43;case Cyc_Absyn_Signed: _LL46: _LL47:
 t=Cyc_Absyn_schar_typ;goto _LL43;default: _LL48: _LL49:
 t=Cyc_Absyn_char_typ;goto _LL43;}_LL43:;}
# 185
*c=Cyc_Absyn_Char_c(_tmp37,(char)_tmp5C);
goto _LL34;case Cyc_Absyn_Short_sz: _LL37: _tmp38=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL38:
# 188
 t=_tmp38 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
*c=Cyc_Absyn_Short_c(_tmp38,(short)_tmp5C);
goto _LL34;case Cyc_Absyn_Int_sz: _LL39: _tmp39=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL3A:
# 192
 t=_tmp39 == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;case Cyc_Absyn_Long_sz: _LL3B: _tmp3A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL3C:
# 195
 t=_tmp3A == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;default: goto _LL41;}case 5: _LL3D: _tmp40=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_typ;_tmp3F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_tq;_tmp3E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).rgn;_tmp3D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).nullable;_tmp3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).bounds;_tmp3B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;if(_tmp5C == 0){_LL3E: {
# 199
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct nullc={0,{.Null_c={1,0}}};
e->r=(void*)& nullc;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp3D))return*topt;{
struct Cyc_List_List*_tmp2D=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));_tmp2E[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp2F;_tmp2F.tag=5;_tmp2F.f1=({struct Cyc_Absyn_PtrInfo _tmp30;_tmp30.elt_typ=_tmp40;_tmp30.elt_tq=_tmp3F;_tmp30.ptr_atts=({(_tmp30.ptr_atts).rgn=_tmp3E;(_tmp30.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp30.ptr_atts).bounds=_tmp3C;(_tmp30.ptr_atts).zero_term=_tmp3B;(_tmp30.ptr_atts).ptrloc=0;_tmp30.ptr_atts;});_tmp30;});_tmp2F;});_tmp2E;});
# 205
goto _LL34;};}}else{goto _LL41;}case 19: _LL3F: _tmp41=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B)->f1;_LL40: {
# 207
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp32=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));_tmp35[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp36;_tmp36.tag=18;_tmp36.f1=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0);_tmp36;});_tmp35;});
# 214
t=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp34;_tmp34.tag=19;_tmp34.f1=(void*)_tmp32;_tmp34;});_tmp33;});
goto _LL34;}default: _LL41: _LL42:
# 217
 t=_tmp5D == Cyc_Absyn_Unsigned?Cyc_Absyn_uint_typ: Cyc_Absyn_sint_typ;
goto _LL34;}_LL34:;}
# 220
goto _LL1D;case 8: _LL2E: _tmp5E=(_tmp29.String_c).val;_LL2F:
# 222
 string_numelts=(int)_get_dyneither_size(_tmp5E,sizeof(char));
_tmp5F=_tmp5E;goto _LL31;case 9: _LL30: _tmp5F=(_tmp29.Wstring_c).val;_LL31:
# 225
 if(string_numelts == 0){
string_numelts=Cyc_Tcexp_wchar_numelts(_tmp5F);
string_elt_typ=Cyc_Absyn_wchar_typ();}{
# 229
struct Cyc_Absyn_Exp*elen=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts),loc);
elen->topt=Cyc_Absyn_uint_typ;{
# 234
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp42=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp47;_tmp47.tag=1;_tmp47.f1=elen;_tmp47;});_tmp46;});
t=Cyc_Absyn_atb_typ(string_elt_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_const_tqual(0),(void*)_tmp42,Cyc_Absyn_true_conref);
# 237
if(topt != 0){
void*_tmp43=Cyc_Tcutil_compress(*topt);void*_tmp44=_tmp43;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 8: _LL4B: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL4C:
# 242
 return Cyc_Absyn_array_typ(string_elt_typ,_tmp45,elen,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);case 5: _LL4D: _LL4E:
# 244
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 250
t=Cyc_Absyn_atb_typ(string_elt_typ,Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te)),
Cyc_Absyn_const_tqual(0),(void*)_tmp42,Cyc_Absyn_true_conref);
if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}}
# 258
goto _LL4A;default: _LL4F: _LL50:
 goto _LL4A;}_LL4A:;}
# 262
return t;};};default: _LL32: _LL33:
# 264
 if(topt != 0){
void*_tmp48=Cyc_Tcutil_compress(*topt);void*_tmp49=_tmp48;void*_tmp52;struct Cyc_Absyn_Tqual _tmp51;void*_tmp50;union Cyc_Absyn_Constraint*_tmp4F;union Cyc_Absyn_Constraint*_tmp4E;union Cyc_Absyn_Constraint*_tmp4D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->tag == 5){_LL52: _tmp52=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).elt_typ;_tmp51=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).elt_tq;_tmp50=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).rgn;_tmp4F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).nullable;_tmp4E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).bounds;_tmp4D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49)->f1).ptr_atts).zero_term;_LL53:
# 268
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmp4F))return*topt;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4B;_tmp4B.tag=5;_tmp4B.f1=({struct Cyc_Absyn_PtrInfo _tmp4C;_tmp4C.elt_typ=_tmp52;_tmp4C.elt_tq=_tmp51;_tmp4C.ptr_atts=({(_tmp4C.ptr_atts).rgn=_tmp50;(_tmp4C.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4C.ptr_atts).bounds=_tmp4E;(_tmp4C.ptr_atts).zero_term=_tmp4D;(_tmp4C.ptr_atts).ptrloc=0;_tmp4C.ptr_atts;});_tmp4C;});_tmp4B;});_tmp4A;});}else{_LL54: _LL55:
# 271
 goto _LL51;}_LL51:;}{
# 274
struct Cyc_List_List*_tmp53=Cyc_Tcenv_lookup_type_vars(te);
t=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp55;_tmp55.tag=5;_tmp55.f1=({struct Cyc_Absyn_PtrInfo _tmp56;_tmp56.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->v=_tmp53;_tmp58;}));_tmp56.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp56.ptr_atts=({(_tmp56.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->v=_tmp53;_tmp57;}));(_tmp56.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp56.ptr_atts).bounds=
# 279
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp56.ptr_atts).zero_term=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp56.ptr_atts).ptrloc=0;_tmp56.ptr_atts;});_tmp56;});_tmp55;});_tmp54;});
# 281
goto _LL1D;};}_LL1D:;}
# 283
return t;}
# 287
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf);
# 292
static void*Cyc_Tcexp_tcVar(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,void**b){
void*_tmp60=*((void**)_check_null(b));void*_tmp61=_tmp60;struct Cyc_Absyn_Vardecl*_tmp6F;struct Cyc_Absyn_Vardecl*_tmp6E;struct Cyc_Absyn_Vardecl*_tmp6D;struct Cyc_Absyn_Fndecl*_tmp6C;struct Cyc_Absyn_Vardecl*_tmp6B;struct _tuple1*_tmp6A;switch(*((int*)_tmp61)){case 0: _LL57: _tmp6A=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp61)->f1;_LL58:
# 295
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62[0]=({struct Cyc_Core_Impossible_exn_struct _tmp63;_tmp63.tag=Cyc_Core_Impossible;_tmp63.f1=({const char*_tmp64="unresolved binding in tcVar";_tag_dyneither(_tmp64,sizeof(char),28);});_tmp63;});_tmp62;}));case 1: _LL59: _tmp6B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5A:
# 299
 Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp6B->name,1);
return _tmp6B->type;case 2: _LL5B: _tmp6C=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5C:
# 306
 if(_tmp6C->fn_vardecl == 0)
Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp6C->name,1);
return Cyc_Tcutil_fndecl2typ(_tmp6C);case 5: _LL5D: _tmp6D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5E:
 _tmp6E=_tmp6D;goto _LL60;case 4: _LL5F: _tmp6E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp61)->f1;_LL60:
 _tmp6F=_tmp6E;goto _LL62;default: _LL61: _tmp6F=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp61)->f1;_LL62:
# 312
 if(te->allow_valueof){
void*_tmp65=Cyc_Tcutil_compress(_tmp6F->type);void*_tmp66=_tmp65;void*_tmp69;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66)->tag == 19){_LL64: _tmp69=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp66)->f1;_LL65:
# 315
 e->r=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp68;_tmp68.tag=38;_tmp68.f1=_tmp69;_tmp68;});_tmp67;});
goto _LL63;}else{_LL66: _LL67:
 goto _LL63;}_LL63:;}
# 320
return _tmp6F->type;}_LL56:;}
# 324
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 333
struct Cyc_List_List*desc_types;
{void*_tmp70=fmt->r;void*_tmp71=_tmp70;struct _dyneither_ptr _tmp77;struct _dyneither_ptr _tmp76;switch(*((int*)_tmp71)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp71)->f1).String_c).tag == 8){_LL69: _tmp76=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp71)->f1).String_c).val;_LL6A:
 _tmp77=_tmp76;goto _LL6C;}else{goto _LL6D;}case 13: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->f1).String_c).tag == 8){_LL6B: _tmp77=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->f1).String_c).val;_LL6C:
# 337
 desc_types=type_getter(temp,te,(struct _dyneither_ptr)_tmp77,fmt->loc);goto _LL68;}else{goto _LL6D;}}else{goto _LL6D;}default: _LL6D: _LL6E:
# 341
 if(opt_args != 0){
struct Cyc_List_List*_tmp72=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp72 != 0;_tmp72=_tmp72->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp72->hd);{
struct _RegionHandle _tmp73=_new_region("temp");struct _RegionHandle*temp=& _tmp73;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,(void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp72->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path(temp,(struct Cyc_Absyn_Exp*)_tmp72->hd))
({void*_tmp74=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp72->hd)->loc,({const char*_tmp75="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp75,sizeof(char),49);}),_tag_dyneither(_tmp74,sizeof(void*),0));});
# 346
;_pop_region(temp);};}}
# 352
return;}_LL68:;}
# 354
if(opt_args != 0){
struct Cyc_List_List*_tmp78=(struct Cyc_List_List*)opt_args->v;
# 357
for(0;desc_types != 0  && _tmp78 != 0;
(((desc_types=desc_types->tl,_tmp78=_tmp78->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp78->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp7C;_tmp7C.tag=0;_tmp7C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp7B;_tmp7B.tag=0;_tmp7B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp79[2]={& _tmp7B,& _tmp7C};Cyc_Tcutil_terr(e->loc,({const char*_tmp7A="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp7A,sizeof(char),48);}),_tag_dyneither(_tmp79,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 368
if(alias_coercion)
*alias_arg_exps=({struct Cyc_List_List*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->hd=(void*)arg_cnt;_tmp7D->tl=*alias_arg_exps;_tmp7D;});{
struct _RegionHandle _tmp7E=_new_region("temp");struct _RegionHandle*temp=& _tmp7E;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e))
({void*_tmp7F=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp78->hd)->loc,({const char*_tmp80="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp80,sizeof(char),49);}),_tag_dyneither(_tmp7F,sizeof(void*),0));});
# 371
;_pop_region(temp);};}
# 376
if(desc_types != 0)
({void*_tmp81=0;Cyc_Tcutil_terr(fmt->loc,({const char*_tmp82="too few arguments";_tag_dyneither(_tmp82,sizeof(char),18);}),_tag_dyneither(_tmp81,sizeof(void*),0));});
if(_tmp78 != 0){
({void*_tmp83=0;Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_tmp78->hd)->loc,({const char*_tmp84="too many arguments";_tag_dyneither(_tmp84,sizeof(char),19);}),_tag_dyneither(_tmp83,sizeof(void*),0));});
# 381
for(0;_tmp78 != 0;_tmp78=_tmp78->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp78->hd);}}}}
# 386
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 388
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp85=p;switch(_tmp85){case Cyc_Absyn_Plus: _LL70: _LL71:
 goto _LL73;case Cyc_Absyn_Minus: _LL72: _LL73:
# 392
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp88;_tmp88.tag=0;_tmp88.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp86[1]={& _tmp88};Cyc_Tcutil_terr(loc,({const char*_tmp87="expecting numeric type but found %s";_tag_dyneither(_tmp87,sizeof(char),36);}),_tag_dyneither(_tmp86,sizeof(void*),1));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL74: _LL75:
# 396
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp8B;_tmp8B.tag=0;_tmp8B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp89[1]={& _tmp8B};Cyc_Tcutil_terr(loc,({const char*_tmp8A="expecting integral or * type but found %s";_tag_dyneither(_tmp8A,sizeof(char),42);}),_tag_dyneither(_tmp89,sizeof(void*),1));});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL76: _LL77:
# 401
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp8E;_tmp8E.tag=0;_tmp8E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp8C[1]={& _tmp8E};Cyc_Tcutil_terr(loc,({const char*_tmp8D="expecting integral type but found %s";_tag_dyneither(_tmp8D,sizeof(char),37);}),_tag_dyneither(_tmp8C,sizeof(void*),1));});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL78: _LL79:
# 405
{void*_tmp8F=t;union Cyc_Absyn_Constraint*_tmp98;switch(*((int*)_tmp8F)){case 8: _LL7D: _LL7E:
 goto _LL7C;case 5: _LL7F: _tmp98=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8F)->f1).ptr_atts).bounds;_LL80:
# 408
{void*_tmp90=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp98);void*_tmp91=_tmp90;struct Cyc_Absyn_Exp*_tmp94;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp91)->tag == 0){_LL84: _LL85:
 goto _LL83;}else{_LL86: _tmp94=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp91)->f1;_LL87:
# 411
 if(!Cyc_Evexp_c_can_eval(_tmp94) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp92=0;Cyc_Tcutil_terr(loc,({const char*_tmp93="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp93,sizeof(char),55);}),_tag_dyneither(_tmp92,sizeof(void*),0));});
goto _LL83;}_LL83:;}
# 415
goto _LL7C;default: _LL81: _LL82:
# 417
({struct Cyc_String_pa_PrintArg_struct _tmp97;_tmp97.tag=0;_tmp97.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp95[1]={& _tmp97};Cyc_Tcutil_terr(loc,({const char*_tmp96="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp96,sizeof(char),47);}),_tag_dyneither(_tmp95,sizeof(void*),1));});});}_LL7C:;}
# 419
return Cyc_Absyn_uint_typ;default: _LL7A: _LL7B:
({void*_tmp99=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9A="Non-unary primop";_tag_dyneither(_tmp9A,sizeof(char),17);}),_tag_dyneither(_tmp99,sizeof(void*),0));});}_LL6F:;}
# 425
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 428
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp9D;_tmp9D.tag=0;_tmp9D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));({void*_tmp9B[1]={& _tmp9D};Cyc_Tcutil_terr(e1->loc,({const char*_tmp9C="type %s cannot be used here";_tag_dyneither(_tmp9C,sizeof(char),28);}),_tag_dyneither(_tmp9B,sizeof(void*),1));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 432
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));({void*_tmp9E[1]={& _tmpA0};Cyc_Tcutil_terr(e2->loc,({const char*_tmp9F="type %s cannot be used here";_tag_dyneither(_tmp9F,sizeof(char),28);}),_tag_dyneither(_tmp9E,sizeof(void*),1));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 436
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 441
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmpA1=t1;void*_tmpB7;struct Cyc_Absyn_Tqual _tmpB6;void*_tmpB5;union Cyc_Absyn_Constraint*_tmpB4;union Cyc_Absyn_Constraint*_tmpB3;union Cyc_Absyn_Constraint*_tmpB2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1)->tag == 5){_LL89: _tmpB7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1)->f1).elt_typ;_tmpB6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1)->f1).elt_tq;_tmpB5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1)->f1).ptr_atts).rgn;_tmpB4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1)->f1).ptr_atts).nullable;_tmpB3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1)->f1).ptr_atts).bounds;_tmpB2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA1)->f1).ptr_atts).zero_term;_LL8A:
# 446
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpB7),& Cyc_Tcutil_tmk))
({void*_tmpA2=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpA3="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpA3,sizeof(char),50);}),_tag_dyneither(_tmpA2,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpA4=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpA5="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpA5,sizeof(char),54);}),_tag_dyneither(_tmpA4,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));({void*_tmpA6[1]={& _tmpA8};Cyc_Tcutil_terr(e2->loc,({const char*_tmpA7="expecting int but found %s";_tag_dyneither(_tmpA7,sizeof(char),27);}),_tag_dyneither(_tmpA6,sizeof(void*),1));});});{
void*_tmpA9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB3);void*_tmpAA=_tmpA9;struct Cyc_Absyn_Exp*_tmpB1;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAA)->tag == 0){_LL8E: _LL8F:
 return t1;}else{_LL90: _tmpB1=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAA)->f1;_LL91:
# 457
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB2))
({void*_tmpAB=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpAC="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmpAC,sizeof(char),70);}),_tag_dyneither(_tmpAB,sizeof(void*),0));});{
# 465
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAD=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpAF;_tmpAF.tag=5;_tmpAF.f1=({struct Cyc_Absyn_PtrInfo _tmpB0;_tmpB0.elt_typ=_tmpB7;_tmpB0.elt_tq=_tmpB6;_tmpB0.ptr_atts=({(_tmpB0.ptr_atts).rgn=_tmpB5;(_tmpB0.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmpB0.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref;(_tmpB0.ptr_atts).zero_term=_tmpB2;(_tmpB0.ptr_atts).ptrloc=0;_tmpB0.ptr_atts;});_tmpB0;});_tmpAF;});_tmpAE;});
# 469
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpAD,Cyc_Absyn_Other_coercion);
return(void*)_tmpAD;};}_LL8D:;};}else{_LL8B: _LL8C:
# 472
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL88:;}
# 477
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpBB;_tmpBB.tag=0;_tmpBB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 487
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpBA;_tmpBA.tag=0;_tmpBA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpB8[2]={& _tmpBA,& _tmpBB};Cyc_Tcutil_terr(e1->loc,({const char*_tmpB9="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpB9,sizeof(char),59);}),_tag_dyneither(_tmpB8,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 490
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!Cyc_Tcutil_unify(t1_elt,Cyc_Tcutil_pointer_elt_type(t2))){
({struct Cyc_String_pa_PrintArg_struct _tmpBF;_tmpBF.tag=0;_tmpBF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 495
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpBE;_tmpBE.tag=0;_tmpBE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpBC[2]={& _tmpBE,& _tmpBF};Cyc_Tcutil_terr(e1->loc,({const char*_tmpBD="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpBD,sizeof(char),59);}),_tag_dyneither(_tmpBC,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 499
({void*_tmpC0=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpC1="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpC1,sizeof(char),60);}),_tag_dyneither(_tmpC0,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_star_typ(t1_elt,Cyc_Tcutil_pointer_region(t1),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);
# 503
return Cyc_Absyn_sint_typ;}else{
# 505
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk))
({void*_tmpC2=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpC3="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpC3,sizeof(char),50);}),_tag_dyneither(_tmpC2,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpC4=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmpC5="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpC5,sizeof(char),54);}),_tag_dyneither(_tmpC4,sizeof(void*),0));});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpC9;_tmpC9.tag=0;_tmpC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpC8;_tmpC8.tag=0;_tmpC8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpC6[2]={& _tmpC8,& _tmpC9};Cyc_Tcutil_terr(e2->loc,({const char*_tmpC7="expecting either %s or int but found %s";_tag_dyneither(_tmpC7,sizeof(char),40);}),_tag_dyneither(_tmpC6,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 514
return t1;}}}
# 518
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_unify(Cyc_Tcutil_pointer_elt_type(t1),
Cyc_Tcutil_pointer_elt_type(t2))){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpCA=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpCB="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpCB,sizeof(char),60);}),_tag_dyneither(_tmpCA,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_star_typ(Cyc_Tcutil_pointer_elt_type(t2),
Cyc_Tcutil_pointer_region(t2),
Cyc_Absyn_empty_tqual(0),Cyc_Absyn_false_conref),Cyc_Absyn_Other_coercion);}
# 528
({void*_tmpCC=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpCD="thin pointer subtraction!";_tag_dyneither(_tmpCD,sizeof(char),26);}),_tag_dyneither(_tmpCC,sizeof(void*),0));});
return Cyc_Absyn_sint_typ;}
# 531
({void*_tmpCE=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpCF="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpCF,sizeof(char),56);}),_tag_dyneither(_tmpCE,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 534
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpD0=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmpD1="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpD1,sizeof(char),51);}),_tag_dyneither(_tmpD0,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 539
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 542
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 550
if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind  || 
Cyc_Tcutil_unify(t1,Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te)))){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 555
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 568
pointer_cmp: {
struct _tuple0 _tmpD2=({struct _tuple0 _tmpD6;_tmpD6.f1=Cyc_Tcutil_compress(t1);_tmpD6.f2=Cyc_Tcutil_compress(t2);_tmpD6;});struct _tuple0 _tmpD3=_tmpD2;void*_tmpD5;void*_tmpD4;switch(*((int*)_tmpD3.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD3.f2)->tag == 5){_LL93: _tmpD5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD3.f1)->f1).elt_typ;_tmpD4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD3.f2)->f1).elt_typ;_LL94:
# 572
 if(Cyc_Tcutil_unify(_tmpD5,_tmpD4))return Cyc_Absyn_sint_typ;goto _LL92;}else{goto _LL97;}case 15: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD3.f2)->tag == 15){_LL95: _LL96:
# 574
 return Cyc_Absyn_sint_typ;}else{goto _LL97;}default: _LL97: _LL98:
 goto _LL92;}_LL92:;}
# 577
({struct Cyc_String_pa_PrintArg_struct _tmpDA;_tmpDA.tag=0;_tmpDA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpD9;_tmpD9.tag=0;_tmpD9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpD7[2]={& _tmpD9,& _tmpDA};Cyc_Tcutil_terr(loc,({const char*_tmpD8="comparison not allowed between %s and %s";_tag_dyneither(_tmpD8,sizeof(char),41);}),_tag_dyneither(_tmpD7,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 584
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 586
if(Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2)== Cyc_Absyn_sint_typ)
return Cyc_Absyn_sint_typ;{
# 592
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpDB=({struct _tuple0 _tmpDF;_tmpDF.f1=t1;_tmpDF.f2=t2;_tmpDF;});struct _tuple0 _tmpDC=_tmpDB;void*_tmpDE;void*_tmpDD;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDC.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDC.f2)->tag == 15){_LL9A: _tmpDE=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDC.f1)->f1;_tmpDD=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDC.f2)->f1;_LL9B:
# 597
 return Cyc_Absyn_sint_typ;}else{goto _LL9C;}}else{_LL9C: _LL9D:
 goto _LL99;}_LL99:;}
# 600
({struct Cyc_String_pa_PrintArg_struct _tmpE3;_tmpE3.tag=0;_tmpE3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmpE2;_tmpE2.tag=0;_tmpE2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmpE0[2]={& _tmpE2,& _tmpE3};Cyc_Tcutil_terr(loc,({const char*_tmpE1="comparison not allowed between %s and %s";_tag_dyneither(_tmpE1,sizeof(char),41);}),_tag_dyneither(_tmpE0,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 608
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 610
enum Cyc_Absyn_Primop _tmpE4=p;switch(_tmpE4){case Cyc_Absyn_Plus: _LL9F: _LLA0:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLA1: _LLA2:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLA3: _LLA4:
# 614
 goto _LLA6;case Cyc_Absyn_Div: _LLA5: _LLA6:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLA7: _LLA8:
# 617
 goto _LLAA;case Cyc_Absyn_Bitand: _LLA9: _LLAA:
 goto _LLAC;case Cyc_Absyn_Bitor: _LLAB: _LLAC:
 goto _LLAE;case Cyc_Absyn_Bitxor: _LLAD: _LLAE:
 goto _LLB0;case Cyc_Absyn_Bitlshift: _LLAF: _LLB0:
 goto _LLB2;case Cyc_Absyn_Bitlrshift: _LLB1: _LLB2:
 goto _LLB4;case Cyc_Absyn_Bitarshift: _LLB3: _LLB4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLB5: _LLB6:
# 627
 goto _LLB8;case Cyc_Absyn_Neq: _LLB7: _LLB8:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLB9: _LLBA:
# 630
 goto _LLBC;case Cyc_Absyn_Lt: _LLBB: _LLBC:
 goto _LLBE;case Cyc_Absyn_Gte: _LLBD: _LLBE:
 goto _LLC0;case Cyc_Absyn_Lte: _LLBF: _LLC0:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLC1: _LLC2:
# 635
({void*_tmpE5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpE6="bad binary primop";_tag_dyneither(_tmpE6,sizeof(char),18);}),_tag_dyneither(_tmpE5,sizeof(void*),0));});}_LL9E:;}
# 639
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 647
if(p == Cyc_Absyn_Minus  && ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmpE7=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpE8=Cyc_Tcexp_tcExp(te,topt,_tmpE7);
if(!Cyc_Tcutil_is_numeric(_tmpE7))
({struct Cyc_String_pa_PrintArg_struct _tmpEB;_tmpEB.tag=0;_tmpEB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpE8));({void*_tmpE9[1]={& _tmpEB};Cyc_Tcutil_terr(_tmpE7->loc,({const char*_tmpEA="expecting numeric type but found %s";_tag_dyneither(_tmpEA,sizeof(char),36);}),_tag_dyneither(_tmpE9,sizeof(void*),1));});});
return _tmpE8;}
# 654
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpEC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpED=_tmpEC;switch(_tmpED){case 0: _LLC4: _LLC5:
 return({void*_tmpEE=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmpEF="primitive operator has 0 arguments";_tag_dyneither(_tmpEF,sizeof(char),35);}),_tag_dyneither(_tmpEE,sizeof(void*),0));});case 1: _LLC6: _LLC7:
 t=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);goto _LLC3;case 2: _LLC8: _LLC9:
 t=Cyc_Tcexp_tcBinPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*)_check_null(es))->tl))->hd);goto _LLC3;default: _LLCA: _LLCB:
 return({void*_tmpF0=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmpF1="primitive operator has > 2 arguments";_tag_dyneither(_tmpF1,sizeof(char),37);}),_tag_dyneither(_tmpF0,sizeof(void*),0));});}_LLC3:;}
# 662
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 665
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpF2=Cyc_Tcutil_compress(t);
void*_tmpF3=_tmpF2;struct Cyc_List_List*_tmp10F;void*_tmp10E;struct Cyc_Absyn_Tqual _tmp10D;struct Cyc_Absyn_Datatypefield*_tmp10C;struct Cyc_List_List*_tmp10B;struct Cyc_Absyn_Aggrdecl*_tmp10A;switch(*((int*)_tmpF3)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF3)->f1).aggr_info).KnownAggr).tag == 2){_LLCD: _tmp10A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF3)->f1).aggr_info).KnownAggr).val;_LLCE:
# 669
 if(_tmp10A->impl == 0){
({void*_tmpF4=0;Cyc_Tcutil_terr(loc,({const char*_tmpF5="attempt to write an abstract aggregate";_tag_dyneither(_tmpF5,sizeof(char),39);}),_tag_dyneither(_tmpF4,sizeof(void*),0));});
return 0;}else{
# 673
_tmp10B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp10A->impl))->fields;goto _LLD0;}}else{goto _LLD7;}case 12: _LLCF: _tmp10B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF3)->f2;_LLD0:
# 675
 for(0;_tmp10B != 0;_tmp10B=_tmp10B->tl){
struct Cyc_Absyn_Aggrfield*_tmpF6=(struct Cyc_Absyn_Aggrfield*)_tmp10B->hd;
if((_tmpF6->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpF9;_tmpF9.tag=0;_tmpF9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF6->name);({void*_tmpF7[1]={& _tmpF9};Cyc_Tcutil_terr(loc,({const char*_tmpF8="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpF8,sizeof(char),56);}),_tag_dyneither(_tmpF7,sizeof(void*),1));});});
return 0;}
# 681
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF6->type))return 0;}
# 683
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF3)->f1).field_info).KnownDatatypefield).tag == 2){_LLD1: _tmp10C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF3)->f1).field_info).KnownDatatypefield).val).f2;_LLD2:
# 685
{struct Cyc_List_List*_tmpFA=_tmp10C->typs;for(0;_tmpFA != 0;_tmpFA=_tmpFA->tl){
struct _tuple17*_tmpFB=(struct _tuple17*)_tmpFA->hd;struct _tuple17*_tmpFC=_tmpFB;struct Cyc_Absyn_Tqual _tmp101;void*_tmp100;_LLDA: _tmp101=_tmpFC->f1;_tmp100=_tmpFC->f2;_LLDB:;
if(_tmp101.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpFF;_tmpFF.tag=0;_tmpFF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp10C->name));({void*_tmpFD[1]={& _tmpFF};Cyc_Tcutil_terr(loc,({const char*_tmpFE="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpFE,sizeof(char),64);}),_tag_dyneither(_tmpFD,sizeof(void*),1));});});
return 0;}
# 691
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp100))return 0;}}
# 693
return 1;}else{goto _LLD7;}case 8: _LLD3: _tmp10E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3)->f1).elt_type;_tmp10D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF3)->f1).tq;_LLD4:
# 695
 if(_tmp10D.real_const){
({void*_tmp102=0;Cyc_Tcutil_terr(loc,({const char*_tmp103="attempt to over-write a const array";_tag_dyneither(_tmp103,sizeof(char),36);}),_tag_dyneither(_tmp102,sizeof(void*),0));});
return 0;}
# 699
return Cyc_Tcexp_check_writable_aggr(loc,_tmp10E);case 10: _LLD5: _tmp10F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF3)->f1;_LLD6:
# 701
 for(0;_tmp10F != 0;_tmp10F=_tmp10F->tl){
struct _tuple17*_tmp104=(struct _tuple17*)_tmp10F->hd;struct _tuple17*_tmp105=_tmp104;struct Cyc_Absyn_Tqual _tmp109;void*_tmp108;_LLDD: _tmp109=_tmp105->f1;_tmp108=_tmp105->f2;_LLDE:;
if(_tmp109.real_const){
({void*_tmp106=0;Cyc_Tcutil_terr(loc,({const char*_tmp107="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmp107,sizeof(char),56);}),_tag_dyneither(_tmp106,sizeof(void*),0));});
return 0;}
# 707
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp108))return 0;}
# 709
return 1;default: _LLD7: _LLD8:
 return 1;}_LLCC:;}
# 717
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 720
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp110=e->r;void*_tmp111=_tmp110;struct Cyc_Absyn_Exp*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct _dyneither_ptr*_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;struct _dyneither_ptr*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Exp*_tmp138;struct Cyc_Absyn_Vardecl*_tmp137;struct Cyc_Absyn_Vardecl*_tmp136;struct Cyc_Absyn_Vardecl*_tmp135;struct Cyc_Absyn_Vardecl*_tmp134;switch(*((int*)_tmp111)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp111)->f1)){case 3: _LLE0: _tmp134=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp111)->f1)->f1;_LLE1:
 _tmp135=_tmp134;goto _LLE3;case 4: _LLE2: _tmp135=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp111)->f1)->f1;_LLE3:
 _tmp136=_tmp135;goto _LLE5;case 5: _LLE4: _tmp136=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp111)->f1)->f1;_LLE5:
 _tmp137=_tmp136;goto _LLE7;case 1: _LLE6: _tmp137=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp111)->f1)->f1;_LLE7:
 if(!(_tmp137->tq).real_const)return;goto _LLDF;default: goto _LLF4;}case 22: _LLE8: _tmp139=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp111)->f1;_tmp138=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp111)->f2;_LLE9:
# 727
{void*_tmp112=Cyc_Tcutil_compress((void*)_check_null(_tmp139->topt));void*_tmp113=_tmp112;struct Cyc_List_List*_tmp124;struct Cyc_Absyn_Tqual _tmp123;struct Cyc_Absyn_Tqual _tmp122;switch(*((int*)_tmp113)){case 5: _LLF7: _tmp122=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp113)->f1).elt_tq;_LLF8:
 _tmp123=_tmp122;goto _LLFA;case 8: _LLF9: _tmp123=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp113)->f1).tq;_LLFA:
 if(!_tmp123.real_const)return;goto _LLF6;case 10: _LLFB: _tmp124=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp113)->f1;_LLFC: {
# 731
struct _tuple15 _tmp114=Cyc_Evexp_eval_const_uint_exp(_tmp138);struct _tuple15 _tmp115=_tmp114;unsigned int _tmp121;int _tmp120;_LL100: _tmp121=_tmp115.f1;_tmp120=_tmp115.f2;_LL101:;
if(!_tmp120){
({void*_tmp116=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp117="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp117,sizeof(char),47);}),_tag_dyneither(_tmp116,sizeof(void*),0));});
return;}
# 736
{struct _handler_cons _tmp118;_push_handler(& _tmp118);{int _tmp11A=0;if(setjmp(_tmp118.handler))_tmp11A=1;if(!_tmp11A){
{struct _tuple17*_tmp11B=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp124,(int)_tmp121);struct _tuple17*_tmp11C=_tmp11B;struct Cyc_Absyn_Tqual _tmp11D;_LL103: _tmp11D=_tmp11C->f1;_LL104:;
if(!_tmp11D.real_const){_npop_handler(0);return;}}
# 737
;_pop_handler();}else{void*_tmp119=(void*)_exn_thrown;void*_tmp11E=_tmp119;void*_tmp11F;if(((struct Cyc_List_Nth_exn_struct*)_tmp11E)->tag == Cyc_List_Nth){_LL106: _LL107:
# 739
 return;}else{_LL108: _tmp11F=_tmp11E;_LL109:(int)_rethrow(_tmp11F);}_LL105:;}};}
goto _LLF6;}default: _LLFD: _LLFE:
 goto _LLF6;}_LLF6:;}
# 743
goto _LLDF;case 20: _LLEA: _tmp13B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp111)->f1;_tmp13A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp111)->f2;_LLEB:
# 745
{void*_tmp125=Cyc_Tcutil_compress((void*)_check_null(_tmp13B->topt));void*_tmp126=_tmp125;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_Aggrdecl**_tmp127;switch(*((int*)_tmp126)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp126)->f1).aggr_info).KnownAggr).tag == 2){_LL10B: _tmp127=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp126)->f1).aggr_info).KnownAggr).val;_LL10C: {
# 747
struct Cyc_Absyn_Aggrfield*sf=
_tmp127 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp127,_tmp13A);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}}else{goto _LL10F;}case 12: _LL10D: _tmp128=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp126)->f2;_LL10E: {
# 752
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp128,_tmp13A);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}default: _LL10F: _LL110:
 goto _LL10A;}_LL10A:;}
# 757
goto _LLDF;case 21: _LLEC: _tmp13D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp111)->f1;_tmp13C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp111)->f2;_LLED:
# 759
{void*_tmp129=Cyc_Tcutil_compress((void*)_check_null(_tmp13D->topt));void*_tmp12A=_tmp129;void*_tmp12F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12A)->tag == 5){_LL112: _tmp12F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12A)->f1).elt_typ;_LL113:
# 761
{void*_tmp12B=Cyc_Tcutil_compress(_tmp12F);void*_tmp12C=_tmp12B;struct Cyc_List_List*_tmp12E;struct Cyc_Absyn_Aggrdecl**_tmp12D;switch(*((int*)_tmp12C)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12C)->f1).aggr_info).KnownAggr).tag == 2){_LL117: _tmp12D=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12C)->f1).aggr_info).KnownAggr).val;_LL118: {
# 763
struct Cyc_Absyn_Aggrfield*sf=
_tmp12D == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp12D,_tmp13C);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}}else{goto _LL11B;}case 12: _LL119: _tmp12E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12C)->f2;_LL11A: {
# 768
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp12E,_tmp13C);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}default: _LL11B: _LL11C:
 goto _LL116;}_LL116:;}
# 773
goto _LL111;}else{_LL114: _LL115:
 goto _LL111;}_LL111:;}
# 776
goto _LLDF;case 19: _LLEE: _tmp13E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp111)->f1;_LLEF:
# 778
{void*_tmp130=Cyc_Tcutil_compress((void*)_check_null(_tmp13E->topt));void*_tmp131=_tmp130;struct Cyc_Absyn_Tqual _tmp133;struct Cyc_Absyn_Tqual _tmp132;switch(*((int*)_tmp131)){case 5: _LL11E: _tmp132=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->f1).elt_tq;_LL11F:
 _tmp133=_tmp132;goto _LL121;case 8: _LL120: _tmp133=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->f1).tq;_LL121:
 if(!_tmp133.real_const)return;goto _LL11D;default: _LL122: _LL123:
 goto _LL11D;}_LL11D:;}
# 783
goto _LLDF;case 11: _LLF0: _tmp13F=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp111)->f1;_LLF1:
 _tmp140=_tmp13F;goto _LLF3;case 12: _LLF2: _tmp140=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp111)->f1;_LLF3:
 Cyc_Tcexp_check_writable(te,_tmp140);return;default: _LLF4: _LLF5:
 goto _LLDF;}_LLDF:;}
# 788
({struct Cyc_String_pa_PrintArg_struct _tmp143;_tmp143.tag=0;_tmp143.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp141[1]={& _tmp143};Cyc_Tcutil_terr(e->loc,({const char*_tmp142="attempt to write a const location: %s";_tag_dyneither(_tmp142,sizeof(char),38);}),_tag_dyneither(_tmp141,sizeof(void*),1));});});}
# 791
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 794
struct _RegionHandle _tmp144=_new_region("temp");struct _RegionHandle*temp=& _tmp144;_push_region(temp);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(temp,te),0,e);
# 797
if(!Cyc_Absyn_is_lvalue(e)){
void*_tmp147=({void*_tmp145=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp146="increment/decrement of non-lvalue";_tag_dyneither(_tmp146,sizeof(char),34);}),_tag_dyneither(_tmp145,sizeof(void*),0));});_npop_handler(0);return _tmp147;}
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 802
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk))
({void*_tmp148=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp149="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp149,sizeof(char),50);}),_tag_dyneither(_tmp148,sizeof(void*),0));});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp14A=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp14B="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp14B,sizeof(char),54);}),_tag_dyneither(_tmp14A,sizeof(void*),0));});}else{
# 811
({struct Cyc_String_pa_PrintArg_struct _tmp14E;_tmp14E.tag=0;_tmp14E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp14C[1]={& _tmp14E};Cyc_Tcutil_terr(e->loc,({const char*_tmp14D="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp14D,sizeof(char),44);}),_tag_dyneither(_tmp14C,sizeof(void*),1));});});}}{
# 813
void*_tmp14F=t;_npop_handler(0);return _tmp14F;};};
# 794
;_pop_region(temp);}
# 817
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 819
struct _RegionHandle _tmp150=_new_region("r");struct _RegionHandle*r=& _tmp150;_push_region(r);
Cyc_Tcexp_tcTest(Cyc_Tcenv_clear_abstract_val_ok(r,te),e1,({const char*_tmp151="conditional expression";_tag_dyneither(_tmp151,sizeof(char),23);}));
# 822
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 828
t=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));}{
struct Cyc_List_List _tmp152=({struct Cyc_List_List _tmp15A;_tmp15A.hd=e3;_tmp15A.tl=0;_tmp15A;});
struct Cyc_List_List _tmp153=({struct Cyc_List_List _tmp159;_tmp159.hd=e2;_tmp159.tl=& _tmp152;_tmp159;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp153)){
({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0;_tmp157.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0;_tmp156.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));({void*_tmp154[2]={& _tmp156,& _tmp157};Cyc_Tcutil_terr(loc,({const char*_tmp155="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp155,sizeof(char),48);}),_tag_dyneither(_tmp154,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}{
# 836
void*_tmp158=t;_npop_handler(0);return _tmp158;};};};
# 819
;_pop_region(r);}
# 840
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 842
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp15B="logical-and expression";_tag_dyneither(_tmp15B,sizeof(char),23);}));
Cyc_Tcexp_tcTest(te,e2,({const char*_tmp15C="logical-and expression";_tag_dyneither(_tmp15C,sizeof(char),23);}));
return Cyc_Absyn_sint_typ;}
# 848
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 850
Cyc_Tcexp_tcTest(te,e1,({const char*_tmp15D="logical-or expression";_tag_dyneither(_tmp15D,sizeof(char),22);}));
Cyc_Tcexp_tcTest(te,e2,({const char*_tmp15E="logical-or expression";_tag_dyneither(_tmp15E,sizeof(char),22);}));
return Cyc_Absyn_sint_typ;}
# 856
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 863
{struct _RegionHandle _tmp15F=_new_region("r");struct _RegionHandle*r=& _tmp15F;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_enter_notreadctxt(r,te)),0,e1);;_pop_region(r);}{
# 866
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 870
{void*_tmp160=Cyc_Tcutil_compress(t1);void*_tmp161=_tmp160;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp161)->tag == 8){_LL125: _LL126:
({void*_tmp162=0;Cyc_Tcutil_terr(loc,({const char*_tmp163="cannot assign to an array";_tag_dyneither(_tmp163,sizeof(char),26);}),_tag_dyneither(_tmp162,sizeof(void*),0));});goto _LL124;}else{_LL127: _LL128:
 goto _LL124;}_LL124:;}
# 875
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk))
({void*_tmp164=0;Cyc_Tcutil_terr(loc,({const char*_tmp165="type is abstract (can't determine size).";_tag_dyneither(_tmp165,sizeof(char),41);}),_tag_dyneither(_tmp164,sizeof(void*),0));});
# 879
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp166=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp167="assignment to non-lvalue";_tag_dyneither(_tmp167,sizeof(char),25);}),_tag_dyneither(_tmp166,sizeof(void*),0));});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
{struct _RegionHandle _tmp168=_new_region("temp");struct _RegionHandle*temp=& _tmp168;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t2) && !Cyc_Tcutil_is_noalias_path(temp,e2))
({void*_tmp169=0;Cyc_Tcutil_terr(e2->loc,({const char*_tmp16A="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp16A,sizeof(char),49);}),_tag_dyneither(_tmp169,sizeof(void*),0));});
# 884
;_pop_region(temp);}
# 887
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp16B=({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0;_tmp16F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp16E;_tmp16E.tag=0;_tmp16E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp16C[2]={& _tmp16E,& _tmp16F};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp16D="type mismatch: %s != %s";_tag_dyneither(_tmp16D,sizeof(char),24);}),_tag_dyneither(_tmp16C,sizeof(void*),2));});});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp16B;}}else{
# 895
enum Cyc_Absyn_Primop _tmp170=(enum Cyc_Absyn_Primop)po->v;
void*_tmp171=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp170,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp171,t1) || Cyc_Tcutil_coerceable(_tmp171) && Cyc_Tcutil_coerceable(t1))){
void*_tmp172=({struct Cyc_String_pa_PrintArg_struct _tmp176;_tmp176.tag=0;_tmp176.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 901
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp175;_tmp175.tag=0;_tmp175.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 900
Cyc_Absynpp_typ2string(t1));({void*_tmp173[2]={& _tmp175,& _tmp176};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp174="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp174,sizeof(char),82);}),_tag_dyneither(_tmp173,sizeof(void*),2));});});});
# 902
Cyc_Tcutil_unify(_tmp171,t1);
Cyc_Tcutil_explain_failure();
return _tmp172;}
# 906
return _tmp171;}
# 908
return t1;};};}
# 912
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _RegionHandle _tmp177=_new_region("r");struct _RegionHandle*r=& _tmp177;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),topt,e2);
# 914
;_pop_region(r);}
# 917
return(void*)_check_null(e2->topt);}
# 921
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 925
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 928
{void*_tmp178=Cyc_Tcutil_compress(t1);void*_tmp179=_tmp178;switch(*((int*)_tmp179)){case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp179)->f1 == 0){_LL12A: _LL12B:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);t1=Cyc_Absyn_float_typ(1);goto _LL129;}else{goto _LL130;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp179)->f2){case Cyc_Absyn_Char_sz: _LL12C: _LL12D:
 goto _LL12F;case Cyc_Absyn_Short_sz: _LL12E: _LL12F:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL129;default: goto _LL130;}default: _LL130: _LL131:
 goto _LL129;}_LL129:;}
# 935
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp17A=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp17B=_tmp17A;struct _tuple1*_tmp17F;struct Cyc_List_List*_tmp17E;unsigned int _tmp17D;enum Cyc_Absyn_Scope _tmp17C;_LL133: _tmp17F=_tmp17B.name;_tmp17E=_tmp17B.typs;_tmp17D=_tmp17B.loc;_tmp17C=_tmp17B.sc;_LL134:;
# 938
if(_tmp17E == 0  || _tmp17E->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp17E->hd)).f2);
# 941
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 946
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp180=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp181=_tmp180;struct _tuple1*_tmp185;struct Cyc_List_List*_tmp184;unsigned int _tmp183;enum Cyc_Absyn_Scope _tmp182;_LL136: _tmp185=_tmp181.name;_tmp184=_tmp181.typs;_tmp183=_tmp181.loc;_tmp182=_tmp181.sc;_LL137:;
# 949
if(_tmp184 == 0  || _tmp184->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp184->hd)).f2);
# 953
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 958
({struct Cyc_String_pa_PrintArg_struct _tmp189;_tmp189.tag=0;_tmp189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_PrintArg_struct _tmp188;_tmp188.tag=0;_tmp188.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));({void*_tmp186[2]={& _tmp188,& _tmp189};Cyc_Tcutil_terr(e->loc,({const char*_tmp187="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp187,sizeof(char),54);}),_tag_dyneither(_tmp186,sizeof(void*),2));});});});
return 0;}
# 964
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 970
struct Cyc_List_List*_tmp18A=args;
int _tmp18B=0;
struct _RegionHandle _tmp18C=_new_region("ter");struct _RegionHandle*ter=& _tmp18C;_push_region(ter);
{struct Cyc_Tcenv_Tenv*_tmp18D=Cyc_Tcenv_new_block(ter,loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp18E=Cyc_Tcenv_clear_abstract_val_ok(ter,_tmp18D);
Cyc_Tcexp_tcExp(_tmp18E,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 980
void*_tmp18F=t;void*_tmp1F7;struct Cyc_Absyn_Tqual _tmp1F6;void*_tmp1F5;union Cyc_Absyn_Constraint*_tmp1F4;union Cyc_Absyn_Constraint*_tmp1F3;union Cyc_Absyn_Constraint*_tmp1F2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->tag == 5){_LL139: _tmp1F7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).elt_typ;_tmp1F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).elt_tq;_tmp1F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).rgn;_tmp1F4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).nullable;_tmp1F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).bounds;_tmp1F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1).ptr_atts).zero_term;_LL13A:
# 985
 Cyc_Tcenv_check_rgn_accessible(_tmp18E,loc,_tmp1F5);
# 987
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1F3);{
void*_tmp190=Cyc_Tcutil_compress(_tmp1F7);void*_tmp191=_tmp190;struct Cyc_List_List*_tmp1ED;void*_tmp1EC;struct Cyc_Absyn_Tqual _tmp1EB;void*_tmp1EA;struct Cyc_List_List*_tmp1E9;int _tmp1E8;struct Cyc_Absyn_VarargInfo*_tmp1E7;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E5;struct Cyc_Absyn_Exp*_tmp1E4;struct Cyc_List_List*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E2;struct Cyc_List_List*_tmp1E1;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->tag == 9){_LL13E: _tmp1ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).tvars;_tmp1EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).effect;_tmp1EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).ret_tqual;_tmp1EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).ret_typ;_tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).args;_tmp1E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).c_varargs;_tmp1E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).cyc_varargs;_tmp1E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).rgn_po;_tmp1E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).attributes;_tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).requires_clause;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).requires_relns;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).ensures_clause;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp191)->f1).ensures_relns;_LL13F:
# 992
 if(_tmp1ED != 0  || _tmp1E6 != 0)
({void*_tmp192=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp193="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp193,sizeof(char),79);}),_tag_dyneither(_tmp192,sizeof(void*),0));});
# 996
if(topt != 0)Cyc_Tcutil_unify(_tmp1EA,*topt);
# 998
while(_tmp18A != 0  && _tmp1E9 != 0){
# 1000
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18A->hd;
void*t2=(*((struct _tuple9*)_tmp1E9->hd)).f3;
Cyc_Tcexp_tcExp(_tmp18E,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp18E,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp1A3="actual argument has type ";_tag_dyneither(_tmp1A3,sizeof(char),26);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp1A2=" but formal has type ";_tag_dyneither(_tmp1A2,sizeof(char),22);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(((Cyc_strlen((struct _dyneither_ptr)s0)+ Cyc_strlen((struct _dyneither_ptr)s1))+ Cyc_strlen((struct _dyneither_ptr)s2))+ Cyc_strlen((struct _dyneither_ptr)s3)>= 70)
({void*_tmp194=0;Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp19A;_tmp19A.tag=0;_tmp19A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0;_tmp199.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0;_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp197;_tmp197.tag=0;_tmp197.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp195[4]={& _tmp197,& _tmp198,& _tmp199,& _tmp19A};Cyc_aprintf(({const char*_tmp196="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp196,sizeof(char),15);}),_tag_dyneither(_tmp195,sizeof(void*),4));});});});});}),_tag_dyneither(_tmp194,sizeof(void*),0));});else{
# 1012
({void*_tmp19B=0;Cyc_Tcutil_terr(e1->loc,(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1A1;_tmp1A1.tag=0;_tmp1A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp1A0;_tmp1A0.tag=0;_tmp1A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp19F;_tmp19F.tag=0;_tmp19F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp19C[4]={& _tmp19E,& _tmp19F,& _tmp1A0,& _tmp1A1};Cyc_aprintf(({const char*_tmp19D="%s%s%s%s.";_tag_dyneither(_tmp19D,sizeof(char),10);}),_tag_dyneither(_tmp19C,sizeof(void*),4));});});});});}),_tag_dyneither(_tmp19B,sizeof(void*),0));});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1017
if(alias_coercion)
*alias_arg_exps=({struct Cyc_List_List*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->hd=(void*)_tmp18B;_tmp1A4->tl=*alias_arg_exps;_tmp1A4;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,t2) && !Cyc_Tcutil_is_noalias_path(ter,e1))
({void*_tmp1A5=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1A6="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1A6,sizeof(char),49);}),_tag_dyneither(_tmp1A5,sizeof(void*),0));});
_tmp18A=_tmp18A->tl;
_tmp1E9=_tmp1E9->tl;
++ _tmp18B;}{
# 1028
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1E5;for(0;a != 0;a=a->tl){
void*_tmp1A7=(void*)a->hd;void*_tmp1A8=_tmp1A7;enum Cyc_Absyn_Format_Type _tmp1B6;int _tmp1B5;int _tmp1B4;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A8)->tag == 19){_LL143: _tmp1B6=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A8)->f1;_tmp1B5=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A8)->f2;_tmp1B4=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp1A8)->f3;_LL144:
# 1032
{struct _handler_cons _tmp1A9;_push_handler(& _tmp1A9);{int _tmp1AB=0;if(setjmp(_tmp1A9.handler))_tmp1AB=1;if(!_tmp1AB){
# 1034
{struct Cyc_Absyn_Exp*_tmp1AC=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1B5 - 1);
# 1036
struct Cyc_Core_Opt*fmt_args;
if(_tmp1B4 == 0)
fmt_args=0;else{
# 1040
fmt_args=({struct Cyc_Core_Opt*_tmp1AD=_cycalloc(sizeof(*_tmp1AD));_tmp1AD->v=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1B4 - 1);_tmp1AD;});}
args_already_checked=1;{
struct _RegionHandle _tmp1AE=_new_region("temp");struct _RegionHandle*temp=& _tmp1AE;_push_region(temp);
{enum Cyc_Absyn_Format_Type _tmp1AF=_tmp1B6;if(_tmp1AF == Cyc_Absyn_Printf_ft){_LL148: _LL149:
# 1045
 Cyc_Tcexp_check_format_args(_tmp18E,_tmp1AC,fmt_args,_tmp1B4 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_format_typs);
# 1048
goto _LL147;}else{_LL14A: _LL14B:
# 1050
 Cyc_Tcexp_check_format_args(_tmp18E,_tmp1AC,fmt_args,_tmp1B4 - 1,alias_arg_exps,temp,Cyc_Formatstr_get_scanf_typs);
# 1053
goto _LL147;}_LL147:;}
# 1043
;_pop_region(temp);};}
# 1034
;_pop_handler();}else{void*_tmp1AA=(void*)_exn_thrown;void*_tmp1B0=_tmp1AA;void*_tmp1B3;if(((struct Cyc_List_Nth_exn_struct*)_tmp1B0)->tag == Cyc_List_Nth){_LL14D: _LL14E:
# 1057
({void*_tmp1B1=0;Cyc_Tcutil_terr(loc,({const char*_tmp1B2="bad format arguments";_tag_dyneither(_tmp1B2,sizeof(char),21);}),_tag_dyneither(_tmp1B1,sizeof(void*),0));});goto _LL14C;}else{_LL14F: _tmp1B3=_tmp1B0;_LL150:(int)_rethrow(_tmp1B3);}_LL14C:;}};}
# 1059
goto _LL142;}else{_LL145: _LL146:
 goto _LL142;}_LL142:;}}
# 1063
if(_tmp1E9 != 0)({void*_tmp1B7=0;Cyc_Tcutil_terr(loc,({const char*_tmp1B8="too few arguments for function";_tag_dyneither(_tmp1B8,sizeof(char),31);}),_tag_dyneither(_tmp1B7,sizeof(void*),0));});else{
# 1065
if((_tmp18A != 0  || _tmp1E8) || _tmp1E7 != 0){
if(_tmp1E8)
for(0;_tmp18A != 0;_tmp18A=_tmp18A->tl){
Cyc_Tcexp_tcExp(_tmp18E,0,(struct Cyc_Absyn_Exp*)_tmp18A->hd);}else{
if(_tmp1E7 == 0)
({void*_tmp1B9=0;Cyc_Tcutil_terr(loc,({const char*_tmp1BA="too many arguments for function";_tag_dyneither(_tmp1BA,sizeof(char),32);}),_tag_dyneither(_tmp1B9,sizeof(void*),0));});else{
# 1072
struct Cyc_Absyn_VarargInfo _tmp1BB=*_tmp1E7;struct Cyc_Absyn_VarargInfo _tmp1BC=_tmp1BB;void*_tmp1DB;int _tmp1DA;_LL152: _tmp1DB=_tmp1BC.type;_tmp1DA=_tmp1BC.inject;_LL153:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1BD=({struct Cyc_Absyn_VarargCallInfo*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->num_varargs=0;_tmp1D9->injectors=0;_tmp1D9->vai=_tmp1E7;_tmp1D9;});
# 1076
*vararg_call_info=_tmp1BD;
# 1078
if(!_tmp1DA)
# 1080
for(0;_tmp18A != 0;(_tmp18A=_tmp18A->tl,_tmp18B ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18A->hd;
++ _tmp1BD->num_varargs;
Cyc_Tcexp_tcExp(_tmp18E,& _tmp1DB,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp18E,e1,_tmp1DB,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1C1;_tmp1C1.tag=0;_tmp1C1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp1C0;_tmp1C0.tag=0;_tmp1C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1DB));({void*_tmp1BE[2]={& _tmp1C0,& _tmp1C1};Cyc_Tcutil_terr(loc,({const char*_tmp1BF="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1BF,sizeof(char),49);}),_tag_dyneither(_tmp1BE,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();}
# 1090
if(alias_coercion)
*alias_arg_exps=({struct Cyc_List_List*_tmp1C2=_cycalloc(sizeof(*_tmp1C2));_tmp1C2->hd=(void*)_tmp18B;_tmp1C2->tl=*alias_arg_exps;_tmp1C2;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(ter,_tmp1DB) && !
Cyc_Tcutil_is_noalias_path(ter,e1))
({void*_tmp1C3=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1C4="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1C4,sizeof(char),49);}),_tag_dyneither(_tmp1C3,sizeof(void*),0));});}else{
# 1099
void*_tmp1C5=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1DB));void*_tmp1C6=_tmp1C5;struct Cyc_Absyn_Datatypedecl*_tmp1D8;struct Cyc_List_List*_tmp1D7;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C6)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C6)->f1).datatype_info).KnownDatatype).tag == 2){_LL155: _tmp1D8=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C6)->f1).datatype_info).KnownDatatype).val;_tmp1D7=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C6)->f1).targs;_LL156: {
# 1103
struct Cyc_Absyn_Datatypedecl*_tmp1C7=*Cyc_Tcenv_lookup_datatypedecl(_tmp18E,loc,_tmp1D8->name);
struct Cyc_List_List*fields=0;
if(_tmp1C7->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1CA;_tmp1CA.tag=0;_tmp1CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1DB));({void*_tmp1C8[1]={& _tmp1CA};Cyc_Tcutil_terr(loc,({const char*_tmp1C9="can't inject into abstract datatype %s";_tag_dyneither(_tmp1C9,sizeof(char),39);}),_tag_dyneither(_tmp1C8,sizeof(void*),1));});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1C7->fields))->v;}
# 1115
if(!Cyc_Tcutil_unify(Cyc_Tcutil_pointer_region(_tmp1DB),Cyc_Tcenv_curr_rgn(_tmp18E)))
({struct Cyc_String_pa_PrintArg_struct _tmp1CE;_tmp1CE.tag=0;_tmp1CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp18E)));({struct Cyc_String_pa_PrintArg_struct _tmp1CD;_tmp1CD.tag=0;_tmp1CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1DB));({void*_tmp1CB[2]={& _tmp1CD,& _tmp1CE};Cyc_Tcutil_terr(loc,({const char*_tmp1CC="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1CC,sizeof(char),49);}),_tag_dyneither(_tmp1CB,sizeof(void*),2));});});});{
# 1119
struct _RegionHandle _tmp1CF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp1CF;_push_region(rgn);{
struct Cyc_List_List*_tmp1D0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp1C7->tvs,_tmp1D7);
for(0;_tmp18A != 0;_tmp18A=_tmp18A->tl){
++ _tmp1BD->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp18A->hd;
# 1125
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp18E,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(rgn,e1))
({void*_tmp1D1=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp1D2="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1D2,sizeof(char),49);}),_tag_dyneither(_tmp1D1,sizeof(void*),0));});}{
# 1131
struct Cyc_Absyn_Datatypefield*_tmp1D3=Cyc_Tcexp_tcInjection(_tmp18E,e1,Cyc_Tcutil_pointer_elt_type(_tmp1DB),rgn,_tmp1D0,fields);
# 1133
if(_tmp1D3 != 0)
_tmp1BD->injectors=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BD->injectors,({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));_tmp1D4->hd=_tmp1D3;_tmp1D4->tl=0;_tmp1D4;}));};};}}
# 1139
_npop_handler(0);goto _LL154;
# 1119
;_pop_region(rgn);};}}else{goto _LL157;}}else{_LL157: _LL158:
# 1140
({void*_tmp1D5=0;Cyc_Tcutil_terr(loc,({const char*_tmp1D6="bad inject vararg type";_tag_dyneither(_tmp1D6,sizeof(char),23);}),_tag_dyneither(_tmp1D5,sizeof(void*),0));});goto _LL154;}_LL154:;}};}}}}
# 1145
if(*alias_arg_exps == 0)
# 1154 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp18E,loc,(void*)_check_null(_tmp1EC));{
# 1156
void*_tmp1DC=_tmp1EA;_npop_handler(0);return _tmp1DC;};};}else{_LL140: _LL141: {
void*_tmp1E0=({struct Cyc_String_pa_PrintArg_struct _tmp1DF;_tmp1DF.tag=0;_tmp1DF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1DD[1]={& _tmp1DF};Cyc_Tcexp_expr_err(_tmp18E,loc,topt,({const char*_tmp1DE="expected pointer to function but found %s";_tag_dyneither(_tmp1DE,sizeof(char),42);}),_tag_dyneither(_tmp1DD,sizeof(void*),1));});});_npop_handler(0);return _tmp1E0;}}_LL13D:;};}else{_LL13B: _LL13C: {
# 1159
void*_tmp1F1=({struct Cyc_String_pa_PrintArg_struct _tmp1F0;_tmp1F0.tag=0;_tmp1F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1EE[1]={& _tmp1F0};Cyc_Tcexp_expr_err(_tmp18E,loc,topt,({const char*_tmp1EF="expected pointer to function but found %s";_tag_dyneither(_tmp1EF,sizeof(char),42);}),_tag_dyneither(_tmp1EE,sizeof(void*),1));});});_npop_handler(0);return _tmp1F1;}}_LL138:;};}
# 973 "tcexp.cyc"
;_pop_region(ter);}
# 1165 "tcexp.cyc"
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1F8=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1F8,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1F8,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1FC;_tmp1FC.tag=0;_tmp1FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp1FB;_tmp1FB.tag=0;_tmp1FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1170
Cyc_Absynpp_typ2string(_tmp1F8));({void*_tmp1F9[2]={& _tmp1FB,& _tmp1FC};Cyc_Tcutil_terr(loc,({const char*_tmp1FA="expected %s but found %s";_tag_dyneither(_tmp1FA,sizeof(char),25);}),_tag_dyneither(_tmp1F9,sizeof(void*),2));});});});
# 1172
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1177
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1179
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1181
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1FD=t1;void*_tmp21F;struct Cyc_Absyn_Tqual _tmp21E;void*_tmp21D;union Cyc_Absyn_Constraint*_tmp21C;union Cyc_Absyn_Constraint*_tmp21B;union Cyc_Absyn_Constraint*_tmp21A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD)->tag == 5){_LL15A: _tmp21F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD)->f1).elt_typ;_tmp21E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD)->f1).elt_tq;_tmp21D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD)->f1).ptr_atts).rgn;_tmp21C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD)->f1).ptr_atts).nullable;_tmp21B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD)->f1).ptr_atts).bounds;_tmp21A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1FD)->f1).ptr_atts).zero_term;_LL15B:
# 1185
{void*_tmp1FE=Cyc_Tcutil_compress(_tmp21F);void*_tmp1FF=_tmp1FE;struct Cyc_List_List*_tmp219;void*_tmp218;struct Cyc_Absyn_Tqual _tmp217;void*_tmp216;struct Cyc_List_List*_tmp215;int _tmp214;struct Cyc_Absyn_VarargInfo*_tmp213;struct Cyc_List_List*_tmp212;struct Cyc_List_List*_tmp211;struct Cyc_Absyn_Exp*_tmp210;struct Cyc_List_List*_tmp20F;struct Cyc_Absyn_Exp*_tmp20E;struct Cyc_List_List*_tmp20D;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->tag == 9){_LL15F: _tmp219=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).tvars;_tmp218=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).effect;_tmp217=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).ret_tqual;_tmp216=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).ret_typ;_tmp215=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).args;_tmp214=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).c_varargs;_tmp213=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).cyc_varargs;_tmp212=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).rgn_po;_tmp211=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).attributes;_tmp210=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).requires_clause;_tmp20F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).requires_relns;_tmp20E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).ensures_clause;_tmp20D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1FF)->f1).ensures_relns;_LL160: {
# 1187
struct _RegionHandle _tmp200=_new_region("temp");struct _RegionHandle*temp=& _tmp200;_push_region(temp);
{struct Cyc_List_List*instantiation=0;
# 1190
for(0;ts != 0  && _tmp219 != 0;(ts=ts->tl,_tmp219=_tmp219->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp219->hd,& Cyc_Tcutil_bk);
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),k,1,1,(void*)ts->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
instantiation=({struct Cyc_List_List*_tmp201=_region_malloc(temp,sizeof(*_tmp201));_tmp201->hd=({struct _tuple13*_tmp202=_region_malloc(temp,sizeof(*_tmp202));_tmp202->f1=(struct Cyc_Absyn_Tvar*)_tmp219->hd;_tmp202->f2=(void*)ts->hd;_tmp202;});_tmp201->tl=instantiation;_tmp201;});}
# 1197
if(ts != 0){
void*_tmp205=({void*_tmp203=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp204="too many type variables in instantiation";_tag_dyneither(_tmp204,sizeof(char),41);}),_tag_dyneither(_tmp203,sizeof(void*),0));});_npop_handler(0);return _tmp205;}
# 1203
if(_tmp219 == 0){
_tmp212=Cyc_Tcutil_rsubst_rgnpo(temp,instantiation,_tmp212);
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp212);
_tmp212=0;}{
# 1208
void*new_fn_typ=
Cyc_Tcutil_rsubstitute(temp,instantiation,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20A=_cycalloc(sizeof(*_tmp20A));_tmp20A[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp20B;_tmp20B.tag=9;_tmp20B.f1=({struct Cyc_Absyn_FnInfo _tmp20C;_tmp20C.tvars=_tmp219;_tmp20C.effect=_tmp218;_tmp20C.ret_tqual=_tmp217;_tmp20C.ret_typ=_tmp216;_tmp20C.args=_tmp215;_tmp20C.c_varargs=_tmp214;_tmp20C.cyc_varargs=_tmp213;_tmp20C.rgn_po=_tmp212;_tmp20C.attributes=_tmp211;_tmp20C.requires_clause=_tmp210;_tmp20C.requires_relns=_tmp20F;_tmp20C.ensures_clause=_tmp20E;_tmp20C.ensures_relns=_tmp20D;_tmp20C;});_tmp20B;});_tmp20A;}));
# 1213
void*_tmp209=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp207;_tmp207.tag=5;_tmp207.f1=({struct Cyc_Absyn_PtrInfo _tmp208;_tmp208.elt_typ=new_fn_typ;_tmp208.elt_tq=_tmp21E;_tmp208.ptr_atts=({(_tmp208.ptr_atts).rgn=_tmp21D;(_tmp208.ptr_atts).nullable=_tmp21C;(_tmp208.ptr_atts).bounds=_tmp21B;(_tmp208.ptr_atts).zero_term=_tmp21A;(_tmp208.ptr_atts).ptrloc=0;_tmp208.ptr_atts;});_tmp208;});_tmp207;});_tmp206;});_npop_handler(0);return _tmp209;};}
# 1188
;_pop_region(temp);}}else{_LL161: _LL162:
# 1215
 goto _LL15E;}_LL15E:;}
# 1217
goto _LL159;}else{_LL15C: _LL15D:
 goto _LL159;}_LL159:;}
# 1220
return({struct Cyc_String_pa_PrintArg_struct _tmp222;_tmp222.tag=0;_tmp222.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));({void*_tmp220[1]={& _tmp222};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp221="expecting polymorphic type but found %s";_tag_dyneither(_tmp221,sizeof(char),40);}),_tag_dyneither(_tmp220,sizeof(void*),1));});});};}
# 1225
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1227
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk,1,0,t);
Cyc_Tcutil_check_no_qual(loc,t);
# 1231
Cyc_Tcexp_tcExp(te,& t,e);{
void*t2=(void*)_check_null(e->topt);
if(Cyc_Tcutil_silent_castable(te,loc,t2,t))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1236
enum Cyc_Absyn_Coercion crc=Cyc_Tcutil_castable(te,loc,t2,t);
if(crc != Cyc_Absyn_Unknown_coercion)
*((enum Cyc_Absyn_Coercion*)_check_null(c))=crc;else{
if(Cyc_Tcutil_zero_to_null(te,t,e))
*((enum Cyc_Absyn_Coercion*)_check_null(c))=Cyc_Absyn_No_coercion;else{
# 1243
Cyc_Tcutil_unify(t2,t);{
void*_tmp223=({struct Cyc_String_pa_PrintArg_struct _tmp227;_tmp227.tag=0;_tmp227.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp226;_tmp226.tag=0;_tmp226.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));({void*_tmp224[2]={& _tmp226,& _tmp227};Cyc_Tcexp_expr_err(te,loc,& t,({const char*_tmp225="cannot cast %s to %s";_tag_dyneither(_tmp225,sizeof(char),21);}),_tag_dyneither(_tmp224,sizeof(void*),2));});});});
Cyc_Tcutil_explain_failure();
return _tmp223;};}}}
# 1253
{struct _tuple0 _tmp228=({struct _tuple0 _tmp233;_tmp233.f1=e->r;_tmp233.f2=Cyc_Tcutil_compress(t);_tmp233;});struct _tuple0 _tmp229=_tmp228;int _tmp232;union Cyc_Absyn_Constraint*_tmp231;union Cyc_Absyn_Constraint*_tmp230;union Cyc_Absyn_Constraint*_tmp22F;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp229.f1)->tag == 33){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229.f2)->tag == 5){_LL164: _tmp232=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp229.f1)->f1).fat_result;_tmp231=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229.f2)->f1).ptr_atts).nullable;_tmp230=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229.f2)->f1).ptr_atts).bounds;_tmp22F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp229.f2)->f1).ptr_atts).zero_term;_LL165:
# 1257
 if((_tmp232  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp22F)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp231)){
void*_tmp22A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp230);void*_tmp22B=_tmp22A;struct Cyc_Absyn_Exp*_tmp22E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22B)->tag == 1){_LL169: _tmp22E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp22B)->f1;_LL16A:
# 1260
 if((Cyc_Evexp_eval_const_uint_exp(_tmp22E)).f1 == 1)
({void*_tmp22C=0;Cyc_Tcutil_warn(loc,({const char*_tmp22D="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp22D,sizeof(char),60);}),_tag_dyneither(_tmp22C,sizeof(void*),0));});
goto _LL168;}else{_LL16B: _LL16C:
 goto _LL168;}_LL168:;}
# 1266
goto _LL163;}else{goto _LL166;}}else{_LL166: _LL167:
 goto _LL163;}_LL163:;}
# 1269
return t;};}
# 1273
static void*Cyc_Tcexp_tcAddress(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e0,void**topt,struct Cyc_Absyn_Exp*e){
# 1275
void**_tmp234=0;
struct Cyc_Absyn_Tqual _tmp235=Cyc_Absyn_empty_tqual(0);
int _tmp236=0;
if(topt != 0){
void*_tmp237=Cyc_Tcutil_compress(*topt);void*_tmp238=_tmp237;void**_tmp23B;struct Cyc_Absyn_Tqual _tmp23A;union Cyc_Absyn_Constraint*_tmp239;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp238)->tag == 5){_LL16E: _tmp23B=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp238)->f1).elt_typ;_tmp23A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp238)->f1).elt_tq;_tmp239=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp238)->f1).ptr_atts).nullable;_LL16F:
# 1281
 _tmp234=_tmp23B;
_tmp235=_tmp23A;
_tmp236=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp239);
goto _LL16D;}else{_LL170: _LL171:
 goto _LL16D;}_LL16D:;}{
# 1295
struct _RegionHandle _tmp23C=_new_region("r");struct _RegionHandle*r=& _tmp23C;_push_region(r);
Cyc_Tcexp_tcExpNoInst(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_enter_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),_tmp234,e);
# 1299
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(e->topt)))
({void*_tmp23D=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp23E="Cannot take the address of an alias-free path";_tag_dyneither(_tmp23E,sizeof(char),46);}),_tag_dyneither(_tmp23D,sizeof(void*),0));});
# 1306
{void*_tmp23F=e->r;void*_tmp240=_tmp23F;struct Cyc_Absyn_Exp*_tmp245;struct Cyc_Absyn_Exp*_tmp244;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp240)->tag == 22){_LL173: _tmp245=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp240)->f1;_tmp244=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp240)->f2;_LL174:
# 1308
{void*_tmp241=Cyc_Tcutil_compress((void*)_check_null(_tmp245->topt));void*_tmp242=_tmp241;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp242)->tag == 10){_LL178: _LL179:
 goto _LL177;}else{_LL17A: _LL17B:
# 1313
 e0->r=(Cyc_Absyn_add_exp(_tmp245,_tmp244,0))->r;{
void*_tmp243=Cyc_Tcexp_tcPlus(te,_tmp245,_tmp244);_npop_handler(0);return _tmp243;};}_LL177:;}
# 1316
goto _LL172;}else{_LL175: _LL176:
 goto _LL172;}_LL172:;}
# 1321
{void*_tmp246=e->r;void*_tmp247=_tmp246;switch(*((int*)_tmp247)){case 20: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp247)->f3 == 1){_LL17D: _LL17E:
 goto _LL180;}else{goto _LL181;}case 21: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp247)->f3 == 1){_LL17F: _LL180:
# 1324
({void*_tmp248=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp249="cannot take the address of a @tagged union member";_tag_dyneither(_tmp249,sizeof(char),50);}),_tag_dyneither(_tmp248,sizeof(void*),0));});
goto _LL17C;}else{goto _LL181;}default: _LL181: _LL182:
 goto _LL17C;}_LL17C:;}{
# 1330
struct _tuple14 _tmp24A=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp24B=_tmp24A;int _tmp250;void*_tmp24F;_LL184: _tmp250=_tmp24B.f1;_tmp24F=_tmp24B.f2;_LL185:;
# 1332
if(Cyc_Tcutil_is_noalias_region(_tmp24F,0))
({void*_tmp24C=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp24D="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp24D,sizeof(char),60);}),_tag_dyneither(_tmp24C,sizeof(void*),0));});{
# 1335
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp250){
tq.print_const=1;
tq.real_const=1;}{
# 1341
void*t=_tmp236?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp24F,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp24F,tq,Cyc_Absyn_false_conref);
void*_tmp24E=t;_npop_handler(0);return _tmp24E;};};};
# 1295
;_pop_region(r);};}
# 1348
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1352
return Cyc_Absyn_uint_typ;
# 1354
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,t);
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp253;_tmp253.tag=0;_tmp253.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmp251[1]={& _tmp253};Cyc_Tcutil_terr(loc,({const char*_tmp252="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp252,sizeof(char),55);}),_tag_dyneither(_tmp251,sizeof(void*),1));});});
if(topt != 0){
void*_tmp254=Cyc_Tcutil_compress(*topt);void*_tmp255=_tmp254;void*_tmp25B;void*_tmp25A;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp255)->tag == 19){_LL187: _tmp25B=_tmp255;_tmp25A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp255)->f1;_LL188: {
# 1363
struct Cyc_Absyn_Exp*_tmp256=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp257=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp259;_tmp259.tag=18;_tmp259.f1=_tmp256;_tmp259;});_tmp258;});
if(Cyc_Tcutil_unify(_tmp25A,(void*)_tmp257))return _tmp25B;
goto _LL186;}}else{_LL189: _LL18A:
 goto _LL186;}_LL186:;}
# 1369
return Cyc_Absyn_uint_typ;}
# 1372
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0)return sf->type;else{
return 0;}}
# 1379
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1381
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,t_orig);
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp25C=fs;
void*_tmp25D=t_orig;
for(0;_tmp25C != 0;_tmp25C=_tmp25C->tl){
void*_tmp25E=(void*)_tmp25C->hd;
void*_tmp25F=_tmp25E;unsigned int _tmp299;struct _dyneither_ptr*_tmp298;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->tag == 0){_LL18C: _tmp298=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp25F)->f1;_LL18D: {
# 1389
int bad_type=1;
{void*_tmp260=Cyc_Tcutil_compress(_tmp25D);void*_tmp261=_tmp260;struct Cyc_List_List*_tmp269;struct Cyc_Absyn_Aggrdecl**_tmp268;switch(*((int*)_tmp261)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp261)->f1).aggr_info).KnownAggr).tag == 2){_LL191: _tmp268=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp261)->f1).aggr_info).KnownAggr).val;_LL192:
# 1392
 if((*_tmp268)->impl == 0)goto _LL190;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp298,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp268)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp264;_tmp264.tag=0;_tmp264.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp298);({void*_tmp262[1]={& _tmp264};Cyc_Tcutil_terr(loc,({const char*_tmp263="no field of struct/union has name %s";_tag_dyneither(_tmp263,sizeof(char),37);}),_tag_dyneither(_tmp262,sizeof(void*),1));});});else{
# 1397
_tmp25D=t2;}
bad_type=0;
goto _LL190;};}else{goto _LL195;}case 12: _LL193: _tmp269=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp261)->f2;_LL194: {
# 1401
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp298,_tmp269);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp267;_tmp267.tag=0;_tmp267.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp298);({void*_tmp265[1]={& _tmp267};Cyc_Tcutil_terr(loc,({const char*_tmp266="no field of struct/union has name %s";_tag_dyneither(_tmp266,sizeof(char),37);}),_tag_dyneither(_tmp265,sizeof(void*),1));});});else{
# 1405
_tmp25D=t2;}
bad_type=0;
goto _LL190;}default: _LL195: _LL196:
 goto _LL190;}_LL190:;}
# 1410
if(bad_type){
if(_tmp25C == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp26C;_tmp26C.tag=0;_tmp26C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp25D));({void*_tmp26A[1]={& _tmp26C};Cyc_Tcutil_terr(loc,({const char*_tmp26B="%s is not a known struct/union type";_tag_dyneither(_tmp26B,sizeof(char),36);}),_tag_dyneither(_tmp26A,sizeof(void*),1));});});else{
# 1414
struct _dyneither_ptr _tmp26D=({struct Cyc_String_pa_PrintArg_struct _tmp280;_tmp280.tag=0;_tmp280.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));({void*_tmp27E[1]={& _tmp280};Cyc_aprintf(({const char*_tmp27F="(%s)";_tag_dyneither(_tmp27F,sizeof(char),5);}),_tag_dyneither(_tmp27E,sizeof(void*),1));});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp25C;x=x->tl){
void*_tmp26E=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp26F=_tmp26E;unsigned int _tmp279;struct _dyneither_ptr*_tmp278;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp26F)->tag == 0){_LL198: _tmp278=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp26F)->f1;_LL199:
# 1419
 _tmp26D=({struct Cyc_String_pa_PrintArg_struct _tmp273;_tmp273.tag=0;_tmp273.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp278);({struct Cyc_String_pa_PrintArg_struct _tmp272;_tmp272.tag=0;_tmp272.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26D);({void*_tmp270[2]={& _tmp272,& _tmp273};Cyc_aprintf(({const char*_tmp271="%s.%s";_tag_dyneither(_tmp271,sizeof(char),6);}),_tag_dyneither(_tmp270,sizeof(void*),2));});});});goto _LL197;}else{_LL19A: _tmp279=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp26F)->f1;_LL19B:
# 1421
 _tmp26D=({struct Cyc_Int_pa_PrintArg_struct _tmp277;_tmp277.tag=1;_tmp277.f1=(unsigned long)((int)_tmp279);({struct Cyc_String_pa_PrintArg_struct _tmp276;_tmp276.tag=0;_tmp276.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26D);({void*_tmp274[2]={& _tmp276,& _tmp277};Cyc_aprintf(({const char*_tmp275="%s.%d";_tag_dyneither(_tmp275,sizeof(char),6);}),_tag_dyneither(_tmp274,sizeof(void*),2));});});});goto _LL197;}_LL197:;}
# 1423
({struct Cyc_String_pa_PrintArg_struct _tmp27D;_tmp27D.tag=0;_tmp27D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp25D));({struct Cyc_String_pa_PrintArg_struct _tmp27C;_tmp27C.tag=0;_tmp27C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp26D);({void*_tmp27A[2]={& _tmp27C,& _tmp27D};Cyc_Tcutil_terr(loc,({const char*_tmp27B="%s == %s is not a struct/union type";_tag_dyneither(_tmp27B,sizeof(char),36);}),_tag_dyneither(_tmp27A,sizeof(void*),2));});});});}}
# 1426
goto _LL18B;}}else{_LL18E: _tmp299=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp25F)->f1;_LL18F: {
# 1428
int bad_type=1;
{void*_tmp281=Cyc_Tcutil_compress(_tmp25D);void*_tmp282=_tmp281;struct Cyc_Absyn_Datatypefield*_tmp294;struct Cyc_List_List*_tmp293;struct Cyc_List_List*_tmp292;struct Cyc_Absyn_Aggrdecl**_tmp291;switch(*((int*)_tmp282)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp282)->f1).aggr_info).KnownAggr).tag == 2){_LL19D: _tmp291=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp282)->f1).aggr_info).KnownAggr).val;_LL19E:
# 1431
 if((*_tmp291)->impl == 0)
goto _LL19C;
_tmp292=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp291)->impl))->fields;goto _LL1A0;}else{goto _LL1A5;}case 12: _LL19F: _tmp292=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp282)->f2;_LL1A0:
# 1435
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp292)<= _tmp299)
({struct Cyc_Int_pa_PrintArg_struct _tmp286;_tmp286.tag=1;_tmp286.f1=(unsigned long)((int)_tmp299);({struct Cyc_Int_pa_PrintArg_struct _tmp285;_tmp285.tag=1;_tmp285.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp292);({void*_tmp283[2]={& _tmp285,& _tmp286};Cyc_Tcutil_terr(loc,({const char*_tmp284="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp284,sizeof(char),46);}),_tag_dyneither(_tmp283,sizeof(void*),2));});});});else{
# 1439
_tmp25D=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp292,(int)_tmp299))->type;}
bad_type=0;
goto _LL19C;case 10: _LL1A1: _tmp293=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp282)->f1;_LL1A2:
# 1443
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp293)<= _tmp299)
({struct Cyc_Int_pa_PrintArg_struct _tmp28A;_tmp28A.tag=1;_tmp28A.f1=(unsigned long)((int)_tmp299);({struct Cyc_Int_pa_PrintArg_struct _tmp289;_tmp289.tag=1;_tmp289.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp293);({void*_tmp287[2]={& _tmp289,& _tmp28A};Cyc_Tcutil_terr(loc,({const char*_tmp288="tuple has too few components: %d <= %d";_tag_dyneither(_tmp288,sizeof(char),39);}),_tag_dyneither(_tmp287,sizeof(void*),2));});});});else{
# 1447
_tmp25D=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp293,(int)_tmp299)).f2;}
bad_type=0;
goto _LL19C;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp282)->f1).field_info).KnownDatatypefield).tag == 2){_LL1A3: _tmp294=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp282)->f1).field_info).KnownDatatypefield).val).f2;_LL1A4:
# 1451
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp294->typs)< _tmp299)
({struct Cyc_Int_pa_PrintArg_struct _tmp28E;_tmp28E.tag=1;_tmp28E.f1=(unsigned long)((int)_tmp299);({struct Cyc_Int_pa_PrintArg_struct _tmp28D;_tmp28D.tag=1;_tmp28D.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp294->typs);({void*_tmp28B[2]={& _tmp28D,& _tmp28E};Cyc_Tcutil_terr(loc,({const char*_tmp28C="datatype field has too few components: %d < %d";_tag_dyneither(_tmp28C,sizeof(char),47);}),_tag_dyneither(_tmp28B,sizeof(void*),2));});});});else{
# 1455
if(_tmp299 != 0)
_tmp25D=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp294->typs,(int)(_tmp299 - 1))).f2;else{
if(_tmp25C->tl != 0)
({void*_tmp28F=0;Cyc_Tcutil_terr(loc,({const char*_tmp290="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp290,sizeof(char),68);}),_tag_dyneither(_tmp28F,sizeof(void*),0));});}}
# 1460
bad_type=0;
goto _LL19C;}else{goto _LL1A5;}default: _LL1A5: _LL1A6:
 goto _LL19C;}_LL19C:;}
# 1464
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp297;_tmp297.tag=0;_tmp297.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp25D));({void*_tmp295[1]={& _tmp297};Cyc_Tcutil_terr(loc,({const char*_tmp296="%s is not a known type";_tag_dyneither(_tmp296,sizeof(char),23);}),_tag_dyneither(_tmp295,sizeof(void*),1));});});
goto _LL18B;}}_LL18B:;}
# 1469
return Cyc_Absyn_uint_typ;};}
# 1473
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct _RegionHandle _tmp29A=_new_region("r");struct _RegionHandle*r=& _tmp29A;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp29B=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(_tmp29B,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp29C=t;void*_tmp2B4;void*_tmp2B3;union Cyc_Absyn_Constraint*_tmp2B2;union Cyc_Absyn_Constraint*_tmp2B1;switch(*((int*)_tmp29C)){case 1: _LL1A8: _LL1A9: {
# 1480
struct Cyc_List_List*_tmp29D=Cyc_Tcenv_lookup_type_vars(_tmp29B);
void*_tmp29E=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->v=_tmp29D;_tmp2A9;}));
void*_tmp29F=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->v=_tmp29D;_tmp2A8;}));
union Cyc_Absyn_Constraint*_tmp2A0=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp2A1=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp2A2=({struct Cyc_Absyn_PtrAtts _tmp2A7;_tmp2A7.rgn=_tmp29F;_tmp2A7.nullable=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp2A7.bounds=_tmp2A0;_tmp2A7.zero_term=_tmp2A1;_tmp2A7.ptrloc=0;_tmp2A7;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A3=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2A4=_cycalloc(sizeof(*_tmp2A4));_tmp2A4[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp2A5;_tmp2A5.tag=5;_tmp2A5.f1=({struct Cyc_Absyn_PtrInfo _tmp2A6;_tmp2A6.elt_typ=_tmp29E;_tmp2A6.elt_tq=Cyc_Absyn_empty_tqual(0);_tmp2A6.ptr_atts=_tmp2A2;_tmp2A6;});_tmp2A5;});_tmp2A4;});
Cyc_Tcutil_unify(t,(void*)_tmp2A3);
_tmp2B4=_tmp29E;_tmp2B3=_tmp29F;_tmp2B2=_tmp2A0;_tmp2B1=_tmp2A1;goto _LL1AB;}case 5: _LL1AA: _tmp2B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->f1).elt_typ;_tmp2B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->f1).ptr_atts).rgn;_tmp2B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->f1).ptr_atts).bounds;_tmp2B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29C)->f1).ptr_atts).zero_term;_LL1AB:
# 1490
 Cyc_Tcenv_check_rgn_accessible(_tmp29B,loc,_tmp2B3);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp2B2);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp2B4),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp29B))
({void*_tmp2AA=0;Cyc_Tcutil_terr(loc,({const char*_tmp2AB="can't dereference abstract pointer type";_tag_dyneither(_tmp2AB,sizeof(char),40);}),_tag_dyneither(_tmp2AA,sizeof(void*),0));});{
void*_tmp2AC=_tmp2B4;_npop_handler(0);return _tmp2AC;};default: _LL1AC: _LL1AD: {
# 1496
void*_tmp2B0=({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0;_tmp2AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp2AD[1]={& _tmp2AF};Cyc_Tcexp_expr_err(_tmp29B,loc,topt,({const char*_tmp2AE="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp2AE,sizeof(char),39);}),_tag_dyneither(_tmp2AD,sizeof(void*),1));});});_npop_handler(0);return _tmp2B0;}}_LL1A7:;};}
# 1475
;_pop_region(r);}
# 1502
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1506
struct _RegionHandle _tmp2B5=_new_region("r");struct _RegionHandle*r=& _tmp2B5;_push_region(r);
Cyc_Tcexp_tcExpNoPromote(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e);
# 1510
*is_read=!Cyc_Tcenv_in_notreadctxt(te);{
void*_tmp2B6=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2B7=_tmp2B6;enum Cyc_Absyn_AggrKind _tmp2DA;struct Cyc_List_List*_tmp2D9;struct Cyc_Absyn_Aggrdecl*_tmp2D8;struct Cyc_List_List*_tmp2D7;switch(*((int*)_tmp2B7)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B7)->f1).aggr_info).KnownAggr).tag == 2){_LL1AF: _tmp2D8=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B7)->f1).aggr_info).KnownAggr).val;_tmp2D7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2B7)->f1).targs;_LL1B0: {
# 1513
struct Cyc_Absyn_Aggrfield*_tmp2B8=Cyc_Absyn_lookup_decl_field(_tmp2D8,f);
if(_tmp2B8 == 0){
void*_tmp2BD=({struct Cyc_String_pa_PrintArg_struct _tmp2BC;_tmp2BC.tag=0;_tmp2BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2BB;_tmp2BB.tag=0;_tmp2BB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2D8->name));({void*_tmp2B9[2]={& _tmp2BB,& _tmp2BC};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2BA="%s has no %s member";_tag_dyneither(_tmp2BA,sizeof(char),20);}),_tag_dyneither(_tmp2B9,sizeof(void*),2));});});});_npop_handler(0);return _tmp2BD;}
# 1518
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2D8->impl))->tagged)*is_tagged=1;{
void*t2=_tmp2B8->type;
if(_tmp2D7 != 0){
struct _RegionHandle _tmp2BE=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2BE;_push_region(rgn);
{struct Cyc_List_List*_tmp2BF=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp2D8->tvs,_tmp2D7);
t2=Cyc_Tcutil_rsubstitute(rgn,_tmp2BF,_tmp2B8->type);}
# 1522
;_pop_region(rgn);}
# 1528
if((((_tmp2D8->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2D8->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2B8->requires_clause == 0){
void*_tmp2C3=({struct Cyc_String_pa_PrintArg_struct _tmp2C2;_tmp2C2.tag=0;_tmp2C2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2C0[1]={& _tmp2C2};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2C1="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2C1,sizeof(char),56);}),_tag_dyneither(_tmp2C0,sizeof(void*),1));});});_npop_handler(0);return _tmp2C3;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2D8->impl))->exist_vars != 0){
# 1533
if(!Cyc_Tcutil_unify(t2,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
void*_tmp2C7=({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0;_tmp2C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2C4[1]={& _tmp2C6};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2C5="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp2C5,sizeof(char),81);}),_tag_dyneither(_tmp2C4,sizeof(void*),1));});});_npop_handler(0);return _tmp2C7;}}{
# 1536
void*_tmp2C8=t2;_npop_handler(0);return _tmp2C8;};};}}else{goto _LL1B3;}case 12: _LL1B1: _tmp2DA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B7)->f1;_tmp2D9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2B7)->f2;_LL1B2: {
# 1538
struct Cyc_Absyn_Aggrfield*_tmp2C9=Cyc_Absyn_lookup_field(_tmp2D9,f);
if(_tmp2C9 == 0){
void*_tmp2CD=({struct Cyc_String_pa_PrintArg_struct _tmp2CC;_tmp2CC.tag=0;_tmp2CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CA[1]={& _tmp2CC};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2CB="type has no %s member";_tag_dyneither(_tmp2CB,sizeof(char),22);}),_tag_dyneither(_tmp2CA,sizeof(void*),1));});});_npop_handler(0);return _tmp2CD;}
# 1543
if(((_tmp2DA == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2C9->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2C9->requires_clause == 0){
# 1545
void*_tmp2D1=({struct Cyc_String_pa_PrintArg_struct _tmp2D0;_tmp2D0.tag=0;_tmp2D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CE[1]={& _tmp2D0};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2CF="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2CF,sizeof(char),56);}),_tag_dyneither(_tmp2CE,sizeof(void*),1));});});_npop_handler(0);return _tmp2D1;}{
void*_tmp2D2=_tmp2C9->type;_npop_handler(0);return _tmp2D2;};}default: _LL1B3: _LL1B4: {
# 1548
void*_tmp2D6=({struct Cyc_String_pa_PrintArg_struct _tmp2D5;_tmp2D5.tag=0;_tmp2D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({void*_tmp2D3[1]={& _tmp2D5};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2D4="expecting struct or union, found %s";_tag_dyneither(_tmp2D4,sizeof(char),36);}),_tag_dyneither(_tmp2D3,sizeof(void*),1));});});_npop_handler(0);return _tmp2D6;}}_LL1AE:;};
# 1506
;_pop_region(r);}
# 1554
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1557
struct _RegionHandle _tmp2DB=_new_region("r");struct _RegionHandle*r=& _tmp2DB;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te))),0,e);
# 1562
*is_read=!Cyc_Tcenv_in_notreadctxt(te);
{void*_tmp2DC=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2DD=_tmp2DC;void*_tmp309;void*_tmp308;union Cyc_Absyn_Constraint*_tmp307;union Cyc_Absyn_Constraint*_tmp306;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DD)->tag == 5){_LL1B6: _tmp309=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DD)->f1).elt_typ;_tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DD)->f1).ptr_atts).rgn;_tmp307=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DD)->f1).ptr_atts).bounds;_tmp306=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DD)->f1).ptr_atts).zero_term;_LL1B7:
# 1565
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp307);
{void*_tmp2DE=Cyc_Tcutil_compress(_tmp309);void*_tmp2DF=_tmp2DE;enum Cyc_Absyn_AggrKind _tmp305;struct Cyc_List_List*_tmp304;struct Cyc_Absyn_Aggrdecl*_tmp303;struct Cyc_List_List*_tmp302;switch(*((int*)_tmp2DF)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2DF)->f1).aggr_info).KnownAggr).tag == 2){_LL1BB: _tmp303=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2DF)->f1).aggr_info).KnownAggr).val;_tmp302=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2DF)->f1).targs;_LL1BC: {
# 1568
struct Cyc_Absyn_Aggrfield*_tmp2E0=Cyc_Absyn_lookup_decl_field(_tmp303,f);
if(_tmp2E0 == 0){
void*_tmp2E5=({struct Cyc_String_pa_PrintArg_struct _tmp2E4;_tmp2E4.tag=0;_tmp2E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2E3;_tmp2E3.tag=0;_tmp2E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp303->name));({void*_tmp2E1[2]={& _tmp2E3,& _tmp2E4};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2E2="type %s has no %s member";_tag_dyneither(_tmp2E2,sizeof(char),25);}),_tag_dyneither(_tmp2E1,sizeof(void*),2));});});});_npop_handler(0);return _tmp2E5;}
# 1573
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp303->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2E0->type;
if(_tmp302 != 0){
struct _RegionHandle _tmp2E6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2E6;_push_region(rgn);
{struct Cyc_List_List*_tmp2E7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp303->tvs,_tmp302);
t3=Cyc_Tcutil_rsubstitute(rgn,_tmp2E7,_tmp2E0->type);}
# 1577
;_pop_region(rgn);}{
# 1581
struct Cyc_Absyn_Kind*_tmp2E8=Cyc_Tcutil_typ_kind(t3);
# 1584
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2E8) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2E9=Cyc_Tcutil_compress(t3);void*_tmp2EA=_tmp2E9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2EA)->tag == 8){_LL1C2: _LL1C3:
 goto _LL1C1;}else{_LL1C4: _LL1C5: {
# 1588
void*_tmp2EE=({struct Cyc_String_pa_PrintArg_struct _tmp2ED;_tmp2ED.tag=0;_tmp2ED.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2EB[1]={& _tmp2ED};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2EC="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2EC,sizeof(char),48);}),_tag_dyneither(_tmp2EB,sizeof(void*),1));});});_npop_handler(0);return _tmp2EE;}}_LL1C1:;}
# 1593
if((((_tmp303->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp303->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2E0->requires_clause == 0){
# 1596
void*_tmp2F2=({struct Cyc_String_pa_PrintArg_struct _tmp2F1;_tmp2F1.tag=0;_tmp2F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2EF[1]={& _tmp2F1};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2F0="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2F0,sizeof(char),56);}),_tag_dyneither(_tmp2EF,sizeof(void*),1));});});_npop_handler(0);return _tmp2F2;}
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp303->impl))->exist_vars != 0){
if(!Cyc_Tcutil_unify(t3,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
void*_tmp2F6=({struct Cyc_String_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=0;_tmp2F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2F3[1]={& _tmp2F5};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2F4="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2F4,sizeof(char),72);}),_tag_dyneither(_tmp2F3,sizeof(void*),1));});});_npop_handler(0);return _tmp2F6;}}{
# 1603
void*_tmp2F7=t3;_npop_handler(0);return _tmp2F7;};};};}}else{goto _LL1BF;}case 12: _LL1BD: _tmp305=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2DF)->f1;_tmp304=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2DF)->f2;_LL1BE: {
# 1605
struct Cyc_Absyn_Aggrfield*_tmp2F8=Cyc_Absyn_lookup_field(_tmp304,f);
if(_tmp2F8 == 0){
void*_tmp2FC=({struct Cyc_String_pa_PrintArg_struct _tmp2FB;_tmp2FB.tag=0;_tmp2FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2F9[1]={& _tmp2FB};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2FA="type has no %s field";_tag_dyneither(_tmp2FA,sizeof(char),21);}),_tag_dyneither(_tmp2F9,sizeof(void*),1));});});_npop_handler(0);return _tmp2FC;}
# 1610
if((_tmp305 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2F8->type)) && !Cyc_Tcenv_in_notreadctxt(te)){
void*_tmp300=({struct Cyc_String_pa_PrintArg_struct _tmp2FF;_tmp2FF.tag=0;_tmp2FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2FD[1]={& _tmp2FF};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp2FE="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2FE,sizeof(char),56);}),_tag_dyneither(_tmp2FD,sizeof(void*),1));});});_npop_handler(0);return _tmp300;}{
void*_tmp301=_tmp2F8->type;_npop_handler(0);return _tmp301;};}default: _LL1BF: _LL1C0:
 goto _LL1BA;}_LL1BA:;}
# 1615
goto _LL1B5;}else{_LL1B8: _LL1B9:
 goto _LL1B5;}_LL1B5:;}{
# 1618
void*_tmp30D=({struct Cyc_String_pa_PrintArg_struct _tmp30C;_tmp30C.tag=0;_tmp30C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));({void*_tmp30A[1]={& _tmp30C};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp30B="expecting struct or union pointer, found %s";_tag_dyneither(_tmp30B,sizeof(char),44);}),_tag_dyneither(_tmp30A,sizeof(void*),1));});});_npop_handler(0);return _tmp30D;};
# 1557
;_pop_region(r);}
# 1623
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1625
struct _tuple15 _tmp30E=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp30F=_tmp30E;unsigned int _tmp31D;int _tmp31C;_LL1C7: _tmp31D=_tmp30F.f1;_tmp31C=_tmp30F.f2;_LL1C8:;
if(!_tmp31C)
return({void*_tmp310=0;Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp311="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp311,sizeof(char),47);}),_tag_dyneither(_tmp310,sizeof(void*),0));});{
# 1629
struct _handler_cons _tmp312;_push_handler(& _tmp312);{int _tmp314=0;if(setjmp(_tmp312.handler))_tmp314=1;if(!_tmp314){
{void*_tmp315=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp31D)).f2;_npop_handler(0);return _tmp315;};_pop_handler();}else{void*_tmp313=(void*)_exn_thrown;void*_tmp316=_tmp313;void*_tmp31B;if(((struct Cyc_List_Nth_exn_struct*)_tmp316)->tag == Cyc_List_Nth){_LL1CA: _LL1CB:
# 1632
 return({struct Cyc_Int_pa_PrintArg_struct _tmp31A;_tmp31A.tag=1;_tmp31A.f1=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);({struct Cyc_Int_pa_PrintArg_struct _tmp319;_tmp319.tag=1;_tmp319.f1=(unsigned long)((int)_tmp31D);({void*_tmp317[2]={& _tmp319,& _tmp31A};Cyc_Tcexp_expr_err(te,loc,0,({const char*_tmp318="index is %d but tuple has only %d fields";_tag_dyneither(_tmp318,sizeof(char),41);}),_tag_dyneither(_tmp317,sizeof(void*),2));});});});}else{_LL1CC: _tmp31B=_tmp316;_LL1CD:(int)_rethrow(_tmp31B);}_LL1C9:;}};};}
# 1637
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1639
struct _RegionHandle _tmp31E=_new_region("r");struct _RegionHandle*r=& _tmp31E;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp31F=Cyc_Tcenv_clear_lhs(r,Cyc_Tcenv_clear_notreadctxt(r,te_orig));
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp31F),0,e1);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,_tmp31F),0,e2);{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp31F,e2)){
void*_tmp323=({struct Cyc_String_pa_PrintArg_struct _tmp322;_tmp322.tag=0;_tmp322.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({void*_tmp320[1]={& _tmp322};Cyc_Tcexp_expr_err(_tmp31F,e2->loc,topt,({const char*_tmp321="expecting int subscript, found %s";_tag_dyneither(_tmp321,sizeof(char),34);}),_tag_dyneither(_tmp320,sizeof(void*),1));});});_npop_handler(0);return _tmp323;}{
# 1650
void*_tmp324=t1;struct Cyc_List_List*_tmp345;void*_tmp344;struct Cyc_Absyn_Tqual _tmp343;void*_tmp342;union Cyc_Absyn_Constraint*_tmp341;union Cyc_Absyn_Constraint*_tmp340;switch(*((int*)_tmp324)){case 5: _LL1CF: _tmp344=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).elt_typ;_tmp343=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).elt_tq;_tmp342=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).ptr_atts).rgn;_tmp341=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).ptr_atts).bounds;_tmp340=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp324)->f1).ptr_atts).zero_term;_LL1D0:
# 1654
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp340)){
int emit_warning=1;
{void*_tmp325=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp341);void*_tmp326=_tmp325;struct Cyc_Absyn_Exp*_tmp32F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp326)->tag == 1){_LL1D6: _tmp32F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp326)->f1;_LL1D7:
# 1658
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp327=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp328=_tmp327;unsigned int _tmp32E;int _tmp32D;_LL1DB: _tmp32E=_tmp328.f1;_tmp32D=_tmp328.f2;_LL1DC:;
if(_tmp32D){
struct _tuple15 _tmp329=Cyc_Evexp_eval_const_uint_exp(_tmp32F);struct _tuple15 _tmp32A=_tmp329;unsigned int _tmp32C;int _tmp32B;_LL1DE: _tmp32C=_tmp32A.f1;_tmp32B=_tmp32A.f2;_LL1DF:;
if(_tmp32B  && _tmp32C > _tmp32E)emit_warning=0;}}
# 1665
goto _LL1D5;}else{_LL1D8: _LL1D9:
 emit_warning=0;goto _LL1D5;}_LL1D5:;}
# 1668
if(emit_warning)
({void*_tmp330=0;Cyc_Tcutil_warn(e2->loc,({const char*_tmp331="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp331,sizeof(char),63);}),_tag_dyneither(_tmp330,sizeof(void*),0));});}else{
# 1672
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp332=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp333=_tmp332;unsigned int _tmp335;int _tmp334;_LL1E1: _tmp335=_tmp333.f1;_tmp334=_tmp333.f2;_LL1E2:;
if(_tmp334)
Cyc_Tcutil_check_bound(loc,_tmp335,_tmp341);}else{
# 1678
if(Cyc_Tcutil_is_bound_one(_tmp341) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp340))
({void*_tmp336=0;Cyc_Tcutil_warn(e1->loc,({const char*_tmp337="subscript applied to pointer to one object";_tag_dyneither(_tmp337,sizeof(char),43);}),_tag_dyneither(_tmp336,sizeof(void*),0));});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp341);}}
# 1683
Cyc_Tcenv_check_rgn_accessible(_tmp31F,loc,_tmp342);
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp344),& Cyc_Tcutil_tmk) && !Cyc_Tcenv_abstract_val_ok(_tmp31F))
({void*_tmp338=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp339="can't subscript an abstract pointer";_tag_dyneither(_tmp339,sizeof(char),36);}),_tag_dyneither(_tmp338,sizeof(void*),0));});{
void*_tmp33A=_tmp344;_npop_handler(0);return _tmp33A;};case 10: _LL1D1: _tmp345=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp324)->f1;_LL1D2: {
void*_tmp33B=Cyc_Tcexp_ithTupleType(_tmp31F,loc,_tmp345,e2);_npop_handler(0);return _tmp33B;}default: _LL1D3: _LL1D4: {
void*_tmp33F=({struct Cyc_String_pa_PrintArg_struct _tmp33E;_tmp33E.tag=0;_tmp33E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp33C[1]={& _tmp33E};Cyc_Tcexp_expr_err(_tmp31F,loc,topt,({const char*_tmp33D="subscript applied to %s";_tag_dyneither(_tmp33D,sizeof(char),24);}),_tag_dyneither(_tmp33C,sizeof(void*),1));});});_npop_handler(0);return _tmp33F;}}_LL1CE:;};};}
# 1640
;_pop_region(r);}
# 1694
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp346=Cyc_Tcutil_compress(*topt);void*_tmp347=_tmp346;struct Cyc_List_List*_tmp34C;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp347)->tag == 10){_LL1E4: _tmp34C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp347)->f1;_LL1E5:
# 1700
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp34C)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es))
# 1703
goto _LL1E3;
# 1705
for(0;es != 0;(es=es->tl,_tmp34C=_tmp34C->tl)){
struct _RegionHandle _tmp348=_new_region("r");struct _RegionHandle*r=& _tmp348;_push_region(r);
{int bogus=0;
void*_tmp349=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp34C))->hd)).f2;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),& _tmp349,(struct Cyc_Absyn_Exp*)es->hd);
# 1711
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp34C->hd)).f2,& bogus);
fields=({struct Cyc_List_List*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A->hd=({struct _tuple17*_tmp34B=_cycalloc(sizeof(*_tmp34B));_tmp34B->f1=(*((struct _tuple17*)_tmp34C->hd)).f1;_tmp34B->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp34B;});_tmp34A->tl=fields;_tmp34A;});}
# 1707
;_pop_region(r);}
# 1715
done=1;
goto _LL1E3;}else{_LL1E6: _LL1E7:
 goto _LL1E3;}_LL1E3:;}
# 1719
if(!done)
for(0;es != 0;es=es->tl){
struct _RegionHandle _tmp34D=_new_region("r");struct _RegionHandle*r=& _tmp34D;_push_region(r);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(r,te),0,(struct Cyc_Absyn_Exp*)es->hd);
fields=({struct Cyc_List_List*_tmp34E=_cycalloc(sizeof(*_tmp34E));_tmp34E->hd=({struct _tuple17*_tmp34F=_cycalloc(sizeof(*_tmp34F));_tmp34F->f1=Cyc_Absyn_empty_tqual(0);_tmp34F->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp34F;});_tmp34E->tl=fields;_tmp34E;});
# 1722
;_pop_region(r);}
# 1726
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp350=_cycalloc(sizeof(*_tmp350));_tmp350[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp351;_tmp351.tag=10;_tmp351.f1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp351;});_tmp350;});}
# 1730
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1733
return({void*_tmp352=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp353="tcCompoundLit";_tag_dyneither(_tmp353,sizeof(char),14);}),_tag_dyneither(_tmp352,sizeof(void*),0));});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1743 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1746
void*res_t2;
struct _RegionHandle _tmp354=_new_region("r");struct _RegionHandle*r=& _tmp354;_push_region(r);{
int _tmp355=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp356=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp35D=_cycalloc(sizeof(*_tmp35D));_tmp35D[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp35E;_tmp35E.tag=0;_tmp35E.f1=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp355);_tmp35E;});_tmp35D;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp356,loc);
# 1755
if(zero_term){
struct Cyc_Absyn_Exp*_tmp357=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp355 - 1);
if(!Cyc_Tcutil_is_zero(_tmp357))
({void*_tmp358=0;Cyc_Tcutil_terr(_tmp357->loc,({const char*_tmp359="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp359,sizeof(char),45);}),_tag_dyneither(_tmp358,sizeof(void*),0));});}
# 1760
sz_exp->topt=Cyc_Absyn_uint_typ;
res_t2=Cyc_Absyn_array_typ(res,
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0),sz_exp,
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);
# 1765
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1768
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1770
({struct Cyc_String_pa_PrintArg_struct _tmp35C;_tmp35C.tag=0;_tmp35C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res));({void*_tmp35A[1]={& _tmp35C};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc,({const char*_tmp35B="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp35B,sizeof(char),53);}),_tag_dyneither(_tmp35A,sizeof(void*),1));});});}
# 1774
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1779
void*_tmp35F=(void*)ds->hd;void*_tmp360=_tmp35F;struct Cyc_Absyn_Exp*_tmp36D;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp360)->tag == 1){_LL1E9: _LL1EA:
# 1781
({void*_tmp361=0;Cyc_Tcutil_terr(loc,({const char*_tmp362="only array index designators are supported";_tag_dyneither(_tmp362,sizeof(char),43);}),_tag_dyneither(_tmp361,sizeof(void*),0));});
goto _LL1E8;}else{_LL1EB: _tmp36D=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp360)->f1;_LL1EC:
# 1784
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp36D);{
struct _tuple15 _tmp363=Cyc_Evexp_eval_const_uint_exp(_tmp36D);struct _tuple15 _tmp364=_tmp363;unsigned int _tmp36C;int _tmp36B;_LL1EE: _tmp36C=_tmp364.f1;_tmp36B=_tmp364.f2;_LL1EF:;
if(!_tmp36B)
({void*_tmp365=0;Cyc_Tcutil_terr(_tmp36D->loc,({const char*_tmp366="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp366,sizeof(char),47);}),_tag_dyneither(_tmp365,sizeof(void*),0));});else{
if(_tmp36C != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp36A;_tmp36A.tag=1;_tmp36A.f1=(unsigned long)((int)_tmp36C);({struct Cyc_Int_pa_PrintArg_struct _tmp369;_tmp369.tag=1;_tmp369.f1=(unsigned long)offset;({void*_tmp367[2]={& _tmp369,& _tmp36A};Cyc_Tcutil_terr(_tmp36D->loc,({const char*_tmp368="expecting index designator %d but found %d";_tag_dyneither(_tmp368,sizeof(char),43);}),_tag_dyneither(_tmp367,sizeof(void*),2));});});});}
# 1791
goto _LL1E8;};}_LL1E8:;}}}{
# 1795
void*_tmp36E=res_t2;_npop_handler(0);return _tmp36E;};
# 1747
;_pop_region(r);}
# 1799
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1802
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp36F=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp370=_tmp36F;void*_tmp375;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp370)->tag == 19){_LL1F1: _tmp375=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp370)->f1;_LL1F2:
# 1807
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp371=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp375,0),0,Cyc_Absyn_No_coercion,0);
_tmp371->topt=bound->topt;
# 1812
bound=_tmp371;}
# 1814
goto _LL1F0;}else{_LL1F3: _LL1F4:
# 1816
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp374;_tmp374.tag=0;_tmp374.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));({void*_tmp372[1]={& _tmp374};Cyc_Tcutil_terr(bound->loc,({const char*_tmp373="expecting unsigned int, found %s";_tag_dyneither(_tmp373,sizeof(char),33);}),_tag_dyneither(_tmp372,sizeof(void*),1));});});}_LL1F0:;}
# 1821
if(!(vd->tq).real_const)
({void*_tmp376=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp377="comprehension index variable is not declared const!";_tag_dyneither(_tmp377,sizeof(char),52);}),_tag_dyneither(_tmp376,sizeof(void*),0));});{
# 1824
struct _RegionHandle _tmp378=_new_region("r");struct _RegionHandle*r=& _tmp378;_push_region(r);
{struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(r,te);
if(te2->le != 0)
te2=Cyc_Tcenv_new_block(r,loc,te2);{
# 1830
struct Cyc_Tcenv_Tenv*_tmp379=te2;
void**_tmp37A=0;
struct Cyc_Absyn_Tqual*_tmp37B=0;
union Cyc_Absyn_Constraint**_tmp37C=0;
# 1835
if(topt != 0){
void*_tmp37D=Cyc_Tcutil_compress(*topt);void*_tmp37E=_tmp37D;void*_tmp389;struct Cyc_Absyn_Tqual _tmp388;struct Cyc_Absyn_Exp*_tmp387;union Cyc_Absyn_Constraint*_tmp386;struct Cyc_Absyn_PtrInfo _tmp385;switch(*((int*)_tmp37E)){case 5: _LL1F6: _tmp385=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37E)->f1;_LL1F7:
# 1838
 _tmp37A=({void**_tmp37F=_region_malloc(r,sizeof(*_tmp37F));_tmp37F[0]=_tmp385.elt_typ;_tmp37F;});
_tmp37B=({struct Cyc_Absyn_Tqual*_tmp380=_region_malloc(r,sizeof(*_tmp380));_tmp380[0]=_tmp385.elt_tq;_tmp380;});
_tmp37C=({union Cyc_Absyn_Constraint**_tmp381=_region_malloc(r,sizeof(*_tmp381));_tmp381[0]=(_tmp385.ptr_atts).zero_term;_tmp381;});
goto _LL1F5;case 8: _LL1F8: _tmp389=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E)->f1).elt_type;_tmp388=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E)->f1).tq;_tmp387=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E)->f1).num_elts;_tmp386=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp37E)->f1).zero_term;_LL1F9:
# 1843
 _tmp37A=({void**_tmp382=_region_malloc(r,sizeof(*_tmp382));_tmp382[0]=_tmp389;_tmp382;});
_tmp37B=({struct Cyc_Absyn_Tqual*_tmp383=_region_malloc(r,sizeof(*_tmp383));_tmp383[0]=_tmp388;_tmp383;});
_tmp37C=({union Cyc_Absyn_Constraint**_tmp384=_region_malloc(r,sizeof(*_tmp384));_tmp384[0]=_tmp386;_tmp384;});
goto _LL1F5;default: _LL1FA: _LL1FB:
# 1848
 goto _LL1F5;}_LL1F5:;}{
# 1851
void*t=Cyc_Tcexp_tcExp(_tmp379,_tmp37A,body);
# 1853
struct _RegionHandle _tmp38A=_new_region("temp");struct _RegionHandle*temp=& _tmp38A;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,body))
({void*_tmp38B=0;Cyc_Tcutil_terr(body->loc,({const char*_tmp38C="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp38C,sizeof(char),49);}),_tag_dyneither(_tmp38B,sizeof(void*),0));});
# 1857
if(_tmp379->le == 0){
# 1859
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp38D=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp38E="bound is not constant";_tag_dyneither(_tmp38E,sizeof(char),22);}),_tag_dyneither(_tmp38D,sizeof(void*),0));});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp38F=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp390="body is not constant";_tag_dyneither(_tmp390,sizeof(char),21);}),_tag_dyneither(_tmp38F,sizeof(void*),0));});}
# 1864
if(_tmp37C != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp37C)){
# 1867
struct Cyc_Absyn_Exp*_tmp391=Cyc_Absyn_uint_exp(1,0);_tmp391->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp391,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}{
# 1871
struct _RegionHandle _tmp392=_new_region("r");struct _RegionHandle*r=& _tmp392;_push_region(r);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(r,(void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(r,body))
({void*_tmp393=0;Cyc_Tcutil_terr(body->loc,({const char*_tmp394="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp394,sizeof(char),49);}),_tag_dyneither(_tmp393,sizeof(void*),0));});{
# 1877
void*_tmp395=Cyc_Absyn_array_typ(t,_tmp37B == 0?Cyc_Absyn_empty_tqual(0):*_tmp37B,bound,
_tmp37C == 0?Cyc_Absyn_false_conref:*_tmp37C,0);
void*_tmp396=_tmp395;_npop_handler(2);return _tmp396;};
# 1871
;_pop_region(r);};
# 1853
;_pop_region(temp);};};}
# 1825
;_pop_region(r);};}
# 1884
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1887
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp397=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp398=_tmp397;void*_tmp39D;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp398)->tag == 19){_LL1FD: _tmp39D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp398)->f1;_LL1FE:
# 1892
 if(Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr){
struct Cyc_Absyn_Exp*_tmp399=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp39D,0),0,Cyc_Absyn_No_coercion,0);
_tmp399->topt=bound->topt;
# 1897
bound=_tmp399;}
# 1899
goto _LL1FC;}else{_LL1FF: _LL200:
# 1901
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp39C;_tmp39C.tag=0;_tmp39C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));({void*_tmp39A[1]={& _tmp39C};Cyc_Tcutil_terr(bound->loc,({const char*_tmp39B="expecting unsigned int, found %s";_tag_dyneither(_tmp39B,sizeof(char),33);}),_tag_dyneither(_tmp39A,sizeof(void*),1));});});}_LL1FC:;}{
# 1906
struct _RegionHandle _tmp39E=_new_region("r");struct _RegionHandle*r=& _tmp39E;_push_region(r);
{void**_tmp39F=0;
struct Cyc_Absyn_Tqual*_tmp3A0=0;
union Cyc_Absyn_Constraint**_tmp3A1=0;
# 1911
if(topt != 0){
void*_tmp3A2=Cyc_Tcutil_compress(*topt);void*_tmp3A3=_tmp3A2;void*_tmp3AE;struct Cyc_Absyn_Tqual _tmp3AD;struct Cyc_Absyn_Exp*_tmp3AC;union Cyc_Absyn_Constraint*_tmp3AB;struct Cyc_Absyn_PtrInfo _tmp3AA;switch(*((int*)_tmp3A3)){case 5: _LL202: _tmp3AA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A3)->f1;_LL203:
# 1914
 _tmp39F=({void**_tmp3A4=_region_malloc(r,sizeof(*_tmp3A4));_tmp3A4[0]=_tmp3AA.elt_typ;_tmp3A4;});
_tmp3A0=({struct Cyc_Absyn_Tqual*_tmp3A5=_region_malloc(r,sizeof(*_tmp3A5));_tmp3A5[0]=_tmp3AA.elt_tq;_tmp3A5;});
_tmp3A1=({union Cyc_Absyn_Constraint**_tmp3A6=_region_malloc(r,sizeof(*_tmp3A6));_tmp3A6[0]=(_tmp3AA.ptr_atts).zero_term;_tmp3A6;});
goto _LL201;case 8: _LL204: _tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).elt_type;_tmp3AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).tq;_tmp3AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).num_elts;_tmp3AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).zero_term;_LL205:
# 1919
 _tmp39F=({void**_tmp3A7=_region_malloc(r,sizeof(*_tmp3A7));_tmp3A7[0]=_tmp3AE;_tmp3A7;});
_tmp3A0=({struct Cyc_Absyn_Tqual*_tmp3A8=_region_malloc(r,sizeof(*_tmp3A8));_tmp3A8[0]=_tmp3AD;_tmp3A8;});
_tmp3A1=({union Cyc_Absyn_Constraint**_tmp3A9=_region_malloc(r,sizeof(*_tmp3A9));_tmp3A9[0]=_tmp3AB;_tmp3A9;});
goto _LL201;default: _LL206: _LL207:
# 1924
 goto _LL201;}_LL201:;}
# 1927
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,1,t);
if(_tmp39F != 0)Cyc_Tcutil_unify(*_tmp39F,t);
# 1930
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp3AF=0;Cyc_Tcutil_terr(bound->loc,({const char*_tmp3B0="bound is not constant";_tag_dyneither(_tmp3B0,sizeof(char),22);}),_tag_dyneither(_tmp3AF,sizeof(void*),0));});}
# 1934
if(_tmp3A1 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp3A1)){
# 1937
struct Cyc_Absyn_Exp*_tmp3B1=Cyc_Absyn_uint_exp(1,0);_tmp3B1->topt=Cyc_Absyn_uint_typ;
bound=Cyc_Absyn_add_exp(bound,_tmp3B1,0);bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1941
({void*_tmp3B2=0;Cyc_Tcutil_terr(loc,({const char*_tmp3B3="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp3B3,sizeof(char),74);}),_tag_dyneither(_tmp3B2,sizeof(void*),0));});}{
# 1944
void*_tmp3B4=Cyc_Absyn_array_typ(t,_tmp3A0 == 0?Cyc_Absyn_empty_tqual(0):*_tmp3A0,bound,
_tmp3A1 == 0?Cyc_Absyn_false_conref:*_tmp3A1,0);
void*_tmp3B5=_tmp3B4;_npop_handler(0);return _tmp3B5;};}
# 1907
;_pop_region(r);};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1960 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1964
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
adptr=({struct Cyc_Absyn_Aggrdecl**_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6[0]=ad;_tmp3B6;});}else{
# 1970
{struct _handler_cons _tmp3B7;_push_handler(& _tmp3B7);{int _tmp3B9=0;if(setjmp(_tmp3B7.handler))_tmp3B9=1;if(!_tmp3B9){adptr=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);
ad=*adptr;
# 1970
;_pop_handler();}else{void*_tmp3B8=(void*)_exn_thrown;void*_tmp3BA=_tmp3B8;void*_tmp3BE;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3BA)->tag == Cyc_Dict_Absent){_LL209: _LL20A:
# 1973
({struct Cyc_String_pa_PrintArg_struct _tmp3BD;_tmp3BD.tag=0;_tmp3BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));({void*_tmp3BB[1]={& _tmp3BD};Cyc_Tcutil_terr(loc,({const char*_tmp3BC="unbound struct/union name %s";_tag_dyneither(_tmp3BC,sizeof(char),29);}),_tag_dyneither(_tmp3BB,sizeof(void*),1));});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL20B: _tmp3BE=_tmp3BA;_LL20C:(int)_rethrow(_tmp3BE);}_LL208:;}};}
# 1976
*ad_opt=ad;
*tn=ad->name;}
# 1979
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp3C1;_tmp3C1.tag=0;_tmp3C1.f1=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp3C2="struct";_tag_dyneither(_tmp3C2,sizeof(char),7);}):({const char*_tmp3C3="union";_tag_dyneither(_tmp3C3,sizeof(char),6);}));({void*_tmp3BF[1]={& _tmp3C1};Cyc_Tcutil_terr(loc,({const char*_tmp3C0="can't construct abstract %s";_tag_dyneither(_tmp3C0,sizeof(char),28);}),_tag_dyneither(_tmp3BF,sizeof(void*),1));});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1983
struct _RegionHandle _tmp3C4=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3C4;_push_region(rgn);
# 1988
{struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp3C5=Cyc_Tcenv_new_status(te);
if(_tmp3C5 == Cyc_Tcenv_InNew)
te2=Cyc_Tcenv_set_new_status(rgn,Cyc_Tcenv_InNewAggr,te);else{
# 1997
te2=Cyc_Tcenv_set_new_status(rgn,_tmp3C5,te);}{
# 1999
struct _tuple12 _tmp3C6=({struct _tuple12 _tmp3EE;_tmp3EE.f1=Cyc_Tcenv_lookup_type_vars(te2);_tmp3EE.f2=rgn;_tmp3EE;});
struct Cyc_List_List*_tmp3C7=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp3C6,ad->tvs);
struct Cyc_List_List*_tmp3C8=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp3C6,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp3C9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3C7);
struct Cyc_List_List*_tmp3CA=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3C8);
struct Cyc_List_List*_tmp3CB=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmp3C7,_tmp3C8);
void*res_typ;
# 2010
if(topt != 0){
void*_tmp3CC=Cyc_Tcutil_compress(*topt);void*_tmp3CD=_tmp3CC;struct Cyc_Absyn_Aggrdecl**_tmp3D3;struct Cyc_List_List*_tmp3D2;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CD)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CD)->f1).aggr_info).KnownAggr).tag == 2){_LL20E: _tmp3D3=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CD)->f1).aggr_info).KnownAggr).val;_tmp3D2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3CD)->f1).targs;_LL20F:
# 2013
 if(*_tmp3D3 == *adptr){
{struct Cyc_List_List*_tmp3CE=_tmp3C9;for(0;_tmp3CE != 0  && _tmp3D2 != 0;
(_tmp3CE=_tmp3CE->tl,_tmp3D2=_tmp3D2->tl)){
Cyc_Tcutil_unify((void*)_tmp3CE->hd,(void*)_tmp3D2->hd);}}
# 2018
res_typ=*topt;
goto _LL20D;}
# 2021
goto _LL211;}else{goto _LL210;}}else{_LL210: _LL211:
# 2023
 res_typ=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3D0;_tmp3D0.tag=11;_tmp3D0.f1=({struct Cyc_Absyn_AggrInfo _tmp3D1;_tmp3D1.aggr_info=Cyc_Absyn_KnownAggr(adptr);_tmp3D1.targs=_tmp3C9;_tmp3D1;});_tmp3D0;});_tmp3CF;});}_LL20D:;}else{
# 2026
res_typ=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp3D5;_tmp3D5.tag=11;_tmp3D5.f1=({struct Cyc_Absyn_AggrInfo _tmp3D6;_tmp3D6.aggr_info=Cyc_Absyn_KnownAggr(adptr);_tmp3D6.targs=_tmp3C9;_tmp3D6;});_tmp3D5;});_tmp3D4;});}{
# 2029
struct Cyc_List_List*_tmp3D7=*ts;
struct Cyc_List_List*_tmp3D8=_tmp3CA;
while(_tmp3D7 != 0  && _tmp3D8 != 0){
# 2033
Cyc_Tcutil_check_type(loc,te2,Cyc_Tcenv_lookup_type_vars(te2),& Cyc_Tcutil_ak,1,0,(void*)_tmp3D7->hd);
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp3D7->hd);
Cyc_Tcutil_unify((void*)_tmp3D7->hd,(void*)_tmp3D8->hd);
_tmp3D7=_tmp3D7->tl;
_tmp3D8=_tmp3D8->tl;}
# 2039
if(_tmp3D7 != 0)
({void*_tmp3D9=0;Cyc_Tcutil_terr(loc,({const char*_tmp3DA="too many explicit witness types";_tag_dyneither(_tmp3DA,sizeof(char),32);}),_tag_dyneither(_tmp3D9,sizeof(void*),0));});
# 2042
*ts=_tmp3CA;{
# 2045
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3DB=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3DC=_tmp3DB;struct Cyc_Absyn_Aggrfield*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E7;_LL213: _tmp3E8=_tmp3DC->f1;_tmp3E7=_tmp3DC->f2;_LL214:;{
void*_tmp3DD=Cyc_Tcutil_rsubstitute(rgn,_tmp3CB,_tmp3E8->type);
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te2),& _tmp3DD,_tmp3E7);
# 2056
if(!Cyc_Tcutil_coerce_arg(te2,_tmp3E7,_tmp3DD,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3E4;_tmp3E4.tag=0;_tmp3E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2060
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3E7->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp3E3;_tmp3E3.tag=0;_tmp3E3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2059
Cyc_Absynpp_typ2string(_tmp3DD));({struct Cyc_String_pa_PrintArg_struct _tmp3E2;_tmp3E2.tag=0;_tmp3E2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));({struct Cyc_String_pa_PrintArg_struct _tmp3E1;_tmp3E1.tag=0;_tmp3E1.f1=(struct _dyneither_ptr)(
# 2058
ad->kind == Cyc_Absyn_StructA?({const char*_tmp3E5="struct";_tag_dyneither(_tmp3E5,sizeof(char),7);}):({const char*_tmp3E6="union";_tag_dyneither(_tmp3E6,sizeof(char),6);}));({struct Cyc_String_pa_PrintArg_struct _tmp3E0;_tmp3E0.tag=0;_tmp3E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3E8->name);({void*_tmp3DE[5]={& _tmp3E0,& _tmp3E1,& _tmp3E2,& _tmp3E3,& _tmp3E4};Cyc_Tcutil_terr(_tmp3E7->loc,({const char*_tmp3DF="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp3DF,sizeof(char),40);}),_tag_dyneither(_tmp3DE,sizeof(void*),5));});});});});});});
# 2061
Cyc_Tcutil_explain_failure();}};}{
# 2065
struct Cyc_List_List*_tmp3E9=0;
{struct Cyc_List_List*_tmp3EA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp3EA != 0;_tmp3EA=_tmp3EA->tl){
_tmp3E9=({struct Cyc_List_List*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->hd=({struct _tuple0*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC->f1=Cyc_Tcutil_rsubstitute(rgn,_tmp3CB,(*((struct _tuple0*)_tmp3EA->hd)).f1);_tmp3EC->f2=
Cyc_Tcutil_rsubstitute(rgn,_tmp3CB,(*((struct _tuple0*)_tmp3EA->hd)).f2);_tmp3EC;});_tmp3EB->tl=_tmp3E9;_tmp3EB;});}}
# 2070
_tmp3E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3E9);
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp3E9);{
void*_tmp3ED=res_typ;_npop_handler(0);return _tmp3ED;};};};};};}
# 1988
;_pop_region(rgn);};}
# 2077
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2079
struct _RegionHandle _tmp3EF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp3EF;_push_region(rgn);{
void*_tmp3F0=Cyc_Tcutil_compress(ts);void*_tmp3F1=_tmp3F0;enum Cyc_Absyn_AggrKind _tmp400;struct Cyc_List_List*_tmp3FF;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F1)->tag == 12){_LL216: _tmp400=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F1)->f1;_tmp3FF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F1)->f2;_LL217:
# 2082
 if(_tmp400 == Cyc_Absyn_UnionA)
({void*_tmp3F2=0;Cyc_Tcutil_terr(loc,({const char*_tmp3F3="expecting struct but found union";_tag_dyneither(_tmp3F3,sizeof(char),33);}),_tag_dyneither(_tmp3F2,sizeof(void*),0));});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,loc,args,Cyc_Absyn_StructA,_tmp3FF);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3F4=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3F5=_tmp3F4;struct Cyc_Absyn_Aggrfield*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FB;_LL21B: _tmp3FC=_tmp3F5->f1;_tmp3FB=_tmp3F5->f2;_LL21C:;
Cyc_Tcexp_tcExpInitializer(Cyc_Tcenv_clear_abstract_val_ok(rgn,te),& _tmp3FC->type,_tmp3FB);
# 2091
if(!Cyc_Tcutil_coerce_arg(te,_tmp3FB,_tmp3FC->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3FA;_tmp3FA.tag=0;_tmp3FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2094
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3FB->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp3F9;_tmp3F9.tag=0;_tmp3F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2093
Cyc_Absynpp_typ2string(_tmp3FC->type));({struct Cyc_String_pa_PrintArg_struct _tmp3F8;_tmp3F8.tag=0;_tmp3F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3FC->name);({void*_tmp3F6[3]={& _tmp3F8,& _tmp3F9,& _tmp3FA};Cyc_Tcutil_terr(_tmp3FB->loc,({const char*_tmp3F7="field %s of struct expects type %s != %s";_tag_dyneither(_tmp3F7,sizeof(char),41);}),_tag_dyneither(_tmp3F6,sizeof(void*),3));});});});});
# 2095
Cyc_Tcutil_explain_failure();}}
# 2098
goto _LL215;};}else{_LL218: _LL219:
({void*_tmp3FD=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3FE="tcAnonStruct: wrong type";_tag_dyneither(_tmp3FE,sizeof(char),25);}),_tag_dyneither(_tmp3FD,sizeof(void*),0));});}_LL215:;}{
# 2102
void*_tmp401=ts;_npop_handler(0);return _tmp401;};
# 2079
;_pop_region(rgn);}
# 2106
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2109
struct _RegionHandle _tmp402=_new_region("rgn");struct _RegionHandle*rgn=& _tmp402;_push_region(rgn);
{struct _tuple12 _tmp403=({struct _tuple12 _tmp41A;_tmp41A.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp41A.f2=rgn;_tmp41A;});
struct Cyc_List_List*_tmp404=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp403,tud->tvs);
struct Cyc_List_List*_tmp405=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp404);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp418;_tmp418.tag=4;_tmp418.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp419;_tmp419.field_info=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp419.targs=_tmp405;_tmp419;});_tmp418;});_tmp417;});
# 2117
if(topt != 0){
void*_tmp406=Cyc_Tcutil_compress(*topt);void*_tmp407=_tmp406;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp407)->tag == 4){_LL21E: _LL21F:
 Cyc_Tcutil_unify(*topt,res);goto _LL21D;}else{_LL220: _LL221:
 goto _LL21D;}_LL21D:;}{
# 2123
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp404 != 0)t=Cyc_Tcutil_rsubstitute(rgn,_tmp404,t);
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp40C;_tmp40C.tag=0;_tmp40C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2134
e->topt == 0?(struct _dyneither_ptr)({const char*_tmp40D="?";_tag_dyneither(_tmp40D,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));({struct Cyc_String_pa_PrintArg_struct _tmp40B;_tmp40B.tag=0;_tmp40B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2133
Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp40A;_tmp40A.tag=0;_tmp40A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp408[3]={& _tmp40A,& _tmp40B,& _tmp40C};Cyc_Tcutil_terr(e->loc,({const char*_tmp409="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp409,sizeof(char),82);}),_tag_dyneither(_tmp408,sizeof(void*),3));});});});});
# 2135
Cyc_Tcutil_explain_failure();}}
# 2138
if(es != 0){
void*_tmp411=({struct Cyc_String_pa_PrintArg_struct _tmp410;_tmp410.tag=0;_tmp410.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2141
Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp40E[1]={& _tmp410};Cyc_Tcexp_expr_err(te,((struct Cyc_Absyn_Exp*)es->hd)->loc,topt,({const char*_tmp40F="too many arguments for datatype constructor %s";_tag_dyneither(_tmp40F,sizeof(char),47);}),_tag_dyneither(_tmp40E,sizeof(void*),1));});});_npop_handler(0);return _tmp411;}
if(ts != 0){
void*_tmp415=({struct Cyc_String_pa_PrintArg_struct _tmp414;_tmp414.tag=0;_tmp414.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));({void*_tmp412[1]={& _tmp414};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp413="too few arguments for datatype constructor %s";_tag_dyneither(_tmp413,sizeof(char),46);}),_tag_dyneither(_tmp412,sizeof(void*),1));});});_npop_handler(0);return _tmp415;}{
void*_tmp416=res;_npop_handler(0);return _tmp416;};};}
# 2110
;_pop_region(rgn);}
# 2149
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2151
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2153
if(topt != 0){
void*_tmp41B=Cyc_Tcutil_compress(*topt);void*_tmp41C=_tmp41B;void*_tmp41D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41C)->tag == 5){_LL223: _tmp41D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp41C)->f1).elt_typ;_LL224:
# 2156
 Cyc_Tcutil_unify(_tmp41D,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL222;}else{_LL225: _LL226:
 goto _LL222;}_LL222:;}
# 2162
({struct Cyc_String_pa_PrintArg_struct _tmp421;_tmp421.tag=0;_tmp421.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp420;_tmp420.tag=0;_tmp420.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp422="calloc";_tag_dyneither(_tmp422,sizeof(char),7);}):(struct _dyneither_ptr)({const char*_tmp423="malloc";_tag_dyneither(_tmp423,sizeof(char),7);})));({void*_tmp41E[2]={& _tmp420,& _tmp421};Cyc_Tcutil_terr(loc,({const char*_tmp41F="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp41F,sizeof(char),60);}),_tag_dyneither(_tmp41E,sizeof(void*),2));});});});}
# 2166
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2168
enum Cyc_Absyn_AliasQual _tmp424=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp425=_tmp424;if(_tmp425 == Cyc_Absyn_Unique){_LL228: _LL229:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL22A: _LL22B:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL227:;}
# 2174
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2179
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2182
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp42D;_tmp42D.tag=15;_tmp42D.f1=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));_tmp42D;});_tmp42C;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp426=Cyc_Tcutil_compress(handle_type);void*_tmp427=_tmp426;void*_tmp42B;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp427)->tag == 15){_LL22D: _tmp42B=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp427)->f1;_LL22E:
# 2187
 rgn=_tmp42B;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22C;}else{_LL22F: _LL230:
# 2191
({struct Cyc_String_pa_PrintArg_struct _tmp42A;_tmp42A.tag=0;_tmp42A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));({void*_tmp428[1]={& _tmp42A};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc,({const char*_tmp429="expecting region_t type but found %s";_tag_dyneither(_tmp429,sizeof(char),37);}),_tag_dyneither(_tmp428,sizeof(void*),1));});});
goto _LL22C;}_LL22C:;}else{
# 2198
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2205
{struct _RegionHandle _tmp42E=_new_region("r");struct _RegionHandle*r=& _tmp42E;_push_region(r);
Cyc_Tcexp_tcExp(Cyc_Tcenv_clear_abstract_val_ok(r,te),& Cyc_Absyn_uint_typ,*e);;_pop_region(r);}{
# 2216 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp42F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp430="calloc with empty type";_tag_dyneither(_tmp430,sizeof(char),23);}),_tag_dyneither(_tmp42F,sizeof(void*),0));});
elt_type=*((void**)_check_null(*t));
Cyc_Tcutil_check_type(loc,te,Cyc_Tcenv_lookup_type_vars(te),& Cyc_Tcutil_tmk,1,0,elt_type);
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2228
void*er=(*e)->r;
retry_sizeof: {
void*_tmp431=er;struct Cyc_Absyn_Exp*_tmp447;struct Cyc_Absyn_Exp*_tmp446;void*_tmp445;switch(*((int*)_tmp431)){case 16: _LL232: _tmp445=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp431)->f1;_LL233:
# 2232
 elt_type=_tmp445;
*t=({void**_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432[0]=elt_type;_tmp432;});
num_elts=Cyc_Absyn_uint_exp(1,0);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL231;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp431)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp431)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp431)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp431)->f2)->tl)->tl == 0){_LL234: _tmp447=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp431)->f2)->hd;_tmp446=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp431)->f2)->tl)->hd;_LL235:
# 2239
{struct _tuple0 _tmp433=({struct _tuple0 _tmp439;_tmp439.f1=_tmp447->r;_tmp439.f2=_tmp446->r;_tmp439;});struct _tuple0 _tmp434=_tmp433;void*_tmp438;void*_tmp437;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp434.f1)->tag == 16){_LL239: _tmp437=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp434.f1)->f1;_LL23A:
# 2241
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp437);
elt_type=_tmp437;
*t=({void**_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435[0]=elt_type;_tmp435;});
num_elts=_tmp446;
one_elt=0;
goto _LL238;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp434.f2)->tag == 16){_LL23B: _tmp438=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp434.f2)->f1;_LL23C:
# 2248
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp438);
elt_type=_tmp438;
*t=({void**_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436[0]=elt_type;_tmp436;});
num_elts=_tmp447;
one_elt=0;
goto _LL238;}else{_LL23D: _LL23E:
 goto No_sizeof;}}_LL238:;}
# 2256
goto _LL231;}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}default: _LL236: _LL237:
# 2258
 No_sizeof: {
# 2261
struct Cyc_Absyn_Exp*_tmp43A=*e;
{void*_tmp43B=_tmp43A->r;void*_tmp43C=_tmp43B;struct Cyc_Absyn_Exp*_tmp43D;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp43C)->tag == 13){_LL240: _tmp43D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp43C)->f2;_LL241:
 _tmp43A=_tmp43D;goto _LL23F;}else{_LL242: _LL243:
 goto _LL23F;}_LL23F:;}
# 2266
{void*_tmp43E=Cyc_Tcutil_compress((void*)_check_null(_tmp43A->topt));void*_tmp43F=_tmp43E;void*_tmp443;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp43F)->tag == 19){_LL245: _tmp443=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp43F)->f1;_LL246:
# 2268
{void*_tmp440=Cyc_Tcutil_compress(_tmp443);void*_tmp441=_tmp440;struct Cyc_Absyn_Exp*_tmp442;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp441)->tag == 18){_LL24A: _tmp442=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp441)->f1;_LL24B:
# 2270
 er=_tmp442->r;goto retry_sizeof;}else{_LL24C: _LL24D:
 goto _LL249;}_LL249:;}
# 2273
goto _LL244;}else{_LL247: _LL248:
 goto _LL244;}_LL244:;}
# 2276
elt_type=Cyc_Absyn_char_typ;
*t=({void**_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444[0]=elt_type;_tmp444;});
num_elts=*e;
one_elt=0;}
# 2281
goto _LL231;}_LL231:;}}
# 2285
*is_fat=!one_elt;
# 2288
{void*_tmp448=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp44B;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp448)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp448)->f1).aggr_info).KnownAggr).tag == 2){_LL24F: _tmp44B=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp448)->f1).aggr_info).KnownAggr).val;_LL250:
# 2290
 if(_tmp44B->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44B->impl))->exist_vars != 0)
({void*_tmp449=0;Cyc_Tcutil_terr(loc,({const char*_tmp44A="malloc with existential types not yet implemented";_tag_dyneither(_tmp44A,sizeof(char),50);}),_tag_dyneither(_tmp449,sizeof(void*),0));});
goto _LL24E;}else{goto _LL251;}}else{_LL251: _LL252:
 goto _LL24E;}_LL24E:;}{
# 2297
void*(*_tmp44C)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp44D=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp44E=Cyc_Tcutil_compress(*topt);void*_tmp44F=_tmp44E;union Cyc_Absyn_Constraint*_tmp45C;union Cyc_Absyn_Constraint*_tmp45B;union Cyc_Absyn_Constraint*_tmp45A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->tag == 5){_LL254: _tmp45C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).ptr_atts).nullable;_tmp45B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).ptr_atts).bounds;_tmp45A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).ptr_atts).zero_term;_LL255:
# 2302
 _tmp44D=_tmp45A;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp45C))
_tmp44C=Cyc_Absyn_star_typ;
# 2307
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp45A) && !(*is_calloc)){
({void*_tmp450=0;Cyc_Tcutil_warn(loc,({const char*_tmp451="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp451,sizeof(char),55);}),_tag_dyneither(_tmp450,sizeof(void*),0));});
*is_calloc=1;}
# 2313
{void*_tmp452=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp45B);void*_tmp453=_tmp452;struct Cyc_Absyn_Exp*_tmp459;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp453)->tag == 0){_LL259: _LL25A:
 goto _LL258;}else{_LL25B: _tmp459=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp453)->f1;if(!one_elt){_LL25C: {
# 2316
int _tmp454=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp454  && Cyc_Evexp_same_const_exp(_tmp459,num_elts)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp45B),_tmp44D);}
# 2322
{void*_tmp455=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp456=_tmp455;void*_tmp458;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp456)->tag == 19){_LL260: _tmp458=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp456)->f1;_LL261: {
# 2324
struct Cyc_Absyn_Exp*_tmp457=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmp458,0),0,Cyc_Absyn_No_coercion,0);
# 2326
if(Cyc_Evexp_same_const_exp(_tmp457,_tmp459)){
*is_fat=0;
return Cyc_Absyn_atb_typ(elt_type,rgn,Cyc_Absyn_empty_tqual(0),((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp45B),_tmp44D);}
# 2331
goto _LL25F;}}else{_LL262: _LL263:
 goto _LL25F;}_LL25F:;}
# 2334
goto _LL258;}}else{_LL25D: _LL25E:
 goto _LL258;}}_LL258:;}
# 2337
goto _LL253;}else{_LL256: _LL257:
 goto _LL253;}_LL253:;}
# 2340
if(!one_elt)_tmp44C=Cyc_Absyn_dyneither_typ;
return _tmp44C(elt_type,rgn,Cyc_Absyn_empty_tqual(0),_tmp44D);};};}
# 2345
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2347
struct _RegionHandle _tmp45D=_new_region("r");struct _RegionHandle*r=& _tmp45D;_push_region(r);{
struct Cyc_Tcenv_Tenv*_tmp45E=Cyc_Tcenv_enter_lhs(r,te);
Cyc_Tcexp_tcExpNoPromote(_tmp45E,0,e1);{
void*_tmp45F=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp45E,& _tmp45F,e2);};}{
# 2353
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2356
{void*_tmp460=Cyc_Tcutil_compress(t1);void*_tmp461=_tmp460;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp461)->tag == 8){_LL265: _LL266:
({void*_tmp462=0;Cyc_Tcutil_terr(loc,({const char*_tmp463="cannot assign to an array";_tag_dyneither(_tmp463,sizeof(char),26);}),_tag_dyneither(_tmp462,sizeof(void*),0));});goto _LL264;}else{_LL267: _LL268:
 goto _LL264;}_LL264:;}
# 2361
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp464=0;Cyc_Tcutil_terr(loc,({const char*_tmp465="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp465,sizeof(char),58);}),_tag_dyneither(_tmp464,sizeof(void*),0));});
# 2365
if(!Cyc_Absyn_is_lvalue(e1)){
void*_tmp468=({void*_tmp466=0;Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp467="swap non-lvalue";_tag_dyneither(_tmp467,sizeof(char),16);}),_tag_dyneither(_tmp466,sizeof(void*),0));});_npop_handler(0);return _tmp468;}
if(!Cyc_Absyn_is_lvalue(e2)){
void*_tmp46B=({void*_tmp469=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp46A="swap non-lvalue";_tag_dyneither(_tmp46A,sizeof(char),16);}),_tag_dyneither(_tmp469,sizeof(void*),0));});_npop_handler(0);return _tmp46B;}{
# 2370
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp46E=({void*_tmp46C=0;Cyc_Tcexp_expr_err(te,e1->loc,topt,({const char*_tmp46D="swap value in zeroterm array";_tag_dyneither(_tmp46D,sizeof(char),29);}),_tag_dyneither(_tmp46C,sizeof(void*),0));});_npop_handler(0);return _tmp46E;}
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2)){
void*_tmp471=({void*_tmp46F=0;Cyc_Tcexp_expr_err(te,e2->loc,topt,({const char*_tmp470="swap value in zeroterm array";_tag_dyneither(_tmp470,sizeof(char),29);}),_tag_dyneither(_tmp46F,sizeof(void*),0));});_npop_handler(0);return _tmp471;}
# 2377
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp472=({struct Cyc_String_pa_PrintArg_struct _tmp477;_tmp477.tag=0;_tmp477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp476;_tmp476.tag=0;_tmp476.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp474[2]={& _tmp476,& _tmp477};Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp475="type mismatch: %s != %s";_tag_dyneither(_tmp475,sizeof(char),24);}),_tag_dyneither(_tmp474,sizeof(void*),2));});});});
void*_tmp473=_tmp472;_npop_handler(0);return _tmp473;}{
# 2384
void*_tmp478=(void*)& Cyc_Absyn_VoidType_val;_npop_handler(0);return _tmp478;};};};
# 2347
;_pop_region(r);}
# 2388
int Cyc_Tcexp_in_stmt_exp=0;
# 2390
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
{struct _RegionHandle _tmp479=_new_region("r");struct _RegionHandle*r=& _tmp479;_push_region(r);
{int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
Cyc_Tcstmt_tcStmt(Cyc_Tcenv_clear_abstract_val_ok(r,te),s,1);
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;}
# 2392
;_pop_region(r);}
# 2397
while(1){
void*_tmp47A=s->r;void*_tmp47B=_tmp47A;struct Cyc_Absyn_Decl*_tmp486;struct Cyc_Absyn_Stmt*_tmp485;struct Cyc_Absyn_Stmt*_tmp484;struct Cyc_Absyn_Stmt*_tmp483;struct Cyc_Absyn_Exp*_tmp482;switch(*((int*)_tmp47B)){case 1: _LL26A: _tmp482=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp47B)->f1;_LL26B: {
# 2401
void*_tmp47C=(void*)_check_null(_tmp482->topt);
if(!Cyc_Tcutil_unify(_tmp47C,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)))){
({struct Cyc_String_pa_PrintArg_struct _tmp47F;_tmp47F.tag=0;_tmp47F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp47C));({void*_tmp47D[1]={& _tmp47F};Cyc_Tcutil_terr(loc,({const char*_tmp47E="statement expression returns type %s";_tag_dyneither(_tmp47E,sizeof(char),37);}),_tag_dyneither(_tmp47D,sizeof(void*),1));});});
Cyc_Tcutil_explain_failure();}
# 2407
return _tmp47C;}case 2: _LL26C: _tmp484=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47B)->f1;_tmp483=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47B)->f2;_LL26D:
 s=_tmp483;continue;case 12: _LL26E: _tmp486=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47B)->f1;_tmp485=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47B)->f2;_LL26F:
 s=_tmp485;continue;default: _LL270: _LL271:
# 2411
 return({void*_tmp480=0;Cyc_Tcexp_expr_err(te,loc,topt,({const char*_tmp481="statement expression must end with expression";_tag_dyneither(_tmp481,sizeof(char),46);}),_tag_dyneither(_tmp480,sizeof(void*),0));});}_LL269:;}}
# 2416
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t;
{struct _RegionHandle _tmp487=_new_region("r");struct _RegionHandle*r=& _tmp487;_push_region(r);
t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExp(Cyc_Tcenv_enter_abstract_val_ok(r,te),0,e));;_pop_region(r);}
# 2421
{void*_tmp488=t;struct Cyc_Absyn_Aggrdecl*_tmp48C;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp488)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp488)->f1).aggr_info).KnownAggr).tag == 2){_LL273: _tmp48C=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp488)->f1).aggr_info).KnownAggr).val;_LL274:
# 2423
 if((_tmp48C->kind == Cyc_Absyn_UnionA  && _tmp48C->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp48C->impl))->tagged)goto _LL272;
goto _LL276;}else{goto _LL275;}}else{_LL275: _LL276:
# 2426
({struct Cyc_String_pa_PrintArg_struct _tmp48B;_tmp48B.tag=0;_tmp48B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp489[1]={& _tmp48B};Cyc_Tcutil_terr(loc,({const char*_tmp48A="expecting @tagged union but found %s";_tag_dyneither(_tmp48A,sizeof(char),37);}),_tag_dyneither(_tmp489,sizeof(void*),1));});});
goto _LL272;}_LL272:;}
# 2429
return Cyc_Absyn_uint_typ;}
# 2433
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2437
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct _RegionHandle _tmp48D=_new_region("r");struct _RegionHandle*r=& _tmp48D;_push_region(r);
{struct Cyc_Tcenv_Tenv*_tmp48E=Cyc_Tcenv_clear_abstract_val_ok(r,Cyc_Tcenv_set_new_status(r,Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2442
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp496;_tmp496.tag=15;_tmp496.f1=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp48E));_tmp496;});_tmp495;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp48E,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp48F=Cyc_Tcutil_compress(handle_type);void*_tmp490=_tmp48F;void*_tmp494;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp490)->tag == 15){_LL278: _tmp494=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp490)->f1;_LL279:
# 2447
 rgn=_tmp494;
Cyc_Tcenv_check_rgn_accessible(_tmp48E,loc,rgn);
goto _LL277;}else{_LL27A: _LL27B:
# 2451
({struct Cyc_String_pa_PrintArg_struct _tmp493;_tmp493.tag=0;_tmp493.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));({void*_tmp491[1]={& _tmp493};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc,({const char*_tmp492="expecting region_t type but found %s";_tag_dyneither(_tmp492,sizeof(char),37);}),_tag_dyneither(_tmp491,sizeof(void*),1));});});
goto _LL277;}_LL277:;}else{
# 2458
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
rgn=Cyc_Tcexp_mallocRgn(optrgn);
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2466
void*_tmp497=e1->r;void*_tmp498=_tmp497;struct Cyc_List_List*_tmp4BD;struct Cyc_Core_Opt*_tmp4BC;struct Cyc_List_List*_tmp4BB;switch(*((int*)_tmp498)){case 26: _LL27D: _LL27E:
 goto _LL280;case 27: _LL27F: _LL280: {
# 2471
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp48E,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp499=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp49A="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp49A,sizeof(char),45);}),_tag_dyneither(_tmp499,sizeof(void*),0));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,1);
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp48E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp48E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2482
void*_tmp49B=res_typ;_npop_handler(0);return _tmp49B;};}case 35: _LL281: _tmp4BC=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp4BB=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL282:
# 2484
 e1->r=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp49D;_tmp49D.tag=25;_tmp49D.f1=_tmp4BB;_tmp49D;});_tmp49C;});
_tmp4BD=_tmp4BB;goto _LL284;case 25: _LL283: _tmp4BD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_LL284: {
# 2487
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp49E=Cyc_Tcutil_compress(*topt);void*_tmp49F=_tmp49E;void**_tmp4A2;struct Cyc_Absyn_Tqual _tmp4A1;union Cyc_Absyn_Constraint*_tmp4A0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49F)->tag == 5){_LL28C: _tmp4A2=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49F)->f1).elt_typ;_tmp4A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49F)->f1).elt_tq;_tmp4A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49F)->f1).ptr_atts).zero_term;_LL28D:
# 2493
 elt_typ_opt=_tmp4A2;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4A0);
goto _LL28B;}else{_LL28E: _LL28F:
 goto _LL28B;}_LL28B:;}{
# 2499
void*res_typ=Cyc_Tcexp_tcArray(_tmp48E,e1->loc,elt_typ_opt,0,zero_term,_tmp4BD);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp4A3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A4="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp4A4,sizeof(char),50);}),_tag_dyneither(_tmp4A3,sizeof(void*),0));});
res_typ=Cyc_Tcutil_promote_array(res_typ,rgn,0);
if(topt != 0){
# 2508
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp48E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp48E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2514
void*_tmp4A5=res_typ;_npop_handler(0);return _tmp4A5;};};}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp498)->f1).Wstring_c).tag){case 8: _LL285: _LL286: {
# 2519
void*_tmp4A6=Cyc_Absyn_atb_typ(Cyc_Absyn_char_typ,rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2521
void*_tmp4A7=Cyc_Tcexp_tcExp(_tmp48E,& _tmp4A6,e1);
void*_tmp4AA=Cyc_Absyn_atb_typ(_tmp4A7,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp4A9;_tmp4A9.tag=1;_tmp4A9.f1=Cyc_Absyn_uint_exp(1,0);_tmp4A9;});_tmp4A8;}),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp4AA;}case 9: _LL287: _LL288: {
# 2526
void*_tmp4AB=Cyc_Absyn_atb_typ(Cyc_Absyn_wchar_typ(),rgn,Cyc_Absyn_const_tqual(0),(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);
# 2528
void*_tmp4AC=Cyc_Tcexp_tcExp(_tmp48E,& _tmp4AB,e1);
void*_tmp4AF=Cyc_Absyn_atb_typ(_tmp4AC,rgn,Cyc_Absyn_empty_tqual(0),(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp4AE;_tmp4AE.tag=1;_tmp4AE.f1=Cyc_Absyn_uint_exp(1,0);_tmp4AE;});_tmp4AD;}),Cyc_Absyn_false_conref);_npop_handler(0);return _tmp4AF;}default: goto _LL289;}default: _LL289: _LL28A:
# 2535
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp4B0=Cyc_Tcutil_compress(*topt);void*_tmp4B1=_tmp4B0;void**_tmp4B3;struct Cyc_Absyn_Tqual _tmp4B2;switch(*((int*)_tmp4B1)){case 5: _LL291: _tmp4B3=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B1)->f1).elt_typ;_tmp4B2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4B1)->f1).elt_tq;_LL292:
# 2541
 topt2=_tmp4B3;goto _LL290;case 3: _LL293: _LL294:
# 2545
 bogus=*topt;
topt2=& bogus;
goto _LL290;default: _LL295: _LL296:
 goto _LL290;}_LL290:;}{
# 2551
void*telt=Cyc_Tcexp_tcExp(_tmp48E,topt2,e1);
# 2553
struct _RegionHandle _tmp4B4=_new_region("temp");struct _RegionHandle*temp=& _tmp4B4;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,telt) && !
Cyc_Tcutil_is_noalias_path(temp,e1))
({void*_tmp4B5=0;Cyc_Tcutil_terr(e1->loc,({const char*_tmp4B6="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp4B6,sizeof(char),49);}),_tag_dyneither(_tmp4B5,sizeof(void*),0));});{
# 2558
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4B9;_tmp4B9.tag=5;_tmp4B9.f1=({struct Cyc_Absyn_PtrInfo _tmp4BA;_tmp4BA.elt_typ=telt;_tmp4BA.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp4BA.ptr_atts=({(_tmp4BA.ptr_atts).rgn=rgn;(_tmp4BA.ptr_atts).nullable=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4BA.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp4BA.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp4BA.ptr_atts).ptrloc=0;_tmp4BA.ptr_atts;});_tmp4BA;});_tmp4B9;});_tmp4B8;});
# 2563
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp48E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp48E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}{
# 2570
void*_tmp4B7=res_typ;_npop_handler(1);return _tmp4B7;};};
# 2553
;_pop_region(temp);};}}_LL27C:;};}
# 2439
;_pop_region(r);}
# 2577
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
e->topt=(t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0));
return t;}
# 2587
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2591
{struct _RegionHandle _tmp4BE=_new_region("temp");struct _RegionHandle*temp=& _tmp4BE;_push_region(temp);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(temp,t) && !Cyc_Tcutil_is_noalias_path(temp,e))
# 2597
({void*_tmp4BF=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp4C0="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp4C0,sizeof(char),49);}),_tag_dyneither(_tmp4BF,sizeof(void*),0));});
# 2592
;_pop_region(temp);}{
# 2600
void*_tmp4C1=e->r;void*_tmp4C2=_tmp4C1;switch(*((int*)_tmp4C2)){case 25: _LL298: _LL299:
 goto _LL29B;case 26: _LL29A: _LL29B:
 goto _LL29D;case 27: _LL29C: _LL29D:
 goto _LL29F;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4C2)->f1).String_c).tag){case 9: _LL29E: _LL29F:
 goto _LL2A1;case 8: _LL2A0: _LL2A1:
 return t;default: goto _LL2A2;}default: _LL2A2: _LL2A3:
# 2607
 t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_array(t)){
# 2610
t=Cyc_Tcutil_promote_array(t,(Cyc_Tcutil_addressof_props(te,e)).f2,0);
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2613
return t;}_LL297:;};}
# 2624 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp4C3=e->r;void*_tmp4C4=_tmp4C3;struct Cyc_Absyn_Exp*_tmp4E9;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4C4)->tag == 11){_LL2A5: _tmp4E9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4C4)->f1;_LL2A6:
# 2628
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp4E9);
_tmp4E9->topt=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp4E9->topt),0);
e->topt=_tmp4E9->topt;
goto _LL2A4;}else{_LL2A7: _LL2A8:
# 2634
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
e->topt=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);
# 2637
{void*_tmp4C5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp4C6=_tmp4C5;void*_tmp4E8;struct Cyc_Absyn_Tqual _tmp4E7;void*_tmp4E6;union Cyc_Absyn_Constraint*_tmp4E5;union Cyc_Absyn_Constraint*_tmp4E4;union Cyc_Absyn_Constraint*_tmp4E3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C6)->tag == 5){_LL2AA: _tmp4E8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C6)->f1).elt_typ;_tmp4E7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C6)->f1).elt_tq;_tmp4E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C6)->f1).ptr_atts).rgn;_tmp4E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C6)->f1).ptr_atts).nullable;_tmp4E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C6)->f1).ptr_atts).bounds;_tmp4E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C6)->f1).ptr_atts).zero_term;_LL2AB:
# 2639
{void*_tmp4C7=Cyc_Tcutil_compress(_tmp4E8);void*_tmp4C8=_tmp4C7;struct Cyc_List_List*_tmp4E2;void*_tmp4E1;struct Cyc_Absyn_Tqual _tmp4E0;void*_tmp4DF;struct Cyc_List_List*_tmp4DE;int _tmp4DD;struct Cyc_Absyn_VarargInfo*_tmp4DC;struct Cyc_List_List*_tmp4DB;struct Cyc_List_List*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_List_List*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_List_List*_tmp4D6;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->tag == 9){_LL2AF: _tmp4E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).tvars;_tmp4E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).effect;_tmp4E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).ret_tqual;_tmp4DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).ret_typ;_tmp4DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).args;_tmp4DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).c_varargs;_tmp4DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).cyc_varargs;_tmp4DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).rgn_po;_tmp4DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).attributes;_tmp4D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).requires_clause;_tmp4D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).requires_relns;_tmp4D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).ensures_clause;_tmp4D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C8)->f1).ensures_relns;_LL2B0:
# 2641
 if(_tmp4E2 != 0){
struct _RegionHandle _tmp4C9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp4C9;_push_region(rgn);
{struct _tuple12 _tmp4CA=({struct _tuple12 _tmp4D5;_tmp4D5.f1=Cyc_Tcenv_lookup_type_vars(te);_tmp4D5.f2=rgn;_tmp4D5;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmp4CA,_tmp4E2);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2650
_tmp4DB=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp4DB);
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp4DB);{
void*ftyp=Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp4D3;_tmp4D3.tag=9;_tmp4D3.f1=({struct Cyc_Absyn_FnInfo _tmp4D4;_tmp4D4.tvars=0;_tmp4D4.effect=_tmp4E1;_tmp4D4.ret_tqual=_tmp4E0;_tmp4D4.ret_typ=_tmp4DF;_tmp4D4.args=_tmp4DE;_tmp4D4.c_varargs=_tmp4DD;_tmp4D4.cyc_varargs=_tmp4DC;_tmp4D4.rgn_po=0;_tmp4D4.attributes=_tmp4DA;_tmp4D4.requires_clause=_tmp4D9;_tmp4D4.requires_relns=_tmp4D8;_tmp4D4.ensures_clause=_tmp4D7;_tmp4D4.ensures_relns=_tmp4D6;_tmp4D4;});_tmp4D3;});_tmp4D2;}));
# 2657
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CB=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4D0;_tmp4D0.tag=5;_tmp4D0.f1=({struct Cyc_Absyn_PtrInfo _tmp4D1;_tmp4D1.elt_typ=ftyp;_tmp4D1.elt_tq=_tmp4E7;_tmp4D1.ptr_atts=({(_tmp4D1.ptr_atts).rgn=_tmp4E6;(_tmp4D1.ptr_atts).nullable=_tmp4E5;(_tmp4D1.ptr_atts).bounds=_tmp4E4;(_tmp4D1.ptr_atts).zero_term=_tmp4E3;(_tmp4D1.ptr_atts).ptrloc=0;_tmp4D1.ptr_atts;});_tmp4D1;});_tmp4D0;});_tmp4CF;});
# 2659
struct Cyc_Absyn_Exp*_tmp4CC=Cyc_Absyn_copy_exp(e);
e->r=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD[0]=({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp4CE;_tmp4CE.tag=12;_tmp4CE.f1=_tmp4CC;_tmp4CE.f2=ts;_tmp4CE;});_tmp4CD;});
e->topt=(void*)_tmp4CB;};}
# 2643
;_pop_region(rgn);}
# 2664
goto _LL2AE;}else{_LL2B1: _LL2B2:
 goto _LL2AE;}_LL2AE:;}
# 2667
goto _LL2A9;}else{_LL2AC: _LL2AD:
 goto _LL2A9;}_LL2A9:;}
# 2670
goto _LL2A4;}_LL2A4:;}
# 2672
return(void*)_check_null(e->topt);}
# 2680
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2682
struct _RegionHandle _tmp4EA=_new_region("r");struct _RegionHandle*r=& _tmp4EA;_push_region(r);
{struct Cyc_List_List*_tmp4EB=0;
# 2685
{void*_tmp4EC=fn_exp->r;void*_tmp4ED=_tmp4EC;struct Cyc_List_List*_tmp501;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4ED)->tag == 9){_LL2B4: _tmp501=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4ED)->f2;_LL2B5:
# 2687
{void*_tmp4EE=e->r;void*_tmp4EF=_tmp4EE;struct Cyc_List_List*_tmp4FE;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4EF)->tag == 9){_LL2B9: _tmp4FE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4EF)->f2;_LL2BA: {
# 2689
struct Cyc_List_List*_tmp4F0=alias_arg_exps;
int _tmp4F1=0;
while(_tmp4F0 != 0){
while(_tmp4F1 != (int)_tmp4F0->hd){
if(_tmp501 == 0)
({void*_tmp4F2=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)((struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=1;_tmp4F6.f1=(unsigned long)((int)_tmp4F0->hd);({struct Cyc_Int_pa_PrintArg_struct _tmp4F5;_tmp4F5.tag=1;_tmp4F5.f1=(unsigned long)_tmp4F1;({void*_tmp4F3[2]={& _tmp4F5,& _tmp4F6};Cyc_aprintf(({const char*_tmp4F4="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp4F4,sizeof(char),36);}),_tag_dyneither(_tmp4F3,sizeof(void*),2));});});}),_tag_dyneither(_tmp4F2,sizeof(void*),0));});
# 2696
++ _tmp4F1;
_tmp501=_tmp501->tl;
_tmp4FE=((struct Cyc_List_List*)_check_null(_tmp4FE))->tl;}{
# 2701
struct _tuple11 _tmp4F7=Cyc_Tcutil_insert_alias((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp501))->hd,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4FE))->hd)->topt)));struct _tuple11 _tmp4F8=_tmp4F7;struct Cyc_Absyn_Decl*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;_LL2BE: _tmp4FB=_tmp4F8.f1;_tmp4FA=_tmp4F8.f2;_LL2BF:;
_tmp501->hd=(void*)_tmp4FA;
_tmp4EB=({struct Cyc_List_List*_tmp4F9=_region_malloc(r,sizeof(*_tmp4F9));_tmp4F9->hd=_tmp4FB;_tmp4F9->tl=_tmp4EB;_tmp4F9;});
_tmp4F0=_tmp4F0->tl;};}
# 2706
goto _LL2B8;}}else{_LL2BB: _LL2BC:
({void*_tmp4FC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4FD="not a function call!";_tag_dyneither(_tmp4FD,sizeof(char),21);}),_tag_dyneither(_tmp4FC,sizeof(void*),0));});}_LL2B8:;}
# 2709
goto _LL2B3;}else{_LL2B6: _LL2B7:
({void*_tmp4FF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp500="not a function call!";_tag_dyneither(_tmp500,sizeof(char),21);}),_tag_dyneither(_tmp4FF,sizeof(void*),0));});}_LL2B3:;}
# 2714
while(_tmp4EB != 0){
struct Cyc_Absyn_Decl*_tmp502=(struct Cyc_Absyn_Decl*)_tmp4EB->hd;
fn_exp=Cyc_Absyn_stmt_exp(Cyc_Absyn_decl_stmt(_tmp502,Cyc_Absyn_exp_stmt(fn_exp,e->loc),e->loc),e->loc);
_tmp4EB=_tmp4EB->tl;}
# 2721
e->topt=0;
e->r=fn_exp->r;}
# 2683
;_pop_region(r);}
# 2728
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2732
{void*_tmp503=e->r;void*_tmp504=_tmp503;void*_tmp570;struct Cyc_Absyn_Exp*_tmp56F;struct _dyneither_ptr*_tmp56E;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Exp*_tmp56C;int*_tmp56B;struct Cyc_Absyn_Exp**_tmp56A;void***_tmp569;struct Cyc_Absyn_Exp**_tmp568;int*_tmp567;void*_tmp566;struct Cyc_Absyn_Enumfield*_tmp565;struct Cyc_Absyn_Enumdecl*_tmp564;struct Cyc_Absyn_Enumfield*_tmp563;struct Cyc_List_List*_tmp562;struct Cyc_Absyn_Datatypedecl*_tmp561;struct Cyc_Absyn_Datatypefield*_tmp560;void*_tmp55F;struct Cyc_List_List*_tmp55E;struct _tuple1**_tmp55D;struct Cyc_List_List**_tmp55C;struct Cyc_List_List*_tmp55B;struct Cyc_Absyn_Aggrdecl**_tmp55A;struct Cyc_Absyn_Exp*_tmp559;void*_tmp558;int*_tmp557;struct Cyc_Absyn_Vardecl*_tmp556;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp554;int*_tmp553;struct Cyc_Absyn_Stmt*_tmp552;struct Cyc_List_List*_tmp551;struct _tuple9*_tmp550;struct Cyc_List_List*_tmp54F;struct Cyc_List_List*_tmp54E;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54C;struct Cyc_Absyn_Exp*_tmp54B;struct _dyneither_ptr*_tmp54A;int*_tmp549;int*_tmp548;struct Cyc_Absyn_Exp*_tmp547;struct _dyneither_ptr*_tmp546;int*_tmp545;int*_tmp544;struct Cyc_Absyn_Exp*_tmp543;void*_tmp542;struct Cyc_List_List*_tmp541;void*_tmp540;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Exp**_tmp53E;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp53C;void*_tmp53B;struct Cyc_Absyn_Exp*_tmp53A;enum Cyc_Absyn_Coercion*_tmp539;struct Cyc_Absyn_Exp*_tmp538;struct Cyc_List_List*_tmp537;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp534;struct Cyc_Absyn_Exp*_tmp533;struct Cyc_Absyn_Exp*_tmp532;struct Cyc_Absyn_Exp*_tmp531;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Core_Opt*_tmp52B;struct Cyc_Absyn_Exp*_tmp52A;struct Cyc_Absyn_Exp*_tmp529;enum Cyc_Absyn_Incrementor _tmp528;enum Cyc_Absyn_Primop _tmp527;struct Cyc_List_List*_tmp526;void**_tmp525;union Cyc_Absyn_Cnst*_tmp524;struct Cyc_Core_Opt*_tmp523;struct Cyc_List_List*_tmp522;struct Cyc_Absyn_Exp*_tmp521;struct Cyc_List_List*_tmp520;struct Cyc_Absyn_VarargCallInfo**_tmp51F;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_List_List*_tmp51D;struct Cyc_Absyn_VarargCallInfo**_tmp51C;int*_tmp51B;struct Cyc_Absyn_Exp*_tmp51A;switch(*((int*)_tmp504)){case 11: _LL2C1: _tmp51A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2C2:
# 2737
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp51A);
return;case 9: _LL2C3: _tmp51E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp51D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp51C=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_tmp51B=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp504)->f4;if(!(*_tmp51B)){_LL2C4:
# 2741
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp505=_cycalloc(sizeof(*_tmp505));_tmp505[0]=({struct Cyc_Core_Impossible_exn_struct _tmp506;_tmp506.tag=Cyc_Core_Impossible;_tmp506.f1=({const char*_tmp507="unresolved function in tcExpNoInst";_tag_dyneither(_tmp507,sizeof(char),35);});_tmp506;});_tmp505;}));}else{_LL2D9: _tmp521=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp520=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp51F=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_LL2DA: {
# 2772
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp508;_push_handler(& _tmp508);{int _tmp50A=0;if(setjmp(_tmp508.handler))_tmp50A=1;if(!_tmp50A){
fn_exp=Cyc_Tcutil_deep_copy_exp(0,e);;_pop_handler();}else{void*_tmp509=(void*)_exn_thrown;void*_tmp50B=_tmp509;void*_tmp50D;struct _dyneither_ptr _tmp50C;if(((struct Cyc_Core_Failure_exn_struct*)_tmp50B)->tag == Cyc_Core_Failure){_LL314: _tmp50C=((struct Cyc_Core_Failure_exn_struct*)_tmp50B)->f1;_LL315:
# 2779
 ok=0;
fn_exp=e;
goto _LL313;}else{_LL316: _tmp50D=_tmp50B;_LL317:(int)_rethrow(_tmp50D);}_LL313:;}};}
# 2783
t=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp521,_tmp520,_tmp51F,& alias_arg_exps);
if(alias_arg_exps != 0  && ok){
alias_arg_exps=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2790
goto _LL2C0;}}case 35: _LL2C5: _tmp523=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp522=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2C6:
# 2745
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp522);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0: _LL2C7: _tmp524=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2C8:
# 2750
 t=Cyc_Tcexp_tcConst(te,loc,topt,_tmp524,e);goto _LL2C0;case 1: _LL2C9: _tmp525=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2CA:
# 2752
 t=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp525);goto _LL2C0;case 2: _LL2CB: _tmp527=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp526=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2CC:
# 2754
 t=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp527,_tmp526);goto _LL2C0;case 4: _LL2CD: _tmp529=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp528=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2CE:
# 2756
 t=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp529,_tmp528);goto _LL2C0;case 3: _LL2CF: _tmp52C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp52B=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp52A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_LL2D0:
# 2758
 t=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp52C,_tmp52B,_tmp52A);goto _LL2C0;case 5: _LL2D1: _tmp52F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp52E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp52D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_LL2D2:
# 2760
 t=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp52F,_tmp52E,_tmp52D);goto _LL2C0;case 6: _LL2D3: _tmp531=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp530=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2D4:
# 2762
 t=Cyc_Tcexp_tcAnd(te,loc,_tmp531,_tmp530);goto _LL2C0;case 7: _LL2D5: _tmp533=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp532=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2D6:
# 2764
 t=Cyc_Tcexp_tcOr(te,loc,_tmp533,_tmp532);goto _LL2C0;case 8: _LL2D7: _tmp535=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp534=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2D8:
# 2766
 t=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp535,_tmp534);goto _LL2C0;case 10: _LL2DB: _tmp536=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2DC:
# 2792
 t=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp536);goto _LL2C0;case 12: _LL2DD: _tmp538=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp537=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2DE:
# 2794
 t=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp538,_tmp537);goto _LL2C0;case 13: _LL2DF: _tmp53B=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp53A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp539=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp504)->f4;_LL2E0:
# 2796
 t=Cyc_Tcexp_tcCast(te,loc,topt,_tmp53B,_tmp53A,_tmp539);goto _LL2C0;case 14: _LL2E1: _tmp53C=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2E2:
# 2798
 t=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp53C);goto _LL2C0;case 15: _LL2E3: _tmp53E=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp53D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2E4:
# 2800
 t=Cyc_Tcexp_tcNew(te,loc,topt,_tmp53E,e,_tmp53D);goto _LL2C0;case 17: _LL2E5: _tmp53F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2E6: {
# 2802
void*_tmp50E=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp53F);
t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp50E);goto _LL2C0;}case 16: _LL2E7: _tmp540=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2E8:
# 2805
 t=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp540);goto _LL2C0;case 18: _LL2E9: _tmp542=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp541=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2EA:
# 2807
 t=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp542,_tmp541);goto _LL2C0;case 19: _LL2EB: _tmp543=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2EC:
# 2809
 t=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp543);goto _LL2C0;case 20: _LL2ED: _tmp547=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp546=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp545=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_tmp544=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp504)->f4;_LL2EE:
# 2811
 t=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp547,_tmp546,_tmp545,_tmp544);goto _LL2C0;case 21: _LL2EF: _tmp54B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp54A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp549=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_tmp548=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp504)->f4;_LL2F0:
# 2813
 t=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp54B,_tmp54A,_tmp549,_tmp548);goto _LL2C0;case 22: _LL2F1: _tmp54D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp54C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2F2:
# 2815
 t=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp54D,_tmp54C);goto _LL2C0;case 23: _LL2F3: _tmp54E=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2F4:
# 2817
 t=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp54E);goto _LL2C0;case 24: _LL2F5: _tmp550=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp54F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL2F6:
# 2819
 t=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp550,_tmp54F);goto _LL2C0;case 25: _LL2F7: _tmp551=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2F8: {
# 2823
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp50F=Cyc_Tcutil_compress(*topt);void*_tmp510=_tmp50F;void**_tmp513;struct Cyc_Absyn_Tqual*_tmp512;union Cyc_Absyn_Constraint*_tmp511;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510)->tag == 8){_LL319: _tmp513=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510)->f1).elt_type;_tmp512=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510)->f1).tq;_tmp511=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp510)->f1).zero_term;_LL31A:
# 2829
 elt_topt=_tmp513;
elt_tqopt=_tmp512;
zero_term=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp511);
goto _LL318;}else{_LL31B: _LL31C:
 goto _LL318;}_LL318:;}
# 2836
t=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp551);goto _LL2C0;}case 36: _LL2F9: _tmp552=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL2FA:
# 2838
 t=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp552);goto _LL2C0;case 26: _LL2FB: _tmp556=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp555=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp554=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_tmp553=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp504)->f4;_LL2FC:
# 2840
 t=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp556,_tmp555,_tmp554,_tmp553);goto _LL2C0;case 27: _LL2FD: _tmp559=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp558=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp557=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_LL2FE:
# 2842
 t=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp559,_tmp558,_tmp557);goto _LL2C0;case 28: _LL2FF: _tmp55D=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp55C=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp55B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_tmp55A=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp504)->f4;_LL300:
# 2844
 t=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp55D,_tmp55C,_tmp55B,_tmp55A);goto _LL2C0;case 29: _LL301: _tmp55F=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp55E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL302:
# 2846
 t=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp55F,_tmp55E);goto _LL2C0;case 30: _LL303: _tmp562=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp561=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_tmp560=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp504)->f3;_LL304:
# 2848
 t=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp562,_tmp561,_tmp560);goto _LL2C0;case 31: _LL305: _tmp564=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp563=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL306:
# 2850
 t=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp515;_tmp515.tag=13;_tmp515.f1=_tmp564->name;_tmp515.f2=_tmp564;_tmp515;});_tmp514;});goto _LL2C0;case 32: _LL307: _tmp566=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp565=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL308:
# 2852
 t=_tmp566;goto _LL2C0;case 33: _LL309: _tmp56B=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp504)->f1).is_calloc;_tmp56A=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp504)->f1).rgn;_tmp569=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp504)->f1).elt_type;_tmp568=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp504)->f1).num_elts;_tmp567=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp504)->f1).fat_result;_LL30A:
# 2854
 t=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp56A,_tmp569,_tmp568,_tmp56B,_tmp567);goto _LL2C0;case 34: _LL30B: _tmp56D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp56C=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL30C:
# 2856
 t=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp56D,_tmp56C);goto _LL2C0;case 37: _LL30D: _tmp56F=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_tmp56E=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp504)->f2;_LL30E:
# 2858
 t=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp56F,_tmp56E);goto _LL2C0;case 38: _LL30F: _tmp570=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp504)->f1;_LL310:
# 2860
 if(!te->allow_valueof)
({void*_tmp516=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp517="valueof(-) can only occur within types";_tag_dyneither(_tmp517,sizeof(char),39);}),_tag_dyneither(_tmp516,sizeof(void*),0));});
# 2868
t=Cyc_Absyn_sint_typ;
goto _LL2C0;default: _LL311: _LL312:
# 2871
({void*_tmp518=0;Cyc_Tcutil_terr(e->loc,({const char*_tmp519="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp519,sizeof(char),50);}),_tag_dyneither(_tmp518,sizeof(void*),0));});
t=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}_LL2C0:;}
# 2874
e->topt=t;}
