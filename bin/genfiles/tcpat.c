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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 379
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 64
struct _dyneither_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_PP_Ppstate;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 102
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 316
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 428 "absyn.h"
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 443
typedef void*Cyc_Absyn_funcparams_t;
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 510
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 924
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 930
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 938
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 944
void*Cyc_Absyn_compress_kb(void*);
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 951
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 956
extern void*Cyc_Absyn_sint_typ;
# 958
void*Cyc_Absyn_float_typ(int);
# 960
extern void*Cyc_Absyn_empty_effect;
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1077
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 72
struct _dyneither_ptr Cyc_Absynpp_pat2string(struct Cyc_Absyn_Pat*p);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 63
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt);
# 94
int Cyc_Set_cardinality(struct Cyc_Set_Set*s);
# 97
int Cyc_Set_is_empty(struct Cyc_Set_Set*s);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};
# 137
void*Cyc_Set_choose(struct Cyc_Set_Set*s);struct Cyc_RgnOrder_RgnPO;
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
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 137
void Cyc_Tcenv_check_rgn_accessible(struct Cyc_Tcenv_Tenv*,unsigned int,void*rgn);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 45
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 48
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 53
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 58
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 64
int Cyc_Tcutil_coerceable(void*);
# 71
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 95
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 111
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
# 114
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
# 145
int Cyc_Tcutil_unify(void*,void*);
# 148
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);struct _tuple12{struct Cyc_List_List*f1;struct _RegionHandle*f2;};struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 177
struct _tuple13*Cyc_Tcutil_r_make_inst_var(struct _tuple12*,struct Cyc_Absyn_Tvar*);
# 219 "tcutil.h"
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 222
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
# 237
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 262
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 270
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 290
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 354
int Cyc_Tcutil_is_array(void*t);
# 358
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 35
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 49 "tcpat.h"
typedef struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcpat_result_t;
# 53
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 55
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};
# 70
typedef void*Cyc_Tcpat_pat_test_t;struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};
# 78
typedef void*Cyc_Tcpat_access_t;struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};
# 88
typedef struct Cyc_Tcpat_PathNode*Cyc_Tcpat_path_node_t;
# 90
typedef void*Cyc_Tcpat_term_desc_t;
typedef struct Cyc_List_List*Cyc_Tcpat_path_t;struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};
# 98
typedef struct Cyc_Tcpat_Rhs*Cyc_Tcpat_rhs_t;
# 100
typedef void*Cyc_Tcpat_decision_t;struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 107
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 109
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p,void**);
# 111
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 113
void Cyc_Tcpat_print_decision_tree(void*);
# 115
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 42 "tcpat.cyc"
static void Cyc_Tcpat_resolve_pat(struct Cyc_Tcenv_Tenv*te,void**topt,struct Cyc_Absyn_Pat*p){
void*_tmp0=p->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Aggrdecl**_tmp31;struct Cyc_List_List*_tmp30;struct Cyc_List_List**_tmp2F;struct Cyc_List_List*_tmp2E;int _tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2B;int _tmp2A;switch(*((int*)_tmp1)){case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1 == 0){_LL1: _tmp2C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp2B=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp2A=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL2:
# 48
 if(topt == 0)
({void*_tmp2=0;({unsigned int _tmp3E0=p->loc;struct _dyneither_ptr _tmp3DF=({const char*_tmp3="cannot determine pattern type";_tag_dyneither(_tmp3,sizeof(char),30);});Cyc_Tcutil_terr(_tmp3E0,_tmp3DF,_tag_dyneither(_tmp2,sizeof(void*),0));});});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp4=t;struct Cyc_Absyn_AggrInfo _tmpB;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->tag == 11){_LL10: _tmpB=((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1;_LL11:
# 53
({void*_tmp3E3=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3E2=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp6;_tmp6.tag=7;({struct Cyc_Absyn_AggrInfo*_tmp3E1=({struct Cyc_Absyn_AggrInfo*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=_tmpB;_tmp7;});_tmp6.f1=_tmp3E1;});_tmp6.f2=_tmp2C;_tmp6.f3=_tmp2B;_tmp6.f4=_tmp2A;_tmp6;});_tmp5[0]=_tmp3E2;});_tmp5;});p->r=_tmp3E3;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}else{_LL12: _LL13:
# 57
({struct Cyc_String_pa_PrintArg_struct _tmpA;_tmpA.tag=0;({struct _dyneither_ptr _tmp3E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpA.f1=_tmp3E4;});({void*_tmp8[1]={& _tmpA};({unsigned int _tmp3E6=p->loc;struct _dyneither_ptr _tmp3E5=({const char*_tmp9="pattern expects aggregate type instead of %s";_tag_dyneither(_tmp9,sizeof(char),45);});Cyc_Tcutil_terr(_tmp3E6,_tmp3E5,_tag_dyneither(_tmp8,sizeof(void*),1));});});});
goto _LLF;}_LLF:;}
# 60
return;};}else{if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).KnownAggr).tag == 2){_LL3: _tmp31=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).KnownAggr).val;_tmp30=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->targs;_tmp2F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp2E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp2D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {
# 63
struct Cyc_Absyn_Aggrdecl*_tmpC=*_tmp31;
if(_tmpC->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;({struct _dyneither_ptr _tmp3E7=(struct _dyneither_ptr)(
_tmpC->kind == Cyc_Absyn_StructA?({const char*_tmp10="struct";_tag_dyneither(_tmp10,sizeof(char),7);}):({const char*_tmp11="union";_tag_dyneither(_tmp11,sizeof(char),6);}));_tmpF.f1=_tmp3E7;});({void*_tmpD[1]={& _tmpF};({unsigned int _tmp3E9=p->loc;struct _dyneither_ptr _tmp3E8=({const char*_tmpE="can't destructure an abstract %s";_tag_dyneither(_tmpE,sizeof(char),33);});Cyc_Tcutil_terr(_tmp3E9,_tmp3E8,_tag_dyneither(_tmpD,sizeof(void*),1));});});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 70
int more_exists=({int _tmp3EA=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC->impl))->exist_vars);_tmp3EA - ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp2F);});
if(more_exists < 0){
({void*_tmp12=0;({unsigned int _tmp3EC=p->loc;struct _dyneither_ptr _tmp3EB=({const char*_tmp13="too many existentially bound type variables in pattern";_tag_dyneither(_tmp13,sizeof(char),55);});Cyc_Tcutil_terr(_tmp3EC,_tmp3EB,_tag_dyneither(_tmp12,sizeof(void*),0));});});{
struct Cyc_List_List**_tmp14=_tmp2F;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC->impl))->exist_vars);for(0;n != 0;-- n){
_tmp14=&((struct Cyc_List_List*)_check_null(*_tmp14))->tl;}}
*_tmp14=0;};}else{
if(more_exists > 0){
# 79
struct Cyc_List_List*_tmp15=0;
for(0;more_exists != 0;-- more_exists){
({struct Cyc_List_List*_tmp3EF=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct Cyc_Absyn_Tvar*_tmp3EE=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp17=_cycalloc(sizeof(*_tmp17));({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3ED=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp18;_tmp18.tag=1;_tmp18.f1=0;_tmp18;});_tmp17[0]=_tmp3ED;});_tmp17;}));_tmp16->hd=_tmp3EE;});_tmp16->tl=_tmp15;_tmp16;});_tmp15=_tmp3EF;});}
({struct Cyc_List_List*_tmp3F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp2F,_tmp15);*_tmp2F=_tmp3F0;});}}
# 84
return;};}}else{_LLB: _LLC:
# 97
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp27=_cycalloc(sizeof(*_tmp27));({struct Cyc_Core_Impossible_exn_struct _tmp3F2=({struct Cyc_Core_Impossible_exn_struct _tmp28;_tmp28.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp3F1=({const char*_tmp29="resolve_pat unknownAggr";_tag_dyneither(_tmp29,sizeof(char),24);});_tmp28.f1=_tmp3F1;});_tmp28;});_tmp27[0]=_tmp3F2;});_tmp27;}));}}case 17: _LL5: _tmp32=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6:
# 86
 Cyc_Tcexp_tcExp(te,0,_tmp32);
if(!Cyc_Tcutil_is_const_exp(_tmp32)){
({void*_tmp19=0;({unsigned int _tmp3F4=p->loc;struct _dyneither_ptr _tmp3F3=({const char*_tmp1A="non-constant expression in case pattern";_tag_dyneither(_tmp1A,sizeof(char),40);});Cyc_Tcutil_terr(_tmp3F4,_tmp3F3,_tag_dyneither(_tmp19,sizeof(void*),0));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 91
struct _tuple14 _tmp1B=Cyc_Evexp_eval_const_uint_exp(_tmp32);struct _tuple14 _tmp1C=_tmp1B;unsigned int _tmp20;int _tmp1F;_LL15: _tmp20=_tmp1C.f1;_tmp1F=_tmp1C.f2;_LL16:;
({void*_tmp3F6=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D=_cycalloc_atomic(sizeof(*_tmp1D));({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3F5=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp1E;_tmp1E.tag=10;_tmp1E.f1=Cyc_Absyn_None;_tmp1E.f2=(int)_tmp20;_tmp1E;});_tmp1D[0]=_tmp3F5;});_tmp1D;});p->r=_tmp3F6;});
return;};case 15: _LL7: _LL8:
# 95
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp21=_cycalloc(sizeof(*_tmp21));({struct Cyc_Core_Impossible_exn_struct _tmp3F8=({struct Cyc_Core_Impossible_exn_struct _tmp22;_tmp22.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp3F7=({const char*_tmp23="resolve_pat UnknownId_p";_tag_dyneither(_tmp23,sizeof(char),24);});_tmp22.f1=_tmp3F7;});_tmp22;});_tmp21[0]=_tmp3F8;});_tmp21;}));case 16: _LL9: _LLA:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));({struct Cyc_Core_Impossible_exn_struct _tmp3FA=({struct Cyc_Core_Impossible_exn_struct _tmp25;_tmp25.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp3F9=({const char*_tmp26="resolve_pat UnknownCall_p";_tag_dyneither(_tmp26,sizeof(char),26);});_tmp25.f1=_tmp3F9;});_tmp25;});_tmp24[0]=_tmp3FA;});_tmp24;}));default: _LLD: _LLE:
# 99
 return;}_LL0:;}
# 103
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 106
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->v=s;_tmp33;}));}
# 111
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 115
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 118
{void*_tmp34=Cyc_Tcutil_compress(numt);void*_tmp35=_tmp34;switch(*((int*)_tmp35)){case 13: _LL18: _LL19:
 goto _LL1B;case 14: _LL1A: _LL1B:
 if(topt != 0)return*topt;goto _LL17;default: _LL1C: _LL1D:
 goto _LL17;}_LL17:;}
# 123
return numt;}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 126
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 128
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp3FB=Cyc_Absyn_empty_tqual(0);(*vd)->tq=_tmp3FB;});}
# 132
({struct Cyc_List_List*_tmp3FD=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _tuple15*_tmp3FC=({struct _tuple15*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=vd;_tmp37->f2=e;_tmp37;});_tmp36->hd=_tmp3FC;});_tmp36->tl=*v_result_ptr;_tmp36;});*v_result_ptr=_tmp3FD;});}
# 134
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 136
struct Cyc_Tcpat_TcPatResult _tmp38=res1;struct _tuple1*_tmp42;struct Cyc_List_List*_tmp41;_LL1F: _tmp42=_tmp38.tvars_and_bounds_opt;_tmp41=_tmp38.patvars;_LL20:;{
struct Cyc_Tcpat_TcPatResult _tmp39=res2;struct _tuple1*_tmp40;struct Cyc_List_List*_tmp3F;_LL22: _tmp40=_tmp39.tvars_and_bounds_opt;_tmp3F=_tmp39.patvars;_LL23:;
if(_tmp42 != 0  || _tmp40 != 0){
if(_tmp42 == 0)
({struct _tuple1*_tmp3FE=({struct _tuple1*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->f1=0;_tmp3A->f2=0;_tmp3A;});_tmp42=_tmp3FE;});
if(_tmp40 == 0)
({struct _tuple1*_tmp3FF=({struct _tuple1*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=0;_tmp3B->f2=0;_tmp3B;});_tmp40=_tmp3FF;});
return({struct Cyc_Tcpat_TcPatResult _tmp3C;({struct _tuple1*_tmp402=({struct _tuple1*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct Cyc_List_List*_tmp400=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp42).f1,(*_tmp40).f1);_tmp3D->f1=_tmp400;});({struct Cyc_List_List*_tmp401=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp42).f2,(*_tmp40).f2);_tmp3D->f2=_tmp401;});_tmp3D;});_tmp3C.tvars_and_bounds_opt=_tmp402;});({struct Cyc_List_List*_tmp403=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp41,_tmp3F);_tmp3C.patvars=_tmp403;});_tmp3C;});}
# 147
return({struct Cyc_Tcpat_TcPatResult _tmp3E;_tmp3E.tvars_and_bounds_opt=0;({struct Cyc_List_List*_tmp404=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp41,_tmp3F);_tmp3E.patvars=_tmp404;});_tmp3E;});};}
# 150
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
return({struct Cyc_Absyn_Pat*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->loc=loc;_tmp43->topt=0;_tmp43->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp43;});}
# 155
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array(t)?({
void*_tmp406=t;Cyc_Tcutil_promote_array(_tmp406,rgn_opt == 0?Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*_tmp44));({struct Cyc_List_List*_tmp405=Cyc_Tcenv_lookup_type_vars(te);_tmp44->v=_tmp405;});_tmp44;})): rgn_opt,0);}): t;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};
# 162
static struct _tuple16*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple16*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=tv;_tmp45->f2=0;_tmp45;});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 166
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 169
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp118;_tmp118.tvars_and_bounds_opt=0;_tmp118.patvars=0;_tmp118;});
# 174
{void*_tmp46=p->r;void*_tmp47=_tmp46;struct Cyc_Absyn_Datatypedecl*_tmp117;struct Cyc_Absyn_Datatypefield*_tmp116;struct Cyc_List_List**_tmp115;int _tmp114;struct Cyc_Absyn_Aggrdecl*_tmp113;struct Cyc_List_List**_tmp112;struct Cyc_List_List*_tmp111;struct Cyc_List_List**_tmp110;int _tmp10F;struct Cyc_List_List**_tmp10E;int _tmp10D;struct Cyc_Absyn_Pat*_tmp10C;void*_tmp10B;struct Cyc_Absyn_Enumdecl*_tmp10A;int _tmp109;struct Cyc_Absyn_Tvar*_tmp108;struct Cyc_Absyn_Vardecl*_tmp107;struct Cyc_Absyn_Vardecl*_tmp106;struct Cyc_Absyn_Pat*_tmp105;struct Cyc_Absyn_Tvar*_tmp104;struct Cyc_Absyn_Vardecl*_tmp103;struct Cyc_Absyn_Vardecl*_tmp102;struct Cyc_Absyn_Pat*_tmp101;switch(*((int*)_tmp47)){case 0: _LL25: _LL26:
# 177
 if(topt != 0)
t=*topt;else{
# 180
({void*_tmp408=({struct Cyc_List_List*_tmp407=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp407,topt);});t=_tmp408;});}
goto _LL24;case 1: _LL27: _tmp102=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp101=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL28: {
# 184
struct _tuple2*_tmp48=_tmp102->name;struct _tuple2*_tmp49=_tmp48;struct _dyneither_ptr _tmp56;_LL52: _tmp56=*_tmp49->f2;_LL53:;
if(({struct _dyneither_ptr _tmp40A=(struct _dyneither_ptr)_tmp56;Cyc_strcmp(_tmp40A,({const char*_tmp4A="true";_tag_dyneither(_tmp4A,sizeof(char),5);}));})== 0  || ({struct _dyneither_ptr _tmp409=(struct _dyneither_ptr)_tmp56;Cyc_strcmp(_tmp409,({const char*_tmp4B="false";_tag_dyneither(_tmp4B,sizeof(char),6);}));})== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp56);({void*_tmp4C[1]={& _tmp4E};({unsigned int _tmp40C=p->loc;struct _dyneither_ptr _tmp40B=({const char*_tmp4D="you probably do not want to use %s as a local variable...";_tag_dyneither(_tmp4D,sizeof(char),58);});Cyc_Tcutil_warn(_tmp40C,_tmp40B,_tag_dyneither(_tmp4C,sizeof(void*),1));});});});
# 188
({struct Cyc_Tcpat_TcPatResult _tmp40D=Cyc_Tcpat_tcPatRec(te,_tmp101,topt,rgn_pat,allow_ref_pat,access_exp);res=_tmp40D;});
t=(void*)_check_null(_tmp101->topt);
# 192
{void*_tmp4F=Cyc_Tcutil_compress(t);void*_tmp50=_tmp4F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50)->tag == 8){_LL55: _LL56:
# 194
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp51=0;({unsigned int _tmp40F=p->loc;struct _dyneither_ptr _tmp40E=({const char*_tmp52="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp52,sizeof(char),58);});Cyc_Tcutil_terr(_tmp40F,_tmp40E,_tag_dyneither(_tmp51,sizeof(void*),0));});});
goto _LL54;}else{_LL57: _LL58:
# 198
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp53=0;({unsigned int _tmp411=p->loc;struct _dyneither_ptr _tmp410=({const char*_tmp54="pattern would point to an abstract member";_tag_dyneither(_tmp54,sizeof(char),42);});Cyc_Tcutil_terr(_tmp411,_tmp410,_tag_dyneither(_tmp53,sizeof(void*),0));});});
goto _LL54;}_LL54:;}
# 202
({struct Cyc_Absyn_Vardecl**_tmp414=({struct Cyc_Absyn_Vardecl**_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=_tmp102;_tmp55;});struct Cyc_Absyn_Exp*_tmp413=access_exp;struct Cyc_List_List**_tmp412=& res.patvars;Cyc_Tcpat_set_vd(_tmp414,_tmp413,_tmp412,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL24;}case 2: _LL29: _tmp104=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp103=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2A: {
# 205
struct Cyc_Tcenv_Tenv*te2=({unsigned int _tmp416=p->loc;struct Cyc_Tcenv_Tenv*_tmp415=te;Cyc_Tcenv_add_type_vars(_tmp416,_tmp415,({struct Cyc_Absyn_Tvar*_tmp61[1];_tmp61[0]=_tmp104;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp417=({struct _tuple1*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=0;_tmp57->f2=0;_tmp57;});res.tvars_and_bounds_opt=_tmp417;});
({struct Cyc_List_List*_tmp41A=({
struct Cyc_List_List*_tmp419=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp419,({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));({struct _tuple16*_tmp418=({struct _tuple16*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=_tmp104;_tmp59->f2=1;_tmp59;});_tmp58->hd=_tmp418;});_tmp58->tl=0;_tmp58;}));});
# 208
(*res.tvars_and_bounds_opt).f1=_tmp41A;});
# 210
({unsigned int _tmp41D=p->loc;struct Cyc_Tcenv_Tenv*_tmp41C=te2;struct Cyc_List_List*_tmp41B=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp41D,_tmp41C,_tmp41B,& Cyc_Tcutil_tmk,1,0,_tmp103->type);});
# 213
if(topt != 0)t=*topt;else{
({void*_tmp41F=({struct Cyc_List_List*_tmp41E=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp41E,topt);});t=_tmp41F;});}
{void*_tmp5A=Cyc_Tcutil_compress(t);void*_tmp5B=_tmp5A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->tag == 8){_LL5A: _LL5B:
# 217
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp5C=0;({unsigned int _tmp421=p->loc;struct _dyneither_ptr _tmp420=({const char*_tmp5D="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp5D,sizeof(char),58);});Cyc_Tcutil_terr(_tmp421,_tmp420,_tag_dyneither(_tmp5C,sizeof(void*),0));});});
goto _LL59;}else{_LL5C: _LL5D:
# 221
 if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk))
({void*_tmp5E=0;({unsigned int _tmp423=p->loc;struct _dyneither_ptr _tmp422=({const char*_tmp5F="pattern would point to an abstract member";_tag_dyneither(_tmp5F,sizeof(char),42);});Cyc_Tcutil_terr(_tmp423,_tmp422,_tag_dyneither(_tmp5E,sizeof(void*),0));});});
goto _LL59;}_LL59:;}
# 225
({struct Cyc_Absyn_Vardecl**_tmp426=({struct Cyc_Absyn_Vardecl**_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=_tmp103;_tmp60;});struct Cyc_Absyn_Exp*_tmp425=access_exp;struct Cyc_List_List**_tmp424=& res.patvars;Cyc_Tcpat_set_vd(_tmp426,_tmp425,_tmp424,_tmp103->type);});
goto _LL24;}case 3: _LL2B: _tmp106=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp105=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2C:
# 229
({struct Cyc_Tcpat_TcPatResult _tmp427=Cyc_Tcpat_tcPatRec(te,_tmp105,topt,rgn_pat,allow_ref_pat,access_exp);res=_tmp427;});
t=(void*)_check_null(_tmp105->topt);
if(!allow_ref_pat  || rgn_pat == 0){
({void*_tmp62=0;({unsigned int _tmp429=p->loc;struct _dyneither_ptr _tmp428=({const char*_tmp63="* pattern would point into an unknown/unallowed region";_tag_dyneither(_tmp63,sizeof(char),55);});Cyc_Tcutil_terr(_tmp429,_tmp428,_tag_dyneither(_tmp62,sizeof(void*),0));});});
goto _LL24;}else{
# 236
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
({void*_tmp64=0;({unsigned int _tmp42B=p->loc;struct _dyneither_ptr _tmp42A=({const char*_tmp65="* pattern cannot take the address of an alias-free path";_tag_dyneither(_tmp65,sizeof(char),56);});Cyc_Tcutil_terr(_tmp42B,_tmp42A,_tag_dyneither(_tmp64,sizeof(void*),0));});});}{
# 239
struct Cyc_Absyn_Exp*new_access_exp=0;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp431=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp68;_tmp68.tag=5;({struct Cyc_Absyn_PtrInfo _tmp430=({struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=t;({struct Cyc_Absyn_Tqual _tmp42C=Cyc_Absyn_empty_tqual(0);_tmp69.elt_tq=_tmp42C;});({struct Cyc_Absyn_PtrAtts _tmp42F=({(_tmp69.ptr_atts).rgn=rgn_pat;(_tmp69.ptr_atts).nullable=Cyc_Absyn_false_conref;({union Cyc_Absyn_Constraint*_tmp42D=
# 242
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp69.ptr_atts).bounds=_tmp42D;});({union Cyc_Absyn_Constraint*_tmp42E=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp69.ptr_atts).zero_term=_tmp42E;});(_tmp69.ptr_atts).ptrloc=0;_tmp69.ptr_atts;});_tmp69.ptr_atts=_tmp42F;});_tmp69;});_tmp68.f1=_tmp430;});_tmp68;});_tmp67[0]=_tmp431;});_tmp67;});
# 244
if((unsigned int)access_exp){
({struct Cyc_Absyn_Exp*_tmp432=Cyc_Absyn_address_exp(access_exp,0);new_access_exp=_tmp432;});
new_access_exp->topt=t2;}
# 248
({struct Cyc_Absyn_Vardecl**_tmp435=({struct Cyc_Absyn_Vardecl**_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66[0]=_tmp106;_tmp66;});struct Cyc_Absyn_Exp*_tmp434=new_access_exp;struct Cyc_List_List**_tmp433=& res.patvars;Cyc_Tcpat_set_vd(_tmp435,_tmp434,_tmp433,t2);});
goto _LL24;};case 4: _LL2D: _tmp108=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp107=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2E:
# 254
({struct Cyc_Absyn_Vardecl**_tmp438=({struct Cyc_Absyn_Vardecl**_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=_tmp107;_tmp6A;});struct Cyc_Absyn_Exp*_tmp437=access_exp;struct Cyc_List_List**_tmp436=& res.patvars;Cyc_Tcpat_set_vd(_tmp438,_tmp437,_tmp436,_tmp107->type);});
# 258
({unsigned int _tmp43A=p->loc;struct Cyc_Tcenv_Tenv*_tmp439=te;Cyc_Tcenv_add_type_vars(_tmp43A,_tmp439,({struct Cyc_Absyn_Tvar*_tmp6B[1];_tmp6B[0]=_tmp108;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6B,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp43B=({struct _tuple1*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->f1=0;_tmp6C->f2=0;_tmp6C;});res.tvars_and_bounds_opt=_tmp43B;});
({struct Cyc_List_List*_tmp43D=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct _tuple16*_tmp43C=({struct _tuple16*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=_tmp108;_tmp6E->f2=0;_tmp6E;});_tmp6D->hd=_tmp43C;});_tmp6D->tl=(*res.tvars_and_bounds_opt).f1;_tmp6D;});(*res.tvars_and_bounds_opt).f1=_tmp43D;});
# 263
t=Cyc_Absyn_uint_typ;
goto _LL24;case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp47)->f1){case Cyc_Absyn_Unsigned: _LL2F: _LL30:
# 266
({void*_tmp43E=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);t=_tmp43E;});goto _LL24;case Cyc_Absyn_None: _LL31: _LL32:
 goto _LL34;default: _LL33: _LL34:
