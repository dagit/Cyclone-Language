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
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct Cyc_Tcenv_Tenv*);
# 96
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 112
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_notreadctxt(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_notreadctxt(struct Cyc_Tcenv_Tenv*);
int Cyc_Tcenv_in_notreadctxt(struct Cyc_Tcenv_Tenv*);
# 117
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_lhs(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_lhs(struct Cyc_Tcenv_Tenv*);
# 125
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
# 132
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 136
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 138
void Cyc_Tcenv_check_effect_accessible(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*eff);
# 143
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
({void*_tmp0=0;({unsigned int _tmp518=loc;struct _dyneither_ptr _tmp517=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,msg,ap);Cyc_Tcutil_terr(_tmp518,_tmp517,_tag_dyneither(_tmp0,sizeof(void*),0));});});
if(topt == 0)
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));else{
# 54
return*topt;}}
# 63
static void Cyc_Tcexp_resolve_unresolved_mem(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*des){
# 67
if(topt == 0){
# 69
({void*_tmp51A=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp1=_cycalloc(sizeof(*_tmp1));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp519=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp2;_tmp2.tag=25;_tmp2.f1=des;_tmp2;});_tmp1[0]=_tmp519;});_tmp1;});e->r=_tmp51A;});
return;}{
# 72
void*t=*topt;
void*_tmp3=Cyc_Tcutil_compress(t);void*_tmp4=_tmp3;void*_tmp13;struct Cyc_Absyn_Tqual _tmp12;union Cyc_Absyn_AggrInfoU _tmp11;switch(*((int*)_tmp4)){case 11: _LL1: _tmp11=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1).aggr_info;_LL2:
# 75
{union Cyc_Absyn_AggrInfoU _tmp5=_tmp11;struct Cyc_Absyn_Aggrdecl*_tmpA;if((_tmp5.UnknownAggr).tag == 1){_LLA: _LLB:
({void*_tmp6=0;({struct _dyneither_ptr _tmp51B=({const char*_tmp7="struct type not properly set";_tag_dyneither(_tmp7,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp51B,_tag_dyneither(_tmp6,sizeof(void*),0));});});}else{_LLC: _tmpA=*(_tmp5.KnownAggr).val;_LLD:
({void*_tmp51D=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp8=_cycalloc(sizeof(*_tmp8));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp51C=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp9;_tmp9.tag=28;_tmp9.f1=_tmpA->name;_tmp9.f2=0;_tmp9.f3=des;_tmp9.f4=_tmpA;_tmp9;});_tmp8[0]=_tmp51C;});_tmp8;});e->r=_tmp51D;});}_LL9:;}
# 79
goto _LL0;case 8: _LL3: _tmp13=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).elt_type;_tmp12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4)->f1).tq;_LL4:
({void*_tmp51F=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpB=_cycalloc(sizeof(*_tmpB));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp51E=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpC;_tmpC.tag=25;_tmpC.f1=des;_tmpC;});_tmpB[0]=_tmp51E;});_tmpB;});e->r=_tmp51F;});goto _LL0;case 12: _LL5: _LL6:
({void*_tmp521=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpD=_cycalloc(sizeof(*_tmpD));({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp520=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpE;_tmpE.tag=29;_tmpE.f1=t;_tmpE.f2=des;_tmpE;});_tmpD[0]=_tmp520;});_tmpD;});e->r=_tmp521;});goto _LL0;default: _LL7: _LL8:
({void*_tmp523=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF=_cycalloc(sizeof(*_tmpF));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp522=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp10;_tmp10.tag=25;_tmp10.f1=des;_tmp10;});_tmpF[0]=_tmp522;});_tmpF;});e->r=_tmp523;});goto _LL0;}_LL0:;};}
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
({void*_tmp16=0;({unsigned int _tmp525=e->loc;struct _dyneither_ptr _tmp524=({const char*_tmp17="assignment in test";_tag_dyneither(_tmp17,sizeof(char),19);});Cyc_Tcutil_warn(_tmp525,_tmp524,_tag_dyneither(_tmp16,sizeof(void*),0));});});goto _LLE;}else{goto _LL11;}}else{_LL11: _LL12:
 goto _LLE;}_LLE:;}
# 107
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part){
Cyc_Tcexp_check_contains_assign(e);
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_sint_typ,e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp1B;_tmp1B.tag=0;({struct _dyneither_ptr _tmp526=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp1B.f1=_tmp526;});({struct Cyc_String_pa_PrintArg_struct _tmp1A;_tmp1A.tag=0;_tmp1A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg_part);({void*_tmp18[2]={& _tmp1A,& _tmp1B};({unsigned int _tmp528=e->loc;struct _dyneither_ptr _tmp527=({const char*_tmp19="test of %s has type %s instead of integral or pointer type";_tag_dyneither(_tmp19,sizeof(char),59);});Cyc_Tcutil_terr(_tmp528,_tmp527,_tag_dyneither(_tmp18,sizeof(void*),2));});});});});
# 116
{void*_tmp1C=e->r;void*_tmp1D=_tmp1C;enum Cyc_Absyn_Primop _tmp26;struct Cyc_List_List*_tmp25;if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->tag == 2){_LL14: _tmp26=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f1;_tmp25=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp1D)->f2;_LL15:
# 118
 if(_tmp26 == Cyc_Absyn_Eq  || _tmp26 == Cyc_Absyn_Neq){
struct _tuple0 _tmp1E=({struct _tuple0 _tmp24;_tmp24.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25))->hd)->topt);_tmp24.f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp25->tl))->hd)->topt);_tmp24;});struct _tuple0 _tmp1F=_tmp1E;void*_tmp23;void*_tmp22;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->tag == 15){_LL19: _tmp23=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f1)->f1;_tmp22=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1F.f2)->f1;_LL1A:
# 121
 return({struct Cyc_Tcexp_TestEnv _tmp20;({struct _tuple0*_tmp529=({struct _tuple0*_tmp21=_cycalloc(sizeof(*_tmp21));_tmp21->f1=_tmp23;_tmp21->f2=_tmp22;_tmp21;});_tmp20.eq=_tmp529;});_tmp20.isTrue=_tmp26 == Cyc_Absyn_Eq;_tmp20;});}else{goto _LL1B;}}else{_LL1B: _LL1C:
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
({void*_tmp52A=Cyc_Absyn_wchar_typ();t=_tmp52A;});goto _LL1D;case 4: _LL26: _tmp59=((_tmp29.Short_c).val).f1;_LL27:
# 165
 t=_tmp59 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;goto _LL1D;case 6: _LL28: _tmp5A=((_tmp29.LongLong_c).val).f1;_LL29:
# 167
 t=_tmp5A == Cyc_Absyn_Unsigned?Cyc_Absyn_ulonglong_typ: Cyc_Absyn_slonglong_typ;goto _LL1D;case 7: _LL2A: _tmp5B=((_tmp29.Float_c).val).f2;_LL2B:
({void*_tmp52B=Cyc_Absyn_float_typ(_tmp5B);t=_tmp52B;});goto _LL1D;case 5: _LL2C: _tmp5D=((_tmp29.Int_c).val).f1;_tmp5C=((_tmp29.Int_c).val).f2;_LL2D:
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
({union Cyc_Absyn_Cnst _tmp52C=Cyc_Absyn_Char_c(_tmp37,(char)_tmp5C);*c=_tmp52C;});
goto _LL34;case Cyc_Absyn_Short_sz: _LL37: _tmp38=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp2B)->f1;_LL38:
# 187
 t=_tmp38 == Cyc_Absyn_Unsigned?Cyc_Absyn_ushort_typ: Cyc_Absyn_sshort_typ;
({union Cyc_Absyn_Cnst _tmp52D=Cyc_Absyn_Short_c(_tmp38,(short)_tmp5C);*c=_tmp52D;});
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
({void*_tmp531=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp2E=_cycalloc(sizeof(*_tmp2E));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp530=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp2F;_tmp2F.tag=5;({struct Cyc_Absyn_PtrInfo _tmp52F=({struct Cyc_Absyn_PtrInfo _tmp30;_tmp30.elt_typ=_tmp40;_tmp30.elt_tq=_tmp3F;({struct Cyc_Absyn_PtrAtts _tmp52E=({(_tmp30.ptr_atts).rgn=_tmp3E;(_tmp30.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp30.ptr_atts).bounds=_tmp3C;(_tmp30.ptr_atts).zero_term=_tmp3B;(_tmp30.ptr_atts).ptrloc=0;_tmp30.ptr_atts;});_tmp30.ptr_atts=_tmp52E;});_tmp30;});_tmp2F.f1=_tmp52F;});_tmp2F;});_tmp2E[0]=_tmp530;});_tmp2E;});t=_tmp531;});
# 204
goto _LL34;};}}else{goto _LL41;}case 19: _LL3F: _tmp41=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp2B)->f1;_LL40: {
# 206
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp32=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp35=_cycalloc(sizeof(*_tmp35));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp533=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp36;_tmp36.tag=18;({struct Cyc_Absyn_Exp*_tmp532=Cyc_Absyn_uint_exp((unsigned int)_tmp5C,0);_tmp36.f1=_tmp532;});_tmp36;});_tmp35[0]=_tmp533;});_tmp35;});
# 213
({void*_tmp535=(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp33=_cycalloc(sizeof(*_tmp33));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp534=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp34;_tmp34.tag=19;_tmp34.f1=(void*)_tmp32;_tmp34;});_tmp33[0]=_tmp534;});_tmp33;});t=_tmp535;});
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
({int _tmp536=Cyc_Tcexp_wchar_numelts(_tmp5F);string_numelts=_tmp536;});
({void*_tmp537=Cyc_Absyn_wchar_typ();string_elt_typ=_tmp537;});}{
# 228
struct Cyc_Absyn_Exp*elen=({union Cyc_Absyn_Cnst _tmp538=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,string_numelts);Cyc_Absyn_const_exp(_tmp538,loc);});
elen->topt=Cyc_Absyn_uint_typ;{
# 233
struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp42=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp46=_cycalloc(sizeof(*_tmp46));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp539=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp47;_tmp47.tag=1;_tmp47.f1=elen;_tmp47;});_tmp46[0]=_tmp539;});_tmp46;});
({void*_tmp53D=({void*_tmp53C=string_elt_typ;struct Cyc_Absyn_Tqual _tmp53B=Cyc_Absyn_const_tqual(0);void*_tmp53A=(void*)_tmp42;Cyc_Absyn_atb_typ(_tmp53C,(void*)& Cyc_Absyn_HeapRgn_val,_tmp53B,_tmp53A,Cyc_Absyn_true_conref);});t=_tmp53D;});
# 236
if(topt != 0){
void*_tmp43=Cyc_Tcutil_compress(*topt);void*_tmp44=_tmp43;struct Cyc_Absyn_Tqual _tmp45;switch(*((int*)_tmp44)){case 8: _LL4B: _tmp45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44)->f1).tq;_LL4C:
# 241
 return({void*_tmp540=string_elt_typ;struct Cyc_Absyn_Tqual _tmp53F=_tmp45;struct Cyc_Absyn_Exp*_tmp53E=elen;Cyc_Absyn_array_typ(_tmp540,_tmp53F,_tmp53E,((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)(),0);});case 5: _LL4D: _LL4E:
# 243
 if(!Cyc_Tcutil_unify(*topt,t) && Cyc_Tcutil_silent_castable(te,loc,t,*topt)){
e->topt=t;
Cyc_Tcutil_unchecked_cast(te,e,*topt,Cyc_Absyn_Other_coercion);
t=*topt;}else{
# 249
({void*_tmp545=({void*_tmp544=string_elt_typ;void*_tmp543=({Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,Cyc_Tcenv_lookup_opt_type_vars(te));});struct Cyc_Absyn_Tqual _tmp542=
Cyc_Absyn_const_tqual(0);
# 249
void*_tmp541=(void*)_tmp42;Cyc_Absyn_atb_typ(_tmp544,_tmp543,_tmp542,_tmp541,Cyc_Absyn_true_conref);});t=_tmp545;});
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
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4A=_cycalloc(sizeof(*_tmp4A));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp548=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4B;_tmp4B.tag=5;({struct Cyc_Absyn_PtrInfo _tmp547=({struct Cyc_Absyn_PtrInfo _tmp4C;_tmp4C.elt_typ=_tmp52;_tmp4C.elt_tq=_tmp51;({struct Cyc_Absyn_PtrAtts _tmp546=({(_tmp4C.ptr_atts).rgn=_tmp50;(_tmp4C.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4C.ptr_atts).bounds=_tmp4E;(_tmp4C.ptr_atts).zero_term=_tmp4D;(_tmp4C.ptr_atts).ptrloc=0;_tmp4C.ptr_atts;});_tmp4C.ptr_atts=_tmp546;});_tmp4C;});_tmp4B.f1=_tmp547;});_tmp4B;});_tmp4A[0]=_tmp548;});_tmp4A;});}else{_LL54: _LL55:
# 270
 goto _LL51;}_LL51:;}{
# 273
struct Cyc_List_List*_tmp53=Cyc_Tcenv_lookup_type_vars(te);
({void*_tmp551=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp54=_cycalloc(sizeof(*_tmp54));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp550=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp55;_tmp55.tag=5;({struct Cyc_Absyn_PtrInfo _tmp54F=({struct Cyc_Absyn_PtrInfo _tmp56;({void*_tmp549=({Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,({struct Cyc_Core_Opt*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->v=_tmp53;_tmp58;}));});_tmp56.elt_typ=_tmp549;});({struct Cyc_Absyn_Tqual _tmp54A=
Cyc_Absyn_empty_tqual(0);_tmp56.elt_tq=_tmp54A;});({struct Cyc_Absyn_PtrAtts _tmp54E=({({void*_tmp54B=({
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->v=_tmp53;_tmp57;}));});(_tmp56.ptr_atts).rgn=_tmp54B;});(_tmp56.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmp54C=
# 278
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp56.ptr_atts).bounds=_tmp54C;});({union Cyc_Absyn_Constraint*_tmp54D=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp56.ptr_atts).zero_term=_tmp54D;});(_tmp56.ptr_atts).ptrloc=0;_tmp56.ptr_atts;});_tmp56.ptr_atts=_tmp54E;});_tmp56;});_tmp55.f1=_tmp54F;});_tmp55;});_tmp54[0]=_tmp550;});_tmp54;});
# 274
t=_tmp551;});
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
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp62=_cycalloc(sizeof(*_tmp62));({struct Cyc_Core_Impossible_exn_struct _tmp553=({struct Cyc_Core_Impossible_exn_struct _tmp63;_tmp63.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp552=({const char*_tmp64="unresolved binding in tcVar";_tag_dyneither(_tmp64,sizeof(char),28);});_tmp63.f1=_tmp552;});_tmp63;});_tmp62[0]=_tmp553;});_tmp62;}));case 1: _LL59: _tmp6B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp61)->f1;_LL5A:
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
({void*_tmp555=(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp67=_cycalloc(sizeof(*_tmp67));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp554=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp68;_tmp68.tag=38;_tmp68.f1=_tmp69;_tmp68;});_tmp67[0]=_tmp554;});_tmp67;});e->r=_tmp555;});
goto _LL63;}else{_LL66: _LL67:
 goto _LL63;}_LL63:;}
# 319
return _tmp6F->type;}_LL56:;}
# 323
static void Cyc_Tcexp_check_format_args(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*fmt,struct Cyc_Core_Opt*opt_args,int arg_cnt,struct Cyc_List_List**alias_arg_exps,struct _RegionHandle*temp,struct Cyc_List_List*(*type_getter)(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,struct _dyneither_ptr,unsigned int)){
# 332
struct Cyc_List_List*desc_types;
{void*_tmp70=fmt->r;void*_tmp71=_tmp70;struct _dyneither_ptr _tmp76;struct _dyneither_ptr _tmp75;switch(*((int*)_tmp71)){case 0: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp71)->f1).String_c).tag == 8){_LL69: _tmp75=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp71)->f1).String_c).val;_LL6A:
 _tmp76=_tmp75;goto _LL6C;}else{goto _LL6D;}case 13: if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->tag == 0){if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)((struct Cyc_Absyn_Exp*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->f1).String_c).tag == 8){_LL6B: _tmp76=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp71)->f2)->r)->f1).String_c).val;_LL6C:
# 336
({struct Cyc_List_List*_tmp556=type_getter(temp,te,(struct _dyneither_ptr)_tmp76,fmt->loc);desc_types=_tmp556;});goto _LL68;}else{goto _LL6D;}}else{goto _LL6D;}default: _LL6D: _LL6E:
# 340
 if(opt_args != 0){
struct Cyc_List_List*_tmp72=(struct Cyc_List_List*)opt_args->v;
for(0;_tmp72 != 0;_tmp72=_tmp72->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp72->hd);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp72->hd)->topt)) && !
Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp72->hd))
({void*_tmp73=0;({unsigned int _tmp558=((struct Cyc_Absyn_Exp*)_tmp72->hd)->loc;struct _dyneither_ptr _tmp557=({const char*_tmp74="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp74,sizeof(char),49);});Cyc_Tcutil_terr(_tmp558,_tmp557,_tag_dyneither(_tmp73,sizeof(void*),0));});});}}
# 349
return;}_LL68:;}
# 351
if(opt_args != 0){
struct Cyc_List_List*_tmp77=(struct Cyc_List_List*)opt_args->v;
# 354
for(0;desc_types != 0  && _tmp77 != 0;
(((desc_types=desc_types->tl,_tmp77=_tmp77->tl)),arg_cnt ++)){
int alias_coercion=0;
void*t=(void*)desc_types->hd;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)_tmp77->hd;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp7B;_tmp7B.tag=0;({struct _dyneither_ptr _tmp559=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp7B.f1=_tmp559;});({struct Cyc_String_pa_PrintArg_struct _tmp7A;_tmp7A.tag=0;({struct _dyneither_ptr _tmp55A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp7A.f1=_tmp55A;});({void*_tmp78[2]={& _tmp7A,& _tmp7B};({unsigned int _tmp55C=e->loc;struct _dyneither_ptr _tmp55B=({const char*_tmp79="descriptor has type %s but argument has type %s";_tag_dyneither(_tmp79,sizeof(char),48);});Cyc_Tcutil_terr(_tmp55C,_tmp55B,_tag_dyneither(_tmp78,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 365
if(alias_coercion)
({struct Cyc_List_List*_tmp55D=({struct Cyc_List_List*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->hd=(void*)arg_cnt;_tmp7C->tl=*alias_arg_exps;_tmp7C;});*alias_arg_exps=_tmp55D;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !
Cyc_Tcutil_is_noalias_path(e))
({void*_tmp7D=0;({unsigned int _tmp55F=((struct Cyc_Absyn_Exp*)_tmp77->hd)->loc;struct _dyneither_ptr _tmp55E=({const char*_tmp7E="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp7E,sizeof(char),49);});Cyc_Tcutil_terr(_tmp55F,_tmp55E,_tag_dyneither(_tmp7D,sizeof(void*),0));});});}
# 372
if(desc_types != 0)
({void*_tmp7F=0;({unsigned int _tmp561=fmt->loc;struct _dyneither_ptr _tmp560=({const char*_tmp80="too few arguments";_tag_dyneither(_tmp80,sizeof(char),18);});Cyc_Tcutil_terr(_tmp561,_tmp560,_tag_dyneither(_tmp7F,sizeof(void*),0));});});
if(_tmp77 != 0){
({void*_tmp81=0;({unsigned int _tmp563=((struct Cyc_Absyn_Exp*)_tmp77->hd)->loc;struct _dyneither_ptr _tmp562=({const char*_tmp82="too many arguments";_tag_dyneither(_tmp82,sizeof(char),19);});Cyc_Tcutil_terr(_tmp563,_tmp562,_tag_dyneither(_tmp81,sizeof(void*),0));});});
# 377
for(0;_tmp77 != 0;_tmp77=_tmp77->tl){
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp77->hd);}}}}
# 382
static void*Cyc_Tcexp_tcUnPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){
# 384
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Primop _tmp83=p;switch(_tmp83){case Cyc_Absyn_Plus: _LL70: _LL71:
 goto _LL73;case Cyc_Absyn_Minus: _LL72: _LL73:
# 388
 if(!Cyc_Tcutil_is_numeric(e))
({struct Cyc_String_pa_PrintArg_struct _tmp86;_tmp86.tag=0;({struct _dyneither_ptr _tmp564=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp86.f1=_tmp564;});({void*_tmp84[1]={& _tmp86};({unsigned int _tmp566=loc;struct _dyneither_ptr _tmp565=({const char*_tmp85="expecting numeric type but found %s";_tag_dyneither(_tmp85,sizeof(char),36);});Cyc_Tcutil_terr(_tmp566,_tmp565,_tag_dyneither(_tmp84,sizeof(void*),1));});});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Not: _LL74: _LL75:
# 392
 Cyc_Tcexp_check_contains_assign(e);
if(!Cyc_Tcutil_coerce_to_bool(te,e))
({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0;({struct _dyneither_ptr _tmp567=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp89.f1=_tmp567;});({void*_tmp87[1]={& _tmp89};({unsigned int _tmp569=loc;struct _dyneither_ptr _tmp568=({const char*_tmp88="expecting integral or * type but found %s";_tag_dyneither(_tmp88,sizeof(char),42);});Cyc_Tcutil_terr(_tmp569,_tmp568,_tag_dyneither(_tmp87,sizeof(void*),1));});});});
return Cyc_Absyn_sint_typ;case Cyc_Absyn_Bitnot: _LL76: _LL77:
# 397
 if(!Cyc_Tcutil_is_integral(e))
({struct Cyc_String_pa_PrintArg_struct _tmp8C;_tmp8C.tag=0;({struct _dyneither_ptr _tmp56A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp8C.f1=_tmp56A;});({void*_tmp8A[1]={& _tmp8C};({unsigned int _tmp56C=loc;struct _dyneither_ptr _tmp56B=({const char*_tmp8B="expecting integral type but found %s";_tag_dyneither(_tmp8B,sizeof(char),37);});Cyc_Tcutil_terr(_tmp56C,_tmp56B,_tag_dyneither(_tmp8A,sizeof(void*),1));});});});
return(void*)_check_null(e->topt);case Cyc_Absyn_Numelts: _LL78: _LL79:
# 401
{void*_tmp8D=t;union Cyc_Absyn_Constraint*_tmp96;switch(*((int*)_tmp8D)){case 8: _LL7D: _LL7E:
 goto _LL7C;case 5: _LL7F: _tmp96=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp8D)->f1).ptr_atts).bounds;_LL80:
# 404
{void*_tmp8E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp96);void*_tmp8F=_tmp8E;struct Cyc_Absyn_Exp*_tmp92;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp8F)->tag == 0){_LL84: _LL85:
 goto _LL83;}else{_LL86: _tmp92=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp8F)->f1;_LL87:
# 407
 if(!Cyc_Evexp_c_can_eval(_tmp92) && !((unsigned int)Cyc_Tcenv_allow_valueof))
({void*_tmp90=0;({unsigned int _tmp56E=loc;struct _dyneither_ptr _tmp56D=({const char*_tmp91="cannot apply numelts to a pointer with abstract bounds";_tag_dyneither(_tmp91,sizeof(char),55);});Cyc_Tcutil_terr(_tmp56E,_tmp56D,_tag_dyneither(_tmp90,sizeof(void*),0));});});
goto _LL83;}_LL83:;}
# 411
goto _LL7C;default: _LL81: _LL82:
# 413
({struct Cyc_String_pa_PrintArg_struct _tmp95;_tmp95.tag=0;({struct _dyneither_ptr _tmp56F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp95.f1=_tmp56F;});({void*_tmp93[1]={& _tmp95};({unsigned int _tmp571=loc;struct _dyneither_ptr _tmp570=({const char*_tmp94="numelts requires pointer or array type, not %s";_tag_dyneither(_tmp94,sizeof(char),47);});Cyc_Tcutil_terr(_tmp571,_tmp570,_tag_dyneither(_tmp93,sizeof(void*),1));});});});}_LL7C:;}
# 415
return Cyc_Absyn_uint_typ;default: _LL7A: _LL7B:
({void*_tmp97=0;({struct _dyneither_ptr _tmp572=({const char*_tmp98="Non-unary primop";_tag_dyneither(_tmp98,sizeof(char),17);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp572,_tag_dyneither(_tmp97,sizeof(void*),0));});});}_LL6F:;}
# 421
static void*Cyc_Tcexp_tcArithBinop(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,int(*checker)(struct Cyc_Absyn_Exp*)){
# 424
if(!checker(e1)){
({struct Cyc_String_pa_PrintArg_struct _tmp9B;_tmp9B.tag=0;({struct _dyneither_ptr _tmp573=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp9B.f1=_tmp573;});({void*_tmp99[1]={& _tmp9B};({unsigned int _tmp575=e1->loc;struct _dyneither_ptr _tmp574=({const char*_tmp9A="type %s cannot be used here";_tag_dyneither(_tmp9A,sizeof(char),28);});Cyc_Tcutil_terr(_tmp575,_tmp574,_tag_dyneither(_tmp99,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 428
if(!checker(e2)){
({struct Cyc_String_pa_PrintArg_struct _tmp9E;_tmp9E.tag=0;({struct _dyneither_ptr _tmp576=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp9E.f1=_tmp576;});({void*_tmp9C[1]={& _tmp9E};({unsigned int _tmp578=e2->loc;struct _dyneither_ptr _tmp577=({const char*_tmp9D="type %s cannot be used here";_tag_dyneither(_tmp9D,sizeof(char),28);});Cyc_Tcutil_terr(_tmp578,_tmp577,_tag_dyneither(_tmp9C,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 432
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
return Cyc_Tcutil_max_arithmetic_type(t1,t2);};}
# 437
static void*Cyc_Tcexp_tcPlus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
void*_tmp9F=t1;void*_tmpB5;struct Cyc_Absyn_Tqual _tmpB4;void*_tmpB3;union Cyc_Absyn_Constraint*_tmpB2;union Cyc_Absyn_Constraint*_tmpB1;union Cyc_Absyn_Constraint*_tmpB0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F)->tag == 5){_LL89: _tmpB5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F)->f1).elt_typ;_tmpB4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F)->f1).elt_tq;_tmpB3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F)->f1).ptr_atts).rgn;_tmpB2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F)->f1).ptr_atts).nullable;_tmpB1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F)->f1).ptr_atts).bounds;_tmpB0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F)->f1).ptr_atts).zero_term;_LL8A:
# 442
 if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmpB5),& Cyc_Tcutil_tmk);}))
({void*_tmpA0=0;({unsigned int _tmp57A=e1->loc;struct _dyneither_ptr _tmp579=({const char*_tmpA1="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpA1,sizeof(char),50);});Cyc_Tcutil_terr(_tmp57A,_tmp579,_tag_dyneither(_tmpA0,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpA2=0;({unsigned int _tmp57C=e1->loc;struct _dyneither_ptr _tmp57B=({const char*_tmpA3="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpA3,sizeof(char),54);});Cyc_Tcutil_terr(_tmp57C,_tmp57B,_tag_dyneither(_tmpA2,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2))
({struct Cyc_String_pa_PrintArg_struct _tmpA6;_tmpA6.tag=0;({struct _dyneither_ptr _tmp57D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmpA6.f1=_tmp57D;});({void*_tmpA4[1]={& _tmpA6};({unsigned int _tmp57F=e2->loc;struct _dyneither_ptr _tmp57E=({const char*_tmpA5="expecting int but found %s";_tag_dyneither(_tmpA5,sizeof(char),27);});Cyc_Tcutil_terr(_tmp57F,_tmp57E,_tag_dyneither(_tmpA4,sizeof(void*),1));});});});{
void*_tmpA7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB1);void*_tmpA8=_tmpA7;struct Cyc_Absyn_Exp*_tmpAF;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA8)->tag == 0){_LL8E: _LL8F:
 return t1;}else{_LL90: _tmpAF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA8)->f1;_LL91:
# 453
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB0))
({void*_tmpA9=0;({unsigned int _tmp581=e1->loc;struct _dyneither_ptr _tmp580=({const char*_tmpAA="pointer arithmetic on thin, zero-terminated pointer may be expensive.";_tag_dyneither(_tmpAA,sizeof(char),70);});Cyc_Tcutil_warn(_tmp581,_tmp580,_tag_dyneither(_tmpA9,sizeof(void*),0));});});{
# 461
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAB=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp584=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpAD;_tmpAD.tag=5;({struct Cyc_Absyn_PtrInfo _tmp583=({struct Cyc_Absyn_PtrInfo _tmpAE;_tmpAE.elt_typ=_tmpB5;_tmpAE.elt_tq=_tmpB4;({struct Cyc_Absyn_PtrAtts _tmp582=({(_tmpAE.ptr_atts).rgn=_tmpB3;(_tmpAE.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmpAE.ptr_atts).bounds=Cyc_Absyn_bounds_dyneither_conref;(_tmpAE.ptr_atts).zero_term=_tmpB0;(_tmpAE.ptr_atts).ptrloc=0;_tmpAE.ptr_atts;});_tmpAE.ptr_atts=_tmp582;});_tmpAE;});_tmpAD.f1=_tmp583;});_tmpAD;});_tmpAC[0]=_tmp584;});_tmpAC;});
# 465
Cyc_Tcutil_unchecked_cast(te,e1,(void*)_tmpAB,Cyc_Absyn_Other_coercion);
return(void*)_tmpAB;};}_LL8D:;};}else{_LL8B: _LL8C:
# 468
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}_LL88:;}
# 473
static void*Cyc_Tcexp_tcMinus(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
void*t1_elt=(void*)& Cyc_Absyn_VoidType_val;
void*t2_elt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& t1_elt)){
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t2,& t2_elt)){
if(!Cyc_Tcutil_unify(t1_elt,t2_elt)){
({struct Cyc_String_pa_PrintArg_struct _tmpB9;_tmpB9.tag=0;({struct _dyneither_ptr _tmp585=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 483
Cyc_Absynpp_typ2string(t2));_tmpB9.f1=_tmp585;});({struct Cyc_String_pa_PrintArg_struct _tmpB8;_tmpB8.tag=0;({struct _dyneither_ptr _tmp586=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpB8.f1=_tmp586;});({void*_tmpB6[2]={& _tmpB8,& _tmpB9};({unsigned int _tmp588=e1->loc;struct _dyneither_ptr _tmp587=({const char*_tmpB7="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpB7,sizeof(char),59);});Cyc_Tcutil_terr(_tmp588,_tmp587,_tag_dyneither(_tmpB6,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 486
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_is_pointer_type(t2)){
if(!({void*_tmp589=t1_elt;Cyc_Tcutil_unify(_tmp589,Cyc_Tcutil_pointer_elt_type(t2));})){
({struct Cyc_String_pa_PrintArg_struct _tmpBD;_tmpBD.tag=0;({struct _dyneither_ptr _tmp58A=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 491
Cyc_Absynpp_typ2string(t2));_tmpBD.f1=_tmp58A;});({struct Cyc_String_pa_PrintArg_struct _tmpBC;_tmpBC.tag=0;({struct _dyneither_ptr _tmp58B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpBC.f1=_tmp58B;});({void*_tmpBA[2]={& _tmpBC,& _tmpBD};({unsigned int _tmp58D=e1->loc;struct _dyneither_ptr _tmp58C=({const char*_tmpBB="pointer arithmetic on values of different types (%s != %s)";_tag_dyneither(_tmpBB,sizeof(char),59);});Cyc_Tcutil_terr(_tmp58D,_tmp58C,_tag_dyneither(_tmpBA,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 495
({void*_tmpBE=0;({unsigned int _tmp58F=e1->loc;struct _dyneither_ptr _tmp58E=({const char*_tmpBF="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpBF,sizeof(char),60);});Cyc_Tcutil_warn(_tmp58F,_tmp58E,_tag_dyneither(_tmpBE,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmp594=te;struct Cyc_Absyn_Exp*_tmp593=e1;Cyc_Tcutil_unchecked_cast(_tmp594,_tmp593,({void*_tmp592=t1_elt;void*_tmp591=Cyc_Tcutil_pointer_region(t1);struct Cyc_Absyn_Tqual _tmp590=
Cyc_Absyn_empty_tqual(0);
# 496
Cyc_Absyn_star_typ(_tmp592,_tmp591,_tmp590,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});
# 499
return Cyc_Absyn_sint_typ;}else{
# 501
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1_elt),& Cyc_Tcutil_tmk);}))
({void*_tmpC0=0;({unsigned int _tmp596=e1->loc;struct _dyneither_ptr _tmp595=({const char*_tmpC1="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmpC1,sizeof(char),50);});Cyc_Tcutil_terr(_tmp596,_tmp595,_tag_dyneither(_tmpC0,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t1,0))
({void*_tmpC2=0;({unsigned int _tmp598=e1->loc;struct _dyneither_ptr _tmp597=({const char*_tmpC3="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmpC3,sizeof(char),54);});Cyc_Tcutil_terr(_tmp598,_tmp597,_tag_dyneither(_tmpC2,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_sint_typ(te,e2)){
({struct Cyc_String_pa_PrintArg_struct _tmpC7;_tmpC7.tag=0;({struct _dyneither_ptr _tmp599=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpC7.f1=_tmp599;});({struct Cyc_String_pa_PrintArg_struct _tmpC6;_tmpC6.tag=0;({struct _dyneither_ptr _tmp59A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpC6.f1=_tmp59A;});({void*_tmpC4[2]={& _tmpC6,& _tmpC7};({unsigned int _tmp59C=e2->loc;struct _dyneither_ptr _tmp59B=({const char*_tmpC5="expecting either %s or int but found %s";_tag_dyneither(_tmpC5,sizeof(char),40);});Cyc_Tcutil_terr(_tmp59C,_tmp59B,_tag_dyneither(_tmpC4,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 510
return t1;}}}
# 514
if(Cyc_Tcutil_is_pointer_type(t1)){
if(Cyc_Tcutil_is_pointer_type(t2) && ({void*_tmp59D=Cyc_Tcutil_pointer_elt_type(t1);Cyc_Tcutil_unify(_tmp59D,
Cyc_Tcutil_pointer_elt_type(t2));})){
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
({void*_tmpC8=0;({unsigned int _tmp59F=e1->loc;struct _dyneither_ptr _tmp59E=({const char*_tmpC9="coercing fat pointer to thin pointer to support subtraction";_tag_dyneither(_tmpC9,sizeof(char),60);});Cyc_Tcutil_warn(_tmp59F,_tmp59E,_tag_dyneither(_tmpC8,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmp5A4=te;struct Cyc_Absyn_Exp*_tmp5A3=e2;Cyc_Tcutil_unchecked_cast(_tmp5A4,_tmp5A3,({void*_tmp5A2=Cyc_Tcutil_pointer_elt_type(t2);void*_tmp5A1=
Cyc_Tcutil_pointer_region(t2);
# 519
struct Cyc_Absyn_Tqual _tmp5A0=
# 521
Cyc_Absyn_empty_tqual(0);
# 519
Cyc_Absyn_star_typ(_tmp5A2,_tmp5A1,_tmp5A0,Cyc_Absyn_false_conref);}),Cyc_Absyn_Other_coercion);});}
# 524
({void*_tmpCA=0;({unsigned int _tmp5A6=e1->loc;struct _dyneither_ptr _tmp5A5=({const char*_tmpCB="thin pointer subtraction!";_tag_dyneither(_tmpCB,sizeof(char),26);});Cyc_Tcutil_warn(_tmp5A6,_tmp5A5,_tag_dyneither(_tmpCA,sizeof(void*),0));});});
return Cyc_Absyn_sint_typ;}
# 527
({void*_tmpCC=0;({unsigned int _tmp5A8=e1->loc;struct _dyneither_ptr _tmp5A7=({const char*_tmpCD="coercing thin pointer to integer to support subtraction";_tag_dyneither(_tmpCD,sizeof(char),56);});Cyc_Tcutil_warn(_tmp5A8,_tmp5A7,_tag_dyneither(_tmpCC,sizeof(void*),0));});});
Cyc_Tcutil_unchecked_cast(te,e1,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 530
if(Cyc_Tcutil_is_pointer_type(t2)){
({void*_tmpCE=0;({unsigned int _tmp5AA=e1->loc;struct _dyneither_ptr _tmp5A9=({const char*_tmpCF="coercing pointer to integer to support subtraction";_tag_dyneither(_tmpCF,sizeof(char),51);});Cyc_Tcutil_warn(_tmp5AA,_tmp5A9,_tag_dyneither(_tmpCE,sizeof(void*),0));});});
Cyc_Tcutil_unchecked_cast(te,e2,Cyc_Absyn_sint_typ,Cyc_Absyn_Other_coercion);}
# 535
return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);}
# 538
static void*Cyc_Tcexp_tcAnyBinop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
int e1_is_num=Cyc_Tcutil_is_numeric(e1);
int e2_is_num=Cyc_Tcutil_is_numeric(e2);
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(e1_is_num  && e2_is_num)
return Cyc_Absyn_sint_typ;else{
# 546
if(({(Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind;}) || ({
void*_tmp5AB=t1;Cyc_Tcutil_unify(_tmp5AB,({Cyc_Absyn_new_evar(& Cyc_Tcutil_bko,Cyc_Tcenv_lookup_opt_type_vars(te));}));})){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_sint_typ;else{
# 551
if(Cyc_Tcutil_silent_castable(te,loc,t2,t1)){
Cyc_Tcutil_unchecked_cast(te,e2,t1,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_silent_castable(te,loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e1,t2,Cyc_Absyn_Other_coercion);
return Cyc_Absyn_sint_typ;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e1) || Cyc_Tcutil_zero_to_null(te,t1,e2))
return Cyc_Absyn_sint_typ;else{
goto pointer_cmp;}}}}}else{
# 564
pointer_cmp: {
struct _tuple0 _tmpD0=({struct _tuple0 _tmpD4;({void*_tmp5AC=Cyc_Tcutil_compress(t1);_tmpD4.f1=_tmp5AC;});({void*_tmp5AD=Cyc_Tcutil_compress(t2);_tmpD4.f2=_tmp5AD;});_tmpD4;});struct _tuple0 _tmpD1=_tmpD0;void*_tmpD3;void*_tmpD2;switch(*((int*)_tmpD1.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f2)->tag == 5){_LL93: _tmpD3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f1)->f1).elt_typ;_tmpD2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpD1.f2)->f1).elt_typ;_LL94:
# 568
 if(Cyc_Tcutil_unify(_tmpD3,_tmpD2))return Cyc_Absyn_sint_typ;goto _LL92;}else{goto _LL97;}case 15: if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpD1.f2)->tag == 15){_LL95: _LL96:
# 570
 return Cyc_Absyn_sint_typ;}else{goto _LL97;}default: _LL97: _LL98:
 goto _LL92;}_LL92:;}
# 573
({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0;({struct _dyneither_ptr _tmp5AE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpD8.f1=_tmp5AE;});({struct Cyc_String_pa_PrintArg_struct _tmpD7;_tmpD7.tag=0;({struct _dyneither_ptr _tmp5AF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpD7.f1=_tmp5AF;});({void*_tmpD5[2]={& _tmpD7,& _tmpD8};({unsigned int _tmp5B1=loc;struct _dyneither_ptr _tmp5B0=({const char*_tmpD6="comparison not allowed between %s and %s";_tag_dyneither(_tmpD6,sizeof(char),41);});Cyc_Tcutil_terr(_tmp5B1,_tmp5B0,_tag_dyneither(_tmpD5,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}}}
# 580
static void*Cyc_Tcexp_tcEqPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 582
if(({void*_tmp5B2=Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);_tmp5B2 == Cyc_Absyn_sint_typ;}))
return Cyc_Absyn_sint_typ;{
# 588
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
{struct _tuple0 _tmpD9=({struct _tuple0 _tmpDD;_tmpDD.f1=t1;_tmpDD.f2=t2;_tmpDD;});struct _tuple0 _tmpDA=_tmpD9;void*_tmpDC;void*_tmpDB;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDA.f1)->tag == 15){if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDA.f2)->tag == 15){_LL9A: _tmpDC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDA.f1)->f1;_tmpDB=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmpDA.f2)->f1;_LL9B:
# 593
 return Cyc_Absyn_sint_typ;}else{goto _LL9C;}}else{_LL9C: _LL9D:
 goto _LL99;}_LL99:;}
# 596
({struct Cyc_String_pa_PrintArg_struct _tmpE1;_tmpE1.tag=0;({struct _dyneither_ptr _tmp5B3=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmpE1.f1=_tmp5B3;});({struct Cyc_String_pa_PrintArg_struct _tmpE0;_tmpE0.tag=0;({struct _dyneither_ptr _tmp5B4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmpE0.f1=_tmp5B4;});({void*_tmpDE[2]={& _tmpE0,& _tmpE1};({unsigned int _tmp5B6=loc;struct _dyneither_ptr _tmp5B5=({const char*_tmpDF="comparison not allowed between %s and %s";_tag_dyneither(_tmpDF,sizeof(char),41);});Cyc_Tcutil_terr(_tmp5B6,_tmp5B5,_tag_dyneither(_tmpDE,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));};}
# 604
static void*Cyc_Tcexp_tcBinPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 606
enum Cyc_Absyn_Primop _tmpE2=p;switch(_tmpE2){case Cyc_Absyn_Plus: _LL9F: _LLA0:
 return Cyc_Tcexp_tcPlus(te,e1,e2);case Cyc_Absyn_Minus: _LLA1: _LLA2:
 return Cyc_Tcexp_tcMinus(te,e1,e2);case Cyc_Absyn_Times: _LLA3: _LLA4:
# 610
 goto _LLA6;case Cyc_Absyn_Div: _LLA5: _LLA6:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_numeric);case Cyc_Absyn_Mod: _LLA7: _LLA8:
# 613
 goto _LLAA;case Cyc_Absyn_Bitand: _LLA9: _LLAA:
 goto _LLAC;case Cyc_Absyn_Bitor: _LLAB: _LLAC:
 goto _LLAE;case Cyc_Absyn_Bitxor: _LLAD: _LLAE:
 goto _LLB0;case Cyc_Absyn_Bitlshift: _LLAF: _LLB0:
 goto _LLB2;case Cyc_Absyn_Bitlrshift: _LLB1: _LLB2:
 goto _LLB4;case Cyc_Absyn_Bitarshift: _LLB3: _LLB4:
 return Cyc_Tcexp_tcArithBinop(te,e1,e2,Cyc_Tcutil_is_integral);case Cyc_Absyn_Eq: _LLB5: _LLB6:
# 623
 goto _LLB8;case Cyc_Absyn_Neq: _LLB7: _LLB8:
 return Cyc_Tcexp_tcEqPrimop(te,loc,e1,e2);case Cyc_Absyn_Gt: _LLB9: _LLBA:
# 626
 goto _LLBC;case Cyc_Absyn_Lt: _LLBB: _LLBC:
 goto _LLBE;case Cyc_Absyn_Gte: _LLBD: _LLBE:
 goto _LLC0;case Cyc_Absyn_Lte: _LLBF: _LLC0:
 return Cyc_Tcexp_tcAnyBinop(te,loc,e1,e2);default: _LLC1: _LLC2:
# 631
({void*_tmpE3=0;({struct _dyneither_ptr _tmp5B7=({const char*_tmpE4="bad binary primop";_tag_dyneither(_tmpE4,sizeof(char),18);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5B7,_tag_dyneither(_tmpE3,sizeof(void*),0));});});}_LL9E:;}
# 635
static void*Cyc_Tcexp_tcPrimop(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,enum Cyc_Absyn_Primop p,struct Cyc_List_List*es){
# 643
if(p == Cyc_Absyn_Minus  && ({((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1;})){
struct Cyc_Absyn_Exp*_tmpE5=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;
void*_tmpE6=Cyc_Tcexp_tcExp(te,topt,_tmpE5);
if(!Cyc_Tcutil_is_numeric(_tmpE5))
({struct Cyc_String_pa_PrintArg_struct _tmpE9;_tmpE9.tag=0;({struct _dyneither_ptr _tmp5B8=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpE6));_tmpE9.f1=_tmp5B8;});({void*_tmpE7[1]={& _tmpE9};({unsigned int _tmp5BA=_tmpE5->loc;struct _dyneither_ptr _tmp5B9=({const char*_tmpE8="expecting numeric type but found %s";_tag_dyneither(_tmpE8,sizeof(char),36);});Cyc_Tcutil_terr(_tmp5BA,_tmp5B9,_tag_dyneither(_tmpE7,sizeof(void*),1));});});});
return _tmpE6;}
# 650
Cyc_Tcexp_tcExpList(te,es);{
void*t;
{int _tmpEA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);int _tmpEB=_tmpEA;switch(_tmpEB){case 0: _LLC4: _LLC5:
 return({void*_tmpEC=0;({struct Cyc_Tcenv_Tenv*_tmp5BE=te;unsigned int _tmp5BD=loc;void**_tmp5BC=topt;struct _dyneither_ptr _tmp5BB=({const char*_tmpED="primitive operator has 0 arguments";_tag_dyneither(_tmpED,sizeof(char),35);});Cyc_Tcexp_expr_err(_tmp5BE,_tmp5BD,_tmp5BC,_tmp5BB,_tag_dyneither(_tmpEC,sizeof(void*),0));});});case 1: _LLC6: _LLC7:
({void*_tmp5BF=Cyc_Tcexp_tcUnPrimop(te,loc,topt,p,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd);t=_tmp5BF;});goto _LLC3;case 2: _LLC8: _LLC9:
({void*_tmp5C5=({struct Cyc_Tcenv_Tenv*_tmp5C4=te;unsigned int _tmp5C3=loc;void**_tmp5C2=topt;enum Cyc_Absyn_Primop _tmp5C1=p;struct Cyc_Absyn_Exp*_tmp5C0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;Cyc_Tcexp_tcBinPrimop(_tmp5C4,_tmp5C3,_tmp5C2,_tmp5C1,_tmp5C0,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd);});t=_tmp5C5;});goto _LLC3;default: _LLCA: _LLCB:
 return({void*_tmpEE=0;({struct Cyc_Tcenv_Tenv*_tmp5C9=te;unsigned int _tmp5C8=loc;void**_tmp5C7=topt;struct _dyneither_ptr _tmp5C6=({const char*_tmpEF="primitive operator has > 2 arguments";_tag_dyneither(_tmpEF,sizeof(char),37);});Cyc_Tcexp_expr_err(_tmp5C9,_tmp5C8,_tmp5C7,_tmp5C6,_tag_dyneither(_tmpEE,sizeof(void*),0));});});}_LLC3:;}
# 658
return t;};}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};
# 661
static int Cyc_Tcexp_check_writable_aggr(unsigned int loc,void*t){
void*_tmpF0=Cyc_Tcutil_compress(t);
void*_tmpF1=_tmpF0;struct Cyc_List_List*_tmp10D;void*_tmp10C;struct Cyc_Absyn_Tqual _tmp10B;struct Cyc_Absyn_Datatypefield*_tmp10A;struct Cyc_List_List*_tmp109;struct Cyc_Absyn_Aggrdecl*_tmp108;switch(*((int*)_tmpF1)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF1)->f1).aggr_info).KnownAggr).tag == 2){_LLCD: _tmp108=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpF1)->f1).aggr_info).KnownAggr).val;_LLCE:
# 665
 if(_tmp108->impl == 0){
({void*_tmpF2=0;({unsigned int _tmp5CB=loc;struct _dyneither_ptr _tmp5CA=({const char*_tmpF3="attempt to write an abstract aggregate";_tag_dyneither(_tmpF3,sizeof(char),39);});Cyc_Tcutil_terr(_tmp5CB,_tmp5CA,_tag_dyneither(_tmpF2,sizeof(void*),0));});});
return 0;}else{
# 669
_tmp109=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp108->impl))->fields;goto _LLD0;}}else{goto _LLD7;}case 12: _LLCF: _tmp109=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpF1)->f2;_LLD0:
# 671
 for(0;_tmp109 != 0;_tmp109=_tmp109->tl){
struct Cyc_Absyn_Aggrfield*_tmpF4=(struct Cyc_Absyn_Aggrfield*)_tmp109->hd;
if((_tmpF4->tq).real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpF7;_tmpF7.tag=0;_tmpF7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpF4->name);({void*_tmpF5[1]={& _tmpF7};({unsigned int _tmp5CD=loc;struct _dyneither_ptr _tmp5CC=({const char*_tmpF6="attempt to over-write an aggregate with const member %s";_tag_dyneither(_tmpF6,sizeof(char),56);});Cyc_Tcutil_terr(_tmp5CD,_tmp5CC,_tag_dyneither(_tmpF5,sizeof(void*),1));});});});
return 0;}
# 677
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpF4->type))return 0;}
# 679
return 1;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF1)->f1).field_info).KnownDatatypefield).tag == 2){_LLD1: _tmp10A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpF1)->f1).field_info).KnownDatatypefield).val).f2;_LLD2:
# 681
{struct Cyc_List_List*_tmpF8=_tmp10A->typs;for(0;_tmpF8 != 0;_tmpF8=_tmpF8->tl){
struct _tuple17*_tmpF9=(struct _tuple17*)_tmpF8->hd;struct _tuple17*_tmpFA=_tmpF9;struct Cyc_Absyn_Tqual _tmpFF;void*_tmpFE;_LLDA: _tmpFF=_tmpFA->f1;_tmpFE=_tmpFA->f2;_LLDB:;
if(_tmpFF.real_const){
({struct Cyc_String_pa_PrintArg_struct _tmpFD;_tmpFD.tag=0;({struct _dyneither_ptr _tmp5CE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp10A->name));_tmpFD.f1=_tmp5CE;});({void*_tmpFB[1]={& _tmpFD};({unsigned int _tmp5D0=loc;struct _dyneither_ptr _tmp5CF=({const char*_tmpFC="attempt to over-write a datatype field (%s) with a const member";_tag_dyneither(_tmpFC,sizeof(char),64);});Cyc_Tcutil_terr(_tmp5D0,_tmp5CF,_tag_dyneither(_tmpFB,sizeof(void*),1));});});});
return 0;}
# 687
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmpFE))return 0;}}
# 689
return 1;}else{goto _LLD7;}case 8: _LLD3: _tmp10C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF1)->f1).elt_type;_tmp10B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpF1)->f1).tq;_LLD4:
# 691
 if(_tmp10B.real_const){
({void*_tmp100=0;({unsigned int _tmp5D2=loc;struct _dyneither_ptr _tmp5D1=({const char*_tmp101="attempt to over-write a const array";_tag_dyneither(_tmp101,sizeof(char),36);});Cyc_Tcutil_terr(_tmp5D2,_tmp5D1,_tag_dyneither(_tmp100,sizeof(void*),0));});});
return 0;}
# 695
return Cyc_Tcexp_check_writable_aggr(loc,_tmp10C);case 10: _LLD5: _tmp10D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpF1)->f1;_LLD6:
# 697
 for(0;_tmp10D != 0;_tmp10D=_tmp10D->tl){
struct _tuple17*_tmp102=(struct _tuple17*)_tmp10D->hd;struct _tuple17*_tmp103=_tmp102;struct Cyc_Absyn_Tqual _tmp107;void*_tmp106;_LLDD: _tmp107=_tmp103->f1;_tmp106=_tmp103->f2;_LLDE:;
if(_tmp107.real_const){
({void*_tmp104=0;({unsigned int _tmp5D4=loc;struct _dyneither_ptr _tmp5D3=({const char*_tmp105="attempt to over-write a tuple field with a const member";_tag_dyneither(_tmp105,sizeof(char),56);});Cyc_Tcutil_terr(_tmp5D4,_tmp5D3,_tag_dyneither(_tmp104,sizeof(void*),0));});});
return 0;}
# 703
if(!Cyc_Tcexp_check_writable_aggr(loc,_tmp106))return 0;}
# 705
return 1;default: _LLD7: _LLD8:
 return 1;}_LLCC:;}
# 713
static void Cyc_Tcexp_check_writable(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 716
if(!Cyc_Tcexp_check_writable_aggr(e->loc,(void*)_check_null(e->topt)))return;
{void*_tmp10E=e->r;void*_tmp10F=_tmp10E;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;struct _dyneither_ptr*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct _dyneither_ptr*_tmp138;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Vardecl*_tmp135;struct Cyc_Absyn_Vardecl*_tmp134;struct Cyc_Absyn_Vardecl*_tmp133;struct Cyc_Absyn_Vardecl*_tmp132;switch(*((int*)_tmp10F)){case 1: switch(*((int*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10F)->f1)){case 3: _LLE0: _tmp132=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10F)->f1)->f1;_LLE1:
 _tmp133=_tmp132;goto _LLE3;case 4: _LLE2: _tmp133=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10F)->f1)->f1;_LLE3:
 _tmp134=_tmp133;goto _LLE5;case 5: _LLE4: _tmp134=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10F)->f1)->f1;_LLE5:
 _tmp135=_tmp134;goto _LLE7;case 1: _LLE6: _tmp135=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp10F)->f1)->f1;_LLE7:
 if(!(_tmp135->tq).real_const)return;goto _LLDF;default: goto _LLF4;}case 22: _LLE8: _tmp137=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp136=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LLE9:
# 723
{void*_tmp110=Cyc_Tcutil_compress((void*)_check_null(_tmp137->topt));void*_tmp111=_tmp110;struct Cyc_List_List*_tmp122;struct Cyc_Absyn_Tqual _tmp121;struct Cyc_Absyn_Tqual _tmp120;switch(*((int*)_tmp111)){case 5: _LLF7: _tmp120=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp111)->f1).elt_tq;_LLF8:
 _tmp121=_tmp120;goto _LLFA;case 8: _LLF9: _tmp121=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp111)->f1).tq;_LLFA:
 if(!_tmp121.real_const)return;goto _LLF6;case 10: _LLFB: _tmp122=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp111)->f1;_LLFC: {
# 727
struct _tuple15 _tmp112=Cyc_Evexp_eval_const_uint_exp(_tmp136);struct _tuple15 _tmp113=_tmp112;unsigned int _tmp11F;int _tmp11E;_LL100: _tmp11F=_tmp113.f1;_tmp11E=_tmp113.f2;_LL101:;
if(!_tmp11E){
({void*_tmp114=0;({unsigned int _tmp5D6=e->loc;struct _dyneither_ptr _tmp5D5=({const char*_tmp115="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp115,sizeof(char),47);});Cyc_Tcutil_terr(_tmp5D6,_tmp5D5,_tag_dyneither(_tmp114,sizeof(void*),0));});});
return;}
# 732
{struct _handler_cons _tmp116;_push_handler(& _tmp116);{int _tmp118=0;if(setjmp(_tmp116.handler))_tmp118=1;if(!_tmp118){
{struct _tuple17*_tmp119=((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp122,(int)_tmp11F);struct _tuple17*_tmp11A=_tmp119;struct Cyc_Absyn_Tqual _tmp11B;_LL103: _tmp11B=_tmp11A->f1;_LL104:;
if(!_tmp11B.real_const){_npop_handler(0);return;}}
# 733
;_pop_handler();}else{void*_tmp117=(void*)_exn_thrown;void*_tmp11C=_tmp117;void*_tmp11D;if(((struct Cyc_List_Nth_exn_struct*)_tmp11C)->tag == Cyc_List_Nth){_LL106: _LL107:
# 735
 return;}else{_LL108: _tmp11D=_tmp11C;_LL109:(int)_rethrow(_tmp11D);}_LL105:;}};}
goto _LLF6;}default: _LLFD: _LLFE:
 goto _LLF6;}_LLF6:;}
# 739
goto _LLDF;case 20: _LLEA: _tmp139=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp138=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LLEB:
# 741
{void*_tmp123=Cyc_Tcutil_compress((void*)_check_null(_tmp139->topt));void*_tmp124=_tmp123;struct Cyc_List_List*_tmp126;struct Cyc_Absyn_Aggrdecl**_tmp125;switch(*((int*)_tmp124)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp124)->f1).aggr_info).KnownAggr).tag == 2){_LL10B: _tmp125=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp124)->f1).aggr_info).KnownAggr).val;_LL10C: {
# 743
struct Cyc_Absyn_Aggrfield*sf=
_tmp125 == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp125,_tmp138);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}}else{goto _LL10F;}case 12: _LL10D: _tmp126=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp124)->f2;_LL10E: {
# 748
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp126,_tmp138);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL10A;}default: _LL10F: _LL110:
 goto _LL10A;}_LL10A:;}
# 753
goto _LLDF;case 21: _LLEC: _tmp13B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_tmp13A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp10F)->f2;_LLED:
# 755
{void*_tmp127=Cyc_Tcutil_compress((void*)_check_null(_tmp13B->topt));void*_tmp128=_tmp127;void*_tmp12D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128)->tag == 5){_LL112: _tmp12D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128)->f1).elt_typ;_LL113:
# 757
{void*_tmp129=Cyc_Tcutil_compress(_tmp12D);void*_tmp12A=_tmp129;struct Cyc_List_List*_tmp12C;struct Cyc_Absyn_Aggrdecl**_tmp12B;switch(*((int*)_tmp12A)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12A)->f1).aggr_info).KnownAggr).tag == 2){_LL117: _tmp12B=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp12A)->f1).aggr_info).KnownAggr).val;_LL118: {
# 759
struct Cyc_Absyn_Aggrfield*sf=
_tmp12B == 0?0: Cyc_Absyn_lookup_decl_field(*_tmp12B,_tmp13A);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}}else{goto _LL11B;}case 12: _LL119: _tmp12C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp12A)->f2;_LL11A: {
# 764
struct Cyc_Absyn_Aggrfield*sf=Cyc_Absyn_lookup_field(_tmp12C,_tmp13A);
if(sf == 0  || !(sf->tq).real_const)return;
goto _LL116;}default: _LL11B: _LL11C:
 goto _LL116;}_LL116:;}
# 769
goto _LL111;}else{_LL114: _LL115:
 goto _LL111;}_LL111:;}
# 772
goto _LLDF;case 19: _LLEE: _tmp13C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LLEF:
# 774
{void*_tmp12E=Cyc_Tcutil_compress((void*)_check_null(_tmp13C->topt));void*_tmp12F=_tmp12E;struct Cyc_Absyn_Tqual _tmp131;struct Cyc_Absyn_Tqual _tmp130;switch(*((int*)_tmp12F)){case 5: _LL11E: _tmp130=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12F)->f1).elt_tq;_LL11F:
 _tmp131=_tmp130;goto _LL121;case 8: _LL120: _tmp131=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp12F)->f1).tq;_LL121:
 if(!_tmp131.real_const)return;goto _LL11D;default: _LL122: _LL123:
 goto _LL11D;}_LL11D:;}
# 779
goto _LLDF;case 11: _LLF0: _tmp13D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LLF1:
 _tmp13E=_tmp13D;goto _LLF3;case 12: _LLF2: _tmp13E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp10F)->f1;_LLF3:
 Cyc_Tcexp_check_writable(te,_tmp13E);return;default: _LLF4: _LLF5:
 goto _LLDF;}_LLDF:;}
