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
typedef void*Cyc_sarg_t;extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 113
void*Cyc_Core_snd(struct _tuple0*);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple1 Cyc_List_split(struct Cyc_List_List*x);
# 391
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);struct Cyc_PP_Ppstate;
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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 367
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 372
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
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
# 923
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned int);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 939
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 948
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 967
void*Cyc_Absyn_exn_typ();
# 989
void*Cyc_Absyn_at_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 1005
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1080
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1104
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1109
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*p,struct Cyc_Absyn_Exp*e,unsigned int loc);
# 1113
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1165
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
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
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 68
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned int,struct Cyc_Tcenv_Fenv*old_fenv,struct Cyc_Absyn_Fndecl*new_fn);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 92
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 94
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv(struct Cyc_Tcenv_Tenv*);
# 96
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 98
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 100
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*new_tvs,struct Cyc_List_List*vds,struct Cyc_Absyn_Switch_clause*clause);
# 104
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 122
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple12{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 125
const struct _tuple12*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 128
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned int,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned int);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region_equality(struct Cyc_Tcenv_Tenv*te,void*r1,void*r2,struct _tuple13**oldtv,unsigned int loc);
# 135
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 137
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int opened);
# 149
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
# 94
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
# 104
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 129
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 143
void Cyc_Tcutil_explain_failure();
# 145
int Cyc_Tcutil_unify(void*,void*);
# 148
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 176
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 211 "tcutil.h"
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 219
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 270
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);
# 292
int Cyc_Tcutil_new_tvar_id();
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 312
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 330
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 334
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 351
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
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
void Cyc_Tcpat_check_catch_overlap(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);struct Cyc_Hashtable_Table;
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
typedef struct Cyc_CfFlowInfo_UnionRInfo Cyc_CfFlowInfo_union_rinfo_t;struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroThis_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};
# 131 "cf_flowinfo.h"
typedef union Cyc_CfFlowInfo_FlowInfo Cyc_CfFlowInfo_flow_t;struct Cyc_CfFlowInfo_FlowEnv{struct _RegionHandle*r;void*zero;void*notzeroall;void*notzerothis;void*unknown_none;void*unknown_this;void*unknown_all;void*esc_none;void*esc_this;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 149
typedef struct Cyc_CfFlowInfo_FlowEnv*Cyc_CfFlowInfo_flow_env_t;
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int new_block);
# 48 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_tmp0=e->r;void*_tmp1=_tmp0;struct Cyc_Absyn_Exp*_tmp7;struct Cyc_Absyn_Exp*_tmp6;if(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->tag == 4)switch(((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f2){case Cyc_Absyn_PostInc: _LL1: _tmp6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL2:
({void*_tmp134=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp2=_cycalloc(sizeof(*_tmp2));({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp133=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp3;_tmp3.tag=4;_tmp3.f1=_tmp6;_tmp3.f2=Cyc_Absyn_PreInc;_tmp3;});_tmp2[0]=_tmp133;});_tmp2;});e->r=_tmp134;});goto _LL0;case Cyc_Absyn_PostDec: _LL3: _tmp7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp1)->f1;_LL4:
({void*_tmp136=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp4=_cycalloc(sizeof(*_tmp4));({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp135=({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct _tmp5;_tmp5.tag=4;_tmp5.f1=_tmp7;_tmp5.f2=Cyc_Absyn_PreDec;_tmp5;});_tmp4[0]=_tmp135;});_tmp4;});e->r=_tmp136;});goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 57
static void Cyc_Tcstmt_pattern_synth(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 59
struct Cyc_Tcpat_TcPatResult _tmp8=pat_res;struct _tuple1*_tmp13;struct Cyc_List_List*_tmp12;_LL8: _tmp13=_tmp8.tvars_and_bounds_opt;_tmp12=_tmp8.patvars;_LL9:;{
struct Cyc_List_List*_tmp9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*f)(struct _tuple16*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*))Cyc_Core_fst,_tmp12);
struct Cyc_List_List*_tmpA=_tmp13 == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmp13).f1);
struct Cyc_List_List*_tmpB=_tmp13 == 0?0:(*_tmp13).f2;
struct Cyc_List_List*_tmpC=_tmp13 == 0?0:((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,((struct Cyc_List_List*(*)(int(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_filter)((int(*)(struct _tuple15*))Cyc_Core_snd,(*_tmp13).f1));
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_add_type_vars(loc,te,_tmpA);
for(0;_tmpC != 0;_tmpC=_tmpC->tl){
({struct Cyc_Tcenv_Tenv*_tmp139=({struct Cyc_Tcenv_Tenv*_tmp138=te2;Cyc_Tcenv_add_region(_tmp138,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpD=_cycalloc(sizeof(*_tmpD));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp137=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE;_tmpE.tag=2;_tmpE.f1=(struct Cyc_Absyn_Tvar*)_tmpC->hd;_tmpE;});_tmpD[0]=_tmp137;});_tmpD;}),1);});te2=_tmp139;});}
({struct Cyc_Tcenv_Tenv*_tmp13A=Cyc_Tcenv_new_outlives_constraints(te2,_tmpB,loc);te2=_tmp13A;});
if(new_block)
({struct Cyc_Tcenv_Tenv*_tmp13B=Cyc_Tcenv_new_block(loc,te2);te2=_tmp13B;});{
void*_tmpF=Cyc_Tcenv_curr_rgn(te2);
{struct Cyc_List_List*_tmp10=_tmp9;for(0;_tmp10 != 0;_tmp10=_tmp10->tl){
if((struct Cyc_Absyn_Vardecl**)_tmp10->hd != 0)
# 74
(*((struct Cyc_Absyn_Vardecl**)_check_null((struct Cyc_Absyn_Vardecl**)_tmp10->hd)))->rgn=_tmpF;}}
# 77
if(where_opt != 0)
({struct Cyc_Tcenv_Tenv*_tmp13D=te2;struct Cyc_Absyn_Exp*_tmp13C=where_opt;Cyc_Tcexp_tcTest(_tmp13D,_tmp13C,({const char*_tmp11="switch clause guard";_tag_dyneither(_tmp11,sizeof(char),20);}));});
Cyc_Tcstmt_tcStmt(te2,s,0);};};}
# 84
static void Cyc_Tcstmt_tcStmtRefine(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,void*r1,void*r2,int new_block){
# 86
struct _tuple13*tk=0;
struct Cyc_Tcenv_Tenv*_tmp14=Cyc_Tcenv_add_region_equality(te,r1,r2,& tk,s0->loc);
Cyc_Tcstmt_tcStmt(_tmp14,s0,new_block);
if(tk != 0)
({void*_tmp13E=(*((struct _tuple13*)_check_null(tk))).f2;((*((struct _tuple13*)_check_null(tk))).f1)->kind=_tmp13E;});}
# 93
static int Cyc_Tcstmt_stmt_temp_var_counter=0;
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var(){
int _tmp15=Cyc_Tcstmt_stmt_temp_var_counter ++;
struct _tuple2*res=({struct _tuple2*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp141=({struct _dyneither_ptr*_tmp17=_cycalloc(sizeof(*_tmp17));({struct _dyneither_ptr _tmp140=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp1A;_tmp1A.tag=1;_tmp1A.f1=(unsigned int)_tmp15;({void*_tmp18[1]={& _tmp1A};({struct _dyneither_ptr _tmp13F=({const char*_tmp19="_stmttmp%X";_tag_dyneither(_tmp19,sizeof(char),11);});Cyc_aprintf(_tmp13F,_tag_dyneither(_tmp18,sizeof(void*),1));});});});_tmp17[0]=_tmp140;});_tmp17;});_tmp16->f2=_tmp141;});_tmp16;});
return res;}
# 107 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 109
void*_tmp1B=s0->r;void*_tmp1C=_tmp1B;struct Cyc_Absyn_Decl*_tmp132;struct Cyc_Absyn_Stmt*_tmp131;struct Cyc_Absyn_Stmt*_tmp130;struct Cyc_List_List**_tmp12F;void**_tmp12E;struct Cyc_Absyn_Exp**_tmp12D;struct Cyc_List_List*_tmp12C;void**_tmp12B;struct _dyneither_ptr*_tmp12A;struct Cyc_Absyn_Stmt*_tmp129;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_Switch_clause***_tmp127;struct Cyc_Absyn_Stmt*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Stmt*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Stmt*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Stmt*_tmp11F;struct Cyc_Absyn_Stmt*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Stmt*_tmp11C;struct Cyc_Absyn_Stmt*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Stmt*_tmp119;struct Cyc_Absyn_Stmt*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Stmt*_tmp116;struct Cyc_Absyn_Stmt*_tmp115;struct Cyc_Absyn_Exp*_tmp114;switch(*((int*)_tmp1C)){case 0: _LLB: _LLC:
# 111
 return;case 1: _LLD: _tmp114=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_LLE:
# 114
 Cyc_Tcexp_tcExp(te,0,_tmp114);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(_tmp114);
return;case 2: _LLF: _tmp116=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp115=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2;_LL10:
# 120
 Cyc_Tcstmt_tcStmt(te,_tmp116,1);
Cyc_Tcstmt_tcStmt(te,_tmp115,1);
return;case 3: _LL11: _tmp117=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_LL12: {
# 125
void*t=Cyc_Tcenv_return_typ(te);
if(_tmp117 == 0){
void*_tmp1D=Cyc_Tcutil_compress(t);void*_tmp1E=_tmp1D;switch(*((int*)_tmp1E)){case 0: _LL2C: _LL2D:
 goto _LL2B;case 7: _LL2E: _LL2F:
 goto _LL31;case 6: _LL30: _LL31:
# 131
({struct Cyc_String_pa_PrintArg_struct _tmp21;_tmp21.tag=0;({struct _dyneither_ptr _tmp142=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp21.f1=_tmp142;});({void*_tmp1F[1]={& _tmp21};({unsigned int _tmp144=s0->loc;struct _dyneither_ptr _tmp143=({const char*_tmp20="should return a value of type %s";_tag_dyneither(_tmp20,sizeof(char),33);});Cyc_Tcutil_warn(_tmp144,_tmp143,_tag_dyneither(_tmp1F,sizeof(void*),1));});});});goto _LL2B;default: _LL32: _LL33:
# 133
({struct Cyc_String_pa_PrintArg_struct _tmp24;_tmp24.tag=0;({struct _dyneither_ptr _tmp145=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp24.f1=_tmp145;});({void*_tmp22[1]={& _tmp24};({unsigned int _tmp147=s0->loc;struct _dyneither_ptr _tmp146=({const char*_tmp23="must return a value of type %s";_tag_dyneither(_tmp23,sizeof(char),31);});Cyc_Tcutil_terr(_tmp147,_tmp146,_tag_dyneither(_tmp22,sizeof(void*),1));});});});goto _LL2B;}_LL2B:;}else{
# 137
int bogus=0;
struct Cyc_Absyn_Exp*e=_tmp117;
Cyc_Tcexp_tcExp(te,& t,e);
if(!Cyc_Tcutil_coerce_arg(te,e,t,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0;({struct _dyneither_ptr _tmp148=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));_tmp28.f1=_tmp148;});({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;({struct _dyneither_ptr _tmp149=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(e->topt)));_tmp27.f1=_tmp149;});({void*_tmp25[2]={& _tmp27,& _tmp28};({unsigned int _tmp14B=s0->loc;struct _dyneither_ptr _tmp14A=({const char*_tmp26="returns value of type %s but requires %s";_tag_dyneither(_tmp26,sizeof(char),41);});Cyc_Tcutil_terr(_tmp14B,_tmp14A,_tag_dyneither(_tmp25,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 145
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t) && !Cyc_Tcutil_is_noalias_path(e))
({void*_tmp29=0;({unsigned int _tmp14D=e->loc;struct _dyneither_ptr _tmp14C=({const char*_tmp2A="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp2A,sizeof(char),49);});Cyc_Tcutil_terr(_tmp14D,_tmp14C,_tag_dyneither(_tmp29,sizeof(void*),0));});});}
# 148
return;}case 4: _LL13: _tmp11A=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp119=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2;_tmp118=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp1C)->f3;_LL14: {
# 151
struct Cyc_Tcexp_TestEnv _tmp2B=({struct Cyc_Tcenv_Tenv*_tmp14F=te;struct Cyc_Absyn_Exp*_tmp14E=_tmp11A;Cyc_Tcexp_tcTest(_tmp14F,_tmp14E,({const char*_tmp32="if statement";_tag_dyneither(_tmp32,sizeof(char),13);}));});struct Cyc_Tcexp_TestEnv _tmp2C=_tmp2B;struct _tuple0*_tmp31;int _tmp30;_LL35: _tmp31=_tmp2C.eq;_tmp30=_tmp2C.isTrue;_LL36:;
{struct _tuple0*_tmp2D=_tmp31;void*_tmp2F;void*_tmp2E;if(_tmp2D != 0){_LL38: _tmp2F=_tmp2D->f1;_tmp2E=_tmp2D->f2;_LL39:
# 154
 if(_tmp30){
Cyc_Tcstmt_tcStmtRefine(te,_tmp119,_tmp2F,_tmp2E,1);
Cyc_Tcstmt_tcStmt(te,_tmp118,1);}else{
# 159
Cyc_Tcstmt_tcStmt(te,_tmp119,1);
Cyc_Tcstmt_tcStmtRefine(te,_tmp118,_tmp2F,_tmp2E,1);}
# 162
goto _LL37;}else{_LL3A: _LL3B:
# 164
 Cyc_Tcstmt_tcStmt(te,_tmp119,1);
Cyc_Tcstmt_tcStmt(te,_tmp118,1);
goto _LL37;}_LL37:;}
# 168
return;}case 5: _LL15: _tmp11D=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1).f1;_tmp11C=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1).f2;_tmp11B=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2;_LL16:
# 171
({struct Cyc_Tcenv_Tenv*_tmp151=te;struct Cyc_Absyn_Exp*_tmp150=_tmp11D;Cyc_Tcexp_tcTest(_tmp151,_tmp150,({const char*_tmp33="while loop";_tag_dyneither(_tmp33,sizeof(char),11);}));});
Cyc_Tcstmt_tcStmt(te,_tmp11B,1);
return;case 9: _LL17: _tmp123=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp122=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2).f1;_tmp121=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2).f2;_tmp120=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1C)->f3).f1;_tmp11F=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1C)->f3).f2;_tmp11E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp1C)->f4;_LL18:
# 176
 Cyc_Tcexp_tcExp(te,0,_tmp123);
({struct Cyc_Tcenv_Tenv*_tmp153=te;struct Cyc_Absyn_Exp*_tmp152=_tmp122;Cyc_Tcexp_tcTest(_tmp153,_tmp152,({const char*_tmp34="for loop";_tag_dyneither(_tmp34,sizeof(char),9);}));});
Cyc_Tcstmt_tcStmt(te,_tmp11E,1);
Cyc_Tcexp_tcExp(te,0,_tmp120);
Cyc_Tcstmt_simplify_unused_result_exp(_tmp120);
return;case 14: _LL19: _tmp126=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp125=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2).f1;_tmp124=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2).f2;_LL1A:
# 184
 Cyc_Tcstmt_tcStmt(te,_tmp126,1);
