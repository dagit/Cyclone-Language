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
typedef struct Cyc___cycFILE Cyc_FILE;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 68
typedef void*Cyc_parg_t;
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 127
typedef void*Cyc_sarg_t;
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 79 "core.h"
typedef unsigned int Cyc_Core_sizeof_t;struct Cyc_Core_Opt{void*v;};
# 83
typedef struct Cyc_Core_Opt*Cyc_Core_opt_t;struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
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
# 54
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple1 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;
# 41 "pp.h"
typedef struct Cyc_PP_Ppstate*Cyc_PP_ppstate_t;struct Cyc_PP_Out;
# 43
typedef struct Cyc_PP_Out*Cyc_PP_out_t;struct Cyc_PP_Doc;
# 45
typedef struct Cyc_PP_Doc*Cyc_PP_doc_t;
# 33 "position.h"
typedef unsigned int Cyc_Position_seg_t;
# 37
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 42
typedef struct Cyc_Position_Error*Cyc_Position_error_t;
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 47
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;
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
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
# 101
typedef struct _tuple2*Cyc_Absyn_qvar_t;typedef struct _tuple2*Cyc_Absyn_qvar_opt_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_t;
typedef struct _tuple2*Cyc_Absyn_typedef_name_opt_t;
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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 314
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 333
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 345
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 366
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 371
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 426 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 442
typedef void*Cyc_Absyn_funcparams_t;
# 445
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 509
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 535
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 542
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 560
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 916 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 918
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 933
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
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
# 954
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 956
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 960
extern void*Cyc_Absyn_empty_effect;
# 970
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 979
extern void*Cyc_Absyn_bounds_one;
# 984
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1073
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1113
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1160
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1162
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1164
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1166
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1168
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1172
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1174
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU info);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _dyneither_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
struct _dyneither_ptr Cyc_Absynpp_kindbound2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 69
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple2*);
# 74
struct _dyneither_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 25 "warn.h"
void Cyc_Warn_vwarn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 27
void Cyc_Warn_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29
void Cyc_Warn_flush_warnings();
# 31
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 34
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct _tuple14{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple14 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 34 "iter.h"
typedef struct Cyc_Iter_Iter Cyc_Iter_iter_t;
# 37
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Set_Set;
# 40 "set.h"
typedef struct Cyc_Set_Set*Cyc_Set_set_t;extern char Cyc_Set_Absent[7];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Dict_T;
# 46 "dict.h"
typedef const struct Cyc_Dict_T*Cyc_Dict_tree;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 52
typedef struct Cyc_Dict_Dict Cyc_Dict_dict_t;extern char Cyc_Dict_Present[8];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);struct Cyc_RgnOrder_RgnPO;
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
# 100 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(struct _RegionHandle*,void*ret_type,struct Cyc_List_List*args);
# 104
void*Cyc_Tcenv_lookup_ordinary_global(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*,int is_use);
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 108
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 111
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*);
# 114
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 131
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*te);
# 179
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
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
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
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
int Cyc_Tcutil_is_arithmetic_type(void*);
# 68
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 70
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 72
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 76
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 78
extern int Cyc_Tcutil_warn_alias_coerce;
# 81
extern int Cyc_Tcutil_warn_region_coerce;
# 84
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 95
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 102
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 107
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 112
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 119
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 124
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 129
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 133
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 138
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 140
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 144
void Cyc_Tcutil_explain_failure();
# 146
int Cyc_Tcutil_unify(void*,void*);
# 148
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 151
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 156
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 160
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 164
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 167
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 171
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 173
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 177
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 183
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 210 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 212
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 220
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 223
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 230
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 232
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 234
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 236
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 238
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 244
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 246
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 248
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 252
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 258
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 263
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 266
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 271
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 276
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
# 280
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 288
void*Cyc_Tcutil_normalize_effect(void*e);
# 291
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 293
int Cyc_Tcutil_new_tvar_id();
# 295
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 298
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 300
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 303
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 307
int Cyc_Tcutil_bits_only(void*t);
# 310
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 313
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 316
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 320
int Cyc_Tcutil_supports_default(void*);
# 324
int Cyc_Tcutil_admits_zero(void*t);
# 327
int Cyc_Tcutil_is_noreturn(void*);
# 331
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 335
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 339
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 342
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 345
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 349
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 352
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 355
int Cyc_Tcutil_is_array(void*t);
# 359
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 362
int Cyc_Tcutil_zeroable_type(void*t);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 35
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 41 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0,Cyc_Cyclone_Vc_c  = 1};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 38 "relations.h"
typedef union Cyc_Relations_RelnOp Cyc_Relations_reln_op_t;
# 40
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0,Cyc_Relations_Rneq  = 1,Cyc_Relations_Rlte  = 2,Cyc_Relations_Rlt  = 3};
typedef enum Cyc_Relations_Relation Cyc_Relations_relation_t;struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 69
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 109
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);char Cyc_Tcutil_Unify[6]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 41 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 43
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 47
int Cyc_Tcutil_warn_region_coerce=0;
# 50
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 55
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 59
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 64
if(Cyc_strcmp(({const char*_tmp1="(qualifiers don't match)";_tag_dyneither(_tmp1,sizeof(char),25);}),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp2[1]={& _tmp4};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="  %s\n";_tag_dyneither(_tmp3,sizeof(char),6);}),_tag_dyneither(_tmp2,sizeof(void*),1));});});
return;}
# 69
if(Cyc_strcmp(({const char*_tmp5="(function effects do not match)";_tag_dyneither(_tmp5,sizeof(char),32);}),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}{
# 74
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp7[1]={& _tmp9};Cyc_fprintf(Cyc_stderr,({const char*_tmp8="  %s";_tag_dyneither(_tmp8,sizeof(char),5);}),_tag_dyneither(_tmp7,sizeof(void*),1));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpB="\n\t";_tag_dyneither(_tmpB,sizeof(char),3);}),_tag_dyneither(_tmpA,sizeof(void*),0));});
pos=8;}else{
# 83
({void*_tmpC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpD=" ";_tag_dyneither(_tmpD,sizeof(char),2);}),_tag_dyneither(_tmpC,sizeof(void*),0));});
++ pos;}
# 86
({void*_tmpE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpF="and ";_tag_dyneither(_tmpF,sizeof(char),5);}),_tag_dyneither(_tmpE,sizeof(void*),0));});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmp10=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp11="\n\t";_tag_dyneither(_tmp11,sizeof(char),3);}),_tag_dyneither(_tmp10,sizeof(void*),0));});
pos=8;}
# 92
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({void*_tmp12[1]={& _tmp14};Cyc_fprintf(Cyc_stderr,({const char*_tmp13="%s ";_tag_dyneither(_tmp13,sizeof(char),4);}),_tag_dyneither(_tmp12,sizeof(void*),1));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp15=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp16="\n\t";_tag_dyneither(_tmp16,sizeof(char),3);}),_tag_dyneither(_tmp15,sizeof(void*),0));});
pos=8;}
# 98
({void*_tmp17=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18="are not compatible. ";_tag_dyneither(_tmp18,sizeof(char),21);}),_tag_dyneither(_tmp17,sizeof(void*),0));});
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80)
({void*_tmp19=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1A="\n\t";_tag_dyneither(_tmp1A,sizeof(char),3);}),_tag_dyneither(_tmp19,sizeof(void*),0));});
# 104
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp1B[1]={& _tmp1D};Cyc_fprintf(Cyc_stderr,({const char*_tmp1C="%s";_tag_dyneither(_tmp1C,sizeof(char),3);}),_tag_dyneither(_tmp1B,sizeof(void*),1));});});}
# 106
({void*_tmp1E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1F="\n";_tag_dyneither(_tmp1F,sizeof(char),2);}),_tag_dyneither(_tmp1E,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);};}
# 110
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 112
Cyc_Position_post_error(Cyc_Position_mk_err(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 114
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 117
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp20[1]={& _tmp22};Cyc_fprintf(Cyc_stderr,({const char*_tmp21="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp21,sizeof(char),36);}),_tag_dyneither(_tmp20,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;_tmp24.f1=msg;_tmp24;});_tmp23;}));}
# 122
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 124
Cyc_Warn_vwarn(sg,fmt,ap);}
# 128
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 133
void*Cyc_Tcutil_compress(void*t){
void*_tmp25=t;void*_tmp2E;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;void**_tmp2B;void**_tmp2A;switch(*((int*)_tmp25)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp2A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25)->f2;_LL8: {
# 144
void*ta=(void*)_check_null(*_tmp2A);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp2A=t2;
return t2;}}case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp25)->f4 == 0){_LL3: _LL4:
# 136
 return t;}else{_LL5: _tmp2B=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp25)->f4;_LL6: {
# 138
void*ta=(void*)_check_null(*_tmp2B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp2B=t2;
return t2;}}case 18: _LL9: _tmp2C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp25)->f1;_LLA:
# 150
 Cyc_Evexp_eval_const_uint_exp(_tmp2C);{
void*_tmp26=_tmp2C->r;void*_tmp27=_tmp26;void*_tmp28;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp27)->tag == 38){_LL12: _tmp28=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp27)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp28);}else{_LL14: _LL15:
 return t;}_LL11:;};case 27: _LLB: _tmp2D=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp25)->f1;_LLC: {
# 156
void*_tmp29=_tmp2D->topt;
if(_tmp29 != 0)return _tmp29;else{
return t;}}case 26: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp25)->f2 != 0){_LLD: _tmp2E=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp25)->f2;_LLE:
# 160
 return Cyc_Tcutil_compress(_tmp2E);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 168
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 172
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp2F=cptr;union Cyc_Absyn_Constraint*_tmp31;void*_tmp30;switch((((union Cyc_Absyn_Constraint*)_tmp2F)->Eq_constr).tag){case 3: _LL17: _LL18:
 return Cyc_Absyn_empty_conref();case 1: _LL19: _tmp30=(void*)(_tmp2F->Eq_constr).val;_LL1A:
 return Cyc_Absyn_new_conref(_tmp30);default: _LL1B: _tmp31=(_tmp2F->Forward_constr).val;_LL1C:
 return Cyc_Tcutil_copy_conref(_tmp31);}_LL16:;}
# 179
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp32=Cyc_Absyn_compress_kb(kb);void*_tmp33=_tmp32;struct Cyc_Absyn_Kind*_tmp38;switch(*((int*)_tmp33)){case 1: _LL1E: _LL1F:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp35;_tmp35.tag=1;_tmp35.f1=0;_tmp35;});_tmp34;});case 2: _LL20: _tmp38=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp33)->f2;_LL21:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp37;_tmp37.tag=2;_tmp37.f1=0;_tmp37.f2=_tmp38;_tmp37;});_tmp36;});default: _LL22: _LL23:
 return kb;}_LL1D:;}
# 186
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 188
return({struct Cyc_Absyn_Tvar*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=tv->name;_tmp39->identity=- 1;_tmp39->kind=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp39;});}
# 190
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 192
struct _tuple10*_tmp3A=arg;struct _dyneither_ptr*_tmp3E;struct Cyc_Absyn_Tqual _tmp3D;void*_tmp3C;_LL25: _tmp3E=_tmp3A->f1;_tmp3D=_tmp3A->f2;_tmp3C=_tmp3A->f3;_LL26:;
return({struct _tuple10*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=_tmp3E;_tmp3B->f2=_tmp3D;_tmp3B->f3=Cyc_Tcutil_copy_type(_tmp3C);_tmp3B;});}
# 195
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp3F=arg;struct Cyc_Absyn_Tqual _tmp42;void*_tmp41;_LL28: _tmp42=_tmp3F->f1;_tmp41=_tmp3F->f2;_LL29:;
return({struct _tuple12*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->f1=_tmp42;_tmp40->f2=Cyc_Tcutil_copy_type(_tmp41);_tmp40;});}
# 199
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 201
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->name=f->name;_tmp43->tq=f->tq;_tmp43->type=Cyc_Tcutil_copy_type(f->type);_tmp43->width=f->width;_tmp43->attributes=f->attributes;_tmp43->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp43;});}
# 205
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp44=x;void*_tmp47;void*_tmp46;_LL2B: _tmp47=_tmp44->f1;_tmp46=_tmp44->f2;_LL2C:;
return({struct _tuple0*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=Cyc_Tcutil_copy_type(_tmp47);_tmp45->f2=Cyc_Tcutil_copy_type(_tmp46);_tmp45;});}
# 209
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->name=f->name;_tmp48->tag=f->tag;_tmp48->loc=f->loc;_tmp48;});}
# 212
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A;_tmp4A.tag=2;_tmp4A.f1=Cyc_Tcutil_copy_tvar(t);_tmp4A;});_tmp49;});}
# 216
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_Absyn_Datatypedecl*_tmpD2;struct Cyc_Absyn_Enumdecl*_tmpD1;struct Cyc_Absyn_Aggrdecl*_tmpD0;void*_tmpCF;struct Cyc_List_List*_tmpCE;void*_tmpCD;struct _tuple2*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_Absyn_Typedefdecl*_tmpCA;void*_tmpC9;void*_tmpC8;void*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;void*_tmpC4;struct Cyc_List_List*_tmpC3;struct _tuple2*_tmpC2;struct Cyc_Absyn_Enumdecl*_tmpC1;enum Cyc_Absyn_AggrKind _tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Aggrdecl**_tmpBE;struct Cyc_List_List*_tmpBD;enum Cyc_Absyn_AggrKind _tmpBC;struct _tuple2*_tmpBB;struct Cyc_Core_Opt*_tmpBA;struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;void*_tmpB6;struct Cyc_Absyn_Tqual _tmpB5;void*_tmpB4;struct Cyc_List_List*_tmpB3;int _tmpB2;struct Cyc_Absyn_VarargInfo*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_List_List*_tmpAB;void*_tmpAA;struct Cyc_Absyn_Tqual _tmpA9;struct Cyc_Absyn_Exp*_tmpA8;union Cyc_Absyn_Constraint*_tmpA7;unsigned int _tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;union Cyc_Absyn_Constraint*_tmpA2;union Cyc_Absyn_Constraint*_tmpA1;union Cyc_Absyn_Constraint*_tmpA0;struct Cyc_Absyn_PtrLoc*_tmp9F;union Cyc_Absyn_DatatypeFieldInfoU _tmp9E;struct Cyc_List_List*_tmp9D;union Cyc_Absyn_DatatypeInfoU _tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Tvar*_tmp9A;switch(*((int*)_tmp4C)){case 0: _LL2E: _LL2F:
 goto _LL31;case 1: _LL30: _LL31:
 return t;case 2: _LL32: _tmp9A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4C)->f1;_LL33:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4E;_tmp4E.tag=2;_tmp4E.f1=Cyc_Tcutil_copy_tvar(_tmp9A);_tmp4E;});_tmp4D;});case 3: _LL34: _tmp9C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).datatype_info;_tmp9B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL35:
# 222
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp50;_tmp50.tag=3;_tmp50.f1=({struct Cyc_Absyn_DatatypeInfo _tmp51;_tmp51.datatype_info=_tmp9C;_tmp51.targs=Cyc_Tcutil_copy_types(_tmp9B);_tmp51;});_tmp50;});_tmp4F;});case 4: _LL36: _tmp9E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).field_info;_tmp9D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL37:
# 224
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp53;_tmp53.tag=4;_tmp53.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp54;_tmp54.field_info=_tmp9E;_tmp54.targs=Cyc_Tcutil_copy_types(_tmp9D);_tmp54;});_tmp53;});_tmp52;});case 5: _LL38: _tmpA5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_typ;_tmpA4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_tq;_tmpA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).rgn;_tmpA2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_tmpA1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).bounds;_tmpA0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).zero_term;_tmp9F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).ptrloc;_LL39: {
# 226
void*_tmp55=Cyc_Tcutil_copy_type(_tmpA5);
void*_tmp56=Cyc_Tcutil_copy_type(_tmpA3);
union Cyc_Absyn_Constraint*_tmp57=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA2);
struct Cyc_Absyn_Tqual _tmp58=_tmpA4;
# 231
union Cyc_Absyn_Constraint*_tmp59=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA1);
union Cyc_Absyn_Constraint*_tmp5A=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA0);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp5C;_tmp5C.tag=5;_tmp5C.f1=({struct Cyc_Absyn_PtrInfo _tmp5D;_tmp5D.elt_typ=_tmp55;_tmp5D.elt_tq=_tmp58;_tmp5D.ptr_atts=({(_tmp5D.ptr_atts).rgn=_tmp56;(_tmp5D.ptr_atts).nullable=_tmp57;(_tmp5D.ptr_atts).bounds=_tmp59;(_tmp5D.ptr_atts).zero_term=_tmp5A;(_tmp5D.ptr_atts).ptrloc=_tmp9F;_tmp5D.ptr_atts;});_tmp5D;});_tmp5C;});_tmp5B;});}case 6: _LL3A: _LL3B:
 goto _LL3D;case 7: _LL3C: _LL3D:
 return t;case 8: _LL3E: _tmpAA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_tmpA9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).tq;_tmpA8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).num_elts;_tmpA7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zero_term;_tmpA6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zt_loc;_LL3F: {
# 237
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpA8);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp5F;_tmp5F.tag=8;_tmp5F.f1=({struct Cyc_Absyn_ArrayInfo _tmp60;_tmp60.elt_type=Cyc_Tcutil_copy_type(_tmpAA);_tmp60.tq=_tmpA9;_tmp60.num_elts=eopt2;_tmp60.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA7);_tmp60.zt_loc=_tmpA6;_tmp60;});_tmp5F;});_tmp5E;});}case 9: _LL40: _tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).tvars;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).effect;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_tqual;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_typ;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).args;_tmpB2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).c_varargs;_tmpB1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).cyc_varargs;_tmpB0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).rgn_po;_tmpAF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_tmpAE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_clause;_tmpAD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_relns;_tmpAC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_clause;_tmpAB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_relns;_LL41: {
# 241
struct Cyc_List_List*_tmp61=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpB7);
void*effopt2=_tmpB6 == 0?0: Cyc_Tcutil_copy_type(_tmpB6);
void*_tmp62=Cyc_Tcutil_copy_type(_tmpB4);
struct Cyc_List_List*_tmp63=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpB3);
int _tmp64=_tmpB2;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpB1 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpB1;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->name=cv->name;_tmp65->tq=cv->tq;_tmp65->type=Cyc_Tcutil_copy_type(cv->type);_tmp65->inject=cv->inject;_tmp65;});}{
# 252
struct Cyc_List_List*_tmp66=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpB0);
struct Cyc_List_List*_tmp67=_tmpAF;
struct Cyc_Absyn_Exp*_tmp68=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAE);
struct Cyc_List_List*_tmp69=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAD);
struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAC);
struct Cyc_List_List*_tmp6B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAB);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6D;_tmp6D.tag=9;_tmp6D.f1=({struct Cyc_Absyn_FnInfo _tmp6E;_tmp6E.tvars=_tmp61;_tmp6E.effect=effopt2;_tmp6E.ret_tqual=_tmpB5;_tmp6E.ret_typ=_tmp62;_tmp6E.args=_tmp63;_tmp6E.c_varargs=_tmp64;_tmp6E.cyc_varargs=cyc_varargs2;_tmp6E.rgn_po=_tmp66;_tmp6E.attributes=_tmp67;_tmp6E.requires_clause=_tmp68;_tmp6E.requires_relns=_tmp69;_tmp6E.ensures_clause=_tmp6A;_tmp6E.ensures_relns=_tmp6B;_tmp6E;});_tmp6D;});_tmp6C;});};}case 10: _LL42: _tmpB8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4C)->f1;_LL43:
# 261
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp70;_tmp70.tag=10;_tmp70.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpB8);_tmp70;});_tmp6F;});case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).tag == 1){_LL44: _tmpBC=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f1;_tmpBB=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f2;_tmpBA=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f3;_tmpB9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL45:
# 263
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp72;_tmp72.tag=11;_tmp72.f1=({struct Cyc_Absyn_AggrInfo _tmp73;_tmp73.aggr_info=Cyc_Absyn_UnknownAggr(_tmpBC,_tmpBB,_tmpBA);_tmp73.targs=Cyc_Tcutil_copy_types(_tmpB9);_tmp73;});_tmp72;});_tmp71;});}else{_LL46: _tmpBE=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).KnownAggr).val;_tmpBD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL47:
# 265
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp75;_tmp75.tag=11;_tmp75.f1=({struct Cyc_Absyn_AggrInfo _tmp76;_tmp76.aggr_info=Cyc_Absyn_KnownAggr(_tmpBE);_tmp76.targs=Cyc_Tcutil_copy_types(_tmpBD);_tmp76;});_tmp75;});_tmp74;});}case 12: _LL48: _tmpC0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f1;_tmpBF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f2;_LL49:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp78;_tmp78.tag=12;_tmp78.f1=_tmpC0;_tmp78.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpBF);_tmp78;});_tmp77;});case 13: _LL4A: _tmpC2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f2;_LL4B:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp7A;_tmp7A.tag=13;_tmp7A.f1=_tmpC2;_tmp7A.f2=_tmpC1;_tmp7A;});_tmp79;});case 14: _LL4C: _tmpC3=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4C)->f1;_LL4D:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp7C;_tmp7C.tag=14;_tmp7C.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpC3);_tmp7C;});_tmp7B;});case 19: _LL4E: _tmpC4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->f1;_LL4F:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7E;_tmp7E.tag=19;_tmp7E.f1=Cyc_Tcutil_copy_type(_tmpC4);_tmp7E;});_tmp7D;});case 18: _LL50: _tmpC5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4C)->f1;_LL51:
# 272
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp80;_tmp80.tag=18;_tmp80.f1=_tmpC5;_tmp80;});_tmp7F;});case 27: _LL52: _tmpC6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp4C)->f1;_LL53:
# 275
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp82;_tmp82.tag=27;_tmp82.f1=_tmpC6;_tmp82;});_tmp81;});case 15: _LL54: _tmpC7=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4C)->f1;_LL55:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp84;_tmp84.tag=15;_tmp84.f1=Cyc_Tcutil_copy_type(_tmpC7);_tmp84;});_tmp83;});case 16: _LL56: _tmpC9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f2;_LL57:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp86;_tmp86.tag=16;_tmp86.f1=Cyc_Tcutil_copy_type(_tmpC9);_tmp86.f2=Cyc_Tcutil_copy_type(_tmpC8);_tmp86;});_tmp85;});case 17: _LL58: _tmpCC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f1;_tmpCB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f2;_tmpCA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f3;_LL59:
# 279
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp88;_tmp88.tag=17;_tmp88.f1=_tmpCC;_tmp88.f2=Cyc_Tcutil_copy_types(_tmpCB);_tmp88.f3=_tmpCA;_tmp88.f4=0;_tmp88;});_tmp87;});case 21: _LL5A: _LL5B:
 goto _LL5D;case 22: _LL5C: _LL5D:
 goto _LL5F;case 28: _LL5E: _LL5F:
 goto _LL61;case 20: _LL60: _LL61:
 return t;case 23: _LL62: _tmpCD=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4C)->f1;_LL63:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8A;_tmp8A.tag=23;_tmp8A.f1=Cyc_Tcutil_copy_type(_tmpCD);_tmp8A;});_tmp89;});case 24: _LL64: _tmpCE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4C)->f1;_LL65:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8C;_tmp8C.tag=24;_tmp8C.f1=Cyc_Tcutil_copy_types(_tmpCE);_tmp8C;});_tmp8B;});case 25: _LL66: _tmpCF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4C)->f1;_LL67:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8E;_tmp8E.tag=25;_tmp8E.f1=Cyc_Tcutil_copy_type(_tmpCF);_tmp8E;});_tmp8D;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)){case 0: _LL68: _tmpD0=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL69: {
# 289
struct Cyc_List_List*_tmp8F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD0->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp91;_tmp91.tag=11;_tmp91.f1=({struct Cyc_Absyn_AggrInfo _tmp92;_tmp92.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD0->kind,_tmpD0->name,0);_tmp92.targs=_tmp8F;_tmp92;});_tmp91;});_tmp90;});}case 1: _LL6A: _tmpD1=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL6B:
# 292
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp94;_tmp94.tag=13;_tmp94.f1=_tmpD1->name;_tmp94.f2=0;_tmp94;});_tmp93;});default: _LL6C: _tmpD2=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL6D: {
# 294
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD2->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp97;_tmp97.tag=3;_tmp97.f1=({struct Cyc_Absyn_DatatypeInfo _tmp98;_tmp98.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp99;_tmp99.name=_tmpD2->name;_tmp99.is_extensible=0;_tmp99;}));_tmp98.targs=_tmp95;_tmp98;});_tmp97;});_tmp96;});}}}_LL2D:;}
# 309 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD3=d;struct _dyneither_ptr*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->tag == 0){_LL6F: _tmpD6=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->f1;_LL70:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD5;_tmpD5.tag=0;_tmpD5.f1=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpD6);_tmpD5;});_tmpD4;});}else{_LL71: _tmpD7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD3)->f1;_LL72:
 return d;}_LL6E:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 315
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 317
return({struct _tuple19*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpD8->f2=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpD8;});}
# 320
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpD9=preserve_types;
{void*_tmpDA=e->r;void*_tmpDB=_tmpDA;int _tmp167;struct _dyneither_ptr _tmp166;void*_tmp165;struct Cyc_Absyn_Exp*_tmp164;struct _dyneither_ptr*_tmp163;struct Cyc_Core_Opt*_tmp162;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;int _tmp15E;struct Cyc_Absyn_Exp*_tmp15D;void**_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;int _tmp15A;int _tmp159;void*_tmp158;struct Cyc_Absyn_Enumfield*_tmp157;struct Cyc_Absyn_Enumdecl*_tmp156;struct Cyc_Absyn_Enumfield*_tmp155;struct Cyc_List_List*_tmp154;struct Cyc_Absyn_Datatypedecl*_tmp153;struct Cyc_Absyn_Datatypefield*_tmp152;void*_tmp151;struct Cyc_List_List*_tmp150;struct _tuple2*_tmp14F;struct Cyc_List_List*_tmp14E;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Aggrdecl*_tmp14C;struct Cyc_Absyn_Exp*_tmp14B;void*_tmp14A;int _tmp149;struct Cyc_Absyn_Vardecl*_tmp148;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp146;int _tmp145;struct Cyc_List_List*_tmp144;struct _dyneither_ptr*_tmp143;struct Cyc_Absyn_Tqual _tmp142;void*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct _dyneither_ptr*_tmp13B;int _tmp13A;int _tmp139;struct Cyc_Absyn_Exp*_tmp138;struct _dyneither_ptr*_tmp137;int _tmp136;int _tmp135;struct Cyc_Absyn_Exp*_tmp134;void*_tmp133;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_Exp*_tmp131;void*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;void*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;int _tmp12A;enum Cyc_Absyn_Coercion _tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;int _tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_List_List*_tmp122;struct Cyc_Absyn_VarargCallInfo*_tmp121;int _tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;enum Cyc_Absyn_Incrementor _tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Core_Opt*_tmp113;struct Cyc_Absyn_Exp*_tmp112;enum Cyc_Absyn_Primop _tmp111;struct Cyc_List_List*_tmp110;void*_tmp10F;union Cyc_Absyn_Cnst _tmp10E;switch(*((int*)_tmpDB)){case 0: _LL74: _tmp10E=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL75:
 new_e=Cyc_Absyn_const_exp(_tmp10E,e->loc);goto _LL73;case 1: _LL76: _tmp10F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL77:
 new_e=Cyc_Absyn_varb_exp(_tmp10F,e->loc);goto _LL73;case 2: _LL78: _tmp111=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp110=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL79:
 new_e=Cyc_Absyn_primop_exp(_tmp111,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp110),e->loc);goto _LL73;case 3: _LL7A: _tmp114=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp113=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp112=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL7B:
# 328
 new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp114),(unsigned int)_tmp113?({struct Cyc_Core_Opt*_tmpDC=_cycalloc_atomic(sizeof(*_tmpDC));_tmpDC->v=(void*)_tmp113->v;_tmpDC;}): 0,Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp112),e->loc);
goto _LL73;case 4: _LL7C: _tmp116=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp115=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL7D:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp116),_tmp115,e->loc);goto _LL73;case 5: _LL7E: _tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp118=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp117=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL7F:
# 332
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp119),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp118),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp117),e->loc);goto _LL73;case 6: _LL80: _tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL81:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11B),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11A),e->loc);goto _LL73;case 7: _LL82: _tmp11D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL83:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11D),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11C),e->loc);goto _LL73;case 8: _LL84: _tmp11F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL85:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11E),e->loc);goto _LL73;case 9: _LL86: _tmp123=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp122=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp121=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp120=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL87:
# 337
{struct Cyc_Absyn_VarargCallInfo*_tmpDD=_tmp121;int _tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_VarargInfo*_tmpE9;if(_tmpDD != 0){_LLC5: _tmpEB=_tmpDD->num_varargs;_tmpEA=_tmpDD->injectors;_tmpE9=_tmpDD->vai;_LLC6: {
# 339
struct Cyc_Absyn_VarargInfo*_tmpDE=_tmpE9;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Tqual _tmpE5;void*_tmpE4;int _tmpE3;_LLCA: _tmpE6=_tmpDE->name;_tmpE5=_tmpDE->tq;_tmpE4=_tmpDE->type;_tmpE3=_tmpDE->inject;_LLCB:;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE0;_tmpE0.tag=9;_tmpE0.f1=
Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp123);_tmpE0.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp122);_tmpE0.f3=({struct Cyc_Absyn_VarargCallInfo*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->num_varargs=_tmpEB;_tmpE1->injectors=_tmpEA;_tmpE1->vai=({struct Cyc_Absyn_VarargInfo*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->name=_tmpE6;_tmpE2->tq=_tmpE5;_tmpE2->type=
# 343
Cyc_Tcutil_copy_type(_tmpE4);_tmpE2->inject=_tmpE3;_tmpE2;});_tmpE1;});_tmpE0.f4=_tmp120;_tmpE0;});_tmpDF;}),e->loc);
# 345
goto _LLC4;}}else{_LLC7: _LLC8:
# 347
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE8;_tmpE8.tag=9;_tmpE8.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp123);_tmpE8.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp122);_tmpE8.f3=_tmp121;_tmpE8.f4=_tmp120;_tmpE8;});_tmpE7;}),e->loc);
goto _LLC4;}_LLC4:;}
# 350
goto _LL73;case 10: _LL88: _tmp125=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp124=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL89:
# 352
 new_e=_tmp124?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp125),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp125),e->loc);
goto _LL73;case 11: _LL8A: _tmp126=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL8B:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp126),e->loc);
goto _LL73;case 12: _LL8C: _tmp128=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp127=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL8D:
# 357
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp128),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp127),e->loc);
goto _LL73;case 13: _LL8E: _tmp12C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp12A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp129=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL8F:
# 360
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp12C),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12B),_tmp12A,_tmp129,e->loc);goto _LL73;case 14: _LL90: _tmp12D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL91:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12D),e->loc);goto _LL73;case 15: _LL92: _tmp12F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL93: {
# 363
struct Cyc_Absyn_Exp*eo1=_tmp12F;if(_tmp12F != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12F);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12E),e->loc);
goto _LL73;}case 16: _LL94: _tmp130=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL95:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp130),e->loc);
goto _LL73;case 17: _LL96: _tmp131=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL97:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp131),e->loc);goto _LL73;case 18: _LL98: _tmp133=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp132=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL99:
# 370
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp133),_tmp132,e->loc);goto _LL73;case 19: _LL9A: _tmp134=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL9B:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp134),e->loc);goto _LL73;case 20: _LL9C: _tmp138=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp137=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp136=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp135=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL9D:
# 373
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpED;_tmpED.tag=20;_tmpED.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp138);_tmpED.f2=_tmp137;_tmpED.f3=_tmp136;_tmpED.f4=_tmp135;_tmpED;});_tmpEC;}),e->loc);goto _LL73;case 21: _LL9E: _tmp13C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp13A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp139=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL9F:
# 375
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpEF;_tmpEF.tag=21;_tmpEF.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13C);_tmpEF.f2=_tmp13B;_tmpEF.f3=_tmp13A;_tmpEF.f4=_tmp139;_tmpEF;});_tmpEE;}),e->loc);goto _LL73;case 22: _LLA0: _tmp13E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLA1:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13E),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13D),e->loc);
goto _LL73;case 23: _LLA2: _tmp13F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLA3:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp13F),e->loc);goto _LL73;case 24: _LLA4: _tmp143=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f1;_tmp142=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f2;_tmp141=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f3;_tmp140=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLA5: {
# 380
struct _dyneither_ptr*vopt1=_tmp143;
if(_tmp143 != 0)vopt1=_tmp143;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF1;_tmpF1.tag=24;_tmpF1.f1=({struct _tuple10*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=vopt1;_tmpF2->f2=_tmp142;_tmpF2->f3=Cyc_Tcutil_copy_type(_tmp141);_tmpF2;});_tmpF1.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp140);_tmpF1;});_tmpF0;}),e->loc);
goto _LL73;}case 25: _LLA6: _tmp144=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLA7:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF4;_tmpF4.tag=25;_tmpF4.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp144);_tmpF4;});_tmpF3;}),e->loc);
goto _LL73;case 26: _LLA8: _tmp148=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp147=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp146=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp145=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LLA9:
# 388
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF6;_tmpF6.tag=26;_tmpF6.f1=_tmp148;_tmpF6.f2=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp147);_tmpF6.f3=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp146);_tmpF6.f4=_tmp145;_tmpF6;});_tmpF5;}),e->loc);
goto _LL73;case 27: _LLAA: _tmp14B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14A=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp149=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLAB:
# 391
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF8;_tmpF8.tag=27;_tmpF8.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp14B);_tmpF8.f2=Cyc_Tcutil_copy_type(_tmp14A);_tmpF8.f3=_tmp149;_tmpF8;});_tmpF7;}),_tmp14B->loc);
# 393
goto _LL73;case 28: _LLAC: _tmp14F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp14D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp14C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LLAD:
# 395
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpFA;_tmpFA.tag=28;_tmpFA.f1=_tmp14F;_tmpFA.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp14E);_tmpFA.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp14D);_tmpFA.f4=_tmp14C;_tmpFA;});_tmpF9;}),e->loc);
# 397
goto _LL73;case 29: _LLAE: _tmp151=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp150=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLAF:
# 399
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpFC;_tmpFC.tag=29;_tmpFC.f1=Cyc_Tcutil_copy_type(_tmp151);_tmpFC.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp150);_tmpFC;});_tmpFB;}),e->loc);
goto _LL73;case 30: _LLB0: _tmp154=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp153=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp152=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLB1:
# 402
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpFE;_tmpFE.tag=30;_tmpFE.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp154);_tmpFE.f2=_tmp153;_tmpFE.f3=_tmp152;_tmpFE;});_tmpFD;}),e->loc);
goto _LL73;case 31: _LLB2: _tmp156=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp155=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB3:
 new_e=e;goto _LL73;case 32: _LLB4: _tmp158=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp157=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB5:
# 406
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp100;_tmp100.tag=32;_tmp100.f1=Cyc_Tcutil_copy_type(_tmp158);_tmp100.f2=_tmp157;_tmp100;});_tmpFF;}),e->loc);
goto _LL73;case 33: _LLB6: _tmp15E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).is_calloc;_tmp15D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).rgn;_tmp15C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).elt_type;_tmp15B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).num_elts;_tmp15A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).fat_result;_tmp159=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).inline_call;_LLB7: {
# 409
struct Cyc_Absyn_Exp*_tmp101=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp15D;if(_tmp15D != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp15D);{
void**t1=_tmp15C;if(_tmp15C != 0)t1=({void**_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=Cyc_Tcutil_copy_type(*_tmp15C);_tmp102;});
_tmp101->r=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp104;_tmp104.tag=33;_tmp104.f1=({struct Cyc_Absyn_MallocInfo _tmp105;_tmp105.is_calloc=_tmp15E;_tmp105.rgn=r1;_tmp105.elt_type=t1;_tmp105.num_elts=_tmp15B;_tmp105.fat_result=_tmp15A;_tmp105.inline_call=_tmp159;_tmp105;});_tmp104;});_tmp103;});
new_e=_tmp101;
goto _LL73;};}case 34: _LLB8: _tmp160=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp15F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB9:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp160),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp15F),e->loc);goto _LL73;case 35: _LLBA: _tmp162=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp161=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLBB: {
# 417
struct Cyc_Core_Opt*nopt1=_tmp162;
if(_tmp162 != 0)nopt1=({struct Cyc_Core_Opt*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->v=(struct _tuple2*)_tmp162->v;_tmp106;});
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=35;_tmp108.f1=nopt1;_tmp108.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp161);_tmp108;});_tmp107;}),e->loc);
goto _LL73;}case 36: _LLBC: _LLBD:
# 422
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Core_Failure_exn_struct _tmp10A;_tmp10A.tag=Cyc_Core_Failure;_tmp10A.f1=({const char*_tmp10B="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp10B,sizeof(char),45);});_tmp10A;});_tmp109;}));case 37: _LLBE: _tmp164=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp163=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLBF:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp10D;_tmp10D.tag=37;_tmp10D.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp164);_tmp10D.f2=_tmp163;_tmp10D;});_tmp10C;}),e->loc);
goto _LL73;case 38: _LLC0: _tmp165=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLC1:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp165),e->loc);
goto _LL73;default: _LLC2: _tmp167=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp166=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLC3:
 new_e=Cyc_Absyn_asm_exp(_tmp167,_tmp166,e->loc);goto _LL73;}_LL73:;}
# 430
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 434
return new_e;}
# 437
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 450 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp168=ka1;enum Cyc_Absyn_KindQual _tmp173;enum Cyc_Absyn_AliasQual _tmp172;_LLCD: _tmp173=_tmp168->kind;_tmp172=_tmp168->aliasqual;_LLCE:;{
struct Cyc_Absyn_Kind*_tmp169=ka2;enum Cyc_Absyn_KindQual _tmp171;enum Cyc_Absyn_AliasQual _tmp170;_LLD0: _tmp171=_tmp169->kind;_tmp170=_tmp169->aliasqual;_LLD1:;
# 454
if(_tmp173 != _tmp171){
struct _tuple20 _tmp16A=({struct _tuple20 _tmp16C;_tmp16C.f1=_tmp173;_tmp16C.f2=_tmp171;_tmp16C;});struct _tuple20 _tmp16B=_tmp16A;switch(_tmp16B.f1){case Cyc_Absyn_BoxKind: switch(_tmp16B.f2){case Cyc_Absyn_MemKind: _LLD3: _LLD4:
 goto _LLD6;case Cyc_Absyn_AnyKind: _LLD5: _LLD6:
 goto _LLD8;default: goto _LLD9;}case Cyc_Absyn_MemKind: if(_tmp16B.f2 == Cyc_Absyn_AnyKind){_LLD7: _LLD8:
 goto _LLD2;}else{goto _LLD9;}default: _LLD9: _LLDA:
 return 0;}_LLD2:;}
# 463
if(_tmp172 != _tmp170){
struct _tuple21 _tmp16D=({struct _tuple21 _tmp16F;_tmp16F.f1=_tmp172;_tmp16F.f2=_tmp170;_tmp16F;});struct _tuple21 _tmp16E=_tmp16D;switch(_tmp16E.f1){case Cyc_Absyn_Aliasable: if(_tmp16E.f2 == Cyc_Absyn_Top){_LLDC: _LLDD:
 goto _LLDF;}else{goto _LLE0;}case Cyc_Absyn_Unique: if(_tmp16E.f2 == Cyc_Absyn_Top){_LLDE: _LLDF:
 return 1;}else{goto _LLE0;}default: _LLE0: _LLE1:
 return 0;}_LLDB:;}
# 470
return 1;};}
# 473
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp174=Cyc_Absyn_compress_kb(tv->kind);void*_tmp175=_tmp174;struct Cyc_Absyn_Kind*_tmp179;struct Cyc_Absyn_Kind*_tmp178;switch(*((int*)_tmp175)){case 0: _LLE3: _tmp178=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp175)->f1;_LLE4:
 return _tmp178;case 2: _LLE5: _tmp179=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp175)->f2;_LLE6:
 return _tmp179;default: _LLE7: _LLE8:
# 478
 tv->kind=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp177;_tmp177.tag=2;_tmp177.f1=0;_tmp177.f2=def;_tmp177;});_tmp176;});
return def;}_LLE2:;}
# 483
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp17A=({struct _tuple0 _tmp191;_tmp191.f1=Cyc_Absyn_compress_kb(kb1);_tmp191.f2=Cyc_Absyn_compress_kb(kb2);_tmp191;});struct _tuple0 _tmp17B=_tmp17A;struct Cyc_Core_Opt**_tmp190;void*_tmp18F;void*_tmp18E;struct Cyc_Core_Opt**_tmp18D;struct Cyc_Core_Opt**_tmp18C;struct Cyc_Absyn_Kind*_tmp18B;struct Cyc_Core_Opt**_tmp18A;struct Cyc_Absyn_Kind*_tmp189;struct Cyc_Core_Opt**_tmp188;struct Cyc_Absyn_Kind*_tmp187;struct Cyc_Absyn_Kind*_tmp186;struct Cyc_Absyn_Kind*_tmp185;struct Cyc_Core_Opt**_tmp184;struct Cyc_Absyn_Kind*_tmp183;struct Cyc_Absyn_Kind*_tmp182;struct Cyc_Absyn_Kind*_tmp181;switch(*((int*)_tmp17B.f1)){case 0: switch(*((int*)_tmp17B.f2)){case 0: _LLEA: _tmp182=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp181=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLEB:
 return _tmp182 == _tmp181;case 2: _LLEE: _tmp185=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp184=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_tmp183=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f2;_LLEF:
# 492
 if(Cyc_Tcutil_kind_leq(_tmp185,_tmp183)){
*_tmp184=({struct Cyc_Core_Opt*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->v=kb1;_tmp17D;});
return 1;}else{
return 0;}default: goto _LLF4;}case 2: switch(*((int*)_tmp17B.f2)){case 0: _LLEC: _tmp188=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp187=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f2;_tmp186=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLED:
# 487
 if(Cyc_Tcutil_kind_leq(_tmp186,_tmp187)){
*_tmp188=({struct Cyc_Core_Opt*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->v=kb2;_tmp17C;});
return 1;}else{
return 0;}case 2: _LLF0: _tmp18C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp18B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f2;_tmp18A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_tmp189=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f2;_LLF1:
# 497
 if(Cyc_Tcutil_kind_leq(_tmp18B,_tmp189)){
*_tmp18A=({struct Cyc_Core_Opt*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->v=kb1;_tmp17E;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp189,_tmp18B)){
*_tmp18C=({struct Cyc_Core_Opt*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->v=kb2;_tmp17F;});
return 1;}else{
return 0;}}default: _LLF4: _tmp18E=_tmp17B.f1;_tmp18D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLF5:
# 506
*_tmp18D=({struct Cyc_Core_Opt*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=_tmp18E;_tmp180;});
return 1;}default: _LLF2: _tmp190=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp18F=_tmp17B.f2;_LLF3:
# 504
 _tmp18E=_tmp18F;_tmp18D=_tmp190;goto _LLF5;}_LLE9:;}
# 511
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp192=Cyc_Tcutil_compress(t);void*_tmp193=_tmp192;struct Cyc_Absyn_Tvar*_tmp199;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp193)->tag == 2){_LLF7: _tmp199=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp193)->f1;_LLF8: {
# 514
void*_tmp194=_tmp199->kind;
_tmp199->kind=kb;
return({struct _tuple16 _tmp195;_tmp195.f1=_tmp199;_tmp195.f2=_tmp194;_tmp195;});}}else{_LLF9: _LLFA:
# 518
({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0;_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp196[1]={& _tmp198};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp197="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp197,sizeof(char),40);}),_tag_dyneither(_tmp196,sizeof(void*),1));});});}_LLF6:;}
# 524
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 526
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 530
struct _RegionHandle _tmp19A=_new_region("temp");struct _RegionHandle*temp=& _tmp19A;_push_region(temp);
{struct Cyc_List_List*_tmp19B=0;
# 533
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp19C=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp19D=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp19E=Cyc_Tcutil_tvar_kind(_tmp19C,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp19F=_tmp19E;switch(((struct Cyc_Absyn_Kind*)_tmp19F)->kind){case Cyc_Absyn_IntKind: _LLFC: _LLFD:
 goto _LLFF;case Cyc_Absyn_AnyKind: _LLFE: _LLFF:
# 539
 _tmp19B=({struct Cyc_List_List*_tmp1A0=_region_malloc(temp,sizeof(*_tmp1A0));_tmp1A0->hd=({struct _tuple16*_tmp1A1=_region_malloc(temp,sizeof(*_tmp1A1));_tmp1A1->f1=_tmp19C;_tmp1A1->f2=_tmp19D;_tmp1A1;});_tmp1A0->tl=_tmp19B;_tmp1A0;});goto _LLFB;default: _LL100: _LL101:
 goto _LLFB;}_LLFB:;}
# 543
if(_tmp19B != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp19B),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 531
;_pop_region(temp);}
# 548
return k;}
# 555
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1A2=Cyc_Tcutil_compress(t);void*_tmp1A3=_tmp1A2;struct Cyc_Absyn_Typedefdecl*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Kind*_tmp1BF;struct Cyc_Absyn_PtrInfo _tmp1BE;enum Cyc_Absyn_AggrKind _tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_AggrdeclImpl*_tmp1BB;int _tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Datatypedecl*_tmp1B8;struct Cyc_Absyn_Datatypefield*_tmp1B7;enum Cyc_Absyn_Size_of _tmp1B6;struct Cyc_Absyn_Tvar*_tmp1B5;struct Cyc_Core_Opt*_tmp1B4;switch(*((int*)_tmp1A3)){case 1: _LL103: _tmp1B4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A3)->f1;_LL104:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B4))->v;case 2: _LL105: _tmp1B5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A3)->f1;_LL106:
 return Cyc_Tcutil_tvar_kind(_tmp1B5,& Cyc_Tcutil_bk);case 0: _LL107: _LL108:
 return& Cyc_Tcutil_mk;case 6: _LL109: _tmp1B6=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1A3)->f2;_LL10A:
# 561
 return(_tmp1B6 == Cyc_Absyn_Int_sz  || _tmp1B6 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7: _LL10B: _LL10C:
# 563
 return& Cyc_Tcutil_mk;case 9: _LL10D: _LL10E:
 return& Cyc_Tcutil_ak;case 16: _LL10F: _LL110:
 goto _LL112;case 13: _LL111: _LL112:
 goto _LL114;case 14: _LL113: _LL114:
 goto _LL116;case 15: _LL115: _LL116:
 return& Cyc_Tcutil_bk;case 21: _LL117: _LL118:
 return& Cyc_Tcutil_urk;case 20: _LL119: _LL11A:
 return& Cyc_Tcutil_rk;case 22: _LL11B: _LL11C:
 return& Cyc_Tcutil_trk;case 3: _LL11D: _LL11E:
# 574
 return& Cyc_Tcutil_ak;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A3)->f1).field_info).KnownDatatypefield).tag == 2){_LL11F: _tmp1B8=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A3)->f1).field_info).KnownDatatypefield).val).f1;_tmp1B7=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A3)->f1).field_info).KnownDatatypefield).val).f2;_LL120:
# 576
 return& Cyc_Tcutil_mk;}else{_LL121: _LL122:
# 578
({void*_tmp1A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A5="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1A5,sizeof(char),39);}),_tag_dyneither(_tmp1A4,sizeof(void*),0));});}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).UnknownAggr).tag == 1){_LL123: _LL124:
# 581
 return& Cyc_Tcutil_ak;}else{_LL125: _tmp1BD=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).KnownAggr).val)->kind;_tmp1BC=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1BB=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).KnownAggr).val)->impl;_tmp1BA=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1B9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).targs;_LL126:
# 583
 if(_tmp1BB == 0){
if(_tmp1BA)
return& Cyc_Tcutil_mk;else{
# 587
return& Cyc_Tcutil_ak;}}{
# 589
struct Cyc_List_List*_tmp1A6=_tmp1BB->fields;
if(_tmp1A6 == 0)return& Cyc_Tcutil_mk;
# 592
if(_tmp1BD == Cyc_Absyn_StructA){
for(0;_tmp1A6->tl != 0;_tmp1A6=_tmp1A6->tl){;}{
void*_tmp1A7=((struct Cyc_Absyn_Aggrfield*)_tmp1A6->hd)->type;
struct Cyc_Absyn_Kind*_tmp1A8=Cyc_Tcutil_field_kind(_tmp1A7,_tmp1B9,_tmp1BC);
if(_tmp1A8 == & Cyc_Tcutil_ak  || _tmp1A8 == & Cyc_Tcutil_tak)return _tmp1A8;};}else{
# 600
for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){
void*_tmp1A9=((struct Cyc_Absyn_Aggrfield*)_tmp1A6->hd)->type;
struct Cyc_Absyn_Kind*_tmp1AA=Cyc_Tcutil_field_kind(_tmp1A9,_tmp1B9,_tmp1BC);
if(_tmp1AA == & Cyc_Tcutil_ak  || _tmp1AA == & Cyc_Tcutil_tak)return _tmp1AA;}}
# 606
return& Cyc_Tcutil_mk;};}case 12: _LL127: _LL128:
 return& Cyc_Tcutil_mk;case 5: _LL129: _tmp1BE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A3)->f1;_LL12A: {
# 609
void*_tmp1AB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1BE.ptr_atts).bounds);void*_tmp1AC=_tmp1AB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1AC)->tag == 0){_LL146: _LL147: {
# 611
enum Cyc_Absyn_AliasQual _tmp1AD=(Cyc_Tcutil_typ_kind((_tmp1BE.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1AE=_tmp1AD;switch(_tmp1AE){case Cyc_Absyn_Aliasable: _LL14B: _LL14C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL14D: _LL14E:
 return& Cyc_Tcutil_umk;default: _LL14F: _LL150:
 return& Cyc_Tcutil_tmk;}_LL14A:;}}else{_LL148: _LL149: {
# 617
enum Cyc_Absyn_AliasQual _tmp1AF=(Cyc_Tcutil_typ_kind((_tmp1BE.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1B0=_tmp1AF;switch(_tmp1B0){case Cyc_Absyn_Aliasable: _LL152: _LL153:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL154: _LL155:
 return& Cyc_Tcutil_ubk;default: _LL156: _LL157:
 return& Cyc_Tcutil_tbk;}_LL151:;}}_LL145:;}case 18: _LL12B: _LL12C:
# 623
 return& Cyc_Tcutil_ik;case 28: _LL12D: _tmp1BF=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1A3)->f2;_LL12E:
 return _tmp1BF;case 27: _LL12F: _LL130:
# 628
 return& Cyc_Tcutil_ak;case 19: _LL131: _LL132:
 return& Cyc_Tcutil_bk;case 8: _LL133: _tmp1C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A3)->f1).num_elts;_LL134:
# 631
 if(_tmp1C0 == 0  || Cyc_Tcutil_is_const_exp(_tmp1C0))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10: _LL135: _LL136:
 return& Cyc_Tcutil_mk;case 17: _LL137: _tmp1C1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A3)->f3;_LL138:
# 635
 if(_tmp1C1 == 0  || _tmp1C1->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1B1[1]={& _tmp1B3};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B2="typ_kind: typedef found: %s";_tag_dyneither(_tmp1B2,sizeof(char),28);}),_tag_dyneither(_tmp1B1,sizeof(void*),1));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C1->kind))->v;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A3)->f1)->r)){case 0: _LL139: _LL13A:
 return& Cyc_Tcutil_ak;case 1: _LL13B: _LL13C:
 return& Cyc_Tcutil_bk;default: _LL13D: _LL13E:
 return& Cyc_Tcutil_ak;}case 23: _LL13F: _LL140:
 goto _LL142;case 24: _LL141: _LL142:
 goto _LL144;default: _LL143: _LL144:
 return& Cyc_Tcutil_ek;}_LL102:;}
# 647
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 652
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1C2;_push_handler(& _tmp1C2);{int _tmp1C4=0;if(setjmp(_tmp1C2.handler))_tmp1C4=1;if(!_tmp1C4){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1C5=1;_npop_handler(0);return _tmp1C5;};
# 654
;_pop_handler();}else{void*_tmp1C3=(void*)_exn_thrown;void*_tmp1C6=_tmp1C3;void*_tmp1C7;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1C6)->tag == Cyc_Tcutil_Unify){_LL159: _LL15A:
# 656
 return 0;}else{_LL15B: _tmp1C7=_tmp1C6;_LL15C:(int)_rethrow(_tmp1C7);}_LL158:;}};}
# 661
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1C8=t;struct Cyc_List_List*_tmp1EE;void*_tmp1ED;void*_tmp1EC;void*_tmp1EB;void*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E3;void*_tmp1E2;struct Cyc_Absyn_Tqual _tmp1E1;void*_tmp1E0;struct Cyc_List_List*_tmp1DF;int _tmp1DE;struct Cyc_Absyn_VarargInfo*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_List_List*_tmp1D7;void*_tmp1D6;struct Cyc_Absyn_PtrInfo _tmp1D5;void*_tmp1D4;struct Cyc_Core_Opt**_tmp1D3;struct Cyc_Absyn_Tvar*_tmp1D2;switch(*((int*)_tmp1C8)){case 2: _LL15E: _tmp1D2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C8)->f1;_LL15F:
# 666
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1D2)){
Cyc_Tcutil_failure_reason=({const char*_tmp1C9="(type variable would escape scope)";_tag_dyneither(_tmp1C9,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 670
goto _LL15D;case 1: _LL160: _tmp1D4=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C8)->f2;_tmp1D3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C8)->f4;_LL161:
# 672
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1CA="(occurs check)";_tag_dyneither(_tmp1CA,sizeof(char),15);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 676
if(_tmp1D4 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1D4);else{
# 679
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D3))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 686
if(problem){
struct Cyc_List_List*_tmp1CB=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D3))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1CB=({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1CC->tl=_tmp1CB;_tmp1CC;});}}
# 692
*_tmp1D3=({struct Cyc_Core_Opt*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->v=_tmp1CB;_tmp1CD;});}}}
# 695
goto _LL15D;case 5: _LL162: _tmp1D5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C8)->f1;_LL163:
# 698
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D5.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1D5.ptr_atts).rgn);
goto _LL15D;case 8: _LL164: _tmp1D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C8)->f1).elt_type;_LL165:
# 702
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D6);goto _LL15D;case 9: _LL166: _tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).tvars;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).effect;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ret_tqual;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ret_typ;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).args;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).c_varargs;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).cyc_varargs;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).rgn_po;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).attributes;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_clause;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_relns;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_clause;_tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_relns;_LL167:
# 705
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1E3,env);
if(_tmp1E2 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1E2);
Cyc_Tcutil_occurs(evar,r,env,_tmp1E0);
for(0;_tmp1DF != 0;_tmp1DF=_tmp1DF->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1DF->hd)).f3);}
if(_tmp1DD != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1DD->type);
for(0;_tmp1DC != 0;_tmp1DC=_tmp1DC->tl){
struct _tuple0*_tmp1CE=(struct _tuple0*)_tmp1DC->hd;struct _tuple0*_tmp1CF=_tmp1CE;void*_tmp1D1;void*_tmp1D0;_LL181: _tmp1D1=_tmp1CF->f1;_tmp1D0=_tmp1CF->f2;_LL182:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1D1);
Cyc_Tcutil_occurs(evar,r,env,_tmp1D0);}
# 717
goto _LL15D;case 10: _LL168: _tmp1E4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1C8)->f1;_LL169:
# 719
 for(0;_tmp1E4 != 0;_tmp1E4=_tmp1E4->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1E4->hd)).f2);}
goto _LL15D;case 3: _LL16A: _tmp1E5=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C8)->f1).targs;_LL16B:
# 723
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E5);goto _LL15D;case 17: _LL16C: _tmp1E6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1C8)->f2;_LL16D:
 _tmp1E7=_tmp1E6;goto _LL16F;case 4: _LL16E: _tmp1E7=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1C8)->f1).targs;_LL16F:
 _tmp1E8=_tmp1E7;goto _LL171;case 11: _LL170: _tmp1E8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C8)->f1).targs;_LL171:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E8);goto _LL15D;case 12: _LL172: _tmp1E9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C8)->f2;_LL173:
# 729
 for(0;_tmp1E9 != 0;_tmp1E9=_tmp1E9->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1E9->hd)->type);}
goto _LL15D;case 19: _LL174: _tmp1EA=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1C8)->f1;_LL175:
 _tmp1EB=_tmp1EA;goto _LL177;case 23: _LL176: _tmp1EB=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1C8)->f1;_LL177:
 _tmp1EC=_tmp1EB;goto _LL179;case 15: _LL178: _tmp1EC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1C8)->f1;_LL179:
 _tmp1ED=_tmp1EC;goto _LL17B;case 25: _LL17A: _tmp1ED=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1C8)->f1;_LL17B:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1ED);goto _LL15D;case 24: _LL17C: _tmp1EE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1C8)->f1;_LL17D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1EE);goto _LL15D;default: _LL17E: _LL17F:
# 739
 goto _LL15D;}_LL15D:;};}
# 742
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 744
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 749
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 757
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1EF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F0="tq1 real_const not set.";_tag_dyneither(_tmp1F0,sizeof(char),24);}),_tag_dyneither(_tmp1EF,sizeof(void*),0));});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1F1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F2="tq2 real_const not set.";_tag_dyneither(_tmp1F2,sizeof(char),24);}),_tag_dyneither(_tmp1F1,sizeof(void*),0));});
# 763
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 766
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp1F3="(qualifiers don't match)";_tag_dyneither(_tmp1F3,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 774
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 778
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 785
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 787
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1F4=x;void*_tmp1FC;switch((((union Cyc_Absyn_Constraint*)_tmp1F4)->Eq_constr).tag){case 3: _LL184: _LL185:
# 792
*x=({union Cyc_Absyn_Constraint _tmp1F5;(_tmp1F5.Forward_constr).val=y;(_tmp1F5.Forward_constr).tag=2;_tmp1F5;});return;case 1: _LL186: _tmp1FC=(void*)(_tmp1F4->Eq_constr).val;_LL187: {
# 794
union Cyc_Absyn_Constraint*_tmp1F6=y;void*_tmp1F9;switch((((union Cyc_Absyn_Constraint*)_tmp1F6)->Eq_constr).tag){case 3: _LL18B: _LL18C:
*y=*x;return;case 1: _LL18D: _tmp1F9=(void*)(_tmp1F6->Eq_constr).val;_LL18E:
# 797
 if(cmp(_tmp1FC,_tmp1F9)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 801
return;default: _LL18F: _LL190:
({void*_tmp1F7=0;Cyc_Tcutil_impos(({const char*_tmp1F8="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1F8,sizeof(char),40);}),_tag_dyneither(_tmp1F7,sizeof(void*),0));});}_LL18A:;}default: _LL188: _LL189:
# 804
({void*_tmp1FA=0;Cyc_Tcutil_impos(({const char*_tmp1FB="unify_conref: forward after compress";_tag_dyneither(_tmp1FB,sizeof(char),37);}),_tag_dyneither(_tmp1FA,sizeof(void*),0));});}_LL183:;};}
# 808
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp1FD;_push_handler(& _tmp1FD);{int _tmp1FF=0;if(setjmp(_tmp1FD.handler))_tmp1FF=1;if(!_tmp1FF){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp200=1;_npop_handler(0);return _tmp200;};
# 810
;_pop_handler();}else{void*_tmp1FE=(void*)_exn_thrown;void*_tmp201=_tmp1FE;void*_tmp202;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp201)->tag == Cyc_Tcutil_Unify){_LL192: _LL193:
# 812
 return 0;}else{_LL194: _tmp202=_tmp201;_LL195:(int)_rethrow(_tmp202);}_LL191:;}};}
# 815
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp203=({struct _tuple0 _tmp207;_tmp207.f1=b1;_tmp207.f2=b2;_tmp207;});struct _tuple0 _tmp204=_tmp203;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*_tmp205;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp204.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp204.f2)->tag == 0){_LL197: _LL198:
 return 0;}else{_LL199: _LL19A:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp204.f2)->tag == 0){_LL19B: _LL19C:
 return 1;}else{_LL19D: _tmp206=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp204.f1)->f1;_tmp205=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp204.f2)->f1;_LL19E:
 return Cyc_Evexp_const_exp_cmp(_tmp206,_tmp205);}}_LL196:;}
# 824
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmp208=({struct _tuple0 _tmp216;_tmp216.f1=e1->r;_tmp216.f2=e2->r;_tmp216;});struct _tuple0 _tmp209=_tmp208;void*_tmp215;void*_tmp214;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*_tmp212;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209.f1)->tag == 13){_LL1A0: _tmp212=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209.f1)->f2;_LL1A1:
# 830
 Cyc_Tcutil_unify_cmp_exp(_tmp212,e2);
return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209.f2)->tag == 13){_LL1A2: _tmp213=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209.f2)->f2;_LL1A3:
# 833
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp213);
return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f1)->tag == 38){_LL1A4: _tmp214=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f1)->f1;_LL1A5:
# 836
{void*_tmp20A=Cyc_Tcutil_compress(_tmp214);void*_tmp20B=_tmp20A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20B)->tag == 1){_LL1AB: _LL1AC:
# 838
 Cyc_Tcutil_unify_it(_tmp214,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp20D;_tmp20D.tag=18;_tmp20D.f1=e2;_tmp20D;});_tmp20C;}));
return;}else{_LL1AD: _LL1AE:
# 841
 goto _LL1AA;}_LL1AA:;}
# 843
goto _LL19F;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f2)->tag == 38){_LL1A6: _tmp215=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f2)->f1;_LL1A7:
# 845
{void*_tmp20E=Cyc_Tcutil_compress(_tmp215);void*_tmp20F=_tmp20E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20F)->tag == 1){_LL1B0: _LL1B1:
# 847
 Cyc_Tcutil_unify_it(_tmp215,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp211;_tmp211.tag=18;_tmp211.f1=e1;_tmp211;});_tmp210;}));
return;}else{_LL1B2: _LL1B3:
# 850
 goto _LL1AF;}_LL1AF:;}
# 852
goto _LL19F;}else{_LL1A8: _LL1A9:
 goto _LL19F;}}}}_LL19F:;}
# 855
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 858
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp217=({struct _tuple0 _tmp221;_tmp221.f1=b1;_tmp221.f2=b2;_tmp221;});struct _tuple0 _tmp218=_tmp217;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp21F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f2)->tag == 0){_LL1B5: _LL1B6:
 return 0;}else{_LL1B7: _LL1B8:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f2)->tag == 0){_LL1B9: _LL1BA:
 return 1;}else{_LL1BB: _tmp220=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218.f1)->f1;_tmp21F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218.f2)->f1;_LL1BC: {
# 864
int r=Cyc_Evexp_const_exp_cmp(_tmp220,_tmp21F);
if(r == 0)return 0;{
struct _handler_cons _tmp219;_push_handler(& _tmp219);{int _tmp21B=0;if(setjmp(_tmp219.handler))_tmp21B=1;if(!_tmp21B){
Cyc_Tcutil_unify_cmp_exp(_tmp220,_tmp21F);{
int _tmp21C=0;_npop_handler(0);return _tmp21C;};
# 867
;_pop_handler();}else{void*_tmp21A=(void*)_exn_thrown;void*_tmp21D=_tmp21A;void*_tmp21E;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp21D)->tag == Cyc_Tcutil_Unify){_LL1BE: _LL1BF:
# 870
 return r;}else{_LL1C0: _tmp21E=_tmp21D;_LL1C1:(int)_rethrow(_tmp21E);}_LL1BD:;}};};}}}_LL1B4:;}
# 875
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp222=att;switch(*((int*)_tmp222)){case 0: _LL1C3: _LL1C4:
 return 0;case 1: _LL1C5: _LL1C6:
 return 1;case 2: _LL1C7: _LL1C8:
 return 2;case 3: _LL1C9: _LL1CA:
 return 3;case 4: _LL1CB: _LL1CC:
 return 4;case 5: _LL1CD: _LL1CE:
 return 5;case 6: _LL1CF: _LL1D0:
 return 6;case 7: _LL1D1: _LL1D2:
 return 7;case 8: _LL1D3: _LL1D4:
 return 8;case 9: _LL1D5: _LL1D6:
 return 9;case 10: _LL1D7: _LL1D8:
 return 10;case 11: _LL1D9: _LL1DA:
 return 11;case 12: _LL1DB: _LL1DC:
 return 12;case 13: _LL1DD: _LL1DE:
 return 13;case 14: _LL1DF: _LL1E0:
 return 14;case 15: _LL1E1: _LL1E2:
 return 15;case 16: _LL1E3: _LL1E4:
 return 16;case 17: _LL1E5: _LL1E6:
 return 17;case 18: _LL1E7: _LL1E8:
 return 18;case 19: _LL1E9: _LL1EA:
 return 19;case 20: _LL1EB: _LL1EC:
 return 20;default: _LL1ED: _LL1EE:
 return 21;}_LL1C2:;}
# 902
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp223=({struct _tuple0 _tmp235;_tmp235.f1=att1;_tmp235.f2=att2;_tmp235;});struct _tuple0 _tmp224=_tmp223;enum Cyc_Absyn_Format_Type _tmp234;int _tmp233;int _tmp232;enum Cyc_Absyn_Format_Type _tmp231;int _tmp230;int _tmp22F;struct _dyneither_ptr _tmp22E;struct _dyneither_ptr _tmp22D;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Exp*_tmp22B;int _tmp22A;int _tmp229;int _tmp228;int _tmp227;switch(*((int*)_tmp224.f1)){case 0: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 0){_LL1F0: _tmp228=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp227=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_LL1F1:
 _tmp22A=_tmp228;_tmp229=_tmp227;goto _LL1F3;}else{goto _LL1FA;}case 20: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 20){_LL1F2: _tmp22A=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp229=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_LL1F3:
# 906
 return Cyc_Core_intcmp(_tmp22A,_tmp229);}else{goto _LL1FA;}case 6: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 6){_LL1F4: _tmp22C=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp22B=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_LL1F5:
# 908
 if(_tmp22C == _tmp22B)return 0;
if(_tmp22C == 0)return - 1;
if(_tmp22B == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp22C,_tmp22B);}else{goto _LL1FA;}case 8: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 8){_LL1F6: _tmp22E=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp22D=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_LL1F7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp22E,(struct _dyneither_ptr)_tmp22D);}else{goto _LL1FA;}case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 19){_LL1F8: _tmp234=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp233=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f1)->f2;_tmp232=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f1)->f3;_tmp231=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_tmp230=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f2)->f2;_tmp22F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f2)->f3;_LL1F9: {
# 914
int _tmp225=Cyc_Core_intcmp((int)((unsigned int)_tmp234),(int)((unsigned int)_tmp231));
if(_tmp225 != 0)return _tmp225;{
int _tmp226=Cyc_Core_intcmp(_tmp233,_tmp230);
if(_tmp226 != 0)return _tmp226;
return Cyc_Core_intcmp(_tmp232,_tmp22F);};}}else{goto _LL1FA;}default: _LL1FA: _LL1FB:
# 920
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));}_LL1EF:;}
# 924
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 928
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 937
static void*Cyc_Tcutil_rgns_of(void*t);
# 939
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 943
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp236=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp237=_tmp236;switch(((struct Cyc_Absyn_Kind*)_tmp237)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp237)->aliasqual){case Cyc_Absyn_Unique: _LL1FD: _LL1FE:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1FC;case Cyc_Absyn_Aliasable: _LL1FF: _LL200:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1FC;default: goto _LL205;}case Cyc_Absyn_EffKind: _LL201: _LL202:
 t=Cyc_Absyn_empty_effect;goto _LL1FC;case Cyc_Absyn_IntKind: _LL203: _LL204:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp239;_tmp239.tag=18;_tmp239.f1=Cyc_Absyn_uint_exp(0,0);_tmp239;});_tmp238;});goto _LL1FC;default: _LL205: _LL206:
 t=Cyc_Absyn_sint_typ;goto _LL1FC;}_LL1FC:;}
# 952
return({struct _tuple16*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->f1=tv;_tmp23A->f2=t;_tmp23A;});}
# 959
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp23B=Cyc_Tcutil_compress(t);void*_tmp23C=_tmp23B;struct Cyc_List_List*_tmp26C;void*_tmp26B;struct Cyc_List_List*_tmp26A;void*_tmp269;struct Cyc_Absyn_Tqual _tmp268;void*_tmp267;struct Cyc_List_List*_tmp266;struct Cyc_Absyn_VarargInfo*_tmp265;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp261;struct Cyc_List_List*_tmp260;void*_tmp25F;void*_tmp25E;void*_tmp25D;struct Cyc_List_List*_tmp25C;void*_tmp25B;void*_tmp25A;void*_tmp259;switch(*((int*)_tmp23C)){case 0: _LL208: _LL209:
 goto _LL20B;case 7: _LL20A: _LL20B:
 goto _LL20D;case 13: _LL20C: _LL20D:
 goto _LL20F;case 14: _LL20E: _LL20F:
 goto _LL211;case 18: _LL210: _LL211:
 goto _LL213;case 27: _LL212: _LL213:
 goto _LL215;case 28: _LL214: _LL215:
 goto _LL217;case 6: _LL216: _LL217:
 return Cyc_Absyn_empty_effect;case 1: _LL218: _LL219:
 goto _LL21B;case 2: _LL21A: _LL21B: {
# 971
struct Cyc_Absyn_Kind*_tmp23D=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp23E=_tmp23D;switch(((struct Cyc_Absyn_Kind*)_tmp23E)->kind){case Cyc_Absyn_RgnKind: _LL243: _LL244:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp240;_tmp240.tag=23;_tmp240.f1=t;_tmp240;});_tmp23F;});case Cyc_Absyn_EffKind: _LL245: _LL246:
 return t;case Cyc_Absyn_IntKind: _LL247: _LL248:
 return Cyc_Absyn_empty_effect;default: _LL249: _LL24A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp242;_tmp242.tag=25;_tmp242.f1=t;_tmp242;});_tmp241;});}_LL242:;}case 15: _LL21C: _tmp259=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp23C)->f1;_LL21D:
# 977
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp244;_tmp244.tag=23;_tmp244.f1=_tmp259;_tmp244;});_tmp243;});case 16: _LL21E: _tmp25B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23C)->f1;_tmp25A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23C)->f2;_LL21F:
# 981
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp246;_tmp246.tag=23;_tmp246.f1=_tmp25A;_tmp246;});_tmp245;});case 3: _LL220: _tmp25C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL221: {
# 983
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp25C);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp248;_tmp248.tag=24;_tmp248.f1=ts;_tmp248;});_tmp247;}));}case 5: _LL222: _tmp25E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23C)->f1).elt_typ;_tmp25D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23C)->f1).ptr_atts).rgn;_LL223:
# 987
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp24A;_tmp24A.tag=24;_tmp24A.f1=({void*_tmp24B[2];_tmp24B[1]=Cyc_Tcutil_rgns_of(_tmp25E);_tmp24B[0]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp24D;_tmp24D.tag=23;_tmp24D.f1=_tmp25D;_tmp24D;});_tmp24C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp24B,sizeof(void*),2));});_tmp24A;});_tmp249;}));case 8: _LL224: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C)->f1).elt_type;_LL225:
# 989
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp25F));case 10: _LL226: _tmp260=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23C)->f1;_LL227: {
# 991
struct Cyc_List_List*_tmp24E=0;
for(0;_tmp260 != 0;_tmp260=_tmp260->tl){
_tmp24E=({struct Cyc_List_List*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F->hd=(*((struct _tuple12*)_tmp260->hd)).f2;_tmp24F->tl=_tmp24E;_tmp24F;});}
_tmp261=_tmp24E;goto _LL229;}case 4: _LL228: _tmp261=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL229:
 _tmp262=_tmp261;goto _LL22B;case 11: _LL22A: _tmp262=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL22B:
# 997
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp251;_tmp251.tag=24;_tmp251.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp262);_tmp251;});_tmp250;}));case 12: _LL22C: _tmp263=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23C)->f2;_LL22D:
# 999
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp253;_tmp253.tag=24;_tmp253.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp263);_tmp253;});_tmp252;}));case 19: _LL22E: _LL22F:
 return Cyc_Absyn_empty_effect;case 9: _LL230: _tmp26A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).tvars;_tmp269=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).effect;_tmp268=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).ret_tqual;_tmp267=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).ret_typ;_tmp266=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).args;_tmp265=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).cyc_varargs;_tmp264=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).rgn_po;_LL231: {
# 1009
void*_tmp254=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp26A),(void*)_check_null(_tmp269));
return Cyc_Tcutil_normalize_effect(_tmp254);}case 21: _LL232: _LL233:
 goto _LL235;case 22: _LL234: _LL235:
 goto _LL237;case 20: _LL236: _LL237:
 return Cyc_Absyn_empty_effect;case 23: _LL238: _LL239:
 goto _LL23B;case 24: _LL23A: _LL23B:
 return t;case 25: _LL23C: _tmp26B=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp23C)->f1;_LL23D:
 return Cyc_Tcutil_rgns_of(_tmp26B);case 17: _LL23E: _tmp26C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23C)->f2;_LL23F:
# 1018
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp256;_tmp256.tag=24;_tmp256.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp26C);_tmp256;});_tmp255;}));default: _LL240: _LL241:
({void*_tmp257=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp258="typedecl in rgns_of";_tag_dyneither(_tmp258,sizeof(char),20);}),_tag_dyneither(_tmp257,sizeof(void*),0));});}_LL207:;}
# 1026
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp26D=e;void*_tmp279;struct Cyc_List_List**_tmp278;switch(*((int*)_tmp26D)){case 24: _LL24C: _tmp278=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp26D)->f1;_LL24D: {
# 1030
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp278;for(0;effs != 0;effs=effs->tl){
void*_tmp26E=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp26E));{
void*_tmp26F=(void*)effs->hd;void*_tmp270=_tmp26F;switch(*((int*)_tmp270)){case 24: _LL253: _LL254:
 goto _LL256;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp270)->f1)){case 20: _LL255: _LL256:
 redo_join=1;goto _LL252;case 22: _LL257: _LL258:
 redo_join=1;goto _LL252;case 21: _LL259: _LL25A:
 redo_join=1;goto _LL252;default: goto _LL25B;}default: _LL25B: _LL25C:
 goto _LL252;}_LL252:;};}}
# 1042
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp278;for(0;effs != 0;effs=effs->tl){
void*_tmp271=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp272=_tmp271;void*_tmp275;struct Cyc_List_List*_tmp274;switch(*((int*)_tmp272)){case 24: _LL25E: _tmp274=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp272)->f1;_LL25F:
# 1047
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp274,effects);
goto _LL25D;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp272)->f1)){case 20: _LL260: _LL261:
 goto _LL25D;case 22: _LL262: _LL263:
 goto _LL25D;case 21: _LL264: _LL265:
 goto _LL25D;default: goto _LL266;}default: _LL266: _tmp275=_tmp272;_LL267:
 effects=({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=_tmp275;_tmp273->tl=effects;_tmp273;});goto _LL25D;}_LL25D:;}}
# 1055
*_tmp278=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}case 25: _LL24E: _tmp279=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp26D)->f1;_LL24F: {
# 1058
void*_tmp276=Cyc_Tcutil_compress(_tmp279);void*_tmp277=_tmp276;switch(*((int*)_tmp277)){case 1: _LL269: _LL26A:
 goto _LL26C;case 2: _LL26B: _LL26C:
 return e;default: _LL26D: _LL26E:
 return Cyc_Tcutil_rgns_of(_tmp279);}_LL268:;}default: _LL250: _LL251:
# 1063
 return e;}_LL24B:;};}
# 1068
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27A=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp27C;_tmp27C.tag=9;_tmp27C.f1=({struct Cyc_Absyn_FnInfo _tmp27D;_tmp27D.tvars=0;_tmp27D.effect=eff;_tmp27D.ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp27D.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp27D.args=0;_tmp27D.c_varargs=0;_tmp27D.cyc_varargs=0;_tmp27D.rgn_po=0;_tmp27D.attributes=0;_tmp27D.requires_clause=0;_tmp27D.requires_relns=0;_tmp27D.ensures_clause=0;_tmp27D.ensures_relns=0;_tmp27D;});_tmp27C;});_tmp27B;});
# 1078
return Cyc_Absyn_atb_typ((void*)_tmp27A,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1085
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp27E=Cyc_Tcutil_compress(e);void*_tmp27F=_tmp27E;struct Cyc_Core_Opt*_tmp2A4;void**_tmp2A3;struct Cyc_Core_Opt*_tmp2A2;void*_tmp2A1;struct Cyc_List_List*_tmp2A0;void*_tmp29F;switch(*((int*)_tmp27F)){case 23: _LL270: _tmp29F=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp27F)->f1;_LL271:
# 1092
 if(constrain)return Cyc_Tcutil_unify(r,_tmp29F);
_tmp29F=Cyc_Tcutil_compress(_tmp29F);
if(r == _tmp29F)return 1;{
struct _tuple0 _tmp280=({struct _tuple0 _tmp284;_tmp284.f1=r;_tmp284.f2=_tmp29F;_tmp284;});struct _tuple0 _tmp281=_tmp280;struct Cyc_Absyn_Tvar*_tmp283;struct Cyc_Absyn_Tvar*_tmp282;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f2)->tag == 2){_LL27B: _tmp283=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f1)->f1;_tmp282=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f2)->f1;_LL27C:
 return Cyc_Absyn_tvar_cmp(_tmp283,_tmp282)== 0;}else{goto _LL27D;}}else{_LL27D: _LL27E:
 return 0;}_LL27A:;};case 24: _LL272: _tmp2A0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp27F)->f1;_LL273:
# 1100
 for(0;_tmp2A0 != 0;_tmp2A0=_tmp2A0->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp2A0->hd))return 1;}
return 0;case 25: _LL274: _tmp2A1=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp27F)->f1;_LL275: {
# 1104
void*_tmp285=Cyc_Tcutil_rgns_of(_tmp2A1);void*_tmp286=_tmp285;void*_tmp294;void*_tmp293;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp286)->tag == 25){_LL280: _tmp293=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp286)->f1;_LL281:
# 1106
 if(!constrain)return 0;{
void*_tmp287=Cyc_Tcutil_compress(_tmp293);void*_tmp288=_tmp287;struct Cyc_Core_Opt*_tmp292;void**_tmp291;struct Cyc_Core_Opt*_tmp290;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp288)->tag == 1){_LL285: _tmp292=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp288)->f1;_tmp291=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp288)->f2;_tmp290=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp288)->f4;_LL286: {
# 1111
void*_tmp289=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp290);
# 1114
Cyc_Tcutil_occurs(_tmp289,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp290))->v,r);{
void*_tmp28A=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp28C;_tmp28C.tag=24;_tmp28C.f1=({void*_tmp28D[2];_tmp28D[1]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp28F;_tmp28F.tag=23;_tmp28F.f1=r;_tmp28F;});_tmp28E;});_tmp28D[0]=_tmp289;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp28D,sizeof(void*),2));});_tmp28C;});_tmp28B;}));
*_tmp291=_tmp28A;
return 1;};}}else{_LL287: _LL288:
 return 0;}_LL284:;};}else{_LL282: _tmp294=_tmp286;_LL283:
# 1120
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp294);}_LL27F:;}case 1: _LL276: _tmp2A4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f1;_tmp2A3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f2;_tmp2A2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f4;_LL277:
# 1123
 if(_tmp2A4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2A4->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp295=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp296="effect evar has wrong kind";_tag_dyneither(_tmp296,sizeof(char),27);}),_tag_dyneither(_tmp295,sizeof(void*),0));});
if(!constrain)return 0;{
# 1128
void*_tmp297=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A2);
# 1131
Cyc_Tcutil_occurs(_tmp297,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A2))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp298=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp29A;_tmp29A.tag=24;_tmp29A.f1=({struct Cyc_List_List*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->hd=_tmp297;_tmp29B->tl=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp29E;_tmp29E.tag=23;_tmp29E.f1=r;_tmp29E;});_tmp29D;});_tmp29C->tl=0;_tmp29C;});_tmp29B;});_tmp29A;});_tmp299;});
*_tmp2A3=(void*)_tmp298;
return 1;};};default: _LL278: _LL279:
 return 0;}_LL26F:;};}
# 1142
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp2A5=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2A6=_tmp2A5;struct Cyc_Core_Opt*_tmp2BE;void**_tmp2BD;struct Cyc_Core_Opt*_tmp2BC;void*_tmp2BB;struct Cyc_List_List*_tmp2BA;switch(*((int*)_tmp2A6)){case 23: _LL28A: _LL28B:
 return 0;case 24: _LL28C: _tmp2BA=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2A6)->f1;_LL28D:
# 1147
 for(0;_tmp2BA != 0;_tmp2BA=_tmp2BA->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2BA->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2BB=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A6)->f1;_LL28F:
# 1152
 _tmp2BB=Cyc_Tcutil_compress(_tmp2BB);
if(t == _tmp2BB)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2BB);{
void*_tmp2A7=Cyc_Tcutil_rgns_of(t);void*_tmp2A8=_tmp2A7;void*_tmp2AF;void*_tmp2AE;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A8)->tag == 25){_LL295: _tmp2AE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A8)->f1;_LL296: {
# 1157
struct _tuple0 _tmp2A9=({struct _tuple0 _tmp2AD;_tmp2AD.f1=Cyc_Tcutil_compress(_tmp2AE);_tmp2AD.f2=_tmp2BB;_tmp2AD;});struct _tuple0 _tmp2AA=_tmp2A9;struct Cyc_Absyn_Tvar*_tmp2AC;struct Cyc_Absyn_Tvar*_tmp2AB;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f2)->tag == 2){_LL29A: _tmp2AC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f1)->f1;_tmp2AB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f2)->f1;_LL29B:
 return Cyc_Tcutil_unify(t,_tmp2BB);}else{goto _LL29C;}}else{_LL29C: _LL29D:
 return _tmp2AE == _tmp2BB;}_LL299:;}}else{_LL297: _tmp2AF=_tmp2A8;_LL298:
# 1161
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2AF);}_LL294:;};case 1: _LL290: _tmp2BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f1;_tmp2BD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f2;_tmp2BC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f4;_LL291:
# 1164
 if(_tmp2BE == 0  || ((struct Cyc_Absyn_Kind*)_tmp2BE->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2B0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2B1="effect evar has wrong kind";_tag_dyneither(_tmp2B1,sizeof(char),27);}),_tag_dyneither(_tmp2B0,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{
# 1169
void*_tmp2B2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2BC);
# 1172
Cyc_Tcutil_occurs(_tmp2B2,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BC))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B3=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2B5;_tmp2B5.tag=24;_tmp2B5.f1=({struct Cyc_List_List*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->hd=_tmp2B2;_tmp2B6->tl=({struct Cyc_List_List*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2B9;_tmp2B9.tag=25;_tmp2B9.f1=t;_tmp2B9;});_tmp2B8;});_tmp2B7->tl=0;_tmp2B7;});_tmp2B6;});_tmp2B5;});_tmp2B4;});
*_tmp2BD=(void*)_tmp2B3;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1183
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2BF=e;struct Cyc_Core_Opt*_tmp2DF;void**_tmp2DE;struct Cyc_Core_Opt*_tmp2DD;void*_tmp2DC;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Tvar*_tmp2DA;switch(*((int*)_tmp2BF)){case 2: _LL29F: _tmp2DA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BF)->f1;_LL2A0:
 return Cyc_Absyn_tvar_cmp(v,_tmp2DA)== 0;case 24: _LL2A1: _tmp2DB=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2BF)->f1;_LL2A2:
# 1188
 for(0;_tmp2DB != 0;_tmp2DB=_tmp2DB->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2DB->hd))
return 1;}
return 0;case 25: _LL2A3: _tmp2DC=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BF)->f1;_LL2A4: {
# 1193
void*_tmp2C0=Cyc_Tcutil_rgns_of(_tmp2DC);void*_tmp2C1=_tmp2C0;void*_tmp2CF;void*_tmp2CE;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C1)->tag == 25){_LL2AA: _tmp2CE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C1)->f1;_LL2AB:
# 1195
 if(!may_constrain_evars)return 0;{
void*_tmp2C2=Cyc_Tcutil_compress(_tmp2CE);void*_tmp2C3=_tmp2C2;struct Cyc_Core_Opt*_tmp2CD;void**_tmp2CC;struct Cyc_Core_Opt*_tmp2CB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->tag == 1){_LL2AF: _tmp2CD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f1;_tmp2CC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f2;_tmp2CB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f4;_LL2B0: {
# 1201
void*_tmp2C4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2CB);
# 1203
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CB))->v,v))return 0;{
void*_tmp2C5=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2C7;_tmp2C7.tag=24;_tmp2C7.f1=({void*_tmp2C8[2];_tmp2C8[1]=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2CA;_tmp2CA.tag=2;_tmp2CA.f1=v;_tmp2CA;});_tmp2C9;});_tmp2C8[0]=_tmp2C4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2C8,sizeof(void*),2));});_tmp2C7;});_tmp2C6;}));
*_tmp2CC=_tmp2C5;
return 1;};}}else{_LL2B1: _LL2B2:
 return 0;}_LL2AE:;};}else{_LL2AC: _tmp2CF=_tmp2C1;_LL2AD:
# 1209
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2CF);}_LL2A9:;}case 1: _LL2A5: _tmp2DF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f1;_tmp2DE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f2;_tmp2DD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f4;_LL2A6:
# 1212
 if(_tmp2DF == 0  || ((struct Cyc_Absyn_Kind*)_tmp2DF->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2D0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2D1="effect evar has wrong kind";_tag_dyneither(_tmp2D1,sizeof(char),27);}),_tag_dyneither(_tmp2D0,sizeof(void*),0));});{
# 1216
void*_tmp2D2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2DD);
# 1218
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DD))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D3=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2D5;_tmp2D5.tag=24;_tmp2D5.f1=({struct Cyc_List_List*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->hd=_tmp2D2;_tmp2D6->tl=({struct Cyc_List_List*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2D9;_tmp2D9.tag=2;_tmp2D9.f1=v;_tmp2D9;});_tmp2D8;});_tmp2D7->tl=0;_tmp2D7;});_tmp2D6;});_tmp2D5;});_tmp2D4;});
*_tmp2DE=(void*)_tmp2D3;
return 1;};};default: _LL2A7: _LL2A8:
 return 0;}_LL29E:;};}
# 1228
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2E0=e;void*_tmp2E6;struct Cyc_List_List*_tmp2E5;switch(*((int*)_tmp2E0)){case 24: _LL2B4: _tmp2E5=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E0)->f1;_LL2B5:
# 1232
 for(0;_tmp2E5 != 0;_tmp2E5=_tmp2E5->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2E5->hd))
return 1;}
return 0;case 25: _LL2B6: _tmp2E6=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E0)->f1;_LL2B7: {
# 1237
void*_tmp2E1=Cyc_Tcutil_rgns_of(_tmp2E6);void*_tmp2E2=_tmp2E1;void*_tmp2E4;void*_tmp2E3;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E2)->tag == 25){_LL2BD: _tmp2E3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E2)->f1;_LL2BE:
 return 0;}else{_LL2BF: _tmp2E4=_tmp2E2;_LL2C0:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2E4);}_LL2BC:;}case 1: _LL2B8: _LL2B9:
# 1241
 return evar == e;default: _LL2BA: _LL2BB:
 return 0;}_LL2B3:;};}
# 1255 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1260
void*_tmp2E7=Cyc_Tcutil_compress(e1);void*_tmp2E8=_tmp2E7;void**_tmp2F5;struct Cyc_Core_Opt*_tmp2F4;void*_tmp2F3;struct Cyc_Absyn_Tvar*_tmp2F2;void*_tmp2F1;struct Cyc_List_List*_tmp2F0;switch(*((int*)_tmp2E8)){case 24: _LL2C2: _tmp2F0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E8)->f1;_LL2C3:
# 1262
 for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp2F0->hd,e2))
return 0;}
return 1;case 23: _LL2C4: _tmp2F1=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2E8)->f1;_LL2C5:
# 1273
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp2F1,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2F1,(void*)& Cyc_Absyn_HeapRgn_val);case 2: _LL2C6: _tmp2F2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2E8)->f1;_LL2C7:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp2F2,e2);case 25: _LL2C8: _tmp2F3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E8)->f1;_LL2C9: {
# 1277
void*_tmp2E9=Cyc_Tcutil_rgns_of(_tmp2F3);void*_tmp2EA=_tmp2E9;void*_tmp2EC;void*_tmp2EB;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2EA)->tag == 25){_LL2CF: _tmp2EB=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2EA)->f1;_LL2D0:
# 1282
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp2EB,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2EB,Cyc_Absyn_sint_typ);}else{_LL2D1: _tmp2EC=_tmp2EA;_LL2D2:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp2EC,e2);}_LL2CE:;}case 1: _LL2CA: _tmp2F5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E8)->f2;_tmp2F4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E8)->f4;_LL2CB:
# 1287
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1291
*_tmp2F5=Cyc_Absyn_empty_effect;
# 1294
return 1;}else{
# 1296
return 0;}default: _LL2CC: _LL2CD:
({struct Cyc_String_pa_PrintArg_struct _tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));({void*_tmp2ED[1]={& _tmp2EF};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2EE="subset_effect: bad effect: %s";_tag_dyneither(_tmp2EE,sizeof(char),30);}),_tag_dyneither(_tmp2ED,sizeof(void*),1));});});}_LL2C1:;}
# 1312 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1328
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1330
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp2F6=(struct _tuple0*)r1->hd;struct _tuple0*_tmp2F7=_tmp2F6;void*_tmp2FD;void*_tmp2FC;_LL2D4: _tmp2FD=_tmp2F7->f1;_tmp2FC=_tmp2F7->f2;_LL2D5:;{
int found=_tmp2FD == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp2F8=(struct _tuple0*)r2->hd;struct _tuple0*_tmp2F9=_tmp2F8;void*_tmp2FB;void*_tmp2FA;_LL2D7: _tmp2FB=_tmp2F9->f1;_tmp2FA=_tmp2F9->f2;_LL2D8:;
if(Cyc_Tcutil_unify(_tmp2FD,_tmp2FB) && Cyc_Tcutil_unify(_tmp2FC,_tmp2FA)){
found=1;
break;}}}
# 1340
if(!found)return 0;};}}
# 1342
return 1;}
# 1345
typedef struct Cyc_List_List*Cyc_Tcutil_relns_t;
# 1349
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp2FE=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp2FF=({struct Cyc_List_List*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->hd=_tmp2FE;_tmp300->tl=r1;_tmp300;});
if(Cyc_Relations_consistent_relations(_tmp2FF))return 0;}
# 1355
return 1;}
# 1360
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1366
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1368
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1372
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp301=t1;struct Cyc_Core_Opt*_tmp310;void**_tmp30F;struct Cyc_Core_Opt*_tmp30E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp301)->tag == 1){_LL2DA: _tmp310=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp301)->f1;_tmp30F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp301)->f2;_tmp30E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp301)->f4;_LL2DB:
# 1383
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp30E))->v,t2);{
struct Cyc_Absyn_Kind*_tmp302=Cyc_Tcutil_typ_kind(t2);
# 1388
if(Cyc_Tcutil_kind_leq(_tmp302,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp310))->v)){
*_tmp30F=t2;
return;}else{
# 1392
{void*_tmp303=t2;struct Cyc_Absyn_PtrInfo _tmp30C;void**_tmp30B;struct Cyc_Core_Opt*_tmp30A;switch(*((int*)_tmp303)){case 1: _LL2DF: _tmp30B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp303)->f2;_tmp30A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp303)->f4;_LL2E0: {
# 1395
struct Cyc_List_List*_tmp304=(struct Cyc_List_List*)_tmp30E->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp30A))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp304,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp305="(type variable would escape scope)";_tag_dyneither(_tmp305,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1402
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp310->v,_tmp302)){
*_tmp30B=t1;return;}
# 1405
Cyc_Tcutil_failure_reason=({const char*_tmp306="(kinds are incompatible)";_tag_dyneither(_tmp306,sizeof(char),25);});
goto _LL2DE;}case 5: _LL2E1: _tmp30C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp303)->f1;if(((struct Cyc_Absyn_Kind*)_tmp310->v)->kind == Cyc_Absyn_BoxKind){_LL2E2: {
# 1411
union Cyc_Absyn_Constraint*_tmp307=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp30C.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp308=_tmp307;if((((union Cyc_Absyn_Constraint*)_tmp308)->No_constr).tag == 3){_LL2E6: _LL2E7:
# 1415
({struct _union_Constraint_Eq_constr*_tmp309=& _tmp307->Eq_constr;_tmp309->tag=1;_tmp309->val=Cyc_Absyn_bounds_one;});
*_tmp30F=t2;
return;}else{_LL2E8: _LL2E9:
 goto _LL2E5;}_LL2E5:;}
# 1420
goto _LL2DE;}}else{goto _LL2E3;}default: _LL2E3: _LL2E4:
 goto _LL2DE;}_LL2DE:;}
# 1423
Cyc_Tcutil_failure_reason=({const char*_tmp30D="(kinds are incompatible)";_tag_dyneither(_tmp30D,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL2DC: _LL2DD:
# 1426
 goto _LL2D9;}_LL2D9:;}
# 1431
{struct _tuple0 _tmp311=({struct _tuple0 _tmp3D6;_tmp3D6.f1=t2;_tmp3D6.f2=t1;_tmp3D6;});struct _tuple0 _tmp312=_tmp311;void*_tmp3D5;void*_tmp3D4;void*_tmp3D3;void*_tmp3D2;void*_tmp3D1;void*_tmp3D0;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Typedefdecl*_tmp3CE;struct Cyc_List_List*_tmp3CD;struct Cyc_Absyn_Typedefdecl*_tmp3CC;enum Cyc_Absyn_AggrKind _tmp3CB;struct Cyc_List_List*_tmp3CA;enum Cyc_Absyn_AggrKind _tmp3C9;struct Cyc_List_List*_tmp3C8;struct Cyc_List_List*_tmp3C7;struct Cyc_List_List*_tmp3C6;struct Cyc_List_List*_tmp3C5;void*_tmp3C4;struct Cyc_Absyn_Tqual _tmp3C3;void*_tmp3C2;struct Cyc_List_List*_tmp3C1;int _tmp3C0;struct Cyc_Absyn_VarargInfo*_tmp3BF;struct Cyc_List_List*_tmp3BE;struct Cyc_List_List*_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_List_List*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_List_List*_tmp3B8;void*_tmp3B7;struct Cyc_Absyn_Tqual _tmp3B6;void*_tmp3B5;struct Cyc_List_List*_tmp3B4;int _tmp3B3;struct Cyc_Absyn_VarargInfo*_tmp3B2;struct Cyc_List_List*_tmp3B1;struct Cyc_List_List*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_List_List*_tmp3AC;void*_tmp3AB;struct Cyc_Absyn_Tqual _tmp3AA;struct Cyc_Absyn_Exp*_tmp3A9;union Cyc_Absyn_Constraint*_tmp3A8;void*_tmp3A7;struct Cyc_Absyn_Tqual _tmp3A6;struct Cyc_Absyn_Exp*_tmp3A5;union Cyc_Absyn_Constraint*_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A2;void*_tmp3A1;void*_tmp3A0;int _tmp39F;int _tmp39E;enum Cyc_Absyn_Sign _tmp39D;enum Cyc_Absyn_Size_of _tmp39C;enum Cyc_Absyn_Sign _tmp39B;enum Cyc_Absyn_Size_of _tmp39A;void*_tmp399;struct Cyc_Absyn_Tqual _tmp398;void*_tmp397;union Cyc_Absyn_Constraint*_tmp396;union Cyc_Absyn_Constraint*_tmp395;union Cyc_Absyn_Constraint*_tmp394;void*_tmp393;struct Cyc_Absyn_Tqual _tmp392;void*_tmp391;union Cyc_Absyn_Constraint*_tmp390;union Cyc_Absyn_Constraint*_tmp38F;union Cyc_Absyn_Constraint*_tmp38E;struct Cyc_Absyn_Datatypedecl*_tmp38D;struct Cyc_Absyn_Datatypefield*_tmp38C;struct Cyc_List_List*_tmp38B;struct Cyc_Absyn_Datatypedecl*_tmp38A;struct Cyc_Absyn_Datatypefield*_tmp389;struct Cyc_List_List*_tmp388;struct Cyc_Absyn_Datatypedecl*_tmp387;struct Cyc_List_List*_tmp386;struct Cyc_Absyn_Datatypedecl*_tmp385;struct Cyc_List_List*_tmp384;struct Cyc_List_List*_tmp383;struct Cyc_List_List*_tmp382;struct _tuple2*_tmp381;struct _tuple2*_tmp380;union Cyc_Absyn_AggrInfoU _tmp37F;struct Cyc_List_List*_tmp37E;union Cyc_Absyn_AggrInfoU _tmp37D;struct Cyc_List_List*_tmp37C;struct Cyc_Absyn_Tvar*_tmp37B;struct Cyc_Absyn_Tvar*_tmp37A;switch(*((int*)_tmp312.f1)){case 1: _LL2EB: _LL2EC:
# 1434
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0: switch(*((int*)_tmp312.f2)){case 0: _LL2ED: _LL2EE:
# 1437
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 2: switch(*((int*)_tmp312.f2)){case 2: _LL2EF: _tmp37B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp37A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL2F0: {
# 1440
struct _dyneither_ptr*_tmp313=_tmp37B->name;
struct _dyneither_ptr*_tmp314=_tmp37A->name;
# 1443
int _tmp315=_tmp37B->identity;
int _tmp316=_tmp37A->identity;
if(_tmp316 == _tmp315)return;
Cyc_Tcutil_failure_reason=({const char*_tmp317="(variable types are not the same)";_tag_dyneither(_tmp317,sizeof(char),34);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 11: switch(*((int*)_tmp312.f2)){case 11: _LL2F1: _tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f1)->f1).aggr_info;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp37D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f2)->f1).aggr_info;_tmp37C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2F2: {
# 1450
struct _tuple13 _tmp318=Cyc_Absyn_aggr_kinded_name(_tmp37D);struct _tuple13 _tmp319=_tmp318;enum Cyc_Absyn_AggrKind _tmp321;struct _tuple2*_tmp320;_LL328: _tmp321=_tmp319.f1;_tmp320=_tmp319.f2;_LL329:;{
struct _tuple13 _tmp31A=Cyc_Absyn_aggr_kinded_name(_tmp37F);struct _tuple13 _tmp31B=_tmp31A;enum Cyc_Absyn_AggrKind _tmp31F;struct _tuple2*_tmp31E;_LL32B: _tmp31F=_tmp31B.f1;_tmp31E=_tmp31B.f2;_LL32C:;
if(_tmp321 != _tmp31F){Cyc_Tcutil_failure_reason=({const char*_tmp31C="(struct and union type)";_tag_dyneither(_tmp31C,sizeof(char),24);});goto _LL2EA;}
if(Cyc_Absyn_qvar_cmp(_tmp320,_tmp31E)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp31D="(different type name)";_tag_dyneither(_tmp31D,sizeof(char),22);});goto _LL2EA;}
Cyc_Tcutil_unify_list(_tmp37C,_tmp37E);
return;};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 13: switch(*((int*)_tmp312.f2)){case 13: _LL2F3: _tmp381=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp380=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL2F4:
# 1459
 if(Cyc_Absyn_qvar_cmp(_tmp381,_tmp380)== 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp322="(different enum types)";_tag_dyneither(_tmp322,sizeof(char),23);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 14: switch(*((int*)_tmp312.f2)){case 14: _LL2F5: _tmp383=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp382=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL2F6: {
# 1464
int bad=0;
for(0;_tmp383 != 0  && _tmp382 != 0;(_tmp383=_tmp383->tl,_tmp382=_tmp382->tl)){
struct Cyc_Absyn_Enumfield*_tmp323=(struct Cyc_Absyn_Enumfield*)_tmp383->hd;
struct Cyc_Absyn_Enumfield*_tmp324=(struct Cyc_Absyn_Enumfield*)_tmp382->hd;
if(Cyc_Absyn_qvar_cmp(_tmp323->name,_tmp324->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp325="(different names for enum fields)";_tag_dyneither(_tmp325,sizeof(char),34);});
bad=1;
break;}
# 1473
if(_tmp323->tag == _tmp324->tag)continue;
if(_tmp323->tag == 0  || _tmp324->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp326="(different tag values for enum fields)";_tag_dyneither(_tmp326,sizeof(char),39);});
bad=1;
break;}
# 1479
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp323->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp324->tag))){
Cyc_Tcutil_failure_reason=({const char*_tmp327="(different tag values for enum fields)";_tag_dyneither(_tmp327,sizeof(char),39);});
bad=1;
break;}}
# 1485
if(bad)goto _LL2EA;
if(_tmp383 == 0  && _tmp382 == 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp328="(different number of fields for enums)";_tag_dyneither(_tmp328,sizeof(char),39);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp312.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2F7: _tmp387=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).datatype_info).KnownDatatype).val;_tmp386=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp385=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).datatype_info).KnownDatatype).val;_tmp384=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2F8:
# 1492
 if(_tmp387 == _tmp385  || Cyc_Absyn_qvar_cmp(_tmp387->name,_tmp385->name)== 0){
Cyc_Tcutil_unify_list(_tmp384,_tmp386);
return;}
# 1496
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp312.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp312.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2F9: _tmp38D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp38C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp38B=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp38A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp389=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp388=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2FA:
# 1500
 if((_tmp38D == _tmp38A  || Cyc_Absyn_qvar_cmp(_tmp38D->name,_tmp38A->name)== 0) && (
_tmp38C == _tmp389  || Cyc_Absyn_qvar_cmp(_tmp38C->name,_tmp389->name)== 0)){
Cyc_Tcutil_unify_list(_tmp388,_tmp38B);
return;}
# 1505
Cyc_Tcutil_failure_reason=({const char*_tmp329="(different datatype field types)";_tag_dyneither(_tmp329,sizeof(char),33);});
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp312.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 5: switch(*((int*)_tmp312.f2)){case 5: _LL2FB: _tmp399=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_typ;_tmp398=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_tq;_tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).rgn;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).nullable;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).bounds;_tmp394=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).zero_term;_tmp393=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_typ;_tmp392=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_tq;_tmp391=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).rgn;_tmp390=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).nullable;_tmp38F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).bounds;_tmp38E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).zero_term;_LL2FC:
# 1510
 Cyc_Tcutil_unify_it(_tmp393,_tmp399);
Cyc_Tcutil_unify_it(_tmp397,_tmp391);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp38E,_tmp394,({const char*_tmp32A="(not both zero terminated)";_tag_dyneither(_tmp32A,sizeof(char),27);}));
Cyc_Tcutil_unify_tqual(_tmp392,_tmp393,_tmp398,_tmp399);
((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp38F,_tmp395,({const char*_tmp32B="(different pointer bounds)";_tag_dyneither(_tmp32B,sizeof(char),27);}));
# 1518
{void*_tmp32C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp38F);void*_tmp32D=_tmp32C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp32D)->tag == 0){_LL32E: _LL32F:
 return;}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;}
# 1522
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp390,_tmp396,({const char*_tmp32E="(incompatible pointer types)";_tag_dyneither(_tmp32E,sizeof(char),29);}));
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 6: switch(*((int*)_tmp312.f2)){case 6: _LL2FD: _tmp39D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp39C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp39B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f2)->f1;_tmp39A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f2)->f2;_LL2FE:
# 1527
 if(_tmp39B == _tmp39D  && ((_tmp39A == _tmp39C  || _tmp39A == Cyc_Absyn_Int_sz  && _tmp39C == Cyc_Absyn_Long_sz) || 
_tmp39A == Cyc_Absyn_Long_sz  && _tmp39C == Cyc_Absyn_Int_sz))return;
Cyc_Tcutil_failure_reason=({const char*_tmp32F="(different integral types)";_tag_dyneither(_tmp32F,sizeof(char),27);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 7: switch(*((int*)_tmp312.f2)){case 7: _LL2FF: _tmp39F=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp39E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL300:
# 1533
 if(_tmp39E == 0  && _tmp39F == 0)return;else{
if(_tmp39E == 1  && _tmp39F == 1)return;else{
# 1536
if(((_tmp39E != 0  && _tmp39E != 1) && _tmp39F != 0) && _tmp39F != 1)return;}}
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 19: switch(*((int*)_tmp312.f2)){case 19: _LL301: _tmp3A1=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3A0=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL302:
# 1539
 Cyc_Tcutil_unify_it(_tmp3A1,_tmp3A0);return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 18: switch(*((int*)_tmp312.f2)){case 18: _LL303: _tmp3A3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3A2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL304:
# 1542
 if(!Cyc_Evexp_same_const_exp(_tmp3A3,_tmp3A2)){
Cyc_Tcutil_failure_reason=({const char*_tmp330="(cannot prove expressions are the same)";_tag_dyneither(_tmp330,sizeof(char),40);});
goto _LL2EA;}
# 1546
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 8: switch(*((int*)_tmp312.f2)){case 8: _LL305: _tmp3AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_type;_tmp3AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).tq;_tmp3A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).num_elts;_tmp3A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).zero_term;_tmp3A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_type;_tmp3A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).tq;_tmp3A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).num_elts;_tmp3A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).zero_term;_LL306:
# 1550
 Cyc_Tcutil_unify_it(_tmp3A7,_tmp3AB);
Cyc_Tcutil_unify_tqual(_tmp3A6,_tmp3A7,_tmp3AA,_tmp3AB);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3A8,_tmp3A4,({const char*_tmp331="(not both zero terminated)";_tag_dyneither(_tmp331,sizeof(char),27);}));
if(_tmp3A9 == _tmp3A5)return;
if(_tmp3A9 == 0  || _tmp3A5 == 0)goto _LL2EA;
if(Cyc_Evexp_same_const_exp(_tmp3A9,_tmp3A5))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp332="(different array sizes)";_tag_dyneither(_tmp332,sizeof(char),24);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 9: switch(*((int*)_tmp312.f2)){case 9: _LL307: _tmp3C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).tvars;_tmp3C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).effect;_tmp3C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).ret_tqual;_tmp3C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).ret_typ;_tmp3C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).args;_tmp3C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).c_varargs;_tmp3BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).cyc_varargs;_tmp3BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).rgn_po;_tmp3BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).attributes;_tmp3BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).requires_clause;_tmp3BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).requires_relns;_tmp3BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).ensures_clause;_tmp3B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).ensures_relns;_tmp3B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).tvars;_tmp3B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).effect;_tmp3B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).ret_tqual;_tmp3B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).ret_typ;_tmp3B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).args;_tmp3B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).c_varargs;_tmp3B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).cyc_varargs;_tmp3B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).rgn_po;_tmp3B0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).attributes;_tmp3AF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).requires_clause;_tmp3AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).requires_relns;_tmp3AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).ensures_clause;_tmp3AC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).ensures_relns;_LL308: {
# 1564
int done=0;
{struct _RegionHandle _tmp333=_new_region("rgn");struct _RegionHandle*rgn=& _tmp333;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp3B8 != 0){
if(_tmp3C5 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp334="(second function type has too few type variables)";_tag_dyneither(_tmp334,sizeof(char),50);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1572
void*_tmp335=((struct Cyc_Absyn_Tvar*)_tmp3B8->hd)->kind;
void*_tmp336=((struct Cyc_Absyn_Tvar*)_tmp3C5->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp335,_tmp336)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33B;_tmp33B.tag=0;_tmp33B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1578
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3C5->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp33A;_tmp33A.tag=0;_tmp33A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1577
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3B8->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp339;_tmp339.tag=0;_tmp339.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1576
Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp3B8->hd));({void*_tmp337[3]={& _tmp339,& _tmp33A,& _tmp33B};Cyc_aprintf(({const char*_tmp338="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp338,sizeof(char),44);}),_tag_dyneither(_tmp337,sizeof(void*),3));});});});});
# 1579
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1581
inst=({struct Cyc_List_List*_tmp33C=_region_malloc(rgn,sizeof(*_tmp33C));_tmp33C->hd=({struct _tuple16*_tmp33D=_region_malloc(rgn,sizeof(*_tmp33D));_tmp33D->f1=(struct Cyc_Absyn_Tvar*)_tmp3C5->hd;_tmp33D->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp33F;_tmp33F.tag=2;_tmp33F.f1=(struct Cyc_Absyn_Tvar*)_tmp3B8->hd;_tmp33F;});_tmp33E;});_tmp33D;});_tmp33C->tl=inst;_tmp33C;});
_tmp3B8=_tmp3B8->tl;
_tmp3C5=_tmp3C5->tl;};}
# 1585
if(_tmp3C5 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp340="(second function type has too many type variables)";_tag_dyneither(_tmp340,sizeof(char),51);});
_npop_handler(0);goto _LL2EA;}
# 1589
if(inst != 0){
Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp342;_tmp342.tag=9;_tmp342.f1=({struct Cyc_Absyn_FnInfo _tmp343;_tmp343.tvars=0;_tmp343.effect=_tmp3B7;_tmp343.ret_tqual=_tmp3B6;_tmp343.ret_typ=_tmp3B5;_tmp343.args=_tmp3B4;_tmp343.c_varargs=_tmp3B3;_tmp343.cyc_varargs=_tmp3B2;_tmp343.rgn_po=_tmp3B1;_tmp343.attributes=_tmp3B0;_tmp343.requires_clause=_tmp3BC;_tmp343.requires_relns=_tmp3BB;_tmp343.ensures_clause=_tmp3BA;_tmp343.ensures_relns=_tmp3B9;_tmp343;});_tmp342;});_tmp341;}),
# 1593
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp345;_tmp345.tag=9;_tmp345.f1=({struct Cyc_Absyn_FnInfo _tmp346;_tmp346.tvars=0;_tmp346.effect=_tmp3C4;_tmp346.ret_tqual=_tmp3C3;_tmp346.ret_typ=_tmp3C2;_tmp346.args=_tmp3C1;_tmp346.c_varargs=_tmp3C0;_tmp346.cyc_varargs=_tmp3BF;_tmp346.rgn_po=_tmp3BE;_tmp346.attributes=_tmp3BD;_tmp346.requires_clause=_tmp3AF;_tmp346.requires_relns=_tmp3AE;_tmp346.ensures_clause=_tmp3AD;_tmp346.ensures_relns=_tmp3AC;_tmp346;});_tmp345;});_tmp344;})));
# 1598
done=1;}}
# 1566
;_pop_region(rgn);}
# 1601
if(done)
return;
Cyc_Tcutil_unify_it(_tmp3B5,_tmp3C2);
Cyc_Tcutil_unify_tqual(_tmp3B6,_tmp3B5,_tmp3C3,_tmp3C2);
for(0;_tmp3B4 != 0  && _tmp3C1 != 0;(_tmp3B4=_tmp3B4->tl,_tmp3C1=_tmp3C1->tl)){
struct _tuple10 _tmp347=*((struct _tuple10*)_tmp3B4->hd);struct _tuple10 _tmp348=_tmp347;struct Cyc_Absyn_Tqual _tmp34E;void*_tmp34D;_LL333: _tmp34E=_tmp348.f2;_tmp34D=_tmp348.f3;_LL334:;{
struct _tuple10 _tmp349=*((struct _tuple10*)_tmp3C1->hd);struct _tuple10 _tmp34A=_tmp349;struct Cyc_Absyn_Tqual _tmp34C;void*_tmp34B;_LL336: _tmp34C=_tmp34A.f2;_tmp34B=_tmp34A.f3;_LL337:;
Cyc_Tcutil_unify_it(_tmp34D,_tmp34B);
Cyc_Tcutil_unify_tqual(_tmp34E,_tmp34D,_tmp34C,_tmp34B);};}
# 1611
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp3B4 != 0  || _tmp3C1 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp34F="(function types have different number of arguments)";_tag_dyneither(_tmp34F,sizeof(char),52);});
goto _LL2EA;}
# 1617
if(_tmp3B3 != _tmp3C0){
Cyc_Tcutil_failure_reason=({const char*_tmp350="(only one function type takes C varargs)";_tag_dyneither(_tmp350,sizeof(char),41);});
goto _LL2EA;}{
# 1622
int bad_cyc_vararg=0;
{struct _tuple22 _tmp351=({struct _tuple22 _tmp35D;_tmp35D.f1=_tmp3B2;_tmp35D.f2=_tmp3BF;_tmp35D;});struct _tuple22 _tmp352=_tmp351;struct _dyneither_ptr*_tmp35C;struct Cyc_Absyn_Tqual _tmp35B;void*_tmp35A;int _tmp359;struct _dyneither_ptr*_tmp358;struct Cyc_Absyn_Tqual _tmp357;void*_tmp356;int _tmp355;if(_tmp352.f1 == 0){if(_tmp352.f2 == 0){_LL339: _LL33A:
 goto _LL338;}else{_LL33B: _LL33C:
 goto _LL33E;}}else{if(_tmp352.f2 == 0){_LL33D: _LL33E:
# 1627
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp353="(only one function type takes varargs)";_tag_dyneither(_tmp353,sizeof(char),39);});
goto _LL338;}else{_LL33F: _tmp35C=(_tmp352.f1)->name;_tmp35B=(_tmp352.f1)->tq;_tmp35A=(_tmp352.f1)->type;_tmp359=(_tmp352.f1)->inject;_tmp358=(_tmp352.f2)->name;_tmp357=(_tmp352.f2)->tq;_tmp356=(_tmp352.f2)->type;_tmp355=(_tmp352.f2)->inject;_LL340:
# 1631
 Cyc_Tcutil_unify_it(_tmp35A,_tmp356);
Cyc_Tcutil_unify_tqual(_tmp35B,_tmp35A,_tmp357,_tmp356);
if(_tmp359 != _tmp355){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp354="(only one function type injects varargs)";_tag_dyneither(_tmp354,sizeof(char),41);});}
# 1637
goto _LL338;}}_LL338:;}
# 1639
if(bad_cyc_vararg)goto _LL2EA;{
# 1642
int bad_effect=0;
{struct _tuple0 _tmp35E=({struct _tuple0 _tmp360;_tmp360.f1=_tmp3B7;_tmp360.f2=_tmp3C4;_tmp360;});struct _tuple0 _tmp35F=_tmp35E;if(_tmp35F.f1 == 0){if(_tmp35F.f2 == 0){_LL342: _LL343:
 goto _LL341;}else{_LL344: _LL345:
 goto _LL347;}}else{if(_tmp35F.f2 == 0){_LL346: _LL347:
 bad_effect=1;goto _LL341;}else{_LL348: _LL349:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp3B7),(void*)_check_null(_tmp3C4));goto _LL341;}}_LL341:;}
# 1649
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp361="(function effects do not match)";_tag_dyneither(_tmp361,sizeof(char),32);});
goto _LL2EA;}
# 1655
if(!Cyc_Tcutil_same_atts(_tmp3BD,_tmp3B0)){
Cyc_Tcutil_failure_reason=({const char*_tmp362="(function types have different attributes)";_tag_dyneither(_tmp362,sizeof(char),43);});
goto _LL2EA;}
# 1659
if(!Cyc_Tcutil_same_rgn_po(_tmp3BE,_tmp3B1)){
Cyc_Tcutil_failure_reason=({const char*_tmp363="(function types have different region lifetime orderings)";_tag_dyneither(_tmp363,sizeof(char),58);});
goto _LL2EA;}
# 1663
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3BB,_tmp3AE)){
Cyc_Tcutil_failure_reason=({const char*_tmp364="(@requires clauses not equivalent)";_tag_dyneither(_tmp364,sizeof(char),35);});
goto _LL2EA;}
# 1667
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3B9,_tmp3AC)){
Cyc_Tcutil_failure_reason=({const char*_tmp365="(@ensures clauses not equivalent)";_tag_dyneither(_tmp365,sizeof(char),34);});
goto _LL2EA;}
# 1671
return;};};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 10: switch(*((int*)_tmp312.f2)){case 10: _LL309: _tmp3C7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3C6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL30A:
# 1674
 for(0;_tmp3C6 != 0  && _tmp3C7 != 0;(_tmp3C6=_tmp3C6->tl,_tmp3C7=_tmp3C7->tl)){
struct _tuple12 _tmp366=*((struct _tuple12*)_tmp3C6->hd);struct _tuple12 _tmp367=_tmp366;struct Cyc_Absyn_Tqual _tmp36D;void*_tmp36C;_LL34B: _tmp36D=_tmp367.f1;_tmp36C=_tmp367.f2;_LL34C:;{
struct _tuple12 _tmp368=*((struct _tuple12*)_tmp3C7->hd);struct _tuple12 _tmp369=_tmp368;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36A;_LL34E: _tmp36B=_tmp369.f1;_tmp36A=_tmp369.f2;_LL34F:;
Cyc_Tcutil_unify_it(_tmp36C,_tmp36A);
Cyc_Tcutil_unify_tqual(_tmp36D,_tmp36C,_tmp36B,_tmp36A);};}
# 1680
if(_tmp3C6 == 0  && _tmp3C7 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp36E="(tuple types have different numbers of components)";_tag_dyneither(_tmp36E,sizeof(char),51);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 12: switch(*((int*)_tmp312.f2)){case 12: _LL30B: _tmp3CB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3CA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp3C9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f2)->f1;_tmp3C8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f2)->f2;_LL30C:
# 1687
 if(_tmp3C9 != _tmp3CB){Cyc_Tcutil_failure_reason=({const char*_tmp36F="(struct and union type)";_tag_dyneither(_tmp36F,sizeof(char),24);});goto _LL2EA;}
for(0;_tmp3C8 != 0  && _tmp3CA != 0;(_tmp3C8=_tmp3C8->tl,_tmp3CA=_tmp3CA->tl)){
struct Cyc_Absyn_Aggrfield*_tmp370=(struct Cyc_Absyn_Aggrfield*)_tmp3C8->hd;
struct Cyc_Absyn_Aggrfield*_tmp371=(struct Cyc_Absyn_Aggrfield*)_tmp3CA->hd;
if(Cyc_strptrcmp(_tmp370->name,_tmp371->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp372="(different member names)";_tag_dyneither(_tmp372,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1695
Cyc_Tcutil_unify_it(_tmp370->type,_tmp371->type);
Cyc_Tcutil_unify_tqual(_tmp370->tq,_tmp370->type,_tmp371->tq,_tmp371->type);
if(!Cyc_Tcutil_same_atts(_tmp370->attributes,_tmp371->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp373="(different attributes on member)";_tag_dyneither(_tmp373,sizeof(char),33);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1703
if((_tmp370->width != 0  && _tmp371->width == 0  || 
_tmp371->width != 0  && _tmp370->width == 0) || 
(_tmp370->width != 0  && _tmp371->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp370->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp371->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp374="(different bitfield widths on member)";_tag_dyneither(_tmp374,sizeof(char),38);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1712
if((_tmp370->requires_clause != 0  && _tmp371->requires_clause == 0  || 
_tmp370->requires_clause == 0  && _tmp371->requires_clause != 0) || 
(_tmp370->requires_clause == 0  && _tmp371->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp370->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp371->requires_clause))){
# 1717
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp375="(different @requires clauses on member)";_tag_dyneither(_tmp375,sizeof(char),40);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1723
if(_tmp3C8 == 0  && _tmp3CA == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp376="(different number of members)";_tag_dyneither(_tmp376,sizeof(char),30);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 17: switch(*((int*)_tmp312.f2)){case 17: _LL30D: _tmp3CF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp3CE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f1)->f3;_tmp3CD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f2)->f2;_tmp3CC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f2)->f3;_LL30E:
# 1729
 if(_tmp3CE != _tmp3CC){
Cyc_Tcutil_failure_reason=({const char*_tmp377="(different abstract typedefs)";_tag_dyneither(_tmp377,sizeof(char),30);});
goto _LL2EA;}
# 1733
Cyc_Tcutil_failure_reason=({const char*_tmp378="(type parameters to typedef differ)";_tag_dyneither(_tmp378,sizeof(char),36);});
Cyc_Tcutil_unify_list(_tmp3CF,_tmp3CD);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 20: switch(*((int*)_tmp312.f2)){case 20: _LL30F: _LL310:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 21: switch(*((int*)_tmp312.f2)){case 21: _LL311: _LL312:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 22: switch(*((int*)_tmp312.f2)){case 22: _LL313: _LL314:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 15: switch(*((int*)_tmp312.f2)){case 15: _LL315: _tmp3D1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3D0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL316:
# 1740
 Cyc_Tcutil_unify_it(_tmp3D1,_tmp3D0);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 16: switch(*((int*)_tmp312.f2)){case 16: _LL317: _tmp3D5=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3D4=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp3D3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312.f2)->f1;_tmp3D2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312.f2)->f2;_LL318:
# 1743
 Cyc_Tcutil_unify_it(_tmp3D5,_tmp3D3);
Cyc_Tcutil_unify_it(_tmp3D4,_tmp3D2);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 24: _LL319: _LL31A:
 goto _LL31C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp312.f2)->tag == 24){_LL31B: _LL31C:
 goto _LL31E;}else{switch(*((int*)_tmp312.f1)){case 23: _LL31D: _LL31E:
 goto _LL320;case 25: _LL31F: _LL320:
 goto _LL322;default: switch(*((int*)_tmp312.f2)){case 25: _LL321: _LL322:
 goto _LL324;case 23: _LL323: _LL324:
# 1752
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp379="(effects don't unify)";_tag_dyneither(_tmp379,sizeof(char),22);});
goto _LL2EA;default: _LL325: _LL326:
 goto _LL2EA;}}}}_LL2EA:;}
# 1757
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1760
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1767
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3D7=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3D8=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3D7,_tmp3D8);}
# 1774
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3D9=x;void*_tmp3E0;switch((((union Cyc_Absyn_Constraint*)_tmp3D9)->Eq_constr).tag){case 3: _LL351: _LL352:
 return - 1;case 1: _LL353: _tmp3E0=(void*)(_tmp3D9->Eq_constr).val;_LL354: {
# 1781
union Cyc_Absyn_Constraint*_tmp3DA=y;void*_tmp3DD;switch((((union Cyc_Absyn_Constraint*)_tmp3DA)->Eq_constr).tag){case 3: _LL358: _LL359:
 return 1;case 1: _LL35A: _tmp3DD=(void*)(_tmp3DA->Eq_constr).val;_LL35B:
 return cmp(_tmp3E0,_tmp3DD);default: _LL35C: _LL35D:
({void*_tmp3DB=0;Cyc_Tcutil_impos(({const char*_tmp3DC="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3DC,sizeof(char),40);}),_tag_dyneither(_tmp3DB,sizeof(void*),0));});}_LL357:;}default: _LL355: _LL356:
# 1786
({void*_tmp3DE=0;Cyc_Tcutil_impos(({const char*_tmp3DF="unify_conref: forward after compress";_tag_dyneither(_tmp3DF,sizeof(char),37);}),_tag_dyneither(_tmp3DE,sizeof(void*),0));});}_LL350:;};}
# 1790
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp3E1=tqt1;struct Cyc_Absyn_Tqual _tmp3E7;void*_tmp3E6;_LL35F: _tmp3E7=_tmp3E1->f1;_tmp3E6=_tmp3E1->f2;_LL360:;{
struct _tuple12*_tmp3E2=tqt2;struct Cyc_Absyn_Tqual _tmp3E5;void*_tmp3E4;_LL362: _tmp3E5=_tmp3E2->f1;_tmp3E4=_tmp3E2->f2;_LL363:;{
int _tmp3E3=Cyc_Tcutil_tqual_cmp(_tmp3E7,_tmp3E5);
if(_tmp3E3 != 0)return _tmp3E3;
return Cyc_Tcutil_typecmp(_tmp3E6,_tmp3E4);};};}
# 1798
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp3E8=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp3E8 != 0)return _tmp3E8;{
int _tmp3E9=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp3E9 != 0)return _tmp3E9;{
int _tmp3EA=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp3EA != 0)return _tmp3EA;{
int _tmp3EB=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp3EB != 0)return _tmp3EB;
_tmp3EB=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp3EB != 0)return _tmp3EB;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1812
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp3EC=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3EC != 0)return _tmp3EC;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1818
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp3ED=t;switch(*((int*)_tmp3ED)){case 0: _LL365: _LL366:
 return 0;case 1: _LL367: _LL368:
 return 1;case 2: _LL369: _LL36A:
 return 2;case 3: _LL36B: _LL36C:
 return 3;case 4: _LL36D: _LL36E:
 return 4;case 5: _LL36F: _LL370:
 return 5;case 6: _LL371: _LL372:
 return 6;case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3ED)->f1 == 0){_LL373: _LL374:
 return 7;}else{_LL375: _LL376:
 return 8;}case 8: _LL377: _LL378:
 return 9;case 9: _LL379: _LL37A:
 return 10;case 10: _LL37B: _LL37C:
 return 11;case 11: _LL37D: _LL37E:
 return 12;case 12: _LL37F: _LL380:
 return 14;case 13: _LL381: _LL382:
 return 16;case 14: _LL383: _LL384:
 return 17;case 15: _LL385: _LL386:
 return 18;case 17: _LL387: _LL388:
 return 19;case 21: _LL389: _LL38A:
 return 20;case 20: _LL38B: _LL38C:
 return 21;case 23: _LL38D: _LL38E:
 return 22;case 24: _LL38F: _LL390:
 return 23;case 25: _LL391: _LL392:
 return 24;case 22: _LL393: _LL394:
 return 25;case 19: _LL395: _LL396:
 return 26;case 16: _LL397: _LL398:
 return 27;case 18: _LL399: _LL39A:
 return 28;case 26: _LL39B: _LL39C:
 return 29;case 27: _LL39D: _LL39E:
 return 30;default: _LL39F: _LL3A0:
 return 31;}_LL364:;}
# 1855
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp3EE=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp3EE != 0)
return _tmp3EE;{
# 1864
struct _tuple0 _tmp3EF=({struct _tuple0 _tmp452;_tmp452.f1=t2;_tmp452.f2=t1;_tmp452;});struct _tuple0 _tmp3F0=_tmp3EF;struct _dyneither_ptr _tmp451;struct _dyneither_ptr _tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*_tmp44C;void*_tmp44B;void*_tmp44A;void*_tmp449;void*_tmp448;void*_tmp447;void*_tmp446;void*_tmp445;void*_tmp444;enum Cyc_Absyn_AggrKind _tmp443;struct Cyc_List_List*_tmp442;enum Cyc_Absyn_AggrKind _tmp441;struct Cyc_List_List*_tmp440;struct Cyc_List_List*_tmp43F;struct Cyc_List_List*_tmp43E;void*_tmp43D;struct Cyc_Absyn_Tqual _tmp43C;struct Cyc_Absyn_Exp*_tmp43B;union Cyc_Absyn_Constraint*_tmp43A;void*_tmp439;struct Cyc_Absyn_Tqual _tmp438;struct Cyc_Absyn_Exp*_tmp437;union Cyc_Absyn_Constraint*_tmp436;int _tmp435;int _tmp434;enum Cyc_Absyn_Sign _tmp433;enum Cyc_Absyn_Size_of _tmp432;enum Cyc_Absyn_Sign _tmp431;enum Cyc_Absyn_Size_of _tmp430;void*_tmp42F;struct Cyc_Absyn_Tqual _tmp42E;void*_tmp42D;union Cyc_Absyn_Constraint*_tmp42C;union Cyc_Absyn_Constraint*_tmp42B;union Cyc_Absyn_Constraint*_tmp42A;void*_tmp429;struct Cyc_Absyn_Tqual _tmp428;void*_tmp427;union Cyc_Absyn_Constraint*_tmp426;union Cyc_Absyn_Constraint*_tmp425;union Cyc_Absyn_Constraint*_tmp424;struct Cyc_Absyn_Datatypedecl*_tmp423;struct Cyc_Absyn_Datatypefield*_tmp422;struct Cyc_List_List*_tmp421;struct Cyc_Absyn_Datatypedecl*_tmp420;struct Cyc_Absyn_Datatypefield*_tmp41F;struct Cyc_List_List*_tmp41E;struct Cyc_Absyn_Datatypedecl*_tmp41D;struct Cyc_List_List*_tmp41C;struct Cyc_Absyn_Datatypedecl*_tmp41B;struct Cyc_List_List*_tmp41A;struct Cyc_List_List*_tmp419;struct Cyc_List_List*_tmp418;struct _tuple2*_tmp417;struct _tuple2*_tmp416;union Cyc_Absyn_AggrInfoU _tmp415;struct Cyc_List_List*_tmp414;union Cyc_Absyn_AggrInfoU _tmp413;struct Cyc_List_List*_tmp412;struct Cyc_Absyn_Tvar*_tmp411;struct Cyc_Absyn_Tvar*_tmp410;switch(*((int*)_tmp3F0.f1)){case 1: switch(*((int*)_tmp3F0.f2)){case 1: _LL3A2: _LL3A3:
# 1866
({void*_tmp3F1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3F2="typecmp: can only compare closed types";_tag_dyneither(_tmp3F2,sizeof(char),39);}),_tag_dyneither(_tmp3F1,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 2: switch(*((int*)_tmp3F0.f2)){case 2: _LL3A4: _tmp411=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp410=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3A5:
# 1870
 return Cyc_Core_intcmp(_tmp410->identity,_tmp411->identity);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 11: switch(*((int*)_tmp3F0.f2)){case 11: _LL3A6: _tmp415=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F0.f1)->f1).aggr_info;_tmp414=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F0.f1)->f1).targs;_tmp413=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F0.f2)->f1).aggr_info;_tmp412=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F0.f2)->f1).targs;_LL3A7: {
# 1874
struct _tuple13 _tmp3F3=Cyc_Absyn_aggr_kinded_name(_tmp415);struct _tuple13 _tmp3F4=_tmp3F3;struct _tuple2*_tmp3F9;_LL3D9: _tmp3F9=_tmp3F4.f2;_LL3DA:;{
struct _tuple13 _tmp3F5=Cyc_Absyn_aggr_kinded_name(_tmp413);struct _tuple13 _tmp3F6=_tmp3F5;struct _tuple2*_tmp3F8;_LL3DC: _tmp3F8=_tmp3F6.f2;_LL3DD:;{
int _tmp3F7=Cyc_Absyn_qvar_cmp(_tmp3F9,_tmp3F8);
if(_tmp3F7 != 0)return _tmp3F7;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp414,_tmp412);}};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 13: switch(*((int*)_tmp3F0.f2)){case 13: _LL3A8: _tmp417=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp416=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3A9:
# 1881
 return Cyc_Absyn_qvar_cmp(_tmp417,_tmp416);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 14: switch(*((int*)_tmp3F0.f2)){case 14: _LL3AA: _tmp419=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp418=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3AB:
# 1884
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp419,_tmp418);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp3F0.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3AC: _tmp41D=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f1)->f1).datatype_info).KnownDatatype).val;_tmp41C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f1)->f1).targs;_tmp41B=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f2)->f1).datatype_info).KnownDatatype).val;_tmp41A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f2)->f1).targs;_LL3AD:
# 1888
 if(_tmp41B == _tmp41D)return 0;{
int _tmp3FA=Cyc_Absyn_qvar_cmp(_tmp41B->name,_tmp41D->name);
if(_tmp3FA != 0)return _tmp3FA;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp41A,_tmp41C);};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp3F0.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp3F0.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL3AE: _tmp423=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp422=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp421=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f1)->f1).targs;_tmp420=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp41F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp41E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f2)->f1).targs;_LL3AF:
# 1895
 if(_tmp420 == _tmp423)return 0;{
int _tmp3FB=Cyc_Absyn_qvar_cmp(_tmp423->name,_tmp420->name);
if(_tmp3FB != 0)return _tmp3FB;{
int _tmp3FC=Cyc_Absyn_qvar_cmp(_tmp422->name,_tmp41F->name);
if(_tmp3FC != 0)return _tmp3FC;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp41E,_tmp421);};};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp3F0.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 5: switch(*((int*)_tmp3F0.f2)){case 5: _LL3B0: _tmp42F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).elt_typ;_tmp42E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).elt_tq;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).ptr_atts).rgn;_tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).ptr_atts).nullable;_tmp42B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).ptr_atts).bounds;_tmp42A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).ptr_atts).zero_term;_tmp429=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).elt_typ;_tmp428=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).elt_tq;_tmp427=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).ptr_atts).rgn;_tmp426=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).ptr_atts).nullable;_tmp425=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).ptr_atts).bounds;_tmp424=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).ptr_atts).zero_term;_LL3B1: {
# 1904
int _tmp3FD=Cyc_Tcutil_typecmp(_tmp429,_tmp42F);
if(_tmp3FD != 0)return _tmp3FD;{
int _tmp3FE=Cyc_Tcutil_typecmp(_tmp427,_tmp42D);
if(_tmp3FE != 0)return _tmp3FE;{
int _tmp3FF=Cyc_Tcutil_tqual_cmp(_tmp428,_tmp42E);
if(_tmp3FF != 0)return _tmp3FF;{
int _tmp400=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp425,_tmp42B);
if(_tmp400 != 0)return _tmp400;{
int _tmp401=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp424,_tmp42A);
if(_tmp401 != 0)return _tmp401;
{void*_tmp402=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp425);void*_tmp403=_tmp402;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp403)->tag == 0){_LL3DF: _LL3E0:
 return 0;}else{_LL3E1: _LL3E2:
 goto _LL3DE;}_LL3DE:;}
# 1918
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp426,_tmp42C);};};};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 6: switch(*((int*)_tmp3F0.f2)){case 6: _LL3B2: _tmp433=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp432=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f1)->f2;_tmp431=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_tmp430=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f2;_LL3B3:
# 1921
 if(_tmp431 != _tmp433)return Cyc_Core_intcmp((int)((unsigned int)_tmp431),(int)((unsigned int)_tmp433));
if(_tmp430 != _tmp432)return Cyc_Core_intcmp((int)((unsigned int)_tmp430),(int)((unsigned int)_tmp432));
return 0;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 7: switch(*((int*)_tmp3F0.f2)){case 7: _LL3B4: _tmp435=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp434=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3B5:
# 1926
 if(_tmp435 == _tmp434)return 0;else{
if(_tmp434 == 0)return - 1;else{
if(_tmp434 == 1  && _tmp435 == 0)return - 1;else{
return 1;}}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 8: switch(*((int*)_tmp3F0.f2)){case 8: _LL3B6: _tmp43D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f1)->f1).elt_type;_tmp43C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f1)->f1).tq;_tmp43B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f1)->f1).num_elts;_tmp43A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f1)->f1).zero_term;_tmp439=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f2)->f1).elt_type;_tmp438=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f2)->f1).tq;_tmp437=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f2)->f1).num_elts;_tmp436=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f2)->f1).zero_term;_LL3B7: {
# 1933
int _tmp404=Cyc_Tcutil_tqual_cmp(_tmp438,_tmp43C);
if(_tmp404 != 0)return _tmp404;{
int _tmp405=Cyc_Tcutil_typecmp(_tmp439,_tmp43D);
if(_tmp405 != 0)return _tmp405;{
int _tmp406=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp43A,_tmp436);
if(_tmp406 != 0)return _tmp406;
if(_tmp43B == _tmp437)return 0;
if(_tmp43B == 0  || _tmp437 == 0)
({void*_tmp407=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp408="missing expression in array index";_tag_dyneither(_tmp408,sizeof(char),34);}),_tag_dyneither(_tmp407,sizeof(void*),0));});
# 1943
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp43B,_tmp437);};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 9: switch(*((int*)_tmp3F0.f2)){case 9: _LL3B8: _LL3B9:
# 1946
({void*_tmp409=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp40A="typecmp: function types not handled";_tag_dyneither(_tmp40A,sizeof(char),36);}),_tag_dyneither(_tmp409,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 10: switch(*((int*)_tmp3F0.f2)){case 10: _LL3BA: _tmp43F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp43E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3BB:
# 1949
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp43E,_tmp43F);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 12: switch(*((int*)_tmp3F0.f2)){case 12: _LL3BC: _tmp443=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp442=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F0.f1)->f2;_tmp441=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_tmp440=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F0.f2)->f2;_LL3BD:
# 1952
 if(_tmp441 != _tmp443){
if(_tmp441 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp440,_tmp442);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 15: switch(*((int*)_tmp3F0.f2)){case 15: _LL3BE: _tmp445=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp444=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3BF:
# 1957
 return Cyc_Tcutil_typecmp(_tmp445,_tmp444);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 16: switch(*((int*)_tmp3F0.f2)){case 16: _LL3C0: _tmp449=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp448=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F0.f1)->f2;_tmp447=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_tmp446=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F0.f2)->f2;_LL3C1: {
# 1959
int _tmp40B=Cyc_Tcutil_typecmp(_tmp449,_tmp447);
if(_tmp40B != 0)return _tmp40B;else{
return Cyc_Tcutil_typecmp(_tmp448,_tmp446);}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 19: switch(*((int*)_tmp3F0.f2)){case 19: _LL3C2: _tmp44B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp44A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3C3:
 return Cyc_Tcutil_typecmp(_tmp44B,_tmp44A);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 18: switch(*((int*)_tmp3F0.f2)){case 18: _LL3C4: _tmp44D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp44C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3C5:
 _tmp44F=_tmp44D;_tmp44E=_tmp44C;goto _LL3C7;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 27: switch(*((int*)_tmp3F0.f2)){case 27: _LL3C6: _tmp44F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp44E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3C7:
# 1965
 return Cyc_Evexp_const_exp_cmp(_tmp44F,_tmp44E);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 28: switch(*((int*)_tmp3F0.f2)){case 28: _LL3C8: _tmp451=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp450=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3C9:
# 1967
 return Cyc_strcmp((struct _dyneither_ptr)_tmp451,(struct _dyneither_ptr)_tmp450);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 24: _LL3CA: _LL3CB:
 goto _LL3CD;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3F0.f2)->tag == 24){_LL3CC: _LL3CD:
 goto _LL3CF;}else{switch(*((int*)_tmp3F0.f1)){case 23: _LL3CE: _LL3CF:
 goto _LL3D1;case 25: _LL3D0: _LL3D1:
 goto _LL3D3;default: switch(*((int*)_tmp3F0.f2)){case 25: _LL3D2: _LL3D3:
 goto _LL3D5;case 23: _LL3D4: _LL3D5:
({void*_tmp40C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp40D="typecmp: effects not handled";_tag_dyneither(_tmp40D,sizeof(char),29);}),_tag_dyneither(_tmp40C,sizeof(void*),0));});default: _LL3D6: _LL3D7:
({void*_tmp40E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp40F="Unmatched case in typecmp";_tag_dyneither(_tmp40F,sizeof(char),26);}),_tag_dyneither(_tmp40E,sizeof(void*),0));});}}}}_LL3A1:;};};}
# 1978
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp453=Cyc_Tcutil_compress(t);void*_tmp454=_tmp453;switch(*((int*)_tmp454)){case 6: _LL3E4: _LL3E5:
 goto _LL3E7;case 7: _LL3E6: _LL3E7:
 goto _LL3E9;case 13: _LL3E8: _LL3E9:
 goto _LL3EB;case 14: _LL3EA: _LL3EB:
 return 1;default: _LL3EC: _LL3ED:
 return 0;}_LL3E3:;}
# 1990
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp455=({struct _tuple0 _tmp459;_tmp459.f1=t1;_tmp459.f2=t2;_tmp459;});struct _tuple0 _tmp456=_tmp455;int _tmp458;int _tmp457;switch(*((int*)_tmp456.f1)){case 7: switch(*((int*)_tmp456.f2)){case 7: _LL3EF: _tmp458=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp456.f1)->f1;_tmp457=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp456.f2)->f1;_LL3F0:
# 1995
 return _tmp457 == 0  && _tmp458 != 0  || (_tmp457 == 1  && _tmp458 != 0) && _tmp458 != 1;case 6: _LL3F1: _LL3F2:
 goto _LL3F4;case 19: _LL3F3: _LL3F4:
 return 1;default: goto _LL40F;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL3F5: _LL3F6:
 return 0;}else{goto _LL3F7;}}else{_LL3F7: _LL3F8:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp456.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2){case Cyc_Absyn_Int_sz: _LL3F9: _LL3FA:
# 2002
 goto _LL3FC;case Cyc_Absyn_Short_sz: _LL401: _LL402:
# 2007
 goto _LL404;case Cyc_Absyn_Char_sz: _LL407: _LL408:
# 2010
 goto _LL40A;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp456.f2)->f1 == 0){_LL3FD: _LL3FE:
# 2005
 goto _LL400;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp456.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2){case Cyc_Absyn_Long_sz: _LL3FB: _LL3FC:
# 2003
 return 0;case Cyc_Absyn_Short_sz: _LL403: _LL404:
# 2008
 goto _LL406;case Cyc_Absyn_Char_sz: _LL409: _LL40A:
# 2011
 goto _LL40C;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp456.f2)->f1 == 0){_LL3FF: _LL400:
# 2006
 goto _LL402;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2 == Cyc_Absyn_Char_sz){_LL40B: _LL40C:
# 2012
 goto _LL40E;}else{goto _LL40F;}}else{goto _LL40F;}default: goto _LL40F;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2){case Cyc_Absyn_Short_sz: _LL405: _LL406:
# 2009
 goto _LL408;case Cyc_Absyn_Char_sz: _LL40D: _LL40E:
# 2013
 return 1;default: goto _LL40F;}else{goto _LL40F;}default: _LL40F: _LL410:
# 2015
 return 0;}_LL3EE:;};}
# 2021
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2024
struct _RegionHandle _tmp45A=_new_region("r");struct _RegionHandle*r=& _tmp45A;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp45B=_region_malloc(r,sizeof(*_tmp45B));_tmp45B->v=t1;_tmp45B;});}}}
# 2034
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp45C=0;_npop_handler(0);return _tmp45C;}}}
# 2039
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp460;_tmp460.tag=0;_tmp460.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp45F;_tmp45F.tag=0;_tmp45F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp45D[2]={& _tmp45F,& _tmp460};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp45E="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp45E,sizeof(char),41);}),_tag_dyneither(_tmp45D,sizeof(void*),2));});});});{
int _tmp461=0;_npop_handler(0);return _tmp461;};}}}{
# 2045
int _tmp462=1;_npop_handler(0);return _tmp462;};
# 2024
;_pop_region(r);}
# 2050
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp463=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp464=_tmp463;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp464)->tag == 5){_LL412: _LL413:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL411;}else{_LL414: _LL415:
 return 0;}_LL411:;}
# 2056
return 1;}
# 2059
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp465=Cyc_Tcutil_compress(t);void*_tmp466=_tmp465;switch(*((int*)_tmp466)){case 6: _LL417: _LL418:
 goto _LL41A;case 19: _LL419: _LL41A:
 goto _LL41C;case 13: _LL41B: _LL41C:
 goto _LL41E;case 14: _LL41D: _LL41E:
 return 1;default: _LL41F: _LL420:
 return 0;}_LL416:;}
# 2070
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2074
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp467=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp468="integral size mismatch; conversion supplied";_tag_dyneither(_tmp468,sizeof(char),44);}),_tag_dyneither(_tmp467,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2080
return 0;}
# 2084
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2088
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp469=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp46A="integral size mismatch; conversion supplied";_tag_dyneither(_tmp46A,sizeof(char),44);}),_tag_dyneither(_tmp469,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2094
return 0;}
# 2099
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2107
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2109
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp46B=({struct _tuple0 _tmp484;_tmp484.f1=t1;_tmp484.f2=t2;_tmp484;});struct _tuple0 _tmp46C=_tmp46B;void*_tmp483;struct Cyc_Absyn_Tqual _tmp482;struct Cyc_Absyn_Exp*_tmp481;union Cyc_Absyn_Constraint*_tmp480;void*_tmp47F;struct Cyc_Absyn_Tqual _tmp47E;struct Cyc_Absyn_Exp*_tmp47D;union Cyc_Absyn_Constraint*_tmp47C;struct Cyc_Absyn_PtrInfo _tmp47B;struct Cyc_Absyn_PtrInfo _tmp47A;switch(*((int*)_tmp46C.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46C.f2)->tag == 5){_LL422: _tmp47B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46C.f1)->f1;_tmp47A=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46C.f2)->f1;_LL423: {
# 2113
int okay=1;
# 2115
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp47B.ptr_atts).nullable,(_tmp47A.ptr_atts).nullable))
# 2118
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp47B.ptr_atts).nullable);
# 2120
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp47B.ptr_atts).bounds,(_tmp47A.ptr_atts).bounds)){
# 2123
struct _tuple0 _tmp46D=({struct _tuple0 _tmp474;_tmp474.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp47B.ptr_atts).bounds);_tmp474.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp47A.ptr_atts).bounds);_tmp474;});
# 2123
struct _tuple0 _tmp46E=_tmp46D;struct Cyc_Absyn_Exp*_tmp473;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp471;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46E.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46E.f2)->tag == 0){_LL42B: _LL42C:
# 2125
 goto _LL42E;}else{_LL42F: _tmp472=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46E.f1)->f1;_tmp471=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46E.f2)->f1;_LL430:
# 2128
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp471,_tmp472);
# 2132
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp47A.ptr_atts).zero_term))
({void*_tmp46F=0;Cyc_Tcutil_warn(loc,({const char*_tmp470="implicit cast to shorter array";_tag_dyneither(_tmp470,sizeof(char),31);}),_tag_dyneither(_tmp46F,sizeof(void*),0));});
goto _LL42A;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46E.f2)->tag == 0){_LL42D: _LL42E:
# 2126
 okay=1;goto _LL42A;}else{_LL431: _tmp473=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46E.f2)->f1;_LL432:
# 2137
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp47B.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp47A.ptr_atts).bounds))
goto _LL42A;
okay=0;
goto _LL42A;}}_LL42A:;}
# 2146
okay=okay  && (!(_tmp47B.elt_tq).real_const  || (_tmp47A.elt_tq).real_const);
# 2149
if(!Cyc_Tcutil_unify((_tmp47B.ptr_atts).rgn,(_tmp47A.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp47B.ptr_atts).rgn,(_tmp47A.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp478;_tmp478.tag=0;_tmp478.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2154
Cyc_Absynpp_typ2string((_tmp47A.ptr_atts).rgn));({struct Cyc_String_pa_PrintArg_struct _tmp477;_tmp477.tag=0;_tmp477.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2153
Cyc_Absynpp_typ2string((_tmp47B.ptr_atts).rgn));({void*_tmp475[2]={& _tmp477,& _tmp478};Cyc_Tcutil_warn(loc,({const char*_tmp476="implicit cast from region %s to region %s";_tag_dyneither(_tmp476,sizeof(char),42);}),_tag_dyneither(_tmp475,sizeof(void*),2));});});});}else{
# 2155
okay=0;}}
# 2159
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp47B.ptr_atts).zero_term,(_tmp47A.ptr_atts).zero_term) || 
# 2162
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp47B.ptr_atts).zero_term) && (_tmp47A.elt_tq).real_const);{
# 2170
int _tmp479=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp47A.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2173
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp47A.ptr_atts).zero_term);
# 2177
okay=okay  && (Cyc_Tcutil_unify(_tmp47B.elt_typ,_tmp47A.elt_typ) || 
(_tmp479  && ((_tmp47A.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp47A.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp47B.elt_typ,_tmp47A.elt_typ));
# 2180
return okay;};}}else{goto _LL428;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f2)->tag == 8){_LL424: _tmp483=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f1)->f1).elt_type;_tmp482=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f1)->f1).tq;_tmp481=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f1)->f1).num_elts;_tmp480=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f1)->f1).zero_term;_tmp47F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f2)->f1).elt_type;_tmp47E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f2)->f1).tq;_tmp47D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f2)->f1).num_elts;_tmp47C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp46C.f2)->f1).zero_term;_LL425: {
# 2184
int okay;
# 2186
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp480,_tmp47C) && (
(_tmp481 != 0  && _tmp47D != 0) && Cyc_Evexp_same_const_exp(_tmp481,_tmp47D));
# 2189
return(okay  && Cyc_Tcutil_unify(_tmp483,_tmp47F)) && (!_tmp482.real_const  || _tmp47E.real_const);}}else{goto _LL428;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp46C.f2)->tag == 6){_LL426: _LL427:
# 2191
 return 0;}else{goto _LL428;}default: _LL428: _LL429:
# 2193
 return Cyc_Tcutil_unify(t1,t2);}_LL421:;};}
# 2197
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp485=Cyc_Tcutil_compress(t);void*_tmp486=_tmp485;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp486)->tag == 5){_LL434: _LL435:
 return 1;}else{_LL436: _LL437:
 return 0;}_LL433:;}
# 2203
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp487=Cyc_Tcutil_compress(t);void*_tmp488=_tmp487;void*_tmp48B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp488)->tag == 5){_LL439: _tmp48B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp488)->f1).elt_typ;_LL43A:
 return _tmp48B;}else{_LL43B: _LL43C:
({void*_tmp489=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp48A="pointer_elt_type";_tag_dyneither(_tmp48A,sizeof(char),17);}),_tag_dyneither(_tmp489,sizeof(void*),0));});}_LL438:;}
# 2209
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp48C=Cyc_Tcutil_compress(t);void*_tmp48D=_tmp48C;struct Cyc_Absyn_PtrAtts*_tmp490;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D)->tag == 5){_LL43E: _tmp490=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48D)->f1).ptr_atts;_LL43F:
 return _tmp490->rgn;}else{_LL440: _LL441:
({void*_tmp48E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp48F="pointer_elt_type";_tag_dyneither(_tmp48F,sizeof(char),17);}),_tag_dyneither(_tmp48E,sizeof(void*),0));});}_LL43D:;}
# 2216
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp491=Cyc_Tcutil_compress(t);void*_tmp492=_tmp491;void*_tmp493;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp492)->tag == 5){_LL443: _tmp493=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp492)->f1).ptr_atts).rgn;_LL444:
# 2219
*rgn=_tmp493;
return 1;}else{_LL445: _LL446:
 return 0;}_LL442:;}
# 2226
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2230
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp494=Cyc_Tcutil_compress(t);void*_tmp495=_tmp494;union Cyc_Absyn_Constraint*_tmp496;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp495)->tag == 5){_LL448: _tmp496=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp495)->f1).ptr_atts).bounds;_LL449:
# 2233
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp496)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL44A: _LL44B:
 return 0;}_LL447:;}
# 2239
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp497=e->r;void*_tmp498=_tmp497;void*_tmp49C;struct Cyc_Absyn_Exp*_tmp49B;struct _dyneither_ptr _tmp49A;switch(*((int*)_tmp498)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp498)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp498)->f1).Int_c).val).f2 == 0){_LL44D: _LL44E:
 goto _LL450;}else{goto _LL459;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp498)->f1).Char_c).val).f2 == 0){_LL44F: _LL450:
 goto _LL452;}else{goto _LL459;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp498)->f1).Short_c).val).f2 == 0){_LL451: _LL452:
 goto _LL454;}else{goto _LL459;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp498)->f1).LongLong_c).val).f2 == 0){_LL453: _LL454:
# 2245
 return 1;}else{goto _LL459;}case 3: _LL455: _tmp49A=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp498)->f1).Wchar_c).val;_LL456: {
# 2247
unsigned long _tmp499=Cyc_strlen((struct _dyneither_ptr)_tmp49A);
int i=0;
if(_tmp499 >= 2  && *((const char*)_check_dyneither_subscript(_tmp49A,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp49A,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp49A,sizeof(char),1))== 'x'  && _tmp499 >= 3) && *((const char*)_check_dyneither_subscript(_tmp49A,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp499;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp49A,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2257
return 0;}}default: goto _LL459;}case 13: _LL457: _tmp49C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp498)->f1;_tmp49B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp498)->f2;_LL458:
 return Cyc_Tcutil_is_zero(_tmp49B) && Cyc_Tcutil_admits_zero(_tmp49C);default: _LL459: _LL45A:
 return 0;}_LL44C:;}
# 2263
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2270
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2275
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2280
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2287
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2292
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2297
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp49D=ka;enum Cyc_Absyn_KindQual _tmp4A6;enum Cyc_Absyn_AliasQual _tmp4A5;_LL45C: _tmp4A6=_tmp49D->kind;_tmp4A5=_tmp49D->aliasqual;_LL45D:;
{enum Cyc_Absyn_AliasQual _tmp49E=_tmp4A5;switch(_tmp49E){case Cyc_Absyn_Aliasable: _LL45F: _LL460: {
# 2301
enum Cyc_Absyn_KindQual _tmp49F=_tmp4A6;switch(_tmp49F){case Cyc_Absyn_AnyKind: _LL466: _LL467:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL468: _LL469:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL46A: _LL46B:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL46C: _LL46D:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL46E: _LL46F:
 return& Cyc_Tcutil_eko;default: _LL470: _LL471:
 return& Cyc_Tcutil_iko;}_LL465:;}case Cyc_Absyn_Unique: _LL461: _LL462:
# 2310
{enum Cyc_Absyn_KindQual _tmp4A0=_tmp4A6;switch(_tmp4A0){case Cyc_Absyn_AnyKind: _LL473: _LL474:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL475: _LL476:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL477: _LL478:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL479: _LL47A:
 return& Cyc_Tcutil_urko;default: _LL47B: _LL47C:
 goto _LL472;}_LL472:;}
# 2317
goto _LL45E;default: _LL463: _LL464:
# 2319
{enum Cyc_Absyn_KindQual _tmp4A1=_tmp4A6;switch(_tmp4A1){case Cyc_Absyn_AnyKind: _LL47E: _LL47F:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL480: _LL481:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL482: _LL483:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL484: _LL485:
 return& Cyc_Tcutil_trko;default: _LL486: _LL487:
 goto _LL47D;}_LL47D:;}
# 2326
goto _LL45E;}_LL45E:;}
# 2328
({struct Cyc_String_pa_PrintArg_struct _tmp4A4;_tmp4A4.tag=0;_tmp4A4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4A2[1]={& _tmp4A4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A3="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4A3,sizeof(char),26);}),_tag_dyneither(_tmp4A2,sizeof(void*),1));});});}
# 2331
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2339
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2346
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2351
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2356
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2361
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2366
struct Cyc_Absyn_Kind*_tmp4A7=ka;enum Cyc_Absyn_KindQual _tmp4B0;enum Cyc_Absyn_AliasQual _tmp4AF;_LL489: _tmp4B0=_tmp4A7->kind;_tmp4AF=_tmp4A7->aliasqual;_LL48A:;
{enum Cyc_Absyn_AliasQual _tmp4A8=_tmp4AF;switch(_tmp4A8){case Cyc_Absyn_Aliasable: _LL48C: _LL48D: {
# 2369
enum Cyc_Absyn_KindQual _tmp4A9=_tmp4B0;switch(_tmp4A9){case Cyc_Absyn_AnyKind: _LL493: _LL494:
 return& ab;case Cyc_Absyn_MemKind: _LL495: _LL496:
 return& mb;case Cyc_Absyn_BoxKind: _LL497: _LL498:
 return& bb;case Cyc_Absyn_RgnKind: _LL499: _LL49A:
 return& rb;case Cyc_Absyn_EffKind: _LL49B: _LL49C:
 return& eb;default: _LL49D: _LL49E:
 return& ib;}_LL492:;}case Cyc_Absyn_Unique: _LL48E: _LL48F:
# 2378
{enum Cyc_Absyn_KindQual _tmp4AA=_tmp4B0;switch(_tmp4AA){case Cyc_Absyn_AnyKind: _LL4A0: _LL4A1:
 return& uab;case Cyc_Absyn_MemKind: _LL4A2: _LL4A3:
 return& umb;case Cyc_Absyn_BoxKind: _LL4A4: _LL4A5:
 return& ubb;case Cyc_Absyn_RgnKind: _LL4A6: _LL4A7:
 return& urb;default: _LL4A8: _LL4A9:
 goto _LL49F;}_LL49F:;}
# 2385
goto _LL48B;default: _LL490: _LL491:
# 2387
{enum Cyc_Absyn_KindQual _tmp4AB=_tmp4B0;switch(_tmp4AB){case Cyc_Absyn_AnyKind: _LL4AB: _LL4AC:
 return& tab;case Cyc_Absyn_MemKind: _LL4AD: _LL4AE:
 return& tmb;case Cyc_Absyn_BoxKind: _LL4AF: _LL4B0:
 return& tbb;case Cyc_Absyn_RgnKind: _LL4B1: _LL4B2:
 return& trb;default: _LL4B3: _LL4B4:
 goto _LL4AA;}_LL4AA:;}
# 2394
goto _LL48B;}_LL48B:;}
# 2396
({struct Cyc_String_pa_PrintArg_struct _tmp4AE;_tmp4AE.tag=0;_tmp4AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4AC[1]={& _tmp4AE};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4AD="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4AD,sizeof(char),24);}),_tag_dyneither(_tmp4AC,sizeof(void*),1));});});}
# 2399
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2402
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2404
return({struct Cyc_Core_Opt*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF->v=Cyc_Tcutil_kind_to_bound(k);_tmp4BF;});}
# 2409
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
e1->r=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4C1;_tmp4C1.tag=0;_tmp4C1.f1=Cyc_Absyn_Null_c;_tmp4C1;});_tmp4C0;});{
struct Cyc_Core_Opt*_tmp4C2=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C3=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4C9;_tmp4C9.tag=5;_tmp4C9.f1=({struct Cyc_Absyn_PtrInfo _tmp4CA;_tmp4CA.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4C2);_tmp4CA.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp4CA.ptr_atts=({(_tmp4CA.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4C2);(_tmp4CA.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4CA.ptr_atts).bounds=
# 2417
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4CA.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4CA.ptr_atts).ptrloc=0;_tmp4CA.ptr_atts;});_tmp4CA;});_tmp4C9;});_tmp4C8;});
e1->topt=(void*)_tmp4C3;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4C7;_tmp4C7.tag=0;_tmp4C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));({struct Cyc_String_pa_PrintArg_struct _tmp4C6;_tmp4C6.tag=0;_tmp4C6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp4C4[2]={& _tmp4C6,& _tmp4C7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4C5="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4C5,sizeof(char),56);}),_tag_dyneither(_tmp4C4,sizeof(void*),2));});});});
return retv;};};}
# 2427
return 0;}
# 2430
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4CB=c;switch(_tmp4CB){case Cyc_Absyn_Unknown_coercion: _LL4B6: _LL4B7:
 return({const char*_tmp4CC="unknown";_tag_dyneither(_tmp4CC,sizeof(char),8);});case Cyc_Absyn_No_coercion: _LL4B8: _LL4B9:
 return({const char*_tmp4CD="no coercion";_tag_dyneither(_tmp4CD,sizeof(char),12);});case Cyc_Absyn_Null_to_NonNull: _LL4BA: _LL4BB:
 return({const char*_tmp4CE="null check";_tag_dyneither(_tmp4CE,sizeof(char),11);});default: _LL4BC: _LL4BD:
 return({const char*_tmp4CF="other coercion";_tag_dyneither(_tmp4CF,sizeof(char),15);});}_LL4B5:;}
# 2439
int Cyc_Tcutil_warn_alias_coerce=0;
# 2445
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2449
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4E3=_cycalloc(sizeof(*_tmp4E3));_tmp4E3->f1=Cyc_Absyn_Loc_n;_tmp4E3->f2=({struct _dyneither_ptr*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));_tmp4E4[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4E7;_tmp4E7.tag=1;_tmp4E7.f1=(unsigned long)counter ++;({void*_tmp4E5[1]={& _tmp4E7};Cyc_aprintf(({const char*_tmp4E6="__aliasvar%d";_tag_dyneither(_tmp4E6,sizeof(char),13);}),_tag_dyneither(_tmp4E5,sizeof(void*),1));});});_tmp4E4;});_tmp4E3;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4E2;_tmp4E2.tag=4;_tmp4E2.f1=vd;_tmp4E2;});_tmp4E1;}),e->loc);
# 2458
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2460
{void*_tmp4D0=Cyc_Tcutil_compress(e_typ);void*_tmp4D1=_tmp4D0;void*_tmp4DF;struct Cyc_Absyn_Tqual _tmp4DE;void*_tmp4DD;union Cyc_Absyn_Constraint*_tmp4DC;union Cyc_Absyn_Constraint*_tmp4DB;union Cyc_Absyn_Constraint*_tmp4DA;struct Cyc_Absyn_PtrLoc*_tmp4D9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D1)->tag == 5){_LL4BF: _tmp4DF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D1)->f1).elt_typ;_tmp4DE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D1)->f1).elt_tq;_tmp4DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D1)->f1).ptr_atts).rgn;_tmp4DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D1)->f1).ptr_atts).nullable;_tmp4DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D1)->f1).ptr_atts).bounds;_tmp4DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D1)->f1).ptr_atts).zero_term;_tmp4D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D1)->f1).ptr_atts).ptrloc;_LL4C0:
# 2462
{void*_tmp4D2=Cyc_Tcutil_compress(_tmp4DD);void*_tmp4D3=_tmp4D2;void**_tmp4D8;struct Cyc_Core_Opt*_tmp4D7;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D3)->tag == 1){_LL4C4: _tmp4D8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D3)->f2;_tmp4D7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D3)->f4;_LL4C5: {
# 2464
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D4=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D5=_cycalloc(sizeof(*_tmp4D5));_tmp4D5[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4D6;_tmp4D6.tag=2;_tmp4D6.f1=tv;_tmp4D6;});_tmp4D5;});
*_tmp4D8=(void*)_tmp4D4;
goto _LL4C3;}}else{_LL4C6: _LL4C7:
 goto _LL4C3;}_LL4C3:;}
# 2469
goto _LL4BE;}else{_LL4C1: _LL4C2:
 goto _LL4BE;}_LL4BE:;}
# 2473
e->topt=0;
vd->initializer=0;{
# 2477
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2479
return({struct _tuple15 _tmp4E0;_tmp4E0.f1=d;_tmp4E0.f2=ve;_tmp4E0;});};}
# 2484
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2487
struct _RegionHandle _tmp4E9=_new_region("r");struct _RegionHandle*r=& _tmp4E9;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2492
void*_tmp4EA=Cyc_Tcutil_compress(wants_typ);void*_tmp4EB=_tmp4EA;void*_tmp4F6;switch(*((int*)_tmp4EB)){case 5: _LL4C9: _tmp4F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4EB)->f1).ptr_atts).rgn;_LL4CA: {
# 2494
void*_tmp4EC=Cyc_Tcutil_compress(_tmp4F6);void*_tmp4ED=_tmp4EC;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4ED)->tag == 20){_LL4D0: _LL4D1: {
int _tmp4EE=0;_npop_handler(0);return _tmp4EE;}}else{_LL4D2: _LL4D3: {
# 2497
struct Cyc_Absyn_Kind*_tmp4EF=Cyc_Tcutil_typ_kind(_tmp4F6);
int _tmp4F0=_tmp4EF->kind == Cyc_Absyn_RgnKind  && _tmp4EF->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp4F0;}}_LL4CF:;}case 17: _LL4CB: _LL4CC:
# 2501
({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0;_tmp4F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc));({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0;_tmp4F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ)));({void*_tmp4F1[2]={& _tmp4F3,& _tmp4F4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4F2="got typedef %s in can_insert_alias at %s\n";_tag_dyneither(_tmp4F2,sizeof(char),42);}),_tag_dyneither(_tmp4F1,sizeof(void*),2));});});});default: _LL4CD: _LL4CE: {
int _tmp4F5=0;_npop_handler(0);return _tmp4F5;}}_LL4C8:;}{
# 2506
int _tmp4F7=0;_npop_handler(0);return _tmp4F7;};
# 2488
;_pop_region(r);}
# 2510
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2515
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2517
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2519
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0;_tmp4FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0;_tmp4FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp4F8[2]={& _tmp4FA,& _tmp4FB};Cyc_Tcutil_warn(e->loc,({const char*_tmp4F9="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp4F9,sizeof(char),53);}),_tag_dyneither(_tmp4F8,sizeof(void*),2));});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2528
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0;_tmp500.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp4FF;_tmp4FF.tag=0;_tmp4FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0;_tmp4FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp4FC[3]={& _tmp4FE,& _tmp4FF,& _tmp500};Cyc_Tcutil_warn(e->loc,({const char*_tmp4FD="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp4FD,sizeof(char),40);}),_tag_dyneither(_tmp4FC,sizeof(void*),3));});});});});
*alias_coercion=1;}
# 2535
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2543
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0;_tmp504.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp503;_tmp503.tag=0;_tmp503.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp501[2]={& _tmp503,& _tmp504};Cyc_Tcutil_warn(e->loc,({const char*_tmp502="implicit cast from %s to %s";_tag_dyneither(_tmp502,sizeof(char),28);}),_tag_dyneither(_tmp501,sizeof(void*),2));});});});
return 1;}else{
# 2549
return 0;}}}}}
# 2556
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2559
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2563
int Cyc_Tcutil_coerceable(void*t){
void*_tmp505=Cyc_Tcutil_compress(t);void*_tmp506=_tmp505;switch(*((int*)_tmp506)){case 6: _LL4D5: _LL4D6:
 goto _LL4D8;case 7: _LL4D7: _LL4D8:
 return 1;default: _LL4D9: _LL4DA:
 return 0;}_LL4D4:;}
# 2581 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2585
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2588
struct _tuple23 _tmp507=*env;struct _tuple23 _tmp508=_tmp507;struct Cyc_List_List*_tmp510;struct _RegionHandle*_tmp50F;struct Cyc_Tcenv_Tenv*_tmp50E;int _tmp50D;_LL4DC: _tmp510=_tmp508.f1;_tmp50F=_tmp508.f2;_tmp50E=_tmp508.f3;_tmp50D=_tmp508.f4;_LL4DD:;{
# 2590
void*_tmp509=_tmp510 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp50F,_tmp510,x->type);
struct Cyc_List_List*_tmp50A=Cyc_Tcutil_flatten_typ(_tmp50F,_tmp50D,_tmp50E,_tmp509);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp50A)== 1)
return({struct Cyc_List_List*_tmp50B=_region_malloc(_tmp50F,sizeof(*_tmp50B));_tmp50B->hd=({struct _tuple12*_tmp50C=_region_malloc(_tmp50F,sizeof(*_tmp50C));_tmp50C->f1=x->tq;_tmp50C->f2=_tmp509;_tmp50C;});_tmp50B->tl=0;_tmp50B;});else{
return _tmp50A;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2596
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2598
struct _tuple24 _tmp511=*env;struct _tuple24 _tmp512=_tmp511;struct _RegionHandle*_tmp51C;struct Cyc_Tcenv_Tenv*_tmp51B;int _tmp51A;_LL4DF: _tmp51C=_tmp512.f1;_tmp51B=_tmp512.f2;_tmp51A=_tmp512.f3;_LL4E0:;{
struct _tuple12 _tmp513=*x;struct _tuple12 _tmp514=_tmp513;struct Cyc_Absyn_Tqual _tmp519;void*_tmp518;_LL4E2: _tmp519=_tmp514.f1;_tmp518=_tmp514.f2;_LL4E3:;{
struct Cyc_List_List*_tmp515=Cyc_Tcutil_flatten_typ(_tmp51C,_tmp51A,_tmp51B,_tmp518);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp515)== 1)
return({struct Cyc_List_List*_tmp516=_region_malloc(_tmp51C,sizeof(*_tmp516));_tmp516->hd=({struct _tuple12*_tmp517=_region_malloc(_tmp51C,sizeof(*_tmp517));_tmp517->f1=_tmp519;_tmp517->f2=_tmp518;_tmp517;});_tmp516->tl=0;_tmp516;});else{
return _tmp515;}};};}
# 2605
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2609
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp51D=t1;struct Cyc_List_List*_tmp53F;struct Cyc_Absyn_Aggrdecl*_tmp53E;struct Cyc_List_List*_tmp53D;struct Cyc_List_List*_tmp53C;switch(*((int*)_tmp51D)){case 0: _LL4E5: _LL4E6:
 return 0;case 10: _LL4E7: _tmp53C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp51D)->f1;_LL4E8: {
# 2614
struct _tuple24 _tmp51E=({struct _tuple24 _tmp527;_tmp527.f1=r;_tmp527.f2=te;_tmp527.f3=flatten;_tmp527;});
# 2616
struct Cyc_List_List*_tmp51F=_tmp53C;struct _tuple12*_tmp526;struct Cyc_List_List*_tmp525;if(_tmp51F == 0){_LL4F0: _LL4F1:
 return 0;}else{_LL4F2: _tmp526=(struct _tuple12*)_tmp51F->hd;_tmp525=_tmp51F->tl;_LL4F3: {
# 2619
struct Cyc_List_List*_tmp520=Cyc_Tcutil_rcopy_tqt(& _tmp51E,_tmp526);
_tmp51E.f3=0;{
struct Cyc_List_List*_tmp521=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp51E,_tmp53C);
struct Cyc_List_List*_tmp522=({struct Cyc_List_List*_tmp524=_region_malloc(r,sizeof(*_tmp524));_tmp524->hd=_tmp520;_tmp524->tl=_tmp521;_tmp524;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,({struct Cyc_List_List*_tmp523=_region_malloc(r,sizeof(*_tmp523));_tmp523->hd=_tmp520;_tmp523->tl=_tmp521;_tmp523;}));};}}_LL4EF:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp51D)->f1).aggr_info).KnownAggr).tag == 2){_LL4E9: _tmp53E=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp51D)->f1).aggr_info).KnownAggr).val;_tmp53D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp51D)->f1).targs;_LL4EA:
# 2627
 if(((_tmp53E->kind == Cyc_Absyn_UnionA  || _tmp53E->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53E->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53E->impl))->rgn_po != 0)
# 2629
return({struct Cyc_List_List*_tmp528=_region_malloc(r,sizeof(*_tmp528));_tmp528->hd=({struct _tuple12*_tmp529=_region_malloc(r,sizeof(*_tmp529));_tmp529->f1=Cyc_Absyn_empty_tqual(0);_tmp529->f2=t1;_tmp529;});_tmp528->tl=0;_tmp528;});{
struct Cyc_List_List*_tmp52A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp53E->tvs,_tmp53D);
struct _tuple23 env=({struct _tuple23 _tmp533;_tmp533.f1=_tmp52A;_tmp533.f2=r;_tmp533.f3=te;_tmp533.f4=flatten;_tmp533;});
struct Cyc_List_List*_tmp52B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53E->impl))->fields;struct Cyc_List_List*_tmp52C=_tmp52B;struct Cyc_Absyn_Aggrfield*_tmp532;struct Cyc_List_List*_tmp531;if(_tmp52C == 0){_LL4F5: _LL4F6:
 return 0;}else{_LL4F7: _tmp532=(struct Cyc_Absyn_Aggrfield*)_tmp52C->hd;_tmp531=_tmp52C->tl;_LL4F8: {
# 2635
struct Cyc_List_List*_tmp52D=Cyc_Tcutil_flatten_typ_f(& env,_tmp532);
env.f4=0;{
struct Cyc_List_List*_tmp52E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp531);
struct Cyc_List_List*_tmp52F=({struct Cyc_List_List*_tmp530=_region_malloc(r,sizeof(*_tmp530));_tmp530->hd=_tmp52D;_tmp530->tl=_tmp52E;_tmp530;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp52F);};}}_LL4F4:;};}else{goto _LL4ED;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp51D)->f1 == Cyc_Absyn_StructA){_LL4EB: _tmp53F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp51D)->f2;_LL4EC: {
# 2642
struct _tuple23 env=({struct _tuple23 _tmp53B;_tmp53B.f1=0;_tmp53B.f2=r;_tmp53B.f3=te;_tmp53B.f4=flatten;_tmp53B;});
struct Cyc_List_List*_tmp534=_tmp53F;struct Cyc_Absyn_Aggrfield*_tmp53A;struct Cyc_List_List*_tmp539;if(_tmp534 == 0){_LL4FA: _LL4FB:
 return 0;}else{_LL4FC: _tmp53A=(struct Cyc_Absyn_Aggrfield*)_tmp534->hd;_tmp539=_tmp534->tl;_LL4FD: {
# 2646
struct Cyc_List_List*_tmp535=Cyc_Tcutil_flatten_typ_f(& env,_tmp53A);
env.f4=0;{
struct Cyc_List_List*_tmp536=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp539);
struct Cyc_List_List*_tmp537=({struct Cyc_List_List*_tmp538=_region_malloc(r,sizeof(*_tmp538));_tmp538->hd=_tmp535;_tmp538->tl=_tmp536;_tmp538;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp537);};}}_LL4F9:;}}else{goto _LL4ED;}default: _LL4ED: _LL4EE:
# 2652
 goto _LL4E4;}_LL4E4:;};}
# 2655
return({struct Cyc_List_List*_tmp540=_region_malloc(r,sizeof(*_tmp540));_tmp540->hd=({struct _tuple12*_tmp541=_region_malloc(r,sizeof(*_tmp541));_tmp541->f1=Cyc_Absyn_empty_tqual(0);_tmp541->f2=t1;_tmp541;});_tmp540->tl=0;_tmp540;});}
# 2659
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp542=(void*)t->hd;void*_tmp543=_tmp542;switch(*((int*)_tmp543)){case 23: _LL4FF: _LL500:
 goto _LL502;case 4: _LL501: _LL502:
 goto _LL504;case 20: _LL503: _LL504:
# 2665
 continue;default: _LL505: _LL506:
# 2667
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL4FE:;}}
# 2670
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2673
return 1;}
# 2676
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp544=({struct _tuple0 _tmp548;_tmp548.f1=Cyc_Tcutil_compress(t1);_tmp548.f2=Cyc_Tcutil_compress(t2);_tmp548;});struct _tuple0 _tmp545=_tmp544;enum Cyc_Absyn_Size_of _tmp547;enum Cyc_Absyn_Size_of _tmp546;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp545.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp545.f2)->tag == 6){_LL508: _tmp547=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp545.f1)->f2;_tmp546=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp545.f2)->f2;_LL509:
# 2679
 return(_tmp547 == _tmp546  || _tmp547 == Cyc_Absyn_Int_sz  && _tmp546 == Cyc_Absyn_Long_sz) || 
_tmp547 == Cyc_Absyn_Long_sz  && _tmp546 == Cyc_Absyn_Int_sz;}else{goto _LL50A;}}else{_LL50A: _LL50B:
 return 0;}_LL507:;}
# 2687
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2690
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2695
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp549=({struct _tuple0 _tmp57F;_tmp57F.f1=t1;_tmp57F.f2=t2;_tmp57F;});struct _tuple0 _tmp54A=_tmp549;struct Cyc_Absyn_FnInfo _tmp57E;struct Cyc_Absyn_FnInfo _tmp57D;struct Cyc_Absyn_Datatypedecl*_tmp57C;struct Cyc_Absyn_Datatypefield*_tmp57B;struct Cyc_List_List*_tmp57A;struct Cyc_Absyn_Datatypedecl*_tmp579;struct Cyc_List_List*_tmp578;void*_tmp577;struct Cyc_Absyn_Tqual _tmp576;void*_tmp575;union Cyc_Absyn_Constraint*_tmp574;union Cyc_Absyn_Constraint*_tmp573;union Cyc_Absyn_Constraint*_tmp572;void*_tmp571;struct Cyc_Absyn_Tqual _tmp570;void*_tmp56F;union Cyc_Absyn_Constraint*_tmp56E;union Cyc_Absyn_Constraint*_tmp56D;union Cyc_Absyn_Constraint*_tmp56C;switch(*((int*)_tmp54A.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->tag == 5){_LL50D: _tmp577=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->f1).elt_typ;_tmp576=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->f1).elt_tq;_tmp575=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->f1).ptr_atts).rgn;_tmp574=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->f1).ptr_atts).nullable;_tmp573=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->f1).ptr_atts).bounds;_tmp572=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f1)->f1).ptr_atts).zero_term;_tmp571=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->f1).elt_typ;_tmp570=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->f1).elt_tq;_tmp56F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->f1).ptr_atts).rgn;_tmp56E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->f1).ptr_atts).nullable;_tmp56D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->f1).ptr_atts).bounds;_tmp56C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp54A.f2)->f1).ptr_atts).zero_term;_LL50E:
# 2703
 if(_tmp576.real_const  && !_tmp570.real_const)
return 0;
# 2706
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp574,_tmp56E) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp574)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56E))
return 0;
# 2710
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp572,_tmp56C) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp572)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56C))
return 0;
# 2714
if((!Cyc_Tcutil_unify(_tmp575,_tmp56F) && !Cyc_Tcenv_region_outlives(te,_tmp575,_tmp56F)) && !
Cyc_Tcutil_subtype(te,assume,_tmp575,_tmp56F))
return 0;
# 2718
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp573,_tmp56D)){
struct _tuple0 _tmp54B=({struct _tuple0 _tmp54F;_tmp54F.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp573);_tmp54F.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp56D);_tmp54F;});struct _tuple0 _tmp54C=_tmp54B;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp54D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp54C.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp54C.f2)->tag == 0){_LL516: _LL517:
 goto _LL515;}else{goto _LL51A;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp54C.f2)->tag == 1){_LL518: _tmp54E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp54C.f1)->f1;_tmp54D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp54C.f2)->f1;_LL519:
# 2723
 if(!Cyc_Evexp_lte_const_exp(_tmp54D,_tmp54E))
return 0;
goto _LL515;}else{_LL51A: _LL51B:
 return 0;}}_LL515:;}
# 2731
if(!_tmp570.real_const  && _tmp576.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp571)))
return 0;}{
# 2737
int _tmp550=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp56D,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp56C);
# 2743
return(_tmp550  && Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->hd=({struct _tuple0*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->f1=t1;_tmp552->f2=t2;_tmp552;});_tmp551->tl=assume;_tmp551;}),_tmp577,_tmp571) || Cyc_Tcutil_unify(_tmp577,_tmp571)) || Cyc_Tcutil_isomorphic(_tmp577,_tmp571);};}else{goto _LL513;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54A.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp54A.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp54A.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL50F: _tmp57C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54A.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp57B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54A.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp57A=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp54A.f1)->f1).targs;_tmp579=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp54A.f2)->f1).datatype_info).KnownDatatype).val;_tmp578=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp54A.f2)->f1).targs;_LL510:
# 2749
 if(_tmp57C != _tmp579  && Cyc_Absyn_qvar_cmp(_tmp57C->name,_tmp579->name)!= 0)return 0;
# 2751
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57A)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp578))return 0;
for(0;_tmp57A != 0;(_tmp57A=_tmp57A->tl,_tmp578=_tmp578->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp57A->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp578))->hd))return 0;}
return 1;}else{goto _LL513;}}else{goto _LL513;}}else{goto _LL513;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp54A.f2)->tag == 9){_LL511: _tmp57E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp54A.f1)->f1;_tmp57D=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp54A.f2)->f1;_LL512:
# 2758
 if(_tmp57E.tvars != 0  || _tmp57D.tvars != 0){
struct Cyc_List_List*_tmp553=_tmp57E.tvars;
struct Cyc_List_List*_tmp554=_tmp57D.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp553)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp554))return 0;{
struct _RegionHandle _tmp555=_new_region("r");struct _RegionHandle*r=& _tmp555;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp553 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp553->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp554))->hd)->kind)){int _tmp556=0;_npop_handler(0);return _tmp556;}
inst=({struct Cyc_List_List*_tmp557=_region_malloc(r,sizeof(*_tmp557));_tmp557->hd=({struct _tuple16*_tmp558=_region_malloc(r,sizeof(*_tmp558));_tmp558->f1=(struct Cyc_Absyn_Tvar*)_tmp554->hd;_tmp558->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp55A;_tmp55A.tag=2;_tmp55A.f1=(struct Cyc_Absyn_Tvar*)_tmp553->hd;_tmp55A;});_tmp559;});_tmp558;});_tmp557->tl=inst;_tmp557;});
_tmp553=_tmp553->tl;
_tmp554=_tmp554->tl;}
# 2770
if(inst != 0){
_tmp57E.tvars=0;
_tmp57D.tvars=0;{
int _tmp55F=Cyc_Tcutil_subtype(te,assume,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp55B=_cycalloc(sizeof(*_tmp55B));_tmp55B[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp55C;_tmp55C.tag=9;_tmp55C.f1=_tmp57E;_tmp55C;});_tmp55B;}),(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp55E;_tmp55E.tag=9;_tmp55E.f1=_tmp57D;_tmp55E;});_tmp55D;}));_npop_handler(0);return _tmp55F;};}}
# 2763
;_pop_region(r);};}
# 2778
if(!Cyc_Tcutil_subtype(te,assume,_tmp57E.ret_typ,_tmp57D.ret_typ))return 0;{
struct Cyc_List_List*_tmp560=_tmp57E.args;
struct Cyc_List_List*_tmp561=_tmp57D.args;
# 2783
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp560)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp561))return 0;
# 2785
for(0;_tmp560 != 0;(_tmp560=_tmp560->tl,_tmp561=_tmp561->tl)){
struct _tuple10 _tmp562=*((struct _tuple10*)_tmp560->hd);struct _tuple10 _tmp563=_tmp562;struct Cyc_Absyn_Tqual _tmp569;void*_tmp568;_LL51D: _tmp569=_tmp563.f2;_tmp568=_tmp563.f3;_LL51E:;{
struct _tuple10 _tmp564=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp561))->hd);struct _tuple10 _tmp565=_tmp564;struct Cyc_Absyn_Tqual _tmp567;void*_tmp566;_LL520: _tmp567=_tmp565.f2;_tmp566=_tmp565.f3;_LL521:;
# 2789
if(_tmp567.real_const  && !_tmp569.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp566,_tmp568))
return 0;};}
# 2793
if(_tmp57E.c_varargs != _tmp57D.c_varargs)return 0;
if(_tmp57E.cyc_varargs != 0  && _tmp57D.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp56A=*_tmp57E.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp56B=*_tmp57D.cyc_varargs;
# 2798
if((_tmp56B.tq).real_const  && !(_tmp56A.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp56B.type,_tmp56A.type))
return 0;}else{
if(_tmp57E.cyc_varargs != 0  || _tmp57D.cyc_varargs != 0)return 0;}
# 2803
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp57E.effect),(void*)_check_null(_tmp57D.effect)))return 0;
# 2805
if(!Cyc_Tcutil_sub_rgnpo(_tmp57E.rgn_po,_tmp57D.rgn_po))return 0;
# 2807
if(!Cyc_Tcutil_sub_attributes(_tmp57E.attributes,_tmp57D.attributes))return 0;
# 2809
if(!Cyc_Tcutil_check_logical_implication(_tmp57D.requires_relns,_tmp57E.requires_relns))
return 0;
# 2812
if(!Cyc_Tcutil_check_logical_implication(_tmp57E.ensures_relns,_tmp57D.ensures_relns))
return 0;
# 2815
return 1;};}else{goto _LL513;}default: _LL513: _LL514:
 return 0;}_LL50C:;};}
# 2827 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2829
struct _RegionHandle _tmp580=_new_region("temp");struct _RegionHandle*temp=& _tmp580;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp581=0;_npop_handler(0);return _tmp581;}{
struct _tuple12*_tmp582=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp583=_tmp582;struct Cyc_Absyn_Tqual _tmp58B;void*_tmp58A;_LL523: _tmp58B=_tmp583->f1;_tmp58A=_tmp583->f2;_LL524:;{
struct _tuple12*_tmp584=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp585=_tmp584;struct Cyc_Absyn_Tqual _tmp589;void*_tmp588;_LL526: _tmp589=_tmp585->f1;_tmp588=_tmp585->f2;_LL527:;
# 2837
if(_tmp58B.real_const  && !_tmp589.real_const){int _tmp586=0;_npop_handler(0);return _tmp586;}
# 2839
if((_tmp589.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp588))) && 
Cyc_Tcutil_subtype(te,assume,_tmp58A,_tmp588))
# 2842
continue;
# 2844
if(Cyc_Tcutil_unify(_tmp58A,_tmp588))
# 2846
continue;
# 2848
if(Cyc_Tcutil_isomorphic(_tmp58A,_tmp588))
# 2850
continue;{
# 2853
int _tmp587=0;_npop_handler(0);return _tmp587;};};};}{
# 2855
int _tmp58C=1;_npop_handler(0);return _tmp58C;};}
# 2830
;_pop_region(temp);}
# 2859
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp58D=Cyc_Tcutil_compress(t);void*_tmp58E=_tmp58D;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp58E)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp58E)->f2 == Cyc_Absyn_Char_sz){_LL529: _LL52A:
 return 1;}else{goto _LL52B;}}else{_LL52B: _LL52C:
 return 0;}_LL528:;}
# 2868
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2874
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp58F=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp58F)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp58F)->f2){case Cyc_Absyn_Int_sz: _LL52E: _LL52F:
# 2878
 goto _LL531;case Cyc_Absyn_Long_sz: _LL530: _LL531:
# 2880
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL52D;default: goto _LL532;}else{_LL532: _LL533:
 goto _LL52D;}_LL52D:;}{
# 2884
void*_tmp590=t1;void*_tmp5B8;struct Cyc_Absyn_Enumdecl*_tmp5B7;void*_tmp5B6;struct Cyc_Absyn_Tqual _tmp5B5;struct Cyc_Absyn_Exp*_tmp5B4;union Cyc_Absyn_Constraint*_tmp5B3;void*_tmp5B2;struct Cyc_Absyn_Tqual _tmp5B1;void*_tmp5B0;union Cyc_Absyn_Constraint*_tmp5AF;union Cyc_Absyn_Constraint*_tmp5AE;union Cyc_Absyn_Constraint*_tmp5AD;switch(*((int*)_tmp590)){case 5: _LL535: _tmp5B2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp590)->f1).elt_typ;_tmp5B1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp590)->f1).elt_tq;_tmp5B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp590)->f1).ptr_atts).rgn;_tmp5AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp590)->f1).ptr_atts).nullable;_tmp5AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp590)->f1).ptr_atts).bounds;_tmp5AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp590)->f1).ptr_atts).zero_term;_LL536:
# 2893
{void*_tmp591=t2;void*_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A1;union Cyc_Absyn_Constraint*_tmp5A0;union Cyc_Absyn_Constraint*_tmp59F;union Cyc_Absyn_Constraint*_tmp59E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->tag == 5){_LL544: _tmp5A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).elt_typ;_tmp5A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).elt_tq;_tmp5A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).ptr_atts).rgn;_tmp5A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).ptr_atts).nullable;_tmp59F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).ptr_atts).bounds;_tmp59E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp591)->f1).ptr_atts).zero_term;_LL545: {
# 2897
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp592=({struct Cyc_List_List*_tmp59C=_cycalloc(sizeof(*_tmp59C));_tmp59C->hd=({struct _tuple0*_tmp59D=_cycalloc(sizeof(*_tmp59D));_tmp59D->f1=t1;_tmp59D->f2=t2;_tmp59D;});_tmp59C->tl=0;_tmp59C;});
int _tmp593=_tmp5A2.real_const  || !_tmp5B1.real_const;
# 2911 "tcutil.cyc"
int _tmp594=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp59F,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp59E);
# 2915
int _tmp595=_tmp593  && (
((_tmp594  && Cyc_Tcutil_ptrsubtype(te,_tmp592,_tmp5B2,_tmp5A3) || 
Cyc_Tcutil_unify(_tmp5B2,_tmp5A3)) || Cyc_Tcutil_isomorphic(_tmp5B2,_tmp5A3)) || Cyc_Tcutil_unify(_tmp5A3,(void*)& Cyc_Absyn_VoidType_val));
# 2919
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5AD,_tmp59E) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp59E);
# 2923
int _tmp596=_tmp595?0:((Cyc_Tcutil_bits_only(_tmp5B2) && Cyc_Tcutil_is_char_type(_tmp5A3)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp59E)) && (
_tmp5A2.real_const  || !_tmp5B1.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5AE,_tmp59F);
if(!bounds_ok  && !_tmp596){
struct _tuple0 _tmp597=({struct _tuple0 _tmp59B;_tmp59B.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5AE);_tmp59B.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp59F);_tmp59B;});struct _tuple0 _tmp598=_tmp597;struct Cyc_Absyn_Exp*_tmp59A;struct Cyc_Absyn_Exp*_tmp599;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp598.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp598.f2)->tag == 1){_LL549: _tmp59A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp598.f1)->f1;_tmp599=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp598.f2)->f1;_LL54A:
# 2933
 if(Cyc_Evexp_lte_const_exp(_tmp599,_tmp59A))
bounds_ok=1;
goto _LL548;}else{goto _LL54B;}}else{_LL54B: _LL54C:
# 2938
 bounds_ok=1;goto _LL548;}_LL548:;}{
# 2940
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5AF);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A0);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2947
if(((bounds_ok  && zeroterm_ok) && (_tmp595  || _tmp596)) && (
Cyc_Tcutil_unify(_tmp5B0,_tmp5A1) || Cyc_Tcenv_region_outlives(te,_tmp5B0,_tmp5A1)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL546: _LL547:
 goto _LL543;}_LL543:;}
# 2953
return Cyc_Absyn_Unknown_coercion;case 8: _LL537: _tmp5B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->f1).elt_type;_tmp5B5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->f1).tq;_tmp5B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->f1).num_elts;_tmp5B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->f1).zero_term;_LL538:
# 2955
{void*_tmp5A4=t2;void*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A7;struct Cyc_Absyn_Exp*_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->tag == 8){_LL54E: _tmp5A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).elt_type;_tmp5A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).tq;_tmp5A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).num_elts;_tmp5A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).zero_term;_LL54F: {
# 2957
int okay;
okay=
(((_tmp5B4 != 0  && _tmp5A6 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B3,_tmp5A5)) && 
Cyc_Evexp_lte_const_exp(_tmp5A6,_tmp5B4)) && 
Cyc_Evexp_lte_const_exp(_tmp5B4,_tmp5A6);
return
# 2964
(okay  && Cyc_Tcutil_unify(_tmp5B6,_tmp5A8)) && (!_tmp5B5.real_const  || _tmp5A7.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL550: _LL551:
# 2966
 return Cyc_Absyn_Unknown_coercion;}_LL54D:;}
# 2968
return Cyc_Absyn_Unknown_coercion;case 13: _LL539: _tmp5B7=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp590)->f2;_LL53A:
# 2972
{void*_tmp5A9=t2;struct Cyc_Absyn_Enumdecl*_tmp5AA;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A9)->tag == 13){_LL553: _tmp5AA=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A9)->f2;_LL554:
# 2974
 if((_tmp5B7->fields != 0  && _tmp5AA->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5B7->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5AA->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL552;}else{_LL555: _LL556:
 goto _LL552;}_LL552:;}
# 2980
goto _LL53C;case 6: _LL53B: _LL53C:
 goto _LL53E;case 7: _LL53D: _LL53E:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL53F: _tmp5B8=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp590)->f1;_LL540:
# 2985
{void*_tmp5AB=t2;void*_tmp5AC;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5AB)->tag == 15){_LL558: _tmp5AC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5AB)->f1;_LL559:
# 2987
 if(Cyc_Tcenv_region_outlives(te,_tmp5B8,_tmp5AC))return Cyc_Absyn_No_coercion;
goto _LL557;}else{_LL55A: _LL55B:
 goto _LL557;}_LL557:;}
# 2991
return Cyc_Absyn_Unknown_coercion;default: _LL541: _LL542:
 return Cyc_Absyn_Unknown_coercion;}_LL534:;};}
# 2997
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5B9=Cyc_Absyn_copy_exp(e);
e->r=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5BB;_tmp5BB.tag=13;_tmp5BB.f1=t;_tmp5BB.f2=_tmp5B9;_tmp5BB.f3=0;_tmp5BB.f4=c;_tmp5BB;});_tmp5BA;});
e->topt=t;}}
# 3005
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5BC=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5BD=_tmp5BC;switch(*((int*)_tmp5BD)){case 6: _LL55D: _LL55E:
 goto _LL560;case 13: _LL55F: _LL560:
 goto _LL562;case 14: _LL561: _LL562:
 goto _LL564;case 19: _LL563: _LL564:
 return 1;case 1: _LL565: _LL566:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LL567: _LL568:
 return 0;}_LL55C:;}
# 3016
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5BE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5BF=_tmp5BE;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5BF)->tag == 7){_LL56A: _LL56B:
 return 1;}else{_LL56C: _LL56D:
 return 0;}_LL569:;};}
# 3025
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5C0=Cyc_Tcutil_compress(t);void*_tmp5C1=_tmp5C0;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5C1)->tag == 9){_LL56F: _LL570:
 return 1;}else{_LL571: _LL572:
 return 0;}_LL56E:;}
# 3032
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5C2=({struct _tuple0 _tmp5C6;_tmp5C6.f1=t1;_tmp5C6.f2=t2;_tmp5C6;});struct _tuple0 _tmp5C3=_tmp5C2;int _tmp5C5;int _tmp5C4;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C3.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C3.f2)->tag == 7){_LL574: _tmp5C5=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C3.f1)->f1;_tmp5C4=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C3.f2)->f1;_LL575:
# 3035
 if(_tmp5C5 != 0  && _tmp5C5 != 1)return t1;else{
if(_tmp5C4 != 0  && _tmp5C4 != 1)return t2;else{
if(_tmp5C5 >= _tmp5C4)return t1;else{
return t2;}}}}else{_LL576: _LL577:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C3.f2)->tag == 7){_LL578: _LL579:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL57A: _LL57B:
 goto _LL57D;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL582;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL584;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL586;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL588;else{goto _LL58E;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL582;case Cyc_Absyn_Int_sz: goto _LL586;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2){case Cyc_Absyn_Long_sz: _LL582: _LL583:
# 3045
 goto _LL585;case Cyc_Absyn_Int_sz: _LL586: _LL587:
# 3048
 goto _LL589;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL584;case Cyc_Absyn_Int_sz: goto _LL588;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{goto _LL58E;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL57E: _LL57F:
# 3043
 goto _LL581;case Cyc_Absyn_Long_sz: _LL58A: _LL58B:
# 3050
 goto _LL58D;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL57C: _LL57D:
# 3042
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL584: _LL585:
# 3046
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL588: _LL589:
# 3049
 return Cyc_Absyn_uint_typ;default: goto _LL58E;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C3.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL580: _LL581:
# 3044
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL58C: _LL58D:
# 3051
 return Cyc_Absyn_slong_typ;default: goto _LL58E;}}}else{_LL58E: _LL58F:
 return Cyc_Absyn_sint_typ;}}}}_LL573:;}
# 3063 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp5C7=({struct _tuple0 _tmp5DD;_tmp5DD.f1=c1;_tmp5DD.f2=c2;_tmp5DD;});struct _tuple0 _tmp5C8=_tmp5C7;struct Cyc_Core_Opt**_tmp5DC;struct Cyc_Absyn_Kind*_tmp5DB;struct Cyc_Core_Opt**_tmp5DA;struct Cyc_Absyn_Kind*_tmp5D9;struct Cyc_Core_Opt**_tmp5D8;struct Cyc_Absyn_Kind*_tmp5D7;struct Cyc_Absyn_Kind*_tmp5D6;struct Cyc_Core_Opt**_tmp5D5;struct Cyc_Core_Opt**_tmp5D4;struct Cyc_Absyn_Kind*_tmp5D3;struct Cyc_Core_Opt**_tmp5D2;struct Cyc_Absyn_Kind*_tmp5D1;struct Cyc_Absyn_Kind*_tmp5D0;struct Cyc_Absyn_Kind*_tmp5CF;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->tag == 0)switch(*((int*)_tmp5C8.f2)){case 0: _LL591: _tmp5D0=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->f1;_tmp5CF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->f1;_LL592:
 return _tmp5D0 == _tmp5CF;case 1: goto _LL593;default: _LL599: _tmp5D3=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->f1;_tmp5D2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->f1;_tmp5D1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->f2;_LL59A:
# 3076
 if(Cyc_Tcutil_kind_leq(_tmp5D3,_tmp5D1)){
*_tmp5D2=({struct Cyc_Core_Opt*_tmp5CC=_cycalloc(sizeof(*_tmp5CC));_tmp5CC->v=c1;_tmp5CC;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->tag == 1){_LL593: _tmp5D4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->f1;_LL594:
# 3069
*_tmp5D4=({struct Cyc_Core_Opt*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));_tmp5C9->v=c1;_tmp5C9;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->tag == 1){_LL595: _tmp5D5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->f1;_LL596:
*_tmp5D5=({struct Cyc_Core_Opt*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));_tmp5CA->v=c2;_tmp5CA;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->tag == 0){_LL597: _tmp5D8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->f1;_tmp5D7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->f2;_tmp5D6=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->f1;_LL598:
# 3072
 if(Cyc_Tcutil_kind_leq(_tmp5D6,_tmp5D7)){
*_tmp5D8=({struct Cyc_Core_Opt*_tmp5CB=_cycalloc(sizeof(*_tmp5CB));_tmp5CB->v=c2;_tmp5CB;});return 1;}else{
return 0;}}else{_LL59B: _tmp5DC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->f1;_tmp5DB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C8.f1)->f2;_tmp5DA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->f1;_tmp5D9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C8.f2)->f2;_LL59C:
# 3080
 if(Cyc_Tcutil_kind_leq(_tmp5DB,_tmp5D9)){
*_tmp5DA=({struct Cyc_Core_Opt*_tmp5CD=_cycalloc(sizeof(*_tmp5CD));_tmp5CD->v=c1;_tmp5CD;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5D9,_tmp5DB)){
*_tmp5DC=({struct Cyc_Core_Opt*_tmp5CE=_cycalloc(sizeof(*_tmp5CE));_tmp5CE->v=c2;_tmp5CE;});return 1;}else{
return 0;}}}}}}_LL590:;};}
# 3089
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3094
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5E2;_tmp5E2.tag=1;_tmp5E2.f1=(unsigned long)i;({void*_tmp5E0[1]={& _tmp5E2};Cyc_aprintf(({const char*_tmp5E1="#%d";_tag_dyneither(_tmp5E1,sizeof(char),4);}),_tag_dyneither(_tmp5E0,sizeof(void*),1));});});
return({struct Cyc_Absyn_Tvar*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->name=({struct _dyneither_ptr*_tmp5DF=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5DF[0]=(struct _dyneither_ptr)s;_tmp5DF;});_tmp5DE->identity=- 1;_tmp5DE->kind=k;_tmp5DE;});}
# 3101
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5E3=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5E3,sizeof(char),0))== '#';}
# 3106
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5E6;_tmp5E6.tag=0;_tmp5E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5E4[1]={& _tmp5E6};Cyc_printf(({const char*_tmp5E5="%s";_tag_dyneither(_tmp5E5,sizeof(char),3);}),_tag_dyneither(_tmp5E4,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp5E7=Cyc_strconcat(({const char*_tmp5EC="`";_tag_dyneither(_tmp5EC,sizeof(char),2);}),(struct _dyneither_ptr)*t->name);
({struct _dyneither_ptr _tmp5E8=_dyneither_ptr_plus(_tmp5E7,sizeof(char),1);char _tmp5E9=*((char*)_check_dyneither_subscript(_tmp5E8,sizeof(char),0));char _tmp5EA='t';if(_get_dyneither_size(_tmp5E8,sizeof(char))== 1  && (_tmp5E9 == '\000'  && _tmp5EA != '\000'))_throw_arraybounds();*((char*)_tmp5E8.curr)=_tmp5EA;});
t->name=({struct _dyneither_ptr*_tmp5EB=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5EB[0]=(struct _dyneither_ptr)_tmp5E7;_tmp5EB;});};}
# 3115
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3117
return({struct _tuple10*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->f1=(*x).f1;_tmp5ED->f2=(*x).f2;_tmp5ED->f3=(*x).f3;_tmp5ED;});}
# 3120
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3127
struct Cyc_List_List*_tmp5EE=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp5EE=({struct Cyc_List_List*_tmp5EF=_cycalloc(sizeof(*_tmp5EF));_tmp5EF->hd=(void*)atts->hd;_tmp5EF->tl=_tmp5EE;_tmp5EF;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5F0=_cycalloc(sizeof(*_tmp5F0));_tmp5F0[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp5F1;_tmp5F1.tag=9;_tmp5F1.f1=({struct Cyc_Absyn_FnInfo _tmp5F2;_tmp5F2.tvars=fd->tvs;_tmp5F2.effect=fd->effect;_tmp5F2.ret_tqual=fd->ret_tqual;_tmp5F2.ret_typ=fd->ret_type;_tmp5F2.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp5F2.c_varargs=fd->c_varargs;_tmp5F2.cyc_varargs=fd->cyc_varargs;_tmp5F2.rgn_po=fd->rgn_po;_tmp5F2.attributes=_tmp5EE;_tmp5F2.requires_clause=fd->requires_clause;_tmp5F2.requires_relns=fd->requires_relns;_tmp5F2.ensures_clause=fd->ensures_clause;_tmp5F2.ensures_relns=fd->ensures_relns;_tmp5F2;});_tmp5F1;});_tmp5F0;});}
# 3140
return(void*)_check_null(fd->cached_typ);}
# 3146
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3148
union Cyc_Relations_RelnOp _tmp5F3=*rop;union Cyc_Relations_RelnOp _tmp5F4=_tmp5F3;struct Cyc_Absyn_Vardecl*_tmp607;struct Cyc_Absyn_Vardecl*_tmp606;switch((_tmp5F4.RNumelts).tag){case 2: _LL59E: _tmp606=(_tmp5F4.RVar).val;_LL59F: {
# 3150
struct _tuple2 _tmp5F5=*_tmp606->name;struct _tuple2 _tmp5F6=_tmp5F5;union Cyc_Absyn_Nmspace _tmp5FD;struct _dyneither_ptr*_tmp5FC;_LL5A5: _tmp5FD=_tmp5F6.f1;_tmp5FC=_tmp5F6.f2;_LL5A6:;
if(!((int)((_tmp5FD.Loc_n).tag == 4)))goto _LL59D;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5FC,({const char*_tmp5F7="return_value";_tag_dyneither(_tmp5F7,sizeof(char),13);}))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL59D;}{
# 3156
unsigned int c=0;
{struct Cyc_List_List*_tmp5F8=args;for(0;_tmp5F8 != 0;(_tmp5F8=_tmp5F8->tl,c ++)){
struct _tuple10*_tmp5F9=(struct _tuple10*)_tmp5F8->hd;struct _tuple10*_tmp5FA=_tmp5F9;struct _dyneither_ptr*_tmp5FB;_LL5A8: _tmp5FB=_tmp5FA->f1;_LL5A9:;
if(_tmp5FB != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5FC,(struct _dyneither_ptr)*_tmp5FB)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3166
goto _LL59D;};}case 3: _LL5A0: _tmp607=(_tmp5F4.RNumelts).val;_LL5A1: {
# 3168
struct _tuple2 _tmp5FE=*_tmp607->name;struct _tuple2 _tmp5FF=_tmp5FE;union Cyc_Absyn_Nmspace _tmp605;struct _dyneither_ptr*_tmp604;_LL5AB: _tmp605=_tmp5FF.f1;_tmp604=_tmp5FF.f2;_LL5AC:;
if(!((int)((_tmp605.Loc_n).tag == 4)))goto _LL59D;{
unsigned int c=0;
{struct Cyc_List_List*_tmp600=args;for(0;_tmp600 != 0;(_tmp600=_tmp600->tl,c ++)){
struct _tuple10*_tmp601=(struct _tuple10*)_tmp600->hd;struct _tuple10*_tmp602=_tmp601;struct _dyneither_ptr*_tmp603;_LL5AE: _tmp603=_tmp602->f1;_LL5AF:;
if(_tmp603 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp604,(struct _dyneither_ptr)*_tmp603)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3180
goto _LL59D;};}default: _LL5A2: _LL5A3:
 goto _LL59D;}_LL59D:;}
# 3185
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3187
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3191
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3193
if(e == 0)return 0;{
struct Cyc_List_List*_tmp608=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp608);
return _tmp608;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3200
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp609=pr;struct Cyc_Absyn_Tqual _tmp60C;void*_tmp60B;_LL5B1: _tmp60C=_tmp609->f1;_tmp60B=_tmp609->f2;_LL5B2:;
if(_tmp60B == t)return pr;else{
return({struct _tuple12*_tmp60A=_cycalloc(sizeof(*_tmp60A));_tmp60A->f1=_tmp60C;_tmp60A->f2=t;_tmp60A;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3207
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3209
return({struct _tuple27*_tmp60D=_region_malloc(rgn,sizeof(*_tmp60D));_tmp60D->f1=({struct _tuple26*_tmp60E=_region_malloc(rgn,sizeof(*_tmp60E));_tmp60E->f1=(*y).f1;_tmp60E->f2=(*y).f2;_tmp60E;});_tmp60D->f2=(*y).f3;_tmp60D;});}
# 3211
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3213
struct _tuple10 _tmp60F=*orig_arg;struct _tuple10 _tmp610=_tmp60F;struct _dyneither_ptr*_tmp614;struct Cyc_Absyn_Tqual _tmp613;void*_tmp612;_LL5B4: _tmp614=_tmp610.f1;_tmp613=_tmp610.f2;_tmp612=_tmp610.f3;_LL5B5:;
if(t == _tmp612)return orig_arg;
return({struct _tuple10*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->f1=_tmp614;_tmp611->f2=_tmp613;_tmp611->f3=t;_tmp611;});}
# 3217
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3220
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3225
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3227
return({struct Cyc_Absyn_Exp*_tmp615=_cycalloc(sizeof(*_tmp615));_tmp615->topt=old->topt;_tmp615->r=r;_tmp615->loc=old->loc;_tmp615->annot=old->annot;_tmp615;});}
# 3232
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp616=e->r;void*_tmp617=_tmp616;void*_tmp65F;void*_tmp65E;struct Cyc_List_List*_tmp65D;struct Cyc_Absyn_Exp*_tmp65C;void*_tmp65B;void*_tmp65A;struct Cyc_Absyn_Exp*_tmp659;int _tmp658;enum Cyc_Absyn_Coercion _tmp657;struct Cyc_Absyn_Exp*_tmp656;struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Exp*_tmp654;struct Cyc_Absyn_Exp*_tmp653;struct Cyc_Absyn_Exp*_tmp652;struct Cyc_Absyn_Exp*_tmp651;struct Cyc_Absyn_Exp*_tmp650;struct Cyc_Absyn_Exp*_tmp64F;struct Cyc_Absyn_Exp*_tmp64E;enum Cyc_Absyn_Primop _tmp64D;struct Cyc_List_List*_tmp64C;switch(*((int*)_tmp617)){case 0: _LL5B7: _LL5B8:
 goto _LL5BA;case 31: _LL5B9: _LL5BA:
 goto _LL5BC;case 32: _LL5BB: _LL5BC:
 goto _LL5BE;case 1: _LL5BD: _LL5BE:
 return e;case 2: _LL5BF: _tmp64D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_tmp64C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp617)->f2;_LL5C0:
# 3240
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp64C)== 1){
struct Cyc_Absyn_Exp*_tmp618=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp64C))->hd;
struct Cyc_Absyn_Exp*_tmp619=Cyc_Tcutil_rsubsexp(r,inst,_tmp618);
if(_tmp619 == _tmp618)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp61A=_cycalloc(sizeof(*_tmp61A));_tmp61A[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp61B;_tmp61B.tag=2;_tmp61B.f1=_tmp64D;_tmp61B.f2=({struct Cyc_Absyn_Exp*_tmp61C[1];_tmp61C[0]=_tmp619;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61C,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp61B;});_tmp61A;}));}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp64C)== 2){
struct Cyc_Absyn_Exp*_tmp61D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp64C))->hd;
struct Cyc_Absyn_Exp*_tmp61E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp64C->tl))->hd;
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Tcutil_rsubsexp(r,inst,_tmp61D);
struct Cyc_Absyn_Exp*_tmp620=Cyc_Tcutil_rsubsexp(r,inst,_tmp61E);
if(_tmp61F == _tmp61D  && _tmp620 == _tmp61E)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp621=_cycalloc(sizeof(*_tmp621));_tmp621[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp622;_tmp622.tag=2;_tmp622.f1=_tmp64D;_tmp622.f2=({struct Cyc_Absyn_Exp*_tmp623[2];_tmp623[1]=_tmp620;_tmp623[0]=_tmp61F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp623,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp622;});_tmp621;}));}else{
return({void*_tmp624=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp625="primop does not have 1 or 2 args!";_tag_dyneither(_tmp625,sizeof(char),34);}),_tag_dyneither(_tmp624,sizeof(void*),0));});}}case 5: _LL5C1: _tmp650=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_tmp64F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp617)->f2;_tmp64E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp617)->f3;_LL5C2: {
# 3254
struct Cyc_Absyn_Exp*_tmp626=Cyc_Tcutil_rsubsexp(r,inst,_tmp650);
struct Cyc_Absyn_Exp*_tmp627=Cyc_Tcutil_rsubsexp(r,inst,_tmp64F);
struct Cyc_Absyn_Exp*_tmp628=Cyc_Tcutil_rsubsexp(r,inst,_tmp64E);
if((_tmp626 == _tmp650  && _tmp627 == _tmp64F) && _tmp628 == _tmp64E)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp629=_cycalloc(sizeof(*_tmp629));_tmp629[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp62A;_tmp62A.tag=5;_tmp62A.f1=_tmp626;_tmp62A.f2=_tmp627;_tmp62A.f3=_tmp628;_tmp62A;});_tmp629;}));}case 6: _LL5C3: _tmp652=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_tmp651=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp617)->f2;_LL5C4: {
# 3260
struct Cyc_Absyn_Exp*_tmp62B=Cyc_Tcutil_rsubsexp(r,inst,_tmp652);
struct Cyc_Absyn_Exp*_tmp62C=Cyc_Tcutil_rsubsexp(r,inst,_tmp651);
if(_tmp62B == _tmp652  && _tmp62C == _tmp651)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));_tmp62D[0]=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp62E;_tmp62E.tag=6;_tmp62E.f1=_tmp62B;_tmp62E.f2=_tmp62C;_tmp62E;});_tmp62D;}));}case 7: _LL5C5: _tmp654=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_tmp653=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp617)->f2;_LL5C6: {
# 3265
struct Cyc_Absyn_Exp*_tmp62F=Cyc_Tcutil_rsubsexp(r,inst,_tmp654);
struct Cyc_Absyn_Exp*_tmp630=Cyc_Tcutil_rsubsexp(r,inst,_tmp653);
if(_tmp62F == _tmp654  && _tmp630 == _tmp653)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp631=_cycalloc(sizeof(*_tmp631));_tmp631[0]=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp632;_tmp632.tag=7;_tmp632.f1=_tmp62F;_tmp632.f2=_tmp630;_tmp632;});_tmp631;}));}case 8: _LL5C7: _tmp656=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_tmp655=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp617)->f2;_LL5C8: {
# 3270
struct Cyc_Absyn_Exp*_tmp633=Cyc_Tcutil_rsubsexp(r,inst,_tmp656);
struct Cyc_Absyn_Exp*_tmp634=Cyc_Tcutil_rsubsexp(r,inst,_tmp655);
if(_tmp633 == _tmp656  && _tmp634 == _tmp655)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp635=_cycalloc(sizeof(*_tmp635));_tmp635[0]=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp636;_tmp636.tag=8;_tmp636.f1=_tmp633;_tmp636.f2=_tmp634;_tmp636;});_tmp635;}));}case 13: _LL5C9: _tmp65A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_tmp659=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp617)->f2;_tmp658=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp617)->f3;_tmp657=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp617)->f4;_LL5CA: {
# 3275
struct Cyc_Absyn_Exp*_tmp637=Cyc_Tcutil_rsubsexp(r,inst,_tmp659);
void*_tmp638=Cyc_Tcutil_rsubstitute(r,inst,_tmp65A);
if(_tmp637 == _tmp659  && _tmp638 == _tmp65A)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp63A;_tmp63A.tag=13;_tmp63A.f1=_tmp638;_tmp63A.f2=_tmp637;_tmp63A.f3=_tmp658;_tmp63A.f4=_tmp657;_tmp63A;});_tmp639;}));}case 16: _LL5CB: _tmp65B=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_LL5CC: {
# 3280
void*_tmp63B=Cyc_Tcutil_rsubstitute(r,inst,_tmp65B);
if(_tmp63B == _tmp65B)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp63C=_cycalloc(sizeof(*_tmp63C));_tmp63C[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp63D;_tmp63D.tag=16;_tmp63D.f1=_tmp63B;_tmp63D;});_tmp63C;}));}case 17: _LL5CD: _tmp65C=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_LL5CE: {
# 3284
struct Cyc_Absyn_Exp*_tmp63E=Cyc_Tcutil_rsubsexp(r,inst,_tmp65C);
if(_tmp63E == _tmp65C)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp640;_tmp640.tag=17;_tmp640.f1=_tmp63E;_tmp640;});_tmp63F;}));}case 18: _LL5CF: _tmp65E=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_tmp65D=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp617)->f2;_LL5D0: {
# 3288
void*_tmp641=Cyc_Tcutil_rsubstitute(r,inst,_tmp65E);
if(_tmp641 == _tmp65E)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp643;_tmp643.tag=18;_tmp643.f1=_tmp641;_tmp643.f2=_tmp65D;_tmp643;});_tmp642;}));}case 38: _LL5D1: _tmp65F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp617)->f1;_LL5D2: {
# 3292
void*_tmp644=Cyc_Tcutil_rsubstitute(r,inst,_tmp65F);
if(_tmp644 == _tmp65F)return e;{
# 3295
void*_tmp645=Cyc_Tcutil_compress(_tmp644);void*_tmp646=_tmp645;struct Cyc_Absyn_Exp*_tmp649;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp646)->tag == 18){_LL5D6: _tmp649=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp646)->f1;_LL5D7:
 return _tmp649;}else{_LL5D8: _LL5D9:
# 3298
 return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp648;_tmp648.tag=38;_tmp648.f1=_tmp644;_tmp648;});_tmp647;}));}_LL5D5:;};}default: _LL5D3: _LL5D4:
# 3301
 return({void*_tmp64A=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp64B="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp64B,sizeof(char),46);}),_tag_dyneither(_tmp64A,sizeof(void*),0));});}_LL5B6:;}
# 3305
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3308
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3312
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3315
void*_tmp660=f->type;
struct Cyc_Absyn_Exp*_tmp661=f->requires_clause;
void*_tmp662=Cyc_Tcutil_rsubstitute(r,inst,_tmp660);
struct Cyc_Absyn_Exp*_tmp663=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp661);
if(_tmp660 == _tmp662  && _tmp661 == _tmp663)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp664=_cycalloc(sizeof(*_tmp664));_tmp664->name=f->name;_tmp664->tq=f->tq;_tmp664->type=_tmp662;_tmp664->width=f->width;_tmp664->attributes=f->attributes;_tmp664->requires_clause=_tmp663;_tmp664;});}}
# 3325
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3328
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp665=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp666=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp667=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp665);
struct Cyc_List_List*_tmp668=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp666);
if(_tmp667 == _tmp665  && _tmp668 == _tmp666)return fs;
# 3335
return({struct Cyc_List_List*_tmp669=_cycalloc(sizeof(*_tmp669));_tmp669->hd=_tmp667;_tmp669->tl=_tmp668;_tmp669;});};}
# 3338
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3341
struct _tuple1 _tmp66A=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp66B=_tmp66A;struct Cyc_List_List*_tmp66F;struct Cyc_List_List*_tmp66E;_LL5DB: _tmp66F=_tmp66B.f1;_tmp66E=_tmp66B.f2;_LL5DC:;{
struct Cyc_List_List*_tmp66C=Cyc_Tcutil_substs(rgn,inst,_tmp66F);
struct Cyc_List_List*_tmp66D=Cyc_Tcutil_substs(rgn,inst,_tmp66E);
if(_tmp66C == _tmp66F  && _tmp66D == _tmp66E)
return rgn_po;else{
# 3347
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp66C,_tmp66D);}};}
# 3350
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3353
void*_tmp670=Cyc_Tcutil_compress(t);void*_tmp671=_tmp670;struct Cyc_List_List*_tmp702;void*_tmp701;void*_tmp700;struct Cyc_Absyn_Exp*_tmp6FF;struct Cyc_Absyn_Exp*_tmp6FE;void*_tmp6FD;void*_tmp6FC;void*_tmp6FB;void*_tmp6FA;void*_tmp6F9;enum Cyc_Absyn_AggrKind _tmp6F8;struct Cyc_List_List*_tmp6F7;struct Cyc_List_List*_tmp6F6;struct Cyc_List_List*_tmp6F5;void*_tmp6F4;struct Cyc_Absyn_Tqual _tmp6F3;void*_tmp6F2;struct Cyc_List_List*_tmp6F1;int _tmp6F0;struct Cyc_Absyn_VarargInfo*_tmp6EF;struct Cyc_List_List*_tmp6EE;struct Cyc_List_List*_tmp6ED;struct Cyc_Absyn_Exp*_tmp6EC;struct Cyc_Absyn_Exp*_tmp6EB;void*_tmp6EA;struct Cyc_Absyn_Tqual _tmp6E9;void*_tmp6E8;union Cyc_Absyn_Constraint*_tmp6E7;union Cyc_Absyn_Constraint*_tmp6E6;union Cyc_Absyn_Constraint*_tmp6E5;void*_tmp6E4;struct Cyc_Absyn_Tqual _tmp6E3;struct Cyc_Absyn_Exp*_tmp6E2;union Cyc_Absyn_Constraint*_tmp6E1;unsigned int _tmp6E0;struct _tuple2*_tmp6DF;struct Cyc_List_List*_tmp6DE;struct Cyc_Absyn_Typedefdecl*_tmp6DD;void*_tmp6DC;union Cyc_Absyn_DatatypeFieldInfoU _tmp6DB;struct Cyc_List_List*_tmp6DA;union Cyc_Absyn_DatatypeInfoU _tmp6D9;struct Cyc_List_List*_tmp6D8;union Cyc_Absyn_AggrInfoU _tmp6D7;struct Cyc_List_List*_tmp6D6;struct Cyc_Absyn_Tvar*_tmp6D5;switch(*((int*)_tmp671)){case 2: _LL5DE: _tmp6D5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp671)->f1;_LL5DF: {
# 3356
struct _handler_cons _tmp672;_push_handler(& _tmp672);{int _tmp674=0;if(setjmp(_tmp672.handler))_tmp674=1;if(!_tmp674){{void*_tmp675=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6D5);_npop_handler(0);return _tmp675;};_pop_handler();}else{void*_tmp673=(void*)_exn_thrown;void*_tmp676=_tmp673;void*_tmp677;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp676)->tag == Cyc_Core_Not_found){_LL619: _LL61A:
 return t;}else{_LL61B: _tmp677=_tmp676;_LL61C:(int)_rethrow(_tmp677);}_LL618:;}};}case 11: _LL5E0: _tmp6D7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp671)->f1).aggr_info;_tmp6D6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp671)->f1).targs;_LL5E1: {
# 3359
struct Cyc_List_List*_tmp678=Cyc_Tcutil_substs(rgn,inst,_tmp6D6);
return _tmp678 == _tmp6D6?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp67A;_tmp67A.tag=11;_tmp67A.f1=({struct Cyc_Absyn_AggrInfo _tmp67B;_tmp67B.aggr_info=_tmp6D7;_tmp67B.targs=_tmp678;_tmp67B;});_tmp67A;});_tmp679;});}case 3: _LL5E2: _tmp6D9=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp671)->f1).datatype_info;_tmp6D8=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp671)->f1).targs;_LL5E3: {
# 3362
struct Cyc_List_List*_tmp67C=Cyc_Tcutil_substs(rgn,inst,_tmp6D8);
return _tmp67C == _tmp6D8?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp67E;_tmp67E.tag=3;_tmp67E.f1=({struct Cyc_Absyn_DatatypeInfo _tmp67F;_tmp67F.datatype_info=_tmp6D9;_tmp67F.targs=_tmp67C;_tmp67F;});_tmp67E;});_tmp67D;});}case 4: _LL5E4: _tmp6DB=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp671)->f1).field_info;_tmp6DA=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp671)->f1).targs;_LL5E5: {
# 3366
struct Cyc_List_List*_tmp680=Cyc_Tcutil_substs(rgn,inst,_tmp6DA);
return _tmp680 == _tmp6DA?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp681=_cycalloc(sizeof(*_tmp681));_tmp681[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp682;_tmp682.tag=4;_tmp682.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp683;_tmp683.field_info=_tmp6DB;_tmp683.targs=_tmp680;_tmp683;});_tmp682;});_tmp681;});}case 17: _LL5E6: _tmp6DF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp671)->f1;_tmp6DE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp671)->f2;_tmp6DD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp671)->f3;_tmp6DC=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp671)->f4;_LL5E7: {
# 3370
struct Cyc_List_List*_tmp684=Cyc_Tcutil_substs(rgn,inst,_tmp6DE);
return _tmp684 == _tmp6DE?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp686;_tmp686.tag=17;_tmp686.f1=_tmp6DF;_tmp686.f2=_tmp684;_tmp686.f3=_tmp6DD;_tmp686.f4=_tmp6DC;_tmp686;});_tmp685;});}case 8: _LL5E8: _tmp6E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp671)->f1).elt_type;_tmp6E3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp671)->f1).tq;_tmp6E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp671)->f1).num_elts;_tmp6E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp671)->f1).zero_term;_tmp6E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp671)->f1).zt_loc;_LL5E9: {
# 3373
void*_tmp687=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E4);
struct Cyc_Absyn_Exp*_tmp688=_tmp6E2 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6E2);
return _tmp687 == _tmp6E4  && _tmp688 == _tmp6E2?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp68A;_tmp68A.tag=8;_tmp68A.f1=({struct Cyc_Absyn_ArrayInfo _tmp68B;_tmp68B.elt_type=_tmp687;_tmp68B.tq=_tmp6E3;_tmp68B.num_elts=_tmp688;_tmp68B.zero_term=_tmp6E1;_tmp68B.zt_loc=_tmp6E0;_tmp68B;});_tmp68A;});_tmp689;});}case 5: _LL5EA: _tmp6EA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp671)->f1).elt_typ;_tmp6E9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp671)->f1).elt_tq;_tmp6E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp671)->f1).ptr_atts).rgn;_tmp6E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp671)->f1).ptr_atts).nullable;_tmp6E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp671)->f1).ptr_atts).bounds;_tmp6E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp671)->f1).ptr_atts).zero_term;_LL5EB: {
# 3378
void*_tmp68C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EA);
void*_tmp68D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E8);
union Cyc_Absyn_Constraint*_tmp68E=_tmp6E6;
{void*_tmp68F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6E6);void*_tmp690=_tmp68F;struct Cyc_Absyn_Exp*_tmp694;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp690)->tag == 1){_LL61E: _tmp694=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp690)->f1;_LL61F: {
# 3383
struct Cyc_Absyn_Exp*_tmp691=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp694);
if(_tmp691 != _tmp694)
_tmp68E=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp693;_tmp693.tag=1;_tmp693.f1=_tmp691;_tmp693;});_tmp692;}));
goto _LL61D;}}else{_LL620: _LL621:
 goto _LL61D;}_LL61D:;}
# 3389
if((_tmp68C == _tmp6EA  && _tmp68D == _tmp6E8) && _tmp68E == _tmp6E6)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp696;_tmp696.tag=5;_tmp696.f1=({struct Cyc_Absyn_PtrInfo _tmp697;_tmp697.elt_typ=_tmp68C;_tmp697.elt_tq=_tmp6E9;_tmp697.ptr_atts=({(_tmp697.ptr_atts).rgn=_tmp68D;(_tmp697.ptr_atts).nullable=_tmp6E7;(_tmp697.ptr_atts).bounds=_tmp68E;(_tmp697.ptr_atts).zero_term=_tmp6E5;(_tmp697.ptr_atts).ptrloc=0;_tmp697.ptr_atts;});_tmp697;});_tmp696;});_tmp695;});}case 9: _LL5EC: _tmp6F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).tvars;_tmp6F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).effect;_tmp6F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).ret_tqual;_tmp6F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).ret_typ;_tmp6F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).args;_tmp6F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).c_varargs;_tmp6EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).cyc_varargs;_tmp6EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).rgn_po;_tmp6ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).attributes;_tmp6EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).requires_clause;_tmp6EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp671)->f1).ensures_clause;_LL5ED:
# 3395
{struct Cyc_List_List*_tmp698=_tmp6F5;for(0;_tmp698 != 0;_tmp698=_tmp698->tl){
inst=({struct Cyc_List_List*_tmp699=_region_malloc(rgn,sizeof(*_tmp699));_tmp699->hd=({struct _tuple16*_tmp69A=_region_malloc(rgn,sizeof(*_tmp69A));_tmp69A->f1=(struct Cyc_Absyn_Tvar*)_tmp698->hd;_tmp69A->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp69C;_tmp69C.tag=2;_tmp69C.f1=(struct Cyc_Absyn_Tvar*)_tmp698->hd;_tmp69C;});_tmp69B;});_tmp69A;});_tmp699->tl=inst;_tmp699;});}}{
struct _tuple1 _tmp69D=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6F1));
# 3397
struct _tuple1 _tmp69E=_tmp69D;struct Cyc_List_List*_tmp6AF;struct Cyc_List_List*_tmp6AE;_LL623: _tmp6AF=_tmp69E.f1;_tmp6AE=_tmp69E.f2;_LL624:;{
# 3399
struct Cyc_List_List*_tmp69F=_tmp6F1;
struct Cyc_List_List*_tmp6A0=Cyc_Tcutil_substs(rgn,inst,_tmp6AE);
if(_tmp6A0 != _tmp6AE)
_tmp69F=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp6F1,_tmp6A0);{
void*eff2;
if(_tmp6F4 == 0)
eff2=0;else{
# 3407
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F4);
if(new_eff == _tmp6F4)
eff2=_tmp6F4;else{
# 3411
eff2=new_eff;}}{
# 3413
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp6EF == 0)
cyc_varargs2=0;else{
# 3417
struct Cyc_Absyn_VarargInfo _tmp6A1=*_tmp6EF;struct Cyc_Absyn_VarargInfo _tmp6A2=_tmp6A1;struct _dyneither_ptr*_tmp6A8;struct Cyc_Absyn_Tqual _tmp6A7;void*_tmp6A6;int _tmp6A5;_LL626: _tmp6A8=_tmp6A2.name;_tmp6A7=_tmp6A2.tq;_tmp6A6=_tmp6A2.type;_tmp6A5=_tmp6A2.inject;_LL627:;{
void*_tmp6A3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A6);
if(_tmp6A3 == _tmp6A6)cyc_varargs2=_tmp6EF;else{
# 3421
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));_tmp6A4->name=_tmp6A8;_tmp6A4->tq=_tmp6A7;_tmp6A4->type=_tmp6A3;_tmp6A4->inject=_tmp6A5;_tmp6A4;});}};}{
# 3423
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp6EE);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6EC);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6EB);
struct Cyc_List_List*_tmp6A9=Cyc_Tcutil_extract_relns(_tmp69F,req2);
struct Cyc_List_List*_tmp6AA=Cyc_Tcutil_extract_relns(_tmp69F,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6AC;_tmp6AC.tag=9;_tmp6AC.f1=({struct Cyc_Absyn_FnInfo _tmp6AD;_tmp6AD.tvars=_tmp6F5;_tmp6AD.effect=eff2;_tmp6AD.ret_tqual=_tmp6F3;_tmp6AD.ret_typ=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F2);_tmp6AD.args=_tmp69F;_tmp6AD.c_varargs=_tmp6F0;_tmp6AD.cyc_varargs=cyc_varargs2;_tmp6AD.rgn_po=rgn_po2;_tmp6AD.attributes=_tmp6ED;_tmp6AD.requires_clause=req2;_tmp6AD.requires_relns=_tmp6A9;_tmp6AD.ensures_clause=ens2;_tmp6AD.ensures_relns=_tmp6AA;_tmp6AD;});_tmp6AC;});_tmp6AB;});};};};};};case 10: _LL5EE: _tmp6F6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp671)->f1;_LL5EF: {
# 3433
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6B0=_tmp6F6;for(0;_tmp6B0 != 0;_tmp6B0=_tmp6B0->tl){
void*_tmp6B1=(*((struct _tuple12*)_tmp6B0->hd)).f2;
void*_tmp6B2=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B1);
if(_tmp6B1 != _tmp6B2)
change=1;
# 3441
ts2=({struct Cyc_List_List*_tmp6B3=_region_malloc(rgn,sizeof(*_tmp6B3));_tmp6B3->hd=_tmp6B2;_tmp6B3->tl=ts2;_tmp6B3;});}}
# 3443
if(!change)
return t;{
struct Cyc_List_List*_tmp6B4=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp6F6,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));_tmp6B5[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6B6;_tmp6B6.tag=10;_tmp6B6.f1=_tmp6B4;_tmp6B6;});_tmp6B5;});};}case 12: _LL5F0: _tmp6F8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp671)->f1;_tmp6F7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp671)->f2;_LL5F1: {
# 3448
struct Cyc_List_List*_tmp6B7=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp6F7);
if(_tmp6F7 == _tmp6B7)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6B8=_cycalloc(sizeof(*_tmp6B8));_tmp6B8[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6B9;_tmp6B9.tag=12;_tmp6B9.f1=_tmp6F8;_tmp6B9.f2=_tmp6B7;_tmp6B9;});_tmp6B8;});}case 1: _LL5F2: _tmp6F9=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp671)->f2;_LL5F3:
# 3452
 if(_tmp6F9 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F9);else{
return t;}case 15: _LL5F4: _tmp6FA=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp671)->f1;_LL5F5: {
# 3455
void*_tmp6BA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FA);
return _tmp6BA == _tmp6FA?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6BC;_tmp6BC.tag=15;_tmp6BC.f1=_tmp6BA;_tmp6BC;});_tmp6BB;});}case 16: _LL5F6: _tmp6FC=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp671)->f1;_tmp6FB=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp671)->f2;_LL5F7: {
# 3458
void*_tmp6BD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FC);
void*_tmp6BE=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FB);
return _tmp6BD == _tmp6FC  && _tmp6BE == _tmp6FB?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp6C0;_tmp6C0.tag=16;_tmp6C0.f1=_tmp6BD;_tmp6C0.f2=_tmp6BE;_tmp6C0;});_tmp6BF;});}case 19: _LL5F8: _tmp6FD=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp671)->f1;_LL5F9: {
# 3462
void*_tmp6C1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FD);
return _tmp6C1 == _tmp6FD?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6C3;_tmp6C3.tag=19;_tmp6C3.f1=_tmp6C1;_tmp6C3;});_tmp6C2;});}case 18: _LL5FA: _tmp6FE=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp671)->f1;_LL5FB: {
# 3465
struct Cyc_Absyn_Exp*_tmp6C4=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6FE);
return _tmp6C4 == _tmp6FE?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6C6;_tmp6C6.tag=18;_tmp6C6.f1=_tmp6C4;_tmp6C6;});_tmp6C5;});}case 27: _LL5FC: _tmp6FF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp671)->f1;_LL5FD: {
# 3468
struct Cyc_Absyn_Exp*_tmp6C7=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6FF);
return _tmp6C7 == _tmp6FF?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp6C9;_tmp6C9.tag=27;_tmp6C9.f1=_tmp6C7;_tmp6C9;});_tmp6C8;});}case 13: _LL5FE: _LL5FF:
 goto _LL601;case 14: _LL600: _LL601:
 goto _LL603;case 0: _LL602: _LL603:
 goto _LL605;case 6: _LL604: _LL605:
 goto _LL607;case 7: _LL606: _LL607:
 goto _LL609;case 22: _LL608: _LL609:
 goto _LL60B;case 21: _LL60A: _LL60B:
 goto _LL60D;case 28: _LL60C: _LL60D:
 goto _LL60F;case 20: _LL60E: _LL60F:
 return t;case 25: _LL610: _tmp700=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp671)->f1;_LL611: {
# 3480
void*_tmp6CA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp700);
return _tmp6CA == _tmp700?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6CC;_tmp6CC.tag=25;_tmp6CC.f1=_tmp6CA;_tmp6CC;});_tmp6CB;});}case 23: _LL612: _tmp701=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp671)->f1;_LL613: {
# 3483
void*_tmp6CD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp701);
return _tmp6CD == _tmp701?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp6CF;_tmp6CF.tag=23;_tmp6CF.f1=_tmp6CD;_tmp6CF;});_tmp6CE;});}case 24: _LL614: _tmp702=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp671)->f1;_LL615: {
# 3486
struct Cyc_List_List*_tmp6D0=Cyc_Tcutil_substs(rgn,inst,_tmp702);
return _tmp6D0 == _tmp702?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6D2;_tmp6D2.tag=24;_tmp6D2.f1=_tmp6D0;_tmp6D2;});_tmp6D1;});}default: _LL616: _LL617:
({void*_tmp6D3=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6D4="found typedecltype in rsubs";_tag_dyneither(_tmp6D4,sizeof(char),28);}),_tag_dyneither(_tmp6D3,sizeof(void*),0));});}_LL5DD:;}
# 3492
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3495
if(ts == 0)
return 0;{
void*_tmp703=(void*)ts->hd;
struct Cyc_List_List*_tmp704=ts->tl;
void*_tmp705=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp703);
struct Cyc_List_List*_tmp706=Cyc_Tcutil_substs(rgn,inst,_tmp704);
if(_tmp703 == _tmp705  && _tmp704 == _tmp706)
return ts;
return({struct Cyc_List_List*_tmp707=_cycalloc(sizeof(*_tmp707));_tmp707->hd=_tmp705;_tmp707->tl=_tmp706;_tmp707;});};}
# 3506
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3513
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp708=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709->f1=tv;_tmp709->f2=Cyc_Absyn_new_evar(_tmp708,({struct Cyc_Core_Opt*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->v=s;_tmp70A;}));_tmp709;});}
# 3518
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3520
struct _tuple17*_tmp70B=env;struct Cyc_List_List*_tmp710;struct _RegionHandle*_tmp70F;_LL629: _tmp710=_tmp70B->f1;_tmp70F=_tmp70B->f2;_LL62A:;{
struct Cyc_Core_Opt*_tmp70C=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp70D=_region_malloc(_tmp70F,sizeof(*_tmp70D));_tmp70D->f1=tv;_tmp70D->f2=Cyc_Absyn_new_evar(_tmp70C,({struct Cyc_Core_Opt*_tmp70E=_cycalloc(sizeof(*_tmp70E));_tmp70E->v=_tmp710;_tmp70E;}));_tmp70D;});};}
# 3530
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3534
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp715;_tmp715.tag=0;_tmp715.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmp714;_tmp714.tag=0;_tmp714.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));({struct Cyc_String_pa_PrintArg_struct _tmp713;_tmp713.tag=0;_tmp713.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp711[3]={& _tmp713,& _tmp714,& _tmp715};Cyc_Tcutil_terr(loc,({const char*_tmp712="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp712,sizeof(char),59);}),_tag_dyneither(_tmp711,sizeof(void*),3));});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp716=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp717="same type variable has different identity!";_tag_dyneither(_tmp717,sizeof(char),43);}),_tag_dyneither(_tmp716,sizeof(void*),0));});}
return tvs;}}}
# 3548
tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718->hd=tv;_tmp718->tl=tvs;_tmp718;});}
# 3554
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3556
if(tv->identity == - 1)
({void*_tmp719=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp71A="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp71A,sizeof(char),39);}),_tag_dyneither(_tmp719,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3560
struct Cyc_Absyn_Tvar*_tmp71B=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp71B->identity == - 1)
({void*_tmp71C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp71D="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp71D,sizeof(char),41);}),_tag_dyneither(_tmp71C,sizeof(void*),0));});
if(_tmp71B->identity == tv->identity)
return tvs;}}
# 3567
return({struct Cyc_List_List*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->hd=tv;_tmp71E->tl=tvs;_tmp71E;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3573
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3578
if(tv->identity == - 1)
({void*_tmp71F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp720="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp720,sizeof(char),44);}),_tag_dyneither(_tmp71F,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3582
struct _tuple28*_tmp721=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp722=_tmp721;struct Cyc_Absyn_Tvar*_tmp726;int*_tmp725;_LL62C: _tmp726=_tmp722->f1;_tmp725=(int*)& _tmp722->f2;_LL62D:;
if(_tmp726->identity == - 1)
({void*_tmp723=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp724="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp724,sizeof(char),46);}),_tag_dyneither(_tmp723,sizeof(void*),0));});
if(_tmp726->identity == tv->identity){
*_tmp725=*_tmp725  || b;
return tvs;}}}
# 3590
return({struct Cyc_List_List*_tmp727=_region_malloc(r,sizeof(*_tmp727));_tmp727->hd=({struct _tuple28*_tmp728=_region_malloc(r,sizeof(*_tmp728));_tmp728->f1=tv;_tmp728->f2=b;_tmp728;});_tmp727->tl=tvs;_tmp727;});}
# 3594
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3596
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp72B;_tmp72B.tag=0;_tmp72B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));({void*_tmp729[1]={& _tmp72B};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp72A="bound tvar id for %s is NULL";_tag_dyneither(_tmp72A,sizeof(char),29);}),_tag_dyneither(_tmp729,sizeof(void*),1));});});
return({struct Cyc_List_List*_tmp72C=_cycalloc(sizeof(*_tmp72C));_tmp72C->hd=tv;_tmp72C->tl=tvs;_tmp72C;});}struct _tuple29{void*f1;int f2;};
# 3605
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3608
void*_tmp72D=Cyc_Tcutil_compress(e);void*_tmp72E=_tmp72D;int _tmp738;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp72E)->tag == 1){_LL62F: _tmp738=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp72E)->f3;_LL630:
# 3610
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp72F=(struct _tuple29*)es2->hd;struct _tuple29*_tmp730=_tmp72F;void*_tmp735;int*_tmp734;_LL634: _tmp735=_tmp730->f1;_tmp734=(int*)& _tmp730->f2;_LL635:;{
void*_tmp731=Cyc_Tcutil_compress(_tmp735);void*_tmp732=_tmp731;int _tmp733;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp732)->tag == 1){_LL637: _tmp733=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp732)->f3;_LL638:
# 3614
 if(_tmp738 == _tmp733){
if(b != *_tmp734)*_tmp734=1;
return es;}
# 3618
goto _LL636;}else{_LL639: _LL63A:
 goto _LL636;}_LL636:;};}}
# 3622
return({struct Cyc_List_List*_tmp736=_region_malloc(r,sizeof(*_tmp736));_tmp736->hd=({struct _tuple29*_tmp737=_region_malloc(r,sizeof(*_tmp737));_tmp737->f1=e;_tmp737->f2=b;_tmp737;});_tmp736->tl=es;_tmp736;});}else{_LL631: _LL632:
 return es;}_LL62E:;}
# 3627
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3630
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3639
if(!present)r=({struct Cyc_List_List*_tmp739=_region_malloc(rgn,sizeof(*_tmp739));_tmp739->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp739->tl=r;_tmp739;});}
# 3641
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3646
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3650
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp73A=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp73B=_tmp73A;struct Cyc_Absyn_Tvar*_tmp73E;int _tmp73D;_LL63C: _tmp73E=_tmp73B.f1;_tmp73D=_tmp73B.f2;_LL63D:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp73E->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3660
if(!present)res=({struct Cyc_List_List*_tmp73C=_region_malloc(r,sizeof(*_tmp73C));_tmp73C->hd=(struct _tuple28*)tvs->hd;_tmp73C->tl=res;_tmp73C;});};}
# 3662
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3666
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3668
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp741;_tmp741.tag=0;_tmp741.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp73F[1]={& _tmp741};Cyc_Tcutil_terr(loc,({const char*_tmp740="bitfield %s does not have constant width";_tag_dyneither(_tmp740,sizeof(char),41);}),_tag_dyneither(_tmp73F,sizeof(void*),1));});});else{
# 3673
struct _tuple14 _tmp742=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp743=_tmp742;unsigned int _tmp747;int _tmp746;_LL63F: _tmp747=_tmp743.f1;_tmp746=_tmp743.f2;_LL640:;
if(!_tmp746)
({void*_tmp744=0;Cyc_Tcutil_terr(loc,({const char*_tmp745="bitfield width cannot use sizeof or offsetof";_tag_dyneither(_tmp745,sizeof(char),45);}),_tag_dyneither(_tmp744,sizeof(void*),0));});
w=_tmp747;}{
# 3678
void*_tmp748=Cyc_Tcutil_compress(field_typ);void*_tmp749=_tmp748;enum Cyc_Absyn_Size_of _tmp757;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp749)->tag == 6){_LL642: _tmp757=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp749)->f2;_LL643:
# 3681
{enum Cyc_Absyn_Size_of _tmp74A=_tmp757;switch(_tmp74A){case Cyc_Absyn_Char_sz: _LL647: _LL648:
 if(w > 8)({void*_tmp74B=0;Cyc_Tcutil_terr(loc,({const char*_tmp74C="bitfield larger than type";_tag_dyneither(_tmp74C,sizeof(char),26);}),_tag_dyneither(_tmp74B,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Short_sz: _LL649: _LL64A:
 if(w > 16)({void*_tmp74D=0;Cyc_Tcutil_terr(loc,({const char*_tmp74E="bitfield larger than type";_tag_dyneither(_tmp74E,sizeof(char),26);}),_tag_dyneither(_tmp74D,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Long_sz: _LL64B: _LL64C:
 goto _LL64E;case Cyc_Absyn_Int_sz: _LL64D: _LL64E:
# 3686
 if(w > 32)({void*_tmp74F=0;Cyc_Tcutil_terr(loc,({const char*_tmp750="bitfield larger than type";_tag_dyneither(_tmp750,sizeof(char),26);}),_tag_dyneither(_tmp74F,sizeof(void*),0));});goto _LL646;default: _LL64F: _LL650:
# 3688
 if(w > 64)({void*_tmp751=0;Cyc_Tcutil_terr(loc,({const char*_tmp752="bitfield larger than type";_tag_dyneither(_tmp752,sizeof(char),26);}),_tag_dyneither(_tmp751,sizeof(void*),0));});goto _LL646;}_LL646:;}
# 3690
goto _LL641;}else{_LL644: _LL645:
# 3692
({struct Cyc_String_pa_PrintArg_struct _tmp756;_tmp756.tag=0;_tmp756.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));({struct Cyc_String_pa_PrintArg_struct _tmp755;_tmp755.tag=0;_tmp755.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp753[2]={& _tmp755,& _tmp756};Cyc_Tcutil_terr(loc,({const char*_tmp754="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp754,sizeof(char),52);}),_tag_dyneither(_tmp753,sizeof(void*),2));});});});
goto _LL641;}_LL641:;};}}
# 3699
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp758=(void*)atts->hd;void*_tmp759=_tmp758;switch(*((int*)_tmp759)){case 7: _LL652: _LL653:
 continue;case 6: _LL654: _LL655:
 continue;default: _LL656: _LL657:
({struct Cyc_String_pa_PrintArg_struct _tmp75D;_tmp75D.tag=0;_tmp75D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp75C;_tmp75C.tag=0;_tmp75C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp75A[2]={& _tmp75C,& _tmp75D};Cyc_Tcutil_terr(loc,({const char*_tmp75B="bad attribute %s on member %s";_tag_dyneither(_tmp75B,sizeof(char),30);}),_tag_dyneither(_tmp75A,sizeof(void*),2));});});});}_LL651:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3723
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3727
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp75E=t;struct Cyc_Absyn_Typedefdecl*_tmp762;void*_tmp761;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75E)->tag == 17){_LL659: _tmp762=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75E)->f3;_tmp761=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp75E)->f4;_LL65A:
# 3730
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp762))->tq).real_const  || (_tmp762->tq).print_const){
if(declared_const)({void*_tmp75F=0;Cyc_Tcutil_warn(loc,({const char*_tmp760="extra const";_tag_dyneither(_tmp760,sizeof(char),12);}),_tag_dyneither(_tmp75F,sizeof(void*),0));});
return 1;}
# 3735
if((unsigned int)_tmp761)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp761);else{
return declared_const;}}else{_LL65B: _LL65C:
 return declared_const;}_LL658:;}
# 3742
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp763=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp764=_tmp763;void*_tmp768;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp764)->tag == 5){_LL65E: _tmp768=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp764)->f1).ptr_atts).rgn;_LL65F:
# 3747
{void*_tmp765=Cyc_Tcutil_compress(_tmp768);void*_tmp766=_tmp765;struct Cyc_Absyn_Tvar*_tmp767;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp766)->tag == 2){_LL663: _tmp767=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp766)->f1;_LL664:
# 3749
 return Cyc_Absyn_tvar_cmp(tvar,_tmp767)== 0;}else{_LL665: _LL666:
 goto _LL662;}_LL662:;}
# 3752
goto _LL65D;}else{_LL660: _LL661:
 goto _LL65D;}_LL65D:;}}else{
# 3758
return 1;}
return 0;}
# 3762
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3765
void*_tmp769=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp76A=_tmp769;switch(*((int*)_tmp76A)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp76A)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp76A)->f2)->aliasqual == Cyc_Absyn_Top){_LL668: _LL669:
 goto _LL66B;}else{goto _LL66C;}}else{goto _LL66C;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp76A)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp76A)->f1)->aliasqual == Cyc_Absyn_Top){_LL66A: _LL66B:
# 3774
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3777
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3783
return& Cyc_Tcutil_trk;}else{goto _LL66C;}}else{goto _LL66C;}default: _LL66C: _LL66D:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL667:;}
# 3789
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3793
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3796
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp76E;_tmp76E.tag=0;_tmp76E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp76D;_tmp76D.tag=0;_tmp76D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp76B[2]={& _tmp76D,& _tmp76E};Cyc_Tcutil_terr(loc,({const char*_tmp76C="%s clause has type %s instead of integral type";_tag_dyneither(_tmp76C,sizeof(char),47);}),_tag_dyneither(_tmp76B,sizeof(void*),2));});});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp772;_tmp772.tag=0;_tmp772.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));({struct Cyc_String_pa_PrintArg_struct _tmp771;_tmp771.tag=0;_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp76F[2]={& _tmp771,& _tmp772};Cyc_Tcutil_terr(clause->loc,({const char*_tmp770="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp770,sizeof(char),36);}),_tag_dyneither(_tmp76F,sizeof(void*),2));});});});}
# 3808
return({struct _tuple30 _tmp773;_tmp773.f1=cvtenv;_tmp773.f2=relns;_tmp773;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3839 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3847
{void*_tmp774=Cyc_Tcutil_compress(t);void*_tmp775=_tmp774;struct Cyc_List_List*_tmp940;void*_tmp93F;void*_tmp93E;void*_tmp93D;void*_tmp93C;void*_tmp93B;struct _tuple2*_tmp93A;struct Cyc_List_List**_tmp939;struct Cyc_Absyn_Typedefdecl**_tmp938;void**_tmp937;union Cyc_Absyn_AggrInfoU*_tmp936;struct Cyc_List_List**_tmp935;enum Cyc_Absyn_AggrKind _tmp934;struct Cyc_List_List*_tmp933;struct Cyc_List_List*_tmp932;struct Cyc_List_List**_tmp931;void**_tmp930;struct Cyc_Absyn_Tqual*_tmp92F;void*_tmp92E;struct Cyc_List_List*_tmp92D;int _tmp92C;struct Cyc_Absyn_VarargInfo*_tmp92B;struct Cyc_List_List*_tmp92A;struct Cyc_List_List*_tmp929;struct Cyc_Absyn_Exp*_tmp928;struct Cyc_List_List**_tmp927;struct Cyc_Absyn_Exp*_tmp926;struct Cyc_List_List**_tmp925;void*_tmp924;struct Cyc_Absyn_Tqual*_tmp923;struct Cyc_Absyn_Exp**_tmp922;union Cyc_Absyn_Constraint*_tmp921;unsigned int _tmp920;struct Cyc_Absyn_Exp*_tmp91F;struct Cyc_Absyn_Exp*_tmp91E;void*_tmp91D;void*_tmp91C;struct Cyc_Absyn_Tqual*_tmp91B;void*_tmp91A;union Cyc_Absyn_Constraint*_tmp919;union Cyc_Absyn_Constraint*_tmp918;union Cyc_Absyn_Constraint*_tmp917;union Cyc_Absyn_DatatypeFieldInfoU*_tmp916;struct Cyc_List_List*_tmp915;union Cyc_Absyn_DatatypeInfoU*_tmp914;struct Cyc_List_List**_tmp913;struct _tuple2*_tmp912;struct Cyc_Absyn_Enumdecl**_tmp911;struct Cyc_List_List*_tmp910;void*_tmp90F;void***_tmp90E;struct Cyc_Absyn_Tvar*_tmp90D;struct Cyc_Core_Opt**_tmp90C;void**_tmp90B;switch(*((int*)_tmp775)){case 0: _LL66F: _LL670:
 goto _LL66E;case 1: _LL671: _tmp90C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp775)->f1;_tmp90B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp775)->f2;_LL672:
# 3851
 if(*_tmp90C == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp90C))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp90C))->v,expected_kind))
*_tmp90C=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3856
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp90B=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3859
*_tmp90B=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp776=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp779=_cycalloc(sizeof(*_tmp779));_tmp779[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp77A;_tmp77A.tag=2;_tmp77A.f1=0;_tmp77A.f2=expected_kind;_tmp77A;});_tmp779;}));
*_tmp90B=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp778;_tmp778.tag=2;_tmp778.f1=_tmp776;_tmp778;});_tmp777;});
_tmp90D=_tmp776;goto _LL674;}else{
# 3865
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3867
goto _LL66E;case 2: _LL673: _tmp90D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp775)->f1;_LL674:
# 3869
{void*_tmp77B=Cyc_Absyn_compress_kb(_tmp90D->kind);void*_tmp77C=_tmp77B;struct Cyc_Core_Opt**_tmp780;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp77C)->tag == 1){_LL6AA: _tmp780=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp77C)->f1;_LL6AB:
# 3871
*_tmp780=({struct Cyc_Core_Opt*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp77F;_tmp77F.tag=2;_tmp77F.f1=0;_tmp77F.f2=expected_kind;_tmp77F;});_tmp77E;});_tmp77D;});goto _LL6A9;}else{_LL6AC: _LL6AD:
 goto _LL6A9;}_LL6A9:;}
# 3876
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp90D);
# 3879
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp90D,put_in_effect);
# 3881
{void*_tmp781=Cyc_Absyn_compress_kb(_tmp90D->kind);void*_tmp782=_tmp781;struct Cyc_Core_Opt**_tmp787;struct Cyc_Absyn_Kind*_tmp786;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp782)->tag == 2){_LL6AF: _tmp787=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp782)->f1;_tmp786=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp782)->f2;_LL6B0:
# 3883
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp786))
*_tmp787=({struct Cyc_Core_Opt*_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp785;_tmp785.tag=2;_tmp785.f1=0;_tmp785.f2=expected_kind;_tmp785;});_tmp784;});_tmp783;});
goto _LL6AE;}else{_LL6B1: _LL6B2:
 goto _LL6AE;}_LL6AE:;}
# 3888
goto _LL66E;case 26: _LL675: _tmp90F=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp775)->f1)->r;_tmp90E=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp775)->f2;_LL676: {
# 3894
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp788=_tmp90F;struct Cyc_Absyn_Datatypedecl*_tmp78B;struct Cyc_Absyn_Enumdecl*_tmp78A;struct Cyc_Absyn_Aggrdecl*_tmp789;switch(*((int*)_tmp788)){case 0: _LL6B4: _tmp789=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp788)->f1;_LL6B5:
# 3897
 if(te->in_extern_c_include)
_tmp789->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp789);goto _LL6B3;case 1: _LL6B6: _tmp78A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp788)->f1;_LL6B7:
# 3901
 if(te->in_extern_c_include)
_tmp78A->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp78A);goto _LL6B3;default: _LL6B8: _tmp78B=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp788)->f1;_LL6B9:
# 3905
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp78B);goto _LL6B3;}_LL6B3:;}
# 3907
*_tmp90E=({void**_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C[0]=new_t;_tmp78C;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL677: _tmp910=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp775)->f1;_LL678: {
# 3913
struct _RegionHandle _tmp78D=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp78D;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp910 != 0;_tmp910=_tmp910->tl){
struct Cyc_Absyn_Enumfield*_tmp78E=(struct Cyc_Absyn_Enumfield*)_tmp910->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp78E->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0;_tmp791.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp78E->name).f2);({void*_tmp78F[1]={& _tmp791};Cyc_Tcutil_terr(_tmp78E->loc,({const char*_tmp790="duplicate enum field name %s";_tag_dyneither(_tmp790,sizeof(char),29);}),_tag_dyneither(_tmp78F,sizeof(void*),1));});});else{
# 3921
prev_fields=({struct Cyc_List_List*_tmp792=_region_malloc(uprev_rgn,sizeof(*_tmp792));_tmp792->hd=(*_tmp78E->name).f2;_tmp792->tl=prev_fields;_tmp792;});}
# 3923
if(_tmp78E->tag == 0)
_tmp78E->tag=Cyc_Absyn_uint_exp(tag_count,_tmp78E->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp78E->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp795;_tmp795.tag=0;_tmp795.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp78E->name).f2);({void*_tmp793[1]={& _tmp795};Cyc_Tcutil_terr(loc,({const char*_tmp794="enum field %s: expression is not constant";_tag_dyneither(_tmp794,sizeof(char),42);}),_tag_dyneither(_tmp793,sizeof(void*),1));});});}{
# 3928
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp78E->tag))).f1;
tag_count=t1 + 1;
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple2*k,struct _tuple29*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp78E->name,({struct _tuple29*_tmp796=_cycalloc(sizeof(*_tmp796));_tmp796->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp797=_cycalloc(sizeof(*_tmp797));_tmp797[0]=({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp798;_tmp798.tag=4;_tmp798.f1=t;_tmp798.f2=_tmp78E;_tmp798;});_tmp797;});_tmp796->f2=1;_tmp796;}));};}}
# 3934
_npop_handler(0);goto _LL66E;
# 3913
;_pop_region(uprev_rgn);}case 13: _LL679: _tmp912=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp775)->f1;_tmp911=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp775)->f2;_LL67A:
# 3936
 if(*_tmp911 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp911))->fields == 0){
struct _handler_cons _tmp799;_push_handler(& _tmp799);{int _tmp79B=0;if(setjmp(_tmp799.handler))_tmp79B=1;if(!_tmp79B){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp912);
*_tmp911=*ed;}
# 3938
;_pop_handler();}else{void*_tmp79A=(void*)_exn_thrown;void*_tmp79C=_tmp79A;void*_tmp79F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp79C)->tag == Cyc_Dict_Absent){_LL6BB: _LL6BC: {
# 3942
struct Cyc_Absyn_Enumdecl*_tmp79D=({struct Cyc_Absyn_Enumdecl*_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E->sc=Cyc_Absyn_Extern;_tmp79E->name=_tmp912;_tmp79E->fields=0;_tmp79E;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp79D);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp912);
*_tmp911=*ed;
goto _LL6BA;};}}else{_LL6BD: _tmp79F=_tmp79C;_LL6BE:(int)_rethrow(_tmp79F);}_LL6BA:;}};}{
# 3948
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp911);
goto _LL66E;};case 3: _LL67B: _tmp914=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp775)->f1).datatype_info;_tmp913=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp775)->f1).targs;_LL67C: {
# 3951
struct Cyc_List_List*_tmp7A0=*_tmp913;
{union Cyc_Absyn_DatatypeInfoU _tmp7A1=*_tmp914;union Cyc_Absyn_DatatypeInfoU _tmp7A2=_tmp7A1;struct Cyc_Absyn_Datatypedecl*_tmp7BA;struct _tuple2*_tmp7B9;int _tmp7B8;if((_tmp7A2.UnknownDatatype).tag == 1){_LL6C0: _tmp7B9=((_tmp7A2.UnknownDatatype).val).name;_tmp7B8=((_tmp7A2.UnknownDatatype).val).is_extensible;_LL6C1: {
# 3954
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp7A3;_push_handler(& _tmp7A3);{int _tmp7A5=0;if(setjmp(_tmp7A3.handler))_tmp7A5=1;if(!_tmp7A5){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7B9);;_pop_handler();}else{void*_tmp7A4=(void*)_exn_thrown;void*_tmp7A6=_tmp7A4;void*_tmp7AC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7A6)->tag == Cyc_Dict_Absent){_LL6C5: _LL6C6: {
# 3958
struct Cyc_Absyn_Datatypedecl*_tmp7A7=({struct Cyc_Absyn_Datatypedecl*_tmp7AB=_cycalloc(sizeof(*_tmp7AB));_tmp7AB->sc=Cyc_Absyn_Extern;_tmp7AB->name=_tmp7B9;_tmp7AB->tvs=0;_tmp7AB->fields=0;_tmp7AB->is_extensible=_tmp7B8;_tmp7AB;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp7A7);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7B9);
# 3962
if(_tmp7A0 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7AA;_tmp7AA.tag=0;_tmp7AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B9));({void*_tmp7A8[1]={& _tmp7AA};Cyc_Tcutil_terr(loc,({const char*_tmp7A9="declare parameterized datatype %s before using";_tag_dyneither(_tmp7A9,sizeof(char),47);}),_tag_dyneither(_tmp7A8,sizeof(void*),1));});});
return cvtenv;}
# 3967
goto _LL6C4;}}else{_LL6C7: _tmp7AC=_tmp7A6;_LL6C8:(int)_rethrow(_tmp7AC);}_LL6C4:;}};}
# 3971
if(_tmp7B8  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7AF;_tmp7AF.tag=0;_tmp7AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B9));({void*_tmp7AD[1]={& _tmp7AF};Cyc_Tcutil_terr(loc,({const char*_tmp7AE="datatype %s was not declared @extensible";_tag_dyneither(_tmp7AE,sizeof(char),41);}),_tag_dyneither(_tmp7AD,sizeof(void*),1));});});
*_tmp914=Cyc_Absyn_KnownDatatype(tudp);
_tmp7BA=*tudp;goto _LL6C3;}}else{_LL6C2: _tmp7BA=*(_tmp7A2.KnownDatatype).val;_LL6C3: {
# 3978
struct Cyc_List_List*tvs=_tmp7BA->tvs;
for(0;_tmp7A0 != 0  && tvs != 0;(_tmp7A0=_tmp7A0->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7A0->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3984
{struct _tuple0 _tmp7B0=({struct _tuple0 _tmp7B3;_tmp7B3.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7B3.f2=t;_tmp7B3;});struct _tuple0 _tmp7B1=_tmp7B0;struct Cyc_Absyn_Tvar*_tmp7B2;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7B1.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7B1.f2)->tag == 2){_LL6CA: _tmp7B2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7B1.f2)->f1;_LL6CB:
# 3986
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7B2);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7B2,1);
continue;}else{goto _LL6CC;}}else{_LL6CC: _LL6CD:
 goto _LL6C9;}_LL6C9:;}{
# 3991
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3995
if(_tmp7A0 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B6;_tmp7B6.tag=0;_tmp7B6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7BA->name));({void*_tmp7B4[1]={& _tmp7B6};Cyc_Tcutil_terr(loc,({const char*_tmp7B5="too many type arguments for datatype %s";_tag_dyneither(_tmp7B5,sizeof(char),40);}),_tag_dyneither(_tmp7B4,sizeof(void*),1));});});
if(tvs != 0){
# 4000
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp7B7=_cycalloc(sizeof(*_tmp7B7));_tmp7B7->hd=e;_tmp7B7->tl=hidden_ts;_tmp7B7;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4007
*_tmp913=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp913,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4009
goto _LL6BF;}}_LL6BF:;}
# 4011
goto _LL66E;}case 4: _LL67D: _tmp916=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp775)->f1).field_info;_tmp915=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp775)->f1).targs;_LL67E:
# 4014
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7BB=*_tmp916;union Cyc_Absyn_DatatypeFieldInfoU _tmp7BC=_tmp7BB;struct Cyc_Absyn_Datatypedecl*_tmp7EA;struct Cyc_Absyn_Datatypefield*_tmp7E9;struct _tuple2*_tmp7E8;struct _tuple2*_tmp7E7;int _tmp7E6;if((_tmp7BC.UnknownDatatypefield).tag == 1){_LL6CF: _tmp7E8=((_tmp7BC.UnknownDatatypefield).val).datatype_name;_tmp7E7=((_tmp7BC.UnknownDatatypefield).val).field_name;_tmp7E6=((_tmp7BC.UnknownDatatypefield).val).is_extensible;_LL6D0: {
# 4016
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp7BD;_push_handler(& _tmp7BD);{int _tmp7BF=0;if(setjmp(_tmp7BD.handler))_tmp7BF=1;if(!_tmp7BF){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7E8);;_pop_handler();}else{void*_tmp7BE=(void*)_exn_thrown;void*_tmp7C0=_tmp7BE;void*_tmp7C4;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7C0)->tag == Cyc_Dict_Absent){_LL6D4: _LL6D5:
# 4020
({struct Cyc_String_pa_PrintArg_struct _tmp7C3;_tmp7C3.tag=0;_tmp7C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7E8));({void*_tmp7C1[1]={& _tmp7C3};Cyc_Tcutil_terr(loc,({const char*_tmp7C2="unbound datatype %s";_tag_dyneither(_tmp7C2,sizeof(char),20);}),_tag_dyneither(_tmp7C1,sizeof(void*),1));});});
return cvtenv;}else{_LL6D6: _tmp7C4=_tmp7C0;_LL6D7:(int)_rethrow(_tmp7C4);}_LL6D3:;}};}
# 4023
{struct _handler_cons _tmp7C5;_push_handler(& _tmp7C5);{int _tmp7C7=0;if(setjmp(_tmp7C5.handler))_tmp7C7=1;if(!_tmp7C7){{void*_tmp7C8=Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp7E7,0);void*_tmp7C9=_tmp7C8;struct Cyc_Absyn_Datatypedecl*_tmp7D3;struct Cyc_Absyn_Datatypefield*_tmp7D2;if(((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7C9)->tag == 2){_LL6D9: _tmp7D3=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7C9)->f1;_tmp7D2=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7C9)->f2;_LL6DA:
# 4025
 tuf=_tmp7D2;
tud=_tmp7D3;
if(_tmp7E6  && !tud->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7CC;_tmp7CC.tag=0;_tmp7CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7E8));({void*_tmp7CA[1]={& _tmp7CC};Cyc_Tcutil_terr(loc,({const char*_tmp7CB="datatype %s was not declared @extensible";_tag_dyneither(_tmp7CB,sizeof(char),41);}),_tag_dyneither(_tmp7CA,sizeof(void*),1));});});
goto _LL6D8;}else{_LL6DB: _LL6DC:
({struct Cyc_String_pa_PrintArg_struct _tmp7D0;_tmp7D0.tag=0;_tmp7D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7E8));({struct Cyc_String_pa_PrintArg_struct _tmp7CF;_tmp7CF.tag=0;_tmp7CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7E7));({void*_tmp7CD[2]={& _tmp7CF,& _tmp7D0};Cyc_Tcutil_terr(loc,({const char*_tmp7CE="unbound field %s in type datatype %s";_tag_dyneither(_tmp7CE,sizeof(char),37);}),_tag_dyneither(_tmp7CD,sizeof(void*),2));});});});{
struct Cyc_Tcutil_CVTEnv _tmp7D1=cvtenv;_npop_handler(0);return _tmp7D1;};}_LL6D8:;}
# 4023
;_pop_handler();}else{void*_tmp7C6=(void*)_exn_thrown;void*_tmp7D4=_tmp7C6;void*_tmp7D9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7D4)->tag == Cyc_Dict_Absent){_LL6DE: _LL6DF:
# 4036
({struct Cyc_String_pa_PrintArg_struct _tmp7D8;_tmp7D8.tag=0;_tmp7D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7E8));({struct Cyc_String_pa_PrintArg_struct _tmp7D7;_tmp7D7.tag=0;_tmp7D7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7E7));({void*_tmp7D5[2]={& _tmp7D7,& _tmp7D8};Cyc_Tcutil_terr(loc,({const char*_tmp7D6="unbound field %s in type datatype %s";_tag_dyneither(_tmp7D6,sizeof(char),37);}),_tag_dyneither(_tmp7D5,sizeof(void*),2));});});});
return cvtenv;}else{_LL6E0: _tmp7D9=_tmp7D4;_LL6E1:(int)_rethrow(_tmp7D9);}_LL6DD:;}};}
# 4040
*_tmp916=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp7EA=tud;_tmp7E9=tuf;goto _LL6D2;}}else{_LL6D1: _tmp7EA=((_tmp7BC.KnownDatatypefield).val).f1;_tmp7E9=((_tmp7BC.KnownDatatypefield).val).f2;_LL6D2: {
# 4044
struct Cyc_List_List*tvs=_tmp7EA->tvs;
for(0;_tmp915 != 0  && tvs != 0;(_tmp915=_tmp915->tl,tvs=tvs->tl)){
void*t=(void*)_tmp915->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4050
{struct _tuple0 _tmp7DA=({struct _tuple0 _tmp7DD;_tmp7DD.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7DD.f2=t;_tmp7DD;});struct _tuple0 _tmp7DB=_tmp7DA;struct Cyc_Absyn_Tvar*_tmp7DC;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7DB.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7DB.f2)->tag == 2){_LL6E3: _tmp7DC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7DB.f2)->f1;_LL6E4:
# 4052
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7DC);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7DC,1);
continue;}else{goto _LL6E5;}}else{_LL6E5: _LL6E6:
 goto _LL6E2;}_LL6E2:;}{
# 4057
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4061
if(_tmp915 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7E1;_tmp7E1.tag=0;_tmp7E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7E9->name));({struct Cyc_String_pa_PrintArg_struct _tmp7E0;_tmp7E0.tag=0;_tmp7E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7EA->name));({void*_tmp7DE[2]={& _tmp7E0,& _tmp7E1};Cyc_Tcutil_terr(loc,({const char*_tmp7DF="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp7DF,sizeof(char),43);}),_tag_dyneither(_tmp7DE,sizeof(void*),2));});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7E5;_tmp7E5.tag=0;_tmp7E5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7E9->name));({struct Cyc_String_pa_PrintArg_struct _tmp7E4;_tmp7E4.tag=0;_tmp7E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7EA->name));({void*_tmp7E2[2]={& _tmp7E4,& _tmp7E5};Cyc_Tcutil_terr(loc,({const char*_tmp7E3="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp7E3,sizeof(char),42);}),_tag_dyneither(_tmp7E2,sizeof(void*),2));});});});
goto _LL6CE;}}_LL6CE:;}
# 4069
goto _LL66E;case 5: _LL67F: _tmp91C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp775)->f1).elt_typ;_tmp91B=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp775)->f1).elt_tq;_tmp91A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp775)->f1).ptr_atts).rgn;_tmp919=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp775)->f1).ptr_atts).nullable;_tmp918=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp775)->f1).ptr_atts).bounds;_tmp917=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp775)->f1).ptr_atts).zero_term;_LL680: {
# 4072
int is_zero_terminated;
# 4074
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp91C,1,1);
_tmp91B->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp91B->print_const,_tmp91C);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp7EB=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp7EC=_tmp7EB;switch(_tmp7EC){case Cyc_Absyn_Aliasable: _LL6E8: _LL6E9:
 k=& Cyc_Tcutil_rk;goto _LL6E7;case Cyc_Absyn_Unique: _LL6EA: _LL6EB:
 k=& Cyc_Tcutil_urk;goto _LL6E7;default: _LL6EC: _LL6ED:
 k=& Cyc_Tcutil_trk;goto _LL6E7;}_LL6E7:;}
# 4082
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp91A,1,1);
{union Cyc_Absyn_Constraint*_tmp7ED=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp917);union Cyc_Absyn_Constraint*_tmp7EE=_tmp7ED;int _tmp7F4;switch((((union Cyc_Absyn_Constraint*)_tmp7EE)->Eq_constr).tag){case 3: _LL6EF: _LL6F0:
# 4087
{void*_tmp7EF=Cyc_Tcutil_compress(_tmp91C);void*_tmp7F0=_tmp7EF;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F0)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F0)->f2 == Cyc_Absyn_Char_sz){_LL6F6: _LL6F7:
# 4089
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp917,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL6F5;}else{goto _LL6F8;}}else{_LL6F8: _LL6F9:
# 4093
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp917,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL6F5;}_LL6F5:;}
# 4097
goto _LL6EE;case 1: _LL6F1: _tmp7F4=(int)(_tmp7EE->Eq_constr).val;if(_tmp7F4){_LL6F2:
# 4100
 if(!Cyc_Tcutil_admits_zero(_tmp91C))
({struct Cyc_String_pa_PrintArg_struct _tmp7F3;_tmp7F3.tag=0;_tmp7F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp91C));({void*_tmp7F1[1]={& _tmp7F3};Cyc_Tcutil_terr(loc,({const char*_tmp7F2="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp7F2,sizeof(char),53);}),_tag_dyneither(_tmp7F1,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL6EE;}else{goto _LL6F3;}default: _LL6F3: _LL6F4:
# 4106
 is_zero_terminated=0;
goto _LL6EE;}_LL6EE:;}
# 4110
{void*_tmp7F5=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp918);void*_tmp7F6=_tmp7F5;struct Cyc_Absyn_Exp*_tmp801;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp7F6)->tag == 0){_LL6FB: _LL6FC:
 goto _LL6FA;}else{_LL6FD: _tmp801=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7F6)->f1;_LL6FE: {
# 4113
struct _RegionHandle _tmp7F7=_new_region("temp");struct _RegionHandle*temp=& _tmp7F7;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp7F8=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp7F8,0,_tmp801);}
# 4117
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp801,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp801))
({void*_tmp7F9=0;Cyc_Tcutil_terr(loc,({const char*_tmp7FA="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp7FA,sizeof(char),49);}),_tag_dyneither(_tmp7F9,sizeof(void*),0));});{
struct _tuple14 _tmp7FB=Cyc_Evexp_eval_const_uint_exp(_tmp801);struct _tuple14 _tmp7FC=_tmp7FB;unsigned int _tmp800;int _tmp7FF;_LL700: _tmp800=_tmp7FC.f1;_tmp7FF=_tmp7FC.f2;_LL701:;
if(is_zero_terminated  && (!_tmp7FF  || _tmp800 < 1))
({void*_tmp7FD=0;Cyc_Tcutil_terr(loc,({const char*_tmp7FE="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp7FE,sizeof(char),55);}),_tag_dyneither(_tmp7FD,sizeof(void*),0));});
_npop_handler(0);goto _LL6FA;};
# 4113
;_pop_region(temp);}}_LL6FA:;}
# 4125
goto _LL66E;};}case 19: _LL681: _tmp91D=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp775)->f1;_LL682:
# 4127
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp91D,1,1);goto _LL66E;case 28: _LL683: _LL684:
 goto _LL66E;case 18: _LL685: _tmp91E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp775)->f1;_LL686: {
# 4133
struct _RegionHandle _tmp802=_new_region("temp");struct _RegionHandle*temp=& _tmp802;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp803=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp803,0,_tmp91E);}
# 4137
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp91E))
({void*_tmp804=0;Cyc_Tcutil_terr(loc,({const char*_tmp805="valueof_t requires an int expression";_tag_dyneither(_tmp805,sizeof(char),37);}),_tag_dyneither(_tmp804,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91E,te,cvtenv);
_npop_handler(0);goto _LL66E;
# 4133
;_pop_region(temp);}case 27: _LL687: _tmp91F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp775)->f1;_LL688: {
# 4145
struct _RegionHandle _tmp806=_new_region("temp");struct _RegionHandle*temp=& _tmp806;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp807=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp807,0,_tmp91F);}
# 4149
_npop_handler(0);goto _LL66E;
# 4145
;_pop_region(temp);}case 6: _LL689: _LL68A:
# 4150
 goto _LL68C;case 7: _LL68B: _LL68C:
 goto _LL66E;case 8: _LL68D: _tmp924=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp775)->f1).elt_type;_tmp923=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp775)->f1).tq;_tmp922=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp775)->f1).num_elts;_tmp921=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp775)->f1).zero_term;_tmp920=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp775)->f1).zt_loc;_LL68E: {
# 4155
struct Cyc_Absyn_Exp*_tmp808=*_tmp922;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp924,1,allow_abs_aggr);
_tmp923->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp923->print_const,_tmp924);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp809=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp921);union Cyc_Absyn_Constraint*_tmp80A=_tmp809;int _tmp80E;switch((((union Cyc_Absyn_Constraint*)_tmp80A)->Eq_constr).tag){case 3: _LL703: _LL704:
# 4162
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp921,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4177 "tcutil.cyc"
goto _LL702;case 1: _LL705: _tmp80E=(int)(_tmp80A->Eq_constr).val;if(_tmp80E){_LL706:
# 4180
 if(!Cyc_Tcutil_admits_zero(_tmp924))
({struct Cyc_String_pa_PrintArg_struct _tmp80D;_tmp80D.tag=0;_tmp80D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp924));({void*_tmp80B[1]={& _tmp80D};Cyc_Tcutil_terr(loc,({const char*_tmp80C="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp80C,sizeof(char),51);}),_tag_dyneither(_tmp80B,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL702;}else{goto _LL707;}default: _LL707: _LL708:
# 4186
 is_zero_terminated=0;
goto _LL702;}_LL702:;}
# 4191
if(_tmp808 == 0){
# 4193
if(is_zero_terminated)
# 4195
*_tmp922=(_tmp808=Cyc_Absyn_uint_exp(1,0));else{
# 4198
({void*_tmp80F=0;Cyc_Tcutil_warn(loc,({const char*_tmp810="array bound defaults to 1 here";_tag_dyneither(_tmp810,sizeof(char),31);}),_tag_dyneither(_tmp80F,sizeof(void*),0));});
*_tmp922=(_tmp808=Cyc_Absyn_uint_exp(1,0));}}{
# 4202
struct _RegionHandle _tmp811=_new_region("temp");struct _RegionHandle*temp=& _tmp811;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp812=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp812,0,_tmp808);}
# 4206
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp808))
({void*_tmp813=0;Cyc_Tcutil_terr(loc,({const char*_tmp814="array bounds expression is not an unsigned int";_tag_dyneither(_tmp814,sizeof(char),47);}),_tag_dyneither(_tmp813,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp808,te,cvtenv);{
# 4213
struct _tuple14 _tmp815=Cyc_Evexp_eval_const_uint_exp(_tmp808);struct _tuple14 _tmp816=_tmp815;unsigned int _tmp81C;int _tmp81B;_LL70A: _tmp81C=_tmp816.f1;_tmp81B=_tmp816.f2;_LL70B:;
# 4215
if((is_zero_terminated  && _tmp81B) && _tmp81C < 1)
({void*_tmp817=0;Cyc_Tcutil_warn(loc,({const char*_tmp818="zero terminated array cannot have zero elements";_tag_dyneither(_tmp818,sizeof(char),48);}),_tag_dyneither(_tmp817,sizeof(void*),0));});
# 4218
if((_tmp81B  && _tmp81C < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp819=0;Cyc_Tcutil_warn(loc,({const char*_tmp81A="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp81A,sizeof(char),75);}),_tag_dyneither(_tmp819,sizeof(void*),0));});
*_tmp922=Cyc_Absyn_uint_exp(1,0);}
# 4222
_npop_handler(0);goto _LL66E;};
# 4202
;_pop_region(temp);};};}case 9: _LL68F: _tmp931=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).tvars;_tmp930=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).effect;_tmp92F=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).ret_tqual;_tmp92E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).ret_typ;_tmp92D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).args;_tmp92C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).c_varargs;_tmp92B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).cyc_varargs;_tmp92A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).rgn_po;_tmp929=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).attributes;_tmp928=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).requires_clause;_tmp927=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).requires_relns;_tmp926=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).ensures_clause;_tmp925=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp775)->f1).ensures_relns;_LL690: {
# 4229
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp929 != 0;_tmp929=_tmp929->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp929->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp81F;_tmp81F.tag=0;_tmp81F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp929->hd));({void*_tmp81D[1]={& _tmp81F};Cyc_Tcutil_terr(loc,({const char*_tmp81E="bad function type attribute %s";_tag_dyneither(_tmp81E,sizeof(char),31);}),_tag_dyneither(_tmp81D,sizeof(void*),1));});});{
void*_tmp820=(void*)_tmp929->hd;void*_tmp821=_tmp820;enum Cyc_Absyn_Format_Type _tmp826;int _tmp825;int _tmp824;switch(*((int*)_tmp821)){case 1: _LL70D: _LL70E:
# 4242
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL70C;case 2: _LL70F: _LL710:
# 4244
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL70C;case 3: _LL711: _LL712:
# 4246
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL70C;case 19: _LL713: _tmp826=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp821)->f1;_tmp825=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp821)->f2;_tmp824=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp821)->f3;_LL714:
# 4248
 if(!seen_format){
seen_format=1;
ft=_tmp826;
fmt_desc_arg=_tmp825;
fmt_arg_start=_tmp824;}else{
# 4254
({void*_tmp822=0;Cyc_Tcutil_terr(loc,({const char*_tmp823="function can't have multiple format attributes";_tag_dyneither(_tmp823,sizeof(char),47);}),_tag_dyneither(_tmp822,sizeof(void*),0));});}
goto _LL70C;default: _LL715: _LL716:
 goto _LL70C;}_LL70C:;};}
# 4259
if(num_convs > 1)
({void*_tmp827=0;Cyc_Tcutil_terr(loc,({const char*_tmp828="function can't have multiple calling conventions";_tag_dyneither(_tmp828,sizeof(char),49);}),_tag_dyneither(_tmp827,sizeof(void*),0));});
# 4264
Cyc_Tcutil_check_unique_tvars(loc,*_tmp931);
{struct Cyc_List_List*b=*_tmp931;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmp829=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp82A=_tmp829;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp82A)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp82A)->f1)->kind == Cyc_Absyn_MemKind){_LL718: _LL719:
# 4270
({struct Cyc_String_pa_PrintArg_struct _tmp82D;_tmp82D.tag=0;_tmp82D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp82B[1]={& _tmp82D};Cyc_Tcutil_terr(loc,({const char*_tmp82C="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp82C,sizeof(char),51);}),_tag_dyneither(_tmp82B,sizeof(void*),1));});});
goto _LL717;}else{goto _LL71A;}}else{_LL71A: _LL71B:
 goto _LL717;}_LL717:;};}}{
# 4278
struct _RegionHandle _tmp82E=_new_region("newr");struct _RegionHandle*newr=& _tmp82E;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp82F=({struct Cyc_Tcutil_CVTEnv _tmp8BD;_tmp8BD.r=newr;_tmp8BD.kind_env=cvtenv.kind_env;_tmp8BD.free_vars=0;_tmp8BD.free_evars=0;_tmp8BD.generalize_evars=cvtenv.generalize_evars;_tmp8BD.fn_result=1;_tmp8BD;});
# 4284
_tmp82F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp82F,& Cyc_Tcutil_tmk,_tmp92E,1,1);
_tmp92F->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp92F->print_const,_tmp92E);
_tmp82F.fn_result=0;
{struct Cyc_List_List*a=_tmp92D;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp830=(struct _tuple10*)a->hd;
void*_tmp831=(*_tmp830).f3;
_tmp82F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp82F,& Cyc_Tcutil_tmk,_tmp831,1,1);{
int _tmp832=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp830).f2).print_const,_tmp831);
((*_tmp830).f2).real_const=_tmp832;
# 4295
if(Cyc_Tcutil_is_array(_tmp831)){
# 4298
void*_tmp833=Cyc_Absyn_new_evar(0,0);
_tmp82F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp82F,& Cyc_Tcutil_rk,_tmp833,1,1);
(*_tmp830).f3=Cyc_Tcutil_promote_array(_tmp831,_tmp833,0);}};}}
# 4305
if(_tmp92B != 0){
if(_tmp92C)({void*_tmp834=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp835="both c_vararg and cyc_vararg";_tag_dyneither(_tmp835,sizeof(char),29);}),_tag_dyneither(_tmp834,sizeof(void*),0));});{
struct Cyc_Absyn_VarargInfo _tmp836=*_tmp92B;struct Cyc_Absyn_VarargInfo _tmp837=_tmp836;void*_tmp84A;int _tmp849;_LL71D: _tmp84A=_tmp837.type;_tmp849=_tmp837.inject;_LL71E:;
_tmp82F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp82F,& Cyc_Tcutil_tmk,_tmp84A,1,1);
(_tmp92B->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp92B->tq).print_const,_tmp84A);
# 4311
if(_tmp849){
void*_tmp838=Cyc_Tcutil_compress(_tmp84A);void*_tmp839=_tmp838;void*_tmp848;union Cyc_Absyn_Constraint*_tmp847;union Cyc_Absyn_Constraint*_tmp846;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp839)->tag == 5){_LL720: _tmp848=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp839)->f1).elt_typ;_tmp847=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp839)->f1).ptr_atts).bounds;_tmp846=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp839)->f1).ptr_atts).zero_term;_LL721:
# 4314
{void*_tmp83A=Cyc_Tcutil_compress(_tmp848);void*_tmp83B=_tmp83A;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp83B)->tag == 3){_LL725: _LL726:
# 4316
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp846))
({void*_tmp83C=0;Cyc_Tcutil_terr(loc,({const char*_tmp83D="can't inject into a zeroterm pointer";_tag_dyneither(_tmp83D,sizeof(char),37);}),_tag_dyneither(_tmp83C,sizeof(void*),0));});
{void*_tmp83E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp847);void*_tmp83F=_tmp83E;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp83F)->tag == 0){_LL72A: _LL72B:
# 4320
({void*_tmp840=0;Cyc_Tcutil_terr(loc,({const char*_tmp841="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp841,sizeof(char),44);}),_tag_dyneither(_tmp840,sizeof(void*),0));});
goto _LL729;}else{_LL72C: _LL72D:
 goto _LL729;}_LL729:;}
# 4324
goto _LL724;}else{_LL727: _LL728:
({void*_tmp842=0;Cyc_Tcutil_terr(loc,({const char*_tmp843="can't inject a non-datatype type";_tag_dyneither(_tmp843,sizeof(char),33);}),_tag_dyneither(_tmp842,sizeof(void*),0));});goto _LL724;}_LL724:;}
# 4327
goto _LL71F;}else{_LL722: _LL723:
({void*_tmp844=0;Cyc_Tcutil_terr(loc,({const char*_tmp845="expecting a datatype pointer type";_tag_dyneither(_tmp845,sizeof(char),34);}),_tag_dyneither(_tmp844,sizeof(void*),0));});goto _LL71F;}_LL71F:;}};}
# 4333
if(seen_format){
int _tmp84B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp92D);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp84B) || fmt_arg_start < 0) || 
# 4337
(_tmp92B == 0  && !_tmp92C) && fmt_arg_start != 0) || 
(_tmp92B != 0  || _tmp92C) && fmt_arg_start != _tmp84B + 1)
# 4340
({void*_tmp84C=0;Cyc_Tcutil_terr(loc,({const char*_tmp84D="bad format descriptor";_tag_dyneither(_tmp84D,sizeof(char),22);}),_tag_dyneither(_tmp84C,sizeof(void*),0));});else{
# 4343
struct _tuple10 _tmp84E=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp92D,fmt_desc_arg - 1);struct _tuple10 _tmp84F=_tmp84E;void*_tmp867;_LL72F: _tmp867=_tmp84F.f3;_LL730:;
# 4345
{void*_tmp850=Cyc_Tcutil_compress(_tmp867);void*_tmp851=_tmp850;void*_tmp85F;union Cyc_Absyn_Constraint*_tmp85E;union Cyc_Absyn_Constraint*_tmp85D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp851)->tag == 5){_LL732: _tmp85F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp851)->f1).elt_typ;_tmp85E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp851)->f1).ptr_atts).bounds;_tmp85D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp851)->f1).ptr_atts).zero_term;_LL733:
# 4348
{struct _tuple0 _tmp852=({struct _tuple0 _tmp85A;_tmp85A.f1=Cyc_Tcutil_compress(_tmp85F);_tmp85A.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp85E);_tmp85A;});struct _tuple0 _tmp853=_tmp852;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp853.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp853.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp853.f2)->tag == 0){_LL737: _LL738:
# 4350
 if(_tmp92C)
({void*_tmp854=0;Cyc_Tcutil_terr(loc,({const char*_tmp855="format descriptor is not a char * type";_tag_dyneither(_tmp855,sizeof(char),39);}),_tag_dyneither(_tmp854,sizeof(void*),0));});
goto _LL736;}else{_LL739: _LL73A:
# 4354
 if(!_tmp92C)
({void*_tmp856=0;Cyc_Tcutil_terr(loc,({const char*_tmp857="format descriptor is not a char ? type";_tag_dyneither(_tmp857,sizeof(char),39);}),_tag_dyneither(_tmp856,sizeof(void*),0));});
goto _LL736;}}else{goto _LL73B;}}else{_LL73B: _LL73C:
# 4358
({void*_tmp858=0;Cyc_Tcutil_terr(loc,({const char*_tmp859="format descriptor is not a string type";_tag_dyneither(_tmp859,sizeof(char),39);}),_tag_dyneither(_tmp858,sizeof(void*),0));});
goto _LL736;}_LL736:;}
# 4361
goto _LL731;}else{_LL734: _LL735:
({void*_tmp85B=0;Cyc_Tcutil_terr(loc,({const char*_tmp85C="format descriptor is not a string type";_tag_dyneither(_tmp85C,sizeof(char),39);}),_tag_dyneither(_tmp85B,sizeof(void*),0));});goto _LL731;}_LL731:;}
# 4364
if(fmt_arg_start != 0  && !_tmp92C){
# 4368
int problem;
{struct _tuple31 _tmp860=({struct _tuple31 _tmp864;_tmp864.f1=ft;_tmp864.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp92B))->type));_tmp864;});struct _tuple31 _tmp861=_tmp860;struct Cyc_Absyn_Datatypedecl*_tmp863;struct Cyc_Absyn_Datatypedecl*_tmp862;if(_tmp861.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp861.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp861.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL73E: _tmp862=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp861.f2)->f1).datatype_info).KnownDatatype).val;_LL73F:
# 4371
 problem=Cyc_Absyn_qvar_cmp(_tmp862->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL73D;}else{goto _LL742;}}else{goto _LL742;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp861.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp861.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL740: _tmp863=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp861.f2)->f1).datatype_info).KnownDatatype).val;_LL741:
# 4373
 problem=Cyc_Absyn_qvar_cmp(_tmp863->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL73D;}else{goto _LL742;}}else{_LL742: _LL743:
# 4375
 problem=1;goto _LL73D;}}_LL73D:;}
# 4377
if(problem)
({void*_tmp865=0;Cyc_Tcutil_terr(loc,({const char*_tmp866="format attribute and vararg types don't match";_tag_dyneither(_tmp866,sizeof(char),46);}),_tag_dyneither(_tmp865,sizeof(void*),0));});}}}
# 4385
{struct Cyc_List_List*rpo=_tmp92A;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp868=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp869=_tmp868;void*_tmp86B;void*_tmp86A;_LL745: _tmp86B=_tmp869->f1;_tmp86A=_tmp869->f2;_LL746:;
_tmp82F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp82F,& Cyc_Tcutil_ek,_tmp86B,1,1);
_tmp82F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp82F,& Cyc_Tcutil_trk,_tmp86A,1,1);}}{
# 4393
struct Cyc_Tcenv_Fenv*_tmp86C=Cyc_Tcenv_bogus_fenv(newr,_tmp92E,_tmp92D);
struct Cyc_Tcenv_Tenv*_tmp86D=({struct Cyc_Tcenv_Tenv*_tmp8BC=_region_malloc(newr,sizeof(*_tmp8BC));_tmp8BC->ns=te->ns;_tmp8BC->ae=te->ae;_tmp8BC->le=_tmp86C;_tmp8BC->allow_valueof=1;_tmp8BC->in_extern_c_include=te->in_extern_c_include;_tmp8BC;});
struct _tuple30 _tmp86E=Cyc_Tcutil_check_clause(loc,_tmp86D,_tmp82F,({const char*_tmp8BB="@requires";_tag_dyneither(_tmp8BB,sizeof(char),10);}),_tmp928);struct _tuple30 _tmp86F=_tmp86E;struct Cyc_Tcutil_CVTEnv _tmp8BA;struct Cyc_List_List*_tmp8B9;_LL748: _tmp8BA=_tmp86F.f1;_tmp8B9=_tmp86F.f2;_LL749:;
_tmp82F=_tmp8BA;
*_tmp927=_tmp8B9;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp92D,_tmp8B9);{
# 4407 "tcutil.cyc"
struct _tuple30 _tmp870=Cyc_Tcutil_check_clause(loc,_tmp86D,_tmp82F,({const char*_tmp8B8="@ensures";_tag_dyneither(_tmp8B8,sizeof(char),9);}),_tmp926);struct _tuple30 _tmp871=_tmp870;struct Cyc_Tcutil_CVTEnv _tmp8B7;struct Cyc_List_List*_tmp8B6;_LL74B: _tmp8B7=_tmp871.f1;_tmp8B6=_tmp871.f2;_LL74C:;
_tmp82F=_tmp8B7;
*_tmp925=_tmp8B6;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp92D,_tmp8B6);
# 4412
if(*_tmp930 != 0)
_tmp82F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp82F,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp930),1,1);else{
# 4415
struct Cyc_List_List*effect=0;
# 4420
{struct Cyc_List_List*tvs=_tmp82F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp872=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp873=_tmp872;struct Cyc_Absyn_Tvar*_tmp88C;int _tmp88B;_LL74E: _tmp88C=_tmp873.f1;_tmp88B=_tmp873.f2;_LL74F:;
if(!_tmp88B)continue;{
void*_tmp874=Cyc_Absyn_compress_kb(_tmp88C->kind);void*_tmp875=_tmp874;struct Cyc_Core_Opt**_tmp88A;struct Cyc_Absyn_Kind*_tmp889;struct Cyc_Core_Opt**_tmp888;struct Cyc_Core_Opt**_tmp887;struct Cyc_Absyn_Kind*_tmp886;switch(*((int*)_tmp875)){case 2: _LL751: _tmp887=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp875)->f1;_tmp886=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp875)->f2;if(_tmp886->kind == Cyc_Absyn_RgnKind){_LL752:
# 4426
 if(_tmp886->aliasqual == Cyc_Absyn_Top){
*_tmp887=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmp889=_tmp886;goto _LL754;}
# 4429
*_tmp887=Cyc_Tcutil_kind_to_bound_opt(_tmp886);_tmp889=_tmp886;goto _LL754;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp875)->f2)->kind){case Cyc_Absyn_IntKind: _LL755: _LL756:
# 4432
 goto _LL758;case Cyc_Absyn_EffKind: _LL759: _tmp888=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp875)->f1;_LL75A:
# 4435
*_tmp888=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL75C;default: goto _LL75F;}}case 0: _LL753: _tmp889=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp875)->f1;if(_tmp889->kind == Cyc_Absyn_RgnKind){_LL754:
# 4431
 effect=({struct Cyc_List_List*_tmp876=_cycalloc(sizeof(*_tmp876));_tmp876->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp877=_cycalloc(sizeof(*_tmp877));_tmp877[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp878;_tmp878.tag=23;_tmp878.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp879=_cycalloc(sizeof(*_tmp879));_tmp879[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp87A;_tmp87A.tag=2;_tmp87A.f1=_tmp88C;_tmp87A;});_tmp879;});_tmp878;});_tmp877;});_tmp876->tl=effect;_tmp876;});goto _LL750;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp875)->f1)->kind){case Cyc_Absyn_IntKind: _LL757: _LL758:
# 4433
 goto _LL750;case Cyc_Absyn_EffKind: _LL75B: _LL75C:
# 4437
 effect=({struct Cyc_List_List*_tmp87B=_cycalloc(sizeof(*_tmp87B));_tmp87B->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp87C=_cycalloc(sizeof(*_tmp87C));_tmp87C[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp87D;_tmp87D.tag=2;_tmp87D.f1=_tmp88C;_tmp87D;});_tmp87C;});_tmp87B->tl=effect;_tmp87B;});goto _LL750;default: _LL75F: _LL760:
# 4442
 effect=({struct Cyc_List_List*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp882=_cycalloc(sizeof(*_tmp882));_tmp882[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp883;_tmp883.tag=25;_tmp883.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp884=_cycalloc(sizeof(*_tmp884));_tmp884[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp885;_tmp885.tag=2;_tmp885.f1=_tmp88C;_tmp885;});_tmp884;});_tmp883;});_tmp882;});_tmp881->tl=effect;_tmp881;});goto _LL750;}}default: _LL75D: _tmp88A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp875)->f1;_LL75E:
# 4439
*_tmp88A=({struct Cyc_Core_Opt*_tmp87E=_cycalloc(sizeof(*_tmp87E));_tmp87E->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp880;_tmp880.tag=2;_tmp880.f1=0;_tmp880.f2=& Cyc_Tcutil_ak;_tmp880;});_tmp87F;});_tmp87E;});goto _LL760;}_LL750:;};}}
# 4446
{struct Cyc_List_List*ts=_tmp82F.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp88D=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp88E=_tmp88D;void*_tmp899;int _tmp898;_LL762: _tmp899=_tmp88E.f1;_tmp898=_tmp88E.f2;_LL763:;
if(!_tmp898)continue;{
struct Cyc_Absyn_Kind*_tmp88F=Cyc_Tcutil_typ_kind(_tmp899);struct Cyc_Absyn_Kind*_tmp890=_tmp88F;switch(((struct Cyc_Absyn_Kind*)_tmp890)->kind){case Cyc_Absyn_RgnKind: _LL765: _LL766:
# 4451
 effect=({struct Cyc_List_List*_tmp891=_cycalloc(sizeof(*_tmp891));_tmp891->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp892=_cycalloc(sizeof(*_tmp892));_tmp892[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp893;_tmp893.tag=23;_tmp893.f1=_tmp899;_tmp893;});_tmp892;});_tmp891->tl=effect;_tmp891;});goto _LL764;case Cyc_Absyn_EffKind: _LL767: _LL768:
# 4453
 effect=({struct Cyc_List_List*_tmp894=_cycalloc(sizeof(*_tmp894));_tmp894->hd=_tmp899;_tmp894->tl=effect;_tmp894;});goto _LL764;case Cyc_Absyn_IntKind: _LL769: _LL76A:
 goto _LL764;default: _LL76B: _LL76C:
# 4456
 effect=({struct Cyc_List_List*_tmp895=_cycalloc(sizeof(*_tmp895));_tmp895->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp896=_cycalloc(sizeof(*_tmp896));_tmp896[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp897;_tmp897.tag=25;_tmp897.f1=_tmp899;_tmp897;});_tmp896;});_tmp895->tl=effect;_tmp895;});goto _LL764;}_LL764:;};}}
# 4459
*_tmp930=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp89B;_tmp89B.tag=24;_tmp89B.f1=effect;_tmp89B;});_tmp89A;});}
# 4465
if(*_tmp931 != 0){
struct Cyc_List_List*bs=*_tmp931;for(0;bs != 0;bs=bs->tl){
void*_tmp89C=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp89D=_tmp89C;struct Cyc_Core_Opt**_tmp8AD;struct Cyc_Absyn_Kind*_tmp8AC;struct Cyc_Core_Opt**_tmp8AB;struct Cyc_Core_Opt**_tmp8AA;struct Cyc_Core_Opt**_tmp8A9;struct Cyc_Core_Opt**_tmp8A8;struct Cyc_Core_Opt**_tmp8A7;struct Cyc_Core_Opt**_tmp8A6;struct Cyc_Core_Opt**_tmp8A5;struct Cyc_Core_Opt**_tmp8A4;struct Cyc_Core_Opt**_tmp8A3;switch(*((int*)_tmp89D)){case 1: _LL76E: _tmp8A3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL76F:
# 4469
({struct Cyc_String_pa_PrintArg_struct _tmp8A0;_tmp8A0.tag=0;_tmp8A0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp89E[1]={& _tmp8A0};Cyc_Tcutil_warn(loc,({const char*_tmp89F="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp89F,sizeof(char),47);}),_tag_dyneither(_tmp89E,sizeof(void*),1));});});
# 4471
_tmp8A4=_tmp8A3;goto _LL771;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f2)->aliasqual == Cyc_Absyn_Top){_LL770: _tmp8A4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL771:
 _tmp8A5=_tmp8A4;goto _LL773;}else{goto _LL780;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f2)->aliasqual){case Cyc_Absyn_Top: _LL772: _tmp8A5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL773:
 _tmp8A6=_tmp8A5;goto _LL775;case Cyc_Absyn_Aliasable: _LL774: _tmp8A6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL775:
 _tmp8A8=_tmp8A6;goto _LL777;default: _LL77A: _tmp8A7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL77B:
# 4478
 _tmp8AA=_tmp8A7;goto _LL77D;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f2)->aliasqual){case Cyc_Absyn_Top: _LL776: _tmp8A8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL777:
# 4475
 _tmp8A9=_tmp8A8;goto _LL779;case Cyc_Absyn_Aliasable: _LL778: _tmp8A9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL779:
# 4477
*_tmp8A9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL76D;default: _LL77C: _tmp8AA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL77D:
# 4480
*_tmp8AA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL76D;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f2)->aliasqual == Cyc_Absyn_Top){_LL77E: _tmp8AB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_LL77F:
# 4482
*_tmp8AB=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL76D;}else{goto _LL780;}default: _LL780: _tmp8AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f1;_tmp8AC=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp89D)->f2;_LL781:
# 4484
*_tmp8AD=Cyc_Tcutil_kind_to_bound_opt(_tmp8AC);goto _LL76D;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp89D)->f1)->kind == Cyc_Absyn_MemKind){_LL782: _LL783:
# 4486
({void*_tmp8A1=0;Cyc_Tcutil_terr(loc,({const char*_tmp8A2="functions can't abstract M types";_tag_dyneither(_tmp8A2,sizeof(char),33);}),_tag_dyneither(_tmp8A1,sizeof(void*),0));});goto _LL76D;}else{_LL784: _LL785:
 goto _LL76D;}}_LL76D:;}}
# 4491
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp82F.kind_env,*_tmp931);
_tmp82F.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmp82F.r,_tmp82F.free_vars,*_tmp931);
# 4494
{struct Cyc_List_List*tvs=_tmp82F.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp8AE=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp8AF=_tmp8AE;struct Cyc_Absyn_Tvar*_tmp8B1;int _tmp8B0;_LL787: _tmp8B1=_tmp8AF.f1;_tmp8B0=_tmp8AF.f2;_LL788:;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8B1,_tmp8B0);}}{
# 4499
struct Cyc_List_List*evs=_tmp82F.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp8B2=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp8B3=_tmp8B2;void*_tmp8B5;int _tmp8B4;_LL78A: _tmp8B5=_tmp8B3.f1;_tmp8B4=_tmp8B3.f2;_LL78B:;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp8B5,_tmp8B4);}};};};}
# 4504
_npop_handler(0);goto _LL66E;
# 4278 "tcutil.cyc"
;_pop_region(newr);};}case 10: _LL691: _tmp932=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp775)->f1;_LL692:
# 4507 "tcutil.cyc"
 for(0;_tmp932 != 0;_tmp932=_tmp932->tl){
struct _tuple12*_tmp8BE=(struct _tuple12*)_tmp932->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp8BE).f2,1,0);
((*_tmp8BE).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp8BE).f1).print_const,(*_tmp8BE).f2);}
# 4513
goto _LL66E;case 12: _LL693: _tmp934=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp775)->f1;_tmp933=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp775)->f2;_LL694: {
# 4517
struct _RegionHandle _tmp8BF=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmp8BF;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp933 != 0;_tmp933=_tmp933->tl){
struct Cyc_Absyn_Aggrfield*_tmp8C0=(struct Cyc_Absyn_Aggrfield*)_tmp933->hd;struct Cyc_Absyn_Aggrfield*_tmp8C1=_tmp8C0;struct _dyneither_ptr*_tmp8D3;struct Cyc_Absyn_Tqual*_tmp8D2;void*_tmp8D1;struct Cyc_Absyn_Exp*_tmp8D0;struct Cyc_List_List*_tmp8CF;struct Cyc_Absyn_Exp*_tmp8CE;_LL78D: _tmp8D3=_tmp8C1->name;_tmp8D2=(struct Cyc_Absyn_Tqual*)& _tmp8C1->tq;_tmp8D1=_tmp8C1->type;_tmp8D0=_tmp8C1->width;_tmp8CF=_tmp8C1->attributes;_tmp8CE=_tmp8C1->requires_clause;_LL78E:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp8D3))
({struct Cyc_String_pa_PrintArg_struct _tmp8C4;_tmp8C4.tag=0;_tmp8C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8D3);({void*_tmp8C2[1]={& _tmp8C4};Cyc_Tcutil_terr(loc,({const char*_tmp8C3="duplicate field %s";_tag_dyneither(_tmp8C3,sizeof(char),19);}),_tag_dyneither(_tmp8C2,sizeof(void*),1));});});
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp8D3,({const char*_tmp8C5="";_tag_dyneither(_tmp8C5,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp8C6=_region_malloc(aprev_rgn,sizeof(*_tmp8C6));_tmp8C6->hd=_tmp8D3;_tmp8C6->tl=prev_fields;_tmp8C6;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8D1,1,0);
_tmp8D2->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8D2->print_const,_tmp8D1);
# 4531
Cyc_Tcutil_check_bitfield(loc,te,_tmp8D1,_tmp8D0,_tmp8D3);
Cyc_Tcutil_check_field_atts(loc,_tmp8D3,_tmp8CF);
if(_tmp8CE != 0){
# 4535
if(_tmp934 != Cyc_Absyn_UnionA)
({void*_tmp8C7=0;Cyc_Tcutil_terr(loc,({const char*_tmp8C8="@requires clause is only allowed on union members";_tag_dyneither(_tmp8C8,sizeof(char),50);}),_tag_dyneither(_tmp8C7,sizeof(void*),0));});{
struct _RegionHandle _tmp8C9=_new_region("temp");struct _RegionHandle*temp=& _tmp8C9;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp8CA=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp8CA,0,_tmp8CE);}
# 4541
if(!Cyc_Tcutil_is_integral(_tmp8CE))
({struct Cyc_String_pa_PrintArg_struct _tmp8CD;_tmp8CD.tag=0;_tmp8CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp8CE->topt)));({void*_tmp8CB[1]={& _tmp8CD};Cyc_Tcutil_terr(loc,({const char*_tmp8CC="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp8CC,sizeof(char),54);}),_tag_dyneither(_tmp8CB,sizeof(void*),1));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp8CE,te,cvtenv);
# 4537
;_pop_region(temp);};}}}
# 4548
_npop_handler(0);goto _LL66E;
# 4517
;_pop_region(aprev_rgn);}case 11: _LL695: _tmp936=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp775)->f1).aggr_info;_tmp935=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp775)->f1).targs;_LL696:
# 4551
{union Cyc_Absyn_AggrInfoU _tmp8D4=*_tmp936;union Cyc_Absyn_AggrInfoU _tmp8D5=_tmp8D4;struct Cyc_Absyn_Aggrdecl*_tmp8F9;enum Cyc_Absyn_AggrKind _tmp8F8;struct _tuple2*_tmp8F7;struct Cyc_Core_Opt*_tmp8F6;if((_tmp8D5.UnknownAggr).tag == 1){_LL790: _tmp8F8=((_tmp8D5.UnknownAggr).val).f1;_tmp8F7=((_tmp8D5.UnknownAggr).val).f2;_tmp8F6=((_tmp8D5.UnknownAggr).val).f3;_LL791: {
# 4553
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp8D6;_push_handler(& _tmp8D6);{int _tmp8D8=0;if(setjmp(_tmp8D6.handler))_tmp8D8=1;if(!_tmp8D8){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8F7);{
struct Cyc_Absyn_Aggrdecl*_tmp8D9=*adp;
if(_tmp8D9->kind != _tmp8F8){
if(_tmp8D9->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp8DD;_tmp8DD.tag=0;_tmp8DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8F7));({struct Cyc_String_pa_PrintArg_struct _tmp8DC;_tmp8DC.tag=0;_tmp8DC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4559
Cyc_Absynpp_qvar2string(_tmp8F7));({void*_tmp8DA[2]={& _tmp8DC,& _tmp8DD};Cyc_Tcutil_terr(loc,({const char*_tmp8DB="expecting struct %s instead of union %s";_tag_dyneither(_tmp8DB,sizeof(char),40);}),_tag_dyneither(_tmp8DA,sizeof(void*),2));});});});else{
# 4562
({struct Cyc_String_pa_PrintArg_struct _tmp8E1;_tmp8E1.tag=0;_tmp8E1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8F7));({struct Cyc_String_pa_PrintArg_struct _tmp8E0;_tmp8E0.tag=0;_tmp8E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4562
Cyc_Absynpp_qvar2string(_tmp8F7));({void*_tmp8DE[2]={& _tmp8E0,& _tmp8E1};Cyc_Tcutil_terr(loc,({const char*_tmp8DF="expecting union %s instead of struct %s";_tag_dyneither(_tmp8DF,sizeof(char),40);}),_tag_dyneither(_tmp8DE,sizeof(void*),2));});});});}}
# 4565
if((unsigned int)_tmp8F6  && (int)_tmp8F6->v){
if(!((unsigned int)_tmp8D9->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8D9->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp8E4;_tmp8E4.tag=0;_tmp8E4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8F7));({void*_tmp8E2[1]={& _tmp8E4};Cyc_Tcutil_terr(loc,({const char*_tmp8E3="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp8E3,sizeof(char),42);}),_tag_dyneither(_tmp8E2,sizeof(void*),1));});});}
# 4571
*_tmp936=Cyc_Absyn_KnownAggr(adp);};
# 4555
;_pop_handler();}else{void*_tmp8D7=(void*)_exn_thrown;void*_tmp8E5=_tmp8D7;void*_tmp8EB;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8E5)->tag == Cyc_Dict_Absent){_LL795: _LL796: {
# 4575
struct Cyc_Absyn_Aggrdecl*_tmp8E6=({struct Cyc_Absyn_Aggrdecl*_tmp8EA=_cycalloc(sizeof(*_tmp8EA));_tmp8EA->kind=_tmp8F8;_tmp8EA->sc=Cyc_Absyn_Extern;_tmp8EA->name=_tmp8F7;_tmp8EA->tvs=0;_tmp8EA->impl=0;_tmp8EA->attributes=0;_tmp8EA->expected_mem_kind=0;_tmp8EA;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp8E6);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8F7);
*_tmp936=Cyc_Absyn_KnownAggr(adp);
# 4580
if(*_tmp935 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8E9;_tmp8E9.tag=0;_tmp8E9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8F7));({void*_tmp8E7[1]={& _tmp8E9};Cyc_Tcutil_terr(loc,({const char*_tmp8E8="declare parameterized type %s before using";_tag_dyneither(_tmp8E8,sizeof(char),43);}),_tag_dyneither(_tmp8E7,sizeof(void*),1));});});
return cvtenv;}
# 4585
goto _LL794;}}else{_LL797: _tmp8EB=_tmp8E5;_LL798:(int)_rethrow(_tmp8EB);}_LL794:;}};}
# 4587
_tmp8F9=*adp;goto _LL793;}}else{_LL792: _tmp8F9=*(_tmp8D5.KnownAggr).val;_LL793: {
# 4589
struct Cyc_List_List*tvs=_tmp8F9->tvs;
struct Cyc_List_List*ts=*_tmp935;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp8EC=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8ED=(void*)ts->hd;
# 4597
{struct _tuple0 _tmp8EE=({struct _tuple0 _tmp8F1;_tmp8F1.f1=Cyc_Absyn_compress_kb(_tmp8EC->kind);_tmp8F1.f2=_tmp8ED;_tmp8F1;});struct _tuple0 _tmp8EF=_tmp8EE;struct Cyc_Absyn_Tvar*_tmp8F0;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8EF.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8EF.f2)->tag == 2){_LL79A: _tmp8F0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8EF.f2)->f1;_LL79B:
# 4599
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8F0);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8F0,1);
continue;}else{goto _LL79C;}}else{_LL79C: _LL79D:
 goto _LL799;}_LL799:;}{
# 4604
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4608
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8F4;_tmp8F4.tag=0;_tmp8F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8F9->name));({void*_tmp8F2[1]={& _tmp8F4};Cyc_Tcutil_terr(loc,({const char*_tmp8F3="too many parameters for type %s";_tag_dyneither(_tmp8F3,sizeof(char),32);}),_tag_dyneither(_tmp8F2,sizeof(void*),1));});});
if(tvs != 0){
# 4612
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5->hd=e;_tmp8F5->tl=hidden_ts;_tmp8F5;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4619
*_tmp935=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp935,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4621
if((allow_abs_aggr  && _tmp8F9->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4626
_tmp8F9->expected_mem_kind=1;}}_LL78F:;}
# 4629
goto _LL66E;case 17: _LL697: _tmp93A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp775)->f1;_tmp939=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp775)->f2;_tmp938=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp775)->f3;_tmp937=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp775)->f4;_LL698: {
# 4632
struct Cyc_List_List*targs=*_tmp939;
# 4634
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp8FA;_push_handler(& _tmp8FA);{int _tmp8FC=0;if(setjmp(_tmp8FA.handler))_tmp8FC=1;if(!_tmp8FC){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp93A);;_pop_handler();}else{void*_tmp8FB=(void*)_exn_thrown;void*_tmp8FD=_tmp8FB;void*_tmp901;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8FD)->tag == Cyc_Dict_Absent){_LL79F: _LL7A0:
# 4637
({struct Cyc_String_pa_PrintArg_struct _tmp900;_tmp900.tag=0;_tmp900.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp93A));({void*_tmp8FE[1]={& _tmp900};Cyc_Tcutil_terr(loc,({const char*_tmp8FF="unbound typedef name %s";_tag_dyneither(_tmp8FF,sizeof(char),24);}),_tag_dyneither(_tmp8FE,sizeof(void*),1));});});
return cvtenv;}else{_LL7A1: _tmp901=_tmp8FD;_LL7A2:(int)_rethrow(_tmp901);}_LL79E:;}};}
# 4640
*_tmp938=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmp902=_new_region("temp");struct _RegionHandle*temp=& _tmp902;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4646
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4651
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp903=_region_malloc(temp,sizeof(*_tmp903));_tmp903->hd=({struct _tuple16*_tmp904=_region_malloc(temp,sizeof(*_tmp904));_tmp904->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp904->f2=(void*)ts->hd;_tmp904;});_tmp903->tl=inst;_tmp903;});}
# 4655
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp907;_tmp907.tag=0;_tmp907.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp93A));({void*_tmp905[1]={& _tmp907};Cyc_Tcutil_terr(loc,({const char*_tmp906="too many parameters for typedef %s";_tag_dyneither(_tmp906,sizeof(char),35);}),_tag_dyneither(_tmp905,sizeof(void*),1));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4660
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp908=_cycalloc(sizeof(*_tmp908));_tmp908->hd=e;_tmp908->tl=hidden_ts;_tmp908;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp909=_cycalloc(sizeof(*_tmp909));_tmp909->hd=({struct _tuple16*_tmp90A=_cycalloc(sizeof(*_tmp90A));_tmp90A->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp90A->f2=e;_tmp90A;});_tmp909->tl=inst;_tmp909;});}
# 4668
*_tmp939=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4670
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp937=new_typ;}}
# 4677
_npop_handler(0);goto _LL66E;
# 4643
;_pop_region(temp);};}case 22: _LL699: _LL69A:
# 4678
 goto _LL69C;case 21: _LL69B: _LL69C:
 goto _LL69E;case 20: _LL69D: _LL69E:
 goto _LL66E;case 15: _LL69F: _tmp93B=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp775)->f1;_LL6A0:
# 4682
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp93B,1,allow_abs_aggr);goto _LL66E;case 16: _LL6A1: _tmp93D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp775)->f1;_tmp93C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp775)->f2;_LL6A2:
# 4685
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp93D,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp93C,1,0);
goto _LL66E;case 23: _LL6A3: _tmp93E=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp775)->f1;_LL6A4:
# 4689
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp93E,1,1);goto _LL66E;case 25: _LL6A5: _tmp93F=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp775)->f1;_LL6A6:
# 4691
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp93F,1,1);goto _LL66E;default: _LL6A7: _tmp940=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp775)->f1;_LL6A8:
# 4693
 for(0;_tmp940 != 0;_tmp940=_tmp940->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp940->hd,1,1);}
goto _LL66E;}_LL66E:;}
# 4697
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))
({struct Cyc_String_pa_PrintArg_struct _tmp945;_tmp945.tag=0;_tmp945.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));({struct Cyc_String_pa_PrintArg_struct _tmp944;_tmp944.tag=0;_tmp944.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));({struct Cyc_String_pa_PrintArg_struct _tmp943;_tmp943.tag=0;_tmp943.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp941[3]={& _tmp943,& _tmp944,& _tmp945};Cyc_Tcutil_terr(loc,({const char*_tmp942="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp942,sizeof(char),51);}),_tag_dyneither(_tmp941,sizeof(void*),3));});});});});
# 4701
return cvtenv;}
# 4709
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4711
{void*_tmp946=e->r;void*_tmp947=_tmp946;struct Cyc_Absyn_Exp*_tmp959;void*_tmp958;void*_tmp957;void*_tmp956;void*_tmp955;struct Cyc_Absyn_Exp*_tmp954;struct Cyc_Absyn_Exp*_tmp953;struct Cyc_Absyn_Exp*_tmp952;struct Cyc_Absyn_Exp*_tmp951;struct Cyc_Absyn_Exp*_tmp950;struct Cyc_Absyn_Exp*_tmp94F;struct Cyc_Absyn_Exp*_tmp94E;struct Cyc_Absyn_Exp*_tmp94D;struct Cyc_Absyn_Exp*_tmp94C;struct Cyc_Absyn_Exp*_tmp94B;struct Cyc_List_List*_tmp94A;switch(*((int*)_tmp947)){case 0: _LL7A4: _LL7A5:
 goto _LL7A7;case 31: _LL7A6: _LL7A7:
 goto _LL7A9;case 32: _LL7A8: _LL7A9:
 goto _LL7AB;case 1: _LL7AA: _LL7AB:
 goto _LL7A3;case 2: _LL7AC: _tmp94A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp947)->f2;_LL7AD:
# 4717
 for(0;_tmp94A != 0;_tmp94A=_tmp94A->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp94A->hd,te,cvtenv);}
goto _LL7A3;case 5: _LL7AE: _tmp94D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_tmp94C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp947)->f2;_tmp94B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp947)->f3;_LL7AF:
# 4721
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp94D,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp94C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp94B,te,cvtenv);
goto _LL7A3;case 6: _LL7B0: _tmp94F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_tmp94E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp947)->f2;_LL7B1:
 _tmp951=_tmp94F;_tmp950=_tmp94E;goto _LL7B3;case 7: _LL7B2: _tmp951=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_tmp950=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp947)->f2;_LL7B3:
 _tmp953=_tmp951;_tmp952=_tmp950;goto _LL7B5;case 8: _LL7B4: _tmp953=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_tmp952=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp947)->f2;_LL7B5:
# 4728
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp953,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp952,te,cvtenv);
goto _LL7A3;case 13: _LL7B6: _tmp955=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_tmp954=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp947)->f2;_LL7B7:
# 4732
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp954,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp955,1,0);
goto _LL7A3;case 18: _LL7B8: _tmp956=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_LL7B9:
 _tmp957=_tmp956;goto _LL7BB;case 16: _LL7BA: _tmp957=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_LL7BB:
# 4737
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp957,1,0);
goto _LL7A3;case 38: _LL7BC: _tmp958=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_LL7BD:
# 4740
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp958,1,0);
goto _LL7A3;case 17: _LL7BE: _tmp959=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp947)->f1;_LL7BF:
# 4743
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp959,te,cvtenv);
goto _LL7A3;default: _LL7C0: _LL7C1:
# 4746
({void*_tmp948=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp949="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp949,sizeof(char),66);}),_tag_dyneither(_tmp948,sizeof(void*),0));});}_LL7A3:;}
# 4748
return cvtenv;}
# 4751
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4756
struct Cyc_List_List*_tmp95A=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4759
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp95B=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp95C=_tmp95B;struct Cyc_Absyn_Tvar*_tmp95D;_LL7C3: _tmp95D=_tmp95C.f1;_LL7C4:;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp95A,_tmp95D);}}
# 4767
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp95E=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp95F=_tmp95E;void*_tmp968;_LL7C6: _tmp968=_tmp95F.f1;_LL7C7:;{
void*_tmp960=Cyc_Tcutil_compress(_tmp968);void*_tmp961=_tmp960;struct Cyc_Core_Opt**_tmp967;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp961)->tag == 1){_LL7C9: _tmp967=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp961)->f4;_LL7CA:
# 4771
 if(*_tmp967 == 0)
*_tmp967=({struct Cyc_Core_Opt*_tmp962=_cycalloc(sizeof(*_tmp962));_tmp962->v=_tmp95A;_tmp962;});else{
# 4775
struct Cyc_List_List*_tmp963=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp967))->v;
struct Cyc_List_List*_tmp964=0;
for(0;_tmp963 != 0;_tmp963=_tmp963->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp95A,(struct Cyc_Absyn_Tvar*)_tmp963->hd))
_tmp964=({struct Cyc_List_List*_tmp965=_cycalloc(sizeof(*_tmp965));_tmp965->hd=(struct Cyc_Absyn_Tvar*)_tmp963->hd;_tmp965->tl=_tmp964;_tmp965;});}
*_tmp967=({struct Cyc_Core_Opt*_tmp966=_cycalloc(sizeof(*_tmp966));_tmp966->v=_tmp964;_tmp966;});}
# 4782
goto _LL7C8;}else{_LL7CB: _LL7CC:
 goto _LL7C8;}_LL7C8:;};}}
# 4786
return cvt;}
# 4792
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp969=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp96A=_tmp969;switch(((struct Cyc_Absyn_Kind*)_tmp96A)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp96A)->aliasqual){case Cyc_Absyn_Unique: _LL7CE: _LL7CF:
# 4797
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp96B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp96C="can't unify evar with unique region!";_tag_dyneither(_tmp96C,sizeof(char),37);}),_tag_dyneither(_tmp96B,sizeof(void*),0));});
goto _LL7CD;case Cyc_Absyn_Aliasable: _LL7D0: _LL7D1:
 goto _LL7D3;default: _LL7D2: _LL7D3:
# 4802
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp96D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp96E="can't unify evar with heap!";_tag_dyneither(_tmp96E,sizeof(char),28);}),_tag_dyneither(_tmp96D,sizeof(void*),0));});
goto _LL7CD;}case Cyc_Absyn_EffKind: _LL7D4: _LL7D5:
# 4805
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp96F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp970="can't unify evar with {}!";_tag_dyneither(_tmp970,sizeof(char),26);}),_tag_dyneither(_tmp96F,sizeof(void*),0));});
goto _LL7CD;default: _LL7D6: _LL7D7:
# 4808
({struct Cyc_String_pa_PrintArg_struct _tmp974;_tmp974.tag=0;_tmp974.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));({struct Cyc_String_pa_PrintArg_struct _tmp973;_tmp973.tag=0;_tmp973.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));({void*_tmp971[2]={& _tmp973,& _tmp974};Cyc_Tcutil_terr(loc,({const char*_tmp972="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp972,sizeof(char),52);}),_tag_dyneither(_tmp971,sizeof(void*),2));});});});
goto _LL7CD;}_LL7CD:;}}
# 4820
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp975=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmp976=_new_region("temp");struct _RegionHandle*temp=& _tmp976;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp977=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp995;_tmp995.r=temp;_tmp995.kind_env=_tmp975;_tmp995.free_vars=0;_tmp995.free_evars=0;_tmp995.generalize_evars=generalize_evars;_tmp995.fn_result=0;_tmp995;}),expected_kind,1,t);
# 4829
struct Cyc_List_List*_tmp978=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp977.free_vars);
struct Cyc_List_List*_tmp979=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp977.free_evars);
# 4833
if(_tmp975 != 0){
struct Cyc_List_List*_tmp97A=0;
{struct Cyc_List_List*_tmp97B=_tmp978;for(0;(unsigned int)_tmp97B;_tmp97B=_tmp97B->tl){
struct Cyc_Absyn_Tvar*_tmp97C=(struct Cyc_Absyn_Tvar*)_tmp97B->hd;
int found=0;
{struct Cyc_List_List*_tmp97D=_tmp975;for(0;(unsigned int)_tmp97D;_tmp97D=_tmp97D->tl){
if(Cyc_Absyn_tvar_cmp(_tmp97C,(struct Cyc_Absyn_Tvar*)_tmp97D->hd)== 0){found=1;break;}}}
if(!found)
_tmp97A=({struct Cyc_List_List*_tmp97E=_region_malloc(temp,sizeof(*_tmp97E));_tmp97E->hd=(struct Cyc_Absyn_Tvar*)_tmp97B->hd;_tmp97E->tl=_tmp97A;_tmp97E;});}}
# 4843
_tmp978=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp97A);}
# 4849
{struct Cyc_List_List*x=_tmp978;for(0;x != 0;x=x->tl){
void*_tmp97F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp980=_tmp97F;enum Cyc_Absyn_AliasQual _tmp98E;struct Cyc_Core_Opt**_tmp98D;struct Cyc_Absyn_Kind*_tmp98C;struct Cyc_Core_Opt**_tmp98B;struct Cyc_Core_Opt**_tmp98A;struct Cyc_Core_Opt**_tmp989;struct Cyc_Core_Opt**_tmp988;struct Cyc_Core_Opt**_tmp987;struct Cyc_Core_Opt**_tmp986;switch(*((int*)_tmp980)){case 1: _LL7D9: _tmp986=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp980)->f1;_LL7DA:
 _tmp987=_tmp986;goto _LL7DC;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f2)->aliasqual == Cyc_Absyn_Top){_LL7DB: _tmp987=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f1;_LL7DC:
 _tmp988=_tmp987;goto _LL7DE;}else{goto _LL7E5;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7DD: _tmp988=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f1;_LL7DE:
 _tmp989=_tmp988;goto _LL7E0;case Cyc_Absyn_Aliasable: _LL7DF: _tmp989=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f1;_LL7E0:
# 4855
*_tmp989=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL7D8;default: _LL7E1: _tmp98A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f1;_LL7E2:
# 4857
*_tmp98A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL7D8;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f2)->aliasqual == Cyc_Absyn_Top){_LL7E3: _tmp98B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f1;_LL7E4:
# 4859
*_tmp98B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL7D8;}else{goto _LL7E5;}default: _LL7E5: _tmp98D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f1;_tmp98C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp980)->f2;_LL7E6:
# 4861
*_tmp98D=Cyc_Tcutil_kind_to_bound_opt(_tmp98C);goto _LL7D8;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp980)->f1)->kind == Cyc_Absyn_MemKind){_LL7E7: _tmp98E=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp980)->f1)->aliasqual;_LL7E8:
# 4863
({struct Cyc_String_pa_PrintArg_struct _tmp984;_tmp984.tag=0;_tmp984.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp985=_cycalloc_atomic(sizeof(*_tmp985));_tmp985->kind=Cyc_Absyn_MemKind;_tmp985->aliasqual=_tmp98E;_tmp985;})));({struct Cyc_String_pa_PrintArg_struct _tmp983;_tmp983.tag=0;_tmp983.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));({void*_tmp981[2]={& _tmp983,& _tmp984};Cyc_Tcutil_terr(loc,({const char*_tmp982="type variable %s cannot have kind %s";_tag_dyneither(_tmp982,sizeof(char),37);}),_tag_dyneither(_tmp981,sizeof(void*),2));});});});
goto _LL7D8;}else{_LL7E9: _LL7EA:
 goto _LL7D8;}}_LL7D8:;}}
# 4870
if(_tmp978 != 0  || _tmp979 != 0){
{void*_tmp98F=Cyc_Tcutil_compress(t);void*_tmp990=_tmp98F;struct Cyc_List_List**_tmp991;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp990)->tag == 9){_LL7EC: _tmp991=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp990)->f1).tvars;_LL7ED:
# 4873
 if(*_tmp991 == 0){
# 4875
*_tmp991=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp978);
_tmp978=0;}
# 4878
goto _LL7EB;}else{_LL7EE: _LL7EF:
 goto _LL7EB;}_LL7EB:;}
# 4881
if(_tmp978 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp994;_tmp994.tag=0;_tmp994.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp978->hd)->name);({void*_tmp992[1]={& _tmp994};Cyc_Tcutil_terr(loc,({const char*_tmp993="unbound type variable %s";_tag_dyneither(_tmp993,sizeof(char),25);}),_tag_dyneither(_tmp992,sizeof(void*),1));});});
Cyc_Tcutil_check_free_evars(_tmp979,t,loc);}}
# 4825
;_pop_region(temp);}
# 4892
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4895
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp996=Cyc_Tcutil_compress(t);void*_tmp997=_tmp996;struct Cyc_List_List*_tmp9A3;void*_tmp9A2;struct Cyc_Absyn_Tqual _tmp9A1;void*_tmp9A0;struct Cyc_List_List*_tmp99F;struct Cyc_Absyn_Exp*_tmp99E;struct Cyc_List_List*_tmp99D;struct Cyc_Absyn_Exp*_tmp99C;struct Cyc_List_List*_tmp99B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->tag == 9){_LL7F1: _tmp9A3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).tvars;_tmp9A2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).effect;_tmp9A1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).ret_tqual;_tmp9A0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).ret_typ;_tmp99F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).args;_tmp99E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).requires_clause;_tmp99D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).requires_relns;_tmp99C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).ensures_clause;_tmp99B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp997)->f1).ensures_relns;_LL7F2:
# 4898
 fd->tvs=_tmp9A3;
fd->effect=_tmp9A2;
{struct Cyc_List_List*_tmp998=fd->args;for(0;_tmp998 != 0;(_tmp998=_tmp998->tl,_tmp99F=_tmp99F->tl)){
# 4902
(*((struct _tuple10*)_tmp998->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp99F))->hd)).f2;
(*((struct _tuple10*)_tmp998->hd)).f3=(*((struct _tuple10*)_tmp99F->hd)).f3;}}
# 4905
fd->ret_tqual=_tmp9A1;
fd->ret_type=_tmp9A0;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp9A0);
fd->requires_clause=_tmp99E;
fd->requires_relns=_tmp99D;
fd->ensures_clause=_tmp99C;
fd->ensures_relns=_tmp99B;
goto _LL7F0;}else{_LL7F3: _LL7F4:
({void*_tmp999=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp99A="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp99A,sizeof(char),38);}),_tag_dyneither(_tmp999,sizeof(void*),0));});}_LL7F0:;}{
# 4916
struct _RegionHandle _tmp9A4=_new_region("r");struct _RegionHandle*r=& _tmp9A4;_push_region(r);
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9A5="function declaration has repeated parameter";_tag_dyneither(_tmp9A5,sizeof(char),44);}));
# 4921
fd->cached_typ=t;
# 4916
;_pop_region(r);};}
# 4926
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4932
struct _RegionHandle _tmp9A6=_new_region("r");struct _RegionHandle*r=& _tmp9A6;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp9A7=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9AF;_tmp9AF.r=r;_tmp9AF.kind_env=bound_tvars;_tmp9AF.free_vars=0;_tmp9AF.free_evars=0;_tmp9AF.generalize_evars=0;_tmp9AF.fn_result=0;_tmp9AF;}),expected_kind,allow_abs_aggr,t);
# 4936
struct Cyc_List_List*_tmp9A8=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp9A7.free_vars),bound_tvars);
# 4938
struct Cyc_List_List*_tmp9A9=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp9A7.free_evars);
{struct Cyc_List_List*fs=_tmp9A8;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9AA=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp9AE;_tmp9AE.tag=0;_tmp9AE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp9AD;_tmp9AD.tag=0;_tmp9AD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9AA);({void*_tmp9AB[2]={& _tmp9AD,& _tmp9AE};Cyc_Tcutil_terr(loc,({const char*_tmp9AC="unbound type variable %s in type %s";_tag_dyneither(_tmp9AC,sizeof(char),36);}),_tag_dyneither(_tmp9AB,sizeof(void*),2));});});});}}
# 4943
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp9A9,t,loc);}
# 4933
;_pop_region(r);}
# 4948
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4953
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4959
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4962
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9B3;_tmp9B3.tag=0;_tmp9B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));({struct Cyc_String_pa_PrintArg_struct _tmp9B2;_tmp9B2.tag=0;_tmp9B2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp9B0[2]={& _tmp9B2,& _tmp9B3};Cyc_Tcutil_terr(loc,({const char*_tmp9B1="%s: %s";_tag_dyneither(_tmp9B1,sizeof(char),7);}),_tag_dyneither(_tmp9B0,sizeof(void*),2));});});});}}}
# 4968
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4972
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4976
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Absynpp_tvar2string,({const char*_tmp9B4="duplicate type variable";_tag_dyneither(_tmp9B4,sizeof(char),24);}));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4990 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4995
struct _RegionHandle _tmp9B5=_new_region("temp");struct _RegionHandle*temp=& _tmp9B5;_push_region(temp);
# 4999
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*_tmp9B6="";_tag_dyneither(_tmp9B6,sizeof(char),1);}))!= 0)
fields=({struct Cyc_List_List*_tmp9B7=_region_malloc(temp,sizeof(*_tmp9B7));_tmp9B7->hd=({struct _tuple32*_tmp9B8=_region_malloc(temp,sizeof(*_tmp9B8));_tmp9B8->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp9B8->f2=0;_tmp9B8;});_tmp9B7->tl=fields;_tmp9B7;});}}
# 5004
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5006
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp9DE="struct";_tag_dyneither(_tmp9DE,sizeof(char),7);}):({const char*_tmp9DF="union";_tag_dyneither(_tmp9DF,sizeof(char),6);});
# 5009
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp9B9=(struct _tuple33*)des->hd;struct _tuple33*_tmp9BA=_tmp9B9;struct Cyc_List_List*_tmp9D6;void*_tmp9D5;_LL7F6: _tmp9D6=_tmp9BA->f1;_tmp9D5=_tmp9BA->f2;_LL7F7:;
if(_tmp9D6 == 0){
# 5014
struct Cyc_List_List*_tmp9BB=fields;
for(0;_tmp9BB != 0;_tmp9BB=_tmp9BB->tl){
if(!(*((struct _tuple32*)_tmp9BB->hd)).f2){
(*((struct _tuple32*)_tmp9BB->hd)).f2=1;
(*((struct _tuple33*)des->hd)).f1=({struct Cyc_List_List*_tmp9BC=_cycalloc(sizeof(*_tmp9BC));_tmp9BC->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp9BD=_cycalloc(sizeof(*_tmp9BD));_tmp9BD[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp9BE;_tmp9BE.tag=1;_tmp9BE.f1=((*((struct _tuple32*)_tmp9BB->hd)).f1)->name;_tmp9BE;});_tmp9BD;});_tmp9BC->tl=0;_tmp9BC;});
ans=({struct Cyc_List_List*_tmp9BF=_region_malloc(rgn,sizeof(*_tmp9BF));_tmp9BF->hd=({struct _tuple34*_tmp9C0=_region_malloc(rgn,sizeof(*_tmp9C0));_tmp9C0->f1=(*((struct _tuple32*)_tmp9BB->hd)).f1;_tmp9C0->f2=_tmp9D5;_tmp9C0;});_tmp9BF->tl=ans;_tmp9BF;});
break;}}
# 5022
if(_tmp9BB == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9C3;_tmp9C3.tag=0;_tmp9C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9C1[1]={& _tmp9C3};Cyc_Tcutil_terr(loc,({const char*_tmp9C2="too many arguments to %s";_tag_dyneither(_tmp9C2,sizeof(char),25);}),_tag_dyneither(_tmp9C1,sizeof(void*),1));});});}else{
if(_tmp9D6->tl != 0)
# 5026
({void*_tmp9C4=0;Cyc_Tcutil_terr(loc,({const char*_tmp9C5="multiple designators are not yet supported";_tag_dyneither(_tmp9C5,sizeof(char),43);}),_tag_dyneither(_tmp9C4,sizeof(void*),0));});else{
# 5029
void*_tmp9C6=(void*)_tmp9D6->hd;void*_tmp9C7=_tmp9C6;struct _dyneither_ptr*_tmp9D4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp9C7)->tag == 0){_LL7F9: _LL7FA:
# 5031
({struct Cyc_String_pa_PrintArg_struct _tmp9CA;_tmp9CA.tag=0;_tmp9CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9C8[1]={& _tmp9CA};Cyc_Tcutil_terr(loc,({const char*_tmp9C9="array designator used in argument to %s";_tag_dyneither(_tmp9C9,sizeof(char),40);}),_tag_dyneither(_tmp9C8,sizeof(void*),1));});});
goto _LL7F8;}else{_LL7FB: _tmp9D4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp9C7)->f1;_LL7FC: {
# 5034
struct Cyc_List_List*_tmp9CB=fields;
for(0;_tmp9CB != 0;_tmp9CB=_tmp9CB->tl){
if(Cyc_strptrcmp(_tmp9D4,((*((struct _tuple32*)_tmp9CB->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp9CB->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp9CE;_tmp9CE.tag=0;_tmp9CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9D4);({void*_tmp9CC[1]={& _tmp9CE};Cyc_Tcutil_terr(loc,({const char*_tmp9CD="member %s has already been used as an argument";_tag_dyneither(_tmp9CD,sizeof(char),47);}),_tag_dyneither(_tmp9CC,sizeof(void*),1));});});
(*((struct _tuple32*)_tmp9CB->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp9CF=_region_malloc(rgn,sizeof(*_tmp9CF));_tmp9CF->hd=({struct _tuple34*_tmp9D0=_region_malloc(rgn,sizeof(*_tmp9D0));_tmp9D0->f1=(*((struct _tuple32*)_tmp9CB->hd)).f1;_tmp9D0->f2=_tmp9D5;_tmp9D0;});_tmp9CF->tl=ans;_tmp9CF;});
break;}}
# 5043
if(_tmp9CB == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9D3;_tmp9D3.tag=0;_tmp9D3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9D4);({void*_tmp9D1[1]={& _tmp9D3};Cyc_Tcutil_terr(loc,({const char*_tmp9D2="bad field designator %s";_tag_dyneither(_tmp9D2,sizeof(char),24);}),_tag_dyneither(_tmp9D1,sizeof(void*),1));});});
goto _LL7F8;}}_LL7F8:;}}}
# 5048
if(aggr_kind == Cyc_Absyn_StructA)
# 5050
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp9D7=0;Cyc_Tcutil_terr(loc,({const char*_tmp9D8="too few arguments to struct";_tag_dyneither(_tmp9D8,sizeof(char),28);}),_tag_dyneither(_tmp9D7,sizeof(void*),0));});
break;}}else{
# 5057
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp9D9=0;Cyc_Tcutil_terr(loc,({const char*_tmp9DA="only one member of a union is allowed";_tag_dyneither(_tmp9DA,sizeof(char),38);}),_tag_dyneither(_tmp9D9,sizeof(void*),0));});
found=1;}}
# 5064
if(!found)({void*_tmp9DB=0;Cyc_Tcutil_terr(loc,({const char*_tmp9DC="missing member for union";_tag_dyneither(_tmp9DC,sizeof(char),25);}),_tag_dyneither(_tmp9DB,sizeof(void*),0));});}{
# 5067
struct Cyc_List_List*_tmp9DD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmp9DD;};};}
# 4999
;_pop_region(temp);}
# 5070
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9E0=Cyc_Tcutil_compress(t);void*_tmp9E1=_tmp9E0;void*_tmp9E5;union Cyc_Absyn_Constraint*_tmp9E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E1)->tag == 5){_LL7FE: _tmp9E5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E1)->f1).elt_typ;_tmp9E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E1)->f1).ptr_atts).bounds;_LL7FF: {
# 5073
void*_tmp9E2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9E4);void*_tmp9E3=_tmp9E2;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9E3)->tag == 0){_LL803: _LL804:
# 5075
*elt_typ_dest=_tmp9E5;
return 1;}else{_LL805: _LL806:
 return 0;}_LL802:;}}else{_LL800: _LL801:
# 5079
 return 0;}_LL7FD:;}
# 5083
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9E6=Cyc_Tcutil_compress(t);void*_tmp9E7=_tmp9E6;void*_tmp9E9;union Cyc_Absyn_Constraint*_tmp9E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E7)->tag == 5){_LL808: _tmp9E9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E7)->f1).elt_typ;_tmp9E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E7)->f1).ptr_atts).zero_term;_LL809:
# 5086
*elt_typ_dest=_tmp9E9;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9E8);}else{_LL80A: _LL80B:
 return 0;}_LL807:;}
# 5094
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5096
void*_tmp9EA=Cyc_Tcutil_compress(t);void*_tmp9EB=_tmp9EA;void*_tmp9F4;struct Cyc_Absyn_Tqual _tmp9F3;struct Cyc_Absyn_Exp*_tmp9F2;union Cyc_Absyn_Constraint*_tmp9F1;void*_tmp9F0;union Cyc_Absyn_Constraint*_tmp9EF;union Cyc_Absyn_Constraint*_tmp9EE;switch(*((int*)_tmp9EB)){case 5: _LL80D: _tmp9F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9EB)->f1).elt_typ;_tmp9EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9EB)->f1).ptr_atts).bounds;_tmp9EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9EB)->f1).ptr_atts).zero_term;_LL80E:
# 5098
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9EE)){
*ptr_type=t;
*elt_type=_tmp9F0;
{void*_tmp9EC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp9EF);void*_tmp9ED=_tmp9EC;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9ED)->tag == 0){_LL814: _LL815:
*is_dyneither=1;goto _LL813;}else{_LL816: _LL817:
*is_dyneither=0;goto _LL813;}_LL813:;}
# 5105
return 1;}else{
return 0;}case 8: _LL80F: _tmp9F4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9EB)->f1).elt_type;_tmp9F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9EB)->f1).tq;_tmp9F2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9EB)->f1).num_elts;_tmp9F1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9EB)->f1).zero_term;_LL810:
# 5108
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9F1)){
*elt_type=_tmp9F4;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}default: _LL811: _LL812:
 return 0;}_LL80C:;}
# 5121
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5123
void*_tmp9F5=e1->r;void*_tmp9F6=_tmp9F5;struct Cyc_Absyn_Exp*_tmpA08;struct Cyc_Absyn_Exp*_tmpA07;struct Cyc_Absyn_Exp*_tmpA06;struct Cyc_Absyn_Exp*_tmpA05;struct Cyc_Absyn_Exp*_tmpA04;struct Cyc_Absyn_Exp*_tmpA03;switch(*((int*)_tmp9F6)){case 13: _LL819: _LL81A:
# 5125
({struct Cyc_String_pa_PrintArg_struct _tmp9F9;_tmp9F9.tag=0;_tmp9F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp9F7[1]={& _tmp9F9};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9F8="we have a cast in a lhs:  %s";_tag_dyneither(_tmp9F8,sizeof(char),29);}),_tag_dyneither(_tmp9F7,sizeof(void*),1));});});case 19: _LL81B: _tmpA03=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9F6)->f1;_LL81C:
 _tmpA04=_tmpA03;goto _LL81E;case 22: _LL81D: _tmpA04=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9F6)->f1;_LL81E:
# 5128
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA04->topt),ptr_type,is_dyneither,elt_type);case 21: _LL81F: _tmpA05=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9F6)->f1;_LL820:
 _tmpA06=_tmpA05;goto _LL822;case 20: _LL821: _tmpA06=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9F6)->f1;_LL822:
# 5132
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA06->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9FC;_tmp9FC.tag=0;_tmp9FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmp9FA[1]={& _tmp9FC};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9FB="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp9FB,sizeof(char),51);}),_tag_dyneither(_tmp9FA,sizeof(void*),1));});});
return 0;case 12: _LL823: _tmpA07=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9F6)->f1;_LL824:
 _tmpA08=_tmpA07;goto _LL826;case 11: _LL825: _tmpA08=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9F6)->f1;_LL826:
# 5138
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA08->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9FF;_tmp9FF.tag=0;_tmp9FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmp9FD[1]={& _tmp9FF};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9FE="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp9FE,sizeof(char),49);}),_tag_dyneither(_tmp9FD,sizeof(void*),1));});});
return 0;case 1: _LL827: _LL828:
 return 0;default: _LL829: _LL82A:
({struct Cyc_String_pa_PrintArg_struct _tmpA02;_tmpA02.tag=0;_tmpA02.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA00[1]={& _tmpA02};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA01="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmpA01,sizeof(char),39);}),_tag_dyneither(_tmpA00,sizeof(void*),1));});});}_LL818:;}
# 5147
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5158
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5161
void*_tmpA09=Cyc_Tcutil_compress(r);void*_tmpA0A=_tmpA09;struct Cyc_Absyn_Tvar*_tmpA17;enum Cyc_Absyn_KindQual _tmpA16;enum Cyc_Absyn_AliasQual _tmpA15;switch(*((int*)_tmpA0A)){case 22: _LL82C: _LL82D:
 return !must_be_unique;case 21: _LL82E: _LL82F:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA0A)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA0A)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA0A)->f4 == 0){_LL830: _tmpA16=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA0A)->f3)->kind)->v)->kind;_tmpA15=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA0A)->f3)->kind)->v)->aliasqual;_LL831:
# 5165
 return _tmpA16 == Cyc_Absyn_RgnKind  && (_tmpA15 == Cyc_Absyn_Unique  || _tmpA15 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL834;}}else{goto _LL834;}}else{goto _LL834;}case 2: _LL832: _tmpA17=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA0A)->f1;_LL833: {
# 5169
struct Cyc_Absyn_Kind*_tmpA0B=Cyc_Tcutil_tvar_kind(_tmpA17,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmpA0C=_tmpA0B;enum Cyc_Absyn_KindQual _tmpA14;enum Cyc_Absyn_AliasQual _tmpA13;_LL837: _tmpA14=_tmpA0C->kind;_tmpA13=_tmpA0C->aliasqual;_LL838:;
if(_tmpA14 == Cyc_Absyn_RgnKind  && (_tmpA13 == Cyc_Absyn_Unique  || _tmpA13 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpA0D=Cyc_Absyn_compress_kb(_tmpA17->kind);void*_tmpA0E=_tmpA0D;struct Cyc_Core_Opt**_tmpA12;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA0E)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA0E)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA0E)->f2)->aliasqual == Cyc_Absyn_Top){_LL83A: _tmpA12=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA0E)->f1;_LL83B:
# 5173
*_tmpA12=({struct Cyc_Core_Opt*_tmpA0F=_cycalloc(sizeof(*_tmpA0F));_tmpA0F->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA10=_cycalloc(sizeof(*_tmpA10));_tmpA10[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA11;_tmpA11.tag=2;_tmpA11.f1=0;_tmpA11.f2=& Cyc_Tcutil_rk;_tmpA11;});_tmpA10;});_tmpA0F;});
return 0;}else{goto _LL83C;}}else{goto _LL83C;}}else{_LL83C: _LL83D:
 return 1;}_LL839:;}
# 5178
return 0;}default: _LL834: _LL835:
 return 0;}_LL82B:;}
# 5185
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpA18=Cyc_Tcutil_compress(t);void*_tmpA19=_tmpA18;struct Cyc_Absyn_Tvar*_tmpA28;void*_tmpA27;switch(*((int*)_tmpA19)){case 5: _LL83F: _tmpA27=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA19)->f1).ptr_atts).rgn;_LL840:
# 5188
 return Cyc_Tcutil_is_noalias_region(_tmpA27,must_be_unique);case 2: _LL841: _tmpA28=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA19)->f1;_LL842: {
# 5190
struct Cyc_Absyn_Kind*_tmpA1A=Cyc_Tcutil_tvar_kind(_tmpA28,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpA1B=_tmpA1A;enum Cyc_Absyn_KindQual _tmpA26;enum Cyc_Absyn_AliasQual _tmpA25;_LL846: _tmpA26=_tmpA1B->kind;_tmpA25=_tmpA1B->aliasqual;_LL847:;{
enum Cyc_Absyn_KindQual _tmpA1C=_tmpA26;switch(_tmpA1C){case Cyc_Absyn_BoxKind: _LL849: _LL84A:
 goto _LL84C;case Cyc_Absyn_AnyKind: _LL84B: _LL84C: goto _LL84E;case Cyc_Absyn_MemKind: _LL84D: _LL84E:
 if(_tmpA25 == Cyc_Absyn_Unique  || _tmpA25 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpA1D=Cyc_Absyn_compress_kb(_tmpA28->kind);void*_tmpA1E=_tmpA1D;struct Cyc_Core_Opt**_tmpA24;enum Cyc_Absyn_KindQual _tmpA23;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA1E)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA1E)->f2)->aliasqual == Cyc_Absyn_Top){_LL852: _tmpA24=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA1E)->f1;_tmpA23=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA1E)->f2)->kind;_LL853:
# 5196
*_tmpA24=({struct Cyc_Core_Opt*_tmpA1F=_cycalloc(sizeof(*_tmpA1F));_tmpA1F->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA20=_cycalloc(sizeof(*_tmpA20));_tmpA20[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA21;_tmpA21.tag=2;_tmpA21.f1=0;_tmpA21.f2=({struct Cyc_Absyn_Kind*_tmpA22=_cycalloc_atomic(sizeof(*_tmpA22));_tmpA22->kind=_tmpA23;_tmpA22->aliasqual=Cyc_Absyn_Aliasable;_tmpA22;});_tmpA21;});_tmpA20;});_tmpA1F;});
return 0;}else{goto _LL854;}}else{_LL854: _LL855:
# 5200
 return 1;}_LL851:;}
# 5203
return 0;default: _LL84F: _LL850:
 return 0;}_LL848:;};}default: _LL843: _LL844:
# 5206
 return 0;}_LL83E:;}
# 5209
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpA29=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpA29,0))return 1;{
void*_tmpA2A=_tmpA29;union Cyc_Absyn_DatatypeFieldInfoU _tmpA3E;struct Cyc_List_List*_tmpA3D;union Cyc_Absyn_DatatypeInfoU _tmpA3C;struct Cyc_List_List*_tmpA3B;struct Cyc_List_List*_tmpA3A;struct Cyc_Absyn_Aggrdecl**_tmpA39;struct Cyc_List_List*_tmpA38;struct Cyc_List_List*_tmpA37;switch(*((int*)_tmpA2A)){case 10: _LL857: _tmpA37=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA2A)->f1;_LL858:
# 5214
 while(_tmpA37 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpA37->hd)).f2))return 1;
_tmpA37=_tmpA37->tl;}
# 5218
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA2A)->f1).aggr_info).KnownAggr).tag == 2){_LL859: _tmpA39=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA2A)->f1).aggr_info).KnownAggr).val;_tmpA38=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA2A)->f1).targs;_LL85A:
# 5220
 if((*_tmpA39)->impl == 0)return 0;else{
# 5222
struct Cyc_List_List*_tmpA2B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA39)->tvs,_tmpA38);
struct Cyc_List_List*_tmpA2C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA39)->impl))->fields;
void*t;
while(_tmpA2C != 0){
t=_tmpA2B == 0?((struct Cyc_Absyn_Aggrfield*)_tmpA2C->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpA2B,((struct Cyc_Absyn_Aggrfield*)_tmpA2C->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA2C=_tmpA2C->tl;}
# 5230
return 0;}}else{_LL85D: _LL85E:
# 5240
 return 0;}case 12: _LL85B: _tmpA3A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA2A)->f2;_LL85C:
# 5233
 while(_tmpA3A != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA3A->hd)->type))return 1;
_tmpA3A=_tmpA3A->tl;}
# 5237
return 0;case 3: _LL85F: _tmpA3C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA2A)->f1).datatype_info;_tmpA3B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA2A)->f1).targs;_LL860: {
# 5242
union Cyc_Absyn_DatatypeInfoU _tmpA2D=_tmpA3C;struct Cyc_List_List*_tmpA31;struct Cyc_Core_Opt*_tmpA30;struct _tuple2*_tmpA2F;int _tmpA2E;if((_tmpA2D.UnknownDatatype).tag == 1){_LL866: _tmpA2F=((_tmpA2D.UnknownDatatype).val).name;_tmpA2E=((_tmpA2D.UnknownDatatype).val).is_extensible;_LL867:
# 5245
 return 0;}else{_LL868: _tmpA31=(*(_tmpA2D.KnownDatatype).val)->tvs;_tmpA30=(*(_tmpA2D.KnownDatatype).val)->fields;_LL869:
# 5248
 return 0;}_LL865:;}case 4: _LL861: _tmpA3E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA2A)->f1).field_info;_tmpA3D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA2A)->f1).targs;_LL862: {
# 5251
union Cyc_Absyn_DatatypeFieldInfoU _tmpA32=_tmpA3E;struct Cyc_Absyn_Datatypedecl*_tmpA36;struct Cyc_Absyn_Datatypefield*_tmpA35;if((_tmpA32.UnknownDatatypefield).tag == 1){_LL86B: _LL86C:
# 5254
 return 0;}else{_LL86D: _tmpA36=((_tmpA32.KnownDatatypefield).val).f1;_tmpA35=((_tmpA32.KnownDatatypefield).val).f2;_LL86E: {
# 5256
struct Cyc_List_List*_tmpA33=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA36->tvs,_tmpA3D);
struct Cyc_List_List*_tmpA34=_tmpA35->typs;
while(_tmpA34 != 0){
_tmpA29=_tmpA33 == 0?(*((struct _tuple12*)_tmpA34->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpA33,(*((struct _tuple12*)_tmpA34->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA29))return 1;
_tmpA34=_tmpA34->tl;}
# 5263
return 0;}}_LL86A:;}default: _LL863: _LL864:
# 5265
 return 0;}_LL856:;};}
# 5272
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpA3F=e->r;void*_tmpA40=_tmpA3F;struct Cyc_Absyn_Stmt*_tmpA56;struct Cyc_Absyn_Exp*_tmpA55;struct Cyc_Absyn_Exp*_tmpA54;struct Cyc_Absyn_Exp*_tmpA53;struct Cyc_Absyn_Exp*_tmpA52;struct Cyc_Absyn_Exp*_tmpA51;struct Cyc_Absyn_Exp*_tmpA50;struct Cyc_Absyn_Exp*_tmpA4F;struct _dyneither_ptr*_tmpA4E;struct Cyc_Absyn_Exp*_tmpA4D;struct Cyc_Absyn_Exp*_tmpA4C;switch(*((int*)_tmpA40)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA40)->f1)->tag == 1){_LL870: _LL871:
 return 0;}else{goto _LL882;}case 21: _LL872: _tmpA4C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA40)->f1;_LL873:
 _tmpA4D=_tmpA4C;goto _LL875;case 19: _LL874: _tmpA4D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA40)->f1;_LL875:
# 5277
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA4D->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpA4D);case 20: _LL876: _tmpA4F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA40)->f1;_tmpA4E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA40)->f2;_LL877:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA4F);case 22: _LL878: _tmpA51=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA40)->f1;_tmpA50=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA40)->f2;_LL879: {
# 5281
void*_tmpA41=Cyc_Tcutil_compress((void*)_check_null(_tmpA51->topt));void*_tmpA42=_tmpA41;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA42)->tag == 10){_LL885: _LL886:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA51);}else{_LL887: _LL888:
 return 0;}_LL884:;}case 5: _LL87A: _tmpA53=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA40)->f2;_tmpA52=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA40)->f3;_LL87B:
# 5286
 return Cyc_Tcutil_is_noalias_path(r,_tmpA53) && Cyc_Tcutil_is_noalias_path(r,_tmpA52);case 8: _LL87C: _tmpA54=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA40)->f2;_LL87D:
 _tmpA55=_tmpA54;goto _LL87F;case 13: _LL87E: _tmpA55=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA40)->f2;_LL87F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA55);case 36: _LL880: _tmpA56=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA40)->f1;_LL881:
# 5290
 while(1){
void*_tmpA43=_tmpA56->r;void*_tmpA44=_tmpA43;struct Cyc_Absyn_Exp*_tmpA4B;struct Cyc_Absyn_Decl*_tmpA4A;struct Cyc_Absyn_Stmt*_tmpA49;struct Cyc_Absyn_Stmt*_tmpA48;struct Cyc_Absyn_Stmt*_tmpA47;switch(*((int*)_tmpA44)){case 2: _LL88A: _tmpA48=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA44)->f1;_tmpA47=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA44)->f2;_LL88B:
 _tmpA56=_tmpA47;goto _LL889;case 12: _LL88C: _tmpA4A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA44)->f1;_tmpA49=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA44)->f2;_LL88D:
 _tmpA56=_tmpA49;goto _LL889;case 1: _LL88E: _tmpA4B=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA44)->f1;_LL88F:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA4B);default: _LL890: _LL891:
({void*_tmpA45=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA46="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA46,sizeof(char),40);}),_tag_dyneither(_tmpA45,sizeof(void*),0));});}_LL889:;}default: _LL882: _LL883:
# 5298
 return 1;}_LL86F:;}
# 5315 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5317
struct _tuple18 bogus_ans=({struct _tuple18 _tmpA9E;_tmpA9E.f1=0;_tmpA9E.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA9E;});
void*_tmpA57=e->r;void*_tmpA58=_tmpA57;struct Cyc_Absyn_Exp*_tmpA9D;struct Cyc_Absyn_Exp*_tmpA9C;struct Cyc_Absyn_Exp*_tmpA9B;struct Cyc_Absyn_Exp*_tmpA9A;struct _dyneither_ptr*_tmpA99;int _tmpA98;struct Cyc_Absyn_Exp*_tmpA97;struct _dyneither_ptr*_tmpA96;int _tmpA95;void*_tmpA94;switch(*((int*)_tmpA58)){case 1: _LL893: _tmpA94=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_LL894: {
# 5321
void*_tmpA59=_tmpA94;struct Cyc_Absyn_Vardecl*_tmpA66;struct Cyc_Absyn_Vardecl*_tmpA65;struct Cyc_Absyn_Vardecl*_tmpA64;struct Cyc_Absyn_Vardecl*_tmpA63;switch(*((int*)_tmpA59)){case 0: _LL8A0: _LL8A1:
 goto _LL8A3;case 2: _LL8A2: _LL8A3:
 return bogus_ans;case 1: _LL8A4: _tmpA63=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA59)->f1;_LL8A5: {
# 5325
void*_tmpA5A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA5B=_tmpA5A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA5B)->tag == 8){_LL8AD: _LL8AE:
# 5327
 return({struct _tuple18 _tmpA5C;_tmpA5C.f1=1;_tmpA5C.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA5C;});}else{_LL8AF: _LL8B0:
 return({struct _tuple18 _tmpA5D;_tmpA5D.f1=(_tmpA63->tq).real_const;_tmpA5D.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA5D;});}_LL8AC:;}case 4: _LL8A6: _tmpA64=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA59)->f1;_LL8A7: {
# 5331
void*_tmpA5E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA5F=_tmpA5E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA5F)->tag == 8){_LL8B2: _LL8B3:
 return({struct _tuple18 _tmpA60;_tmpA60.f1=1;_tmpA60.f2=(void*)_check_null(_tmpA64->rgn);_tmpA60;});}else{_LL8B4: _LL8B5:
# 5334
 _tmpA64->escapes=1;
return({struct _tuple18 _tmpA61;_tmpA61.f1=(_tmpA64->tq).real_const;_tmpA61.f2=(void*)_check_null(_tmpA64->rgn);_tmpA61;});}_LL8B1:;}case 5: _LL8A8: _tmpA65=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA59)->f1;_LL8A9:
# 5337
 _tmpA66=_tmpA65;goto _LL8AB;default: _LL8AA: _tmpA66=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA59)->f1;_LL8AB:
# 5339
 _tmpA66->escapes=1;
return({struct _tuple18 _tmpA62;_tmpA62.f1=(_tmpA66->tq).real_const;_tmpA62.f2=(void*)_check_null(_tmpA66->rgn);_tmpA62;});}_LL89F:;}case 20: _LL895: _tmpA97=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_tmpA96=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA58)->f2;_tmpA95=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA58)->f3;_LL896:
# 5344
 if(_tmpA95)return bogus_ans;{
# 5347
void*_tmpA67=Cyc_Tcutil_compress((void*)_check_null(_tmpA97->topt));void*_tmpA68=_tmpA67;struct Cyc_Absyn_Aggrdecl*_tmpA76;struct Cyc_List_List*_tmpA75;switch(*((int*)_tmpA68)){case 12: _LL8B7: _tmpA75=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA68)->f2;_LL8B8: {
# 5349
struct Cyc_Absyn_Aggrfield*_tmpA69=Cyc_Absyn_lookup_field(_tmpA75,_tmpA96);
if(_tmpA69 != 0  && _tmpA69->width == 0){
struct _tuple18 _tmpA6A=Cyc_Tcutil_addressof_props(te,_tmpA97);struct _tuple18 _tmpA6B=_tmpA6A;int _tmpA6E;void*_tmpA6D;_LL8BE: _tmpA6E=_tmpA6B.f1;_tmpA6D=_tmpA6B.f2;_LL8BF:;
return({struct _tuple18 _tmpA6C;_tmpA6C.f1=(_tmpA69->tq).real_const  || _tmpA6E;_tmpA6C.f2=_tmpA6D;_tmpA6C;});}
# 5354
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA68)->f1).aggr_info).KnownAggr).tag == 2){_LL8B9: _tmpA76=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA68)->f1).aggr_info).KnownAggr).val;_LL8BA: {
# 5356
struct Cyc_Absyn_Aggrfield*_tmpA6F=Cyc_Absyn_lookup_decl_field(_tmpA76,_tmpA96);
if(_tmpA6F != 0  && _tmpA6F->width == 0){
struct _tuple18 _tmpA70=Cyc_Tcutil_addressof_props(te,_tmpA97);struct _tuple18 _tmpA71=_tmpA70;int _tmpA74;void*_tmpA73;_LL8C1: _tmpA74=_tmpA71.f1;_tmpA73=_tmpA71.f2;_LL8C2:;
return({struct _tuple18 _tmpA72;_tmpA72.f1=(_tmpA6F->tq).real_const  || _tmpA74;_tmpA72.f2=_tmpA73;_tmpA72;});}
# 5361
return bogus_ans;}}else{goto _LL8BB;}default: _LL8BB: _LL8BC:
 return bogus_ans;}_LL8B6:;};case 21: _LL897: _tmpA9A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_tmpA99=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA58)->f2;_tmpA98=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA58)->f3;_LL898:
# 5366
 if(_tmpA98)return bogus_ans;{
# 5370
void*_tmpA77=Cyc_Tcutil_compress((void*)_check_null(_tmpA9A->topt));void*_tmpA78=_tmpA77;void*_tmpA7F;void*_tmpA7E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA78)->tag == 5){_LL8C4: _tmpA7F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA78)->f1).elt_typ;_tmpA7E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA78)->f1).ptr_atts).rgn;_LL8C5: {
# 5372
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpA79=Cyc_Tcutil_compress(_tmpA7F);void*_tmpA7A=_tmpA79;struct Cyc_Absyn_Aggrdecl*_tmpA7C;struct Cyc_List_List*_tmpA7B;switch(*((int*)_tmpA7A)){case 12: _LL8C9: _tmpA7B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA7A)->f2;_LL8CA:
# 5375
 finfo=Cyc_Absyn_lookup_field(_tmpA7B,_tmpA99);goto _LL8C8;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA7A)->f1).aggr_info).KnownAggr).tag == 2){_LL8CB: _tmpA7C=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA7A)->f1).aggr_info).KnownAggr).val;_LL8CC:
# 5377
 finfo=Cyc_Absyn_lookup_decl_field(_tmpA7C,_tmpA99);goto _LL8C8;}else{goto _LL8CD;}default: _LL8CD: _LL8CE:
 return bogus_ans;}_LL8C8:;}
# 5380
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpA7D;_tmpA7D.f1=(finfo->tq).real_const;_tmpA7D.f2=_tmpA7E;_tmpA7D;});
return bogus_ans;}}else{_LL8C6: _LL8C7:
 return bogus_ans;}_LL8C3:;};case 19: _LL899: _tmpA9B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_LL89A: {
# 5387
void*_tmpA80=Cyc_Tcutil_compress((void*)_check_null(_tmpA9B->topt));void*_tmpA81=_tmpA80;struct Cyc_Absyn_Tqual _tmpA84;void*_tmpA83;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA81)->tag == 5){_LL8D0: _tmpA84=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA81)->f1).elt_tq;_tmpA83=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA81)->f1).ptr_atts).rgn;_LL8D1:
# 5389
 return({struct _tuple18 _tmpA82;_tmpA82.f1=_tmpA84.real_const;_tmpA82.f2=_tmpA83;_tmpA82;});}else{_LL8D2: _LL8D3:
 return bogus_ans;}_LL8CF:;}case 22: _LL89B: _tmpA9D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_tmpA9C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA58)->f2;_LL89C: {
# 5395
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpA9D->topt));
void*_tmpA85=t;struct Cyc_Absyn_Tqual _tmpA91;struct Cyc_Absyn_Tqual _tmpA90;void*_tmpA8F;struct Cyc_List_List*_tmpA8E;switch(*((int*)_tmpA85)){case 10: _LL8D5: _tmpA8E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA85)->f1;_LL8D6: {
# 5399
struct _tuple14 _tmpA86=Cyc_Evexp_eval_const_uint_exp(_tmpA9C);struct _tuple14 _tmpA87=_tmpA86;unsigned int _tmpA8B;int _tmpA8A;_LL8DE: _tmpA8B=_tmpA87.f1;_tmpA8A=_tmpA87.f2;_LL8DF:;
if(!_tmpA8A)
return bogus_ans;{
struct _tuple12*_tmpA88=Cyc_Absyn_lookup_tuple_field(_tmpA8E,(int)_tmpA8B);
if(_tmpA88 != 0)
return({struct _tuple18 _tmpA89;_tmpA89.f1=((*_tmpA88).f1).real_const;_tmpA89.f2=(Cyc_Tcutil_addressof_props(te,_tmpA9D)).f2;_tmpA89;});
return bogus_ans;};}case 5: _LL8D7: _tmpA90=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA85)->f1).elt_tq;_tmpA8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA85)->f1).ptr_atts).rgn;_LL8D8:
# 5407
 return({struct _tuple18 _tmpA8C;_tmpA8C.f1=_tmpA90.real_const;_tmpA8C.f2=_tmpA8F;_tmpA8C;});case 8: _LL8D9: _tmpA91=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA85)->f1).tq;_LL8DA:
# 5413
 return({struct _tuple18 _tmpA8D;_tmpA8D.f1=_tmpA91.real_const;_tmpA8D.f2=(Cyc_Tcutil_addressof_props(te,_tmpA9D)).f2;_tmpA8D;});default: _LL8DB: _LL8DC:
 return bogus_ans;}_LL8D4:;}default: _LL89D: _LL89E:
# 5417
({void*_tmpA92=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpA93="unary & applied to non-lvalue";_tag_dyneither(_tmpA93,sizeof(char),30);}),_tag_dyneither(_tmpA92,sizeof(void*),0));});
return bogus_ans;}_LL892:;}
# 5424
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpA9F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpAA0=_tmpA9F;struct Cyc_Absyn_Exp*_tmpAA9;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAA0)->tag == 0){_LL8E1: _LL8E2:
 return;}else{_LL8E3: _tmpAA9=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAA0)->f1;_LL8E4: {
# 5429
struct _tuple14 _tmpAA1=Cyc_Evexp_eval_const_uint_exp(_tmpAA9);struct _tuple14 _tmpAA2=_tmpAA1;unsigned int _tmpAA8;int _tmpAA7;_LL8E6: _tmpAA8=_tmpAA2.f1;_tmpAA7=_tmpAA2.f2;_LL8E7:;
if(_tmpAA7  && _tmpAA8 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpAA6;_tmpAA6.tag=1;_tmpAA6.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpAA5;_tmpAA5.tag=1;_tmpAA5.f1=(unsigned long)((int)_tmpAA8);({void*_tmpAA3[2]={& _tmpAA5,& _tmpAA6};Cyc_Tcutil_terr(loc,({const char*_tmpAA4="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpAA4,sizeof(char),39);}),_tag_dyneither(_tmpAA3,sizeof(void*),2));});});});
return;}}_LL8E0:;};}
# 5436
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5440
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpAAA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpAAB=_tmpAAA;struct Cyc_Absyn_Exp*_tmpAB0;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAAB)->tag == 1){_LL8E9: _tmpAB0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAAB)->f1;_LL8EA: {
# 5443
struct _tuple14 _tmpAAC=Cyc_Evexp_eval_const_uint_exp(_tmpAB0);struct _tuple14 _tmpAAD=_tmpAAC;unsigned int _tmpAAF;int _tmpAAE;_LL8EE: _tmpAAF=_tmpAAD.f1;_tmpAAE=_tmpAAD.f2;_LL8EF:;
return _tmpAAE  && _tmpAAF == 1;}}else{_LL8EB: _LL8EC:
 return 0;}_LL8E8:;}
# 5449
int Cyc_Tcutil_bits_only(void*t){
void*_tmpAB1=Cyc_Tcutil_compress(t);void*_tmpAB2=_tmpAB1;struct Cyc_List_List*_tmpABC;struct Cyc_Absyn_Aggrdecl*_tmpABB;struct Cyc_List_List*_tmpABA;struct Cyc_List_List*_tmpAB9;void*_tmpAB8;union Cyc_Absyn_Constraint*_tmpAB7;switch(*((int*)_tmpAB2)){case 0: _LL8F1: _LL8F2:
 goto _LL8F4;case 6: _LL8F3: _LL8F4:
 goto _LL8F6;case 7: _LL8F5: _LL8F6:
 return 1;case 13: _LL8F7: _LL8F8:
 goto _LL8FA;case 14: _LL8F9: _LL8FA:
 return 0;case 8: _LL8FB: _tmpAB8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAB2)->f1).elt_type;_tmpAB7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAB2)->f1).zero_term;_LL8FC:
# 5459
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAB7) && Cyc_Tcutil_bits_only(_tmpAB8);case 10: _LL8FD: _tmpAB9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAB2)->f1;_LL8FE:
# 5461
 for(0;_tmpAB9 != 0;_tmpAB9=_tmpAB9->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpAB9->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB2)->f1).aggr_info).UnknownAggr).tag == 1){_LL8FF: _LL900:
 return 0;}else{_LL901: _tmpABB=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB2)->f1).aggr_info).KnownAggr).val;_tmpABA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB2)->f1).targs;_LL902:
# 5466
 if(_tmpABB->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpABB->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpAB3=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAB3;_push_region(rgn);
{struct Cyc_List_List*_tmpAB4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpABB->tvs,_tmpABA);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpABB->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpAB4,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAB5=0;_npop_handler(0);return _tmpAB5;}}}{
int _tmpAB6=1;_npop_handler(0);return _tmpAB6;};}
# 5473
;_pop_region(rgn);};};}case 12: _LL903: _tmpABC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAB2)->f2;_LL904:
# 5478
 for(0;_tmpABC != 0;_tmpABC=_tmpABC->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpABC->hd)->type))return 0;}
return 1;default: _LL905: _LL906:
 return 0;}_LL8F0:;}
# 5486
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpABD=e->r;void*_tmpABE=_tmpABD;struct Cyc_Absyn_Exp*_tmpAC0;struct Cyc_Absyn_Exp*_tmpABF;switch(*((int*)_tmpABE)){case 1: _LL908: _LL909:
 return 1;case 11: _LL90A: _tmpABF=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpABE)->f1;_LL90B:
 _tmpAC0=_tmpABF;goto _LL90D;case 12: _LL90C: _tmpAC0=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpABE)->f1;_LL90D:
 e=_tmpAC0;continue;default: _LL90E: _LL90F:
# 5493
 return 0;}_LL907:;}}
# 5503
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpAC1=e->r;void*_tmpAC2=_tmpAC1;struct Cyc_List_List*_tmpAE0;struct Cyc_List_List*_tmpADF;enum Cyc_Absyn_Primop _tmpADE;struct Cyc_List_List*_tmpADD;struct Cyc_List_List*_tmpADC;struct Cyc_List_List*_tmpADB;struct Cyc_List_List*_tmpADA;struct Cyc_Absyn_Exp*_tmpAD9;struct Cyc_Absyn_Exp*_tmpAD8;struct Cyc_Absyn_Exp*_tmpAD7;struct Cyc_Absyn_Exp*_tmpAD6;void*_tmpAD5;struct Cyc_Absyn_Exp*_tmpAD4;void*_tmpAD3;struct Cyc_Absyn_Exp*_tmpAD2;struct Cyc_Absyn_Exp*_tmpAD1;struct Cyc_Absyn_Exp*_tmpAD0;struct Cyc_Absyn_Exp*_tmpACF;struct Cyc_Absyn_Exp*_tmpACE;struct Cyc_Absyn_Exp*_tmpACD;struct Cyc_Absyn_Exp*_tmpACC;struct Cyc_Absyn_Exp*_tmpACB;void*_tmpACA;switch(*((int*)_tmpAC2)){case 0: _LL911: _LL912:
 goto _LL914;case 16: _LL913: _LL914:
 goto _LL916;case 17: _LL915: _LL916:
 goto _LL918;case 18: _LL917: _LL918:
 goto _LL91A;case 31: _LL919: _LL91A:
 goto _LL91C;case 32: _LL91B: _LL91C:
 return 1;case 1: _LL91D: _tmpACA=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_LL91E: {
# 5514
void*_tmpAC3=_tmpACA;struct Cyc_Absyn_Vardecl*_tmpAC9;struct Cyc_Absyn_Vardecl*_tmpAC8;switch(*((int*)_tmpAC3)){case 2: _LL940: _LL941:
 return 1;case 1: _LL942: _tmpAC8=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpAC3)->f1;_LL943: {
# 5517
void*_tmpAC4=Cyc_Tcutil_compress(_tmpAC8->type);void*_tmpAC5=_tmpAC4;switch(*((int*)_tmpAC5)){case 8: _LL94B: _LL94C:
 goto _LL94E;case 9: _LL94D: _LL94E:
 return 1;default: _LL94F: _LL950:
 return var_okay;}_LL94A:;}case 4: _LL944: _tmpAC9=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpAC3)->f1;_LL945:
# 5524
 if(_tmpAC9->sc == Cyc_Absyn_Static){
void*_tmpAC6=Cyc_Tcutil_compress(_tmpAC9->type);void*_tmpAC7=_tmpAC6;switch(*((int*)_tmpAC7)){case 8: _LL952: _LL953:
 goto _LL955;case 9: _LL954: _LL955:
 return 1;default: _LL956: _LL957:
 return var_okay;}_LL951:;}else{
# 5531
return var_okay;}case 0: _LL946: _LL947:
 return 0;default: _LL948: _LL949:
 return var_okay;}_LL93F:;}case 5: _LL91F: _tmpACD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_tmpACC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC2)->f2;_tmpACB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC2)->f3;_LL920:
# 5537
 return(Cyc_Tcutil_cnst_exp(0,_tmpACD) && 
Cyc_Tcutil_cnst_exp(0,_tmpACC)) && 
Cyc_Tcutil_cnst_exp(0,_tmpACB);case 8: _LL921: _tmpACF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_tmpACE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC2)->f2;_LL922:
# 5541
 return Cyc_Tcutil_cnst_exp(0,_tmpACF) && Cyc_Tcutil_cnst_exp(0,_tmpACE);case 11: _LL923: _tmpAD0=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_LL924:
 _tmpAD1=_tmpAD0;goto _LL926;case 12: _LL925: _tmpAD1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_LL926:
# 5544
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAD1);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC2)->f4 == Cyc_Absyn_No_coercion){_LL927: _tmpAD3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_tmpAD2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC2)->f2;_LL928:
# 5546
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAD2);}else{_LL929: _tmpAD5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_tmpAD4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC2)->f2;_LL92A:
# 5549
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAD4);}case 14: _LL92B: _tmpAD6=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_LL92C:
# 5551
 return Cyc_Tcutil_cnst_exp(1,_tmpAD6);case 26: _LL92D: _tmpAD8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC2)->f2;_tmpAD7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC2)->f3;_LL92E:
# 5553
 return Cyc_Tcutil_cnst_exp(0,_tmpAD8) && Cyc_Tcutil_cnst_exp(0,_tmpAD7);case 27: _LL92F: _tmpAD9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_LL930:
# 5555
 return Cyc_Tcutil_cnst_exp(0,_tmpAD9);case 25: _LL931: _tmpADA=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_LL932:
 _tmpADB=_tmpADA;goto _LL934;case 29: _LL933: _tmpADB=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC2)->f2;_LL934:
 _tmpADC=_tmpADB;goto _LL936;case 28: _LL935: _tmpADC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC2)->f3;_LL936:
# 5559
 for(0;_tmpADC != 0;_tmpADC=_tmpADC->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpADC->hd)).f2))
return 0;}
return 1;case 2: _LL937: _tmpADE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_tmpADD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAC2)->f2;_LL938:
# 5564
 _tmpADF=_tmpADD;goto _LL93A;case 23: _LL939: _tmpADF=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_LL93A:
 _tmpAE0=_tmpADF;goto _LL93C;case 30: _LL93B: _tmpAE0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC2)->f1;_LL93C:
# 5567
 for(0;_tmpAE0 != 0;_tmpAE0=_tmpAE0->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpAE0->hd))
return 0;}
return 1;default: _LL93D: _LL93E:
 return 0;}_LL910:;}
# 5575
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5579
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5581
int Cyc_Tcutil_supports_default(void*t){
void*_tmpAE1=Cyc_Tcutil_compress(t);void*_tmpAE2=_tmpAE1;struct Cyc_List_List*_tmpAEC;union Cyc_Absyn_AggrInfoU _tmpAEB;struct Cyc_List_List*_tmpAEA;struct Cyc_List_List*_tmpAE9;void*_tmpAE8;union Cyc_Absyn_Constraint*_tmpAE7;union Cyc_Absyn_Constraint*_tmpAE6;switch(*((int*)_tmpAE2)){case 0: _LL959: _LL95A:
 goto _LL95C;case 6: _LL95B: _LL95C:
 goto _LL95E;case 7: _LL95D: _LL95E:
 return 1;case 5: _LL95F: _tmpAE7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE2)->f1).ptr_atts).nullable;_tmpAE6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAE2)->f1).ptr_atts).bounds;_LL960: {
# 5588
void*_tmpAE3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAE6);void*_tmpAE4=_tmpAE3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAE4)->tag == 0){_LL970: _LL971:
 return 1;}else{_LL972: _LL973:
# 5591
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpAE7);}_LL96F:;}case 8: _LL961: _tmpAE8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE2)->f1).elt_type;_LL962:
# 5594
 return Cyc_Tcutil_supports_default(_tmpAE8);case 10: _LL963: _tmpAE9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAE2)->f1;_LL964:
# 5596
 for(0;_tmpAE9 != 0;_tmpAE9=_tmpAE9->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpAE9->hd)).f2))return 0;}
return 1;case 11: _LL965: _tmpAEB=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE2)->f1).aggr_info;_tmpAEA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAE2)->f1).targs;_LL966: {
# 5600
struct Cyc_Absyn_Aggrdecl*_tmpAE5=Cyc_Absyn_get_known_aggrdecl(_tmpAEB);
if(_tmpAE5->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE5->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpAE5->tvs,_tmpAEA,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAE5->impl))->fields);}case 12: _LL967: _tmpAEC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAE2)->f2;_LL968:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpAEC);case 14: _LL969: _LL96A:
# 5606
 goto _LL96C;case 13: _LL96B: _LL96C:
 return 1;default: _LL96D: _LL96E:
# 5609
 return 0;}_LL958:;}
# 5614
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpAED=t;struct Cyc_Absyn_Typedefdecl*_tmpAF2;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAED)->tag == 17){_LL975: _tmpAF2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAED)->f3;_LL976:
# 5617
 if(_tmpAF2 != 0){
struct Cyc_Absyn_Tqual _tmpAEE=_tmpAF2->tq;
if(((_tmpAEE.print_const  || _tmpAEE.q_volatile) || _tmpAEE.q_restrict) || _tmpAEE.real_const)
# 5622
({struct Cyc_String_pa_PrintArg_struct _tmpAF1;_tmpAF1.tag=0;_tmpAF1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpAEF[1]={& _tmpAF1};Cyc_Tcutil_warn(loc,({const char*_tmpAF0="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpAF0,sizeof(char),44);}),_tag_dyneither(_tmpAEF,sizeof(void*),1));});});}
# 5625
goto _LL974;}else{_LL977: _LL978:
 goto _LL974;}_LL974:;}
# 5630
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5632
struct _RegionHandle _tmpAF3=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAF3;_push_region(rgn);
{struct Cyc_List_List*_tmpAF4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpAF5=1;_npop_handler(0);return _tmpAF5;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpAF4,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpAF6=0;_npop_handler(0);return _tmpAF6;}}{
# 5640
int _tmpAF7=1;_npop_handler(0);return _tmpAF7;};}
# 5633
;_pop_region(rgn);}
# 5646
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpAF8=Cyc_Tcutil_compress(t);void*_tmpAF9=_tmpAF8;union Cyc_Absyn_Constraint*_tmpAFD;union Cyc_Absyn_Constraint*_tmpAFC;switch(*((int*)_tmpAF9)){case 6: _LL97A: _LL97B:
 goto _LL97D;case 7: _LL97C: _LL97D:
 return 1;case 5: _LL97E: _tmpAFD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAF9)->f1).ptr_atts).nullable;_tmpAFC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAF9)->f1).ptr_atts).bounds;_LL97F: {
# 5651
void*_tmpAFA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAFC);void*_tmpAFB=_tmpAFA;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAFB)->tag == 0){_LL983: _LL984:
# 5655
 return 0;}else{_LL985: _LL986:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAFD);}_LL982:;}default: _LL980: _LL981:
# 5658
 return 0;}_LL979:;}
# 5662
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpAFE=Cyc_Tcutil_compress(t);void*_tmpAFF=_tmpAFE;struct Cyc_List_List*_tmpB03;void*_tmpB02;switch(*((int*)_tmpAFF)){case 5: _LL988: _tmpB02=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAFF)->f1).elt_typ;_LL989:
 return Cyc_Tcutil_is_noreturn(_tmpB02);case 9: _LL98A: _tmpB03=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAFF)->f1).attributes;_LL98B:
# 5666
 for(0;_tmpB03 != 0;_tmpB03=_tmpB03->tl){
void*_tmpB00=(void*)_tmpB03->hd;void*_tmpB01=_tmpB00;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpB01)->tag == 4){_LL98F: _LL990:
 return 1;}else{_LL991: _LL992:
 continue;}_LL98E:;}
# 5671
goto _LL987;default: _LL98C: _LL98D:
 goto _LL987;}_LL987:;}
# 5674
return 0;}
# 5679
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpB04=Cyc_Tcutil_compress(t);void*_tmpB05=_tmpB04;struct Cyc_List_List**_tmpB0B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB05)->tag == 9){_LL994: _tmpB0B=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB05)->f1).attributes;_LL995: {
# 5682
struct Cyc_List_List*_tmpB06=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpB0B,(void*)atts->hd))
*_tmpB0B=({struct Cyc_List_List*_tmpB07=_cycalloc(sizeof(*_tmpB07));_tmpB07->hd=(void*)atts->hd;_tmpB07->tl=*_tmpB0B;_tmpB07;});}else{
# 5689
_tmpB06=({struct Cyc_List_List*_tmpB08=_cycalloc(sizeof(*_tmpB08));_tmpB08->hd=(void*)atts->hd;_tmpB08->tl=_tmpB06;_tmpB08;});}}
return _tmpB06;}}else{_LL996: _LL997:
({void*_tmpB09=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB0A="transfer_fn_type_atts";_tag_dyneither(_tmpB0A,sizeof(char),22);}),_tag_dyneither(_tmpB09,sizeof(void*),0));});}_LL993:;}
# 5696
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpB0C=Cyc_Tcutil_compress(t);void*_tmpB0D=_tmpB0C;struct Cyc_Absyn_Exp*_tmpB12;struct Cyc_Absyn_PtrInfo*_tmpB11;switch(*((int*)_tmpB0D)){case 5: _LL999: _tmpB11=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB0D)->f1;_LL99A:
# 5700
{void*_tmpB0E=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpB11->ptr_atts).bounds);void*_tmpB0F=_tmpB0E;struct Cyc_Absyn_Exp*_tmpB10;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB0F)->tag == 1){_LL9A0: _tmpB10=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB0F)->f1;_LL9A1:
 bound=_tmpB10;goto _LL99F;}else{_LL9A2: _LL9A3:
 goto _LL99F;}_LL99F:;}
# 5704
goto _LL998;case 8: _LL99B: _tmpB12=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB0D)->f1).num_elts;_LL99C:
# 5706
 bound=_tmpB12;
goto _LL998;default: _LL99D: _LL99E:
 goto _LL998;}_LL998:;}
# 5710
return bound;}
# 5715
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpB13=b;struct Cyc_Absyn_Vardecl*_tmpB17;struct Cyc_Absyn_Vardecl*_tmpB16;struct Cyc_Absyn_Vardecl*_tmpB15;struct Cyc_Absyn_Vardecl*_tmpB14;switch(*((int*)_tmpB13)){case 5: _LL9A5: _tmpB14=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpB13)->f1;_LL9A6:
 _tmpB15=_tmpB14;goto _LL9A8;case 4: _LL9A7: _tmpB15=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB13)->f1;_LL9A8:
 _tmpB16=_tmpB15;goto _LL9AA;case 3: _LL9A9: _tmpB16=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpB13)->f1;_LL9AA:
 _tmpB17=_tmpB16;goto _LL9AC;case 1: _LL9AB: _tmpB17=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB13)->f1;_LL9AC:
# 5721
 if(!_tmpB17->escapes)return _tmpB17;
goto _LL9A4;default: _LL9AD: _LL9AE:
 goto _LL9A4;}_LL9A4:;}
# 5725
return 0;}
# 5729
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpB18=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmpB18=({struct Cyc_List_List*_tmpB19=_cycalloc(sizeof(*_tmpB19));_tmpB19->hd=*((void**)_check_null((void**)x->hd));_tmpB19->tl=_tmpB18;_tmpB19;});}}
return _tmpB18;}
# 5736
int Cyc_Tcutil_is_array(void*t){
void*_tmpB1A=Cyc_Tcutil_compress(t);void*_tmpB1B=_tmpB1A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB1B)->tag == 8){_LL9B0: _LL9B1:
 return 1;}else{_LL9B2: _LL9B3:
 return 0;}_LL9AF:;}
# 5743
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmpB1C=Cyc_Tcutil_compress(t);void*_tmpB1D=_tmpB1C;void*_tmpB2D;struct Cyc_Absyn_Tqual _tmpB2C;struct Cyc_Absyn_Exp*_tmpB2B;union Cyc_Absyn_Constraint*_tmpB2A;unsigned int _tmpB29;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB1D)->tag == 8){_LL9B5: _tmpB2D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB1D)->f1).elt_type;_tmpB2C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB1D)->f1).tq;_tmpB2B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB1D)->f1).num_elts;_tmpB2A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB1D)->f1).zero_term;_tmpB29=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB1D)->f1).zt_loc;_LL9B6: {
# 5746
void*b;
if(_tmpB2B == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5750
struct Cyc_Absyn_Exp*bnd=_tmpB2B;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpB1E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB1F="cannot convert tag without type!";_tag_dyneither(_tmpB1F,sizeof(char),33);}),_tag_dyneither(_tmpB1E,sizeof(void*),0));});{
void*_tmpB20=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpB21=_tmpB20;void*_tmpB26;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB21)->tag == 19){_LL9BA: _tmpB26=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB21)->f1;_LL9BB:
# 5756
 b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB22=_cycalloc(sizeof(*_tmpB22));_tmpB22[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB23;_tmpB23.tag=1;_tmpB23.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpB26,0),0,Cyc_Absyn_No_coercion,0);_tmpB23;});_tmpB22;});
goto _LL9B9;}else{_LL9BC: _LL9BD:
# 5759
 if(Cyc_Tcutil_is_const_exp(bnd))
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB24=_cycalloc(sizeof(*_tmpB24));_tmpB24[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB25;_tmpB25.tag=1;_tmpB25.f1=bnd;_tmpB25;});_tmpB24;});else{
# 5762
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL9B9:;};}else{
# 5766
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB27=_cycalloc(sizeof(*_tmpB27));_tmpB27[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB28;_tmpB28.tag=1;_tmpB28.f1=bnd;_tmpB28;});_tmpB27;});}}
# 5768
return Cyc_Absyn_atb_typ(_tmpB2D,rgn,_tmpB2C,b,_tmpB2A);}}else{_LL9B7: _LL9B8:
# 5771
 return t;}_LL9B4:;}
# 5776
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpB2E=Cyc_Tcutil_compress(t);void*_tmpB2F=_tmpB2E;struct Cyc_List_List*_tmpB3D;union Cyc_Absyn_AggrInfoU _tmpB3C;struct Cyc_List_List*_tmpB3B;struct Cyc_List_List*_tmpB3A;void*_tmpB39;union Cyc_Absyn_Constraint*_tmpB38;switch(*((int*)_tmpB2F)){case 0: _LL9BF: _LL9C0:
 return 1;case 1: _LL9C1: _LL9C2:
 goto _LL9C4;case 2: _LL9C3: _LL9C4:
 goto _LL9C6;case 3: _LL9C5: _LL9C6:
 goto _LL9C8;case 4: _LL9C7: _LL9C8:
 return 0;case 5: _LL9C9: _tmpB38=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB2F)->f1).ptr_atts).nullable;_LL9CA:
# 5784
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB38);case 6: _LL9CB: _LL9CC:
 goto _LL9CE;case 7: _LL9CD: _LL9CE:
 return 1;case 8: _LL9CF: _tmpB39=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB2F)->f1).elt_type;_LL9D0:
 return Cyc_Tcutil_zeroable_type(_tmpB39);case 9: _LL9D1: _LL9D2:
 return 0;case 10: _LL9D3: _tmpB3A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB2F)->f1;_LL9D4:
# 5790
 for(0;(unsigned int)_tmpB3A;_tmpB3A=_tmpB3A->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpB3A->hd)).f2))return 0;}
return 1;case 11: _LL9D5: _tmpB3C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB2F)->f1).aggr_info;_tmpB3B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB2F)->f1).targs;_LL9D6: {
# 5794
struct Cyc_Absyn_Aggrdecl*_tmpB30=Cyc_Absyn_get_known_aggrdecl(_tmpB3C);
if(_tmpB30->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB30->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpB31=_new_region("r");struct _RegionHandle*r=& _tmpB31;_push_region(r);
{struct Cyc_List_List*_tmpB32=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpB30->tvs,_tmpB3B);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB30->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpB32,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB33=0;_npop_handler(0);return _tmpB33;}}}{
int _tmpB34=1;_npop_handler(0);return _tmpB34;};}
# 5798
;_pop_region(r);};}case 13: _LL9D7: _LL9D8:
# 5802
 goto _LL9DA;case 19: _LL9D9: _LL9DA:
 return 1;case 12: _LL9DB: _tmpB3D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB2F)->f2;_LL9DC:
# 5805
 for(0;_tmpB3D != 0;_tmpB3D=_tmpB3D->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB3D->hd)->type))return 0;}
return 1;case 14: _LL9DD: _LL9DE:
 return 1;case 17: _LL9DF: _LL9E0:
 goto _LL9E2;case 16: _LL9E1: _LL9E2:
 goto _LL9E4;case 15: _LL9E3: _LL9E4:
 goto _LL9E6;case 28: _LL9E5: _LL9E6:
 return 0;case 26: _LL9E7: _LL9E8:
# 5814
 goto _LL9EA;case 27: _LL9E9: _LL9EA:
 goto _LL9EC;case 18: _LL9EB: _LL9EC:
 goto _LL9EE;case 20: _LL9ED: _LL9EE:
 goto _LL9F0;case 21: _LL9EF: _LL9F0:
 goto _LL9F2;case 22: _LL9F1: _LL9F2:
 goto _LL9F4;case 23: _LL9F3: _LL9F4:
 goto _LL9F6;case 24: _LL9F5: _LL9F6:
 goto _LL9F8;default: _LL9F7: _LL9F8:
({struct Cyc_String_pa_PrintArg_struct _tmpB37;_tmpB37.tag=0;_tmpB37.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB35[1]={& _tmpB37};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB36="bad type `%s' in zeroable type";_tag_dyneither(_tmpB36,sizeof(char),31);}),_tag_dyneither(_tmpB35,sizeof(void*),1));});});}_LL9BE:;}