({void*_tmp43F=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);t=_tmp43F;});goto _LL24;}case 11: _LL35: _LL36:
({void*_tmp440=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);t=_tmp440;});goto _LL24;case 12: _LL37: _tmp109=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL38:
({void*_tmp442=({void**_tmp441=topt;Cyc_Tcpat_num_type(_tmp441,Cyc_Absyn_float_typ(_tmp109));});t=_tmp442;});goto _LL24;case 13: _LL39: _tmp10A=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL3A:
# 272
({void*_tmp445=({void**_tmp444=topt;Cyc_Tcpat_num_type(_tmp444,(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp443=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp70;_tmp70.tag=13;_tmp70.f1=_tmp10A->name;_tmp70.f2=_tmp10A;_tmp70;});_tmp6F[0]=_tmp443;});_tmp6F;}));});t=_tmp445;});
goto _LL24;case 14: _LL3B: _tmp10B=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL3C:
({void*_tmp446=Cyc_Tcpat_num_type(topt,_tmp10B);t=_tmp446;});goto _LL24;case 9: _LL3D: _LL3E:
# 276
 if(topt != 0){
void*_tmp71=Cyc_Tcutil_compress(*topt);void*_tmp72=_tmp71;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72)->tag == 5){_LL5F: _LL60:
 t=*topt;goto tcpat_end;}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}{
# 281
struct Cyc_Core_Opt*_tmp73=Cyc_Tcenv_lookup_opt_type_vars(te);
({void*_tmp44F=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp44E=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp75;_tmp75.tag=5;({struct Cyc_Absyn_PtrInfo _tmp44D=({struct Cyc_Absyn_PtrInfo _tmp76;({void*_tmp447=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp73);_tmp76.elt_typ=_tmp447;});({struct Cyc_Absyn_Tqual _tmp448=
Cyc_Absyn_empty_tqual(0);_tmp76.elt_tq=_tmp448;});({struct Cyc_Absyn_PtrAtts _tmp44C=({({void*_tmp449=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp73);(_tmp76.ptr_atts).rgn=_tmp449;});(_tmp76.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmp44A=
# 286
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp76.ptr_atts).bounds=_tmp44A;});({union Cyc_Absyn_Constraint*_tmp44B=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp76.ptr_atts).zero_term=_tmp44B;});(_tmp76.ptr_atts).ptrloc=0;_tmp76.ptr_atts;});_tmp76.ptr_atts=_tmp44C;});_tmp76;});_tmp75.f1=_tmp44D;});_tmp75;});_tmp74[0]=_tmp44E;});_tmp74;});
# 282
t=_tmp44F;});
# 287
goto _LL24;};case 6: _LL3F: _tmp10C=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL40: {
# 292
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmp77=0;
int elt_const=0;
if(topt != 0){
void*_tmp78=Cyc_Tcutil_compress(*topt);void*_tmp79=_tmp78;void*_tmp7B;struct Cyc_Absyn_Tqual _tmp7A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp79)->tag == 5){_LL64: _tmp7B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp79)->f1).elt_typ;_tmp7A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp79)->f1).elt_tq;_LL65:
# 298
 inner_typ=_tmp7B;
_tmp77=& inner_typ;
elt_const=_tmp7A.real_const;
goto _LL63;}else{_LL66: _LL67:
 goto _LL63;}_LL63:;}{
# 307
void*ptr_rgn=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)({struct Cyc_Absyn_Exp*_tmp450=Cyc_Absyn_deref_exp(access_exp,0);new_access_exp=_tmp450;});
({struct Cyc_Tcpat_TcPatResult _tmp452=({struct Cyc_Tcpat_TcPatResult _tmp451=res;Cyc_Tcpat_combine_results(_tmp451,Cyc_Tcpat_tcPatRec(te,_tmp10C,_tmp77,ptr_rgn,1,new_access_exp));});res=_tmp452;});
# 316
{void*_tmp7C=Cyc_Tcutil_compress((void*)_check_null(_tmp10C->topt));void*_tmp7D=_tmp7C;struct Cyc_Absyn_Datatypedecl*_tmp8C;struct Cyc_Absyn_Datatypefield*_tmp8B;struct Cyc_List_List*_tmp8A;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->tag == 4){if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->f1).field_info).KnownDatatypefield).tag == 2){_LL69: _tmp8C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->f1).field_info).KnownDatatypefield).val).f1;_tmp8B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->f1).field_info).KnownDatatypefield).val).f2;_tmp8A=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->f1).targs;_LL6A:
# 320
{void*_tmp7E=Cyc_Tcutil_compress(inner_typ);void*_tmp7F=_tmp7E;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7F)->tag == 4){_LL6E: _LL6F:
# 322
 goto DONT_PROMOTE;}else{_LL70: _LL71:
 goto _LL6D;}_LL6D:;}{
# 326
void*new_type=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp455=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp84;_tmp84.tag=3;({struct Cyc_Absyn_DatatypeInfo _tmp454=({struct Cyc_Absyn_DatatypeInfo _tmp85;({union Cyc_Absyn_DatatypeInfoU _tmp453=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=_tmp8C;_tmp86;}));_tmp85.datatype_info=_tmp453;});_tmp85.targs=_tmp8A;_tmp85;});_tmp84.f1=_tmp454;});_tmp84;});_tmp83[0]=_tmp455;});_tmp83;});
# 329
_tmp10C->topt=new_type;
({void*_tmp45B=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp80=_cycalloc(sizeof(*_tmp80));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp45A=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp81;_tmp81.tag=5;({struct Cyc_Absyn_PtrInfo _tmp459=({struct Cyc_Absyn_PtrInfo _tmp82;_tmp82.elt_typ=new_type;({struct Cyc_Absyn_Tqual _tmp456=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmp82.elt_tq=_tmp456;});({struct Cyc_Absyn_PtrAtts _tmp458=({(_tmp82.ptr_atts).rgn=ptr_rgn;({union Cyc_Absyn_Constraint*_tmp457=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp82.ptr_atts).nullable=_tmp457;});(_tmp82.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp82.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp82.ptr_atts).ptrloc=0;_tmp82.ptr_atts;});_tmp82.ptr_atts=_tmp458;});_tmp82;});_tmp81.f1=_tmp459;});_tmp81;});_tmp80[0]=_tmp45A;});_tmp80;});
# 330
t=_tmp45B;});
# 336
goto _LL68;};}else{goto _LL6B;}}else{_LL6B: _LL6C:
# 338
 DONT_PROMOTE:
({void*_tmp463=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp462=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp88;_tmp88.tag=5;({struct Cyc_Absyn_PtrInfo _tmp461=({struct Cyc_Absyn_PtrInfo _tmp89;_tmp89.elt_typ=(void*)_check_null(_tmp10C->topt);({struct Cyc_Absyn_Tqual _tmp45C=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmp89.elt_tq=_tmp45C;});({struct Cyc_Absyn_PtrAtts _tmp460=({(_tmp89.ptr_atts).rgn=ptr_rgn;({union Cyc_Absyn_Constraint*_tmp45D=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp89.ptr_atts).nullable=_tmp45D;});({union Cyc_Absyn_Constraint*_tmp45E=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp89.ptr_atts).bounds=_tmp45E;});({union Cyc_Absyn_Constraint*_tmp45F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp89.ptr_atts).zero_term=_tmp45F;});(_tmp89.ptr_atts).ptrloc=0;_tmp89.ptr_atts;});_tmp89.ptr_atts=_tmp460;});_tmp89;});_tmp88.f1=_tmp461;});_tmp88;});_tmp87[0]=_tmp462;});_tmp87;});
# 339
t=_tmp463;});}_LL68:;}
# 346
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp10C->topt;
goto _LL24;};}case 5: _LL41: _tmp10E=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp10D=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL42: {
# 350
struct Cyc_List_List*_tmp8D=*_tmp10E;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp8E=Cyc_Tcutil_compress(*topt);void*_tmp8F=_tmp8E;struct Cyc_List_List*_tmp95;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8F)->tag == 10){_LL73: _tmp95=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8F)->f1;_LL74:
# 356
 topt_ts=_tmp95;
if(_tmp10D){
# 359
int _tmp90=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);
int _tmp91=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp95);
if(_tmp90 < _tmp91){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp91 - _tmp90;++ i){
({struct Cyc_List_List*_tmp465=({struct Cyc_List_List*_tmp92=_cycalloc(sizeof(*_tmp92));({struct Cyc_Absyn_Pat*_tmp464=Cyc_Tcpat_wild_pat(p->loc);_tmp92->hd=_tmp464;});_tmp92->tl=wild_ps;_tmp92;});wild_ps=_tmp465;});}}
({struct Cyc_List_List*_tmp466=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8D,wild_ps);*_tmp10E=_tmp466;});
_tmp8D=*_tmp10E;}else{
if(({int _tmp467=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);_tmp467 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp95);}))
({void*_tmp93=0;({unsigned int _tmp469=p->loc;struct _dyneither_ptr _tmp468=({const char*_tmp94="unnecessary ... in tuple pattern";_tag_dyneither(_tmp94,sizeof(char),33);});Cyc_Tcutil_warn(_tmp469,_tmp468,_tag_dyneither(_tmp93,sizeof(void*),0));});});}}
# 371
goto _LL72;}else{_LL75: _LL76:
# 373
 goto _LL72;}_LL72:;}else{
# 375
if(_tmp10D)
({void*_tmp96=0;({unsigned int _tmp46B=p->loc;struct _dyneither_ptr _tmp46A=({const char*_tmp97="cannot determine missing fields for ... in tuple pattern";_tag_dyneither(_tmp97,sizeof(char),57);});Cyc_Tcutil_terr(_tmp46B,_tmp46A,_tag_dyneither(_tmp96,sizeof(void*),0));});});}
{int i=0;for(0;_tmp8D != 0;(_tmp8D=_tmp8D->tl,i ++)){
void**_tmp98=0;
if(topt_ts != 0){
_tmp98=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 383
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
({struct Cyc_Absyn_Exp*_tmp46E=({struct Cyc_Absyn_Exp*_tmp46D=access_exp;Cyc_Absyn_subscript_exp(_tmp46D,
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp99;({struct _tuple7 _tmp46C=({struct _tuple7 _tmp9A;_tmp9A.f1=Cyc_Absyn_Unsigned;_tmp9A.f2=i;_tmp9A;});(_tmp99.Int_c).val=_tmp46C;});(_tmp99.Int_c).tag=5;_tmp99;}),0),0);});
# 385
new_access_exp=_tmp46E;});
# 388
({struct Cyc_Tcpat_TcPatResult _tmp470=({struct Cyc_Tcpat_TcPatResult _tmp46F=res;Cyc_Tcpat_combine_results(_tmp46F,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp8D->hd,_tmp98,rgn_pat,allow_ref_pat,new_access_exp));});res=_tmp470;});
# 391
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp8D->hd)->topt;
({struct Cyc_List_List*_tmp473=({struct Cyc_List_List*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct _tuple17*_tmp472=({struct _tuple17*_tmp9C=_cycalloc(sizeof(*_tmp9C));({struct Cyc_Absyn_Tqual _tmp471=Cyc_Absyn_empty_tqual(0);_tmp9C->f1=_tmp471;});_tmp9C->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp8D->hd)->topt);_tmp9C;});_tmp9B->hd=_tmp472;});_tmp9B->tl=pat_ts;_tmp9B;});pat_ts=_tmp473;});};}}
# 394
({void*_tmp476=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp475=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp9E;_tmp9E.tag=10;({struct Cyc_List_List*_tmp474=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp9E.f1=_tmp474;});_tmp9E;});_tmp9D[0]=_tmp475;});_tmp9D;});t=_tmp476;});
goto _LL24;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->aggr_info).KnownAggr).tag == 2){_LL43: _tmp113=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->aggr_info).KnownAggr).val;_tmp112=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->targs;_tmp111=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_tmp110=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f3;_tmp10F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f4;_LL44: {
# 398
struct Cyc_List_List*_tmp9F=*_tmp110;
struct _dyneither_ptr aggr_str=_tmp113->kind == Cyc_Absyn_StructA?({const char*_tmpE3="struct";_tag_dyneither(_tmpE3,sizeof(char),7);}):({const char*_tmpE4="union";_tag_dyneither(_tmpE4,sizeof(char),6);});
if(_tmp113->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmpA0[1]={& _tmpA2};({unsigned int _tmp478=p->loc;struct _dyneither_ptr _tmp477=({const char*_tmpA1="can't destructure an abstract %s";_tag_dyneither(_tmpA1,sizeof(char),33);});Cyc_Tcutil_terr(_tmp478,_tmp477,_tag_dyneither(_tmpA0,sizeof(void*),1));});});});
({void*_tmp479=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp479;});
goto _LL24;}
# 407
if(_tmp113->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp113->impl))->tagged)
allow_ref_pat=0;
if(_tmp111 != 0){
# 413
if(topt == 0  || Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak)
allow_ref_pat=0;}
# 416
{struct _RegionHandle _tmpA3=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA3;_push_region(rgn);
# 418
{struct Cyc_List_List*_tmpA4=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpA5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp113->impl))->exist_vars;
{struct Cyc_List_List*t=_tmp111;for(0;t != 0;t=t->tl){
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)t->hd;
struct Cyc_Absyn_Tvar*uv=(struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmpA5))->hd;
_tmpA5=_tmpA5->tl;{
void*_tmpA6=Cyc_Absyn_compress_kb(tv->kind);
void*_tmpA7=Cyc_Absyn_compress_kb(uv->kind);
int error=0;
struct Cyc_Absyn_Kind*k2;
{void*_tmpA8=_tmpA7;struct Cyc_Absyn_Kind*_tmpAC;struct Cyc_Absyn_Kind*_tmpAB;switch(*((int*)_tmpA8)){case 2: _LL78: _tmpAB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA8)->f2;_LL79:
 _tmpAC=_tmpAB;goto _LL7B;case 0: _LL7A: _tmpAC=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpA8)->f1;_LL7B:
 k2=_tmpAC;goto _LL77;default: _LL7C: _LL7D:
({void*_tmpA9=0;({struct _dyneither_ptr _tmp47A=({const char*_tmpAA="unconstrained existential type variable in aggregate";_tag_dyneither(_tmpAA,sizeof(char),53);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp47A,_tag_dyneither(_tmpA9,sizeof(void*),0));});});}_LL77:;}
# 434
{void*_tmpAD=_tmpA6;struct Cyc_Core_Opt**_tmpB2;struct Cyc_Core_Opt**_tmpB1;struct Cyc_Absyn_Kind*_tmpB0;struct Cyc_Absyn_Kind*_tmpAF;switch(*((int*)_tmpAD)){case 0: _LL7F: _tmpAF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAD)->f1;_LL80:
# 437
 if(!Cyc_Tcutil_kind_leq(k2,_tmpAF))
error=1;
goto _LL7E;case 2: _LL81: _tmpB1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAD)->f1;_tmpB0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAD)->f2;_LL82:
 _tmpB2=_tmpB1;goto _LL84;default: _LL83: _tmpB2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAD)->f1;_LL84:
({struct Cyc_Core_Opt*_tmp47B=({struct Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=_tmpA7;_tmpAE;});*_tmpB2=_tmp47B;});goto _LL7E;}_LL7E:;}
# 443
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmpB7;_tmpB7.tag=0;({struct _dyneither_ptr _tmp47C=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 447
Cyc_Absynpp_kind2string(k2));_tmpB7.f1=_tmp47C;});({struct Cyc_String_pa_PrintArg_struct _tmpB6;_tmpB6.tag=0;({struct _dyneither_ptr _tmp47D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpA6));_tmpB6.f1=_tmp47D;});({struct Cyc_String_pa_PrintArg_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmpB3[3]={& _tmpB5,& _tmpB6,& _tmpB7};({unsigned int _tmp47F=p->loc;struct _dyneither_ptr _tmp47E=({const char*_tmpB4="type variable %s has kind %s but must have at least kind %s";_tag_dyneither(_tmpB4,sizeof(char),60);});Cyc_Tcutil_terr(_tmp47F,_tmp47E,_tag_dyneither(_tmpB3,sizeof(void*),3));});});});});});{
# 449
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp480=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpBE;_tmpBE.tag=2;_tmpBE.f1=tv;_tmpBE;});_tmpBD[0]=_tmp480;});_tmpBD;});
({struct Cyc_List_List*_tmp481=({struct Cyc_List_List*_tmpB8=_region_malloc(rgn,sizeof(*_tmpB8));_tmpB8->hd=vartype;_tmpB8->tl=_tmpA4;_tmpB8;});_tmpA4=_tmp481;});
# 453
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable)
({struct Cyc_List_List*_tmp483=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct _tuple0*_tmp482=({struct _tuple0*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=Cyc_Absyn_empty_effect;_tmpBA->f2=vartype;_tmpBA;});_tmpB9->hd=_tmp482;});_tmpB9->tl=outlives_constraints;_tmpB9;});outlives_constraints=_tmp483;});else{
# 458
({void*_tmpBB=0;({struct _dyneither_ptr _tmp484=({const char*_tmpBC="opened existential had unique or top region kind";_tag_dyneither(_tmpBC,sizeof(char),49);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp484,_tag_dyneither(_tmpBB,sizeof(void*),0));});});}}};};}}
# 462
({struct Cyc_List_List*_tmp485=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA4);_tmpA4=_tmp485;});{
# 464
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmp111);
# 466
struct Cyc_List_List*_tmpBF=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmpC0=({struct _tuple12 _tmpE2;_tmpE2.f1=_tmpBF;_tmpE2.f2=rgn;_tmpE2;});
struct Cyc_List_List*_tmpC1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpC0,_tmp113->tvs);
struct Cyc_List_List*_tmpC2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp113->impl))->exist_vars,_tmpA4);
struct Cyc_List_List*_tmpC3=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpC1);
struct Cyc_List_List*_tmpC4=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpC2);
struct Cyc_List_List*_tmpC5=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpC1,_tmpC2);
# 474
if(_tmp111 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp113->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp486=({struct _tuple1*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->f1=0;_tmpC6->f2=0;_tmpC6;});res.tvars_and_bounds_opt=_tmp486;});
({struct Cyc_List_List*_tmp488=({
struct Cyc_List_List*_tmp487=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp487,((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmp111));});
# 477
(*res.tvars_and_bounds_opt).f1=_tmp488;});
# 479
({struct Cyc_List_List*_tmp489=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 479
(*res.tvars_and_bounds_opt).f2=_tmp489;});{
# 481
struct Cyc_List_List*_tmpC7=0;
{struct Cyc_List_List*_tmpC8=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp113->impl))->rgn_po;for(0;_tmpC8 != 0;_tmpC8=_tmpC8->tl){
({struct Cyc_List_List*_tmp48D=({struct Cyc_List_List*_tmpC9=_cycalloc(sizeof(*_tmpC9));({struct _tuple0*_tmp48C=({struct _tuple0*_tmpCA=_cycalloc(sizeof(*_tmpCA));({void*_tmp48A=Cyc_Tcutil_rsubstitute(rgn,_tmpC5,(*((struct _tuple0*)_tmpC8->hd)).f1);_tmpCA->f1=_tmp48A;});({void*_tmp48B=
Cyc_Tcutil_rsubstitute(rgn,_tmpC5,(*((struct _tuple0*)_tmpC8->hd)).f2);_tmpCA->f2=_tmp48B;});_tmpCA;});_tmpC9->hd=_tmp48C;});_tmpC9->tl=_tmpC7;_tmpC9;});
# 483
_tmpC7=_tmp48D;});}}
# 486
({struct Cyc_List_List*_tmp48E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC7);_tmpC7=_tmp48E;});
({struct Cyc_List_List*_tmp48F=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmpC7);
# 487
(*res.tvars_and_bounds_opt).f2=_tmp48F;});};}
# 491
*_tmp112=_tmpC3;
({void*_tmp493=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp492=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpCC;_tmpCC.tag=11;({struct Cyc_Absyn_AggrInfo _tmp491=({struct Cyc_Absyn_AggrInfo _tmpCD;({union Cyc_Absyn_AggrInfoU _tmp490=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE[0]=_tmp113;_tmpCE;}));_tmpCD.aggr_info=_tmp490;});_tmpCD.targs=*_tmp112;_tmpCD;});_tmpCC.f1=_tmp491;});_tmpCC;});_tmpCB[0]=_tmp492;});_tmpCB;});t=_tmp493;});
if(_tmp10F  && _tmp113->kind == Cyc_Absyn_UnionA)
({void*_tmpCF=0;({unsigned int _tmp495=p->loc;struct _dyneither_ptr _tmp494=({const char*_tmpD0="`...' pattern not allowed in union pattern";_tag_dyneither(_tmpD0,sizeof(char),43);});Cyc_Tcutil_warn(_tmp495,_tmp494,_tag_dyneither(_tmpCF,sizeof(void*),0));});});else{
if(_tmp10F){
# 497
int _tmpD1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F);
int _tmpD2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp113->impl))->fields);
if(_tmpD1 < _tmpD2){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmpD2 - _tmpD1;++ i){
({struct Cyc_List_List*_tmp498=({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));({struct _tuple18*_tmp497=({struct _tuple18*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->f1=0;({struct Cyc_Absyn_Pat*_tmp496=Cyc_Tcpat_wild_pat(p->loc);_tmpD4->f2=_tmp496;});_tmpD4;});_tmpD3->hd=_tmp497;});_tmpD3->tl=wild_dps;_tmpD3;});wild_dps=_tmp498;});}}
({struct Cyc_List_List*_tmp499=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F,wild_dps);*_tmp110=_tmp499;});
_tmp9F=*_tmp110;}else{
if(_tmpD1 == _tmpD2)
({void*_tmpD5=0;({unsigned int _tmp49B=p->loc;struct _dyneither_ptr _tmp49A=({const char*_tmpD6="unnecessary ... in struct pattern";_tag_dyneither(_tmpD6,sizeof(char),34);});Cyc_Tcutil_warn(_tmp49B,_tmp49A,_tag_dyneither(_tmpD5,sizeof(void*),0));});});}}}{
# 508
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp9F,_tmp113->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp113->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmpD7=(struct _tuple19*)fields->hd;struct _tuple19*_tmpD8=_tmpD7;struct Cyc_Absyn_Aggrfield*_tmpE1;struct Cyc_Absyn_Pat*_tmpE0;_LL86: _tmpE1=_tmpD8->f1;_tmpE0=_tmpD8->f2;_LL87:;{
void*_tmpD9=Cyc_Tcutil_rsubstitute(rgn,_tmpC5,_tmpE1->type);
# 514
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
({struct Cyc_Absyn_Exp*_tmp49C=Cyc_Absyn_aggrmember_exp(access_exp,_tmpE1->name,0);new_access_exp=_tmp49C;});
({struct Cyc_Tcpat_TcPatResult _tmp49E=({struct Cyc_Tcpat_TcPatResult _tmp49D=res;Cyc_Tcpat_combine_results(_tmp49D,Cyc_Tcpat_tcPatRec(te2,_tmpE0,& _tmpD9,rgn_pat,allow_ref_pat,new_access_exp));});res=_tmp49E;});
# 522
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpE0->topt),_tmpD9))
({struct Cyc_String_pa_PrintArg_struct _tmpDF;_tmpDF.tag=0;({struct _dyneither_ptr _tmp49F=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 525
Cyc_Absynpp_typ2string((void*)_check_null(_tmpE0->topt)));_tmpDF.f1=_tmp49F;});({struct Cyc_String_pa_PrintArg_struct _tmpDE;_tmpDE.tag=0;({struct _dyneither_ptr _tmp4A0=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 524
Cyc_Absynpp_typ2string(_tmpD9));_tmpDE.f1=_tmp4A0;});({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({struct Cyc_String_pa_PrintArg_struct _tmpDC;_tmpDC.tag=0;_tmpDC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE1->name);({void*_tmpDA[4]={& _tmpDC,& _tmpDD,& _tmpDE,& _tmpDF};({unsigned int _tmp4A2=p->loc;struct _dyneither_ptr _tmp4A1=({const char*_tmpDB="field %s of %s pattern expects type %s != %s";_tag_dyneither(_tmpDB,sizeof(char),45);});Cyc_Tcutil_terr(_tmp4A2,_tmp4A1,_tag_dyneither(_tmpDA,sizeof(void*),4));});});});});});});
# 526
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpE0->topt;};}};};}
# 418
;_pop_region(rgn);}
# 529
goto _LL24;}}else{_LL49: _LL4A:
# 583
 goto _LL4C;}}else{_LL47: _LL48:
# 582
 goto _LL4A;}case 8: _LL45: _tmp117=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp116=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_tmp115=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f3;_tmp114=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f4;_LL46: {
# 532
struct Cyc_List_List*_tmpE5=*_tmp115;
struct Cyc_List_List*tqts=_tmp116->typs;
# 535
struct Cyc_List_List*_tmpE6=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmpE7=({struct _tuple12 _tmp100;_tmp100.f1=_tmpE6;_tmp100.f2=Cyc_Core_heap_region;_tmp100;});
struct Cyc_List_List*_tmpE8=((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_r_make_inst_var,& _tmpE7,_tmp117->tvs);
struct Cyc_List_List*_tmpE9=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpE8);
({void*_tmp4A6=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp4A5=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpEB;_tmpEB.tag=4;({struct Cyc_Absyn_DatatypeFieldInfo _tmp4A4=({struct Cyc_Absyn_DatatypeFieldInfo _tmpEC;({union Cyc_Absyn_DatatypeFieldInfoU _tmp4A3=Cyc_Absyn_KnownDatatypefield(_tmp117,_tmp116);_tmpEC.field_info=_tmp4A3;});_tmpEC.targs=_tmpE9;_tmpEC;});_tmpEB.f1=_tmp4A4;});_tmpEB;});_tmpEA[0]=_tmp4A5;});_tmpEA;});t=_tmp4A6;});
# 541
if(_tmp114){
# 543
int _tmpED=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE5);
int _tmpEE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmpED < _tmpEE){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpEE - _tmpED;++ i){
({struct Cyc_List_List*_tmp4A8=({struct Cyc_List_List*_tmpEF=_cycalloc(sizeof(*_tmpEF));({struct Cyc_Absyn_Pat*_tmp4A7=Cyc_Tcpat_wild_pat(p->loc);_tmpEF->hd=_tmp4A7;});_tmpEF->tl=wild_ps;_tmpEF;});wild_ps=_tmp4A8;});}}
({struct Cyc_List_List*_tmp4A9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE5,wild_ps);*_tmp115=_tmp4A9;});
_tmpE5=*_tmp115;}else{
if(_tmpED == _tmpEE)
({struct Cyc_String_pa_PrintArg_struct _tmpF2;_tmpF2.tag=0;({struct _dyneither_ptr _tmp4AA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp117->name));_tmpF2.f1=_tmp4AA;});({void*_tmpF0[1]={& _tmpF2};({unsigned int _tmp4AC=p->loc;struct _dyneither_ptr _tmp4AB=({const char*_tmpF1="unnecessary ... in datatype field %s";_tag_dyneither(_tmpF1,sizeof(char),37);});Cyc_Tcutil_warn(_tmp4AC,_tmp4AB,_tag_dyneither(_tmpF0,sizeof(void*),1));});});});}}
# 555
for(0;_tmpE5 != 0  && tqts != 0;(_tmpE5=_tmpE5->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpF3=(struct Cyc_Absyn_Pat*)_tmpE5->hd;
# 559
void*_tmpF4=Cyc_Tcutil_substitute(_tmpE8,(*((struct _tuple17*)tqts->hd)).f2);
# 562
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
({struct Cyc_Tcpat_TcPatResult _tmp4AE=({struct Cyc_Tcpat_TcPatResult _tmp4AD=res;Cyc_Tcpat_combine_results(_tmp4AD,Cyc_Tcpat_tcPatRec(te,_tmpF3,& _tmpF4,rgn_pat,allow_ref_pat,0));});res=_tmp4AE;});
# 569
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpF3->topt),_tmpF4))
({struct Cyc_String_pa_PrintArg_struct _tmpF9;_tmpF9.tag=0;({struct _dyneither_ptr _tmp4AF=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 572
Cyc_Absynpp_typ2string((void*)_check_null(_tmpF3->topt)));_tmpF9.f1=_tmp4AF;});({struct Cyc_String_pa_PrintArg_struct _tmpF8;_tmpF8.tag=0;({struct _dyneither_ptr _tmp4B0=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 571
Cyc_Absynpp_typ2string(_tmpF4));_tmpF8.f1=_tmp4B0;});({struct Cyc_String_pa_PrintArg_struct _tmpF7;_tmpF7.tag=0;({struct _dyneither_ptr _tmp4B1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp116->name));_tmpF7.f1=_tmp4B1;});({void*_tmpF5[3]={& _tmpF7,& _tmpF8,& _tmpF9};({unsigned int _tmp4B3=_tmpF3->loc;struct _dyneither_ptr _tmp4B2=({const char*_tmpF6="%s expects argument type %s, not %s";_tag_dyneither(_tmpF6,sizeof(char),36);});Cyc_Tcutil_terr(_tmp4B3,_tmp4B2,_tag_dyneither(_tmpF5,sizeof(void*),3));});});});});});}
# 574
if(_tmpE5 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpFC;_tmpFC.tag=0;({struct _dyneither_ptr _tmp4B4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp116->name));_tmpFC.f1=_tmp4B4;});({void*_tmpFA[1]={& _tmpFC};({unsigned int _tmp4B6=p->loc;struct _dyneither_ptr _tmp4B5=({const char*_tmpFB="too many arguments for datatype constructor %s";_tag_dyneither(_tmpFB,sizeof(char),47);});Cyc_Tcutil_terr(_tmp4B6,_tmp4B5,_tag_dyneither(_tmpFA,sizeof(void*),1));});});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpFF;_tmpFF.tag=0;({struct _dyneither_ptr _tmp4B7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp116->name));_tmpFF.f1=_tmp4B7;});({void*_tmpFD[1]={& _tmpFF};({unsigned int _tmp4B9=p->loc;struct _dyneither_ptr _tmp4B8=({const char*_tmpFE="too few arguments for datatype constructor %s";_tag_dyneither(_tmpFE,sizeof(char),46);});Cyc_Tcutil_terr(_tmp4B9,_tmp4B8,_tag_dyneither(_tmpFD,sizeof(void*),1));});});});
goto _LL24;}case 15: _LL4B: _LL4C:
# 584
 goto _LL4E;case 17: _LL4D: _LL4E:
 goto _LL50;default: _LL4F: _LL50:
# 587
({void*_tmp4BA=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp4BA;});goto _LL24;}_LL24:;}
# 589
tcpat_end:
 p->topt=t;
return res;};}
# 594
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 596
struct Cyc_Tcpat_TcPatResult _tmp119=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 598
struct _tuple1 _tmp11A=((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp119.patvars);struct _tuple1 _tmp11B=_tmp11A;struct Cyc_List_List*_tmp123;_LL89: _tmp123=_tmp11B.f1;_LL8A:;{
struct Cyc_List_List*_tmp11C=0;
{struct Cyc_List_List*x=_tmp123;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)({struct Cyc_List_List*_tmp4BB=({struct Cyc_List_List*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd));_tmp11D->tl=_tmp11C;_tmp11D;});_tmp11C=_tmp4BB;});}}
({struct Cyc_List_List*_tmp4BD=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_name,_tmp11C);unsigned int _tmp4BC=p->loc;Cyc_Tcutil_check_unique_vars(_tmp4BD,_tmp4BC,({const char*_tmp11E="pattern contains a repeated variable";_tag_dyneither(_tmp11E,sizeof(char),37);}));});
# 607
{struct Cyc_List_List*x=_tmp119.patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp11F=(struct _tuple15*)x->hd;struct _tuple15*_tmp120=_tmp11F;struct Cyc_Absyn_Vardecl**_tmp122;struct Cyc_Absyn_Exp**_tmp121;_LL8C: _tmp122=_tmp120->f1;_tmp121=(struct Cyc_Absyn_Exp**)& _tmp120->f2;_LL8D:;
if(*_tmp121 != 0  && *_tmp121 != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp4BE=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp121));*_tmp121=_tmp4BE;});}}
# 612
return _tmp119;};}
# 618
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 621
struct _tuple0 _tmp124=({struct _tuple0 _tmp12C;({void*_tmp4BF=Cyc_Tcutil_compress(old_type);_tmp12C.f1=_tmp4BF;});({void*_tmp4C0=Cyc_Tcutil_compress(new_type);_tmp12C.f2=_tmp4C0;});_tmp12C;});struct _tuple0 _tmp125=_tmp124;struct Cyc_Absyn_PtrInfo _tmp12B;struct Cyc_Absyn_PtrInfo _tmp12A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp125.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp125.f2)->tag == 5){_LL8F: _tmp12B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp125.f1)->f1;_tmp12A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp125.f2)->f1;_LL90: {
# 623
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp126=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp127=_cycalloc(sizeof(*_tmp127));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4C3=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp128;_tmp128.tag=5;({struct Cyc_Absyn_PtrInfo _tmp4C2=({struct Cyc_Absyn_PtrInfo _tmp129;_tmp129.elt_typ=_tmp12B.elt_typ;_tmp129.elt_tq=_tmp12A.elt_tq;({struct Cyc_Absyn_PtrAtts _tmp4C1=({(_tmp129.ptr_atts).rgn=(_tmp12B.ptr_atts).rgn;(_tmp129.ptr_atts).nullable=(_tmp12A.ptr_atts).nullable;(_tmp129.ptr_atts).bounds=(_tmp12A.ptr_atts).bounds;(_tmp129.ptr_atts).zero_term=(_tmp12A.ptr_atts).zero_term;(_tmp129.ptr_atts).ptrloc=(_tmp12B.ptr_atts).ptrloc;_tmp129.ptr_atts;});_tmp129.ptr_atts=_tmp4C1;});_tmp129;});_tmp128.f1=_tmp4C2;});_tmp128;});_tmp127[0]=_tmp4C3;});_tmp127;});
# 629
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp126,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp126);}}else{goto _LL91;}}else{_LL91: _LL92:
 return 0;}_LL8E:;}
# 638
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 641
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp138=_cycalloc(sizeof(*_tmp138));({struct _tuple0*_tmp4C6=({struct _tuple0*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->f1=(void*)& Cyc_Absyn_UniqueRgn_val;({void*_tmp4C5=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4C4=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp13B;_tmp13B.tag=2;_tmp13B.f1=tv;_tmp13B;});_tmp13A[0]=_tmp4C4;});_tmp13A;});_tmp139->f2=_tmp4C5;});_tmp139;});_tmp138->hd=_tmp4C6;});_tmp138->tl=0;_tmp138;});
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 659 "tcpat.cyc"
struct _tuple0 _tmp12D=({struct _tuple0 _tmp133;({void*_tmp4C7=Cyc_Tcutil_compress(old_type);_tmp133.f1=_tmp4C7;});({void*_tmp4C8=Cyc_Tcutil_compress(new_type);_tmp133.f2=_tmp4C8;});_tmp133;});struct _tuple0 _tmp12E=_tmp12D;struct Cyc_Absyn_PtrInfo _tmp132;struct Cyc_Absyn_PtrInfo _tmp131;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E.f2)->tag == 5){_LL94: _tmp132=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E.f1)->f1;_tmp131=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp12E.f2)->f1;_LL95:
 goto _LL93;}else{goto _LL96;}}else{_LL96: _LL97:
({void*_tmp12F=0;({unsigned int _tmp4CA=loc;struct _dyneither_ptr _tmp4C9=({const char*_tmp130="alias requires pointer type";_tag_dyneither(_tmp130,sizeof(char),28);});Cyc_Tcutil_terr(_tmp4CA,_tmp4C9,_tag_dyneither(_tmp12F,sizeof(void*),0));});});goto _LL93;}_LL93:;}else{
# 664
({struct Cyc_String_pa_PrintArg_struct _tmp137;_tmp137.tag=0;({struct _dyneither_ptr _tmp4CB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type));_tmp137.f1=_tmp4CB;});({struct Cyc_String_pa_PrintArg_struct _tmp136;_tmp136.tag=0;({struct _dyneither_ptr _tmp4CC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type));_tmp136.f1=_tmp4CC;});({void*_tmp134[2]={& _tmp136,& _tmp137};({unsigned int _tmp4CE=loc;struct _dyneither_ptr _tmp4CD=({const char*_tmp135="cannot alias value of type %s to type %s";_tag_dyneither(_tmp135,sizeof(char),41);});Cyc_Tcutil_terr(_tmp4CE,_tmp4CD,_tag_dyneither(_tmp134,sizeof(void*),2));});});});});}}
# 671
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 675
void*_tmp13C=p->r;void*_tmp13D=_tmp13C;struct Cyc_Absyn_Tvar*_tmp166;struct Cyc_Absyn_Vardecl*_tmp165;struct Cyc_Absyn_Vardecl*_tmp164;struct Cyc_Absyn_Pat*_tmp163;struct Cyc_Absyn_Vardecl*_tmp162;struct Cyc_Absyn_Pat*_tmp161;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_AggrInfo*_tmp15E;struct Cyc_List_List*_tmp15D;struct Cyc_List_List*_tmp15C;struct Cyc_Absyn_Pat*_tmp15B;switch(*((int*)_tmp13D)){case 6: _LL99: _tmp15B=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp13D)->f1;_LL9A: {
# 677
void*_tmp13E=(void*)_check_null(p->topt);void*_tmp13F=_tmp13E;void*_tmp142;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13F)->tag == 5){_LLAA: _tmp142=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp13F)->f1).ptr_atts).rgn;_LLAB:
# 679
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp142);
({struct Cyc_Tcenv_Tenv*_tmp4D1=te;struct Cyc_Absyn_Pat*_tmp4D0=_tmp15B;int _tmp4CF=Cyc_Tcutil_is_noalias_region(_tmp142,0);Cyc_Tcpat_check_pat_regions_rec(_tmp4D1,_tmp4D0,_tmp4CF,patvars);});
return;}else{_LLAC: _LLAD:
({void*_tmp140=0;({struct _dyneither_ptr _tmp4D2=({const char*_tmp141="check_pat_regions: bad pointer type";_tag_dyneither(_tmp141,sizeof(char),36);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4D2,_tag_dyneither(_tmp140,sizeof(void*),0));});});}_LLA9:;}case 7: _LL9B: _tmp15E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp13D)->f1;_tmp15D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp13D)->f2;_tmp15C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp13D)->f3;_LL9C:
# 685
 for(0;_tmp15C != 0;_tmp15C=_tmp15C->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple18*)_tmp15C->hd)).f2,did_noalias_deref,patvars);}
return;case 8: _LL9D: _tmp15F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp13D)->f3;_LL9E:
# 689
 did_noalias_deref=0;_tmp160=_tmp15F;goto _LLA0;case 5: _LL9F: _tmp160=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp13D)->f1;_LLA0:
# 691
 for(0;_tmp160 != 0;_tmp160=_tmp160->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp160->hd,did_noalias_deref,patvars);}
