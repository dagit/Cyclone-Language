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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 915 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple2*,struct _tuple2*);
# 917
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 925
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 928
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
union Cyc_Absyn_Constraint*Cyc_Absyn_empty_conref();
union Cyc_Absyn_Constraint*Cyc_Absyn_compress_conref(union Cyc_Absyn_Constraint*x);
# 932
void*Cyc_Absyn_conref_val(union Cyc_Absyn_Constraint*x);
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
# 953
extern void*Cyc_Absyn_uint_typ;extern void*Cyc_Absyn_ulong_typ;extern void*Cyc_Absyn_ulonglong_typ;
# 955
extern void*Cyc_Absyn_sint_typ;extern void*Cyc_Absyn_slong_typ;extern void*Cyc_Absyn_slonglong_typ;
# 959
extern void*Cyc_Absyn_empty_effect;
# 969
extern struct _tuple2*Cyc_Absyn_datatype_print_arg_qvar;
extern struct _tuple2*Cyc_Absyn_datatype_scanf_arg_qvar;
# 978
extern void*Cyc_Absyn_bounds_one;
# 983
void*Cyc_Absyn_atb_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,union Cyc_Absyn_Constraint*zero_term);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*es,unsigned int);
# 1031
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1041
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned int);
# 1048
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1054
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
# 1066
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned int);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _dyneither_ptr body,unsigned int);
# 1112
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
# 1159
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1161
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _dyneither_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1163
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1165
struct _dyneither_ptr Cyc_Absyn_attribute2string(void*);
# 1167
int Cyc_Absyn_fntype_att(void*a);struct _tuple13{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;};
# 1171
struct _tuple13 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1173
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
# 70
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
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0,Cyc_Tcenv_InNew  = 1,Cyc_Tcenv_InNewAggr  = 2};
# 96
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 143
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
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 275
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple18{int f1;void*f2;};
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
if(({({struct _dyneither_ptr _tmpAFB=({const char*_tmp1="(qualifiers don't match)";_tag_dyneither(_tmp1,sizeof(char),25);});Cyc_strcmp(_tmpAFB,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0;})){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp2[1]={& _tmp4};({struct Cyc___cycFILE*_tmpAFD=Cyc_stderr;struct _dyneither_ptr _tmpAFC=({const char*_tmp3="  %s\n";_tag_dyneither(_tmp3,sizeof(char),6);});Cyc_fprintf(_tmpAFD,_tmpAFC,_tag_dyneither(_tmp2,sizeof(void*),1));});});});
return;}
# 68
if(({({struct _dyneither_ptr _tmpAFE=({const char*_tmp5="(function effects do not match)";_tag_dyneither(_tmp5,sizeof(char),32);});Cyc_strcmp(_tmpAFE,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0;})){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp9;_tmp9.tag=0;_tmp9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp7[1]={& _tmp9};({struct Cyc___cycFILE*_tmpB00=Cyc_stderr;struct _dyneither_ptr _tmpAFF=({const char*_tmp8="  %s";_tag_dyneither(_tmp8,sizeof(char),5);});Cyc_fprintf(_tmpB00,_tmpAFF,_tag_dyneither(_tmp7,sizeof(void*),1));});});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpA=0;({struct Cyc___cycFILE*_tmpB02=Cyc_stderr;struct _dyneither_ptr _tmpB01=({const char*_tmpB="\n\t";_tag_dyneither(_tmpB,sizeof(char),3);});Cyc_fprintf(_tmpB02,_tmpB01,_tag_dyneither(_tmpA,sizeof(void*),0));});});
pos=8;}else{
# 82
({void*_tmpC=0;({struct Cyc___cycFILE*_tmpB04=Cyc_stderr;struct _dyneither_ptr _tmpB03=({const char*_tmpD=" ";_tag_dyneither(_tmpD,sizeof(char),2);});Cyc_fprintf(_tmpB04,_tmpB03,_tag_dyneither(_tmpC,sizeof(void*),0));});});
++ pos;}
# 85
({void*_tmpE=0;({struct Cyc___cycFILE*_tmpB06=Cyc_stderr;struct _dyneither_ptr _tmpB05=({const char*_tmpF="and ";_tag_dyneither(_tmpF,sizeof(char),5);});Cyc_fprintf(_tmpB06,_tmpB05,_tag_dyneither(_tmpE,sizeof(void*),0));});});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmp10=0;({struct Cyc___cycFILE*_tmpB08=Cyc_stderr;struct _dyneither_ptr _tmpB07=({const char*_tmp11="\n\t";_tag_dyneither(_tmp11,sizeof(char),3);});Cyc_fprintf(_tmpB08,_tmpB07,_tag_dyneither(_tmp10,sizeof(void*),0));});});
pos=8;}
# 91
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({void*_tmp12[1]={& _tmp14};({struct Cyc___cycFILE*_tmpB0A=Cyc_stderr;struct _dyneither_ptr _tmpB09=({const char*_tmp13="%s ";_tag_dyneither(_tmp13,sizeof(char),4);});Cyc_fprintf(_tmpB0A,_tmpB09,_tag_dyneither(_tmp12,sizeof(void*),1));});});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp15=0;({struct Cyc___cycFILE*_tmpB0C=Cyc_stderr;struct _dyneither_ptr _tmpB0B=({const char*_tmp16="\n\t";_tag_dyneither(_tmp16,sizeof(char),3);});Cyc_fprintf(_tmpB0C,_tmpB0B,_tag_dyneither(_tmp15,sizeof(void*),0));});});
pos=8;}
# 97
({void*_tmp17=0;({struct Cyc___cycFILE*_tmpB0E=Cyc_stderr;struct _dyneither_ptr _tmpB0D=({const char*_tmp18="are not compatible. ";_tag_dyneither(_tmp18,sizeof(char),21);});Cyc_fprintf(_tmpB0E,_tmpB0D,_tag_dyneither(_tmp17,sizeof(void*),0));});});
pos +=17;
if(({char*_tmpB0F=(char*)Cyc_Tcutil_failure_reason.curr;_tmpB0F != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({({int _tmpB10=pos;_tmpB10 + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= 80;}))
({void*_tmp19=0;({struct Cyc___cycFILE*_tmpB12=Cyc_stderr;struct _dyneither_ptr _tmpB11=({const char*_tmp1A="\n\t";_tag_dyneither(_tmp1A,sizeof(char),3);});Cyc_fprintf(_tmpB12,_tmpB11,_tag_dyneither(_tmp19,sizeof(void*),0));});});
# 103
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp1B[1]={& _tmp1D};({struct Cyc___cycFILE*_tmpB14=Cyc_stderr;struct _dyneither_ptr _tmpB13=({const char*_tmp1C="%s";_tag_dyneither(_tmp1C,sizeof(char),3);});Cyc_fprintf(_tmpB14,_tmpB13,_tag_dyneither(_tmp1B,sizeof(void*),1));});});});}
# 105
({void*_tmp1E=0;({struct Cyc___cycFILE*_tmpB16=Cyc_stderr;struct _dyneither_ptr _tmpB15=({const char*_tmp1F="\n";_tag_dyneither(_tmp1F,sizeof(char),2);});Cyc_fprintf(_tmpB16,_tmpB15,_tag_dyneither(_tmp1E,sizeof(void*),0));});});
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 111
Cyc_Position_post_error(({unsigned int _tmpB17=loc;Cyc_Position_mk_err(_tmpB17,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 113
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 116
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp20[1]={& _tmp22};({struct Cyc___cycFILE*_tmpB19=Cyc_stderr;struct _dyneither_ptr _tmpB18=({const char*_tmp21="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp21,sizeof(char),36);});Cyc_fprintf(_tmpB19,_tmpB18,_tag_dyneither(_tmp20,sizeof(void*),1));});});});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));({struct Cyc_Core_Impossible_exn_struct _tmpB1A=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;_tmp24.f1=msg;_tmp24;});_tmp23[0]=_tmpB1A;});_tmp23;}));}
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
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp34=_cycalloc(sizeof(*_tmp34));({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpB1B=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp35;_tmp35.tag=1;_tmp35.f1=0;_tmp35;});_tmp34[0]=_tmpB1B;});_tmp34;});case 2: _LL20: _tmp38=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp33)->f2;_LL21:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp36=_cycalloc(sizeof(*_tmp36));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpB1C=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp37;_tmp37.tag=2;_tmp37.f1=0;_tmp37.f2=_tmp38;_tmp37;});_tmp36[0]=_tmpB1C;});_tmp36;});default: _LL22: _LL23:
 return kb;}_LL1D:;}
# 185
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 187
return({struct Cyc_Absyn_Tvar*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=tv->name;_tmp39->identity=- 1;({void*_tmpB1D=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp39->kind=_tmpB1D;});_tmp39;});}
# 189
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 191
struct _tuple10*_tmp3A=arg;struct _dyneither_ptr*_tmp3E;struct Cyc_Absyn_Tqual _tmp3D;void*_tmp3C;_LL25: _tmp3E=_tmp3A->f1;_tmp3D=_tmp3A->f2;_tmp3C=_tmp3A->f3;_LL26:;
return({struct _tuple10*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=_tmp3E;_tmp3B->f2=_tmp3D;({void*_tmpB1E=Cyc_Tcutil_copy_type(_tmp3C);_tmp3B->f3=_tmpB1E;});_tmp3B;});}
# 194
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp3F=arg;struct Cyc_Absyn_Tqual _tmp42;void*_tmp41;_LL28: _tmp42=_tmp3F->f1;_tmp41=_tmp3F->f2;_LL29:;
return({struct _tuple12*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->f1=_tmp42;({void*_tmpB1F=Cyc_Tcutil_copy_type(_tmp41);_tmp40->f2=_tmpB1F;});_tmp40;});}
# 198
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 200
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->name=f->name;_tmp43->tq=f->tq;({void*_tmpB20=Cyc_Tcutil_copy_type(f->type);_tmp43->type=_tmpB20;});_tmp43->width=f->width;_tmp43->attributes=f->attributes;({struct Cyc_Absyn_Exp*_tmpB21=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp43->requires_clause=_tmpB21;});_tmp43;});}
# 204
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp44=x;void*_tmp47;void*_tmp46;_LL2B: _tmp47=_tmp44->f1;_tmp46=_tmp44->f2;_LL2C:;
return({struct _tuple0*_tmp45=_cycalloc(sizeof(*_tmp45));({void*_tmpB22=Cyc_Tcutil_copy_type(_tmp47);_tmp45->f1=_tmpB22;});({void*_tmpB23=Cyc_Tcutil_copy_type(_tmp46);_tmp45->f2=_tmpB23;});_tmp45;});}
# 208
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->name=f->name;_tmp48->tag=f->tag;_tmp48->loc=f->loc;_tmp48;});}
# 211
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpB25=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A;_tmp4A.tag=2;({struct Cyc_Absyn_Tvar*_tmpB24=Cyc_Tcutil_copy_tvar(t);_tmp4A.f1=_tmpB24;});_tmp4A;});_tmp49[0]=_tmpB25;});_tmp49;});}
# 215
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_Absyn_Datatypedecl*_tmpD2;struct Cyc_Absyn_Enumdecl*_tmpD1;struct Cyc_Absyn_Aggrdecl*_tmpD0;void*_tmpCF;struct Cyc_List_List*_tmpCE;void*_tmpCD;struct _tuple2*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_Absyn_Typedefdecl*_tmpCA;void*_tmpC9;void*_tmpC8;void*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;void*_tmpC4;struct Cyc_List_List*_tmpC3;struct _tuple2*_tmpC2;struct Cyc_Absyn_Enumdecl*_tmpC1;enum Cyc_Absyn_AggrKind _tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Aggrdecl**_tmpBE;struct Cyc_List_List*_tmpBD;enum Cyc_Absyn_AggrKind _tmpBC;struct _tuple2*_tmpBB;struct Cyc_Core_Opt*_tmpBA;struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;void*_tmpB6;struct Cyc_Absyn_Tqual _tmpB5;void*_tmpB4;struct Cyc_List_List*_tmpB3;int _tmpB2;struct Cyc_Absyn_VarargInfo*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_List_List*_tmpAB;void*_tmpAA;struct Cyc_Absyn_Tqual _tmpA9;struct Cyc_Absyn_Exp*_tmpA8;union Cyc_Absyn_Constraint*_tmpA7;unsigned int _tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;union Cyc_Absyn_Constraint*_tmpA2;union Cyc_Absyn_Constraint*_tmpA1;union Cyc_Absyn_Constraint*_tmpA0;struct Cyc_Absyn_PtrLoc*_tmp9F;union Cyc_Absyn_DatatypeFieldInfoU _tmp9E;struct Cyc_List_List*_tmp9D;union Cyc_Absyn_DatatypeInfoU _tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Tvar*_tmp9A;switch(*((int*)_tmp4C)){case 0: _LL2E: _LL2F:
 goto _LL31;case 1: _LL30: _LL31:
 return t;case 2: _LL32: _tmp9A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4C)->f1;_LL33:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpB27=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4E;_tmp4E.tag=2;({struct Cyc_Absyn_Tvar*_tmpB26=Cyc_Tcutil_copy_tvar(_tmp9A);_tmp4E.f1=_tmpB26;});_tmp4E;});_tmp4D[0]=_tmpB27;});_tmp4D;});case 3: _LL34: _tmp9C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).datatype_info;_tmp9B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL35:
# 221
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB2A=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp50;_tmp50.tag=3;({struct Cyc_Absyn_DatatypeInfo _tmpB29=({struct Cyc_Absyn_DatatypeInfo _tmp51;_tmp51.datatype_info=_tmp9C;({struct Cyc_List_List*_tmpB28=Cyc_Tcutil_copy_types(_tmp9B);_tmp51.targs=_tmpB28;});_tmp51;});_tmp50.f1=_tmpB29;});_tmp50;});_tmp4F[0]=_tmpB2A;});_tmp4F;});case 4: _LL36: _tmp9E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).field_info;_tmp9D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL37:
# 223
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB2D=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp53;_tmp53.tag=4;({struct Cyc_Absyn_DatatypeFieldInfo _tmpB2C=({struct Cyc_Absyn_DatatypeFieldInfo _tmp54;_tmp54.field_info=_tmp9E;({struct Cyc_List_List*_tmpB2B=Cyc_Tcutil_copy_types(_tmp9D);_tmp54.targs=_tmpB2B;});_tmp54;});_tmp53.f1=_tmpB2C;});_tmp53;});_tmp52[0]=_tmpB2D;});_tmp52;});case 5: _LL38: _tmpA5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_typ;_tmpA4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_tq;_tmpA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).rgn;_tmpA2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_tmpA1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).bounds;_tmpA0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).zero_term;_tmp9F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).ptrloc;_LL39: {
# 225
void*_tmp55=Cyc_Tcutil_copy_type(_tmpA5);
void*_tmp56=Cyc_Tcutil_copy_type(_tmpA3);
union Cyc_Absyn_Constraint*_tmp57=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA2);
struct Cyc_Absyn_Tqual _tmp58=_tmpA4;
# 230
union Cyc_Absyn_Constraint*_tmp59=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA1);
union Cyc_Absyn_Constraint*_tmp5A=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA0);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpB30=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp5C;_tmp5C.tag=5;({struct Cyc_Absyn_PtrInfo _tmpB2F=({struct Cyc_Absyn_PtrInfo _tmp5D;_tmp5D.elt_typ=_tmp55;_tmp5D.elt_tq=_tmp58;({struct Cyc_Absyn_PtrAtts _tmpB2E=({(_tmp5D.ptr_atts).rgn=_tmp56;(_tmp5D.ptr_atts).nullable=_tmp57;(_tmp5D.ptr_atts).bounds=_tmp59;(_tmp5D.ptr_atts).zero_term=_tmp5A;(_tmp5D.ptr_atts).ptrloc=_tmp9F;_tmp5D.ptr_atts;});_tmp5D.ptr_atts=_tmpB2E;});_tmp5D;});_tmp5C.f1=_tmpB2F;});_tmp5C;});_tmp5B[0]=_tmpB30;});_tmp5B;});}case 6: _LL3A: _LL3B:
 goto _LL3D;case 7: _LL3C: _LL3D:
 return t;case 8: _LL3E: _tmpAA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_tmpA9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).tq;_tmpA8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).num_elts;_tmpA7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zero_term;_tmpA6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zt_loc;_LL3F: {
# 236
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpA8);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB34=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp5F;_tmp5F.tag=8;({struct Cyc_Absyn_ArrayInfo _tmpB33=({struct Cyc_Absyn_ArrayInfo _tmp60;({void*_tmpB31=Cyc_Tcutil_copy_type(_tmpAA);_tmp60.elt_type=_tmpB31;});_tmp60.tq=_tmpA9;_tmp60.num_elts=eopt2;({union Cyc_Absyn_Constraint*_tmpB32=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA7);_tmp60.zero_term=_tmpB32;});_tmp60.zt_loc=_tmpA6;_tmp60;});_tmp5F.f1=_tmpB33;});_tmp5F;});_tmp5E[0]=_tmpB34;});_tmp5E;});}case 9: _LL40: _tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).tvars;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).effect;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_tqual;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_typ;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).args;_tmpB2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).c_varargs;_tmpB1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).cyc_varargs;_tmpB0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).rgn_po;_tmpAF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_tmpAE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_clause;_tmpAD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_relns;_tmpAC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_clause;_tmpAB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_relns;_LL41: {
# 240
struct Cyc_List_List*_tmp61=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpB7);
void*effopt2=_tmpB6 == 0?0: Cyc_Tcutil_copy_type(_tmpB6);
void*_tmp62=Cyc_Tcutil_copy_type(_tmpB4);
struct Cyc_List_List*_tmp63=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpB3);
int _tmp64=_tmpB2;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpB1 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpB1;
({struct Cyc_Absyn_VarargInfo*_tmpB36=({struct Cyc_Absyn_VarargInfo*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->name=cv->name;_tmp65->tq=cv->tq;({void*_tmpB35=Cyc_Tcutil_copy_type(cv->type);_tmp65->type=_tmpB35;});_tmp65->inject=cv->inject;_tmp65;});cyc_varargs2=_tmpB36;});}{
# 251
struct Cyc_List_List*_tmp66=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpB0);
struct Cyc_List_List*_tmp67=_tmpAF;
struct Cyc_Absyn_Exp*_tmp68=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAE);
struct Cyc_List_List*_tmp69=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAD);
struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAC);
struct Cyc_List_List*_tmp6B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAB);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB38=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6D;_tmp6D.tag=9;({struct Cyc_Absyn_FnInfo _tmpB37=({struct Cyc_Absyn_FnInfo _tmp6E;_tmp6E.tvars=_tmp61;_tmp6E.effect=effopt2;_tmp6E.ret_tqual=_tmpB5;_tmp6E.ret_typ=_tmp62;_tmp6E.args=_tmp63;_tmp6E.c_varargs=_tmp64;_tmp6E.cyc_varargs=cyc_varargs2;_tmp6E.rgn_po=_tmp66;_tmp6E.attributes=_tmp67;_tmp6E.requires_clause=_tmp68;_tmp6E.requires_relns=_tmp69;_tmp6E.ensures_clause=_tmp6A;_tmp6E.ensures_relns=_tmp6B;_tmp6E;});_tmp6D.f1=_tmpB37;});_tmp6D;});_tmp6C[0]=_tmpB38;});_tmp6C;});};}case 10: _LL42: _tmpB8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4C)->f1;_LL43:
# 260
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB3A=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp70;_tmp70.tag=10;({struct Cyc_List_List*_tmpB39=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpB8);_tmp70.f1=_tmpB39;});_tmp70;});_tmp6F[0]=_tmpB3A;});_tmp6F;});case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).tag == 1){_LL44: _tmpBC=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f1;_tmpBB=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f2;_tmpBA=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f3;_tmpB9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL45:
# 262
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB3E=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp72;_tmp72.tag=11;({struct Cyc_Absyn_AggrInfo _tmpB3D=({struct Cyc_Absyn_AggrInfo _tmp73;({union Cyc_Absyn_AggrInfoU _tmpB3B=Cyc_Absyn_UnknownAggr(_tmpBC,_tmpBB,_tmpBA);_tmp73.aggr_info=_tmpB3B;});({struct Cyc_List_List*_tmpB3C=Cyc_Tcutil_copy_types(_tmpB9);_tmp73.targs=_tmpB3C;});_tmp73;});_tmp72.f1=_tmpB3D;});_tmp72;});_tmp71[0]=_tmpB3E;});_tmp71;});}else{_LL46: _tmpBE=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).KnownAggr).val;_tmpBD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL47:
# 264
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB42=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp75;_tmp75.tag=11;({struct Cyc_Absyn_AggrInfo _tmpB41=({struct Cyc_Absyn_AggrInfo _tmp76;({union Cyc_Absyn_AggrInfoU _tmpB3F=Cyc_Absyn_KnownAggr(_tmpBE);_tmp76.aggr_info=_tmpB3F;});({struct Cyc_List_List*_tmpB40=Cyc_Tcutil_copy_types(_tmpBD);_tmp76.targs=_tmpB40;});_tmp76;});_tmp75.f1=_tmpB41;});_tmp75;});_tmp74[0]=_tmpB42;});_tmp74;});}case 12: _LL48: _tmpC0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f1;_tmpBF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f2;_LL49:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB44=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp78;_tmp78.tag=12;_tmp78.f1=_tmpC0;({struct Cyc_List_List*_tmpB43=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpBF);_tmp78.f2=_tmpB43;});_tmp78;});_tmp77[0]=_tmpB44;});_tmp77;});case 13: _LL4A: _tmpC2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f2;_LL4B:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp79=_cycalloc(sizeof(*_tmp79));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB45=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp7A;_tmp7A.tag=13;_tmp7A.f1=_tmpC2;_tmp7A.f2=_tmpC1;_tmp7A;});_tmp79[0]=_tmpB45;});_tmp79;});case 14: _LL4C: _tmpC3=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4C)->f1;_LL4D:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB47=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp7C;_tmp7C.tag=14;({struct Cyc_List_List*_tmpB46=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpC3);_tmp7C.f1=_tmpB46;});_tmp7C;});_tmp7B[0]=_tmpB47;});_tmp7B;});case 19: _LL4E: _tmpC4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->f1;_LL4F:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB49=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7E;_tmp7E.tag=19;({void*_tmpB48=Cyc_Tcutil_copy_type(_tmpC4);_tmp7E.f1=_tmpB48;});_tmp7E;});_tmp7D[0]=_tmpB49;});_tmp7D;});case 18: _LL50: _tmpC5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4C)->f1;_LL51:
# 271
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB4A=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp80;_tmp80.tag=18;_tmp80.f1=_tmpC5;_tmp80;});_tmp7F[0]=_tmpB4A;});_tmp7F;});case 27: _LL52: _tmpC6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp4C)->f1;_LL53:
# 274
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpB4B=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp82;_tmp82.tag=27;_tmp82.f1=_tmpC6;_tmp82;});_tmp81[0]=_tmpB4B;});_tmp81;});case 15: _LL54: _tmpC7=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4C)->f1;_LL55:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB4D=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp84;_tmp84.tag=15;({void*_tmpB4C=Cyc_Tcutil_copy_type(_tmpC7);_tmp84.f1=_tmpB4C;});_tmp84;});_tmp83[0]=_tmpB4D;});_tmp83;});case 16: _LL56: _tmpC9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f2;_LL57:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB50=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp86;_tmp86.tag=16;({void*_tmpB4E=Cyc_Tcutil_copy_type(_tmpC9);_tmp86.f1=_tmpB4E;});({void*_tmpB4F=Cyc_Tcutil_copy_type(_tmpC8);_tmp86.f2=_tmpB4F;});_tmp86;});_tmp85[0]=_tmpB50;});_tmp85;});case 17: _LL58: _tmpCC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f1;_tmpCB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f2;_tmpCA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f3;_LL59:
# 278
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB52=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp88;_tmp88.tag=17;_tmp88.f1=_tmpCC;({struct Cyc_List_List*_tmpB51=Cyc_Tcutil_copy_types(_tmpCB);_tmp88.f2=_tmpB51;});_tmp88.f3=_tmpCA;_tmp88.f4=0;_tmp88;});_tmp87[0]=_tmpB52;});_tmp87;});case 21: _LL5A: _LL5B:
 goto _LL5D;case 22: _LL5C: _LL5D:
 goto _LL5F;case 28: _LL5E: _LL5F:
 goto _LL61;case 20: _LL60: _LL61:
 return t;case 23: _LL62: _tmpCD=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4C)->f1;_LL63:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpB54=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8A;_tmp8A.tag=23;({void*_tmpB53=Cyc_Tcutil_copy_type(_tmpCD);_tmp8A.f1=_tmpB53;});_tmp8A;});_tmp89[0]=_tmpB54;});_tmp89;});case 24: _LL64: _tmpCE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4C)->f1;_LL65:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpB56=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8C;_tmp8C.tag=24;({struct Cyc_List_List*_tmpB55=Cyc_Tcutil_copy_types(_tmpCE);_tmp8C.f1=_tmpB55;});_tmp8C;});_tmp8B[0]=_tmpB56;});_tmp8B;});case 25: _LL66: _tmpCF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4C)->f1;_LL67:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpB58=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8E;_tmp8E.tag=25;({void*_tmpB57=Cyc_Tcutil_copy_type(_tmpCF);_tmp8E.f1=_tmpB57;});_tmp8E;});_tmp8D[0]=_tmpB58;});_tmp8D;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)){case 0: _LL68: _tmpD0=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL69: {
# 288
struct Cyc_List_List*_tmp8F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD0->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp90=_cycalloc(sizeof(*_tmp90));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB5B=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp91;_tmp91.tag=11;({struct Cyc_Absyn_AggrInfo _tmpB5A=({struct Cyc_Absyn_AggrInfo _tmp92;({union Cyc_Absyn_AggrInfoU _tmpB59=Cyc_Absyn_UnknownAggr(_tmpD0->kind,_tmpD0->name,0);_tmp92.aggr_info=_tmpB59;});_tmp92.targs=_tmp8F;_tmp92;});_tmp91.f1=_tmpB5A;});_tmp91;});_tmp90[0]=_tmpB5B;});_tmp90;});}case 1: _LL6A: _tmpD1=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL6B:
# 291
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp93=_cycalloc(sizeof(*_tmp93));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB5C=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp94;_tmp94.tag=13;_tmp94.f1=_tmpD1->name;_tmp94.f2=0;_tmp94;});_tmp93[0]=_tmpB5C;});_tmp93;});default: _LL6C: _tmpD2=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL6D: {
# 293
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD2->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB5F=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp97;_tmp97.tag=3;({struct Cyc_Absyn_DatatypeInfo _tmpB5E=({struct Cyc_Absyn_DatatypeInfo _tmp98;({union Cyc_Absyn_DatatypeInfoU _tmpB5D=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp99;_tmp99.name=_tmpD2->name;_tmp99.is_extensible=0;_tmp99;}));_tmp98.datatype_info=_tmpB5D;});_tmp98.targs=_tmp95;_tmp98;});_tmp97.f1=_tmpB5E;});_tmp97;});_tmp96[0]=_tmpB5F;});_tmp96;});}}}_LL2D:;}
# 308 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD3=d;struct _dyneither_ptr*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->tag == 0){_LL6F: _tmpD6=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->f1;_LL70:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpB61=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD5;_tmpD5.tag=0;({struct Cyc_Absyn_Exp*_tmpB60=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpD6);_tmpD5.f1=_tmpB60;});_tmpD5;});_tmpD4[0]=_tmpB61;});_tmpD4;});}else{_LL71: _tmpD7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD3)->f1;_LL72:
 return d;}_LL6E:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 314
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 316
return({struct _tuple19*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_List_List*_tmpB62=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpD8->f1=_tmpB62;});({struct Cyc_Absyn_Exp*_tmpB63=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpD8->f2=_tmpB63;});_tmpD8;});}
# 319
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpD9=preserve_types;
{void*_tmpDA=e->r;void*_tmpDB=_tmpDA;int _tmp167;struct _dyneither_ptr _tmp166;void*_tmp165;struct Cyc_Absyn_Exp*_tmp164;struct _dyneither_ptr*_tmp163;struct Cyc_Core_Opt*_tmp162;struct Cyc_List_List*_tmp161;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_Exp*_tmp15F;int _tmp15E;struct Cyc_Absyn_Exp*_tmp15D;void**_tmp15C;struct Cyc_Absyn_Exp*_tmp15B;int _tmp15A;int _tmp159;void*_tmp158;struct Cyc_Absyn_Enumfield*_tmp157;struct Cyc_Absyn_Enumdecl*_tmp156;struct Cyc_Absyn_Enumfield*_tmp155;struct Cyc_List_List*_tmp154;struct Cyc_Absyn_Datatypedecl*_tmp153;struct Cyc_Absyn_Datatypefield*_tmp152;void*_tmp151;struct Cyc_List_List*_tmp150;struct _tuple2*_tmp14F;struct Cyc_List_List*_tmp14E;struct Cyc_List_List*_tmp14D;struct Cyc_Absyn_Aggrdecl*_tmp14C;struct Cyc_Absyn_Exp*_tmp14B;void*_tmp14A;int _tmp149;struct Cyc_Absyn_Vardecl*_tmp148;struct Cyc_Absyn_Exp*_tmp147;struct Cyc_Absyn_Exp*_tmp146;int _tmp145;struct Cyc_List_List*_tmp144;struct _dyneither_ptr*_tmp143;struct Cyc_Absyn_Tqual _tmp142;void*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_List_List*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct Cyc_Absyn_Exp*_tmp13C;struct _dyneither_ptr*_tmp13B;int _tmp13A;int _tmp139;struct Cyc_Absyn_Exp*_tmp138;struct _dyneither_ptr*_tmp137;int _tmp136;int _tmp135;struct Cyc_Absyn_Exp*_tmp134;void*_tmp133;struct Cyc_List_List*_tmp132;struct Cyc_Absyn_Exp*_tmp131;void*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;void*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;int _tmp12A;enum Cyc_Absyn_Coercion _tmp129;struct Cyc_Absyn_Exp*_tmp128;struct Cyc_List_List*_tmp127;struct Cyc_Absyn_Exp*_tmp126;struct Cyc_Absyn_Exp*_tmp125;int _tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_List_List*_tmp122;struct Cyc_Absyn_VarargCallInfo*_tmp121;int _tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;enum Cyc_Absyn_Incrementor _tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Core_Opt*_tmp113;struct Cyc_Absyn_Exp*_tmp112;enum Cyc_Absyn_Primop _tmp111;struct Cyc_List_List*_tmp110;void*_tmp10F;union Cyc_Absyn_Cnst _tmp10E;switch(*((int*)_tmpDB)){case 0: _LL74: _tmp10E=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL75:
({struct Cyc_Absyn_Exp*_tmpB64=Cyc_Absyn_const_exp(_tmp10E,e->loc);new_e=_tmpB64;});goto _LL73;case 1: _LL76: _tmp10F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL77:
({struct Cyc_Absyn_Exp*_tmpB65=Cyc_Absyn_varb_exp(_tmp10F,e->loc);new_e=_tmpB65;});goto _LL73;case 2: _LL78: _tmp111=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp110=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL79:
({struct Cyc_Absyn_Exp*_tmpB68=({enum Cyc_Absyn_Primop _tmpB67=_tmp111;struct Cyc_List_List*_tmpB66=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp110);Cyc_Absyn_primop_exp(_tmpB67,_tmpB66,e->loc);});new_e=_tmpB68;});goto _LL73;case 3: _LL7A: _tmp114=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp113=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp112=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL7B:
# 327
({struct Cyc_Absyn_Exp*_tmpB6C=({struct Cyc_Absyn_Exp*_tmpB6B=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp114);struct Cyc_Core_Opt*_tmpB6A=(unsigned int)_tmp113?({struct Cyc_Core_Opt*_tmpDC=_cycalloc_atomic(sizeof(*_tmpDC));_tmpDC->v=(void*)_tmp113->v;_tmpDC;}): 0;struct Cyc_Absyn_Exp*_tmpB69=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp112);Cyc_Absyn_assignop_exp(_tmpB6B,_tmpB6A,_tmpB69,e->loc);});new_e=_tmpB6C;});
goto _LL73;case 4: _LL7C: _tmp116=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp115=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL7D:
({struct Cyc_Absyn_Exp*_tmpB6F=({struct Cyc_Absyn_Exp*_tmpB6E=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp116);enum Cyc_Absyn_Incrementor _tmpB6D=_tmp115;Cyc_Absyn_increment_exp(_tmpB6E,_tmpB6D,e->loc);});new_e=_tmpB6F;});goto _LL73;case 5: _LL7E: _tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp118=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp117=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL7F:
# 331
({struct Cyc_Absyn_Exp*_tmpB73=({struct Cyc_Absyn_Exp*_tmpB72=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp119);struct Cyc_Absyn_Exp*_tmpB71=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp118);struct Cyc_Absyn_Exp*_tmpB70=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp117);Cyc_Absyn_conditional_exp(_tmpB72,_tmpB71,_tmpB70,e->loc);});new_e=_tmpB73;});goto _LL73;case 6: _LL80: _tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL81:
({struct Cyc_Absyn_Exp*_tmpB76=({struct Cyc_Absyn_Exp*_tmpB75=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11B);struct Cyc_Absyn_Exp*_tmpB74=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11A);Cyc_Absyn_and_exp(_tmpB75,_tmpB74,e->loc);});new_e=_tmpB76;});goto _LL73;case 7: _LL82: _tmp11D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL83:
({struct Cyc_Absyn_Exp*_tmpB79=({struct Cyc_Absyn_Exp*_tmpB78=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11D);struct Cyc_Absyn_Exp*_tmpB77=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11C);Cyc_Absyn_or_exp(_tmpB78,_tmpB77,e->loc);});new_e=_tmpB79;});goto _LL73;case 8: _LL84: _tmp11F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL85:
({struct Cyc_Absyn_Exp*_tmpB7C=({struct Cyc_Absyn_Exp*_tmpB7B=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11F);struct Cyc_Absyn_Exp*_tmpB7A=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11E);Cyc_Absyn_seq_exp(_tmpB7B,_tmpB7A,e->loc);});new_e=_tmpB7C;});goto _LL73;case 9: _LL86: _tmp123=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp122=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp121=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp120=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL87:
# 336
{struct Cyc_Absyn_VarargCallInfo*_tmpDD=_tmp121;int _tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_VarargInfo*_tmpE9;if(_tmpDD != 0){_LLC5: _tmpEB=_tmpDD->num_varargs;_tmpEA=_tmpDD->injectors;_tmpE9=_tmpDD->vai;_LLC6: {
# 338
struct Cyc_Absyn_VarargInfo*_tmpDE=_tmpE9;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Tqual _tmpE5;void*_tmpE4;int _tmpE3;_LLCA: _tmpE6=_tmpDE->name;_tmpE5=_tmpDE->tq;_tmpE4=_tmpDE->type;_tmpE3=_tmpDE->inject;_LLCB:;
({struct Cyc_Absyn_Exp*_tmpB84=({void*_tmpB83=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpB82=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE0;_tmpE0.tag=9;({struct Cyc_Absyn_Exp*_tmpB7D=
Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp123);_tmpE0.f1=_tmpB7D;});({struct Cyc_List_List*_tmpB7E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp122);_tmpE0.f2=_tmpB7E;});({struct Cyc_Absyn_VarargCallInfo*_tmpB81=({struct Cyc_Absyn_VarargCallInfo*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->num_varargs=_tmpEB;_tmpE1->injectors=_tmpEA;({struct Cyc_Absyn_VarargInfo*_tmpB80=({struct Cyc_Absyn_VarargInfo*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->name=_tmpE6;_tmpE2->tq=_tmpE5;({void*_tmpB7F=
# 342
Cyc_Tcutil_copy_type(_tmpE4);_tmpE2->type=_tmpB7F;});_tmpE2->inject=_tmpE3;_tmpE2;});_tmpE1->vai=_tmpB80;});_tmpE1;});_tmpE0.f3=_tmpB81;});_tmpE0.f4=_tmp120;_tmpE0;});_tmpDF[0]=_tmpB82;});_tmpDF;});
# 339
Cyc_Absyn_new_exp(_tmpB83,e->loc);});new_e=_tmpB84;});
# 344
goto _LLC4;}}else{_LLC7: _LLC8:
# 346
({struct Cyc_Absyn_Exp*_tmpB89=({void*_tmpB88=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpB87=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE8;_tmpE8.tag=9;({struct Cyc_Absyn_Exp*_tmpB85=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp123);_tmpE8.f1=_tmpB85;});({struct Cyc_List_List*_tmpB86=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp122);_tmpE8.f2=_tmpB86;});_tmpE8.f3=_tmp121;_tmpE8.f4=_tmp120;_tmpE8;});_tmpE7[0]=_tmpB87;});_tmpE7;});Cyc_Absyn_new_exp(_tmpB88,e->loc);});new_e=_tmpB89;});
goto _LLC4;}_LLC4:;}
# 349
goto _LL73;case 10: _LL88: _tmp125=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp124=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL89:
# 351
({struct Cyc_Absyn_Exp*_tmpB8C=_tmp124?({struct Cyc_Absyn_Exp*_tmpB8B=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp125);Cyc_Absyn_rethrow_exp(_tmpB8B,e->loc);}):({struct Cyc_Absyn_Exp*_tmpB8A=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp125);Cyc_Absyn_throw_exp(_tmpB8A,e->loc);});new_e=_tmpB8C;});
goto _LL73;case 11: _LL8A: _tmp126=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL8B:
({struct Cyc_Absyn_Exp*_tmpB8E=({struct Cyc_Absyn_Exp*_tmpB8D=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp126);Cyc_Absyn_noinstantiate_exp(_tmpB8D,e->loc);});new_e=_tmpB8E;});
goto _LL73;case 12: _LL8C: _tmp128=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp127=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL8D:
# 356
({struct Cyc_Absyn_Exp*_tmpB91=({struct Cyc_Absyn_Exp*_tmpB90=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp128);struct Cyc_List_List*_tmpB8F=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp127);Cyc_Absyn_instantiate_exp(_tmpB90,_tmpB8F,e->loc);});new_e=_tmpB91;});
goto _LL73;case 13: _LL8E: _tmp12C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp12A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp129=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL8F:
# 359
({struct Cyc_Absyn_Exp*_tmpB96=({void*_tmpB95=Cyc_Tcutil_copy_type(_tmp12C);struct Cyc_Absyn_Exp*_tmpB94=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12B);int _tmpB93=_tmp12A;enum Cyc_Absyn_Coercion _tmpB92=_tmp129;Cyc_Absyn_cast_exp(_tmpB95,_tmpB94,_tmpB93,_tmpB92,e->loc);});new_e=_tmpB96;});goto _LL73;case 14: _LL90: _tmp12D=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL91:
({struct Cyc_Absyn_Exp*_tmpB98=({struct Cyc_Absyn_Exp*_tmpB97=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12D);Cyc_Absyn_address_exp(_tmpB97,e->loc);});new_e=_tmpB98;});goto _LL73;case 15: _LL92: _tmp12F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL93: {
# 362
struct Cyc_Absyn_Exp*eo1=_tmp12F;if(_tmp12F != 0)({struct Cyc_Absyn_Exp*_tmpB99=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12F);eo1=_tmpB99;});
({struct Cyc_Absyn_Exp*_tmpB9C=({struct Cyc_Absyn_Exp*_tmpB9B=eo1;struct Cyc_Absyn_Exp*_tmpB9A=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12E);Cyc_Absyn_New_exp(_tmpB9B,_tmpB9A,e->loc);});new_e=_tmpB9C;});
goto _LL73;}case 16: _LL94: _tmp130=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL95:
({struct Cyc_Absyn_Exp*_tmpB9E=({void*_tmpB9D=Cyc_Tcutil_copy_type(_tmp130);Cyc_Absyn_sizeoftyp_exp(_tmpB9D,e->loc);});new_e=_tmpB9E;});
goto _LL73;case 17: _LL96: _tmp131=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL97:
({struct Cyc_Absyn_Exp*_tmpBA0=({struct Cyc_Absyn_Exp*_tmpB9F=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp131);Cyc_Absyn_sizeofexp_exp(_tmpB9F,e->loc);});new_e=_tmpBA0;});goto _LL73;case 18: _LL98: _tmp133=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp132=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL99:
# 369
({struct Cyc_Absyn_Exp*_tmpBA3=({void*_tmpBA2=Cyc_Tcutil_copy_type(_tmp133);struct Cyc_List_List*_tmpBA1=_tmp132;Cyc_Absyn_offsetof_exp(_tmpBA2,_tmpBA1,e->loc);});new_e=_tmpBA3;});goto _LL73;case 19: _LL9A: _tmp134=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL9B:
({struct Cyc_Absyn_Exp*_tmpBA5=({struct Cyc_Absyn_Exp*_tmpBA4=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp134);Cyc_Absyn_deref_exp(_tmpBA4,e->loc);});new_e=_tmpBA5;});goto _LL73;case 20: _LL9C: _tmp138=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp137=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp136=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp135=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL9D:
# 372
({struct Cyc_Absyn_Exp*_tmpBA9=({void*_tmpBA8=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpBA7=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpED;_tmpED.tag=20;({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp138);_tmpED.f1=_tmpBA6;});_tmpED.f2=_tmp137;_tmpED.f3=_tmp136;_tmpED.f4=_tmp135;_tmpED;});_tmpEC[0]=_tmpBA7;});_tmpEC;});Cyc_Absyn_new_exp(_tmpBA8,e->loc);});new_e=_tmpBA9;});goto _LL73;case 21: _LL9E: _tmp13C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp13A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp139=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL9F:
# 374
({struct Cyc_Absyn_Exp*_tmpBAD=({void*_tmpBAC=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpBAB=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpEF;_tmpEF.tag=21;({struct Cyc_Absyn_Exp*_tmpBAA=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13C);_tmpEF.f1=_tmpBAA;});_tmpEF.f2=_tmp13B;_tmpEF.f3=_tmp13A;_tmpEF.f4=_tmp139;_tmpEF;});_tmpEE[0]=_tmpBAB;});_tmpEE;});Cyc_Absyn_new_exp(_tmpBAC,e->loc);});new_e=_tmpBAD;});goto _LL73;case 22: _LLA0: _tmp13E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLA1:
({struct Cyc_Absyn_Exp*_tmpBB0=({struct Cyc_Absyn_Exp*_tmpBAF=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13E);struct Cyc_Absyn_Exp*_tmpBAE=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13D);Cyc_Absyn_subscript_exp(_tmpBAF,_tmpBAE,e->loc);});new_e=_tmpBB0;});
goto _LL73;case 23: _LLA2: _tmp13F=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLA3:
({struct Cyc_Absyn_Exp*_tmpBB2=({struct Cyc_List_List*_tmpBB1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp13F);Cyc_Absyn_tuple_exp(_tmpBB1,e->loc);});new_e=_tmpBB2;});goto _LL73;case 24: _LLA4: _tmp143=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f1;_tmp142=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f2;_tmp141=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f3;_tmp140=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLA5: {
# 379
struct _dyneither_ptr*vopt1=_tmp143;
if(_tmp143 != 0)vopt1=_tmp143;
({struct Cyc_Absyn_Exp*_tmpBB8=({void*_tmpBB7=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpBB6=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF1;_tmpF1.tag=24;({struct _tuple10*_tmpBB4=({struct _tuple10*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=vopt1;_tmpF2->f2=_tmp142;({void*_tmpBB3=Cyc_Tcutil_copy_type(_tmp141);_tmpF2->f3=_tmpBB3;});_tmpF2;});_tmpF1.f1=_tmpBB4;});({struct Cyc_List_List*_tmpBB5=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp140);_tmpF1.f2=_tmpBB5;});_tmpF1;});_tmpF0[0]=_tmpBB6;});_tmpF0;});
# 381
Cyc_Absyn_new_exp(_tmpBB7,e->loc);});new_e=_tmpBB8;});
# 383
goto _LL73;}case 25: _LLA6: _tmp144=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLA7:
({struct Cyc_Absyn_Exp*_tmpBBC=({void*_tmpBBB=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpBBA=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF4;_tmpF4.tag=25;({struct Cyc_List_List*_tmpBB9=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp144);_tmpF4.f1=_tmpBB9;});_tmpF4;});_tmpF3[0]=_tmpBBA;});_tmpF3;});Cyc_Absyn_new_exp(_tmpBBB,e->loc);});new_e=_tmpBBC;});
goto _LL73;case 26: _LLA8: _tmp148=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp147=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp146=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp145=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LLA9:
# 387
({struct Cyc_Absyn_Exp*_tmpBC1=({void*_tmpBC0=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpBBF=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF6;_tmpF6.tag=26;_tmpF6.f1=_tmp148;({struct Cyc_Absyn_Exp*_tmpBBD=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp147);_tmpF6.f2=_tmpBBD;});({struct Cyc_Absyn_Exp*_tmpBBE=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp146);_tmpF6.f3=_tmpBBE;});_tmpF6.f4=_tmp145;_tmpF6;});_tmpF5[0]=_tmpBBF;});_tmpF5;});Cyc_Absyn_new_exp(_tmpBC0,e->loc);});new_e=_tmpBC1;});
goto _LL73;case 27: _LLAA: _tmp14B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14A=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp149=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLAB:
# 390
({struct Cyc_Absyn_Exp*_tmpBC6=({void*_tmpBC5=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpBC4=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF8;_tmpF8.tag=27;({struct Cyc_Absyn_Exp*_tmpBC2=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp14B);_tmpF8.f1=_tmpBC2;});({void*_tmpBC3=Cyc_Tcutil_copy_type(_tmp14A);_tmpF8.f2=_tmpBC3;});_tmpF8.f3=_tmp149;_tmpF8;});_tmpF7[0]=_tmpBC4;});_tmpF7;});Cyc_Absyn_new_exp(_tmpBC5,_tmp14B->loc);});new_e=_tmpBC6;});
# 392
goto _LL73;case 28: _LLAC: _tmp14F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp14D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp14C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LLAD:
# 394
({struct Cyc_Absyn_Exp*_tmpBCB=({void*_tmpBCA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpBC9=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpFA;_tmpFA.tag=28;_tmpFA.f1=_tmp14F;({struct Cyc_List_List*_tmpBC7=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp14E);_tmpFA.f2=_tmpBC7;});({struct Cyc_List_List*_tmpBC8=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp14D);_tmpFA.f3=_tmpBC8;});_tmpFA.f4=_tmp14C;_tmpFA;});_tmpF9[0]=_tmpBC9;});_tmpF9;});Cyc_Absyn_new_exp(_tmpBCA,e->loc);});new_e=_tmpBCB;});
# 396
goto _LL73;case 29: _LLAE: _tmp151=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp150=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLAF:
# 398
({struct Cyc_Absyn_Exp*_tmpBD0=({void*_tmpBCF=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpBCE=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpFC;_tmpFC.tag=29;({void*_tmpBCC=Cyc_Tcutil_copy_type(_tmp151);_tmpFC.f1=_tmpBCC;});({struct Cyc_List_List*_tmpBCD=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp150);_tmpFC.f2=_tmpBCD;});_tmpFC;});_tmpFB[0]=_tmpBCE;});_tmpFB;});Cyc_Absyn_new_exp(_tmpBCF,e->loc);});new_e=_tmpBD0;});
goto _LL73;case 30: _LLB0: _tmp154=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp153=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp152=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLB1:
# 401
({struct Cyc_Absyn_Exp*_tmpBD4=({void*_tmpBD3=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpBD2=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpFE;_tmpFE.tag=30;({struct Cyc_List_List*_tmpBD1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp154);_tmpFE.f1=_tmpBD1;});_tmpFE.f2=_tmp153;_tmpFE.f3=_tmp152;_tmpFE;});_tmpFD[0]=_tmpBD2;});_tmpFD;});Cyc_Absyn_new_exp(_tmpBD3,e->loc);});new_e=_tmpBD4;});
goto _LL73;case 31: _LLB2: _tmp156=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp155=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB3:
 new_e=e;goto _LL73;case 32: _LLB4: _tmp158=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp157=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB5:
# 405
({struct Cyc_Absyn_Exp*_tmpBD8=({void*_tmpBD7=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpBD6=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp100;_tmp100.tag=32;({void*_tmpBD5=Cyc_Tcutil_copy_type(_tmp158);_tmp100.f1=_tmpBD5;});_tmp100.f2=_tmp157;_tmp100;});_tmpFF[0]=_tmpBD6;});_tmpFF;});Cyc_Absyn_new_exp(_tmpBD7,e->loc);});new_e=_tmpBD8;});
goto _LL73;case 33: _LLB6: _tmp15E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).is_calloc;_tmp15D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).rgn;_tmp15C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).elt_type;_tmp15B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).num_elts;_tmp15A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).fat_result;_tmp159=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).inline_call;_LLB7: {
# 408
struct Cyc_Absyn_Exp*_tmp101=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp15D;if(_tmp15D != 0)({struct Cyc_Absyn_Exp*_tmpBD9=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp15D);r1=_tmpBD9;});{
void**t1=_tmp15C;if(_tmp15C != 0)({void**_tmpBDB=({void**_tmp102=_cycalloc(sizeof(*_tmp102));({void*_tmpBDA=Cyc_Tcutil_copy_type(*_tmp15C);_tmp102[0]=_tmpBDA;});_tmp102;});t1=_tmpBDB;});
({void*_tmpBDE=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp103=_cycalloc(sizeof(*_tmp103));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpBDD=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp104;_tmp104.tag=33;({struct Cyc_Absyn_MallocInfo _tmpBDC=({struct Cyc_Absyn_MallocInfo _tmp105;_tmp105.is_calloc=_tmp15E;_tmp105.rgn=r1;_tmp105.elt_type=t1;_tmp105.num_elts=_tmp15B;_tmp105.fat_result=_tmp15A;_tmp105.inline_call=_tmp159;_tmp105;});_tmp104.f1=_tmpBDC;});_tmp104;});_tmp103[0]=_tmpBDD;});_tmp103;});_tmp101->r=_tmpBDE;});
new_e=_tmp101;
goto _LL73;};}case 34: _LLB8: _tmp160=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp15F=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLB9:
({struct Cyc_Absyn_Exp*_tmpBE1=({struct Cyc_Absyn_Exp*_tmpBE0=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp160);struct Cyc_Absyn_Exp*_tmpBDF=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp15F);Cyc_Absyn_swap_exp(_tmpBE0,_tmpBDF,e->loc);});new_e=_tmpBE1;});goto _LL73;case 35: _LLBA: _tmp162=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp161=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLBB: {
# 416
struct Cyc_Core_Opt*nopt1=_tmp162;
if(_tmp162 != 0)({struct Cyc_Core_Opt*_tmpBE2=({struct Cyc_Core_Opt*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->v=(struct _tuple2*)_tmp162->v;_tmp106;});nopt1=_tmpBE2;});
({struct Cyc_Absyn_Exp*_tmpBE6=({void*_tmpBE5=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpBE4=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=35;_tmp108.f1=nopt1;({struct Cyc_List_List*_tmpBE3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp161);_tmp108.f2=_tmpBE3;});_tmp108;});_tmp107[0]=_tmpBE4;});_tmp107;});Cyc_Absyn_new_exp(_tmpBE5,e->loc);});new_e=_tmpBE6;});
goto _LL73;}case 36: _LLBC: _LLBD:
# 421
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp109=_cycalloc(sizeof(*_tmp109));({struct Cyc_Core_Failure_exn_struct _tmpBE8=({struct Cyc_Core_Failure_exn_struct _tmp10A;_tmp10A.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmpBE7=({const char*_tmp10B="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp10B,sizeof(char),45);});_tmp10A.f1=_tmpBE7;});_tmp10A;});_tmp109[0]=_tmpBE8;});_tmp109;}));case 37: _LLBE: _tmp164=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp163=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLBF:
({struct Cyc_Absyn_Exp*_tmpBEC=({void*_tmpBEB=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpBEA=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp10D;_tmp10D.tag=37;({struct Cyc_Absyn_Exp*_tmpBE9=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp164);_tmp10D.f1=_tmpBE9;});_tmp10D.f2=_tmp163;_tmp10D;});_tmp10C[0]=_tmpBEA;});_tmp10C;});Cyc_Absyn_new_exp(_tmpBEB,e->loc);});new_e=_tmpBEC;});
goto _LL73;case 38: _LLC0: _tmp165=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LLC1:
({struct Cyc_Absyn_Exp*_tmpBEE=({void*_tmpBED=Cyc_Tcutil_copy_type(_tmp165);Cyc_Absyn_valueof_exp(_tmpBED,e->loc);});new_e=_tmpBEE;});
goto _LL73;default: _LLC2: _tmp167=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp166=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLC3:
({struct Cyc_Absyn_Exp*_tmpBEF=Cyc_Absyn_asm_exp(_tmp167,_tmp166,e->loc);new_e=_tmpBEF;});goto _LL73;}_LL73:;}
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
({void*_tmpBF1=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp176=_cycalloc(sizeof(*_tmp176));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpBF0=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp177;_tmp177.tag=2;_tmp177.f1=0;_tmp177.f2=def;_tmp177;});_tmp176[0]=_tmpBF0;});_tmp176;});tv->kind=_tmpBF1;});
return def;}_LLE2:;}
# 482
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp17A=({struct _tuple0 _tmp191;({void*_tmpBF2=Cyc_Absyn_compress_kb(kb1);_tmp191.f1=_tmpBF2;});({void*_tmpBF3=Cyc_Absyn_compress_kb(kb2);_tmp191.f2=_tmpBF3;});_tmp191;});struct _tuple0 _tmp17B=_tmp17A;struct Cyc_Core_Opt**_tmp190;void*_tmp18F;void*_tmp18E;struct Cyc_Core_Opt**_tmp18D;struct Cyc_Core_Opt**_tmp18C;struct Cyc_Absyn_Kind*_tmp18B;struct Cyc_Core_Opt**_tmp18A;struct Cyc_Absyn_Kind*_tmp189;struct Cyc_Core_Opt**_tmp188;struct Cyc_Absyn_Kind*_tmp187;struct Cyc_Absyn_Kind*_tmp186;struct Cyc_Absyn_Kind*_tmp185;struct Cyc_Core_Opt**_tmp184;struct Cyc_Absyn_Kind*_tmp183;struct Cyc_Absyn_Kind*_tmp182;struct Cyc_Absyn_Kind*_tmp181;switch(*((int*)_tmp17B.f1)){case 0: switch(*((int*)_tmp17B.f2)){case 0: _LLEA: _tmp182=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp181=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLEB:
 return _tmp182 == _tmp181;case 2: _LLEE: _tmp185=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp184=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_tmp183=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f2;_LLEF:
# 491
 if(Cyc_Tcutil_kind_leq(_tmp185,_tmp183)){
({struct Cyc_Core_Opt*_tmpBF4=({struct Cyc_Core_Opt*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->v=kb1;_tmp17D;});*_tmp184=_tmpBF4;});
return 1;}else{
return 0;}default: goto _LLF4;}case 2: switch(*((int*)_tmp17B.f2)){case 0: _LLEC: _tmp188=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp187=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f2;_tmp186=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLED:
# 486
 if(Cyc_Tcutil_kind_leq(_tmp186,_tmp187)){
({struct Cyc_Core_Opt*_tmpBF5=({struct Cyc_Core_Opt*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->v=kb2;_tmp17C;});*_tmp188=_tmpBF5;});
return 1;}else{
return 0;}case 2: _LLF0: _tmp18C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f1;_tmp18B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f1)->f2;_tmp18A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_tmp189=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f2;_LLF1:
# 496
 if(Cyc_Tcutil_kind_leq(_tmp18B,_tmp189)){
({struct Cyc_Core_Opt*_tmpBF6=({struct Cyc_Core_Opt*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->v=kb1;_tmp17E;});*_tmp18A=_tmpBF6;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp189,_tmp18B)){
({struct Cyc_Core_Opt*_tmpBF7=({struct Cyc_Core_Opt*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->v=kb2;_tmp17F;});*_tmp18C=_tmpBF7;});
return 1;}else{
return 0;}}default: _LLF4: _tmp18E=_tmp17B.f1;_tmp18D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17B.f2)->f1;_LLF5:
# 505
({struct Cyc_Core_Opt*_tmpBF8=({struct Cyc_Core_Opt*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=_tmp18E;_tmp180;});*_tmp18D=_tmpBF8;});
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
({struct Cyc_String_pa_PrintArg_struct _tmp198;_tmp198.tag=0;({struct _dyneither_ptr _tmpBF9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp198.f1=_tmpBF9;});({void*_tmp196[1]={& _tmp198};({struct _dyneither_ptr _tmpBFA=({const char*_tmp197="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp197,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBFA,_tag_dyneither(_tmp196,sizeof(void*),1));});});});}_LLF6:;}
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
({struct Cyc_List_List*_tmpBFC=({struct Cyc_List_List*_tmp1A0=_region_malloc(temp,sizeof(*_tmp1A0));({struct _tuple16*_tmpBFB=({struct _tuple16*_tmp1A1=_region_malloc(temp,sizeof(*_tmp1A1));_tmp1A1->f1=_tmp19C;_tmp1A1->f2=_tmp19D;_tmp1A1;});_tmp1A0->hd=_tmpBFB;});_tmp1A0->tl=_tmp19B;_tmp1A0;});_tmp19B=_tmpBFC;});goto _LLFB;default: _LL100: _LL101:
 goto _LLFB;}_LLFB:;}
# 542
if(_tmp19B != 0){
({void*_tmpBFF=({struct _RegionHandle*_tmpBFE=temp;struct Cyc_List_List*_tmpBFD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp19B);Cyc_Tcutil_rsubstitute(_tmpBFE,_tmpBFD,field_typ);});field_typ=_tmpBFF;});
({struct Cyc_Absyn_Kind*_tmpC00=Cyc_Tcutil_typ_kind(field_typ);k=_tmpC00;});}}
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
({void*_tmp1A4=0;({struct _dyneither_ptr _tmpC01=({const char*_tmp1A5="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1A5,sizeof(char),39);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC01,_tag_dyneither(_tmp1A4,sizeof(void*),0));});});}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A3)->f1).aggr_info).UnknownAggr).tag == 1){_LL123: _LL124:
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
({struct Cyc_String_pa_PrintArg_struct _tmp1B3;_tmp1B3.tag=0;({struct _dyneither_ptr _tmpC02=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1B3.f1=_tmpC02;});({void*_tmp1B1[1]={& _tmp1B3};({struct _dyneither_ptr _tmpC03=({const char*_tmp1B2="typ_kind: typedef found: %s";_tag_dyneither(_tmp1B2,sizeof(char),28);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC03,_tag_dyneither(_tmp1B1,sizeof(void*),1));});});});
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
({void*_tmpC04=Cyc_Tcutil_compress(t);t=_tmpC04;});{
void*_tmp1C8=t;struct Cyc_List_List*_tmp1EE;void*_tmp1ED;void*_tmp1EC;void*_tmp1EB;void*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E4;struct Cyc_List_List*_tmp1E3;void*_tmp1E2;struct Cyc_Absyn_Tqual _tmp1E1;void*_tmp1E0;struct Cyc_List_List*_tmp1DF;int _tmp1DE;struct Cyc_Absyn_VarargInfo*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Exp*_tmp1DA;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Exp*_tmp1D8;struct Cyc_List_List*_tmp1D7;void*_tmp1D6;struct Cyc_Absyn_PtrInfo _tmp1D5;void*_tmp1D4;struct Cyc_Core_Opt**_tmp1D3;struct Cyc_Absyn_Tvar*_tmp1D2;switch(*((int*)_tmp1C8)){case 2: _LL15E: _tmp1D2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C8)->f1;_LL15F:
# 665
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1D2)){
({struct _dyneither_ptr _tmpC05=({const char*_tmp1C9="(type variable would escape scope)";_tag_dyneither(_tmp1C9,sizeof(char),35);});Cyc_Tcutil_failure_reason=_tmpC05;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 669
goto _LL15D;case 1: _LL160: _tmp1D4=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C8)->f2;_tmp1D3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C8)->f4;_LL161:
# 671
 if(t == evar){
({struct _dyneither_ptr _tmpC06=({const char*_tmp1CA="(occurs check)";_tag_dyneither(_tmp1CA,sizeof(char),15);});Cyc_Tcutil_failure_reason=_tmpC06;});
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
({struct Cyc_List_List*_tmpC07=({struct Cyc_List_List*_tmp1CC=_cycalloc(sizeof(*_tmp1CC));_tmp1CC->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1CC->tl=_tmp1CB;_tmp1CC;});_tmp1CB=_tmpC07;});}}
# 691
({struct Cyc_Core_Opt*_tmpC08=({struct Cyc_Core_Opt*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->v=_tmp1CB;_tmp1CD;});*_tmp1D3=_tmpC08;});}}}
# 694
goto _LL15D;case 5: _LL162: _tmp1D5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C8)->f1;_LL163:
# 697
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D5.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1D5.ptr_atts).rgn);
goto _LL15D;case 8: _LL164: _tmp1D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C8)->f1).elt_type;_LL165:
# 701
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D6);goto _LL15D;case 9: _LL166: _tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).tvars;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).effect;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ret_tqual;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ret_typ;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).args;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).c_varargs;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).cyc_varargs;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).rgn_po;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).attributes;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_clause;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).requires_relns;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_clause;_tmp1D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C8)->f1).ensures_relns;_LL167:
# 704
({struct Cyc_List_List*_tmpC09=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1E3,env);env=_tmpC09;});
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
({void*_tmp1EF=0;({struct _dyneither_ptr _tmpC0A=({const char*_tmp1F0="tq1 real_const not set.";_tag_dyneither(_tmp1F0,sizeof(char),24);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC0A,_tag_dyneither(_tmp1EF,sizeof(void*),0));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1F1=0;({struct _dyneither_ptr _tmpC0B=({const char*_tmp1F2="tq2 real_const not set.";_tag_dyneither(_tmp1F2,sizeof(char),24);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC0B,_tag_dyneither(_tmp1F1,sizeof(void*),0));});});
# 762
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 765
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
({struct _dyneither_ptr _tmpC0C=({const char*_tmp1F3="(qualifiers don't match)";_tag_dyneither(_tmp1F3,sizeof(char),25);});Cyc_Tcutil_failure_reason=_tmpC0C;});
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
({union Cyc_Absyn_Constraint*_tmpC0D=Cyc_Absyn_compress_conref(x);x=_tmpC0D;});
({union Cyc_Absyn_Constraint*_tmpC0E=Cyc_Absyn_compress_conref(y);y=_tmpC0E;});
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1F4=x;void*_tmp1FC;switch((((union Cyc_Absyn_Constraint*)_tmp1F4)->Eq_constr).tag){case 3: _LL184: _LL185:
# 791
({union Cyc_Absyn_Constraint _tmpC0F=({union Cyc_Absyn_Constraint _tmp1F5;(_tmp1F5.Forward_constr).val=y;(_tmp1F5.Forward_constr).tag=2;_tmp1F5;});*x=_tmpC0F;});return;case 1: _LL186: _tmp1FC=(void*)(_tmp1F4->Eq_constr).val;_LL187: {
# 793
union Cyc_Absyn_Constraint*_tmp1F6=y;void*_tmp1F9;switch((((union Cyc_Absyn_Constraint*)_tmp1F6)->Eq_constr).tag){case 3: _LL18B: _LL18C:
*y=*x;return;case 1: _LL18D: _tmp1F9=(void*)(_tmp1F6->Eq_constr).val;_LL18E:
# 796
 if(({cmp(_tmp1FC,_tmp1F9)!= 0;})){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 800
return;default: _LL18F: _LL190:
({void*_tmp1F7=0;({struct _dyneither_ptr _tmpC10=({const char*_tmp1F8="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1F8,sizeof(char),40);});Cyc_Tcutil_impos(_tmpC10,_tag_dyneither(_tmp1F7,sizeof(void*),0));});});}_LL18A:;}default: _LL188: _LL189:
# 803
({void*_tmp1FA=0;({struct _dyneither_ptr _tmpC11=({const char*_tmp1FB="unify_conref: forward after compress";_tag_dyneither(_tmp1FB,sizeof(char),37);});Cyc_Tcutil_impos(_tmpC11,_tag_dyneither(_tmp1FA,sizeof(void*),0));});});}_LL183:;};}
# 807
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp1FD;_push_handler(& _tmp1FD);{int _tmp1FF=0;if(setjmp(_tmp1FD.handler))_tmp1FF=1;if(!_tmp1FF){
({int(*_tmpC14)(void*,void*)=cmp;union Cyc_Absyn_Constraint*_tmpC13=x;union Cyc_Absyn_Constraint*_tmpC12=y;Cyc_Tcutil_unify_it_conrefs(_tmpC14,_tmpC13,_tmpC12,_tag_dyneither(0,0,0));});{
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
({void*_tmpC16=_tmp214;Cyc_Tcutil_unify_it(_tmpC16,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp20C=_cycalloc(sizeof(*_tmp20C));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC15=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp20D;_tmp20D.tag=18;_tmp20D.f1=e2;_tmp20D;});_tmp20C[0]=_tmpC15;});_tmp20C;}));});return;}else{_LL1AD: _LL1AE:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL1AA:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f2)->tag == 38){_LL1A6: _tmp215=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp209.f2)->f1;_LL1A7: {
# 836
void*_tmp20E=Cyc_Tcutil_compress(_tmp215);void*_tmp20F=_tmp20E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp20F)->tag == 1){_LL1B0: _LL1B1:
({void*_tmpC18=_tmp215;Cyc_Tcutil_unify_it(_tmpC18,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp210=_cycalloc(sizeof(*_tmp210));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC17=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp211;_tmp211.tag=18;_tmp211.f1=e1;_tmp211;});_tmp210[0]=_tmpC17;});_tmp210;}));});return;}else{_LL1B2: _LL1B3:
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
 return({int _tmpC19=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpC19,Cyc_Tcutil_attribute_case_number(att2));});}_LL1EF:;}
# 910
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return({Cyc_Tcutil_attribute_cmp(a1,a2)== 0;});}
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
({void*_tmpC1C=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp238=_cycalloc(sizeof(*_tmp238));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC1B=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp239;_tmp239.tag=18;({struct Cyc_Absyn_Exp*_tmpC1A=Cyc_Absyn_uint_exp(0,0);_tmp239.f1=_tmpC1A;});_tmp239;});_tmp238[0]=_tmpC1B;});_tmp238;});t=_tmpC1C;});goto _LL1FC;default: _LL205: _LL206:
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
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp23F=_cycalloc(sizeof(*_tmp23F));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC1D=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp240;_tmp240.tag=23;_tmp240.f1=t;_tmp240;});_tmp23F[0]=_tmpC1D;});_tmp23F;});case Cyc_Absyn_EffKind: _LL245: _LL246:
 return t;case Cyc_Absyn_IntKind: _LL247: _LL248:
 return Cyc_Absyn_empty_effect;default: _LL249: _LL24A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp241=_cycalloc(sizeof(*_tmp241));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC1E=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp242;_tmp242.tag=25;_tmp242.f1=t;_tmp242;});_tmp241[0]=_tmpC1E;});_tmp241;});}_LL242:;}case 15: _LL21C: _tmp259=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp23C)->f1;_LL21D:
# 963
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp243=_cycalloc(sizeof(*_tmp243));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC1F=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp244;_tmp244.tag=23;_tmp244.f1=_tmp259;_tmp244;});_tmp243[0]=_tmpC1F;});_tmp243;});case 16: _LL21E: _tmp25B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23C)->f1;_tmp25A=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23C)->f2;_LL21F:
# 967
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp245=_cycalloc(sizeof(*_tmp245));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC20=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp246;_tmp246.tag=23;_tmp246.f1=_tmp25A;_tmp246;});_tmp245[0]=_tmpC20;});_tmp245;});case 3: _LL220: _tmp25C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL221: {
# 969
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp25C);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp247=_cycalloc(sizeof(*_tmp247));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC21=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp248;_tmp248.tag=24;_tmp248.f1=ts;_tmp248;});_tmp247[0]=_tmpC21;});_tmp247;}));}case 5: _LL222: _tmp25E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23C)->f1).elt_typ;_tmp25D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23C)->f1).ptr_atts).rgn;_LL223:
# 973
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp249=_cycalloc(sizeof(*_tmp249));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC26=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp24A;_tmp24A.tag=24;({struct Cyc_List_List*_tmpC25=({void*_tmp24B[2];({void*_tmpC22=Cyc_Tcutil_rgns_of(_tmp25E);_tmp24B[1]=_tmpC22;});({void*_tmpC24=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp24C=_cycalloc(sizeof(*_tmp24C));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC23=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp24D;_tmp24D.tag=23;_tmp24D.f1=_tmp25D;_tmp24D;});_tmp24C[0]=_tmpC23;});_tmp24C;});_tmp24B[0]=_tmpC24;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp24B,sizeof(void*),2));});_tmp24A.f1=_tmpC25;});_tmp24A;});_tmp249[0]=_tmpC26;});_tmp249;}));case 8: _LL224: _tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23C)->f1).elt_type;_LL225:
# 975
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp25F));case 10: _LL226: _tmp260=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23C)->f1;_LL227: {
# 977
struct Cyc_List_List*_tmp24E=0;
for(0;_tmp260 != 0;_tmp260=_tmp260->tl){
({struct Cyc_List_List*_tmpC27=({struct Cyc_List_List*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F->hd=(*((struct _tuple12*)_tmp260->hd)).f2;_tmp24F->tl=_tmp24E;_tmp24F;});_tmp24E=_tmpC27;});}
_tmp261=_tmp24E;goto _LL229;}case 4: _LL228: _tmp261=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL229:
 _tmp262=_tmp261;goto _LL22B;case 11: _LL22A: _tmp262=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23C)->f1).targs;_LL22B:
# 983
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp250=_cycalloc(sizeof(*_tmp250));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC29=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp251;_tmp251.tag=24;({struct Cyc_List_List*_tmpC28=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp262);_tmp251.f1=_tmpC28;});_tmp251;});_tmp250[0]=_tmpC29;});_tmp250;}));case 12: _LL22C: _tmp263=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23C)->f2;_LL22D:
# 985
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp252=_cycalloc(sizeof(*_tmp252));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC2B=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp253;_tmp253.tag=24;({struct Cyc_List_List*_tmpC2A=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp263);_tmp253.f1=_tmpC2A;});_tmp253;});_tmp252[0]=_tmpC2B;});_tmp252;}));case 19: _LL22E: _LL22F:
 return Cyc_Absyn_empty_effect;case 9: _LL230: _tmp26A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).tvars;_tmp269=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).effect;_tmp268=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).ret_tqual;_tmp267=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).ret_typ;_tmp266=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).args;_tmp265=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).cyc_varargs;_tmp264=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23C)->f1).rgn_po;_LL231: {
# 995
void*_tmp254=({struct Cyc_List_List*_tmpC2C=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp26A);Cyc_Tcutil_substitute(_tmpC2C,(void*)_check_null(_tmp269));});
return Cyc_Tcutil_normalize_effect(_tmp254);}case 21: _LL232: _LL233:
 goto _LL235;case 22: _LL234: _LL235:
 goto _LL237;case 20: _LL236: _LL237:
 return Cyc_Absyn_empty_effect;case 23: _LL238: _LL239:
 goto _LL23B;case 24: _LL23A: _LL23B:
 return t;case 25: _LL23C: _tmp26B=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp23C)->f1;_LL23D:
 return Cyc_Tcutil_rgns_of(_tmp26B);case 17: _LL23E: _tmp26C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23C)->f2;_LL23F:
# 1004
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp255=_cycalloc(sizeof(*_tmp255));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC2E=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp256;_tmp256.tag=24;({struct Cyc_List_List*_tmpC2D=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp26C);_tmp256.f1=_tmpC2D;});_tmp256;});_tmp255[0]=_tmpC2E;});_tmp255;}));default: _LL240: _LL241:
({void*_tmp257=0;({struct _dyneither_ptr _tmpC2F=({const char*_tmp258="typedecl in rgns_of";_tag_dyneither(_tmp258,sizeof(char),20);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC2F,_tag_dyneither(_tmp257,sizeof(void*),0));});});}_LL207:;}
# 1012
void*Cyc_Tcutil_normalize_effect(void*e){
({void*_tmpC30=Cyc_Tcutil_compress(e);e=_tmpC30;});{
void*_tmp26D=e;void*_tmp279;struct Cyc_List_List**_tmp278;switch(*((int*)_tmp26D)){case 24: _LL24C: _tmp278=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp26D)->f1;_LL24D: {
# 1016
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp278;for(0;effs != 0;effs=effs->tl){
void*_tmp26E=(void*)effs->hd;
({void*_tmpC31=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp26E));effs->hd=_tmpC31;});{
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
({struct Cyc_List_List*_tmpC32=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp274,effects);effects=_tmpC32;});
goto _LL25D;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp272)->f1)){case 20: _LL260: _LL261:
 goto _LL263;case 22: _LL262: _LL263:
 goto _LL265;case 21: _LL264: _LL265:
 goto _LL25D;default: goto _LL266;}default: _LL266: _tmp275=_tmp272;_LL267:
({struct Cyc_List_List*_tmpC33=({struct Cyc_List_List*_tmp273=_cycalloc(sizeof(*_tmp273));_tmp273->hd=_tmp275;_tmp273->tl=effects;_tmp273;});effects=_tmpC33;});goto _LL25D;}_LL25D:;}}
# 1041
({struct Cyc_List_List*_tmpC34=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp278=_tmpC34;});
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
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27A=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27B=_cycalloc(sizeof(*_tmp27B));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC37=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp27C;_tmp27C.tag=9;({struct Cyc_Absyn_FnInfo _tmpC36=({struct Cyc_Absyn_FnInfo _tmp27D;_tmp27D.tvars=0;_tmp27D.effect=eff;({struct Cyc_Absyn_Tqual _tmpC35=
Cyc_Absyn_empty_tqual(0);_tmp27D.ret_tqual=_tmpC35;});_tmp27D.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp27D.args=0;_tmp27D.c_varargs=0;_tmp27D.cyc_varargs=0;_tmp27D.rgn_po=0;_tmp27D.attributes=0;_tmp27D.requires_clause=0;_tmp27D.requires_relns=0;_tmp27D.ensures_clause=0;_tmp27D.ensures_relns=0;_tmp27D;});_tmp27C.f1=_tmpC36;});_tmp27C;});_tmp27B[0]=_tmpC37;});_tmp27B;});
# 1064
return({void*_tmpC3A=(void*)_tmp27A;struct Cyc_Absyn_Tqual _tmpC39=Cyc_Absyn_empty_tqual(0);void*_tmpC38=Cyc_Absyn_bounds_one;Cyc_Absyn_atb_typ(_tmpC3A,(void*)& Cyc_Absyn_HeapRgn_val,_tmpC39,_tmpC38,Cyc_Absyn_false_conref);});}
# 1071
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
({void*_tmpC3B=Cyc_Tcutil_compress(r);r=_tmpC3B;});
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp27E=Cyc_Tcutil_compress(e);void*_tmp27F=_tmp27E;struct Cyc_Core_Opt*_tmp2A4;void**_tmp2A3;struct Cyc_Core_Opt*_tmp2A2;void*_tmp2A1;struct Cyc_List_List*_tmp2A0;void*_tmp29F;switch(*((int*)_tmp27F)){case 23: _LL270: _tmp29F=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp27F)->f1;_LL271:
# 1078
 if(constrain)return Cyc_Tcutil_unify(r,_tmp29F);
({void*_tmpC3C=Cyc_Tcutil_compress(_tmp29F);_tmp29F=_tmpC3C;});
if(r == _tmp29F)return 1;{
struct _tuple0 _tmp280=({struct _tuple0 _tmp284;_tmp284.f1=r;_tmp284.f2=_tmp29F;_tmp284;});struct _tuple0 _tmp281=_tmp280;struct Cyc_Absyn_Tvar*_tmp283;struct Cyc_Absyn_Tvar*_tmp282;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f2)->tag == 2){_LL27B: _tmp283=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f1)->f1;_tmp282=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp281.f2)->f1;_LL27C:
 return({Cyc_Absyn_tvar_cmp(_tmp283,_tmp282)== 0;});}else{goto _LL27D;}}else{_LL27D: _LL27E:
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
void*_tmp28A=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp28B=_cycalloc(sizeof(*_tmp28B));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC40=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp28C;_tmp28C.tag=24;({struct Cyc_List_List*_tmpC3F=({void*_tmp28D[2];({void*_tmpC3E=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp28E=_cycalloc(sizeof(*_tmp28E));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC3D=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp28F;_tmp28F.tag=23;_tmp28F.f1=r;_tmp28F;});_tmp28E[0]=_tmpC3D;});_tmp28E;});_tmp28D[1]=_tmpC3E;});_tmp28D[0]=_tmp289;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp28D,sizeof(void*),2));});_tmp28C.f1=_tmpC3F;});_tmp28C;});_tmp28B[0]=_tmpC40;});_tmp28B;}));
*_tmp291=_tmp28A;
return 1;};}}else{_LL287: _LL288:
 return 0;}_LL284:;};}else{_LL282: _tmp294=_tmp286;_LL283:
# 1106
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp294);}_LL27F:;}case 1: _LL276: _tmp2A4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f1;_tmp2A3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f2;_tmp2A2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp27F)->f4;_LL277:
# 1109
 if(_tmp2A4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2A4->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp295=0;({struct _dyneither_ptr _tmpC41=({const char*_tmp296="effect evar has wrong kind";_tag_dyneither(_tmp296,sizeof(char),27);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC41,_tag_dyneither(_tmp295,sizeof(void*),0));});});
if(!constrain)return 0;{
# 1114
void*_tmp297=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A2);
# 1117
Cyc_Tcutil_occurs(_tmp297,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A2))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp298=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp299=_cycalloc(sizeof(*_tmp299));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC46=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp29A;_tmp29A.tag=24;({struct Cyc_List_List*_tmpC45=({struct Cyc_List_List*_tmp29B=_cycalloc(sizeof(*_tmp29B));_tmp29B->hd=_tmp297;({struct Cyc_List_List*_tmpC44=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*_tmp29C));({void*_tmpC43=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp29D=_cycalloc(sizeof(*_tmp29D));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC42=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp29E;_tmp29E.tag=23;_tmp29E.f1=r;_tmp29E;});_tmp29D[0]=_tmpC42;});_tmp29D;});_tmp29C->hd=_tmpC43;});_tmp29C->tl=0;_tmp29C;});_tmp29B->tl=_tmpC44;});_tmp29B;});_tmp29A.f1=_tmpC45;});_tmp29A;});_tmp299[0]=_tmpC46;});_tmp299;});
*_tmp2A3=(void*)_tmp298;
return 1;};};default: _LL278: _LL279:
 return 0;}_LL26F:;};}
# 1128
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
({void*_tmpC47=Cyc_Tcutil_compress(t);t=_tmpC47;});{
void*_tmp2A5=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2A6=_tmp2A5;struct Cyc_Core_Opt*_tmp2BE;void**_tmp2BD;struct Cyc_Core_Opt*_tmp2BC;void*_tmp2BB;struct Cyc_List_List*_tmp2BA;switch(*((int*)_tmp2A6)){case 23: _LL28A: _LL28B:
 return 0;case 24: _LL28C: _tmp2BA=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2A6)->f1;_LL28D:
# 1133
 for(0;_tmp2BA != 0;_tmp2BA=_tmp2BA->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2BA->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2BB=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A6)->f1;_LL28F:
# 1138
({void*_tmpC48=Cyc_Tcutil_compress(_tmp2BB);_tmp2BB=_tmpC48;});
if(t == _tmp2BB)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2BB);{
void*_tmp2A7=Cyc_Tcutil_rgns_of(t);void*_tmp2A8=_tmp2A7;void*_tmp2AF;void*_tmp2AE;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A8)->tag == 25){_LL295: _tmp2AE=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A8)->f1;_LL296: {
# 1143
struct _tuple0 _tmp2A9=({struct _tuple0 _tmp2AD;({void*_tmpC49=Cyc_Tcutil_compress(_tmp2AE);_tmp2AD.f1=_tmpC49;});_tmp2AD.f2=_tmp2BB;_tmp2AD;});struct _tuple0 _tmp2AA=_tmp2A9;struct Cyc_Absyn_Tvar*_tmp2AC;struct Cyc_Absyn_Tvar*_tmp2AB;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f2)->tag == 2){_LL29A: _tmp2AC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f1)->f1;_tmp2AB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AA.f2)->f1;_LL29B:
 return Cyc_Tcutil_unify(t,_tmp2BB);}else{goto _LL29C;}}else{_LL29C: _LL29D:
 return _tmp2AE == _tmp2BB;}_LL299:;}}else{_LL297: _tmp2AF=_tmp2A8;_LL298:
# 1147
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2AF);}_LL294:;};case 1: _LL290: _tmp2BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f1;_tmp2BD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f2;_tmp2BC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A6)->f4;_LL291:
# 1150
 if(_tmp2BE == 0  || ((struct Cyc_Absyn_Kind*)_tmp2BE->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2B0=0;({struct _dyneither_ptr _tmpC4A=({const char*_tmp2B1="effect evar has wrong kind";_tag_dyneither(_tmp2B1,sizeof(char),27);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC4A,_tag_dyneither(_tmp2B0,sizeof(void*),0));});});
if(!may_constrain_evars)return 0;{
# 1155
void*_tmp2B2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2BC);
# 1158
Cyc_Tcutil_occurs(_tmp2B2,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BC))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B3=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B4=_cycalloc(sizeof(*_tmp2B4));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC4F=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2B5;_tmp2B5.tag=24;({struct Cyc_List_List*_tmpC4E=({struct Cyc_List_List*_tmp2B6=_cycalloc(sizeof(*_tmp2B6));_tmp2B6->hd=_tmp2B2;({struct Cyc_List_List*_tmpC4D=({struct Cyc_List_List*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));({void*_tmpC4C=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC4B=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2B9;_tmp2B9.tag=25;_tmp2B9.f1=t;_tmp2B9;});_tmp2B8[0]=_tmpC4B;});_tmp2B8;});_tmp2B7->hd=_tmpC4C;});_tmp2B7->tl=0;_tmp2B7;});_tmp2B6->tl=_tmpC4D;});_tmp2B6;});_tmp2B5.f1=_tmpC4E;});_tmp2B5;});_tmp2B4[0]=_tmpC4F;});_tmp2B4;});
*_tmp2BD=(void*)_tmp2B3;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1169
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
({void*_tmpC50=Cyc_Tcutil_compress(e);e=_tmpC50;});{
void*_tmp2BF=e;struct Cyc_Core_Opt*_tmp2DF;void**_tmp2DE;struct Cyc_Core_Opt*_tmp2DD;void*_tmp2DC;struct Cyc_List_List*_tmp2DB;struct Cyc_Absyn_Tvar*_tmp2DA;switch(*((int*)_tmp2BF)){case 2: _LL29F: _tmp2DA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BF)->f1;_LL2A0:
 return({Cyc_Absyn_tvar_cmp(v,_tmp2DA)== 0;});case 24: _LL2A1: _tmp2DB=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2BF)->f1;_LL2A2:
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
void*_tmp2C5=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC54=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2C7;_tmp2C7.tag=24;({struct Cyc_List_List*_tmpC53=({void*_tmp2C8[2];({void*_tmpC52=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2C9=_cycalloc(sizeof(*_tmp2C9));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC51=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2CA;_tmp2CA.tag=2;_tmp2CA.f1=v;_tmp2CA;});_tmp2C9[0]=_tmpC51;});_tmp2C9;});_tmp2C8[1]=_tmpC52;});_tmp2C8[0]=_tmp2C4;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2C8,sizeof(void*),2));});_tmp2C7.f1=_tmpC53;});_tmp2C7;});_tmp2C6[0]=_tmpC54;});_tmp2C6;}));
*_tmp2CC=_tmp2C5;
return 1;};}}else{_LL2B1: _LL2B2:
 return 0;}_LL2AE:;};}else{_LL2AC: _tmp2CF=_tmp2C1;_LL2AD:
# 1195
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2CF);}_LL2A9:;}case 1: _LL2A5: _tmp2DF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f1;_tmp2DE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f2;_tmp2DD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2BF)->f4;_LL2A6:
# 1198
 if(_tmp2DF == 0  || ((struct Cyc_Absyn_Kind*)_tmp2DF->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2D0=0;({struct _dyneither_ptr _tmpC55=({const char*_tmp2D1="effect evar has wrong kind";_tag_dyneither(_tmp2D1,sizeof(char),27);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC55,_tag_dyneither(_tmp2D0,sizeof(void*),0));});});{
# 1202
void*_tmp2D2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2DD);
# 1204
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DD))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D3=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC5A=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2D5;_tmp2D5.tag=24;({struct Cyc_List_List*_tmpC59=({struct Cyc_List_List*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->hd=_tmp2D2;({struct Cyc_List_List*_tmpC58=({struct Cyc_List_List*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));({void*_tmpC57=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC56=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2D9;_tmp2D9.tag=2;_tmp2D9.f1=v;_tmp2D9;});_tmp2D8[0]=_tmpC56;});_tmp2D8;});_tmp2D7->hd=_tmpC57;});_tmp2D7->tl=0;_tmp2D7;});_tmp2D6->tl=_tmpC58;});_tmp2D6;});_tmp2D5.f1=_tmpC59;});_tmp2D5;});_tmp2D4[0]=_tmpC5A;});_tmp2D4;});
*_tmp2DE=(void*)_tmp2D3;
return 1;};};default: _LL2A7: _LL2A8:
 return 0;}_LL29E:;};}
# 1214
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
({void*_tmpC5B=Cyc_Tcutil_compress(e);e=_tmpC5B;});{
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
({struct Cyc_String_pa_PrintArg_struct _tmp2EF;_tmp2EF.tag=0;({struct _dyneither_ptr _tmpC5C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmp2EF.f1=_tmpC5C;});({void*_tmp2ED[1]={& _tmp2EF};({struct _dyneither_ptr _tmpC5D=({const char*_tmp2EE="subset_effect: bad effect: %s";_tag_dyneither(_tmp2EE,sizeof(char),30);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5D,_tag_dyneither(_tmp2ED,sizeof(void*),1));});});});}_LL2C1:;}
# 1298 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
({void*_tmpC5E=Cyc_Tcutil_normalize_effect(e1);e1=_tmpC5E;});
({void*_tmpC5F=Cyc_Tcutil_normalize_effect(e2);e2=_tmpC5F;});
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
({struct _dyneither_ptr _tmpC60=_tag_dyneither(0,0,0);Cyc_Tcutil_failure_reason=_tmpC60;});
({void*_tmpC61=Cyc_Tcutil_compress(t1);t1=_tmpC61;});
({void*_tmpC62=Cyc_Tcutil_compress(t2);t2=_tmpC62;});
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
({struct _dyneither_ptr _tmpC63=({const char*_tmp305="(type variable would escape scope)";_tag_dyneither(_tmp305,sizeof(char),35);});Cyc_Tcutil_failure_reason=_tmpC63;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1388
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp310->v,_tmp302)){
*_tmp30B=t1;return;}
# 1391
({struct _dyneither_ptr _tmpC64=({const char*_tmp306="(kinds are incompatible)";_tag_dyneither(_tmp306,sizeof(char),25);});Cyc_Tcutil_failure_reason=_tmpC64;});
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
({struct _dyneither_ptr _tmpC65=({const char*_tmp30D="(kinds are incompatible)";_tag_dyneither(_tmp30D,sizeof(char),25);});Cyc_Tcutil_failure_reason=_tmpC65;});
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
({struct _dyneither_ptr _tmpC66=({const char*_tmp317="(variable types are not the same)";_tag_dyneither(_tmp317,sizeof(char),34);});Cyc_Tcutil_failure_reason=_tmpC66;});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 11: switch(*((int*)_tmp312.f2)){case 11: _LL2F1: _tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f1)->f1).aggr_info;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp37D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f2)->f1).aggr_info;_tmp37C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2F2: {
# 1436
struct _tuple13 _tmp318=Cyc_Absyn_aggr_kinded_name(_tmp37D);struct _tuple13 _tmp319=_tmp318;enum Cyc_Absyn_AggrKind _tmp321;struct _tuple2*_tmp320;_LL328: _tmp321=_tmp319.f1;_tmp320=_tmp319.f2;_LL329:;{
struct _tuple13 _tmp31A=Cyc_Absyn_aggr_kinded_name(_tmp37F);struct _tuple13 _tmp31B=_tmp31A;enum Cyc_Absyn_AggrKind _tmp31F;struct _tuple2*_tmp31E;_LL32B: _tmp31F=_tmp31B.f1;_tmp31E=_tmp31B.f2;_LL32C:;
if(_tmp321 != _tmp31F){({struct _dyneither_ptr _tmpC67=({const char*_tmp31C="(struct and union type)";_tag_dyneither(_tmp31C,sizeof(char),24);});Cyc_Tcutil_failure_reason=_tmpC67;});goto _LL2EA;}
if(({Cyc_Absyn_qvar_cmp(_tmp320,_tmp31E)!= 0;})){({struct _dyneither_ptr _tmpC68=({const char*_tmp31D="(different type name)";_tag_dyneither(_tmp31D,sizeof(char),22);});Cyc_Tcutil_failure_reason=_tmpC68;});goto _LL2EA;}
Cyc_Tcutil_unify_list(_tmp37C,_tmp37E);
return;};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 13: switch(*((int*)_tmp312.f2)){case 13: _LL2F3: _tmp381=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp380=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL2F4:
# 1445
 if(({Cyc_Absyn_qvar_cmp(_tmp381,_tmp380)== 0;}))return;
({struct _dyneither_ptr _tmpC69=({const char*_tmp322="(different enum types)";_tag_dyneither(_tmp322,sizeof(char),23);});Cyc_Tcutil_failure_reason=_tmpC69;});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 14: switch(*((int*)_tmp312.f2)){case 14: _LL2F5: _tmp383=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp382=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp312.f2)->f1;_LL2F6: {
# 1450
int bad=0;
for(0;_tmp383 != 0  && _tmp382 != 0;(_tmp383=_tmp383->tl,_tmp382=_tmp382->tl)){
struct Cyc_Absyn_Enumfield*_tmp323=(struct Cyc_Absyn_Enumfield*)_tmp383->hd;
struct Cyc_Absyn_Enumfield*_tmp324=(struct Cyc_Absyn_Enumfield*)_tmp382->hd;
if(({Cyc_Absyn_qvar_cmp(_tmp323->name,_tmp324->name)!= 0;})){
({struct _dyneither_ptr _tmpC6A=({const char*_tmp325="(different names for enum fields)";_tag_dyneither(_tmp325,sizeof(char),34);});Cyc_Tcutil_failure_reason=_tmpC6A;});
bad=1;
break;}
# 1459
if(_tmp323->tag == _tmp324->tag)continue;
if(_tmp323->tag == 0  || _tmp324->tag == 0){
({struct _dyneither_ptr _tmpC6B=({const char*_tmp326="(different tag values for enum fields)";_tag_dyneither(_tmp326,sizeof(char),39);});Cyc_Tcutil_failure_reason=_tmpC6B;});
bad=1;
break;}
# 1465
if(!({struct Cyc_Absyn_Exp*_tmpC6C=(struct Cyc_Absyn_Exp*)_check_null(_tmp323->tag);Cyc_Evexp_same_const_exp(_tmpC6C,(struct Cyc_Absyn_Exp*)_check_null(_tmp324->tag));})){
({struct _dyneither_ptr _tmpC6D=({const char*_tmp327="(different tag values for enum fields)";_tag_dyneither(_tmp327,sizeof(char),39);});Cyc_Tcutil_failure_reason=_tmpC6D;});
bad=1;
break;}}
# 1471
if(bad)goto _LL2EA;
if(_tmp383 == 0  && _tmp382 == 0)return;
({struct _dyneither_ptr _tmpC6E=({const char*_tmp328="(different number of fields for enums)";_tag_dyneither(_tmp328,sizeof(char),39);});Cyc_Tcutil_failure_reason=_tmpC6E;});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp312.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2F7: _tmp387=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).datatype_info).KnownDatatype).val;_tmp386=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp385=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).datatype_info).KnownDatatype).val;_tmp384=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2F8:
# 1478
 if(_tmp387 == _tmp385  || ({Cyc_Absyn_qvar_cmp(_tmp387->name,_tmp385->name)== 0;})){
Cyc_Tcutil_unify_list(_tmp384,_tmp386);
return;}
# 1482
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp312.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp312.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2F9: _tmp38D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp38C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp38B=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f1)->f1).targs;_tmp38A=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp389=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp388=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp312.f2)->f1).targs;_LL2FA:
# 1486
 if((_tmp38D == _tmp38A  || ({Cyc_Absyn_qvar_cmp(_tmp38D->name,_tmp38A->name)== 0;})) && (
_tmp38C == _tmp389  || ({Cyc_Absyn_qvar_cmp(_tmp38C->name,_tmp389->name)== 0;}))){
Cyc_Tcutil_unify_list(_tmp388,_tmp38B);
return;}
# 1491
({struct _dyneither_ptr _tmpC6F=({const char*_tmp329="(different datatype field types)";_tag_dyneither(_tmp329,sizeof(char),33);});Cyc_Tcutil_failure_reason=_tmpC6F;});
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp312.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 5: switch(*((int*)_tmp312.f2)){case 5: _LL2FB: _tmp399=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_typ;_tmp398=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_tq;_tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).rgn;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).nullable;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).bounds;_tmp394=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f1)->f1).ptr_atts).zero_term;_tmp393=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_typ;_tmp392=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_tq;_tmp391=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).rgn;_tmp390=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).nullable;_tmp38F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).bounds;_tmp38E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312.f2)->f1).ptr_atts).zero_term;_LL2FC:
# 1496
 Cyc_Tcutil_unify_it(_tmp393,_tmp399);
Cyc_Tcutil_unify_it(_tmp397,_tmp391);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpC71=_tmp38E;union Cyc_Absyn_Constraint*_tmpC70=_tmp394;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC71,_tmpC70,({const char*_tmp32A="(not both zero terminated)";_tag_dyneither(_tmp32A,sizeof(char),27);}));});
Cyc_Tcutil_unify_tqual(_tmp392,_tmp393,_tmp398,_tmp399);
({union Cyc_Absyn_Constraint*_tmpC73=_tmp38F;union Cyc_Absyn_Constraint*_tmpC72=_tmp395;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmpC73,_tmpC72,({const char*_tmp32B="(different pointer bounds)";_tag_dyneither(_tmp32B,sizeof(char),27);}));});
# 1504
{void*_tmp32C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp38F);void*_tmp32D=_tmp32C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp32D)->tag == 0){_LL32E: _LL32F:
 return;}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;}
# 1508
({union Cyc_Absyn_Constraint*_tmpC75=_tmp390;union Cyc_Absyn_Constraint*_tmpC74=_tmp396;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC75,_tmpC74,({const char*_tmp32E="(incompatible pointer types)";_tag_dyneither(_tmp32E,sizeof(char),29);}));});
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 6: switch(*((int*)_tmp312.f2)){case 6: _LL2FD: _tmp39D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp39C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp39B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f2)->f1;_tmp39A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp312.f2)->f2;_LL2FE:
# 1513
 if(_tmp39B == _tmp39D  && ((_tmp39A == _tmp39C  || _tmp39A == Cyc_Absyn_Int_sz  && _tmp39C == Cyc_Absyn_Long_sz) || 
_tmp39A == Cyc_Absyn_Long_sz  && _tmp39C == Cyc_Absyn_Int_sz))return;
({struct _dyneither_ptr _tmpC76=({const char*_tmp32F="(different integral types)";_tag_dyneither(_tmp32F,sizeof(char),27);});Cyc_Tcutil_failure_reason=_tmpC76;});
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
({struct _dyneither_ptr _tmpC77=({const char*_tmp330="(cannot prove expressions are the same)";_tag_dyneither(_tmp330,sizeof(char),40);});Cyc_Tcutil_failure_reason=_tmpC77;});
goto _LL2EA;}
# 1532
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 8: switch(*((int*)_tmp312.f2)){case 8: _LL305: _tmp3AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).elt_type;_tmp3AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).tq;_tmp3A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).num_elts;_tmp3A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f1)->f1).zero_term;_tmp3A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).elt_type;_tmp3A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).tq;_tmp3A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).num_elts;_tmp3A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp312.f2)->f1).zero_term;_LL306:
# 1536
 Cyc_Tcutil_unify_it(_tmp3A7,_tmp3AB);
Cyc_Tcutil_unify_tqual(_tmp3A6,_tmp3A7,_tmp3AA,_tmp3AB);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpC79=_tmp3A8;union Cyc_Absyn_Constraint*_tmpC78=_tmp3A4;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC79,_tmpC78,({const char*_tmp331="(not both zero terminated)";_tag_dyneither(_tmp331,sizeof(char),27);}));});
if(_tmp3A9 == _tmp3A5)return;
if(_tmp3A9 == 0  || _tmp3A5 == 0)goto _LL2EA;
if(Cyc_Evexp_same_const_exp(_tmp3A9,_tmp3A5))
return;
({struct _dyneither_ptr _tmpC7A=({const char*_tmp332="(different array sizes)";_tag_dyneither(_tmp332,sizeof(char),24);});Cyc_Tcutil_failure_reason=_tmpC7A;});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 9: switch(*((int*)_tmp312.f2)){case 9: _LL307: _tmp3C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).tvars;_tmp3C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).effect;_tmp3C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).ret_tqual;_tmp3C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).ret_typ;_tmp3C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).args;_tmp3C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).c_varargs;_tmp3BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).cyc_varargs;_tmp3BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).rgn_po;_tmp3BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).attributes;_tmp3BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).requires_clause;_tmp3BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).requires_relns;_tmp3BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).ensures_clause;_tmp3B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f1)->f1).ensures_relns;_tmp3B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).tvars;_tmp3B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).effect;_tmp3B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).ret_tqual;_tmp3B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).ret_typ;_tmp3B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).args;_tmp3B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).c_varargs;_tmp3B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).cyc_varargs;_tmp3B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).rgn_po;_tmp3B0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).attributes;_tmp3AF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).requires_clause;_tmp3AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).requires_relns;_tmp3AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).ensures_clause;_tmp3AC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp312.f2)->f1).ensures_relns;_LL308: {
# 1550
int done=0;
{struct _RegionHandle _tmp333=_new_region("rgn");struct _RegionHandle*rgn=& _tmp333;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp3B8 != 0){
if(_tmp3C5 == 0){
({struct _dyneither_ptr _tmpC7B=({const char*_tmp334="(second function type has too few type variables)";_tag_dyneither(_tmp334,sizeof(char),50);});Cyc_Tcutil_failure_reason=_tmpC7B;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1558
void*_tmp335=((struct Cyc_Absyn_Tvar*)_tmp3B8->hd)->kind;
void*_tmp336=((struct Cyc_Absyn_Tvar*)_tmp3C5->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp335,_tmp336)){
({struct _dyneither_ptr _tmpC80=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33B;_tmp33B.tag=0;({struct _dyneither_ptr _tmpC7C=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1564
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3C5->hd,& Cyc_Tcutil_bk)));_tmp33B.f1=_tmpC7C;});({struct Cyc_String_pa_PrintArg_struct _tmp33A;_tmp33A.tag=0;({struct _dyneither_ptr _tmpC7D=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1563
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3B8->hd,& Cyc_Tcutil_bk)));_tmp33A.f1=_tmpC7D;});({struct Cyc_String_pa_PrintArg_struct _tmp339;_tmp339.tag=0;({struct _dyneither_ptr _tmpC7E=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1562
Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp3B8->hd));_tmp339.f1=_tmpC7E;});({void*_tmp337[3]={& _tmp339,& _tmp33A,& _tmp33B};({struct _dyneither_ptr _tmpC7F=({const char*_tmp338="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp338,sizeof(char),44);});Cyc_aprintf(_tmpC7F,_tag_dyneither(_tmp337,sizeof(void*),3));});});});});});
# 1561
Cyc_Tcutil_failure_reason=_tmpC80;});
# 1565
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1567
({struct Cyc_List_List*_tmpC84=({struct Cyc_List_List*_tmp33C=_region_malloc(rgn,sizeof(*_tmp33C));({struct _tuple16*_tmpC83=({struct _tuple16*_tmp33D=_region_malloc(rgn,sizeof(*_tmp33D));_tmp33D->f1=(struct Cyc_Absyn_Tvar*)_tmp3C5->hd;({void*_tmpC82=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33E=_cycalloc(sizeof(*_tmp33E));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC81=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp33F;_tmp33F.tag=2;_tmp33F.f1=(struct Cyc_Absyn_Tvar*)_tmp3B8->hd;_tmp33F;});_tmp33E[0]=_tmpC81;});_tmp33E;});_tmp33D->f2=_tmpC82;});_tmp33D;});_tmp33C->hd=_tmpC83;});_tmp33C->tl=inst;_tmp33C;});inst=_tmpC84;});
_tmp3B8=_tmp3B8->tl;
_tmp3C5=_tmp3C5->tl;};}
# 1571
if(_tmp3C5 != 0){
({struct _dyneither_ptr _tmpC85=({const char*_tmp340="(second function type has too many type variables)";_tag_dyneither(_tmp340,sizeof(char),51);});Cyc_Tcutil_failure_reason=_tmpC85;});
_npop_handler(0);goto _LL2EA;}
# 1575
if(inst != 0){
({void*_tmpC8C=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp341=_cycalloc(sizeof(*_tmp341));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC87=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp342;_tmp342.tag=9;({struct Cyc_Absyn_FnInfo _tmpC86=({struct Cyc_Absyn_FnInfo _tmp343;_tmp343.tvars=0;_tmp343.effect=_tmp3B7;_tmp343.ret_tqual=_tmp3B6;_tmp343.ret_typ=_tmp3B5;_tmp343.args=_tmp3B4;_tmp343.c_varargs=_tmp3B3;_tmp343.cyc_varargs=_tmp3B2;_tmp343.rgn_po=_tmp3B1;_tmp343.attributes=_tmp3B0;_tmp343.requires_clause=_tmp3BC;_tmp343.requires_relns=_tmp3BB;_tmp343.ensures_clause=_tmp3BA;_tmp343.ensures_relns=_tmp3B9;_tmp343;});_tmp342.f1=_tmpC86;});_tmp342;});_tmp341[0]=_tmpC87;});_tmp341;});Cyc_Tcutil_unify_it(_tmpC8C,({
# 1579
struct _RegionHandle*_tmpC8B=rgn;struct Cyc_List_List*_tmpC8A=inst;Cyc_Tcutil_rsubstitute(_tmpC8B,_tmpC8A,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp344=_cycalloc(sizeof(*_tmp344));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC89=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp345;_tmp345.tag=9;({struct Cyc_Absyn_FnInfo _tmpC88=({struct Cyc_Absyn_FnInfo _tmp346;_tmp346.tvars=0;_tmp346.effect=_tmp3C4;_tmp346.ret_tqual=_tmp3C3;_tmp346.ret_typ=_tmp3C2;_tmp346.args=_tmp3C1;_tmp346.c_varargs=_tmp3C0;_tmp346.cyc_varargs=_tmp3BF;_tmp346.rgn_po=_tmp3BE;_tmp346.attributes=_tmp3BD;_tmp346.requires_clause=_tmp3AF;_tmp346.requires_relns=_tmp3AE;_tmp346.ensures_clause=_tmp3AD;_tmp346.ensures_relns=_tmp3AC;_tmp346;});_tmp345.f1=_tmpC88;});_tmp345;});_tmp344[0]=_tmpC89;});_tmp344;}));}));});
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
({struct _dyneither_ptr _tmpC8D=({const char*_tmp34F="(function types have different number of arguments)";_tag_dyneither(_tmp34F,sizeof(char),52);});Cyc_Tcutil_failure_reason=_tmpC8D;});
goto _LL2EA;}
# 1603
if(_tmp3B3 != _tmp3C0){
({struct _dyneither_ptr _tmpC8E=({const char*_tmp350="(only one function type takes C varargs)";_tag_dyneither(_tmp350,sizeof(char),41);});Cyc_Tcutil_failure_reason=_tmpC8E;});
goto _LL2EA;}{
# 1608
int bad_cyc_vararg=0;
{struct _tuple22 _tmp351=({struct _tuple22 _tmp35D;_tmp35D.f1=_tmp3B2;_tmp35D.f2=_tmp3BF;_tmp35D;});struct _tuple22 _tmp352=_tmp351;struct _dyneither_ptr*_tmp35C;struct Cyc_Absyn_Tqual _tmp35B;void*_tmp35A;int _tmp359;struct _dyneither_ptr*_tmp358;struct Cyc_Absyn_Tqual _tmp357;void*_tmp356;int _tmp355;if(_tmp352.f1 == 0){if(_tmp352.f2 == 0){_LL339: _LL33A:
 goto _LL338;}else{_LL33B: _LL33C:
 goto _LL33E;}}else{if(_tmp352.f2 == 0){_LL33D: _LL33E:
# 1613
 bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC8F=({const char*_tmp353="(only one function type takes varargs)";_tag_dyneither(_tmp353,sizeof(char),39);});Cyc_Tcutil_failure_reason=_tmpC8F;});
goto _LL338;}else{_LL33F: _tmp35C=(_tmp352.f1)->name;_tmp35B=(_tmp352.f1)->tq;_tmp35A=(_tmp352.f1)->type;_tmp359=(_tmp352.f1)->inject;_tmp358=(_tmp352.f2)->name;_tmp357=(_tmp352.f2)->tq;_tmp356=(_tmp352.f2)->type;_tmp355=(_tmp352.f2)->inject;_LL340:
# 1617
 Cyc_Tcutil_unify_it(_tmp35A,_tmp356);
Cyc_Tcutil_unify_tqual(_tmp35B,_tmp35A,_tmp357,_tmp356);
if(_tmp359 != _tmp355){
bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC90=({const char*_tmp354="(only one function type injects varargs)";_tag_dyneither(_tmp354,sizeof(char),41);});Cyc_Tcutil_failure_reason=_tmpC90;});}
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
({int _tmpC92=!({void*_tmpC91=(void*)_check_null(_tmp3B7);Cyc_Tcutil_unify_effect(_tmpC91,(void*)_check_null(_tmp3C4));});bad_effect=_tmpC92;});goto _LL341;}}_LL341:;}
# 1635
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
({struct _dyneither_ptr _tmpC93=({const char*_tmp361="(function effects do not match)";_tag_dyneither(_tmp361,sizeof(char),32);});Cyc_Tcutil_failure_reason=_tmpC93;});
goto _LL2EA;}
# 1641
if(!Cyc_Tcutil_same_atts(_tmp3BD,_tmp3B0)){
({struct _dyneither_ptr _tmpC94=({const char*_tmp362="(function types have different attributes)";_tag_dyneither(_tmp362,sizeof(char),43);});Cyc_Tcutil_failure_reason=_tmpC94;});
goto _LL2EA;}
# 1645
if(!Cyc_Tcutil_same_rgn_po(_tmp3BE,_tmp3B1)){
({struct _dyneither_ptr _tmpC95=({const char*_tmp363="(function types have different region lifetime orderings)";_tag_dyneither(_tmp363,sizeof(char),58);});Cyc_Tcutil_failure_reason=_tmpC95;});
goto _LL2EA;}
# 1649
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3BB,_tmp3AE)){
({struct _dyneither_ptr _tmpC96=({const char*_tmp364="(@requires clauses not equivalent)";_tag_dyneither(_tmp364,sizeof(char),35);});Cyc_Tcutil_failure_reason=_tmpC96;});
goto _LL2EA;}
# 1653
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3B9,_tmp3AC)){
({struct _dyneither_ptr _tmpC97=({const char*_tmp365="(@ensures clauses not equivalent)";_tag_dyneither(_tmp365,sizeof(char),34);});Cyc_Tcutil_failure_reason=_tmpC97;});
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
({struct _dyneither_ptr _tmpC98=({const char*_tmp36E="(tuple types have different numbers of components)";_tag_dyneither(_tmp36E,sizeof(char),51);});Cyc_Tcutil_failure_reason=_tmpC98;});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 12: switch(*((int*)_tmp312.f2)){case 12: _LL30B: _tmp3CB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f1)->f1;_tmp3CA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp3C9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f2)->f1;_tmp3C8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp312.f2)->f2;_LL30C:
# 1673
 if(_tmp3C9 != _tmp3CB){({struct _dyneither_ptr _tmpC99=({const char*_tmp36F="(struct and union type)";_tag_dyneither(_tmp36F,sizeof(char),24);});Cyc_Tcutil_failure_reason=_tmpC99;});goto _LL2EA;}
for(0;_tmp3C8 != 0  && _tmp3CA != 0;(_tmp3C8=_tmp3C8->tl,_tmp3CA=_tmp3CA->tl)){
struct Cyc_Absyn_Aggrfield*_tmp370=(struct Cyc_Absyn_Aggrfield*)_tmp3C8->hd;
struct Cyc_Absyn_Aggrfield*_tmp371=(struct Cyc_Absyn_Aggrfield*)_tmp3CA->hd;
if(({Cyc_strptrcmp(_tmp370->name,_tmp371->name)!= 0;})){
({struct _dyneither_ptr _tmpC9A=({const char*_tmp372="(different member names)";_tag_dyneither(_tmp372,sizeof(char),25);});Cyc_Tcutil_failure_reason=_tmpC9A;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1681
Cyc_Tcutil_unify_it(_tmp370->type,_tmp371->type);
Cyc_Tcutil_unify_tqual(_tmp370->tq,_tmp370->type,_tmp371->tq,_tmp371->type);
if(!Cyc_Tcutil_same_atts(_tmp370->attributes,_tmp371->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC9B=({const char*_tmp373="(different attributes on member)";_tag_dyneither(_tmp373,sizeof(char),33);});Cyc_Tcutil_failure_reason=_tmpC9B;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1689
if((_tmp370->width != 0  && _tmp371->width == 0  || 
_tmp371->width != 0  && _tmp370->width == 0) || 
(_tmp370->width != 0  && _tmp371->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpC9C=(struct Cyc_Absyn_Exp*)_check_null(_tmp370->width);Cyc_Evexp_same_const_exp(_tmpC9C,(struct Cyc_Absyn_Exp*)_check_null(_tmp371->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC9D=({const char*_tmp374="(different bitfield widths on member)";_tag_dyneither(_tmp374,sizeof(char),38);});Cyc_Tcutil_failure_reason=_tmpC9D;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1698
if((_tmp370->requires_clause != 0  && _tmp371->requires_clause == 0  || 
_tmp370->requires_clause == 0  && _tmp371->requires_clause != 0) || 
(_tmp370->requires_clause == 0  && _tmp371->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpC9E=(struct Cyc_Absyn_Exp*)_check_null(_tmp370->requires_clause);Cyc_Evexp_same_const_exp(_tmpC9E,(struct Cyc_Absyn_Exp*)_check_null(_tmp371->requires_clause));})){
# 1703
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC9F=({const char*_tmp375="(different @requires clauses on member)";_tag_dyneither(_tmp375,sizeof(char),40);});Cyc_Tcutil_failure_reason=_tmpC9F;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1709
if(_tmp3C8 == 0  && _tmp3CA == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpCA0=({const char*_tmp376="(different number of members)";_tag_dyneither(_tmp376,sizeof(char),30);});Cyc_Tcutil_failure_reason=_tmpCA0;});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 17: switch(*((int*)_tmp312.f2)){case 17: _LL30D: _tmp3CF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f1)->f2;_tmp3CE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f1)->f3;_tmp3CD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f2)->f2;_tmp3CC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp312.f2)->f3;_LL30E:
# 1715
 if(_tmp3CE != _tmp3CC){
({struct _dyneither_ptr _tmpCA1=({const char*_tmp377="(different abstract typedefs)";_tag_dyneither(_tmp377,sizeof(char),30);});Cyc_Tcutil_failure_reason=_tmpCA1;});
goto _LL2EA;}
# 1719
({struct _dyneither_ptr _tmpCA2=({const char*_tmp378="(type parameters to typedef differ)";_tag_dyneither(_tmp378,sizeof(char),36);});Cyc_Tcutil_failure_reason=_tmpCA2;});
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
({struct _dyneither_ptr _tmpCA3=({const char*_tmp379="(effects don't unify)";_tag_dyneither(_tmp379,sizeof(char),22);});Cyc_Tcutil_failure_reason=_tmpCA3;});
goto _LL2EA;default: _LL325: _LL326:
 goto _LL2EA;}}}}_LL2EA:;}
# 1743
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1746
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({void*_tmpCA4=(void*)_check_null(a1);cmp(_tmpCA4,(void*)_check_null(a2));});}
# 1753
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3D7=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3D8=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3D7,_tmp3D8);}
# 1760
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
({union Cyc_Absyn_Constraint*_tmpCA5=Cyc_Absyn_compress_conref(x);x=_tmpCA5;});
({union Cyc_Absyn_Constraint*_tmpCA6=Cyc_Absyn_compress_conref(y);y=_tmpCA6;});
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3D9=x;void*_tmp3E0;switch((((union Cyc_Absyn_Constraint*)_tmp3D9)->Eq_constr).tag){case 3: _LL351: _LL352:
 return - 1;case 1: _LL353: _tmp3E0=(void*)(_tmp3D9->Eq_constr).val;_LL354: {
# 1767
union Cyc_Absyn_Constraint*_tmp3DA=y;void*_tmp3DD;switch((((union Cyc_Absyn_Constraint*)_tmp3DA)->Eq_constr).tag){case 3: _LL358: _LL359:
 return 1;case 1: _LL35A: _tmp3DD=(void*)(_tmp3DA->Eq_constr).val;_LL35B:
 return cmp(_tmp3E0,_tmp3DD);default: _LL35C: _LL35D:
({void*_tmp3DB=0;({struct _dyneither_ptr _tmpCA7=({const char*_tmp3DC="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3DC,sizeof(char),40);});Cyc_Tcutil_impos(_tmpCA7,_tag_dyneither(_tmp3DB,sizeof(void*),0));});});}_LL357:;}default: _LL355: _LL356:
# 1772
({void*_tmp3DE=0;({struct _dyneither_ptr _tmpCA8=({const char*_tmp3DF="unify_conref: forward after compress";_tag_dyneither(_tmp3DF,sizeof(char),37);});Cyc_Tcutil_impos(_tmpCA8,_tag_dyneither(_tmp3DE,sizeof(void*),0));});});}_LL350:;};}
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
({int _tmpCA9=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);_tmp3EB=_tmpCA9;});
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
({void*_tmpCAA=Cyc_Tcutil_compress(t1);t1=_tmpCAA;});
({void*_tmpCAB=Cyc_Tcutil_compress(t2);t2=_tmpCAB;});
if(t1 == t2)return 0;{
int _tmp3EE=({int _tmpCAC=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpCAC,Cyc_Tcutil_type_case_number(t2));});
if(_tmp3EE != 0)
return _tmp3EE;{
# 1850
struct _tuple0 _tmp3EF=({struct _tuple0 _tmp452;_tmp452.f1=t2;_tmp452.f2=t1;_tmp452;});struct _tuple0 _tmp3F0=_tmp3EF;struct _dyneither_ptr _tmp451;struct _dyneither_ptr _tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp44D;struct Cyc_Absyn_Exp*_tmp44C;void*_tmp44B;void*_tmp44A;void*_tmp449;void*_tmp448;void*_tmp447;void*_tmp446;void*_tmp445;void*_tmp444;enum Cyc_Absyn_AggrKind _tmp443;struct Cyc_List_List*_tmp442;enum Cyc_Absyn_AggrKind _tmp441;struct Cyc_List_List*_tmp440;struct Cyc_List_List*_tmp43F;struct Cyc_List_List*_tmp43E;void*_tmp43D;struct Cyc_Absyn_Tqual _tmp43C;struct Cyc_Absyn_Exp*_tmp43B;union Cyc_Absyn_Constraint*_tmp43A;void*_tmp439;struct Cyc_Absyn_Tqual _tmp438;struct Cyc_Absyn_Exp*_tmp437;union Cyc_Absyn_Constraint*_tmp436;int _tmp435;int _tmp434;enum Cyc_Absyn_Sign _tmp433;enum Cyc_Absyn_Size_of _tmp432;enum Cyc_Absyn_Sign _tmp431;enum Cyc_Absyn_Size_of _tmp430;void*_tmp42F;struct Cyc_Absyn_Tqual _tmp42E;void*_tmp42D;union Cyc_Absyn_Constraint*_tmp42C;union Cyc_Absyn_Constraint*_tmp42B;union Cyc_Absyn_Constraint*_tmp42A;void*_tmp429;struct Cyc_Absyn_Tqual _tmp428;void*_tmp427;union Cyc_Absyn_Constraint*_tmp426;union Cyc_Absyn_Constraint*_tmp425;union Cyc_Absyn_Constraint*_tmp424;struct Cyc_Absyn_Datatypedecl*_tmp423;struct Cyc_Absyn_Datatypefield*_tmp422;struct Cyc_List_List*_tmp421;struct Cyc_Absyn_Datatypedecl*_tmp420;struct Cyc_Absyn_Datatypefield*_tmp41F;struct Cyc_List_List*_tmp41E;struct Cyc_Absyn_Datatypedecl*_tmp41D;struct Cyc_List_List*_tmp41C;struct Cyc_Absyn_Datatypedecl*_tmp41B;struct Cyc_List_List*_tmp41A;struct Cyc_List_List*_tmp419;struct Cyc_List_List*_tmp418;struct _tuple2*_tmp417;struct _tuple2*_tmp416;union Cyc_Absyn_AggrInfoU _tmp415;struct Cyc_List_List*_tmp414;union Cyc_Absyn_AggrInfoU _tmp413;struct Cyc_List_List*_tmp412;struct Cyc_Absyn_Tvar*_tmp411;struct Cyc_Absyn_Tvar*_tmp410;switch(*((int*)_tmp3F0.f1)){case 1: switch(*((int*)_tmp3F0.f2)){case 1: _LL3A2: _LL3A3:
# 1852
({void*_tmp3F1=0;({struct _dyneither_ptr _tmpCAD=({const char*_tmp3F2="typecmp: can only compare closed types";_tag_dyneither(_tmp3F2,sizeof(char),39);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCAD,_tag_dyneither(_tmp3F1,sizeof(void*),0));});});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 2: switch(*((int*)_tmp3F0.f2)){case 2: _LL3A4: _tmp411=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp410=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3A5:
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
({void*_tmp407=0;({struct _dyneither_ptr _tmpCAE=({const char*_tmp408="missing expression in array index";_tag_dyneither(_tmp408,sizeof(char),34);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCAE,_tag_dyneither(_tmp407,sizeof(void*),0));});});
# 1929
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp43B,_tmp437);};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 9: switch(*((int*)_tmp3F0.f2)){case 9: _LL3B8: _LL3B9:
# 1932
({void*_tmp409=0;({struct _dyneither_ptr _tmpCAF=({const char*_tmp40A="typecmp: function types not handled";_tag_dyneither(_tmp40A,sizeof(char),36);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCAF,_tag_dyneither(_tmp409,sizeof(void*),0));});});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 10: switch(*((int*)_tmp3F0.f2)){case 10: _LL3BA: _tmp43F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F0.f1)->f1;_tmp43E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F0.f2)->f1;_LL3BB:
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
({void*_tmp40C=0;({struct _dyneither_ptr _tmpCB0=({const char*_tmp40D="typecmp: effects not handled";_tag_dyneither(_tmp40D,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB0,_tag_dyneither(_tmp40C,sizeof(void*),0));});});default: _LL3D6: _LL3D7:
({void*_tmp40E=0;({struct _dyneither_ptr _tmpCB1=({const char*_tmp40F="Unmatched case in typecmp";_tag_dyneither(_tmp40F,sizeof(char),26);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB1,_tag_dyneither(_tmp40E,sizeof(void*),0));});});}}}}_LL3A1:;};};}
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
({void*_tmpCB2=Cyc_Tcutil_compress(t1);t1=_tmpCB2;});
({void*_tmpCB3=Cyc_Tcutil_compress(t2);t2=_tmpCB3;});{
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
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
({struct Cyc_Core_Opt*_tmpCB4=({struct Cyc_Core_Opt*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->v=t1;_tmp45A;});max_arith_type=_tmpCB4;});}}}
# 2019
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2023
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp45E;_tmp45E.tag=0;({struct _dyneither_ptr _tmpCB5=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp45E.f1=_tmpCB5;});({struct Cyc_String_pa_PrintArg_struct _tmp45D;_tmp45D.tag=0;({struct _dyneither_ptr _tmpCB6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp45D.f1=_tmpCB6;});({void*_tmp45B[2]={& _tmp45D,& _tmp45E};({unsigned int _tmpCB8=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpCB7=({const char*_tmp45C="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp45C,sizeof(char),41);});Cyc_Tcutil_terr(_tmpCB8,_tmpCB7,_tag_dyneither(_tmp45B,sizeof(void*),2));});});});});
return 0;}}}
# 2029
return 1;}
# 2034
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp45F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp460=_tmp45F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp460)->tag == 5){_LL412: _LL413:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL411;}else{_LL414: _LL415:
 return 0;}_LL411:;}
# 2040
return 1;}
# 2043
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp461=Cyc_Tcutil_compress(t);void*_tmp462=_tmp461;switch(*((int*)_tmp462)){case 6: _LL417: _LL418:
 goto _LL41A;case 19: _LL419: _LL41A:
 goto _LL41C;case 13: _LL41B: _LL41C:
 goto _LL41E;case 14: _LL41D: _LL41E:
 return 1;default: _LL41F: _LL420:
 return 0;}_LL416:;}
# 2054
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2058
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp463=0;({unsigned int _tmpCBA=e->loc;struct _dyneither_ptr _tmpCB9=({const char*_tmp464="integral size mismatch; conversion supplied";_tag_dyneither(_tmp464,sizeof(char),44);});Cyc_Tcutil_warn(_tmpCBA,_tmpCB9,_tag_dyneither(_tmp463,sizeof(void*),0));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2064
return 0;}
# 2068
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2072
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp465=0;({unsigned int _tmpCBC=e->loc;struct _dyneither_ptr _tmpCBB=({const char*_tmp466="integral size mismatch; conversion supplied";_tag_dyneither(_tmp466,sizeof(char),44);});Cyc_Tcutil_warn(_tmpCBC,_tmpCBB,_tag_dyneither(_tmp465,sizeof(void*),0));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2078
return 0;}
# 2083
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2091
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2093
({void*_tmpCBD=Cyc_Tcutil_compress(t1);t1=_tmpCBD;});
({void*_tmpCBE=Cyc_Tcutil_compress(t2);t2=_tmpCBE;});{
struct _tuple0 _tmp467=({struct _tuple0 _tmp480;_tmp480.f1=t1;_tmp480.f2=t2;_tmp480;});struct _tuple0 _tmp468=_tmp467;void*_tmp47F;struct Cyc_Absyn_Tqual _tmp47E;struct Cyc_Absyn_Exp*_tmp47D;union Cyc_Absyn_Constraint*_tmp47C;void*_tmp47B;struct Cyc_Absyn_Tqual _tmp47A;struct Cyc_Absyn_Exp*_tmp479;union Cyc_Absyn_Constraint*_tmp478;struct Cyc_Absyn_PtrInfo _tmp477;struct Cyc_Absyn_PtrInfo _tmp476;switch(*((int*)_tmp468.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468.f2)->tag == 5){_LL422: _tmp477=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468.f1)->f1;_tmp476=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468.f2)->f1;_LL423: {
# 2097
int okay=1;
# 2099
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp477.ptr_atts).nullable,(_tmp476.ptr_atts).nullable))
# 2102
({int _tmpCBF=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp477.ptr_atts).nullable);okay=_tmpCBF;});
# 2104
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp477.ptr_atts).bounds,(_tmp476.ptr_atts).bounds)){
# 2107
struct _tuple0 _tmp469=({struct _tuple0 _tmp470;({void*_tmpCC0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp477.ptr_atts).bounds);_tmp470.f1=_tmpCC0;});({void*_tmpCC1=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp476.ptr_atts).bounds);_tmp470.f2=_tmpCC1;});_tmp470;});
# 2107
struct _tuple0 _tmp46A=_tmp469;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_Absyn_Exp*_tmp46E;struct Cyc_Absyn_Exp*_tmp46D;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46A.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46A.f2)->tag == 0){_LL42B: _LL42C:
# 2109
 goto _LL42E;}else{_LL42F: _tmp46E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46A.f1)->f1;_tmp46D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46A.f2)->f1;_LL430:
# 2112
({int _tmpCC2=okay  && Cyc_Evexp_lte_const_exp(_tmp46D,_tmp46E);okay=_tmpCC2;});
# 2116
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp476.ptr_atts).zero_term))
({void*_tmp46B=0;({unsigned int _tmpCC4=loc;struct _dyneither_ptr _tmpCC3=({const char*_tmp46C="implicit cast to shorter array";_tag_dyneither(_tmp46C,sizeof(char),31);});Cyc_Tcutil_warn(_tmpCC4,_tmpCC3,_tag_dyneither(_tmp46B,sizeof(void*),0));});});
goto _LL42A;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46A.f2)->tag == 0){_LL42D: _LL42E:
# 2110
 okay=1;goto _LL42A;}else{_LL431: _tmp46F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46A.f2)->f1;_LL432:
# 2121
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp477.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp476.ptr_atts).bounds))
goto _LL42A;
okay=0;
goto _LL42A;}}_LL42A:;}
# 2130
okay=okay  && (!(_tmp477.elt_tq).real_const  || (_tmp476.elt_tq).real_const);
# 2133
if(!Cyc_Tcutil_unify((_tmp477.ptr_atts).rgn,(_tmp476.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp477.ptr_atts).rgn,(_tmp476.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0;({struct _dyneither_ptr _tmpCC5=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2138
Cyc_Absynpp_typ2string((_tmp476.ptr_atts).rgn));_tmp474.f1=_tmpCC5;});({struct Cyc_String_pa_PrintArg_struct _tmp473;_tmp473.tag=0;({struct _dyneither_ptr _tmpCC6=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2137
Cyc_Absynpp_typ2string((_tmp477.ptr_atts).rgn));_tmp473.f1=_tmpCC6;});({void*_tmp471[2]={& _tmp473,& _tmp474};({unsigned int _tmpCC8=loc;struct _dyneither_ptr _tmpCC7=({const char*_tmp472="implicit cast from region %s to region %s";_tag_dyneither(_tmp472,sizeof(char),42);});Cyc_Tcutil_warn(_tmpCC8,_tmpCC7,_tag_dyneither(_tmp471,sizeof(void*),2));});});});});}else{
# 2139
okay=0;}}
# 2143
({int _tmpCC9=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp477.ptr_atts).zero_term,(_tmp476.ptr_atts).zero_term) || 
# 2146
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp477.ptr_atts).zero_term) && (_tmp476.elt_tq).real_const);
# 2143
okay=_tmpCC9;});{
# 2154
int _tmp475=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp476.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2157
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp476.ptr_atts).zero_term);
# 2161
({int _tmpCCA=okay  && (Cyc_Tcutil_unify(_tmp477.elt_typ,_tmp476.elt_typ) || 
(_tmp475  && ((_tmp476.elt_tq).real_const  || ({Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp476.elt_typ));}))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp477.elt_typ,_tmp476.elt_typ));
# 2161
okay=_tmpCCA;});
# 2164
return okay;};}}else{goto _LL428;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f2)->tag == 8){_LL424: _tmp47F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f1)->f1).elt_type;_tmp47E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f1)->f1).tq;_tmp47D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f1)->f1).num_elts;_tmp47C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f1)->f1).zero_term;_tmp47B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f2)->f1).elt_type;_tmp47A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f2)->f1).tq;_tmp479=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f2)->f1).num_elts;_tmp478=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468.f2)->f1).zero_term;_LL425: {
# 2168
int okay;
# 2170
({int _tmpCCB=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp47C,_tmp478) && (
(_tmp47D != 0  && _tmp479 != 0) && Cyc_Evexp_same_const_exp(_tmp47D,_tmp479));
# 2170
okay=_tmpCCB;});
# 2173
return(okay  && Cyc_Tcutil_unify(_tmp47F,_tmp47B)) && (!_tmp47E.real_const  || _tmp47A.real_const);}}else{goto _LL428;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp468.f2)->tag == 6){_LL426: _LL427:
# 2175
 return 0;}else{goto _LL428;}default: _LL428: _LL429:
# 2177
 return Cyc_Tcutil_unify(t1,t2);}_LL421:;};}
# 2181
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp481=Cyc_Tcutil_compress(t);void*_tmp482=_tmp481;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp482)->tag == 5){_LL434: _LL435:
 return 1;}else{_LL436: _LL437:
 return 0;}_LL433:;}
# 2187
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp483=Cyc_Tcutil_compress(t);void*_tmp484=_tmp483;void*_tmp487;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->tag == 5){_LL439: _tmp487=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp484)->f1).elt_typ;_LL43A:
 return _tmp487;}else{_LL43B: _LL43C:
({void*_tmp485=0;({struct _dyneither_ptr _tmpCCC=({const char*_tmp486="pointer_elt_type";_tag_dyneither(_tmp486,sizeof(char),17);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCCC,_tag_dyneither(_tmp485,sizeof(void*),0));});});}_LL438:;}
# 2193
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp488=Cyc_Tcutil_compress(t);void*_tmp489=_tmp488;struct Cyc_Absyn_PtrAtts*_tmp48C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->tag == 5){_LL43E: _tmp48C=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp489)->f1).ptr_atts;_LL43F:
 return _tmp48C->rgn;}else{_LL440: _LL441:
({void*_tmp48A=0;({struct _dyneither_ptr _tmpCCD=({const char*_tmp48B="pointer_elt_type";_tag_dyneither(_tmp48B,sizeof(char),17);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCCD,_tag_dyneither(_tmp48A,sizeof(void*),0));});});}_LL43D:;}
# 2200
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp48D=Cyc_Tcutil_compress(t);void*_tmp48E=_tmp48D;void*_tmp48F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E)->tag == 5){_LL443: _tmp48F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E)->f1).ptr_atts).rgn;_LL444:
# 2203
*rgn=_tmp48F;
return 1;}else{_LL445: _LL446:
 return 0;}_LL442:;}
# 2210
int Cyc_Tcutil_is_boxed(void*t){
return({(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;});}
# 2214
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp490=Cyc_Tcutil_compress(t);void*_tmp491=_tmp490;union Cyc_Absyn_Constraint*_tmp492;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp491)->tag == 5){_LL448: _tmp492=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp491)->f1).ptr_atts).bounds;_LL449:
# 2217
 return({((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp492)== (void*)& Cyc_Absyn_DynEither_b_val;});}else{_LL44A: _LL44B:
 return 0;}_LL447:;}
# 2223
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp493=e->r;void*_tmp494=_tmp493;void*_tmp498;struct Cyc_Absyn_Exp*_tmp497;struct _dyneither_ptr _tmp496;switch(*((int*)_tmp494)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp494)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp494)->f1).Int_c).val).f2 == 0){_LL44D: _LL44E:
 goto _LL450;}else{goto _LL459;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp494)->f1).Char_c).val).f2 == 0){_LL44F: _LL450:
 goto _LL452;}else{goto _LL459;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp494)->f1).Short_c).val).f2 == 0){_LL451: _LL452:
 goto _LL454;}else{goto _LL459;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp494)->f1).LongLong_c).val).f2 == 0){_LL453: _LL454:
# 2229
 return 1;}else{goto _LL459;}case 3: _LL455: _tmp496=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp494)->f1).Wchar_c).val;_LL456: {
# 2231
unsigned long _tmp495=Cyc_strlen((struct _dyneither_ptr)_tmp496);
int i=0;
if(_tmp495 >= 2  && *((const char*)_check_dyneither_subscript(_tmp496,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp496,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp496.curr)[1]== 'x'  && _tmp495 >= 3) && *((const char*)_check_dyneither_subscript(_tmp496,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp495;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp496,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2241
return 0;}}default: goto _LL459;}case 13: _LL457: _tmp498=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp494)->f1;_tmp497=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp494)->f2;_LL458:
 return Cyc_Tcutil_is_zero(_tmp497) && Cyc_Tcutil_admits_zero(_tmp498);default: _LL459: _LL45A:
 return 0;}_LL44C:;}
# 2247
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2254
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2259
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2264
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2271
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2276
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2281
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp499=ka;enum Cyc_Absyn_KindQual _tmp4A2;enum Cyc_Absyn_AliasQual _tmp4A1;_LL45C: _tmp4A2=_tmp499->kind;_tmp4A1=_tmp499->aliasqual;_LL45D:;
{enum Cyc_Absyn_AliasQual _tmp49A=_tmp4A1;switch(_tmp49A){case Cyc_Absyn_Aliasable: _LL45F: _LL460: {
# 2285
enum Cyc_Absyn_KindQual _tmp49B=_tmp4A2;switch(_tmp49B){case Cyc_Absyn_AnyKind: _LL466: _LL467:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL468: _LL469:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL46A: _LL46B:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL46C: _LL46D:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL46E: _LL46F:
 return& Cyc_Tcutil_eko;default: _LL470: _LL471:
 return& Cyc_Tcutil_iko;}_LL465:;}case Cyc_Absyn_Unique: _LL461: _LL462:
# 2294
{enum Cyc_Absyn_KindQual _tmp49C=_tmp4A2;switch(_tmp49C){case Cyc_Absyn_AnyKind: _LL473: _LL474:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL475: _LL476:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL477: _LL478:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL479: _LL47A:
 return& Cyc_Tcutil_urko;default: _LL47B: _LL47C:
 goto _LL472;}_LL472:;}
# 2301
goto _LL45E;default: _LL463: _LL464:
# 2303
{enum Cyc_Absyn_KindQual _tmp49D=_tmp4A2;switch(_tmp49D){case Cyc_Absyn_AnyKind: _LL47E: _LL47F:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL480: _LL481:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL482: _LL483:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL484: _LL485:
 return& Cyc_Tcutil_trko;default: _LL486: _LL487:
 goto _LL47D;}_LL47D:;}
# 2310
goto _LL45E;}_LL45E:;}
# 2312
({struct Cyc_String_pa_PrintArg_struct _tmp4A0;_tmp4A0.tag=0;({struct _dyneither_ptr _tmpCCE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp4A0.f1=_tmpCCE;});({void*_tmp49E[1]={& _tmp4A0};({struct _dyneither_ptr _tmpCCF=({const char*_tmp49F="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp49F,sizeof(char),26);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCCF,_tag_dyneither(_tmp49E,sizeof(void*),1));});});});}
# 2315
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2323
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2330
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2335
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2340
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2345
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2350
struct Cyc_Absyn_Kind*_tmp4A3=ka;enum Cyc_Absyn_KindQual _tmp4AC;enum Cyc_Absyn_AliasQual _tmp4AB;_LL489: _tmp4AC=_tmp4A3->kind;_tmp4AB=_tmp4A3->aliasqual;_LL48A:;
{enum Cyc_Absyn_AliasQual _tmp4A4=_tmp4AB;switch(_tmp4A4){case Cyc_Absyn_Aliasable: _LL48C: _LL48D: {
# 2353
enum Cyc_Absyn_KindQual _tmp4A5=_tmp4AC;switch(_tmp4A5){case Cyc_Absyn_AnyKind: _LL493: _LL494:
 return& ab;case Cyc_Absyn_MemKind: _LL495: _LL496:
 return& mb;case Cyc_Absyn_BoxKind: _LL497: _LL498:
 return& bb;case Cyc_Absyn_RgnKind: _LL499: _LL49A:
 return& rb;case Cyc_Absyn_EffKind: _LL49B: _LL49C:
 return& eb;default: _LL49D: _LL49E:
 return& ib;}_LL492:;}case Cyc_Absyn_Unique: _LL48E: _LL48F:
# 2362
{enum Cyc_Absyn_KindQual _tmp4A6=_tmp4AC;switch(_tmp4A6){case Cyc_Absyn_AnyKind: _LL4A0: _LL4A1:
 return& uab;case Cyc_Absyn_MemKind: _LL4A2: _LL4A3:
 return& umb;case Cyc_Absyn_BoxKind: _LL4A4: _LL4A5:
 return& ubb;case Cyc_Absyn_RgnKind: _LL4A6: _LL4A7:
 return& urb;default: _LL4A8: _LL4A9:
 goto _LL49F;}_LL49F:;}
# 2369
goto _LL48B;default: _LL490: _LL491:
# 2371
{enum Cyc_Absyn_KindQual _tmp4A7=_tmp4AC;switch(_tmp4A7){case Cyc_Absyn_AnyKind: _LL4AB: _LL4AC:
 return& tab;case Cyc_Absyn_MemKind: _LL4AD: _LL4AE:
 return& tmb;case Cyc_Absyn_BoxKind: _LL4AF: _LL4B0:
 return& tbb;case Cyc_Absyn_RgnKind: _LL4B1: _LL4B2:
 return& trb;default: _LL4B3: _LL4B4:
 goto _LL4AA;}_LL4AA:;}
# 2378
goto _LL48B;}_LL48B:;}
# 2380
({struct Cyc_String_pa_PrintArg_struct _tmp4AA;_tmp4AA.tag=0;({struct _dyneither_ptr _tmpCD0=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp4AA.f1=_tmpCD0;});({void*_tmp4A8[1]={& _tmp4AA};({struct _dyneither_ptr _tmpCD1=({const char*_tmp4A9="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4A9,sizeof(char),24);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD1,_tag_dyneither(_tmp4A8,sizeof(void*),1));});});});}
# 2383
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2386
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2388
return({struct Cyc_Core_Opt*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));({void*_tmpCD2=Cyc_Tcutil_kind_to_bound(k);_tmp4BB->v=_tmpCD2;});_tmp4BB;});}
# 2393
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpCD4=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpCD3=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4BD;_tmp4BD.tag=0;_tmp4BD.f1=Cyc_Absyn_Null_c;_tmp4BD;});_tmp4BC[0]=_tmpCD3;});_tmp4BC;});e1->r=_tmpCD4;});{
struct Cyc_Core_Opt*_tmp4BE=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4BF=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C4=_cycalloc(sizeof(*_tmp4C4));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpCDC=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4C5;_tmp4C5.tag=5;({struct Cyc_Absyn_PtrInfo _tmpCDB=({struct Cyc_Absyn_PtrInfo _tmp4C6;({void*_tmpCD5=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4BE);_tmp4C6.elt_typ=_tmpCD5;});({struct Cyc_Absyn_Tqual _tmpCD6=
Cyc_Absyn_empty_tqual(0);_tmp4C6.elt_tq=_tmpCD6;});({struct Cyc_Absyn_PtrAtts _tmpCDA=({({void*_tmpCD7=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4BE);(_tmp4C6.ptr_atts).rgn=_tmpCD7;});(_tmp4C6.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmpCD8=
# 2401
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4C6.ptr_atts).bounds=_tmpCD8;});({union Cyc_Absyn_Constraint*_tmpCD9=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4C6.ptr_atts).zero_term=_tmpCD9;});(_tmp4C6.ptr_atts).ptrloc=0;_tmp4C6.ptr_atts;});_tmp4C6.ptr_atts=_tmpCDA;});_tmp4C6;});_tmp4C5.f1=_tmpCDB;});_tmp4C5;});_tmp4C4[0]=_tmpCDC;});_tmp4C4;});
e1->topt=(void*)_tmp4BF;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4C3;_tmp4C3.tag=0;({struct _dyneither_ptr _tmpCDD=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));_tmp4C3.f1=_tmpCDD;});({struct Cyc_String_pa_PrintArg_struct _tmp4C2;_tmp4C2.tag=0;({struct _dyneither_ptr _tmpCDE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp4C2.f1=_tmpCDE;});({void*_tmp4C0[2]={& _tmp4C2,& _tmp4C3};({struct _dyneither_ptr _tmpCDF=({const char*_tmp4C1="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4C1,sizeof(char),56);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCDF,_tag_dyneither(_tmp4C0,sizeof(void*),2));});});});});
return retv;};};}
# 2411
return 0;}
# 2414
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4C7=c;switch(_tmp4C7){case Cyc_Absyn_Unknown_coercion: _LL4B6: _LL4B7:
 return({const char*_tmp4C8="unknown";_tag_dyneither(_tmp4C8,sizeof(char),8);});case Cyc_Absyn_No_coercion: _LL4B8: _LL4B9:
 return({const char*_tmp4C9="no coercion";_tag_dyneither(_tmp4C9,sizeof(char),12);});case Cyc_Absyn_Null_to_NonNull: _LL4BA: _LL4BB:
 return({const char*_tmp4CA="null check";_tag_dyneither(_tmp4CA,sizeof(char),11);});default: _LL4BC: _LL4BD:
 return({const char*_tmp4CB="other coercion";_tag_dyneither(_tmp4CB,sizeof(char),15);});}_LL4B5:;}
# 2423
int Cyc_Tcutil_warn_alias_coerce=0;
# 2429
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2433
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4DF=_cycalloc(sizeof(*_tmp4DF));_tmp4DF->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpCE2=({struct _dyneither_ptr*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));({struct _dyneither_ptr _tmpCE1=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4E3;_tmp4E3.tag=1;_tmp4E3.f1=(unsigned long)counter ++;({void*_tmp4E1[1]={& _tmp4E3};({struct _dyneither_ptr _tmpCE0=({const char*_tmp4E2="__aliasvar%d";_tag_dyneither(_tmp4E2,sizeof(char),13);});Cyc_aprintf(_tmpCE0,_tag_dyneither(_tmp4E1,sizeof(void*),1));});});});_tmp4E0[0]=_tmpCE1;});_tmp4E0;});_tmp4DF->f2=_tmpCE2;});_tmp4DF;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0,v,e_typ,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpCE4=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCE3=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4DE;_tmp4DE.tag=4;_tmp4DE.f1=vd;_tmp4DE;});_tmp4DD[0]=_tmpCE3;});_tmp4DD;});Cyc_Absyn_varb_exp(_tmpCE4,e->loc);});
# 2442
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2444
{void*_tmp4CC=Cyc_Tcutil_compress(e_typ);void*_tmp4CD=_tmp4CC;void*_tmp4DB;struct Cyc_Absyn_Tqual _tmp4DA;void*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4D8;union Cyc_Absyn_Constraint*_tmp4D7;union Cyc_Absyn_Constraint*_tmp4D6;struct Cyc_Absyn_PtrLoc*_tmp4D5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->tag == 5){_LL4BF: _tmp4DB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).elt_typ;_tmp4DA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).elt_tq;_tmp4D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).rgn;_tmp4D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).nullable;_tmp4D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).bounds;_tmp4D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).zero_term;_tmp4D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CD)->f1).ptr_atts).ptrloc;_LL4C0:
# 2446
{void*_tmp4CE=Cyc_Tcutil_compress(_tmp4D9);void*_tmp4CF=_tmp4CE;void**_tmp4D4;struct Cyc_Core_Opt*_tmp4D3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4CF)->tag == 1){_LL4C4: _tmp4D4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4CF)->f2;_tmp4D3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4CF)->f4;_LL4C5: {
# 2448
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D0=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpCE5=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4D2;_tmp4D2.tag=2;_tmp4D2.f1=tv;_tmp4D2;});_tmp4D1[0]=_tmpCE5;});_tmp4D1;});
*_tmp4D4=(void*)_tmp4D0;
goto _LL4C3;}}else{_LL4C6: _LL4C7:
 goto _LL4C3;}_LL4C3:;}
# 2453
goto _LL4BE;}else{_LL4C1: _LL4C2:
 goto _LL4BE;}_LL4BE:;}
# 2457
e->topt=0;
vd->initializer=0;{
# 2461
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2463
return({struct _tuple15 _tmp4DC;_tmp4DC.f1=d;_tmp4DC.f2=ve;_tmp4DC;});};}
# 2468
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2471
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2476
void*_tmp4E5=Cyc_Tcutil_compress(wants_typ);void*_tmp4E6=_tmp4E5;void*_tmp4EA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->tag == 5){_LL4C9: _tmp4EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E6)->f1).ptr_atts).rgn;_LL4CA: {
# 2478
void*_tmp4E7=Cyc_Tcutil_compress(_tmp4EA);void*_tmp4E8=_tmp4E7;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4E8)->tag == 20){_LL4CE: _LL4CF:
 return 0;}else{_LL4D0: _LL4D1: {
# 2481
struct Cyc_Absyn_Kind*_tmp4E9=Cyc_Tcutil_typ_kind(_tmp4EA);
return _tmp4E9->kind == Cyc_Absyn_RgnKind  && _tmp4E9->aliasqual == Cyc_Absyn_Aliasable;}}_LL4CD:;}}else{_LL4CB: _LL4CC:
# 2484
 return 0;}_LL4C8:;}
# 2487
return 0;}
# 2491
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2496
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2498
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2500
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0;({struct _dyneither_ptr _tmpCE6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4EE.f1=_tmpCE6;});({struct Cyc_String_pa_PrintArg_struct _tmp4ED;_tmp4ED.tag=0;({struct _dyneither_ptr _tmpCE7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4ED.f1=_tmpCE7;});({void*_tmp4EB[2]={& _tmp4ED,& _tmp4EE};({unsigned int _tmpCE9=e->loc;struct _dyneither_ptr _tmpCE8=({const char*_tmp4EC="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp4EC,sizeof(char),53);});Cyc_Tcutil_warn(_tmpCE9,_tmpCE8,_tag_dyneither(_tmp4EB,sizeof(void*),2));});});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2509
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0;({struct _dyneither_ptr _tmpCEA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4F3.f1=_tmpCEA;});({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0;({struct _dyneither_ptr _tmpCEB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F2.f1=_tmpCEB;});({struct Cyc_String_pa_PrintArg_struct _tmp4F1;_tmp4F1.tag=0;({struct _dyneither_ptr _tmpCEC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4F1.f1=_tmpCEC;});({void*_tmp4EF[3]={& _tmp4F1,& _tmp4F2,& _tmp4F3};({unsigned int _tmpCEE=e->loc;struct _dyneither_ptr _tmpCED=({const char*_tmp4F0="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp4F0,sizeof(char),40);});Cyc_Tcutil_warn(_tmpCEE,_tmpCED,_tag_dyneither(_tmp4EF,sizeof(void*),3));});});});});});
*alias_coercion=1;}
# 2516
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if(({({enum Cyc_Absyn_Coercion _tmpCEF=Cyc_Tcutil_castable(te,e->loc,t1,t2);c=_tmpCEF;})!= Cyc_Absyn_Unknown_coercion;})){
# 2524
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0;({struct _dyneither_ptr _tmpCF0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4F7.f1=_tmpCF0;});({struct Cyc_String_pa_PrintArg_struct _tmp4F6;_tmp4F6.tag=0;({struct _dyneither_ptr _tmpCF1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F6.f1=_tmpCF1;});({void*_tmp4F4[2]={& _tmp4F6,& _tmp4F7};({unsigned int _tmpCF3=e->loc;struct _dyneither_ptr _tmpCF2=({const char*_tmp4F5="implicit cast from %s to %s";_tag_dyneither(_tmp4F5,sizeof(char),28);});Cyc_Tcutil_warn(_tmpCF3,_tmpCF2,_tag_dyneither(_tmp4F4,sizeof(void*),2));});});});});
return 1;}else{
# 2530
return 0;}}}}}
# 2537
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2540
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2544
int Cyc_Tcutil_coerceable(void*t){
void*_tmp4F8=Cyc_Tcutil_compress(t);void*_tmp4F9=_tmp4F8;switch(*((int*)_tmp4F9)){case 6: _LL4D3: _LL4D4:
 goto _LL4D6;case 7: _LL4D5: _LL4D6:
 return 1;default: _LL4D7: _LL4D8:
 return 0;}_LL4D2:;}
# 2562 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2566
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2569
struct _tuple23 _tmp4FA=*env;struct _tuple23 _tmp4FB=_tmp4FA;struct Cyc_List_List*_tmp503;struct _RegionHandle*_tmp502;struct Cyc_Tcenv_Tenv*_tmp501;int _tmp500;_LL4DA: _tmp503=_tmp4FB.f1;_tmp502=_tmp4FB.f2;_tmp501=_tmp4FB.f3;_tmp500=_tmp4FB.f4;_LL4DB:;{
# 2571
void*_tmp4FC=_tmp503 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp502,_tmp503,x->type);
struct Cyc_List_List*_tmp4FD=Cyc_Tcutil_flatten_typ(_tmp502,_tmp500,_tmp501,_tmp4FC);
if(({((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4FD)== 1;}))
return({struct Cyc_List_List*_tmp4FE=_region_malloc(_tmp502,sizeof(*_tmp4FE));({struct _tuple12*_tmpCF4=({struct _tuple12*_tmp4FF=_region_malloc(_tmp502,sizeof(*_tmp4FF));_tmp4FF->f1=x->tq;_tmp4FF->f2=_tmp4FC;_tmp4FF;});_tmp4FE->hd=_tmpCF4;});_tmp4FE->tl=0;_tmp4FE;});else{
return _tmp4FD;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2577
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2579
struct _tuple24 _tmp504=*env;struct _tuple24 _tmp505=_tmp504;struct _RegionHandle*_tmp50F;struct Cyc_Tcenv_Tenv*_tmp50E;int _tmp50D;_LL4DD: _tmp50F=_tmp505.f1;_tmp50E=_tmp505.f2;_tmp50D=_tmp505.f3;_LL4DE:;{
struct _tuple12 _tmp506=*x;struct _tuple12 _tmp507=_tmp506;struct Cyc_Absyn_Tqual _tmp50C;void*_tmp50B;_LL4E0: _tmp50C=_tmp507.f1;_tmp50B=_tmp507.f2;_LL4E1:;{
struct Cyc_List_List*_tmp508=Cyc_Tcutil_flatten_typ(_tmp50F,_tmp50D,_tmp50E,_tmp50B);
if(({((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp508)== 1;}))
return({struct Cyc_List_List*_tmp509=_region_malloc(_tmp50F,sizeof(*_tmp509));({struct _tuple12*_tmpCF5=({struct _tuple12*_tmp50A=_region_malloc(_tmp50F,sizeof(*_tmp50A));_tmp50A->f1=_tmp50C;_tmp50A->f2=_tmp50B;_tmp50A;});_tmp509->hd=_tmpCF5;});_tmp509->tl=0;_tmp509;});else{
return _tmp508;}};};}
# 2586
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2590
if(flatten){
({void*_tmpCF6=Cyc_Tcutil_compress(t1);t1=_tmpCF6;});{
void*_tmp510=t1;struct Cyc_List_List*_tmp532;struct Cyc_Absyn_Aggrdecl*_tmp531;struct Cyc_List_List*_tmp530;struct Cyc_List_List*_tmp52F;switch(*((int*)_tmp510)){case 0: _LL4E3: _LL4E4:
 return 0;case 10: _LL4E5: _tmp52F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp510)->f1;_LL4E6: {
# 2595
struct _tuple24 _tmp511=({struct _tuple24 _tmp51A;_tmp51A.f1=r;_tmp51A.f2=te;_tmp51A.f3=flatten;_tmp51A;});
# 2597
struct Cyc_List_List*_tmp512=_tmp52F;struct _tuple12*_tmp519;struct Cyc_List_List*_tmp518;if(_tmp512 == 0){_LL4EE: _LL4EF:
 return 0;}else{_LL4F0: _tmp519=(struct _tuple12*)_tmp512->hd;_tmp518=_tmp512->tl;_LL4F1: {
# 2600
struct Cyc_List_List*_tmp513=Cyc_Tcutil_rcopy_tqt(& _tmp511,_tmp519);
_tmp511.f3=0;{
struct Cyc_List_List*_tmp514=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp511,_tmp52F);
struct Cyc_List_List*_tmp515=({struct Cyc_List_List*_tmp517=_region_malloc(r,sizeof(*_tmp517));_tmp517->hd=_tmp513;_tmp517->tl=_tmp514;_tmp517;});
return({struct _RegionHandle*_tmpCF7=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpCF7,({struct Cyc_List_List*_tmp516=_region_malloc(r,sizeof(*_tmp516));_tmp516->hd=_tmp513;_tmp516->tl=_tmp514;_tmp516;}));});};}}_LL4ED:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp510)->f1).aggr_info).KnownAggr).tag == 2){_LL4E7: _tmp531=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp510)->f1).aggr_info).KnownAggr).val;_tmp530=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp510)->f1).targs;_LL4E8:
# 2608
 if(((_tmp531->kind == Cyc_Absyn_UnionA  || _tmp531->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp531->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp531->impl))->rgn_po != 0)
# 2610
return({struct Cyc_List_List*_tmp51B=_region_malloc(r,sizeof(*_tmp51B));({struct _tuple12*_tmpCF9=({struct _tuple12*_tmp51C=_region_malloc(r,sizeof(*_tmp51C));({struct Cyc_Absyn_Tqual _tmpCF8=Cyc_Absyn_empty_tqual(0);_tmp51C->f1=_tmpCF8;});_tmp51C->f2=t1;_tmp51C;});_tmp51B->hd=_tmpCF9;});_tmp51B->tl=0;_tmp51B;});{
struct Cyc_List_List*_tmp51D=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp531->tvs,_tmp530);
struct _tuple23 env=({struct _tuple23 _tmp526;_tmp526.f1=_tmp51D;_tmp526.f2=r;_tmp526.f3=te;_tmp526.f4=flatten;_tmp526;});
struct Cyc_List_List*_tmp51E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp531->impl))->fields;struct Cyc_List_List*_tmp51F=_tmp51E;struct Cyc_Absyn_Aggrfield*_tmp525;struct Cyc_List_List*_tmp524;if(_tmp51F == 0){_LL4F3: _LL4F4:
 return 0;}else{_LL4F5: _tmp525=(struct Cyc_Absyn_Aggrfield*)_tmp51F->hd;_tmp524=_tmp51F->tl;_LL4F6: {
# 2616
struct Cyc_List_List*_tmp520=Cyc_Tcutil_flatten_typ_f(& env,_tmp525);
env.f4=0;{
struct Cyc_List_List*_tmp521=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp524);
struct Cyc_List_List*_tmp522=({struct Cyc_List_List*_tmp523=_region_malloc(r,sizeof(*_tmp523));_tmp523->hd=_tmp520;_tmp523->tl=_tmp521;_tmp523;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp522);};}}_LL4F2:;};}else{goto _LL4EB;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp510)->f1 == Cyc_Absyn_StructA){_LL4E9: _tmp532=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp510)->f2;_LL4EA: {
# 2623
struct _tuple23 env=({struct _tuple23 _tmp52E;_tmp52E.f1=0;_tmp52E.f2=r;_tmp52E.f3=te;_tmp52E.f4=flatten;_tmp52E;});
struct Cyc_List_List*_tmp527=_tmp532;struct Cyc_Absyn_Aggrfield*_tmp52D;struct Cyc_List_List*_tmp52C;if(_tmp527 == 0){_LL4F8: _LL4F9:
 return 0;}else{_LL4FA: _tmp52D=(struct Cyc_Absyn_Aggrfield*)_tmp527->hd;_tmp52C=_tmp527->tl;_LL4FB: {
# 2627
struct Cyc_List_List*_tmp528=Cyc_Tcutil_flatten_typ_f(& env,_tmp52D);
env.f4=0;{
struct Cyc_List_List*_tmp529=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp52C);
struct Cyc_List_List*_tmp52A=({struct Cyc_List_List*_tmp52B=_region_malloc(r,sizeof(*_tmp52B));_tmp52B->hd=_tmp528;_tmp52B->tl=_tmp529;_tmp52B;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp52A);};}}_LL4F7:;}}else{goto _LL4EB;}default: _LL4EB: _LL4EC:
# 2633
 goto _LL4E2;}_LL4E2:;};}
# 2636
return({struct Cyc_List_List*_tmp533=_region_malloc(r,sizeof(*_tmp533));({struct _tuple12*_tmpCFB=({struct _tuple12*_tmp534=_region_malloc(r,sizeof(*_tmp534));({struct Cyc_Absyn_Tqual _tmpCFA=Cyc_Absyn_empty_tqual(0);_tmp534->f1=_tmpCFA;});_tmp534->f2=t1;_tmp534;});_tmp533->hd=_tmpCFB;});_tmp533->tl=0;_tmp533;});}
# 2640
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp535=(void*)t->hd;void*_tmp536=_tmp535;switch(*((int*)_tmp536)){case 23: _LL4FD: _LL4FE:
 goto _LL500;case 4: _LL4FF: _LL500:
 goto _LL502;case 20: _LL501: _LL502:
# 2646
 continue;default: _LL503: _LL504:
# 2648
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL4FC:;}}
# 2651
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2654
return 1;}
# 2657
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp537=({struct _tuple0 _tmp53B;({void*_tmpCFC=Cyc_Tcutil_compress(t1);_tmp53B.f1=_tmpCFC;});({void*_tmpCFD=Cyc_Tcutil_compress(t2);_tmp53B.f2=_tmpCFD;});_tmp53B;});struct _tuple0 _tmp538=_tmp537;enum Cyc_Absyn_Size_of _tmp53A;enum Cyc_Absyn_Size_of _tmp539;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp538.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp538.f2)->tag == 6){_LL506: _tmp53A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp538.f1)->f2;_tmp539=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp538.f2)->f2;_LL507:
# 2660
 return(_tmp53A == _tmp539  || _tmp53A == Cyc_Absyn_Int_sz  && _tmp539 == Cyc_Absyn_Long_sz) || 
_tmp53A == Cyc_Absyn_Long_sz  && _tmp539 == Cyc_Absyn_Int_sz;}else{goto _LL508;}}else{_LL508: _LL509:
 return 0;}_LL505:;}
# 2668
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2671
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2676
({void*_tmpCFE=Cyc_Tcutil_compress(t1);t1=_tmpCFE;});
({void*_tmpCFF=Cyc_Tcutil_compress(t2);t2=_tmpCFF;});{
struct _tuple0 _tmp53C=({struct _tuple0 _tmp56F;_tmp56F.f1=t1;_tmp56F.f2=t2;_tmp56F;});struct _tuple0 _tmp53D=_tmp53C;struct Cyc_Absyn_FnInfo _tmp56E;struct Cyc_Absyn_FnInfo _tmp56D;struct Cyc_Absyn_Datatypedecl*_tmp56C;struct Cyc_Absyn_Datatypefield*_tmp56B;struct Cyc_List_List*_tmp56A;struct Cyc_Absyn_Datatypedecl*_tmp569;struct Cyc_List_List*_tmp568;void*_tmp567;struct Cyc_Absyn_Tqual _tmp566;void*_tmp565;union Cyc_Absyn_Constraint*_tmp564;union Cyc_Absyn_Constraint*_tmp563;union Cyc_Absyn_Constraint*_tmp562;void*_tmp561;struct Cyc_Absyn_Tqual _tmp560;void*_tmp55F;union Cyc_Absyn_Constraint*_tmp55E;union Cyc_Absyn_Constraint*_tmp55D;union Cyc_Absyn_Constraint*_tmp55C;switch(*((int*)_tmp53D.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f2)->tag == 5){_LL50B: _tmp567=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f1)->f1).elt_typ;_tmp566=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f1)->f1).elt_tq;_tmp565=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f1)->f1).ptr_atts).rgn;_tmp564=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f1)->f1).ptr_atts).nullable;_tmp563=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f1)->f1).ptr_atts).bounds;_tmp562=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f1)->f1).ptr_atts).zero_term;_tmp561=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f2)->f1).elt_typ;_tmp560=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f2)->f1).elt_tq;_tmp55F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f2)->f1).ptr_atts).rgn;_tmp55E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f2)->f1).ptr_atts).nullable;_tmp55D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f2)->f1).ptr_atts).bounds;_tmp55C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53D.f2)->f1).ptr_atts).zero_term;_LL50C:
# 2684
 if(_tmp566.real_const  && !_tmp560.real_const)
return 0;
# 2687
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp564,_tmp55E) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp564)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp55E))
return 0;
# 2691
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp562,_tmp55C) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp562)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp55C))
return 0;
# 2695
if((!Cyc_Tcutil_unify(_tmp565,_tmp55F) && !Cyc_Tcenv_region_outlives(te,_tmp565,_tmp55F)) && !
Cyc_Tcutil_subtype(te,assume,_tmp565,_tmp55F))
return 0;
# 2699
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp563,_tmp55D)){
struct _tuple0 _tmp53E=({struct _tuple0 _tmp542;({void*_tmpD00=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp563);_tmp542.f1=_tmpD00;});({void*_tmpD01=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp55D);_tmp542.f2=_tmpD01;});_tmp542;});struct _tuple0 _tmp53F=_tmp53E;struct Cyc_Absyn_Exp*_tmp541;struct Cyc_Absyn_Exp*_tmp540;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp53F.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp53F.f2)->tag == 0){_LL514: _LL515:
 goto _LL513;}else{goto _LL518;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp53F.f2)->tag == 1){_LL516: _tmp541=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp53F.f1)->f1;_tmp540=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp53F.f2)->f1;_LL517:
# 2704
 if(!Cyc_Evexp_lte_const_exp(_tmp540,_tmp541))
return 0;
goto _LL513;}else{_LL518: _LL519:
 return 0;}}_LL513:;}
# 2712
if(!_tmp560.real_const  && _tmp566.real_const){
if(!({Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp561));}))
return 0;}{
# 2718
int _tmp543=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp55D,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp55C);
# 2724
return(_tmp543  && ({struct Cyc_Tcenv_Tenv*_tmpD05=te;struct Cyc_List_List*_tmpD04=({struct Cyc_List_List*_tmp544=_cycalloc(sizeof(*_tmp544));({struct _tuple0*_tmpD02=({struct _tuple0*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->f1=t1;_tmp545->f2=t2;_tmp545;});_tmp544->hd=_tmpD02;});_tmp544->tl=assume;_tmp544;});void*_tmpD03=_tmp567;Cyc_Tcutil_ptrsubtype(_tmpD05,_tmpD04,_tmpD03,_tmp561);}) || Cyc_Tcutil_unify(_tmp567,_tmp561)) || Cyc_Tcutil_isomorphic(_tmp567,_tmp561);};}else{goto _LL511;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp53D.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp53D.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp53D.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL50D: _tmp56C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp53D.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp56B=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp53D.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp56A=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp53D.f1)->f1).targs;_tmp569=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp53D.f2)->f1).datatype_info).KnownDatatype).val;_tmp568=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp53D.f2)->f1).targs;_LL50E:
# 2730
 if(_tmp56C != _tmp569  && ({Cyc_Absyn_qvar_cmp(_tmp56C->name,_tmp569->name)!= 0;}))return 0;
# 2732
if(({int _tmpD06=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56A);_tmpD06 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp568);}))return 0;
for(0;_tmp56A != 0;(_tmp56A=_tmp56A->tl,_tmp568=_tmp568->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp56A->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp568))->hd))return 0;}
return 1;}else{goto _LL511;}}else{goto _LL511;}}else{goto _LL511;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53D.f2)->tag == 9){_LL50F: _tmp56E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53D.f1)->f1;_tmp56D=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53D.f2)->f1;_LL510:
# 2739
 if(_tmp56E.tvars != 0  || _tmp56D.tvars != 0){
struct Cyc_List_List*_tmp546=_tmp56E.tvars;
struct Cyc_List_List*_tmp547=_tmp56D.tvars;
if(({int _tmpD07=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp546);_tmpD07 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp547);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp546 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp546->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp547))->hd)->kind))return 0;
({struct Cyc_List_List*_tmpD0B=({struct Cyc_List_List*_tmp548=_cycalloc(sizeof(*_tmp548));({struct _tuple16*_tmpD0A=({struct _tuple16*_tmp549=_cycalloc(sizeof(*_tmp549));_tmp549->f1=(struct Cyc_Absyn_Tvar*)_tmp547->hd;({void*_tmpD09=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp54A=_cycalloc(sizeof(*_tmp54A));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD08=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp54B;_tmp54B.tag=2;_tmp54B.f1=(struct Cyc_Absyn_Tvar*)_tmp546->hd;_tmp54B;});_tmp54A[0]=_tmpD08;});_tmp54A;});_tmp549->f2=_tmpD09;});_tmp549;});_tmp548->hd=_tmpD0A;});_tmp548->tl=inst;_tmp548;});inst=_tmpD0B;});
_tmp546=_tmp546->tl;
_tmp547=_tmp547->tl;}
# 2750
if(inst != 0){
_tmp56E.tvars=0;
_tmp56D.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpD10=te;struct Cyc_List_List*_tmpD0F=assume;void*_tmpD0E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54C=_cycalloc(sizeof(*_tmp54C));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD0C=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp54D;_tmp54D.tag=9;_tmp54D.f1=_tmp56E;_tmp54D;});_tmp54C[0]=_tmpD0C;});_tmp54C;});Cyc_Tcutil_subtype(_tmpD10,_tmpD0F,_tmpD0E,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54E=_cycalloc(sizeof(*_tmp54E));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD0D=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp54F;_tmp54F.tag=9;_tmp54F.f1=_tmp56D;_tmp54F;});_tmp54E[0]=_tmpD0D;});_tmp54E;}));});}};}
# 2757
if(!Cyc_Tcutil_subtype(te,assume,_tmp56E.ret_typ,_tmp56D.ret_typ))return 0;{
struct Cyc_List_List*_tmp550=_tmp56E.args;
struct Cyc_List_List*_tmp551=_tmp56D.args;
# 2762
if(({int _tmpD11=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp550);_tmpD11 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp551);}))return 0;
# 2764
for(0;_tmp550 != 0;(_tmp550=_tmp550->tl,_tmp551=_tmp551->tl)){
struct _tuple10 _tmp552=*((struct _tuple10*)_tmp550->hd);struct _tuple10 _tmp553=_tmp552;struct Cyc_Absyn_Tqual _tmp559;void*_tmp558;_LL51B: _tmp559=_tmp553.f2;_tmp558=_tmp553.f3;_LL51C:;{
struct _tuple10 _tmp554=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp551))->hd);struct _tuple10 _tmp555=_tmp554;struct Cyc_Absyn_Tqual _tmp557;void*_tmp556;_LL51E: _tmp557=_tmp555.f2;_tmp556=_tmp555.f3;_LL51F:;
# 2768
if(_tmp557.real_const  && !_tmp559.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp556,_tmp558))
return 0;};}
# 2772
if(_tmp56E.c_varargs != _tmp56D.c_varargs)return 0;
if(_tmp56E.cyc_varargs != 0  && _tmp56D.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp55A=*_tmp56E.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp55B=*_tmp56D.cyc_varargs;
# 2777
if((_tmp55B.tq).real_const  && !(_tmp55A.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp55B.type,_tmp55A.type))
return 0;}else{
if(_tmp56E.cyc_varargs != 0  || _tmp56D.cyc_varargs != 0)return 0;}
# 2782
if(!({void*_tmpD12=(void*)_check_null(_tmp56E.effect);Cyc_Tcutil_subset_effect(1,_tmpD12,(void*)_check_null(_tmp56D.effect));}))return 0;
# 2784
if(!Cyc_Tcutil_sub_rgnpo(_tmp56E.rgn_po,_tmp56D.rgn_po))return 0;
# 2786
if(!Cyc_Tcutil_sub_attributes(_tmp56E.attributes,_tmp56D.attributes))return 0;
# 2788
if(!Cyc_Tcutil_check_logical_implication(_tmp56D.requires_relns,_tmp56E.requires_relns))
return 0;
# 2791
if(!Cyc_Tcutil_check_logical_implication(_tmp56E.ensures_relns,_tmp56D.ensures_relns))
return 0;
# 2794
return 1;};}else{goto _LL511;}default: _LL511: _LL512:
 return 0;}_LL50A:;};}
# 2806 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2808
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp570=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp571=_tmp570;struct Cyc_Absyn_Tqual _tmp577;void*_tmp576;_LL521: _tmp577=_tmp571->f1;_tmp576=_tmp571->f2;_LL522:;{
struct _tuple12*_tmp572=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp573=_tmp572;struct Cyc_Absyn_Tqual _tmp575;void*_tmp574;_LL524: _tmp575=_tmp573->f1;_tmp574=_tmp573->f2;_LL525:;
# 2815
if(_tmp577.real_const  && !_tmp575.real_const)return 0;
# 2817
if((_tmp575.real_const  || ({Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp574));})) && 
Cyc_Tcutil_subtype(te,assume,_tmp576,_tmp574))
# 2820
continue;
# 2822
if(Cyc_Tcutil_unify(_tmp576,_tmp574))
# 2824
continue;
# 2826
if(Cyc_Tcutil_isomorphic(_tmp576,_tmp574))
# 2828
continue;
# 2831
return 0;};};}
# 2833
return 1;}
# 2836
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp578=Cyc_Tcutil_compress(t);void*_tmp579=_tmp578;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp579)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp579)->f2 == Cyc_Absyn_Char_sz){_LL527: _LL528:
 return 1;}else{goto _LL529;}}else{_LL529: _LL52A:
 return 0;}_LL526:;}
# 2845
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
({void*_tmpD13=Cyc_Tcutil_compress(t1);t1=_tmpD13;});
({void*_tmpD14=Cyc_Tcutil_compress(t2);t2=_tmpD14;});
# 2851
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp57A=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp57A)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp57A)->f2){case Cyc_Absyn_Int_sz: _LL52C: _LL52D:
# 2855
 goto _LL52F;case Cyc_Absyn_Long_sz: _LL52E: _LL52F:
# 2857
 if(({(Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind;}))return Cyc_Absyn_Other_coercion;
goto _LL52B;default: goto _LL530;}else{_LL530: _LL531:
 goto _LL52B;}_LL52B:;}{
# 2861
void*_tmp57B=t1;void*_tmp5A3;struct Cyc_Absyn_Enumdecl*_tmp5A2;void*_tmp5A1;struct Cyc_Absyn_Tqual _tmp5A0;struct Cyc_Absyn_Exp*_tmp59F;union Cyc_Absyn_Constraint*_tmp59E;void*_tmp59D;struct Cyc_Absyn_Tqual _tmp59C;void*_tmp59B;union Cyc_Absyn_Constraint*_tmp59A;union Cyc_Absyn_Constraint*_tmp599;union Cyc_Absyn_Constraint*_tmp598;switch(*((int*)_tmp57B)){case 5: _LL533: _tmp59D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57B)->f1).elt_typ;_tmp59C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57B)->f1).elt_tq;_tmp59B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57B)->f1).ptr_atts).rgn;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57B)->f1).ptr_atts).nullable;_tmp599=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57B)->f1).ptr_atts).bounds;_tmp598=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57B)->f1).ptr_atts).zero_term;_LL534:
# 2870
{void*_tmp57C=t2;void*_tmp58E;struct Cyc_Absyn_Tqual _tmp58D;void*_tmp58C;union Cyc_Absyn_Constraint*_tmp58B;union Cyc_Absyn_Constraint*_tmp58A;union Cyc_Absyn_Constraint*_tmp589;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->tag == 5){_LL542: _tmp58E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).elt_typ;_tmp58D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).elt_tq;_tmp58C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).rgn;_tmp58B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).nullable;_tmp58A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).bounds;_tmp589=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).zero_term;_LL543: {
# 2874
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp57D=({struct Cyc_List_List*_tmp587=_cycalloc(sizeof(*_tmp587));({struct _tuple0*_tmpD15=({struct _tuple0*_tmp588=_cycalloc(sizeof(*_tmp588));_tmp588->f1=t1;_tmp588->f2=t2;_tmp588;});_tmp587->hd=_tmpD15;});_tmp587->tl=0;_tmp587;});
int _tmp57E=_tmp58D.real_const  || !_tmp59C.real_const;
# 2888 "tcutil.cyc"
int _tmp57F=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp58A,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp589);
# 2892
int _tmp580=_tmp57E  && (
((_tmp57F  && Cyc_Tcutil_ptrsubtype(te,_tmp57D,_tmp59D,_tmp58E) || 
Cyc_Tcutil_unify(_tmp59D,_tmp58E)) || Cyc_Tcutil_isomorphic(_tmp59D,_tmp58E)) || Cyc_Tcutil_unify(_tmp58E,(void*)& Cyc_Absyn_VoidType_val));
# 2896
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp598,_tmp589) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp589);
# 2900
int _tmp581=_tmp580?0:((Cyc_Tcutil_bits_only(_tmp59D) && Cyc_Tcutil_is_char_type(_tmp58E)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp589)) && (
_tmp58D.real_const  || !_tmp59C.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp599,_tmp58A);
if(!bounds_ok  && !_tmp581){
struct _tuple0 _tmp582=({struct _tuple0 _tmp586;({void*_tmpD16=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp599);_tmp586.f1=_tmpD16;});({void*_tmpD17=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp58A);_tmp586.f2=_tmpD17;});_tmp586;});struct _tuple0 _tmp583=_tmp582;struct Cyc_Absyn_Exp*_tmp585;struct Cyc_Absyn_Exp*_tmp584;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp583.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp583.f2)->tag == 1){_LL547: _tmp585=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp583.f1)->f1;_tmp584=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp583.f2)->f1;_LL548:
# 2910
 if(Cyc_Evexp_lte_const_exp(_tmp584,_tmp585))
bounds_ok=1;
goto _LL546;}else{goto _LL549;}}else{_LL549: _LL54A:
# 2915
 bounds_ok=1;goto _LL546;}_LL546:;}{
# 2917
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp59A);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp58B);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2924
if(((bounds_ok  && zeroterm_ok) && (_tmp580  || _tmp581)) && (
Cyc_Tcutil_unify(_tmp59B,_tmp58C) || Cyc_Tcenv_region_outlives(te,_tmp59B,_tmp58C)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL544: _LL545:
 goto _LL541;}_LL541:;}
# 2930
return Cyc_Absyn_Unknown_coercion;case 8: _LL535: _tmp5A1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57B)->f1).elt_type;_tmp5A0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57B)->f1).tq;_tmp59F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57B)->f1).num_elts;_tmp59E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57B)->f1).zero_term;_LL536:
# 2932
{void*_tmp58F=t2;void*_tmp593;struct Cyc_Absyn_Tqual _tmp592;struct Cyc_Absyn_Exp*_tmp591;union Cyc_Absyn_Constraint*_tmp590;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp58F)->tag == 8){_LL54C: _tmp593=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp58F)->f1).elt_type;_tmp592=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp58F)->f1).tq;_tmp591=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp58F)->f1).num_elts;_tmp590=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp58F)->f1).zero_term;_LL54D: {
# 2934
int okay;
({int _tmpD18=
(((_tmp59F != 0  && _tmp591 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp59E,_tmp590)) && 
Cyc_Evexp_lte_const_exp(_tmp591,_tmp59F)) && 
Cyc_Evexp_lte_const_exp(_tmp59F,_tmp591);
# 2935
okay=_tmpD18;});
# 2939
return
# 2941
(okay  && Cyc_Tcutil_unify(_tmp5A1,_tmp593)) && (!_tmp5A0.real_const  || _tmp592.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL54E: _LL54F:
# 2943
 return Cyc_Absyn_Unknown_coercion;}_LL54B:;}
# 2945
return Cyc_Absyn_Unknown_coercion;case 13: _LL537: _tmp5A2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp57B)->f2;_LL538:
# 2949
{void*_tmp594=t2;struct Cyc_Absyn_Enumdecl*_tmp595;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp594)->tag == 13){_LL551: _tmp595=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp594)->f2;_LL552:
# 2951
 if((_tmp5A2->fields != 0  && _tmp595->fields != 0) && ({
int _tmpD19=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A2->fields))->v);_tmpD19 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp595->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL550;}else{_LL553: _LL554:
 goto _LL550;}_LL550:;}
# 2957
goto _LL53A;case 6: _LL539: _LL53A:
 goto _LL53C;case 7: _LL53B: _LL53C:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL53D: _tmp5A3=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp57B)->f1;_LL53E:
# 2962
{void*_tmp596=t2;void*_tmp597;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp596)->tag == 15){_LL556: _tmp597=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp596)->f1;_LL557:
# 2964
 if(Cyc_Tcenv_region_outlives(te,_tmp5A3,_tmp597))return Cyc_Absyn_No_coercion;
goto _LL555;}else{_LL558: _LL559:
 goto _LL555;}_LL555:;}
# 2968
return Cyc_Absyn_Unknown_coercion;default: _LL53F: _LL540:
 return Cyc_Absyn_Unknown_coercion;}_LL532:;};}
# 2974
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5A4=Cyc_Absyn_copy_exp(e);
({void*_tmpD1B=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5A5=_cycalloc(sizeof(*_tmp5A5));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpD1A=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5A6;_tmp5A6.tag=13;_tmp5A6.f1=t;_tmp5A6.f2=_tmp5A4;_tmp5A6.f3=0;_tmp5A6.f4=c;_tmp5A6;});_tmp5A5[0]=_tmpD1A;});_tmp5A5;});e->r=_tmpD1B;});
e->topt=t;}}
# 2982
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5A7=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5A8=_tmp5A7;switch(*((int*)_tmp5A8)){case 6: _LL55B: _LL55C:
 goto _LL55E;case 13: _LL55D: _LL55E:
 goto _LL560;case 14: _LL55F: _LL560:
 goto _LL562;case 19: _LL561: _LL562:
 return 1;case 1: _LL563: _LL564:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LL565: _LL566:
 return 0;}_LL55A:;}
# 2993
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5A9=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5AA=_tmp5A9;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AA)->tag == 7){_LL568: _LL569:
 return 1;}else{_LL56A: _LL56B:
 return 0;}_LL567:;};}
# 3002
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5AB=Cyc_Tcutil_compress(t);void*_tmp5AC=_tmp5AB;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5AC)->tag == 9){_LL56D: _LL56E:
 return 1;}else{_LL56F: _LL570:
 return 0;}_LL56C:;}
# 3009
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5AD=({struct _tuple0 _tmp5B1;_tmp5B1.f1=t1;_tmp5B1.f2=t2;_tmp5B1;});struct _tuple0 _tmp5AE=_tmp5AD;int _tmp5B0;int _tmp5AF;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AE.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AE.f2)->tag == 7){_LL572: _tmp5B0=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AE.f1)->f1;_tmp5AF=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AE.f2)->f1;_LL573:
# 3012
 if(_tmp5B0 != 0  && _tmp5B0 != 1)return t1;else{
if(_tmp5AF != 0  && _tmp5AF != 1)return t2;else{
if(_tmp5B0 >= _tmp5AF)return t1;else{
return t2;}}}}else{_LL574: _LL575:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AE.f2)->tag == 7){_LL576: _LL577:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL578: _LL579:
 goto _LL57B;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL580;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL582;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL584;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL586;else{goto _LL58C;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL580;case Cyc_Absyn_Int_sz: goto _LL584;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{goto _LL58C;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2){case Cyc_Absyn_Long_sz: _LL580: _LL581:
# 3022
 goto _LL583;case Cyc_Absyn_Int_sz: _LL584: _LL585:
# 3025
 goto _LL587;default: goto _LL58C;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL582;case Cyc_Absyn_Int_sz: goto _LL586;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL588;else{goto _LL58C;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL588;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{goto _LL58C;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL57C: _LL57D:
# 3020
 goto _LL57F;case Cyc_Absyn_Long_sz: _LL588: _LL589:
# 3027
 goto _LL58B;default: goto _LL58C;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL57A: _LL57B:
# 3019
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL582: _LL583:
# 3023
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL586: _LL587:
# 3026
 return Cyc_Absyn_uint_typ;default: goto _LL58C;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AE.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL57E: _LL57F:
# 3021
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL58A: _LL58B:
# 3028
 return Cyc_Absyn_slong_typ;default: goto _LL58C;}}}else{_LL58C: _LL58D:
 return Cyc_Absyn_sint_typ;}}}}_LL571:;}
# 3040 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
({void*_tmpD1C=Cyc_Absyn_compress_kb(c1);c1=_tmpD1C;});
({void*_tmpD1D=Cyc_Absyn_compress_kb(c2);c2=_tmpD1D;});
if(c1 == c2)return 1;{
struct _tuple0 _tmp5B2=({struct _tuple0 _tmp5C8;_tmp5C8.f1=c1;_tmp5C8.f2=c2;_tmp5C8;});struct _tuple0 _tmp5B3=_tmp5B2;struct Cyc_Core_Opt**_tmp5C7;struct Cyc_Absyn_Kind*_tmp5C6;struct Cyc_Core_Opt**_tmp5C5;struct Cyc_Absyn_Kind*_tmp5C4;struct Cyc_Core_Opt**_tmp5C3;struct Cyc_Absyn_Kind*_tmp5C2;struct Cyc_Absyn_Kind*_tmp5C1;struct Cyc_Core_Opt**_tmp5C0;struct Cyc_Core_Opt**_tmp5BF;struct Cyc_Absyn_Kind*_tmp5BE;struct Cyc_Core_Opt**_tmp5BD;struct Cyc_Absyn_Kind*_tmp5BC;struct Cyc_Absyn_Kind*_tmp5BB;struct Cyc_Absyn_Kind*_tmp5BA;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->tag == 0)switch(*((int*)_tmp5B3.f2)){case 0: _LL58F: _tmp5BB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->f1;_tmp5BA=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->f1;_LL590:
 return _tmp5BB == _tmp5BA;case 1: goto _LL591;default: _LL597: _tmp5BE=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->f1;_tmp5BD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->f1;_tmp5BC=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->f2;_LL598:
# 3053
 if(Cyc_Tcutil_kind_leq(_tmp5BE,_tmp5BC)){
({struct Cyc_Core_Opt*_tmpD1E=({struct Cyc_Core_Opt*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->v=c1;_tmp5B7;});*_tmp5BD=_tmpD1E;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->tag == 1){_LL591: _tmp5BF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->f1;_LL592:
# 3046
({struct Cyc_Core_Opt*_tmpD1F=({struct Cyc_Core_Opt*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->v=c1;_tmp5B4;});*_tmp5BF=_tmpD1F;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->tag == 1){_LL593: _tmp5C0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->f1;_LL594:
({struct Cyc_Core_Opt*_tmpD20=({struct Cyc_Core_Opt*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->v=c2;_tmp5B5;});*_tmp5C0=_tmpD20;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->tag == 0){_LL595: _tmp5C3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->f1;_tmp5C2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->f2;_tmp5C1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->f1;_LL596:
# 3049
 if(Cyc_Tcutil_kind_leq(_tmp5C1,_tmp5C2)){
({struct Cyc_Core_Opt*_tmpD21=({struct Cyc_Core_Opt*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->v=c2;_tmp5B6;});*_tmp5C3=_tmpD21;});return 1;}else{
return 0;}}else{_LL599: _tmp5C7=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->f1;_tmp5C6=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B3.f1)->f2;_tmp5C5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->f1;_tmp5C4=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B3.f2)->f2;_LL59A:
# 3057
 if(Cyc_Tcutil_kind_leq(_tmp5C6,_tmp5C4)){
({struct Cyc_Core_Opt*_tmpD22=({struct Cyc_Core_Opt*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->v=c1;_tmp5B8;});*_tmp5C5=_tmpD22;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5C4,_tmp5C6)){
({struct Cyc_Core_Opt*_tmpD23=({struct Cyc_Core_Opt*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->v=c2;_tmp5B9;});*_tmp5C7=_tmpD23;});return 1;}else{
return 0;}}}}}}_LL58E:;};}
# 3066
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3071
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5CD;_tmp5CD.tag=1;_tmp5CD.f1=(unsigned long)i;({void*_tmp5CB[1]={& _tmp5CD};({struct _dyneither_ptr _tmpD24=({const char*_tmp5CC="#%d";_tag_dyneither(_tmp5CC,sizeof(char),4);});Cyc_aprintf(_tmpD24,_tag_dyneither(_tmp5CB,sizeof(void*),1));});});});
return({struct Cyc_Absyn_Tvar*_tmp5C9=_cycalloc(sizeof(*_tmp5C9));({struct _dyneither_ptr*_tmpD25=({struct _dyneither_ptr*_tmp5CA=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5CA[0]=(struct _dyneither_ptr)s;_tmp5CA;});_tmp5C9->name=_tmpD25;});_tmp5C9->identity=- 1;_tmp5C9->kind=k;_tmp5C9;});}
# 3078
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5CE=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5CE,sizeof(char),0))== '#';}
# 3083
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5D1;_tmp5D1.tag=0;_tmp5D1.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5CF[1]={& _tmp5D1};({struct _dyneither_ptr _tmpD26=({const char*_tmp5D0="%s";_tag_dyneither(_tmp5D0,sizeof(char),3);});Cyc_printf(_tmpD26,_tag_dyneither(_tmp5CF,sizeof(void*),1));});});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp5D2=({struct _dyneither_ptr _tmpD27=({const char*_tmp5D7="`";_tag_dyneither(_tmp5D7,sizeof(char),2);});Cyc_strconcat(_tmpD27,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp5D3=_dyneither_ptr_plus(_tmp5D2,sizeof(char),1);char _tmp5D4=*((char*)_check_dyneither_subscript(_tmp5D3,sizeof(char),0));char _tmp5D5='t';if(_get_dyneither_size(_tmp5D3,sizeof(char))== 1  && (_tmp5D4 == '\000'  && _tmp5D5 != '\000'))_throw_arraybounds();*((char*)_tmp5D3.curr)=_tmp5D5;});
({struct _dyneither_ptr*_tmpD28=({struct _dyneither_ptr*_tmp5D6=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5D6[0]=(struct _dyneither_ptr)_tmp5D2;_tmp5D6;});t->name=_tmpD28;});};}
# 3092
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3094
return({struct _tuple10*_tmp5D8=_cycalloc(sizeof(*_tmp5D8));_tmp5D8->f1=(*x).f1;_tmp5D8->f2=(*x).f2;_tmp5D8->f3=(*x).f3;_tmp5D8;});}
# 3097
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3104
struct Cyc_List_List*_tmp5D9=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_List_List*_tmpD29=({struct Cyc_List_List*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->hd=(void*)atts->hd;_tmp5DA->tl=_tmp5D9;_tmp5DA;});_tmp5D9=_tmpD29;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD2C=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp5DC;_tmp5DC.tag=9;({struct Cyc_Absyn_FnInfo _tmpD2B=({struct Cyc_Absyn_FnInfo _tmp5DD;_tmp5DD.tvars=fd->tvs;_tmp5DD.effect=fd->effect;_tmp5DD.ret_tqual=fd->ret_tqual;_tmp5DD.ret_typ=fd->ret_type;({struct Cyc_List_List*_tmpD2A=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp5DD.args=_tmpD2A;});_tmp5DD.c_varargs=fd->c_varargs;_tmp5DD.cyc_varargs=fd->cyc_varargs;_tmp5DD.rgn_po=fd->rgn_po;_tmp5DD.attributes=_tmp5D9;_tmp5DD.requires_clause=fd->requires_clause;_tmp5DD.requires_relns=fd->requires_relns;_tmp5DD.ensures_clause=fd->ensures_clause;_tmp5DD.ensures_relns=fd->ensures_relns;_tmp5DD;});_tmp5DC.f1=_tmpD2B;});_tmp5DC;});_tmp5DB[0]=_tmpD2C;});_tmp5DB;});}
# 3117
return(void*)_check_null(fd->cached_typ);}
# 3123
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3125
union Cyc_Relations_RelnOp _tmp5DE=*rop;union Cyc_Relations_RelnOp _tmp5DF=_tmp5DE;struct Cyc_Absyn_Vardecl*_tmp5F2;struct Cyc_Absyn_Vardecl*_tmp5F1;switch((_tmp5DF.RNumelts).tag){case 2: _LL59C: _tmp5F1=(_tmp5DF.RVar).val;_LL59D: {
# 3127
struct _tuple2 _tmp5E0=*_tmp5F1->name;struct _tuple2 _tmp5E1=_tmp5E0;union Cyc_Absyn_Nmspace _tmp5E8;struct _dyneither_ptr*_tmp5E7;_LL5A3: _tmp5E8=_tmp5E1.f1;_tmp5E7=_tmp5E1.f2;_LL5A4:;
if(!((int)((_tmp5E8.Loc_n).tag == 4)))goto _LL59B;
if(({({struct _dyneither_ptr _tmpD2D=(struct _dyneither_ptr)*_tmp5E7;Cyc_strcmp(_tmpD2D,({const char*_tmp5E2="return_value";_tag_dyneither(_tmp5E2,sizeof(char),13);}));})== 0;})){
({union Cyc_Relations_RelnOp _tmpD2E=Cyc_Relations_RReturn();*rop=_tmpD2E;});
goto _LL59B;}{
# 3133
unsigned int c=0;
{struct Cyc_List_List*_tmp5E3=args;for(0;_tmp5E3 != 0;(_tmp5E3=_tmp5E3->tl,c ++)){
struct _tuple10*_tmp5E4=(struct _tuple10*)_tmp5E3->hd;struct _tuple10*_tmp5E5=_tmp5E4;struct _dyneither_ptr*_tmp5E6;_LL5A6: _tmp5E6=_tmp5E5->f1;_LL5A7:;
if(_tmp5E6 != 0){
if(({Cyc_strcmp((struct _dyneither_ptr)*_tmp5E7,(struct _dyneither_ptr)*_tmp5E6)== 0;})){
({union Cyc_Relations_RelnOp _tmpD2F=Cyc_Relations_RParam(c);*rop=_tmpD2F;});
break;}}}}
# 3143
goto _LL59B;};}case 3: _LL59E: _tmp5F2=(_tmp5DF.RNumelts).val;_LL59F: {
# 3145
struct _tuple2 _tmp5E9=*_tmp5F2->name;struct _tuple2 _tmp5EA=_tmp5E9;union Cyc_Absyn_Nmspace _tmp5F0;struct _dyneither_ptr*_tmp5EF;_LL5A9: _tmp5F0=_tmp5EA.f1;_tmp5EF=_tmp5EA.f2;_LL5AA:;
if(!((int)((_tmp5F0.Loc_n).tag == 4)))goto _LL59B;{
unsigned int c=0;
{struct Cyc_List_List*_tmp5EB=args;for(0;_tmp5EB != 0;(_tmp5EB=_tmp5EB->tl,c ++)){
struct _tuple10*_tmp5EC=(struct _tuple10*)_tmp5EB->hd;struct _tuple10*_tmp5ED=_tmp5EC;struct _dyneither_ptr*_tmp5EE;_LL5AC: _tmp5EE=_tmp5ED->f1;_LL5AD:;
if(_tmp5EE != 0){
if(({Cyc_strcmp((struct _dyneither_ptr)*_tmp5EF,(struct _dyneither_ptr)*_tmp5EE)== 0;})){
({union Cyc_Relations_RelnOp _tmpD30=Cyc_Relations_RParamNumelts(c);*rop=_tmpD30;});
break;}}}}
# 3157
goto _LL59B;};}default: _LL5A0: _LL5A1:
 goto _LL59B;}_LL59B:;}
# 3162
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3164
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3168
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3170
if(e == 0)return 0;{
struct Cyc_List_List*_tmp5F3=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp5F3);
return _tmp5F3;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3177
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp5F4=pr;struct Cyc_Absyn_Tqual _tmp5F7;void*_tmp5F6;_LL5AF: _tmp5F7=_tmp5F4->f1;_tmp5F6=_tmp5F4->f2;_LL5B0:;
if(_tmp5F6 == t)return pr;else{
return({struct _tuple12*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->f1=_tmp5F7;_tmp5F5->f2=t;_tmp5F5;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3184
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3186
return({struct _tuple27*_tmp5F8=_region_malloc(rgn,sizeof(*_tmp5F8));({struct _tuple26*_tmpD31=({struct _tuple26*_tmp5F9=_region_malloc(rgn,sizeof(*_tmp5F9));_tmp5F9->f1=(*y).f1;_tmp5F9->f2=(*y).f2;_tmp5F9;});_tmp5F8->f1=_tmpD31;});_tmp5F8->f2=(*y).f3;_tmp5F8;});}
# 3188
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3190
struct _tuple10 _tmp5FA=*orig_arg;struct _tuple10 _tmp5FB=_tmp5FA;struct _dyneither_ptr*_tmp5FF;struct Cyc_Absyn_Tqual _tmp5FE;void*_tmp5FD;_LL5B2: _tmp5FF=_tmp5FB.f1;_tmp5FE=_tmp5FB.f2;_tmp5FD=_tmp5FB.f3;_LL5B3:;
if(t == _tmp5FD)return orig_arg;
return({struct _tuple10*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->f1=_tmp5FF;_tmp5FC->f2=_tmp5FE;_tmp5FC->f3=t;_tmp5FC;});}
# 3194
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3197
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3202
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3204
return({struct Cyc_Absyn_Exp*_tmp600=_cycalloc(sizeof(*_tmp600));_tmp600->topt=old->topt;_tmp600->r=r;_tmp600->loc=old->loc;_tmp600->annot=old->annot;_tmp600;});}
# 3209
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp601=e->r;void*_tmp602=_tmp601;void*_tmp64A;void*_tmp649;struct Cyc_List_List*_tmp648;struct Cyc_Absyn_Exp*_tmp647;void*_tmp646;void*_tmp645;struct Cyc_Absyn_Exp*_tmp644;int _tmp643;enum Cyc_Absyn_Coercion _tmp642;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Exp*_tmp63A;struct Cyc_Absyn_Exp*_tmp639;enum Cyc_Absyn_Primop _tmp638;struct Cyc_List_List*_tmp637;switch(*((int*)_tmp602)){case 0: _LL5B5: _LL5B6:
 goto _LL5B8;case 31: _LL5B7: _LL5B8:
 goto _LL5BA;case 32: _LL5B9: _LL5BA:
 goto _LL5BC;case 1: _LL5BB: _LL5BC:
 return e;case 2: _LL5BD: _tmp638=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_tmp637=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp602)->f2;_LL5BE:
# 3217
 if(({((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp637)== 1;})){
struct Cyc_Absyn_Exp*_tmp603=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp637))->hd;
struct Cyc_Absyn_Exp*_tmp604=Cyc_Tcutil_rsubsexp(r,inst,_tmp603);
if(_tmp604 == _tmp603)return e;
return({struct Cyc_Absyn_Exp*_tmpD34=e;Cyc_Tcutil_copye(_tmpD34,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp605=_cycalloc(sizeof(*_tmp605));({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpD33=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp606;_tmp606.tag=2;_tmp606.f1=_tmp638;({struct Cyc_List_List*_tmpD32=({struct Cyc_Absyn_Exp*_tmp607[1];_tmp607[0]=_tmp604;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp607,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp606.f2=_tmpD32;});_tmp606;});_tmp605[0]=_tmpD33;});_tmp605;}));});}else{
if(({((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp637)== 2;})){
struct Cyc_Absyn_Exp*_tmp608=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp637))->hd;
struct Cyc_Absyn_Exp*_tmp609=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp637->tl))->hd;
struct Cyc_Absyn_Exp*_tmp60A=Cyc_Tcutil_rsubsexp(r,inst,_tmp608);
struct Cyc_Absyn_Exp*_tmp60B=Cyc_Tcutil_rsubsexp(r,inst,_tmp609);
if(_tmp60A == _tmp608  && _tmp60B == _tmp609)return e;
return({struct Cyc_Absyn_Exp*_tmpD37=e;Cyc_Tcutil_copye(_tmpD37,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp60C=_cycalloc(sizeof(*_tmp60C));({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpD36=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp60D;_tmp60D.tag=2;_tmp60D.f1=_tmp638;({struct Cyc_List_List*_tmpD35=({struct Cyc_Absyn_Exp*_tmp60E[2];_tmp60E[1]=_tmp60B;_tmp60E[0]=_tmp60A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60E,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp60D.f2=_tmpD35;});_tmp60D;});_tmp60C[0]=_tmpD36;});_tmp60C;}));});}else{
return({void*_tmp60F=0;({struct _dyneither_ptr _tmpD38=({const char*_tmp610="primop does not have 1 or 2 args!";_tag_dyneither(_tmp610,sizeof(char),34);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD38,_tag_dyneither(_tmp60F,sizeof(void*),0));});});}}case 5: _LL5BF: _tmp63B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_tmp63A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp602)->f2;_tmp639=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp602)->f3;_LL5C0: {
# 3231
struct Cyc_Absyn_Exp*_tmp611=Cyc_Tcutil_rsubsexp(r,inst,_tmp63B);
struct Cyc_Absyn_Exp*_tmp612=Cyc_Tcutil_rsubsexp(r,inst,_tmp63A);
struct Cyc_Absyn_Exp*_tmp613=Cyc_Tcutil_rsubsexp(r,inst,_tmp639);
if((_tmp611 == _tmp63B  && _tmp612 == _tmp63A) && _tmp613 == _tmp639)return e;
return({struct Cyc_Absyn_Exp*_tmpD3A=e;Cyc_Tcutil_copye(_tmpD3A,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp614=_cycalloc(sizeof(*_tmp614));({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpD39=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp615;_tmp615.tag=5;_tmp615.f1=_tmp611;_tmp615.f2=_tmp612;_tmp615.f3=_tmp613;_tmp615;});_tmp614[0]=_tmpD39;});_tmp614;}));});}case 6: _LL5C1: _tmp63D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_tmp63C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp602)->f2;_LL5C2: {
# 3237
struct Cyc_Absyn_Exp*_tmp616=Cyc_Tcutil_rsubsexp(r,inst,_tmp63D);
struct Cyc_Absyn_Exp*_tmp617=Cyc_Tcutil_rsubsexp(r,inst,_tmp63C);
if(_tmp616 == _tmp63D  && _tmp617 == _tmp63C)return e;
return({struct Cyc_Absyn_Exp*_tmpD3C=e;Cyc_Tcutil_copye(_tmpD3C,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp618=_cycalloc(sizeof(*_tmp618));({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmpD3B=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp619;_tmp619.tag=6;_tmp619.f1=_tmp616;_tmp619.f2=_tmp617;_tmp619;});_tmp618[0]=_tmpD3B;});_tmp618;}));});}case 7: _LL5C3: _tmp63F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_tmp63E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp602)->f2;_LL5C4: {
# 3242
struct Cyc_Absyn_Exp*_tmp61A=Cyc_Tcutil_rsubsexp(r,inst,_tmp63F);
struct Cyc_Absyn_Exp*_tmp61B=Cyc_Tcutil_rsubsexp(r,inst,_tmp63E);
if(_tmp61A == _tmp63F  && _tmp61B == _tmp63E)return e;
return({struct Cyc_Absyn_Exp*_tmpD3E=e;Cyc_Tcutil_copye(_tmpD3E,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp61C=_cycalloc(sizeof(*_tmp61C));({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmpD3D=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp61D;_tmp61D.tag=7;_tmp61D.f1=_tmp61A;_tmp61D.f2=_tmp61B;_tmp61D;});_tmp61C[0]=_tmpD3D;});_tmp61C;}));});}case 8: _LL5C5: _tmp641=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_tmp640=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp602)->f2;_LL5C6: {
# 3247
struct Cyc_Absyn_Exp*_tmp61E=Cyc_Tcutil_rsubsexp(r,inst,_tmp641);
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Tcutil_rsubsexp(r,inst,_tmp640);
if(_tmp61E == _tmp641  && _tmp61F == _tmp640)return e;
return({struct Cyc_Absyn_Exp*_tmpD40=e;Cyc_Tcutil_copye(_tmpD40,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp620=_cycalloc(sizeof(*_tmp620));({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmpD3F=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp621;_tmp621.tag=8;_tmp621.f1=_tmp61E;_tmp621.f2=_tmp61F;_tmp621;});_tmp620[0]=_tmpD3F;});_tmp620;}));});}case 13: _LL5C7: _tmp645=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_tmp644=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp602)->f2;_tmp643=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp602)->f3;_tmp642=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp602)->f4;_LL5C8: {
# 3252
struct Cyc_Absyn_Exp*_tmp622=Cyc_Tcutil_rsubsexp(r,inst,_tmp644);
void*_tmp623=Cyc_Tcutil_rsubstitute(r,inst,_tmp645);
if(_tmp622 == _tmp644  && _tmp623 == _tmp645)return e;
return({struct Cyc_Absyn_Exp*_tmpD42=e;Cyc_Tcutil_copye(_tmpD42,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp624=_cycalloc(sizeof(*_tmp624));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpD41=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp625;_tmp625.tag=13;_tmp625.f1=_tmp623;_tmp625.f2=_tmp622;_tmp625.f3=_tmp643;_tmp625.f4=_tmp642;_tmp625;});_tmp624[0]=_tmpD41;});_tmp624;}));});}case 16: _LL5C9: _tmp646=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_LL5CA: {
# 3257
void*_tmp626=Cyc_Tcutil_rsubstitute(r,inst,_tmp646);
if(_tmp626 == _tmp646)return e;
return({struct Cyc_Absyn_Exp*_tmpD44=e;Cyc_Tcutil_copye(_tmpD44,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp627=_cycalloc(sizeof(*_tmp627));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpD43=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp628;_tmp628.tag=16;_tmp628.f1=_tmp626;_tmp628;});_tmp627[0]=_tmpD43;});_tmp627;}));});}case 17: _LL5CB: _tmp647=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_LL5CC: {
# 3261
struct Cyc_Absyn_Exp*_tmp629=Cyc_Tcutil_rsubsexp(r,inst,_tmp647);
if(_tmp629 == _tmp647)return e;
return({struct Cyc_Absyn_Exp*_tmpD46=e;Cyc_Tcutil_copye(_tmpD46,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp62A=_cycalloc(sizeof(*_tmp62A));({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmpD45=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp62B;_tmp62B.tag=17;_tmp62B.f1=_tmp629;_tmp62B;});_tmp62A[0]=_tmpD45;});_tmp62A;}));});}case 18: _LL5CD: _tmp649=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_tmp648=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp602)->f2;_LL5CE: {
# 3265
void*_tmp62C=Cyc_Tcutil_rsubstitute(r,inst,_tmp649);
if(_tmp62C == _tmp649)return e;
return({struct Cyc_Absyn_Exp*_tmpD48=e;Cyc_Tcutil_copye(_tmpD48,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp62D=_cycalloc(sizeof(*_tmp62D));({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpD47=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp62E;_tmp62E.tag=18;_tmp62E.f1=_tmp62C;_tmp62E.f2=_tmp648;_tmp62E;});_tmp62D[0]=_tmpD47;});_tmp62D;}));});}case 38: _LL5CF: _tmp64A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp602)->f1;_LL5D0: {
# 3269
void*_tmp62F=Cyc_Tcutil_rsubstitute(r,inst,_tmp64A);
if(_tmp62F == _tmp64A)return e;{
# 3272
void*_tmp630=Cyc_Tcutil_compress(_tmp62F);void*_tmp631=_tmp630;struct Cyc_Absyn_Exp*_tmp634;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp631)->tag == 18){_LL5D4: _tmp634=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp631)->f1;_LL5D5:
 return _tmp634;}else{_LL5D6: _LL5D7:
# 3275
 return({struct Cyc_Absyn_Exp*_tmpD4A=e;Cyc_Tcutil_copye(_tmpD4A,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp632=_cycalloc(sizeof(*_tmp632));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmpD49=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp633;_tmp633.tag=38;_tmp633.f1=_tmp62F;_tmp633;});_tmp632[0]=_tmpD49;});_tmp632;}));});}_LL5D3:;};}default: _LL5D1: _LL5D2:
# 3278
 return({void*_tmp635=0;({struct _dyneither_ptr _tmpD4B=({const char*_tmp636="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp636,sizeof(char),46);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD4B,_tag_dyneither(_tmp635,sizeof(void*),0));});});}_LL5B4:;}
# 3282
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3285
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3289
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3292
void*_tmp64B=f->type;
struct Cyc_Absyn_Exp*_tmp64C=f->requires_clause;
void*_tmp64D=Cyc_Tcutil_rsubstitute(r,inst,_tmp64B);
struct Cyc_Absyn_Exp*_tmp64E=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp64C);
if(_tmp64B == _tmp64D  && _tmp64C == _tmp64E)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->name=f->name;_tmp64F->tq=f->tq;_tmp64F->type=_tmp64D;_tmp64F->width=f->width;_tmp64F->attributes=f->attributes;_tmp64F->requires_clause=_tmp64E;_tmp64F;});}}
# 3302
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3305
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp650=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp651=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp652=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp650);
struct Cyc_List_List*_tmp653=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp651);
if(_tmp652 == _tmp650  && _tmp653 == _tmp651)return fs;
# 3312
return({struct Cyc_List_List*_tmp654=_cycalloc(sizeof(*_tmp654));_tmp654->hd=_tmp652;_tmp654->tl=_tmp653;_tmp654;});};}
# 3315
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3318
struct _tuple1 _tmp655=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp656=_tmp655;struct Cyc_List_List*_tmp65A;struct Cyc_List_List*_tmp659;_LL5D9: _tmp65A=_tmp656.f1;_tmp659=_tmp656.f2;_LL5DA:;{
struct Cyc_List_List*_tmp657=Cyc_Tcutil_substs(rgn,inst,_tmp65A);
struct Cyc_List_List*_tmp658=Cyc_Tcutil_substs(rgn,inst,_tmp659);
if(_tmp657 == _tmp65A  && _tmp658 == _tmp659)
return rgn_po;else{
# 3324
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp657,_tmp658);}};}
# 3327
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3330
void*_tmp65B=Cyc_Tcutil_compress(t);void*_tmp65C=_tmp65B;struct Cyc_List_List*_tmp6ED;void*_tmp6EC;void*_tmp6EB;struct Cyc_Absyn_Exp*_tmp6EA;struct Cyc_Absyn_Exp*_tmp6E9;void*_tmp6E8;void*_tmp6E7;void*_tmp6E6;void*_tmp6E5;void*_tmp6E4;enum Cyc_Absyn_AggrKind _tmp6E3;struct Cyc_List_List*_tmp6E2;struct Cyc_List_List*_tmp6E1;struct Cyc_List_List*_tmp6E0;void*_tmp6DF;struct Cyc_Absyn_Tqual _tmp6DE;void*_tmp6DD;struct Cyc_List_List*_tmp6DC;int _tmp6DB;struct Cyc_Absyn_VarargInfo*_tmp6DA;struct Cyc_List_List*_tmp6D9;struct Cyc_List_List*_tmp6D8;struct Cyc_Absyn_Exp*_tmp6D7;struct Cyc_Absyn_Exp*_tmp6D6;void*_tmp6D5;struct Cyc_Absyn_Tqual _tmp6D4;void*_tmp6D3;union Cyc_Absyn_Constraint*_tmp6D2;union Cyc_Absyn_Constraint*_tmp6D1;union Cyc_Absyn_Constraint*_tmp6D0;void*_tmp6CF;struct Cyc_Absyn_Tqual _tmp6CE;struct Cyc_Absyn_Exp*_tmp6CD;union Cyc_Absyn_Constraint*_tmp6CC;unsigned int _tmp6CB;struct _tuple2*_tmp6CA;struct Cyc_List_List*_tmp6C9;struct Cyc_Absyn_Typedefdecl*_tmp6C8;void*_tmp6C7;union Cyc_Absyn_DatatypeFieldInfoU _tmp6C6;struct Cyc_List_List*_tmp6C5;union Cyc_Absyn_DatatypeInfoU _tmp6C4;struct Cyc_List_List*_tmp6C3;union Cyc_Absyn_AggrInfoU _tmp6C2;struct Cyc_List_List*_tmp6C1;struct Cyc_Absyn_Tvar*_tmp6C0;switch(*((int*)_tmp65C)){case 2: _LL5DC: _tmp6C0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp65C)->f1;_LL5DD: {
# 3333
struct _handler_cons _tmp65D;_push_handler(& _tmp65D);{int _tmp65F=0;if(setjmp(_tmp65D.handler))_tmp65F=1;if(!_tmp65F){{void*_tmp660=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6C0);_npop_handler(0);return _tmp660;};_pop_handler();}else{void*_tmp65E=(void*)_exn_thrown;void*_tmp661=_tmp65E;void*_tmp662;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp661)->tag == Cyc_Core_Not_found){_LL617: _LL618:
 return t;}else{_LL619: _tmp662=_tmp661;_LL61A:(int)_rethrow(_tmp662);}_LL616:;}};}case 11: _LL5DE: _tmp6C2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65C)->f1).aggr_info;_tmp6C1=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65C)->f1).targs;_LL5DF: {
# 3336
struct Cyc_List_List*_tmp663=Cyc_Tcutil_substs(rgn,inst,_tmp6C1);
return _tmp663 == _tmp6C1?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp664=_cycalloc(sizeof(*_tmp664));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpD4D=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp665;_tmp665.tag=11;({struct Cyc_Absyn_AggrInfo _tmpD4C=({struct Cyc_Absyn_AggrInfo _tmp666;_tmp666.aggr_info=_tmp6C2;_tmp666.targs=_tmp663;_tmp666;});_tmp665.f1=_tmpD4C;});_tmp665;});_tmp664[0]=_tmpD4D;});_tmp664;});}case 3: _LL5E0: _tmp6C4=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp65C)->f1).datatype_info;_tmp6C3=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp65C)->f1).targs;_LL5E1: {
# 3339
struct Cyc_List_List*_tmp667=Cyc_Tcutil_substs(rgn,inst,_tmp6C3);
return _tmp667 == _tmp6C3?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp668=_cycalloc(sizeof(*_tmp668));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpD4F=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp669;_tmp669.tag=3;({struct Cyc_Absyn_DatatypeInfo _tmpD4E=({struct Cyc_Absyn_DatatypeInfo _tmp66A;_tmp66A.datatype_info=_tmp6C4;_tmp66A.targs=_tmp667;_tmp66A;});_tmp669.f1=_tmpD4E;});_tmp669;});_tmp668[0]=_tmpD4F;});_tmp668;});}case 4: _LL5E2: _tmp6C6=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp65C)->f1).field_info;_tmp6C5=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp65C)->f1).targs;_LL5E3: {
# 3343
struct Cyc_List_List*_tmp66B=Cyc_Tcutil_substs(rgn,inst,_tmp6C5);
return _tmp66B == _tmp6C5?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp66C=_cycalloc(sizeof(*_tmp66C));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpD51=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp66D;_tmp66D.tag=4;({struct Cyc_Absyn_DatatypeFieldInfo _tmpD50=({struct Cyc_Absyn_DatatypeFieldInfo _tmp66E;_tmp66E.field_info=_tmp6C6;_tmp66E.targs=_tmp66B;_tmp66E;});_tmp66D.f1=_tmpD50;});_tmp66D;});_tmp66C[0]=_tmpD51;});_tmp66C;});}case 17: _LL5E4: _tmp6CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65C)->f1;_tmp6C9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65C)->f2;_tmp6C8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65C)->f3;_tmp6C7=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65C)->f4;_LL5E5: {
# 3347
struct Cyc_List_List*_tmp66F=Cyc_Tcutil_substs(rgn,inst,_tmp6C9);
return _tmp66F == _tmp6C9?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp670=_cycalloc(sizeof(*_tmp670));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpD52=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp671;_tmp671.tag=17;_tmp671.f1=_tmp6CA;_tmp671.f2=_tmp66F;_tmp671.f3=_tmp6C8;_tmp671.f4=_tmp6C7;_tmp671;});_tmp670[0]=_tmpD52;});_tmp670;});}case 8: _LL5E6: _tmp6CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65C)->f1).elt_type;_tmp6CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65C)->f1).tq;_tmp6CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65C)->f1).num_elts;_tmp6CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65C)->f1).zero_term;_tmp6CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65C)->f1).zt_loc;_LL5E7: {
# 3350
void*_tmp672=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6CF);
struct Cyc_Absyn_Exp*_tmp673=_tmp6CD == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6CD);
return _tmp672 == _tmp6CF  && _tmp673 == _tmp6CD?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp674=_cycalloc(sizeof(*_tmp674));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD54=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp675;_tmp675.tag=8;({struct Cyc_Absyn_ArrayInfo _tmpD53=({struct Cyc_Absyn_ArrayInfo _tmp676;_tmp676.elt_type=_tmp672;_tmp676.tq=_tmp6CE;_tmp676.num_elts=_tmp673;_tmp676.zero_term=_tmp6CC;_tmp676.zt_loc=_tmp6CB;_tmp676;});_tmp675.f1=_tmpD53;});_tmp675;});_tmp674[0]=_tmpD54;});_tmp674;});}case 5: _LL5E8: _tmp6D5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65C)->f1).elt_typ;_tmp6D4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65C)->f1).elt_tq;_tmp6D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65C)->f1).ptr_atts).rgn;_tmp6D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65C)->f1).ptr_atts).nullable;_tmp6D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65C)->f1).ptr_atts).bounds;_tmp6D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65C)->f1).ptr_atts).zero_term;_LL5E9: {
# 3355
void*_tmp677=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D5);
void*_tmp678=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D3);
union Cyc_Absyn_Constraint*_tmp679=_tmp6D1;
{void*_tmp67A=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6D1);void*_tmp67B=_tmp67A;struct Cyc_Absyn_Exp*_tmp67F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp67B)->tag == 1){_LL61C: _tmp67F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp67B)->f1;_LL61D: {
# 3360
struct Cyc_Absyn_Exp*_tmp67C=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp67F);
if(_tmp67C != _tmp67F)
({union Cyc_Absyn_Constraint*_tmpD56=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp67D=_cycalloc(sizeof(*_tmp67D));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpD55=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp67E;_tmp67E.tag=1;_tmp67E.f1=_tmp67C;_tmp67E;});_tmp67D[0]=_tmpD55;});_tmp67D;}));_tmp679=_tmpD56;});
goto _LL61B;}}else{_LL61E: _LL61F:
 goto _LL61B;}_LL61B:;}
# 3366
if((_tmp677 == _tmp6D5  && _tmp678 == _tmp6D3) && _tmp679 == _tmp6D1)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp680=_cycalloc(sizeof(*_tmp680));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpD59=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp681;_tmp681.tag=5;({struct Cyc_Absyn_PtrInfo _tmpD58=({struct Cyc_Absyn_PtrInfo _tmp682;_tmp682.elt_typ=_tmp677;_tmp682.elt_tq=_tmp6D4;({struct Cyc_Absyn_PtrAtts _tmpD57=({(_tmp682.ptr_atts).rgn=_tmp678;(_tmp682.ptr_atts).nullable=_tmp6D2;(_tmp682.ptr_atts).bounds=_tmp679;(_tmp682.ptr_atts).zero_term=_tmp6D0;(_tmp682.ptr_atts).ptrloc=0;_tmp682.ptr_atts;});_tmp682.ptr_atts=_tmpD57;});_tmp682;});_tmp681.f1=_tmpD58;});_tmp681;});_tmp680[0]=_tmpD59;});_tmp680;});}case 9: _LL5EA: _tmp6E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).tvars;_tmp6DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).effect;_tmp6DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).ret_tqual;_tmp6DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).ret_typ;_tmp6DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).args;_tmp6DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).c_varargs;_tmp6DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).cyc_varargs;_tmp6D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).rgn_po;_tmp6D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).attributes;_tmp6D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).requires_clause;_tmp6D6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65C)->f1).ensures_clause;_LL5EB:
# 3372
{struct Cyc_List_List*_tmp683=_tmp6E0;for(0;_tmp683 != 0;_tmp683=_tmp683->tl){
({struct Cyc_List_List*_tmpD5D=({struct Cyc_List_List*_tmp684=_region_malloc(rgn,sizeof(*_tmp684));({struct _tuple16*_tmpD5C=({struct _tuple16*_tmp685=_region_malloc(rgn,sizeof(*_tmp685));_tmp685->f1=(struct Cyc_Absyn_Tvar*)_tmp683->hd;({void*_tmpD5B=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp686=_cycalloc(sizeof(*_tmp686));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD5A=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp687;_tmp687.tag=2;_tmp687.f1=(struct Cyc_Absyn_Tvar*)_tmp683->hd;_tmp687;});_tmp686[0]=_tmpD5A;});_tmp686;});_tmp685->f2=_tmpD5B;});_tmp685;});_tmp684->hd=_tmpD5C;});_tmp684->tl=inst;_tmp684;});inst=_tmpD5D;});}}{
struct _tuple1 _tmp688=({struct _RegionHandle*_tmpD5F=rgn;struct _RegionHandle*_tmpD5E=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpD5F,_tmpD5E,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6DC));});
# 3374
struct _tuple1 _tmp689=_tmp688;struct Cyc_List_List*_tmp69A;struct Cyc_List_List*_tmp699;_LL621: _tmp69A=_tmp689.f1;_tmp699=_tmp689.f2;_LL622:;{
# 3376
struct Cyc_List_List*_tmp68A=_tmp6DC;
struct Cyc_List_List*_tmp68B=Cyc_Tcutil_substs(rgn,inst,_tmp699);
if(_tmp68B != _tmp699)
({struct Cyc_List_List*_tmpD60=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp6DC,_tmp68B);_tmp68A=_tmpD60;});{
void*eff2;
if(_tmp6DF == 0)
eff2=0;else{
# 3384
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DF);
if(new_eff == _tmp6DF)
eff2=_tmp6DF;else{
# 3388
eff2=new_eff;}}{
# 3390
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp6DA == 0)
cyc_varargs2=0;else{
# 3394
struct Cyc_Absyn_VarargInfo _tmp68C=*_tmp6DA;struct Cyc_Absyn_VarargInfo _tmp68D=_tmp68C;struct _dyneither_ptr*_tmp693;struct Cyc_Absyn_Tqual _tmp692;void*_tmp691;int _tmp690;_LL624: _tmp693=_tmp68D.name;_tmp692=_tmp68D.tq;_tmp691=_tmp68D.type;_tmp690=_tmp68D.inject;_LL625:;{
void*_tmp68E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp691);
if(_tmp68E == _tmp691)cyc_varargs2=_tmp6DA;else{
# 3398
({struct Cyc_Absyn_VarargInfo*_tmpD61=({struct Cyc_Absyn_VarargInfo*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F->name=_tmp693;_tmp68F->tq=_tmp692;_tmp68F->type=_tmp68E;_tmp68F->inject=_tmp690;_tmp68F;});cyc_varargs2=_tmpD61;});}};}{
# 3400
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp6D9);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6D7);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6D6);
struct Cyc_List_List*_tmp694=Cyc_Tcutil_extract_relns(_tmp68A,req2);
struct Cyc_List_List*_tmp695=Cyc_Tcutil_extract_relns(_tmp68A,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp696=_cycalloc(sizeof(*_tmp696));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD64=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp697;_tmp697.tag=9;({struct Cyc_Absyn_FnInfo _tmpD63=({struct Cyc_Absyn_FnInfo _tmp698;_tmp698.tvars=_tmp6E0;_tmp698.effect=eff2;_tmp698.ret_tqual=_tmp6DE;({void*_tmpD62=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DD);_tmp698.ret_typ=_tmpD62;});_tmp698.args=_tmp68A;_tmp698.c_varargs=_tmp6DB;_tmp698.cyc_varargs=cyc_varargs2;_tmp698.rgn_po=rgn_po2;_tmp698.attributes=_tmp6D8;_tmp698.requires_clause=req2;_tmp698.requires_relns=_tmp694;_tmp698.ensures_clause=ens2;_tmp698.ensures_relns=_tmp695;_tmp698;});_tmp697.f1=_tmpD63;});_tmp697;});_tmp696[0]=_tmpD64;});_tmp696;});};};};};};case 10: _LL5EC: _tmp6E1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp65C)->f1;_LL5ED: {
# 3410
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp69B=_tmp6E1;for(0;_tmp69B != 0;_tmp69B=_tmp69B->tl){
void*_tmp69C=(*((struct _tuple12*)_tmp69B->hd)).f2;
void*_tmp69D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69C);
if(_tmp69C != _tmp69D)
change=1;
# 3418
({struct Cyc_List_List*_tmpD65=({struct Cyc_List_List*_tmp69E=_region_malloc(rgn,sizeof(*_tmp69E));_tmp69E->hd=_tmp69D;_tmp69E->tl=ts2;_tmp69E;});ts2=_tmpD65;});}}
# 3420
if(!change)
return t;{
struct Cyc_List_List*_tmp69F=({struct Cyc_List_List*_tmpD66=_tmp6E1;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpD66,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6A0=_cycalloc(sizeof(*_tmp6A0));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpD67=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6A1;_tmp6A1.tag=10;_tmp6A1.f1=_tmp69F;_tmp6A1;});_tmp6A0[0]=_tmpD67;});_tmp6A0;});};}case 12: _LL5EE: _tmp6E3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp65C)->f1;_tmp6E2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp65C)->f2;_LL5EF: {
# 3425
struct Cyc_List_List*_tmp6A2=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp6E2);
if(_tmp6E2 == _tmp6A2)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6A3=_cycalloc(sizeof(*_tmp6A3));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpD68=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6A4;_tmp6A4.tag=12;_tmp6A4.f1=_tmp6E3;_tmp6A4.f2=_tmp6A2;_tmp6A4;});_tmp6A3[0]=_tmpD68;});_tmp6A3;});}case 1: _LL5F0: _tmp6E4=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp65C)->f2;_LL5F1:
# 3429
 if(_tmp6E4 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E4);else{
return t;}case 15: _LL5F2: _tmp6E5=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp65C)->f1;_LL5F3: {
# 3432
void*_tmp6A5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E5);
return _tmp6A5 == _tmp6E5?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6A6=_cycalloc(sizeof(*_tmp6A6));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpD69=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6A7;_tmp6A7.tag=15;_tmp6A7.f1=_tmp6A5;_tmp6A7;});_tmp6A6[0]=_tmpD69;});_tmp6A6;});}case 16: _LL5F4: _tmp6E7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp65C)->f1;_tmp6E6=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp65C)->f2;_LL5F5: {
# 3435
void*_tmp6A8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E7);
void*_tmp6A9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E6);
return _tmp6A8 == _tmp6E7  && _tmp6A9 == _tmp6E6?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpD6A=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp6AB;_tmp6AB.tag=16;_tmp6AB.f1=_tmp6A8;_tmp6AB.f2=_tmp6A9;_tmp6AB;});_tmp6AA[0]=_tmpD6A;});_tmp6AA;});}case 19: _LL5F6: _tmp6E8=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp65C)->f1;_LL5F7: {
# 3439
void*_tmp6AC=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E8);
return _tmp6AC == _tmp6E8?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6AD=_cycalloc(sizeof(*_tmp6AD));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpD6B=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6AE;_tmp6AE.tag=19;_tmp6AE.f1=_tmp6AC;_tmp6AE;});_tmp6AD[0]=_tmpD6B;});_tmp6AD;});}case 18: _LL5F8: _tmp6E9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp65C)->f1;_LL5F9: {
# 3442
struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6E9);
return _tmp6AF == _tmp6E9?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpD6C=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6B1;_tmp6B1.tag=18;_tmp6B1.f1=_tmp6AF;_tmp6B1;});_tmp6B0[0]=_tmpD6C;});_tmp6B0;});}case 27: _LL5FA: _tmp6EA=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp65C)->f1;_LL5FB: {
# 3445
struct Cyc_Absyn_Exp*_tmp6B2=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6EA);
return _tmp6B2 == _tmp6EA?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp6B3=_cycalloc(sizeof(*_tmp6B3));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpD6D=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp6B4;_tmp6B4.tag=27;_tmp6B4.f1=_tmp6B2;_tmp6B4;});_tmp6B3[0]=_tmpD6D;});_tmp6B3;});}case 13: _LL5FC: _LL5FD:
 goto _LL5FF;case 14: _LL5FE: _LL5FF:
 goto _LL601;case 0: _LL600: _LL601:
 goto _LL603;case 6: _LL602: _LL603:
 goto _LL605;case 7: _LL604: _LL605:
 goto _LL607;case 22: _LL606: _LL607:
 goto _LL609;case 21: _LL608: _LL609:
 goto _LL60B;case 28: _LL60A: _LL60B:
 goto _LL60D;case 20: _LL60C: _LL60D:
 return t;case 25: _LL60E: _tmp6EB=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp65C)->f1;_LL60F: {
# 3457
void*_tmp6B5=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EB);
return _tmp6B5 == _tmp6EB?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD6E=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6B7;_tmp6B7.tag=25;_tmp6B7.f1=_tmp6B5;_tmp6B7;});_tmp6B6[0]=_tmpD6E;});_tmp6B6;});}case 23: _LL610: _tmp6EC=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp65C)->f1;_LL611: {
# 3460
void*_tmp6B8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EC);
return _tmp6B8 == _tmp6EC?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD6F=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp6BA;_tmp6BA.tag=23;_tmp6BA.f1=_tmp6B8;_tmp6BA;});_tmp6B9[0]=_tmpD6F;});_tmp6B9;});}case 24: _LL612: _tmp6ED=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp65C)->f1;_LL613: {
# 3463
struct Cyc_List_List*_tmp6BB=Cyc_Tcutil_substs(rgn,inst,_tmp6ED);
return _tmp6BB == _tmp6ED?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD70=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6BD;_tmp6BD.tag=24;_tmp6BD.f1=_tmp6BB;_tmp6BD;});_tmp6BC[0]=_tmpD70;});_tmp6BC;});}default: _LL614: _LL615:
({void*_tmp6BE=0;({struct _dyneither_ptr _tmpD71=({const char*_tmp6BF="found typedecltype in rsubs";_tag_dyneither(_tmp6BF,sizeof(char),28);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD71,_tag_dyneither(_tmp6BE,sizeof(void*),0));});});}_LL5DB:;}
# 3469
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3472
if(ts == 0)
return 0;{
void*_tmp6EE=(void*)ts->hd;
struct Cyc_List_List*_tmp6EF=ts->tl;
void*_tmp6F0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EE);
struct Cyc_List_List*_tmp6F1=Cyc_Tcutil_substs(rgn,inst,_tmp6EF);
if(_tmp6EE == _tmp6F0  && _tmp6EF == _tmp6F1)
return ts;
return({struct Cyc_List_List*_tmp6F2=_cycalloc(sizeof(*_tmp6F2));_tmp6F2->hd=_tmp6F0;_tmp6F2->tl=_tmp6F1;_tmp6F2;});};}
# 3483
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3490
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp6F3=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp6F4=_cycalloc(sizeof(*_tmp6F4));_tmp6F4->f1=tv;({void*_tmpD73=({struct Cyc_Core_Opt*_tmpD72=_tmp6F3;Cyc_Absyn_new_evar(_tmpD72,({struct Cyc_Core_Opt*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5->v=s;_tmp6F5;}));});_tmp6F4->f2=_tmpD73;});_tmp6F4;});}
# 3495
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3497
struct _tuple17*_tmp6F6=env;struct Cyc_List_List*_tmp6FB;struct _RegionHandle*_tmp6FA;_LL627: _tmp6FB=_tmp6F6->f1;_tmp6FA=_tmp6F6->f2;_LL628:;{
struct Cyc_Core_Opt*_tmp6F7=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp6F8=_region_malloc(_tmp6FA,sizeof(*_tmp6F8));_tmp6F8->f1=tv;({void*_tmpD75=({struct Cyc_Core_Opt*_tmpD74=_tmp6F7;Cyc_Absyn_new_evar(_tmpD74,({struct Cyc_Core_Opt*_tmp6F9=_cycalloc(sizeof(*_tmp6F9));_tmp6F9->v=_tmp6FB;_tmp6F9;}));});_tmp6F8->f2=_tmpD75;});_tmp6F8;});};}
# 3507
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3511
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(({Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0;})){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp700;_tmp700.tag=0;({struct _dyneither_ptr _tmpD76=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));_tmp700.f1=_tmpD76;});({struct Cyc_String_pa_PrintArg_struct _tmp6FF;_tmp6FF.tag=0;({struct _dyneither_ptr _tmpD77=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp6FF.f1=_tmpD77;});({struct Cyc_String_pa_PrintArg_struct _tmp6FE;_tmp6FE.tag=0;_tmp6FE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp6FC[3]={& _tmp6FE,& _tmp6FF,& _tmp700};({unsigned int _tmpD79=loc;struct _dyneither_ptr _tmpD78=({const char*_tmp6FD="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp6FD,sizeof(char),59);});Cyc_Tcutil_terr(_tmpD79,_tmpD78,_tag_dyneither(_tmp6FC,sizeof(void*),3));});});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp701=0;({struct _dyneither_ptr _tmpD7A=({const char*_tmp702="same type variable has different identity!";_tag_dyneither(_tmp702,sizeof(char),43);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD7A,_tag_dyneither(_tmp701,sizeof(void*),0));});});}
return tvs;}}}
# 3525
({int _tmpD7B=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpD7B;});
return({struct Cyc_List_List*_tmp703=_cycalloc(sizeof(*_tmp703));_tmp703->hd=tv;_tmp703->tl=tvs;_tmp703;});}
# 3531
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3533
if(tv->identity == - 1)
({void*_tmp704=0;({struct _dyneither_ptr _tmpD7C=({const char*_tmp705="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp705,sizeof(char),39);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD7C,_tag_dyneither(_tmp704,sizeof(void*),0));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3537
struct Cyc_Absyn_Tvar*_tmp706=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp706->identity == - 1)
({void*_tmp707=0;({struct _dyneither_ptr _tmpD7D=({const char*_tmp708="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp708,sizeof(char),41);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD7D,_tag_dyneither(_tmp707,sizeof(void*),0));});});
if(_tmp706->identity == tv->identity)
return tvs;}}
# 3544
return({struct Cyc_List_List*_tmp709=_cycalloc(sizeof(*_tmp709));_tmp709->hd=tv;_tmp709->tl=tvs;_tmp709;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3550
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3555
if(tv->identity == - 1)
({void*_tmp70A=0;({struct _dyneither_ptr _tmpD7E=({const char*_tmp70B="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp70B,sizeof(char),44);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD7E,_tag_dyneither(_tmp70A,sizeof(void*),0));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3559
struct _tuple28*_tmp70C=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp70D=_tmp70C;struct Cyc_Absyn_Tvar*_tmp711;int*_tmp710;_LL62A: _tmp711=_tmp70D->f1;_tmp710=(int*)& _tmp70D->f2;_LL62B:;
if(_tmp711->identity == - 1)
({void*_tmp70E=0;({struct _dyneither_ptr _tmpD7F=({const char*_tmp70F="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp70F,sizeof(char),46);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD7F,_tag_dyneither(_tmp70E,sizeof(void*),0));});});
if(_tmp711->identity == tv->identity){
*_tmp710=*_tmp710  || b;
return tvs;}}}
# 3567
return({struct Cyc_List_List*_tmp712=_region_malloc(r,sizeof(*_tmp712));({struct _tuple28*_tmpD80=({struct _tuple28*_tmp713=_region_malloc(r,sizeof(*_tmp713));_tmp713->f1=tv;_tmp713->f2=b;_tmp713;});_tmp712->hd=_tmpD80;});_tmp712->tl=tvs;_tmp712;});}
# 3571
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3573
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp716;_tmp716.tag=0;({struct _dyneither_ptr _tmpD81=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmp716.f1=_tmpD81;});({void*_tmp714[1]={& _tmp716};({struct _dyneither_ptr _tmpD82=({const char*_tmp715="bound tvar id for %s is NULL";_tag_dyneither(_tmp715,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD82,_tag_dyneither(_tmp714,sizeof(void*),1));});});});
return({struct Cyc_List_List*_tmp717=_cycalloc(sizeof(*_tmp717));_tmp717->hd=tv;_tmp717->tl=tvs;_tmp717;});}struct _tuple29{void*f1;int f2;};
# 3582
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3585
void*_tmp718=Cyc_Tcutil_compress(e);void*_tmp719=_tmp718;int _tmp723;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp719)->tag == 1){_LL62D: _tmp723=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp719)->f3;_LL62E:
# 3587
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp71A=(struct _tuple29*)es2->hd;struct _tuple29*_tmp71B=_tmp71A;void*_tmp720;int*_tmp71F;_LL632: _tmp720=_tmp71B->f1;_tmp71F=(int*)& _tmp71B->f2;_LL633:;{
void*_tmp71C=Cyc_Tcutil_compress(_tmp720);void*_tmp71D=_tmp71C;int _tmp71E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71D)->tag == 1){_LL635: _tmp71E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71D)->f3;_LL636:
# 3591
 if(_tmp723 == _tmp71E){
if(b != *_tmp71F)*_tmp71F=1;
return es;}
# 3595
goto _LL634;}else{_LL637: _LL638:
 goto _LL634;}_LL634:;};}}
# 3599
return({struct Cyc_List_List*_tmp721=_region_malloc(r,sizeof(*_tmp721));({struct _tuple29*_tmpD83=({struct _tuple29*_tmp722=_region_malloc(r,sizeof(*_tmp722));_tmp722->f1=e;_tmp722->f2=b;_tmp722;});_tmp721->hd=_tmpD83;});_tmp721->tl=es;_tmp721;});}else{_LL62F: _LL630:
 return es;}_LL62C:;}
# 3604
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3607
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3616
if(!present)({struct Cyc_List_List*_tmpD84=({struct Cyc_List_List*_tmp724=_region_malloc(rgn,sizeof(*_tmp724));_tmp724->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp724->tl=r;_tmp724;});r=_tmpD84;});}
# 3618
({struct Cyc_List_List*_tmpD85=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);r=_tmpD85;});
return r;}
# 3623
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3627
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp725=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp726=_tmp725;struct Cyc_Absyn_Tvar*_tmp729;int _tmp728;_LL63A: _tmp729=_tmp726.f1;_tmp728=_tmp726.f2;_LL63B:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp729->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3637
if(!present)({struct Cyc_List_List*_tmpD86=({struct Cyc_List_List*_tmp727=_region_malloc(r,sizeof(*_tmp727));_tmp727->hd=(struct _tuple28*)tvs->hd;_tmp727->tl=res;_tmp727;});res=_tmpD86;});};}
# 3639
({struct Cyc_List_List*_tmpD87=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);res=_tmpD87;});
return res;}
# 3643
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3645
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp72C;_tmp72C.tag=0;_tmp72C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp72A[1]={& _tmp72C};({unsigned int _tmpD89=loc;struct _dyneither_ptr _tmpD88=({const char*_tmp72B="bitfield %s does not have constant width";_tag_dyneither(_tmp72B,sizeof(char),41);});Cyc_Tcutil_terr(_tmpD89,_tmpD88,_tag_dyneither(_tmp72A,sizeof(void*),1));});});});else{
# 3650
struct _tuple14 _tmp72D=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp72E=_tmp72D;unsigned int _tmp732;int _tmp731;_LL63D: _tmp732=_tmp72E.f1;_tmp731=_tmp72E.f2;_LL63E:;
if(!_tmp731)
({void*_tmp72F=0;({unsigned int _tmpD8B=loc;struct _dyneither_ptr _tmpD8A=({const char*_tmp730="bitfield width cannot use sizeof or offsetof";_tag_dyneither(_tmp730,sizeof(char),45);});Cyc_Tcutil_terr(_tmpD8B,_tmpD8A,_tag_dyneither(_tmp72F,sizeof(void*),0));});});
w=_tmp732;}{
# 3655
void*_tmp733=Cyc_Tcutil_compress(field_typ);void*_tmp734=_tmp733;enum Cyc_Absyn_Size_of _tmp742;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp734)->tag == 6){_LL640: _tmp742=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp734)->f2;_LL641:
# 3658
{enum Cyc_Absyn_Size_of _tmp735=_tmp742;switch(_tmp735){case Cyc_Absyn_Char_sz: _LL645: _LL646:
 if(w > 8)({void*_tmp736=0;({unsigned int _tmpD8D=loc;struct _dyneither_ptr _tmpD8C=({const char*_tmp737="bitfield larger than type";_tag_dyneither(_tmp737,sizeof(char),26);});Cyc_Tcutil_terr(_tmpD8D,_tmpD8C,_tag_dyneither(_tmp736,sizeof(void*),0));});});goto _LL644;case Cyc_Absyn_Short_sz: _LL647: _LL648:
 if(w > 16)({void*_tmp738=0;({unsigned int _tmpD8F=loc;struct _dyneither_ptr _tmpD8E=({const char*_tmp739="bitfield larger than type";_tag_dyneither(_tmp739,sizeof(char),26);});Cyc_Tcutil_terr(_tmpD8F,_tmpD8E,_tag_dyneither(_tmp738,sizeof(void*),0));});});goto _LL644;case Cyc_Absyn_Long_sz: _LL649: _LL64A:
 goto _LL64C;case Cyc_Absyn_Int_sz: _LL64B: _LL64C:
# 3663
 if(w > 32)({void*_tmp73A=0;({unsigned int _tmpD91=loc;struct _dyneither_ptr _tmpD90=({const char*_tmp73B="bitfield larger than type";_tag_dyneither(_tmp73B,sizeof(char),26);});Cyc_Tcutil_terr(_tmpD91,_tmpD90,_tag_dyneither(_tmp73A,sizeof(void*),0));});});goto _LL644;default: _LL64D: _LL64E:
# 3665
 if(w > 64)({void*_tmp73C=0;({unsigned int _tmpD93=loc;struct _dyneither_ptr _tmpD92=({const char*_tmp73D="bitfield larger than type";_tag_dyneither(_tmp73D,sizeof(char),26);});Cyc_Tcutil_terr(_tmpD93,_tmpD92,_tag_dyneither(_tmp73C,sizeof(void*),0));});});goto _LL644;}_LL644:;}
# 3667
goto _LL63F;}else{_LL642: _LL643:
# 3669
({struct Cyc_String_pa_PrintArg_struct _tmp741;_tmp741.tag=0;({struct _dyneither_ptr _tmpD94=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));_tmp741.f1=_tmpD94;});({struct Cyc_String_pa_PrintArg_struct _tmp740;_tmp740.tag=0;_tmp740.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp73E[2]={& _tmp740,& _tmp741};({unsigned int _tmpD96=loc;struct _dyneither_ptr _tmpD95=({const char*_tmp73F="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp73F,sizeof(char),52);});Cyc_Tcutil_terr(_tmpD96,_tmpD95,_tag_dyneither(_tmp73E,sizeof(void*),2));});});});});
goto _LL63F;}_LL63F:;};}}
# 3676
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp743=(void*)atts->hd;void*_tmp744=_tmp743;switch(*((int*)_tmp744)){case 7: _LL650: _LL651:
 continue;case 6: _LL652: _LL653:
 continue;default: _LL654: _LL655:
({struct Cyc_String_pa_PrintArg_struct _tmp748;_tmp748.tag=0;_tmp748.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp747;_tmp747.tag=0;({struct _dyneither_ptr _tmpD97=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));_tmp747.f1=_tmpD97;});({void*_tmp745[2]={& _tmp747,& _tmp748};({unsigned int _tmpD99=loc;struct _dyneither_ptr _tmpD98=({const char*_tmp746="bad attribute %s on member %s";_tag_dyneither(_tmp746,sizeof(char),30);});Cyc_Tcutil_terr(_tmpD99,_tmpD98,_tag_dyneither(_tmp745,sizeof(void*),2));});});});});}_LL64F:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3700
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3704
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp749=t;struct Cyc_Absyn_Typedefdecl*_tmp74D;void*_tmp74C;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp749)->tag == 17){_LL657: _tmp74D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp749)->f3;_tmp74C=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp749)->f4;_LL658:
# 3707
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp74D))->tq).real_const  || (_tmp74D->tq).print_const){
if(declared_const)({void*_tmp74A=0;({unsigned int _tmpD9B=loc;struct _dyneither_ptr _tmpD9A=({const char*_tmp74B="extra const";_tag_dyneither(_tmp74B,sizeof(char),12);});Cyc_Tcutil_warn(_tmpD9B,_tmpD9A,_tag_dyneither(_tmp74A,sizeof(void*),0));});});
return 1;}
# 3712
if((unsigned int)_tmp74C)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp74C);else{
return declared_const;}}else{_LL659: _LL65A:
 return declared_const;}_LL656:;}
# 3719
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp74E=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp74F=_tmp74E;void*_tmp753;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74F)->tag == 5){_LL65C: _tmp753=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp74F)->f1).ptr_atts).rgn;_LL65D:
# 3724
{void*_tmp750=Cyc_Tcutil_compress(_tmp753);void*_tmp751=_tmp750;struct Cyc_Absyn_Tvar*_tmp752;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp751)->tag == 2){_LL661: _tmp752=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp751)->f1;_LL662:
# 3726
 return({Cyc_Absyn_tvar_cmp(tvar,_tmp752)== 0;});}else{_LL663: _LL664:
 goto _LL660;}_LL660:;}
# 3729
goto _LL65B;}else{_LL65E: _LL65F:
 goto _LL65B;}_LL65B:;}}else{
# 3735
return 1;}
return 0;}
# 3739
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3742
void*_tmp754=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp755=_tmp754;switch(*((int*)_tmp755)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp755)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp755)->f2)->aliasqual == Cyc_Absyn_Top){_LL666: _LL667:
 goto _LL669;}else{goto _LL66A;}}else{goto _LL66A;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp755)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp755)->f1)->aliasqual == Cyc_Absyn_Top){_LL668: _LL669:
# 3751
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3754
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3760
return& Cyc_Tcutil_trk;}else{goto _LL66A;}}else{goto _LL66A;}default: _LL66A: _LL66B:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL665:;}
# 3766
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3770
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3773
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp759;_tmp759.tag=0;({struct _dyneither_ptr _tmpD9C=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp759.f1=_tmpD9C;});({struct Cyc_String_pa_PrintArg_struct _tmp758;_tmp758.tag=0;_tmp758.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp756[2]={& _tmp758,& _tmp759};({unsigned int _tmpD9E=loc;struct _dyneither_ptr _tmpD9D=({const char*_tmp757="%s clause has type %s instead of integral type";_tag_dyneither(_tmp757,sizeof(char),47);});Cyc_Tcutil_terr(_tmpD9E,_tmpD9D,_tag_dyneither(_tmp756,sizeof(void*),2));});});});});
({struct Cyc_Tcutil_CVTEnv _tmpD9F=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);cvtenv=_tmpD9F;});
({struct Cyc_List_List*_tmpDA0=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);relns=_tmpDA0;});
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp75D;_tmp75D.tag=0;({struct _dyneither_ptr _tmpDA1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));_tmp75D.f1=_tmpDA1;});({struct Cyc_String_pa_PrintArg_struct _tmp75C;_tmp75C.tag=0;_tmp75C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp75A[2]={& _tmp75C,& _tmp75D};({unsigned int _tmpDA3=clause->loc;struct _dyneither_ptr _tmpDA2=({const char*_tmp75B="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp75B,sizeof(char),36);});Cyc_Tcutil_terr(_tmpDA3,_tmpDA2,_tag_dyneither(_tmp75A,sizeof(void*),2));});});});});}
# 3785
return({struct _tuple30 _tmp75E;_tmp75E.f1=cvtenv;_tmp75E.f2=relns;_tmp75E;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3816 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3824
{void*_tmp75F=Cyc_Tcutil_compress(t);void*_tmp760=_tmp75F;struct Cyc_List_List*_tmp900;void*_tmp8FF;void*_tmp8FE;void*_tmp8FD;void*_tmp8FC;void*_tmp8FB;struct _tuple2*_tmp8FA;struct Cyc_List_List**_tmp8F9;struct Cyc_Absyn_Typedefdecl**_tmp8F8;void**_tmp8F7;union Cyc_Absyn_AggrInfoU*_tmp8F6;struct Cyc_List_List**_tmp8F5;enum Cyc_Absyn_AggrKind _tmp8F4;struct Cyc_List_List*_tmp8F3;struct Cyc_List_List*_tmp8F2;struct Cyc_List_List**_tmp8F1;void**_tmp8F0;struct Cyc_Absyn_Tqual*_tmp8EF;void*_tmp8EE;struct Cyc_List_List*_tmp8ED;int _tmp8EC;struct Cyc_Absyn_VarargInfo*_tmp8EB;struct Cyc_List_List*_tmp8EA;struct Cyc_List_List*_tmp8E9;struct Cyc_Absyn_Exp*_tmp8E8;struct Cyc_List_List**_tmp8E7;struct Cyc_Absyn_Exp*_tmp8E6;struct Cyc_List_List**_tmp8E5;void*_tmp8E4;struct Cyc_Absyn_Tqual*_tmp8E3;struct Cyc_Absyn_Exp**_tmp8E2;union Cyc_Absyn_Constraint*_tmp8E1;unsigned int _tmp8E0;struct Cyc_Absyn_Exp*_tmp8DF;struct Cyc_Absyn_Exp*_tmp8DE;void*_tmp8DD;void*_tmp8DC;struct Cyc_Absyn_Tqual*_tmp8DB;void*_tmp8DA;union Cyc_Absyn_Constraint*_tmp8D9;union Cyc_Absyn_Constraint*_tmp8D8;union Cyc_Absyn_Constraint*_tmp8D7;union Cyc_Absyn_DatatypeFieldInfoU*_tmp8D6;struct Cyc_List_List*_tmp8D5;union Cyc_Absyn_DatatypeInfoU*_tmp8D4;struct Cyc_List_List**_tmp8D3;struct _tuple2*_tmp8D2;struct Cyc_Absyn_Enumdecl**_tmp8D1;struct Cyc_List_List*_tmp8D0;void*_tmp8CF;void***_tmp8CE;struct Cyc_Absyn_Tvar*_tmp8CD;struct Cyc_Core_Opt**_tmp8CC;void**_tmp8CB;switch(*((int*)_tmp760)){case 0: _LL66D: _LL66E:
 goto _LL66C;case 1: _LL66F: _tmp8CC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp760)->f1;_tmp8CB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp760)->f2;_LL670:
# 3828
 if(*_tmp8CC == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp8CC))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp8CC))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpDA4=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp8CC=_tmpDA4;});
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3833
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp8CB=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3836
*_tmp8CB=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp761=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp764=_cycalloc(sizeof(*_tmp764));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDA5=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp765;_tmp765.tag=2;_tmp765.f1=0;_tmp765.f2=expected_kind;_tmp765;});_tmp764[0]=_tmpDA5;});_tmp764;}));
({void*_tmpDA7=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp762=_cycalloc(sizeof(*_tmp762));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDA6=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp763;_tmp763.tag=2;_tmp763.f1=_tmp761;_tmp763;});_tmp762[0]=_tmpDA6;});_tmp762;});*_tmp8CB=_tmpDA7;});
_tmp8CD=_tmp761;goto _LL672;}else{
# 3842
({struct Cyc_List_List*_tmpDA8=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpDA8;});}}
# 3844
goto _LL66C;case 2: _LL671: _tmp8CD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp760)->f1;_LL672:
# 3846
{void*_tmp766=Cyc_Absyn_compress_kb(_tmp8CD->kind);void*_tmp767=_tmp766;struct Cyc_Core_Opt**_tmp76B;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp767)->tag == 1){_LL6A8: _tmp76B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp767)->f1;_LL6A9:
# 3848
({struct Cyc_Core_Opt*_tmpDAB=({struct Cyc_Core_Opt*_tmp768=_cycalloc(sizeof(*_tmp768));({void*_tmpDAA=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp769=_cycalloc(sizeof(*_tmp769));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDA9=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp76A;_tmp76A.tag=2;_tmp76A.f1=0;_tmp76A.f2=expected_kind;_tmp76A;});_tmp769[0]=_tmpDA9;});_tmp769;});_tmp768->v=_tmpDAA;});_tmp768;});*_tmp76B=_tmpDAB;});goto _LL6A7;}else{_LL6AA: _LL6AB:
 goto _LL6A7;}_LL6A7:;}
# 3853
({struct Cyc_List_List*_tmpDAC=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8CD);cvtenv.kind_env=_tmpDAC;});
# 3856
({struct Cyc_List_List*_tmpDAD=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8CD,put_in_effect);cvtenv.free_vars=_tmpDAD;});
# 3858
{void*_tmp76C=Cyc_Absyn_compress_kb(_tmp8CD->kind);void*_tmp76D=_tmp76C;struct Cyc_Core_Opt**_tmp772;struct Cyc_Absyn_Kind*_tmp771;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp76D)->tag == 2){_LL6AD: _tmp772=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp76D)->f1;_tmp771=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp76D)->f2;_LL6AE:
# 3860
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp771))
({struct Cyc_Core_Opt*_tmpDB0=({struct Cyc_Core_Opt*_tmp76E=_cycalloc(sizeof(*_tmp76E));({void*_tmpDAF=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp76F=_cycalloc(sizeof(*_tmp76F));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDAE=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp770;_tmp770.tag=2;_tmp770.f1=0;_tmp770.f2=expected_kind;_tmp770;});_tmp76F[0]=_tmpDAE;});_tmp76F;});_tmp76E->v=_tmpDAF;});_tmp76E;});*_tmp772=_tmpDB0;});
goto _LL6AC;}else{_LL6AF: _LL6B0:
 goto _LL6AC;}_LL6AC:;}
# 3865
goto _LL66C;case 26: _LL673: _tmp8CF=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp760)->f1)->r;_tmp8CE=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp760)->f2;_LL674: {
# 3871
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp773=_tmp8CF;struct Cyc_Absyn_Datatypedecl*_tmp776;struct Cyc_Absyn_Enumdecl*_tmp775;struct Cyc_Absyn_Aggrdecl*_tmp774;switch(*((int*)_tmp773)){case 0: _LL6B2: _tmp774=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp773)->f1;_LL6B3:
# 3874
 if(te->in_extern_c_include)
_tmp774->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp774);goto _LL6B1;case 1: _LL6B4: _tmp775=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp773)->f1;_LL6B5:
# 3878
 if(te->in_extern_c_include)
_tmp775->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp775);goto _LL6B1;default: _LL6B6: _tmp776=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp773)->f1;_LL6B7:
# 3882
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp776);goto _LL6B1;}_LL6B1:;}
# 3884
({void**_tmpDB1=({void**_tmp777=_cycalloc(sizeof(*_tmp777));_tmp777[0]=new_t;_tmp777;});*_tmp8CE=_tmpDB1;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL675: _tmp8D0=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp760)->f1;_LL676: {
# 3890
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp8D0 != 0;_tmp8D0=_tmp8D0->tl){
struct Cyc_Absyn_Enumfield*_tmp778=(struct Cyc_Absyn_Enumfield*)_tmp8D0->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp778->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp77B;_tmp77B.tag=0;_tmp77B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp778->name).f2);({void*_tmp779[1]={& _tmp77B};({unsigned int _tmpDB3=_tmp778->loc;struct _dyneither_ptr _tmpDB2=({const char*_tmp77A="duplicate enum field name %s";_tag_dyneither(_tmp77A,sizeof(char),29);});Cyc_Tcutil_terr(_tmpDB3,_tmpDB2,_tag_dyneither(_tmp779,sizeof(void*),1));});});});else{
# 3897
({struct Cyc_List_List*_tmpDB4=({struct Cyc_List_List*_tmp77C=_cycalloc(sizeof(*_tmp77C));_tmp77C->hd=(*_tmp778->name).f2;_tmp77C->tl=prev_fields;_tmp77C;});prev_fields=_tmpDB4;});}
# 3899
if(_tmp778->tag == 0)
({struct Cyc_Absyn_Exp*_tmpDB5=Cyc_Absyn_uint_exp(tag_count,_tmp778->loc);_tmp778->tag=_tmpDB5;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp778->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp77F;_tmp77F.tag=0;_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp778->name).f2);({void*_tmp77D[1]={& _tmp77F};({unsigned int _tmpDB7=loc;struct _dyneither_ptr _tmpDB6=({const char*_tmp77E="enum field %s: expression is not constant";_tag_dyneither(_tmp77E,sizeof(char),42);});Cyc_Tcutil_terr(_tmpDB7,_tmpDB6,_tag_dyneither(_tmp77D,sizeof(void*),1));});});});}{
# 3904
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp778->tag))).f1;
tag_count=t1 + 1;};}
# 3907
goto _LL66C;}case 13: _LL677: _tmp8D2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp760)->f1;_tmp8D1=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp760)->f2;_LL678:
# 3909
 if(*_tmp8D1 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp8D1))->fields == 0){
struct _handler_cons _tmp780;_push_handler(& _tmp780);{int _tmp782=0;if(setjmp(_tmp780.handler))_tmp782=1;if(!_tmp782){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp8D2);
*_tmp8D1=*ed;}
# 3911
;_pop_handler();}else{void*_tmp781=(void*)_exn_thrown;void*_tmp783=_tmp781;void*_tmp786;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp783)->tag == Cyc_Dict_Absent){_LL6B9: _LL6BA: {
# 3915
struct Cyc_Absyn_Enumdecl*_tmp784=({struct Cyc_Absyn_Enumdecl*_tmp785=_cycalloc(sizeof(*_tmp785));_tmp785->sc=Cyc_Absyn_Extern;_tmp785->name=_tmp8D2;_tmp785->fields=0;_tmp785;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp784);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp8D2);
*_tmp8D1=*ed;
goto _LL6B8;};}}else{_LL6BB: _tmp786=_tmp783;_LL6BC:(int)_rethrow(_tmp786);}_LL6B8:;}};}{
# 3921
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp8D1);
goto _LL66C;};case 3: _LL679: _tmp8D4=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp760)->f1).datatype_info;_tmp8D3=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp760)->f1).targs;_LL67A: {
# 3924
struct Cyc_List_List*_tmp787=*_tmp8D3;
{union Cyc_Absyn_DatatypeInfoU _tmp788=*_tmp8D4;union Cyc_Absyn_DatatypeInfoU _tmp789=_tmp788;struct Cyc_Absyn_Datatypedecl*_tmp7A1;struct _tuple2*_tmp7A0;int _tmp79F;if((_tmp789.UnknownDatatype).tag == 1){_LL6BE: _tmp7A0=((_tmp789.UnknownDatatype).val).name;_tmp79F=((_tmp789.UnknownDatatype).val).is_extensible;_LL6BF: {
# 3927
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp78A;_push_handler(& _tmp78A);{int _tmp78C=0;if(setjmp(_tmp78A.handler))_tmp78C=1;if(!_tmp78C){({struct Cyc_Absyn_Datatypedecl**_tmpDB8=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7A0);tudp=_tmpDB8;});;_pop_handler();}else{void*_tmp78B=(void*)_exn_thrown;void*_tmp78D=_tmp78B;void*_tmp793;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp78D)->tag == Cyc_Dict_Absent){_LL6C3: _LL6C4: {
# 3931
struct Cyc_Absyn_Datatypedecl*_tmp78E=({struct Cyc_Absyn_Datatypedecl*_tmp792=_cycalloc(sizeof(*_tmp792));_tmp792->sc=Cyc_Absyn_Extern;_tmp792->name=_tmp7A0;_tmp792->tvs=0;_tmp792->fields=0;_tmp792->is_extensible=_tmp79F;_tmp792;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp78E);
({struct Cyc_Absyn_Datatypedecl**_tmpDB9=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7A0);tudp=_tmpDB9;});
# 3935
if(_tmp787 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0;({struct _dyneither_ptr _tmpDBA=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A0));_tmp791.f1=_tmpDBA;});({void*_tmp78F[1]={& _tmp791};({unsigned int _tmpDBC=loc;struct _dyneither_ptr _tmpDBB=({const char*_tmp790="declare parameterized datatype %s before using";_tag_dyneither(_tmp790,sizeof(char),47);});Cyc_Tcutil_terr(_tmpDBC,_tmpDBB,_tag_dyneither(_tmp78F,sizeof(void*),1));});});});
return cvtenv;}
# 3940
goto _LL6C2;}}else{_LL6C5: _tmp793=_tmp78D;_LL6C6:(int)_rethrow(_tmp793);}_LL6C2:;}};}
# 3944
if(_tmp79F  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp796;_tmp796.tag=0;({struct _dyneither_ptr _tmpDBD=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A0));_tmp796.f1=_tmpDBD;});({void*_tmp794[1]={& _tmp796};({unsigned int _tmpDBF=loc;struct _dyneither_ptr _tmpDBE=({const char*_tmp795="datatype %s was not declared @extensible";_tag_dyneither(_tmp795,sizeof(char),41);});Cyc_Tcutil_terr(_tmpDBF,_tmpDBE,_tag_dyneither(_tmp794,sizeof(void*),1));});});});
({union Cyc_Absyn_DatatypeInfoU _tmpDC0=Cyc_Absyn_KnownDatatype(tudp);*_tmp8D4=_tmpDC0;});
_tmp7A1=*tudp;goto _LL6C1;}}else{_LL6C0: _tmp7A1=*(_tmp789.KnownDatatype).val;_LL6C1: {
# 3951
struct Cyc_List_List*tvs=_tmp7A1->tvs;
for(0;_tmp787 != 0  && tvs != 0;(_tmp787=_tmp787->tl,tvs=tvs->tl)){
void*t=(void*)_tmp787->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3957
{struct _tuple0 _tmp797=({struct _tuple0 _tmp79A;({void*_tmpDC1=Cyc_Absyn_compress_kb(tv->kind);_tmp79A.f1=_tmpDC1;});_tmp79A.f2=t;_tmp79A;});struct _tuple0 _tmp798=_tmp797;struct Cyc_Absyn_Tvar*_tmp799;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp798.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp798.f2)->tag == 2){_LL6C8: _tmp799=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp798.f2)->f1;_LL6C9:
# 3959
({struct Cyc_List_List*_tmpDC2=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp799);cvtenv.kind_env=_tmpDC2;});
({struct Cyc_List_List*_tmpDC3=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp799,1);cvtenv.free_vars=_tmpDC3;});
continue;}else{goto _LL6CA;}}else{_LL6CA: _LL6CB:
 goto _LL6C7;}_LL6C7:;}{
# 3964
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDC4=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpDC4;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3968
if(_tmp787 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp79D;_tmp79D.tag=0;({struct _dyneither_ptr _tmpDC5=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A1->name));_tmp79D.f1=_tmpDC5;});({void*_tmp79B[1]={& _tmp79D};({unsigned int _tmpDC7=loc;struct _dyneither_ptr _tmpDC6=({const char*_tmp79C="too many type arguments for datatype %s";_tag_dyneither(_tmp79C,sizeof(char),40);});Cyc_Tcutil_terr(_tmpDC7,_tmpDC6,_tag_dyneither(_tmp79B,sizeof(void*),1));});});});
if(tvs != 0){
# 3973
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDC8=({struct Cyc_List_List*_tmp79E=_cycalloc(sizeof(*_tmp79E));_tmp79E->hd=e;_tmp79E->tl=hidden_ts;_tmp79E;});hidden_ts=_tmpDC8;});
({struct Cyc_Tcutil_CVTEnv _tmpDC9=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);cvtenv=_tmpDC9;});}
# 3980
({struct Cyc_List_List*_tmpDCB=({struct Cyc_List_List*_tmpDCA=*_tmp8D3;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDCA,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp8D3=_tmpDCB;});}
# 3982
goto _LL6BD;}}_LL6BD:;}
# 3984
goto _LL66C;}case 4: _LL67B: _tmp8D6=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp760)->f1).field_info;_tmp8D5=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp760)->f1).targs;_LL67C:
# 3987
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7A2=*_tmp8D6;union Cyc_Absyn_DatatypeFieldInfoU _tmp7A3=_tmp7A2;struct Cyc_Absyn_Datatypedecl*_tmp7B7;struct Cyc_Absyn_Datatypefield*_tmp7B6;struct _tuple2*_tmp7B5;struct _tuple2*_tmp7B4;int _tmp7B3;if((_tmp7A3.UnknownDatatypefield).tag == 1){_LL6CD: _tmp7B5=((_tmp7A3.UnknownDatatypefield).val).datatype_name;_tmp7B4=((_tmp7A3.UnknownDatatypefield).val).field_name;_tmp7B3=((_tmp7A3.UnknownDatatypefield).val).is_extensible;_LL6CE: {
# 3990
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7B5);
struct Cyc_Absyn_Datatypefield*tuf;
# 3995
{struct Cyc_List_List*_tmp7A4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp7A4=_tmp7A4->tl){
if(_tmp7A4 == 0)
({void*_tmp7A5=0;({struct _dyneither_ptr _tmpDCC=({const char*_tmp7A6="Tcutil found a bad datatypefield";_tag_dyneither(_tmp7A6,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDCC,_tag_dyneither(_tmp7A5,sizeof(void*),0));});});
if(({Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp7A4->hd)->name,_tmp7B4)== 0;})){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp7A4->hd;
break;}}}
# 4003
({union Cyc_Absyn_DatatypeFieldInfoU _tmpDCD=Cyc_Absyn_KnownDatatypefield(tud,tuf);*_tmp8D6=_tmpDCD;});
_tmp7B7=tud;_tmp7B6=tuf;goto _LL6D0;}}else{_LL6CF: _tmp7B7=((_tmp7A3.KnownDatatypefield).val).f1;_tmp7B6=((_tmp7A3.KnownDatatypefield).val).f2;_LL6D0: {
# 4007
struct Cyc_List_List*tvs=_tmp7B7->tvs;
for(0;_tmp8D5 != 0  && tvs != 0;(_tmp8D5=_tmp8D5->tl,tvs=tvs->tl)){
void*t=(void*)_tmp8D5->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4013
{struct _tuple0 _tmp7A7=({struct _tuple0 _tmp7AA;({void*_tmpDCE=Cyc_Absyn_compress_kb(tv->kind);_tmp7AA.f1=_tmpDCE;});_tmp7AA.f2=t;_tmp7AA;});struct _tuple0 _tmp7A8=_tmp7A7;struct Cyc_Absyn_Tvar*_tmp7A9;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7A8.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7A8.f2)->tag == 2){_LL6D2: _tmp7A9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7A8.f2)->f1;_LL6D3:
# 4015
({struct Cyc_List_List*_tmpDCF=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7A9);cvtenv.kind_env=_tmpDCF;});
({struct Cyc_List_List*_tmpDD0=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7A9,1);cvtenv.free_vars=_tmpDD0;});
continue;}else{goto _LL6D4;}}else{_LL6D4: _LL6D5:
 goto _LL6D1;}_LL6D1:;}{
# 4020
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDD1=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpDD1;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4024
if(_tmp8D5 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7AE;_tmp7AE.tag=0;({struct _dyneither_ptr _tmpDD2=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B6->name));_tmp7AE.f1=_tmpDD2;});({struct Cyc_String_pa_PrintArg_struct _tmp7AD;_tmp7AD.tag=0;({struct _dyneither_ptr _tmpDD3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7B7->name));_tmp7AD.f1=_tmpDD3;});({void*_tmp7AB[2]={& _tmp7AD,& _tmp7AE};({unsigned int _tmpDD5=loc;struct _dyneither_ptr _tmpDD4=({const char*_tmp7AC="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp7AC,sizeof(char),43);});Cyc_Tcutil_terr(_tmpDD5,_tmpDD4,_tag_dyneither(_tmp7AB,sizeof(void*),2));});});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B2;_tmp7B2.tag=0;({struct _dyneither_ptr _tmpDD6=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B6->name));_tmp7B2.f1=_tmpDD6;});({struct Cyc_String_pa_PrintArg_struct _tmp7B1;_tmp7B1.tag=0;({struct _dyneither_ptr _tmpDD7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7B7->name));_tmp7B1.f1=_tmpDD7;});({void*_tmp7AF[2]={& _tmp7B1,& _tmp7B2};({unsigned int _tmpDD9=loc;struct _dyneither_ptr _tmpDD8=({const char*_tmp7B0="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp7B0,sizeof(char),42);});Cyc_Tcutil_terr(_tmpDD9,_tmpDD8,_tag_dyneither(_tmp7AF,sizeof(void*),2));});});});});
goto _LL6CC;}}_LL6CC:;}
# 4032
goto _LL66C;case 5: _LL67D: _tmp8DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp760)->f1).elt_typ;_tmp8DB=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp760)->f1).elt_tq;_tmp8DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp760)->f1).ptr_atts).rgn;_tmp8D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp760)->f1).ptr_atts).nullable;_tmp8D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp760)->f1).ptr_atts).bounds;_tmp8D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp760)->f1).ptr_atts).zero_term;_LL67E: {
# 4035
int is_zero_terminated;
# 4037
({struct Cyc_Tcutil_CVTEnv _tmpDDA=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp8DC,1,1);cvtenv=_tmpDDA;});
({int _tmpDDB=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8DB->print_const,_tmp8DC);_tmp8DB->real_const=_tmpDDB;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp7B8=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp7B9=_tmp7B8;switch(_tmp7B9){case Cyc_Absyn_Aliasable: _LL6D7: _LL6D8:
 k=& Cyc_Tcutil_rk;goto _LL6D6;case Cyc_Absyn_Unique: _LL6D9: _LL6DA:
 k=& Cyc_Tcutil_urk;goto _LL6D6;default: _LL6DB: _LL6DC:
 k=& Cyc_Tcutil_trk;goto _LL6D6;}_LL6D6:;}
# 4045
({struct Cyc_Tcutil_CVTEnv _tmpDDC=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp8DA,1,1);cvtenv=_tmpDDC;});
{union Cyc_Absyn_Constraint*_tmp7BA=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp8D7);union Cyc_Absyn_Constraint*_tmp7BB=_tmp7BA;int _tmp7C1;switch((((union Cyc_Absyn_Constraint*)_tmp7BB)->Eq_constr).tag){case 3: _LL6DE: _LL6DF:
# 4050
{void*_tmp7BC=Cyc_Tcutil_compress(_tmp8DC);void*_tmp7BD=_tmp7BC;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7BD)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7BD)->f2 == Cyc_Absyn_Char_sz){_LL6E5: _LL6E6:
# 4052
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D7,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL6E4;}else{goto _LL6E7;}}else{_LL6E7: _LL6E8:
# 4056
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D7,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL6E4;}_LL6E4:;}
# 4060
goto _LL6DD;case 1: _LL6E0: _tmp7C1=(int)(_tmp7BB->Eq_constr).val;if(_tmp7C1){_LL6E1:
# 4063
 if(!Cyc_Tcutil_admits_zero(_tmp8DC))
({struct Cyc_String_pa_PrintArg_struct _tmp7C0;_tmp7C0.tag=0;({struct _dyneither_ptr _tmpDDD=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8DC));_tmp7C0.f1=_tmpDDD;});({void*_tmp7BE[1]={& _tmp7C0};({unsigned int _tmpDDF=loc;struct _dyneither_ptr _tmpDDE=({const char*_tmp7BF="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp7BF,sizeof(char),53);});Cyc_Tcutil_terr(_tmpDDF,_tmpDDE,_tag_dyneither(_tmp7BE,sizeof(void*),1));});});});
is_zero_terminated=1;
goto _LL6DD;}else{goto _LL6E2;}default: _LL6E2: _LL6E3:
# 4069
 is_zero_terminated=0;
goto _LL6DD;}_LL6DD:;}
# 4073
{void*_tmp7C2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp8D8);void*_tmp7C3=_tmp7C2;struct Cyc_Absyn_Exp*_tmp7CC;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp7C3)->tag == 0){_LL6EA: _LL6EB:
 goto _LL6E9;}else{_LL6EC: _tmp7CC=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7C3)->f1;_LL6ED:
# 4076
({struct Cyc_Tcenv_Tenv*_tmpDE0=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDE0,0,_tmp7CC);});
({struct Cyc_Tcutil_CVTEnv _tmpDE1=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7CC,te,cvtenv);cvtenv=_tmpDE1;});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7CC))
({void*_tmp7C4=0;({unsigned int _tmpDE3=loc;struct _dyneither_ptr _tmpDE2=({const char*_tmp7C5="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp7C5,sizeof(char),49);});Cyc_Tcutil_terr(_tmpDE3,_tmpDE2,_tag_dyneither(_tmp7C4,sizeof(void*),0));});});{
struct _tuple14 _tmp7C6=Cyc_Evexp_eval_const_uint_exp(_tmp7CC);struct _tuple14 _tmp7C7=_tmp7C6;unsigned int _tmp7CB;int _tmp7CA;_LL6EF: _tmp7CB=_tmp7C7.f1;_tmp7CA=_tmp7C7.f2;_LL6F0:;
if(is_zero_terminated  && (!_tmp7CA  || _tmp7CB < 1))
({void*_tmp7C8=0;({unsigned int _tmpDE5=loc;struct _dyneither_ptr _tmpDE4=({const char*_tmp7C9="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp7C9,sizeof(char),55);});Cyc_Tcutil_terr(_tmpDE5,_tmpDE4,_tag_dyneither(_tmp7C8,sizeof(void*),0));});});
goto _LL6E9;};}_LL6E9:;}
# 4085
goto _LL66C;};}case 19: _LL67F: _tmp8DD=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp760)->f1;_LL680:
# 4087
({struct Cyc_Tcutil_CVTEnv _tmpDE6=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp8DD,1,1);cvtenv=_tmpDE6;});goto _LL66C;case 28: _LL681: _LL682:
 goto _LL66C;case 18: _LL683: _tmp8DE=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp760)->f1;_LL684:
# 4093
({struct Cyc_Tcenv_Tenv*_tmpDE7=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDE7,0,_tmp8DE);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp8DE))
({void*_tmp7CD=0;({unsigned int _tmpDE9=loc;struct _dyneither_ptr _tmpDE8=({const char*_tmp7CE="valueof_t requires an int expression";_tag_dyneither(_tmp7CE,sizeof(char),37);});Cyc_Tcutil_terr(_tmpDE9,_tmpDE8,_tag_dyneither(_tmp7CD,sizeof(void*),0));});});
({struct Cyc_Tcutil_CVTEnv _tmpDEA=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp8DE,te,cvtenv);cvtenv=_tmpDEA;});
goto _LL66C;case 27: _LL685: _tmp8DF=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp760)->f1;_LL686:
# 4102
({struct Cyc_Tcenv_Tenv*_tmpDEB=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDEB,0,_tmp8DF);});
goto _LL66C;case 6: _LL687: _LL688:
 goto _LL68A;case 7: _LL689: _LL68A:
 goto _LL66C;case 8: _LL68B: _tmp8E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp760)->f1).elt_type;_tmp8E3=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp760)->f1).tq;_tmp8E2=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp760)->f1).num_elts;_tmp8E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp760)->f1).zero_term;_tmp8E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp760)->f1).zt_loc;_LL68C: {
# 4109
struct Cyc_Absyn_Exp*_tmp7CF=*_tmp8E2;
({struct Cyc_Tcutil_CVTEnv _tmpDEC=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8E4,1,allow_abs_aggr);cvtenv=_tmpDEC;});
({int _tmpDED=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8E3->print_const,_tmp8E4);_tmp8E3->real_const=_tmpDED;});{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp7D0=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp8E1);union Cyc_Absyn_Constraint*_tmp7D1=_tmp7D0;int _tmp7D5;switch((((union Cyc_Absyn_Constraint*)_tmp7D1)->Eq_constr).tag){case 3: _LL6F2: _LL6F3:
# 4116
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8E1,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4131 "tcutil.cyc"
goto _LL6F1;case 1: _LL6F4: _tmp7D5=(int)(_tmp7D1->Eq_constr).val;if(_tmp7D5){_LL6F5:
# 4134
 if(!Cyc_Tcutil_admits_zero(_tmp8E4))
({struct Cyc_String_pa_PrintArg_struct _tmp7D4;_tmp7D4.tag=0;({struct _dyneither_ptr _tmpDEE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8E4));_tmp7D4.f1=_tmpDEE;});({void*_tmp7D2[1]={& _tmp7D4};({unsigned int _tmpDF0=loc;struct _dyneither_ptr _tmpDEF=({const char*_tmp7D3="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp7D3,sizeof(char),51);});Cyc_Tcutil_terr(_tmpDF0,_tmpDEF,_tag_dyneither(_tmp7D2,sizeof(void*),1));});});});
is_zero_terminated=1;
goto _LL6F1;}else{goto _LL6F6;}default: _LL6F6: _LL6F7:
# 4140
 is_zero_terminated=0;
goto _LL6F1;}_LL6F1:;}
# 4145
if(_tmp7CF == 0){
# 4147
if(is_zero_terminated)
# 4149
({struct Cyc_Absyn_Exp*_tmpDF2=({struct Cyc_Absyn_Exp*_tmpDF1=Cyc_Absyn_uint_exp(1,0);_tmp7CF=_tmpDF1;});*_tmp8E2=_tmpDF2;});else{
# 4152
({void*_tmp7D6=0;({unsigned int _tmpDF4=loc;struct _dyneither_ptr _tmpDF3=({const char*_tmp7D7="array bound defaults to 1 here";_tag_dyneither(_tmp7D7,sizeof(char),31);});Cyc_Tcutil_warn(_tmpDF4,_tmpDF3,_tag_dyneither(_tmp7D6,sizeof(void*),0));});});
({struct Cyc_Absyn_Exp*_tmpDF6=({struct Cyc_Absyn_Exp*_tmpDF5=Cyc_Absyn_uint_exp(1,0);_tmp7CF=_tmpDF5;});*_tmp8E2=_tmpDF6;});}}
# 4156
({struct Cyc_Tcenv_Tenv*_tmpDF7=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDF7,0,_tmp7CF);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7CF))
({void*_tmp7D8=0;({unsigned int _tmpDF9=loc;struct _dyneither_ptr _tmpDF8=({const char*_tmp7D9="array bounds expression is not an unsigned int";_tag_dyneither(_tmp7D9,sizeof(char),47);});Cyc_Tcutil_terr(_tmpDF9,_tmpDF8,_tag_dyneither(_tmp7D8,sizeof(void*),0));});});
({struct Cyc_Tcutil_CVTEnv _tmpDFA=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7CF,te,cvtenv);cvtenv=_tmpDFA;});{
# 4164
struct _tuple14 _tmp7DA=Cyc_Evexp_eval_const_uint_exp(_tmp7CF);struct _tuple14 _tmp7DB=_tmp7DA;unsigned int _tmp7E1;int _tmp7E0;_LL6F9: _tmp7E1=_tmp7DB.f1;_tmp7E0=_tmp7DB.f2;_LL6FA:;
# 4166
if((is_zero_terminated  && _tmp7E0) && _tmp7E1 < 1)
({void*_tmp7DC=0;({unsigned int _tmpDFC=loc;struct _dyneither_ptr _tmpDFB=({const char*_tmp7DD="zero terminated array cannot have zero elements";_tag_dyneither(_tmp7DD,sizeof(char),48);});Cyc_Tcutil_warn(_tmpDFC,_tmpDFB,_tag_dyneither(_tmp7DC,sizeof(void*),0));});});
# 4169
if((_tmp7E0  && _tmp7E1 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp7DE=0;({unsigned int _tmpDFE=loc;struct _dyneither_ptr _tmpDFD=({const char*_tmp7DF="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp7DF,sizeof(char),75);});Cyc_Tcutil_warn(_tmpDFE,_tmpDFD,_tag_dyneither(_tmp7DE,sizeof(void*),0));});});
({struct Cyc_Absyn_Exp*_tmpDFF=Cyc_Absyn_uint_exp(1,0);*_tmp8E2=_tmpDFF;});}
# 4173
goto _LL66C;};};}case 9: _LL68D: _tmp8F1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).tvars;_tmp8F0=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).effect;_tmp8EF=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).ret_tqual;_tmp8EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).ret_typ;_tmp8ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).args;_tmp8EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).c_varargs;_tmp8EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).cyc_varargs;_tmp8EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).rgn_po;_tmp8E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).attributes;_tmp8E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).requires_clause;_tmp8E7=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).requires_relns;_tmp8E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).ensures_clause;_tmp8E5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp760)->f1).ensures_relns;_LL68E: {
# 4180
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp8E9 != 0;_tmp8E9=_tmp8E9->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp8E9->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp7E4;_tmp7E4.tag=0;({struct _dyneither_ptr _tmpE00=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8E9->hd));_tmp7E4.f1=_tmpE00;});({void*_tmp7E2[1]={& _tmp7E4};({unsigned int _tmpE02=loc;struct _dyneither_ptr _tmpE01=({const char*_tmp7E3="bad function type attribute %s";_tag_dyneither(_tmp7E3,sizeof(char),31);});Cyc_Tcutil_terr(_tmpE02,_tmpE01,_tag_dyneither(_tmp7E2,sizeof(void*),1));});});});{
void*_tmp7E5=(void*)_tmp8E9->hd;void*_tmp7E6=_tmp7E5;enum Cyc_Absyn_Format_Type _tmp7EB;int _tmp7EA;int _tmp7E9;switch(*((int*)_tmp7E6)){case 1: _LL6FC: _LL6FD:
# 4193
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL6FB;case 2: _LL6FE: _LL6FF:
# 4195
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL6FB;case 3: _LL700: _LL701:
# 4197
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL6FB;case 19: _LL702: _tmp7EB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7E6)->f1;_tmp7EA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7E6)->f2;_tmp7E9=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7E6)->f3;_LL703:
# 4199
 if(!seen_format){
seen_format=1;
ft=_tmp7EB;
fmt_desc_arg=_tmp7EA;
fmt_arg_start=_tmp7E9;}else{
# 4205
({void*_tmp7E7=0;({unsigned int _tmpE04=loc;struct _dyneither_ptr _tmpE03=({const char*_tmp7E8="function can't have multiple format attributes";_tag_dyneither(_tmp7E8,sizeof(char),47);});Cyc_Tcutil_terr(_tmpE04,_tmpE03,_tag_dyneither(_tmp7E7,sizeof(void*),0));});});}
goto _LL6FB;default: _LL704: _LL705:
 goto _LL6FB;}_LL6FB:;};}
# 4210
if(num_convs > 1)
({void*_tmp7EC=0;({unsigned int _tmpE06=loc;struct _dyneither_ptr _tmpE05=({const char*_tmp7ED="function can't have multiple calling conventions";_tag_dyneither(_tmp7ED,sizeof(char),49);});Cyc_Tcutil_terr(_tmpE06,_tmpE05,_tag_dyneither(_tmp7EC,sizeof(void*),0));});});
# 4215
Cyc_Tcutil_check_unique_tvars(loc,*_tmp8F1);
{struct Cyc_List_List*b=*_tmp8F1;for(0;b != 0;b=b->tl){
({int _tmpE07=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpE07;});
({struct Cyc_List_List*_tmpE08=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpE08;});{
void*_tmp7EE=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7EF=_tmp7EE;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7EF)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7EF)->f1)->kind == Cyc_Absyn_MemKind){_LL707: _LL708:
# 4221
({struct Cyc_String_pa_PrintArg_struct _tmp7F2;_tmp7F2.tag=0;_tmp7F2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp7F0[1]={& _tmp7F2};({unsigned int _tmpE0A=loc;struct _dyneither_ptr _tmpE09=({const char*_tmp7F1="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp7F1,sizeof(char),51);});Cyc_Tcutil_terr(_tmpE0A,_tmpE09,_tag_dyneither(_tmp7F0,sizeof(void*),1));});});});
goto _LL706;}else{goto _LL709;}}else{_LL709: _LL70A:
 goto _LL706;}_LL706:;};}}{
# 4229
struct Cyc_Tcutil_CVTEnv _tmp7F3=({struct Cyc_Tcutil_CVTEnv _tmp881;_tmp881.r=Cyc_Core_heap_region;_tmp881.kind_env=cvtenv.kind_env;_tmp881.free_vars=0;_tmp881.free_evars=0;_tmp881.generalize_evars=cvtenv.generalize_evars;_tmp881.fn_result=1;_tmp881;});
# 4234
({struct Cyc_Tcutil_CVTEnv _tmpE0B=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F3,& Cyc_Tcutil_tmk,_tmp8EE,1,1);_tmp7F3=_tmpE0B;});
({int _tmpE0C=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8EF->print_const,_tmp8EE);_tmp8EF->real_const=_tmpE0C;});
_tmp7F3.fn_result=0;
{struct Cyc_List_List*a=_tmp8ED;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp7F4=(struct _tuple10*)a->hd;
void*_tmp7F5=(*_tmp7F4).f3;
({struct Cyc_Tcutil_CVTEnv _tmpE0D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F3,& Cyc_Tcutil_tmk,_tmp7F5,1,1);_tmp7F3=_tmpE0D;});{
int _tmp7F6=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp7F4).f2).print_const,_tmp7F5);
((*_tmp7F4).f2).real_const=_tmp7F6;
# 4245
if(Cyc_Tcutil_is_array(_tmp7F5)){
# 4247
void*_tmp7F7=Cyc_Absyn_new_evar(0,0);
({struct Cyc_Tcutil_CVTEnv _tmpE0E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F3,& Cyc_Tcutil_rk,_tmp7F7,1,1);_tmp7F3=_tmpE0E;});
({void*_tmpE0F=Cyc_Tcutil_promote_array(_tmp7F5,_tmp7F7,0);(*_tmp7F4).f3=_tmpE0F;});}};}}
# 4254
if(_tmp8EB != 0){
if(_tmp8EC)({void*_tmp7F8=0;({struct _dyneither_ptr _tmpE10=({const char*_tmp7F9="both c_vararg and cyc_vararg";_tag_dyneither(_tmp7F9,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE10,_tag_dyneither(_tmp7F8,sizeof(void*),0));});});{
struct Cyc_Absyn_VarargInfo _tmp7FA=*_tmp8EB;struct Cyc_Absyn_VarargInfo _tmp7FB=_tmp7FA;void*_tmp80E;int _tmp80D;_LL70C: _tmp80E=_tmp7FB.type;_tmp80D=_tmp7FB.inject;_LL70D:;
({struct Cyc_Tcutil_CVTEnv _tmpE11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F3,& Cyc_Tcutil_tmk,_tmp80E,1,1);_tmp7F3=_tmpE11;});
({int _tmpE12=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp8EB->tq).print_const,_tmp80E);(_tmp8EB->tq).real_const=_tmpE12;});
# 4260
if(_tmp80D){
void*_tmp7FC=Cyc_Tcutil_compress(_tmp80E);void*_tmp7FD=_tmp7FC;void*_tmp80C;union Cyc_Absyn_Constraint*_tmp80B;union Cyc_Absyn_Constraint*_tmp80A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FD)->tag == 5){_LL70F: _tmp80C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FD)->f1).elt_typ;_tmp80B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FD)->f1).ptr_atts).bounds;_tmp80A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FD)->f1).ptr_atts).zero_term;_LL710:
# 4263
{void*_tmp7FE=Cyc_Tcutil_compress(_tmp80C);void*_tmp7FF=_tmp7FE;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp7FF)->tag == 3){_LL714: _LL715:
# 4265
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp80A))
({void*_tmp800=0;({unsigned int _tmpE14=loc;struct _dyneither_ptr _tmpE13=({const char*_tmp801="can't inject into a zeroterm pointer";_tag_dyneither(_tmp801,sizeof(char),37);});Cyc_Tcutil_terr(_tmpE14,_tmpE13,_tag_dyneither(_tmp800,sizeof(void*),0));});});
{void*_tmp802=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp80B);void*_tmp803=_tmp802;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp803)->tag == 0){_LL719: _LL71A:
# 4269
({void*_tmp804=0;({unsigned int _tmpE16=loc;struct _dyneither_ptr _tmpE15=({const char*_tmp805="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp805,sizeof(char),44);});Cyc_Tcutil_terr(_tmpE16,_tmpE15,_tag_dyneither(_tmp804,sizeof(void*),0));});});
goto _LL718;}else{_LL71B: _LL71C:
 goto _LL718;}_LL718:;}
# 4273
goto _LL713;}else{_LL716: _LL717:
({void*_tmp806=0;({unsigned int _tmpE18=loc;struct _dyneither_ptr _tmpE17=({const char*_tmp807="can't inject a non-datatype type";_tag_dyneither(_tmp807,sizeof(char),33);});Cyc_Tcutil_terr(_tmpE18,_tmpE17,_tag_dyneither(_tmp806,sizeof(void*),0));});});goto _LL713;}_LL713:;}
# 4276
goto _LL70E;}else{_LL711: _LL712:
({void*_tmp808=0;({unsigned int _tmpE1A=loc;struct _dyneither_ptr _tmpE19=({const char*_tmp809="expecting a datatype pointer type";_tag_dyneither(_tmp809,sizeof(char),34);});Cyc_Tcutil_terr(_tmpE1A,_tmpE19,_tag_dyneither(_tmp808,sizeof(void*),0));});});goto _LL70E;}_LL70E:;}};}
# 4282
if(seen_format){
int _tmp80F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8ED);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp80F) || fmt_arg_start < 0) || 
# 4286
(_tmp8EB == 0  && !_tmp8EC) && fmt_arg_start != 0) || 
(_tmp8EB != 0  || _tmp8EC) && fmt_arg_start != _tmp80F + 1)
# 4289
({void*_tmp810=0;({unsigned int _tmpE1C=loc;struct _dyneither_ptr _tmpE1B=({const char*_tmp811="bad format descriptor";_tag_dyneither(_tmp811,sizeof(char),22);});Cyc_Tcutil_terr(_tmpE1C,_tmpE1B,_tag_dyneither(_tmp810,sizeof(void*),0));});});else{
# 4292
struct _tuple10 _tmp812=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp8ED,fmt_desc_arg - 1);struct _tuple10 _tmp813=_tmp812;void*_tmp82B;_LL71E: _tmp82B=_tmp813.f3;_LL71F:;
# 4294
{void*_tmp814=Cyc_Tcutil_compress(_tmp82B);void*_tmp815=_tmp814;void*_tmp823;union Cyc_Absyn_Constraint*_tmp822;union Cyc_Absyn_Constraint*_tmp821;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp815)->tag == 5){_LL721: _tmp823=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp815)->f1).elt_typ;_tmp822=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp815)->f1).ptr_atts).bounds;_tmp821=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp815)->f1).ptr_atts).zero_term;_LL722:
# 4297
{struct _tuple0 _tmp816=({struct _tuple0 _tmp81E;({void*_tmpE1D=Cyc_Tcutil_compress(_tmp823);_tmp81E.f1=_tmpE1D;});({void*_tmpE1E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp822);_tmp81E.f2=_tmpE1E;});_tmp81E;});struct _tuple0 _tmp817=_tmp816;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp817.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp817.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp817.f2)->tag == 0){_LL726: _LL727:
# 4299
 if(_tmp8EC)
({void*_tmp818=0;({unsigned int _tmpE20=loc;struct _dyneither_ptr _tmpE1F=({const char*_tmp819="format descriptor is not a char * type";_tag_dyneither(_tmp819,sizeof(char),39);});Cyc_Tcutil_terr(_tmpE20,_tmpE1F,_tag_dyneither(_tmp818,sizeof(void*),0));});});
goto _LL725;}else{_LL728: _LL729:
# 4303
 if(!_tmp8EC)
({void*_tmp81A=0;({unsigned int _tmpE22=loc;struct _dyneither_ptr _tmpE21=({const char*_tmp81B="format descriptor is not a char ? type";_tag_dyneither(_tmp81B,sizeof(char),39);});Cyc_Tcutil_terr(_tmpE22,_tmpE21,_tag_dyneither(_tmp81A,sizeof(void*),0));});});
goto _LL725;}}else{goto _LL72A;}}else{_LL72A: _LL72B:
# 4307
({void*_tmp81C=0;({unsigned int _tmpE24=loc;struct _dyneither_ptr _tmpE23=({const char*_tmp81D="format descriptor is not a string type";_tag_dyneither(_tmp81D,sizeof(char),39);});Cyc_Tcutil_terr(_tmpE24,_tmpE23,_tag_dyneither(_tmp81C,sizeof(void*),0));});});
goto _LL725;}_LL725:;}
# 4310
goto _LL720;}else{_LL723: _LL724:
({void*_tmp81F=0;({unsigned int _tmpE26=loc;struct _dyneither_ptr _tmpE25=({const char*_tmp820="format descriptor is not a string type";_tag_dyneither(_tmp820,sizeof(char),39);});Cyc_Tcutil_terr(_tmpE26,_tmpE25,_tag_dyneither(_tmp81F,sizeof(void*),0));});});goto _LL720;}_LL720:;}
# 4313
if(fmt_arg_start != 0  && !_tmp8EC){
# 4317
int problem;
{struct _tuple31 _tmp824=({struct _tuple31 _tmp828;_tmp828.f1=ft;({void*_tmpE27=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp8EB))->type));_tmp828.f2=_tmpE27;});_tmp828;});struct _tuple31 _tmp825=_tmp824;struct Cyc_Absyn_Datatypedecl*_tmp827;struct Cyc_Absyn_Datatypedecl*_tmp826;if(_tmp825.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp825.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp825.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL72D: _tmp826=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp825.f2)->f1).datatype_info).KnownDatatype).val;_LL72E:
# 4320
({int _tmpE28=({Cyc_Absyn_qvar_cmp(_tmp826->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;});problem=_tmpE28;});goto _LL72C;}else{goto _LL731;}}else{goto _LL731;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp825.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp825.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL72F: _tmp827=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp825.f2)->f1).datatype_info).KnownDatatype).val;_LL730:
# 4322
({int _tmpE29=({Cyc_Absyn_qvar_cmp(_tmp827->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;});problem=_tmpE29;});goto _LL72C;}else{goto _LL731;}}else{_LL731: _LL732:
# 4324
 problem=1;goto _LL72C;}}_LL72C:;}
# 4326
if(problem)
({void*_tmp829=0;({unsigned int _tmpE2B=loc;struct _dyneither_ptr _tmpE2A=({const char*_tmp82A="format attribute and vararg types don't match";_tag_dyneither(_tmp82A,sizeof(char),46);});Cyc_Tcutil_terr(_tmpE2B,_tmpE2A,_tag_dyneither(_tmp829,sizeof(void*),0));});});}}}
# 4334
{struct Cyc_List_List*rpo=_tmp8EA;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp82C=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp82D=_tmp82C;void*_tmp82F;void*_tmp82E;_LL734: _tmp82F=_tmp82D->f1;_tmp82E=_tmp82D->f2;_LL735:;
({struct Cyc_Tcutil_CVTEnv _tmpE2C=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F3,& Cyc_Tcutil_ek,_tmp82F,1,1);_tmp7F3=_tmpE2C;});
({struct Cyc_Tcutil_CVTEnv _tmpE2D=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F3,& Cyc_Tcutil_trk,_tmp82E,1,1);_tmp7F3=_tmpE2D;});}}{
# 4342
struct Cyc_Tcenv_Fenv*_tmp830=Cyc_Tcenv_bogus_fenv(_tmp8EE,_tmp8ED);
struct Cyc_Tcenv_Tenv*_tmp831=({struct Cyc_Tcenv_Tenv*_tmp880=_cycalloc(sizeof(*_tmp880));_tmp880->ns=te->ns;_tmp880->ae=te->ae;_tmp880->le=_tmp830;_tmp880->allow_valueof=1;_tmp880->in_extern_c_include=te->in_extern_c_include;_tmp880;});
struct _tuple30 _tmp832=({unsigned int _tmpE31=loc;struct Cyc_Tcenv_Tenv*_tmpE30=_tmp831;struct Cyc_Tcutil_CVTEnv _tmpE2F=_tmp7F3;struct _dyneither_ptr _tmpE2E=({const char*_tmp87F="@requires";_tag_dyneither(_tmp87F,sizeof(char),10);});Cyc_Tcutil_check_clause(_tmpE31,_tmpE30,_tmpE2F,_tmpE2E,_tmp8E8);});struct _tuple30 _tmp833=_tmp832;struct Cyc_Tcutil_CVTEnv _tmp87E;struct Cyc_List_List*_tmp87D;_LL737: _tmp87E=_tmp833.f1;_tmp87D=_tmp833.f2;_LL738:;
_tmp7F3=_tmp87E;
*_tmp8E7=_tmp87D;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp8ED,_tmp87D);{
# 4355
struct _tuple30 _tmp834=({unsigned int _tmpE35=loc;struct Cyc_Tcenv_Tenv*_tmpE34=_tmp831;struct Cyc_Tcutil_CVTEnv _tmpE33=_tmp7F3;struct _dyneither_ptr _tmpE32=({const char*_tmp87C="@ensures";_tag_dyneither(_tmp87C,sizeof(char),9);});Cyc_Tcutil_check_clause(_tmpE35,_tmpE34,_tmpE33,_tmpE32,_tmp8E6);});struct _tuple30 _tmp835=_tmp834;struct Cyc_Tcutil_CVTEnv _tmp87B;struct Cyc_List_List*_tmp87A;_LL73A: _tmp87B=_tmp835.f1;_tmp87A=_tmp835.f2;_LL73B:;
_tmp7F3=_tmp87B;
*_tmp8E5=_tmp87A;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp8ED,_tmp87A);
# 4360
if(*_tmp8F0 != 0)
({struct Cyc_Tcutil_CVTEnv _tmpE36=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F3,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp8F0),1,1);_tmp7F3=_tmpE36;});else{
# 4363
struct Cyc_List_List*effect=0;
# 4368
{struct Cyc_List_List*tvs=_tmp7F3.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp836=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp837=_tmp836;struct Cyc_Absyn_Tvar*_tmp850;int _tmp84F;_LL73D: _tmp850=_tmp837.f1;_tmp84F=_tmp837.f2;_LL73E:;
if(!_tmp84F)continue;{
void*_tmp838=Cyc_Absyn_compress_kb(_tmp850->kind);void*_tmp839=_tmp838;struct Cyc_Core_Opt**_tmp84E;struct Cyc_Absyn_Kind*_tmp84D;struct Cyc_Core_Opt**_tmp84C;struct Cyc_Core_Opt**_tmp84B;struct Cyc_Absyn_Kind*_tmp84A;switch(*((int*)_tmp839)){case 2: _LL740: _tmp84B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp839)->f1;_tmp84A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp839)->f2;if(_tmp84A->kind == Cyc_Absyn_RgnKind){_LL741:
# 4374
 if(_tmp84A->aliasqual == Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpE37=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp84B=_tmpE37;});_tmp84D=_tmp84A;goto _LL743;}
# 4377
({struct Cyc_Core_Opt*_tmpE38=Cyc_Tcutil_kind_to_bound_opt(_tmp84A);*_tmp84B=_tmpE38;});_tmp84D=_tmp84A;goto _LL743;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp839)->f2)->kind){case Cyc_Absyn_IntKind: _LL744: _LL745:
# 4380
 goto _LL747;case Cyc_Absyn_EffKind: _LL748: _tmp84C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp839)->f1;_LL749:
# 4383
({struct Cyc_Core_Opt*_tmpE39=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp84C=_tmpE39;});goto _LL74B;default: goto _LL74E;}}case 0: _LL742: _tmp84D=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp839)->f1;if(_tmp84D->kind == Cyc_Absyn_RgnKind){_LL743:
# 4379
({struct Cyc_List_List*_tmpE3E=({struct Cyc_List_List*_tmp83A=_cycalloc(sizeof(*_tmp83A));({void*_tmpE3D=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp83B=_cycalloc(sizeof(*_tmp83B));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE3C=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp83C;_tmp83C.tag=23;({void*_tmpE3B=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp83D=_cycalloc(sizeof(*_tmp83D));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE3A=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp83E;_tmp83E.tag=2;_tmp83E.f1=_tmp850;_tmp83E;});_tmp83D[0]=_tmpE3A;});_tmp83D;});_tmp83C.f1=_tmpE3B;});_tmp83C;});_tmp83B[0]=_tmpE3C;});_tmp83B;});_tmp83A->hd=_tmpE3D;});_tmp83A->tl=effect;_tmp83A;});effect=_tmpE3E;});goto _LL73F;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp839)->f1)->kind){case Cyc_Absyn_IntKind: _LL746: _LL747:
# 4381
 goto _LL73F;case Cyc_Absyn_EffKind: _LL74A: _LL74B:
# 4385
({struct Cyc_List_List*_tmpE41=({struct Cyc_List_List*_tmp83F=_cycalloc(sizeof(*_tmp83F));({void*_tmpE40=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp840=_cycalloc(sizeof(*_tmp840));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE3F=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp841;_tmp841.tag=2;_tmp841.f1=_tmp850;_tmp841;});_tmp840[0]=_tmpE3F;});_tmp840;});_tmp83F->hd=_tmpE40;});_tmp83F->tl=effect;_tmp83F;});effect=_tmpE41;});goto _LL73F;default: _LL74E: _LL74F:
# 4390
({struct Cyc_List_List*_tmpE46=({struct Cyc_List_List*_tmp845=_cycalloc(sizeof(*_tmp845));({void*_tmpE45=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp846=_cycalloc(sizeof(*_tmp846));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE44=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp847;_tmp847.tag=25;({void*_tmpE43=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp848=_cycalloc(sizeof(*_tmp848));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE42=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp849;_tmp849.tag=2;_tmp849.f1=_tmp850;_tmp849;});_tmp848[0]=_tmpE42;});_tmp848;});_tmp847.f1=_tmpE43;});_tmp847;});_tmp846[0]=_tmpE44;});_tmp846;});_tmp845->hd=_tmpE45;});_tmp845->tl=effect;_tmp845;});effect=_tmpE46;});goto _LL73F;}}default: _LL74C: _tmp84E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp839)->f1;_LL74D:
# 4387
({struct Cyc_Core_Opt*_tmpE49=({struct Cyc_Core_Opt*_tmp842=_cycalloc(sizeof(*_tmp842));({void*_tmpE48=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp843=_cycalloc(sizeof(*_tmp843));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE47=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp844;_tmp844.tag=2;_tmp844.f1=0;_tmp844.f2=& Cyc_Tcutil_ak;_tmp844;});_tmp843[0]=_tmpE47;});_tmp843;});_tmp842->v=_tmpE48;});_tmp842;});*_tmp84E=_tmpE49;});goto _LL74F;}_LL73F:;};}}
# 4394
{struct Cyc_List_List*ts=_tmp7F3.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp851=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp852=_tmp851;void*_tmp85D;int _tmp85C;_LL751: _tmp85D=_tmp852.f1;_tmp85C=_tmp852.f2;_LL752:;
if(!_tmp85C)continue;{
struct Cyc_Absyn_Kind*_tmp853=Cyc_Tcutil_typ_kind(_tmp85D);struct Cyc_Absyn_Kind*_tmp854=_tmp853;switch(((struct Cyc_Absyn_Kind*)_tmp854)->kind){case Cyc_Absyn_RgnKind: _LL754: _LL755:
# 4399
({struct Cyc_List_List*_tmpE4C=({struct Cyc_List_List*_tmp855=_cycalloc(sizeof(*_tmp855));({void*_tmpE4B=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp856=_cycalloc(sizeof(*_tmp856));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE4A=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp857;_tmp857.tag=23;_tmp857.f1=_tmp85D;_tmp857;});_tmp856[0]=_tmpE4A;});_tmp856;});_tmp855->hd=_tmpE4B;});_tmp855->tl=effect;_tmp855;});effect=_tmpE4C;});goto _LL753;case Cyc_Absyn_EffKind: _LL756: _LL757:
# 4401
({struct Cyc_List_List*_tmpE4D=({struct Cyc_List_List*_tmp858=_cycalloc(sizeof(*_tmp858));_tmp858->hd=_tmp85D;_tmp858->tl=effect;_tmp858;});effect=_tmpE4D;});goto _LL753;case Cyc_Absyn_IntKind: _LL758: _LL759:
 goto _LL753;default: _LL75A: _LL75B:
# 4404
({struct Cyc_List_List*_tmpE50=({struct Cyc_List_List*_tmp859=_cycalloc(sizeof(*_tmp859));({void*_tmpE4F=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp85A=_cycalloc(sizeof(*_tmp85A));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE4E=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp85B;_tmp85B.tag=25;_tmp85B.f1=_tmp85D;_tmp85B;});_tmp85A[0]=_tmpE4E;});_tmp85A;});_tmp859->hd=_tmpE4F;});_tmp859->tl=effect;_tmp859;});effect=_tmpE50;});goto _LL753;}_LL753:;};}}
# 4407
({void*_tmpE52=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp85E=_cycalloc(sizeof(*_tmp85E));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE51=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp85F;_tmp85F.tag=24;_tmp85F.f1=effect;_tmp85F;});_tmp85E[0]=_tmpE51;});_tmp85E;});*_tmp8F0=_tmpE52;});}
# 4413
if(*_tmp8F1 != 0){
struct Cyc_List_List*bs=*_tmp8F1;for(0;bs != 0;bs=bs->tl){
void*_tmp860=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp861=_tmp860;struct Cyc_Core_Opt**_tmp871;struct Cyc_Absyn_Kind*_tmp870;struct Cyc_Core_Opt**_tmp86F;struct Cyc_Core_Opt**_tmp86E;struct Cyc_Core_Opt**_tmp86D;struct Cyc_Core_Opt**_tmp86C;struct Cyc_Core_Opt**_tmp86B;struct Cyc_Core_Opt**_tmp86A;struct Cyc_Core_Opt**_tmp869;struct Cyc_Core_Opt**_tmp868;struct Cyc_Core_Opt**_tmp867;switch(*((int*)_tmp861)){case 1: _LL75D: _tmp867=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL75E:
# 4417
({struct Cyc_String_pa_PrintArg_struct _tmp864;_tmp864.tag=0;_tmp864.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp862[1]={& _tmp864};({unsigned int _tmpE54=loc;struct _dyneither_ptr _tmpE53=({const char*_tmp863="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp863,sizeof(char),47);});Cyc_Tcutil_warn(_tmpE54,_tmpE53,_tag_dyneither(_tmp862,sizeof(void*),1));});});});
# 4419
_tmp868=_tmp867;goto _LL760;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f2)->aliasqual == Cyc_Absyn_Top){_LL75F: _tmp868=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL760:
 _tmp869=_tmp868;goto _LL762;}else{goto _LL76F;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f2)->aliasqual){case Cyc_Absyn_Top: _LL761: _tmp869=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL762:
 _tmp86A=_tmp869;goto _LL764;case Cyc_Absyn_Aliasable: _LL763: _tmp86A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL764:
 _tmp86C=_tmp86A;goto _LL766;default: _LL769: _tmp86B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL76A:
# 4426
 _tmp86E=_tmp86B;goto _LL76C;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f2)->aliasqual){case Cyc_Absyn_Top: _LL765: _tmp86C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL766:
# 4423
 _tmp86D=_tmp86C;goto _LL768;case Cyc_Absyn_Aliasable: _LL767: _tmp86D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL768:
# 4425
({struct Cyc_Core_Opt*_tmpE55=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp86D=_tmpE55;});goto _LL75C;default: _LL76B: _tmp86E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL76C:
# 4428
({struct Cyc_Core_Opt*_tmpE56=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp86E=_tmpE56;});goto _LL75C;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f2)->aliasqual == Cyc_Absyn_Top){_LL76D: _tmp86F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_LL76E:
# 4430
({struct Cyc_Core_Opt*_tmpE57=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp86F=_tmpE57;});goto _LL75C;}else{goto _LL76F;}default: _LL76F: _tmp871=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f1;_tmp870=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp861)->f2;_LL770:
# 4432
({struct Cyc_Core_Opt*_tmpE58=Cyc_Tcutil_kind_to_bound_opt(_tmp870);*_tmp871=_tmpE58;});goto _LL75C;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp861)->f1)->kind == Cyc_Absyn_MemKind){_LL771: _LL772:
# 4434
({void*_tmp865=0;({unsigned int _tmpE5A=loc;struct _dyneither_ptr _tmpE59=({const char*_tmp866="functions can't abstract M types";_tag_dyneither(_tmp866,sizeof(char),33);});Cyc_Tcutil_terr(_tmpE5A,_tmpE59,_tag_dyneither(_tmp865,sizeof(void*),0));});});goto _LL75C;}else{_LL773: _LL774:
 goto _LL75C;}}_LL75C:;}}
# 4439
({struct Cyc_List_List*_tmpE5B=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp7F3.kind_env,*_tmp8F1);cvtenv.kind_env=_tmpE5B;});
({struct Cyc_List_List*_tmpE5C=Cyc_Tcutil_remove_bound_tvars_bool(_tmp7F3.r,_tmp7F3.free_vars,*_tmp8F1);_tmp7F3.free_vars=_tmpE5C;});
# 4442
{struct Cyc_List_List*tvs=_tmp7F3.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp872=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp873=_tmp872;struct Cyc_Absyn_Tvar*_tmp875;int _tmp874;_LL776: _tmp875=_tmp873.f1;_tmp874=_tmp873.f2;_LL777:;
({struct Cyc_List_List*_tmpE5D=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp875,_tmp874);cvtenv.free_vars=_tmpE5D;});}}
# 4447
{struct Cyc_List_List*evs=_tmp7F3.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp876=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp877=_tmp876;void*_tmp879;int _tmp878;_LL779: _tmp879=_tmp877.f1;_tmp878=_tmp877.f2;_LL77A:;
({struct Cyc_List_List*_tmpE5E=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp879,_tmp878);cvtenv.free_evars=_tmpE5E;});}}
# 4451
goto _LL66C;};};};}case 10: _LL68F: _tmp8F2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp760)->f1;_LL690:
# 4454
 for(0;_tmp8F2 != 0;_tmp8F2=_tmp8F2->tl){
struct _tuple12*_tmp882=(struct _tuple12*)_tmp8F2->hd;
({struct Cyc_Tcutil_CVTEnv _tmpE5F=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp882).f2,1,0);cvtenv=_tmpE5F;});
({int _tmpE60=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp882).f1).print_const,(*_tmp882).f2);
# 4457
((*_tmp882).f1).real_const=_tmpE60;});}
# 4460
goto _LL66C;case 12: _LL691: _tmp8F4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp760)->f1;_tmp8F3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp760)->f2;_LL692: {
# 4464
struct Cyc_List_List*prev_fields=0;
for(0;_tmp8F3 != 0;_tmp8F3=_tmp8F3->tl){
struct Cyc_Absyn_Aggrfield*_tmp883=(struct Cyc_Absyn_Aggrfield*)_tmp8F3->hd;struct Cyc_Absyn_Aggrfield*_tmp884=_tmp883;struct _dyneither_ptr*_tmp894;struct Cyc_Absyn_Tqual*_tmp893;void*_tmp892;struct Cyc_Absyn_Exp*_tmp891;struct Cyc_List_List*_tmp890;struct Cyc_Absyn_Exp*_tmp88F;_LL77C: _tmp894=_tmp884->name;_tmp893=(struct Cyc_Absyn_Tqual*)& _tmp884->tq;_tmp892=_tmp884->type;_tmp891=_tmp884->width;_tmp890=_tmp884->attributes;_tmp88F=_tmp884->requires_clause;_LL77D:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp894))
({struct Cyc_String_pa_PrintArg_struct _tmp887;_tmp887.tag=0;_tmp887.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp894);({void*_tmp885[1]={& _tmp887};({unsigned int _tmpE62=loc;struct _dyneither_ptr _tmpE61=({const char*_tmp886="duplicate field %s";_tag_dyneither(_tmp886,sizeof(char),19);});Cyc_Tcutil_terr(_tmpE62,_tmpE61,_tag_dyneither(_tmp885,sizeof(void*),1));});});});
if(({({struct _dyneither_ptr _tmpE63=(struct _dyneither_ptr)*_tmp894;Cyc_strcmp(_tmpE63,({const char*_tmp888="";_tag_dyneither(_tmp888,sizeof(char),1);}));})!= 0;}))
({struct Cyc_List_List*_tmpE64=({struct Cyc_List_List*_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889->hd=_tmp894;_tmp889->tl=prev_fields;_tmp889;});prev_fields=_tmpE64;});
({struct Cyc_Tcutil_CVTEnv _tmpE65=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp892,1,0);cvtenv=_tmpE65;});
({int _tmpE66=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp893->print_const,_tmp892);_tmp893->real_const=_tmpE66;});
# 4477
Cyc_Tcutil_check_bitfield(loc,te,_tmp892,_tmp891,_tmp894);
Cyc_Tcutil_check_field_atts(loc,_tmp894,_tmp890);
if(_tmp88F != 0){
# 4481
if(_tmp8F4 != Cyc_Absyn_UnionA)
({void*_tmp88A=0;({unsigned int _tmpE68=loc;struct _dyneither_ptr _tmpE67=({const char*_tmp88B="@requires clause is only allowed on union members";_tag_dyneither(_tmp88B,sizeof(char),50);});Cyc_Tcutil_terr(_tmpE68,_tmpE67,_tag_dyneither(_tmp88A,sizeof(void*),0));});});
({struct Cyc_Tcenv_Tenv*_tmpE69=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpE69,0,_tmp88F);});
if(!Cyc_Tcutil_is_integral(_tmp88F))
({struct Cyc_String_pa_PrintArg_struct _tmp88E;_tmp88E.tag=0;({struct _dyneither_ptr _tmpE6A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp88F->topt)));_tmp88E.f1=_tmpE6A;});({void*_tmp88C[1]={& _tmp88E};({unsigned int _tmpE6C=loc;struct _dyneither_ptr _tmpE6B=({const char*_tmp88D="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp88D,sizeof(char),54);});Cyc_Tcutil_terr(_tmpE6C,_tmpE6B,_tag_dyneither(_tmp88C,sizeof(void*),1));});});});
({struct Cyc_Tcutil_CVTEnv _tmpE6D=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp88F,te,cvtenv);cvtenv=_tmpE6D;});}}
# 4490
goto _LL66C;}case 11: _LL693: _tmp8F6=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp760)->f1).aggr_info;_tmp8F5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp760)->f1).targs;_LL694:
# 4493
{union Cyc_Absyn_AggrInfoU _tmp895=*_tmp8F6;union Cyc_Absyn_AggrInfoU _tmp896=_tmp895;struct Cyc_Absyn_Aggrdecl*_tmp8BA;enum Cyc_Absyn_AggrKind _tmp8B9;struct _tuple2*_tmp8B8;struct Cyc_Core_Opt*_tmp8B7;if((_tmp896.UnknownAggr).tag == 1){_LL77F: _tmp8B9=((_tmp896.UnknownAggr).val).f1;_tmp8B8=((_tmp896.UnknownAggr).val).f2;_tmp8B7=((_tmp896.UnknownAggr).val).f3;_LL780: {
# 4495
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp897;_push_handler(& _tmp897);{int _tmp899=0;if(setjmp(_tmp897.handler))_tmp899=1;if(!_tmp899){
({struct Cyc_Absyn_Aggrdecl**_tmpE6E=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8B8);adp=_tmpE6E;});{
struct Cyc_Absyn_Aggrdecl*_tmp89A=*adp;
if(_tmp89A->kind != _tmp8B9){
if(_tmp89A->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp89E;_tmp89E.tag=0;({struct _dyneither_ptr _tmpE6F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8B8));_tmp89E.f1=_tmpE6F;});({struct Cyc_String_pa_PrintArg_struct _tmp89D;_tmp89D.tag=0;({struct _dyneither_ptr _tmpE70=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4501
Cyc_Absynpp_qvar2string(_tmp8B8));_tmp89D.f1=_tmpE70;});({void*_tmp89B[2]={& _tmp89D,& _tmp89E};({unsigned int _tmpE72=loc;struct _dyneither_ptr _tmpE71=({const char*_tmp89C="expecting struct %s instead of union %s";_tag_dyneither(_tmp89C,sizeof(char),40);});Cyc_Tcutil_terr(_tmpE72,_tmpE71,_tag_dyneither(_tmp89B,sizeof(void*),2));});});});});else{
# 4504
({struct Cyc_String_pa_PrintArg_struct _tmp8A2;_tmp8A2.tag=0;({struct _dyneither_ptr _tmpE73=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8B8));_tmp8A2.f1=_tmpE73;});({struct Cyc_String_pa_PrintArg_struct _tmp8A1;_tmp8A1.tag=0;({struct _dyneither_ptr _tmpE74=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4504
Cyc_Absynpp_qvar2string(_tmp8B8));_tmp8A1.f1=_tmpE74;});({void*_tmp89F[2]={& _tmp8A1,& _tmp8A2};({unsigned int _tmpE76=loc;struct _dyneither_ptr _tmpE75=({const char*_tmp8A0="expecting union %s instead of struct %s";_tag_dyneither(_tmp8A0,sizeof(char),40);});Cyc_Tcutil_terr(_tmpE76,_tmpE75,_tag_dyneither(_tmp89F,sizeof(void*),2));});});});});}}
# 4507
if((unsigned int)_tmp8B7  && (int)_tmp8B7->v){
if(!((unsigned int)_tmp89A->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp89A->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp8A5;_tmp8A5.tag=0;({struct _dyneither_ptr _tmpE77=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8B8));_tmp8A5.f1=_tmpE77;});({void*_tmp8A3[1]={& _tmp8A5};({unsigned int _tmpE79=loc;struct _dyneither_ptr _tmpE78=({const char*_tmp8A4="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp8A4,sizeof(char),42);});Cyc_Tcutil_terr(_tmpE79,_tmpE78,_tag_dyneither(_tmp8A3,sizeof(void*),1));});});});}
# 4513
({union Cyc_Absyn_AggrInfoU _tmpE7A=Cyc_Absyn_KnownAggr(adp);*_tmp8F6=_tmpE7A;});};
# 4497
;_pop_handler();}else{void*_tmp898=(void*)_exn_thrown;void*_tmp8A6=_tmp898;void*_tmp8AC;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8A6)->tag == Cyc_Dict_Absent){_LL784: _LL785: {
# 4517
struct Cyc_Absyn_Aggrdecl*_tmp8A7=({struct Cyc_Absyn_Aggrdecl*_tmp8AB=_cycalloc(sizeof(*_tmp8AB));_tmp8AB->kind=_tmp8B9;_tmp8AB->sc=Cyc_Absyn_Extern;_tmp8AB->name=_tmp8B8;_tmp8AB->tvs=0;_tmp8AB->impl=0;_tmp8AB->attributes=0;_tmp8AB->expected_mem_kind=0;_tmp8AB;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp8A7);
({struct Cyc_Absyn_Aggrdecl**_tmpE7B=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8B8);adp=_tmpE7B;});
({union Cyc_Absyn_AggrInfoU _tmpE7C=Cyc_Absyn_KnownAggr(adp);*_tmp8F6=_tmpE7C;});
# 4522
if(*_tmp8F5 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8AA;_tmp8AA.tag=0;({struct _dyneither_ptr _tmpE7D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8B8));_tmp8AA.f1=_tmpE7D;});({void*_tmp8A8[1]={& _tmp8AA};({unsigned int _tmpE7F=loc;struct _dyneither_ptr _tmpE7E=({const char*_tmp8A9="declare parameterized type %s before using";_tag_dyneither(_tmp8A9,sizeof(char),43);});Cyc_Tcutil_terr(_tmpE7F,_tmpE7E,_tag_dyneither(_tmp8A8,sizeof(void*),1));});});});
return cvtenv;}
# 4527
goto _LL783;}}else{_LL786: _tmp8AC=_tmp8A6;_LL787:(int)_rethrow(_tmp8AC);}_LL783:;}};}
# 4529
_tmp8BA=*adp;goto _LL782;}}else{_LL781: _tmp8BA=*(_tmp896.KnownAggr).val;_LL782: {
# 4531
struct Cyc_List_List*tvs=_tmp8BA->tvs;
struct Cyc_List_List*ts=*_tmp8F5;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp8AD=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8AE=(void*)ts->hd;
# 4539
{struct _tuple0 _tmp8AF=({struct _tuple0 _tmp8B2;({void*_tmpE80=Cyc_Absyn_compress_kb(_tmp8AD->kind);_tmp8B2.f1=_tmpE80;});_tmp8B2.f2=_tmp8AE;_tmp8B2;});struct _tuple0 _tmp8B0=_tmp8AF;struct Cyc_Absyn_Tvar*_tmp8B1;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B0.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8B0.f2)->tag == 2){_LL789: _tmp8B1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8B0.f2)->f1;_LL78A:
# 4541
({struct Cyc_List_List*_tmpE81=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8B1);cvtenv.kind_env=_tmpE81;});
({struct Cyc_List_List*_tmpE82=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8B1,1);cvtenv.free_vars=_tmpE82;});
continue;}else{goto _LL78B;}}else{_LL78B: _LL78C:
 goto _LL788;}_LL788:;}{
# 4546
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpE83=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpE83;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4550
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8B5;_tmp8B5.tag=0;({struct _dyneither_ptr _tmpE84=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8BA->name));_tmp8B5.f1=_tmpE84;});({void*_tmp8B3[1]={& _tmp8B5};({unsigned int _tmpE86=loc;struct _dyneither_ptr _tmpE85=({const char*_tmp8B4="too many parameters for type %s";_tag_dyneither(_tmp8B4,sizeof(char),32);});Cyc_Tcutil_terr(_tmpE86,_tmpE85,_tag_dyneither(_tmp8B3,sizeof(void*),1));});});});
if(tvs != 0){
# 4554
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpE87=({struct Cyc_List_List*_tmp8B6=_cycalloc(sizeof(*_tmp8B6));_tmp8B6->hd=e;_tmp8B6->tl=hidden_ts;_tmp8B6;});hidden_ts=_tmpE87;});
({struct Cyc_Tcutil_CVTEnv _tmpE88=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpE88;});}
# 4561
({struct Cyc_List_List*_tmpE8A=({struct Cyc_List_List*_tmpE89=*_tmp8F5;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE89,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp8F5=_tmpE8A;});}
# 4563
if((allow_abs_aggr  && _tmp8BA->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4568
_tmp8BA->expected_mem_kind=1;}}_LL77E:;}
# 4571
goto _LL66C;case 17: _LL695: _tmp8FA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp760)->f1;_tmp8F9=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp760)->f2;_tmp8F8=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp760)->f3;_tmp8F7=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp760)->f4;_LL696: {
# 4574
struct Cyc_List_List*targs=*_tmp8F9;
# 4576
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp8BB;_push_handler(& _tmp8BB);{int _tmp8BD=0;if(setjmp(_tmp8BB.handler))_tmp8BD=1;if(!_tmp8BD){({struct Cyc_Absyn_Typedefdecl*_tmpE8B=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp8FA);td=_tmpE8B;});;_pop_handler();}else{void*_tmp8BC=(void*)_exn_thrown;void*_tmp8BE=_tmp8BC;void*_tmp8C2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8BE)->tag == Cyc_Dict_Absent){_LL78E: _LL78F:
# 4579
({struct Cyc_String_pa_PrintArg_struct _tmp8C1;_tmp8C1.tag=0;({struct _dyneither_ptr _tmpE8C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8FA));_tmp8C1.f1=_tmpE8C;});({void*_tmp8BF[1]={& _tmp8C1};({unsigned int _tmpE8E=loc;struct _dyneither_ptr _tmpE8D=({const char*_tmp8C0="unbound typedef name %s";_tag_dyneither(_tmp8C0,sizeof(char),24);});Cyc_Tcutil_terr(_tmpE8E,_tmpE8D,_tag_dyneither(_tmp8BF,sizeof(void*),1));});});});
return cvtenv;}else{_LL790: _tmp8C2=_tmp8BE;_LL791:(int)_rethrow(_tmp8C2);}_LL78D:;}};}
# 4582
*_tmp8F8=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4587
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4592
({struct Cyc_Tcutil_CVTEnv _tmpE8F=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpE8F;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmpE91=({struct Cyc_List_List*_tmp8C3=_cycalloc(sizeof(*_tmp8C3));({struct _tuple16*_tmpE90=({struct _tuple16*_tmp8C4=_cycalloc(sizeof(*_tmp8C4));_tmp8C4->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8C4->f2=(void*)ts->hd;_tmp8C4;});_tmp8C3->hd=_tmpE90;});_tmp8C3->tl=inst;_tmp8C3;});inst=_tmpE91;});}
# 4596
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8C7;_tmp8C7.tag=0;({struct _dyneither_ptr _tmpE92=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8FA));_tmp8C7.f1=_tmpE92;});({void*_tmp8C5[1]={& _tmp8C7};({unsigned int _tmpE94=loc;struct _dyneither_ptr _tmpE93=({const char*_tmp8C6="too many parameters for typedef %s";_tag_dyneither(_tmp8C6,sizeof(char),35);});Cyc_Tcutil_terr(_tmpE94,_tmpE93,_tag_dyneither(_tmp8C5,sizeof(void*),1));});});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4601
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpE95=({struct Cyc_List_List*_tmp8C8=_cycalloc(sizeof(*_tmp8C8));_tmp8C8->hd=e;_tmp8C8->tl=hidden_ts;_tmp8C8;});hidden_ts=_tmpE95;});
({struct Cyc_Tcutil_CVTEnv _tmpE96=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpE96;});
({struct Cyc_List_List*_tmpE98=({struct Cyc_List_List*_tmp8C9=_cycalloc(sizeof(*_tmp8C9));({struct _tuple16*_tmpE97=({struct _tuple16*_tmp8CA=_cycalloc(sizeof(*_tmp8CA));_tmp8CA->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8CA->f2=e;_tmp8CA;});_tmp8C9->hd=_tmpE97;});_tmp8C9->tl=inst;_tmp8C9;});inst=_tmpE98;});}
# 4609
({struct Cyc_List_List*_tmpE9A=({struct Cyc_List_List*_tmpE99=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE99,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp8F9=_tmpE9A;});}
# 4611
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp8F7=new_typ;}
# 4617
goto _LL66C;};}case 22: _LL697: _LL698:
 goto _LL69A;case 21: _LL699: _LL69A:
 goto _LL69C;case 20: _LL69B: _LL69C:
 goto _LL66C;case 15: _LL69D: _tmp8FB=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp760)->f1;_LL69E:
# 4622
({struct Cyc_Tcutil_CVTEnv _tmpE9B=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp8FB,1,allow_abs_aggr);cvtenv=_tmpE9B;});goto _LL66C;case 16: _LL69F: _tmp8FD=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp760)->f1;_tmp8FC=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp760)->f2;_LL6A0:
# 4625
({struct Cyc_Tcutil_CVTEnv _tmpE9C=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp8FD,0,0);cvtenv=_tmpE9C;});
({struct Cyc_Tcutil_CVTEnv _tmpE9D=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp8FC,1,0);cvtenv=_tmpE9D;});
goto _LL66C;case 23: _LL6A1: _tmp8FE=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp760)->f1;_LL6A2:
# 4629
({struct Cyc_Tcutil_CVTEnv _tmpE9E=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp8FE,1,1);cvtenv=_tmpE9E;});goto _LL66C;case 25: _LL6A3: _tmp8FF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp760)->f1;_LL6A4:
# 4631
({struct Cyc_Tcutil_CVTEnv _tmpE9F=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp8FF,1,1);cvtenv=_tmpE9F;});goto _LL66C;default: _LL6A5: _tmp900=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp760)->f1;_LL6A6:
# 4633
 for(0;_tmp900 != 0;_tmp900=_tmp900->tl){
({struct Cyc_Tcutil_CVTEnv _tmpEA0=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp900->hd,1,1);cvtenv=_tmpEA0;});}
goto _LL66C;}_LL66C:;}
# 4637
if(!({struct Cyc_Absyn_Kind*_tmpEA1=Cyc_Tcutil_typ_kind(t);Cyc_Tcutil_kind_leq(_tmpEA1,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp905;_tmp905.tag=0;({struct _dyneither_ptr _tmpEA2=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));_tmp905.f1=_tmpEA2;});({struct Cyc_String_pa_PrintArg_struct _tmp904;_tmp904.tag=0;({struct _dyneither_ptr _tmpEA3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));_tmp904.f1=_tmpEA3;});({struct Cyc_String_pa_PrintArg_struct _tmp903;_tmp903.tag=0;({struct _dyneither_ptr _tmpEA4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp903.f1=_tmpEA4;});({void*_tmp901[3]={& _tmp903,& _tmp904,& _tmp905};({unsigned int _tmpEA6=loc;struct _dyneither_ptr _tmpEA5=({const char*_tmp902="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp902,sizeof(char),51);});Cyc_Tcutil_terr(_tmpEA6,_tmpEA5,_tag_dyneither(_tmp901,sizeof(void*),3));});});});});});
# 4641
return cvtenv;}
# 4649
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4651
{void*_tmp906=e->r;void*_tmp907=_tmp906;struct Cyc_Absyn_Exp*_tmp919;void*_tmp918;void*_tmp917;void*_tmp916;void*_tmp915;struct Cyc_Absyn_Exp*_tmp914;struct Cyc_Absyn_Exp*_tmp913;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp910;struct Cyc_Absyn_Exp*_tmp90F;struct Cyc_Absyn_Exp*_tmp90E;struct Cyc_Absyn_Exp*_tmp90D;struct Cyc_Absyn_Exp*_tmp90C;struct Cyc_Absyn_Exp*_tmp90B;struct Cyc_List_List*_tmp90A;switch(*((int*)_tmp907)){case 0: _LL793: _LL794:
 goto _LL796;case 31: _LL795: _LL796:
 goto _LL798;case 32: _LL797: _LL798:
 goto _LL79A;case 1: _LL799: _LL79A:
 goto _LL792;case 2: _LL79B: _tmp90A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp907)->f2;_LL79C:
# 4657
 for(0;_tmp90A != 0;_tmp90A=_tmp90A->tl){
({struct Cyc_Tcutil_CVTEnv _tmpEA7=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp90A->hd,te,cvtenv);cvtenv=_tmpEA7;});}
goto _LL792;case 5: _LL79D: _tmp90D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_tmp90C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp907)->f2;_tmp90B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp907)->f3;_LL79E:
# 4661
({struct Cyc_Tcutil_CVTEnv _tmpEA8=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp90D,te,cvtenv);cvtenv=_tmpEA8;});
({struct Cyc_Tcutil_CVTEnv _tmpEA9=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp90C,te,cvtenv);cvtenv=_tmpEA9;});
({struct Cyc_Tcutil_CVTEnv _tmpEAA=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp90B,te,cvtenv);cvtenv=_tmpEAA;});
goto _LL792;case 6: _LL79F: _tmp90F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_tmp90E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp907)->f2;_LL7A0:
 _tmp911=_tmp90F;_tmp910=_tmp90E;goto _LL7A2;case 7: _LL7A1: _tmp911=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_tmp910=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp907)->f2;_LL7A2:
 _tmp913=_tmp911;_tmp912=_tmp910;goto _LL7A4;case 8: _LL7A3: _tmp913=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_tmp912=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp907)->f2;_LL7A4:
# 4668
({struct Cyc_Tcutil_CVTEnv _tmpEAB=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp913,te,cvtenv);cvtenv=_tmpEAB;});
({struct Cyc_Tcutil_CVTEnv _tmpEAC=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp912,te,cvtenv);cvtenv=_tmpEAC;});
goto _LL792;case 13: _LL7A5: _tmp915=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_tmp914=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp907)->f2;_LL7A6:
# 4672
({struct Cyc_Tcutil_CVTEnv _tmpEAD=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp914,te,cvtenv);cvtenv=_tmpEAD;});
({struct Cyc_Tcutil_CVTEnv _tmpEAE=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp915,1,0);cvtenv=_tmpEAE;});
goto _LL792;case 18: _LL7A7: _tmp916=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LL7A8:
 _tmp917=_tmp916;goto _LL7AA;case 16: _LL7A9: _tmp917=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LL7AA:
# 4677
({struct Cyc_Tcutil_CVTEnv _tmpEAF=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp917,1,0);cvtenv=_tmpEAF;});
goto _LL792;case 38: _LL7AB: _tmp918=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LL7AC:
# 4680
({struct Cyc_Tcutil_CVTEnv _tmpEB0=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp918,1,0);cvtenv=_tmpEB0;});
goto _LL792;case 17: _LL7AD: _tmp919=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp907)->f1;_LL7AE:
# 4683
({struct Cyc_Tcutil_CVTEnv _tmpEB1=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp919,te,cvtenv);cvtenv=_tmpEB1;});
goto _LL792;default: _LL7AF: _LL7B0:
# 4686
({void*_tmp908=0;({struct _dyneither_ptr _tmpEB2=({const char*_tmp909="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp909,sizeof(char),66);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEB2,_tag_dyneither(_tmp908,sizeof(void*),0));});});}_LL792:;}
# 4688
return cvtenv;}
# 4691
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4696
struct Cyc_List_List*_tmp91A=cvt.kind_env;
({struct Cyc_Tcutil_CVTEnv _tmpEB3=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);cvt=_tmpEB3;});
# 4699
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp91B=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp91C=_tmp91B;struct Cyc_Absyn_Tvar*_tmp91D;_LL7B2: _tmp91D=_tmp91C.f1;_LL7B3:;
({struct Cyc_List_List*_tmpEB4=Cyc_Tcutil_fast_add_free_tvar(_tmp91A,_tmp91D);cvt.kind_env=_tmpEB4;});}}
# 4707
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp91E=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp91F=_tmp91E;void*_tmp928;_LL7B5: _tmp928=_tmp91F.f1;_LL7B6:;{
void*_tmp920=Cyc_Tcutil_compress(_tmp928);void*_tmp921=_tmp920;struct Cyc_Core_Opt**_tmp927;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp921)->tag == 1){_LL7B8: _tmp927=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp921)->f4;_LL7B9:
# 4711
 if(*_tmp927 == 0)
({struct Cyc_Core_Opt*_tmpEB5=({struct Cyc_Core_Opt*_tmp922=_cycalloc(sizeof(*_tmp922));_tmp922->v=_tmp91A;_tmp922;});*_tmp927=_tmpEB5;});else{
# 4715
struct Cyc_List_List*_tmp923=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp927))->v;
struct Cyc_List_List*_tmp924=0;
for(0;_tmp923 != 0;_tmp923=_tmp923->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp91A,(struct Cyc_Absyn_Tvar*)_tmp923->hd))
({struct Cyc_List_List*_tmpEB6=({struct Cyc_List_List*_tmp925=_cycalloc(sizeof(*_tmp925));_tmp925->hd=(struct Cyc_Absyn_Tvar*)_tmp923->hd;_tmp925->tl=_tmp924;_tmp925;});_tmp924=_tmpEB6;});}
({struct Cyc_Core_Opt*_tmpEB7=({struct Cyc_Core_Opt*_tmp926=_cycalloc(sizeof(*_tmp926));_tmp926->v=_tmp924;_tmp926;});*_tmp927=_tmpEB7;});}
# 4722
goto _LL7B7;}else{_LL7BA: _LL7BB:
 goto _LL7B7;}_LL7B7:;};}}
# 4726
return cvt;}
# 4732
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp929=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp92A=_tmp929;switch(((struct Cyc_Absyn_Kind*)_tmp92A)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp92A)->aliasqual){case Cyc_Absyn_Unique: _LL7BD: _LL7BE:
# 4737
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp92B=0;({struct _dyneither_ptr _tmpEB8=({const char*_tmp92C="can't unify evar with unique region!";_tag_dyneither(_tmp92C,sizeof(char),37);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEB8,_tag_dyneither(_tmp92B,sizeof(void*),0));});});
goto _LL7BC;case Cyc_Absyn_Aliasable: _LL7BF: _LL7C0:
 goto _LL7C2;default: _LL7C1: _LL7C2:
# 4742
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp92D=0;({struct _dyneither_ptr _tmpEB9=({const char*_tmp92E="can't unify evar with heap!";_tag_dyneither(_tmp92E,sizeof(char),28);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEB9,_tag_dyneither(_tmp92D,sizeof(void*),0));});});
goto _LL7BC;}case Cyc_Absyn_EffKind: _LL7C3: _LL7C4:
# 4745
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp92F=0;({struct _dyneither_ptr _tmpEBA=({const char*_tmp930="can't unify evar with {}!";_tag_dyneither(_tmp930,sizeof(char),26);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEBA,_tag_dyneither(_tmp92F,sizeof(void*),0));});});
goto _LL7BC;default: _LL7C5: _LL7C6:
# 4748
({struct Cyc_String_pa_PrintArg_struct _tmp934;_tmp934.tag=0;({struct _dyneither_ptr _tmpEBB=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));_tmp934.f1=_tmpEBB;});({struct Cyc_String_pa_PrintArg_struct _tmp933;_tmp933.tag=0;({struct _dyneither_ptr _tmpEBC=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp933.f1=_tmpEBC;});({void*_tmp931[2]={& _tmp933,& _tmp934};({unsigned int _tmpEBE=loc;struct _dyneither_ptr _tmpEBD=({const char*_tmp932="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp932,sizeof(char),52);});Cyc_Tcutil_terr(_tmpEBE,_tmpEBD,_tag_dyneither(_tmp931,sizeof(void*),2));});});});});
goto _LL7BC;}_LL7BC:;}}
# 4760
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp935=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp936=({unsigned int _tmpEC2=loc;struct Cyc_Tcenv_Tenv*_tmpEC1=te;struct Cyc_Tcutil_CVTEnv _tmpEC0=({struct Cyc_Tcutil_CVTEnv _tmp954;_tmp954.r=Cyc_Core_heap_region;_tmp954.kind_env=_tmp935;_tmp954.free_vars=0;_tmp954.free_evars=0;_tmp954.generalize_evars=generalize_evars;_tmp954.fn_result=0;_tmp954;});struct Cyc_Absyn_Kind*_tmpEBF=expected_kind;Cyc_Tcutil_check_valid_type(_tmpEC2,_tmpEC1,_tmpEC0,_tmpEBF,1,t);});
# 4768
struct Cyc_List_List*_tmp937=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp936.free_vars);
struct Cyc_List_List*_tmp938=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp936.free_evars);
# 4772
if(_tmp935 != 0){
struct Cyc_List_List*_tmp939=0;
{struct Cyc_List_List*_tmp93A=_tmp937;for(0;(unsigned int)_tmp93A;_tmp93A=_tmp93A->tl){
struct Cyc_Absyn_Tvar*_tmp93B=(struct Cyc_Absyn_Tvar*)_tmp93A->hd;
int found=0;
{struct Cyc_List_List*_tmp93C=_tmp935;for(0;(unsigned int)_tmp93C;_tmp93C=_tmp93C->tl){
if(({Cyc_Absyn_tvar_cmp(_tmp93B,(struct Cyc_Absyn_Tvar*)_tmp93C->hd)== 0;})){found=1;break;}}}
if(!found)
({struct Cyc_List_List*_tmpEC3=({struct Cyc_List_List*_tmp93D=_cycalloc(sizeof(*_tmp93D));_tmp93D->hd=(struct Cyc_Absyn_Tvar*)_tmp93A->hd;_tmp93D->tl=_tmp939;_tmp93D;});_tmp939=_tmpEC3;});}}
# 4782
({struct Cyc_List_List*_tmpEC4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp939);_tmp937=_tmpEC4;});}
# 4788
{struct Cyc_List_List*x=_tmp937;for(0;x != 0;x=x->tl){
void*_tmp93E=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp93F=_tmp93E;enum Cyc_Absyn_AliasQual _tmp94D;struct Cyc_Core_Opt**_tmp94C;struct Cyc_Absyn_Kind*_tmp94B;struct Cyc_Core_Opt**_tmp94A;struct Cyc_Core_Opt**_tmp949;struct Cyc_Core_Opt**_tmp948;struct Cyc_Core_Opt**_tmp947;struct Cyc_Core_Opt**_tmp946;struct Cyc_Core_Opt**_tmp945;switch(*((int*)_tmp93F)){case 1: _LL7C8: _tmp945=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp93F)->f1;_LL7C9:
 _tmp946=_tmp945;goto _LL7CB;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f2)->aliasqual == Cyc_Absyn_Top){_LL7CA: _tmp946=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f1;_LL7CB:
 _tmp947=_tmp946;goto _LL7CD;}else{goto _LL7D4;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7CC: _tmp947=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f1;_LL7CD:
 _tmp948=_tmp947;goto _LL7CF;case Cyc_Absyn_Aliasable: _LL7CE: _tmp948=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f1;_LL7CF:
# 4794
({struct Cyc_Core_Opt*_tmpEC5=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp948=_tmpEC5;});goto _LL7C7;default: _LL7D0: _tmp949=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f1;_LL7D1:
# 4796
({struct Cyc_Core_Opt*_tmpEC6=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp949=_tmpEC6;});goto _LL7C7;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f2)->aliasqual == Cyc_Absyn_Top){_LL7D2: _tmp94A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f1;_LL7D3:
# 4798
({struct Cyc_Core_Opt*_tmpEC7=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp94A=_tmpEC7;});goto _LL7C7;}else{goto _LL7D4;}default: _LL7D4: _tmp94C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f1;_tmp94B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp93F)->f2;_LL7D5:
# 4800
({struct Cyc_Core_Opt*_tmpEC8=Cyc_Tcutil_kind_to_bound_opt(_tmp94B);*_tmp94C=_tmpEC8;});goto _LL7C7;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp93F)->f1)->kind == Cyc_Absyn_MemKind){_LL7D6: _tmp94D=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp93F)->f1)->aliasqual;_LL7D7:
# 4802
({struct Cyc_String_pa_PrintArg_struct _tmp943;_tmp943.tag=0;({struct _dyneither_ptr _tmpEC9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp944=_cycalloc_atomic(sizeof(*_tmp944));_tmp944->kind=Cyc_Absyn_MemKind;_tmp944->aliasqual=_tmp94D;_tmp944;})));_tmp943.f1=_tmpEC9;});({struct Cyc_String_pa_PrintArg_struct _tmp942;_tmp942.tag=0;({struct _dyneither_ptr _tmpECA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmp942.f1=_tmpECA;});({void*_tmp940[2]={& _tmp942,& _tmp943};({unsigned int _tmpECC=loc;struct _dyneither_ptr _tmpECB=({const char*_tmp941="type variable %s cannot have kind %s";_tag_dyneither(_tmp941,sizeof(char),37);});Cyc_Tcutil_terr(_tmpECC,_tmpECB,_tag_dyneither(_tmp940,sizeof(void*),2));});});});});
goto _LL7C7;}else{_LL7D8: _LL7D9:
 goto _LL7C7;}}_LL7C7:;}}
# 4809
if(_tmp937 != 0  || _tmp938 != 0){
{void*_tmp94E=Cyc_Tcutil_compress(t);void*_tmp94F=_tmp94E;struct Cyc_List_List**_tmp950;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94F)->tag == 9){_LL7DB: _tmp950=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp94F)->f1).tvars;_LL7DC:
# 4812
 if(*_tmp950 == 0){
# 4814
({struct Cyc_List_List*_tmpECD=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp937);*_tmp950=_tmpECD;});
_tmp937=0;}
# 4817
goto _LL7DA;}else{_LL7DD: _LL7DE:
 goto _LL7DA;}_LL7DA:;}
# 4820
if(_tmp937 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp953;_tmp953.tag=0;_tmp953.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp937->hd)->name);({void*_tmp951[1]={& _tmp953};({unsigned int _tmpECF=loc;struct _dyneither_ptr _tmpECE=({const char*_tmp952="unbound type variable %s";_tag_dyneither(_tmp952,sizeof(char),25);});Cyc_Tcutil_terr(_tmpECF,_tmpECE,_tag_dyneither(_tmp951,sizeof(void*),1));});});});
Cyc_Tcutil_check_free_evars(_tmp938,t,loc);}}
# 4830
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4833
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp955=Cyc_Tcutil_compress(t);void*_tmp956=_tmp955;struct Cyc_List_List*_tmp962;void*_tmp961;struct Cyc_Absyn_Tqual _tmp960;void*_tmp95F;struct Cyc_List_List*_tmp95E;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_List_List*_tmp95C;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_List_List*_tmp95A;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->tag == 9){_LL7E0: _tmp962=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).tvars;_tmp961=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).effect;_tmp960=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).ret_tqual;_tmp95F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).ret_typ;_tmp95E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).args;_tmp95D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).requires_clause;_tmp95C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).requires_relns;_tmp95B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).ensures_clause;_tmp95A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp956)->f1).ensures_relns;_LL7E1:
# 4836
 fd->tvs=_tmp962;
fd->effect=_tmp961;
{struct Cyc_List_List*_tmp957=fd->args;for(0;_tmp957 != 0;(_tmp957=_tmp957->tl,_tmp95E=_tmp95E->tl)){
# 4840
(*((struct _tuple10*)_tmp957->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp95E))->hd)).f2;
(*((struct _tuple10*)_tmp957->hd)).f3=(*((struct _tuple10*)_tmp95E->hd)).f3;}}
# 4843
fd->ret_tqual=_tmp960;
fd->ret_type=_tmp95F;
({int _tmpED0=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp95F);(fd->ret_tqual).real_const=_tmpED0;});
fd->requires_clause=_tmp95D;
fd->requires_relns=_tmp95C;
fd->ensures_clause=_tmp95B;
fd->ensures_relns=_tmp95A;
goto _LL7DF;}else{_LL7E2: _LL7E3:
({void*_tmp958=0;({struct _dyneither_ptr _tmpED1=({const char*_tmp959="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp959,sizeof(char),38);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpED1,_tag_dyneither(_tmp958,sizeof(void*),0));});});}_LL7DF:;}
# 4853
({struct Cyc_List_List*_tmpED3=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args);unsigned int _tmpED2=loc;Cyc_Tcutil_check_unique_vars(_tmpED3,_tmpED2,({const char*_tmp963="function declaration has repeated parameter";_tag_dyneither(_tmp963,sizeof(char),44);}));});
# 4856
fd->cached_typ=t;}
# 4861
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4867
struct Cyc_Tcutil_CVTEnv _tmp964=({unsigned int _tmpED8=loc;struct Cyc_Tcenv_Tenv*_tmpED7=te;struct Cyc_Tcutil_CVTEnv _tmpED6=({struct Cyc_Tcutil_CVTEnv _tmp96C;_tmp96C.r=Cyc_Core_heap_region;_tmp96C.kind_env=bound_tvars;_tmp96C.free_vars=0;_tmp96C.free_evars=0;_tmp96C.generalize_evars=0;_tmp96C.fn_result=0;_tmp96C;});struct Cyc_Absyn_Kind*_tmpED5=expected_kind;int _tmpED4=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpED8,_tmpED7,_tmpED6,_tmpED5,_tmpED4,t);});
# 4871
struct Cyc_List_List*_tmp965=({struct _RegionHandle*_tmpEDA=Cyc_Core_heap_region;struct Cyc_List_List*_tmpED9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp964.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpEDA,_tmpED9,bound_tvars);});
# 4873
struct Cyc_List_List*_tmp966=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp964.free_evars);
{struct Cyc_List_List*fs=_tmp965;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp967=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp96B;_tmp96B.tag=0;({struct _dyneither_ptr _tmpEDB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp96B.f1=_tmpEDB;});({struct Cyc_String_pa_PrintArg_struct _tmp96A;_tmp96A.tag=0;_tmp96A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp967);({void*_tmp968[2]={& _tmp96A,& _tmp96B};({unsigned int _tmpEDD=loc;struct _dyneither_ptr _tmpEDC=({const char*_tmp969="unbound type variable %s in type %s";_tag_dyneither(_tmp969,sizeof(char),36);});Cyc_Tcutil_terr(_tmpEDD,_tmpEDC,_tag_dyneither(_tmp968,sizeof(void*),2));});});});});}}
# 4878
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp966,t,loc);}
# 4882
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpEDE=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpEDE;});}
# 4887
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4893
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4896
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(({cmp(vs->hd,vs2->hd)== 0;}))
({struct Cyc_String_pa_PrintArg_struct _tmp970;_tmp970.tag=0;({struct _dyneither_ptr _tmpEDF=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmp970.f1=_tmpEDF;});({struct Cyc_String_pa_PrintArg_struct _tmp96F;_tmp96F.tag=0;_tmp96F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp96D[2]={& _tmp96F,& _tmp970};({unsigned int _tmpEE1=loc;struct _dyneither_ptr _tmpEE0=({const char*_tmp96E="%s: %s";_tag_dyneither(_tmp96E,sizeof(char),7);});Cyc_Tcutil_terr(_tmpEE1,_tmpEE0,_tag_dyneither(_tmp96D,sizeof(void*),2));});});});});}}}
# 4902
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4906
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4910
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpEE3=tvs;unsigned int _tmpEE2=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpEE3,_tmpEE2,Cyc_Absynpp_tvar2string,({const char*_tmp971="duplicate type variable";_tag_dyneither(_tmp971,sizeof(char),24);}));});}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4924 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4929
struct _RegionHandle _tmp972=_new_region("temp");struct _RegionHandle*temp=& _tmp972;_push_region(temp);
# 4933
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({({struct _dyneither_ptr _tmpEE4=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpEE4,({const char*_tmp973="";_tag_dyneither(_tmp973,sizeof(char),1);}));})!= 0;}))
({struct Cyc_List_List*_tmpEE6=({struct Cyc_List_List*_tmp974=_region_malloc(temp,sizeof(*_tmp974));({struct _tuple32*_tmpEE5=({struct _tuple32*_tmp975=_region_malloc(temp,sizeof(*_tmp975));_tmp975->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp975->f2=0;_tmp975;});_tmp974->hd=_tmpEE5;});_tmp974->tl=fields;_tmp974;});fields=_tmpEE6;});}}
# 4938
({struct Cyc_List_List*_tmpEE7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);fields=_tmpEE7;});{
# 4940
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp99B="struct";_tag_dyneither(_tmp99B,sizeof(char),7);}):({const char*_tmp99C="union";_tag_dyneither(_tmp99C,sizeof(char),6);});
# 4943
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp976=(struct _tuple33*)des->hd;struct _tuple33*_tmp977=_tmp976;struct Cyc_List_List*_tmp993;void*_tmp992;_LL7E5: _tmp993=_tmp977->f1;_tmp992=_tmp977->f2;_LL7E6:;
if(_tmp993 == 0){
# 4948
struct Cyc_List_List*_tmp978=fields;
for(0;_tmp978 != 0;_tmp978=_tmp978->tl){
if(!(*((struct _tuple32*)_tmp978->hd)).f2){
(*((struct _tuple32*)_tmp978->hd)).f2=1;
({struct Cyc_List_List*_tmpEEA=({struct Cyc_List_List*_tmp979=_cycalloc(sizeof(*_tmp979));({void*_tmpEE9=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp97A=_cycalloc(sizeof(*_tmp97A));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpEE8=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp97B;_tmp97B.tag=1;_tmp97B.f1=((*((struct _tuple32*)_tmp978->hd)).f1)->name;_tmp97B;});_tmp97A[0]=_tmpEE8;});_tmp97A;});_tmp979->hd=_tmpEE9;});_tmp979->tl=0;_tmp979;});(*((struct _tuple33*)des->hd)).f1=_tmpEEA;});
({struct Cyc_List_List*_tmpEEC=({struct Cyc_List_List*_tmp97C=_region_malloc(rgn,sizeof(*_tmp97C));({struct _tuple34*_tmpEEB=({struct _tuple34*_tmp97D=_region_malloc(rgn,sizeof(*_tmp97D));_tmp97D->f1=(*((struct _tuple32*)_tmp978->hd)).f1;_tmp97D->f2=_tmp992;_tmp97D;});_tmp97C->hd=_tmpEEB;});_tmp97C->tl=ans;_tmp97C;});ans=_tmpEEC;});
break;}}
# 4956
if(_tmp978 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp980;_tmp980.tag=0;_tmp980.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp97E[1]={& _tmp980};({unsigned int _tmpEEE=loc;struct _dyneither_ptr _tmpEED=({const char*_tmp97F="too many arguments to %s";_tag_dyneither(_tmp97F,sizeof(char),25);});Cyc_Tcutil_terr(_tmpEEE,_tmpEED,_tag_dyneither(_tmp97E,sizeof(void*),1));});});});}else{
if(_tmp993->tl != 0)
# 4960
({void*_tmp981=0;({unsigned int _tmpEF0=loc;struct _dyneither_ptr _tmpEEF=({const char*_tmp982="multiple designators are not yet supported";_tag_dyneither(_tmp982,sizeof(char),43);});Cyc_Tcutil_terr(_tmpEF0,_tmpEEF,_tag_dyneither(_tmp981,sizeof(void*),0));});});else{
# 4963
void*_tmp983=(void*)_tmp993->hd;void*_tmp984=_tmp983;struct _dyneither_ptr*_tmp991;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp984)->tag == 0){_LL7E8: _LL7E9:
# 4965
({struct Cyc_String_pa_PrintArg_struct _tmp987;_tmp987.tag=0;_tmp987.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp985[1]={& _tmp987};({unsigned int _tmpEF2=loc;struct _dyneither_ptr _tmpEF1=({const char*_tmp986="array designator used in argument to %s";_tag_dyneither(_tmp986,sizeof(char),40);});Cyc_Tcutil_terr(_tmpEF2,_tmpEF1,_tag_dyneither(_tmp985,sizeof(void*),1));});});});
goto _LL7E7;}else{_LL7EA: _tmp991=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp984)->f1;_LL7EB: {
# 4968
struct Cyc_List_List*_tmp988=fields;
for(0;_tmp988 != 0;_tmp988=_tmp988->tl){
if(({Cyc_strptrcmp(_tmp991,((*((struct _tuple32*)_tmp988->hd)).f1)->name)== 0;})){
if((*((struct _tuple32*)_tmp988->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp98B;_tmp98B.tag=0;_tmp98B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp991);({void*_tmp989[1]={& _tmp98B};({unsigned int _tmpEF4=loc;struct _dyneither_ptr _tmpEF3=({const char*_tmp98A="member %s has already been used as an argument";_tag_dyneither(_tmp98A,sizeof(char),47);});Cyc_Tcutil_terr(_tmpEF4,_tmpEF3,_tag_dyneither(_tmp989,sizeof(void*),1));});});});
(*((struct _tuple32*)_tmp988->hd)).f2=1;
({struct Cyc_List_List*_tmpEF6=({struct Cyc_List_List*_tmp98C=_region_malloc(rgn,sizeof(*_tmp98C));({struct _tuple34*_tmpEF5=({struct _tuple34*_tmp98D=_region_malloc(rgn,sizeof(*_tmp98D));_tmp98D->f1=(*((struct _tuple32*)_tmp988->hd)).f1;_tmp98D->f2=_tmp992;_tmp98D;});_tmp98C->hd=_tmpEF5;});_tmp98C->tl=ans;_tmp98C;});ans=_tmpEF6;});
break;}}
# 4977
if(_tmp988 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp990;_tmp990.tag=0;_tmp990.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp991);({void*_tmp98E[1]={& _tmp990};({unsigned int _tmpEF8=loc;struct _dyneither_ptr _tmpEF7=({const char*_tmp98F="bad field designator %s";_tag_dyneither(_tmp98F,sizeof(char),24);});Cyc_Tcutil_terr(_tmpEF8,_tmpEF7,_tag_dyneither(_tmp98E,sizeof(void*),1));});});});
goto _LL7E7;}}_LL7E7:;}}}
# 4982
if(aggr_kind == Cyc_Absyn_StructA)
# 4984
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp994=0;({unsigned int _tmpEFA=loc;struct _dyneither_ptr _tmpEF9=({const char*_tmp995="too few arguments to struct";_tag_dyneither(_tmp995,sizeof(char),28);});Cyc_Tcutil_terr(_tmpEFA,_tmpEF9,_tag_dyneither(_tmp994,sizeof(void*),0));});});
break;}}else{
# 4991
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp996=0;({unsigned int _tmpEFC=loc;struct _dyneither_ptr _tmpEFB=({const char*_tmp997="only one member of a union is allowed";_tag_dyneither(_tmp997,sizeof(char),38);});Cyc_Tcutil_terr(_tmpEFC,_tmpEFB,_tag_dyneither(_tmp996,sizeof(void*),0));});});
found=1;}}
# 4998
if(!found)({void*_tmp998=0;({unsigned int _tmpEFE=loc;struct _dyneither_ptr _tmpEFD=({const char*_tmp999="missing member for union";_tag_dyneither(_tmp999,sizeof(char),25);});Cyc_Tcutil_terr(_tmpEFE,_tmpEFD,_tag_dyneither(_tmp998,sizeof(void*),0));});});}{
# 5001
struct Cyc_List_List*_tmp99A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmp99A;};};}
# 4933
;_pop_region(temp);}
# 5004
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp99D=Cyc_Tcutil_compress(t);void*_tmp99E=_tmp99D;void*_tmp9A2;union Cyc_Absyn_Constraint*_tmp9A1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99E)->tag == 5){_LL7ED: _tmp9A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99E)->f1).elt_typ;_tmp9A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99E)->f1).ptr_atts).bounds;_LL7EE: {
# 5007
void*_tmp99F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9A1);void*_tmp9A0=_tmp99F;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9A0)->tag == 0){_LL7F2: _LL7F3:
# 5009
*elt_typ_dest=_tmp9A2;
return 1;}else{_LL7F4: _LL7F5:
 return 0;}_LL7F1:;}}else{_LL7EF: _LL7F0:
# 5013
 return 0;}_LL7EC:;}
# 5017
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9A3=Cyc_Tcutil_compress(t);void*_tmp9A4=_tmp9A3;void*_tmp9A6;union Cyc_Absyn_Constraint*_tmp9A5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A4)->tag == 5){_LL7F7: _tmp9A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A4)->f1).elt_typ;_tmp9A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A4)->f1).ptr_atts).zero_term;_LL7F8:
# 5020
*elt_typ_dest=_tmp9A6;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9A5);}else{_LL7F9: _LL7FA:
 return 0;}_LL7F6:;}
# 5028
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5030
void*_tmp9A7=Cyc_Tcutil_compress(t);void*_tmp9A8=_tmp9A7;void*_tmp9B1;struct Cyc_Absyn_Tqual _tmp9B0;struct Cyc_Absyn_Exp*_tmp9AF;union Cyc_Absyn_Constraint*_tmp9AE;void*_tmp9AD;union Cyc_Absyn_Constraint*_tmp9AC;union Cyc_Absyn_Constraint*_tmp9AB;switch(*((int*)_tmp9A8)){case 5: _LL7FC: _tmp9AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A8)->f1).elt_typ;_tmp9AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A8)->f1).ptr_atts).bounds;_tmp9AB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A8)->f1).ptr_atts).zero_term;_LL7FD:
# 5032
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9AB)){
*ptr_type=t;
*elt_type=_tmp9AD;
{void*_tmp9A9=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp9AC);void*_tmp9AA=_tmp9A9;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9AA)->tag == 0){_LL803: _LL804:
*is_dyneither=1;goto _LL802;}else{_LL805: _LL806:
*is_dyneither=0;goto _LL802;}_LL802:;}
# 5039
return 1;}else{
return 0;}case 8: _LL7FE: _tmp9B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A8)->f1).elt_type;_tmp9B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A8)->f1).tq;_tmp9AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A8)->f1).num_elts;_tmp9AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A8)->f1).zero_term;_LL7FF:
# 5042
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9AE)){
*elt_type=_tmp9B1;
*is_dyneither=0;
({void*_tmpEFF=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);*ptr_type=_tmpEFF;});
return 1;}else{
return 0;}default: _LL800: _LL801:
 return 0;}_LL7FB:;}
# 5055
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5057
void*_tmp9B2=e1->r;void*_tmp9B3=_tmp9B2;struct Cyc_Absyn_Exp*_tmp9C5;struct Cyc_Absyn_Exp*_tmp9C4;struct Cyc_Absyn_Exp*_tmp9C3;struct Cyc_Absyn_Exp*_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C1;struct Cyc_Absyn_Exp*_tmp9C0;switch(*((int*)_tmp9B3)){case 13: _LL808: _LL809:
# 5059
({struct Cyc_String_pa_PrintArg_struct _tmp9B6;_tmp9B6.tag=0;({struct _dyneither_ptr _tmpF00=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9B6.f1=_tmpF00;});({void*_tmp9B4[1]={& _tmp9B6};({struct _dyneither_ptr _tmpF01=({const char*_tmp9B5="we have a cast in a lhs:  %s";_tag_dyneither(_tmp9B5,sizeof(char),29);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF01,_tag_dyneither(_tmp9B4,sizeof(void*),1));});});});case 19: _LL80A: _tmp9C0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9B3)->f1;_LL80B:
 _tmp9C1=_tmp9C0;goto _LL80D;case 22: _LL80C: _tmp9C1=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9B3)->f1;_LL80D:
# 5062
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9C1->topt),ptr_type,is_dyneither,elt_type);case 21: _LL80E: _tmp9C2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9B3)->f1;_LL80F:
 _tmp9C3=_tmp9C2;goto _LL811;case 20: _LL810: _tmp9C3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9B3)->f1;_LL811:
# 5066
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9C3->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9B9;_tmp9B9.tag=0;({struct _dyneither_ptr _tmpF02=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));_tmp9B9.f1=_tmpF02;});({void*_tmp9B7[1]={& _tmp9B9};({struct _dyneither_ptr _tmpF03=({const char*_tmp9B8="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp9B8,sizeof(char),51);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF03,_tag_dyneither(_tmp9B7,sizeof(void*),1));});});});
return 0;case 12: _LL812: _tmp9C4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9B3)->f1;_LL813:
 _tmp9C5=_tmp9C4;goto _LL815;case 11: _LL814: _tmp9C5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9B3)->f1;_LL815:
# 5072
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9C5->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9BC;_tmp9BC.tag=0;({struct _dyneither_ptr _tmpF04=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));_tmp9BC.f1=_tmpF04;});({void*_tmp9BA[1]={& _tmp9BC};({struct _dyneither_ptr _tmpF05=({const char*_tmp9BB="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp9BB,sizeof(char),49);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF05,_tag_dyneither(_tmp9BA,sizeof(void*),1));});});});
return 0;case 1: _LL816: _LL817:
 return 0;default: _LL818: _LL819:
({struct Cyc_String_pa_PrintArg_struct _tmp9BF;_tmp9BF.tag=0;({struct _dyneither_ptr _tmpF06=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9BF.f1=_tmpF06;});({void*_tmp9BD[1]={& _tmp9BF};({struct _dyneither_ptr _tmpF07=({const char*_tmp9BE="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp9BE,sizeof(char),39);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF07,_tag_dyneither(_tmp9BD,sizeof(void*),1));});});});}_LL807:;}
# 5081
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5092
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5095
void*_tmp9C6=Cyc_Tcutil_compress(r);void*_tmp9C7=_tmp9C6;struct Cyc_Absyn_Tvar*_tmp9D4;enum Cyc_Absyn_KindQual _tmp9D3;enum Cyc_Absyn_AliasQual _tmp9D2;switch(*((int*)_tmp9C7)){case 22: _LL81B: _LL81C:
 return !must_be_unique;case 21: _LL81D: _LL81E:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C7)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C7)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C7)->f4 == 0){_LL81F: _tmp9D3=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C7)->f3)->kind)->v)->kind;_tmp9D2=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C7)->f3)->kind)->v)->aliasqual;_LL820:
# 5099
 return _tmp9D3 == Cyc_Absyn_RgnKind  && (_tmp9D2 == Cyc_Absyn_Unique  || _tmp9D2 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL823;}}else{goto _LL823;}}else{goto _LL823;}case 2: _LL821: _tmp9D4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9C7)->f1;_LL822: {
# 5103
struct Cyc_Absyn_Kind*_tmp9C8=Cyc_Tcutil_tvar_kind(_tmp9D4,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp9C9=_tmp9C8;enum Cyc_Absyn_KindQual _tmp9D1;enum Cyc_Absyn_AliasQual _tmp9D0;_LL826: _tmp9D1=_tmp9C9->kind;_tmp9D0=_tmp9C9->aliasqual;_LL827:;
if(_tmp9D1 == Cyc_Absyn_RgnKind  && (_tmp9D0 == Cyc_Absyn_Unique  || _tmp9D0 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp9CA=Cyc_Absyn_compress_kb(_tmp9D4->kind);void*_tmp9CB=_tmp9CA;struct Cyc_Core_Opt**_tmp9CF;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9CB)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9CB)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9CB)->f2)->aliasqual == Cyc_Absyn_Top){_LL829: _tmp9CF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9CB)->f1;_LL82A:
# 5107
({struct Cyc_Core_Opt*_tmpF0A=({struct Cyc_Core_Opt*_tmp9CC=_cycalloc(sizeof(*_tmp9CC));({void*_tmpF09=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9CD=_cycalloc(sizeof(*_tmp9CD));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF08=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9CE;_tmp9CE.tag=2;_tmp9CE.f1=0;_tmp9CE.f2=& Cyc_Tcutil_rk;_tmp9CE;});_tmp9CD[0]=_tmpF08;});_tmp9CD;});_tmp9CC->v=_tmpF09;});_tmp9CC;});*_tmp9CF=_tmpF0A;});
return 0;}else{goto _LL82B;}}else{goto _LL82B;}}else{_LL82B: _LL82C:
 return 1;}_LL828:;}
# 5112
return 0;}default: _LL823: _LL824:
 return 0;}_LL81A:;}
# 5119
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp9D5=Cyc_Tcutil_compress(t);void*_tmp9D6=_tmp9D5;struct Cyc_Absyn_Tvar*_tmp9E5;void*_tmp9E4;switch(*((int*)_tmp9D6)){case 5: _LL82E: _tmp9E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9D6)->f1).ptr_atts).rgn;_LL82F:
# 5122
 return Cyc_Tcutil_is_noalias_region(_tmp9E4,must_be_unique);case 2: _LL830: _tmp9E5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D6)->f1;_LL831: {
# 5124
struct Cyc_Absyn_Kind*_tmp9D7=Cyc_Tcutil_tvar_kind(_tmp9E5,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp9D8=_tmp9D7;enum Cyc_Absyn_KindQual _tmp9E3;enum Cyc_Absyn_AliasQual _tmp9E2;_LL835: _tmp9E3=_tmp9D8->kind;_tmp9E2=_tmp9D8->aliasqual;_LL836:;{
enum Cyc_Absyn_KindQual _tmp9D9=_tmp9E3;switch(_tmp9D9){case Cyc_Absyn_BoxKind: _LL838: _LL839:
 goto _LL83B;case Cyc_Absyn_AnyKind: _LL83A: _LL83B: goto _LL83D;case Cyc_Absyn_MemKind: _LL83C: _LL83D:
 if(_tmp9E2 == Cyc_Absyn_Unique  || _tmp9E2 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp9DA=Cyc_Absyn_compress_kb(_tmp9E5->kind);void*_tmp9DB=_tmp9DA;struct Cyc_Core_Opt**_tmp9E1;enum Cyc_Absyn_KindQual _tmp9E0;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DB)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DB)->f2)->aliasqual == Cyc_Absyn_Top){_LL841: _tmp9E1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DB)->f1;_tmp9E0=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DB)->f2)->kind;_LL842:
# 5130
({struct Cyc_Core_Opt*_tmpF0E=({struct Cyc_Core_Opt*_tmp9DC=_cycalloc(sizeof(*_tmp9DC));({void*_tmpF0D=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9DD=_cycalloc(sizeof(*_tmp9DD));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF0C=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9DE;_tmp9DE.tag=2;_tmp9DE.f1=0;({struct Cyc_Absyn_Kind*_tmpF0B=({struct Cyc_Absyn_Kind*_tmp9DF=_cycalloc_atomic(sizeof(*_tmp9DF));_tmp9DF->kind=_tmp9E0;_tmp9DF->aliasqual=Cyc_Absyn_Aliasable;_tmp9DF;});_tmp9DE.f2=_tmpF0B;});_tmp9DE;});_tmp9DD[0]=_tmpF0C;});_tmp9DD;});_tmp9DC->v=_tmpF0D;});_tmp9DC;});*_tmp9E1=_tmpF0E;});
return 0;}else{goto _LL843;}}else{_LL843: _LL844:
# 5134
 return 1;}_LL840:;}
# 5137
return 0;default: _LL83E: _LL83F:
 return 0;}_LL837:;};}default: _LL832: _LL833:
# 5140
 return 0;}_LL82D:;}
# 5143
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp9E6=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp9E6,0))return 1;{
void*_tmp9E7=_tmp9E6;union Cyc_Absyn_DatatypeFieldInfoU _tmp9FB;struct Cyc_List_List*_tmp9FA;union Cyc_Absyn_DatatypeInfoU _tmp9F9;struct Cyc_List_List*_tmp9F8;struct Cyc_List_List*_tmp9F7;struct Cyc_Absyn_Aggrdecl**_tmp9F6;struct Cyc_List_List*_tmp9F5;struct Cyc_List_List*_tmp9F4;switch(*((int*)_tmp9E7)){case 10: _LL846: _tmp9F4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9E7)->f1;_LL847:
# 5148
 while(_tmp9F4 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp9F4->hd)).f2))return 1;
_tmp9F4=_tmp9F4->tl;}
# 5152
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E7)->f1).aggr_info).KnownAggr).tag == 2){_LL848: _tmp9F6=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E7)->f1).aggr_info).KnownAggr).val;_tmp9F5=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E7)->f1).targs;_LL849:
# 5154
 if((*_tmp9F6)->impl == 0)return 0;else{
# 5156
struct Cyc_List_List*_tmp9E8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp9F6)->tvs,_tmp9F5);
struct Cyc_List_List*_tmp9E9=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp9F6)->impl))->fields;
void*t;
while(_tmp9E9 != 0){
({void*_tmpF0F=_tmp9E8 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp9E9->hd)->type: Cyc_Tcutil_substitute(_tmp9E8,((struct Cyc_Absyn_Aggrfield*)_tmp9E9->hd)->type);t=_tmpF0F;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp9E9=_tmp9E9->tl;}
# 5164
return 0;}}else{_LL84C: _LL84D:
# 5174
 return 0;}case 12: _LL84A: _tmp9F7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9E7)->f2;_LL84B:
# 5167
 while(_tmp9F7 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp9F7->hd)->type))return 1;
_tmp9F7=_tmp9F7->tl;}
# 5171
return 0;case 3: _LL84E: _tmp9F9=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9E7)->f1).datatype_info;_tmp9F8=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9E7)->f1).targs;_LL84F: {
# 5176
union Cyc_Absyn_DatatypeInfoU _tmp9EA=_tmp9F9;struct Cyc_List_List*_tmp9EE;struct Cyc_Core_Opt*_tmp9ED;struct _tuple2*_tmp9EC;int _tmp9EB;if((_tmp9EA.UnknownDatatype).tag == 1){_LL855: _tmp9EC=((_tmp9EA.UnknownDatatype).val).name;_tmp9EB=((_tmp9EA.UnknownDatatype).val).is_extensible;_LL856:
# 5179
 return 0;}else{_LL857: _tmp9EE=(*(_tmp9EA.KnownDatatype).val)->tvs;_tmp9ED=(*(_tmp9EA.KnownDatatype).val)->fields;_LL858:
# 5182
 return 0;}_LL854:;}case 4: _LL850: _tmp9FB=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9E7)->f1).field_info;_tmp9FA=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9E7)->f1).targs;_LL851: {
# 5185
union Cyc_Absyn_DatatypeFieldInfoU _tmp9EF=_tmp9FB;struct Cyc_Absyn_Datatypedecl*_tmp9F3;struct Cyc_Absyn_Datatypefield*_tmp9F2;if((_tmp9EF.UnknownDatatypefield).tag == 1){_LL85A: _LL85B:
# 5188
 return 0;}else{_LL85C: _tmp9F3=((_tmp9EF.KnownDatatypefield).val).f1;_tmp9F2=((_tmp9EF.KnownDatatypefield).val).f2;_LL85D: {
# 5190
struct Cyc_List_List*_tmp9F0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp9F3->tvs,_tmp9FA);
struct Cyc_List_List*_tmp9F1=_tmp9F2->typs;
while(_tmp9F1 != 0){
({void*_tmpF10=_tmp9F0 == 0?(*((struct _tuple12*)_tmp9F1->hd)).f2: Cyc_Tcutil_substitute(_tmp9F0,(*((struct _tuple12*)_tmp9F1->hd)).f2);_tmp9E6=_tmpF10;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp9E6))return 1;
_tmp9F1=_tmp9F1->tl;}
# 5197
return 0;}}_LL859:;}default: _LL852: _LL853:
# 5199
 return 0;}_LL845:;};}
# 5206
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp9FC=e->r;void*_tmp9FD=_tmp9FC;struct Cyc_Absyn_Stmt*_tmpA13;struct Cyc_Absyn_Exp*_tmpA12;struct Cyc_Absyn_Exp*_tmpA11;struct Cyc_Absyn_Exp*_tmpA10;struct Cyc_Absyn_Exp*_tmpA0F;struct Cyc_Absyn_Exp*_tmpA0E;struct Cyc_Absyn_Exp*_tmpA0D;struct Cyc_Absyn_Exp*_tmpA0C;struct _dyneither_ptr*_tmpA0B;struct Cyc_Absyn_Exp*_tmpA0A;struct Cyc_Absyn_Exp*_tmpA09;switch(*((int*)_tmp9FD)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9FD)->f1)->tag == 1){_LL85F: _LL860:
 return 0;}else{goto _LL871;}case 21: _LL861: _tmpA09=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9FD)->f1;_LL862:
 _tmpA0A=_tmpA09;goto _LL864;case 19: _LL863: _tmpA0A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9FD)->f1;_LL864:
# 5211
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA0A->topt),1) && Cyc_Tcutil_is_noalias_path(_tmpA0A);case 20: _LL865: _tmpA0C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9FD)->f1;_tmpA0B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9FD)->f2;_LL866:
 return Cyc_Tcutil_is_noalias_path(_tmpA0C);case 22: _LL867: _tmpA0E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9FD)->f1;_tmpA0D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9FD)->f2;_LL868: {
# 5214
void*_tmp9FE=Cyc_Tcutil_compress((void*)_check_null(_tmpA0E->topt));void*_tmp9FF=_tmp9FE;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9FF)->tag == 10){_LL874: _LL875:
 return Cyc_Tcutil_is_noalias_path(_tmpA0E);}else{_LL876: _LL877:
 return 0;}_LL873:;}case 5: _LL869: _tmpA10=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9FD)->f2;_tmpA0F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9FD)->f3;_LL86A:
# 5219
 return Cyc_Tcutil_is_noalias_path(_tmpA10) && Cyc_Tcutil_is_noalias_path(_tmpA0F);case 8: _LL86B: _tmpA11=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9FD)->f2;_LL86C:
 _tmpA12=_tmpA11;goto _LL86E;case 13: _LL86D: _tmpA12=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9FD)->f2;_LL86E:
 return Cyc_Tcutil_is_noalias_path(_tmpA12);case 36: _LL86F: _tmpA13=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp9FD)->f1;_LL870:
# 5223
 while(1){
void*_tmpA00=_tmpA13->r;void*_tmpA01=_tmpA00;struct Cyc_Absyn_Exp*_tmpA08;struct Cyc_Absyn_Decl*_tmpA07;struct Cyc_Absyn_Stmt*_tmpA06;struct Cyc_Absyn_Stmt*_tmpA05;struct Cyc_Absyn_Stmt*_tmpA04;switch(*((int*)_tmpA01)){case 2: _LL879: _tmpA05=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA01)->f1;_tmpA04=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA01)->f2;_LL87A:
 _tmpA13=_tmpA04;goto _LL878;case 12: _LL87B: _tmpA07=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA01)->f1;_tmpA06=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA01)->f2;_LL87C:
 _tmpA13=_tmpA06;goto _LL878;case 1: _LL87D: _tmpA08=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA01)->f1;_LL87E:
 return Cyc_Tcutil_is_noalias_path(_tmpA08);default: _LL87F: _LL880:
({void*_tmpA02=0;({struct _dyneither_ptr _tmpF11=({const char*_tmpA03="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA03,sizeof(char),40);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF11,_tag_dyneither(_tmpA02,sizeof(void*),0));});});}_LL878:;}default: _LL871: _LL872:
# 5231
 return 1;}_LL85E:;}
# 5248 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5250
struct _tuple18 bogus_ans=({struct _tuple18 _tmpA5B;_tmpA5B.f1=0;_tmpA5B.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA5B;});
void*_tmpA14=e->r;void*_tmpA15=_tmpA14;struct Cyc_Absyn_Exp*_tmpA5A;struct Cyc_Absyn_Exp*_tmpA59;struct Cyc_Absyn_Exp*_tmpA58;struct Cyc_Absyn_Exp*_tmpA57;struct _dyneither_ptr*_tmpA56;int _tmpA55;struct Cyc_Absyn_Exp*_tmpA54;struct _dyneither_ptr*_tmpA53;int _tmpA52;void*_tmpA51;switch(*((int*)_tmpA15)){case 1: _LL882: _tmpA51=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL883: {
# 5254
void*_tmpA16=_tmpA51;struct Cyc_Absyn_Vardecl*_tmpA23;struct Cyc_Absyn_Vardecl*_tmpA22;struct Cyc_Absyn_Vardecl*_tmpA21;struct Cyc_Absyn_Vardecl*_tmpA20;switch(*((int*)_tmpA16)){case 0: _LL88F: _LL890:
 goto _LL892;case 2: _LL891: _LL892:
 return bogus_ans;case 1: _LL893: _tmpA20=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA16)->f1;_LL894: {
# 5258
void*_tmpA17=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA18=_tmpA17;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA18)->tag == 8){_LL89C: _LL89D:
# 5260
 return({struct _tuple18 _tmpA19;_tmpA19.f1=1;_tmpA19.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA19;});}else{_LL89E: _LL89F:
 return({struct _tuple18 _tmpA1A;_tmpA1A.f1=(_tmpA20->tq).real_const;_tmpA1A.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA1A;});}_LL89B:;}case 4: _LL895: _tmpA21=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA16)->f1;_LL896: {
# 5264
void*_tmpA1B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA1C=_tmpA1B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1C)->tag == 8){_LL8A1: _LL8A2:
 return({struct _tuple18 _tmpA1D;_tmpA1D.f1=1;_tmpA1D.f2=(void*)_check_null(_tmpA21->rgn);_tmpA1D;});}else{_LL8A3: _LL8A4:
# 5267
 _tmpA21->escapes=1;
return({struct _tuple18 _tmpA1E;_tmpA1E.f1=(_tmpA21->tq).real_const;_tmpA1E.f2=(void*)_check_null(_tmpA21->rgn);_tmpA1E;});}_LL8A0:;}case 5: _LL897: _tmpA22=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA16)->f1;_LL898:
# 5270
 _tmpA23=_tmpA22;goto _LL89A;default: _LL899: _tmpA23=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA16)->f1;_LL89A:
# 5272
 _tmpA23->escapes=1;
return({struct _tuple18 _tmpA1F;_tmpA1F.f1=(_tmpA23->tq).real_const;_tmpA1F.f2=(void*)_check_null(_tmpA23->rgn);_tmpA1F;});}_LL88E:;}case 20: _LL884: _tmpA54=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_tmpA53=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA15)->f2;_tmpA52=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA15)->f3;_LL885:
# 5277
 if(_tmpA52)return bogus_ans;{
# 5280
void*_tmpA24=Cyc_Tcutil_compress((void*)_check_null(_tmpA54->topt));void*_tmpA25=_tmpA24;struct Cyc_Absyn_Aggrdecl*_tmpA33;struct Cyc_List_List*_tmpA32;switch(*((int*)_tmpA25)){case 12: _LL8A6: _tmpA32=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA25)->f2;_LL8A7: {
# 5282
struct Cyc_Absyn_Aggrfield*_tmpA26=Cyc_Absyn_lookup_field(_tmpA32,_tmpA53);
if(_tmpA26 != 0  && _tmpA26->width == 0){
struct _tuple18 _tmpA27=Cyc_Tcutil_addressof_props(te,_tmpA54);struct _tuple18 _tmpA28=_tmpA27;int _tmpA2B;void*_tmpA2A;_LL8AD: _tmpA2B=_tmpA28.f1;_tmpA2A=_tmpA28.f2;_LL8AE:;
return({struct _tuple18 _tmpA29;_tmpA29.f1=(_tmpA26->tq).real_const  || _tmpA2B;_tmpA29.f2=_tmpA2A;_tmpA29;});}
# 5287
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA25)->f1).aggr_info).KnownAggr).tag == 2){_LL8A8: _tmpA33=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA25)->f1).aggr_info).KnownAggr).val;_LL8A9: {
# 5289
struct Cyc_Absyn_Aggrfield*_tmpA2C=Cyc_Absyn_lookup_decl_field(_tmpA33,_tmpA53);
if(_tmpA2C != 0  && _tmpA2C->width == 0){
struct _tuple18 _tmpA2D=Cyc_Tcutil_addressof_props(te,_tmpA54);struct _tuple18 _tmpA2E=_tmpA2D;int _tmpA31;void*_tmpA30;_LL8B0: _tmpA31=_tmpA2E.f1;_tmpA30=_tmpA2E.f2;_LL8B1:;
return({struct _tuple18 _tmpA2F;_tmpA2F.f1=(_tmpA2C->tq).real_const  || _tmpA31;_tmpA2F.f2=_tmpA30;_tmpA2F;});}
# 5294
return bogus_ans;}}else{goto _LL8AA;}default: _LL8AA: _LL8AB:
 return bogus_ans;}_LL8A5:;};case 21: _LL886: _tmpA57=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_tmpA56=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA15)->f2;_tmpA55=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA15)->f3;_LL887:
# 5299
 if(_tmpA55)return bogus_ans;{
# 5302
void*_tmpA34=Cyc_Tcutil_compress((void*)_check_null(_tmpA57->topt));void*_tmpA35=_tmpA34;void*_tmpA3C;void*_tmpA3B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA35)->tag == 5){_LL8B3: _tmpA3C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA35)->f1).elt_typ;_tmpA3B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA35)->f1).ptr_atts).rgn;_LL8B4: {
# 5304
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpA36=Cyc_Tcutil_compress(_tmpA3C);void*_tmpA37=_tmpA36;struct Cyc_Absyn_Aggrdecl*_tmpA39;struct Cyc_List_List*_tmpA38;switch(*((int*)_tmpA37)){case 12: _LL8B8: _tmpA38=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA37)->f2;_LL8B9:
# 5307
({struct Cyc_Absyn_Aggrfield*_tmpF12=Cyc_Absyn_lookup_field(_tmpA38,_tmpA56);finfo=_tmpF12;});goto _LL8B7;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA37)->f1).aggr_info).KnownAggr).tag == 2){_LL8BA: _tmpA39=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA37)->f1).aggr_info).KnownAggr).val;_LL8BB:
# 5309
({struct Cyc_Absyn_Aggrfield*_tmpF13=Cyc_Absyn_lookup_decl_field(_tmpA39,_tmpA56);finfo=_tmpF13;});goto _LL8B7;}else{goto _LL8BC;}default: _LL8BC: _LL8BD:
 return bogus_ans;}_LL8B7:;}
# 5312
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpA3A;_tmpA3A.f1=(finfo->tq).real_const;_tmpA3A.f2=_tmpA3B;_tmpA3A;});
return bogus_ans;}}else{_LL8B5: _LL8B6:
 return bogus_ans;}_LL8B2:;};case 19: _LL888: _tmpA58=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_LL889: {
# 5319
void*_tmpA3D=Cyc_Tcutil_compress((void*)_check_null(_tmpA58->topt));void*_tmpA3E=_tmpA3D;struct Cyc_Absyn_Tqual _tmpA41;void*_tmpA40;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3E)->tag == 5){_LL8BF: _tmpA41=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3E)->f1).elt_tq;_tmpA40=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3E)->f1).ptr_atts).rgn;_LL8C0:
# 5321
 return({struct _tuple18 _tmpA3F;_tmpA3F.f1=_tmpA41.real_const;_tmpA3F.f2=_tmpA40;_tmpA3F;});}else{_LL8C1: _LL8C2:
 return bogus_ans;}_LL8BE:;}case 22: _LL88A: _tmpA5A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA15)->f1;_tmpA59=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA15)->f2;_LL88B: {
# 5327
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpA5A->topt));
void*_tmpA42=t;struct Cyc_Absyn_Tqual _tmpA4E;struct Cyc_Absyn_Tqual _tmpA4D;void*_tmpA4C;struct Cyc_List_List*_tmpA4B;switch(*((int*)_tmpA42)){case 10: _LL8C4: _tmpA4B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA42)->f1;_LL8C5: {
# 5331
struct _tuple14 _tmpA43=Cyc_Evexp_eval_const_uint_exp(_tmpA59);struct _tuple14 _tmpA44=_tmpA43;unsigned int _tmpA48;int _tmpA47;_LL8CD: _tmpA48=_tmpA44.f1;_tmpA47=_tmpA44.f2;_LL8CE:;
if(!_tmpA47)
return bogus_ans;{
struct _tuple12*_tmpA45=Cyc_Absyn_lookup_tuple_field(_tmpA4B,(int)_tmpA48);
if(_tmpA45 != 0)
return({struct _tuple18 _tmpA46;_tmpA46.f1=((*_tmpA45).f1).real_const;({void*_tmpF14=(Cyc_Tcutil_addressof_props(te,_tmpA5A)).f2;_tmpA46.f2=_tmpF14;});_tmpA46;});
return bogus_ans;};}case 5: _LL8C6: _tmpA4D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA42)->f1).elt_tq;_tmpA4C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA42)->f1).ptr_atts).rgn;_LL8C7:
# 5339
 return({struct _tuple18 _tmpA49;_tmpA49.f1=_tmpA4D.real_const;_tmpA49.f2=_tmpA4C;_tmpA49;});case 8: _LL8C8: _tmpA4E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA42)->f1).tq;_LL8C9:
# 5345
 return({struct _tuple18 _tmpA4A;_tmpA4A.f1=_tmpA4E.real_const;({void*_tmpF15=(Cyc_Tcutil_addressof_props(te,_tmpA5A)).f2;_tmpA4A.f2=_tmpF15;});_tmpA4A;});default: _LL8CA: _LL8CB:
 return bogus_ans;}_LL8C3:;}default: _LL88C: _LL88D:
# 5349
({void*_tmpA4F=0;({unsigned int _tmpF17=e->loc;struct _dyneither_ptr _tmpF16=({const char*_tmpA50="unary & applied to non-lvalue";_tag_dyneither(_tmpA50,sizeof(char),30);});Cyc_Tcutil_terr(_tmpF17,_tmpF16,_tag_dyneither(_tmpA4F,sizeof(void*),0));});});
return bogus_ans;}_LL881:;}
# 5356
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
({union Cyc_Absyn_Constraint*_tmpF18=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);b=_tmpF18;});{
void*_tmpA5C=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpA5D=_tmpA5C;struct Cyc_Absyn_Exp*_tmpA66;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA5D)->tag == 0){_LL8D0: _LL8D1:
 return;}else{_LL8D2: _tmpA66=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA5D)->f1;_LL8D3: {
# 5361
struct _tuple14 _tmpA5E=Cyc_Evexp_eval_const_uint_exp(_tmpA66);struct _tuple14 _tmpA5F=_tmpA5E;unsigned int _tmpA65;int _tmpA64;_LL8D5: _tmpA65=_tmpA5F.f1;_tmpA64=_tmpA5F.f2;_LL8D6:;
if(_tmpA64  && _tmpA65 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpA63;_tmpA63.tag=1;_tmpA63.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpA62;_tmpA62.tag=1;_tmpA62.f1=(unsigned long)((int)_tmpA65);({void*_tmpA60[2]={& _tmpA62,& _tmpA63};({unsigned int _tmpF1A=loc;struct _dyneither_ptr _tmpF19=({const char*_tmpA61="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpA61,sizeof(char),39);});Cyc_Tcutil_terr(_tmpF1A,_tmpF19,_tag_dyneither(_tmpA60,sizeof(void*),2));});});});});
return;}}_LL8CF:;};}
# 5368
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5372
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpA67=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpA68=_tmpA67;struct Cyc_Absyn_Exp*_tmpA6D;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA68)->tag == 1){_LL8D8: _tmpA6D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA68)->f1;_LL8D9: {
# 5375
struct _tuple14 _tmpA69=Cyc_Evexp_eval_const_uint_exp(_tmpA6D);struct _tuple14 _tmpA6A=_tmpA69;unsigned int _tmpA6C;int _tmpA6B;_LL8DD: _tmpA6C=_tmpA6A.f1;_tmpA6B=_tmpA6A.f2;_LL8DE:;
return _tmpA6B  && _tmpA6C == 1;}}else{_LL8DA: _LL8DB:
 return 0;}_LL8D7:;}
# 5381
int Cyc_Tcutil_bits_only(void*t){
void*_tmpA6E=Cyc_Tcutil_compress(t);void*_tmpA6F=_tmpA6E;struct Cyc_List_List*_tmpA79;struct Cyc_Absyn_Aggrdecl*_tmpA78;struct Cyc_List_List*_tmpA77;struct Cyc_List_List*_tmpA76;void*_tmpA75;union Cyc_Absyn_Constraint*_tmpA74;switch(*((int*)_tmpA6F)){case 0: _LL8E0: _LL8E1:
 goto _LL8E3;case 6: _LL8E2: _LL8E3:
 goto _LL8E5;case 7: _LL8E4: _LL8E5:
 return 1;case 13: _LL8E6: _LL8E7:
 goto _LL8E9;case 14: _LL8E8: _LL8E9:
 return 0;case 8: _LL8EA: _tmpA75=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA6F)->f1).elt_type;_tmpA74=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA6F)->f1).zero_term;_LL8EB:
# 5391
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA74) && Cyc_Tcutil_bits_only(_tmpA75);case 10: _LL8EC: _tmpA76=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA6F)->f1;_LL8ED:
# 5393
 for(0;_tmpA76 != 0;_tmpA76=_tmpA76->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpA76->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA6F)->f1).aggr_info).UnknownAggr).tag == 1){_LL8EE: _LL8EF:
 return 0;}else{_LL8F0: _tmpA78=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA6F)->f1).aggr_info).KnownAggr).val;_tmpA77=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA6F)->f1).targs;_LL8F1:
# 5398
 if(_tmpA78->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA78->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpA70=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA70;_push_region(rgn);
{struct Cyc_List_List*_tmpA71=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA78->tvs,_tmpA77);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA78->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpA71,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA72=0;_npop_handler(0);return _tmpA72;}}}{
int _tmpA73=1;_npop_handler(0);return _tmpA73;};}
# 5405
;_pop_region(rgn);};};}case 12: _LL8F2: _tmpA79=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA6F)->f2;_LL8F3:
# 5410
 for(0;_tmpA79 != 0;_tmpA79=_tmpA79->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpA79->hd)->type))return 0;}
return 1;default: _LL8F4: _LL8F5:
 return 0;}_LL8DF:;}
# 5418
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpA7A=e->r;void*_tmpA7B=_tmpA7A;struct Cyc_Absyn_Exp*_tmpA7D;struct Cyc_Absyn_Exp*_tmpA7C;switch(*((int*)_tmpA7B)){case 1: _LL8F7: _LL8F8:
 return 1;case 11: _LL8F9: _tmpA7C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA7B)->f1;_LL8FA:
 _tmpA7D=_tmpA7C;goto _LL8FC;case 12: _LL8FB: _tmpA7D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA7B)->f1;_LL8FC:
 e=_tmpA7D;continue;default: _LL8FD: _LL8FE:
# 5425
 return 0;}_LL8F6:;}}
# 5435
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpA7E=e->r;void*_tmpA7F=_tmpA7E;struct Cyc_List_List*_tmpA9D;struct Cyc_List_List*_tmpA9C;enum Cyc_Absyn_Primop _tmpA9B;struct Cyc_List_List*_tmpA9A;struct Cyc_List_List*_tmpA99;struct Cyc_List_List*_tmpA98;struct Cyc_List_List*_tmpA97;struct Cyc_Absyn_Exp*_tmpA96;struct Cyc_Absyn_Exp*_tmpA95;struct Cyc_Absyn_Exp*_tmpA94;struct Cyc_Absyn_Exp*_tmpA93;void*_tmpA92;struct Cyc_Absyn_Exp*_tmpA91;void*_tmpA90;struct Cyc_Absyn_Exp*_tmpA8F;struct Cyc_Absyn_Exp*_tmpA8E;struct Cyc_Absyn_Exp*_tmpA8D;struct Cyc_Absyn_Exp*_tmpA8C;struct Cyc_Absyn_Exp*_tmpA8B;struct Cyc_Absyn_Exp*_tmpA8A;struct Cyc_Absyn_Exp*_tmpA89;struct Cyc_Absyn_Exp*_tmpA88;void*_tmpA87;switch(*((int*)_tmpA7F)){case 0: _LL900: _LL901:
 goto _LL903;case 16: _LL902: _LL903:
 goto _LL905;case 17: _LL904: _LL905:
 goto _LL907;case 18: _LL906: _LL907:
 goto _LL909;case 31: _LL908: _LL909:
 goto _LL90B;case 32: _LL90A: _LL90B:
 return 1;case 1: _LL90C: _tmpA87=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_LL90D: {
# 5446
void*_tmpA80=_tmpA87;struct Cyc_Absyn_Vardecl*_tmpA86;struct Cyc_Absyn_Vardecl*_tmpA85;switch(*((int*)_tmpA80)){case 2: _LL92F: _LL930:
 return 1;case 1: _LL931: _tmpA85=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA80)->f1;_LL932: {
# 5449
void*_tmpA81=Cyc_Tcutil_compress(_tmpA85->type);void*_tmpA82=_tmpA81;switch(*((int*)_tmpA82)){case 8: _LL93A: _LL93B:
 goto _LL93D;case 9: _LL93C: _LL93D:
 return 1;default: _LL93E: _LL93F:
 return var_okay;}_LL939:;}case 4: _LL933: _tmpA86=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA80)->f1;_LL934:
# 5456
 if(_tmpA86->sc == Cyc_Absyn_Static){
void*_tmpA83=Cyc_Tcutil_compress(_tmpA86->type);void*_tmpA84=_tmpA83;switch(*((int*)_tmpA84)){case 8: _LL941: _LL942:
 goto _LL944;case 9: _LL943: _LL944:
 return 1;default: _LL945: _LL946:
 return var_okay;}_LL940:;}else{
# 5463
return var_okay;}case 0: _LL935: _LL936:
 return 0;default: _LL937: _LL938:
 return var_okay;}_LL92E:;}case 5: _LL90E: _tmpA8A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_tmpA89=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA7F)->f2;_tmpA88=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA7F)->f3;_LL90F:
# 5469
 return(Cyc_Tcutil_cnst_exp(0,_tmpA8A) && 
Cyc_Tcutil_cnst_exp(0,_tmpA89)) && 
Cyc_Tcutil_cnst_exp(0,_tmpA88);case 8: _LL910: _tmpA8C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_tmpA8B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA7F)->f2;_LL911:
# 5473
 return Cyc_Tcutil_cnst_exp(0,_tmpA8C) && Cyc_Tcutil_cnst_exp(0,_tmpA8B);case 11: _LL912: _tmpA8D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_LL913:
 _tmpA8E=_tmpA8D;goto _LL915;case 12: _LL914: _tmpA8E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_LL915:
# 5476
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA8E);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7F)->f4 == Cyc_Absyn_No_coercion){_LL916: _tmpA90=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_tmpA8F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7F)->f2;_LL917:
# 5478
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA8F);}else{_LL918: _tmpA92=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_tmpA91=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7F)->f2;_LL919:
# 5481
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA91);}case 14: _LL91A: _tmpA93=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_LL91B:
# 5483
 return Cyc_Tcutil_cnst_exp(1,_tmpA93);case 26: _LL91C: _tmpA95=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA7F)->f2;_tmpA94=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA7F)->f3;_LL91D:
# 5485
 return Cyc_Tcutil_cnst_exp(0,_tmpA95) && Cyc_Tcutil_cnst_exp(0,_tmpA94);case 27: _LL91E: _tmpA96=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_LL91F:
# 5487
 return Cyc_Tcutil_cnst_exp(0,_tmpA96);case 25: _LL920: _tmpA97=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_LL921:
 _tmpA98=_tmpA97;goto _LL923;case 29: _LL922: _tmpA98=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA7F)->f2;_LL923:
 _tmpA99=_tmpA98;goto _LL925;case 28: _LL924: _tmpA99=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA7F)->f3;_LL925:
# 5491
 for(0;_tmpA99 != 0;_tmpA99=_tmpA99->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpA99->hd)).f2))
return 0;}
return 1;case 2: _LL926: _tmpA9B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_tmpA9A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA7F)->f2;_LL927:
# 5496
 _tmpA9C=_tmpA9A;goto _LL929;case 23: _LL928: _tmpA9C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_LL929:
 _tmpA9D=_tmpA9C;goto _LL92B;case 30: _LL92A: _tmpA9D=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA7F)->f1;_LL92B:
# 5499
 for(0;_tmpA9D != 0;_tmpA9D=_tmpA9D->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpA9D->hd))
return 0;}
return 1;default: _LL92C: _LL92D:
 return 0;}_LL8FF:;}
# 5507
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5511
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5513
int Cyc_Tcutil_supports_default(void*t){
void*_tmpA9E=Cyc_Tcutil_compress(t);void*_tmpA9F=_tmpA9E;struct Cyc_List_List*_tmpAA9;union Cyc_Absyn_AggrInfoU _tmpAA8;struct Cyc_List_List*_tmpAA7;struct Cyc_List_List*_tmpAA6;void*_tmpAA5;union Cyc_Absyn_Constraint*_tmpAA4;union Cyc_Absyn_Constraint*_tmpAA3;switch(*((int*)_tmpA9F)){case 0: _LL948: _LL949:
 goto _LL94B;case 6: _LL94A: _LL94B:
 goto _LL94D;case 7: _LL94C: _LL94D:
 return 1;case 5: _LL94E: _tmpAA4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA9F)->f1).ptr_atts).nullable;_tmpAA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA9F)->f1).ptr_atts).bounds;_LL94F: {
# 5520
void*_tmpAA0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAA3);void*_tmpAA1=_tmpAA0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAA1)->tag == 0){_LL95F: _LL960:
 return 1;}else{_LL961: _LL962:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpAA4);}_LL95E:;}case 8: _LL950: _tmpAA5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9F)->f1).elt_type;_LL951:
# 5525
 return Cyc_Tcutil_supports_default(_tmpAA5);case 10: _LL952: _tmpAA6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA9F)->f1;_LL953:
# 5527
 for(0;_tmpAA6 != 0;_tmpAA6=_tmpAA6->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpAA6->hd)).f2))return 0;}
return 1;case 11: _LL954: _tmpAA8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA9F)->f1).aggr_info;_tmpAA7=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA9F)->f1).targs;_LL955: {
# 5531
struct Cyc_Absyn_Aggrdecl*_tmpAA2=Cyc_Absyn_get_known_aggrdecl(_tmpAA8);
if(_tmpAA2->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAA2->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpAA2->tvs,_tmpAA7,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAA2->impl))->fields);}case 12: _LL956: _tmpAA9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA9F)->f2;_LL957:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpAA9);case 14: _LL958: _LL959:
# 5537
 goto _LL95B;case 13: _LL95A: _LL95B:
 return 1;default: _LL95C: _LL95D:
# 5540
 return 0;}_LL947:;}
# 5545
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpAAA=t;struct Cyc_Absyn_Typedefdecl*_tmpAAF;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAAA)->tag == 17){_LL964: _tmpAAF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAAA)->f3;_LL965:
# 5548
 if(_tmpAAF != 0){
struct Cyc_Absyn_Tqual _tmpAAB=_tmpAAF->tq;
if(((_tmpAAB.print_const  || _tmpAAB.q_volatile) || _tmpAAB.q_restrict) || _tmpAAB.real_const)
# 5553
({struct Cyc_String_pa_PrintArg_struct _tmpAAE;_tmpAAE.tag=0;({struct _dyneither_ptr _tmpF1B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAAE.f1=_tmpF1B;});({void*_tmpAAC[1]={& _tmpAAE};({unsigned int _tmpF1D=loc;struct _dyneither_ptr _tmpF1C=({const char*_tmpAAD="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpAAD,sizeof(char),44);});Cyc_Tcutil_warn(_tmpF1D,_tmpF1C,_tag_dyneither(_tmpAAC,sizeof(void*),1));});});});}
# 5556
goto _LL963;}else{_LL966: _LL967:
 goto _LL963;}_LL963:;}
# 5561
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5563
struct _RegionHandle _tmpAB0=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAB0;_push_region(rgn);
{struct Cyc_List_List*_tmpAB1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpAB2=1;_npop_handler(0);return _tmpAB2;}
({void*_tmpF1E=Cyc_Tcutil_rsubstitute(rgn,_tmpAB1,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);t=_tmpF1E;});
if(!Cyc_Tcutil_supports_default(t)){int _tmpAB3=0;_npop_handler(0);return _tmpAB3;}}{
# 5571
int _tmpAB4=1;_npop_handler(0);return _tmpAB4;};}
# 5564
;_pop_region(rgn);}
# 5577
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpAB5=Cyc_Tcutil_compress(t);void*_tmpAB6=_tmpAB5;union Cyc_Absyn_Constraint*_tmpABA;union Cyc_Absyn_Constraint*_tmpAB9;switch(*((int*)_tmpAB6)){case 6: _LL969: _LL96A:
 goto _LL96C;case 7: _LL96B: _LL96C:
 return 1;case 5: _LL96D: _tmpABA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB6)->f1).ptr_atts).nullable;_tmpAB9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB6)->f1).ptr_atts).bounds;_LL96E: {
# 5582
void*_tmpAB7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAB9);void*_tmpAB8=_tmpAB7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAB8)->tag == 0){_LL972: _LL973:
# 5586
 return 0;}else{_LL974: _LL975:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpABA);}_LL971:;}default: _LL96F: _LL970:
# 5589
 return 0;}_LL968:;}
# 5593
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpABB=Cyc_Tcutil_compress(t);void*_tmpABC=_tmpABB;struct Cyc_List_List*_tmpAC0;void*_tmpABF;switch(*((int*)_tmpABC)){case 5: _LL977: _tmpABF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABC)->f1).elt_typ;_LL978:
 return Cyc_Tcutil_is_noreturn(_tmpABF);case 9: _LL979: _tmpAC0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpABC)->f1).attributes;_LL97A:
# 5597
 for(0;_tmpAC0 != 0;_tmpAC0=_tmpAC0->tl){
void*_tmpABD=(void*)_tmpAC0->hd;void*_tmpABE=_tmpABD;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpABE)->tag == 4){_LL97E: _LL97F:
 return 1;}else{_LL980: _LL981:
 continue;}_LL97D:;}
# 5602
goto _LL976;default: _LL97B: _LL97C:
 goto _LL976;}_LL976:;}
# 5605
return 0;}
# 5610
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpAC1=Cyc_Tcutil_compress(t);void*_tmpAC2=_tmpAC1;struct Cyc_List_List**_tmpAC8;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAC2)->tag == 9){_LL983: _tmpAC8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAC2)->f1).attributes;_LL984: {
# 5613
struct Cyc_List_List*_tmpAC3=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpAC8,(void*)atts->hd))
({struct Cyc_List_List*_tmpF1F=({struct Cyc_List_List*_tmpAC4=_cycalloc(sizeof(*_tmpAC4));_tmpAC4->hd=(void*)atts->hd;_tmpAC4->tl=*_tmpAC8;_tmpAC4;});*_tmpAC8=_tmpF1F;});}else{
# 5620
({struct Cyc_List_List*_tmpF20=({struct Cyc_List_List*_tmpAC5=_cycalloc(sizeof(*_tmpAC5));_tmpAC5->hd=(void*)atts->hd;_tmpAC5->tl=_tmpAC3;_tmpAC5;});_tmpAC3=_tmpF20;});}}
return _tmpAC3;}}else{_LL985: _LL986:
({void*_tmpAC6=0;({struct _dyneither_ptr _tmpF21=({const char*_tmpAC7="transfer_fn_type_atts";_tag_dyneither(_tmpAC7,sizeof(char),22);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF21,_tag_dyneither(_tmpAC6,sizeof(void*),0));});});}_LL982:;}
# 5627
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpAC9=Cyc_Tcutil_compress(t);void*_tmpACA=_tmpAC9;struct Cyc_Absyn_Exp*_tmpACF;struct Cyc_Absyn_PtrInfo*_tmpACE;switch(*((int*)_tmpACA)){case 5: _LL988: _tmpACE=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpACA)->f1;_LL989:
# 5631
{void*_tmpACB=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpACE->ptr_atts).bounds);void*_tmpACC=_tmpACB;struct Cyc_Absyn_Exp*_tmpACD;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpACC)->tag == 1){_LL98F: _tmpACD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpACC)->f1;_LL990:
 bound=_tmpACD;goto _LL98E;}else{_LL991: _LL992:
 goto _LL98E;}_LL98E:;}
# 5635
goto _LL987;case 8: _LL98A: _tmpACF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpACA)->f1).num_elts;_LL98B:
# 5637
 bound=_tmpACF;
goto _LL987;default: _LL98C: _LL98D:
 goto _LL987;}_LL987:;}
# 5641
return bound;}
# 5646
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpAD0=b;struct Cyc_Absyn_Vardecl*_tmpAD4;struct Cyc_Absyn_Vardecl*_tmpAD3;struct Cyc_Absyn_Vardecl*_tmpAD2;struct Cyc_Absyn_Vardecl*_tmpAD1;switch(*((int*)_tmpAD0)){case 5: _LL994: _tmpAD1=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpAD0)->f1;_LL995:
 _tmpAD2=_tmpAD1;goto _LL997;case 4: _LL996: _tmpAD2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpAD0)->f1;_LL997:
 _tmpAD3=_tmpAD2;goto _LL999;case 3: _LL998: _tmpAD3=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpAD0)->f1;_LL999:
 _tmpAD4=_tmpAD3;goto _LL99B;case 1: _LL99A: _tmpAD4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpAD0)->f1;_LL99B:
# 5652
 if(!_tmpAD4->escapes)return _tmpAD4;
goto _LL993;default: _LL99C: _LL99D:
 goto _LL993;}_LL993:;}
# 5656
return 0;}
# 5660
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpAD5=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)({struct Cyc_List_List*_tmpF22=({struct Cyc_List_List*_tmpAD6=_cycalloc(sizeof(*_tmpAD6));_tmpAD6->hd=*((void**)_check_null((void**)x->hd));_tmpAD6->tl=_tmpAD5;_tmpAD6;});_tmpAD5=_tmpF22;});}}
return _tmpAD5;}
# 5667
int Cyc_Tcutil_is_array(void*t){
void*_tmpAD7=Cyc_Tcutil_compress(t);void*_tmpAD8=_tmpAD7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAD8)->tag == 8){_LL99F: _LL9A0:
 return 1;}else{_LL9A1: _LL9A2:
 return 0;}_LL99E:;}
# 5674
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmpAD9=Cyc_Tcutil_compress(t);void*_tmpADA=_tmpAD9;void*_tmpAEA;struct Cyc_Absyn_Tqual _tmpAE9;struct Cyc_Absyn_Exp*_tmpAE8;union Cyc_Absyn_Constraint*_tmpAE7;unsigned int _tmpAE6;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADA)->tag == 8){_LL9A4: _tmpAEA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADA)->f1).elt_type;_tmpAE9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADA)->f1).tq;_tmpAE8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADA)->f1).num_elts;_tmpAE7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADA)->f1).zero_term;_tmpAE6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADA)->f1).zt_loc;_LL9A5: {
# 5677
void*b;
if(_tmpAE8 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5681
struct Cyc_Absyn_Exp*bnd=_tmpAE8;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpADB=0;({struct _dyneither_ptr _tmpF23=({const char*_tmpADC="cannot convert tag without type!";_tag_dyneither(_tmpADC,sizeof(char),33);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF23,_tag_dyneither(_tmpADB,sizeof(void*),0));});});{
void*_tmpADD=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpADE=_tmpADD;void*_tmpAE3;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpADE)->tag == 19){_LL9A9: _tmpAE3=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpADE)->f1;_LL9AA:
# 5687
({void*_tmpF27=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpADF=_cycalloc(sizeof(*_tmpADF));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF26=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAE0;_tmpAE0.tag=1;({struct Cyc_Absyn_Exp*_tmpF25=({void*_tmpF24=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmpF24,Cyc_Absyn_valueof_exp(_tmpAE3,0),0,Cyc_Absyn_No_coercion,0);});_tmpAE0.f1=_tmpF25;});_tmpAE0;});_tmpADF[0]=_tmpF26;});_tmpADF;});b=_tmpF27;});
goto _LL9A8;}else{_LL9AB: _LL9AC:
# 5690
 if(Cyc_Tcutil_is_const_exp(bnd))
({void*_tmpF29=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAE1=_cycalloc(sizeof(*_tmpAE1));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF28=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAE2;_tmpAE2.tag=1;_tmpAE2.f1=bnd;_tmpAE2;});_tmpAE1[0]=_tmpF28;});_tmpAE1;});b=_tmpF29;});else{
# 5693
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL9A8:;};}else{
# 5697
({void*_tmpF2B=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAE4=_cycalloc(sizeof(*_tmpAE4));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF2A=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAE5;_tmpAE5.tag=1;_tmpAE5.f1=bnd;_tmpAE5;});_tmpAE4[0]=_tmpF2A;});_tmpAE4;});b=_tmpF2B;});}}
# 5699
return Cyc_Absyn_atb_typ(_tmpAEA,rgn,_tmpAE9,b,_tmpAE7);}}else{_LL9A6: _LL9A7:
# 5702
 return t;}_LL9A3:;}
# 5707
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpAEB=Cyc_Tcutil_compress(t);void*_tmpAEC=_tmpAEB;struct Cyc_List_List*_tmpAFA;union Cyc_Absyn_AggrInfoU _tmpAF9;struct Cyc_List_List*_tmpAF8;struct Cyc_List_List*_tmpAF7;void*_tmpAF6;union Cyc_Absyn_Constraint*_tmpAF5;switch(*((int*)_tmpAEC)){case 0: _LL9AE: _LL9AF:
 return 1;case 1: _LL9B0: _LL9B1:
 goto _LL9B3;case 2: _LL9B2: _LL9B3:
 goto _LL9B5;case 3: _LL9B4: _LL9B5:
 goto _LL9B7;case 4: _LL9B6: _LL9B7:
 return 0;case 5: _LL9B8: _tmpAF5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAEC)->f1).ptr_atts).nullable;_LL9B9:
# 5715
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpAF5);case 6: _LL9BA: _LL9BB:
 goto _LL9BD;case 7: _LL9BC: _LL9BD:
 return 1;case 8: _LL9BE: _tmpAF6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAEC)->f1).elt_type;_LL9BF:
 return Cyc_Tcutil_zeroable_type(_tmpAF6);case 9: _LL9C0: _LL9C1:
 return 0;case 10: _LL9C2: _tmpAF7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAEC)->f1;_LL9C3:
# 5721
 for(0;(unsigned int)_tmpAF7;_tmpAF7=_tmpAF7->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpAF7->hd)).f2))return 0;}
return 1;case 11: _LL9C4: _tmpAF9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAEC)->f1).aggr_info;_tmpAF8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAEC)->f1).targs;_LL9C5: {
# 5725
struct Cyc_Absyn_Aggrdecl*_tmpAED=Cyc_Absyn_get_known_aggrdecl(_tmpAF9);
if(_tmpAED->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAED->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpAEE=_new_region("r");struct _RegionHandle*r=& _tmpAEE;_push_region(r);
{struct Cyc_List_List*_tmpAEF=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpAED->tvs,_tmpAF8);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAED->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpAEF,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAF0=0;_npop_handler(0);return _tmpAF0;}}}{
int _tmpAF1=1;_npop_handler(0);return _tmpAF1;};}
# 5729
;_pop_region(r);};}case 13: _LL9C6: _LL9C7:
# 5733
 goto _LL9C9;case 19: _LL9C8: _LL9C9:
 return 1;case 12: _LL9CA: _tmpAFA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAEC)->f2;_LL9CB:
# 5736
 for(0;_tmpAFA != 0;_tmpAFA=_tmpAFA->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpAFA->hd)->type))return 0;}
return 1;case 14: _LL9CC: _LL9CD:
 return 1;case 17: _LL9CE: _LL9CF:
 goto _LL9D1;case 16: _LL9D0: _LL9D1:
 goto _LL9D3;case 15: _LL9D2: _LL9D3:
 goto _LL9D5;case 28: _LL9D4: _LL9D5:
 return 0;case 26: _LL9D6: _LL9D7:
# 5745
 goto _LL9D9;case 27: _LL9D8: _LL9D9:
 goto _LL9DB;case 18: _LL9DA: _LL9DB:
 goto _LL9DD;case 20: _LL9DC: _LL9DD:
 goto _LL9DF;case 21: _LL9DE: _LL9DF:
 goto _LL9E1;case 22: _LL9E0: _LL9E1:
 goto _LL9E3;case 23: _LL9E2: _LL9E3:
 goto _LL9E5;case 24: _LL9E4: _LL9E5:
 goto _LL9E7;default: _LL9E6: _LL9E7:
({struct Cyc_String_pa_PrintArg_struct _tmpAF4;_tmpAF4.tag=0;({struct _dyneither_ptr _tmpF2C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAF4.f1=_tmpF2C;});({void*_tmpAF2[1]={& _tmpAF4};({struct _dyneither_ptr _tmpF2D=({const char*_tmpAF3="bad type `%s' in zeroable type";_tag_dyneither(_tmpAF3,sizeof(char),31);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF2D,_tag_dyneither(_tmpAF2,sizeof(void*),1));});});});}_LL9AD:;}
