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
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
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
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 723 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 915
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 923
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
# 935
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 937
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 943
void*Cyc_Absyn_compress_kb(void*);
# 948
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 950
void*Cyc_Absyn_wildtyp(struct Cyc_Core_Opt*);
# 953
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uint_typ;
# 955
extern void*Cyc_Absyn_sint_typ;
# 957
void*Cyc_Absyn_float_typ(int);
# 959
extern void*Cyc_Absyn_empty_effect;
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1063
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
# 1173
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
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 136
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 138
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
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);
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
({void*_tmp2=0;({unsigned int _tmp3E4=p->loc;struct _dyneither_ptr _tmp3E3=({const char*_tmp3="cannot determine pattern type";_tag_dyneither(_tmp3,sizeof(char),30);});Cyc_Tcutil_terr(_tmp3E4,_tmp3E3,_tag_dyneither(_tmp2,sizeof(void*),0));});});{
void*t=Cyc_Tcutil_compress(*((void**)_check_null(topt)));
{void*_tmp4=t;struct Cyc_Absyn_AggrInfo _tmpB;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->tag == 11){_LL10: _tmpB=((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4)->f1;_LL11:
# 53
({void*_tmp3E7=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp5=_cycalloc(sizeof(*_tmp5));({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp3E6=({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct _tmp6;_tmp6.tag=7;({struct Cyc_Absyn_AggrInfo*_tmp3E5=({struct Cyc_Absyn_AggrInfo*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7[0]=_tmpB;_tmp7;});_tmp6.f1=_tmp3E5;});_tmp6.f2=_tmp2C;_tmp6.f3=_tmp2B;_tmp6.f4=_tmp2A;_tmp6;});_tmp5[0]=_tmp3E6;});_tmp5;});p->r=_tmp3E7;});
Cyc_Tcpat_resolve_pat(te,topt,p);
goto _LLF;}else{_LL12: _LL13:
# 57
({struct Cyc_String_pa_PrintArg_struct _tmpA;_tmpA.tag=0;({struct _dyneither_ptr _tmp3E8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmpA.f1=_tmp3E8;});({void*_tmp8[1]={& _tmpA};({unsigned int _tmp3EA=p->loc;struct _dyneither_ptr _tmp3E9=({const char*_tmp9="pattern expects aggregate type instead of %s";_tag_dyneither(_tmp9,sizeof(char),45);});Cyc_Tcutil_terr(_tmp3EA,_tmp3E9,_tag_dyneither(_tmp8,sizeof(void*),1));});});});
goto _LLF;}_LLF:;}
# 61
return;};}else{if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).KnownAggr).tag == 2){_LL3: _tmp31=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->aggr_info).KnownAggr).val;_tmp30=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f1)->targs;_tmp2F=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f2;_tmp2E=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f3;_tmp2D=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1)->f4;_LL4: {
# 64
struct Cyc_Absyn_Aggrdecl*_tmpC=*_tmp31;
if(_tmpC->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpF;_tmpF.tag=0;({struct _dyneither_ptr _tmp3EB=(struct _dyneither_ptr)(
_tmpC->kind == Cyc_Absyn_StructA?({const char*_tmp10="struct";_tag_dyneither(_tmp10,sizeof(char),7);}):({const char*_tmp11="union";_tag_dyneither(_tmp11,sizeof(char),6);}));_tmpF.f1=_tmp3EB;});({void*_tmpD[1]={& _tmpF};({unsigned int _tmp3ED=p->loc;struct _dyneither_ptr _tmp3EC=({const char*_tmpE="can't destructure an abstract %s";_tag_dyneither(_tmpE,sizeof(char),33);});Cyc_Tcutil_terr(_tmp3ED,_tmp3EC,_tag_dyneither(_tmpD,sizeof(void*),1));});});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;
return;}{
# 71
int more_exists=({int _tmp3EE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC->impl))->exist_vars);_tmp3EE - ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(*_tmp2F);});
if(more_exists < 0){
({void*_tmp12=0;({unsigned int _tmp3F0=p->loc;struct _dyneither_ptr _tmp3EF=({const char*_tmp13="too many existentially bound type variables in pattern";_tag_dyneither(_tmp13,sizeof(char),55);});Cyc_Tcutil_terr(_tmp3F0,_tmp3EF,_tag_dyneither(_tmp12,sizeof(void*),0));});});{
struct Cyc_List_List**_tmp14=_tmp2F;
{int n=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpC->impl))->exist_vars);for(0;n != 0;-- n){
_tmp14=&((struct Cyc_List_List*)_check_null(*_tmp14))->tl;}}
*_tmp14=0;};}else{
if(more_exists > 0){
# 80
struct Cyc_List_List*_tmp15=0;
for(0;more_exists != 0;-- more_exists){
({struct Cyc_List_List*_tmp3F3=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));({struct Cyc_Absyn_Tvar*_tmp3F2=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp17=_cycalloc(sizeof(*_tmp17));({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3F1=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp18;_tmp18.tag=1;_tmp18.f1=0;_tmp18;});_tmp17[0]=_tmp3F1;});_tmp17;}));_tmp16->hd=_tmp3F2;});_tmp16->tl=_tmp15;_tmp16;});_tmp15=_tmp3F3;});}
({struct Cyc_List_List*_tmp3F4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp2F,_tmp15);*_tmp2F=_tmp3F4;});}}
# 85
return;};}}else{_LLB: _LLC:
# 98
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp27=_cycalloc(sizeof(*_tmp27));({struct Cyc_Core_Impossible_exn_struct _tmp3F6=({struct Cyc_Core_Impossible_exn_struct _tmp28;_tmp28.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp3F5=({const char*_tmp29="resolve_pat unknownAggr";_tag_dyneither(_tmp29,sizeof(char),24);});_tmp28.f1=_tmp3F5;});_tmp28;});_tmp27[0]=_tmp3F6;});_tmp27;}));}}case 17: _LL5: _tmp32=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1)->f1;_LL6:
# 87
 Cyc_Tcexp_tcExp(te,0,_tmp32);
if(!Cyc_Tcutil_is_const_exp(_tmp32)){
({void*_tmp19=0;({unsigned int _tmp3F8=p->loc;struct _dyneither_ptr _tmp3F7=({const char*_tmp1A="non-constant expression in case pattern";_tag_dyneither(_tmp1A,sizeof(char),40);});Cyc_Tcutil_terr(_tmp3F8,_tmp3F7,_tag_dyneither(_tmp19,sizeof(void*),0));});});
p->r=(void*)& Cyc_Absyn_Wild_p_val;}{
# 92
struct _tuple14 _tmp1B=Cyc_Evexp_eval_const_uint_exp(_tmp32);struct _tuple14 _tmp1C=_tmp1B;unsigned int _tmp20;int _tmp1F;_LL15: _tmp20=_tmp1C.f1;_tmp1F=_tmp1C.f2;_LL16:;
({void*_tmp3FA=(void*)({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*_tmp1D=_cycalloc_atomic(sizeof(*_tmp1D));({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp3F9=({struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct _tmp1E;_tmp1E.tag=10;_tmp1E.f1=Cyc_Absyn_None;_tmp1E.f2=(int)_tmp20;_tmp1E;});_tmp1D[0]=_tmp3F9;});_tmp1D;});p->r=_tmp3FA;});
return;};case 15: _LL7: _LL8:
# 96
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp21=_cycalloc(sizeof(*_tmp21));({struct Cyc_Core_Impossible_exn_struct _tmp3FC=({struct Cyc_Core_Impossible_exn_struct _tmp22;_tmp22.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp3FB=({const char*_tmp23="resolve_pat UnknownId_p";_tag_dyneither(_tmp23,sizeof(char),24);});_tmp22.f1=_tmp3FB;});_tmp22;});_tmp21[0]=_tmp3FC;});_tmp21;}));case 16: _LL9: _LLA:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp24=_cycalloc(sizeof(*_tmp24));({struct Cyc_Core_Impossible_exn_struct _tmp3FE=({struct Cyc_Core_Impossible_exn_struct _tmp25;_tmp25.tag=Cyc_Core_Impossible;({struct _dyneither_ptr _tmp3FD=({const char*_tmp26="resolve_pat UnknownCall_p";_tag_dyneither(_tmp26,sizeof(char),26);});_tmp25.f1=_tmp3FD;});_tmp25;});_tmp24[0]=_tmp3FE;});_tmp24;}));default: _LLD: _LLE:
# 100
 return;}_LL0:;}
# 104
static struct _dyneither_ptr*Cyc_Tcpat_get_name(struct Cyc_Absyn_Vardecl*vd){
return(*vd->name).f2;}
# 107
static void*Cyc_Tcpat_any_type(struct Cyc_List_List*s,void**topt){
if(topt != 0)
return*topt;
return({Cyc_Absyn_new_evar(& Cyc_Tcutil_mko,({struct Cyc_Core_Opt*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->v=s;_tmp33;}));});}
# 112
static void*Cyc_Tcpat_num_type(void**topt,void*numt){
# 116
if(topt != 0  && Cyc_Tcutil_coerceable(*topt))
return*topt;
# 119
{void*_tmp34=Cyc_Tcutil_compress(numt);void*_tmp35=_tmp34;switch(*((int*)_tmp35)){case 13: _LL18: _LL19:
 goto _LL1B;case 14: _LL1A: _LL1B:
 if(topt != 0)return*topt;goto _LL17;default: _LL1C: _LL1D:
 goto _LL17;}_LL17:;}
# 124
return numt;}struct _tuple15{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 127
static void Cyc_Tcpat_set_vd(struct Cyc_Absyn_Vardecl**vd,struct Cyc_Absyn_Exp*e,struct Cyc_List_List**v_result_ptr,void*t){
# 129
if(vd != 0){
(*vd)->type=t;
({struct Cyc_Absyn_Tqual _tmp3FF=Cyc_Absyn_empty_tqual(0);(*vd)->tq=_tmp3FF;});}
# 133
({struct Cyc_List_List*_tmp401=({struct Cyc_List_List*_tmp36=_cycalloc(sizeof(*_tmp36));({struct _tuple15*_tmp400=({struct _tuple15*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->f1=vd;_tmp37->f2=e;_tmp37;});_tmp36->hd=_tmp400;});_tmp36->tl=*v_result_ptr;_tmp36;});*v_result_ptr=_tmp401;});}
# 135
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_combine_results(struct Cyc_Tcpat_TcPatResult res1,struct Cyc_Tcpat_TcPatResult res2){
# 137
struct Cyc_Tcpat_TcPatResult _tmp38=res1;struct _tuple1*_tmp42;struct Cyc_List_List*_tmp41;_LL1F: _tmp42=_tmp38.tvars_and_bounds_opt;_tmp41=_tmp38.patvars;_LL20:;{
struct Cyc_Tcpat_TcPatResult _tmp39=res2;struct _tuple1*_tmp40;struct Cyc_List_List*_tmp3F;_LL22: _tmp40=_tmp39.tvars_and_bounds_opt;_tmp3F=_tmp39.patvars;_LL23:;
if(_tmp42 != 0  || _tmp40 != 0){
if(_tmp42 == 0)
({struct _tuple1*_tmp402=({struct _tuple1*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A->f1=0;_tmp3A->f2=0;_tmp3A;});_tmp42=_tmp402;});
if(_tmp40 == 0)
({struct _tuple1*_tmp403=({struct _tuple1*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=0;_tmp3B->f2=0;_tmp3B;});_tmp40=_tmp403;});
return({struct Cyc_Tcpat_TcPatResult _tmp3C;({struct _tuple1*_tmp406=({struct _tuple1*_tmp3D=_cycalloc(sizeof(*_tmp3D));({struct Cyc_List_List*_tmp404=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp42).f1,(*_tmp40).f1);_tmp3D->f1=_tmp404;});({struct Cyc_List_List*_tmp405=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*_tmp42).f2,(*_tmp40).f2);_tmp3D->f2=_tmp405;});_tmp3D;});_tmp3C.tvars_and_bounds_opt=_tmp406;});({struct Cyc_List_List*_tmp407=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp41,_tmp3F);_tmp3C.patvars=_tmp407;});_tmp3C;});}
# 148
return({struct Cyc_Tcpat_TcPatResult _tmp3E;_tmp3E.tvars_and_bounds_opt=0;({struct Cyc_List_List*_tmp408=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp41,_tmp3F);_tmp3E.patvars=_tmp408;});_tmp3E;});};}
# 151
static struct Cyc_Absyn_Pat*Cyc_Tcpat_wild_pat(unsigned int loc){
return({struct Cyc_Absyn_Pat*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->loc=loc;_tmp43->topt=0;_tmp43->r=(void*)& Cyc_Absyn_Wild_p_val;_tmp43;});}
# 156
static void*Cyc_Tcpat_pat_promote_array(struct Cyc_Tcenv_Tenv*te,void*t,void*rgn_opt){
return Cyc_Tcutil_is_array(t)?({
void*_tmp40A=t;Cyc_Tcutil_promote_array(_tmp40A,rgn_opt == 0?({Cyc_Absyn_new_evar(& Cyc_Tcutil_rko,({struct Cyc_Core_Opt*_tmp44=_cycalloc(sizeof(*_tmp44));({struct Cyc_List_List*_tmp409=Cyc_Tcenv_lookup_type_vars(te);_tmp44->v=_tmp409;});_tmp44;}));}): rgn_opt,0);}): t;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};
# 163
static struct _tuple16*Cyc_Tcpat_add_false(struct Cyc_Absyn_Tvar*tv){
return({struct _tuple16*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=tv;_tmp45->f2=0;_tmp45;});}struct _tuple17{struct Cyc_Absyn_Tqual f1;void*f2;};struct _tuple18{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _tuple19{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Pat*f2;};
# 167
static struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPatRec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,void*rgn_pat,int allow_ref_pat,struct Cyc_Absyn_Exp*access_exp){
# 170
Cyc_Tcpat_resolve_pat(te,topt,p);{
void*t;
struct Cyc_Tcpat_TcPatResult res=({struct Cyc_Tcpat_TcPatResult _tmp11A;_tmp11A.tvars_and_bounds_opt=0;_tmp11A.patvars=0;_tmp11A;});
# 175
{void*_tmp46=p->r;void*_tmp47=_tmp46;struct Cyc_Absyn_Datatypedecl*_tmp119;struct Cyc_Absyn_Datatypefield*_tmp118;struct Cyc_List_List**_tmp117;int _tmp116;struct Cyc_Absyn_Aggrdecl*_tmp115;struct Cyc_List_List**_tmp114;struct Cyc_List_List*_tmp113;struct Cyc_List_List**_tmp112;int _tmp111;struct Cyc_List_List**_tmp110;int _tmp10F;struct Cyc_Absyn_Pat*_tmp10E;void*_tmp10D;struct Cyc_Absyn_Enumdecl*_tmp10C;int _tmp10B;struct Cyc_Absyn_Tvar*_tmp10A;struct Cyc_Absyn_Vardecl*_tmp109;struct Cyc_Absyn_Vardecl*_tmp108;struct Cyc_Absyn_Pat*_tmp107;struct Cyc_Absyn_Tvar*_tmp106;struct Cyc_Absyn_Vardecl*_tmp105;struct Cyc_Absyn_Vardecl*_tmp104;struct Cyc_Absyn_Pat*_tmp103;switch(*((int*)_tmp47)){case 0: _LL25: _LL26:
# 178
 if(topt != 0)
t=*topt;else{
# 181
({void*_tmp40C=({struct Cyc_List_List*_tmp40B=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp40B,topt);});t=_tmp40C;});}
goto _LL24;case 1: _LL27: _tmp104=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp103=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL28: {
# 185
struct _tuple2*_tmp48=_tmp104->name;struct _tuple2*_tmp49=_tmp48;struct _dyneither_ptr _tmp56;_LL52: _tmp56=*_tmp49->f2;_LL53:;
if(({({struct _dyneither_ptr _tmp40E=(struct _dyneither_ptr)_tmp56;Cyc_strcmp(_tmp40E,({const char*_tmp4A="true";_tag_dyneither(_tmp4A,sizeof(char),5);}));})== 0;}) || ({({struct _dyneither_ptr _tmp40D=(struct _dyneither_ptr)_tmp56;Cyc_strcmp(_tmp40D,({const char*_tmp4B="false";_tag_dyneither(_tmp4B,sizeof(char),6);}));})== 0;}))
({struct Cyc_String_pa_PrintArg_struct _tmp4E;_tmp4E.tag=0;_tmp4E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp56);({void*_tmp4C[1]={& _tmp4E};({unsigned int _tmp410=p->loc;struct _dyneither_ptr _tmp40F=({const char*_tmp4D="you probably do not want to use %s as a local variable...";_tag_dyneither(_tmp4D,sizeof(char),58);});Cyc_Tcutil_warn(_tmp410,_tmp40F,_tag_dyneither(_tmp4C,sizeof(void*),1));});});});
# 189
({struct Cyc_Tcpat_TcPatResult _tmp411=Cyc_Tcpat_tcPatRec(te,_tmp103,topt,rgn_pat,allow_ref_pat,access_exp);res=_tmp411;});
t=(void*)_check_null(_tmp103->topt);
# 193
{void*_tmp4F=Cyc_Tcutil_compress(t);void*_tmp50=_tmp4F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp50)->tag == 8){_LL55: _LL56:
# 195
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp51=0;({unsigned int _tmp413=p->loc;struct _dyneither_ptr _tmp412=({const char*_tmp52="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp52,sizeof(char),58);});Cyc_Tcutil_terr(_tmp413,_tmp412,_tag_dyneither(_tmp51,sizeof(void*),0));});});
goto _LL54;}else{_LL57: _LL58:
# 199
 if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk);}))
({void*_tmp53=0;({unsigned int _tmp415=p->loc;struct _dyneither_ptr _tmp414=({const char*_tmp54="pattern would point to an abstract member";_tag_dyneither(_tmp54,sizeof(char),42);});Cyc_Tcutil_terr(_tmp415,_tmp414,_tag_dyneither(_tmp53,sizeof(void*),0));});});
goto _LL54;}_LL54:;}
# 203
({struct Cyc_Absyn_Vardecl**_tmp418=({struct Cyc_Absyn_Vardecl**_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=_tmp104;_tmp55;});struct Cyc_Absyn_Exp*_tmp417=access_exp;struct Cyc_List_List**_tmp416=& res.patvars;Cyc_Tcpat_set_vd(_tmp418,_tmp417,_tmp416,Cyc_Tcpat_pat_promote_array(te,t,rgn_pat));});
goto _LL24;}case 2: _LL29: _tmp106=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp105=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2A: {
# 206
struct Cyc_Tcenv_Tenv*te2=({unsigned int _tmp41A=p->loc;struct Cyc_Tcenv_Tenv*_tmp419=te;Cyc_Tcenv_add_type_vars(_tmp41A,_tmp419,({struct Cyc_Absyn_Tvar*_tmp61[1];_tmp61[0]=_tmp106;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp41B=({struct _tuple1*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=0;_tmp57->f2=0;_tmp57;});res.tvars_and_bounds_opt=_tmp41B;});
({struct Cyc_List_List*_tmp41E=({
struct Cyc_List_List*_tmp41D=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp41D,({struct Cyc_List_List*_tmp58=_cycalloc(sizeof(*_tmp58));({struct _tuple16*_tmp41C=({struct _tuple16*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=_tmp106;_tmp59->f2=1;_tmp59;});_tmp58->hd=_tmp41C;});_tmp58->tl=0;_tmp58;}));});
# 209
(*res.tvars_and_bounds_opt).f1=_tmp41E;});
# 211
({unsigned int _tmp421=p->loc;struct Cyc_Tcenv_Tenv*_tmp420=te2;struct Cyc_List_List*_tmp41F=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tcutil_check_type(_tmp421,_tmp420,_tmp41F,& Cyc_Tcutil_tmk,1,0,_tmp105->type);});
# 214
if(topt != 0)t=*topt;else{
({void*_tmp423=({struct Cyc_List_List*_tmp422=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tcpat_any_type(_tmp422,topt);});t=_tmp423;});}
{void*_tmp5A=Cyc_Tcutil_compress(t);void*_tmp5B=_tmp5A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B)->tag == 8){_LL5A: _LL5B:
# 218
 if(rgn_pat == 0  || !allow_ref_pat)
({void*_tmp5C=0;({unsigned int _tmp425=p->loc;struct _dyneither_ptr _tmp424=({const char*_tmp5D="array reference would point into unknown/unallowed region";_tag_dyneither(_tmp5D,sizeof(char),58);});Cyc_Tcutil_terr(_tmp425,_tmp424,_tag_dyneither(_tmp5C,sizeof(void*),0));});});
goto _LL59;}else{_LL5C: _LL5D:
# 222
 if(!({Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tmk);}))
({void*_tmp5E=0;({unsigned int _tmp427=p->loc;struct _dyneither_ptr _tmp426=({const char*_tmp5F="pattern would point to an abstract member";_tag_dyneither(_tmp5F,sizeof(char),42);});Cyc_Tcutil_terr(_tmp427,_tmp426,_tag_dyneither(_tmp5E,sizeof(void*),0));});});
goto _LL59;}_LL59:;}
# 226
({struct Cyc_Absyn_Vardecl**_tmp42A=({struct Cyc_Absyn_Vardecl**_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60[0]=_tmp105;_tmp60;});struct Cyc_Absyn_Exp*_tmp429=access_exp;struct Cyc_List_List**_tmp428=& res.patvars;Cyc_Tcpat_set_vd(_tmp42A,_tmp429,_tmp428,_tmp105->type);});
goto _LL24;}case 3: _LL2B: _tmp108=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp107=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2C:
# 230
({struct Cyc_Tcpat_TcPatResult _tmp42B=Cyc_Tcpat_tcPatRec(te,_tmp107,topt,rgn_pat,allow_ref_pat,access_exp);res=_tmp42B;});
t=(void*)_check_null(_tmp107->topt);
if(!allow_ref_pat  || rgn_pat == 0){
({void*_tmp62=0;({unsigned int _tmp42D=p->loc;struct _dyneither_ptr _tmp42C=({const char*_tmp63="* pattern would point into an unknown/unallowed region";_tag_dyneither(_tmp63,sizeof(char),55);});Cyc_Tcutil_terr(_tmp42D,_tmp42C,_tag_dyneither(_tmp62,sizeof(void*),0));});});
goto _LL24;}else{
# 237
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Core_heap_region,t))
({void*_tmp64=0;({unsigned int _tmp42F=p->loc;struct _dyneither_ptr _tmp42E=({const char*_tmp65="* pattern cannot take the address of an alias-free path";_tag_dyneither(_tmp65,sizeof(char),56);});Cyc_Tcutil_terr(_tmp42F,_tmp42E,_tag_dyneither(_tmp64,sizeof(void*),0));});});}{
# 240
struct Cyc_Absyn_Exp*new_access_exp=0;
void*t2=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp67=_cycalloc(sizeof(*_tmp67));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp435=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp68;_tmp68.tag=5;({struct Cyc_Absyn_PtrInfo _tmp434=({struct Cyc_Absyn_PtrInfo _tmp69;_tmp69.elt_typ=t;({struct Cyc_Absyn_Tqual _tmp430=Cyc_Absyn_empty_tqual(0);_tmp69.elt_tq=_tmp430;});({struct Cyc_Absyn_PtrAtts _tmp433=({(_tmp69.ptr_atts).rgn=rgn_pat;(_tmp69.ptr_atts).nullable=Cyc_Absyn_false_conref;({union Cyc_Absyn_Constraint*_tmp431=
# 243
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp69.ptr_atts).bounds=_tmp431;});({union Cyc_Absyn_Constraint*_tmp432=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp69.ptr_atts).zero_term=_tmp432;});(_tmp69.ptr_atts).ptrloc=0;_tmp69.ptr_atts;});_tmp69.ptr_atts=_tmp433;});_tmp69;});_tmp68.f1=_tmp434;});_tmp68;});_tmp67[0]=_tmp435;});_tmp67;});
# 245
if((unsigned int)access_exp){
({struct Cyc_Absyn_Exp*_tmp436=Cyc_Absyn_address_exp(access_exp,0);new_access_exp=_tmp436;});
new_access_exp->topt=t2;}
# 249
({struct Cyc_Absyn_Vardecl**_tmp439=({struct Cyc_Absyn_Vardecl**_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66[0]=_tmp108;_tmp66;});struct Cyc_Absyn_Exp*_tmp438=new_access_exp;struct Cyc_List_List**_tmp437=& res.patvars;Cyc_Tcpat_set_vd(_tmp439,_tmp438,_tmp437,t2);});
goto _LL24;};case 4: _LL2D: _tmp10A=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp109=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL2E:
# 255
({struct Cyc_Absyn_Vardecl**_tmp43C=({struct Cyc_Absyn_Vardecl**_tmp6A=_cycalloc(sizeof(*_tmp6A));_tmp6A[0]=_tmp109;_tmp6A;});struct Cyc_Absyn_Exp*_tmp43B=access_exp;struct Cyc_List_List**_tmp43A=& res.patvars;Cyc_Tcpat_set_vd(_tmp43C,_tmp43B,_tmp43A,_tmp109->type);});
# 259
({unsigned int _tmp43E=p->loc;struct Cyc_Tcenv_Tenv*_tmp43D=te;Cyc_Tcenv_add_type_vars(_tmp43E,_tmp43D,({struct Cyc_Absyn_Tvar*_tmp6B[1];_tmp6B[0]=_tmp10A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp6B,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp43F=({struct _tuple1*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C->f1=0;_tmp6C->f2=0;_tmp6C;});res.tvars_and_bounds_opt=_tmp43F;});
({struct Cyc_List_List*_tmp441=({struct Cyc_List_List*_tmp6D=_cycalloc(sizeof(*_tmp6D));({struct _tuple16*_tmp440=({struct _tuple16*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->f1=_tmp10A;_tmp6E->f2=0;_tmp6E;});_tmp6D->hd=_tmp440;});_tmp6D->tl=(*res.tvars_and_bounds_opt).f1;_tmp6D;});(*res.tvars_and_bounds_opt).f1=_tmp441;});
# 264
t=Cyc_Absyn_uint_typ;
goto _LL24;case 10: switch(((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp47)->f1){case Cyc_Absyn_Unsigned: _LL2F: _LL30:
# 267
({void*_tmp442=Cyc_Tcpat_num_type(topt,Cyc_Absyn_uint_typ);t=_tmp442;});goto _LL24;case Cyc_Absyn_None: _LL31: _LL32:
 goto _LL34;default: _LL33: _LL34:
({void*_tmp443=Cyc_Tcpat_num_type(topt,Cyc_Absyn_sint_typ);t=_tmp443;});goto _LL24;}case 11: _LL35: _LL36:
({void*_tmp444=Cyc_Tcpat_num_type(topt,Cyc_Absyn_char_typ);t=_tmp444;});goto _LL24;case 12: _LL37: _tmp10B=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL38:
({void*_tmp446=({void**_tmp445=topt;Cyc_Tcpat_num_type(_tmp445,Cyc_Absyn_float_typ(_tmp10B));});t=_tmp446;});goto _LL24;case 13: _LL39: _tmp10C=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL3A:
# 273
({void*_tmp449=({void**_tmp448=topt;Cyc_Tcpat_num_type(_tmp448,(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp447=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp70;_tmp70.tag=13;_tmp70.f1=_tmp10C->name;_tmp70.f2=_tmp10C;_tmp70;});_tmp6F[0]=_tmp447;});_tmp6F;}));});t=_tmp449;});
goto _LL24;case 14: _LL3B: _tmp10D=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL3C:
({void*_tmp44A=Cyc_Tcpat_num_type(topt,_tmp10D);t=_tmp44A;});goto _LL24;case 9: _LL3D: _LL3E:
# 277
 if(topt != 0){
void*_tmp71=Cyc_Tcutil_compress(*topt);void*_tmp72=_tmp71;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp72)->tag == 5){_LL5F: _LL60:
 t=*topt;goto tcpat_end;}else{_LL61: _LL62:
 goto _LL5E;}_LL5E:;}{
# 282
struct Cyc_Core_Opt*_tmp73=Cyc_Tcenv_lookup_opt_type_vars(te);
({void*_tmp453=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp452=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp75;_tmp75.tag=5;({struct Cyc_Absyn_PtrInfo _tmp451=({struct Cyc_Absyn_PtrInfo _tmp76;({void*_tmp44B=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp73);_tmp76.elt_typ=_tmp44B;});({struct Cyc_Absyn_Tqual _tmp44C=
Cyc_Absyn_empty_tqual(0);_tmp76.elt_tq=_tmp44C;});({struct Cyc_Absyn_PtrAtts _tmp450=({({void*_tmp44D=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp73);(_tmp76.ptr_atts).rgn=_tmp44D;});(_tmp76.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmp44E=
# 287
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp76.ptr_atts).bounds=_tmp44E;});({union Cyc_Absyn_Constraint*_tmp44F=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp76.ptr_atts).zero_term=_tmp44F;});(_tmp76.ptr_atts).ptrloc=0;_tmp76.ptr_atts;});_tmp76.ptr_atts=_tmp450;});_tmp76;});_tmp75.f1=_tmp451;});_tmp75;});_tmp74[0]=_tmp452;});_tmp74;});
# 283
t=_tmp453;});
# 290
goto _LL24;};case 6: _LL3F: _tmp10E=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_LL40: {
# 295
void*inner_typ=(void*)& Cyc_Absyn_VoidType_val;
void**_tmp77=0;
int elt_const=0;
if(topt != 0){
void*_tmp78=Cyc_Tcutil_compress(*topt);void*_tmp79=_tmp78;void*_tmp7B;struct Cyc_Absyn_Tqual _tmp7A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp79)->tag == 5){_LL64: _tmp7B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp79)->f1).elt_typ;_tmp7A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp79)->f1).elt_tq;_LL65:
# 301
 inner_typ=_tmp7B;