return;case 3: _LLA1: _tmp162=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp13D)->f1;_tmp161=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp13D)->f2;_LLA2:
# 695
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp143=(struct _tuple15*)x->hd;struct _tuple15*_tmp144=_tmp143;struct Cyc_Absyn_Vardecl**_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;_LLAF: _tmp14D=_tmp144->f1;_tmp14C=_tmp144->f2;_LLB0:;
# 701
if((_tmp14D != 0  && *_tmp14D == _tmp162) && _tmp14C != 0){
{void*_tmp145=_tmp14C->r;void*_tmp146=_tmp145;struct Cyc_Absyn_Exp*_tmp14B;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp146)->tag == 15){_LLB2: _tmp14B=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp146)->f1;_LLB3:
# 704
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp14B->topt)))
({void*_tmp147=0;({unsigned int _tmp4D4=p->loc;struct _dyneither_ptr _tmp4D3=({const char*_tmp148="reference pattern not allowed on alias-free pointers";_tag_dyneither(_tmp148,sizeof(char),53);});Cyc_Tcutil_terr(_tmp4D4,_tmp4D3,_tag_dyneither(_tmp147,sizeof(void*),0));});});
goto _LLB1;}else{_LLB4: _LLB5:
# 708
({void*_tmp149=0;({struct _dyneither_ptr _tmp4D5=({const char*_tmp14A="check_pat_regions: bad reference access expression";_tag_dyneither(_tmp14A,sizeof(char),51);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4D5,_tag_dyneither(_tmp149,sizeof(void*),0));});});}_LLB1:;}
# 710
break;}}}
# 713
Cyc_Tcpat_check_pat_regions_rec(te,_tmp161,did_noalias_deref,patvars);
return;case 1: _LLA3: _tmp164=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp13D)->f1;_tmp163=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp13D)->f2;_LLA4:
# 716
{void*_tmp14E=p->topt;void*_tmp14F=_tmp14E;if(_tmp14F != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14F)->tag == 8){_LLB7: _LLB8:
# 718
 if(did_noalias_deref){
({void*_tmp150=0;({unsigned int _tmp4D7=p->loc;struct _dyneither_ptr _tmp4D6=({const char*_tmp151="pattern to array would create alias of no-alias pointer";_tag_dyneither(_tmp151,sizeof(char),56);});Cyc_Tcutil_terr(_tmp4D7,_tmp4D6,_tag_dyneither(_tmp150,sizeof(void*),0));});});
return;}
# 722
goto _LLB6;}else{goto _LLB9;}}else{_LLB9: _LLBA:
 goto _LLB6;}_LLB6:;}
# 725
Cyc_Tcpat_check_pat_regions_rec(te,_tmp163,did_noalias_deref,patvars);
return;case 2: _LLA5: _tmp166=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp13D)->f1;_tmp165=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp13D)->f2;_LLA6:
# 728
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp152=(struct _tuple15*)x->hd;struct _tuple15*_tmp153=_tmp152;struct Cyc_Absyn_Vardecl**_tmp15A;struct Cyc_Absyn_Exp*_tmp159;_LLBC: _tmp15A=_tmp153->f1;_tmp159=_tmp153->f2;_LLBD:;
# 732
if(_tmp15A != 0  && *_tmp15A == _tmp165){
if(_tmp159 == 0)
({void*_tmp154=0;({unsigned int _tmp4D9=p->loc;struct _dyneither_ptr _tmp4D8=({const char*_tmp155="cannot alias pattern expression in datatype";_tag_dyneither(_tmp155,sizeof(char),44);});Cyc_Tcutil_terr(_tmp4D9,_tmp4D8,_tag_dyneither(_tmp154,sizeof(void*),0));});});else{
# 736
struct Cyc_Tcenv_Tenv*te2=({unsigned int _tmp4DB=p->loc;struct Cyc_Tcenv_Tenv*_tmp4DA=te;Cyc_Tcenv_add_type_vars(_tmp4DB,_tmp4DA,({struct Cyc_Absyn_Tvar*_tmp158[1];_tmp158[0]=_tmp166;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp158,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
({struct Cyc_Tcenv_Tenv*_tmp4DE=({struct Cyc_Tcenv_Tenv*_tmp4DD=te2;Cyc_Tcenv_add_region(_tmp4DD,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp156=_cycalloc(sizeof(*_tmp156));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4DC=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp157;_tmp157.tag=2;_tmp157.f1=_tmp166;_tmp157;});_tmp156[0]=_tmp4DC;});_tmp156;}),1);});te2=_tmp4DE;});
# 739
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp159->topt),_tmp166,_tmp165->type,_tmp159);}
# 742
break;}}}
# 745
goto _LL98;default: _LLA7: _LLA8:
 return;}_LL98:;}
# 761 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 763
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp167=(struct _tuple15*)x->hd;struct _tuple15*_tmp168=_tmp167;struct Cyc_Absyn_Vardecl**_tmp172;struct Cyc_Absyn_Exp*_tmp171;_LLBF: _tmp172=_tmp168->f1;_tmp171=_tmp168->f2;_LLC0:;
if(_tmp171 != 0){
struct Cyc_Absyn_Exp*_tmp169=_tmp171;
# 770
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_check_null(_tmp169->topt)) && !Cyc_Tcutil_is_noalias_path(_tmp169))
# 772
({struct Cyc_String_pa_PrintArg_struct _tmp16C;_tmp16C.tag=0;({struct _dyneither_ptr _tmp4E1=(struct _dyneither_ptr)(
# 774
_tmp172 != 0?(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp16F;_tmp16F.tag=0;({struct _dyneither_ptr _tmp4DF=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 776
Cyc_Absynpp_qvar2string((*_tmp172)->name));_tmp16F.f1=_tmp4DF;});({void*_tmp16D[1]={& _tmp16F};({struct _dyneither_ptr _tmp4E0=({const char*_tmp16E="for variable %s";_tag_dyneither(_tmp16E,sizeof(char),16);});Cyc_aprintf(_tmp4E0,_tag_dyneither(_tmp16D,sizeof(void*),1));});});}):({const char*_tmp170="";_tag_dyneither(_tmp170,sizeof(char),1);}));_tmp16C.f1=_tmp4E1;});({void*_tmp16A[1]={& _tmp16C};({unsigned int _tmp4E3=p->loc;struct _dyneither_ptr _tmp4E2=({const char*_tmp16B="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_dyneither(_tmp16B,sizeof(char),68);});Cyc_Tcutil_terr(_tmp4E3,_tmp4E2,_tag_dyneither(_tmp16A,sizeof(void*),1));});});});}}};}
# 830 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 840
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};union Cyc_Tcpat_PatOrWhere;struct Cyc_Tcpat_PathNode;struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 853
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp177;(_tmp177.Name_v).val=s;(_tmp177.Name_v).tag=1;_tmp177;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp178;(_tmp178.Int_v).val=i;(_tmp178.Int_v).tag=2;_tmp178;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};
# 863
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 868
typedef void*Cyc_Tcpat_simple_pat_t;
# 871
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp179=c1->name;union Cyc_Tcpat_Name_value _tmp17A=_tmp179;int _tmp182;struct _dyneither_ptr _tmp181;if((_tmp17A.Name_v).tag == 1){_LLC2: _tmp181=(_tmp17A.Name_v).val;_LLC3: {
# 874
union Cyc_Tcpat_Name_value _tmp17B=c2->name;union Cyc_Tcpat_Name_value _tmp17C=_tmp17B;struct _dyneither_ptr _tmp17D;if((_tmp17C.Name_v).tag == 1){_LLC7: _tmp17D=(_tmp17C.Name_v).val;_LLC8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp181,(struct _dyneither_ptr)_tmp17D);}else{_LLC9: _LLCA:
 return - 1;}_LLC6:;}}else{_LLC4: _tmp182=(_tmp17A.Int_v).val;_LLC5: {
# 879
union Cyc_Tcpat_Name_value _tmp17E=c2->name;union Cyc_Tcpat_Name_value _tmp17F=_tmp17E;int _tmp180;if((_tmp17F.Name_v).tag == 1){_LLCC: _LLCD:
 return 1;}else{_LLCE: _tmp180=(_tmp17F.Int_v).val;_LLCF:
 return _tmp182 - _tmp180;}_LLCB:;}}_LLC1:;}
# 887
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 891
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 895
static unsigned int Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned int ans=0;
struct Cyc_List_List*_tmp183=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp183))->hd)->name)!= 0){
++ ans;
_tmp183=_tmp183->tl;}
# 902
return ans;}
# 905
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp184=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp184 != 0;_tmp184=_tmp184->tl){
struct Cyc_Absyn_Aggrfield*_tmp185=(struct Cyc_Absyn_Aggrfield*)_tmp184->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp185->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 912
({void*_tmp186=0;({struct _dyneither_ptr _tmp4E5=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp189;_tmp189.tag=0;_tmp189.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp187[1]={& _tmp189};({struct _dyneither_ptr _tmp4E4=({const char*_tmp188="get_member_offset %s failed";_tag_dyneither(_tmp188,sizeof(char),28);});Cyc_aprintf(_tmp4E4,_tag_dyneither(_tmp187,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4E5,_tag_dyneither(_tmp186,sizeof(void*),0));});});}
# 915
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp18A=pw;struct Cyc_Absyn_Pat*_tmp1C3;struct Cyc_Absyn_Exp*_tmp1C2;if((_tmp18A.where_clause).tag == 2){_LLD1: _tmp1C2=(_tmp18A.where_clause).val;_LLD2:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp18B=_cycalloc(sizeof(*_tmp18B));({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct _tmp4E6=({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct _tmp18C;_tmp18C.tag=0;_tmp18C.f1=_tmp1C2;_tmp18C;});_tmp18B[0]=_tmp4E6;});_tmp18B;});}else{_LLD3: _tmp1C3=(_tmp18A.pattern).val;_LLD4: {
# 919
void*_tmp18D=_tmp1C3->r;void*_tmp18E=_tmp18D;union Cyc_Absyn_AggrInfoU _tmp1C1;struct Cyc_List_List*_tmp1C0;struct Cyc_Absyn_Datatypedecl*_tmp1BF;struct Cyc_Absyn_Datatypefield*_tmp1BE;void*_tmp1BD;struct Cyc_Absyn_Enumfield*_tmp1BC;struct Cyc_Absyn_Enumdecl*_tmp1BB;struct Cyc_Absyn_Enumfield*_tmp1BA;struct _dyneither_ptr _tmp1B9;int _tmp1B8;char _tmp1B7;enum Cyc_Absyn_Sign _tmp1B6;int _tmp1B5;struct Cyc_Absyn_Pat*_tmp1B4;struct Cyc_Absyn_Pat*_tmp1B3;switch(*((int*)_tmp18E)){case 1: _LLD6: _tmp1B3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp18E)->f2;_LLD7:
 _tmp1B4=_tmp1B3;goto _LLD9;case 3: _LLD8: _tmp1B4=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp18E)->f2;_LLD9:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp18F;(_tmp18F.pattern).val=_tmp1B4;(_tmp18F.pattern).tag=1;_tmp18F;}));case 9: _LLDA: _LLDB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _LLDC: _tmp1B6=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp18E)->f1;_tmp1B5=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp18E)->f2;_LLDD:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp190=_cycalloc_atomic(sizeof(*_tmp190));({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp4E7=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp191;_tmp191.tag=6;_tmp191.f1=(unsigned int)_tmp1B5;_tmp191;});_tmp190[0]=_tmp4E7;});_tmp190;});case 11: _LLDE: _tmp1B7=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp18E)->f1;_LLDF:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp192=_cycalloc_atomic(sizeof(*_tmp192));({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp4E8=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp193;_tmp193.tag=6;_tmp193.f1=(unsigned int)_tmp1B7;_tmp193;});_tmp192[0]=_tmp4E8;});_tmp192;});case 12: _LLE0: _tmp1B9=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp18E)->f1;_tmp1B8=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp18E)->f2;_LLE1:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp194=_cycalloc(sizeof(*_tmp194));({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct _tmp4E9=({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct _tmp195;_tmp195.tag=5;_tmp195.f1=_tmp1B9;_tmp195.f2=_tmp1B8;_tmp195;});_tmp194[0]=_tmp4E9;});_tmp194;});case 13: _LLE2: _tmp1BB=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp18E)->f1;_tmp1BA=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp18E)->f2;_LLE3:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp196=_cycalloc(sizeof(*_tmp196));({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct _tmp4EA=({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct _tmp197;_tmp197.tag=3;_tmp197.f1=_tmp1BB;_tmp197.f2=_tmp1BA;_tmp197;});_tmp196[0]=_tmp4EA;});_tmp196;});case 14: _LLE4: _tmp1BD=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp18E)->f1;_tmp1BC=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp18E)->f2;_LLE5:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp198=_cycalloc(sizeof(*_tmp198));({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct _tmp4EB=({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct _tmp199;_tmp199.tag=4;_tmp199.f1=_tmp1BD;_tmp199.f2=_tmp1BC;_tmp199;});_tmp198[0]=_tmp4EB;});_tmp198;});case 6: _LLE6: _LLE7:
# 929
{void*_tmp19A=Cyc_Tcutil_compress((void*)_check_null(_tmp1C3->topt));void*_tmp19B=_tmp19A;union Cyc_Absyn_Constraint*_tmp19C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19B)->tag == 5){_LLEF: _tmp19C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19B)->f1).ptr_atts).nullable;_LLF0:
# 931
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp19C))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LLEE;}else{_LLF1: _LLF2:
 goto _LLEE;}_LLEE:;}
