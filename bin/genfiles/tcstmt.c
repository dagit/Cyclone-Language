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
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 176
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 180
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 215 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 223
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 274
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 279
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 296
int Cyc_Tcutil_new_tvar_id();
# 313
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 316
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 334
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 338
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 355
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
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp5;struct Cyc_Absyn_Exp*_tmp4;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 5U)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp4=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
({void*_tmp16A=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=5U,_tmp2->f1=_tmp4,_tmp2->f2=Cyc_Absyn_PreInc;_tmp2;});e->r=_tmp16A;});goto _LL0;case Cyc_Absyn_PostDec: _LL3: _tmp5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4:
({void*_tmp16B=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=5U,_tmp3->f1=_tmp5,_tmp3->f2=Cyc_Absyn_PreDec;_tmp3;});e->r=_tmp16B;});goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 61
static int Cyc_Tcstmt_noassign_exp(struct Cyc_Absyn_Exp*e){
void*_tmp6=e->r;void*_tmp7=_tmp6;struct Cyc_List_List*_tmp2E;struct Cyc_List_List*_tmp2D;struct Cyc_List_List*_tmp2C;struct Cyc_List_List*_tmp2B;struct Cyc_List_List*_tmp2A;struct Cyc_List_List*_tmp29;struct Cyc_List_List*_tmp28;struct Cyc_List_List*_tmp27;struct Cyc_Absyn_Exp*_tmp26;struct Cyc_Absyn_Exp*_tmp25;struct Cyc_Absyn_Exp*_tmp24;struct Cyc_Absyn_Exp*_tmp23;struct Cyc_Absyn_Exp*_tmp22;struct Cyc_Absyn_Exp*_tmp21;struct Cyc_Absyn_Exp*_tmp20;struct Cyc_Absyn_Exp*_tmp1F;struct Cyc_Absyn_Exp*_tmp1E;struct Cyc_Absyn_Exp*_tmp1D;struct Cyc_Absyn_Exp*_tmp1C;struct Cyc_Absyn_Exp*_tmp1B;struct Cyc_Absyn_Exp*_tmp1A;struct Cyc_Absyn_Exp*_tmp19;struct Cyc_Absyn_Exp*_tmp18;struct Cyc_Absyn_Exp*_tmp17;struct Cyc_Absyn_Exp*_tmp16;struct Cyc_Absyn_Exp*_tmp15;struct Cyc_Absyn_Exp*_tmp14;struct Cyc_Absyn_Exp*_tmp13;struct Cyc_Absyn_Exp*_tmp12;struct Cyc_Absyn_Exp*_tmp11;struct Cyc_Absyn_Exp*_tmp10;struct Cyc_Absyn_Exp*_tmpF;struct Cyc_Absyn_Exp*_tmpE;struct Cyc_Absyn_Exp*_tmpD;struct Cyc_Absyn_Exp*_tmpC;struct Cyc_Absyn_Exp*_tmpB;switch(*((int*)_tmp7)){case 10U: _LL1: _LL2:
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
 return 1;case 6U: _LL1D: _tmpD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmpC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_tmpB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL1E:
# 80
 if(!Cyc_Tcstmt_noassign_exp(_tmpD))return 0;
_tmpF=_tmpC;_tmpE=_tmpB;goto _LL20;case 27U: _LL1F: _tmpF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_tmpE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL20:
 _tmp11=_tmpF;_tmp10=_tmpE;goto _LL22;case 7U: _LL21: _tmp11=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp10=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL22:
 _tmp13=_tmp11;_tmp12=_tmp10;goto _LL24;case 8U: _LL23: _tmp13=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp12=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL24:
 _tmp15=_tmp13;_tmp14=_tmp12;goto _LL26;case 23U: _LL25: _tmp15=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp14=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL26:
 _tmp17=_tmp15;_tmp16=_tmp14;goto _LL28;case 9U: _LL27: _tmp17=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp16=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL28:
 return Cyc_Tcstmt_noassign_exp(_tmp17) && Cyc_Tcstmt_noassign_exp(_tmp16);case 38U: _LL29: _tmp18=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2A:
# 88
 _tmp19=_tmp18;goto _LL2C;case 11U: _LL2B: _tmp19=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2C:
 _tmp1A=_tmp19;goto _LL2E;case 12U: _LL2D: _tmp1A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL2E:
 _tmp1B=_tmp1A;goto _LL30;case 13U: _LL2F: _tmp1B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL30:
 _tmp1C=_tmp1B;goto _LL32;case 14U: _LL31: _tmp1C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL32:
 _tmp1D=_tmp1C;goto _LL34;case 18U: _LL33: _tmp1D=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL34:
 _tmp1E=_tmp1D;goto _LL36;case 20U: _LL35: _tmp1E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL36:
 _tmp1F=_tmp1E;goto _LL38;case 21U: _LL37: _tmp1F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL38:
 _tmp20=_tmp1F;goto _LL3A;case 22U: _LL39: _tmp20=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3A:
 _tmp21=_tmp20;goto _LL3C;case 28U: _LL3B: _tmp21=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3C:
 _tmp22=_tmp21;goto _LL3E;case 15U: _LL3D: _tmp22=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL3E:
 return Cyc_Tcstmt_noassign_exp(_tmp22);case 34U: _LL3F: _tmp24=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp7)->f1).rgn;_tmp23=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp7)->f1).num_elts;_LL40:
# 101
 _tmp26=_tmp24;_tmp25=_tmp23;goto _LL42;case 16U: _LL41: _tmp26=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_tmp25=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL42:
# 103
 if(!Cyc_Tcstmt_noassign_exp(_tmp25))return 0;
if(_tmp26 != 0)return Cyc_Tcstmt_noassign_exp(_tmp26);else{
return 1;}case 3U: _LL43: _tmp27=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL44:
# 107
 _tmp28=_tmp27;goto _LL46;case 31U: _LL45: _tmp28=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL46:
 _tmp29=_tmp28;goto _LL48;case 24U: _LL47: _tmp29=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL48:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Tcstmt_noassign_exp,_tmp29);case 36U: _LL49: _tmp2A=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL4A:
# 111
 _tmp2B=_tmp2A;goto _LL4C;case 29U: _LL4B: _tmp2B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp7)->f3;_LL4C:
 _tmp2C=_tmp2B;goto _LL4E;case 30U: _LL4D: _tmp2C=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL4E:
 _tmp2D=_tmp2C;goto _LL50;case 26U: _LL4F: _tmp2D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp7)->f1;_LL50:
 _tmp2E=_tmp2D;goto _LL52;default: _LL51: _tmp2E=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp7)->f2;_LL52:
# 116
 for(0;_tmp2E != 0;_tmp2E=_tmp2E->tl){
struct _tuple15*_tmp8=(struct _tuple15*)_tmp2E->hd;struct _tuple15*_tmp9=_tmp8;struct Cyc_Absyn_Exp*_tmpA;_LL54: _tmpA=_tmp9->f2;_LL55:;
if(!Cyc_Tcstmt_noassign_exp(_tmpA))return 0;}
# 120
return 1;}_LL0:;}struct _tuple16{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple17{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 125
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 127
struct Cyc_Tcpat_TcPatResult _tmp2F=pat_res;struct _tuple1*_tmp3B;struct Cyc_List_List*_tmp3A;_LL1: _tmp3B=_tmp2F.tvars_and_bounds_opt;_tmp3A=_tmp2F.patvars;_LL2:;{
struct Cyc_List_List*_tmp30=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple17*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple17*))Cyc_Core_fst,_tmp3A);
struct Cyc_List_List*_tmp31=_tmp3B == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*_tmp3B).f1);
struct Cyc_List_List*_tmp32=_tmp3B == 0?0:(*_tmp3B).f2;
struct Cyc_List_List*_tmp33=_tmp3B == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple16*))Cyc_Core_snd,(*_tmp3B).f1));
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmp31);
for(0;_tmp33 != 0;_tmp33=_tmp33->tl){
({struct Cyc_Tcenv_Tenv*_tmp16D=({struct Cyc_Tcenv_Tenv*_tmp16C=te2;Cyc_Tcenv_add_region(_tmp16C,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=2U,_tmp34->f1=(struct Cyc_Absyn_Tvar*)_tmp33->hd;_tmp34;}),0);});te2=_tmp16D;});}
({struct Cyc_Tcenv_Tenv*_tmp16E=Cyc_Tcenv_new_outlives_constraints(te2,_tmp32,loc);te2=_tmp16E;});
if(new_block)
({struct Cyc_Tcenv_Tenv*_tmp16F=Cyc_Tcenv_new_block(loc,te2);te2=_tmp16F;});{
void*_tmp35=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp36=_tmp30;for(0;_tmp36 != 0;_tmp36=_tmp36->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp36->hd != 0)
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp36->hd)))->rgn=_tmp35;}}
# 143
if(where_opt != 0){
({struct Cyc_Tcenv_Tenv*_tmp171=te2;struct Cyc_Absyn_Exp*_tmp170=where_opt;Cyc_Tcexp_tcTest(_tmp171,_tmp170,({const char*_tmp37="switch clause guard";_tag_dyneither(_tmp37,sizeof(char),20U);}));});
if(!Cyc_Tcstmt_noassign_exp(where_opt))
({void*_tmp38=0U;({unsigned int _tmp173=where_opt->loc;struct _dyneither_ptr _tmp172=({const char*_tmp39="cannot show &&-clause in pattern has no effects";_tag_dyneither(_tmp39,sizeof(char),48U);});Cyc_Tcutil_terr(_tmp173,_tmp172,_tag_dyneither(_tmp38,sizeof(void*),0U));});});}
# 148
Cyc_Tcstmt_tcStmt(te2,s,0);};};}
# 153
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 155
struct _tuple13*tk=0;
struct Cyc_Tcenv_Tenv*_tmp3C=Cyc_Tcenv_add_region_equality(te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp3C,s0,new_block);
if(tk != 0)
({void*_tmp174=(*((struct _tuple13*)_check_null(tk))).f2;((*((struct _tuple13*)_check_null(tk))).f1)->kind=_tmp174;});}
# 162
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp3D=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _tuple2*res=({struct _tuple2*_tmp42=_cycalloc(sizeof(*_tmp42));_tmp42->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp177=({struct _dyneither_ptr*_tmp41=_cycalloc(sizeof(*_tmp41));({struct _dyneither_ptr _tmp176=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp40=({struct Cyc_Int_pa_PrintArg_struct _tmp14F;_tmp14F.tag=1U,_tmp14F.f1=(unsigned int)_tmp3D;_tmp14F;});void*_tmp3E[1U];_tmp3E[0]=& _tmp40;({struct _dyneither_ptr _tmp175=({const char*_tmp3F="_stmttmp%X";_tag_dyneither(_tmp3F,sizeof(char),11U);});Cyc_aprintf(_tmp175,_tag_dyneither(_tmp3E,sizeof(void*),1U));});});*_tmp41=_tmp176;});_tmp41;});_tmp42->f2=_tmp177;});_tmp42;});
return res;}
# 176 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 178
void*_tmp43=s0->r;void*_tmp44=_tmp43;struct Cyc_Absyn_Decl*_tmp14E;struct Cyc_Absyn_Stmt*_tmp14D;struct Cyc_Absyn_Stmt*_tmp14C;struct Cyc_List_List**_tmp14B;void**_tmp14A;struct Cyc_Absyn_Exp**_tmp149;struct Cyc_List_List*_tmp148;void**_tmp147;struct _dyneither_ptr*_tmp146;struct Cyc_Absyn_Stmt*_tmp145;struct Cyc_List_List*_tmp144;struct Cyc_Absyn_Switch_clause***_tmp143;struct Cyc_Absyn_Stmt*_tmp142;struct Cyc_Absyn_Exp*_tmp141;struct Cyc_Absyn_Stmt*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Stmt*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct Cyc_Absyn_Stmt*_tmp13B;struct Cyc_Absyn_Stmt*_tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct Cyc_Absyn_Stmt*_tmp138;struct Cyc_Absyn_Stmt*_tmp137;struct Cyc_Absyn_Exp*_tmp136;struct Cyc_Absyn_Stmt*_tmp135;struct Cyc_Absyn_Stmt*_tmp134;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_Stmt*_tmp132;struct Cyc_Absyn_Stmt*_tmp131;struct Cyc_Absyn_Exp*_tmp130;switch(*((int*)_tmp44)){case 0U: _LL1: _LL2:
# 180
 return;case 1U: _LL3: _tmp130=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_LL4:
# 183
 Cyc_Tcexp_tcExp(te,0,_tmp130);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(_tmp130);
return;case 2U: _LL5: _tmp132=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp131=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL6:
# 189
 Cyc_Tcstmt_tcStmt(te,_tmp132,1);
Cyc_Tcstmt_tcStmt(te,_tmp131,1);
return;case 3U: _LL7: _tmp133=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_LL8: {
# 194
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp133 == 0){
void*_tmp45=Cyc_Tcutil_compress(t);void*_tmp46=_tmp45;switch(*((int*)_tmp46)){case 0U: _LL22: _LL23:
 goto _LL21;case 7U: _LL24: _LL25:
 goto _LL27;case 6U: _LL26: _LL27:
# 200
({struct Cyc_String_pa_PrintArg_struct _tmp49=({struct Cyc_String_pa_PrintArg_struct _tmp150;_tmp150.tag=0U,({struct _dyneither_ptr _tmp178=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp150.f1=_tmp178;});_tmp150;});void*_tmp47[1U];_tmp47[0]=& _tmp49;({unsigned int _tmp17A=s0->loc;struct _dyneither_ptr _tmp179=({const char*_tmp48="should return a value of type %s";_tag_dyneither(_tmp48,sizeof(char),33U);});Cyc_Tcutil_warn(_tmp17A,_tmp179,_tag_dyneither(_tmp47,sizeof(void*),1U));});});goto _LL21;default: _LL28: _LL29:
# 202
({struct Cyc_String_pa_PrintArg_struct _tmp4C=({struct Cyc_String_pa_PrintArg_struct _tmp151;_tmp151.tag=0U,({struct _dyneither_ptr _tmp17B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp151.f1=_tmp17B;});_tmp151;});void*_tmp4A[1U];_tmp4A[0]=& _tmp4C;({unsigned int _tmp17D=s0->loc;struct _dyneither_ptr _tmp17C=({const char*_tmp4B="must return a value of type %s";_tag_dyneither(_tmp4B,sizeof(char),31U);});Cyc_Tcutil_terr(_tmp17D,_tmp17C,_tag_dyneither(_tmp4A,sizeof(void*),1U));});});goto _LL21;}_LL21:;}else{
# 206
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp133;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp153;_tmp153.tag=0U,({
struct _dyneither_ptr _tmp17E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp153.f1=_tmp17E;});_tmp153;});struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp152;_tmp152.tag=0U,({struct _dyneither_ptr _tmp17F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp152.f1=_tmp17F;});_tmp152;});void*_tmp4D[2U];_tmp4D[0]=& _tmp4F,_tmp4D[1]=& _tmp50;({unsigned int _tmp181=s0->loc;struct _dyneither_ptr _tmp180=({const char*_tmp4E="returns value of type %s but requires %s";_tag_dyneither(_tmp4E,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp181,_tmp180,_tag_dyneither(_tmp4D,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 214
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
({void*_tmp51=0U;({unsigned int _tmp183=e->loc;struct _dyneither_ptr _tmp182=({const char*_tmp52="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp52,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp183,_tmp182,_tag_dyneither(_tmp51,sizeof(void*),0U));});});}
# 217
return;}case 4U: _LL9: _tmp136=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp135=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp134=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LLA: {
# 220
struct Cyc_Tcexp_TestEnv _tmp53=({struct Cyc_Tcenv_Tenv*_tmp185=te;struct Cyc_Absyn_Exp*_tmp184=_tmp136;Cyc_Tcexp_tcTest(_tmp185,_tmp184,({const char*_tmp5A="if statement";_tag_dyneither(_tmp5A,sizeof(char),13U);}));});struct Cyc_Tcexp_TestEnv _tmp54=_tmp53;struct _tuple0*_tmp59;int _tmp58;_LL2B: _tmp59=_tmp54.eq;_tmp58=_tmp54.isTrue;_LL2C:;
{struct _tuple0*_tmp55=_tmp59;void*_tmp57;void*_tmp56;if(_tmp55 != 0){_LL2E: _tmp57=_tmp55->f1;_tmp56=_tmp55->f2;_LL2F:
# 223
 if(_tmp58){
Cyc_Tcstmt_tcStmtRefine(te,_tmp135,_tmp57,_tmp56,1);
Cyc_Tcstmt_tcStmt(te,_tmp134,1);}else{
# 228
Cyc_Tcstmt_tcStmt(te,_tmp135,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp134,_tmp57,_tmp56,1);}
# 231
goto _LL2D;}else{_LL30: _LL31:
# 233
 Cyc_Tcstmt_tcStmt(te,_tmp135,1);
Cyc_Tcstmt_tcStmt(te,_tmp134,1);
goto _LL2D;}_LL2D:;}
# 237
return;}case 5U: _LLB: _tmp139=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f1).f1;_tmp138=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f1).f2;_tmp137=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LLC:
# 240
({struct Cyc_Tcenv_Tenv*_tmp187=te;struct Cyc_Absyn_Exp*_tmp186=_tmp139;Cyc_Tcexp_tcTest(_tmp187,_tmp186,({const char*_tmp5B="while loop";_tag_dyneither(_tmp5B,sizeof(char),11U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp137,1);
return;case 9U: _LLD: _tmp13F=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp13E=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f1;_tmp13D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f2;_tmp13C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f3).f1;_tmp13B=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f3).f2;_tmp13A=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp44)->f4;_LLE:
# 245
 Cyc_Tcexp_tcExp(te,0,_tmp13F);
({struct Cyc_Tcenv_Tenv*_tmp189=te;struct Cyc_Absyn_Exp*_tmp188=_tmp13E;Cyc_Tcexp_tcTest(_tmp189,_tmp188,({const char*_tmp5C="for loop";_tag_dyneither(_tmp5C,sizeof(char),9U);}));});
Cyc_Tcstmt_tcStmt(te,_tmp13A,1);
Cyc_Tcexp_tcExp(te,0,_tmp13C);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp13C);
return;case 14U: _LLF: _tmp142=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp141=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f1;_tmp140=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp44)->f2).f2;_LL10:
# 253
 Cyc_Tcstmt_tcStmt(te,_tmp142,1);
({struct Cyc_Tcenv_Tenv*_tmp18B=te;struct Cyc_Absyn_Exp*_tmp18A=_tmp141;Cyc_Tcexp_tcTest(_tmp18B,_tmp18A,({const char*_tmp5D="do loop";_tag_dyneither(_tmp5D,sizeof(char),8U);}));});
return;case 6U: _LL11: _LL12:
# 257
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp144=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp143=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL18: {
# 262
const struct _tuple12*_tmp5E=Cyc_Tcenv_process_fallthru(te,s0,_tmp143);
if(_tmp5E == (const struct _tuple12*)0){
({void*_tmp5F=0U;({unsigned int _tmp18D=s0->loc;struct _dyneither_ptr _tmp18C=({const char*_tmp60="fallthru not in a non-last case";_tag_dyneither(_tmp60,sizeof(char),32U);});Cyc_Tcutil_terr(_tmp18D,_tmp18C,_tag_dyneither(_tmp5F,sizeof(void*),0U));});});
return;}{
# 267
struct Cyc_List_List*_tmp61=(*_tmp5E).f2;
struct Cyc_List_List*_tmp62=(*_tmp5E).f3;
struct Cyc_List_List*instantiation=({
struct Cyc_List_List*_tmp18E=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp18E,_tmp61);});
struct Cyc_List_List*_tmp63=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,_tmp62);
for(0;_tmp63 != 0  && _tmp144 != 0;(_tmp63=_tmp63->tl,_tmp144=_tmp144->tl)){
# 274
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp144->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp144->hd,(void*)_tmp63->hd,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp155;_tmp155.tag=0U,({
# 279
struct _dyneither_ptr _tmp18F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp144->hd)->topt)));_tmp155.f1=_tmp18F;});_tmp155;});struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp154;_tmp154.tag=0U,({struct _dyneither_ptr _tmp190=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_tmp63->hd));_tmp154.f1=_tmp190;});_tmp154;});void*_tmp64[2U];_tmp64[0]=& _tmp66,_tmp64[1]=& _tmp67;({unsigned int _tmp192=s0->loc;struct _dyneither_ptr _tmp191=({const char*_tmp65="fallthru argument has type %s but pattern variable has type %s";_tag_dyneither(_tmp65,sizeof(char),63U);});Cyc_Tcutil_terr(_tmp192,_tmp191,_tag_dyneither(_tmp64,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}
# 283
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_tmp63->hd) && !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp144->hd))
({void*_tmp68=0U;({unsigned int _tmp194=((struct Cyc_Absyn_Exp*)_tmp144->hd)->loc;struct _dyneither_ptr _tmp193=({const char*_tmp69="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp69,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp194,_tmp193,_tag_dyneither(_tmp68,sizeof(void*),0U));});});}
# 286
if(_tmp144 != 0)
({void*_tmp6A=0U;({unsigned int _tmp196=s0->loc;struct _dyneither_ptr _tmp195=({const char*_tmp6B="too many arguments in explicit fallthru";_tag_dyneither(_tmp6B,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp196,_tmp195,_tag_dyneither(_tmp6A,sizeof(void*),0U));});});
if(_tmp63 != 0)
({void*_tmp6C=0U;({unsigned int _tmp198=s0->loc;struct _dyneither_ptr _tmp197=({const char*_tmp6D="too few arguments in explicit fallthru";_tag_dyneither(_tmp6D,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp198,_tmp197,_tag_dyneither(_tmp6C,sizeof(void*),0U));});});
return;};}case 13U: _LL19: _tmp146=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp145=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL1A:
# 296
({struct Cyc_Tcenv_Tenv*_tmp1A0=({unsigned int _tmp19F=s0->loc;struct Cyc_Tcenv_Tenv*_tmp19E=te;Cyc_Tcenv_new_named_block(_tmp19F,_tmp19E,({struct Cyc_Absyn_Tvar*_tmp72=_cycalloc(sizeof(*_tmp72));
({struct _dyneither_ptr*_tmp19D=({struct _dyneither_ptr*_tmp71=_cycalloc(sizeof(*_tmp71));({struct _dyneither_ptr _tmp19C=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp156;_tmp156.tag=0U,_tmp156.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp146);_tmp156;});void*_tmp6E[1U];_tmp6E[0]=& _tmp70;({struct _dyneither_ptr _tmp19B=({const char*_tmp6F="`%s";_tag_dyneither(_tmp6F,sizeof(char),4U);});Cyc_aprintf(_tmp19B,_tag_dyneither(_tmp6E,sizeof(void*),1U));});});*_tmp71=_tmp19C;});_tmp71;});_tmp72->name=_tmp19D;}),({
int _tmp19A=Cyc_Tcutil_new_tvar_id();_tmp72->identity=_tmp19A;}),({
void*_tmp199=Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp72->kind=_tmp199;});_tmp72;}));});
# 296
Cyc_Tcstmt_tcStmt(_tmp1A0,_tmp145,0);});
# 300
return;case 10U: _LL1B: _tmp149=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp148=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp147=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LL1C: {
# 305
struct Cyc_Absyn_Exp*_tmp73=*_tmp149;
if(!Cyc_Tcutil_is_var_exp(_tmp73)){
struct Cyc_Absyn_Vardecl*_tmp74=({struct _tuple2*_tmp1A1=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1A1,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmp75=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmp76=({struct Cyc_Absyn_Pat*_tmp1A5=({void*_tmp1A3=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=1U,_tmp79->f1=_tmp74,({struct Cyc_Absyn_Pat*_tmp1A2=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp73->loc);_tmp79->f2=_tmp1A2;});_tmp79;});Cyc_Absyn_new_pat(_tmp1A3,_tmp73->loc);});struct Cyc_Absyn_Exp*_tmp1A4=_tmp73;Cyc_Absyn_let_decl(_tmp1A5,_tmp1A4,s0->loc);});
# 312
struct Cyc_Absyn_Stmt*_tmp77=Cyc_Absyn_decl_stmt(_tmp76,_tmp75,s0->loc);
s0->r=_tmp77->r;
({struct Cyc_Absyn_Exp*_tmp1A7=({void*_tmp1A6=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=4U,_tmp78->f1=_tmp74;_tmp78;});Cyc_Absyn_varb_exp(_tmp1A6,_tmp73->loc);});*_tmp149=_tmp1A7;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 319
void*_tmp7A=Cyc_Tcexp_tcExp(te,0,_tmp73);
# 321
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp7A) && !Cyc_Tcutil_is_noalias_path(_tmp73))
({void*_tmp7B=0U;({unsigned int _tmp1A9=_tmp73->loc;struct _dyneither_ptr _tmp1A8=({const char*_tmp7C="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp7C,sizeof(char),49U);});Cyc_Tcutil_terr(_tmp1A9,_tmp1A8,_tag_dyneither(_tmp7B,sizeof(void*),0U));});});{
# 326
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp7D=_tmp148;for(0;_tmp7D != 0;_tmp7D=_tmp7D->tl){
if(first_case){
first_case=0;
({struct Cyc_Tcpat_TcPatResult _tmp1AA=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->pattern,& _tmp7A,_tmp73);this_pattern=_tmp1AA;});}else{
# 334
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 336
if(_tmp7D->tl != 0){
({struct Cyc_Tcpat_TcPatResult*_tmp1AC=({struct Cyc_Tcpat_TcPatResult*_tmp7E=_cycalloc(sizeof(*_tmp7E));({struct Cyc_Tcpat_TcPatResult _tmp1AB=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp7D->tl))->hd)->pattern,& _tmp7A,_tmp73);*_tmp7E=_tmp1AB;});_tmp7E;});next_pattern=_tmp1AC;});
if(next_pattern->tvars_and_bounds_opt != 0  && (*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 340
({struct Cyc_Tcenv_Tenv*_tmp1AD=Cyc_Tcenv_clear_fallthru(te);te=_tmp1AD;});else{
# 342
struct Cyc_List_List*_tmp7F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp80=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
({struct Cyc_Tcenv_Tenv*_tmp1AE=Cyc_Tcenv_set_fallthru(te,_tmp80,_tmp7F,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp7D->tl))->hd);te=_tmp1AE;});}}else{
# 348
({struct Cyc_Tcenv_Tenv*_tmp1AF=Cyc_Tcenv_clear_fallthru(te);te=_tmp1AF;});}{
# 350
struct Cyc_Absyn_Pat*_tmp81=((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp81->topt),_tmp7A)){
({struct Cyc_String_pa_PrintArg_struct _tmp84=({struct Cyc_String_pa_PrintArg_struct _tmp158;_tmp158.tag=0U,({
struct _dyneither_ptr _tmp1B0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp7A));_tmp158.f1=_tmp1B0;});_tmp158;});struct Cyc_String_pa_PrintArg_struct _tmp85=({struct Cyc_String_pa_PrintArg_struct _tmp157;_tmp157.tag=0U,({struct _dyneither_ptr _tmp1B1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp81->topt)));_tmp157.f1=_tmp1B1;});_tmp157;});void*_tmp82[2U];_tmp82[0]=& _tmp84,_tmp82[1]=& _tmp85;({unsigned int _tmp1B3=((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->loc;struct _dyneither_ptr _tmp1B2=({const char*_tmp83="switch on type %s, but case expects type %s";_tag_dyneither(_tmp83,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1B3,_tmp1B2,_tag_dyneither(_tmp82,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}else{
# 357
Cyc_Tcpat_check_pat_regions(te,_tmp81,this_pattern.patvars);}
# 359
({struct Cyc_Core_Opt*_tmp1B4=({struct Cyc_Core_Opt*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->v=this_pattern.patvars;_tmp86;});((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->pat_vars=_tmp1B4;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp7D->hd)->where_clause,1);};}}
# 363
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,_tmp148,_tmp147);
return;};};}case 15U: _LL1D: _tmp14C=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp14B=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_tmp14A=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp44)->f3;_LL1E:
# 371
({void*_tmp1B8=({void*_tmp1B7=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1B6=Cyc_Absyn_new_stmt(_tmp14C->r,_tmp14C->loc);_tmp87->f1=_tmp1B6;}),({struct Cyc_Absyn_Stmt*_tmp1B5=Cyc_Absyn_skip_stmt(_tmp14C->loc);_tmp87->f2=_tmp1B5;});_tmp87;});Cyc_Absyn_new_stmt(_tmp1B7,_tmp14C->loc);})->r;_tmp14C->r=_tmp1B8;});
# 373
Cyc_Tcstmt_tcStmt(te,_tmp14C,1);{
# 376
struct _tuple2*def_v=({struct _tuple2*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp1BA=({struct _dyneither_ptr*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct _dyneither_ptr _tmp1B9=({const char*_tmp9C="exn";_tag_dyneither(_tmp9C,sizeof(char),4U);});*_tmp9D=_tmp1B9;});_tmp9D;});_tmp9E->f2=_tmp1BA;});_tmp9E;});
struct Cyc_Absyn_Vardecl*_tmp88=Cyc_Absyn_new_vardecl(0U,def_v,(void*)& Cyc_Absyn_VoidType_val,0);
struct Cyc_Absyn_Pat*_tmp89=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=1U,_tmp9B->f1=_tmp88,({struct Cyc_Absyn_Pat*_tmp1BB=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp9B->f2=_tmp1BB;});_tmp9B;}),0U);
struct Cyc_Absyn_Stmt*_tmp8A=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=11U,({struct Cyc_Absyn_Exp*_tmp1BC=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=4U,_tmp99->f1=_tmp88;_tmp99;}),0U);_tmp9A->f1=_tmp1BC;}),_tmp9A->f2=1;_tmp9A;}),0U),0U);
struct Cyc_Absyn_Switch_clause*_tmp8B=({struct Cyc_Absyn_Switch_clause*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->pattern=_tmp89,_tmp98->pat_vars=0,_tmp98->where_clause=0,_tmp98->body=_tmp8A,_tmp98->loc=0U;_tmp98;});
({struct Cyc_List_List*_tmp1BE=({struct Cyc_List_List*_tmp1BD=*_tmp14B;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp1BD,({struct Cyc_Absyn_Switch_clause*_tmp8C[1U];_tmp8C[0]=_tmp8B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp8C,sizeof(struct Cyc_Absyn_Switch_clause*),1U));}));});*_tmp14B=_tmp1BE;});{
# 388
void*_tmp8D=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*_tmp8E=*_tmp14B;for(0;_tmp8E != 0;_tmp8E=_tmp8E->tl){
if(first_case){
first_case=0;
({struct Cyc_Tcpat_TcPatResult _tmp1BF=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->pattern,& _tmp8D,0);this_pattern=_tmp1BF;});}else{
# 397
this_pattern=*((struct Cyc_Tcpat_TcPatResult*)_check_null(next_pattern));}
# 399
if(_tmp8E->tl != 0){
({struct Cyc_Tcpat_TcPatResult*_tmp1C1=({struct Cyc_Tcpat_TcPatResult*_tmp8F=_cycalloc(sizeof(*_tmp8F));({struct Cyc_Tcpat_TcPatResult _tmp1C0=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp8E->tl))->hd)->pattern,& _tmp8D,0);*_tmp8F=_tmp1C0;});_tmp8F;});next_pattern=_tmp1C1;});
if(next_pattern->tvars_and_bounds_opt != 0  && (*next_pattern->tvars_and_bounds_opt).f2 != 0)
# 403
({struct Cyc_Tcenv_Tenv*_tmp1C2=Cyc_Tcenv_clear_fallthru(te);te=_tmp1C2;});else{
# 405
struct Cyc_List_List*_tmp90=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(next_pattern->patvars)).f1));
struct Cyc_List_List*_tmp91=next_pattern->tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple16*))Cyc_Core_fst,(*next_pattern->tvars_and_bounds_opt).f1);
({struct Cyc_Tcenv_Tenv*_tmp1C3=Cyc_Tcenv_set_fallthru(te,_tmp91,_tmp90,(struct Cyc_Absyn_Switch_clause*)((struct Cyc_List_List*)_check_null(_tmp8E->tl))->hd);te=_tmp1C3;});}}else{
# 411
({struct Cyc_Tcenv_Tenv*_tmp1C4=Cyc_Tcenv_clear_fallthru(te);te=_tmp1C4;});}{
# 413
struct Cyc_Absyn_Pat*_tmp92=((struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->pattern;
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp92->topt),_tmp8D)){
({struct Cyc_String_pa_PrintArg_struct _tmp95=({struct Cyc_String_pa_PrintArg_struct _tmp15A;_tmp15A.tag=0U,({
struct _dyneither_ptr _tmp1C5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp8D));_tmp15A.f1=_tmp1C5;});_tmp15A;});struct Cyc_String_pa_PrintArg_struct _tmp96=({struct Cyc_String_pa_PrintArg_struct _tmp159;_tmp159.tag=0U,({struct _dyneither_ptr _tmp1C6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp92->topt)));_tmp159.f1=_tmp1C6;});_tmp159;});void*_tmp93[2U];_tmp93[0]=& _tmp95,_tmp93[1]=& _tmp96;({unsigned int _tmp1C8=((struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->loc;struct _dyneither_ptr _tmp1C7=({const char*_tmp94="switch on type %s, but case expects type %s";_tag_dyneither(_tmp94,sizeof(char),44U);});Cyc_Tcutil_terr(_tmp1C8,_tmp1C7,_tag_dyneither(_tmp93,sizeof(void*),2U));});});
Cyc_Tcutil_explain_failure();}else{
# 420
Cyc_Tcpat_check_pat_regions(te,_tmp92,this_pattern.patvars);}
# 422
({struct Cyc_Core_Opt*_tmp1C9=({struct Cyc_Core_Opt*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->v=this_pattern.patvars;_tmp97;});((struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->pat_vars=_tmp1C9;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp8E->hd)->where_clause,1);};}}
# 426
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*_tmp14B,_tmp14A);
return;};};default: _LL1F: _tmp14E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp44)->f1;_tmp14D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp44)->f2;_LL20: {
# 430
struct _dyneither_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmp9F=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;
{void*_tmpA0=_tmp14E->r;void*_tmpA1=_tmpA0;struct _tuple2*_tmp12C;struct Cyc_List_List*_tmp12B;struct _dyneither_ptr*_tmp12A;struct Cyc_List_List*_tmp129;struct Cyc_Absyn_Fndecl*_tmp128;struct Cyc_Absyn_Tvar*_tmp127;struct Cyc_Absyn_Vardecl*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_List_List*_tmp124;struct Cyc_Absyn_Pat*_tmp123;struct Cyc_Core_Opt**_tmp122;struct Cyc_Absyn_Exp**_tmp121;void**_tmp120;struct Cyc_Absyn_Vardecl*_tmp11F;switch(*((int*)_tmpA1)){case 0U: _LL33: _tmp11F=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL34: {
# 434
struct Cyc_Absyn_Vardecl*_tmpA2=_tmp11F;enum Cyc_Absyn_Scope _tmpD5;union Cyc_Absyn_Nmspace _tmpD4;struct _dyneither_ptr*_tmpD3;struct Cyc_Absyn_Tqual _tmpD2;void*_tmpD1;struct Cyc_Absyn_Exp*_tmpD0;void**_tmpCF;struct Cyc_List_List*_tmpCE;_LL52: _tmpD5=_tmpA2->sc;_tmpD4=(_tmpA2->name)->f1;_tmpD3=(_tmpA2->name)->f2;_tmpD2=_tmpA2->tq;_tmpD1=_tmpA2->type;_tmpD0=_tmpA2->initializer;_tmpCF=(void**)& _tmpA2->rgn;_tmpCE=_tmpA2->attributes;_LL53:;{
void*_tmpA3=Cyc_Tcenv_curr_rgn(_tmp9F);
int is_local;
{enum Cyc_Absyn_Scope _tmpA4=_tmpD5;switch(_tmpA4){case Cyc_Absyn_Static: _LL55: _LL56:
 goto _LL58;case Cyc_Absyn_Extern: _LL57: _LL58:
 goto _LL5A;case Cyc_Absyn_ExternC: _LL59: _LL5A:
# 441
 _tmp11F->escapes=1;
is_local=0;goto _LL54;case Cyc_Absyn_Abstract: _LL5B: _LL5C:
# 444
({void*_tmpA5=0U;({unsigned int _tmp1CB=_tmp14E->loc;struct _dyneither_ptr _tmp1CA=({const char*_tmpA6="bad abstract scope for local variable";_tag_dyneither(_tmpA6,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp1CB,_tmp1CA,_tag_dyneither(_tmpA5,sizeof(void*),0U));});});
goto _LL5E;case Cyc_Absyn_Register: _LL5D: _LL5E:
 goto _LL60;default: _LL5F: _LL60:
 is_local=1;goto _LL54;}_LL54:;}
# 450
*_tmpCF=is_local?_tmpA3:(void*)& Cyc_Absyn_HeapRgn_val;
# 452
{void*_tmpA7=Cyc_Tcutil_compress(_tmpD1);void*_tmpA8=_tmpA7;void*_tmpB4;struct Cyc_Absyn_Tqual _tmpB3;union Cyc_Absyn_Constraint*_tmpB2;unsigned int _tmpB1;switch(*((int*)_tmpA8)){case 9U: _LL62: if(is_local){_LL63:
# 455
 _tmp11F->escapes=1;
_tmpD5=Cyc_Absyn_Extern;
is_local=0;
goto _LL61;}else{goto _LL66;}case 8U: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA8)->f1).num_elts == 0){_LL64: _tmpB4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA8)->f1).elt_type;_tmpB3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA8)->f1).tq;_tmpB2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA8)->f1).zero_term;_tmpB1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA8)->f1).zt_loc;if(_tmp11F->initializer != 0){_LL65:
# 460
{void*_tmpA9=((struct Cyc_Absyn_Exp*)_check_null(_tmp11F->initializer))->r;void*_tmpAA=_tmpA9;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_Absyn_Exp*_tmpAD;struct _dyneither_ptr _tmpAC;struct _dyneither_ptr _tmpAB;switch(*((int*)_tmpAA)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpAA)->f1).Wstring_c).tag){case 8U: _LL69: _tmpAB=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpAA)->f1).String_c).val;_LL6A:
# 462
({void*_tmp1D1=({void*_tmp1D0=({void*_tmp1CF=_tmpB4;struct Cyc_Absyn_Tqual _tmp1CE=_tmpB3;struct Cyc_Absyn_Exp*_tmp1CD=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmpAB,sizeof(char)),0U);union Cyc_Absyn_Constraint*_tmp1CC=_tmpB2;Cyc_Absyn_array_typ(_tmp1CF,_tmp1CE,_tmp1CD,_tmp1CC,_tmpB1);});_tmp11F->type=_tmp1D0;});_tmpD1=_tmp1D1;});
goto _LL68;case 9U: _LL6B: _tmpAC=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpAA)->f1).Wstring_c).val;_LL6C:
# 465
({void*_tmp1D7=({void*_tmp1D6=({void*_tmp1D5=_tmpB4;struct Cyc_Absyn_Tqual _tmp1D4=_tmpB3;struct Cyc_Absyn_Exp*_tmp1D3=Cyc_Absyn_uint_exp(1U,0U);union Cyc_Absyn_Constraint*_tmp1D2=_tmpB2;Cyc_Absyn_array_typ(_tmp1D5,_tmp1D4,_tmp1D3,_tmp1D2,_tmpB1);});_tmp11F->type=_tmp1D6;});_tmpD1=_tmp1D7;});
goto _LL68;default: goto _LL75;}case 27U: _LL6D: _tmpAD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAA)->f2;_LL6E:
 _tmpAE=_tmpAD;goto _LL70;case 28U: _LL6F: _tmpAE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAA)->f1;_LL70:
# 470
({void*_tmp1D9=({void*_tmp1D8=Cyc_Absyn_array_typ(_tmpB4,_tmpB3,_tmpAE,_tmpB2,_tmpB1);_tmp11F->type=_tmp1D8;});_tmpD1=_tmp1D9;});
goto _LL68;case 36U: _LL71: _tmpAF=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAA)->f2;_LL72:
 _tmpB0=_tmpAF;goto _LL74;case 26U: _LL73: _tmpB0=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAA)->f1;_LL74:
# 474
({void*_tmp1DF=({void*_tmp1DE=({void*_tmp1DD=_tmpB4;struct Cyc_Absyn_Tqual _tmp1DC=_tmpB3;struct Cyc_Absyn_Exp*_tmp1DB=Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmpB0),0U);union Cyc_Absyn_Constraint*_tmp1DA=_tmpB2;Cyc_Absyn_array_typ(_tmp1DD,_tmp1DC,_tmp1DB,_tmp1DA,_tmpB1);});_tmp11F->type=_tmp1DE;});_tmpD1=_tmp1DF;});
goto _LL68;default: _LL75: _LL76:
 goto _LL68;}_LL68:;}
# 478
goto _LL61;}else{goto _LL66;}}else{goto _LL66;}default: _LL66: _LL67:
 goto _LL61;}_LL61:;}{
# 485
struct Cyc_List_List*_tmpB5=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmp9F);
int _tmpB6=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmp9F,_tmpB5,& Cyc_Tcutil_tmk,_tmpB6,1,_tmpD1);
({int _tmp1E0=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmp11F->tq).print_const,_tmpD1);(_tmp11F->tq).real_const=_tmp1E0;});{
# 490
struct Cyc_Tcenv_Tenv*_tmpB7=_tmp9F;
if(_tmpD5 == Cyc_Absyn_Extern  || _tmpD5 == Cyc_Absyn_ExternC)
({void*_tmpB8=0U;({unsigned int _tmp1E2=_tmp14E->loc;struct _dyneither_ptr _tmp1E1=({const char*_tmpB9="extern declarations are not yet supported within functions";_tag_dyneither(_tmpB9,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1E2,_tmp1E1,_tag_dyneither(_tmpB8,sizeof(void*),0U));});});
if(_tmpD0 != 0){
Cyc_Tcexp_tcExpInitializer(_tmpB7,& _tmpD1,_tmpD0);
# 496
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpD0))
({void*_tmpBA=0U;({unsigned int _tmp1E4=_tmp14E->loc;struct _dyneither_ptr _tmp1E3=({const char*_tmpBB="initializer for static variable needs to be a constant expression";_tag_dyneither(_tmpBB,sizeof(char),66U);});Cyc_Tcutil_terr(_tmp1E4,_tmp1E3,_tag_dyneither(_tmpBA,sizeof(void*),0U));});});
if(!Cyc_Tcutil_coerce_assign(_tmpB7,_tmpD0,_tmpD1)){
struct _dyneither_ptr _tmpBC=*_tmpD3;
struct _dyneither_ptr _tmpBD=Cyc_Absynpp_typ2string(_tmpD1);
struct _dyneither_ptr _tmpBE=Cyc_Absynpp_typ2string((void*)_check_null(_tmpD0->topt));
if(((_get_dyneither_size(_tmpBC,sizeof(char))+ _get_dyneither_size(_tmpBD,sizeof(char)))+ _get_dyneither_size(_tmpBE,sizeof(char)))+ 50 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpC1=({struct Cyc_String_pa_PrintArg_struct _tmp15D;_tmp15D.tag=0U,_tmp15D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBC);_tmp15D;});struct Cyc_String_pa_PrintArg_struct _tmpC2=({struct Cyc_String_pa_PrintArg_struct _tmp15C;_tmp15C.tag=0U,_tmp15C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBD);_tmp15C;});struct Cyc_String_pa_PrintArg_struct _tmpC3=({struct Cyc_String_pa_PrintArg_struct _tmp15B;_tmp15B.tag=0U,_tmp15B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE);_tmp15B;});void*_tmpBF[3U];_tmpBF[0]=& _tmpC1,_tmpBF[1]=& _tmpC2,_tmpBF[2]=& _tmpC3;({unsigned int _tmp1E6=_tmp14E->loc;struct _dyneither_ptr _tmp1E5=({const char*_tmpC0="%s was declared with type %s but initialized with type %s.";_tag_dyneither(_tmpC0,sizeof(char),59U);});Cyc_Tcutil_terr(_tmp1E6,_tmp1E5,_tag_dyneither(_tmpBF,sizeof(void*),3U));});});else{
# 505
if((_get_dyneither_size(_tmpBC,sizeof(char))+ _get_dyneither_size(_tmpBD,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmpBE,sizeof(char))+ 25 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpC6=({struct Cyc_String_pa_PrintArg_struct _tmp160;_tmp160.tag=0U,_tmp160.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBC);_tmp160;});struct Cyc_String_pa_PrintArg_struct _tmpC7=({struct Cyc_String_pa_PrintArg_struct _tmp15F;_tmp15F.tag=0U,_tmp15F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBD);_tmp15F;});struct Cyc_String_pa_PrintArg_struct _tmpC8=({struct Cyc_String_pa_PrintArg_struct _tmp15E;_tmp15E.tag=0U,_tmp15E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE);_tmp15E;});void*_tmpC4[3U];_tmpC4[0]=& _tmpC6,_tmpC4[1]=& _tmpC7,_tmpC4[2]=& _tmpC8;({unsigned int _tmp1E8=_tmp14E->loc;struct _dyneither_ptr _tmp1E7=({const char*_tmpC5="%s was declared with type %s\n but initialized with type %s.";_tag_dyneither(_tmpC5,sizeof(char),60U);});Cyc_Tcutil_terr(_tmp1E8,_tmp1E7,_tag_dyneither(_tmpC4,sizeof(void*),3U));});});else{
# 510
({struct Cyc_String_pa_PrintArg_struct _tmpCB=({struct Cyc_String_pa_PrintArg_struct _tmp163;_tmp163.tag=0U,_tmp163.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBC);_tmp163;});struct Cyc_String_pa_PrintArg_struct _tmpCC=({struct Cyc_String_pa_PrintArg_struct _tmp162;_tmp162.tag=0U,_tmp162.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBD);_tmp162;});struct Cyc_String_pa_PrintArg_struct _tmpCD=({struct Cyc_String_pa_PrintArg_struct _tmp161;_tmp161.tag=0U,_tmp161.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmpBE);_tmp161;});void*_tmpC9[3U];_tmpC9[0]=& _tmpCB,_tmpC9[1]=& _tmpCC,_tmpC9[2]=& _tmpCD;({unsigned int _tmp1EA=_tmp14E->loc;struct _dyneither_ptr _tmp1E9=({const char*_tmpCA="%s declared with type \n%s\n but initialized with type \n%s.";_tag_dyneither(_tmpCA,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp1EA,_tmp1E9,_tag_dyneither(_tmpC9,sizeof(void*),3U));});});}}
# 512
Cyc_Tcutil_unify(_tmpD1,(void*)_check_null(_tmpD0->topt));
Cyc_Tcutil_explain_failure();}}
# 516
Cyc_Tcstmt_tcStmt(_tmpB7,_tmp14D,0);
return;};};};}case 2U: _LL35: _tmp123=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmp122=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_tmp121=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f3;_tmp120=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmpA1)->f4;_LL36: {
# 524
struct Cyc_Absyn_Exp*_tmpD6=*_tmp121;
{void*_tmpD7=_tmp123->r;void*_tmpD8=_tmpD7;switch(*((int*)_tmpD8)){case 1U: _LL78: _LL79:
 goto _LL7B;case 2U: _LL7A: _LL7B:
 goto _LL7D;case 15U: _LL7C: _LL7D:
 goto _LL77;default: _LL7E: _LL7F:
# 530
 if(!Cyc_Tcutil_is_var_exp(_tmpD6)){
struct Cyc_Absyn_Vardecl*_tmpD9=({struct _tuple2*_tmp1EB=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_tmp1EB,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmpDA=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmpDB=({struct Cyc_Absyn_Pat*_tmp1EF=({void*_tmp1ED=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->tag=1U,_tmpDE->f1=_tmpD9,({struct Cyc_Absyn_Pat*_tmp1EC=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpD6->loc);_tmpDE->f2=_tmp1EC;});_tmpDE;});Cyc_Absyn_new_pat(_tmp1ED,_tmpD6->loc);});struct Cyc_Absyn_Exp*_tmp1EE=_tmpD6;Cyc_Absyn_let_decl(_tmp1EF,_tmp1EE,s0->loc);});
# 536
struct Cyc_Absyn_Stmt*_tmpDC=Cyc_Absyn_decl_stmt(_tmpDB,_tmpDA,s0->loc);
# 538
s0->r=_tmpDC->r;
({struct Cyc_Absyn_Exp*_tmp1F1=({void*_tmp1F0=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->tag=4U,_tmpDD->f1=_tmpD9;_tmpDD;});Cyc_Absyn_varb_exp(_tmp1F0,_tmpD6->loc);});*_tmp121=_tmp1F1;});
Cyc_Tcstmt_tcStmt(_tmp9F,s0,new_block);
return;}}_LL77:;}
# 544
Cyc_Tcexp_tcExpInitializer(_tmp9F,0,_tmpD6);{
# 546
void*pat_type=(void*)_check_null(_tmpD6->topt);
# 548
struct Cyc_Tcpat_TcPatResult _tmpDF=Cyc_Tcpat_tcPat(_tmp9F,_tmp123,& pat_type,_tmpD6);
({struct Cyc_Core_Opt*_tmp1F2=({struct Cyc_Core_Opt*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->v=_tmpDF.patvars;_tmpE0;});*_tmp122=_tmp1F2;});
if(!({void*_tmp1F3=(void*)_check_null(_tmp123->topt);Cyc_Tcutil_unify(_tmp1F3,(void*)_check_null(_tmpD6->topt));}) && !
Cyc_Tcutil_coerce_assign(_tmp9F,_tmpD6,(void*)_check_null(_tmp123->topt))){
({struct Cyc_String_pa_PrintArg_struct _tmpE3=({struct Cyc_String_pa_PrintArg_struct _tmp165;_tmp165.tag=0U,({
struct _dyneither_ptr _tmp1F4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp123->topt)));_tmp165.f1=_tmp1F4;});_tmp165;});struct Cyc_String_pa_PrintArg_struct _tmpE4=({struct Cyc_String_pa_PrintArg_struct _tmp164;_tmp164.tag=0U,({struct _dyneither_ptr _tmp1F5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmpD6->topt)));_tmp164.f1=_tmp1F5;});_tmp164;});void*_tmpE1[2U];_tmpE1[0]=& _tmpE3,_tmpE1[1]=& _tmpE4;({unsigned int _tmp1F7=_tmp14E->loc;struct _dyneither_ptr _tmp1F6=({const char*_tmpE2="pattern type %s does not match definition type %s";_tag_dyneither(_tmpE2,sizeof(char),50U);});Cyc_Tcutil_terr(_tmp1F7,_tmp1F6,_tag_dyneither(_tmpE1,sizeof(void*),2U));});});
({void*_tmp1F8=(void*)_check_null(_tmp123->topt);Cyc_Tcutil_unify(_tmp1F8,(void*)_check_null(_tmpD6->topt));});
Cyc_Tcutil_explain_failure();}else{
# 558
Cyc_Tcpat_check_pat_regions(_tmp9F,_tmp123,_tmpDF.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmp123->loc,_tmp9F,_tmp123,_tmp120);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmp9F,_tmpDF,_tmp14D,0,0);
return;};}case 3U: _LL37: _tmp124=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL38: {
# 564
void*_tmpE5=Cyc_Tcenv_curr_rgn(_tmp9F);
struct Cyc_Tcenv_Tenv*_tmpE6=_tmp9F;
for(0;_tmp124 != 0;_tmp124=_tmp124->tl){
struct Cyc_Absyn_Vardecl*_tmpE7=(struct Cyc_Absyn_Vardecl*)_tmp124->hd;
struct Cyc_Absyn_Vardecl*_tmpE8=_tmpE7;union Cyc_Absyn_Nmspace _tmpEB;void*_tmpEA;void**_tmpE9;_LL81: _tmpEB=(_tmpE8->name)->f1;_tmpEA=_tmpE8->type;_tmpE9=(void**)& _tmpE8->rgn;_LL82:;
*_tmpE9=_tmpE5;
({unsigned int _tmp1FB=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1FA=_tmpE6;struct Cyc_List_List*_tmp1F9=Cyc_Tcenv_lookup_type_vars(_tmpE6);Cyc_Tcutil_check_type(_tmp1FB,_tmp1FA,_tmp1F9,& Cyc_Tcutil_tmk,1,1,_tmpEA);});}
# 572
Cyc_Tcstmt_tcStmt(_tmpE6,_tmp14D,0);
return;}case 4U: _LL39: _tmp127=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmp126=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_tmp125=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmpA1)->f3;_LL3A:
# 578
({void*_tmp1FF=({void*_tmp1FE=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=2U,({struct Cyc_Absyn_Stmt*_tmp1FD=Cyc_Absyn_new_stmt(_tmp14D->r,_tmp14D->loc);_tmpEC->f1=_tmp1FD;}),({struct Cyc_Absyn_Stmt*_tmp1FC=Cyc_Absyn_skip_stmt(_tmp14D->loc);_tmpEC->f2=_tmp1FC;});_tmpEC;});Cyc_Absyn_new_stmt(_tmp1FE,_tmp14D->loc);})->r;_tmp14D->r=_tmp1FF;});{
# 580
struct Cyc_Tcenv_Tenv*_tmpED=_tmp9F;
struct Cyc_Absyn_Vardecl*_tmpEE=_tmp126;void**_tmp10B;void**_tmp10A;_LL84: _tmp10B=(void**)& _tmpEE->type;_tmp10A=(void**)& _tmpEE->rgn;_LL85:;{
void*_tmpEF=Cyc_Tcenv_curr_rgn(_tmp9F);
*_tmp10A=_tmpEF;{
void*rgn_typ;
if((unsigned int)_tmp125){
# 588
struct _tuple2*drname=({struct _tuple2*_tmp104=_cycalloc(sizeof(*_tmp104));((_tmp104->f1).Abs_n).tag=2U,({struct Cyc_List_List*_tmp204=({struct _dyneither_ptr*_tmpFF[1U];({struct _dyneither_ptr*_tmp203=({struct _dyneither_ptr*_tmp101=_cycalloc(sizeof(*_tmp101));({struct _dyneither_ptr _tmp202=({const char*_tmp100="Core";_tag_dyneither(_tmp100,sizeof(char),5U);});*_tmp101=_tmp202;});_tmp101;});_tmpFF[0]=_tmp203;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFF,sizeof(struct _dyneither_ptr*),1U));});((_tmp104->f1).Abs_n).val=_tmp204;}),({
struct _dyneither_ptr*_tmp201=({struct _dyneither_ptr*_tmp103=_cycalloc(sizeof(*_tmp103));({struct _dyneither_ptr _tmp200=({const char*_tmp102="DynamicRegion";_tag_dyneither(_tmp102,sizeof(char),14U);});*_tmp103=_tmp200;});_tmp103;});_tmp104->f2=_tmp201;});_tmp104;});
void*_tmpF0=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpFE=_cycalloc(sizeof(*_tmpFE));_tmpFE->v=& Cyc_Tcutil_trk;_tmpFE;}),0);
({void*_tmp205=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpF1=_cycalloc(sizeof(*_tmpF1));_tmpF1->v=& Cyc_Tcutil_rk;_tmpF1;}),0);rgn_typ=_tmp205;});{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF2=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));
_tmpFD->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp207=Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);(_tmpFD->f1).aggr_info=_tmp207;}),({
struct Cyc_List_List*_tmp206=({void*_tmpFC[1U];_tmpFC[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpFC,sizeof(void*),1U));});(_tmpFD->f1).targs=_tmp206;});_tmpFD;});
void*_tmpF3=({void*_tmp20A=(void*)_tmpF2;void*_tmp209=_tmpF0;struct Cyc_Absyn_Tqual _tmp208=
Cyc_Absyn_empty_tqual(0U);
# 595
Cyc_Absyn_at_typ(_tmp20A,_tmp209,_tmp208,Cyc_Absyn_false_conref);});
# 597
({unsigned int _tmp20D=s0->loc;struct Cyc_Tcenv_Tenv*_tmp20C=_tmp9F;struct Cyc_List_List*_tmp20B=Cyc_Tcenv_lookup_type_vars(_tmp9F);Cyc_Tcutil_check_type(_tmp20D,_tmp20C,_tmp20B,& Cyc_Tcutil_tmk,1,0,_tmpF3);});{
void*_tmpF4=Cyc_Tcexp_tcExp(_tmp9F,& _tmpF3,_tmp125);
if(!Cyc_Tcutil_unify(_tmpF3,_tmpF4) && !
Cyc_Tcutil_coerce_assign(_tmp9F,_tmp125,_tmpF3))
({struct Cyc_String_pa_PrintArg_struct _tmpF7=({struct Cyc_String_pa_PrintArg_struct _tmp167;_tmp167.tag=0U,({
struct _dyneither_ptr _tmp20E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpF3));_tmp167.f1=_tmp20E;});_tmp167;});struct Cyc_String_pa_PrintArg_struct _tmpF8=({struct Cyc_String_pa_PrintArg_struct _tmp166;_tmp166.tag=0U,({struct _dyneither_ptr _tmp20F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpF4));_tmp166.f1=_tmp20F;});_tmp166;});void*_tmpF5[2U];_tmpF5[0]=& _tmpF7,_tmpF5[1]=& _tmpF8;({unsigned int _tmp211=s0->loc;struct _dyneither_ptr _tmp210=({const char*_tmpF6="expected %s but found %s";_tag_dyneither(_tmpF6,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp211,_tmp210,_tag_dyneither(_tmpF5,sizeof(void*),2U));});});
if(!Cyc_Tcutil_unify(_tmpF0,(void*)& Cyc_Absyn_UniqueRgn_val) && !
Cyc_Tcutil_unify(_tmpF0,(void*)& Cyc_Absyn_RefCntRgn_val))
({void*_tmpF9=0U;({unsigned int _tmp213=s0->loc;struct _dyneither_ptr _tmp212=({const char*_tmpFA="open is only allowed on unique or reference-counted keys";_tag_dyneither(_tmpFA,sizeof(char),57U);});Cyc_Tcutil_terr(_tmp213,_tmp212,_tag_dyneither(_tmpF9,sizeof(void*),0U));});});
({void*_tmp214=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB->tag=15U,_tmpFB->f1=rgn_typ;_tmpFB;});*_tmp10B=_tmp214;});
({struct Cyc_Tcenv_Tenv*_tmp215=Cyc_Tcenv_add_region(_tmpED,rgn_typ,1);_tmpED=_tmp215;});};};}else{
# 609
({void*_tmp216=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=2U,_tmp105->f1=_tmp127;_tmp105;});rgn_typ=_tmp216;});
({struct Cyc_Tcenv_Tenv*_tmp219=({unsigned int _tmp218=s0->loc;struct Cyc_Tcenv_Tenv*_tmp217=_tmpED;Cyc_Tcenv_add_type_vars(_tmp218,_tmp217,({struct Cyc_List_List*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->hd=_tmp127,_tmp106->tl=0;_tmp106;}));});_tmpED=_tmp219;});
({struct Cyc_Tcenv_Tenv*_tmp21A=Cyc_Tcenv_add_region(_tmpED,rgn_typ,0);_tmpED=_tmp21A;});}
# 613
({unsigned int _tmp21D=s0->loc;struct Cyc_Tcenv_Tenv*_tmp21C=_tmp9F;struct Cyc_List_List*_tmp21B=Cyc_Tcenv_lookup_type_vars(_tmpED);Cyc_Tcutil_check_type(_tmp21D,_tmp21C,_tmp21B,& Cyc_Tcutil_bk,1,0,*_tmp10B);});
if(!({void*_tmp21E=*_tmp10B;Cyc_Tcutil_unify(_tmp21E,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107->tag=15U,_tmp107->f1=rgn_typ;_tmp107;}));}))
({void*_tmp108=0U;({struct _dyneither_ptr _tmp21F=({const char*_tmp109="region stmt: type of region handle is wrong!";_tag_dyneither(_tmp109,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp21F,_tag_dyneither(_tmp108,sizeof(void*),0U));});});
# 617
Cyc_Tcstmt_tcStmt(_tmpED,_tmp14D,0);
return;};};};case 1U: _LL3B: _tmp128=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_LL3C: {
# 622
void*_tmp10C=Cyc_Tcenv_curr_rgn(_tmp9F);
if(_tmp128->sc != Cyc_Absyn_Public)
({void*_tmp10D=0U;({unsigned int _tmp221=_tmp14E->loc;struct _dyneither_ptr _tmp220=({const char*_tmp10E="bad storage class for inner function";_tag_dyneither(_tmp10E,sizeof(char),37U);});Cyc_Tcutil_terr(_tmp221,_tmp220,_tag_dyneither(_tmp10D,sizeof(void*),0U));});});
Cyc_Tcutil_check_fndecl_valid_type(_tmp14E->loc,_tmp9F,_tmp128);{
void*t=Cyc_Tcutil_fndecl2typ(_tmp128);
({struct Cyc_List_List*_tmp222=Cyc_Tcutil_transfer_fn_type_atts(t,_tmp128->attributes);_tmp128->attributes=_tmp222;});
# 629
{struct Cyc_List_List*atts=_tmp128->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmp10F=(void*)atts->hd;void*_tmp110=_tmp10F;switch(*((int*)_tmp110)){case 7U: _LL87: _LL88:
 goto _LL8A;case 6U: _LL89: _LL8A:
({struct Cyc_String_pa_PrintArg_struct _tmp113=({struct Cyc_String_pa_PrintArg_struct _tmp168;_tmp168.tag=0U,({
struct _dyneither_ptr _tmp223=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)atts->hd));_tmp168.f1=_tmp223;});_tmp168;});void*_tmp111[1U];_tmp111[0]=& _tmp113;({unsigned int _tmp225=_tmp14E->loc;struct _dyneither_ptr _tmp224=({const char*_tmp112="bad attribute %s for function";_tag_dyneither(_tmp112,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp225,_tmp224,_tag_dyneither(_tmp111,sizeof(void*),1U));});});
goto _LL86;default: _LL8B: _LL8C:
 goto _LL86;}_LL86:;}}{
# 637
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->sc=_tmp128->sc,_tmp114->name=_tmp128->name,_tmp114->varloc=0U,({struct Cyc_Absyn_Tqual _tmp22A=Cyc_Absyn_const_tqual(0U);_tmp114->tq=_tmp22A;}),({
void*_tmp229=({void*_tmp228=t;void*_tmp227=_tmp10C;struct Cyc_Absyn_Tqual _tmp226=Cyc_Absyn_empty_tqual(0U);Cyc_Absyn_at_typ(_tmp228,_tmp227,_tmp226,Cyc_Absyn_false_conref);});_tmp114->type=_tmp229;}),_tmp114->initializer=0,_tmp114->rgn=_tmp10C,_tmp114->attributes=0,_tmp114->escapes=0;_tmp114;});
# 641
_tmp128->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmp9F->le);
({struct Cyc_Tcenv_Fenv*_tmp22B=Cyc_Tcenv_nested_fenv(_tmp14E->loc,old_fenv,_tmp128);_tmp9F->le=_tmp22B;});
Cyc_Tcstmt_tcStmt(_tmp9F,_tmp128->body,0);
Cyc_Tcenv_check_delayed_effects(_tmp9F);
Cyc_Tcenv_check_delayed_constraints(_tmp9F);
_tmp9F->le=old_fenv;
Cyc_Tcstmt_tcStmt(_tmp9F,_tmp14D,0);
return;};};};}case 9U: _LL3D: _tmp12A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmp129=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_LL3E:
({struct _dyneither_ptr _tmp22C=({const char*_tmp115="namespace";_tag_dyneither(_tmp115,sizeof(char),10U);});unimp_msg_part=_tmp22C;});goto _LL32;case 10U: _LL3F: _tmp12C=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA1)->f1;_tmp12B=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmpA1)->f2;_LL40:
({struct _dyneither_ptr _tmp22D=({const char*_tmp116="using";_tag_dyneither(_tmp116,sizeof(char),6U);});unimp_msg_part=_tmp22D;});goto _LL32;case 5U: _LL41: _LL42:
({struct _dyneither_ptr _tmp22E=({const char*_tmp117="type";_tag_dyneither(_tmp117,sizeof(char),5U);});unimp_msg_part=_tmp22E;});goto _LL32;case 6U: _LL43: _LL44:
({struct _dyneither_ptr _tmp22F=({const char*_tmp118="datatype";_tag_dyneither(_tmp118,sizeof(char),9U);});unimp_msg_part=_tmp22F;});goto _LL32;case 7U: _LL45: _LL46:
({struct _dyneither_ptr _tmp230=({const char*_tmp119="enum";_tag_dyneither(_tmp119,sizeof(char),5U);});unimp_msg_part=_tmp230;});goto _LL32;case 8U: _LL47: _LL48:
({struct _dyneither_ptr _tmp231=({const char*_tmp11A="typedef";_tag_dyneither(_tmp11A,sizeof(char),8U);});unimp_msg_part=_tmp231;});goto _LL32;case 11U: _LL49: _LL4A:
({struct _dyneither_ptr _tmp232=({const char*_tmp11B="extern \"C\"";_tag_dyneither(_tmp11B,sizeof(char),11U);});unimp_msg_part=_tmp232;});goto _LL32;case 12U: _LL4B: _LL4C:
# 658
({struct _dyneither_ptr _tmp233=({const char*_tmp11C="extern \"C include\"";_tag_dyneither(_tmp11C,sizeof(char),19U);});unimp_msg_part=_tmp233;});goto _LL32;case 13U: _LL4D: _LL4E:
({struct _dyneither_ptr _tmp234=({const char*_tmp11D="__cyclone_port_on__";_tag_dyneither(_tmp11D,sizeof(char),20U);});unimp_msg_part=_tmp234;});goto _LL32;default: _LL4F: _LL50:
({struct _dyneither_ptr _tmp235=({const char*_tmp11E="__cyclone_port_off__";_tag_dyneither(_tmp11E,sizeof(char),21U);});unimp_msg_part=_tmp235;});goto _LL32;}_LL32:;}
# 662
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp12F=({struct Cyc_String_pa_PrintArg_struct _tmp169;_tmp169.tag=0U,_tmp169.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part);_tmp169;});void*_tmp12D[1U];_tmp12D[0]=& _tmp12F;({struct _dyneither_ptr _tmp236=({const char*_tmp12E="tcStmt: nested %s declarations unimplemented";_tag_dyneither(_tmp12E,sizeof(char),45U);});((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp236,_tag_dyneither(_tmp12D,sizeof(void*),1U));});}));}}_LL0:;}