# 784
({struct Cyc_String_pa_PrintArg_struct _tmp141;_tmp141.tag=0;({struct _dyneither_ptr _tmp5D7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp141.f1=_tmp5D7;});({void*_tmp13F[1]={& _tmp141};({unsigned int _tmp5D9=e->loc;struct _dyneither_ptr _tmp5D8=({const char*_tmp140="attempt to write a const location: %s";_tag_dyneither(_tmp140,sizeof(char),38);});Cyc_Tcutil_terr(_tmp5D9,_tmp5D8,_tag_dyneither(_tmp13F,sizeof(void*),1));});});});}
# 787
static void*Cyc_Tcexp_tcIncrement(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Incrementor i){
# 790
({struct Cyc_Tcenv_Tenv*_tmp5DA=Cyc_Tcenv_enter_lhs(te);Cyc_Tcexp_tcExpNoPromote(_tmp5DA,0,e);});
if(!Cyc_Absyn_is_lvalue(e))
return({void*_tmp142=0;({struct Cyc_Tcenv_Tenv*_tmp5DE=te;unsigned int _tmp5DD=loc;void**_tmp5DC=topt;struct _dyneither_ptr _tmp5DB=({const char*_tmp143="increment/decrement of non-lvalue";_tag_dyneither(_tmp143,sizeof(char),34);});Cyc_Tcexp_expr_err(_tmp5DE,_tmp5DD,_tmp5DC,_tmp5DB,_tag_dyneither(_tmp142,sizeof(void*),0));});});
Cyc_Tcexp_check_writable(te,e);{
void*t=(void*)_check_null(e->topt);
# 796
if(!Cyc_Tcutil_is_numeric(e)){
void*telt=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& telt) || 
Cyc_Tcutil_is_zero_pointer_typ_elt(t,& telt) && (i == Cyc_Absyn_PreInc  || i == Cyc_Absyn_PostInc)){
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(telt),& Cyc_Tcutil_tmk);}))
({void*_tmp144=0;({unsigned int _tmp5E0=e->loc;struct _dyneither_ptr _tmp5DF=({const char*_tmp145="can't perform arithmetic on abstract pointer type";_tag_dyneither(_tmp145,sizeof(char),50);});Cyc_Tcutil_terr(_tmp5E0,_tmp5DF,_tag_dyneither(_tmp144,sizeof(void*),0));});});
if(Cyc_Tcutil_is_noalias_pointer(t,0))
({void*_tmp146=0;({unsigned int _tmp5E2=e->loc;struct _dyneither_ptr _tmp5E1=({const char*_tmp147="can't perform arithmetic on non-aliasing pointer type";_tag_dyneither(_tmp147,sizeof(char),54);});Cyc_Tcutil_terr(_tmp5E2,_tmp5E1,_tag_dyneither(_tmp146,sizeof(void*),0));});});}else{
# 805
({struct Cyc_String_pa_PrintArg_struct _tmp14A;_tmp14A.tag=0;({struct _dyneither_ptr _tmp5E3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp14A.f1=_tmp5E3;});({void*_tmp148[1]={& _tmp14A};({unsigned int _tmp5E5=e->loc;struct _dyneither_ptr _tmp5E4=({const char*_tmp149="expecting arithmetic or ? type but found %s";_tag_dyneither(_tmp149,sizeof(char),44);});Cyc_Tcutil_terr(_tmp5E5,_tmp5E4,_tag_dyneither(_tmp148,sizeof(void*),1));});});});}}
# 807
return t;};}
# 811
static void*Cyc_Tcexp_tcConditional(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
# 813
({struct Cyc_Tcenv_Tenv*_tmp5E7=Cyc_Tcenv_clear_abstract_val_ok(te);struct Cyc_Absyn_Exp*_tmp5E6=e1;Cyc_Tcexp_tcTest(_tmp5E7,_tmp5E6,({const char*_tmp14B="conditional expression";_tag_dyneither(_tmp14B,sizeof(char),23);}));});
Cyc_Tcexp_tcExp(te,topt,e2);
Cyc_Tcexp_tcExp(te,topt,e3);{
void*t;
if(Cyc_Tcenv_abstract_val_ok(te))
({void*_tmp5E8=({Cyc_Absyn_new_evar(& Cyc_Tcutil_tako,Cyc_Tcenv_lookup_opt_type_vars(te));});t=_tmp5E8;});else{
# 820
({void*_tmp5E9=({Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));});t=_tmp5E9;});}{
struct Cyc_List_List _tmp14C=({struct Cyc_List_List _tmp153;_tmp153.hd=e3;_tmp153.tl=0;_tmp153;});
struct Cyc_List_List _tmp14D=({struct Cyc_List_List _tmp152;_tmp152.hd=e2;_tmp152.tl=& _tmp14C;_tmp152;});
if(!Cyc_Tcutil_coerce_list(te,t,& _tmp14D)){
({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0;({struct _dyneither_ptr _tmp5EA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e3->topt)));_tmp151.f1=_tmp5EA;});({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0;({struct _dyneither_ptr _tmp5EB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e2->topt)));_tmp150.f1=_tmp5EB;});({void*_tmp14E[2]={& _tmp150,& _tmp151};({unsigned int _tmp5ED=loc;struct _dyneither_ptr _tmp5EC=({const char*_tmp14F="conditional clause types do not match: %s != %s";_tag_dyneither(_tmp14F,sizeof(char),48);});Cyc_Tcutil_terr(_tmp5ED,_tmp5EC,_tag_dyneither(_tmp14E,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 828
return t;};};}
# 832
static void*Cyc_Tcexp_tcAnd(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 834
({struct Cyc_Tcenv_Tenv*_tmp5EF=te;struct Cyc_Absyn_Exp*_tmp5EE=e1;Cyc_Tcexp_tcTest(_tmp5EF,_tmp5EE,({const char*_tmp154="logical-and expression";_tag_dyneither(_tmp154,sizeof(char),23);}));});
({struct Cyc_Tcenv_Tenv*_tmp5F1=te;struct Cyc_Absyn_Exp*_tmp5F0=e2;Cyc_Tcexp_tcTest(_tmp5F1,_tmp5F0,({const char*_tmp155="logical-and expression";_tag_dyneither(_tmp155,sizeof(char),23);}));});
return Cyc_Absyn_sint_typ;}
# 840
static void*Cyc_Tcexp_tcOr(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 842
({struct Cyc_Tcenv_Tenv*_tmp5F3=te;struct Cyc_Absyn_Exp*_tmp5F2=e1;Cyc_Tcexp_tcTest(_tmp5F3,_tmp5F2,({const char*_tmp156="logical-or expression";_tag_dyneither(_tmp156,sizeof(char),22);}));});
({struct Cyc_Tcenv_Tenv*_tmp5F5=te;struct Cyc_Absyn_Exp*_tmp5F4=e2;Cyc_Tcexp_tcTest(_tmp5F5,_tmp5F4,({const char*_tmp157="logical-or expression";_tag_dyneither(_tmp157,sizeof(char),22);}));});
return Cyc_Absyn_sint_typ;}
# 848
static void*Cyc_Tcexp_tcAssignOp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*po,struct Cyc_Absyn_Exp*e2){
# 855
({struct Cyc_Tcenv_Tenv*_tmp5F6=Cyc_Tcenv_enter_lhs(Cyc_Tcenv_enter_notreadctxt(te));Cyc_Tcexp_tcExpNoPromote(_tmp5F6,0,e1);});{
void*t1=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExp(te,& t1,e2);{
void*t2=(void*)_check_null(e2->topt);
# 860
{void*_tmp158=Cyc_Tcutil_compress(t1);void*_tmp159=_tmp158;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp159)->tag == 8){_LL125: _LL126:
({void*_tmp15A=0;({unsigned int _tmp5F8=loc;struct _dyneither_ptr _tmp5F7=({const char*_tmp15B="cannot assign to an array";_tag_dyneither(_tmp15B,sizeof(char),26);});Cyc_Tcutil_terr(_tmp5F8,_tmp5F7,_tag_dyneither(_tmp15A,sizeof(void*),0));});});goto _LL124;}else{_LL127: _LL128:
 goto _LL124;}_LL124:;}
# 865
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t1),& Cyc_Tcutil_tmk);}))
({void*_tmp15C=0;({unsigned int _tmp5FA=loc;struct _dyneither_ptr _tmp5F9=({const char*_tmp15D="type is abstract (can't determine size).";_tag_dyneither(_tmp15D,sizeof(char),41);});Cyc_Tcutil_terr(_tmp5FA,_tmp5F9,_tag_dyneither(_tmp15C,sizeof(void*),0));});});
# 869
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp15E=0;({struct Cyc_Tcenv_Tenv*_tmp5FE=te;unsigned int _tmp5FD=loc;void**_tmp5FC=topt;struct _dyneither_ptr _tmp5FB=({const char*_tmp15F="assignment to non-lvalue";_tag_dyneither(_tmp15F,sizeof(char),25);});Cyc_Tcexp_expr_err(_tmp5FE,_tmp5FD,_tmp5FC,_tmp5FB,_tag_dyneither(_tmp15E,sizeof(void*),0));});});
Cyc_Tcexp_check_writable(te,e1);
if(po == 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e2))
({void*_tmp160=0;({unsigned int _tmp600=e2->loc;struct _dyneither_ptr _tmp5FF=({const char*_tmp161="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp161,sizeof(char),49);});Cyc_Tcutil_terr(_tmp600,_tmp5FF,_tag_dyneither(_tmp160,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_assign(te,e2,t1)){
void*_tmp162=({struct Cyc_String_pa_PrintArg_struct _tmp166;_tmp166.tag=0;({struct _dyneither_ptr _tmp601=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp166.f1=_tmp601;});({struct Cyc_String_pa_PrintArg_struct _tmp165;_tmp165.tag=0;({struct _dyneither_ptr _tmp602=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp165.f1=_tmp602;});({void*_tmp163[2]={& _tmp165,& _tmp166};({struct Cyc_Tcenv_Tenv*_tmp606=te;unsigned int _tmp605=loc;void**_tmp604=topt;struct _dyneither_ptr _tmp603=({const char*_tmp164="type mismatch: %s != %s";_tag_dyneither(_tmp164,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp606,_tmp605,_tmp604,_tmp603,_tag_dyneither(_tmp163,sizeof(void*),2));});});});});
Cyc_Tcutil_unify(t1,t2);
Cyc_Tcutil_explain_failure();
return _tmp162;}}else{
# 883
enum Cyc_Absyn_Primop _tmp167=(enum Cyc_Absyn_Primop)po->v;
void*_tmp168=Cyc_Tcexp_tcBinPrimop(te,loc,0,_tmp167,e1,e2);
if(!(Cyc_Tcutil_unify(_tmp168,t1) || Cyc_Tcutil_coerceable(_tmp168) && Cyc_Tcutil_coerceable(t1))){
void*_tmp169=({struct Cyc_String_pa_PrintArg_struct _tmp16D;_tmp16D.tag=0;({struct _dyneither_ptr _tmp607=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 889
Cyc_Absynpp_typ2string(t2));_tmp16D.f1=_tmp607;});({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0;({struct _dyneither_ptr _tmp608=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 888
Cyc_Absynpp_typ2string(t1));_tmp16C.f1=_tmp608;});({void*_tmp16A[2]={& _tmp16C,& _tmp16D};({struct Cyc_Tcenv_Tenv*_tmp60C=te;unsigned int _tmp60B=loc;void**_tmp60A=topt;struct _dyneither_ptr _tmp609=({const char*_tmp16B="Cannot use this operator in an assignment when the arguments have types %s and %s";_tag_dyneither(_tmp16B,sizeof(char),82);});Cyc_Tcexp_expr_err(_tmp60C,_tmp60B,_tmp60A,_tmp609,_tag_dyneither(_tmp16A,sizeof(void*),2));});});});});
# 890
Cyc_Tcutil_unify(_tmp168,t1);
Cyc_Tcutil_explain_failure();
return _tmp169;}
# 894
return _tmp168;}
# 896
return t1;};};}
# 900
static void*Cyc_Tcexp_tcSeqExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
({struct Cyc_Tcenv_Tenv*_tmp60D=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp60D,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp60F=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp60E=topt;Cyc_Tcexp_tcExp(_tmp60F,_tmp60E,e2);});
return(void*)_check_null(e2->topt);}
# 907
static struct Cyc_Absyn_Datatypefield*Cyc_Tcexp_tcInjection(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*tu,struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 911
struct Cyc_List_List*fields;
void*t1=(void*)_check_null(e->topt);
# 914
{void*_tmp16E=Cyc_Tcutil_compress(t1);void*_tmp16F=_tmp16E;switch(*((int*)_tmp16F)){case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp16F)->f1 == 0){_LL12A: _LL12B:
({struct Cyc_Tcenv_Tenv*_tmp611=te;struct Cyc_Absyn_Exp*_tmp610=e;Cyc_Tcutil_unchecked_cast(_tmp611,_tmp610,Cyc_Absyn_float_typ(1),Cyc_Absyn_No_coercion);});({void*_tmp612=Cyc_Absyn_float_typ(1);t1=_tmp612;});goto _LL129;}else{goto _LL130;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp16F)->f2){case Cyc_Absyn_Char_sz: _LL12C: _LL12D:
 goto _LL12F;case Cyc_Absyn_Short_sz: _LL12E: _LL12F:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);t1=Cyc_Absyn_sint_typ;goto _LL129;default: goto _LL130;}default: _LL130: _LL131:
 goto _LL129;}_LL129:;}
# 921
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp170=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp171=_tmp170;struct _tuple1*_tmp175;struct Cyc_List_List*_tmp174;unsigned int _tmp173;enum Cyc_Absyn_Scope _tmp172;_LL133: _tmp175=_tmp171.name;_tmp174=_tmp171.typs;_tmp173=_tmp171.loc;_tmp172=_tmp171.sc;_LL134:;
# 924
if(_tmp174 == 0  || _tmp174->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp174->hd)).f2);
# 927
if(Cyc_Tcutil_unify(t1,t2))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 932
for(fields=fs;fields != 0;fields=fields->tl){
struct Cyc_Absyn_Datatypefield _tmp176=*((struct Cyc_Absyn_Datatypefield*)fields->hd);struct Cyc_Absyn_Datatypefield _tmp177=_tmp176;struct _tuple1*_tmp17B;struct Cyc_List_List*_tmp17A;unsigned int _tmp179;enum Cyc_Absyn_Scope _tmp178;_LL136: _tmp17B=_tmp177.name;_tmp17A=_tmp177.typs;_tmp179=_tmp177.loc;_tmp178=_tmp177.sc;_LL137:;
# 935
if(_tmp17A == 0  || _tmp17A->tl != 0)continue;{
void*t2=Cyc_Tcutil_rsubstitute(r,inst,(*((struct _tuple17*)_tmp17A->hd)).f2);
# 939
int bogus=0;
if(Cyc_Tcutil_coerce_arg(te,e,t2,& bogus))
return(struct Cyc_Absyn_Datatypefield*)fields->hd;};}
# 944
({struct Cyc_String_pa_PrintArg_struct _tmp17F;_tmp17F.tag=0;({struct _dyneither_ptr _tmp613=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));_tmp17F.f1=_tmp613;});({struct Cyc_String_pa_PrintArg_struct _tmp17E;_tmp17E.tag=0;({struct _dyneither_ptr _tmp614=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(tu));_tmp17E.f1=_tmp614;});({void*_tmp17C[2]={& _tmp17E,& _tmp17F};({unsigned int _tmp616=e->loc;struct _dyneither_ptr _tmp615=({const char*_tmp17D="can't find a field in %s to inject a value of type %s";_tag_dyneither(_tmp17D,sizeof(char),54);});Cyc_Tcutil_terr(_tmp616,_tmp615,_tag_dyneither(_tmp17C,sizeof(void*),2));});});});});
return 0;}
# 950
static void*Cyc_Tcexp_tcFnCall(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*args,struct Cyc_Absyn_VarargCallInfo**vararg_call_info,struct Cyc_List_List**alias_arg_exps){
# 956
struct Cyc_List_List*_tmp180=args;
int _tmp181=0;
struct Cyc_Tcenv_Tenv*_tmp182=Cyc_Tcenv_new_block(loc,te_orig);
struct Cyc_Tcenv_Tenv*_tmp183=Cyc_Tcenv_clear_abstract_val_ok(_tmp182);
Cyc_Tcexp_tcExp(_tmp183,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
# 965
void*_tmp184=t;void*_tmp1E7;struct Cyc_Absyn_Tqual _tmp1E6;void*_tmp1E5;union Cyc_Absyn_Constraint*_tmp1E4;union Cyc_Absyn_Constraint*_tmp1E3;union Cyc_Absyn_Constraint*_tmp1E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp184)->tag == 5){_LL139: _tmp1E7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp184)->f1).elt_typ;_tmp1E6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp184)->f1).elt_tq;_tmp1E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp184)->f1).ptr_atts).rgn;_tmp1E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp184)->f1).ptr_atts).nullable;_tmp1E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp184)->f1).ptr_atts).bounds;_tmp1E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp184)->f1).ptr_atts).zero_term;_LL13A:
# 970
 Cyc_Tcenv_check_rgn_accessible(_tmp183,loc,_tmp1E5);
# 972
Cyc_Tcutil_check_nonzero_bound(loc,_tmp1E3);{
void*_tmp185=Cyc_Tcutil_compress(_tmp1E7);void*_tmp186=_tmp185;struct Cyc_List_List*_tmp1DE;void*_tmp1DD;struct Cyc_Absyn_Tqual _tmp1DC;void*_tmp1DB;struct Cyc_List_List*_tmp1DA;int _tmp1D9;struct Cyc_Absyn_VarargInfo*_tmp1D8;struct Cyc_List_List*_tmp1D7;struct Cyc_List_List*_tmp1D6;struct Cyc_Absyn_Exp*_tmp1D5;struct Cyc_List_List*_tmp1D4;struct Cyc_Absyn_Exp*_tmp1D3;struct Cyc_List_List*_tmp1D2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->tag == 9){_LL13E: _tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).tvars;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).effect;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).ret_tqual;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).ret_typ;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).args;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).c_varargs;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).cyc_varargs;_tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).rgn_po;_tmp1D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).attributes;_tmp1D5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).requires_clause;_tmp1D4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).requires_relns;_tmp1D3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).ensures_clause;_tmp1D2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp186)->f1).ensures_relns;_LL13F:
# 977
 if(_tmp1DE != 0  || _tmp1D7 != 0)
({void*_tmp187=0;({unsigned int _tmp618=e->loc;struct _dyneither_ptr _tmp617=({const char*_tmp188="function should have been instantiated prior to use -- probably a compiler bug";_tag_dyneither(_tmp188,sizeof(char),79);});Cyc_Tcutil_terr(_tmp618,_tmp617,_tag_dyneither(_tmp187,sizeof(void*),0));});});
# 981
if(topt != 0)Cyc_Tcutil_unify(_tmp1DB,*topt);
# 983
while(_tmp180 != 0  && _tmp1DA != 0){
# 985
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp180->hd;
void*t2=(*((struct _tuple9*)_tmp1DA->hd)).f3;
Cyc_Tcexp_tcExp(_tmp183,& t2,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp183,e1,t2,& alias_coercion)){
struct _dyneither_ptr s0=({const char*_tmp198="actual argument has type ";_tag_dyneither(_tmp198,sizeof(char),26);});
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string((void*)_check_null(e1->topt));
struct _dyneither_ptr s2=({const char*_tmp197=" but formal has type ";_tag_dyneither(_tmp197,sizeof(char),22);});
struct _dyneither_ptr s3=Cyc_Absynpp_typ2string(t2);
if(({({unsigned long _tmp61B=({unsigned long _tmp61A=({unsigned long _tmp619=Cyc_strlen((struct _dyneither_ptr)s0);_tmp619 + Cyc_strlen((struct _dyneither_ptr)s1);});_tmp61A + Cyc_strlen((struct _dyneither_ptr)s2);});_tmp61B + Cyc_strlen((struct _dyneither_ptr)s3);})>= 70;}))
({void*_tmp189=0;({unsigned int _tmp61E=e1->loc;struct _dyneither_ptr _tmp61D=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18F;_tmp18F.tag=0;_tmp18F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp18E;_tmp18E.tag=0;_tmp18E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp18D;_tmp18D.tag=0;_tmp18D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp18C;_tmp18C.tag=0;_tmp18C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp18A[4]={& _tmp18C,& _tmp18D,& _tmp18E,& _tmp18F};({struct _dyneither_ptr _tmp61C=({const char*_tmp18B="%s\n\t%s\n%s\n\t%s.";_tag_dyneither(_tmp18B,sizeof(char),15);});Cyc_aprintf(_tmp61C,_tag_dyneither(_tmp18A,sizeof(void*),4));});});});});});});Cyc_Tcutil_terr(_tmp61E,_tmp61D,_tag_dyneither(_tmp189,sizeof(void*),0));});});else{
# 997
({void*_tmp190=0;({unsigned int _tmp621=e1->loc;struct _dyneither_ptr _tmp620=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp196;_tmp196.tag=0;_tmp196.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s3);({struct Cyc_String_pa_PrintArg_struct _tmp195;_tmp195.tag=0;_tmp195.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({struct Cyc_String_pa_PrintArg_struct _tmp194;_tmp194.tag=0;_tmp194.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({struct Cyc_String_pa_PrintArg_struct _tmp193;_tmp193.tag=0;_tmp193.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s0);({void*_tmp191[4]={& _tmp193,& _tmp194,& _tmp195,& _tmp196};({struct _dyneither_ptr _tmp61F=({const char*_tmp192="%s%s%s%s.";_tag_dyneither(_tmp192,sizeof(char),10);});Cyc_aprintf(_tmp61F,_tag_dyneither(_tmp191,sizeof(void*),4));});});});});});});Cyc_Tcutil_terr(_tmp621,_tmp620,_tag_dyneither(_tmp190,sizeof(void*),0));});});}
Cyc_Tcutil_unify((void*)_check_null(e1->topt),t2);
Cyc_Tcutil_explain_failure();}
# 1002
if(alias_coercion)
({struct Cyc_List_List*_tmp622=({struct Cyc_List_List*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->hd=(void*)_tmp181;_tmp199->tl=*alias_arg_exps;_tmp199;});*alias_arg_exps=_tmp622;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t2) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp19A=0;({unsigned int _tmp624=e1->loc;struct _dyneither_ptr _tmp623=({const char*_tmp19B="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp19B,sizeof(char),49);});Cyc_Tcutil_terr(_tmp624,_tmp623,_tag_dyneither(_tmp19A,sizeof(void*),0));});});
_tmp180=_tmp180->tl;
_tmp1DA=_tmp1DA->tl;
++ _tmp181;}{
# 1013
int args_already_checked=0;
{struct Cyc_List_List*a=_tmp1D6;for(0;a != 0;a=a->tl){
void*_tmp19C=(void*)a->hd;void*_tmp19D=_tmp19C;enum Cyc_Absyn_Format_Type _tmp1AA;int _tmp1A9;int _tmp1A8;if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp19D)->tag == 19){_LL143: _tmp1AA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp19D)->f1;_tmp1A9=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp19D)->f2;_tmp1A8=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp19D)->f3;_LL144:
# 1017
{struct _handler_cons _tmp19E;_push_handler(& _tmp19E);{int _tmp1A0=0;if(setjmp(_tmp19E.handler))_tmp1A0=1;if(!_tmp1A0){
# 1019
{struct Cyc_Absyn_Exp*_tmp1A1=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(args,_tmp1A9 - 1);
# 1021
struct Cyc_Core_Opt*fmt_args;
if(_tmp1A8 == 0)
fmt_args=0;else{
# 1025
({struct Cyc_Core_Opt*_tmp626=({struct Cyc_Core_Opt*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));({struct Cyc_List_List*_tmp625=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,int i))Cyc_List_nth_tail)(args,_tmp1A8 - 1);_tmp1A2->v=_tmp625;});_tmp1A2;});fmt_args=_tmp626;});}
args_already_checked=1;{
enum Cyc_Absyn_Format_Type _tmp1A3=_tmp1AA;if(_tmp1A3 == Cyc_Absyn_Printf_ft){_LL148: _LL149:
# 1029
 Cyc_Tcexp_check_format_args(_tmp183,_tmp1A1,fmt_args,_tmp1A8 - 1,alias_arg_exps,Cyc_Core_heap_region,Cyc_Formatstr_get_format_typs);
# 1032
goto _LL147;}else{_LL14A: _LL14B:
# 1034
 Cyc_Tcexp_check_format_args(_tmp183,_tmp1A1,fmt_args,_tmp1A8 - 1,alias_arg_exps,Cyc_Core_heap_region,Cyc_Formatstr_get_scanf_typs);
# 1037
goto _LL147;}_LL147:;};}
# 1019
;_pop_handler();}else{void*_tmp19F=(void*)_exn_thrown;void*_tmp1A4=_tmp19F;void*_tmp1A7;if(((struct Cyc_List_Nth_exn_struct*)_tmp1A4)->tag == Cyc_List_Nth){_LL14D: _LL14E:
# 1039
({void*_tmp1A5=0;({unsigned int _tmp628=loc;struct _dyneither_ptr _tmp627=({const char*_tmp1A6="bad format arguments";_tag_dyneither(_tmp1A6,sizeof(char),21);});Cyc_Tcutil_terr(_tmp628,_tmp627,_tag_dyneither(_tmp1A5,sizeof(void*),0));});});goto _LL14C;}else{_LL14F: _tmp1A7=_tmp1A4;_LL150:(int)_rethrow(_tmp1A7);}_LL14C:;}};}
goto _LL142;}else{_LL145: _LL146:
 goto _LL142;}_LL142:;}}
# 1044
if(_tmp1DA != 0)({void*_tmp1AB=0;({unsigned int _tmp62A=loc;struct _dyneither_ptr _tmp629=({const char*_tmp1AC="too few arguments for function";_tag_dyneither(_tmp1AC,sizeof(char),31);});Cyc_Tcutil_terr(_tmp62A,_tmp629,_tag_dyneither(_tmp1AB,sizeof(void*),0));});});else{
# 1046
if((_tmp180 != 0  || _tmp1D9) || _tmp1D8 != 0){
if(_tmp1D9)
for(0;_tmp180 != 0;_tmp180=_tmp180->tl){
Cyc_Tcexp_tcExp(_tmp183,0,(struct Cyc_Absyn_Exp*)_tmp180->hd);}else{
if(_tmp1D8 == 0)
({void*_tmp1AD=0;({unsigned int _tmp62C=loc;struct _dyneither_ptr _tmp62B=({const char*_tmp1AE="too many arguments for function";_tag_dyneither(_tmp1AE,sizeof(char),32);});Cyc_Tcutil_terr(_tmp62C,_tmp62B,_tag_dyneither(_tmp1AD,sizeof(void*),0));});});else{
# 1053
struct Cyc_Absyn_VarargInfo _tmp1AF=*_tmp1D8;struct Cyc_Absyn_VarargInfo _tmp1B0=_tmp1AF;void*_tmp1CE;int _tmp1CD;_LL152: _tmp1CE=_tmp1B0.type;_tmp1CD=_tmp1B0.inject;_LL153:;{
struct Cyc_Absyn_VarargCallInfo*_tmp1B1=({struct Cyc_Absyn_VarargCallInfo*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->num_varargs=0;_tmp1CC->injectors=0;_tmp1CC->vai=_tmp1D8;_tmp1CC;});
# 1057
*vararg_call_info=_tmp1B1;
# 1059
if(!_tmp1CD)
# 1061
for(0;_tmp180 != 0;(_tmp180=_tmp180->tl,_tmp181 ++)){
int alias_coercion=0;
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp180->hd;
++ _tmp1B1->num_varargs;
Cyc_Tcexp_tcExp(_tmp183,& _tmp1CE,e1);
if(!Cyc_Tcutil_coerce_arg(_tmp183,e1,_tmp1CE,& alias_coercion)){
({struct Cyc_String_pa_PrintArg_struct _tmp1B5;_tmp1B5.tag=0;({struct _dyneither_ptr _tmp62D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e1->topt)));_tmp1B5.f1=_tmp62D;});({struct Cyc_String_pa_PrintArg_struct _tmp1B4;_tmp1B4.tag=0;({struct _dyneither_ptr _tmp62E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1CE));_tmp1B4.f1=_tmp62E;});({void*_tmp1B2[2]={& _tmp1B4,& _tmp1B5};({unsigned int _tmp630=loc;struct _dyneither_ptr _tmp62F=({const char*_tmp1B3="vararg requires type %s but argument has type %s";_tag_dyneither(_tmp1B3,sizeof(char),49);});Cyc_Tcutil_terr(_tmp630,_tmp62F,_tag_dyneither(_tmp1B2,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 1071
if(alias_coercion)
({struct Cyc_List_List*_tmp631=({struct Cyc_List_List*_tmp1B6=_cycalloc(sizeof(*_tmp1B6));_tmp1B6->hd=(void*)_tmp181;_tmp1B6->tl=*alias_arg_exps;_tmp1B6;});*alias_arg_exps=_tmp631;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp1CE) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1B7=0;({unsigned int _tmp633=e1->loc;struct _dyneither_ptr _tmp632=({const char*_tmp1B8="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1B8,sizeof(char),49);});Cyc_Tcutil_terr(_tmp633,_tmp632,_tag_dyneither(_tmp1B7,sizeof(void*),0));});});}else{
# 1080
void*_tmp1B9=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp1CE));void*_tmp1BA=_tmp1B9;struct Cyc_Absyn_Datatypedecl*_tmp1CB;struct Cyc_List_List*_tmp1CA;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BA)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BA)->f1).datatype_info).KnownDatatype).tag == 2){_LL155: _tmp1CB=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BA)->f1).datatype_info).KnownDatatype).val;_tmp1CA=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1BA)->f1).targs;_LL156: {
# 1084
struct Cyc_Absyn_Datatypedecl*_tmp1BB=*Cyc_Tcenv_lookup_datatypedecl(_tmp183,loc,_tmp1CB->name);
struct Cyc_List_List*fields=0;
if(_tmp1BB->fields == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1BE;_tmp1BE.tag=0;({struct _dyneither_ptr _tmp634=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp1CE));_tmp1BE.f1=_tmp634;});({void*_tmp1BC[1]={& _tmp1BE};({unsigned int _tmp636=loc;struct _dyneither_ptr _tmp635=({const char*_tmp1BD="can't inject into abstract datatype %s";_tag_dyneither(_tmp1BD,sizeof(char),39);});Cyc_Tcutil_terr(_tmp636,_tmp635,_tag_dyneither(_tmp1BC,sizeof(void*),1));});});});else{
fields=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1BB->fields))->v;}
# 1095
if(!({void*_tmp637=Cyc_Tcutil_pointer_region(_tmp1CE);Cyc_Tcutil_unify(_tmp637,Cyc_Tcenv_curr_rgn(_tmp183));}))
({struct Cyc_String_pa_PrintArg_struct _tmp1C2;_tmp1C2.tag=0;({struct _dyneither_ptr _tmp638=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(Cyc_Tcenv_curr_rgn(_tmp183)));_tmp1C2.f1=_tmp638;});({struct Cyc_String_pa_PrintArg_struct _tmp1C1;_tmp1C1.tag=0;({struct _dyneither_ptr _tmp639=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp1CE));_tmp1C1.f1=_tmp639;});({void*_tmp1BF[2]={& _tmp1C1,& _tmp1C2};({unsigned int _tmp63B=loc;struct _dyneither_ptr _tmp63A=({const char*_tmp1C0="can't unify pointer region for %s to curr_rgn %s";_tag_dyneither(_tmp1C0,sizeof(char),49);});Cyc_Tcutil_terr(_tmp63B,_tmp63A,_tag_dyneither(_tmp1BF,sizeof(void*),2));});});});});{
# 1099
struct Cyc_List_List*_tmp1C3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp1BB->tvs,_tmp1CA);
for(0;_tmp180 != 0;_tmp180=_tmp180->tl){
++ _tmp1B1->num_varargs;{
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)_tmp180->hd;
# 1104
if(!args_already_checked){
Cyc_Tcexp_tcExp(_tmp183,0,e1);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e1->topt)) && !
Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp1C4=0;({unsigned int _tmp63D=e1->loc;struct _dyneither_ptr _tmp63C=({const char*_tmp1C5="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp1C5,sizeof(char),49);});Cyc_Tcutil_terr(_tmp63D,_tmp63C,_tag_dyneither(_tmp1C4,sizeof(void*),0));});});}{
# 1110
struct Cyc_Absyn_Datatypefield*_tmp1C6=({struct Cyc_Tcenv_Tenv*_tmp642=_tmp183;struct Cyc_Absyn_Exp*_tmp641=e1;void*_tmp640=Cyc_Tcutil_pointer_elt_type(_tmp1CE);struct _RegionHandle*_tmp63F=Cyc_Core_heap_region;struct Cyc_List_List*_tmp63E=_tmp1C3;Cyc_Tcexp_tcInjection(_tmp642,_tmp641,_tmp640,_tmp63F,_tmp63E,fields);});
# 1112
if(_tmp1C6 != 0)
({struct Cyc_List_List*_tmp644=({
struct Cyc_List_List*_tmp643=_tmp1B1->injectors;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp643,({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=_tmp1C6;_tmp1C7->tl=0;_tmp1C7;}));});
# 1113
_tmp1B1->injectors=_tmp644;});};};}
# 1117
goto _LL154;};}}else{goto _LL157;}}else{_LL157: _LL158:
({void*_tmp1C8=0;({unsigned int _tmp646=loc;struct _dyneither_ptr _tmp645=({const char*_tmp1C9="bad inject vararg type";_tag_dyneither(_tmp1C9,sizeof(char),23);});Cyc_Tcutil_terr(_tmp646,_tmp645,_tag_dyneither(_tmp1C8,sizeof(void*),0));});});goto _LL154;}_LL154:;}};}}}}
# 1123
if(*alias_arg_exps == 0)
# 1132 "tcexp.cyc"
Cyc_Tcenv_check_effect_accessible(_tmp183,loc,(void*)_check_null(_tmp1DD));
# 1134
return _tmp1DB;};}else{_LL140: _LL141:
 return({struct Cyc_String_pa_PrintArg_struct _tmp1D1;_tmp1D1.tag=0;({struct _dyneither_ptr _tmp647=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1D1.f1=_tmp647;});({void*_tmp1CF[1]={& _tmp1D1};({struct Cyc_Tcenv_Tenv*_tmp64B=_tmp183;unsigned int _tmp64A=loc;void**_tmp649=topt;struct _dyneither_ptr _tmp648=({const char*_tmp1D0="expected pointer to function but found %s";_tag_dyneither(_tmp1D0,sizeof(char),42);});Cyc_Tcexp_expr_err(_tmp64B,_tmp64A,_tmp649,_tmp648,_tag_dyneither(_tmp1CF,sizeof(void*),1));});});});}_LL13D:;};}else{_LL13B: _LL13C:
# 1137
 return({struct Cyc_String_pa_PrintArg_struct _tmp1E1;_tmp1E1.tag=0;({struct _dyneither_ptr _tmp64C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1E1.f1=_tmp64C;});({void*_tmp1DF[1]={& _tmp1E1};({struct Cyc_Tcenv_Tenv*_tmp650=_tmp183;unsigned int _tmp64F=loc;void**_tmp64E=topt;struct _dyneither_ptr _tmp64D=({const char*_tmp1E0="expected pointer to function but found %s";_tag_dyneither(_tmp1E0,sizeof(char),42);});Cyc_Tcexp_expr_err(_tmp650,_tmp64F,_tmp64E,_tmp64D,_tag_dyneither(_tmp1DF,sizeof(void*),1));});});});}_LL138:;};}