# 936
({void*_tmp19D=0;({struct _dyneither_ptr _tmp4EC=({const char*_tmp19E="non-null pointer type or non-pointer type in pointer pattern";_tag_dyneither(_tmp19E,sizeof(char),61);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4EC,_tag_dyneither(_tmp19D,sizeof(void*),0));});});case 8: _LLE8: _tmp1BF=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp18E)->f1;_tmp1BE=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp18E)->f2;_LLE9:
# 938
 if(_tmp1BF->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp19F=_cycalloc(sizeof(*_tmp19F));({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct _tmp4ED=({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct _tmp1A0;_tmp1A0.tag=9;_tmp1A0.f1=_tmp1BF;_tmp1A0.f2=_tmp1BE;_tmp1A0;});_tmp19F[0]=_tmp4ED;});_tmp19F;});else{
# 941
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct _tmp4EF=({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct _tmp1A2;_tmp1A2.tag=7;({int _tmp4EE=(int)Cyc_Tcpat_datatype_tag_number(_tmp1BF,_tmp1BE->name);_tmp1A2.f1=_tmp4EE;});_tmp1A2.f2=_tmp1BF;_tmp1A2.f3=_tmp1BE;_tmp1A2;});_tmp1A1[0]=_tmp4EF;});_tmp1A1;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp18E)->f1 != 0){_LLEA: _tmp1C1=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp18E)->f1)->aggr_info;_tmp1C0=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp18E)->f3;_LLEB: {
# 943
struct Cyc_Absyn_Aggrdecl*_tmp1A3=Cyc_Absyn_get_known_aggrdecl(_tmp1C1);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A3->impl))->tagged){
struct _tuple18*_tmp1A4=(struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp1C0))->hd;struct _tuple18*_tmp1A5=_tmp1A4;struct Cyc_List_List*_tmp1AE;struct Cyc_Absyn_Pat*_tmp1AD;_LLF4: _tmp1AE=_tmp1A5->f1;_tmp1AD=_tmp1A5->f2;_LLF5:;{
struct _dyneither_ptr*f;
{void*_tmp1A6=(void*)((struct Cyc_List_List*)_check_null(_tmp1AE))->hd;void*_tmp1A7=_tmp1A6;struct _dyneither_ptr*_tmp1AA;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1A7)->tag == 1){_LLF7: _tmp1AA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1A7)->f1;_LLF8:
 f=_tmp1AA;goto _LLF6;}else{_LLF9: _LLFA:
({void*_tmp1A8=0;({struct _dyneither_ptr _tmp4F0=({const char*_tmp1A9="no field name in tagged union pattern";_tag_dyneither(_tmp1A9,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F0,_tag_dyneither(_tmp1A8,sizeof(void*),0));});});}_LLF6:;}
# 951
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct _tmp4F2=({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct _tmp1AC;_tmp1AC.tag=8;_tmp1AC.f1=f;({int _tmp4F1=Cyc_Tcpat_get_member_offset(_tmp1A3,f);_tmp1AC.f2=_tmp4F1;});_tmp1AC;});_tmp1AB[0]=_tmp4F2;});_tmp1AB;});};}else{
# 953
({void*_tmp1AF=0;({struct _dyneither_ptr _tmp4F3=({const char*_tmp1B0="non-tagged aggregate in pattern test";_tag_dyneither(_tmp1B0,sizeof(char),37);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F3,_tag_dyneither(_tmp1AF,sizeof(void*),0));});});}}}else{goto _LLEC;}default: _LLEC: _LLED:
({void*_tmp1B1=0;({struct _dyneither_ptr _tmp4F4=({const char*_tmp1B2="non-test pattern in pattern test";_tag_dyneither(_tmp1B2,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F4,_tag_dyneither(_tmp1B1,sizeof(void*),0));});});}_LLD5:;}}_LLD0:;}
# 959
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp1C4;(_tmp1C4.pattern).val=p;(_tmp1C4.pattern).tag=1;_tmp1C4;});}
# 963
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1C5=_cycalloc(sizeof(*_tmp1C5));({union Cyc_Tcpat_Name_value _tmp4F5=Cyc_Tcpat_Name_v(({const char*_tmp1C6="NULL";_tag_dyneither(_tmp1C6,sizeof(char),5);}));_tmp1C5->name=_tmp4F5;});_tmp1C5->arity=0;_tmp1C5->span=& Cyc_Tcpat_two_opt;({union Cyc_Tcpat_PatOrWhere _tmp4F6=Cyc_Tcpat_pw(p);_tmp1C5->orig_pat=_tmp4F6;});_tmp1C5;});}
# 966
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));({union Cyc_Tcpat_Name_value _tmp4F7=Cyc_Tcpat_Name_v(({const char*_tmp1C8="&";_tag_dyneither(_tmp1C8,sizeof(char),2);}));_tmp1C7->name=_tmp4F7;});_tmp1C7->arity=1;_tmp1C7->span=& Cyc_Tcpat_two_opt;({union Cyc_Tcpat_PatOrWhere _tmp4F8=Cyc_Tcpat_pw(p);_tmp1C7->orig_pat=_tmp4F8;});_tmp1C7;});}
# 969
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));({union Cyc_Tcpat_Name_value _tmp4F9=Cyc_Tcpat_Name_v(({const char*_tmp1CA="&";_tag_dyneither(_tmp1CA,sizeof(char),2);}));_tmp1C9->name=_tmp4F9;});_tmp1C9->arity=1;_tmp1C9->span=& Cyc_Tcpat_one_opt;({union Cyc_Tcpat_PatOrWhere _tmp4FA=Cyc_Tcpat_pw(p);_tmp1C9->orig_pat=_tmp4FA;});_tmp1C9;});}
# 972
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));({union Cyc_Tcpat_Name_value _tmp4FB=Cyc_Tcpat_Int_v(i);_tmp1CB->name=_tmp4FB;});_tmp1CB->arity=0;_tmp1CB->span=0;_tmp1CB->orig_pat=p;_tmp1CB;});}
# 975
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));({union Cyc_Tcpat_Name_value _tmp4FC=Cyc_Tcpat_Name_v(f);_tmp1CC->name=_tmp4FC;});_tmp1CC->arity=0;_tmp1CC->span=0;({union Cyc_Tcpat_PatOrWhere _tmp4FD=Cyc_Tcpat_pw(p);_tmp1CC->orig_pat=_tmp4FD;});_tmp1CC;});}
# 978
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));({union Cyc_Tcpat_Name_value _tmp4FE=Cyc_Tcpat_Int_v((int)c);_tmp1CD->name=_tmp4FE;});_tmp1CD->arity=0;_tmp1CD->span=& Cyc_Tcpat_twofiftysix_opt;({union Cyc_Tcpat_PatOrWhere _tmp4FF=Cyc_Tcpat_pw(p);_tmp1CD->orig_pat=_tmp4FF;});_tmp1CD;});}
# 981
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));({union Cyc_Tcpat_Name_value _tmp500=Cyc_Tcpat_Name_v(({const char*_tmp1CF="$";_tag_dyneither(_tmp1CF,sizeof(char),2);}));_tmp1CE->name=_tmp500;});_tmp1CE->arity=i;_tmp1CE->span=& Cyc_Tcpat_one_opt;_tmp1CE->orig_pat=p;_tmp1CE;});}
# 986
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp502=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1D1;_tmp1D1.tag=1;({struct Cyc_Tcpat_Con_s*_tmp501=Cyc_Tcpat_null_con(p);_tmp1D1.f1=_tmp501;});_tmp1D1.f2=0;_tmp1D1;});_tmp1D0[0]=_tmp502;});_tmp1D0;});}
# 989
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp504=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1D3;_tmp1D3.tag=1;({struct Cyc_Tcpat_Con_s*_tmp503=Cyc_Tcpat_int_con(i,p);_tmp1D3.f1=_tmp503;});_tmp1D3.f2=0;_tmp1D3;});_tmp1D2[0]=_tmp504;});_tmp1D2;});}
# 992
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp506=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1D5;_tmp1D5.tag=1;({struct Cyc_Tcpat_Con_s*_tmp505=Cyc_Tcpat_char_con(c,p);_tmp1D5.f1=_tmp505;});_tmp1D5.f2=0;_tmp1D5;});_tmp1D4[0]=_tmp506;});_tmp1D4;});}
# 995
static void*Cyc_Tcpat_float_pat(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp508=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1D7;_tmp1D7.tag=1;({struct Cyc_Tcpat_Con_s*_tmp507=Cyc_Tcpat_float_con(f,p);_tmp1D7.f1=_tmp507;});_tmp1D7.f2=0;_tmp1D7;});_tmp1D6[0]=_tmp508;});_tmp1D6;});}
# 998
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp50B=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1D9;_tmp1D9.tag=1;({struct Cyc_Tcpat_Con_s*_tmp509=Cyc_Tcpat_null_ptr_con(p0);_tmp1D9.f1=_tmp509;});({struct Cyc_List_List*_tmp50A=({struct Cyc_List_List*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));_tmp1DA->hd=p;_tmp1DA->tl=0;_tmp1DA;});_tmp1D9.f2=_tmp50A;});_tmp1D9;});_tmp1D8[0]=_tmp50B;});_tmp1D8;});}
# 1001
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp50E=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1DC;_tmp1DC.tag=1;({struct Cyc_Tcpat_Con_s*_tmp50C=Cyc_Tcpat_ptr_con(p0);_tmp1DC.f1=_tmp50C;});({struct Cyc_List_List*_tmp50D=({struct Cyc_List_List*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->hd=p;_tmp1DD->tl=0;_tmp1DD;});_tmp1DC.f2=_tmp50D;});_tmp1DC;});_tmp1DB[0]=_tmp50E;});_tmp1DB;});}
# 1004
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp511=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1DF;_tmp1DF.tag=1;({struct Cyc_Tcpat_Con_s*_tmp510=({int _tmp50F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp50F,p);});_tmp1DF.f1=_tmp510;});_tmp1DF.f2=ss;_tmp1DF;});_tmp1DE[0]=_tmp511;});_tmp1DE;});}
# 1007
static void*Cyc_Tcpat_con_pat(struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1009
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));({union Cyc_Tcpat_Name_value _tmp512=Cyc_Tcpat_Name_v(con_name);_tmp1E2->name=_tmp512;});({int _tmp513=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp1E2->arity=_tmp513;});_tmp1E2->span=span;({union Cyc_Tcpat_PatOrWhere _tmp514=Cyc_Tcpat_pw(p);_tmp1E2->orig_pat=_tmp514;});_tmp1E2;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1E0=_cycalloc(sizeof(*_tmp1E0));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp515=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1E1;_tmp1E1.tag=1;_tmp1E1.f1=c;_tmp1E1.f2=ps;_tmp1E1;});_tmp1E0[0]=_tmp515;});_tmp1E0;});}
# 1014
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1E3=p->r;void*_tmp1E4=_tmp1E3;void*_tmp227;struct Cyc_Absyn_Enumfield*_tmp226;struct Cyc_Absyn_Enumdecl*_tmp225;struct Cyc_Absyn_Enumfield*_tmp224;struct Cyc_Absyn_Aggrdecl*_tmp223;struct Cyc_List_List*_tmp222;struct Cyc_List_List*_tmp221;struct Cyc_Absyn_Datatypedecl*_tmp220;struct Cyc_Absyn_Datatypefield*_tmp21F;struct Cyc_List_List*_tmp21E;struct Cyc_Absyn_Pat*_tmp21D;struct Cyc_Absyn_Pat*_tmp21C;struct Cyc_Absyn_Pat*_tmp21B;struct _dyneither_ptr _tmp21A;char _tmp219;enum Cyc_Absyn_Sign _tmp218;int _tmp217;switch(*((int*)_tmp1E4)){case 0: _LLFC: _LLFD:
 goto _LLFF;case 2: _LLFE: _LLFF:
 goto _LL101;case 4: _LL100: _LL101:
({void*_tmp517=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1E5=_cycalloc_atomic(sizeof(*_tmp1E5));({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp516=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp1E6;_tmp1E6.tag=0;_tmp1E6;});_tmp1E5[0]=_tmp516;});_tmp1E5;});s=_tmp517;});goto _LLFB;case 9: _LL102: _LL103:
({void*_tmp518=Cyc_Tcpat_null_pat(p);s=_tmp518;});goto _LLFB;case 10: _LL104: _tmp218=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1;_tmp217=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1E4)->f2;_LL105:
({void*_tmp51A=({int _tmp519=_tmp217;Cyc_Tcpat_int_pat(_tmp519,Cyc_Tcpat_pw(p));});s=_tmp51A;});goto _LLFB;case 11: _LL106: _tmp219=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1;_LL107:
({void*_tmp51B=Cyc_Tcpat_char_pat(_tmp219,p);s=_tmp51B;});goto _LLFB;case 12: _LL108: _tmp21A=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1;_LL109:
({void*_tmp51C=Cyc_Tcpat_float_pat(_tmp21A,p);s=_tmp51C;});goto _LLFB;case 1: _LL10A: _tmp21B=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1E4)->f2;_LL10B:
({void*_tmp51D=Cyc_Tcpat_compile_pat(_tmp21B);s=_tmp51D;});goto _LLFB;case 3: _LL10C: _tmp21C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E4)->f2;_LL10D:
({void*_tmp51E=Cyc_Tcpat_compile_pat(_tmp21C);s=_tmp51E;});goto _LLFB;case 6: _LL10E: _tmp21D=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1;_LL10F:
# 1027
{void*_tmp1E7=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1E8=_tmp1E7;union Cyc_Absyn_Constraint*_tmp1EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E8)->tag == 5){_LL123: _tmp1EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E8)->f1).ptr_atts).nullable;_LL124: {
# 1029
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1E9=_tmp1EF;int _tmp1EC;union Cyc_Absyn_Constraint*_tmp1EB;switch((((union Cyc_Absyn_Constraint*)_tmp1E9)->No_constr).tag){case 2: _LL128: _tmp1EB=(_tmp1E9->Forward_constr).val;_LL129:
# 1034
*_tmp1EF=*_tmp1EB;
continue;case 3: _LL12A: _LL12B:
# 1037
({struct _union_Constraint_Eq_constr*_tmp1EA=& _tmp1EF->Eq_constr;_tmp1EA->tag=1;_tmp1EA->val=0;});
is_nullable=0;
still_working=0;
goto _LL127;default: _LL12C: _tmp1EC=(int)(_tmp1E9->Eq_constr).val;_LL12D:
# 1042
 is_nullable=_tmp1EC;
still_working=0;
goto _LL127;}_LL127:;}{
# 1047
void*ss=Cyc_Tcpat_compile_pat(_tmp21D);
if(is_nullable)
({void*_tmp51F=Cyc_Tcpat_null_ptr_pat(ss,p);s=_tmp51F;});else{
# 1051
({void*_tmp520=Cyc_Tcpat_ptr_pat(ss,p);s=_tmp520;});}
goto _LL122;};}}else{_LL125: _LL126:
({void*_tmp1ED=0;({struct _dyneither_ptr _tmp521=({const char*_tmp1EE="expecting pointertype for pattern!";_tag_dyneither(_tmp1EE,sizeof(char),35);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp521,_tag_dyneither(_tmp1ED,sizeof(void*),0));});});}_LL122:;}
# 1055
goto _LLFB;case 8: _LL110: _tmp220=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1;_tmp21F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E4)->f2;_tmp21E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E4)->f3;_LL111: {
# 1057
int*span;
{void*_tmp1F0=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1F1=_tmp1F0;switch(*((int*)_tmp1F1)){case 3: _LL12F: _LL130:
# 1060
 if(_tmp220->is_extensible)
span=0;else{
# 1063
({int*_tmp523=({int*_tmp1F2=_cycalloc_atomic(sizeof(*_tmp1F2));({int _tmp522=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp220->fields))->v);_tmp1F2[0]=_tmp522;});_tmp1F2;});span=_tmp523;});}
goto _LL12E;case 4: _LL131: _LL132:
 span=& Cyc_Tcpat_one_opt;goto _LL12E;default: _LL133: _LL134:
({int*_tmp525=({void*_tmp1F3=0;({struct _dyneither_ptr _tmp524=({const char*_tmp1F4="void datatype pattern has bad type";_tag_dyneither(_tmp1F4,sizeof(char),35);});((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp524,_tag_dyneither(_tmp1F3,sizeof(void*),0));});});span=_tmp525;});goto _LL12E;}_LL12E:;}
# 1068
({void*_tmp529=({struct _dyneither_ptr _tmp528=*(*_tmp21F->name).f2;int*_tmp527=span;struct Cyc_List_List*_tmp526=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp21E);Cyc_Tcpat_con_pat(_tmp528,_tmp527,_tmp526,p);});s=_tmp529;});
goto _LLFB;}case 5: _LL112: _tmp221=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1;_LL113:
# 1072
({void*_tmp52B=({struct Cyc_List_List*_tmp52A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp221);Cyc_Tcpat_tuple_pat(_tmp52A,Cyc_Tcpat_pw(p));});s=_tmp52B;});goto _LLFB;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1)->aggr_info).KnownAggr).tag == 2){_LL114: _tmp223=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1)->aggr_info).KnownAggr).val;_tmp222=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E4)->f3;_LL115:
# 1077
 if(_tmp223->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp223->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1081
int found=({struct _dyneither_ptr _tmp52C=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp52C,({const char*_tmp200="";_tag_dyneither(_tmp200,sizeof(char),1);}));})== 0;
{struct Cyc_List_List*dlps0=_tmp222;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple18*_tmp1F5=(struct _tuple18*)dlps0->hd;struct _tuple18*_tmp1F6=_tmp1F5;struct Cyc_List_List*_tmp1FD;struct Cyc_Absyn_Pat*_tmp1FC;_LL136: _tmp1FD=_tmp1F6->f1;_tmp1FC=_tmp1F6->f2;_LL137:;{
struct Cyc_List_List*_tmp1F7=_tmp1FD;struct _dyneither_ptr*_tmp1FB;if(_tmp1F7 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp1F7)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp1F7)->tl == 0){_LL139: _tmp1FB=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1F7->hd)->f1;_LL13A:
# 1086
 if(Cyc_strptrcmp(_tmp1FB,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0){
({struct Cyc_List_List*_tmp52E=({struct Cyc_List_List*_tmp1F8=_cycalloc(sizeof(*_tmp1F8));({void*_tmp52D=Cyc_Tcpat_compile_pat(_tmp1FC);_tmp1F8->hd=_tmp52D;});_tmp1F8->tl=ps;_tmp1F8;});ps=_tmp52E;});
found=1;}
# 1090
goto _LL138;}else{goto _LL13B;}}else{goto _LL13B;}}else{_LL13B: _LL13C:
({void*_tmp1F9=0;({struct _dyneither_ptr _tmp52F=({const char*_tmp1FA="bad designator(s)";_tag_dyneither(_tmp1FA,sizeof(char),18);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp52F,_tag_dyneither(_tmp1F9,sizeof(void*),0));});});}_LL138:;};}}
# 1094
if(!found)
({void*_tmp1FE=0;({struct _dyneither_ptr _tmp530=({const char*_tmp1FF="bad designator";_tag_dyneither(_tmp1FF,sizeof(char),15);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp530,_tag_dyneither(_tmp1FE,sizeof(void*),0));});});}}
# 1097
({void*_tmp532=({struct Cyc_List_List*_tmp531=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp531,Cyc_Tcpat_pw(p));});s=_tmp532;});}else{
# 1100
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp223->impl))->tagged)
({void*_tmp201=0;({unsigned int _tmp534=p->loc;struct _dyneither_ptr _tmp533=({const char*_tmp202="patterns on untagged unions not yet supported.";_tag_dyneither(_tmp202,sizeof(char),47);});Cyc_Tcutil_terr(_tmp534,_tmp533,_tag_dyneither(_tmp201,sizeof(void*),0));});});{
int*span=({int*_tmp209=_cycalloc_atomic(sizeof(*_tmp209));({int _tmp535=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp223->impl))->fields);_tmp209[0]=_tmp535;});_tmp209;});
struct Cyc_List_List*_tmp203=_tmp222;struct _dyneither_ptr*_tmp208;struct Cyc_Absyn_Pat*_tmp207;if(_tmp203 != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp203)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp203)->hd)->f1)->hd)->tag == 1){if(((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp203)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp203)->tl == 0){_LL13E: _tmp208=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp203->hd)->f1)->hd)->f1;_tmp207=((struct _tuple18*)_tmp203->hd)->f2;_LL13F:
# 1105
({void*_tmp53A=({struct _dyneither_ptr _tmp539=*_tmp208;int*_tmp538=span;struct Cyc_List_List*_tmp537=({struct Cyc_List_List*_tmp204=_cycalloc(sizeof(*_tmp204));({void*_tmp536=Cyc_Tcpat_compile_pat(_tmp207);_tmp204->hd=_tmp536;});_tmp204->tl=0;_tmp204;});Cyc_Tcpat_con_pat(_tmp539,_tmp538,_tmp537,p);});s=_tmp53A;});
goto _LL13D;}else{goto _LL140;}}else{goto _LL140;}}else{goto _LL140;}}else{goto _LL140;}}else{_LL140: _LL141:
({void*_tmp205=0;({struct _dyneither_ptr _tmp53B=({const char*_tmp206="bad union pattern";_tag_dyneither(_tmp206,sizeof(char),18);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp53B,_tag_dyneither(_tmp205,sizeof(void*),0));});});}_LL13D:;};}
# 1110
goto _LLFB;}else{goto _LL11E;}}else{_LL11E: _LL11F:
# 1152
 goto _LL121;}case 13: _LL116: _tmp225=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1;_tmp224=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1E4)->f2;_LL117:
# 1115
{void*_tmp20A=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp20B=_tmp20A;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp20B)->tag == 6){_LL143: _LL144:
# 1119
({void*_tmp53C=Cyc_Tcpat_con_pat(*(*_tmp224->name).f2,0,0,p);s=_tmp53C;});
goto _LL142;}else{_LL145: _LL146: {
# 1122
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp225->fields))->v);
({void*_tmp53F=({struct _dyneither_ptr _tmp53E=*(*_tmp224->name).f2;int*_tmp53D=({int*_tmp20C=_cycalloc_atomic(sizeof(*_tmp20C));_tmp20C[0]=span;_tmp20C;});Cyc_Tcpat_con_pat(_tmp53E,_tmp53D,0,p);});s=_tmp53F;});
goto _LL142;}}_LL142:;}
# 1126
goto _LLFB;case 14: _LL118: _tmp227=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1E4)->f1;_tmp226=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1E4)->f2;_LL119: {
# 1131
struct Cyc_List_List*fields;
{void*_tmp20D=Cyc_Tcutil_compress(_tmp227);void*_tmp20E=_tmp20D;struct Cyc_List_List*_tmp211;if(((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp20E)->tag == 14){_LL148: _tmp211=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp20E)->f1;_LL149:
 fields=_tmp211;goto _LL147;}else{_LL14A: _LL14B:
({void*_tmp20F=0;({struct _dyneither_ptr _tmp540=({const char*_tmp210="bad type in AnonEnum_p";_tag_dyneither(_tmp210,sizeof(char),23);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp540,_tag_dyneither(_tmp20F,sizeof(void*),0));});});}_LL147:;}
# 1138
{void*_tmp212=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp213=_tmp212;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp213)->tag == 6){_LL14D: _LL14E:
# 1142
({void*_tmp541=Cyc_Tcpat_con_pat(*(*_tmp226->name).f2,0,0,p);s=_tmp541;});
goto _LL14C;}else{_LL14F: _LL150: {
# 1145
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
({void*_tmp544=({struct _dyneither_ptr _tmp543=*(*_tmp226->name).f2;int*_tmp542=({int*_tmp214=_cycalloc_atomic(sizeof(*_tmp214));_tmp214[0]=span;_tmp214;});Cyc_Tcpat_con_pat(_tmp543,_tmp542,0,p);});s=_tmp544;});
goto _LL14C;}}_LL14C:;}
# 1149
goto _LLFB;}case 15: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;default: _LL120: _LL121:
# 1153
({void*_tmp546=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp215=_cycalloc_atomic(sizeof(*_tmp215));({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp545=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp216;_tmp216.tag=0;_tmp216;});_tmp215[0]=_tmp545;});_tmp215;});s=_tmp546;});}_LLFB:;}
# 1155
return s;}
# 1163
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1177
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1184
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp228=({struct _tuple0 _tmp234;_tmp234.f1=a1;_tmp234.f2=a2;_tmp234;});struct _tuple0 _tmp229=_tmp228;int _tmp233;struct _dyneither_ptr*_tmp232;int _tmp231;struct _dyneither_ptr*_tmp230;struct Cyc_Absyn_Datatypefield*_tmp22F;unsigned int _tmp22E;struct Cyc_Absyn_Datatypefield*_tmp22D;unsigned int _tmp22C;unsigned int _tmp22B;unsigned int _tmp22A;switch(*((int*)_tmp229.f1)){case 0: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp229.f2)->tag == 0){_LL152: _LL153:
 return 1;}else{goto _LL15C;}case 1: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp229.f2)->tag == 1){_LL154: _LL155:
 return 1;}else{goto _LL15C;}case 2: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp229.f2)->tag == 2){_LL156: _tmp22B=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp229.f1)->f1;_tmp22A=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp229.f2)->f1;_LL157:
 return _tmp22B == _tmp22A;}else{goto _LL15C;}case 3: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp229.f2)->tag == 3){_LL158: _tmp22F=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp229.f1)->f2;_tmp22E=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp229.f1)->f3;_tmp22D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp229.f2)->f2;_tmp22C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp229.f2)->f3;_LL159:
# 1190
 return _tmp22F == _tmp22D  && _tmp22E == _tmp22C;}else{goto _LL15C;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp229.f2)->tag == 4){_LL15A: _tmp233=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp229.f1)->f1;_tmp232=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp229.f1)->f2;_tmp231=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp229.f2)->f1;_tmp230=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp229.f2)->f2;_LL15B:
# 1192
 return _tmp233 == _tmp231  && Cyc_strptrcmp(_tmp232,_tmp230)== 0;}else{_LL15C: _LL15D:
 return 0;}}_LL151:;}
# 1197
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0  && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1203
if(p1 != p2)return 0;
return 1;}
# 1207
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp235=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp236=d2;struct Cyc_List_List*_tmp23D;struct Cyc_List_List*_tmp23C;void*_tmp23B;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp236)->tag == 2){_LL15F: _tmp23D=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp236)->f1;_tmp23C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp236)->f2;_tmp23B=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp236)->f3;_LL160:
# 1211
 if(Cyc_Tcpat_same_path(access,_tmp23D) && (int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp237=_cycalloc(sizeof(*_tmp237));({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp549=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp238;_tmp238.tag=2;_tmp238.f1=_tmp23D;({struct Cyc_List_List*_tmp548=({struct Cyc_List_List*_tmp239=_cycalloc(sizeof(*_tmp239));({struct _tuple0*_tmp547=({struct _tuple0*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->f1=_tmp235;_tmp23A->f2=d1;_tmp23A;});_tmp239->hd=_tmp547;});_tmp239->tl=_tmp23C;_tmp239;});_tmp238.f2=_tmp548;});_tmp238.f3=_tmp23B;_tmp238;});_tmp237[0]=_tmp549;});_tmp237;});else{
# 1215
goto _LL15E;}}else{_LL161: _LL162:
 goto _LL15E;}_LL15E:;}