_tmp77=& inner_typ;
elt_const=_tmp7A.real_const;
goto _LL63;}else{_LL66: _LL67:
 goto _LL63;}_LL63:;}{
# 310
void*ptr_rgn=({Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,Cyc_Tcenv_lookup_opt_type_vars(te));});
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)({struct Cyc_Absyn_Exp*_tmp454=Cyc_Absyn_deref_exp(access_exp,0);new_access_exp=_tmp454;});
({struct Cyc_Tcpat_TcPatResult _tmp456=({struct Cyc_Tcpat_TcPatResult _tmp455=res;Cyc_Tcpat_combine_results(_tmp455,Cyc_Tcpat_tcPatRec(te,_tmp10E,_tmp77,ptr_rgn,1,new_access_exp));});res=_tmp456;});
# 319
{void*_tmp7C=Cyc_Tcutil_compress((void*)_check_null(_tmp10E->topt));void*_tmp7D=_tmp7C;struct Cyc_Absyn_Datatypedecl*_tmp8C;struct Cyc_Absyn_Datatypefield*_tmp8B;struct Cyc_List_List*_tmp8A;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->tag == 4){if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->f1).field_info).KnownDatatypefield).tag == 2){_LL69: _tmp8C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->f1).field_info).KnownDatatypefield).val).f1;_tmp8B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->f1).field_info).KnownDatatypefield).val).f2;_tmp8A=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7D)->f1).targs;_LL6A:
# 323
{void*_tmp7E=Cyc_Tcutil_compress(inner_typ);void*_tmp7F=_tmp7E;if(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp7F)->tag == 4){_LL6E: _LL6F:
# 325
 goto DONT_PROMOTE;}else{_LL70: _LL71:
 goto _LL6D;}_LL6D:;}{
# 329
void*new_type=(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp459=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp84;_tmp84.tag=3;({struct Cyc_Absyn_DatatypeInfo _tmp458=({struct Cyc_Absyn_DatatypeInfo _tmp85;({union Cyc_Absyn_DatatypeInfoU _tmp457=Cyc_Absyn_KnownDatatype(({struct Cyc_Absyn_Datatypedecl**_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86[0]=_tmp8C;_tmp86;}));_tmp85.datatype_info=_tmp457;});_tmp85.targs=_tmp8A;_tmp85;});_tmp84.f1=_tmp458;});_tmp84;});_tmp83[0]=_tmp459;});_tmp83;});
# 332
_tmp10E->topt=new_type;
({void*_tmp45F=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp80=_cycalloc(sizeof(*_tmp80));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp45E=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp81;_tmp81.tag=5;({struct Cyc_Absyn_PtrInfo _tmp45D=({struct Cyc_Absyn_PtrInfo _tmp82;_tmp82.elt_typ=new_type;({struct Cyc_Absyn_Tqual _tmp45A=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmp82.elt_tq=_tmp45A;});({struct Cyc_Absyn_PtrAtts _tmp45C=({(_tmp82.ptr_atts).rgn=ptr_rgn;({union Cyc_Absyn_Constraint*_tmp45B=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp82.ptr_atts).nullable=_tmp45B;});(_tmp82.ptr_atts).bounds=Cyc_Absyn_bounds_one_conref;(_tmp82.ptr_atts).zero_term=Cyc_Absyn_false_conref;(_tmp82.ptr_atts).ptrloc=0;_tmp82.ptr_atts;});_tmp82.ptr_atts=_tmp45C;});_tmp82;});_tmp81.f1=_tmp45D;});_tmp81;});_tmp80[0]=_tmp45E;});_tmp80;});
# 333
t=_tmp45F;});
# 339
goto _LL68;};}else{goto _LL6B;}}else{_LL6B: _LL6C:
# 341
 DONT_PROMOTE:
({void*_tmp467=(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp466=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp88;_tmp88.tag=5;({struct Cyc_Absyn_PtrInfo _tmp465=({struct Cyc_Absyn_PtrInfo _tmp89;_tmp89.elt_typ=(void*)_check_null(_tmp10E->topt);({struct Cyc_Absyn_Tqual _tmp460=
elt_const?Cyc_Absyn_const_tqual(0):
 Cyc_Absyn_empty_tqual(0);_tmp89.elt_tq=_tmp460;});({struct Cyc_Absyn_PtrAtts _tmp464=({(_tmp89.ptr_atts).rgn=ptr_rgn;({union Cyc_Absyn_Constraint*_tmp461=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp89.ptr_atts).nullable=_tmp461;});({union Cyc_Absyn_Constraint*_tmp462=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp89.ptr_atts).bounds=_tmp462;});({union Cyc_Absyn_Constraint*_tmp463=((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp89.ptr_atts).zero_term=_tmp463;});(_tmp89.ptr_atts).ptrloc=0;_tmp89.ptr_atts;});_tmp89.ptr_atts=_tmp464;});_tmp89;});_tmp88.f1=_tmp465;});_tmp88;});_tmp87[0]=_tmp466;});_tmp87;});
# 342
t=_tmp467;});}_LL68:;}
# 349
if((unsigned int)new_access_exp)new_access_exp->topt=_tmp10E->topt;
goto _LL24;};}case 5: _LL41: _tmp110=(struct Cyc_List_List**)&((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp10F=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_LL42: {
# 353
struct Cyc_List_List*_tmp8D=*_tmp110;
struct Cyc_List_List*pat_ts=0;
struct Cyc_List_List*topt_ts=0;
if(topt != 0){
void*_tmp8E=Cyc_Tcutil_compress(*topt);void*_tmp8F=_tmp8E;struct Cyc_List_List*_tmp95;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8F)->tag == 10){_LL73: _tmp95=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp8F)->f1;_LL74:
# 359
 topt_ts=_tmp95;
if(_tmp10F){
# 362
int _tmp90=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);
int _tmp91=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp95);
if(_tmp90 < _tmp91){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmp91 - _tmp90;++ i){
({struct Cyc_List_List*_tmp469=({struct Cyc_List_List*_tmp92=_cycalloc(sizeof(*_tmp92));({struct Cyc_Absyn_Pat*_tmp468=Cyc_Tcpat_wild_pat(p->loc);_tmp92->hd=_tmp468;});_tmp92->tl=wild_ps;_tmp92;});wild_ps=_tmp469;});}}
({struct Cyc_List_List*_tmp46A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp8D,wild_ps);*_tmp110=_tmp46A;});
_tmp8D=*_tmp110;}else{
if(({int _tmp46B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D);_tmp46B == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp95);}))
({void*_tmp93=0;({unsigned int _tmp46D=p->loc;struct _dyneither_ptr _tmp46C=({const char*_tmp94="unnecessary ... in tuple pattern";_tag_dyneither(_tmp94,sizeof(char),33);});Cyc_Tcutil_warn(_tmp46D,_tmp46C,_tag_dyneither(_tmp93,sizeof(void*),0));});});}}
# 374
goto _LL72;}else{_LL75: _LL76:
# 376
 goto _LL72;}_LL72:;}else{
# 378
if(_tmp10F)
({void*_tmp96=0;({unsigned int _tmp46F=p->loc;struct _dyneither_ptr _tmp46E=({const char*_tmp97="cannot determine missing fields for ... in tuple pattern";_tag_dyneither(_tmp97,sizeof(char),57);});Cyc_Tcutil_terr(_tmp46F,_tmp46E,_tag_dyneither(_tmp96,sizeof(void*),0));});});}
{int i=0;for(0;_tmp8D != 0;(_tmp8D=_tmp8D->tl,i ++)){
void**_tmp98=0;
if(topt_ts != 0){
_tmp98=&(*((struct _tuple17*)topt_ts->hd)).f2;
topt_ts=topt_ts->tl;}{
# 386
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
({struct Cyc_Absyn_Exp*_tmp472=({struct Cyc_Absyn_Exp*_tmp471=access_exp;Cyc_Absyn_subscript_exp(_tmp471,({
Cyc_Absyn_const_exp(({union Cyc_Absyn_Cnst _tmp99;({struct _tuple7 _tmp470=({struct _tuple7 _tmp9A;_tmp9A.f1=Cyc_Absyn_Unsigned;_tmp9A.f2=i;_tmp9A;});(_tmp99.Int_c).val=_tmp470;});(_tmp99.Int_c).tag=5;_tmp99;}),0);}),0);});
# 388
new_access_exp=_tmp472;});
# 391
({struct Cyc_Tcpat_TcPatResult _tmp474=({struct Cyc_Tcpat_TcPatResult _tmp473=res;Cyc_Tcpat_combine_results(_tmp473,Cyc_Tcpat_tcPatRec(te,(struct Cyc_Absyn_Pat*)_tmp8D->hd,_tmp98,rgn_pat,allow_ref_pat,new_access_exp));});res=_tmp474;});
# 394
if((unsigned int)new_access_exp)new_access_exp->topt=((struct Cyc_Absyn_Pat*)_tmp8D->hd)->topt;
({struct Cyc_List_List*_tmp477=({struct Cyc_List_List*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct _tuple17*_tmp476=({struct _tuple17*_tmp9C=_cycalloc(sizeof(*_tmp9C));({struct Cyc_Absyn_Tqual _tmp475=Cyc_Absyn_empty_tqual(0);_tmp9C->f1=_tmp475;});_tmp9C->f2=(void*)_check_null(((struct Cyc_Absyn_Pat*)_tmp8D->hd)->topt);_tmp9C;});_tmp9B->hd=_tmp476;});_tmp9B->tl=pat_ts;_tmp9B;});pat_ts=_tmp477;});};}}
# 397
({void*_tmp47A=(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp479=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp9E;_tmp9E.tag=10;({struct Cyc_List_List*_tmp478=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(pat_ts);_tmp9E.f1=_tmp478;});_tmp9E;});_tmp9D[0]=_tmp479;});_tmp9D;});t=_tmp47A;});
goto _LL24;}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->aggr_info).KnownAggr).tag == 2){_LL43: _tmp115=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->aggr_info).KnownAggr).val;_tmp114=(struct Cyc_List_List**)&(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f1)->targs;_tmp113=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_tmp112=(struct Cyc_List_List**)&((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f3;_tmp111=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp47)->f4;_LL44: {
# 401
struct Cyc_List_List*_tmp9F=*_tmp112;
struct _dyneither_ptr aggr_str=_tmp115->kind == Cyc_Absyn_StructA?({const char*_tmpE4="struct";_tag_dyneither(_tmpE4,sizeof(char),7);}):({const char*_tmpE5="union";_tag_dyneither(_tmpE5,sizeof(char),6);});
if(_tmp115->impl == 0){
({struct Cyc_String_pa_PrintArg_struct _tmpA2;_tmpA2.tag=0;_tmpA2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmpA0[1]={& _tmpA2};({unsigned int _tmp47C=p->loc;struct _dyneither_ptr _tmp47B=({const char*_tmpA1="can't destructure an abstract %s";_tag_dyneither(_tmpA1,sizeof(char),33);});Cyc_Tcutil_terr(_tmp47C,_tmp47B,_tag_dyneither(_tmpA0,sizeof(void*),1));});});});
({void*_tmp47D=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp47D;});
goto _LL24;}
# 410
if(_tmp115->kind == Cyc_Absyn_UnionA  && ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp115->impl))->tagged)
allow_ref_pat=0;
if(_tmp113 != 0){
# 416
if(topt == 0  || ({Cyc_Tcutil_typ_kind(*topt)!= & Cyc_Tcutil_ak;}))
allow_ref_pat=0;}{
# 419
struct _RegionHandle _tmpA3=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA3;_push_region(rgn);{
# 421
struct Cyc_List_List*_tmpA4=0;
struct Cyc_List_List*outlives_constraints=0;
struct Cyc_List_List*_tmpA5=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp115->impl))->exist_vars;
{struct Cyc_List_List*t=_tmp113;for(0;t != 0;t=t->tl){
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
({void*_tmpA9=0;({struct _dyneither_ptr _tmp47E=({const char*_tmpAA="unconstrained existential type variable in aggregate";_tag_dyneither(_tmpAA,sizeof(char),53);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp47E,_tag_dyneither(_tmpA9,sizeof(void*),0));});});}_LL77:;}
# 437
{void*_tmpAD=_tmpA6;struct Cyc_Core_Opt**_tmpB2;struct Cyc_Core_Opt**_tmpB1;struct Cyc_Absyn_Kind*_tmpB0;struct Cyc_Absyn_Kind*_tmpAF;switch(*((int*)_tmpAD)){case 0: _LL7F: _tmpAF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmpAD)->f1;_LL80:
# 440
 if(!Cyc_Tcutil_kind_leq(k2,_tmpAF))
error=1;
goto _LL7E;case 2: _LL81: _tmpB1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAD)->f1;_tmpB0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpAD)->f2;_LL82:
 _tmpB2=_tmpB1;goto _LL84;default: _LL83: _tmpB2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmpAD)->f1;_LL84:
({struct Cyc_Core_Opt*_tmp47F=({struct Cyc_Core_Opt*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->v=_tmpA7;_tmpAE;});*_tmpB2=_tmp47F;});goto _LL7E;}_LL7E:;}
# 446
if(error)
({struct Cyc_String_pa_PrintArg_struct _tmpB7;_tmpB7.tag=0;({struct _dyneither_ptr _tmp480=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 450
Cyc_Absynpp_kind2string(k2));_tmpB7.f1=_tmp480;});({struct Cyc_String_pa_PrintArg_struct _tmpB6;_tmpB6.tag=0;({struct _dyneither_ptr _tmp481=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(_tmpA6));_tmpB6.f1=_tmp481;});({struct Cyc_String_pa_PrintArg_struct _tmpB5;_tmpB5.tag=0;_tmpB5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmpB3[3]={& _tmpB5,& _tmpB6,& _tmpB7};({unsigned int _tmp483=p->loc;struct _dyneither_ptr _tmp482=({const char*_tmpB4="type variable %s has kind %s but must have at least kind %s";_tag_dyneither(_tmpB4,sizeof(char),60);});Cyc_Tcutil_terr(_tmp483,_tmp482,_tag_dyneither(_tmpB3,sizeof(void*),3));});});});});});{
# 452
void*vartype=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp484=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpBE;_tmpBE.tag=2;_tmpBE.f1=tv;_tmpBE;});_tmpBD[0]=_tmp484;});_tmpBD;});
({struct Cyc_List_List*_tmp485=({struct Cyc_List_List*_tmpB8=_region_malloc(rgn,sizeof(*_tmpB8));_tmpB8->hd=vartype;_tmpB8->tl=_tmpA4;_tmpB8;});_tmpA4=_tmp485;});
# 456
if(k2->kind == Cyc_Absyn_RgnKind){
if(k2->aliasqual == Cyc_Absyn_Aliasable)
({struct Cyc_List_List*_tmp487=({struct Cyc_List_List*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct _tuple0*_tmp486=({struct _tuple0*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->f1=Cyc_Absyn_empty_effect;_tmpBA->f2=vartype;_tmpBA;});_tmpB9->hd=_tmp486;});_tmpB9->tl=outlives_constraints;_tmpB9;});outlives_constraints=_tmp487;});else{
# 461
({void*_tmpBB=0;({struct _dyneither_ptr _tmp488=({const char*_tmpBC="opened existential had unique or top region kind";_tag_dyneither(_tmpBC,sizeof(char),49);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp488,_tag_dyneither(_tmpBB,sizeof(void*),0));});});}}};};}}
# 465
({struct Cyc_List_List*_tmp489=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpA4);_tmpA4=_tmp489;});{
# 467
struct _RegionHandle*_tmpBF=Cyc_Core_heap_region;
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(p->loc,te,_tmp113);
# 470
struct Cyc_List_List*_tmpC0=Cyc_Tcenv_lookup_type_vars(te2);
struct _tuple12 _tmpC1=({struct _tuple12 _tmpE3;_tmpE3.f1=_tmpC0;_tmpE3.f2=rgn;_tmpE3;});
struct Cyc_List_List*_tmpC2=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_r_make_inst_var,& _tmpC1,_tmp115->tvs);
struct Cyc_List_List*_tmpC3=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp115->impl))->exist_vars,_tmpA4);
struct Cyc_List_List*_tmpC4=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpC2);
struct Cyc_List_List*_tmpC5=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpC3);
struct Cyc_List_List*_tmpC6=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(rgn,_tmpC2,_tmpC3);
# 478
if(_tmp113 != 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp115->impl))->rgn_po != 0){
if(res.tvars_and_bounds_opt == 0)
({struct _tuple1*_tmp48A=({struct _tuple1*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=0;_tmpC7->f2=0;_tmpC7;});res.tvars_and_bounds_opt=_tmp48A;});
({struct Cyc_List_List*_tmp48C=({
struct Cyc_List_List*_tmp48B=(*res.tvars_and_bounds_opt).f1;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp48B,((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_add_false,_tmp113));});
# 481
(*res.tvars_and_bounds_opt).f1=_tmp48C;});
# 483
({struct Cyc_List_List*_tmp48D=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,outlives_constraints);
# 483
(*res.tvars_and_bounds_opt).f2=_tmp48D;});{
# 485
struct Cyc_List_List*_tmpC8=0;
{struct Cyc_List_List*_tmpC9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp115->impl))->rgn_po;for(0;_tmpC9 != 0;_tmpC9=_tmpC9->tl){
({struct Cyc_List_List*_tmp491=({struct Cyc_List_List*_tmpCA=_cycalloc(sizeof(*_tmpCA));({struct _tuple0*_tmp490=({struct _tuple0*_tmpCB=_cycalloc(sizeof(*_tmpCB));({void*_tmp48E=Cyc_Tcutil_rsubstitute(rgn,_tmpC6,(*((struct _tuple0*)_tmpC9->hd)).f1);_tmpCB->f1=_tmp48E;});({void*_tmp48F=
Cyc_Tcutil_rsubstitute(rgn,_tmpC6,(*((struct _tuple0*)_tmpC9->hd)).f2);_tmpCB->f2=_tmp48F;});_tmpCB;});_tmpCA->hd=_tmp490;});_tmpCA->tl=_tmpC8;_tmpCA;});
# 487
_tmpC8=_tmp491;});}}
# 490
({struct Cyc_List_List*_tmp492=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpC8);_tmpC8=_tmp492;});
({struct Cyc_List_List*_tmp493=
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)((*res.tvars_and_bounds_opt).f2,_tmpC8);
# 491
(*res.tvars_and_bounds_opt).f2=_tmp493;});};}
# 495
*_tmp114=_tmpC4;
({void*_tmp497=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpCC=_cycalloc(sizeof(*_tmpCC));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp496=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpCD;_tmpCD.tag=11;({struct Cyc_Absyn_AggrInfo _tmp495=({struct Cyc_Absyn_AggrInfo _tmpCE;({union Cyc_Absyn_AggrInfoU _tmp494=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF[0]=_tmp115;_tmpCF;}));_tmpCE.aggr_info=_tmp494;});_tmpCE.targs=*_tmp114;_tmpCE;});_tmpCD.f1=_tmp495;});_tmpCD;});_tmpCC[0]=_tmp496;});_tmpCC;});t=_tmp497;});
if(_tmp111  && _tmp115->kind == Cyc_Absyn_UnionA)
({void*_tmpD0=0;({unsigned int _tmp499=p->loc;struct _dyneither_ptr _tmp498=({const char*_tmpD1="`...' pattern not allowed in union pattern";_tag_dyneither(_tmpD1,sizeof(char),43);});Cyc_Tcutil_warn(_tmp499,_tmp498,_tag_dyneither(_tmpD0,sizeof(void*),0));});});else{
if(_tmp111){
# 501
int _tmpD2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp9F);
int _tmpD3=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp115->impl))->fields);
if(_tmpD2 < _tmpD3){
struct Cyc_List_List*wild_dps=0;
{int i=0;for(0;i < _tmpD3 - _tmpD2;++ i){
({struct Cyc_List_List*_tmp49C=({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct _tuple18*_tmp49B=({struct _tuple18*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->f1=0;({struct Cyc_Absyn_Pat*_tmp49A=Cyc_Tcpat_wild_pat(p->loc);_tmpD5->f2=_tmp49A;});_tmpD5;});_tmpD4->hd=_tmp49B;});_tmpD4->tl=wild_dps;_tmpD4;});wild_dps=_tmp49C;});}}
({struct Cyc_List_List*_tmp49D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9F,wild_dps);*_tmp112=_tmp49D;});
_tmp9F=*_tmp112;}else{
if(_tmpD2 == _tmpD3)
({void*_tmpD6=0;({unsigned int _tmp49F=p->loc;struct _dyneither_ptr _tmp49E=({const char*_tmpD7="unnecessary ... in struct pattern";_tag_dyneither(_tmpD7,sizeof(char),34);});Cyc_Tcutil_warn(_tmp49F,_tmp49E,_tag_dyneither(_tmpD6,sizeof(void*),0));});});}}}{
# 512
struct Cyc_List_List*fields=
((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(rgn,p->loc,_tmp9F,_tmp115->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp115->impl))->fields);
for(0;fields != 0;fields=fields->tl){
struct _tuple19*_tmpD8=(struct _tuple19*)fields->hd;struct _tuple19*_tmpD9=_tmpD8;struct Cyc_Absyn_Aggrfield*_tmpE2;struct Cyc_Absyn_Pat*_tmpE1;_LL86: _tmpE2=_tmpD9->f1;_tmpE1=_tmpD9->f2;_LL87:;{
void*_tmpDA=Cyc_Tcutil_rsubstitute(rgn,_tmpC6,_tmpE2->type);
# 518
struct Cyc_Absyn_Exp*new_access_exp=0;
if((unsigned int)access_exp)
({struct Cyc_Absyn_Exp*_tmp4A0=Cyc_Absyn_aggrmember_exp(access_exp,_tmpE2->name,0);new_access_exp=_tmp4A0;});
({struct Cyc_Tcpat_TcPatResult _tmp4A2=({struct Cyc_Tcpat_TcPatResult _tmp4A1=res;Cyc_Tcpat_combine_results(_tmp4A1,Cyc_Tcpat_tcPatRec(te2,_tmpE1,& _tmpDA,rgn_pat,allow_ref_pat,new_access_exp));});res=_tmp4A2;});
# 526
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpE1->topt),_tmpDA))
({struct Cyc_String_pa_PrintArg_struct _tmpE0;_tmpE0.tag=0;({struct _dyneither_ptr _tmp4A3=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 529
Cyc_Absynpp_typ2string((void*)_check_null(_tmpE1->topt)));_tmpE0.f1=_tmp4A3;});({struct Cyc_String_pa_PrintArg_struct _tmpDF;_tmpDF.tag=0;({struct _dyneither_ptr _tmp4A4=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 528
Cyc_Absynpp_typ2string(_tmpDA));_tmpDF.f1=_tmp4A4;});({struct Cyc_String_pa_PrintArg_struct _tmpDE;_tmpDE.tag=0;_tmpDE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({struct Cyc_String_pa_PrintArg_struct _tmpDD;_tmpDD.tag=0;_tmpDD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpE2->name);({void*_tmpDB[4]={& _tmpDD,& _tmpDE,& _tmpDF,& _tmpE0};({unsigned int _tmp4A6=p->loc;struct _dyneither_ptr _tmp4A5=({const char*_tmpDC="field %s of %s pattern expects type %s != %s";_tag_dyneither(_tmpDC,sizeof(char),45);});Cyc_Tcutil_terr(_tmp4A6,_tmp4A5,_tag_dyneither(_tmpDB,sizeof(void*),4));});});});});});});
# 530
if((unsigned int)new_access_exp)new_access_exp->topt=_tmpE1->topt;};}};};}
# 534
_npop_handler(0);goto _LL24;
# 419
;_pop_region(rgn);};}}else{_LL49: _LL4A:
# 590
 goto _LL4C;}}else{_LL47: _LL48:
# 589
 goto _LL4A;}case 8: _LL45: _tmp119=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f1;_tmp118=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f2;_tmp117=(struct Cyc_List_List**)&((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f3;_tmp116=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp47)->f4;_LL46: {
# 537
struct Cyc_List_List*_tmpE6=*_tmp117;
struct _RegionHandle*_tmpE7=Cyc_Core_heap_region;{
struct Cyc_List_List*tqts=_tmp118->typs;
# 541
struct Cyc_List_List*_tmpE8=Cyc_Tcenv_lookup_type_vars(te);
struct _tuple12 _tmpE9=({struct _tuple12 _tmp102;_tmp102.f1=_tmpE8;_tmp102.f2=_tmpE7;_tmp102;});
struct Cyc_List_List*_tmpEA=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple13*(*f)(struct _tuple12*,struct Cyc_Absyn_Tvar*),struct _tuple12*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpE7,Cyc_Tcutil_r_make_inst_var,& _tmpE9,_tmp119->tvs);
struct Cyc_List_List*_tmpEB=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple13*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple13*))Cyc_Core_snd,_tmpEA);
({void*_tmp4AA=(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp4A9=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpED;_tmpED.tag=4;({struct Cyc_Absyn_DatatypeFieldInfo _tmp4A8=({struct Cyc_Absyn_DatatypeFieldInfo _tmpEE;({union Cyc_Absyn_DatatypeFieldInfoU _tmp4A7=Cyc_Absyn_KnownDatatypefield(_tmp119,_tmp118);_tmpEE.field_info=_tmp4A7;});_tmpEE.targs=_tmpEB;_tmpEE;});_tmpED.f1=_tmp4A8;});_tmpED;});_tmpEC[0]=_tmp4A9;});_tmpEC;});t=_tmp4AA;});
# 547
if(_tmp116){
# 549
int _tmpEF=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpE6);
int _tmpF0=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tqts);
if(_tmpEF < _tmpF0){
struct Cyc_List_List*wild_ps=0;
{int i=0;for(0;i < _tmpF0 - _tmpEF;++ i){
({struct Cyc_List_List*_tmp4AC=({struct Cyc_List_List*_tmpF1=_cycalloc(sizeof(*_tmpF1));({struct Cyc_Absyn_Pat*_tmp4AB=Cyc_Tcpat_wild_pat(p->loc);_tmpF1->hd=_tmp4AB;});_tmpF1->tl=wild_ps;_tmpF1;});wild_ps=_tmp4AC;});}}
({struct Cyc_List_List*_tmp4AD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE6,wild_ps);*_tmp117=_tmp4AD;});
_tmpE6=*_tmp117;}else{
if(_tmpEF == _tmpF0)
({struct Cyc_String_pa_PrintArg_struct _tmpF4;_tmpF4.tag=0;({struct _dyneither_ptr _tmp4AE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp119->name));_tmpF4.f1=_tmp4AE;});({void*_tmpF2[1]={& _tmpF4};({unsigned int _tmp4B0=p->loc;struct _dyneither_ptr _tmp4AF=({const char*_tmpF3="unnecessary ... in datatype field %s";_tag_dyneither(_tmpF3,sizeof(char),37);});Cyc_Tcutil_warn(_tmp4B0,_tmp4AF,_tag_dyneither(_tmpF2,sizeof(void*),1));});});});}}
# 561
for(0;_tmpE6 != 0  && tqts != 0;(_tmpE6=_tmpE6->tl,tqts=tqts->tl)){
struct Cyc_Absyn_Pat*_tmpF5=(struct Cyc_Absyn_Pat*)_tmpE6->hd;
# 565
void*_tmpF6=Cyc_Tcutil_rsubstitute(_tmpE7,_tmpEA,(*((struct _tuple17*)tqts->hd)).f2);
# 568
if((unsigned int)access_exp)
Cyc_Tcpat_set_vd(0,access_exp,& res.patvars,Cyc_Absyn_char_typ);
({struct Cyc_Tcpat_TcPatResult _tmp4B2=({struct Cyc_Tcpat_TcPatResult _tmp4B1=res;Cyc_Tcpat_combine_results(_tmp4B1,Cyc_Tcpat_tcPatRec(te,_tmpF5,& _tmpF6,rgn_pat,allow_ref_pat,0));});res=_tmp4B2;});
# 575
if(!Cyc_Tcutil_unify((void*)_check_null(_tmpF5->topt),_tmpF6))
({struct Cyc_String_pa_PrintArg_struct _tmpFB;_tmpFB.tag=0;({struct _dyneither_ptr _tmp4B3=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 578
Cyc_Absynpp_typ2string((void*)_check_null(_tmpF5->topt)));_tmpFB.f1=_tmp4B3;});({struct Cyc_String_pa_PrintArg_struct _tmpFA;_tmpFA.tag=0;({struct _dyneither_ptr _tmp4B4=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 577
Cyc_Absynpp_typ2string(_tmpF6));_tmpFA.f1=_tmp4B4;});({struct Cyc_String_pa_PrintArg_struct _tmpF9;_tmpF9.tag=0;({struct _dyneither_ptr _tmp4B5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp118->name));_tmpF9.f1=_tmp4B5;});({void*_tmpF7[3]={& _tmpF9,& _tmpFA,& _tmpFB};({unsigned int _tmp4B7=_tmpF5->loc;struct _dyneither_ptr _tmp4B6=({const char*_tmpF8="%s expects argument type %s, not %s";_tag_dyneither(_tmpF8,sizeof(char),36);});Cyc_Tcutil_terr(_tmp4B7,_tmp4B6,_tag_dyneither(_tmpF7,sizeof(void*),3));});});});});});}
# 580
if(_tmpE6 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmpFE;_tmpFE.tag=0;({struct _dyneither_ptr _tmp4B8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp118->name));_tmpFE.f1=_tmp4B8;});({void*_tmpFC[1]={& _tmpFE};({unsigned int _tmp4BA=p->loc;struct _dyneither_ptr _tmp4B9=({const char*_tmpFD="too many arguments for datatype constructor %s";_tag_dyneither(_tmpFD,sizeof(char),47);});Cyc_Tcutil_terr(_tmp4BA,_tmp4B9,_tag_dyneither(_tmpFC,sizeof(void*),1));});});});
if(tqts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp101;_tmp101.tag=0;({struct _dyneither_ptr _tmp4BB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp118->name));_tmp101.f1=_tmp4BB;});({void*_tmpFF[1]={& _tmp101};({unsigned int _tmp4BD=p->loc;struct _dyneither_ptr _tmp4BC=({const char*_tmp100="too few arguments for datatype constructor %s";_tag_dyneither(_tmp100,sizeof(char),46);});Cyc_Tcutil_terr(_tmp4BD,_tmp4BC,_tag_dyneither(_tmpFF,sizeof(void*),1));});});});}
# 587
goto _LL24;}case 15: _LL4B: _LL4C:
# 591
 goto _LL4E;case 17: _LL4D: _LL4E:
 goto _LL50;default: _LL4F: _LL50:
# 594
({void*_tmp4BE=Cyc_Absyn_wildtyp(Cyc_Tcenv_lookup_opt_type_vars(te));t=_tmp4BE;});goto _LL24;}_LL24:;}
# 596
tcpat_end:
 p->topt=t;
return res;};}
# 601
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp){
# 603
struct Cyc_Tcpat_TcPatResult _tmp11B=Cyc_Tcpat_tcPatRec(te,p,topt,0,0,pat_var_exp);
# 605
struct _RegionHandle*_tmp11C=Cyc_Core_heap_region;{
struct _tuple1 _tmp11D=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp11C,_tmp11C,_tmp11B.patvars);struct _tuple1 _tmp11E=_tmp11D;struct Cyc_List_List*_tmp126;_LL89: _tmp126=_tmp11E.f1;_LL8A:;{
struct Cyc_List_List*_tmp11F=0;
{struct Cyc_List_List*x=_tmp126;for(0;x != 0;x=x->tl){
if((struct Cyc_Absyn_Vardecl**)x->hd != 0)({struct Cyc_List_List*_tmp4BF=({struct Cyc_List_List*_tmp120=_region_malloc(_tmp11C,sizeof(*_tmp120));_tmp120->hd=*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)x->hd));_tmp120->tl=_tmp11F;_tmp120;});_tmp11F=_tmp4BF;});}}
({struct Cyc_List_List*_tmp4C1=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct Cyc_Absyn_Vardecl*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp11C,Cyc_Tcpat_get_name,_tmp11F);unsigned int _tmp4C0=p->loc;Cyc_Tcutil_check_unique_vars(_tmp4C1,_tmp4C0,({const char*_tmp121="pattern contains a repeated variable";_tag_dyneither(_tmp121,sizeof(char),37);}));});{
# 615
struct Cyc_List_List*x=_tmp11B.patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp122=(struct _tuple15*)x->hd;struct _tuple15*_tmp123=_tmp122;struct Cyc_Absyn_Vardecl**_tmp125;struct Cyc_Absyn_Exp**_tmp124;_LL8C: _tmp125=_tmp123->f1;_tmp124=(struct Cyc_Absyn_Exp**)& _tmp123->f2;_LL8D:;
if(*_tmp124 != 0  && *_tmp124 != pat_var_exp)
({struct Cyc_Absyn_Exp*_tmp4C2=Cyc_Tcutil_deep_copy_exp(1,(struct Cyc_Absyn_Exp*)_check_null(*_tmp124));*_tmp124=_tmp4C2;});}};};}
# 621
return _tmp11B;}
# 627
static int Cyc_Tcpat_try_alias_coerce(struct Cyc_Tcenv_Tenv*tenv,void*old_type,void*new_type,struct Cyc_Absyn_Exp*initializer,struct Cyc_List_List*assump){
# 630
struct _tuple0 _tmp127=({struct _tuple0 _tmp12F;({void*_tmp4C3=Cyc_Tcutil_compress(old_type);_tmp12F.f1=_tmp4C3;});({void*_tmp4C4=Cyc_Tcutil_compress(new_type);_tmp12F.f2=_tmp4C4;});_tmp12F;});struct _tuple0 _tmp128=_tmp127;struct Cyc_Absyn_PtrInfo _tmp12E;struct Cyc_Absyn_PtrInfo _tmp12D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128.f2)->tag == 5){_LL8F: _tmp12E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128.f1)->f1;_tmp12D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp128.f2)->f1;_LL90: {
# 632
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp129=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4C7=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp12B;_tmp12B.tag=5;({struct Cyc_Absyn_PtrInfo _tmp4C6=({struct Cyc_Absyn_PtrInfo _tmp12C;_tmp12C.elt_typ=_tmp12E.elt_typ;_tmp12C.elt_tq=_tmp12D.elt_tq;({struct Cyc_Absyn_PtrAtts _tmp4C5=({(_tmp12C.ptr_atts).rgn=(_tmp12E.ptr_atts).rgn;(_tmp12C.ptr_atts).nullable=(_tmp12D.ptr_atts).nullable;(_tmp12C.ptr_atts).bounds=(_tmp12D.ptr_atts).bounds;(_tmp12C.ptr_atts).zero_term=(_tmp12D.ptr_atts).zero_term;(_tmp12C.ptr_atts).ptrloc=(_tmp12E.ptr_atts).ptrloc;_tmp12C.ptr_atts;});_tmp12C.ptr_atts=_tmp4C5;});_tmp12C;});_tmp12B.f1=_tmp4C6;});_tmp12B;});_tmp12A[0]=_tmp4C7;});_tmp12A;});
# 638
return Cyc_Tcutil_subtype(tenv,assump,(void*)_tmp129,new_type) && 
Cyc_Tcutil_coerce_assign(tenv,initializer,(void*)_tmp129);}}else{goto _LL91;}}else{_LL91: _LL92:
 return 0;}_LL8E:;}
# 647
static void Cyc_Tcpat_check_alias_coercion(struct Cyc_Tcenv_Tenv*tenv,unsigned int loc,void*old_type,struct Cyc_Absyn_Tvar*tv,void*new_type,struct Cyc_Absyn_Exp*initializer){
# 650
struct Cyc_List_List*assump=({struct Cyc_List_List*_tmp13B=_cycalloc(sizeof(*_tmp13B));({struct _tuple0*_tmp4CA=({struct _tuple0*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->f1=(void*)& Cyc_Absyn_UniqueRgn_val;({void*_tmp4C9=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp13D=_cycalloc(sizeof(*_tmp13D));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4C8=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp13E;_tmp13E.tag=2;_tmp13E.f1=tv;_tmp13E;});_tmp13D[0]=_tmp4C8;});_tmp13D;});_tmp13C->f2=_tmp4C9;});_tmp13C;});_tmp13B->hd=_tmp4CA;});_tmp13B->tl=0;_tmp13B;});
if(Cyc_Tcutil_subtype(tenv,assump,old_type,new_type)){
# 668 "tcpat.cyc"
struct _tuple0 _tmp130=({struct _tuple0 _tmp136;({void*_tmp4CB=Cyc_Tcutil_compress(old_type);_tmp136.f1=_tmp4CB;});({void*_tmp4CC=Cyc_Tcutil_compress(new_type);_tmp136.f2=_tmp4CC;});_tmp136;});struct _tuple0 _tmp131=_tmp130;struct Cyc_Absyn_PtrInfo _tmp135;struct Cyc_Absyn_PtrInfo _tmp134;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131.f1)->tag == 5){if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131.f2)->tag == 5){_LL94: _tmp135=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131.f1)->f1;_tmp134=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131.f2)->f1;_LL95:
 goto _LL93;}else{goto _LL96;}}else{_LL96: _LL97:
({void*_tmp132=0;({unsigned int _tmp4CE=loc;struct _dyneither_ptr _tmp4CD=({const char*_tmp133="alias requires pointer type";_tag_dyneither(_tmp133,sizeof(char),28);});Cyc_Tcutil_terr(_tmp4CE,_tmp4CD,_tag_dyneither(_tmp132,sizeof(void*),0));});});goto _LL93;}_LL93:;}else{
# 673
({struct Cyc_String_pa_PrintArg_struct _tmp13A;_tmp13A.tag=0;({struct _dyneither_ptr _tmp4CF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(new_type));_tmp13A.f1=_tmp4CF;});({struct Cyc_String_pa_PrintArg_struct _tmp139;_tmp139.tag=0;({struct _dyneither_ptr _tmp4D0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(old_type));_tmp139.f1=_tmp4D0;});({void*_tmp137[2]={& _tmp139,& _tmp13A};({unsigned int _tmp4D2=loc;struct _dyneither_ptr _tmp4D1=({const char*_tmp138="cannot alias value of type %s to type %s";_tag_dyneither(_tmp138,sizeof(char),41);});Cyc_Tcutil_terr(_tmp4D2,_tmp4D1,_tag_dyneither(_tmp137,sizeof(void*),2));});});});});}}
# 680
void Cyc_Tcpat_check_pat_regions_rec(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,int did_noalias_deref,struct Cyc_List_List*patvars){
# 684
void*_tmp13F=p->r;void*_tmp140=_tmp13F;struct Cyc_Absyn_Tvar*_tmp169;struct Cyc_Absyn_Vardecl*_tmp168;struct Cyc_Absyn_Vardecl*_tmp167;struct Cyc_Absyn_Pat*_tmp166;struct Cyc_Absyn_Vardecl*_tmp165;struct Cyc_Absyn_Pat*_tmp164;struct Cyc_List_List*_tmp163;struct Cyc_List_List*_tmp162;struct Cyc_Absyn_AggrInfo*_tmp161;struct Cyc_List_List*_tmp160;struct Cyc_List_List*_tmp15F;struct Cyc_Absyn_Pat*_tmp15E;switch(*((int*)_tmp140)){case 6: _LL99: _tmp15E=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp140)->f1;_LL9A: {
# 686
void*_tmp141=(void*)_check_null(p->topt);void*_tmp142=_tmp141;void*_tmp145;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142)->tag == 5){_LLAA: _tmp145=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp142)->f1).ptr_atts).rgn;_LLAB:
# 688
 Cyc_Tcenv_check_rgn_accessible(te,p->loc,_tmp145);
({struct Cyc_Tcenv_Tenv*_tmp4D5=te;struct Cyc_Absyn_Pat*_tmp4D4=_tmp15E;int _tmp4D3=Cyc_Tcutil_is_noalias_region(_tmp145,0);Cyc_Tcpat_check_pat_regions_rec(_tmp4D5,_tmp4D4,_tmp4D3,patvars);});
return;}else{_LLAC: _LLAD:
({void*_tmp143=0;({struct _dyneither_ptr _tmp4D6=({const char*_tmp144="check_pat_regions: bad pointer type";_tag_dyneither(_tmp144,sizeof(char),36);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4D6,_tag_dyneither(_tmp143,sizeof(void*),0));});});}_LLA9:;}case 7: _LL9B: _tmp161=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp140)->f1;_tmp160=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp140)->f2;_tmp15F=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp140)->f3;_LL9C:
# 694
 for(0;_tmp15F != 0;_tmp15F=_tmp15F->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(*((struct _tuple18*)_tmp15F->hd)).f2,did_noalias_deref,patvars);}
return;case 8: _LL9D: _tmp162=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp140)->f3;_LL9E:
# 698
 did_noalias_deref=0;_tmp163=_tmp162;goto _LLA0;case 5: _LL9F: _tmp163=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp140)->f1;_LLA0:
# 700
 for(0;_tmp163 != 0;_tmp163=_tmp163->tl){
Cyc_Tcpat_check_pat_regions_rec(te,(struct Cyc_Absyn_Pat*)_tmp163->hd,did_noalias_deref,patvars);}
return;case 3: _LLA1: _tmp165=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp140)->f1;_tmp164=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp140)->f2;_LLA2:
# 704
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp146=(struct _tuple15*)x->hd;struct _tuple15*_tmp147=_tmp146;struct Cyc_Absyn_Vardecl**_tmp150;struct Cyc_Absyn_Exp*_tmp14F;_LLAF: _tmp150=_tmp147->f1;_tmp14F=_tmp147->f2;_LLB0:;
# 710
if((_tmp150 != 0  && *_tmp150 == _tmp165) && _tmp14F != 0){
{void*_tmp148=_tmp14F->r;void*_tmp149=_tmp148;struct Cyc_Absyn_Exp*_tmp14E;if(((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp149)->tag == 14){_LLB2: _tmp14E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp149)->f1;_LLB3:
# 713
 if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Core_heap_region,(void*)_check_null(_tmp14E->topt)))
({void*_tmp14A=0;({unsigned int _tmp4D8=p->loc;struct _dyneither_ptr _tmp4D7=({const char*_tmp14B="reference pattern not allowed on alias-free pointers";_tag_dyneither(_tmp14B,sizeof(char),53);});Cyc_Tcutil_terr(_tmp4D8,_tmp4D7,_tag_dyneither(_tmp14A,sizeof(void*),0));});});
goto _LLB1;}else{_LLB4: _LLB5:
# 717
({void*_tmp14C=0;({struct _dyneither_ptr _tmp4D9=({const char*_tmp14D="check_pat_regions: bad reference access expression";_tag_dyneither(_tmp14D,sizeof(char),51);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4D9,_tag_dyneither(_tmp14C,sizeof(void*),0));});});}_LLB1:;}
# 719
break;}}}
# 722
Cyc_Tcpat_check_pat_regions_rec(te,_tmp164,did_noalias_deref,patvars);
return;case 1: _LLA3: _tmp167=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp140)->f1;_tmp166=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp140)->f2;_LLA4:
# 725
{void*_tmp151=p->topt;void*_tmp152=_tmp151;if(_tmp152 != 0){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp152)->tag == 8){_LLB7: _LLB8:
# 727
 if(did_noalias_deref){
({void*_tmp153=0;({unsigned int _tmp4DB=p->loc;struct _dyneither_ptr _tmp4DA=({const char*_tmp154="pattern to array would create alias of no-alias pointer";_tag_dyneither(_tmp154,sizeof(char),56);});Cyc_Tcutil_terr(_tmp4DB,_tmp4DA,_tag_dyneither(_tmp153,sizeof(void*),0));});});
return;}
# 731
goto _LLB6;}else{goto _LLB9;}}else{_LLB9: _LLBA:
 goto _LLB6;}_LLB6:;}
