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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _dyneither_ptr,struct _dyneither_ptr);
# 232 "cycboot.h"
struct _dyneither_ptr Cyc_vrprintf(struct _RegionHandle*,struct _dyneither_ptr,struct _dyneither_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 110 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 70
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
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
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
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
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 37 "position.h"
struct _dyneither_ptr Cyc_Position_string_of_segment(unsigned int);struct Cyc_Position_Error;
# 43
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned int,struct _dyneither_ptr);
# 47
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;
void Cyc_Position_post_error(struct Cyc_Position_Error*);struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0U,Cyc_Absyn_UnionA  = 1U};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 247
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 315
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 334
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 346
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 525
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
# 966
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 973
extern void*Cyc_Absyn_bounds_one;
# 978
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _dyneither_ptr s,unsigned int loc);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1050
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
# 1062
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1108
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1157
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1159
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1161
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1163
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1167
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1169
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
# 70 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_bogus_fenv(void*ret_type,struct Cyc_List_List*args);
# 75
struct Cyc_Absyn_Aggrdecl**Cyc_Tcenv_lookup_aggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Datatypedecl**Cyc_Tcenv_lookup_datatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 78
struct Cyc_Absyn_Enumdecl**Cyc_Tcenv_lookup_enumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
struct Cyc_Absyn_Typedefdecl*Cyc_Tcenv_lookup_typedefdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct _tuple2*);
# 81
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
# 84
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
# 94
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 141
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
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t);
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 96
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 108
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 113
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 125
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 130
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 134
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 139
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 141
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 145
void Cyc_Tcutil_explain_failure();
# 147
int Cyc_Tcutil_unify(void*,void*);
# 149
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 152
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 157
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 161
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 165
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 168
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 172
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 174
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 178
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 184
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 211 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 213
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 221
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 224
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 231
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 233
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 235
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 237
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 239
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 245
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 249
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 253
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 259
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 264
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 267
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 272
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 277
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple18{int f1;void*f2;};
# 281
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 289
void*Cyc_Tcutil_normalize_effect(void*e);
# 292
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 294
int Cyc_Tcutil_new_tvar_id();
# 296
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 299
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 301
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 304
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 308
int Cyc_Tcutil_bits_only(void*t);
# 311
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 314
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 317
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 321
int Cyc_Tcutil_supports_default(void*);
# 325
int Cyc_Tcutil_admits_zero(void*t);
# 328
int Cyc_Tcutil_is_noreturn(void*);
# 332
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 336
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 340
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 343
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 346
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 350
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 353
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 356
int Cyc_Tcutil_is_array(void*t);
# 360
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 363
int Cyc_Tcutil_zeroable_type(void*t);
# 28 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);struct Cyc_Tcexp_TestEnv{struct _tuple0*eq;int isTrue;};
# 36
struct Cyc_Tcexp_TestEnv Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,struct _dyneither_ptr msg_part);
# 40 "tc.h"
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Aggrdecl*);
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Datatypedecl*);
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*,unsigned int,struct Cyc_Absyn_Enumdecl*);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c  = 0U,Cyc_Cyclone_Vc_c  = 1U};struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 69
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 83
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 109
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);char Cyc_Tcutil_Unify[6U]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
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
if(({struct _dyneither_ptr _tmpB02=({const char*_tmp1="(qualifiers don't match)";_tag_dyneither(_tmp1,sizeof(char),25U);});Cyc_strcmp(_tmpB02,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0U;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp2[1U]={& _tmp4};({struct Cyc___cycFILE*_tmpB04=Cyc_stderr;struct _dyneither_ptr _tmpB03=({const char*_tmp3="  %s\n";_tag_dyneither(_tmp3,sizeof(char),6U);});Cyc_fprintf(_tmpB04,_tmpB03,_tag_dyneither(_tmp2,sizeof(void*),1U));});});});
return;}
# 68
if(({struct _dyneither_ptr _tmpB05=({const char*_tmp5="(function effects do not match)";_tag_dyneither(_tmp5,sizeof(char),32U);});Cyc_strcmp(_tmpB05,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp9;_tmp9.tag=0U;_tmp9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp7[1U]={& _tmp9};({struct Cyc___cycFILE*_tmpB07=Cyc_stderr;struct _dyneither_ptr _tmpB06=({const char*_tmp8="  %s";_tag_dyneither(_tmp8,sizeof(char),5U);});Cyc_fprintf(_tmpB07,_tmpB06,_tag_dyneither(_tmp7,sizeof(void*),1U));});});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpA=0U;({struct Cyc___cycFILE*_tmpB09=Cyc_stderr;struct _dyneither_ptr _tmpB08=({const char*_tmpB="\n\t";_tag_dyneither(_tmpB,sizeof(char),3U);});Cyc_fprintf(_tmpB09,_tmpB08,_tag_dyneither(_tmpA,sizeof(void*),0U));});});
pos=8;}else{
# 82
({void*_tmpC=0U;({struct Cyc___cycFILE*_tmpB0B=Cyc_stderr;struct _dyneither_ptr _tmpB0A=({const char*_tmpD=" ";_tag_dyneither(_tmpD,sizeof(char),2U);});Cyc_fprintf(_tmpB0B,_tmpB0A,_tag_dyneither(_tmpC,sizeof(void*),0U));});});
++ pos;}
# 85
({void*_tmpE=0U;({struct Cyc___cycFILE*_tmpB0D=Cyc_stderr;struct _dyneither_ptr _tmpB0C=({const char*_tmpF="and ";_tag_dyneither(_tmpF,sizeof(char),5U);});Cyc_fprintf(_tmpB0D,_tmpB0C,_tag_dyneither(_tmpE,sizeof(void*),0U));});});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmpB0F=Cyc_stderr;struct _dyneither_ptr _tmpB0E=({const char*_tmp11="\n\t";_tag_dyneither(_tmp11,sizeof(char),3U);});Cyc_fprintf(_tmpB0F,_tmpB0E,_tag_dyneither(_tmp10,sizeof(void*),0U));});});
pos=8;}
# 91
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0U;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({void*_tmp12[1U]={& _tmp14};({struct Cyc___cycFILE*_tmpB11=Cyc_stderr;struct _dyneither_ptr _tmpB10=({const char*_tmp13="%s ";_tag_dyneither(_tmp13,sizeof(char),4U);});Cyc_fprintf(_tmpB11,_tmpB10,_tag_dyneither(_tmp12,sizeof(void*),1U));});});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp15=0U;({struct Cyc___cycFILE*_tmpB13=Cyc_stderr;struct _dyneither_ptr _tmpB12=({const char*_tmp16="\n\t";_tag_dyneither(_tmp16,sizeof(char),3U);});Cyc_fprintf(_tmpB13,_tmpB12,_tag_dyneither(_tmp15,sizeof(void*),0U));});});
pos=8;}
# 97
({void*_tmp17=0U;({struct Cyc___cycFILE*_tmpB15=Cyc_stderr;struct _dyneither_ptr _tmpB14=({const char*_tmp18="are not compatible. ";_tag_dyneither(_tmp18,sizeof(char),21U);});Cyc_fprintf(_tmpB15,_tmpB14,_tag_dyneither(_tmp17,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpB16=(char*)Cyc_Tcutil_failure_reason.curr;_tmpB16 != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({int _tmpB17=pos;_tmpB17 + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= 80)
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmpB19=Cyc_stderr;struct _dyneither_ptr _tmpB18=({const char*_tmp1A="\n\t";_tag_dyneither(_tmp1A,sizeof(char),3U);});Cyc_fprintf(_tmpB19,_tmpB18,_tag_dyneither(_tmp19,sizeof(void*),0U));});});
# 103
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0U;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp1B[1U]={& _tmp1D};({struct Cyc___cycFILE*_tmpB1B=Cyc_stderr;struct _dyneither_ptr _tmpB1A=({const char*_tmp1C="%s";_tag_dyneither(_tmp1C,sizeof(char),3U);});Cyc_fprintf(_tmpB1B,_tmpB1A,_tag_dyneither(_tmp1B,sizeof(void*),1U));});});});}
# 105
({void*_tmp1E=0U;({struct Cyc___cycFILE*_tmpB1D=Cyc_stderr;struct _dyneither_ptr _tmpB1C=({const char*_tmp1F="\n";_tag_dyneither(_tmp1F,sizeof(char),2U);});Cyc_fprintf(_tmpB1D,_tmpB1C,_tag_dyneither(_tmp1E,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 111
Cyc_Position_post_error(({unsigned int _tmpB1E=loc;Cyc_Position_mk_err(_tmpB1E,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 113
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 116
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0U;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp20[1U]={& _tmp22};({struct Cyc___cycFILE*_tmpB20=Cyc_stderr;struct _dyneither_ptr _tmpB1F=({const char*_tmp21="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp21,sizeof(char),36U);});Cyc_fprintf(_tmpB20,_tmpB1F,_tag_dyneither(_tmp20,sizeof(void*),1U));});});});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));({struct Cyc_Core_Impossible_exn_struct _tmpB21=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;_tmp24.f1=msg;_tmp24;});_tmp23[0]=_tmpB21;});_tmp23;}));}
# 121
void Cyc_Tcutil_warn(unsigned int sg,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 123
Cyc_Warn_vwarn(sg,fmt,ap);}
# 127
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 132
void*Cyc_Tcutil_compress(void*t){
void*_tmp25=t;void*_tmp2E;struct Cyc_Absyn_Exp*_tmp2D;struct Cyc_Absyn_Exp*_tmp2C;void**_tmp2B;void**_tmp2A;switch(*((int*)_tmp25)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp2A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp25)->f2;_LL8: {
# 143
void*ta=(void*)_check_null(*_tmp2A);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp2A=t2;
return t2;}}case 17U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp25)->f4 == 0){_LL3: _LL4:
# 135
 return t;}else{_LL5: _tmp2B=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp25)->f4;_LL6: {
# 137
void*ta=(void*)_check_null(*_tmp2B);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp2B=t2;
return t2;}}case 18U: _LL9: _tmp2C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp25)->f1;_LLA:
# 149
 Cyc_Evexp_eval_const_uint_exp(_tmp2C);{
void*_tmp26=_tmp2C->r;void*_tmp27=_tmp26;void*_tmp28;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp27)->tag == 39U){_LL12: _tmp28=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp27)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp28);}else{_LL14: _LL15:
 return t;}_LL11:;};case 27U: _LLB: _tmp2D=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp25)->f1;_LLC: {
# 155
void*_tmp29=_tmp2D->topt;
if(_tmp29 != 0)return _tmp29;else{
return t;}}case 26U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp25)->f2 != 0){_LLD: _tmp2E=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp25)->f2;_LLE:
# 159
 return Cyc_Tcutil_compress(_tmp2E);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 167
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 171
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp2F=cptr;union Cyc_Absyn_Constraint*_tmp31;void*_tmp30;switch((((union Cyc_Absyn_Constraint*)_tmp2F)->Eq_constr).tag){case 3U: _LL1: _LL2:
 return Cyc_Absyn_empty_conref();case 1U: _LL3: _tmp30=(void*)(_tmp2F->Eq_constr).val;_LL4:
 return Cyc_Absyn_new_conref(_tmp30);default: _LL5: _tmp31=(_tmp2F->Forward_constr).val;_LL6:
 return Cyc_Tcutil_copy_conref(_tmp31);}_LL0:;}
# 178
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp32=Cyc_Absyn_compress_kb(kb);void*_tmp33=_tmp32;struct Cyc_Absyn_Kind*_tmp38;switch(*((int*)_tmp33)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp34=_cycalloc(sizeof(*_tmp34));({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpB22=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp35;_tmp35.tag=1U;_tmp35.f1=0;_tmp35;});_tmp34[0]=_tmpB22;});_tmp34;});case 2U: _LL3: _tmp38=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp33)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp36=_cycalloc(sizeof(*_tmp36));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpB23=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp37;_tmp37.tag=2U;_tmp37.f1=0;_tmp37.f2=_tmp38;_tmp37;});_tmp36[0]=_tmpB23;});_tmp36;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 185
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 187
return({struct Cyc_Absyn_Tvar*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=tv->name;_tmp39->identity=- 1;({void*_tmpB24=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp39->kind=_tmpB24;});_tmp39;});}
# 189
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 191
struct _tuple10*_tmp3A=arg;struct _dyneither_ptr*_tmp3E;struct Cyc_Absyn_Tqual _tmp3D;void*_tmp3C;_LL1: _tmp3E=_tmp3A->f1;_tmp3D=_tmp3A->f2;_tmp3C=_tmp3A->f3;_LL2:;
return({struct _tuple10*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=_tmp3E;_tmp3B->f2=_tmp3D;({void*_tmpB25=Cyc_Tcutil_copy_type(_tmp3C);_tmp3B->f3=_tmpB25;});_tmp3B;});}
# 194
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp3F=arg;struct Cyc_Absyn_Tqual _tmp42;void*_tmp41;_LL1: _tmp42=_tmp3F->f1;_tmp41=_tmp3F->f2;_LL2:;
return({struct _tuple12*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->f1=_tmp42;({void*_tmpB26=Cyc_Tcutil_copy_type(_tmp41);_tmp40->f2=_tmpB26;});_tmp40;});}
# 198
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 200
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->name=f->name;_tmp43->tq=f->tq;({void*_tmpB27=Cyc_Tcutil_copy_type(f->type);_tmp43->type=_tmpB27;});_tmp43->width=f->width;_tmp43->attributes=f->attributes;({struct Cyc_Absyn_Exp*_tmpB28=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp43->requires_clause=_tmpB28;});_tmp43;});}
# 204
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp44=x;void*_tmp47;void*_tmp46;_LL1: _tmp47=_tmp44->f1;_tmp46=_tmp44->f2;_LL2:;
return({struct _tuple0*_tmp45=_cycalloc(sizeof(*_tmp45));({void*_tmpB29=Cyc_Tcutil_copy_type(_tmp47);_tmp45->f1=_tmpB29;});({void*_tmpB2A=Cyc_Tcutil_copy_type(_tmp46);_tmp45->f2=_tmpB2A;});_tmp45;});}
# 208
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->name=f->name;_tmp48->tag=f->tag;_tmp48->loc=f->loc;_tmp48;});}
# 211
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpB2C=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A;_tmp4A.tag=2U;({struct Cyc_Absyn_Tvar*_tmpB2B=Cyc_Tcutil_copy_tvar(t);_tmp4A.f1=_tmpB2B;});_tmp4A;});_tmp49[0]=_tmpB2C;});_tmp49;});}
# 215
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_Absyn_Datatypedecl*_tmpD2;struct Cyc_Absyn_Enumdecl*_tmpD1;struct Cyc_Absyn_Aggrdecl*_tmpD0;void*_tmpCF;struct Cyc_List_List*_tmpCE;void*_tmpCD;struct _tuple2*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_Absyn_Typedefdecl*_tmpCA;void*_tmpC9;void*_tmpC8;void*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;void*_tmpC4;struct Cyc_List_List*_tmpC3;struct _tuple2*_tmpC2;struct Cyc_Absyn_Enumdecl*_tmpC1;enum Cyc_Absyn_AggrKind _tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Aggrdecl**_tmpBE;struct Cyc_List_List*_tmpBD;enum Cyc_Absyn_AggrKind _tmpBC;struct _tuple2*_tmpBB;struct Cyc_Core_Opt*_tmpBA;struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;void*_tmpB6;struct Cyc_Absyn_Tqual _tmpB5;void*_tmpB4;struct Cyc_List_List*_tmpB3;int _tmpB2;struct Cyc_Absyn_VarargInfo*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_List_List*_tmpAB;void*_tmpAA;struct Cyc_Absyn_Tqual _tmpA9;struct Cyc_Absyn_Exp*_tmpA8;union Cyc_Absyn_Constraint*_tmpA7;unsigned int _tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;union Cyc_Absyn_Constraint*_tmpA2;union Cyc_Absyn_Constraint*_tmpA1;union Cyc_Absyn_Constraint*_tmpA0;struct Cyc_Absyn_PtrLoc*_tmp9F;union Cyc_Absyn_DatatypeFieldInfoU _tmp9E;struct Cyc_List_List*_tmp9D;union Cyc_Absyn_DatatypeInfoU _tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Tvar*_tmp9A;switch(*((int*)_tmp4C)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return t;case 2U: _LL5: _tmp9A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4C)->f1;_LL6:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpB2E=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4E;_tmp4E.tag=2U;({struct Cyc_Absyn_Tvar*_tmpB2D=Cyc_Tcutil_copy_tvar(_tmp9A);_tmp4E.f1=_tmpB2D;});_tmp4E;});_tmp4D[0]=_tmpB2E;});_tmp4D;});case 3U: _LL7: _tmp9C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).datatype_info;_tmp9B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL8:
# 221
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB31=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp50;_tmp50.tag=3U;({struct Cyc_Absyn_DatatypeInfo _tmpB30=({struct Cyc_Absyn_DatatypeInfo _tmp51;_tmp51.datatype_info=_tmp9C;({struct Cyc_List_List*_tmpB2F=Cyc_Tcutil_copy_types(_tmp9B);_tmp51.targs=_tmpB2F;});_tmp51;});_tmp50.f1=_tmpB30;});_tmp50;});_tmp4F[0]=_tmpB31;});_tmp4F;});case 4U: _LL9: _tmp9E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).field_info;_tmp9D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LLA:
# 223
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB34=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp53;_tmp53.tag=4U;({struct Cyc_Absyn_DatatypeFieldInfo _tmpB33=({struct Cyc_Absyn_DatatypeFieldInfo _tmp54;_tmp54.field_info=_tmp9E;({struct Cyc_List_List*_tmpB32=Cyc_Tcutil_copy_types(_tmp9D);_tmp54.targs=_tmpB32;});_tmp54;});_tmp53.f1=_tmpB33;});_tmp53;});_tmp52[0]=_tmpB34;});_tmp52;});case 5U: _LLB: _tmpA5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_typ;_tmpA4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_tq;_tmpA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).rgn;_tmpA2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_tmpA1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).bounds;_tmpA0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).zero_term;_tmp9F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).ptrloc;_LLC: {
# 225
void*_tmp55=Cyc_Tcutil_copy_type(_tmpA5);
void*_tmp56=Cyc_Tcutil_copy_type(_tmpA3);
union Cyc_Absyn_Constraint*_tmp57=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA2);
struct Cyc_Absyn_Tqual _tmp58=_tmpA4;
# 230
union Cyc_Absyn_Constraint*_tmp59=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA1);
union Cyc_Absyn_Constraint*_tmp5A=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA0);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpB37=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp5C;_tmp5C.tag=5U;({struct Cyc_Absyn_PtrInfo _tmpB36=({struct Cyc_Absyn_PtrInfo _tmp5D;_tmp5D.elt_typ=_tmp55;_tmp5D.elt_tq=_tmp58;({struct Cyc_Absyn_PtrAtts _tmpB35=({(_tmp5D.ptr_atts).rgn=_tmp56;(_tmp5D.ptr_atts).nullable=_tmp57;(_tmp5D.ptr_atts).bounds=_tmp59;(_tmp5D.ptr_atts).zero_term=_tmp5A;(_tmp5D.ptr_atts).ptrloc=_tmp9F;_tmp5D.ptr_atts;});_tmp5D.ptr_atts=_tmpB35;});_tmp5D;});_tmp5C.f1=_tmpB36;});_tmp5C;});_tmp5B[0]=_tmpB37;});_tmp5B;});}case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 return t;case 8U: _LL11: _tmpAA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_tmpA9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).tq;_tmpA8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).num_elts;_tmpA7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zero_term;_tmpA6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zt_loc;_LL12: {
# 236
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpA8);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB3B=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp5F;_tmp5F.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpB3A=({struct Cyc_Absyn_ArrayInfo _tmp60;({void*_tmpB38=Cyc_Tcutil_copy_type(_tmpAA);_tmp60.elt_type=_tmpB38;});_tmp60.tq=_tmpA9;_tmp60.num_elts=eopt2;({union Cyc_Absyn_Constraint*_tmpB39=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA7);_tmp60.zero_term=_tmpB39;});_tmp60.zt_loc=_tmpA6;_tmp60;});_tmp5F.f1=_tmpB3A;});_tmp5F;});_tmp5E[0]=_tmpB3B;});_tmp5E;});}case 9U: _LL13: _tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).tvars;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).effect;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_tqual;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_typ;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).args;_tmpB2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).c_varargs;_tmpB1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).cyc_varargs;_tmpB0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).rgn_po;_tmpAF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_tmpAE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_clause;_tmpAD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_relns;_tmpAC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_clause;_tmpAB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_relns;_LL14: {
# 240
struct Cyc_List_List*_tmp61=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpB7);
void*effopt2=_tmpB6 == 0?0: Cyc_Tcutil_copy_type(_tmpB6);
void*_tmp62=Cyc_Tcutil_copy_type(_tmpB4);
struct Cyc_List_List*_tmp63=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpB3);
int _tmp64=_tmpB2;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpB1 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpB1;
({struct Cyc_Absyn_VarargInfo*_tmpB3D=({struct Cyc_Absyn_VarargInfo*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->name=cv->name;_tmp65->tq=cv->tq;({void*_tmpB3C=Cyc_Tcutil_copy_type(cv->type);_tmp65->type=_tmpB3C;});_tmp65->inject=cv->inject;_tmp65;});cyc_varargs2=_tmpB3D;});}{
# 251
struct Cyc_List_List*_tmp66=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpB0);
struct Cyc_List_List*_tmp67=_tmpAF;
struct Cyc_Absyn_Exp*_tmp68=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAE);
struct Cyc_List_List*_tmp69=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAD);
struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAC);
struct Cyc_List_List*_tmp6B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAB);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB3F=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6D;_tmp6D.tag=9U;({struct Cyc_Absyn_FnInfo _tmpB3E=({struct Cyc_Absyn_FnInfo _tmp6E;_tmp6E.tvars=_tmp61;_tmp6E.effect=effopt2;_tmp6E.ret_tqual=_tmpB5;_tmp6E.ret_typ=_tmp62;_tmp6E.args=_tmp63;_tmp6E.c_varargs=_tmp64;_tmp6E.cyc_varargs=cyc_varargs2;_tmp6E.rgn_po=_tmp66;_tmp6E.attributes=_tmp67;_tmp6E.requires_clause=_tmp68;_tmp6E.requires_relns=_tmp69;_tmp6E.ensures_clause=_tmp6A;_tmp6E.ensures_relns=_tmp6B;_tmp6E;});_tmp6D.f1=_tmpB3E;});_tmp6D;});_tmp6C[0]=_tmpB3F;});_tmp6C;});};}case 10U: _LL15: _tmpB8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4C)->f1;_LL16:
# 260
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB41=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp70;_tmp70.tag=10U;({struct Cyc_List_List*_tmpB40=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpB8);_tmp70.f1=_tmpB40;});_tmp70;});_tmp6F[0]=_tmpB41;});_tmp6F;});case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).tag == 1){_LL17: _tmpBC=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f1;_tmpBB=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f2;_tmpBA=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f3;_tmpB9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL18:
# 262
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB45=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp72;_tmp72.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpB44=({struct Cyc_Absyn_AggrInfo _tmp73;({union Cyc_Absyn_AggrInfoU _tmpB42=Cyc_Absyn_UnknownAggr(_tmpBC,_tmpBB,_tmpBA);_tmp73.aggr_info=_tmpB42;});({struct Cyc_List_List*_tmpB43=Cyc_Tcutil_copy_types(_tmpB9);_tmp73.targs=_tmpB43;});_tmp73;});_tmp72.f1=_tmpB44;});_tmp72;});_tmp71[0]=_tmpB45;});_tmp71;});}else{_LL19: _tmpBE=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).KnownAggr).val;_tmpBD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL1A:
# 264
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB49=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp75;_tmp75.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpB48=({struct Cyc_Absyn_AggrInfo _tmp76;({union Cyc_Absyn_AggrInfoU _tmpB46=Cyc_Absyn_KnownAggr(_tmpBE);_tmp76.aggr_info=_tmpB46;});({struct Cyc_List_List*_tmpB47=Cyc_Tcutil_copy_types(_tmpBD);_tmp76.targs=_tmpB47;});_tmp76;});_tmp75.f1=_tmpB48;});_tmp75;});_tmp74[0]=_tmpB49;});_tmp74;});}case 12U: _LL1B: _tmpC0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f1;_tmpBF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f2;_LL1C:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB4B=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp78;_tmp78.tag=12U;_tmp78.f1=_tmpC0;({struct Cyc_List_List*_tmpB4A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpBF);_tmp78.f2=_tmpB4A;});_tmp78;});_tmp77[0]=_tmpB4B;});_tmp77;});case 13U: _LL1D: _tmpC2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f2;_LL1E:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp79=_cycalloc(sizeof(*_tmp79));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB4C=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp7A;_tmp7A.tag=13U;_tmp7A.f1=_tmpC2;_tmp7A.f2=_tmpC1;_tmp7A;});_tmp79[0]=_tmpB4C;});_tmp79;});case 14U: _LL1F: _tmpC3=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4C)->f1;_LL20:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB4E=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp7C;_tmp7C.tag=14U;({struct Cyc_List_List*_tmpB4D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpC3);_tmp7C.f1=_tmpB4D;});_tmp7C;});_tmp7B[0]=_tmpB4E;});_tmp7B;});case 19U: _LL21: _tmpC4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->f1;_LL22:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB50=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7E;_tmp7E.tag=19U;({void*_tmpB4F=Cyc_Tcutil_copy_type(_tmpC4);_tmp7E.f1=_tmpB4F;});_tmp7E;});_tmp7D[0]=_tmpB50;});_tmp7D;});case 18U: _LL23: _tmpC5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4C)->f1;_LL24:
# 271
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB51=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp80;_tmp80.tag=18U;_tmp80.f1=_tmpC5;_tmp80;});_tmp7F[0]=_tmpB51;});_tmp7F;});case 27U: _LL25: _tmpC6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp4C)->f1;_LL26:
# 274
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpB52=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp82;_tmp82.tag=27U;_tmp82.f1=_tmpC6;_tmp82;});_tmp81[0]=_tmpB52;});_tmp81;});case 15U: _LL27: _tmpC7=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4C)->f1;_LL28:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB54=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp84;_tmp84.tag=15U;({void*_tmpB53=Cyc_Tcutil_copy_type(_tmpC7);_tmp84.f1=_tmpB53;});_tmp84;});_tmp83[0]=_tmpB54;});_tmp83;});case 16U: _LL29: _tmpC9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f2;_LL2A:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB57=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp86;_tmp86.tag=16U;({void*_tmpB55=Cyc_Tcutil_copy_type(_tmpC9);_tmp86.f1=_tmpB55;});({void*_tmpB56=Cyc_Tcutil_copy_type(_tmpC8);_tmp86.f2=_tmpB56;});_tmp86;});_tmp85[0]=_tmpB57;});_tmp85;});case 17U: _LL2B: _tmpCC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f1;_tmpCB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f2;_tmpCA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f3;_LL2C:
# 278
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB59=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp88;_tmp88.tag=17U;_tmp88.f1=_tmpCC;({struct Cyc_List_List*_tmpB58=Cyc_Tcutil_copy_types(_tmpCB);_tmp88.f2=_tmpB58;});_tmp88.f3=_tmpCA;_tmp88.f4=0;_tmp88;});_tmp87[0]=_tmpB59;});_tmp87;});case 21U: _LL2D: _LL2E:
 goto _LL30;case 22U: _LL2F: _LL30:
 goto _LL32;case 28U: _LL31: _LL32:
 goto _LL34;case 20U: _LL33: _LL34:
 return t;case 23U: _LL35: _tmpCD=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4C)->f1;_LL36:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpB5B=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8A;_tmp8A.tag=23U;({void*_tmpB5A=Cyc_Tcutil_copy_type(_tmpCD);_tmp8A.f1=_tmpB5A;});_tmp8A;});_tmp89[0]=_tmpB5B;});_tmp89;});case 24U: _LL37: _tmpCE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4C)->f1;_LL38:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpB5D=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8C;_tmp8C.tag=24U;({struct Cyc_List_List*_tmpB5C=Cyc_Tcutil_copy_types(_tmpCE);_tmp8C.f1=_tmpB5C;});_tmp8C;});_tmp8B[0]=_tmpB5D;});_tmp8B;});case 25U: _LL39: _tmpCF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4C)->f1;_LL3A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpB5F=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8E;_tmp8E.tag=25U;({void*_tmpB5E=Cyc_Tcutil_copy_type(_tmpCF);_tmp8E.f1=_tmpB5E;});_tmp8E;});_tmp8D[0]=_tmpB5F;});_tmp8D;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)){case 0U: _LL3B: _tmpD0=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL3C: {
# 288
struct Cyc_List_List*_tmp8F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD0->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp90=_cycalloc(sizeof(*_tmp90));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB62=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp91;_tmp91.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpB61=({struct Cyc_Absyn_AggrInfo _tmp92;({union Cyc_Absyn_AggrInfoU _tmpB60=Cyc_Absyn_UnknownAggr(_tmpD0->kind,_tmpD0->name,0);_tmp92.aggr_info=_tmpB60;});_tmp92.targs=_tmp8F;_tmp92;});_tmp91.f1=_tmpB61;});_tmp91;});_tmp90[0]=_tmpB62;});_tmp90;});}case 1U: _LL3D: _tmpD1=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL3E:
# 291
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp93=_cycalloc(sizeof(*_tmp93));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB63=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp94;_tmp94.tag=13U;_tmp94.f1=_tmpD1->name;_tmp94.f2=0;_tmp94;});_tmp93[0]=_tmpB63;});_tmp93;});default: _LL3F: _tmpD2=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL40: {
# 293
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD2->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB66=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp97;_tmp97.tag=3U;({struct Cyc_Absyn_DatatypeInfo _tmpB65=({struct Cyc_Absyn_DatatypeInfo _tmp98;({union Cyc_Absyn_DatatypeInfoU _tmpB64=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp99;_tmp99.name=_tmpD2->name;_tmp99.is_extensible=0;_tmp99;}));_tmp98.datatype_info=_tmpB64;});_tmp98.targs=_tmp95;_tmp98;});_tmp97.f1=_tmpB65;});_tmp97;});_tmp96[0]=_tmpB66;});_tmp96;});}}}_LL0:;}
# 308 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD3=d;struct _dyneither_ptr*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->tag == 0U){_LL1: _tmpD6=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpB68=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD5;_tmpD5.tag=0U;({struct Cyc_Absyn_Exp*_tmpB67=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpD6);_tmpD5.f1=_tmpB67;});_tmpD5;});_tmpD4[0]=_tmpB68;});_tmpD4;});}else{_LL3: _tmpD7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD3)->f1;_LL4:
 return d;}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 314
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 316
return({struct _tuple19*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_List_List*_tmpB69=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpD8->f1=_tmpB69;});({struct Cyc_Absyn_Exp*_tmpB6A=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpD8->f2=_tmpB6A;});_tmpD8;});}
# 319
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpD9=preserve_types;
{void*_tmpDA=e->r;void*_tmpDB=_tmpDA;int _tmp168;struct _dyneither_ptr _tmp167;void*_tmp166;struct Cyc_Absyn_Exp*_tmp165;struct _dyneither_ptr*_tmp164;struct Cyc_Core_Opt*_tmp163;struct Cyc_List_List*_tmp162;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp160;int _tmp15F;struct Cyc_Absyn_Exp*_tmp15E;void**_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;int _tmp15B;int _tmp15A;void*_tmp159;struct Cyc_Absyn_Enumfield*_tmp158;struct Cyc_Absyn_Enumdecl*_tmp157;struct Cyc_Absyn_Enumfield*_tmp156;struct Cyc_List_List*_tmp155;struct Cyc_Absyn_Datatypedecl*_tmp154;struct Cyc_Absyn_Datatypefield*_tmp153;void*_tmp152;struct Cyc_List_List*_tmp151;struct _tuple2*_tmp150;struct Cyc_List_List*_tmp14F;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_Aggrdecl*_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;void*_tmp14B;int _tmp14A;struct Cyc_Absyn_Vardecl*_tmp149;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp147;int _tmp146;struct Cyc_List_List*_tmp145;struct _dyneither_ptr*_tmp144;struct Cyc_Absyn_Tqual _tmp143;void*_tmp142;struct Cyc_List_List*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct _dyneither_ptr*_tmp13C;int _tmp13B;int _tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct _dyneither_ptr*_tmp138;int _tmp137;int _tmp136;struct Cyc_Absyn_Exp*_tmp135;void*_tmp134;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_Exp*_tmp132;void*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;void*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;int _tmp12B;enum Cyc_Absyn_Coercion _tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp126;int _tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_VarargCallInfo*_tmp122;int _tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;enum Cyc_Absyn_Incrementor _tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Core_Opt*_tmp114;struct Cyc_Absyn_Exp*_tmp113;enum Cyc_Absyn_Primop _tmp112;struct Cyc_List_List*_tmp111;struct _dyneither_ptr _tmp110;void*_tmp10F;union Cyc_Absyn_Cnst _tmp10E;switch(*((int*)_tmpDB)){case 0U: _LL1: _tmp10E=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL2:
({struct Cyc_Absyn_Exp*_tmpB6B=Cyc_Absyn_const_exp(_tmp10E,e->loc);new_e=_tmpB6B;});goto _LL0;case 1U: _LL3: _tmp10F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpB6C=Cyc_Absyn_varb_exp(_tmp10F,e->loc);new_e=_tmpB6C;});goto _LL0;case 2U: _LL5: _tmp110=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpB6D=Cyc_Absyn_pragma_exp(_tmp110,e->loc);new_e=_tmpB6D;});goto _LL0;case 3U: _LL7: _tmp112=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp111=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL8:
({struct Cyc_Absyn_Exp*_tmpB70=({enum Cyc_Absyn_Primop _tmpB6F=_tmp112;struct Cyc_List_List*_tmpB6E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp111);Cyc_Absyn_primop_exp(_tmpB6F,_tmpB6E,e->loc);});new_e=_tmpB70;});goto _LL0;case 4U: _LL9: _tmp115=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp114=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp113=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLA:
# 328
({struct Cyc_Absyn_Exp*_tmpB74=({struct Cyc_Absyn_Exp*_tmpB73=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp115);struct Cyc_Core_Opt*_tmpB72=(unsigned int)_tmp114?({struct Cyc_Core_Opt*_tmpDC=_cycalloc_atomic(sizeof(*_tmpDC));_tmpDC->v=(void*)_tmp114->v;_tmpDC;}): 0;struct Cyc_Absyn_Exp*_tmpB71=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp113);Cyc_Absyn_assignop_exp(_tmpB73,_tmpB72,_tmpB71,e->loc);});new_e=_tmpB74;});
goto _LL0;case 5U: _LLB: _tmp117=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp116=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLC:
({struct Cyc_Absyn_Exp*_tmpB77=({struct Cyc_Absyn_Exp*_tmpB76=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp117);enum Cyc_Absyn_Incrementor _tmpB75=_tmp116;Cyc_Absyn_increment_exp(_tmpB76,_tmpB75,e->loc);});new_e=_tmpB77;});goto _LL0;case 6U: _LLD: _tmp11A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp118=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLE:
# 332
({struct Cyc_Absyn_Exp*_tmpB7B=({struct Cyc_Absyn_Exp*_tmpB7A=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11A);struct Cyc_Absyn_Exp*_tmpB79=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp119);struct Cyc_Absyn_Exp*_tmpB78=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp118);Cyc_Absyn_conditional_exp(_tmpB7A,_tmpB79,_tmpB78,e->loc);});new_e=_tmpB7B;});goto _LL0;case 7U: _LLF: _tmp11C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL10:
({struct Cyc_Absyn_Exp*_tmpB7E=({struct Cyc_Absyn_Exp*_tmpB7D=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11C);struct Cyc_Absyn_Exp*_tmpB7C=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11B);Cyc_Absyn_and_exp(_tmpB7D,_tmpB7C,e->loc);});new_e=_tmpB7E;});goto _LL0;case 8U: _LL11: _tmp11E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL12:
({struct Cyc_Absyn_Exp*_tmpB81=({struct Cyc_Absyn_Exp*_tmpB80=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11E);struct Cyc_Absyn_Exp*_tmpB7F=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11D);Cyc_Absyn_or_exp(_tmpB80,_tmpB7F,e->loc);});new_e=_tmpB81;});goto _LL0;case 9U: _LL13: _tmp120=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL14:
({struct Cyc_Absyn_Exp*_tmpB84=({struct Cyc_Absyn_Exp*_tmpB83=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp120);struct Cyc_Absyn_Exp*_tmpB82=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11F);Cyc_Absyn_seq_exp(_tmpB83,_tmpB82,e->loc);});new_e=_tmpB84;});goto _LL0;case 10U: _LL15: _tmp124=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp123=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp122=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp121=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL16:
# 337
{struct Cyc_Absyn_VarargCallInfo*_tmpDD=_tmp122;int _tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_VarargInfo*_tmpE9;if(_tmpDD != 0){_LL54: _tmpEB=_tmpDD->num_varargs;_tmpEA=_tmpDD->injectors;_tmpE9=_tmpDD->vai;_LL55: {
# 339
struct Cyc_Absyn_VarargInfo*_tmpDE=_tmpE9;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Tqual _tmpE5;void*_tmpE4;int _tmpE3;_LL59: _tmpE6=_tmpDE->name;_tmpE5=_tmpDE->tq;_tmpE4=_tmpDE->type;_tmpE3=_tmpDE->inject;_LL5A:;
({struct Cyc_Absyn_Exp*_tmpB8C=({void*_tmpB8B=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpB8A=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE0;_tmpE0.tag=10U;({struct Cyc_Absyn_Exp*_tmpB85=
Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp124);_tmpE0.f1=_tmpB85;});({struct Cyc_List_List*_tmpB86=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp123);_tmpE0.f2=_tmpB86;});({struct Cyc_Absyn_VarargCallInfo*_tmpB89=({struct Cyc_Absyn_VarargCallInfo*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->num_varargs=_tmpEB;_tmpE1->injectors=_tmpEA;({struct Cyc_Absyn_VarargInfo*_tmpB88=({struct Cyc_Absyn_VarargInfo*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->name=_tmpE6;_tmpE2->tq=_tmpE5;({void*_tmpB87=
# 343
Cyc_Tcutil_copy_type(_tmpE4);_tmpE2->type=_tmpB87;});_tmpE2->inject=_tmpE3;_tmpE2;});_tmpE1->vai=_tmpB88;});_tmpE1;});_tmpE0.f3=_tmpB89;});_tmpE0.f4=_tmp121;_tmpE0;});_tmpDF[0]=_tmpB8A;});_tmpDF;});
# 340
Cyc_Absyn_new_exp(_tmpB8B,e->loc);});new_e=_tmpB8C;});
# 345
goto _LL53;}}else{_LL56: _LL57:
# 347
({struct Cyc_Absyn_Exp*_tmpB91=({void*_tmpB90=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpB8F=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE8;_tmpE8.tag=10U;({struct Cyc_Absyn_Exp*_tmpB8D=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp124);_tmpE8.f1=_tmpB8D;});({struct Cyc_List_List*_tmpB8E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp123);_tmpE8.f2=_tmpB8E;});_tmpE8.f3=_tmp122;_tmpE8.f4=_tmp121;_tmpE8;});_tmpE7[0]=_tmpB8F;});_tmpE7;});Cyc_Absyn_new_exp(_tmpB90,e->loc);});new_e=_tmpB91;});
goto _LL53;}_LL53:;}
# 350
goto _LL0;case 11U: _LL17: _tmp126=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp125=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL18:
# 352
({struct Cyc_Absyn_Exp*_tmpB94=_tmp125?({struct Cyc_Absyn_Exp*_tmpB93=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp126);Cyc_Absyn_rethrow_exp(_tmpB93,e->loc);}):({struct Cyc_Absyn_Exp*_tmpB92=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp126);Cyc_Absyn_throw_exp(_tmpB92,e->loc);});new_e=_tmpB94;});
goto _LL0;case 12U: _LL19: _tmp127=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL1A:
({struct Cyc_Absyn_Exp*_tmpB96=({struct Cyc_Absyn_Exp*_tmpB95=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp127);Cyc_Absyn_noinstantiate_exp(_tmpB95,e->loc);});new_e=_tmpB96;});
goto _LL0;case 13U: _LL1B: _tmp129=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp128=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL1C:
# 357
({struct Cyc_Absyn_Exp*_tmpB99=({struct Cyc_Absyn_Exp*_tmpB98=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp129);struct Cyc_List_List*_tmpB97=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp128);Cyc_Absyn_instantiate_exp(_tmpB98,_tmpB97,e->loc);});new_e=_tmpB99;});
goto _LL0;case 14U: _LL1D: _tmp12D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp12B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp12A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL1E:
# 360
({struct Cyc_Absyn_Exp*_tmpB9E=({void*_tmpB9D=Cyc_Tcutil_copy_type(_tmp12D);struct Cyc_Absyn_Exp*_tmpB9C=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12C);int _tmpB9B=_tmp12B;enum Cyc_Absyn_Coercion _tmpB9A=_tmp12A;Cyc_Absyn_cast_exp(_tmpB9D,_tmpB9C,_tmpB9B,_tmpB9A,e->loc);});new_e=_tmpB9E;});goto _LL0;case 15U: _LL1F: _tmp12E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL20:
({struct Cyc_Absyn_Exp*_tmpBA0=({struct Cyc_Absyn_Exp*_tmpB9F=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12E);Cyc_Absyn_address_exp(_tmpB9F,e->loc);});new_e=_tmpBA0;});goto _LL0;case 16U: _LL21: _tmp130=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL22: {
# 363
struct Cyc_Absyn_Exp*eo1=_tmp130;if(_tmp130 != 0)({struct Cyc_Absyn_Exp*_tmpBA1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp130);eo1=_tmpBA1;});
({struct Cyc_Absyn_Exp*_tmpBA4=({struct Cyc_Absyn_Exp*_tmpBA3=eo1;struct Cyc_Absyn_Exp*_tmpBA2=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12F);Cyc_Absyn_New_exp(_tmpBA3,_tmpBA2,e->loc);});new_e=_tmpBA4;});
goto _LL0;}case 17U: _LL23: _tmp131=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL24:
({struct Cyc_Absyn_Exp*_tmpBA6=({void*_tmpBA5=Cyc_Tcutil_copy_type(_tmp131);Cyc_Absyn_sizeoftyp_exp(_tmpBA5,e->loc);});new_e=_tmpBA6;});
goto _LL0;case 18U: _LL25: _tmp132=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL26:
({struct Cyc_Absyn_Exp*_tmpBA8=({struct Cyc_Absyn_Exp*_tmpBA7=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp132);Cyc_Absyn_sizeofexp_exp(_tmpBA7,e->loc);});new_e=_tmpBA8;});goto _LL0;case 19U: _LL27: _tmp134=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp133=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL28:
# 370
({struct Cyc_Absyn_Exp*_tmpBAB=({void*_tmpBAA=Cyc_Tcutil_copy_type(_tmp134);struct Cyc_List_List*_tmpBA9=_tmp133;Cyc_Absyn_offsetof_exp(_tmpBAA,_tmpBA9,e->loc);});new_e=_tmpBAB;});goto _LL0;case 20U: _LL29: _tmp135=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL2A:
({struct Cyc_Absyn_Exp*_tmpBAD=({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp135);Cyc_Absyn_deref_exp(_tmpBAC,e->loc);});new_e=_tmpBAD;});goto _LL0;case 21U: _LL2B: _tmp139=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp138=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp137=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp136=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL2C:
# 373
({struct Cyc_Absyn_Exp*_tmpBB1=({void*_tmpBB0=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpBAF=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpED;_tmpED.tag=21U;({struct Cyc_Absyn_Exp*_tmpBAE=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp139);_tmpED.f1=_tmpBAE;});_tmpED.f2=_tmp138;_tmpED.f3=_tmp137;_tmpED.f4=_tmp136;_tmpED;});_tmpEC[0]=_tmpBAF;});_tmpEC;});Cyc_Absyn_new_exp(_tmpBB0,e->loc);});new_e=_tmpBB1;});goto _LL0;case 22U: _LL2D: _tmp13D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp13B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp13A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL2E:
# 375
({struct Cyc_Absyn_Exp*_tmpBB5=({void*_tmpBB4=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpBB3=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpEF;_tmpEF.tag=22U;({struct Cyc_Absyn_Exp*_tmpBB2=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13D);_tmpEF.f1=_tmpBB2;});_tmpEF.f2=_tmp13C;_tmpEF.f3=_tmp13B;_tmpEF.f4=_tmp13A;_tmpEF;});_tmpEE[0]=_tmpBB3;});_tmpEE;});Cyc_Absyn_new_exp(_tmpBB4,e->loc);});new_e=_tmpBB5;});goto _LL0;case 23U: _LL2F: _tmp13F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL30:
({struct Cyc_Absyn_Exp*_tmpBB8=({struct Cyc_Absyn_Exp*_tmpBB7=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13F);struct Cyc_Absyn_Exp*_tmpBB6=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13E);Cyc_Absyn_subscript_exp(_tmpBB7,_tmpBB6,e->loc);});new_e=_tmpBB8;});
goto _LL0;case 24U: _LL31: _tmp140=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL32:
({struct Cyc_Absyn_Exp*_tmpBBA=({struct Cyc_List_List*_tmpBB9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp140);Cyc_Absyn_tuple_exp(_tmpBB9,e->loc);});new_e=_tmpBBA;});goto _LL0;case 25U: _LL33: _tmp144=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f1;_tmp143=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f2;_tmp142=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f3;_tmp141=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL34: {
# 380
struct _dyneither_ptr*vopt1=_tmp144;
if(_tmp144 != 0)vopt1=_tmp144;
({struct Cyc_Absyn_Exp*_tmpBC0=({void*_tmpBBF=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpBBE=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF1;_tmpF1.tag=25U;({struct _tuple10*_tmpBBC=({struct _tuple10*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=vopt1;_tmpF2->f2=_tmp143;({void*_tmpBBB=Cyc_Tcutil_copy_type(_tmp142);_tmpF2->f3=_tmpBBB;});_tmpF2;});_tmpF1.f1=_tmpBBC;});({struct Cyc_List_List*_tmpBBD=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp141);_tmpF1.f2=_tmpBBD;});_tmpF1;});_tmpF0[0]=_tmpBBE;});_tmpF0;});
# 382
Cyc_Absyn_new_exp(_tmpBBF,e->loc);});new_e=_tmpBC0;});
# 384
goto _LL0;}case 26U: _LL35: _tmp145=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL36:
({struct Cyc_Absyn_Exp*_tmpBC4=({void*_tmpBC3=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpBC2=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF4;_tmpF4.tag=26U;({struct Cyc_List_List*_tmpBC1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp145);_tmpF4.f1=_tmpBC1;});_tmpF4;});_tmpF3[0]=_tmpBC2;});_tmpF3;});Cyc_Absyn_new_exp(_tmpBC3,e->loc);});new_e=_tmpBC4;});
goto _LL0;case 27U: _LL37: _tmp149=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp148=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp147=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp146=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL38:
# 388
({struct Cyc_Absyn_Exp*_tmpBC9=({void*_tmpBC8=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpBC7=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF6;_tmpF6.tag=27U;_tmpF6.f1=_tmp149;({struct Cyc_Absyn_Exp*_tmpBC5=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp148);_tmpF6.f2=_tmpBC5;});({struct Cyc_Absyn_Exp*_tmpBC6=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp147);_tmpF6.f3=_tmpBC6;});_tmpF6.f4=_tmp146;_tmpF6;});_tmpF5[0]=_tmpBC7;});_tmpF5;});Cyc_Absyn_new_exp(_tmpBC8,e->loc);});new_e=_tmpBC9;});
goto _LL0;case 28U: _LL39: _tmp14C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp14A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL3A:
# 391
({struct Cyc_Absyn_Exp*_tmpBCE=({void*_tmpBCD=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpBCC=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF8;_tmpF8.tag=28U;({struct Cyc_Absyn_Exp*_tmpBCA=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp14C);_tmpF8.f1=_tmpBCA;});({void*_tmpBCB=Cyc_Tcutil_copy_type(_tmp14B);_tmpF8.f2=_tmpBCB;});_tmpF8.f3=_tmp14A;_tmpF8;});_tmpF7[0]=_tmpBCC;});_tmpF7;});Cyc_Absyn_new_exp(_tmpBCD,_tmp14C->loc);});new_e=_tmpBCE;});
# 393
goto _LL0;case 29U: _LL3B: _tmp150=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp14E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp14D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL3C:
# 395
({struct Cyc_Absyn_Exp*_tmpBD3=({void*_tmpBD2=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpBD1=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpFA;_tmpFA.tag=29U;_tmpFA.f1=_tmp150;({struct Cyc_List_List*_tmpBCF=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp14F);_tmpFA.f2=_tmpBCF;});({struct Cyc_List_List*_tmpBD0=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp14E);_tmpFA.f3=_tmpBD0;});_tmpFA.f4=_tmp14D;_tmpFA;});_tmpF9[0]=_tmpBD1;});_tmpF9;});Cyc_Absyn_new_exp(_tmpBD2,e->loc);});new_e=_tmpBD3;});
# 397
goto _LL0;case 30U: _LL3D: _tmp152=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp151=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL3E:
# 399
({struct Cyc_Absyn_Exp*_tmpBD8=({void*_tmpBD7=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpBD6=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpFC;_tmpFC.tag=30U;({void*_tmpBD4=Cyc_Tcutil_copy_type(_tmp152);_tmpFC.f1=_tmpBD4;});({struct Cyc_List_List*_tmpBD5=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp151);_tmpFC.f2=_tmpBD5;});_tmpFC;});_tmpFB[0]=_tmpBD6;});_tmpFB;});Cyc_Absyn_new_exp(_tmpBD7,e->loc);});new_e=_tmpBD8;});
goto _LL0;case 31U: _LL3F: _tmp155=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp154=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp153=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL40:
# 402
({struct Cyc_Absyn_Exp*_tmpBDC=({void*_tmpBDB=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpBDA=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpFE;_tmpFE.tag=31U;({struct Cyc_List_List*_tmpBD9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp155);_tmpFE.f1=_tmpBD9;});_tmpFE.f2=_tmp154;_tmpFE.f3=_tmp153;_tmpFE;});_tmpFD[0]=_tmpBDA;});_tmpFD;});Cyc_Absyn_new_exp(_tmpBDB,e->loc);});new_e=_tmpBDC;});
goto _LL0;case 32U: _LL41: _tmp157=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp156=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL42:
 new_e=e;goto _LL0;case 33U: _LL43: _tmp159=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp158=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL44:
# 406
({struct Cyc_Absyn_Exp*_tmpBE0=({void*_tmpBDF=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpBDE=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp100;_tmp100.tag=33U;({void*_tmpBDD=Cyc_Tcutil_copy_type(_tmp159);_tmp100.f1=_tmpBDD;});_tmp100.f2=_tmp158;_tmp100;});_tmpFF[0]=_tmpBDE;});_tmpFF;});Cyc_Absyn_new_exp(_tmpBDF,e->loc);});new_e=_tmpBE0;});
goto _LL0;case 34U: _LL45: _tmp15F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).is_calloc;_tmp15E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).rgn;_tmp15D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).elt_type;_tmp15C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).num_elts;_tmp15B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).fat_result;_tmp15A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).inline_call;_LL46: {
# 409
struct Cyc_Absyn_Exp*_tmp101=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp15E;if(_tmp15E != 0)({struct Cyc_Absyn_Exp*_tmpBE1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp15E);r1=_tmpBE1;});{
void**t1=_tmp15D;if(_tmp15D != 0)({void**_tmpBE3=({void**_tmp102=_cycalloc(sizeof(*_tmp102));({void*_tmpBE2=Cyc_Tcutil_copy_type(*_tmp15D);_tmp102[0]=_tmpBE2;});_tmp102;});t1=_tmpBE3;});
({void*_tmpBE6=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp103=_cycalloc(sizeof(*_tmp103));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpBE5=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp104;_tmp104.tag=34U;({struct Cyc_Absyn_MallocInfo _tmpBE4=({struct Cyc_Absyn_MallocInfo _tmp105;_tmp105.is_calloc=_tmp15F;_tmp105.rgn=r1;_tmp105.elt_type=t1;_tmp105.num_elts=_tmp15C;_tmp105.fat_result=_tmp15B;_tmp105.inline_call=_tmp15A;_tmp105;});_tmp104.f1=_tmpBE4;});_tmp104;});_tmp103[0]=_tmpBE5;});_tmp103;});_tmp101->r=_tmpBE6;});
new_e=_tmp101;
goto _LL0;};}case 35U: _LL47: _tmp161=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp160=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL48:
({struct Cyc_Absyn_Exp*_tmpBE9=({struct Cyc_Absyn_Exp*_tmpBE8=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp161);struct Cyc_Absyn_Exp*_tmpBE7=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp160);Cyc_Absyn_swap_exp(_tmpBE8,_tmpBE7,e->loc);});new_e=_tmpBE9;});goto _LL0;case 36U: _LL49: _tmp163=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp162=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL4A: {
# 417
struct Cyc_Core_Opt*nopt1=_tmp163;
if(_tmp163 != 0)({struct Cyc_Core_Opt*_tmpBEA=({struct Cyc_Core_Opt*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->v=(struct _tuple2*)_tmp163->v;_tmp106;});nopt1=_tmpBEA;});
({struct Cyc_Absyn_Exp*_tmpBEE=({void*_tmpBED=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpBEC=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=36U;_tmp108.f1=nopt1;({struct Cyc_List_List*_tmpBEB=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp162);_tmp108.f2=_tmpBEB;});_tmp108;});_tmp107[0]=_tmpBEC;});_tmp107;});Cyc_Absyn_new_exp(_tmpBED,e->loc);});new_e=_tmpBEE;});
goto _LL0;}case 37U: _LL4B: _LL4C:
# 422
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp109=_cycalloc(sizeof(*_tmp109));({struct Cyc_Core_Failure_exn_struct _tmpBF0=({struct Cyc_Core_Failure_exn_struct _tmp10A;_tmp10A.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmpBEF=({const char*_tmp10B="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp10B,sizeof(char),45U);});_tmp10A.f1=_tmpBEF;});_tmp10A;});_tmp109[0]=_tmpBF0;});_tmp109;}));case 38U: _LL4D: _tmp165=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp164=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL4E:
({struct Cyc_Absyn_Exp*_tmpBF4=({void*_tmpBF3=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpBF2=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp10D;_tmp10D.tag=38U;({struct Cyc_Absyn_Exp*_tmpBF1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp165);_tmp10D.f1=_tmpBF1;});_tmp10D.f2=_tmp164;_tmp10D;});_tmp10C[0]=_tmpBF2;});_tmp10C;});Cyc_Absyn_new_exp(_tmpBF3,e->loc);});new_e=_tmpBF4;});
goto _LL0;case 39U: _LL4F: _tmp166=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL50:
({struct Cyc_Absyn_Exp*_tmpBF6=({void*_tmpBF5=Cyc_Tcutil_copy_type(_tmp166);Cyc_Absyn_valueof_exp(_tmpBF5,e->loc);});new_e=_tmpBF6;});
goto _LL0;default: _LL51: _tmp168=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp167=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL52:
({struct Cyc_Absyn_Exp*_tmpBF7=Cyc_Absyn_asm_exp(_tmp168,_tmp167,e->loc);new_e=_tmpBF7;});goto _LL0;}_LL0:;}
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
struct Cyc_Absyn_Kind*_tmp169=ka1;enum Cyc_Absyn_KindQual _tmp174;enum Cyc_Absyn_AliasQual _tmp173;_LL1: _tmp174=_tmp169->kind;_tmp173=_tmp169->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp16A=ka2;enum Cyc_Absyn_KindQual _tmp172;enum Cyc_Absyn_AliasQual _tmp171;_LL4: _tmp172=_tmp16A->kind;_tmp171=_tmp16A->aliasqual;_LL5:;
# 454
if(_tmp174 != _tmp172){
struct _tuple20 _tmp16B=({struct _tuple20 _tmp16D;_tmp16D.f1=_tmp174;_tmp16D.f2=_tmp172;_tmp16D;});struct _tuple20 _tmp16C=_tmp16B;switch(_tmp16C.f1){case Cyc_Absyn_BoxKind: switch(_tmp16C.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp16C.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 463
if(_tmp173 != _tmp171){
struct _tuple21 _tmp16E=({struct _tuple21 _tmp170;_tmp170.f1=_tmp173;_tmp170.f2=_tmp171;_tmp170;});struct _tuple21 _tmp16F=_tmp16E;switch(_tmp16F.f1){case Cyc_Absyn_Aliasable: if(_tmp16F.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp16F.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 470
return 1;};}
# 473
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp175=Cyc_Absyn_compress_kb(tv->kind);void*_tmp176=_tmp175;struct Cyc_Absyn_Kind*_tmp17A;struct Cyc_Absyn_Kind*_tmp179;switch(*((int*)_tmp176)){case 0U: _LL1: _tmp179=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp176)->f1;_LL2:
 return _tmp179;case 2U: _LL3: _tmp17A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp176)->f2;_LL4:
 return _tmp17A;default: _LL5: _LL6:
# 478
({void*_tmpBF9=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp177=_cycalloc(sizeof(*_tmp177));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpBF8=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp178;_tmp178.tag=2U;_tmp178.f1=0;_tmp178.f2=def;_tmp178;});_tmp177[0]=_tmpBF8;});_tmp177;});tv->kind=_tmpBF9;});
return def;}_LL0:;}
# 483
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp17B=({struct _tuple0 _tmp192;({void*_tmpBFA=Cyc_Absyn_compress_kb(kb1);_tmp192.f1=_tmpBFA;});({void*_tmpBFB=Cyc_Absyn_compress_kb(kb2);_tmp192.f2=_tmpBFB;});_tmp192;});struct _tuple0 _tmp17C=_tmp17B;struct Cyc_Core_Opt**_tmp191;void*_tmp190;void*_tmp18F;struct Cyc_Core_Opt**_tmp18E;struct Cyc_Core_Opt**_tmp18D;struct Cyc_Absyn_Kind*_tmp18C;struct Cyc_Core_Opt**_tmp18B;struct Cyc_Absyn_Kind*_tmp18A;struct Cyc_Core_Opt**_tmp189;struct Cyc_Absyn_Kind*_tmp188;struct Cyc_Absyn_Kind*_tmp187;struct Cyc_Absyn_Kind*_tmp186;struct Cyc_Core_Opt**_tmp185;struct Cyc_Absyn_Kind*_tmp184;struct Cyc_Absyn_Kind*_tmp183;struct Cyc_Absyn_Kind*_tmp182;switch(*((int*)_tmp17C.f1)){case 0U: switch(*((int*)_tmp17C.f2)){case 0U: _LL1: _tmp183=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp182=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_LL2:
 return _tmp183 == _tmp182;case 2U: _LL5: _tmp186=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp185=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_tmp184=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f2;_LL6:
# 492
 if(Cyc_Tcutil_kind_leq(_tmp186,_tmp184)){
({struct Cyc_Core_Opt*_tmpBFC=({struct Cyc_Core_Opt*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->v=kb1;_tmp17E;});*_tmp185=_tmpBFC;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp17C.f2)){case 0U: _LL3: _tmp189=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp188=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f2;_tmp187=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_LL4:
# 487
 if(Cyc_Tcutil_kind_leq(_tmp187,_tmp188)){
({struct Cyc_Core_Opt*_tmpBFD=({struct Cyc_Core_Opt*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->v=kb2;_tmp17D;});*_tmp189=_tmpBFD;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp18D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp18C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f2;_tmp18B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_tmp18A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f2;_LL8:
# 497
 if(Cyc_Tcutil_kind_leq(_tmp18C,_tmp18A)){
({struct Cyc_Core_Opt*_tmpBFE=({struct Cyc_Core_Opt*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->v=kb1;_tmp17F;});*_tmp18B=_tmpBFE;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp18A,_tmp18C)){
({struct Cyc_Core_Opt*_tmpBFF=({struct Cyc_Core_Opt*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=kb2;_tmp180;});*_tmp18D=_tmpBFF;});
return 1;}else{
return 0;}}default: _LLB: _tmp18F=_tmp17C.f1;_tmp18E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_LLC:
# 506
({struct Cyc_Core_Opt*_tmpC00=({struct Cyc_Core_Opt*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->v=_tmp18F;_tmp181;});*_tmp18E=_tmpC00;});
return 1;}default: _LL9: _tmp191=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp190=_tmp17C.f2;_LLA:
# 504
 _tmp18F=_tmp190;_tmp18E=_tmp191;goto _LLC;}_LL0:;}
# 511
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp193=Cyc_Tcutil_compress(t);void*_tmp194=_tmp193;struct Cyc_Absyn_Tvar*_tmp19A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp194)->tag == 2U){_LL1: _tmp19A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp194)->f1;_LL2: {
# 514
void*_tmp195=_tmp19A->kind;
_tmp19A->kind=kb;
return({struct _tuple16 _tmp196;_tmp196.f1=_tmp19A;_tmp196.f2=_tmp195;_tmp196;});}}else{_LL3: _LL4:
# 518
({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0U;({struct _dyneither_ptr _tmpC01=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp199.f1=_tmpC01;});({void*_tmp197[1U]={& _tmp199};({struct _dyneither_ptr _tmpC02=({const char*_tmp198="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp198,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC02,_tag_dyneither(_tmp197,sizeof(void*),1U));});});});}_LL0:;}
# 524
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 526
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 530
struct _RegionHandle _tmp19B=_new_region("temp");struct _RegionHandle*temp=& _tmp19B;_push_region(temp);
{struct Cyc_List_List*_tmp19C=0;
# 533
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp19D=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp19E=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp19F=Cyc_Tcutil_tvar_kind(_tmp19D,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1A0=_tmp19F;switch(((struct Cyc_Absyn_Kind*)_tmp1A0)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 539
({struct Cyc_List_List*_tmpC04=({struct Cyc_List_List*_tmp1A1=_region_malloc(temp,sizeof(*_tmp1A1));({struct _tuple16*_tmpC03=({struct _tuple16*_tmp1A2=_region_malloc(temp,sizeof(*_tmp1A2));_tmp1A2->f1=_tmp19D;_tmp1A2->f2=_tmp19E;_tmp1A2;});_tmp1A1->hd=_tmpC03;});_tmp1A1->tl=_tmp19C;_tmp1A1;});_tmp19C=_tmpC04;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 543
if(_tmp19C != 0){
({void*_tmpC07=({struct _RegionHandle*_tmpC06=temp;struct Cyc_List_List*_tmpC05=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp19C);Cyc_Tcutil_rsubstitute(_tmpC06,_tmpC05,field_typ);});field_typ=_tmpC07;});
({struct Cyc_Absyn_Kind*_tmpC08=Cyc_Tcutil_typ_kind(field_typ);k=_tmpC08;});}}
# 531
;_pop_region(temp);}
# 548
return k;}
# 555
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1A3=Cyc_Tcutil_compress(t);void*_tmp1A4=_tmp1A3;struct Cyc_Absyn_Typedefdecl*_tmp1C2;struct Cyc_Absyn_Exp*_tmp1C1;struct Cyc_Absyn_Kind*_tmp1C0;struct Cyc_Absyn_PtrInfo _tmp1BF;enum Cyc_Absyn_AggrKind _tmp1BE;struct Cyc_List_List*_tmp1BD;struct Cyc_Absyn_AggrdeclImpl*_tmp1BC;int _tmp1BB;struct Cyc_List_List*_tmp1BA;struct Cyc_Absyn_Datatypedecl*_tmp1B9;struct Cyc_Absyn_Datatypefield*_tmp1B8;enum Cyc_Absyn_Size_of _tmp1B7;struct Cyc_Absyn_Tvar*_tmp1B6;struct Cyc_Core_Opt*_tmp1B5;switch(*((int*)_tmp1A4)){case 1U: _LL1: _tmp1B5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A4)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1B5))->v;case 2U: _LL3: _tmp1B6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A4)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp1B6,& Cyc_Tcutil_bk);case 0U: _LL5: _LL6:
 return& Cyc_Tcutil_mk;case 6U: _LL7: _tmp1B7=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1A4)->f2;_LL8:
# 561
 return(_tmp1B7 == Cyc_Absyn_Int_sz  || _tmp1B7 == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7U: _LL9: _LLA:
# 563
 return& Cyc_Tcutil_mk;case 9U: _LLB: _LLC:
 return& Cyc_Tcutil_ak;case 16U: _LLD: _LLE:
 goto _LL10;case 13U: _LLF: _LL10:
 goto _LL12;case 14U: _LL11: _LL12:
 goto _LL14;case 15U: _LL13: _LL14:
 return& Cyc_Tcutil_bk;case 21U: _LL15: _LL16:
 return& Cyc_Tcutil_urk;case 20U: _LL17: _LL18:
 return& Cyc_Tcutil_rk;case 22U: _LL19: _LL1A:
 return& Cyc_Tcutil_trk;case 3U: _LL1B: _LL1C:
# 574
 return& Cyc_Tcutil_ak;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A4)->f1).field_info).KnownDatatypefield).tag == 2){_LL1D: _tmp1B9=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A4)->f1).field_info).KnownDatatypefield).val).f1;_tmp1B8=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A4)->f1).field_info).KnownDatatypefield).val).f2;_LL1E:
# 576
 return& Cyc_Tcutil_mk;}else{_LL1F: _LL20:
# 578
({void*_tmp1A5=0U;({struct _dyneither_ptr _tmpC09=({const char*_tmp1A6="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1A6,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC09,_tag_dyneither(_tmp1A5,sizeof(void*),0U));});});}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A4)->f1).aggr_info).UnknownAggr).tag == 1){_LL21: _LL22:
# 581
 return& Cyc_Tcutil_ak;}else{_LL23: _tmp1BE=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A4)->f1).aggr_info).KnownAggr).val)->kind;_tmp1BD=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A4)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1BC=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A4)->f1).aggr_info).KnownAggr).val)->impl;_tmp1BB=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A4)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1BA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A4)->f1).targs;_LL24:
# 583
 if(_tmp1BC == 0){
if(_tmp1BB)
return& Cyc_Tcutil_mk;else{
# 587
return& Cyc_Tcutil_ak;}}{
# 589
struct Cyc_List_List*_tmp1A7=_tmp1BC->fields;
if(_tmp1A7 == 0)return& Cyc_Tcutil_mk;
# 592
if(_tmp1BE == Cyc_Absyn_StructA){
for(0;_tmp1A7->tl != 0;_tmp1A7=_tmp1A7->tl){;}{
void*_tmp1A8=((struct Cyc_Absyn_Aggrfield*)_tmp1A7->hd)->type;
struct Cyc_Absyn_Kind*_tmp1A9=Cyc_Tcutil_field_kind(_tmp1A8,_tmp1BA,_tmp1BD);
if(_tmp1A9 == & Cyc_Tcutil_ak  || _tmp1A9 == & Cyc_Tcutil_tak)return _tmp1A9;};}else{
# 600
for(0;_tmp1A7 != 0;_tmp1A7=_tmp1A7->tl){
void*_tmp1AA=((struct Cyc_Absyn_Aggrfield*)_tmp1A7->hd)->type;
struct Cyc_Absyn_Kind*_tmp1AB=Cyc_Tcutil_field_kind(_tmp1AA,_tmp1BA,_tmp1BD);
if(_tmp1AB == & Cyc_Tcutil_ak  || _tmp1AB == & Cyc_Tcutil_tak)return _tmp1AB;}}
# 606
return& Cyc_Tcutil_mk;};}case 12U: _LL25: _LL26:
 return& Cyc_Tcutil_mk;case 5U: _LL27: _tmp1BF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A4)->f1;_LL28: {
# 609
void*_tmp1AC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1BF.ptr_atts).bounds);void*_tmp1AD=_tmp1AC;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1AD)->tag == 0U){_LL44: _LL45: {
# 611
enum Cyc_Absyn_AliasQual _tmp1AE=(Cyc_Tcutil_typ_kind((_tmp1BF.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1AF=_tmp1AE;switch(_tmp1AF){case Cyc_Absyn_Aliasable: _LL49: _LL4A:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL4B: _LL4C:
 return& Cyc_Tcutil_umk;default: _LL4D: _LL4E:
 return& Cyc_Tcutil_tmk;}_LL48:;}}else{_LL46: _LL47: {
# 617
enum Cyc_Absyn_AliasQual _tmp1B0=(Cyc_Tcutil_typ_kind((_tmp1BF.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1B1=_tmp1B0;switch(_tmp1B1){case Cyc_Absyn_Aliasable: _LL50: _LL51:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL52: _LL53:
 return& Cyc_Tcutil_ubk;default: _LL54: _LL55:
 return& Cyc_Tcutil_tbk;}_LL4F:;}}_LL43:;}case 18U: _LL29: _LL2A:
# 623
 return& Cyc_Tcutil_ik;case 28U: _LL2B: _tmp1C0=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1A4)->f2;_LL2C:
 return _tmp1C0;case 27U: _LL2D: _LL2E:
# 628
 return& Cyc_Tcutil_ak;case 19U: _LL2F: _LL30:
 return& Cyc_Tcutil_bk;case 8U: _LL31: _tmp1C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A4)->f1).num_elts;_LL32:
# 631
 if(_tmp1C1 == 0  || Cyc_Tcutil_is_const_exp(_tmp1C1))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10U: _LL33: _LL34:
 return& Cyc_Tcutil_mk;case 17U: _LL35: _tmp1C2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A4)->f3;_LL36:
# 635
 if(_tmp1C2 == 0  || _tmp1C2->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B4;_tmp1B4.tag=0U;({struct _dyneither_ptr _tmpC0A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1B4.f1=_tmpC0A;});({void*_tmp1B2[1U]={& _tmp1B4};({struct _dyneither_ptr _tmpC0B=({const char*_tmp1B3="typ_kind: typedef found: %s";_tag_dyneither(_tmp1B3,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC0B,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C2->kind))->v;case 26U: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A4)->f1)->r)){case 0U: _LL37: _LL38:
 return& Cyc_Tcutil_ak;case 1U: _LL39: _LL3A:
 return& Cyc_Tcutil_bk;default: _LL3B: _LL3C:
 return& Cyc_Tcutil_ak;}case 23U: _LL3D: _LL3E:
 goto _LL40;case 24U: _LL3F: _LL40:
 goto _LL42;default: _LL41: _LL42:
 return& Cyc_Tcutil_ek;}_LL0:;}
# 647
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 652
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1C3;_push_handler(& _tmp1C3);{int _tmp1C5=0;if(setjmp(_tmp1C3.handler))_tmp1C5=1;if(!_tmp1C5){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1C6=1;_npop_handler(0U);return _tmp1C6;};
# 654
;_pop_handler();}else{void*_tmp1C4=(void*)_exn_thrown;void*_tmp1C7=_tmp1C4;void*_tmp1C8;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1C7)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 656
 return 0;}else{_LL3: _tmp1C8=_tmp1C7;_LL4:(int)_rethrow(_tmp1C8);}_LL0:;}};}
# 661
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
({void*_tmpC0C=Cyc_Tcutil_compress(t);t=_tmpC0C;});{
void*_tmp1C9=t;struct Cyc_List_List*_tmp1EF;void*_tmp1EE;void*_tmp1ED;void*_tmp1EC;void*_tmp1EB;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E4;void*_tmp1E3;struct Cyc_Absyn_Tqual _tmp1E2;void*_tmp1E1;struct Cyc_List_List*_tmp1E0;int _tmp1DF;struct Cyc_Absyn_VarargInfo*_tmp1DE;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1D9;struct Cyc_List_List*_tmp1D8;void*_tmp1D7;struct Cyc_Absyn_PtrInfo _tmp1D6;void*_tmp1D5;struct Cyc_Core_Opt**_tmp1D4;struct Cyc_Absyn_Tvar*_tmp1D3;switch(*((int*)_tmp1C9)){case 2U: _LL1: _tmp1D3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C9)->f1;_LL2:
# 666
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1D3)){
({struct _dyneither_ptr _tmpC0D=({const char*_tmp1CA="(type variable would escape scope)";_tag_dyneither(_tmp1CA,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpC0D;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 670
goto _LL0;case 1U: _LL3: _tmp1D5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C9)->f2;_tmp1D4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C9)->f4;_LL4:
# 672
 if(t == evar){
({struct _dyneither_ptr _tmpC0E=({const char*_tmp1CB="(occurs check)";_tag_dyneither(_tmp1CB,sizeof(char),15U);});Cyc_Tcutil_failure_reason=_tmpC0E;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 676
if(_tmp1D5 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1D5);else{
# 679
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D4))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 686
if(problem){
struct Cyc_List_List*_tmp1CC=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D4))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
({struct Cyc_List_List*_tmpC0F=({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1CD->tl=_tmp1CC;_tmp1CD;});_tmp1CC=_tmpC0F;});}}
# 692
({struct Cyc_Core_Opt*_tmpC10=({struct Cyc_Core_Opt*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->v=_tmp1CC;_tmp1CE;});*_tmp1D4=_tmpC10;});}}}
# 695
goto _LL0;case 5U: _LL5: _tmp1D6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C9)->f1;_LL6:
# 698
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D6.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1D6.ptr_atts).rgn);
goto _LL0;case 8U: _LL7: _tmp1D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C9)->f1).elt_type;_LL8:
# 702
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D7);goto _LL0;case 9U: _LL9: _tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).tvars;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).effect;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ret_tqual;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ret_typ;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).args;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).c_varargs;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).cyc_varargs;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).rgn_po;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).attributes;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).requires_clause;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).requires_relns;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ensures_clause;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ensures_relns;_LLA:
# 705
({struct Cyc_List_List*_tmpC11=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1E4,env);env=_tmpC11;});
if(_tmp1E3 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1E3);
Cyc_Tcutil_occurs(evar,r,env,_tmp1E1);
for(0;_tmp1E0 != 0;_tmp1E0=_tmp1E0->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1E0->hd)).f3);}
if(_tmp1DE != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1DE->type);
for(0;_tmp1DD != 0;_tmp1DD=_tmp1DD->tl){
struct _tuple0*_tmp1CF=(struct _tuple0*)_tmp1DD->hd;struct _tuple0*_tmp1D0=_tmp1CF;void*_tmp1D2;void*_tmp1D1;_LL24: _tmp1D2=_tmp1D0->f1;_tmp1D1=_tmp1D0->f2;_LL25:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1D2);
Cyc_Tcutil_occurs(evar,r,env,_tmp1D1);}
# 717
goto _LL0;case 10U: _LLB: _tmp1E5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1C9)->f1;_LLC:
# 719
 for(0;_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1E5->hd)).f2);}
goto _LL0;case 3U: _LLD: _tmp1E6=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1C9)->f1).targs;_LLE:
# 723
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E6);goto _LL0;case 17U: _LLF: _tmp1E7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1C9)->f2;_LL10:
 _tmp1E8=_tmp1E7;goto _LL12;case 4U: _LL11: _tmp1E8=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1C9)->f1).targs;_LL12:
 _tmp1E9=_tmp1E8;goto _LL14;case 11U: _LL13: _tmp1E9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1C9)->f1).targs;_LL14:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1E9);goto _LL0;case 12U: _LL15: _tmp1EA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1C9)->f2;_LL16:
# 729
 for(0;_tmp1EA != 0;_tmp1EA=_tmp1EA->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1EA->hd)->type);}
goto _LL0;case 19U: _LL17: _tmp1EB=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1C9)->f1;_LL18:
 _tmp1EC=_tmp1EB;goto _LL1A;case 23U: _LL19: _tmp1EC=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1C9)->f1;_LL1A:
 _tmp1ED=_tmp1EC;goto _LL1C;case 15U: _LL1B: _tmp1ED=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1C9)->f1;_LL1C:
 _tmp1EE=_tmp1ED;goto _LL1E;case 25U: _LL1D: _tmp1EE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1C9)->f1;_LL1E:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1EE);goto _LL0;case 24U: _LL1F: _tmp1EF=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1C9)->f1;_LL20:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1EF);goto _LL0;default: _LL21: _LL22:
# 739
 goto _LL0;}_LL0:;};}
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
({void*_tmp1F0=0U;({struct _dyneither_ptr _tmpC12=({const char*_tmp1F1="tq1 real_const not set.";_tag_dyneither(_tmp1F1,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC12,_tag_dyneither(_tmp1F0,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1F2=0U;({struct _dyneither_ptr _tmpC13=({const char*_tmp1F3="tq2 real_const not set.";_tag_dyneither(_tmp1F3,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC13,_tag_dyneither(_tmp1F2,sizeof(void*),0U));});});
# 763
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 766
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
({struct _dyneither_ptr _tmpC14=({const char*_tmp1F4="(qualifiers don't match)";_tag_dyneither(_tmp1F4,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC14;});
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
({union Cyc_Absyn_Constraint*_tmpC15=Cyc_Absyn_compress_conref(x);x=_tmpC15;});
({union Cyc_Absyn_Constraint*_tmpC16=Cyc_Absyn_compress_conref(y);y=_tmpC16;});
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1F5=x;void*_tmp1FD;switch((((union Cyc_Absyn_Constraint*)_tmp1F5)->Eq_constr).tag){case 3U: _LL1: _LL2:
# 792
({union Cyc_Absyn_Constraint _tmpC17=({union Cyc_Absyn_Constraint _tmp1F6;(_tmp1F6.Forward_constr).val=y;(_tmp1F6.Forward_constr).tag=2;_tmp1F6;});*x=_tmpC17;});return;case 1U: _LL3: _tmp1FD=(void*)(_tmp1F5->Eq_constr).val;_LL4: {
# 794
union Cyc_Absyn_Constraint*_tmp1F7=y;void*_tmp1FA;switch((((union Cyc_Absyn_Constraint*)_tmp1F7)->Eq_constr).tag){case 3U: _LL8: _LL9:
*y=*x;return;case 1U: _LLA: _tmp1FA=(void*)(_tmp1F7->Eq_constr).val;_LLB:
# 797
 if(cmp(_tmp1FD,_tmp1FA)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 801
return;default: _LLC: _LLD:
({void*_tmp1F8=0U;({struct _dyneither_ptr _tmpC18=({const char*_tmp1F9="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1F9,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpC18,_tag_dyneither(_tmp1F8,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 804
({void*_tmp1FB=0U;({struct _dyneither_ptr _tmpC19=({const char*_tmp1FC="unify_conref: forward after compress";_tag_dyneither(_tmp1FC,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpC19,_tag_dyneither(_tmp1FB,sizeof(void*),0U));});});}_LL0:;};}
# 808
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp1FE;_push_handler(& _tmp1FE);{int _tmp200=0;if(setjmp(_tmp1FE.handler))_tmp200=1;if(!_tmp200){
({int(*_tmpC1C)(void*,void*)=cmp;union Cyc_Absyn_Constraint*_tmpC1B=x;union Cyc_Absyn_Constraint*_tmpC1A=y;Cyc_Tcutil_unify_it_conrefs(_tmpC1C,_tmpC1B,_tmpC1A,_tag_dyneither(0,0,0));});{
int _tmp201=1;_npop_handler(0U);return _tmp201;};
# 810
;_pop_handler();}else{void*_tmp1FF=(void*)_exn_thrown;void*_tmp202=_tmp1FF;void*_tmp203;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp202)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 812
 return 0;}else{_LL3: _tmp203=_tmp202;_LL4:(int)_rethrow(_tmp203);}_LL0:;}};}
# 815
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp204=({struct _tuple0 _tmp208;_tmp208.f1=b1;_tmp208.f2=b2;_tmp208;});struct _tuple0 _tmp205=_tmp204;struct Cyc_Absyn_Exp*_tmp207;struct Cyc_Absyn_Exp*_tmp206;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp205.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp205.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp205.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{_LL7: _tmp207=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp205.f1)->f1;_tmp206=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp205.f2)->f1;_LL8:
 return Cyc_Evexp_const_exp_cmp(_tmp207,_tmp206);}}_LL0:;}
# 824
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple0 _tmp209=({struct _tuple0 _tmp217;_tmp217.f1=e1->r;_tmp217.f2=e2->r;_tmp217;});struct _tuple0 _tmp20A=_tmp209;void*_tmp216;void*_tmp215;struct Cyc_Absyn_Exp*_tmp214;struct Cyc_Absyn_Exp*_tmp213;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20A.f1)->tag == 14U){_LL1: _tmp213=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20A.f1)->f2;_LL2:
# 829
 Cyc_Tcutil_unify_cmp_exp(_tmp213,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20A.f2)->tag == 14U){_LL3: _tmp214=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20A.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp214);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20A.f1)->tag == 39U){_LL5: _tmp215=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20A.f1)->f1;_LL6: {
# 832
void*_tmp20B=Cyc_Tcutil_compress(_tmp215);void*_tmp20C=_tmp20B;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20C)->tag == 1U){_LLC: _LLD:
({void*_tmpC1E=_tmp215;Cyc_Tcutil_unify_it(_tmpC1E,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp20D=_cycalloc(sizeof(*_tmp20D));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC1D=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp20E;_tmp20E.tag=18U;_tmp20E.f1=e2;_tmp20E;});_tmp20D[0]=_tmpC1D;});_tmp20D;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20A.f2)->tag == 39U){_LL7: _tmp216=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20A.f2)->f1;_LL8: {
# 837
void*_tmp20F=Cyc_Tcutil_compress(_tmp216);void*_tmp210=_tmp20F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp210)->tag == 1U){_LL11: _LL12:
({void*_tmpC20=_tmp216;Cyc_Tcutil_unify_it(_tmpC20,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp211=_cycalloc(sizeof(*_tmp211));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC1F=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp212;_tmp212.tag=18U;_tmp212.f1=e1;_tmp212;});_tmp211[0]=_tmpC1F;});_tmp211;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 841
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 845
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp218=({struct _tuple0 _tmp222;_tmp222.f1=b1;_tmp222.f2=b2;_tmp222;});struct _tuple0 _tmp219=_tmp218;struct Cyc_Absyn_Exp*_tmp221;struct Cyc_Absyn_Exp*_tmp220;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f2)->tag == 0U){_LL1: _LL2:
 return 0;}else{_LL3: _LL4:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp219.f2)->tag == 0U){_LL5: _LL6:
 return 1;}else{_LL7: _tmp221=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp219.f1)->f1;_tmp220=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp219.f2)->f1;_LL8: {
# 851
int r=Cyc_Evexp_const_exp_cmp(_tmp221,_tmp220);
if(r == 0)return 0;{
struct _handler_cons _tmp21A;_push_handler(& _tmp21A);{int _tmp21C=0;if(setjmp(_tmp21A.handler))_tmp21C=1;if(!_tmp21C){
Cyc_Tcutil_unify_cmp_exp(_tmp221,_tmp220);{
int _tmp21D=0;_npop_handler(0U);return _tmp21D;};
# 854
;_pop_handler();}else{void*_tmp21B=(void*)_exn_thrown;void*_tmp21E=_tmp21B;void*_tmp21F;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp21E)->tag == Cyc_Tcutil_Unify){_LLA: _LLB:
# 857
 return r;}else{_LLC: _tmp21F=_tmp21E;_LLD:(int)_rethrow(_tmp21F);}_LL9:;}};};}}}_LL0:;}
# 862
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp223=att;switch(*((int*)_tmp223)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: _LLF: _LL10:
 return 7;case 8U: _LL11: _LL12:
 return 8;case 9U: _LL13: _LL14:
 return 9;case 10U: _LL15: _LL16:
 return 10;case 11U: _LL17: _LL18:
 return 11;case 12U: _LL19: _LL1A:
 return 12;case 13U: _LL1B: _LL1C:
 return 13;case 14U: _LL1D: _LL1E:
 return 14;case 15U: _LL1F: _LL20:
 return 15;case 16U: _LL21: _LL22:
 return 16;case 17U: _LL23: _LL24:
 return 17;case 18U: _LL25: _LL26:
 return 18;case 19U: _LL27: _LL28:
 return 19;case 20U: _LL29: _LL2A:
 return 20;default: _LL2B: _LL2C:
 return 21;}_LL0:;}
# 889
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp224=({struct _tuple0 _tmp236;_tmp236.f1=att1;_tmp236.f2=att2;_tmp236;});struct _tuple0 _tmp225=_tmp224;enum Cyc_Absyn_Format_Type _tmp235;int _tmp234;int _tmp233;enum Cyc_Absyn_Format_Type _tmp232;int _tmp231;int _tmp230;struct _dyneither_ptr _tmp22F;struct _dyneither_ptr _tmp22E;struct Cyc_Absyn_Exp*_tmp22D;struct Cyc_Absyn_Exp*_tmp22C;int _tmp22B;int _tmp22A;int _tmp229;int _tmp228;switch(*((int*)_tmp225.f1)){case 0U: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp225.f2)->tag == 0U){_LL1: _tmp229=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp225.f1)->f1;_tmp228=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp225.f2)->f1;_LL2:
 _tmp22B=_tmp229;_tmp22A=_tmp228;goto _LL4;}else{goto _LLB;}case 20U: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp225.f2)->tag == 20U){_LL3: _tmp22B=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp225.f1)->f1;_tmp22A=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp225.f2)->f1;_LL4:
# 893
 return Cyc_Core_intcmp(_tmp22B,_tmp22A);}else{goto _LLB;}case 6U: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp225.f2)->tag == 6U){_LL5: _tmp22D=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp225.f1)->f1;_tmp22C=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp225.f2)->f1;_LL6:
# 895
 if(_tmp22D == _tmp22C)return 0;
if(_tmp22D == 0)return - 1;
if(_tmp22C == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp22D,_tmp22C);}else{goto _LLB;}case 8U: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp225.f2)->tag == 8U){_LL7: _tmp22F=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp225.f1)->f1;_tmp22E=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp225.f2)->f1;_LL8:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp22F,(struct _dyneither_ptr)_tmp22E);}else{goto _LLB;}case 19U: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp225.f2)->tag == 19U){_LL9: _tmp235=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp225.f1)->f1;_tmp234=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp225.f1)->f2;_tmp233=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp225.f1)->f3;_tmp232=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp225.f2)->f1;_tmp231=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp225.f2)->f2;_tmp230=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp225.f2)->f3;_LLA: {
# 901
int _tmp226=Cyc_Core_intcmp((int)((unsigned int)_tmp235),(int)((unsigned int)_tmp232));
if(_tmp226 != 0)return _tmp226;{
int _tmp227=Cyc_Core_intcmp(_tmp234,_tmp231);
if(_tmp227 != 0)return _tmp227;
return Cyc_Core_intcmp(_tmp233,_tmp230);};}}else{goto _LLB;}default: _LLB: _LLC:
# 907
 return({int _tmpC21=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpC21,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
# 911
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 915
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 924
static void*Cyc_Tcutil_rgns_of(void*t);
# 926
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 930
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp237=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp238=_tmp237;switch(((struct Cyc_Absyn_Kind*)_tmp238)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp238)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL0;default: goto _LL9;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
({void*_tmpC24=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp239=_cycalloc(sizeof(*_tmp239));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC23=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp23A;_tmp23A.tag=18U;({struct Cyc_Absyn_Exp*_tmpC22=Cyc_Absyn_uint_exp(0U,0U);_tmp23A.f1=_tmpC22;});_tmp23A;});_tmp239[0]=_tmpC23;});_tmp239;});t=_tmpC24;});goto _LL0;default: _LL9: _LLA:
 t=Cyc_Absyn_sint_typ;goto _LL0;}_LL0:;}
# 939
return({struct _tuple16*_tmp23B=_cycalloc(sizeof(*_tmp23B));_tmp23B->f1=tv;_tmp23B->f2=t;_tmp23B;});}
# 946
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp23C=Cyc_Tcutil_compress(t);void*_tmp23D=_tmp23C;struct Cyc_List_List*_tmp26D;void*_tmp26C;struct Cyc_List_List*_tmp26B;void*_tmp26A;struct Cyc_Absyn_Tqual _tmp269;void*_tmp268;struct Cyc_List_List*_tmp267;struct Cyc_Absyn_VarargInfo*_tmp266;struct Cyc_List_List*_tmp265;struct Cyc_List_List*_tmp264;struct Cyc_List_List*_tmp263;struct Cyc_List_List*_tmp262;struct Cyc_List_List*_tmp261;void*_tmp260;void*_tmp25F;void*_tmp25E;struct Cyc_List_List*_tmp25D;void*_tmp25C;void*_tmp25B;void*_tmp25A;switch(*((int*)_tmp23D)){case 0U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 goto _LLA;case 18U: _LL9: _LLA:
 goto _LLC;case 27U: _LLB: _LLC:
 goto _LLE;case 28U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 return Cyc_Absyn_empty_effect;case 1U: _LL11: _LL12:
 goto _LL14;case 2U: _LL13: _LL14: {
# 958
struct Cyc_Absyn_Kind*_tmp23E=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp23F=_tmp23E;switch(((struct Cyc_Absyn_Kind*)_tmp23F)->kind){case Cyc_Absyn_RgnKind: _LL3C: _LL3D:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp240=_cycalloc(sizeof(*_tmp240));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC25=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp241;_tmp241.tag=23U;_tmp241.f1=t;_tmp241;});_tmp240[0]=_tmpC25;});_tmp240;});case Cyc_Absyn_EffKind: _LL3E: _LL3F:
 return t;case Cyc_Absyn_IntKind: _LL40: _LL41:
 return Cyc_Absyn_empty_effect;default: _LL42: _LL43:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp242=_cycalloc(sizeof(*_tmp242));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC26=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp243;_tmp243.tag=25U;_tmp243.f1=t;_tmp243;});_tmp242[0]=_tmpC26;});_tmp242;});}_LL3B:;}case 15U: _LL15: _tmp25A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp23D)->f1;_LL16:
# 964
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp244=_cycalloc(sizeof(*_tmp244));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC27=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp245;_tmp245.tag=23U;_tmp245.f1=_tmp25A;_tmp245;});_tmp244[0]=_tmpC27;});_tmp244;});case 16U: _LL17: _tmp25C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23D)->f1;_tmp25B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23D)->f2;_LL18:
# 968
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp246=_cycalloc(sizeof(*_tmp246));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC28=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp247;_tmp247.tag=23U;_tmp247.f1=_tmp25B;_tmp247;});_tmp246[0]=_tmpC28;});_tmp246;});case 3U: _LL19: _tmp25D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp23D)->f1).targs;_LL1A: {
# 970
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp25D);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp248=_cycalloc(sizeof(*_tmp248));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC29=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp249;_tmp249.tag=24U;_tmp249.f1=ts;_tmp249;});_tmp248[0]=_tmpC29;});_tmp248;}));}case 5U: _LL1B: _tmp25F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23D)->f1).elt_typ;_tmp25E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23D)->f1).ptr_atts).rgn;_LL1C:
# 974
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp24A=_cycalloc(sizeof(*_tmp24A));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC2E=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp24B;_tmp24B.tag=24U;({struct Cyc_List_List*_tmpC2D=({void*_tmp24C[2U];({void*_tmpC2A=Cyc_Tcutil_rgns_of(_tmp25F);_tmp24C[1U]=_tmpC2A;});({void*_tmpC2C=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp24D=_cycalloc(sizeof(*_tmp24D));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC2B=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp24E;_tmp24E.tag=23U;_tmp24E.f1=_tmp25E;_tmp24E;});_tmp24D[0]=_tmpC2B;});_tmp24D;});_tmp24C[0U]=_tmpC2C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp24C,sizeof(void*),2U));});_tmp24B.f1=_tmpC2D;});_tmp24B;});_tmp24A[0]=_tmpC2E;});_tmp24A;}));case 8U: _LL1D: _tmp260=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23D)->f1).elt_type;_LL1E:
# 976
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp260));case 10U: _LL1F: _tmp261=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23D)->f1;_LL20: {
# 978
struct Cyc_List_List*_tmp24F=0;
for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
({struct Cyc_List_List*_tmpC2F=({struct Cyc_List_List*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->hd=(*((struct _tuple12*)_tmp261->hd)).f2;_tmp250->tl=_tmp24F;_tmp250;});_tmp24F=_tmpC2F;});}
_tmp262=_tmp24F;goto _LL22;}case 4U: _LL21: _tmp262=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23D)->f1).targs;_LL22:
 _tmp263=_tmp262;goto _LL24;case 11U: _LL23: _tmp263=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23D)->f1).targs;_LL24:
# 984
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp251=_cycalloc(sizeof(*_tmp251));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC31=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp252;_tmp252.tag=24U;({struct Cyc_List_List*_tmpC30=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp263);_tmp252.f1=_tmpC30;});_tmp252;});_tmp251[0]=_tmpC31;});_tmp251;}));case 12U: _LL25: _tmp264=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23D)->f2;_LL26:
# 986
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp253=_cycalloc(sizeof(*_tmp253));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC33=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp254;_tmp254.tag=24U;({struct Cyc_List_List*_tmpC32=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp264);_tmp254.f1=_tmpC32;});_tmp254;});_tmp253[0]=_tmpC33;});_tmp253;}));case 19U: _LL27: _LL28:
 return Cyc_Absyn_empty_effect;case 9U: _LL29: _tmp26B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).tvars;_tmp26A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).effect;_tmp269=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).ret_tqual;_tmp268=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).ret_typ;_tmp267=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).args;_tmp266=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).cyc_varargs;_tmp265=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).rgn_po;_LL2A: {
# 996
void*_tmp255=({struct Cyc_List_List*_tmpC34=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp26B);Cyc_Tcutil_substitute(_tmpC34,(void*)_check_null(_tmp26A));});
return Cyc_Tcutil_normalize_effect(_tmp255);}case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 return Cyc_Absyn_empty_effect;case 23U: _LL31: _LL32:
 goto _LL34;case 24U: _LL33: _LL34:
 return t;case 25U: _LL35: _tmp26C=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp23D)->f1;_LL36:
 return Cyc_Tcutil_rgns_of(_tmp26C);case 17U: _LL37: _tmp26D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23D)->f2;_LL38:
# 1005
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp256=_cycalloc(sizeof(*_tmp256));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC36=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp257;_tmp257.tag=24U;({struct Cyc_List_List*_tmpC35=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp26D);_tmp257.f1=_tmpC35;});_tmp257;});_tmp256[0]=_tmpC36;});_tmp256;}));default: _LL39: _LL3A:
({void*_tmp258=0U;({struct _dyneither_ptr _tmpC37=({const char*_tmp259="typedecl in rgns_of";_tag_dyneither(_tmp259,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC37,_tag_dyneither(_tmp258,sizeof(void*),0U));});});}_LL0:;}
# 1013
void*Cyc_Tcutil_normalize_effect(void*e){
({void*_tmpC38=Cyc_Tcutil_compress(e);e=_tmpC38;});{
void*_tmp26E=e;void*_tmp27A;struct Cyc_List_List**_tmp279;switch(*((int*)_tmp26E)){case 24U: _LL1: _tmp279=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp26E)->f1;_LL2: {
# 1017
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp279;for(0;effs != 0;effs=effs->tl){
void*_tmp26F=(void*)effs->hd;
({void*_tmpC39=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp26F));effs->hd=_tmpC39;});{
void*_tmp270=(void*)effs->hd;void*_tmp271=_tmp270;switch(*((int*)_tmp271)){case 24U: _LL8: _LL9:
 goto _LLB;case 23U: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp271)->f1)){case 20U: _LLA: _LLB:
 goto _LLD;case 22U: _LLC: _LLD:
 goto _LLF;case 21U: _LLE: _LLF:
 redo_join=1;goto _LL7;default: goto _LL10;}default: _LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1029
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp279;for(0;effs != 0;effs=effs->tl){
void*_tmp272=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp273=_tmp272;void*_tmp276;struct Cyc_List_List*_tmp275;switch(*((int*)_tmp273)){case 24U: _LL13: _tmp275=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp273)->f1;_LL14:
# 1034
({struct Cyc_List_List*_tmpC3A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp275,effects);effects=_tmpC3A;});
goto _LL12;case 23U: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp273)->f1)){case 20U: _LL15: _LL16:
 goto _LL18;case 22U: _LL17: _LL18:
 goto _LL1A;case 21U: _LL19: _LL1A:
 goto _LL12;default: goto _LL1B;}default: _LL1B: _tmp276=_tmp273;_LL1C:
({struct Cyc_List_List*_tmpC3B=({struct Cyc_List_List*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->hd=_tmp276;_tmp274->tl=effects;_tmp274;});effects=_tmpC3B;});goto _LL12;}_LL12:;}}
# 1042
({struct Cyc_List_List*_tmpC3C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp279=_tmpC3C;});
return e;};}case 25U: _LL3: _tmp27A=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp26E)->f1;_LL4: {
# 1045
void*_tmp277=Cyc_Tcutil_compress(_tmp27A);void*_tmp278=_tmp277;switch(*((int*)_tmp278)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp27A);}_LL1D:;}default: _LL5: _LL6:
# 1050
 return e;}_LL0:;};}
# 1055
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27B=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC3F=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp27D;_tmp27D.tag=9U;({struct Cyc_Absyn_FnInfo _tmpC3E=({struct Cyc_Absyn_FnInfo _tmp27E;_tmp27E.tvars=0;_tmp27E.effect=eff;({struct Cyc_Absyn_Tqual _tmpC3D=
Cyc_Absyn_empty_tqual(0U);_tmp27E.ret_tqual=_tmpC3D;});_tmp27E.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp27E.args=0;_tmp27E.c_varargs=0;_tmp27E.cyc_varargs=0;_tmp27E.rgn_po=0;_tmp27E.attributes=0;_tmp27E.requires_clause=0;_tmp27E.requires_relns=0;_tmp27E.ensures_clause=0;_tmp27E.ensures_relns=0;_tmp27E;});_tmp27D.f1=_tmpC3E;});_tmp27D;});_tmp27C[0]=_tmpC3F;});_tmp27C;});
# 1065
return({void*_tmpC42=(void*)_tmp27B;struct Cyc_Absyn_Tqual _tmpC41=Cyc_Absyn_empty_tqual(0U);void*_tmpC40=Cyc_Absyn_bounds_one;Cyc_Absyn_atb_typ(_tmpC42,(void*)& Cyc_Absyn_HeapRgn_val,_tmpC41,_tmpC40,Cyc_Absyn_false_conref);});}
# 1072
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
({void*_tmpC43=Cyc_Tcutil_compress(r);r=_tmpC43;});
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp27F=Cyc_Tcutil_compress(e);void*_tmp280=_tmp27F;struct Cyc_Core_Opt*_tmp2A5;void**_tmp2A4;struct Cyc_Core_Opt*_tmp2A3;void*_tmp2A2;struct Cyc_List_List*_tmp2A1;void*_tmp2A0;switch(*((int*)_tmp280)){case 23U: _LL1: _tmp2A0=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp280)->f1;_LL2:
# 1079
 if(constrain)return Cyc_Tcutil_unify(r,_tmp2A0);
({void*_tmpC44=Cyc_Tcutil_compress(_tmp2A0);_tmp2A0=_tmpC44;});
if(r == _tmp2A0)return 1;{
struct _tuple0 _tmp281=({struct _tuple0 _tmp285;_tmp285.f1=r;_tmp285.f2=_tmp2A0;_tmp285;});struct _tuple0 _tmp282=_tmp281;struct Cyc_Absyn_Tvar*_tmp284;struct Cyc_Absyn_Tvar*_tmp283;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp282.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp282.f2)->tag == 2U){_LLC: _tmp284=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp282.f1)->f1;_tmp283=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp282.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp284,_tmp283)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};case 24U: _LL3: _tmp2A1=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp280)->f1;_LL4:
# 1087
 for(0;_tmp2A1 != 0;_tmp2A1=_tmp2A1->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp2A1->hd))return 1;}
return 0;case 25U: _LL5: _tmp2A2=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp280)->f1;_LL6: {
# 1091
void*_tmp286=Cyc_Tcutil_rgns_of(_tmp2A2);void*_tmp287=_tmp286;void*_tmp295;void*_tmp294;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp287)->tag == 25U){_LL11: _tmp294=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp287)->f1;_LL12:
# 1093
 if(!constrain)return 0;{
void*_tmp288=Cyc_Tcutil_compress(_tmp294);void*_tmp289=_tmp288;struct Cyc_Core_Opt*_tmp293;void**_tmp292;struct Cyc_Core_Opt*_tmp291;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp289)->tag == 1U){_LL16: _tmp293=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp289)->f1;_tmp292=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp289)->f2;_tmp291=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp289)->f4;_LL17: {
# 1098
void*_tmp28A=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp291);
# 1101
Cyc_Tcutil_occurs(_tmp28A,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp291))->v,r);{
void*_tmp28B=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp28C=_cycalloc(sizeof(*_tmp28C));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC48=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp28D;_tmp28D.tag=24U;({struct Cyc_List_List*_tmpC47=({void*_tmp28E[2U];({void*_tmpC46=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC45=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp290;_tmp290.tag=23U;_tmp290.f1=r;_tmp290;});_tmp28F[0]=_tmpC45;});_tmp28F;});_tmp28E[1U]=_tmpC46;});_tmp28E[0U]=_tmp28A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp28E,sizeof(void*),2U));});_tmp28D.f1=_tmpC47;});_tmp28D;});_tmp28C[0]=_tmpC48;});_tmp28C;}));
*_tmp292=_tmp28B;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{_LL13: _tmp295=_tmp287;_LL14:
# 1107
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp295);}_LL10:;}case 1U: _LL7: _tmp2A5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->f1;_tmp2A4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->f2;_tmp2A3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->f4;_LL8:
# 1110
 if(_tmp2A5 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2A5->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp296=0U;({struct _dyneither_ptr _tmpC49=({const char*_tmp297="effect evar has wrong kind";_tag_dyneither(_tmp297,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC49,_tag_dyneither(_tmp296,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1115
void*_tmp298=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A3);
# 1118
Cyc_Tcutil_occurs(_tmp298,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A3))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp299=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp29A=_cycalloc(sizeof(*_tmp29A));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC4E=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp29B;_tmp29B.tag=24U;({struct Cyc_List_List*_tmpC4D=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->hd=_tmp298;({struct Cyc_List_List*_tmpC4C=({struct Cyc_List_List*_tmp29D=_cycalloc(sizeof(*_tmp29D));({void*_tmpC4B=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp29E=_cycalloc(sizeof(*_tmp29E));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC4A=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp29F;_tmp29F.tag=23U;_tmp29F.f1=r;_tmp29F;});_tmp29E[0]=_tmpC4A;});_tmp29E;});_tmp29D->hd=_tmpC4B;});_tmp29D->tl=0;_tmp29D;});_tmp29C->tl=_tmpC4C;});_tmp29C;});_tmp29B.f1=_tmpC4D;});_tmp29B;});_tmp29A[0]=_tmpC4E;});_tmp29A;});
*_tmp2A4=(void*)_tmp299;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1129
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
({void*_tmpC4F=Cyc_Tcutil_compress(t);t=_tmpC4F;});{
void*_tmp2A6=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2A7=_tmp2A6;struct Cyc_Core_Opt*_tmp2BF;void**_tmp2BE;struct Cyc_Core_Opt*_tmp2BD;void*_tmp2BC;struct Cyc_List_List*_tmp2BB;switch(*((int*)_tmp2A7)){case 23U: _LL1: _LL2:
 return 0;case 24U: _LL3: _tmp2BB=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2A7)->f1;_LL4:
# 1134
 for(0;_tmp2BB != 0;_tmp2BB=_tmp2BB->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2BB->hd))
return 1;}
return 0;case 25U: _LL5: _tmp2BC=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A7)->f1;_LL6:
# 1139
({void*_tmpC50=Cyc_Tcutil_compress(_tmp2BC);_tmp2BC=_tmpC50;});
if(t == _tmp2BC)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2BC);{
void*_tmp2A8=Cyc_Tcutil_rgns_of(t);void*_tmp2A9=_tmp2A8;void*_tmp2B0;void*_tmp2AF;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A9)->tag == 25U){_LLC: _tmp2AF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A9)->f1;_LLD: {
# 1144
struct _tuple0 _tmp2AA=({struct _tuple0 _tmp2AE;({void*_tmpC51=Cyc_Tcutil_compress(_tmp2AF);_tmp2AE.f1=_tmpC51;});_tmp2AE.f2=_tmp2BC;_tmp2AE;});struct _tuple0 _tmp2AB=_tmp2AA;struct Cyc_Absyn_Tvar*_tmp2AD;struct Cyc_Absyn_Tvar*_tmp2AC;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AB.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AB.f2)->tag == 2U){_LL11: _tmp2AD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AB.f1)->f1;_tmp2AC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AB.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp2BC);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp2AF == _tmp2BC;}_LL10:;}}else{_LLE: _tmp2B0=_tmp2A9;_LLF:
# 1148
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2B0);}_LLB:;};case 1U: _LL7: _tmp2BF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A7)->f1;_tmp2BE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A7)->f2;_tmp2BD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A7)->f4;_LL8:
# 1151
 if(_tmp2BF == 0  || ((struct Cyc_Absyn_Kind*)_tmp2BF->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2B1=0U;({struct _dyneither_ptr _tmpC52=({const char*_tmp2B2="effect evar has wrong kind";_tag_dyneither(_tmp2B2,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC52,_tag_dyneither(_tmp2B1,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1156
void*_tmp2B3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2BD);
# 1159
Cyc_Tcutil_occurs(_tmp2B3,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BD))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B4=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC57=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2B6;_tmp2B6.tag=24U;({struct Cyc_List_List*_tmpC56=({struct Cyc_List_List*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->hd=_tmp2B3;({struct Cyc_List_List*_tmpC55=({struct Cyc_List_List*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));({void*_tmpC54=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC53=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2BA;_tmp2BA.tag=25U;_tmp2BA.f1=t;_tmp2BA;});_tmp2B9[0]=_tmpC53;});_tmp2B9;});_tmp2B8->hd=_tmpC54;});_tmp2B8->tl=0;_tmp2B8;});_tmp2B7->tl=_tmpC55;});_tmp2B7;});_tmp2B6.f1=_tmpC56;});_tmp2B6;});_tmp2B5[0]=_tmpC57;});_tmp2B5;});
*_tmp2BE=(void*)_tmp2B4;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1170
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
({void*_tmpC58=Cyc_Tcutil_compress(e);e=_tmpC58;});{
void*_tmp2C0=e;struct Cyc_Core_Opt*_tmp2E0;void**_tmp2DF;struct Cyc_Core_Opt*_tmp2DE;void*_tmp2DD;struct Cyc_List_List*_tmp2DC;struct Cyc_Absyn_Tvar*_tmp2DB;switch(*((int*)_tmp2C0)){case 2U: _LL1: _tmp2DB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C0)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp2DB)== 0;case 24U: _LL3: _tmp2DC=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2C0)->f1;_LL4:
# 1175
 for(0;_tmp2DC != 0;_tmp2DC=_tmp2DC->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2DC->hd))
return 1;}
return 0;case 25U: _LL5: _tmp2DD=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C0)->f1;_LL6: {
# 1180
void*_tmp2C1=Cyc_Tcutil_rgns_of(_tmp2DD);void*_tmp2C2=_tmp2C1;void*_tmp2D0;void*_tmp2CF;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C2)->tag == 25U){_LLC: _tmp2CF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C2)->f1;_LLD:
# 1182
 if(!may_constrain_evars)return 0;{
void*_tmp2C3=Cyc_Tcutil_compress(_tmp2CF);void*_tmp2C4=_tmp2C3;struct Cyc_Core_Opt*_tmp2CE;void**_tmp2CD;struct Cyc_Core_Opt*_tmp2CC;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C4)->tag == 1U){_LL11: _tmp2CE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C4)->f1;_tmp2CD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C4)->f2;_tmp2CC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C4)->f4;_LL12: {
# 1188
void*_tmp2C5=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2CC);
# 1190
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2CC))->v,v))return 0;{
void*_tmp2C6=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC5C=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2C8;_tmp2C8.tag=24U;({struct Cyc_List_List*_tmpC5B=({void*_tmp2C9[2U];({void*_tmpC5A=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC59=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2CB;_tmp2CB.tag=2U;_tmp2CB.f1=v;_tmp2CB;});_tmp2CA[0]=_tmpC59;});_tmp2CA;});_tmp2C9[1U]=_tmpC5A;});_tmp2C9[0U]=_tmp2C5;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2C9,sizeof(void*),2U));});_tmp2C8.f1=_tmpC5B;});_tmp2C8;});_tmp2C7[0]=_tmpC5C;});_tmp2C7;}));
*_tmp2CD=_tmp2C6;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{_LLE: _tmp2D0=_tmp2C2;_LLF:
# 1196
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2D0);}_LLB:;}case 1U: _LL7: _tmp2E0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C0)->f1;_tmp2DF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C0)->f2;_tmp2DE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C0)->f4;_LL8:
# 1199
 if(_tmp2E0 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2E0->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2D1=0U;({struct _dyneither_ptr _tmpC5D=({const char*_tmp2D2="effect evar has wrong kind";_tag_dyneither(_tmp2D2,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5D,_tag_dyneither(_tmp2D1,sizeof(void*),0U));});});{
# 1203
void*_tmp2D3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2DE);
# 1205
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DE))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D4=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC62=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2D6;_tmp2D6.tag=24U;({struct Cyc_List_List*_tmpC61=({struct Cyc_List_List*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->hd=_tmp2D3;({struct Cyc_List_List*_tmpC60=({struct Cyc_List_List*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));({void*_tmpC5F=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC5E=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2DA;_tmp2DA.tag=2U;_tmp2DA.f1=v;_tmp2DA;});_tmp2D9[0]=_tmpC5E;});_tmp2D9;});_tmp2D8->hd=_tmpC5F;});_tmp2D8->tl=0;_tmp2D8;});_tmp2D7->tl=_tmpC60;});_tmp2D7;});_tmp2D6.f1=_tmpC61;});_tmp2D6;});_tmp2D5[0]=_tmpC62;});_tmp2D5;});
*_tmp2DF=(void*)_tmp2D4;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1215
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
({void*_tmpC63=Cyc_Tcutil_compress(e);e=_tmpC63;});{
void*_tmp2E1=e;void*_tmp2E7;struct Cyc_List_List*_tmp2E6;switch(*((int*)_tmp2E1)){case 24U: _LL1: _tmp2E6=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E1)->f1;_LL2:
# 1219
 for(0;_tmp2E6 != 0;_tmp2E6=_tmp2E6->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2E6->hd))
return 1;}
return 0;case 25U: _LL3: _tmp2E7=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E1)->f1;_LL4: {
# 1224
void*_tmp2E2=Cyc_Tcutil_rgns_of(_tmp2E7);void*_tmp2E3=_tmp2E2;void*_tmp2E5;void*_tmp2E4;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E3)->tag == 25U){_LLA: _tmp2E4=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E3)->f1;_LLB:
 return 0;}else{_LLC: _tmp2E5=_tmp2E3;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2E5);}_LL9:;}case 1U: _LL5: _LL6:
# 1228
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1242 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1247
void*_tmp2E8=Cyc_Tcutil_compress(e1);void*_tmp2E9=_tmp2E8;void**_tmp2F6;struct Cyc_Core_Opt*_tmp2F5;void*_tmp2F4;struct Cyc_Absyn_Tvar*_tmp2F3;void*_tmp2F2;struct Cyc_List_List*_tmp2F1;switch(*((int*)_tmp2E9)){case 24U: _LL1: _tmp2F1=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E9)->f1;_LL2:
# 1249
 for(0;_tmp2F1 != 0;_tmp2F1=_tmp2F1->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp2F1->hd,e2))
return 0;}
return 1;case 23U: _LL3: _tmp2F2=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2E9)->f1;_LL4:
# 1260
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp2F2,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2F2,(void*)& Cyc_Absyn_HeapRgn_val);case 2U: _LL5: _tmp2F3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2E9)->f1;_LL6:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp2F3,e2);case 25U: _LL7: _tmp2F4=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E9)->f1;_LL8: {
# 1264
void*_tmp2EA=Cyc_Tcutil_rgns_of(_tmp2F4);void*_tmp2EB=_tmp2EA;void*_tmp2ED;void*_tmp2EC;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2EB)->tag == 25U){_LLE: _tmp2EC=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2EB)->f1;_LLF:
# 1269
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp2EC,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2EC,Cyc_Absyn_sint_typ);}else{_LL10: _tmp2ED=_tmp2EB;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp2ED,e2);}_LLD:;}case 1U: _LL9: _tmp2F6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E9)->f2;_tmp2F5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2E9)->f4;_LLA:
# 1274
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1278
*_tmp2F6=Cyc_Absyn_empty_effect;
# 1281
return 1;}else{
# 1283
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp2F0;_tmp2F0.tag=0U;({struct _dyneither_ptr _tmpC64=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmp2F0.f1=_tmpC64;});({void*_tmp2EE[1U]={& _tmp2F0};({struct _dyneither_ptr _tmpC65=({const char*_tmp2EF="subset_effect: bad effect: %s";_tag_dyneither(_tmp2EF,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC65,_tag_dyneither(_tmp2EE,sizeof(void*),1U));});});});}_LL0:;}
# 1299 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
({void*_tmpC66=Cyc_Tcutil_normalize_effect(e1);e1=_tmpC66;});
({void*_tmpC67=Cyc_Tcutil_normalize_effect(e2);e2=_tmpC67;});
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1315
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1317
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp2F7=(struct _tuple0*)r1->hd;struct _tuple0*_tmp2F8=_tmp2F7;void*_tmp2FE;void*_tmp2FD;_LL1: _tmp2FE=_tmp2F8->f1;_tmp2FD=_tmp2F8->f2;_LL2:;{
int found=_tmp2FE == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp2F9=(struct _tuple0*)r2->hd;struct _tuple0*_tmp2FA=_tmp2F9;void*_tmp2FC;void*_tmp2FB;_LL4: _tmp2FC=_tmp2FA->f1;_tmp2FB=_tmp2FA->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp2FE,_tmp2FC) && Cyc_Tcutil_unify(_tmp2FD,_tmp2FB)){
found=1;
break;}}}
# 1327
if(!found)return 0;};}}
# 1329
return 1;}
# 1336
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp2FF=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp300=({struct Cyc_List_List*_tmp301=_cycalloc(sizeof(*_tmp301));_tmp301->hd=_tmp2FF;_tmp301->tl=r1;_tmp301;});
if(Cyc_Relations_consistent_relations(_tmp300))return 0;}
# 1342
return 1;}
# 1347
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1353
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1355
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1359
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC68=_tag_dyneither(0,0,0);Cyc_Tcutil_failure_reason=_tmpC68;});
({void*_tmpC69=Cyc_Tcutil_compress(t1);t1=_tmpC69;});
({void*_tmpC6A=Cyc_Tcutil_compress(t2);t2=_tmpC6A;});
if(t1 == t2)return;
{void*_tmp302=t1;struct Cyc_Core_Opt*_tmp311;void**_tmp310;struct Cyc_Core_Opt*_tmp30F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp302)->tag == 1U){_LL1: _tmp311=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp302)->f1;_tmp310=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp302)->f2;_tmp30F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp302)->f4;_LL2:
# 1370
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp30F))->v,t2);{
struct Cyc_Absyn_Kind*_tmp303=Cyc_Tcutil_typ_kind(t2);
# 1375
if(Cyc_Tcutil_kind_leq(_tmp303,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp311))->v)){
*_tmp310=t2;
return;}else{
# 1379
{void*_tmp304=t2;struct Cyc_Absyn_PtrInfo _tmp30D;void**_tmp30C;struct Cyc_Core_Opt*_tmp30B;switch(*((int*)_tmp304)){case 1U: _LL6: _tmp30C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp304)->f2;_tmp30B=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp304)->f4;_LL7: {
# 1382
struct Cyc_List_List*_tmp305=(struct Cyc_List_List*)_tmp30F->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp30B))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp305,(struct Cyc_Absyn_Tvar*)s2->hd)){
({struct _dyneither_ptr _tmpC6B=({const char*_tmp306="(type variable would escape scope)";_tag_dyneither(_tmp306,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpC6B;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1389
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp311->v,_tmp303)){
*_tmp30C=t1;return;}
# 1392
({struct _dyneither_ptr _tmpC6C=({const char*_tmp307="(kinds are incompatible)";_tag_dyneither(_tmp307,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC6C;});
goto _LL5;}case 5U: _LL8: _tmp30D=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1;if(((struct Cyc_Absyn_Kind*)_tmp311->v)->kind == Cyc_Absyn_BoxKind){_LL9: {
# 1398
union Cyc_Absyn_Constraint*_tmp308=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp30D.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp309=_tmp308;if((((union Cyc_Absyn_Constraint*)_tmp309)->No_constr).tag == 3){_LLD: _LLE:
# 1402
({struct _union_Constraint_Eq_constr*_tmp30A=& _tmp308->Eq_constr;_tmp30A->tag=1;_tmp30A->val=Cyc_Absyn_bounds_one;});
*_tmp310=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1407
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1410
({struct _dyneither_ptr _tmpC6D=({const char*_tmp30E="(kinds are incompatible)";_tag_dyneither(_tmp30E,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC6D;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1413
 goto _LL0;}_LL0:;}
# 1418
{struct _tuple0 _tmp312=({struct _tuple0 _tmp3D7;_tmp3D7.f1=t2;_tmp3D7.f2=t1;_tmp3D7;});struct _tuple0 _tmp313=_tmp312;void*_tmp3D6;void*_tmp3D5;void*_tmp3D4;void*_tmp3D3;void*_tmp3D2;void*_tmp3D1;struct Cyc_List_List*_tmp3D0;struct Cyc_Absyn_Typedefdecl*_tmp3CF;struct Cyc_List_List*_tmp3CE;struct Cyc_Absyn_Typedefdecl*_tmp3CD;enum Cyc_Absyn_AggrKind _tmp3CC;struct Cyc_List_List*_tmp3CB;enum Cyc_Absyn_AggrKind _tmp3CA;struct Cyc_List_List*_tmp3C9;struct Cyc_List_List*_tmp3C8;struct Cyc_List_List*_tmp3C7;struct Cyc_List_List*_tmp3C6;void*_tmp3C5;struct Cyc_Absyn_Tqual _tmp3C4;void*_tmp3C3;struct Cyc_List_List*_tmp3C2;int _tmp3C1;struct Cyc_Absyn_VarargInfo*_tmp3C0;struct Cyc_List_List*_tmp3BF;struct Cyc_List_List*_tmp3BE;struct Cyc_Absyn_Exp*_tmp3BD;struct Cyc_List_List*_tmp3BC;struct Cyc_Absyn_Exp*_tmp3BB;struct Cyc_List_List*_tmp3BA;struct Cyc_List_List*_tmp3B9;void*_tmp3B8;struct Cyc_Absyn_Tqual _tmp3B7;void*_tmp3B6;struct Cyc_List_List*_tmp3B5;int _tmp3B4;struct Cyc_Absyn_VarargInfo*_tmp3B3;struct Cyc_List_List*_tmp3B2;struct Cyc_List_List*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_List_List*_tmp3AF;struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_List_List*_tmp3AD;void*_tmp3AC;struct Cyc_Absyn_Tqual _tmp3AB;struct Cyc_Absyn_Exp*_tmp3AA;union Cyc_Absyn_Constraint*_tmp3A9;void*_tmp3A8;struct Cyc_Absyn_Tqual _tmp3A7;struct Cyc_Absyn_Exp*_tmp3A6;union Cyc_Absyn_Constraint*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A3;void*_tmp3A2;void*_tmp3A1;int _tmp3A0;int _tmp39F;enum Cyc_Absyn_Sign _tmp39E;enum Cyc_Absyn_Size_of _tmp39D;enum Cyc_Absyn_Sign _tmp39C;enum Cyc_Absyn_Size_of _tmp39B;void*_tmp39A;struct Cyc_Absyn_Tqual _tmp399;void*_tmp398;union Cyc_Absyn_Constraint*_tmp397;union Cyc_Absyn_Constraint*_tmp396;union Cyc_Absyn_Constraint*_tmp395;void*_tmp394;struct Cyc_Absyn_Tqual _tmp393;void*_tmp392;union Cyc_Absyn_Constraint*_tmp391;union Cyc_Absyn_Constraint*_tmp390;union Cyc_Absyn_Constraint*_tmp38F;struct Cyc_Absyn_Datatypedecl*_tmp38E;struct Cyc_Absyn_Datatypefield*_tmp38D;struct Cyc_List_List*_tmp38C;struct Cyc_Absyn_Datatypedecl*_tmp38B;struct Cyc_Absyn_Datatypefield*_tmp38A;struct Cyc_List_List*_tmp389;struct Cyc_Absyn_Datatypedecl*_tmp388;struct Cyc_List_List*_tmp387;struct Cyc_Absyn_Datatypedecl*_tmp386;struct Cyc_List_List*_tmp385;struct Cyc_List_List*_tmp384;struct Cyc_List_List*_tmp383;struct _tuple2*_tmp382;struct _tuple2*_tmp381;union Cyc_Absyn_AggrInfoU _tmp380;struct Cyc_List_List*_tmp37F;union Cyc_Absyn_AggrInfoU _tmp37E;struct Cyc_List_List*_tmp37D;struct Cyc_Absyn_Tvar*_tmp37C;struct Cyc_Absyn_Tvar*_tmp37B;switch(*((int*)_tmp313.f1)){case 1U: _LL12: _LL13:
# 1421
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0U: switch(*((int*)_tmp313.f2)){case 0U: _LL14: _LL15:
# 1424
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 2U: switch(*((int*)_tmp313.f2)){case 2U: _LL16: _tmp37C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp37B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL17: {
# 1427
struct _dyneither_ptr*_tmp314=_tmp37C->name;
struct _dyneither_ptr*_tmp315=_tmp37B->name;
# 1430
int _tmp316=_tmp37C->identity;
int _tmp317=_tmp37B->identity;
if(_tmp317 == _tmp316)return;
({struct _dyneither_ptr _tmpC6E=({const char*_tmp318="(variable types are not the same)";_tag_dyneither(_tmp318,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpC6E;});
goto _LL11;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 11U: switch(*((int*)_tmp313.f2)){case 11U: _LL18: _tmp380=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313.f1)->f1).aggr_info;_tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313.f1)->f1).targs;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313.f2)->f1).aggr_info;_tmp37D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313.f2)->f1).targs;_LL19: {
# 1437
struct _tuple13 _tmp319=Cyc_Absyn_aggr_kinded_name(_tmp37E);struct _tuple13 _tmp31A=_tmp319;enum Cyc_Absyn_AggrKind _tmp322;struct _tuple2*_tmp321;_LL4F: _tmp322=_tmp31A.f1;_tmp321=_tmp31A.f2;_LL50:;{
struct _tuple13 _tmp31B=Cyc_Absyn_aggr_kinded_name(_tmp380);struct _tuple13 _tmp31C=_tmp31B;enum Cyc_Absyn_AggrKind _tmp320;struct _tuple2*_tmp31F;_LL52: _tmp320=_tmp31C.f1;_tmp31F=_tmp31C.f2;_LL53:;
if(_tmp322 != _tmp320){({struct _dyneither_ptr _tmpC6F=({const char*_tmp31D="(struct and union type)";_tag_dyneither(_tmp31D,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpC6F;});goto _LL11;}
if(Cyc_Absyn_qvar_cmp(_tmp321,_tmp31F)!= 0){({struct _dyneither_ptr _tmpC70=({const char*_tmp31E="(different type name)";_tag_dyneither(_tmp31E,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpC70;});goto _LL11;}
Cyc_Tcutil_unify_list(_tmp37D,_tmp37F);
return;};}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 13U: switch(*((int*)_tmp313.f2)){case 13U: _LL1A: _tmp382=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp381=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL1B:
# 1446
 if(Cyc_Absyn_qvar_cmp(_tmp382,_tmp381)== 0)return;
({struct _dyneither_ptr _tmpC71=({const char*_tmp323="(different enum types)";_tag_dyneither(_tmp323,sizeof(char),23U);});Cyc_Tcutil_failure_reason=_tmpC71;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 14U: switch(*((int*)_tmp313.f2)){case 14U: _LL1C: _tmp384=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp383=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL1D: {
# 1451
int bad=0;
for(0;_tmp384 != 0  && _tmp383 != 0;(_tmp384=_tmp384->tl,_tmp383=_tmp383->tl)){
struct Cyc_Absyn_Enumfield*_tmp324=(struct Cyc_Absyn_Enumfield*)_tmp384->hd;
struct Cyc_Absyn_Enumfield*_tmp325=(struct Cyc_Absyn_Enumfield*)_tmp383->hd;
if(Cyc_Absyn_qvar_cmp(_tmp324->name,_tmp325->name)!= 0){
({struct _dyneither_ptr _tmpC72=({const char*_tmp326="(different names for enum fields)";_tag_dyneither(_tmp326,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpC72;});
bad=1;
break;}
# 1460
if(_tmp324->tag == _tmp325->tag)continue;
if(_tmp324->tag == 0  || _tmp325->tag == 0){
({struct _dyneither_ptr _tmpC73=({const char*_tmp327="(different tag values for enum fields)";_tag_dyneither(_tmp327,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC73;});
bad=1;
break;}
# 1466
if(!({struct Cyc_Absyn_Exp*_tmpC74=(struct Cyc_Absyn_Exp*)_check_null(_tmp324->tag);Cyc_Evexp_same_const_exp(_tmpC74,(struct Cyc_Absyn_Exp*)_check_null(_tmp325->tag));})){
({struct _dyneither_ptr _tmpC75=({const char*_tmp328="(different tag values for enum fields)";_tag_dyneither(_tmp328,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC75;});
bad=1;
break;}}
# 1472
if(bad)goto _LL11;
if(_tmp384 == 0  && _tmp383 == 0)return;
({struct _dyneither_ptr _tmpC76=({const char*_tmp329="(different number of fields for enums)";_tag_dyneither(_tmp329,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC76;});
goto _LL11;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp313.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp313.f2)){case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp313.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL1E: _tmp388=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp313.f1)->f1).datatype_info).KnownDatatype).val;_tmp387=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp313.f1)->f1).targs;_tmp386=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp313.f2)->f1).datatype_info).KnownDatatype).val;_tmp385=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp313.f2)->f1).targs;_LL1F:
# 1479
 if(_tmp388 == _tmp386  || Cyc_Absyn_qvar_cmp(_tmp388->name,_tmp386->name)== 0){
Cyc_Tcutil_unify_list(_tmp385,_tmp387);
return;}
# 1483
goto _LL11;}else{goto _LL4C;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}else{switch(*((int*)_tmp313.f2)){case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp313.f2)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL20: _tmp38E=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp38D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp38C=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313.f1)->f1).targs;_tmp38B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp38A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp389=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp313.f2)->f1).targs;_LL21:
# 1487
 if((_tmp38E == _tmp38B  || Cyc_Absyn_qvar_cmp(_tmp38E->name,_tmp38B->name)== 0) && (
_tmp38D == _tmp38A  || Cyc_Absyn_qvar_cmp(_tmp38D->name,_tmp38A->name)== 0)){
Cyc_Tcutil_unify_list(_tmp389,_tmp38C);
return;}
# 1492
({struct _dyneither_ptr _tmpC77=({const char*_tmp32A="(different datatype field types)";_tag_dyneither(_tmp32A,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpC77;});
goto _LL11;}else{goto _LL4C;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}else{switch(*((int*)_tmp313.f2)){case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}}case 5U: switch(*((int*)_tmp313.f2)){case 5U: _LL22: _tmp39A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).elt_typ;_tmp399=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).elt_tq;_tmp398=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).ptr_atts).rgn;_tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).ptr_atts).nullable;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).ptr_atts).bounds;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).ptr_atts).zero_term;_tmp394=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).elt_typ;_tmp393=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).elt_tq;_tmp392=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).ptr_atts).rgn;_tmp391=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).ptr_atts).nullable;_tmp390=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).ptr_atts).bounds;_tmp38F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).ptr_atts).zero_term;_LL23:
# 1497
 Cyc_Tcutil_unify_it(_tmp394,_tmp39A);
Cyc_Tcutil_unify_it(_tmp398,_tmp392);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpC79=_tmp38F;union Cyc_Absyn_Constraint*_tmpC78=_tmp395;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC79,_tmpC78,({const char*_tmp32B="(not both zero terminated)";_tag_dyneither(_tmp32B,sizeof(char),27U);}));});
Cyc_Tcutil_unify_tqual(_tmp393,_tmp394,_tmp399,_tmp39A);
({union Cyc_Absyn_Constraint*_tmpC7B=_tmp390;union Cyc_Absyn_Constraint*_tmpC7A=_tmp396;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmpC7B,_tmpC7A,({const char*_tmp32C="(different pointer bounds)";_tag_dyneither(_tmp32C,sizeof(char),27U);}));});
# 1505
{void*_tmp32D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp390);void*_tmp32E=_tmp32D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp32E)->tag == 0U){_LL55: _LL56:
 return;}else{_LL57: _LL58:
 goto _LL54;}_LL54:;}
# 1509
({union Cyc_Absyn_Constraint*_tmpC7D=_tmp391;union Cyc_Absyn_Constraint*_tmpC7C=_tmp397;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC7D,_tmpC7C,({const char*_tmp32F="(incompatible pointer types)";_tag_dyneither(_tmp32F,sizeof(char),29U);}));});
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 6U: switch(*((int*)_tmp313.f2)){case 6U: _LL24: _tmp39E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp39D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313.f1)->f2;_tmp39C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313.f2)->f1;_tmp39B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313.f2)->f2;_LL25:
# 1514
 if(_tmp39C == _tmp39E  && ((_tmp39B == _tmp39D  || _tmp39B == Cyc_Absyn_Int_sz  && _tmp39D == Cyc_Absyn_Long_sz) || 
_tmp39B == Cyc_Absyn_Long_sz  && _tmp39D == Cyc_Absyn_Int_sz))return;
({struct _dyneither_ptr _tmpC7E=({const char*_tmp330="(different integral types)";_tag_dyneither(_tmp330,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpC7E;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 7U: switch(*((int*)_tmp313.f2)){case 7U: _LL26: _tmp3A0=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp39F=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL27:
# 1520
 if(_tmp39F == 0  && _tmp3A0 == 0)return;else{
if(_tmp39F == 1  && _tmp3A0 == 1)return;else{
# 1523
if(((_tmp39F != 0  && _tmp39F != 1) && _tmp3A0 != 0) && _tmp3A0 != 1)return;}}
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 19U: switch(*((int*)_tmp313.f2)){case 19U: _LL28: _tmp3A2=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp3A1=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL29:
# 1526
 Cyc_Tcutil_unify_it(_tmp3A2,_tmp3A1);return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 18U: switch(*((int*)_tmp313.f2)){case 18U: _LL2A: _tmp3A4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp3A3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL2B:
# 1529
 if(!Cyc_Evexp_same_const_exp(_tmp3A4,_tmp3A3)){
({struct _dyneither_ptr _tmpC7F=({const char*_tmp331="(cannot prove expressions are the same)";_tag_dyneither(_tmp331,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpC7F;});
goto _LL11;}
# 1533
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 8U: switch(*((int*)_tmp313.f2)){case 8U: _LL2C: _tmp3AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f1)->f1).elt_type;_tmp3AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f1)->f1).tq;_tmp3AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f1)->f1).num_elts;_tmp3A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f1)->f1).zero_term;_tmp3A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f2)->f1).elt_type;_tmp3A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f2)->f1).tq;_tmp3A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f2)->f1).num_elts;_tmp3A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f2)->f1).zero_term;_LL2D:
# 1537
 Cyc_Tcutil_unify_it(_tmp3A8,_tmp3AC);
Cyc_Tcutil_unify_tqual(_tmp3A7,_tmp3A8,_tmp3AB,_tmp3AC);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpC81=_tmp3A9;union Cyc_Absyn_Constraint*_tmpC80=_tmp3A5;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC81,_tmpC80,({const char*_tmp332="(not both zero terminated)";_tag_dyneither(_tmp332,sizeof(char),27U);}));});
if(_tmp3AA == _tmp3A6)return;
if(_tmp3AA == 0  || _tmp3A6 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp3AA,_tmp3A6))
return;
({struct _dyneither_ptr _tmpC82=({const char*_tmp333="(different array sizes)";_tag_dyneither(_tmp333,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpC82;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 9U: switch(*((int*)_tmp313.f2)){case 9U: _LL2E: _tmp3C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).tvars;_tmp3C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).effect;_tmp3C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).ret_tqual;_tmp3C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).ret_typ;_tmp3C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).args;_tmp3C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).c_varargs;_tmp3C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).cyc_varargs;_tmp3BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).rgn_po;_tmp3BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).attributes;_tmp3BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).requires_clause;_tmp3BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).requires_relns;_tmp3BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).ensures_clause;_tmp3BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).ensures_relns;_tmp3B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).tvars;_tmp3B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).effect;_tmp3B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).ret_tqual;_tmp3B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).ret_typ;_tmp3B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).args;_tmp3B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).c_varargs;_tmp3B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).cyc_varargs;_tmp3B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).rgn_po;_tmp3B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).attributes;_tmp3B0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).requires_clause;_tmp3AF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).requires_relns;_tmp3AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).ensures_clause;_tmp3AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).ensures_relns;_LL2F: {
# 1551
int done=0;
{struct _RegionHandle _tmp334=_new_region("rgn");struct _RegionHandle*rgn=& _tmp334;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp3B9 != 0){
if(_tmp3C6 == 0){
({struct _dyneither_ptr _tmpC83=({const char*_tmp335="(second function type has too few type variables)";_tag_dyneither(_tmp335,sizeof(char),50U);});Cyc_Tcutil_failure_reason=_tmpC83;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1559
void*_tmp336=((struct Cyc_Absyn_Tvar*)_tmp3B9->hd)->kind;
void*_tmp337=((struct Cyc_Absyn_Tvar*)_tmp3C6->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp336,_tmp337)){
({struct _dyneither_ptr _tmpC88=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33C;_tmp33C.tag=0U;({struct _dyneither_ptr _tmpC84=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1565
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3C6->hd,& Cyc_Tcutil_bk)));_tmp33C.f1=_tmpC84;});({struct Cyc_String_pa_PrintArg_struct _tmp33B;_tmp33B.tag=0U;({struct _dyneither_ptr _tmpC85=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1564
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3B9->hd,& Cyc_Tcutil_bk)));_tmp33B.f1=_tmpC85;});({struct Cyc_String_pa_PrintArg_struct _tmp33A;_tmp33A.tag=0U;({struct _dyneither_ptr _tmpC86=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1563
Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp3B9->hd));_tmp33A.f1=_tmpC86;});({void*_tmp338[3U]={& _tmp33A,& _tmp33B,& _tmp33C};({struct _dyneither_ptr _tmpC87=({const char*_tmp339="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp339,sizeof(char),44U);});Cyc_aprintf(_tmpC87,_tag_dyneither(_tmp338,sizeof(void*),3U));});});});});});
# 1562
Cyc_Tcutil_failure_reason=_tmpC88;});
# 1566
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1568
({struct Cyc_List_List*_tmpC8C=({struct Cyc_List_List*_tmp33D=_region_malloc(rgn,sizeof(*_tmp33D));({struct _tuple16*_tmpC8B=({struct _tuple16*_tmp33E=_region_malloc(rgn,sizeof(*_tmp33E));_tmp33E->f1=(struct Cyc_Absyn_Tvar*)_tmp3C6->hd;({void*_tmpC8A=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33F=_cycalloc(sizeof(*_tmp33F));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC89=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp340;_tmp340.tag=2U;_tmp340.f1=(struct Cyc_Absyn_Tvar*)_tmp3B9->hd;_tmp340;});_tmp33F[0]=_tmpC89;});_tmp33F;});_tmp33E->f2=_tmpC8A;});_tmp33E;});_tmp33D->hd=_tmpC8B;});_tmp33D->tl=inst;_tmp33D;});inst=_tmpC8C;});
_tmp3B9=_tmp3B9->tl;
_tmp3C6=_tmp3C6->tl;};}
# 1572
if(_tmp3C6 != 0){
({struct _dyneither_ptr _tmpC8D=({const char*_tmp341="(second function type has too many type variables)";_tag_dyneither(_tmp341,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpC8D;});
_npop_handler(0U);goto _LL11;}
# 1576
if(inst != 0){
({void*_tmpC94=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp342=_cycalloc(sizeof(*_tmp342));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC8F=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp343;_tmp343.tag=9U;({struct Cyc_Absyn_FnInfo _tmpC8E=({struct Cyc_Absyn_FnInfo _tmp344;_tmp344.tvars=0;_tmp344.effect=_tmp3B8;_tmp344.ret_tqual=_tmp3B7;_tmp344.ret_typ=_tmp3B6;_tmp344.args=_tmp3B5;_tmp344.c_varargs=_tmp3B4;_tmp344.cyc_varargs=_tmp3B3;_tmp344.rgn_po=_tmp3B2;_tmp344.attributes=_tmp3B1;_tmp344.requires_clause=_tmp3BD;_tmp344.requires_relns=_tmp3BC;_tmp344.ensures_clause=_tmp3BB;_tmp344.ensures_relns=_tmp3BA;_tmp344;});_tmp343.f1=_tmpC8E;});_tmp343;});_tmp342[0]=_tmpC8F;});_tmp342;});Cyc_Tcutil_unify_it(_tmpC94,({
# 1580
struct _RegionHandle*_tmpC93=rgn;struct Cyc_List_List*_tmpC92=inst;Cyc_Tcutil_rsubstitute(_tmpC93,_tmpC92,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp345=_cycalloc(sizeof(*_tmp345));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC91=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp346;_tmp346.tag=9U;({struct Cyc_Absyn_FnInfo _tmpC90=({struct Cyc_Absyn_FnInfo _tmp347;_tmp347.tvars=0;_tmp347.effect=_tmp3C5;_tmp347.ret_tqual=_tmp3C4;_tmp347.ret_typ=_tmp3C3;_tmp347.args=_tmp3C2;_tmp347.c_varargs=_tmp3C1;_tmp347.cyc_varargs=_tmp3C0;_tmp347.rgn_po=_tmp3BF;_tmp347.attributes=_tmp3BE;_tmp347.requires_clause=_tmp3B0;_tmp347.requires_relns=_tmp3AF;_tmp347.ensures_clause=_tmp3AE;_tmp347.ensures_relns=_tmp3AD;_tmp347;});_tmp346.f1=_tmpC90;});_tmp346;});_tmp345[0]=_tmpC91;});_tmp345;}));}));});
# 1585
done=1;}}
# 1553
;_pop_region(rgn);}
# 1588
if(done)
return;
Cyc_Tcutil_unify_it(_tmp3B6,_tmp3C3);
Cyc_Tcutil_unify_tqual(_tmp3B7,_tmp3B6,_tmp3C4,_tmp3C3);
for(0;_tmp3B5 != 0  && _tmp3C2 != 0;(_tmp3B5=_tmp3B5->tl,_tmp3C2=_tmp3C2->tl)){
struct _tuple10 _tmp348=*((struct _tuple10*)_tmp3B5->hd);struct _tuple10 _tmp349=_tmp348;struct Cyc_Absyn_Tqual _tmp34F;void*_tmp34E;_LL5A: _tmp34F=_tmp349.f2;_tmp34E=_tmp349.f3;_LL5B:;{
struct _tuple10 _tmp34A=*((struct _tuple10*)_tmp3C2->hd);struct _tuple10 _tmp34B=_tmp34A;struct Cyc_Absyn_Tqual _tmp34D;void*_tmp34C;_LL5D: _tmp34D=_tmp34B.f2;_tmp34C=_tmp34B.f3;_LL5E:;
Cyc_Tcutil_unify_it(_tmp34E,_tmp34C);
Cyc_Tcutil_unify_tqual(_tmp34F,_tmp34E,_tmp34D,_tmp34C);};}
# 1598
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp3B5 != 0  || _tmp3C2 != 0){
({struct _dyneither_ptr _tmpC95=({const char*_tmp350="(function types have different number of arguments)";_tag_dyneither(_tmp350,sizeof(char),52U);});Cyc_Tcutil_failure_reason=_tmpC95;});
goto _LL11;}
# 1604
if(_tmp3B4 != _tmp3C1){
({struct _dyneither_ptr _tmpC96=({const char*_tmp351="(only one function type takes C varargs)";_tag_dyneither(_tmp351,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpC96;});
goto _LL11;}{
# 1609
int bad_cyc_vararg=0;
{struct _tuple22 _tmp352=({struct _tuple22 _tmp35E;_tmp35E.f1=_tmp3B3;_tmp35E.f2=_tmp3C0;_tmp35E;});struct _tuple22 _tmp353=_tmp352;struct _dyneither_ptr*_tmp35D;struct Cyc_Absyn_Tqual _tmp35C;void*_tmp35B;int _tmp35A;struct _dyneither_ptr*_tmp359;struct Cyc_Absyn_Tqual _tmp358;void*_tmp357;int _tmp356;if(_tmp353.f1 == 0){if(_tmp353.f2 == 0){_LL60: _LL61:
 goto _LL5F;}else{_LL62: _LL63:
 goto _LL65;}}else{if(_tmp353.f2 == 0){_LL64: _LL65:
# 1614
 bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC97=({const char*_tmp354="(only one function type takes varargs)";_tag_dyneither(_tmp354,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC97;});
goto _LL5F;}else{_LL66: _tmp35D=(_tmp353.f1)->name;_tmp35C=(_tmp353.f1)->tq;_tmp35B=(_tmp353.f1)->type;_tmp35A=(_tmp353.f1)->inject;_tmp359=(_tmp353.f2)->name;_tmp358=(_tmp353.f2)->tq;_tmp357=(_tmp353.f2)->type;_tmp356=(_tmp353.f2)->inject;_LL67:
# 1618
 Cyc_Tcutil_unify_it(_tmp35B,_tmp357);
Cyc_Tcutil_unify_tqual(_tmp35C,_tmp35B,_tmp358,_tmp357);
if(_tmp35A != _tmp356){
bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC98=({const char*_tmp355="(only one function type injects varargs)";_tag_dyneither(_tmp355,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpC98;});}
# 1624
goto _LL5F;}}_LL5F:;}
# 1626
if(bad_cyc_vararg)goto _LL11;{
# 1629
int bad_effect=0;
{struct _tuple0 _tmp35F=({struct _tuple0 _tmp361;_tmp361.f1=_tmp3B8;_tmp361.f2=_tmp3C5;_tmp361;});struct _tuple0 _tmp360=_tmp35F;if(_tmp360.f1 == 0){if(_tmp360.f2 == 0){_LL69: _LL6A:
 goto _LL68;}else{_LL6B: _LL6C:
 goto _LL6E;}}else{if(_tmp360.f2 == 0){_LL6D: _LL6E:
 bad_effect=1;goto _LL68;}else{_LL6F: _LL70:
({int _tmpC9A=!({void*_tmpC99=(void*)_check_null(_tmp3B8);Cyc_Tcutil_unify_effect(_tmpC99,(void*)_check_null(_tmp3C5));});bad_effect=_tmpC9A;});goto _LL68;}}_LL68:;}
# 1636
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
({struct _dyneither_ptr _tmpC9B=({const char*_tmp362="(function effects do not match)";_tag_dyneither(_tmp362,sizeof(char),32U);});Cyc_Tcutil_failure_reason=_tmpC9B;});
goto _LL11;}
# 1642
if(!Cyc_Tcutil_same_atts(_tmp3BE,_tmp3B1)){
({struct _dyneither_ptr _tmpC9C=({const char*_tmp363="(function types have different attributes)";_tag_dyneither(_tmp363,sizeof(char),43U);});Cyc_Tcutil_failure_reason=_tmpC9C;});
goto _LL11;}
# 1646
if(!Cyc_Tcutil_same_rgn_po(_tmp3BF,_tmp3B2)){
({struct _dyneither_ptr _tmpC9D=({const char*_tmp364="(function types have different region lifetime orderings)";_tag_dyneither(_tmp364,sizeof(char),58U);});Cyc_Tcutil_failure_reason=_tmpC9D;});
goto _LL11;}
# 1650
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3BC,_tmp3AF)){
({struct _dyneither_ptr _tmpC9E=({const char*_tmp365="(@requires clauses not equivalent)";_tag_dyneither(_tmp365,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpC9E;});
goto _LL11;}
# 1654
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3BA,_tmp3AD)){
({struct _dyneither_ptr _tmpC9F=({const char*_tmp366="(@ensures clauses not equivalent)";_tag_dyneither(_tmp366,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpC9F;});
goto _LL11;}
# 1658
return;};};}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 10U: switch(*((int*)_tmp313.f2)){case 10U: _LL30: _tmp3C8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp3C7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL31:
# 1661
 for(0;_tmp3C7 != 0  && _tmp3C8 != 0;(_tmp3C7=_tmp3C7->tl,_tmp3C8=_tmp3C8->tl)){
struct _tuple12 _tmp367=*((struct _tuple12*)_tmp3C7->hd);struct _tuple12 _tmp368=_tmp367;struct Cyc_Absyn_Tqual _tmp36E;void*_tmp36D;_LL72: _tmp36E=_tmp368.f1;_tmp36D=_tmp368.f2;_LL73:;{
struct _tuple12 _tmp369=*((struct _tuple12*)_tmp3C8->hd);struct _tuple12 _tmp36A=_tmp369;struct Cyc_Absyn_Tqual _tmp36C;void*_tmp36B;_LL75: _tmp36C=_tmp36A.f1;_tmp36B=_tmp36A.f2;_LL76:;
Cyc_Tcutil_unify_it(_tmp36D,_tmp36B);
Cyc_Tcutil_unify_tqual(_tmp36E,_tmp36D,_tmp36C,_tmp36B);};}
# 1667
if(_tmp3C7 == 0  && _tmp3C8 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpCA0=({const char*_tmp36F="(tuple types have different numbers of components)";_tag_dyneither(_tmp36F,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpCA0;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 12U: switch(*((int*)_tmp313.f2)){case 12U: _LL32: _tmp3CC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp3CB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313.f1)->f2;_tmp3CA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313.f2)->f1;_tmp3C9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313.f2)->f2;_LL33:
# 1674
 if(_tmp3CA != _tmp3CC){({struct _dyneither_ptr _tmpCA1=({const char*_tmp370="(struct and union type)";_tag_dyneither(_tmp370,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpCA1;});goto _LL11;}
for(0;_tmp3C9 != 0  && _tmp3CB != 0;(_tmp3C9=_tmp3C9->tl,_tmp3CB=_tmp3CB->tl)){
struct Cyc_Absyn_Aggrfield*_tmp371=(struct Cyc_Absyn_Aggrfield*)_tmp3C9->hd;
struct Cyc_Absyn_Aggrfield*_tmp372=(struct Cyc_Absyn_Aggrfield*)_tmp3CB->hd;
if(Cyc_strptrcmp(_tmp371->name,_tmp372->name)!= 0){
({struct _dyneither_ptr _tmpCA2=({const char*_tmp373="(different member names)";_tag_dyneither(_tmp373,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpCA2;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1682
Cyc_Tcutil_unify_it(_tmp371->type,_tmp372->type);
Cyc_Tcutil_unify_tqual(_tmp371->tq,_tmp371->type,_tmp372->tq,_tmp372->type);
if(!Cyc_Tcutil_same_atts(_tmp371->attributes,_tmp372->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpCA3=({const char*_tmp374="(different attributes on member)";_tag_dyneither(_tmp374,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpCA3;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1690
if((_tmp371->width != 0  && _tmp372->width == 0  || 
_tmp372->width != 0  && _tmp371->width == 0) || 
(_tmp371->width != 0  && _tmp372->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpCA4=(struct Cyc_Absyn_Exp*)_check_null(_tmp371->width);Cyc_Evexp_same_const_exp(_tmpCA4,(struct Cyc_Absyn_Exp*)_check_null(_tmp372->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpCA5=({const char*_tmp375="(different bitfield widths on member)";_tag_dyneither(_tmp375,sizeof(char),38U);});Cyc_Tcutil_failure_reason=_tmpCA5;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1699
if((_tmp371->requires_clause != 0  && _tmp372->requires_clause == 0  || 
_tmp371->requires_clause == 0  && _tmp372->requires_clause != 0) || 
(_tmp371->requires_clause == 0  && _tmp372->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpCA6=(struct Cyc_Absyn_Exp*)_check_null(_tmp371->requires_clause);Cyc_Evexp_same_const_exp(_tmpCA6,(struct Cyc_Absyn_Exp*)_check_null(_tmp372->requires_clause));})){
# 1704
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpCA7=({const char*_tmp376="(different @requires clauses on member)";_tag_dyneither(_tmp376,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpCA7;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1710
if(_tmp3C9 == 0  && _tmp3CB == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpCA8=({const char*_tmp377="(different number of members)";_tag_dyneither(_tmp377,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpCA8;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 17U: switch(*((int*)_tmp313.f2)){case 17U: _LL34: _tmp3D0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313.f1)->f2;_tmp3CF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313.f1)->f3;_tmp3CE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313.f2)->f2;_tmp3CD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313.f2)->f3;_LL35:
# 1716
 if(_tmp3CF != _tmp3CD){
({struct _dyneither_ptr _tmpCA9=({const char*_tmp378="(different abstract typedefs)";_tag_dyneither(_tmp378,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpCA9;});
goto _LL11;}
# 1720
({struct _dyneither_ptr _tmpCAA=({const char*_tmp379="(type parameters to typedef differ)";_tag_dyneither(_tmp379,sizeof(char),36U);});Cyc_Tcutil_failure_reason=_tmpCAA;});
Cyc_Tcutil_unify_list(_tmp3D0,_tmp3CE);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 20U: switch(*((int*)_tmp313.f2)){case 20U: _LL36: _LL37:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 21U: switch(*((int*)_tmp313.f2)){case 21U: _LL38: _LL39:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 22U: switch(*((int*)_tmp313.f2)){case 22U: _LL3A: _LL3B:
 return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 15U: switch(*((int*)_tmp313.f2)){case 15U: _LL3C: _tmp3D2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp3D1=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL3D:
# 1727
 Cyc_Tcutil_unify_it(_tmp3D2,_tmp3D1);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 16U: switch(*((int*)_tmp313.f2)){case 16U: _LL3E: _tmp3D6=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp3D5=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp313.f1)->f2;_tmp3D4=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp313.f2)->f1;_tmp3D3=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp313.f2)->f2;_LL3F:
# 1730
 Cyc_Tcutil_unify_it(_tmp3D6,_tmp3D4);
Cyc_Tcutil_unify_it(_tmp3D5,_tmp3D3);
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 24U: _LL40: _LL41:
 goto _LL43;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp313.f2)->tag == 24U){_LL42: _LL43:
 goto _LL45;}else{switch(*((int*)_tmp313.f1)){case 23U: _LL44: _LL45:
 goto _LL47;case 25U: _LL46: _LL47:
 goto _LL49;default: switch(*((int*)_tmp313.f2)){case 25U: _LL48: _LL49:
 goto _LL4B;case 23U: _LL4A: _LL4B:
# 1739
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
({struct _dyneither_ptr _tmpCAB=({const char*_tmp37A="(effects don't unify)";_tag_dyneither(_tmp37A,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpCAB;});
goto _LL11;default: _LL4C: _LL4D:
 goto _LL11;}}}}_LL11:;}
# 1744
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1747
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpCAD)(void*,void*)=cmp;void*_tmpCAC=(void*)_check_null(a1);_tmpCAD(_tmpCAC,(void*)_check_null(a2));});}
# 1754
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3D8=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3D9=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3D8,_tmp3D9);}
# 1761
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
({union Cyc_Absyn_Constraint*_tmpCAE=Cyc_Absyn_compress_conref(x);x=_tmpCAE;});
({union Cyc_Absyn_Constraint*_tmpCAF=Cyc_Absyn_compress_conref(y);y=_tmpCAF;});
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3DA=x;void*_tmp3E1;switch((((union Cyc_Absyn_Constraint*)_tmp3DA)->Eq_constr).tag){case 3U: _LL1: _LL2:
 return - 1;case 1U: _LL3: _tmp3E1=(void*)(_tmp3DA->Eq_constr).val;_LL4: {
# 1768
union Cyc_Absyn_Constraint*_tmp3DB=y;void*_tmp3DE;switch((((union Cyc_Absyn_Constraint*)_tmp3DB)->Eq_constr).tag){case 3U: _LL8: _LL9:
 return 1;case 1U: _LLA: _tmp3DE=(void*)(_tmp3DB->Eq_constr).val;_LLB:
 return cmp(_tmp3E1,_tmp3DE);default: _LLC: _LLD:
({void*_tmp3DC=0U;({struct _dyneither_ptr _tmpCB0=({const char*_tmp3DD="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3DD,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpCB0,_tag_dyneither(_tmp3DC,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 1773
({void*_tmp3DF=0U;({struct _dyneither_ptr _tmpCB1=({const char*_tmp3E0="unify_conref: forward after compress";_tag_dyneither(_tmp3E0,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpCB1,_tag_dyneither(_tmp3DF,sizeof(void*),0U));});});}_LL0:;};}
# 1777
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp3E2=tqt1;struct Cyc_Absyn_Tqual _tmp3E8;void*_tmp3E7;_LL1: _tmp3E8=_tmp3E2->f1;_tmp3E7=_tmp3E2->f2;_LL2:;{
struct _tuple12*_tmp3E3=tqt2;struct Cyc_Absyn_Tqual _tmp3E6;void*_tmp3E5;_LL4: _tmp3E6=_tmp3E3->f1;_tmp3E5=_tmp3E3->f2;_LL5:;{
int _tmp3E4=Cyc_Tcutil_tqual_cmp(_tmp3E8,_tmp3E6);
if(_tmp3E4 != 0)return _tmp3E4;
return Cyc_Tcutil_typecmp(_tmp3E7,_tmp3E5);};};}
# 1785
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp3E9=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp3E9 != 0)return _tmp3E9;{
int _tmp3EA=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp3EA != 0)return _tmp3EA;{
int _tmp3EB=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp3EB != 0)return _tmp3EB;{
int _tmp3EC=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp3EC != 0)return _tmp3EC;
({int _tmpCB2=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);_tmp3EC=_tmpCB2;});
if(_tmp3EC != 0)return _tmp3EC;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1799
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp3ED=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3ED != 0)return _tmp3ED;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1805
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp3EE=t;switch(*((int*)_tmp3EE)){case 0U: _LL1: _LL2:
 return 0;case 1U: _LL3: _LL4:
 return 1;case 2U: _LL5: _LL6:
 return 2;case 3U: _LL7: _LL8:
 return 3;case 4U: _LL9: _LLA:
 return 4;case 5U: _LLB: _LLC:
 return 5;case 6U: _LLD: _LLE:
 return 6;case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3EE)->f1 == 0){_LLF: _LL10:
 return 7;}else{_LL11: _LL12:
 return 8;}case 8U: _LL13: _LL14:
 return 9;case 9U: _LL15: _LL16:
 return 10;case 10U: _LL17: _LL18:
 return 11;case 11U: _LL19: _LL1A:
 return 12;case 12U: _LL1B: _LL1C:
 return 14;case 13U: _LL1D: _LL1E:
 return 16;case 14U: _LL1F: _LL20:
 return 17;case 15U: _LL21: _LL22:
 return 18;case 17U: _LL23: _LL24:
 return 19;case 21U: _LL25: _LL26:
 return 20;case 20U: _LL27: _LL28:
 return 21;case 23U: _LL29: _LL2A:
 return 22;case 24U: _LL2B: _LL2C:
 return 23;case 25U: _LL2D: _LL2E:
 return 24;case 22U: _LL2F: _LL30:
 return 25;case 19U: _LL31: _LL32:
 return 26;case 16U: _LL33: _LL34:
 return 27;case 18U: _LL35: _LL36:
 return 28;case 26U: _LL37: _LL38:
 return 29;case 27U: _LL39: _LL3A:
 return 30;default: _LL3B: _LL3C:
 return 31;}_LL0:;}
# 1842
int Cyc_Tcutil_typecmp(void*t1,void*t2){
({void*_tmpCB3=Cyc_Tcutil_compress(t1);t1=_tmpCB3;});
({void*_tmpCB4=Cyc_Tcutil_compress(t2);t2=_tmpCB4;});
if(t1 == t2)return 0;{
int _tmp3EF=({int _tmpCB5=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpCB5,Cyc_Tcutil_type_case_number(t2));});
if(_tmp3EF != 0)
return _tmp3EF;{
# 1851
struct _tuple0 _tmp3F0=({struct _tuple0 _tmp453;_tmp453.f1=t2;_tmp453.f2=t1;_tmp453;});struct _tuple0 _tmp3F1=_tmp3F0;struct _dyneither_ptr _tmp452;struct _dyneither_ptr _tmp451;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp44D;void*_tmp44C;void*_tmp44B;void*_tmp44A;void*_tmp449;void*_tmp448;void*_tmp447;void*_tmp446;void*_tmp445;enum Cyc_Absyn_AggrKind _tmp444;struct Cyc_List_List*_tmp443;enum Cyc_Absyn_AggrKind _tmp442;struct Cyc_List_List*_tmp441;struct Cyc_List_List*_tmp440;struct Cyc_List_List*_tmp43F;void*_tmp43E;struct Cyc_Absyn_Tqual _tmp43D;struct Cyc_Absyn_Exp*_tmp43C;union Cyc_Absyn_Constraint*_tmp43B;void*_tmp43A;struct Cyc_Absyn_Tqual _tmp439;struct Cyc_Absyn_Exp*_tmp438;union Cyc_Absyn_Constraint*_tmp437;int _tmp436;int _tmp435;enum Cyc_Absyn_Sign _tmp434;enum Cyc_Absyn_Size_of _tmp433;enum Cyc_Absyn_Sign _tmp432;enum Cyc_Absyn_Size_of _tmp431;void*_tmp430;struct Cyc_Absyn_Tqual _tmp42F;void*_tmp42E;union Cyc_Absyn_Constraint*_tmp42D;union Cyc_Absyn_Constraint*_tmp42C;union Cyc_Absyn_Constraint*_tmp42B;void*_tmp42A;struct Cyc_Absyn_Tqual _tmp429;void*_tmp428;union Cyc_Absyn_Constraint*_tmp427;union Cyc_Absyn_Constraint*_tmp426;union Cyc_Absyn_Constraint*_tmp425;struct Cyc_Absyn_Datatypedecl*_tmp424;struct Cyc_Absyn_Datatypefield*_tmp423;struct Cyc_List_List*_tmp422;struct Cyc_Absyn_Datatypedecl*_tmp421;struct Cyc_Absyn_Datatypefield*_tmp420;struct Cyc_List_List*_tmp41F;struct Cyc_Absyn_Datatypedecl*_tmp41E;struct Cyc_List_List*_tmp41D;struct Cyc_Absyn_Datatypedecl*_tmp41C;struct Cyc_List_List*_tmp41B;struct Cyc_List_List*_tmp41A;struct Cyc_List_List*_tmp419;struct _tuple2*_tmp418;struct _tuple2*_tmp417;union Cyc_Absyn_AggrInfoU _tmp416;struct Cyc_List_List*_tmp415;union Cyc_Absyn_AggrInfoU _tmp414;struct Cyc_List_List*_tmp413;struct Cyc_Absyn_Tvar*_tmp412;struct Cyc_Absyn_Tvar*_tmp411;switch(*((int*)_tmp3F1.f1)){case 1U: switch(*((int*)_tmp3F1.f2)){case 1U: _LL1: _LL2:
# 1853
({void*_tmp3F2=0U;({struct _dyneither_ptr _tmpCB6=({const char*_tmp3F3="typecmp: can only compare closed types";_tag_dyneither(_tmp3F3,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB6,_tag_dyneither(_tmp3F2,sizeof(void*),0U));});});case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 2U: switch(*((int*)_tmp3F1.f2)){case 2U: _LL3: _tmp412=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp411=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL4:
# 1857
 return Cyc_Core_intcmp(_tmp411->identity,_tmp412->identity);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 11U: switch(*((int*)_tmp3F1.f2)){case 11U: _LL5: _tmp416=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F1.f1)->f1).aggr_info;_tmp415=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F1.f1)->f1).targs;_tmp414=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F1.f2)->f1).aggr_info;_tmp413=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F1.f2)->f1).targs;_LL6: {
# 1861
struct _tuple13 _tmp3F4=Cyc_Absyn_aggr_kinded_name(_tmp416);struct _tuple13 _tmp3F5=_tmp3F4;struct _tuple2*_tmp3FA;_LL38: _tmp3FA=_tmp3F5.f2;_LL39:;{
struct _tuple13 _tmp3F6=Cyc_Absyn_aggr_kinded_name(_tmp414);struct _tuple13 _tmp3F7=_tmp3F6;struct _tuple2*_tmp3F9;_LL3B: _tmp3F9=_tmp3F7.f2;_LL3C:;{
int _tmp3F8=Cyc_Absyn_qvar_cmp(_tmp3FA,_tmp3F9);
if(_tmp3F8 != 0)return _tmp3F8;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp415,_tmp413);}};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 13U: switch(*((int*)_tmp3F1.f2)){case 13U: _LL7: _tmp418=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp417=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL8:
# 1868
 return Cyc_Absyn_qvar_cmp(_tmp418,_tmp417);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 14U: switch(*((int*)_tmp3F1.f2)){case 14U: _LL9: _tmp41A=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp419=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LLA:
# 1871
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp41A,_tmp419);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F1.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp3F1.f2)){case 3U: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F1.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLB: _tmp41E=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F1.f1)->f1).datatype_info).KnownDatatype).val;_tmp41D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F1.f1)->f1).targs;_tmp41C=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F1.f2)->f1).datatype_info).KnownDatatype).val;_tmp41B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F1.f2)->f1).targs;_LLC:
# 1875
 if(_tmp41C == _tmp41E)return 0;{
int _tmp3FB=Cyc_Absyn_qvar_cmp(_tmp41C->name,_tmp41E->name);
if(_tmp3FB != 0)return _tmp3FB;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp41B,_tmp41D);};}else{goto _LL35;}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}else{switch(*((int*)_tmp3F1.f2)){case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F1.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp3F1.f2)){case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F1.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LLD: _tmp424=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F1.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp423=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F1.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp422=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F1.f1)->f1).targs;_tmp421=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F1.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp420=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F1.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp41F=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F1.f2)->f1).targs;_LLE:
# 1882
 if(_tmp421 == _tmp424)return 0;{
int _tmp3FC=Cyc_Absyn_qvar_cmp(_tmp424->name,_tmp421->name);
if(_tmp3FC != 0)return _tmp3FC;{
int _tmp3FD=Cyc_Absyn_qvar_cmp(_tmp423->name,_tmp420->name);
if(_tmp3FD != 0)return _tmp3FD;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp41F,_tmp422);};};}else{goto _LL35;}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}else{switch(*((int*)_tmp3F1.f2)){case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}}case 5U: switch(*((int*)_tmp3F1.f2)){case 5U: _LLF: _tmp430=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f1)->f1).elt_typ;_tmp42F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f1)->f1).elt_tq;_tmp42E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f1)->f1).ptr_atts).rgn;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f1)->f1).ptr_atts).nullable;_tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f1)->f1).ptr_atts).bounds;_tmp42B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f1)->f1).ptr_atts).zero_term;_tmp42A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f2)->f1).elt_typ;_tmp429=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f2)->f1).elt_tq;_tmp428=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f2)->f1).ptr_atts).rgn;_tmp427=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f2)->f1).ptr_atts).nullable;_tmp426=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f2)->f1).ptr_atts).bounds;_tmp425=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F1.f2)->f1).ptr_atts).zero_term;_LL10: {
# 1891
int _tmp3FE=Cyc_Tcutil_typecmp(_tmp42A,_tmp430);
if(_tmp3FE != 0)return _tmp3FE;{
int _tmp3FF=Cyc_Tcutil_typecmp(_tmp428,_tmp42E);
if(_tmp3FF != 0)return _tmp3FF;{
int _tmp400=Cyc_Tcutil_tqual_cmp(_tmp429,_tmp42F);
if(_tmp400 != 0)return _tmp400;{
int _tmp401=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp426,_tmp42C);
if(_tmp401 != 0)return _tmp401;{
int _tmp402=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp425,_tmp42B);
if(_tmp402 != 0)return _tmp402;
{void*_tmp403=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp426);void*_tmp404=_tmp403;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp404)->tag == 0U){_LL3E: _LL3F:
 return 0;}else{_LL40: _LL41:
 goto _LL3D;}_LL3D:;}
# 1905
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp427,_tmp42D);};};};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 6U: switch(*((int*)_tmp3F1.f2)){case 6U: _LL11: _tmp434=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp433=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1.f1)->f2;_tmp432=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_tmp431=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F1.f2)->f2;_LL12:
# 1908
 if(_tmp432 != _tmp434)return Cyc_Core_intcmp((int)((unsigned int)_tmp432),(int)((unsigned int)_tmp434));
if(_tmp431 != _tmp433)return Cyc_Core_intcmp((int)((unsigned int)_tmp431),(int)((unsigned int)_tmp433));
return 0;case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 7U: switch(*((int*)_tmp3F1.f2)){case 7U: _LL13: _tmp436=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp435=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL14:
# 1913
 if(_tmp436 == _tmp435)return 0;else{
if(_tmp435 == 0)return - 1;else{
if(_tmp435 == 1  && _tmp436 == 0)return - 1;else{
return 1;}}}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 8U: switch(*((int*)_tmp3F1.f2)){case 8U: _LL15: _tmp43E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F1.f1)->f1).elt_type;_tmp43D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F1.f1)->f1).tq;_tmp43C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F1.f1)->f1).num_elts;_tmp43B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F1.f1)->f1).zero_term;_tmp43A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F1.f2)->f1).elt_type;_tmp439=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F1.f2)->f1).tq;_tmp438=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F1.f2)->f1).num_elts;_tmp437=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F1.f2)->f1).zero_term;_LL16: {
# 1920
int _tmp405=Cyc_Tcutil_tqual_cmp(_tmp439,_tmp43D);
if(_tmp405 != 0)return _tmp405;{
int _tmp406=Cyc_Tcutil_typecmp(_tmp43A,_tmp43E);
if(_tmp406 != 0)return _tmp406;{
int _tmp407=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp43B,_tmp437);
if(_tmp407 != 0)return _tmp407;
if(_tmp43C == _tmp438)return 0;
if(_tmp43C == 0  || _tmp438 == 0)
({void*_tmp408=0U;({struct _dyneither_ptr _tmpCB7=({const char*_tmp409="missing expression in array index";_tag_dyneither(_tmp409,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB7,_tag_dyneither(_tmp408,sizeof(void*),0U));});});
# 1930
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp43C,_tmp438);};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 9U: switch(*((int*)_tmp3F1.f2)){case 9U: _LL17: _LL18:
# 1933
({void*_tmp40A=0U;({struct _dyneither_ptr _tmpCB8=({const char*_tmp40B="typecmp: function types not handled";_tag_dyneither(_tmp40B,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB8,_tag_dyneither(_tmp40A,sizeof(void*),0U));});});case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 10U: switch(*((int*)_tmp3F1.f2)){case 10U: _LL19: _tmp440=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp43F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL1A:
# 1936
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp43F,_tmp440);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 12U: switch(*((int*)_tmp3F1.f2)){case 12U: _LL1B: _tmp444=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp443=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F1.f1)->f2;_tmp442=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_tmp441=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F1.f2)->f2;_LL1C:
# 1939
 if(_tmp442 != _tmp444){
if(_tmp442 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp441,_tmp443);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 15U: switch(*((int*)_tmp3F1.f2)){case 15U: _LL1D: _tmp446=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp445=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL1E:
# 1944
 return Cyc_Tcutil_typecmp(_tmp446,_tmp445);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 16U: switch(*((int*)_tmp3F1.f2)){case 16U: _LL1F: _tmp44A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp449=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F1.f1)->f2;_tmp448=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_tmp447=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F1.f2)->f2;_LL20: {
# 1946
int _tmp40C=Cyc_Tcutil_typecmp(_tmp44A,_tmp448);
if(_tmp40C != 0)return _tmp40C;else{
return Cyc_Tcutil_typecmp(_tmp449,_tmp447);}}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 19U: switch(*((int*)_tmp3F1.f2)){case 19U: _LL21: _tmp44C=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp44B=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL22:
 return Cyc_Tcutil_typecmp(_tmp44C,_tmp44B);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 18U: switch(*((int*)_tmp3F1.f2)){case 18U: _LL23: _tmp44E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp44D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL24:
 _tmp450=_tmp44E;_tmp44F=_tmp44D;goto _LL26;case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 27U: switch(*((int*)_tmp3F1.f2)){case 27U: _LL25: _tmp450=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp44F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL26:
# 1952
 return Cyc_Evexp_const_exp_cmp(_tmp450,_tmp44F);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 28U: switch(*((int*)_tmp3F1.f2)){case 28U: _LL27: _tmp452=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp451=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL28:
# 1954
 return Cyc_strcmp((struct _dyneither_ptr)_tmp452,(struct _dyneither_ptr)_tmp451);case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 24U: _LL29: _LL2A:
 goto _LL2C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3F1.f2)->tag == 24U){_LL2B: _LL2C:
 goto _LL2E;}else{switch(*((int*)_tmp3F1.f1)){case 23U: _LL2D: _LL2E:
 goto _LL30;case 25U: _LL2F: _LL30:
 goto _LL32;default: switch(*((int*)_tmp3F1.f2)){case 25U: _LL31: _LL32:
 goto _LL34;case 23U: _LL33: _LL34:
({void*_tmp40D=0U;({struct _dyneither_ptr _tmpCB9=({const char*_tmp40E="typecmp: effects not handled";_tag_dyneither(_tmp40E,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB9,_tag_dyneither(_tmp40D,sizeof(void*),0U));});});default: _LL35: _LL36:
({void*_tmp40F=0U;({struct _dyneither_ptr _tmpCBA=({const char*_tmp410="Unmatched case in typecmp";_tag_dyneither(_tmp410,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCBA,_tag_dyneither(_tmp40F,sizeof(void*),0U));});});}}}}_LL0:;};};}
# 1965
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp454=Cyc_Tcutil_compress(t);void*_tmp455=_tmp454;switch(*((int*)_tmp455)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 1977
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
({void*_tmpCBB=Cyc_Tcutil_compress(t1);t1=_tmpCBB;});
({void*_tmpCBC=Cyc_Tcutil_compress(t2);t2=_tmpCBC;});{
struct _tuple0 _tmp456=({struct _tuple0 _tmp45A;_tmp45A.f1=t1;_tmp45A.f2=t2;_tmp45A;});struct _tuple0 _tmp457=_tmp456;int _tmp459;int _tmp458;switch(*((int*)_tmp457.f1)){case 7U: switch(*((int*)_tmp457.f2)){case 7U: _LL1: _tmp459=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp457.f1)->f1;_tmp458=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp457.f2)->f1;_LL2:
# 1982
 return _tmp458 == 0  && _tmp459 != 0  || (_tmp458 == 1  && _tmp459 != 0) && _tmp459 != 1;case 6U: _LL3: _LL4:
 goto _LL6;case 19U: _LL5: _LL6:
 return 1;default: goto _LL21;}case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 return 0;}else{goto _LL9;}}else{_LL9: _LLA:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp457.f2)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f2){case Cyc_Absyn_Int_sz: _LLB: _LLC:
# 1989
 goto _LLE;case Cyc_Absyn_Short_sz: _LL13: _LL14:
# 1994
 goto _LL16;case Cyc_Absyn_Char_sz: _LL19: _LL1A:
# 1997
 goto _LL1C;default: goto _LL21;}case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp457.f2)->f1 == 0){_LLF: _LL10:
# 1992
 goto _LL12;}else{goto _LL21;}default: goto _LL21;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp457.f2)){case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f2){case Cyc_Absyn_Long_sz: _LLD: _LLE:
# 1990
 return 0;case Cyc_Absyn_Short_sz: _LL15: _LL16:
# 1995
 goto _LL18;case Cyc_Absyn_Char_sz: _LL1B: _LL1C:
# 1998
 goto _LL1E;default: goto _LL21;}case 7U: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp457.f2)->f1 == 0){_LL11: _LL12:
# 1993
 goto _LL14;}else{goto _LL21;}default: goto _LL21;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f2 == Cyc_Absyn_Char_sz){_LL1D: _LL1E:
# 1999
 goto _LL20;}else{goto _LL21;}}else{goto _LL21;}default: goto _LL21;}case 19U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp457.f2)->f2){case Cyc_Absyn_Short_sz: _LL17: _LL18:
# 1996
 goto _LL1A;case Cyc_Absyn_Char_sz: _LL1F: _LL20:
# 2000
 return 1;default: goto _LL21;}else{goto _LL21;}default: _LL21: _LL22:
# 2002
 return 0;}_LL0:;};}
# 2008
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2011
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
({struct Cyc_Core_Opt*_tmpCBD=({struct Cyc_Core_Opt*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->v=t1;_tmp45B;});max_arith_type=_tmpCBD;});}}}
# 2020
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2024
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp45F;_tmp45F.tag=0U;({struct _dyneither_ptr _tmpCBE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp45F.f1=_tmpCBE;});({struct Cyc_String_pa_PrintArg_struct _tmp45E;_tmp45E.tag=0U;({struct _dyneither_ptr _tmpCBF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp45E.f1=_tmpCBF;});({void*_tmp45C[2U]={& _tmp45E,& _tmp45F};({unsigned int _tmpCC1=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpCC0=({const char*_tmp45D="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp45D,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpCC1,_tmpCC0,_tag_dyneither(_tmp45C,sizeof(void*),2U));});});});});
return 0;}}}
# 2030
return 1;}
# 2035
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp460=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp461=_tmp460;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp461)->tag == 5U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2041
return 1;}
# 2044
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp462=Cyc_Tcutil_compress(t);void*_tmp463=_tmp462;switch(*((int*)_tmp463)){case 6U: _LL1: _LL2:
 goto _LL4;case 19U: _LL3: _LL4:
 goto _LL6;case 13U: _LL5: _LL6:
 goto _LL8;case 14U: _LL7: _LL8:
 return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2055
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2059
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp464=0U;({unsigned int _tmpCC3=e->loc;struct _dyneither_ptr _tmpCC2=({const char*_tmp465="integral size mismatch; conversion supplied";_tag_dyneither(_tmp465,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpCC3,_tmpCC2,_tag_dyneither(_tmp464,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2065
return 0;}
# 2069
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2073
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp466=0U;({unsigned int _tmpCC5=e->loc;struct _dyneither_ptr _tmpCC4=({const char*_tmp467="integral size mismatch; conversion supplied";_tag_dyneither(_tmp467,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpCC5,_tmpCC4,_tag_dyneither(_tmp466,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2079
return 0;}
# 2084
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2092
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2094
({void*_tmpCC6=Cyc_Tcutil_compress(t1);t1=_tmpCC6;});
({void*_tmpCC7=Cyc_Tcutil_compress(t2);t2=_tmpCC7;});{
struct _tuple0 _tmp468=({struct _tuple0 _tmp481;_tmp481.f1=t1;_tmp481.f2=t2;_tmp481;});struct _tuple0 _tmp469=_tmp468;void*_tmp480;struct Cyc_Absyn_Tqual _tmp47F;struct Cyc_Absyn_Exp*_tmp47E;union Cyc_Absyn_Constraint*_tmp47D;void*_tmp47C;struct Cyc_Absyn_Tqual _tmp47B;struct Cyc_Absyn_Exp*_tmp47A;union Cyc_Absyn_Constraint*_tmp479;struct Cyc_Absyn_PtrInfo _tmp478;struct Cyc_Absyn_PtrInfo _tmp477;switch(*((int*)_tmp469.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f2)->tag == 5U){_LL1: _tmp478=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f1)->f1;_tmp477=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f2)->f1;_LL2: {
# 2098
int okay=1;
# 2100
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp478.ptr_atts).nullable,(_tmp477.ptr_atts).nullable))
# 2103
({int _tmpCC8=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp478.ptr_atts).nullable);okay=_tmpCC8;});
# 2105
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp478.ptr_atts).bounds,(_tmp477.ptr_atts).bounds)){
# 2108
struct _tuple0 _tmp46A=({struct _tuple0 _tmp471;({void*_tmpCC9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp478.ptr_atts).bounds);_tmp471.f1=_tmpCC9;});({void*_tmpCCA=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp477.ptr_atts).bounds);_tmp471.f2=_tmpCCA;});_tmp471;});
# 2108
struct _tuple0 _tmp46B=_tmp46A;struct Cyc_Absyn_Exp*_tmp470;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_Absyn_Exp*_tmp46E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f1)->tag == 1U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46B.f2)->tag == 0U){_LLA: _LLB:
# 2110
 goto _LLD;}else{_LLE: _tmp46F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f1)->f1;_tmp46E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f2)->f1;_LLF:
# 2113
({int _tmpCCB=okay  && Cyc_Evexp_lte_const_exp(_tmp46E,_tmp46F);okay=_tmpCCB;});
# 2117
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp477.ptr_atts).zero_term))
({void*_tmp46C=0U;({unsigned int _tmpCCD=loc;struct _dyneither_ptr _tmpCCC=({const char*_tmp46D="implicit cast to shorter array";_tag_dyneither(_tmp46D,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpCCD,_tmpCCC,_tag_dyneither(_tmp46C,sizeof(void*),0U));});});
goto _LL9;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46B.f2)->tag == 0U){_LLC: _LLD:
# 2111
 okay=1;goto _LL9;}else{_LL10: _tmp470=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f2)->f1;_LL11:
# 2122
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp478.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp477.ptr_atts).bounds))
goto _LL9;
okay=0;
goto _LL9;}}_LL9:;}
# 2131
okay=okay  && (!(_tmp478.elt_tq).real_const  || (_tmp477.elt_tq).real_const);
# 2134
if(!Cyc_Tcutil_unify((_tmp478.ptr_atts).rgn,(_tmp477.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp478.ptr_atts).rgn,(_tmp477.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp475;_tmp475.tag=0U;({struct _dyneither_ptr _tmpCCE=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2139
Cyc_Absynpp_typ2string((_tmp477.ptr_atts).rgn));_tmp475.f1=_tmpCCE;});({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0U;({struct _dyneither_ptr _tmpCCF=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2138
Cyc_Absynpp_typ2string((_tmp478.ptr_atts).rgn));_tmp474.f1=_tmpCCF;});({void*_tmp472[2U]={& _tmp474,& _tmp475};({unsigned int _tmpCD1=loc;struct _dyneither_ptr _tmpCD0=({const char*_tmp473="implicit cast from region %s to region %s";_tag_dyneither(_tmp473,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpCD1,_tmpCD0,_tag_dyneither(_tmp472,sizeof(void*),2U));});});});});}else{
# 2140
okay=0;}}
# 2144
({int _tmpCD2=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp478.ptr_atts).zero_term,(_tmp477.ptr_atts).zero_term) || 
# 2147
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp478.ptr_atts).zero_term) && (_tmp477.elt_tq).real_const);
# 2144
okay=_tmpCD2;});{
# 2155
int _tmp476=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp477.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2158
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp477.ptr_atts).zero_term);
# 2162
({int _tmpCD3=okay  && (Cyc_Tcutil_unify(_tmp478.elt_typ,_tmp477.elt_typ) || 
(_tmp476  && ((_tmp477.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp477.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp478.elt_typ,_tmp477.elt_typ));
# 2162
okay=_tmpCD3;});
# 2165
return okay;};}}else{goto _LL7;}case 8U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->tag == 8U){_LL3: _tmp480=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).elt_type;_tmp47F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).tq;_tmp47E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).num_elts;_tmp47D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).zero_term;_tmp47C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).elt_type;_tmp47B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).tq;_tmp47A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).num_elts;_tmp479=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).zero_term;_LL4: {
# 2169
int okay;
# 2171
({int _tmpCD4=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp47D,_tmp479) && (
(_tmp47E != 0  && _tmp47A != 0) && Cyc_Evexp_same_const_exp(_tmp47E,_tmp47A));
# 2171
okay=_tmpCD4;});
# 2174
return(okay  && Cyc_Tcutil_unify(_tmp480,_tmp47C)) && (!_tmp47F.real_const  || _tmp47B.real_const);}}else{goto _LL7;}case 19U: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp469.f2)->tag == 6U){_LL5: _LL6:
# 2176
 return 0;}else{goto _LL7;}default: _LL7: _LL8:
# 2178
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2182
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp482=Cyc_Tcutil_compress(t);void*_tmp483=_tmp482;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp483)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2189
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp484=Cyc_Tcutil_compress(t);void*_tmp485=_tmp484;union Cyc_Absyn_Constraint*_tmp486;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp485)->tag == 5U){_LL1: _tmp486=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp485)->f1).ptr_atts).zero_term;_LL2:
# 2192
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp486);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2197
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp487=Cyc_Tcutil_compress(t);void*_tmp488=_tmp487;union Cyc_Absyn_Constraint*_tmp489;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp488)->tag == 5U){_LL1: _tmp489=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp488)->f1).ptr_atts).nullable;_LL2:
# 2200
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp489);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2206
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp48A=Cyc_Tcutil_compress(t);void*_tmp48B=_tmp48A;void*_tmp48E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48B)->tag == 5U){_LL1: _tmp48E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48B)->f1).elt_typ;_LL2:
 return _tmp48E;}else{_LL3: _LL4:
({void*_tmp48C=0U;({struct _dyneither_ptr _tmpCD5=({const char*_tmp48D="pointer_elt_type";_tag_dyneither(_tmp48D,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD5,_tag_dyneither(_tmp48C,sizeof(void*),0U));});});}_LL0:;}
# 2212
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp48F=Cyc_Tcutil_compress(t);void*_tmp490=_tmp48F;struct Cyc_Absyn_PtrAtts*_tmp493;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490)->tag == 5U){_LL1: _tmp493=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp490)->f1).ptr_atts;_LL2:
 return _tmp493->rgn;}else{_LL3: _LL4:
({void*_tmp491=0U;({struct _dyneither_ptr _tmpCD6=({const char*_tmp492="pointer_elt_type";_tag_dyneither(_tmp492,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD6,_tag_dyneither(_tmp491,sizeof(void*),0U));});});}_LL0:;}
# 2219
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp494=Cyc_Tcutil_compress(t);void*_tmp495=_tmp494;void*_tmp496;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp495)->tag == 5U){_LL1: _tmp496=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp495)->f1).ptr_atts).rgn;_LL2:
# 2222
*rgn=_tmp496;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2229
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2233
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp497=Cyc_Tcutil_compress(t);void*_tmp498=_tmp497;union Cyc_Absyn_Constraint*_tmp499;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp498)->tag == 5U){_LL1: _tmp499=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp498)->f1).ptr_atts).bounds;_LL2:
# 2236
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp499)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2242
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp49A=e->r;void*_tmp49B=_tmp49A;void*_tmp49F;struct Cyc_Absyn_Exp*_tmp49E;struct _dyneither_ptr _tmp49D;switch(*((int*)_tmp49B)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49B)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49B)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49B)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49B)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49B)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp49D=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49B)->f1).Wchar_c).val;_LLC: {
# 2251
unsigned long _tmp49C=Cyc_strlen((struct _dyneither_ptr)_tmp49D);
int i=0;
if(_tmp49C >= 2  && *((const char*)_check_dyneither_subscript(_tmp49D,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp49D,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp49D.curr)[1]== 'x'  && _tmp49C >= 3) && *((const char*)_check_dyneither_subscript(_tmp49D,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp49C;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp49D,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2261
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2249
 return 1;case 14U: _LLD: _tmp49F=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp49B)->f1;_tmp49E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp49B)->f2;_LLE:
# 2262
 return Cyc_Tcutil_is_zero(_tmp49E) && Cyc_Tcutil_admits_zero(_tmp49F);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2267
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2274
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2279
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2284
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2291
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2296
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2301
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp4A0=ka;enum Cyc_Absyn_KindQual _tmp4A9;enum Cyc_Absyn_AliasQual _tmp4A8;_LL1: _tmp4A9=_tmp4A0->kind;_tmp4A8=_tmp4A0->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp4A1=_tmp4A8;switch(_tmp4A1){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2305
enum Cyc_Absyn_KindQual _tmp4A2=_tmp4A9;switch(_tmp4A2){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& Cyc_Tcutil_eko;default: _LL15: _LL16:
 return& Cyc_Tcutil_iko;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2314
{enum Cyc_Absyn_KindQual _tmp4A3=_tmp4A9;switch(_tmp4A3){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& Cyc_Tcutil_urko;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2321
goto _LL3;default: _LL8: _LL9:
# 2323
{enum Cyc_Absyn_KindQual _tmp4A4=_tmp4A9;switch(_tmp4A4){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& Cyc_Tcutil_trko;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2330
goto _LL3;}_LL3:;}
# 2332
({struct Cyc_String_pa_PrintArg_struct _tmp4A7;_tmp4A7.tag=0U;({struct _dyneither_ptr _tmpCD7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp4A7.f1=_tmpCD7;});({void*_tmp4A5[1U]={& _tmp4A7};({struct _dyneither_ptr _tmpCD8=({const char*_tmp4A6="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4A6,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD8,_tag_dyneither(_tmp4A5,sizeof(void*),1U));});});});}
# 2335
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0U,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0U,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0U,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0U,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0U,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0U,& Cyc_Tcutil_ik};
# 2343
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2350
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0U,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0U,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0U,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0U,& Cyc_Tcutil_trk};
# 2355
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2360
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0U,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0U,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0U,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0U,& Cyc_Tcutil_urk};
# 2365
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2370
struct Cyc_Absyn_Kind*_tmp4AA=ka;enum Cyc_Absyn_KindQual _tmp4B3;enum Cyc_Absyn_AliasQual _tmp4B2;_LL1: _tmp4B3=_tmp4AA->kind;_tmp4B2=_tmp4AA->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp4AB=_tmp4B2;switch(_tmp4AB){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2373
enum Cyc_Absyn_KindQual _tmp4AC=_tmp4B3;switch(_tmp4AC){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& ab;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& mb;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& bb;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& rb;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& eb;default: _LL15: _LL16:
 return& ib;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2382
{enum Cyc_Absyn_KindQual _tmp4AD=_tmp4B3;switch(_tmp4AD){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& uab;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& umb;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& urb;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2389
goto _LL3;default: _LL8: _LL9:
# 2391
{enum Cyc_Absyn_KindQual _tmp4AE=_tmp4B3;switch(_tmp4AE){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& tab;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& tmb;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& tbb;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& trb;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2398
goto _LL3;}_LL3:;}
# 2400
({struct Cyc_String_pa_PrintArg_struct _tmp4B1;_tmp4B1.tag=0U;({struct _dyneither_ptr _tmpCD9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp4B1.f1=_tmpCD9;});({void*_tmp4AF[1U]={& _tmp4B1};({struct _dyneither_ptr _tmpCDA=({const char*_tmp4B0="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4B0,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCDA,_tag_dyneither(_tmp4AF,sizeof(void*),1U));});});});}
# 2403
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2406
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2408
return({struct Cyc_Core_Opt*_tmp4C2=_cycalloc(sizeof(*_tmp4C2));({void*_tmpCDB=Cyc_Tcutil_kind_to_bound(k);_tmp4C2->v=_tmpCDB;});_tmp4C2;});}
# 2413
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpCDD=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4C3=_cycalloc(sizeof(*_tmp4C3));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpCDC=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4C4;_tmp4C4.tag=0U;_tmp4C4.f1=Cyc_Absyn_Null_c;_tmp4C4;});_tmp4C3[0]=_tmpCDC;});_tmp4C3;});e1->r=_tmpCDD;});{
struct Cyc_Core_Opt*_tmp4C5=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C6=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CB=_cycalloc(sizeof(*_tmp4CB));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpCE5=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4CC;_tmp4CC.tag=5U;({struct Cyc_Absyn_PtrInfo _tmpCE4=({struct Cyc_Absyn_PtrInfo _tmp4CD;({void*_tmpCDE=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4C5);_tmp4CD.elt_typ=_tmpCDE;});({struct Cyc_Absyn_Tqual _tmpCDF=
Cyc_Absyn_empty_tqual(0U);_tmp4CD.elt_tq=_tmpCDF;});({struct Cyc_Absyn_PtrAtts _tmpCE3=({({void*_tmpCE0=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4C5);(_tmp4CD.ptr_atts).rgn=_tmpCE0;});(_tmp4CD.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmpCE1=
# 2421
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4CD.ptr_atts).bounds=_tmpCE1;});({union Cyc_Absyn_Constraint*_tmpCE2=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4CD.ptr_atts).zero_term=_tmpCE2;});(_tmp4CD.ptr_atts).ptrloc=0;_tmp4CD.ptr_atts;});_tmp4CD.ptr_atts=_tmpCE3;});_tmp4CD;});_tmp4CC.f1=_tmpCE4;});_tmp4CC;});_tmp4CB[0]=_tmpCE5;});_tmp4CB;});
e1->topt=(void*)_tmp4C6;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4CA;_tmp4CA.tag=0U;({struct _dyneither_ptr _tmpCE6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));_tmp4CA.f1=_tmpCE6;});({struct Cyc_String_pa_PrintArg_struct _tmp4C9;_tmp4C9.tag=0U;({struct _dyneither_ptr _tmpCE7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp4C9.f1=_tmpCE7;});({void*_tmp4C7[2U]={& _tmp4C9,& _tmp4CA};({struct _dyneither_ptr _tmpCE8=({const char*_tmp4C8="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4C8,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCE8,_tag_dyneither(_tmp4C7,sizeof(void*),2U));});});});});
return retv;};};}
# 2431
return 0;}
# 2434
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4CE=c;switch(_tmp4CE){case Cyc_Absyn_Unknown_coercion: _LL1: _LL2:
 return({const char*_tmp4CF="unknown";_tag_dyneither(_tmp4CF,sizeof(char),8U);});case Cyc_Absyn_No_coercion: _LL3: _LL4:
 return({const char*_tmp4D0="no coercion";_tag_dyneither(_tmp4D0,sizeof(char),12U);});case Cyc_Absyn_Null_to_NonNull: _LL5: _LL6:
 return({const char*_tmp4D1="null check";_tag_dyneither(_tmp4D1,sizeof(char),11U);});default: _LL7: _LL8:
 return({const char*_tmp4D2="other coercion";_tag_dyneither(_tmp4D2,sizeof(char),15U);});}_LL0:;}
# 2443
int Cyc_Tcutil_warn_alias_coerce=0;
# 2449
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2453
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4E6=_cycalloc(sizeof(*_tmp4E6));_tmp4E6->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpCEB=({struct _dyneither_ptr*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));({struct _dyneither_ptr _tmpCEA=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4EA;_tmp4EA.tag=1U;_tmp4EA.f1=(unsigned long)counter ++;({void*_tmp4E8[1U]={& _tmp4EA};({struct _dyneither_ptr _tmpCE9=({const char*_tmp4E9="__aliasvar%d";_tag_dyneither(_tmp4E9,sizeof(char),13U);});Cyc_aprintf(_tmpCE9,_tag_dyneither(_tmp4E8,sizeof(void*),1U));});});});_tmp4E7[0]=_tmpCEA;});_tmp4E7;});_tmp4E6->f2=_tmpCEB;});_tmp4E6;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_typ,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpCED=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E4=_cycalloc(sizeof(*_tmp4E4));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCEC=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4E5;_tmp4E5.tag=4U;_tmp4E5.f1=vd;_tmp4E5;});_tmp4E4[0]=_tmpCEC;});_tmp4E4;});Cyc_Absyn_varb_exp(_tmpCED,e->loc);});
# 2462
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2464
{void*_tmp4D3=Cyc_Tcutil_compress(e_typ);void*_tmp4D4=_tmp4D3;void*_tmp4E2;struct Cyc_Absyn_Tqual _tmp4E1;void*_tmp4E0;union Cyc_Absyn_Constraint*_tmp4DF;union Cyc_Absyn_Constraint*_tmp4DE;union Cyc_Absyn_Constraint*_tmp4DD;struct Cyc_Absyn_PtrLoc*_tmp4DC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4)->tag == 5U){_LL1: _tmp4E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4)->f1).elt_typ;_tmp4E1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4)->f1).elt_tq;_tmp4E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4)->f1).ptr_atts).rgn;_tmp4DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4)->f1).ptr_atts).nullable;_tmp4DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4)->f1).ptr_atts).bounds;_tmp4DD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4)->f1).ptr_atts).zero_term;_tmp4DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D4)->f1).ptr_atts).ptrloc;_LL2:
# 2466
{void*_tmp4D5=Cyc_Tcutil_compress(_tmp4E0);void*_tmp4D6=_tmp4D5;void**_tmp4DB;struct Cyc_Core_Opt*_tmp4DA;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D6)->tag == 1U){_LL6: _tmp4DB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D6)->f2;_tmp4DA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D6)->f4;_LL7: {
# 2468
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D7=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpCEE=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4D9;_tmp4D9.tag=2U;_tmp4D9.f1=tv;_tmp4D9;});_tmp4D8[0]=_tmpCEE;});_tmp4D8;});
*_tmp4DB=(void*)_tmp4D7;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2473
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2477
e->topt=0;
vd->initializer=0;{
# 2481
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2483
return({struct _tuple15 _tmp4E3;_tmp4E3.f1=d;_tmp4E3.f2=ve;_tmp4E3;});};}
# 2488
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2491
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2496
void*_tmp4EC=Cyc_Tcutil_compress(wants_typ);void*_tmp4ED=_tmp4EC;void*_tmp4F1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4ED)->tag == 5U){_LL1: _tmp4F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4ED)->f1).ptr_atts).rgn;_LL2: {
# 2498
void*_tmp4EE=Cyc_Tcutil_compress(_tmp4F1);void*_tmp4EF=_tmp4EE;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4EF)->tag == 20U){_LL6: _LL7:
 return 0;}else{_LL8: _LL9: {
# 2501
struct Cyc_Absyn_Kind*_tmp4F0=Cyc_Tcutil_typ_kind(_tmp4F1);
return _tmp4F0->kind == Cyc_Absyn_RgnKind  && _tmp4F0->aliasqual == Cyc_Absyn_Aliasable;}}_LL5:;}}else{_LL3: _LL4:
# 2504
 return 0;}_LL0:;}
# 2507
return 0;}
# 2511
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2516
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2518
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2520
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp4F5;_tmp4F5.tag=0U;({struct _dyneither_ptr _tmpCEF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4F5.f1=_tmpCEF;});({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U;({struct _dyneither_ptr _tmpCF0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F4.f1=_tmpCF0;});({void*_tmp4F2[2U]={& _tmp4F4,& _tmp4F5};({unsigned int _tmpCF2=e->loc;struct _dyneither_ptr _tmpCF1=({const char*_tmp4F3="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp4F3,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpCF2,_tmpCF1,_tag_dyneither(_tmp4F2,sizeof(void*),2U));});});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2528
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0U;({struct _dyneither_ptr _tmpCF3=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4FA.f1=_tmpCF3;});({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0U;({struct _dyneither_ptr _tmpCF4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F9.f1=_tmpCF4;});({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0U;({struct _dyneither_ptr _tmpCF5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4F8.f1=_tmpCF5;});({void*_tmp4F6[3U]={& _tmp4F8,& _tmp4F9,& _tmp4FA};({unsigned int _tmpCF7=e->loc;struct _dyneither_ptr _tmpCF6=({const char*_tmp4F7="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp4F7,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpCF7,_tmpCF6,_tag_dyneither(_tmp4F6,sizeof(void*),3U));});});});});});
*alias_coercion=1;}
# 2535
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if(({enum Cyc_Absyn_Coercion _tmpCF8=Cyc_Tcutil_castable(te,e->loc,t1,t2);c=_tmpCF8;})!= Cyc_Absyn_Unknown_coercion){
# 2543
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp4FE;_tmp4FE.tag=0U;({struct _dyneither_ptr _tmpCF9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4FE.f1=_tmpCF9;});({struct Cyc_String_pa_PrintArg_struct _tmp4FD;_tmp4FD.tag=0U;({struct _dyneither_ptr _tmpCFA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4FD.f1=_tmpCFA;});({void*_tmp4FB[2U]={& _tmp4FD,& _tmp4FE};({unsigned int _tmpCFC=e->loc;struct _dyneither_ptr _tmpCFB=({const char*_tmp4FC="implicit cast from %s to %s";_tag_dyneither(_tmp4FC,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpCFC,_tmpCFB,_tag_dyneither(_tmp4FB,sizeof(void*),2U));});});});});
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
void*_tmp4FF=Cyc_Tcutil_compress(t);void*_tmp500=_tmp4FF;switch(*((int*)_tmp500)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2581 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2585
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2588
struct _tuple23 _tmp501=*env;struct _tuple23 _tmp502=_tmp501;struct Cyc_List_List*_tmp50A;struct _RegionHandle*_tmp509;struct Cyc_Tcenv_Tenv*_tmp508;int _tmp507;_LL1: _tmp50A=_tmp502.f1;_tmp509=_tmp502.f2;_tmp508=_tmp502.f3;_tmp507=_tmp502.f4;_LL2:;{
# 2590
void*_tmp503=_tmp50A == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp509,_tmp50A,x->type);
struct Cyc_List_List*_tmp504=Cyc_Tcutil_flatten_typ(_tmp509,_tmp507,_tmp508,_tmp503);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp504)== 1)
return({struct Cyc_List_List*_tmp505=_region_malloc(_tmp509,sizeof(*_tmp505));({struct _tuple12*_tmpCFD=({struct _tuple12*_tmp506=_region_malloc(_tmp509,sizeof(*_tmp506));_tmp506->f1=x->tq;_tmp506->f2=_tmp503;_tmp506;});_tmp505->hd=_tmpCFD;});_tmp505->tl=0;_tmp505;});else{
return _tmp504;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2596
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2598
struct _tuple24 _tmp50B=*env;struct _tuple24 _tmp50C=_tmp50B;struct _RegionHandle*_tmp516;struct Cyc_Tcenv_Tenv*_tmp515;int _tmp514;_LL1: _tmp516=_tmp50C.f1;_tmp515=_tmp50C.f2;_tmp514=_tmp50C.f3;_LL2:;{
struct _tuple12 _tmp50D=*x;struct _tuple12 _tmp50E=_tmp50D;struct Cyc_Absyn_Tqual _tmp513;void*_tmp512;_LL4: _tmp513=_tmp50E.f1;_tmp512=_tmp50E.f2;_LL5:;{
struct Cyc_List_List*_tmp50F=Cyc_Tcutil_flatten_typ(_tmp516,_tmp514,_tmp515,_tmp512);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp50F)== 1)
return({struct Cyc_List_List*_tmp510=_region_malloc(_tmp516,sizeof(*_tmp510));({struct _tuple12*_tmpCFE=({struct _tuple12*_tmp511=_region_malloc(_tmp516,sizeof(*_tmp511));_tmp511->f1=_tmp513;_tmp511->f2=_tmp512;_tmp511;});_tmp510->hd=_tmpCFE;});_tmp510->tl=0;_tmp510;});else{
return _tmp50F;}};};}
# 2605
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2609
if(flatten){
({void*_tmpCFF=Cyc_Tcutil_compress(t1);t1=_tmpCFF;});{
void*_tmp517=t1;struct Cyc_List_List*_tmp539;struct Cyc_Absyn_Aggrdecl*_tmp538;struct Cyc_List_List*_tmp537;struct Cyc_List_List*_tmp536;switch(*((int*)_tmp517)){case 0U: _LL1: _LL2:
 return 0;case 10U: _LL3: _tmp536=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp517)->f1;_LL4: {
# 2614
struct _tuple24 _tmp518=({struct _tuple24 _tmp521;_tmp521.f1=r;_tmp521.f2=te;_tmp521.f3=flatten;_tmp521;});
# 2616
struct Cyc_List_List*_tmp519=_tmp536;struct _tuple12*_tmp520;struct Cyc_List_List*_tmp51F;if(_tmp519 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp520=(struct _tuple12*)_tmp519->hd;_tmp51F=_tmp519->tl;_LLF: {
# 2619
struct Cyc_List_List*_tmp51A=Cyc_Tcutil_rcopy_tqt(& _tmp518,_tmp520);
_tmp518.f3=0;{
struct Cyc_List_List*_tmp51B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp518,_tmp536);
struct Cyc_List_List*_tmp51C=({struct Cyc_List_List*_tmp51E=_region_malloc(r,sizeof(*_tmp51E));_tmp51E->hd=_tmp51A;_tmp51E->tl=_tmp51B;_tmp51E;});
return({struct _RegionHandle*_tmpD00=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpD00,({struct Cyc_List_List*_tmp51D=_region_malloc(r,sizeof(*_tmp51D));_tmp51D->hd=_tmp51A;_tmp51D->tl=_tmp51B;_tmp51D;}));});};}}_LLB:;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp517)->f1).aggr_info).KnownAggr).tag == 2){_LL5: _tmp538=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp517)->f1).aggr_info).KnownAggr).val;_tmp537=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp517)->f1).targs;_LL6:
# 2627
 if(((_tmp538->kind == Cyc_Absyn_UnionA  || _tmp538->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp538->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp538->impl))->rgn_po != 0)
# 2629
return({struct Cyc_List_List*_tmp522=_region_malloc(r,sizeof(*_tmp522));({struct _tuple12*_tmpD02=({struct _tuple12*_tmp523=_region_malloc(r,sizeof(*_tmp523));({struct Cyc_Absyn_Tqual _tmpD01=Cyc_Absyn_empty_tqual(0U);_tmp523->f1=_tmpD01;});_tmp523->f2=t1;_tmp523;});_tmp522->hd=_tmpD02;});_tmp522->tl=0;_tmp522;});{
struct Cyc_List_List*_tmp524=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp538->tvs,_tmp537);
struct _tuple23 env=({struct _tuple23 _tmp52D;_tmp52D.f1=_tmp524;_tmp52D.f2=r;_tmp52D.f3=te;_tmp52D.f4=flatten;_tmp52D;});
struct Cyc_List_List*_tmp525=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp538->impl))->fields;struct Cyc_List_List*_tmp526=_tmp525;struct Cyc_Absyn_Aggrfield*_tmp52C;struct Cyc_List_List*_tmp52B;if(_tmp526 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp52C=(struct Cyc_Absyn_Aggrfield*)_tmp526->hd;_tmp52B=_tmp526->tl;_LL14: {
# 2635
struct Cyc_List_List*_tmp527=Cyc_Tcutil_flatten_typ_f(& env,_tmp52C);
env.f4=0;{
struct Cyc_List_List*_tmp528=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp52B);
struct Cyc_List_List*_tmp529=({struct Cyc_List_List*_tmp52A=_region_malloc(r,sizeof(*_tmp52A));_tmp52A->hd=_tmp527;_tmp52A->tl=_tmp528;_tmp52A;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp529);};}}_LL10:;};}else{goto _LL9;}case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp517)->f1 == Cyc_Absyn_StructA){_LL7: _tmp539=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp517)->f2;_LL8: {
# 2642
struct _tuple23 env=({struct _tuple23 _tmp535;_tmp535.f1=0;_tmp535.f2=r;_tmp535.f3=te;_tmp535.f4=flatten;_tmp535;});
struct Cyc_List_List*_tmp52E=_tmp539;struct Cyc_Absyn_Aggrfield*_tmp534;struct Cyc_List_List*_tmp533;if(_tmp52E == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp534=(struct Cyc_Absyn_Aggrfield*)_tmp52E->hd;_tmp533=_tmp52E->tl;_LL19: {
# 2646
struct Cyc_List_List*_tmp52F=Cyc_Tcutil_flatten_typ_f(& env,_tmp534);
env.f4=0;{
struct Cyc_List_List*_tmp530=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp533);
struct Cyc_List_List*_tmp531=({struct Cyc_List_List*_tmp532=_region_malloc(r,sizeof(*_tmp532));_tmp532->hd=_tmp52F;_tmp532->tl=_tmp530;_tmp532;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp531);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2652
 goto _LL0;}_LL0:;};}
# 2655
return({struct Cyc_List_List*_tmp53A=_region_malloc(r,sizeof(*_tmp53A));({struct _tuple12*_tmpD04=({struct _tuple12*_tmp53B=_region_malloc(r,sizeof(*_tmp53B));({struct Cyc_Absyn_Tqual _tmpD03=Cyc_Absyn_empty_tqual(0U);_tmp53B->f1=_tmpD03;});_tmp53B->f2=t1;_tmp53B;});_tmp53A->hd=_tmpD04;});_tmp53A->tl=0;_tmp53A;});}
# 2659
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp53C=(void*)t->hd;void*_tmp53D=_tmp53C;switch(*((int*)_tmp53D)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2665
 continue;default: _LL7: _LL8:
# 2667
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2670
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2673
return 1;}
# 2676
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp53E=({struct _tuple0 _tmp542;({void*_tmpD05=Cyc_Tcutil_compress(t1);_tmp542.f1=_tmpD05;});({void*_tmpD06=Cyc_Tcutil_compress(t2);_tmp542.f2=_tmpD06;});_tmp542;});struct _tuple0 _tmp53F=_tmp53E;enum Cyc_Absyn_Size_of _tmp541;enum Cyc_Absyn_Size_of _tmp540;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->tag == 6U){_LL1: _tmp541=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f1)->f2;_tmp540=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp53F.f2)->f2;_LL2:
# 2679
 return(_tmp541 == _tmp540  || _tmp541 == Cyc_Absyn_Int_sz  && _tmp540 == Cyc_Absyn_Long_sz) || 
_tmp541 == Cyc_Absyn_Long_sz  && _tmp540 == Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2687
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2690
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
({void*_tmpD07=Cyc_Tcutil_compress(t1);t1=_tmpD07;});
({void*_tmpD08=Cyc_Tcutil_compress(t2);t2=_tmpD08;});{
struct _tuple0 _tmp543=({struct _tuple0 _tmp576;_tmp576.f1=t1;_tmp576.f2=t2;_tmp576;});struct _tuple0 _tmp544=_tmp543;struct Cyc_Absyn_FnInfo _tmp575;struct Cyc_Absyn_FnInfo _tmp574;struct Cyc_Absyn_Datatypedecl*_tmp573;struct Cyc_Absyn_Datatypefield*_tmp572;struct Cyc_List_List*_tmp571;struct Cyc_Absyn_Datatypedecl*_tmp570;struct Cyc_List_List*_tmp56F;void*_tmp56E;struct Cyc_Absyn_Tqual _tmp56D;void*_tmp56C;union Cyc_Absyn_Constraint*_tmp56B;union Cyc_Absyn_Constraint*_tmp56A;union Cyc_Absyn_Constraint*_tmp569;void*_tmp568;struct Cyc_Absyn_Tqual _tmp567;void*_tmp566;union Cyc_Absyn_Constraint*_tmp565;union Cyc_Absyn_Constraint*_tmp564;union Cyc_Absyn_Constraint*_tmp563;switch(*((int*)_tmp544.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f2)->tag == 5U){_LL1: _tmp56E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f1)->f1).elt_typ;_tmp56D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f1)->f1).elt_tq;_tmp56C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f1)->f1).ptr_atts).rgn;_tmp56B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f1)->f1).ptr_atts).nullable;_tmp56A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f1)->f1).ptr_atts).bounds;_tmp569=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f1)->f1).ptr_atts).zero_term;_tmp568=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f2)->f1).elt_typ;_tmp567=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f2)->f1).elt_tq;_tmp566=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f2)->f1).ptr_atts).rgn;_tmp565=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f2)->f1).ptr_atts).nullable;_tmp564=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f2)->f1).ptr_atts).bounds;_tmp563=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp544.f2)->f1).ptr_atts).zero_term;_LL2:
# 2702
 if(_tmp56D.real_const  && !_tmp567.real_const)
return 0;
# 2705
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp56B,_tmp565) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp56B)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp565))
return 0;
# 2709
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp569,_tmp563) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp569)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp563))
return 0;
# 2713
if((!Cyc_Tcutil_unify(_tmp56C,_tmp566) && !Cyc_Tcenv_region_outlives(te,_tmp56C,_tmp566)) && !
Cyc_Tcutil_subtype(te,assume,_tmp56C,_tmp566))
return 0;
# 2717
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp56A,_tmp564)){
struct _tuple0 _tmp545=({struct _tuple0 _tmp549;({void*_tmpD09=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp56A);_tmp549.f1=_tmpD09;});({void*_tmpD0A=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp564);_tmp549.f2=_tmpD0A;});_tmp549;});struct _tuple0 _tmp546=_tmp545;struct Cyc_Absyn_Exp*_tmp548;struct Cyc_Absyn_Exp*_tmp547;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp546.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp546.f2)->tag == 0U){_LLA: _LLB:
 goto _LL9;}else{goto _LLE;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp546.f2)->tag == 1U){_LLC: _tmp548=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp546.f1)->f1;_tmp547=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp546.f2)->f1;_LLD:
# 2722
 if(!Cyc_Evexp_lte_const_exp(_tmp547,_tmp548))
return 0;
goto _LL9;}else{_LLE: _LLF:
 return 0;}}_LL9:;}
# 2730
if(!_tmp567.real_const  && _tmp56D.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp568)))
return 0;}{
# 2736
int _tmp54A=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp564,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp563);
# 2742
return(_tmp54A  && ({struct Cyc_Tcenv_Tenv*_tmpD0E=te;struct Cyc_List_List*_tmpD0D=({struct Cyc_List_List*_tmp54B=_cycalloc(sizeof(*_tmp54B));({struct _tuple0*_tmpD0B=({struct _tuple0*_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->f1=t1;_tmp54C->f2=t2;_tmp54C;});_tmp54B->hd=_tmpD0B;});_tmp54B->tl=assume;_tmp54B;});void*_tmpD0C=_tmp56E;Cyc_Tcutil_ptrsubtype(_tmpD0E,_tmpD0D,_tmpD0C,_tmp568);}) || Cyc_Tcutil_unify(_tmp56E,_tmp568)) || Cyc_Tcutil_isomorphic(_tmp56E,_tmp568);};}else{goto _LL7;}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp544.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp544.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp544.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3: _tmp573=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp544.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp572=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp544.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp571=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp544.f1)->f1).targs;_tmp570=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp544.f2)->f1).datatype_info).KnownDatatype).val;_tmp56F=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp544.f2)->f1).targs;_LL4:
# 2748
 if(_tmp573 != _tmp570  && Cyc_Absyn_qvar_cmp(_tmp573->name,_tmp570->name)!= 0)return 0;
# 2750
if(({int _tmpD0F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp571);_tmpD0F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56F);}))return 0;
for(0;_tmp571 != 0;(_tmp571=_tmp571->tl,_tmp56F=_tmp56F->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp571->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp56F))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 9U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544.f2)->tag == 9U){_LL5: _tmp575=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544.f1)->f1;_tmp574=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp544.f2)->f1;_LL6:
# 2757
 if(_tmp575.tvars != 0  || _tmp574.tvars != 0){
struct Cyc_List_List*_tmp54D=_tmp575.tvars;
struct Cyc_List_List*_tmp54E=_tmp574.tvars;
if(({int _tmpD10=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp54D);_tmpD10 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp54E);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp54D != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp54D->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp54E))->hd)->kind))return 0;
({struct Cyc_List_List*_tmpD14=({struct Cyc_List_List*_tmp54F=_cycalloc(sizeof(*_tmp54F));({struct _tuple16*_tmpD13=({struct _tuple16*_tmp550=_cycalloc(sizeof(*_tmp550));_tmp550->f1=(struct Cyc_Absyn_Tvar*)_tmp54E->hd;({void*_tmpD12=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp551=_cycalloc(sizeof(*_tmp551));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD11=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp552;_tmp552.tag=2U;_tmp552.f1=(struct Cyc_Absyn_Tvar*)_tmp54D->hd;_tmp552;});_tmp551[0]=_tmpD11;});_tmp551;});_tmp550->f2=_tmpD12;});_tmp550;});_tmp54F->hd=_tmpD13;});_tmp54F->tl=inst;_tmp54F;});inst=_tmpD14;});
_tmp54D=_tmp54D->tl;
_tmp54E=_tmp54E->tl;}
# 2768
if(inst != 0){
_tmp575.tvars=0;
_tmp574.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpD19=te;struct Cyc_List_List*_tmpD18=assume;void*_tmpD17=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp553=_cycalloc(sizeof(*_tmp553));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD15=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp554;_tmp554.tag=9U;_tmp554.f1=_tmp575;_tmp554;});_tmp553[0]=_tmpD15;});_tmp553;});Cyc_Tcutil_subtype(_tmpD19,_tmpD18,_tmpD17,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp555=_cycalloc(sizeof(*_tmp555));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD16=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp556;_tmp556.tag=9U;_tmp556.f1=_tmp574;_tmp556;});_tmp555[0]=_tmpD16;});_tmp555;}));});}};}
# 2775
if(!Cyc_Tcutil_subtype(te,assume,_tmp575.ret_typ,_tmp574.ret_typ))return 0;{
struct Cyc_List_List*_tmp557=_tmp575.args;
struct Cyc_List_List*_tmp558=_tmp574.args;
# 2780
if(({int _tmpD1A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp557);_tmpD1A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp558);}))return 0;
# 2782
for(0;_tmp557 != 0;(_tmp557=_tmp557->tl,_tmp558=_tmp558->tl)){
struct _tuple10 _tmp559=*((struct _tuple10*)_tmp557->hd);struct _tuple10 _tmp55A=_tmp559;struct Cyc_Absyn_Tqual _tmp560;void*_tmp55F;_LL11: _tmp560=_tmp55A.f2;_tmp55F=_tmp55A.f3;_LL12:;{
struct _tuple10 _tmp55B=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp558))->hd);struct _tuple10 _tmp55C=_tmp55B;struct Cyc_Absyn_Tqual _tmp55E;void*_tmp55D;_LL14: _tmp55E=_tmp55C.f2;_tmp55D=_tmp55C.f3;_LL15:;
# 2786
if(_tmp55E.real_const  && !_tmp560.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp55D,_tmp55F))
return 0;};}
# 2790
if(_tmp575.c_varargs != _tmp574.c_varargs)return 0;
if(_tmp575.cyc_varargs != 0  && _tmp574.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp561=*_tmp575.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp562=*_tmp574.cyc_varargs;
# 2795
if((_tmp562.tq).real_const  && !(_tmp561.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp562.type,_tmp561.type))
return 0;}else{
if(_tmp575.cyc_varargs != 0  || _tmp574.cyc_varargs != 0)return 0;}
# 2800
if(!({void*_tmpD1B=(void*)_check_null(_tmp575.effect);Cyc_Tcutil_subset_effect(1,_tmpD1B,(void*)_check_null(_tmp574.effect));}))return 0;
# 2802
if(!Cyc_Tcutil_sub_rgnpo(_tmp575.rgn_po,_tmp574.rgn_po))return 0;
# 2804
if(!Cyc_Tcutil_sub_attributes(_tmp575.attributes,_tmp574.attributes))return 0;
# 2806
if(!Cyc_Tcutil_check_logical_implication(_tmp574.requires_relns,_tmp575.requires_relns))
return 0;
# 2809
if(!Cyc_Tcutil_check_logical_implication(_tmp575.ensures_relns,_tmp574.ensures_relns))
return 0;
# 2812
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2824 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2826
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp577=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp578=_tmp577;struct Cyc_Absyn_Tqual _tmp57E;void*_tmp57D;_LL1: _tmp57E=_tmp578->f1;_tmp57D=_tmp578->f2;_LL2:;{
struct _tuple12*_tmp579=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp57A=_tmp579;struct Cyc_Absyn_Tqual _tmp57C;void*_tmp57B;_LL4: _tmp57C=_tmp57A->f1;_tmp57B=_tmp57A->f2;_LL5:;
# 2833
if(_tmp57E.real_const  && !_tmp57C.real_const)return 0;
# 2835
if((_tmp57C.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp57B))) && 
Cyc_Tcutil_subtype(te,assume,_tmp57D,_tmp57B))
# 2838
continue;
# 2840
if(Cyc_Tcutil_unify(_tmp57D,_tmp57B))
# 2842
continue;
# 2844
if(Cyc_Tcutil_isomorphic(_tmp57D,_tmp57B))
# 2846
continue;
# 2849
return 0;};};}
# 2851
return 1;}
# 2854
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp57F=Cyc_Tcutil_compress(t);void*_tmp580=_tmp57F;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp580)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp580)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2863
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
({void*_tmpD1C=Cyc_Tcutil_compress(t1);t1=_tmpD1C;});
({void*_tmpD1D=Cyc_Tcutil_compress(t2);t2=_tmpD1D;});
# 2869
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp581=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp581)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp581)->f2){case Cyc_Absyn_Int_sz: _LL1: _LL2:
# 2873
 goto _LL4;case Cyc_Absyn_Long_sz: _LL3: _LL4:
# 2875
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 2879
void*_tmp582=t1;void*_tmp5AA;struct Cyc_Absyn_Enumdecl*_tmp5A9;void*_tmp5A8;struct Cyc_Absyn_Tqual _tmp5A7;struct Cyc_Absyn_Exp*_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A5;void*_tmp5A4;struct Cyc_Absyn_Tqual _tmp5A3;void*_tmp5A2;union Cyc_Absyn_Constraint*_tmp5A1;union Cyc_Absyn_Constraint*_tmp5A0;union Cyc_Absyn_Constraint*_tmp59F;switch(*((int*)_tmp582)){case 5U: _LL8: _tmp5A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp582)->f1).elt_typ;_tmp5A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp582)->f1).elt_tq;_tmp5A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp582)->f1).ptr_atts).rgn;_tmp5A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp582)->f1).ptr_atts).nullable;_tmp5A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp582)->f1).ptr_atts).bounds;_tmp59F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp582)->f1).ptr_atts).zero_term;_LL9:
# 2888
{void*_tmp583=t2;void*_tmp595;struct Cyc_Absyn_Tqual _tmp594;void*_tmp593;union Cyc_Absyn_Constraint*_tmp592;union Cyc_Absyn_Constraint*_tmp591;union Cyc_Absyn_Constraint*_tmp590;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->tag == 5U){_LL17: _tmp595=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->f1).elt_typ;_tmp594=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->f1).elt_tq;_tmp593=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->f1).ptr_atts).rgn;_tmp592=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->f1).ptr_atts).nullable;_tmp591=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->f1).ptr_atts).bounds;_tmp590=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->f1).ptr_atts).zero_term;_LL18: {
# 2892
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp584=({struct Cyc_List_List*_tmp58E=_cycalloc(sizeof(*_tmp58E));({struct _tuple0*_tmpD1E=({struct _tuple0*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->f1=t1;_tmp58F->f2=t2;_tmp58F;});_tmp58E->hd=_tmpD1E;});_tmp58E->tl=0;_tmp58E;});
int _tmp585=_tmp594.real_const  || !_tmp5A3.real_const;
# 2906 "tcutil.cyc"
int _tmp586=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp591,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp590);
# 2910
int _tmp587=_tmp585  && (
((_tmp586  && Cyc_Tcutil_ptrsubtype(te,_tmp584,_tmp5A4,_tmp595) || 
Cyc_Tcutil_unify(_tmp5A4,_tmp595)) || Cyc_Tcutil_isomorphic(_tmp5A4,_tmp595)) || Cyc_Tcutil_unify(_tmp595,(void*)& Cyc_Absyn_VoidType_val));
# 2914
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp59F,_tmp590) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp590);
# 2918
int _tmp588=_tmp587?0:((Cyc_Tcutil_bits_only(_tmp5A4) && Cyc_Tcutil_is_char_type(_tmp595)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp590)) && (
_tmp594.real_const  || !_tmp5A3.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5A0,_tmp591);
if(!bounds_ok  && !_tmp588){
struct _tuple0 _tmp589=({struct _tuple0 _tmp58D;({void*_tmpD1F=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5A0);_tmp58D.f1=_tmpD1F;});({void*_tmpD20=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp591);_tmp58D.f2=_tmpD20;});_tmp58D;});struct _tuple0 _tmp58A=_tmp589;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_Absyn_Exp*_tmp58B;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58A.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58A.f2)->tag == 1U){_LL1C: _tmp58C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58A.f1)->f1;_tmp58B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp58A.f2)->f1;_LL1D:
# 2928
 if(Cyc_Evexp_lte_const_exp(_tmp58B,_tmp58C))
bounds_ok=1;
goto _LL1B;}else{goto _LL1E;}}else{_LL1E: _LL1F:
# 2933
 bounds_ok=1;goto _LL1B;}_LL1B:;}{
# 2935
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A1);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp592);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2942
if(((bounds_ok  && zeroterm_ok) && (_tmp587  || _tmp588)) && (
Cyc_Tcutil_unify(_tmp5A2,_tmp593) || Cyc_Tcenv_region_outlives(te,_tmp5A2,_tmp593)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL19: _LL1A:
 goto _LL16;}_LL16:;}
# 2948
return Cyc_Absyn_Unknown_coercion;case 8U: _LLA: _tmp5A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp582)->f1).elt_type;_tmp5A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp582)->f1).tq;_tmp5A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp582)->f1).num_elts;_tmp5A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp582)->f1).zero_term;_LLB:
# 2950
{void*_tmp596=t2;void*_tmp59A;struct Cyc_Absyn_Tqual _tmp599;struct Cyc_Absyn_Exp*_tmp598;union Cyc_Absyn_Constraint*_tmp597;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->tag == 8U){_LL21: _tmp59A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->f1).elt_type;_tmp599=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->f1).tq;_tmp598=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->f1).num_elts;_tmp597=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->f1).zero_term;_LL22: {
# 2952
int okay;
({int _tmpD21=
(((_tmp5A6 != 0  && _tmp598 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5A5,_tmp597)) && 
Cyc_Evexp_lte_const_exp(_tmp598,_tmp5A6)) && 
Cyc_Evexp_lte_const_exp(_tmp5A6,_tmp598);
# 2953
okay=_tmpD21;});
# 2957
return
# 2959
(okay  && Cyc_Tcutil_unify(_tmp5A8,_tmp59A)) && (!_tmp5A7.real_const  || _tmp599.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL23: _LL24:
# 2961
 return Cyc_Absyn_Unknown_coercion;}_LL20:;}
# 2963
return Cyc_Absyn_Unknown_coercion;case 13U: _LLC: _tmp5A9=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp582)->f2;_LLD:
# 2967
{void*_tmp59B=t2;struct Cyc_Absyn_Enumdecl*_tmp59C;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp59B)->tag == 13U){_LL26: _tmp59C=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp59B)->f2;_LL27:
# 2969
 if((_tmp5A9->fields != 0  && _tmp59C->fields != 0) && ({
int _tmpD22=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A9->fields))->v);_tmpD22 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp59C->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL25;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}
# 2975
goto _LLF;case 6U: _LLE: _LLF:
 goto _LL11;case 7U: _LL10: _LL11:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15U: _LL12: _tmp5AA=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp582)->f1;_LL13:
# 2980
{void*_tmp59D=t2;void*_tmp59E;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp59D)->tag == 15U){_LL2B: _tmp59E=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp59D)->f1;_LL2C:
# 2982
 if(Cyc_Tcenv_region_outlives(te,_tmp5AA,_tmp59E))return Cyc_Absyn_No_coercion;
goto _LL2A;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}
# 2986
return Cyc_Absyn_Unknown_coercion;default: _LL14: _LL15:
 return Cyc_Absyn_Unknown_coercion;}_LL7:;};}
# 2992
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5AB=Cyc_Absyn_copy_exp(e);
({void*_tmpD24=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5AC=_cycalloc(sizeof(*_tmp5AC));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpD23=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5AD;_tmp5AD.tag=14U;_tmp5AD.f1=t;_tmp5AD.f2=_tmp5AB;_tmp5AD.f3=0;_tmp5AD.f4=c;_tmp5AD;});_tmp5AC[0]=_tmpD23;});_tmp5AC;});e->r=_tmpD24;});
e->topt=t;}}
# 3000
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5AE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5AF=_tmp5AE;switch(*((int*)_tmp5AF)){case 6U: _LL1: _LL2:
 goto _LL4;case 13U: _LL3: _LL4:
 goto _LL6;case 14U: _LL5: _LL6:
 goto _LL8;case 19U: _LL7: _LL8:
 return 1;case 1U: _LL9: _LLA:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3011
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5B0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5B1=_tmp5B0;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5B1)->tag == 7U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 3020
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5B2=Cyc_Tcutil_compress(t);void*_tmp5B3=_tmp5B2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5B3)->tag == 9U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 3027
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5B4=({struct _tuple0 _tmp5B8;_tmp5B8.f1=t1;_tmp5B8.f2=t2;_tmp5B8;});struct _tuple0 _tmp5B5=_tmp5B4;int _tmp5B7;int _tmp5B6;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5B5.f1)->tag == 7U){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5B5.f2)->tag == 7U){_LL1: _tmp5B7=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5B5.f1)->f1;_tmp5B6=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5B5.f2)->f1;_LL2:
# 3030
 if(_tmp5B7 != 0  && _tmp5B7 != 1)return t1;else{
if(_tmp5B6 != 0  && _tmp5B6 != 1)return t2;else{
if(_tmp5B7 >= _tmp5B6)return t1;else{
return t2;}}}}else{_LL3: _LL4:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5B5.f2)->tag == 7U){_LL5: _LL6:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 goto _LLA;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2 == Cyc_Absyn_Long_sz)goto _LLF;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL11;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL13;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL15;else{goto _LL1B;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2){case Cyc_Absyn_Long_sz: goto _LLF;case Cyc_Absyn_Int_sz: goto _LL13;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2){case Cyc_Absyn_Long_sz: _LLF: _LL10:
# 3040
 goto _LL12;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 3043
 goto _LL16;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL11;case Cyc_Absyn_Int_sz: goto _LL15;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{goto _LL1B;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f1)->f2){case Cyc_Absyn_LongLong_sz: _LLB: _LLC:
# 3038
 goto _LLE;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 3045
 goto _LL1A;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL9: _LLA:
# 3037
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL11: _LL12:
# 3041
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL15: _LL16:
# 3044
 return Cyc_Absyn_uint_typ;default: goto _LL1B;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5B5.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLD: _LLE:
# 3039
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 3046
 return Cyc_Absyn_slong_typ;default: goto _LL1B;}}}else{_LL1B: _LL1C:
 return Cyc_Absyn_sint_typ;}}}}_LL0:;}
# 3058 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
({void*_tmpD25=Cyc_Absyn_compress_kb(c1);c1=_tmpD25;});
({void*_tmpD26=Cyc_Absyn_compress_kb(c2);c2=_tmpD26;});
if(c1 == c2)return 1;{
struct _tuple0 _tmp5B9=({struct _tuple0 _tmp5CF;_tmp5CF.f1=c1;_tmp5CF.f2=c2;_tmp5CF;});struct _tuple0 _tmp5BA=_tmp5B9;struct Cyc_Core_Opt**_tmp5CE;struct Cyc_Absyn_Kind*_tmp5CD;struct Cyc_Core_Opt**_tmp5CC;struct Cyc_Absyn_Kind*_tmp5CB;struct Cyc_Core_Opt**_tmp5CA;struct Cyc_Absyn_Kind*_tmp5C9;struct Cyc_Absyn_Kind*_tmp5C8;struct Cyc_Core_Opt**_tmp5C7;struct Cyc_Core_Opt**_tmp5C6;struct Cyc_Absyn_Kind*_tmp5C5;struct Cyc_Core_Opt**_tmp5C4;struct Cyc_Absyn_Kind*_tmp5C3;struct Cyc_Absyn_Kind*_tmp5C2;struct Cyc_Absyn_Kind*_tmp5C1;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->tag == 0U)switch(*((int*)_tmp5BA.f2)){case 0U: _LL1: _tmp5C2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->f1;_tmp5C1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->f1;_LL2:
 return _tmp5C2 == _tmp5C1;case 1U: goto _LL3;default: _LL9: _tmp5C5=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->f1;_tmp5C4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->f1;_tmp5C3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->f2;_LLA:
# 3071
 if(Cyc_Tcutil_kind_leq(_tmp5C5,_tmp5C3)){
({struct Cyc_Core_Opt*_tmpD27=({struct Cyc_Core_Opt*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->v=c1;_tmp5BE;});*_tmp5C4=_tmpD27;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->tag == 1U){_LL3: _tmp5C6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->f1;_LL4:
# 3064
({struct Cyc_Core_Opt*_tmpD28=({struct Cyc_Core_Opt*_tmp5BB=_cycalloc(sizeof(*_tmp5BB));_tmp5BB->v=c1;_tmp5BB;});*_tmp5C6=_tmpD28;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->tag == 1U){_LL5: _tmp5C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpD29=({struct Cyc_Core_Opt*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->v=c2;_tmp5BC;});*_tmp5C7=_tmpD29;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->tag == 0U){_LL7: _tmp5CA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->f1;_tmp5C9=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->f2;_tmp5C8=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->f1;_LL8:
# 3067
 if(Cyc_Tcutil_kind_leq(_tmp5C8,_tmp5C9)){
({struct Cyc_Core_Opt*_tmpD2A=({struct Cyc_Core_Opt*_tmp5BD=_cycalloc(sizeof(*_tmp5BD));_tmp5BD->v=c2;_tmp5BD;});*_tmp5CA=_tmpD2A;});return 1;}else{
return 0;}}else{_LLB: _tmp5CE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->f1;_tmp5CD=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BA.f1)->f2;_tmp5CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->f1;_tmp5CB=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5BA.f2)->f2;_LLC:
# 3075
 if(Cyc_Tcutil_kind_leq(_tmp5CD,_tmp5CB)){
({struct Cyc_Core_Opt*_tmpD2B=({struct Cyc_Core_Opt*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->v=c1;_tmp5BF;});*_tmp5CC=_tmpD2B;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5CB,_tmp5CD)){
({struct Cyc_Core_Opt*_tmpD2C=({struct Cyc_Core_Opt*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0->v=c2;_tmp5C0;});*_tmp5CE=_tmpD2C;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3084
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3089
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5D4;_tmp5D4.tag=1U;_tmp5D4.f1=(unsigned long)i;({void*_tmp5D2[1U]={& _tmp5D4};({struct _dyneither_ptr _tmpD2D=({const char*_tmp5D3="#%d";_tag_dyneither(_tmp5D3,sizeof(char),4U);});Cyc_aprintf(_tmpD2D,_tag_dyneither(_tmp5D2,sizeof(void*),1U));});});});
return({struct Cyc_Absyn_Tvar*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));({struct _dyneither_ptr*_tmpD2E=({struct _dyneither_ptr*_tmp5D1=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5D1[0]=(struct _dyneither_ptr)s;_tmp5D1;});_tmp5D0->name=_tmpD2E;});_tmp5D0->identity=- 1;_tmp5D0->kind=k;_tmp5D0;});}
# 3096
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5D5=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5D5,sizeof(char),0))== '#';}
# 3101
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5D8;_tmp5D8.tag=0U;_tmp5D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5D6[1U]={& _tmp5D8};({struct _dyneither_ptr _tmpD2F=({const char*_tmp5D7="%s";_tag_dyneither(_tmp5D7,sizeof(char),3U);});Cyc_printf(_tmpD2F,_tag_dyneither(_tmp5D6,sizeof(void*),1U));});});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp5D9=({struct _dyneither_ptr _tmpD30=({const char*_tmp5DE="`";_tag_dyneither(_tmp5DE,sizeof(char),2U);});Cyc_strconcat(_tmpD30,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp5DA=_dyneither_ptr_plus(_tmp5D9,sizeof(char),1);char _tmp5DB=*((char*)_check_dyneither_subscript(_tmp5DA,sizeof(char),0U));char _tmp5DC='t';if(_get_dyneither_size(_tmp5DA,sizeof(char))== 1U  && (_tmp5DB == '\000'  && _tmp5DC != '\000'))_throw_arraybounds();*((char*)_tmp5DA.curr)=_tmp5DC;});
({struct _dyneither_ptr*_tmpD31=({struct _dyneither_ptr*_tmp5DD=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5DD[0]=(struct _dyneither_ptr)_tmp5D9;_tmp5DD;});t->name=_tmpD31;});};}
# 3110
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3112
return({struct _tuple10*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->f1=(*x).f1;_tmp5DF->f2=(*x).f2;_tmp5DF->f3=(*x).f3;_tmp5DF;});}
# 3115
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3122
struct Cyc_List_List*_tmp5E0=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_List_List*_tmpD32=({struct Cyc_List_List*_tmp5E1=_cycalloc(sizeof(*_tmp5E1));_tmp5E1->hd=(void*)atts->hd;_tmp5E1->tl=_tmp5E0;_tmp5E1;});_tmp5E0=_tmpD32;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5E2=_cycalloc(sizeof(*_tmp5E2));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD35=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp5E3;_tmp5E3.tag=9U;({struct Cyc_Absyn_FnInfo _tmpD34=({struct Cyc_Absyn_FnInfo _tmp5E4;_tmp5E4.tvars=fd->tvs;_tmp5E4.effect=fd->effect;_tmp5E4.ret_tqual=fd->ret_tqual;_tmp5E4.ret_typ=fd->ret_type;({struct Cyc_List_List*_tmpD33=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp5E4.args=_tmpD33;});_tmp5E4.c_varargs=fd->c_varargs;_tmp5E4.cyc_varargs=fd->cyc_varargs;_tmp5E4.rgn_po=fd->rgn_po;_tmp5E4.attributes=_tmp5E0;_tmp5E4.requires_clause=fd->requires_clause;_tmp5E4.requires_relns=fd->requires_relns;_tmp5E4.ensures_clause=fd->ensures_clause;_tmp5E4.ensures_relns=fd->ensures_relns;_tmp5E4;});_tmp5E3.f1=_tmpD34;});_tmp5E3;});_tmp5E2[0]=_tmpD35;});_tmp5E2;});}
# 3135
return(void*)_check_null(fd->cached_typ);}
# 3141
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3143
union Cyc_Relations_RelnOp _tmp5E5=*rop;union Cyc_Relations_RelnOp _tmp5E6=_tmp5E5;struct Cyc_Absyn_Vardecl*_tmp5F9;struct Cyc_Absyn_Vardecl*_tmp5F8;switch((_tmp5E6.RNumelts).tag){case 2U: _LL1: _tmp5F8=(_tmp5E6.RVar).val;_LL2: {
# 3145
struct _tuple2 _tmp5E7=*_tmp5F8->name;struct _tuple2 _tmp5E8=_tmp5E7;union Cyc_Absyn_Nmspace _tmp5EF;struct _dyneither_ptr*_tmp5EE;_LL8: _tmp5EF=_tmp5E8.f1;_tmp5EE=_tmp5E8.f2;_LL9:;
if(!((int)((_tmp5EF.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpD36=(struct _dyneither_ptr)*_tmp5EE;Cyc_strcmp(_tmpD36,({const char*_tmp5E9="return_value";_tag_dyneither(_tmp5E9,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpD37=Cyc_Relations_RReturn();*rop=_tmpD37;});
goto _LL0;}{
# 3151
unsigned int c=0U;
{struct Cyc_List_List*_tmp5EA=args;for(0;_tmp5EA != 0;(_tmp5EA=_tmp5EA->tl,c ++)){
struct _tuple10*_tmp5EB=(struct _tuple10*)_tmp5EA->hd;struct _tuple10*_tmp5EC=_tmp5EB;struct _dyneither_ptr*_tmp5ED;_LLB: _tmp5ED=_tmp5EC->f1;_LLC:;
if(_tmp5ED != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5EE,(struct _dyneither_ptr)*_tmp5ED)== 0){
({union Cyc_Relations_RelnOp _tmpD38=Cyc_Relations_RParam(c);*rop=_tmpD38;});
break;}}}}
# 3161
goto _LL0;};}case 3U: _LL3: _tmp5F9=(_tmp5E6.RNumelts).val;_LL4: {
# 3163
struct _tuple2 _tmp5F0=*_tmp5F9->name;struct _tuple2 _tmp5F1=_tmp5F0;union Cyc_Absyn_Nmspace _tmp5F7;struct _dyneither_ptr*_tmp5F6;_LLE: _tmp5F7=_tmp5F1.f1;_tmp5F6=_tmp5F1.f2;_LLF:;
if(!((int)((_tmp5F7.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp5F2=args;for(0;_tmp5F2 != 0;(_tmp5F2=_tmp5F2->tl,c ++)){
struct _tuple10*_tmp5F3=(struct _tuple10*)_tmp5F2->hd;struct _tuple10*_tmp5F4=_tmp5F3;struct _dyneither_ptr*_tmp5F5;_LL11: _tmp5F5=_tmp5F4->f1;_LL12:;
if(_tmp5F5 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5F6,(struct _dyneither_ptr)*_tmp5F5)== 0){
({union Cyc_Relations_RelnOp _tmpD39=Cyc_Relations_RParamNumelts(c);*rop=_tmpD39;});
break;}}}}
# 3175
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3180
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3182
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3186
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3188
if(e == 0)return 0;{
struct Cyc_List_List*_tmp5FA=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp5FA);
return _tmp5FA;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3195
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp5FB=pr;struct Cyc_Absyn_Tqual _tmp5FE;void*_tmp5FD;_LL1: _tmp5FE=_tmp5FB->f1;_tmp5FD=_tmp5FB->f2;_LL2:;
if(_tmp5FD == t)return pr;else{
return({struct _tuple12*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->f1=_tmp5FE;_tmp5FC->f2=t;_tmp5FC;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3202
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3204
return({struct _tuple27*_tmp5FF=_region_malloc(rgn,sizeof(*_tmp5FF));({struct _tuple26*_tmpD3A=({struct _tuple26*_tmp600=_region_malloc(rgn,sizeof(*_tmp600));_tmp600->f1=(*y).f1;_tmp600->f2=(*y).f2;_tmp600;});_tmp5FF->f1=_tmpD3A;});_tmp5FF->f2=(*y).f3;_tmp5FF;});}
# 3206
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3208
struct _tuple10 _tmp601=*orig_arg;struct _tuple10 _tmp602=_tmp601;struct _dyneither_ptr*_tmp606;struct Cyc_Absyn_Tqual _tmp605;void*_tmp604;_LL1: _tmp606=_tmp602.f1;_tmp605=_tmp602.f2;_tmp604=_tmp602.f3;_LL2:;
if(t == _tmp604)return orig_arg;
return({struct _tuple10*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->f1=_tmp606;_tmp603->f2=_tmp605;_tmp603->f3=t;_tmp603;});}
# 3212
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3215
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3220
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3222
return({struct Cyc_Absyn_Exp*_tmp607=_cycalloc(sizeof(*_tmp607));_tmp607->topt=old->topt;_tmp607->r=r;_tmp607->loc=old->loc;_tmp607->annot=old->annot;_tmp607;});}
# 3227
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp608=e->r;void*_tmp609=_tmp608;void*_tmp651;void*_tmp650;struct Cyc_List_List*_tmp64F;struct Cyc_Absyn_Exp*_tmp64E;void*_tmp64D;void*_tmp64C;struct Cyc_Absyn_Exp*_tmp64B;int _tmp64A;enum Cyc_Absyn_Coercion _tmp649;struct Cyc_Absyn_Exp*_tmp648;struct Cyc_Absyn_Exp*_tmp647;struct Cyc_Absyn_Exp*_tmp646;struct Cyc_Absyn_Exp*_tmp645;struct Cyc_Absyn_Exp*_tmp644;struct Cyc_Absyn_Exp*_tmp643;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp640;enum Cyc_Absyn_Primop _tmp63F;struct Cyc_List_List*_tmp63E;switch(*((int*)_tmp609)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp63F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_tmp63E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp609)->f2;_LLC:
# 3236
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp63E)== 1){
struct Cyc_Absyn_Exp*_tmp60A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63E))->hd;
struct Cyc_Absyn_Exp*_tmp60B=Cyc_Tcutil_rsubsexp(r,inst,_tmp60A);
if(_tmp60B == _tmp60A)return e;
return({struct Cyc_Absyn_Exp*_tmpD3D=e;Cyc_Tcutil_copye(_tmpD3D,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpD3C=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp60D;_tmp60D.tag=3U;_tmp60D.f1=_tmp63F;({struct Cyc_List_List*_tmpD3B=({struct Cyc_Absyn_Exp*_tmp60E[1U];_tmp60E[0U]=_tmp60B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60E,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp60D.f2=_tmpD3B;});_tmp60D;});_tmp60C[0]=_tmpD3C;});_tmp60C;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp63E)== 2){
struct Cyc_Absyn_Exp*_tmp60F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63E))->hd;
struct Cyc_Absyn_Exp*_tmp610=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp63E->tl))->hd;
struct Cyc_Absyn_Exp*_tmp611=Cyc_Tcutil_rsubsexp(r,inst,_tmp60F);
struct Cyc_Absyn_Exp*_tmp612=Cyc_Tcutil_rsubsexp(r,inst,_tmp610);
if(_tmp611 == _tmp60F  && _tmp612 == _tmp610)return e;
return({struct Cyc_Absyn_Exp*_tmpD40=e;Cyc_Tcutil_copye(_tmpD40,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp613=_cycalloc(sizeof(*_tmp613));({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpD3F=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp614;_tmp614.tag=3U;_tmp614.f1=_tmp63F;({struct Cyc_List_List*_tmpD3E=({struct Cyc_Absyn_Exp*_tmp615[2U];_tmp615[1U]=_tmp612;_tmp615[0U]=_tmp611;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp615,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp614.f2=_tmpD3E;});_tmp614;});_tmp613[0]=_tmpD3F;});_tmp613;}));});}else{
return({void*_tmp616=0U;({struct _dyneither_ptr _tmpD41=({const char*_tmp617="primop does not have 1 or 2 args!";_tag_dyneither(_tmp617,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD41,_tag_dyneither(_tmp616,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp642=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_tmp641=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp609)->f2;_tmp640=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp609)->f3;_LLE: {
# 3250
struct Cyc_Absyn_Exp*_tmp618=Cyc_Tcutil_rsubsexp(r,inst,_tmp642);
struct Cyc_Absyn_Exp*_tmp619=Cyc_Tcutil_rsubsexp(r,inst,_tmp641);
struct Cyc_Absyn_Exp*_tmp61A=Cyc_Tcutil_rsubsexp(r,inst,_tmp640);
if((_tmp618 == _tmp642  && _tmp619 == _tmp641) && _tmp61A == _tmp640)return e;
return({struct Cyc_Absyn_Exp*_tmpD43=e;Cyc_Tcutil_copye(_tmpD43,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp61B=_cycalloc(sizeof(*_tmp61B));({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpD42=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp61C;_tmp61C.tag=6U;_tmp61C.f1=_tmp618;_tmp61C.f2=_tmp619;_tmp61C.f3=_tmp61A;_tmp61C;});_tmp61B[0]=_tmpD42;});_tmp61B;}));});}case 7U: _LLF: _tmp644=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_tmp643=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp609)->f2;_LL10: {
# 3256
struct Cyc_Absyn_Exp*_tmp61D=Cyc_Tcutil_rsubsexp(r,inst,_tmp644);
struct Cyc_Absyn_Exp*_tmp61E=Cyc_Tcutil_rsubsexp(r,inst,_tmp643);
if(_tmp61D == _tmp644  && _tmp61E == _tmp643)return e;
return({struct Cyc_Absyn_Exp*_tmpD45=e;Cyc_Tcutil_copye(_tmpD45,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp61F=_cycalloc(sizeof(*_tmp61F));({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmpD44=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp620;_tmp620.tag=7U;_tmp620.f1=_tmp61D;_tmp620.f2=_tmp61E;_tmp620;});_tmp61F[0]=_tmpD44;});_tmp61F;}));});}case 8U: _LL11: _tmp646=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_tmp645=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp609)->f2;_LL12: {
# 3261
struct Cyc_Absyn_Exp*_tmp621=Cyc_Tcutil_rsubsexp(r,inst,_tmp646);
struct Cyc_Absyn_Exp*_tmp622=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);
if(_tmp621 == _tmp646  && _tmp622 == _tmp645)return e;
return({struct Cyc_Absyn_Exp*_tmpD47=e;Cyc_Tcutil_copye(_tmpD47,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp623=_cycalloc(sizeof(*_tmp623));({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmpD46=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp624;_tmp624.tag=8U;_tmp624.f1=_tmp621;_tmp624.f2=_tmp622;_tmp624;});_tmp623[0]=_tmpD46;});_tmp623;}));});}case 9U: _LL13: _tmp648=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_tmp647=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp609)->f2;_LL14: {
# 3266
struct Cyc_Absyn_Exp*_tmp625=Cyc_Tcutil_rsubsexp(r,inst,_tmp648);
struct Cyc_Absyn_Exp*_tmp626=Cyc_Tcutil_rsubsexp(r,inst,_tmp647);
if(_tmp625 == _tmp648  && _tmp626 == _tmp647)return e;
return({struct Cyc_Absyn_Exp*_tmpD49=e;Cyc_Tcutil_copye(_tmpD49,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp627=_cycalloc(sizeof(*_tmp627));({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmpD48=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp628;_tmp628.tag=9U;_tmp628.f1=_tmp625;_tmp628.f2=_tmp626;_tmp628;});_tmp627[0]=_tmpD48;});_tmp627;}));});}case 14U: _LL15: _tmp64C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_tmp64B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp609)->f2;_tmp64A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp609)->f3;_tmp649=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp609)->f4;_LL16: {
# 3271
struct Cyc_Absyn_Exp*_tmp629=Cyc_Tcutil_rsubsexp(r,inst,_tmp64B);
void*_tmp62A=Cyc_Tcutil_rsubstitute(r,inst,_tmp64C);
if(_tmp629 == _tmp64B  && _tmp62A == _tmp64C)return e;
return({struct Cyc_Absyn_Exp*_tmpD4B=e;Cyc_Tcutil_copye(_tmpD4B,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpD4A=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp62C;_tmp62C.tag=14U;_tmp62C.f1=_tmp62A;_tmp62C.f2=_tmp629;_tmp62C.f3=_tmp64A;_tmp62C.f4=_tmp649;_tmp62C;});_tmp62B[0]=_tmpD4A;});_tmp62B;}));});}case 17U: _LL17: _tmp64D=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_LL18: {
# 3276
void*_tmp62D=Cyc_Tcutil_rsubstitute(r,inst,_tmp64D);
if(_tmp62D == _tmp64D)return e;
return({struct Cyc_Absyn_Exp*_tmpD4D=e;Cyc_Tcutil_copye(_tmpD4D,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpD4C=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp62F;_tmp62F.tag=17U;_tmp62F.f1=_tmp62D;_tmp62F;});_tmp62E[0]=_tmpD4C;});_tmp62E;}));});}case 18U: _LL19: _tmp64E=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_LL1A: {
# 3280
struct Cyc_Absyn_Exp*_tmp630=Cyc_Tcutil_rsubsexp(r,inst,_tmp64E);
if(_tmp630 == _tmp64E)return e;
return({struct Cyc_Absyn_Exp*_tmpD4F=e;Cyc_Tcutil_copye(_tmpD4F,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp631=_cycalloc(sizeof(*_tmp631));({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmpD4E=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp632;_tmp632.tag=18U;_tmp632.f1=_tmp630;_tmp632;});_tmp631[0]=_tmpD4E;});_tmp631;}));});}case 19U: _LL1B: _tmp650=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_tmp64F=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp609)->f2;_LL1C: {
# 3284
void*_tmp633=Cyc_Tcutil_rsubstitute(r,inst,_tmp650);
if(_tmp633 == _tmp650)return e;
return({struct Cyc_Absyn_Exp*_tmpD51=e;Cyc_Tcutil_copye(_tmpD51,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp634=_cycalloc(sizeof(*_tmp634));({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpD50=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp635;_tmp635.tag=19U;_tmp635.f1=_tmp633;_tmp635.f2=_tmp64F;_tmp635;});_tmp634[0]=_tmpD50;});_tmp634;}));});}case 39U: _LL1D: _tmp651=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp609)->f1;_LL1E: {
# 3288
void*_tmp636=Cyc_Tcutil_rsubstitute(r,inst,_tmp651);
if(_tmp636 == _tmp651)return e;{
# 3291
void*_tmp637=Cyc_Tcutil_compress(_tmp636);void*_tmp638=_tmp637;struct Cyc_Absyn_Exp*_tmp63B;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp638)->tag == 18U){_LL22: _tmp63B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp638)->f1;_LL23:
 return _tmp63B;}else{_LL24: _LL25:
# 3294
 return({struct Cyc_Absyn_Exp*_tmpD53=e;Cyc_Tcutil_copye(_tmpD53,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp639=_cycalloc(sizeof(*_tmp639));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmpD52=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp63A;_tmp63A.tag=39U;_tmp63A.f1=_tmp636;_tmp63A;});_tmp639[0]=_tmpD52;});_tmp639;}));});}_LL21:;};}default: _LL1F: _LL20:
# 3297
 return({void*_tmp63C=0U;({struct _dyneither_ptr _tmpD54=({const char*_tmp63D="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp63D,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD54,_tag_dyneither(_tmp63C,sizeof(void*),0U));});});}_LL0:;}
# 3301
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3304
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3308
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3311
void*_tmp652=f->type;
struct Cyc_Absyn_Exp*_tmp653=f->requires_clause;
void*_tmp654=Cyc_Tcutil_rsubstitute(r,inst,_tmp652);
struct Cyc_Absyn_Exp*_tmp655=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp653);
if(_tmp652 == _tmp654  && _tmp653 == _tmp655)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp656=_cycalloc(sizeof(*_tmp656));_tmp656->name=f->name;_tmp656->tq=f->tq;_tmp656->type=_tmp654;_tmp656->width=f->width;_tmp656->attributes=f->attributes;_tmp656->requires_clause=_tmp655;_tmp656;});}}
# 3321
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3324
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp657=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp658=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp659=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp657);
struct Cyc_List_List*_tmp65A=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp658);
if(_tmp659 == _tmp657  && _tmp65A == _tmp658)return fs;
# 3331
return({struct Cyc_List_List*_tmp65B=_cycalloc(sizeof(*_tmp65B));_tmp65B->hd=_tmp659;_tmp65B->tl=_tmp65A;_tmp65B;});};}
# 3334
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3337
struct _tuple1 _tmp65C=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp65D=_tmp65C;struct Cyc_List_List*_tmp661;struct Cyc_List_List*_tmp660;_LL1: _tmp661=_tmp65D.f1;_tmp660=_tmp65D.f2;_LL2:;{
struct Cyc_List_List*_tmp65E=Cyc_Tcutil_substs(rgn,inst,_tmp661);
struct Cyc_List_List*_tmp65F=Cyc_Tcutil_substs(rgn,inst,_tmp660);
if(_tmp65E == _tmp661  && _tmp65F == _tmp660)
return rgn_po;else{
# 3343
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp65E,_tmp65F);}};}
# 3346
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3349
void*_tmp662=Cyc_Tcutil_compress(t);void*_tmp663=_tmp662;struct Cyc_List_List*_tmp6F4;void*_tmp6F3;void*_tmp6F2;struct Cyc_Absyn_Exp*_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F0;void*_tmp6EF;void*_tmp6EE;void*_tmp6ED;void*_tmp6EC;void*_tmp6EB;enum Cyc_Absyn_AggrKind _tmp6EA;struct Cyc_List_List*_tmp6E9;struct Cyc_List_List*_tmp6E8;struct Cyc_List_List*_tmp6E7;void*_tmp6E6;struct Cyc_Absyn_Tqual _tmp6E5;void*_tmp6E4;struct Cyc_List_List*_tmp6E3;int _tmp6E2;struct Cyc_Absyn_VarargInfo*_tmp6E1;struct Cyc_List_List*_tmp6E0;struct Cyc_List_List*_tmp6DF;struct Cyc_Absyn_Exp*_tmp6DE;struct Cyc_Absyn_Exp*_tmp6DD;void*_tmp6DC;struct Cyc_Absyn_Tqual _tmp6DB;void*_tmp6DA;union Cyc_Absyn_Constraint*_tmp6D9;union Cyc_Absyn_Constraint*_tmp6D8;union Cyc_Absyn_Constraint*_tmp6D7;void*_tmp6D6;struct Cyc_Absyn_Tqual _tmp6D5;struct Cyc_Absyn_Exp*_tmp6D4;union Cyc_Absyn_Constraint*_tmp6D3;unsigned int _tmp6D2;struct _tuple2*_tmp6D1;struct Cyc_List_List*_tmp6D0;struct Cyc_Absyn_Typedefdecl*_tmp6CF;void*_tmp6CE;union Cyc_Absyn_DatatypeFieldInfoU _tmp6CD;struct Cyc_List_List*_tmp6CC;union Cyc_Absyn_DatatypeInfoU _tmp6CB;struct Cyc_List_List*_tmp6CA;union Cyc_Absyn_AggrInfoU _tmp6C9;struct Cyc_List_List*_tmp6C8;struct Cyc_Absyn_Tvar*_tmp6C7;switch(*((int*)_tmp663)){case 2U: _LL1: _tmp6C7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp663)->f1;_LL2: {
# 3352
struct _handler_cons _tmp664;_push_handler(& _tmp664);{int _tmp666=0;if(setjmp(_tmp664.handler))_tmp666=1;if(!_tmp666){{void*_tmp667=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6C7);_npop_handler(0U);return _tmp667;};_pop_handler();}else{void*_tmp665=(void*)_exn_thrown;void*_tmp668=_tmp665;void*_tmp669;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp668)->tag == Cyc_Core_Not_found){_LL3C: _LL3D:
 return t;}else{_LL3E: _tmp669=_tmp668;_LL3F:(int)_rethrow(_tmp669);}_LL3B:;}};}case 11U: _LL3: _tmp6C9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp663)->f1).aggr_info;_tmp6C8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp663)->f1).targs;_LL4: {
# 3355
struct Cyc_List_List*_tmp66A=Cyc_Tcutil_substs(rgn,inst,_tmp6C8);
return _tmp66A == _tmp6C8?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpD56=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp66C;_tmp66C.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpD55=({struct Cyc_Absyn_AggrInfo _tmp66D;_tmp66D.aggr_info=_tmp6C9;_tmp66D.targs=_tmp66A;_tmp66D;});_tmp66C.f1=_tmpD55;});_tmp66C;});_tmp66B[0]=_tmpD56;});_tmp66B;});}case 3U: _LL5: _tmp6CB=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp663)->f1).datatype_info;_tmp6CA=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp663)->f1).targs;_LL6: {
# 3358
struct Cyc_List_List*_tmp66E=Cyc_Tcutil_substs(rgn,inst,_tmp6CA);
return _tmp66E == _tmp6CA?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp66F=_cycalloc(sizeof(*_tmp66F));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpD58=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp670;_tmp670.tag=3U;({struct Cyc_Absyn_DatatypeInfo _tmpD57=({struct Cyc_Absyn_DatatypeInfo _tmp671;_tmp671.datatype_info=_tmp6CB;_tmp671.targs=_tmp66E;_tmp671;});_tmp670.f1=_tmpD57;});_tmp670;});_tmp66F[0]=_tmpD58;});_tmp66F;});}case 4U: _LL7: _tmp6CD=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp663)->f1).field_info;_tmp6CC=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp663)->f1).targs;_LL8: {
# 3362
struct Cyc_List_List*_tmp672=Cyc_Tcutil_substs(rgn,inst,_tmp6CC);
return _tmp672 == _tmp6CC?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp673=_cycalloc(sizeof(*_tmp673));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpD5A=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp674;_tmp674.tag=4U;({struct Cyc_Absyn_DatatypeFieldInfo _tmpD59=({struct Cyc_Absyn_DatatypeFieldInfo _tmp675;_tmp675.field_info=_tmp6CD;_tmp675.targs=_tmp672;_tmp675;});_tmp674.f1=_tmpD59;});_tmp674;});_tmp673[0]=_tmpD5A;});_tmp673;});}case 17U: _LL9: _tmp6D1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp663)->f1;_tmp6D0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp663)->f2;_tmp6CF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp663)->f3;_tmp6CE=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp663)->f4;_LLA: {
# 3366
struct Cyc_List_List*_tmp676=Cyc_Tcutil_substs(rgn,inst,_tmp6D0);
return _tmp676 == _tmp6D0?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp677=_cycalloc(sizeof(*_tmp677));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpD5B=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp678;_tmp678.tag=17U;_tmp678.f1=_tmp6D1;_tmp678.f2=_tmp676;_tmp678.f3=_tmp6CF;_tmp678.f4=_tmp6CE;_tmp678;});_tmp677[0]=_tmpD5B;});_tmp677;});}case 8U: _LLB: _tmp6D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp663)->f1).elt_type;_tmp6D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp663)->f1).tq;_tmp6D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp663)->f1).num_elts;_tmp6D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp663)->f1).zero_term;_tmp6D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp663)->f1).zt_loc;_LLC: {
# 3369
void*_tmp679=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D6);
struct Cyc_Absyn_Exp*_tmp67A=_tmp6D4 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6D4);
return _tmp679 == _tmp6D6  && _tmp67A == _tmp6D4?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp67B=_cycalloc(sizeof(*_tmp67B));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD5D=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp67C;_tmp67C.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpD5C=({struct Cyc_Absyn_ArrayInfo _tmp67D;_tmp67D.elt_type=_tmp679;_tmp67D.tq=_tmp6D5;_tmp67D.num_elts=_tmp67A;_tmp67D.zero_term=_tmp6D3;_tmp67D.zt_loc=_tmp6D2;_tmp67D;});_tmp67C.f1=_tmpD5C;});_tmp67C;});_tmp67B[0]=_tmpD5D;});_tmp67B;});}case 5U: _LLD: _tmp6DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp663)->f1).elt_typ;_tmp6DB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp663)->f1).elt_tq;_tmp6DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp663)->f1).ptr_atts).rgn;_tmp6D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp663)->f1).ptr_atts).nullable;_tmp6D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp663)->f1).ptr_atts).bounds;_tmp6D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp663)->f1).ptr_atts).zero_term;_LLE: {
# 3374
void*_tmp67E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DC);
void*_tmp67F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DA);
union Cyc_Absyn_Constraint*_tmp680=_tmp6D8;
{void*_tmp681=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6D8);void*_tmp682=_tmp681;struct Cyc_Absyn_Exp*_tmp686;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp682)->tag == 1U){_LL41: _tmp686=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp682)->f1;_LL42: {
# 3379
struct Cyc_Absyn_Exp*_tmp683=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp686);
if(_tmp683 != _tmp686)
({union Cyc_Absyn_Constraint*_tmpD5F=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp684=_cycalloc(sizeof(*_tmp684));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpD5E=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp685;_tmp685.tag=1U;_tmp685.f1=_tmp683;_tmp685;});_tmp684[0]=_tmpD5E;});_tmp684;}));_tmp680=_tmpD5F;});
goto _LL40;}}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3385
if((_tmp67E == _tmp6DC  && _tmp67F == _tmp6DA) && _tmp680 == _tmp6D8)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp687=_cycalloc(sizeof(*_tmp687));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpD62=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp688;_tmp688.tag=5U;({struct Cyc_Absyn_PtrInfo _tmpD61=({struct Cyc_Absyn_PtrInfo _tmp689;_tmp689.elt_typ=_tmp67E;_tmp689.elt_tq=_tmp6DB;({struct Cyc_Absyn_PtrAtts _tmpD60=({(_tmp689.ptr_atts).rgn=_tmp67F;(_tmp689.ptr_atts).nullable=_tmp6D9;(_tmp689.ptr_atts).bounds=_tmp680;(_tmp689.ptr_atts).zero_term=_tmp6D7;(_tmp689.ptr_atts).ptrloc=0;_tmp689.ptr_atts;});_tmp689.ptr_atts=_tmpD60;});_tmp689;});_tmp688.f1=_tmpD61;});_tmp688;});_tmp687[0]=_tmpD62;});_tmp687;});}case 9U: _LLF: _tmp6E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).tvars;_tmp6E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).effect;_tmp6E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).ret_tqual;_tmp6E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).ret_typ;_tmp6E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).args;_tmp6E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).c_varargs;_tmp6E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).cyc_varargs;_tmp6E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).rgn_po;_tmp6DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).attributes;_tmp6DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).requires_clause;_tmp6DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp663)->f1).ensures_clause;_LL10:
# 3391
{struct Cyc_List_List*_tmp68A=_tmp6E7;for(0;_tmp68A != 0;_tmp68A=_tmp68A->tl){
({struct Cyc_List_List*_tmpD66=({struct Cyc_List_List*_tmp68B=_region_malloc(rgn,sizeof(*_tmp68B));({struct _tuple16*_tmpD65=({struct _tuple16*_tmp68C=_region_malloc(rgn,sizeof(*_tmp68C));_tmp68C->f1=(struct Cyc_Absyn_Tvar*)_tmp68A->hd;({void*_tmpD64=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp68D=_cycalloc(sizeof(*_tmp68D));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD63=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp68E;_tmp68E.tag=2U;_tmp68E.f1=(struct Cyc_Absyn_Tvar*)_tmp68A->hd;_tmp68E;});_tmp68D[0]=_tmpD63;});_tmp68D;});_tmp68C->f2=_tmpD64;});_tmp68C;});_tmp68B->hd=_tmpD65;});_tmp68B->tl=inst;_tmp68B;});inst=_tmpD66;});}}{
struct _tuple1 _tmp68F=({struct _RegionHandle*_tmpD68=rgn;struct _RegionHandle*_tmpD67=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpD68,_tmpD67,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6E3));});
# 3393
struct _tuple1 _tmp690=_tmp68F;struct Cyc_List_List*_tmp6A1;struct Cyc_List_List*_tmp6A0;_LL46: _tmp6A1=_tmp690.f1;_tmp6A0=_tmp690.f2;_LL47:;{
# 3395
struct Cyc_List_List*_tmp691=_tmp6E3;
struct Cyc_List_List*_tmp692=Cyc_Tcutil_substs(rgn,inst,_tmp6A0);
if(_tmp692 != _tmp6A0)
({struct Cyc_List_List*_tmpD69=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp6E3,_tmp692);_tmp691=_tmpD69;});{
void*eff2;
if(_tmp6E6 == 0)
eff2=0;else{
# 3403
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E6);
if(new_eff == _tmp6E6)
eff2=_tmp6E6;else{
# 3407
eff2=new_eff;}}{
# 3409
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp6E1 == 0)
cyc_varargs2=0;else{
# 3413
struct Cyc_Absyn_VarargInfo _tmp693=*_tmp6E1;struct Cyc_Absyn_VarargInfo _tmp694=_tmp693;struct _dyneither_ptr*_tmp69A;struct Cyc_Absyn_Tqual _tmp699;void*_tmp698;int _tmp697;_LL49: _tmp69A=_tmp694.name;_tmp699=_tmp694.tq;_tmp698=_tmp694.type;_tmp697=_tmp694.inject;_LL4A:;{
void*_tmp695=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp698);
if(_tmp695 == _tmp698)cyc_varargs2=_tmp6E1;else{
# 3417
({struct Cyc_Absyn_VarargInfo*_tmpD6A=({struct Cyc_Absyn_VarargInfo*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->name=_tmp69A;_tmp696->tq=_tmp699;_tmp696->type=_tmp695;_tmp696->inject=_tmp697;_tmp696;});cyc_varargs2=_tmpD6A;});}};}{
# 3419
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp6E0);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6DE);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6DD);
struct Cyc_List_List*_tmp69B=Cyc_Tcutil_extract_relns(_tmp691,req2);
struct Cyc_List_List*_tmp69C=Cyc_Tcutil_extract_relns(_tmp691,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp69D=_cycalloc(sizeof(*_tmp69D));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD6D=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp69E;_tmp69E.tag=9U;({struct Cyc_Absyn_FnInfo _tmpD6C=({struct Cyc_Absyn_FnInfo _tmp69F;_tmp69F.tvars=_tmp6E7;_tmp69F.effect=eff2;_tmp69F.ret_tqual=_tmp6E5;({void*_tmpD6B=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E4);_tmp69F.ret_typ=_tmpD6B;});_tmp69F.args=_tmp691;_tmp69F.c_varargs=_tmp6E2;_tmp69F.cyc_varargs=cyc_varargs2;_tmp69F.rgn_po=rgn_po2;_tmp69F.attributes=_tmp6DF;_tmp69F.requires_clause=req2;_tmp69F.requires_relns=_tmp69B;_tmp69F.ensures_clause=ens2;_tmp69F.ensures_relns=_tmp69C;_tmp69F;});_tmp69E.f1=_tmpD6C;});_tmp69E;});_tmp69D[0]=_tmpD6D;});_tmp69D;});};};};};};case 10U: _LL11: _tmp6E8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp663)->f1;_LL12: {
# 3429
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6A2=_tmp6E8;for(0;_tmp6A2 != 0;_tmp6A2=_tmp6A2->tl){
void*_tmp6A3=(*((struct _tuple12*)_tmp6A2->hd)).f2;
void*_tmp6A4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6A3);
if(_tmp6A3 != _tmp6A4)
change=1;
# 3437
({struct Cyc_List_List*_tmpD6E=({struct Cyc_List_List*_tmp6A5=_region_malloc(rgn,sizeof(*_tmp6A5));_tmp6A5->hd=_tmp6A4;_tmp6A5->tl=ts2;_tmp6A5;});ts2=_tmpD6E;});}}
# 3439
if(!change)
return t;{
struct Cyc_List_List*_tmp6A6=({struct Cyc_List_List*_tmpD6F=_tmp6E8;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpD6F,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpD70=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6A8;_tmp6A8.tag=10U;_tmp6A8.f1=_tmp6A6;_tmp6A8;});_tmp6A7[0]=_tmpD70;});_tmp6A7;});};}case 12U: _LL13: _tmp6EA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp663)->f1;_tmp6E9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp663)->f2;_LL14: {
# 3444
struct Cyc_List_List*_tmp6A9=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp6E9);
if(_tmp6E9 == _tmp6A9)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpD71=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6AB;_tmp6AB.tag=12U;_tmp6AB.f1=_tmp6EA;_tmp6AB.f2=_tmp6A9;_tmp6AB;});_tmp6AA[0]=_tmpD71;});_tmp6AA;});}case 1U: _LL15: _tmp6EB=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp663)->f2;_LL16:
# 3448
 if(_tmp6EB != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EB);else{
return t;}case 15U: _LL17: _tmp6EC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp663)->f1;_LL18: {
# 3451
void*_tmp6AC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EC);
return _tmp6AC == _tmp6EC?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpD72=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6AE;_tmp6AE.tag=15U;_tmp6AE.f1=_tmp6AC;_tmp6AE;});_tmp6AD[0]=_tmpD72;});_tmp6AD;});}case 16U: _LL19: _tmp6EE=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp663)->f1;_tmp6ED=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp663)->f2;_LL1A: {
# 3454
void*_tmp6AF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EE);
void*_tmp6B0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6ED);
return _tmp6AF == _tmp6EE  && _tmp6B0 == _tmp6ED?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpD73=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp6B2;_tmp6B2.tag=16U;_tmp6B2.f1=_tmp6AF;_tmp6B2.f2=_tmp6B0;_tmp6B2;});_tmp6B1[0]=_tmpD73;});_tmp6B1;});}case 19U: _LL1B: _tmp6EF=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp663)->f1;_LL1C: {
# 3458
void*_tmp6B3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EF);
return _tmp6B3 == _tmp6EF?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6B4=_cycalloc(sizeof(*_tmp6B4));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpD74=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6B5;_tmp6B5.tag=19U;_tmp6B5.f1=_tmp6B3;_tmp6B5;});_tmp6B4[0]=_tmpD74;});_tmp6B4;});}case 18U: _LL1D: _tmp6F0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp663)->f1;_LL1E: {
# 3461
struct Cyc_Absyn_Exp*_tmp6B6=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6F0);
return _tmp6B6 == _tmp6F0?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpD75=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6B8;_tmp6B8.tag=18U;_tmp6B8.f1=_tmp6B6;_tmp6B8;});_tmp6B7[0]=_tmpD75;});_tmp6B7;});}case 27U: _LL1F: _tmp6F1=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp663)->f1;_LL20: {
# 3464
struct Cyc_Absyn_Exp*_tmp6B9=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6F1);
return _tmp6B9 == _tmp6F1?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpD76=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp6BB;_tmp6BB.tag=27U;_tmp6BB.f1=_tmp6B9;_tmp6BB;});_tmp6BA[0]=_tmpD76;});_tmp6BA;});}case 13U: _LL21: _LL22:
 goto _LL24;case 14U: _LL23: _LL24:
 goto _LL26;case 0U: _LL25: _LL26:
 goto _LL28;case 6U: _LL27: _LL28:
 goto _LL2A;case 7U: _LL29: _LL2A:
 goto _LL2C;case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 28U: _LL2F: _LL30:
 goto _LL32;case 20U: _LL31: _LL32:
 return t;case 25U: _LL33: _tmp6F2=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp663)->f1;_LL34: {
# 3476
void*_tmp6BC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F2);
return _tmp6BC == _tmp6F2?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD77=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6BE;_tmp6BE.tag=25U;_tmp6BE.f1=_tmp6BC;_tmp6BE;});_tmp6BD[0]=_tmpD77;});_tmp6BD;});}case 23U: _LL35: _tmp6F3=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp663)->f1;_LL36: {
# 3479
void*_tmp6BF=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F3);
return _tmp6BF == _tmp6F3?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD78=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp6C1;_tmp6C1.tag=23U;_tmp6C1.f1=_tmp6BF;_tmp6C1;});_tmp6C0[0]=_tmpD78;});_tmp6C0;});}case 24U: _LL37: _tmp6F4=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp663)->f1;_LL38: {
# 3482
struct Cyc_List_List*_tmp6C2=Cyc_Tcutil_substs(rgn,inst,_tmp6F4);
return _tmp6C2 == _tmp6F4?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6C3=_cycalloc(sizeof(*_tmp6C3));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD79=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6C4;_tmp6C4.tag=24U;_tmp6C4.f1=_tmp6C2;_tmp6C4;});_tmp6C3[0]=_tmpD79;});_tmp6C3;});}default: _LL39: _LL3A:
({void*_tmp6C5=0U;({struct _dyneither_ptr _tmpD7A=({const char*_tmp6C6="found typedecltype in rsubs";_tag_dyneither(_tmp6C6,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD7A,_tag_dyneither(_tmp6C5,sizeof(void*),0U));});});}_LL0:;}
# 3488
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3491
if(ts == 0)
return 0;{
void*_tmp6F5=(void*)ts->hd;
struct Cyc_List_List*_tmp6F6=ts->tl;
void*_tmp6F7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F5);
struct Cyc_List_List*_tmp6F8=Cyc_Tcutil_substs(rgn,inst,_tmp6F6);
if(_tmp6F5 == _tmp6F7  && _tmp6F6 == _tmp6F8)
return ts;
return({struct Cyc_List_List*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->hd=_tmp6F7;_tmp6F9->tl=_tmp6F8;_tmp6F9;});};}
# 3502
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3509
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp6FA=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp6FB=_cycalloc(sizeof(*_tmp6FB));_tmp6FB->f1=tv;({void*_tmpD7C=({struct Cyc_Core_Opt*_tmpD7B=_tmp6FA;Cyc_Absyn_new_evar(_tmpD7B,({struct Cyc_Core_Opt*_tmp6FC=_cycalloc(sizeof(*_tmp6FC));_tmp6FC->v=s;_tmp6FC;}));});_tmp6FB->f2=_tmpD7C;});_tmp6FB;});}
# 3514
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3516
struct _tuple17*_tmp6FD=env;struct Cyc_List_List*_tmp702;struct _RegionHandle*_tmp701;_LL1: _tmp702=_tmp6FD->f1;_tmp701=_tmp6FD->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp6FE=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp6FF=_region_malloc(_tmp701,sizeof(*_tmp6FF));_tmp6FF->f1=tv;({void*_tmpD7E=({struct Cyc_Core_Opt*_tmpD7D=_tmp6FE;Cyc_Absyn_new_evar(_tmpD7D,({struct Cyc_Core_Opt*_tmp700=_cycalloc(sizeof(*_tmp700));_tmp700->v=_tmp702;_tmp700;}));});_tmp6FF->f2=_tmpD7E;});_tmp6FF;});};}
# 3526
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3530
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp707;_tmp707.tag=0U;({struct _dyneither_ptr _tmpD7F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));_tmp707.f1=_tmpD7F;});({struct Cyc_String_pa_PrintArg_struct _tmp706;_tmp706.tag=0U;({struct _dyneither_ptr _tmpD80=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp706.f1=_tmpD80;});({struct Cyc_String_pa_PrintArg_struct _tmp705;_tmp705.tag=0U;_tmp705.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp703[3U]={& _tmp705,& _tmp706,& _tmp707};({unsigned int _tmpD82=loc;struct _dyneither_ptr _tmpD81=({const char*_tmp704="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp704,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpD82,_tmpD81,_tag_dyneither(_tmp703,sizeof(void*),3U));});});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp708=0U;({struct _dyneither_ptr _tmpD83=({const char*_tmp709="same type variable has different identity!";_tag_dyneither(_tmp709,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD83,_tag_dyneither(_tmp708,sizeof(void*),0U));});});}
return tvs;}}}
# 3544
({int _tmpD84=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpD84;});
return({struct Cyc_List_List*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->hd=tv;_tmp70A->tl=tvs;_tmp70A;});}
# 3550
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3552
if(tv->identity == - 1)
({void*_tmp70B=0U;({struct _dyneither_ptr _tmpD85=({const char*_tmp70C="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp70C,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD85,_tag_dyneither(_tmp70B,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3556
struct Cyc_Absyn_Tvar*_tmp70D=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp70D->identity == - 1)
({void*_tmp70E=0U;({struct _dyneither_ptr _tmpD86=({const char*_tmp70F="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp70F,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD86,_tag_dyneither(_tmp70E,sizeof(void*),0U));});});
if(_tmp70D->identity == tv->identity)
return tvs;}}
# 3563
return({struct Cyc_List_List*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->hd=tv;_tmp710->tl=tvs;_tmp710;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3569
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3574
if(tv->identity == - 1)
({void*_tmp711=0U;({struct _dyneither_ptr _tmpD87=({const char*_tmp712="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp712,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD87,_tag_dyneither(_tmp711,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3578
struct _tuple28*_tmp713=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp714=_tmp713;struct Cyc_Absyn_Tvar*_tmp718;int*_tmp717;_LL1: _tmp718=_tmp714->f1;_tmp717=(int*)& _tmp714->f2;_LL2:;
if(_tmp718->identity == - 1)
({void*_tmp715=0U;({struct _dyneither_ptr _tmpD88=({const char*_tmp716="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp716,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD88,_tag_dyneither(_tmp715,sizeof(void*),0U));});});
if(_tmp718->identity == tv->identity){
*_tmp717=*_tmp717  || b;
return tvs;}}}
# 3586
return({struct Cyc_List_List*_tmp719=_region_malloc(r,sizeof(*_tmp719));({struct _tuple28*_tmpD89=({struct _tuple28*_tmp71A=_region_malloc(r,sizeof(*_tmp71A));_tmp71A->f1=tv;_tmp71A->f2=b;_tmp71A;});_tmp719->hd=_tmpD89;});_tmp719->tl=tvs;_tmp719;});}
# 3590
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3592
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp71D;_tmp71D.tag=0U;({struct _dyneither_ptr _tmpD8A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmp71D.f1=_tmpD8A;});({void*_tmp71B[1U]={& _tmp71D};({struct _dyneither_ptr _tmpD8B=({const char*_tmp71C="bound tvar id for %s is NULL";_tag_dyneither(_tmp71C,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD8B,_tag_dyneither(_tmp71B,sizeof(void*),1U));});});});
return({struct Cyc_List_List*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->hd=tv;_tmp71E->tl=tvs;_tmp71E;});}struct _tuple29{void*f1;int f2;};
# 3601
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3604
void*_tmp71F=Cyc_Tcutil_compress(e);void*_tmp720=_tmp71F;int _tmp72A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp720)->tag == 1U){_LL1: _tmp72A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp720)->f3;_LL2:
# 3606
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp721=(struct _tuple29*)es2->hd;struct _tuple29*_tmp722=_tmp721;void*_tmp727;int*_tmp726;_LL6: _tmp727=_tmp722->f1;_tmp726=(int*)& _tmp722->f2;_LL7:;{
void*_tmp723=Cyc_Tcutil_compress(_tmp727);void*_tmp724=_tmp723;int _tmp725;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp724)->tag == 1U){_LL9: _tmp725=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp724)->f3;_LLA:
# 3610
 if(_tmp72A == _tmp725){
if(b != *_tmp726)*_tmp726=1;
return es;}
# 3614
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3618
return({struct Cyc_List_List*_tmp728=_region_malloc(r,sizeof(*_tmp728));({struct _tuple29*_tmpD8C=({struct _tuple29*_tmp729=_region_malloc(r,sizeof(*_tmp729));_tmp729->f1=e;_tmp729->f2=b;_tmp729;});_tmp728->hd=_tmpD8C;});_tmp728->tl=es;_tmp728;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3623
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3626
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3635
if(!present)({struct Cyc_List_List*_tmpD8D=({struct Cyc_List_List*_tmp72B=_region_malloc(rgn,sizeof(*_tmp72B));_tmp72B->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp72B->tl=r;_tmp72B;});r=_tmpD8D;});}
# 3637
({struct Cyc_List_List*_tmpD8E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);r=_tmpD8E;});
return r;}
# 3642
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3646
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp72C=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp72D=_tmp72C;struct Cyc_Absyn_Tvar*_tmp730;int _tmp72F;_LL1: _tmp730=_tmp72D.f1;_tmp72F=_tmp72D.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp730->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3656
if(!present)({struct Cyc_List_List*_tmpD8F=({struct Cyc_List_List*_tmp72E=_region_malloc(r,sizeof(*_tmp72E));_tmp72E->hd=(struct _tuple28*)tvs->hd;_tmp72E->tl=res;_tmp72E;});res=_tmpD8F;});};}
# 3658
({struct Cyc_List_List*_tmpD90=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);res=_tmpD90;});
return res;}
# 3662
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3664
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp733;_tmp733.tag=0U;_tmp733.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp731[1U]={& _tmp733};({unsigned int _tmpD92=loc;struct _dyneither_ptr _tmpD91=({const char*_tmp732="bitfield %s does not have constant width";_tag_dyneither(_tmp732,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpD92,_tmpD91,_tag_dyneither(_tmp731,sizeof(void*),1U));});});});else{
# 3669
struct _tuple14 _tmp734=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp735=_tmp734;unsigned int _tmp739;int _tmp738;_LL1: _tmp739=_tmp735.f1;_tmp738=_tmp735.f2;_LL2:;
if(!_tmp738)
({void*_tmp736=0U;({unsigned int _tmpD94=loc;struct _dyneither_ptr _tmpD93=({const char*_tmp737="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp737,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpD94,_tmpD93,_tag_dyneither(_tmp736,sizeof(void*),0U));});});
w=_tmp739;}{
# 3674
void*_tmp73A=Cyc_Tcutil_compress(field_typ);void*_tmp73B=_tmp73A;enum Cyc_Absyn_Size_of _tmp749;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73B)->tag == 6U){_LL4: _tmp749=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp73B)->f2;_LL5:
# 3677
{enum Cyc_Absyn_Size_of _tmp73C=_tmp749;switch(_tmp73C){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > 8)({void*_tmp73D=0U;({unsigned int _tmpD96=loc;struct _dyneither_ptr _tmpD95=({const char*_tmp73E="bitfield larger than type";_tag_dyneither(_tmp73E,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpD96,_tmpD95,_tag_dyneither(_tmp73D,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > 16)({void*_tmp73F=0U;({unsigned int _tmpD98=loc;struct _dyneither_ptr _tmpD97=({const char*_tmp740="bitfield larger than type";_tag_dyneither(_tmp740,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpD98,_tmpD97,_tag_dyneither(_tmp73F,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 3682
 if(w > 32)({void*_tmp741=0U;({unsigned int _tmpD9A=loc;struct _dyneither_ptr _tmpD99=({const char*_tmp742="bitfield larger than type";_tag_dyneither(_tmp742,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpD9A,_tmpD99,_tag_dyneither(_tmp741,sizeof(void*),0U));});});goto _LL8;default: _LL11: _LL12:
# 3684
 if(w > 64)({void*_tmp743=0U;({unsigned int _tmpD9C=loc;struct _dyneither_ptr _tmpD9B=({const char*_tmp744="bitfield larger than type";_tag_dyneither(_tmp744,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpD9C,_tmpD9B,_tag_dyneither(_tmp743,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3686
goto _LL3;}else{_LL6: _LL7:
# 3688
({struct Cyc_String_pa_PrintArg_struct _tmp748;_tmp748.tag=0U;({struct _dyneither_ptr _tmpD9D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));_tmp748.f1=_tmpD9D;});({struct Cyc_String_pa_PrintArg_struct _tmp747;_tmp747.tag=0U;_tmp747.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp745[2U]={& _tmp747,& _tmp748};({unsigned int _tmpD9F=loc;struct _dyneither_ptr _tmpD9E=({const char*_tmp746="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp746,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpD9F,_tmpD9E,_tag_dyneither(_tmp745,sizeof(void*),2U));});});});});
goto _LL3;}_LL3:;};}}
# 3695
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp74A=(void*)atts->hd;void*_tmp74B=_tmp74A;switch(*((int*)_tmp74B)){case 7U: _LL1: _LL2:
 continue;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp74F;_tmp74F.tag=0U;_tmp74F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp74E;_tmp74E.tag=0U;({struct _dyneither_ptr _tmpDA0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));_tmp74E.f1=_tmpDA0;});({void*_tmp74C[2U]={& _tmp74E,& _tmp74F};({unsigned int _tmpDA2=loc;struct _dyneither_ptr _tmpDA1=({const char*_tmp74D="bad attribute %s on member %s";_tag_dyneither(_tmp74D,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpDA2,_tmpDA1,_tag_dyneither(_tmp74C,sizeof(void*),2U));});});});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3723
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp750=t;struct Cyc_Absyn_Typedefdecl*_tmp754;void*_tmp753;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp750)->tag == 17U){_LL1: _tmp754=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp750)->f3;_tmp753=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp750)->f4;_LL2:
# 3726
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp754))->tq).real_const  || (_tmp754->tq).print_const){
if(declared_const)({void*_tmp751=0U;({unsigned int _tmpDA4=loc;struct _dyneither_ptr _tmpDA3=({const char*_tmp752="extra const";_tag_dyneither(_tmp752,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpDA4,_tmpDA3,_tag_dyneither(_tmp751,sizeof(void*),0U));});});
return 1;}
# 3731
if((unsigned int)_tmp753)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp753);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3738
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp755=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp756=_tmp755;void*_tmp75A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp756)->tag == 5U){_LL1: _tmp75A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp756)->f1).ptr_atts).rgn;_LL2:
# 3743
{void*_tmp757=Cyc_Tcutil_compress(_tmp75A);void*_tmp758=_tmp757;struct Cyc_Absyn_Tvar*_tmp759;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp758)->tag == 2U){_LL6: _tmp759=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp758)->f1;_LL7:
# 3745
 return Cyc_Absyn_tvar_cmp(tvar,_tmp759)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3748
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3754
return 1;}
return 0;}
# 3758
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3761
void*_tmp75B=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp75C=_tmp75B;switch(*((int*)_tmp75C)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75C)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp75C)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75C)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp75C)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3770
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3773
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3779
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3785
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3789
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3792
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp760;_tmp760.tag=0U;({struct _dyneither_ptr _tmpDA5=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp760.f1=_tmpDA5;});({struct Cyc_String_pa_PrintArg_struct _tmp75F;_tmp75F.tag=0U;_tmp75F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp75D[2U]={& _tmp75F,& _tmp760};({unsigned int _tmpDA7=loc;struct _dyneither_ptr _tmpDA6=({const char*_tmp75E="%s clause has type %s instead of integral type";_tag_dyneither(_tmp75E,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpDA7,_tmpDA6,_tag_dyneither(_tmp75D,sizeof(void*),2U));});});});});
({struct Cyc_Tcutil_CVTEnv _tmpDA8=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);cvtenv=_tmpDA8;});
({struct Cyc_List_List*_tmpDA9=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);relns=_tmpDA9;});
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp764;_tmp764.tag=0U;({struct _dyneither_ptr _tmpDAA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));_tmp764.f1=_tmpDAA;});({struct Cyc_String_pa_PrintArg_struct _tmp763;_tmp763.tag=0U;_tmp763.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp761[2U]={& _tmp763,& _tmp764};({unsigned int _tmpDAC=clause->loc;struct _dyneither_ptr _tmpDAB=({const char*_tmp762="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp762,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpDAC,_tmpDAB,_tag_dyneither(_tmp761,sizeof(void*),2U));});});});});}
# 3804
return({struct _tuple30 _tmp765;_tmp765.f1=cvtenv;_tmp765.f2=relns;_tmp765;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3835 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3843
{void*_tmp766=Cyc_Tcutil_compress(t);void*_tmp767=_tmp766;struct Cyc_List_List*_tmp907;void*_tmp906;void*_tmp905;void*_tmp904;void*_tmp903;void*_tmp902;struct _tuple2*_tmp901;struct Cyc_List_List**_tmp900;struct Cyc_Absyn_Typedefdecl**_tmp8FF;void**_tmp8FE;union Cyc_Absyn_AggrInfoU*_tmp8FD;struct Cyc_List_List**_tmp8FC;enum Cyc_Absyn_AggrKind _tmp8FB;struct Cyc_List_List*_tmp8FA;struct Cyc_List_List*_tmp8F9;struct Cyc_List_List**_tmp8F8;void**_tmp8F7;struct Cyc_Absyn_Tqual*_tmp8F6;void*_tmp8F5;struct Cyc_List_List*_tmp8F4;int _tmp8F3;struct Cyc_Absyn_VarargInfo*_tmp8F2;struct Cyc_List_List*_tmp8F1;struct Cyc_List_List*_tmp8F0;struct Cyc_Absyn_Exp*_tmp8EF;struct Cyc_List_List**_tmp8EE;struct Cyc_Absyn_Exp*_tmp8ED;struct Cyc_List_List**_tmp8EC;void*_tmp8EB;struct Cyc_Absyn_Tqual*_tmp8EA;struct Cyc_Absyn_Exp**_tmp8E9;union Cyc_Absyn_Constraint*_tmp8E8;unsigned int _tmp8E7;struct Cyc_Absyn_Exp*_tmp8E6;struct Cyc_Absyn_Exp*_tmp8E5;void*_tmp8E4;void*_tmp8E3;struct Cyc_Absyn_Tqual*_tmp8E2;void*_tmp8E1;union Cyc_Absyn_Constraint*_tmp8E0;union Cyc_Absyn_Constraint*_tmp8DF;union Cyc_Absyn_Constraint*_tmp8DE;union Cyc_Absyn_DatatypeFieldInfoU*_tmp8DD;struct Cyc_List_List*_tmp8DC;union Cyc_Absyn_DatatypeInfoU*_tmp8DB;struct Cyc_List_List**_tmp8DA;struct _tuple2*_tmp8D9;struct Cyc_Absyn_Enumdecl**_tmp8D8;struct Cyc_List_List*_tmp8D7;void*_tmp8D6;void***_tmp8D5;struct Cyc_Absyn_Tvar*_tmp8D4;struct Cyc_Core_Opt**_tmp8D3;void**_tmp8D2;switch(*((int*)_tmp767)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp8D3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp767)->f1;_tmp8D2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp767)->f2;_LL4:
# 3847
 if(*_tmp8D3 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp8D3))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp8D3))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpDAD=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp8D3=_tmpDAD;});
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3852
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp8D2=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3855
*_tmp8D2=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp768=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp76B=_cycalloc(sizeof(*_tmp76B));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDAE=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp76C;_tmp76C.tag=2U;_tmp76C.f1=0;_tmp76C.f2=expected_kind;_tmp76C;});_tmp76B[0]=_tmpDAE;});_tmp76B;}));
({void*_tmpDB0=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp769=_cycalloc(sizeof(*_tmp769));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDAF=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp76A;_tmp76A.tag=2U;_tmp76A.f1=_tmp768;_tmp76A;});_tmp769[0]=_tmpDAF;});_tmp769;});*_tmp8D2=_tmpDB0;});
_tmp8D4=_tmp768;goto _LL6;}else{
# 3861
({struct Cyc_List_List*_tmpDB1=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpDB1;});}}
# 3863
goto _LL0;case 2U: _LL5: _tmp8D4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp767)->f1;_LL6:
# 3865
{void*_tmp76D=Cyc_Absyn_compress_kb(_tmp8D4->kind);void*_tmp76E=_tmp76D;struct Cyc_Core_Opt**_tmp772;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp76E)->tag == 1U){_LL3C: _tmp772=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp76E)->f1;_LL3D:
# 3867
({struct Cyc_Core_Opt*_tmpDB4=({struct Cyc_Core_Opt*_tmp76F=_cycalloc(sizeof(*_tmp76F));({void*_tmpDB3=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp770=_cycalloc(sizeof(*_tmp770));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDB2=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp771;_tmp771.tag=2U;_tmp771.f1=0;_tmp771.f2=expected_kind;_tmp771;});_tmp770[0]=_tmpDB2;});_tmp770;});_tmp76F->v=_tmpDB3;});_tmp76F;});*_tmp772=_tmpDB4;});goto _LL3B;}else{_LL3E: _LL3F:
 goto _LL3B;}_LL3B:;}
# 3872
({struct Cyc_List_List*_tmpDB5=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8D4);cvtenv.kind_env=_tmpDB5;});
# 3875
({struct Cyc_List_List*_tmpDB6=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8D4,put_in_effect);cvtenv.free_vars=_tmpDB6;});
# 3877
{void*_tmp773=Cyc_Absyn_compress_kb(_tmp8D4->kind);void*_tmp774=_tmp773;struct Cyc_Core_Opt**_tmp779;struct Cyc_Absyn_Kind*_tmp778;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->tag == 2U){_LL41: _tmp779=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f1;_tmp778=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp774)->f2;_LL42:
# 3879
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp778))
({struct Cyc_Core_Opt*_tmpDB9=({struct Cyc_Core_Opt*_tmp775=_cycalloc(sizeof(*_tmp775));({void*_tmpDB8=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp776=_cycalloc(sizeof(*_tmp776));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDB7=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp777;_tmp777.tag=2U;_tmp777.f1=0;_tmp777.f2=expected_kind;_tmp777;});_tmp776[0]=_tmpDB7;});_tmp776;});_tmp775->v=_tmpDB8;});_tmp775;});*_tmp779=_tmpDB9;});
goto _LL40;}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3884
goto _LL0;case 26U: _LL7: _tmp8D6=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp767)->f1)->r;_tmp8D5=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp767)->f2;_LL8: {
# 3890
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp77A=_tmp8D6;struct Cyc_Absyn_Datatypedecl*_tmp77D;struct Cyc_Absyn_Enumdecl*_tmp77C;struct Cyc_Absyn_Aggrdecl*_tmp77B;switch(*((int*)_tmp77A)){case 0U: _LL46: _tmp77B=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp77A)->f1;_LL47:
# 3893
 if(te->in_extern_c_include)
_tmp77B->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp77B);goto _LL45;case 1U: _LL48: _tmp77C=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp77A)->f1;_LL49:
# 3897
 if(te->in_extern_c_include)
_tmp77C->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp77C);goto _LL45;default: _LL4A: _tmp77D=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp77A)->f1;_LL4B:
# 3901
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp77D);goto _LL45;}_LL45:;}
# 3903
({void**_tmpDBA=({void**_tmp77E=_cycalloc(sizeof(*_tmp77E));_tmp77E[0]=new_t;_tmp77E;});*_tmp8D5=_tmpDBA;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14U: _LL9: _tmp8D7=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp767)->f1;_LLA: {
# 3909
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp8D7 != 0;_tmp8D7=_tmp8D7->tl){
struct Cyc_Absyn_Enumfield*_tmp77F=(struct Cyc_Absyn_Enumfield*)_tmp8D7->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp77F->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp782;_tmp782.tag=0U;_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp77F->name).f2);({void*_tmp780[1U]={& _tmp782};({unsigned int _tmpDBC=_tmp77F->loc;struct _dyneither_ptr _tmpDBB=({const char*_tmp781="duplicate enum field name %s";_tag_dyneither(_tmp781,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpDBC,_tmpDBB,_tag_dyneither(_tmp780,sizeof(void*),1U));});});});else{
# 3916
({struct Cyc_List_List*_tmpDBD=({struct Cyc_List_List*_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783->hd=(*_tmp77F->name).f2;_tmp783->tl=prev_fields;_tmp783;});prev_fields=_tmpDBD;});}
# 3918
if(_tmp77F->tag == 0)
({struct Cyc_Absyn_Exp*_tmpDBE=Cyc_Absyn_uint_exp(tag_count,_tmp77F->loc);_tmp77F->tag=_tmpDBE;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp77F->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0U;_tmp786.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp77F->name).f2);({void*_tmp784[1U]={& _tmp786};({unsigned int _tmpDC0=loc;struct _dyneither_ptr _tmpDBF=({const char*_tmp785="enum field %s: expression is not constant";_tag_dyneither(_tmp785,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpDC0,_tmpDBF,_tag_dyneither(_tmp784,sizeof(void*),1U));});});});}{
# 3923
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp77F->tag))).f1;
tag_count=t1 + 1;};}
# 3926
goto _LL0;}case 13U: _LLB: _tmp8D9=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp767)->f1;_tmp8D8=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp767)->f2;_LLC:
# 3928
 if(*_tmp8D8 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp8D8))->fields == 0){
struct _handler_cons _tmp787;_push_handler(& _tmp787);{int _tmp789=0;if(setjmp(_tmp787.handler))_tmp789=1;if(!_tmp789){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp8D9);
*_tmp8D8=*ed;}
# 3930
;_pop_handler();}else{void*_tmp788=(void*)_exn_thrown;void*_tmp78A=_tmp788;void*_tmp78D;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp78A)->tag == Cyc_Dict_Absent){_LL4D: _LL4E: {
# 3934
struct Cyc_Absyn_Enumdecl*_tmp78B=({struct Cyc_Absyn_Enumdecl*_tmp78C=_cycalloc(sizeof(*_tmp78C));_tmp78C->sc=Cyc_Absyn_Extern;_tmp78C->name=_tmp8D9;_tmp78C->fields=0;_tmp78C;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp78B);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp8D9);
*_tmp8D8=*ed;
goto _LL4C;};}}else{_LL4F: _tmp78D=_tmp78A;_LL50:(int)_rethrow(_tmp78D);}_LL4C:;}};}{
# 3940
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp8D8);
goto _LL0;};case 3U: _LLD: _tmp8DB=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp767)->f1).datatype_info;_tmp8DA=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp767)->f1).targs;_LLE: {
# 3943
struct Cyc_List_List*_tmp78E=*_tmp8DA;
{union Cyc_Absyn_DatatypeInfoU _tmp78F=*_tmp8DB;union Cyc_Absyn_DatatypeInfoU _tmp790=_tmp78F;struct Cyc_Absyn_Datatypedecl*_tmp7A8;struct _tuple2*_tmp7A7;int _tmp7A6;if((_tmp790.UnknownDatatype).tag == 1){_LL52: _tmp7A7=((_tmp790.UnknownDatatype).val).name;_tmp7A6=((_tmp790.UnknownDatatype).val).is_extensible;_LL53: {
# 3946
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp791;_push_handler(& _tmp791);{int _tmp793=0;if(setjmp(_tmp791.handler))_tmp793=1;if(!_tmp793){({struct Cyc_Absyn_Datatypedecl**_tmpDC1=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7A7);tudp=_tmpDC1;});;_pop_handler();}else{void*_tmp792=(void*)_exn_thrown;void*_tmp794=_tmp792;void*_tmp79A;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp794)->tag == Cyc_Dict_Absent){_LL57: _LL58: {
# 3950
struct Cyc_Absyn_Datatypedecl*_tmp795=({struct Cyc_Absyn_Datatypedecl*_tmp799=_cycalloc(sizeof(*_tmp799));_tmp799->sc=Cyc_Absyn_Extern;_tmp799->name=_tmp7A7;_tmp799->tvs=0;_tmp799->fields=0;_tmp799->is_extensible=_tmp7A6;_tmp799;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp795);
({struct Cyc_Absyn_Datatypedecl**_tmpDC2=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7A7);tudp=_tmpDC2;});
# 3954
if(_tmp78E != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp798;_tmp798.tag=0U;({struct _dyneither_ptr _tmpDC3=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A7));_tmp798.f1=_tmpDC3;});({void*_tmp796[1U]={& _tmp798};({unsigned int _tmpDC5=loc;struct _dyneither_ptr _tmpDC4=({const char*_tmp797="declare parameterized datatype %s before using";_tag_dyneither(_tmp797,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpDC5,_tmpDC4,_tag_dyneither(_tmp796,sizeof(void*),1U));});});});
return cvtenv;}
# 3959
goto _LL56;}}else{_LL59: _tmp79A=_tmp794;_LL5A:(int)_rethrow(_tmp79A);}_LL56:;}};}
# 3963
if(_tmp7A6  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp79D;_tmp79D.tag=0U;({struct _dyneither_ptr _tmpDC6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A7));_tmp79D.f1=_tmpDC6;});({void*_tmp79B[1U]={& _tmp79D};({unsigned int _tmpDC8=loc;struct _dyneither_ptr _tmpDC7=({const char*_tmp79C="datatype %s was not declared @extensible";_tag_dyneither(_tmp79C,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpDC8,_tmpDC7,_tag_dyneither(_tmp79B,sizeof(void*),1U));});});});
({union Cyc_Absyn_DatatypeInfoU _tmpDC9=Cyc_Absyn_KnownDatatype(tudp);*_tmp8DB=_tmpDC9;});
_tmp7A8=*tudp;goto _LL55;}}else{_LL54: _tmp7A8=*(_tmp790.KnownDatatype).val;_LL55: {
# 3970
struct Cyc_List_List*tvs=_tmp7A8->tvs;
for(0;_tmp78E != 0  && tvs != 0;(_tmp78E=_tmp78E->tl,tvs=tvs->tl)){
void*t=(void*)_tmp78E->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3976
{struct _tuple0 _tmp79E=({struct _tuple0 _tmp7A1;({void*_tmpDCA=Cyc_Absyn_compress_kb(tv->kind);_tmp7A1.f1=_tmpDCA;});_tmp7A1.f2=t;_tmp7A1;});struct _tuple0 _tmp79F=_tmp79E;struct Cyc_Absyn_Tvar*_tmp7A0;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp79F.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp79F.f2)->tag == 2U){_LL5C: _tmp7A0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp79F.f2)->f1;_LL5D:
# 3978
({struct Cyc_List_List*_tmpDCB=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7A0);cvtenv.kind_env=_tmpDCB;});
({struct Cyc_List_List*_tmpDCC=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7A0,1);cvtenv.free_vars=_tmpDCC;});
continue;}else{goto _LL5E;}}else{_LL5E: _LL5F:
 goto _LL5B;}_LL5B:;}{
# 3983
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDCD=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpDCD;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3987
if(_tmp78E != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7A4;_tmp7A4.tag=0U;({struct _dyneither_ptr _tmpDCE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A8->name));_tmp7A4.f1=_tmpDCE;});({void*_tmp7A2[1U]={& _tmp7A4};({unsigned int _tmpDD0=loc;struct _dyneither_ptr _tmpDCF=({const char*_tmp7A3="too many type arguments for datatype %s";_tag_dyneither(_tmp7A3,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpDD0,_tmpDCF,_tag_dyneither(_tmp7A2,sizeof(void*),1U));});});});
if(tvs != 0){
# 3992
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDD1=({struct Cyc_List_List*_tmp7A5=_cycalloc(sizeof(*_tmp7A5));_tmp7A5->hd=e;_tmp7A5->tl=hidden_ts;_tmp7A5;});hidden_ts=_tmpDD1;});
({struct Cyc_Tcutil_CVTEnv _tmpDD2=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);cvtenv=_tmpDD2;});}
# 3999
({struct Cyc_List_List*_tmpDD4=({struct Cyc_List_List*_tmpDD3=*_tmp8DA;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDD3,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp8DA=_tmpDD4;});}
# 4001
goto _LL51;}}_LL51:;}
# 4003
goto _LL0;}case 4U: _LLF: _tmp8DD=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp767)->f1).field_info;_tmp8DC=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp767)->f1).targs;_LL10:
# 4006
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7A9=*_tmp8DD;union Cyc_Absyn_DatatypeFieldInfoU _tmp7AA=_tmp7A9;struct Cyc_Absyn_Datatypedecl*_tmp7BE;struct Cyc_Absyn_Datatypefield*_tmp7BD;struct _tuple2*_tmp7BC;struct _tuple2*_tmp7BB;int _tmp7BA;if((_tmp7AA.UnknownDatatypefield).tag == 1){_LL61: _tmp7BC=((_tmp7AA.UnknownDatatypefield).val).datatype_name;_tmp7BB=((_tmp7AA.UnknownDatatypefield).val).field_name;_tmp7BA=((_tmp7AA.UnknownDatatypefield).val).is_extensible;_LL62: {
# 4009
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7BC);
struct Cyc_Absyn_Datatypefield*tuf;
# 4014
{struct Cyc_List_List*_tmp7AB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp7AB=_tmp7AB->tl){
if(_tmp7AB == 0)
({void*_tmp7AC=0U;({struct _dyneither_ptr _tmpDD5=({const char*_tmp7AD="Tcutil found a bad datatypefield";_tag_dyneither(_tmp7AD,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDD5,_tag_dyneither(_tmp7AC,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp7AB->hd)->name,_tmp7BB)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp7AB->hd;
break;}}}
# 4022
({union Cyc_Absyn_DatatypeFieldInfoU _tmpDD6=Cyc_Absyn_KnownDatatypefield(tud,tuf);*_tmp8DD=_tmpDD6;});
_tmp7BE=tud;_tmp7BD=tuf;goto _LL64;}}else{_LL63: _tmp7BE=((_tmp7AA.KnownDatatypefield).val).f1;_tmp7BD=((_tmp7AA.KnownDatatypefield).val).f2;_LL64: {
# 4026
struct Cyc_List_List*tvs=_tmp7BE->tvs;
for(0;_tmp8DC != 0  && tvs != 0;(_tmp8DC=_tmp8DC->tl,tvs=tvs->tl)){
void*t=(void*)_tmp8DC->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4032
{struct _tuple0 _tmp7AE=({struct _tuple0 _tmp7B1;({void*_tmpDD7=Cyc_Absyn_compress_kb(tv->kind);_tmp7B1.f1=_tmpDD7;});_tmp7B1.f2=t;_tmp7B1;});struct _tuple0 _tmp7AF=_tmp7AE;struct Cyc_Absyn_Tvar*_tmp7B0;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7AF.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7AF.f2)->tag == 2U){_LL66: _tmp7B0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7AF.f2)->f1;_LL67:
# 4034
({struct Cyc_List_List*_tmpDD8=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7B0);cvtenv.kind_env=_tmpDD8;});
({struct Cyc_List_List*_tmpDD9=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7B0,1);cvtenv.free_vars=_tmpDD9;});
continue;}else{goto _LL68;}}else{_LL68: _LL69:
 goto _LL65;}_LL65:;}{
# 4039
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDDA=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpDDA;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4043
if(_tmp8DC != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B5;_tmp7B5.tag=0U;({struct _dyneither_ptr _tmpDDB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7BD->name));_tmp7B5.f1=_tmpDDB;});({struct Cyc_String_pa_PrintArg_struct _tmp7B4;_tmp7B4.tag=0U;({struct _dyneither_ptr _tmpDDC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7BE->name));_tmp7B4.f1=_tmpDDC;});({void*_tmp7B2[2U]={& _tmp7B4,& _tmp7B5};({unsigned int _tmpDDE=loc;struct _dyneither_ptr _tmpDDD=({const char*_tmp7B3="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp7B3,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpDDE,_tmpDDD,_tag_dyneither(_tmp7B2,sizeof(void*),2U));});});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B9;_tmp7B9.tag=0U;({struct _dyneither_ptr _tmpDDF=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7BD->name));_tmp7B9.f1=_tmpDDF;});({struct Cyc_String_pa_PrintArg_struct _tmp7B8;_tmp7B8.tag=0U;({struct _dyneither_ptr _tmpDE0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7BE->name));_tmp7B8.f1=_tmpDE0;});({void*_tmp7B6[2U]={& _tmp7B8,& _tmp7B9};({unsigned int _tmpDE2=loc;struct _dyneither_ptr _tmpDE1=({const char*_tmp7B7="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp7B7,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpDE2,_tmpDE1,_tag_dyneither(_tmp7B6,sizeof(void*),2U));});});});});
goto _LL60;}}_LL60:;}
# 4051
goto _LL0;case 5U: _LL11: _tmp8E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp767)->f1).elt_typ;_tmp8E2=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp767)->f1).elt_tq;_tmp8E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp767)->f1).ptr_atts).rgn;_tmp8E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp767)->f1).ptr_atts).nullable;_tmp8DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp767)->f1).ptr_atts).bounds;_tmp8DE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp767)->f1).ptr_atts).zero_term;_LL12: {
# 4054
int is_zero_terminated;
# 4056
({struct Cyc_Tcutil_CVTEnv _tmpDE3=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp8E3,1,1);cvtenv=_tmpDE3;});
({int _tmpDE4=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8E2->print_const,_tmp8E3);_tmp8E2->real_const=_tmpDE4;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp7BF=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp7C0=_tmp7BF;switch(_tmp7C0){case Cyc_Absyn_Aliasable: _LL6B: _LL6C:
 k=& Cyc_Tcutil_rk;goto _LL6A;case Cyc_Absyn_Unique: _LL6D: _LL6E:
 k=& Cyc_Tcutil_urk;goto _LL6A;default: _LL6F: _LL70:
 k=& Cyc_Tcutil_trk;goto _LL6A;}_LL6A:;}
# 4064
({struct Cyc_Tcutil_CVTEnv _tmpDE5=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp8E1,1,1);cvtenv=_tmpDE5;});
{union Cyc_Absyn_Constraint*_tmp7C1=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp8DE);union Cyc_Absyn_Constraint*_tmp7C2=_tmp7C1;int _tmp7C8;switch((((union Cyc_Absyn_Constraint*)_tmp7C2)->Eq_constr).tag){case 3U: _LL72: _LL73:
# 4069
{void*_tmp7C3=Cyc_Tcutil_compress(_tmp8E3);void*_tmp7C4=_tmp7C3;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C4)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7C4)->f2 == Cyc_Absyn_Char_sz){_LL79: _LL7A:
# 4071
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8DE,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL78;}else{goto _LL7B;}}else{_LL7B: _LL7C:
# 4075
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8DE,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL78;}_LL78:;}
# 4079
goto _LL71;case 1U: _LL74: _tmp7C8=(int)(_tmp7C2->Eq_constr).val;if(_tmp7C8){_LL75:
# 4082
 if(!Cyc_Tcutil_admits_zero(_tmp8E3))
({struct Cyc_String_pa_PrintArg_struct _tmp7C7;_tmp7C7.tag=0U;({struct _dyneither_ptr _tmpDE6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8E3));_tmp7C7.f1=_tmpDE6;});({void*_tmp7C5[1U]={& _tmp7C7};({unsigned int _tmpDE8=loc;struct _dyneither_ptr _tmpDE7=({const char*_tmp7C6="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp7C6,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpDE8,_tmpDE7,_tag_dyneither(_tmp7C5,sizeof(void*),1U));});});});
is_zero_terminated=1;
goto _LL71;}else{goto _LL76;}default: _LL76: _LL77:
# 4088
 is_zero_terminated=0;
goto _LL71;}_LL71:;}
# 4092
{void*_tmp7C9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp8DF);void*_tmp7CA=_tmp7C9;struct Cyc_Absyn_Exp*_tmp7D3;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp7CA)->tag == 0U){_LL7E: _LL7F:
 goto _LL7D;}else{_LL80: _tmp7D3=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7CA)->f1;_LL81:
# 4095
({struct Cyc_Tcenv_Tenv*_tmpDE9=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDE9,0,_tmp7D3);});
({struct Cyc_Tcutil_CVTEnv _tmpDEA=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7D3,te,cvtenv);cvtenv=_tmpDEA;});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7D3))
({void*_tmp7CB=0U;({unsigned int _tmpDEC=loc;struct _dyneither_ptr _tmpDEB=({const char*_tmp7CC="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp7CC,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpDEC,_tmpDEB,_tag_dyneither(_tmp7CB,sizeof(void*),0U));});});{
struct _tuple14 _tmp7CD=Cyc_Evexp_eval_const_uint_exp(_tmp7D3);struct _tuple14 _tmp7CE=_tmp7CD;unsigned int _tmp7D2;int _tmp7D1;_LL83: _tmp7D2=_tmp7CE.f1;_tmp7D1=_tmp7CE.f2;_LL84:;
if(is_zero_terminated  && (!_tmp7D1  || _tmp7D2 < 1))
({void*_tmp7CF=0U;({unsigned int _tmpDEE=loc;struct _dyneither_ptr _tmpDED=({const char*_tmp7D0="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp7D0,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpDEE,_tmpDED,_tag_dyneither(_tmp7CF,sizeof(void*),0U));});});
goto _LL7D;};}_LL7D:;}
# 4104
goto _LL0;};}case 19U: _LL13: _tmp8E4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp767)->f1;_LL14:
# 4106
({struct Cyc_Tcutil_CVTEnv _tmpDEF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp8E4,1,1);cvtenv=_tmpDEF;});goto _LL0;case 28U: _LL15: _LL16:
 goto _LL0;case 18U: _LL17: _tmp8E5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp767)->f1;_LL18:
# 4112
({struct Cyc_Tcenv_Tenv*_tmpDF0=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDF0,0,_tmp8E5);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp8E5))
({void*_tmp7D4=0U;({unsigned int _tmpDF2=loc;struct _dyneither_ptr _tmpDF1=({const char*_tmp7D5="valueof_t requires an int expression";_tag_dyneither(_tmp7D5,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpDF2,_tmpDF1,_tag_dyneither(_tmp7D4,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpDF3=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp8E5,te,cvtenv);cvtenv=_tmpDF3;});
goto _LL0;case 27U: _LL19: _tmp8E6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp767)->f1;_LL1A:
# 4121
({struct Cyc_Tcenv_Tenv*_tmpDF4=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDF4,0,_tmp8E6);});
goto _LL0;case 6U: _LL1B: _LL1C:
 goto _LL1E;case 7U: _LL1D: _LL1E:
 goto _LL0;case 8U: _LL1F: _tmp8EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp767)->f1).elt_type;_tmp8EA=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp767)->f1).tq;_tmp8E9=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp767)->f1).num_elts;_tmp8E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp767)->f1).zero_term;_tmp8E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp767)->f1).zt_loc;_LL20: {
# 4128
struct Cyc_Absyn_Exp*_tmp7D6=*_tmp8E9;
({struct Cyc_Tcutil_CVTEnv _tmpDF5=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8EB,1,allow_abs_aggr);cvtenv=_tmpDF5;});
({int _tmpDF6=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8EA->print_const,_tmp8EB);_tmp8EA->real_const=_tmpDF6;});{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp7D7=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp8E8);union Cyc_Absyn_Constraint*_tmp7D8=_tmp7D7;int _tmp7DC;switch((((union Cyc_Absyn_Constraint*)_tmp7D8)->Eq_constr).tag){case 3U: _LL86: _LL87:
# 4135
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8E8,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4150 "tcutil.cyc"
goto _LL85;case 1U: _LL88: _tmp7DC=(int)(_tmp7D8->Eq_constr).val;if(_tmp7DC){_LL89:
# 4153
 if(!Cyc_Tcutil_admits_zero(_tmp8EB))
({struct Cyc_String_pa_PrintArg_struct _tmp7DB;_tmp7DB.tag=0U;({struct _dyneither_ptr _tmpDF7=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8EB));_tmp7DB.f1=_tmpDF7;});({void*_tmp7D9[1U]={& _tmp7DB};({unsigned int _tmpDF9=loc;struct _dyneither_ptr _tmpDF8=({const char*_tmp7DA="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp7DA,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpDF9,_tmpDF8,_tag_dyneither(_tmp7D9,sizeof(void*),1U));});});});
is_zero_terminated=1;
goto _LL85;}else{goto _LL8A;}default: _LL8A: _LL8B:
# 4159
 is_zero_terminated=0;
goto _LL85;}_LL85:;}
# 4164
if(_tmp7D6 == 0){
# 4166
if(is_zero_terminated)
# 4168
({struct Cyc_Absyn_Exp*_tmpDFB=({struct Cyc_Absyn_Exp*_tmpDFA=Cyc_Absyn_uint_exp(1U,0U);_tmp7D6=_tmpDFA;});*_tmp8E9=_tmpDFB;});else{
# 4171
({void*_tmp7DD=0U;({unsigned int _tmpDFD=loc;struct _dyneither_ptr _tmpDFC=({const char*_tmp7DE="array bound defaults to 1 here";_tag_dyneither(_tmp7DE,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpDFD,_tmpDFC,_tag_dyneither(_tmp7DD,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpDFF=({struct Cyc_Absyn_Exp*_tmpDFE=Cyc_Absyn_uint_exp(1U,0U);_tmp7D6=_tmpDFE;});*_tmp8E9=_tmpDFF;});}}
# 4175
({struct Cyc_Tcenv_Tenv*_tmpE00=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpE00,0,_tmp7D6);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7D6))
({void*_tmp7DF=0U;({unsigned int _tmpE02=loc;struct _dyneither_ptr _tmpE01=({const char*_tmp7E0="array bounds expression is not an unsigned int";_tag_dyneither(_tmp7E0,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpE02,_tmpE01,_tag_dyneither(_tmp7DF,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpE03=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7D6,te,cvtenv);cvtenv=_tmpE03;});{
# 4183
struct _tuple14 _tmp7E1=Cyc_Evexp_eval_const_uint_exp(_tmp7D6);struct _tuple14 _tmp7E2=_tmp7E1;unsigned int _tmp7E8;int _tmp7E7;_LL8D: _tmp7E8=_tmp7E2.f1;_tmp7E7=_tmp7E2.f2;_LL8E:;
# 4185
if((is_zero_terminated  && _tmp7E7) && _tmp7E8 < 1)
({void*_tmp7E3=0U;({unsigned int _tmpE05=loc;struct _dyneither_ptr _tmpE04=({const char*_tmp7E4="zero terminated array cannot have zero elements";_tag_dyneither(_tmp7E4,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpE05,_tmpE04,_tag_dyneither(_tmp7E3,sizeof(void*),0U));});});
# 4188
if((_tmp7E7  && _tmp7E8 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp7E5=0U;({unsigned int _tmpE07=loc;struct _dyneither_ptr _tmpE06=({const char*_tmp7E6="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp7E6,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpE07,_tmpE06,_tag_dyneither(_tmp7E5,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpE08=Cyc_Absyn_uint_exp(1U,0U);*_tmp8E9=_tmpE08;});}
# 4192
goto _LL0;};};}case 9U: _LL21: _tmp8F8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).tvars;_tmp8F7=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).effect;_tmp8F6=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).ret_tqual;_tmp8F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).ret_typ;_tmp8F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).args;_tmp8F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).c_varargs;_tmp8F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).cyc_varargs;_tmp8F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).rgn_po;_tmp8F0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).attributes;_tmp8EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).requires_clause;_tmp8EE=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).requires_relns;_tmp8ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).ensures_clause;_tmp8EC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp767)->f1).ensures_relns;_LL22: {
# 4199
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp8F0 != 0;_tmp8F0=_tmp8F0->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp8F0->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp7EB;_tmp7EB.tag=0U;({struct _dyneither_ptr _tmpE09=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8F0->hd));_tmp7EB.f1=_tmpE09;});({void*_tmp7E9[1U]={& _tmp7EB};({unsigned int _tmpE0B=loc;struct _dyneither_ptr _tmpE0A=({const char*_tmp7EA="bad function type attribute %s";_tag_dyneither(_tmp7EA,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpE0B,_tmpE0A,_tag_dyneither(_tmp7E9,sizeof(void*),1U));});});});{
void*_tmp7EC=(void*)_tmp8F0->hd;void*_tmp7ED=_tmp7EC;enum Cyc_Absyn_Format_Type _tmp7F2;int _tmp7F1;int _tmp7F0;switch(*((int*)_tmp7ED)){case 1U: _LL90: _LL91:
# 4212
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL8F;case 2U: _LL92: _LL93:
# 4214
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL8F;case 3U: _LL94: _LL95:
# 4216
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL8F;case 19U: _LL96: _tmp7F2=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7ED)->f1;_tmp7F1=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7ED)->f2;_tmp7F0=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7ED)->f3;_LL97:
# 4218
 if(!seen_format){
seen_format=1;
ft=_tmp7F2;
fmt_desc_arg=_tmp7F1;
fmt_arg_start=_tmp7F0;}else{
# 4224
({void*_tmp7EE=0U;({unsigned int _tmpE0D=loc;struct _dyneither_ptr _tmpE0C=({const char*_tmp7EF="function can't have multiple format attributes";_tag_dyneither(_tmp7EF,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpE0D,_tmpE0C,_tag_dyneither(_tmp7EE,sizeof(void*),0U));});});}
goto _LL8F;default: _LL98: _LL99:
 goto _LL8F;}_LL8F:;};}
# 4229
if(num_convs > 1)
({void*_tmp7F3=0U;({unsigned int _tmpE0F=loc;struct _dyneither_ptr _tmpE0E=({const char*_tmp7F4="function can't have multiple calling conventions";_tag_dyneither(_tmp7F4,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpE0F,_tmpE0E,_tag_dyneither(_tmp7F3,sizeof(void*),0U));});});
# 4234
Cyc_Tcutil_check_unique_tvars(loc,*_tmp8F8);
{struct Cyc_List_List*b=*_tmp8F8;for(0;b != 0;b=b->tl){
({int _tmpE10=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpE10;});
({struct Cyc_List_List*_tmpE11=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpE11;});{
void*_tmp7F5=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7F6=_tmp7F5;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F6)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F6)->f1)->kind == Cyc_Absyn_MemKind){_LL9B: _LL9C:
# 4240
({struct Cyc_String_pa_PrintArg_struct _tmp7F9;_tmp7F9.tag=0U;_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp7F7[1U]={& _tmp7F9};({unsigned int _tmpE13=loc;struct _dyneither_ptr _tmpE12=({const char*_tmp7F8="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp7F8,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpE13,_tmpE12,_tag_dyneither(_tmp7F7,sizeof(void*),1U));});});});
goto _LL9A;}else{goto _LL9D;}}else{_LL9D: _LL9E:
 goto _LL9A;}_LL9A:;};}}{
# 4248
struct Cyc_Tcutil_CVTEnv _tmp7FA=({struct Cyc_Tcutil_CVTEnv _tmp888;_tmp888.r=Cyc_Core_heap_region;_tmp888.kind_env=cvtenv.kind_env;_tmp888.free_vars=0;_tmp888.free_evars=0;_tmp888.generalize_evars=cvtenv.generalize_evars;_tmp888.fn_result=1;_tmp888;});
# 4253
({struct Cyc_Tcutil_CVTEnv _tmpE14=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7FA,& Cyc_Tcutil_tmk,_tmp8F5,1,1);_tmp7FA=_tmpE14;});
({int _tmpE15=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8F6->print_const,_tmp8F5);_tmp8F6->real_const=_tmpE15;});
_tmp7FA.fn_result=0;
{struct Cyc_List_List*a=_tmp8F4;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp7FB=(struct _tuple10*)a->hd;
void*_tmp7FC=(*_tmp7FB).f3;
({struct Cyc_Tcutil_CVTEnv _tmpE16=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7FA,& Cyc_Tcutil_tmk,_tmp7FC,1,1);_tmp7FA=_tmpE16;});{
int _tmp7FD=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp7FB).f2).print_const,_tmp7FC);
((*_tmp7FB).f2).real_const=_tmp7FD;
# 4264
if(Cyc_Tcutil_is_array(_tmp7FC)){
# 4266
void*_tmp7FE=Cyc_Absyn_new_evar(0,0);
({struct Cyc_Tcutil_CVTEnv _tmpE17=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7FA,& Cyc_Tcutil_rk,_tmp7FE,1,1);_tmp7FA=_tmpE17;});
({void*_tmpE18=Cyc_Tcutil_promote_array(_tmp7FC,_tmp7FE,0);(*_tmp7FB).f3=_tmpE18;});}};}}
# 4273
if(_tmp8F2 != 0){
if(_tmp8F3)({void*_tmp7FF=0U;({struct _dyneither_ptr _tmpE19=({const char*_tmp800="both c_vararg and cyc_vararg";_tag_dyneither(_tmp800,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE19,_tag_dyneither(_tmp7FF,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp801=*_tmp8F2;struct Cyc_Absyn_VarargInfo _tmp802=_tmp801;void*_tmp815;int _tmp814;_LLA0: _tmp815=_tmp802.type;_tmp814=_tmp802.inject;_LLA1:;
({struct Cyc_Tcutil_CVTEnv _tmpE1A=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7FA,& Cyc_Tcutil_tmk,_tmp815,1,1);_tmp7FA=_tmpE1A;});
({int _tmpE1B=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp8F2->tq).print_const,_tmp815);(_tmp8F2->tq).real_const=_tmpE1B;});
# 4279
if(_tmp814){
void*_tmp803=Cyc_Tcutil_compress(_tmp815);void*_tmp804=_tmp803;void*_tmp813;union Cyc_Absyn_Constraint*_tmp812;union Cyc_Absyn_Constraint*_tmp811;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp804)->tag == 5U){_LLA3: _tmp813=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp804)->f1).elt_typ;_tmp812=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp804)->f1).ptr_atts).bounds;_tmp811=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp804)->f1).ptr_atts).zero_term;_LLA4:
# 4282
{void*_tmp805=Cyc_Tcutil_compress(_tmp813);void*_tmp806=_tmp805;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp806)->tag == 3U){_LLA8: _LLA9:
# 4284
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp811))
({void*_tmp807=0U;({unsigned int _tmpE1D=loc;struct _dyneither_ptr _tmpE1C=({const char*_tmp808="can't inject into a zeroterm pointer";_tag_dyneither(_tmp808,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpE1D,_tmpE1C,_tag_dyneither(_tmp807,sizeof(void*),0U));});});
{void*_tmp809=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp812);void*_tmp80A=_tmp809;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp80A)->tag == 0U){_LLAD: _LLAE:
# 4288
({void*_tmp80B=0U;({unsigned int _tmpE1F=loc;struct _dyneither_ptr _tmpE1E=({const char*_tmp80C="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp80C,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpE1F,_tmpE1E,_tag_dyneither(_tmp80B,sizeof(void*),0U));});});
goto _LLAC;}else{_LLAF: _LLB0:
 goto _LLAC;}_LLAC:;}
# 4292
goto _LLA7;}else{_LLAA: _LLAB:
({void*_tmp80D=0U;({unsigned int _tmpE21=loc;struct _dyneither_ptr _tmpE20=({const char*_tmp80E="can't inject a non-datatype type";_tag_dyneither(_tmp80E,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpE21,_tmpE20,_tag_dyneither(_tmp80D,sizeof(void*),0U));});});goto _LLA7;}_LLA7:;}
# 4295
goto _LLA2;}else{_LLA5: _LLA6:
({void*_tmp80F=0U;({unsigned int _tmpE23=loc;struct _dyneither_ptr _tmpE22=({const char*_tmp810="expecting a datatype pointer type";_tag_dyneither(_tmp810,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpE23,_tmpE22,_tag_dyneither(_tmp80F,sizeof(void*),0U));});});goto _LLA2;}_LLA2:;}};}
# 4301
if(seen_format){
int _tmp816=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8F4);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp816) || fmt_arg_start < 0) || 
# 4305
(_tmp8F2 == 0  && !_tmp8F3) && fmt_arg_start != 0) || 
(_tmp8F2 != 0  || _tmp8F3) && fmt_arg_start != _tmp816 + 1)
# 4308
({void*_tmp817=0U;({unsigned int _tmpE25=loc;struct _dyneither_ptr _tmpE24=({const char*_tmp818="bad format descriptor";_tag_dyneither(_tmp818,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpE25,_tmpE24,_tag_dyneither(_tmp817,sizeof(void*),0U));});});else{
# 4311
struct _tuple10 _tmp819=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp8F4,fmt_desc_arg - 1);struct _tuple10 _tmp81A=_tmp819;void*_tmp832;_LLB2: _tmp832=_tmp81A.f3;_LLB3:;
# 4313
{void*_tmp81B=Cyc_Tcutil_compress(_tmp832);void*_tmp81C=_tmp81B;void*_tmp82A;union Cyc_Absyn_Constraint*_tmp829;union Cyc_Absyn_Constraint*_tmp828;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp81C)->tag == 5U){_LLB5: _tmp82A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp81C)->f1).elt_typ;_tmp829=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp81C)->f1).ptr_atts).bounds;_tmp828=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp81C)->f1).ptr_atts).zero_term;_LLB6:
# 4316
{struct _tuple0 _tmp81D=({struct _tuple0 _tmp825;({void*_tmpE26=Cyc_Tcutil_compress(_tmp82A);_tmp825.f1=_tmpE26;});({void*_tmpE27=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp829);_tmp825.f2=_tmpE27;});_tmp825;});struct _tuple0 _tmp81E=_tmp81D;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp81E.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp81E.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp81E.f2)->tag == 0U){_LLBA: _LLBB:
# 4318
 if(_tmp8F3)
({void*_tmp81F=0U;({unsigned int _tmpE29=loc;struct _dyneither_ptr _tmpE28=({const char*_tmp820="format descriptor is not a char * type";_tag_dyneither(_tmp820,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE29,_tmpE28,_tag_dyneither(_tmp81F,sizeof(void*),0U));});});
goto _LLB9;}else{_LLBC: _LLBD:
# 4322
 if(!_tmp8F3)
({void*_tmp821=0U;({unsigned int _tmpE2B=loc;struct _dyneither_ptr _tmpE2A=({const char*_tmp822="format descriptor is not a char ? type";_tag_dyneither(_tmp822,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE2B,_tmpE2A,_tag_dyneither(_tmp821,sizeof(void*),0U));});});
goto _LLB9;}}else{goto _LLBE;}}else{_LLBE: _LLBF:
# 4326
({void*_tmp823=0U;({unsigned int _tmpE2D=loc;struct _dyneither_ptr _tmpE2C=({const char*_tmp824="format descriptor is not a string type";_tag_dyneither(_tmp824,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE2D,_tmpE2C,_tag_dyneither(_tmp823,sizeof(void*),0U));});});
goto _LLB9;}_LLB9:;}
# 4329
goto _LLB4;}else{_LLB7: _LLB8:
({void*_tmp826=0U;({unsigned int _tmpE2F=loc;struct _dyneither_ptr _tmpE2E=({const char*_tmp827="format descriptor is not a string type";_tag_dyneither(_tmp827,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE2F,_tmpE2E,_tag_dyneither(_tmp826,sizeof(void*),0U));});});goto _LLB4;}_LLB4:;}
# 4332
if(fmt_arg_start != 0  && !_tmp8F3){
# 4336
int problem;
{struct _tuple31 _tmp82B=({struct _tuple31 _tmp82F;_tmp82F.f1=ft;({void*_tmpE30=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp8F2))->type));_tmp82F.f2=_tmpE30;});_tmp82F;});struct _tuple31 _tmp82C=_tmp82B;struct Cyc_Absyn_Datatypedecl*_tmp82E;struct Cyc_Absyn_Datatypedecl*_tmp82D;if(_tmp82C.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp82C.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp82C.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC1: _tmp82D=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp82C.f2)->f1).datatype_info).KnownDatatype).val;_LLC2:
# 4339
({int _tmpE31=Cyc_Absyn_qvar_cmp(_tmp82D->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;problem=_tmpE31;});goto _LLC0;}else{goto _LLC5;}}else{goto _LLC5;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp82C.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp82C.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC3: _tmp82E=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp82C.f2)->f1).datatype_info).KnownDatatype).val;_LLC4:
# 4341
({int _tmpE32=Cyc_Absyn_qvar_cmp(_tmp82E->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;problem=_tmpE32;});goto _LLC0;}else{goto _LLC5;}}else{_LLC5: _LLC6:
# 4343
 problem=1;goto _LLC0;}}_LLC0:;}
# 4345
if(problem)
({void*_tmp830=0U;({unsigned int _tmpE34=loc;struct _dyneither_ptr _tmpE33=({const char*_tmp831="format attribute and vararg types don't match";_tag_dyneither(_tmp831,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpE34,_tmpE33,_tag_dyneither(_tmp830,sizeof(void*),0U));});});}}}
# 4353
{struct Cyc_List_List*rpo=_tmp8F1;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp833=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp834=_tmp833;void*_tmp836;void*_tmp835;_LLC8: _tmp836=_tmp834->f1;_tmp835=_tmp834->f2;_LLC9:;
({struct Cyc_Tcutil_CVTEnv _tmpE35=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7FA,& Cyc_Tcutil_ek,_tmp836,1,1);_tmp7FA=_tmpE35;});
({struct Cyc_Tcutil_CVTEnv _tmpE36=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7FA,& Cyc_Tcutil_trk,_tmp835,1,1);_tmp7FA=_tmpE36;});}}{
# 4361
struct Cyc_Tcenv_Fenv*_tmp837=Cyc_Tcenv_bogus_fenv(_tmp8F5,_tmp8F4);
struct Cyc_Tcenv_Tenv*_tmp838=({struct Cyc_Tcenv_Tenv*_tmp887=_cycalloc(sizeof(*_tmp887));_tmp887->ns=te->ns;_tmp887->ae=te->ae;_tmp887->le=_tmp837;_tmp887->allow_valueof=1;_tmp887->in_extern_c_include=te->in_extern_c_include;_tmp887;});
struct _tuple30 _tmp839=({unsigned int _tmpE3A=loc;struct Cyc_Tcenv_Tenv*_tmpE39=_tmp838;struct Cyc_Tcutil_CVTEnv _tmpE38=_tmp7FA;struct _dyneither_ptr _tmpE37=({const char*_tmp886="@requires";_tag_dyneither(_tmp886,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpE3A,_tmpE39,_tmpE38,_tmpE37,_tmp8EF);});struct _tuple30 _tmp83A=_tmp839;struct Cyc_Tcutil_CVTEnv _tmp885;struct Cyc_List_List*_tmp884;_LLCB: _tmp885=_tmp83A.f1;_tmp884=_tmp83A.f2;_LLCC:;
_tmp7FA=_tmp885;
*_tmp8EE=_tmp884;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp8F4,_tmp884);{
# 4374
struct _tuple30 _tmp83B=({unsigned int _tmpE3E=loc;struct Cyc_Tcenv_Tenv*_tmpE3D=_tmp838;struct Cyc_Tcutil_CVTEnv _tmpE3C=_tmp7FA;struct _dyneither_ptr _tmpE3B=({const char*_tmp883="@ensures";_tag_dyneither(_tmp883,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpE3E,_tmpE3D,_tmpE3C,_tmpE3B,_tmp8ED);});struct _tuple30 _tmp83C=_tmp83B;struct Cyc_Tcutil_CVTEnv _tmp882;struct Cyc_List_List*_tmp881;_LLCE: _tmp882=_tmp83C.f1;_tmp881=_tmp83C.f2;_LLCF:;
_tmp7FA=_tmp882;
*_tmp8EC=_tmp881;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp8F4,_tmp881);
# 4379
if(*_tmp8F7 != 0)
({struct Cyc_Tcutil_CVTEnv _tmpE3F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7FA,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp8F7),1,1);_tmp7FA=_tmpE3F;});else{
# 4382
struct Cyc_List_List*effect=0;
# 4387
{struct Cyc_List_List*tvs=_tmp7FA.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp83D=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp83E=_tmp83D;struct Cyc_Absyn_Tvar*_tmp857;int _tmp856;_LLD1: _tmp857=_tmp83E.f1;_tmp856=_tmp83E.f2;_LLD2:;
if(!_tmp856)continue;{
void*_tmp83F=Cyc_Absyn_compress_kb(_tmp857->kind);void*_tmp840=_tmp83F;struct Cyc_Core_Opt**_tmp855;struct Cyc_Absyn_Kind*_tmp854;struct Cyc_Core_Opt**_tmp853;struct Cyc_Core_Opt**_tmp852;struct Cyc_Absyn_Kind*_tmp851;switch(*((int*)_tmp840)){case 2U: _LLD4: _tmp852=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp840)->f1;_tmp851=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp840)->f2;if(_tmp851->kind == Cyc_Absyn_RgnKind){_LLD5:
# 4393
 if(_tmp851->aliasqual == Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpE40=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp852=_tmpE40;});_tmp854=_tmp851;goto _LLD7;}
# 4396
({struct Cyc_Core_Opt*_tmpE41=Cyc_Tcutil_kind_to_bound_opt(_tmp851);*_tmp852=_tmpE41;});_tmp854=_tmp851;goto _LLD7;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp840)->f2)->kind){case Cyc_Absyn_IntKind: _LLD8: _LLD9:
# 4399
 goto _LLDB;case Cyc_Absyn_EffKind: _LLDC: _tmp853=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp840)->f1;_LLDD:
# 4402
({struct Cyc_Core_Opt*_tmpE42=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp853=_tmpE42;});goto _LLDF;default: goto _LLE2;}}case 0U: _LLD6: _tmp854=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp840)->f1;if(_tmp854->kind == Cyc_Absyn_RgnKind){_LLD7:
# 4398
({struct Cyc_List_List*_tmpE47=({struct Cyc_List_List*_tmp841=_cycalloc(sizeof(*_tmp841));({void*_tmpE46=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp842=_cycalloc(sizeof(*_tmp842));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE45=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp843;_tmp843.tag=23U;({void*_tmpE44=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp844=_cycalloc(sizeof(*_tmp844));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE43=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp845;_tmp845.tag=2U;_tmp845.f1=_tmp857;_tmp845;});_tmp844[0]=_tmpE43;});_tmp844;});_tmp843.f1=_tmpE44;});_tmp843;});_tmp842[0]=_tmpE45;});_tmp842;});_tmp841->hd=_tmpE46;});_tmp841->tl=effect;_tmp841;});effect=_tmpE47;});goto _LLD3;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp840)->f1)->kind){case Cyc_Absyn_IntKind: _LLDA: _LLDB:
# 4400
 goto _LLD3;case Cyc_Absyn_EffKind: _LLDE: _LLDF:
# 4404
({struct Cyc_List_List*_tmpE4A=({struct Cyc_List_List*_tmp846=_cycalloc(sizeof(*_tmp846));({void*_tmpE49=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp847=_cycalloc(sizeof(*_tmp847));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE48=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp848;_tmp848.tag=2U;_tmp848.f1=_tmp857;_tmp848;});_tmp847[0]=_tmpE48;});_tmp847;});_tmp846->hd=_tmpE49;});_tmp846->tl=effect;_tmp846;});effect=_tmpE4A;});goto _LLD3;default: _LLE2: _LLE3:
# 4409
({struct Cyc_List_List*_tmpE4F=({struct Cyc_List_List*_tmp84C=_cycalloc(sizeof(*_tmp84C));({void*_tmpE4E=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp84D=_cycalloc(sizeof(*_tmp84D));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE4D=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp84E;_tmp84E.tag=25U;({void*_tmpE4C=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp84F=_cycalloc(sizeof(*_tmp84F));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE4B=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp850;_tmp850.tag=2U;_tmp850.f1=_tmp857;_tmp850;});_tmp84F[0]=_tmpE4B;});_tmp84F;});_tmp84E.f1=_tmpE4C;});_tmp84E;});_tmp84D[0]=_tmpE4D;});_tmp84D;});_tmp84C->hd=_tmpE4E;});_tmp84C->tl=effect;_tmp84C;});effect=_tmpE4F;});goto _LLD3;}}default: _LLE0: _tmp855=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp840)->f1;_LLE1:
# 4406
({struct Cyc_Core_Opt*_tmpE52=({struct Cyc_Core_Opt*_tmp849=_cycalloc(sizeof(*_tmp849));({void*_tmpE51=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp84A=_cycalloc(sizeof(*_tmp84A));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE50=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp84B;_tmp84B.tag=2U;_tmp84B.f1=0;_tmp84B.f2=& Cyc_Tcutil_ak;_tmp84B;});_tmp84A[0]=_tmpE50;});_tmp84A;});_tmp849->v=_tmpE51;});_tmp849;});*_tmp855=_tmpE52;});goto _LLE3;}_LLD3:;};}}
# 4413
{struct Cyc_List_List*ts=_tmp7FA.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp858=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp859=_tmp858;void*_tmp864;int _tmp863;_LLE5: _tmp864=_tmp859.f1;_tmp863=_tmp859.f2;_LLE6:;
if(!_tmp863)continue;{
struct Cyc_Absyn_Kind*_tmp85A=Cyc_Tcutil_typ_kind(_tmp864);struct Cyc_Absyn_Kind*_tmp85B=_tmp85A;switch(((struct Cyc_Absyn_Kind*)_tmp85B)->kind){case Cyc_Absyn_RgnKind: _LLE8: _LLE9:
# 4418
({struct Cyc_List_List*_tmpE55=({struct Cyc_List_List*_tmp85C=_cycalloc(sizeof(*_tmp85C));({void*_tmpE54=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp85D=_cycalloc(sizeof(*_tmp85D));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE53=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp85E;_tmp85E.tag=23U;_tmp85E.f1=_tmp864;_tmp85E;});_tmp85D[0]=_tmpE53;});_tmp85D;});_tmp85C->hd=_tmpE54;});_tmp85C->tl=effect;_tmp85C;});effect=_tmpE55;});goto _LLE7;case Cyc_Absyn_EffKind: _LLEA: _LLEB:
# 4420
({struct Cyc_List_List*_tmpE56=({struct Cyc_List_List*_tmp85F=_cycalloc(sizeof(*_tmp85F));_tmp85F->hd=_tmp864;_tmp85F->tl=effect;_tmp85F;});effect=_tmpE56;});goto _LLE7;case Cyc_Absyn_IntKind: _LLEC: _LLED:
 goto _LLE7;default: _LLEE: _LLEF:
# 4423
({struct Cyc_List_List*_tmpE59=({struct Cyc_List_List*_tmp860=_cycalloc(sizeof(*_tmp860));({void*_tmpE58=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp861=_cycalloc(sizeof(*_tmp861));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE57=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp862;_tmp862.tag=25U;_tmp862.f1=_tmp864;_tmp862;});_tmp861[0]=_tmpE57;});_tmp861;});_tmp860->hd=_tmpE58;});_tmp860->tl=effect;_tmp860;});effect=_tmpE59;});goto _LLE7;}_LLE7:;};}}
# 4426
({void*_tmpE5B=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp865=_cycalloc(sizeof(*_tmp865));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE5A=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp866;_tmp866.tag=24U;_tmp866.f1=effect;_tmp866;});_tmp865[0]=_tmpE5A;});_tmp865;});*_tmp8F7=_tmpE5B;});}
# 4432
if(*_tmp8F8 != 0){
struct Cyc_List_List*bs=*_tmp8F8;for(0;bs != 0;bs=bs->tl){
void*_tmp867=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp868=_tmp867;struct Cyc_Core_Opt**_tmp878;struct Cyc_Absyn_Kind*_tmp877;struct Cyc_Core_Opt**_tmp876;struct Cyc_Core_Opt**_tmp875;struct Cyc_Core_Opt**_tmp874;struct Cyc_Core_Opt**_tmp873;struct Cyc_Core_Opt**_tmp872;struct Cyc_Core_Opt**_tmp871;struct Cyc_Core_Opt**_tmp870;struct Cyc_Core_Opt**_tmp86F;struct Cyc_Core_Opt**_tmp86E;switch(*((int*)_tmp868)){case 1U: _LLF1: _tmp86E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LLF2:
# 4436
({struct Cyc_String_pa_PrintArg_struct _tmp86B;_tmp86B.tag=0U;_tmp86B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp869[1U]={& _tmp86B};({unsigned int _tmpE5D=loc;struct _dyneither_ptr _tmpE5C=({const char*_tmp86A="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp86A,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpE5D,_tmpE5C,_tag_dyneither(_tmp869,sizeof(void*),1U));});});});
# 4438
_tmp86F=_tmp86E;goto _LLF4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f2)->aliasqual == Cyc_Absyn_Top){_LLF3: _tmp86F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LLF4:
 _tmp870=_tmp86F;goto _LLF6;}else{goto _LL103;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF5: _tmp870=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LLF6:
 _tmp871=_tmp870;goto _LLF8;case Cyc_Absyn_Aliasable: _LLF7: _tmp871=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LLF8:
 _tmp873=_tmp871;goto _LLFA;default: _LLFD: _tmp872=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LLFE:
# 4445
 _tmp875=_tmp872;goto _LL100;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF9: _tmp873=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LLFA:
# 4442
 _tmp874=_tmp873;goto _LLFC;case Cyc_Absyn_Aliasable: _LLFB: _tmp874=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LLFC:
# 4444
({struct Cyc_Core_Opt*_tmpE5E=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp874=_tmpE5E;});goto _LLF0;default: _LLFF: _tmp875=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LL100:
# 4447
({struct Cyc_Core_Opt*_tmpE5F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp875=_tmpE5F;});goto _LLF0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f2)->aliasqual == Cyc_Absyn_Top){_LL101: _tmp876=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_LL102:
# 4449
({struct Cyc_Core_Opt*_tmpE60=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp876=_tmpE60;});goto _LLF0;}else{goto _LL103;}default: _LL103: _tmp878=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f1;_tmp877=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp868)->f2;_LL104:
# 4451
({struct Cyc_Core_Opt*_tmpE61=Cyc_Tcutil_kind_to_bound_opt(_tmp877);*_tmp878=_tmpE61;});goto _LLF0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp868)->f1)->kind == Cyc_Absyn_MemKind){_LL105: _LL106:
# 4453
({void*_tmp86C=0U;({unsigned int _tmpE63=loc;struct _dyneither_ptr _tmpE62=({const char*_tmp86D="functions can't abstract M types";_tag_dyneither(_tmp86D,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpE63,_tmpE62,_tag_dyneither(_tmp86C,sizeof(void*),0U));});});goto _LLF0;}else{_LL107: _LL108:
 goto _LLF0;}}_LLF0:;}}
# 4458
({struct Cyc_List_List*_tmpE64=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp7FA.kind_env,*_tmp8F8);cvtenv.kind_env=_tmpE64;});
({struct Cyc_List_List*_tmpE65=Cyc_Tcutil_remove_bound_tvars_bool(_tmp7FA.r,_tmp7FA.free_vars,*_tmp8F8);_tmp7FA.free_vars=_tmpE65;});
# 4461
{struct Cyc_List_List*tvs=_tmp7FA.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp879=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp87A=_tmp879;struct Cyc_Absyn_Tvar*_tmp87C;int _tmp87B;_LL10A: _tmp87C=_tmp87A.f1;_tmp87B=_tmp87A.f2;_LL10B:;
({struct Cyc_List_List*_tmpE66=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp87C,_tmp87B);cvtenv.free_vars=_tmpE66;});}}
# 4466
{struct Cyc_List_List*evs=_tmp7FA.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp87D=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp87E=_tmp87D;void*_tmp880;int _tmp87F;_LL10D: _tmp880=_tmp87E.f1;_tmp87F=_tmp87E.f2;_LL10E:;
({struct Cyc_List_List*_tmpE67=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp880,_tmp87F);cvtenv.free_evars=_tmpE67;});}}
# 4470
goto _LL0;};};};}case 10U: _LL23: _tmp8F9=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp767)->f1;_LL24:
# 4473
 for(0;_tmp8F9 != 0;_tmp8F9=_tmp8F9->tl){
struct _tuple12*_tmp889=(struct _tuple12*)_tmp8F9->hd;
({struct Cyc_Tcutil_CVTEnv _tmpE68=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp889).f2,1,0);cvtenv=_tmpE68;});
({int _tmpE69=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp889).f1).print_const,(*_tmp889).f2);
# 4476
((*_tmp889).f1).real_const=_tmpE69;});}
# 4479
goto _LL0;case 12U: _LL25: _tmp8FB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp767)->f1;_tmp8FA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp767)->f2;_LL26: {
# 4483
struct Cyc_List_List*prev_fields=0;
for(0;_tmp8FA != 0;_tmp8FA=_tmp8FA->tl){
struct Cyc_Absyn_Aggrfield*_tmp88A=(struct Cyc_Absyn_Aggrfield*)_tmp8FA->hd;struct Cyc_Absyn_Aggrfield*_tmp88B=_tmp88A;struct _dyneither_ptr*_tmp89B;struct Cyc_Absyn_Tqual*_tmp89A;void*_tmp899;struct Cyc_Absyn_Exp*_tmp898;struct Cyc_List_List*_tmp897;struct Cyc_Absyn_Exp*_tmp896;_LL110: _tmp89B=_tmp88B->name;_tmp89A=(struct Cyc_Absyn_Tqual*)& _tmp88B->tq;_tmp899=_tmp88B->type;_tmp898=_tmp88B->width;_tmp897=_tmp88B->attributes;_tmp896=_tmp88B->requires_clause;_LL111:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp89B))
({struct Cyc_String_pa_PrintArg_struct _tmp88E;_tmp88E.tag=0U;_tmp88E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp89B);({void*_tmp88C[1U]={& _tmp88E};({unsigned int _tmpE6B=loc;struct _dyneither_ptr _tmpE6A=({const char*_tmp88D="duplicate field %s";_tag_dyneither(_tmp88D,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpE6B,_tmpE6A,_tag_dyneither(_tmp88C,sizeof(void*),1U));});});});
if(({struct _dyneither_ptr _tmpE6C=(struct _dyneither_ptr)*_tmp89B;Cyc_strcmp(_tmpE6C,({const char*_tmp88F="";_tag_dyneither(_tmp88F,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpE6D=({struct Cyc_List_List*_tmp890=_cycalloc(sizeof(*_tmp890));_tmp890->hd=_tmp89B;_tmp890->tl=prev_fields;_tmp890;});prev_fields=_tmpE6D;});
({struct Cyc_Tcutil_CVTEnv _tmpE6E=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp899,1,0);cvtenv=_tmpE6E;});
({int _tmpE6F=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp89A->print_const,_tmp899);_tmp89A->real_const=_tmpE6F;});
# 4496
Cyc_Tcutil_check_bitfield(loc,te,_tmp899,_tmp898,_tmp89B);
Cyc_Tcutil_check_field_atts(loc,_tmp89B,_tmp897);
if(_tmp896 != 0){
# 4500
if(_tmp8FB != Cyc_Absyn_UnionA)
({void*_tmp891=0U;({unsigned int _tmpE71=loc;struct _dyneither_ptr _tmpE70=({const char*_tmp892="@requires clause is only allowed on union members";_tag_dyneither(_tmp892,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpE71,_tmpE70,_tag_dyneither(_tmp891,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpE72=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpE72,0,_tmp896);});
if(!Cyc_Tcutil_is_integral(_tmp896))
({struct Cyc_String_pa_PrintArg_struct _tmp895;_tmp895.tag=0U;({struct _dyneither_ptr _tmpE73=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp896->topt)));_tmp895.f1=_tmpE73;});({void*_tmp893[1U]={& _tmp895};({unsigned int _tmpE75=loc;struct _dyneither_ptr _tmpE74=({const char*_tmp894="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp894,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpE75,_tmpE74,_tag_dyneither(_tmp893,sizeof(void*),1U));});});});
({struct Cyc_Tcutil_CVTEnv _tmpE76=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp896,te,cvtenv);cvtenv=_tmpE76;});}}
# 4509
goto _LL0;}case 11U: _LL27: _tmp8FD=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp767)->f1).aggr_info;_tmp8FC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp767)->f1).targs;_LL28:
# 4512
{union Cyc_Absyn_AggrInfoU _tmp89C=*_tmp8FD;union Cyc_Absyn_AggrInfoU _tmp89D=_tmp89C;struct Cyc_Absyn_Aggrdecl*_tmp8C1;enum Cyc_Absyn_AggrKind _tmp8C0;struct _tuple2*_tmp8BF;struct Cyc_Core_Opt*_tmp8BE;if((_tmp89D.UnknownAggr).tag == 1){_LL113: _tmp8C0=((_tmp89D.UnknownAggr).val).f1;_tmp8BF=((_tmp89D.UnknownAggr).val).f2;_tmp8BE=((_tmp89D.UnknownAggr).val).f3;_LL114: {
# 4514
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp89E;_push_handler(& _tmp89E);{int _tmp8A0=0;if(setjmp(_tmp89E.handler))_tmp8A0=1;if(!_tmp8A0){
({struct Cyc_Absyn_Aggrdecl**_tmpE77=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8BF);adp=_tmpE77;});{
struct Cyc_Absyn_Aggrdecl*_tmp8A1=*adp;
if(_tmp8A1->kind != _tmp8C0){
if(_tmp8A1->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp8A5;_tmp8A5.tag=0U;({struct _dyneither_ptr _tmpE78=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8BF));_tmp8A5.f1=_tmpE78;});({struct Cyc_String_pa_PrintArg_struct _tmp8A4;_tmp8A4.tag=0U;({struct _dyneither_ptr _tmpE79=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4520
Cyc_Absynpp_qvar2string(_tmp8BF));_tmp8A4.f1=_tmpE79;});({void*_tmp8A2[2U]={& _tmp8A4,& _tmp8A5};({unsigned int _tmpE7B=loc;struct _dyneither_ptr _tmpE7A=({const char*_tmp8A3="expecting struct %s instead of union %s";_tag_dyneither(_tmp8A3,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpE7B,_tmpE7A,_tag_dyneither(_tmp8A2,sizeof(void*),2U));});});});});else{
# 4523
({struct Cyc_String_pa_PrintArg_struct _tmp8A9;_tmp8A9.tag=0U;({struct _dyneither_ptr _tmpE7C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8BF));_tmp8A9.f1=_tmpE7C;});({struct Cyc_String_pa_PrintArg_struct _tmp8A8;_tmp8A8.tag=0U;({struct _dyneither_ptr _tmpE7D=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4523
Cyc_Absynpp_qvar2string(_tmp8BF));_tmp8A8.f1=_tmpE7D;});({void*_tmp8A6[2U]={& _tmp8A8,& _tmp8A9};({unsigned int _tmpE7F=loc;struct _dyneither_ptr _tmpE7E=({const char*_tmp8A7="expecting union %s instead of struct %s";_tag_dyneither(_tmp8A7,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpE7F,_tmpE7E,_tag_dyneither(_tmp8A6,sizeof(void*),2U));});});});});}}
# 4526
if((unsigned int)_tmp8BE  && (int)_tmp8BE->v){
if(!((unsigned int)_tmp8A1->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8A1->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp8AC;_tmp8AC.tag=0U;({struct _dyneither_ptr _tmpE80=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8BF));_tmp8AC.f1=_tmpE80;});({void*_tmp8AA[1U]={& _tmp8AC};({unsigned int _tmpE82=loc;struct _dyneither_ptr _tmpE81=({const char*_tmp8AB="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp8AB,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpE82,_tmpE81,_tag_dyneither(_tmp8AA,sizeof(void*),1U));});});});}
# 4532
({union Cyc_Absyn_AggrInfoU _tmpE83=Cyc_Absyn_KnownAggr(adp);*_tmp8FD=_tmpE83;});};
# 4516
;_pop_handler();}else{void*_tmp89F=(void*)_exn_thrown;void*_tmp8AD=_tmp89F;void*_tmp8B3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8AD)->tag == Cyc_Dict_Absent){_LL118: _LL119: {
# 4536
struct Cyc_Absyn_Aggrdecl*_tmp8AE=({struct Cyc_Absyn_Aggrdecl*_tmp8B2=_cycalloc(sizeof(*_tmp8B2));_tmp8B2->kind=_tmp8C0;_tmp8B2->sc=Cyc_Absyn_Extern;_tmp8B2->name=_tmp8BF;_tmp8B2->tvs=0;_tmp8B2->impl=0;_tmp8B2->attributes=0;_tmp8B2->expected_mem_kind=0;_tmp8B2;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp8AE);
({struct Cyc_Absyn_Aggrdecl**_tmpE84=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8BF);adp=_tmpE84;});
({union Cyc_Absyn_AggrInfoU _tmpE85=Cyc_Absyn_KnownAggr(adp);*_tmp8FD=_tmpE85;});
# 4541
if(*_tmp8FC != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8B1;_tmp8B1.tag=0U;({struct _dyneither_ptr _tmpE86=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8BF));_tmp8B1.f1=_tmpE86;});({void*_tmp8AF[1U]={& _tmp8B1};({unsigned int _tmpE88=loc;struct _dyneither_ptr _tmpE87=({const char*_tmp8B0="declare parameterized type %s before using";_tag_dyneither(_tmp8B0,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpE88,_tmpE87,_tag_dyneither(_tmp8AF,sizeof(void*),1U));});});});
return cvtenv;}
# 4546
goto _LL117;}}else{_LL11A: _tmp8B3=_tmp8AD;_LL11B:(int)_rethrow(_tmp8B3);}_LL117:;}};}
# 4548
_tmp8C1=*adp;goto _LL116;}}else{_LL115: _tmp8C1=*(_tmp89D.KnownAggr).val;_LL116: {
# 4550
struct Cyc_List_List*tvs=_tmp8C1->tvs;
struct Cyc_List_List*ts=*_tmp8FC;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp8B4=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8B5=(void*)ts->hd;
# 4558
{struct _tuple0 _tmp8B6=({struct _tuple0 _tmp8B9;({void*_tmpE89=Cyc_Absyn_compress_kb(_tmp8B4->kind);_tmp8B9.f1=_tmpE89;});_tmp8B9.f2=_tmp8B5;_tmp8B9;});struct _tuple0 _tmp8B7=_tmp8B6;struct Cyc_Absyn_Tvar*_tmp8B8;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B7.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8B7.f2)->tag == 2U){_LL11D: _tmp8B8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8B7.f2)->f1;_LL11E:
# 4560
({struct Cyc_List_List*_tmpE8A=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8B8);cvtenv.kind_env=_tmpE8A;});
({struct Cyc_List_List*_tmpE8B=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8B8,1);cvtenv.free_vars=_tmpE8B;});
continue;}else{goto _LL11F;}}else{_LL11F: _LL120:
 goto _LL11C;}_LL11C:;}{
# 4565
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpE8C=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpE8C;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4569
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8BC;_tmp8BC.tag=0U;({struct _dyneither_ptr _tmpE8D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8C1->name));_tmp8BC.f1=_tmpE8D;});({void*_tmp8BA[1U]={& _tmp8BC};({unsigned int _tmpE8F=loc;struct _dyneither_ptr _tmpE8E=({const char*_tmp8BB="too many parameters for type %s";_tag_dyneither(_tmp8BB,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpE8F,_tmpE8E,_tag_dyneither(_tmp8BA,sizeof(void*),1U));});});});
if(tvs != 0){
# 4573
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpE90=({struct Cyc_List_List*_tmp8BD=_cycalloc(sizeof(*_tmp8BD));_tmp8BD->hd=e;_tmp8BD->tl=hidden_ts;_tmp8BD;});hidden_ts=_tmpE90;});
({struct Cyc_Tcutil_CVTEnv _tmpE91=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpE91;});}
# 4580
({struct Cyc_List_List*_tmpE93=({struct Cyc_List_List*_tmpE92=*_tmp8FC;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE92,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp8FC=_tmpE93;});}
# 4582
if((allow_abs_aggr  && _tmp8C1->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4587
_tmp8C1->expected_mem_kind=1;}}_LL112:;}
# 4590
goto _LL0;case 17U: _LL29: _tmp901=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp767)->f1;_tmp900=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp767)->f2;_tmp8FF=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp767)->f3;_tmp8FE=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp767)->f4;_LL2A: {
# 4593
struct Cyc_List_List*targs=*_tmp900;
# 4595
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp8C2;_push_handler(& _tmp8C2);{int _tmp8C4=0;if(setjmp(_tmp8C2.handler))_tmp8C4=1;if(!_tmp8C4){({struct Cyc_Absyn_Typedefdecl*_tmpE94=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp901);td=_tmpE94;});;_pop_handler();}else{void*_tmp8C3=(void*)_exn_thrown;void*_tmp8C5=_tmp8C3;void*_tmp8C9;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8C5)->tag == Cyc_Dict_Absent){_LL122: _LL123:
# 4598
({struct Cyc_String_pa_PrintArg_struct _tmp8C8;_tmp8C8.tag=0U;({struct _dyneither_ptr _tmpE95=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp901));_tmp8C8.f1=_tmpE95;});({void*_tmp8C6[1U]={& _tmp8C8};({unsigned int _tmpE97=loc;struct _dyneither_ptr _tmpE96=({const char*_tmp8C7="unbound typedef name %s";_tag_dyneither(_tmp8C7,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpE97,_tmpE96,_tag_dyneither(_tmp8C6,sizeof(void*),1U));});});});
return cvtenv;}else{_LL124: _tmp8C9=_tmp8C5;_LL125:(int)_rethrow(_tmp8C9);}_LL121:;}};}
# 4601
*_tmp8FF=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4606
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4611
({struct Cyc_Tcutil_CVTEnv _tmpE98=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpE98;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmpE9A=({struct Cyc_List_List*_tmp8CA=_cycalloc(sizeof(*_tmp8CA));({struct _tuple16*_tmpE99=({struct _tuple16*_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8CB->f2=(void*)ts->hd;_tmp8CB;});_tmp8CA->hd=_tmpE99;});_tmp8CA->tl=inst;_tmp8CA;});inst=_tmpE9A;});}
# 4615
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8CE;_tmp8CE.tag=0U;({struct _dyneither_ptr _tmpE9B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp901));_tmp8CE.f1=_tmpE9B;});({void*_tmp8CC[1U]={& _tmp8CE};({unsigned int _tmpE9D=loc;struct _dyneither_ptr _tmpE9C=({const char*_tmp8CD="too many parameters for typedef %s";_tag_dyneither(_tmp8CD,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpE9D,_tmpE9C,_tag_dyneither(_tmp8CC,sizeof(void*),1U));});});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4620
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpE9E=({struct Cyc_List_List*_tmp8CF=_cycalloc(sizeof(*_tmp8CF));_tmp8CF->hd=e;_tmp8CF->tl=hidden_ts;_tmp8CF;});hidden_ts=_tmpE9E;});
({struct Cyc_Tcutil_CVTEnv _tmpE9F=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpE9F;});
({struct Cyc_List_List*_tmpEA1=({struct Cyc_List_List*_tmp8D0=_cycalloc(sizeof(*_tmp8D0));({struct _tuple16*_tmpEA0=({struct _tuple16*_tmp8D1=_cycalloc(sizeof(*_tmp8D1));_tmp8D1->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8D1->f2=e;_tmp8D1;});_tmp8D0->hd=_tmpEA0;});_tmp8D0->tl=inst;_tmp8D0;});inst=_tmpEA1;});}
# 4628
({struct Cyc_List_List*_tmpEA3=({struct Cyc_List_List*_tmpEA2=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpEA2,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp900=_tmpEA3;});}
# 4630
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp8FE=new_typ;}
# 4636
goto _LL0;};}case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL0;case 15U: _LL31: _tmp902=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp767)->f1;_LL32:
# 4641
({struct Cyc_Tcutil_CVTEnv _tmpEA4=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp902,1,allow_abs_aggr);cvtenv=_tmpEA4;});goto _LL0;case 16U: _LL33: _tmp904=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp767)->f1;_tmp903=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp767)->f2;_LL34:
# 4644
({struct Cyc_Tcutil_CVTEnv _tmpEA5=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp904,0,0);cvtenv=_tmpEA5;});
({struct Cyc_Tcutil_CVTEnv _tmpEA6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp903,1,0);cvtenv=_tmpEA6;});
goto _LL0;case 23U: _LL35: _tmp905=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp767)->f1;_LL36:
# 4648
({struct Cyc_Tcutil_CVTEnv _tmpEA7=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp905,1,1);cvtenv=_tmpEA7;});goto _LL0;case 25U: _LL37: _tmp906=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp767)->f1;_LL38:
# 4650
({struct Cyc_Tcutil_CVTEnv _tmpEA8=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp906,1,1);cvtenv=_tmpEA8;});goto _LL0;default: _LL39: _tmp907=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp767)->f1;_LL3A:
# 4652
 for(0;_tmp907 != 0;_tmp907=_tmp907->tl){
({struct Cyc_Tcutil_CVTEnv _tmpEA9=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp907->hd,1,1);cvtenv=_tmpEA9;});}
goto _LL0;}_LL0:;}
# 4656
if(!({struct Cyc_Absyn_Kind*_tmpEAA=Cyc_Tcutil_typ_kind(t);Cyc_Tcutil_kind_leq(_tmpEAA,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp90C;_tmp90C.tag=0U;({struct _dyneither_ptr _tmpEAB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));_tmp90C.f1=_tmpEAB;});({struct Cyc_String_pa_PrintArg_struct _tmp90B;_tmp90B.tag=0U;({struct _dyneither_ptr _tmpEAC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));_tmp90B.f1=_tmpEAC;});({struct Cyc_String_pa_PrintArg_struct _tmp90A;_tmp90A.tag=0U;({struct _dyneither_ptr _tmpEAD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp90A.f1=_tmpEAD;});({void*_tmp908[3U]={& _tmp90A,& _tmp90B,& _tmp90C};({unsigned int _tmpEAF=loc;struct _dyneither_ptr _tmpEAE=({const char*_tmp909="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp909,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpEAF,_tmpEAE,_tag_dyneither(_tmp908,sizeof(void*),3U));});});});});});
# 4660
return cvtenv;}
# 4668
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4670
{void*_tmp90D=e->r;void*_tmp90E=_tmp90D;struct Cyc_Absyn_Exp*_tmp920;void*_tmp91F;void*_tmp91E;void*_tmp91D;void*_tmp91C;struct Cyc_Absyn_Exp*_tmp91B;struct Cyc_Absyn_Exp*_tmp91A;struct Cyc_Absyn_Exp*_tmp919;struct Cyc_Absyn_Exp*_tmp918;struct Cyc_Absyn_Exp*_tmp917;struct Cyc_Absyn_Exp*_tmp916;struct Cyc_Absyn_Exp*_tmp915;struct Cyc_Absyn_Exp*_tmp914;struct Cyc_Absyn_Exp*_tmp913;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_List_List*_tmp911;switch(*((int*)_tmp90E)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp911=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp90E)->f2;_LLC:
# 4677
 for(0;_tmp911 != 0;_tmp911=_tmp911->tl){
({struct Cyc_Tcutil_CVTEnv _tmpEB0=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp911->hd,te,cvtenv);cvtenv=_tmpEB0;});}
goto _LL0;case 6U: _LLD: _tmp914=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_tmp913=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp90E)->f2;_tmp912=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp90E)->f3;_LLE:
# 4681
({struct Cyc_Tcutil_CVTEnv _tmpEB1=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp914,te,cvtenv);cvtenv=_tmpEB1;});
({struct Cyc_Tcutil_CVTEnv _tmpEB2=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp913,te,cvtenv);cvtenv=_tmpEB2;});
({struct Cyc_Tcutil_CVTEnv _tmpEB3=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp912,te,cvtenv);cvtenv=_tmpEB3;});
goto _LL0;case 7U: _LLF: _tmp916=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_tmp915=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp90E)->f2;_LL10:
 _tmp918=_tmp916;_tmp917=_tmp915;goto _LL12;case 8U: _LL11: _tmp918=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_tmp917=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp90E)->f2;_LL12:
 _tmp91A=_tmp918;_tmp919=_tmp917;goto _LL14;case 9U: _LL13: _tmp91A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_tmp919=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp90E)->f2;_LL14:
# 4688
({struct Cyc_Tcutil_CVTEnv _tmpEB4=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91A,te,cvtenv);cvtenv=_tmpEB4;});
({struct Cyc_Tcutil_CVTEnv _tmpEB5=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp919,te,cvtenv);cvtenv=_tmpEB5;});
goto _LL0;case 14U: _LL15: _tmp91C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_tmp91B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp90E)->f2;_LL16:
# 4692
({struct Cyc_Tcutil_CVTEnv _tmpEB6=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91B,te,cvtenv);cvtenv=_tmpEB6;});
({struct Cyc_Tcutil_CVTEnv _tmpEB7=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp91C,1,0);cvtenv=_tmpEB7;});
goto _LL0;case 19U: _LL17: _tmp91D=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_LL18:
 _tmp91E=_tmp91D;goto _LL1A;case 17U: _LL19: _tmp91E=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_LL1A:
# 4697
({struct Cyc_Tcutil_CVTEnv _tmpEB8=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp91E,1,0);cvtenv=_tmpEB8;});
goto _LL0;case 39U: _LL1B: _tmp91F=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_LL1C:
# 4700
({struct Cyc_Tcutil_CVTEnv _tmpEB9=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp91F,1,0);cvtenv=_tmpEB9;});
goto _LL0;case 18U: _LL1D: _tmp920=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp90E)->f1;_LL1E:
# 4703
({struct Cyc_Tcutil_CVTEnv _tmpEBA=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp920,te,cvtenv);cvtenv=_tmpEBA;});
goto _LL0;default: _LL1F: _LL20:
# 4706
({void*_tmp90F=0U;({struct _dyneither_ptr _tmpEBB=({const char*_tmp910="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp910,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEBB,_tag_dyneither(_tmp90F,sizeof(void*),0U));});});}_LL0:;}
# 4708
return cvtenv;}
# 4711
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4716
struct Cyc_List_List*_tmp921=cvt.kind_env;
({struct Cyc_Tcutil_CVTEnv _tmpEBC=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);cvt=_tmpEBC;});
# 4719
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp922=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp923=_tmp922;struct Cyc_Absyn_Tvar*_tmp924;_LL1: _tmp924=_tmp923.f1;_LL2:;
({struct Cyc_List_List*_tmpEBD=Cyc_Tcutil_fast_add_free_tvar(_tmp921,_tmp924);cvt.kind_env=_tmpEBD;});}}
# 4727
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp925=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp926=_tmp925;void*_tmp92F;_LL4: _tmp92F=_tmp926.f1;_LL5:;{
void*_tmp927=Cyc_Tcutil_compress(_tmp92F);void*_tmp928=_tmp927;struct Cyc_Core_Opt**_tmp92E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp928)->tag == 1U){_LL7: _tmp92E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp928)->f4;_LL8:
# 4731
 if(*_tmp92E == 0)
({struct Cyc_Core_Opt*_tmpEBE=({struct Cyc_Core_Opt*_tmp929=_cycalloc(sizeof(*_tmp929));_tmp929->v=_tmp921;_tmp929;});*_tmp92E=_tmpEBE;});else{
# 4735
struct Cyc_List_List*_tmp92A=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp92E))->v;
struct Cyc_List_List*_tmp92B=0;
for(0;_tmp92A != 0;_tmp92A=_tmp92A->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp921,(struct Cyc_Absyn_Tvar*)_tmp92A->hd))
({struct Cyc_List_List*_tmpEBF=({struct Cyc_List_List*_tmp92C=_cycalloc(sizeof(*_tmp92C));_tmp92C->hd=(struct Cyc_Absyn_Tvar*)_tmp92A->hd;_tmp92C->tl=_tmp92B;_tmp92C;});_tmp92B=_tmpEBF;});}
({struct Cyc_Core_Opt*_tmpEC0=({struct Cyc_Core_Opt*_tmp92D=_cycalloc(sizeof(*_tmp92D));_tmp92D->v=_tmp92B;_tmp92D;});*_tmp92E=_tmpEC0;});}
# 4742
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4746
return cvt;}
# 4752
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp930=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp931=_tmp930;switch(((struct Cyc_Absyn_Kind*)_tmp931)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp931)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
# 4757
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp932=0U;({struct _dyneither_ptr _tmpEC1=({const char*_tmp933="can't unify evar with unique region!";_tag_dyneither(_tmp933,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEC1,_tag_dyneither(_tmp932,sizeof(void*),0U));});});
goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 goto _LL6;default: _LL5: _LL6:
# 4762
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp934=0U;({struct _dyneither_ptr _tmpEC2=({const char*_tmp935="can't unify evar with heap!";_tag_dyneither(_tmp935,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEC2,_tag_dyneither(_tmp934,sizeof(void*),0U));});});
goto _LL0;}case Cyc_Absyn_EffKind: _LL7: _LL8:
# 4765
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp936=0U;({struct _dyneither_ptr _tmpEC3=({const char*_tmp937="can't unify evar with {}!";_tag_dyneither(_tmp937,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEC3,_tag_dyneither(_tmp936,sizeof(void*),0U));});});
goto _LL0;default: _LL9: _LLA:
# 4768
({struct Cyc_String_pa_PrintArg_struct _tmp93B;_tmp93B.tag=0U;({struct _dyneither_ptr _tmpEC4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));_tmp93B.f1=_tmpEC4;});({struct Cyc_String_pa_PrintArg_struct _tmp93A;_tmp93A.tag=0U;({struct _dyneither_ptr _tmpEC5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp93A.f1=_tmpEC5;});({void*_tmp938[2U]={& _tmp93A,& _tmp93B};({unsigned int _tmpEC7=loc;struct _dyneither_ptr _tmpEC6=({const char*_tmp939="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp939,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpEC7,_tmpEC6,_tag_dyneither(_tmp938,sizeof(void*),2U));});});});});
goto _LL0;}_LL0:;}}
# 4780
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp93C=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp93D=({unsigned int _tmpECB=loc;struct Cyc_Tcenv_Tenv*_tmpECA=te;struct Cyc_Tcutil_CVTEnv _tmpEC9=({struct Cyc_Tcutil_CVTEnv _tmp95B;_tmp95B.r=Cyc_Core_heap_region;_tmp95B.kind_env=_tmp93C;_tmp95B.free_vars=0;_tmp95B.free_evars=0;_tmp95B.generalize_evars=generalize_evars;_tmp95B.fn_result=0;_tmp95B;});struct Cyc_Absyn_Kind*_tmpEC8=expected_kind;Cyc_Tcutil_check_valid_type(_tmpECB,_tmpECA,_tmpEC9,_tmpEC8,1,t);});
# 4788
struct Cyc_List_List*_tmp93E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp93D.free_vars);
struct Cyc_List_List*_tmp93F=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp93D.free_evars);
# 4792
if(_tmp93C != 0){
struct Cyc_List_List*_tmp940=0;
{struct Cyc_List_List*_tmp941=_tmp93E;for(0;(unsigned int)_tmp941;_tmp941=_tmp941->tl){
struct Cyc_Absyn_Tvar*_tmp942=(struct Cyc_Absyn_Tvar*)_tmp941->hd;
int found=0;
{struct Cyc_List_List*_tmp943=_tmp93C;for(0;(unsigned int)_tmp943;_tmp943=_tmp943->tl){
if(Cyc_Absyn_tvar_cmp(_tmp942,(struct Cyc_Absyn_Tvar*)_tmp943->hd)== 0){found=1;break;}}}
if(!found)
({struct Cyc_List_List*_tmpECC=({struct Cyc_List_List*_tmp944=_cycalloc(sizeof(*_tmp944));_tmp944->hd=(struct Cyc_Absyn_Tvar*)_tmp941->hd;_tmp944->tl=_tmp940;_tmp944;});_tmp940=_tmpECC;});}}
# 4802
({struct Cyc_List_List*_tmpECD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp940);_tmp93E=_tmpECD;});}
# 4808
{struct Cyc_List_List*x=_tmp93E;for(0;x != 0;x=x->tl){
void*_tmp945=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp946=_tmp945;enum Cyc_Absyn_AliasQual _tmp954;struct Cyc_Core_Opt**_tmp953;struct Cyc_Absyn_Kind*_tmp952;struct Cyc_Core_Opt**_tmp951;struct Cyc_Core_Opt**_tmp950;struct Cyc_Core_Opt**_tmp94F;struct Cyc_Core_Opt**_tmp94E;struct Cyc_Core_Opt**_tmp94D;struct Cyc_Core_Opt**_tmp94C;switch(*((int*)_tmp946)){case 1U: _LL1: _tmp94C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp946)->f1;_LL2:
 _tmp94D=_tmp94C;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp94D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f1;_LL4:
 _tmp94E=_tmp94D;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp94E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f1;_LL6:
 _tmp94F=_tmp94E;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp94F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f1;_LL8:
# 4814
({struct Cyc_Core_Opt*_tmpECE=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp94F=_tmpECE;});goto _LL0;default: _LL9: _tmp950=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f1;_LLA:
# 4816
({struct Cyc_Core_Opt*_tmpECF=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp950=_tmpECF;});goto _LL0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp951=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f1;_LLC:
# 4818
({struct Cyc_Core_Opt*_tmpED0=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp951=_tmpED0;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp953=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f1;_tmp952=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp946)->f2;_LLE:
# 4820
({struct Cyc_Core_Opt*_tmpED1=Cyc_Tcutil_kind_to_bound_opt(_tmp952);*_tmp953=_tmpED1;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp946)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp954=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp946)->f1)->aliasqual;_LL10:
# 4822
({struct Cyc_String_pa_PrintArg_struct _tmp94A;_tmp94A.tag=0U;({struct _dyneither_ptr _tmpED2=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp94B=_cycalloc_atomic(sizeof(*_tmp94B));_tmp94B->kind=Cyc_Absyn_MemKind;_tmp94B->aliasqual=_tmp954;_tmp94B;})));_tmp94A.f1=_tmpED2;});({struct Cyc_String_pa_PrintArg_struct _tmp949;_tmp949.tag=0U;({struct _dyneither_ptr _tmpED3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmp949.f1=_tmpED3;});({void*_tmp947[2U]={& _tmp949,& _tmp94A};({unsigned int _tmpED5=loc;struct _dyneither_ptr _tmpED4=({const char*_tmp948="type variable %s cannot have kind %s";_tag_dyneither(_tmp948,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpED5,_tmpED4,_tag_dyneither(_tmp947,sizeof(void*),2U));});});});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4829
if(_tmp93E != 0  || _tmp93F != 0){
{void*_tmp955=Cyc_Tcutil_compress(t);void*_tmp956=_tmp955;struct Cyc_List_List**_tmp957;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->tag == 9U){_LL14: _tmp957=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).tvars;_LL15:
# 4832
 if(*_tmp957 == 0){
# 4834
({struct Cyc_List_List*_tmpED6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp93E);*_tmp957=_tmpED6;});
_tmp93E=0;}
# 4837
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4840
if(_tmp93E != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp95A;_tmp95A.tag=0U;_tmp95A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp93E->hd)->name);({void*_tmp958[1U]={& _tmp95A};({unsigned int _tmpED8=loc;struct _dyneither_ptr _tmpED7=({const char*_tmp959="unbound type variable %s";_tag_dyneither(_tmp959,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpED8,_tmpED7,_tag_dyneither(_tmp958,sizeof(void*),1U));});});});
Cyc_Tcutil_check_free_evars(_tmp93F,t,loc);}}
# 4850
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4853
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp95C=Cyc_Tcutil_compress(t);void*_tmp95D=_tmp95C;struct Cyc_List_List*_tmp969;void*_tmp968;struct Cyc_Absyn_Tqual _tmp967;void*_tmp966;struct Cyc_List_List*_tmp965;struct Cyc_Absyn_Exp*_tmp964;struct Cyc_List_List*_tmp963;struct Cyc_Absyn_Exp*_tmp962;struct Cyc_List_List*_tmp961;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->tag == 9U){_LL1: _tmp969=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).tvars;_tmp968=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).effect;_tmp967=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).ret_tqual;_tmp966=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).ret_typ;_tmp965=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).args;_tmp964=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).requires_clause;_tmp963=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).requires_relns;_tmp962=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).ensures_clause;_tmp961=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp95D)->f1).ensures_relns;_LL2:
# 4856
 fd->tvs=_tmp969;
fd->effect=_tmp968;
{struct Cyc_List_List*_tmp95E=fd->args;for(0;_tmp95E != 0;(_tmp95E=_tmp95E->tl,_tmp965=_tmp965->tl)){
# 4860
(*((struct _tuple10*)_tmp95E->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp965))->hd)).f2;
(*((struct _tuple10*)_tmp95E->hd)).f3=(*((struct _tuple10*)_tmp965->hd)).f3;}}
# 4863
fd->ret_tqual=_tmp967;
fd->ret_type=_tmp966;
({int _tmpED9=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp966);(fd->ret_tqual).real_const=_tmpED9;});
fd->requires_clause=_tmp964;
fd->requires_relns=_tmp963;
fd->ensures_clause=_tmp962;
fd->ensures_relns=_tmp961;
goto _LL0;}else{_LL3: _LL4:
({void*_tmp95F=0U;({struct _dyneither_ptr _tmpEDA=({const char*_tmp960="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp960,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEDA,_tag_dyneither(_tmp95F,sizeof(void*),0U));});});}_LL0:;}
# 4873
({struct Cyc_List_List*_tmpEDC=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args);unsigned int _tmpEDB=loc;Cyc_Tcutil_check_unique_vars(_tmpEDC,_tmpEDB,({const char*_tmp96A="function declaration has repeated parameter";_tag_dyneither(_tmp96A,sizeof(char),44U);}));});
# 4876
fd->cached_typ=t;}
# 4881
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4887
struct Cyc_Tcutil_CVTEnv _tmp96B=({unsigned int _tmpEE1=loc;struct Cyc_Tcenv_Tenv*_tmpEE0=te;struct Cyc_Tcutil_CVTEnv _tmpEDF=({struct Cyc_Tcutil_CVTEnv _tmp973;_tmp973.r=Cyc_Core_heap_region;_tmp973.kind_env=bound_tvars;_tmp973.free_vars=0;_tmp973.free_evars=0;_tmp973.generalize_evars=0;_tmp973.fn_result=0;_tmp973;});struct Cyc_Absyn_Kind*_tmpEDE=expected_kind;int _tmpEDD=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpEE1,_tmpEE0,_tmpEDF,_tmpEDE,_tmpEDD,t);});
# 4891
struct Cyc_List_List*_tmp96C=({struct _RegionHandle*_tmpEE3=Cyc_Core_heap_region;struct Cyc_List_List*_tmpEE2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp96B.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpEE3,_tmpEE2,bound_tvars);});
# 4893
struct Cyc_List_List*_tmp96D=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp96B.free_evars);
{struct Cyc_List_List*fs=_tmp96C;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp96E=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp972;_tmp972.tag=0U;({struct _dyneither_ptr _tmpEE4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp972.f1=_tmpEE4;});({struct Cyc_String_pa_PrintArg_struct _tmp971;_tmp971.tag=0U;_tmp971.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp96E);({void*_tmp96F[2U]={& _tmp971,& _tmp972};({unsigned int _tmpEE6=loc;struct _dyneither_ptr _tmpEE5=({const char*_tmp970="unbound type variable %s in type %s";_tag_dyneither(_tmp970,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpEE6,_tmpEE5,_tag_dyneither(_tmp96F,sizeof(void*),2U));});});});});}}
# 4898
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp96D,t,loc);}
# 4902
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpEE7=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpEE7;});}
# 4907
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4913
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4916
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp977;_tmp977.tag=0U;({struct _dyneither_ptr _tmpEE8=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmp977.f1=_tmpEE8;});({struct Cyc_String_pa_PrintArg_struct _tmp976;_tmp976.tag=0U;_tmp976.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp974[2U]={& _tmp976,& _tmp977};({unsigned int _tmpEEA=loc;struct _dyneither_ptr _tmpEE9=({const char*_tmp975="%s: %s";_tag_dyneither(_tmp975,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpEEA,_tmpEE9,_tag_dyneither(_tmp974,sizeof(void*),2U));});});});});}}}
# 4922
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4926
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4930
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpEEC=tvs;unsigned int _tmpEEB=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpEEC,_tmpEEB,Cyc_Absynpp_tvar2string,({const char*_tmp978="duplicate type variable";_tag_dyneither(_tmp978,sizeof(char),24U);}));});}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4944 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4949
struct _RegionHandle _tmp979=_new_region("temp");struct _RegionHandle*temp=& _tmp979;_push_region(temp);
# 4953
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpEED=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpEED,({const char*_tmp97A="";_tag_dyneither(_tmp97A,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpEEF=({struct Cyc_List_List*_tmp97B=_region_malloc(temp,sizeof(*_tmp97B));({struct _tuple32*_tmpEEE=({struct _tuple32*_tmp97C=_region_malloc(temp,sizeof(*_tmp97C));_tmp97C->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp97C->f2=0;_tmp97C;});_tmp97B->hd=_tmpEEE;});_tmp97B->tl=fields;_tmp97B;});fields=_tmpEEF;});}}
# 4958
({struct Cyc_List_List*_tmpEF0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);fields=_tmpEF0;});{
# 4960
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp9A2="struct";_tag_dyneither(_tmp9A2,sizeof(char),7U);}):({const char*_tmp9A3="union";_tag_dyneither(_tmp9A3,sizeof(char),6U);});
# 4963
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp97D=(struct _tuple33*)des->hd;struct _tuple33*_tmp97E=_tmp97D;struct Cyc_List_List*_tmp99A;void*_tmp999;_LL1: _tmp99A=_tmp97E->f1;_tmp999=_tmp97E->f2;_LL2:;
if(_tmp99A == 0){
# 4968
struct Cyc_List_List*_tmp97F=fields;
for(0;_tmp97F != 0;_tmp97F=_tmp97F->tl){
if(!(*((struct _tuple32*)_tmp97F->hd)).f2){
(*((struct _tuple32*)_tmp97F->hd)).f2=1;
({struct Cyc_List_List*_tmpEF3=({struct Cyc_List_List*_tmp980=_cycalloc(sizeof(*_tmp980));({void*_tmpEF2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp981=_cycalloc(sizeof(*_tmp981));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpEF1=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp982;_tmp982.tag=1U;_tmp982.f1=((*((struct _tuple32*)_tmp97F->hd)).f1)->name;_tmp982;});_tmp981[0]=_tmpEF1;});_tmp981;});_tmp980->hd=_tmpEF2;});_tmp980->tl=0;_tmp980;});(*((struct _tuple33*)des->hd)).f1=_tmpEF3;});
({struct Cyc_List_List*_tmpEF5=({struct Cyc_List_List*_tmp983=_region_malloc(rgn,sizeof(*_tmp983));({struct _tuple34*_tmpEF4=({struct _tuple34*_tmp984=_region_malloc(rgn,sizeof(*_tmp984));_tmp984->f1=(*((struct _tuple32*)_tmp97F->hd)).f1;_tmp984->f2=_tmp999;_tmp984;});_tmp983->hd=_tmpEF4;});_tmp983->tl=ans;_tmp983;});ans=_tmpEF5;});
break;}}
# 4976
if(_tmp97F == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp987;_tmp987.tag=0U;_tmp987.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp985[1U]={& _tmp987};({unsigned int _tmpEF7=loc;struct _dyneither_ptr _tmpEF6=({const char*_tmp986="too many arguments to %s";_tag_dyneither(_tmp986,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpEF7,_tmpEF6,_tag_dyneither(_tmp985,sizeof(void*),1U));});});});}else{
if(_tmp99A->tl != 0)
# 4980
({void*_tmp988=0U;({unsigned int _tmpEF9=loc;struct _dyneither_ptr _tmpEF8=({const char*_tmp989="multiple designators are not yet supported";_tag_dyneither(_tmp989,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpEF9,_tmpEF8,_tag_dyneither(_tmp988,sizeof(void*),0U));});});else{
# 4983
void*_tmp98A=(void*)_tmp99A->hd;void*_tmp98B=_tmp98A;struct _dyneither_ptr*_tmp998;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp98B)->tag == 0U){_LL4: _LL5:
# 4985
({struct Cyc_String_pa_PrintArg_struct _tmp98E;_tmp98E.tag=0U;_tmp98E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp98C[1U]={& _tmp98E};({unsigned int _tmpEFB=loc;struct _dyneither_ptr _tmpEFA=({const char*_tmp98D="array designator used in argument to %s";_tag_dyneither(_tmp98D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpEFB,_tmpEFA,_tag_dyneither(_tmp98C,sizeof(void*),1U));});});});
goto _LL3;}else{_LL6: _tmp998=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp98B)->f1;_LL7: {
# 4988
struct Cyc_List_List*_tmp98F=fields;
for(0;_tmp98F != 0;_tmp98F=_tmp98F->tl){
if(Cyc_strptrcmp(_tmp998,((*((struct _tuple32*)_tmp98F->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp98F->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp992;_tmp992.tag=0U;_tmp992.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp998);({void*_tmp990[1U]={& _tmp992};({unsigned int _tmpEFD=loc;struct _dyneither_ptr _tmpEFC=({const char*_tmp991="member %s has already been used as an argument";_tag_dyneither(_tmp991,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpEFD,_tmpEFC,_tag_dyneither(_tmp990,sizeof(void*),1U));});});});
(*((struct _tuple32*)_tmp98F->hd)).f2=1;
({struct Cyc_List_List*_tmpEFF=({struct Cyc_List_List*_tmp993=_region_malloc(rgn,sizeof(*_tmp993));({struct _tuple34*_tmpEFE=({struct _tuple34*_tmp994=_region_malloc(rgn,sizeof(*_tmp994));_tmp994->f1=(*((struct _tuple32*)_tmp98F->hd)).f1;_tmp994->f2=_tmp999;_tmp994;});_tmp993->hd=_tmpEFE;});_tmp993->tl=ans;_tmp993;});ans=_tmpEFF;});
break;}}
# 4997
if(_tmp98F == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp997;_tmp997.tag=0U;_tmp997.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp998);({void*_tmp995[1U]={& _tmp997};({unsigned int _tmpF01=loc;struct _dyneither_ptr _tmpF00=({const char*_tmp996="bad field designator %s";_tag_dyneither(_tmp996,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpF01,_tmpF00,_tag_dyneither(_tmp995,sizeof(void*),1U));});});});
goto _LL3;}}_LL3:;}}}
# 5002
if(aggr_kind == Cyc_Absyn_StructA)
# 5004
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp99B=0U;({unsigned int _tmpF03=loc;struct _dyneither_ptr _tmpF02=({const char*_tmp99C="too few arguments to struct";_tag_dyneither(_tmp99C,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpF03,_tmpF02,_tag_dyneither(_tmp99B,sizeof(void*),0U));});});
break;}}else{
# 5011
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp99D=0U;({unsigned int _tmpF05=loc;struct _dyneither_ptr _tmpF04=({const char*_tmp99E="only one member of a union is allowed";_tag_dyneither(_tmp99E,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpF05,_tmpF04,_tag_dyneither(_tmp99D,sizeof(void*),0U));});});
found=1;}}
# 5018
if(!found)({void*_tmp99F=0U;({unsigned int _tmpF07=loc;struct _dyneither_ptr _tmpF06=({const char*_tmp9A0="missing member for union";_tag_dyneither(_tmp9A0,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpF07,_tmpF06,_tag_dyneither(_tmp99F,sizeof(void*),0U));});});}{
# 5021
struct Cyc_List_List*_tmp9A1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp9A1;};};}
# 4953
;_pop_region(temp);}
# 5024
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9A4=Cyc_Tcutil_compress(t);void*_tmp9A5=_tmp9A4;void*_tmp9A9;union Cyc_Absyn_Constraint*_tmp9A8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A5)->tag == 5U){_LL1: _tmp9A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A5)->f1).elt_typ;_tmp9A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A5)->f1).ptr_atts).bounds;_LL2: {
# 5027
void*_tmp9A6=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9A8);void*_tmp9A7=_tmp9A6;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9A7)->tag == 0U){_LL6: _LL7:
# 5029
*elt_typ_dest=_tmp9A9;
return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 5033
 return 0;}_LL0:;}
# 5037
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9AA=Cyc_Tcutil_compress(t);void*_tmp9AB=_tmp9AA;void*_tmp9AD;union Cyc_Absyn_Constraint*_tmp9AC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AB)->tag == 5U){_LL1: _tmp9AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AB)->f1).elt_typ;_tmp9AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AB)->f1).ptr_atts).zero_term;_LL2:
# 5040
*elt_typ_dest=_tmp9AD;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9AC);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5048
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5050
void*_tmp9AE=Cyc_Tcutil_compress(t);void*_tmp9AF=_tmp9AE;void*_tmp9B8;struct Cyc_Absyn_Tqual _tmp9B7;struct Cyc_Absyn_Exp*_tmp9B6;union Cyc_Absyn_Constraint*_tmp9B5;void*_tmp9B4;union Cyc_Absyn_Constraint*_tmp9B3;union Cyc_Absyn_Constraint*_tmp9B2;switch(*((int*)_tmp9AF)){case 5U: _LL1: _tmp9B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AF)->f1).elt_typ;_tmp9B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AF)->f1).ptr_atts).bounds;_tmp9B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9AF)->f1).ptr_atts).zero_term;_LL2:
# 5052
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9B2)){
*ptr_type=t;
*elt_type=_tmp9B4;
{void*_tmp9B0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp9B3);void*_tmp9B1=_tmp9B0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9B1)->tag == 0U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 5059
return 1;}else{
return 0;}case 8U: _LL3: _tmp9B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9AF)->f1).elt_type;_tmp9B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9AF)->f1).tq;_tmp9B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9AF)->f1).num_elts;_tmp9B5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9AF)->f1).zero_term;_LL4:
# 5062
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9B5)){
*elt_type=_tmp9B8;
*is_dyneither=0;
({void*_tmpF08=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);*ptr_type=_tmpF08;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5075
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5077
void*_tmp9B9=e1->r;void*_tmp9BA=_tmp9B9;struct Cyc_Absyn_Exp*_tmp9CC;struct Cyc_Absyn_Exp*_tmp9CB;struct Cyc_Absyn_Exp*_tmp9CA;struct Cyc_Absyn_Exp*_tmp9C9;struct Cyc_Absyn_Exp*_tmp9C8;struct Cyc_Absyn_Exp*_tmp9C7;switch(*((int*)_tmp9BA)){case 14U: _LL1: _LL2:
# 5079
({struct Cyc_String_pa_PrintArg_struct _tmp9BD;_tmp9BD.tag=0U;({struct _dyneither_ptr _tmpF09=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9BD.f1=_tmpF09;});({void*_tmp9BB[1U]={& _tmp9BD};({struct _dyneither_ptr _tmpF0A=({const char*_tmp9BC="we have a cast in a lhs:  %s";_tag_dyneither(_tmp9BC,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF0A,_tag_dyneither(_tmp9BB,sizeof(void*),1U));});});});case 20U: _LL3: _tmp9C7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9BA)->f1;_LL4:
 _tmp9C8=_tmp9C7;goto _LL6;case 23U: _LL5: _tmp9C8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9BA)->f1;_LL6:
# 5082
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9C8->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp9C9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9BA)->f1;_LL8:
 _tmp9CA=_tmp9C9;goto _LLA;case 21U: _LL9: _tmp9CA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9BA)->f1;_LLA:
# 5086
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9CA->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9C0;_tmp9C0.tag=0U;({struct _dyneither_ptr _tmpF0B=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));_tmp9C0.f1=_tmpF0B;});({void*_tmp9BE[1U]={& _tmp9C0};({struct _dyneither_ptr _tmpF0C=({const char*_tmp9BF="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp9BF,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF0C,_tag_dyneither(_tmp9BE,sizeof(void*),1U));});});});
return 0;case 13U: _LLB: _tmp9CB=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9BA)->f1;_LLC:
 _tmp9CC=_tmp9CB;goto _LLE;case 12U: _LLD: _tmp9CC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9BA)->f1;_LLE:
# 5092
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9CC->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9C3;_tmp9C3.tag=0U;({struct _dyneither_ptr _tmpF0D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));_tmp9C3.f1=_tmpF0D;});({void*_tmp9C1[1U]={& _tmp9C3};({struct _dyneither_ptr _tmpF0E=({const char*_tmp9C2="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp9C2,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF0E,_tag_dyneither(_tmp9C1,sizeof(void*),1U));});});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp9C6;_tmp9C6.tag=0U;({struct _dyneither_ptr _tmpF0F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9C6.f1=_tmpF0F;});({void*_tmp9C4[1U]={& _tmp9C6};({struct _dyneither_ptr _tmpF10=({const char*_tmp9C5="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp9C5,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF10,_tag_dyneither(_tmp9C4,sizeof(void*),1U));});});});}_LL0:;}
# 5101
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5112
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5115
void*_tmp9CD=Cyc_Tcutil_compress(r);void*_tmp9CE=_tmp9CD;struct Cyc_Absyn_Tvar*_tmp9DB;enum Cyc_Absyn_KindQual _tmp9DA;enum Cyc_Absyn_AliasQual _tmp9D9;switch(*((int*)_tmp9CE)){case 22U: _LL1: _LL2:
 return !must_be_unique;case 21U: _LL3: _LL4:
 return 1;case 17U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9CE)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9CE)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9CE)->f4 == 0){_LL5: _tmp9DA=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9CE)->f3)->kind)->v)->kind;_tmp9D9=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9CE)->f3)->kind)->v)->aliasqual;_LL6:
# 5119
 return _tmp9DA == Cyc_Absyn_RgnKind  && (_tmp9D9 == Cyc_Absyn_Unique  || _tmp9D9 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp9DB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9CE)->f1;_LL8: {
# 5123
struct Cyc_Absyn_Kind*_tmp9CF=Cyc_Tcutil_tvar_kind(_tmp9DB,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp9D0=_tmp9CF;enum Cyc_Absyn_KindQual _tmp9D8;enum Cyc_Absyn_AliasQual _tmp9D7;_LLC: _tmp9D8=_tmp9D0->kind;_tmp9D7=_tmp9D0->aliasqual;_LLD:;
if(_tmp9D8 == Cyc_Absyn_RgnKind  && (_tmp9D7 == Cyc_Absyn_Unique  || _tmp9D7 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp9D1=Cyc_Absyn_compress_kb(_tmp9DB->kind);void*_tmp9D2=_tmp9D1;struct Cyc_Core_Opt**_tmp9D6;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D2)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D2)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D2)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp9D6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9D2)->f1;_LL10:
# 5127
({struct Cyc_Core_Opt*_tmpF13=({struct Cyc_Core_Opt*_tmp9D3=_cycalloc(sizeof(*_tmp9D3));({void*_tmpF12=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9D4=_cycalloc(sizeof(*_tmp9D4));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF11=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9D5;_tmp9D5.tag=2U;_tmp9D5.f1=0;_tmp9D5.f2=& Cyc_Tcutil_rk;_tmp9D5;});_tmp9D4[0]=_tmpF11;});_tmp9D4;});_tmp9D3->v=_tmpF12;});_tmp9D3;});*_tmp9D6=_tmpF13;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5132
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5139
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp9DC=Cyc_Tcutil_compress(t);void*_tmp9DD=_tmp9DC;struct Cyc_Absyn_Tvar*_tmp9EC;void*_tmp9EB;switch(*((int*)_tmp9DD)){case 5U: _LL1: _tmp9EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9DD)->f1).ptr_atts).rgn;_LL2:
# 5142
 return Cyc_Tcutil_is_noalias_region(_tmp9EB,must_be_unique);case 2U: _LL3: _tmp9EC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9DD)->f1;_LL4: {
# 5144
struct Cyc_Absyn_Kind*_tmp9DE=Cyc_Tcutil_tvar_kind(_tmp9EC,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp9DF=_tmp9DE;enum Cyc_Absyn_KindQual _tmp9EA;enum Cyc_Absyn_AliasQual _tmp9E9;_LL8: _tmp9EA=_tmp9DF->kind;_tmp9E9=_tmp9DF->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp9E0=_tmp9EA;switch(_tmp9E0){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if(_tmp9E9 == Cyc_Absyn_Unique  || _tmp9E9 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp9E1=Cyc_Absyn_compress_kb(_tmp9EC->kind);void*_tmp9E2=_tmp9E1;struct Cyc_Core_Opt**_tmp9E8;enum Cyc_Absyn_KindQual _tmp9E7;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E2)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E2)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp9E8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E2)->f1;_tmp9E7=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9E2)->f2)->kind;_LL15:
# 5150
({struct Cyc_Core_Opt*_tmpF17=({struct Cyc_Core_Opt*_tmp9E3=_cycalloc(sizeof(*_tmp9E3));({void*_tmpF16=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9E4=_cycalloc(sizeof(*_tmp9E4));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF15=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9E5;_tmp9E5.tag=2U;_tmp9E5.f1=0;({struct Cyc_Absyn_Kind*_tmpF14=({struct Cyc_Absyn_Kind*_tmp9E6=_cycalloc_atomic(sizeof(*_tmp9E6));_tmp9E6->kind=_tmp9E7;_tmp9E6->aliasqual=Cyc_Absyn_Aliasable;_tmp9E6;});_tmp9E5.f2=_tmpF14;});_tmp9E5;});_tmp9E4[0]=_tmpF15;});_tmp9E4;});_tmp9E3->v=_tmpF16;});_tmp9E3;});*_tmp9E8=_tmpF17;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5154
 return 1;}_LL13:;}
# 5157
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5160
 return 0;}_LL0:;}
# 5163
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp9ED=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp9ED,0))return 1;{
void*_tmp9EE=_tmp9ED;union Cyc_Absyn_DatatypeFieldInfoU _tmpA02;struct Cyc_List_List*_tmpA01;union Cyc_Absyn_DatatypeInfoU _tmpA00;struct Cyc_List_List*_tmp9FF;struct Cyc_List_List*_tmp9FE;struct Cyc_Absyn_Aggrdecl**_tmp9FD;struct Cyc_List_List*_tmp9FC;struct Cyc_List_List*_tmp9FB;switch(*((int*)_tmp9EE)){case 10U: _LL1: _tmp9FB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9EE)->f1;_LL2:
# 5168
 while(_tmp9FB != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp9FB->hd)).f2))return 1;
_tmp9FB=_tmp9FB->tl;}
# 5172
return 0;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9EE)->f1).aggr_info).KnownAggr).tag == 2){_LL3: _tmp9FD=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9EE)->f1).aggr_info).KnownAggr).val;_tmp9FC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9EE)->f1).targs;_LL4:
# 5174
 if((*_tmp9FD)->impl == 0)return 0;else{
# 5176
struct Cyc_List_List*_tmp9EF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp9FD)->tvs,_tmp9FC);
struct Cyc_List_List*_tmp9F0=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp9FD)->impl))->fields;
void*t;
while(_tmp9F0 != 0){
({void*_tmpF18=_tmp9EF == 0?((struct Cyc_Absyn_Aggrfield*)_tmp9F0->hd)->type: Cyc_Tcutil_substitute(_tmp9EF,((struct Cyc_Absyn_Aggrfield*)_tmp9F0->hd)->type);t=_tmpF18;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp9F0=_tmp9F0->tl;}
# 5184
return 0;}}else{_LL7: _LL8:
# 5194
 return 0;}case 12U: _LL5: _tmp9FE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9EE)->f2;_LL6:
# 5187
 while(_tmp9FE != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp9FE->hd)->type))return 1;
_tmp9FE=_tmp9FE->tl;}
# 5191
return 0;case 3U: _LL9: _tmpA00=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9EE)->f1).datatype_info;_tmp9FF=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9EE)->f1).targs;_LLA: {
# 5196
union Cyc_Absyn_DatatypeInfoU _tmp9F1=_tmpA00;struct Cyc_List_List*_tmp9F5;struct Cyc_Core_Opt*_tmp9F4;struct _tuple2*_tmp9F3;int _tmp9F2;if((_tmp9F1.UnknownDatatype).tag == 1){_LL10: _tmp9F3=((_tmp9F1.UnknownDatatype).val).name;_tmp9F2=((_tmp9F1.UnknownDatatype).val).is_extensible;_LL11:
# 5199
 return 0;}else{_LL12: _tmp9F5=(*(_tmp9F1.KnownDatatype).val)->tvs;_tmp9F4=(*(_tmp9F1.KnownDatatype).val)->fields;_LL13:
# 5202
 return 0;}_LLF:;}case 4U: _LLB: _tmpA02=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9EE)->f1).field_info;_tmpA01=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9EE)->f1).targs;_LLC: {
# 5205
union Cyc_Absyn_DatatypeFieldInfoU _tmp9F6=_tmpA02;struct Cyc_Absyn_Datatypedecl*_tmp9FA;struct Cyc_Absyn_Datatypefield*_tmp9F9;if((_tmp9F6.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5208
 return 0;}else{_LL17: _tmp9FA=((_tmp9F6.KnownDatatypefield).val).f1;_tmp9F9=((_tmp9F6.KnownDatatypefield).val).f2;_LL18: {
# 5210
struct Cyc_List_List*_tmp9F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp9FA->tvs,_tmpA01);
struct Cyc_List_List*_tmp9F8=_tmp9F9->typs;
while(_tmp9F8 != 0){
({void*_tmpF19=_tmp9F7 == 0?(*((struct _tuple12*)_tmp9F8->hd)).f2: Cyc_Tcutil_substitute(_tmp9F7,(*((struct _tuple12*)_tmp9F8->hd)).f2);_tmp9ED=_tmpF19;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp9ED))return 1;
_tmp9F8=_tmp9F8->tl;}
# 5217
return 0;}}_LL14:;}default: _LLD: _LLE:
# 5219
 return 0;}_LL0:;};}
# 5226
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmpA03=e->r;void*_tmpA04=_tmpA03;struct Cyc_Absyn_Stmt*_tmpA1A;struct Cyc_Absyn_Exp*_tmpA19;struct Cyc_Absyn_Exp*_tmpA18;struct Cyc_Absyn_Exp*_tmpA17;struct Cyc_Absyn_Exp*_tmpA16;struct Cyc_Absyn_Exp*_tmpA15;struct Cyc_Absyn_Exp*_tmpA14;struct Cyc_Absyn_Exp*_tmpA13;struct _dyneither_ptr*_tmpA12;struct Cyc_Absyn_Exp*_tmpA11;struct Cyc_Absyn_Exp*_tmpA10;switch(*((int*)_tmpA04)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA04)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmpA10=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA04)->f1;_LL4:
 _tmpA11=_tmpA10;goto _LL6;case 20U: _LL5: _tmpA11=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA04)->f1;_LL6:
# 5231
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA11->topt),1) && Cyc_Tcutil_is_noalias_path(_tmpA11);case 21U: _LL7: _tmpA13=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA04)->f1;_tmpA12=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA04)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmpA13);case 23U: _LL9: _tmpA15=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA04)->f1;_tmpA14=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA04)->f2;_LLA: {
# 5234
void*_tmpA05=Cyc_Tcutil_compress((void*)_check_null(_tmpA15->topt));void*_tmpA06=_tmpA05;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA06)->tag == 10U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmpA15);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmpA17=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA04)->f2;_tmpA16=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA04)->f3;_LLC:
# 5239
 return Cyc_Tcutil_is_noalias_path(_tmpA17) && Cyc_Tcutil_is_noalias_path(_tmpA16);case 9U: _LLD: _tmpA18=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA04)->f2;_LLE:
 _tmpA19=_tmpA18;goto _LL10;case 14U: _LLF: _tmpA19=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA04)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmpA19);case 37U: _LL11: _tmpA1A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA04)->f1;_LL12:
# 5243
 while(1){
void*_tmpA07=_tmpA1A->r;void*_tmpA08=_tmpA07;struct Cyc_Absyn_Exp*_tmpA0F;struct Cyc_Absyn_Decl*_tmpA0E;struct Cyc_Absyn_Stmt*_tmpA0D;struct Cyc_Absyn_Stmt*_tmpA0C;struct Cyc_Absyn_Stmt*_tmpA0B;switch(*((int*)_tmpA08)){case 2U: _LL1B: _tmpA0C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA08)->f1;_tmpA0B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA08)->f2;_LL1C:
 _tmpA1A=_tmpA0B;goto _LL1A;case 12U: _LL1D: _tmpA0E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA08)->f1;_tmpA0D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA08)->f2;_LL1E:
 _tmpA1A=_tmpA0D;goto _LL1A;case 1U: _LL1F: _tmpA0F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA08)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmpA0F);default: _LL21: _LL22:
({void*_tmpA09=0U;({struct _dyneither_ptr _tmpF1A=({const char*_tmpA0A="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA0A,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF1A,_tag_dyneither(_tmpA09,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5251
 return 1;}_LL0:;}
# 5268 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5270
struct _tuple18 bogus_ans=({struct _tuple18 _tmpA62;_tmpA62.f1=0;_tmpA62.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA62;});
void*_tmpA1B=e->r;void*_tmpA1C=_tmpA1B;struct Cyc_Absyn_Exp*_tmpA61;struct Cyc_Absyn_Exp*_tmpA60;struct Cyc_Absyn_Exp*_tmpA5F;struct Cyc_Absyn_Exp*_tmpA5E;struct _dyneither_ptr*_tmpA5D;int _tmpA5C;struct Cyc_Absyn_Exp*_tmpA5B;struct _dyneither_ptr*_tmpA5A;int _tmpA59;void*_tmpA58;switch(*((int*)_tmpA1C)){case 1U: _LL1: _tmpA58=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA1C)->f1;_LL2: {
# 5274
void*_tmpA1D=_tmpA58;struct Cyc_Absyn_Vardecl*_tmpA2A;struct Cyc_Absyn_Vardecl*_tmpA29;struct Cyc_Absyn_Vardecl*_tmpA28;struct Cyc_Absyn_Vardecl*_tmpA27;switch(*((int*)_tmpA1D)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmpA27=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA1D)->f1;_LL13: {
# 5278
void*_tmpA1E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA1F=_tmpA1E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1F)->tag == 8U){_LL1B: _LL1C:
# 5280
 return({struct _tuple18 _tmpA20;_tmpA20.f1=1;_tmpA20.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA20;});}else{_LL1D: _LL1E:
 return({struct _tuple18 _tmpA21;_tmpA21.f1=(_tmpA27->tq).real_const;_tmpA21.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA21;});}_LL1A:;}case 4U: _LL14: _tmpA28=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA1D)->f1;_LL15: {
# 5284
void*_tmpA22=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA23=_tmpA22;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA23)->tag == 8U){_LL20: _LL21:
 return({struct _tuple18 _tmpA24;_tmpA24.f1=1;_tmpA24.f2=(void*)_check_null(_tmpA28->rgn);_tmpA24;});}else{_LL22: _LL23:
# 5287
 _tmpA28->escapes=1;
return({struct _tuple18 _tmpA25;_tmpA25.f1=(_tmpA28->tq).real_const;_tmpA25.f2=(void*)_check_null(_tmpA28->rgn);_tmpA25;});}_LL1F:;}case 5U: _LL16: _tmpA29=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA1D)->f1;_LL17:
# 5290
 _tmpA2A=_tmpA29;goto _LL19;default: _LL18: _tmpA2A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA1D)->f1;_LL19:
# 5292
 _tmpA2A->escapes=1;
return({struct _tuple18 _tmpA26;_tmpA26.f1=(_tmpA2A->tq).real_const;_tmpA26.f2=(void*)_check_null(_tmpA2A->rgn);_tmpA26;});}_LLD:;}case 21U: _LL3: _tmpA5B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA1C)->f1;_tmpA5A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA1C)->f2;_tmpA59=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA1C)->f3;_LL4:
# 5297
 if(_tmpA59)return bogus_ans;{
# 5300
void*_tmpA2B=Cyc_Tcutil_compress((void*)_check_null(_tmpA5B->topt));void*_tmpA2C=_tmpA2B;struct Cyc_Absyn_Aggrdecl*_tmpA3A;struct Cyc_List_List*_tmpA39;switch(*((int*)_tmpA2C)){case 12U: _LL25: _tmpA39=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA2C)->f2;_LL26: {
# 5302
struct Cyc_Absyn_Aggrfield*_tmpA2D=Cyc_Absyn_lookup_field(_tmpA39,_tmpA5A);
if(_tmpA2D != 0  && _tmpA2D->width == 0){
struct _tuple18 _tmpA2E=Cyc_Tcutil_addressof_props(te,_tmpA5B);struct _tuple18 _tmpA2F=_tmpA2E;int _tmpA32;void*_tmpA31;_LL2C: _tmpA32=_tmpA2F.f1;_tmpA31=_tmpA2F.f2;_LL2D:;
return({struct _tuple18 _tmpA30;_tmpA30.f1=(_tmpA2D->tq).real_const  || _tmpA32;_tmpA30.f2=_tmpA31;_tmpA30;});}
# 5307
return bogus_ans;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA2C)->f1).aggr_info).KnownAggr).tag == 2){_LL27: _tmpA3A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA2C)->f1).aggr_info).KnownAggr).val;_LL28: {
# 5309
struct Cyc_Absyn_Aggrfield*_tmpA33=Cyc_Absyn_lookup_decl_field(_tmpA3A,_tmpA5A);
if(_tmpA33 != 0  && _tmpA33->width == 0){
struct _tuple18 _tmpA34=Cyc_Tcutil_addressof_props(te,_tmpA5B);struct _tuple18 _tmpA35=_tmpA34;int _tmpA38;void*_tmpA37;_LL2F: _tmpA38=_tmpA35.f1;_tmpA37=_tmpA35.f2;_LL30:;
return({struct _tuple18 _tmpA36;_tmpA36.f1=(_tmpA33->tq).real_const  || _tmpA38;_tmpA36.f2=_tmpA37;_tmpA36;});}
# 5314
return bogus_ans;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmpA5E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA1C)->f1;_tmpA5D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA1C)->f2;_tmpA5C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA1C)->f3;_LL6:
# 5319
 if(_tmpA5C)return bogus_ans;{
# 5322
void*_tmpA3B=Cyc_Tcutil_compress((void*)_check_null(_tmpA5E->topt));void*_tmpA3C=_tmpA3B;void*_tmpA43;void*_tmpA42;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3C)->tag == 5U){_LL32: _tmpA43=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3C)->f1).elt_typ;_tmpA42=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3C)->f1).ptr_atts).rgn;_LL33: {
# 5324
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpA3D=Cyc_Tcutil_compress(_tmpA43);void*_tmpA3E=_tmpA3D;struct Cyc_Absyn_Aggrdecl*_tmpA40;struct Cyc_List_List*_tmpA3F;switch(*((int*)_tmpA3E)){case 12U: _LL37: _tmpA3F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA3E)->f2;_LL38:
# 5327
({struct Cyc_Absyn_Aggrfield*_tmpF1B=Cyc_Absyn_lookup_field(_tmpA3F,_tmpA5D);finfo=_tmpF1B;});goto _LL36;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA3E)->f1).aggr_info).KnownAggr).tag == 2){_LL39: _tmpA40=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA3E)->f1).aggr_info).KnownAggr).val;_LL3A:
# 5329
({struct Cyc_Absyn_Aggrfield*_tmpF1C=Cyc_Absyn_lookup_decl_field(_tmpA40,_tmpA5D);finfo=_tmpF1C;});goto _LL36;}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5332
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpA41;_tmpA41.f1=(finfo->tq).real_const;_tmpA41.f2=_tmpA42;_tmpA41;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmpA5F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA1C)->f1;_LL8: {
# 5339
void*_tmpA44=Cyc_Tcutil_compress((void*)_check_null(_tmpA5F->topt));void*_tmpA45=_tmpA44;struct Cyc_Absyn_Tqual _tmpA48;void*_tmpA47;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA45)->tag == 5U){_LL3E: _tmpA48=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA45)->f1).elt_tq;_tmpA47=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA45)->f1).ptr_atts).rgn;_LL3F:
# 5341
 return({struct _tuple18 _tmpA46;_tmpA46.f1=_tmpA48.real_const;_tmpA46.f2=_tmpA47;_tmpA46;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmpA61=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA1C)->f1;_tmpA60=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA1C)->f2;_LLA: {
# 5347
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpA61->topt));
void*_tmpA49=t;struct Cyc_Absyn_Tqual _tmpA55;struct Cyc_Absyn_Tqual _tmpA54;void*_tmpA53;struct Cyc_List_List*_tmpA52;switch(*((int*)_tmpA49)){case 10U: _LL43: _tmpA52=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA49)->f1;_LL44: {
# 5351
struct _tuple14 _tmpA4A=Cyc_Evexp_eval_const_uint_exp(_tmpA60);struct _tuple14 _tmpA4B=_tmpA4A;unsigned int _tmpA4F;int _tmpA4E;_LL4C: _tmpA4F=_tmpA4B.f1;_tmpA4E=_tmpA4B.f2;_LL4D:;
if(!_tmpA4E)
return bogus_ans;{
struct _tuple12*_tmpA4C=Cyc_Absyn_lookup_tuple_field(_tmpA52,(int)_tmpA4F);
if(_tmpA4C != 0)
return({struct _tuple18 _tmpA4D;_tmpA4D.f1=((*_tmpA4C).f1).real_const;({void*_tmpF1D=(Cyc_Tcutil_addressof_props(te,_tmpA61)).f2;_tmpA4D.f2=_tmpF1D;});_tmpA4D;});
return bogus_ans;};}case 5U: _LL45: _tmpA54=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA49)->f1).elt_tq;_tmpA53=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA49)->f1).ptr_atts).rgn;_LL46:
# 5359
 return({struct _tuple18 _tmpA50;_tmpA50.f1=_tmpA54.real_const;_tmpA50.f2=_tmpA53;_tmpA50;});case 8U: _LL47: _tmpA55=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA49)->f1).tq;_LL48:
# 5365
 return({struct _tuple18 _tmpA51;_tmpA51.f1=_tmpA55.real_const;({void*_tmpF1E=(Cyc_Tcutil_addressof_props(te,_tmpA61)).f2;_tmpA51.f2=_tmpF1E;});_tmpA51;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5369
({void*_tmpA56=0U;({unsigned int _tmpF20=e->loc;struct _dyneither_ptr _tmpF1F=({const char*_tmpA57="unary & applied to non-lvalue";_tag_dyneither(_tmpA57,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpF20,_tmpF1F,_tag_dyneither(_tmpA56,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5376
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
({union Cyc_Absyn_Constraint*_tmpF21=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);b=_tmpF21;});{
void*_tmpA63=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpA64=_tmpA63;struct Cyc_Absyn_Exp*_tmpA6D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA64)->tag == 0U){_LL1: _LL2:
 return;}else{_LL3: _tmpA6D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA64)->f1;_LL4: {
# 5381
struct _tuple14 _tmpA65=Cyc_Evexp_eval_const_uint_exp(_tmpA6D);struct _tuple14 _tmpA66=_tmpA65;unsigned int _tmpA6C;int _tmpA6B;_LL6: _tmpA6C=_tmpA66.f1;_tmpA6B=_tmpA66.f2;_LL7:;
if(_tmpA6B  && _tmpA6C <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpA6A;_tmpA6A.tag=1U;_tmpA6A.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpA69;_tmpA69.tag=1U;_tmpA69.f1=(unsigned long)((int)_tmpA6C);({void*_tmpA67[2U]={& _tmpA69,& _tmpA6A};({unsigned int _tmpF23=loc;struct _dyneither_ptr _tmpF22=({const char*_tmpA68="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpA68,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpF23,_tmpF22,_tag_dyneither(_tmpA67,sizeof(void*),2U));});});});});
return;}}_LL0:;};}
# 5388
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0U,b);}
# 5392
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpA6E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpA6F=_tmpA6E;struct Cyc_Absyn_Exp*_tmpA74;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA6F)->tag == 1U){_LL1: _tmpA74=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA6F)->f1;_LL2: {
# 5395
struct _tuple14 _tmpA70=Cyc_Evexp_eval_const_uint_exp(_tmpA74);struct _tuple14 _tmpA71=_tmpA70;unsigned int _tmpA73;int _tmpA72;_LL6: _tmpA73=_tmpA71.f1;_tmpA72=_tmpA71.f2;_LL7:;
return _tmpA72  && _tmpA73 == 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5401
int Cyc_Tcutil_bits_only(void*t){
void*_tmpA75=Cyc_Tcutil_compress(t);void*_tmpA76=_tmpA75;struct Cyc_List_List*_tmpA80;struct Cyc_Absyn_Aggrdecl*_tmpA7F;struct Cyc_List_List*_tmpA7E;struct Cyc_List_List*_tmpA7D;void*_tmpA7C;union Cyc_Absyn_Constraint*_tmpA7B;switch(*((int*)_tmpA76)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 return 0;case 8U: _LLB: _tmpA7C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA76)->f1).elt_type;_tmpA7B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA76)->f1).zero_term;_LLC:
# 5411
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA7B) && Cyc_Tcutil_bits_only(_tmpA7C);case 10U: _LLD: _tmpA7D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA76)->f1;_LLE:
# 5413
 for(0;_tmpA7D != 0;_tmpA7D=_tmpA7D->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpA7D->hd)).f2))return 0;}
return 1;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA76)->f1).aggr_info).UnknownAggr).tag == 1){_LLF: _LL10:
 return 0;}else{_LL11: _tmpA7F=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA76)->f1).aggr_info).KnownAggr).val;_tmpA7E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA76)->f1).targs;_LL12:
# 5418
 if(_tmpA7F->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA7F->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpA77=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA77;_push_region(rgn);
{struct Cyc_List_List*_tmpA78=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA7F->tvs,_tmpA7E);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA7F->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpA78,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA79=0;_npop_handler(0U);return _tmpA79;}}}{
int _tmpA7A=1;_npop_handler(0U);return _tmpA7A;};}
# 5425
;_pop_region(rgn);};};}case 12U: _LL13: _tmpA80=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA76)->f2;_LL14:
# 5430
 for(0;_tmpA80 != 0;_tmpA80=_tmpA80->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpA80->hd)->type))return 0;}
return 1;default: _LL15: _LL16:
 return 0;}_LL0:;}
# 5438
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpA81=e->r;void*_tmpA82=_tmpA81;struct Cyc_Absyn_Exp*_tmpA84;struct Cyc_Absyn_Exp*_tmpA83;switch(*((int*)_tmpA82)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmpA83=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA82)->f1;_LL4:
 _tmpA84=_tmpA83;goto _LL6;case 13U: _LL5: _tmpA84=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA82)->f1;_LL6:
 e=_tmpA84;continue;default: _LL7: _LL8:
# 5445
 return 0;}_LL0:;}}
# 5455
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpA85=e->r;void*_tmpA86=_tmpA85;struct Cyc_List_List*_tmpAA4;struct Cyc_List_List*_tmpAA3;enum Cyc_Absyn_Primop _tmpAA2;struct Cyc_List_List*_tmpAA1;struct Cyc_List_List*_tmpAA0;struct Cyc_List_List*_tmpA9F;struct Cyc_List_List*_tmpA9E;struct Cyc_Absyn_Exp*_tmpA9D;struct Cyc_Absyn_Exp*_tmpA9C;struct Cyc_Absyn_Exp*_tmpA9B;struct Cyc_Absyn_Exp*_tmpA9A;void*_tmpA99;struct Cyc_Absyn_Exp*_tmpA98;void*_tmpA97;struct Cyc_Absyn_Exp*_tmpA96;struct Cyc_Absyn_Exp*_tmpA95;struct Cyc_Absyn_Exp*_tmpA94;struct Cyc_Absyn_Exp*_tmpA93;struct Cyc_Absyn_Exp*_tmpA92;struct Cyc_Absyn_Exp*_tmpA91;struct Cyc_Absyn_Exp*_tmpA90;struct Cyc_Absyn_Exp*_tmpA8F;void*_tmpA8E;switch(*((int*)_tmpA86)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmpA8E=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_LL10: {
# 5467
void*_tmpA87=_tmpA8E;struct Cyc_Absyn_Vardecl*_tmpA8D;struct Cyc_Absyn_Vardecl*_tmpA8C;switch(*((int*)_tmpA87)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmpA8C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA87)->f1;_LL35: {
# 5470
void*_tmpA88=Cyc_Tcutil_compress(_tmpA8C->type);void*_tmpA89=_tmpA88;switch(*((int*)_tmpA89)){case 8U: _LL3D: _LL3E:
 goto _LL40;case 9U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmpA8D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA87)->f1;_LL37:
# 5477
 if(_tmpA8D->sc == Cyc_Absyn_Static){
void*_tmpA8A=Cyc_Tcutil_compress(_tmpA8D->type);void*_tmpA8B=_tmpA8A;switch(*((int*)_tmpA8B)){case 8U: _LL44: _LL45:
 goto _LL47;case 9U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}else{
# 5484
return var_okay;}case 0U: _LL38: _LL39:
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmpA91=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_tmpA90=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA86)->f2;_tmpA8F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA86)->f3;_LL12:
# 5490
 return(Cyc_Tcutil_cnst_exp(0,_tmpA91) && 
Cyc_Tcutil_cnst_exp(0,_tmpA90)) && 
Cyc_Tcutil_cnst_exp(0,_tmpA8F);case 9U: _LL13: _tmpA93=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_tmpA92=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA86)->f2;_LL14:
# 5494
 return Cyc_Tcutil_cnst_exp(0,_tmpA93) && Cyc_Tcutil_cnst_exp(0,_tmpA92);case 12U: _LL15: _tmpA94=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_LL16:
 _tmpA95=_tmpA94;goto _LL18;case 13U: _LL17: _tmpA95=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_LL18:
# 5497
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA95);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA86)->f4 == Cyc_Absyn_No_coercion){_LL19: _tmpA97=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_tmpA96=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA86)->f2;_LL1A:
# 5499
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA96);}else{_LL1B: _tmpA99=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_tmpA98=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA86)->f2;_LL1C:
# 5502
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA98);}case 15U: _LL1D: _tmpA9A=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_LL1E:
# 5504
 return Cyc_Tcutil_cnst_exp(1,_tmpA9A);case 27U: _LL1F: _tmpA9C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA86)->f2;_tmpA9B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA86)->f3;_LL20:
# 5506
 return Cyc_Tcutil_cnst_exp(0,_tmpA9C) && Cyc_Tcutil_cnst_exp(0,_tmpA9B);case 28U: _LL21: _tmpA9D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_LL22:
# 5508
 return Cyc_Tcutil_cnst_exp(0,_tmpA9D);case 26U: _LL23: _tmpA9E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_LL24:
 _tmpA9F=_tmpA9E;goto _LL26;case 30U: _LL25: _tmpA9F=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA86)->f2;_LL26:
 _tmpAA0=_tmpA9F;goto _LL28;case 29U: _LL27: _tmpAA0=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA86)->f3;_LL28:
# 5512
 for(0;_tmpAA0 != 0;_tmpAA0=_tmpAA0->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpAA0->hd)).f2))
return 0;}
return 1;case 3U: _LL29: _tmpAA2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_tmpAA1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA86)->f2;_LL2A:
# 5517
 _tmpAA3=_tmpAA1;goto _LL2C;case 24U: _LL2B: _tmpAA3=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_LL2C:
 _tmpAA4=_tmpAA3;goto _LL2E;case 31U: _LL2D: _tmpAA4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA86)->f1;_LL2E:
# 5520
 for(0;_tmpAA4 != 0;_tmpAA4=_tmpAA4->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpAA4->hd))
return 0;}
return 1;default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 5528
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5532
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5534
int Cyc_Tcutil_supports_default(void*t){
void*_tmpAA5=Cyc_Tcutil_compress(t);void*_tmpAA6=_tmpAA5;struct Cyc_List_List*_tmpAB0;union Cyc_Absyn_AggrInfoU _tmpAAF;struct Cyc_List_List*_tmpAAE;struct Cyc_List_List*_tmpAAD;void*_tmpAAC;union Cyc_Absyn_Constraint*_tmpAAB;union Cyc_Absyn_Constraint*_tmpAAA;switch(*((int*)_tmpAA6)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 5U: _LL7: _tmpAAB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA6)->f1).ptr_atts).nullable;_tmpAAA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA6)->f1).ptr_atts).bounds;_LL8: {
# 5541
void*_tmpAA7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAAA);void*_tmpAA8=_tmpAA7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAA8)->tag == 0U){_LL18: _LL19:
 return 1;}else{_LL1A: _LL1B:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpAAB);}_LL17:;}case 8U: _LL9: _tmpAAC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAA6)->f1).elt_type;_LLA:
# 5546
 return Cyc_Tcutil_supports_default(_tmpAAC);case 10U: _LLB: _tmpAAD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAA6)->f1;_LLC:
# 5548
 for(0;_tmpAAD != 0;_tmpAAD=_tmpAAD->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpAAD->hd)).f2))return 0;}
return 1;case 11U: _LLD: _tmpAAF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA6)->f1).aggr_info;_tmpAAE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA6)->f1).targs;_LLE: {
# 5552
struct Cyc_Absyn_Aggrdecl*_tmpAA9=Cyc_Absyn_get_known_aggrdecl(_tmpAAF);
if(_tmpAA9->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAA9->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpAA9->tvs,_tmpAAE,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAA9->impl))->fields);}case 12U: _LLF: _tmpAB0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAA6)->f2;_LL10:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpAB0);case 14U: _LL11: _LL12:
# 5558
 goto _LL14;case 13U: _LL13: _LL14:
 return 1;default: _LL15: _LL16:
# 5561
 return 0;}_LL0:;}
# 5566
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpAB1=t;struct Cyc_Absyn_Typedefdecl*_tmpAB6;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAB1)->tag == 17U){_LL1: _tmpAB6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAB1)->f3;_LL2:
# 5569
 if(_tmpAB6 != 0){
struct Cyc_Absyn_Tqual _tmpAB2=_tmpAB6->tq;
if(((_tmpAB2.print_const  || _tmpAB2.q_volatile) || _tmpAB2.q_restrict) || _tmpAB2.real_const)
# 5574
({struct Cyc_String_pa_PrintArg_struct _tmpAB5;_tmpAB5.tag=0U;({struct _dyneither_ptr _tmpF24=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAB5.f1=_tmpF24;});({void*_tmpAB3[1U]={& _tmpAB5};({unsigned int _tmpF26=loc;struct _dyneither_ptr _tmpF25=({const char*_tmpAB4="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpAB4,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpF26,_tmpF25,_tag_dyneither(_tmpAB3,sizeof(void*),1U));});});});}
# 5577
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5582
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5584
struct _RegionHandle _tmpAB7=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAB7;_push_region(rgn);
{struct Cyc_List_List*_tmpAB8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpAB9=1;_npop_handler(0U);return _tmpAB9;}
({void*_tmpF27=Cyc_Tcutil_rsubstitute(rgn,_tmpAB8,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);t=_tmpF27;});
if(!Cyc_Tcutil_supports_default(t)){int _tmpABA=0;_npop_handler(0U);return _tmpABA;}}{
# 5592
int _tmpABB=1;_npop_handler(0U);return _tmpABB;};}
# 5585
;_pop_region(rgn);}
# 5598
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpABC=Cyc_Tcutil_compress(t);void*_tmpABD=_tmpABC;union Cyc_Absyn_Constraint*_tmpAC1;union Cyc_Absyn_Constraint*_tmpAC0;switch(*((int*)_tmpABD)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;case 5U: _LL5: _tmpAC1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->f1).ptr_atts).nullable;_tmpAC0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->f1).ptr_atts).bounds;_LL6: {
# 5603
void*_tmpABE=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAC0);void*_tmpABF=_tmpABE;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpABF)->tag == 0U){_LLA: _LLB:
# 5607
 return 0;}else{_LLC: _LLD:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpAC1);}_LL9:;}default: _LL7: _LL8:
# 5610
 return 0;}_LL0:;}
# 5614
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpAC2=Cyc_Tcutil_compress(t);void*_tmpAC3=_tmpAC2;struct Cyc_List_List*_tmpAC7;void*_tmpAC6;switch(*((int*)_tmpAC3)){case 5U: _LL1: _tmpAC6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAC3)->f1).elt_typ;_LL2:
 return Cyc_Tcutil_is_noreturn(_tmpAC6);case 9U: _LL3: _tmpAC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAC3)->f1).attributes;_LL4:
# 5618
 for(0;_tmpAC7 != 0;_tmpAC7=_tmpAC7->tl){
void*_tmpAC4=(void*)_tmpAC7->hd;void*_tmpAC5=_tmpAC4;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpAC5)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 5623
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5626
return 0;}
# 5631
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpAC8=Cyc_Tcutil_compress(t);void*_tmpAC9=_tmpAC8;struct Cyc_List_List**_tmpACF;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAC9)->tag == 9U){_LL1: _tmpACF=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAC9)->f1).attributes;_LL2: {
# 5634
struct Cyc_List_List*_tmpACA=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpACF,(void*)atts->hd))
({struct Cyc_List_List*_tmpF28=({struct Cyc_List_List*_tmpACB=_cycalloc(sizeof(*_tmpACB));_tmpACB->hd=(void*)atts->hd;_tmpACB->tl=*_tmpACF;_tmpACB;});*_tmpACF=_tmpF28;});}else{
# 5641
({struct Cyc_List_List*_tmpF29=({struct Cyc_List_List*_tmpACC=_cycalloc(sizeof(*_tmpACC));_tmpACC->hd=(void*)atts->hd;_tmpACC->tl=_tmpACA;_tmpACC;});_tmpACA=_tmpF29;});}}
return _tmpACA;}}else{_LL3: _LL4:
({void*_tmpACD=0U;({struct _dyneither_ptr _tmpF2A=({const char*_tmpACE="transfer_fn_type_atts";_tag_dyneither(_tmpACE,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF2A,_tag_dyneither(_tmpACD,sizeof(void*),0U));});});}_LL0:;}
# 5648
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpAD0=Cyc_Tcutil_compress(t);void*_tmpAD1=_tmpAD0;struct Cyc_Absyn_Exp*_tmpAD6;struct Cyc_Absyn_PtrInfo*_tmpAD5;switch(*((int*)_tmpAD1)){case 5U: _LL1: _tmpAD5=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAD1)->f1;_LL2:
# 5652
{void*_tmpAD2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpAD5->ptr_atts).bounds);void*_tmpAD3=_tmpAD2;struct Cyc_Absyn_Exp*_tmpAD4;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD3)->tag == 1U){_LL8: _tmpAD4=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAD3)->f1;_LL9:
 bound=_tmpAD4;goto _LL7;}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 5656
goto _LL0;case 8U: _LL3: _tmpAD6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAD1)->f1).num_elts;_LL4:
# 5658
 bound=_tmpAD6;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5662
return bound;}
# 5667
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpAD7=b;struct Cyc_Absyn_Vardecl*_tmpADB;struct Cyc_Absyn_Vardecl*_tmpADA;struct Cyc_Absyn_Vardecl*_tmpAD9;struct Cyc_Absyn_Vardecl*_tmpAD8;switch(*((int*)_tmpAD7)){case 5U: _LL1: _tmpAD8=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpAD7)->f1;_LL2:
 _tmpAD9=_tmpAD8;goto _LL4;case 4U: _LL3: _tmpAD9=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpAD7)->f1;_LL4:
 _tmpADA=_tmpAD9;goto _LL6;case 3U: _LL5: _tmpADA=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpAD7)->f1;_LL6:
 _tmpADB=_tmpADA;goto _LL8;case 1U: _LL7: _tmpADB=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpAD7)->f1;_LL8:
# 5673
 if(!_tmpADB->escapes)return _tmpADB;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5677
return 0;}
# 5681
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpADC=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)({struct Cyc_List_List*_tmpF2B=({struct Cyc_List_List*_tmpADD=_cycalloc(sizeof(*_tmpADD));_tmpADD->hd=*((void**)_check_null((void**)x->hd));_tmpADD->tl=_tmpADC;_tmpADD;});_tmpADC=_tmpF2B;});}}
return _tmpADC;}
# 5688
int Cyc_Tcutil_is_array(void*t){
void*_tmpADE=Cyc_Tcutil_compress(t);void*_tmpADF=_tmpADE;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADF)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5695
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmpAE0=Cyc_Tcutil_compress(t);void*_tmpAE1=_tmpAE0;void*_tmpAF1;struct Cyc_Absyn_Tqual _tmpAF0;struct Cyc_Absyn_Exp*_tmpAEF;union Cyc_Absyn_Constraint*_tmpAEE;unsigned int _tmpAED;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE1)->tag == 8U){_LL1: _tmpAF1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE1)->f1).elt_type;_tmpAF0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE1)->f1).tq;_tmpAEF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE1)->f1).num_elts;_tmpAEE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE1)->f1).zero_term;_tmpAED=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAE1)->f1).zt_loc;_LL2: {
# 5698
void*b;
if(_tmpAEF == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5702
struct Cyc_Absyn_Exp*bnd=_tmpAEF;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpAE2=0U;({struct _dyneither_ptr _tmpF2C=({const char*_tmpAE3="cannot convert tag without type!";_tag_dyneither(_tmpAE3,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF2C,_tag_dyneither(_tmpAE2,sizeof(void*),0U));});});{
void*_tmpAE4=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpAE5=_tmpAE4;void*_tmpAEA;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpAE5)->tag == 19U){_LL6: _tmpAEA=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpAE5)->f1;_LL7:
# 5708
({void*_tmpF30=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAE6=_cycalloc(sizeof(*_tmpAE6));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF2F=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAE7;_tmpAE7.tag=1U;({struct Cyc_Absyn_Exp*_tmpF2E=({void*_tmpF2D=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmpF2D,Cyc_Absyn_valueof_exp(_tmpAEA,0U),0,Cyc_Absyn_No_coercion,0U);});_tmpAE7.f1=_tmpF2E;});_tmpAE7;});_tmpAE6[0]=_tmpF2F;});_tmpAE6;});b=_tmpF30;});
goto _LL5;}else{_LL8: _LL9:
# 5711
 if(Cyc_Tcutil_is_const_exp(bnd))
({void*_tmpF32=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAE8=_cycalloc(sizeof(*_tmpAE8));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF31=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAE9;_tmpAE9.tag=1U;_tmpAE9.f1=bnd;_tmpAE9;});_tmpAE8[0]=_tmpF31;});_tmpAE8;});b=_tmpF32;});else{
# 5714
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL5:;};}else{
# 5718
({void*_tmpF34=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAEB=_cycalloc(sizeof(*_tmpAEB));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF33=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAEC;_tmpAEC.tag=1U;_tmpAEC.f1=bnd;_tmpAEC;});_tmpAEB[0]=_tmpF33;});_tmpAEB;});b=_tmpF34;});}}
# 5720
return Cyc_Absyn_atb_typ(_tmpAF1,rgn,_tmpAF0,b,_tmpAEE);}}else{_LL3: _LL4:
# 5723
 return t;}_LL0:;}
# 5728
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpAF2=Cyc_Tcutil_compress(t);void*_tmpAF3=_tmpAF2;struct Cyc_List_List*_tmpB01;union Cyc_Absyn_AggrInfoU _tmpB00;struct Cyc_List_List*_tmpAFF;struct Cyc_List_List*_tmpAFE;void*_tmpAFD;union Cyc_Absyn_Constraint*_tmpAFC;switch(*((int*)_tmpAF3)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 return 0;case 5U: _LLB: _tmpAFC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAF3)->f1).ptr_atts).nullable;_LLC:
# 5736
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpAFC);case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmpAFD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAF3)->f1).elt_type;_LL12:
 return Cyc_Tcutil_zeroable_type(_tmpAFD);case 9U: _LL13: _LL14:
 return 0;case 10U: _LL15: _tmpAFE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAF3)->f1;_LL16:
# 5742
 for(0;(unsigned int)_tmpAFE;_tmpAFE=_tmpAFE->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpAFE->hd)).f2))return 0;}
return 1;case 11U: _LL17: _tmpB00=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAF3)->f1).aggr_info;_tmpAFF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAF3)->f1).targs;_LL18: {
# 5746
struct Cyc_Absyn_Aggrdecl*_tmpAF4=Cyc_Absyn_get_known_aggrdecl(_tmpB00);
if(_tmpAF4->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF4->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpAF5=_new_region("r");struct _RegionHandle*r=& _tmpAF5;_push_region(r);
{struct Cyc_List_List*_tmpAF6=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpAF4->tvs,_tmpAFF);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAF4->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpAF6,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAF7=0;_npop_handler(0U);return _tmpAF7;}}}{
int _tmpAF8=1;_npop_handler(0U);return _tmpAF8;};}
# 5750
;_pop_region(r);};}case 13U: _LL19: _LL1A:
# 5754
 goto _LL1C;case 19U: _LL1B: _LL1C:
 return 1;case 12U: _LL1D: _tmpB01=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAF3)->f2;_LL1E:
# 5757
 for(0;_tmpB01 != 0;_tmpB01=_tmpB01->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB01->hd)->type))return 0;}
return 1;case 14U: _LL1F: _LL20:
 return 1;case 17U: _LL21: _LL22:
 goto _LL24;case 16U: _LL23: _LL24:
 goto _LL26;case 15U: _LL25: _LL26:
 goto _LL28;case 28U: _LL27: _LL28:
 return 0;case 26U: _LL29: _LL2A:
# 5766
 goto _LL2C;case 27U: _LL2B: _LL2C:
 goto _LL2E;case 18U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 21U: _LL31: _LL32:
 goto _LL34;case 22U: _LL33: _LL34:
 goto _LL36;case 23U: _LL35: _LL36:
 goto _LL38;case 24U: _LL37: _LL38:
 goto _LL3A;default: _LL39: _LL3A:
({struct Cyc_String_pa_PrintArg_struct _tmpAFB;_tmpAFB.tag=0U;({struct _dyneither_ptr _tmpF35=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAFB.f1=_tmpF35;});({void*_tmpAF9[1U]={& _tmpAFB};({struct _dyneither_ptr _tmpF36=({const char*_tmpAFA="bad type `%s' in zeroable type";_tag_dyneither(_tmpAFA,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF36,_tag_dyneither(_tmpAF9,sizeof(void*),1U));});});});}_LL0:;}