# 1218
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp54C=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp23F;_tmp23F.tag=2;_tmp23F.f1=access;({struct Cyc_List_List*_tmp54B=({struct Cyc_List_List*_tmp240=_cycalloc(sizeof(*_tmp240));({struct _tuple0*_tmp54A=({struct _tuple0*_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241->f1=_tmp235;_tmp241->f2=d1;_tmp241;});_tmp240->hd=_tmp54A;});_tmp240->tl=0;_tmp240;});_tmp23F.f2=_tmp54B;});_tmp23F.f3=d2;_tmp23F;});_tmp23E[0]=_tmp54C;});_tmp23E;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1224
typedef struct _tuple20*Cyc_Tcpat_work_stack_frame_t;
# 1227
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1229
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1232
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp242=0;({struct Cyc___cycFILE*_tmp54E=Cyc_stderr;struct _dyneither_ptr _tmp54D=({const char*_tmp243=" ";_tag_dyneither(_tmp243,sizeof(char),2);});Cyc_fprintf(_tmp54E,_tmp54D,_tag_dyneither(_tmp242,sizeof(void*),0));});});}}
# 1238
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp244=c->name;
union Cyc_Tcpat_Name_value _tmp245=_tmp244;int _tmp24D;struct _dyneither_ptr _tmp24C;if((_tmp245.Name_v).tag == 1){_LL164: _tmp24C=(_tmp245.Name_v).val;_LL165:
({struct Cyc_String_pa_PrintArg_struct _tmp248;_tmp248.tag=0;_tmp248.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp24C);({void*_tmp246[1]={& _tmp248};({struct Cyc___cycFILE*_tmp550=Cyc_stderr;struct _dyneither_ptr _tmp54F=({const char*_tmp247="%s";_tag_dyneither(_tmp247,sizeof(char),3);});Cyc_fprintf(_tmp550,_tmp54F,_tag_dyneither(_tmp246,sizeof(void*),1));});});});goto _LL163;}else{_LL166: _tmp24D=(_tmp245.Int_v).val;_LL167:
({struct Cyc_Int_pa_PrintArg_struct _tmp24B;_tmp24B.tag=1;_tmp24B.f1=(unsigned long)_tmp24D;({void*_tmp249[1]={& _tmp24B};({struct Cyc___cycFILE*_tmp552=Cyc_stderr;struct _dyneither_ptr _tmp551=({const char*_tmp24A="%d";_tag_dyneither(_tmp24A,sizeof(char),3);});Cyc_fprintf(_tmp552,_tmp551,_tag_dyneither(_tmp249,sizeof(void*),1));});});});goto _LL163;}_LL163:;}
# 1246
static void Cyc_Tcpat_print_access(void*a){
void*_tmp24E=a;int _tmp264;struct _dyneither_ptr*_tmp263;struct Cyc_Absyn_Datatypefield*_tmp262;unsigned int _tmp261;unsigned int _tmp260;switch(*((int*)_tmp24E)){case 0: _LL169: _LL16A:
({void*_tmp24F=0;({struct Cyc___cycFILE*_tmp554=Cyc_stderr;struct _dyneither_ptr _tmp553=({const char*_tmp250="DUMMY";_tag_dyneither(_tmp250,sizeof(char),6);});Cyc_fprintf(_tmp554,_tmp553,_tag_dyneither(_tmp24F,sizeof(void*),0));});});goto _LL168;case 1: _LL16B: _LL16C:
({void*_tmp251=0;({struct Cyc___cycFILE*_tmp556=Cyc_stderr;struct _dyneither_ptr _tmp555=({const char*_tmp252="*";_tag_dyneither(_tmp252,sizeof(char),2);});Cyc_fprintf(_tmp556,_tmp555,_tag_dyneither(_tmp251,sizeof(void*),0));});});goto _LL168;case 2: _LL16D: _tmp260=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp24E)->f1;_LL16E:
({struct Cyc_Int_pa_PrintArg_struct _tmp255;_tmp255.tag=1;_tmp255.f1=(unsigned long)((int)_tmp260);({void*_tmp253[1]={& _tmp255};({struct Cyc___cycFILE*_tmp558=Cyc_stderr;struct _dyneither_ptr _tmp557=({const char*_tmp254="[%d]";_tag_dyneither(_tmp254,sizeof(char),5);});Cyc_fprintf(_tmp558,_tmp557,_tag_dyneither(_tmp253,sizeof(void*),1));});});});goto _LL168;case 3: _LL16F: _tmp262=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp24E)->f2;_tmp261=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp24E)->f3;_LL170:
# 1252
({struct Cyc_Int_pa_PrintArg_struct _tmp259;_tmp259.tag=1;_tmp259.f1=(unsigned long)((int)_tmp261);({struct Cyc_String_pa_PrintArg_struct _tmp258;_tmp258.tag=0;({struct _dyneither_ptr _tmp559=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp262->name));_tmp258.f1=_tmp559;});({void*_tmp256[2]={& _tmp258,& _tmp259};({struct Cyc___cycFILE*_tmp55B=Cyc_stderr;struct _dyneither_ptr _tmp55A=({const char*_tmp257="%s[%d]";_tag_dyneither(_tmp257,sizeof(char),7);});Cyc_fprintf(_tmp55B,_tmp55A,_tag_dyneither(_tmp256,sizeof(void*),2));});});});});goto _LL168;default: _LL171: _tmp264=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp24E)->f1;_tmp263=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp24E)->f2;_LL172:
# 1254
 if(_tmp264)
({struct Cyc_String_pa_PrintArg_struct _tmp25C;_tmp25C.tag=0;_tmp25C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp263);({void*_tmp25A[1]={& _tmp25C};({struct Cyc___cycFILE*_tmp55D=Cyc_stderr;struct _dyneither_ptr _tmp55C=({const char*_tmp25B=".tagunion.%s";_tag_dyneither(_tmp25B,sizeof(char),13);});Cyc_fprintf(_tmp55D,_tmp55C,_tag_dyneither(_tmp25A,sizeof(void*),1));});});});else{
# 1257
({struct Cyc_String_pa_PrintArg_struct _tmp25F;_tmp25F.tag=0;_tmp25F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp263);({void*_tmp25D[1]={& _tmp25F};({struct Cyc___cycFILE*_tmp55F=Cyc_stderr;struct _dyneither_ptr _tmp55E=({const char*_tmp25E=".%s";_tag_dyneither(_tmp25E,sizeof(char),4);});Cyc_fprintf(_tmp55F,_tmp55E,_tag_dyneither(_tmp25D,sizeof(void*),1));});});});}
goto _LL168;}_LL168:;}
# 1262
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp265=p;struct Cyc_Absyn_Datatypefield*_tmp28A;struct _dyneither_ptr*_tmp289;int _tmp288;int _tmp287;unsigned int _tmp286;struct _dyneither_ptr _tmp285;struct Cyc_Absyn_Enumfield*_tmp284;struct Cyc_Absyn_Enumfield*_tmp283;struct Cyc_Absyn_Exp*_tmp282;switch(*((int*)_tmp265)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp265)->f1 == 0){_LL174: _LL175:
({void*_tmp266=0;({struct Cyc___cycFILE*_tmp561=Cyc_stderr;struct _dyneither_ptr _tmp560=({const char*_tmp267="where(NULL)";_tag_dyneither(_tmp267,sizeof(char),12);});Cyc_fprintf(_tmp561,_tmp560,_tag_dyneither(_tmp266,sizeof(void*),0));});});goto _LL173;}else{_LL176: _tmp282=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp265)->f1;_LL177:
({struct Cyc_String_pa_PrintArg_struct _tmp26A;_tmp26A.tag=0;({struct _dyneither_ptr _tmp562=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp282)));_tmp26A.f1=_tmp562;});({void*_tmp268[1]={& _tmp26A};({struct Cyc___cycFILE*_tmp564=Cyc_stderr;struct _dyneither_ptr _tmp563=({const char*_tmp269="where(%s)";_tag_dyneither(_tmp269,sizeof(char),10);});Cyc_fprintf(_tmp564,_tmp563,_tag_dyneither(_tmp268,sizeof(void*),1));});});});goto _LL173;}case 1: _LL178: _LL179:
({void*_tmp26B=0;({struct Cyc___cycFILE*_tmp566=Cyc_stderr;struct _dyneither_ptr _tmp565=({const char*_tmp26C="NULL";_tag_dyneither(_tmp26C,sizeof(char),5);});Cyc_fprintf(_tmp566,_tmp565,_tag_dyneither(_tmp26B,sizeof(void*),0));});});goto _LL173;case 2: _LL17A: _LL17B:
({void*_tmp26D=0;({struct Cyc___cycFILE*_tmp568=Cyc_stderr;struct _dyneither_ptr _tmp567=({const char*_tmp26E="NOT-NULL:";_tag_dyneither(_tmp26E,sizeof(char),10);});Cyc_fprintf(_tmp568,_tmp567,_tag_dyneither(_tmp26D,sizeof(void*),0));});});goto _LL173;case 4: _LL17C: _tmp283=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp265)->f2;_LL17D:
 _tmp284=_tmp283;goto _LL17F;case 3: _LL17E: _tmp284=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp265)->f2;_LL17F:
({struct Cyc_String_pa_PrintArg_struct _tmp271;_tmp271.tag=0;({struct _dyneither_ptr _tmp569=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp284->name));_tmp271.f1=_tmp569;});({void*_tmp26F[1]={& _tmp271};({struct Cyc___cycFILE*_tmp56B=Cyc_stderr;struct _dyneither_ptr _tmp56A=({const char*_tmp270="%s";_tag_dyneither(_tmp270,sizeof(char),3);});Cyc_fprintf(_tmp56B,_tmp56A,_tag_dyneither(_tmp26F,sizeof(void*),1));});});});goto _LL173;case 5: _LL180: _tmp285=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp265)->f1;_LL181:
({struct Cyc_String_pa_PrintArg_struct _tmp274;_tmp274.tag=0;_tmp274.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp285);({void*_tmp272[1]={& _tmp274};({struct Cyc___cycFILE*_tmp56D=Cyc_stderr;struct _dyneither_ptr _tmp56C=({const char*_tmp273="%s";_tag_dyneither(_tmp273,sizeof(char),3);});Cyc_fprintf(_tmp56D,_tmp56C,_tag_dyneither(_tmp272,sizeof(void*),1));});});});goto _LL173;case 6: _LL182: _tmp286=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp265)->f1;_LL183:
({struct Cyc_Int_pa_PrintArg_struct _tmp277;_tmp277.tag=1;_tmp277.f1=(unsigned long)((int)_tmp286);({void*_tmp275[1]={& _tmp277};({struct Cyc___cycFILE*_tmp56F=Cyc_stderr;struct _dyneither_ptr _tmp56E=({const char*_tmp276="%d";_tag_dyneither(_tmp276,sizeof(char),3);});Cyc_fprintf(_tmp56F,_tmp56E,_tag_dyneither(_tmp275,sizeof(void*),1));});});});goto _LL173;case 7: _LL184: _tmp287=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp265)->f1;_LL185:
({struct Cyc_Int_pa_PrintArg_struct _tmp27A;_tmp27A.tag=1;_tmp27A.f1=(unsigned long)_tmp287;({void*_tmp278[1]={& _tmp27A};({struct Cyc___cycFILE*_tmp571=Cyc_stderr;struct _dyneither_ptr _tmp570=({const char*_tmp279="datatypetag(%d)";_tag_dyneither(_tmp279,sizeof(char),16);});Cyc_fprintf(_tmp571,_tmp570,_tag_dyneither(_tmp278,sizeof(void*),1));});});});goto _LL173;case 8: _LL186: _tmp289=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp265)->f1;_tmp288=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp265)->f2;_LL187:
({struct Cyc_Int_pa_PrintArg_struct _tmp27E;_tmp27E.tag=1;_tmp27E.f1=(unsigned long)_tmp288;({struct Cyc_String_pa_PrintArg_struct _tmp27D;_tmp27D.tag=0;_tmp27D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp289);({void*_tmp27B[2]={& _tmp27D,& _tmp27E};({struct Cyc___cycFILE*_tmp573=Cyc_stderr;struct _dyneither_ptr _tmp572=({const char*_tmp27C="uniontag[%s](%d)";_tag_dyneither(_tmp27C,sizeof(char),17);});Cyc_fprintf(_tmp573,_tmp572,_tag_dyneither(_tmp27B,sizeof(void*),2));});});});});goto _LL173;default: _LL188: _tmp28A=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp265)->f2;_LL189:
# 1275
({struct Cyc_String_pa_PrintArg_struct _tmp281;_tmp281.tag=0;({struct _dyneither_ptr _tmp574=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp28A->name));_tmp281.f1=_tmp574;});({void*_tmp27F[1]={& _tmp281};({struct Cyc___cycFILE*_tmp576=Cyc_stderr;struct _dyneither_ptr _tmp575=({const char*_tmp280="datatypefield(%s)";_tag_dyneither(_tmp280,sizeof(char),18);});Cyc_fprintf(_tmp576,_tmp575,_tag_dyneither(_tmp27F,sizeof(void*),1));});});});}_LL173:;}
# 1279
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp28D;_tmp28D.tag=0;({struct _dyneither_ptr _tmp577=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp28D.f1=_tmp577;});({void*_tmp28B[1]={& _tmp28D};({struct Cyc___cycFILE*_tmp579=Cyc_stderr;struct _dyneither_ptr _tmp578=({const char*_tmp28C="%s";_tag_dyneither(_tmp28C,sizeof(char),3);});Cyc_fprintf(_tmp579,_tmp578,_tag_dyneither(_tmp28B,sizeof(void*),1));});});});}
# 1283
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp28E=d;struct Cyc_List_List*_tmp2AA;struct Cyc_List_List*_tmp2A9;void*_tmp2A8;struct Cyc_Tcpat_Rhs*_tmp2A7;switch(*((int*)_tmp28E)){case 1: _LL18B: _tmp2A7=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp28E)->f1;_LL18C:
# 1286
 Cyc_Tcpat_print_tab(tab);
({void*_tmp28F=0;({struct Cyc___cycFILE*_tmp57B=Cyc_stderr;struct _dyneither_ptr _tmp57A=({const char*_tmp290="Success(";_tag_dyneither(_tmp290,sizeof(char),9);});Cyc_fprintf(_tmp57B,_tmp57A,_tag_dyneither(_tmp28F,sizeof(void*),0));});});Cyc_Tcpat_print_rhs(_tmp2A7);({void*_tmp291=0;({struct Cyc___cycFILE*_tmp57D=Cyc_stderr;struct _dyneither_ptr _tmp57C=({const char*_tmp292=")\n";_tag_dyneither(_tmp292,sizeof(char),3);});Cyc_fprintf(_tmp57D,_tmp57C,_tag_dyneither(_tmp291,sizeof(void*),0));});});
goto _LL18A;case 0: _LL18D: _LL18E:
({void*_tmp293=0;({struct Cyc___cycFILE*_tmp57F=Cyc_stderr;struct _dyneither_ptr _tmp57E=({const char*_tmp294="Failure\n";_tag_dyneither(_tmp294,sizeof(char),9);});Cyc_fprintf(_tmp57F,_tmp57E,_tag_dyneither(_tmp293,sizeof(void*),0));});});goto _LL18A;default: _LL18F: _tmp2AA=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp28E)->f1;_tmp2A9=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp28E)->f2;_tmp2A8=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp28E)->f3;_LL190:
# 1291
 Cyc_Tcpat_print_tab(tab);
({void*_tmp295=0;({struct Cyc___cycFILE*_tmp581=Cyc_stderr;struct _dyneither_ptr _tmp580=({const char*_tmp296="Switch[";_tag_dyneither(_tmp296,sizeof(char),8);});Cyc_fprintf(_tmp581,_tmp580,_tag_dyneither(_tmp295,sizeof(void*),0));});});
for(0;_tmp2AA != 0;_tmp2AA=_tmp2AA->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp2AA->hd)->access);
if(_tmp2AA->tl != 0)({void*_tmp297=0;({struct Cyc___cycFILE*_tmp583=Cyc_stderr;struct _dyneither_ptr _tmp582=({const char*_tmp298=",";_tag_dyneither(_tmp298,sizeof(char),2);});Cyc_fprintf(_tmp583,_tmp582,_tag_dyneither(_tmp297,sizeof(void*),0));});});}
# 1297
({void*_tmp299=0;({struct Cyc___cycFILE*_tmp585=Cyc_stderr;struct _dyneither_ptr _tmp584=({const char*_tmp29A="] {\n";_tag_dyneither(_tmp29A,sizeof(char),5);});Cyc_fprintf(_tmp585,_tmp584,_tag_dyneither(_tmp299,sizeof(void*),0));});});
for(0;_tmp2A9 != 0;_tmp2A9=_tmp2A9->tl){
struct _tuple0 _tmp29B=*((struct _tuple0*)_tmp2A9->hd);struct _tuple0 _tmp29C=_tmp29B;void*_tmp2A2;void*_tmp2A1;_LL192: _tmp2A2=_tmp29C.f1;_tmp2A1=_tmp29C.f2;_LL193:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp29D=0;({struct Cyc___cycFILE*_tmp587=Cyc_stderr;struct _dyneither_ptr _tmp586=({const char*_tmp29E="case ";_tag_dyneither(_tmp29E,sizeof(char),6);});Cyc_fprintf(_tmp587,_tmp586,_tag_dyneither(_tmp29D,sizeof(void*),0));});});
Cyc_Tcpat_print_pat_test(_tmp2A2);
({void*_tmp29F=0;({struct Cyc___cycFILE*_tmp589=Cyc_stderr;struct _dyneither_ptr _tmp588=({const char*_tmp2A0=":\n";_tag_dyneither(_tmp2A0,sizeof(char),3);});Cyc_fprintf(_tmp589,_tmp588,_tag_dyneither(_tmp29F,sizeof(void*),0));});});
Cyc_Tcpat_print_dec_tree(_tmp2A1,tab + 7);}
# 1306
Cyc_Tcpat_print_tab(tab);
({void*_tmp2A3=0;({struct Cyc___cycFILE*_tmp58B=Cyc_stderr;struct _dyneither_ptr _tmp58A=({const char*_tmp2A4="default:\n";_tag_dyneither(_tmp2A4,sizeof(char),10);});Cyc_fprintf(_tmp58B,_tmp58A,_tag_dyneither(_tmp2A3,sizeof(void*),0));});});
Cyc_Tcpat_print_dec_tree(_tmp2A8,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp2A5=0;({struct Cyc___cycFILE*_tmp58D=Cyc_stderr;struct _dyneither_ptr _tmp58C=({const char*_tmp2A6="}\n";_tag_dyneither(_tmp2A6,sizeof(char),3);});Cyc_fprintf(_tmp58D,_tmp58C,_tag_dyneither(_tmp2A5,sizeof(void*),0));});});}_LL18A:;}
# 1314
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1322
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp2AB=td;struct Cyc_Set_Set*_tmp2B0;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2AB)->tag == 1){_LL195: _tmp2B0=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2AB)->f1;_LL196:
# 1332
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2AC=_cycalloc(sizeof(*_tmp2AC));({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp58F=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2AD;_tmp2AD.tag=1;({struct Cyc_Set_Set*_tmp58E=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp2B0,c);_tmp2AD.f1=_tmp58E;});_tmp2AD;});_tmp2AC[0]=_tmp58F;});_tmp2AC;});}else{_LL197: _LL198:
({void*_tmp2AE=0;({struct _dyneither_ptr _tmp590=({const char*_tmp2AF="add_neg called when td is Positive";_tag_dyneither(_tmp2AF,sizeof(char),35);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp590,_tag_dyneither(_tmp2AE,sizeof(void*),0));});});}_LL194:;}
# 1339
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2B1=td;struct Cyc_Set_Set*_tmp2B3;struct Cyc_Tcpat_Con_s*_tmp2B2;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B1)->tag == 0){_LL19A: _tmp2B2=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B1)->f1;_LL19B:
# 1343
 if(Cyc_Tcpat_compare_con(c,_tmp2B2)== 0)return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL19C: _tmp2B3=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2B1)->f1;_LL19D:
# 1347
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2B3,c))return Cyc_Tcpat_No;else{
# 1350
if(c->span != 0  && ({int _tmp591=*((int*)_check_null(c->span));_tmp591 == ((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2B3)+ 1;}))
return Cyc_Tcpat_Yes;else{
# 1353
return Cyc_Tcpat_Maybe;}}}_LL199:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1361
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp2B4=ctxt;struct Cyc_Tcpat_Con_s*_tmp2BA;struct Cyc_List_List*_tmp2B9;struct Cyc_List_List*_tmp2B8;if(_tmp2B4 == 0){_LL19F: _LL1A0:
 return 0;}else{_LL1A1: _tmp2BA=((struct _tuple21*)_tmp2B4->hd)->f1;_tmp2B9=((struct _tuple21*)_tmp2B4->hd)->f2;_tmp2B8=_tmp2B4->tl;_LL1A2:
# 1365
 return({struct Cyc_List_List*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));({struct _tuple21*_tmp593=({struct _tuple21*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->f1=_tmp2BA;({struct Cyc_List_List*_tmp592=({struct Cyc_List_List*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->hd=dsc;_tmp2B7->tl=_tmp2B9;_tmp2B7;});_tmp2B6->f2=_tmp592;});_tmp2B6;});_tmp2B5->hd=_tmp593;});_tmp2B5->tl=_tmp2B8;_tmp2B5;});}_LL19E:;}
# 1373
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp2BB=ctxt;struct Cyc_Tcpat_Con_s*_tmp2C2;struct Cyc_List_List*_tmp2C1;struct Cyc_List_List*_tmp2C0;if(_tmp2BB == 0){_LL1A4: _LL1A5:
({void*_tmp2BC=0;({struct _dyneither_ptr _tmp594=({const char*_tmp2BD="norm_context: empty context";_tag_dyneither(_tmp2BD,sizeof(char),28);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp594,_tag_dyneither(_tmp2BC,sizeof(void*),0));});});}else{_LL1A6: _tmp2C2=((struct _tuple21*)_tmp2BB->hd)->f1;_tmp2C1=((struct _tuple21*)_tmp2BB->hd)->f2;_tmp2C0=_tmp2BB->tl;_LL1A7:
# 1377
 return({struct Cyc_List_List*_tmp597=_tmp2C0;Cyc_Tcpat_augment(_tmp597,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp596=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp2BF;_tmp2BF.tag=0;_tmp2BF.f1=_tmp2C2;({struct Cyc_List_List*_tmp595=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp2C1);_tmp2BF.f2=_tmp595;});_tmp2BF;});_tmp2BE[0]=_tmp596;});_tmp2BE;}));});}_LL1A3:;}