# 1141
static void*Cyc_Tcexp_tcThrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
int bogus=0;
void*_tmp1E8=Cyc_Absyn_exn_typ();
Cyc_Tcexp_tcExp(te,& _tmp1E8,e);
if(!Cyc_Tcutil_coerce_arg(te,e,_tmp1E8,& bogus))
({struct Cyc_String_pa_PrintArg_struct _tmp1EC;_tmp1EC.tag=0;({struct _dyneither_ptr _tmp651=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp1EC.f1=_tmp651;});({struct Cyc_String_pa_PrintArg_struct _tmp1EB;_tmp1EB.tag=0;({struct _dyneither_ptr _tmp652=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1146
Cyc_Absynpp_typ2string(_tmp1E8));_tmp1EB.f1=_tmp652;});({void*_tmp1E9[2]={& _tmp1EB,& _tmp1EC};({unsigned int _tmp654=loc;struct _dyneither_ptr _tmp653=({const char*_tmp1EA="expected %s but found %s";_tag_dyneither(_tmp1EA,sizeof(char),25);});Cyc_Tcutil_terr(_tmp654,_tmp653,_tag_dyneither(_tmp1E9,sizeof(void*),2));});});});});
# 1148
if(topt != 0)return*topt;
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}
# 1153
static void*Cyc_Tcexp_tcInstantiate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*ts){
# 1155
Cyc_Tcexp_tcExpNoInst(te,0,e);
# 1157
({void*_tmp655=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp655;});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
{void*_tmp1ED=t1;void*_tmp20C;struct Cyc_Absyn_Tqual _tmp20B;void*_tmp20A;union Cyc_Absyn_Constraint*_tmp209;union Cyc_Absyn_Constraint*_tmp208;union Cyc_Absyn_Constraint*_tmp207;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1ED)->tag == 5){_LL15A: _tmp20C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1ED)->f1).elt_typ;_tmp20B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1ED)->f1).elt_tq;_tmp20A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1ED)->f1).ptr_atts).rgn;_tmp209=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1ED)->f1).ptr_atts).nullable;_tmp208=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1ED)->f1).ptr_atts).bounds;_tmp207=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1ED)->f1).ptr_atts).zero_term;_LL15B:
# 1161
{void*_tmp1EE=Cyc_Tcutil_compress(_tmp20C);void*_tmp1EF=_tmp1EE;struct Cyc_List_List*_tmp206;void*_tmp205;struct Cyc_Absyn_Tqual _tmp204;void*_tmp203;struct Cyc_List_List*_tmp202;int _tmp201;struct Cyc_Absyn_VarargInfo*_tmp200;struct Cyc_List_List*_tmp1FF;struct Cyc_List_List*_tmp1FE;struct Cyc_Absyn_Exp*_tmp1FD;struct Cyc_List_List*_tmp1FC;struct Cyc_Absyn_Exp*_tmp1FB;struct Cyc_List_List*_tmp1FA;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->tag == 9){_LL15F: _tmp206=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).tvars;_tmp205=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).effect;_tmp204=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).ret_tqual;_tmp203=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).ret_typ;_tmp202=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).args;_tmp201=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).c_varargs;_tmp200=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).cyc_varargs;_tmp1FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).rgn_po;_tmp1FE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).attributes;_tmp1FD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).requires_clause;_tmp1FC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).requires_relns;_tmp1FB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).ensures_clause;_tmp1FA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EF)->f1).ensures_relns;_LL160: {
# 1163
struct Cyc_List_List*instantiation=0;
# 1165
for(0;ts != 0  && _tmp206 != 0;(ts=ts->tl,_tmp206=_tmp206->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp206->hd,& Cyc_Tcutil_bk);
({unsigned int _tmp659=loc;struct Cyc_Tcenv_Tenv*_tmp658=te;struct Cyc_List_List*_tmp657=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp656=k;Cyc_Tcutil_check_type(_tmp659,_tmp658,_tmp657,_tmp656,1,1,(void*)ts->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmp65B=({struct Cyc_List_List*_tmp1F0=_cycalloc(sizeof(*_tmp1F0));({struct _tuple13*_tmp65A=({struct _tuple13*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->f1=(struct Cyc_Absyn_Tvar*)_tmp206->hd;_tmp1F1->f2=(void*)ts->hd;_tmp1F1;});_tmp1F0->hd=_tmp65A;});_tmp1F0->tl=instantiation;_tmp1F0;});instantiation=_tmp65B;});}
# 1171
if(ts != 0)
return({void*_tmp1F2=0;({struct Cyc_Tcenv_Tenv*_tmp65F=te;unsigned int _tmp65E=loc;void**_tmp65D=topt;struct _dyneither_ptr _tmp65C=({const char*_tmp1F3="too many type variables in instantiation";_tag_dyneither(_tmp1F3,sizeof(char),41);});Cyc_Tcexp_expr_err(_tmp65F,_tmp65E,_tmp65D,_tmp65C,_tag_dyneither(_tmp1F2,sizeof(void*),0));});});
# 1176
if(_tmp206 == 0){
({struct Cyc_List_List*_tmp660=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,instantiation,_tmp1FF);_tmp1FF=_tmp660;});
Cyc_Tcenv_check_rgn_partial_order(te,loc,_tmp1FF);
_tmp1FF=0;}{
# 1181
void*new_fn_typ=({
struct Cyc_List_List*_tmp663=instantiation;Cyc_Tcutil_substitute(_tmp663,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1F7=_cycalloc(sizeof(*_tmp1F7));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp662=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp1F8;_tmp1F8.tag=9;({struct Cyc_Absyn_FnInfo _tmp661=({struct Cyc_Absyn_FnInfo _tmp1F9;_tmp1F9.tvars=_tmp206;_tmp1F9.effect=_tmp205;_tmp1F9.ret_tqual=_tmp204;_tmp1F9.ret_typ=_tmp203;_tmp1F9.args=_tmp202;_tmp1F9.c_varargs=_tmp201;_tmp1F9.cyc_varargs=_tmp200;_tmp1F9.rgn_po=_tmp1FF;_tmp1F9.attributes=_tmp1FE;_tmp1F9.requires_clause=_tmp1FD;_tmp1F9.requires_relns=_tmp1FC;_tmp1F9.ensures_clause=_tmp1FB;_tmp1F9.ensures_relns=_tmp1FA;_tmp1F9;});_tmp1F8.f1=_tmp661;});_tmp1F8;});_tmp1F7[0]=_tmp662;});_tmp1F7;}));});
# 1186
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp1F4=_cycalloc(sizeof(*_tmp1F4));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp666=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp1F5;_tmp1F5.tag=5;({struct Cyc_Absyn_PtrInfo _tmp665=({struct Cyc_Absyn_PtrInfo _tmp1F6;_tmp1F6.elt_typ=new_fn_typ;_tmp1F6.elt_tq=_tmp20B;({struct Cyc_Absyn_PtrAtts _tmp664=({(_tmp1F6.ptr_atts).rgn=_tmp20A;(_tmp1F6.ptr_atts).nullable=_tmp209;(_tmp1F6.ptr_atts).bounds=_tmp208;(_tmp1F6.ptr_atts).zero_term=_tmp207;(_tmp1F6.ptr_atts).ptrloc=0;_tmp1F6.ptr_atts;});_tmp1F6.ptr_atts=_tmp664;});_tmp1F6;});_tmp1F5.f1=_tmp665;});_tmp1F5;});_tmp1F4[0]=_tmp666;});_tmp1F4;});};}}else{_LL161: _LL162:
 goto _LL15E;}_LL15E:;}
# 1189
goto _LL159;}else{_LL15C: _LL15D:
 goto _LL159;}_LL159:;}
# 1192
return({struct Cyc_String_pa_PrintArg_struct _tmp20F;_tmp20F.tag=0;({struct _dyneither_ptr _tmp667=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t1));_tmp20F.f1=_tmp667;});({void*_tmp20D[1]={& _tmp20F};({struct Cyc_Tcenv_Tenv*_tmp66B=te;unsigned int _tmp66A=loc;void**_tmp669=topt;struct _dyneither_ptr _tmp668=({const char*_tmp20E="expecting polymorphic type but found %s";_tag_dyneither(_tmp20E,sizeof(char),40);});Cyc_Tcexp_expr_err(_tmp66B,_tmp66A,_tmp669,_tmp668,_tag_dyneither(_tmp20D,sizeof(void*),1));});});});};}
# 1197
static void*Cyc_Tcexp_tcCast(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t,struct Cyc_Absyn_Exp*e,enum Cyc_Absyn_Coercion*c){
# 1199
({unsigned int _tmp66F=loc;struct Cyc_Tcenv_Tenv*_tmp66E=te;struct Cyc_List_List*_tmp66D=Cyc_Tcenv_lookup_type_vars(te);struct Cyc_Absyn_Kind*_tmp66C=
Cyc_Tcenv_abstract_val_ok(te)?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
# 1199
Cyc_Tcutil_check_type(_tmp66F,_tmp66E,_tmp66D,_tmp66C,1,0,t);});
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
void*_tmp210=({struct Cyc_String_pa_PrintArg_struct _tmp214;_tmp214.tag=0;({struct _dyneither_ptr _tmp670=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp214.f1=_tmp670;});({struct Cyc_String_pa_PrintArg_struct _tmp213;_tmp213.tag=0;({struct _dyneither_ptr _tmp671=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t2));_tmp213.f1=_tmp671;});({void*_tmp211[2]={& _tmp213,& _tmp214};({struct Cyc_Tcenv_Tenv*_tmp674=te;unsigned int _tmp673=loc;struct _dyneither_ptr _tmp672=({const char*_tmp212="cannot cast %s to %s";_tag_dyneither(_tmp212,sizeof(char),21);});Cyc_Tcexp_expr_err(_tmp674,_tmp673,& t,_tmp672,_tag_dyneither(_tmp211,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();
return _tmp210;};}}}
# 1225
{struct _tuple0 _tmp215=({struct _tuple0 _tmp220;_tmp220.f1=e->r;({void*_tmp675=Cyc_Tcutil_compress(t);_tmp220.f2=_tmp675;});_tmp220;});struct _tuple0 _tmp216=_tmp215;int _tmp21F;union Cyc_Absyn_Constraint*_tmp21E;union Cyc_Absyn_Constraint*_tmp21D;union Cyc_Absyn_Constraint*_tmp21C;if(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp216.f1)->tag == 33){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp216.f2)->tag == 5){_LL164: _tmp21F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp216.f1)->f1).fat_result;_tmp21E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp216.f2)->f1).ptr_atts).nullable;_tmp21D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp216.f2)->f1).ptr_atts).bounds;_tmp21C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp216.f2)->f1).ptr_atts).zero_term;_LL165:
# 1229
 if((_tmp21F  && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp21C)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp21E)){
void*_tmp217=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp21D);void*_tmp218=_tmp217;struct Cyc_Absyn_Exp*_tmp21B;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218)->tag == 1){_LL169: _tmp21B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218)->f1;_LL16A:
# 1232
 if(({(Cyc_Evexp_eval_const_uint_exp(_tmp21B)).f1 == 1;}))
({void*_tmp219=0;({unsigned int _tmp677=loc;struct _dyneither_ptr _tmp676=({const char*_tmp21A="cast from ? pointer to * pointer will lose size information";_tag_dyneither(_tmp21A,sizeof(char),60);});Cyc_Tcutil_warn(_tmp677,_tmp676,_tag_dyneither(_tmp219,sizeof(void*),0));});});
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
void**_tmp221=0;
struct Cyc_Absyn_Tqual _tmp222=Cyc_Absyn_empty_tqual(0);
int _tmp223=0;
if(topt != 0){
void*_tmp224=Cyc_Tcutil_compress(*topt);void*_tmp225=_tmp224;void**_tmp228;struct Cyc_Absyn_Tqual _tmp227;union Cyc_Absyn_Constraint*_tmp226;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp225)->tag == 5){_LL16E: _tmp228=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp225)->f1).elt_typ;_tmp227=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp225)->f1).elt_tq;_tmp226=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp225)->f1).ptr_atts).nullable;_LL16F:
# 1253
 _tmp221=_tmp228;
_tmp222=_tmp227;
({int _tmp678=((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp226);_tmp223=_tmp678;});
goto _LL16D;}else{_LL170: _LL171:
 goto _LL16D;}_LL16D:;}
# 1267
({struct Cyc_Tcenv_Tenv*_tmp67A=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_enter_lhs(Cyc_Tcenv_clear_notreadctxt(te)));void**_tmp679=_tmp221;Cyc_Tcexp_tcExpNoInst(_tmp67A,_tmp679,e);});
# 1270
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(e->topt)))
({void*_tmp229=0;({unsigned int _tmp67C=e->loc;struct _dyneither_ptr _tmp67B=({const char*_tmp22A="Cannot take the address of an alias-free path";_tag_dyneither(_tmp22A,sizeof(char),46);});Cyc_Tcutil_terr(_tmp67C,_tmp67B,_tag_dyneither(_tmp229,sizeof(void*),0));});});
# 1276
{void*_tmp22B=e->r;void*_tmp22C=_tmp22B;struct Cyc_Absyn_Exp*_tmp230;struct Cyc_Absyn_Exp*_tmp22F;if(((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp22C)->tag == 22){_LL173: _tmp230=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp22C)->f1;_tmp22F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp22C)->f2;_LL174:
# 1278
{void*_tmp22D=Cyc_Tcutil_compress((void*)_check_null(_tmp230->topt));void*_tmp22E=_tmp22D;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp22E)->tag == 10){_LL178: _LL179:
 goto _LL177;}else{_LL17A: _LL17B:
# 1283
({void*_tmp67D=(Cyc_Absyn_add_exp(_tmp230,_tmp22F,0))->r;e0->r=_tmp67D;});
return Cyc_Tcexp_tcPlus(te,_tmp230,_tmp22F);}_LL177:;}
# 1286
goto _LL172;}else{_LL175: _LL176:
 goto _LL172;}_LL172:;}
# 1291
{void*_tmp231=e->r;void*_tmp232=_tmp231;switch(*((int*)_tmp232)){case 20: if(((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp232)->f3 == 1){_LL17D: _LL17E:
 goto _LL180;}else{goto _LL181;}case 21: if(((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp232)->f3 == 1){_LL17F: _LL180:
# 1294
({void*_tmp233=0;({unsigned int _tmp67F=e->loc;struct _dyneither_ptr _tmp67E=({const char*_tmp234="cannot take the address of a @tagged union member";_tag_dyneither(_tmp234,sizeof(char),50);});Cyc_Tcutil_terr(_tmp67F,_tmp67E,_tag_dyneither(_tmp233,sizeof(void*),0));});});
goto _LL17C;}else{goto _LL181;}default: _LL181: _LL182:
 goto _LL17C;}_LL17C:;}{
# 1300
struct _tuple14 _tmp235=Cyc_Tcutil_addressof_props(te,e);struct _tuple14 _tmp236=_tmp235;int _tmp23A;void*_tmp239;_LL184: _tmp23A=_tmp236.f1;_tmp239=_tmp236.f2;_LL185:;
# 1302
if(Cyc_Tcutil_is_noalias_region(_tmp239,0))
({void*_tmp237=0;({unsigned int _tmp681=e->loc;struct _dyneither_ptr _tmp680=({const char*_tmp238="using & would manufacture an alias to an alias-free pointer";_tag_dyneither(_tmp238,sizeof(char),60);});Cyc_Tcutil_terr(_tmp681,_tmp680,_tag_dyneither(_tmp237,sizeof(void*),0));});});{
# 1305
struct Cyc_Absyn_Tqual tq=Cyc_Absyn_empty_tqual(0);
if(_tmp23A){
tq.print_const=1;
tq.real_const=1;}{
# 1311
void*t=_tmp223?
Cyc_Absyn_star_typ((void*)_check_null(e->topt),_tmp239,tq,Cyc_Absyn_false_conref):
 Cyc_Absyn_at_typ((void*)_check_null(e->topt),_tmp239,tq,Cyc_Absyn_false_conref);
return t;};};};}
# 1318
static void*Cyc_Tcexp_tcSizeof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t){
if(te->allow_valueof)
# 1322
return Cyc_Absyn_uint_typ;
# 1324
({unsigned int _tmp684=loc;struct Cyc_Tcenv_Tenv*_tmp683=te;struct Cyc_List_List*_tmp682=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp684,_tmp683,_tmp682,& Cyc_Tcutil_tmk,1,0,t);});
Cyc_Tcutil_check_no_qual(loc,t);
if(!Cyc_Evexp_okay_szofarg(t))
({struct Cyc_String_pa_PrintArg_struct _tmp23D;_tmp23D.tag=0;({struct _dyneither_ptr _tmp685=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp23D.f1=_tmp685;});({void*_tmp23B[1]={& _tmp23D};({unsigned int _tmp687=loc;struct _dyneither_ptr _tmp686=({const char*_tmp23C="sizeof applied to type %s, which has unknown size here";_tag_dyneither(_tmp23C,sizeof(char),55);});Cyc_Tcutil_terr(_tmp687,_tmp686,_tag_dyneither(_tmp23B,sizeof(void*),1));});});});
if(topt != 0){
void*_tmp23E=Cyc_Tcutil_compress(*topt);void*_tmp23F=_tmp23E;void*_tmp245;void*_tmp244;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp23F)->tag == 19){_LL187: _tmp245=_tmp23F;_tmp244=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp23F)->f1;_LL188: {
# 1333
struct Cyc_Absyn_Exp*_tmp240=Cyc_Absyn_sizeoftyp_exp(t,0);
struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp241=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp242=_cycalloc(sizeof(*_tmp242));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp688=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp243;_tmp243.tag=18;_tmp243.f1=_tmp240;_tmp243;});_tmp242[0]=_tmp688;});_tmp242;});
if(Cyc_Tcutil_unify(_tmp244,(void*)_tmp241))return _tmp245;
goto _LL186;}}else{_LL189: _LL18A:
 goto _LL186;}_LL186:;}
# 1339
return Cyc_Absyn_uint_typ;}
# 1342
void*Cyc_Tcexp_structfield_type(struct _dyneither_ptr*n,struct Cyc_Absyn_Aggrfield*sf){
if(({Cyc_strcmp((struct _dyneither_ptr)*n,(struct _dyneither_ptr)*sf->name)== 0;}))return sf->type;else{
return 0;}}
# 1349
static void*Cyc_Tcexp_tcOffsetof(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,void*t_orig,struct Cyc_List_List*fs){
# 1351
({unsigned int _tmp68B=loc;struct Cyc_Tcenv_Tenv*_tmp68A=te;struct Cyc_List_List*_tmp689=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp68B,_tmp68A,_tmp689,& Cyc_Tcutil_tmk,1,0,t_orig);});
Cyc_Tcutil_check_no_qual(loc,t_orig);{
struct Cyc_List_List*_tmp246=fs;
void*_tmp247=t_orig;
for(0;_tmp246 != 0;_tmp246=_tmp246->tl){
void*_tmp248=(void*)_tmp246->hd;
void*_tmp249=_tmp248;unsigned int _tmp283;struct _dyneither_ptr*_tmp282;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp249)->tag == 0){_LL18C: _tmp282=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp249)->f1;_LL18D: {
# 1359
int bad_type=1;
{void*_tmp24A=Cyc_Tcutil_compress(_tmp247);void*_tmp24B=_tmp24A;struct Cyc_List_List*_tmp253;struct Cyc_Absyn_Aggrdecl**_tmp252;switch(*((int*)_tmp24B)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp24B)->f1).aggr_info).KnownAggr).tag == 2){_LL191: _tmp252=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp24B)->f1).aggr_info).KnownAggr).val;_LL192:
# 1362
 if((*_tmp252)->impl == 0)goto _LL190;{
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp282,((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp252)->impl))->fields);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp24E;_tmp24E.tag=0;_tmp24E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp282);({void*_tmp24C[1]={& _tmp24E};({unsigned int _tmp68D=loc;struct _dyneither_ptr _tmp68C=({const char*_tmp24D="no field of struct/union has name %s";_tag_dyneither(_tmp24D,sizeof(char),37);});Cyc_Tcutil_terr(_tmp68D,_tmp68C,_tag_dyneither(_tmp24C,sizeof(void*),1));});});});else{
# 1367
_tmp247=t2;}
bad_type=0;
goto _LL190;};}else{goto _LL195;}case 12: _LL193: _tmp253=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24B)->f2;_LL194: {
# 1371
void*t2=((void*(*)(void*(*pred)(struct _dyneither_ptr*,struct Cyc_Absyn_Aggrfield*),struct _dyneither_ptr*env,struct Cyc_List_List*x))Cyc_List_find_c)(Cyc_Tcexp_structfield_type,_tmp282,_tmp253);
if(!((unsigned int)t2))
({struct Cyc_String_pa_PrintArg_struct _tmp251;_tmp251.tag=0;_tmp251.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp282);({void*_tmp24F[1]={& _tmp251};({unsigned int _tmp68F=loc;struct _dyneither_ptr _tmp68E=({const char*_tmp250="no field of struct/union has name %s";_tag_dyneither(_tmp250,sizeof(char),37);});Cyc_Tcutil_terr(_tmp68F,_tmp68E,_tag_dyneither(_tmp24F,sizeof(void*),1));});});});else{
# 1375
_tmp247=t2;}
bad_type=0;
goto _LL190;}default: _LL195: _LL196:
 goto _LL190;}_LL190:;}
# 1380
if(bad_type){
if(_tmp246 == fs)
({struct Cyc_String_pa_PrintArg_struct _tmp256;_tmp256.tag=0;({struct _dyneither_ptr _tmp690=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp247));_tmp256.f1=_tmp690;});({void*_tmp254[1]={& _tmp256};({unsigned int _tmp692=loc;struct _dyneither_ptr _tmp691=({const char*_tmp255="%s is not a known struct/union type";_tag_dyneither(_tmp255,sizeof(char),36);});Cyc_Tcutil_terr(_tmp692,_tmp691,_tag_dyneither(_tmp254,sizeof(void*),1));});});});else{
# 1384
struct _dyneither_ptr _tmp257=({struct Cyc_String_pa_PrintArg_struct _tmp26A;_tmp26A.tag=0;({struct _dyneither_ptr _tmp693=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t_orig));_tmp26A.f1=_tmp693;});({void*_tmp268[1]={& _tmp26A};({struct _dyneither_ptr _tmp694=({const char*_tmp269="(%s)";_tag_dyneither(_tmp269,sizeof(char),5);});Cyc_aprintf(_tmp694,_tag_dyneither(_tmp268,sizeof(void*),1));});});});
struct Cyc_List_List*x;
for(x=fs;x != _tmp246;x=x->tl){
void*_tmp258=(void*)((struct Cyc_List_List*)_check_null(x))->hd;void*_tmp259=_tmp258;unsigned int _tmp263;struct _dyneither_ptr*_tmp262;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp259)->tag == 0){_LL198: _tmp262=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp259)->f1;_LL199:
# 1389
({struct _dyneither_ptr _tmp696=({struct Cyc_String_pa_PrintArg_struct _tmp25D;_tmp25D.tag=0;_tmp25D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp262);({struct Cyc_String_pa_PrintArg_struct _tmp25C;_tmp25C.tag=0;_tmp25C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp257);({void*_tmp25A[2]={& _tmp25C,& _tmp25D};({struct _dyneither_ptr _tmp695=({const char*_tmp25B="%s.%s";_tag_dyneither(_tmp25B,sizeof(char),6);});Cyc_aprintf(_tmp695,_tag_dyneither(_tmp25A,sizeof(void*),2));});});});});_tmp257=_tmp696;});goto _LL197;}else{_LL19A: _tmp263=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp259)->f1;_LL19B:
# 1391
({struct _dyneither_ptr _tmp698=({struct Cyc_Int_pa_PrintArg_struct _tmp261;_tmp261.tag=1;_tmp261.f1=(unsigned long)((int)_tmp263);({struct Cyc_String_pa_PrintArg_struct _tmp260;_tmp260.tag=0;_tmp260.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp257);({void*_tmp25E[2]={& _tmp260,& _tmp261};({struct _dyneither_ptr _tmp697=({const char*_tmp25F="%s.%d";_tag_dyneither(_tmp25F,sizeof(char),6);});Cyc_aprintf(_tmp697,_tag_dyneither(_tmp25E,sizeof(void*),2));});});});});_tmp257=_tmp698;});goto _LL197;}_LL197:;}
# 1393
({struct Cyc_String_pa_PrintArg_struct _tmp267;_tmp267.tag=0;({struct _dyneither_ptr _tmp699=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp247));_tmp267.f1=_tmp699;});({struct Cyc_String_pa_PrintArg_struct _tmp266;_tmp266.tag=0;_tmp266.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp257);({void*_tmp264[2]={& _tmp266,& _tmp267};({unsigned int _tmp69B=loc;struct _dyneither_ptr _tmp69A=({const char*_tmp265="%s == %s is not a struct/union type";_tag_dyneither(_tmp265,sizeof(char),36);});Cyc_Tcutil_terr(_tmp69B,_tmp69A,_tag_dyneither(_tmp264,sizeof(void*),2));});});});});}}
# 1396
goto _LL18B;}}else{_LL18E: _tmp283=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp249)->f1;_LL18F: {
# 1398
int bad_type=1;
{void*_tmp26B=Cyc_Tcutil_compress(_tmp247);void*_tmp26C=_tmp26B;struct Cyc_Absyn_Datatypefield*_tmp27E;struct Cyc_List_List*_tmp27D;struct Cyc_List_List*_tmp27C;struct Cyc_Absyn_Aggrdecl**_tmp27B;switch(*((int*)_tmp26C)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp26C)->f1).aggr_info).KnownAggr).tag == 2){_LL19D: _tmp27B=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp26C)->f1).aggr_info).KnownAggr).val;_LL19E:
# 1401
 if((*_tmp27B)->impl == 0)
goto _LL19C;
_tmp27C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp27B)->impl))->fields;goto _LL1A0;}else{goto _LL1A5;}case 12: _LL19F: _tmp27C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp26C)->f2;_LL1A0:
# 1405
 if(({int _tmp69C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp27C);_tmp69C <= _tmp283;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp270;_tmp270.tag=1;_tmp270.f1=(unsigned long)((int)_tmp283);({struct Cyc_Int_pa_PrintArg_struct _tmp26F;_tmp26F.tag=1;({unsigned long _tmp69D=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp27C);_tmp26F.f1=_tmp69D;});({void*_tmp26D[2]={& _tmp26F,& _tmp270};({unsigned int _tmp69F=loc;struct _dyneither_ptr _tmp69E=({const char*_tmp26E="struct/union has too few components: %d <= %d";_tag_dyneither(_tmp26E,sizeof(char),46);});Cyc_Tcutil_terr(_tmp69F,_tmp69E,_tag_dyneither(_tmp26D,sizeof(void*),2));});});});});else{
# 1409
({void*_tmp6A0=(((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp27C,(int)_tmp283))->type;_tmp247=_tmp6A0;});}
bad_type=0;
goto _LL19C;case 10: _LL1A1: _tmp27D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp26C)->f1;_LL1A2:
# 1413
 if(({int _tmp6A1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp27D);_tmp6A1 <= _tmp283;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp274;_tmp274.tag=1;_tmp274.f1=(unsigned long)((int)_tmp283);({struct Cyc_Int_pa_PrintArg_struct _tmp273;_tmp273.tag=1;({unsigned long _tmp6A2=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp27D);_tmp273.f1=_tmp6A2;});({void*_tmp271[2]={& _tmp273,& _tmp274};({unsigned int _tmp6A4=loc;struct _dyneither_ptr _tmp6A3=({const char*_tmp272="tuple has too few components: %d <= %d";_tag_dyneither(_tmp272,sizeof(char),39);});Cyc_Tcutil_terr(_tmp6A4,_tmp6A3,_tag_dyneither(_tmp271,sizeof(void*),2));});});});});else{
# 1417
({void*_tmp6A5=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp27D,(int)_tmp283)).f2;_tmp247=_tmp6A5;});}
bad_type=0;
goto _LL19C;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp26C)->f1).field_info).KnownDatatypefield).tag == 2){_LL1A3: _tmp27E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp26C)->f1).field_info).KnownDatatypefield).val).f2;_LL1A4:
# 1421
 if(({int _tmp6A6=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp27E->typs);_tmp6A6 < _tmp283;}))
({struct Cyc_Int_pa_PrintArg_struct _tmp278;_tmp278.tag=1;_tmp278.f1=(unsigned long)((int)_tmp283);({struct Cyc_Int_pa_PrintArg_struct _tmp277;_tmp277.tag=1;({unsigned long _tmp6A7=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp27E->typs);_tmp277.f1=_tmp6A7;});({void*_tmp275[2]={& _tmp277,& _tmp278};({unsigned int _tmp6A9=loc;struct _dyneither_ptr _tmp6A8=({const char*_tmp276="datatype field has too few components: %d < %d";_tag_dyneither(_tmp276,sizeof(char),47);});Cyc_Tcutil_terr(_tmp6A9,_tmp6A8,_tag_dyneither(_tmp275,sizeof(void*),2));});});});});else{
# 1425
if(_tmp283 != 0)
({void*_tmp6AA=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp27E->typs,(int)(_tmp283 - 1))).f2;_tmp247=_tmp6AA;});else{
if(_tmp246->tl != 0)
({void*_tmp279=0;({unsigned int _tmp6AC=loc;struct _dyneither_ptr _tmp6AB=({const char*_tmp27A="datatype field index 0 refers to the tag; cannot be indexed through";_tag_dyneither(_tmp27A,sizeof(char),68);});Cyc_Tcutil_terr(_tmp6AC,_tmp6AB,_tag_dyneither(_tmp279,sizeof(void*),0));});});}}
# 1430
bad_type=0;
goto _LL19C;}else{goto _LL1A5;}default: _LL1A5: _LL1A6:
 goto _LL19C;}_LL19C:;}
# 1434
if(bad_type)
({struct Cyc_String_pa_PrintArg_struct _tmp281;_tmp281.tag=0;({struct _dyneither_ptr _tmp6AD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp247));_tmp281.f1=_tmp6AD;});({void*_tmp27F[1]={& _tmp281};({unsigned int _tmp6AF=loc;struct _dyneither_ptr _tmp6AE=({const char*_tmp280="%s is not a known type";_tag_dyneither(_tmp280,sizeof(char),23);});Cyc_Tcutil_terr(_tmp6AF,_tmp6AE,_tag_dyneither(_tmp27F,sizeof(void*),1));});});});
goto _LL18B;}}_LL18B:;}
# 1439
return Cyc_Absyn_uint_typ;};}
# 1443
static void*Cyc_Tcexp_tcDeref(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e){
struct Cyc_Tcenv_Tenv*_tmp284=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
Cyc_Tcexp_tcExp(_tmp284,0,e);{
void*t=Cyc_Tcutil_compress((void*)_check_null(e->topt));
void*_tmp285=t;void*_tmp29B;void*_tmp29A;union Cyc_Absyn_Constraint*_tmp299;union Cyc_Absyn_Constraint*_tmp298;switch(*((int*)_tmp285)){case 1: _LL1A8: _LL1A9: {
# 1449
struct Cyc_List_List*_tmp286=Cyc_Tcenv_lookup_type_vars(_tmp284);
void*_tmp287=({Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,({struct Cyc_Core_Opt*_tmp292=_cycalloc(sizeof(*_tmp292));_tmp292->v=_tmp286;_tmp292;}));});
void*_tmp288=({Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,({struct Cyc_Core_Opt*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291->v=_tmp286;_tmp291;}));});
union Cyc_Absyn_Constraint*_tmp289=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
union Cyc_Absyn_Constraint*_tmp28A=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();
struct Cyc_Absyn_PtrAtts _tmp28B=({struct Cyc_Absyn_PtrAtts _tmp290;_tmp290.rgn=_tmp288;({union Cyc_Absyn_Constraint*_tmp6B0=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();_tmp290.nullable=_tmp6B0;});_tmp290.bounds=_tmp289;_tmp290.zero_term=_tmp28A;_tmp290.ptrloc=0;_tmp290;});
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp28C=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp28D=_cycalloc(sizeof(*_tmp28D));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp6B3=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp28E;_tmp28E.tag=5;({struct Cyc_Absyn_PtrInfo _tmp6B2=({struct Cyc_Absyn_PtrInfo _tmp28F;_tmp28F.elt_typ=_tmp287;({struct Cyc_Absyn_Tqual _tmp6B1=Cyc_Absyn_empty_tqual(0);_tmp28F.elt_tq=_tmp6B1;});_tmp28F.ptr_atts=_tmp28B;_tmp28F;});_tmp28E.f1=_tmp6B2;});_tmp28E;});_tmp28D[0]=_tmp6B3;});_tmp28D;});
Cyc_Tcutil_unify(t,(void*)_tmp28C);
_tmp29B=_tmp287;_tmp29A=_tmp288;_tmp299=_tmp289;_tmp298=_tmp28A;goto _LL1AB;}case 5: _LL1AA: _tmp29B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp285)->f1).elt_typ;_tmp29A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp285)->f1).ptr_atts).rgn;_tmp299=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp285)->f1).ptr_atts).bounds;_tmp298=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp285)->f1).ptr_atts).zero_term;_LL1AB:
# 1459
 Cyc_Tcenv_check_rgn_accessible(_tmp284,loc,_tmp29A);
Cyc_Tcutil_check_nonzero_bound(loc,_tmp299);
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp29B),& Cyc_Tcutil_tmk);}) && !Cyc_Tcenv_abstract_val_ok(_tmp284))
({void*_tmp293=0;({unsigned int _tmp6B5=loc;struct _dyneither_ptr _tmp6B4=({const char*_tmp294="can't dereference abstract pointer type";_tag_dyneither(_tmp294,sizeof(char),40);});Cyc_Tcutil_terr(_tmp6B5,_tmp6B4,_tag_dyneither(_tmp293,sizeof(void*),0));});});
return _tmp29B;default: _LL1AC: _LL1AD:
# 1465
 return({struct Cyc_String_pa_PrintArg_struct _tmp297;_tmp297.tag=0;({struct _dyneither_ptr _tmp6B6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp297.f1=_tmp6B6;});({void*_tmp295[1]={& _tmp297};({struct Cyc_Tcenv_Tenv*_tmp6BA=_tmp284;unsigned int _tmp6B9=loc;void**_tmp6B8=topt;struct _dyneither_ptr _tmp6B7=({const char*_tmp296="expecting *, @, or ? type but found %s";_tag_dyneither(_tmp296,sizeof(char),39);});Cyc_Tcexp_expr_err(_tmp6BA,_tmp6B9,_tmp6B8,_tmp6B7,_tag_dyneither(_tmp295,sizeof(void*),1));});});});}_LL1A7:;};}