# 734
Cyc_Tcpat_check_pat_regions_rec(te,_tmp166,did_noalias_deref,patvars);
return;case 2: _LLA5: _tmp169=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp140)->f1;_tmp168=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp140)->f2;_LLA6:
# 737
{struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp155=(struct _tuple15*)x->hd;struct _tuple15*_tmp156=_tmp155;struct Cyc_Absyn_Vardecl**_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;_LLBC: _tmp15D=_tmp156->f1;_tmp15C=_tmp156->f2;_LLBD:;
# 741
if(_tmp15D != 0  && *_tmp15D == _tmp168){
if(_tmp15C == 0)
({void*_tmp157=0;({unsigned int _tmp4DD=p->loc;struct _dyneither_ptr _tmp4DC=({const char*_tmp158="cannot alias pattern expression in datatype";_tag_dyneither(_tmp158,sizeof(char),44);});Cyc_Tcutil_terr(_tmp4DD,_tmp4DC,_tag_dyneither(_tmp157,sizeof(void*),0));});});else{
# 745
struct Cyc_Tcenv_Tenv*te2=({unsigned int _tmp4DF=p->loc;struct Cyc_Tcenv_Tenv*_tmp4DE=te;Cyc_Tcenv_add_type_vars(_tmp4DF,_tmp4DE,({struct Cyc_Absyn_Tvar*_tmp15B[1];_tmp15B[0]=_tmp169;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp15B,sizeof(struct Cyc_Absyn_Tvar*),1));}));});
({struct Cyc_Tcenv_Tenv*_tmp4E2=({struct Cyc_Tcenv_Tenv*_tmp4E1=te2;Cyc_Tcenv_add_region(_tmp4E1,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp159=_cycalloc(sizeof(*_tmp159));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4E0=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp15A;_tmp15A.tag=2;_tmp15A.f1=_tmp169;_tmp15A;});_tmp159[0]=_tmp4E0;});_tmp159;}),1);});te2=_tmp4E2;});
# 748
Cyc_Tcpat_check_alias_coercion(te2,p->loc,(void*)_check_null(_tmp15C->topt),_tmp169,_tmp168->type,_tmp15C);}
# 751
break;}}}
# 754
goto _LL98;default: _LLA7: _LLA8:
 return;}_LL98:;}
# 770 "tcpat.cyc"
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars){
# 772
Cyc_Tcpat_check_pat_regions_rec(te,p,0,patvars);{
struct _RegionHandle*_tmp16A=Cyc_Core_heap_region;
struct Cyc_List_List*x=patvars;for(0;x != 0;x=x->tl){
struct _tuple15*_tmp16B=(struct _tuple15*)x->hd;struct _tuple15*_tmp16C=_tmp16B;struct Cyc_Absyn_Vardecl**_tmp176;struct Cyc_Absyn_Exp*_tmp175;_LLBF: _tmp176=_tmp16C->f1;_tmp175=_tmp16C->f2;_LLC0:;
if(_tmp175 != 0){
struct Cyc_Absyn_Exp*_tmp16D=_tmp175;
# 780
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp16A,(void*)_check_null(_tmp16D->topt)) && !
Cyc_Tcutil_is_noalias_path(_tmp16A,_tmp16D))
# 783
({struct Cyc_String_pa_PrintArg_struct _tmp170;_tmp170.tag=0;({struct _dyneither_ptr _tmp4E5=(struct _dyneither_ptr)(
# 785
_tmp176 != 0?(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp173;_tmp173.tag=0;({struct _dyneither_ptr _tmp4E3=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 787
Cyc_Absynpp_qvar2string((*_tmp176)->name));_tmp173.f1=_tmp4E3;});({void*_tmp171[1]={& _tmp173};({struct _dyneither_ptr _tmp4E4=({const char*_tmp172="for variable %s";_tag_dyneither(_tmp172,sizeof(char),16);});Cyc_aprintf(_tmp4E4,_tag_dyneither(_tmp171,sizeof(void*),1));});});}):({const char*_tmp174="";_tag_dyneither(_tmp174,sizeof(char),1);}));_tmp170.f1=_tmp4E5;});({void*_tmp16E[1]={& _tmp170};({unsigned int _tmp4E7=p->loc;struct _dyneither_ptr _tmp4E6=({const char*_tmp16F="pattern %s dereferences a alias-free pointer from a non-unique path";_tag_dyneither(_tmp16F,sizeof(char),68);});Cyc_Tcutil_terr(_tmp4E7,_tmp4E6,_tag_dyneither(_tmp16E,sizeof(void*),1));});});});}}};}
# 842 "tcpat.cyc"
struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct Cyc_Tcpat_EqNull_val={1};
struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct Cyc_Tcpat_NeqNull_val={2};
# 852
struct Cyc_Tcpat_Dummy_Tcpat_Access_struct Cyc_Tcpat_Dummy_val={0};
struct Cyc_Tcpat_Deref_Tcpat_Access_struct Cyc_Tcpat_Deref_val={1};union Cyc_Tcpat_PatOrWhere;struct Cyc_Tcpat_PathNode;struct _union_Name_value_Name_v{int tag;struct _dyneither_ptr val;};struct _union_Name_value_Int_v{int tag;int val;};union Cyc_Tcpat_Name_value{struct _union_Name_value_Name_v Name_v;struct _union_Name_value_Int_v Int_v;};
# 865
typedef union Cyc_Tcpat_Name_value Cyc_Tcpat_name_value_t;
union Cyc_Tcpat_Name_value Cyc_Tcpat_Name_v(struct _dyneither_ptr s){return({union Cyc_Tcpat_Name_value _tmp17B;(_tmp17B.Name_v).val=s;(_tmp17B.Name_v).tag=1;_tmp17B;});}
union Cyc_Tcpat_Name_value Cyc_Tcpat_Int_v(int i){return({union Cyc_Tcpat_Name_value _tmp17C;(_tmp17C.Int_v).val=i;(_tmp17C.Int_v).tag=2;_tmp17C;});}struct Cyc_Tcpat_Con_s{union Cyc_Tcpat_Name_value name;int arity;int*span;union Cyc_Tcpat_PatOrWhere orig_pat;};
# 875
typedef struct Cyc_Tcpat_Con_s*Cyc_Tcpat_con_t;struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct{int tag;};struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 880
typedef void*Cyc_Tcpat_simple_pat_t;
# 883
static int Cyc_Tcpat_compare_con(struct Cyc_Tcpat_Con_s*c1,struct Cyc_Tcpat_Con_s*c2){
union Cyc_Tcpat_Name_value _tmp17D=c1->name;union Cyc_Tcpat_Name_value _tmp17E=_tmp17D;int _tmp186;struct _dyneither_ptr _tmp185;if((_tmp17E.Name_v).tag == 1){_LLC2: _tmp185=(_tmp17E.Name_v).val;_LLC3: {
# 886
union Cyc_Tcpat_Name_value _tmp17F=c2->name;union Cyc_Tcpat_Name_value _tmp180=_tmp17F;struct _dyneither_ptr _tmp181;if((_tmp180.Name_v).tag == 1){_LLC7: _tmp181=(_tmp180.Name_v).val;_LLC8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp185,(struct _dyneither_ptr)_tmp181);}else{_LLC9: _LLCA:
 return - 1;}_LLC6:;}}else{_LLC4: _tmp186=(_tmp17E.Int_v).val;_LLC5: {
# 891
union Cyc_Tcpat_Name_value _tmp182=c2->name;union Cyc_Tcpat_Name_value _tmp183=_tmp182;int _tmp184;if((_tmp183.Name_v).tag == 1){_LLCC: _LLCD:
 return 1;}else{_LLCE: _tmp184=(_tmp183.Int_v).val;_LLCF:
 return _tmp186 - _tmp184;}_LLCB:;}}_LLC1:;}
# 899
static struct Cyc_Set_Set*Cyc_Tcpat_empty_con_set(){
return((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct Cyc_Tcpat_Con_s*,struct Cyc_Tcpat_Con_s*)))Cyc_Set_rempty)(Cyc_Core_heap_region,Cyc_Tcpat_compare_con);}
# 903
static int Cyc_Tcpat_one_opt=1;
static int Cyc_Tcpat_two_opt=2;
static int Cyc_Tcpat_twofiftysix_opt=256;
# 907
static unsigned int Cyc_Tcpat_datatype_tag_number(struct Cyc_Absyn_Datatypedecl*td,struct _tuple2*name){
unsigned int ans=0;
struct Cyc_List_List*_tmp187=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(({Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp187))->hd)->name)!= 0;})){
++ ans;
_tmp187=_tmp187->tl;}
# 914
return ans;}
# 917
static int Cyc_Tcpat_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp188=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp188 != 0;_tmp188=_tmp188->tl){
struct Cyc_Absyn_Aggrfield*_tmp189=(struct Cyc_Absyn_Aggrfield*)_tmp188->hd;
if(({Cyc_strcmp((struct _dyneither_ptr)*_tmp189->name,(struct _dyneither_ptr)*f)== 0;}))return i;
++ i;}}
# 924
({void*_tmp18A=0;({struct _dyneither_ptr _tmp4E9=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18D;_tmp18D.tag=0;_tmp18D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);({void*_tmp18B[1]={& _tmp18D};({struct _dyneither_ptr _tmp4E8=({const char*_tmp18C="get_member_offset %s failed";_tag_dyneither(_tmp18C,sizeof(char),28);});Cyc_aprintf(_tmp4E8,_tag_dyneither(_tmp18B,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4E9,_tag_dyneither(_tmp18A,sizeof(void*),0));});});}
# 927
static void*Cyc_Tcpat_get_pat_test(union Cyc_Tcpat_PatOrWhere pw){
union Cyc_Tcpat_PatOrWhere _tmp18E=pw;struct Cyc_Absyn_Pat*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C6;if((_tmp18E.where_clause).tag == 2){_LLD1: _tmp1C6=(_tmp18E.where_clause).val;_LLD2:
 return(void*)({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*_tmp18F=_cycalloc(sizeof(*_tmp18F));({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct _tmp4EA=({struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct _tmp190;_tmp190.tag=0;_tmp190.f1=_tmp1C6;_tmp190;});_tmp18F[0]=_tmp4EA;});_tmp18F;});}else{_LLD3: _tmp1C7=(_tmp18E.pattern).val;_LLD4: {
# 931
void*_tmp191=_tmp1C7->r;void*_tmp192=_tmp191;union Cyc_Absyn_AggrInfoU _tmp1C5;struct Cyc_List_List*_tmp1C4;struct Cyc_Absyn_Datatypedecl*_tmp1C3;struct Cyc_Absyn_Datatypefield*_tmp1C2;void*_tmp1C1;struct Cyc_Absyn_Enumfield*_tmp1C0;struct Cyc_Absyn_Enumdecl*_tmp1BF;struct Cyc_Absyn_Enumfield*_tmp1BE;struct _dyneither_ptr _tmp1BD;int _tmp1BC;char _tmp1BB;enum Cyc_Absyn_Sign _tmp1BA;int _tmp1B9;struct Cyc_Absyn_Pat*_tmp1B8;struct Cyc_Absyn_Pat*_tmp1B7;switch(*((int*)_tmp192)){case 1: _LLD6: _tmp1B7=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp192)->f2;_LLD7:
 _tmp1B8=_tmp1B7;goto _LLD9;case 3: _LLD8: _tmp1B8=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp192)->f2;_LLD9:
 return Cyc_Tcpat_get_pat_test(({union Cyc_Tcpat_PatOrWhere _tmp193;(_tmp193.pattern).val=_tmp1B8;(_tmp193.pattern).tag=1;_tmp193;}));case 9: _LLDA: _LLDB:
 return(void*)& Cyc_Tcpat_EqNull_val;case 10: _LLDC: _tmp1BA=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp192)->f1;_tmp1B9=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp192)->f2;_LLDD:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp194=_cycalloc_atomic(sizeof(*_tmp194));({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp4EB=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp195;_tmp195.tag=6;_tmp195.f1=(unsigned int)_tmp1B9;_tmp195;});_tmp194[0]=_tmp4EB;});_tmp194;});case 11: _LLDE: _tmp1BB=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp192)->f1;_LLDF:
 return(void*)({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*_tmp196=_cycalloc_atomic(sizeof(*_tmp196));({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp4EC=({struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct _tmp197;_tmp197.tag=6;_tmp197.f1=(unsigned int)_tmp1BB;_tmp197;});_tmp196[0]=_tmp4EC;});_tmp196;});case 12: _LLE0: _tmp1BD=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp192)->f1;_tmp1BC=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp192)->f2;_LLE1:
 return(void*)({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*_tmp198=_cycalloc(sizeof(*_tmp198));({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct _tmp4ED=({struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct _tmp199;_tmp199.tag=5;_tmp199.f1=_tmp1BD;_tmp199.f2=_tmp1BC;_tmp199;});_tmp198[0]=_tmp4ED;});_tmp198;});case 13: _LLE2: _tmp1BF=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp192)->f1;_tmp1BE=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp192)->f2;_LLE3:
 return(void*)({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct _tmp4EE=({struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct _tmp19B;_tmp19B.tag=3;_tmp19B.f1=_tmp1BF;_tmp19B.f2=_tmp1BE;_tmp19B;});_tmp19A[0]=_tmp4EE;});_tmp19A;});case 14: _LLE4: _tmp1C1=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp192)->f1;_tmp1C0=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp192)->f2;_LLE5:
 return(void*)({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*_tmp19C=_cycalloc(sizeof(*_tmp19C));({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct _tmp4EF=({struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct _tmp19D;_tmp19D.tag=4;_tmp19D.f1=_tmp1C1;_tmp19D.f2=_tmp1C0;_tmp19D;});_tmp19C[0]=_tmp4EF;});_tmp19C;});case 6: _LLE6: _LLE7:
# 941
{void*_tmp19E=Cyc_Tcutil_compress((void*)_check_null(_tmp1C7->topt));void*_tmp19F=_tmp19E;union Cyc_Absyn_Constraint*_tmp1A0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19F)->tag == 5){_LLEF: _tmp1A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp19F)->f1).ptr_atts).nullable;_LLF0:
# 943
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp1A0))
return(void*)& Cyc_Tcpat_NeqNull_val;
goto _LLEE;}else{_LLF1: _LLF2:
 goto _LLEE;}_LLEE:;}
# 948
({void*_tmp1A1=0;({struct _dyneither_ptr _tmp4F0=({const char*_tmp1A2="non-null pointer type or non-pointer type in pointer pattern";_tag_dyneither(_tmp1A2,sizeof(char),61);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F0,_tag_dyneither(_tmp1A1,sizeof(void*),0));});});case 8: _LLE8: _tmp1C3=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp192)->f1;_tmp1C2=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp192)->f2;_LLE9:
# 950
 if(_tmp1C3->is_extensible)
return(void*)({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct _tmp4F1=({struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct _tmp1A4;_tmp1A4.tag=9;_tmp1A4.f1=_tmp1C3;_tmp1A4.f2=_tmp1C2;_tmp1A4;});_tmp1A3[0]=_tmp4F1;});_tmp1A3;});else{
# 953
return(void*)({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct _tmp4F3=({struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct _tmp1A6;_tmp1A6.tag=7;({int _tmp4F2=(int)Cyc_Tcpat_datatype_tag_number(_tmp1C3,_tmp1C2->name);_tmp1A6.f1=_tmp4F2;});_tmp1A6.f2=_tmp1C3;_tmp1A6.f3=_tmp1C2;_tmp1A6;});_tmp1A5[0]=_tmp4F3;});_tmp1A5;});}case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp192)->f1 != 0){_LLEA: _tmp1C5=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp192)->f1)->aggr_info;_tmp1C4=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp192)->f3;_LLEB: {
# 955
struct Cyc_Absyn_Aggrdecl*_tmp1A7=Cyc_Absyn_get_known_aggrdecl(_tmp1C5);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1A7->impl))->tagged){
struct _tuple18*_tmp1A8=(struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp1C4))->hd;struct _tuple18*_tmp1A9=_tmp1A8;struct Cyc_List_List*_tmp1B2;struct Cyc_Absyn_Pat*_tmp1B1;_LLF4: _tmp1B2=_tmp1A9->f1;_tmp1B1=_tmp1A9->f2;_LLF5:;{
struct _dyneither_ptr*f;
{void*_tmp1AA=(void*)((struct Cyc_List_List*)_check_null(_tmp1B2))->hd;void*_tmp1AB=_tmp1AA;struct _dyneither_ptr*_tmp1AE;if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1AB)->tag == 1){_LLF7: _tmp1AE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1AB)->f1;_LLF8:
 f=_tmp1AE;goto _LLF6;}else{_LLF9: _LLFA:
({void*_tmp1AC=0;({struct _dyneither_ptr _tmp4F4=({const char*_tmp1AD="no field name in tagged union pattern";_tag_dyneither(_tmp1AD,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F4,_tag_dyneither(_tmp1AC,sizeof(void*),0));});});}_LLF6:;}
# 963
return(void*)({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*_tmp1AF=_cycalloc(sizeof(*_tmp1AF));({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct _tmp4F6=({struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct _tmp1B0;_tmp1B0.tag=8;_tmp1B0.f1=f;({int _tmp4F5=Cyc_Tcpat_get_member_offset(_tmp1A7,f);_tmp1B0.f2=_tmp4F5;});_tmp1B0;});_tmp1AF[0]=_tmp4F6;});_tmp1AF;});};}else{
# 965
({void*_tmp1B3=0;({struct _dyneither_ptr _tmp4F7=({const char*_tmp1B4="non-tagged aggregate in pattern test";_tag_dyneither(_tmp1B4,sizeof(char),37);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F7,_tag_dyneither(_tmp1B3,sizeof(void*),0));});});}}}else{goto _LLEC;}default: _LLEC: _LLED:
({void*_tmp1B5=0;({struct _dyneither_ptr _tmp4F8=({const char*_tmp1B6="non-test pattern in pattern test";_tag_dyneither(_tmp1B6,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp4F8,_tag_dyneither(_tmp1B5,sizeof(void*),0));});});}_LLD5:;}}_LLD0:;}
# 971
static union Cyc_Tcpat_PatOrWhere Cyc_Tcpat_pw(struct Cyc_Absyn_Pat*p){
return({union Cyc_Tcpat_PatOrWhere _tmp1C8;(_tmp1C8.pattern).val=p;(_tmp1C8.pattern).tag=1;_tmp1C8;});}
# 975
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1C9=_cycalloc(sizeof(*_tmp1C9));({union Cyc_Tcpat_Name_value _tmp4F9=Cyc_Tcpat_Name_v(({const char*_tmp1CA="NULL";_tag_dyneither(_tmp1CA,sizeof(char),5);}));_tmp1C9->name=_tmp4F9;});_tmp1C9->arity=0;_tmp1C9->span=& Cyc_Tcpat_two_opt;({union Cyc_Tcpat_PatOrWhere _tmp4FA=Cyc_Tcpat_pw(p);_tmp1C9->orig_pat=_tmp4FA;});_tmp1C9;});}
# 978
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_null_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1CB=_cycalloc(sizeof(*_tmp1CB));({union Cyc_Tcpat_Name_value _tmp4FB=Cyc_Tcpat_Name_v(({const char*_tmp1CC="&";_tag_dyneither(_tmp1CC,sizeof(char),2);}));_tmp1CB->name=_tmp4FB;});_tmp1CB->arity=1;_tmp1CB->span=& Cyc_Tcpat_two_opt;({union Cyc_Tcpat_PatOrWhere _tmp4FC=Cyc_Tcpat_pw(p);_tmp1CB->orig_pat=_tmp4FC;});_tmp1CB;});}
# 981
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_ptr_con(struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));({union Cyc_Tcpat_Name_value _tmp4FD=Cyc_Tcpat_Name_v(({const char*_tmp1CE="&";_tag_dyneither(_tmp1CE,sizeof(char),2);}));_tmp1CD->name=_tmp4FD;});_tmp1CD->arity=1;_tmp1CD->span=& Cyc_Tcpat_one_opt;({union Cyc_Tcpat_PatOrWhere _tmp4FE=Cyc_Tcpat_pw(p);_tmp1CD->orig_pat=_tmp4FE;});_tmp1CD;});}
# 984
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_int_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));({union Cyc_Tcpat_Name_value _tmp4FF=Cyc_Tcpat_Int_v(i);_tmp1CF->name=_tmp4FF;});_tmp1CF->arity=0;_tmp1CF->span=0;_tmp1CF->orig_pat=p;_tmp1CF;});}
# 987
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_float_con(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1D0=_cycalloc(sizeof(*_tmp1D0));({union Cyc_Tcpat_Name_value _tmp500=Cyc_Tcpat_Name_v(f);_tmp1D0->name=_tmp500;});_tmp1D0->arity=0;_tmp1D0->span=0;({union Cyc_Tcpat_PatOrWhere _tmp501=Cyc_Tcpat_pw(p);_tmp1D0->orig_pat=_tmp501;});_tmp1D0;});}
# 990
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_char_con(char c,struct Cyc_Absyn_Pat*p){
return({struct Cyc_Tcpat_Con_s*_tmp1D1=_cycalloc(sizeof(*_tmp1D1));({union Cyc_Tcpat_Name_value _tmp502=Cyc_Tcpat_Int_v((int)c);_tmp1D1->name=_tmp502;});_tmp1D1->arity=0;_tmp1D1->span=& Cyc_Tcpat_twofiftysix_opt;({union Cyc_Tcpat_PatOrWhere _tmp503=Cyc_Tcpat_pw(p);_tmp1D1->orig_pat=_tmp503;});_tmp1D1;});}
# 993
static struct Cyc_Tcpat_Con_s*Cyc_Tcpat_tuple_con(int i,union Cyc_Tcpat_PatOrWhere p){
return({struct Cyc_Tcpat_Con_s*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));({union Cyc_Tcpat_Name_value _tmp504=Cyc_Tcpat_Name_v(({const char*_tmp1D3="$";_tag_dyneither(_tmp1D3,sizeof(char),2);}));_tmp1D2->name=_tmp504;});_tmp1D2->arity=i;_tmp1D2->span=& Cyc_Tcpat_one_opt;_tmp1D2->orig_pat=p;_tmp1D2;});}
# 998
static void*Cyc_Tcpat_null_pat(struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp506=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1D5;_tmp1D5.tag=1;({struct Cyc_Tcpat_Con_s*_tmp505=Cyc_Tcpat_null_con(p);_tmp1D5.f1=_tmp505;});_tmp1D5.f2=0;_tmp1D5;});_tmp1D4[0]=_tmp506;});_tmp1D4;});}
# 1001
static void*Cyc_Tcpat_int_pat(int i,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp508=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1D7;_tmp1D7.tag=1;({struct Cyc_Tcpat_Con_s*_tmp507=Cyc_Tcpat_int_con(i,p);_tmp1D7.f1=_tmp507;});_tmp1D7.f2=0;_tmp1D7;});_tmp1D6[0]=_tmp508;});_tmp1D6;});}
# 1004
static void*Cyc_Tcpat_char_pat(char c,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp50A=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1D9;_tmp1D9.tag=1;({struct Cyc_Tcpat_Con_s*_tmp509=Cyc_Tcpat_char_con(c,p);_tmp1D9.f1=_tmp509;});_tmp1D9.f2=0;_tmp1D9;});_tmp1D8[0]=_tmp50A;});_tmp1D8;});}
# 1007
static void*Cyc_Tcpat_float_pat(struct _dyneither_ptr f,struct Cyc_Absyn_Pat*p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp50C=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1DB;_tmp1DB.tag=1;({struct Cyc_Tcpat_Con_s*_tmp50B=Cyc_Tcpat_float_con(f,p);_tmp1DB.f1=_tmp50B;});_tmp1DB.f2=0;_tmp1DB;});_tmp1DA[0]=_tmp50C;});_tmp1DA;});}
# 1010
static void*Cyc_Tcpat_null_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp50F=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1DD;_tmp1DD.tag=1;({struct Cyc_Tcpat_Con_s*_tmp50D=Cyc_Tcpat_null_ptr_con(p0);_tmp1DD.f1=_tmp50D;});({struct Cyc_List_List*_tmp50E=({struct Cyc_List_List*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->hd=p;_tmp1DE->tl=0;_tmp1DE;});_tmp1DD.f2=_tmp50E;});_tmp1DD;});_tmp1DC[0]=_tmp50F;});_tmp1DC;});}
# 1013
static void*Cyc_Tcpat_ptr_pat(void*p,struct Cyc_Absyn_Pat*p0){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1DF=_cycalloc(sizeof(*_tmp1DF));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp512=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1E0;_tmp1E0.tag=1;({struct Cyc_Tcpat_Con_s*_tmp510=Cyc_Tcpat_ptr_con(p0);_tmp1E0.f1=_tmp510;});({struct Cyc_List_List*_tmp511=({struct Cyc_List_List*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->hd=p;_tmp1E1->tl=0;_tmp1E1;});_tmp1E0.f2=_tmp511;});_tmp1E0;});_tmp1DF[0]=_tmp512;});_tmp1DF;});}
# 1016
static void*Cyc_Tcpat_tuple_pat(struct Cyc_List_List*ss,union Cyc_Tcpat_PatOrWhere p){
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1E2=_cycalloc(sizeof(*_tmp1E2));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp515=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1E3;_tmp1E3.tag=1;({struct Cyc_Tcpat_Con_s*_tmp514=({int _tmp513=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ss);Cyc_Tcpat_tuple_con(_tmp513,p);});_tmp1E3.f1=_tmp514;});_tmp1E3.f2=ss;_tmp1E3;});_tmp1E2[0]=_tmp515;});_tmp1E2;});}
# 1019
static void*Cyc_Tcpat_con_pat(struct _dyneither_ptr con_name,int*span,struct Cyc_List_List*ps,struct Cyc_Absyn_Pat*p){
# 1021
struct Cyc_Tcpat_Con_s*c=({struct Cyc_Tcpat_Con_s*_tmp1E6=_cycalloc(sizeof(*_tmp1E6));({union Cyc_Tcpat_Name_value _tmp516=Cyc_Tcpat_Name_v(con_name);_tmp1E6->name=_tmp516;});({int _tmp517=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ps);_tmp1E6->arity=_tmp517;});_tmp1E6->span=span;({union Cyc_Tcpat_PatOrWhere _tmp518=Cyc_Tcpat_pw(p);_tmp1E6->orig_pat=_tmp518;});_tmp1E6;});
return(void*)({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp519=({struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct _tmp1E5;_tmp1E5.tag=1;_tmp1E5.f1=c;_tmp1E5.f2=ps;_tmp1E5;});_tmp1E4[0]=_tmp519;});_tmp1E4;});}
# 1026
static void*Cyc_Tcpat_compile_pat(struct Cyc_Absyn_Pat*p){
void*s;
{void*_tmp1E7=p->r;void*_tmp1E8=_tmp1E7;void*_tmp22B;struct Cyc_Absyn_Enumfield*_tmp22A;struct Cyc_Absyn_Enumdecl*_tmp229;struct Cyc_Absyn_Enumfield*_tmp228;struct Cyc_Absyn_Aggrdecl*_tmp227;struct Cyc_List_List*_tmp226;struct Cyc_List_List*_tmp225;struct Cyc_Absyn_Datatypedecl*_tmp224;struct Cyc_Absyn_Datatypefield*_tmp223;struct Cyc_List_List*_tmp222;struct Cyc_Absyn_Pat*_tmp221;struct Cyc_Absyn_Pat*_tmp220;struct Cyc_Absyn_Pat*_tmp21F;struct _dyneither_ptr _tmp21E;char _tmp21D;enum Cyc_Absyn_Sign _tmp21C;int _tmp21B;switch(*((int*)_tmp1E8)){case 0: _LLFC: _LLFD:
 goto _LLFF;case 2: _LLFE: _LLFF:
 goto _LL101;case 4: _LL100: _LL101:
({void*_tmp51B=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp1E9=_cycalloc_atomic(sizeof(*_tmp1E9));({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp51A=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp1EA;_tmp1EA.tag=0;_tmp1EA;});_tmp1E9[0]=_tmp51A;});_tmp1E9;});s=_tmp51B;});goto _LLFB;case 9: _LL102: _LL103:
({void*_tmp51C=Cyc_Tcpat_null_pat(p);s=_tmp51C;});goto _LLFB;case 10: _LL104: _tmp21C=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1;_tmp21B=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp1E8)->f2;_LL105:
({void*_tmp51E=({int _tmp51D=_tmp21B;Cyc_Tcpat_int_pat(_tmp51D,Cyc_Tcpat_pw(p));});s=_tmp51E;});goto _LLFB;case 11: _LL106: _tmp21D=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1;_LL107:
({void*_tmp51F=Cyc_Tcpat_char_pat(_tmp21D,p);s=_tmp51F;});goto _LLFB;case 12: _LL108: _tmp21E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1;_LL109:
({void*_tmp520=Cyc_Tcpat_float_pat(_tmp21E,p);s=_tmp520;});goto _LLFB;case 1: _LL10A: _tmp21F=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1E8)->f2;_LL10B:
({void*_tmp521=Cyc_Tcpat_compile_pat(_tmp21F);s=_tmp521;});goto _LLFB;case 3: _LL10C: _tmp220=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1E8)->f2;_LL10D:
({void*_tmp522=Cyc_Tcpat_compile_pat(_tmp220);s=_tmp522;});goto _LLFB;case 6: _LL10E: _tmp221=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1;_LL10F:
# 1039
{void*_tmp1EB=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1EC=_tmp1EB;union Cyc_Absyn_Constraint*_tmp1F3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->tag == 5){_LL123: _tmp1F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EC)->f1).ptr_atts).nullable;_LL124: {
# 1041
int is_nullable=0;
int still_working=1;
while(still_working){
union Cyc_Absyn_Constraint*_tmp1ED=_tmp1F3;int _tmp1F0;union Cyc_Absyn_Constraint*_tmp1EF;switch((((union Cyc_Absyn_Constraint*)_tmp1ED)->No_constr).tag){case 2: _LL128: _tmp1EF=(_tmp1ED->Forward_constr).val;_LL129:
# 1046
*_tmp1F3=*_tmp1EF;
continue;case 3: _LL12A: _LL12B:
# 1049
({struct _union_Constraint_Eq_constr*_tmp1EE=& _tmp1F3->Eq_constr;_tmp1EE->tag=1;_tmp1EE->val=0;});
is_nullable=0;
still_working=0;
goto _LL127;default: _LL12C: _tmp1F0=(int)(_tmp1ED->Eq_constr).val;_LL12D:
# 1054
 is_nullable=_tmp1F0;
still_working=0;
goto _LL127;}_LL127:;}{
# 1059
void*ss=Cyc_Tcpat_compile_pat(_tmp221);
if(is_nullable)
({void*_tmp523=Cyc_Tcpat_null_ptr_pat(ss,p);s=_tmp523;});else{
# 1063
({void*_tmp524=Cyc_Tcpat_ptr_pat(ss,p);s=_tmp524;});}
goto _LL122;};}}else{_LL125: _LL126:
({void*_tmp1F1=0;({struct _dyneither_ptr _tmp525=({const char*_tmp1F2="expecting pointertype for pattern!";_tag_dyneither(_tmp1F2,sizeof(char),35);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp525,_tag_dyneither(_tmp1F1,sizeof(void*),0));});});}_LL122:;}
# 1067
goto _LLFB;case 8: _LL110: _tmp224=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1;_tmp223=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E8)->f2;_tmp222=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1E8)->f3;_LL111: {
# 1069
int*span;
{void*_tmp1F4=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp1F5=_tmp1F4;switch(*((int*)_tmp1F5)){case 3: _LL12F: _LL130:
# 1072
 if(_tmp224->is_extensible)
span=0;else{
# 1075
({int*_tmp527=({int*_tmp1F6=_cycalloc_atomic(sizeof(*_tmp1F6));({int _tmp526=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp224->fields))->v);_tmp1F6[0]=_tmp526;});_tmp1F6;});span=_tmp527;});}
goto _LL12E;case 4: _LL131: _LL132:
 span=& Cyc_Tcpat_one_opt;goto _LL12E;default: _LL133: _LL134:
({int*_tmp529=({void*_tmp1F7=0;({struct _dyneither_ptr _tmp528=({const char*_tmp1F8="void datatype pattern has bad type";_tag_dyneither(_tmp1F8,sizeof(char),35);});((int*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp528,_tag_dyneither(_tmp1F7,sizeof(void*),0));});});span=_tmp529;});goto _LL12E;}_LL12E:;}
# 1080
({void*_tmp52D=({struct _dyneither_ptr _tmp52C=*(*_tmp223->name).f2;int*_tmp52B=span;struct Cyc_List_List*_tmp52A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp222);Cyc_Tcpat_con_pat(_tmp52C,_tmp52B,_tmp52A,p);});s=_tmp52D;});
goto _LLFB;}case 5: _LL112: _tmp225=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1;_LL113:
# 1084
({void*_tmp52F=({struct Cyc_List_List*_tmp52E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Pat*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_compile_pat,_tmp225);Cyc_Tcpat_tuple_pat(_tmp52E,Cyc_Tcpat_pw(p));});s=_tmp52F;});goto _LLFB;case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1)->aggr_info).KnownAggr).tag == 2){_LL114: _tmp227=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1)->aggr_info).KnownAggr).val;_tmp226=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1E8)->f3;_LL115:
# 1089
 if(_tmp227->kind == Cyc_Absyn_StructA){
struct Cyc_List_List*ps=0;
{struct Cyc_List_List*fields=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp227->impl))->fields;for(0;fields != 0;fields=fields->tl){
# 1093
int found=({({struct _dyneither_ptr _tmp530=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)fields->hd)->name;Cyc_strcmp(_tmp530,({const char*_tmp204="";_tag_dyneither(_tmp204,sizeof(char),1);}));})== 0;});
{struct Cyc_List_List*dlps0=_tmp226;for(0;!found  && dlps0 != 0;dlps0=dlps0->tl){
struct _tuple18*_tmp1F9=(struct _tuple18*)dlps0->hd;struct _tuple18*_tmp1FA=_tmp1F9;struct Cyc_List_List*_tmp201;struct Cyc_Absyn_Pat*_tmp200;_LL136: _tmp201=_tmp1FA->f1;_tmp200=_tmp1FA->f2;_LL137:;{
struct Cyc_List_List*_tmp1FB=_tmp201;struct _dyneither_ptr*_tmp1FF;if(_tmp1FB != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp1FB)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp1FB)->tl == 0){_LL139: _tmp1FF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp1FB->hd)->f1;_LL13A:
# 1098
 if(({Cyc_strptrcmp(_tmp1FF,((struct Cyc_Absyn_Aggrfield*)fields->hd)->name)== 0;})){
({struct Cyc_List_List*_tmp532=({struct Cyc_List_List*_tmp1FC=_cycalloc(sizeof(*_tmp1FC));({void*_tmp531=Cyc_Tcpat_compile_pat(_tmp200);_tmp1FC->hd=_tmp531;});_tmp1FC->tl=ps;_tmp1FC;});ps=_tmp532;});
found=1;}
# 1102
goto _LL138;}else{goto _LL13B;}}else{goto _LL13B;}}else{_LL13B: _LL13C:
({void*_tmp1FD=0;({struct _dyneither_ptr _tmp533=({const char*_tmp1FE="bad designator(s)";_tag_dyneither(_tmp1FE,sizeof(char),18);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp533,_tag_dyneither(_tmp1FD,sizeof(void*),0));});});}_LL138:;};}}
# 1106
if(!found)
({void*_tmp202=0;({struct _dyneither_ptr _tmp534=({const char*_tmp203="bad designator";_tag_dyneither(_tmp203,sizeof(char),15);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp534,_tag_dyneither(_tmp202,sizeof(void*),0));});});}}
# 1109
({void*_tmp536=({struct Cyc_List_List*_tmp535=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ps);Cyc_Tcpat_tuple_pat(_tmp535,Cyc_Tcpat_pw(p));});s=_tmp536;});}else{
# 1112
if(!((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp227->impl))->tagged)
({void*_tmp205=0;({unsigned int _tmp538=p->loc;struct _dyneither_ptr _tmp537=({const char*_tmp206="patterns on untagged unions not yet supported.";_tag_dyneither(_tmp206,sizeof(char),47);});Cyc_Tcutil_terr(_tmp538,_tmp537,_tag_dyneither(_tmp205,sizeof(void*),0));});});{
int*span=({int*_tmp20D=_cycalloc_atomic(sizeof(*_tmp20D));({int _tmp539=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp227->impl))->fields);_tmp20D[0]=_tmp539;});_tmp20D;});
struct Cyc_List_List*_tmp207=_tmp226;struct _dyneither_ptr*_tmp20C;struct Cyc_Absyn_Pat*_tmp20B;if(_tmp207 != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp207)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp207)->hd)->f1)->hd)->tag == 1){if(((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp207)->hd)->f1)->tl == 0){if(((struct Cyc_List_List*)_tmp207)->tl == 0){_LL13E: _tmp20C=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp207->hd)->f1)->hd)->f1;_tmp20B=((struct _tuple18*)_tmp207->hd)->f2;_LL13F:
# 1117
({void*_tmp53E=({struct _dyneither_ptr _tmp53D=*_tmp20C;int*_tmp53C=span;struct Cyc_List_List*_tmp53B=({struct Cyc_List_List*_tmp208=_cycalloc(sizeof(*_tmp208));({void*_tmp53A=Cyc_Tcpat_compile_pat(_tmp20B);_tmp208->hd=_tmp53A;});_tmp208->tl=0;_tmp208;});Cyc_Tcpat_con_pat(_tmp53D,_tmp53C,_tmp53B,p);});s=_tmp53E;});
goto _LL13D;}else{goto _LL140;}}else{goto _LL140;}}else{goto _LL140;}}else{goto _LL140;}}else{_LL140: _LL141:
({void*_tmp209=0;({struct _dyneither_ptr _tmp53F=({const char*_tmp20A="bad union pattern";_tag_dyneither(_tmp20A,sizeof(char),18);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp53F,_tag_dyneither(_tmp209,sizeof(void*),0));});});}_LL13D:;};}
# 1122
goto _LLFB;}else{goto _LL11E;}}else{_LL11E: _LL11F:
# 1164
 goto _LL121;}case 13: _LL116: _tmp229=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1;_tmp228=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp1E8)->f2;_LL117:
# 1127
{void*_tmp20E=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp20F=_tmp20E;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp20F)->tag == 6){_LL143: _LL144:
# 1131
({void*_tmp540=Cyc_Tcpat_con_pat(*(*_tmp228->name).f2,0,0,p);s=_tmp540;});
goto _LL142;}else{_LL145: _LL146: {
# 1134
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp229->fields))->v);
({void*_tmp543=({struct _dyneither_ptr _tmp542=*(*_tmp228->name).f2;int*_tmp541=({int*_tmp210=_cycalloc_atomic(sizeof(*_tmp210));_tmp210[0]=span;_tmp210;});Cyc_Tcpat_con_pat(_tmp542,_tmp541,0,p);});s=_tmp543;});
goto _LL142;}}_LL142:;}
# 1138
goto _LLFB;case 14: _LL118: _tmp22B=(void*)((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1E8)->f1;_tmp22A=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp1E8)->f2;_LL119: {
# 1143
struct Cyc_List_List*fields;
{void*_tmp211=Cyc_Tcutil_compress(_tmp22B);void*_tmp212=_tmp211;struct Cyc_List_List*_tmp215;if(((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp212)->tag == 14){_LL148: _tmp215=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp212)->f1;_LL149:
 fields=_tmp215;goto _LL147;}else{_LL14A: _LL14B:
({void*_tmp213=0;({struct _dyneither_ptr _tmp544=({const char*_tmp214="bad type in AnonEnum_p";_tag_dyneither(_tmp214,sizeof(char),23);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp544,_tag_dyneither(_tmp213,sizeof(void*),0));});});}_LL147:;}
# 1150
{void*_tmp216=Cyc_Tcutil_compress((void*)_check_null(p->topt));void*_tmp217=_tmp216;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp217)->tag == 6){_LL14D: _LL14E:
# 1154
({void*_tmp545=Cyc_Tcpat_con_pat(*(*_tmp22A->name).f2,0,0,p);s=_tmp545;});
goto _LL14C;}else{_LL14F: _LL150: {
# 1157
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);
({void*_tmp548=({struct _dyneither_ptr _tmp547=*(*_tmp22A->name).f2;int*_tmp546=({int*_tmp218=_cycalloc_atomic(sizeof(*_tmp218));_tmp218[0]=span;_tmp218;});Cyc_Tcpat_con_pat(_tmp547,_tmp546,0,p);});s=_tmp548;});
goto _LL14C;}}_LL14C:;}
# 1161
goto _LLFB;}case 15: _LL11A: _LL11B:
 goto _LL11D;case 16: _LL11C: _LL11D:
 goto _LL11F;default: _LL120: _LL121:
# 1165
({void*_tmp54A=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp219=_cycalloc_atomic(sizeof(*_tmp219));({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp549=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp21A;_tmp21A.tag=0;_tmp21A;});_tmp219[0]=_tmp549;});_tmp219;});s=_tmp54A;});}_LLFB:;}
# 1167
return s;}
# 1175
typedef struct Cyc_List_List*Cyc_Tcpat_match_t;struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct{int tag;struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct{int tag;struct Cyc_Set_Set*f1;};
# 1189
typedef struct Cyc_List_List*Cyc_Tcpat_context_t;
# 1196
static int Cyc_Tcpat_same_access(void*a1,void*a2){
struct _tuple0 _tmp22C=({struct _tuple0 _tmp238;_tmp238.f1=a1;_tmp238.f2=a2;_tmp238;});struct _tuple0 _tmp22D=_tmp22C;int _tmp237;struct _dyneither_ptr*_tmp236;int _tmp235;struct _dyneither_ptr*_tmp234;struct Cyc_Absyn_Datatypefield*_tmp233;unsigned int _tmp232;struct Cyc_Absyn_Datatypefield*_tmp231;unsigned int _tmp230;unsigned int _tmp22F;unsigned int _tmp22E;switch(*((int*)_tmp22D.f1)){case 0: if(((struct Cyc_Tcpat_Dummy_Tcpat_Access_struct*)_tmp22D.f2)->tag == 0){_LL152: _LL153:
 return 1;}else{goto _LL15C;}case 1: if(((struct Cyc_Tcpat_Deref_Tcpat_Access_struct*)_tmp22D.f2)->tag == 1){_LL154: _LL155:
 return 1;}else{goto _LL15C;}case 2: if(((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp22D.f2)->tag == 2){_LL156: _tmp22F=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp22D.f1)->f1;_tmp22E=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp22D.f2)->f1;_LL157:
 return _tmp22F == _tmp22E;}else{goto _LL15C;}case 3: if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp22D.f2)->tag == 3){_LL158: _tmp233=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp22D.f1)->f2;_tmp232=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp22D.f1)->f3;_tmp231=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp22D.f2)->f2;_tmp230=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp22D.f2)->f3;_LL159:
# 1202
 return _tmp233 == _tmp231  && _tmp232 == _tmp230;}else{goto _LL15C;}default: if(((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp22D.f2)->tag == 4){_LL15A: _tmp237=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp22D.f1)->f1;_tmp236=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp22D.f1)->f2;_tmp235=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp22D.f2)->f1;_tmp234=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp22D.f2)->f2;_LL15B:
# 1204
 return _tmp237 == _tmp235  && ({Cyc_strptrcmp(_tmp236,_tmp234)== 0;});}else{_LL15C: _LL15D:
 return 0;}}_LL151:;}
# 1209
static int Cyc_Tcpat_same_path(struct Cyc_List_List*p1,struct Cyc_List_List*p2){
while(p1 != 0  && p2 != 0){
if(!Cyc_Tcpat_same_access(((struct Cyc_Tcpat_PathNode*)p1->hd)->access,((struct Cyc_Tcpat_PathNode*)p2->hd)->access))return 0;
p1=p1->tl;
p2=p2->tl;}
# 1215
if(p1 != p2)return 0;
return 1;}
# 1219
static void*Cyc_Tcpat_ifeq(struct Cyc_List_List*access,struct Cyc_Tcpat_Con_s*con,void*d1,void*d2){
void*_tmp239=Cyc_Tcpat_get_pat_test(con->orig_pat);
{void*_tmp23A=d2;struct Cyc_List_List*_tmp241;struct Cyc_List_List*_tmp240;void*_tmp23F;if(((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23A)->tag == 2){_LL15F: _tmp241=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23A)->f1;_tmp240=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23A)->f2;_tmp23F=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp23A)->f3;_LL160:
# 1223
 if(Cyc_Tcpat_same_path(access,_tmp241) && (int)(((con->orig_pat).pattern).tag == 1))
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp23B=_cycalloc(sizeof(*_tmp23B));({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp54D=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp23C;_tmp23C.tag=2;_tmp23C.f1=_tmp241;({struct Cyc_List_List*_tmp54C=({struct Cyc_List_List*_tmp23D=_cycalloc(sizeof(*_tmp23D));({struct _tuple0*_tmp54B=({struct _tuple0*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E->f1=_tmp239;_tmp23E->f2=d1;_tmp23E;});_tmp23D->hd=_tmp54B;});_tmp23D->tl=_tmp240;_tmp23D;});_tmp23C.f2=_tmp54C;});_tmp23C.f3=_tmp23F;_tmp23C;});_tmp23B[0]=_tmp54D;});_tmp23B;});else{
# 1227
goto _LL15E;}}else{_LL161: _LL162:
 goto _LL15E;}_LL15E:;}
# 1230
return(void*)({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*_tmp242=_cycalloc(sizeof(*_tmp242));({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp550=({struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct _tmp243;_tmp243.tag=2;_tmp243.f1=access;({struct Cyc_List_List*_tmp54F=({struct Cyc_List_List*_tmp244=_cycalloc(sizeof(*_tmp244));({struct _tuple0*_tmp54E=({struct _tuple0*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245->f1=_tmp239;_tmp245->f2=d1;_tmp245;});_tmp244->hd=_tmp54E;});_tmp244->tl=0;_tmp244;});_tmp243.f2=_tmp54F;});_tmp243.f3=d2;_tmp243;});_tmp242[0]=_tmp550;});_tmp242;});}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 1236
typedef struct _tuple20*Cyc_Tcpat_work_stack_frame_t;
# 1239
typedef struct Cyc_List_List*Cyc_Tcpat_work_stack_t;
# 1241
enum Cyc_Tcpat_Answer{Cyc_Tcpat_Yes  = 0,Cyc_Tcpat_No  = 1,Cyc_Tcpat_Maybe  = 2};
typedef enum Cyc_Tcpat_Answer Cyc_Tcpat_answer_t;
# 1244
static void Cyc_Tcpat_print_tab(int i){
for(0;i != 0;-- i){
({void*_tmp246=0;({struct Cyc___cycFILE*_tmp552=Cyc_stderr;struct _dyneither_ptr _tmp551=({const char*_tmp247=" ";_tag_dyneither(_tmp247,sizeof(char),2);});Cyc_fprintf(_tmp552,_tmp551,_tag_dyneither(_tmp246,sizeof(void*),0));});});}}
# 1250
static void Cyc_Tcpat_print_con(struct Cyc_Tcpat_Con_s*c){
union Cyc_Tcpat_Name_value _tmp248=c->name;
union Cyc_Tcpat_Name_value _tmp249=_tmp248;int _tmp251;struct _dyneither_ptr _tmp250;if((_tmp249.Name_v).tag == 1){_LL164: _tmp250=(_tmp249.Name_v).val;_LL165:
({struct Cyc_String_pa_PrintArg_struct _tmp24C;_tmp24C.tag=0;_tmp24C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp250);({void*_tmp24A[1]={& _tmp24C};({struct Cyc___cycFILE*_tmp554=Cyc_stderr;struct _dyneither_ptr _tmp553=({const char*_tmp24B="%s";_tag_dyneither(_tmp24B,sizeof(char),3);});Cyc_fprintf(_tmp554,_tmp553,_tag_dyneither(_tmp24A,sizeof(void*),1));});});});goto _LL163;}else{_LL166: _tmp251=(_tmp249.Int_v).val;_LL167:
({struct Cyc_Int_pa_PrintArg_struct _tmp24F;_tmp24F.tag=1;_tmp24F.f1=(unsigned long)_tmp251;({void*_tmp24D[1]={& _tmp24F};({struct Cyc___cycFILE*_tmp556=Cyc_stderr;struct _dyneither_ptr _tmp555=({const char*_tmp24E="%d";_tag_dyneither(_tmp24E,sizeof(char),3);});Cyc_fprintf(_tmp556,_tmp555,_tag_dyneither(_tmp24D,sizeof(void*),1));});});});goto _LL163;}_LL163:;}
# 1258
static void Cyc_Tcpat_print_access(void*a){
void*_tmp252=a;int _tmp268;struct _dyneither_ptr*_tmp267;struct Cyc_Absyn_Datatypefield*_tmp266;unsigned int _tmp265;unsigned int _tmp264;switch(*((int*)_tmp252)){case 0: _LL169: _LL16A:
({void*_tmp253=0;({struct Cyc___cycFILE*_tmp558=Cyc_stderr;struct _dyneither_ptr _tmp557=({const char*_tmp254="DUMMY";_tag_dyneither(_tmp254,sizeof(char),6);});Cyc_fprintf(_tmp558,_tmp557,_tag_dyneither(_tmp253,sizeof(void*),0));});});goto _LL168;case 1: _LL16B: _LL16C:
({void*_tmp255=0;({struct Cyc___cycFILE*_tmp55A=Cyc_stderr;struct _dyneither_ptr _tmp559=({const char*_tmp256="*";_tag_dyneither(_tmp256,sizeof(char),2);});Cyc_fprintf(_tmp55A,_tmp559,_tag_dyneither(_tmp255,sizeof(void*),0));});});goto _LL168;case 2: _LL16D: _tmp264=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp252)->f1;_LL16E:
({struct Cyc_Int_pa_PrintArg_struct _tmp259;_tmp259.tag=1;_tmp259.f1=(unsigned long)((int)_tmp264);({void*_tmp257[1]={& _tmp259};({struct Cyc___cycFILE*_tmp55C=Cyc_stderr;struct _dyneither_ptr _tmp55B=({const char*_tmp258="[%d]";_tag_dyneither(_tmp258,sizeof(char),5);});Cyc_fprintf(_tmp55C,_tmp55B,_tag_dyneither(_tmp257,sizeof(void*),1));});});});goto _LL168;case 3: _LL16F: _tmp266=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp252)->f2;_tmp265=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp252)->f3;_LL170:
# 1264
({struct Cyc_Int_pa_PrintArg_struct _tmp25D;_tmp25D.tag=1;_tmp25D.f1=(unsigned long)((int)_tmp265);({struct Cyc_String_pa_PrintArg_struct _tmp25C;_tmp25C.tag=0;({struct _dyneither_ptr _tmp55D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp266->name));_tmp25C.f1=_tmp55D;});({void*_tmp25A[2]={& _tmp25C,& _tmp25D};({struct Cyc___cycFILE*_tmp55F=Cyc_stderr;struct _dyneither_ptr _tmp55E=({const char*_tmp25B="%s[%d]";_tag_dyneither(_tmp25B,sizeof(char),7);});Cyc_fprintf(_tmp55F,_tmp55E,_tag_dyneither(_tmp25A,sizeof(void*),2));});});});});goto _LL168;default: _LL171: _tmp268=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp252)->f1;_tmp267=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp252)->f2;_LL172:
# 1266
 if(_tmp268)
