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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 113
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 924
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 940
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 949
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 964
void*Cyc_Absyn_exn_typ();
# 984
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1105
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1109
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1161
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
# 68 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 92
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 96
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 98
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 102
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 120
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 123
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 126
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned int);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple13**oldtv,unsigned int loc);
# 133
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 135
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 147
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*te);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*te);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 32
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 55
void*Cyc_Tcutil_compress(void*t);
# 57
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 98
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 131
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 145
void Cyc_Tcutil_explain_failure();
# 147
int Cyc_Tcutil_unify(void*,void*);
# 150
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 174
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 178
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 213 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 221
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 272
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 277
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 294
int Cyc_Tcutil_new_tvar_id();
# 311
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 314
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 332
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 336
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 353
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 36
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 53 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,void**topt,struct Cyc_Absyn_Exp*pat_var_exp);
# 55
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Pat*p,struct Cyc_List_List*patvars);struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 107
void Cyc_Tcpat_check_switch_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 109
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*p,void**);
# 111
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct Cyc_Hashtable_Table;struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 49 "relations.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 48 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp7;struct Cyc_Absyn_Exp*_tmp6;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
({void*_tmp15F=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2=_cycalloc(sizeof(*_tmp2));({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp15E=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3;_tmp3.tag=5U;_tmp3.f1=_tmp6;_tmp3.f2=Cyc_Absyn_PreInc;_tmp3;});_tmp2[0]=_tmp15E;});_tmp2;});e->r=_tmp15F;});goto _LL0;case Cyc_Absyn_PostDec: _LL3: _tmp7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4:
({void*_tmp161=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp160=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp5;_tmp5.tag=5U;_tmp5.f1=_tmp7;_tmp5.f2=Cyc_Absyn_PreDec;_tmp5;});_tmp4[0]=_tmp160;});_tmp4;});e->r=_tmp161;});goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 61
static int Cyc_Tcstmt_noassign_exp(struct Cyc_Absyn_Exp*e){
void*_tmp8=e->r;void*_tmp9=_tmp8;struct Cyc_List_List*_tmp30;struct Cyc_List_List*_tmp2F;struct Cyc_List_List*_tmp2E;struct Cyc_List_List*_tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2A;struct Cyc_List_List*_tmp29;struct Cyc_Absyn_Exp*_tmp28;struct Cyc_Absyn_Exp*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpD;switch(*((int*)_tmp9)){case 10U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 35U: _LL5: _LL6:
 goto _LL8;case 40U: _LL7: _LL8:
 goto _LLA;case 37U: _LL9: _LLA:
 goto _LLC;case 5U: _LLB: _LLC:
 return 0;case 39U: _LLD: _LLE:
# 70
 goto _LL10;case 0U: _LLF: _LL10:
 goto _LL12;case 1U: _LL11: _LL12:
 goto _LL14;case 17U: _LL13: _LL14:
 goto _LL16;case 19U: _LL15: _LL16:
 goto _LL18;case 32U: _LL17: _LL18:
 goto _LL1A;case 33U: _LL19: _LL1A:
 goto _LL1C;case 2U: _LL1B: _LL1C:
 return 1;case 6U: _LL1D: _tmpF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_tmpE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_tmpD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9)->f3;_LL1E:
# 80
 if(!Cyc_Tcstmt_noassign_exp(_tmpF))return 0;
_tmp11=_tmpE;_tmp10=_tmpD;goto _LL20;case 27U: _LL1F: _tmp11=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_tmp10=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp9)->f3;_LL20:
 _tmp13=_tmp11;_tmp12=_tmp10;goto _LL22;case 7U: _LL21: _tmp13=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_tmp12=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL22:
 _tmp15=_tmp13;_tmp14=_tmp12;goto _LL24;case 8U: _LL23: _tmp15=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_tmp14=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL24:
 _tmp17=_tmp15;_tmp16=_tmp14;goto _LL26;case 23U: _LL25: _tmp17=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_tmp16=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL26:
 _tmp19=_tmp17;_tmp18=_tmp16;goto _LL28;case 9U: _LL27: _tmp19=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_tmp18=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL28:
 return Cyc_Tcstmt_noassign_exp(_tmp19) && Cyc_Tcstmt_noassign_exp(_tmp18);case 38U: _LL29: _tmp1A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL2A:
# 88
 _tmp1B=_tmp1A;goto _LL2C;case 11U: _LL2B: _tmp1B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL2C:
 _tmp1C=_tmp1B;goto _LL2E;case 12U: _LL2D: _tmp1C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL2E:
 _tmp1D=_tmp1C;goto _LL30;case 13U: _LL2F: _tmp1D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL30:
 _tmp1E=_tmp1D;goto _LL32;case 14U: _LL31: _tmp1E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL32:
 _tmp1F=_tmp1E;goto _LL34;case 18U: _LL33: _tmp1F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL34:
 _tmp20=_tmp1F;goto _LL36;case 20U: _LL35: _tmp20=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL36:
 _tmp21=_tmp20;goto _LL38;case 21U: _LL37: _tmp21=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL38:
 _tmp22=_tmp21;goto _LL3A;case 22U: _LL39: _tmp22=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL3A:
 _tmp23=_tmp22;goto _LL3C;case 28U: _LL3B: _tmp23=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL3C:
 _tmp24=_tmp23;goto _LL3E;case 15U: _LL3D: _tmp24=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL3E:
 return Cyc_Tcstmt_noassign_exp(_tmp24);case 34U: _LL3F: _tmp26=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp9)->f1).rgn;_tmp25=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp9)->f1).num_elts;_LL40:
# 101
 _tmp28=_tmp26;_tmp27=_tmp25;goto _LL42;case 16U: _LL41: _tmp28=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_tmp27=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL42:
# 103
 if(!Cyc_Tcstmt_noassign_exp(_tmp27))return 0;
if(_tmp28 != 0)return Cyc_Tcstmt_noassign_exp(_tmp28);else{
return 1;}case 3U: _LL43: _tmp29=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL44:
# 107
 _tmp2A=_tmp29;goto _LL46;case 31U: _LL45: _tmp2A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL46:
 _tmp2B=_tmp2A;goto _LL48;case 24U: _LL47: _tmp2B=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL48:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_noassign_exp,_tmp2B);case 36U: _LL49: _tmp2C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL4A:
# 111
 _tmp2D=_tmp2C;goto _LL4C;case 29U: _LL4B: _tmp2D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp9)->f3;_LL4C:
 _tmp2E=_tmp2D;goto _LL4E;case 30U: _LL4D: _tmp2E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL4E:
 _tmp2F=_tmp2E;goto _LL50;case 26U: _LL4F: _tmp2F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp9)->f1;_LL50:
 _tmp30=_tmp2F;goto _LL52;default: _LL51: _tmp30=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp9)->f2;_LL52:
# 116
 for(0;_tmp30 != 0;_tmp30=_tmp30->tl){
struct _tuple15*_tmpA=(struct _tuple15*)_tmp30->hd;struct _tuple15*_tmpB=_tmpA;struct Cyc_Absyn_Exp*_tmpC;_LL54: _tmpC=_tmpB->f2;_LL55:;
if(!Cyc_Tcstmt_noassign_exp(_tmpC))return 0;}
# 120
return 1;}_LL0:;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 125
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 127
struct Cyc_Tcpat_TcPatResult _tmp31=pat_res;struct _tuple1*_tmp3E;struct Cyc_List_List*_tmp3D;_LL1: _tmp3E=_tmp31.tvars_and_bounds_opt;_tmp3D=_tmp31.patvars;_LL2:;{
struct Cyc_List_List*_tmp32=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple17*))Cyc_Core_fst,_tmp3D);
struct Cyc_List_List*_tmp33=_tmp3E == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*_tmp3E).f1);
struct Cyc_List_List*_tmp34=_tmp3E == 0?0:(*_tmp3E).f2;
struct Cyc_List_List*_tmp35=_tmp3E == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple16*))Cyc_Core_snd,(*_tmp3E).f1));
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmp33);
for(0;_tmp35 != 0;_tmp35=_tmp35->tl){
({struct Cyc_Tcenv_Tenv*_tmp164=({struct Cyc_Tcenv_Tenv*_tmp163=te2;Cyc_Tcenv_add_region(_tmp163,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp36=_cycalloc(sizeof(*_tmp36));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp162=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp37;_tmp37.tag=2U;_tmp37.f1=(struct Cyc_Absyn_Tvar*)_tmp35->hd;_tmp37;});_tmp36[0]=_tmp162;});_tmp36;}),1);});te2=_tmp164;});}
({struct Cyc_Tcenv_Tenv*_tmp165=Cyc_Tcenv_new_outlives_constraints(te2,_tmp34,loc);te2=_tmp165;});
if(new_block)
({struct Cyc_Tcenv_Tenv*_tmp166=Cyc_Tcenv_new_block(loc,te2);te2=_tmp166;});{
void*_tmp38=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp39=_tmp32;for(0;_tmp39 != 0;_tmp39=_tmp39->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp39->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp39->hd)))->rgn=_tmp38;}}
# 143
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp168=te2;struct Cyc_Absyn_Exp*_tmp167=where_opt;Cyc_Tcexp_tcTest(_tmp168,_tmp167,({const char*_tmp3A="switch clause guard";_tag_dyneither(_tmp3A,sizeof(char),20U);}));});
if(!Cyc_Tcstmt_noassign_exp(where_opt))
({void*_tmp3B=0U;({unsigned int _tmp16A=where_opt->loc;struct _dyneither_ptr _tmp169=({const char*_tmp3C="cannot show &&-clause in pattern has no effects";_tag_dyneither(_tmp3C,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp16A,_tmp169,_tag_dyneither(_tmp3B,sizeof(void*),0U));});});}
# 148
Cyc_Tcstmt_tcStmt(te2,s,0);};};}
# 153
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 155
struct _tuple13*tk=0;
struct Cyc_Tcenv_Tenv*_tmp3F=Cyc_Tcenv_add_region_equality(te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp3F,s0,new_block);
if(tk != 0)
({void*_tmp16B=(*((struct _tuple13*)_check_null(tk))).f2;((*((struct _tuple13*)_check_null(tk))).f1)->kind=_tmp16B;});}
# 162
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp40=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _tuple2*res=({struct _tuple2*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp16E=({struct _dyneither_ptr*_tmp42=_cycalloc(sizeof(*_tmp42));({struct _dyneither_ptr _tmp16D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp45;_tmp45.tag=1U;_tmp45.f1=(unsigned int)_tmp40;({void*_tmp43[1U]={& _tmp45};({struct _dyneither_ptr _tmp16C=({const char*_tmp44="_stmttmp%X";_tag_dyneither(_tmp44,sizeof(char),11U);});Cyc_aprintf(_tmp16C,_tag_dyneither(_tmp43,sizeof(void*),1U));});});});_tmp42[0]=_tmp16D;});_tmp42;});_tmp41->f2=_tmp16E;});_tmp41;});
return res;}
# 176 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 178
void*_tmp46=s0->r;void*_tmp47=_tmp46;struct Cyc_Absyn_Decl*_tmp15D;struct Cyc_Absyn_Stmt*_tmp15C;struct Cyc_Absyn_Stmt*_tmp15B;struct Cyc_List_List**_tmp15A;void**_tmp159;struct Cyc_Absyn_Exp**_tmp158;struct Cyc_List_List*_tmp157;void**_tmp156;struct _dyneither_ptr*_tmp155;struct Cyc_Absyn_Stmt*_tmp154;struct Cyc_List_List*_tmp153;struct Cyc_Absyn_Switch_clause***_tmp152;struct Cyc_Absyn_Stmt*_tmp151;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_Stmt*_tmp14F;struct Cyc_Absyn_Exp*_tmp14E;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Stmt*_tmp14C;struct Cyc_Absyn_Exp*_tmp14B;struct Cyc_Absyn_Stmt*_tmp14A;struct Cyc_Absyn_Stmt*_tmp149;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Stmt*_tmp147;struct Cyc_Absyn_Stmt*_tmp146;struct Cyc_Absyn_Exp*_tmp145;struct Cyc_Absyn_Stmt*_tmp144;struct Cyc_Absyn_Stmt*_tmp143;struct Cyc_Absyn_Exp*_tmp142;struct Cyc_Absyn_Stmt*_tmp141;struct Cyc_Absyn_Stmt*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;switch(*((int*)_tmp47)){case 0U: _LL1: _LL2:
# 180
 return;case 1U: _LL3: _tmp13F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_LL4:
# 183
 Cyc_Tcexp_tcExp(te,0,_tmp13F);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(_tmp13F);
return;case 2U: _LL5: _tmp141=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp140=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL6:
# 189
 Cyc_Tcstmt_tcStmt(te,_tmp141,1);
Cyc_Tcstmt_tcStmt(te,_tmp140,1);
return;case 3U: _LL7: _tmp142=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_LL8: {
# 194
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp142 == 0){
void*_tmp48=Cyc_Tcutil_compress(t);void*_tmp49=_tmp48;switch(*((int*)_tmp49)){case 0U: _LL22: _LL23:
 goto _LL21;case 7U: _LL24: _LL25:
 goto _LL27;case 6U: _LL26: _LL27:
# 200
({struct Cyc_String_pa_PrintArg_struct _tmp4C;_tmp4C.tag=0U;({struct _dyneither_ptr _tmp16F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4C.f1=_tmp16F;});({void*_tmp4A[1U]={& _tmp4C};({unsigned int _tmp171=s0->loc;struct _dyneither_ptr _tmp170=({const char*_tmp4B="should return a value of type %s";_tag_dyneither(_tmp4B,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp171,_tmp170,_tag_dyneither(_tmp4A,sizeof(void*),1U));});});});goto _LL21;default: _LL28: _LL29:
# 202
({struct Cyc_String_pa_PrintArg_struct _tmp4F;_tmp4F.tag=0U;({struct _dyneither_ptr _tmp172=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp4F.f1=_tmp172;});({void*_tmp4D[1U]={& _tmp4F};({unsigned int _tmp174=s0->loc;struct _dyneither_ptr _tmp173=({const char*_tmp4E="must return a value of type %s";_tag_dyneither(_tmp4E,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp174,_tmp173,_tag_dyneither(_tmp4D,sizeof(void*),1U));});});});goto _LL21;}_LL21:;}else{
# 206
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp142;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp53;_tmp53.tag=0U;({struct _dyneither_ptr _tmp175=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp53.f1=_tmp175;});({struct Cyc_String_pa_PrintArg_struct _tmp52;_tmp52.tag=0U;({struct _dyneither_ptr _tmp176=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp52.f1=_tmp176;});({void*_tmp50[2U]={& _tmp52,& _tmp53};({unsigned int _tmp178=s0->loc;struct _dyneither_ptr _tmp177=({const char*_tmp51="returns value of type %s but requires %s";_tag_dyneither(_tmp51,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp178,_tmp177,_tag_dyneither(_tmp50,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}
# 214
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
({void*_tmp54=0U;({unsigned int _tmp17A=e->loc;struct _dyneither_ptr _tmp179=({const char*_tmp55="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp55,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp17A,_tmp179,_tag_dyneither(_tmp54,sizeof(void*),0U));});});}
# 217
return;}case 4U: _LL9: _tmp145=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp144=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_tmp143=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp47)->f3;_LLA: {
# 220
struct Cyc_Tcexp_TestEnv _tmp56=({struct Cyc_Tcenv_Tenv*_tmp17C=te;struct Cyc_Absyn_Exp*_tmp17B=_tmp145;Cyc_Tcexp_tcTest(_tmp17C,_tmp17B,({const char*_tmp5D="if statement";_tag_dyneither(_tmp5D,sizeof(char),13U);}));});struct Cyc_Tcexp_TestEnv _tmp57=_tmp56;struct _tuple0*_tmp5C;int _tmp5B;_LL2B: _tmp5C=_tmp57.eq;_tmp5B=_tmp57.isTrue;_LL2C:;
{struct _tuple0*_tmp58=_tmp5C;void*_tmp5A;void*_tmp59;if(_tmp58 != 0){_LL2E: _tmp5A=_tmp58->f1;_tmp59=_tmp58->f2;_LL2F:
# 223
 if(_tmp5B){
Cyc_Tcstmt_tcStmtRefine(te,_tmp144,_tmp5A,_tmp59,1);
Cyc_Tcstmt_tcStmt(te,_tmp143,1);}else{
# 228
Cyc_Tcstmt_tcStmt(te,_tmp144,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp143,_tmp5A,_tmp59,1);}
# 231
goto _LL2D;}else{_LL30: _LL31:
# 233
 Cyc_Tcstmt_tcStmt(te,_tmp144,1);
Cyc_Tcstmt_tcStmt(te,_tmp143,1);
goto _LL2D;}_LL2D:;}
# 237
return;}case 5U: _LLB: _tmp148=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp47)->f1).f1;_tmp147=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp47)->f1).f2;_tmp146=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LLC:
# 240
({struct Cyc_Tcenv_Tenv*_tmp17E=te;struct Cyc_Absyn_Exp*_tmp17D=_tmp148;Cyc_Tcexp_tcTest(_tmp17E,_tmp17D,({const char*_tmp5E="while loop";_tag_dyneither(_tmp5E,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp146,1);
return;case 9U: _LLD: _tmp14E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp14D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f1;_tmp14C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f2;_tmp14B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f3).f1;_tmp14A=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f3).f2;_tmp149=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp47)->f4;_LLE:
# 245
 Cyc_Tcexp_tcExp(te,0,_tmp14E);
({struct Cyc_Tcenv_Tenv*_tmp180=te;struct Cyc_Absyn_Exp*_tmp17F=_tmp14D;Cyc_Tcexp_tcTest(_tmp180,_tmp17F,({const char*_tmp5F="for loop";_tag_dyneither(_tmp5F,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp149,1);
Cyc_Tcexp_tcExp(te,0,_tmp14B);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp14B);
return;case 14U: _LLF: _tmp151=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp150=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f1;_tmp14F=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp47)->f2).f2;_LL10:
# 253
 Cyc_Tcstmt_tcStmt(te,_tmp151,1);
({struct Cyc_Tcenv_Tenv*_tmp182=te;struct Cyc_Absyn_Exp*_tmp181=_tmp150;Cyc_Tcexp_tcTest(_tmp182,_tmp181,({const char*_tmp60="do loop";_tag_dyneither(_tmp60,sizeof(char),8U);}));});
return;case 6U: _LL11: _LL12:
# 257
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp153=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp152=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL18: {
# 262
const struct _tuple12*_tmp61=Cyc_Tcenv_process_fallthru(te,s0,_tmp152);
if(_tmp61 == (const struct _tuple12*)0){
({void*_tmp62=0U;({unsigned int _tmp184=s0->loc;struct _dyneither_ptr _tmp183=({const char*_tmp63="fallthru not in a non-last case";_tag_dyneither(_tmp63,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp184,_tmp183,_tag_dyneither(_tmp62,sizeof(void*),0U));});});
return;}{
# 267
struct Cyc_List_List*_tmp64=(*_tmp61).f2;
struct Cyc_List_List*_tmp65=(*_tmp61).f3;
struct Cyc_List_List*instantiation=({
struct Cyc_List_List*_tmp185=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp185,_tmp64);});
struct Cyc_List_List*_tmp66=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,_tmp65);
for(0;_tmp66 != 0  && _tmp153 != 0;(_tmp66=_tmp66->tl,_tmp153=_tmp153->tl)){
# 274
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp153->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp153->hd,(void*)_tmp66->hd,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp6A;_tmp6A.tag=0U;({struct _dyneither_ptr _tmp186=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 279
Cyc_Absynpp_typ2string((void*)_tmp66->hd));_tmp6A.f1=_tmp186;});({struct Cyc_String_pa_PrintArg_struct _tmp69;_tmp69.tag=0U;({struct _dyneither_ptr _tmp187=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp153->hd)->topt)));_tmp69.f1=_tmp187;});({void*_tmp67[2U]={& _tmp69,& _tmp6A};({unsigned int _tmp189=s0->loc;struct _dyneither_ptr _tmp188=({const char*_tmp68="fallthru argument has type %s but pattern variable has type %s";_tag_dyneither(_tmp68,sizeof(char),63U);});Cyc_Tcutil_terr(_tmp189,_tmp188,_tag_dyneither(_tmp67,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}
# 283
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_tmp66->hd) && !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp153->hd))
({void*_tmp6B=0U;({unsigned int _tmp18B=((struct Cyc_Absyn_Exp*)_tmp153->hd)->loc;struct _dyneither_ptr _tmp18A=({const char*_tmp6C="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp6C,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp18B,_tmp18A,_tag_dyneither(_tmp6B,sizeof(void*),0U));});});}
# 286
if(_tmp153 != 0)
({void*_tmp6D=0U;({unsigned int _tmp18D=s0->loc;struct _dyneither_ptr _tmp18C=({const char*_tmp6E="too many arguments in explicit fallthru";_tag_dyneither(_tmp6E,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp18D,_tmp18C,_tag_dyneither(_tmp6D,sizeof(void*),0U));});});
if(_tmp66 != 0)
({void*_tmp6F=0U;({unsigned int _tmp18F=s0->loc;struct _dyneither_ptr _tmp18E=({const char*_tmp70="too few arguments in explicit fallthru";_tag_dyneither(_tmp70,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp18F,_tmp18E,_tag_dyneither(_tmp6F,sizeof(void*),0U));});});
return;};}case 13U: _LL19: _tmp155=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp154=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL1A:
# 296
({struct Cyc_Tcenv_Tenv*_tmp197=({unsigned int _tmp196=s0->loc;struct Cyc_Tcenv_Tenv*_tmp195=te;Cyc_Tcenv_new_named_block(_tmp196,_tmp195,({struct Cyc_Absyn_Tvar*_tmp71=_cycalloc(sizeof(*_tmp71));({struct _dyneither_ptr*_tmp192=({struct _dyneither_ptr*_tmp72=_cycalloc(sizeof(*_tmp72));({struct _dyneither_ptr _tmp191=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp75;_tmp75.tag=0U;_tmp75.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp155);({void*_tmp73[1U]={& _tmp75};({struct _dyneither_ptr _tmp190=({const char*_tmp74="`%s";_tag_dyneither(_tmp74,sizeof(char),4U);});Cyc_aprintf(_tmp190,_tag_dyneither(_tmp73,sizeof(void*),1U));});});});_tmp72[0]=_tmp191;});_tmp72;});_tmp71->name=_tmp192;});({int _tmp193=
# 298
Cyc_Tcutil_new_tvar_id();_tmp71->identity=_tmp193;});({void*_tmp194=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp71->kind=_tmp194;});_tmp71;}));});
# 296
Cyc_Tcstmt_tcStmt(_tmp197,_tmp154,0);});
# 300
return;case 10U: _LL1B: _tmp158=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp157=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_tmp156=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp47)->f3;_LL1C: {
# 305
struct Cyc_Absyn_Exp*_tmp76=*_tmp158;
if(!Cyc_Tcutil_is_var_exp(_tmp76)){
struct Cyc_Absyn_Vardecl*_tmp77=({struct _tuple2*_tmp198=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp198,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmp78=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmp79=({struct Cyc_Absyn_Pat*_tmp19D=({void*_tmp19B=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp19A=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp7E;_tmp7E.tag=1U;_tmp7E.f1=_tmp77;({struct Cyc_Absyn_Pat*_tmp199=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp76->loc);_tmp7E.f2=_tmp199;});_tmp7E;});_tmp7D[0]=_tmp19A;});_tmp7D;});Cyc_Absyn_new_pat(_tmp19B,_tmp76->loc);});struct Cyc_Absyn_Exp*_tmp19C=_tmp76;Cyc_Absyn_let_decl(_tmp19D,_tmp19C,s0->loc);});
# 312
struct Cyc_Absyn_Stmt*_tmp7A=Cyc_Absyn_decl_stmt(_tmp79,_tmp78,s0->loc);
s0->r=_tmp7A->r;
({struct Cyc_Absyn_Exp*_tmp1A0=({void*_tmp19F=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp19E=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp7C;_tmp7C.tag=4U;_tmp7C.f1=_tmp77;_tmp7C;});_tmp7B[0]=_tmp19E;});_tmp7B;});Cyc_Absyn_varb_exp(_tmp19F,_tmp76->loc);});*_tmp158=_tmp1A0;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 319
void*_tmp7F=Cyc_Tcexp_tcExp(te,0,_tmp76);
# 321
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp7F) && !Cyc_Tcutil_is_noalias_path(_tmp76))
({void*_tmp80=0U;({unsigned int _tmp1A2=_tmp76->loc;struct _dyneither_ptr _tmp1A1=({const char*_tmp81="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp81,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp1A2,_tmp1A1,_tag_dyneither(_tmp80,sizeof(void*),0U));});});{
# 326
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp82=_tmp157;for(0;_tmp82 != 0;_tmp82=_tmp82->tl){
if(first_case){
first_case=0;
({struct Cyc_Tcpat_TcPatResult _tmp1A3=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp82->hd)->pattern,& _tmp7F,_tmp76);this_pattern=_tmp1A3;});}else{
# 334
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 336
if(_tmp82->tl != 0){
({struct Cyc_Tcpat_TcPatResult*_tmp1A5=({struct Cyc_Tcpat_TcPatResult*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Tcpat_TcPatResult _tmp1A4=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp82->tl))->hd)->pattern,& _tmp7F,_tmp76);_tmp83[0]=_tmp1A4;});_tmp83;});next_pattern=_tmp1A5;});
if(next_pattern->tvars_and_bounds_opt != 0  && (*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 340
({struct Cyc_Tcenv_Tenv*_tmp1A6=Cyc_Tcenv_clear_fallthru(te);te=_tmp1A6;});else{
# 342
struct Cyc_List_List*_tmp84=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp85=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
({struct Cyc_Tcenv_Tenv*_tmp1A7=Cyc_Tcenv_set_fallthru(te,_tmp85,_tmp84,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp82->tl))->hd);te=_tmp1A7;});}}else{
# 348
({struct Cyc_Tcenv_Tenv*_tmp1A8=Cyc_Tcenv_clear_fallthru(te);te=_tmp1A8;});}{
# 350
struct Cyc_Absyn_Pat*_tmp86=((struct Cyc_Absyn_Switch_clause*)_tmp82->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp86->topt),_tmp7F)){
({struct Cyc_String_pa_PrintArg_struct _tmp8A;_tmp8A.tag=0U;({struct _dyneither_ptr _tmp1A9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp86->topt)));_tmp8A.f1=_tmp1A9;});({struct Cyc_String_pa_PrintArg_struct _tmp89;_tmp89.tag=0U;({struct _dyneither_ptr _tmp1AA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7F));_tmp89.f1=_tmp1AA;});({void*_tmp87[2U]={& _tmp89,& _tmp8A};({unsigned int _tmp1AC=((struct Cyc_Absyn_Switch_clause*)_tmp82->hd)->loc;struct _dyneither_ptr _tmp1AB=({const char*_tmp88="switch on type %s, but case expects type %s";_tag_dyneither(_tmp88,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1AC,_tmp1AB,_tag_dyneither(_tmp87,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}else{
# 357
Cyc_Tcpat_check_pat_regions(te,_tmp86,this_pattern.patvars);}
# 359
({struct Cyc_Core_Opt*_tmp1AD=({struct Cyc_Core_Opt*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->v=this_pattern.patvars;_tmp8B;});((struct Cyc_Absyn_Switch_clause*)_tmp82->hd)->pat_vars=_tmp1AD;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp82->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp82->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp82->hd)->where_clause,1);};}}
# 363
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp157,_tmp156);
return;};};}case 15U: _LL1D: _tmp15B=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp15A=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_tmp159=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp47)->f3;_LL1E:
# 371
({void*_tmp1B2=({void*_tmp1B1=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1B0=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp8D;_tmp8D.tag=2U;({struct Cyc_Absyn_Stmt*_tmp1AE=Cyc_Absyn_new_stmt(_tmp15B->r,_tmp15B->loc);_tmp8D.f1=_tmp1AE;});({struct Cyc_Absyn_Stmt*_tmp1AF=Cyc_Absyn_skip_stmt(_tmp15B->loc);_tmp8D.f2=_tmp1AF;});_tmp8D;});_tmp8C[0]=_tmp1B0;});_tmp8C;});Cyc_Absyn_new_stmt(_tmp1B1,_tmp15B->loc);})->r;_tmp15B->r=_tmp1B2;});
# 373
Cyc_Tcstmt_tcStmt(te,_tmp15B,1);{
# 376
struct _tuple2*def_v=({struct _tuple2*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp1B3=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"exn",sizeof(char),4U);_tmpA5->f2=_tmp1B3;});_tmpA5;});
struct Cyc_Absyn_Vardecl*_tmp8E=Cyc_Absyn_new_vardecl(0U,def_v,(void*)& Cyc_Absyn_VoidType_val,0);
struct Cyc_Absyn_Pat*_tmp8F=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1B5=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpA4;_tmpA4.tag=1U;_tmpA4.f1=_tmp8E;({struct Cyc_Absyn_Pat*_tmp1B4=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmpA4.f2=_tmp1B4;});_tmpA4;});_tmpA3[0]=_tmp1B5;});_tmpA3;}),0U);
struct Cyc_Absyn_Stmt*_tmp90=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp1B8=({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmpA0;_tmpA0.tag=11U;({struct Cyc_Absyn_Exp*_tmp1B7=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp1B6=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpA2;_tmpA2.tag=4U;_tmpA2.f1=_tmp8E;_tmpA2;});_tmpA1[0]=_tmp1B6;});_tmpA1;}),0U);_tmpA0.f1=_tmp1B7;});_tmpA0.f2=1;_tmpA0;});_tmp9F[0]=_tmp1B8;});_tmp9F;}),0U),0U);
struct Cyc_Absyn_Switch_clause*_tmp91=({struct Cyc_Absyn_Switch_clause*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->pattern=_tmp8F;_tmp9E->pat_vars=0;_tmp9E->where_clause=0;_tmp9E->body=_tmp90;_tmp9E->loc=0U;_tmp9E;});
({struct Cyc_List_List*_tmp1BA=({struct Cyc_List_List*_tmp1B9=*_tmp15A;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1B9,({struct Cyc_Absyn_Switch_clause*_tmp92[1U];_tmp92[0U]=_tmp91;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp92,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*_tmp15A=_tmp1BA;});{
# 388
void*_tmp93=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp94=*_tmp15A;for(0;_tmp94 != 0;_tmp94=_tmp94->tl){
if(first_case){
first_case=0;
({struct Cyc_Tcpat_TcPatResult _tmp1BB=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp94->hd)->pattern,& _tmp93,0);this_pattern=_tmp1BB;});}else{
# 397
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 399
if(_tmp94->tl != 0){
({struct Cyc_Tcpat_TcPatResult*_tmp1BD=({struct Cyc_Tcpat_TcPatResult*_tmp95=_cycalloc(sizeof(*_tmp95));({struct Cyc_Tcpat_TcPatResult _tmp1BC=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp94->tl))->hd)->pattern,& _tmp93,0);_tmp95[0]=_tmp1BC;});_tmp95;});next_pattern=_tmp1BD;});
if(next_pattern->tvars_and_bounds_opt != 0  && (*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 403
({struct Cyc_Tcenv_Tenv*_tmp1BE=Cyc_Tcenv_clear_fallthru(te);te=_tmp1BE;});else{
# 405
struct Cyc_List_List*_tmp96=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp97=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
({struct Cyc_Tcenv_Tenv*_tmp1BF=Cyc_Tcenv_set_fallthru(te,_tmp97,_tmp96,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp94->tl))->hd);te=_tmp1BF;});}}else{
# 411
({struct Cyc_Tcenv_Tenv*_tmp1C0=Cyc_Tcenv_clear_fallthru(te);te=_tmp1C0;});}{
# 413
struct Cyc_Absyn_Pat*_tmp98=((struct Cyc_Absyn_Switch_clause*)_tmp94->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp98->topt),_tmp93)){
({struct Cyc_String_pa_PrintArg_struct _tmp9C;_tmp9C.tag=0U;({struct _dyneither_ptr _tmp1C1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp98->topt)));_tmp9C.f1=_tmp1C1;});({struct Cyc_String_pa_PrintArg_struct _tmp9B;_tmp9B.tag=0U;({struct _dyneither_ptr _tmp1C2=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp93));_tmp9B.f1=_tmp1C2;});({void*_tmp99[2U]={& _tmp9B,& _tmp9C};({unsigned int _tmp1C4=((struct Cyc_Absyn_Switch_clause*)_tmp94->hd)->loc;struct _dyneither_ptr _tmp1C3=({const char*_tmp9A="switch on type %s, but case expects type %s";_tag_dyneither(_tmp9A,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1C4,_tmp1C3,_tag_dyneither(_tmp99,sizeof(void*),2U));});});});});
Cyc_Tcutil_explain_failure();}else{
# 420
Cyc_Tcpat_check_pat_regions(te,_tmp98,this_pattern.patvars);}
# 422
({struct Cyc_Core_Opt*_tmp1C5=({struct Cyc_Core_Opt*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->v=this_pattern.patvars;_tmp9D;});((struct Cyc_Absyn_Switch_clause*)_tmp94->hd)->pat_vars=_tmp1C5;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp94->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp94->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp94->hd)->where_clause,1);};}}
# 426
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*_tmp15A,_tmp159);
return;};};default: _LL1F: _tmp15D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47)->f1;_tmp15C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp47)->f2;_LL20: {
# 430
struct _dyneither_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmpA7=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;
{void*_tmpA8=_tmp15D->r;void*_tmpA9=_tmpA8;struct _tuple2*_tmp13B;struct Cyc_List_List*_tmp13A;struct _dyneither_ptr*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Fndecl*_tmp137;struct Cyc_Absyn_Tvar*_tmp136;struct Cyc_Absyn_Vardecl*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_Pat*_tmp132;struct Cyc_Core_Opt**_tmp131;struct Cyc_Absyn_Exp**_tmp130;void**_tmp12F;struct Cyc_Absyn_Vardecl*_tmp12E;switch(*((int*)_tmpA9)){case 0U: _LL33: _tmp12E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA9)->f1;_LL34: {
# 434
struct Cyc_Absyn_Vardecl*_tmpAA=_tmp12E;enum Cyc_Absyn_Scope _tmpDD;union Cyc_Absyn_Nmspace _tmpDC;struct _dyneither_ptr*_tmpDB;struct Cyc_Absyn_Tqual _tmpDA;void*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;void**_tmpD7;struct Cyc_List_List*_tmpD6;_LL52: _tmpDD=_tmpAA->sc;_tmpDC=(_tmpAA->name)->f1;_tmpDB=(_tmpAA->name)->f2;_tmpDA=_tmpAA->tq;_tmpD9=_tmpAA->type;_tmpD8=_tmpAA->initializer;_tmpD7=(void**)& _tmpAA->rgn;_tmpD6=_tmpAA->attributes;_LL53:;{
void*_tmpAB=Cyc_Tcenv_curr_rgn(_tmpA7);
int is_local;
{enum Cyc_Absyn_Scope _tmpAC=_tmpDD;switch(_tmpAC){case Cyc_Absyn_Static: _LL55: _LL56:
 goto _LL58;case Cyc_Absyn_Extern: _LL57: _LL58:
 goto _LL5A;case Cyc_Absyn_ExternC: _LL59: _LL5A:
# 441
 _tmp12E->escapes=1;
is_local=0;goto _LL54;case Cyc_Absyn_Abstract: _LL5B: _LL5C:
# 444
({void*_tmpAD=0U;({unsigned int _tmp1C7=_tmp15D->loc;struct _dyneither_ptr _tmp1C6=({const char*_tmpAE="bad abstract scope for local variable";_tag_dyneither(_tmpAE,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp1C7,_tmp1C6,_tag_dyneither(_tmpAD,sizeof(void*),0U));});});
goto _LL5E;case Cyc_Absyn_Register: _LL5D: _LL5E:
 goto _LL60;default: _LL5F: _LL60:
 is_local=1;goto _LL54;}_LL54:;}
# 450
*_tmpD7=is_local?_tmpAB:(void*)& Cyc_Absyn_HeapRgn_val;
# 452
{void*_tmpAF=Cyc_Tcutil_compress(_tmpD9);void*_tmpB0=_tmpAF;void*_tmpBC;struct Cyc_Absyn_Tqual _tmpBB;union Cyc_Absyn_Constraint*_tmpBA;unsigned int _tmpB9;switch(*((int*)_tmpB0)){case 9U: _LL62: if(is_local){_LL63:
# 455
 _tmp12E->escapes=1;
_tmpDD=Cyc_Absyn_Extern;
is_local=0;
goto _LL61;}else{goto _LL66;}case 8U: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0)->f1).num_elts == 0){_LL64: _tmpBC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0)->f1).elt_type;_tmpBB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0)->f1).tq;_tmpBA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0)->f1).zero_term;_tmpB9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0)->f1).zt_loc;if(_tmp12E->initializer != 0){_LL65:
# 460
{void*_tmpB1=((struct Cyc_Absyn_Exp*)_check_null(_tmp12E->initializer))->r;void*_tmpB2=_tmpB1;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;struct Cyc_Absyn_Exp*_tmpB6;struct Cyc_Absyn_Exp*_tmpB5;struct _dyneither_ptr _tmpB4;struct _dyneither_ptr _tmpB3;switch(*((int*)_tmpB2)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB2)->f1).Wstring_c).tag){case 8U: _LL69: _tmpB3=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB2)->f1).String_c).val;_LL6A:
# 462
({void*_tmp1CD=({void*_tmp1CC=({void*_tmp1CB=_tmpBC;struct Cyc_Absyn_Tqual _tmp1CA=_tmpBB;struct Cyc_Absyn_Exp*_tmp1C9=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpB3,sizeof(char)),0U);union Cyc_Absyn_Constraint*_tmp1C8=_tmpBA;Cyc_Absyn_array_typ(_tmp1CB,_tmp1CA,_tmp1C9,_tmp1C8,_tmpB9);});_tmp12E->type=_tmp1CC;});_tmpD9=_tmp1CD;});
goto _LL68;case 9U: _LL6B: _tmpB4=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB2)->f1).Wstring_c).val;_LL6C:
# 465
({void*_tmp1D3=({void*_tmp1D2=({void*_tmp1D1=_tmpBC;struct Cyc_Absyn_Tqual _tmp1D0=_tmpBB;struct Cyc_Absyn_Exp*_tmp1CF=Cyc_Absyn_uint_exp(1U,0U);union Cyc_Absyn_Constraint*_tmp1CE=_tmpBA;Cyc_Absyn_array_typ(_tmp1D1,_tmp1D0,_tmp1CF,_tmp1CE,_tmpB9);});_tmp12E->type=_tmp1D2;});_tmpD9=_tmp1D3;});
goto _LL68;default: goto _LL75;}case 27U: _LL6D: _tmpB5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LL6E:
 _tmpB6=_tmpB5;goto _LL70;case 28U: _LL6F: _tmpB6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LL70:
# 470
({void*_tmp1D5=({void*_tmp1D4=Cyc_Absyn_array_typ(_tmpBC,_tmpBB,_tmpB6,_tmpBA,_tmpB9);_tmp12E->type=_tmp1D4;});_tmpD9=_tmp1D5;});
goto _LL68;case 36U: _LL71: _tmpB7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB2)->f2;_LL72:
 _tmpB8=_tmpB7;goto _LL74;case 26U: _LL73: _tmpB8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB2)->f1;_LL74:
# 474
({void*_tmp1DB=({void*_tmp1DA=({void*_tmp1D9=_tmpBC;struct Cyc_Absyn_Tqual _tmp1D8=_tmpBB;struct Cyc_Absyn_Exp*_tmp1D7=Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB8),0U);union Cyc_Absyn_Constraint*_tmp1D6=_tmpBA;Cyc_Absyn_array_typ(_tmp1D9,_tmp1D8,_tmp1D7,_tmp1D6,_tmpB9);});_tmp12E->type=_tmp1DA;});_tmpD9=_tmp1DB;});
goto _LL68;default: _LL75: _LL76:
 goto _LL68;}_LL68:;}
# 478
goto _LL61;}else{goto _LL66;}}else{goto _LL66;}default: _LL66: _LL67:
 goto _LL61;}_LL61:;}{
# 485
struct Cyc_List_List*_tmpBD=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmpA7);
int _tmpBE=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmpA7,_tmpBD,& Cyc_Tcutil_tmk,_tmpBE,1,_tmpD9);
({int _tmp1DC=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmp12E->tq).print_const,_tmpD9);(_tmp12E->tq).real_const=_tmp1DC;});{
# 490
struct Cyc_Tcenv_Tenv*_tmpBF=_tmpA7;
if(_tmpDD == Cyc_Absyn_Extern  || _tmpDD == Cyc_Absyn_ExternC)
({void*_tmpC0=0U;({unsigned int _tmp1DE=_tmp15D->loc;struct _dyneither_ptr _tmp1DD=({const char*_tmpC1="extern declarations are not yet supported within functions";_tag_dyneither(_tmpC1,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1DE,_tmp1DD,_tag_dyneither(_tmpC0,sizeof(void*),0U));});});
if(_tmpD8 != 0){
Cyc_Tcexp_tcExpInitializer(_tmpBF,& _tmpD9,_tmpD8);
# 496
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpD8))
({void*_tmpC2=0U;({unsigned int _tmp1E0=_tmp15D->loc;struct _dyneither_ptr _tmp1DF=({const char*_tmpC3="initializer for static variable needs to be a constant expression";_tag_dyneither(_tmpC3,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1E0,_tmp1DF,_tag_dyneither(_tmpC2,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(_tmpBF,_tmpD8,_tmpD9)){
struct _dyneither_ptr _tmpC4=*_tmpDB;
struct _dyneither_ptr _tmpC5=Cyc_Absynpp_typ2string(_tmpD9);
struct _dyneither_ptr _tmpC6=Cyc_Absynpp_typ2string((void*)_check_null(_tmpD8->topt));
if(((_get_dyneither_size(_tmpC4,sizeof(char))+ _get_dyneither_size(_tmpC5,sizeof(char)))+ _get_dyneither_size(_tmpC6,sizeof(char)))+ 50 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpCB;_tmpCB.tag=0U;_tmpCB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC6);({struct Cyc_String_pa_PrintArg_struct _tmpCA;_tmpCA.tag=0U;_tmpCA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);({struct Cyc_String_pa_PrintArg_struct _tmpC9;_tmpC9.tag=0U;_tmpC9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC4);({void*_tmpC7[3U]={& _tmpC9,& _tmpCA,& _tmpCB};({unsigned int _tmp1E2=_tmp15D->loc;struct _dyneither_ptr _tmp1E1=({const char*_tmpC8="%s was declared with type %s but initialized with type %s.";_tag_dyneither(_tmpC8,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1E2,_tmp1E1,_tag_dyneither(_tmpC7,sizeof(void*),3U));});});});});});else{
# 505
if((_get_dyneither_size(_tmpC4,sizeof(char))+ _get_dyneither_size(_tmpC5,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmpC6,sizeof(char))+ 25 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpD0;_tmpD0.tag=0U;_tmpD0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC6);({struct Cyc_String_pa_PrintArg_struct _tmpCF;_tmpCF.tag=0U;_tmpCF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);({struct Cyc_String_pa_PrintArg_struct _tmpCE;_tmpCE.tag=0U;_tmpCE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC4);({void*_tmpCC[3U]={& _tmpCE,& _tmpCF,& _tmpD0};({unsigned int _tmp1E4=_tmp15D->loc;struct _dyneither_ptr _tmp1E3=({const char*_tmpCD="%s was declared with type %s\n but initialized with type %s.";_tag_dyneither(_tmpCD,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp1E4,_tmp1E3,_tag_dyneither(_tmpCC,sizeof(void*),3U));});});});});});else{
# 510
({struct Cyc_String_pa_PrintArg_struct _tmpD5;_tmpD5.tag=0U;_tmpD5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC6);({struct Cyc_String_pa_PrintArg_struct _tmpD4;_tmpD4.tag=0U;_tmpD4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC5);({struct Cyc_String_pa_PrintArg_struct _tmpD3;_tmpD3.tag=0U;_tmpD3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpC4);({void*_tmpD1[3U]={& _tmpD3,& _tmpD4,& _tmpD5};({unsigned int _tmp1E6=_tmp15D->loc;struct _dyneither_ptr _tmp1E5=({const char*_tmpD2="%s declared with type \n%s\n but initialized with type \n%s.";_tag_dyneither(_tmpD2,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp1E6,_tmp1E5,_tag_dyneither(_tmpD1,sizeof(void*),3U));});});});});});}}
# 512
Cyc_Tcutil_unify(_tmpD9,(void*)_check_null(_tmpD8->topt));
Cyc_Tcutil_explain_failure();}}
# 516
Cyc_Tcstmt_tcStmt(_tmpBF,_tmp15C,0);
return;};};};}case 2U: _LL35: _tmp132=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA9)->f1;_tmp131=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA9)->f2;_tmp130=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA9)->f3;_tmp12F=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA9)->f4;_LL36: {
# 524
struct Cyc_Absyn_Exp*_tmpDE=*_tmp130;
{void*_tmpDF=_tmp132->r;void*_tmpE0=_tmpDF;switch(*((int*)_tmpE0)){case 1U: _LL78: _LL79:
 goto _LL7B;case 2U: _LL7A: _LL7B:
 goto _LL7D;case 15U: _LL7C: _LL7D:
 goto _LL77;default: _LL7E: _LL7F:
# 530
 if(!Cyc_Tcutil_is_var_exp(_tmpDE)){
struct Cyc_Absyn_Vardecl*_tmpE1=({struct _tuple2*_tmp1E7=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1E7,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmpE2=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmpE3=({struct Cyc_Absyn_Pat*_tmp1EC=({void*_tmp1EA=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1E9=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpE8;_tmpE8.tag=1U;_tmpE8.f1=_tmpE1;({struct Cyc_Absyn_Pat*_tmp1E8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpDE->loc);_tmpE8.f2=_tmp1E8;});_tmpE8;});_tmpE7[0]=_tmp1E9;});_tmpE7;});Cyc_Absyn_new_pat(_tmp1EA,_tmpDE->loc);});struct Cyc_Absyn_Exp*_tmp1EB=_tmpDE;Cyc_Absyn_let_decl(_tmp1EC,_tmp1EB,s0->loc);});
# 536
struct Cyc_Absyn_Stmt*_tmpE4=Cyc_Absyn_decl_stmt(_tmpE3,_tmpE2,s0->loc);
# 538
s0->r=_tmpE4->r;
({struct Cyc_Absyn_Exp*_tmp1EF=({void*_tmp1EE=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp1ED=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpE6;_tmpE6.tag=4U;_tmpE6.f1=_tmpE1;_tmpE6;});_tmpE5[0]=_tmp1ED;});_tmpE5;});Cyc_Absyn_varb_exp(_tmp1EE,_tmpDE->loc);});*_tmp130=_tmp1EF;});
Cyc_Tcstmt_tcStmt(_tmpA7,s0,new_block);
return;}}_LL77:;}
# 544
Cyc_Tcexp_tcExpInitializer(_tmpA7,0,_tmpDE);{
# 546
void*pat_type=(void*)_check_null(_tmpDE->topt);
# 548
struct Cyc_Tcpat_TcPatResult _tmpE9=Cyc_Tcpat_tcPat(_tmpA7,_tmp132,& pat_type,_tmpDE);
({struct Cyc_Core_Opt*_tmp1F0=({struct Cyc_Core_Opt*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->v=_tmpE9.patvars;_tmpEA;});*_tmp131=_tmp1F0;});
if(!({void*_tmp1F1=(void*)_check_null(_tmp132->topt);Cyc_Tcutil_unify(_tmp1F1,(void*)_check_null(_tmpDE->topt));}) && !
Cyc_Tcutil_coerce_assign(_tmpA7,_tmpDE,(void*)_check_null(_tmp132->topt))){
({struct Cyc_String_pa_PrintArg_struct _tmpEE;_tmpEE.tag=0U;({struct _dyneither_ptr _tmp1F2=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpDE->topt)));_tmpEE.f1=_tmp1F2;});({struct Cyc_String_pa_PrintArg_struct _tmpED;_tmpED.tag=0U;({struct _dyneither_ptr _tmp1F3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp132->topt)));_tmpED.f1=_tmp1F3;});({void*_tmpEB[2U]={& _tmpED,& _tmpEE};({unsigned int _tmp1F5=_tmp15D->loc;struct _dyneither_ptr _tmp1F4=({const char*_tmpEC="pattern type %s does not match definition type %s";_tag_dyneither(_tmpEC,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp1F5,_tmp1F4,_tag_dyneither(_tmpEB,sizeof(void*),2U));});});});});
({void*_tmp1F6=(void*)_check_null(_tmp132->topt);Cyc_Tcutil_unify(_tmp1F6,(void*)_check_null(_tmpDE->topt));});
Cyc_Tcutil_explain_failure();}else{
# 558
Cyc_Tcpat_check_pat_regions(_tmpA7,_tmp132,_tmpE9.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmp132->loc,_tmpA7,_tmp132,_tmp12F);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmpA7,_tmpE9,_tmp15C,0,0);
return;};}case 3U: _LL37: _tmp133=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA9)->f1;_LL38: {
# 564
void*_tmpEF=Cyc_Tcenv_curr_rgn(_tmpA7);
struct Cyc_Tcenv_Tenv*_tmpF0=_tmpA7;
for(0;_tmp133 != 0;_tmp133=_tmp133->tl){
struct Cyc_Absyn_Vardecl*_tmpF1=(struct Cyc_Absyn_Vardecl*)_tmp133->hd;
struct Cyc_Absyn_Vardecl*_tmpF2=_tmpF1;union Cyc_Absyn_Nmspace _tmpF5;void*_tmpF4;void**_tmpF3;_LL81: _tmpF5=(_tmpF2->name)->f1;_tmpF4=_tmpF2->type;_tmpF3=(void**)& _tmpF2->rgn;_LL82:;
*_tmpF3=_tmpEF;
({unsigned int _tmp1F9=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1F8=_tmpF0;struct Cyc_List_List*_tmp1F7=Cyc_Tcenv_lookup_type_vars(_tmpF0);Cyc_Tcutil_check_type(_tmp1F9,_tmp1F8,_tmp1F7,& Cyc_Tcutil_tmk,1,1,_tmpF4);});}
# 572
Cyc_Tcstmt_tcStmt(_tmpF0,_tmp15C,0);
return;}case 4U: _LL39: _tmp136=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA9)->f1;_tmp135=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA9)->f2;_tmp134=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA9)->f3;_LL3A:
# 578
({void*_tmp1FE=({void*_tmp1FD=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1FC=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpF7;_tmpF7.tag=2U;({struct Cyc_Absyn_Stmt*_tmp1FA=Cyc_Absyn_new_stmt(_tmp15C->r,_tmp15C->loc);_tmpF7.f1=_tmp1FA;});({struct Cyc_Absyn_Stmt*_tmp1FB=Cyc_Absyn_skip_stmt(_tmp15C->loc);_tmpF7.f2=_tmp1FB;});_tmpF7;});_tmpF6[0]=_tmp1FC;});_tmpF6;});Cyc_Absyn_new_stmt(_tmp1FD,_tmp15C->loc);})->r;_tmp15C->r=_tmp1FE;});{
# 580
struct Cyc_Tcenv_Tenv*_tmpF8=_tmpA7;
struct Cyc_Absyn_Vardecl*_tmpF9=_tmp135;void**_tmp11A;void**_tmp119;_LL84: _tmp11A=(void**)& _tmpF9->type;_tmp119=(void**)& _tmpF9->rgn;_LL85:;{
void*_tmpFA=Cyc_Tcenv_curr_rgn(_tmpA7);
*_tmp119=_tmpFA;{
void*rgn_typ;
if((unsigned int)_tmp134){
# 588
struct _tuple2*drname=({struct _tuple2*_tmp10D=_cycalloc(sizeof(*_tmp10D));({union Cyc_Absyn_Nmspace _tmp201=({union Cyc_Absyn_Nmspace _tmp10F;({struct Cyc_List_List*_tmp200=({struct _dyneither_ptr*_tmp110[1U];({struct _dyneither_ptr*_tmp1FF=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"Core",sizeof(char),5U);_tmp110[0U]=_tmp1FF;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp110,sizeof(struct _dyneither_ptr*),1U));});(_tmp10F.Abs_n).val=_tmp200;});(_tmp10F.Abs_n).tag=2;_tmp10F;});_tmp10D->f1=_tmp201;});({struct _dyneither_ptr*_tmp202=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"DynamicRegion",sizeof(char),14U);_tmp10D->f2=_tmp202;});_tmp10D;});
void*_tmpFB=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->v=& Cyc_Tcutil_trk;_tmp10C;}),0);
({void*_tmp203=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFC=_cycalloc(sizeof(*_tmpFC));_tmpFC->v=& Cyc_Tcutil_rk;_tmpFC;}),0);rgn_typ=_tmp203;});{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFD=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp108=_cycalloc(sizeof(*_tmp108));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp207=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp109;_tmp109.tag=11U;({struct Cyc_Absyn_AggrInfo _tmp206=({struct Cyc_Absyn_AggrInfo _tmp10A;({union Cyc_Absyn_AggrInfoU _tmp204=
Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);_tmp10A.aggr_info=_tmp204;});({struct Cyc_List_List*_tmp205=({void*_tmp10B[1U];_tmp10B[0U]=rgn_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp10B,sizeof(void*),1U));});_tmp10A.targs=_tmp205;});_tmp10A;});_tmp109.f1=_tmp206;});_tmp109;});_tmp108[0]=_tmp207;});_tmp108;});
# 595
void*_tmpFE=({void*_tmp20A=(void*)_tmpFD;void*_tmp209=_tmpFB;struct Cyc_Absyn_Tqual _tmp208=
Cyc_Absyn_empty_tqual(0U);
# 595
Cyc_Absyn_at_typ(_tmp20A,_tmp209,_tmp208,Cyc_Absyn_false_conref);});
# 597
({unsigned int _tmp20D=s0->loc;struct Cyc_Tcenv_Tenv*_tmp20C=_tmpA7;struct Cyc_List_List*_tmp20B=Cyc_Tcenv_lookup_type_vars(_tmpA7);Cyc_Tcutil_check_type(_tmp20D,_tmp20C,_tmp20B,& Cyc_Tcutil_tmk,1,0,_tmpFE);});{
void*_tmpFF=Cyc_Tcexp_tcExp(_tmpA7,& _tmpFE,_tmp134);
if(!Cyc_Tcutil_unify(_tmpFE,_tmpFF) && !
Cyc_Tcutil_coerce_assign(_tmpA7,_tmp134,_tmpFE))
({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0U;({struct _dyneither_ptr _tmp20E=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpFF));_tmp103.f1=_tmp20E;});({struct Cyc_String_pa_PrintArg_struct _tmp102;_tmp102.tag=0U;({struct _dyneither_ptr _tmp20F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpFE));_tmp102.f1=_tmp20F;});({void*_tmp100[2U]={& _tmp102,& _tmp103};({unsigned int _tmp211=s0->loc;struct _dyneither_ptr _tmp210=({const char*_tmp101="expected %s but found %s";_tag_dyneither(_tmp101,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp211,_tmp210,_tag_dyneither(_tmp100,sizeof(void*),2U));});});});});
if(!Cyc_Tcutil_unify(_tmpFB,(void*)& Cyc_Absyn_UniqueRgn_val) && !
Cyc_Tcutil_unify(_tmpFB,(void*)& Cyc_Absyn_RefCntRgn_val))
({void*_tmp104=0U;({unsigned int _tmp213=s0->loc;struct _dyneither_ptr _tmp212=({const char*_tmp105="open is only allowed on unique or reference-counted keys";_tag_dyneither(_tmp105,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp213,_tmp212,_tag_dyneither(_tmp104,sizeof(void*),0U));});});
({void*_tmp215=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp106=_cycalloc(sizeof(*_tmp106));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp214=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp107;_tmp107.tag=15U;_tmp107.f1=rgn_typ;_tmp107;});_tmp106[0]=_tmp214;});_tmp106;});*_tmp11A=_tmp215;});};};}else{
# 608
({void*_tmp217=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp112=_cycalloc(sizeof(*_tmp112));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp216=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp113;_tmp113.tag=2U;_tmp113.f1=_tmp136;_tmp113;});_tmp112[0]=_tmp216;});_tmp112;});rgn_typ=_tmp217;});
({struct Cyc_Tcenv_Tenv*_tmp21A=({unsigned int _tmp219=s0->loc;struct Cyc_Tcenv_Tenv*_tmp218=_tmpF8;Cyc_Tcenv_add_type_vars(_tmp219,_tmp218,({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->hd=_tmp136;_tmp114->tl=0;_tmp114;}));});_tmpF8=_tmp21A;});}
# 611
({struct Cyc_Tcenv_Tenv*_tmp21B=Cyc_Tcenv_add_region(_tmpF8,rgn_typ,1);_tmpF8=_tmp21B;});
({unsigned int _tmp21E=s0->loc;struct Cyc_Tcenv_Tenv*_tmp21D=_tmpA7;struct Cyc_List_List*_tmp21C=Cyc_Tcenv_lookup_type_vars(_tmpF8);Cyc_Tcutil_check_type(_tmp21E,_tmp21D,_tmp21C,& Cyc_Tcutil_bk,1,0,*_tmp11A);});
if(!({void*_tmp220=*_tmp11A;Cyc_Tcutil_unify(_tmp220,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp115=_cycalloc(sizeof(*_tmp115));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp21F=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp116;_tmp116.tag=15U;_tmp116.f1=rgn_typ;_tmp116;});_tmp115[0]=_tmp21F;});_tmp115;}));}))
({void*_tmp117=0U;({struct _dyneither_ptr _tmp221=({const char*_tmp118="region stmt: type of region handle is wrong!";_tag_dyneither(_tmp118,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp221,_tag_dyneither(_tmp117,sizeof(void*),0U));});});
# 616
Cyc_Tcstmt_tcStmt(_tmpF8,_tmp15C,0);
return;};};};case 1U: _LL3B: _tmp137=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA9)->f1;_LL3C: {
# 621
void*_tmp11B=Cyc_Tcenv_curr_rgn(_tmpA7);
if(_tmp137->sc != Cyc_Absyn_Public)
({void*_tmp11C=0U;({unsigned int _tmp223=_tmp15D->loc;struct _dyneither_ptr _tmp222=({const char*_tmp11D="bad storage class for inner function";_tag_dyneither(_tmp11D,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp223,_tmp222,_tag_dyneither(_tmp11C,sizeof(void*),0U));});});
Cyc_Tcutil_check_fndecl_valid_type(_tmp15D->loc,_tmpA7,_tmp137);{
void*t=Cyc_Tcutil_fndecl2typ(_tmp137);
({struct Cyc_List_List*_tmp224=Cyc_Tcutil_transfer_fn_type_atts(t,_tmp137->attributes);_tmp137->attributes=_tmp224;});
# 628
{struct Cyc_List_List*atts=_tmp137->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp11E=(void*)atts->hd;void*_tmp11F=_tmp11E;switch(*((int*)_tmp11F)){case 7U: _LL87: _LL88:
 goto _LL8A;case 6U: _LL89: _LL8A:
({struct Cyc_String_pa_PrintArg_struct _tmp122;_tmp122.tag=0U;({struct _dyneither_ptr _tmp225=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));_tmp122.f1=_tmp225;});({void*_tmp120[1U]={& _tmp122};({unsigned int _tmp227=_tmp15D->loc;struct _dyneither_ptr _tmp226=({const char*_tmp121="bad attribute %s for function";_tag_dyneither(_tmp121,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp227,_tmp226,_tag_dyneither(_tmp120,sizeof(void*),1U));});});});
goto _LL86;default: _LL8B: _LL8C:
 goto _LL86;}_LL86:;}}{
# 636
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->sc=_tmp137->sc;_tmp123->name=_tmp137->name;_tmp123->varloc=0U;({struct Cyc_Absyn_Tqual _tmp228=Cyc_Absyn_const_tqual(0U);_tmp123->tq=_tmp228;});({void*_tmp22C=({
void*_tmp22B=t;void*_tmp22A=_tmp11B;struct Cyc_Absyn_Tqual _tmp229=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_typ(_tmp22B,_tmp22A,_tmp229,Cyc_Absyn_false_conref);});_tmp123->type=_tmp22C;});_tmp123->initializer=0;_tmp123->rgn=_tmp11B;_tmp123->attributes=0;_tmp123->escapes=0;_tmp123;});
# 640
_tmp137->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmpA7->le);
({struct Cyc_Tcenv_Fenv*_tmp22D=Cyc_Tcenv_nested_fenv(_tmp15D->loc,old_fenv,_tmp137);_tmpA7->le=_tmp22D;});
Cyc_Tcstmt_tcStmt(_tmpA7,_tmp137->body,0);
Cyc_Tcenv_check_delayed_effects(_tmpA7);
Cyc_Tcenv_check_delayed_constraints(_tmpA7);
_tmpA7->le=old_fenv;
Cyc_Tcstmt_tcStmt(_tmpA7,_tmp15C,0);
return;};};};}case 9U: _LL3D: _tmp139=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA9)->f1;_tmp138=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA9)->f2;_LL3E:
({struct _dyneither_ptr _tmp22E=({const char*_tmp124="namespace";_tag_dyneither(_tmp124,sizeof(char),10U);});unimp_msg_part=_tmp22E;});goto _LL32;case 10U: _LL3F: _tmp13B=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA9)->f1;_tmp13A=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA9)->f2;_LL40:
({struct _dyneither_ptr _tmp22F=({const char*_tmp125="using";_tag_dyneither(_tmp125,sizeof(char),6U);});unimp_msg_part=_tmp22F;});goto _LL32;case 5U: _LL41: _LL42:
({struct _dyneither_ptr _tmp230=({const char*_tmp126="type";_tag_dyneither(_tmp126,sizeof(char),5U);});unimp_msg_part=_tmp230;});goto _LL32;case 6U: _LL43: _LL44:
({struct _dyneither_ptr _tmp231=({const char*_tmp127="datatype";_tag_dyneither(_tmp127,sizeof(char),9U);});unimp_msg_part=_tmp231;});goto _LL32;case 7U: _LL45: _LL46:
({struct _dyneither_ptr _tmp232=({const char*_tmp128="enum";_tag_dyneither(_tmp128,sizeof(char),5U);});unimp_msg_part=_tmp232;});goto _LL32;case 8U: _LL47: _LL48:
({struct _dyneither_ptr _tmp233=({const char*_tmp129="typedef";_tag_dyneither(_tmp129,sizeof(char),8U);});unimp_msg_part=_tmp233;});goto _LL32;case 11U: _LL49: _LL4A:
({struct _dyneither_ptr _tmp234=({const char*_tmp12A="extern \"C\"";_tag_dyneither(_tmp12A,sizeof(char),11U);});unimp_msg_part=_tmp234;});goto _LL32;case 12U: _LL4B: _LL4C:
# 657
({struct _dyneither_ptr _tmp235=({const char*_tmp12B="extern \"C include\"";_tag_dyneither(_tmp12B,sizeof(char),19U);});unimp_msg_part=_tmp235;});goto _LL32;case 13U: _LL4D: _LL4E:
({struct _dyneither_ptr _tmp236=({const char*_tmp12C="__cyclone_port_on__";_tag_dyneither(_tmp12C,sizeof(char),20U);});unimp_msg_part=_tmp236;});goto _LL32;default: _LL4F: _LL50:
({struct _dyneither_ptr _tmp237=({const char*_tmp12D="__cyclone_port_off__";_tag_dyneither(_tmp12D,sizeof(char),21U);});unimp_msg_part=_tmp237;});goto _LL32;}_LL32:;}
# 661
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp13E;_tmp13E.tag=0U;_tmp13E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part);({void*_tmp13C[1U]={& _tmp13E};({struct _dyneither_ptr _tmp238=({const char*_tmp13D="tcStmt: nested %s declarations unimplemented";_tag_dyneither(_tmp13D,sizeof(char),45U);});((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp238,_tag_dyneither(_tmp13C,sizeof(void*),1U));});});}));}}_LL0:;}