# 1470
static void*Cyc_Tcexp_tcAggrMember(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*outer_e,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1474
({struct Cyc_Tcenv_Tenv*_tmp6BB=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExpNoPromote(_tmp6BB,0,e);});
# 1476
({int _tmp6BC=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6BC;});{
void*_tmp29C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp29D=_tmp29C;enum Cyc_Absyn_AggrKind _tmp2B7;struct Cyc_List_List*_tmp2B6;struct Cyc_Absyn_Aggrdecl*_tmp2B5;struct Cyc_List_List*_tmp2B4;switch(*((int*)_tmp29D)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp29D)->f1).aggr_info).KnownAggr).tag == 2){_LL1AF: _tmp2B5=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp29D)->f1).aggr_info).KnownAggr).val;_tmp2B4=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp29D)->f1).targs;_LL1B0: {
# 1479
struct Cyc_Absyn_Aggrfield*_tmp29E=Cyc_Absyn_lookup_decl_field(_tmp2B5,f);
if(_tmp29E == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0;_tmp2A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0;({struct _dyneither_ptr _tmp6BD=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2B5->name));_tmp2A1.f1=_tmp6BD;});({void*_tmp29F[2]={& _tmp2A1,& _tmp2A2};({struct Cyc_Tcenv_Tenv*_tmp6C1=te;unsigned int _tmp6C0=loc;void**_tmp6BF=topt;struct _dyneither_ptr _tmp6BE=({const char*_tmp2A0="%s has no %s member";_tag_dyneither(_tmp2A0,sizeof(char),20);});Cyc_Tcexp_expr_err(_tmp6C1,_tmp6C0,_tmp6BF,_tmp6BE,_tag_dyneither(_tmp29F,sizeof(void*),2));});});});});
# 1484
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B5->impl))->tagged)*is_tagged=1;{
void*t2=_tmp29E->type;
if(_tmp2B4 != 0){
struct Cyc_List_List*_tmp2A3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2B5->tvs,_tmp2B4);
({void*_tmp6C2=Cyc_Tcutil_substitute(_tmp2A3,_tmp29E->type);t2=_tmp6C2;});}
# 1492
if((((_tmp2B5->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B5->impl))->tagged) && !Cyc_Tcutil_bits_only(t2)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp29E->requires_clause == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0;_tmp2A6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2A4[1]={& _tmp2A6};({struct Cyc_Tcenv_Tenv*_tmp6C6=te;unsigned int _tmp6C5=loc;void**_tmp6C4=topt;struct _dyneither_ptr _tmp6C3=({const char*_tmp2A5="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2A5,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6C6,_tmp6C5,_tmp6C4,_tmp6C3,_tag_dyneither(_tmp2A4,sizeof(void*),1));});});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2B5->impl))->exist_vars != 0){
# 1497
if(!({void*_tmp6C7=t2;Cyc_Tcutil_unify(_tmp6C7,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0;_tmp2A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2A7[1]={& _tmp2A9};({struct Cyc_Tcenv_Tenv*_tmp6CB=te;unsigned int _tmp6CA=loc;void**_tmp6C9=topt;struct _dyneither_ptr _tmp6C8=({const char*_tmp2A8="must use pattern-matching to access field %s\n\tdue to existential type variables.";_tag_dyneither(_tmp2A8,sizeof(char),81);});Cyc_Tcexp_expr_err(_tmp6CB,_tmp6CA,_tmp6C9,_tmp6C8,_tag_dyneither(_tmp2A7,sizeof(void*),1));});});});}
# 1500
return t2;};}}else{goto _LL1B3;}case 12: _LL1B1: _tmp2B7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29D)->f1;_tmp2B6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp29D)->f2;_LL1B2: {
# 1502
struct Cyc_Absyn_Aggrfield*_tmp2AA=Cyc_Absyn_lookup_field(_tmp2B6,f);
if(_tmp2AA == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0;_tmp2AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2AB[1]={& _tmp2AD};({struct Cyc_Tcenv_Tenv*_tmp6CF=te;unsigned int _tmp6CE=loc;void**_tmp6CD=topt;struct _dyneither_ptr _tmp6CC=({const char*_tmp2AC="type has no %s member";_tag_dyneither(_tmp2AC,sizeof(char),22);});Cyc_Tcexp_expr_err(_tmp6CF,_tmp6CE,_tmp6CD,_tmp6CC,_tag_dyneither(_tmp2AB,sizeof(void*),1));});});});
# 1507
if(((_tmp2B7 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2AA->type)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2AA->requires_clause == 0)
# 1509
return({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0;_tmp2B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2AE[1]={& _tmp2B0};({struct Cyc_Tcenv_Tenv*_tmp6D3=te;unsigned int _tmp6D2=loc;void**_tmp6D1=topt;struct _dyneither_ptr _tmp6D0=({const char*_tmp2AF="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2AF,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6D3,_tmp6D2,_tmp6D1,_tmp6D0,_tag_dyneither(_tmp2AE,sizeof(void*),1));});});});
return _tmp2AA->type;}default: _LL1B3: _LL1B4:
# 1512
 return({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0;({struct _dyneither_ptr _tmp6D4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp2B3.f1=_tmp6D4;});({void*_tmp2B1[1]={& _tmp2B3};({struct Cyc_Tcenv_Tenv*_tmp6D8=te;unsigned int _tmp6D7=loc;void**_tmp6D6=topt;struct _dyneither_ptr _tmp6D5=({const char*_tmp2B2="expecting struct or union, found %s";_tag_dyneither(_tmp2B2,sizeof(char),36);});Cyc_Tcexp_expr_err(_tmp6D8,_tmp6D7,_tmp6D6,_tmp6D5,_tag_dyneither(_tmp2B1,sizeof(void*),1));});});});}_LL1AE:;};}
# 1518
static void*Cyc_Tcexp_tcAggrArrow(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,int*is_tagged,int*is_read){
# 1521
({struct Cyc_Tcenv_Tenv*_tmp6D9=Cyc_Tcenv_enter_abstract_val_ok(Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te)));Cyc_Tcexp_tcExp(_tmp6D9,0,e);});
# 1523
({int _tmp6DA=!Cyc_Tcenv_in_notreadctxt(te);*is_read=_tmp6DA;});
{void*_tmp2B8=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2B9=_tmp2B8;void*_tmp2DC;void*_tmp2DB;union Cyc_Absyn_Constraint*_tmp2DA;union Cyc_Absyn_Constraint*_tmp2D9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B9)->tag == 5){_LL1B6: _tmp2DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B9)->f1).elt_typ;_tmp2DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B9)->f1).ptr_atts).rgn;_tmp2DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B9)->f1).ptr_atts).bounds;_tmp2D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B9)->f1).ptr_atts).zero_term;_LL1B7:
# 1526
 Cyc_Tcutil_check_nonzero_bound(loc,_tmp2DA);
{void*_tmp2BA=Cyc_Tcutil_compress(_tmp2DC);void*_tmp2BB=_tmp2BA;enum Cyc_Absyn_AggrKind _tmp2D8;struct Cyc_List_List*_tmp2D7;struct Cyc_Absyn_Aggrdecl*_tmp2D6;struct Cyc_List_List*_tmp2D5;switch(*((int*)_tmp2BB)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2BB)->f1).aggr_info).KnownAggr).tag == 2){_LL1BB: _tmp2D6=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2BB)->f1).aggr_info).KnownAggr).val;_tmp2D5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2BB)->f1).targs;_LL1BC: {
# 1529
struct Cyc_Absyn_Aggrfield*_tmp2BC=Cyc_Absyn_lookup_decl_field(_tmp2D6,f);
if(_tmp2BC == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2C0;_tmp2C0.tag=0;_tmp2C0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({struct Cyc_String_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=0;({struct _dyneither_ptr _tmp6DB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp2D6->name));_tmp2BF.f1=_tmp6DB;});({void*_tmp2BD[2]={& _tmp2BF,& _tmp2C0};({struct Cyc_Tcenv_Tenv*_tmp6DF=te;unsigned int _tmp6DE=loc;void**_tmp6DD=topt;struct _dyneither_ptr _tmp6DC=({const char*_tmp2BE="type %s has no %s member";_tag_dyneither(_tmp2BE,sizeof(char),25);});Cyc_Tcexp_expr_err(_tmp6DF,_tmp6DE,_tmp6DD,_tmp6DC,_tag_dyneither(_tmp2BD,sizeof(void*),2));});});});});
# 1534
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2D6->impl))->tagged)*is_tagged=1;{
void*t3=_tmp2BC->type;
if(_tmp2D5 != 0){
struct Cyc_List_List*_tmp2C1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp2D6->tvs,_tmp2D5);
({void*_tmp6E0=Cyc_Tcutil_substitute(_tmp2C1,_tmp2BC->type);t3=_tmp6E0;});}{
# 1540
struct Cyc_Absyn_Kind*_tmp2C2=Cyc_Tcutil_typ_kind(t3);
# 1543
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,_tmp2C2) && !Cyc_Tcenv_abstract_val_ok(te)){
void*_tmp2C3=Cyc_Tcutil_compress(t3);void*_tmp2C4=_tmp2C3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4)->tag == 8){_LL1C2: _LL1C3:
 goto _LL1C1;}else{_LL1C4: _LL1C5:
# 1547
 return({struct Cyc_String_pa_PrintArg_struct _tmp2C7;_tmp2C7.tag=0;_tmp2C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2C5[1]={& _tmp2C7};({struct Cyc_Tcenv_Tenv*_tmp6E4=te;unsigned int _tmp6E3=loc;void**_tmp6E2=topt;struct _dyneither_ptr _tmp6E1=({const char*_tmp2C6="cannot get member %s since its type is abstract";_tag_dyneither(_tmp2C6,sizeof(char),48);});Cyc_Tcexp_expr_err(_tmp6E4,_tmp6E3,_tmp6E2,_tmp6E1,_tag_dyneither(_tmp2C5,sizeof(void*),1));});});});}_LL1C1:;}
# 1552
if((((_tmp2D6->kind == Cyc_Absyn_UnionA  && !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2D6->impl))->tagged) && !
Cyc_Tcutil_bits_only(t3)) && !Cyc_Tcenv_in_notreadctxt(te)) && _tmp2BC->requires_clause == 0)
# 1555
return({struct Cyc_String_pa_PrintArg_struct _tmp2CA;_tmp2CA.tag=0;_tmp2CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2C8[1]={& _tmp2CA};({struct Cyc_Tcenv_Tenv*_tmp6E8=te;unsigned int _tmp6E7=loc;void**_tmp6E6=topt;struct _dyneither_ptr _tmp6E5=({const char*_tmp2C9="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2C9,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6E8,_tmp6E7,_tmp6E6,_tmp6E5,_tag_dyneither(_tmp2C8,sizeof(void*),1));});});});
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2D6->impl))->exist_vars != 0){
if(!({void*_tmp6E9=t3;Cyc_Tcutil_unify(_tmp6E9,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));}))
return({struct Cyc_String_pa_PrintArg_struct _tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CB[1]={& _tmp2CD};({struct Cyc_Tcenv_Tenv*_tmp6ED=te;unsigned int _tmp6EC=loc;void**_tmp6EB=topt;struct _dyneither_ptr _tmp6EA=({const char*_tmp2CC="must use pattern-matching to access field %s\n\tdue to extistential types";_tag_dyneither(_tmp2CC,sizeof(char),72);});Cyc_Tcexp_expr_err(_tmp6ED,_tmp6EC,_tmp6EB,_tmp6EA,_tag_dyneither(_tmp2CB,sizeof(void*),1));});});});}
# 1562
return t3;};};}}else{goto _LL1BF;}case 12: _LL1BD: _tmp2D8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2BB)->f1;_tmp2D7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2BB)->f2;_LL1BE: {
# 1564
struct Cyc_Absyn_Aggrfield*_tmp2CE=Cyc_Absyn_lookup_field(_tmp2D7,f);
if(_tmp2CE == 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp2D1;_tmp2D1.tag=0;_tmp2D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2CF[1]={& _tmp2D1};({struct Cyc_Tcenv_Tenv*_tmp6F1=te;unsigned int _tmp6F0=loc;void**_tmp6EF=topt;struct _dyneither_ptr _tmp6EE=({const char*_tmp2D0="type has no %s field";_tag_dyneither(_tmp2D0,sizeof(char),21);});Cyc_Tcexp_expr_err(_tmp6F1,_tmp6F0,_tmp6EF,_tmp6EE,_tag_dyneither(_tmp2CF,sizeof(void*),1));});});});
# 1569
if((_tmp2D8 == Cyc_Absyn_UnionA  && !Cyc_Tcutil_bits_only(_tmp2CE->type)) && !Cyc_Tcenv_in_notreadctxt(te))
return({struct Cyc_String_pa_PrintArg_struct _tmp2D4;_tmp2D4.tag=0;_tmp2D4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp2D2[1]={& _tmp2D4};({struct Cyc_Tcenv_Tenv*_tmp6F5=te;unsigned int _tmp6F4=loc;void**_tmp6F3=topt;struct _dyneither_ptr _tmp6F2=({const char*_tmp2D3="cannot read union member %s since it is not `bits-only'";_tag_dyneither(_tmp2D3,sizeof(char),56);});Cyc_Tcexp_expr_err(_tmp6F5,_tmp6F4,_tmp6F3,_tmp6F2,_tag_dyneither(_tmp2D2,sizeof(void*),1));});});});
return _tmp2CE->type;}default: _LL1BF: _LL1C0:
 goto _LL1BA;}_LL1BA:;}
# 1574
goto _LL1B5;}else{_LL1B8: _LL1B9:
 goto _LL1B5;}_LL1B5:;}
# 1577
return({struct Cyc_String_pa_PrintArg_struct _tmp2DF;_tmp2DF.tag=0;({struct _dyneither_ptr _tmp6F6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp2DF.f1=_tmp6F6;});({void*_tmp2DD[1]={& _tmp2DF};({struct Cyc_Tcenv_Tenv*_tmp6FA=te;unsigned int _tmp6F9=loc;void**_tmp6F8=topt;struct _dyneither_ptr _tmp6F7=({const char*_tmp2DE="expecting struct or union pointer, found %s";_tag_dyneither(_tmp2DE,sizeof(char),44);});Cyc_Tcexp_expr_err(_tmp6FA,_tmp6F9,_tmp6F8,_tmp6F7,_tag_dyneither(_tmp2DD,sizeof(void*),1));});});});}
# 1582
static void*Cyc_Tcexp_ithTupleType(struct Cyc_Tcenv_Tenv*te,unsigned int loc,struct Cyc_List_List*ts,struct Cyc_Absyn_Exp*index){
# 1584
struct _tuple15 _tmp2E0=Cyc_Evexp_eval_const_uint_exp(index);struct _tuple15 _tmp2E1=_tmp2E0;unsigned int _tmp2EF;int _tmp2EE;_LL1C7: _tmp2EF=_tmp2E1.f1;_tmp2EE=_tmp2E1.f2;_LL1C8:;
if(!_tmp2EE)
return({void*_tmp2E2=0;({struct Cyc_Tcenv_Tenv*_tmp6FD=te;unsigned int _tmp6FC=loc;struct _dyneither_ptr _tmp6FB=({const char*_tmp2E3="tuple projection cannot use sizeof or offsetof";_tag_dyneither(_tmp2E3,sizeof(char),47);});Cyc_Tcexp_expr_err(_tmp6FD,_tmp6FC,0,_tmp6FB,_tag_dyneither(_tmp2E2,sizeof(void*),0));});});{
# 1588
struct _handler_cons _tmp2E4;_push_handler(& _tmp2E4);{int _tmp2E6=0;if(setjmp(_tmp2E4.handler))_tmp2E6=1;if(!_tmp2E6){
{void*_tmp2E7=(*((struct _tuple17*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(ts,(int)_tmp2EF)).f2;_npop_handler(0);return _tmp2E7;};_pop_handler();}else{void*_tmp2E5=(void*)_exn_thrown;void*_tmp2E8=_tmp2E5;void*_tmp2ED;if(((struct Cyc_List_Nth_exn_struct*)_tmp2E8)->tag == Cyc_List_Nth){_LL1CA: _LL1CB:
# 1591
 return({struct Cyc_Int_pa_PrintArg_struct _tmp2EC;_tmp2EC.tag=1;({unsigned long _tmp6FE=(unsigned long)
((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts);_tmp2EC.f1=_tmp6FE;});({struct Cyc_Int_pa_PrintArg_struct _tmp2EB;_tmp2EB.tag=1;_tmp2EB.f1=(unsigned long)((int)_tmp2EF);({void*_tmp2E9[2]={& _tmp2EB,& _tmp2EC};({struct Cyc_Tcenv_Tenv*_tmp701=te;unsigned int _tmp700=loc;struct _dyneither_ptr _tmp6FF=({const char*_tmp2EA="index is %d but tuple has only %d fields";_tag_dyneither(_tmp2EA,sizeof(char),41);});Cyc_Tcexp_expr_err(_tmp701,_tmp700,0,_tmp6FF,_tag_dyneither(_tmp2E9,sizeof(void*),2));});});});});}else{_LL1CC: _tmp2ED=_tmp2E8;_LL1CD:(int)_rethrow(_tmp2ED);}_LL1C9:;}};};}
# 1596
static void*Cyc_Tcexp_tcSubscript(struct Cyc_Tcenv_Tenv*te_orig,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 1598
struct Cyc_Tcenv_Tenv*_tmp2F0=Cyc_Tcenv_clear_lhs(Cyc_Tcenv_clear_notreadctxt(te_orig));
({struct Cyc_Tcenv_Tenv*_tmp702=Cyc_Tcenv_clear_abstract_val_ok(_tmp2F0);Cyc_Tcexp_tcExp(_tmp702,0,e1);});
({struct Cyc_Tcenv_Tenv*_tmp703=Cyc_Tcenv_clear_abstract_val_ok(_tmp2F0);Cyc_Tcexp_tcExp(_tmp703,0,e2);});{
void*t1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*t2=Cyc_Tcutil_compress((void*)_check_null(e2->topt));
if(!Cyc_Tcutil_coerce_sint_typ(_tmp2F0,e2))
return({struct Cyc_String_pa_PrintArg_struct _tmp2F3;_tmp2F3.tag=0;({struct _dyneither_ptr _tmp704=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp2F3.f1=_tmp704;});({void*_tmp2F1[1]={& _tmp2F3};({struct Cyc_Tcenv_Tenv*_tmp708=_tmp2F0;unsigned int _tmp707=e2->loc;void**_tmp706=topt;struct _dyneither_ptr _tmp705=({const char*_tmp2F2="expecting int subscript, found %s";_tag_dyneither(_tmp2F2,sizeof(char),34);});Cyc_Tcexp_expr_err(_tmp708,_tmp707,_tmp706,_tmp705,_tag_dyneither(_tmp2F1,sizeof(void*),1));});});});{
# 1608
void*_tmp2F4=t1;struct Cyc_List_List*_tmp312;void*_tmp311;struct Cyc_Absyn_Tqual _tmp310;void*_tmp30F;union Cyc_Absyn_Constraint*_tmp30E;union Cyc_Absyn_Constraint*_tmp30D;switch(*((int*)_tmp2F4)){case 5: _LL1CF: _tmp311=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).elt_typ;_tmp310=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).elt_tq;_tmp30F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).ptr_atts).rgn;_tmp30E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).ptr_atts).bounds;_tmp30D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F4)->f1).ptr_atts).zero_term;_LL1D0:
# 1612
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp30D)){
int emit_warning=1;
{void*_tmp2F5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp30E);void*_tmp2F6=_tmp2F5;struct Cyc_Absyn_Exp*_tmp2FF;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2F6)->tag == 1){_LL1D6: _tmp2FF=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp2F6)->f1;_LL1D7:
# 1616
 if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp2F7=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp2F8=_tmp2F7;unsigned int _tmp2FE;int _tmp2FD;_LL1DB: _tmp2FE=_tmp2F8.f1;_tmp2FD=_tmp2F8.f2;_LL1DC:;
if(_tmp2FD){
struct _tuple15 _tmp2F9=Cyc_Evexp_eval_const_uint_exp(_tmp2FF);struct _tuple15 _tmp2FA=_tmp2F9;unsigned int _tmp2FC;int _tmp2FB;_LL1DE: _tmp2FC=_tmp2FA.f1;_tmp2FB=_tmp2FA.f2;_LL1DF:;
if(_tmp2FB  && _tmp2FC > _tmp2FE)emit_warning=0;}}
# 1623
goto _LL1D5;}else{_LL1D8: _LL1D9:
 emit_warning=0;goto _LL1D5;}_LL1D5:;}
# 1626
if(emit_warning)
({void*_tmp300=0;({unsigned int _tmp70A=e2->loc;struct _dyneither_ptr _tmp709=({const char*_tmp301="subscript on thin, zero-terminated pointer could be expensive.";_tag_dyneither(_tmp301,sizeof(char),63);});Cyc_Tcutil_warn(_tmp70A,_tmp709,_tag_dyneither(_tmp300,sizeof(void*),0));});});}else{
# 1630
if(Cyc_Tcutil_is_const_exp(e2)){
struct _tuple15 _tmp302=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple15 _tmp303=_tmp302;unsigned int _tmp305;int _tmp304;_LL1E1: _tmp305=_tmp303.f1;_tmp304=_tmp303.f2;_LL1E2:;
if(_tmp304)
Cyc_Tcutil_check_bound(loc,_tmp305,_tmp30E);}else{
# 1636
if(Cyc_Tcutil_is_bound_one(_tmp30E) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp30D))
({void*_tmp306=0;({unsigned int _tmp70C=e1->loc;struct _dyneither_ptr _tmp70B=({const char*_tmp307="subscript applied to pointer to one object";_tag_dyneither(_tmp307,sizeof(char),43);});Cyc_Tcutil_warn(_tmp70C,_tmp70B,_tag_dyneither(_tmp306,sizeof(void*),0));});});
Cyc_Tcutil_check_nonzero_bound(loc,_tmp30E);}}
# 1641
Cyc_Tcenv_check_rgn_accessible(_tmp2F0,loc,_tmp30F);
if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(_tmp311),& Cyc_Tcutil_tmk);}) && !Cyc_Tcenv_abstract_val_ok(_tmp2F0))
({void*_tmp308=0;({unsigned int _tmp70E=e1->loc;struct _dyneither_ptr _tmp70D=({const char*_tmp309="can't subscript an abstract pointer";_tag_dyneither(_tmp309,sizeof(char),36);});Cyc_Tcutil_terr(_tmp70E,_tmp70D,_tag_dyneither(_tmp308,sizeof(void*),0));});});
return _tmp311;case 10: _LL1D1: _tmp312=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2F4)->f1;_LL1D2:
 return Cyc_Tcexp_ithTupleType(_tmp2F0,loc,_tmp312,e2);default: _LL1D3: _LL1D4:
 return({struct Cyc_String_pa_PrintArg_struct _tmp30C;_tmp30C.tag=0;({struct _dyneither_ptr _tmp70F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp30C.f1=_tmp70F;});({void*_tmp30A[1]={& _tmp30C};({struct Cyc_Tcenv_Tenv*_tmp713=_tmp2F0;unsigned int _tmp712=loc;void**_tmp711=topt;struct _dyneither_ptr _tmp710=({const char*_tmp30B="subscript applied to %s";_tag_dyneither(_tmp30B,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp713,_tmp712,_tmp711,_tmp710,_tag_dyneither(_tmp30A,sizeof(void*),1));});});});}_LL1CE:;};};}
# 1651
static void*Cyc_Tcexp_tcTuple(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_List_List*es){
int done=0;
struct Cyc_List_List*fields=0;
if(topt != 0){
void*_tmp313=Cyc_Tcutil_compress(*topt);void*_tmp314=_tmp313;struct Cyc_List_List*_tmp318;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp314)->tag == 10){_LL1E4: _tmp318=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp314)->f1;_LL1E5:
# 1657
 if(({int _tmp714=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp318);_tmp714 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es);}))
# 1660
goto _LL1E3;
# 1662
for(0;es != 0;(es=es->tl,_tmp318=_tmp318->tl)){
int bogus=0;
void*_tmp315=(*((struct _tuple17*)((struct Cyc_List_List*)_check_null(_tmp318))->hd)).f2;
({struct Cyc_Tcenv_Tenv*_tmp715=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp715,& _tmp315,(struct Cyc_Absyn_Exp*)es->hd);});
# 1667
Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)es->hd,(*((struct _tuple17*)_tmp318->hd)).f2,& bogus);
({struct Cyc_List_List*_tmp717=({struct Cyc_List_List*_tmp316=_cycalloc(sizeof(*_tmp316));({struct _tuple17*_tmp716=({struct _tuple17*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->f1=(*((struct _tuple17*)_tmp318->hd)).f1;_tmp317->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp317;});_tmp316->hd=_tmp716;});_tmp316->tl=fields;_tmp316;});fields=_tmp717;});}
# 1670
done=1;
goto _LL1E3;}else{_LL1E6: _LL1E7:
 goto _LL1E3;}_LL1E3:;}