({struct Cyc_String_pa_PrintArg_struct _tmp260;_tmp260.tag=0;_tmp260.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp267);({void*_tmp25E[1]={& _tmp260};({struct Cyc___cycFILE*_tmp561=Cyc_stderr;struct _dyneither_ptr _tmp560=({const char*_tmp25F=".tagunion.%s";_tag_dyneither(_tmp25F,sizeof(char),13);});Cyc_fprintf(_tmp561,_tmp560,_tag_dyneither(_tmp25E,sizeof(void*),1));});});});else{
# 1269
({struct Cyc_String_pa_PrintArg_struct _tmp263;_tmp263.tag=0;_tmp263.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp267);({void*_tmp261[1]={& _tmp263};({struct Cyc___cycFILE*_tmp563=Cyc_stderr;struct _dyneither_ptr _tmp562=({const char*_tmp262=".%s";_tag_dyneither(_tmp262,sizeof(char),4);});Cyc_fprintf(_tmp563,_tmp562,_tag_dyneither(_tmp261,sizeof(void*),1));});});});}
goto _LL168;}_LL168:;}
# 1274
static void Cyc_Tcpat_print_pat_test(void*p){
void*_tmp269=p;struct Cyc_Absyn_Datatypefield*_tmp28E;struct _dyneither_ptr*_tmp28D;int _tmp28C;int _tmp28B;unsigned int _tmp28A;struct _dyneither_ptr _tmp289;struct Cyc_Absyn_Enumfield*_tmp288;struct Cyc_Absyn_Enumfield*_tmp287;struct Cyc_Absyn_Exp*_tmp286;switch(*((int*)_tmp269)){case 0: if(((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp269)->f1 == 0){_LL174: _LL175:
({void*_tmp26A=0;({struct Cyc___cycFILE*_tmp565=Cyc_stderr;struct _dyneither_ptr _tmp564=({const char*_tmp26B="where(NULL)";_tag_dyneither(_tmp26B,sizeof(char),12);});Cyc_fprintf(_tmp565,_tmp564,_tag_dyneither(_tmp26A,sizeof(void*),0));});});goto _LL173;}else{_LL176: _tmp286=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp269)->f1;_LL177:
({struct Cyc_String_pa_PrintArg_struct _tmp26E;_tmp26E.tag=0;({struct _dyneither_ptr _tmp566=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string((struct Cyc_Absyn_Exp*)_check_null(_tmp286)));_tmp26E.f1=_tmp566;});({void*_tmp26C[1]={& _tmp26E};({struct Cyc___cycFILE*_tmp568=Cyc_stderr;struct _dyneither_ptr _tmp567=({const char*_tmp26D="where(%s)";_tag_dyneither(_tmp26D,sizeof(char),10);});Cyc_fprintf(_tmp568,_tmp567,_tag_dyneither(_tmp26C,sizeof(void*),1));});});});goto _LL173;}case 1: _LL178: _LL179:
({void*_tmp26F=0;({struct Cyc___cycFILE*_tmp56A=Cyc_stderr;struct _dyneither_ptr _tmp569=({const char*_tmp270="NULL";_tag_dyneither(_tmp270,sizeof(char),5);});Cyc_fprintf(_tmp56A,_tmp569,_tag_dyneither(_tmp26F,sizeof(void*),0));});});goto _LL173;case 2: _LL17A: _LL17B:
({void*_tmp271=0;({struct Cyc___cycFILE*_tmp56C=Cyc_stderr;struct _dyneither_ptr _tmp56B=({const char*_tmp272="NOT-NULL:";_tag_dyneither(_tmp272,sizeof(char),10);});Cyc_fprintf(_tmp56C,_tmp56B,_tag_dyneither(_tmp271,sizeof(void*),0));});});goto _LL173;case 4: _LL17C: _tmp287=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp269)->f2;_LL17D:
 _tmp288=_tmp287;goto _LL17F;case 3: _LL17E: _tmp288=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp269)->f2;_LL17F:
({struct Cyc_String_pa_PrintArg_struct _tmp275;_tmp275.tag=0;({struct _dyneither_ptr _tmp56D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp288->name));_tmp275.f1=_tmp56D;});({void*_tmp273[1]={& _tmp275};({struct Cyc___cycFILE*_tmp56F=Cyc_stderr;struct _dyneither_ptr _tmp56E=({const char*_tmp274="%s";_tag_dyneither(_tmp274,sizeof(char),3);});Cyc_fprintf(_tmp56F,_tmp56E,_tag_dyneither(_tmp273,sizeof(void*),1));});});});goto _LL173;case 5: _LL180: _tmp289=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp269)->f1;_LL181:
({struct Cyc_String_pa_PrintArg_struct _tmp278;_tmp278.tag=0;_tmp278.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp289);({void*_tmp276[1]={& _tmp278};({struct Cyc___cycFILE*_tmp571=Cyc_stderr;struct _dyneither_ptr _tmp570=({const char*_tmp277="%s";_tag_dyneither(_tmp277,sizeof(char),3);});Cyc_fprintf(_tmp571,_tmp570,_tag_dyneither(_tmp276,sizeof(void*),1));});});});goto _LL173;case 6: _LL182: _tmp28A=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp269)->f1;_LL183:
({struct Cyc_Int_pa_PrintArg_struct _tmp27B;_tmp27B.tag=1;_tmp27B.f1=(unsigned long)((int)_tmp28A);({void*_tmp279[1]={& _tmp27B};({struct Cyc___cycFILE*_tmp573=Cyc_stderr;struct _dyneither_ptr _tmp572=({const char*_tmp27A="%d";_tag_dyneither(_tmp27A,sizeof(char),3);});Cyc_fprintf(_tmp573,_tmp572,_tag_dyneither(_tmp279,sizeof(void*),1));});});});goto _LL173;case 7: _LL184: _tmp28B=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp269)->f1;_LL185:
({struct Cyc_Int_pa_PrintArg_struct _tmp27E;_tmp27E.tag=1;_tmp27E.f1=(unsigned long)_tmp28B;({void*_tmp27C[1]={& _tmp27E};({struct Cyc___cycFILE*_tmp575=Cyc_stderr;struct _dyneither_ptr _tmp574=({const char*_tmp27D="datatypetag(%d)";_tag_dyneither(_tmp27D,sizeof(char),16);});Cyc_fprintf(_tmp575,_tmp574,_tag_dyneither(_tmp27C,sizeof(void*),1));});});});goto _LL173;case 8: _LL186: _tmp28D=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp269)->f1;_tmp28C=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp269)->f2;_LL187:
({struct Cyc_Int_pa_PrintArg_struct _tmp282;_tmp282.tag=1;_tmp282.f1=(unsigned long)_tmp28C;({struct Cyc_String_pa_PrintArg_struct _tmp281;_tmp281.tag=0;_tmp281.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp28D);({void*_tmp27F[2]={& _tmp281,& _tmp282};({struct Cyc___cycFILE*_tmp577=Cyc_stderr;struct _dyneither_ptr _tmp576=({const char*_tmp280="uniontag[%s](%d)";_tag_dyneither(_tmp280,sizeof(char),17);});Cyc_fprintf(_tmp577,_tmp576,_tag_dyneither(_tmp27F,sizeof(void*),2));});});});});goto _LL173;default: _LL188: _tmp28E=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp269)->f2;_LL189:
# 1287
({struct Cyc_String_pa_PrintArg_struct _tmp285;_tmp285.tag=0;({struct _dyneither_ptr _tmp578=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp28E->name));_tmp285.f1=_tmp578;});({void*_tmp283[1]={& _tmp285};({struct Cyc___cycFILE*_tmp57A=Cyc_stderr;struct _dyneither_ptr _tmp579=({const char*_tmp284="datatypefield(%s)";_tag_dyneither(_tmp284,sizeof(char),18);});Cyc_fprintf(_tmp57A,_tmp579,_tag_dyneither(_tmp283,sizeof(void*),1));});});});}_LL173:;}
# 1291
static void Cyc_Tcpat_print_rhs(struct Cyc_Tcpat_Rhs*r){
({struct Cyc_String_pa_PrintArg_struct _tmp291;_tmp291.tag=0;({struct _dyneither_ptr _tmp57B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(r->rhs));_tmp291.f1=_tmp57B;});({void*_tmp28F[1]={& _tmp291};({struct Cyc___cycFILE*_tmp57D=Cyc_stderr;struct _dyneither_ptr _tmp57C=({const char*_tmp290="%s";_tag_dyneither(_tmp290,sizeof(char),3);});Cyc_fprintf(_tmp57D,_tmp57C,_tag_dyneither(_tmp28F,sizeof(void*),1));});});});}
# 1295
static void Cyc_Tcpat_print_dec_tree(void*d,int tab){
void*_tmp292=d;struct Cyc_List_List*_tmp2AE;struct Cyc_List_List*_tmp2AD;void*_tmp2AC;struct Cyc_Tcpat_Rhs*_tmp2AB;switch(*((int*)_tmp292)){case 1: _LL18B: _tmp2AB=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp292)->f1;_LL18C:
# 1298
 Cyc_Tcpat_print_tab(tab);
({void*_tmp293=0;({struct Cyc___cycFILE*_tmp57F=Cyc_stderr;struct _dyneither_ptr _tmp57E=({const char*_tmp294="Success(";_tag_dyneither(_tmp294,sizeof(char),9);});Cyc_fprintf(_tmp57F,_tmp57E,_tag_dyneither(_tmp293,sizeof(void*),0));});});Cyc_Tcpat_print_rhs(_tmp2AB);({void*_tmp295=0;({struct Cyc___cycFILE*_tmp581=Cyc_stderr;struct _dyneither_ptr _tmp580=({const char*_tmp296=")\n";_tag_dyneither(_tmp296,sizeof(char),3);});Cyc_fprintf(_tmp581,_tmp580,_tag_dyneither(_tmp295,sizeof(void*),0));});});
goto _LL18A;case 0: _LL18D: _LL18E:
({void*_tmp297=0;({struct Cyc___cycFILE*_tmp583=Cyc_stderr;struct _dyneither_ptr _tmp582=({const char*_tmp298="Failure\n";_tag_dyneither(_tmp298,sizeof(char),9);});Cyc_fprintf(_tmp583,_tmp582,_tag_dyneither(_tmp297,sizeof(void*),0));});});goto _LL18A;default: _LL18F: _tmp2AE=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp292)->f1;_tmp2AD=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp292)->f2;_tmp2AC=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp292)->f3;_LL190:
# 1303
 Cyc_Tcpat_print_tab(tab);
({void*_tmp299=0;({struct Cyc___cycFILE*_tmp585=Cyc_stderr;struct _dyneither_ptr _tmp584=({const char*_tmp29A="Switch[";_tag_dyneither(_tmp29A,sizeof(char),8);});Cyc_fprintf(_tmp585,_tmp584,_tag_dyneither(_tmp299,sizeof(void*),0));});});
for(0;_tmp2AE != 0;_tmp2AE=_tmp2AE->tl){
Cyc_Tcpat_print_access(((struct Cyc_Tcpat_PathNode*)_tmp2AE->hd)->access);
if(_tmp2AE->tl != 0)({void*_tmp29B=0;({struct Cyc___cycFILE*_tmp587=Cyc_stderr;struct _dyneither_ptr _tmp586=({const char*_tmp29C=",";_tag_dyneither(_tmp29C,sizeof(char),2);});Cyc_fprintf(_tmp587,_tmp586,_tag_dyneither(_tmp29B,sizeof(void*),0));});});}
# 1309
({void*_tmp29D=0;({struct Cyc___cycFILE*_tmp589=Cyc_stderr;struct _dyneither_ptr _tmp588=({const char*_tmp29E="] {\n";_tag_dyneither(_tmp29E,sizeof(char),5);});Cyc_fprintf(_tmp589,_tmp588,_tag_dyneither(_tmp29D,sizeof(void*),0));});});
for(0;_tmp2AD != 0;_tmp2AD=_tmp2AD->tl){
struct _tuple0 _tmp29F=*((struct _tuple0*)_tmp2AD->hd);struct _tuple0 _tmp2A0=_tmp29F;void*_tmp2A6;void*_tmp2A5;_LL192: _tmp2A6=_tmp2A0.f1;_tmp2A5=_tmp2A0.f2;_LL193:;
Cyc_Tcpat_print_tab(tab);
({void*_tmp2A1=0;({struct Cyc___cycFILE*_tmp58B=Cyc_stderr;struct _dyneither_ptr _tmp58A=({const char*_tmp2A2="case ";_tag_dyneither(_tmp2A2,sizeof(char),6);});Cyc_fprintf(_tmp58B,_tmp58A,_tag_dyneither(_tmp2A1,sizeof(void*),0));});});
Cyc_Tcpat_print_pat_test(_tmp2A6);
({void*_tmp2A3=0;({struct Cyc___cycFILE*_tmp58D=Cyc_stderr;struct _dyneither_ptr _tmp58C=({const char*_tmp2A4=":\n";_tag_dyneither(_tmp2A4,sizeof(char),3);});Cyc_fprintf(_tmp58D,_tmp58C,_tag_dyneither(_tmp2A3,sizeof(void*),0));});});
Cyc_Tcpat_print_dec_tree(_tmp2A5,tab + 7);}
# 1318
Cyc_Tcpat_print_tab(tab);
({void*_tmp2A7=0;({struct Cyc___cycFILE*_tmp58F=Cyc_stderr;struct _dyneither_ptr _tmp58E=({const char*_tmp2A8="default:\n";_tag_dyneither(_tmp2A8,sizeof(char),10);});Cyc_fprintf(_tmp58F,_tmp58E,_tag_dyneither(_tmp2A7,sizeof(void*),0));});});
Cyc_Tcpat_print_dec_tree(_tmp2AC,tab + 7);
Cyc_Tcpat_print_tab(tab);
({void*_tmp2A9=0;({struct Cyc___cycFILE*_tmp591=Cyc_stderr;struct _dyneither_ptr _tmp590=({const char*_tmp2AA="}\n";_tag_dyneither(_tmp2AA,sizeof(char),3);});Cyc_fprintf(_tmp591,_tmp590,_tag_dyneither(_tmp2A9,sizeof(void*),0));});});}_LL18A:;}
# 1326
void Cyc_Tcpat_print_decision_tree(void*d){
Cyc_Tcpat_print_dec_tree(d,0);}
# 1334
static void*Cyc_Tcpat_add_neg(void*td,struct Cyc_Tcpat_Con_s*c){
void*_tmp2AF=td;struct Cyc_Set_Set*_tmp2B4;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2AF)->tag == 1){_LL195: _tmp2B4=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2AF)->f1;_LL196:
# 1344
 return(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp593=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2B1;_tmp2B1.tag=1;({struct Cyc_Set_Set*_tmp592=((struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_insert)(_tmp2B4,c);_tmp2B1.f1=_tmp592;});_tmp2B1;});_tmp2B0[0]=_tmp593;});_tmp2B0;});}else{_LL197: _LL198:
({void*_tmp2B2=0;({struct _dyneither_ptr _tmp594=({const char*_tmp2B3="add_neg called when td is Positive";_tag_dyneither(_tmp2B3,sizeof(char),35);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp594,_tag_dyneither(_tmp2B2,sizeof(void*),0));});});}_LL194:;}
# 1351
static enum Cyc_Tcpat_Answer Cyc_Tcpat_static_match(struct Cyc_Tcpat_Con_s*c,void*td){
void*_tmp2B5=td;struct Cyc_Set_Set*_tmp2B7;struct Cyc_Tcpat_Con_s*_tmp2B6;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B5)->tag == 0){_LL19A: _tmp2B6=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2B5)->f1;_LL19B:
# 1355
 if(({Cyc_Tcpat_compare_con(c,_tmp2B6)== 0;}))return Cyc_Tcpat_Yes;else{
return Cyc_Tcpat_No;}}else{_LL19C: _tmp2B7=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2B5)->f1;_LL19D:
# 1359
 if(((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp2B7,c))return Cyc_Tcpat_No;else{
# 1362
if(c->span != 0  && ({int _tmp595=*((int*)_check_null(c->span));_tmp595 == ({((int(*)(struct Cyc_Set_Set*s))Cyc_Set_cardinality)(_tmp2B7)+ 1;});}))
return Cyc_Tcpat_Yes;else{
# 1365
return Cyc_Tcpat_Maybe;}}}_LL199:;}struct _tuple21{struct Cyc_Tcpat_Con_s*f1;struct Cyc_List_List*f2;};
# 1373
static struct Cyc_List_List*Cyc_Tcpat_augment(struct Cyc_List_List*ctxt,void*dsc){
struct Cyc_List_List*_tmp2B8=ctxt;struct Cyc_Tcpat_Con_s*_tmp2BE;struct Cyc_List_List*_tmp2BD;struct Cyc_List_List*_tmp2BC;if(_tmp2B8 == 0){_LL19F: _LL1A0:
 return 0;}else{_LL1A1: _tmp2BE=((struct _tuple21*)_tmp2B8->hd)->f1;_tmp2BD=((struct _tuple21*)_tmp2B8->hd)->f2;_tmp2BC=_tmp2B8->tl;_LL1A2:
# 1377
 return({struct Cyc_List_List*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));({struct _tuple21*_tmp597=({struct _tuple21*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA->f1=_tmp2BE;({struct Cyc_List_List*_tmp596=({struct Cyc_List_List*_tmp2BB=_cycalloc(sizeof(*_tmp2BB));_tmp2BB->hd=dsc;_tmp2BB->tl=_tmp2BD;_tmp2BB;});_tmp2BA->f2=_tmp596;});_tmp2BA;});_tmp2B9->hd=_tmp597;});_tmp2B9->tl=_tmp2BC;_tmp2B9;});}_LL19E:;}
# 1385
static struct Cyc_List_List*Cyc_Tcpat_norm_context(struct Cyc_List_List*ctxt){
struct Cyc_List_List*_tmp2BF=ctxt;struct Cyc_Tcpat_Con_s*_tmp2C6;struct Cyc_List_List*_tmp2C5;struct Cyc_List_List*_tmp2C4;if(_tmp2BF == 0){_LL1A4: _LL1A5:
({void*_tmp2C0=0;({struct _dyneither_ptr _tmp598=({const char*_tmp2C1="norm_context: empty context";_tag_dyneither(_tmp2C1,sizeof(char),28);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp598,_tag_dyneither(_tmp2C0,sizeof(void*),0));});});}else{_LL1A6: _tmp2C6=((struct _tuple21*)_tmp2BF->hd)->f1;_tmp2C5=((struct _tuple21*)_tmp2BF->hd)->f2;_tmp2C4=_tmp2BF->tl;_LL1A7:
# 1389
 return({struct Cyc_List_List*_tmp59B=_tmp2C4;Cyc_Tcpat_augment(_tmp59B,(void*)({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp59A=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp2C3;_tmp2C3.tag=0;_tmp2C3.f1=_tmp2C6;({struct Cyc_List_List*_tmp599=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp2C5);_tmp2C3.f2=_tmp599;});_tmp2C3;});_tmp2C2[0]=_tmp59A;});_tmp2C2;}));});}_LL1A3:;}
# 1398
static void*Cyc_Tcpat_build_desc(struct Cyc_List_List*ctxt,void*dsc,struct Cyc_List_List*work){
# 1400
struct _tuple1 _tmp2C7=({struct _tuple1 _tmp2D4;_tmp2D4.f1=ctxt;_tmp2D4.f2=work;_tmp2D4;});struct _tuple1 _tmp2C8=_tmp2C7;struct Cyc_Tcpat_Con_s*_tmp2D3;struct Cyc_List_List*_tmp2D2;struct Cyc_List_List*_tmp2D1;struct Cyc_List_List*_tmp2D0;struct Cyc_List_List*_tmp2CF;if(_tmp2C8.f1 == 0){if(_tmp2C8.f2 == 0){_LL1A9: _LL1AA:
 return dsc;}else{_LL1AB: _LL1AC:
 goto _LL1AE;}}else{if(_tmp2C8.f2 == 0){_LL1AD: _LL1AE:
({void*_tmp2C9=0;({struct _dyneither_ptr _tmp59C=({const char*_tmp2CA="build_desc: ctxt and work don't match";_tag_dyneither(_tmp2CA,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp59C,_tag_dyneither(_tmp2C9,sizeof(void*),0));});});}else{_LL1AF: _tmp2D3=((struct _tuple21*)(_tmp2C8.f1)->hd)->f1;_tmp2D2=((struct _tuple21*)(_tmp2C8.f1)->hd)->f2;_tmp2D1=(_tmp2C8.f1)->tl;_tmp2D0=((struct _tuple20*)(_tmp2C8.f2)->hd)->f3;_tmp2CF=(_tmp2C8.f2)->tl;_LL1B0: {
# 1405
struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2CB=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp59F=({struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct _tmp2CD;_tmp2CD.tag=0;_tmp2CD.f1=_tmp2D3;({struct Cyc_List_List*_tmp59E=({struct Cyc_List_List*_tmp59D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp2D2);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp59D,({struct Cyc_List_List*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->hd=dsc;_tmp2CE->tl=_tmp2D0;_tmp2CE;}));});_tmp2CD.f2=_tmp59E;});_tmp2CD;});_tmp2CC[0]=_tmp59F;});_tmp2CC;});
return Cyc_Tcpat_build_desc(_tmp2D1,(void*)_tmp2CB,_tmp2CF);}}}_LL1A8:;}
# 1410
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules);struct _tuple22{void*f1;struct Cyc_Tcpat_Rhs*f2;};
# 1417
static void*Cyc_Tcpat_or_match(void*dsc,struct Cyc_List_List*allmrules){
struct Cyc_List_List*_tmp2D5=allmrules;void*_tmp2DA;struct Cyc_Tcpat_Rhs*_tmp2D9;struct Cyc_List_List*_tmp2D8;if(_tmp2D5 == 0){_LL1B2: _LL1B3:
 return(void*)({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp5A0=({struct Cyc_Tcpat_Failure_Tcpat_Decision_struct _tmp2D7;_tmp2D7.tag=0;_tmp2D7.f1=dsc;_tmp2D7;});_tmp2D6[0]=_tmp5A0;});_tmp2D6;});}else{_LL1B4: _tmp2DA=((struct _tuple22*)_tmp2D5->hd)->f1;_tmp2D9=((struct _tuple22*)_tmp2D5->hd)->f2;_tmp2D8=_tmp2D5->tl;_LL1B5:
# 1421
 return Cyc_Tcpat_match(_tmp2DA,0,dsc,0,0,_tmp2D9,_tmp2D8);}_LL1B1:;}