# 1386
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1388
struct _tuple1 _tmp2C3=({struct _tuple1 _tmp2D0;_tmp2D0.f1=ctxt;_tmp2D0.f2=work;_tmp2D0;});struct _tuple1 _tmp2C4=_tmp2C3;struct Cyc_Tcpat_Con_s*_tmp2CF;struct Cyc_List_List*_tmp2CE;struct Cyc_List_List*_tmp2CD;struct Cyc_List_List*_tmp2CC;struct Cyc_List_List*_tmp2CB;if(_tmp2C4.f1 == 0){if(_tmp2C4.f2 == 0){_LL1A9: _LL1AA:
 return dsc;}else{_LL1AB: _LL1AC:
 goto _LL1AE;}}else{if(_tmp2C4.f2 == 0){_LL1AD: _LL1AE:
({void*_tmp2C5=0;({struct _dyneither_ptr _tmp598=({const char*_tmp2C6="build_desc: ctxt and work don't match";_tag_dyneither(_tmp2C6,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp598,_tag_dyneither(_tmp2C5,sizeof(void*),0));});});}else{_LL1AF: _tmp2CF=((struct _tuple21*)(_tmp2C4.f1)->hd)->f1;_tmp2CE=((struct _tuple21*)(_tmp2C4.f1)->hd)->f2;_tmp2CD=(_tmp2C4.f1)->tl;_tmp2CC=((struct _tuple20*)(_tmp2C4.f2)->hd)->f3;_tmp2CB=(_tmp2C4.f2)->tl;_LL1B0: {
# 1393
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2C7=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2C8=_cycalloc(sizeof(*_tmp2C8));({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp59B=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp2C9;_tmp2C9.tag=0;_tmp2C9.f1=_tmp2CF;({struct Cyc_List_List*_tmp59A=({struct Cyc_List_List*_tmp599=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp2CE);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp599,({struct Cyc_List_List*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA->hd=dsc;_tmp2CA->tl=_tmp2CC;_tmp2CA;}));});_tmp2C9.f2=_tmp59A;});_tmp2C9;});_tmp2C8[0]=_tmp59B;});_tmp2C8;});
return Cyc_Tcpat_build_desc(_tmp2CD,(void*)_tmp2C7,_tmp2CB);}}}_LL1A8:;}
# 1398
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1405
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp2D1=allmrules;void*_tmp2D6;struct Cyc_Tcpat_Rhs*_tmp2D5;struct Cyc_List_List*_tmp2D4;if(_tmp2D1 == 0){_LL1B2: _LL1B3:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp59C=({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp2D3;_tmp2D3.tag=0;_tmp2D3.f1=dsc;_tmp2D3;});_tmp2D2[0]=_tmp59C;});_tmp2D2;});}else{_LL1B4: _tmp2D6=((struct _tuple22*)_tmp2D1->hd)->f1;_tmp2D5=((struct _tuple22*)_tmp2D1->hd)->f2;_tmp2D4=_tmp2D1->tl;_LL1B5:
# 1409
 return Cyc_Tcpat_match(_tmp2D6,0,dsc,0,0,_tmp2D5,_tmp2D4);}_LL1B1:;}
# 1414
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp59F=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp59E=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2D8;_tmp2D8.tag=1;({struct Cyc_Set_Set*_tmp59D=Cyc_Tcpat_empty_con_set();_tmp2D8.f1=_tmp59D;});_tmp2D8;});_tmp2D7[0]=_tmp59E;});_tmp2D7;});Cyc_Tcpat_or_match(_tmp59F,allmrules);});}
# 1421
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1424
struct Cyc_List_List*_tmp2D9=work;struct Cyc_List_List*_tmp2F1;struct Cyc_List_List*_tmp2F0;struct Cyc_List_List*_tmp2EF;struct Cyc_List_List*_tmp2EE;struct Cyc_List_List*_tmp2ED;if(_tmp2D9 == 0){_LL1B7: _LL1B8:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));({struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp5A0=({struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp2DB;_tmp2DB.tag=1;_tmp2DB.f1=right_hand_side;_tmp2DB;});_tmp2DA[0]=_tmp5A0;});_tmp2DA;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2D9)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2D9)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2D9)->hd)->f3 == 0){_LL1B9: _tmp2ED=_tmp2D9->tl;_LL1BA:
# 1427
 return({struct Cyc_List_List*_tmp5A3=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp5A2=_tmp2ED;struct Cyc_Tcpat_Rhs*_tmp5A1=right_hand_side;Cyc_Tcpat_and_match(_tmp5A3,_tmp5A2,_tmp5A1,rules);});}else{goto _LL1BB;}}else{goto _LL1BB;}}else{_LL1BB: _tmp2F1=((struct _tuple20*)_tmp2D9->hd)->f1;_tmp2F0=((struct _tuple20*)_tmp2D9->hd)->f2;_tmp2EF=((struct _tuple20*)_tmp2D9->hd)->f3;_tmp2EE=_tmp2D9->tl;_LL1BC:
# 1429
 if((_tmp2F1 == 0  || _tmp2F0 == 0) || _tmp2EF == 0)
({void*_tmp2DC=0;({struct _dyneither_ptr _tmp5A4=({const char*_tmp2DD="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp2DD,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5A4,_tag_dyneither(_tmp2DC,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp2DE=_tmp2F1;struct Cyc_List_List*_tmp2DF=_tmp2DE;void*_tmp2EC;struct Cyc_List_List*_tmp2EB;_LL1BE: _tmp2EC=(void*)_tmp2DF->hd;_tmp2EB=_tmp2DF->tl;_LL1BF:;{
struct Cyc_List_List*_tmp2E0=_tmp2F0;struct Cyc_List_List*_tmp2E1=_tmp2E0;struct Cyc_List_List*_tmp2EA;struct Cyc_List_List*_tmp2E9;_LL1C1: _tmp2EA=(struct Cyc_List_List*)_tmp2E1->hd;_tmp2E9=_tmp2E1->tl;_LL1C2:;{
struct Cyc_List_List*_tmp2E2=_tmp2EF;struct Cyc_List_List*_tmp2E3=_tmp2E2;void*_tmp2E8;struct Cyc_List_List*_tmp2E7;_LL1C4: _tmp2E8=(void*)_tmp2E3->hd;_tmp2E7=_tmp2E3->tl;_LL1C5:;{
struct _tuple20*_tmp2E4=({struct _tuple20*_tmp2E6=_cycalloc(sizeof(*_tmp2E6));_tmp2E6->f1=_tmp2EB;_tmp2E6->f2=_tmp2E9;_tmp2E6->f3=_tmp2E7;_tmp2E6;});
return({void*_tmp5AA=_tmp2EC;struct Cyc_List_List*_tmp5A9=_tmp2EA;void*_tmp5A8=_tmp2E8;struct Cyc_List_List*_tmp5A7=ctx;struct Cyc_List_List*_tmp5A6=({struct Cyc_List_List*_tmp2E5=_cycalloc(sizeof(*_tmp2E5));_tmp2E5->hd=_tmp2E4;_tmp2E5->tl=_tmp2EE;_tmp2E5;});struct Cyc_Tcpat_Rhs*_tmp5A5=right_hand_side;Cyc_Tcpat_match(_tmp5AA,_tmp5A9,_tmp5A8,_tmp5A7,_tmp5A6,_tmp5A5,rules);});};};};};}}_LL1B6:;}
# 1440
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp2F2=dsc;struct Cyc_List_List*_tmp2F8;struct Cyc_Set_Set*_tmp2F7;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F2)->tag == 1){_LL1C7: _tmp2F7=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F2)->f1;_LL1C8: {
# 1446
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2F5=_cycalloc(sizeof(*_tmp2F5));({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5AC=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2F6;_tmp2F6.tag=1;({struct Cyc_Set_Set*_tmp5AB=Cyc_Tcpat_empty_con_set();_tmp2F6.f1=_tmp5AB;});_tmp2F6;});_tmp2F5[0]=_tmp5AC;});_tmp2F5;});
struct Cyc_List_List*_tmp2F3=0;
{int i=0;for(0;i < pcon->arity;++ i){
({struct Cyc_List_List*_tmp5AD=({struct Cyc_List_List*_tmp2F4=_cycalloc(sizeof(*_tmp2F4));_tmp2F4->hd=any;_tmp2F4->tl=_tmp2F3;_tmp2F4;});_tmp2F3=_tmp5AD;});}}
return _tmp2F3;}}else{_LL1C9: _tmp2F8=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F2)->f2;_LL1CA:
 return _tmp2F8;}_LL1C6:;}
# 1455
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp2F9=pw;struct Cyc_Absyn_Pat*_tmp31A;if((_tmp2F9.where_clause).tag == 2){_LL1CC: _LL1CD:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL1CE: _tmp31A=(_tmp2F9.pattern).val;_LL1CF: {
# 1459
void*_tmp2FA=_tmp31A->r;void*_tmp2FB=_tmp2FA;union Cyc_Absyn_AggrInfoU _tmp319;struct Cyc_List_List*_tmp318;struct Cyc_Absyn_Datatypedecl*_tmp317;struct Cyc_Absyn_Datatypefield*_tmp316;switch(*((int*)_tmp2FB)){case 6: _LL1D1: _LL1D2:
# 1461
 if(i != 0)
({void*_tmp2FC=0;({struct _dyneither_ptr _tmp5AF=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2FF;_tmp2FF.tag=1;_tmp2FF.f1=(unsigned long)i;({void*_tmp2FD[1]={& _tmp2FF};({struct _dyneither_ptr _tmp5AE=({const char*_tmp2FE="get_access on pointer pattern with offset %d\n";_tag_dyneither(_tmp2FE,sizeof(char),46);});Cyc_aprintf(_tmp5AE,_tag_dyneither(_tmp2FD,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5AF,_tag_dyneither(_tmp2FC,sizeof(void*),0));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5: _LL1D3: _LL1D4:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp300=_cycalloc_atomic(sizeof(*_tmp300));({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct _tmp5B0=({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct _tmp301;_tmp301.tag=2;_tmp301.f1=(unsigned int)i;_tmp301;});_tmp300[0]=_tmp5B0;});_tmp300;});case 8: _LL1D5: _tmp317=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2FB)->f1;_tmp316=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2FB)->f2;_LL1D6:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp302=_cycalloc(sizeof(*_tmp302));({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct _tmp5B1=({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct _tmp303;_tmp303.tag=3;_tmp303.f1=_tmp317;_tmp303.f2=_tmp316;_tmp303.f3=(unsigned int)i;_tmp303;});_tmp302[0]=_tmp5B1;});_tmp302;});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2FB)->f1 != 0){_LL1D7: _tmp319=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2FB)->f1)->aggr_info;_tmp318=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2FB)->f3;_LL1D8: {
# 1467
struct Cyc_Absyn_Aggrdecl*_tmp304=Cyc_Absyn_get_known_aggrdecl(_tmp319);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->tagged){
struct Cyc_List_List*_tmp305=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp318))->hd)).f1;struct Cyc_List_List*_tmp306=_tmp305;struct _dyneither_ptr*_tmp30D;if(_tmp306 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp306)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp306)->tl == 0){_LL1DC: _tmp30D=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp306->hd)->f1;_LL1DD:
# 1471
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp307=_cycalloc(sizeof(*_tmp307));({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp5B2=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp308;_tmp308.tag=4;_tmp308.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->tagged;_tmp308.f2=_tmp30D;_tmp308;});_tmp307[0]=_tmp5B2;});_tmp307;});}else{goto _LL1DE;}}else{goto _LL1DE;}}else{_LL1DE: _LL1DF:
({void*_tmp309=0;({struct _dyneither_ptr _tmp5B5=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp30C;_tmp30C.tag=0;({struct _dyneither_ptr _tmp5B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp31A));_tmp30C.f1=_tmp5B3;});({void*_tmp30A[1]={& _tmp30C};({struct _dyneither_ptr _tmp5B4=({const char*_tmp30B="get_access on bad aggr pattern: %s";_tag_dyneither(_tmp30B,sizeof(char),35);});Cyc_aprintf(_tmp5B4,_tag_dyneither(_tmp30A,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5B5,_tag_dyneither(_tmp309,sizeof(void*),0));});});}_LL1DB:;}{
# 1475
struct Cyc_List_List*_tmp30E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->fields;
int _tmp30F=i;
for(0;i != 0;-- i){_tmp30E=((struct Cyc_List_List*)_check_null(_tmp30E))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp310=_cycalloc(sizeof(*_tmp310));({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp5B6=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp311;_tmp311.tag=4;_tmp311.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp304->impl))->tagged;_tmp311.f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp30E))->hd)->name;_tmp311;});_tmp310[0]=_tmp5B6;});_tmp310;});};}}else{goto _LL1D9;}default: _LL1D9: _LL1DA:
({void*_tmp312=0;({struct _dyneither_ptr _tmp5B9=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp315;_tmp315.tag=0;({struct _dyneither_ptr _tmp5B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp31A));_tmp315.f1=_tmp5B7;});({void*_tmp313[1]={& _tmp315};({struct _dyneither_ptr _tmp5B8=({const char*_tmp314="get_access on bad pattern: %s";_tag_dyneither(_tmp314,sizeof(char),30);});Cyc_aprintf(_tmp5B8,_tag_dyneither(_tmp313,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5B9,_tag_dyneither(_tmp312,sizeof(void*),0));});});}_LL1D0:;}}_LL1CB:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1485
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp31B=env;struct Cyc_List_List*_tmp31F;struct Cyc_Tcpat_Con_s*_tmp31E;_LL1E1: _tmp31F=_tmp31B->f1;_tmp31E=_tmp31B->f2;_LL1E2:;{
void*acc=Cyc_Tcpat_get_access(_tmp31E->orig_pat,i);
return({struct Cyc_List_List*_tmp31C=_cycalloc(sizeof(*_tmp31C));({struct Cyc_Tcpat_PathNode*_tmp5BA=({struct Cyc_Tcpat_PathNode*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->orig_pat=_tmp31E->orig_pat;_tmp31D->access=acc;_tmp31D;});_tmp31C->hd=_tmp5BA;});_tmp31C->tl=_tmp31F;_tmp31C;});};}
# 1490
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp320=({struct _tuple23 _tmp321;_tmp321.f1=obj;_tmp321.f2=pcon;_tmp321;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp320);}
# 1498
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1502
void*_tmp322=p;struct Cyc_Tcpat_Con_s*_tmp330;struct Cyc_List_List*_tmp32F;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp322)->tag == 0){_LL1E4: _LL1E5:
# 1504
 return({struct Cyc_List_List*_tmp5BD=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp5BC=work;struct Cyc_Tcpat_Rhs*_tmp5BB=right_hand_side;Cyc_Tcpat_and_match(_tmp5BD,_tmp5BC,_tmp5BB,rules);});}else{_LL1E6: _tmp330=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp322)->f1;_tmp32F=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp322)->f2;_LL1E7: {
# 1506
enum Cyc_Tcpat_Answer _tmp323=Cyc_Tcpat_static_match(_tmp330,dsc);enum Cyc_Tcpat_Answer _tmp324=_tmp323;switch(_tmp324){case Cyc_Tcpat_Yes: _LL1E9: _LL1EA: {
# 1508
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp327=_cycalloc(sizeof(*_tmp327));({struct _tuple21*_tmp5BE=({struct _tuple21*_tmp328=_cycalloc(sizeof(*_tmp328));_tmp328->f1=_tmp330;_tmp328->f2=0;_tmp328;});_tmp327->hd=_tmp5BE;});_tmp327->tl=ctx;_tmp327;});
struct _tuple20*work_frame=({struct _tuple20*_tmp326=_cycalloc(sizeof(*_tmp326));_tmp326->f1=_tmp32F;({struct Cyc_List_List*_tmp5BF=Cyc_Tcpat_getoargs(_tmp330,obj);_tmp326->f2=_tmp5BF;});({struct Cyc_List_List*_tmp5C0=
Cyc_Tcpat_getdargs(_tmp330,dsc);_tmp326->f3=_tmp5C0;});_tmp326;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp325=_cycalloc(sizeof(*_tmp325));_tmp325->hd=work_frame;_tmp325->tl=work;_tmp325;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL1EB: _LL1EC:
# 1514
 return({void*_tmp5C1=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp5C1,rules);});default: _LL1ED: _LL1EE: {
# 1516
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp32D=_cycalloc(sizeof(*_tmp32D));({struct _tuple21*_tmp5C2=({struct _tuple21*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->f1=_tmp330;_tmp32E->f2=0;_tmp32E;});_tmp32D->hd=_tmp5C2;});_tmp32D->tl=ctx;_tmp32D;});
struct _tuple20*work_frame=({struct _tuple20*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->f1=_tmp32F;({struct Cyc_List_List*_tmp5C3=Cyc_Tcpat_getoargs(_tmp330,obj);_tmp32C->f2=_tmp5C3;});({struct Cyc_List_List*_tmp5C4=
Cyc_Tcpat_getdargs(_tmp330,dsc);_tmp32C->f3=_tmp5C4;});_tmp32C;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp32B=_cycalloc(sizeof(*_tmp32B));_tmp32B->hd=work_frame;_tmp32B->tl=work;_tmp32B;});
void*_tmp329=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp32A=({void*_tmp5C7=({struct Cyc_List_List*_tmp5C6=ctx;void*_tmp5C5=Cyc_Tcpat_add_neg(dsc,_tmp330);Cyc_Tcpat_build_desc(_tmp5C6,_tmp5C5,work);});Cyc_Tcpat_or_match(_tmp5C7,rules);});
# 1523
return Cyc_Tcpat_ifeq(obj,_tmp330,_tmp329,_tmp32A);}}_LL1E8:;}}_LL1E3:;}
# 1533
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1538
void*_tmp331=d;struct Cyc_List_List*_tmp338;void*_tmp337;struct Cyc_Tcpat_Rhs*_tmp336;void*_tmp335;switch(*((int*)_tmp331)){case 0: _LL1F0: _tmp335=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp331)->f1;_LL1F1:
# 1540
 if(!(*exhaust_done)){not_exhaust(env1,_tmp335);*exhaust_done=1;}
goto _LL1EF;case 1: _LL1F2: _tmp336=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp331)->f1;_LL1F3:
 rhs_appears(env2,_tmp336);goto _LL1EF;default: _LL1F4: _tmp338=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp331)->f2;_tmp337=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp331)->f3;_LL1F5:
# 1544
 for(0;_tmp338 != 0;_tmp338=_tmp338->tl){
struct _tuple0 _tmp332=*((struct _tuple0*)_tmp338->hd);struct _tuple0 _tmp333=_tmp332;void*_tmp334;_LL1F7: _tmp334=_tmp333.f2;_LL1F8:;
Cyc_Tcpat_check_exhaust_overlap(_tmp334,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1549
Cyc_Tcpat_check_exhaust_overlap(_tmp337,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1551
goto _LL1EF;}_LL1EF:;}
# 1559
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1561
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341->used=0;_tmp341->pat_loc=(swc->pattern)->loc;_tmp341->rhs=swc->body;_tmp341;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp339=({union Cyc_Tcpat_PatOrWhere _tmp33B;(_tmp33B.where_clause).val=swc->where_clause;(_tmp33B.where_clause).tag=2;_tmp33B;});
({void*_tmp5CA=({struct Cyc_List_List*_tmp5C9=({void*_tmp33A[2];({void*_tmp5C8=Cyc_Tcpat_int_pat(*ctr,_tmp339);_tmp33A[1]=_tmp5C8;});_tmp33A[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33A,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_tmp5C9,_tmp339);});sp=_tmp5CA;});
*ctr=*ctr + 1;}else{
# 1569
({void*_tmp5CE=({struct Cyc_List_List*_tmp5CD=({void*_tmp33C[2];({void*_tmp5CC=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp33D=_cycalloc_atomic(sizeof(*_tmp33D));({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp5CB=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp33E;_tmp33E.tag=0;_tmp33E;});_tmp33D[0]=_tmp5CB;});_tmp33D;});_tmp33C[1]=_tmp5CC;});_tmp33C[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33C,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_tmp5CD,({union Cyc_Tcpat_PatOrWhere _tmp33F;(_tmp33F.where_clause).val=0;(_tmp33F.where_clause).tag=2;_tmp33F;}));});sp=_tmp5CE;});}
return({struct _tuple22*_tmp340=_cycalloc(sizeof(*_tmp340));_tmp340->f1=sp;_tmp340->f2=rhs;_tmp340;});}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1575
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1577
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp343=d;struct Cyc_Set_Set*_tmp3A4;struct Cyc_Tcpat_Con_s*_tmp3A3;struct Cyc_List_List*_tmp3A2;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp343)->tag == 0){_LL1FA: _tmp3A3=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp343)->f1;_tmp3A2=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp343)->f2;_LL1FB: {
# 1581
union Cyc_Tcpat_Name_value _tmp344=_tmp3A3->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp345=_tmp3A3->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp346=_tmp345;struct Cyc_Absyn_Pat*_tmp347;if((_tmp346.where_clause).tag == 2){_LL1FF: _LL200:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp3A2))->hd);}else{_LL201: _tmp347=(_tmp346.pattern).val;_LL202:
 p=_tmp347;goto _LL1FE;}_LL1FE:;}{
# 1587
void*_tmp348=p->r;void*_tmp349=_tmp348;struct Cyc_Absyn_Exp*_tmp37E;struct Cyc_Absyn_Enumfield*_tmp37D;struct Cyc_Absyn_Enumfield*_tmp37C;struct _dyneither_ptr _tmp37B;int _tmp37A;char _tmp379;int _tmp378;struct Cyc_Absyn_Datatypefield*_tmp377;struct Cyc_Absyn_Aggrdecl*_tmp376;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Tvar*_tmp374;struct Cyc_Absyn_Vardecl*_tmp373;struct Cyc_Absyn_Vardecl*_tmp372;struct Cyc_Absyn_Vardecl*_tmp371;switch(*((int*)_tmp349)){case 0: _LL204: _LL205:
 return({const char*_tmp34A="_";_tag_dyneither(_tmp34A,sizeof(char),2);});case 1: _LL206: _tmp371=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp349)->f1;_LL207:
 return Cyc_Absynpp_qvar2string(_tmp371->name);case 3: _LL208: _tmp372=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp349)->f1;_LL209:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp34D;_tmp34D.tag=0;({struct _dyneither_ptr _tmp5CF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp372->name));_tmp34D.f1=_tmp5CF;});({void*_tmp34B[1]={& _tmp34D};({struct _dyneither_ptr _tmp5D0=({const char*_tmp34C="*%s";_tag_dyneither(_tmp34C,sizeof(char),4);});Cyc_aprintf(_tmp5D0,_tag_dyneither(_tmp34B,sizeof(void*),1));});});});case 4: _LL20A: _tmp374=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp349)->f1;_tmp373=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp349)->f2;_LL20B:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp351;_tmp351.tag=0;_tmp351.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp374->name);({struct Cyc_String_pa_PrintArg_struct _tmp350;_tmp350.tag=0;({struct _dyneither_ptr _tmp5D1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp373->name));_tmp350.f1=_tmp5D1;});({void*_tmp34E[2]={& _tmp350,& _tmp351};({struct _dyneither_ptr _tmp5D2=({const char*_tmp34F="%s<`%s>";_tag_dyneither(_tmp34F,sizeof(char),8);});Cyc_aprintf(_tmp5D2,_tag_dyneither(_tmp34E,sizeof(void*),2));});});});});case 5: _LL20C: _LL20D:
# 1593
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp354;_tmp354.tag=0;({struct _dyneither_ptr _tmp5D3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3A2));_tmp354.f1=_tmp5D3;});({void*_tmp352[1]={& _tmp354};({struct _dyneither_ptr _tmp5D4=({const char*_tmp353="$(%s)";_tag_dyneither(_tmp353,sizeof(char),6);});Cyc_aprintf(_tmp5D4,_tag_dyneither(_tmp352,sizeof(void*),1));});});});case 6: _LL20E: _LL20F:
# 1595
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp357;_tmp357.tag=0;({struct _dyneither_ptr _tmp5D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3A2));_tmp357.f1=_tmp5D5;});({void*_tmp355[1]={& _tmp357};({struct _dyneither_ptr _tmp5D6=({const char*_tmp356="&%s";_tag_dyneither(_tmp356,sizeof(char),4);});Cyc_aprintf(_tmp5D6,_tag_dyneither(_tmp355,sizeof(void*),1));});});});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp349)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp349)->f1)->aggr_info).KnownAggr).tag == 2){_LL210: _tmp376=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp349)->f1)->aggr_info).KnownAggr).val;_tmp375=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp349)->f3;_LL211:
# 1597
 if(_tmp376->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp358=_tmp375;struct _dyneither_ptr*_tmp35E;if(_tmp358 != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp358)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp358)->hd)->f1)->hd)->tag == 1){_LL225: _tmp35E=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp358->hd)->f1)->hd)->f1;_LL226:
# 1600
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35D;_tmp35D.tag=0;({struct _dyneither_ptr _tmp5D7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(_tmp3A2));_tmp35D.f1=_tmp5D7;});({struct Cyc_String_pa_PrintArg_struct _tmp35C;_tmp35C.tag=0;_tmp35C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp35E);({struct Cyc_String_pa_PrintArg_struct _tmp35B;_tmp35B.tag=0;({struct _dyneither_ptr _tmp5D8=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1600
Cyc_Absynpp_qvar2string(_tmp376->name));_tmp35B.f1=_tmp5D8;});({void*_tmp359[3]={& _tmp35B,& _tmp35C,& _tmp35D};({struct _dyneither_ptr _tmp5D9=({const char*_tmp35A="%s{.%s = %s}";_tag_dyneither(_tmp35A,sizeof(char),13);});Cyc_aprintf(_tmp5D9,_tag_dyneither(_tmp359,sizeof(void*),3));});});});});});}else{goto _LL227;}}else{goto _LL227;}}else{_LL227: _LL228:
# 1602
 goto _LL224;}_LL224:;}
# 1605
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp362;_tmp362.tag=0;({struct _dyneither_ptr _tmp5DA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3A2));_tmp362.f1=_tmp5DA;});({struct Cyc_String_pa_PrintArg_struct _tmp361;_tmp361.tag=0;({struct _dyneither_ptr _tmp5DB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp376->name));_tmp361.f1=_tmp5DB;});({void*_tmp35F[2]={& _tmp361,& _tmp362};({struct _dyneither_ptr _tmp5DC=({const char*_tmp360="%s{%s}";_tag_dyneither(_tmp360,sizeof(char),7);});Cyc_aprintf(_tmp5DC,_tag_dyneither(_tmp35F,sizeof(void*),2));});});});});}else{goto _LL222;}}else{goto _LL222;}case 8: _LL212: _tmp377=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp349)->f2;_LL213:
# 1607
 if(_tmp3A2 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp365;_tmp365.tag=0;({struct _dyneither_ptr _tmp5DD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp377->name));_tmp365.f1=_tmp5DD;});({void*_tmp363[1]={& _tmp365};({struct _dyneither_ptr _tmp5DE=({const char*_tmp364="%s";_tag_dyneither(_tmp364,sizeof(char),3);});Cyc_aprintf(_tmp5DE,_tag_dyneither(_tmp363,sizeof(void*),1));});});});else{
# 1610
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp369;_tmp369.tag=0;({struct _dyneither_ptr _tmp5DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3A2));_tmp369.f1=_tmp5DF;});({struct Cyc_String_pa_PrintArg_struct _tmp368;_tmp368.tag=0;({struct _dyneither_ptr _tmp5E0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp377->name));_tmp368.f1=_tmp5E0;});({void*_tmp366[2]={& _tmp368,& _tmp369};({struct _dyneither_ptr _tmp5E1=({const char*_tmp367="%s(%s)";_tag_dyneither(_tmp367,sizeof(char),7);});Cyc_aprintf(_tmp5E1,_tag_dyneither(_tmp366,sizeof(void*),2));});});});});}case 9: _LL214: _LL215:
 return({const char*_tmp36A="NULL";_tag_dyneither(_tmp36A,sizeof(char),5);});case 10: _LL216: _tmp378=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp349)->f2;_LL217:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp36D;_tmp36D.tag=1;_tmp36D.f1=(unsigned long)_tmp378;({void*_tmp36B[1]={& _tmp36D};({struct _dyneither_ptr _tmp5E2=({const char*_tmp36C="%d";_tag_dyneither(_tmp36C,sizeof(char),3);});Cyc_aprintf(_tmp5E2,_tag_dyneither(_tmp36B,sizeof(void*),1));});});});case 11: _LL218: _tmp379=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp349)->f1;_LL219:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp370;_tmp370.tag=1;_tmp370.f1=(unsigned long)((int)_tmp379);({void*_tmp36E[1]={& _tmp370};({struct _dyneither_ptr _tmp5E3=({const char*_tmp36F="%d";_tag_dyneither(_tmp36F,sizeof(char),3);});Cyc_aprintf(_tmp5E3,_tag_dyneither(_tmp36E,sizeof(void*),1));});});});case 12: _LL21A: _tmp37B=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp349)->f1;_tmp37A=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp349)->f2;_LL21B:
 return _tmp37B;case 13: _LL21C: _tmp37C=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp349)->f2;_LL21D:
 _tmp37D=_tmp37C;goto _LL21F;case 14: _LL21E: _tmp37D=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp349)->f2;_LL21F:
 return Cyc_Absynpp_qvar2string(_tmp37D->name);case 17: _LL220: _tmp37E=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp349)->f1;_LL221:
 return Cyc_Absynpp_exp2string(_tmp37E);default: _LL222: _LL223:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL203:;};}}else{_LL1FC: _tmp3A4=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp343)->f1;_LL1FD:
# 1621
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp3A4))return({const char*_tmp37F="_";_tag_dyneither(_tmp37F,sizeof(char),2);});{
struct Cyc_Tcpat_Con_s*_tmp380=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp3A4);
# 1625
if((int)(((_tmp380->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp381=({union Cyc_Tcpat_PatOrWhere _tmp3A1=_tmp380->orig_pat;if((_tmp3A1.pattern).tag != 1)_throw_match();(_tmp3A1.pattern).val;});
void*_tmp382=Cyc_Tcutil_compress((void*)_check_null(_tmp381->topt));void*_tmp383=_tmp382;struct Cyc_Absyn_Aggrdecl*_tmp3A0;struct Cyc_Absyn_Datatypedecl*_tmp39F;struct Cyc_Absyn_PtrAtts _tmp39E;switch(*((int*)_tmp383)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp383)->f2 == Cyc_Absyn_Char_sz){_LL22A: _LL22B:
# 1630
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp384=Cyc_Tcpat_char_con((char)i,_tmp381);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3A4,_tmp384))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp387;_tmp387.tag=1;_tmp387.f1=(unsigned long)i;({void*_tmp385[1]={& _tmp387};({struct _dyneither_ptr _tmp5E4=({const char*_tmp386="%d";_tag_dyneither(_tmp386,sizeof(char),3);});Cyc_aprintf(_tmp5E4,_tag_dyneither(_tmp385,sizeof(void*),1));});});});}}
# 1635
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL22C: _LL22D:
# 1638
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp388=Cyc_Tcpat_int_con((int)i,_tmp380->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3A4,_tmp388))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp38B;_tmp38B.tag=1;_tmp38B.f1=(unsigned long)((int)i);({void*_tmp389[1]={& _tmp38B};({struct _dyneither_ptr _tmp5E5=({const char*_tmp38A="%d";_tag_dyneither(_tmp38A,sizeof(char),3);});Cyc_aprintf(_tmp5E5,_tag_dyneither(_tmp389,sizeof(void*),1));});});});}}
# 1643
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 5: _LL22E: _tmp39E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp383)->f1).ptr_atts;_LL22F: {
# 1645
union Cyc_Absyn_Constraint*_tmp38C=_tmp39E.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp38C);
if(is_nullable){
if(!({struct Cyc_Set_Set*_tmp5E6=_tmp3A4;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5E6,Cyc_Tcpat_null_con(_tmp381));}))
return({const char*_tmp38D="NULL";_tag_dyneither(_tmp38D,sizeof(char),5);});}
# 1651
return({const char*_tmp38E="&_";_tag_dyneither(_tmp38E,sizeof(char),3);});}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp383)->f1).datatype_info).KnownDatatype).tag == 2){_LL230: _tmp39F=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp383)->f1).datatype_info).KnownDatatype).val;_LL231:
# 1653
 if(_tmp39F->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp38F=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp39F->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38F);
for(0;(unsigned int)_tmp38F;_tmp38F=_tmp38F->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp38F->hd)->name).f2;
struct Cyc_List_List*_tmp390=((struct Cyc_Absyn_Datatypefield*)_tmp38F->hd)->typs;
if(!({struct Cyc_Set_Set*_tmp5E8=_tmp3A4;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5E8,({struct Cyc_Tcpat_Con_s*_tmp391=_cycalloc(sizeof(*_tmp391));({union Cyc_Tcpat_Name_value _tmp5E7=Cyc_Tcpat_Name_v(n);_tmp391->name=_tmp5E7;});_tmp391->arity=0;_tmp391->span=0;_tmp391->orig_pat=_tmp380->orig_pat;_tmp391;}));})){
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp390)== 0)
return n;else{
# 1663
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp394;_tmp394.tag=0;_tmp394.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({void*_tmp392[1]={& _tmp394};({struct _dyneither_ptr _tmp5E9=({const char*_tmp393="%s(...)";_tag_dyneither(_tmp393,sizeof(char),8);});Cyc_aprintf(_tmp5E9,_tag_dyneither(_tmp392,sizeof(void*),1));});});});}}}
# 1666
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL234;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp383)->f1).aggr_info).KnownAggr).tag == 2){_LL232: _tmp3A0=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp383)->f1).aggr_info).KnownAggr).val;if(_tmp3A0->kind == Cyc_Absyn_UnionA){_LL233: {
# 1668
struct Cyc_List_List*_tmp395=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A0->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp395);
struct _tuple2*_tmp396=_tmp3A0->name;struct _tuple2*_tmp397=_tmp396;struct _dyneither_ptr _tmp39D;_LL237: _tmp39D=*_tmp397->f2;_LL238:;
for(0;(unsigned int)_tmp395;_tmp395=_tmp395->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp395->hd)->name;
if(!({struct Cyc_Set_Set*_tmp5EB=_tmp3A4;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5EB,({struct Cyc_Tcpat_Con_s*_tmp398=_cycalloc(sizeof(*_tmp398));({union Cyc_Tcpat_Name_value _tmp5EA=Cyc_Tcpat_Name_v(n);_tmp398->name=_tmp5EA;});_tmp398->arity=0;_tmp398->span=0;_tmp398->orig_pat=_tmp380->orig_pat;_tmp398;}));}))
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp39C;_tmp39C.tag=0;_tmp39C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({struct Cyc_String_pa_PrintArg_struct _tmp39B;_tmp39B.tag=0;_tmp39B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp39D);({void*_tmp399[2]={& _tmp39B,& _tmp39C};({struct _dyneither_ptr _tmp5EC=({const char*_tmp39A="%s{.%s = _}";_tag_dyneither(_tmp39A,sizeof(char),12);});Cyc_aprintf(_tmp5EC,_tag_dyneither(_tmp399,sizeof(void*),2));});});});});}
# 1676
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL234;}}else{goto _LL234;}default: _LL234: _LL235:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL229:;};};}_LL1F9:;}
# 1681
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dyneither_ptr*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));({struct _dyneither_ptr _tmp5ED=Cyc_Tcpat_desc2string(d);_tmp3A5[0]=_tmp5ED;});_tmp3A5;});}
# 1684
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3A6=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dyneither_ptr*comma=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);
{struct Cyc_List_List*_tmp3A7=_tmp3A6;for(0;_tmp3A7 != 0;_tmp3A7=((struct Cyc_List_List*)_check_null(_tmp3A7))->tl){
if(_tmp3A7->tl != 0){
({struct Cyc_List_List*_tmp5EE=({struct Cyc_List_List*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->hd=comma;_tmp3A8->tl=_tmp3A7->tl;_tmp3A8;});_tmp3A7->tl=_tmp5EE;});
_tmp3A7=_tmp3A7->tl;}}}
# 1693
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp3A6);}
# 1696
static void Cyc_Tcpat_not_exhaust_err(unsigned int loc,void*desc){
struct _handler_cons _tmp3AA;_push_handler(& _tmp3AA);{int _tmp3AC=0;if(setjmp(_tmp3AA.handler))_tmp3AC=1;if(!_tmp3AC){
{struct _dyneither_ptr _tmp3AD=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp3B0;_tmp3B0.tag=0;_tmp3B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3AD);({void*_tmp3AE[1]={& _tmp3B0};({unsigned int _tmp5F0=loc;struct _dyneither_ptr _tmp5EF=({const char*_tmp3AF="patterns may not be exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp3AF,sizeof(char),53);});Cyc_Tcutil_terr(_tmp5F0,_tmp5EF,_tag_dyneither(_tmp3AE,sizeof(void*),1));});});});}
# 1698
;_pop_handler();}else{void*_tmp3AB=(void*)_exn_thrown;void*_tmp3B1=_tmp3AB;void*_tmp3B4;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3B1)->tag == Cyc_Tcpat_Desc2string){_LL23A: _LL23B:
# 1702
({void*_tmp3B2=0;({unsigned int _tmp5F2=loc;struct _dyneither_ptr _tmp5F1=({const char*_tmp3B3="patterns may not be exhaustive.";_tag_dyneither(_tmp3B3,sizeof(char),32);});Cyc_Tcutil_terr(_tmp5F2,_tmp5F1,_tag_dyneither(_tmp3B2,sizeof(void*),0));});});
goto _LL239;}else{_LL23C: _tmp3B4=_tmp3B1;_LL23D:(int)_rethrow(_tmp3B4);}_LL239:;}};}
# 1706
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1710
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1716
int _tmp3B5=0;
int*_tmp3B6=& _tmp3B5;
struct Cyc_List_List*_tmp3B7=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp3B6,swcs);
void*_tmp3B8=Cyc_Tcpat_match_compile(_tmp3B7);
*dopt=_tmp3B8;{
# 1722
int _tmp3B9=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3B8,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3B9);
# 1725
for(0;_tmp3B7 != 0;_tmp3B7=_tmp3B7->tl){
struct _tuple22*_tmp3BA=(struct _tuple22*)_tmp3B7->hd;struct _tuple22*_tmp3BB=_tmp3BA;int _tmp3BF;unsigned int _tmp3BE;_LL23F: _tmp3BF=(_tmp3BB->f2)->used;_tmp3BE=(_tmp3BB->f2)->pat_loc;_LL240:;
if(!_tmp3BF){
({void*_tmp3BC=0;({unsigned int _tmp5F4=_tmp3BE;struct _dyneither_ptr _tmp5F3=({const char*_tmp3BD="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp3BD,sizeof(char),74);});Cyc_Tcutil_terr(_tmp5F4,_tmp5F3,_tag_dyneither(_tmp3BC,sizeof(void*),0));});});
# 1730
break;}}};}
# 1739
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp3C0;_push_handler(& _tmp3C0);{int _tmp3C2=0;if(setjmp(_tmp3C0.handler))_tmp3C2=1;if(!_tmp3C2){
{struct _dyneither_ptr _tmp3C3=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp3C6;_tmp3C6.tag=0;_tmp3C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C3);({void*_tmp3C4[1]={& _tmp3C6};({unsigned int _tmp5F6=(*pr).f1;struct _dyneither_ptr _tmp5F5=({const char*_tmp3C5="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp3C5,sizeof(char),45);});Cyc_Tcutil_warn(_tmp5F6,_tmp5F5,_tag_dyneither(_tmp3C4,sizeof(void*),1));});});});}
# 1742
;_pop_handler();}else{void*_tmp3C1=(void*)_exn_thrown;void*_tmp3C7=_tmp3C1;void*_tmp3CA;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3C7)->tag == Cyc_Tcpat_Desc2string){_LL242: _LL243:
# 1746
({void*_tmp3C8=0;({unsigned int _tmp5F8=(*pr).f1;struct _dyneither_ptr _tmp5F7=({const char*_tmp3C9="pattern not exhaustive.";_tag_dyneither(_tmp3C9,sizeof(char),24);});Cyc_Tcutil_warn(_tmp5F8,_tmp5F7,_tag_dyneither(_tmp3C8,sizeof(void*),0));});});
goto _LL241;}else{_LL244: _tmp3CA=_tmp3C7;_LL245:(int)_rethrow(_tmp3CA);}_LL241:;}};};}
# 1750
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1754
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->used=0;_tmp3D2->pat_loc=p->loc;({struct Cyc_Absyn_Stmt*_tmp5F9=Cyc_Absyn_skip_stmt(0);_tmp3D2->rhs=_tmp5F9;});_tmp3D2;});
struct Cyc_List_List*_tmp3CB=({struct Cyc_List_List*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));({struct _tuple22*_tmp5FB=({struct _tuple22*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({void*_tmp5FA=Cyc_Tcpat_compile_pat(p);_tmp3D1->f1=_tmp5FA;});_tmp3D1->f2=rhs;_tmp3D1;});_tmp3D0->hd=_tmp5FB;});_tmp3D0->tl=0;_tmp3D0;});
void*_tmp3CC=Cyc_Tcpat_match_compile(_tmp3CB);
struct _tuple14 _tmp3CD=({struct _tuple14 _tmp3CF;_tmp3CF.f1=loc;_tmp3CF.f2=1;_tmp3CF;});
int _tmp3CE=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3CC,Cyc_Tcpat_not_exhaust_warn,& _tmp3CD,Cyc_Tcpat_dummy_fn,0,& _tmp3CE);
# 1762
*dopt=_tmp3CC;
return _tmp3CD.f2;}
# 1770
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->used=0;_tmp3D4->pat_loc=(swc->pattern)->loc;_tmp3D4->rhs=swc->body;_tmp3D4;});
return({struct _tuple22*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=sp0;_tmp3D3->f2=rhs;_tmp3D3;});}
# 1775
static void Cyc_Tcpat_not_exhaust_err2(unsigned int loc,void*d){;}
# 1778
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1782
struct Cyc_List_List*_tmp3D5=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp3D6=Cyc_Tcpat_match_compile(_tmp3D5);
*dopt=_tmp3D6;{
int _tmp3D7=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3D6,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3D7);
# 1788
for(0;_tmp3D5 != 0;_tmp3D5=_tmp3D5->tl){
# 1790
if(_tmp3D5->tl == 0)break;{
struct _tuple22*_tmp3D8=(struct _tuple22*)_tmp3D5->hd;struct _tuple22*_tmp3D9=_tmp3D8;int _tmp3DD;unsigned int _tmp3DC;_LL247: _tmp3DD=(_tmp3D9->f2)->used;_tmp3DC=(_tmp3D9->f2)->pat_loc;_LL248:;
if(!_tmp3DD){
({void*_tmp3DA=0;({unsigned int _tmp5FD=_tmp3DC;struct _dyneither_ptr _tmp5FC=({const char*_tmp3DB="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp3DB,sizeof(char),74);});Cyc_Tcutil_terr(_tmp5FD,_tmp5FC,_tag_dyneither(_tmp3DA,sizeof(void*),0));});});
break;}};}};}
# 1799
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp3DE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;for(0;_tmp3DE != 0;_tmp3DE=_tmp3DE->tl){
if((*((struct _tuple15*)_tmp3DE->hd)).f1 != 0)
return 1;}}
return 0;}