# 1674
if(!done)
for(0;es != 0;es=es->tl){
({struct Cyc_Tcenv_Tenv*_tmp718=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExpInitializer(_tmp718,0,(struct Cyc_Absyn_Exp*)es->hd);});
({struct Cyc_List_List*_tmp71B=({struct Cyc_List_List*_tmp319=_cycalloc(sizeof(*_tmp319));({struct _tuple17*_tmp71A=({struct _tuple17*_tmp31A=_cycalloc(sizeof(*_tmp31A));({struct Cyc_Absyn_Tqual _tmp719=Cyc_Absyn_empty_tqual(0);_tmp31A->f1=_tmp719;});_tmp31A->f2=(void*)_check_null(((struct Cyc_Absyn_Exp*)es->hd)->topt);_tmp31A;});_tmp319->hd=_tmp71A;});_tmp319->tl=fields;_tmp319;});fields=_tmp71B;});}
# 1679
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp31B=_cycalloc(sizeof(*_tmp31B));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp71D=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp31C;_tmp31C.tag=10;({struct Cyc_List_List*_tmp71C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp31C.f1=_tmp71C;});_tmp31C;});_tmp31B[0]=_tmp71D;});_tmp31B;});}
# 1683
static void*Cyc_Tcexp_tcCompoundLit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple9*t,struct Cyc_List_List*des){
# 1686
return({void*_tmp31D=0;({struct Cyc_Tcenv_Tenv*_tmp721=te;unsigned int _tmp720=loc;void**_tmp71F=topt;struct _dyneither_ptr _tmp71E=({const char*_tmp31E="tcCompoundLit";_tag_dyneither(_tmp31E,sizeof(char),14);});Cyc_Tcexp_expr_err(_tmp721,_tmp720,_tmp71F,_tmp71E,_tag_dyneither(_tmp31D,sizeof(void*),0));});});}struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 1696 "tcexp.cyc"
static void*Cyc_Tcexp_tcArray(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**elt_topt,struct Cyc_Absyn_Tqual*elt_tqopt,int zero_term,struct Cyc_List_List*des){
# 1699
void*res_t2;
int _tmp31F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(des);
struct Cyc_List_List*es=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(struct _tuple18*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Exp*(*)(struct _tuple18*))Cyc_Core_snd,des);
void*res=({Cyc_Absyn_new_evar(& Cyc_Tcutil_tmko,Cyc_Tcenv_lookup_opt_type_vars(te));});
struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp320=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp336=_cycalloc(sizeof(*_tmp336));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp723=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp337;_tmp337.tag=0;({union Cyc_Absyn_Cnst _tmp722=Cyc_Absyn_Int_c(Cyc_Absyn_Unsigned,_tmp31F);_tmp337.f1=_tmp722;});_tmp337;});_tmp336[0]=_tmp723;});_tmp336;});
struct Cyc_Absyn_Exp*sz_exp=Cyc_Absyn_new_exp((void*)_tmp320,loc);
# 1707
if(zero_term){
struct Cyc_Absyn_Exp*_tmp321=((struct Cyc_Absyn_Exp*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(es,_tmp31F - 1);
if(!Cyc_Tcutil_is_zero(_tmp321))
({void*_tmp322=0;({unsigned int _tmp725=_tmp321->loc;struct _dyneither_ptr _tmp724=({const char*_tmp323="zero-terminated array doesn't end with zero.";_tag_dyneither(_tmp323,sizeof(char),45);});Cyc_Tcutil_terr(_tmp725,_tmp724,_tag_dyneither(_tmp322,sizeof(void*),0));});});}
# 1712
sz_exp->topt=Cyc_Absyn_uint_typ;
({void*_tmp729=({void*_tmp728=res;struct Cyc_Absyn_Tqual _tmp727=
(unsigned int)elt_tqopt?*elt_tqopt: Cyc_Absyn_empty_tqual(0);
# 1713
struct Cyc_Absyn_Exp*_tmp726=sz_exp;Cyc_Absyn_array_typ(_tmp728,_tmp727,_tmp726,
# 1715
zero_term?Cyc_Absyn_true_conref: Cyc_Absyn_false_conref,0);});
# 1713
res_t2=_tmp729;});
# 1717
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
Cyc_Tcexp_tcExpInitializer(te,elt_topt,(struct Cyc_Absyn_Exp*)es2->hd);}}
# 1720
if(!Cyc_Tcutil_coerce_list(te,res,es))
# 1722
({struct Cyc_String_pa_PrintArg_struct _tmp326;_tmp326.tag=0;({struct _dyneither_ptr _tmp72A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(res));_tmp326.f1=_tmp72A;});({void*_tmp324[1]={& _tmp326};({unsigned int _tmp72C=((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd)->loc;struct _dyneither_ptr _tmp72B=({const char*_tmp325="elements of array do not all have the same type (%s)";_tag_dyneither(_tmp325,sizeof(char),53);});Cyc_Tcutil_terr(_tmp72C,_tmp72B,_tag_dyneither(_tmp324,sizeof(void*),1));});});});
# 1725
{int offset=0;for(0;des != 0;(offset ++,des=des->tl)){
struct Cyc_List_List*ds=(*((struct _tuple18*)des->hd)).f1;
if(ds != 0){
# 1730
void*_tmp327=(void*)ds->hd;void*_tmp328=_tmp327;struct Cyc_Absyn_Exp*_tmp335;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp328)->tag == 1){_LL1E9: _LL1EA:
# 1732
({void*_tmp329=0;({unsigned int _tmp72E=loc;struct _dyneither_ptr _tmp72D=({const char*_tmp32A="only array index designators are supported";_tag_dyneither(_tmp32A,sizeof(char),43);});Cyc_Tcutil_terr(_tmp72E,_tmp72D,_tag_dyneither(_tmp329,sizeof(void*),0));});});
goto _LL1E8;}else{_LL1EB: _tmp335=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp328)->f1;_LL1EC:
# 1735
 Cyc_Tcexp_tcExpInitializer(te,0,_tmp335);{
struct _tuple15 _tmp32B=Cyc_Evexp_eval_const_uint_exp(_tmp335);struct _tuple15 _tmp32C=_tmp32B;unsigned int _tmp334;int _tmp333;_LL1EE: _tmp334=_tmp32C.f1;_tmp333=_tmp32C.f2;_LL1EF:;
if(!_tmp333)
({void*_tmp32D=0;({unsigned int _tmp730=_tmp335->loc;struct _dyneither_ptr _tmp72F=({const char*_tmp32E="index designator cannot use sizeof or offsetof";_tag_dyneither(_tmp32E,sizeof(char),47);});Cyc_Tcutil_terr(_tmp730,_tmp72F,_tag_dyneither(_tmp32D,sizeof(void*),0));});});else{
if(_tmp334 != offset)
({struct Cyc_Int_pa_PrintArg_struct _tmp332;_tmp332.tag=1;_tmp332.f1=(unsigned long)((int)_tmp334);({struct Cyc_Int_pa_PrintArg_struct _tmp331;_tmp331.tag=1;_tmp331.f1=(unsigned long)offset;({void*_tmp32F[2]={& _tmp331,& _tmp332};({unsigned int _tmp732=_tmp335->loc;struct _dyneither_ptr _tmp731=({const char*_tmp330="expecting index designator %d but found %d";_tag_dyneither(_tmp330,sizeof(char),43);});Cyc_Tcutil_terr(_tmp732,_tmp731,_tag_dyneither(_tmp32F,sizeof(void*),2));});});});});}
# 1742
goto _LL1E8;};}_LL1E8:;}}}
# 1746
return res_t2;}
# 1750
static void*Cyc_Tcexp_tcComprehension(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Exp*bound,struct Cyc_Absyn_Exp*body,int*is_zero_term){
# 1753
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp338=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp339=_tmp338;void*_tmp33E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp339)->tag == 19){_LL1F1: _tmp33E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp339)->f1;_LL1F2:
# 1758
 if(({Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr;})){
struct Cyc_Absyn_Exp*_tmp33A=({void*_tmp733=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp733,Cyc_Absyn_valueof_exp(_tmp33E,0),0,Cyc_Absyn_No_coercion,0);});
_tmp33A->topt=bound->topt;
# 1763
bound=_tmp33A;}
# 1765
goto _LL1F0;}else{_LL1F3: _LL1F4:
# 1767
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp33D;_tmp33D.tag=0;({struct _dyneither_ptr _tmp734=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp33D.f1=_tmp734;});({void*_tmp33B[1]={& _tmp33D};({unsigned int _tmp736=bound->loc;struct _dyneither_ptr _tmp735=({const char*_tmp33C="expecting unsigned int, found %s";_tag_dyneither(_tmp33C,sizeof(char),33);});Cyc_Tcutil_terr(_tmp736,_tmp735,_tag_dyneither(_tmp33B,sizeof(void*),1));});});});}_LL1F0:;}
# 1772
if(!(vd->tq).real_const)
({void*_tmp33F=0;({struct _dyneither_ptr _tmp737=({const char*_tmp340="comprehension index variable is not declared const!";_tag_dyneither(_tmp340,sizeof(char),52);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp737,_tag_dyneither(_tmp33F,sizeof(void*),0));});});{
# 1775
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_copy_tenv(te);
if(te2->le != 0)
({struct Cyc_Tcenv_Tenv*_tmp738=Cyc_Tcenv_new_block(loc,te2);te2=_tmp738;});{
struct Cyc_Tcenv_Tenv*_tmp341=te2;
void**_tmp342=0;
struct Cyc_Absyn_Tqual*_tmp343=0;
union Cyc_Absyn_Constraint**_tmp344=0;
# 1783
if(topt != 0){
void*_tmp345=Cyc_Tcutil_compress(*topt);void*_tmp346=_tmp345;void*_tmp351;struct Cyc_Absyn_Tqual _tmp350;struct Cyc_Absyn_Exp*_tmp34F;union Cyc_Absyn_Constraint*_tmp34E;struct Cyc_Absyn_PtrInfo _tmp34D;switch(*((int*)_tmp346)){case 5: _LL1F6: _tmp34D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp346)->f1;_LL1F7:
# 1786
({void**_tmp739=({void**_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347[0]=_tmp34D.elt_typ;_tmp347;});_tmp342=_tmp739;});
({struct Cyc_Absyn_Tqual*_tmp73A=({struct Cyc_Absyn_Tqual*_tmp348=_cycalloc_atomic(sizeof(*_tmp348));_tmp348[0]=_tmp34D.elt_tq;_tmp348;});_tmp343=_tmp73A;});
({union Cyc_Absyn_Constraint**_tmp73B=({union Cyc_Absyn_Constraint**_tmp349=_cycalloc(sizeof(*_tmp349));_tmp349[0]=(_tmp34D.ptr_atts).zero_term;_tmp349;});_tmp344=_tmp73B;});
goto _LL1F5;case 8: _LL1F8: _tmp351=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp346)->f1).elt_type;_tmp350=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp346)->f1).tq;_tmp34F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp346)->f1).num_elts;_tmp34E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp346)->f1).zero_term;_LL1F9:
# 1791
({void**_tmp73C=({void**_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A[0]=_tmp351;_tmp34A;});_tmp342=_tmp73C;});
({struct Cyc_Absyn_Tqual*_tmp73D=({struct Cyc_Absyn_Tqual*_tmp34B=_cycalloc_atomic(sizeof(*_tmp34B));_tmp34B[0]=_tmp350;_tmp34B;});_tmp343=_tmp73D;});
({union Cyc_Absyn_Constraint**_tmp73E=({union Cyc_Absyn_Constraint**_tmp34C=_cycalloc(sizeof(*_tmp34C));_tmp34C[0]=_tmp34E;_tmp34C;});_tmp344=_tmp73E;});
goto _LL1F5;default: _LL1FA: _LL1FB:
# 1796
 goto _LL1F5;}_LL1F5:;}{
# 1799
void*t=Cyc_Tcexp_tcExp(_tmp341,_tmp342,body);
# 1801
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(body))
({void*_tmp352=0;({unsigned int _tmp740=body->loc;struct _dyneither_ptr _tmp73F=({const char*_tmp353="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp353,sizeof(char),49);});Cyc_Tcutil_terr(_tmp740,_tmp73F,_tag_dyneither(_tmp352,sizeof(void*),0));});});
if(_tmp341->le == 0){
# 1805
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp354=0;({unsigned int _tmp742=bound->loc;struct _dyneither_ptr _tmp741=({const char*_tmp355="bound is not constant";_tag_dyneither(_tmp355,sizeof(char),22);});Cyc_Tcutil_terr(_tmp742,_tmp741,_tag_dyneither(_tmp354,sizeof(void*),0));});});
if(!Cyc_Tcutil_is_const_exp(body))
({void*_tmp356=0;({unsigned int _tmp744=bound->loc;struct _dyneither_ptr _tmp743=({const char*_tmp357="body is not constant";_tag_dyneither(_tmp357,sizeof(char),21);});Cyc_Tcutil_terr(_tmp744,_tmp743,_tag_dyneither(_tmp356,sizeof(void*),0));});});}
# 1810
if(_tmp344 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp344)){
# 1813
struct Cyc_Absyn_Exp*_tmp358=Cyc_Absyn_uint_exp(1,0);_tmp358->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp745=Cyc_Absyn_add_exp(bound,_tmp358,0);bound=_tmp745;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;}
# 1817
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(body->topt)) && !
Cyc_Tcutil_is_noalias_path(body))
({void*_tmp359=0;({unsigned int _tmp747=body->loc;struct _dyneither_ptr _tmp746=({const char*_tmp35A="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp35A,sizeof(char),49);});Cyc_Tcutil_terr(_tmp747,_tmp746,_tag_dyneither(_tmp359,sizeof(void*),0));});});{
# 1821
void*_tmp35B=({void*_tmp74A=t;struct Cyc_Absyn_Tqual _tmp749=_tmp343 == 0?Cyc_Absyn_empty_tqual(0):*_tmp343;struct Cyc_Absyn_Exp*_tmp748=bound;Cyc_Absyn_array_typ(_tmp74A,_tmp749,_tmp748,
_tmp344 == 0?Cyc_Absyn_false_conref:*_tmp344,0);});
return _tmp35B;};};};};}
# 1827
static void*Cyc_Tcexp_tcComprehensionNoinit(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*bound,void*t,int*is_zero_term){
# 1830
Cyc_Tcexp_tcExp(te,0,bound);
{void*_tmp35C=Cyc_Tcutil_compress((void*)_check_null(bound->topt));void*_tmp35D=_tmp35C;void*_tmp362;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp35D)->tag == 19){_LL1FD: _tmp362=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp35D)->f1;_LL1FE:
# 1835
 if(({Cyc_Tcenv_new_status(te)== Cyc_Tcenv_InNewAggr;})){
struct Cyc_Absyn_Exp*_tmp35E=({void*_tmp74B=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp74B,Cyc_Absyn_valueof_exp(_tmp362,0),0,Cyc_Absyn_No_coercion,0);});
_tmp35E->topt=bound->topt;
# 1840
bound=_tmp35E;}
# 1842
goto _LL1FC;}else{_LL1FF: _LL200:
# 1844
 if(!Cyc_Tcutil_coerce_uint_typ(te,bound))
({struct Cyc_String_pa_PrintArg_struct _tmp361;_tmp361.tag=0;({struct _dyneither_ptr _tmp74C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(bound->topt)));_tmp361.f1=_tmp74C;});({void*_tmp35F[1]={& _tmp361};({unsigned int _tmp74E=bound->loc;struct _dyneither_ptr _tmp74D=({const char*_tmp360="expecting unsigned int, found %s";_tag_dyneither(_tmp360,sizeof(char),33);});Cyc_Tcutil_terr(_tmp74E,_tmp74D,_tag_dyneither(_tmp35F,sizeof(void*),1));});});});}_LL1FC:;}{
# 1849
void**_tmp363=0;
struct Cyc_Absyn_Tqual*_tmp364=0;
union Cyc_Absyn_Constraint**_tmp365=0;
# 1853
if(topt != 0){
void*_tmp366=Cyc_Tcutil_compress(*topt);void*_tmp367=_tmp366;void*_tmp372;struct Cyc_Absyn_Tqual _tmp371;struct Cyc_Absyn_Exp*_tmp370;union Cyc_Absyn_Constraint*_tmp36F;struct Cyc_Absyn_PtrInfo _tmp36E;switch(*((int*)_tmp367)){case 5: _LL202: _tmp36E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp367)->f1;_LL203:
# 1856
({void**_tmp74F=({void**_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368[0]=_tmp36E.elt_typ;_tmp368;});_tmp363=_tmp74F;});
({struct Cyc_Absyn_Tqual*_tmp750=({struct Cyc_Absyn_Tqual*_tmp369=_cycalloc_atomic(sizeof(*_tmp369));_tmp369[0]=_tmp36E.elt_tq;_tmp369;});_tmp364=_tmp750;});
({union Cyc_Absyn_Constraint**_tmp751=({union Cyc_Absyn_Constraint**_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A[0]=(_tmp36E.ptr_atts).zero_term;_tmp36A;});_tmp365=_tmp751;});
goto _LL201;case 8: _LL204: _tmp372=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp367)->f1).elt_type;_tmp371=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp367)->f1).tq;_tmp370=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp367)->f1).num_elts;_tmp36F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp367)->f1).zero_term;_LL205:
# 1861
({void**_tmp752=({void**_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B[0]=_tmp372;_tmp36B;});_tmp363=_tmp752;});
({struct Cyc_Absyn_Tqual*_tmp753=({struct Cyc_Absyn_Tqual*_tmp36C=_cycalloc_atomic(sizeof(*_tmp36C));_tmp36C[0]=_tmp371;_tmp36C;});_tmp364=_tmp753;});
({union Cyc_Absyn_Constraint**_tmp754=({union Cyc_Absyn_Constraint**_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D[0]=_tmp36F;_tmp36D;});_tmp365=_tmp754;});
goto _LL201;default: _LL206: _LL207:
# 1866
 goto _LL201;}_LL201:;}
# 1869
({unsigned int _tmp757=loc;struct Cyc_Tcenv_Tenv*_tmp756=te;struct Cyc_List_List*_tmp755=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp757,_tmp756,_tmp755,& Cyc_Tcutil_tmk,1,1,t);});
if(_tmp363 != 0)Cyc_Tcutil_unify(*_tmp363,t);
# 1872
if(te->le == 0){
if(!Cyc_Tcutil_is_const_exp(bound))
({void*_tmp373=0;({unsigned int _tmp759=bound->loc;struct _dyneither_ptr _tmp758=({const char*_tmp374="bound is not constant";_tag_dyneither(_tmp374,sizeof(char),22);});Cyc_Tcutil_terr(_tmp759,_tmp758,_tag_dyneither(_tmp373,sizeof(void*),0));});});}
# 1876
if(_tmp365 != 0  && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,*_tmp365)){
# 1879
struct Cyc_Absyn_Exp*_tmp375=Cyc_Absyn_uint_exp(1,0);_tmp375->topt=Cyc_Absyn_uint_typ;
({struct Cyc_Absyn_Exp*_tmp75A=Cyc_Absyn_add_exp(bound,_tmp375,0);bound=_tmp75A;});bound->topt=Cyc_Absyn_uint_typ;
*is_zero_term=1;
# 1883
({void*_tmp376=0;({unsigned int _tmp75C=loc;struct _dyneither_ptr _tmp75B=({const char*_tmp377="non-initializing comprehensions do not currently support @zeroterm arrays";_tag_dyneither(_tmp377,sizeof(char),74);});Cyc_Tcutil_terr(_tmp75C,_tmp75B,_tag_dyneither(_tmp376,sizeof(void*),0));});});}{
# 1886
void*_tmp378=({void*_tmp75F=t;struct Cyc_Absyn_Tqual _tmp75E=_tmp364 == 0?Cyc_Absyn_empty_tqual(0):*_tmp364;struct Cyc_Absyn_Exp*_tmp75D=bound;Cyc_Absyn_array_typ(_tmp75F,_tmp75E,_tmp75D,
_tmp365 == 0?Cyc_Absyn_false_conref:*_tmp365,0);});
return _tmp378;};};}struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1901 "tcexp.cyc"
static void*Cyc_Tcexp_tcAggregate(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct _tuple1**tn,struct Cyc_List_List**ts,struct Cyc_List_List*args,struct Cyc_Absyn_Aggrdecl**ad_opt){
# 1905
struct Cyc_Absyn_Aggrdecl**adptr;
struct Cyc_Absyn_Aggrdecl*ad;
if(*ad_opt != 0){
ad=(struct Cyc_Absyn_Aggrdecl*)_check_null(*ad_opt);
({struct Cyc_Absyn_Aggrdecl**_tmp760=({struct Cyc_Absyn_Aggrdecl**_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379[0]=ad;_tmp379;});adptr=_tmp760;});}else{
# 1911
{struct _handler_cons _tmp37A;_push_handler(& _tmp37A);{int _tmp37C=0;if(setjmp(_tmp37A.handler))_tmp37C=1;if(!_tmp37C){({struct Cyc_Absyn_Aggrdecl**_tmp761=Cyc_Tcenv_lookup_aggrdecl(te,loc,*tn);adptr=_tmp761;});
ad=*adptr;
# 1911
;_pop_handler();}else{void*_tmp37B=(void*)_exn_thrown;void*_tmp37D=_tmp37B;void*_tmp381;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp37D)->tag == Cyc_Dict_Absent){_LL209: _LL20A:
# 1914
({struct Cyc_String_pa_PrintArg_struct _tmp380;_tmp380.tag=0;({struct _dyneither_ptr _tmp762=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp380.f1=_tmp762;});({void*_tmp37E[1]={& _tmp380};({unsigned int _tmp764=loc;struct _dyneither_ptr _tmp763=({const char*_tmp37F="unbound struct/union name %s";_tag_dyneither(_tmp37F,sizeof(char),29);});Cyc_Tcutil_terr(_tmp764,_tmp763,_tag_dyneither(_tmp37E,sizeof(void*),1));});});});
return topt != 0?*topt:(void*)& Cyc_Absyn_VoidType_val;}else{_LL20B: _tmp381=_tmp37D;_LL20C:(int)_rethrow(_tmp381);}_LL208:;}};}
# 1917
*ad_opt=ad;
*tn=ad->name;}
# 1920
if(ad->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmp384;_tmp384.tag=0;({struct _dyneither_ptr _tmp765=(struct _dyneither_ptr)(ad->kind == Cyc_Absyn_StructA?({const char*_tmp385="struct";_tag_dyneither(_tmp385,sizeof(char),7);}):({const char*_tmp386="union";_tag_dyneither(_tmp386,sizeof(char),6);}));_tmp384.f1=_tmp765;});({void*_tmp382[1]={& _tmp384};({unsigned int _tmp767=loc;struct _dyneither_ptr _tmp766=({const char*_tmp383="can't construct abstract %s";_tag_dyneither(_tmp383,sizeof(char),28);});Cyc_Tcutil_terr(_tmp767,_tmp766,_tag_dyneither(_tmp382,sizeof(void*),1));});});});
return Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));}{
# 1928
struct Cyc_Tcenv_Tenv*te2;
enum Cyc_Tcenv_NewStatus _tmp387=Cyc_Tcenv_new_status(te);
if(_tmp387 == Cyc_Tcenv_InNew)
({struct Cyc_Tcenv_Tenv*_tmp768=Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNewAggr,te);te2=_tmp768;});else{
# 1937
({struct Cyc_Tcenv_Tenv*_tmp769=Cyc_Tcenv_set_new_status(_tmp387,te);te2=_tmp769;});}{
# 1939
struct _tuple12 _tmp388=({struct _tuple12 _tmp3AF;({struct Cyc_List_List*_tmp76A=Cyc_Tcenv_lookup_type_vars(te2);_tmp3AF.f1=_tmp76A;});_tmp3AF.f2=Cyc_Core_heap_region;_tmp3AF;});
struct Cyc_List_List*_tmp389=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp388,ad->tvs);
struct Cyc_List_List*_tmp38A=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp388,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars);
struct Cyc_List_List*_tmp38B=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp389);
struct Cyc_List_List*_tmp38C=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp38A);
struct Cyc_List_List*_tmp38D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp389,_tmp38A);
void*res_typ;
# 1950
if(topt != 0){
void*_tmp38E=Cyc_Tcutil_compress(*topt);void*_tmp38F=_tmp38E;struct Cyc_Absyn_Aggrdecl**_tmp395;struct Cyc_List_List*_tmp394;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38F)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38F)->f1).aggr_info).KnownAggr).tag == 2){_LL20E: _tmp395=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38F)->f1).aggr_info).KnownAggr).val;_tmp394=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp38F)->f1).targs;_LL20F:
# 1953
 if(*_tmp395 == *adptr){
{struct Cyc_List_List*_tmp390=_tmp38B;for(0;_tmp390 != 0  && _tmp394 != 0;
(_tmp390=_tmp390->tl,_tmp394=_tmp394->tl)){
Cyc_Tcutil_unify((void*)_tmp390->hd,(void*)_tmp394->hd);}}
# 1958
res_typ=*topt;
goto _LL20D;}
# 1961
goto _LL211;}else{goto _LL210;}}else{_LL210: _LL211:
# 1963
({void*_tmp76E=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp391=_cycalloc(sizeof(*_tmp391));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp76D=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp392;_tmp392.tag=11;({struct Cyc_Absyn_AggrInfo _tmp76C=({struct Cyc_Absyn_AggrInfo _tmp393;({union Cyc_Absyn_AggrInfoU _tmp76B=Cyc_Absyn_KnownAggr(adptr);_tmp393.aggr_info=_tmp76B;});_tmp393.targs=_tmp38B;_tmp393;});_tmp392.f1=_tmp76C;});_tmp392;});_tmp391[0]=_tmp76D;});_tmp391;});res_typ=_tmp76E;});}_LL20D:;}else{
# 1966
({void*_tmp772=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp396=_cycalloc(sizeof(*_tmp396));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp771=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp397;_tmp397.tag=11;({struct Cyc_Absyn_AggrInfo _tmp770=({struct Cyc_Absyn_AggrInfo _tmp398;({union Cyc_Absyn_AggrInfoU _tmp76F=Cyc_Absyn_KnownAggr(adptr);_tmp398.aggr_info=_tmp76F;});_tmp398.targs=_tmp38B;_tmp398;});_tmp397.f1=_tmp770;});_tmp397;});_tmp396[0]=_tmp771;});_tmp396;});res_typ=_tmp772;});}{
# 1969
struct Cyc_List_List*_tmp399=*ts;
struct Cyc_List_List*_tmp39A=_tmp38C;
while(_tmp399 != 0  && _tmp39A != 0){
# 1973
({unsigned int _tmp775=loc;struct Cyc_Tcenv_Tenv*_tmp774=te2;struct Cyc_List_List*_tmp773=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp775,_tmp774,_tmp773,& Cyc_Tcutil_ak,1,0,(void*)_tmp399->hd);});
Cyc_Tcutil_check_no_qual(loc,(void*)_tmp399->hd);
Cyc_Tcutil_unify((void*)_tmp399->hd,(void*)_tmp39A->hd);
_tmp399=_tmp399->tl;
_tmp39A=_tmp39A->tl;}
# 1979
if(_tmp399 != 0)
({void*_tmp39B=0;({unsigned int _tmp777=loc;struct _dyneither_ptr _tmp776=({const char*_tmp39C="too many explicit witness types";_tag_dyneither(_tmp39C,sizeof(char),32);});Cyc_Tcutil_terr(_tmp777,_tmp776,_tag_dyneither(_tmp39B,sizeof(void*),0));});});
# 1982
*ts=_tmp38C;{
# 1985
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,ad->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp39D=(struct _tuple19*)fields->hd;struct _tuple19*_tmp39E=_tmp39D;struct Cyc_Absyn_Aggrfield*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3A9;_LL213: _tmp3AA=_tmp39E->f1;_tmp3A9=_tmp39E->f2;_LL214:;{
void*_tmp39F=Cyc_Tcutil_substitute(_tmp38D,_tmp3AA->type);
({struct Cyc_Tcenv_Tenv*_tmp778=Cyc_Tcenv_clear_abstract_val_ok(te2);Cyc_Tcexp_tcExpInitializer(_tmp778,& _tmp39F,_tmp3A9);});
# 1996
if(!Cyc_Tcutil_coerce_arg(te2,_tmp3A9,_tmp39F,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3A6;_tmp3A6.tag=0;({struct _dyneither_ptr _tmp779=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2000
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3A9->topt)));_tmp3A6.f1=_tmp779;});({struct Cyc_String_pa_PrintArg_struct _tmp3A5;_tmp3A5.tag=0;({struct _dyneither_ptr _tmp77A=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1999
Cyc_Absynpp_typ2string(_tmp39F));_tmp3A5.f1=_tmp77A;});({struct Cyc_String_pa_PrintArg_struct _tmp3A4;_tmp3A4.tag=0;({struct _dyneither_ptr _tmp77B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(*tn));_tmp3A4.f1=_tmp77B;});({struct Cyc_String_pa_PrintArg_struct _tmp3A3;_tmp3A3.tag=0;({struct _dyneither_ptr _tmp77C=(struct _dyneither_ptr)(
# 1998
ad->kind == Cyc_Absyn_StructA?({const char*_tmp3A7="struct";_tag_dyneither(_tmp3A7,sizeof(char),7);}):({const char*_tmp3A8="union";_tag_dyneither(_tmp3A8,sizeof(char),6);}));_tmp3A3.f1=_tmp77C;});({struct Cyc_String_pa_PrintArg_struct _tmp3A2;_tmp3A2.tag=0;_tmp3A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3AA->name);({void*_tmp3A0[5]={& _tmp3A2,& _tmp3A3,& _tmp3A4,& _tmp3A5,& _tmp3A6};({unsigned int _tmp77E=_tmp3A9->loc;struct _dyneither_ptr _tmp77D=({const char*_tmp3A1="field %s of %s %s expects type %s != %s";_tag_dyneither(_tmp3A1,sizeof(char),40);});Cyc_Tcutil_terr(_tmp77E,_tmp77D,_tag_dyneither(_tmp3A0,sizeof(void*),5));});});});});});});});
# 2001
Cyc_Tcutil_explain_failure();}};}{
# 2005
struct Cyc_List_List*_tmp3AB=0;
{struct Cyc_List_List*_tmp3AC=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po;for(0;_tmp3AC != 0;_tmp3AC=_tmp3AC->tl){
({struct Cyc_List_List*_tmp782=({struct Cyc_List_List*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct _tuple0*_tmp781=({struct _tuple0*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));({void*_tmp77F=Cyc_Tcutil_substitute(_tmp38D,(*((struct _tuple0*)_tmp3AC->hd)).f1);_tmp3AE->f1=_tmp77F;});({void*_tmp780=
Cyc_Tcutil_substitute(_tmp38D,(*((struct _tuple0*)_tmp3AC->hd)).f2);_tmp3AE->f2=_tmp780;});_tmp3AE;});_tmp3AD->hd=_tmp781;});_tmp3AD->tl=_tmp3AB;_tmp3AD;});
# 2007
_tmp3AB=_tmp782;});}}
# 2010
({struct Cyc_List_List*_tmp783=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp3AB);_tmp3AB=_tmp783;});
Cyc_Tcenv_check_rgn_partial_order(te2,loc,_tmp3AB);
return res_typ;};};};};};}
# 2016
static void*Cyc_Tcexp_tcAnonStruct(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*ts,struct Cyc_List_List*args){
# 2018
{void*_tmp3B0=Cyc_Tcutil_compress(ts);void*_tmp3B1=_tmp3B0;enum Cyc_Absyn_AggrKind _tmp3C0;struct Cyc_List_List*_tmp3BF;if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B1)->tag == 12){_LL216: _tmp3C0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B1)->f1;_tmp3BF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3B1)->f2;_LL217:
# 2020
 if(_tmp3C0 == Cyc_Absyn_UnionA)
({void*_tmp3B2=0;({unsigned int _tmp785=loc;struct _dyneither_ptr _tmp784=({const char*_tmp3B3="expecting struct but found union";_tag_dyneither(_tmp3B3,sizeof(char),33);});Cyc_Tcutil_terr(_tmp785,_tmp784,_tag_dyneither(_tmp3B2,sizeof(void*),0));});});{
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,loc,args,Cyc_Absyn_StructA,_tmp3BF);
for(0;fields != 0;fields=fields->tl){
int bogus=0;
struct _tuple19*_tmp3B4=(struct _tuple19*)fields->hd;struct _tuple19*_tmp3B5=_tmp3B4;struct Cyc_Absyn_Aggrfield*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BB;_LL21B: _tmp3BC=_tmp3B5->f1;_tmp3BB=_tmp3B5->f2;_LL21C:;
({struct Cyc_Tcenv_Tenv*_tmp787=Cyc_Tcenv_clear_abstract_val_ok(te);void**_tmp786=& _tmp3BC->type;Cyc_Tcexp_tcExpInitializer(_tmp787,_tmp786,_tmp3BB);});
# 2029
if(!Cyc_Tcutil_coerce_arg(te,_tmp3BB,_tmp3BC->type,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3BA;_tmp3BA.tag=0;({struct _dyneither_ptr _tmp788=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2032
Cyc_Absynpp_typ2string((void*)_check_null(_tmp3BB->topt)));_tmp3BA.f1=_tmp788;});({struct Cyc_String_pa_PrintArg_struct _tmp3B9;_tmp3B9.tag=0;({struct _dyneither_ptr _tmp789=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2031
Cyc_Absynpp_typ2string(_tmp3BC->type));_tmp3B9.f1=_tmp789;});({struct Cyc_String_pa_PrintArg_struct _tmp3B8;_tmp3B8.tag=0;_tmp3B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp3BC->name);({void*_tmp3B6[3]={& _tmp3B8,& _tmp3B9,& _tmp3BA};({unsigned int _tmp78B=_tmp3BB->loc;struct _dyneither_ptr _tmp78A=({const char*_tmp3B7="field %s of struct expects type %s != %s";_tag_dyneither(_tmp3B7,sizeof(char),41);});Cyc_Tcutil_terr(_tmp78B,_tmp78A,_tag_dyneither(_tmp3B6,sizeof(void*),3));});});});});});
# 2033
Cyc_Tcutil_explain_failure();}}
# 2036
goto _LL215;};}else{_LL218: _LL219:
({void*_tmp3BD=0;({struct _dyneither_ptr _tmp78C=({const char*_tmp3BE="tcAnonStruct: wrong type";_tag_dyneither(_tmp3BE,sizeof(char),25);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp78C,_tag_dyneither(_tmp3BD,sizeof(void*),0));});});}_LL215:;}
# 2039
return ts;}
# 2043
static void*Cyc_Tcexp_tcDatatype(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es,struct Cyc_Absyn_Datatypedecl*tud,struct Cyc_Absyn_Datatypefield*tuf){
# 2046
struct _tuple12 _tmp3C1=({struct _tuple12 _tmp3D5;({struct Cyc_List_List*_tmp78D=Cyc_Tcenv_lookup_type_vars(te);_tmp3D5.f1=_tmp78D;});_tmp3D5.f2=Cyc_Core_heap_region;_tmp3D5;});
struct Cyc_List_List*_tmp3C2=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp3C1,tud->tvs);
struct Cyc_List_List*_tmp3C3=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmp3C2);
void*res=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp790=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp3D3;_tmp3D3.tag=4;({struct Cyc_Absyn_DatatypeFieldInfo _tmp78F=({struct Cyc_Absyn_DatatypeFieldInfo _tmp3D4;({union Cyc_Absyn_DatatypeFieldInfoU _tmp78E=
Cyc_Absyn_KnownDatatypefield(tud,tuf);_tmp3D4.field_info=_tmp78E;});_tmp3D4.targs=_tmp3C3;_tmp3D4;});_tmp3D3.f1=_tmp78F;});_tmp3D3;});_tmp3D2[0]=_tmp790;});_tmp3D2;});
# 2053
if(topt != 0){
void*_tmp3C4=Cyc_Tcutil_compress(*topt);void*_tmp3C5=_tmp3C4;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3C5)->tag == 4){_LL21E: _LL21F:
 Cyc_Tcutil_unify(*topt,res);goto _LL21D;}else{_LL220: _LL221:
 goto _LL21D;}_LL21D:;}{
# 2059
struct Cyc_List_List*ts=tuf->typs;
for(0;es != 0  && ts != 0;(es=es->tl,ts=ts->tl)){
int bogus=0;
struct Cyc_Absyn_Exp*e=(struct Cyc_Absyn_Exp*)es->hd;
void*t=(*((struct _tuple17*)ts->hd)).f2;
if(_tmp3C2 != 0)({void*_tmp791=Cyc_Tcutil_substitute(_tmp3C2,t);t=_tmp791;});
Cyc_Tcexp_tcExpInitializer(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0;({struct _dyneither_ptr _tmp792=(struct _dyneither_ptr)((struct _dyneither_ptr)(
# 2070
e->topt == 0?(struct _dyneither_ptr)({const char*_tmp3CB="?";_tag_dyneither(_tmp3CB,sizeof(char),2);}): Cyc_Absynpp_typ2string((void*)_check_null(e->topt))));_tmp3CA.f1=_tmp792;});({struct Cyc_String_pa_PrintArg_struct _tmp3C9;_tmp3C9.tag=0;({struct _dyneither_ptr _tmp793=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2069
Cyc_Absynpp_typ2string(t));_tmp3C9.f1=_tmp793;});({struct Cyc_String_pa_PrintArg_struct _tmp3C8;_tmp3C8.tag=0;({struct _dyneither_ptr _tmp794=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(tuf->name));_tmp3C8.f1=_tmp794;});({void*_tmp3C6[3]={& _tmp3C8,& _tmp3C9,& _tmp3CA};({unsigned int _tmp796=e->loc;struct _dyneither_ptr _tmp795=({const char*_tmp3C7="datatype constructor %s expects argument of type %s but this argument has type %s";_tag_dyneither(_tmp3C7,sizeof(char),82);});Cyc_Tcutil_terr(_tmp796,_tmp795,_tag_dyneither(_tmp3C6,sizeof(void*),3));});});});});});
# 2071
Cyc_Tcutil_explain_failure();}}
# 2074
if(es != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3CE;_tmp3CE.tag=0;({struct _dyneither_ptr _tmp797=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2077
Cyc_Absynpp_qvar2string(tuf->name));_tmp3CE.f1=_tmp797;});({void*_tmp3CC[1]={& _tmp3CE};({struct Cyc_Tcenv_Tenv*_tmp79B=te;unsigned int _tmp79A=((struct Cyc_Absyn_Exp*)es->hd)->loc;void**_tmp799=topt;struct _dyneither_ptr _tmp798=({const char*_tmp3CD="too many arguments for datatype constructor %s";_tag_dyneither(_tmp3CD,sizeof(char),47);});Cyc_Tcexp_expr_err(_tmp79B,_tmp79A,_tmp799,_tmp798,_tag_dyneither(_tmp3CC,sizeof(void*),1));});});});
if(ts != 0)
return({struct Cyc_String_pa_PrintArg_struct _tmp3D1;_tmp3D1.tag=0;({struct _dyneither_ptr _tmp79C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(tuf->name));_tmp3D1.f1=_tmp79C;});({void*_tmp3CF[1]={& _tmp3D1};({struct Cyc_Tcenv_Tenv*_tmp7A0=te;unsigned int _tmp79F=loc;void**_tmp79E=topt;struct _dyneither_ptr _tmp79D=({const char*_tmp3D0="too few arguments for datatype constructor %s";_tag_dyneither(_tmp3D0,sizeof(char),46);});Cyc_Tcexp_expr_err(_tmp7A0,_tmp79F,_tmp79E,_tmp79D,_tag_dyneither(_tmp3CF,sizeof(void*),1));});});});
return res;};}
# 2084
static void Cyc_Tcexp_check_malloc_type(int allow_zero,unsigned int loc,void**topt,void*t){
# 2086
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
# 2088
if(topt != 0){
void*_tmp3D6=Cyc_Tcutil_compress(*topt);void*_tmp3D7=_tmp3D6;void*_tmp3D8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->tag == 5){_LL223: _tmp3D8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D7)->f1).elt_typ;_LL224:
# 2091
 Cyc_Tcutil_unify(_tmp3D8,t);