# 1426
static void*Cyc_Tcpat_match_compile(struct Cyc_List_List*allmrules){
return({void*_tmp5A3=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5A2=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2DC;_tmp2DC.tag=1;({struct Cyc_Set_Set*_tmp5A1=Cyc_Tcpat_empty_con_set();_tmp2DC.f1=_tmp5A1;});_tmp2DC;});_tmp2DB[0]=_tmp5A2;});_tmp2DB;});Cyc_Tcpat_or_match(_tmp5A3,allmrules);});}
# 1433
static void*Cyc_Tcpat_and_match(struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1436
struct Cyc_List_List*_tmp2DD=work;struct Cyc_List_List*_tmp2F5;struct Cyc_List_List*_tmp2F4;struct Cyc_List_List*_tmp2F3;struct Cyc_List_List*_tmp2F2;struct Cyc_List_List*_tmp2F1;if(_tmp2DD == 0){_LL1B7: _LL1B8:
 return(void*)({struct Cyc_Tcpat_Success_Tcpat_Decision_struct*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));({struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp5A4=({struct Cyc_Tcpat_Success_Tcpat_Decision_struct _tmp2DF;_tmp2DF.tag=1;_tmp2DF.f1=right_hand_side;_tmp2DF;});_tmp2DE[0]=_tmp5A4;});_tmp2DE;});}else{if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2DD)->hd)->f1 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2DD)->hd)->f2 == 0){if(((struct _tuple20*)((struct Cyc_List_List*)_tmp2DD)->hd)->f3 == 0){_LL1B9: _tmp2F1=_tmp2DD->tl;_LL1BA:
# 1439
 return({struct Cyc_List_List*_tmp5A7=Cyc_Tcpat_norm_context(ctx);struct Cyc_List_List*_tmp5A6=_tmp2F1;struct Cyc_Tcpat_Rhs*_tmp5A5=right_hand_side;Cyc_Tcpat_and_match(_tmp5A7,_tmp5A6,_tmp5A5,rules);});}else{goto _LL1BB;}}else{goto _LL1BB;}}else{_LL1BB: _tmp2F5=((struct _tuple20*)_tmp2DD->hd)->f1;_tmp2F4=((struct _tuple20*)_tmp2DD->hd)->f2;_tmp2F3=((struct _tuple20*)_tmp2DD->hd)->f3;_tmp2F2=_tmp2DD->tl;_LL1BC:
# 1441
 if((_tmp2F5 == 0  || _tmp2F4 == 0) || _tmp2F3 == 0)
({void*_tmp2E0=0;({struct _dyneither_ptr _tmp5A8=({const char*_tmp2E1="tcpat:and_match: malformed work frame";_tag_dyneither(_tmp2E1,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5A8,_tag_dyneither(_tmp2E0,sizeof(void*),0));});});{
struct Cyc_List_List*_tmp2E2=_tmp2F5;struct Cyc_List_List*_tmp2E3=_tmp2E2;void*_tmp2F0;struct Cyc_List_List*_tmp2EF;_LL1BE: _tmp2F0=(void*)_tmp2E3->hd;_tmp2EF=_tmp2E3->tl;_LL1BF:;{
struct Cyc_List_List*_tmp2E4=_tmp2F4;struct Cyc_List_List*_tmp2E5=_tmp2E4;struct Cyc_List_List*_tmp2EE;struct Cyc_List_List*_tmp2ED;_LL1C1: _tmp2EE=(struct Cyc_List_List*)_tmp2E5->hd;_tmp2ED=_tmp2E5->tl;_LL1C2:;{
struct Cyc_List_List*_tmp2E6=_tmp2F3;struct Cyc_List_List*_tmp2E7=_tmp2E6;void*_tmp2EC;struct Cyc_List_List*_tmp2EB;_LL1C4: _tmp2EC=(void*)_tmp2E7->hd;_tmp2EB=_tmp2E7->tl;_LL1C5:;{
struct _tuple20*_tmp2E8=({struct _tuple20*_tmp2EA=_cycalloc(sizeof(*_tmp2EA));_tmp2EA->f1=_tmp2EF;_tmp2EA->f2=_tmp2ED;_tmp2EA->f3=_tmp2EB;_tmp2EA;});
return({void*_tmp5AE=_tmp2F0;struct Cyc_List_List*_tmp5AD=_tmp2EE;void*_tmp5AC=_tmp2EC;struct Cyc_List_List*_tmp5AB=ctx;struct Cyc_List_List*_tmp5AA=({struct Cyc_List_List*_tmp2E9=_cycalloc(sizeof(*_tmp2E9));_tmp2E9->hd=_tmp2E8;_tmp2E9->tl=_tmp2F2;_tmp2E9;});struct Cyc_Tcpat_Rhs*_tmp5A9=right_hand_side;Cyc_Tcpat_match(_tmp5AE,_tmp5AD,_tmp5AC,_tmp5AB,_tmp5AA,_tmp5A9,rules);});};};};};}}_LL1B6:;}
# 1452
static struct Cyc_List_List*Cyc_Tcpat_getdargs(struct Cyc_Tcpat_Con_s*pcon,void*dsc){
void*_tmp2F6=dsc;struct Cyc_List_List*_tmp2FC;struct Cyc_Set_Set*_tmp2FB;if(((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F6)->tag == 1){_LL1C7: _tmp2FB=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp2F6)->f1;_LL1C8: {
# 1458
void*any=(void*)({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*_tmp2F9=_cycalloc(sizeof(*_tmp2F9));({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp5B0=({struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct _tmp2FA;_tmp2FA.tag=1;({struct Cyc_Set_Set*_tmp5AF=Cyc_Tcpat_empty_con_set();_tmp2FA.f1=_tmp5AF;});_tmp2FA;});_tmp2F9[0]=_tmp5B0;});_tmp2F9;});
struct Cyc_List_List*_tmp2F7=0;
{int i=0;for(0;i < pcon->arity;++ i){
({struct Cyc_List_List*_tmp5B1=({struct Cyc_List_List*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));_tmp2F8->hd=any;_tmp2F8->tl=_tmp2F7;_tmp2F8;});_tmp2F7=_tmp5B1;});}}
return _tmp2F7;}}else{_LL1C9: _tmp2FC=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp2F6)->f2;_LL1CA:
 return _tmp2FC;}_LL1C6:;}
# 1467
static void*Cyc_Tcpat_get_access(union Cyc_Tcpat_PatOrWhere pw,int i){
union Cyc_Tcpat_PatOrWhere _tmp2FD=pw;struct Cyc_Absyn_Pat*_tmp31E;if((_tmp2FD.where_clause).tag == 2){_LL1CC: _LL1CD:
 return(void*)& Cyc_Tcpat_Dummy_val;}else{_LL1CE: _tmp31E=(_tmp2FD.pattern).val;_LL1CF: {
# 1471
void*_tmp2FE=_tmp31E->r;void*_tmp2FF=_tmp2FE;union Cyc_Absyn_AggrInfoU _tmp31D;struct Cyc_List_List*_tmp31C;struct Cyc_Absyn_Datatypedecl*_tmp31B;struct Cyc_Absyn_Datatypefield*_tmp31A;switch(*((int*)_tmp2FF)){case 6: _LL1D1: _LL1D2:
# 1473
 if(i != 0)
({void*_tmp300=0;({struct _dyneither_ptr _tmp5B3=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp303;_tmp303.tag=1;_tmp303.f1=(unsigned long)i;({void*_tmp301[1]={& _tmp303};({struct _dyneither_ptr _tmp5B2=({const char*_tmp302="get_access on pointer pattern with offset %d\n";_tag_dyneither(_tmp302,sizeof(char),46);});Cyc_aprintf(_tmp5B2,_tag_dyneither(_tmp301,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5B3,_tag_dyneither(_tmp300,sizeof(void*),0));});});
return(void*)& Cyc_Tcpat_Deref_val;case 5: _LL1D3: _LL1D4:
 return(void*)({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*_tmp304=_cycalloc_atomic(sizeof(*_tmp304));({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct _tmp5B4=({struct Cyc_Tcpat_TupleField_Tcpat_Access_struct _tmp305;_tmp305.tag=2;_tmp305.f1=(unsigned int)i;_tmp305;});_tmp304[0]=_tmp5B4;});_tmp304;});case 8: _LL1D5: _tmp31B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2FF)->f1;_tmp31A=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp2FF)->f2;_LL1D6:
 return(void*)({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*_tmp306=_cycalloc(sizeof(*_tmp306));({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct _tmp5B5=({struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct _tmp307;_tmp307.tag=3;_tmp307.f1=_tmp31B;_tmp307.f2=_tmp31A;_tmp307.f3=(unsigned int)i;_tmp307;});_tmp306[0]=_tmp5B5;});_tmp306;});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2FF)->f1 != 0){_LL1D7: _tmp31D=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2FF)->f1)->aggr_info;_tmp31C=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp2FF)->f3;_LL1D8: {
# 1479
struct Cyc_Absyn_Aggrdecl*_tmp308=Cyc_Absyn_get_known_aggrdecl(_tmp31D);
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->tagged){
struct Cyc_List_List*_tmp309=(*((struct _tuple18*)((struct Cyc_List_List*)_check_null(_tmp31C))->hd)).f1;struct Cyc_List_List*_tmp30A=_tmp309;struct _dyneither_ptr*_tmp311;if(_tmp30A != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)_tmp30A)->hd)->tag == 1){if(((struct Cyc_List_List*)_tmp30A)->tl == 0){_LL1DC: _tmp311=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp30A->hd)->f1;_LL1DD:
# 1483
 return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp30B=_cycalloc(sizeof(*_tmp30B));({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp5B6=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp30C;_tmp30C.tag=4;_tmp30C.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->tagged;_tmp30C.f2=_tmp311;_tmp30C;});_tmp30B[0]=_tmp5B6;});_tmp30B;});}else{goto _LL1DE;}}else{goto _LL1DE;}}else{_LL1DE: _LL1DF:
({void*_tmp30D=0;({struct _dyneither_ptr _tmp5B9=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp310;_tmp310.tag=0;({struct _dyneither_ptr _tmp5B7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp31E));_tmp310.f1=_tmp5B7;});({void*_tmp30E[1]={& _tmp310};({struct _dyneither_ptr _tmp5B8=({const char*_tmp30F="get_access on bad aggr pattern: %s";_tag_dyneither(_tmp30F,sizeof(char),35);});Cyc_aprintf(_tmp5B8,_tag_dyneither(_tmp30E,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5B9,_tag_dyneither(_tmp30D,sizeof(void*),0));});});}_LL1DB:;}{
# 1487
struct Cyc_List_List*_tmp312=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->fields;
int _tmp313=i;
for(0;i != 0;-- i){_tmp312=((struct Cyc_List_List*)_check_null(_tmp312))->tl;}
return(void*)({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*_tmp314=_cycalloc(sizeof(*_tmp314));({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp5BA=({struct Cyc_Tcpat_AggrField_Tcpat_Access_struct _tmp315;_tmp315.tag=4;_tmp315.f1=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp308->impl))->tagged;_tmp315.f2=((struct Cyc_Absyn_Aggrfield*)((struct Cyc_List_List*)_check_null(_tmp312))->hd)->name;_tmp315;});_tmp314[0]=_tmp5BA;});_tmp314;});};}}else{goto _LL1D9;}default: _LL1D9: _LL1DA:
({void*_tmp316=0;({struct _dyneither_ptr _tmp5BD=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp319;_tmp319.tag=0;({struct _dyneither_ptr _tmp5BB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_pat2string(_tmp31E));_tmp319.f1=_tmp5BB;});({void*_tmp317[1]={& _tmp319};({struct _dyneither_ptr _tmp5BC=({const char*_tmp318="get_access on bad pattern: %s";_tag_dyneither(_tmp318,sizeof(char),30);});Cyc_aprintf(_tmp5BC,_tag_dyneither(_tmp317,sizeof(void*),1));});});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp5BD,_tag_dyneither(_tmp316,sizeof(void*),0));});});}_LL1D0:;}}_LL1CB:;}struct _tuple23{struct Cyc_List_List*f1;struct Cyc_Tcpat_Con_s*f2;};
# 1497
static struct Cyc_List_List*Cyc_Tcpat_getoarg(struct _tuple23*env,int i){
struct _tuple23*_tmp31F=env;struct Cyc_List_List*_tmp323;struct Cyc_Tcpat_Con_s*_tmp322;_LL1E1: _tmp323=_tmp31F->f1;_tmp322=_tmp31F->f2;_LL1E2:;{
void*acc=Cyc_Tcpat_get_access(_tmp322->orig_pat,i);
return({struct Cyc_List_List*_tmp320=_cycalloc(sizeof(*_tmp320));({struct Cyc_Tcpat_PathNode*_tmp5BE=({struct Cyc_Tcpat_PathNode*_tmp321=_cycalloc(sizeof(*_tmp321));_tmp321->orig_pat=_tmp322->orig_pat;_tmp321->access=acc;_tmp321;});_tmp320->hd=_tmp5BE;});_tmp320->tl=_tmp323;_tmp320;});};}
# 1502
static struct Cyc_List_List*Cyc_Tcpat_getoargs(struct Cyc_Tcpat_Con_s*pcon,struct Cyc_List_List*obj){
struct _tuple23 _tmp324=({struct _tuple23 _tmp325;_tmp325.f1=obj;_tmp325.f2=pcon;_tmp325;});
return((struct Cyc_List_List*(*)(int n,struct Cyc_List_List*(*f)(struct _tuple23*,int),struct _tuple23*env))Cyc_List_tabulate_c)(pcon->arity,Cyc_Tcpat_getoarg,& _tmp324);}
# 1510
static void*Cyc_Tcpat_match(void*p,struct Cyc_List_List*obj,void*dsc,struct Cyc_List_List*ctx,struct Cyc_List_List*work,struct Cyc_Tcpat_Rhs*right_hand_side,struct Cyc_List_List*rules){
# 1514
void*_tmp326=p;struct Cyc_Tcpat_Con_s*_tmp334;struct Cyc_List_List*_tmp333;if(((struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*)_tmp326)->tag == 0){_LL1E4: _LL1E5:
# 1516
 return({struct Cyc_List_List*_tmp5C1=Cyc_Tcpat_augment(ctx,dsc);struct Cyc_List_List*_tmp5C0=work;struct Cyc_Tcpat_Rhs*_tmp5BF=right_hand_side;Cyc_Tcpat_and_match(_tmp5C1,_tmp5C0,_tmp5BF,rules);});}else{_LL1E6: _tmp334=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp326)->f1;_tmp333=((struct Cyc_Tcpat_Con_Tcpat_Simple_pat_struct*)_tmp326)->f2;_LL1E7: {
# 1518
enum Cyc_Tcpat_Answer _tmp327=Cyc_Tcpat_static_match(_tmp334,dsc);enum Cyc_Tcpat_Answer _tmp328=_tmp327;switch(_tmp328){case Cyc_Tcpat_Yes: _LL1E9: _LL1EA: {
# 1520
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp32B=_cycalloc(sizeof(*_tmp32B));({struct _tuple21*_tmp5C2=({struct _tuple21*_tmp32C=_cycalloc(sizeof(*_tmp32C));_tmp32C->f1=_tmp334;_tmp32C->f2=0;_tmp32C;});_tmp32B->hd=_tmp5C2;});_tmp32B->tl=ctx;_tmp32B;});
struct _tuple20*work_frame=({struct _tuple20*_tmp32A=_cycalloc(sizeof(*_tmp32A));_tmp32A->f1=_tmp333;({struct Cyc_List_List*_tmp5C3=Cyc_Tcpat_getoargs(_tmp334,obj);_tmp32A->f2=_tmp5C3;});({struct Cyc_List_List*_tmp5C4=
Cyc_Tcpat_getdargs(_tmp334,dsc);_tmp32A->f3=_tmp5C4;});_tmp32A;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->hd=work_frame;_tmp329->tl=work;_tmp329;});
return Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);}case Cyc_Tcpat_No: _LL1EB: _LL1EC:
# 1526
 return({void*_tmp5C5=Cyc_Tcpat_build_desc(ctx,dsc,work);Cyc_Tcpat_or_match(_tmp5C5,rules);});default: _LL1ED: _LL1EE: {
# 1528
struct Cyc_List_List*ctx2=({struct Cyc_List_List*_tmp331=_cycalloc(sizeof(*_tmp331));({struct _tuple21*_tmp5C6=({struct _tuple21*_tmp332=_cycalloc(sizeof(*_tmp332));_tmp332->f1=_tmp334;_tmp332->f2=0;_tmp332;});_tmp331->hd=_tmp5C6;});_tmp331->tl=ctx;_tmp331;});
struct _tuple20*work_frame=({struct _tuple20*_tmp330=_cycalloc(sizeof(*_tmp330));_tmp330->f1=_tmp333;({struct Cyc_List_List*_tmp5C7=Cyc_Tcpat_getoargs(_tmp334,obj);_tmp330->f2=_tmp5C7;});({struct Cyc_List_List*_tmp5C8=
Cyc_Tcpat_getdargs(_tmp334,dsc);_tmp330->f3=_tmp5C8;});_tmp330;});
struct Cyc_List_List*work2=({struct Cyc_List_List*_tmp32F=_cycalloc(sizeof(*_tmp32F));_tmp32F->hd=work_frame;_tmp32F->tl=work;_tmp32F;});
void*_tmp32D=Cyc_Tcpat_and_match(ctx2,work2,right_hand_side,rules);
void*_tmp32E=({void*_tmp5CB=({struct Cyc_List_List*_tmp5CA=ctx;void*_tmp5C9=Cyc_Tcpat_add_neg(dsc,_tmp334);Cyc_Tcpat_build_desc(_tmp5CA,_tmp5C9,work);});Cyc_Tcpat_or_match(_tmp5CB,rules);});
# 1535
return Cyc_Tcpat_ifeq(obj,_tmp334,_tmp32D,_tmp32E);}}_LL1E8:;}}_LL1E3:;}
# 1545
static void Cyc_Tcpat_check_exhaust_overlap(void*d,void(*not_exhaust)(void*,void*),void*env1,void(*rhs_appears)(void*,struct Cyc_Tcpat_Rhs*),void*env2,int*exhaust_done){
# 1550
void*_tmp335=d;struct Cyc_List_List*_tmp33C;void*_tmp33B;struct Cyc_Tcpat_Rhs*_tmp33A;void*_tmp339;switch(*((int*)_tmp335)){case 0: _LL1F0: _tmp339=(void*)((struct Cyc_Tcpat_Failure_Tcpat_Decision_struct*)_tmp335)->f1;_LL1F1:
# 1552
 if(!(*exhaust_done)){not_exhaust(env1,_tmp339);*exhaust_done=1;}
goto _LL1EF;case 1: _LL1F2: _tmp33A=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp335)->f1;_LL1F3:
 rhs_appears(env2,_tmp33A);goto _LL1EF;default: _LL1F4: _tmp33C=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp335)->f2;_tmp33B=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp335)->f3;_LL1F5:
# 1556
 for(0;_tmp33C != 0;_tmp33C=_tmp33C->tl){
struct _tuple0 _tmp336=*((struct _tuple0*)_tmp33C->hd);struct _tuple0 _tmp337=_tmp336;void*_tmp338;_LL1F7: _tmp338=_tmp337.f2;_LL1F8:;
Cyc_Tcpat_check_exhaust_overlap(_tmp338,not_exhaust,env1,rhs_appears,env2,exhaust_done);}
# 1561
Cyc_Tcpat_check_exhaust_overlap(_tmp33B,not_exhaust,env1,rhs_appears,env2,exhaust_done);
# 1563
goto _LL1EF;}_LL1EF:;}
# 1571
static struct _tuple22*Cyc_Tcpat_get_match(int*ctr,struct Cyc_Absyn_Switch_clause*swc){
# 1573
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp345=_cycalloc(sizeof(*_tmp345));_tmp345->used=0;_tmp345->pat_loc=(swc->pattern)->loc;_tmp345->rhs=swc->body;_tmp345;});
void*sp;
if(swc->where_clause != 0){
union Cyc_Tcpat_PatOrWhere _tmp33D=({union Cyc_Tcpat_PatOrWhere _tmp33F;(_tmp33F.where_clause).val=swc->where_clause;(_tmp33F.where_clause).tag=2;_tmp33F;});
({void*_tmp5CE=({struct Cyc_List_List*_tmp5CD=({void*_tmp33E[2];({void*_tmp5CC=Cyc_Tcpat_int_pat(*ctr,_tmp33D);_tmp33E[1]=_tmp5CC;});_tmp33E[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp33E,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_tmp5CD,_tmp33D);});sp=_tmp5CE;});
*ctr=*ctr + 1;}else{
# 1581
({void*_tmp5D2=({struct Cyc_List_List*_tmp5D1=({void*_tmp340[2];({void*_tmp5D0=(void*)({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct*_tmp341=_cycalloc_atomic(sizeof(*_tmp341));({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp5CF=({struct Cyc_Tcpat_Any_Tcpat_Simple_pat_struct _tmp342;_tmp342.tag=0;_tmp342;});_tmp341[0]=_tmp5CF;});_tmp341;});_tmp340[1]=_tmp5D0;});_tmp340[0]=sp0;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp340,sizeof(void*),2));});Cyc_Tcpat_tuple_pat(_tmp5D1,({union Cyc_Tcpat_PatOrWhere _tmp343;(_tmp343.where_clause).val=0;(_tmp343.where_clause).tag=2;_tmp343;}));});sp=_tmp5D2;});}
return({struct _tuple22*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344->f1=sp;_tmp344->f2=rhs;_tmp344;});}char Cyc_Tcpat_Desc2string[12]="Desc2string";struct Cyc_Tcpat_Desc2string_exn_struct{char*tag;};
# 1587
struct Cyc_Tcpat_Desc2string_exn_struct Cyc_Tcpat_Desc2string_val={Cyc_Tcpat_Desc2string};
# 1589
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*);
static struct _dyneither_ptr Cyc_Tcpat_desc2string(void*d){
void*_tmp347=d;struct Cyc_Set_Set*_tmp3A8;struct Cyc_Tcpat_Con_s*_tmp3A7;struct Cyc_List_List*_tmp3A6;if(((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp347)->tag == 0){_LL1FA: _tmp3A7=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp347)->f1;_tmp3A6=((struct Cyc_Tcpat_Pos_Tcpat_Term_desc_struct*)_tmp347)->f2;_LL1FB: {
# 1593
union Cyc_Tcpat_Name_value _tmp348=_tmp3A7->name;
struct Cyc_Absyn_Pat*p;
{union Cyc_Tcpat_PatOrWhere _tmp349=_tmp3A7->orig_pat;union Cyc_Tcpat_PatOrWhere _tmp34A=_tmp349;struct Cyc_Absyn_Pat*_tmp34B;if((_tmp34A.where_clause).tag == 2){_LL1FF: _LL200:
 return Cyc_Tcpat_desc2string((void*)((struct Cyc_List_List*)_check_null(_tmp3A6))->hd);}else{_LL201: _tmp34B=(_tmp34A.pattern).val;_LL202:
 p=_tmp34B;goto _LL1FE;}_LL1FE:;}{
# 1599
void*_tmp34C=p->r;void*_tmp34D=_tmp34C;struct Cyc_Absyn_Exp*_tmp382;struct Cyc_Absyn_Enumfield*_tmp381;struct Cyc_Absyn_Enumfield*_tmp380;struct _dyneither_ptr _tmp37F;int _tmp37E;char _tmp37D;int _tmp37C;struct Cyc_Absyn_Datatypefield*_tmp37B;struct Cyc_Absyn_Aggrdecl*_tmp37A;struct Cyc_List_List*_tmp379;struct Cyc_Absyn_Tvar*_tmp378;struct Cyc_Absyn_Vardecl*_tmp377;struct Cyc_Absyn_Vardecl*_tmp376;struct Cyc_Absyn_Vardecl*_tmp375;switch(*((int*)_tmp34D)){case 0: _LL204: _LL205:
 return({const char*_tmp34E="_";_tag_dyneither(_tmp34E,sizeof(char),2);});case 1: _LL206: _tmp375=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp34D)->f1;_LL207:
 return Cyc_Absynpp_qvar2string(_tmp375->name);case 3: _LL208: _tmp376=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp34D)->f1;_LL209:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp351;_tmp351.tag=0;({struct _dyneither_ptr _tmp5D3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp376->name));_tmp351.f1=_tmp5D3;});({void*_tmp34F[1]={& _tmp351};({struct _dyneither_ptr _tmp5D4=({const char*_tmp350="*%s";_tag_dyneither(_tmp350,sizeof(char),4);});Cyc_aprintf(_tmp5D4,_tag_dyneither(_tmp34F,sizeof(void*),1));});});});case 4: _LL20A: _tmp378=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp34D)->f1;_tmp377=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp34D)->f2;_LL20B:
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp355;_tmp355.tag=0;_tmp355.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp378->name);({struct Cyc_String_pa_PrintArg_struct _tmp354;_tmp354.tag=0;({struct _dyneither_ptr _tmp5D5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp377->name));_tmp354.f1=_tmp5D5;});({void*_tmp352[2]={& _tmp354,& _tmp355};({struct _dyneither_ptr _tmp5D6=({const char*_tmp353="%s<`%s>";_tag_dyneither(_tmp353,sizeof(char),8);});Cyc_aprintf(_tmp5D6,_tag_dyneither(_tmp352,sizeof(void*),2));});});});});case 5: _LL20C: _LL20D:
# 1605
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp358;_tmp358.tag=0;({struct _dyneither_ptr _tmp5D7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3A6));_tmp358.f1=_tmp5D7;});({void*_tmp356[1]={& _tmp358};({struct _dyneither_ptr _tmp5D8=({const char*_tmp357="$(%s)";_tag_dyneither(_tmp357,sizeof(char),6);});Cyc_aprintf(_tmp5D8,_tag_dyneither(_tmp356,sizeof(void*),1));});});});case 6: _LL20E: _LL20F:
# 1607
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp35B;_tmp35B.tag=0;({struct _dyneither_ptr _tmp5D9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3A6));_tmp35B.f1=_tmp5D9;});({void*_tmp359[1]={& _tmp35B};({struct _dyneither_ptr _tmp5DA=({const char*_tmp35A="&%s";_tag_dyneither(_tmp35A,sizeof(char),4);});Cyc_aprintf(_tmp5DA,_tag_dyneither(_tmp359,sizeof(void*),1));});});});case 7: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp34D)->f1 != 0){if(((((struct Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp34D)->f1)->aggr_info).KnownAggr).tag == 2){_LL210: _tmp37A=*(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp34D)->f1)->aggr_info).KnownAggr).val;_tmp379=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp34D)->f3;_LL211:
# 1609
 if(_tmp37A->kind == Cyc_Absyn_UnionA){
struct Cyc_List_List*_tmp35C=_tmp379;struct _dyneither_ptr*_tmp362;if(_tmp35C != 0){if(((struct _tuple18*)((struct Cyc_List_List*)_tmp35C)->hd)->f1 != 0){if(((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)((struct Cyc_List_List*)((struct _tuple18*)((struct Cyc_List_List*)_tmp35C)->hd)->f1)->hd)->tag == 1){_LL225: _tmp362=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)(((struct _tuple18*)_tmp35C->hd)->f1)->hd)->f1;_LL226:
# 1612
 return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp361;_tmp361.tag=0;({struct _dyneither_ptr _tmp5DB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Tcpat_descs2string(_tmp3A6));_tmp361.f1=_tmp5DB;});({struct Cyc_String_pa_PrintArg_struct _tmp360;_tmp360.tag=0;_tmp360.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp362);({struct Cyc_String_pa_PrintArg_struct _tmp35F;_tmp35F.tag=0;({struct _dyneither_ptr _tmp5DC=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1612
Cyc_Absynpp_qvar2string(_tmp37A->name));_tmp35F.f1=_tmp5DC;});({void*_tmp35D[3]={& _tmp35F,& _tmp360,& _tmp361};({struct _dyneither_ptr _tmp5DD=({const char*_tmp35E="%s{.%s = %s}";_tag_dyneither(_tmp35E,sizeof(char),13);});Cyc_aprintf(_tmp5DD,_tag_dyneither(_tmp35D,sizeof(void*),3));});});});});});}else{goto _LL227;}}else{goto _LL227;}}else{_LL227: _LL228:
# 1614
 goto _LL224;}_LL224:;}
