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
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
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
# 525
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 920
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 928
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 931
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 935
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
# 938
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 940
void*Cyc_Absyn_conref_constr(void*y,union Cyc_Absyn_Constraint*x);
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_bounds_one_conref;
# 946
void*Cyc_Absyn_compress_kb(void*);
# 951
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 956
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 958
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 962
extern void*Cyc_Absyn_empty_effect;
# 972
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 981
extern void*Cyc_Absyn_bounds_one;
# 986
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1022
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1029
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1057
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
# 1069
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1115
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1162
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1164
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1166
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1168
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1170
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1174
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1176
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
void Cyc_Warn_vwarn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 27
void Cyc_Warn_warn(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 29
void Cyc_Warn_flush_warnings();
# 31
void Cyc_Warn_verr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Warn_err(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 36
void*Cyc_Warn_vimpos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 38
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
void Cyc_RgnOrder_print_region_po(struct Cyc_RgnOrder_RgnPO*po);extern char Cyc_Tcenv_Env_error[10];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};
# 48 "tcenv.h"
typedef struct Cyc_Tcenv_Genv*Cyc_Tcenv_genv_t;struct Cyc_Tcenv_Fenv;
# 52
typedef struct Cyc_Tcenv_Fenv*Cyc_Tcenv_fenv_t;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof;int in_extern_c_include;};
# 62
typedef struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tenv_t;
# 72
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args);
# 77
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 80
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 86
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 144
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
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
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
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
int Cyc_Tcutil_is_arithmetic_type(void*);
# 67
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 69
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 71
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 75
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 77
extern int Cyc_Tcutil_warn_alias_coerce;
# 80
extern int Cyc_Tcutil_warn_region_coerce;
# 83
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 94
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 101
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 106
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 111
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 118
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 123
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 128
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 132
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 137
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 139
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 143
void Cyc_Tcutil_explain_failure();
# 145
int Cyc_Tcutil_unify(void*,void*);
# 147
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 150
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 155
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 159
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 163
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 166
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 170
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 172
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 176
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 182
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 209 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 211
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 219
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 222
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 229
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 231
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 233
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 235
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 237
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 243
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 245
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 247
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 257
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 262
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 265
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 270
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
# 279
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 287
void*Cyc_Tcutil_normalize_effect(void*e);
# 290
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 292
int Cyc_Tcutil_new_tvar_id();
# 294
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 297
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 299
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 302
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 306
int Cyc_Tcutil_bits_only(void*t);
# 309
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 312
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 315
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 319
int Cyc_Tcutil_supports_default(void*);
# 323
int Cyc_Tcutil_admits_zero(void*t);
# 326
int Cyc_Tcutil_is_noreturn(void*);
# 330
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 334
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 338
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 341
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 344
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 348
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 351
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 354
int Cyc_Tcutil_is_array(void*t);
# 358
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 361
int Cyc_Tcutil_zeroable_type(void*t);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 35
typedef struct Cyc_Tcexp_TestEnv Cyc_Tcexp_testenv_t;
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 40 "tc.h"
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
# 40 "tcutil.cyc"
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 42
void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 46
int Cyc_Tcutil_warn_region_coerce=0;
# 49
void*Cyc_Tcutil_t1_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq1_const=0;
void*Cyc_Tcutil_t2_failure=(void*)& Cyc_Absyn_VoidType_val;
int Cyc_Tcutil_tq2_const=0;
# 54
struct _dyneither_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 58
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 63
if(Cyc_strcmp(({const char*_tmp1="(qualifiers don't match)";_tag_dyneither(_tmp1,sizeof(char),25);}),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp2[1]={& _tmp4};Cyc_fprintf(Cyc_stderr,({const char*_tmp3="  %s\n";_tag_dyneither(_tmp3,sizeof(char),6);}),_tag_dyneither(_tmp2,sizeof(void*),1));});});
return;}
# 68
if(Cyc_strcmp(({const char*_tmp5="(function effects do not match)";_tag_dyneither(_tmp5,sizeof(char),32);}),(struct _dyneither_ptr)Cyc_Tcutil_failure_reason)== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp7[1]={& _tmp9};Cyc_fprintf(Cyc_stderr,({const char*_tmp8="  %s";_tag_dyneither(_tmp8,sizeof(char),5);}),_tag_dyneither(_tmp7,sizeof(void*),1));});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpA=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpB="\n\t";_tag_dyneither(_tmpB,sizeof(char),3);}),_tag_dyneither(_tmpA,sizeof(void*),0));});
pos=8;}else{
# 82
({void*_tmpC=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpD=" ";_tag_dyneither(_tmpD,sizeof(char),2);}),_tag_dyneither(_tmpC,sizeof(void*),0));});
++ pos;}
# 85
({void*_tmpE=0;Cyc_fprintf(Cyc_stderr,({const char*_tmpF="and ";_tag_dyneither(_tmpF,sizeof(char),5);}),_tag_dyneither(_tmpE,sizeof(void*),0));});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmp10=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp11="\n\t";_tag_dyneither(_tmp11,sizeof(char),3);}),_tag_dyneither(_tmp10,sizeof(void*),0));});
pos=8;}
# 91
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({void*_tmp12[1]={& _tmp14};Cyc_fprintf(Cyc_stderr,({const char*_tmp13="%s ";_tag_dyneither(_tmp13,sizeof(char),4);}),_tag_dyneither(_tmp12,sizeof(void*),1));});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp15=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp16="\n\t";_tag_dyneither(_tmp16,sizeof(char),3);}),_tag_dyneither(_tmp15,sizeof(void*),0));});
pos=8;}
# 97
({void*_tmp17=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp18="are not compatible. ";_tag_dyneither(_tmp18,sizeof(char),21);}),_tag_dyneither(_tmp17,sizeof(void*),0));});
pos +=17;
if((char*)Cyc_Tcutil_failure_reason.curr != (char*)(_tag_dyneither(0,0,0)).curr){
if(pos + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason)>= 80)
({void*_tmp19=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1A="\n\t";_tag_dyneither(_tmp1A,sizeof(char),3);}),_tag_dyneither(_tmp19,sizeof(void*),0));});
# 103
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp1B[1]={& _tmp1D};Cyc_fprintf(Cyc_stderr,({const char*_tmp1C="%s";_tag_dyneither(_tmp1C,sizeof(char),3);}),_tag_dyneither(_tmp1B,sizeof(void*),1));});});}
# 105
({void*_tmp1E=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp1F="\n";_tag_dyneither(_tmp1F,sizeof(char),2);}),_tag_dyneither(_tmp1E,sizeof(void*),0));});
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 111
Cyc_Position_post_error(Cyc_Position_mk_err(loc,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap)));}
# 113
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 116
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp20[1]={& _tmp22};Cyc_fprintf(Cyc_stderr,({const char*_tmp21="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp21,sizeof(char),36);}),_tag_dyneither(_tmp20,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));_tmp23[0]=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;_tmp24.f1=msg;_tmp24;});_tmp23;}));}
# 121
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 123
Cyc_Warn_vwarn(sg,fmt,ap);}
# 127
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 132
void*Cyc_Tcutil_compress(void*t){
void*_tmp25=t;void*_tmp2E;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;void**_tmp2B;void**_tmp2A;switch(*((int*)_tmp25)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp2A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25)->f2;_LL8: {
# 143
void*ta=(void*)_check_null(*_tmp2A);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp2A=t2;
return t2;}}case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp25)->f4 == 0){_LL3: _LL4:
# 135
 return t;}else{_LL5: _tmp2B=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp25)->f4;_LL6: {
# 137
void*ta=(void*)_check_null(*_tmp2B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp2B=t2;
return t2;}}case 18: _LL9: _tmp2C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp25)->f1;_LLA:
# 149
 Cyc_Evexp_eval_const_uint_exp(_tmp2C);{
void*_tmp26=_tmp2C->r;void*_tmp27=_tmp26;void*_tmp28;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp27)->tag == 38){_LL12: _tmp28=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp27)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp28);}else{_LL14: _LL15:
 return t;}_LL11:;};case 27: _LLB: _tmp2D=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp25)->f1;_LLC: {
# 155
void*_tmp29=_tmp2D->topt;
if(_tmp29 != 0)return _tmp29;else{
return t;}}case 26: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp25)->f2 != 0){_LLD: _tmp2E=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp25)->f2;_LLE:
# 159
 return Cyc_Tcutil_compress(_tmp2E);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 167
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 171
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp2F=cptr;union Cyc_Absyn_Constraint*_tmp31;void*_tmp30;switch((((union Cyc_Absyn_Constraint*)_tmp2F)->Eq_constr).tag){case 3: _LL17: _LL18:
 return Cyc_Absyn_empty_conref();case 1: _LL19: _tmp30=(void*)(_tmp2F->Eq_constr).val;_LL1A:
 return Cyc_Absyn_new_conref(_tmp30);default: _LL1B: _tmp31=(_tmp2F->Forward_constr).val;_LL1C:
 return Cyc_Tcutil_copy_conref(_tmp31);}_LL16:;}
# 178
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp32=Cyc_Absyn_compress_kb(kb);void*_tmp33=_tmp32;struct Cyc_Absyn_Kind*_tmp38;switch(*((int*)_tmp33)){case 1: _LL1E: _LL1F:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp35;_tmp35.tag=1;_tmp35.f1=0;_tmp35;});_tmp34;});case 2: _LL20: _tmp38=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp33)->f2;_LL21:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp37;_tmp37.tag=2;_tmp37.f1=0;_tmp37.f2=_tmp38;_tmp37;});_tmp36;});default: _LL22: _LL23:
 return kb;}_LL1D:;}
# 185
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 187
return({struct Cyc_Absyn_Tvar*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=tv->name;_tmp39->identity=- 1;_tmp39->kind=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp39;});}
# 189
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 191
struct _tuple10*_tmp3A=arg;struct _dyneither_ptr*_tmp3E;struct Cyc_Absyn_Tqual _tmp3D;void*_tmp3C;_LL25: _tmp3E=_tmp3A->f1;_tmp3D=_tmp3A->f2;_tmp3C=_tmp3A->f3;_LL26:;
return({struct _tuple10*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=_tmp3E;_tmp3B->f2=_tmp3D;_tmp3B->f3=Cyc_Tcutil_copy_type(_tmp3C);_tmp3B;});}
# 194
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp3F=arg;struct Cyc_Absyn_Tqual _tmp42;void*_tmp41;_LL28: _tmp42=_tmp3F->f1;_tmp41=_tmp3F->f2;_LL29:;
return({struct _tuple12*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->f1=_tmp42;_tmp40->f2=Cyc_Tcutil_copy_type(_tmp41);_tmp40;});}
# 198
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 200
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->name=f->name;_tmp43->tq=f->tq;_tmp43->type=Cyc_Tcutil_copy_type(f->type);_tmp43->width=f->width;_tmp43->attributes=f->attributes;_tmp43->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp43;});}
# 204
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp44=x;void*_tmp47;void*_tmp46;_LL2B: _tmp47=_tmp44->f1;_tmp46=_tmp44->f2;_LL2C:;
return({struct _tuple0*_tmp45=_cycalloc(sizeof(*_tmp45));_tmp45->f1=Cyc_Tcutil_copy_type(_tmp47);_tmp45->f2=Cyc_Tcutil_copy_type(_tmp46);_tmp45;});}
# 208
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->name=f->name;_tmp48->tag=f->tag;_tmp48->loc=f->loc;_tmp48;});}
# 211
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A;_tmp4A.tag=2;_tmp4A.f1=Cyc_Tcutil_copy_tvar(t);_tmp4A;});_tmp49;});}
# 215
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_Absyn_Datatypedecl*_tmpD2;struct Cyc_Absyn_Enumdecl*_tmpD1;struct Cyc_Absyn_Aggrdecl*_tmpD0;void*_tmpCF;struct Cyc_List_List*_tmpCE;void*_tmpCD;struct _tuple2*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_Absyn_Typedefdecl*_tmpCA;void*_tmpC9;void*_tmpC8;void*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;void*_tmpC4;struct Cyc_List_List*_tmpC3;struct _tuple2*_tmpC2;struct Cyc_Absyn_Enumdecl*_tmpC1;enum Cyc_Absyn_AggrKind _tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Aggrdecl**_tmpBE;struct Cyc_List_List*_tmpBD;enum Cyc_Absyn_AggrKind _tmpBC;struct _tuple2*_tmpBB;struct Cyc_Core_Opt*_tmpBA;struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;void*_tmpB6;struct Cyc_Absyn_Tqual _tmpB5;void*_tmpB4;struct Cyc_List_List*_tmpB3;int _tmpB2;struct Cyc_Absyn_VarargInfo*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_List_List*_tmpAB;void*_tmpAA;struct Cyc_Absyn_Tqual _tmpA9;struct Cyc_Absyn_Exp*_tmpA8;union Cyc_Absyn_Constraint*_tmpA7;unsigned int _tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;union Cyc_Absyn_Constraint*_tmpA2;union Cyc_Absyn_Constraint*_tmpA1;union Cyc_Absyn_Constraint*_tmpA0;struct Cyc_Absyn_PtrLoc*_tmp9F;union Cyc_Absyn_DatatypeFieldInfoU _tmp9E;struct Cyc_List_List*_tmp9D;union Cyc_Absyn_DatatypeInfoU _tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Tvar*_tmp9A;switch(*((int*)_tmp4C)){case 0: _LL2E: _LL2F:
 goto _LL31;case 1: _LL30: _LL31:
 return t;case 2: _LL32: _tmp9A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4C)->f1;_LL33:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4E;_tmp4E.tag=2;_tmp4E.f1=Cyc_Tcutil_copy_tvar(_tmp9A);_tmp4E;});_tmp4D;});case 3: _LL34: _tmp9C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).datatype_info;_tmp9B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL35:
# 221
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp50;_tmp50.tag=3;_tmp50.f1=({struct Cyc_Absyn_DatatypeInfo _tmp51;_tmp51.datatype_info=_tmp9C;_tmp51.targs=Cyc_Tcutil_copy_types(_tmp9B);_tmp51;});_tmp50;});_tmp4F;});case 4: _LL36: _tmp9E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).field_info;_tmp9D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL37:
# 223
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp53;_tmp53.tag=4;_tmp53.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp54;_tmp54.field_info=_tmp9E;_tmp54.targs=Cyc_Tcutil_copy_types(_tmp9D);_tmp54;});_tmp53;});_tmp52;});case 5: _LL38: _tmpA5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_typ;_tmpA4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_tq;_tmpA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).rgn;_tmpA2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_tmpA1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).bounds;_tmpA0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).zero_term;_tmp9F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).ptrloc;_LL39: {
# 225
void*_tmp55=Cyc_Tcutil_copy_type(_tmpA5);
void*_tmp56=Cyc_Tcutil_copy_type(_tmpA3);
union Cyc_Absyn_Constraint*_tmp57=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA2);
struct Cyc_Absyn_Tqual _tmp58=_tmpA4;
# 230
union Cyc_Absyn_Constraint*_tmp59=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA1);
union Cyc_Absyn_Constraint*_tmp5A=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA0);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp5C;_tmp5C.tag=5;_tmp5C.f1=({struct Cyc_Absyn_PtrInfo _tmp5D;_tmp5D.elt_typ=_tmp55;_tmp5D.elt_tq=_tmp58;_tmp5D.ptr_atts=({(_tmp5D.ptr_atts).rgn=_tmp56;(_tmp5D.ptr_atts).nullable=_tmp57;(_tmp5D.ptr_atts).bounds=_tmp59;(_tmp5D.ptr_atts).zero_term=_tmp5A;(_tmp5D.ptr_atts).ptrloc=_tmp9F;_tmp5D.ptr_atts;});_tmp5D;});_tmp5C;});_tmp5B;});}case 6: _LL3A: _LL3B:
 goto _LL3D;case 7: _LL3C: _LL3D:
 return t;case 8: _LL3E: _tmpAA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_tmpA9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).tq;_tmpA8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).num_elts;_tmpA7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zero_term;_tmpA6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zt_loc;_LL3F: {
# 236
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpA8);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp5F;_tmp5F.tag=8;_tmp5F.f1=({struct Cyc_Absyn_ArrayInfo _tmp60;_tmp60.elt_type=Cyc_Tcutil_copy_type(_tmpAA);_tmp60.tq=_tmpA9;_tmp60.num_elts=eopt2;_tmp60.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA7);_tmp60.zt_loc=_tmpA6;_tmp60;});_tmp5F;});_tmp5E;});}case 9: _LL40: _tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).tvars;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).effect;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_tqual;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_typ;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).args;_tmpB2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).c_varargs;_tmpB1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).cyc_varargs;_tmpB0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).rgn_po;_tmpAF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_tmpAE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_clause;_tmpAD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_relns;_tmpAC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_clause;_tmpAB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_relns;_LL41: {
# 240
struct Cyc_List_List*_tmp61=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpB7);
void*effopt2=_tmpB6 == 0?0: Cyc_Tcutil_copy_type(_tmpB6);
void*_tmp62=Cyc_Tcutil_copy_type(_tmpB4);
struct Cyc_List_List*_tmp63=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpB3);
int _tmp64=_tmpB2;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpB1 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpB1;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->name=cv->name;_tmp65->tq=cv->tq;_tmp65->type=Cyc_Tcutil_copy_type(cv->type);_tmp65->inject=cv->inject;_tmp65;});}{
# 251
struct Cyc_List_List*_tmp66=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpB0);
struct Cyc_List_List*_tmp67=_tmpAF;
struct Cyc_Absyn_Exp*_tmp68=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAE);
struct Cyc_List_List*_tmp69=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAD);
struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAC);
struct Cyc_List_List*_tmp6B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAB);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));_tmp6C[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6D;_tmp6D.tag=9;_tmp6D.f1=({struct Cyc_Absyn_FnInfo _tmp6E;_tmp6E.tvars=_tmp61;_tmp6E.effect=effopt2;_tmp6E.ret_tqual=_tmpB5;_tmp6E.ret_typ=_tmp62;_tmp6E.args=_tmp63;_tmp6E.c_varargs=_tmp64;_tmp6E.cyc_varargs=cyc_varargs2;_tmp6E.rgn_po=_tmp66;_tmp6E.attributes=_tmp67;_tmp6E.requires_clause=_tmp68;_tmp6E.requires_relns=_tmp69;_tmp6E.ensures_clause=_tmp6A;_tmp6E.ensures_relns=_tmp6B;_tmp6E;});_tmp6D;});_tmp6C;});};}case 10: _LL42: _tmpB8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4C)->f1;_LL43:
# 260
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));_tmp6F[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp70;_tmp70.tag=10;_tmp70.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpB8);_tmp70;});_tmp6F;});case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).tag == 1){_LL44: _tmpBC=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f1;_tmpBB=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f2;_tmpBA=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f3;_tmpB9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL45:
# 262
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp72;_tmp72.tag=11;_tmp72.f1=({struct Cyc_Absyn_AggrInfo _tmp73;_tmp73.aggr_info=Cyc_Absyn_UnknownAggr(_tmpBC,_tmpBB,_tmpBA);_tmp73.targs=Cyc_Tcutil_copy_types(_tmpB9);_tmp73;});_tmp72;});_tmp71;});}else{_LL46: _tmpBE=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).KnownAggr).val;_tmpBD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL47:
# 264
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));_tmp74[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp75;_tmp75.tag=11;_tmp75.f1=({struct Cyc_Absyn_AggrInfo _tmp76;_tmp76.aggr_info=Cyc_Absyn_KnownAggr(_tmpBE);_tmp76.targs=Cyc_Tcutil_copy_types(_tmpBD);_tmp76;});_tmp75;});_tmp74;});}case 12: _LL48: _tmpC0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f1;_tmpBF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f2;_LL49:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp78;_tmp78.tag=12;_tmp78.f1=_tmpC0;_tmp78.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpBF);_tmp78;});_tmp77;});case 13: _LL4A: _tmpC2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f2;_LL4B:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp7A;_tmp7A.tag=13;_tmp7A.f1=_tmpC2;_tmp7A.f2=_tmpC1;_tmp7A;});_tmp79;});case 14: _LL4C: _tmpC3=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4C)->f1;_LL4D:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B[0]=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp7C;_tmp7C.tag=14;_tmp7C.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpC3);_tmp7C;});_tmp7B;});case 19: _LL4E: _tmpC4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->f1;_LL4F:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7E;_tmp7E.tag=19;_tmp7E.f1=Cyc_Tcutil_copy_type(_tmpC4);_tmp7E;});_tmp7D;});case 18: _LL50: _tmpC5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4C)->f1;_LL51:
# 271
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp80;_tmp80.tag=18;_tmp80.f1=_tmpC5;_tmp80;});_tmp7F;});case 27: _LL52: _tmpC6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp4C)->f1;_LL53:
# 274
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp82;_tmp82.tag=27;_tmp82.f1=_tmpC6;_tmp82;});_tmp81;});case 15: _LL54: _tmpC7=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4C)->f1;_LL55:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp84;_tmp84.tag=15;_tmp84.f1=Cyc_Tcutil_copy_type(_tmpC7);_tmp84;});_tmp83;});case 16: _LL56: _tmpC9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f2;_LL57:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp86;_tmp86.tag=16;_tmp86.f1=Cyc_Tcutil_copy_type(_tmpC9);_tmp86.f2=Cyc_Tcutil_copy_type(_tmpC8);_tmp86;});_tmp85;});case 17: _LL58: _tmpCC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f1;_tmpCB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f2;_tmpCA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f3;_LL59:
# 278
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp88;_tmp88.tag=17;_tmp88.f1=_tmpCC;_tmp88.f2=Cyc_Tcutil_copy_types(_tmpCB);_tmp88.f3=_tmpCA;_tmp88.f4=0;_tmp88;});_tmp87;});case 21: _LL5A: _LL5B:
 goto _LL5D;case 22: _LL5C: _LL5D:
 goto _LL5F;case 28: _LL5E: _LL5F:
 goto _LL61;case 20: _LL60: _LL61:
 return t;case 23: _LL62: _tmpCD=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4C)->f1;_LL63:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8A;_tmp8A.tag=23;_tmp8A.f1=Cyc_Tcutil_copy_type(_tmpCD);_tmp8A;});_tmp89;});case 24: _LL64: _tmpCE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4C)->f1;_LL65:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8C;_tmp8C.tag=24;_tmp8C.f1=Cyc_Tcutil_copy_types(_tmpCE);_tmp8C;});_tmp8B;});case 25: _LL66: _tmpCF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4C)->f1;_LL67:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8E;_tmp8E.tag=25;_tmp8E.f1=Cyc_Tcutil_copy_type(_tmpCF);_tmp8E;});_tmp8D;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)){case 0: _LL68: _tmpD0=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL69: {
# 288
struct Cyc_List_List*_tmp8F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD0->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp91;_tmp91.tag=11;_tmp91.f1=({struct Cyc_Absyn_AggrInfo _tmp92;_tmp92.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD0->kind,_tmpD0->name,0);_tmp92.targs=_tmp8F;_tmp92;});_tmp91;});_tmp90;});}case 1: _LL6A: _tmpD1=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL6B:
# 291
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp94;_tmp94.tag=13;_tmp94.f1=_tmpD1->name;_tmp94.f2=0;_tmp94;});_tmp93;});default: _LL6C: _tmpD2=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL6D: {
# 293
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD2->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp97;_tmp97.tag=3;_tmp97.f1=({struct Cyc_Absyn_DatatypeInfo _tmp98;_tmp98.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp99;_tmp99.name=_tmpD2->name;_tmp99.is_extensible=0;_tmp99;}));_tmp98.targs=_tmp95;_tmp98;});_tmp97;});_tmp96;});}}}_LL2D:;}
# 308 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD3=d;struct _dyneither_ptr*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->tag == 0){_LL6F: _tmpD6=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->f1;_LL70:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4[0]=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD5;_tmpD5.tag=0;_tmpD5.f1=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpD6);_tmpD5;});_tmpD4;});}else{_LL71: _tmpD7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD3)->f1;_LL72:
 return d;}_LL6E:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 314
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 316
return({struct _tuple19*_tmpD8=_cycalloc(sizeof(*_tmpD8));_tmpD8->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpD8->f2=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpD8;});}
# 319
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpD9=preserve_types;
{void*_tmpDA=e->r;void*_tmpDB=_tmpDA;int _tmp167;struct _dyneither_ptr _tmp166;void*_tmp165;struct Cyc_Absyn_Exp*_tmp164;struct _dyneither_ptr*_tmp163;struct Cyc_Core_Opt*_tmp162;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;int _tmp15E;struct Cyc_Absyn_Exp*_tmp15D;void**_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;int _tmp15A;int _tmp159;void*_tmp158;struct Cyc_Absyn_Enumfield*_tmp157;struct Cyc_Absyn_Enumdecl*_tmp156;struct Cyc_Absyn_Enumfield*_tmp155;struct Cyc_List_List*_tmp154;struct Cyc_Absyn_Datatypedecl*_tmp153;struct Cyc_Absyn_Datatypefield*_tmp152;void*_tmp151;struct Cyc_List_List*_tmp150;struct _tuple2*_tmp14F;struct Cyc_List_List*_tmp14E;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Aggrdecl*_tmp14C;struct Cyc_Absyn_Exp*_tmp14B;void*_tmp14A;int _tmp149;struct Cyc_Absyn_Vardecl*_tmp148;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp146;int _tmp145;struct Cyc_List_List*_tmp144;struct _dyneither_ptr*_tmp143;struct Cyc_Absyn_Tqual _tmp142;void*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct _dyneither_ptr*_tmp13B;int _tmp13A;int _tmp139;struct Cyc_Absyn_Exp*_tmp138;struct _dyneither_ptr*_tmp137;int _tmp136;int _tmp135;struct Cyc_Absyn_Exp*_tmp134;void*_tmp133;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_Exp*_tmp131;void*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;void*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;int _tmp12A;enum Cyc_Absyn_Coercion _tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;int _tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_List_List*_tmp122;struct Cyc_Absyn_VarargCallInfo*_tmp121;int _tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;enum Cyc_Absyn_Incrementor _tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Core_Opt*_tmp113;struct Cyc_Absyn_Exp*_tmp112;enum Cyc_Absyn_Primop _tmp111;struct Cyc_List_List*_tmp110;void*_tmp10F;union Cyc_Absyn_Cnst _tmp10E;switch(*((int*)_tmpDB)){case 0: _LL74: _tmp10E=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL75:
 new_e=Cyc_Absyn_const_exp(_tmp10E,e->loc);goto _LL73;case 1: _LL76: _tmp10F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL77:
 new_e=Cyc_Absyn_varb_exp(_tmp10F,e->loc);goto _LL73;case 2: _LL78: _tmp111=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp110=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL79:
 new_e=Cyc_Absyn_primop_exp(_tmp111,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp110),e->loc);goto _LL73;case 3: _LL7A: _tmp114=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp113=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp112=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL7B:
# 327
 new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp114),(unsigned int)_tmp113?({struct Cyc_Core_Opt*_tmpDC=_cycalloc_atomic(sizeof(*_tmpDC));_tmpDC->v=(void*)_tmp113->v;_tmpDC;}): 0,Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp112),e->loc);
goto _LL73;case 4: _LL7C: _tmp116=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp115=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL7D:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp116),_tmp115,e->loc);goto _LL73;case 5: _LL7E: _tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp118=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp117=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL7F:
# 331
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp119),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp118),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp117),e->loc);goto _LL73;case 6: _LL80: _tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL81:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11B),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11A),e->loc);goto _LL73;case 7: _LL82: _tmp11D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL83:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11D),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11C),e->loc);goto _LL73;case 8: _LL84: _tmp11F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL85:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11E),e->loc);goto _LL73;case 9: _LL86: _tmp123=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp122=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp121=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp120=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL87:
# 336
{struct Cyc_Absyn_VarargCallInfo*_tmpDD=_tmp121;int _tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_VarargInfo*_tmpE9;if(_tmpDD != 0){_LLC5: _tmpEB=_tmpDD->num_varargs;_tmpEA=_tmpDD->injectors;_tmpE9=_tmpDD->vai;_LLC6: {
# 338
struct Cyc_Absyn_VarargInfo*_tmpDE=_tmpE9;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Tqual _tmpE5;void*_tmpE4;int _tmpE3;_LLCA: _tmpE6=_tmpDE->name;_tmpE5=_tmpDE->tq;_tmpE4=_tmpDE->type;_tmpE3=_tmpDE->inject;_LLCB:;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE0;_tmpE0.tag=9;_tmpE0.f1=
Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp123);_tmpE0.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp122);_tmpE0.f3=({struct Cyc_Absyn_VarargCallInfo*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->num_varargs=_tmpEB;_tmpE1->injectors=_tmpEA;_tmpE1->vai=({struct Cyc_Absyn_VarargInfo*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->name=_tmpE6;_tmpE2->tq=_tmpE5;_tmpE2->type=
# 342
Cyc_Tcutil_copy_type(_tmpE4);_tmpE2->inject=_tmpE3;_tmpE2;});_tmpE1;});_tmpE0.f4=_tmp120;_tmpE0;});_tmpDF;}),e->loc);
# 344
goto _LLC4;}}else{_LLC7: _LLC8:
# 346
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE8;_tmpE8.tag=9;_tmpE8.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp123);_tmpE8.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp122);_tmpE8.f3=_tmp121;_tmpE8.f4=_tmp120;_tmpE8;});_tmpE7;}),e->loc);
goto _LLC4;}_LLC4:;}
# 349
goto _LL73;case 10: _LL88: _tmp125=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp124=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL89:
# 351
 new_e=_tmp124?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp125),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp125),e->loc);
goto _LL73;case 11: _LL8A: _tmp126=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL8B:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp126),e->loc);
goto _LL73;case 12: _LL8C: _tmp128=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp127=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL8D:
# 356
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp128),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp127),e->loc);
goto _LL73;case 13: _LL8E: _tmp12C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp12A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp129=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL8F:
# 359
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp12C),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12B),_tmp12A,_tmp129,e->loc);goto _LL73;case 14: _LL90: _tmp12D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL91:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12D),e->loc);goto _LL73;case 15: _LL92: _tmp12F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL93: {
# 362
struct Cyc_Absyn_Exp*eo1=_tmp12F;if(_tmp12F != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12F);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12E),e->loc);
goto _LL73;}case 16: _LL94: _tmp130=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL95:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp130),e->loc);
goto _LL73;case 17: _LL96: _tmp131=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL97:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp131),e->loc);goto _LL73;case 18: _LL98: _tmp133=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp132=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL99:
# 369
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp133),_tmp132,e->loc);goto _LL73;case 19: _LL9A: _tmp134=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL9B:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp134),e->loc);goto _LL73;case 20: _LL9C: _tmp138=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp137=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp136=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp135=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL9D:
# 372
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpED;_tmpED.tag=20;_tmpED.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp138);_tmpED.f2=_tmp137;_tmpED.f3=_tmp136;_tmpED.f4=_tmp135;_tmpED;});_tmpEC;}),e->loc);goto _LL73;case 21: _LL9E: _tmp13C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp13A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp139=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL9F:
# 374
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpEF;_tmpEF.tag=21;_tmpEF.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13C);_tmpEF.f2=_tmp13B;_tmpEF.f3=_tmp13A;_tmpEF.f4=_tmp139;_tmpEF;});_tmpEE;}),e->loc);goto _LL73;case 22: _LLA0: _tmp13E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLA1:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13E),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13D),e->loc);
goto _LL73;case 23: _LLA2: _tmp13F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLA3:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp13F),e->loc);goto _LL73;case 24: _LLA4: _tmp143=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f1;_tmp142=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f2;_tmp141=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f3;_tmp140=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLA5: {
# 379
struct _dyneither_ptr*vopt1=_tmp143;
if(_tmp143 != 0)vopt1=_tmp143;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF1;_tmpF1.tag=24;_tmpF1.f1=({struct _tuple10*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=vopt1;_tmpF2->f2=_tmp142;_tmpF2->f3=Cyc_Tcutil_copy_type(_tmp141);_tmpF2;});_tmpF1.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp140);_tmpF1;});_tmpF0;}),e->loc);
goto _LL73;}case 25: _LLA6: _tmp144=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLA7:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF4;_tmpF4.tag=25;_tmpF4.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp144);_tmpF4;});_tmpF3;}),e->loc);
goto _LL73;case 26: _LLA8: _tmp148=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp147=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp146=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp145=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LLA9:
# 387
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));_tmpF5[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF6;_tmpF6.tag=26;_tmpF6.f1=_tmp148;_tmpF6.f2=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp147);_tmpF6.f3=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp146);_tmpF6.f4=_tmp145;_tmpF6;});_tmpF5;}),e->loc);
goto _LL73;case 27: _LLAA: _tmp14B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14A=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp149=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLAB:
# 390
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF8;_tmpF8.tag=27;_tmpF8.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp14B);_tmpF8.f2=Cyc_Tcutil_copy_type(_tmp14A);_tmpF8.f3=_tmp149;_tmpF8;});_tmpF7;}),_tmp14B->loc);
# 392
goto _LL73;case 28: _LLAC: _tmp14F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp14D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp14C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LLAD:
# 394
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpFA;_tmpFA.tag=28;_tmpFA.f1=_tmp14F;_tmpFA.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp14E);_tmpFA.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp14D);_tmpFA.f4=_tmp14C;_tmpFA;});_tmpF9;}),e->loc);
# 396
goto _LL73;case 29: _LLAE: _tmp151=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp150=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLAF:
# 398
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpFC;_tmpFC.tag=29;_tmpFC.f1=Cyc_Tcutil_copy_type(_tmp151);_tmpFC.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp150);_tmpFC;});_tmpFB;}),e->loc);
goto _LL73;case 30: _LLB0: _tmp154=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp153=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp152=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLB1:
# 401
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpFE;_tmpFE.tag=30;_tmpFE.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp154);_tmpFE.f2=_tmp153;_tmpFE.f3=_tmp152;_tmpFE;});_tmpFD;}),e->loc);
goto _LL73;case 31: _LLB2: _tmp156=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp155=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB3:
 new_e=e;goto _LL73;case 32: _LLB4: _tmp158=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp157=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB5:
# 405
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp100;_tmp100.tag=32;_tmp100.f1=Cyc_Tcutil_copy_type(_tmp158);_tmp100.f2=_tmp157;_tmp100;});_tmpFF;}),e->loc);
goto _LL73;case 33: _LLB6: _tmp15E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).is_calloc;_tmp15D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).rgn;_tmp15C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).elt_type;_tmp15B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).num_elts;_tmp15A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).fat_result;_tmp159=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).inline_call;_LLB7: {
# 408
struct Cyc_Absyn_Exp*_tmp101=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp15D;if(_tmp15D != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp15D);{
void**t1=_tmp15C;if(_tmp15C != 0)t1=({void**_tmp102=_cycalloc(sizeof(*_tmp102));_tmp102[0]=Cyc_Tcutil_copy_type(*_tmp15C);_tmp102;});
_tmp101->r=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp104;_tmp104.tag=33;_tmp104.f1=({struct Cyc_Absyn_MallocInfo _tmp105;_tmp105.is_calloc=_tmp15E;_tmp105.rgn=r1;_tmp105.elt_type=t1;_tmp105.num_elts=_tmp15B;_tmp105.fat_result=_tmp15A;_tmp105.inline_call=_tmp159;_tmp105;});_tmp104;});_tmp103;});
new_e=_tmp101;
goto _LL73;};}case 34: _LLB8: _tmp160=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp15F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB9:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp160),Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp15F),e->loc);goto _LL73;case 35: _LLBA: _tmp162=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp161=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLBB: {
# 416
struct Cyc_Core_Opt*nopt1=_tmp162;
if(_tmp162 != 0)nopt1=({struct Cyc_Core_Opt*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->v=(struct _tuple2*)_tmp162->v;_tmp106;});
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));_tmp107[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=35;_tmp108.f1=nopt1;_tmp108.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp161);_tmp108;});_tmp107;}),e->loc);
goto _LL73;}case 36: _LLBC: _LLBD:
# 421
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Core_Failure_exn_struct _tmp10A;_tmp10A.tag=Cyc_Core_Failure;_tmp10A.f1=({const char*_tmp10B="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp10B,sizeof(char),45);});_tmp10A;});_tmp109;}));case 37: _LLBE: _tmp164=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp163=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLBF:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp10D;_tmp10D.tag=37;_tmp10D.f1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp164);_tmp10D.f2=_tmp163;_tmp10D;});_tmp10C;}),e->loc);
goto _LL73;case 38: _LLC0: _tmp165=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLC1:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp165),e->loc);
goto _LL73;default: _LLC2: _tmp167=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp166=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLC3:
 new_e=Cyc_Absyn_asm_exp(_tmp167,_tmp166,e->loc);goto _LL73;}_LL73:;}
# 429
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 433
return new_e;}
# 436
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 449 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp168=ka1;enum Cyc_Absyn_KindQual _tmp173;enum Cyc_Absyn_AliasQual _tmp172;_LLCD: _tmp173=_tmp168->kind;_tmp172=_tmp168->aliasqual;_LLCE:;{
struct Cyc_Absyn_Kind*_tmp169=ka2;enum Cyc_Absyn_KindQual _tmp171;enum Cyc_Absyn_AliasQual _tmp170;_LLD0: _tmp171=_tmp169->kind;_tmp170=_tmp169->aliasqual;_LLD1:;
# 453
if(_tmp173 != _tmp171){
struct _tuple20 _tmp16A=({struct _tuple20 _tmp16C;_tmp16C.f1=_tmp173;_tmp16C.f2=_tmp171;_tmp16C;});struct _tuple20 _tmp16B=_tmp16A;switch(_tmp16B.f1){case Cyc_Absyn_BoxKind: switch(_tmp16B.f2){case Cyc_Absyn_MemKind: _LLD3: _LLD4:
 goto _LLD6;case Cyc_Absyn_AnyKind: _LLD5: _LLD6:
 goto _LLD8;default: goto _LLD9;}case Cyc_Absyn_MemKind: if(_tmp16B.f2 == Cyc_Absyn_AnyKind){_LLD7: _LLD8:
 goto _LLD2;}else{goto _LLD9;}default: _LLD9: _LLDA:
 return 0;}_LLD2:;}
# 462
if(_tmp172 != _tmp170){
struct _tuple21 _tmp16D=({struct _tuple21 _tmp16F;_tmp16F.f1=_tmp172;_tmp16F.f2=_tmp170;_tmp16F;});struct _tuple21 _tmp16E=_tmp16D;switch(_tmp16E.f1){case Cyc_Absyn_Aliasable: if(_tmp16E.f2 == Cyc_Absyn_Top){_LLDC: _LLDD:
 goto _LLDF;}else{goto _LLE0;}case Cyc_Absyn_Unique: if(_tmp16E.f2 == Cyc_Absyn_Top){_LLDE: _LLDF:
 return 1;}else{goto _LLE0;}default: _LLE0: _LLE1:
 return 0;}_LLDB:;}
# 469
return 1;};}
# 472
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp174=Cyc_Absyn_compress_kb(tv->kind);void*_tmp175=_tmp174;struct Cyc_Absyn_Kind*_tmp179;struct Cyc_Absyn_Kind*_tmp178;switch(*((int*)_tmp175)){case 0: _LLE3: _tmp178=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp175)->f1;_LLE4:
 return _tmp178;case 2: _LLE5: _tmp179=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp175)->f2;_LLE6:
 return _tmp179;default: _LLE7: _LLE8:
# 477
 tv->kind=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp177;_tmp177.tag=2;_tmp177.f1=0;_tmp177.f2=def;_tmp177;});_tmp176;});
return def;}_LLE2:;}
# 482
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp17A=({struct _tuple0 _tmp191;_tmp191.f1=Cyc_Absyn_compress_kb(kb1);_tmp191.f2=Cyc_Absyn_compress_kb(kb2);_tmp191;});struct _tuple0 _tmp17B=_tmp17A;struct Cyc_Core_Opt**_tmp190;void*_tmp18F;void*_tmp18E;struct Cyc_Core_Opt**_tmp18D;struct Cyc_Core_Opt**_tmp18C;struct Cyc_Absyn_Kind*_tmp18B;struct Cyc_Core_Opt**_tmp18A;struct Cyc_Absyn_Kind*_tmp189;struct Cyc_Core_Opt**_tmp188;struct Cyc_Absyn_Kind*_tmp187;struct Cyc_Absyn_Kind*_tmp186;struct Cyc_Absyn_Kind*_tmp185;struct Cyc_Core_Opt**_tmp184;struct Cyc_Absyn_Kind*_tmp183;struct Cyc_Absyn_Kind*_tmp182;struct Cyc_Absyn_Kind*_tmp181;switch(*((int*)_tmp17B.f1)){case 0: switch(*((int*)_tmp17B.f2)){case 0: _LLEA: _tmp182=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp181=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLEB:
 return _tmp182 == _tmp181;case 2: _LLEE: _tmp185=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp184=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_tmp183=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f2;_LLEF:
# 491
 if(Cyc_Tcutil_kind_leq(_tmp185,_tmp183)){
*_tmp184=({struct Cyc_Core_Opt*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->v=kb1;_tmp17D;});
return 1;}else{
return 0;}default: goto _LLF4;}case 2: switch(*((int*)_tmp17B.f2)){case 0: _LLEC: _tmp188=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp187=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f2;_tmp186=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLED:
# 486
 if(Cyc_Tcutil_kind_leq(_tmp186,_tmp187)){
*_tmp188=({struct Cyc_Core_Opt*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->v=kb2;_tmp17C;});
return 1;}else{
return 0;}case 2: _LLF0: _tmp18C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp18B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f2;_tmp18A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_tmp189=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f2;_LLF1:
# 496
 if(Cyc_Tcutil_kind_leq(_tmp18B,_tmp189)){
*_tmp18A=({struct Cyc_Core_Opt*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->v=kb1;_tmp17E;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp189,_tmp18B)){
*_tmp18C=({struct Cyc_Core_Opt*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->v=kb2;_tmp17F;});
return 1;}else{
return 0;}}default: _LLF4: _tmp18E=_tmp17B.f1;_tmp18D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLF5:
# 505
*_tmp18D=({struct Cyc_Core_Opt*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=_tmp18E;_tmp180;});
return 1;}default: _LLF2: _tmp190=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp18F=_tmp17B.f2;_LLF3:
# 503
 _tmp18E=_tmp18F;_tmp18D=_tmp190;goto _LLF5;}_LLE9:;}
# 510
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp192=Cyc_Tcutil_compress(t);void*_tmp193=_tmp192;struct Cyc_Absyn_Tvar*_tmp199;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp193)->tag == 2){_LLF7: _tmp199=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp193)->f1;_LLF8: {
# 513
void*_tmp194=_tmp199->kind;
_tmp199->kind=kb;
return({struct _tuple16 _tmp195;_tmp195.f1=_tmp199;_tmp195.f2=_tmp194;_tmp195;});}}else{_LLF9: _LLFA:
# 517
({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0;_tmp198.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp196[1]={& _tmp198};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp197="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp197,sizeof(char),40);}),_tag_dyneither(_tmp196,sizeof(void*),1));});});}_LLF6:;}
# 523
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 525
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 529
struct _RegionHandle _tmp19A=_new_region("temp");struct _RegionHandle*temp=& _tmp19A;_push_region(temp);
{struct Cyc_List_List*_tmp19B=0;
# 532
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp19C=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp19D=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp19E=Cyc_Tcutil_tvar_kind(_tmp19C,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp19F=_tmp19E;switch(((struct Cyc_Absyn_Kind*)_tmp19F)->kind){case Cyc_Absyn_IntKind: _LLFC: _LLFD:
 goto _LLFF;case Cyc_Absyn_AnyKind: _LLFE: _LLFF:
# 538
 _tmp19B=({struct Cyc_List_List*_tmp1A0=_region_malloc(temp,sizeof(*_tmp1A0));_tmp1A0->hd=({struct _tuple16*_tmp1A1=_region_malloc(temp,sizeof(*_tmp1A1));_tmp1A1->f1=_tmp19C;_tmp1A1->f2=_tmp19D;_tmp1A1;});_tmp1A0->tl=_tmp19B;_tmp1A0;});goto _LLFB;default: _LL100: _LL101:
 goto _LLFB;}_LLFB:;}
# 542
if(_tmp19B != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp19B),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 530
;_pop_region(temp);}
# 547
return k;}
# 554
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1A2=Cyc_Tcutil_compress(t);void*_tmp1A3=_tmp1A2;struct Cyc_Absyn_Typedefdecl*_tmp1C1;struct Cyc_Absyn_Exp*_tmp1C0;struct Cyc_Absyn_Kind*_tmp1BF;struct Cyc_Absyn_PtrInfo _tmp1BE;enum Cyc_Absyn_AggrKind _tmp1BD;struct Cyc_List_List*_tmp1BC;struct Cyc_Absyn_AggrdeclImpl*_tmp1BB;int _tmp1BA;struct Cyc_List_List*_tmp1B9;struct Cyc_Absyn_Datatypedecl*_tmp1B8;struct Cyc_Absyn_Datatypefield*_tmp1B7;enum Cyc_Absyn_Size_of _tmp1B6;struct Cyc_Absyn_Tvar*_tmp1B5;struct Cyc_Core_Opt*_tmp1B4;switch(*((int*)_tmp1A3)){case 1: _LL103: _tmp1B4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A3)->f1;_LL104:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B4))->v;case 2: _LL105: _tmp1B5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A3)->f1;_LL106:
 return Cyc_Tcutil_tvar_kind(_tmp1B5,& Cyc_Tcutil_bk);case 0: _LL107: _LL108:
 return& Cyc_Tcutil_mk;case 6: _LL109: _tmp1B6=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1A3)->f2;_LL10A:
# 560
 return(_tmp1B6 == Cyc_Absyn_Int_sz  || _tmp1B6 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7: _LL10B: _LL10C:
# 562
 return& Cyc_Tcutil_mk;case 9: _LL10D: _LL10E:
 return& Cyc_Tcutil_ak;case 16: _LL10F: _LL110:
 goto _LL112;case 13: _LL111: _LL112:
 goto _LL114;case 14: _LL113: _LL114:
 goto _LL116;case 15: _LL115: _LL116:
 return& Cyc_Tcutil_bk;case 21: _LL117: _LL118:
 return& Cyc_Tcutil_urk;case 20: _LL119: _LL11A:
 return& Cyc_Tcutil_rk;case 22: _LL11B: _LL11C:
 return& Cyc_Tcutil_trk;case 3: _LL11D: _LL11E:
# 573
 return& Cyc_Tcutil_ak;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A3)->f1).field_info).KnownDatatypefield).tag == 2){_LL11F: _tmp1B8=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A3)->f1).field_info).KnownDatatypefield).val).f1;_tmp1B7=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A3)->f1).field_info).KnownDatatypefield).val).f2;_LL120:
# 575
 return& Cyc_Tcutil_mk;}else{_LL121: _LL122:
# 577
({void*_tmp1A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1A5="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1A5,sizeof(char),39);}),_tag_dyneither(_tmp1A4,sizeof(void*),0));});}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).UnknownAggr).tag == 1){_LL123: _LL124:
# 580
 return& Cyc_Tcutil_ak;}else{_LL125: _tmp1BD=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).KnownAggr).val)->kind;_tmp1BC=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1BB=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).KnownAggr).val)->impl;_tmp1BA=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1B9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).targs;_LL126:
# 582
 if(_tmp1BB == 0){
if(_tmp1BA)
return& Cyc_Tcutil_mk;else{
# 586
return& Cyc_Tcutil_ak;}}{
# 588
struct Cyc_List_List*_tmp1A6=_tmp1BB->fields;
if(_tmp1A6 == 0)return& Cyc_Tcutil_mk;
# 591
if(_tmp1BD == Cyc_Absyn_StructA){
for(0;_tmp1A6->tl != 0;_tmp1A6=_tmp1A6->tl){;}{
void*_tmp1A7=((struct Cyc_Absyn_Aggrfield*)_tmp1A6->hd)->type;
struct Cyc_Absyn_Kind*_tmp1A8=Cyc_Tcutil_field_kind(_tmp1A7,_tmp1B9,_tmp1BC);
if(_tmp1A8 == & Cyc_Tcutil_ak  || _tmp1A8 == & Cyc_Tcutil_tak)return _tmp1A8;};}else{
# 599
for(0;_tmp1A6 != 0;_tmp1A6=_tmp1A6->tl){
void*_tmp1A9=((struct Cyc_Absyn_Aggrfield*)_tmp1A6->hd)->type;
struct Cyc_Absyn_Kind*_tmp1AA=Cyc_Tcutil_field_kind(_tmp1A9,_tmp1B9,_tmp1BC);
if(_tmp1AA == & Cyc_Tcutil_ak  || _tmp1AA == & Cyc_Tcutil_tak)return _tmp1AA;}}
# 605
return& Cyc_Tcutil_mk;};}case 12: _LL127: _LL128:
 return& Cyc_Tcutil_mk;case 5: _LL129: _tmp1BE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A3)->f1;_LL12A: {
# 608
void*_tmp1AB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1BE.ptr_atts).bounds);void*_tmp1AC=_tmp1AB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1AC)->tag == 0){_LL146: _LL147: {
# 610
enum Cyc_Absyn_AliasQual _tmp1AD=(Cyc_Tcutil_typ_kind((_tmp1BE.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1AE=_tmp1AD;switch(_tmp1AE){case Cyc_Absyn_Aliasable: _LL14B: _LL14C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL14D: _LL14E:
 return& Cyc_Tcutil_umk;default: _LL14F: _LL150:
 return& Cyc_Tcutil_tmk;}_LL14A:;}}else{_LL148: _LL149: {
# 616
enum Cyc_Absyn_AliasQual _tmp1AF=(Cyc_Tcutil_typ_kind((_tmp1BE.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1B0=_tmp1AF;switch(_tmp1B0){case Cyc_Absyn_Aliasable: _LL152: _LL153:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL154: _LL155:
 return& Cyc_Tcutil_ubk;default: _LL156: _LL157:
 return& Cyc_Tcutil_tbk;}_LL151:;}}_LL145:;}case 18: _LL12B: _LL12C:
# 622
 return& Cyc_Tcutil_ik;case 28: _LL12D: _tmp1BF=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1A3)->f2;_LL12E:
 return _tmp1BF;case 27: _LL12F: _LL130:
# 627
 return& Cyc_Tcutil_ak;case 19: _LL131: _LL132:
 return& Cyc_Tcutil_bk;case 8: _LL133: _tmp1C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A3)->f1).num_elts;_LL134:
# 630
 if(_tmp1C0 == 0  || Cyc_Tcutil_is_const_exp(_tmp1C0))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10: _LL135: _LL136:
 return& Cyc_Tcutil_mk;case 17: _LL137: _tmp1C1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A3)->f3;_LL138:
# 634
 if(_tmp1C1 == 0  || _tmp1C1->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0;_tmp1B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1B1[1]={& _tmp1B3};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B2="typ_kind: typedef found: %s";_tag_dyneither(_tmp1B2,sizeof(char),28);}),_tag_dyneither(_tmp1B1,sizeof(void*),1));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C1->kind))->v;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A3)->f1)->r)){case 0: _LL139: _LL13A:
 return& Cyc_Tcutil_ak;case 1: _LL13B: _LL13C:
 return& Cyc_Tcutil_bk;default: _LL13D: _LL13E:
 return& Cyc_Tcutil_ak;}case 23: _LL13F: _LL140:
 goto _LL142;case 24: _LL141: _LL142:
 goto _LL144;default: _LL143: _LL144:
 return& Cyc_Tcutil_ek;}_LL102:;}
# 646
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 651
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1C2;_push_handler(& _tmp1C2);{int _tmp1C4=0;if(setjmp(_tmp1C2.handler))_tmp1C4=1;if(!_tmp1C4){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1C5=1;_npop_handler(0);return _tmp1C5;};
# 653
;_pop_handler();}else{void*_tmp1C3=(void*)_exn_thrown;void*_tmp1C6=_tmp1C3;void*_tmp1C7;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1C6)->tag == Cyc_Tcutil_Unify){_LL159: _LL15A:
# 655
 return 0;}else{_LL15B: _tmp1C7=_tmp1C6;_LL15C:(int)_rethrow(_tmp1C7);}_LL158:;}};}
# 660
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1C8=t;struct Cyc_List_List*_tmp1EE;void*_tmp1ED;void*_tmp1EC;void*_tmp1EB;void*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E3;void*_tmp1E2;struct Cyc_Absyn_Tqual _tmp1E1;void*_tmp1E0;struct Cyc_List_List*_tmp1DF;int _tmp1DE;struct Cyc_Absyn_VarargInfo*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_List_List*_tmp1D7;void*_tmp1D6;struct Cyc_Absyn_PtrInfo _tmp1D5;void*_tmp1D4;struct Cyc_Core_Opt**_tmp1D3;struct Cyc_Absyn_Tvar*_tmp1D2;switch(*((int*)_tmp1C8)){case 2: _LL15E: _tmp1D2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C8)->f1;_LL15F:
# 665
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1D2)){
Cyc_Tcutil_failure_reason=({const char*_tmp1C9="(type variable would escape scope)";_tag_dyneither(_tmp1C9,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 669
goto _LL15D;case 1: _LL160: _tmp1D4=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C8)->f2;_tmp1D3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C8)->f4;_LL161:
# 671
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1CA="(occurs check)";_tag_dyneither(_tmp1CA,sizeof(char),15);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 675
if(_tmp1D4 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1D4);else{
# 678
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D3))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 685
if(problem){
struct Cyc_List_List*_tmp1CB=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D3))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1CB=({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1CC->tl=_tmp1CB;_tmp1CC;});}}
# 691
*_tmp1D3=({struct Cyc_Core_Opt*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->v=_tmp1CB;_tmp1CD;});}}}
# 694
goto _LL15D;case 5: _LL162: _tmp1D5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C8)->f1;_LL163:
# 697
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D5.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1D5.ptr_atts).rgn);
goto _LL15D;case 8: _LL164: _tmp1D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C8)->f1).elt_type;_LL165:
# 701
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D6);goto _LL15D;case 9: _LL166: _tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).tvars;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).effect;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ret_tqual;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ret_typ;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).args;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).c_varargs;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).cyc_varargs;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).rgn_po;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).attributes;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_clause;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_relns;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_clause;_tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_relns;_LL167:
# 704
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
# 716
goto _LL15D;case 10: _LL168: _tmp1E4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1C8)->f1;_LL169:
# 718
 for(0;_tmp1E4 != 0;_tmp1E4=_tmp1E4->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1E4->hd)).f2);}
goto _LL15D;case 3: _LL16A: _tmp1E5=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C8)->f1).targs;_LL16B:
# 722
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E5);goto _LL15D;case 17: _LL16C: _tmp1E6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1C8)->f2;_LL16D:
 _tmp1E7=_tmp1E6;goto _LL16F;case 4: _LL16E: _tmp1E7=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1C8)->f1).targs;_LL16F:
 _tmp1E8=_tmp1E7;goto _LL171;case 11: _LL170: _tmp1E8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C8)->f1).targs;_LL171:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E8);goto _LL15D;case 12: _LL172: _tmp1E9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C8)->f2;_LL173:
# 728
 for(0;_tmp1E9 != 0;_tmp1E9=_tmp1E9->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1E9->hd)->type);}
goto _LL15D;case 19: _LL174: _tmp1EA=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1C8)->f1;_LL175:
 _tmp1EB=_tmp1EA;goto _LL177;case 23: _LL176: _tmp1EB=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1C8)->f1;_LL177:
 _tmp1EC=_tmp1EB;goto _LL179;case 15: _LL178: _tmp1EC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1C8)->f1;_LL179:
 _tmp1ED=_tmp1EC;goto _LL17B;case 25: _LL17A: _tmp1ED=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1C8)->f1;_LL17B:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1ED);goto _LL15D;case 24: _LL17C: _tmp1EE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1C8)->f1;_LL17D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1EE);goto _LL15D;default: _LL17E: _LL17F:
# 738
 goto _LL15D;}_LL15D:;};}
# 741
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 743
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 748
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 756
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1EF=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F0="tq1 real_const not set.";_tag_dyneither(_tmp1F0,sizeof(char),24);}),_tag_dyneither(_tmp1EF,sizeof(void*),0));});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1F1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F2="tq2 real_const not set.";_tag_dyneither(_tmp1F2,sizeof(char),24);}),_tag_dyneither(_tmp1F1,sizeof(void*),0));});
# 762
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 765
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp1F3="(qualifiers don't match)";_tag_dyneither(_tmp1F3,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 773
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 777
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 784
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 786
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1F4=x;void*_tmp1FC;switch((((union Cyc_Absyn_Constraint*)_tmp1F4)->Eq_constr).tag){case 3: _LL184: _LL185:
# 791
*x=({union Cyc_Absyn_Constraint _tmp1F5;(_tmp1F5.Forward_constr).val=y;(_tmp1F5.Forward_constr).tag=2;_tmp1F5;});return;case 1: _LL186: _tmp1FC=(void*)(_tmp1F4->Eq_constr).val;_LL187: {
# 793
union Cyc_Absyn_Constraint*_tmp1F6=y;void*_tmp1F9;switch((((union Cyc_Absyn_Constraint*)_tmp1F6)->Eq_constr).tag){case 3: _LL18B: _LL18C:
*y=*x;return;case 1: _LL18D: _tmp1F9=(void*)(_tmp1F6->Eq_constr).val;_LL18E:
# 796
 if(cmp(_tmp1FC,_tmp1F9)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 800
return;default: _LL18F: _LL190:
({void*_tmp1F7=0;Cyc_Tcutil_impos(({const char*_tmp1F8="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1F8,sizeof(char),40);}),_tag_dyneither(_tmp1F7,sizeof(void*),0));});}_LL18A:;}default: _LL188: _LL189:
# 803
({void*_tmp1FA=0;Cyc_Tcutil_impos(({const char*_tmp1FB="unify_conref: forward after compress";_tag_dyneither(_tmp1FB,sizeof(char),37);}),_tag_dyneither(_tmp1FA,sizeof(void*),0));});}_LL183:;};}
# 807
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp1FD;_push_handler(& _tmp1FD);{int _tmp1FF=0;if(setjmp(_tmp1FD.handler))_tmp1FF=1;if(!_tmp1FF){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp200=1;_npop_handler(0);return _tmp200;};
# 809
;_pop_handler();}else{void*_tmp1FE=(void*)_exn_thrown;void*_tmp201=_tmp1FE;void*_tmp202;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp201)->tag == Cyc_Tcutil_Unify){_LL192: _LL193:
# 811
 return 0;}else{_LL194: _tmp202=_tmp201;_LL195:(int)_rethrow(_tmp202);}_LL191:;}};}
# 814
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp203=({struct _tuple0 _tmp207;_tmp207.f1=b1;_tmp207.f2=b2;_tmp207;});struct _tuple0 _tmp204=_tmp203;struct Cyc_Absyn_Exp*_tmp206;struct Cyc_Absyn_Exp*_tmp205;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp204.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp204.f2)->tag == 0){_LL197: _LL198:
 return 0;}else{_LL199: _LL19A:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp204.f2)->tag == 0){_LL19B: _LL19C:
 return 1;}else{_LL19D: _tmp206=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp204.f1)->f1;_tmp205=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp204.f2)->f1;_LL19E:
 return Cyc_Evexp_const_exp_cmp(_tmp206,_tmp205);}}_LL196:;}
# 823
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp208=({struct _tuple0 _tmp216;_tmp216.f1=e1->r;_tmp216.f2=e2->r;_tmp216;});struct _tuple0 _tmp209=_tmp208;void*_tmp215;void*_tmp214;struct Cyc_Absyn_Exp*_tmp213;struct Cyc_Absyn_Exp*_tmp212;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209.f1)->tag == 13){_LL1A0: _tmp212=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209.f1)->f2;_LL1A1:
# 828
 Cyc_Tcutil_unify_cmp_exp(_tmp212,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209.f2)->tag == 13){_LL1A2: _tmp213=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp209.f2)->f2;_LL1A3:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp213);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f1)->tag == 38){_LL1A4: _tmp214=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f1)->f1;_LL1A5: {
# 831
void*_tmp20A=Cyc_Tcutil_compress(_tmp214);void*_tmp20B=_tmp20A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20B)->tag == 1){_LL1AB: _LL1AC:
 Cyc_Tcutil_unify_it(_tmp214,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp20C=_cycalloc(sizeof(*_tmp20C));_tmp20C[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp20D;_tmp20D.tag=18;_tmp20D.f1=e2;_tmp20D;});_tmp20C;}));return;}else{_LL1AD: _LL1AE:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL1AA:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f2)->tag == 38){_LL1A6: _tmp215=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f2)->f1;_LL1A7: {
# 836
void*_tmp20E=Cyc_Tcutil_compress(_tmp215);void*_tmp20F=_tmp20E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20F)->tag == 1){_LL1B0: _LL1B1:
 Cyc_Tcutil_unify_it(_tmp215,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp211;_tmp211.tag=18;_tmp211.f1=e1;_tmp211;});_tmp210;}));return;}else{_LL1B2: _LL1B3:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL1AF:;}}else{_LL1A8: _LL1A9:
# 840
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL19F:;}
# 844
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp217=({struct _tuple0 _tmp221;_tmp221.f1=b1;_tmp221.f2=b2;_tmp221;});struct _tuple0 _tmp218=_tmp217;struct Cyc_Absyn_Exp*_tmp220;struct Cyc_Absyn_Exp*_tmp21F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f2)->tag == 0){_LL1B5: _LL1B6:
 return 0;}else{_LL1B7: _LL1B8:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp218.f2)->tag == 0){_LL1B9: _LL1BA:
 return 1;}else{_LL1BB: _tmp220=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218.f1)->f1;_tmp21F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp218.f2)->f1;_LL1BC: {
# 850
int r=Cyc_Evexp_const_exp_cmp(_tmp220,_tmp21F);
if(r == 0)return 0;{
struct _handler_cons _tmp219;_push_handler(& _tmp219);{int _tmp21B=0;if(setjmp(_tmp219.handler))_tmp21B=1;if(!_tmp21B){
Cyc_Tcutil_unify_cmp_exp(_tmp220,_tmp21F);{
int _tmp21C=0;_npop_handler(0);return _tmp21C;};
# 853
;_pop_handler();}else{void*_tmp21A=(void*)_exn_thrown;void*_tmp21D=_tmp21A;void*_tmp21E;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp21D)->tag == Cyc_Tcutil_Unify){_LL1BE: _LL1BF:
# 856
 return r;}else{_LL1C0: _tmp21E=_tmp21D;_LL1C1:(int)_rethrow(_tmp21E);}_LL1BD:;}};};}}}_LL1B4:;}
# 861
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
# 888
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp223=({struct _tuple0 _tmp235;_tmp235.f1=att1;_tmp235.f2=att2;_tmp235;});struct _tuple0 _tmp224=_tmp223;enum Cyc_Absyn_Format_Type _tmp234;int _tmp233;int _tmp232;enum Cyc_Absyn_Format_Type _tmp231;int _tmp230;int _tmp22F;struct _dyneither_ptr _tmp22E;struct _dyneither_ptr _tmp22D;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Exp*_tmp22B;int _tmp22A;int _tmp229;int _tmp228;int _tmp227;switch(*((int*)_tmp224.f1)){case 0: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 0){_LL1F0: _tmp228=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp227=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_LL1F1:
 _tmp22A=_tmp228;_tmp229=_tmp227;goto _LL1F3;}else{goto _LL1FA;}case 20: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 20){_LL1F2: _tmp22A=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp229=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_LL1F3:
# 892
 return Cyc_Core_intcmp(_tmp22A,_tmp229);}else{goto _LL1FA;}case 6: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 6){_LL1F4: _tmp22C=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp22B=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_LL1F5:
# 894
 if(_tmp22C == _tmp22B)return 0;
if(_tmp22C == 0)return - 1;
if(_tmp22B == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp22C,_tmp22B);}else{goto _LL1FA;}case 8: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 8){_LL1F6: _tmp22E=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp22D=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_LL1F7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp22E,(struct _dyneither_ptr)_tmp22D);}else{goto _LL1FA;}case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f2)->tag == 19){_LL1F8: _tmp234=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f1)->f1;_tmp233=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f1)->f2;_tmp232=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f1)->f3;_tmp231=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f2)->f1;_tmp230=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f2)->f2;_tmp22F=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp224.f2)->f3;_LL1F9: {
# 900
int _tmp225=Cyc_Core_intcmp((int)((unsigned int)_tmp234),(int)((unsigned int)_tmp231));
if(_tmp225 != 0)return _tmp225;{
int _tmp226=Cyc_Core_intcmp(_tmp233,_tmp230);
if(_tmp226 != 0)return _tmp226;
return Cyc_Core_intcmp(_tmp232,_tmp22F);};}}else{goto _LL1FA;}default: _LL1FA: _LL1FB:
# 906
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));}_LL1EF:;}
# 910
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 914
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 923
static void*Cyc_Tcutil_rgns_of(void*t);
# 925
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 929
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp236=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp237=_tmp236;switch(((struct Cyc_Absyn_Kind*)_tmp237)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp237)->aliasqual){case Cyc_Absyn_Unique: _LL1FD: _LL1FE:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1FC;case Cyc_Absyn_Aliasable: _LL1FF: _LL200:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1FC;default: goto _LL205;}case Cyc_Absyn_EffKind: _LL201: _LL202:
 t=Cyc_Absyn_empty_effect;goto _LL1FC;case Cyc_Absyn_IntKind: _LL203: _LL204:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp238=_cycalloc(sizeof(*_tmp238));_tmp238[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp239;_tmp239.tag=18;_tmp239.f1=Cyc_Absyn_uint_exp(0,0);_tmp239;});_tmp238;});goto _LL1FC;default: _LL205: _LL206:
 t=Cyc_Absyn_sint_typ;goto _LL1FC;}_LL1FC:;}
# 938
return({struct _tuple16*_tmp23A=_cycalloc(sizeof(*_tmp23A));_tmp23A->f1=tv;_tmp23A->f2=t;_tmp23A;});}
# 945
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
# 957
struct Cyc_Absyn_Kind*_tmp23D=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp23E=_tmp23D;switch(((struct Cyc_Absyn_Kind*)_tmp23E)->kind){case Cyc_Absyn_RgnKind: _LL243: _LL244:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23F=_cycalloc(sizeof(*_tmp23F));_tmp23F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp240;_tmp240.tag=23;_tmp240.f1=t;_tmp240;});_tmp23F;});case Cyc_Absyn_EffKind: _LL245: _LL246:
 return t;case Cyc_Absyn_IntKind: _LL247: _LL248:
 return Cyc_Absyn_empty_effect;default: _LL249: _LL24A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp241=_cycalloc(sizeof(*_tmp241));_tmp241[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp242;_tmp242.tag=25;_tmp242.f1=t;_tmp242;});_tmp241;});}_LL242:;}case 15: _LL21C: _tmp259=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp23C)->f1;_LL21D:
# 963
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp243=_cycalloc(sizeof(*_tmp243));_tmp243[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp244;_tmp244.tag=23;_tmp244.f1=_tmp259;_tmp244;});_tmp243;});case 16: _LL21E: _tmp25B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23C)->f1;_tmp25A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23C)->f2;_LL21F:
# 967
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp246;_tmp246.tag=23;_tmp246.f1=_tmp25A;_tmp246;});_tmp245;});case 3: _LL220: _tmp25C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL221: {
# 969
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp25C);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp248;_tmp248.tag=24;_tmp248.f1=ts;_tmp248;});_tmp247;}));}case 5: _LL222: _tmp25E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23C)->f1).elt_typ;_tmp25D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23C)->f1).ptr_atts).rgn;_LL223:
# 973
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp24A;_tmp24A.tag=24;_tmp24A.f1=({void*_tmp24B[2];_tmp24B[1]=Cyc_Tcutil_rgns_of(_tmp25E);_tmp24B[0]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp24D;_tmp24D.tag=23;_tmp24D.f1=_tmp25D;_tmp24D;});_tmp24C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp24B,sizeof(void*),2));});_tmp24A;});_tmp249;}));case 8: _LL224: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C)->f1).elt_type;_LL225:
# 975
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp25F));case 10: _LL226: _tmp260=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23C)->f1;_LL227: {
# 977
struct Cyc_List_List*_tmp24E=0;
for(0;_tmp260 != 0;_tmp260=_tmp260->tl){
_tmp24E=({struct Cyc_List_List*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F->hd=(*((struct _tuple12*)_tmp260->hd)).f2;_tmp24F->tl=_tmp24E;_tmp24F;});}
_tmp261=_tmp24E;goto _LL229;}case 4: _LL228: _tmp261=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL229:
 _tmp262=_tmp261;goto _LL22B;case 11: _LL22A: _tmp262=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL22B:
# 983
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp251;_tmp251.tag=24;_tmp251.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp262);_tmp251;});_tmp250;}));case 12: _LL22C: _tmp263=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23C)->f2;_LL22D:
# 985
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp253;_tmp253.tag=24;_tmp253.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp263);_tmp253;});_tmp252;}));case 19: _LL22E: _LL22F:
 return Cyc_Absyn_empty_effect;case 9: _LL230: _tmp26A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).tvars;_tmp269=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).effect;_tmp268=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).ret_tqual;_tmp267=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).ret_typ;_tmp266=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).args;_tmp265=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).cyc_varargs;_tmp264=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).rgn_po;_LL231: {
# 995
void*_tmp254=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp26A),(void*)_check_null(_tmp269));
return Cyc_Tcutil_normalize_effect(_tmp254);}case 21: _LL232: _LL233:
 goto _LL235;case 22: _LL234: _LL235:
 goto _LL237;case 20: _LL236: _LL237:
 return Cyc_Absyn_empty_effect;case 23: _LL238: _LL239:
 goto _LL23B;case 24: _LL23A: _LL23B:
 return t;case 25: _LL23C: _tmp26B=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp23C)->f1;_LL23D:
 return Cyc_Tcutil_rgns_of(_tmp26B);case 17: _LL23E: _tmp26C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23C)->f2;_LL23F:
# 1004
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp256;_tmp256.tag=24;_tmp256.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp26C);_tmp256;});_tmp255;}));default: _LL240: _LL241:
({void*_tmp257=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp258="typedecl in rgns_of";_tag_dyneither(_tmp258,sizeof(char),20);}),_tag_dyneither(_tmp257,sizeof(void*),0));});}_LL207:;}
# 1012
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp26D=e;void*_tmp279;struct Cyc_List_List**_tmp278;switch(*((int*)_tmp26D)){case 24: _LL24C: _tmp278=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp26D)->f1;_LL24D: {
# 1016
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp278;for(0;effs != 0;effs=effs->tl){
void*_tmp26E=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp26E));{
void*_tmp26F=(void*)effs->hd;void*_tmp270=_tmp26F;switch(*((int*)_tmp270)){case 24: _LL253: _LL254:
 goto _LL256;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp270)->f1)){case 20: _LL255: _LL256:
 goto _LL258;case 22: _LL257: _LL258:
 goto _LL25A;case 21: _LL259: _LL25A:
 redo_join=1;goto _LL252;default: goto _LL25B;}default: _LL25B: _LL25C:
 goto _LL252;}_LL252:;};}}
# 1028
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp278;for(0;effs != 0;effs=effs->tl){
void*_tmp271=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp272=_tmp271;void*_tmp275;struct Cyc_List_List*_tmp274;switch(*((int*)_tmp272)){case 24: _LL25E: _tmp274=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp272)->f1;_LL25F:
# 1033
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp274,effects);
goto _LL25D;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp272)->f1)){case 20: _LL260: _LL261:
 goto _LL263;case 22: _LL262: _LL263:
 goto _LL265;case 21: _LL264: _LL265:
 goto _LL25D;default: goto _LL266;}default: _LL266: _tmp275=_tmp272;_LL267:
 effects=({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=_tmp275;_tmp273->tl=effects;_tmp273;});goto _LL25D;}_LL25D:;}}
# 1041
*_tmp278=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}case 25: _LL24E: _tmp279=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp26D)->f1;_LL24F: {
# 1044
void*_tmp276=Cyc_Tcutil_compress(_tmp279);void*_tmp277=_tmp276;switch(*((int*)_tmp277)){case 1: _LL269: _LL26A:
 goto _LL26C;case 2: _LL26B: _LL26C:
 return e;default: _LL26D: _LL26E:
 return Cyc_Tcutil_rgns_of(_tmp279);}_LL268:;}default: _LL250: _LL251:
# 1049
 return e;}_LL24B:;};}
# 1054
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27A=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));_tmp27B[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp27C;_tmp27C.tag=9;_tmp27C.f1=({struct Cyc_Absyn_FnInfo _tmp27D;_tmp27D.tvars=0;_tmp27D.effect=eff;_tmp27D.ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp27D.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp27D.args=0;_tmp27D.c_varargs=0;_tmp27D.cyc_varargs=0;_tmp27D.rgn_po=0;_tmp27D.attributes=0;_tmp27D.requires_clause=0;_tmp27D.requires_relns=0;_tmp27D.ensures_clause=0;_tmp27D.ensures_relns=0;_tmp27D;});_tmp27C;});_tmp27B;});
# 1064
return Cyc_Absyn_atb_typ((void*)_tmp27A,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1071
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp27E=Cyc_Tcutil_compress(e);void*_tmp27F=_tmp27E;struct Cyc_Core_Opt*_tmp2A4;void**_tmp2A3;struct Cyc_Core_Opt*_tmp2A2;void*_tmp2A1;struct Cyc_List_List*_tmp2A0;void*_tmp29F;switch(*((int*)_tmp27F)){case 23: _LL270: _tmp29F=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp27F)->f1;_LL271:
# 1078
 if(constrain)return Cyc_Tcutil_unify(r,_tmp29F);
_tmp29F=Cyc_Tcutil_compress(_tmp29F);
if(r == _tmp29F)return 1;{
struct _tuple0 _tmp280=({struct _tuple0 _tmp284;_tmp284.f1=r;_tmp284.f2=_tmp29F;_tmp284;});struct _tuple0 _tmp281=_tmp280;struct Cyc_Absyn_Tvar*_tmp283;struct Cyc_Absyn_Tvar*_tmp282;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f2)->tag == 2){_LL27B: _tmp283=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f1)->f1;_tmp282=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f2)->f1;_LL27C:
 return Cyc_Absyn_tvar_cmp(_tmp283,_tmp282)== 0;}else{goto _LL27D;}}else{_LL27D: _LL27E:
 return 0;}_LL27A:;};case 24: _LL272: _tmp2A0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp27F)->f1;_LL273:
# 1086
 for(0;_tmp2A0 != 0;_tmp2A0=_tmp2A0->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp2A0->hd))return 1;}
return 0;case 25: _LL274: _tmp2A1=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp27F)->f1;_LL275: {
# 1090
void*_tmp285=Cyc_Tcutil_rgns_of(_tmp2A1);void*_tmp286=_tmp285;void*_tmp294;void*_tmp293;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp286)->tag == 25){_LL280: _tmp293=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp286)->f1;_LL281:
# 1092
 if(!constrain)return 0;{
void*_tmp287=Cyc_Tcutil_compress(_tmp293);void*_tmp288=_tmp287;struct Cyc_Core_Opt*_tmp292;void**_tmp291;struct Cyc_Core_Opt*_tmp290;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp288)->tag == 1){_LL285: _tmp292=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp288)->f1;_tmp291=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp288)->f2;_tmp290=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp288)->f4;_LL286: {
# 1097
void*_tmp289=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp290);
# 1100
Cyc_Tcutil_occurs(_tmp289,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp290))->v,r);{
void*_tmp28A=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));_tmp28B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp28C;_tmp28C.tag=24;_tmp28C.f1=({void*_tmp28D[2];_tmp28D[1]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp28E=_cycalloc(sizeof(*_tmp28E));_tmp28E[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp28F;_tmp28F.tag=23;_tmp28F.f1=r;_tmp28F;});_tmp28E;});_tmp28D[0]=_tmp289;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp28D,sizeof(void*),2));});_tmp28C;});_tmp28B;}));
*_tmp291=_tmp28A;
return 1;};}}else{_LL287: _LL288:
 return 0;}_LL284:;};}else{_LL282: _tmp294=_tmp286;_LL283:
# 1106
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp294);}_LL27F:;}case 1: _LL276: _tmp2A4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f1;_tmp2A3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f2;_tmp2A2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f4;_LL277:
# 1109
 if(_tmp2A4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2A4->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp295=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp296="effect evar has wrong kind";_tag_dyneither(_tmp296,sizeof(char),27);}),_tag_dyneither(_tmp295,sizeof(void*),0));});
if(!constrain)return 0;{
# 1114
void*_tmp297=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A2);
# 1117
Cyc_Tcutil_occurs(_tmp297,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A2))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp298=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp299=_cycalloc(sizeof(*_tmp299));_tmp299[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp29A;_tmp29A.tag=24;_tmp29A.f1=({struct Cyc_List_List*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->hd=_tmp297;_tmp29B->tl=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));_tmp29D[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp29E;_tmp29E.tag=23;_tmp29E.f1=r;_tmp29E;});_tmp29D;});_tmp29C->tl=0;_tmp29C;});_tmp29B;});_tmp29A;});_tmp299;});
*_tmp2A3=(void*)_tmp298;
return 1;};};default: _LL278: _LL279:
 return 0;}_LL26F:;};}
# 1128
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp2A5=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2A6=_tmp2A5;struct Cyc_Core_Opt*_tmp2BE;void**_tmp2BD;struct Cyc_Core_Opt*_tmp2BC;void*_tmp2BB;struct Cyc_List_List*_tmp2BA;switch(*((int*)_tmp2A6)){case 23: _LL28A: _LL28B:
 return 0;case 24: _LL28C: _tmp2BA=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2A6)->f1;_LL28D:
# 1133
 for(0;_tmp2BA != 0;_tmp2BA=_tmp2BA->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2BA->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2BB=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A6)->f1;_LL28F:
# 1138
 _tmp2BB=Cyc_Tcutil_compress(_tmp2BB);
if(t == _tmp2BB)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2BB);{
void*_tmp2A7=Cyc_Tcutil_rgns_of(t);void*_tmp2A8=_tmp2A7;void*_tmp2AF;void*_tmp2AE;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A8)->tag == 25){_LL295: _tmp2AE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A8)->f1;_LL296: {
# 1143
struct _tuple0 _tmp2A9=({struct _tuple0 _tmp2AD;_tmp2AD.f1=Cyc_Tcutil_compress(_tmp2AE);_tmp2AD.f2=_tmp2BB;_tmp2AD;});struct _tuple0 _tmp2AA=_tmp2A9;struct Cyc_Absyn_Tvar*_tmp2AC;struct Cyc_Absyn_Tvar*_tmp2AB;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f2)->tag == 2){_LL29A: _tmp2AC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f1)->f1;_tmp2AB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f2)->f1;_LL29B:
 return Cyc_Tcutil_unify(t,_tmp2BB);}else{goto _LL29C;}}else{_LL29C: _LL29D:
 return _tmp2AE == _tmp2BB;}_LL299:;}}else{_LL297: _tmp2AF=_tmp2A8;_LL298:
# 1147
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2AF);}_LL294:;};case 1: _LL290: _tmp2BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f1;_tmp2BD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f2;_tmp2BC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f4;_LL291:
# 1150
 if(_tmp2BE == 0  || ((struct Cyc_Absyn_Kind*)_tmp2BE->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2B0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2B1="effect evar has wrong kind";_tag_dyneither(_tmp2B1,sizeof(char),27);}),_tag_dyneither(_tmp2B0,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{
# 1155
void*_tmp2B2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2BC);
# 1158
Cyc_Tcutil_occurs(_tmp2B2,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BC))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B3=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));_tmp2B4[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2B5;_tmp2B5.tag=24;_tmp2B5.f1=({struct Cyc_List_List*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->hd=_tmp2B2;_tmp2B6->tl=({struct Cyc_List_List*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));_tmp2B8[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2B9;_tmp2B9.tag=25;_tmp2B9.f1=t;_tmp2B9;});_tmp2B8;});_tmp2B7->tl=0;_tmp2B7;});_tmp2B6;});_tmp2B5;});_tmp2B4;});
*_tmp2BD=(void*)_tmp2B3;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1169
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2BF=e;struct Cyc_Core_Opt*_tmp2DF;void**_tmp2DE;struct Cyc_Core_Opt*_tmp2DD;void*_tmp2DC;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Tvar*_tmp2DA;switch(*((int*)_tmp2BF)){case 2: _LL29F: _tmp2DA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BF)->f1;_LL2A0:
 return Cyc_Absyn_tvar_cmp(v,_tmp2DA)== 0;case 24: _LL2A1: _tmp2DB=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2BF)->f1;_LL2A2:
# 1174
 for(0;_tmp2DB != 0;_tmp2DB=_tmp2DB->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2DB->hd))
return 1;}
return 0;case 25: _LL2A3: _tmp2DC=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2BF)->f1;_LL2A4: {
# 1179
void*_tmp2C0=Cyc_Tcutil_rgns_of(_tmp2DC);void*_tmp2C1=_tmp2C0;void*_tmp2CF;void*_tmp2CE;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C1)->tag == 25){_LL2AA: _tmp2CE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C1)->f1;_LL2AB:
# 1181
 if(!may_constrain_evars)return 0;{
void*_tmp2C2=Cyc_Tcutil_compress(_tmp2CE);void*_tmp2C3=_tmp2C2;struct Cyc_Core_Opt*_tmp2CD;void**_tmp2CC;struct Cyc_Core_Opt*_tmp2CB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->tag == 1){_LL2AF: _tmp2CD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f1;_tmp2CC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f2;_tmp2CB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C3)->f4;_LL2B0: {
# 1187
void*_tmp2C4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2CB);
# 1189
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CB))->v,v))return 0;{
void*_tmp2C5=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2C7;_tmp2C7.tag=24;_tmp2C7.f1=({void*_tmp2C8[2];_tmp2C8[1]=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));_tmp2C9[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2CA;_tmp2CA.tag=2;_tmp2CA.f1=v;_tmp2CA;});_tmp2C9;});_tmp2C8[0]=_tmp2C4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2C8,sizeof(void*),2));});_tmp2C7;});_tmp2C6;}));
*_tmp2CC=_tmp2C5;
return 1;};}}else{_LL2B1: _LL2B2:
 return 0;}_LL2AE:;};}else{_LL2AC: _tmp2CF=_tmp2C1;_LL2AD:
# 1195
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2CF);}_LL2A9:;}case 1: _LL2A5: _tmp2DF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f1;_tmp2DE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f2;_tmp2DD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f4;_LL2A6:
# 1198
 if(_tmp2DF == 0  || ((struct Cyc_Absyn_Kind*)_tmp2DF->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2D0=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2D1="effect evar has wrong kind";_tag_dyneither(_tmp2D1,sizeof(char),27);}),_tag_dyneither(_tmp2D0,sizeof(void*),0));});{
# 1202
void*_tmp2D2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2DD);
# 1204
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DD))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D3=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2D5;_tmp2D5.tag=24;_tmp2D5.f1=({struct Cyc_List_List*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->hd=_tmp2D2;_tmp2D6->tl=({struct Cyc_List_List*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));_tmp2D8[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2D9;_tmp2D9.tag=2;_tmp2D9.f1=v;_tmp2D9;});_tmp2D8;});_tmp2D7->tl=0;_tmp2D7;});_tmp2D6;});_tmp2D5;});_tmp2D4;});
*_tmp2DE=(void*)_tmp2D3;
return 1;};};default: _LL2A7: _LL2A8:
 return 0;}_LL29E:;};}
# 1214
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2E0=e;void*_tmp2E6;struct Cyc_List_List*_tmp2E5;switch(*((int*)_tmp2E0)){case 24: _LL2B4: _tmp2E5=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E0)->f1;_LL2B5:
# 1218
 for(0;_tmp2E5 != 0;_tmp2E5=_tmp2E5->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2E5->hd))
return 1;}
return 0;case 25: _LL2B6: _tmp2E6=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E0)->f1;_LL2B7: {
# 1223
void*_tmp2E1=Cyc_Tcutil_rgns_of(_tmp2E6);void*_tmp2E2=_tmp2E1;void*_tmp2E4;void*_tmp2E3;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E2)->tag == 25){_LL2BD: _tmp2E3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E2)->f1;_LL2BE:
 return 0;}else{_LL2BF: _tmp2E4=_tmp2E2;_LL2C0:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2E4);}_LL2BC:;}case 1: _LL2B8: _LL2B9:
# 1227
 return evar == e;default: _LL2BA: _LL2BB:
 return 0;}_LL2B3:;};}
# 1241 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1246
void*_tmp2E7=Cyc_Tcutil_compress(e1);void*_tmp2E8=_tmp2E7;void**_tmp2F5;struct Cyc_Core_Opt*_tmp2F4;void*_tmp2F3;struct Cyc_Absyn_Tvar*_tmp2F2;void*_tmp2F1;struct Cyc_List_List*_tmp2F0;switch(*((int*)_tmp2E8)){case 24: _LL2C2: _tmp2F0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E8)->f1;_LL2C3:
# 1248
 for(0;_tmp2F0 != 0;_tmp2F0=_tmp2F0->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp2F0->hd,e2))
return 0;}
return 1;case 23: _LL2C4: _tmp2F1=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2E8)->f1;_LL2C5:
# 1259
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp2F1,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2F1,(void*)& Cyc_Absyn_HeapRgn_val);case 2: _LL2C6: _tmp2F2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2E8)->f1;_LL2C7:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp2F2,e2);case 25: _LL2C8: _tmp2F3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E8)->f1;_LL2C9: {
# 1263
void*_tmp2E9=Cyc_Tcutil_rgns_of(_tmp2F3);void*_tmp2EA=_tmp2E9;void*_tmp2EC;void*_tmp2EB;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2EA)->tag == 25){_LL2CF: _tmp2EB=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2EA)->f1;_LL2D0:
# 1268
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp2EB,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2EB,Cyc_Absyn_sint_typ);}else{_LL2D1: _tmp2EC=_tmp2EA;_LL2D2:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp2EC,e2);}_LL2CE:;}case 1: _LL2CA: _tmp2F5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E8)->f2;_tmp2F4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E8)->f4;_LL2CB:
# 1273
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1277
*_tmp2F5=Cyc_Absyn_empty_effect;
# 1280
return 1;}else{
# 1282
return 0;}default: _LL2CC: _LL2CD:
({struct Cyc_String_pa_PrintArg_struct _tmp2EF;_tmp2EF.tag=0;_tmp2EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));({void*_tmp2ED[1]={& _tmp2EF};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2EE="subset_effect: bad effect: %s";_tag_dyneither(_tmp2EE,sizeof(char),30);}),_tag_dyneither(_tmp2ED,sizeof(void*),1));});});}_LL2C1:;}
# 1298 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1314
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1316
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp2F6=(struct _tuple0*)r1->hd;struct _tuple0*_tmp2F7=_tmp2F6;void*_tmp2FD;void*_tmp2FC;_LL2D4: _tmp2FD=_tmp2F7->f1;_tmp2FC=_tmp2F7->f2;_LL2D5:;{
int found=_tmp2FD == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp2F8=(struct _tuple0*)r2->hd;struct _tuple0*_tmp2F9=_tmp2F8;void*_tmp2FB;void*_tmp2FA;_LL2D7: _tmp2FB=_tmp2F9->f1;_tmp2FA=_tmp2F9->f2;_LL2D8:;
if(Cyc_Tcutil_unify(_tmp2FD,_tmp2FB) && Cyc_Tcutil_unify(_tmp2FC,_tmp2FA)){
found=1;
break;}}}
# 1326
if(!found)return 0;};}}
# 1328
return 1;}
# 1331
typedef struct Cyc_List_List*Cyc_Tcutil_relns_t;
# 1335
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp2FE=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp2FF=({struct Cyc_List_List*_tmp300=_cycalloc(sizeof(*_tmp300));_tmp300->hd=_tmp2FE;_tmp300->tl=r1;_tmp300;});
if(Cyc_Relations_consistent_relations(_tmp2FF))return 0;}
# 1341
return 1;}
# 1346
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1352
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1354
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1358
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp301=t1;struct Cyc_Core_Opt*_tmp310;void**_tmp30F;struct Cyc_Core_Opt*_tmp30E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp301)->tag == 1){_LL2DA: _tmp310=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp301)->f1;_tmp30F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp301)->f2;_tmp30E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp301)->f4;_LL2DB:
# 1369
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp30E))->v,t2);{
struct Cyc_Absyn_Kind*_tmp302=Cyc_Tcutil_typ_kind(t2);
# 1374
if(Cyc_Tcutil_kind_leq(_tmp302,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp310))->v)){
*_tmp30F=t2;
return;}else{
# 1378
{void*_tmp303=t2;struct Cyc_Absyn_PtrInfo _tmp30C;void**_tmp30B;struct Cyc_Core_Opt*_tmp30A;switch(*((int*)_tmp303)){case 1: _LL2DF: _tmp30B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp303)->f2;_tmp30A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp303)->f4;_LL2E0: {
# 1381
struct Cyc_List_List*_tmp304=(struct Cyc_List_List*)_tmp30E->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp30A))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp304,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp305="(type variable would escape scope)";_tag_dyneither(_tmp305,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1388
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp310->v,_tmp302)){
*_tmp30B=t1;return;}
# 1391
Cyc_Tcutil_failure_reason=({const char*_tmp306="(kinds are incompatible)";_tag_dyneither(_tmp306,sizeof(char),25);});
goto _LL2DE;}case 5: _LL2E1: _tmp30C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp303)->f1;if(((struct Cyc_Absyn_Kind*)_tmp310->v)->kind == Cyc_Absyn_BoxKind){_LL2E2: {
# 1397
union Cyc_Absyn_Constraint*_tmp307=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp30C.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp308=_tmp307;if((((union Cyc_Absyn_Constraint*)_tmp308)->No_constr).tag == 3){_LL2E6: _LL2E7:
# 1401
({struct _union_Constraint_Eq_constr*_tmp309=& _tmp307->Eq_constr;_tmp309->tag=1;_tmp309->val=Cyc_Absyn_bounds_one;});
*_tmp30F=t2;
return;}else{_LL2E8: _LL2E9:
 goto _LL2E5;}_LL2E5:;}
# 1406
goto _LL2DE;}}else{goto _LL2E3;}default: _LL2E3: _LL2E4:
 goto _LL2DE;}_LL2DE:;}
# 1409
Cyc_Tcutil_failure_reason=({const char*_tmp30D="(kinds are incompatible)";_tag_dyneither(_tmp30D,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL2DC: _LL2DD:
# 1412
 goto _LL2D9;}_LL2D9:;}
# 1417
{struct _tuple0 _tmp311=({struct _tuple0 _tmp3D6;_tmp3D6.f1=t2;_tmp3D6.f2=t1;_tmp3D6;});struct _tuple0 _tmp312=_tmp311;void*_tmp3D5;void*_tmp3D4;void*_tmp3D3;void*_tmp3D2;void*_tmp3D1;void*_tmp3D0;struct Cyc_List_List*_tmp3CF;struct Cyc_Absyn_Typedefdecl*_tmp3CE;struct Cyc_List_List*_tmp3CD;struct Cyc_Absyn_Typedefdecl*_tmp3CC;enum Cyc_Absyn_AggrKind _tmp3CB;struct Cyc_List_List*_tmp3CA;enum Cyc_Absyn_AggrKind _tmp3C9;struct Cyc_List_List*_tmp3C8;struct Cyc_List_List*_tmp3C7;struct Cyc_List_List*_tmp3C6;struct Cyc_List_List*_tmp3C5;void*_tmp3C4;struct Cyc_Absyn_Tqual _tmp3C3;void*_tmp3C2;struct Cyc_List_List*_tmp3C1;int _tmp3C0;struct Cyc_Absyn_VarargInfo*_tmp3BF;struct Cyc_List_List*_tmp3BE;struct Cyc_List_List*_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_List_List*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_List_List*_tmp3B9;struct Cyc_List_List*_tmp3B8;void*_tmp3B7;struct Cyc_Absyn_Tqual _tmp3B6;void*_tmp3B5;struct Cyc_List_List*_tmp3B4;int _tmp3B3;struct Cyc_Absyn_VarargInfo*_tmp3B2;struct Cyc_List_List*_tmp3B1;struct Cyc_List_List*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_List_List*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_List_List*_tmp3AC;void*_tmp3AB;struct Cyc_Absyn_Tqual _tmp3AA;struct Cyc_Absyn_Exp*_tmp3A9;union Cyc_Absyn_Constraint*_tmp3A8;void*_tmp3A7;struct Cyc_Absyn_Tqual _tmp3A6;struct Cyc_Absyn_Exp*_tmp3A5;union Cyc_Absyn_Constraint*_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A2;void*_tmp3A1;void*_tmp3A0;int _tmp39F;int _tmp39E;enum Cyc_Absyn_Sign _tmp39D;enum Cyc_Absyn_Size_of _tmp39C;enum Cyc_Absyn_Sign _tmp39B;enum Cyc_Absyn_Size_of _tmp39A;void*_tmp399;struct Cyc_Absyn_Tqual _tmp398;void*_tmp397;union Cyc_Absyn_Constraint*_tmp396;union Cyc_Absyn_Constraint*_tmp395;union Cyc_Absyn_Constraint*_tmp394;void*_tmp393;struct Cyc_Absyn_Tqual _tmp392;void*_tmp391;union Cyc_Absyn_Constraint*_tmp390;union Cyc_Absyn_Constraint*_tmp38F;union Cyc_Absyn_Constraint*_tmp38E;struct Cyc_Absyn_Datatypedecl*_tmp38D;struct Cyc_Absyn_Datatypefield*_tmp38C;struct Cyc_List_List*_tmp38B;struct Cyc_Absyn_Datatypedecl*_tmp38A;struct Cyc_Absyn_Datatypefield*_tmp389;struct Cyc_List_List*_tmp388;struct Cyc_Absyn_Datatypedecl*_tmp387;struct Cyc_List_List*_tmp386;struct Cyc_Absyn_Datatypedecl*_tmp385;struct Cyc_List_List*_tmp384;struct Cyc_List_List*_tmp383;struct Cyc_List_List*_tmp382;struct _tuple2*_tmp381;struct _tuple2*_tmp380;union Cyc_Absyn_AggrInfoU _tmp37F;struct Cyc_List_List*_tmp37E;union Cyc_Absyn_AggrInfoU _tmp37D;struct Cyc_List_List*_tmp37C;struct Cyc_Absyn_Tvar*_tmp37B;struct Cyc_Absyn_Tvar*_tmp37A;switch(*((int*)_tmp312.f1)){case 1: _LL2EB: _LL2EC:
# 1420
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0: switch(*((int*)_tmp312.f2)){case 0: _LL2ED: _LL2EE:
# 1423
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 2: switch(*((int*)_tmp312.f2)){case 2: _LL2EF: _tmp37B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp37A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL2F0: {
# 1426
struct _dyneither_ptr*_tmp313=_tmp37B->name;
struct _dyneither_ptr*_tmp314=_tmp37A->name;
# 1429
int _tmp315=_tmp37B->identity;
int _tmp316=_tmp37A->identity;
if(_tmp316 == _tmp315)return;
Cyc_Tcutil_failure_reason=({const char*_tmp317="(variable types are not the same)";_tag_dyneither(_tmp317,sizeof(char),34);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 11: switch(*((int*)_tmp312.f2)){case 11: _LL2F1: _tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f1)->f1).aggr_info;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp37D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f2)->f1).aggr_info;_tmp37C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2F2: {
# 1436
struct _tuple13 _tmp318=Cyc_Absyn_aggr_kinded_name(_tmp37D);struct _tuple13 _tmp319=_tmp318;enum Cyc_Absyn_AggrKind _tmp321;struct _tuple2*_tmp320;_LL328: _tmp321=_tmp319.f1;_tmp320=_tmp319.f2;_LL329:;{
struct _tuple13 _tmp31A=Cyc_Absyn_aggr_kinded_name(_tmp37F);struct _tuple13 _tmp31B=_tmp31A;enum Cyc_Absyn_AggrKind _tmp31F;struct _tuple2*_tmp31E;_LL32B: _tmp31F=_tmp31B.f1;_tmp31E=_tmp31B.f2;_LL32C:;
if(_tmp321 != _tmp31F){Cyc_Tcutil_failure_reason=({const char*_tmp31C="(struct and union type)";_tag_dyneither(_tmp31C,sizeof(char),24);});goto _LL2EA;}
if(Cyc_Absyn_qvar_cmp(_tmp320,_tmp31E)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp31D="(different type name)";_tag_dyneither(_tmp31D,sizeof(char),22);});goto _LL2EA;}
Cyc_Tcutil_unify_list(_tmp37C,_tmp37E);
return;};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 13: switch(*((int*)_tmp312.f2)){case 13: _LL2F3: _tmp381=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp380=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL2F4:
# 1445
 if(Cyc_Absyn_qvar_cmp(_tmp381,_tmp380)== 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp322="(different enum types)";_tag_dyneither(_tmp322,sizeof(char),23);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 14: switch(*((int*)_tmp312.f2)){case 14: _LL2F5: _tmp383=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp382=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL2F6: {
# 1450
int bad=0;
for(0;_tmp383 != 0  && _tmp382 != 0;(_tmp383=_tmp383->tl,_tmp382=_tmp382->tl)){
struct Cyc_Absyn_Enumfield*_tmp323=(struct Cyc_Absyn_Enumfield*)_tmp383->hd;
struct Cyc_Absyn_Enumfield*_tmp324=(struct Cyc_Absyn_Enumfield*)_tmp382->hd;
if(Cyc_Absyn_qvar_cmp(_tmp323->name,_tmp324->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp325="(different names for enum fields)";_tag_dyneither(_tmp325,sizeof(char),34);});
bad=1;
break;}
# 1459
if(_tmp323->tag == _tmp324->tag)continue;
if(_tmp323->tag == 0  || _tmp324->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp326="(different tag values for enum fields)";_tag_dyneither(_tmp326,sizeof(char),39);});
bad=1;
break;}
# 1465
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp323->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp324->tag))){
Cyc_Tcutil_failure_reason=({const char*_tmp327="(different tag values for enum fields)";_tag_dyneither(_tmp327,sizeof(char),39);});
bad=1;
break;}}
# 1471
if(bad)goto _LL2EA;
if(_tmp383 == 0  && _tmp382 == 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp328="(different number of fields for enums)";_tag_dyneither(_tmp328,sizeof(char),39);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp312.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2F7: _tmp387=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).datatype_info).KnownDatatype).val;_tmp386=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp385=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).datatype_info).KnownDatatype).val;_tmp384=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2F8:
# 1478
 if(_tmp387 == _tmp385  || Cyc_Absyn_qvar_cmp(_tmp387->name,_tmp385->name)== 0){
Cyc_Tcutil_unify_list(_tmp384,_tmp386);
return;}
# 1482
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp312.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp312.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2F9: _tmp38D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp38C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp38B=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp38A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp389=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp388=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2FA:
# 1486
 if((_tmp38D == _tmp38A  || Cyc_Absyn_qvar_cmp(_tmp38D->name,_tmp38A->name)== 0) && (
_tmp38C == _tmp389  || Cyc_Absyn_qvar_cmp(_tmp38C->name,_tmp389->name)== 0)){
Cyc_Tcutil_unify_list(_tmp388,_tmp38B);
return;}
# 1491
Cyc_Tcutil_failure_reason=({const char*_tmp329="(different datatype field types)";_tag_dyneither(_tmp329,sizeof(char),33);});
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp312.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 5: switch(*((int*)_tmp312.f2)){case 5: _LL2FB: _tmp399=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_typ;_tmp398=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_tq;_tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).rgn;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).nullable;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).bounds;_tmp394=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).zero_term;_tmp393=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_typ;_tmp392=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_tq;_tmp391=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).rgn;_tmp390=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).nullable;_tmp38F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).bounds;_tmp38E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).zero_term;_LL2FC:
# 1496
 Cyc_Tcutil_unify_it(_tmp393,_tmp399);
Cyc_Tcutil_unify_it(_tmp397,_tmp391);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp38E,_tmp394,({const char*_tmp32A="(not both zero terminated)";_tag_dyneither(_tmp32A,sizeof(char),27);}));
Cyc_Tcutil_unify_tqual(_tmp392,_tmp393,_tmp398,_tmp399);
((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp38F,_tmp395,({const char*_tmp32B="(different pointer bounds)";_tag_dyneither(_tmp32B,sizeof(char),27);}));
# 1504
{void*_tmp32C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp38F);void*_tmp32D=_tmp32C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp32D)->tag == 0){_LL32E: _LL32F:
 return;}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;}
# 1508
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp390,_tmp396,({const char*_tmp32E="(incompatible pointer types)";_tag_dyneither(_tmp32E,sizeof(char),29);}));
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 6: switch(*((int*)_tmp312.f2)){case 6: _LL2FD: _tmp39D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp39C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp39B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f2)->f1;_tmp39A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f2)->f2;_LL2FE:
# 1513
 if(_tmp39B == _tmp39D  && ((_tmp39A == _tmp39C  || _tmp39A == Cyc_Absyn_Int_sz  && _tmp39C == Cyc_Absyn_Long_sz) || 
_tmp39A == Cyc_Absyn_Long_sz  && _tmp39C == Cyc_Absyn_Int_sz))return;
Cyc_Tcutil_failure_reason=({const char*_tmp32F="(different integral types)";_tag_dyneither(_tmp32F,sizeof(char),27);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 7: switch(*((int*)_tmp312.f2)){case 7: _LL2FF: _tmp39F=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp39E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL300:
# 1519
 if(_tmp39E == 0  && _tmp39F == 0)return;else{
if(_tmp39E == 1  && _tmp39F == 1)return;else{
# 1522
if(((_tmp39E != 0  && _tmp39E != 1) && _tmp39F != 0) && _tmp39F != 1)return;}}
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 19: switch(*((int*)_tmp312.f2)){case 19: _LL301: _tmp3A1=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3A0=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL302:
# 1525
 Cyc_Tcutil_unify_it(_tmp3A1,_tmp3A0);return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 18: switch(*((int*)_tmp312.f2)){case 18: _LL303: _tmp3A3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3A2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL304:
# 1528
 if(!Cyc_Evexp_same_const_exp(_tmp3A3,_tmp3A2)){
Cyc_Tcutil_failure_reason=({const char*_tmp330="(cannot prove expressions are the same)";_tag_dyneither(_tmp330,sizeof(char),40);});
goto _LL2EA;}
# 1532
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 8: switch(*((int*)_tmp312.f2)){case 8: _LL305: _tmp3AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_type;_tmp3AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).tq;_tmp3A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).num_elts;_tmp3A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).zero_term;_tmp3A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_type;_tmp3A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).tq;_tmp3A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).num_elts;_tmp3A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).zero_term;_LL306:
# 1536
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
# 1550
int done=0;
{struct _RegionHandle _tmp333=_new_region("rgn");struct _RegionHandle*rgn=& _tmp333;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp3B8 != 0){
if(_tmp3C5 == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp334="(second function type has too few type variables)";_tag_dyneither(_tmp334,sizeof(char),50);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1558
void*_tmp335=((struct Cyc_Absyn_Tvar*)_tmp3B8->hd)->kind;
void*_tmp336=((struct Cyc_Absyn_Tvar*)_tmp3C5->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp335,_tmp336)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33B;_tmp33B.tag=0;_tmp33B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1564
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3C5->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp33A;_tmp33A.tag=0;_tmp33A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1563
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3B8->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp339;_tmp339.tag=0;_tmp339.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1562
Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp3B8->hd));({void*_tmp337[3]={& _tmp339,& _tmp33A,& _tmp33B};Cyc_aprintf(({const char*_tmp338="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp338,sizeof(char),44);}),_tag_dyneither(_tmp337,sizeof(void*),3));});});});});
# 1565
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1567
inst=({struct Cyc_List_List*_tmp33C=_region_malloc(rgn,sizeof(*_tmp33C));_tmp33C->hd=({struct _tuple16*_tmp33D=_region_malloc(rgn,sizeof(*_tmp33D));_tmp33D->f1=(struct Cyc_Absyn_Tvar*)_tmp3C5->hd;_tmp33D->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp33F;_tmp33F.tag=2;_tmp33F.f1=(struct Cyc_Absyn_Tvar*)_tmp3B8->hd;_tmp33F;});_tmp33E;});_tmp33D;});_tmp33C->tl=inst;_tmp33C;});
_tmp3B8=_tmp3B8->tl;
_tmp3C5=_tmp3C5->tl;};}
# 1571
if(_tmp3C5 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp340="(second function type has too many type variables)";_tag_dyneither(_tmp340,sizeof(char),51);});
_npop_handler(0);goto _LL2EA;}
# 1575
if(inst != 0){
Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp341=_cycalloc(sizeof(*_tmp341));_tmp341[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp342;_tmp342.tag=9;_tmp342.f1=({struct Cyc_Absyn_FnInfo _tmp343;_tmp343.tvars=0;_tmp343.effect=_tmp3B7;_tmp343.ret_tqual=_tmp3B6;_tmp343.ret_typ=_tmp3B5;_tmp343.args=_tmp3B4;_tmp343.c_varargs=_tmp3B3;_tmp343.cyc_varargs=_tmp3B2;_tmp343.rgn_po=_tmp3B1;_tmp343.attributes=_tmp3B0;_tmp343.requires_clause=_tmp3BC;_tmp343.requires_relns=_tmp3BB;_tmp343.ensures_clause=_tmp3BA;_tmp343.ensures_relns=_tmp3B9;_tmp343;});_tmp342;});_tmp341;}),
# 1579
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp345;_tmp345.tag=9;_tmp345.f1=({struct Cyc_Absyn_FnInfo _tmp346;_tmp346.tvars=0;_tmp346.effect=_tmp3C4;_tmp346.ret_tqual=_tmp3C3;_tmp346.ret_typ=_tmp3C2;_tmp346.args=_tmp3C1;_tmp346.c_varargs=_tmp3C0;_tmp346.cyc_varargs=_tmp3BF;_tmp346.rgn_po=_tmp3BE;_tmp346.attributes=_tmp3BD;_tmp346.requires_clause=_tmp3AF;_tmp346.requires_relns=_tmp3AE;_tmp346.ensures_clause=_tmp3AD;_tmp346.ensures_relns=_tmp3AC;_tmp346;});_tmp345;});_tmp344;})));
# 1584
done=1;}}
# 1552
;_pop_region(rgn);}
# 1587
if(done)
return;
Cyc_Tcutil_unify_it(_tmp3B5,_tmp3C2);
Cyc_Tcutil_unify_tqual(_tmp3B6,_tmp3B5,_tmp3C3,_tmp3C2);
for(0;_tmp3B4 != 0  && _tmp3C1 != 0;(_tmp3B4=_tmp3B4->tl,_tmp3C1=_tmp3C1->tl)){
struct _tuple10 _tmp347=*((struct _tuple10*)_tmp3B4->hd);struct _tuple10 _tmp348=_tmp347;struct Cyc_Absyn_Tqual _tmp34E;void*_tmp34D;_LL333: _tmp34E=_tmp348.f2;_tmp34D=_tmp348.f3;_LL334:;{
struct _tuple10 _tmp349=*((struct _tuple10*)_tmp3C1->hd);struct _tuple10 _tmp34A=_tmp349;struct Cyc_Absyn_Tqual _tmp34C;void*_tmp34B;_LL336: _tmp34C=_tmp34A.f2;_tmp34B=_tmp34A.f3;_LL337:;
Cyc_Tcutil_unify_it(_tmp34D,_tmp34B);
Cyc_Tcutil_unify_tqual(_tmp34E,_tmp34D,_tmp34C,_tmp34B);};}
# 1597
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp3B4 != 0  || _tmp3C1 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp34F="(function types have different number of arguments)";_tag_dyneither(_tmp34F,sizeof(char),52);});
goto _LL2EA;}
# 1603
if(_tmp3B3 != _tmp3C0){
Cyc_Tcutil_failure_reason=({const char*_tmp350="(only one function type takes C varargs)";_tag_dyneither(_tmp350,sizeof(char),41);});
goto _LL2EA;}{
# 1608
int bad_cyc_vararg=0;
{struct _tuple22 _tmp351=({struct _tuple22 _tmp35D;_tmp35D.f1=_tmp3B2;_tmp35D.f2=_tmp3BF;_tmp35D;});struct _tuple22 _tmp352=_tmp351;struct _dyneither_ptr*_tmp35C;struct Cyc_Absyn_Tqual _tmp35B;void*_tmp35A;int _tmp359;struct _dyneither_ptr*_tmp358;struct Cyc_Absyn_Tqual _tmp357;void*_tmp356;int _tmp355;if(_tmp352.f1 == 0){if(_tmp352.f2 == 0){_LL339: _LL33A:
 goto _LL338;}else{_LL33B: _LL33C:
 goto _LL33E;}}else{if(_tmp352.f2 == 0){_LL33D: _LL33E:
# 1613
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp353="(only one function type takes varargs)";_tag_dyneither(_tmp353,sizeof(char),39);});
goto _LL338;}else{_LL33F: _tmp35C=(_tmp352.f1)->name;_tmp35B=(_tmp352.f1)->tq;_tmp35A=(_tmp352.f1)->type;_tmp359=(_tmp352.f1)->inject;_tmp358=(_tmp352.f2)->name;_tmp357=(_tmp352.f2)->tq;_tmp356=(_tmp352.f2)->type;_tmp355=(_tmp352.f2)->inject;_LL340:
# 1617
 Cyc_Tcutil_unify_it(_tmp35A,_tmp356);
Cyc_Tcutil_unify_tqual(_tmp35B,_tmp35A,_tmp357,_tmp356);
if(_tmp359 != _tmp355){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp354="(only one function type injects varargs)";_tag_dyneither(_tmp354,sizeof(char),41);});}
# 1623
goto _LL338;}}_LL338:;}
# 1625
if(bad_cyc_vararg)goto _LL2EA;{
# 1628
int bad_effect=0;
{struct _tuple0 _tmp35E=({struct _tuple0 _tmp360;_tmp360.f1=_tmp3B7;_tmp360.f2=_tmp3C4;_tmp360;});struct _tuple0 _tmp35F=_tmp35E;if(_tmp35F.f1 == 0){if(_tmp35F.f2 == 0){_LL342: _LL343:
 goto _LL341;}else{_LL344: _LL345:
 goto _LL347;}}else{if(_tmp35F.f2 == 0){_LL346: _LL347:
 bad_effect=1;goto _LL341;}else{_LL348: _LL349:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp3B7),(void*)_check_null(_tmp3C4));goto _LL341;}}_LL341:;}
# 1635
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp361="(function effects do not match)";_tag_dyneither(_tmp361,sizeof(char),32);});
goto _LL2EA;}
# 1641
if(!Cyc_Tcutil_same_atts(_tmp3BD,_tmp3B0)){
Cyc_Tcutil_failure_reason=({const char*_tmp362="(function types have different attributes)";_tag_dyneither(_tmp362,sizeof(char),43);});
goto _LL2EA;}
# 1645
if(!Cyc_Tcutil_same_rgn_po(_tmp3BE,_tmp3B1)){
Cyc_Tcutil_failure_reason=({const char*_tmp363="(function types have different region lifetime orderings)";_tag_dyneither(_tmp363,sizeof(char),58);});
goto _LL2EA;}
# 1649
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3BB,_tmp3AE)){
Cyc_Tcutil_failure_reason=({const char*_tmp364="(@requires clauses not equivalent)";_tag_dyneither(_tmp364,sizeof(char),35);});
goto _LL2EA;}
# 1653
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3B9,_tmp3AC)){
Cyc_Tcutil_failure_reason=({const char*_tmp365="(@ensures clauses not equivalent)";_tag_dyneither(_tmp365,sizeof(char),34);});
goto _LL2EA;}
# 1657
return;};};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 10: switch(*((int*)_tmp312.f2)){case 10: _LL309: _tmp3C7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3C6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL30A:
# 1660
 for(0;_tmp3C6 != 0  && _tmp3C7 != 0;(_tmp3C6=_tmp3C6->tl,_tmp3C7=_tmp3C7->tl)){
struct _tuple12 _tmp366=*((struct _tuple12*)_tmp3C6->hd);struct _tuple12 _tmp367=_tmp366;struct Cyc_Absyn_Tqual _tmp36D;void*_tmp36C;_LL34B: _tmp36D=_tmp367.f1;_tmp36C=_tmp367.f2;_LL34C:;{
struct _tuple12 _tmp368=*((struct _tuple12*)_tmp3C7->hd);struct _tuple12 _tmp369=_tmp368;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36A;_LL34E: _tmp36B=_tmp369.f1;_tmp36A=_tmp369.f2;_LL34F:;
Cyc_Tcutil_unify_it(_tmp36C,_tmp36A);
Cyc_Tcutil_unify_tqual(_tmp36D,_tmp36C,_tmp36B,_tmp36A);};}
# 1666
if(_tmp3C6 == 0  && _tmp3C7 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp36E="(tuple types have different numbers of components)";_tag_dyneither(_tmp36E,sizeof(char),51);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 12: switch(*((int*)_tmp312.f2)){case 12: _LL30B: _tmp3CB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3CA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp3C9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f2)->f1;_tmp3C8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f2)->f2;_LL30C:
# 1673
 if(_tmp3C9 != _tmp3CB){Cyc_Tcutil_failure_reason=({const char*_tmp36F="(struct and union type)";_tag_dyneither(_tmp36F,sizeof(char),24);});goto _LL2EA;}
for(0;_tmp3C8 != 0  && _tmp3CA != 0;(_tmp3C8=_tmp3C8->tl,_tmp3CA=_tmp3CA->tl)){
struct Cyc_Absyn_Aggrfield*_tmp370=(struct Cyc_Absyn_Aggrfield*)_tmp3C8->hd;
struct Cyc_Absyn_Aggrfield*_tmp371=(struct Cyc_Absyn_Aggrfield*)_tmp3CA->hd;
if(Cyc_strptrcmp(_tmp370->name,_tmp371->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp372="(different member names)";_tag_dyneither(_tmp372,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1681
Cyc_Tcutil_unify_it(_tmp370->type,_tmp371->type);
Cyc_Tcutil_unify_tqual(_tmp370->tq,_tmp370->type,_tmp371->tq,_tmp371->type);
if(!Cyc_Tcutil_same_atts(_tmp370->attributes,_tmp371->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp373="(different attributes on member)";_tag_dyneither(_tmp373,sizeof(char),33);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1689
if((_tmp370->width != 0  && _tmp371->width == 0  || 
_tmp371->width != 0  && _tmp370->width == 0) || 
(_tmp370->width != 0  && _tmp371->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp370->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp371->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp374="(different bitfield widths on member)";_tag_dyneither(_tmp374,sizeof(char),38);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1698
if((_tmp370->requires_clause != 0  && _tmp371->requires_clause == 0  || 
_tmp370->requires_clause == 0  && _tmp371->requires_clause != 0) || 
(_tmp370->requires_clause == 0  && _tmp371->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp370->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp371->requires_clause))){
# 1703
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp375="(different @requires clauses on member)";_tag_dyneither(_tmp375,sizeof(char),40);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1709
if(_tmp3C8 == 0  && _tmp3CA == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp376="(different number of members)";_tag_dyneither(_tmp376,sizeof(char),30);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 17: switch(*((int*)_tmp312.f2)){case 17: _LL30D: _tmp3CF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp3CE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f1)->f3;_tmp3CD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f2)->f2;_tmp3CC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f2)->f3;_LL30E:
# 1715
 if(_tmp3CE != _tmp3CC){
Cyc_Tcutil_failure_reason=({const char*_tmp377="(different abstract typedefs)";_tag_dyneither(_tmp377,sizeof(char),30);});
goto _LL2EA;}
# 1719
Cyc_Tcutil_failure_reason=({const char*_tmp378="(type parameters to typedef differ)";_tag_dyneither(_tmp378,sizeof(char),36);});
Cyc_Tcutil_unify_list(_tmp3CF,_tmp3CD);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 20: switch(*((int*)_tmp312.f2)){case 20: _LL30F: _LL310:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 21: switch(*((int*)_tmp312.f2)){case 21: _LL311: _LL312:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 22: switch(*((int*)_tmp312.f2)){case 22: _LL313: _LL314:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 15: switch(*((int*)_tmp312.f2)){case 15: _LL315: _tmp3D1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3D0=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL316:
# 1726
 Cyc_Tcutil_unify_it(_tmp3D1,_tmp3D0);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 16: switch(*((int*)_tmp312.f2)){case 16: _LL317: _tmp3D5=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3D4=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp3D3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312.f2)->f1;_tmp3D2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp312.f2)->f2;_LL318:
# 1729
 Cyc_Tcutil_unify_it(_tmp3D5,_tmp3D3);
Cyc_Tcutil_unify_it(_tmp3D4,_tmp3D2);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 24: _LL319: _LL31A:
 goto _LL31C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp312.f2)->tag == 24){_LL31B: _LL31C:
 goto _LL31E;}else{switch(*((int*)_tmp312.f1)){case 23: _LL31D: _LL31E:
 goto _LL320;case 25: _LL31F: _LL320:
 goto _LL322;default: switch(*((int*)_tmp312.f2)){case 25: _LL321: _LL322:
 goto _LL324;case 23: _LL323: _LL324:
# 1738
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp379="(effects don't unify)";_tag_dyneither(_tmp379,sizeof(char),22);});
goto _LL2EA;default: _LL325: _LL326:
 goto _LL2EA;}}}}_LL2EA:;}
# 1743
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1746
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1753
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3D7=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3D8=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3D7,_tmp3D8);}
# 1760
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3D9=x;void*_tmp3E0;switch((((union Cyc_Absyn_Constraint*)_tmp3D9)->Eq_constr).tag){case 3: _LL351: _LL352:
 return - 1;case 1: _LL353: _tmp3E0=(void*)(_tmp3D9->Eq_constr).val;_LL354: {
# 1767
union Cyc_Absyn_Constraint*_tmp3DA=y;void*_tmp3DD;switch((((union Cyc_Absyn_Constraint*)_tmp3DA)->Eq_constr).tag){case 3: _LL358: _LL359:
 return 1;case 1: _LL35A: _tmp3DD=(void*)(_tmp3DA->Eq_constr).val;_LL35B:
 return cmp(_tmp3E0,_tmp3DD);default: _LL35C: _LL35D:
({void*_tmp3DB=0;Cyc_Tcutil_impos(({const char*_tmp3DC="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3DC,sizeof(char),40);}),_tag_dyneither(_tmp3DB,sizeof(void*),0));});}_LL357:;}default: _LL355: _LL356:
# 1772
({void*_tmp3DE=0;Cyc_Tcutil_impos(({const char*_tmp3DF="unify_conref: forward after compress";_tag_dyneither(_tmp3DF,sizeof(char),37);}),_tag_dyneither(_tmp3DE,sizeof(void*),0));});}_LL350:;};}
# 1776
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp3E1=tqt1;struct Cyc_Absyn_Tqual _tmp3E7;void*_tmp3E6;_LL35F: _tmp3E7=_tmp3E1->f1;_tmp3E6=_tmp3E1->f2;_LL360:;{
struct _tuple12*_tmp3E2=tqt2;struct Cyc_Absyn_Tqual _tmp3E5;void*_tmp3E4;_LL362: _tmp3E5=_tmp3E2->f1;_tmp3E4=_tmp3E2->f2;_LL363:;{
int _tmp3E3=Cyc_Tcutil_tqual_cmp(_tmp3E7,_tmp3E5);
if(_tmp3E3 != 0)return _tmp3E3;
return Cyc_Tcutil_typecmp(_tmp3E6,_tmp3E4);};};}
# 1784
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
# 1798
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp3EC=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3EC != 0)return _tmp3EC;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1804
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
# 1841
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp3EE=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp3EE != 0)
return _tmp3EE;{
# 1850
struct _tuple0 _tmp3EF=({struct _tuple0 _tmp452;_tmp452.f1=t2;_tmp452.f2=t1;_tmp452;});struct _tuple0 _tmp3F0=_tmp3EF;struct _dyneither_ptr _tmp451;struct _dyneither_ptr _tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*_tmp44C;void*_tmp44B;void*_tmp44A;void*_tmp449;void*_tmp448;void*_tmp447;void*_tmp446;void*_tmp445;void*_tmp444;enum Cyc_Absyn_AggrKind _tmp443;struct Cyc_List_List*_tmp442;enum Cyc_Absyn_AggrKind _tmp441;struct Cyc_List_List*_tmp440;struct Cyc_List_List*_tmp43F;struct Cyc_List_List*_tmp43E;void*_tmp43D;struct Cyc_Absyn_Tqual _tmp43C;struct Cyc_Absyn_Exp*_tmp43B;union Cyc_Absyn_Constraint*_tmp43A;void*_tmp439;struct Cyc_Absyn_Tqual _tmp438;struct Cyc_Absyn_Exp*_tmp437;union Cyc_Absyn_Constraint*_tmp436;int _tmp435;int _tmp434;enum Cyc_Absyn_Sign _tmp433;enum Cyc_Absyn_Size_of _tmp432;enum Cyc_Absyn_Sign _tmp431;enum Cyc_Absyn_Size_of _tmp430;void*_tmp42F;struct Cyc_Absyn_Tqual _tmp42E;void*_tmp42D;union Cyc_Absyn_Constraint*_tmp42C;union Cyc_Absyn_Constraint*_tmp42B;union Cyc_Absyn_Constraint*_tmp42A;void*_tmp429;struct Cyc_Absyn_Tqual _tmp428;void*_tmp427;union Cyc_Absyn_Constraint*_tmp426;union Cyc_Absyn_Constraint*_tmp425;union Cyc_Absyn_Constraint*_tmp424;struct Cyc_Absyn_Datatypedecl*_tmp423;struct Cyc_Absyn_Datatypefield*_tmp422;struct Cyc_List_List*_tmp421;struct Cyc_Absyn_Datatypedecl*_tmp420;struct Cyc_Absyn_Datatypefield*_tmp41F;struct Cyc_List_List*_tmp41E;struct Cyc_Absyn_Datatypedecl*_tmp41D;struct Cyc_List_List*_tmp41C;struct Cyc_Absyn_Datatypedecl*_tmp41B;struct Cyc_List_List*_tmp41A;struct Cyc_List_List*_tmp419;struct Cyc_List_List*_tmp418;struct _tuple2*_tmp417;struct _tuple2*_tmp416;union Cyc_Absyn_AggrInfoU _tmp415;struct Cyc_List_List*_tmp414;union Cyc_Absyn_AggrInfoU _tmp413;struct Cyc_List_List*_tmp412;struct Cyc_Absyn_Tvar*_tmp411;struct Cyc_Absyn_Tvar*_tmp410;switch(*((int*)_tmp3F0.f1)){case 1: switch(*((int*)_tmp3F0.f2)){case 1: _LL3A2: _LL3A3:
# 1852
({void*_tmp3F1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3F2="typecmp: can only compare closed types";_tag_dyneither(_tmp3F2,sizeof(char),39);}),_tag_dyneither(_tmp3F1,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 2: switch(*((int*)_tmp3F0.f2)){case 2: _LL3A4: _tmp411=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp410=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3A5:
# 1856
 return Cyc_Core_intcmp(_tmp410->identity,_tmp411->identity);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 11: switch(*((int*)_tmp3F0.f2)){case 11: _LL3A6: _tmp415=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F0.f1)->f1).aggr_info;_tmp414=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F0.f1)->f1).targs;_tmp413=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F0.f2)->f1).aggr_info;_tmp412=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F0.f2)->f1).targs;_LL3A7: {
# 1860
struct _tuple13 _tmp3F3=Cyc_Absyn_aggr_kinded_name(_tmp415);struct _tuple13 _tmp3F4=_tmp3F3;struct _tuple2*_tmp3F9;_LL3D9: _tmp3F9=_tmp3F4.f2;_LL3DA:;{
struct _tuple13 _tmp3F5=Cyc_Absyn_aggr_kinded_name(_tmp413);struct _tuple13 _tmp3F6=_tmp3F5;struct _tuple2*_tmp3F8;_LL3DC: _tmp3F8=_tmp3F6.f2;_LL3DD:;{
int _tmp3F7=Cyc_Absyn_qvar_cmp(_tmp3F9,_tmp3F8);
if(_tmp3F7 != 0)return _tmp3F7;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp414,_tmp412);}};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 13: switch(*((int*)_tmp3F0.f2)){case 13: _LL3A8: _tmp417=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp416=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3A9:
# 1867
 return Cyc_Absyn_qvar_cmp(_tmp417,_tmp416);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 14: switch(*((int*)_tmp3F0.f2)){case 14: _LL3AA: _tmp419=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp418=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3AB:
# 1870
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp419,_tmp418);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp3F0.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3AC: _tmp41D=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f1)->f1).datatype_info).KnownDatatype).val;_tmp41C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f1)->f1).targs;_tmp41B=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f2)->f1).datatype_info).KnownDatatype).val;_tmp41A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F0.f2)->f1).targs;_LL3AD:
# 1874
 if(_tmp41B == _tmp41D)return 0;{
int _tmp3FA=Cyc_Absyn_qvar_cmp(_tmp41B->name,_tmp41D->name);
if(_tmp3FA != 0)return _tmp3FA;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp41A,_tmp41C);};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp3F0.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp3F0.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL3AE: _tmp423=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp422=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp421=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f1)->f1).targs;_tmp420=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp41F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp41E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F0.f2)->f1).targs;_LL3AF:
# 1881
 if(_tmp420 == _tmp423)return 0;{
int _tmp3FB=Cyc_Absyn_qvar_cmp(_tmp423->name,_tmp420->name);
if(_tmp3FB != 0)return _tmp3FB;{
int _tmp3FC=Cyc_Absyn_qvar_cmp(_tmp422->name,_tmp41F->name);
if(_tmp3FC != 0)return _tmp3FC;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp41E,_tmp421);};};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp3F0.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 5: switch(*((int*)_tmp3F0.f2)){case 5: _LL3B0: _tmp42F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).elt_typ;_tmp42E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).elt_tq;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).ptr_atts).rgn;_tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).ptr_atts).nullable;_tmp42B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).ptr_atts).bounds;_tmp42A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f1)->f1).ptr_atts).zero_term;_tmp429=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).elt_typ;_tmp428=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).elt_tq;_tmp427=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).ptr_atts).rgn;_tmp426=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).ptr_atts).nullable;_tmp425=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).ptr_atts).bounds;_tmp424=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F0.f2)->f1).ptr_atts).zero_term;_LL3B1: {
# 1890
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
# 1904
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp426,_tmp42C);};};};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 6: switch(*((int*)_tmp3F0.f2)){case 6: _LL3B2: _tmp433=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp432=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f1)->f2;_tmp431=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_tmp430=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F0.f2)->f2;_LL3B3:
# 1907
 if(_tmp431 != _tmp433)return Cyc_Core_intcmp((int)((unsigned int)_tmp431),(int)((unsigned int)_tmp433));
if(_tmp430 != _tmp432)return Cyc_Core_intcmp((int)((unsigned int)_tmp430),(int)((unsigned int)_tmp432));
return 0;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 7: switch(*((int*)_tmp3F0.f2)){case 7: _LL3B4: _tmp435=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp434=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3B5:
# 1912
 if(_tmp435 == _tmp434)return 0;else{
if(_tmp434 == 0)return - 1;else{
if(_tmp434 == 1  && _tmp435 == 0)return - 1;else{
return 1;}}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 8: switch(*((int*)_tmp3F0.f2)){case 8: _LL3B6: _tmp43D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f1)->f1).elt_type;_tmp43C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f1)->f1).tq;_tmp43B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f1)->f1).num_elts;_tmp43A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f1)->f1).zero_term;_tmp439=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f2)->f1).elt_type;_tmp438=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f2)->f1).tq;_tmp437=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f2)->f1).num_elts;_tmp436=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F0.f2)->f1).zero_term;_LL3B7: {
# 1919
int _tmp404=Cyc_Tcutil_tqual_cmp(_tmp438,_tmp43C);
if(_tmp404 != 0)return _tmp404;{
int _tmp405=Cyc_Tcutil_typecmp(_tmp439,_tmp43D);
if(_tmp405 != 0)return _tmp405;{
int _tmp406=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp43A,_tmp436);
if(_tmp406 != 0)return _tmp406;
if(_tmp43B == _tmp437)return 0;
if(_tmp43B == 0  || _tmp437 == 0)
({void*_tmp407=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp408="missing expression in array index";_tag_dyneither(_tmp408,sizeof(char),34);}),_tag_dyneither(_tmp407,sizeof(void*),0));});
# 1929
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp43B,_tmp437);};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 9: switch(*((int*)_tmp3F0.f2)){case 9: _LL3B8: _LL3B9:
# 1932
({void*_tmp409=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp40A="typecmp: function types not handled";_tag_dyneither(_tmp40A,sizeof(char),36);}),_tag_dyneither(_tmp409,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 10: switch(*((int*)_tmp3F0.f2)){case 10: _LL3BA: _tmp43F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp43E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3BB:
# 1935
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp43E,_tmp43F);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 12: switch(*((int*)_tmp3F0.f2)){case 12: _LL3BC: _tmp443=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp442=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F0.f1)->f2;_tmp441=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_tmp440=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F0.f2)->f2;_LL3BD:
# 1938
 if(_tmp441 != _tmp443){
if(_tmp441 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp440,_tmp442);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 15: switch(*((int*)_tmp3F0.f2)){case 15: _LL3BE: _tmp445=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp444=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3BF:
# 1943
 return Cyc_Tcutil_typecmp(_tmp445,_tmp444);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 16: switch(*((int*)_tmp3F0.f2)){case 16: _LL3C0: _tmp449=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp448=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F0.f1)->f2;_tmp447=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_tmp446=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F0.f2)->f2;_LL3C1: {
# 1945
int _tmp40B=Cyc_Tcutil_typecmp(_tmp449,_tmp447);
if(_tmp40B != 0)return _tmp40B;else{
return Cyc_Tcutil_typecmp(_tmp448,_tmp446);}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 19: switch(*((int*)_tmp3F0.f2)){case 19: _LL3C2: _tmp44B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp44A=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3C3:
 return Cyc_Tcutil_typecmp(_tmp44B,_tmp44A);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 18: switch(*((int*)_tmp3F0.f2)){case 18: _LL3C4: _tmp44D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp44C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3C5:
 _tmp44F=_tmp44D;_tmp44E=_tmp44C;goto _LL3C7;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 27: switch(*((int*)_tmp3F0.f2)){case 27: _LL3C6: _tmp44F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp44E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3C7:
# 1951
 return Cyc_Evexp_const_exp_cmp(_tmp44F,_tmp44E);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 28: switch(*((int*)_tmp3F0.f2)){case 28: _LL3C8: _tmp451=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp450=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3C9:
# 1953
 return Cyc_strcmp((struct _dyneither_ptr)_tmp451,(struct _dyneither_ptr)_tmp450);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 24: _LL3CA: _LL3CB:
 goto _LL3CD;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3F0.f2)->tag == 24){_LL3CC: _LL3CD:
 goto _LL3CF;}else{switch(*((int*)_tmp3F0.f1)){case 23: _LL3CE: _LL3CF:
 goto _LL3D1;case 25: _LL3D0: _LL3D1:
 goto _LL3D3;default: switch(*((int*)_tmp3F0.f2)){case 25: _LL3D2: _LL3D3:
 goto _LL3D5;case 23: _LL3D4: _LL3D5:
({void*_tmp40C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp40D="typecmp: effects not handled";_tag_dyneither(_tmp40D,sizeof(char),29);}),_tag_dyneither(_tmp40C,sizeof(void*),0));});default: _LL3D6: _LL3D7:
({void*_tmp40E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp40F="Unmatched case in typecmp";_tag_dyneither(_tmp40F,sizeof(char),26);}),_tag_dyneither(_tmp40E,sizeof(void*),0));});}}}}_LL3A1:;};};}
# 1964
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp453=Cyc_Tcutil_compress(t);void*_tmp454=_tmp453;switch(*((int*)_tmp454)){case 6: _LL3E4: _LL3E5:
 goto _LL3E7;case 7: _LL3E6: _LL3E7:
 goto _LL3E9;case 13: _LL3E8: _LL3E9:
 goto _LL3EB;case 14: _LL3EA: _LL3EB:
 return 1;default: _LL3EC: _LL3ED:
 return 0;}_LL3E3:;}
# 1976
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp455=({struct _tuple0 _tmp459;_tmp459.f1=t1;_tmp459.f2=t2;_tmp459;});struct _tuple0 _tmp456=_tmp455;int _tmp458;int _tmp457;switch(*((int*)_tmp456.f1)){case 7: switch(*((int*)_tmp456.f2)){case 7: _LL3EF: _tmp458=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp456.f1)->f1;_tmp457=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp456.f2)->f1;_LL3F0:
# 1981
 return _tmp457 == 0  && _tmp458 != 0  || (_tmp457 == 1  && _tmp458 != 0) && _tmp458 != 1;case 6: _LL3F1: _LL3F2:
 goto _LL3F4;case 19: _LL3F3: _LL3F4:
 return 1;default: goto _LL40F;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL3F5: _LL3F6:
 return 0;}else{goto _LL3F7;}}else{_LL3F7: _LL3F8:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp456.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2){case Cyc_Absyn_Int_sz: _LL3F9: _LL3FA:
# 1988
 goto _LL3FC;case Cyc_Absyn_Short_sz: _LL401: _LL402:
# 1993
 goto _LL404;case Cyc_Absyn_Char_sz: _LL407: _LL408:
# 1996
 goto _LL40A;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp456.f2)->f1 == 0){_LL3FD: _LL3FE:
# 1991
 goto _LL400;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp456.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2){case Cyc_Absyn_Long_sz: _LL3FB: _LL3FC:
# 1989
 return 0;case Cyc_Absyn_Short_sz: _LL403: _LL404:
# 1994
 goto _LL406;case Cyc_Absyn_Char_sz: _LL409: _LL40A:
# 1997
 goto _LL40C;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp456.f2)->f1 == 0){_LL3FF: _LL400:
# 1992
 goto _LL402;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2 == Cyc_Absyn_Char_sz){_LL40B: _LL40C:
# 1998
 goto _LL40E;}else{goto _LL40F;}}else{goto _LL40F;}default: goto _LL40F;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp456.f2)->f2){case Cyc_Absyn_Short_sz: _LL405: _LL406:
# 1995
 goto _LL408;case Cyc_Absyn_Char_sz: _LL40D: _LL40E:
# 1999
 return 1;default: goto _LL40F;}else{goto _LL40F;}default: _LL40F: _LL410:
# 2001
 return 0;}_LL3EE:;};}
# 2007
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2010
struct _RegionHandle*_tmp45A=Cyc_Core_heap_region;{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp45B=_region_malloc(_tmp45A,sizeof(*_tmp45B));_tmp45B->v=t1;_tmp45B;});}}}
# 2020
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}}
# 2025
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp45F;_tmp45F.tag=0;_tmp45F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp45E;_tmp45E.tag=0;_tmp45E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp45C[2]={& _tmp45E,& _tmp45F};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp45D="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp45D,sizeof(char),41);}),_tag_dyneither(_tmp45C,sizeof(void*),2));});});});
return 0;}}}
# 2031
return 1;}
# 2036
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp460=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp461=_tmp460;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp461)->tag == 5){_LL412: _LL413:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL411;}else{_LL414: _LL415:
 return 0;}_LL411:;}
# 2042
return 1;}
# 2045
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp462=Cyc_Tcutil_compress(t);void*_tmp463=_tmp462;switch(*((int*)_tmp463)){case 6: _LL417: _LL418:
 goto _LL41A;case 19: _LL419: _LL41A:
 goto _LL41C;case 13: _LL41B: _LL41C:
 goto _LL41E;case 14: _LL41D: _LL41E:
 return 1;default: _LL41F: _LL420:
 return 0;}_LL416:;}
# 2056
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2060
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp464=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp465="integral size mismatch; conversion supplied";_tag_dyneither(_tmp465,sizeof(char),44);}),_tag_dyneither(_tmp464,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2066
return 0;}
# 2070
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2074
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp466=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp467="integral size mismatch; conversion supplied";_tag_dyneither(_tmp467,sizeof(char),44);}),_tag_dyneither(_tmp466,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2080
return 0;}
# 2085
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2093
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2095
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp468=({struct _tuple0 _tmp481;_tmp481.f1=t1;_tmp481.f2=t2;_tmp481;});struct _tuple0 _tmp469=_tmp468;void*_tmp480;struct Cyc_Absyn_Tqual _tmp47F;struct Cyc_Absyn_Exp*_tmp47E;union Cyc_Absyn_Constraint*_tmp47D;void*_tmp47C;struct Cyc_Absyn_Tqual _tmp47B;struct Cyc_Absyn_Exp*_tmp47A;union Cyc_Absyn_Constraint*_tmp479;struct Cyc_Absyn_PtrInfo _tmp478;struct Cyc_Absyn_PtrInfo _tmp477;switch(*((int*)_tmp469.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f2)->tag == 5){_LL422: _tmp478=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f1)->f1;_tmp477=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f2)->f1;_LL423: {
# 2099
int okay=1;
# 2101
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp478.ptr_atts).nullable,(_tmp477.ptr_atts).nullable))
# 2104
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp478.ptr_atts).nullable);
# 2106
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp478.ptr_atts).bounds,(_tmp477.ptr_atts).bounds)){
# 2109
struct _tuple0 _tmp46A=({struct _tuple0 _tmp471;_tmp471.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp478.ptr_atts).bounds);_tmp471.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp477.ptr_atts).bounds);_tmp471;});
# 2109
struct _tuple0 _tmp46B=_tmp46A;struct Cyc_Absyn_Exp*_tmp470;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_Absyn_Exp*_tmp46E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46B.f2)->tag == 0){_LL42B: _LL42C:
# 2111
 goto _LL42E;}else{_LL42F: _tmp46F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f1)->f1;_tmp46E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f2)->f1;_LL430:
# 2114
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp46E,_tmp46F);
# 2118
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp477.ptr_atts).zero_term))
({void*_tmp46C=0;Cyc_Tcutil_warn(loc,({const char*_tmp46D="implicit cast to shorter array";_tag_dyneither(_tmp46D,sizeof(char),31);}),_tag_dyneither(_tmp46C,sizeof(void*),0));});
goto _LL42A;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46B.f2)->tag == 0){_LL42D: _LL42E:
# 2112
 okay=1;goto _LL42A;}else{_LL431: _tmp470=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f2)->f1;_LL432:
# 2123
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp478.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp477.ptr_atts).bounds))
goto _LL42A;
okay=0;
goto _LL42A;}}_LL42A:;}
# 2132
okay=okay  && (!(_tmp478.elt_tq).real_const  || (_tmp477.elt_tq).real_const);
# 2135
if(!Cyc_Tcutil_unify((_tmp478.ptr_atts).rgn,(_tmp477.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp478.ptr_atts).rgn,(_tmp477.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp475;_tmp475.tag=0;_tmp475.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2140
Cyc_Absynpp_typ2string((_tmp477.ptr_atts).rgn));({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0;_tmp474.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2139
Cyc_Absynpp_typ2string((_tmp478.ptr_atts).rgn));({void*_tmp472[2]={& _tmp474,& _tmp475};Cyc_Tcutil_warn(loc,({const char*_tmp473="implicit cast from region %s to region %s";_tag_dyneither(_tmp473,sizeof(char),42);}),_tag_dyneither(_tmp472,sizeof(void*),2));});});});}else{
# 2141
okay=0;}}
# 2145
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp478.ptr_atts).zero_term,(_tmp477.ptr_atts).zero_term) || 
# 2148
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp478.ptr_atts).zero_term) && (_tmp477.elt_tq).real_const);{
# 2156
int _tmp476=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp477.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2159
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp477.ptr_atts).zero_term);
# 2163
okay=okay  && (Cyc_Tcutil_unify(_tmp478.elt_typ,_tmp477.elt_typ) || 
(_tmp476  && ((_tmp477.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp477.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp478.elt_typ,_tmp477.elt_typ));
# 2166
return okay;};}}else{goto _LL428;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->tag == 8){_LL424: _tmp480=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).elt_type;_tmp47F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).tq;_tmp47E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).num_elts;_tmp47D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).zero_term;_tmp47C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).elt_type;_tmp47B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).tq;_tmp47A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).num_elts;_tmp479=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).zero_term;_LL425: {
# 2170
int okay;
# 2172
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp47D,_tmp479) && (
(_tmp47E != 0  && _tmp47A != 0) && Cyc_Evexp_same_const_exp(_tmp47E,_tmp47A));
# 2175
return(okay  && Cyc_Tcutil_unify(_tmp480,_tmp47C)) && (!_tmp47F.real_const  || _tmp47B.real_const);}}else{goto _LL428;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp469.f2)->tag == 6){_LL426: _LL427:
# 2177
 return 0;}else{goto _LL428;}default: _LL428: _LL429:
# 2179
 return Cyc_Tcutil_unify(t1,t2);}_LL421:;};}
# 2183
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp482=Cyc_Tcutil_compress(t);void*_tmp483=_tmp482;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp483)->tag == 5){_LL434: _LL435:
 return 1;}else{_LL436: _LL437:
 return 0;}_LL433:;}
# 2189
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp484=Cyc_Tcutil_compress(t);void*_tmp485=_tmp484;void*_tmp488;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp485)->tag == 5){_LL439: _tmp488=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp485)->f1).elt_typ;_LL43A:
 return _tmp488;}else{_LL43B: _LL43C:
({void*_tmp486=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp487="pointer_elt_type";_tag_dyneither(_tmp487,sizeof(char),17);}),_tag_dyneither(_tmp486,sizeof(void*),0));});}_LL438:;}
# 2195
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp489=Cyc_Tcutil_compress(t);void*_tmp48A=_tmp489;struct Cyc_Absyn_PtrAtts*_tmp48D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48A)->tag == 5){_LL43E: _tmp48D=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48A)->f1).ptr_atts;_LL43F:
 return _tmp48D->rgn;}else{_LL440: _LL441:
({void*_tmp48B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp48C="pointer_elt_type";_tag_dyneither(_tmp48C,sizeof(char),17);}),_tag_dyneither(_tmp48B,sizeof(void*),0));});}_LL43D:;}
# 2202
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp48E=Cyc_Tcutil_compress(t);void*_tmp48F=_tmp48E;void*_tmp490;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48F)->tag == 5){_LL443: _tmp490=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48F)->f1).ptr_atts).rgn;_LL444:
# 2205
*rgn=_tmp490;
return 1;}else{_LL445: _LL446:
 return 0;}_LL442:;}
# 2212
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2216
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp491=Cyc_Tcutil_compress(t);void*_tmp492=_tmp491;union Cyc_Absyn_Constraint*_tmp493;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp492)->tag == 5){_LL448: _tmp493=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp492)->f1).ptr_atts).bounds;_LL449:
# 2219
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp493)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL44A: _LL44B:
 return 0;}_LL447:;}
# 2225
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp494=e->r;void*_tmp495=_tmp494;void*_tmp499;struct Cyc_Absyn_Exp*_tmp498;struct _dyneither_ptr _tmp497;switch(*((int*)_tmp495)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Int_c).val).f2 == 0){_LL44D: _LL44E:
 goto _LL450;}else{goto _LL459;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Char_c).val).f2 == 0){_LL44F: _LL450:
 goto _LL452;}else{goto _LL459;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Short_c).val).f2 == 0){_LL451: _LL452:
 goto _LL454;}else{goto _LL459;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).LongLong_c).val).f2 == 0){_LL453: _LL454:
# 2231
 return 1;}else{goto _LL459;}case 3: _LL455: _tmp497=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Wchar_c).val;_LL456: {
# 2233
unsigned long _tmp496=Cyc_strlen((struct _dyneither_ptr)_tmp497);
int i=0;
if(_tmp496 >= 2  && *((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),1))== 'x'  && _tmp496 >= 3) && *((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp496;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2243
return 0;}}default: goto _LL459;}case 13: _LL457: _tmp499=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp498=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LL458:
 return Cyc_Tcutil_is_zero(_tmp498) && Cyc_Tcutil_admits_zero(_tmp499);default: _LL459: _LL45A:
 return 0;}_LL44C:;}
# 2249
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2256
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2261
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2266
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2273
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2278
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2283
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp49A=ka;enum Cyc_Absyn_KindQual _tmp4A3;enum Cyc_Absyn_AliasQual _tmp4A2;_LL45C: _tmp4A3=_tmp49A->kind;_tmp4A2=_tmp49A->aliasqual;_LL45D:;
{enum Cyc_Absyn_AliasQual _tmp49B=_tmp4A2;switch(_tmp49B){case Cyc_Absyn_Aliasable: _LL45F: _LL460: {
# 2287
enum Cyc_Absyn_KindQual _tmp49C=_tmp4A3;switch(_tmp49C){case Cyc_Absyn_AnyKind: _LL466: _LL467:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL468: _LL469:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL46A: _LL46B:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL46C: _LL46D:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL46E: _LL46F:
 return& Cyc_Tcutil_eko;default: _LL470: _LL471:
 return& Cyc_Tcutil_iko;}_LL465:;}case Cyc_Absyn_Unique: _LL461: _LL462:
# 2296
{enum Cyc_Absyn_KindQual _tmp49D=_tmp4A3;switch(_tmp49D){case Cyc_Absyn_AnyKind: _LL473: _LL474:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL475: _LL476:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL477: _LL478:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL479: _LL47A:
 return& Cyc_Tcutil_urko;default: _LL47B: _LL47C:
 goto _LL472;}_LL472:;}
# 2303
goto _LL45E;default: _LL463: _LL464:
# 2305
{enum Cyc_Absyn_KindQual _tmp49E=_tmp4A3;switch(_tmp49E){case Cyc_Absyn_AnyKind: _LL47E: _LL47F:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL480: _LL481:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL482: _LL483:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL484: _LL485:
 return& Cyc_Tcutil_trko;default: _LL486: _LL487:
 goto _LL47D;}_LL47D:;}
# 2312
goto _LL45E;}_LL45E:;}
# 2314
({struct Cyc_String_pa_PrintArg_struct _tmp4A1;_tmp4A1.tag=0;_tmp4A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp49F[1]={& _tmp4A1};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A0="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4A0,sizeof(char),26);}),_tag_dyneither(_tmp49F,sizeof(void*),1));});});}
# 2317
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2325
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2332
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2337
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2342
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2347
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2352
struct Cyc_Absyn_Kind*_tmp4A4=ka;enum Cyc_Absyn_KindQual _tmp4AD;enum Cyc_Absyn_AliasQual _tmp4AC;_LL489: _tmp4AD=_tmp4A4->kind;_tmp4AC=_tmp4A4->aliasqual;_LL48A:;
{enum Cyc_Absyn_AliasQual _tmp4A5=_tmp4AC;switch(_tmp4A5){case Cyc_Absyn_Aliasable: _LL48C: _LL48D: {
# 2355
enum Cyc_Absyn_KindQual _tmp4A6=_tmp4AD;switch(_tmp4A6){case Cyc_Absyn_AnyKind: _LL493: _LL494:
 return& ab;case Cyc_Absyn_MemKind: _LL495: _LL496:
 return& mb;case Cyc_Absyn_BoxKind: _LL497: _LL498:
 return& bb;case Cyc_Absyn_RgnKind: _LL499: _LL49A:
 return& rb;case Cyc_Absyn_EffKind: _LL49B: _LL49C:
 return& eb;default: _LL49D: _LL49E:
 return& ib;}_LL492:;}case Cyc_Absyn_Unique: _LL48E: _LL48F:
# 2364
{enum Cyc_Absyn_KindQual _tmp4A7=_tmp4AD;switch(_tmp4A7){case Cyc_Absyn_AnyKind: _LL4A0: _LL4A1:
 return& uab;case Cyc_Absyn_MemKind: _LL4A2: _LL4A3:
 return& umb;case Cyc_Absyn_BoxKind: _LL4A4: _LL4A5:
 return& ubb;case Cyc_Absyn_RgnKind: _LL4A6: _LL4A7:
 return& urb;default: _LL4A8: _LL4A9:
 goto _LL49F;}_LL49F:;}
# 2371
goto _LL48B;default: _LL490: _LL491:
# 2373
{enum Cyc_Absyn_KindQual _tmp4A8=_tmp4AD;switch(_tmp4A8){case Cyc_Absyn_AnyKind: _LL4AB: _LL4AC:
 return& tab;case Cyc_Absyn_MemKind: _LL4AD: _LL4AE:
 return& tmb;case Cyc_Absyn_BoxKind: _LL4AF: _LL4B0:
 return& tbb;case Cyc_Absyn_RgnKind: _LL4B1: _LL4B2:
 return& trb;default: _LL4B3: _LL4B4:
 goto _LL4AA;}_LL4AA:;}
# 2380
goto _LL48B;}_LL48B:;}
# 2382
({struct Cyc_String_pa_PrintArg_struct _tmp4AB;_tmp4AB.tag=0;_tmp4AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4A9[1]={& _tmp4AB};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4AA="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4AA,sizeof(char),24);}),_tag_dyneither(_tmp4A9,sizeof(void*),1));});});}
# 2385
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2388
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2390
return({struct Cyc_Core_Opt*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->v=Cyc_Tcutil_kind_to_bound(k);_tmp4BC;});}
# 2395
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
e1->r=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4BE;_tmp4BE.tag=0;_tmp4BE.f1=Cyc_Absyn_Null_c;_tmp4BE;});_tmp4BD;});{
struct Cyc_Core_Opt*_tmp4BF=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C0=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4C6;_tmp4C6.tag=5;_tmp4C6.f1=({struct Cyc_Absyn_PtrInfo _tmp4C7;_tmp4C7.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4BF);_tmp4C7.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp4C7.ptr_atts=({(_tmp4C7.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4BF);(_tmp4C7.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4C7.ptr_atts).bounds=
# 2403
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4C7.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4C7.ptr_atts).ptrloc=0;_tmp4C7.ptr_atts;});_tmp4C7;});_tmp4C6;});_tmp4C5;});
e1->topt=(void*)_tmp4C0;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4C4;_tmp4C4.tag=0;_tmp4C4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));({struct Cyc_String_pa_PrintArg_struct _tmp4C3;_tmp4C3.tag=0;_tmp4C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp4C1[2]={& _tmp4C3,& _tmp4C4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4C2="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4C2,sizeof(char),56);}),_tag_dyneither(_tmp4C1,sizeof(void*),2));});});});
return retv;};};}
# 2413
return 0;}
# 2416
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4C8=c;switch(_tmp4C8){case Cyc_Absyn_Unknown_coercion: _LL4B6: _LL4B7:
 return({const char*_tmp4C9="unknown";_tag_dyneither(_tmp4C9,sizeof(char),8);});case Cyc_Absyn_No_coercion: _LL4B8: _LL4B9:
 return({const char*_tmp4CA="no coercion";_tag_dyneither(_tmp4CA,sizeof(char),12);});case Cyc_Absyn_Null_to_NonNull: _LL4BA: _LL4BB:
 return({const char*_tmp4CB="null check";_tag_dyneither(_tmp4CB,sizeof(char),11);});default: _LL4BC: _LL4BD:
 return({const char*_tmp4CC="other coercion";_tag_dyneither(_tmp4CC,sizeof(char),15);});}_LL4B5:;}
# 2425
int Cyc_Tcutil_warn_alias_coerce=0;
# 2431
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2435
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->f1=Cyc_Absyn_Loc_n;_tmp4E0->f2=({struct _dyneither_ptr*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4E4;_tmp4E4.tag=1;_tmp4E4.f1=(unsigned long)counter ++;({void*_tmp4E2[1]={& _tmp4E4};Cyc_aprintf(({const char*_tmp4E3="__aliasvar%d";_tag_dyneither(_tmp4E3,sizeof(char),13);}),_tag_dyneither(_tmp4E2,sizeof(void*),1));});});_tmp4E1;});_tmp4E0;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0,v,e_typ,e);
struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));_tmp4DE[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4DF;_tmp4DF.tag=4;_tmp4DF.f1=vd;_tmp4DF;});_tmp4DE;}),e->loc);
# 2444
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2446
{void*_tmp4CD=Cyc_Tcutil_compress(e_typ);void*_tmp4CE=_tmp4CD;void*_tmp4DC;struct Cyc_Absyn_Tqual _tmp4DB;void*_tmp4DA;union Cyc_Absyn_Constraint*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4D8;union Cyc_Absyn_Constraint*_tmp4D7;struct Cyc_Absyn_PtrLoc*_tmp4D6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->tag == 5){_LL4BF: _tmp4DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).elt_typ;_tmp4DB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).elt_tq;_tmp4DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).rgn;_tmp4D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).nullable;_tmp4D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).bounds;_tmp4D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).zero_term;_tmp4D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).ptrloc;_LL4C0:
# 2448
{void*_tmp4CF=Cyc_Tcutil_compress(_tmp4DA);void*_tmp4D0=_tmp4CF;void**_tmp4D5;struct Cyc_Core_Opt*_tmp4D4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D0)->tag == 1){_LL4C4: _tmp4D5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D0)->f2;_tmp4D4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D0)->f4;_LL4C5: {
# 2450
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D1=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));_tmp4D2[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4D3;_tmp4D3.tag=2;_tmp4D3.f1=tv;_tmp4D3;});_tmp4D2;});
*_tmp4D5=(void*)_tmp4D1;
goto _LL4C3;}}else{_LL4C6: _LL4C7:
 goto _LL4C3;}_LL4C3:;}
# 2455
goto _LL4BE;}else{_LL4C1: _LL4C2:
 goto _LL4BE;}_LL4BE:;}
# 2459
e->topt=0;
vd->initializer=0;{
# 2463
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2465
return({struct _tuple15 _tmp4DD;_tmp4DD.f1=d;_tmp4DD.f2=ve;_tmp4DD;});};}
# 2470
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2473
struct _RegionHandle _tmp4E6=_new_region("r");struct _RegionHandle*r=& _tmp4E6;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2478
void*_tmp4E7=Cyc_Tcutil_compress(wants_typ);void*_tmp4E8=_tmp4E7;void*_tmp4F3;switch(*((int*)_tmp4E8)){case 5: _LL4C9: _tmp4F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E8)->f1).ptr_atts).rgn;_LL4CA: {
# 2480
void*_tmp4E9=Cyc_Tcutil_compress(_tmp4F3);void*_tmp4EA=_tmp4E9;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4EA)->tag == 20){_LL4D0: _LL4D1: {
int _tmp4EB=0;_npop_handler(0);return _tmp4EB;}}else{_LL4D2: _LL4D3: {
# 2483
struct Cyc_Absyn_Kind*_tmp4EC=Cyc_Tcutil_typ_kind(_tmp4F3);
int _tmp4ED=_tmp4EC->kind == Cyc_Absyn_RgnKind  && _tmp4EC->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp4ED;}}_LL4CF:;}case 17: _LL4CB: _LL4CC:
# 2487
({struct Cyc_String_pa_PrintArg_struct _tmp4F1;_tmp4F1.tag=0;_tmp4F1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc));({struct Cyc_String_pa_PrintArg_struct _tmp4F0;_tmp4F0.tag=0;_tmp4F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ)));({void*_tmp4EE[2]={& _tmp4F0,& _tmp4F1};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4EF="got typedef %s in can_insert_alias at %s\n";_tag_dyneither(_tmp4EF,sizeof(char),42);}),_tag_dyneither(_tmp4EE,sizeof(void*),2));});});});default: _LL4CD: _LL4CE: {
int _tmp4F2=0;_npop_handler(0);return _tmp4F2;}}_LL4C8:;}{
# 2492
int _tmp4F4=0;_npop_handler(0);return _tmp4F4;};
# 2474
;_pop_region(r);}
# 2496
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2501
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2503
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2505
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0;_tmp4F8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0;_tmp4F7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp4F5[2]={& _tmp4F7,& _tmp4F8};Cyc_Tcutil_warn(e->loc,({const char*_tmp4F6="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp4F6,sizeof(char),53);}),_tag_dyneither(_tmp4F5,sizeof(void*),2));});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2514
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0;_tmp4FD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp4FC;_tmp4FC.tag=0;_tmp4FC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_PrintArg_struct _tmp4FB;_tmp4FB.tag=0;_tmp4FB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp4F9[3]={& _tmp4FB,& _tmp4FC,& _tmp4FD};Cyc_Tcutil_warn(e->loc,({const char*_tmp4FA="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp4FA,sizeof(char),40);}),_tag_dyneither(_tmp4F9,sizeof(void*),3));});});});});
*alias_coercion=1;}
# 2521
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2529
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0;_tmp501.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0;_tmp500.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp4FE[2]={& _tmp500,& _tmp501};Cyc_Tcutil_warn(e->loc,({const char*_tmp4FF="implicit cast from %s to %s";_tag_dyneither(_tmp4FF,sizeof(char),28);}),_tag_dyneither(_tmp4FE,sizeof(void*),2));});});});
return 1;}else{
# 2535
return 0;}}}}}
# 2542
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2545
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2549
int Cyc_Tcutil_coerceable(void*t){
void*_tmp502=Cyc_Tcutil_compress(t);void*_tmp503=_tmp502;switch(*((int*)_tmp503)){case 6: _LL4D5: _LL4D6:
 goto _LL4D8;case 7: _LL4D7: _LL4D8:
 return 1;default: _LL4D9: _LL4DA:
 return 0;}_LL4D4:;}
# 2567 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2571
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2574
struct _tuple23 _tmp504=*env;struct _tuple23 _tmp505=_tmp504;struct Cyc_List_List*_tmp50D;struct _RegionHandle*_tmp50C;struct Cyc_Tcenv_Tenv*_tmp50B;int _tmp50A;_LL4DC: _tmp50D=_tmp505.f1;_tmp50C=_tmp505.f2;_tmp50B=_tmp505.f3;_tmp50A=_tmp505.f4;_LL4DD:;{
# 2576
void*_tmp506=_tmp50D == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp50C,_tmp50D,x->type);
struct Cyc_List_List*_tmp507=Cyc_Tcutil_flatten_typ(_tmp50C,_tmp50A,_tmp50B,_tmp506);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp507)== 1)
return({struct Cyc_List_List*_tmp508=_region_malloc(_tmp50C,sizeof(*_tmp508));_tmp508->hd=({struct _tuple12*_tmp509=_region_malloc(_tmp50C,sizeof(*_tmp509));_tmp509->f1=x->tq;_tmp509->f2=_tmp506;_tmp509;});_tmp508->tl=0;_tmp508;});else{
return _tmp507;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2582
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2584
struct _tuple24 _tmp50E=*env;struct _tuple24 _tmp50F=_tmp50E;struct _RegionHandle*_tmp519;struct Cyc_Tcenv_Tenv*_tmp518;int _tmp517;_LL4DF: _tmp519=_tmp50F.f1;_tmp518=_tmp50F.f2;_tmp517=_tmp50F.f3;_LL4E0:;{
struct _tuple12 _tmp510=*x;struct _tuple12 _tmp511=_tmp510;struct Cyc_Absyn_Tqual _tmp516;void*_tmp515;_LL4E2: _tmp516=_tmp511.f1;_tmp515=_tmp511.f2;_LL4E3:;{
struct Cyc_List_List*_tmp512=Cyc_Tcutil_flatten_typ(_tmp519,_tmp517,_tmp518,_tmp515);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp512)== 1)
return({struct Cyc_List_List*_tmp513=_region_malloc(_tmp519,sizeof(*_tmp513));_tmp513->hd=({struct _tuple12*_tmp514=_region_malloc(_tmp519,sizeof(*_tmp514));_tmp514->f1=_tmp516;_tmp514->f2=_tmp515;_tmp514;});_tmp513->tl=0;_tmp513;});else{
return _tmp512;}};};}
# 2591
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2595
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp51A=t1;struct Cyc_List_List*_tmp53C;struct Cyc_Absyn_Aggrdecl*_tmp53B;struct Cyc_List_List*_tmp53A;struct Cyc_List_List*_tmp539;switch(*((int*)_tmp51A)){case 0: _LL4E5: _LL4E6:
 return 0;case 10: _LL4E7: _tmp539=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp51A)->f1;_LL4E8: {
# 2600
struct _tuple24 _tmp51B=({struct _tuple24 _tmp524;_tmp524.f1=r;_tmp524.f2=te;_tmp524.f3=flatten;_tmp524;});
# 2602
struct Cyc_List_List*_tmp51C=_tmp539;struct _tuple12*_tmp523;struct Cyc_List_List*_tmp522;if(_tmp51C == 0){_LL4F0: _LL4F1:
 return 0;}else{_LL4F2: _tmp523=(struct _tuple12*)_tmp51C->hd;_tmp522=_tmp51C->tl;_LL4F3: {
# 2605
struct Cyc_List_List*_tmp51D=Cyc_Tcutil_rcopy_tqt(& _tmp51B,_tmp523);
_tmp51B.f3=0;{
struct Cyc_List_List*_tmp51E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp51B,_tmp539);
struct Cyc_List_List*_tmp51F=({struct Cyc_List_List*_tmp521=_region_malloc(r,sizeof(*_tmp521));_tmp521->hd=_tmp51D;_tmp521->tl=_tmp51E;_tmp521;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,({struct Cyc_List_List*_tmp520=_region_malloc(r,sizeof(*_tmp520));_tmp520->hd=_tmp51D;_tmp520->tl=_tmp51E;_tmp520;}));};}}_LL4EF:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp51A)->f1).aggr_info).KnownAggr).tag == 2){_LL4E9: _tmp53B=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp51A)->f1).aggr_info).KnownAggr).val;_tmp53A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp51A)->f1).targs;_LL4EA:
# 2613
 if(((_tmp53B->kind == Cyc_Absyn_UnionA  || _tmp53B->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53B->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53B->impl))->rgn_po != 0)
# 2615
return({struct Cyc_List_List*_tmp525=_region_malloc(r,sizeof(*_tmp525));_tmp525->hd=({struct _tuple12*_tmp526=_region_malloc(r,sizeof(*_tmp526));_tmp526->f1=Cyc_Absyn_empty_tqual(0);_tmp526->f2=t1;_tmp526;});_tmp525->tl=0;_tmp525;});{
struct Cyc_List_List*_tmp527=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp53B->tvs,_tmp53A);
struct _tuple23 env=({struct _tuple23 _tmp530;_tmp530.f1=_tmp527;_tmp530.f2=r;_tmp530.f3=te;_tmp530.f4=flatten;_tmp530;});
struct Cyc_List_List*_tmp528=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp53B->impl))->fields;struct Cyc_List_List*_tmp529=_tmp528;struct Cyc_Absyn_Aggrfield*_tmp52F;struct Cyc_List_List*_tmp52E;if(_tmp529 == 0){_LL4F5: _LL4F6:
 return 0;}else{_LL4F7: _tmp52F=(struct Cyc_Absyn_Aggrfield*)_tmp529->hd;_tmp52E=_tmp529->tl;_LL4F8: {
# 2621
struct Cyc_List_List*_tmp52A=Cyc_Tcutil_flatten_typ_f(& env,_tmp52F);
env.f4=0;{
struct Cyc_List_List*_tmp52B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp52E);
struct Cyc_List_List*_tmp52C=({struct Cyc_List_List*_tmp52D=_region_malloc(r,sizeof(*_tmp52D));_tmp52D->hd=_tmp52A;_tmp52D->tl=_tmp52B;_tmp52D;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp52C);};}}_LL4F4:;};}else{goto _LL4ED;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp51A)->f1 == Cyc_Absyn_StructA){_LL4EB: _tmp53C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp51A)->f2;_LL4EC: {
# 2628
struct _tuple23 env=({struct _tuple23 _tmp538;_tmp538.f1=0;_tmp538.f2=r;_tmp538.f3=te;_tmp538.f4=flatten;_tmp538;});
struct Cyc_List_List*_tmp531=_tmp53C;struct Cyc_Absyn_Aggrfield*_tmp537;struct Cyc_List_List*_tmp536;if(_tmp531 == 0){_LL4FA: _LL4FB:
 return 0;}else{_LL4FC: _tmp537=(struct Cyc_Absyn_Aggrfield*)_tmp531->hd;_tmp536=_tmp531->tl;_LL4FD: {
# 2632
struct Cyc_List_List*_tmp532=Cyc_Tcutil_flatten_typ_f(& env,_tmp537);
env.f4=0;{
struct Cyc_List_List*_tmp533=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp536);
struct Cyc_List_List*_tmp534=({struct Cyc_List_List*_tmp535=_region_malloc(r,sizeof(*_tmp535));_tmp535->hd=_tmp532;_tmp535->tl=_tmp533;_tmp535;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp534);};}}_LL4F9:;}}else{goto _LL4ED;}default: _LL4ED: _LL4EE:
# 2638
 goto _LL4E4;}_LL4E4:;};}
# 2641
return({struct Cyc_List_List*_tmp53D=_region_malloc(r,sizeof(*_tmp53D));_tmp53D->hd=({struct _tuple12*_tmp53E=_region_malloc(r,sizeof(*_tmp53E));_tmp53E->f1=Cyc_Absyn_empty_tqual(0);_tmp53E->f2=t1;_tmp53E;});_tmp53D->tl=0;_tmp53D;});}
# 2645
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp53F=(void*)t->hd;void*_tmp540=_tmp53F;switch(*((int*)_tmp540)){case 23: _LL4FF: _LL500:
 goto _LL502;case 4: _LL501: _LL502:
 goto _LL504;case 20: _LL503: _LL504:
# 2651
 continue;default: _LL505: _LL506:
# 2653
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL4FE:;}}
# 2656
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2659
return 1;}
# 2662
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp541=({struct _tuple0 _tmp545;_tmp545.f1=Cyc_Tcutil_compress(t1);_tmp545.f2=Cyc_Tcutil_compress(t2);_tmp545;});struct _tuple0 _tmp542=_tmp541;enum Cyc_Absyn_Size_of _tmp544;enum Cyc_Absyn_Size_of _tmp543;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp542.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp542.f2)->tag == 6){_LL508: _tmp544=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp542.f1)->f2;_tmp543=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp542.f2)->f2;_LL509:
# 2665
 return(_tmp544 == _tmp543  || _tmp544 == Cyc_Absyn_Int_sz  && _tmp543 == Cyc_Absyn_Long_sz) || 
_tmp544 == Cyc_Absyn_Long_sz  && _tmp543 == Cyc_Absyn_Int_sz;}else{goto _LL50A;}}else{_LL50A: _LL50B:
 return 0;}_LL507:;}
# 2673
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2676
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2681
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp546=({struct _tuple0 _tmp57A;_tmp57A.f1=t1;_tmp57A.f2=t2;_tmp57A;});struct _tuple0 _tmp547=_tmp546;struct Cyc_Absyn_FnInfo _tmp579;struct Cyc_Absyn_FnInfo _tmp578;struct Cyc_Absyn_Datatypedecl*_tmp577;struct Cyc_Absyn_Datatypefield*_tmp576;struct Cyc_List_List*_tmp575;struct Cyc_Absyn_Datatypedecl*_tmp574;struct Cyc_List_List*_tmp573;void*_tmp572;struct Cyc_Absyn_Tqual _tmp571;void*_tmp570;union Cyc_Absyn_Constraint*_tmp56F;union Cyc_Absyn_Constraint*_tmp56E;union Cyc_Absyn_Constraint*_tmp56D;void*_tmp56C;struct Cyc_Absyn_Tqual _tmp56B;void*_tmp56A;union Cyc_Absyn_Constraint*_tmp569;union Cyc_Absyn_Constraint*_tmp568;union Cyc_Absyn_Constraint*_tmp567;switch(*((int*)_tmp547.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->tag == 5){_LL50D: _tmp572=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->f1).elt_typ;_tmp571=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->f1).elt_tq;_tmp570=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->f1).ptr_atts).rgn;_tmp56F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->f1).ptr_atts).nullable;_tmp56E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->f1).ptr_atts).bounds;_tmp56D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f1)->f1).ptr_atts).zero_term;_tmp56C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->f1).elt_typ;_tmp56B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->f1).elt_tq;_tmp56A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->f1).ptr_atts).rgn;_tmp569=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->f1).ptr_atts).nullable;_tmp568=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->f1).ptr_atts).bounds;_tmp567=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp547.f2)->f1).ptr_atts).zero_term;_LL50E:
# 2689
 if(_tmp571.real_const  && !_tmp56B.real_const)
return 0;
# 2692
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp56F,_tmp569) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56F)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp569))
return 0;
# 2696
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp56D,_tmp567) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56D)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp567))
return 0;
# 2700
if((!Cyc_Tcutil_unify(_tmp570,_tmp56A) && !Cyc_Tcenv_region_outlives(te,_tmp570,_tmp56A)) && !
Cyc_Tcutil_subtype(te,assume,_tmp570,_tmp56A))
return 0;
# 2704
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp56E,_tmp568)){
struct _tuple0 _tmp548=({struct _tuple0 _tmp54C;_tmp54C.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp56E);_tmp54C.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp568);_tmp54C;});struct _tuple0 _tmp549=_tmp548;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54A;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp549.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp549.f2)->tag == 0){_LL516: _LL517:
 goto _LL515;}else{goto _LL51A;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp549.f2)->tag == 1){_LL518: _tmp54B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp549.f1)->f1;_tmp54A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp549.f2)->f1;_LL519:
# 2709
 if(!Cyc_Evexp_lte_const_exp(_tmp54A,_tmp54B))
return 0;
goto _LL515;}else{_LL51A: _LL51B:
 return 0;}}_LL515:;}
# 2717
if(!_tmp56B.real_const  && _tmp571.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp56C)))
return 0;}{
# 2723
int _tmp54D=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp568,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp567);
# 2729
return(_tmp54D  && Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->hd=({struct _tuple0*_tmp54F=_cycalloc(sizeof(*_tmp54F));_tmp54F->f1=t1;_tmp54F->f2=t2;_tmp54F;});_tmp54E->tl=assume;_tmp54E;}),_tmp572,_tmp56C) || Cyc_Tcutil_unify(_tmp572,_tmp56C)) || Cyc_Tcutil_isomorphic(_tmp572,_tmp56C);};}else{goto _LL513;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp547.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp547.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp547.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL50F: _tmp577=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp547.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp576=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp547.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp575=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp547.f1)->f1).targs;_tmp574=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp547.f2)->f1).datatype_info).KnownDatatype).val;_tmp573=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp547.f2)->f1).targs;_LL510:
# 2735
 if(_tmp577 != _tmp574  && Cyc_Absyn_qvar_cmp(_tmp577->name,_tmp574->name)!= 0)return 0;
# 2737
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp575)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp573))return 0;
for(0;_tmp575 != 0;(_tmp575=_tmp575->tl,_tmp573=_tmp573->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp575->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp573))->hd))return 0;}
return 1;}else{goto _LL513;}}else{goto _LL513;}}else{goto _LL513;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp547.f2)->tag == 9){_LL511: _tmp579=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp547.f1)->f1;_tmp578=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp547.f2)->f1;_LL512:
# 2744
 if(_tmp579.tvars != 0  || _tmp578.tvars != 0){
struct Cyc_List_List*_tmp550=_tmp579.tvars;
struct Cyc_List_List*_tmp551=_tmp578.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp550)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp551))return 0;{
struct _RegionHandle*_tmp552=Cyc_Core_heap_region;
struct Cyc_List_List*inst=0;
while(_tmp550 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp550->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp551))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp553=_region_malloc(_tmp552,sizeof(*_tmp553));_tmp553->hd=({struct _tuple16*_tmp554=_region_malloc(_tmp552,sizeof(*_tmp554));_tmp554->f1=(struct Cyc_Absyn_Tvar*)_tmp551->hd;_tmp554->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp556;_tmp556.tag=2;_tmp556.f1=(struct Cyc_Absyn_Tvar*)_tmp550->hd;_tmp556;});_tmp555;});_tmp554;});_tmp553->tl=inst;_tmp553;});
_tmp550=_tmp550->tl;
_tmp551=_tmp551->tl;}
# 2756
if(inst != 0){
_tmp579.tvars=0;
_tmp578.tvars=0;
return Cyc_Tcutil_subtype(te,assume,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp558;_tmp558.tag=9;_tmp558.f1=_tmp579;_tmp558;});_tmp557;}),(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp559=_cycalloc(sizeof(*_tmp559));_tmp559[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp55A;_tmp55A.tag=9;_tmp55A.f1=_tmp578;_tmp55A;});_tmp559;}));}};}
# 2764
if(!Cyc_Tcutil_subtype(te,assume,_tmp579.ret_typ,_tmp578.ret_typ))return 0;{
struct Cyc_List_List*_tmp55B=_tmp579.args;
struct Cyc_List_List*_tmp55C=_tmp578.args;
# 2769
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp55B)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp55C))return 0;
# 2771
for(0;_tmp55B != 0;(_tmp55B=_tmp55B->tl,_tmp55C=_tmp55C->tl)){
struct _tuple10 _tmp55D=*((struct _tuple10*)_tmp55B->hd);struct _tuple10 _tmp55E=_tmp55D;struct Cyc_Absyn_Tqual _tmp564;void*_tmp563;_LL51D: _tmp564=_tmp55E.f2;_tmp563=_tmp55E.f3;_LL51E:;{
struct _tuple10 _tmp55F=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp55C))->hd);struct _tuple10 _tmp560=_tmp55F;struct Cyc_Absyn_Tqual _tmp562;void*_tmp561;_LL520: _tmp562=_tmp560.f2;_tmp561=_tmp560.f3;_LL521:;
# 2775
if(_tmp562.real_const  && !_tmp564.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp561,_tmp563))
return 0;};}
# 2779
if(_tmp579.c_varargs != _tmp578.c_varargs)return 0;
if(_tmp579.cyc_varargs != 0  && _tmp578.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp565=*_tmp579.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp566=*_tmp578.cyc_varargs;
# 2784
if((_tmp566.tq).real_const  && !(_tmp565.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp566.type,_tmp565.type))
return 0;}else{
if(_tmp579.cyc_varargs != 0  || _tmp578.cyc_varargs != 0)return 0;}
# 2789
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp579.effect),(void*)_check_null(_tmp578.effect)))return 0;
# 2791
if(!Cyc_Tcutil_sub_rgnpo(_tmp579.rgn_po,_tmp578.rgn_po))return 0;
# 2793
if(!Cyc_Tcutil_sub_attributes(_tmp579.attributes,_tmp578.attributes))return 0;
# 2795
if(!Cyc_Tcutil_check_logical_implication(_tmp578.requires_relns,_tmp579.requires_relns))
return 0;
# 2798
if(!Cyc_Tcutil_check_logical_implication(_tmp579.ensures_relns,_tmp578.ensures_relns))
return 0;
# 2801
return 1;};}else{goto _LL513;}default: _LL513: _LL514:
 return 0;}_LL50C:;};}
# 2813 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2815
struct _RegionHandle*_tmp57B=Cyc_Core_heap_region;
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(_tmp57B,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(_tmp57B,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp57C=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp57D=_tmp57C;struct Cyc_Absyn_Tqual _tmp583;void*_tmp582;_LL523: _tmp583=_tmp57D->f1;_tmp582=_tmp57D->f2;_LL524:;{
struct _tuple12*_tmp57E=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp57F=_tmp57E;struct Cyc_Absyn_Tqual _tmp581;void*_tmp580;_LL526: _tmp581=_tmp57F->f1;_tmp580=_tmp57F->f2;_LL527:;
# 2823
if(_tmp583.real_const  && !_tmp581.real_const)return 0;
# 2825
if((_tmp581.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp580))) && 
Cyc_Tcutil_subtype(te,assume,_tmp582,_tmp580))
# 2828
continue;
# 2830
if(Cyc_Tcutil_unify(_tmp582,_tmp580))
# 2832
continue;
# 2834
if(Cyc_Tcutil_isomorphic(_tmp582,_tmp580))
# 2836
continue;
# 2839
return 0;};};}
# 2841
return 1;}
# 2845
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp584=Cyc_Tcutil_compress(t);void*_tmp585=_tmp584;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp585)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp585)->f2 == Cyc_Absyn_Char_sz){_LL529: _LL52A:
 return 1;}else{goto _LL52B;}}else{_LL52B: _LL52C:
 return 0;}_LL528:;}
# 2854
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2860
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp586=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp586)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp586)->f2){case Cyc_Absyn_Int_sz: _LL52E: _LL52F:
# 2864
 goto _LL531;case Cyc_Absyn_Long_sz: _LL530: _LL531:
# 2866
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL52D;default: goto _LL532;}else{_LL532: _LL533:
 goto _LL52D;}_LL52D:;}{
# 2870
void*_tmp587=t1;void*_tmp5AF;struct Cyc_Absyn_Enumdecl*_tmp5AE;void*_tmp5AD;struct Cyc_Absyn_Tqual _tmp5AC;struct Cyc_Absyn_Exp*_tmp5AB;union Cyc_Absyn_Constraint*_tmp5AA;void*_tmp5A9;struct Cyc_Absyn_Tqual _tmp5A8;void*_tmp5A7;union Cyc_Absyn_Constraint*_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A5;union Cyc_Absyn_Constraint*_tmp5A4;switch(*((int*)_tmp587)){case 5: _LL535: _tmp5A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).elt_typ;_tmp5A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).elt_tq;_tmp5A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).ptr_atts).rgn;_tmp5A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).ptr_atts).nullable;_tmp5A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).ptr_atts).bounds;_tmp5A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp587)->f1).ptr_atts).zero_term;_LL536:
# 2879
{void*_tmp588=t2;void*_tmp59A;struct Cyc_Absyn_Tqual _tmp599;void*_tmp598;union Cyc_Absyn_Constraint*_tmp597;union Cyc_Absyn_Constraint*_tmp596;union Cyc_Absyn_Constraint*_tmp595;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->tag == 5){_LL544: _tmp59A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).elt_typ;_tmp599=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).elt_tq;_tmp598=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).rgn;_tmp597=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).nullable;_tmp596=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).bounds;_tmp595=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp588)->f1).ptr_atts).zero_term;_LL545: {
# 2883
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp589=({struct Cyc_List_List*_tmp593=_cycalloc(sizeof(*_tmp593));_tmp593->hd=({struct _tuple0*_tmp594=_cycalloc(sizeof(*_tmp594));_tmp594->f1=t1;_tmp594->f2=t2;_tmp594;});_tmp593->tl=0;_tmp593;});
int _tmp58A=_tmp599.real_const  || !_tmp5A8.real_const;
# 2897 "tcutil.cyc"
int _tmp58B=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp596,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp595);
# 2901
int _tmp58C=_tmp58A  && (
((_tmp58B  && Cyc_Tcutil_ptrsubtype(te,_tmp589,_tmp5A9,_tmp59A) || 
Cyc_Tcutil_unify(_tmp5A9,_tmp59A)) || Cyc_Tcutil_isomorphic(_tmp5A9,_tmp59A)) || Cyc_Tcutil_unify(_tmp59A,(void*)& Cyc_Absyn_VoidType_val));
# 2905
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5A4,_tmp595) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp595);
# 2909
int _tmp58D=_tmp58C?0:((Cyc_Tcutil_bits_only(_tmp5A9) && Cyc_Tcutil_is_char_type(_tmp59A)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp595)) && (
_tmp599.real_const  || !_tmp5A8.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5A5,_tmp596);
if(!bounds_ok  && !_tmp58D){
struct _tuple0 _tmp58E=({struct _tuple0 _tmp592;_tmp592.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5A5);_tmp592.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp596);_tmp592;});struct _tuple0 _tmp58F=_tmp58E;struct Cyc_Absyn_Exp*_tmp591;struct Cyc_Absyn_Exp*_tmp590;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58F.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58F.f2)->tag == 1){_LL549: _tmp591=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58F.f1)->f1;_tmp590=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58F.f2)->f1;_LL54A:
# 2919
 if(Cyc_Evexp_lte_const_exp(_tmp590,_tmp591))
bounds_ok=1;
goto _LL548;}else{goto _LL54B;}}else{_LL54B: _LL54C:
# 2924
 bounds_ok=1;goto _LL548;}_LL548:;}{
# 2926
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A6);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp597);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2933
if(((bounds_ok  && zeroterm_ok) && (_tmp58C  || _tmp58D)) && (
Cyc_Tcutil_unify(_tmp5A7,_tmp598) || Cyc_Tcenv_region_outlives(te,_tmp5A7,_tmp598)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL546: _LL547:
 goto _LL543;}_LL543:;}
# 2939
return Cyc_Absyn_Unknown_coercion;case 8: _LL537: _tmp5AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp587)->f1).elt_type;_tmp5AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp587)->f1).tq;_tmp5AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp587)->f1).num_elts;_tmp5AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp587)->f1).zero_term;_LL538:
# 2941
{void*_tmp59B=t2;void*_tmp59F;struct Cyc_Absyn_Tqual _tmp59E;struct Cyc_Absyn_Exp*_tmp59D;union Cyc_Absyn_Constraint*_tmp59C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59B)->tag == 8){_LL54E: _tmp59F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59B)->f1).elt_type;_tmp59E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59B)->f1).tq;_tmp59D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59B)->f1).num_elts;_tmp59C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59B)->f1).zero_term;_LL54F: {
# 2943
int okay;
okay=
(((_tmp5AB != 0  && _tmp59D != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5AA,_tmp59C)) && 
Cyc_Evexp_lte_const_exp(_tmp59D,_tmp5AB)) && 
Cyc_Evexp_lte_const_exp(_tmp5AB,_tmp59D);
return
# 2950
(okay  && Cyc_Tcutil_unify(_tmp5AD,_tmp59F)) && (!_tmp5AC.real_const  || _tmp59E.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL550: _LL551:
# 2952
 return Cyc_Absyn_Unknown_coercion;}_LL54D:;}
# 2954
return Cyc_Absyn_Unknown_coercion;case 13: _LL539: _tmp5AE=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp587)->f2;_LL53A:
# 2958
{void*_tmp5A0=t2;struct Cyc_Absyn_Enumdecl*_tmp5A1;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A0)->tag == 13){_LL553: _tmp5A1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5A0)->f2;_LL554:
# 2960
 if((_tmp5AE->fields != 0  && _tmp5A1->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5AE->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A1->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL552;}else{_LL555: _LL556:
 goto _LL552;}_LL552:;}
# 2966
goto _LL53C;case 6: _LL53B: _LL53C:
 goto _LL53E;case 7: _LL53D: _LL53E:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL53F: _tmp5AF=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp587)->f1;_LL540:
# 2971
{void*_tmp5A2=t2;void*_tmp5A3;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5A2)->tag == 15){_LL558: _tmp5A3=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5A2)->f1;_LL559:
# 2973
 if(Cyc_Tcenv_region_outlives(te,_tmp5AF,_tmp5A3))return Cyc_Absyn_No_coercion;
goto _LL557;}else{_LL55A: _LL55B:
 goto _LL557;}_LL557:;}
# 2977
return Cyc_Absyn_Unknown_coercion;default: _LL541: _LL542:
 return Cyc_Absyn_Unknown_coercion;}_LL534:;};}
# 2983
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5B0=Cyc_Absyn_copy_exp(e);
e->r=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5B1=_cycalloc(sizeof(*_tmp5B1));_tmp5B1[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5B2;_tmp5B2.tag=13;_tmp5B2.f1=t;_tmp5B2.f2=_tmp5B0;_tmp5B2.f3=0;_tmp5B2.f4=c;_tmp5B2;});_tmp5B1;});
e->topt=t;}}
# 2991
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5B3=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5B4=_tmp5B3;switch(*((int*)_tmp5B4)){case 6: _LL55D: _LL55E:
 goto _LL560;case 13: _LL55F: _LL560:
 goto _LL562;case 14: _LL561: _LL562:
 goto _LL564;case 19: _LL563: _LL564:
 return 1;case 1: _LL565: _LL566:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LL567: _LL568:
 return 0;}_LL55C:;}
# 3002
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5B5=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5B6=_tmp5B5;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5B6)->tag == 7){_LL56A: _LL56B:
 return 1;}else{_LL56C: _LL56D:
 return 0;}_LL569:;};}
# 3011
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5B7=Cyc_Tcutil_compress(t);void*_tmp5B8=_tmp5B7;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5B8)->tag == 9){_LL56F: _LL570:
 return 1;}else{_LL571: _LL572:
 return 0;}_LL56E:;}
# 3018
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5B9=({struct _tuple0 _tmp5BD;_tmp5BD.f1=t1;_tmp5BD.f2=t2;_tmp5BD;});struct _tuple0 _tmp5BA=_tmp5B9;int _tmp5BC;int _tmp5BB;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5BA.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5BA.f2)->tag == 7){_LL574: _tmp5BC=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5BA.f1)->f1;_tmp5BB=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5BA.f2)->f1;_LL575:
# 3021
 if(_tmp5BC != 0  && _tmp5BC != 1)return t1;else{
if(_tmp5BB != 0  && _tmp5BB != 1)return t2;else{
if(_tmp5BC >= _tmp5BB)return t1;else{
return t2;}}}}else{_LL576: _LL577:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5BA.f2)->tag == 7){_LL578: _LL579:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL57A: _LL57B:
 goto _LL57D;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL582;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL584;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL586;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL588;else{goto _LL58E;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL582;case Cyc_Absyn_Int_sz: goto _LL586;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2){case Cyc_Absyn_Long_sz: _LL582: _LL583:
# 3031
 goto _LL585;case Cyc_Absyn_Int_sz: _LL586: _LL587:
# 3034
 goto _LL589;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL584;case Cyc_Absyn_Int_sz: goto _LL588;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{goto _LL58E;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL57E: _LL57F:
# 3029
 goto _LL581;case Cyc_Absyn_Long_sz: _LL58A: _LL58B:
# 3036
 goto _LL58D;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL57C: _LL57D:
# 3028
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL584: _LL585:
# 3032
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL588: _LL589:
# 3035
 return Cyc_Absyn_uint_typ;default: goto _LL58E;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5BA.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL580: _LL581:
# 3030
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL58C: _LL58D:
# 3037
 return Cyc_Absyn_slong_typ;default: goto _LL58E;}}}else{_LL58E: _LL58F:
 return Cyc_Absyn_sint_typ;}}}}_LL573:;}
# 3049 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp5BE=({struct _tuple0 _tmp5D4;_tmp5D4.f1=c1;_tmp5D4.f2=c2;_tmp5D4;});struct _tuple0 _tmp5BF=_tmp5BE;struct Cyc_Core_Opt**_tmp5D3;struct Cyc_Absyn_Kind*_tmp5D2;struct Cyc_Core_Opt**_tmp5D1;struct Cyc_Absyn_Kind*_tmp5D0;struct Cyc_Core_Opt**_tmp5CF;struct Cyc_Absyn_Kind*_tmp5CE;struct Cyc_Absyn_Kind*_tmp5CD;struct Cyc_Core_Opt**_tmp5CC;struct Cyc_Core_Opt**_tmp5CB;struct Cyc_Absyn_Kind*_tmp5CA;struct Cyc_Core_Opt**_tmp5C9;struct Cyc_Absyn_Kind*_tmp5C8;struct Cyc_Absyn_Kind*_tmp5C7;struct Cyc_Absyn_Kind*_tmp5C6;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->tag == 0)switch(*((int*)_tmp5BF.f2)){case 0: _LL591: _tmp5C7=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->f1;_tmp5C6=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->f1;_LL592:
 return _tmp5C7 == _tmp5C6;case 1: goto _LL593;default: _LL599: _tmp5CA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->f1;_tmp5C9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->f1;_tmp5C8=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->f2;_LL59A:
# 3062
 if(Cyc_Tcutil_kind_leq(_tmp5CA,_tmp5C8)){
*_tmp5C9=({struct Cyc_Core_Opt*_tmp5C3=_cycalloc(sizeof(*_tmp5C3));_tmp5C3->v=c1;_tmp5C3;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->tag == 1){_LL593: _tmp5CB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->f1;_LL594:
# 3055
*_tmp5CB=({struct Cyc_Core_Opt*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->v=c1;_tmp5C0;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->tag == 1){_LL595: _tmp5CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->f1;_LL596:
*_tmp5CC=({struct Cyc_Core_Opt*_tmp5C1=_cycalloc(sizeof(*_tmp5C1));_tmp5C1->v=c2;_tmp5C1;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->tag == 0){_LL597: _tmp5CF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->f1;_tmp5CE=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->f2;_tmp5CD=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->f1;_LL598:
# 3058
 if(Cyc_Tcutil_kind_leq(_tmp5CD,_tmp5CE)){
*_tmp5CF=({struct Cyc_Core_Opt*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->v=c2;_tmp5C2;});return 1;}else{
return 0;}}else{_LL59B: _tmp5D3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->f1;_tmp5D2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BF.f1)->f2;_tmp5D1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->f1;_tmp5D0=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BF.f2)->f2;_LL59C:
# 3066
 if(Cyc_Tcutil_kind_leq(_tmp5D2,_tmp5D0)){
*_tmp5D1=({struct Cyc_Core_Opt*_tmp5C4=_cycalloc(sizeof(*_tmp5C4));_tmp5C4->v=c1;_tmp5C4;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5D0,_tmp5D2)){
*_tmp5D3=({struct Cyc_Core_Opt*_tmp5C5=_cycalloc(sizeof(*_tmp5C5));_tmp5C5->v=c2;_tmp5C5;});return 1;}else{
return 0;}}}}}}_LL590:;};}
# 3075
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3080
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5D9;_tmp5D9.tag=1;_tmp5D9.f1=(unsigned long)i;({void*_tmp5D7[1]={& _tmp5D9};Cyc_aprintf(({const char*_tmp5D8="#%d";_tag_dyneither(_tmp5D8,sizeof(char),4);}),_tag_dyneither(_tmp5D7,sizeof(void*),1));});});
return({struct Cyc_Absyn_Tvar*_tmp5D5=_cycalloc(sizeof(*_tmp5D5));_tmp5D5->name=({struct _dyneither_ptr*_tmp5D6=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5D6[0]=(struct _dyneither_ptr)s;_tmp5D6;});_tmp5D5->identity=- 1;_tmp5D5->kind=k;_tmp5D5;});}
# 3087
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5DA=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5DA,sizeof(char),0))== '#';}
# 3092
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5DD;_tmp5DD.tag=0;_tmp5DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5DB[1]={& _tmp5DD};Cyc_printf(({const char*_tmp5DC="%s";_tag_dyneither(_tmp5DC,sizeof(char),3);}),_tag_dyneither(_tmp5DB,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp5DE=Cyc_strconcat(({const char*_tmp5E3="`";_tag_dyneither(_tmp5E3,sizeof(char),2);}),(struct _dyneither_ptr)*t->name);
({struct _dyneither_ptr _tmp5DF=_dyneither_ptr_plus(_tmp5DE,sizeof(char),1);char _tmp5E0=*((char*)_check_dyneither_subscript(_tmp5DF,sizeof(char),0));char _tmp5E1='t';if(_get_dyneither_size(_tmp5DF,sizeof(char))== 1  && (_tmp5E0 == '\000'  && _tmp5E1 != '\000'))_throw_arraybounds();*((char*)_tmp5DF.curr)=_tmp5E1;});
t->name=({struct _dyneither_ptr*_tmp5E2=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5E2[0]=(struct _dyneither_ptr)_tmp5DE;_tmp5E2;});};}
# 3101
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3103
return({struct _tuple10*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->f1=(*x).f1;_tmp5E4->f2=(*x).f2;_tmp5E4->f3=(*x).f3;_tmp5E4;});}
# 3106
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3113
struct Cyc_List_List*_tmp5E5=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp5E5=({struct Cyc_List_List*_tmp5E6=_cycalloc(sizeof(*_tmp5E6));_tmp5E6->hd=(void*)atts->hd;_tmp5E6->tl=_tmp5E5;_tmp5E6;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp5E8;_tmp5E8.tag=9;_tmp5E8.f1=({struct Cyc_Absyn_FnInfo _tmp5E9;_tmp5E9.tvars=fd->tvs;_tmp5E9.effect=fd->effect;_tmp5E9.ret_tqual=fd->ret_tqual;_tmp5E9.ret_typ=fd->ret_type;_tmp5E9.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp5E9.c_varargs=fd->c_varargs;_tmp5E9.cyc_varargs=fd->cyc_varargs;_tmp5E9.rgn_po=fd->rgn_po;_tmp5E9.attributes=_tmp5E5;_tmp5E9.requires_clause=fd->requires_clause;_tmp5E9.requires_relns=fd->requires_relns;_tmp5E9.ensures_clause=fd->ensures_clause;_tmp5E9.ensures_relns=fd->ensures_relns;_tmp5E9;});_tmp5E8;});_tmp5E7;});}
# 3126
return(void*)_check_null(fd->cached_typ);}
# 3132
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3134
union Cyc_Relations_RelnOp _tmp5EA=*rop;union Cyc_Relations_RelnOp _tmp5EB=_tmp5EA;struct Cyc_Absyn_Vardecl*_tmp5FE;struct Cyc_Absyn_Vardecl*_tmp5FD;switch((_tmp5EB.RNumelts).tag){case 2: _LL59E: _tmp5FD=(_tmp5EB.RVar).val;_LL59F: {
# 3136
struct _tuple2 _tmp5EC=*_tmp5FD->name;struct _tuple2 _tmp5ED=_tmp5EC;union Cyc_Absyn_Nmspace _tmp5F4;struct _dyneither_ptr*_tmp5F3;_LL5A5: _tmp5F4=_tmp5ED.f1;_tmp5F3=_tmp5ED.f2;_LL5A6:;
if(!((int)((_tmp5F4.Loc_n).tag == 4)))goto _LL59D;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5F3,({const char*_tmp5EE="return_value";_tag_dyneither(_tmp5EE,sizeof(char),13);}))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL59D;}{
# 3142
unsigned int c=0;
{struct Cyc_List_List*_tmp5EF=args;for(0;_tmp5EF != 0;(_tmp5EF=_tmp5EF->tl,c ++)){
struct _tuple10*_tmp5F0=(struct _tuple10*)_tmp5EF->hd;struct _tuple10*_tmp5F1=_tmp5F0;struct _dyneither_ptr*_tmp5F2;_LL5A8: _tmp5F2=_tmp5F1->f1;_LL5A9:;
if(_tmp5F2 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5F3,(struct _dyneither_ptr)*_tmp5F2)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3152
goto _LL59D;};}case 3: _LL5A0: _tmp5FE=(_tmp5EB.RNumelts).val;_LL5A1: {
# 3154
struct _tuple2 _tmp5F5=*_tmp5FE->name;struct _tuple2 _tmp5F6=_tmp5F5;union Cyc_Absyn_Nmspace _tmp5FC;struct _dyneither_ptr*_tmp5FB;_LL5AB: _tmp5FC=_tmp5F6.f1;_tmp5FB=_tmp5F6.f2;_LL5AC:;
if(!((int)((_tmp5FC.Loc_n).tag == 4)))goto _LL59D;{
unsigned int c=0;
{struct Cyc_List_List*_tmp5F7=args;for(0;_tmp5F7 != 0;(_tmp5F7=_tmp5F7->tl,c ++)){
struct _tuple10*_tmp5F8=(struct _tuple10*)_tmp5F7->hd;struct _tuple10*_tmp5F9=_tmp5F8;struct _dyneither_ptr*_tmp5FA;_LL5AE: _tmp5FA=_tmp5F9->f1;_LL5AF:;
if(_tmp5FA != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5FB,(struct _dyneither_ptr)*_tmp5FA)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3166
goto _LL59D;};}default: _LL5A2: _LL5A3:
 goto _LL59D;}_LL59D:;}
# 3171
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3173
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3177
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3179
if(e == 0)return 0;{
struct Cyc_List_List*_tmp5FF=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp5FF);
return _tmp5FF;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3186
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp600=pr;struct Cyc_Absyn_Tqual _tmp603;void*_tmp602;_LL5B1: _tmp603=_tmp600->f1;_tmp602=_tmp600->f2;_LL5B2:;
if(_tmp602 == t)return pr;else{
return({struct _tuple12*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->f1=_tmp603;_tmp601->f2=t;_tmp601;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3193
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3195
return({struct _tuple27*_tmp604=_region_malloc(rgn,sizeof(*_tmp604));_tmp604->f1=({struct _tuple26*_tmp605=_region_malloc(rgn,sizeof(*_tmp605));_tmp605->f1=(*y).f1;_tmp605->f2=(*y).f2;_tmp605;});_tmp604->f2=(*y).f3;_tmp604;});}
# 3197
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3199
struct _tuple10 _tmp606=*orig_arg;struct _tuple10 _tmp607=_tmp606;struct _dyneither_ptr*_tmp60B;struct Cyc_Absyn_Tqual _tmp60A;void*_tmp609;_LL5B4: _tmp60B=_tmp607.f1;_tmp60A=_tmp607.f2;_tmp609=_tmp607.f3;_LL5B5:;
if(t == _tmp609)return orig_arg;
return({struct _tuple10*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->f1=_tmp60B;_tmp608->f2=_tmp60A;_tmp608->f3=t;_tmp608;});}
# 3203
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3206
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3211
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3213
return({struct Cyc_Absyn_Exp*_tmp60C=_cycalloc(sizeof(*_tmp60C));_tmp60C->topt=old->topt;_tmp60C->r=r;_tmp60C->loc=old->loc;_tmp60C->annot=old->annot;_tmp60C;});}
# 3218
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp60D=e->r;void*_tmp60E=_tmp60D;void*_tmp656;void*_tmp655;struct Cyc_List_List*_tmp654;struct Cyc_Absyn_Exp*_tmp653;void*_tmp652;void*_tmp651;struct Cyc_Absyn_Exp*_tmp650;int _tmp64F;enum Cyc_Absyn_Coercion _tmp64E;struct Cyc_Absyn_Exp*_tmp64D;struct Cyc_Absyn_Exp*_tmp64C;struct Cyc_Absyn_Exp*_tmp64B;struct Cyc_Absyn_Exp*_tmp64A;struct Cyc_Absyn_Exp*_tmp649;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Exp*_tmp645;enum Cyc_Absyn_Primop _tmp644;struct Cyc_List_List*_tmp643;switch(*((int*)_tmp60E)){case 0: _LL5B7: _LL5B8:
 goto _LL5BA;case 31: _LL5B9: _LL5BA:
 goto _LL5BC;case 32: _LL5BB: _LL5BC:
 goto _LL5BE;case 1: _LL5BD: _LL5BE:
 return e;case 2: _LL5BF: _tmp644=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_tmp643=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp60E)->f2;_LL5C0:
# 3226
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp643)== 1){
struct Cyc_Absyn_Exp*_tmp60F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp643))->hd;
struct Cyc_Absyn_Exp*_tmp610=Cyc_Tcutil_rsubsexp(r,inst,_tmp60F);
if(_tmp610 == _tmp60F)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp612;_tmp612.tag=2;_tmp612.f1=_tmp644;_tmp612.f2=({struct Cyc_Absyn_Exp*_tmp613[1];_tmp613[0]=_tmp610;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp613,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp612;});_tmp611;}));}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp643)== 2){
struct Cyc_Absyn_Exp*_tmp614=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp643))->hd;
struct Cyc_Absyn_Exp*_tmp615=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp643->tl))->hd;
struct Cyc_Absyn_Exp*_tmp616=Cyc_Tcutil_rsubsexp(r,inst,_tmp614);
struct Cyc_Absyn_Exp*_tmp617=Cyc_Tcutil_rsubsexp(r,inst,_tmp615);
if(_tmp616 == _tmp614  && _tmp617 == _tmp615)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp618=_cycalloc(sizeof(*_tmp618));_tmp618[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp619;_tmp619.tag=2;_tmp619.f1=_tmp644;_tmp619.f2=({struct Cyc_Absyn_Exp*_tmp61A[2];_tmp61A[1]=_tmp617;_tmp61A[0]=_tmp616;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp61A,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp619;});_tmp618;}));}else{
return({void*_tmp61B=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp61C="primop does not have 1 or 2 args!";_tag_dyneither(_tmp61C,sizeof(char),34);}),_tag_dyneither(_tmp61B,sizeof(void*),0));});}}case 5: _LL5C1: _tmp647=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_tmp646=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp60E)->f2;_tmp645=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp60E)->f3;_LL5C2: {
# 3240
struct Cyc_Absyn_Exp*_tmp61D=Cyc_Tcutil_rsubsexp(r,inst,_tmp647);
struct Cyc_Absyn_Exp*_tmp61E=Cyc_Tcutil_rsubsexp(r,inst,_tmp646);
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);
if((_tmp61D == _tmp647  && _tmp61E == _tmp646) && _tmp61F == _tmp645)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp621;_tmp621.tag=5;_tmp621.f1=_tmp61D;_tmp621.f2=_tmp61E;_tmp621.f3=_tmp61F;_tmp621;});_tmp620;}));}case 6: _LL5C3: _tmp649=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_tmp648=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp60E)->f2;_LL5C4: {
# 3246
struct Cyc_Absyn_Exp*_tmp622=Cyc_Tcutil_rsubsexp(r,inst,_tmp649);
struct Cyc_Absyn_Exp*_tmp623=Cyc_Tcutil_rsubsexp(r,inst,_tmp648);
if(_tmp622 == _tmp649  && _tmp623 == _tmp648)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp624=_cycalloc(sizeof(*_tmp624));_tmp624[0]=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp625;_tmp625.tag=6;_tmp625.f1=_tmp622;_tmp625.f2=_tmp623;_tmp625;});_tmp624;}));}case 7: _LL5C5: _tmp64B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_tmp64A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp60E)->f2;_LL5C6: {
# 3251
struct Cyc_Absyn_Exp*_tmp626=Cyc_Tcutil_rsubsexp(r,inst,_tmp64B);
struct Cyc_Absyn_Exp*_tmp627=Cyc_Tcutil_rsubsexp(r,inst,_tmp64A);
if(_tmp626 == _tmp64B  && _tmp627 == _tmp64A)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp628=_cycalloc(sizeof(*_tmp628));_tmp628[0]=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp629;_tmp629.tag=7;_tmp629.f1=_tmp626;_tmp629.f2=_tmp627;_tmp629;});_tmp628;}));}case 8: _LL5C7: _tmp64D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_tmp64C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp60E)->f2;_LL5C8: {
# 3256
struct Cyc_Absyn_Exp*_tmp62A=Cyc_Tcutil_rsubsexp(r,inst,_tmp64D);
struct Cyc_Absyn_Exp*_tmp62B=Cyc_Tcutil_rsubsexp(r,inst,_tmp64C);
if(_tmp62A == _tmp64D  && _tmp62B == _tmp64C)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp62C=_cycalloc(sizeof(*_tmp62C));_tmp62C[0]=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp62D;_tmp62D.tag=8;_tmp62D.f1=_tmp62A;_tmp62D.f2=_tmp62B;_tmp62D;});_tmp62C;}));}case 13: _LL5C9: _tmp651=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_tmp650=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp60E)->f2;_tmp64F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp60E)->f3;_tmp64E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp60E)->f4;_LL5CA: {
# 3261
struct Cyc_Absyn_Exp*_tmp62E=Cyc_Tcutil_rsubsexp(r,inst,_tmp650);
void*_tmp62F=Cyc_Tcutil_rsubstitute(r,inst,_tmp651);
if(_tmp62E == _tmp650  && _tmp62F == _tmp651)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp630=_cycalloc(sizeof(*_tmp630));_tmp630[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp631;_tmp631.tag=13;_tmp631.f1=_tmp62F;_tmp631.f2=_tmp62E;_tmp631.f3=_tmp64F;_tmp631.f4=_tmp64E;_tmp631;});_tmp630;}));}case 16: _LL5CB: _tmp652=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_LL5CC: {
# 3266
void*_tmp632=Cyc_Tcutil_rsubstitute(r,inst,_tmp652);
if(_tmp632 == _tmp652)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp634;_tmp634.tag=16;_tmp634.f1=_tmp632;_tmp634;});_tmp633;}));}case 17: _LL5CD: _tmp653=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_LL5CE: {
# 3270
struct Cyc_Absyn_Exp*_tmp635=Cyc_Tcutil_rsubsexp(r,inst,_tmp653);
if(_tmp635 == _tmp653)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp636=_cycalloc(sizeof(*_tmp636));_tmp636[0]=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp637;_tmp637.tag=17;_tmp637.f1=_tmp635;_tmp637;});_tmp636;}));}case 18: _LL5CF: _tmp655=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_tmp654=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp60E)->f2;_LL5D0: {
# 3274
void*_tmp638=Cyc_Tcutil_rsubstitute(r,inst,_tmp655);
if(_tmp638 == _tmp655)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp639=_cycalloc(sizeof(*_tmp639));_tmp639[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp63A;_tmp63A.tag=18;_tmp63A.f1=_tmp638;_tmp63A.f2=_tmp654;_tmp63A;});_tmp639;}));}case 38: _LL5D1: _tmp656=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp60E)->f1;_LL5D2: {
# 3278
void*_tmp63B=Cyc_Tcutil_rsubstitute(r,inst,_tmp656);
if(_tmp63B == _tmp656)return e;{
# 3281
void*_tmp63C=Cyc_Tcutil_compress(_tmp63B);void*_tmp63D=_tmp63C;struct Cyc_Absyn_Exp*_tmp640;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp63D)->tag == 18){_LL5D6: _tmp640=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp63D)->f1;_LL5D7:
 return _tmp640;}else{_LL5D8: _LL5D9:
# 3284
 return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp63F;_tmp63F.tag=38;_tmp63F.f1=_tmp63B;_tmp63F;});_tmp63E;}));}_LL5D5:;};}default: _LL5D3: _LL5D4:
# 3287
 return({void*_tmp641=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp642="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp642,sizeof(char),46);}),_tag_dyneither(_tmp641,sizeof(void*),0));});}_LL5B6:;}
# 3291
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3294
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3298
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3301
void*_tmp657=f->type;
struct Cyc_Absyn_Exp*_tmp658=f->requires_clause;
void*_tmp659=Cyc_Tcutil_rsubstitute(r,inst,_tmp657);
struct Cyc_Absyn_Exp*_tmp65A=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp658);
if(_tmp657 == _tmp659  && _tmp658 == _tmp65A)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->name=f->name;_tmp65B->tq=f->tq;_tmp65B->type=_tmp659;_tmp65B->width=f->width;_tmp65B->attributes=f->attributes;_tmp65B->requires_clause=_tmp65A;_tmp65B;});}}
# 3311
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3314
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp65C=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp65D=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp65E=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp65C);
struct Cyc_List_List*_tmp65F=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp65D);
if(_tmp65E == _tmp65C  && _tmp65F == _tmp65D)return fs;
# 3321
return({struct Cyc_List_List*_tmp660=_cycalloc(sizeof(*_tmp660));_tmp660->hd=_tmp65E;_tmp660->tl=_tmp65F;_tmp660;});};}
# 3324
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3327
struct _tuple1 _tmp661=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp662=_tmp661;struct Cyc_List_List*_tmp666;struct Cyc_List_List*_tmp665;_LL5DB: _tmp666=_tmp662.f1;_tmp665=_tmp662.f2;_LL5DC:;{
struct Cyc_List_List*_tmp663=Cyc_Tcutil_substs(rgn,inst,_tmp666);
struct Cyc_List_List*_tmp664=Cyc_Tcutil_substs(rgn,inst,_tmp665);
if(_tmp663 == _tmp666  && _tmp664 == _tmp665)
return rgn_po;else{
# 3333
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp663,_tmp664);}};}
# 3336
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3339
void*_tmp667=Cyc_Tcutil_compress(t);void*_tmp668=_tmp667;struct Cyc_List_List*_tmp6F9;void*_tmp6F8;void*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F5;void*_tmp6F4;void*_tmp6F3;void*_tmp6F2;void*_tmp6F1;void*_tmp6F0;enum Cyc_Absyn_AggrKind _tmp6EF;struct Cyc_List_List*_tmp6EE;struct Cyc_List_List*_tmp6ED;struct Cyc_List_List*_tmp6EC;void*_tmp6EB;struct Cyc_Absyn_Tqual _tmp6EA;void*_tmp6E9;struct Cyc_List_List*_tmp6E8;int _tmp6E7;struct Cyc_Absyn_VarargInfo*_tmp6E6;struct Cyc_List_List*_tmp6E5;struct Cyc_List_List*_tmp6E4;struct Cyc_Absyn_Exp*_tmp6E3;struct Cyc_Absyn_Exp*_tmp6E2;void*_tmp6E1;struct Cyc_Absyn_Tqual _tmp6E0;void*_tmp6DF;union Cyc_Absyn_Constraint*_tmp6DE;union Cyc_Absyn_Constraint*_tmp6DD;union Cyc_Absyn_Constraint*_tmp6DC;void*_tmp6DB;struct Cyc_Absyn_Tqual _tmp6DA;struct Cyc_Absyn_Exp*_tmp6D9;union Cyc_Absyn_Constraint*_tmp6D8;unsigned int _tmp6D7;struct _tuple2*_tmp6D6;struct Cyc_List_List*_tmp6D5;struct Cyc_Absyn_Typedefdecl*_tmp6D4;void*_tmp6D3;union Cyc_Absyn_DatatypeFieldInfoU _tmp6D2;struct Cyc_List_List*_tmp6D1;union Cyc_Absyn_DatatypeInfoU _tmp6D0;struct Cyc_List_List*_tmp6CF;union Cyc_Absyn_AggrInfoU _tmp6CE;struct Cyc_List_List*_tmp6CD;struct Cyc_Absyn_Tvar*_tmp6CC;switch(*((int*)_tmp668)){case 2: _LL5DE: _tmp6CC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp668)->f1;_LL5DF: {
# 3342
struct _handler_cons _tmp669;_push_handler(& _tmp669);{int _tmp66B=0;if(setjmp(_tmp669.handler))_tmp66B=1;if(!_tmp66B){{void*_tmp66C=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6CC);_npop_handler(0);return _tmp66C;};_pop_handler();}else{void*_tmp66A=(void*)_exn_thrown;void*_tmp66D=_tmp66A;void*_tmp66E;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp66D)->tag == Cyc_Core_Not_found){_LL619: _LL61A:
 return t;}else{_LL61B: _tmp66E=_tmp66D;_LL61C:(int)_rethrow(_tmp66E);}_LL618:;}};}case 11: _LL5E0: _tmp6CE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp668)->f1).aggr_info;_tmp6CD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp668)->f1).targs;_LL5E1: {
# 3345
struct Cyc_List_List*_tmp66F=Cyc_Tcutil_substs(rgn,inst,_tmp6CD);
return _tmp66F == _tmp6CD?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp670=_cycalloc(sizeof(*_tmp670));_tmp670[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp671;_tmp671.tag=11;_tmp671.f1=({struct Cyc_Absyn_AggrInfo _tmp672;_tmp672.aggr_info=_tmp6CE;_tmp672.targs=_tmp66F;_tmp672;});_tmp671;});_tmp670;});}case 3: _LL5E2: _tmp6D0=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp668)->f1).datatype_info;_tmp6CF=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp668)->f1).targs;_LL5E3: {
# 3348
struct Cyc_List_List*_tmp673=Cyc_Tcutil_substs(rgn,inst,_tmp6CF);
return _tmp673 == _tmp6CF?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp674=_cycalloc(sizeof(*_tmp674));_tmp674[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp675;_tmp675.tag=3;_tmp675.f1=({struct Cyc_Absyn_DatatypeInfo _tmp676;_tmp676.datatype_info=_tmp6D0;_tmp676.targs=_tmp673;_tmp676;});_tmp675;});_tmp674;});}case 4: _LL5E4: _tmp6D2=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp668)->f1).field_info;_tmp6D1=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp668)->f1).targs;_LL5E5: {
# 3352
struct Cyc_List_List*_tmp677=Cyc_Tcutil_substs(rgn,inst,_tmp6D1);
return _tmp677 == _tmp6D1?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp678=_cycalloc(sizeof(*_tmp678));_tmp678[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp679;_tmp679.tag=4;_tmp679.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp67A;_tmp67A.field_info=_tmp6D2;_tmp67A.targs=_tmp677;_tmp67A;});_tmp679;});_tmp678;});}case 17: _LL5E6: _tmp6D6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp668)->f1;_tmp6D5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp668)->f2;_tmp6D4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp668)->f3;_tmp6D3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp668)->f4;_LL5E7: {
# 3356
struct Cyc_List_List*_tmp67B=Cyc_Tcutil_substs(rgn,inst,_tmp6D5);
return _tmp67B == _tmp6D5?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp67D;_tmp67D.tag=17;_tmp67D.f1=_tmp6D6;_tmp67D.f2=_tmp67B;_tmp67D.f3=_tmp6D4;_tmp67D.f4=_tmp6D3;_tmp67D;});_tmp67C;});}case 8: _LL5E8: _tmp6DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp668)->f1).elt_type;_tmp6DA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp668)->f1).tq;_tmp6D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp668)->f1).num_elts;_tmp6D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp668)->f1).zero_term;_tmp6D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp668)->f1).zt_loc;_LL5E9: {
# 3359
void*_tmp67E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DB);
struct Cyc_Absyn_Exp*_tmp67F=_tmp6D9 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6D9);
return _tmp67E == _tmp6DB  && _tmp67F == _tmp6D9?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp680=_cycalloc(sizeof(*_tmp680));_tmp680[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp681;_tmp681.tag=8;_tmp681.f1=({struct Cyc_Absyn_ArrayInfo _tmp682;_tmp682.elt_type=_tmp67E;_tmp682.tq=_tmp6DA;_tmp682.num_elts=_tmp67F;_tmp682.zero_term=_tmp6D8;_tmp682.zt_loc=_tmp6D7;_tmp682;});_tmp681;});_tmp680;});}case 5: _LL5EA: _tmp6E1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668)->f1).elt_typ;_tmp6E0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668)->f1).elt_tq;_tmp6DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668)->f1).ptr_atts).rgn;_tmp6DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668)->f1).ptr_atts).nullable;_tmp6DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668)->f1).ptr_atts).bounds;_tmp6DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp668)->f1).ptr_atts).zero_term;_LL5EB: {
# 3364
void*_tmp683=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E1);
void*_tmp684=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DF);
union Cyc_Absyn_Constraint*_tmp685=_tmp6DD;
{void*_tmp686=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6DD);void*_tmp687=_tmp686;struct Cyc_Absyn_Exp*_tmp68B;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp687)->tag == 1){_LL61E: _tmp68B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp687)->f1;_LL61F: {
# 3369
struct Cyc_Absyn_Exp*_tmp688=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp68B);
if(_tmp688 != _tmp68B)
_tmp685=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp689=_cycalloc(sizeof(*_tmp689));_tmp689[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp68A;_tmp68A.tag=1;_tmp68A.f1=_tmp688;_tmp68A;});_tmp689;}));
goto _LL61D;}}else{_LL620: _LL621:
 goto _LL61D;}_LL61D:;}
# 3375
if((_tmp683 == _tmp6E1  && _tmp684 == _tmp6DF) && _tmp685 == _tmp6DD)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp68C=_cycalloc(sizeof(*_tmp68C));_tmp68C[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp68D;_tmp68D.tag=5;_tmp68D.f1=({struct Cyc_Absyn_PtrInfo _tmp68E;_tmp68E.elt_typ=_tmp683;_tmp68E.elt_tq=_tmp6E0;_tmp68E.ptr_atts=({(_tmp68E.ptr_atts).rgn=_tmp684;(_tmp68E.ptr_atts).nullable=_tmp6DE;(_tmp68E.ptr_atts).bounds=_tmp685;(_tmp68E.ptr_atts).zero_term=_tmp6DC;(_tmp68E.ptr_atts).ptrloc=0;_tmp68E.ptr_atts;});_tmp68E;});_tmp68D;});_tmp68C;});}case 9: _LL5EC: _tmp6EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).tvars;_tmp6EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).effect;_tmp6EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).ret_tqual;_tmp6E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).ret_typ;_tmp6E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).args;_tmp6E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).c_varargs;_tmp6E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).cyc_varargs;_tmp6E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).rgn_po;_tmp6E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).attributes;_tmp6E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).requires_clause;_tmp6E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp668)->f1).ensures_clause;_LL5ED:
# 3381
{struct Cyc_List_List*_tmp68F=_tmp6EC;for(0;_tmp68F != 0;_tmp68F=_tmp68F->tl){
inst=({struct Cyc_List_List*_tmp690=_region_malloc(rgn,sizeof(*_tmp690));_tmp690->hd=({struct _tuple16*_tmp691=_region_malloc(rgn,sizeof(*_tmp691));_tmp691->f1=(struct Cyc_Absyn_Tvar*)_tmp68F->hd;_tmp691->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp692=_cycalloc(sizeof(*_tmp692));_tmp692[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp693;_tmp693.tag=2;_tmp693.f1=(struct Cyc_Absyn_Tvar*)_tmp68F->hd;_tmp693;});_tmp692;});_tmp691;});_tmp690->tl=inst;_tmp690;});}}{
struct _tuple1 _tmp694=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6E8));
# 3383
struct _tuple1 _tmp695=_tmp694;struct Cyc_List_List*_tmp6A6;struct Cyc_List_List*_tmp6A5;_LL623: _tmp6A6=_tmp695.f1;_tmp6A5=_tmp695.f2;_LL624:;{
# 3385
struct Cyc_List_List*_tmp696=_tmp6E8;
struct Cyc_List_List*_tmp697=Cyc_Tcutil_substs(rgn,inst,_tmp6A5);
if(_tmp697 != _tmp6A5)
_tmp696=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp6E8,_tmp697);{
void*eff2;
if(_tmp6EB == 0)
eff2=0;else{
# 3393
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EB);
if(new_eff == _tmp6EB)
eff2=_tmp6EB;else{
# 3397
eff2=new_eff;}}{
# 3399
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp6E6 == 0)
cyc_varargs2=0;else{
# 3403
struct Cyc_Absyn_VarargInfo _tmp698=*_tmp6E6;struct Cyc_Absyn_VarargInfo _tmp699=_tmp698;struct _dyneither_ptr*_tmp69F;struct Cyc_Absyn_Tqual _tmp69E;void*_tmp69D;int _tmp69C;_LL626: _tmp69F=_tmp699.name;_tmp69E=_tmp699.tq;_tmp69D=_tmp699.type;_tmp69C=_tmp699.inject;_LL627:;{
void*_tmp69A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69D);
if(_tmp69A == _tmp69D)cyc_varargs2=_tmp6E6;else{
# 3407
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B->name=_tmp69F;_tmp69B->tq=_tmp69E;_tmp69B->type=_tmp69A;_tmp69B->inject=_tmp69C;_tmp69B;});}};}{
# 3409
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp6E5);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6E3);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6E2);
struct Cyc_List_List*_tmp6A0=Cyc_Tcutil_extract_relns(_tmp696,req2);
struct Cyc_List_List*_tmp6A1=Cyc_Tcutil_extract_relns(_tmp696,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6A2=_cycalloc(sizeof(*_tmp6A2));_tmp6A2[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6A3;_tmp6A3.tag=9;_tmp6A3.f1=({struct Cyc_Absyn_FnInfo _tmp6A4;_tmp6A4.tvars=_tmp6EC;_tmp6A4.effect=eff2;_tmp6A4.ret_tqual=_tmp6EA;_tmp6A4.ret_typ=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E9);_tmp6A4.args=_tmp696;_tmp6A4.c_varargs=_tmp6E7;_tmp6A4.cyc_varargs=cyc_varargs2;_tmp6A4.rgn_po=rgn_po2;_tmp6A4.attributes=_tmp6E4;_tmp6A4.requires_clause=req2;_tmp6A4.requires_relns=_tmp6A0;_tmp6A4.ensures_clause=ens2;_tmp6A4.ensures_relns=_tmp6A1;_tmp6A4;});_tmp6A3;});_tmp6A2;});};};};};};case 10: _LL5EE: _tmp6ED=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp668)->f1;_LL5EF: {
# 3419
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6A7=_tmp6ED;for(0;_tmp6A7 != 0;_tmp6A7=_tmp6A7->tl){
void*_tmp6A8=(*((struct _tuple12*)_tmp6A7->hd)).f2;
void*_tmp6A9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A8);
if(_tmp6A8 != _tmp6A9)
change=1;
# 3427
ts2=({struct Cyc_List_List*_tmp6AA=_region_malloc(rgn,sizeof(*_tmp6AA));_tmp6AA->hd=_tmp6A9;_tmp6AA->tl=ts2;_tmp6AA;});}}
# 3429
if(!change)
return t;{
struct Cyc_List_List*_tmp6AB=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp6ED,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));_tmp6AC[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6AD;_tmp6AD.tag=10;_tmp6AD.f1=_tmp6AB;_tmp6AD;});_tmp6AC;});};}case 12: _LL5F0: _tmp6EF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp668)->f1;_tmp6EE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp668)->f2;_LL5F1: {
# 3434
struct Cyc_List_List*_tmp6AE=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp6EE);
if(_tmp6EE == _tmp6AE)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6AF=_cycalloc(sizeof(*_tmp6AF));_tmp6AF[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6B0;_tmp6B0.tag=12;_tmp6B0.f1=_tmp6EF;_tmp6B0.f2=_tmp6AE;_tmp6B0;});_tmp6AF;});}case 1: _LL5F2: _tmp6F0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp668)->f2;_LL5F3:
# 3438
 if(_tmp6F0 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F0);else{
return t;}case 15: _LL5F4: _tmp6F1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp668)->f1;_LL5F5: {
# 3441
void*_tmp6B1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F1);
return _tmp6B1 == _tmp6F1?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6B2=_cycalloc(sizeof(*_tmp6B2));_tmp6B2[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6B3;_tmp6B3.tag=15;_tmp6B3.f1=_tmp6B1;_tmp6B3;});_tmp6B2;});}case 16: _LL5F6: _tmp6F3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp668)->f1;_tmp6F2=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp668)->f2;_LL5F7: {
# 3444
void*_tmp6B4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F3);
void*_tmp6B5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F2);
return _tmp6B4 == _tmp6F3  && _tmp6B5 == _tmp6F2?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp6B7;_tmp6B7.tag=16;_tmp6B7.f1=_tmp6B4;_tmp6B7.f2=_tmp6B5;_tmp6B7;});_tmp6B6;});}case 19: _LL5F8: _tmp6F4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp668)->f1;_LL5F9: {
# 3448
void*_tmp6B8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F4);
return _tmp6B8 == _tmp6F4?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6BA;_tmp6BA.tag=19;_tmp6BA.f1=_tmp6B8;_tmp6BA;});_tmp6B9;});}case 18: _LL5FA: _tmp6F5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp668)->f1;_LL5FB: {
# 3451
struct Cyc_Absyn_Exp*_tmp6BB=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6F5);
return _tmp6BB == _tmp6F5?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));_tmp6BC[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6BD;_tmp6BD.tag=18;_tmp6BD.f1=_tmp6BB;_tmp6BD;});_tmp6BC;});}case 27: _LL5FC: _tmp6F6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp668)->f1;_LL5FD: {
# 3454
struct Cyc_Absyn_Exp*_tmp6BE=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6F6);
return _tmp6BE == _tmp6F6?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp6C0;_tmp6C0.tag=27;_tmp6C0.f1=_tmp6BE;_tmp6C0;});_tmp6BF;});}case 13: _LL5FE: _LL5FF:
 goto _LL601;case 14: _LL600: _LL601:
 goto _LL603;case 0: _LL602: _LL603:
 goto _LL605;case 6: _LL604: _LL605:
 goto _LL607;case 7: _LL606: _LL607:
 goto _LL609;case 22: _LL608: _LL609:
 goto _LL60B;case 21: _LL60A: _LL60B:
 goto _LL60D;case 28: _LL60C: _LL60D:
 goto _LL60F;case 20: _LL60E: _LL60F:
 return t;case 25: _LL610: _tmp6F7=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp668)->f1;_LL611: {
# 3466
void*_tmp6C1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F7);
return _tmp6C1 == _tmp6F7?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6C2=_cycalloc(sizeof(*_tmp6C2));_tmp6C2[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6C3;_tmp6C3.tag=25;_tmp6C3.f1=_tmp6C1;_tmp6C3;});_tmp6C2;});}case 23: _LL612: _tmp6F8=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp668)->f1;_LL613: {
# 3469
void*_tmp6C4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F8);
return _tmp6C4 == _tmp6F8?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp6C6;_tmp6C6.tag=23;_tmp6C6.f1=_tmp6C4;_tmp6C6;});_tmp6C5;});}case 24: _LL614: _tmp6F9=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp668)->f1;_LL615: {
# 3472
struct Cyc_List_List*_tmp6C7=Cyc_Tcutil_substs(rgn,inst,_tmp6F9);
return _tmp6C7 == _tmp6F9?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6C9;_tmp6C9.tag=24;_tmp6C9.f1=_tmp6C7;_tmp6C9;});_tmp6C8;});}default: _LL616: _LL617:
({void*_tmp6CA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6CB="found typedecltype in rsubs";_tag_dyneither(_tmp6CB,sizeof(char),28);}),_tag_dyneither(_tmp6CA,sizeof(void*),0));});}_LL5DD:;}
# 3478
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3481
if(ts == 0)
return 0;{
void*_tmp6FA=(void*)ts->hd;
struct Cyc_List_List*_tmp6FB=ts->tl;
void*_tmp6FC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FA);
struct Cyc_List_List*_tmp6FD=Cyc_Tcutil_substs(rgn,inst,_tmp6FB);
if(_tmp6FA == _tmp6FC  && _tmp6FB == _tmp6FD)
return ts;
return({struct Cyc_List_List*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE->hd=_tmp6FC;_tmp6FE->tl=_tmp6FD;_tmp6FE;});};}
# 3492
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3499
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp6FF=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700->f1=tv;_tmp700->f2=Cyc_Absyn_new_evar(_tmp6FF,({struct Cyc_Core_Opt*_tmp701=_cycalloc(sizeof(*_tmp701));_tmp701->v=s;_tmp701;}));_tmp700;});}
# 3504
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3506
struct _tuple17*_tmp702=env;struct Cyc_List_List*_tmp707;struct _RegionHandle*_tmp706;_LL629: _tmp707=_tmp702->f1;_tmp706=_tmp702->f2;_LL62A:;{
struct Cyc_Core_Opt*_tmp703=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp704=_region_malloc(_tmp706,sizeof(*_tmp704));_tmp704->f1=tv;_tmp704->f2=Cyc_Absyn_new_evar(_tmp703,({struct Cyc_Core_Opt*_tmp705=_cycalloc(sizeof(*_tmp705));_tmp705->v=_tmp707;_tmp705;}));_tmp704;});};}
# 3516
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3520
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp70C;_tmp70C.tag=0;_tmp70C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmp70B;_tmp70B.tag=0;_tmp70B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));({struct Cyc_String_pa_PrintArg_struct _tmp70A;_tmp70A.tag=0;_tmp70A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp708[3]={& _tmp70A,& _tmp70B,& _tmp70C};Cyc_Tcutil_terr(loc,({const char*_tmp709="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp709,sizeof(char),59);}),_tag_dyneither(_tmp708,sizeof(void*),3));});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp70D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp70E="same type variable has different identity!";_tag_dyneither(_tmp70E,sizeof(char),43);}),_tag_dyneither(_tmp70D,sizeof(void*),0));});}
return tvs;}}}
# 3534
tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F->hd=tv;_tmp70F->tl=tvs;_tmp70F;});}
# 3540
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3542
if(tv->identity == - 1)
({void*_tmp710=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp711="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp711,sizeof(char),39);}),_tag_dyneither(_tmp710,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3546
struct Cyc_Absyn_Tvar*_tmp712=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp712->identity == - 1)
({void*_tmp713=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp714="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp714,sizeof(char),41);}),_tag_dyneither(_tmp713,sizeof(void*),0));});
if(_tmp712->identity == tv->identity)
return tvs;}}
# 3553
return({struct Cyc_List_List*_tmp715=_cycalloc(sizeof(*_tmp715));_tmp715->hd=tv;_tmp715->tl=tvs;_tmp715;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3559
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3564
if(tv->identity == - 1)
({void*_tmp716=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp717="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp717,sizeof(char),44);}),_tag_dyneither(_tmp716,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3568
struct _tuple28*_tmp718=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp719=_tmp718;struct Cyc_Absyn_Tvar*_tmp71D;int*_tmp71C;_LL62C: _tmp71D=_tmp719->f1;_tmp71C=(int*)& _tmp719->f2;_LL62D:;
if(_tmp71D->identity == - 1)
({void*_tmp71A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp71B="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp71B,sizeof(char),46);}),_tag_dyneither(_tmp71A,sizeof(void*),0));});
if(_tmp71D->identity == tv->identity){
*_tmp71C=*_tmp71C  || b;
return tvs;}}}
# 3576
return({struct Cyc_List_List*_tmp71E=_region_malloc(r,sizeof(*_tmp71E));_tmp71E->hd=({struct _tuple28*_tmp71F=_region_malloc(r,sizeof(*_tmp71F));_tmp71F->f1=tv;_tmp71F->f2=b;_tmp71F;});_tmp71E->tl=tvs;_tmp71E;});}
# 3580
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3582
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp722;_tmp722.tag=0;_tmp722.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));({void*_tmp720[1]={& _tmp722};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp721="bound tvar id for %s is NULL";_tag_dyneither(_tmp721,sizeof(char),29);}),_tag_dyneither(_tmp720,sizeof(void*),1));});});
return({struct Cyc_List_List*_tmp723=_cycalloc(sizeof(*_tmp723));_tmp723->hd=tv;_tmp723->tl=tvs;_tmp723;});}struct _tuple29{void*f1;int f2;};
# 3591
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3594
void*_tmp724=Cyc_Tcutil_compress(e);void*_tmp725=_tmp724;int _tmp72F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp725)->tag == 1){_LL62F: _tmp72F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp725)->f3;_LL630:
# 3596
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp726=(struct _tuple29*)es2->hd;struct _tuple29*_tmp727=_tmp726;void*_tmp72C;int*_tmp72B;_LL634: _tmp72C=_tmp727->f1;_tmp72B=(int*)& _tmp727->f2;_LL635:;{
void*_tmp728=Cyc_Tcutil_compress(_tmp72C);void*_tmp729=_tmp728;int _tmp72A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp729)->tag == 1){_LL637: _tmp72A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp729)->f3;_LL638:
# 3600
 if(_tmp72F == _tmp72A){
if(b != *_tmp72B)*_tmp72B=1;
return es;}
# 3604
goto _LL636;}else{_LL639: _LL63A:
 goto _LL636;}_LL636:;};}}
# 3608
return({struct Cyc_List_List*_tmp72D=_region_malloc(r,sizeof(*_tmp72D));_tmp72D->hd=({struct _tuple29*_tmp72E=_region_malloc(r,sizeof(*_tmp72E));_tmp72E->f1=e;_tmp72E->f2=b;_tmp72E;});_tmp72D->tl=es;_tmp72D;});}else{_LL631: _LL632:
 return es;}_LL62E:;}
# 3613
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3616
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3625
if(!present)r=({struct Cyc_List_List*_tmp730=_region_malloc(rgn,sizeof(*_tmp730));_tmp730->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp730->tl=r;_tmp730;});}
# 3627
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3632
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3636
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp731=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp732=_tmp731;struct Cyc_Absyn_Tvar*_tmp735;int _tmp734;_LL63C: _tmp735=_tmp732.f1;_tmp734=_tmp732.f2;_LL63D:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp735->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3646
if(!present)res=({struct Cyc_List_List*_tmp733=_region_malloc(r,sizeof(*_tmp733));_tmp733->hd=(struct _tuple28*)tvs->hd;_tmp733->tl=res;_tmp733;});};}
# 3648
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3652
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3654
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp738;_tmp738.tag=0;_tmp738.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp736[1]={& _tmp738};Cyc_Tcutil_terr(loc,({const char*_tmp737="bitfield %s does not have constant width";_tag_dyneither(_tmp737,sizeof(char),41);}),_tag_dyneither(_tmp736,sizeof(void*),1));});});else{
# 3659
struct _tuple14 _tmp739=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp73A=_tmp739;unsigned int _tmp73E;int _tmp73D;_LL63F: _tmp73E=_tmp73A.f1;_tmp73D=_tmp73A.f2;_LL640:;
if(!_tmp73D)
({void*_tmp73B=0;Cyc_Tcutil_terr(loc,({const char*_tmp73C="bitfield width cannot use sizeof or offsetof";_tag_dyneither(_tmp73C,sizeof(char),45);}),_tag_dyneither(_tmp73B,sizeof(void*),0));});
w=_tmp73E;}{
# 3664
void*_tmp73F=Cyc_Tcutil_compress(field_typ);void*_tmp740=_tmp73F;enum Cyc_Absyn_Size_of _tmp74E;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp740)->tag == 6){_LL642: _tmp74E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp740)->f2;_LL643:
# 3667
{enum Cyc_Absyn_Size_of _tmp741=_tmp74E;switch(_tmp741){case Cyc_Absyn_Char_sz: _LL647: _LL648:
 if(w > 8)({void*_tmp742=0;Cyc_Tcutil_terr(loc,({const char*_tmp743="bitfield larger than type";_tag_dyneither(_tmp743,sizeof(char),26);}),_tag_dyneither(_tmp742,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Short_sz: _LL649: _LL64A:
 if(w > 16)({void*_tmp744=0;Cyc_Tcutil_terr(loc,({const char*_tmp745="bitfield larger than type";_tag_dyneither(_tmp745,sizeof(char),26);}),_tag_dyneither(_tmp744,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Long_sz: _LL64B: _LL64C:
 goto _LL64E;case Cyc_Absyn_Int_sz: _LL64D: _LL64E:
# 3672
 if(w > 32)({void*_tmp746=0;Cyc_Tcutil_terr(loc,({const char*_tmp747="bitfield larger than type";_tag_dyneither(_tmp747,sizeof(char),26);}),_tag_dyneither(_tmp746,sizeof(void*),0));});goto _LL646;default: _LL64F: _LL650:
# 3674
 if(w > 64)({void*_tmp748=0;Cyc_Tcutil_terr(loc,({const char*_tmp749="bitfield larger than type";_tag_dyneither(_tmp749,sizeof(char),26);}),_tag_dyneither(_tmp748,sizeof(void*),0));});goto _LL646;}_LL646:;}
# 3676
goto _LL641;}else{_LL644: _LL645:
# 3678
({struct Cyc_String_pa_PrintArg_struct _tmp74D;_tmp74D.tag=0;_tmp74D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));({struct Cyc_String_pa_PrintArg_struct _tmp74C;_tmp74C.tag=0;_tmp74C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp74A[2]={& _tmp74C,& _tmp74D};Cyc_Tcutil_terr(loc,({const char*_tmp74B="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp74B,sizeof(char),52);}),_tag_dyneither(_tmp74A,sizeof(void*),2));});});});
goto _LL641;}_LL641:;};}}
# 3685
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp74F=(void*)atts->hd;void*_tmp750=_tmp74F;switch(*((int*)_tmp750)){case 7: _LL652: _LL653:
 continue;case 6: _LL654: _LL655:
 continue;default: _LL656: _LL657:
({struct Cyc_String_pa_PrintArg_struct _tmp754;_tmp754.tag=0;_tmp754.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp753;_tmp753.tag=0;_tmp753.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp751[2]={& _tmp753,& _tmp754};Cyc_Tcutil_terr(loc,({const char*_tmp752="bad attribute %s on member %s";_tag_dyneither(_tmp752,sizeof(char),30);}),_tag_dyneither(_tmp751,sizeof(void*),2));});});});}_LL651:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3709
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3713
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp755=t;struct Cyc_Absyn_Typedefdecl*_tmp759;void*_tmp758;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp755)->tag == 17){_LL659: _tmp759=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp755)->f3;_tmp758=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp755)->f4;_LL65A:
# 3716
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp759))->tq).real_const  || (_tmp759->tq).print_const){
if(declared_const)({void*_tmp756=0;Cyc_Tcutil_warn(loc,({const char*_tmp757="extra const";_tag_dyneither(_tmp757,sizeof(char),12);}),_tag_dyneither(_tmp756,sizeof(void*),0));});
return 1;}
# 3721
if((unsigned int)_tmp758)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp758);else{
return declared_const;}}else{_LL65B: _LL65C:
 return declared_const;}_LL658:;}
# 3728
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp75A=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp75B=_tmp75A;void*_tmp75F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp75B)->tag == 5){_LL65E: _tmp75F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp75B)->f1).ptr_atts).rgn;_LL65F:
# 3733
{void*_tmp75C=Cyc_Tcutil_compress(_tmp75F);void*_tmp75D=_tmp75C;struct Cyc_Absyn_Tvar*_tmp75E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75D)->tag == 2){_LL663: _tmp75E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp75D)->f1;_LL664:
# 3735
 return Cyc_Absyn_tvar_cmp(tvar,_tmp75E)== 0;}else{_LL665: _LL666:
 goto _LL662;}_LL662:;}
# 3738
goto _LL65D;}else{_LL660: _LL661:
 goto _LL65D;}_LL65D:;}}else{
# 3744
return 1;}
return 0;}
# 3748
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3751
void*_tmp760=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp761=_tmp760;switch(*((int*)_tmp761)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp761)->f2)->aliasqual == Cyc_Absyn_Top){_LL668: _LL669:
 goto _LL66B;}else{goto _LL66C;}}else{goto _LL66C;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp761)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp761)->f1)->aliasqual == Cyc_Absyn_Top){_LL66A: _LL66B:
# 3760
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3763
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3769
return& Cyc_Tcutil_trk;}else{goto _LL66C;}}else{goto _LL66C;}default: _LL66C: _LL66D:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL667:;}
# 3775
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3779
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3782
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp765;_tmp765.tag=0;_tmp765.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp764;_tmp764.tag=0;_tmp764.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp762[2]={& _tmp764,& _tmp765};Cyc_Tcutil_terr(loc,({const char*_tmp763="%s clause has type %s instead of integral type";_tag_dyneither(_tmp763,sizeof(char),47);}),_tag_dyneither(_tmp762,sizeof(void*),2));});});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp769;_tmp769.tag=0;_tmp769.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));({struct Cyc_String_pa_PrintArg_struct _tmp768;_tmp768.tag=0;_tmp768.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp766[2]={& _tmp768,& _tmp769};Cyc_Tcutil_terr(clause->loc,({const char*_tmp767="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp767,sizeof(char),36);}),_tag_dyneither(_tmp766,sizeof(void*),2));});});});}
# 3794
return({struct _tuple30 _tmp76A;_tmp76A.f1=cvtenv;_tmp76A.f2=relns;_tmp76A;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3825 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3833
{void*_tmp76B=Cyc_Tcutil_compress(t);void*_tmp76C=_tmp76B;struct Cyc_List_List*_tmp90F;void*_tmp90E;void*_tmp90D;void*_tmp90C;void*_tmp90B;void*_tmp90A;struct _tuple2*_tmp909;struct Cyc_List_List**_tmp908;struct Cyc_Absyn_Typedefdecl**_tmp907;void**_tmp906;union Cyc_Absyn_AggrInfoU*_tmp905;struct Cyc_List_List**_tmp904;enum Cyc_Absyn_AggrKind _tmp903;struct Cyc_List_List*_tmp902;struct Cyc_List_List*_tmp901;struct Cyc_List_List**_tmp900;void**_tmp8FF;struct Cyc_Absyn_Tqual*_tmp8FE;void*_tmp8FD;struct Cyc_List_List*_tmp8FC;int _tmp8FB;struct Cyc_Absyn_VarargInfo*_tmp8FA;struct Cyc_List_List*_tmp8F9;struct Cyc_List_List*_tmp8F8;struct Cyc_Absyn_Exp*_tmp8F7;struct Cyc_List_List**_tmp8F6;struct Cyc_Absyn_Exp*_tmp8F5;struct Cyc_List_List**_tmp8F4;void*_tmp8F3;struct Cyc_Absyn_Tqual*_tmp8F2;struct Cyc_Absyn_Exp**_tmp8F1;union Cyc_Absyn_Constraint*_tmp8F0;unsigned int _tmp8EF;struct Cyc_Absyn_Exp*_tmp8EE;struct Cyc_Absyn_Exp*_tmp8ED;void*_tmp8EC;void*_tmp8EB;struct Cyc_Absyn_Tqual*_tmp8EA;void*_tmp8E9;union Cyc_Absyn_Constraint*_tmp8E8;union Cyc_Absyn_Constraint*_tmp8E7;union Cyc_Absyn_Constraint*_tmp8E6;union Cyc_Absyn_DatatypeFieldInfoU*_tmp8E5;struct Cyc_List_List*_tmp8E4;union Cyc_Absyn_DatatypeInfoU*_tmp8E3;struct Cyc_List_List**_tmp8E2;struct _tuple2*_tmp8E1;struct Cyc_Absyn_Enumdecl**_tmp8E0;struct Cyc_List_List*_tmp8DF;void*_tmp8DE;void***_tmp8DD;struct Cyc_Absyn_Tvar*_tmp8DC;struct Cyc_Core_Opt**_tmp8DB;void**_tmp8DA;switch(*((int*)_tmp76C)){case 0: _LL66F: _LL670:
 goto _LL66E;case 1: _LL671: _tmp8DB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp76C)->f1;_tmp8DA=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp76C)->f2;_LL672:
# 3837
 if(*_tmp8DB == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp8DB))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp8DB))->v,expected_kind))
*_tmp8DB=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3842
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp8DA=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3845
*_tmp8DA=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp76D=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp770=_cycalloc(sizeof(*_tmp770));_tmp770[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp771;_tmp771.tag=2;_tmp771.f1=0;_tmp771.f2=expected_kind;_tmp771;});_tmp770;}));
*_tmp8DA=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp76E=_cycalloc(sizeof(*_tmp76E));_tmp76E[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp76F;_tmp76F.tag=2;_tmp76F.f1=_tmp76D;_tmp76F;});_tmp76E;});
_tmp8DC=_tmp76D;goto _LL674;}else{
# 3851
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3853
goto _LL66E;case 2: _LL673: _tmp8DC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp76C)->f1;_LL674:
# 3855
{void*_tmp772=Cyc_Absyn_compress_kb(_tmp8DC->kind);void*_tmp773=_tmp772;struct Cyc_Core_Opt**_tmp777;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp773)->tag == 1){_LL6AA: _tmp777=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp773)->f1;_LL6AB:
# 3857
*_tmp777=({struct Cyc_Core_Opt*_tmp774=_cycalloc(sizeof(*_tmp774));_tmp774->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp775=_cycalloc(sizeof(*_tmp775));_tmp775[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp776;_tmp776.tag=2;_tmp776.f1=0;_tmp776.f2=expected_kind;_tmp776;});_tmp775;});_tmp774;});goto _LL6A9;}else{_LL6AC: _LL6AD:
 goto _LL6A9;}_LL6A9:;}
# 3862
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8DC);
# 3865
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8DC,put_in_effect);
# 3867
{void*_tmp778=Cyc_Absyn_compress_kb(_tmp8DC->kind);void*_tmp779=_tmp778;struct Cyc_Core_Opt**_tmp77E;struct Cyc_Absyn_Kind*_tmp77D;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp779)->tag == 2){_LL6AF: _tmp77E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp779)->f1;_tmp77D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp779)->f2;_LL6B0:
# 3869
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp77D))
*_tmp77E=({struct Cyc_Core_Opt*_tmp77A=_cycalloc(sizeof(*_tmp77A));_tmp77A->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77B=_cycalloc(sizeof(*_tmp77B));_tmp77B[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp77C;_tmp77C.tag=2;_tmp77C.f1=0;_tmp77C.f2=expected_kind;_tmp77C;});_tmp77B;});_tmp77A;});
goto _LL6AE;}else{_LL6B1: _LL6B2:
 goto _LL6AE;}_LL6AE:;}
# 3874
goto _LL66E;case 26: _LL675: _tmp8DE=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76C)->f1)->r;_tmp8DD=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp76C)->f2;_LL676: {
# 3880
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp77F=_tmp8DE;struct Cyc_Absyn_Datatypedecl*_tmp782;struct Cyc_Absyn_Enumdecl*_tmp781;struct Cyc_Absyn_Aggrdecl*_tmp780;switch(*((int*)_tmp77F)){case 0: _LL6B4: _tmp780=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp77F)->f1;_LL6B5:
# 3883
 if(te->in_extern_c_include)
_tmp780->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp780);goto _LL6B3;case 1: _LL6B6: _tmp781=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp77F)->f1;_LL6B7:
# 3887
 if(te->in_extern_c_include)
_tmp781->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp781);goto _LL6B3;default: _LL6B8: _tmp782=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp77F)->f1;_LL6B9:
# 3891
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp782);goto _LL6B3;}_LL6B3:;}
# 3893
*_tmp8DD=({void**_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783[0]=new_t;_tmp783;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL677: _tmp8DF=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp76C)->f1;_LL678: {
# 3899
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp8DF != 0;_tmp8DF=_tmp8DF->tl){
struct Cyc_Absyn_Enumfield*_tmp784=(struct Cyc_Absyn_Enumfield*)_tmp8DF->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp784->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp787;_tmp787.tag=0;_tmp787.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp784->name).f2);({void*_tmp785[1]={& _tmp787};Cyc_Tcutil_terr(_tmp784->loc,({const char*_tmp786="duplicate enum field name %s";_tag_dyneither(_tmp786,sizeof(char),29);}),_tag_dyneither(_tmp785,sizeof(void*),1));});});else{
# 3906
prev_fields=({struct Cyc_List_List*_tmp788=_cycalloc(sizeof(*_tmp788));_tmp788->hd=(*_tmp784->name).f2;_tmp788->tl=prev_fields;_tmp788;});}
# 3908
if(_tmp784->tag == 0)
_tmp784->tag=Cyc_Absyn_uint_exp(tag_count,_tmp784->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp784->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp78B;_tmp78B.tag=0;_tmp78B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp784->name).f2);({void*_tmp789[1]={& _tmp78B};Cyc_Tcutil_terr(loc,({const char*_tmp78A="enum field %s: expression is not constant";_tag_dyneither(_tmp78A,sizeof(char),42);}),_tag_dyneither(_tmp789,sizeof(void*),1));});});}{
# 3913
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp784->tag))).f1;
tag_count=t1 + 1;};}
# 3916
goto _LL66E;}case 13: _LL679: _tmp8E1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp76C)->f1;_tmp8E0=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp76C)->f2;_LL67A:
# 3918
 if(*_tmp8E0 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp8E0))->fields == 0){
struct _handler_cons _tmp78C;_push_handler(& _tmp78C);{int _tmp78E=0;if(setjmp(_tmp78C.handler))_tmp78E=1;if(!_tmp78E){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp8E1);
*_tmp8E0=*ed;}
# 3920
;_pop_handler();}else{void*_tmp78D=(void*)_exn_thrown;void*_tmp78F=_tmp78D;void*_tmp792;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp78F)->tag == Cyc_Dict_Absent){_LL6BB: _LL6BC: {
# 3924
struct Cyc_Absyn_Enumdecl*_tmp790=({struct Cyc_Absyn_Enumdecl*_tmp791=_cycalloc(sizeof(*_tmp791));_tmp791->sc=Cyc_Absyn_Extern;_tmp791->name=_tmp8E1;_tmp791->fields=0;_tmp791;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp790);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp8E1);
*_tmp8E0=*ed;
goto _LL6BA;};}}else{_LL6BD: _tmp792=_tmp78F;_LL6BE:(int)_rethrow(_tmp792);}_LL6BA:;}};}{
# 3930
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp8E0);
goto _LL66E;};case 3: _LL67B: _tmp8E3=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp76C)->f1).datatype_info;_tmp8E2=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp76C)->f1).targs;_LL67C: {
# 3933
struct Cyc_List_List*_tmp793=*_tmp8E2;
{union Cyc_Absyn_DatatypeInfoU _tmp794=*_tmp8E3;union Cyc_Absyn_DatatypeInfoU _tmp795=_tmp794;struct Cyc_Absyn_Datatypedecl*_tmp7AD;struct _tuple2*_tmp7AC;int _tmp7AB;if((_tmp795.UnknownDatatype).tag == 1){_LL6C0: _tmp7AC=((_tmp795.UnknownDatatype).val).name;_tmp7AB=((_tmp795.UnknownDatatype).val).is_extensible;_LL6C1: {
# 3936
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp796;_push_handler(& _tmp796);{int _tmp798=0;if(setjmp(_tmp796.handler))_tmp798=1;if(!_tmp798){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7AC);;_pop_handler();}else{void*_tmp797=(void*)_exn_thrown;void*_tmp799=_tmp797;void*_tmp79F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp799)->tag == Cyc_Dict_Absent){_LL6C5: _LL6C6: {
# 3940
struct Cyc_Absyn_Datatypedecl*_tmp79A=({struct Cyc_Absyn_Datatypedecl*_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E->sc=Cyc_Absyn_Extern;_tmp79E->name=_tmp7AC;_tmp79E->tvs=0;_tmp79E->fields=0;_tmp79E->is_extensible=_tmp7AB;_tmp79E;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp79A);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7AC);
# 3944
if(_tmp793 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp79D;_tmp79D.tag=0;_tmp79D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7AC));({void*_tmp79B[1]={& _tmp79D};Cyc_Tcutil_terr(loc,({const char*_tmp79C="declare parameterized datatype %s before using";_tag_dyneither(_tmp79C,sizeof(char),47);}),_tag_dyneither(_tmp79B,sizeof(void*),1));});});
return cvtenv;}
# 3949
goto _LL6C4;}}else{_LL6C7: _tmp79F=_tmp799;_LL6C8:(int)_rethrow(_tmp79F);}_LL6C4:;}};}
# 3953
if(_tmp7AB  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7A2;_tmp7A2.tag=0;_tmp7A2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7AC));({void*_tmp7A0[1]={& _tmp7A2};Cyc_Tcutil_terr(loc,({const char*_tmp7A1="datatype %s was not declared @extensible";_tag_dyneither(_tmp7A1,sizeof(char),41);}),_tag_dyneither(_tmp7A0,sizeof(void*),1));});});
*_tmp8E3=Cyc_Absyn_KnownDatatype(tudp);
_tmp7AD=*tudp;goto _LL6C3;}}else{_LL6C2: _tmp7AD=*(_tmp795.KnownDatatype).val;_LL6C3: {
# 3960
struct Cyc_List_List*tvs=_tmp7AD->tvs;
for(0;_tmp793 != 0  && tvs != 0;(_tmp793=_tmp793->tl,tvs=tvs->tl)){
void*t=(void*)_tmp793->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3966
{struct _tuple0 _tmp7A3=({struct _tuple0 _tmp7A6;_tmp7A6.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7A6.f2=t;_tmp7A6;});struct _tuple0 _tmp7A4=_tmp7A3;struct Cyc_Absyn_Tvar*_tmp7A5;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7A4.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7A4.f2)->tag == 2){_LL6CA: _tmp7A5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7A4.f2)->f1;_LL6CB:
# 3968
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7A5);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7A5,1);
continue;}else{goto _LL6CC;}}else{_LL6CC: _LL6CD:
 goto _LL6C9;}_LL6C9:;}{
# 3973
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3977
if(_tmp793 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7A9;_tmp7A9.tag=0;_tmp7A9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7AD->name));({void*_tmp7A7[1]={& _tmp7A9};Cyc_Tcutil_terr(loc,({const char*_tmp7A8="too many type arguments for datatype %s";_tag_dyneither(_tmp7A8,sizeof(char),40);}),_tag_dyneither(_tmp7A7,sizeof(void*),1));});});
if(tvs != 0){
# 3982
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp7AA=_cycalloc(sizeof(*_tmp7AA));_tmp7AA->hd=e;_tmp7AA->tl=hidden_ts;_tmp7AA;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 3989
*_tmp8E2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp8E2,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 3991
goto _LL6BF;}}_LL6BF:;}
# 3993
goto _LL66E;}case 4: _LL67D: _tmp8E5=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp76C)->f1).field_info;_tmp8E4=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp76C)->f1).targs;_LL67E:
# 3996
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7AE=*_tmp8E5;union Cyc_Absyn_DatatypeFieldInfoU _tmp7AF=_tmp7AE;struct Cyc_Absyn_Datatypedecl*_tmp7C3;struct Cyc_Absyn_Datatypefield*_tmp7C2;struct _tuple2*_tmp7C1;struct _tuple2*_tmp7C0;int _tmp7BF;if((_tmp7AF.UnknownDatatypefield).tag == 1){_LL6CF: _tmp7C1=((_tmp7AF.UnknownDatatypefield).val).datatype_name;_tmp7C0=((_tmp7AF.UnknownDatatypefield).val).field_name;_tmp7BF=((_tmp7AF.UnknownDatatypefield).val).is_extensible;_LL6D0: {
# 3999
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7C1);
struct Cyc_Absyn_Datatypefield*tuf;
# 4004
{struct Cyc_List_List*_tmp7B0=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp7B0=_tmp7B0->tl){
if(_tmp7B0 == 0)
({void*_tmp7B1=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp7B2="Tcutil found a bad datatypefield";_tag_dyneither(_tmp7B2,sizeof(char),33);}),_tag_dyneither(_tmp7B1,sizeof(void*),0));});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp7B0->hd)->name,_tmp7C0)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp7B0->hd;
break;}}}
# 4012
*_tmp8E5=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp7C3=tud;_tmp7C2=tuf;goto _LL6D2;}}else{_LL6D1: _tmp7C3=((_tmp7AF.KnownDatatypefield).val).f1;_tmp7C2=((_tmp7AF.KnownDatatypefield).val).f2;_LL6D2: {
# 4016
struct Cyc_List_List*tvs=_tmp7C3->tvs;
for(0;_tmp8E4 != 0  && tvs != 0;(_tmp8E4=_tmp8E4->tl,tvs=tvs->tl)){
void*t=(void*)_tmp8E4->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4022
{struct _tuple0 _tmp7B3=({struct _tuple0 _tmp7B6;_tmp7B6.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7B6.f2=t;_tmp7B6;});struct _tuple0 _tmp7B4=_tmp7B3;struct Cyc_Absyn_Tvar*_tmp7B5;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7B4.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7B4.f2)->tag == 2){_LL6D4: _tmp7B5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7B4.f2)->f1;_LL6D5:
# 4024
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7B5);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7B5,1);
continue;}else{goto _LL6D6;}}else{_LL6D6: _LL6D7:
 goto _LL6D3;}_LL6D3:;}{
# 4029
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4033
if(_tmp8E4 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7BA;_tmp7BA.tag=0;_tmp7BA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7C2->name));({struct Cyc_String_pa_PrintArg_struct _tmp7B9;_tmp7B9.tag=0;_tmp7B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7C3->name));({void*_tmp7B7[2]={& _tmp7B9,& _tmp7BA};Cyc_Tcutil_terr(loc,({const char*_tmp7B8="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp7B8,sizeof(char),43);}),_tag_dyneither(_tmp7B7,sizeof(void*),2));});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7BE;_tmp7BE.tag=0;_tmp7BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7C2->name));({struct Cyc_String_pa_PrintArg_struct _tmp7BD;_tmp7BD.tag=0;_tmp7BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7C3->name));({void*_tmp7BB[2]={& _tmp7BD,& _tmp7BE};Cyc_Tcutil_terr(loc,({const char*_tmp7BC="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp7BC,sizeof(char),42);}),_tag_dyneither(_tmp7BB,sizeof(void*),2));});});});
goto _LL6CE;}}_LL6CE:;}
# 4041
goto _LL66E;case 5: _LL67F: _tmp8EB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76C)->f1).elt_typ;_tmp8EA=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76C)->f1).elt_tq;_tmp8E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76C)->f1).ptr_atts).rgn;_tmp8E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76C)->f1).ptr_atts).nullable;_tmp8E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76C)->f1).ptr_atts).bounds;_tmp8E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76C)->f1).ptr_atts).zero_term;_LL680: {
# 4044
int is_zero_terminated;
# 4046
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp8EB,1,1);
_tmp8EA->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8EA->print_const,_tmp8EB);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp7C4=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp7C5=_tmp7C4;switch(_tmp7C5){case Cyc_Absyn_Aliasable: _LL6D9: _LL6DA:
 k=& Cyc_Tcutil_rk;goto _LL6D8;case Cyc_Absyn_Unique: _LL6DB: _LL6DC:
 k=& Cyc_Tcutil_urk;goto _LL6D8;default: _LL6DD: _LL6DE:
 k=& Cyc_Tcutil_trk;goto _LL6D8;}_LL6D8:;}
# 4054
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp8E9,1,1);
{union Cyc_Absyn_Constraint*_tmp7C6=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp8E6);union Cyc_Absyn_Constraint*_tmp7C7=_tmp7C6;int _tmp7CD;switch((((union Cyc_Absyn_Constraint*)_tmp7C7)->Eq_constr).tag){case 3: _LL6E0: _LL6E1:
# 4059
{void*_tmp7C8=Cyc_Tcutil_compress(_tmp8EB);void*_tmp7C9=_tmp7C8;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C9)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C9)->f2 == Cyc_Absyn_Char_sz){_LL6E7: _LL6E8:
# 4061
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8E6,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL6E6;}else{goto _LL6E9;}}else{_LL6E9: _LL6EA:
# 4065
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8E6,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL6E6;}_LL6E6:;}
# 4069
goto _LL6DF;case 1: _LL6E2: _tmp7CD=(int)(_tmp7C7->Eq_constr).val;if(_tmp7CD){_LL6E3:
# 4072
 if(!Cyc_Tcutil_admits_zero(_tmp8EB))
({struct Cyc_String_pa_PrintArg_struct _tmp7CC;_tmp7CC.tag=0;_tmp7CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8EB));({void*_tmp7CA[1]={& _tmp7CC};Cyc_Tcutil_terr(loc,({const char*_tmp7CB="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp7CB,sizeof(char),53);}),_tag_dyneither(_tmp7CA,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL6DF;}else{goto _LL6E4;}default: _LL6E4: _LL6E5:
# 4078
 is_zero_terminated=0;
goto _LL6DF;}_LL6DF:;}
# 4082
{void*_tmp7CE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp8E7);void*_tmp7CF=_tmp7CE;struct Cyc_Absyn_Exp*_tmp7D8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp7CF)->tag == 0){_LL6EC: _LL6ED:
 goto _LL6EB;}else{_LL6EE: _tmp7D8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7CF)->f1;_LL6EF:
# 4085
 Cyc_Tcexp_tcExp(Cyc_Tcenv_allow_valueof(te),0,_tmp7D8);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7D8,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7D8))
({void*_tmp7D0=0;Cyc_Tcutil_terr(loc,({const char*_tmp7D1="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp7D1,sizeof(char),49);}),_tag_dyneither(_tmp7D0,sizeof(void*),0));});{
struct _tuple14 _tmp7D2=Cyc_Evexp_eval_const_uint_exp(_tmp7D8);struct _tuple14 _tmp7D3=_tmp7D2;unsigned int _tmp7D7;int _tmp7D6;_LL6F1: _tmp7D7=_tmp7D3.f1;_tmp7D6=_tmp7D3.f2;_LL6F2:;
if(is_zero_terminated  && (!_tmp7D6  || _tmp7D7 < 1))
({void*_tmp7D4=0;Cyc_Tcutil_terr(loc,({const char*_tmp7D5="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp7D5,sizeof(char),55);}),_tag_dyneither(_tmp7D4,sizeof(void*),0));});
goto _LL6EB;};}_LL6EB:;}
# 4094
goto _LL66E;};}case 19: _LL681: _tmp8EC=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp76C)->f1;_LL682:
# 4096
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp8EC,1,1);goto _LL66E;case 28: _LL683: _LL684:
 goto _LL66E;case 18: _LL685: _tmp8ED=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp76C)->f1;_LL686:
# 4102
 Cyc_Tcexp_tcExp(Cyc_Tcenv_allow_valueof(te),0,_tmp8ED);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp8ED))
({void*_tmp7D9=0;Cyc_Tcutil_terr(loc,({const char*_tmp7DA="valueof_t requires an int expression";_tag_dyneither(_tmp7DA,sizeof(char),37);}),_tag_dyneither(_tmp7D9,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp8ED,te,cvtenv);
goto _LL66E;case 27: _LL687: _tmp8EE=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp76C)->f1;_LL688:
# 4111
 Cyc_Tcexp_tcExp(Cyc_Tcenv_allow_valueof(te),0,_tmp8EE);
goto _LL66E;case 6: _LL689: _LL68A:
 goto _LL68C;case 7: _LL68B: _LL68C:
 goto _LL66E;case 8: _LL68D: _tmp8F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp76C)->f1).elt_type;_tmp8F2=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp76C)->f1).tq;_tmp8F1=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp76C)->f1).num_elts;_tmp8F0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp76C)->f1).zero_term;_tmp8EF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp76C)->f1).zt_loc;_LL68E: {
# 4118
struct Cyc_Absyn_Exp*_tmp7DB=*_tmp8F1;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8F3,1,allow_abs_aggr);
_tmp8F2->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8F2->print_const,_tmp8F3);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp7DC=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp8F0);union Cyc_Absyn_Constraint*_tmp7DD=_tmp7DC;int _tmp7E1;switch((((union Cyc_Absyn_Constraint*)_tmp7DD)->Eq_constr).tag){case 3: _LL6F4: _LL6F5:
# 4125
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8F0,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4140 "tcutil.cyc"
goto _LL6F3;case 1: _LL6F6: _tmp7E1=(int)(_tmp7DD->Eq_constr).val;if(_tmp7E1){_LL6F7:
# 4143
 if(!Cyc_Tcutil_admits_zero(_tmp8F3))
({struct Cyc_String_pa_PrintArg_struct _tmp7E0;_tmp7E0.tag=0;_tmp7E0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8F3));({void*_tmp7DE[1]={& _tmp7E0};Cyc_Tcutil_terr(loc,({const char*_tmp7DF="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp7DF,sizeof(char),51);}),_tag_dyneither(_tmp7DE,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL6F3;}else{goto _LL6F8;}default: _LL6F8: _LL6F9:
# 4149
 is_zero_terminated=0;
goto _LL6F3;}_LL6F3:;}
# 4154
if(_tmp7DB == 0){
# 4156
if(is_zero_terminated)
# 4158
*_tmp8F1=(_tmp7DB=Cyc_Absyn_uint_exp(1,0));else{
# 4161
({void*_tmp7E2=0;Cyc_Tcutil_warn(loc,({const char*_tmp7E3="array bound defaults to 1 here";_tag_dyneither(_tmp7E3,sizeof(char),31);}),_tag_dyneither(_tmp7E2,sizeof(void*),0));});
*_tmp8F1=(_tmp7DB=Cyc_Absyn_uint_exp(1,0));}}
# 4165
Cyc_Tcexp_tcExp(Cyc_Tcenv_allow_valueof(te),0,_tmp7DB);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7DB))
({void*_tmp7E4=0;Cyc_Tcutil_terr(loc,({const char*_tmp7E5="array bounds expression is not an unsigned int";_tag_dyneither(_tmp7E5,sizeof(char),47);}),_tag_dyneither(_tmp7E4,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7DB,te,cvtenv);{
# 4173
struct _tuple14 _tmp7E6=Cyc_Evexp_eval_const_uint_exp(_tmp7DB);struct _tuple14 _tmp7E7=_tmp7E6;unsigned int _tmp7ED;int _tmp7EC;_LL6FB: _tmp7ED=_tmp7E7.f1;_tmp7EC=_tmp7E7.f2;_LL6FC:;
# 4175
if((is_zero_terminated  && _tmp7EC) && _tmp7ED < 1)
({void*_tmp7E8=0;Cyc_Tcutil_warn(loc,({const char*_tmp7E9="zero terminated array cannot have zero elements";_tag_dyneither(_tmp7E9,sizeof(char),48);}),_tag_dyneither(_tmp7E8,sizeof(void*),0));});
# 4178
if((_tmp7EC  && _tmp7ED < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp7EA=0;Cyc_Tcutil_warn(loc,({const char*_tmp7EB="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp7EB,sizeof(char),75);}),_tag_dyneither(_tmp7EA,sizeof(void*),0));});
*_tmp8F1=Cyc_Absyn_uint_exp(1,0);}
# 4182
goto _LL66E;};};}case 9: _LL68F: _tmp900=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).tvars;_tmp8FF=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).effect;_tmp8FE=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).ret_tqual;_tmp8FD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).ret_typ;_tmp8FC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).args;_tmp8FB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).c_varargs;_tmp8FA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).cyc_varargs;_tmp8F9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).rgn_po;_tmp8F8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).attributes;_tmp8F7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).requires_clause;_tmp8F6=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).requires_relns;_tmp8F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).ensures_clause;_tmp8F4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp76C)->f1).ensures_relns;_LL690: {
# 4189
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp8F8 != 0;_tmp8F8=_tmp8F8->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp8F8->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp7F0;_tmp7F0.tag=0;_tmp7F0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8F8->hd));({void*_tmp7EE[1]={& _tmp7F0};Cyc_Tcutil_terr(loc,({const char*_tmp7EF="bad function type attribute %s";_tag_dyneither(_tmp7EF,sizeof(char),31);}),_tag_dyneither(_tmp7EE,sizeof(void*),1));});});{
void*_tmp7F1=(void*)_tmp8F8->hd;void*_tmp7F2=_tmp7F1;enum Cyc_Absyn_Format_Type _tmp7F7;int _tmp7F6;int _tmp7F5;switch(*((int*)_tmp7F2)){case 1: _LL6FE: _LL6FF:
# 4202
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL6FD;case 2: _LL700: _LL701:
# 4204
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL6FD;case 3: _LL702: _LL703:
# 4206
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL6FD;case 19: _LL704: _tmp7F7=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7F2)->f1;_tmp7F6=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7F2)->f2;_tmp7F5=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7F2)->f3;_LL705:
# 4208
 if(!seen_format){
seen_format=1;
ft=_tmp7F7;
fmt_desc_arg=_tmp7F6;
fmt_arg_start=_tmp7F5;}else{
# 4214
({void*_tmp7F3=0;Cyc_Tcutil_terr(loc,({const char*_tmp7F4="function can't have multiple format attributes";_tag_dyneither(_tmp7F4,sizeof(char),47);}),_tag_dyneither(_tmp7F3,sizeof(void*),0));});}
goto _LL6FD;default: _LL706: _LL707:
 goto _LL6FD;}_LL6FD:;};}
# 4219
if(num_convs > 1)
({void*_tmp7F8=0;Cyc_Tcutil_terr(loc,({const char*_tmp7F9="function can't have multiple calling conventions";_tag_dyneither(_tmp7F9,sizeof(char),49);}),_tag_dyneither(_tmp7F8,sizeof(void*),0));});
# 4224
Cyc_Tcutil_check_unique_tvars(loc,*_tmp900);
{struct Cyc_List_List*b=*_tmp900;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmp7FA=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7FB=_tmp7FA;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7FB)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7FB)->f1)->kind == Cyc_Absyn_MemKind){_LL709: _LL70A:
# 4230
({struct Cyc_String_pa_PrintArg_struct _tmp7FE;_tmp7FE.tag=0;_tmp7FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp7FC[1]={& _tmp7FE};Cyc_Tcutil_terr(loc,({const char*_tmp7FD="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp7FD,sizeof(char),51);}),_tag_dyneither(_tmp7FC,sizeof(void*),1));});});
goto _LL708;}else{goto _LL70B;}}else{_LL70B: _LL70C:
 goto _LL708;}_LL708:;};}}{
# 4238
struct _RegionHandle*_tmp7FF=Cyc_Core_heap_region;{
struct Cyc_Tcutil_CVTEnv _tmp800=({struct Cyc_Tcutil_CVTEnv _tmp88E;_tmp88E.r=_tmp7FF;_tmp88E.kind_env=cvtenv.kind_env;_tmp88E.free_vars=0;_tmp88E.free_evars=0;_tmp88E.generalize_evars=cvtenv.generalize_evars;_tmp88E.fn_result=1;_tmp88E;});
# 4244
_tmp800=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp800,& Cyc_Tcutil_tmk,_tmp8FD,1,1);
_tmp8FE->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8FE->print_const,_tmp8FD);
_tmp800.fn_result=0;
{struct Cyc_List_List*a=_tmp8FC;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp801=(struct _tuple10*)a->hd;
void*_tmp802=(*_tmp801).f3;
_tmp800=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp800,& Cyc_Tcutil_tmk,_tmp802,1,1);{
int _tmp803=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp801).f2).print_const,_tmp802);
((*_tmp801).f2).real_const=_tmp803;
# 4255
if(Cyc_Tcutil_is_array(_tmp802)){
# 4258
void*_tmp804=Cyc_Absyn_new_evar(0,0);
_tmp800=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp800,& Cyc_Tcutil_rk,_tmp804,1,1);
(*_tmp801).f3=Cyc_Tcutil_promote_array(_tmp802,_tmp804,0);}};}}
# 4265
if(_tmp8FA != 0){
if(_tmp8FB)({void*_tmp805=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp806="both c_vararg and cyc_vararg";_tag_dyneither(_tmp806,sizeof(char),29);}),_tag_dyneither(_tmp805,sizeof(void*),0));});{
struct Cyc_Absyn_VarargInfo _tmp807=*_tmp8FA;struct Cyc_Absyn_VarargInfo _tmp808=_tmp807;void*_tmp81B;int _tmp81A;_LL70E: _tmp81B=_tmp808.type;_tmp81A=_tmp808.inject;_LL70F:;
_tmp800=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp800,& Cyc_Tcutil_tmk,_tmp81B,1,1);
(_tmp8FA->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp8FA->tq).print_const,_tmp81B);
# 4271
if(_tmp81A){
void*_tmp809=Cyc_Tcutil_compress(_tmp81B);void*_tmp80A=_tmp809;void*_tmp819;union Cyc_Absyn_Constraint*_tmp818;union Cyc_Absyn_Constraint*_tmp817;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp80A)->tag == 5){_LL711: _tmp819=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp80A)->f1).elt_typ;_tmp818=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp80A)->f1).ptr_atts).bounds;_tmp817=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp80A)->f1).ptr_atts).zero_term;_LL712:
# 4274
{void*_tmp80B=Cyc_Tcutil_compress(_tmp819);void*_tmp80C=_tmp80B;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp80C)->tag == 3){_LL716: _LL717:
# 4276
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp817))
({void*_tmp80D=0;Cyc_Tcutil_terr(loc,({const char*_tmp80E="can't inject into a zeroterm pointer";_tag_dyneither(_tmp80E,sizeof(char),37);}),_tag_dyneither(_tmp80D,sizeof(void*),0));});
{void*_tmp80F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp818);void*_tmp810=_tmp80F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp810)->tag == 0){_LL71B: _LL71C:
# 4280
({void*_tmp811=0;Cyc_Tcutil_terr(loc,({const char*_tmp812="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp812,sizeof(char),44);}),_tag_dyneither(_tmp811,sizeof(void*),0));});
goto _LL71A;}else{_LL71D: _LL71E:
 goto _LL71A;}_LL71A:;}
# 4284
goto _LL715;}else{_LL718: _LL719:
({void*_tmp813=0;Cyc_Tcutil_terr(loc,({const char*_tmp814="can't inject a non-datatype type";_tag_dyneither(_tmp814,sizeof(char),33);}),_tag_dyneither(_tmp813,sizeof(void*),0));});goto _LL715;}_LL715:;}
# 4287
goto _LL710;}else{_LL713: _LL714:
({void*_tmp815=0;Cyc_Tcutil_terr(loc,({const char*_tmp816="expecting a datatype pointer type";_tag_dyneither(_tmp816,sizeof(char),34);}),_tag_dyneither(_tmp815,sizeof(void*),0));});goto _LL710;}_LL710:;}};}
# 4293
if(seen_format){
int _tmp81C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8FC);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp81C) || fmt_arg_start < 0) || 
# 4297
(_tmp8FA == 0  && !_tmp8FB) && fmt_arg_start != 0) || 
(_tmp8FA != 0  || _tmp8FB) && fmt_arg_start != _tmp81C + 1)
# 4300
({void*_tmp81D=0;Cyc_Tcutil_terr(loc,({const char*_tmp81E="bad format descriptor";_tag_dyneither(_tmp81E,sizeof(char),22);}),_tag_dyneither(_tmp81D,sizeof(void*),0));});else{
# 4303
struct _tuple10 _tmp81F=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp8FC,fmt_desc_arg - 1);struct _tuple10 _tmp820=_tmp81F;void*_tmp838;_LL720: _tmp838=_tmp820.f3;_LL721:;
# 4305
{void*_tmp821=Cyc_Tcutil_compress(_tmp838);void*_tmp822=_tmp821;void*_tmp830;union Cyc_Absyn_Constraint*_tmp82F;union Cyc_Absyn_Constraint*_tmp82E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp822)->tag == 5){_LL723: _tmp830=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp822)->f1).elt_typ;_tmp82F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp822)->f1).ptr_atts).bounds;_tmp82E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp822)->f1).ptr_atts).zero_term;_LL724:
# 4308
{struct _tuple0 _tmp823=({struct _tuple0 _tmp82B;_tmp82B.f1=Cyc_Tcutil_compress(_tmp830);_tmp82B.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp82F);_tmp82B;});struct _tuple0 _tmp824=_tmp823;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp824.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp824.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp824.f2)->tag == 0){_LL728: _LL729:
# 4310
 if(_tmp8FB)
({void*_tmp825=0;Cyc_Tcutil_terr(loc,({const char*_tmp826="format descriptor is not a char * type";_tag_dyneither(_tmp826,sizeof(char),39);}),_tag_dyneither(_tmp825,sizeof(void*),0));});
goto _LL727;}else{_LL72A: _LL72B:
# 4314
 if(!_tmp8FB)
({void*_tmp827=0;Cyc_Tcutil_terr(loc,({const char*_tmp828="format descriptor is not a char ? type";_tag_dyneither(_tmp828,sizeof(char),39);}),_tag_dyneither(_tmp827,sizeof(void*),0));});
goto _LL727;}}else{goto _LL72C;}}else{_LL72C: _LL72D:
# 4318
({void*_tmp829=0;Cyc_Tcutil_terr(loc,({const char*_tmp82A="format descriptor is not a string type";_tag_dyneither(_tmp82A,sizeof(char),39);}),_tag_dyneither(_tmp829,sizeof(void*),0));});
goto _LL727;}_LL727:;}
# 4321
goto _LL722;}else{_LL725: _LL726:
({void*_tmp82C=0;Cyc_Tcutil_terr(loc,({const char*_tmp82D="format descriptor is not a string type";_tag_dyneither(_tmp82D,sizeof(char),39);}),_tag_dyneither(_tmp82C,sizeof(void*),0));});goto _LL722;}_LL722:;}
# 4324
if(fmt_arg_start != 0  && !_tmp8FB){
# 4328
int problem;
{struct _tuple31 _tmp831=({struct _tuple31 _tmp835;_tmp835.f1=ft;_tmp835.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp8FA))->type));_tmp835;});struct _tuple31 _tmp832=_tmp831;struct Cyc_Absyn_Datatypedecl*_tmp834;struct Cyc_Absyn_Datatypedecl*_tmp833;if(_tmp832.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp832.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp832.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL72F: _tmp833=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp832.f2)->f1).datatype_info).KnownDatatype).val;_LL730:
# 4331
 problem=Cyc_Absyn_qvar_cmp(_tmp833->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL72E;}else{goto _LL733;}}else{goto _LL733;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp832.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp832.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL731: _tmp834=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp832.f2)->f1).datatype_info).KnownDatatype).val;_LL732:
# 4333
 problem=Cyc_Absyn_qvar_cmp(_tmp834->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL72E;}else{goto _LL733;}}else{_LL733: _LL734:
# 4335
 problem=1;goto _LL72E;}}_LL72E:;}
# 4337
if(problem)
({void*_tmp836=0;Cyc_Tcutil_terr(loc,({const char*_tmp837="format attribute and vararg types don't match";_tag_dyneither(_tmp837,sizeof(char),46);}),_tag_dyneither(_tmp836,sizeof(void*),0));});}}}
# 4345
{struct Cyc_List_List*rpo=_tmp8F9;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp839=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp83A=_tmp839;void*_tmp83C;void*_tmp83B;_LL736: _tmp83C=_tmp83A->f1;_tmp83B=_tmp83A->f2;_LL737:;
_tmp800=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp800,& Cyc_Tcutil_ek,_tmp83C,1,1);
_tmp800=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp800,& Cyc_Tcutil_trk,_tmp83B,1,1);}}{
# 4353
struct Cyc_Tcenv_Fenv*_tmp83D=Cyc_Tcenv_bogus_fenv(_tmp8FD,_tmp8FC);
struct Cyc_Tcenv_Tenv*_tmp83E=({struct Cyc_Tcenv_Tenv*_tmp88D=_region_malloc(_tmp7FF,sizeof(*_tmp88D));_tmp88D->ns=te->ns;_tmp88D->ae=te->ae;_tmp88D->le=_tmp83D;_tmp88D->allow_valueof=1;_tmp88D->in_extern_c_include=te->in_extern_c_include;_tmp88D;});
struct _tuple30 _tmp83F=Cyc_Tcutil_check_clause(loc,_tmp83E,_tmp800,({const char*_tmp88C="@requires";_tag_dyneither(_tmp88C,sizeof(char),10);}),_tmp8F7);struct _tuple30 _tmp840=_tmp83F;struct Cyc_Tcutil_CVTEnv _tmp88B;struct Cyc_List_List*_tmp88A;_LL739: _tmp88B=_tmp840.f1;_tmp88A=_tmp840.f2;_LL73A:;
_tmp800=_tmp88B;
*_tmp8F6=_tmp88A;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp8FC,_tmp88A);{
# 4367 "tcutil.cyc"
struct _tuple30 _tmp841=Cyc_Tcutil_check_clause(loc,_tmp83E,_tmp800,({const char*_tmp889="@ensures";_tag_dyneither(_tmp889,sizeof(char),9);}),_tmp8F5);struct _tuple30 _tmp842=_tmp841;struct Cyc_Tcutil_CVTEnv _tmp888;struct Cyc_List_List*_tmp887;_LL73C: _tmp888=_tmp842.f1;_tmp887=_tmp842.f2;_LL73D:;
_tmp800=_tmp888;
*_tmp8F4=_tmp887;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp8FC,_tmp887);
# 4372
if(*_tmp8FF != 0)
_tmp800=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp800,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp8FF),1,1);else{
# 4375
struct Cyc_List_List*effect=0;
# 4380
{struct Cyc_List_List*tvs=_tmp800.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp843=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp844=_tmp843;struct Cyc_Absyn_Tvar*_tmp85D;int _tmp85C;_LL73F: _tmp85D=_tmp844.f1;_tmp85C=_tmp844.f2;_LL740:;
if(!_tmp85C)continue;{
void*_tmp845=Cyc_Absyn_compress_kb(_tmp85D->kind);void*_tmp846=_tmp845;struct Cyc_Core_Opt**_tmp85B;struct Cyc_Absyn_Kind*_tmp85A;struct Cyc_Core_Opt**_tmp859;struct Cyc_Core_Opt**_tmp858;struct Cyc_Absyn_Kind*_tmp857;switch(*((int*)_tmp846)){case 2: _LL742: _tmp858=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp846)->f1;_tmp857=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp846)->f2;if(_tmp857->kind == Cyc_Absyn_RgnKind){_LL743:
# 4386
 if(_tmp857->aliasqual == Cyc_Absyn_Top){
*_tmp858=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmp85A=_tmp857;goto _LL745;}
# 4389
*_tmp858=Cyc_Tcutil_kind_to_bound_opt(_tmp857);_tmp85A=_tmp857;goto _LL745;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp846)->f2)->kind){case Cyc_Absyn_IntKind: _LL746: _LL747:
# 4392
 goto _LL749;case Cyc_Absyn_EffKind: _LL74A: _tmp859=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp846)->f1;_LL74B:
# 4395
*_tmp859=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL74D;default: goto _LL750;}}case 0: _LL744: _tmp85A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp846)->f1;if(_tmp85A->kind == Cyc_Absyn_RgnKind){_LL745:
# 4391
 effect=({struct Cyc_List_List*_tmp847=_cycalloc(sizeof(*_tmp847));_tmp847->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp848=_cycalloc(sizeof(*_tmp848));_tmp848[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp849;_tmp849.tag=23;_tmp849.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp84A=_cycalloc(sizeof(*_tmp84A));_tmp84A[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp84B;_tmp84B.tag=2;_tmp84B.f1=_tmp85D;_tmp84B;});_tmp84A;});_tmp849;});_tmp848;});_tmp847->tl=effect;_tmp847;});goto _LL741;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp846)->f1)->kind){case Cyc_Absyn_IntKind: _LL748: _LL749:
# 4393
 goto _LL741;case Cyc_Absyn_EffKind: _LL74C: _LL74D:
# 4397
 effect=({struct Cyc_List_List*_tmp84C=_cycalloc(sizeof(*_tmp84C));_tmp84C->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp84D=_cycalloc(sizeof(*_tmp84D));_tmp84D[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp84E;_tmp84E.tag=2;_tmp84E.f1=_tmp85D;_tmp84E;});_tmp84D;});_tmp84C->tl=effect;_tmp84C;});goto _LL741;default: _LL750: _LL751:
# 4402
 effect=({struct Cyc_List_List*_tmp852=_cycalloc(sizeof(*_tmp852));_tmp852->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp853=_cycalloc(sizeof(*_tmp853));_tmp853[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp854;_tmp854.tag=25;_tmp854.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp855=_cycalloc(sizeof(*_tmp855));_tmp855[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp856;_tmp856.tag=2;_tmp856.f1=_tmp85D;_tmp856;});_tmp855;});_tmp854;});_tmp853;});_tmp852->tl=effect;_tmp852;});goto _LL741;}}default: _LL74E: _tmp85B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp846)->f1;_LL74F:
# 4399
*_tmp85B=({struct Cyc_Core_Opt*_tmp84F=_cycalloc(sizeof(*_tmp84F));_tmp84F->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp850=_cycalloc(sizeof(*_tmp850));_tmp850[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp851;_tmp851.tag=2;_tmp851.f1=0;_tmp851.f2=& Cyc_Tcutil_ak;_tmp851;});_tmp850;});_tmp84F;});goto _LL751;}_LL741:;};}}
# 4406
{struct Cyc_List_List*ts=_tmp800.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp85E=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp85F=_tmp85E;void*_tmp86A;int _tmp869;_LL753: _tmp86A=_tmp85F.f1;_tmp869=_tmp85F.f2;_LL754:;
if(!_tmp869)continue;{
struct Cyc_Absyn_Kind*_tmp860=Cyc_Tcutil_typ_kind(_tmp86A);struct Cyc_Absyn_Kind*_tmp861=_tmp860;switch(((struct Cyc_Absyn_Kind*)_tmp861)->kind){case Cyc_Absyn_RgnKind: _LL756: _LL757:
# 4411
 effect=({struct Cyc_List_List*_tmp862=_cycalloc(sizeof(*_tmp862));_tmp862->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp863=_cycalloc(sizeof(*_tmp863));_tmp863[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp864;_tmp864.tag=23;_tmp864.f1=_tmp86A;_tmp864;});_tmp863;});_tmp862->tl=effect;_tmp862;});goto _LL755;case Cyc_Absyn_EffKind: _LL758: _LL759:
# 4413
 effect=({struct Cyc_List_List*_tmp865=_cycalloc(sizeof(*_tmp865));_tmp865->hd=_tmp86A;_tmp865->tl=effect;_tmp865;});goto _LL755;case Cyc_Absyn_IntKind: _LL75A: _LL75B:
 goto _LL755;default: _LL75C: _LL75D:
# 4416
 effect=({struct Cyc_List_List*_tmp866=_cycalloc(sizeof(*_tmp866));_tmp866->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp867=_cycalloc(sizeof(*_tmp867));_tmp867[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp868;_tmp868.tag=25;_tmp868.f1=_tmp86A;_tmp868;});_tmp867;});_tmp866->tl=effect;_tmp866;});goto _LL755;}_LL755:;};}}
# 4419
*_tmp8FF=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp86B=_cycalloc(sizeof(*_tmp86B));_tmp86B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp86C;_tmp86C.tag=24;_tmp86C.f1=effect;_tmp86C;});_tmp86B;});}
# 4425
if(*_tmp900 != 0){
struct Cyc_List_List*bs=*_tmp900;for(0;bs != 0;bs=bs->tl){
void*_tmp86D=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp86E=_tmp86D;struct Cyc_Core_Opt**_tmp87E;struct Cyc_Absyn_Kind*_tmp87D;struct Cyc_Core_Opt**_tmp87C;struct Cyc_Core_Opt**_tmp87B;struct Cyc_Core_Opt**_tmp87A;struct Cyc_Core_Opt**_tmp879;struct Cyc_Core_Opt**_tmp878;struct Cyc_Core_Opt**_tmp877;struct Cyc_Core_Opt**_tmp876;struct Cyc_Core_Opt**_tmp875;struct Cyc_Core_Opt**_tmp874;switch(*((int*)_tmp86E)){case 1: _LL75F: _tmp874=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL760:
# 4429
({struct Cyc_String_pa_PrintArg_struct _tmp871;_tmp871.tag=0;_tmp871.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp86F[1]={& _tmp871};Cyc_Tcutil_warn(loc,({const char*_tmp870="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp870,sizeof(char),47);}),_tag_dyneither(_tmp86F,sizeof(void*),1));});});
# 4431
_tmp875=_tmp874;goto _LL762;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f2)->aliasqual == Cyc_Absyn_Top){_LL761: _tmp875=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL762:
 _tmp876=_tmp875;goto _LL764;}else{goto _LL771;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f2)->aliasqual){case Cyc_Absyn_Top: _LL763: _tmp876=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL764:
 _tmp877=_tmp876;goto _LL766;case Cyc_Absyn_Aliasable: _LL765: _tmp877=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL766:
 _tmp879=_tmp877;goto _LL768;default: _LL76B: _tmp878=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL76C:
# 4438
 _tmp87B=_tmp878;goto _LL76E;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f2)->aliasqual){case Cyc_Absyn_Top: _LL767: _tmp879=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL768:
# 4435
 _tmp87A=_tmp879;goto _LL76A;case Cyc_Absyn_Aliasable: _LL769: _tmp87A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL76A:
# 4437
*_tmp87A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL75E;default: _LL76D: _tmp87B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL76E:
# 4440
*_tmp87B=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL75E;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f2)->aliasqual == Cyc_Absyn_Top){_LL76F: _tmp87C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_LL770:
# 4442
*_tmp87C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL75E;}else{goto _LL771;}default: _LL771: _tmp87E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f1;_tmp87D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp86E)->f2;_LL772:
# 4444
*_tmp87E=Cyc_Tcutil_kind_to_bound_opt(_tmp87D);goto _LL75E;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp86E)->f1)->kind == Cyc_Absyn_MemKind){_LL773: _LL774:
# 4446
({void*_tmp872=0;Cyc_Tcutil_terr(loc,({const char*_tmp873="functions can't abstract M types";_tag_dyneither(_tmp873,sizeof(char),33);}),_tag_dyneither(_tmp872,sizeof(void*),0));});goto _LL75E;}else{_LL775: _LL776:
 goto _LL75E;}}_LL75E:;}}
# 4451
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp800.kind_env,*_tmp900);
_tmp800.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmp800.r,_tmp800.free_vars,*_tmp900);
# 4454
{struct Cyc_List_List*tvs=_tmp800.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp87F=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp880=_tmp87F;struct Cyc_Absyn_Tvar*_tmp882;int _tmp881;_LL778: _tmp882=_tmp880.f1;_tmp881=_tmp880.f2;_LL779:;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp882,_tmp881);}}{
# 4459
struct Cyc_List_List*evs=_tmp800.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp883=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp884=_tmp883;void*_tmp886;int _tmp885;_LL77B: _tmp886=_tmp884.f1;_tmp885=_tmp884.f2;_LL77C:;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp886,_tmp885);}};};};}
# 4464
goto _LL66E;};}case 10: _LL691: _tmp901=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp76C)->f1;_LL692:
# 4467
 for(0;_tmp901 != 0;_tmp901=_tmp901->tl){
struct _tuple12*_tmp88F=(struct _tuple12*)_tmp901->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp88F).f2,1,0);
((*_tmp88F).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp88F).f1).print_const,(*_tmp88F).f2);}
# 4473
goto _LL66E;case 12: _LL693: _tmp903=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp76C)->f1;_tmp902=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp76C)->f2;_LL694: {
# 4477
struct _RegionHandle*_tmp890=Cyc_Core_heap_region;{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp902 != 0;_tmp902=_tmp902->tl){
struct Cyc_Absyn_Aggrfield*_tmp891=(struct Cyc_Absyn_Aggrfield*)_tmp902->hd;struct Cyc_Absyn_Aggrfield*_tmp892=_tmp891;struct _dyneither_ptr*_tmp8A2;struct Cyc_Absyn_Tqual*_tmp8A1;void*_tmp8A0;struct Cyc_Absyn_Exp*_tmp89F;struct Cyc_List_List*_tmp89E;struct Cyc_Absyn_Exp*_tmp89D;_LL77E: _tmp8A2=_tmp892->name;_tmp8A1=(struct Cyc_Absyn_Tqual*)& _tmp892->tq;_tmp8A0=_tmp892->type;_tmp89F=_tmp892->width;_tmp89E=_tmp892->attributes;_tmp89D=_tmp892->requires_clause;_LL77F:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp8A2))
({struct Cyc_String_pa_PrintArg_struct _tmp895;_tmp895.tag=0;_tmp895.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8A2);({void*_tmp893[1]={& _tmp895};Cyc_Tcutil_terr(loc,({const char*_tmp894="duplicate field %s";_tag_dyneither(_tmp894,sizeof(char),19);}),_tag_dyneither(_tmp893,sizeof(void*),1));});});
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp8A2,({const char*_tmp896="";_tag_dyneither(_tmp896,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp897=_region_malloc(_tmp890,sizeof(*_tmp897));_tmp897->hd=_tmp8A2;_tmp897->tl=prev_fields;_tmp897;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8A0,1,0);
_tmp8A1->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8A1->print_const,_tmp8A0);
# 4491
Cyc_Tcutil_check_bitfield(loc,te,_tmp8A0,_tmp89F,_tmp8A2);
Cyc_Tcutil_check_field_atts(loc,_tmp8A2,_tmp89E);
if(_tmp89D != 0){
# 4495
if(_tmp903 != Cyc_Absyn_UnionA)
({void*_tmp898=0;Cyc_Tcutil_terr(loc,({const char*_tmp899="@requires clause is only allowed on union members";_tag_dyneither(_tmp899,sizeof(char),50);}),_tag_dyneither(_tmp898,sizeof(void*),0));});
Cyc_Tcexp_tcExp(Cyc_Tcenv_allow_valueof(te),0,_tmp89D);
if(!Cyc_Tcutil_is_integral(_tmp89D))
({struct Cyc_String_pa_PrintArg_struct _tmp89C;_tmp89C.tag=0;_tmp89C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp89D->topt)));({void*_tmp89A[1]={& _tmp89C};Cyc_Tcutil_terr(loc,({const char*_tmp89B="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp89B,sizeof(char),54);}),_tag_dyneither(_tmp89A,sizeof(void*),1));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp89D,te,cvtenv);}}}
# 4505
goto _LL66E;}case 11: _LL695: _tmp905=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp76C)->f1).aggr_info;_tmp904=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp76C)->f1).targs;_LL696:
# 4508
{union Cyc_Absyn_AggrInfoU _tmp8A3=*_tmp905;union Cyc_Absyn_AggrInfoU _tmp8A4=_tmp8A3;struct Cyc_Absyn_Aggrdecl*_tmp8C8;enum Cyc_Absyn_AggrKind _tmp8C7;struct _tuple2*_tmp8C6;struct Cyc_Core_Opt*_tmp8C5;if((_tmp8A4.UnknownAggr).tag == 1){_LL781: _tmp8C7=((_tmp8A4.UnknownAggr).val).f1;_tmp8C6=((_tmp8A4.UnknownAggr).val).f2;_tmp8C5=((_tmp8A4.UnknownAggr).val).f3;_LL782: {
# 4510
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp8A5;_push_handler(& _tmp8A5);{int _tmp8A7=0;if(setjmp(_tmp8A5.handler))_tmp8A7=1;if(!_tmp8A7){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8C6);{
struct Cyc_Absyn_Aggrdecl*_tmp8A8=*adp;
if(_tmp8A8->kind != _tmp8C7){
if(_tmp8A8->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp8AC;_tmp8AC.tag=0;_tmp8AC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8C6));({struct Cyc_String_pa_PrintArg_struct _tmp8AB;_tmp8AB.tag=0;_tmp8AB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4516
Cyc_Absynpp_qvar2string(_tmp8C6));({void*_tmp8A9[2]={& _tmp8AB,& _tmp8AC};Cyc_Tcutil_terr(loc,({const char*_tmp8AA="expecting struct %s instead of union %s";_tag_dyneither(_tmp8AA,sizeof(char),40);}),_tag_dyneither(_tmp8A9,sizeof(void*),2));});});});else{
# 4519
({struct Cyc_String_pa_PrintArg_struct _tmp8B0;_tmp8B0.tag=0;_tmp8B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8C6));({struct Cyc_String_pa_PrintArg_struct _tmp8AF;_tmp8AF.tag=0;_tmp8AF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4519
Cyc_Absynpp_qvar2string(_tmp8C6));({void*_tmp8AD[2]={& _tmp8AF,& _tmp8B0};Cyc_Tcutil_terr(loc,({const char*_tmp8AE="expecting union %s instead of struct %s";_tag_dyneither(_tmp8AE,sizeof(char),40);}),_tag_dyneither(_tmp8AD,sizeof(void*),2));});});});}}
# 4522
if((unsigned int)_tmp8C5  && (int)_tmp8C5->v){
if(!((unsigned int)_tmp8A8->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8A8->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp8B3;_tmp8B3.tag=0;_tmp8B3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8C6));({void*_tmp8B1[1]={& _tmp8B3};Cyc_Tcutil_terr(loc,({const char*_tmp8B2="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp8B2,sizeof(char),42);}),_tag_dyneither(_tmp8B1,sizeof(void*),1));});});}
# 4528
*_tmp905=Cyc_Absyn_KnownAggr(adp);};
# 4512
;_pop_handler();}else{void*_tmp8A6=(void*)_exn_thrown;void*_tmp8B4=_tmp8A6;void*_tmp8BA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8B4)->tag == Cyc_Dict_Absent){_LL786: _LL787: {
# 4532
struct Cyc_Absyn_Aggrdecl*_tmp8B5=({struct Cyc_Absyn_Aggrdecl*_tmp8B9=_cycalloc(sizeof(*_tmp8B9));_tmp8B9->kind=_tmp8C7;_tmp8B9->sc=Cyc_Absyn_Extern;_tmp8B9->name=_tmp8C6;_tmp8B9->tvs=0;_tmp8B9->impl=0;_tmp8B9->attributes=0;_tmp8B9->expected_mem_kind=0;_tmp8B9;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp8B5);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8C6);
*_tmp905=Cyc_Absyn_KnownAggr(adp);
# 4537
if(*_tmp904 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8B8;_tmp8B8.tag=0;_tmp8B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8C6));({void*_tmp8B6[1]={& _tmp8B8};Cyc_Tcutil_terr(loc,({const char*_tmp8B7="declare parameterized type %s before using";_tag_dyneither(_tmp8B7,sizeof(char),43);}),_tag_dyneither(_tmp8B6,sizeof(void*),1));});});
return cvtenv;}
# 4542
goto _LL785;}}else{_LL788: _tmp8BA=_tmp8B4;_LL789:(int)_rethrow(_tmp8BA);}_LL785:;}};}
# 4544
_tmp8C8=*adp;goto _LL784;}}else{_LL783: _tmp8C8=*(_tmp8A4.KnownAggr).val;_LL784: {
# 4546
struct Cyc_List_List*tvs=_tmp8C8->tvs;
struct Cyc_List_List*ts=*_tmp904;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp8BB=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8BC=(void*)ts->hd;
# 4554
{struct _tuple0 _tmp8BD=({struct _tuple0 _tmp8C0;_tmp8C0.f1=Cyc_Absyn_compress_kb(_tmp8BB->kind);_tmp8C0.f2=_tmp8BC;_tmp8C0;});struct _tuple0 _tmp8BE=_tmp8BD;struct Cyc_Absyn_Tvar*_tmp8BF;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8BE.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8BE.f2)->tag == 2){_LL78B: _tmp8BF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8BE.f2)->f1;_LL78C:
# 4556
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8BF);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8BF,1);
continue;}else{goto _LL78D;}}else{_LL78D: _LL78E:
 goto _LL78A;}_LL78A:;}{
# 4561
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4565
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8C3;_tmp8C3.tag=0;_tmp8C3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8C8->name));({void*_tmp8C1[1]={& _tmp8C3};Cyc_Tcutil_terr(loc,({const char*_tmp8C2="too many parameters for type %s";_tag_dyneither(_tmp8C2,sizeof(char),32);}),_tag_dyneither(_tmp8C1,sizeof(void*),1));});});
if(tvs != 0){
# 4569
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp8C4=_cycalloc(sizeof(*_tmp8C4));_tmp8C4->hd=e;_tmp8C4->tl=hidden_ts;_tmp8C4;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4576
*_tmp904=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp904,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4578
if((allow_abs_aggr  && _tmp8C8->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4583
_tmp8C8->expected_mem_kind=1;}}_LL780:;}
# 4586
goto _LL66E;case 17: _LL697: _tmp909=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp76C)->f1;_tmp908=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp76C)->f2;_tmp907=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp76C)->f3;_tmp906=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp76C)->f4;_LL698: {
# 4589
struct Cyc_List_List*targs=*_tmp908;
# 4591
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp8C9;_push_handler(& _tmp8C9);{int _tmp8CB=0;if(setjmp(_tmp8C9.handler))_tmp8CB=1;if(!_tmp8CB){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp909);;_pop_handler();}else{void*_tmp8CA=(void*)_exn_thrown;void*_tmp8CC=_tmp8CA;void*_tmp8D0;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8CC)->tag == Cyc_Dict_Absent){_LL790: _LL791:
# 4594
({struct Cyc_String_pa_PrintArg_struct _tmp8CF;_tmp8CF.tag=0;_tmp8CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp909));({void*_tmp8CD[1]={& _tmp8CF};Cyc_Tcutil_terr(loc,({const char*_tmp8CE="unbound typedef name %s";_tag_dyneither(_tmp8CE,sizeof(char),24);}),_tag_dyneither(_tmp8CD,sizeof(void*),1));});});
return cvtenv;}else{_LL792: _tmp8D0=_tmp8CC;_LL793:(int)_rethrow(_tmp8D0);}_LL78F:;}};}
# 4597
*_tmp907=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle*_tmp8D1=Cyc_Core_heap_region;{
struct Cyc_List_List*inst=0;
# 4603
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4608
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp8D2=_region_malloc(_tmp8D1,sizeof(*_tmp8D2));_tmp8D2->hd=({struct _tuple16*_tmp8D3=_region_malloc(_tmp8D1,sizeof(*_tmp8D3));_tmp8D3->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8D3->f2=(void*)ts->hd;_tmp8D3;});_tmp8D2->tl=inst;_tmp8D2;});}
# 4612
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8D6;_tmp8D6.tag=0;_tmp8D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp909));({void*_tmp8D4[1]={& _tmp8D6};Cyc_Tcutil_terr(loc,({const char*_tmp8D5="too many parameters for typedef %s";_tag_dyneither(_tmp8D5,sizeof(char),35);}),_tag_dyneither(_tmp8D4,sizeof(void*),1));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4617
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp8D7=_cycalloc(sizeof(*_tmp8D7));_tmp8D7->hd=e;_tmp8D7->tl=hidden_ts;_tmp8D7;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp8D8=_cycalloc(sizeof(*_tmp8D8));_tmp8D8->hd=({struct _tuple16*_tmp8D9=_cycalloc(sizeof(*_tmp8D9));_tmp8D9->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8D9->f2=e;_tmp8D9;});_tmp8D8->tl=inst;_tmp8D8;});}
# 4625
*_tmp908=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4627
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(_tmp8D1,inst,(void*)_check_null(td->defn));
*_tmp906=new_typ;}}
# 4634
goto _LL66E;};}case 22: _LL699: _LL69A:
 goto _LL69C;case 21: _LL69B: _LL69C:
 goto _LL69E;case 20: _LL69D: _LL69E:
 goto _LL66E;case 15: _LL69F: _tmp90A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp76C)->f1;_LL6A0:
# 4639
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp90A,1,allow_abs_aggr);goto _LL66E;case 16: _LL6A1: _tmp90C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp76C)->f1;_tmp90B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp76C)->f2;_LL6A2:
# 4642
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp90C,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp90B,1,0);
goto _LL66E;case 23: _LL6A3: _tmp90D=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp76C)->f1;_LL6A4:
# 4646
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp90D,1,1);goto _LL66E;case 25: _LL6A5: _tmp90E=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp76C)->f1;_LL6A6:
# 4648
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp90E,1,1);goto _LL66E;default: _LL6A7: _tmp90F=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp76C)->f1;_LL6A8:
# 4650
 for(0;_tmp90F != 0;_tmp90F=_tmp90F->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp90F->hd,1,1);}
goto _LL66E;}_LL66E:;}
# 4654
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))
({struct Cyc_String_pa_PrintArg_struct _tmp914;_tmp914.tag=0;_tmp914.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));({struct Cyc_String_pa_PrintArg_struct _tmp913;_tmp913.tag=0;_tmp913.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));({struct Cyc_String_pa_PrintArg_struct _tmp912;_tmp912.tag=0;_tmp912.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp910[3]={& _tmp912,& _tmp913,& _tmp914};Cyc_Tcutil_terr(loc,({const char*_tmp911="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp911,sizeof(char),51);}),_tag_dyneither(_tmp910,sizeof(void*),3));});});});});
# 4658
return cvtenv;}
# 4666
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4668
{void*_tmp915=e->r;void*_tmp916=_tmp915;struct Cyc_Absyn_Exp*_tmp928;void*_tmp927;void*_tmp926;void*_tmp925;void*_tmp924;struct Cyc_Absyn_Exp*_tmp923;struct Cyc_Absyn_Exp*_tmp922;struct Cyc_Absyn_Exp*_tmp921;struct Cyc_Absyn_Exp*_tmp920;struct Cyc_Absyn_Exp*_tmp91F;struct Cyc_Absyn_Exp*_tmp91E;struct Cyc_Absyn_Exp*_tmp91D;struct Cyc_Absyn_Exp*_tmp91C;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91A;struct Cyc_List_List*_tmp919;switch(*((int*)_tmp916)){case 0: _LL795: _LL796:
 goto _LL798;case 31: _LL797: _LL798:
 goto _LL79A;case 32: _LL799: _LL79A:
 goto _LL79C;case 1: _LL79B: _LL79C:
 goto _LL794;case 2: _LL79D: _tmp919=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp916)->f2;_LL79E:
# 4674
 for(0;_tmp919 != 0;_tmp919=_tmp919->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp919->hd,te,cvtenv);}
goto _LL794;case 5: _LL79F: _tmp91C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_tmp91B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp916)->f2;_tmp91A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp916)->f3;_LL7A0:
# 4678
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91C,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91B,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91A,te,cvtenv);
goto _LL794;case 6: _LL7A1: _tmp91E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_tmp91D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp916)->f2;_LL7A2:
 _tmp920=_tmp91E;_tmp91F=_tmp91D;goto _LL7A4;case 7: _LL7A3: _tmp920=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_tmp91F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp916)->f2;_LL7A4:
 _tmp922=_tmp920;_tmp921=_tmp91F;goto _LL7A6;case 8: _LL7A5: _tmp922=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_tmp921=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp916)->f2;_LL7A6:
# 4685
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp922,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp921,te,cvtenv);
goto _LL794;case 13: _LL7A7: _tmp924=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_tmp923=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp916)->f2;_LL7A8:
# 4689
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp923,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp924,1,0);
goto _LL794;case 18: _LL7A9: _tmp925=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_LL7AA:
 _tmp926=_tmp925;goto _LL7AC;case 16: _LL7AB: _tmp926=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_LL7AC:
# 4694
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp926,1,0);
goto _LL794;case 38: _LL7AD: _tmp927=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_LL7AE:
# 4697
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp927,1,0);
goto _LL794;case 17: _LL7AF: _tmp928=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp916)->f1;_LL7B0:
# 4700
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp928,te,cvtenv);
goto _LL794;default: _LL7B1: _LL7B2:
# 4703
({void*_tmp917=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp918="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp918,sizeof(char),66);}),_tag_dyneither(_tmp917,sizeof(void*),0));});}_LL794:;}
# 4705
return cvtenv;}
# 4708
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4713
struct Cyc_List_List*_tmp929=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4716
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp92A=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp92B=_tmp92A;struct Cyc_Absyn_Tvar*_tmp92C;_LL7B4: _tmp92C=_tmp92B.f1;_LL7B5:;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp929,_tmp92C);}}
# 4724
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp92D=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp92E=_tmp92D;void*_tmp937;_LL7B7: _tmp937=_tmp92E.f1;_LL7B8:;{
void*_tmp92F=Cyc_Tcutil_compress(_tmp937);void*_tmp930=_tmp92F;struct Cyc_Core_Opt**_tmp936;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp930)->tag == 1){_LL7BA: _tmp936=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp930)->f4;_LL7BB:
# 4728
 if(*_tmp936 == 0)
*_tmp936=({struct Cyc_Core_Opt*_tmp931=_cycalloc(sizeof(*_tmp931));_tmp931->v=_tmp929;_tmp931;});else{
# 4732
struct Cyc_List_List*_tmp932=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp936))->v;
struct Cyc_List_List*_tmp933=0;
for(0;_tmp932 != 0;_tmp932=_tmp932->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp929,(struct Cyc_Absyn_Tvar*)_tmp932->hd))
_tmp933=({struct Cyc_List_List*_tmp934=_cycalloc(sizeof(*_tmp934));_tmp934->hd=(struct Cyc_Absyn_Tvar*)_tmp932->hd;_tmp934->tl=_tmp933;_tmp934;});}
*_tmp936=({struct Cyc_Core_Opt*_tmp935=_cycalloc(sizeof(*_tmp935));_tmp935->v=_tmp933;_tmp935;});}
# 4739
goto _LL7B9;}else{_LL7BC: _LL7BD:
 goto _LL7B9;}_LL7B9:;};}}
# 4743
return cvt;}
# 4749
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp938=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp939=_tmp938;switch(((struct Cyc_Absyn_Kind*)_tmp939)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp939)->aliasqual){case Cyc_Absyn_Unique: _LL7BF: _LL7C0:
# 4754
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp93A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp93B="can't unify evar with unique region!";_tag_dyneither(_tmp93B,sizeof(char),37);}),_tag_dyneither(_tmp93A,sizeof(void*),0));});
goto _LL7BE;case Cyc_Absyn_Aliasable: _LL7C1: _LL7C2:
 goto _LL7C4;default: _LL7C3: _LL7C4:
# 4759
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp93C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp93D="can't unify evar with heap!";_tag_dyneither(_tmp93D,sizeof(char),28);}),_tag_dyneither(_tmp93C,sizeof(void*),0));});
goto _LL7BE;}case Cyc_Absyn_EffKind: _LL7C5: _LL7C6:
# 4762
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp93E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp93F="can't unify evar with {}!";_tag_dyneither(_tmp93F,sizeof(char),26);}),_tag_dyneither(_tmp93E,sizeof(void*),0));});
goto _LL7BE;default: _LL7C7: _LL7C8:
# 4765
({struct Cyc_String_pa_PrintArg_struct _tmp943;_tmp943.tag=0;_tmp943.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));({struct Cyc_String_pa_PrintArg_struct _tmp942;_tmp942.tag=0;_tmp942.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));({void*_tmp940[2]={& _tmp942,& _tmp943};Cyc_Tcutil_terr(loc,({const char*_tmp941="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp941,sizeof(char),52);}),_tag_dyneither(_tmp940,sizeof(void*),2));});});});
goto _LL7BE;}_LL7BE:;}}
# 4777
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp944=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle*_tmp945=Cyc_Core_heap_region;
struct Cyc_Tcutil_CVTEnv _tmp946=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp964;_tmp964.r=_tmp945;_tmp964.kind_env=_tmp944;_tmp964.free_vars=0;_tmp964.free_evars=0;_tmp964.generalize_evars=generalize_evars;_tmp964.fn_result=0;_tmp964;}),expected_kind,1,t);
# 4786
struct Cyc_List_List*_tmp947=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp945,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp946.free_vars);
struct Cyc_List_List*_tmp948=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp945,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp946.free_evars);
# 4790
if(_tmp944 != 0){
struct Cyc_List_List*_tmp949=0;
{struct Cyc_List_List*_tmp94A=_tmp947;for(0;(unsigned int)_tmp94A;_tmp94A=_tmp94A->tl){
struct Cyc_Absyn_Tvar*_tmp94B=(struct Cyc_Absyn_Tvar*)_tmp94A->hd;
int found=0;
{struct Cyc_List_List*_tmp94C=_tmp944;for(0;(unsigned int)_tmp94C;_tmp94C=_tmp94C->tl){
if(Cyc_Absyn_tvar_cmp(_tmp94B,(struct Cyc_Absyn_Tvar*)_tmp94C->hd)== 0){found=1;break;}}}
if(!found)
_tmp949=({struct Cyc_List_List*_tmp94D=_region_malloc(_tmp945,sizeof(*_tmp94D));_tmp94D->hd=(struct Cyc_Absyn_Tvar*)_tmp94A->hd;_tmp94D->tl=_tmp949;_tmp94D;});}}
# 4800
_tmp947=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp949);}
# 4806
{struct Cyc_List_List*x=_tmp947;for(0;x != 0;x=x->tl){
void*_tmp94E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp94F=_tmp94E;enum Cyc_Absyn_AliasQual _tmp95D;struct Cyc_Core_Opt**_tmp95C;struct Cyc_Absyn_Kind*_tmp95B;struct Cyc_Core_Opt**_tmp95A;struct Cyc_Core_Opt**_tmp959;struct Cyc_Core_Opt**_tmp958;struct Cyc_Core_Opt**_tmp957;struct Cyc_Core_Opt**_tmp956;struct Cyc_Core_Opt**_tmp955;switch(*((int*)_tmp94F)){case 1: _LL7CA: _tmp955=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp94F)->f1;_LL7CB:
 _tmp956=_tmp955;goto _LL7CD;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f2)->aliasqual == Cyc_Absyn_Top){_LL7CC: _tmp956=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f1;_LL7CD:
 _tmp957=_tmp956;goto _LL7CF;}else{goto _LL7D6;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7CE: _tmp957=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f1;_LL7CF:
 _tmp958=_tmp957;goto _LL7D1;case Cyc_Absyn_Aliasable: _LL7D0: _tmp958=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f1;_LL7D1:
# 4812
*_tmp958=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL7C9;default: _LL7D2: _tmp959=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f1;_LL7D3:
# 4814
*_tmp959=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL7C9;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f2)->aliasqual == Cyc_Absyn_Top){_LL7D4: _tmp95A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f1;_LL7D5:
# 4816
*_tmp95A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL7C9;}else{goto _LL7D6;}default: _LL7D6: _tmp95C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f1;_tmp95B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp94F)->f2;_LL7D7:
# 4818
*_tmp95C=Cyc_Tcutil_kind_to_bound_opt(_tmp95B);goto _LL7C9;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94F)->f1)->kind == Cyc_Absyn_MemKind){_LL7D8: _tmp95D=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp94F)->f1)->aliasqual;_LL7D9:
# 4820
({struct Cyc_String_pa_PrintArg_struct _tmp953;_tmp953.tag=0;_tmp953.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp954=_cycalloc_atomic(sizeof(*_tmp954));_tmp954->kind=Cyc_Absyn_MemKind;_tmp954->aliasqual=_tmp95D;_tmp954;})));({struct Cyc_String_pa_PrintArg_struct _tmp952;_tmp952.tag=0;_tmp952.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));({void*_tmp950[2]={& _tmp952,& _tmp953};Cyc_Tcutil_terr(loc,({const char*_tmp951="type variable %s cannot have kind %s";_tag_dyneither(_tmp951,sizeof(char),37);}),_tag_dyneither(_tmp950,sizeof(void*),2));});});});
goto _LL7C9;}else{_LL7DA: _LL7DB:
 goto _LL7C9;}}_LL7C9:;}}
# 4827
if(_tmp947 != 0  || _tmp948 != 0){
{void*_tmp95E=Cyc_Tcutil_compress(t);void*_tmp95F=_tmp95E;struct Cyc_List_List**_tmp960;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95F)->tag == 9){_LL7DD: _tmp960=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95F)->f1).tvars;_LL7DE:
# 4830
 if(*_tmp960 == 0){
# 4832
*_tmp960=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp947);
_tmp947=0;}
# 4835
goto _LL7DC;}else{_LL7DF: _LL7E0:
 goto _LL7DC;}_LL7DC:;}
# 4838
if(_tmp947 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp963;_tmp963.tag=0;_tmp963.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp947->hd)->name);({void*_tmp961[1]={& _tmp963};Cyc_Tcutil_terr(loc,({const char*_tmp962="unbound type variable %s";_tag_dyneither(_tmp962,sizeof(char),25);}),_tag_dyneither(_tmp961,sizeof(void*),1));});});
Cyc_Tcutil_check_free_evars(_tmp948,t,loc);}}
# 4849
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4852
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp965=Cyc_Tcutil_compress(t);void*_tmp966=_tmp965;struct Cyc_List_List*_tmp972;void*_tmp971;struct Cyc_Absyn_Tqual _tmp970;void*_tmp96F;struct Cyc_List_List*_tmp96E;struct Cyc_Absyn_Exp*_tmp96D;struct Cyc_List_List*_tmp96C;struct Cyc_Absyn_Exp*_tmp96B;struct Cyc_List_List*_tmp96A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->tag == 9){_LL7E2: _tmp972=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).tvars;_tmp971=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).effect;_tmp970=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).ret_tqual;_tmp96F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).ret_typ;_tmp96E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).args;_tmp96D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).requires_clause;_tmp96C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).requires_relns;_tmp96B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).ensures_clause;_tmp96A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp966)->f1).ensures_relns;_LL7E3:
# 4855
 fd->tvs=_tmp972;
fd->effect=_tmp971;
{struct Cyc_List_List*_tmp967=fd->args;for(0;_tmp967 != 0;(_tmp967=_tmp967->tl,_tmp96E=_tmp96E->tl)){
# 4859
(*((struct _tuple10*)_tmp967->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp96E))->hd)).f2;
(*((struct _tuple10*)_tmp967->hd)).f3=(*((struct _tuple10*)_tmp96E->hd)).f3;}}
# 4862
fd->ret_tqual=_tmp970;
fd->ret_type=_tmp96F;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp96F);
fd->requires_clause=_tmp96D;
fd->requires_relns=_tmp96C;
fd->ensures_clause=_tmp96B;
fd->ensures_relns=_tmp96A;
goto _LL7E1;}else{_LL7E4: _LL7E5:
({void*_tmp968=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp969="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp969,sizeof(char),38);}),_tag_dyneither(_tmp968,sizeof(void*),0));});}_LL7E1:;}
# 4872
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp973="function declaration has repeated parameter";_tag_dyneither(_tmp973,sizeof(char),44);}));
# 4875
fd->cached_typ=t;}
# 4880
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4886
struct _RegionHandle*_tmp974=Cyc_Core_heap_region;
struct Cyc_Tcutil_CVTEnv _tmp975=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp97D;_tmp97D.r=_tmp974;_tmp97D.kind_env=bound_tvars;_tmp97D.free_vars=0;_tmp97D.free_evars=0;_tmp97D.generalize_evars=0;_tmp97D.fn_result=0;_tmp97D;}),expected_kind,allow_abs_aggr,t);
# 4890
struct Cyc_List_List*_tmp976=Cyc_Tcutil_remove_bound_tvars(_tmp974,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp974,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp975.free_vars),bound_tvars);
# 4892
struct Cyc_List_List*_tmp977=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(_tmp974,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp975.free_evars);
{struct Cyc_List_List*fs=_tmp976;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp978=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp97C;_tmp97C.tag=0;_tmp97C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp97B;_tmp97B.tag=0;_tmp97B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp978);({void*_tmp979[2]={& _tmp97B,& _tmp97C};Cyc_Tcutil_terr(loc,({const char*_tmp97A="unbound type variable %s in type %s";_tag_dyneither(_tmp97A,sizeof(char),36);}),_tag_dyneither(_tmp979,sizeof(void*),2));});});});}}
# 4897
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp977,t,loc);}
# 4902
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4907
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4913
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4916
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp981;_tmp981.tag=0;_tmp981.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));({struct Cyc_String_pa_PrintArg_struct _tmp980;_tmp980.tag=0;_tmp980.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp97E[2]={& _tmp980,& _tmp981};Cyc_Tcutil_terr(loc,({const char*_tmp97F="%s: %s";_tag_dyneither(_tmp97F,sizeof(char),7);}),_tag_dyneither(_tmp97E,sizeof(void*),2));});});});}}}
# 4922
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4926
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4930
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Absynpp_tvar2string,({const char*_tmp982="duplicate type variable";_tag_dyneither(_tmp982,sizeof(char),24);}));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4944 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4949
struct _RegionHandle _tmp983=_new_region("temp");struct _RegionHandle*temp=& _tmp983;_push_region(temp);
# 4953
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*_tmp984="";_tag_dyneither(_tmp984,sizeof(char),1);}))!= 0)
fields=({struct Cyc_List_List*_tmp985=_region_malloc(temp,sizeof(*_tmp985));_tmp985->hd=({struct _tuple32*_tmp986=_region_malloc(temp,sizeof(*_tmp986));_tmp986->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp986->f2=0;_tmp986;});_tmp985->tl=fields;_tmp985;});}}
# 4958
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 4960
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp9AC="struct";_tag_dyneither(_tmp9AC,sizeof(char),7);}):({const char*_tmp9AD="union";_tag_dyneither(_tmp9AD,sizeof(char),6);});
# 4963
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp987=(struct _tuple33*)des->hd;struct _tuple33*_tmp988=_tmp987;struct Cyc_List_List*_tmp9A4;void*_tmp9A3;_LL7E7: _tmp9A4=_tmp988->f1;_tmp9A3=_tmp988->f2;_LL7E8:;
if(_tmp9A4 == 0){
# 4968
struct Cyc_List_List*_tmp989=fields;
for(0;_tmp989 != 0;_tmp989=_tmp989->tl){
if(!(*((struct _tuple32*)_tmp989->hd)).f2){
(*((struct _tuple32*)_tmp989->hd)).f2=1;
(*((struct _tuple33*)des->hd)).f1=({struct Cyc_List_List*_tmp98A=_cycalloc(sizeof(*_tmp98A));_tmp98A->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp98B=_cycalloc(sizeof(*_tmp98B));_tmp98B[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp98C;_tmp98C.tag=1;_tmp98C.f1=((*((struct _tuple32*)_tmp989->hd)).f1)->name;_tmp98C;});_tmp98B;});_tmp98A->tl=0;_tmp98A;});
ans=({struct Cyc_List_List*_tmp98D=_region_malloc(rgn,sizeof(*_tmp98D));_tmp98D->hd=({struct _tuple34*_tmp98E=_region_malloc(rgn,sizeof(*_tmp98E));_tmp98E->f1=(*((struct _tuple32*)_tmp989->hd)).f1;_tmp98E->f2=_tmp9A3;_tmp98E;});_tmp98D->tl=ans;_tmp98D;});
break;}}
# 4976
if(_tmp989 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp991;_tmp991.tag=0;_tmp991.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp98F[1]={& _tmp991};Cyc_Tcutil_terr(loc,({const char*_tmp990="too many arguments to %s";_tag_dyneither(_tmp990,sizeof(char),25);}),_tag_dyneither(_tmp98F,sizeof(void*),1));});});}else{
if(_tmp9A4->tl != 0)
# 4980
({void*_tmp992=0;Cyc_Tcutil_terr(loc,({const char*_tmp993="multiple designators are not yet supported";_tag_dyneither(_tmp993,sizeof(char),43);}),_tag_dyneither(_tmp992,sizeof(void*),0));});else{
# 4983
void*_tmp994=(void*)_tmp9A4->hd;void*_tmp995=_tmp994;struct _dyneither_ptr*_tmp9A2;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp995)->tag == 0){_LL7EA: _LL7EB:
# 4985
({struct Cyc_String_pa_PrintArg_struct _tmp998;_tmp998.tag=0;_tmp998.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp996[1]={& _tmp998};Cyc_Tcutil_terr(loc,({const char*_tmp997="array designator used in argument to %s";_tag_dyneither(_tmp997,sizeof(char),40);}),_tag_dyneither(_tmp996,sizeof(void*),1));});});
goto _LL7E9;}else{_LL7EC: _tmp9A2=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp995)->f1;_LL7ED: {
# 4988
struct Cyc_List_List*_tmp999=fields;
for(0;_tmp999 != 0;_tmp999=_tmp999->tl){
if(Cyc_strptrcmp(_tmp9A2,((*((struct _tuple32*)_tmp999->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp999->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp99C;_tmp99C.tag=0;_tmp99C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9A2);({void*_tmp99A[1]={& _tmp99C};Cyc_Tcutil_terr(loc,({const char*_tmp99B="member %s has already been used as an argument";_tag_dyneither(_tmp99B,sizeof(char),47);}),_tag_dyneither(_tmp99A,sizeof(void*),1));});});
(*((struct _tuple32*)_tmp999->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp99D=_region_malloc(rgn,sizeof(*_tmp99D));_tmp99D->hd=({struct _tuple34*_tmp99E=_region_malloc(rgn,sizeof(*_tmp99E));_tmp99E->f1=(*((struct _tuple32*)_tmp999->hd)).f1;_tmp99E->f2=_tmp9A3;_tmp99E;});_tmp99D->tl=ans;_tmp99D;});
break;}}
# 4997
if(_tmp999 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9A1;_tmp9A1.tag=0;_tmp9A1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9A2);({void*_tmp99F[1]={& _tmp9A1};Cyc_Tcutil_terr(loc,({const char*_tmp9A0="bad field designator %s";_tag_dyneither(_tmp9A0,sizeof(char),24);}),_tag_dyneither(_tmp99F,sizeof(void*),1));});});
goto _LL7E9;}}_LL7E9:;}}}
# 5002
if(aggr_kind == Cyc_Absyn_StructA)
# 5004
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp9A5=0;Cyc_Tcutil_terr(loc,({const char*_tmp9A6="too few arguments to struct";_tag_dyneither(_tmp9A6,sizeof(char),28);}),_tag_dyneither(_tmp9A5,sizeof(void*),0));});
break;}}else{
# 5011
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp9A7=0;Cyc_Tcutil_terr(loc,({const char*_tmp9A8="only one member of a union is allowed";_tag_dyneither(_tmp9A8,sizeof(char),38);}),_tag_dyneither(_tmp9A7,sizeof(void*),0));});
found=1;}}
# 5018
if(!found)({void*_tmp9A9=0;Cyc_Tcutil_terr(loc,({const char*_tmp9AA="missing member for union";_tag_dyneither(_tmp9AA,sizeof(char),25);}),_tag_dyneither(_tmp9A9,sizeof(void*),0));});}{
# 5021
struct Cyc_List_List*_tmp9AB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmp9AB;};};}
# 4953
;_pop_region(temp);}
# 5024
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9AE=Cyc_Tcutil_compress(t);void*_tmp9AF=_tmp9AE;void*_tmp9B3;union Cyc_Absyn_Constraint*_tmp9B2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AF)->tag == 5){_LL7EF: _tmp9B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AF)->f1).elt_typ;_tmp9B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AF)->f1).ptr_atts).bounds;_LL7F0: {
# 5027
void*_tmp9B0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9B2);void*_tmp9B1=_tmp9B0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9B1)->tag == 0){_LL7F4: _LL7F5:
# 5029
*elt_typ_dest=_tmp9B3;
return 1;}else{_LL7F6: _LL7F7:
 return 0;}_LL7F3:;}}else{_LL7F1: _LL7F2:
# 5033
 return 0;}_LL7EE:;}
# 5037
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9B4=Cyc_Tcutil_compress(t);void*_tmp9B5=_tmp9B4;void*_tmp9B7;union Cyc_Absyn_Constraint*_tmp9B6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B5)->tag == 5){_LL7F9: _tmp9B7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B5)->f1).elt_typ;_tmp9B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B5)->f1).ptr_atts).zero_term;_LL7FA:
# 5040
*elt_typ_dest=_tmp9B7;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9B6);}else{_LL7FB: _LL7FC:
 return 0;}_LL7F8:;}
# 5048
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5050
void*_tmp9B8=Cyc_Tcutil_compress(t);void*_tmp9B9=_tmp9B8;void*_tmp9C2;struct Cyc_Absyn_Tqual _tmp9C1;struct Cyc_Absyn_Exp*_tmp9C0;union Cyc_Absyn_Constraint*_tmp9BF;void*_tmp9BE;union Cyc_Absyn_Constraint*_tmp9BD;union Cyc_Absyn_Constraint*_tmp9BC;switch(*((int*)_tmp9B9)){case 5: _LL7FE: _tmp9BE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B9)->f1).elt_typ;_tmp9BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B9)->f1).ptr_atts).bounds;_tmp9BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9B9)->f1).ptr_atts).zero_term;_LL7FF:
# 5052
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9BC)){
*ptr_type=t;
*elt_type=_tmp9BE;
{void*_tmp9BA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp9BD);void*_tmp9BB=_tmp9BA;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9BB)->tag == 0){_LL805: _LL806:
*is_dyneither=1;goto _LL804;}else{_LL807: _LL808:
*is_dyneither=0;goto _LL804;}_LL804:;}
# 5059
return 1;}else{
return 0;}case 8: _LL800: _tmp9C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9B9)->f1).elt_type;_tmp9C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9B9)->f1).tq;_tmp9C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9B9)->f1).num_elts;_tmp9BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9B9)->f1).zero_term;_LL801:
# 5062
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9BF)){
*elt_type=_tmp9C2;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}default: _LL802: _LL803:
 return 0;}_LL7FD:;}
# 5075
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5077
void*_tmp9C3=e1->r;void*_tmp9C4=_tmp9C3;struct Cyc_Absyn_Exp*_tmp9D6;struct Cyc_Absyn_Exp*_tmp9D5;struct Cyc_Absyn_Exp*_tmp9D4;struct Cyc_Absyn_Exp*_tmp9D3;struct Cyc_Absyn_Exp*_tmp9D2;struct Cyc_Absyn_Exp*_tmp9D1;switch(*((int*)_tmp9C4)){case 13: _LL80A: _LL80B:
# 5079
({struct Cyc_String_pa_PrintArg_struct _tmp9C7;_tmp9C7.tag=0;_tmp9C7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp9C5[1]={& _tmp9C7};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9C6="we have a cast in a lhs:  %s";_tag_dyneither(_tmp9C6,sizeof(char),29);}),_tag_dyneither(_tmp9C5,sizeof(void*),1));});});case 19: _LL80C: _tmp9D1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL80D:
 _tmp9D2=_tmp9D1;goto _LL80F;case 22: _LL80E: _tmp9D2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL80F:
# 5082
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9D2->topt),ptr_type,is_dyneither,elt_type);case 21: _LL810: _tmp9D3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL811:
 _tmp9D4=_tmp9D3;goto _LL813;case 20: _LL812: _tmp9D4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL813:
# 5086
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9D4->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9CA;_tmp9CA.tag=0;_tmp9CA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmp9C8[1]={& _tmp9CA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9C9="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp9C9,sizeof(char),51);}),_tag_dyneither(_tmp9C8,sizeof(void*),1));});});
return 0;case 12: _LL814: _tmp9D5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL815:
 _tmp9D6=_tmp9D5;goto _LL817;case 11: _LL816: _tmp9D6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9C4)->f1;_LL817:
# 5092
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9D6->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9CD;_tmp9CD.tag=0;_tmp9CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmp9CB[1]={& _tmp9CD};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9CC="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp9CC,sizeof(char),49);}),_tag_dyneither(_tmp9CB,sizeof(void*),1));});});
return 0;case 1: _LL818: _LL819:
 return 0;default: _LL81A: _LL81B:
({struct Cyc_String_pa_PrintArg_struct _tmp9D0;_tmp9D0.tag=0;_tmp9D0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp9CE[1]={& _tmp9D0};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9CF="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp9CF,sizeof(char),39);}),_tag_dyneither(_tmp9CE,sizeof(void*),1));});});}_LL809:;}
# 5101
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5112
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5115
void*_tmp9D7=Cyc_Tcutil_compress(r);void*_tmp9D8=_tmp9D7;struct Cyc_Absyn_Tvar*_tmp9E5;enum Cyc_Absyn_KindQual _tmp9E4;enum Cyc_Absyn_AliasQual _tmp9E3;switch(*((int*)_tmp9D8)){case 22: _LL81D: _LL81E:
 return !must_be_unique;case 21: _LL81F: _LL820:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9D8)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9D8)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9D8)->f4 == 0){_LL821: _tmp9E4=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9D8)->f3)->kind)->v)->kind;_tmp9E3=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9D8)->f3)->kind)->v)->aliasqual;_LL822:
# 5119
 return _tmp9E4 == Cyc_Absyn_RgnKind  && (_tmp9E3 == Cyc_Absyn_Unique  || _tmp9E3 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL825;}}else{goto _LL825;}}else{goto _LL825;}case 2: _LL823: _tmp9E5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D8)->f1;_LL824: {
# 5123
struct Cyc_Absyn_Kind*_tmp9D9=Cyc_Tcutil_tvar_kind(_tmp9E5,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp9DA=_tmp9D9;enum Cyc_Absyn_KindQual _tmp9E2;enum Cyc_Absyn_AliasQual _tmp9E1;_LL828: _tmp9E2=_tmp9DA->kind;_tmp9E1=_tmp9DA->aliasqual;_LL829:;
if(_tmp9E2 == Cyc_Absyn_RgnKind  && (_tmp9E1 == Cyc_Absyn_Unique  || _tmp9E1 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp9DB=Cyc_Absyn_compress_kb(_tmp9E5->kind);void*_tmp9DC=_tmp9DB;struct Cyc_Core_Opt**_tmp9E0;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DC)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DC)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DC)->f2)->aliasqual == Cyc_Absyn_Top){_LL82B: _tmp9E0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DC)->f1;_LL82C:
# 5127
*_tmp9E0=({struct Cyc_Core_Opt*_tmp9DD=_cycalloc(sizeof(*_tmp9DD));_tmp9DD->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9DE=_cycalloc(sizeof(*_tmp9DE));_tmp9DE[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9DF;_tmp9DF.tag=2;_tmp9DF.f1=0;_tmp9DF.f2=& Cyc_Tcutil_rk;_tmp9DF;});_tmp9DE;});_tmp9DD;});
return 0;}else{goto _LL82D;}}else{goto _LL82D;}}else{_LL82D: _LL82E:
 return 1;}_LL82A:;}
# 5132
return 0;}default: _LL825: _LL826:
 return 0;}_LL81C:;}
# 5139
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp9E6=Cyc_Tcutil_compress(t);void*_tmp9E7=_tmp9E6;struct Cyc_Absyn_Tvar*_tmp9F6;void*_tmp9F5;switch(*((int*)_tmp9E7)){case 5: _LL830: _tmp9F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9E7)->f1).ptr_atts).rgn;_LL831:
# 5142
 return Cyc_Tcutil_is_noalias_region(_tmp9F5,must_be_unique);case 2: _LL832: _tmp9F6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9E7)->f1;_LL833: {
# 5144
struct Cyc_Absyn_Kind*_tmp9E8=Cyc_Tcutil_tvar_kind(_tmp9F6,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp9E9=_tmp9E8;enum Cyc_Absyn_KindQual _tmp9F4;enum Cyc_Absyn_AliasQual _tmp9F3;_LL837: _tmp9F4=_tmp9E9->kind;_tmp9F3=_tmp9E9->aliasqual;_LL838:;{
enum Cyc_Absyn_KindQual _tmp9EA=_tmp9F4;switch(_tmp9EA){case Cyc_Absyn_BoxKind: _LL83A: _LL83B:
 goto _LL83D;case Cyc_Absyn_AnyKind: _LL83C: _LL83D: goto _LL83F;case Cyc_Absyn_MemKind: _LL83E: _LL83F:
 if(_tmp9F3 == Cyc_Absyn_Unique  || _tmp9F3 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp9EB=Cyc_Absyn_compress_kb(_tmp9F6->kind);void*_tmp9EC=_tmp9EB;struct Cyc_Core_Opt**_tmp9F2;enum Cyc_Absyn_KindQual _tmp9F1;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9EC)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9EC)->f2)->aliasqual == Cyc_Absyn_Top){_LL843: _tmp9F2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9EC)->f1;_tmp9F1=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9EC)->f2)->kind;_LL844:
# 5150
*_tmp9F2=({struct Cyc_Core_Opt*_tmp9ED=_cycalloc(sizeof(*_tmp9ED));_tmp9ED->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9EE=_cycalloc(sizeof(*_tmp9EE));_tmp9EE[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9EF;_tmp9EF.tag=2;_tmp9EF.f1=0;_tmp9EF.f2=({struct Cyc_Absyn_Kind*_tmp9F0=_cycalloc_atomic(sizeof(*_tmp9F0));_tmp9F0->kind=_tmp9F1;_tmp9F0->aliasqual=Cyc_Absyn_Aliasable;_tmp9F0;});_tmp9EF;});_tmp9EE;});_tmp9ED;});
return 0;}else{goto _LL845;}}else{_LL845: _LL846:
# 5154
 return 1;}_LL842:;}
# 5157
return 0;default: _LL840: _LL841:
 return 0;}_LL839:;};}default: _LL834: _LL835:
# 5160
 return 0;}_LL82F:;}
# 5163
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmp9F7=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp9F7,0))return 1;{
void*_tmp9F8=_tmp9F7;union Cyc_Absyn_DatatypeFieldInfoU _tmpA0C;struct Cyc_List_List*_tmpA0B;union Cyc_Absyn_DatatypeInfoU _tmpA0A;struct Cyc_List_List*_tmpA09;struct Cyc_List_List*_tmpA08;struct Cyc_Absyn_Aggrdecl**_tmpA07;struct Cyc_List_List*_tmpA06;struct Cyc_List_List*_tmpA05;switch(*((int*)_tmp9F8)){case 10: _LL848: _tmpA05=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9F8)->f1;_LL849:
# 5168
 while(_tmpA05 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpA05->hd)).f2))return 1;
_tmpA05=_tmpA05->tl;}
# 5172
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9F8)->f1).aggr_info).KnownAggr).tag == 2){_LL84A: _tmpA07=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9F8)->f1).aggr_info).KnownAggr).val;_tmpA06=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9F8)->f1).targs;_LL84B:
# 5174
 if((*_tmpA07)->impl == 0)return 0;else{
# 5176
struct Cyc_List_List*_tmp9F9=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA07)->tvs,_tmpA06);
struct Cyc_List_List*_tmp9FA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA07)->impl))->fields;
void*t;
while(_tmp9FA != 0){
t=_tmp9F9 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp9FA->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmp9F9,((struct Cyc_Absyn_Aggrfield*)_tmp9FA->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmp9FA=_tmp9FA->tl;}
# 5184
return 0;}}else{_LL84E: _LL84F:
# 5194
 return 0;}case 12: _LL84C: _tmpA08=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9F8)->f2;_LL84D:
# 5187
 while(_tmpA08 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA08->hd)->type))return 1;
_tmpA08=_tmpA08->tl;}
# 5191
return 0;case 3: _LL850: _tmpA0A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9F8)->f1).datatype_info;_tmpA09=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9F8)->f1).targs;_LL851: {
# 5196
union Cyc_Absyn_DatatypeInfoU _tmp9FB=_tmpA0A;struct Cyc_List_List*_tmp9FF;struct Cyc_Core_Opt*_tmp9FE;struct _tuple2*_tmp9FD;int _tmp9FC;if((_tmp9FB.UnknownDatatype).tag == 1){_LL857: _tmp9FD=((_tmp9FB.UnknownDatatype).val).name;_tmp9FC=((_tmp9FB.UnknownDatatype).val).is_extensible;_LL858:
# 5199
 return 0;}else{_LL859: _tmp9FF=(*(_tmp9FB.KnownDatatype).val)->tvs;_tmp9FE=(*(_tmp9FB.KnownDatatype).val)->fields;_LL85A:
# 5202
 return 0;}_LL856:;}case 4: _LL852: _tmpA0C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9F8)->f1).field_info;_tmpA0B=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9F8)->f1).targs;_LL853: {
# 5205
union Cyc_Absyn_DatatypeFieldInfoU _tmpA00=_tmpA0C;struct Cyc_Absyn_Datatypedecl*_tmpA04;struct Cyc_Absyn_Datatypefield*_tmpA03;if((_tmpA00.UnknownDatatypefield).tag == 1){_LL85C: _LL85D:
# 5208
 return 0;}else{_LL85E: _tmpA04=((_tmpA00.KnownDatatypefield).val).f1;_tmpA03=((_tmpA00.KnownDatatypefield).val).f2;_LL85F: {
# 5210
struct Cyc_List_List*_tmpA01=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA04->tvs,_tmpA0B);
struct Cyc_List_List*_tmpA02=_tmpA03->typs;
while(_tmpA02 != 0){
_tmp9F7=_tmpA01 == 0?(*((struct _tuple12*)_tmpA02->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpA01,(*((struct _tuple12*)_tmpA02->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmp9F7))return 1;
_tmpA02=_tmpA02->tl;}
# 5217
return 0;}}_LL85B:;}default: _LL854: _LL855:
# 5219
 return 0;}_LL847:;};}
# 5226
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpA0D=e->r;void*_tmpA0E=_tmpA0D;struct Cyc_Absyn_Stmt*_tmpA24;struct Cyc_Absyn_Exp*_tmpA23;struct Cyc_Absyn_Exp*_tmpA22;struct Cyc_Absyn_Exp*_tmpA21;struct Cyc_Absyn_Exp*_tmpA20;struct Cyc_Absyn_Exp*_tmpA1F;struct Cyc_Absyn_Exp*_tmpA1E;struct Cyc_Absyn_Exp*_tmpA1D;struct _dyneither_ptr*_tmpA1C;struct Cyc_Absyn_Exp*_tmpA1B;struct Cyc_Absyn_Exp*_tmpA1A;switch(*((int*)_tmpA0E)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1)->tag == 1){_LL861: _LL862:
 return 0;}else{goto _LL873;}case 21: _LL863: _tmpA1A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL864:
 _tmpA1B=_tmpA1A;goto _LL866;case 19: _LL865: _tmpA1B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL866:
# 5231
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA1B->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpA1B);case 20: _LL867: _tmpA1D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_tmpA1C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA0E)->f2;_LL868:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA1D);case 22: _LL869: _tmpA1F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_tmpA1E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA0E)->f2;_LL86A: {
# 5235
void*_tmpA0F=Cyc_Tcutil_compress((void*)_check_null(_tmpA1F->topt));void*_tmpA10=_tmpA0F;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA10)->tag == 10){_LL876: _LL877:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA1F);}else{_LL878: _LL879:
 return 0;}_LL875:;}case 5: _LL86B: _tmpA21=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA0E)->f2;_tmpA20=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA0E)->f3;_LL86C:
# 5240
 return Cyc_Tcutil_is_noalias_path(r,_tmpA21) && Cyc_Tcutil_is_noalias_path(r,_tmpA20);case 8: _LL86D: _tmpA22=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA0E)->f2;_LL86E:
 _tmpA23=_tmpA22;goto _LL870;case 13: _LL86F: _tmpA23=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA0E)->f2;_LL870:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA23);case 36: _LL871: _tmpA24=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA0E)->f1;_LL872:
# 5244
 while(1){
void*_tmpA11=_tmpA24->r;void*_tmpA12=_tmpA11;struct Cyc_Absyn_Exp*_tmpA19;struct Cyc_Absyn_Decl*_tmpA18;struct Cyc_Absyn_Stmt*_tmpA17;struct Cyc_Absyn_Stmt*_tmpA16;struct Cyc_Absyn_Stmt*_tmpA15;switch(*((int*)_tmpA12)){case 2: _LL87B: _tmpA16=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA12)->f1;_tmpA15=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA12)->f2;_LL87C:
 _tmpA24=_tmpA15;goto _LL87A;case 12: _LL87D: _tmpA18=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA12)->f1;_tmpA17=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA12)->f2;_LL87E:
 _tmpA24=_tmpA17;goto _LL87A;case 1: _LL87F: _tmpA19=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA12)->f1;_LL880:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA19);default: _LL881: _LL882:
({void*_tmpA13=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA14="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA14,sizeof(char),40);}),_tag_dyneither(_tmpA13,sizeof(void*),0));});}_LL87A:;}default: _LL873: _LL874:
# 5252
 return 1;}_LL860:;}
# 5269 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5271
struct _tuple18 bogus_ans=({struct _tuple18 _tmpA6C;_tmpA6C.f1=0;_tmpA6C.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA6C;});
void*_tmpA25=e->r;void*_tmpA26=_tmpA25;struct Cyc_Absyn_Exp*_tmpA6B;struct Cyc_Absyn_Exp*_tmpA6A;struct Cyc_Absyn_Exp*_tmpA69;struct Cyc_Absyn_Exp*_tmpA68;struct _dyneither_ptr*_tmpA67;int _tmpA66;struct Cyc_Absyn_Exp*_tmpA65;struct _dyneither_ptr*_tmpA64;int _tmpA63;void*_tmpA62;switch(*((int*)_tmpA26)){case 1: _LL884: _tmpA62=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA26)->f1;_LL885: {
# 5275
void*_tmpA27=_tmpA62;struct Cyc_Absyn_Vardecl*_tmpA34;struct Cyc_Absyn_Vardecl*_tmpA33;struct Cyc_Absyn_Vardecl*_tmpA32;struct Cyc_Absyn_Vardecl*_tmpA31;switch(*((int*)_tmpA27)){case 0: _LL891: _LL892:
 goto _LL894;case 2: _LL893: _LL894:
 return bogus_ans;case 1: _LL895: _tmpA31=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA27)->f1;_LL896: {
# 5279
void*_tmpA28=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA29=_tmpA28;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA29)->tag == 8){_LL89E: _LL89F:
# 5281
 return({struct _tuple18 _tmpA2A;_tmpA2A.f1=1;_tmpA2A.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA2A;});}else{_LL8A0: _LL8A1:
 return({struct _tuple18 _tmpA2B;_tmpA2B.f1=(_tmpA31->tq).real_const;_tmpA2B.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA2B;});}_LL89D:;}case 4: _LL897: _tmpA32=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA27)->f1;_LL898: {
# 5285
void*_tmpA2C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA2D=_tmpA2C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA2D)->tag == 8){_LL8A3: _LL8A4:
 return({struct _tuple18 _tmpA2E;_tmpA2E.f1=1;_tmpA2E.f2=(void*)_check_null(_tmpA32->rgn);_tmpA2E;});}else{_LL8A5: _LL8A6:
# 5288
 _tmpA32->escapes=1;
return({struct _tuple18 _tmpA2F;_tmpA2F.f1=(_tmpA32->tq).real_const;_tmpA2F.f2=(void*)_check_null(_tmpA32->rgn);_tmpA2F;});}_LL8A2:;}case 5: _LL899: _tmpA33=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA27)->f1;_LL89A:
# 5291
 _tmpA34=_tmpA33;goto _LL89C;default: _LL89B: _tmpA34=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA27)->f1;_LL89C:
# 5293
 _tmpA34->escapes=1;
return({struct _tuple18 _tmpA30;_tmpA30.f1=(_tmpA34->tq).real_const;_tmpA30.f2=(void*)_check_null(_tmpA34->rgn);_tmpA30;});}_LL890:;}case 20: _LL886: _tmpA65=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA26)->f1;_tmpA64=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA26)->f2;_tmpA63=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA26)->f3;_LL887:
# 5298
 if(_tmpA63)return bogus_ans;{
# 5301
void*_tmpA35=Cyc_Tcutil_compress((void*)_check_null(_tmpA65->topt));void*_tmpA36=_tmpA35;struct Cyc_Absyn_Aggrdecl*_tmpA44;struct Cyc_List_List*_tmpA43;switch(*((int*)_tmpA36)){case 12: _LL8A8: _tmpA43=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA36)->f2;_LL8A9: {
# 5303
struct Cyc_Absyn_Aggrfield*_tmpA37=Cyc_Absyn_lookup_field(_tmpA43,_tmpA64);
if(_tmpA37 != 0  && _tmpA37->width == 0){
struct _tuple18 _tmpA38=Cyc_Tcutil_addressof_props(te,_tmpA65);struct _tuple18 _tmpA39=_tmpA38;int _tmpA3C;void*_tmpA3B;_LL8AF: _tmpA3C=_tmpA39.f1;_tmpA3B=_tmpA39.f2;_LL8B0:;
return({struct _tuple18 _tmpA3A;_tmpA3A.f1=(_tmpA37->tq).real_const  || _tmpA3C;_tmpA3A.f2=_tmpA3B;_tmpA3A;});}
# 5308
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA36)->f1).aggr_info).KnownAggr).tag == 2){_LL8AA: _tmpA44=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA36)->f1).aggr_info).KnownAggr).val;_LL8AB: {
# 5310
struct Cyc_Absyn_Aggrfield*_tmpA3D=Cyc_Absyn_lookup_decl_field(_tmpA44,_tmpA64);
if(_tmpA3D != 0  && _tmpA3D->width == 0){
struct _tuple18 _tmpA3E=Cyc_Tcutil_addressof_props(te,_tmpA65);struct _tuple18 _tmpA3F=_tmpA3E;int _tmpA42;void*_tmpA41;_LL8B2: _tmpA42=_tmpA3F.f1;_tmpA41=_tmpA3F.f2;_LL8B3:;
return({struct _tuple18 _tmpA40;_tmpA40.f1=(_tmpA3D->tq).real_const  || _tmpA42;_tmpA40.f2=_tmpA41;_tmpA40;});}
# 5315
return bogus_ans;}}else{goto _LL8AC;}default: _LL8AC: _LL8AD:
 return bogus_ans;}_LL8A7:;};case 21: _LL888: _tmpA68=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA26)->f1;_tmpA67=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA26)->f2;_tmpA66=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA26)->f3;_LL889:
# 5320
 if(_tmpA66)return bogus_ans;{
# 5324
void*_tmpA45=Cyc_Tcutil_compress((void*)_check_null(_tmpA68->topt));void*_tmpA46=_tmpA45;void*_tmpA4D;void*_tmpA4C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA46)->tag == 5){_LL8B5: _tmpA4D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA46)->f1).elt_typ;_tmpA4C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA46)->f1).ptr_atts).rgn;_LL8B6: {
# 5326
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpA47=Cyc_Tcutil_compress(_tmpA4D);void*_tmpA48=_tmpA47;struct Cyc_Absyn_Aggrdecl*_tmpA4A;struct Cyc_List_List*_tmpA49;switch(*((int*)_tmpA48)){case 12: _LL8BA: _tmpA49=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA48)->f2;_LL8BB:
# 5329
 finfo=Cyc_Absyn_lookup_field(_tmpA49,_tmpA67);goto _LL8B9;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA48)->f1).aggr_info).KnownAggr).tag == 2){_LL8BC: _tmpA4A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA48)->f1).aggr_info).KnownAggr).val;_LL8BD:
# 5331
 finfo=Cyc_Absyn_lookup_decl_field(_tmpA4A,_tmpA67);goto _LL8B9;}else{goto _LL8BE;}default: _LL8BE: _LL8BF:
 return bogus_ans;}_LL8B9:;}
# 5334
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpA4B;_tmpA4B.f1=(finfo->tq).real_const;_tmpA4B.f2=_tmpA4C;_tmpA4B;});
return bogus_ans;}}else{_LL8B7: _LL8B8:
 return bogus_ans;}_LL8B4:;};case 19: _LL88A: _tmpA69=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA26)->f1;_LL88B: {
# 5341
void*_tmpA4E=Cyc_Tcutil_compress((void*)_check_null(_tmpA69->topt));void*_tmpA4F=_tmpA4E;struct Cyc_Absyn_Tqual _tmpA52;void*_tmpA51;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA4F)->tag == 5){_LL8C1: _tmpA52=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA4F)->f1).elt_tq;_tmpA51=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA4F)->f1).ptr_atts).rgn;_LL8C2:
# 5343
 return({struct _tuple18 _tmpA50;_tmpA50.f1=_tmpA52.real_const;_tmpA50.f2=_tmpA51;_tmpA50;});}else{_LL8C3: _LL8C4:
 return bogus_ans;}_LL8C0:;}case 22: _LL88C: _tmpA6B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA26)->f1;_tmpA6A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA26)->f2;_LL88D: {
# 5349
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpA6B->topt));
void*_tmpA53=t;struct Cyc_Absyn_Tqual _tmpA5F;struct Cyc_Absyn_Tqual _tmpA5E;void*_tmpA5D;struct Cyc_List_List*_tmpA5C;switch(*((int*)_tmpA53)){case 10: _LL8C6: _tmpA5C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA53)->f1;_LL8C7: {
# 5353
struct _tuple14 _tmpA54=Cyc_Evexp_eval_const_uint_exp(_tmpA6A);struct _tuple14 _tmpA55=_tmpA54;unsigned int _tmpA59;int _tmpA58;_LL8CF: _tmpA59=_tmpA55.f1;_tmpA58=_tmpA55.f2;_LL8D0:;
if(!_tmpA58)
return bogus_ans;{
struct _tuple12*_tmpA56=Cyc_Absyn_lookup_tuple_field(_tmpA5C,(int)_tmpA59);
if(_tmpA56 != 0)
return({struct _tuple18 _tmpA57;_tmpA57.f1=((*_tmpA56).f1).real_const;_tmpA57.f2=(Cyc_Tcutil_addressof_props(te,_tmpA6B)).f2;_tmpA57;});
return bogus_ans;};}case 5: _LL8C8: _tmpA5E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA53)->f1).elt_tq;_tmpA5D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA53)->f1).ptr_atts).rgn;_LL8C9:
# 5361
 return({struct _tuple18 _tmpA5A;_tmpA5A.f1=_tmpA5E.real_const;_tmpA5A.f2=_tmpA5D;_tmpA5A;});case 8: _LL8CA: _tmpA5F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA53)->f1).tq;_LL8CB:
# 5367
 return({struct _tuple18 _tmpA5B;_tmpA5B.f1=_tmpA5F.real_const;_tmpA5B.f2=(Cyc_Tcutil_addressof_props(te,_tmpA6B)).f2;_tmpA5B;});default: _LL8CC: _LL8CD:
 return bogus_ans;}_LL8C5:;}default: _LL88E: _LL88F:
# 5371
({void*_tmpA60=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpA61="unary & applied to non-lvalue";_tag_dyneither(_tmpA61,sizeof(char),30);}),_tag_dyneither(_tmpA60,sizeof(void*),0));});
return bogus_ans;}_LL883:;}
# 5378
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpA6D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpA6E=_tmpA6D;struct Cyc_Absyn_Exp*_tmpA77;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA6E)->tag == 0){_LL8D2: _LL8D3:
 return;}else{_LL8D4: _tmpA77=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA6E)->f1;_LL8D5: {
# 5383
struct _tuple14 _tmpA6F=Cyc_Evexp_eval_const_uint_exp(_tmpA77);struct _tuple14 _tmpA70=_tmpA6F;unsigned int _tmpA76;int _tmpA75;_LL8D7: _tmpA76=_tmpA70.f1;_tmpA75=_tmpA70.f2;_LL8D8:;
if(_tmpA75  && _tmpA76 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpA74;_tmpA74.tag=1;_tmpA74.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpA73;_tmpA73.tag=1;_tmpA73.f1=(unsigned long)((int)_tmpA76);({void*_tmpA71[2]={& _tmpA73,& _tmpA74};Cyc_Tcutil_terr(loc,({const char*_tmpA72="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpA72,sizeof(char),39);}),_tag_dyneither(_tmpA71,sizeof(void*),2));});});});
return;}}_LL8D1:;};}
# 5390
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5394
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpA78=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpA79=_tmpA78;struct Cyc_Absyn_Exp*_tmpA7E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA79)->tag == 1){_LL8DA: _tmpA7E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA79)->f1;_LL8DB: {
# 5397
struct _tuple14 _tmpA7A=Cyc_Evexp_eval_const_uint_exp(_tmpA7E);struct _tuple14 _tmpA7B=_tmpA7A;unsigned int _tmpA7D;int _tmpA7C;_LL8DF: _tmpA7D=_tmpA7B.f1;_tmpA7C=_tmpA7B.f2;_LL8E0:;
return _tmpA7C  && _tmpA7D == 1;}}else{_LL8DC: _LL8DD:
 return 0;}_LL8D9:;}
# 5403
int Cyc_Tcutil_bits_only(void*t){
void*_tmpA7F=Cyc_Tcutil_compress(t);void*_tmpA80=_tmpA7F;struct Cyc_List_List*_tmpA8A;struct Cyc_Absyn_Aggrdecl*_tmpA89;struct Cyc_List_List*_tmpA88;struct Cyc_List_List*_tmpA87;void*_tmpA86;union Cyc_Absyn_Constraint*_tmpA85;switch(*((int*)_tmpA80)){case 0: _LL8E2: _LL8E3:
 goto _LL8E5;case 6: _LL8E4: _LL8E5:
 goto _LL8E7;case 7: _LL8E6: _LL8E7:
 return 1;case 13: _LL8E8: _LL8E9:
 goto _LL8EB;case 14: _LL8EA: _LL8EB:
 return 0;case 8: _LL8EC: _tmpA86=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA80)->f1).elt_type;_tmpA85=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA80)->f1).zero_term;_LL8ED:
# 5413
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA85) && Cyc_Tcutil_bits_only(_tmpA86);case 10: _LL8EE: _tmpA87=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA80)->f1;_LL8EF:
# 5415
 for(0;_tmpA87 != 0;_tmpA87=_tmpA87->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpA87->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA80)->f1).aggr_info).UnknownAggr).tag == 1){_LL8F0: _LL8F1:
 return 0;}else{_LL8F2: _tmpA89=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA80)->f1).aggr_info).KnownAggr).val;_tmpA88=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA80)->f1).targs;_LL8F3:
# 5420
 if(_tmpA89->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA89->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpA81=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA81;_push_region(rgn);
{struct Cyc_List_List*_tmpA82=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA89->tvs,_tmpA88);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA89->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpA82,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA83=0;_npop_handler(0);return _tmpA83;}}}{
int _tmpA84=1;_npop_handler(0);return _tmpA84;};}
# 5427
;_pop_region(rgn);};};}case 12: _LL8F4: _tmpA8A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA80)->f2;_LL8F5:
# 5432
 for(0;_tmpA8A != 0;_tmpA8A=_tmpA8A->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpA8A->hd)->type))return 0;}
return 1;default: _LL8F6: _LL8F7:
 return 0;}_LL8E1:;}
# 5440
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpA8B=e->r;void*_tmpA8C=_tmpA8B;struct Cyc_Absyn_Exp*_tmpA8E;struct Cyc_Absyn_Exp*_tmpA8D;switch(*((int*)_tmpA8C)){case 1: _LL8F9: _LL8FA:
 return 1;case 11: _LL8FB: _tmpA8D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA8C)->f1;_LL8FC:
 _tmpA8E=_tmpA8D;goto _LL8FE;case 12: _LL8FD: _tmpA8E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA8C)->f1;_LL8FE:
 e=_tmpA8E;continue;default: _LL8FF: _LL900:
# 5447
 return 0;}_LL8F8:;}}
# 5457
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpA8F=e->r;void*_tmpA90=_tmpA8F;struct Cyc_List_List*_tmpAAE;struct Cyc_List_List*_tmpAAD;enum Cyc_Absyn_Primop _tmpAAC;struct Cyc_List_List*_tmpAAB;struct Cyc_List_List*_tmpAAA;struct Cyc_List_List*_tmpAA9;struct Cyc_List_List*_tmpAA8;struct Cyc_Absyn_Exp*_tmpAA7;struct Cyc_Absyn_Exp*_tmpAA6;struct Cyc_Absyn_Exp*_tmpAA5;struct Cyc_Absyn_Exp*_tmpAA4;void*_tmpAA3;struct Cyc_Absyn_Exp*_tmpAA2;void*_tmpAA1;struct Cyc_Absyn_Exp*_tmpAA0;struct Cyc_Absyn_Exp*_tmpA9F;struct Cyc_Absyn_Exp*_tmpA9E;struct Cyc_Absyn_Exp*_tmpA9D;struct Cyc_Absyn_Exp*_tmpA9C;struct Cyc_Absyn_Exp*_tmpA9B;struct Cyc_Absyn_Exp*_tmpA9A;struct Cyc_Absyn_Exp*_tmpA99;void*_tmpA98;switch(*((int*)_tmpA90)){case 0: _LL902: _LL903:
 goto _LL905;case 16: _LL904: _LL905:
 goto _LL907;case 17: _LL906: _LL907:
 goto _LL909;case 18: _LL908: _LL909:
 goto _LL90B;case 31: _LL90A: _LL90B:
 goto _LL90D;case 32: _LL90C: _LL90D:
 return 1;case 1: _LL90E: _tmpA98=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_LL90F: {
# 5468
void*_tmpA91=_tmpA98;struct Cyc_Absyn_Vardecl*_tmpA97;struct Cyc_Absyn_Vardecl*_tmpA96;switch(*((int*)_tmpA91)){case 2: _LL931: _LL932:
 return 1;case 1: _LL933: _tmpA96=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA91)->f1;_LL934: {
# 5471
void*_tmpA92=Cyc_Tcutil_compress(_tmpA96->type);void*_tmpA93=_tmpA92;switch(*((int*)_tmpA93)){case 8: _LL93C: _LL93D:
 goto _LL93F;case 9: _LL93E: _LL93F:
 return 1;default: _LL940: _LL941:
 return var_okay;}_LL93B:;}case 4: _LL935: _tmpA97=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA91)->f1;_LL936:
# 5478
 if(_tmpA97->sc == Cyc_Absyn_Static){
void*_tmpA94=Cyc_Tcutil_compress(_tmpA97->type);void*_tmpA95=_tmpA94;switch(*((int*)_tmpA95)){case 8: _LL943: _LL944:
 goto _LL946;case 9: _LL945: _LL946:
 return 1;default: _LL947: _LL948:
 return var_okay;}_LL942:;}else{
# 5485
return var_okay;}case 0: _LL937: _LL938:
 return 0;default: _LL939: _LL93A:
 return var_okay;}_LL930:;}case 5: _LL910: _tmpA9B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_tmpA9A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA90)->f2;_tmpA99=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA90)->f3;_LL911:
# 5491
 return(Cyc_Tcutil_cnst_exp(0,_tmpA9B) && 
Cyc_Tcutil_cnst_exp(0,_tmpA9A)) && 
Cyc_Tcutil_cnst_exp(0,_tmpA99);case 8: _LL912: _tmpA9D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_tmpA9C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA90)->f2;_LL913:
# 5495
 return Cyc_Tcutil_cnst_exp(0,_tmpA9D) && Cyc_Tcutil_cnst_exp(0,_tmpA9C);case 11: _LL914: _tmpA9E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_LL915:
 _tmpA9F=_tmpA9E;goto _LL917;case 12: _LL916: _tmpA9F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_LL917:
# 5498
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA9F);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA90)->f4 == Cyc_Absyn_No_coercion){_LL918: _tmpAA1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_tmpAA0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA90)->f2;_LL919:
# 5500
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAA0);}else{_LL91A: _tmpAA3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_tmpAA2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA90)->f2;_LL91B:
# 5503
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAA2);}case 14: _LL91C: _tmpAA4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_LL91D:
# 5505
 return Cyc_Tcutil_cnst_exp(1,_tmpAA4);case 26: _LL91E: _tmpAA6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA90)->f2;_tmpAA5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA90)->f3;_LL91F:
# 5507
 return Cyc_Tcutil_cnst_exp(0,_tmpAA6) && Cyc_Tcutil_cnst_exp(0,_tmpAA5);case 27: _LL920: _tmpAA7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_LL921:
# 5509
 return Cyc_Tcutil_cnst_exp(0,_tmpAA7);case 25: _LL922: _tmpAA8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_LL923:
 _tmpAA9=_tmpAA8;goto _LL925;case 29: _LL924: _tmpAA9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA90)->f2;_LL925:
 _tmpAAA=_tmpAA9;goto _LL927;case 28: _LL926: _tmpAAA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA90)->f3;_LL927:
# 5513
 for(0;_tmpAAA != 0;_tmpAAA=_tmpAAA->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpAAA->hd)).f2))
return 0;}
return 1;case 2: _LL928: _tmpAAC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_tmpAAB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA90)->f2;_LL929:
# 5518
 _tmpAAD=_tmpAAB;goto _LL92B;case 23: _LL92A: _tmpAAD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_LL92B:
 _tmpAAE=_tmpAAD;goto _LL92D;case 30: _LL92C: _tmpAAE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA90)->f1;_LL92D:
# 5521
 for(0;_tmpAAE != 0;_tmpAAE=_tmpAAE->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpAAE->hd))
return 0;}
return 1;default: _LL92E: _LL92F:
 return 0;}_LL901:;}
# 5529
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5533
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5535
int Cyc_Tcutil_supports_default(void*t){
void*_tmpAAF=Cyc_Tcutil_compress(t);void*_tmpAB0=_tmpAAF;struct Cyc_List_List*_tmpABA;union Cyc_Absyn_AggrInfoU _tmpAB9;struct Cyc_List_List*_tmpAB8;struct Cyc_List_List*_tmpAB7;void*_tmpAB6;union Cyc_Absyn_Constraint*_tmpAB5;union Cyc_Absyn_Constraint*_tmpAB4;switch(*((int*)_tmpAB0)){case 0: _LL94A: _LL94B:
 goto _LL94D;case 6: _LL94C: _LL94D:
 goto _LL94F;case 7: _LL94E: _LL94F:
 return 1;case 5: _LL950: _tmpAB5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB0)->f1).ptr_atts).nullable;_tmpAB4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB0)->f1).ptr_atts).bounds;_LL951: {
# 5542
void*_tmpAB1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAB4);void*_tmpAB2=_tmpAB1;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAB2)->tag == 0){_LL961: _LL962:
 return 1;}else{_LL963: _LL964:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpAB5);}_LL960:;}case 8: _LL952: _tmpAB6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAB0)->f1).elt_type;_LL953:
# 5547
 return Cyc_Tcutil_supports_default(_tmpAB6);case 10: _LL954: _tmpAB7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAB0)->f1;_LL955:
# 5549
 for(0;_tmpAB7 != 0;_tmpAB7=_tmpAB7->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpAB7->hd)).f2))return 0;}
return 1;case 11: _LL956: _tmpAB9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB0)->f1).aggr_info;_tmpAB8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAB0)->f1).targs;_LL957: {
# 5553
struct Cyc_Absyn_Aggrdecl*_tmpAB3=Cyc_Absyn_get_known_aggrdecl(_tmpAB9);
if(_tmpAB3->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAB3->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpAB3->tvs,_tmpAB8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAB3->impl))->fields);}case 12: _LL958: _tmpABA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAB0)->f2;_LL959:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpABA);case 14: _LL95A: _LL95B:
# 5559
 goto _LL95D;case 13: _LL95C: _LL95D:
 return 1;default: _LL95E: _LL95F:
# 5562
 return 0;}_LL949:;}
# 5567
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpABB=t;struct Cyc_Absyn_Typedefdecl*_tmpAC0;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpABB)->tag == 17){_LL966: _tmpAC0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpABB)->f3;_LL967:
# 5570
 if(_tmpAC0 != 0){
struct Cyc_Absyn_Tqual _tmpABC=_tmpAC0->tq;
if(((_tmpABC.print_const  || _tmpABC.q_volatile) || _tmpABC.q_restrict) || _tmpABC.real_const)
# 5575
({struct Cyc_String_pa_PrintArg_struct _tmpABF;_tmpABF.tag=0;_tmpABF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpABD[1]={& _tmpABF};Cyc_Tcutil_warn(loc,({const char*_tmpABE="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpABE,sizeof(char),44);}),_tag_dyneither(_tmpABD,sizeof(void*),1));});});}
# 5578
goto _LL965;}else{_LL968: _LL969:
 goto _LL965;}_LL965:;}
# 5583
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5585
struct _RegionHandle _tmpAC1=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAC1;_push_region(rgn);
{struct Cyc_List_List*_tmpAC2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpAC3=1;_npop_handler(0);return _tmpAC3;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpAC2,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpAC4=0;_npop_handler(0);return _tmpAC4;}}{
# 5593
int _tmpAC5=1;_npop_handler(0);return _tmpAC5;};}
# 5586
;_pop_region(rgn);}
# 5599
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpAC6=Cyc_Tcutil_compress(t);void*_tmpAC7=_tmpAC6;union Cyc_Absyn_Constraint*_tmpACB;union Cyc_Absyn_Constraint*_tmpACA;switch(*((int*)_tmpAC7)){case 6: _LL96B: _LL96C:
 goto _LL96E;case 7: _LL96D: _LL96E:
 return 1;case 5: _LL96F: _tmpACB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC7)->f1).ptr_atts).nullable;_tmpACA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC7)->f1).ptr_atts).bounds;_LL970: {
# 5604
void*_tmpAC8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpACA);void*_tmpAC9=_tmpAC8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAC9)->tag == 0){_LL974: _LL975:
# 5608
 return 0;}else{_LL976: _LL977:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpACB);}_LL973:;}default: _LL971: _LL972:
# 5611
 return 0;}_LL96A:;}
# 5615
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpACC=Cyc_Tcutil_compress(t);void*_tmpACD=_tmpACC;struct Cyc_List_List*_tmpAD1;void*_tmpAD0;switch(*((int*)_tmpACD)){case 5: _LL979: _tmpAD0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpACD)->f1).elt_typ;_LL97A:
 return Cyc_Tcutil_is_noreturn(_tmpAD0);case 9: _LL97B: _tmpAD1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpACD)->f1).attributes;_LL97C:
# 5619
 for(0;_tmpAD1 != 0;_tmpAD1=_tmpAD1->tl){
void*_tmpACE=(void*)_tmpAD1->hd;void*_tmpACF=_tmpACE;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpACF)->tag == 4){_LL980: _LL981:
 return 1;}else{_LL982: _LL983:
 continue;}_LL97F:;}
# 5624
goto _LL978;default: _LL97D: _LL97E:
 goto _LL978;}_LL978:;}
# 5627
return 0;}
# 5632
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpAD2=Cyc_Tcutil_compress(t);void*_tmpAD3=_tmpAD2;struct Cyc_List_List**_tmpAD9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAD3)->tag == 9){_LL985: _tmpAD9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAD3)->f1).attributes;_LL986: {
# 5635
struct Cyc_List_List*_tmpAD4=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpAD9,(void*)atts->hd))
*_tmpAD9=({struct Cyc_List_List*_tmpAD5=_cycalloc(sizeof(*_tmpAD5));_tmpAD5->hd=(void*)atts->hd;_tmpAD5->tl=*_tmpAD9;_tmpAD5;});}else{
# 5642
_tmpAD4=({struct Cyc_List_List*_tmpAD6=_cycalloc(sizeof(*_tmpAD6));_tmpAD6->hd=(void*)atts->hd;_tmpAD6->tl=_tmpAD4;_tmpAD6;});}}
return _tmpAD4;}}else{_LL987: _LL988:
({void*_tmpAD7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpAD8="transfer_fn_type_atts";_tag_dyneither(_tmpAD8,sizeof(char),22);}),_tag_dyneither(_tmpAD7,sizeof(void*),0));});}_LL984:;}
# 5649
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpADA=Cyc_Tcutil_compress(t);void*_tmpADB=_tmpADA;struct Cyc_Absyn_Exp*_tmpAE0;struct Cyc_Absyn_PtrInfo*_tmpADF;switch(*((int*)_tmpADB)){case 5: _LL98A: _tmpADF=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpADB)->f1;_LL98B:
# 5653
{void*_tmpADC=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpADF->ptr_atts).bounds);void*_tmpADD=_tmpADC;struct Cyc_Absyn_Exp*_tmpADE;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpADD)->tag == 1){_LL991: _tmpADE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpADD)->f1;_LL992:
 bound=_tmpADE;goto _LL990;}else{_LL993: _LL994:
 goto _LL990;}_LL990:;}
# 5657
goto _LL989;case 8: _LL98C: _tmpAE0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADB)->f1).num_elts;_LL98D:
# 5659
 bound=_tmpAE0;
goto _LL989;default: _LL98E: _LL98F:
 goto _LL989;}_LL989:;}
# 5663
return bound;}
# 5668
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpAE1=b;struct Cyc_Absyn_Vardecl*_tmpAE5;struct Cyc_Absyn_Vardecl*_tmpAE4;struct Cyc_Absyn_Vardecl*_tmpAE3;struct Cyc_Absyn_Vardecl*_tmpAE2;switch(*((int*)_tmpAE1)){case 5: _LL996: _tmpAE2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpAE1)->f1;_LL997:
 _tmpAE3=_tmpAE2;goto _LL999;case 4: _LL998: _tmpAE3=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpAE1)->f1;_LL999:
 _tmpAE4=_tmpAE3;goto _LL99B;case 3: _LL99A: _tmpAE4=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpAE1)->f1;_LL99B:
 _tmpAE5=_tmpAE4;goto _LL99D;case 1: _LL99C: _tmpAE5=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpAE1)->f1;_LL99D:
# 5674
 if(!_tmpAE5->escapes)return _tmpAE5;
goto _LL995;default: _LL99E: _LL99F:
 goto _LL995;}_LL995:;}
# 5678
return 0;}
# 5682
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpAE6=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmpAE6=({struct Cyc_List_List*_tmpAE7=_cycalloc(sizeof(*_tmpAE7));_tmpAE7->hd=*((void**)_check_null((void**)x->hd));_tmpAE7->tl=_tmpAE6;_tmpAE7;});}}
return _tmpAE6;}
# 5689
int Cyc_Tcutil_is_array(void*t){
void*_tmpAE8=Cyc_Tcutil_compress(t);void*_tmpAE9=_tmpAE8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE9)->tag == 8){_LL9A1: _LL9A2:
 return 1;}else{_LL9A3: _LL9A4:
 return 0;}_LL9A0:;}
# 5696
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmpAEA=Cyc_Tcutil_compress(t);void*_tmpAEB=_tmpAEA;void*_tmpAFB;struct Cyc_Absyn_Tqual _tmpAFA;struct Cyc_Absyn_Exp*_tmpAF9;union Cyc_Absyn_Constraint*_tmpAF8;unsigned int _tmpAF7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAEB)->tag == 8){_LL9A6: _tmpAFB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAEB)->f1).elt_type;_tmpAFA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAEB)->f1).tq;_tmpAF9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAEB)->f1).num_elts;_tmpAF8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAEB)->f1).zero_term;_tmpAF7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAEB)->f1).zt_loc;_LL9A7: {
# 5699
void*b;
if(_tmpAF9 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5703
struct Cyc_Absyn_Exp*bnd=_tmpAF9;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpAEC=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpAED="cannot convert tag without type!";_tag_dyneither(_tmpAED,sizeof(char),33);}),_tag_dyneither(_tmpAEC,sizeof(void*),0));});{
void*_tmpAEE=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpAEF=_tmpAEE;void*_tmpAF4;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpAEF)->tag == 19){_LL9AB: _tmpAF4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpAEF)->f1;_LL9AC:
# 5709
 b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAF0=_cycalloc(sizeof(*_tmpAF0));_tmpAF0[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAF1;_tmpAF1.tag=1;_tmpAF1.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpAF4,0),0,Cyc_Absyn_No_coercion,0);_tmpAF1;});_tmpAF0;});
goto _LL9AA;}else{_LL9AD: _LL9AE:
# 5712
 if(Cyc_Tcutil_is_const_exp(bnd))
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAF2=_cycalloc(sizeof(*_tmpAF2));_tmpAF2[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAF3;_tmpAF3.tag=1;_tmpAF3.f1=bnd;_tmpAF3;});_tmpAF2;});else{
# 5715
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL9AA:;};}else{
# 5719
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAF5=_cycalloc(sizeof(*_tmpAF5));_tmpAF5[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAF6;_tmpAF6.tag=1;_tmpAF6.f1=bnd;_tmpAF6;});_tmpAF5;});}}
# 5721
return Cyc_Absyn_atb_typ(_tmpAFB,rgn,_tmpAFA,b,_tmpAF8);}}else{_LL9A8: _LL9A9:
# 5724
 return t;}_LL9A5:;}
# 5729
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpAFC=Cyc_Tcutil_compress(t);void*_tmpAFD=_tmpAFC;struct Cyc_List_List*_tmpB0B;union Cyc_Absyn_AggrInfoU _tmpB0A;struct Cyc_List_List*_tmpB09;struct Cyc_List_List*_tmpB08;void*_tmpB07;union Cyc_Absyn_Constraint*_tmpB06;switch(*((int*)_tmpAFD)){case 0: _LL9B0: _LL9B1:
 return 1;case 1: _LL9B2: _LL9B3:
 goto _LL9B5;case 2: _LL9B4: _LL9B5:
 goto _LL9B7;case 3: _LL9B6: _LL9B7:
 goto _LL9B9;case 4: _LL9B8: _LL9B9:
 return 0;case 5: _LL9BA: _tmpB06=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAFD)->f1).ptr_atts).nullable;_LL9BB:
# 5737
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB06);case 6: _LL9BC: _LL9BD:
 goto _LL9BF;case 7: _LL9BE: _LL9BF:
 return 1;case 8: _LL9C0: _tmpB07=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAFD)->f1).elt_type;_LL9C1:
 return Cyc_Tcutil_zeroable_type(_tmpB07);case 9: _LL9C2: _LL9C3:
 return 0;case 10: _LL9C4: _tmpB08=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAFD)->f1;_LL9C5:
# 5743
 for(0;(unsigned int)_tmpB08;_tmpB08=_tmpB08->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpB08->hd)).f2))return 0;}
return 1;case 11: _LL9C6: _tmpB0A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAFD)->f1).aggr_info;_tmpB09=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAFD)->f1).targs;_LL9C7: {
# 5747
struct Cyc_Absyn_Aggrdecl*_tmpAFE=Cyc_Absyn_get_known_aggrdecl(_tmpB0A);
if(_tmpAFE->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAFE->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpAFF=_new_region("r");struct _RegionHandle*r=& _tmpAFF;_push_region(r);
{struct Cyc_List_List*_tmpB00=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpAFE->tvs,_tmpB09);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAFE->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpB00,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB01=0;_npop_handler(0);return _tmpB01;}}}{
int _tmpB02=1;_npop_handler(0);return _tmpB02;};}
# 5751
;_pop_region(r);};}case 13: _LL9C8: _LL9C9:
# 5755
 goto _LL9CB;case 19: _LL9CA: _LL9CB:
 return 1;case 12: _LL9CC: _tmpB0B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAFD)->f2;_LL9CD:
# 5758
 for(0;_tmpB0B != 0;_tmpB0B=_tmpB0B->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB0B->hd)->type))return 0;}
return 1;case 14: _LL9CE: _LL9CF:
 return 1;case 17: _LL9D0: _LL9D1:
 goto _LL9D3;case 16: _LL9D2: _LL9D3:
 goto _LL9D5;case 15: _LL9D4: _LL9D5:
 goto _LL9D7;case 28: _LL9D6: _LL9D7:
 return 0;case 26: _LL9D8: _LL9D9:
# 5767
 goto _LL9DB;case 27: _LL9DA: _LL9DB:
 goto _LL9DD;case 18: _LL9DC: _LL9DD:
 goto _LL9DF;case 20: _LL9DE: _LL9DF:
 goto _LL9E1;case 21: _LL9E0: _LL9E1:
 goto _LL9E3;case 22: _LL9E2: _LL9E3:
 goto _LL9E5;case 23: _LL9E4: _LL9E5:
 goto _LL9E7;case 24: _LL9E6: _LL9E7:
 goto _LL9E9;default: _LL9E8: _LL9E9:
({struct Cyc_String_pa_PrintArg_struct _tmpB05;_tmpB05.tag=0;_tmpB05.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB03[1]={& _tmpB05};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB04="bad type `%s' in zeroable type";_tag_dyneither(_tmpB04,sizeof(char),31);}),_tag_dyneither(_tmpB03,sizeof(void*),1));});});}_LL9AF:;}