if(Cyc_Tcutil_bits_only(t) || allow_zero  && Cyc_Tcutil_zeroable_type(t))return;
goto _LL222;}else{_LL225: _LL226:
 goto _LL222;}_LL222:;}
# 2097
({struct Cyc_String_pa_PrintArg_struct _tmp3DC;_tmp3DC.tag=0;({struct _dyneither_ptr _tmp7A1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp3DC.f1=_tmp7A1;});({struct Cyc_String_pa_PrintArg_struct _tmp3DB;_tmp3DB.tag=0;({struct _dyneither_ptr _tmp7A2=(struct _dyneither_ptr)((struct _dyneither_ptr)(allow_zero?(struct _dyneither_ptr)({const char*_tmp3DD="calloc";_tag_dyneither(_tmp3DD,sizeof(char),7);}):(struct _dyneither_ptr)({const char*_tmp3DE="malloc";_tag_dyneither(_tmp3DE,sizeof(char),7);})));_tmp3DB.f1=_tmp7A2;});({void*_tmp3D9[2]={& _tmp3DB,& _tmp3DC};({unsigned int _tmp7A4=loc;struct _dyneither_ptr _tmp7A3=({const char*_tmp3DA="%s cannot be used with type %s\n\t(type needs initialization)";_tag_dyneither(_tmp3DA,sizeof(char),60);});Cyc_Tcutil_terr(_tmp7A4,_tmp7A3,_tag_dyneither(_tmp3D9,sizeof(void*),2));});});});});}
# 2101
static void*Cyc_Tcexp_mallocRgn(void*rgn){
# 2103
enum Cyc_Absyn_AliasQual _tmp3DF=(Cyc_Tcutil_typ_kind(Cyc_Tcutil_compress(rgn)))->aliasqual;enum Cyc_Absyn_AliasQual _tmp3E0=_tmp3DF;if(_tmp3E0 == Cyc_Absyn_Unique){_LL228: _LL229:
 return(void*)& Cyc_Absyn_UniqueRgn_val;}else{_LL22A: _LL22B:
 return(void*)& Cyc_Absyn_HeapRgn_val;}_LL227:;}
# 2109
static void*Cyc_Tcexp_tcMalloc(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**ropt,void***t,struct Cyc_Absyn_Exp**e,int*is_calloc,int*is_fat){
# 2114
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
if(*ropt != 0){
# 2117
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp7A6=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp3E8;_tmp3E8.tag=15;({void*_tmp7A5=({
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));});_tmp3E8.f1=_tmp7A5;});_tmp3E8;});_tmp3E7[0]=_tmp7A6;});_tmp3E7;});
void*handle_type=Cyc_Tcexp_tcExp(te,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*ropt));
void*_tmp3E1=Cyc_Tcutil_compress(handle_type);void*_tmp3E2=_tmp3E1;void*_tmp3E6;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2)->tag == 15){_LL22D: _tmp3E6=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3E2)->f1;_LL22E:
# 2122
 rgn=_tmp3E6;
Cyc_Tcenv_check_rgn_accessible(te,loc,rgn);
goto _LL22C;}else{_LL22F: _LL230:
# 2126
({struct Cyc_String_pa_PrintArg_struct _tmp3E5;_tmp3E5.tag=0;({struct _dyneither_ptr _tmp7A7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));_tmp3E5.f1=_tmp7A7;});({void*_tmp3E3[1]={& _tmp3E5};({unsigned int _tmp7A9=((struct Cyc_Absyn_Exp*)_check_null(*ropt))->loc;struct _dyneither_ptr _tmp7A8=({const char*_tmp3E4="expecting region_t type but found %s";_tag_dyneither(_tmp3E4,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7A9,_tmp7A8,_tag_dyneither(_tmp3E3,sizeof(void*),1));});});});
goto _LL22C;}_LL22C:;}else{
# 2133
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7AA=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7AA;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*ropt=Cyc_Absyn_uniquergn_exp;}}}
# 2140
({struct Cyc_Tcenv_Tenv*_tmp7AB=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7AB,& Cyc_Absyn_uint_typ,*e);});{
# 2149 "tcexp.cyc"
void*elt_type;
struct Cyc_Absyn_Exp*num_elts;
int one_elt;
if(*is_calloc){
if(*t == 0)({void*_tmp3E9=0;({struct _dyneither_ptr _tmp7AC=({const char*_tmp3EA="calloc with empty type";_tag_dyneither(_tmp3EA,sizeof(char),23);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7AC,_tag_dyneither(_tmp3E9,sizeof(void*),0));});});
elt_type=*((void**)_check_null(*t));
({unsigned int _tmp7AF=loc;struct Cyc_Tcenv_Tenv*_tmp7AE=te;struct Cyc_List_List*_tmp7AD=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcutil_check_type(_tmp7AF,_tmp7AE,_tmp7AD,& Cyc_Tcutil_tmk,1,0,elt_type);});
Cyc_Tcutil_check_no_qual(loc,elt_type);
Cyc_Tcexp_check_malloc_type(1,loc,topt,elt_type);
num_elts=*e;
one_elt=0;}else{
# 2161
void*er=(*e)->r;
retry_sizeof: {
void*_tmp3EB=er;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp400;void*_tmp3FF;switch(*((int*)_tmp3EB)){case 16: _LL232: _tmp3FF=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3EB)->f1;_LL233:
# 2165
 elt_type=_tmp3FF;
({void**_tmp7B0=({void**_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC[0]=elt_type;_tmp3EC;});*t=_tmp7B0;});
({struct Cyc_Absyn_Exp*_tmp7B1=Cyc_Absyn_uint_exp(1,0);num_elts=_tmp7B1;});
Cyc_Tcexp_tcExp(te,& Cyc_Absyn_uint_typ,num_elts);
one_elt=1;
goto _LL231;case 2: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3EB)->f1 == Cyc_Absyn_Times){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3EB)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3EB)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3EB)->f2)->tl)->tl == 0){_LL234: _tmp401=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3EB)->f2)->hd;_tmp400=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3EB)->f2)->tl)->hd;_LL235:
# 2172
{struct _tuple0 _tmp3ED=({struct _tuple0 _tmp3F3;_tmp3F3.f1=_tmp401->r;_tmp3F3.f2=_tmp400->r;_tmp3F3;});struct _tuple0 _tmp3EE=_tmp3ED;void*_tmp3F2;void*_tmp3F1;if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3EE.f1)->tag == 16){_LL239: _tmp3F1=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3EE.f1)->f1;_LL23A:
# 2174
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3F1);
elt_type=_tmp3F1;
({void**_tmp7B2=({void**_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF[0]=elt_type;_tmp3EF;});*t=_tmp7B2;});
num_elts=_tmp400;
one_elt=0;
goto _LL238;}else{if(((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3EE.f2)->tag == 16){_LL23B: _tmp3F2=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp3EE.f2)->f1;_LL23C:
# 2181
 Cyc_Tcexp_check_malloc_type(0,loc,topt,_tmp3F2);
elt_type=_tmp3F2;
({void**_tmp7B3=({void**_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0[0]=elt_type;_tmp3F0;});*t=_tmp7B3;});
num_elts=_tmp401;
one_elt=0;
goto _LL238;}else{_LL23D: _LL23E:
 goto No_sizeof;}}_LL238:;}
# 2189
goto _LL231;}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}}else{goto _LL236;}default: _LL236: _LL237:
# 2191
 No_sizeof: {
# 2194
struct Cyc_Absyn_Exp*_tmp3F4=*e;
{void*_tmp3F5=_tmp3F4->r;void*_tmp3F6=_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F7;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F6)->tag == 13){_LL240: _tmp3F7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F6)->f2;_LL241:
 _tmp3F4=_tmp3F7;goto _LL23F;}else{_LL242: _LL243:
 goto _LL23F;}_LL23F:;}
# 2199
{void*_tmp3F8=Cyc_Tcutil_compress((void*)_check_null(_tmp3F4->topt));void*_tmp3F9=_tmp3F8;void*_tmp3FD;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F9)->tag == 19){_LL245: _tmp3FD=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F9)->f1;_LL246:
# 2201
{void*_tmp3FA=Cyc_Tcutil_compress(_tmp3FD);void*_tmp3FB=_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FC;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3FB)->tag == 18){_LL24A: _tmp3FC=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3FB)->f1;_LL24B:
# 2203
 er=_tmp3FC->r;goto retry_sizeof;}else{_LL24C: _LL24D:
 goto _LL249;}_LL249:;}
# 2206
goto _LL244;}else{_LL247: _LL248:
 goto _LL244;}_LL244:;}
# 2209
elt_type=Cyc_Absyn_char_typ;
({void**_tmp7B4=({void**_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE[0]=elt_type;_tmp3FE;});*t=_tmp7B4;});
num_elts=*e;
one_elt=0;}
# 2214
goto _LL231;}_LL231:;}}
# 2218
*is_fat=!one_elt;
# 2221
{void*_tmp402=elt_type;struct Cyc_Absyn_Aggrdecl*_tmp405;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp402)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp402)->f1).aggr_info).KnownAggr).tag == 2){_LL24F: _tmp405=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp402)->f1).aggr_info).KnownAggr).val;_LL250:
# 2223
 if(_tmp405->impl != 0  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp405->impl))->exist_vars != 0)
({void*_tmp403=0;({unsigned int _tmp7B6=loc;struct _dyneither_ptr _tmp7B5=({const char*_tmp404="malloc with existential types not yet implemented";_tag_dyneither(_tmp404,sizeof(char),50);});Cyc_Tcutil_terr(_tmp7B6,_tmp7B5,_tag_dyneither(_tmp403,sizeof(void*),0));});});
goto _LL24E;}else{goto _LL251;}}else{_LL251: _LL252:
 goto _LL24E;}_LL24E:;}{
# 2230
void*(*_tmp406)(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term)=Cyc_Absyn_at_typ;
union Cyc_Absyn_Constraint*_tmp407=Cyc_Absyn_false_conref;
if(topt != 0){
void*_tmp408=Cyc_Tcutil_compress(*topt);void*_tmp409=_tmp408;union Cyc_Absyn_Constraint*_tmp416;union Cyc_Absyn_Constraint*_tmp415;union Cyc_Absyn_Constraint*_tmp414;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp409)->tag == 5){_LL254: _tmp416=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp409)->f1).ptr_atts).nullable;_tmp415=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp409)->f1).ptr_atts).bounds;_tmp414=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp409)->f1).ptr_atts).zero_term;_LL255:
# 2235
 _tmp407=_tmp414;
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp416))
_tmp406=Cyc_Absyn_star_typ;
# 2240
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp414) && !(*is_calloc)){
({void*_tmp40A=0;({unsigned int _tmp7B8=loc;struct _dyneither_ptr _tmp7B7=({const char*_tmp40B="converting malloc to calloc to ensure zero-termination";_tag_dyneither(_tmp40B,sizeof(char),55);});Cyc_Tcutil_warn(_tmp7B8,_tmp7B7,_tag_dyneither(_tmp40A,sizeof(void*),0));});});
*is_calloc=1;}
# 2246
{void*_tmp40C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp415);void*_tmp40D=_tmp40C;struct Cyc_Absyn_Exp*_tmp413;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp40D)->tag == 0){_LL259: _LL25A:
 goto _LL258;}else{_LL25B: _tmp413=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp40D)->f1;if(!one_elt){_LL25C: {
# 2249
int _tmp40E=Cyc_Evexp_c_can_eval(num_elts);
if(_tmp40E  && Cyc_Evexp_same_const_exp(_tmp413,num_elts)){
*is_fat=0;
return({void*_tmp7BC=elt_type;void*_tmp7BB=rgn;struct Cyc_Absyn_Tqual _tmp7BA=Cyc_Absyn_empty_tqual(0);void*_tmp7B9=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp415);Cyc_Absyn_atb_typ(_tmp7BC,_tmp7BB,_tmp7BA,_tmp7B9,_tmp407);});}
# 2255
{void*_tmp40F=Cyc_Tcutil_compress((void*)_check_null(num_elts->topt));void*_tmp410=_tmp40F;void*_tmp412;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp410)->tag == 19){_LL260: _tmp412=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp410)->f1;_LL261: {
# 2257
struct Cyc_Absyn_Exp*_tmp411=({void*_tmp7BD=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmp7BD,Cyc_Absyn_valueof_exp(_tmp412,0),0,Cyc_Absyn_No_coercion,0);});
# 2259
if(Cyc_Evexp_same_const_exp(_tmp411,_tmp413)){
*is_fat=0;
return({void*_tmp7C1=elt_type;void*_tmp7C0=rgn;struct Cyc_Absyn_Tqual _tmp7BF=Cyc_Absyn_empty_tqual(0);void*_tmp7BE=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp415);Cyc_Absyn_atb_typ(_tmp7C1,_tmp7C0,_tmp7BF,_tmp7BE,_tmp407);});}
# 2264
goto _LL25F;}}else{_LL262: _LL263:
 goto _LL25F;}_LL25F:;}
# 2267
goto _LL258;}}else{_LL25D: _LL25E:
 goto _LL258;}}_LL258:;}
# 2270
goto _LL253;}else{_LL256: _LL257:
 goto _LL253;}_LL253:;}
# 2273
if(!one_elt)_tmp406=Cyc_Absyn_dyneither_typ;
return({void*_tmp7C4=elt_type;void*_tmp7C3=rgn;struct Cyc_Absyn_Tqual _tmp7C2=Cyc_Absyn_empty_tqual(0);_tmp406(_tmp7C4,_tmp7C3,_tmp7C2,_tmp407);});};};}
# 2278
static void*Cyc_Tcexp_tcSwap(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
# 2280
struct Cyc_Tcenv_Tenv*_tmp417=Cyc_Tcenv_enter_lhs(te);
Cyc_Tcexp_tcExpNoPromote(_tmp417,0,e1);{
void*_tmp418=(void*)_check_null(e1->topt);
Cyc_Tcexp_tcExpNoPromote(_tmp417,& _tmp418,e2);{
void*t1=(void*)_check_null(e1->topt);
void*t2=(void*)_check_null(e2->topt);
# 2287
{void*_tmp419=Cyc_Tcutil_compress(t1);void*_tmp41A=_tmp419;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp41A)->tag == 8){_LL265: _LL266:
({void*_tmp41B=0;({unsigned int _tmp7C6=loc;struct _dyneither_ptr _tmp7C5=({const char*_tmp41C="cannot assign to an array";_tag_dyneither(_tmp41C,sizeof(char),26);});Cyc_Tcutil_terr(_tmp7C6,_tmp7C5,_tag_dyneither(_tmp41B,sizeof(void*),0));});});goto _LL264;}else{_LL267: _LL268:
 goto _LL264;}_LL264:;}
# 2292
if(!Cyc_Tcutil_is_boxed(t1) && !Cyc_Tcutil_is_pointer_type(t1))
({void*_tmp41D=0;({unsigned int _tmp7C8=loc;struct _dyneither_ptr _tmp7C7=({const char*_tmp41E="Swap not allowed for non-pointer or non-word-sized types.";_tag_dyneither(_tmp41E,sizeof(char),58);});Cyc_Tcutil_terr(_tmp7C8,_tmp7C7,_tag_dyneither(_tmp41D,sizeof(void*),0));});});
# 2296
if(!Cyc_Absyn_is_lvalue(e1))
return({void*_tmp41F=0;({struct Cyc_Tcenv_Tenv*_tmp7CC=te;unsigned int _tmp7CB=e1->loc;void**_tmp7CA=topt;struct _dyneither_ptr _tmp7C9=({const char*_tmp420="swap non-lvalue";_tag_dyneither(_tmp420,sizeof(char),16);});Cyc_Tcexp_expr_err(_tmp7CC,_tmp7CB,_tmp7CA,_tmp7C9,_tag_dyneither(_tmp41F,sizeof(void*),0));});});
if(!Cyc_Absyn_is_lvalue(e2))
return({void*_tmp421=0;({struct Cyc_Tcenv_Tenv*_tmp7D0=te;unsigned int _tmp7CF=e2->loc;void**_tmp7CE=topt;struct _dyneither_ptr _tmp7CD=({const char*_tmp422="swap non-lvalue";_tag_dyneither(_tmp422,sizeof(char),16);});Cyc_Tcexp_expr_err(_tmp7D0,_tmp7CF,_tmp7CE,_tmp7CD,_tag_dyneither(_tmp421,sizeof(void*),0));});});{
# 2301
void*t_ign1=(void*)& Cyc_Absyn_VoidType_val;void*t_ign2=(void*)& Cyc_Absyn_VoidType_val;
int b_ign1=0;
if(Cyc_Tcutil_is_zero_ptr_deref(e1,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp423=0;({struct Cyc_Tcenv_Tenv*_tmp7D4=te;unsigned int _tmp7D3=e1->loc;void**_tmp7D2=topt;struct _dyneither_ptr _tmp7D1=({const char*_tmp424="swap value in zeroterm array";_tag_dyneither(_tmp424,sizeof(char),29);});Cyc_Tcexp_expr_err(_tmp7D4,_tmp7D3,_tmp7D2,_tmp7D1,_tag_dyneither(_tmp423,sizeof(void*),0));});});
if(Cyc_Tcutil_is_zero_ptr_deref(e2,& t_ign1,& b_ign1,& t_ign2))
return({void*_tmp425=0;({struct Cyc_Tcenv_Tenv*_tmp7D8=te;unsigned int _tmp7D7=e2->loc;void**_tmp7D6=topt;struct _dyneither_ptr _tmp7D5=({const char*_tmp426="swap value in zeroterm array";_tag_dyneither(_tmp426,sizeof(char),29);});Cyc_Tcexp_expr_err(_tmp7D8,_tmp7D7,_tmp7D6,_tmp7D5,_tag_dyneither(_tmp425,sizeof(void*),0));});});
# 2308
Cyc_Tcexp_check_writable(te,e1);
Cyc_Tcexp_check_writable(te,e2);
if(!Cyc_Tcutil_unify(t1,t2)){
void*_tmp427=({struct Cyc_String_pa_PrintArg_struct _tmp42B;_tmp42B.tag=0;({struct _dyneither_ptr _tmp7D9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp42B.f1=_tmp7D9;});({struct Cyc_String_pa_PrintArg_struct _tmp42A;_tmp42A.tag=0;({struct _dyneither_ptr _tmp7DA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp42A.f1=_tmp7DA;});({void*_tmp428[2]={& _tmp42A,& _tmp42B};({struct Cyc_Tcenv_Tenv*_tmp7DE=te;unsigned int _tmp7DD=loc;void**_tmp7DC=topt;struct _dyneither_ptr _tmp7DB=({const char*_tmp429="type mismatch: %s != %s";_tag_dyneither(_tmp429,sizeof(char),24);});Cyc_Tcexp_expr_err(_tmp7DE,_tmp7DD,_tmp7DC,_tmp7DB,_tag_dyneither(_tmp428,sizeof(void*),2));});});});});
return _tmp427;}
# 2315
return(void*)& Cyc_Absyn_VoidType_val;};};};}
# 2319
int Cyc_Tcexp_in_stmt_exp=0;
# 2321
static void*Cyc_Tcexp_tcStmtExp(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Stmt*s){
int old_stmt_exp_state=Cyc_Tcexp_in_stmt_exp;
({struct Cyc_Tcenv_Tenv*_tmp7DF=Cyc_Tcenv_clear_abstract_val_ok(te);Cyc_Tcstmt_tcStmt(_tmp7DF,s,1);});
Cyc_Tcexp_in_stmt_exp=old_stmt_exp_state;
# 2326
while(1){
void*_tmp42C=s->r;void*_tmp42D=_tmp42C;struct Cyc_Absyn_Decl*_tmp438;struct Cyc_Absyn_Stmt*_tmp437;struct Cyc_Absyn_Stmt*_tmp436;struct Cyc_Absyn_Stmt*_tmp435;struct Cyc_Absyn_Exp*_tmp434;switch(*((int*)_tmp42D)){case 1: _LL26A: _tmp434=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp42D)->f1;_LL26B: {
# 2330
void*_tmp42E=(void*)_check_null(_tmp434->topt);
if(!({void*_tmp7E0=_tmp42E;Cyc_Tcutil_unify(_tmp7E0,Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te)));})){
({struct Cyc_String_pa_PrintArg_struct _tmp431;_tmp431.tag=0;({struct _dyneither_ptr _tmp7E1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp42E));_tmp431.f1=_tmp7E1;});({void*_tmp42F[1]={& _tmp431};({unsigned int _tmp7E3=loc;struct _dyneither_ptr _tmp7E2=({const char*_tmp430="statement expression returns type %s";_tag_dyneither(_tmp430,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7E3,_tmp7E2,_tag_dyneither(_tmp42F,sizeof(void*),1));});});});
Cyc_Tcutil_explain_failure();}
# 2336
return _tmp42E;}case 2: _LL26C: _tmp436=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp42D)->f1;_tmp435=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp42D)->f2;_LL26D:
 s=_tmp435;continue;case 12: _LL26E: _tmp438=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp42D)->f1;_tmp437=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp42D)->f2;_LL26F:
 s=_tmp437;continue;default: _LL270: _LL271:
# 2340
 return({void*_tmp432=0;({struct Cyc_Tcenv_Tenv*_tmp7E7=te;unsigned int _tmp7E6=loc;void**_tmp7E5=topt;struct _dyneither_ptr _tmp7E4=({const char*_tmp433="statement expression must end with expression";_tag_dyneither(_tmp433,sizeof(char),46);});Cyc_Tcexp_expr_err(_tmp7E7,_tmp7E6,_tmp7E5,_tmp7E4,_tag_dyneither(_tmp432,sizeof(void*),0));});});}_LL269:;}}
# 2345
static void*Cyc_Tcexp_tcTagcheck(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f){
void*t=Cyc_Tcutil_compress(({struct Cyc_Tcenv_Tenv*_tmp7E8=Cyc_Tcenv_enter_abstract_val_ok(te);Cyc_Tcexp_tcExp(_tmp7E8,0,e);}));
{void*_tmp439=t;struct Cyc_Absyn_Aggrdecl*_tmp43D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp439)->tag == 11){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp439)->f1).aggr_info).KnownAggr).tag == 2){_LL273: _tmp43D=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp439)->f1).aggr_info).KnownAggr).val;_LL274:
# 2349
 if((_tmp43D->kind == Cyc_Absyn_UnionA  && _tmp43D->impl != 0) && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp43D->impl))->tagged)goto _LL272;
goto _LL276;}else{goto _LL275;}}else{_LL275: _LL276:
# 2352
({struct Cyc_String_pa_PrintArg_struct _tmp43C;_tmp43C.tag=0;({struct _dyneither_ptr _tmp7E9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp43C.f1=_tmp7E9;});({void*_tmp43A[1]={& _tmp43C};({unsigned int _tmp7EB=loc;struct _dyneither_ptr _tmp7EA=({const char*_tmp43B="expecting @tagged union but found %s";_tag_dyneither(_tmp43B,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7EB,_tmp7EA,_tag_dyneither(_tmp43A,sizeof(void*),1));});});});
goto _LL272;}_LL272:;}
# 2355
return Cyc_Absyn_uint_typ;}
# 2359
static void*Cyc_Tcexp_tcNew(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void**topt,struct Cyc_Absyn_Exp**rgn_handle,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1){
# 2363
void*rgn=(void*)& Cyc_Absyn_HeapRgn_val;
struct Cyc_Tcenv_Tenv*_tmp43E=Cyc_Tcenv_clear_abstract_val_ok(Cyc_Tcenv_set_new_status(Cyc_Tcenv_InNew,te));
if(*rgn_handle != 0){
# 2367
void*expected_type=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp445=_cycalloc(sizeof(*_tmp445));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp7ED=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp446;_tmp446.tag=15;({void*_tmp7EC=({
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(_tmp43E));});_tmp446.f1=_tmp7EC;});_tmp446;});_tmp445[0]=_tmp7ED;});_tmp445;});
void*handle_type=Cyc_Tcexp_tcExp(_tmp43E,& expected_type,(struct Cyc_Absyn_Exp*)_check_null(*rgn_handle));
void*_tmp43F=Cyc_Tcutil_compress(handle_type);void*_tmp440=_tmp43F;void*_tmp444;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp440)->tag == 15){_LL278: _tmp444=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp440)->f1;_LL279:
# 2372
 rgn=_tmp444;
Cyc_Tcenv_check_rgn_accessible(_tmp43E,loc,rgn);
goto _LL277;}else{_LL27A: _LL27B:
# 2376
({struct Cyc_String_pa_PrintArg_struct _tmp443;_tmp443.tag=0;({struct _dyneither_ptr _tmp7EE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(handle_type));_tmp443.f1=_tmp7EE;});({void*_tmp441[1]={& _tmp443};({unsigned int _tmp7F0=((struct Cyc_Absyn_Exp*)_check_null(*rgn_handle))->loc;struct _dyneither_ptr _tmp7EF=({const char*_tmp442="expecting region_t type but found %s";_tag_dyneither(_tmp442,sizeof(char),37);});Cyc_Tcutil_terr(_tmp7F0,_tmp7EF,_tag_dyneither(_tmp441,sizeof(void*),1));});});});
goto _LL277;}_LL277:;}else{
# 2383
if(topt != 0){
void*optrgn=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_rgn_of_pointer(*topt,& optrgn)){
({void*_tmp7F1=Cyc_Tcexp_mallocRgn(optrgn);rgn=_tmp7F1;});
if(rgn == (void*)& Cyc_Absyn_UniqueRgn_val)*rgn_handle=Cyc_Absyn_uniquergn_exp;}}}{
# 2391
void*_tmp447=e1->r;void*_tmp448=_tmp447;struct Cyc_List_List*_tmp467;struct Cyc_Core_Opt*_tmp466;struct Cyc_List_List*_tmp465;switch(*((int*)_tmp448)){case 26: _LL27D: _LL27E:
 goto _LL280;case 27: _LL27F: _LL280: {
# 2396
void*res_typ=Cyc_Tcexp_tcExpNoPromote(_tmp43E,topt,e1);
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp449=0;({struct _dyneither_ptr _tmp7F2=({const char*_tmp44A="tcNew: comprehension returned non-array type";_tag_dyneither(_tmp44A,sizeof(char),45);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7F2,_tag_dyneither(_tmp449,sizeof(void*),0));});});
({void*_tmp7F3=Cyc_Tcutil_promote_array(res_typ,rgn,1);res_typ=_tmp7F3;});
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp43E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp43E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2407
return res_typ;}case 35: _LL281: _tmp466=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp448)->f1;_tmp465=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp448)->f2;_LL282:
# 2409
({void*_tmp7F5=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp7F4=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmp44C;_tmp44C.tag=25;_tmp44C.f1=_tmp465;_tmp44C;});_tmp44B[0]=_tmp7F4;});_tmp44B;});e1->r=_tmp7F5;});
_tmp467=_tmp465;goto _LL284;case 25: _LL283: _tmp467=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp448)->f1;_LL284: {
# 2412
void**elt_typ_opt=0;
int zero_term=0;
if(topt != 0){
void*_tmp44D=Cyc_Tcutil_compress(*topt);void*_tmp44E=_tmp44D;void**_tmp451;struct Cyc_Absyn_Tqual _tmp450;union Cyc_Absyn_Constraint*_tmp44F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44E)->tag == 5){_LL28C: _tmp451=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44E)->f1).elt_typ;_tmp450=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44E)->f1).elt_tq;_tmp44F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44E)->f1).ptr_atts).zero_term;_LL28D:
# 2418
 elt_typ_opt=_tmp451;
({int _tmp7F6=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp44F);zero_term=_tmp7F6;});
goto _LL28B;}else{_LL28E: _LL28F:
 goto _LL28B;}_LL28B:;}{
# 2424
void*res_typ=Cyc_Tcexp_tcArray(_tmp43E,e1->loc,elt_typ_opt,0,zero_term,_tmp467);
e1->topt=res_typ;
if(!Cyc_Tcutil_is_array(res_typ))
({void*_tmp452=0;({struct _dyneither_ptr _tmp7F7=({const char*_tmp453="tcExpNoPromote on Array_e returned non-array type";_tag_dyneither(_tmp453,sizeof(char),50);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp7F7,_tag_dyneither(_tmp452,sizeof(void*),0));});});
({void*_tmp7F8=Cyc_Tcutil_promote_array(res_typ,rgn,0);res_typ=_tmp7F8;});
if(topt != 0){
# 2433
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp43E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp43E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2439
return res_typ;};}case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp448)->f1).Wstring_c).tag){case 8: _LL285: _LL286: {
# 2444
void*_tmp454=({void*_tmp7FB=Cyc_Absyn_char_typ;void*_tmp7FA=rgn;struct Cyc_Absyn_Tqual _tmp7F9=Cyc_Absyn_const_tqual(0);Cyc_Absyn_atb_typ(_tmp7FB,_tmp7FA,_tmp7F9,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2446
void*_tmp455=Cyc_Tcexp_tcExp(_tmp43E,& _tmp454,e1);
return({void*_tmp801=_tmp455;void*_tmp800=rgn;struct Cyc_Absyn_Tqual _tmp7FF=Cyc_Absyn_empty_tqual(0);void*_tmp7FE=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp456=_cycalloc(sizeof(*_tmp456));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp7FD=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp457;_tmp457.tag=1;({struct Cyc_Absyn_Exp*_tmp7FC=Cyc_Absyn_uint_exp(1,0);_tmp457.f1=_tmp7FC;});_tmp457;});_tmp456[0]=_tmp7FD;});_tmp456;});Cyc_Absyn_atb_typ(_tmp801,_tmp800,_tmp7FF,_tmp7FE,Cyc_Absyn_false_conref);});}case 9: _LL287: _LL288: {
# 2451
void*_tmp458=({void*_tmp804=Cyc_Absyn_wchar_typ();void*_tmp803=rgn;struct Cyc_Absyn_Tqual _tmp802=Cyc_Absyn_const_tqual(0);Cyc_Absyn_atb_typ(_tmp804,_tmp803,_tmp802,(void*)& Cyc_Absyn_DynEither_b_val,Cyc_Absyn_true_conref);});
# 2453
void*_tmp459=Cyc_Tcexp_tcExp(_tmp43E,& _tmp458,e1);
return({void*_tmp80A=_tmp459;void*_tmp809=rgn;struct Cyc_Absyn_Tqual _tmp808=Cyc_Absyn_empty_tqual(0);void*_tmp807=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp45A=_cycalloc(sizeof(*_tmp45A));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp806=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp45B;_tmp45B.tag=1;({struct Cyc_Absyn_Exp*_tmp805=Cyc_Absyn_uint_exp(1,0);_tmp45B.f1=_tmp805;});_tmp45B;});_tmp45A[0]=_tmp806;});_tmp45A;});Cyc_Absyn_atb_typ(_tmp80A,_tmp809,_tmp808,_tmp807,Cyc_Absyn_false_conref);});}default: goto _LL289;}default: _LL289: _LL28A:
# 2460
 RG: {
void*bogus=(void*)& Cyc_Absyn_VoidType_val;
void**topt2=0;
if(topt != 0){
void*_tmp45C=Cyc_Tcutil_compress(*topt);void*_tmp45D=_tmp45C;void**_tmp45F;struct Cyc_Absyn_Tqual _tmp45E;switch(*((int*)_tmp45D)){case 5: _LL291: _tmp45F=(void**)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45D)->f1).elt_typ;_tmp45E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45D)->f1).elt_tq;_LL292:
# 2466
 topt2=_tmp45F;goto _LL290;case 3: _LL293: _LL294:
# 2470
 bogus=*topt;
topt2=& bogus;
goto _LL290;default: _LL295: _LL296:
 goto _LL290;}_LL290:;}{
# 2476
void*telt=Cyc_Tcexp_tcExp(_tmp43E,topt2,e1);
# 2478
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(telt) && !Cyc_Tcutil_is_noalias_path(e1))
({void*_tmp460=0;({unsigned int _tmp80C=e1->loc;struct _dyneither_ptr _tmp80B=({const char*_tmp461="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp461,sizeof(char),49);});Cyc_Tcutil_terr(_tmp80C,_tmp80B,_tag_dyneither(_tmp460,sizeof(void*),0));});});{
void*res_typ=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp462=_cycalloc(sizeof(*_tmp462));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp811=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp463;_tmp463.tag=5;({struct Cyc_Absyn_PtrInfo _tmp810=({struct Cyc_Absyn_PtrInfo _tmp464;_tmp464.elt_typ=telt;({struct Cyc_Absyn_Tqual _tmp80D=
Cyc_Absyn_empty_tqual(0);_tmp464.elt_tq=_tmp80D;});({struct Cyc_Absyn_PtrAtts _tmp80F=({(_tmp464.ptr_atts).rgn=rgn;({union Cyc_Absyn_Constraint*_tmp80E=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp464.ptr_atts).nullable=_tmp80E;});(_tmp464.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp464.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp464.ptr_atts).ptrloc=0;_tmp464.ptr_atts;});_tmp464.ptr_atts=_tmp80F;});_tmp464;});_tmp463.f1=_tmp810;});_tmp463;});_tmp462[0]=_tmp811;});_tmp462;});
# 2485
if(topt != 0){
if(!Cyc_Tcutil_unify(*topt,res_typ) && Cyc_Tcutil_silent_castable(_tmp43E,loc,res_typ,*topt)){
e->topt=res_typ;
Cyc_Tcutil_unchecked_cast(_tmp43E,e,*topt,Cyc_Absyn_Other_coercion);
res_typ=*topt;}}
# 2492
return res_typ;};};}}_LL27C:;};}
# 2498
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcutil_compress(Cyc_Tcexp_tcExpNoPromote(te,topt,e));
if(Cyc_Tcutil_is_array(t))
({void*_tmp814=({void*_tmp813=({void*_tmp812=t;Cyc_Tcutil_promote_array(_tmp812,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp813;});e->topt=_tmp814;});
return t;}
# 2508
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
void*t=Cyc_Tcexp_tcExpNoPromote(te,topt,e);
# 2512
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
# 2517
({void*_tmp468=0;({unsigned int _tmp816=e->loc;struct _dyneither_ptr _tmp815=({const char*_tmp469="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp469,sizeof(char),49);});Cyc_Tcutil_terr(_tmp816,_tmp815,_tag_dyneither(_tmp468,sizeof(void*),0));});});{
# 2519
void*_tmp46A=e->r;void*_tmp46B=_tmp46A;switch(*((int*)_tmp46B)){case 25: _LL298: _LL299:
 goto _LL29B;case 26: _LL29A: _LL29B:
 goto _LL29D;case 27: _LL29C: _LL29D:
 goto _LL29F;case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp46B)->f1).String_c).tag){case 9: _LL29E: _LL29F:
 goto _LL2A1;case 8: _LL2A0: _LL2A1:
 return t;default: goto _LL2A2;}default: _LL2A2: _LL2A3:
# 2526
({void*_tmp817=Cyc_Tcutil_compress(t);t=_tmp817;});
if(Cyc_Tcutil_is_array(t)){
# 2529
({void*_tmp819=({void*_tmp818=t;Cyc_Tcutil_promote_array(_tmp818,(Cyc_Tcutil_addressof_props(te,e)).f2,0);});t=_tmp819;});
Cyc_Tcutil_unchecked_cast(te,e,t,Cyc_Absyn_Other_coercion);}
# 2532
return t;}_LL297:;};}
# 2543 "tcexp.cyc"
static void*Cyc_Tcexp_tcExpNoPromote(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
{void*_tmp46C=e->r;void*_tmp46D=_tmp46C;struct Cyc_Absyn_Exp*_tmp491;if(((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46D)->tag == 11){_LL2A5: _tmp491=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp46D)->f1;_LL2A6:
# 2547
 Cyc_Tcexp_tcExpNoInst(te,topt,_tmp491);
({void*_tmp81A=Cyc_Absyn_pointer_expand((void*)_check_null(_tmp491->topt),0);_tmp491->topt=_tmp81A;});
e->topt=_tmp491->topt;
goto _LL2A4;}else{_LL2A7: _LL2A8:
# 2553
 Cyc_Tcexp_tcExpNoInst(te,topt,e);
({void*_tmp81B=Cyc_Absyn_pointer_expand((void*)_check_null(e->topt),0);e->topt=_tmp81B;});
# 2556
{void*_tmp46E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp46F=_tmp46E;void*_tmp490;struct Cyc_Absyn_Tqual _tmp48F;void*_tmp48E;union Cyc_Absyn_Constraint*_tmp48D;union Cyc_Absyn_Constraint*_tmp48C;union Cyc_Absyn_Constraint*_tmp48B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->tag == 5){_LL2AA: _tmp490=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).elt_typ;_tmp48F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).elt_tq;_tmp48E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).ptr_atts).rgn;_tmp48D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).ptr_atts).nullable;_tmp48C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).ptr_atts).bounds;_tmp48B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46F)->f1).ptr_atts).zero_term;_LL2AB:
# 2558
{void*_tmp470=Cyc_Tcutil_compress(_tmp490);void*_tmp471=_tmp470;struct Cyc_List_List*_tmp48A;void*_tmp489;struct Cyc_Absyn_Tqual _tmp488;void*_tmp487;struct Cyc_List_List*_tmp486;int _tmp485;struct Cyc_Absyn_VarargInfo*_tmp484;struct Cyc_List_List*_tmp483;struct Cyc_List_List*_tmp482;struct Cyc_Absyn_Exp*_tmp481;struct Cyc_List_List*_tmp480;struct Cyc_Absyn_Exp*_tmp47F;struct Cyc_List_List*_tmp47E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->tag == 9){_LL2AF: _tmp48A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).tvars;_tmp489=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).effect;_tmp488=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).ret_tqual;_tmp487=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).ret_typ;_tmp486=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).args;_tmp485=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).c_varargs;_tmp484=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).cyc_varargs;_tmp483=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).rgn_po;_tmp482=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).attributes;_tmp481=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).requires_clause;_tmp480=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).requires_relns;_tmp47F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).ensures_clause;_tmp47E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp471)->f1).ensures_relns;_LL2B0:
# 2560
 if(_tmp48A != 0){
struct _tuple12 _tmp472=({struct _tuple12 _tmp47D;({struct Cyc_List_List*_tmp81C=Cyc_Tcenv_lookup_type_vars(te);_tmp47D.f1=_tmp81C;});_tmp47D.f2=Cyc_Core_heap_region;_tmp47D;});
struct Cyc_List_List*inst=
((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmp472,_tmp48A);
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,inst);
# 2568
({struct Cyc_List_List*_tmp81D=Cyc_Tcutil_rsubst_rgnpo(Cyc_Core_heap_region,inst,_tmp483);_tmp483=_tmp81D;});
Cyc_Tcenv_check_rgn_partial_order(te,e->loc,_tmp483);{
void*ftyp=({struct Cyc_List_List*_tmp820=inst;Cyc_Tcutil_substitute(_tmp820,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp81F=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp47B;_tmp47B.tag=9;({struct Cyc_Absyn_FnInfo _tmp81E=({struct Cyc_Absyn_FnInfo _tmp47C;_tmp47C.tvars=0;_tmp47C.effect=_tmp489;_tmp47C.ret_tqual=_tmp488;_tmp47C.ret_typ=_tmp487;_tmp47C.args=_tmp486;_tmp47C.c_varargs=_tmp485;_tmp47C.cyc_varargs=_tmp484;_tmp47C.rgn_po=0;_tmp47C.attributes=_tmp482;_tmp47C.requires_clause=_tmp481;_tmp47C.requires_relns=_tmp480;_tmp47C.ensures_clause=_tmp47F;_tmp47C.ensures_relns=_tmp47E;_tmp47C;});_tmp47B.f1=_tmp81E;});_tmp47B;});_tmp47A[0]=_tmp81F;});_tmp47A;}));});
# 2575
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp473=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp477=_cycalloc(sizeof(*_tmp477));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp823=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp478;_tmp478.tag=5;({struct Cyc_Absyn_PtrInfo _tmp822=({struct Cyc_Absyn_PtrInfo _tmp479;_tmp479.elt_typ=ftyp;_tmp479.elt_tq=_tmp48F;({struct Cyc_Absyn_PtrAtts _tmp821=({(_tmp479.ptr_atts).rgn=_tmp48E;(_tmp479.ptr_atts).nullable=_tmp48D;(_tmp479.ptr_atts).bounds=_tmp48C;(_tmp479.ptr_atts).zero_term=_tmp48B;(_tmp479.ptr_atts).ptrloc=0;_tmp479.ptr_atts;});_tmp479.ptr_atts=_tmp821;});_tmp479;});_tmp478.f1=_tmp822;});_tmp478;});_tmp477[0]=_tmp823;});_tmp477;});
# 2577
struct Cyc_Absyn_Exp*_tmp474=Cyc_Absyn_copy_exp(e);
({void*_tmp825=(void*)({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_tmp475=_cycalloc(sizeof(*_tmp475));({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp824=({struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct _tmp476;_tmp476.tag=12;_tmp476.f1=_tmp474;_tmp476.f2=ts;_tmp476;});_tmp475[0]=_tmp824;});_tmp475;});e->r=_tmp825;});
e->topt=(void*)_tmp473;};}
# 2581
goto _LL2AE;}else{_LL2B1: _LL2B2:
 goto _LL2AE;}_LL2AE:;}
# 2584
goto _LL2A9;}else{_LL2AC: _LL2AD:
 goto _LL2A9;}_LL2A9:;}
# 2587
goto _LL2A4;}_LL2A4:;}
# 2589
return(void*)_check_null(e->topt);}
# 2597
static void Cyc_Tcexp_insert_alias_stmts(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*fn_exp,struct Cyc_List_List*alias_arg_exps){
# 2599
struct Cyc_List_List*_tmp492=0;
# 2601
{void*_tmp493=fn_exp->r;void*_tmp494=_tmp493;struct Cyc_List_List*_tmp4A8;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp494)->tag == 9){_LL2B4: _tmp4A8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp494)->f2;_LL2B5:
# 2603
{void*_tmp495=e->r;void*_tmp496=_tmp495;struct Cyc_List_List*_tmp4A5;if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->tag == 9){_LL2B9: _tmp4A5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp496)->f2;_LL2BA: {
# 2605
struct Cyc_List_List*_tmp497=alias_arg_exps;
int _tmp498=0;
while(_tmp497 != 0){
while(_tmp498 != (int)_tmp497->hd){
if(_tmp4A8 == 0)
({void*_tmp499=0;({struct _dyneither_ptr _tmp827=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp49D;_tmp49D.tag=1;_tmp49D.f1=(unsigned long)((int)_tmp497->hd);({struct Cyc_Int_pa_PrintArg_struct _tmp49C;_tmp49C.tag=1;_tmp49C.f1=(unsigned long)_tmp498;({void*_tmp49A[2]={& _tmp49C,& _tmp49D};({struct _dyneither_ptr _tmp826=({const char*_tmp49B="bad count %d/%d for alias coercion!";_tag_dyneither(_tmp49B,sizeof(char),36);});Cyc_aprintf(_tmp826,_tag_dyneither(_tmp49A,sizeof(void*),2));});});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp827,_tag_dyneither(_tmp499,sizeof(void*),0));});});
# 2612
++ _tmp498;
_tmp4A8=_tmp4A8->tl;
_tmp4A5=((struct Cyc_List_List*)_check_null(_tmp4A5))->tl;}{
# 2617
struct _tuple11 _tmp49E=({struct Cyc_Absyn_Exp*_tmp828=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4A8))->hd;Cyc_Tcutil_insert_alias(_tmp828,Cyc_Tcutil_copy_type((void*)_check_null(((struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4A5))->hd)->topt)));});struct _tuple11 _tmp49F=_tmp49E;struct Cyc_Absyn_Decl*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;_LL2BE: _tmp4A2=_tmp49F.f1;_tmp4A1=_tmp49F.f2;_LL2BF:;
_tmp4A8->hd=(void*)_tmp4A1;
({struct Cyc_List_List*_tmp829=({struct Cyc_List_List*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->hd=_tmp4A2;_tmp4A0->tl=_tmp492;_tmp4A0;});_tmp492=_tmp829;});
_tmp497=_tmp497->tl;};}
# 2622
goto _LL2B8;}}else{_LL2BB: _LL2BC:
({void*_tmp4A3=0;({struct _dyneither_ptr _tmp82A=({const char*_tmp4A4="not a function call!";_tag_dyneither(_tmp4A4,sizeof(char),21);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp82A,_tag_dyneither(_tmp4A3,sizeof(void*),0));});});}_LL2B8:;}
# 2625
goto _LL2B3;}else{_LL2B6: _LL2B7:
({void*_tmp4A6=0;({struct _dyneither_ptr _tmp82B=({const char*_tmp4A7="not a function call!";_tag_dyneither(_tmp4A7,sizeof(char),21);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp82B,_tag_dyneither(_tmp4A6,sizeof(void*),0));});});}_LL2B3:;}
# 2630
while(_tmp492 != 0){
struct Cyc_Absyn_Decl*_tmp4A9=(struct Cyc_Absyn_Decl*)_tmp492->hd;
({struct Cyc_Absyn_Exp*_tmp82F=({struct Cyc_Absyn_Stmt*_tmp82E=({struct Cyc_Absyn_Decl*_tmp82D=_tmp4A9;struct Cyc_Absyn_Stmt*_tmp82C=Cyc_Absyn_exp_stmt(fn_exp,e->loc);Cyc_Absyn_decl_stmt(_tmp82D,_tmp82C,e->loc);});Cyc_Absyn_stmt_exp(_tmp82E,e->loc);});fn_exp=_tmp82F;});
_tmp492=_tmp492->tl;}
# 2637
e->topt=0;
e->r=fn_exp->r;}
# 2643
static void Cyc_Tcexp_tcExpNoInst(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Exp*e){
unsigned int loc=e->loc;
void*t;
# 2647
{void*_tmp4AA=e->r;void*_tmp4AB=_tmp4AA;void*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct _dyneither_ptr*_tmp514;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp512;int*_tmp511;struct Cyc_Absyn_Exp**_tmp510;void***_tmp50F;struct Cyc_Absyn_Exp**_tmp50E;int*_tmp50D;void*_tmp50C;struct Cyc_Absyn_Enumfield*_tmp50B;struct Cyc_Absyn_Enumdecl*_tmp50A;struct Cyc_Absyn_Enumfield*_tmp509;struct Cyc_List_List*_tmp508;struct Cyc_Absyn_Datatypedecl*_tmp507;struct Cyc_Absyn_Datatypefield*_tmp506;void*_tmp505;struct Cyc_List_List*_tmp504;struct _tuple1**_tmp503;struct Cyc_List_List**_tmp502;struct Cyc_List_List*_tmp501;struct Cyc_Absyn_Aggrdecl**_tmp500;struct Cyc_Absyn_Exp*_tmp4FF;void*_tmp4FE;int*_tmp4FD;struct Cyc_Absyn_Vardecl*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;int*_tmp4F9;struct Cyc_Absyn_Stmt*_tmp4F8;struct Cyc_List_List*_tmp4F7;struct _tuple9*_tmp4F6;struct Cyc_List_List*_tmp4F5;struct Cyc_List_List*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;struct _dyneither_ptr*_tmp4F0;int*_tmp4EF;int*_tmp4EE;struct Cyc_Absyn_Exp*_tmp4ED;struct _dyneither_ptr*_tmp4EC;int*_tmp4EB;int*_tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;void*_tmp4E8;struct Cyc_List_List*_tmp4E7;void*_tmp4E6;struct Cyc_Absyn_Exp*_tmp4E5;struct Cyc_Absyn_Exp**_tmp4E4;struct Cyc_Absyn_Exp*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E2;void*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;enum Cyc_Absyn_Coercion*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_List_List*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Absyn_Exp*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D6;struct Cyc_Absyn_Exp*_tmp4D5;struct Cyc_Absyn_Exp*_tmp4D4;struct Cyc_Absyn_Exp*_tmp4D3;struct Cyc_Absyn_Exp*_tmp4D2;struct Cyc_Core_Opt*_tmp4D1;struct Cyc_Absyn_Exp*_tmp4D0;struct Cyc_Absyn_Exp*_tmp4CF;enum Cyc_Absyn_Incrementor _tmp4CE;enum Cyc_Absyn_Primop _tmp4CD;struct Cyc_List_List*_tmp4CC;void**_tmp4CB;union Cyc_Absyn_Cnst*_tmp4CA;struct Cyc_Core_Opt*_tmp4C9;struct Cyc_List_List*_tmp4C8;struct Cyc_Absyn_Exp*_tmp4C7;struct Cyc_List_List*_tmp4C6;struct Cyc_Absyn_VarargCallInfo**_tmp4C5;struct Cyc_Absyn_Exp*_tmp4C4;struct Cyc_List_List*_tmp4C3;struct Cyc_Absyn_VarargCallInfo**_tmp4C2;int*_tmp4C1;struct Cyc_Absyn_Exp*_tmp4C0;switch(*((int*)_tmp4AB)){case 11: _LL2C1: _tmp4C0=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2C2:
# 2652
 Cyc_Tcexp_tcExpNoInst(te,0,_tmp4C0);
return;case 9: _LL2C3: _tmp4C4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4C3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4C2=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_tmp4C1=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AB)->f4;if(!(*_tmp4C1)){_LL2C4:
# 2656
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));({struct Cyc_Core_Impossible_exn_struct _tmp831=({struct Cyc_Core_Impossible_exn_struct _tmp4AD;_tmp4AD.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp830=({const char*_tmp4AE="unresolved function in tcExpNoInst";_tag_dyneither(_tmp4AE,sizeof(char),35);});_tmp4AD.f1=_tmp830;});_tmp4AD;});_tmp4AC[0]=_tmp831;});_tmp4AC;}));}else{_LL2D9: _tmp4C7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4C6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4C5=(struct Cyc_Absyn_VarargCallInfo**)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_LL2DA: {
# 2687
struct Cyc_List_List*alias_arg_exps=0;
int ok=1;
struct Cyc_Absyn_Exp*fn_exp;
{struct _handler_cons _tmp4AF;_push_handler(& _tmp4AF);{int _tmp4B1=0;if(setjmp(_tmp4AF.handler))_tmp4B1=1;if(!_tmp4B1){
({struct Cyc_Absyn_Exp*_tmp832=Cyc_Tcutil_deep_copy_exp(0,e);fn_exp=_tmp832;});;_pop_handler();}else{void*_tmp4B0=(void*)_exn_thrown;void*_tmp4B2=_tmp4B0;void*_tmp4B3;if(((struct Cyc_Core_Failure_exn_struct*)_tmp4B2)->tag == Cyc_Core_Failure){_LL314: _LL315:
# 2694
 ok=0;
fn_exp=e;
goto _LL313;}else{_LL316: _tmp4B3=_tmp4B2;_LL317:(int)_rethrow(_tmp4B3);}_LL313:;}};}
# 2698
({void*_tmp833=Cyc_Tcexp_tcFnCall(te,loc,topt,_tmp4C7,_tmp4C6,_tmp4C5,& alias_arg_exps);t=_tmp833;});
if(alias_arg_exps != 0  && ok){
({struct Cyc_List_List*_tmp834=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(alias_arg_exps);alias_arg_exps=_tmp834;});
Cyc_Tcexp_insert_alias_stmts(te,e,fn_exp,alias_arg_exps);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;}
# 2705
goto _LL2C0;}}case 35: _LL2C5: _tmp4C9=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4C8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2C6:
# 2660
 Cyc_Tcexp_resolve_unresolved_mem(te,loc,topt,e,_tmp4C8);
Cyc_Tcexp_tcExpNoInst(te,topt,e);
return;case 0: _LL2C7: _tmp4CA=(union Cyc_Absyn_Cnst*)&((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2C8:
# 2665
({void*_tmp835=Cyc_Tcexp_tcConst(te,loc,topt,_tmp4CA,e);t=_tmp835;});goto _LL2C0;case 1: _LL2C9: _tmp4CB=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2CA:
# 2667
({void*_tmp836=Cyc_Tcexp_tcVar(te,loc,topt,e,_tmp4CB);t=_tmp836;});goto _LL2C0;case 2: _LL2CB: _tmp4CD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4CC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2CC:
# 2669
({void*_tmp837=Cyc_Tcexp_tcPrimop(te,loc,topt,_tmp4CD,_tmp4CC);t=_tmp837;});goto _LL2C0;case 4: _LL2CD: _tmp4CF=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4CE=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2CE:
# 2671
({void*_tmp838=Cyc_Tcexp_tcIncrement(te,loc,topt,_tmp4CF,_tmp4CE);t=_tmp838;});goto _LL2C0;case 3: _LL2CF: _tmp4D2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4D1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4D0=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_LL2D0:
# 2673
({void*_tmp839=Cyc_Tcexp_tcAssignOp(te,loc,topt,_tmp4D2,_tmp4D1,_tmp4D0);t=_tmp839;});goto _LL2C0;case 5: _LL2D1: _tmp4D5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4D4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4D3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_LL2D2:
# 2675
({void*_tmp83A=Cyc_Tcexp_tcConditional(te,loc,topt,_tmp4D5,_tmp4D4,_tmp4D3);t=_tmp83A;});goto _LL2C0;case 6: _LL2D3: _tmp4D7=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4D6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2D4:
# 2677
({void*_tmp83B=Cyc_Tcexp_tcAnd(te,loc,_tmp4D7,_tmp4D6);t=_tmp83B;});goto _LL2C0;case 7: _LL2D5: _tmp4D9=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4D8=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2D6:
# 2679
({void*_tmp83C=Cyc_Tcexp_tcOr(te,loc,_tmp4D9,_tmp4D8);t=_tmp83C;});goto _LL2C0;case 8: _LL2D7: _tmp4DB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4DA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2D8:
# 2681
({void*_tmp83D=Cyc_Tcexp_tcSeqExp(te,loc,topt,_tmp4DB,_tmp4DA);t=_tmp83D;});goto _LL2C0;case 10: _LL2DB: _tmp4DC=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2DC:
# 2707
({void*_tmp83E=Cyc_Tcexp_tcThrow(te,loc,topt,_tmp4DC);t=_tmp83E;});goto _LL2C0;case 12: _LL2DD: _tmp4DE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4DD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2DE:
# 2709
({void*_tmp83F=Cyc_Tcexp_tcInstantiate(te,loc,topt,_tmp4DE,_tmp4DD);t=_tmp83F;});goto _LL2C0;case 13: _LL2DF: _tmp4E1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4E0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4DF=(enum Cyc_Absyn_Coercion*)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp4AB)->f4;_LL2E0:
# 2711
({void*_tmp840=Cyc_Tcexp_tcCast(te,loc,topt,_tmp4E1,_tmp4E0,_tmp4DF);t=_tmp840;});goto _LL2C0;case 14: _LL2E1: _tmp4E2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2E2:
# 2713
({void*_tmp841=Cyc_Tcexp_tcAddress(te,loc,e,topt,_tmp4E2);t=_tmp841;});goto _LL2C0;case 15: _LL2E3: _tmp4E4=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4E3=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2E4:
# 2715
({void*_tmp842=Cyc_Tcexp_tcNew(te,loc,topt,_tmp4E4,e,_tmp4E3);t=_tmp842;});goto _LL2C0;case 17: _LL2E5: _tmp4E5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2E6: {
# 2717
void*_tmp4B4=Cyc_Tcexp_tcExpNoPromote(te,0,_tmp4E5);
({void*_tmp843=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4B4);t=_tmp843;});goto _LL2C0;}case 16: _LL2E7: _tmp4E6=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2E8:
# 2720
({void*_tmp844=Cyc_Tcexp_tcSizeof(te,loc,topt,_tmp4E6);t=_tmp844;});goto _LL2C0;case 18: _LL2E9: _tmp4E8=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4E7=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2EA:
# 2722
({void*_tmp845=Cyc_Tcexp_tcOffsetof(te,loc,topt,_tmp4E8,_tmp4E7);t=_tmp845;});goto _LL2C0;case 19: _LL2EB: _tmp4E9=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2EC:
# 2724
({void*_tmp846=Cyc_Tcexp_tcDeref(te,loc,topt,_tmp4E9);t=_tmp846;});goto _LL2C0;case 20: _LL2ED: _tmp4ED=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4EC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4EB=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_tmp4EA=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4AB)->f4;_LL2EE:
# 2726
({void*_tmp847=Cyc_Tcexp_tcAggrMember(te,loc,topt,e,_tmp4ED,_tmp4EC,_tmp4EB,_tmp4EA);t=_tmp847;});goto _LL2C0;case 21: _LL2EF: _tmp4F1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4F0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4EF=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_tmp4EE=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4AB)->f4;_LL2F0:
# 2728
({void*_tmp848=Cyc_Tcexp_tcAggrArrow(te,loc,topt,_tmp4F1,_tmp4F0,_tmp4EF,_tmp4EE);t=_tmp848;});goto _LL2C0;case 22: _LL2F1: _tmp4F3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4F2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2F2:
# 2730
({void*_tmp849=Cyc_Tcexp_tcSubscript(te,loc,topt,_tmp4F3,_tmp4F2);t=_tmp849;});goto _LL2C0;case 23: _LL2F3: _tmp4F4=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2F4:
# 2732
({void*_tmp84A=Cyc_Tcexp_tcTuple(te,loc,topt,_tmp4F4);t=_tmp84A;});goto _LL2C0;case 24: _LL2F5: _tmp4F6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4F5=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL2F6:
# 2734
({void*_tmp84B=Cyc_Tcexp_tcCompoundLit(te,loc,topt,_tmp4F6,_tmp4F5);t=_tmp84B;});goto _LL2C0;case 25: _LL2F7: _tmp4F7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2F8: {
# 2738
void**elt_topt=0;
struct Cyc_Absyn_Tqual*elt_tqopt=0;
int zero_term=0;
if(topt != 0){
void*_tmp4B5=Cyc_Tcutil_compress(*topt);void*_tmp4B6=_tmp4B5;void**_tmp4B9;struct Cyc_Absyn_Tqual*_tmp4B8;union Cyc_Absyn_Constraint*_tmp4B7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B6)->tag == 8){_LL319: _tmp4B9=(void**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B6)->f1).elt_type;_tmp4B8=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B6)->f1).tq;_tmp4B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4B6)->f1).zero_term;_LL31A:
# 2744
 elt_topt=_tmp4B9;
elt_tqopt=_tmp4B8;
({int _tmp84C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp4B7);zero_term=_tmp84C;});
goto _LL318;}else{_LL31B: _LL31C:
 goto _LL318;}_LL318:;}
# 2751
({void*_tmp84D=Cyc_Tcexp_tcArray(te,loc,elt_topt,elt_tqopt,zero_term,_tmp4F7);t=_tmp84D;});goto _LL2C0;}case 36: _LL2F9: _tmp4F8=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL2FA:
# 2753
({void*_tmp84E=Cyc_Tcexp_tcStmtExp(te,loc,topt,_tmp4F8);t=_tmp84E;});goto _LL2C0;case 26: _LL2FB: _tmp4FC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4FB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4FA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_tmp4F9=(int*)&((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp4AB)->f4;_LL2FC:
# 2755
({void*_tmp84F=Cyc_Tcexp_tcComprehension(te,loc,topt,_tmp4FC,_tmp4FB,_tmp4FA,_tmp4F9);t=_tmp84F;});goto _LL2C0;case 27: _LL2FD: _tmp4FF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp4FE=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp4FD=(int*)&((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_LL2FE:
# 2757
({void*_tmp850=Cyc_Tcexp_tcComprehensionNoinit(te,loc,topt,_tmp4FF,_tmp4FE,_tmp4FD);t=_tmp850;});goto _LL2C0;case 28: _LL2FF: _tmp503=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp502=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp501=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_tmp500=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp4AB)->f4;_LL300:
# 2759
({void*_tmp851=Cyc_Tcexp_tcAggregate(te,loc,topt,_tmp503,_tmp502,_tmp501,_tmp500);t=_tmp851;});goto _LL2C0;case 29: _LL301: _tmp505=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp504=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL302:
# 2761
({void*_tmp852=Cyc_Tcexp_tcAnonStruct(te,loc,_tmp505,_tmp504);t=_tmp852;});goto _LL2C0;case 30: _LL303: _tmp508=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp507=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_tmp506=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp4AB)->f3;_LL304:
# 2763
({void*_tmp853=Cyc_Tcexp_tcDatatype(te,loc,topt,e,_tmp508,_tmp507,_tmp506);t=_tmp853;});goto _LL2C0;case 31: _LL305: _tmp50A=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp509=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL306:
# 2765
({void*_tmp855=(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp854=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp4BB;_tmp4BB.tag=13;_tmp4BB.f1=_tmp50A->name;_tmp4BB.f2=_tmp50A;_tmp4BB;});_tmp4BA[0]=_tmp854;});_tmp4BA;});t=_tmp855;});goto _LL2C0;case 32: _LL307: _tmp50C=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp50B=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL308:
# 2767
 t=_tmp50C;goto _LL2C0;case 33: _LL309: _tmp511=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1).is_calloc;_tmp510=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1).rgn;_tmp50F=(void***)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1).elt_type;_tmp50E=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1).num_elts;_tmp50D=(int*)&(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1).fat_result;_LL30A:
# 2769
({void*_tmp856=Cyc_Tcexp_tcMalloc(te,loc,topt,_tmp510,_tmp50F,_tmp50E,_tmp511,_tmp50D);t=_tmp856;});goto _LL2C0;case 34: _LL30B: _tmp513=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp512=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL30C:
# 2771
({void*_tmp857=Cyc_Tcexp_tcSwap(te,loc,topt,_tmp513,_tmp512);t=_tmp857;});goto _LL2C0;case 37: _LL30D: _tmp515=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_tmp514=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp4AB)->f2;_LL30E:
# 2773
({void*_tmp858=Cyc_Tcexp_tcTagcheck(te,loc,topt,_tmp515,_tmp514);t=_tmp858;});goto _LL2C0;case 38: _LL30F: _tmp516=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp4AB)->f1;_LL310:
# 2775
 if(!te->allow_valueof)
({void*_tmp4BC=0;({unsigned int _tmp85A=e->loc;struct _dyneither_ptr _tmp859=({const char*_tmp4BD="valueof(-) can only occur within types";_tag_dyneither(_tmp4BD,sizeof(char),39);});Cyc_Tcutil_terr(_tmp85A,_tmp859,_tag_dyneither(_tmp4BC,sizeof(void*),0));});});
# 2783
t=Cyc_Absyn_sint_typ;
goto _LL2C0;default: _LL311: _LL312:
# 2786
({void*_tmp4BE=0;({unsigned int _tmp85C=e->loc;struct _dyneither_ptr _tmp85B=({const char*_tmp4BF="asm expressions cannot occur within Cyclone code.";_tag_dyneither(_tmp4BF,sizeof(char),50);});Cyc_Tcutil_terr(_tmp85C,_tmp85B,_tag_dyneither(_tmp4BE,sizeof(void*),0));});});
({void*_tmp85D=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp85D;});}_LL2C0:;}
# 2789
e->topt=t;}