({struct Cyc_Tcenv_Tenv*_tmp155=te;struct Cyc_Absyn_Exp*_tmp154=_tmp125;Cyc_Tcexp_tcTest(_tmp155,_tmp154,({const char*_tmp35="do loop";_tag_dyneither(_tmp35,sizeof(char),8);}));});
return;case 6: _LL1B: _LL1C:
# 188
 goto _LL1E;case 7: _LL1D: _LL1E:
 goto _LL20;case 8: _LL1F: _LL20:
 return;case 11: _LL21: _tmp128=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp127=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2;_LL22: {
# 193
const struct _tuple12*_tmp36=Cyc_Tcenv_process_fallthru(te,s0,_tmp127);
if(_tmp36 == (const struct _tuple12*)0){
({void*_tmp37=0;({unsigned int _tmp157=s0->loc;struct _dyneither_ptr _tmp156=({const char*_tmp38="fallthru not in a non-last case";_tag_dyneither(_tmp38,sizeof(char),32);});Cyc_Tcutil_terr(_tmp157,_tmp156,_tag_dyneither(_tmp37,sizeof(void*),0));});});
return;}{
# 198
struct Cyc_List_List*_tmp39=(*_tmp36).f2;
struct Cyc_List_List*_tmp3A=(*_tmp36).f3;
struct Cyc_List_List*instantiation=({
struct Cyc_List_List*_tmp158=Cyc_Tcenv_lookup_type_vars(te);((struct Cyc_List_List*(*)(struct _tuple13*(*f)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_make_inst_var,_tmp158,_tmp39);});
struct Cyc_List_List*_tmp3B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_List_List*,void*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_substitute,instantiation,_tmp3A);
for(0;_tmp3B != 0  && _tmp128 != 0;(_tmp3B=_tmp3B->tl,_tmp128=_tmp128->tl)){
# 205
int bogus=0;
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)_tmp128->hd);
if(!Cyc_Tcutil_coerce_arg(te,(struct Cyc_Absyn_Exp*)_tmp128->hd,(void*)_tmp3B->hd,& bogus)){
({struct Cyc_String_pa_PrintArg_struct _tmp3F;_tmp3F.tag=0;({struct _dyneither_ptr _tmp159=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 210
Cyc_Absynpp_typ2string((void*)_tmp3B->hd));_tmp3F.f1=_tmp159;});({struct Cyc_String_pa_PrintArg_struct _tmp3E;_tmp3E.tag=0;({struct _dyneither_ptr _tmp15A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)_tmp128->hd)->topt)));_tmp3E.f1=_tmp15A;});({void*_tmp3C[2]={& _tmp3E,& _tmp3F};({unsigned int _tmp15C=s0->loc;struct _dyneither_ptr _tmp15B=({const char*_tmp3D="fallthru argument has type %s but pattern variable has type %s";_tag_dyneither(_tmp3D,sizeof(char),63);});Cyc_Tcutil_terr(_tmp15C,_tmp15B,_tag_dyneither(_tmp3C,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}
# 214
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((void*)_tmp3B->hd) && !Cyc_Tcutil_is_noalias_path((struct Cyc_Absyn_Exp*)_tmp128->hd))
({void*_tmp40=0;({unsigned int _tmp15E=((struct Cyc_Absyn_Exp*)_tmp128->hd)->loc;struct _dyneither_ptr _tmp15D=({const char*_tmp41="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp41,sizeof(char),49);});Cyc_Tcutil_terr(_tmp15E,_tmp15D,_tag_dyneither(_tmp40,sizeof(void*),0));});});}
# 217
if(_tmp128 != 0)
({void*_tmp42=0;({unsigned int _tmp160=s0->loc;struct _dyneither_ptr _tmp15F=({const char*_tmp43="too many arguments in explicit fallthru";_tag_dyneither(_tmp43,sizeof(char),40);});Cyc_Tcutil_terr(_tmp160,_tmp15F,_tag_dyneither(_tmp42,sizeof(void*),0));});});
if(_tmp3B != 0)
({void*_tmp44=0;({unsigned int _tmp162=s0->loc;struct _dyneither_ptr _tmp161=({const char*_tmp45="too few arguments in explicit fallthru";_tag_dyneither(_tmp45,sizeof(char),39);});Cyc_Tcutil_terr(_tmp162,_tmp161,_tag_dyneither(_tmp44,sizeof(void*),0));});});
return;};}case 13: _LL23: _tmp12A=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp129=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2;_LL24:
# 227
({struct Cyc_Tcenv_Tenv*_tmp16A=({unsigned int _tmp169=s0->loc;struct Cyc_Tcenv_Tenv*_tmp168=te;Cyc_Tcenv_new_named_block(_tmp169,_tmp168,({struct Cyc_Absyn_Tvar*_tmp46=_cycalloc(sizeof(*_tmp46));({struct _dyneither_ptr*_tmp165=({struct _dyneither_ptr*_tmp47=_cycalloc(sizeof(*_tmp47));({struct _dyneither_ptr _tmp164=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4A;_tmp4A.tag=0;_tmp4A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp12A);({void*_tmp48[1]={& _tmp4A};({struct _dyneither_ptr _tmp163=({const char*_tmp49="`%s";_tag_dyneither(_tmp49,sizeof(char),4);});Cyc_aprintf(_tmp163,_tag_dyneither(_tmp48,sizeof(void*),1));});});});_tmp47[0]=_tmp164;});_tmp47;});_tmp46->name=_tmp165;});({int _tmp166=
# 229
Cyc_Tcutil_new_tvar_id();_tmp46->identity=_tmp166;});({void*_tmp167=
Cyc_Tcutil_kind_to_bound(& Cyc_Tcutil_rk);_tmp46->kind=_tmp167;});_tmp46;}));});
# 227
Cyc_Tcstmt_tcStmt(_tmp16A,_tmp129,0);});
# 231
return;case 10: _LL25: _tmp12D=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp12C=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2;_tmp12B=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp1C)->f3;_LL26: {
# 237
struct Cyc_Absyn_Exp*_tmp4B=*_tmp12D;
if(!Cyc_Tcutil_is_var_exp(_tmp4B)){
struct Cyc_Absyn_Vardecl*_tmp4C=({struct _tuple2*_tmp16B=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0,_tmp16B,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmp4D=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmp4E=({struct Cyc_Absyn_Pat*_tmp170=({void*_tmp16E=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp52=_cycalloc(sizeof(*_tmp52));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp16D=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp53;_tmp53.tag=1;_tmp53.f1=_tmp4C;({struct Cyc_Absyn_Pat*_tmp16C=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmp4B->loc);_tmp53.f2=_tmp16C;});_tmp53;});_tmp52[0]=_tmp16D;});_tmp52;});Cyc_Absyn_new_pat(_tmp16E,_tmp4B->loc);});struct Cyc_Absyn_Exp*_tmp16F=_tmp4B;Cyc_Absyn_let_decl(_tmp170,_tmp16F,s0->loc);});
# 244
struct Cyc_Absyn_Stmt*_tmp4F=Cyc_Absyn_decl_stmt(_tmp4E,_tmp4D,s0->loc);
# 246
s0->r=_tmp4F->r;
({struct Cyc_Absyn_Exp*_tmp173=({void*_tmp172=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp50=_cycalloc(sizeof(*_tmp50));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp171=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp51;_tmp51.tag=4;_tmp51.f1=_tmp4C;_tmp51;});_tmp50[0]=_tmp171;});_tmp50;});Cyc_Absyn_varb_exp(_tmp172,_tmp4B->loc);});*_tmp12D=_tmp173;});
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}
# 252
Cyc_Tcexp_tcExp(te,0,_tmp4B);{
void*_tmp54=(void*)_check_null(_tmp4B->topt);
# 257
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp54) && !Cyc_Tcutil_is_noalias_path(_tmp4B))
({void*_tmp55=0;({unsigned int _tmp175=_tmp4B->loc;struct _dyneither_ptr _tmp174=({const char*_tmp56="Cannot consume non-unique paths; do swap instead";_tag_dyneither(_tmp56,sizeof(char),49);});Cyc_Tcutil_terr(_tmp175,_tmp174,_tag_dyneither(_tmp55,sizeof(void*),0));});});{
struct Cyc_Tcenv_Tenv*_tmp57=Cyc_Tcenv_clear_fallthru(te);
{struct Cyc_List_List*scs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp12C);for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Pat*_tmp58=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
struct Cyc_Tcpat_TcPatResult _tmp59=Cyc_Tcpat_tcPat(_tmp57,_tmp58,& _tmp54,_tmp4B);
struct Cyc_List_List*_tmp5A=_tmp59.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmp59.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp58->topt),_tmp54)){
({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0;({struct _dyneither_ptr _tmp176=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp58->topt)));_tmp5E.f1=_tmp176;});({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0;({struct _dyneither_ptr _tmp177=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmp54));_tmp5D.f1=_tmp177;});({void*_tmp5B[2]={& _tmp5D,& _tmp5E};({unsigned int _tmp179=((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc;struct _dyneither_ptr _tmp178=({const char*_tmp5C="switch on type %s, but case expects type %s";_tag_dyneither(_tmp5C,sizeof(char),44);});Cyc_Tcutil_terr(_tmp179,_tmp178,_tag_dyneither(_tmp5B,sizeof(void*),2));});});});});
Cyc_Tcutil_explain_failure();}else{
# 271
Cyc_Tcpat_check_pat_regions(_tmp57,_tmp58,_tmp59.patvars);}
({struct Cyc_Core_Opt*_tmp17A=({struct Cyc_Core_Opt*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->v=_tmp59.patvars;_tmp5F;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_tmp17A;});
# 274
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tmp57,_tmp59,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);
# 276
if(_tmp59.tvars_and_bounds_opt != 0  && (*_tmp59.tvars_and_bounds_opt).f2 != 0)
# 278
({struct Cyc_Tcenv_Tenv*_tmp17B=Cyc_Tcenv_clear_fallthru(_tmp57);_tmp57=_tmp17B;});else{
# 280
struct Cyc_List_List*_tmp60=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp59.patvars)).f1);
({struct Cyc_List_List*_tmp17C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp60);_tmp60=_tmp17C;});
({struct Cyc_Tcenv_Tenv*_tmp17D=Cyc_Tcenv_set_fallthru(_tmp57,_tmp5A,_tmp60,(struct Cyc_Absyn_Switch_clause*)scs->hd);_tmp57=_tmp17D;});}}}
# 285
Cyc_Tcpat_check_switch_exhaustive(s0->loc,_tmp57,_tmp12C,_tmp12B);
return;};};}case 15: _LL27: _tmp130=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp12F=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2;_tmp12E=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp1C)->f3;_LL28:
# 293
({void*_tmp182=({void*_tmp181=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp61=_cycalloc(sizeof(*_tmp61));({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp180=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp62;_tmp62.tag=2;({struct Cyc_Absyn_Stmt*_tmp17E=Cyc_Absyn_new_stmt(_tmp130->r,_tmp130->loc);_tmp62.f1=_tmp17E;});({struct Cyc_Absyn_Stmt*_tmp17F=Cyc_Absyn_skip_stmt(_tmp130->loc);_tmp62.f2=_tmp17F;});_tmp62;});_tmp61[0]=_tmp180;});_tmp61;});Cyc_Absyn_new_stmt(_tmp181,_tmp130->loc);})->r;_tmp130->r=_tmp182;});
# 295
Cyc_Tcstmt_tcStmt(te,_tmp130,1);{
# 298
struct _tuple2*def_v=({struct _tuple2*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmp183=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"exn",sizeof(char),4);_tmp7A->f2=_tmp183;});_tmp7A;});
struct Cyc_Absyn_Vardecl*_tmp63=Cyc_Absyn_new_vardecl(0,def_v,(void*)& Cyc_Absyn_VoidType_val,0);
struct Cyc_Absyn_Pat*_tmp64=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp78=_cycalloc(sizeof(*_tmp78));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp185=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp79;_tmp79.tag=1;_tmp79.f1=_tmp63;({struct Cyc_Absyn_Pat*_tmp184=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0);_tmp79.f2=_tmp184;});_tmp79;});_tmp78[0]=_tmp185;});_tmp78;}),0);
struct Cyc_Absyn_Stmt*_tmp65=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_tmp74=_cycalloc(sizeof(*_tmp74));({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp188=({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct _tmp75;_tmp75.tag=10;({struct Cyc_Absyn_Exp*_tmp187=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp76=_cycalloc(sizeof(*_tmp76));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp186=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp77;_tmp77.tag=4;_tmp77.f1=_tmp63;_tmp77;});_tmp76[0]=_tmp186;});_tmp76;}),0);_tmp75.f1=_tmp187;});_tmp75.f2=1;_tmp75;});_tmp74[0]=_tmp188;});_tmp74;}),0),0);
struct Cyc_Absyn_Switch_clause*_tmp66=({struct Cyc_Absyn_Switch_clause*_tmp73=_cycalloc(sizeof(*_tmp73));_tmp73->pattern=_tmp64;_tmp73->pat_vars=0;_tmp73->where_clause=0;_tmp73->body=_tmp65;_tmp73->loc=0;_tmp73;});
({struct Cyc_List_List*_tmp18A=({struct Cyc_List_List*_tmp189=*_tmp12F;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp189,({struct Cyc_Absyn_Switch_clause*_tmp67[1];_tmp67[0]=_tmp66;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp67,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));});*_tmp12F=_tmp18A;});{
# 307
struct Cyc_Tcenv_Tenv*_tmp68=Cyc_Tcenv_clear_fallthru(te);
{struct Cyc_List_List*_tmp69=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(*_tmp12F);for(0;_tmp69 != 0;_tmp69=_tmp69->tl){
({struct Cyc_Tcenv_Tenv*_tmp18B=Cyc_Tcenv_copy_tenv(_tmp68);_tmp68=_tmp18B;});{
struct Cyc_Absyn_Pat*_tmp6A=((struct Cyc_Absyn_Switch_clause*)_tmp69->hd)->pattern;
void*_tmp6B=Cyc_Absyn_exn_typ();
struct Cyc_Tcpat_TcPatResult _tmp6C=Cyc_Tcpat_tcPat(_tmp68,_tmp6A,& _tmp6B,0);
# 314
struct Cyc_List_List*_tmp6D=_tmp6C.tvars_and_bounds_opt == 0?0:
((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst,(*_tmp6C.tvars_and_bounds_opt).f1);
if(!Cyc_Tcutil_unify((void*)_check_null(_tmp6A->topt),_tmp6B))
({struct Cyc_String_pa_PrintArg_struct _tmp70;_tmp70.tag=0;({struct _dyneither_ptr _tmp18C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp6A->topt)));_tmp70.f1=_tmp18C;});({void*_tmp6E[1]={& _tmp70};({unsigned int _tmp18E=((struct Cyc_Absyn_Switch_clause*)_tmp69->hd)->loc;struct _dyneither_ptr _tmp18D=({const char*_tmp6F="expected datatype exn@ but found %s";_tag_dyneither(_tmp6F,sizeof(char),36);});Cyc_Tcutil_terr(_tmp18E,_tmp18D,_tag_dyneither(_tmp6E,sizeof(void*),1));});});});else{
# 320
Cyc_Tcpat_check_pat_regions(_tmp68,_tmp6A,_tmp6C.patvars);}
({struct Cyc_Core_Opt*_tmp18F=({struct Cyc_Core_Opt*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->v=_tmp6C.patvars;_tmp71;});((struct Cyc_Absyn_Switch_clause*)_tmp69->hd)->pat_vars=_tmp18F;});
# 323
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)_tmp69->hd)->loc,_tmp68,_tmp6C,((struct Cyc_Absyn_Switch_clause*)_tmp69->hd)->body,((struct Cyc_Absyn_Switch_clause*)_tmp69->hd)->where_clause,1);
# 325
if(_tmp6C.tvars_and_bounds_opt != 0  && (*_tmp6C.tvars_and_bounds_opt).f2 != 0)
# 327
({struct Cyc_Tcenv_Tenv*_tmp190=Cyc_Tcenv_clear_fallthru(_tmp68);_tmp68=_tmp190;});else{
# 329
struct Cyc_List_List*_tmp72=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple1(*)(struct Cyc_List_List*x))Cyc_List_split)(_tmp6C.patvars)).f1);
({struct Cyc_List_List*_tmp191=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp72);_tmp72=_tmp191;});
({struct Cyc_Tcenv_Tenv*_tmp192=Cyc_Tcenv_set_fallthru(_tmp68,_tmp6D,_tmp72,(struct Cyc_Absyn_Switch_clause*)_tmp69->hd);_tmp68=_tmp192;});}};}}
# 334
Cyc_Tcpat_check_catch_overlap(s0->loc,_tmp68,*_tmp12F,_tmp12E);
return;};};default: _LL29: _tmp132=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1C)->f1;_tmp131=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp1C)->f2;_LL2A: {
# 338
struct _dyneither_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_tmp7C=new_block?Cyc_Tcenv_new_block(s0->loc,te): Cyc_Tcenv_copy_tenv(te);
{void*_tmp7D=_tmp132->r;void*_tmp7E=_tmp7D;struct _tuple2*_tmp110;struct Cyc_List_List*_tmp10F;struct _dyneither_ptr*_tmp10E;struct Cyc_List_List*_tmp10D;struct Cyc_Absyn_Fndecl*_tmp10C;struct Cyc_Absyn_Tvar*_tmp10B;struct Cyc_Absyn_Vardecl*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_Pat*_tmp107;struct Cyc_Core_Opt**_tmp106;struct Cyc_Absyn_Exp**_tmp105;void**_tmp104;struct Cyc_Absyn_Vardecl*_tmp103;switch(*((int*)_tmp7E)){case 0: _LL3D: _tmp103=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp7E)->f1;_LL3E: {
# 342
struct Cyc_Absyn_Vardecl*_tmp7F=_tmp103;enum Cyc_Absyn_Scope _tmpB2;union Cyc_Absyn_Nmspace _tmpB1;struct _dyneither_ptr*_tmpB0;struct Cyc_Absyn_Tqual _tmpAF;void*_tmpAE;struct Cyc_Absyn_Exp*_tmpAD;void**_tmpAC;struct Cyc_List_List*_tmpAB;_LL5C: _tmpB2=_tmp7F->sc;_tmpB1=(_tmp7F->name)->f1;_tmpB0=(_tmp7F->name)->f2;_tmpAF=_tmp7F->tq;_tmpAE=_tmp7F->type;_tmpAD=_tmp7F->initializer;_tmpAC=(void**)& _tmp7F->rgn;_tmpAB=_tmp7F->attributes;_LL5D:;{
void*_tmp80=Cyc_Tcenv_curr_rgn(_tmp7C);
int is_local;
{enum Cyc_Absyn_Scope _tmp81=_tmpB2;switch(_tmp81){case Cyc_Absyn_Static: _LL5F: _LL60:
 goto _LL62;case Cyc_Absyn_Extern: _LL61: _LL62:
 goto _LL64;case Cyc_Absyn_ExternC: _LL63: _LL64:
# 349
 _tmp103->escapes=1;
is_local=0;goto _LL5E;case Cyc_Absyn_Abstract: _LL65: _LL66:
# 352
({void*_tmp82=0;({unsigned int _tmp194=_tmp132->loc;struct _dyneither_ptr _tmp193=({const char*_tmp83="bad abstract scope for local variable";_tag_dyneither(_tmp83,sizeof(char),38);});Cyc_Tcutil_terr(_tmp194,_tmp193,_tag_dyneither(_tmp82,sizeof(void*),0));});});
goto _LL68;case Cyc_Absyn_Register: _LL67: _LL68:
 goto _LL6A;default: _LL69: _LL6A:
 is_local=1;goto _LL5E;}_LL5E:;}
# 358
*_tmpAC=is_local?_tmp80:(void*)& Cyc_Absyn_HeapRgn_val;
# 360
{void*_tmp84=Cyc_Tcutil_compress(_tmpAE);void*_tmp85=_tmp84;void*_tmp91;struct Cyc_Absyn_Tqual _tmp90;union Cyc_Absyn_Constraint*_tmp8F;unsigned int _tmp8E;switch(*((int*)_tmp85)){case 9: _LL6C: if(is_local){_LL6D:
# 363
 _tmp103->escapes=1;
_tmpB2=Cyc_Absyn_Extern;
is_local=0;
goto _LL6B;}else{goto _LL70;}case 8: if((((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp85)->f1).num_elts == 0){_LL6E: _tmp91=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp85)->f1).elt_type;_tmp90=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp85)->f1).tq;_tmp8F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp85)->f1).zero_term;_tmp8E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp85)->f1).zt_loc;if(_tmp103->initializer != 0){_LL6F:
# 368
{void*_tmp86=((struct Cyc_Absyn_Exp*)_check_null(_tmp103->initializer))->r;void*_tmp87=_tmp86;struct Cyc_List_List*_tmp8D;struct Cyc_List_List*_tmp8C;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;struct _dyneither_ptr _tmp89;struct _dyneither_ptr _tmp88;switch(*((int*)_tmp87)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp87)->f1).Wstring_c).tag){case 8: _LL73: _tmp88=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp87)->f1).String_c).val;_LL74:
# 370
({void*_tmp19A=({void*_tmp199=({void*_tmp198=_tmp91;struct Cyc_Absyn_Tqual _tmp197=_tmp90;struct Cyc_Absyn_Exp*_tmp196=Cyc_Absyn_uint_exp(_get_dyneither_size(_tmp88,sizeof(char)),0);union Cyc_Absyn_Constraint*_tmp195=_tmp8F;Cyc_Absyn_array_typ(_tmp198,_tmp197,_tmp196,_tmp195,_tmp8E);});_tmp103->type=_tmp199;});_tmpAE=_tmp19A;});
goto _LL72;case 9: _LL75: _tmp89=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp87)->f1).Wstring_c).val;_LL76:
# 373
({void*_tmp1A0=({void*_tmp19F=({void*_tmp19E=_tmp91;struct Cyc_Absyn_Tqual _tmp19D=_tmp90;struct Cyc_Absyn_Exp*_tmp19C=Cyc_Absyn_uint_exp(1,0);union Cyc_Absyn_Constraint*_tmp19B=_tmp8F;Cyc_Absyn_array_typ(_tmp19E,_tmp19D,_tmp19C,_tmp19B,_tmp8E);});_tmp103->type=_tmp19F;});_tmpAE=_tmp1A0;});
goto _LL72;default: goto _LL7F;}case 26: _LL77: _tmp8A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp87)->f2;_LL78:
 _tmp8B=_tmp8A;goto _LL7A;case 27: _LL79: _tmp8B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp87)->f1;_LL7A:
# 378
({void*_tmp1A2=({void*_tmp1A1=Cyc_Absyn_array_typ(_tmp91,_tmp90,_tmp8B,_tmp8F,_tmp8E);_tmp103->type=_tmp1A1;});_tmpAE=_tmp1A2;});
goto _LL72;case 35: _LL7B: _tmp8C=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp87)->f2;_LL7C:
 _tmp8D=_tmp8C;goto _LL7E;case 25: _LL7D: _tmp8D=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp87)->f1;_LL7E:
# 382
({void*_tmp1A8=({void*_tmp1A7=({void*_tmp1A6=_tmp91;struct Cyc_Absyn_Tqual _tmp1A5=_tmp90;struct Cyc_Absyn_Exp*_tmp1A4=Cyc_Absyn_uint_exp((unsigned int)((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8D),0);union Cyc_Absyn_Constraint*_tmp1A3=_tmp8F;Cyc_Absyn_array_typ(_tmp1A6,_tmp1A5,_tmp1A4,_tmp1A3,_tmp8E);});_tmp103->type=_tmp1A7;});_tmpAE=_tmp1A8;});
goto _LL72;default: _LL7F: _LL80:
 goto _LL72;}_LL72:;}
# 386
goto _LL6B;}else{goto _LL70;}}else{goto _LL70;}default: _LL70: _LL71:
 goto _LL6B;}_LL6B:;}{
# 393
struct Cyc_List_List*_tmp92=!is_local?0: Cyc_Tcenv_lookup_type_vars(_tmp7C);
int _tmp93=!is_local?0: 1;
Cyc_Tcutil_check_type(s0->loc,_tmp7C,_tmp92,& Cyc_Tcutil_tmk,_tmp93,1,_tmpAE);
({int _tmp1A9=Cyc_Tcutil_extract_const_from_typedef(s0->loc,(_tmp103->tq).print_const,_tmpAE);(_tmp103->tq).real_const=_tmp1A9;});{
# 398
struct Cyc_Tcenv_Tenv*_tmp94=_tmp7C;
# 400
if(_tmpB2 == Cyc_Absyn_Extern  || _tmpB2 == Cyc_Absyn_ExternC)
({void*_tmp95=0;({unsigned int _tmp1AB=_tmp132->loc;struct _dyneither_ptr _tmp1AA=({const char*_tmp96="extern declarations are not yet supported within functions";_tag_dyneither(_tmp96,sizeof(char),59);});Cyc_Tcutil_terr(_tmp1AB,_tmp1AA,_tag_dyneither(_tmp95,sizeof(void*),0));});});
if(_tmpAD != 0){
Cyc_Tcexp_tcExpInitializer(_tmp94,& _tmpAE,_tmpAD);
# 405
if(!is_local  && !Cyc_Tcutil_is_const_exp(_tmpAD))
({void*_tmp97=0;({unsigned int _tmp1AD=_tmp132->loc;struct _dyneither_ptr _tmp1AC=({const char*_tmp98="initializer for static variable needs to be a constant expression";_tag_dyneither(_tmp98,sizeof(char),66);});Cyc_Tcutil_terr(_tmp1AD,_tmp1AC,_tag_dyneither(_tmp97,sizeof(void*),0));});});
if(!Cyc_Tcutil_coerce_assign(_tmp94,_tmpAD,_tmpAE)){
struct _dyneither_ptr _tmp99=*_tmpB0;
struct _dyneither_ptr _tmp9A=Cyc_Absynpp_typ2string(_tmpAE);
struct _dyneither_ptr _tmp9B=Cyc_Absynpp_typ2string((void*)_check_null(_tmpAD->topt));
if(((_get_dyneither_size(_tmp99,sizeof(char))+ _get_dyneither_size(_tmp9A,sizeof(char)))+ _get_dyneither_size(_tmp9B,sizeof(char)))+ 50 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpA0;_tmpA0.tag=0;_tmpA0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9B);({struct Cyc_String_pa_PrintArg_struct _tmp9F;_tmp9F.tag=0;_tmp9F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9A);({struct Cyc_String_pa_PrintArg_struct _tmp9E;_tmp9E.tag=0;_tmp9E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({void*_tmp9C[3]={& _tmp9E,& _tmp9F,& _tmpA0};({unsigned int _tmp1AF=_tmp132->loc;struct _dyneither_ptr _tmp1AE=({const char*_tmp9D="%s was declared with type %s but initialized with type %s.";_tag_dyneither(_tmp9D,sizeof(char),59);});Cyc_Tcutil_terr(_tmp1AF,_tmp1AE,_tag_dyneither(_tmp9C,sizeof(void*),3));});});});});});else{
# 414
if((_get_dyneither_size(_tmp99,sizeof(char))+ _get_dyneither_size(_tmp9A,sizeof(char)))+ 25 < 80  && 
_get_dyneither_size(_tmp9B,sizeof(char))+ 25 < 80)
({struct Cyc_String_pa_PrintArg_struct _tmpA5;_tmpA5.tag=0;_tmpA5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9B);({struct Cyc_String_pa_PrintArg_struct _tmpA4;_tmpA4.tag=0;_tmpA4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9A);({struct Cyc_String_pa_PrintArg_struct _tmpA3;_tmpA3.tag=0;_tmpA3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({void*_tmpA1[3]={& _tmpA3,& _tmpA4,& _tmpA5};({unsigned int _tmp1B1=_tmp132->loc;struct _dyneither_ptr _tmp1B0=({const char*_tmpA2="%s was declared with type %s\n but initialized with type %s.";_tag_dyneither(_tmpA2,sizeof(char),60);});Cyc_Tcutil_terr(_tmp1B1,_tmp1B0,_tag_dyneither(_tmpA1,sizeof(void*),3));});});});});});else{
# 419
({struct Cyc_String_pa_PrintArg_struct _tmpAA;_tmpAA.tag=0;_tmpAA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9B);({struct Cyc_String_pa_PrintArg_struct _tmpA9;_tmpA9.tag=0;_tmpA9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp9A);({struct Cyc_String_pa_PrintArg_struct _tmpA8;_tmpA8.tag=0;_tmpA8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)_tmp99);({void*_tmpA6[3]={& _tmpA8,& _tmpA9,& _tmpAA};({unsigned int _tmp1B3=_tmp132->loc;struct _dyneither_ptr _tmp1B2=({const char*_tmpA7="%s declared with type \n%s\n but initialized with type \n%s.";_tag_dyneither(_tmpA7,sizeof(char),58);});Cyc_Tcutil_terr(_tmp1B3,_tmp1B2,_tag_dyneither(_tmpA6,sizeof(void*),3));});});});});});}}
# 421
Cyc_Tcutil_unify(_tmpAE,(void*)_check_null(_tmpAD->topt));
Cyc_Tcutil_explain_failure();}}
# 425
Cyc_Tcstmt_tcStmt(_tmp94,_tmp131,0);
return;};};};}case 2: _LL3F: _tmp107=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7E)->f1;_tmp106=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7E)->f2;_tmp105=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7E)->f3;_tmp104=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp7E)->f4;_LL40: {
# 433
struct Cyc_Absyn_Exp*_tmpB3=*_tmp105;
{void*_tmpB4=_tmp107->r;void*_tmpB5=_tmpB4;switch(*((int*)_tmpB5)){case 1: _LL82: _LL83:
 goto _LL85;case 2: _LL84: _LL85:
 goto _LL87;case 15: _LL86: _LL87:
 goto _LL81;default: _LL88: _LL89:
# 439
 if(!Cyc_Tcutil_is_var_exp(_tmpB3)){
struct Cyc_Absyn_Vardecl*_tmpB6=({struct _tuple2*_tmp1B4=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0,_tmp1B4,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Stmt*_tmpB7=Cyc_Absyn_new_stmt(s0->r,s0->loc);
struct Cyc_Absyn_Decl*_tmpB8=({struct Cyc_Absyn_Pat*_tmp1B9=({void*_tmp1B7=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmpBC=_cycalloc(sizeof(*_tmpBC));({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmp1B6=({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct _tmpBD;_tmpBD.tag=1;_tmpBD.f1=_tmpB6;({struct Cyc_Absyn_Pat*_tmp1B5=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,_tmpB3->loc);_tmpBD.f2=_tmp1B5;});_tmpBD;});_tmpBC[0]=_tmp1B6;});_tmpBC;});Cyc_Absyn_new_pat(_tmp1B7,_tmpB3->loc);});struct Cyc_Absyn_Exp*_tmp1B8=_tmpB3;Cyc_Absyn_let_decl(_tmp1B9,_tmp1B8,s0->loc);});
# 445
struct Cyc_Absyn_Stmt*_tmpB9=Cyc_Absyn_decl_stmt(_tmpB8,_tmpB7,s0->loc);
# 447
s0->r=_tmpB9->r;
({struct Cyc_Absyn_Exp*_tmp1BC=({void*_tmp1BB=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmpBA=_cycalloc(sizeof(*_tmpBA));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp1BA=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpBB;_tmpBB.tag=4;_tmpBB.f1=_tmpB6;_tmpBB;});_tmpBA[0]=_tmp1BA;});_tmpBA;});Cyc_Absyn_varb_exp(_tmp1BB,_tmpB3->loc);});*_tmp105=_tmp1BC;});
Cyc_Tcstmt_tcStmt(_tmp7C,s0,new_block);
return;}}_LL81:;}
# 453
Cyc_Tcexp_tcExpInitializer(_tmp7C,0,_tmpB3);{
# 455
void*pat_type=(void*)_check_null(_tmpB3->topt);
# 457
struct Cyc_Tcpat_TcPatResult _tmpBE=Cyc_Tcpat_tcPat(_tmp7C,_tmp107,& pat_type,_tmpB3);
({struct Cyc_Core_Opt*_tmp1BD=({struct Cyc_Core_Opt*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->v=_tmpBE.patvars;_tmpBF;});*_tmp106=_tmp1BD;});
if(!({void*_tmp1BE=(void*)_check_null(_tmp107->topt);Cyc_Tcutil_unify(_tmp1BE,(void*)_check_null(_tmpB3->topt));}) && !
Cyc_Tcutil_coerce_assign(_tmp7C,_tmpB3,(void*)_check_null(_tmp107->topt))){
({struct Cyc_String_pa_PrintArg_struct _tmpC3;_tmpC3.tag=0;({struct _dyneither_ptr _tmp1BF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmpB3->topt)));_tmpC3.f1=_tmp1BF;});({struct Cyc_String_pa_PrintArg_struct _tmpC2;_tmpC2.tag=0;({struct _dyneither_ptr _tmp1C0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp107->topt)));_tmpC2.f1=_tmp1C0;});({void*_tmpC0[2]={& _tmpC2,& _tmpC3};({unsigned int _tmp1C2=_tmp132->loc;struct _dyneither_ptr _tmp1C1=({const char*_tmpC1="pattern type %s does not match definition type %s";_tag_dyneither(_tmpC1,sizeof(char),50);});Cyc_Tcutil_terr(_tmp1C2,_tmp1C1,_tag_dyneither(_tmpC0,sizeof(void*),2));});});});});
({void*_tmp1C3=(void*)_check_null(_tmp107->topt);Cyc_Tcutil_unify(_tmp1C3,(void*)_check_null(_tmpB3->topt));});
Cyc_Tcutil_explain_failure();}else{
# 467
Cyc_Tcpat_check_pat_regions(_tmp7C,_tmp107,_tmpBE.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(_tmp107->loc,_tmp7C,_tmp107,_tmp104);
Cyc_Tcstmt_pattern_synth(s0->loc,_tmp7C,_tmpBE,_tmp131,0,0);
return;};}case 3: _LL41: _tmp108=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp7E)->f1;_LL42: {
# 473
void*_tmpC4=Cyc_Tcenv_curr_rgn(_tmp7C);
struct Cyc_Tcenv_Tenv*_tmpC5=_tmp7C;
for(0;_tmp108 != 0;_tmp108=_tmp108->tl){
struct Cyc_Absyn_Vardecl*_tmpC6=(struct Cyc_Absyn_Vardecl*)_tmp108->hd;
struct Cyc_Absyn_Vardecl*_tmpC7=_tmpC6;union Cyc_Absyn_Nmspace _tmpCA;void*_tmpC9;void**_tmpC8;_LL8B: _tmpCA=(_tmpC7->name)->f1;_tmpC9=_tmpC7->type;_tmpC8=(void**)& _tmpC7->rgn;_LL8C:;
*_tmpC8=_tmpC4;
({unsigned int _tmp1C6=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1C5=_tmpC5;struct Cyc_List_List*_tmp1C4=Cyc_Tcenv_lookup_type_vars(_tmpC5);Cyc_Tcutil_check_type(_tmp1C6,_tmp1C5,_tmp1C4,& Cyc_Tcutil_tmk,1,1,_tmpC9);});}
# 482
Cyc_Tcstmt_tcStmt(_tmpC5,_tmp131,0);
return;}case 4: _LL43: _tmp10B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7E)->f1;_tmp10A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7E)->f2;_tmp109=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp7E)->f3;_LL44:
# 488
({void*_tmp1CB=({void*_tmp1CA=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmp1C9=({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct _tmpCC;_tmpCC.tag=2;({struct Cyc_Absyn_Stmt*_tmp1C7=Cyc_Absyn_new_stmt(_tmp131->r,_tmp131->loc);_tmpCC.f1=_tmp1C7;});({struct Cyc_Absyn_Stmt*_tmp1C8=Cyc_Absyn_skip_stmt(_tmp131->loc);_tmpCC.f2=_tmp1C8;});_tmpCC;});_tmpCB[0]=_tmp1C9;});_tmpCB;});Cyc_Absyn_new_stmt(_tmp1CA,_tmp131->loc);})->r;_tmp131->r=_tmp1CB;});{
# 490
struct Cyc_Tcenv_Tenv*_tmpCD=_tmp7C;
struct Cyc_Absyn_Vardecl*_tmpCE=_tmp10A;void**_tmpEF;void**_tmpEE;_LL8E: _tmpEF=(void**)& _tmpCE->type;_tmpEE=(void**)& _tmpCE->rgn;_LL8F:;{
void*_tmpCF=Cyc_Tcenv_curr_rgn(_tmp7C);
*_tmpEE=_tmpCF;{
void*rgn_typ;
if((unsigned int)_tmp109){
# 498
struct _tuple2*drname=({struct _tuple2*_tmpE2=_cycalloc(sizeof(*_tmpE2));({union Cyc_Absyn_Nmspace _tmp1CE=({union Cyc_Absyn_Nmspace _tmpE4;({struct Cyc_List_List*_tmp1CD=({struct _dyneither_ptr*_tmpE5[1];({struct _dyneither_ptr*_tmp1CC=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"Core",sizeof(char),5);_tmpE5[0]=_tmp1CC;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE5,sizeof(struct _dyneither_ptr*),1));});(_tmpE4.Abs_n).val=_tmp1CD;});(_tmpE4.Abs_n).tag=2;_tmpE4;});_tmpE2->f1=_tmp1CE;});({struct _dyneither_ptr*_tmp1CF=
_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"DynamicRegion",sizeof(char),14);_tmpE2->f2=_tmp1CF;});_tmpE2;});
void*_tmpD0=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->v=& Cyc_Tcutil_trk;_tmpE1;}),0);
({void*_tmp1D0=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_tmpD1=_cycalloc(sizeof(*_tmpD1));_tmpD1->v=& Cyc_Tcutil_rk;_tmpD1;}),0);rgn_typ=_tmp1D0;});{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpD2=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp1D4=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpDE;_tmpDE.tag=11;({struct Cyc_Absyn_AggrInfo _tmp1D3=({struct Cyc_Absyn_AggrInfo _tmpDF;({union Cyc_Absyn_AggrInfoU _tmp1D1=
Cyc_Absyn_UnknownAggr(Cyc_Absyn_StructA,drname,0);_tmpDF.aggr_info=_tmp1D1;});({struct Cyc_List_List*_tmp1D2=({void*_tmpE0[1];_tmpE0[0]=rgn_typ;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmpE0,sizeof(void*),1));});_tmpDF.targs=_tmp1D2;});_tmpDF;});_tmpDE.f1=_tmp1D3;});_tmpDE;});_tmpDD[0]=_tmp1D4;});_tmpDD;});
# 505
void*_tmpD3=({void*_tmp1D7=(void*)_tmpD2;void*_tmp1D6=_tmpD0;struct Cyc_Absyn_Tqual _tmp1D5=
Cyc_Absyn_empty_tqual(0);
# 505
Cyc_Absyn_at_typ(_tmp1D7,_tmp1D6,_tmp1D5,Cyc_Absyn_false_conref);});
# 507
({unsigned int _tmp1DA=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1D9=_tmp7C;struct Cyc_List_List*_tmp1D8=Cyc_Tcenv_lookup_type_vars(_tmp7C);Cyc_Tcutil_check_type(_tmp1DA,_tmp1D9,_tmp1D8,& Cyc_Tcutil_tmk,1,0,_tmpD3);});{
void*_tmpD4=Cyc_Tcexp_tcExp(_tmp7C,& _tmpD3,_tmp109);
if(!Cyc_Tcutil_unify(_tmpD3,_tmpD4) && !
Cyc_Tcutil_coerce_assign(_tmp7C,_tmp109,_tmpD3))
({struct Cyc_String_pa_PrintArg_struct _tmpD8;_tmpD8.tag=0;({struct _dyneither_ptr _tmp1DB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmpD4));_tmpD8.f1=_tmp1DB;});({struct Cyc_String_pa_PrintArg_struct _tmpD7;_tmpD7.tag=0;({struct _dyneither_ptr _tmp1DC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(_tmpD3));_tmpD7.f1=_tmp1DC;});({void*_tmpD5[2]={& _tmpD7,& _tmpD8};({unsigned int _tmp1DE=s0->loc;struct _dyneither_ptr _tmp1DD=({const char*_tmpD6="expected %s but found %s";_tag_dyneither(_tmpD6,sizeof(char),25);});Cyc_Tcutil_terr(_tmp1DE,_tmp1DD,_tag_dyneither(_tmpD5,sizeof(void*),2));});});});});
if(!Cyc_Tcutil_unify(_tmpD0,(void*)& Cyc_Absyn_UniqueRgn_val) && !
Cyc_Tcutil_unify(_tmpD0,(void*)& Cyc_Absyn_RefCntRgn_val))
({void*_tmpD9=0;({unsigned int _tmp1E0=s0->loc;struct _dyneither_ptr _tmp1DF=({const char*_tmpDA="open is only allowed on unique or reference-counted keys";_tag_dyneither(_tmpDA,sizeof(char),57);});Cyc_Tcutil_terr(_tmp1E0,_tmp1DF,_tag_dyneither(_tmpD9,sizeof(void*),0));});});
({void*_tmp1E2=(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpDB=_cycalloc(sizeof(*_tmpDB));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp1E1=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpDC;_tmpDC.tag=15;_tmpDC.f1=rgn_typ;_tmpDC;});_tmpDB[0]=_tmp1E1;});_tmpDB;});*_tmpEF=_tmp1E2;});};};}else{
# 518
({void*_tmp1E4=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp1E3=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE8;_tmpE8.tag=2;_tmpE8.f1=_tmp10B;_tmpE8;});_tmpE7[0]=_tmp1E3;});_tmpE7;});rgn_typ=_tmp1E4;});
({struct Cyc_Tcenv_Tenv*_tmp1E7=({unsigned int _tmp1E6=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1E5=_tmpCD;Cyc_Tcenv_add_type_vars(_tmp1E6,_tmp1E5,({struct Cyc_List_List*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->hd=_tmp10B;_tmpE9->tl=0;_tmpE9;}));});_tmpCD=_tmp1E7;});}
# 521
({struct Cyc_Tcenv_Tenv*_tmp1E8=Cyc_Tcenv_add_region(_tmpCD,rgn_typ,1);_tmpCD=_tmp1E8;});
({unsigned int _tmp1EB=s0->loc;struct Cyc_Tcenv_Tenv*_tmp1EA=_tmp7C;struct Cyc_List_List*_tmp1E9=Cyc_Tcenv_lookup_type_vars(_tmpCD);Cyc_Tcutil_check_type(_tmp1EB,_tmp1EA,_tmp1E9,& Cyc_Tcutil_bk,1,0,*_tmpEF);});
if(!({void*_tmp1ED=*_tmpEF;Cyc_Tcutil_unify(_tmp1ED,(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmpEA=_cycalloc(sizeof(*_tmpEA));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp1EC=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpEB;_tmpEB.tag=15;_tmpEB.f1=rgn_typ;_tmpEB;});_tmpEA[0]=_tmp1EC;});_tmpEA;}));}))
({void*_tmpEC=0;({struct _dyneither_ptr _tmp1EE=({const char*_tmpED="region stmt: type of region handle is wrong!";_tag_dyneither(_tmpED,sizeof(char),45);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp1EE,_tag_dyneither(_tmpEC,sizeof(void*),0));});});
# 526
Cyc_Tcstmt_tcStmt(_tmpCD,_tmp131,0);
return;};};};case 1: _LL45: _tmp10C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp7E)->f1;_LL46: {
# 531
void*_tmpF0=Cyc_Tcenv_curr_rgn(_tmp7C);
if(_tmp10C->sc != Cyc_Absyn_Public)
({void*_tmpF1=0;({unsigned int _tmp1F0=_tmp132->loc;struct _dyneither_ptr _tmp1EF=({const char*_tmpF2="bad storage class for inner function";_tag_dyneither(_tmpF2,sizeof(char),37);});Cyc_Tcutil_terr(_tmp1F0,_tmp1EF,_tag_dyneither(_tmpF1,sizeof(void*),0));});});
Cyc_Tcutil_check_fndecl_valid_type(_tmp132->loc,_tmp7C,_tmp10C);{
void*t=Cyc_Tcutil_fndecl2typ(_tmp10C);
({struct Cyc_List_List*_tmp1F1=Cyc_Tcutil_transfer_fn_type_atts(t,_tmp10C->attributes);_tmp10C->attributes=_tmp1F1;});
# 538
{struct Cyc_List_List*atts=_tmp10C->attributes;for(0;(unsigned int)atts;atts=atts->tl){
void*_tmpF3=(void*)atts->hd;void*_tmpF4=_tmpF3;switch(*((int*)_tmpF4)){case 7: _LL91: _LL92:
 goto _LL94;case 6: _LL93: _LL94:
({struct Cyc_String_pa_PrintArg_struct _tmpF7;_tmpF7.tag=0;({struct _dyneither_ptr _tmp1F2=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));_tmpF7.f1=_tmp1F2;});({void*_tmpF5[1]={& _tmpF7};({unsigned int _tmp1F4=_tmp132->loc;struct _dyneither_ptr _tmp1F3=({const char*_tmpF6="bad attribute %s for function";_tag_dyneither(_tmpF6,sizeof(char),30);});Cyc_Tcutil_terr(_tmp1F4,_tmp1F3,_tag_dyneither(_tmpF5,sizeof(void*),1));});});});
goto _LL90;default: _LL95: _LL96:
 goto _LL90;}_LL90:;}}{
# 546
struct Cyc_Absyn_Vardecl*vd=({struct Cyc_Absyn_Vardecl*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->sc=_tmp10C->sc;_tmpF8->name=_tmp10C->name;_tmpF8->varloc=0;({struct Cyc_Absyn_Tqual _tmp1F5=Cyc_Absyn_const_tqual(0);_tmpF8->tq=_tmp1F5;});({void*_tmp1F9=({
void*_tmp1F8=t;void*_tmp1F7=_tmpF0;struct Cyc_Absyn_Tqual _tmp1F6=Cyc_Absyn_empty_tqual(0);Cyc_Absyn_at_typ(_tmp1F8,_tmp1F7,_tmp1F6,Cyc_Absyn_false_conref);});_tmpF8->type=_tmp1F9;});_tmpF8->initializer=0;_tmpF8->rgn=_tmpF0;_tmpF8->attributes=0;_tmpF8->escapes=0;_tmpF8;});
# 550
_tmp10C->fn_vardecl=vd;{
# 552
struct Cyc_Tcenv_Fenv*old_fenv=(struct Cyc_Tcenv_Fenv*)_check_null(_tmp7C->le);
({struct Cyc_Tcenv_Fenv*_tmp1FA=Cyc_Tcenv_nested_fenv(_tmp132->loc,old_fenv,_tmp10C);_tmp7C->le=_tmp1FA;});
Cyc_Tcstmt_tcStmt(_tmp7C,_tmp10C->body,0);
Cyc_Tcenv_check_delayed_effects(_tmp7C);
Cyc_Tcenv_check_delayed_constraints(_tmp7C);
_tmp7C->le=old_fenv;
Cyc_Tcstmt_tcStmt(_tmp7C,_tmp131,0);
return;};};};}case 9: _LL47: _tmp10E=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp7E)->f1;_tmp10D=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp7E)->f2;_LL48:
({struct _dyneither_ptr _tmp1FB=({const char*_tmpF9="namespace";_tag_dyneither(_tmpF9,sizeof(char),10);});unimp_msg_part=_tmp1FB;});goto _LL3C;case 10: _LL49: _tmp110=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp7E)->f1;_tmp10F=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp7E)->f2;_LL4A:
({struct _dyneither_ptr _tmp1FC=({const char*_tmpFA="using";_tag_dyneither(_tmpFA,sizeof(char),6);});unimp_msg_part=_tmp1FC;});goto _LL3C;case 5: _LL4B: _LL4C:
({struct _dyneither_ptr _tmp1FD=({const char*_tmpFB="type";_tag_dyneither(_tmpFB,sizeof(char),5);});unimp_msg_part=_tmp1FD;});goto _LL3C;case 6: _LL4D: _LL4E:
({struct _dyneither_ptr _tmp1FE=({const char*_tmpFC="datatype";_tag_dyneither(_tmpFC,sizeof(char),9);});unimp_msg_part=_tmp1FE;});goto _LL3C;case 7: _LL4F: _LL50:
({struct _dyneither_ptr _tmp1FF=({const char*_tmpFD="enum";_tag_dyneither(_tmpFD,sizeof(char),5);});unimp_msg_part=_tmp1FF;});goto _LL3C;case 8: _LL51: _LL52:
({struct _dyneither_ptr _tmp200=({const char*_tmpFE="typedef";_tag_dyneither(_tmpFE,sizeof(char),8);});unimp_msg_part=_tmp200;});goto _LL3C;case 11: _LL53: _LL54:
({struct _dyneither_ptr _tmp201=({const char*_tmpFF="extern \"C\"";_tag_dyneither(_tmpFF,sizeof(char),11);});unimp_msg_part=_tmp201;});goto _LL3C;case 12: _LL55: _LL56:
# 568
({struct _dyneither_ptr _tmp202=({const char*_tmp100="extern \"C include\"";_tag_dyneither(_tmp100,sizeof(char),19);});unimp_msg_part=_tmp202;});goto _LL3C;case 13: _LL57: _LL58:
({struct _dyneither_ptr _tmp203=({const char*_tmp101="__cyclone_port_on__";_tag_dyneither(_tmp101,sizeof(char),20);});unimp_msg_part=_tmp203;});goto _LL3C;default: _LL59: _LL5A:
({struct _dyneither_ptr _tmp204=({const char*_tmp102="__cyclone_port_off__";_tag_dyneither(_tmp102,sizeof(char),21);});unimp_msg_part=_tmp204;});goto _LL3C;}_LL3C:;}
# 572
(int)_throw(({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0;_tmp113.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)unimp_msg_part);({void*_tmp111[1]={& _tmp113};({struct _dyneither_ptr _tmp205=({const char*_tmp112="tcStmt: nested %s declarations unimplemented";_tag_dyneither(_tmp112,sizeof(char),45);});((void*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp205,_tag_dyneither(_tmp111,sizeof(void*),1));});});}));}}_LLA:;}