# 1617
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp366;_tmp366.tag=0;({struct _dyneither_ptr _tmp5DE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3A6));_tmp366.f1=_tmp5DE;});({struct Cyc_String_pa_PrintArg_struct _tmp365;_tmp365.tag=0;({struct _dyneither_ptr _tmp5DF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp37A->name));_tmp365.f1=_tmp5DF;});({void*_tmp363[2]={& _tmp365,& _tmp366};({struct _dyneither_ptr _tmp5E0=({const char*_tmp364="%s{%s}";_tag_dyneither(_tmp364,sizeof(char),7);});Cyc_aprintf(_tmp5E0,_tag_dyneither(_tmp363,sizeof(void*),2));});});});});}else{goto _LL222;}}else{goto _LL222;}case 8: _LL212: _tmp37B=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp34D)->f2;_LL213:
# 1619
 if(_tmp3A6 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp369;_tmp369.tag=0;({struct _dyneither_ptr _tmp5E1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp37B->name));_tmp369.f1=_tmp5E1;});({void*_tmp367[1]={& _tmp369};({struct _dyneither_ptr _tmp5E2=({const char*_tmp368="%s";_tag_dyneither(_tmp368,sizeof(char),3);});Cyc_aprintf(_tmp5E2,_tag_dyneither(_tmp367,sizeof(void*),1));});});});else{
# 1622
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp36D;_tmp36D.tag=0;({struct _dyneither_ptr _tmp5E3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcpat_descs2string(_tmp3A6));_tmp36D.f1=_tmp5E3;});({struct Cyc_String_pa_PrintArg_struct _tmp36C;_tmp36C.tag=0;({struct _dyneither_ptr _tmp5E4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp37B->name));_tmp36C.f1=_tmp5E4;});({void*_tmp36A[2]={& _tmp36C,& _tmp36D};({struct _dyneither_ptr _tmp5E5=({const char*_tmp36B="%s(%s)";_tag_dyneither(_tmp36B,sizeof(char),7);});Cyc_aprintf(_tmp5E5,_tag_dyneither(_tmp36A,sizeof(void*),2));});});});});}case 9: _LL214: _LL215:
 return({const char*_tmp36E="NULL";_tag_dyneither(_tmp36E,sizeof(char),5);});case 10: _LL216: _tmp37C=((struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct*)_tmp34D)->f2;_LL217:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp371;_tmp371.tag=1;_tmp371.f1=(unsigned long)_tmp37C;({void*_tmp36F[1]={& _tmp371};({struct _dyneither_ptr _tmp5E6=({const char*_tmp370="%d";_tag_dyneither(_tmp370,sizeof(char),3);});Cyc_aprintf(_tmp5E6,_tag_dyneither(_tmp36F,sizeof(void*),1));});});});case 11: _LL218: _tmp37D=((struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct*)_tmp34D)->f1;_LL219:
 return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp374;_tmp374.tag=1;_tmp374.f1=(unsigned long)((int)_tmp37D);({void*_tmp372[1]={& _tmp374};({struct _dyneither_ptr _tmp5E7=({const char*_tmp373="%d";_tag_dyneither(_tmp373,sizeof(char),3);});Cyc_aprintf(_tmp5E7,_tag_dyneither(_tmp372,sizeof(void*),1));});});});case 12: _LL21A: _tmp37F=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp34D)->f1;_tmp37E=((struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct*)_tmp34D)->f2;_LL21B:
 return _tmp37F;case 13: _LL21C: _tmp380=((struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*)_tmp34D)->f2;_LL21D:
 _tmp381=_tmp380;goto _LL21F;case 14: _LL21E: _tmp381=((struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*)_tmp34D)->f2;_LL21F:
 return Cyc_Absynpp_qvar2string(_tmp381->name);case 17: _LL220: _tmp382=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp34D)->f1;_LL221:
 return Cyc_Absynpp_exp2string(_tmp382);default: _LL222: _LL223:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL203:;};}}else{_LL1FC: _tmp3A8=((struct Cyc_Tcpat_Neg_Tcpat_Term_desc_struct*)_tmp347)->f1;_LL1FD:
# 1633
 if(((int(*)(struct Cyc_Set_Set*s))Cyc_Set_is_empty)(_tmp3A8))return({const char*_tmp383="_";_tag_dyneither(_tmp383,sizeof(char),2);});{
struct Cyc_Tcpat_Con_s*_tmp384=((struct Cyc_Tcpat_Con_s*(*)(struct Cyc_Set_Set*s))Cyc_Set_choose)(_tmp3A8);
# 1637
if((int)(((_tmp384->orig_pat).where_clause).tag == 2))(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_Absyn_Pat*_tmp385=({union Cyc_Tcpat_PatOrWhere _tmp3A5=_tmp384->orig_pat;if((_tmp3A5.pattern).tag != 1)_throw_match();(_tmp3A5.pattern).val;});
void*_tmp386=Cyc_Tcutil_compress((void*)_check_null(_tmp385->topt));void*_tmp387=_tmp386;struct Cyc_Absyn_Aggrdecl*_tmp3A4;struct Cyc_Absyn_Datatypedecl*_tmp3A3;struct Cyc_Absyn_PtrAtts _tmp3A2;switch(*((int*)_tmp387)){case 6: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp387)->f2 == Cyc_Absyn_Char_sz){_LL22A: _LL22B:
# 1642
{int i=0;for(0;i < 256;++ i){
struct Cyc_Tcpat_Con_s*_tmp388=Cyc_Tcpat_char_con((char)i,_tmp385);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3A8,_tmp388))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp38B;_tmp38B.tag=1;_tmp38B.f1=(unsigned long)i;({void*_tmp389[1]={& _tmp38B};({struct _dyneither_ptr _tmp5E8=({const char*_tmp38A="%d";_tag_dyneither(_tmp38A,sizeof(char),3);});Cyc_aprintf(_tmp5E8,_tag_dyneither(_tmp389,sizeof(void*),1));});});});}}
# 1647
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}else{_LL22C: _LL22D:
# 1650
{unsigned int i=0;for(0;i < -1;++ i){
struct Cyc_Tcpat_Con_s*_tmp38C=Cyc_Tcpat_int_con((int)i,_tmp384->orig_pat);
if(!((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp3A8,_tmp38C))
return(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp38F;_tmp38F.tag=1;_tmp38F.f1=(unsigned long)((int)i);({void*_tmp38D[1]={& _tmp38F};({struct _dyneither_ptr _tmp5E9=({const char*_tmp38E="%d";_tag_dyneither(_tmp38E,sizeof(char),3);});Cyc_aprintf(_tmp5E9,_tag_dyneither(_tmp38D,sizeof(void*),1));});});});}}
# 1655
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}case 5: _LL22E: _tmp3A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp387)->f1).ptr_atts;_LL22F: {
# 1657
union Cyc_Absyn_Constraint*_tmp390=_tmp3A2.nullable;
int is_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp390);
if(is_nullable){
if(!({struct Cyc_Set_Set*_tmp5EA=_tmp3A8;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5EA,Cyc_Tcpat_null_con(_tmp385));}))
return({const char*_tmp391="NULL";_tag_dyneither(_tmp391,sizeof(char),5);});}
# 1663
return({const char*_tmp392="&_";_tag_dyneither(_tmp392,sizeof(char),3);});}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp387)->f1).datatype_info).KnownDatatype).tag == 2){_LL230: _tmp3A3=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp387)->f1).datatype_info).KnownDatatype).val;_LL231:
# 1665
 if(_tmp3A3->is_extensible)(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);{
struct Cyc_List_List*_tmp393=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3A3->fields))->v;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp393);
for(0;(unsigned int)_tmp393;_tmp393=_tmp393->tl){
struct _dyneither_ptr n=*(*((struct Cyc_Absyn_Datatypefield*)_tmp393->hd)->name).f2;
struct Cyc_List_List*_tmp394=((struct Cyc_Absyn_Datatypefield*)_tmp393->hd)->typs;
if(!({struct Cyc_Set_Set*_tmp5EC=_tmp3A8;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5EC,({struct Cyc_Tcpat_Con_s*_tmp395=_cycalloc(sizeof(*_tmp395));({union Cyc_Tcpat_Name_value _tmp5EB=Cyc_Tcpat_Name_v(n);_tmp395->name=_tmp5EB;});_tmp395->arity=0;_tmp395->span=0;_tmp395->orig_pat=_tmp384->orig_pat;_tmp395;}));})){
if(({((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp394)== 0;}))
return n;else{
# 1675
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp398;_tmp398.tag=0;_tmp398.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({void*_tmp396[1]={& _tmp398};({struct _dyneither_ptr _tmp5ED=({const char*_tmp397="%s(...)";_tag_dyneither(_tmp397,sizeof(char),8);});Cyc_aprintf(_tmp5ED,_tag_dyneither(_tmp396,sizeof(void*),1));});});});}}}
# 1678
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);};}else{goto _LL234;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp387)->f1).aggr_info).KnownAggr).tag == 2){_LL232: _tmp3A4=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp387)->f1).aggr_info).KnownAggr).val;if(_tmp3A4->kind == Cyc_Absyn_UnionA){_LL233: {
# 1680
struct Cyc_List_List*_tmp399=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3A4->impl))->fields;
int span=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp399);
struct _tuple2*_tmp39A=_tmp3A4->name;struct _tuple2*_tmp39B=_tmp39A;struct _dyneither_ptr _tmp3A1;_LL237: _tmp3A1=*_tmp39B->f2;_LL238:;
for(0;(unsigned int)_tmp399;_tmp399=_tmp399->tl){
struct _dyneither_ptr n=*((struct Cyc_Absyn_Aggrfield*)_tmp399->hd)->name;
if(!({struct Cyc_Set_Set*_tmp5EF=_tmp3A8;((int(*)(struct Cyc_Set_Set*s,struct Cyc_Tcpat_Con_s*elt))Cyc_Set_member)(_tmp5EF,({struct Cyc_Tcpat_Con_s*_tmp39C=_cycalloc(sizeof(*_tmp39C));({union Cyc_Tcpat_Name_value _tmp5EE=Cyc_Tcpat_Name_v(n);_tmp39C->name=_tmp5EE;});_tmp39C->arity=0;_tmp39C->span=0;_tmp39C->orig_pat=_tmp384->orig_pat;_tmp39C;}));}))
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp3A0;_tmp3A0.tag=0;_tmp3A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)n);({struct Cyc_String_pa_PrintArg_struct _tmp39F;_tmp39F.tag=0;_tmp39F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3A1);({void*_tmp39D[2]={& _tmp39F,& _tmp3A0};({struct _dyneither_ptr _tmp5F0=({const char*_tmp39E="%s{.%s = _}";_tag_dyneither(_tmp39E,sizeof(char),12);});Cyc_aprintf(_tmp5F0,_tag_dyneither(_tmp39D,sizeof(void*),2));});});});});}
# 1688
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}}else{goto _LL234;}}else{goto _LL234;}default: _LL234: _LL235:
(int)_throw((void*)& Cyc_Tcpat_Desc2string_val);}_LL229:;};};}_LL1F9:;}
# 1693
static struct _dyneither_ptr*Cyc_Tcpat_desc2stringptr(void*d){
return({struct _dyneither_ptr*_tmp3A9=_cycalloc(sizeof(*_tmp3A9));({struct _dyneither_ptr _tmp5F1=Cyc_Tcpat_desc2string(d);_tmp3A9[0]=_tmp5F1;});_tmp3A9;});}
# 1696
static struct _dyneither_ptr Cyc_Tcpat_descs2string(struct Cyc_List_List*ds){
struct Cyc_List_List*_tmp3AA=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_desc2stringptr,ds);
struct _dyneither_ptr*comma=({_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),",",sizeof(char),2);});
{struct Cyc_List_List*_tmp3AB=_tmp3AA;for(0;_tmp3AB != 0;_tmp3AB=((struct Cyc_List_List*)_check_null(_tmp3AB))->tl){
if(_tmp3AB->tl != 0){
({struct Cyc_List_List*_tmp5F2=({struct Cyc_List_List*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));_tmp3AC->hd=comma;_tmp3AC->tl=_tmp3AB->tl;_tmp3AC;});_tmp3AB->tl=_tmp5F2;});
_tmp3AB=_tmp3AB->tl;}}}
# 1705
return(struct _dyneither_ptr)Cyc_strconcat_l(_tmp3AA);}
# 1708
static void Cyc_Tcpat_not_exhaust_err(unsigned int loc,void*desc){
struct _handler_cons _tmp3AE;_push_handler(& _tmp3AE);{int _tmp3B0=0;if(setjmp(_tmp3AE.handler))_tmp3B0=1;if(!_tmp3B0){
{struct _dyneither_ptr _tmp3B1=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp3B4;_tmp3B4.tag=0;_tmp3B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3B1);({void*_tmp3B2[1]={& _tmp3B4};({unsigned int _tmp5F4=loc;struct _dyneither_ptr _tmp5F3=({const char*_tmp3B3="patterns may not be exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp3B3,sizeof(char),53);});Cyc_Tcutil_terr(_tmp5F4,_tmp5F3,_tag_dyneither(_tmp3B2,sizeof(void*),1));});});});}
# 1710
;_pop_handler();}else{void*_tmp3AF=(void*)_exn_thrown;void*_tmp3B5=_tmp3AF;void*_tmp3B8;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3B5)->tag == Cyc_Tcpat_Desc2string){_LL23A: _LL23B:
# 1714
({void*_tmp3B6=0;({unsigned int _tmp5F6=loc;struct _dyneither_ptr _tmp5F5=({const char*_tmp3B7="patterns may not be exhaustive.";_tag_dyneither(_tmp3B7,sizeof(char),32);});Cyc_Tcutil_terr(_tmp5F6,_tmp5F5,_tag_dyneither(_tmp3B6,sizeof(void*),0));});});
goto _LL239;}else{_LL23C: _tmp3B8=_tmp3B5;_LL23D:(int)_rethrow(_tmp3B8);}_LL239:;}};}
# 1718
static void Cyc_Tcpat_rule_occurs(int dummy,struct Cyc_Tcpat_Rhs*rhs){
rhs->used=1;}
# 1722
void Cyc_Tcpat_check_switch_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1728
int _tmp3B9=0;
int*_tmp3BA=& _tmp3B9;
struct Cyc_List_List*_tmp3BB=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(int*,struct Cyc_Absyn_Switch_clause*),int*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcpat_get_match,_tmp3BA,swcs);
void*_tmp3BC=Cyc_Tcpat_match_compile(_tmp3BB);
*dopt=_tmp3BC;{
# 1734
int _tmp3BD=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3BC,Cyc_Tcpat_not_exhaust_err,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3BD);
# 1737
for(0;_tmp3BB != 0;_tmp3BB=_tmp3BB->tl){
struct _tuple22*_tmp3BE=(struct _tuple22*)_tmp3BB->hd;struct _tuple22*_tmp3BF=_tmp3BE;int _tmp3C3;unsigned int _tmp3C2;_LL23F: _tmp3C3=(_tmp3BF->f2)->used;_tmp3C2=(_tmp3BF->f2)->pat_loc;_LL240:;
if(!_tmp3C3){
({void*_tmp3C0=0;({unsigned int _tmp5F8=_tmp3C2;struct _dyneither_ptr _tmp5F7=({const char*_tmp3C1="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp3C1,sizeof(char),74);});Cyc_Tcutil_terr(_tmp5F8,_tmp5F7,_tag_dyneither(_tmp3C0,sizeof(void*),0));});});
# 1742
break;}}};}
# 1751
static void Cyc_Tcpat_not_exhaust_warn(struct _tuple14*pr,void*desc){
(*pr).f2=0;{
struct _handler_cons _tmp3C4;_push_handler(& _tmp3C4);{int _tmp3C6=0;if(setjmp(_tmp3C4.handler))_tmp3C6=1;if(!_tmp3C6){
{struct _dyneither_ptr _tmp3C7=Cyc_Tcpat_desc2string(desc);
({struct Cyc_String_pa_PrintArg_struct _tmp3CA;_tmp3CA.tag=0;_tmp3CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp3C7);({void*_tmp3C8[1]={& _tmp3CA};({unsigned int _tmp5FA=(*pr).f1;struct _dyneither_ptr _tmp5F9=({const char*_tmp3C9="pattern not exhaustive.\n\tmissing case for %s";_tag_dyneither(_tmp3C9,sizeof(char),45);});Cyc_Tcutil_warn(_tmp5FA,_tmp5F9,_tag_dyneither(_tmp3C8,sizeof(void*),1));});});});}
# 1754
;_pop_handler();}else{void*_tmp3C5=(void*)_exn_thrown;void*_tmp3CB=_tmp3C5;void*_tmp3CE;if(((struct Cyc_Tcpat_Desc2string_exn_struct*)_tmp3CB)->tag == Cyc_Tcpat_Desc2string){_LL242: _LL243:
# 1758
({void*_tmp3CC=0;({unsigned int _tmp5FC=(*pr).f1;struct _dyneither_ptr _tmp5FB=({const char*_tmp3CD="pattern not exhaustive.";_tag_dyneither(_tmp3CD,sizeof(char),24);});Cyc_Tcutil_warn(_tmp5FC,_tmp5FB,_tag_dyneither(_tmp3CC,sizeof(void*),0));});});
goto _LL241;}else{_LL244: _tmp3CE=_tmp3CB;_LL245:(int)_rethrow(_tmp3CE);}_LL241:;}};};}
# 1762
static void Cyc_Tcpat_dummy_fn(int i,struct Cyc_Tcpat_Rhs*rhs){
return;}
# 1766
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**dopt){
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->used=0;_tmp3D6->pat_loc=p->loc;({struct Cyc_Absyn_Stmt*_tmp5FD=Cyc_Absyn_skip_stmt(0);_tmp3D6->rhs=_tmp5FD;});_tmp3D6;});
struct Cyc_List_List*_tmp3CF=({struct Cyc_List_List*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));({struct _tuple22*_tmp5FF=({struct _tuple22*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({void*_tmp5FE=Cyc_Tcpat_compile_pat(p);_tmp3D5->f1=_tmp5FE;});_tmp3D5->f2=rhs;_tmp3D5;});_tmp3D4->hd=_tmp5FF;});_tmp3D4->tl=0;_tmp3D4;});
void*_tmp3D0=Cyc_Tcpat_match_compile(_tmp3CF);
struct _tuple14 _tmp3D1=({struct _tuple14 _tmp3D3;_tmp3D3.f1=loc;_tmp3D3.f2=1;_tmp3D3;});
int _tmp3D2=0;
((void(*)(void*d,void(*not_exhaust)(struct _tuple14*,void*),struct _tuple14*env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3D0,Cyc_Tcpat_not_exhaust_warn,& _tmp3D1,Cyc_Tcpat_dummy_fn,0,& _tmp3D2);
# 1774
*dopt=_tmp3D0;
return _tmp3D1.f2;}
# 1782
static struct _tuple22*Cyc_Tcpat_get_match2(struct Cyc_Absyn_Switch_clause*swc){
void*sp0=Cyc_Tcpat_compile_pat(swc->pattern);
struct Cyc_Tcpat_Rhs*rhs=({struct Cyc_Tcpat_Rhs*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->used=0;_tmp3D8->pat_loc=(swc->pattern)->loc;_tmp3D8->rhs=swc->body;_tmp3D8;});
return({struct _tuple22*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));_tmp3D7->f1=sp0;_tmp3D7->f2=rhs;_tmp3D7;});}
# 1787
static void Cyc_Tcpat_not_exhaust_err2(unsigned int loc,void*d){;}
# 1790
void Cyc_Tcpat_check_catch_overlap(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*swcs,void**dopt){
# 1794
struct Cyc_List_List*_tmp3D9=((struct Cyc_List_List*(*)(struct _tuple22*(*f)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcpat_get_match2,swcs);
void*_tmp3DA=Cyc_Tcpat_match_compile(_tmp3D9);
*dopt=_tmp3DA;{
int _tmp3DB=0;
((void(*)(void*d,void(*not_exhaust)(unsigned int,void*),unsigned int env1,void(*rhs_appears)(int,struct Cyc_Tcpat_Rhs*),int env2,int*exhaust_done))Cyc_Tcpat_check_exhaust_overlap)(_tmp3DA,Cyc_Tcpat_not_exhaust_err2,loc,Cyc_Tcpat_rule_occurs,0,& _tmp3DB);
# 1800
for(0;_tmp3D9 != 0;_tmp3D9=_tmp3D9->tl){
# 1802
if(_tmp3D9->tl == 0)break;{
struct _tuple22*_tmp3DC=(struct _tuple22*)_tmp3D9->hd;struct _tuple22*_tmp3DD=_tmp3DC;int _tmp3E1;unsigned int _tmp3E0;_LL247: _tmp3E1=(_tmp3DD->f2)->used;_tmp3E0=(_tmp3DD->f2)->pat_loc;_LL248:;
if(!_tmp3E1){
({void*_tmp3DE=0;({unsigned int _tmp601=_tmp3E0;struct _dyneither_ptr _tmp600=({const char*_tmp3DF="redundant pattern (check for misspelled constructors in earlier patterns)";_tag_dyneither(_tmp3DF,sizeof(char),74);});Cyc_Tcutil_terr(_tmp601,_tmp600,_tag_dyneither(_tmp3DE,sizeof(void*),0));});});
break;}};}};}
# 1811
int Cyc_Tcpat_has_vars(struct Cyc_Core_Opt*pat_vars){
{struct Cyc_List_List*_tmp3E2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(pat_vars))->v;for(0;_tmp3E2 != 0;_tmp3E2=_tmp3E2->tl){
if((*((struct _tuple15*)_tmp3E2->hd)).f1 != 0)
return 1;}}
return 0;}
