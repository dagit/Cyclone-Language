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
# 75
typedef void*Cyc_Tcpat_decision_opt_t;
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
enum Cyc_Absyn_Scope{Cyc_Absyn_Static  = 0,Cyc_Absyn_Abstract  = 1,Cyc_Absyn_Public  = 2,Cyc_Absyn_Extern  = 3,Cyc_Absyn_ExternC  = 4,Cyc_Absyn_Register  = 5};struct Cyc_Absyn_Tqual{int print_const;int q_volatile;int q_restrict;int real_const;unsigned int loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz  = 0,Cyc_Absyn_Short_sz  = 1,Cyc_Absyn_Int_sz  = 2,Cyc_Absyn_Long_sz  = 3,Cyc_Absyn_LongLong_sz  = 4};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable  = 0,Cyc_Absyn_Unique  = 1,Cyc_Absyn_Top  = 2};
# 191
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind  = 0,Cyc_Absyn_MemKind  = 1,Cyc_Absyn_BoxKind  = 2,Cyc_Absyn_RgnKind  = 3,Cyc_Absyn_EffKind  = 4,Cyc_Absyn_IntKind  = 5};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 211
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed  = 0,Cyc_Absyn_Unsigned  = 1,Cyc_Absyn_None  = 2};
# 213
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA  = 0,Cyc_Absyn_UnionA  = 1};struct _union_Constraint_Eq_constr{int tag;void*val;};struct _union_Constraint_Forward_constr{int tag;union Cyc_Absyn_Constraint*val;};struct _union_Constraint_No_constr{int tag;int val;};union Cyc_Absyn_Constraint{struct _union_Constraint_Eq_constr Eq_constr;struct _union_Constraint_Forward_constr Forward_constr;struct _union_Constraint_No_constr No_constr;};
# 222
typedef union Cyc_Absyn_Constraint*Cyc_Absyn_conref_t;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _dyneither_ptr*name;int identity;void*kind;};struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct{int tag;};struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 248
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple2*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};
# 316
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);
union Cyc_Absyn_DatatypeInfoU Cyc_Absyn_KnownDatatype(struct Cyc_Absyn_Datatypedecl**);struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple2*datatype_name;struct _tuple2*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple3{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple3 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};
# 335
union Cyc_Absyn_DatatypeFieldInfoU Cyc_Absyn_KnownDatatypefield(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypefield*);struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};
# 368
typedef void*Cyc_Absyn_raw_type_decl_t;struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};
# 373
typedef struct Cyc_Absyn_TypeDecl*Cyc_Absyn_type_decl_t;struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 428 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
extern struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct Cyc_Absyn_UniqueRgn_val;
extern struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct Cyc_Absyn_RefCntRgn_val;
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 444
typedef void*Cyc_Absyn_funcparams_t;
# 447
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0,Cyc_Absyn_Scanf_ft  = 1};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};
# 511
typedef void*Cyc_Absyn_type_modifier_t;struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
extern union Cyc_Absyn_Cnst Cyc_Absyn_Null_c;
# 537
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0,Cyc_Absyn_Times  = 1,Cyc_Absyn_Minus  = 2,Cyc_Absyn_Div  = 3,Cyc_Absyn_Mod  = 4,Cyc_Absyn_Eq  = 5,Cyc_Absyn_Neq  = 6,Cyc_Absyn_Gt  = 7,Cyc_Absyn_Lt  = 8,Cyc_Absyn_Gte  = 9,Cyc_Absyn_Lte  = 10,Cyc_Absyn_Not  = 11,Cyc_Absyn_Bitnot  = 12,Cyc_Absyn_Bitand  = 13,Cyc_Absyn_Bitor  = 14,Cyc_Absyn_Bitxor  = 15,Cyc_Absyn_Bitlshift  = 16,Cyc_Absyn_Bitlrshift  = 17,Cyc_Absyn_Bitarshift  = 18,Cyc_Absyn_Numelts  = 19};
# 544
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0,Cyc_Absyn_PostInc  = 1,Cyc_Absyn_PreDec  = 2,Cyc_Absyn_PostDec  = 3};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 562
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0,Cyc_Absyn_No_coercion  = 1,Cyc_Absyn_Null_to_NonNull  = 2,Cyc_Absyn_Other_coercion  = 3};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple10*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_ResetRegion_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple2*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple2*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple2*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple2*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;int f3;struct Cyc_Absyn_Exp*f4;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
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
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(struct _tuple2*x,void*t,struct Cyc_Absyn_Exp*init);
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
int Cyc_RgnOrder_is_region_resetable(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Tvar*r);
int Cyc_RgnOrder_effect_outlives(struct Cyc_RgnOrder_RgnPO*po,void*eff,void*rgn);
int Cyc_RgnOrder_satisfies_constraints(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*constraints,void*default_bound,int do_pin);
# 45
int Cyc_RgnOrder_eff_outlives_eff(struct Cyc_RgnOrder_RgnPO*po,void*eff1,void*eff2);
# 48
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
# 169
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 179
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 33
void Cyc_Tcutil_terr(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);
# 35
void Cyc_Tcutil_warn(unsigned int,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);extern char Cyc_Tcutil_AbortTypeCheckingFunction[26];struct Cyc_Tcutil_AbortTypeCheckingFunction_exn_struct{char*tag;};
# 46
extern struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set;
# 51
void*Cyc_Tcutil_copy_type(void*t);
# 54
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 57
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2);
# 61
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
# 73
int Cyc_Tcutil_coerceable(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 76
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 78
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned int,void*,void*);
# 80
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple15{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 84
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ);
# 86
extern int Cyc_Tcutil_warn_alias_coerce;
# 89
extern int Cyc_Tcutil_warn_region_coerce;
# 92
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*);
int Cyc_Tcutil_is_function_type(void*t);
int Cyc_Tcutil_is_pointer_type(void*t);
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e);
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
void*Cyc_Tcutil_pointer_region(void*t);
# 103
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
# 110
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 115
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 120
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
# 127
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 132
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 137
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
int Cyc_Tcutil_unify_kindbound(void*,void*);struct _tuple16{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 141
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb);
# 146
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*t,struct Cyc_Absyn_Exp*e);
# 148
void*Cyc_Tcutil_max_arithmetic_type(void*,void*);
# 152
void Cyc_Tcutil_explain_failure();
# 154
int Cyc_Tcutil_unify(void*,void*);
# 156
int Cyc_Tcutil_typecmp(void*,void*);
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 159
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 164
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Aggrfield*);
# 168
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_List_List*fs);
# 172
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 175
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 179
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 181
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*);
# 185
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct _tuple17{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*,struct Cyc_Absyn_Tvar*);
# 191
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn);
# 218 "tcutil.h"
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int,struct Cyc_Tcenv_Tenv*,void*);
# 220
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 228
void Cyc_Tcutil_check_type(unsigned int,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*k,int allow_evars,int allow_abs_aggr,void*);
# 231
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr err_msg);
void Cyc_Tcutil_check_unique_tvars(unsigned int,struct Cyc_List_List*);
# 238
void Cyc_Tcutil_check_nonzero_bound(unsigned int,union Cyc_Absyn_Constraint*);
# 240
void Cyc_Tcutil_check_bound(unsigned int,unsigned int i,union Cyc_Absyn_Constraint*);
# 242
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b);
# 244
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2);
# 246
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 252
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 254
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 256
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 260
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type);
# 266
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);
# 271
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 274
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 279
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*,struct Cyc_Absyn_Exp*e);
# 284
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*,void*t);struct _tuple18{int f1;void*f2;};
# 288
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e);
# 296
void*Cyc_Tcutil_normalize_effect(void*e);
# 299
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 301
int Cyc_Tcutil_new_tvar_id();
# 303
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 306
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 308
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*);
# 311
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 315
int Cyc_Tcutil_bits_only(void*t);
# 318
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 321
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e);
# 324
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 328
int Cyc_Tcutil_supports_default(void*);
# 332
int Cyc_Tcutil_admits_zero(void*t);
# 335
int Cyc_Tcutil_is_noreturn(void*);
# 339
int Cyc_Tcutil_extract_const_from_typedef(unsigned int,int declared_const,void*);
# 343
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts);
# 347
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn);
# 350
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t);
# 353
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 357
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 360
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);
# 363
int Cyc_Tcutil_is_array(void*t);
# 367
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 370
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
Cyc_Warn_vwarn(sg,fmt,ap);}char Cyc_Tcutil_AbortTypeCheckingFunction[26]="AbortTypeCheckingFunction";
# 129
static struct _dyneither_ptr Cyc_Tcutil_tvar2string(struct Cyc_Absyn_Tvar*tv){
return*tv->name;}
# 133
void Cyc_Tcutil_print_tvars(struct Cyc_List_List*tvs){
for(0;tvs != 0;tvs=tvs->tl){
({struct Cyc_String_pa_PrintArg_struct _tmp28;_tmp28.tag=0;_tmp28.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind));({struct Cyc_String_pa_PrintArg_struct _tmp27;_tmp27.tag=0;_tmp27.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)tvs->hd));({void*_tmp25[2]={& _tmp27,& _tmp28};Cyc_fprintf(Cyc_stderr,({const char*_tmp26="%s::%s ";_tag_dyneither(_tmp26,sizeof(char),8);}),_tag_dyneither(_tmp25,sizeof(void*),2));});});});}
({void*_tmp29=0;Cyc_fprintf(Cyc_stderr,({const char*_tmp2A="\n";_tag_dyneither(_tmp2A,sizeof(char),2);}),_tag_dyneither(_tmp29,sizeof(void*),0));});Cyc_fflush(Cyc_stderr);}
# 141
struct Cyc_Core_Opt*Cyc_Tcutil_empty_var_set=0;
# 145
static int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 150
void*Cyc_Tcutil_compress(void*t){
void*_tmp2B=t;void*_tmp34;struct Cyc_Absyn_Exp*_tmp33;struct Cyc_Absyn_Exp*_tmp32;void**_tmp31;void**_tmp30;switch(*((int*)_tmp2B)){case 1: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp30=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B)->f2;_LL8: {
# 161
void*ta=(void*)_check_null(*_tmp30);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp30=t2;
return t2;}}case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B)->f4 == 0){_LL3: _LL4:
# 153
 return t;}else{_LL5: _tmp31=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2B)->f4;_LL6: {
# 155
void*ta=(void*)_check_null(*_tmp31);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp31=t2;
return t2;}}case 18: _LL9: _tmp32=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B)->f1;_LLA:
# 167
 Cyc_Evexp_eval_const_uint_exp(_tmp32);{
void*_tmp2C=_tmp32->r;void*_tmp2D=_tmp2C;void*_tmp2E;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2D)->tag == 38){_LL12: _tmp2E=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp2D)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp2E);}else{_LL14: _LL15:
 return t;}_LL11:;};case 27: _LLB: _tmp33=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp2B)->f1;_LLC: {
# 173
void*_tmp2F=_tmp33->topt;
if(_tmp2F != 0)return _tmp2F;else{
return t;}}case 26: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2B)->f2 != 0){_LLD: _tmp34=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp2B)->f2;_LLE:
# 177
 return Cyc_Tcutil_compress(_tmp34);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 185
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 189
static union Cyc_Absyn_Constraint*Cyc_Tcutil_copy_conref(union Cyc_Absyn_Constraint*cptr){
union Cyc_Absyn_Constraint*_tmp35=cptr;union Cyc_Absyn_Constraint*_tmp37;void*_tmp36;switch((((union Cyc_Absyn_Constraint*)_tmp35)->Eq_constr).tag){case 3: _LL17: _LL18:
 return Cyc_Absyn_empty_conref();case 1: _LL19: _tmp36=(void*)(_tmp35->Eq_constr).val;_LL1A:
 return Cyc_Absyn_new_conref(_tmp36);default: _LL1B: _tmp37=(_tmp35->Forward_constr).val;_LL1C:
 return Cyc_Tcutil_copy_conref(_tmp37);}_LL16:;}
# 196
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp38=Cyc_Absyn_compress_kb(kb);void*_tmp39=_tmp38;struct Cyc_Absyn_Kind*_tmp3E;switch(*((int*)_tmp39)){case 1: _LL1E: _LL1F:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp3A=_cycalloc(sizeof(*_tmp3A));_tmp3A[0]=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp3B;_tmp3B.tag=1;_tmp3B.f1=0;_tmp3B;});_tmp3A;});case 2: _LL20: _tmp3E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp39)->f2;_LL21:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp3C=_cycalloc(sizeof(*_tmp3C));_tmp3C[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp3D;_tmp3D.tag=2;_tmp3D.f1=0;_tmp3D.f2=_tmp3E;_tmp3D;});_tmp3C;});default: _LL22: _LL23:
 return kb;}_LL1D:;}
# 203
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 205
return({struct Cyc_Absyn_Tvar*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->name=tv->name;_tmp3F->identity=- 1;_tmp3F->kind=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp3F;});}
# 207
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 209
struct _tuple10*_tmp40=arg;struct _dyneither_ptr*_tmp44;struct Cyc_Absyn_Tqual _tmp43;void*_tmp42;_LL25: _tmp44=_tmp40->f1;_tmp43=_tmp40->f2;_tmp42=_tmp40->f3;_LL26:;
return({struct _tuple10*_tmp41=_cycalloc(sizeof(*_tmp41));_tmp41->f1=_tmp44;_tmp41->f2=_tmp43;_tmp41->f3=Cyc_Tcutil_copy_type(_tmp42);_tmp41;});}
# 212
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp45=arg;struct Cyc_Absyn_Tqual _tmp48;void*_tmp47;_LL28: _tmp48=_tmp45->f1;_tmp47=_tmp45->f2;_LL29:;
return({struct _tuple12*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->f1=_tmp48;_tmp46->f2=Cyc_Tcutil_copy_type(_tmp47);_tmp46;});}
# 216
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 218
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp49=_cycalloc(sizeof(*_tmp49));_tmp49->name=f->name;_tmp49->tq=f->tq;_tmp49->type=Cyc_Tcutil_copy_type(f->type);_tmp49->width=f->width;_tmp49->attributes=f->attributes;_tmp49->requires_clause=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp49;});}
# 222
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp4A=x;void*_tmp4D;void*_tmp4C;_LL2B: _tmp4D=_tmp4A->f1;_tmp4C=_tmp4A->f2;_LL2C:;
return({struct _tuple0*_tmp4B=_cycalloc(sizeof(*_tmp4B));_tmp4B->f1=Cyc_Tcutil_copy_type(_tmp4D);_tmp4B->f2=Cyc_Tcutil_copy_type(_tmp4C);_tmp4B;});}
# 226
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp4E=_cycalloc(sizeof(*_tmp4E));_tmp4E->name=f->name;_tmp4E->tag=f->tag;_tmp4E->loc=f->loc;_tmp4E;});}
# 229
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp50;_tmp50.tag=2;_tmp50.f1=Cyc_Tcutil_copy_tvar(t);_tmp50;});_tmp4F;});}
# 233
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp51=Cyc_Tcutil_compress(t);void*_tmp52=_tmp51;struct Cyc_Absyn_Datatypedecl*_tmpD8;struct Cyc_Absyn_Enumdecl*_tmpD7;struct Cyc_Absyn_Aggrdecl*_tmpD6;void*_tmpD5;struct Cyc_List_List*_tmpD4;void*_tmpD3;struct _tuple2*_tmpD2;struct Cyc_List_List*_tmpD1;struct Cyc_Absyn_Typedefdecl*_tmpD0;void*_tmpCF;void*_tmpCE;void*_tmpCD;struct Cyc_Absyn_Exp*_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;void*_tmpCA;struct Cyc_List_List*_tmpC9;struct _tuple2*_tmpC8;struct Cyc_Absyn_Enumdecl*_tmpC7;enum Cyc_Absyn_AggrKind _tmpC6;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Aggrdecl**_tmpC4;struct Cyc_List_List*_tmpC3;enum Cyc_Absyn_AggrKind _tmpC2;struct _tuple2*_tmpC1;struct Cyc_Core_Opt*_tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_List_List*_tmpBE;struct Cyc_List_List*_tmpBD;void*_tmpBC;struct Cyc_Absyn_Tqual _tmpBB;void*_tmpBA;struct Cyc_List_List*_tmpB9;int _tmpB8;struct Cyc_Absyn_VarargInfo*_tmpB7;struct Cyc_List_List*_tmpB6;struct Cyc_List_List*_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_List_List*_tmpB3;struct Cyc_Absyn_Exp*_tmpB2;struct Cyc_List_List*_tmpB1;void*_tmpB0;struct Cyc_Absyn_Tqual _tmpAF;struct Cyc_Absyn_Exp*_tmpAE;union Cyc_Absyn_Constraint*_tmpAD;unsigned int _tmpAC;void*_tmpAB;struct Cyc_Absyn_Tqual _tmpAA;void*_tmpA9;union Cyc_Absyn_Constraint*_tmpA8;union Cyc_Absyn_Constraint*_tmpA7;union Cyc_Absyn_Constraint*_tmpA6;struct Cyc_Absyn_PtrLoc*_tmpA5;union Cyc_Absyn_DatatypeFieldInfoU _tmpA4;struct Cyc_List_List*_tmpA3;union Cyc_Absyn_DatatypeInfoU _tmpA2;struct Cyc_List_List*_tmpA1;struct Cyc_Absyn_Tvar*_tmpA0;switch(*((int*)_tmp52)){case 0: _LL2E: _LL2F:
 goto _LL31;case 1: _LL30: _LL31:
 return t;case 2: _LL32: _tmpA0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp52)->f1;_LL33:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp54;_tmp54.tag=2;_tmp54.f1=Cyc_Tcutil_copy_tvar(_tmpA0);_tmp54;});_tmp53;});case 3: _LL34: _tmpA2=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp52)->f1).datatype_info;_tmpA1=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp52)->f1).targs;_LL35:
# 239
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp56;_tmp56.tag=3;_tmp56.f1=({struct Cyc_Absyn_DatatypeInfo _tmp57;_tmp57.datatype_info=_tmpA2;_tmp57.targs=Cyc_Tcutil_copy_types(_tmpA1);_tmp57;});_tmp56;});_tmp55;});case 4: _LL36: _tmpA4=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp52)->f1).field_info;_tmpA3=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp52)->f1).targs;_LL37:
# 241
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp59;_tmp59.tag=4;_tmp59.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp5A;_tmp5A.field_info=_tmpA4;_tmp5A.targs=Cyc_Tcutil_copy_types(_tmpA3);_tmp5A;});_tmp59;});_tmp58;});case 5: _LL38: _tmpAB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).elt_typ;_tmpAA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).elt_tq;_tmpA9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).rgn;_tmpA8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).nullable;_tmpA7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).bounds;_tmpA6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).zero_term;_tmpA5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp52)->f1).ptr_atts).ptrloc;_LL39: {
# 243
void*_tmp5B=Cyc_Tcutil_copy_type(_tmpAB);
void*_tmp5C=Cyc_Tcutil_copy_type(_tmpA9);
union Cyc_Absyn_Constraint*_tmp5D=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA8);
struct Cyc_Absyn_Tqual _tmp5E=_tmpAA;
# 248
union Cyc_Absyn_Constraint*_tmp5F=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA7);
union Cyc_Absyn_Constraint*_tmp60=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA6);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp62;_tmp62.tag=5;_tmp62.f1=({struct Cyc_Absyn_PtrInfo _tmp63;_tmp63.elt_typ=_tmp5B;_tmp63.elt_tq=_tmp5E;_tmp63.ptr_atts=({(_tmp63.ptr_atts).rgn=_tmp5C;(_tmp63.ptr_atts).nullable=_tmp5D;(_tmp63.ptr_atts).bounds=_tmp5F;(_tmp63.ptr_atts).zero_term=_tmp60;(_tmp63.ptr_atts).ptrloc=_tmpA5;_tmp63.ptr_atts;});_tmp63;});_tmp62;});_tmp61;});}case 6: _LL3A: _LL3B:
 goto _LL3D;case 7: _LL3C: _LL3D:
 return t;case 8: _LL3E: _tmpB0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).elt_type;_tmpAF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).tq;_tmpAE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).num_elts;_tmpAD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).zero_term;_tmpAC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp52)->f1).zt_loc;_LL3F: {
# 254
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAE);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp64=_cycalloc(sizeof(*_tmp64));_tmp64[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp65;_tmp65.tag=8;_tmp65.f1=({struct Cyc_Absyn_ArrayInfo _tmp66;_tmp66.elt_type=Cyc_Tcutil_copy_type(_tmpB0);_tmp66.tq=_tmpAF;_tmp66.num_elts=eopt2;_tmp66.zero_term=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpAD);_tmp66.zt_loc=_tmpAC;_tmp66;});_tmp65;});_tmp64;});}case 9: _LL40: _tmpBD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).tvars;_tmpBC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).effect;_tmpBB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).ret_tqual;_tmpBA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).ret_typ;_tmpB9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).args;_tmpB8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).c_varargs;_tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).cyc_varargs;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).rgn_po;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).attributes;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).requires_clause;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).requires_relns;_tmpB2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).ensures_clause;_tmpB1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp52)->f1).ensures_relns;_LL41: {
# 258
struct Cyc_List_List*_tmp67=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpBD);
void*effopt2=_tmpBC == 0?0: Cyc_Tcutil_copy_type(_tmpBC);
void*_tmp68=Cyc_Tcutil_copy_type(_tmpBA);
struct Cyc_List_List*_tmp69=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpB9);
int _tmp6A=_tmpB8;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpB7 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpB7;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6B=_cycalloc(sizeof(*_tmp6B));_tmp6B->name=cv->name;_tmp6B->tq=cv->tq;_tmp6B->type=Cyc_Tcutil_copy_type(cv->type);_tmp6B->inject=cv->inject;_tmp6B;});}{
# 269
struct Cyc_List_List*_tmp6C=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpB6);
struct Cyc_List_List*_tmp6D=_tmpB5;
struct Cyc_Absyn_Exp*_tmp6E=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpB4);
struct Cyc_List_List*_tmp6F=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpB3);
struct Cyc_Absyn_Exp*_tmp70=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpB2);
struct Cyc_List_List*_tmp71=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpB1);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp72=_cycalloc(sizeof(*_tmp72));_tmp72[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp73;_tmp73.tag=9;_tmp73.f1=({struct Cyc_Absyn_FnInfo _tmp74;_tmp74.tvars=_tmp67;_tmp74.effect=effopt2;_tmp74.ret_tqual=_tmpBB;_tmp74.ret_typ=_tmp68;_tmp74.args=_tmp69;_tmp74.c_varargs=_tmp6A;_tmp74.cyc_varargs=cyc_varargs2;_tmp74.rgn_po=_tmp6C;_tmp74.attributes=_tmp6D;_tmp74.requires_clause=_tmp6E;_tmp74.requires_relns=_tmp6F;_tmp74.ensures_clause=_tmp70;_tmp74.ensures_relns=_tmp71;_tmp74;});_tmp73;});_tmp72;});};}case 10: _LL42: _tmpBE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52)->f1;_LL43:
# 278
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp76;_tmp76.tag=10;_tmp76.f1=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpBE);_tmp76;});_tmp75;});case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52)->f1).aggr_info).UnknownAggr).tag == 1){_LL44: _tmpC2=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52)->f1).aggr_info).UnknownAggr).val).f1;_tmpC1=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52)->f1).aggr_info).UnknownAggr).val).f2;_tmpC0=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52)->f1).aggr_info).UnknownAggr).val).f3;_tmpBF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52)->f1).targs;_LL45:
# 280
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp78;_tmp78.tag=11;_tmp78.f1=({struct Cyc_Absyn_AggrInfo _tmp79;_tmp79.aggr_info=Cyc_Absyn_UnknownAggr(_tmpC2,_tmpC1,_tmpC0);_tmp79.targs=Cyc_Tcutil_copy_types(_tmpBF);_tmp79;});_tmp78;});_tmp77;});}else{_LL46: _tmpC4=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52)->f1).aggr_info).KnownAggr).val;_tmpC3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp52)->f1).targs;_LL47:
# 282
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp7B;_tmp7B.tag=11;_tmp7B.f1=({struct Cyc_Absyn_AggrInfo _tmp7C;_tmp7C.aggr_info=Cyc_Absyn_KnownAggr(_tmpC4);_tmp7C.targs=Cyc_Tcutil_copy_types(_tmpC3);_tmp7C;});_tmp7B;});_tmp7A;});}case 12: _LL48: _tmpC6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52)->f1;_tmpC5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52)->f2;_LL49:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp7E;_tmp7E.tag=12;_tmp7E.f1=_tmpC6;_tmp7E.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpC5);_tmp7E;});_tmp7D;});case 13: _LL4A: _tmpC8=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp52)->f1;_tmpC7=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp52)->f2;_LL4B:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp80;_tmp80.tag=13;_tmp80.f1=_tmpC8;_tmp80.f2=_tmpC7;_tmp80;});_tmp7F;});case 14: _LL4C: _tmpC9=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp52)->f1;_LL4D:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81[0]=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp82;_tmp82.tag=14;_tmp82.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpC9);_tmp82;});_tmp81;});case 19: _LL4E: _tmpCA=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp52)->f1;_LL4F:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp84;_tmp84.tag=19;_tmp84.f1=Cyc_Tcutil_copy_type(_tmpCA);_tmp84;});_tmp83;});case 18: _LL50: _tmpCB=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp52)->f1;_LL51:
# 289
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp86;_tmp86.tag=18;_tmp86.f1=_tmpCB;_tmp86;});_tmp85;});case 27: _LL52: _tmpCC=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp52)->f1;_LL53:
# 292
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp88;_tmp88.tag=27;_tmp88.f1=_tmpCC;_tmp88;});_tmp87;});case 15: _LL54: _tmpCD=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp52)->f1;_LL55:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp8A;_tmp8A.tag=15;_tmp8A.f1=Cyc_Tcutil_copy_type(_tmpCD);_tmp8A;});_tmp89;});case 16: _LL56: _tmpCF=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp52)->f1;_tmpCE=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp52)->f2;_LL57:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp8C;_tmp8C.tag=16;_tmp8C.f1=Cyc_Tcutil_copy_type(_tmpCF);_tmp8C.f2=Cyc_Tcutil_copy_type(_tmpCE);_tmp8C;});_tmp8B;});case 17: _LL58: _tmpD2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f1;_tmpD1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f2;_tmpD0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f3;_LL59:
# 296
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));_tmp8D[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp8E;_tmp8E.tag=17;_tmp8E.f1=_tmpD2;_tmp8E.f2=Cyc_Tcutil_copy_types(_tmpD1);_tmp8E.f3=_tmpD0;_tmp8E.f4=0;_tmp8E;});_tmp8D;});case 21: _LL5A: _LL5B:
 goto _LL5D;case 22: _LL5C: _LL5D:
 goto _LL5F;case 28: _LL5E: _LL5F:
 goto _LL61;case 20: _LL60: _LL61:
 return t;case 23: _LL62: _tmpD3=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp52)->f1;_LL63:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp90;_tmp90.tag=23;_tmp90.f1=Cyc_Tcutil_copy_type(_tmpD3);_tmp90;});_tmp8F;});case 24: _LL64: _tmpD4=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp52)->f1;_LL65:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp92;_tmp92.tag=24;_tmp92.f1=Cyc_Tcutil_copy_types(_tmpD4);_tmp92;});_tmp91;});case 25: _LL66: _tmpD5=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp52)->f1;_LL67:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp93=_cycalloc(sizeof(*_tmp93));_tmp93[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp94;_tmp94.tag=25;_tmp94.f1=Cyc_Tcutil_copy_type(_tmpD5);_tmp94;});_tmp93;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f1)->r)){case 0: _LL68: _tmpD6=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f1)->r)->f1;_LL69: {
# 306
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD6->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp97;_tmp97.tag=11;_tmp97.f1=({struct Cyc_Absyn_AggrInfo _tmp98;_tmp98.aggr_info=Cyc_Absyn_UnknownAggr(_tmpD6->kind,_tmpD6->name,0);_tmp98.targs=_tmp95;_tmp98;});_tmp97;});_tmp96;});}case 1: _LL6A: _tmpD7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f1)->r)->f1;_LL6B:
# 309
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99[0]=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp9A;_tmp9A.tag=13;_tmp9A.f1=_tmpD7->name;_tmp9A.f2=0;_tmp9A;});_tmp99;});default: _LL6C: _tmpD8=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f1)->r)->f1;_LL6D: {
# 311
struct Cyc_List_List*_tmp9B=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD8->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp9D;_tmp9D.tag=3;_tmp9D.f1=({struct Cyc_Absyn_DatatypeInfo _tmp9E;_tmp9E.datatype_info=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp9F;_tmp9F.name=_tmpD8->name;_tmp9F.is_extensible=0;_tmp9F;}));_tmp9E.targs=_tmp9B;_tmp9E;});_tmp9D;});_tmp9C;});}}}_LL2D:;}
# 326 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD9=d;struct _dyneither_ptr*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD9)->tag == 0){_LL6F: _tmpDC=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD9)->f1;_LL70:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpDA=_cycalloc(sizeof(*_tmpDA));_tmpDA[0]=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpDB;_tmpDB.tag=0;_tmpDB.f1=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpDC);_tmpDB;});_tmpDA;});}else{_LL71: _tmpDD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD9)->f1;_LL72:
 return d;}_LL6E:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 332
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 334
return({struct _tuple19*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->f1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpDE->f2=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpDE;});}
# 337
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpDF=preserve_types;
{void*_tmpE0=e->r;void*_tmpE1=_tmpE0;int _tmp16D;struct _dyneither_ptr _tmp16C;void*_tmp16B;struct Cyc_Absyn_Exp*_tmp16A;struct _dyneither_ptr*_tmp169;struct Cyc_Core_Opt*_tmp168;struct Cyc_List_List*_tmp167;struct Cyc_Absyn_Exp*_tmp166;struct Cyc_Absyn_Exp*_tmp165;int _tmp164;struct Cyc_Absyn_Exp*_tmp163;void**_tmp162;struct Cyc_Absyn_Exp*_tmp161;int _tmp160;int _tmp15F;void*_tmp15E;struct Cyc_Absyn_Enumfield*_tmp15D;struct Cyc_Absyn_Enumdecl*_tmp15C;struct Cyc_Absyn_Enumfield*_tmp15B;struct Cyc_List_List*_tmp15A;struct Cyc_Absyn_Datatypedecl*_tmp159;struct Cyc_Absyn_Datatypefield*_tmp158;void*_tmp157;struct Cyc_List_List*_tmp156;struct _tuple2*_tmp155;struct Cyc_List_List*_tmp154;struct Cyc_List_List*_tmp153;struct Cyc_Absyn_Aggrdecl*_tmp152;struct Cyc_Absyn_Exp*_tmp151;void*_tmp150;int _tmp14F;struct Cyc_Absyn_Vardecl*_tmp14E;struct Cyc_Absyn_Exp*_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;int _tmp14B;struct Cyc_List_List*_tmp14A;struct _dyneither_ptr*_tmp149;struct Cyc_Absyn_Tqual _tmp148;void*_tmp147;struct Cyc_List_List*_tmp146;struct Cyc_List_List*_tmp145;struct Cyc_Absyn_Exp*_tmp144;struct Cyc_Absyn_Exp*_tmp143;struct Cyc_Absyn_Exp*_tmp142;struct _dyneither_ptr*_tmp141;int _tmp140;int _tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct _dyneither_ptr*_tmp13D;int _tmp13C;int _tmp13B;struct Cyc_Absyn_Exp*_tmp13A;void*_tmp139;struct Cyc_List_List*_tmp138;struct Cyc_Absyn_Exp*_tmp137;void*_tmp136;struct Cyc_Absyn_Exp*_tmp135;struct Cyc_Absyn_Exp*_tmp134;struct Cyc_Absyn_Exp*_tmp133;void*_tmp132;struct Cyc_Absyn_Exp*_tmp131;int _tmp130;enum Cyc_Absyn_Coercion _tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_List_List*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;struct Cyc_Absyn_Exp*_tmp12B;int _tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_VarargCallInfo*_tmp127;int _tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_Absyn_Exp*_tmp123;struct Cyc_Absyn_Exp*_tmp122;struct Cyc_Absyn_Exp*_tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;enum Cyc_Absyn_Incrementor _tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Core_Opt*_tmp119;struct Cyc_Absyn_Exp*_tmp118;enum Cyc_Absyn_Primop _tmp117;struct Cyc_List_List*_tmp116;void*_tmp115;union Cyc_Absyn_Cnst _tmp114;switch(*((int*)_tmpE1)){case 0: _LL74: _tmp114=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL75:
 new_e=Cyc_Absyn_const_exp(_tmp114,e->loc);goto _LL73;case 1: _LL76: _tmp115=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL77:
 new_e=Cyc_Absyn_varb_exp(_tmp115,e->loc);goto _LL73;case 2: _LL78: _tmp117=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp116=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL79:
 new_e=Cyc_Absyn_primop_exp(_tmp117,((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp116),e->loc);goto _LL73;case 3: _LL7A: _tmp11A=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp119=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp118=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_LL7B:
# 345
 new_e=Cyc_Absyn_assignop_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11A),(unsigned int)_tmp119?({struct Cyc_Core_Opt*_tmpE2=_cycalloc_atomic(sizeof(*_tmpE2));_tmpE2->v=(void*)_tmp119->v;_tmpE2;}): 0,Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp118),e->loc);
goto _LL73;case 4: _LL7C: _tmp11C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp11B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL7D:
 new_e=Cyc_Absyn_increment_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11C),_tmp11B,e->loc);goto _LL73;case 5: _LL7E: _tmp11F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp11E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp11D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_LL7F:
# 349
 new_e=Cyc_Absyn_conditional_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11F),Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11E),Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp11D),e->loc);goto _LL73;case 6: _LL80: _tmp121=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp120=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL81:
 new_e=Cyc_Absyn_and_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp121),Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp120),e->loc);goto _LL73;case 7: _LL82: _tmp123=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp122=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL83:
 new_e=Cyc_Absyn_or_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp123),Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp122),e->loc);goto _LL73;case 8: _LL84: _tmp125=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp124=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL85:
 new_e=Cyc_Absyn_seq_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp125),Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp124),e->loc);goto _LL73;case 9: _LL86: _tmp129=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp128=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp127=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp126=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL87:
# 354
{struct Cyc_Absyn_VarargCallInfo*_tmpE3=_tmp127;int _tmpF1;struct Cyc_List_List*_tmpF0;struct Cyc_Absyn_VarargInfo*_tmpEF;if(_tmpE3 != 0){_LLC5: _tmpF1=_tmpE3->num_varargs;_tmpF0=_tmpE3->injectors;_tmpEF=_tmpE3->vai;_LLC6: {
# 356
struct Cyc_Absyn_VarargInfo*_tmpE4=_tmpEF;struct _dyneither_ptr*_tmpEC;struct Cyc_Absyn_Tqual _tmpEB;void*_tmpEA;int _tmpE9;_LLCA: _tmpEC=_tmpE4->name;_tmpEB=_tmpE4->tq;_tmpEA=_tmpE4->type;_tmpE9=_tmpE4->inject;_LLCB:;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE6;_tmpE6.tag=9;_tmpE6.f1=
Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp129);_tmpE6.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp128);_tmpE6.f3=({struct Cyc_Absyn_VarargCallInfo*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->num_varargs=_tmpF1;_tmpE7->injectors=_tmpF0;_tmpE7->vai=({struct Cyc_Absyn_VarargInfo*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->name=_tmpEC;_tmpE8->tq=_tmpEB;_tmpE8->type=
# 360
Cyc_Tcutil_copy_type(_tmpEA);_tmpE8->inject=_tmpE9;_tmpE8;});_tmpE7;});_tmpE6.f4=_tmp126;_tmpE6;});_tmpE5;}),e->loc);
# 362
goto _LLC4;}}else{_LLC7: _LLC8:
# 364
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpED=_cycalloc(sizeof(*_tmpED));_tmpED[0]=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpEE;_tmpEE.tag=9;_tmpEE.f1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp129);_tmpEE.f2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp128);_tmpEE.f3=_tmp127;_tmpEE.f4=_tmp126;_tmpEE;});_tmpED;}),e->loc);
goto _LLC4;}_LLC4:;}
# 367
goto _LL73;case 10: _LL88: _tmp12B=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp12A=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL89:
# 369
 new_e=_tmp12A?Cyc_Absyn_rethrow_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp12B),e->loc): Cyc_Absyn_throw_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp12B),e->loc);
goto _LL73;case 11: _LL8A: _tmp12C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL8B:
 new_e=Cyc_Absyn_noinstantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp12C),e->loc);
goto _LL73;case 12: _LL8C: _tmp12E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp12D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL8D:
# 374
 new_e=Cyc_Absyn_instantiate_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp12E),((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp12D),e->loc);
goto _LL73;case 13: _LL8E: _tmp132=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp131=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp130=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp12F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL8F:
# 377
 new_e=Cyc_Absyn_cast_exp(Cyc_Tcutil_copy_type(_tmp132),Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp131),_tmp130,_tmp12F,e->loc);goto _LL73;case 14: _LL90: _tmp133=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL91:
 new_e=Cyc_Absyn_address_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp133),e->loc);goto _LL73;case 15: _LL92: _tmp135=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp134=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL93: {
# 380
struct Cyc_Absyn_Exp*eo1=_tmp135;if(_tmp135 != 0)eo1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp135);
new_e=Cyc_Absyn_New_exp(eo1,Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp134),e->loc);
goto _LL73;}case 16: _LL94: _tmp136=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL95:
 new_e=Cyc_Absyn_sizeoftyp_exp(Cyc_Tcutil_copy_type(_tmp136),e->loc);
goto _LL73;case 17: _LL96: _tmp137=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL97:
 new_e=Cyc_Absyn_sizeofexp_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp137),e->loc);goto _LL73;case 18: _LL98: _tmp139=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp138=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LL99:
# 387
 new_e=Cyc_Absyn_offsetof_exp(Cyc_Tcutil_copy_type(_tmp139),_tmp138,e->loc);goto _LL73;case 19: _LL9A: _tmp13A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LL9B:
 new_e=Cyc_Absyn_deref_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp13A),e->loc);goto _LL73;case 20: _LL9C: _tmp13E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp13D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp13C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp13B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL9D:
# 390
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2[0]=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpF3;_tmpF3.tag=20;_tmpF3.f1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp13E);_tmpF3.f2=_tmp13D;_tmpF3.f3=_tmp13C;_tmpF3.f4=_tmp13B;_tmpF3;});_tmpF2;}),e->loc);goto _LL73;case 21: _LL9E: _tmp142=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp141=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp140=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp13F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LL9F:
# 392
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpF4=_cycalloc(sizeof(*_tmpF4));_tmpF4[0]=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpF5;_tmpF5.tag=21;_tmpF5.f1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp142);_tmpF5.f2=_tmp141;_tmpF5.f3=_tmp140;_tmpF5.f4=_tmp13F;_tmpF5;});_tmpF4;}),e->loc);goto _LL73;case 22: _LLA0: _tmp144=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp143=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLA1:
 new_e=Cyc_Absyn_subscript_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp144),Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp143),e->loc);
goto _LL73;case 23: _LLA2: _tmp145=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LLA3:
 new_e=Cyc_Absyn_tuple_exp(((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp145),e->loc);goto _LL73;case 24: _LLA4: _tmp149=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE1)->f1)->f1;_tmp148=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE1)->f1)->f2;_tmp147=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE1)->f1)->f3;_tmp146=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLA5: {
# 397
struct _dyneither_ptr*vopt1=_tmp149;
if(_tmp149 != 0)vopt1=_tmp149;
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF6=_cycalloc(sizeof(*_tmpF6));_tmpF6[0]=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF7;_tmpF7.tag=24;_tmpF7.f1=({struct _tuple10*_tmpF8=_cycalloc(sizeof(*_tmpF8));_tmpF8->f1=vopt1;_tmpF8->f2=_tmp148;_tmpF8->f3=Cyc_Tcutil_copy_type(_tmp147);_tmpF8;});_tmpF7.f2=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp146);_tmpF7;});_tmpF6;}),e->loc);
goto _LL73;}case 25: _LLA6: _tmp14A=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LLA7:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));_tmpF9[0]=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpFA;_tmpFA.tag=25;_tmpFA.f1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp14A);_tmpFA;});_tmpF9;}),e->loc);
goto _LL73;case 26: _LLA8: _tmp14E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp14D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp14C=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp14B=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LLA9:
# 405
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));_tmpFB[0]=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpFC;_tmpFC.tag=26;_tmpFC.f1=_tmp14E;_tmpFC.f2=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp14D);_tmpFC.f3=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp14C);_tmpFC.f4=_tmp14B;_tmpFC;});_tmpFB;}),e->loc);
goto _LL73;case 27: _LLAA: _tmp151=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp150=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp14F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_LLAB:
# 408
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD[0]=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpFE;_tmpFE.tag=27;_tmpFE.f1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp151);_tmpFE.f2=Cyc_Tcutil_copy_type(_tmp150);_tmpFE.f3=_tmp14F;_tmpFE;});_tmpFD;}),_tmp151->loc);
# 410
goto _LL73;case 28: _LLAC: _tmp155=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp154=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp153=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_tmp152=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpE1)->f4;_LLAD:
# 412
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF[0]=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmp100;_tmp100.tag=28;_tmp100.f1=_tmp155;_tmp100.f2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp154);_tmp100.f3=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp153);_tmp100.f4=_tmp152;_tmp100;});_tmpFF;}),e->loc);
# 414
goto _LL73;case 29: _LLAE: _tmp157=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp156=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLAF:
# 416
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp101=_cycalloc(sizeof(*_tmp101));_tmp101[0]=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmp102;_tmp102.tag=29;_tmp102.f1=Cyc_Tcutil_copy_type(_tmp157);_tmp102.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp156);_tmp102;});_tmp101;}),e->loc);
goto _LL73;case 30: _LLB0: _tmp15A=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp159=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_tmp158=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpE1)->f3;_LLB1:
# 419
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp103=_cycalloc(sizeof(*_tmp103));_tmp103[0]=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmp104;_tmp104.tag=30;_tmp104.f1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDF,_tmp15A);_tmp104.f2=_tmp159;_tmp104.f3=_tmp158;_tmp104;});_tmp103;}),e->loc);
goto _LL73;case 31: _LLB2: _tmp15C=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp15B=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLB3:
 new_e=e;goto _LL73;case 32: _LLB4: _tmp15E=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp15D=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLB5:
# 423
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105[0]=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp106;_tmp106.tag=32;_tmp106.f1=Cyc_Tcutil_copy_type(_tmp15E);_tmp106.f2=_tmp15D;_tmp106;});_tmp105;}),e->loc);
goto _LL73;case 33: _LLB6: _tmp164=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).is_calloc;_tmp163=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).rgn;_tmp162=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).elt_type;_tmp161=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).num_elts;_tmp160=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).fat_result;_tmp15F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpE1)->f1).inline_call;_LLB7: {
# 426
struct Cyc_Absyn_Exp*_tmp107=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp163;if(_tmp163 != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp163);{
void**t1=_tmp162;if(_tmp162 != 0)t1=({void**_tmp108=_cycalloc(sizeof(*_tmp108));_tmp108[0]=Cyc_Tcutil_copy_type(*_tmp162);_tmp108;});
_tmp107->r=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp109=_cycalloc(sizeof(*_tmp109));_tmp109[0]=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp10A;_tmp10A.tag=33;_tmp10A.f1=({struct Cyc_Absyn_MallocInfo _tmp10B;_tmp10B.is_calloc=_tmp164;_tmp10B.rgn=r1;_tmp10B.elt_type=t1;_tmp10B.num_elts=_tmp161;_tmp10B.fat_result=_tmp160;_tmp10B.inline_call=_tmp15F;_tmp10B;});_tmp10A;});_tmp109;});
new_e=_tmp107;
goto _LL73;};}case 34: _LLB8: _tmp166=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp165=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLB9:
 new_e=Cyc_Absyn_swap_exp(Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp166),Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp165),e->loc);goto _LL73;case 35: _LLBA: _tmp168=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp167=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLBB: {
# 434
struct Cyc_Core_Opt*nopt1=_tmp168;
if(_tmp168 != 0)nopt1=({struct Cyc_Core_Opt*_tmp10C=_cycalloc(sizeof(*_tmp10C));_tmp10C->v=(struct _tuple2*)_tmp168->v;_tmp10C;});
new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp10D=_cycalloc(sizeof(*_tmp10D));_tmp10D[0]=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp10E;_tmp10E.tag=35;_tmp10E.f1=nopt1;_tmp10E.f2=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDF,_tmp167);_tmp10E;});_tmp10D;}),e->loc);
goto _LL73;}case 36: _LLBC: _LLBD:
# 439
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp10F=_cycalloc(sizeof(*_tmp10F));_tmp10F[0]=({struct Cyc_Core_Failure_exn_struct _tmp110;_tmp110.tag=Cyc_Core_Failure;_tmp110.f1=({const char*_tmp111="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp111,sizeof(char),45);});_tmp110;});_tmp10F;}));case 37: _LLBE: _tmp16A=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp169=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLBF:
 new_e=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112[0]=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp113;_tmp113.tag=37;_tmp113.f1=Cyc_Tcutil_deep_copy_exp(_tmpDF,_tmp16A);_tmp113.f2=_tmp169;_tmp113;});_tmp112;}),e->loc);
goto _LL73;case 38: _LLC0: _tmp16B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_LLC1:
 new_e=Cyc_Absyn_valueof_exp(Cyc_Tcutil_copy_type(_tmp16B),e->loc);
goto _LL73;default: _LLC2: _tmp16D=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE1)->f1;_tmp16C=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpE1)->f2;_LLC3:
 new_e=Cyc_Absyn_asm_exp(_tmp16D,_tmp16C,e->loc);goto _LL73;}_LL73:;}
# 447
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 451
return new_e;}
# 454
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}struct _tuple20{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple21{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 467 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp16E=ka1;enum Cyc_Absyn_KindQual _tmp179;enum Cyc_Absyn_AliasQual _tmp178;_LLCD: _tmp179=_tmp16E->kind;_tmp178=_tmp16E->aliasqual;_LLCE:;{
struct Cyc_Absyn_Kind*_tmp16F=ka2;enum Cyc_Absyn_KindQual _tmp177;enum Cyc_Absyn_AliasQual _tmp176;_LLD0: _tmp177=_tmp16F->kind;_tmp176=_tmp16F->aliasqual;_LLD1:;
# 471
if(_tmp179 != _tmp177){
struct _tuple20 _tmp170=({struct _tuple20 _tmp172;_tmp172.f1=_tmp179;_tmp172.f2=_tmp177;_tmp172;});struct _tuple20 _tmp171=_tmp170;switch(_tmp171.f1){case Cyc_Absyn_BoxKind: switch(_tmp171.f2){case Cyc_Absyn_MemKind: _LLD3: _LLD4:
 goto _LLD6;case Cyc_Absyn_AnyKind: _LLD5: _LLD6:
 goto _LLD8;default: goto _LLD9;}case Cyc_Absyn_MemKind: if(_tmp171.f2 == Cyc_Absyn_AnyKind){_LLD7: _LLD8:
 goto _LLD2;}else{goto _LLD9;}default: _LLD9: _LLDA:
 return 0;}_LLD2:;}
# 480
if(_tmp178 != _tmp176){
struct _tuple21 _tmp173=({struct _tuple21 _tmp175;_tmp175.f1=_tmp178;_tmp175.f2=_tmp176;_tmp175;});struct _tuple21 _tmp174=_tmp173;switch(_tmp174.f1){case Cyc_Absyn_Aliasable: if(_tmp174.f2 == Cyc_Absyn_Top){_LLDC: _LLDD:
 goto _LLDF;}else{goto _LLE0;}case Cyc_Absyn_Unique: if(_tmp174.f2 == Cyc_Absyn_Top){_LLDE: _LLDF:
 return 1;}else{goto _LLE0;}default: _LLE0: _LLE1:
 return 0;}_LLDB:;}
# 487
return 1;};}
# 490
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp17A=Cyc_Absyn_compress_kb(tv->kind);void*_tmp17B=_tmp17A;struct Cyc_Absyn_Kind*_tmp17F;struct Cyc_Absyn_Kind*_tmp17E;switch(*((int*)_tmp17B)){case 0: _LLE3: _tmp17E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17B)->f1;_LLE4:
 return _tmp17E;case 2: _LLE5: _tmp17F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17B)->f2;_LLE6:
 return _tmp17F;default: _LLE7: _LLE8:
# 495
 tv->kind=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp17D;_tmp17D.tag=2;_tmp17D.f1=0;_tmp17D.f2=def;_tmp17D;});_tmp17C;});
return def;}_LLE2:;}
# 500
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp180=({struct _tuple0 _tmp197;_tmp197.f1=Cyc_Absyn_compress_kb(kb1);_tmp197.f2=Cyc_Absyn_compress_kb(kb2);_tmp197;});struct _tuple0 _tmp181=_tmp180;struct Cyc_Core_Opt**_tmp196;void*_tmp195;void*_tmp194;struct Cyc_Core_Opt**_tmp193;struct Cyc_Core_Opt**_tmp192;struct Cyc_Absyn_Kind*_tmp191;struct Cyc_Core_Opt**_tmp190;struct Cyc_Absyn_Kind*_tmp18F;struct Cyc_Core_Opt**_tmp18E;struct Cyc_Absyn_Kind*_tmp18D;struct Cyc_Absyn_Kind*_tmp18C;struct Cyc_Absyn_Kind*_tmp18B;struct Cyc_Core_Opt**_tmp18A;struct Cyc_Absyn_Kind*_tmp189;struct Cyc_Absyn_Kind*_tmp188;struct Cyc_Absyn_Kind*_tmp187;switch(*((int*)_tmp181.f1)){case 0: switch(*((int*)_tmp181.f2)){case 0: _LLEA: _tmp188=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp181.f1)->f1;_tmp187=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp181.f2)->f1;_LLEB:
 return _tmp188 == _tmp187;case 2: _LLEE: _tmp18B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp181.f1)->f1;_tmp18A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp181.f2)->f1;_tmp189=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp181.f2)->f2;_LLEF:
# 509
 if(Cyc_Tcutil_kind_leq(_tmp18B,_tmp189)){
*_tmp18A=({struct Cyc_Core_Opt*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->v=kb1;_tmp183;});
return 1;}else{
return 0;}default: goto _LLF4;}case 2: switch(*((int*)_tmp181.f2)){case 0: _LLEC: _tmp18E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp181.f1)->f1;_tmp18D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp181.f1)->f2;_tmp18C=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp181.f2)->f1;_LLED:
# 504
 if(Cyc_Tcutil_kind_leq(_tmp18C,_tmp18D)){
*_tmp18E=({struct Cyc_Core_Opt*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->v=kb2;_tmp182;});
return 1;}else{
return 0;}case 2: _LLF0: _tmp192=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp181.f1)->f1;_tmp191=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp181.f1)->f2;_tmp190=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp181.f2)->f1;_tmp18F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp181.f2)->f2;_LLF1:
# 514
 if(Cyc_Tcutil_kind_leq(_tmp191,_tmp18F)){
*_tmp190=({struct Cyc_Core_Opt*_tmp184=_cycalloc(sizeof(*_tmp184));_tmp184->v=kb1;_tmp184;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp18F,_tmp191)){
*_tmp192=({struct Cyc_Core_Opt*_tmp185=_cycalloc(sizeof(*_tmp185));_tmp185->v=kb2;_tmp185;});
return 1;}else{
return 0;}}default: _LLF4: _tmp194=_tmp181.f1;_tmp193=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp181.f2)->f1;_LLF5:
# 523
*_tmp193=({struct Cyc_Core_Opt*_tmp186=_cycalloc(sizeof(*_tmp186));_tmp186->v=_tmp194;_tmp186;});
return 1;}default: _LLF2: _tmp196=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp181.f1)->f1;_tmp195=_tmp181.f2;_LLF3:
# 521
 _tmp194=_tmp195;_tmp193=_tmp196;goto _LLF5;}_LLE9:;}
# 528
struct _tuple16 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp198=Cyc_Tcutil_compress(t);void*_tmp199=_tmp198;struct Cyc_Absyn_Tvar*_tmp19F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp199)->tag == 2){_LLF7: _tmp19F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp199)->f1;_LLF8: {
# 531
void*_tmp19A=_tmp19F->kind;
_tmp19F->kind=kb;
return({struct _tuple16 _tmp19B;_tmp19B.f1=_tmp19F;_tmp19B.f2=_tmp19A;_tmp19B;});}}else{_LLF9: _LLFA:
# 535
({struct Cyc_String_pa_PrintArg_struct _tmp19E;_tmp19E.tag=0;_tmp19E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp19C[1]={& _tmp19E};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp19D="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp19D,sizeof(char),40);}),_tag_dyneither(_tmp19C,sizeof(void*),1));});});}_LLF6:;}
# 541
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_typ,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 543
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_typ_kind(field_typ);
if(ts != 0  && (k == & Cyc_Tcutil_ak  || k == & Cyc_Tcutil_tak)){
# 547
struct _RegionHandle _tmp1A0=_new_region("temp");struct _RegionHandle*temp=& _tmp1A0;_push_region(temp);
{struct Cyc_List_List*_tmp1A1=0;
# 550
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp1A2=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp1A3=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp1A4=Cyc_Tcutil_tvar_kind(_tmp1A2,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1A5=_tmp1A4;switch(((struct Cyc_Absyn_Kind*)_tmp1A5)->kind){case Cyc_Absyn_IntKind: _LLFC: _LLFD:
 goto _LLFF;case Cyc_Absyn_AnyKind: _LLFE: _LLFF:
# 556
 _tmp1A1=({struct Cyc_List_List*_tmp1A6=_region_malloc(temp,sizeof(*_tmp1A6));_tmp1A6->hd=({struct _tuple16*_tmp1A7=_region_malloc(temp,sizeof(*_tmp1A7));_tmp1A7->f1=_tmp1A2;_tmp1A7->f2=_tmp1A3;_tmp1A7;});_tmp1A6->tl=_tmp1A1;_tmp1A6;});goto _LLFB;default: _LL100: _LL101:
 goto _LLFB;}_LLFB:;}
# 560
if(_tmp1A1 != 0){
field_typ=Cyc_Tcutil_rsubstitute(temp,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp1A1),field_typ);
k=Cyc_Tcutil_typ_kind(field_typ);}}
# 548
;_pop_region(temp);}
# 565
return k;}
# 572
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t){
void*_tmp1A8=Cyc_Tcutil_compress(t);void*_tmp1A9=_tmp1A8;struct Cyc_Absyn_Typedefdecl*_tmp1C7;struct Cyc_Absyn_Exp*_tmp1C6;struct Cyc_Absyn_Kind*_tmp1C5;struct Cyc_Absyn_PtrInfo _tmp1C4;enum Cyc_Absyn_AggrKind _tmp1C3;struct Cyc_List_List*_tmp1C2;struct Cyc_Absyn_AggrdeclImpl*_tmp1C1;int _tmp1C0;struct Cyc_List_List*_tmp1BF;struct Cyc_Absyn_Datatypedecl*_tmp1BE;struct Cyc_Absyn_Datatypefield*_tmp1BD;enum Cyc_Absyn_Size_of _tmp1BC;struct Cyc_Absyn_Tvar*_tmp1BB;struct Cyc_Core_Opt*_tmp1BA;switch(*((int*)_tmp1A9)){case 1: _LL103: _tmp1BA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A9)->f1;_LL104:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1BA))->v;case 2: _LL105: _tmp1BB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A9)->f1;_LL106:
 return Cyc_Tcutil_tvar_kind(_tmp1BB,& Cyc_Tcutil_bk);case 0: _LL107: _LL108:
 return& Cyc_Tcutil_mk;case 6: _LL109: _tmp1BC=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp1A9)->f2;_LL10A:
# 578
 return(_tmp1BC == Cyc_Absyn_Int_sz  || _tmp1BC == Cyc_Absyn_Long_sz)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 7: _LL10B: _LL10C:
# 580
 return& Cyc_Tcutil_mk;case 9: _LL10D: _LL10E:
 return& Cyc_Tcutil_ak;case 16: _LL10F: _LL110:
 goto _LL112;case 13: _LL111: _LL112:
 goto _LL114;case 14: _LL113: _LL114:
 goto _LL116;case 15: _LL115: _LL116:
 return& Cyc_Tcutil_bk;case 21: _LL117: _LL118:
 return& Cyc_Tcutil_urk;case 20: _LL119: _LL11A:
 return& Cyc_Tcutil_rk;case 22: _LL11B: _LL11C:
 return& Cyc_Tcutil_trk;case 3: _LL11D: _LL11E:
# 591
 return& Cyc_Tcutil_ak;case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A9)->f1).field_info).KnownDatatypefield).tag == 2){_LL11F: _tmp1BE=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A9)->f1).field_info).KnownDatatypefield).val).f1;_tmp1BD=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1A9)->f1).field_info).KnownDatatypefield).val).f2;_LL120:
# 593
 return& Cyc_Tcutil_mk;}else{_LL121: _LL122:
# 595
({void*_tmp1AA=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1AB="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1AB,sizeof(char),39);}),_tag_dyneither(_tmp1AA,sizeof(void*),0));});}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A9)->f1).aggr_info).UnknownAggr).tag == 1){_LL123: _LL124:
# 598
 return& Cyc_Tcutil_ak;}else{_LL125: _tmp1C3=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A9)->f1).aggr_info).KnownAggr).val)->kind;_tmp1C2=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A9)->f1).aggr_info).KnownAggr).val)->tvs;_tmp1C1=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A9)->f1).aggr_info).KnownAggr).val)->impl;_tmp1C0=(*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A9)->f1).aggr_info).KnownAggr).val)->expected_mem_kind;_tmp1BF=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A9)->f1).targs;_LL126:
# 600
 if(_tmp1C1 == 0){
if(_tmp1C0)
return& Cyc_Tcutil_mk;else{
# 604
return& Cyc_Tcutil_ak;}}{
# 606
struct Cyc_List_List*_tmp1AC=_tmp1C1->fields;
if(_tmp1AC == 0)return& Cyc_Tcutil_mk;
# 609
if(_tmp1C3 == Cyc_Absyn_StructA){
for(0;_tmp1AC->tl != 0;_tmp1AC=_tmp1AC->tl){;}{
void*_tmp1AD=((struct Cyc_Absyn_Aggrfield*)_tmp1AC->hd)->type;
struct Cyc_Absyn_Kind*_tmp1AE=Cyc_Tcutil_field_kind(_tmp1AD,_tmp1BF,_tmp1C2);
if(_tmp1AE == & Cyc_Tcutil_ak  || _tmp1AE == & Cyc_Tcutil_tak)return _tmp1AE;};}else{
# 617
for(0;_tmp1AC != 0;_tmp1AC=_tmp1AC->tl){
void*_tmp1AF=((struct Cyc_Absyn_Aggrfield*)_tmp1AC->hd)->type;
struct Cyc_Absyn_Kind*_tmp1B0=Cyc_Tcutil_field_kind(_tmp1AF,_tmp1BF,_tmp1C2);
if(_tmp1B0 == & Cyc_Tcutil_ak  || _tmp1B0 == & Cyc_Tcutil_tak)return _tmp1B0;}}
# 623
return& Cyc_Tcutil_mk;};}case 12: _LL127: _LL128:
 return& Cyc_Tcutil_mk;case 5: _LL129: _tmp1C4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A9)->f1;_LL12A: {
# 626
void*_tmp1B1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,(_tmp1C4.ptr_atts).bounds);void*_tmp1B2=_tmp1B1;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp1B2)->tag == 0){_LL146: _LL147: {
# 628
enum Cyc_Absyn_AliasQual _tmp1B3=(Cyc_Tcutil_typ_kind((_tmp1C4.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1B4=_tmp1B3;switch(_tmp1B4){case Cyc_Absyn_Aliasable: _LL14B: _LL14C:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL14D: _LL14E:
 return& Cyc_Tcutil_umk;default: _LL14F: _LL150:
 return& Cyc_Tcutil_tmk;}_LL14A:;}}else{_LL148: _LL149: {
# 634
enum Cyc_Absyn_AliasQual _tmp1B5=(Cyc_Tcutil_typ_kind((_tmp1C4.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1B6=_tmp1B5;switch(_tmp1B6){case Cyc_Absyn_Aliasable: _LL152: _LL153:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL154: _LL155:
 return& Cyc_Tcutil_ubk;default: _LL156: _LL157:
 return& Cyc_Tcutil_tbk;}_LL151:;}}_LL145:;}case 18: _LL12B: _LL12C:
# 640
 return& Cyc_Tcutil_ik;case 28: _LL12D: _tmp1C5=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp1A9)->f2;_LL12E:
 return _tmp1C5;case 27: _LL12F: _LL130:
# 645
 return& Cyc_Tcutil_ak;case 19: _LL131: _LL132:
 return& Cyc_Tcutil_bk;case 8: _LL133: _tmp1C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1A9)->f1).num_elts;_LL134:
# 648
 if(_tmp1C6 == 0  || Cyc_Tcutil_is_const_exp(_tmp1C6))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 10: _LL135: _LL136:
 return& Cyc_Tcutil_mk;case 17: _LL137: _tmp1C7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1A9)->f3;_LL138:
# 652
 if(_tmp1C7 == 0  || _tmp1C7->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B9;_tmp1B9.tag=0;_tmp1B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp1B7[1]={& _tmp1B9};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1B8="typ_kind: typedef found: %s";_tag_dyneither(_tmp1B8,sizeof(char),28);}),_tag_dyneither(_tmp1B7,sizeof(void*),1));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1C7->kind))->v;case 26: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp1A9)->f1)->r)){case 0: _LL139: _LL13A:
 return& Cyc_Tcutil_ak;case 1: _LL13B: _LL13C:
 return& Cyc_Tcutil_bk;default: _LL13D: _LL13E:
 return& Cyc_Tcutil_ak;}case 23: _LL13F: _LL140:
 goto _LL142;case 24: _LL141: _LL142:
 goto _LL144;default: _LL143: _LL144:
 return& Cyc_Tcutil_ek;}_LL102:;}
# 664
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2  || k1->kind == k2->kind  && k1->aliasqual == k2->aliasqual;}
# 669
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1C8;_push_handler(& _tmp1C8);{int _tmp1CA=0;if(setjmp(_tmp1C8.handler))_tmp1CA=1;if(!_tmp1CA){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1CB=1;_npop_handler(0);return _tmp1CB;};
# 671
;_pop_handler();}else{void*_tmp1C9=(void*)_exn_thrown;void*_tmp1CC=_tmp1C9;void*_tmp1CD;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1CC)->tag == Cyc_Tcutil_Unify){_LL159: _LL15A:
# 673
 return 0;}else{_LL15B: _tmp1CD=_tmp1CC;_LL15C:(int)_rethrow(_tmp1CD);}_LL158:;}};}
# 678
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1CE=t;struct Cyc_List_List*_tmp1F4;void*_tmp1F3;void*_tmp1F2;void*_tmp1F1;void*_tmp1F0;struct Cyc_List_List*_tmp1EF;struct Cyc_List_List*_tmp1EE;struct Cyc_List_List*_tmp1ED;struct Cyc_List_List*_tmp1EC;struct Cyc_List_List*_tmp1EB;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*_tmp1E9;void*_tmp1E8;struct Cyc_Absyn_Tqual _tmp1E7;void*_tmp1E6;struct Cyc_List_List*_tmp1E5;int _tmp1E4;struct Cyc_Absyn_VarargInfo*_tmp1E3;struct Cyc_List_List*_tmp1E2;struct Cyc_List_List*_tmp1E1;struct Cyc_Absyn_Exp*_tmp1E0;struct Cyc_List_List*_tmp1DF;struct Cyc_Absyn_Exp*_tmp1DE;struct Cyc_List_List*_tmp1DD;void*_tmp1DC;struct Cyc_Absyn_PtrInfo _tmp1DB;void*_tmp1DA;struct Cyc_Core_Opt**_tmp1D9;struct Cyc_Absyn_Tvar*_tmp1D8;switch(*((int*)_tmp1CE)){case 2: _LL15E: _tmp1D8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CE)->f1;_LL15F:
# 683
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1D8)){
Cyc_Tcutil_failure_reason=({const char*_tmp1CF="(type variable would escape scope)";_tag_dyneither(_tmp1CF,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 687
goto _LL15D;case 1: _LL160: _tmp1DA=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CE)->f2;_tmp1D9=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CE)->f4;_LL161:
# 689
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1D0="(occurs check)";_tag_dyneither(_tmp1D0,sizeof(char),15);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 693
if(_tmp1DA != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1DA);else{
# 696
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D9))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 703
if(problem){
struct Cyc_List_List*_tmp1D1=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1D9))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1D1=({struct Cyc_List_List*_tmp1D2=_cycalloc(sizeof(*_tmp1D2));_tmp1D2->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1D2->tl=_tmp1D1;_tmp1D2;});}}
# 709
*_tmp1D9=({struct Cyc_Core_Opt*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->v=_tmp1D1;_tmp1D3;});}}}
# 712
goto _LL15D;case 5: _LL162: _tmp1DB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1CE)->f1;_LL163:
# 715
 Cyc_Tcutil_occurs(evar,r,env,_tmp1DB.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1DB.ptr_atts).rgn);
goto _LL15D;case 8: _LL164: _tmp1DC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1CE)->f1).elt_type;_LL165:
# 719
 Cyc_Tcutil_occurs(evar,r,env,_tmp1DC);goto _LL15D;case 9: _LL166: _tmp1E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).tvars;_tmp1E8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).effect;_tmp1E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).ret_tqual;_tmp1E6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).ret_typ;_tmp1E5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).args;_tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).c_varargs;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).cyc_varargs;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).rgn_po;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).attributes;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).requires_clause;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).requires_relns;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).ensures_clause;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1CE)->f1).ensures_relns;_LL167:
# 722
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1E9,env);
if(_tmp1E8 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1E8);
Cyc_Tcutil_occurs(evar,r,env,_tmp1E6);
for(0;_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple10*)_tmp1E5->hd)).f3);}
if(_tmp1E3 != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1E3->type);
for(0;_tmp1E2 != 0;_tmp1E2=_tmp1E2->tl){
struct _tuple0*_tmp1D4=(struct _tuple0*)_tmp1E2->hd;struct _tuple0*_tmp1D5=_tmp1D4;void*_tmp1D7;void*_tmp1D6;_LL181: _tmp1D7=_tmp1D5->f1;_tmp1D6=_tmp1D5->f2;_LL182:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1D7);
Cyc_Tcutil_occurs(evar,r,env,_tmp1D6);}
# 734
goto _LL15D;case 10: _LL168: _tmp1EA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1CE)->f1;_LL169:
# 736
 for(0;_tmp1EA != 0;_tmp1EA=_tmp1EA->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple12*)_tmp1EA->hd)).f2);}
goto _LL15D;case 3: _LL16A: _tmp1EB=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp1CE)->f1).targs;_LL16B:
# 740
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1EB);goto _LL15D;case 17: _LL16C: _tmp1EC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1CE)->f2;_LL16D:
 _tmp1ED=_tmp1EC;goto _LL16F;case 4: _LL16E: _tmp1ED=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp1CE)->f1).targs;_LL16F:
 _tmp1EE=_tmp1ED;goto _LL171;case 11: _LL170: _tmp1EE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1CE)->f1).targs;_LL171:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1EE);goto _LL15D;case 12: _LL172: _tmp1EF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1CE)->f2;_LL173:
# 746
 for(0;_tmp1EF != 0;_tmp1EF=_tmp1EF->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1EF->hd)->type);}
goto _LL15D;case 19: _LL174: _tmp1F0=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp1CE)->f1;_LL175:
 _tmp1F1=_tmp1F0;goto _LL177;case 23: _LL176: _tmp1F1=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp1CE)->f1;_LL177:
 _tmp1F2=_tmp1F1;goto _LL179;case 15: _LL178: _tmp1F2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp1CE)->f1;_LL179:
 _tmp1F3=_tmp1F2;goto _LL17B;case 25: _LL17A: _tmp1F3=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp1CE)->f1;_LL17B:
 Cyc_Tcutil_occurs(evar,r,env,_tmp1F3);goto _LL15D;case 24: _LL17C: _tmp1F4=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp1CE)->f1;_LL17D:
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1F4);goto _LL15D;default: _LL17E: _LL17F:
# 756
 goto _LL15D;}_LL15D:;};}
# 759
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 761
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 766
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0  && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0  || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 774
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const  && !tq1.real_const)
({void*_tmp1F5=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F6="tq1 real_const not set.";_tag_dyneither(_tmp1F6,sizeof(char),24);}),_tag_dyneither(_tmp1F5,sizeof(void*),0));});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1F7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp1F8="tq2 real_const not set.";_tag_dyneither(_tmp1F8,sizeof(char),24);}),_tag_dyneither(_tmp1F7,sizeof(void*),0));});
# 780
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 783
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp1F9="(qualifiers don't match)";_tag_dyneither(_tmp1F9,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 791
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 795
int Cyc_Tcutil_equal_tqual(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
return(tq1.real_const == tq2.real_const  && tq1.q_volatile == tq2.q_volatile) && tq1.q_restrict == tq2.q_restrict;}
# 802
static void Cyc_Tcutil_unify_it_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason){
# 804
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1FA=x;void*_tmp202;switch((((union Cyc_Absyn_Constraint*)_tmp1FA)->Eq_constr).tag){case 3: _LL184: _LL185:
# 809
*x=({union Cyc_Absyn_Constraint _tmp1FB;(_tmp1FB.Forward_constr).val=y;(_tmp1FB.Forward_constr).tag=2;_tmp1FB;});return;case 1: _LL186: _tmp202=(void*)(_tmp1FA->Eq_constr).val;_LL187: {
# 811
union Cyc_Absyn_Constraint*_tmp1FC=y;void*_tmp1FF;switch((((union Cyc_Absyn_Constraint*)_tmp1FC)->Eq_constr).tag){case 3: _LL18B: _LL18C:
*y=*x;return;case 1: _LL18D: _tmp1FF=(void*)(_tmp1FC->Eq_constr).val;_LL18E:
# 814
 if(cmp(_tmp202,_tmp1FF)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 818
return;default: _LL18F: _LL190:
({void*_tmp1FD=0;Cyc_Tcutil_impos(({const char*_tmp1FE="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1FE,sizeof(char),40);}),_tag_dyneither(_tmp1FD,sizeof(void*),0));});}_LL18A:;}default: _LL188: _LL189:
# 821
({void*_tmp200=0;Cyc_Tcutil_impos(({const char*_tmp201="unify_conref: forward after compress";_tag_dyneither(_tmp201,sizeof(char),37);}),_tag_dyneither(_tmp200,sizeof(void*),0));});}_LL183:;};}
# 825
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp203;_push_handler(& _tmp203);{int _tmp205=0;if(setjmp(_tmp203.handler))_tmp205=1;if(!_tmp205){
Cyc_Tcutil_unify_it_conrefs(cmp,x,y,_tag_dyneither(0,0,0));{
int _tmp206=1;_npop_handler(0);return _tmp206;};
# 827
;_pop_handler();}else{void*_tmp204=(void*)_exn_thrown;void*_tmp207=_tmp204;void*_tmp208;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp207)->tag == Cyc_Tcutil_Unify){_LL192: _LL193:
# 829
 return 0;}else{_LL194: _tmp208=_tmp207;_LL195:(int)_rethrow(_tmp208);}_LL191:;}};}
# 832
static int Cyc_Tcutil_boundscmp(void*b1,void*b2){
struct _tuple0 _tmp209=({struct _tuple0 _tmp20D;_tmp20D.f1=b1;_tmp20D.f2=b2;_tmp20D;});struct _tuple0 _tmp20A=_tmp209;struct Cyc_Absyn_Exp*_tmp20C;struct Cyc_Absyn_Exp*_tmp20B;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp20A.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp20A.f2)->tag == 0){_LL197: _LL198:
 return 0;}else{_LL199: _LL19A:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp20A.f2)->tag == 0){_LL19B: _LL19C:
 return 1;}else{_LL19D: _tmp20C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp20A.f1)->f1;_tmp20B=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp20A.f2)->f1;_LL19E:
# 838
 return Cyc_Evexp_const_exp_cmp(_tmp20C,_tmp20B);}}_LL196:;}
# 842
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
{struct _tuple0 _tmp20E=({struct _tuple0 _tmp21C;_tmp21C.f1=e1->r;_tmp21C.f2=e2->r;_tmp21C;});struct _tuple0 _tmp20F=_tmp20E;void*_tmp21B;void*_tmp21A;struct Cyc_Absyn_Exp*_tmp219;struct Cyc_Absyn_Exp*_tmp218;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20F.f1)->tag == 13){_LL1A0: _tmp218=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20F.f1)->f2;_LL1A1:
# 848
 Cyc_Tcutil_unify_cmp_exp(_tmp218,e2);
return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20F.f2)->tag == 13){_LL1A2: _tmp219=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp20F.f2)->f2;_LL1A3:
# 851
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp219);
return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20F.f1)->tag == 38){_LL1A4: _tmp21A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20F.f1)->f1;_LL1A5:
# 854
{void*_tmp210=Cyc_Tcutil_compress(_tmp21A);void*_tmp211=_tmp210;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp211)->tag == 1){_LL1AB: _LL1AC:
# 856
 Cyc_Tcutil_unify_it(_tmp21A,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp212=_cycalloc(sizeof(*_tmp212));_tmp212[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp213;_tmp213.tag=18;_tmp213.f1=e2;_tmp213;});_tmp212;}));
return;}else{_LL1AD: _LL1AE:
# 859
 goto _LL1AA;}_LL1AA:;}
# 861
goto _LL19F;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20F.f2)->tag == 38){_LL1A6: _tmp21B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20F.f2)->f1;_LL1A7:
# 863
{void*_tmp214=Cyc_Tcutil_compress(_tmp21B);void*_tmp215=_tmp214;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp215)->tag == 1){_LL1B0: _LL1B1:
# 865
 Cyc_Tcutil_unify_it(_tmp21B,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp216=_cycalloc(sizeof(*_tmp216));_tmp216[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp217;_tmp217.tag=18;_tmp217.f1=e1;_tmp217;});_tmp216;}));
return;}else{_LL1B2: _LL1B3:
# 868
 goto _LL1AF;}_LL1AF:;}
# 870
goto _LL19F;}else{_LL1A8: _LL1A9:
 goto _LL19F;}}}}_LL19F:;}
# 873
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 876
static int Cyc_Tcutil_unify_it_bounds(void*b1,void*b2){
struct _tuple0 _tmp21D=({struct _tuple0 _tmp227;_tmp227.f1=b1;_tmp227.f2=b2;_tmp227;});struct _tuple0 _tmp21E=_tmp21D;struct Cyc_Absyn_Exp*_tmp226;struct Cyc_Absyn_Exp*_tmp225;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp21E.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp21E.f2)->tag == 0){_LL1B5: _LL1B6:
 return 0;}else{_LL1B7: _LL1B8:
 return - 1;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp21E.f2)->tag == 0){_LL1B9: _LL1BA:
 return 1;}else{_LL1BB: _tmp226=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp21E.f1)->f1;_tmp225=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp21E.f2)->f1;_LL1BC: {
# 882
int r=Cyc_Evexp_const_exp_cmp(_tmp226,_tmp225);
if(r == 0)return 0;{
struct _handler_cons _tmp21F;_push_handler(& _tmp21F);{int _tmp221=0;if(setjmp(_tmp21F.handler))_tmp221=1;if(!_tmp221){
Cyc_Tcutil_unify_cmp_exp(_tmp226,_tmp225);{
int _tmp222=0;_npop_handler(0);return _tmp222;};
# 885
;_pop_handler();}else{void*_tmp220=(void*)_exn_thrown;void*_tmp223=_tmp220;void*_tmp224;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp223)->tag == Cyc_Tcutil_Unify){_LL1BE: _LL1BF:
# 888
 return r;}else{_LL1C0: _tmp224=_tmp223;_LL1C1:(int)_rethrow(_tmp224);}_LL1BD:;}};};}}}_LL1B4:;}
# 893
static int Cyc_Tcutil_attribute_case_number(void*att){
void*_tmp228=att;switch(*((int*)_tmp228)){case 0: _LL1C3: _LL1C4:
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
# 920
static int Cyc_Tcutil_attribute_cmp(void*att1,void*att2){
struct _tuple0 _tmp229=({struct _tuple0 _tmp23B;_tmp23B.f1=att1;_tmp23B.f2=att2;_tmp23B;});struct _tuple0 _tmp22A=_tmp229;enum Cyc_Absyn_Format_Type _tmp23A;int _tmp239;int _tmp238;enum Cyc_Absyn_Format_Type _tmp237;int _tmp236;int _tmp235;struct _dyneither_ptr _tmp234;struct _dyneither_ptr _tmp233;struct Cyc_Absyn_Exp*_tmp232;struct Cyc_Absyn_Exp*_tmp231;int _tmp230;int _tmp22F;int _tmp22E;int _tmp22D;switch(*((int*)_tmp22A.f1)){case 0: if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp22A.f2)->tag == 0){_LL1F0: _tmp22E=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp22A.f1)->f1;_tmp22D=((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp22A.f2)->f1;_LL1F1:
 _tmp230=_tmp22E;_tmp22F=_tmp22D;goto _LL1F3;}else{goto _LL1FA;}case 20: if(((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp22A.f2)->tag == 20){_LL1F2: _tmp230=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp22A.f1)->f1;_tmp22F=((struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct*)_tmp22A.f2)->f1;_LL1F3:
# 924
 return Cyc_Core_intcmp(_tmp230,_tmp22F);}else{goto _LL1FA;}case 6: if(((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp22A.f2)->tag == 6){_LL1F4: _tmp232=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp22A.f1)->f1;_tmp231=((struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*)_tmp22A.f2)->f1;_LL1F5:
# 926
 if(_tmp232 == _tmp231)return 0;
if(_tmp232 == 0)return - 1;
if(_tmp231 == 0)return 1;
return Cyc_Evexp_const_exp_cmp(_tmp232,_tmp231);}else{goto _LL1FA;}case 8: if(((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp22A.f2)->tag == 8){_LL1F6: _tmp234=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp22A.f1)->f1;_tmp233=((struct Cyc_Absyn_Section_att_Absyn_Attribute_struct*)_tmp22A.f2)->f1;_LL1F7:
 return Cyc_strcmp((struct _dyneither_ptr)_tmp234,(struct _dyneither_ptr)_tmp233);}else{goto _LL1FA;}case 19: if(((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp22A.f2)->tag == 19){_LL1F8: _tmp23A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp22A.f1)->f1;_tmp239=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp22A.f1)->f2;_tmp238=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp22A.f1)->f3;_tmp237=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp22A.f2)->f1;_tmp236=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp22A.f2)->f2;_tmp235=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp22A.f2)->f3;_LL1F9: {
# 932
int _tmp22B=Cyc_Core_intcmp((int)((unsigned int)_tmp23A),(int)((unsigned int)_tmp237));
if(_tmp22B != 0)return _tmp22B;{
int _tmp22C=Cyc_Core_intcmp(_tmp239,_tmp236);
if(_tmp22C != 0)return _tmp22C;
return Cyc_Core_intcmp(_tmp238,_tmp235);};}}else{goto _LL1FA;}default: _LL1FA: _LL1FB:
# 938
 return Cyc_Core_intcmp(Cyc_Tcutil_attribute_case_number(att1),Cyc_Tcutil_attribute_case_number(att2));}_LL1EF:;}
# 942
static int Cyc_Tcutil_equal_att(void*a1,void*a2){
return Cyc_Tcutil_attribute_cmp(a1,a2)== 0;}
# 946
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 955
static void*Cyc_Tcutil_rgns_of(void*t);
# 957
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 961
static struct _tuple16*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp23C=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp23D=_tmp23C;switch(((struct Cyc_Absyn_Kind*)_tmp23D)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp23D)->aliasqual){case Cyc_Absyn_Unique: _LL1FD: _LL1FE:
 t=(void*)& Cyc_Absyn_UniqueRgn_val;goto _LL1FC;case Cyc_Absyn_Aliasable: _LL1FF: _LL200:
 t=(void*)& Cyc_Absyn_HeapRgn_val;goto _LL1FC;default: goto _LL205;}case Cyc_Absyn_EffKind: _LL201: _LL202:
 t=Cyc_Absyn_empty_effect;goto _LL1FC;case Cyc_Absyn_IntKind: _LL203: _LL204:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp23E=_cycalloc(sizeof(*_tmp23E));_tmp23E[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp23F;_tmp23F.tag=18;_tmp23F.f1=Cyc_Absyn_uint_exp(0,0);_tmp23F;});_tmp23E;});goto _LL1FC;default: _LL205: _LL206:
 t=Cyc_Absyn_sint_typ;goto _LL1FC;}_LL1FC:;}
# 970
return({struct _tuple16*_tmp240=_cycalloc(sizeof(*_tmp240));_tmp240->f1=tv;_tmp240->f2=t;_tmp240;});}
# 977
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp241=Cyc_Tcutil_compress(t);void*_tmp242=_tmp241;struct Cyc_List_List*_tmp272;void*_tmp271;struct Cyc_List_List*_tmp270;void*_tmp26F;struct Cyc_Absyn_Tqual _tmp26E;void*_tmp26D;struct Cyc_List_List*_tmp26C;struct Cyc_Absyn_VarargInfo*_tmp26B;struct Cyc_List_List*_tmp26A;struct Cyc_List_List*_tmp269;struct Cyc_List_List*_tmp268;struct Cyc_List_List*_tmp267;struct Cyc_List_List*_tmp266;void*_tmp265;void*_tmp264;void*_tmp263;struct Cyc_List_List*_tmp262;void*_tmp261;void*_tmp260;void*_tmp25F;switch(*((int*)_tmp242)){case 0: _LL208: _LL209:
 goto _LL20B;case 7: _LL20A: _LL20B:
 goto _LL20D;case 13: _LL20C: _LL20D:
 goto _LL20F;case 14: _LL20E: _LL20F:
 goto _LL211;case 18: _LL210: _LL211:
 goto _LL213;case 27: _LL212: _LL213:
 goto _LL215;case 28: _LL214: _LL215:
 goto _LL217;case 6: _LL216: _LL217:
 return Cyc_Absyn_empty_effect;case 1: _LL218: _LL219:
 goto _LL21B;case 2: _LL21A: _LL21B: {
# 989
struct Cyc_Absyn_Kind*_tmp243=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp244=_tmp243;switch(((struct Cyc_Absyn_Kind*)_tmp244)->kind){case Cyc_Absyn_RgnKind: _LL243: _LL244:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp245=_cycalloc(sizeof(*_tmp245));_tmp245[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp246;_tmp246.tag=23;_tmp246.f1=t;_tmp246;});_tmp245;});case Cyc_Absyn_EffKind: _LL245: _LL246:
 return t;case Cyc_Absyn_IntKind: _LL247: _LL248:
 return Cyc_Absyn_empty_effect;default: _LL249: _LL24A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp247=_cycalloc(sizeof(*_tmp247));_tmp247[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp248;_tmp248.tag=25;_tmp248.f1=t;_tmp248;});_tmp247;});}_LL242:;}case 15: _LL21C: _tmp25F=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp242)->f1;_LL21D:
# 995
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp249=_cycalloc(sizeof(*_tmp249));_tmp249[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp24A;_tmp24A.tag=23;_tmp24A.f1=_tmp25F;_tmp24A;});_tmp249;});case 16: _LL21E: _tmp261=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp242)->f1;_tmp260=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp242)->f2;_LL21F:
# 999
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp24B=_cycalloc(sizeof(*_tmp24B));_tmp24B[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp24C;_tmp24C.tag=23;_tmp24C.f1=_tmp260;_tmp24C;});_tmp24B;});case 3: _LL220: _tmp262=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp242)->f1).targs;_LL221: {
# 1001
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp262);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp24E;_tmp24E.tag=24;_tmp24E.f1=ts;_tmp24E;});_tmp24D;}));}case 5: _LL222: _tmp264=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp242)->f1).elt_typ;_tmp263=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp242)->f1).ptr_atts).rgn;_LL223:
# 1005
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp250;_tmp250.tag=24;_tmp250.f1=({void*_tmp251[2];_tmp251[1]=Cyc_Tcutil_rgns_of(_tmp264);_tmp251[0]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp252=_cycalloc(sizeof(*_tmp252));_tmp252[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp253;_tmp253.tag=23;_tmp253.f1=_tmp263;_tmp253;});_tmp252;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp251,sizeof(void*),2));});_tmp250;});_tmp24F;}));case 8: _LL224: _tmp265=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp242)->f1).elt_type;_LL225:
# 1007
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp265));case 10: _LL226: _tmp266=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp242)->f1;_LL227: {
# 1009
struct Cyc_List_List*_tmp254=0;
for(0;_tmp266 != 0;_tmp266=_tmp266->tl){
_tmp254=({struct Cyc_List_List*_tmp255=_cycalloc(sizeof(*_tmp255));_tmp255->hd=(*((struct _tuple12*)_tmp266->hd)).f2;_tmp255->tl=_tmp254;_tmp255;});}
_tmp267=_tmp254;goto _LL229;}case 4: _LL228: _tmp267=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp242)->f1).targs;_LL229:
 _tmp268=_tmp267;goto _LL22B;case 11: _LL22A: _tmp268=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp242)->f1).targs;_LL22B:
# 1015
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp256=_cycalloc(sizeof(*_tmp256));_tmp256[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp257;_tmp257.tag=24;_tmp257.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp268);_tmp257;});_tmp256;}));case 12: _LL22C: _tmp269=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp242)->f2;_LL22D:
# 1017
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp258=_cycalloc(sizeof(*_tmp258));_tmp258[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp259;_tmp259.tag=24;_tmp259.f1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp269);_tmp259;});_tmp258;}));case 19: _LL22E: _LL22F:
 return Cyc_Absyn_empty_effect;case 9: _LL230: _tmp270=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242)->f1).tvars;_tmp26F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242)->f1).effect;_tmp26E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242)->f1).ret_tqual;_tmp26D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242)->f1).ret_typ;_tmp26C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242)->f1).args;_tmp26B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242)->f1).cyc_varargs;_tmp26A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp242)->f1).rgn_po;_LL231: {
# 1027
void*_tmp25A=Cyc_Tcutil_substitute(((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp270),(void*)_check_null(_tmp26F));
return Cyc_Tcutil_normalize_effect(_tmp25A);}case 21: _LL232: _LL233:
 goto _LL235;case 22: _LL234: _LL235:
 goto _LL237;case 20: _LL236: _LL237:
 return Cyc_Absyn_empty_effect;case 23: _LL238: _LL239:
 goto _LL23B;case 24: _LL23A: _LL23B:
 return t;case 25: _LL23C: _tmp271=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp242)->f1;_LL23D:
 return Cyc_Tcutil_rgns_of(_tmp271);case 17: _LL23E: _tmp272=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp242)->f2;_LL23F:
# 1036
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp25B=_cycalloc(sizeof(*_tmp25B));_tmp25B[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp25C;_tmp25C.tag=24;_tmp25C.f1=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp272);_tmp25C;});_tmp25B;}));default: _LL240: _LL241:
({void*_tmp25D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp25E="typedecl in rgns_of";_tag_dyneither(_tmp25E,sizeof(char),20);}),_tag_dyneither(_tmp25D,sizeof(void*),0));});}_LL207:;}
# 1044
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp273=e;void*_tmp27F;struct Cyc_List_List**_tmp27E;switch(*((int*)_tmp273)){case 24: _LL24C: _tmp27E=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp273)->f1;_LL24D: {
# 1048
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp27E;for(0;effs != 0;effs=effs->tl){
void*_tmp274=(void*)effs->hd;
effs->hd=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp274));{
void*_tmp275=(void*)effs->hd;void*_tmp276=_tmp275;switch(*((int*)_tmp276)){case 24: _LL253: _LL254:
 goto _LL256;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp276)->f1)){case 20: _LL255: _LL256:
 redo_join=1;goto _LL252;case 22: _LL257: _LL258:
 redo_join=1;goto _LL252;case 21: _LL259: _LL25A:
 redo_join=1;goto _LL252;default: goto _LL25B;}default: _LL25B: _LL25C:
 goto _LL252;}_LL252:;};}}
# 1060
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp27E;for(0;effs != 0;effs=effs->tl){
void*_tmp277=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp278=_tmp277;void*_tmp27B;struct Cyc_List_List*_tmp27A;switch(*((int*)_tmp278)){case 24: _LL25E: _tmp27A=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp278)->f1;_LL25F:
# 1065
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp27A,effects);
goto _LL25D;case 23: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp278)->f1)){case 20: _LL260: _LL261:
 goto _LL25D;case 22: _LL262: _LL263:
 goto _LL25D;case 21: _LL264: _LL265:
 goto _LL25D;default: goto _LL266;}default: _LL266: _tmp27B=_tmp278;_LL267:
 effects=({struct Cyc_List_List*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->hd=_tmp27B;_tmp279->tl=effects;_tmp279;});goto _LL25D;}_LL25D:;}}
# 1073
*_tmp27E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);
return e;};}case 25: _LL24E: _tmp27F=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp273)->f1;_LL24F: {
# 1076
void*_tmp27C=Cyc_Tcutil_compress(_tmp27F);void*_tmp27D=_tmp27C;switch(*((int*)_tmp27D)){case 1: _LL269: _LL26A:
 goto _LL26C;case 2: _LL26B: _LL26C:
 return e;default: _LL26D: _LL26E:
 return Cyc_Tcutil_rgns_of(_tmp27F);}_LL268:;}default: _LL250: _LL251:
# 1081
 return e;}_LL24B:;};}
# 1086
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp280=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp281=_cycalloc(sizeof(*_tmp281));_tmp281[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp282;_tmp282.tag=9;_tmp282.f1=({struct Cyc_Absyn_FnInfo _tmp283;_tmp283.tvars=0;_tmp283.effect=eff;_tmp283.ret_tqual=
Cyc_Absyn_empty_tqual(0);_tmp283.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp283.args=0;_tmp283.c_varargs=0;_tmp283.cyc_varargs=0;_tmp283.rgn_po=0;_tmp283.attributes=0;_tmp283.requires_clause=0;_tmp283.requires_relns=0;_tmp283.ensures_clause=0;_tmp283.ensures_relns=0;_tmp283;});_tmp282;});_tmp281;});
# 1096
return Cyc_Absyn_atb_typ((void*)_tmp280,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Absyn_empty_tqual(0),Cyc_Absyn_bounds_one,Cyc_Absyn_false_conref);}
# 1103
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp284=Cyc_Tcutil_compress(e);void*_tmp285=_tmp284;struct Cyc_Core_Opt*_tmp2AA;void**_tmp2A9;struct Cyc_Core_Opt*_tmp2A8;void*_tmp2A7;struct Cyc_List_List*_tmp2A6;void*_tmp2A5;switch(*((int*)_tmp285)){case 23: _LL270: _tmp2A5=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp285)->f1;_LL271:
# 1110
 if(constrain)return Cyc_Tcutil_unify(r,_tmp2A5);
_tmp2A5=Cyc_Tcutil_compress(_tmp2A5);
if(r == _tmp2A5)return 1;{
struct _tuple0 _tmp286=({struct _tuple0 _tmp28A;_tmp28A.f1=r;_tmp28A.f2=_tmp2A5;_tmp28A;});struct _tuple0 _tmp287=_tmp286;struct Cyc_Absyn_Tvar*_tmp289;struct Cyc_Absyn_Tvar*_tmp288;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp287.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp287.f2)->tag == 2){_LL27B: _tmp289=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp287.f1)->f1;_tmp288=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp287.f2)->f1;_LL27C:
 return Cyc_Absyn_tvar_cmp(_tmp289,_tmp288)== 0;}else{goto _LL27D;}}else{_LL27D: _LL27E:
 return 0;}_LL27A:;};case 24: _LL272: _tmp2A6=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp285)->f1;_LL273:
# 1118
 for(0;_tmp2A6 != 0;_tmp2A6=_tmp2A6->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp2A6->hd))return 1;}
return 0;case 25: _LL274: _tmp2A7=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp285)->f1;_LL275: {
# 1122
void*_tmp28B=Cyc_Tcutil_rgns_of(_tmp2A7);void*_tmp28C=_tmp28B;void*_tmp29A;void*_tmp299;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp28C)->tag == 25){_LL280: _tmp299=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp28C)->f1;_LL281:
# 1124
 if(!constrain)return 0;{
void*_tmp28D=Cyc_Tcutil_compress(_tmp299);void*_tmp28E=_tmp28D;struct Cyc_Core_Opt*_tmp298;void**_tmp297;struct Cyc_Core_Opt*_tmp296;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28E)->tag == 1){_LL285: _tmp298=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28E)->f1;_tmp297=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28E)->f2;_tmp296=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp28E)->f4;_LL286: {
# 1129
void*_tmp28F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp296);
# 1132
Cyc_Tcutil_occurs(_tmp28F,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp296))->v,r);{
void*_tmp290=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp291=_cycalloc(sizeof(*_tmp291));_tmp291[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp292;_tmp292.tag=24;_tmp292.f1=({void*_tmp293[2];_tmp293[1]=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp294=_cycalloc(sizeof(*_tmp294));_tmp294[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp295;_tmp295.tag=23;_tmp295.f1=r;_tmp295;});_tmp294;});_tmp293[0]=_tmp28F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp293,sizeof(void*),2));});_tmp292;});_tmp291;}));
*_tmp297=_tmp290;
return 1;};}}else{_LL287: _LL288:
 return 0;}_LL284:;};}else{_LL282: _tmp29A=_tmp28C;_LL283:
# 1138
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp29A);}_LL27F:;}case 1: _LL276: _tmp2AA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp285)->f1;_tmp2A9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp285)->f2;_tmp2A8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp285)->f4;_LL277:
# 1141
 if(_tmp2AA == 0  || ((struct Cyc_Absyn_Kind*)_tmp2AA->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp29B=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp29C="effect evar has wrong kind";_tag_dyneither(_tmp29C,sizeof(char),27);}),_tag_dyneither(_tmp29B,sizeof(void*),0));});
if(!constrain)return 0;{
# 1146
void*_tmp29D=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A8);
# 1149
Cyc_Tcutil_occurs(_tmp29D,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A8))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp29E=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp29F=_cycalloc(sizeof(*_tmp29F));_tmp29F[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2A0;_tmp2A0.tag=24;_tmp2A0.f1=({struct Cyc_List_List*_tmp2A1=_cycalloc(sizeof(*_tmp2A1));_tmp2A1->hd=_tmp29D;_tmp2A1->tl=({struct Cyc_List_List*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp2A3=_cycalloc(sizeof(*_tmp2A3));_tmp2A3[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp2A4;_tmp2A4.tag=23;_tmp2A4.f1=r;_tmp2A4;});_tmp2A3;});_tmp2A2->tl=0;_tmp2A2;});_tmp2A1;});_tmp2A0;});_tmp29F;});
*_tmp2A9=(void*)_tmp29E;
return 1;};};default: _LL278: _LL279:
 return 0;}_LL26F:;};}
# 1160
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp2AB=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2AC=_tmp2AB;struct Cyc_Core_Opt*_tmp2C4;void**_tmp2C3;struct Cyc_Core_Opt*_tmp2C2;void*_tmp2C1;struct Cyc_List_List*_tmp2C0;switch(*((int*)_tmp2AC)){case 23: _LL28A: _LL28B:
 return 0;case 24: _LL28C: _tmp2C0=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2AC)->f1;_LL28D:
# 1165
 for(0;_tmp2C0 != 0;_tmp2C0=_tmp2C0->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2C0->hd))
return 1;}
return 0;case 25: _LL28E: _tmp2C1=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2AC)->f1;_LL28F:
# 1170
 _tmp2C1=Cyc_Tcutil_compress(_tmp2C1);
if(t == _tmp2C1)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2C1);{
void*_tmp2AD=Cyc_Tcutil_rgns_of(t);void*_tmp2AE=_tmp2AD;void*_tmp2B5;void*_tmp2B4;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2AE)->tag == 25){_LL295: _tmp2B4=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2AE)->f1;_LL296: {
# 1175
struct _tuple0 _tmp2AF=({struct _tuple0 _tmp2B3;_tmp2B3.f1=Cyc_Tcutil_compress(_tmp2B4);_tmp2B3.f2=_tmp2C1;_tmp2B3;});struct _tuple0 _tmp2B0=_tmp2AF;struct Cyc_Absyn_Tvar*_tmp2B2;struct Cyc_Absyn_Tvar*_tmp2B1;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B0.f1)->tag == 2){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B0.f2)->tag == 2){_LL29A: _tmp2B2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B0.f1)->f1;_tmp2B1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2B0.f2)->f1;_LL29B:
 return Cyc_Tcutil_unify(t,_tmp2C1);}else{goto _LL29C;}}else{_LL29C: _LL29D:
 return _tmp2B4 == _tmp2C1;}_LL299:;}}else{_LL297: _tmp2B5=_tmp2AE;_LL298:
# 1179
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2B5);}_LL294:;};case 1: _LL290: _tmp2C4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AC)->f1;_tmp2C3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AC)->f2;_tmp2C2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AC)->f4;_LL291:
# 1182
 if(_tmp2C4 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2C4->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2B6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2B7="effect evar has wrong kind";_tag_dyneither(_tmp2B7,sizeof(char),27);}),_tag_dyneither(_tmp2B6,sizeof(void*),0));});
if(!may_constrain_evars)return 0;{
# 1187
void*_tmp2B8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2C2);
# 1190
Cyc_Tcutil_occurs(_tmp2B8,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2C2))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B9=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2BA=_cycalloc(sizeof(*_tmp2BA));_tmp2BA[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2BB;_tmp2BB.tag=24;_tmp2BB.f1=({struct Cyc_List_List*_tmp2BC=_cycalloc(sizeof(*_tmp2BC));_tmp2BC->hd=_tmp2B8;_tmp2BC->tl=({struct Cyc_List_List*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2BF;_tmp2BF.tag=25;_tmp2BF.f1=t;_tmp2BF;});_tmp2BE;});_tmp2BD->tl=0;_tmp2BD;});_tmp2BC;});_tmp2BB;});_tmp2BA;});
*_tmp2C3=(void*)_tmp2B9;
return 1;};};default: _LL292: _LL293:
 return 0;}_LL289:;};}
# 1201
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2C5=e;struct Cyc_Core_Opt*_tmp2E5;void**_tmp2E4;struct Cyc_Core_Opt*_tmp2E3;void*_tmp2E2;struct Cyc_List_List*_tmp2E1;struct Cyc_Absyn_Tvar*_tmp2E0;switch(*((int*)_tmp2C5)){case 2: _LL29F: _tmp2E0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2C5)->f1;_LL2A0:
 return Cyc_Absyn_tvar_cmp(v,_tmp2E0)== 0;case 24: _LL2A1: _tmp2E1=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2C5)->f1;_LL2A2:
# 1206
 for(0;_tmp2E1 != 0;_tmp2E1=_tmp2E1->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp2E1->hd))
return 1;}
return 0;case 25: _LL2A3: _tmp2E2=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C5)->f1;_LL2A4: {
# 1211
void*_tmp2C6=Cyc_Tcutil_rgns_of(_tmp2E2);void*_tmp2C7=_tmp2C6;void*_tmp2D5;void*_tmp2D4;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C7)->tag == 25){_LL2AA: _tmp2D4=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2C7)->f1;_LL2AB:
# 1213
 if(!may_constrain_evars)return 0;{
void*_tmp2C8=Cyc_Tcutil_compress(_tmp2D4);void*_tmp2C9=_tmp2C8;struct Cyc_Core_Opt*_tmp2D3;void**_tmp2D2;struct Cyc_Core_Opt*_tmp2D1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C9)->tag == 1){_LL2AF: _tmp2D3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C9)->f1;_tmp2D2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C9)->f2;_tmp2D1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C9)->f4;_LL2B0: {
# 1219
void*_tmp2CA=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2D1);
# 1221
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2D1))->v,v))return 0;{
void*_tmp2CB=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2CD;_tmp2CD.tag=24;_tmp2CD.f1=({void*_tmp2CE[2];_tmp2CE[1]=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2D0;_tmp2D0.tag=2;_tmp2D0.f1=v;_tmp2D0;});_tmp2CF;});_tmp2CE[0]=_tmp2CA;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2CE,sizeof(void*),2));});_tmp2CD;});_tmp2CC;}));
*_tmp2D2=_tmp2CB;
return 1;};}}else{_LL2B1: _LL2B2:
 return 0;}_LL2AE:;};}else{_LL2AC: _tmp2D5=_tmp2C7;_LL2AD:
# 1227
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2D5);}_LL2A9:;}case 1: _LL2A5: _tmp2E5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f1;_tmp2E4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f2;_tmp2E3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C5)->f4;_LL2A6:
# 1230
 if(_tmp2E5 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2E5->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2D6=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2D7="effect evar has wrong kind";_tag_dyneither(_tmp2D7,sizeof(char),27);}),_tag_dyneither(_tmp2D6,sizeof(void*),0));});{
# 1234
void*_tmp2D8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2E3);
# 1236
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2E3))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D9=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2DA=_cycalloc(sizeof(*_tmp2DA));_tmp2DA[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2DB;_tmp2DB.tag=24;_tmp2DB.f1=({struct Cyc_List_List*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));_tmp2DC->hd=_tmp2D8;_tmp2DC->tl=({struct Cyc_List_List*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2DE=_cycalloc(sizeof(*_tmp2DE));_tmp2DE[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2DF;_tmp2DF.tag=2;_tmp2DF.f1=v;_tmp2DF;});_tmp2DE;});_tmp2DD->tl=0;_tmp2DD;});_tmp2DC;});_tmp2DB;});_tmp2DA;});
*_tmp2E4=(void*)_tmp2D9;
return 1;};};default: _LL2A7: _LL2A8:
 return 0;}_LL29E:;};}
# 1246
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp2E6=e;void*_tmp2EC;struct Cyc_List_List*_tmp2EB;switch(*((int*)_tmp2E6)){case 24: _LL2B4: _tmp2EB=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2E6)->f1;_LL2B5:
# 1250
 for(0;_tmp2EB != 0;_tmp2EB=_tmp2EB->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp2EB->hd))
return 1;}
return 0;case 25: _LL2B6: _tmp2EC=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E6)->f1;_LL2B7: {
# 1255
void*_tmp2E7=Cyc_Tcutil_rgns_of(_tmp2EC);void*_tmp2E8=_tmp2E7;void*_tmp2EA;void*_tmp2E9;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E8)->tag == 25){_LL2BD: _tmp2E9=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2E8)->f1;_LL2BE:
 return 0;}else{_LL2BF: _tmp2EA=_tmp2E8;_LL2C0:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp2EA);}_LL2BC:;}case 1: _LL2B8: _LL2B9:
# 1259
 return evar == e;default: _LL2BA: _LL2BB:
 return 0;}_LL2B3:;};}
# 1273 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1278
void*_tmp2ED=Cyc_Tcutil_compress(e1);void*_tmp2EE=_tmp2ED;void**_tmp2FB;struct Cyc_Core_Opt*_tmp2FA;void*_tmp2F9;struct Cyc_Absyn_Tvar*_tmp2F8;void*_tmp2F7;struct Cyc_List_List*_tmp2F6;switch(*((int*)_tmp2EE)){case 24: _LL2C2: _tmp2F6=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2EE)->f1;_LL2C3:
# 1280
 for(0;_tmp2F6 != 0;_tmp2F6=_tmp2F6->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp2F6->hd,e2))
return 0;}
return 1;case 23: _LL2C4: _tmp2F7=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp2EE)->f1;_LL2C5:
# 1291
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp2F7,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2F7,(void*)& Cyc_Absyn_HeapRgn_val);case 2: _LL2C6: _tmp2F8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2EE)->f1;_LL2C7:
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp2F8,e2);case 25: _LL2C8: _tmp2F9=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2EE)->f1;_LL2C9: {
# 1295
void*_tmp2EF=Cyc_Tcutil_rgns_of(_tmp2F9);void*_tmp2F0=_tmp2EF;void*_tmp2F2;void*_tmp2F1;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F0)->tag == 25){_LL2CF: _tmp2F1=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2F0)->f1;_LL2D0:
# 1300
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp2F1,e2) || 
may_constrain_evars  && Cyc_Tcutil_unify(_tmp2F1,Cyc_Absyn_sint_typ);}else{_LL2D1: _tmp2F2=_tmp2F0;_LL2D2:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp2F2,e2);}_LL2CE:;}case 1: _LL2CA: _tmp2FB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EE)->f2;_tmp2FA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EE)->f4;_LL2CB:
# 1305
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1309
*_tmp2FB=Cyc_Absyn_empty_effect;
# 1312
return 1;}else{
# 1314
return 0;}default: _LL2CC: _LL2CD:
({struct Cyc_String_pa_PrintArg_struct _tmp2F5;_tmp2F5.tag=0;_tmp2F5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));({void*_tmp2F3[1]={& _tmp2F5};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp2F4="subset_effect: bad effect: %s";_tag_dyneither(_tmp2F4,sizeof(char),30);}),_tag_dyneither(_tmp2F3,sizeof(void*),1));});});}_LL2C1:;}
# 1330 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2) && Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2) && Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1346
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1348
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple0*_tmp2FC=(struct _tuple0*)r1->hd;struct _tuple0*_tmp2FD=_tmp2FC;void*_tmp303;void*_tmp302;_LL2D4: _tmp303=_tmp2FD->f1;_tmp302=_tmp2FD->f2;_LL2D5:;{
int found=_tmp303 == (void*)& Cyc_Absyn_HeapRgn_val;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0  && !found;r2=r2->tl){
struct _tuple0*_tmp2FE=(struct _tuple0*)r2->hd;struct _tuple0*_tmp2FF=_tmp2FE;void*_tmp301;void*_tmp300;_LL2D7: _tmp301=_tmp2FF->f1;_tmp300=_tmp2FF->f2;_LL2D8:;
if(Cyc_Tcutil_unify(_tmp303,_tmp301) && Cyc_Tcutil_unify(_tmp302,_tmp300)){
found=1;
break;}}}
# 1358
if(!found)return 0;};}}
# 1360
return 1;}
# 1363
typedef struct Cyc_List_List*Cyc_Tcutil_relns_t;
# 1367
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp304=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp305=({struct Cyc_List_List*_tmp306=_cycalloc(sizeof(*_tmp306));_tmp306->hd=_tmp304;_tmp306->tl=r1;_tmp306;});
if(Cyc_Relations_consistent_relations(_tmp305))return 0;}
# 1373
return 1;}
# 1378
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2) && Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1384
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1386
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2) && Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}struct _tuple22{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1390
void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_dyneither(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp307=t1;struct Cyc_Core_Opt*_tmp316;void**_tmp315;struct Cyc_Core_Opt*_tmp314;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp307)->tag == 1){_LL2DA: _tmp316=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp307)->f1;_tmp315=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp307)->f2;_tmp314=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp307)->f4;_LL2DB:
# 1401
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp314))->v,t2);{
struct Cyc_Absyn_Kind*_tmp308=Cyc_Tcutil_typ_kind(t2);
# 1406
if(Cyc_Tcutil_kind_leq(_tmp308,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp316))->v)){
*_tmp315=t2;
return;}else{
# 1410
{void*_tmp309=t2;struct Cyc_Absyn_PtrInfo _tmp312;void**_tmp311;struct Cyc_Core_Opt*_tmp310;switch(*((int*)_tmp309)){case 1: _LL2DF: _tmp311=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp309)->f2;_tmp310=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp309)->f4;_LL2E0: {
# 1413
struct Cyc_List_List*_tmp30A=(struct Cyc_List_List*)_tmp314->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp310))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp30A,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp30B="(type variable would escape scope)";_tag_dyneither(_tmp30B,sizeof(char),35);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1420
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp316->v,_tmp308)){
*_tmp311=t1;return;}
# 1423
Cyc_Tcutil_failure_reason=({const char*_tmp30C="(kinds are incompatible)";_tag_dyneither(_tmp30C,sizeof(char),25);});
goto _LL2DE;}case 5: _LL2E1: _tmp312=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp309)->f1;if(((struct Cyc_Absyn_Kind*)_tmp316->v)->kind == Cyc_Absyn_BoxKind){_LL2E2: {
# 1429
union Cyc_Absyn_Constraint*_tmp30D=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)((_tmp312.ptr_atts).bounds);
{union Cyc_Absyn_Constraint*_tmp30E=_tmp30D;if((((union Cyc_Absyn_Constraint*)_tmp30E)->No_constr).tag == 3){_LL2E6: _LL2E7:
# 1433
({struct _union_Constraint_Eq_constr*_tmp30F=& _tmp30D->Eq_constr;_tmp30F->tag=1;_tmp30F->val=Cyc_Absyn_bounds_one;});
*_tmp315=t2;
return;}else{_LL2E8: _LL2E9:
 goto _LL2E5;}_LL2E5:;}
# 1438
goto _LL2DE;}}else{goto _LL2E3;}default: _LL2E3: _LL2E4:
 goto _LL2DE;}_LL2DE:;}
# 1441
Cyc_Tcutil_failure_reason=({const char*_tmp313="(kinds are incompatible)";_tag_dyneither(_tmp313,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL2DC: _LL2DD:
# 1444
 goto _LL2D9;}_LL2D9:;}
# 1449
{struct _tuple0 _tmp317=({struct _tuple0 _tmp3DC;_tmp3DC.f1=t2;_tmp3DC.f2=t1;_tmp3DC;});struct _tuple0 _tmp318=_tmp317;void*_tmp3DB;void*_tmp3DA;void*_tmp3D9;void*_tmp3D8;void*_tmp3D7;void*_tmp3D6;struct Cyc_List_List*_tmp3D5;struct Cyc_Absyn_Typedefdecl*_tmp3D4;struct Cyc_List_List*_tmp3D3;struct Cyc_Absyn_Typedefdecl*_tmp3D2;enum Cyc_Absyn_AggrKind _tmp3D1;struct Cyc_List_List*_tmp3D0;enum Cyc_Absyn_AggrKind _tmp3CF;struct Cyc_List_List*_tmp3CE;struct Cyc_List_List*_tmp3CD;struct Cyc_List_List*_tmp3CC;struct Cyc_List_List*_tmp3CB;void*_tmp3CA;struct Cyc_Absyn_Tqual _tmp3C9;void*_tmp3C8;struct Cyc_List_List*_tmp3C7;int _tmp3C6;struct Cyc_Absyn_VarargInfo*_tmp3C5;struct Cyc_List_List*_tmp3C4;struct Cyc_List_List*_tmp3C3;struct Cyc_Absyn_Exp*_tmp3C2;struct Cyc_List_List*_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C0;struct Cyc_List_List*_tmp3BF;struct Cyc_List_List*_tmp3BE;void*_tmp3BD;struct Cyc_Absyn_Tqual _tmp3BC;void*_tmp3BB;struct Cyc_List_List*_tmp3BA;int _tmp3B9;struct Cyc_Absyn_VarargInfo*_tmp3B8;struct Cyc_List_List*_tmp3B7;struct Cyc_List_List*_tmp3B6;struct Cyc_Absyn_Exp*_tmp3B5;struct Cyc_List_List*_tmp3B4;struct Cyc_Absyn_Exp*_tmp3B3;struct Cyc_List_List*_tmp3B2;void*_tmp3B1;struct Cyc_Absyn_Tqual _tmp3B0;struct Cyc_Absyn_Exp*_tmp3AF;union Cyc_Absyn_Constraint*_tmp3AE;void*_tmp3AD;struct Cyc_Absyn_Tqual _tmp3AC;struct Cyc_Absyn_Exp*_tmp3AB;union Cyc_Absyn_Constraint*_tmp3AA;struct Cyc_Absyn_Exp*_tmp3A9;struct Cyc_Absyn_Exp*_tmp3A8;void*_tmp3A7;void*_tmp3A6;int _tmp3A5;int _tmp3A4;enum Cyc_Absyn_Sign _tmp3A3;enum Cyc_Absyn_Size_of _tmp3A2;enum Cyc_Absyn_Sign _tmp3A1;enum Cyc_Absyn_Size_of _tmp3A0;void*_tmp39F;struct Cyc_Absyn_Tqual _tmp39E;void*_tmp39D;union Cyc_Absyn_Constraint*_tmp39C;union Cyc_Absyn_Constraint*_tmp39B;union Cyc_Absyn_Constraint*_tmp39A;void*_tmp399;struct Cyc_Absyn_Tqual _tmp398;void*_tmp397;union Cyc_Absyn_Constraint*_tmp396;union Cyc_Absyn_Constraint*_tmp395;union Cyc_Absyn_Constraint*_tmp394;struct Cyc_Absyn_Datatypedecl*_tmp393;struct Cyc_Absyn_Datatypefield*_tmp392;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Datatypedecl*_tmp390;struct Cyc_Absyn_Datatypefield*_tmp38F;struct Cyc_List_List*_tmp38E;struct Cyc_Absyn_Datatypedecl*_tmp38D;struct Cyc_List_List*_tmp38C;struct Cyc_Absyn_Datatypedecl*_tmp38B;struct Cyc_List_List*_tmp38A;struct Cyc_List_List*_tmp389;struct Cyc_List_List*_tmp388;struct _tuple2*_tmp387;struct _tuple2*_tmp386;union Cyc_Absyn_AggrInfoU _tmp385;struct Cyc_List_List*_tmp384;union Cyc_Absyn_AggrInfoU _tmp383;struct Cyc_List_List*_tmp382;struct Cyc_Absyn_Tvar*_tmp381;struct Cyc_Absyn_Tvar*_tmp380;switch(*((int*)_tmp318.f1)){case 1: _LL2EB: _LL2EC:
# 1452
 Cyc_Tcutil_unify_it(t2,t1);
return;case 0: switch(*((int*)_tmp318.f2)){case 0: _LL2ED: _LL2EE:
# 1455
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 2: switch(*((int*)_tmp318.f2)){case 2: _LL2EF: _tmp381=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp380=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp318.f2)->f1;_LL2F0: {
# 1458
struct _dyneither_ptr*_tmp319=_tmp381->name;
struct _dyneither_ptr*_tmp31A=_tmp380->name;
# 1461
int _tmp31B=_tmp381->identity;
int _tmp31C=_tmp380->identity;
if(_tmp31C == _tmp31B)return;
Cyc_Tcutil_failure_reason=({const char*_tmp31D="(variable types are not the same)";_tag_dyneither(_tmp31D,sizeof(char),34);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 11: switch(*((int*)_tmp318.f2)){case 11: _LL2F1: _tmp385=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp318.f1)->f1).aggr_info;_tmp384=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp318.f1)->f1).targs;_tmp383=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp318.f2)->f1).aggr_info;_tmp382=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp318.f2)->f1).targs;_LL2F2: {
# 1468
struct _tuple13 _tmp31E=Cyc_Absyn_aggr_kinded_name(_tmp383);struct _tuple13 _tmp31F=_tmp31E;enum Cyc_Absyn_AggrKind _tmp327;struct _tuple2*_tmp326;_LL328: _tmp327=_tmp31F.f1;_tmp326=_tmp31F.f2;_LL329:;{
struct _tuple13 _tmp320=Cyc_Absyn_aggr_kinded_name(_tmp385);struct _tuple13 _tmp321=_tmp320;enum Cyc_Absyn_AggrKind _tmp325;struct _tuple2*_tmp324;_LL32B: _tmp325=_tmp321.f1;_tmp324=_tmp321.f2;_LL32C:;
if(_tmp327 != _tmp325){Cyc_Tcutil_failure_reason=({const char*_tmp322="(struct and union type)";_tag_dyneither(_tmp322,sizeof(char),24);});goto _LL2EA;}
if(Cyc_Absyn_qvar_cmp(_tmp326,_tmp324)!= 0){Cyc_Tcutil_failure_reason=({const char*_tmp323="(different type name)";_tag_dyneither(_tmp323,sizeof(char),22);});goto _LL2EA;}
Cyc_Tcutil_unify_list(_tmp382,_tmp384);
return;};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 13: switch(*((int*)_tmp318.f2)){case 13: _LL2F3: _tmp387=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp386=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp318.f2)->f1;_LL2F4:
# 1477
 if(Cyc_Absyn_qvar_cmp(_tmp387,_tmp386)== 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp328="(different enum types)";_tag_dyneither(_tmp328,sizeof(char),23);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 14: switch(*((int*)_tmp318.f2)){case 14: _LL2F5: _tmp389=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp388=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp318.f2)->f1;_LL2F6: {
# 1482
int bad=0;
for(0;_tmp389 != 0  && _tmp388 != 0;(_tmp389=_tmp389->tl,_tmp388=_tmp388->tl)){
struct Cyc_Absyn_Enumfield*_tmp329=(struct Cyc_Absyn_Enumfield*)_tmp389->hd;
struct Cyc_Absyn_Enumfield*_tmp32A=(struct Cyc_Absyn_Enumfield*)_tmp388->hd;
if(Cyc_Absyn_qvar_cmp(_tmp329->name,_tmp32A->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp32B="(different names for enum fields)";_tag_dyneither(_tmp32B,sizeof(char),34);});
bad=1;
break;}
# 1491
if(_tmp329->tag == _tmp32A->tag)continue;
if(_tmp329->tag == 0  || _tmp32A->tag == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp32C="(different tag values for enum fields)";_tag_dyneither(_tmp32C,sizeof(char),39);});
bad=1;
break;}
# 1497
if(!Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp329->tag),(struct Cyc_Absyn_Exp*)_check_null(_tmp32A->tag))){
Cyc_Tcutil_failure_reason=({const char*_tmp32D="(different tag values for enum fields)";_tag_dyneither(_tmp32D,sizeof(char),39);});
bad=1;
break;}}
# 1503
if(bad)goto _LL2EA;
if(_tmp389 == 0  && _tmp388 == 0)return;
Cyc_Tcutil_failure_reason=({const char*_tmp32E="(different number of fields for enums)";_tag_dyneither(_tmp32E,sizeof(char),39);});
goto _LL2EA;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp318.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp318.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp318.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL2F7: _tmp38D=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp318.f1)->f1).datatype_info).KnownDatatype).val;_tmp38C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp318.f1)->f1).targs;_tmp38B=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp318.f2)->f1).datatype_info).KnownDatatype).val;_tmp38A=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp318.f2)->f1).targs;_LL2F8:
# 1510
 if(_tmp38D == _tmp38B  || Cyc_Absyn_qvar_cmp(_tmp38D->name,_tmp38B->name)== 0){
Cyc_Tcutil_unify_list(_tmp38A,_tmp38C);
return;}
# 1514
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp318.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp318.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp318.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp318.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL2F9: _tmp393=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp318.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp392=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp318.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp391=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp318.f1)->f1).targs;_tmp390=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp318.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp38F=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp318.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp38E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp318.f2)->f1).targs;_LL2FA:
# 1518
 if((_tmp393 == _tmp390  || Cyc_Absyn_qvar_cmp(_tmp393->name,_tmp390->name)== 0) && (
_tmp392 == _tmp38F  || Cyc_Absyn_qvar_cmp(_tmp392->name,_tmp38F->name)== 0)){
Cyc_Tcutil_unify_list(_tmp38E,_tmp391);
return;}
# 1523
Cyc_Tcutil_failure_reason=({const char*_tmp32F="(different datatype field types)";_tag_dyneither(_tmp32F,sizeof(char),33);});
goto _LL2EA;}else{goto _LL325;}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}else{switch(*((int*)_tmp318.f2)){case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}}case 5: switch(*((int*)_tmp318.f2)){case 5: _LL2FB: _tmp39F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f1)->f1).elt_typ;_tmp39E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f1)->f1).elt_tq;_tmp39D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f1)->f1).ptr_atts).rgn;_tmp39C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f1)->f1).ptr_atts).nullable;_tmp39B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f1)->f1).ptr_atts).bounds;_tmp39A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f1)->f1).ptr_atts).zero_term;_tmp399=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f2)->f1).elt_typ;_tmp398=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f2)->f1).elt_tq;_tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f2)->f1).ptr_atts).rgn;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f2)->f1).ptr_atts).nullable;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f2)->f1).ptr_atts).bounds;_tmp394=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318.f2)->f1).ptr_atts).zero_term;_LL2FC:
# 1528
 Cyc_Tcutil_unify_it(_tmp399,_tmp39F);
Cyc_Tcutil_unify_it(_tmp39D,_tmp397);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp394,_tmp39A,({const char*_tmp330="(not both zero terminated)";_tag_dyneither(_tmp330,sizeof(char),27);}));
Cyc_Tcutil_unify_tqual(_tmp398,_tmp399,_tmp39E,_tmp39F);
((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp395,_tmp39B,({const char*_tmp331="(different pointer bounds)";_tag_dyneither(_tmp331,sizeof(char),27);}));
# 1536
{void*_tmp332=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp395);void*_tmp333=_tmp332;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp333)->tag == 0){_LL32E: _LL32F:
 return;}else{_LL330: _LL331:
 goto _LL32D;}_LL32D:;}
# 1540
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp396,_tmp39C,({const char*_tmp334="(incompatible pointer types)";_tag_dyneither(_tmp334,sizeof(char),29);}));
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 6: switch(*((int*)_tmp318.f2)){case 6: _LL2FD: _tmp3A3=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp3A2=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp318.f1)->f2;_tmp3A1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp318.f2)->f1;_tmp3A0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp318.f2)->f2;_LL2FE:
# 1545
 if(_tmp3A1 == _tmp3A3  && ((_tmp3A0 == _tmp3A2  || _tmp3A0 == Cyc_Absyn_Int_sz  && _tmp3A2 == Cyc_Absyn_Long_sz) || 
_tmp3A0 == Cyc_Absyn_Long_sz  && _tmp3A2 == Cyc_Absyn_Int_sz))return;
Cyc_Tcutil_failure_reason=({const char*_tmp335="(different integral types)";_tag_dyneither(_tmp335,sizeof(char),27);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 7: switch(*((int*)_tmp318.f2)){case 7: _LL2FF: _tmp3A5=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp3A4=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp318.f2)->f1;_LL300:
# 1551
 if(_tmp3A4 == 0  && _tmp3A5 == 0)return;else{
if(_tmp3A4 == 1  && _tmp3A5 == 1)return;else{
# 1554
if(((_tmp3A4 != 0  && _tmp3A4 != 1) && _tmp3A5 != 0) && _tmp3A5 != 1)return;}}
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 19: switch(*((int*)_tmp318.f2)){case 19: _LL301: _tmp3A7=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp3A6=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp318.f2)->f1;_LL302:
# 1557
 Cyc_Tcutil_unify_it(_tmp3A7,_tmp3A6);return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 18: switch(*((int*)_tmp318.f2)){case 18: _LL303: _tmp3A9=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp3A8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp318.f2)->f1;_LL304:
# 1560
 if(!Cyc_Evexp_same_const_exp(_tmp3A9,_tmp3A8)){
Cyc_Tcutil_failure_reason=({const char*_tmp336="(cannot prove expressions are the same)";_tag_dyneither(_tmp336,sizeof(char),40);});
goto _LL2EA;}
# 1564
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 8: switch(*((int*)_tmp318.f2)){case 8: _LL305: _tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp318.f1)->f1).elt_type;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp318.f1)->f1).tq;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp318.f1)->f1).num_elts;_tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp318.f1)->f1).zero_term;_tmp3AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp318.f2)->f1).elt_type;_tmp3AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp318.f2)->f1).tq;_tmp3AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp318.f2)->f1).num_elts;_tmp3AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp318.f2)->f1).zero_term;_LL306:
# 1568
 Cyc_Tcutil_unify_it(_tmp3AD,_tmp3B1);
Cyc_Tcutil_unify_tqual(_tmp3AC,_tmp3AD,_tmp3B0,_tmp3B1);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmp3AE,_tmp3AA,({const char*_tmp337="(not both zero terminated)";_tag_dyneither(_tmp337,sizeof(char),27);}));
if(_tmp3AF == _tmp3AB)return;
if(_tmp3AF == 0  || _tmp3AB == 0)goto _LL2EA;
if(Cyc_Evexp_same_const_exp(_tmp3AF,_tmp3AB))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp338="(different array sizes)";_tag_dyneither(_tmp338,sizeof(char),24);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 9: switch(*((int*)_tmp318.f2)){case 9: _LL307: _tmp3CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).tvars;_tmp3CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).effect;_tmp3C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).ret_tqual;_tmp3C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).ret_typ;_tmp3C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).args;_tmp3C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).c_varargs;_tmp3C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).cyc_varargs;_tmp3C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).rgn_po;_tmp3C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).attributes;_tmp3C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).requires_clause;_tmp3C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).requires_relns;_tmp3C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).ensures_clause;_tmp3BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f1)->f1).ensures_relns;_tmp3BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).tvars;_tmp3BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).effect;_tmp3BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).ret_tqual;_tmp3BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).ret_typ;_tmp3BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).args;_tmp3B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).c_varargs;_tmp3B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).cyc_varargs;_tmp3B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).rgn_po;_tmp3B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).attributes;_tmp3B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).requires_clause;_tmp3B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).requires_relns;_tmp3B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).ensures_clause;_tmp3B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp318.f2)->f1).ensures_relns;_LL308: {
# 1582
int done=0;
{struct _RegionHandle _tmp339=_new_region("rgn");struct _RegionHandle*rgn=& _tmp339;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp3BE != 0){
if(_tmp3CB == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp33A="(second function type has too few type variables)";_tag_dyneither(_tmp33A,sizeof(char),50);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1590
void*_tmp33B=((struct Cyc_Absyn_Tvar*)_tmp3BE->hd)->kind;
void*_tmp33C=((struct Cyc_Absyn_Tvar*)_tmp3CB->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp33B,_tmp33C)){
Cyc_Tcutil_failure_reason=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp341;_tmp341.tag=0;_tmp341.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1596
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3CB->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp340;_tmp340.tag=0;_tmp340.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1595
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3BE->hd,& Cyc_Tcutil_bk)));({struct Cyc_String_pa_PrintArg_struct _tmp33F;_tmp33F.tag=0;_tmp33F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1594
Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)_tmp3BE->hd));({void*_tmp33D[3]={& _tmp33F,& _tmp340,& _tmp341};Cyc_aprintf(({const char*_tmp33E="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp33E,sizeof(char),44);}),_tag_dyneither(_tmp33D,sizeof(void*),3));});});});});
# 1597
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1599
inst=({struct Cyc_List_List*_tmp342=_region_malloc(rgn,sizeof(*_tmp342));_tmp342->hd=({struct _tuple16*_tmp343=_region_malloc(rgn,sizeof(*_tmp343));_tmp343->f1=(struct Cyc_Absyn_Tvar*)_tmp3CB->hd;_tmp343->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp344=_cycalloc(sizeof(*_tmp344));_tmp344[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp345;_tmp345.tag=2;_tmp345.f1=(struct Cyc_Absyn_Tvar*)_tmp3BE->hd;_tmp345;});_tmp344;});_tmp343;});_tmp342->tl=inst;_tmp342;});
_tmp3BE=_tmp3BE->tl;
_tmp3CB=_tmp3CB->tl;};}
# 1603
if(_tmp3CB != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp346="(second function type has too many type variables)";_tag_dyneither(_tmp346,sizeof(char),51);});
_npop_handler(0);goto _LL2EA;}
# 1607
if(inst != 0){
Cyc_Tcutil_unify_it((void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp347=_cycalloc(sizeof(*_tmp347));_tmp347[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp348;_tmp348.tag=9;_tmp348.f1=({struct Cyc_Absyn_FnInfo _tmp349;_tmp349.tvars=0;_tmp349.effect=_tmp3BD;_tmp349.ret_tqual=_tmp3BC;_tmp349.ret_typ=_tmp3BB;_tmp349.args=_tmp3BA;_tmp349.c_varargs=_tmp3B9;_tmp349.cyc_varargs=_tmp3B8;_tmp349.rgn_po=_tmp3B7;_tmp349.attributes=_tmp3B6;_tmp349.requires_clause=_tmp3C2;_tmp349.requires_relns=_tmp3C1;_tmp349.ensures_clause=_tmp3C0;_tmp349.ensures_relns=_tmp3BF;_tmp349;});_tmp348;});_tmp347;}),
# 1611
Cyc_Tcutil_rsubstitute(rgn,inst,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp34A=_cycalloc(sizeof(*_tmp34A));_tmp34A[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp34B;_tmp34B.tag=9;_tmp34B.f1=({struct Cyc_Absyn_FnInfo _tmp34C;_tmp34C.tvars=0;_tmp34C.effect=_tmp3CA;_tmp34C.ret_tqual=_tmp3C9;_tmp34C.ret_typ=_tmp3C8;_tmp34C.args=_tmp3C7;_tmp34C.c_varargs=_tmp3C6;_tmp34C.cyc_varargs=_tmp3C5;_tmp34C.rgn_po=_tmp3C4;_tmp34C.attributes=_tmp3C3;_tmp34C.requires_clause=_tmp3B5;_tmp34C.requires_relns=_tmp3B4;_tmp34C.ensures_clause=_tmp3B3;_tmp34C.ensures_relns=_tmp3B2;_tmp34C;});_tmp34B;});_tmp34A;})));
# 1616
done=1;}}
# 1584
;_pop_region(rgn);}
# 1619
if(done)
return;
Cyc_Tcutil_unify_it(_tmp3BB,_tmp3C8);
Cyc_Tcutil_unify_tqual(_tmp3BC,_tmp3BB,_tmp3C9,_tmp3C8);
for(0;_tmp3BA != 0  && _tmp3C7 != 0;(_tmp3BA=_tmp3BA->tl,_tmp3C7=_tmp3C7->tl)){
struct _tuple10 _tmp34D=*((struct _tuple10*)_tmp3BA->hd);struct _tuple10 _tmp34E=_tmp34D;struct Cyc_Absyn_Tqual _tmp354;void*_tmp353;_LL333: _tmp354=_tmp34E.f2;_tmp353=_tmp34E.f3;_LL334:;{
struct _tuple10 _tmp34F=*((struct _tuple10*)_tmp3C7->hd);struct _tuple10 _tmp350=_tmp34F;struct Cyc_Absyn_Tqual _tmp352;void*_tmp351;_LL336: _tmp352=_tmp350.f2;_tmp351=_tmp350.f3;_LL337:;
Cyc_Tcutil_unify_it(_tmp353,_tmp351);
Cyc_Tcutil_unify_tqual(_tmp354,_tmp353,_tmp352,_tmp351);};}
# 1629
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp3BA != 0  || _tmp3C7 != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp355="(function types have different number of arguments)";_tag_dyneither(_tmp355,sizeof(char),52);});
goto _LL2EA;}
# 1635
if(_tmp3B9 != _tmp3C6){
Cyc_Tcutil_failure_reason=({const char*_tmp356="(only one function type takes C varargs)";_tag_dyneither(_tmp356,sizeof(char),41);});
goto _LL2EA;}{
# 1640
int bad_cyc_vararg=0;
{struct _tuple22 _tmp357=({struct _tuple22 _tmp363;_tmp363.f1=_tmp3B8;_tmp363.f2=_tmp3C5;_tmp363;});struct _tuple22 _tmp358=_tmp357;struct _dyneither_ptr*_tmp362;struct Cyc_Absyn_Tqual _tmp361;void*_tmp360;int _tmp35F;struct _dyneither_ptr*_tmp35E;struct Cyc_Absyn_Tqual _tmp35D;void*_tmp35C;int _tmp35B;if(_tmp358.f1 == 0){if(_tmp358.f2 == 0){_LL339: _LL33A:
 goto _LL338;}else{_LL33B: _LL33C:
 goto _LL33E;}}else{if(_tmp358.f2 == 0){_LL33D: _LL33E:
# 1645
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp359="(only one function type takes varargs)";_tag_dyneither(_tmp359,sizeof(char),39);});
goto _LL338;}else{_LL33F: _tmp362=(_tmp358.f1)->name;_tmp361=(_tmp358.f1)->tq;_tmp360=(_tmp358.f1)->type;_tmp35F=(_tmp358.f1)->inject;_tmp35E=(_tmp358.f2)->name;_tmp35D=(_tmp358.f2)->tq;_tmp35C=(_tmp358.f2)->type;_tmp35B=(_tmp358.f2)->inject;_LL340:
# 1649
 Cyc_Tcutil_unify_it(_tmp360,_tmp35C);
Cyc_Tcutil_unify_tqual(_tmp361,_tmp360,_tmp35D,_tmp35C);
if(_tmp35F != _tmp35B){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp35A="(only one function type injects varargs)";_tag_dyneither(_tmp35A,sizeof(char),41);});}
# 1655
goto _LL338;}}_LL338:;}
# 1657
if(bad_cyc_vararg)goto _LL2EA;{
# 1660
int bad_effect=0;
{struct _tuple0 _tmp364=({struct _tuple0 _tmp366;_tmp366.f1=_tmp3BD;_tmp366.f2=_tmp3CA;_tmp366;});struct _tuple0 _tmp365=_tmp364;if(_tmp365.f1 == 0){if(_tmp365.f2 == 0){_LL342: _LL343:
 goto _LL341;}else{_LL344: _LL345:
 goto _LL347;}}else{if(_tmp365.f2 == 0){_LL346: _LL347:
 bad_effect=1;goto _LL341;}else{_LL348: _LL349:
 bad_effect=!Cyc_Tcutil_unify_effect((void*)_check_null(_tmp3BD),(void*)_check_null(_tmp3CA));goto _LL341;}}_LL341:;}
# 1667
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp367="(function effects do not match)";_tag_dyneither(_tmp367,sizeof(char),32);});
goto _LL2EA;}
# 1673
if(!Cyc_Tcutil_same_atts(_tmp3C3,_tmp3B6)){
Cyc_Tcutil_failure_reason=({const char*_tmp368="(function types have different attributes)";_tag_dyneither(_tmp368,sizeof(char),43);});
goto _LL2EA;}
# 1677
if(!Cyc_Tcutil_same_rgn_po(_tmp3C4,_tmp3B7)){
Cyc_Tcutil_failure_reason=({const char*_tmp369="(function types have different region lifetime orderings)";_tag_dyneither(_tmp369,sizeof(char),58);});
goto _LL2EA;}
# 1681
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3C1,_tmp3B4)){
Cyc_Tcutil_failure_reason=({const char*_tmp36A="(@requires clauses not equivalent)";_tag_dyneither(_tmp36A,sizeof(char),35);});
goto _LL2EA;}
# 1685
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3BF,_tmp3B2)){
Cyc_Tcutil_failure_reason=({const char*_tmp36B="(@ensures clauses not equivalent)";_tag_dyneither(_tmp36B,sizeof(char),34);});
goto _LL2EA;}
# 1689
return;};};}case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 10: switch(*((int*)_tmp318.f2)){case 10: _LL309: _tmp3CD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp3CC=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp318.f2)->f1;_LL30A:
# 1692
 for(0;_tmp3CC != 0  && _tmp3CD != 0;(_tmp3CC=_tmp3CC->tl,_tmp3CD=_tmp3CD->tl)){
struct _tuple12 _tmp36C=*((struct _tuple12*)_tmp3CC->hd);struct _tuple12 _tmp36D=_tmp36C;struct Cyc_Absyn_Tqual _tmp373;void*_tmp372;_LL34B: _tmp373=_tmp36D.f1;_tmp372=_tmp36D.f2;_LL34C:;{
struct _tuple12 _tmp36E=*((struct _tuple12*)_tmp3CD->hd);struct _tuple12 _tmp36F=_tmp36E;struct Cyc_Absyn_Tqual _tmp371;void*_tmp370;_LL34E: _tmp371=_tmp36F.f1;_tmp370=_tmp36F.f2;_LL34F:;
Cyc_Tcutil_unify_it(_tmp372,_tmp370);
Cyc_Tcutil_unify_tqual(_tmp373,_tmp372,_tmp371,_tmp370);};}
# 1698
if(_tmp3CC == 0  && _tmp3CD == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp374="(tuple types have different numbers of components)";_tag_dyneither(_tmp374,sizeof(char),51);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 12: switch(*((int*)_tmp318.f2)){case 12: _LL30B: _tmp3D1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp3D0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp318.f1)->f2;_tmp3CF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp318.f2)->f1;_tmp3CE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp318.f2)->f2;_LL30C:
# 1705
 if(_tmp3CF != _tmp3D1){Cyc_Tcutil_failure_reason=({const char*_tmp375="(struct and union type)";_tag_dyneither(_tmp375,sizeof(char),24);});goto _LL2EA;}
for(0;_tmp3CE != 0  && _tmp3D0 != 0;(_tmp3CE=_tmp3CE->tl,_tmp3D0=_tmp3D0->tl)){
struct Cyc_Absyn_Aggrfield*_tmp376=(struct Cyc_Absyn_Aggrfield*)_tmp3CE->hd;
struct Cyc_Absyn_Aggrfield*_tmp377=(struct Cyc_Absyn_Aggrfield*)_tmp3D0->hd;
if(Cyc_strptrcmp(_tmp376->name,_tmp377->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp378="(different member names)";_tag_dyneither(_tmp378,sizeof(char),25);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1713
Cyc_Tcutil_unify_it(_tmp376->type,_tmp377->type);
Cyc_Tcutil_unify_tqual(_tmp376->tq,_tmp376->type,_tmp377->tq,_tmp377->type);
if(!Cyc_Tcutil_same_atts(_tmp376->attributes,_tmp377->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp379="(different attributes on member)";_tag_dyneither(_tmp379,sizeof(char),33);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1721
if((_tmp376->width != 0  && _tmp377->width == 0  || 
_tmp377->width != 0  && _tmp376->width == 0) || 
(_tmp376->width != 0  && _tmp377->width != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp376->width),(struct Cyc_Absyn_Exp*)_check_null(_tmp377->width))){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp37A="(different bitfield widths on member)";_tag_dyneither(_tmp37A,sizeof(char),38);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1730
if((_tmp376->requires_clause != 0  && _tmp377->requires_clause == 0  || 
_tmp376->requires_clause == 0  && _tmp377->requires_clause != 0) || 
(_tmp376->requires_clause == 0  && _tmp377->requires_clause != 0) && !
Cyc_Evexp_same_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp376->requires_clause),(struct Cyc_Absyn_Exp*)_check_null(_tmp377->requires_clause))){
# 1735
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp37B="(different @requires clauses on member)";_tag_dyneither(_tmp37B,sizeof(char),40);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1741
if(_tmp3CE == 0  && _tmp3D0 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp37C="(different number of members)";_tag_dyneither(_tmp37C,sizeof(char),30);});
goto _LL2EA;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 17: switch(*((int*)_tmp318.f2)){case 17: _LL30D: _tmp3D5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp318.f1)->f2;_tmp3D4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp318.f1)->f3;_tmp3D3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp318.f2)->f2;_tmp3D2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp318.f2)->f3;_LL30E:
# 1747
 if(_tmp3D4 != _tmp3D2){
Cyc_Tcutil_failure_reason=({const char*_tmp37D="(different abstract typedefs)";_tag_dyneither(_tmp37D,sizeof(char),30);});
goto _LL2EA;}
# 1751
Cyc_Tcutil_failure_reason=({const char*_tmp37E="(type parameters to typedef differ)";_tag_dyneither(_tmp37E,sizeof(char),36);});
Cyc_Tcutil_unify_list(_tmp3D5,_tmp3D3);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 20: switch(*((int*)_tmp318.f2)){case 20: _LL30F: _LL310:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 21: switch(*((int*)_tmp318.f2)){case 21: _LL311: _LL312:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 22: switch(*((int*)_tmp318.f2)){case 22: _LL313: _LL314:
 return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 15: switch(*((int*)_tmp318.f2)){case 15: _LL315: _tmp3D7=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp3D6=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp318.f2)->f1;_LL316:
# 1758
 Cyc_Tcutil_unify_it(_tmp3D7,_tmp3D6);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 16: switch(*((int*)_tmp318.f2)){case 16: _LL317: _tmp3DB=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp318.f1)->f1;_tmp3DA=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp318.f1)->f2;_tmp3D9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp318.f2)->f1;_tmp3D8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp318.f2)->f2;_LL318:
# 1761
 Cyc_Tcutil_unify_it(_tmp3DB,_tmp3D9);
Cyc_Tcutil_unify_it(_tmp3DA,_tmp3D8);
return;case 24: goto _LL31B;case 25: goto _LL321;case 23: goto _LL323;default: goto _LL325;}case 24: _LL319: _LL31A:
 goto _LL31C;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp318.f2)->tag == 24){_LL31B: _LL31C:
 goto _LL31E;}else{switch(*((int*)_tmp318.f1)){case 23: _LL31D: _LL31E:
 goto _LL320;case 25: _LL31F: _LL320:
 goto _LL322;default: switch(*((int*)_tmp318.f2)){case 25: _LL321: _LL322:
 goto _LL324;case 23: _LL323: _LL324:
# 1770
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp37F="(effects don't unify)";_tag_dyneither(_tmp37F,sizeof(char),22);});
goto _LL2EA;default: _LL325: _LL326:
 goto _LL2EA;}}}}_LL2EA:;}
# 1775
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1778
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return cmp((void*)_check_null(a1),(void*)_check_null(a2));}
# 1785
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3DD=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3DE=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3DD,_tmp3DE);}
# 1792
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
x=Cyc_Absyn_compress_conref(x);
y=Cyc_Absyn_compress_conref(y);
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3DF=x;void*_tmp3E6;switch((((union Cyc_Absyn_Constraint*)_tmp3DF)->Eq_constr).tag){case 3: _LL351: _LL352:
 return - 1;case 1: _LL353: _tmp3E6=(void*)(_tmp3DF->Eq_constr).val;_LL354: {
# 1799
union Cyc_Absyn_Constraint*_tmp3E0=y;void*_tmp3E3;switch((((union Cyc_Absyn_Constraint*)_tmp3E0)->Eq_constr).tag){case 3: _LL358: _LL359:
 return 1;case 1: _LL35A: _tmp3E3=(void*)(_tmp3E0->Eq_constr).val;_LL35B:
 return cmp(_tmp3E6,_tmp3E3);default: _LL35C: _LL35D:
({void*_tmp3E1=0;Cyc_Tcutil_impos(({const char*_tmp3E2="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3E2,sizeof(char),40);}),_tag_dyneither(_tmp3E1,sizeof(void*),0));});}_LL357:;}default: _LL355: _LL356:
# 1804
({void*_tmp3E4=0;Cyc_Tcutil_impos(({const char*_tmp3E5="unify_conref: forward after compress";_tag_dyneither(_tmp3E5,sizeof(char),37);}),_tag_dyneither(_tmp3E4,sizeof(void*),0));});}_LL350:;};}
# 1808
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp3E7=tqt1;struct Cyc_Absyn_Tqual _tmp3ED;void*_tmp3EC;_LL35F: _tmp3ED=_tmp3E7->f1;_tmp3EC=_tmp3E7->f2;_LL360:;{
struct _tuple12*_tmp3E8=tqt2;struct Cyc_Absyn_Tqual _tmp3EB;void*_tmp3EA;_LL362: _tmp3EB=_tmp3E8->f1;_tmp3EA=_tmp3E8->f2;_LL363:;{
int _tmp3E9=Cyc_Tcutil_tqual_cmp(_tmp3ED,_tmp3EB);
if(_tmp3E9 != 0)return _tmp3E9;
return Cyc_Tcutil_typecmp(_tmp3EC,_tmp3EA);};};}
# 1816
static int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp3EE=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp3EE != 0)return _tmp3EE;{
int _tmp3EF=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp3EF != 0)return _tmp3EF;{
int _tmp3F0=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp3F0 != 0)return _tmp3F0;{
int _tmp3F1=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp3F1 != 0)return _tmp3F1;
_tmp3F1=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp3F1 != 0)return _tmp3F1;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1830
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp3F2=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp3F2 != 0)return _tmp3F2;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1836
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp3F3=t;switch(*((int*)_tmp3F3)){case 0: _LL365: _LL366:
 return 0;case 1: _LL367: _LL368:
 return 1;case 2: _LL369: _LL36A:
 return 2;case 3: _LL36B: _LL36C:
 return 3;case 4: _LL36D: _LL36E:
 return 4;case 5: _LL36F: _LL370:
 return 5;case 6: _LL371: _LL372:
 return 6;case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F3)->f1 == 0){_LL373: _LL374:
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
# 1873
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp3F4=Cyc_Core_intcmp(Cyc_Tcutil_type_case_number(t1),Cyc_Tcutil_type_case_number(t2));
if(_tmp3F4 != 0)
return _tmp3F4;{
# 1882
struct _tuple0 _tmp3F5=({struct _tuple0 _tmp458;_tmp458.f1=t2;_tmp458.f2=t1;_tmp458;});struct _tuple0 _tmp3F6=_tmp3F5;struct _dyneither_ptr _tmp457;struct _dyneither_ptr _tmp456;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Exp*_tmp454;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_Absyn_Exp*_tmp452;void*_tmp451;void*_tmp450;void*_tmp44F;void*_tmp44E;void*_tmp44D;void*_tmp44C;void*_tmp44B;void*_tmp44A;enum Cyc_Absyn_AggrKind _tmp449;struct Cyc_List_List*_tmp448;enum Cyc_Absyn_AggrKind _tmp447;struct Cyc_List_List*_tmp446;struct Cyc_List_List*_tmp445;struct Cyc_List_List*_tmp444;void*_tmp443;struct Cyc_Absyn_Tqual _tmp442;struct Cyc_Absyn_Exp*_tmp441;union Cyc_Absyn_Constraint*_tmp440;void*_tmp43F;struct Cyc_Absyn_Tqual _tmp43E;struct Cyc_Absyn_Exp*_tmp43D;union Cyc_Absyn_Constraint*_tmp43C;int _tmp43B;int _tmp43A;enum Cyc_Absyn_Sign _tmp439;enum Cyc_Absyn_Size_of _tmp438;enum Cyc_Absyn_Sign _tmp437;enum Cyc_Absyn_Size_of _tmp436;void*_tmp435;struct Cyc_Absyn_Tqual _tmp434;void*_tmp433;union Cyc_Absyn_Constraint*_tmp432;union Cyc_Absyn_Constraint*_tmp431;union Cyc_Absyn_Constraint*_tmp430;void*_tmp42F;struct Cyc_Absyn_Tqual _tmp42E;void*_tmp42D;union Cyc_Absyn_Constraint*_tmp42C;union Cyc_Absyn_Constraint*_tmp42B;union Cyc_Absyn_Constraint*_tmp42A;struct Cyc_Absyn_Datatypedecl*_tmp429;struct Cyc_Absyn_Datatypefield*_tmp428;struct Cyc_List_List*_tmp427;struct Cyc_Absyn_Datatypedecl*_tmp426;struct Cyc_Absyn_Datatypefield*_tmp425;struct Cyc_List_List*_tmp424;struct Cyc_Absyn_Datatypedecl*_tmp423;struct Cyc_List_List*_tmp422;struct Cyc_Absyn_Datatypedecl*_tmp421;struct Cyc_List_List*_tmp420;struct Cyc_List_List*_tmp41F;struct Cyc_List_List*_tmp41E;struct _tuple2*_tmp41D;struct _tuple2*_tmp41C;union Cyc_Absyn_AggrInfoU _tmp41B;struct Cyc_List_List*_tmp41A;union Cyc_Absyn_AggrInfoU _tmp419;struct Cyc_List_List*_tmp418;struct Cyc_Absyn_Tvar*_tmp417;struct Cyc_Absyn_Tvar*_tmp416;switch(*((int*)_tmp3F6.f1)){case 1: switch(*((int*)_tmp3F6.f2)){case 1: _LL3A2: _LL3A3:
# 1884
({void*_tmp3F7=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp3F8="typecmp: can only compare closed types";_tag_dyneither(_tmp3F8,sizeof(char),39);}),_tag_dyneither(_tmp3F7,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 2: switch(*((int*)_tmp3F6.f2)){case 2: _LL3A4: _tmp417=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp416=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3A5:
# 1888
 return Cyc_Core_intcmp(_tmp416->identity,_tmp417->identity);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 11: switch(*((int*)_tmp3F6.f2)){case 11: _LL3A6: _tmp41B=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F6.f1)->f1).aggr_info;_tmp41A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F6.f1)->f1).targs;_tmp419=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F6.f2)->f1).aggr_info;_tmp418=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3F6.f2)->f1).targs;_LL3A7: {
# 1892
struct _tuple13 _tmp3F9=Cyc_Absyn_aggr_kinded_name(_tmp41B);struct _tuple13 _tmp3FA=_tmp3F9;struct _tuple2*_tmp3FF;_LL3D9: _tmp3FF=_tmp3FA.f2;_LL3DA:;{
struct _tuple13 _tmp3FB=Cyc_Absyn_aggr_kinded_name(_tmp419);struct _tuple13 _tmp3FC=_tmp3FB;struct _tuple2*_tmp3FE;_LL3DC: _tmp3FE=_tmp3FC.f2;_LL3DD:;{
int _tmp3FD=Cyc_Absyn_qvar_cmp(_tmp3FF,_tmp3FE);
if(_tmp3FD != 0)return _tmp3FD;else{
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp41A,_tmp418);}};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 13: switch(*((int*)_tmp3F6.f2)){case 13: _LL3A8: _tmp41D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp41C=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3A9:
# 1899
 return Cyc_Absyn_qvar_cmp(_tmp41D,_tmp41C);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 14: switch(*((int*)_tmp3F6.f2)){case 14: _LL3AA: _tmp41F=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp41E=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3AB:
# 1902
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp41F,_tmp41E);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F6.f1)->f1).datatype_info).KnownDatatype).tag == 2)switch(*((int*)_tmp3F6.f2)){case 3: if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F6.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3AC: _tmp423=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F6.f1)->f1).datatype_info).KnownDatatype).val;_tmp422=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F6.f1)->f1).targs;_tmp421=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F6.f2)->f1).datatype_info).KnownDatatype).val;_tmp420=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp3F6.f2)->f1).targs;_LL3AD:
# 1906
 if(_tmp421 == _tmp423)return 0;{
int _tmp400=Cyc_Absyn_qvar_cmp(_tmp421->name,_tmp423->name);
if(_tmp400 != 0)return _tmp400;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp420,_tmp422);};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp3F6.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F6.f1)->f1).field_info).KnownDatatypefield).tag == 2)switch(*((int*)_tmp3F6.f2)){case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F6.f2)->f1).field_info).KnownDatatypefield).tag == 2){_LL3AE: _tmp429=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F6.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp428=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F6.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp427=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F6.f1)->f1).targs;_tmp426=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F6.f2)->f1).field_info).KnownDatatypefield).val).f1;_tmp425=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F6.f2)->f1).field_info).KnownDatatypefield).val).f2;_tmp424=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3F6.f2)->f1).targs;_LL3AF:
# 1913
 if(_tmp426 == _tmp429)return 0;{
int _tmp401=Cyc_Absyn_qvar_cmp(_tmp429->name,_tmp426->name);
if(_tmp401 != 0)return _tmp401;{
int _tmp402=Cyc_Absyn_qvar_cmp(_tmp428->name,_tmp425->name);
if(_tmp402 != 0)return _tmp402;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp424,_tmp427);};};}else{goto _LL3D6;}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}else{switch(*((int*)_tmp3F6.f2)){case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}}case 5: switch(*((int*)_tmp3F6.f2)){case 5: _LL3B0: _tmp435=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f1)->f1).elt_typ;_tmp434=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f1)->f1).elt_tq;_tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f1)->f1).ptr_atts).rgn;_tmp432=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f1)->f1).ptr_atts).nullable;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f1)->f1).ptr_atts).bounds;_tmp430=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f1)->f1).ptr_atts).zero_term;_tmp42F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f2)->f1).elt_typ;_tmp42E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f2)->f1).elt_tq;_tmp42D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f2)->f1).ptr_atts).rgn;_tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f2)->f1).ptr_atts).nullable;_tmp42B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f2)->f1).ptr_atts).bounds;_tmp42A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F6.f2)->f1).ptr_atts).zero_term;_LL3B1: {
# 1922
int _tmp403=Cyc_Tcutil_typecmp(_tmp42F,_tmp435);
if(_tmp403 != 0)return _tmp403;{
int _tmp404=Cyc_Tcutil_typecmp(_tmp42D,_tmp433);
if(_tmp404 != 0)return _tmp404;{
int _tmp405=Cyc_Tcutil_tqual_cmp(_tmp42E,_tmp434);
if(_tmp405 != 0)return _tmp405;{
int _tmp406=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Tcutil_boundscmp,_tmp42B,_tmp431);
if(_tmp406 != 0)return _tmp406;{
int _tmp407=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp42A,_tmp430);
if(_tmp407 != 0)return _tmp407;
{void*_tmp408=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp42B);void*_tmp409=_tmp408;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp409)->tag == 0){_LL3DF: _LL3E0:
 return 0;}else{_LL3E1: _LL3E2:
 goto _LL3DE;}_LL3DE:;}
# 1936
return((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp42C,_tmp432);};};};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 6: switch(*((int*)_tmp3F6.f2)){case 6: _LL3B2: _tmp439=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp438=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F6.f1)->f2;_tmp437=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_tmp436=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp3F6.f2)->f2;_LL3B3:
# 1939
 if(_tmp437 != _tmp439)return Cyc_Core_intcmp((int)((unsigned int)_tmp437),(int)((unsigned int)_tmp439));
if(_tmp436 != _tmp438)return Cyc_Core_intcmp((int)((unsigned int)_tmp436),(int)((unsigned int)_tmp438));
return 0;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 7: switch(*((int*)_tmp3F6.f2)){case 7: _LL3B4: _tmp43B=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp43A=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3B5:
# 1944
 if(_tmp43B == _tmp43A)return 0;else{
if(_tmp43A == 0)return - 1;else{
if(_tmp43A == 1  && _tmp43B == 0)return - 1;else{
return 1;}}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 8: switch(*((int*)_tmp3F6.f2)){case 8: _LL3B6: _tmp443=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F6.f1)->f1).elt_type;_tmp442=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F6.f1)->f1).tq;_tmp441=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F6.f1)->f1).num_elts;_tmp440=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F6.f1)->f1).zero_term;_tmp43F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F6.f2)->f1).elt_type;_tmp43E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F6.f2)->f1).tq;_tmp43D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F6.f2)->f1).num_elts;_tmp43C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F6.f2)->f1).zero_term;_LL3B7: {
# 1951
int _tmp40A=Cyc_Tcutil_tqual_cmp(_tmp43E,_tmp442);
if(_tmp40A != 0)return _tmp40A;{
int _tmp40B=Cyc_Tcutil_typecmp(_tmp43F,_tmp443);
if(_tmp40B != 0)return _tmp40B;{
int _tmp40C=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_conrefs_cmp)(Cyc_Core_intcmp,_tmp440,_tmp43C);
if(_tmp40C != 0)return _tmp40C;
if(_tmp441 == _tmp43D)return 0;
if(_tmp441 == 0  || _tmp43D == 0)
({void*_tmp40D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp40E="missing expression in array index";_tag_dyneither(_tmp40E,sizeof(char),34);}),_tag_dyneither(_tmp40D,sizeof(void*),0));});
# 1961
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp441,_tmp43D);};};}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 9: switch(*((int*)_tmp3F6.f2)){case 9: _LL3B8: _LL3B9:
# 1964
({void*_tmp40F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp410="typecmp: function types not handled";_tag_dyneither(_tmp410,sizeof(char),36);}),_tag_dyneither(_tmp40F,sizeof(void*),0));});case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 10: switch(*((int*)_tmp3F6.f2)){case 10: _LL3BA: _tmp445=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp444=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3BB:
# 1967
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp444,_tmp445);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 12: switch(*((int*)_tmp3F6.f2)){case 12: _LL3BC: _tmp449=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp448=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F6.f1)->f2;_tmp447=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_tmp446=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F6.f2)->f2;_LL3BD:
# 1970
 if(_tmp447 != _tmp449){
if(_tmp447 == Cyc_Absyn_StructA)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp446,_tmp448);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 15: switch(*((int*)_tmp3F6.f2)){case 15: _LL3BE: _tmp44B=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp44A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3BF:
# 1975
 return Cyc_Tcutil_typecmp(_tmp44B,_tmp44A);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 16: switch(*((int*)_tmp3F6.f2)){case 16: _LL3C0: _tmp44F=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp44E=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F6.f1)->f2;_tmp44D=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_tmp44C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp3F6.f2)->f2;_LL3C1: {
# 1977
int _tmp411=Cyc_Tcutil_typecmp(_tmp44F,_tmp44D);
if(_tmp411 != 0)return _tmp411;else{
return Cyc_Tcutil_typecmp(_tmp44E,_tmp44C);}}case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 19: switch(*((int*)_tmp3F6.f2)){case 19: _LL3C2: _tmp451=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp450=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3C3:
 return Cyc_Tcutil_typecmp(_tmp451,_tmp450);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 18: switch(*((int*)_tmp3F6.f2)){case 18: _LL3C4: _tmp453=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp452=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3C5:
 _tmp455=_tmp453;_tmp454=_tmp452;goto _LL3C7;case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 27: switch(*((int*)_tmp3F6.f2)){case 27: _LL3C6: _tmp455=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp454=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3C7:
# 1983
 return Cyc_Evexp_const_exp_cmp(_tmp455,_tmp454);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 28: switch(*((int*)_tmp3F6.f2)){case 28: _LL3C8: _tmp457=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp3F6.f1)->f1;_tmp456=((struct Cyc_Absyn_BuiltinType_Absyn_Type_struct*)_tmp3F6.f2)->f1;_LL3C9:
# 1985
 return Cyc_strcmp((struct _dyneither_ptr)_tmp457,(struct _dyneither_ptr)_tmp456);case 24: goto _LL3CC;case 25: goto _LL3D2;case 23: goto _LL3D4;default: goto _LL3D6;}case 24: _LL3CA: _LL3CB:
 goto _LL3CD;default: if(((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp3F6.f2)->tag == 24){_LL3CC: _LL3CD:
 goto _LL3CF;}else{switch(*((int*)_tmp3F6.f1)){case 23: _LL3CE: _LL3CF:
 goto _LL3D1;case 25: _LL3D0: _LL3D1:
 goto _LL3D3;default: switch(*((int*)_tmp3F6.f2)){case 25: _LL3D2: _LL3D3:
 goto _LL3D5;case 23: _LL3D4: _LL3D5:
({void*_tmp412=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp413="typecmp: effects not handled";_tag_dyneither(_tmp413,sizeof(char),29);}),_tag_dyneither(_tmp412,sizeof(void*),0));});default: _LL3D6: _LL3D7:
({void*_tmp414=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp415="Unmatched case in typecmp";_tag_dyneither(_tmp415,sizeof(char),26);}),_tag_dyneither(_tmp414,sizeof(void*),0));});}}}}_LL3A1:;};};}
# 1996
int Cyc_Tcutil_is_arithmetic_type(void*t){
void*_tmp459=Cyc_Tcutil_compress(t);void*_tmp45A=_tmp459;switch(*((int*)_tmp45A)){case 6: _LL3E4: _LL3E5:
 goto _LL3E7;case 7: _LL3E6: _LL3E7:
 goto _LL3E9;case 13: _LL3E8: _LL3E9:
 goto _LL3EB;case 14: _LL3EA: _LL3EB:
 return 1;default: _LL3EC: _LL3ED:
 return 0;}_LL3E3:;}
# 2008
int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp45B=({struct _tuple0 _tmp45F;_tmp45F.f1=t1;_tmp45F.f2=t2;_tmp45F;});struct _tuple0 _tmp45C=_tmp45B;int _tmp45E;int _tmp45D;switch(*((int*)_tmp45C.f1)){case 7: switch(*((int*)_tmp45C.f2)){case 7: _LL3EF: _tmp45E=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45C.f1)->f1;_tmp45D=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45C.f2)->f1;_LL3F0:
# 2013
 return _tmp45D == 0  && _tmp45E != 0  || (_tmp45D == 1  && _tmp45E != 0) && _tmp45E != 1;case 6: _LL3F1: _LL3F2:
 goto _LL3F4;case 19: _LL3F3: _LL3F4:
 return 1;default: goto _LL40F;}case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL3F5: _LL3F6:
 return 0;}else{goto _LL3F7;}}else{_LL3F7: _LL3F8:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp45C.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f2)->f2){case Cyc_Absyn_Int_sz: _LL3F9: _LL3FA:
# 2020
 goto _LL3FC;case Cyc_Absyn_Short_sz: _LL401: _LL402:
# 2025
 goto _LL404;case Cyc_Absyn_Char_sz: _LL407: _LL408:
# 2028
 goto _LL40A;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45C.f2)->f1 == 0){_LL3FD: _LL3FE:
# 2023
 goto _LL400;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp45C.f2)){case 6: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f2)->f2){case Cyc_Absyn_Long_sz: _LL3FB: _LL3FC:
# 2021
 return 0;case Cyc_Absyn_Short_sz: _LL403: _LL404:
# 2026
 goto _LL406;case Cyc_Absyn_Char_sz: _LL409: _LL40A:
# 2029
 goto _LL40C;default: goto _LL40F;}case 7: if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp45C.f2)->f1 == 0){_LL3FF: _LL400:
# 2024
 goto _LL402;}else{goto _LL40F;}default: goto _LL40F;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f2)->f2 == Cyc_Absyn_Char_sz){_LL40B: _LL40C:
# 2030
 goto _LL40E;}else{goto _LL40F;}}else{goto _LL40F;}default: goto _LL40F;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f2)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp45C.f2)->f2){case Cyc_Absyn_Short_sz: _LL405: _LL406:
# 2027
 goto _LL408;case Cyc_Absyn_Char_sz: _LL40D: _LL40E:
# 2031
 return 1;default: goto _LL40F;}else{goto _LL40F;}default: _LL40F: _LL410:
# 2033
 return 0;}_LL3EE:;};}
# 2039
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2042
struct _RegionHandle _tmp460=_new_region("r");struct _RegionHandle*r=& _tmp460;_push_region(r);{
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0  || 
Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp461=_region_malloc(r,sizeof(*_tmp461));_tmp461->v=t1;_tmp461;});}}}
# 2052
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v)){
int _tmp462=0;_npop_handler(0);return _tmp462;}}}
# 2057
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp466;_tmp466.tag=0;_tmp466.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp465;_tmp465.tag=0;_tmp465.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp463[2]={& _tmp465,& _tmp466};Cyc_Tcutil_terr(((struct Cyc_Absyn_Exp*)el->hd)->loc,({const char*_tmp464="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp464,sizeof(char),41);}),_tag_dyneither(_tmp463,sizeof(void*),2));});});});{
int _tmp467=0;_npop_handler(0);return _tmp467;};}}}{
# 2063
int _tmp468=1;_npop_handler(0);return _tmp468;};
# 2042
;_pop_region(r);}
# 2068
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_typ(te,e)){
void*_tmp469=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp46A=_tmp469;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp46A)->tag == 5){_LL412: _LL413:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_Other_coercion);goto _LL411;}else{_LL414: _LL415:
 return 0;}_LL411:;}
# 2074
return 1;}
# 2077
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmp46B=Cyc_Tcutil_compress(t);void*_tmp46C=_tmp46B;switch(*((int*)_tmp46C)){case 6: _LL417: _LL418:
 goto _LL41A;case 19: _LL419: _LL41A:
 goto _LL41C;case 13: _LL41B: _LL41C:
 goto _LL41E;case 14: _LL41D: _LL41E:
 return 1;default: _LL41F: _LL420:
 return 0;}_LL416:;}
# 2088
int Cyc_Tcutil_coerce_uint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
return 1;
# 2092
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_typ))
({void*_tmp46D=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp46E="integral size mismatch; conversion supplied";_tag_dyneither(_tmp46E,sizeof(char),44);}),_tag_dyneither(_tmp46D,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2098
return 0;}
# 2102
int Cyc_Tcutil_coerce_sint_typ(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
return 1;
# 2106
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_typ))
({void*_tmp46F=0;Cyc_Tcutil_warn(e->loc,({const char*_tmp470="integral size mismatch; conversion supplied";_tag_dyneither(_tmp470,sizeof(char),44);}),_tag_dyneither(_tmp46F,sizeof(void*),0));});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2112
return 0;}
# 2117
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2125
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2127
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp471=({struct _tuple0 _tmp48A;_tmp48A.f1=t1;_tmp48A.f2=t2;_tmp48A;});struct _tuple0 _tmp472=_tmp471;void*_tmp489;struct Cyc_Absyn_Tqual _tmp488;struct Cyc_Absyn_Exp*_tmp487;union Cyc_Absyn_Constraint*_tmp486;void*_tmp485;struct Cyc_Absyn_Tqual _tmp484;struct Cyc_Absyn_Exp*_tmp483;union Cyc_Absyn_Constraint*_tmp482;struct Cyc_Absyn_PtrInfo _tmp481;struct Cyc_Absyn_PtrInfo _tmp480;switch(*((int*)_tmp472.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp472.f2)->tag == 5){_LL422: _tmp481=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp472.f1)->f1;_tmp480=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp472.f2)->f1;_LL423: {
# 2131
int okay=1;
# 2133
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp481.ptr_atts).nullable,(_tmp480.ptr_atts).nullable))
# 2136
okay=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp481.ptr_atts).nullable);
# 2138
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp481.ptr_atts).bounds,(_tmp480.ptr_atts).bounds)){
# 2141
struct _tuple0 _tmp473=({struct _tuple0 _tmp47A;_tmp47A.f1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp481.ptr_atts).bounds);_tmp47A.f2=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp480.ptr_atts).bounds);_tmp47A;});
# 2141
struct _tuple0 _tmp474=_tmp473;struct Cyc_Absyn_Exp*_tmp479;struct Cyc_Absyn_Exp*_tmp478;struct Cyc_Absyn_Exp*_tmp477;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp474.f1)->tag == 1){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp474.f2)->tag == 0){_LL42B: _LL42C:
# 2143
 goto _LL42E;}else{_LL42F: _tmp478=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp474.f1)->f1;_tmp477=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp474.f2)->f1;_LL430:
# 2146
 okay=okay  && Cyc_Evexp_lte_const_exp(_tmp477,_tmp478);
# 2150
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp480.ptr_atts).zero_term))
({void*_tmp475=0;Cyc_Tcutil_warn(loc,({const char*_tmp476="implicit cast to shorter array";_tag_dyneither(_tmp476,sizeof(char),31);}),_tag_dyneither(_tmp475,sizeof(void*),0));});
goto _LL42A;}}else{if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp474.f2)->tag == 0){_LL42D: _LL42E:
# 2144
 okay=1;goto _LL42A;}else{_LL431: _tmp479=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp474.f2)->f1;_LL432:
# 2155
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp481.ptr_atts).zero_term) && 
Cyc_Tcutil_is_bound_one((_tmp480.ptr_atts).bounds))
goto _LL42A;
okay=0;
goto _LL42A;}}_LL42A:;}
# 2164
okay=okay  && (!(_tmp481.elt_tq).real_const  || (_tmp480.elt_tq).real_const);
# 2167
if(!Cyc_Tcutil_unify((_tmp481.ptr_atts).rgn,(_tmp480.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp481.ptr_atts).rgn,(_tmp480.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp47E;_tmp47E.tag=0;_tmp47E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2172
Cyc_Absynpp_typ2string((_tmp480.ptr_atts).rgn));({struct Cyc_String_pa_PrintArg_struct _tmp47D;_tmp47D.tag=0;_tmp47D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2171
Cyc_Absynpp_typ2string((_tmp481.ptr_atts).rgn));({void*_tmp47B[2]={& _tmp47D,& _tmp47E};Cyc_Tcutil_warn(loc,({const char*_tmp47C="implicit cast from region %s to region %s";_tag_dyneither(_tmp47C,sizeof(char),42);}),_tag_dyneither(_tmp47B,sizeof(void*),2));});});});}else{
# 2173
okay=0;}}
# 2177
okay=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp481.ptr_atts).zero_term,(_tmp480.ptr_atts).zero_term) || 
# 2180
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp481.ptr_atts).zero_term) && (_tmp480.elt_tq).real_const);{
# 2188
int _tmp47F=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp480.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2191
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp480.ptr_atts).zero_term);
# 2195
okay=okay  && (Cyc_Tcutil_unify(_tmp481.elt_typ,_tmp480.elt_typ) || 
(_tmp47F  && ((_tmp480.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp480.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp481.elt_typ,_tmp480.elt_typ));
# 2198
return okay;};}}else{goto _LL428;}case 8: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f2)->tag == 8){_LL424: _tmp489=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f1)->f1).elt_type;_tmp488=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f1)->f1).tq;_tmp487=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f1)->f1).num_elts;_tmp486=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f1)->f1).zero_term;_tmp485=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f2)->f1).elt_type;_tmp484=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f2)->f1).tq;_tmp483=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f2)->f1).num_elts;_tmp482=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp472.f2)->f1).zero_term;_LL425: {
# 2202
int okay;
# 2204
okay=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp486,_tmp482) && (
(_tmp487 != 0  && _tmp483 != 0) && Cyc_Evexp_same_const_exp(_tmp487,_tmp483));
# 2207
return(okay  && Cyc_Tcutil_unify(_tmp489,_tmp485)) && (!_tmp488.real_const  || _tmp484.real_const);}}else{goto _LL428;}case 19: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp472.f2)->tag == 6){_LL426: _LL427:
# 2209
 return 0;}else{goto _LL428;}default: _LL428: _LL429:
# 2211
 return Cyc_Tcutil_unify(t1,t2);}_LL421:;};}
# 2215
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp48B=Cyc_Tcutil_compress(t);void*_tmp48C=_tmp48B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48C)->tag == 5){_LL434: _LL435:
 return 1;}else{_LL436: _LL437:
 return 0;}_LL433:;}
# 2221
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp48D=Cyc_Tcutil_compress(t);void*_tmp48E=_tmp48D;void*_tmp491;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E)->tag == 5){_LL439: _tmp491=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48E)->f1).elt_typ;_LL43A:
 return _tmp491;}else{_LL43B: _LL43C:
({void*_tmp48F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp490="pointer_elt_type";_tag_dyneither(_tmp490,sizeof(char),17);}),_tag_dyneither(_tmp48F,sizeof(void*),0));});}_LL438:;}
# 2227
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp492=Cyc_Tcutil_compress(t);void*_tmp493=_tmp492;struct Cyc_Absyn_PtrAtts*_tmp496;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp493)->tag == 5){_LL43E: _tmp496=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp493)->f1).ptr_atts;_LL43F:
 return _tmp496->rgn;}else{_LL440: _LL441:
({void*_tmp494=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp495="pointer_elt_type";_tag_dyneither(_tmp495,sizeof(char),17);}),_tag_dyneither(_tmp494,sizeof(void*),0));});}_LL43D:;}
# 2234
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp497=Cyc_Tcutil_compress(t);void*_tmp498=_tmp497;void*_tmp499;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp498)->tag == 5){_LL443: _tmp499=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp498)->f1).ptr_atts).rgn;_LL444:
# 2237
*rgn=_tmp499;
return 1;}else{_LL445: _LL446:
 return 0;}_LL442:;}
# 2244
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2248
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp49A=Cyc_Tcutil_compress(t);void*_tmp49B=_tmp49A;union Cyc_Absyn_Constraint*_tmp49C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49B)->tag == 5){_LL448: _tmp49C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp49B)->f1).ptr_atts).bounds;_LL449:
# 2251
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp49C)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL44A: _LL44B:
 return 0;}_LL447:;}
# 2257
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp49D=e->r;void*_tmp49E=_tmp49D;void*_tmp4A2;struct Cyc_Absyn_Exp*_tmp4A1;struct _dyneither_ptr _tmp4A0;switch(*((int*)_tmp49E)){case 0: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49E)->f1).Wchar_c).tag){case 5: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49E)->f1).Int_c).val).f2 == 0){_LL44D: _LL44E:
 goto _LL450;}else{goto _LL459;}case 2: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49E)->f1).Char_c).val).f2 == 0){_LL44F: _LL450:
 goto _LL452;}else{goto _LL459;}case 4: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49E)->f1).Short_c).val).f2 == 0){_LL451: _LL452:
 goto _LL454;}else{goto _LL459;}case 6: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49E)->f1).LongLong_c).val).f2 == 0){_LL453: _LL454:
# 2263
 return 1;}else{goto _LL459;}case 3: _LL455: _tmp4A0=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp49E)->f1).Wchar_c).val;_LL456: {
# 2265
unsigned long _tmp49F=Cyc_strlen((struct _dyneither_ptr)_tmp4A0);
int i=0;
if(_tmp49F >= 2  && *((const char*)_check_dyneither_subscript(_tmp4A0,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp4A0,sizeof(char),1))== '0')i=2;else{
if((*((const char*)_check_dyneither_subscript(_tmp4A0,sizeof(char),1))== 'x'  && _tmp49F >= 3) && *((const char*)_check_dyneither_subscript(_tmp4A0,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp49F;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp4A0,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2275
return 0;}}default: goto _LL459;}case 13: _LL457: _tmp4A2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp49E)->f1;_tmp4A1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp49E)->f2;_LL458:
 return Cyc_Tcutil_is_zero(_tmp4A1) && Cyc_Tcutil_admits_zero(_tmp4A2);default: _LL459: _LL45A:
 return 0;}_LL44C:;}
# 2281
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
# 2288
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2293
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2298
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
# 2305
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2310
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2315
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp4A3=ka;enum Cyc_Absyn_KindQual _tmp4AC;enum Cyc_Absyn_AliasQual _tmp4AB;_LL45C: _tmp4AC=_tmp4A3->kind;_tmp4AB=_tmp4A3->aliasqual;_LL45D:;
{enum Cyc_Absyn_AliasQual _tmp4A4=_tmp4AB;switch(_tmp4A4){case Cyc_Absyn_Aliasable: _LL45F: _LL460: {
# 2319
enum Cyc_Absyn_KindQual _tmp4A5=_tmp4AC;switch(_tmp4A5){case Cyc_Absyn_AnyKind: _LL466: _LL467:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LL468: _LL469:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL46A: _LL46B:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL46C: _LL46D:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL46E: _LL46F:
 return& Cyc_Tcutil_eko;default: _LL470: _LL471:
 return& Cyc_Tcutil_iko;}_LL465:;}case Cyc_Absyn_Unique: _LL461: _LL462:
# 2328
{enum Cyc_Absyn_KindQual _tmp4A6=_tmp4AC;switch(_tmp4A6){case Cyc_Absyn_AnyKind: _LL473: _LL474:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL475: _LL476:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL477: _LL478:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL479: _LL47A:
 return& Cyc_Tcutil_urko;default: _LL47B: _LL47C:
 goto _LL472;}_LL472:;}
# 2335
goto _LL45E;default: _LL463: _LL464:
# 2337
{enum Cyc_Absyn_KindQual _tmp4A7=_tmp4AC;switch(_tmp4A7){case Cyc_Absyn_AnyKind: _LL47E: _LL47F:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL480: _LL481:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL482: _LL483:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL484: _LL485:
 return& Cyc_Tcutil_trko;default: _LL486: _LL487:
 goto _LL47D;}_LL47D:;}
# 2344
goto _LL45E;}_LL45E:;}
# 2346
({struct Cyc_String_pa_PrintArg_struct _tmp4AA;_tmp4AA.tag=0;_tmp4AA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4A8[1]={& _tmp4AA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4A9="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4A9,sizeof(char),26);}),_tag_dyneither(_tmp4A8,sizeof(void*),1));});});}
# 2349
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0,& Cyc_Tcutil_ik};
# 2357
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2364
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0,& Cyc_Tcutil_trk};
# 2369
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2374
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0,& Cyc_Tcutil_urk};
# 2379
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2384
struct Cyc_Absyn_Kind*_tmp4AD=ka;enum Cyc_Absyn_KindQual _tmp4B6;enum Cyc_Absyn_AliasQual _tmp4B5;_LL489: _tmp4B6=_tmp4AD->kind;_tmp4B5=_tmp4AD->aliasqual;_LL48A:;
{enum Cyc_Absyn_AliasQual _tmp4AE=_tmp4B5;switch(_tmp4AE){case Cyc_Absyn_Aliasable: _LL48C: _LL48D: {
# 2387
enum Cyc_Absyn_KindQual _tmp4AF=_tmp4B6;switch(_tmp4AF){case Cyc_Absyn_AnyKind: _LL493: _LL494:
 return& ab;case Cyc_Absyn_MemKind: _LL495: _LL496:
 return& mb;case Cyc_Absyn_BoxKind: _LL497: _LL498:
 return& bb;case Cyc_Absyn_RgnKind: _LL499: _LL49A:
 return& rb;case Cyc_Absyn_EffKind: _LL49B: _LL49C:
 return& eb;default: _LL49D: _LL49E:
 return& ib;}_LL492:;}case Cyc_Absyn_Unique: _LL48E: _LL48F:
# 2396
{enum Cyc_Absyn_KindQual _tmp4B0=_tmp4B6;switch(_tmp4B0){case Cyc_Absyn_AnyKind: _LL4A0: _LL4A1:
 return& uab;case Cyc_Absyn_MemKind: _LL4A2: _LL4A3:
 return& umb;case Cyc_Absyn_BoxKind: _LL4A4: _LL4A5:
 return& ubb;case Cyc_Absyn_RgnKind: _LL4A6: _LL4A7:
 return& urb;default: _LL4A8: _LL4A9:
 goto _LL49F;}_LL49F:;}
# 2403
goto _LL48B;default: _LL490: _LL491:
# 2405
{enum Cyc_Absyn_KindQual _tmp4B1=_tmp4B6;switch(_tmp4B1){case Cyc_Absyn_AnyKind: _LL4AB: _LL4AC:
 return& tab;case Cyc_Absyn_MemKind: _LL4AD: _LL4AE:
 return& tmb;case Cyc_Absyn_BoxKind: _LL4AF: _LL4B0:
 return& tbb;case Cyc_Absyn_RgnKind: _LL4B1: _LL4B2:
 return& trb;default: _LL4B3: _LL4B4:
 goto _LL4AA;}_LL4AA:;}
# 2412
goto _LL48B;}_LL48B:;}
# 2414
({struct Cyc_String_pa_PrintArg_struct _tmp4B4;_tmp4B4.tag=0;_tmp4B4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));({void*_tmp4B2[1]={& _tmp4B4};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4B3="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4B3,sizeof(char),24);}),_tag_dyneither(_tmp4B2,sizeof(void*),1));});});}
# 2417
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2420
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2422
return({struct Cyc_Core_Opt*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->v=Cyc_Tcutil_kind_to_bound(k);_tmp4C5;});}
# 2427
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
e1->r=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6[0]=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4C7;_tmp4C7.tag=0;_tmp4C7.f1=Cyc_Absyn_Null_c;_tmp4C7;});_tmp4C6;});{
struct Cyc_Core_Opt*_tmp4C8=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C9=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4CE=_cycalloc(sizeof(*_tmp4CE));_tmp4CE[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4CF;_tmp4CF.tag=5;_tmp4CF.f1=({struct Cyc_Absyn_PtrInfo _tmp4D0;_tmp4D0.elt_typ=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4C8);_tmp4D0.elt_tq=
Cyc_Absyn_empty_tqual(0);_tmp4D0.ptr_atts=({(_tmp4D0.ptr_atts).rgn=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4C8);(_tmp4D0.ptr_atts).nullable=Cyc_Absyn_true_conref;(_tmp4D0.ptr_atts).bounds=
# 2435
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4D0.ptr_atts).zero_term=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4D0.ptr_atts).ptrloc=0;_tmp4D0.ptr_atts;});_tmp4D0;});_tmp4CF;});_tmp4CE;});
e1->topt=(void*)_tmp4C9;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4CD;_tmp4CD.tag=0;_tmp4CD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));({struct Cyc_String_pa_PrintArg_struct _tmp4CC;_tmp4CC.tag=0;_tmp4CC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmp4CA[2]={& _tmp4CC,& _tmp4CD};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4CB="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4CB,sizeof(char),56);}),_tag_dyneither(_tmp4CA,sizeof(void*),2));});});});
return retv;};};}
# 2445
return 0;}
# 2448
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4D1=c;switch(_tmp4D1){case Cyc_Absyn_Unknown_coercion: _LL4B6: _LL4B7:
 return({const char*_tmp4D2="unknown";_tag_dyneither(_tmp4D2,sizeof(char),8);});case Cyc_Absyn_No_coercion: _LL4B8: _LL4B9:
 return({const char*_tmp4D3="no coercion";_tag_dyneither(_tmp4D3,sizeof(char),12);});case Cyc_Absyn_Null_to_NonNull: _LL4BA: _LL4BB:
 return({const char*_tmp4D4="null check";_tag_dyneither(_tmp4D4,sizeof(char),11);});default: _LL4BC: _LL4BD:
 return({const char*_tmp4D5="other coercion";_tag_dyneither(_tmp4D5,sizeof(char),15);});}_LL4B5:;}
# 2457
int Cyc_Tcutil_warn_alias_coerce=0;
# 2463
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0,& Cyc_Tcutil_rk};
# 2467
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->f1=Cyc_Absyn_Loc_n;_tmp4E9->f2=({struct _dyneither_ptr*_tmp4EA=_cycalloc(sizeof(*_tmp4EA));_tmp4EA[0]=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4ED;_tmp4ED.tag=1;_tmp4ED.f1=(unsigned long)counter ++;({void*_tmp4EB[1]={& _tmp4ED};Cyc_aprintf(({const char*_tmp4EC="__aliasvar%d";_tag_dyneither(_tmp4EC,sizeof(char),13);}),_tag_dyneither(_tmp4EB,sizeof(void*),1));});});_tmp4EA;});_tmp4E9;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(v,e_typ,e);
struct Cyc_Absyn_Exp*ve=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7[0]=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4E8;_tmp4E8.tag=4;_tmp4E8.f1=vd;_tmp4E8;});_tmp4E7;}),e->loc);
# 2476
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2478
{void*_tmp4D6=Cyc_Tcutil_compress(e_typ);void*_tmp4D7=_tmp4D6;void*_tmp4E5;struct Cyc_Absyn_Tqual _tmp4E4;void*_tmp4E3;union Cyc_Absyn_Constraint*_tmp4E2;union Cyc_Absyn_Constraint*_tmp4E1;union Cyc_Absyn_Constraint*_tmp4E0;struct Cyc_Absyn_PtrLoc*_tmp4DF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7)->tag == 5){_LL4BF: _tmp4E5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7)->f1).elt_typ;_tmp4E4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7)->f1).elt_tq;_tmp4E3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7)->f1).ptr_atts).rgn;_tmp4E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7)->f1).ptr_atts).nullable;_tmp4E1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7)->f1).ptr_atts).bounds;_tmp4E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7)->f1).ptr_atts).zero_term;_tmp4DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D7)->f1).ptr_atts).ptrloc;_LL4C0:
# 2480
{void*_tmp4D8=Cyc_Tcutil_compress(_tmp4E3);void*_tmp4D9=_tmp4D8;void**_tmp4DE;struct Cyc_Core_Opt*_tmp4DD;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D9)->tag == 1){_LL4C4: _tmp4DE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D9)->f2;_tmp4DD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D9)->f4;_LL4C5: {
# 2482
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4DA=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4DB=_cycalloc(sizeof(*_tmp4DB));_tmp4DB[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4DC;_tmp4DC.tag=2;_tmp4DC.f1=tv;_tmp4DC;});_tmp4DB;});
*_tmp4DE=(void*)_tmp4DA;
goto _LL4C3;}}else{_LL4C6: _LL4C7:
 goto _LL4C3;}_LL4C3:;}
# 2487
goto _LL4BE;}else{_LL4C1: _LL4C2:
 goto _LL4BE;}_LL4BE:;}
# 2491
e->topt=0;
vd->initializer=0;{
# 2495
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2497
return({struct _tuple15 _tmp4E6;_tmp4E6.f1=d;_tmp4E6.f2=ve;_tmp4E6;});};}
# 2502
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ,void*wants_typ,unsigned int loc){
# 2505
struct _RegionHandle _tmp4EF=_new_region("r");struct _RegionHandle*r=& _tmp4EF;_push_region(r);
if((Cyc_Tcutil_is_noalias_path(r,e) && Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2510
void*_tmp4F0=Cyc_Tcutil_compress(wants_typ);void*_tmp4F1=_tmp4F0;void*_tmp4FC;switch(*((int*)_tmp4F1)){case 5: _LL4C9: _tmp4FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4F1)->f1).ptr_atts).rgn;_LL4CA: {
# 2512
void*_tmp4F2=Cyc_Tcutil_compress(_tmp4FC);void*_tmp4F3=_tmp4F2;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4F3)->tag == 20){_LL4D0: _LL4D1: {
int _tmp4F4=0;_npop_handler(0);return _tmp4F4;}}else{_LL4D2: _LL4D3: {
# 2515
struct Cyc_Absyn_Kind*_tmp4F5=Cyc_Tcutil_typ_kind(_tmp4FC);
int _tmp4F6=_tmp4F5->kind == Cyc_Absyn_RgnKind  && _tmp4F5->aliasqual == Cyc_Absyn_Aliasable;_npop_handler(0);return _tmp4F6;}}_LL4CF:;}case 17: _LL4CB: _LL4CC:
# 2519
({struct Cyc_String_pa_PrintArg_struct _tmp4FA;_tmp4FA.tag=0;_tmp4FA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(loc));({struct Cyc_String_pa_PrintArg_struct _tmp4F9;_tmp4F9.tag=0;_tmp4F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(Cyc_Tcutil_compress(wants_typ)));({void*_tmp4F7[2]={& _tmp4F9,& _tmp4FA};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp4F8="got typedef %s in can_insert_alias at %s\n";_tag_dyneither(_tmp4F8,sizeof(char),42);}),_tag_dyneither(_tmp4F7,sizeof(void*),2));});});});default: _LL4CD: _LL4CE: {
int _tmp4FB=0;_npop_handler(0);return _tmp4FB;}}_LL4C8:;}{
# 2524
int _tmp4FD=0;_npop_handler(0);return _tmp4FD;};
# 2506
;_pop_region(r);}
# 2528
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2533
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2535
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2537
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp501;_tmp501.tag=0;_tmp501.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp500;_tmp500.tag=0;_tmp500.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp4FE[2]={& _tmp500,& _tmp501};Cyc_Tcutil_warn(e->loc,({const char*_tmp4FF="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp4FF,sizeof(char),53);}),_tag_dyneither(_tmp4FE,sizeof(void*),2));});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2546
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp506;_tmp506.tag=0;_tmp506.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp505;_tmp505.tag=0;_tmp505.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({struct Cyc_String_pa_PrintArg_struct _tmp504;_tmp504.tag=0;_tmp504.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));({void*_tmp502[3]={& _tmp504,& _tmp505,& _tmp506};Cyc_Tcutil_warn(e->loc,({const char*_tmp503="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp503,sizeof(char),40);}),_tag_dyneither(_tmp502,sizeof(void*),3));});});});});
*alias_coercion=1;}
# 2553
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= Cyc_Absyn_Unknown_coercion){
# 2561
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp50A;_tmp50A.tag=0;_tmp50A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));({struct Cyc_String_pa_PrintArg_struct _tmp509;_tmp509.tag=0;_tmp509.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));({void*_tmp507[2]={& _tmp509,& _tmp50A};Cyc_Tcutil_warn(e->loc,({const char*_tmp508="implicit cast from %s to %s";_tag_dyneither(_tmp508,sizeof(char),28);}),_tag_dyneither(_tmp507,sizeof(void*),2));});});});
return 1;}else{
# 2567
return 0;}}}}}
# 2574
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2577
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2581
int Cyc_Tcutil_coerceable(void*t){
void*_tmp50B=Cyc_Tcutil_compress(t);void*_tmp50C=_tmp50B;switch(*((int*)_tmp50C)){case 6: _LL4D5: _LL4D6:
 goto _LL4D8;case 7: _LL4D7: _LL4D8:
 return 1;default: _LL4D9: _LL4DA:
 return 0;}_LL4D4:;}
# 2599 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2603
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2606
struct _tuple23 _tmp50D=*env;struct _tuple23 _tmp50E=_tmp50D;struct Cyc_List_List*_tmp516;struct _RegionHandle*_tmp515;struct Cyc_Tcenv_Tenv*_tmp514;int _tmp513;_LL4DC: _tmp516=_tmp50E.f1;_tmp515=_tmp50E.f2;_tmp514=_tmp50E.f3;_tmp513=_tmp50E.f4;_LL4DD:;{
# 2608
void*_tmp50F=_tmp516 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp515,_tmp516,x->type);
struct Cyc_List_List*_tmp510=Cyc_Tcutil_flatten_typ(_tmp515,_tmp513,_tmp514,_tmp50F);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp510)== 1)
return({struct Cyc_List_List*_tmp511=_region_malloc(_tmp515,sizeof(*_tmp511));_tmp511->hd=({struct _tuple12*_tmp512=_region_malloc(_tmp515,sizeof(*_tmp512));_tmp512->f1=x->tq;_tmp512->f2=_tmp50F;_tmp512;});_tmp511->tl=0;_tmp511;});else{
return _tmp510;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2614
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2616
struct _tuple24 _tmp517=*env;struct _tuple24 _tmp518=_tmp517;struct _RegionHandle*_tmp522;struct Cyc_Tcenv_Tenv*_tmp521;int _tmp520;_LL4DF: _tmp522=_tmp518.f1;_tmp521=_tmp518.f2;_tmp520=_tmp518.f3;_LL4E0:;{
struct _tuple12 _tmp519=*x;struct _tuple12 _tmp51A=_tmp519;struct Cyc_Absyn_Tqual _tmp51F;void*_tmp51E;_LL4E2: _tmp51F=_tmp51A.f1;_tmp51E=_tmp51A.f2;_LL4E3:;{
struct Cyc_List_List*_tmp51B=Cyc_Tcutil_flatten_typ(_tmp522,_tmp520,_tmp521,_tmp51E);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp51B)== 1)
return({struct Cyc_List_List*_tmp51C=_region_malloc(_tmp522,sizeof(*_tmp51C));_tmp51C->hd=({struct _tuple12*_tmp51D=_region_malloc(_tmp522,sizeof(*_tmp51D));_tmp51D->f1=_tmp51F;_tmp51D->f2=_tmp51E;_tmp51D;});_tmp51C->tl=0;_tmp51C;});else{
return _tmp51B;}};};}
# 2623
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2627
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp523=t1;struct Cyc_List_List*_tmp545;struct Cyc_Absyn_Aggrdecl*_tmp544;struct Cyc_List_List*_tmp543;struct Cyc_List_List*_tmp542;switch(*((int*)_tmp523)){case 0: _LL4E5: _LL4E6:
 return 0;case 10: _LL4E7: _tmp542=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp523)->f1;_LL4E8: {
# 2632
struct _tuple24 _tmp524=({struct _tuple24 _tmp52D;_tmp52D.f1=r;_tmp52D.f2=te;_tmp52D.f3=flatten;_tmp52D;});
# 2634
struct Cyc_List_List*_tmp525=_tmp542;struct _tuple12*_tmp52C;struct Cyc_List_List*_tmp52B;if(_tmp525 == 0){_LL4F0: _LL4F1:
 return 0;}else{_LL4F2: _tmp52C=(struct _tuple12*)_tmp525->hd;_tmp52B=_tmp525->tl;_LL4F3: {
# 2637
struct Cyc_List_List*_tmp526=Cyc_Tcutil_rcopy_tqt(& _tmp524,_tmp52C);
_tmp524.f3=0;{
struct Cyc_List_List*_tmp527=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp524,_tmp542);
struct Cyc_List_List*_tmp528=({struct Cyc_List_List*_tmp52A=_region_malloc(r,sizeof(*_tmp52A));_tmp52A->hd=_tmp526;_tmp52A->tl=_tmp527;_tmp52A;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,({struct Cyc_List_List*_tmp529=_region_malloc(r,sizeof(*_tmp529));_tmp529->hd=_tmp526;_tmp529->tl=_tmp527;_tmp529;}));};}}_LL4EF:;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp523)->f1).aggr_info).KnownAggr).tag == 2){_LL4E9: _tmp544=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp523)->f1).aggr_info).KnownAggr).val;_tmp543=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp523)->f1).targs;_LL4EA:
# 2645
 if(((_tmp544->kind == Cyc_Absyn_UnionA  || _tmp544->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp544->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp544->impl))->rgn_po != 0)
# 2647
return({struct Cyc_List_List*_tmp52E=_region_malloc(r,sizeof(*_tmp52E));_tmp52E->hd=({struct _tuple12*_tmp52F=_region_malloc(r,sizeof(*_tmp52F));_tmp52F->f1=Cyc_Absyn_empty_tqual(0);_tmp52F->f2=t1;_tmp52F;});_tmp52E->tl=0;_tmp52E;});{
struct Cyc_List_List*_tmp530=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp544->tvs,_tmp543);
struct _tuple23 env=({struct _tuple23 _tmp539;_tmp539.f1=_tmp530;_tmp539.f2=r;_tmp539.f3=te;_tmp539.f4=flatten;_tmp539;});
struct Cyc_List_List*_tmp531=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp544->impl))->fields;struct Cyc_List_List*_tmp532=_tmp531;struct Cyc_Absyn_Aggrfield*_tmp538;struct Cyc_List_List*_tmp537;if(_tmp532 == 0){_LL4F5: _LL4F6:
 return 0;}else{_LL4F7: _tmp538=(struct Cyc_Absyn_Aggrfield*)_tmp532->hd;_tmp537=_tmp532->tl;_LL4F8: {
# 2653
struct Cyc_List_List*_tmp533=Cyc_Tcutil_flatten_typ_f(& env,_tmp538);
env.f4=0;{
struct Cyc_List_List*_tmp534=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp537);
struct Cyc_List_List*_tmp535=({struct Cyc_List_List*_tmp536=_region_malloc(r,sizeof(*_tmp536));_tmp536->hd=_tmp533;_tmp536->tl=_tmp534;_tmp536;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp535);};}}_LL4F4:;};}else{goto _LL4ED;}case 12: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp523)->f1 == Cyc_Absyn_StructA){_LL4EB: _tmp545=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp523)->f2;_LL4EC: {
# 2660
struct _tuple23 env=({struct _tuple23 _tmp541;_tmp541.f1=0;_tmp541.f2=r;_tmp541.f3=te;_tmp541.f4=flatten;_tmp541;});
struct Cyc_List_List*_tmp53A=_tmp545;struct Cyc_Absyn_Aggrfield*_tmp540;struct Cyc_List_List*_tmp53F;if(_tmp53A == 0){_LL4FA: _LL4FB:
 return 0;}else{_LL4FC: _tmp540=(struct Cyc_Absyn_Aggrfield*)_tmp53A->hd;_tmp53F=_tmp53A->tl;_LL4FD: {
# 2664
struct Cyc_List_List*_tmp53B=Cyc_Tcutil_flatten_typ_f(& env,_tmp540);
env.f4=0;{
struct Cyc_List_List*_tmp53C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp53F);
struct Cyc_List_List*_tmp53D=({struct Cyc_List_List*_tmp53E=_region_malloc(r,sizeof(*_tmp53E));_tmp53E->hd=_tmp53B;_tmp53E->tl=_tmp53C;_tmp53E;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp53D);};}}_LL4F9:;}}else{goto _LL4ED;}default: _LL4ED: _LL4EE:
# 2670
 goto _LL4E4;}_LL4E4:;};}
# 2673
return({struct Cyc_List_List*_tmp546=_region_malloc(r,sizeof(*_tmp546));_tmp546->hd=({struct _tuple12*_tmp547=_region_malloc(r,sizeof(*_tmp547));_tmp547->f1=Cyc_Absyn_empty_tqual(0);_tmp547->f2=t1;_tmp547;});_tmp546->tl=0;_tmp546;});}
# 2677
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp548=(void*)t->hd;void*_tmp549=_tmp548;switch(*((int*)_tmp549)){case 23: _LL4FF: _LL500:
 goto _LL502;case 4: _LL501: _LL502:
 goto _LL504;case 20: _LL503: _LL504:
# 2683
 continue;default: _LL505: _LL506:
# 2685
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL4FE:;}}
# 2688
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2691
return 1;}
# 2694
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp54A=({struct _tuple0 _tmp54E;_tmp54E.f1=Cyc_Tcutil_compress(t1);_tmp54E.f2=Cyc_Tcutil_compress(t2);_tmp54E;});struct _tuple0 _tmp54B=_tmp54A;enum Cyc_Absyn_Size_of _tmp54D;enum Cyc_Absyn_Size_of _tmp54C;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp54B.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp54B.f2)->tag == 6){_LL508: _tmp54D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp54B.f1)->f2;_tmp54C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp54B.f2)->f2;_LL509:
# 2697
 return(_tmp54D == _tmp54C  || _tmp54D == Cyc_Absyn_Int_sz  && _tmp54C == Cyc_Absyn_Long_sz) || 
_tmp54D == Cyc_Absyn_Long_sz  && _tmp54C == Cyc_Absyn_Int_sz;}else{goto _LL50A;}}else{_LL50A: _LL50B:
 return 0;}_LL507:;}
# 2705
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2708
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2713
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple0 _tmp54F=({struct _tuple0 _tmp585;_tmp585.f1=t1;_tmp585.f2=t2;_tmp585;});struct _tuple0 _tmp550=_tmp54F;struct Cyc_Absyn_FnInfo _tmp584;struct Cyc_Absyn_FnInfo _tmp583;struct Cyc_Absyn_Datatypedecl*_tmp582;struct Cyc_Absyn_Datatypefield*_tmp581;struct Cyc_List_List*_tmp580;struct Cyc_Absyn_Datatypedecl*_tmp57F;struct Cyc_List_List*_tmp57E;void*_tmp57D;struct Cyc_Absyn_Tqual _tmp57C;void*_tmp57B;union Cyc_Absyn_Constraint*_tmp57A;union Cyc_Absyn_Constraint*_tmp579;union Cyc_Absyn_Constraint*_tmp578;void*_tmp577;struct Cyc_Absyn_Tqual _tmp576;void*_tmp575;union Cyc_Absyn_Constraint*_tmp574;union Cyc_Absyn_Constraint*_tmp573;union Cyc_Absyn_Constraint*_tmp572;switch(*((int*)_tmp550.f1)){case 5: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->tag == 5){_LL50D: _tmp57D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->f1).elt_typ;_tmp57C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->f1).elt_tq;_tmp57B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->f1).ptr_atts).rgn;_tmp57A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->f1).ptr_atts).nullable;_tmp579=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->f1).ptr_atts).bounds;_tmp578=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f1)->f1).ptr_atts).zero_term;_tmp577=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->f1).elt_typ;_tmp576=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->f1).elt_tq;_tmp575=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->f1).ptr_atts).rgn;_tmp574=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->f1).ptr_atts).nullable;_tmp573=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->f1).ptr_atts).bounds;_tmp572=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp550.f2)->f1).ptr_atts).zero_term;_LL50E:
# 2721
 if(_tmp57C.real_const  && !_tmp576.real_const)
return 0;
# 2724
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp57A,_tmp574) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp57A)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp574))
return 0;
# 2728
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp578,_tmp572) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp578)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp572))
return 0;
# 2732
if((!Cyc_Tcutil_unify(_tmp57B,_tmp575) && !Cyc_Tcenv_region_outlives(te,_tmp57B,_tmp575)) && !
Cyc_Tcutil_subtype(te,assume,_tmp57B,_tmp575))
return 0;
# 2736
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp579,_tmp573)){
struct _tuple0 _tmp551=({struct _tuple0 _tmp555;_tmp555.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp579);_tmp555.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp573);_tmp555;});struct _tuple0 _tmp552=_tmp551;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp553;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp552.f1)->tag == 0){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp552.f2)->tag == 0){_LL516: _LL517:
 goto _LL515;}else{goto _LL51A;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp552.f2)->tag == 1){_LL518: _tmp554=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp552.f1)->f1;_tmp553=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp552.f2)->f1;_LL519:
# 2741
 if(!Cyc_Evexp_lte_const_exp(_tmp553,_tmp554))
return 0;
goto _LL515;}else{_LL51A: _LL51B:
 return 0;}}_LL515:;}
# 2749
if(!_tmp576.real_const  && _tmp57C.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp577)))
return 0;}{
# 2755
int _tmp556=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp573,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp572);
# 2761
return(_tmp556  && Cyc_Tcutil_ptrsubtype(te,({struct Cyc_List_List*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->hd=({struct _tuple0*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->f1=t1;_tmp558->f2=t2;_tmp558;});_tmp557->tl=assume;_tmp557;}),_tmp57D,_tmp577) || Cyc_Tcutil_unify(_tmp57D,_tmp577)) || Cyc_Tcutil_isomorphic(_tmp57D,_tmp577);};}else{goto _LL513;}case 4: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp550.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp550.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp550.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL50F: _tmp582=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp550.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp581=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp550.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp580=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp550.f1)->f1).targs;_tmp57F=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp550.f2)->f1).datatype_info).KnownDatatype).val;_tmp57E=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp550.f2)->f1).targs;_LL510:
# 2767
 if(_tmp582 != _tmp57F  && Cyc_Absyn_qvar_cmp(_tmp582->name,_tmp57F->name)!= 0)return 0;
# 2769
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp580)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp57E))return 0;
for(0;_tmp580 != 0;(_tmp580=_tmp580->tl,_tmp57E=_tmp57E->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp580->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp57E))->hd))return 0;}
return 1;}else{goto _LL513;}}else{goto _LL513;}}else{goto _LL513;}case 9: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp550.f2)->tag == 9){_LL511: _tmp584=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp550.f1)->f1;_tmp583=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp550.f2)->f1;_LL512:
# 2776
 if(_tmp584.tvars != 0  || _tmp583.tvars != 0){
struct Cyc_List_List*_tmp559=_tmp584.tvars;
struct Cyc_List_List*_tmp55A=_tmp583.tvars;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp559)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp55A))return 0;{
struct _RegionHandle _tmp55B=_new_region("r");struct _RegionHandle*r=& _tmp55B;_push_region(r);
{struct Cyc_List_List*inst=0;
while(_tmp559 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp559->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp55A))->hd)->kind)){int _tmp55C=0;_npop_handler(0);return _tmp55C;}
inst=({struct Cyc_List_List*_tmp55D=_region_malloc(r,sizeof(*_tmp55D));_tmp55D->hd=({struct _tuple16*_tmp55E=_region_malloc(r,sizeof(*_tmp55E));_tmp55E->f1=(struct Cyc_Absyn_Tvar*)_tmp55A->hd;_tmp55E->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp55F=_cycalloc(sizeof(*_tmp55F));_tmp55F[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp560;_tmp560.tag=2;_tmp560.f1=(struct Cyc_Absyn_Tvar*)_tmp559->hd;_tmp560;});_tmp55F;});_tmp55E;});_tmp55D->tl=inst;_tmp55D;});
_tmp559=_tmp559->tl;
_tmp55A=_tmp55A->tl;}
# 2788
if(inst != 0){
_tmp584.tvars=0;
_tmp583.tvars=0;{
int _tmp565=Cyc_Tcutil_subtype(te,assume,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp561=_cycalloc(sizeof(*_tmp561));_tmp561[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp562;_tmp562.tag=9;_tmp562.f1=_tmp584;_tmp562;});_tmp561;}),(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp563=_cycalloc(sizeof(*_tmp563));_tmp563[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp564;_tmp564.tag=9;_tmp564.f1=_tmp583;_tmp564;});_tmp563;}));_npop_handler(0);return _tmp565;};}}
# 2781
;_pop_region(r);};}
# 2796
if(!Cyc_Tcutil_subtype(te,assume,_tmp584.ret_typ,_tmp583.ret_typ))return 0;{
struct Cyc_List_List*_tmp566=_tmp584.args;
struct Cyc_List_List*_tmp567=_tmp583.args;
# 2801
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp566)!= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp567))return 0;
# 2803
for(0;_tmp566 != 0;(_tmp566=_tmp566->tl,_tmp567=_tmp567->tl)){
struct _tuple10 _tmp568=*((struct _tuple10*)_tmp566->hd);struct _tuple10 _tmp569=_tmp568;struct Cyc_Absyn_Tqual _tmp56F;void*_tmp56E;_LL51D: _tmp56F=_tmp569.f2;_tmp56E=_tmp569.f3;_LL51E:;{
struct _tuple10 _tmp56A=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp567))->hd);struct _tuple10 _tmp56B=_tmp56A;struct Cyc_Absyn_Tqual _tmp56D;void*_tmp56C;_LL520: _tmp56D=_tmp56B.f2;_tmp56C=_tmp56B.f3;_LL521:;
# 2807
if(_tmp56D.real_const  && !_tmp56F.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp56C,_tmp56E))
return 0;};}
# 2811
if(_tmp584.c_varargs != _tmp583.c_varargs)return 0;
if(_tmp584.cyc_varargs != 0  && _tmp583.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp570=*_tmp584.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp571=*_tmp583.cyc_varargs;
# 2816
if((_tmp571.tq).real_const  && !(_tmp570.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp571.type,_tmp570.type))
return 0;}else{
if(_tmp584.cyc_varargs != 0  || _tmp583.cyc_varargs != 0)return 0;}
# 2821
if(!Cyc_Tcutil_subset_effect(1,(void*)_check_null(_tmp584.effect),(void*)_check_null(_tmp583.effect)))return 0;
# 2823
if(!Cyc_Tcutil_sub_rgnpo(_tmp584.rgn_po,_tmp583.rgn_po))return 0;
# 2825
if(!Cyc_Tcutil_sub_attributes(_tmp584.attributes,_tmp583.attributes))return 0;
# 2827
if(!Cyc_Tcutil_check_logical_implication(_tmp583.requires_relns,_tmp584.requires_relns))
return 0;
# 2830
if(!Cyc_Tcutil_check_logical_implication(_tmp584.ensures_relns,_tmp583.ensures_relns))
return 0;
# 2833
return 1;};}else{goto _LL513;}default: _LL513: _LL514:
 return 0;}_LL50C:;};}
# 2845 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2847
struct _RegionHandle _tmp586=_new_region("temp");struct _RegionHandle*temp=& _tmp586;_push_region(temp);
{struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(temp,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(temp,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0){int _tmp587=0;_npop_handler(0);return _tmp587;}{
struct _tuple12*_tmp588=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp589=_tmp588;struct Cyc_Absyn_Tqual _tmp591;void*_tmp590;_LL523: _tmp591=_tmp589->f1;_tmp590=_tmp589->f2;_LL524:;{
struct _tuple12*_tmp58A=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp58B=_tmp58A;struct Cyc_Absyn_Tqual _tmp58F;void*_tmp58E;_LL526: _tmp58F=_tmp58B->f1;_tmp58E=_tmp58B->f2;_LL527:;
# 2855
if(_tmp591.real_const  && !_tmp58F.real_const){int _tmp58C=0;_npop_handler(0);return _tmp58C;}
# 2857
if((_tmp58F.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp58E))) && 
Cyc_Tcutil_subtype(te,assume,_tmp590,_tmp58E))
# 2860
continue;
# 2862
if(Cyc_Tcutil_unify(_tmp590,_tmp58E))
# 2864
continue;
# 2866
if(Cyc_Tcutil_isomorphic(_tmp590,_tmp58E))
# 2868
continue;{
# 2871
int _tmp58D=0;_npop_handler(0);return _tmp58D;};};};}{
# 2873
int _tmp592=1;_npop_handler(0);return _tmp592;};}
# 2848
;_pop_region(temp);}
# 2877
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp593=Cyc_Tcutil_compress(t);void*_tmp594=_tmp593;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp594)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp594)->f2 == Cyc_Absyn_Char_sz){_LL529: _LL52A:
 return 1;}else{goto _LL52B;}}else{_LL52B: _LL52C:
 return 0;}_LL528:;}
# 2886
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2892
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp595=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp595)->tag == 6)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp595)->f2){case Cyc_Absyn_Int_sz: _LL52E: _LL52F:
# 2896
 goto _LL531;case Cyc_Absyn_Long_sz: _LL530: _LL531:
# 2898
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL52D;default: goto _LL532;}else{_LL532: _LL533:
 goto _LL52D;}_LL52D:;}{
# 2902
void*_tmp596=t1;void*_tmp5BE;struct Cyc_Absyn_Enumdecl*_tmp5BD;void*_tmp5BC;struct Cyc_Absyn_Tqual _tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;union Cyc_Absyn_Constraint*_tmp5B9;void*_tmp5B8;struct Cyc_Absyn_Tqual _tmp5B7;void*_tmp5B6;union Cyc_Absyn_Constraint*_tmp5B5;union Cyc_Absyn_Constraint*_tmp5B4;union Cyc_Absyn_Constraint*_tmp5B3;switch(*((int*)_tmp596)){case 5: _LL535: _tmp5B8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).elt_typ;_tmp5B7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).elt_tq;_tmp5B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).ptr_atts).rgn;_tmp5B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).ptr_atts).nullable;_tmp5B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).ptr_atts).bounds;_tmp5B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp596)->f1).ptr_atts).zero_term;_LL536:
# 2911
{void*_tmp597=t2;void*_tmp5A9;struct Cyc_Absyn_Tqual _tmp5A8;void*_tmp5A7;union Cyc_Absyn_Constraint*_tmp5A6;union Cyc_Absyn_Constraint*_tmp5A5;union Cyc_Absyn_Constraint*_tmp5A4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597)->tag == 5){_LL544: _tmp5A9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597)->f1).elt_typ;_tmp5A8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597)->f1).elt_tq;_tmp5A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597)->f1).ptr_atts).rgn;_tmp5A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597)->f1).ptr_atts).nullable;_tmp5A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597)->f1).ptr_atts).bounds;_tmp5A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp597)->f1).ptr_atts).zero_term;_LL545: {
# 2915
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp598=({struct Cyc_List_List*_tmp5A2=_cycalloc(sizeof(*_tmp5A2));_tmp5A2->hd=({struct _tuple0*_tmp5A3=_cycalloc(sizeof(*_tmp5A3));_tmp5A3->f1=t1;_tmp5A3->f2=t2;_tmp5A3;});_tmp5A2->tl=0;_tmp5A2;});
int _tmp599=_tmp5A8.real_const  || !_tmp5B7.real_const;
# 2929 "tcutil.cyc"
int _tmp59A=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5A5,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp5A4);
# 2933
int _tmp59B=_tmp599  && (
((_tmp59A  && Cyc_Tcutil_ptrsubtype(te,_tmp598,_tmp5B8,_tmp5A9) || 
Cyc_Tcutil_unify(_tmp5B8,_tmp5A9)) || Cyc_Tcutil_isomorphic(_tmp5B8,_tmp5A9)) || Cyc_Tcutil_unify(_tmp5A9,(void*)& Cyc_Absyn_VoidType_val));
# 2937
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B3,_tmp5A4) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5A4);
# 2941
int _tmp59C=_tmp59B?0:((Cyc_Tcutil_bits_only(_tmp5B8) && Cyc_Tcutil_is_char_type(_tmp5A9)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A4)) && (
_tmp5A8.real_const  || !_tmp5B7.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp5B4,_tmp5A5);
if(!bounds_ok  && !_tmp59C){
struct _tuple0 _tmp59D=({struct _tuple0 _tmp5A1;_tmp5A1.f1=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5B4);_tmp5A1.f2=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp5A5);_tmp5A1;});struct _tuple0 _tmp59E=_tmp59D;struct Cyc_Absyn_Exp*_tmp5A0;struct Cyc_Absyn_Exp*_tmp59F;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59E.f1)->tag == 1){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59E.f2)->tag == 1){_LL549: _tmp5A0=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59E.f1)->f1;_tmp59F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp59E.f2)->f1;_LL54A:
# 2951
 if(Cyc_Evexp_lte_const_exp(_tmp59F,_tmp5A0))
bounds_ok=1;
goto _LL548;}else{goto _LL54B;}}else{_LL54B: _LL54C:
# 2956
 bounds_ok=1;goto _LL548;}_LL548:;}{
# 2958
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5B5);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp5A6);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2965
if(((bounds_ok  && zeroterm_ok) && (_tmp59B  || _tmp59C)) && (
Cyc_Tcutil_unify(_tmp5B6,_tmp5A7) || Cyc_Tcenv_region_outlives(te,_tmp5B6,_tmp5A7)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL546: _LL547:
 goto _LL543;}_LL543:;}
# 2971
return Cyc_Absyn_Unknown_coercion;case 8: _LL537: _tmp5BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->f1).elt_type;_tmp5BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->f1).tq;_tmp5BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->f1).num_elts;_tmp5B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp596)->f1).zero_term;_LL538:
# 2973
{void*_tmp5AA=t2;void*_tmp5AE;struct Cyc_Absyn_Tqual _tmp5AD;struct Cyc_Absyn_Exp*_tmp5AC;union Cyc_Absyn_Constraint*_tmp5AB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5AA)->tag == 8){_LL54E: _tmp5AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5AA)->f1).elt_type;_tmp5AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5AA)->f1).tq;_tmp5AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5AA)->f1).num_elts;_tmp5AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5AA)->f1).zero_term;_LL54F: {
# 2975
int okay;
okay=
(((_tmp5BA != 0  && _tmp5AC != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp5B9,_tmp5AB)) && 
Cyc_Evexp_lte_const_exp(_tmp5AC,_tmp5BA)) && 
Cyc_Evexp_lte_const_exp(_tmp5BA,_tmp5AC);
return
# 2982
(okay  && Cyc_Tcutil_unify(_tmp5BC,_tmp5AE)) && (!_tmp5BB.real_const  || _tmp5AD.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL550: _LL551:
# 2984
 return Cyc_Absyn_Unknown_coercion;}_LL54D:;}
# 2986
return Cyc_Absyn_Unknown_coercion;case 13: _LL539: _tmp5BD=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp596)->f2;_LL53A:
# 2990
{void*_tmp5AF=t2;struct Cyc_Absyn_Enumdecl*_tmp5B0;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5AF)->tag == 13){_LL553: _tmp5B0=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp5AF)->f2;_LL554:
# 2992
 if((_tmp5BD->fields != 0  && _tmp5B0->fields != 0) && 
((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5BD->fields))->v)>= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5B0->fields))->v))
return Cyc_Absyn_Other_coercion;
goto _LL552;}else{_LL555: _LL556:
 goto _LL552;}_LL552:;}
# 2998
goto _LL53C;case 6: _LL53B: _LL53C:
 goto _LL53E;case 7: _LL53D: _LL53E:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15: _LL53F: _tmp5BE=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp596)->f1;_LL540:
# 3003
{void*_tmp5B1=t2;void*_tmp5B2;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5B1)->tag == 15){_LL558: _tmp5B2=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp5B1)->f1;_LL559:
# 3005
 if(Cyc_Tcenv_region_outlives(te,_tmp5BE,_tmp5B2))return Cyc_Absyn_No_coercion;
goto _LL557;}else{_LL55A: _LL55B:
 goto _LL557;}_LL557:;}
# 3009
return Cyc_Absyn_Unknown_coercion;default: _LL541: _LL542:
 return Cyc_Absyn_Unknown_coercion;}_LL534:;};}
# 3015
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5BF=Cyc_Absyn_copy_exp(e);
e->r=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5C0=_cycalloc(sizeof(*_tmp5C0));_tmp5C0[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5C1;_tmp5C1.tag=13;_tmp5C1.f1=t;_tmp5C1.f2=_tmp5BF;_tmp5C1.f3=0;_tmp5C1.f4=c;_tmp5C1;});_tmp5C0;});
e->topt=t;}}
# 3023
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5C2=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5C3=_tmp5C2;switch(*((int*)_tmp5C3)){case 6: _LL55D: _LL55E:
 goto _LL560;case 13: _LL55F: _LL560:
 goto _LL562;case 14: _LL561: _LL562:
 goto _LL564;case 19: _LL563: _LL564:
 return 1;case 1: _LL565: _LL566:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LL567: _LL568:
 return 0;}_LL55C:;}
# 3034
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5C4=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5C5=_tmp5C4;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C5)->tag == 7){_LL56A: _LL56B:
 return 1;}else{_LL56C: _LL56D:
 return 0;}_LL569:;};}
# 3043
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5C6=Cyc_Tcutil_compress(t);void*_tmp5C7=_tmp5C6;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5C7)->tag == 9){_LL56F: _LL570:
 return 1;}else{_LL571: _LL572:
 return 0;}_LL56E:;}
# 3050
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5C8=({struct _tuple0 _tmp5CC;_tmp5CC.f1=t1;_tmp5CC.f2=t2;_tmp5CC;});struct _tuple0 _tmp5C9=_tmp5C8;int _tmp5CB;int _tmp5CA;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C9.f1)->tag == 7){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C9.f2)->tag == 7){_LL574: _tmp5CB=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C9.f1)->f1;_tmp5CA=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C9.f2)->f1;_LL575:
# 3053
 if(_tmp5CB != 0  && _tmp5CB != 1)return t1;else{
if(_tmp5CA != 0  && _tmp5CA != 1)return t2;else{
if(_tmp5CB >= _tmp5CA)return t1;else{
return t2;}}}}else{_LL576: _LL577:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5C9.f2)->tag == 7){_LL578: _LL579:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL57A: _LL57B:
 goto _LL57D;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL582;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL584;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL586;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL588;else{goto _LL58E;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL582;case Cyc_Absyn_Int_sz: goto _LL586;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2){case Cyc_Absyn_Long_sz: _LL582: _LL583:
# 3063
 goto _LL585;case Cyc_Absyn_Int_sz: _LL586: _LL587:
# 3066
 goto _LL589;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57C;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL584;case Cyc_Absyn_Int_sz: goto _LL588;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{goto _LL58E;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL57E;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL580;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL58A;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL58C;else{goto _LL58E;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f1)->f2){case Cyc_Absyn_LongLong_sz: _LL57E: _LL57F:
# 3061
 goto _LL581;case Cyc_Absyn_Long_sz: _LL58A: _LL58B:
# 3068
 goto _LL58D;default: goto _LL58E;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL57C: _LL57D:
# 3060
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL584: _LL585:
# 3064
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL588: _LL589:
# 3067
 return Cyc_Absyn_uint_typ;default: goto _LL58E;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5C9.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL580: _LL581:
# 3062
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL58C: _LL58D:
# 3069
 return Cyc_Absyn_slong_typ;default: goto _LL58E;}}}else{_LL58E: _LL58F:
 return Cyc_Absyn_sint_typ;}}}}_LL573:;}
# 3081 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
c1=Cyc_Absyn_compress_kb(c1);
c2=Cyc_Absyn_compress_kb(c2);
if(c1 == c2)return 1;{
struct _tuple0 _tmp5CD=({struct _tuple0 _tmp5E3;_tmp5E3.f1=c1;_tmp5E3.f2=c2;_tmp5E3;});struct _tuple0 _tmp5CE=_tmp5CD;struct Cyc_Core_Opt**_tmp5E2;struct Cyc_Absyn_Kind*_tmp5E1;struct Cyc_Core_Opt**_tmp5E0;struct Cyc_Absyn_Kind*_tmp5DF;struct Cyc_Core_Opt**_tmp5DE;struct Cyc_Absyn_Kind*_tmp5DD;struct Cyc_Absyn_Kind*_tmp5DC;struct Cyc_Core_Opt**_tmp5DB;struct Cyc_Core_Opt**_tmp5DA;struct Cyc_Absyn_Kind*_tmp5D9;struct Cyc_Core_Opt**_tmp5D8;struct Cyc_Absyn_Kind*_tmp5D7;struct Cyc_Absyn_Kind*_tmp5D6;struct Cyc_Absyn_Kind*_tmp5D5;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->tag == 0)switch(*((int*)_tmp5CE.f2)){case 0: _LL591: _tmp5D6=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->f1;_tmp5D5=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->f1;_LL592:
 return _tmp5D6 == _tmp5D5;case 1: goto _LL593;default: _LL599: _tmp5D9=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->f1;_tmp5D8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->f1;_tmp5D7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->f2;_LL59A:
# 3094
 if(Cyc_Tcutil_kind_leq(_tmp5D9,_tmp5D7)){
*_tmp5D8=({struct Cyc_Core_Opt*_tmp5D2=_cycalloc(sizeof(*_tmp5D2));_tmp5D2->v=c1;_tmp5D2;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->tag == 1){_LL593: _tmp5DA=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->f1;_LL594:
# 3087
*_tmp5DA=({struct Cyc_Core_Opt*_tmp5CF=_cycalloc(sizeof(*_tmp5CF));_tmp5CF->v=c1;_tmp5CF;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->tag == 1){_LL595: _tmp5DB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->f1;_LL596:
*_tmp5DB=({struct Cyc_Core_Opt*_tmp5D0=_cycalloc(sizeof(*_tmp5D0));_tmp5D0->v=c2;_tmp5D0;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->tag == 0){_LL597: _tmp5DE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->f1;_tmp5DD=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->f2;_tmp5DC=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->f1;_LL598:
# 3090
 if(Cyc_Tcutil_kind_leq(_tmp5DC,_tmp5DD)){
*_tmp5DE=({struct Cyc_Core_Opt*_tmp5D1=_cycalloc(sizeof(*_tmp5D1));_tmp5D1->v=c2;_tmp5D1;});return 1;}else{
return 0;}}else{_LL59B: _tmp5E2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->f1;_tmp5E1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5CE.f1)->f2;_tmp5E0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->f1;_tmp5DF=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5CE.f2)->f2;_LL59C:
# 3098
 if(Cyc_Tcutil_kind_leq(_tmp5E1,_tmp5DF)){
*_tmp5E0=({struct Cyc_Core_Opt*_tmp5D3=_cycalloc(sizeof(*_tmp5D3));_tmp5D3->v=c1;_tmp5D3;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5DF,_tmp5E1)){
*_tmp5E2=({struct Cyc_Core_Opt*_tmp5D4=_cycalloc(sizeof(*_tmp5D4));_tmp5D4->v=c2;_tmp5D4;});return 1;}else{
return 0;}}}}}}_LL590:;};}
# 3107
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3112
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5E8;_tmp5E8.tag=1;_tmp5E8.f1=(unsigned long)i;({void*_tmp5E6[1]={& _tmp5E8};Cyc_aprintf(({const char*_tmp5E7="#%d";_tag_dyneither(_tmp5E7,sizeof(char),4);}),_tag_dyneither(_tmp5E6,sizeof(void*),1));});});
return({struct Cyc_Absyn_Tvar*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->name=({struct _dyneither_ptr*_tmp5E5=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5E5[0]=(struct _dyneither_ptr)s;_tmp5E5;});_tmp5E4->identity=- 1;_tmp5E4->kind=k;_tmp5E4;});}
# 3119
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5E9=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5E9,sizeof(char),0))== '#';}
# 3124
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5EC;_tmp5EC.tag=0;_tmp5EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5EA[1]={& _tmp5EC};Cyc_printf(({const char*_tmp5EB="%s";_tag_dyneither(_tmp5EB,sizeof(char),3);}),_tag_dyneither(_tmp5EA,sizeof(void*),1));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp5ED=Cyc_strconcat(({const char*_tmp5F2="`";_tag_dyneither(_tmp5F2,sizeof(char),2);}),(struct _dyneither_ptr)*t->name);
({struct _dyneither_ptr _tmp5EE=_dyneither_ptr_plus(_tmp5ED,sizeof(char),1);char _tmp5EF=*((char*)_check_dyneither_subscript(_tmp5EE,sizeof(char),0));char _tmp5F0='t';if(_get_dyneither_size(_tmp5EE,sizeof(char))== 1  && (_tmp5EF == '\000'  && _tmp5F0 != '\000'))_throw_arraybounds();*((char*)_tmp5EE.curr)=_tmp5F0;});
t->name=({struct _dyneither_ptr*_tmp5F1=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5F1[0]=(struct _dyneither_ptr)_tmp5ED;_tmp5F1;});};}
# 3133
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3135
return({struct _tuple10*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));_tmp5F3->f1=(*x).f1;_tmp5F3->f2=(*x).f2;_tmp5F3->f3=(*x).f3;_tmp5F3;});}
# 3138
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3145
struct Cyc_List_List*_tmp5F4=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp5F4=({struct Cyc_List_List*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->hd=(void*)atts->hd;_tmp5F5->tl=_tmp5F4;_tmp5F5;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp5F7;_tmp5F7.tag=9;_tmp5F7.f1=({struct Cyc_Absyn_FnInfo _tmp5F8;_tmp5F8.tvars=fd->tvs;_tmp5F8.effect=fd->effect;_tmp5F8.ret_tqual=fd->ret_tqual;_tmp5F8.ret_typ=fd->ret_type;_tmp5F8.args=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp5F8.c_varargs=fd->c_varargs;_tmp5F8.cyc_varargs=fd->cyc_varargs;_tmp5F8.rgn_po=fd->rgn_po;_tmp5F8.attributes=_tmp5F4;_tmp5F8.requires_clause=fd->requires_clause;_tmp5F8.requires_relns=fd->requires_relns;_tmp5F8.ensures_clause=fd->ensures_clause;_tmp5F8.ensures_relns=fd->ensures_relns;_tmp5F8;});_tmp5F7;});_tmp5F6;});}
# 3158
return(void*)_check_null(fd->cached_typ);}
# 3164
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3166
union Cyc_Relations_RelnOp _tmp5F9=*rop;union Cyc_Relations_RelnOp _tmp5FA=_tmp5F9;struct Cyc_Absyn_Vardecl*_tmp60D;struct Cyc_Absyn_Vardecl*_tmp60C;switch((_tmp5FA.RNumelts).tag){case 2: _LL59E: _tmp60C=(_tmp5FA.RVar).val;_LL59F: {
# 3168
struct _tuple2 _tmp5FB=*_tmp60C->name;struct _tuple2 _tmp5FC=_tmp5FB;union Cyc_Absyn_Nmspace _tmp603;struct _dyneither_ptr*_tmp602;_LL5A5: _tmp603=_tmp5FC.f1;_tmp602=_tmp5FC.f2;_LL5A6:;
if(!((int)((_tmp603.Loc_n).tag == 4)))goto _LL59D;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp602,({const char*_tmp5FD="return_value";_tag_dyneither(_tmp5FD,sizeof(char),13);}))== 0){
*rop=Cyc_Relations_RReturn();
goto _LL59D;}{
# 3174
unsigned int c=0;
{struct Cyc_List_List*_tmp5FE=args;for(0;_tmp5FE != 0;(_tmp5FE=_tmp5FE->tl,c ++)){
struct _tuple10*_tmp5FF=(struct _tuple10*)_tmp5FE->hd;struct _tuple10*_tmp600=_tmp5FF;struct _dyneither_ptr*_tmp601;_LL5A8: _tmp601=_tmp600->f1;_LL5A9:;
if(_tmp601 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp602,(struct _dyneither_ptr)*_tmp601)== 0){
*rop=Cyc_Relations_RParam(c);
break;}}}}
# 3184
goto _LL59D;};}case 3: _LL5A0: _tmp60D=(_tmp5FA.RNumelts).val;_LL5A1: {
# 3186
struct _tuple2 _tmp604=*_tmp60D->name;struct _tuple2 _tmp605=_tmp604;union Cyc_Absyn_Nmspace _tmp60B;struct _dyneither_ptr*_tmp60A;_LL5AB: _tmp60B=_tmp605.f1;_tmp60A=_tmp605.f2;_LL5AC:;
if(!((int)((_tmp60B.Loc_n).tag == 4)))goto _LL59D;{
unsigned int c=0;
{struct Cyc_List_List*_tmp606=args;for(0;_tmp606 != 0;(_tmp606=_tmp606->tl,c ++)){
struct _tuple10*_tmp607=(struct _tuple10*)_tmp606->hd;struct _tuple10*_tmp608=_tmp607;struct _dyneither_ptr*_tmp609;_LL5AE: _tmp609=_tmp608->f1;_LL5AF:;
if(_tmp609 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp60A,(struct _dyneither_ptr)*_tmp609)== 0){
*rop=Cyc_Relations_RParamNumelts(c);
break;}}}}
# 3198
goto _LL59D;};}default: _LL5A2: _LL5A3:
 goto _LL59D;}_LL59D:;}
# 3203
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3205
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3209
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3211
if(e == 0)return 0;{
struct Cyc_List_List*_tmp60E=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp60E);
return _tmp60E;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3218
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp60F=pr;struct Cyc_Absyn_Tqual _tmp612;void*_tmp611;_LL5B1: _tmp612=_tmp60F->f1;_tmp611=_tmp60F->f2;_LL5B2:;
if(_tmp611 == t)return pr;else{
return({struct _tuple12*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->f1=_tmp612;_tmp610->f2=t;_tmp610;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3225
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3227
return({struct _tuple27*_tmp613=_region_malloc(rgn,sizeof(*_tmp613));_tmp613->f1=({struct _tuple26*_tmp614=_region_malloc(rgn,sizeof(*_tmp614));_tmp614->f1=(*y).f1;_tmp614->f2=(*y).f2;_tmp614;});_tmp613->f2=(*y).f3;_tmp613;});}
# 3229
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3231
struct _tuple10 _tmp615=*orig_arg;struct _tuple10 _tmp616=_tmp615;struct _dyneither_ptr*_tmp61A;struct Cyc_Absyn_Tqual _tmp619;void*_tmp618;_LL5B4: _tmp61A=_tmp616.f1;_tmp619=_tmp616.f2;_tmp618=_tmp616.f3;_LL5B5:;
if(t == _tmp618)return orig_arg;
return({struct _tuple10*_tmp617=_cycalloc(sizeof(*_tmp617));_tmp617->f1=_tmp61A;_tmp617->f2=_tmp619;_tmp617->f3=t;_tmp617;});}
# 3235
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3238
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3243
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3245
return({struct Cyc_Absyn_Exp*_tmp61B=_cycalloc(sizeof(*_tmp61B));_tmp61B->topt=old->topt;_tmp61B->r=r;_tmp61B->loc=old->loc;_tmp61B->annot=old->annot;_tmp61B;});}
# 3250
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp61C=e->r;void*_tmp61D=_tmp61C;void*_tmp665;void*_tmp664;struct Cyc_List_List*_tmp663;struct Cyc_Absyn_Exp*_tmp662;void*_tmp661;void*_tmp660;struct Cyc_Absyn_Exp*_tmp65F;int _tmp65E;enum Cyc_Absyn_Coercion _tmp65D;struct Cyc_Absyn_Exp*_tmp65C;struct Cyc_Absyn_Exp*_tmp65B;struct Cyc_Absyn_Exp*_tmp65A;struct Cyc_Absyn_Exp*_tmp659;struct Cyc_Absyn_Exp*_tmp658;struct Cyc_Absyn_Exp*_tmp657;struct Cyc_Absyn_Exp*_tmp656;struct Cyc_Absyn_Exp*_tmp655;struct Cyc_Absyn_Exp*_tmp654;enum Cyc_Absyn_Primop _tmp653;struct Cyc_List_List*_tmp652;switch(*((int*)_tmp61D)){case 0: _LL5B7: _LL5B8:
 goto _LL5BA;case 31: _LL5B9: _LL5BA:
 goto _LL5BC;case 32: _LL5BB: _LL5BC:
 goto _LL5BE;case 1: _LL5BD: _LL5BE:
 return e;case 2: _LL5BF: _tmp653=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_tmp652=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp61D)->f2;_LL5C0:
# 3258
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp652)== 1){
struct Cyc_Absyn_Exp*_tmp61E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp652))->hd;
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Tcutil_rsubsexp(r,inst,_tmp61E);
if(_tmp61F == _tmp61E)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp620=_cycalloc(sizeof(*_tmp620));_tmp620[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp621;_tmp621.tag=2;_tmp621.f1=_tmp653;_tmp621.f2=({struct Cyc_Absyn_Exp*_tmp622[1];_tmp622[0]=_tmp61F;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp622,sizeof(struct Cyc_Absyn_Exp*),1));});_tmp621;});_tmp620;}));}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp652)== 2){
struct Cyc_Absyn_Exp*_tmp623=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp652))->hd;
struct Cyc_Absyn_Exp*_tmp624=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp652->tl))->hd;
struct Cyc_Absyn_Exp*_tmp625=Cyc_Tcutil_rsubsexp(r,inst,_tmp623);
struct Cyc_Absyn_Exp*_tmp626=Cyc_Tcutil_rsubsexp(r,inst,_tmp624);
if(_tmp625 == _tmp623  && _tmp626 == _tmp624)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627[0]=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp628;_tmp628.tag=2;_tmp628.f1=_tmp653;_tmp628.f2=({struct Cyc_Absyn_Exp*_tmp629[2];_tmp629[1]=_tmp626;_tmp629[0]=_tmp625;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp629,sizeof(struct Cyc_Absyn_Exp*),2));});_tmp628;});_tmp627;}));}else{
return({void*_tmp62A=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp62B="primop does not have 1 or 2 args!";_tag_dyneither(_tmp62B,sizeof(char),34);}),_tag_dyneither(_tmp62A,sizeof(void*),0));});}}case 5: _LL5C1: _tmp656=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_tmp655=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp61D)->f2;_tmp654=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp61D)->f3;_LL5C2: {
# 3272
struct Cyc_Absyn_Exp*_tmp62C=Cyc_Tcutil_rsubsexp(r,inst,_tmp656);
struct Cyc_Absyn_Exp*_tmp62D=Cyc_Tcutil_rsubsexp(r,inst,_tmp655);
struct Cyc_Absyn_Exp*_tmp62E=Cyc_Tcutil_rsubsexp(r,inst,_tmp654);
if((_tmp62C == _tmp656  && _tmp62D == _tmp655) && _tmp62E == _tmp654)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F[0]=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp630;_tmp630.tag=5;_tmp630.f1=_tmp62C;_tmp630.f2=_tmp62D;_tmp630.f3=_tmp62E;_tmp630;});_tmp62F;}));}case 6: _LL5C3: _tmp658=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_tmp657=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp61D)->f2;_LL5C4: {
# 3278
struct Cyc_Absyn_Exp*_tmp631=Cyc_Tcutil_rsubsexp(r,inst,_tmp658);
struct Cyc_Absyn_Exp*_tmp632=Cyc_Tcutil_rsubsexp(r,inst,_tmp657);
if(_tmp631 == _tmp658  && _tmp632 == _tmp657)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp633=_cycalloc(sizeof(*_tmp633));_tmp633[0]=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp634;_tmp634.tag=6;_tmp634.f1=_tmp631;_tmp634.f2=_tmp632;_tmp634;});_tmp633;}));}case 7: _LL5C5: _tmp65A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_tmp659=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp61D)->f2;_LL5C6: {
# 3283
struct Cyc_Absyn_Exp*_tmp635=Cyc_Tcutil_rsubsexp(r,inst,_tmp65A);
struct Cyc_Absyn_Exp*_tmp636=Cyc_Tcutil_rsubsexp(r,inst,_tmp659);
if(_tmp635 == _tmp65A  && _tmp636 == _tmp659)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp637=_cycalloc(sizeof(*_tmp637));_tmp637[0]=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp638;_tmp638.tag=7;_tmp638.f1=_tmp635;_tmp638.f2=_tmp636;_tmp638;});_tmp637;}));}case 8: _LL5C7: _tmp65C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_tmp65B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp61D)->f2;_LL5C8: {
# 3288
struct Cyc_Absyn_Exp*_tmp639=Cyc_Tcutil_rsubsexp(r,inst,_tmp65C);
struct Cyc_Absyn_Exp*_tmp63A=Cyc_Tcutil_rsubsexp(r,inst,_tmp65B);
if(_tmp639 == _tmp65C  && _tmp63A == _tmp65B)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp63B=_cycalloc(sizeof(*_tmp63B));_tmp63B[0]=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp63C;_tmp63C.tag=8;_tmp63C.f1=_tmp639;_tmp63C.f2=_tmp63A;_tmp63C;});_tmp63B;}));}case 13: _LL5C9: _tmp660=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_tmp65F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp61D)->f2;_tmp65E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp61D)->f3;_tmp65D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp61D)->f4;_LL5CA: {
# 3293
struct Cyc_Absyn_Exp*_tmp63D=Cyc_Tcutil_rsubsexp(r,inst,_tmp65F);
void*_tmp63E=Cyc_Tcutil_rsubstitute(r,inst,_tmp660);
if(_tmp63D == _tmp65F  && _tmp63E == _tmp660)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F[0]=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp640;_tmp640.tag=13;_tmp640.f1=_tmp63E;_tmp640.f2=_tmp63D;_tmp640.f3=_tmp65E;_tmp640.f4=_tmp65D;_tmp640;});_tmp63F;}));}case 16: _LL5CB: _tmp661=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_LL5CC: {
# 3298
void*_tmp641=Cyc_Tcutil_rsubstitute(r,inst,_tmp661);
if(_tmp641 == _tmp661)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp642=_cycalloc(sizeof(*_tmp642));_tmp642[0]=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp643;_tmp643.tag=16;_tmp643.f1=_tmp641;_tmp643;});_tmp642;}));}case 17: _LL5CD: _tmp662=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_LL5CE: {
# 3302
struct Cyc_Absyn_Exp*_tmp644=Cyc_Tcutil_rsubsexp(r,inst,_tmp662);
if(_tmp644 == _tmp662)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp645=_cycalloc(sizeof(*_tmp645));_tmp645[0]=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp646;_tmp646.tag=17;_tmp646.f1=_tmp644;_tmp646;});_tmp645;}));}case 18: _LL5CF: _tmp664=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_tmp663=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp61D)->f2;_LL5D0: {
# 3306
void*_tmp647=Cyc_Tcutil_rsubstitute(r,inst,_tmp664);
if(_tmp647 == _tmp664)return e;
return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp648=_cycalloc(sizeof(*_tmp648));_tmp648[0]=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp649;_tmp649.tag=18;_tmp649.f1=_tmp647;_tmp649.f2=_tmp663;_tmp649;});_tmp648;}));}case 38: _LL5D1: _tmp665=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp61D)->f1;_LL5D2: {
# 3310
void*_tmp64A=Cyc_Tcutil_rsubstitute(r,inst,_tmp665);
if(_tmp64A == _tmp665)return e;{
# 3313
void*_tmp64B=Cyc_Tcutil_compress(_tmp64A);void*_tmp64C=_tmp64B;struct Cyc_Absyn_Exp*_tmp64F;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp64C)->tag == 18){_LL5D6: _tmp64F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp64C)->f1;_LL5D7:
 return _tmp64F;}else{_LL5D8: _LL5D9:
# 3316
 return Cyc_Tcutil_copye(e,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D[0]=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp64E;_tmp64E.tag=38;_tmp64E.f1=_tmp64A;_tmp64E;});_tmp64D;}));}_LL5D5:;};}default: _LL5D3: _LL5D4:
# 3319
 return({void*_tmp650=0;((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp651="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp651,sizeof(char),46);}),_tag_dyneither(_tmp650,sizeof(void*),0));});}_LL5B6:;}
# 3323
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3326
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3330
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3333
void*_tmp666=f->type;
struct Cyc_Absyn_Exp*_tmp667=f->requires_clause;
void*_tmp668=Cyc_Tcutil_rsubstitute(r,inst,_tmp666);
struct Cyc_Absyn_Exp*_tmp669=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp667);
if(_tmp666 == _tmp668  && _tmp667 == _tmp669)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->name=f->name;_tmp66A->tq=f->tq;_tmp66A->type=_tmp668;_tmp66A->width=f->width;_tmp66A->attributes=f->attributes;_tmp66A->requires_clause=_tmp669;_tmp66A;});}}
# 3343
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3346
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp66B=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp66C=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp66D=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp66B);
struct Cyc_List_List*_tmp66E=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp66C);
if(_tmp66D == _tmp66B  && _tmp66E == _tmp66C)return fs;
# 3353
return({struct Cyc_List_List*_tmp66F=_cycalloc(sizeof(*_tmp66F));_tmp66F->hd=_tmp66D;_tmp66F->tl=_tmp66E;_tmp66F;});};}
# 3356
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3359
struct _tuple1 _tmp670=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp671=_tmp670;struct Cyc_List_List*_tmp675;struct Cyc_List_List*_tmp674;_LL5DB: _tmp675=_tmp671.f1;_tmp674=_tmp671.f2;_LL5DC:;{
struct Cyc_List_List*_tmp672=Cyc_Tcutil_substs(rgn,inst,_tmp675);
struct Cyc_List_List*_tmp673=Cyc_Tcutil_substs(rgn,inst,_tmp674);
if(_tmp672 == _tmp675  && _tmp673 == _tmp674)
return rgn_po;else{
# 3365
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp672,_tmp673);}};}
# 3368
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3371
void*_tmp676=Cyc_Tcutil_compress(t);void*_tmp677=_tmp676;struct Cyc_List_List*_tmp708;void*_tmp707;void*_tmp706;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_Absyn_Exp*_tmp704;void*_tmp703;void*_tmp702;void*_tmp701;void*_tmp700;void*_tmp6FF;enum Cyc_Absyn_AggrKind _tmp6FE;struct Cyc_List_List*_tmp6FD;struct Cyc_List_List*_tmp6FC;struct Cyc_List_List*_tmp6FB;void*_tmp6FA;struct Cyc_Absyn_Tqual _tmp6F9;void*_tmp6F8;struct Cyc_List_List*_tmp6F7;int _tmp6F6;struct Cyc_Absyn_VarargInfo*_tmp6F5;struct Cyc_List_List*_tmp6F4;struct Cyc_List_List*_tmp6F3;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Exp*_tmp6F1;void*_tmp6F0;struct Cyc_Absyn_Tqual _tmp6EF;void*_tmp6EE;union Cyc_Absyn_Constraint*_tmp6ED;union Cyc_Absyn_Constraint*_tmp6EC;union Cyc_Absyn_Constraint*_tmp6EB;void*_tmp6EA;struct Cyc_Absyn_Tqual _tmp6E9;struct Cyc_Absyn_Exp*_tmp6E8;union Cyc_Absyn_Constraint*_tmp6E7;unsigned int _tmp6E6;struct _tuple2*_tmp6E5;struct Cyc_List_List*_tmp6E4;struct Cyc_Absyn_Typedefdecl*_tmp6E3;void*_tmp6E2;union Cyc_Absyn_DatatypeFieldInfoU _tmp6E1;struct Cyc_List_List*_tmp6E0;union Cyc_Absyn_DatatypeInfoU _tmp6DF;struct Cyc_List_List*_tmp6DE;union Cyc_Absyn_AggrInfoU _tmp6DD;struct Cyc_List_List*_tmp6DC;struct Cyc_Absyn_Tvar*_tmp6DB;switch(*((int*)_tmp677)){case 2: _LL5DE: _tmp6DB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp677)->f1;_LL5DF: {
# 3374
struct _handler_cons _tmp678;_push_handler(& _tmp678);{int _tmp67A=0;if(setjmp(_tmp678.handler))_tmp67A=1;if(!_tmp67A){{void*_tmp67B=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6DB);_npop_handler(0);return _tmp67B;};_pop_handler();}else{void*_tmp679=(void*)_exn_thrown;void*_tmp67C=_tmp679;void*_tmp67D;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp67C)->tag == Cyc_Core_Not_found){_LL619: _LL61A:
 return t;}else{_LL61B: _tmp67D=_tmp67C;_LL61C:(int)_rethrow(_tmp67D);}_LL618:;}};}case 11: _LL5E0: _tmp6DD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp677)->f1).aggr_info;_tmp6DC=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp677)->f1).targs;_LL5E1: {
# 3377
struct Cyc_List_List*_tmp67E=Cyc_Tcutil_substs(rgn,inst,_tmp6DC);
return _tmp67E == _tmp6DC?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp67F=_cycalloc(sizeof(*_tmp67F));_tmp67F[0]=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp680;_tmp680.tag=11;_tmp680.f1=({struct Cyc_Absyn_AggrInfo _tmp681;_tmp681.aggr_info=_tmp6DD;_tmp681.targs=_tmp67E;_tmp681;});_tmp680;});_tmp67F;});}case 3: _LL5E2: _tmp6DF=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp677)->f1).datatype_info;_tmp6DE=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp677)->f1).targs;_LL5E3: {
# 3380
struct Cyc_List_List*_tmp682=Cyc_Tcutil_substs(rgn,inst,_tmp6DE);
return _tmp682 == _tmp6DE?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683[0]=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp684;_tmp684.tag=3;_tmp684.f1=({struct Cyc_Absyn_DatatypeInfo _tmp685;_tmp685.datatype_info=_tmp6DF;_tmp685.targs=_tmp682;_tmp685;});_tmp684;});_tmp683;});}case 4: _LL5E4: _tmp6E1=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp677)->f1).field_info;_tmp6E0=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp677)->f1).targs;_LL5E5: {
# 3384
struct Cyc_List_List*_tmp686=Cyc_Tcutil_substs(rgn,inst,_tmp6E0);
return _tmp686 == _tmp6E0?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp687=_cycalloc(sizeof(*_tmp687));_tmp687[0]=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp688;_tmp688.tag=4;_tmp688.f1=({struct Cyc_Absyn_DatatypeFieldInfo _tmp689;_tmp689.field_info=_tmp6E1;_tmp689.targs=_tmp686;_tmp689;});_tmp688;});_tmp687;});}case 17: _LL5E6: _tmp6E5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp677)->f1;_tmp6E4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp677)->f2;_tmp6E3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp677)->f3;_tmp6E2=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp677)->f4;_LL5E7: {
# 3388
struct Cyc_List_List*_tmp68A=Cyc_Tcutil_substs(rgn,inst,_tmp6E4);
return _tmp68A == _tmp6E4?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp68B=_cycalloc(sizeof(*_tmp68B));_tmp68B[0]=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp68C;_tmp68C.tag=17;_tmp68C.f1=_tmp6E5;_tmp68C.f2=_tmp68A;_tmp68C.f3=_tmp6E3;_tmp68C.f4=_tmp6E2;_tmp68C;});_tmp68B;});}case 8: _LL5E8: _tmp6EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp677)->f1).elt_type;_tmp6E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp677)->f1).tq;_tmp6E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp677)->f1).num_elts;_tmp6E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp677)->f1).zero_term;_tmp6E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp677)->f1).zt_loc;_LL5E9: {
# 3391
void*_tmp68D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EA);
struct Cyc_Absyn_Exp*_tmp68E=_tmp6E8 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6E8);
return _tmp68D == _tmp6EA  && _tmp68E == _tmp6E8?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp68F=_cycalloc(sizeof(*_tmp68F));_tmp68F[0]=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp690;_tmp690.tag=8;_tmp690.f1=({struct Cyc_Absyn_ArrayInfo _tmp691;_tmp691.elt_type=_tmp68D;_tmp691.tq=_tmp6E9;_tmp691.num_elts=_tmp68E;_tmp691.zero_term=_tmp6E7;_tmp691.zt_loc=_tmp6E6;_tmp691;});_tmp690;});_tmp68F;});}case 5: _LL5EA: _tmp6F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp677)->f1).elt_typ;_tmp6EF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp677)->f1).elt_tq;_tmp6EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp677)->f1).ptr_atts).rgn;_tmp6ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp677)->f1).ptr_atts).nullable;_tmp6EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp677)->f1).ptr_atts).bounds;_tmp6EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp677)->f1).ptr_atts).zero_term;_LL5EB: {
# 3396
void*_tmp692=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F0);
void*_tmp693=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EE);
union Cyc_Absyn_Constraint*_tmp694=_tmp6EC;
{void*_tmp695=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6EC);void*_tmp696=_tmp695;struct Cyc_Absyn_Exp*_tmp69A;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp696)->tag == 1){_LL61E: _tmp69A=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp696)->f1;_LL61F: {
# 3401
struct Cyc_Absyn_Exp*_tmp697=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp69A);
if(_tmp697 != _tmp69A)
_tmp694=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp698=_cycalloc(sizeof(*_tmp698));_tmp698[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp699;_tmp699.tag=1;_tmp699.f1=_tmp697;_tmp699;});_tmp698;}));
goto _LL61D;}}else{_LL620: _LL621:
 goto _LL61D;}_LL61D:;}
# 3407
if((_tmp692 == _tmp6F0  && _tmp693 == _tmp6EE) && _tmp694 == _tmp6EC)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp69B=_cycalloc(sizeof(*_tmp69B));_tmp69B[0]=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp69C;_tmp69C.tag=5;_tmp69C.f1=({struct Cyc_Absyn_PtrInfo _tmp69D;_tmp69D.elt_typ=_tmp692;_tmp69D.elt_tq=_tmp6EF;_tmp69D.ptr_atts=({(_tmp69D.ptr_atts).rgn=_tmp693;(_tmp69D.ptr_atts).nullable=_tmp6ED;(_tmp69D.ptr_atts).bounds=_tmp694;(_tmp69D.ptr_atts).zero_term=_tmp6EB;(_tmp69D.ptr_atts).ptrloc=0;_tmp69D.ptr_atts;});_tmp69D;});_tmp69C;});_tmp69B;});}case 9: _LL5EC: _tmp6FB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).tvars;_tmp6FA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).effect;_tmp6F9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).ret_tqual;_tmp6F8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).ret_typ;_tmp6F7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).args;_tmp6F6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).c_varargs;_tmp6F5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).cyc_varargs;_tmp6F4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).rgn_po;_tmp6F3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).attributes;_tmp6F2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).requires_clause;_tmp6F1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp677)->f1).ensures_clause;_LL5ED:
# 3413
{struct Cyc_List_List*_tmp69E=_tmp6FB;for(0;_tmp69E != 0;_tmp69E=_tmp69E->tl){
inst=({struct Cyc_List_List*_tmp69F=_region_malloc(rgn,sizeof(*_tmp69F));_tmp69F->hd=({struct _tuple16*_tmp6A0=_region_malloc(rgn,sizeof(*_tmp6A0));_tmp6A0->f1=(struct Cyc_Absyn_Tvar*)_tmp69E->hd;_tmp6A0->f2=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));_tmp6A1[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp6A2;_tmp6A2.tag=2;_tmp6A2.f1=(struct Cyc_Absyn_Tvar*)_tmp69E->hd;_tmp6A2;});_tmp6A1;});_tmp6A0;});_tmp69F->tl=inst;_tmp69F;});}}{
struct _tuple1 _tmp6A3=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6F7));
# 3415
struct _tuple1 _tmp6A4=_tmp6A3;struct Cyc_List_List*_tmp6B5;struct Cyc_List_List*_tmp6B4;_LL623: _tmp6B5=_tmp6A4.f1;_tmp6B4=_tmp6A4.f2;_LL624:;{
# 3417
struct Cyc_List_List*_tmp6A5=_tmp6F7;
struct Cyc_List_List*_tmp6A6=Cyc_Tcutil_substs(rgn,inst,_tmp6B4);
if(_tmp6A6 != _tmp6B4)
_tmp6A5=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp6F7,_tmp6A6);{
void*eff2;
if(_tmp6FA == 0)
eff2=0;else{
# 3425
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FA);
if(new_eff == _tmp6FA)
eff2=_tmp6FA;else{
# 3429
eff2=new_eff;}}{
# 3431
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp6F5 == 0)
cyc_varargs2=0;else{
# 3435
struct Cyc_Absyn_VarargInfo _tmp6A7=*_tmp6F5;struct Cyc_Absyn_VarargInfo _tmp6A8=_tmp6A7;struct _dyneither_ptr*_tmp6AE;struct Cyc_Absyn_Tqual _tmp6AD;void*_tmp6AC;int _tmp6AB;_LL626: _tmp6AE=_tmp6A8.name;_tmp6AD=_tmp6A8.tq;_tmp6AC=_tmp6A8.type;_tmp6AB=_tmp6A8.inject;_LL627:;{
void*_tmp6A9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6AC);
if(_tmp6A9 == _tmp6AC)cyc_varargs2=_tmp6F5;else{
# 3439
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp6AA=_cycalloc(sizeof(*_tmp6AA));_tmp6AA->name=_tmp6AE;_tmp6AA->tq=_tmp6AD;_tmp6AA->type=_tmp6A9;_tmp6AA->inject=_tmp6AB;_tmp6AA;});}};}{
# 3441
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp6F4);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6F2);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6F1);
struct Cyc_List_List*_tmp6AF=Cyc_Tcutil_extract_relns(_tmp6A5,req2);
struct Cyc_List_List*_tmp6B0=Cyc_Tcutil_extract_relns(_tmp6A5,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1[0]=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6B2;_tmp6B2.tag=9;_tmp6B2.f1=({struct Cyc_Absyn_FnInfo _tmp6B3;_tmp6B3.tvars=_tmp6FB;_tmp6B3.effect=eff2;_tmp6B3.ret_tqual=_tmp6F9;_tmp6B3.ret_typ=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6F8);_tmp6B3.args=_tmp6A5;_tmp6B3.c_varargs=_tmp6F6;_tmp6B3.cyc_varargs=cyc_varargs2;_tmp6B3.rgn_po=rgn_po2;_tmp6B3.attributes=_tmp6F3;_tmp6B3.requires_clause=req2;_tmp6B3.requires_relns=_tmp6AF;_tmp6B3.ensures_clause=ens2;_tmp6B3.ensures_relns=_tmp6B0;_tmp6B3;});_tmp6B2;});_tmp6B1;});};};};};};case 10: _LL5EE: _tmp6FC=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp677)->f1;_LL5EF: {
# 3451
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp6B6=_tmp6FC;for(0;_tmp6B6 != 0;_tmp6B6=_tmp6B6->tl){
void*_tmp6B7=(*((struct _tuple12*)_tmp6B6->hd)).f2;
void*_tmp6B8=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6B7);
if(_tmp6B7 != _tmp6B8)
change=1;
# 3459
ts2=({struct Cyc_List_List*_tmp6B9=_region_malloc(rgn,sizeof(*_tmp6B9));_tmp6B9->hd=_tmp6B8;_tmp6B9->tl=ts2;_tmp6B9;});}}
# 3461
if(!change)
return t;{
struct Cyc_List_List*_tmp6BA=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp6FC,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6BB=_cycalloc(sizeof(*_tmp6BB));_tmp6BB[0]=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6BC;_tmp6BC.tag=10;_tmp6BC.f1=_tmp6BA;_tmp6BC;});_tmp6BB;});};}case 12: _LL5F0: _tmp6FE=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp677)->f1;_tmp6FD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp677)->f2;_LL5F1: {
# 3466
struct Cyc_List_List*_tmp6BD=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp6FD);
if(_tmp6FD == _tmp6BD)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6BE=_cycalloc(sizeof(*_tmp6BE));_tmp6BE[0]=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6BF;_tmp6BF.tag=12;_tmp6BF.f1=_tmp6FE;_tmp6BF.f2=_tmp6BD;_tmp6BF;});_tmp6BE;});}case 1: _LL5F2: _tmp6FF=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp677)->f2;_LL5F3:
# 3470
 if(_tmp6FF != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6FF);else{
return t;}case 15: _LL5F4: _tmp700=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp677)->f1;_LL5F5: {
# 3473
void*_tmp6C0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp700);
return _tmp6C0 == _tmp700?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6C1=_cycalloc(sizeof(*_tmp6C1));_tmp6C1[0]=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6C2;_tmp6C2.tag=15;_tmp6C2.f1=_tmp6C0;_tmp6C2;});_tmp6C1;});}case 16: _LL5F6: _tmp702=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp677)->f1;_tmp701=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp677)->f2;_LL5F7: {
# 3476
void*_tmp6C3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp702);
void*_tmp6C4=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp701);
return _tmp6C3 == _tmp702  && _tmp6C4 == _tmp701?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6C5=_cycalloc(sizeof(*_tmp6C5));_tmp6C5[0]=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp6C6;_tmp6C6.tag=16;_tmp6C6.f1=_tmp6C3;_tmp6C6.f2=_tmp6C4;_tmp6C6;});_tmp6C5;});}case 19: _LL5F8: _tmp703=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp677)->f1;_LL5F9: {
# 3480
void*_tmp6C7=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp703);
return _tmp6C7 == _tmp703?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6C8=_cycalloc(sizeof(*_tmp6C8));_tmp6C8[0]=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6C9;_tmp6C9.tag=19;_tmp6C9.f1=_tmp6C7;_tmp6C9;});_tmp6C8;});}case 18: _LL5FA: _tmp704=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp677)->f1;_LL5FB: {
# 3483
struct Cyc_Absyn_Exp*_tmp6CA=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp704);
return _tmp6CA == _tmp704?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6CB=_cycalloc(sizeof(*_tmp6CB));_tmp6CB[0]=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6CC;_tmp6CC.tag=18;_tmp6CC.f1=_tmp6CA;_tmp6CC;});_tmp6CB;});}case 27: _LL5FC: _tmp705=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp677)->f1;_LL5FD: {
# 3486
struct Cyc_Absyn_Exp*_tmp6CD=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp705);
return _tmp6CD == _tmp705?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp6CE=_cycalloc(sizeof(*_tmp6CE));_tmp6CE[0]=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp6CF;_tmp6CF.tag=27;_tmp6CF.f1=_tmp6CD;_tmp6CF;});_tmp6CE;});}case 13: _LL5FE: _LL5FF:
 goto _LL601;case 14: _LL600: _LL601:
 goto _LL603;case 0: _LL602: _LL603:
 goto _LL605;case 6: _LL604: _LL605:
 goto _LL607;case 7: _LL606: _LL607:
 goto _LL609;case 22: _LL608: _LL609:
 goto _LL60B;case 21: _LL60A: _LL60B:
 goto _LL60D;case 28: _LL60C: _LL60D:
 goto _LL60F;case 20: _LL60E: _LL60F:
 return t;case 25: _LL610: _tmp706=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp677)->f1;_LL611: {
# 3498
void*_tmp6D0=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp706);
return _tmp6D0 == _tmp706?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6D1=_cycalloc(sizeof(*_tmp6D1));_tmp6D1[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6D2;_tmp6D2.tag=25;_tmp6D2.f1=_tmp6D0;_tmp6D2;});_tmp6D1;});}case 23: _LL612: _tmp707=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp677)->f1;_LL613: {
# 3501
void*_tmp6D3=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp707);
return _tmp6D3 == _tmp707?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6D4=_cycalloc(sizeof(*_tmp6D4));_tmp6D4[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp6D5;_tmp6D5.tag=23;_tmp6D5.f1=_tmp6D3;_tmp6D5;});_tmp6D4;});}case 24: _LL614: _tmp708=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp677)->f1;_LL615: {
# 3504
struct Cyc_List_List*_tmp6D6=Cyc_Tcutil_substs(rgn,inst,_tmp708);
return _tmp6D6 == _tmp708?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6D7=_cycalloc(sizeof(*_tmp6D7));_tmp6D7[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6D8;_tmp6D8.tag=24;_tmp6D8.f1=_tmp6D6;_tmp6D8;});_tmp6D7;});}default: _LL616: _LL617:
({void*_tmp6D9=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp6DA="found typedecltype in rsubs";_tag_dyneither(_tmp6DA,sizeof(char),28);}),_tag_dyneither(_tmp6D9,sizeof(void*),0));});}_LL5DD:;}
# 3510
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3513
if(ts == 0)
return 0;{
void*_tmp709=(void*)ts->hd;
struct Cyc_List_List*_tmp70A=ts->tl;
void*_tmp70B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp709);
struct Cyc_List_List*_tmp70C=Cyc_Tcutil_substs(rgn,inst,_tmp70A);
if(_tmp709 == _tmp70B  && _tmp70A == _tmp70C)
return ts;
return({struct Cyc_List_List*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D->hd=_tmp70B;_tmp70D->tl=_tmp70C;_tmp70D;});};}
# 3524
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3531
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp70E=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp70F=_cycalloc(sizeof(*_tmp70F));_tmp70F->f1=tv;_tmp70F->f2=Cyc_Absyn_new_evar(_tmp70E,({struct Cyc_Core_Opt*_tmp710=_cycalloc(sizeof(*_tmp710));_tmp710->v=s;_tmp710;}));_tmp70F;});}
# 3536
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3538
struct _tuple17*_tmp711=env;struct Cyc_List_List*_tmp716;struct _RegionHandle*_tmp715;_LL629: _tmp716=_tmp711->f1;_tmp715=_tmp711->f2;_LL62A:;{
struct Cyc_Core_Opt*_tmp712=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp713=_region_malloc(_tmp715,sizeof(*_tmp713));_tmp713->f1=tv;_tmp713->f2=Cyc_Absyn_new_evar(_tmp712,({struct Cyc_Core_Opt*_tmp714=_cycalloc(sizeof(*_tmp714));_tmp714->v=_tmp716;_tmp714;}));_tmp713;});};}
# 3548
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3552
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp71B;_tmp71B.tag=0;_tmp71B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0;_tmp71A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));({struct Cyc_String_pa_PrintArg_struct _tmp719;_tmp719.tag=0;_tmp719.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp717[3]={& _tmp719,& _tmp71A,& _tmp71B};Cyc_Tcutil_terr(loc,({const char*_tmp718="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp718,sizeof(char),59);}),_tag_dyneither(_tmp717,sizeof(void*),3));});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp71C=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp71D="same type variable has different identity!";_tag_dyneither(_tmp71D,sizeof(char),43);}),_tag_dyneither(_tmp71C,sizeof(void*),0));});}
return tvs;}}}
# 3566
tv->identity=Cyc_Tcutil_new_tvar_id();
return({struct Cyc_List_List*_tmp71E=_cycalloc(sizeof(*_tmp71E));_tmp71E->hd=tv;_tmp71E->tl=tvs;_tmp71E;});}
# 3572
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3574
if(tv->identity == - 1)
({void*_tmp71F=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp720="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp720,sizeof(char),39);}),_tag_dyneither(_tmp71F,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3578
struct Cyc_Absyn_Tvar*_tmp721=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp721->identity == - 1)
({void*_tmp722=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp723="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp723,sizeof(char),41);}),_tag_dyneither(_tmp722,sizeof(void*),0));});
if(_tmp721->identity == tv->identity)
return tvs;}}
# 3585
return({struct Cyc_List_List*_tmp724=_cycalloc(sizeof(*_tmp724));_tmp724->hd=tv;_tmp724->tl=tvs;_tmp724;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3591
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3596
if(tv->identity == - 1)
({void*_tmp725=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp726="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp726,sizeof(char),44);}),_tag_dyneither(_tmp725,sizeof(void*),0));});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3600
struct _tuple28*_tmp727=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp728=_tmp727;struct Cyc_Absyn_Tvar*_tmp72C;int*_tmp72B;_LL62C: _tmp72C=_tmp728->f1;_tmp72B=(int*)& _tmp728->f2;_LL62D:;
if(_tmp72C->identity == - 1)
({void*_tmp729=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp72A="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp72A,sizeof(char),46);}),_tag_dyneither(_tmp729,sizeof(void*),0));});
if(_tmp72C->identity == tv->identity){
*_tmp72B=*_tmp72B  || b;
return tvs;}}}
# 3608
return({struct Cyc_List_List*_tmp72D=_region_malloc(r,sizeof(*_tmp72D));_tmp72D->hd=({struct _tuple28*_tmp72E=_region_malloc(r,sizeof(*_tmp72E));_tmp72E->f1=tv;_tmp72E->f2=b;_tmp72E;});_tmp72D->tl=tvs;_tmp72D;});}
# 3612
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3614
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp731;_tmp731.tag=0;_tmp731.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string(tv));({void*_tmp72F[1]={& _tmp731};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp730="bound tvar id for %s is NULL";_tag_dyneither(_tmp730,sizeof(char),29);}),_tag_dyneither(_tmp72F,sizeof(void*),1));});});
return({struct Cyc_List_List*_tmp732=_cycalloc(sizeof(*_tmp732));_tmp732->hd=tv;_tmp732->tl=tvs;_tmp732;});}struct _tuple29{void*f1;int f2;};
# 3623
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3626
void*_tmp733=Cyc_Tcutil_compress(e);void*_tmp734=_tmp733;int _tmp73E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp734)->tag == 1){_LL62F: _tmp73E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp734)->f3;_LL630:
# 3628
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp735=(struct _tuple29*)es2->hd;struct _tuple29*_tmp736=_tmp735;void*_tmp73B;int*_tmp73A;_LL634: _tmp73B=_tmp736->f1;_tmp73A=(int*)& _tmp736->f2;_LL635:;{
void*_tmp737=Cyc_Tcutil_compress(_tmp73B);void*_tmp738=_tmp737;int _tmp739;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp738)->tag == 1){_LL637: _tmp739=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp738)->f3;_LL638:
# 3632
 if(_tmp73E == _tmp739){
if(b != *_tmp73A)*_tmp73A=1;
return es;}
# 3636
goto _LL636;}else{_LL639: _LL63A:
 goto _LL636;}_LL636:;};}}
# 3640
return({struct Cyc_List_List*_tmp73C=_region_malloc(r,sizeof(*_tmp73C));_tmp73C->hd=({struct _tuple29*_tmp73D=_region_malloc(r,sizeof(*_tmp73D));_tmp73D->f1=e;_tmp73D->f2=b;_tmp73D;});_tmp73C->tl=es;_tmp73C;});}else{_LL631: _LL632:
 return es;}_LL62E:;}
# 3645
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3648
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3657
if(!present)r=({struct Cyc_List_List*_tmp73F=_region_malloc(rgn,sizeof(*_tmp73F));_tmp73F->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp73F->tl=r;_tmp73F;});}
# 3659
r=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);
return r;}
# 3664
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3668
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp740=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp741=_tmp740;struct Cyc_Absyn_Tvar*_tmp744;int _tmp743;_LL63C: _tmp744=_tmp741.f1;_tmp743=_tmp741.f2;_LL63D:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp744->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3678
if(!present)res=({struct Cyc_List_List*_tmp742=_region_malloc(r,sizeof(*_tmp742));_tmp742->hd=(struct _tuple28*)tvs->hd;_tmp742->tl=res;_tmp742;});};}
# 3680
res=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);
return res;}
# 3684
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3686
if(width != 0){
unsigned int w=0;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp747;_tmp747.tag=0;_tmp747.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp745[1]={& _tmp747};Cyc_Tcutil_terr(loc,({const char*_tmp746="bitfield %s does not have constant width";_tag_dyneither(_tmp746,sizeof(char),41);}),_tag_dyneither(_tmp745,sizeof(void*),1));});});else{
# 3691
struct _tuple14 _tmp748=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp749=_tmp748;unsigned int _tmp74D;int _tmp74C;_LL63F: _tmp74D=_tmp749.f1;_tmp74C=_tmp749.f2;_LL640:;
if(!_tmp74C)
({void*_tmp74A=0;Cyc_Tcutil_terr(loc,({const char*_tmp74B="bitfield width cannot use sizeof or offsetof";_tag_dyneither(_tmp74B,sizeof(char),45);}),_tag_dyneither(_tmp74A,sizeof(void*),0));});
w=_tmp74D;}{
# 3696
void*_tmp74E=Cyc_Tcutil_compress(field_typ);void*_tmp74F=_tmp74E;enum Cyc_Absyn_Size_of _tmp75D;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74F)->tag == 6){_LL642: _tmp75D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp74F)->f2;_LL643:
# 3699
{enum Cyc_Absyn_Size_of _tmp750=_tmp75D;switch(_tmp750){case Cyc_Absyn_Char_sz: _LL647: _LL648:
 if(w > 8)({void*_tmp751=0;Cyc_Tcutil_terr(loc,({const char*_tmp752="bitfield larger than type";_tag_dyneither(_tmp752,sizeof(char),26);}),_tag_dyneither(_tmp751,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Short_sz: _LL649: _LL64A:
 if(w > 16)({void*_tmp753=0;Cyc_Tcutil_terr(loc,({const char*_tmp754="bitfield larger than type";_tag_dyneither(_tmp754,sizeof(char),26);}),_tag_dyneither(_tmp753,sizeof(void*),0));});goto _LL646;case Cyc_Absyn_Long_sz: _LL64B: _LL64C:
 goto _LL64E;case Cyc_Absyn_Int_sz: _LL64D: _LL64E:
# 3704
 if(w > 32)({void*_tmp755=0;Cyc_Tcutil_terr(loc,({const char*_tmp756="bitfield larger than type";_tag_dyneither(_tmp756,sizeof(char),26);}),_tag_dyneither(_tmp755,sizeof(void*),0));});goto _LL646;default: _LL64F: _LL650:
# 3706
 if(w > 64)({void*_tmp757=0;Cyc_Tcutil_terr(loc,({const char*_tmp758="bitfield larger than type";_tag_dyneither(_tmp758,sizeof(char),26);}),_tag_dyneither(_tmp757,sizeof(void*),0));});goto _LL646;}_LL646:;}
# 3708
goto _LL641;}else{_LL644: _LL645:
# 3710
({struct Cyc_String_pa_PrintArg_struct _tmp75C;_tmp75C.tag=0;_tmp75C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));({struct Cyc_String_pa_PrintArg_struct _tmp75B;_tmp75B.tag=0;_tmp75B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp759[2]={& _tmp75B,& _tmp75C};Cyc_Tcutil_terr(loc,({const char*_tmp75A="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp75A,sizeof(char),52);}),_tag_dyneither(_tmp759,sizeof(void*),2));});});});
goto _LL641;}_LL641:;};}}
# 3717
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp75E=(void*)atts->hd;void*_tmp75F=_tmp75E;switch(*((int*)_tmp75F)){case 7: _LL652: _LL653:
 continue;case 6: _LL654: _LL655:
 continue;default: _LL656: _LL657:
({struct Cyc_String_pa_PrintArg_struct _tmp763;_tmp763.tag=0;_tmp763.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp762;_tmp762.tag=0;_tmp762.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));({void*_tmp760[2]={& _tmp762,& _tmp763};Cyc_Tcutil_terr(loc,({const char*_tmp761="bad attribute %s on member %s";_tag_dyneither(_tmp761,sizeof(char),30);}),_tag_dyneither(_tmp760,sizeof(void*),2));});});});}_LL651:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3741
typedef struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_cvtenv_t;
# 3745
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp764=t;struct Cyc_Absyn_Typedefdecl*_tmp768;void*_tmp767;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp764)->tag == 17){_LL659: _tmp768=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp764)->f3;_tmp767=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp764)->f4;_LL65A:
# 3748
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp768))->tq).real_const  || (_tmp768->tq).print_const){
if(declared_const)({void*_tmp765=0;Cyc_Tcutil_warn(loc,({const char*_tmp766="extra const";_tag_dyneither(_tmp766,sizeof(char),12);}),_tag_dyneither(_tmp765,sizeof(void*),0));});
return 1;}
# 3753
if((unsigned int)_tmp767)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp767);else{
return declared_const;}}else{_LL65B: _LL65C:
 return declared_const;}_LL658:;}
# 3760
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp769=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp76A=_tmp769;void*_tmp76E;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76A)->tag == 5){_LL65E: _tmp76E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp76A)->f1).ptr_atts).rgn;_LL65F:
# 3765
{void*_tmp76B=Cyc_Tcutil_compress(_tmp76E);void*_tmp76C=_tmp76B;struct Cyc_Absyn_Tvar*_tmp76D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp76C)->tag == 2){_LL663: _tmp76D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp76C)->f1;_LL664:
# 3767
 return Cyc_Absyn_tvar_cmp(tvar,_tmp76D)== 0;}else{_LL665: _LL666:
 goto _LL662;}_LL662:;}
# 3770
goto _LL65D;}else{_LL660: _LL661:
 goto _LL65D;}_LL65D:;}}else{
# 3776
return 1;}
return 0;}
# 3780
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3783
void*_tmp76F=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp770=_tmp76F;switch(*((int*)_tmp770)){case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp770)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp770)->f2)->aliasqual == Cyc_Absyn_Top){_LL668: _LL669:
 goto _LL66B;}else{goto _LL66C;}}else{goto _LL66C;}case 0: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp770)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp770)->f1)->aliasqual == Cyc_Absyn_Top){_LL66A: _LL66B:
# 3792
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3795
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3801
return& Cyc_Tcutil_trk;}else{goto _LL66C;}}else{goto _LL66C;}default: _LL66C: _LL66D:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL667:;}
# 3807
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3811
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3814
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp774;_tmp774.tag=0;_tmp774.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));({struct Cyc_String_pa_PrintArg_struct _tmp773;_tmp773.tag=0;_tmp773.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp771[2]={& _tmp773,& _tmp774};Cyc_Tcutil_terr(loc,({const char*_tmp772="%s clause has type %s instead of integral type";_tag_dyneither(_tmp772,sizeof(char),47);}),_tag_dyneither(_tmp771,sizeof(void*),2));});});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);
relns=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp778;_tmp778.tag=0;_tmp778.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));({struct Cyc_String_pa_PrintArg_struct _tmp777;_tmp777.tag=0;_tmp777.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp775[2]={& _tmp777,& _tmp778};Cyc_Tcutil_terr(clause->loc,({const char*_tmp776="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp776,sizeof(char),36);}),_tag_dyneither(_tmp775,sizeof(void*),2));});});});}
# 3826
return({struct _tuple30 _tmp779;_tmp779.f1=cvtenv;_tmp779.f2=relns;_tmp779;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3857 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3865
{void*_tmp77A=Cyc_Tcutil_compress(t);void*_tmp77B=_tmp77A;struct Cyc_List_List*_tmp94B;void*_tmp94A;void*_tmp949;void*_tmp948;void*_tmp947;void*_tmp946;struct _tuple2*_tmp945;struct Cyc_List_List**_tmp944;struct Cyc_Absyn_Typedefdecl**_tmp943;void**_tmp942;union Cyc_Absyn_AggrInfoU*_tmp941;struct Cyc_List_List**_tmp940;enum Cyc_Absyn_AggrKind _tmp93F;struct Cyc_List_List*_tmp93E;struct Cyc_List_List*_tmp93D;struct Cyc_List_List**_tmp93C;void**_tmp93B;struct Cyc_Absyn_Tqual*_tmp93A;void*_tmp939;struct Cyc_List_List*_tmp938;int _tmp937;struct Cyc_Absyn_VarargInfo*_tmp936;struct Cyc_List_List*_tmp935;struct Cyc_List_List*_tmp934;struct Cyc_Absyn_Exp*_tmp933;struct Cyc_List_List**_tmp932;struct Cyc_Absyn_Exp*_tmp931;struct Cyc_List_List**_tmp930;void*_tmp92F;struct Cyc_Absyn_Tqual*_tmp92E;struct Cyc_Absyn_Exp**_tmp92D;union Cyc_Absyn_Constraint*_tmp92C;unsigned int _tmp92B;struct Cyc_Absyn_Exp*_tmp92A;struct Cyc_Absyn_Exp*_tmp929;void*_tmp928;void*_tmp927;struct Cyc_Absyn_Tqual*_tmp926;void*_tmp925;union Cyc_Absyn_Constraint*_tmp924;union Cyc_Absyn_Constraint*_tmp923;union Cyc_Absyn_Constraint*_tmp922;union Cyc_Absyn_DatatypeFieldInfoU*_tmp921;struct Cyc_List_List*_tmp920;union Cyc_Absyn_DatatypeInfoU*_tmp91F;struct Cyc_List_List**_tmp91E;struct _tuple2*_tmp91D;struct Cyc_Absyn_Enumdecl**_tmp91C;struct Cyc_List_List*_tmp91B;void*_tmp91A;void***_tmp919;struct Cyc_Absyn_Tvar*_tmp918;struct Cyc_Core_Opt**_tmp917;void**_tmp916;switch(*((int*)_tmp77B)){case 0: _LL66F: _LL670:
 goto _LL66E;case 1: _LL671: _tmp917=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp77B)->f1;_tmp916=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp77B)->f2;_LL672:
# 3869
 if(*_tmp917 == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp917))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp917))->v,expected_kind))
*_tmp917=Cyc_Tcutil_kind_to_opt(expected_kind);
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3874
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp916=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3877
*_tmp916=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp77C=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp77F=_cycalloc(sizeof(*_tmp77F));_tmp77F[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp780;_tmp780.tag=2;_tmp780.f1=0;_tmp780.f2=expected_kind;_tmp780;});_tmp77F;}));
*_tmp916=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp77E;_tmp77E.tag=2;_tmp77E.f1=_tmp77C;_tmp77E;});_tmp77D;});
_tmp918=_tmp77C;goto _LL674;}else{
# 3883
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);}}
# 3885
goto _LL66E;case 2: _LL673: _tmp918=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp77B)->f1;_LL674:
# 3887
{void*_tmp781=Cyc_Absyn_compress_kb(_tmp918->kind);void*_tmp782=_tmp781;struct Cyc_Core_Opt**_tmp786;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp782)->tag == 1){_LL6AA: _tmp786=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp782)->f1;_LL6AB:
# 3889
*_tmp786=({struct Cyc_Core_Opt*_tmp783=_cycalloc(sizeof(*_tmp783));_tmp783->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp784=_cycalloc(sizeof(*_tmp784));_tmp784[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp785;_tmp785.tag=2;_tmp785.f1=0;_tmp785.f2=expected_kind;_tmp785;});_tmp784;});_tmp783;});goto _LL6A9;}else{_LL6AC: _LL6AD:
 goto _LL6A9;}_LL6A9:;}
# 3894
cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp918);
# 3897
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp918,put_in_effect);
# 3899
{void*_tmp787=Cyc_Absyn_compress_kb(_tmp918->kind);void*_tmp788=_tmp787;struct Cyc_Core_Opt**_tmp78D;struct Cyc_Absyn_Kind*_tmp78C;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp788)->tag == 2){_LL6AF: _tmp78D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp788)->f1;_tmp78C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp788)->f2;_LL6B0:
# 3901
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp78C))
*_tmp78D=({struct Cyc_Core_Opt*_tmp789=_cycalloc(sizeof(*_tmp789));_tmp789->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp78A=_cycalloc(sizeof(*_tmp78A));_tmp78A[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp78B;_tmp78B.tag=2;_tmp78B.f1=0;_tmp78B.f2=expected_kind;_tmp78B;});_tmp78A;});_tmp789;});
goto _LL6AE;}else{_LL6B1: _LL6B2:
 goto _LL6AE;}_LL6AE:;}
# 3906
goto _LL66E;case 26: _LL675: _tmp91A=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp77B)->f1)->r;_tmp919=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp77B)->f2;_LL676: {
# 3912
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp78E=_tmp91A;struct Cyc_Absyn_Datatypedecl*_tmp791;struct Cyc_Absyn_Enumdecl*_tmp790;struct Cyc_Absyn_Aggrdecl*_tmp78F;switch(*((int*)_tmp78E)){case 0: _LL6B4: _tmp78F=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp78E)->f1;_LL6B5:
# 3915
 if(te->in_extern_c_include)
_tmp78F->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp78F);goto _LL6B3;case 1: _LL6B6: _tmp790=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp78E)->f1;_LL6B7:
# 3919
 if(te->in_extern_c_include)
_tmp790->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp790);goto _LL6B3;default: _LL6B8: _tmp791=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp78E)->f1;_LL6B9:
# 3923
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp791);goto _LL6B3;}_LL6B3:;}
# 3925
*_tmp919=({void**_tmp792=_cycalloc(sizeof(*_tmp792));_tmp792[0]=new_t;_tmp792;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14: _LL677: _tmp91B=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp77B)->f1;_LL678: {
# 3931
struct _RegionHandle _tmp793=_new_region("uprev_rgn");struct _RegionHandle*uprev_rgn=& _tmp793;_push_region(uprev_rgn);{
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0;
for(0;_tmp91B != 0;_tmp91B=_tmp91B->tl){
struct Cyc_Absyn_Enumfield*_tmp794=(struct Cyc_Absyn_Enumfield*)_tmp91B->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp794->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp797;_tmp797.tag=0;_tmp797.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp794->name).f2);({void*_tmp795[1]={& _tmp797};Cyc_Tcutil_terr(_tmp794->loc,({const char*_tmp796="duplicate enum field name %s";_tag_dyneither(_tmp796,sizeof(char),29);}),_tag_dyneither(_tmp795,sizeof(void*),1));});});else{
# 3939
prev_fields=({struct Cyc_List_List*_tmp798=_region_malloc(uprev_rgn,sizeof(*_tmp798));_tmp798->hd=(*_tmp794->name).f2;_tmp798->tl=prev_fields;_tmp798;});}
# 3941
if(_tmp794->tag == 0)
_tmp794->tag=Cyc_Absyn_uint_exp(tag_count,_tmp794->loc);else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp794->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp79B;_tmp79B.tag=0;_tmp79B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp794->name).f2);({void*_tmp799[1]={& _tmp79B};Cyc_Tcutil_terr(loc,({const char*_tmp79A="enum field %s: expression is not constant";_tag_dyneither(_tmp79A,sizeof(char),42);}),_tag_dyneither(_tmp799,sizeof(void*),1));});});}{
# 3946
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp794->tag))).f1;
tag_count=t1 + 1;
(te->ae)->ordinaries=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple2*k,struct _tuple29*v))Cyc_Dict_insert)((te->ae)->ordinaries,_tmp794->name,({struct _tuple29*_tmp79C=_cycalloc(sizeof(*_tmp79C));_tmp79C->f1=(void*)({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct*_tmp79D=_cycalloc(sizeof(*_tmp79D));_tmp79D[0]=({struct Cyc_Tcenv_AnonEnumRes_Tcenv_Resolved_struct _tmp79E;_tmp79E.tag=4;_tmp79E.f1=t;_tmp79E.f2=_tmp794;_tmp79E;});_tmp79D;});_tmp79C->f2=1;_tmp79C;}));};}}
# 3952
_npop_handler(0);goto _LL66E;
# 3931
;_pop_region(uprev_rgn);}case 13: _LL679: _tmp91D=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp77B)->f1;_tmp91C=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp77B)->f2;_LL67A:
# 3954
 if(*_tmp91C == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp91C))->fields == 0){
struct _handler_cons _tmp79F;_push_handler(& _tmp79F);{int _tmp7A1=0;if(setjmp(_tmp79F.handler))_tmp7A1=1;if(!_tmp7A1){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91D);
*_tmp91C=*ed;}
# 3956
;_pop_handler();}else{void*_tmp7A0=(void*)_exn_thrown;void*_tmp7A2=_tmp7A0;void*_tmp7A5;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7A2)->tag == Cyc_Dict_Absent){_LL6BB: _LL6BC: {
# 3960
struct Cyc_Absyn_Enumdecl*_tmp7A3=({struct Cyc_Absyn_Enumdecl*_tmp7A4=_cycalloc(sizeof(*_tmp7A4));_tmp7A4->sc=Cyc_Absyn_Extern;_tmp7A4->name=_tmp91D;_tmp7A4->fields=0;_tmp7A4;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp7A3);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp91D);
*_tmp91C=*ed;
goto _LL6BA;};}}else{_LL6BD: _tmp7A5=_tmp7A2;_LL6BE:(int)_rethrow(_tmp7A5);}_LL6BA:;}};}{
# 3966
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp91C);
goto _LL66E;};case 3: _LL67B: _tmp91F=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp77B)->f1).datatype_info;_tmp91E=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp77B)->f1).targs;_LL67C: {
# 3969
struct Cyc_List_List*_tmp7A6=*_tmp91E;
{union Cyc_Absyn_DatatypeInfoU _tmp7A7=*_tmp91F;union Cyc_Absyn_DatatypeInfoU _tmp7A8=_tmp7A7;struct Cyc_Absyn_Datatypedecl*_tmp7C0;struct _tuple2*_tmp7BF;int _tmp7BE;if((_tmp7A8.UnknownDatatype).tag == 1){_LL6C0: _tmp7BF=((_tmp7A8.UnknownDatatype).val).name;_tmp7BE=((_tmp7A8.UnknownDatatype).val).is_extensible;_LL6C1: {
# 3972
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp7A9;_push_handler(& _tmp7A9);{int _tmp7AB=0;if(setjmp(_tmp7A9.handler))_tmp7AB=1;if(!_tmp7AB){tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7BF);;_pop_handler();}else{void*_tmp7AA=(void*)_exn_thrown;void*_tmp7AC=_tmp7AA;void*_tmp7B2;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7AC)->tag == Cyc_Dict_Absent){_LL6C5: _LL6C6: {
# 3976
struct Cyc_Absyn_Datatypedecl*_tmp7AD=({struct Cyc_Absyn_Datatypedecl*_tmp7B1=_cycalloc(sizeof(*_tmp7B1));_tmp7B1->sc=Cyc_Absyn_Extern;_tmp7B1->name=_tmp7BF;_tmp7B1->tvs=0;_tmp7B1->fields=0;_tmp7B1->is_extensible=_tmp7BE;_tmp7B1;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp7AD);
tudp=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7BF);
# 3980
if(_tmp7A6 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp7B0;_tmp7B0.tag=0;_tmp7B0.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7BF));({void*_tmp7AE[1]={& _tmp7B0};Cyc_Tcutil_terr(loc,({const char*_tmp7AF="declare parameterized datatype %s before using";_tag_dyneither(_tmp7AF,sizeof(char),47);}),_tag_dyneither(_tmp7AE,sizeof(void*),1));});});
return cvtenv;}
# 3985
goto _LL6C4;}}else{_LL6C7: _tmp7B2=_tmp7AC;_LL6C8:(int)_rethrow(_tmp7B2);}_LL6C4:;}};}
# 3989
if(_tmp7BE  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7B5;_tmp7B5.tag=0;_tmp7B5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7BF));({void*_tmp7B3[1]={& _tmp7B5};Cyc_Tcutil_terr(loc,({const char*_tmp7B4="datatype %s was not declared @extensible";_tag_dyneither(_tmp7B4,sizeof(char),41);}),_tag_dyneither(_tmp7B3,sizeof(void*),1));});});
*_tmp91F=Cyc_Absyn_KnownDatatype(tudp);
_tmp7C0=*tudp;goto _LL6C3;}}else{_LL6C2: _tmp7C0=*(_tmp7A8.KnownDatatype).val;_LL6C3: {
# 3996
struct Cyc_List_List*tvs=_tmp7C0->tvs;
for(0;_tmp7A6 != 0  && tvs != 0;(_tmp7A6=_tmp7A6->tl,tvs=tvs->tl)){
void*t=(void*)_tmp7A6->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4002
{struct _tuple0 _tmp7B6=({struct _tuple0 _tmp7B9;_tmp7B9.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7B9.f2=t;_tmp7B9;});struct _tuple0 _tmp7B7=_tmp7B6;struct Cyc_Absyn_Tvar*_tmp7B8;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7B7.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7B7.f2)->tag == 2){_LL6CA: _tmp7B8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7B7.f2)->f1;_LL6CB:
# 4004
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7B8);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7B8,1);
continue;}else{goto _LL6CC;}}else{_LL6CC: _LL6CD:
 goto _LL6C9;}_LL6C9:;}{
# 4009
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4013
if(_tmp7A6 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7BC;_tmp7BC.tag=0;_tmp7BC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7C0->name));({void*_tmp7BA[1]={& _tmp7BC};Cyc_Tcutil_terr(loc,({const char*_tmp7BB="too many type arguments for datatype %s";_tag_dyneither(_tmp7BB,sizeof(char),40);}),_tag_dyneither(_tmp7BA,sizeof(void*),1));});});
if(tvs != 0){
# 4018
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp7BD=_cycalloc(sizeof(*_tmp7BD));_tmp7BD->hd=e;_tmp7BD->tl=hidden_ts;_tmp7BD;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);}
# 4025
*_tmp91E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp91E,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4027
goto _LL6BF;}}_LL6BF:;}
# 4029
goto _LL66E;}case 4: _LL67D: _tmp921=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp77B)->f1).field_info;_tmp920=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp77B)->f1).targs;_LL67E:
# 4032
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7C1=*_tmp921;union Cyc_Absyn_DatatypeFieldInfoU _tmp7C2=_tmp7C1;struct Cyc_Absyn_Datatypedecl*_tmp7F0;struct Cyc_Absyn_Datatypefield*_tmp7EF;struct _tuple2*_tmp7EE;struct _tuple2*_tmp7ED;int _tmp7EC;if((_tmp7C2.UnknownDatatypefield).tag == 1){_LL6CF: _tmp7EE=((_tmp7C2.UnknownDatatypefield).val).datatype_name;_tmp7ED=((_tmp7C2.UnknownDatatypefield).val).field_name;_tmp7EC=((_tmp7C2.UnknownDatatypefield).val).is_extensible;_LL6D0: {
# 4034
struct Cyc_Absyn_Datatypedecl*tud;
struct Cyc_Absyn_Datatypefield*tuf;
{struct _handler_cons _tmp7C3;_push_handler(& _tmp7C3);{int _tmp7C5=0;if(setjmp(_tmp7C3.handler))_tmp7C5=1;if(!_tmp7C5){*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7EE);;_pop_handler();}else{void*_tmp7C4=(void*)_exn_thrown;void*_tmp7C6=_tmp7C4;void*_tmp7CA;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7C6)->tag == Cyc_Dict_Absent){_LL6D4: _LL6D5:
# 4038
({struct Cyc_String_pa_PrintArg_struct _tmp7C9;_tmp7C9.tag=0;_tmp7C9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7EE));({void*_tmp7C7[1]={& _tmp7C9};Cyc_Tcutil_terr(loc,({const char*_tmp7C8="unbound datatype %s";_tag_dyneither(_tmp7C8,sizeof(char),20);}),_tag_dyneither(_tmp7C7,sizeof(void*),1));});});
return cvtenv;}else{_LL6D6: _tmp7CA=_tmp7C6;_LL6D7:(int)_rethrow(_tmp7CA);}_LL6D3:;}};}
# 4041
{struct _handler_cons _tmp7CB;_push_handler(& _tmp7CB);{int _tmp7CD=0;if(setjmp(_tmp7CB.handler))_tmp7CD=1;if(!_tmp7CD){{void*_tmp7CE=Cyc_Tcenv_lookup_ordinary_global(te,loc,_tmp7ED,0);void*_tmp7CF=_tmp7CE;struct Cyc_Absyn_Datatypedecl*_tmp7D9;struct Cyc_Absyn_Datatypefield*_tmp7D8;if(((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7CF)->tag == 2){_LL6D9: _tmp7D9=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7CF)->f1;_tmp7D8=((struct Cyc_Tcenv_DatatypeRes_Tcenv_Resolved_struct*)_tmp7CF)->f2;_LL6DA:
# 4043
 tuf=_tmp7D8;
tud=_tmp7D9;
if(_tmp7EC  && !tud->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp7D2;_tmp7D2.tag=0;_tmp7D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7EE));({void*_tmp7D0[1]={& _tmp7D2};Cyc_Tcutil_terr(loc,({const char*_tmp7D1="datatype %s was not declared @extensible";_tag_dyneither(_tmp7D1,sizeof(char),41);}),_tag_dyneither(_tmp7D0,sizeof(void*),1));});});
goto _LL6D8;}else{_LL6DB: _LL6DC:
({struct Cyc_String_pa_PrintArg_struct _tmp7D6;_tmp7D6.tag=0;_tmp7D6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7EE));({struct Cyc_String_pa_PrintArg_struct _tmp7D5;_tmp7D5.tag=0;_tmp7D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7ED));({void*_tmp7D3[2]={& _tmp7D5,& _tmp7D6};Cyc_Tcutil_terr(loc,({const char*_tmp7D4="unbound field %s in type datatype %s";_tag_dyneither(_tmp7D4,sizeof(char),37);}),_tag_dyneither(_tmp7D3,sizeof(void*),2));});});});{
struct Cyc_Tcutil_CVTEnv _tmp7D7=cvtenv;_npop_handler(0);return _tmp7D7;};}_LL6D8:;}
# 4041
;_pop_handler();}else{void*_tmp7CC=(void*)_exn_thrown;void*_tmp7DA=_tmp7CC;void*_tmp7DF;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp7DA)->tag == Cyc_Dict_Absent){_LL6DE: _LL6DF:
# 4054
({struct Cyc_String_pa_PrintArg_struct _tmp7DE;_tmp7DE.tag=0;_tmp7DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7EE));({struct Cyc_String_pa_PrintArg_struct _tmp7DD;_tmp7DD.tag=0;_tmp7DD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7ED));({void*_tmp7DB[2]={& _tmp7DD,& _tmp7DE};Cyc_Tcutil_terr(loc,({const char*_tmp7DC="unbound field %s in type datatype %s";_tag_dyneither(_tmp7DC,sizeof(char),37);}),_tag_dyneither(_tmp7DB,sizeof(void*),2));});});});
return cvtenv;}else{_LL6E0: _tmp7DF=_tmp7DA;_LL6E1:(int)_rethrow(_tmp7DF);}_LL6DD:;}};}
# 4058
*_tmp921=Cyc_Absyn_KnownDatatypefield(tud,tuf);
_tmp7F0=tud;_tmp7EF=tuf;goto _LL6D2;}}else{_LL6D1: _tmp7F0=((_tmp7C2.KnownDatatypefield).val).f1;_tmp7EF=((_tmp7C2.KnownDatatypefield).val).f2;_LL6D2: {
# 4062
struct Cyc_List_List*tvs=_tmp7F0->tvs;
for(0;_tmp920 != 0  && tvs != 0;(_tmp920=_tmp920->tl,tvs=tvs->tl)){
void*t=(void*)_tmp920->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4068
{struct _tuple0 _tmp7E0=({struct _tuple0 _tmp7E3;_tmp7E3.f1=Cyc_Absyn_compress_kb(tv->kind);_tmp7E3.f2=t;_tmp7E3;});struct _tuple0 _tmp7E1=_tmp7E0;struct Cyc_Absyn_Tvar*_tmp7E2;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7E1.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7E1.f2)->tag == 2){_LL6E3: _tmp7E2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7E1.f2)->f1;_LL6E4:
# 4070
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7E2);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7E2,1);
continue;}else{goto _LL6E5;}}else{_LL6E5: _LL6E6:
 goto _LL6E2;}_LL6E2:;}{
# 4075
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4079
if(_tmp920 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7E7;_tmp7E7.tag=0;_tmp7E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7EF->name));({struct Cyc_String_pa_PrintArg_struct _tmp7E6;_tmp7E6.tag=0;_tmp7E6.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7F0->name));({void*_tmp7E4[2]={& _tmp7E6,& _tmp7E7};Cyc_Tcutil_terr(loc,({const char*_tmp7E5="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp7E5,sizeof(char),43);}),_tag_dyneither(_tmp7E4,sizeof(void*),2));});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7EB;_tmp7EB.tag=0;_tmp7EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7EF->name));({struct Cyc_String_pa_PrintArg_struct _tmp7EA;_tmp7EA.tag=0;_tmp7EA.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7F0->name));({void*_tmp7E8[2]={& _tmp7EA,& _tmp7EB};Cyc_Tcutil_terr(loc,({const char*_tmp7E9="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp7E9,sizeof(char),42);}),_tag_dyneither(_tmp7E8,sizeof(void*),2));});});});
goto _LL6CE;}}_LL6CE:;}
# 4087
goto _LL66E;case 5: _LL67F: _tmp927=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77B)->f1).elt_typ;_tmp926=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77B)->f1).elt_tq;_tmp925=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77B)->f1).ptr_atts).rgn;_tmp924=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77B)->f1).ptr_atts).nullable;_tmp923=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77B)->f1).ptr_atts).bounds;_tmp922=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp77B)->f1).ptr_atts).zero_term;_LL680: {
# 4090
int is_zero_terminated;
# 4092
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp927,1,1);
_tmp926->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp926->print_const,_tmp927);{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp7F1=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp7F2=_tmp7F1;switch(_tmp7F2){case Cyc_Absyn_Aliasable: _LL6E8: _LL6E9:
 k=& Cyc_Tcutil_rk;goto _LL6E7;case Cyc_Absyn_Unique: _LL6EA: _LL6EB:
 k=& Cyc_Tcutil_urk;goto _LL6E7;default: _LL6EC: _LL6ED:
 k=& Cyc_Tcutil_trk;goto _LL6E7;}_LL6E7:;}
# 4100
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp925,1,1);
{union Cyc_Absyn_Constraint*_tmp7F3=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp922);union Cyc_Absyn_Constraint*_tmp7F4=_tmp7F3;int _tmp7FA;switch((((union Cyc_Absyn_Constraint*)_tmp7F4)->Eq_constr).tag){case 3: _LL6EF: _LL6F0:
# 4105
{void*_tmp7F5=Cyc_Tcutil_compress(_tmp927);void*_tmp7F6=_tmp7F5;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F6)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7F6)->f2 == Cyc_Absyn_Char_sz){_LL6F6: _LL6F7:
# 4107
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp922,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL6F5;}else{goto _LL6F8;}}else{_LL6F8: _LL6F9:
# 4111
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp922,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL6F5;}_LL6F5:;}
# 4115
goto _LL6EE;case 1: _LL6F1: _tmp7FA=(int)(_tmp7F4->Eq_constr).val;if(_tmp7FA){_LL6F2:
# 4118
 if(!Cyc_Tcutil_admits_zero(_tmp927))
({struct Cyc_String_pa_PrintArg_struct _tmp7F9;_tmp7F9.tag=0;_tmp7F9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp927));({void*_tmp7F7[1]={& _tmp7F9};Cyc_Tcutil_terr(loc,({const char*_tmp7F8="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp7F8,sizeof(char),53);}),_tag_dyneither(_tmp7F7,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL6EE;}else{goto _LL6F3;}default: _LL6F3: _LL6F4:
# 4124
 is_zero_terminated=0;
goto _LL6EE;}_LL6EE:;}
# 4128
{void*_tmp7FB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp923);void*_tmp7FC=_tmp7FB;struct Cyc_Absyn_Exp*_tmp807;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp7FC)->tag == 0){_LL6FB: _LL6FC:
 goto _LL6FA;}else{_LL6FD: _tmp807=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7FC)->f1;_LL6FE: {
# 4131
struct _RegionHandle _tmp7FD=_new_region("temp");struct _RegionHandle*temp=& _tmp7FD;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp7FE=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp7FE,0,_tmp807);}
# 4135
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp807,te,cvtenv);
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp807))
({void*_tmp7FF=0;Cyc_Tcutil_terr(loc,({const char*_tmp800="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp800,sizeof(char),49);}),_tag_dyneither(_tmp7FF,sizeof(void*),0));});{
struct _tuple14 _tmp801=Cyc_Evexp_eval_const_uint_exp(_tmp807);struct _tuple14 _tmp802=_tmp801;unsigned int _tmp806;int _tmp805;_LL700: _tmp806=_tmp802.f1;_tmp805=_tmp802.f2;_LL701:;
if(is_zero_terminated  && (!_tmp805  || _tmp806 < 1))
({void*_tmp803=0;Cyc_Tcutil_terr(loc,({const char*_tmp804="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp804,sizeof(char),55);}),_tag_dyneither(_tmp803,sizeof(void*),0));});
_npop_handler(0);goto _LL6FA;};
# 4131
;_pop_region(temp);}}_LL6FA:;}
# 4143
goto _LL66E;};}case 19: _LL681: _tmp928=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp77B)->f1;_LL682:
# 4145
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp928,1,1);goto _LL66E;case 28: _LL683: _LL684:
 goto _LL66E;case 18: _LL685: _tmp929=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp77B)->f1;_LL686: {
# 4151
struct _RegionHandle _tmp808=_new_region("temp");struct _RegionHandle*temp=& _tmp808;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp809=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp809,0,_tmp929);}
# 4155
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp929))
({void*_tmp80A=0;Cyc_Tcutil_terr(loc,({const char*_tmp80B="valueof_t requires an int expression";_tag_dyneither(_tmp80B,sizeof(char),37);}),_tag_dyneither(_tmp80A,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp929,te,cvtenv);
_npop_handler(0);goto _LL66E;
# 4151
;_pop_region(temp);}case 27: _LL687: _tmp92A=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp77B)->f1;_LL688: {
# 4163
struct _RegionHandle _tmp80C=_new_region("temp");struct _RegionHandle*temp=& _tmp80C;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp80D=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp80D,0,_tmp92A);}
# 4167
_npop_handler(0);goto _LL66E;
# 4163
;_pop_region(temp);}case 6: _LL689: _LL68A:
# 4168
 goto _LL68C;case 7: _LL68B: _LL68C:
 goto _LL66E;case 8: _LL68D: _tmp92F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp77B)->f1).elt_type;_tmp92E=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp77B)->f1).tq;_tmp92D=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp77B)->f1).num_elts;_tmp92C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp77B)->f1).zero_term;_tmp92B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp77B)->f1).zt_loc;_LL68E: {
# 4173
struct Cyc_Absyn_Exp*_tmp80E=*_tmp92D;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp92F,1,allow_abs_aggr);
_tmp92E->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp92E->print_const,_tmp92F);{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp80F=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp92C);union Cyc_Absyn_Constraint*_tmp810=_tmp80F;int _tmp814;switch((((union Cyc_Absyn_Constraint*)_tmp810)->Eq_constr).tag){case 3: _LL703: _LL704:
# 4180
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp92C,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4195 "tcutil.cyc"
goto _LL702;case 1: _LL705: _tmp814=(int)(_tmp810->Eq_constr).val;if(_tmp814){_LL706:
# 4198
 if(!Cyc_Tcutil_admits_zero(_tmp92F))
({struct Cyc_String_pa_PrintArg_struct _tmp813;_tmp813.tag=0;_tmp813.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp92F));({void*_tmp811[1]={& _tmp813};Cyc_Tcutil_terr(loc,({const char*_tmp812="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp812,sizeof(char),51);}),_tag_dyneither(_tmp811,sizeof(void*),1));});});
is_zero_terminated=1;
goto _LL702;}else{goto _LL707;}default: _LL707: _LL708:
# 4204
 is_zero_terminated=0;
goto _LL702;}_LL702:;}
# 4209
if(_tmp80E == 0){
# 4211
if(is_zero_terminated)
# 4213
*_tmp92D=(_tmp80E=Cyc_Absyn_uint_exp(1,0));else{
# 4216
({void*_tmp815=0;Cyc_Tcutil_warn(loc,({const char*_tmp816="array bound defaults to 1 here";_tag_dyneither(_tmp816,sizeof(char),31);}),_tag_dyneither(_tmp815,sizeof(void*),0));});
*_tmp92D=(_tmp80E=Cyc_Absyn_uint_exp(1,0));}}{
# 4220
struct _RegionHandle _tmp817=_new_region("temp");struct _RegionHandle*temp=& _tmp817;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp818=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp818,0,_tmp80E);}
# 4224
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp80E))
({void*_tmp819=0;Cyc_Tcutil_terr(loc,({const char*_tmp81A="array bounds expression is not an unsigned int";_tag_dyneither(_tmp81A,sizeof(char),47);}),_tag_dyneither(_tmp819,sizeof(void*),0));});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp80E,te,cvtenv);{
# 4231
struct _tuple14 _tmp81B=Cyc_Evexp_eval_const_uint_exp(_tmp80E);struct _tuple14 _tmp81C=_tmp81B;unsigned int _tmp822;int _tmp821;_LL70A: _tmp822=_tmp81C.f1;_tmp821=_tmp81C.f2;_LL70B:;
# 4233
if((is_zero_terminated  && _tmp821) && _tmp822 < 1)
({void*_tmp81D=0;Cyc_Tcutil_warn(loc,({const char*_tmp81E="zero terminated array cannot have zero elements";_tag_dyneither(_tmp81E,sizeof(char),48);}),_tag_dyneither(_tmp81D,sizeof(void*),0));});
# 4236
if((_tmp821  && _tmp822 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp81F=0;Cyc_Tcutil_warn(loc,({const char*_tmp820="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp820,sizeof(char),75);}),_tag_dyneither(_tmp81F,sizeof(void*),0));});
*_tmp92D=Cyc_Absyn_uint_exp(1,0);}
# 4240
_npop_handler(0);goto _LL66E;};
# 4220
;_pop_region(temp);};};}case 9: _LL68F: _tmp93C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).tvars;_tmp93B=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).effect;_tmp93A=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).ret_tqual;_tmp939=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).ret_typ;_tmp938=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).args;_tmp937=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).c_varargs;_tmp936=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).cyc_varargs;_tmp935=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).rgn_po;_tmp934=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).attributes;_tmp933=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).requires_clause;_tmp932=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).requires_relns;_tmp931=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).ensures_clause;_tmp930=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp77B)->f1).ensures_relns;_LL690: {
# 4247
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp934 != 0;_tmp934=_tmp934->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp934->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp825;_tmp825.tag=0;_tmp825.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp934->hd));({void*_tmp823[1]={& _tmp825};Cyc_Tcutil_terr(loc,({const char*_tmp824="bad function type attribute %s";_tag_dyneither(_tmp824,sizeof(char),31);}),_tag_dyneither(_tmp823,sizeof(void*),1));});});{
void*_tmp826=(void*)_tmp934->hd;void*_tmp827=_tmp826;enum Cyc_Absyn_Format_Type _tmp82C;int _tmp82B;int _tmp82A;switch(*((int*)_tmp827)){case 1: _LL70D: _LL70E:
# 4260
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL70C;case 2: _LL70F: _LL710:
# 4262
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL70C;case 3: _LL711: _LL712:
# 4264
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL70C;case 19: _LL713: _tmp82C=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp827)->f1;_tmp82B=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp827)->f2;_tmp82A=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp827)->f3;_LL714:
# 4266
 if(!seen_format){
seen_format=1;
ft=_tmp82C;
fmt_desc_arg=_tmp82B;
fmt_arg_start=_tmp82A;}else{
# 4272
({void*_tmp828=0;Cyc_Tcutil_terr(loc,({const char*_tmp829="function can't have multiple format attributes";_tag_dyneither(_tmp829,sizeof(char),47);}),_tag_dyneither(_tmp828,sizeof(void*),0));});}
goto _LL70C;default: _LL715: _LL716:
 goto _LL70C;}_LL70C:;};}
# 4277
if(num_convs > 1)
({void*_tmp82D=0;Cyc_Tcutil_terr(loc,({const char*_tmp82E="function can't have multiple calling conventions";_tag_dyneither(_tmp82E,sizeof(char),49);}),_tag_dyneither(_tmp82D,sizeof(void*),0));});
# 4282
Cyc_Tcutil_check_unique_tvars(loc,*_tmp93C);
{struct Cyc_List_List*b=*_tmp93C;for(0;b != 0;b=b->tl){
((struct Cyc_Absyn_Tvar*)b->hd)->identity=Cyc_Tcutil_new_tvar_id();
cvtenv.kind_env=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);{
void*_tmp82F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp830=_tmp82F;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp830)->tag == 0){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp830)->f1)->kind == Cyc_Absyn_MemKind){_LL718: _LL719:
# 4288
({struct Cyc_String_pa_PrintArg_struct _tmp833;_tmp833.tag=0;_tmp833.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp831[1]={& _tmp833};Cyc_Tcutil_terr(loc,({const char*_tmp832="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp832,sizeof(char),51);}),_tag_dyneither(_tmp831,sizeof(void*),1));});});
goto _LL717;}else{goto _LL71A;}}else{_LL71A: _LL71B:
 goto _LL717;}_LL717:;};}}{
# 4296
struct _RegionHandle _tmp834=_new_region("newr");struct _RegionHandle*newr=& _tmp834;_push_region(newr);{
struct Cyc_Tcutil_CVTEnv _tmp835=({struct Cyc_Tcutil_CVTEnv _tmp8C8;_tmp8C8.r=newr;_tmp8C8.kind_env=cvtenv.kind_env;_tmp8C8.free_vars=0;_tmp8C8.free_evars=0;_tmp8C8.generalize_evars=cvtenv.generalize_evars;_tmp8C8.fn_result=1;_tmp8C8;});
# 4302
_tmp835=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp835,& Cyc_Tcutil_tmk,_tmp939,1,1);
_tmp93A->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp93A->print_const,_tmp939);
_tmp835.fn_result=0;
{struct Cyc_List_List*a=_tmp938;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp836=(struct _tuple10*)a->hd;
void*_tmp837=(*_tmp836).f3;
_tmp835=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp835,& Cyc_Tcutil_tmk,_tmp837,1,1);{
int _tmp838=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp836).f2).print_const,_tmp837);
((*_tmp836).f2).real_const=_tmp838;
# 4313
if(Cyc_Tcutil_is_array(_tmp837)){
# 4316
void*_tmp839=Cyc_Absyn_new_evar(0,0);
_tmp835=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp835,& Cyc_Tcutil_rk,_tmp839,1,1);
(*_tmp836).f3=Cyc_Tcutil_promote_array(_tmp837,_tmp839,0);}};}}
# 4323
if(_tmp936 != 0){
if(_tmp937)({void*_tmp83A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp83B="both c_vararg and cyc_vararg";_tag_dyneither(_tmp83B,sizeof(char),29);}),_tag_dyneither(_tmp83A,sizeof(void*),0));});{
struct Cyc_Absyn_VarargInfo _tmp83C=*_tmp936;struct Cyc_Absyn_VarargInfo _tmp83D=_tmp83C;void*_tmp850;int _tmp84F;_LL71D: _tmp850=_tmp83D.type;_tmp84F=_tmp83D.inject;_LL71E:;
_tmp835=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp835,& Cyc_Tcutil_tmk,_tmp850,1,1);
(_tmp936->tq).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp936->tq).print_const,_tmp850);
# 4329
if(_tmp84F){
void*_tmp83E=Cyc_Tcutil_compress(_tmp850);void*_tmp83F=_tmp83E;void*_tmp84E;union Cyc_Absyn_Constraint*_tmp84D;union Cyc_Absyn_Constraint*_tmp84C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp83F)->tag == 5){_LL720: _tmp84E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp83F)->f1).elt_typ;_tmp84D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp83F)->f1).ptr_atts).bounds;_tmp84C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp83F)->f1).ptr_atts).zero_term;_LL721:
# 4332
{void*_tmp840=Cyc_Tcutil_compress(_tmp84E);void*_tmp841=_tmp840;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp841)->tag == 3){_LL725: _LL726:
# 4334
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp84C))
({void*_tmp842=0;Cyc_Tcutil_terr(loc,({const char*_tmp843="can't inject into a zeroterm pointer";_tag_dyneither(_tmp843,sizeof(char),37);}),_tag_dyneither(_tmp842,sizeof(void*),0));});
{void*_tmp844=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp84D);void*_tmp845=_tmp844;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp845)->tag == 0){_LL72A: _LL72B:
# 4338
({void*_tmp846=0;Cyc_Tcutil_terr(loc,({const char*_tmp847="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp847,sizeof(char),44);}),_tag_dyneither(_tmp846,sizeof(void*),0));});
goto _LL729;}else{_LL72C: _LL72D:
 goto _LL729;}_LL729:;}
# 4342
goto _LL724;}else{_LL727: _LL728:
({void*_tmp848=0;Cyc_Tcutil_terr(loc,({const char*_tmp849="can't inject a non-datatype type";_tag_dyneither(_tmp849,sizeof(char),33);}),_tag_dyneither(_tmp848,sizeof(void*),0));});goto _LL724;}_LL724:;}
# 4345
goto _LL71F;}else{_LL722: _LL723:
({void*_tmp84A=0;Cyc_Tcutil_terr(loc,({const char*_tmp84B="expecting a datatype pointer type";_tag_dyneither(_tmp84B,sizeof(char),34);}),_tag_dyneither(_tmp84A,sizeof(void*),0));});goto _LL71F;}_LL71F:;}};}
# 4351
if(seen_format){
int _tmp851=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp938);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp851) || fmt_arg_start < 0) || 
# 4355
(_tmp936 == 0  && !_tmp937) && fmt_arg_start != 0) || 
(_tmp936 != 0  || _tmp937) && fmt_arg_start != _tmp851 + 1)
# 4358
({void*_tmp852=0;Cyc_Tcutil_terr(loc,({const char*_tmp853="bad format descriptor";_tag_dyneither(_tmp853,sizeof(char),22);}),_tag_dyneither(_tmp852,sizeof(void*),0));});else{
# 4361
struct _tuple10 _tmp854=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp938,fmt_desc_arg - 1);struct _tuple10 _tmp855=_tmp854;void*_tmp86D;_LL72F: _tmp86D=_tmp855.f3;_LL730:;
# 4363
{void*_tmp856=Cyc_Tcutil_compress(_tmp86D);void*_tmp857=_tmp856;void*_tmp865;union Cyc_Absyn_Constraint*_tmp864;union Cyc_Absyn_Constraint*_tmp863;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp857)->tag == 5){_LL732: _tmp865=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp857)->f1).elt_typ;_tmp864=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp857)->f1).ptr_atts).bounds;_tmp863=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp857)->f1).ptr_atts).zero_term;_LL733:
# 4366
{struct _tuple0 _tmp858=({struct _tuple0 _tmp860;_tmp860.f1=Cyc_Tcutil_compress(_tmp865);_tmp860.f2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp864);_tmp860;});struct _tuple0 _tmp859=_tmp858;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp859.f1)->tag == 6){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp859.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp859.f2)->tag == 0){_LL737: _LL738:
# 4368
 if(_tmp937)
({void*_tmp85A=0;Cyc_Tcutil_terr(loc,({const char*_tmp85B="format descriptor is not a char * type";_tag_dyneither(_tmp85B,sizeof(char),39);}),_tag_dyneither(_tmp85A,sizeof(void*),0));});
goto _LL736;}else{_LL739: _LL73A:
# 4372
 if(!_tmp937)
({void*_tmp85C=0;Cyc_Tcutil_terr(loc,({const char*_tmp85D="format descriptor is not a char ? type";_tag_dyneither(_tmp85D,sizeof(char),39);}),_tag_dyneither(_tmp85C,sizeof(void*),0));});
goto _LL736;}}else{goto _LL73B;}}else{_LL73B: _LL73C:
# 4376
({void*_tmp85E=0;Cyc_Tcutil_terr(loc,({const char*_tmp85F="format descriptor is not a string type";_tag_dyneither(_tmp85F,sizeof(char),39);}),_tag_dyneither(_tmp85E,sizeof(void*),0));});
goto _LL736;}_LL736:;}
# 4379
goto _LL731;}else{_LL734: _LL735:
({void*_tmp861=0;Cyc_Tcutil_terr(loc,({const char*_tmp862="format descriptor is not a string type";_tag_dyneither(_tmp862,sizeof(char),39);}),_tag_dyneither(_tmp861,sizeof(void*),0));});goto _LL731;}_LL731:;}
# 4382
if(fmt_arg_start != 0  && !_tmp937){
# 4386
int problem;
{struct _tuple31 _tmp866=({struct _tuple31 _tmp86A;_tmp86A.f1=ft;_tmp86A.f2=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp936))->type));_tmp86A;});struct _tuple31 _tmp867=_tmp866;struct Cyc_Absyn_Datatypedecl*_tmp869;struct Cyc_Absyn_Datatypedecl*_tmp868;if(_tmp867.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp867.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp867.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL73E: _tmp868=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp867.f2)->f1).datatype_info).KnownDatatype).val;_LL73F:
# 4389
 problem=Cyc_Absyn_qvar_cmp(_tmp868->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;goto _LL73D;}else{goto _LL742;}}else{goto _LL742;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp867.f2)->tag == 3){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp867.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL740: _tmp869=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp867.f2)->f1).datatype_info).KnownDatatype).val;_LL741:
# 4391
 problem=Cyc_Absyn_qvar_cmp(_tmp869->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;goto _LL73D;}else{goto _LL742;}}else{_LL742: _LL743:
# 4393
 problem=1;goto _LL73D;}}_LL73D:;}
# 4395
if(problem)
({void*_tmp86B=0;Cyc_Tcutil_terr(loc,({const char*_tmp86C="format attribute and vararg types don't match";_tag_dyneither(_tmp86C,sizeof(char),46);}),_tag_dyneither(_tmp86B,sizeof(void*),0));});}}}
# 4403
{struct Cyc_List_List*rpo=_tmp935;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp86E=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp86F=_tmp86E;void*_tmp871;void*_tmp870;_LL745: _tmp871=_tmp86F->f1;_tmp870=_tmp86F->f2;_LL746:;
_tmp835=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp835,& Cyc_Tcutil_ek,_tmp871,1,1);
_tmp835=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp835,& Cyc_Tcutil_trk,_tmp870,1,1);}}{
# 4411
struct Cyc_Tcenv_Fenv*_tmp872=Cyc_Tcenv_bogus_fenv(newr,_tmp939,_tmp938);
struct Cyc_Tcenv_Tenv*_tmp873=({struct Cyc_Tcenv_Tenv*_tmp8C7=_region_malloc(newr,sizeof(*_tmp8C7));_tmp8C7->ns=te->ns;_tmp8C7->ae=te->ae;_tmp8C7->le=_tmp872;_tmp8C7->allow_valueof=1;_tmp8C7->in_extern_c_include=te->in_extern_c_include;_tmp8C7;});
struct _tuple30 _tmp874=Cyc_Tcutil_check_clause(loc,_tmp873,_tmp835,({const char*_tmp8C6="@requires";_tag_dyneither(_tmp8C6,sizeof(char),10);}),_tmp933);struct _tuple30 _tmp875=_tmp874;struct Cyc_Tcutil_CVTEnv _tmp8C5;struct Cyc_List_List*_tmp8C4;_LL748: _tmp8C5=_tmp875.f1;_tmp8C4=_tmp875.f2;_LL749:;
_tmp835=_tmp8C5;
*_tmp932=_tmp8C4;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp938,_tmp8C4);{
# 4418
struct _dyneither_ptr*_tmp876=_init_dyneither_ptr(_cycalloc(sizeof(struct _dyneither_ptr)),"return_value",sizeof(char),13);
struct Cyc_Absyn_Vardecl*_tmp877=({struct Cyc_Absyn_Vardecl*_tmp8C1=_cycalloc(sizeof(*_tmp8C1));_tmp8C1->sc=Cyc_Absyn_Public;_tmp8C1->name=({struct _tuple2*_tmp8C2=_cycalloc(sizeof(*_tmp8C2));_tmp8C2->f1=Cyc_Absyn_Loc_n;_tmp8C2->f2=_tmp876;_tmp8C2;});_tmp8C1->tq=
Cyc_Absyn_empty_tqual(0);_tmp8C1->type=_tmp939;_tmp8C1->initializer=0;_tmp8C1->rgn=
Cyc_Tcenv_curr_rgn(_tmp873);_tmp8C1->attributes=0;_tmp8C1->escapes=0;_tmp8C1;});
# 4424
struct _tuple30 _tmp878=Cyc_Tcutil_check_clause(loc,_tmp873,_tmp835,({const char*_tmp8C0="@ensures";_tag_dyneither(_tmp8C0,sizeof(char),9);}),_tmp931);struct _tuple30 _tmp879=_tmp878;struct Cyc_Tcutil_CVTEnv _tmp8BF;struct Cyc_List_List*_tmp8BE;_LL74B: _tmp8BF=_tmp879.f1;_tmp8BE=_tmp879.f2;_LL74C:;
_tmp835=_tmp8BF;
*_tmp930=_tmp8BE;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp938,_tmp8BE);
# 4429
if(*_tmp93B != 0)
_tmp835=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp835,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp93B),1,1);else{
# 4432
struct Cyc_List_List*effect=0;
# 4437
{struct Cyc_List_List*tvs=_tmp835.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp87A=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp87B=_tmp87A;struct Cyc_Absyn_Tvar*_tmp894;int _tmp893;_LL74E: _tmp894=_tmp87B.f1;_tmp893=_tmp87B.f2;_LL74F:;
if(!_tmp893)continue;{
void*_tmp87C=Cyc_Absyn_compress_kb(_tmp894->kind);void*_tmp87D=_tmp87C;struct Cyc_Core_Opt**_tmp892;struct Cyc_Absyn_Kind*_tmp891;struct Cyc_Core_Opt**_tmp890;struct Cyc_Core_Opt**_tmp88F;struct Cyc_Absyn_Kind*_tmp88E;switch(*((int*)_tmp87D)){case 2: _LL751: _tmp88F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87D)->f1;_tmp88E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87D)->f2;if(_tmp88E->kind == Cyc_Absyn_RgnKind){_LL752:
# 4443
 if(_tmp88E->aliasqual == Cyc_Absyn_Top){
*_tmp88F=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);_tmp891=_tmp88E;goto _LL754;}
# 4446
*_tmp88F=Cyc_Tcutil_kind_to_bound_opt(_tmp88E);_tmp891=_tmp88E;goto _LL754;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87D)->f2)->kind){case Cyc_Absyn_IntKind: _LL755: _LL756:
# 4449
 goto _LL758;case Cyc_Absyn_EffKind: _LL759: _tmp890=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp87D)->f1;_LL75A:
# 4452
*_tmp890=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);goto _LL75C;default: goto _LL75F;}}case 0: _LL753: _tmp891=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp87D)->f1;if(_tmp891->kind == Cyc_Absyn_RgnKind){_LL754:
# 4448
 effect=({struct Cyc_List_List*_tmp87E=_cycalloc(sizeof(*_tmp87E));_tmp87E->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp87F=_cycalloc(sizeof(*_tmp87F));_tmp87F[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp880;_tmp880.tag=23;_tmp880.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp882;_tmp882.tag=2;_tmp882.f1=_tmp894;_tmp882;});_tmp881;});_tmp880;});_tmp87F;});_tmp87E->tl=effect;_tmp87E;});goto _LL750;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp87D)->f1)->kind){case Cyc_Absyn_IntKind: _LL757: _LL758:
# 4450
 goto _LL750;case Cyc_Absyn_EffKind: _LL75B: _LL75C:
# 4454
 effect=({struct Cyc_List_List*_tmp883=_cycalloc(sizeof(*_tmp883));_tmp883->hd=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp884=_cycalloc(sizeof(*_tmp884));_tmp884[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp885;_tmp885.tag=2;_tmp885.f1=_tmp894;_tmp885;});_tmp884;});_tmp883->tl=effect;_tmp883;});goto _LL750;default: _LL75F: _LL760:
# 4459
 effect=({struct Cyc_List_List*_tmp889=_cycalloc(sizeof(*_tmp889));_tmp889->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp88A=_cycalloc(sizeof(*_tmp88A));_tmp88A[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp88B;_tmp88B.tag=25;_tmp88B.f1=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp88C=_cycalloc(sizeof(*_tmp88C));_tmp88C[0]=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp88D;_tmp88D.tag=2;_tmp88D.f1=_tmp894;_tmp88D;});_tmp88C;});_tmp88B;});_tmp88A;});_tmp889->tl=effect;_tmp889;});goto _LL750;}}default: _LL75D: _tmp892=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp87D)->f1;_LL75E:
# 4456
*_tmp892=({struct Cyc_Core_Opt*_tmp886=_cycalloc(sizeof(*_tmp886));_tmp886->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp887=_cycalloc(sizeof(*_tmp887));_tmp887[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp888;_tmp888.tag=2;_tmp888.f1=0;_tmp888.f2=& Cyc_Tcutil_ak;_tmp888;});_tmp887;});_tmp886;});goto _LL760;}_LL750:;};}}
# 4463
{struct Cyc_List_List*ts=_tmp835.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp895=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp896=_tmp895;void*_tmp8A1;int _tmp8A0;_LL762: _tmp8A1=_tmp896.f1;_tmp8A0=_tmp896.f2;_LL763:;
if(!_tmp8A0)continue;{
struct Cyc_Absyn_Kind*_tmp897=Cyc_Tcutil_typ_kind(_tmp8A1);struct Cyc_Absyn_Kind*_tmp898=_tmp897;switch(((struct Cyc_Absyn_Kind*)_tmp898)->kind){case Cyc_Absyn_RgnKind: _LL765: _LL766:
# 4468
 effect=({struct Cyc_List_List*_tmp899=_cycalloc(sizeof(*_tmp899));_tmp899->hd=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp89A=_cycalloc(sizeof(*_tmp89A));_tmp89A[0]=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp89B;_tmp89B.tag=23;_tmp89B.f1=_tmp8A1;_tmp89B;});_tmp89A;});_tmp899->tl=effect;_tmp899;});goto _LL764;case Cyc_Absyn_EffKind: _LL767: _LL768:
# 4470
 effect=({struct Cyc_List_List*_tmp89C=_cycalloc(sizeof(*_tmp89C));_tmp89C->hd=_tmp8A1;_tmp89C->tl=effect;_tmp89C;});goto _LL764;case Cyc_Absyn_IntKind: _LL769: _LL76A:
 goto _LL764;default: _LL76B: _LL76C:
# 4473
 effect=({struct Cyc_List_List*_tmp89D=_cycalloc(sizeof(*_tmp89D));_tmp89D->hd=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp89E=_cycalloc(sizeof(*_tmp89E));_tmp89E[0]=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp89F;_tmp89F.tag=25;_tmp89F.f1=_tmp8A1;_tmp89F;});_tmp89E;});_tmp89D->tl=effect;_tmp89D;});goto _LL764;}_LL764:;};}}
# 4476
*_tmp93B=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8A2=_cycalloc(sizeof(*_tmp8A2));_tmp8A2[0]=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8A3;_tmp8A3.tag=24;_tmp8A3.f1=effect;_tmp8A3;});_tmp8A2;});}
# 4482
if(*_tmp93C != 0){
struct Cyc_List_List*bs=*_tmp93C;for(0;bs != 0;bs=bs->tl){
void*_tmp8A4=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp8A5=_tmp8A4;struct Cyc_Core_Opt**_tmp8B5;struct Cyc_Absyn_Kind*_tmp8B4;struct Cyc_Core_Opt**_tmp8B3;struct Cyc_Core_Opt**_tmp8B2;struct Cyc_Core_Opt**_tmp8B1;struct Cyc_Core_Opt**_tmp8B0;struct Cyc_Core_Opt**_tmp8AF;struct Cyc_Core_Opt**_tmp8AE;struct Cyc_Core_Opt**_tmp8AD;struct Cyc_Core_Opt**_tmp8AC;struct Cyc_Core_Opt**_tmp8AB;switch(*((int*)_tmp8A5)){case 1: _LL76E: _tmp8AB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL76F:
# 4486
({struct Cyc_String_pa_PrintArg_struct _tmp8A8;_tmp8A8.tag=0;_tmp8A8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp8A6[1]={& _tmp8A8};Cyc_Tcutil_warn(loc,({const char*_tmp8A7="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp8A7,sizeof(char),47);}),_tag_dyneither(_tmp8A6,sizeof(void*),1));});});
# 4488
_tmp8AC=_tmp8AB;goto _LL771;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f2)->aliasqual == Cyc_Absyn_Top){_LL770: _tmp8AC=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL771:
 _tmp8AD=_tmp8AC;goto _LL773;}else{goto _LL780;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f2)->aliasqual){case Cyc_Absyn_Top: _LL772: _tmp8AD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL773:
 _tmp8AE=_tmp8AD;goto _LL775;case Cyc_Absyn_Aliasable: _LL774: _tmp8AE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL775:
 _tmp8B0=_tmp8AE;goto _LL777;default: _LL77A: _tmp8AF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL77B:
# 4495
 _tmp8B2=_tmp8AF;goto _LL77D;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f2)->aliasqual){case Cyc_Absyn_Top: _LL776: _tmp8B0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL777:
# 4492
 _tmp8B1=_tmp8B0;goto _LL779;case Cyc_Absyn_Aliasable: _LL778: _tmp8B1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL779:
# 4494
*_tmp8B1=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL76D;default: _LL77C: _tmp8B2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL77D:
# 4497
*_tmp8B2=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL76D;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f2)->aliasqual == Cyc_Absyn_Top){_LL77E: _tmp8B3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_LL77F:
# 4499
*_tmp8B3=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL76D;}else{goto _LL780;}default: _LL780: _tmp8B5=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f1;_tmp8B4=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp8A5)->f2;_LL781:
# 4501
*_tmp8B5=Cyc_Tcutil_kind_to_bound_opt(_tmp8B4);goto _LL76D;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp8A5)->f1)->kind == Cyc_Absyn_MemKind){_LL782: _LL783:
# 4503
({void*_tmp8A9=0;Cyc_Tcutil_terr(loc,({const char*_tmp8AA="functions can't abstract M types";_tag_dyneither(_tmp8AA,sizeof(char),33);}),_tag_dyneither(_tmp8A9,sizeof(void*),0));});goto _LL76D;}else{_LL784: _LL785:
 goto _LL76D;}}_LL76D:;}}
# 4508
cvtenv.kind_env=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp835.kind_env,*_tmp93C);
_tmp835.free_vars=Cyc_Tcutil_remove_bound_tvars_bool(_tmp835.r,_tmp835.free_vars,*_tmp93C);
# 4511
{struct Cyc_List_List*tvs=_tmp835.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp8B6=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp8B7=_tmp8B6;struct Cyc_Absyn_Tvar*_tmp8B9;int _tmp8B8;_LL787: _tmp8B9=_tmp8B7.f1;_tmp8B8=_tmp8B7.f2;_LL788:;
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8B9,_tmp8B8);}}{
# 4516
struct Cyc_List_List*evs=_tmp835.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp8BA=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp8BB=_tmp8BA;void*_tmp8BD;int _tmp8BC;_LL78A: _tmp8BD=_tmp8BB.f1;_tmp8BC=_tmp8BB.f2;_LL78B:;
cvtenv.free_evars=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp8BD,_tmp8BC);}};};};}
# 4521
_npop_handler(0);goto _LL66E;
# 4296
;_pop_region(newr);};}case 10: _LL691: _tmp93D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp77B)->f1;_LL692:
# 4524
 for(0;_tmp93D != 0;_tmp93D=_tmp93D->tl){
struct _tuple12*_tmp8C9=(struct _tuple12*)_tmp93D->hd;
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp8C9).f2,1,0);
((*_tmp8C9).f1).real_const=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp8C9).f1).print_const,(*_tmp8C9).f2);}
# 4530
goto _LL66E;case 12: _LL693: _tmp93F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp77B)->f1;_tmp93E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp77B)->f2;_LL694: {
# 4534
struct _RegionHandle _tmp8CA=_new_region("aprev_rgn");struct _RegionHandle*aprev_rgn=& _tmp8CA;_push_region(aprev_rgn);{
struct Cyc_List_List*prev_fields=0;
for(0;_tmp93E != 0;_tmp93E=_tmp93E->tl){
struct Cyc_Absyn_Aggrfield*_tmp8CB=(struct Cyc_Absyn_Aggrfield*)_tmp93E->hd;struct Cyc_Absyn_Aggrfield*_tmp8CC=_tmp8CB;struct _dyneither_ptr*_tmp8DE;struct Cyc_Absyn_Tqual*_tmp8DD;void*_tmp8DC;struct Cyc_Absyn_Exp*_tmp8DB;struct Cyc_List_List*_tmp8DA;struct Cyc_Absyn_Exp*_tmp8D9;_LL78D: _tmp8DE=_tmp8CC->name;_tmp8DD=(struct Cyc_Absyn_Tqual*)& _tmp8CC->tq;_tmp8DC=_tmp8CC->type;_tmp8DB=_tmp8CC->width;_tmp8DA=_tmp8CC->attributes;_tmp8D9=_tmp8CC->requires_clause;_LL78E:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp8DE))
({struct Cyc_String_pa_PrintArg_struct _tmp8CF;_tmp8CF.tag=0;_tmp8CF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp8DE);({void*_tmp8CD[1]={& _tmp8CF};Cyc_Tcutil_terr(loc,({const char*_tmp8CE="duplicate field %s";_tag_dyneither(_tmp8CE,sizeof(char),19);}),_tag_dyneither(_tmp8CD,sizeof(void*),1));});});
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp8DE,({const char*_tmp8D0="";_tag_dyneither(_tmp8D0,sizeof(char),1);}))!= 0)
prev_fields=({struct Cyc_List_List*_tmp8D1=_region_malloc(aprev_rgn,sizeof(*_tmp8D1));_tmp8D1->hd=_tmp8DE;_tmp8D1->tl=prev_fields;_tmp8D1;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8DC,1,0);
_tmp8DD->real_const=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8DD->print_const,_tmp8DC);
# 4548
Cyc_Tcutil_check_bitfield(loc,te,_tmp8DC,_tmp8DB,_tmp8DE);
Cyc_Tcutil_check_field_atts(loc,_tmp8DE,_tmp8DA);
if(_tmp8D9 != 0){
# 4552
if(_tmp93F != Cyc_Absyn_UnionA)
({void*_tmp8D2=0;Cyc_Tcutil_terr(loc,({const char*_tmp8D3="@requires clause is only allowed on union members";_tag_dyneither(_tmp8D3,sizeof(char),50);}),_tag_dyneither(_tmp8D2,sizeof(void*),0));});{
struct _RegionHandle _tmp8D4=_new_region("temp");struct _RegionHandle*temp=& _tmp8D4;_push_region(temp);{
struct Cyc_Tcenv_Tenv*_tmp8D5=Cyc_Tcenv_allow_valueof(temp,te);
Cyc_Tcexp_tcExp(_tmp8D5,0,_tmp8D9);}
# 4558
if(!Cyc_Tcutil_is_integral(_tmp8D9))
({struct Cyc_String_pa_PrintArg_struct _tmp8D8;_tmp8D8.tag=0;_tmp8D8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp8D9->topt)));({void*_tmp8D6[1]={& _tmp8D8};Cyc_Tcutil_terr(loc,({const char*_tmp8D7="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp8D7,sizeof(char),54);}),_tag_dyneither(_tmp8D6,sizeof(void*),1));});});
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp8D9,te,cvtenv);
# 4554
;_pop_region(temp);};}}}
# 4565
_npop_handler(0);goto _LL66E;
# 4534
;_pop_region(aprev_rgn);}case 11: _LL695: _tmp941=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp77B)->f1).aggr_info;_tmp940=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp77B)->f1).targs;_LL696:
# 4568
{union Cyc_Absyn_AggrInfoU _tmp8DF=*_tmp941;union Cyc_Absyn_AggrInfoU _tmp8E0=_tmp8DF;struct Cyc_Absyn_Aggrdecl*_tmp904;enum Cyc_Absyn_AggrKind _tmp903;struct _tuple2*_tmp902;struct Cyc_Core_Opt*_tmp901;if((_tmp8E0.UnknownAggr).tag == 1){_LL790: _tmp903=((_tmp8E0.UnknownAggr).val).f1;_tmp902=((_tmp8E0.UnknownAggr).val).f2;_tmp901=((_tmp8E0.UnknownAggr).val).f3;_LL791: {
# 4570
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp8E1;_push_handler(& _tmp8E1);{int _tmp8E3=0;if(setjmp(_tmp8E1.handler))_tmp8E3=1;if(!_tmp8E3){
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp902);{
struct Cyc_Absyn_Aggrdecl*_tmp8E4=*adp;
if(_tmp8E4->kind != _tmp903){
if(_tmp8E4->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp8E8;_tmp8E8.tag=0;_tmp8E8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp902));({struct Cyc_String_pa_PrintArg_struct _tmp8E7;_tmp8E7.tag=0;_tmp8E7.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4576
Cyc_Absynpp_qvar2string(_tmp902));({void*_tmp8E5[2]={& _tmp8E7,& _tmp8E8};Cyc_Tcutil_terr(loc,({const char*_tmp8E6="expecting struct %s instead of union %s";_tag_dyneither(_tmp8E6,sizeof(char),40);}),_tag_dyneither(_tmp8E5,sizeof(void*),2));});});});else{
# 4579
({struct Cyc_String_pa_PrintArg_struct _tmp8EC;_tmp8EC.tag=0;_tmp8EC.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp902));({struct Cyc_String_pa_PrintArg_struct _tmp8EB;_tmp8EB.tag=0;_tmp8EB.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4579
Cyc_Absynpp_qvar2string(_tmp902));({void*_tmp8E9[2]={& _tmp8EB,& _tmp8EC};Cyc_Tcutil_terr(loc,({const char*_tmp8EA="expecting union %s instead of struct %s";_tag_dyneither(_tmp8EA,sizeof(char),40);}),_tag_dyneither(_tmp8E9,sizeof(void*),2));});});});}}
# 4582
if((unsigned int)_tmp901  && (int)_tmp901->v){
if(!((unsigned int)_tmp8E4->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp8E4->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp8EF;_tmp8EF.tag=0;_tmp8EF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp902));({void*_tmp8ED[1]={& _tmp8EF};Cyc_Tcutil_terr(loc,({const char*_tmp8EE="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp8EE,sizeof(char),42);}),_tag_dyneither(_tmp8ED,sizeof(void*),1));});});}
# 4588
*_tmp941=Cyc_Absyn_KnownAggr(adp);};
# 4572
;_pop_handler();}else{void*_tmp8E2=(void*)_exn_thrown;void*_tmp8F0=_tmp8E2;void*_tmp8F6;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8F0)->tag == Cyc_Dict_Absent){_LL795: _LL796: {
# 4592
struct Cyc_Absyn_Aggrdecl*_tmp8F1=({struct Cyc_Absyn_Aggrdecl*_tmp8F5=_cycalloc(sizeof(*_tmp8F5));_tmp8F5->kind=_tmp903;_tmp8F5->sc=Cyc_Absyn_Extern;_tmp8F5->name=_tmp902;_tmp8F5->tvs=0;_tmp8F5->impl=0;_tmp8F5->attributes=0;_tmp8F5->expected_mem_kind=0;_tmp8F5;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp8F1);
adp=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp902);
*_tmp941=Cyc_Absyn_KnownAggr(adp);
# 4597
if(*_tmp940 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8F4;_tmp8F4.tag=0;_tmp8F4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp902));({void*_tmp8F2[1]={& _tmp8F4};Cyc_Tcutil_terr(loc,({const char*_tmp8F3="declare parameterized type %s before using";_tag_dyneither(_tmp8F3,sizeof(char),43);}),_tag_dyneither(_tmp8F2,sizeof(void*),1));});});
return cvtenv;}
# 4602
goto _LL794;}}else{_LL797: _tmp8F6=_tmp8F0;_LL798:(int)_rethrow(_tmp8F6);}_LL794:;}};}
# 4604
_tmp904=*adp;goto _LL793;}}else{_LL792: _tmp904=*(_tmp8E0.KnownAggr).val;_LL793: {
# 4606
struct Cyc_List_List*tvs=_tmp904->tvs;
struct Cyc_List_List*ts=*_tmp940;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp8F7=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8F8=(void*)ts->hd;
# 4614
{struct _tuple0 _tmp8F9=({struct _tuple0 _tmp8FC;_tmp8FC.f1=Cyc_Absyn_compress_kb(_tmp8F7->kind);_tmp8FC.f2=_tmp8F8;_tmp8FC;});struct _tuple0 _tmp8FA=_tmp8F9;struct Cyc_Absyn_Tvar*_tmp8FB;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8FA.f1)->tag == 1){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8FA.f2)->tag == 2){_LL79A: _tmp8FB=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8FA.f2)->f1;_LL79B:
# 4616
 cvtenv.kind_env=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8FB);
cvtenv.free_vars=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8FB,1);
continue;}else{goto _LL79C;}}else{_LL79C: _LL79D:
 goto _LL799;}_LL799:;}{
# 4621
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4625
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8FF;_tmp8FF.tag=0;_tmp8FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp904->name));({void*_tmp8FD[1]={& _tmp8FF};Cyc_Tcutil_terr(loc,({const char*_tmp8FE="too many parameters for type %s";_tag_dyneither(_tmp8FE,sizeof(char),32);}),_tag_dyneither(_tmp8FD,sizeof(void*),1));});});
if(tvs != 0){
# 4629
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp900=_cycalloc(sizeof(*_tmp900));_tmp900->hd=e;_tmp900->tl=hidden_ts;_tmp900;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);}
# 4636
*_tmp940=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(*_tmp940,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4638
if((allow_abs_aggr  && _tmp904->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4643
_tmp904->expected_mem_kind=1;}}_LL78F:;}
# 4646
goto _LL66E;case 17: _LL697: _tmp945=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp77B)->f1;_tmp944=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp77B)->f2;_tmp943=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp77B)->f3;_tmp942=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp77B)->f4;_LL698: {
# 4649
struct Cyc_List_List*targs=*_tmp944;
# 4651
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp905;_push_handler(& _tmp905);{int _tmp907=0;if(setjmp(_tmp905.handler))_tmp907=1;if(!_tmp907){td=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp945);;_pop_handler();}else{void*_tmp906=(void*)_exn_thrown;void*_tmp908=_tmp906;void*_tmp90C;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp908)->tag == Cyc_Dict_Absent){_LL79F: _LL7A0:
# 4654
({struct Cyc_String_pa_PrintArg_struct _tmp90B;_tmp90B.tag=0;_tmp90B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp945));({void*_tmp909[1]={& _tmp90B};Cyc_Tcutil_terr(loc,({const char*_tmp90A="unbound typedef name %s";_tag_dyneither(_tmp90A,sizeof(char),24);}),_tag_dyneither(_tmp909,sizeof(void*),1));});});
return cvtenv;}else{_LL7A1: _tmp90C=_tmp908;_LL7A2:(int)_rethrow(_tmp90C);}_LL79E:;}};}
# 4657
*_tmp943=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct _RegionHandle _tmp90D=_new_region("temp");struct _RegionHandle*temp=& _tmp90D;_push_region(temp);{
struct Cyc_List_List*inst=0;
# 4663
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4668
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
inst=({struct Cyc_List_List*_tmp90E=_region_malloc(temp,sizeof(*_tmp90E));_tmp90E->hd=({struct _tuple16*_tmp90F=_region_malloc(temp,sizeof(*_tmp90F));_tmp90F->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp90F->f2=(void*)ts->hd;_tmp90F;});_tmp90E->tl=inst;_tmp90E;});}
# 4672
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp912;_tmp912.tag=0;_tmp912.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp945));({void*_tmp910[1]={& _tmp912};Cyc_Tcutil_terr(loc,({const char*_tmp911="too many parameters for typedef %s";_tag_dyneither(_tmp911,sizeof(char),35);}),_tag_dyneither(_tmp910,sizeof(void*),1));});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4677
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
hidden_ts=({struct Cyc_List_List*_tmp913=_cycalloc(sizeof(*_tmp913));_tmp913->hd=e;_tmp913->tl=hidden_ts;_tmp913;});
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);
inst=({struct Cyc_List_List*_tmp914=_cycalloc(sizeof(*_tmp914));_tmp914->hd=({struct _tuple16*_tmp915=_cycalloc(sizeof(*_tmp915));_tmp915->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp915->f2=e;_tmp915;});_tmp914->tl=inst;_tmp914;});}
# 4685
*_tmp944=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(targs,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));}
# 4687
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_rsubstitute(temp,inst,(void*)_check_null(td->defn));
*_tmp942=new_typ;}}
# 4694
_npop_handler(0);goto _LL66E;
# 4660
;_pop_region(temp);};}case 22: _LL699: _LL69A:
# 4695
 goto _LL69C;case 21: _LL69B: _LL69C:
 goto _LL69E;case 20: _LL69D: _LL69E:
 goto _LL66E;case 15: _LL69F: _tmp946=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp77B)->f1;_LL6A0:
# 4699
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp946,1,allow_abs_aggr);goto _LL66E;case 16: _LL6A1: _tmp948=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp77B)->f1;_tmp947=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp77B)->f2;_LL6A2:
# 4702
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp948,0,0);
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp947,1,0);
goto _LL66E;case 23: _LL6A3: _tmp949=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp77B)->f1;_LL6A4:
# 4706
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp949,1,1);goto _LL66E;case 25: _LL6A5: _tmp94A=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp77B)->f1;_LL6A6:
# 4708
 cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp94A,1,1);goto _LL66E;default: _LL6A7: _tmp94B=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp77B)->f1;_LL6A8:
# 4710
 for(0;_tmp94B != 0;_tmp94B=_tmp94B->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp94B->hd,1,1);}
goto _LL66E;}_LL66E:;}
# 4714
if(!Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),expected_kind))
({struct Cyc_String_pa_PrintArg_struct _tmp950;_tmp950.tag=0;_tmp950.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));({struct Cyc_String_pa_PrintArg_struct _tmp94F;_tmp94F.tag=0;_tmp94F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));({struct Cyc_String_pa_PrintArg_struct _tmp94E;_tmp94E.tag=0;_tmp94E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmp94C[3]={& _tmp94E,& _tmp94F,& _tmp950};Cyc_Tcutil_terr(loc,({const char*_tmp94D="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp94D,sizeof(char),51);}),_tag_dyneither(_tmp94C,sizeof(void*),3));});});});});
# 4718
return cvtenv;}
# 4726
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4728
{void*_tmp951=e->r;void*_tmp952=_tmp951;struct Cyc_Absyn_Exp*_tmp964;void*_tmp963;void*_tmp962;void*_tmp961;void*_tmp960;struct Cyc_Absyn_Exp*_tmp95F;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_Absyn_Exp*_tmp95D;struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_Absyn_Exp*_tmp95B;struct Cyc_Absyn_Exp*_tmp95A;struct Cyc_Absyn_Exp*_tmp959;struct Cyc_Absyn_Exp*_tmp958;struct Cyc_Absyn_Exp*_tmp957;struct Cyc_Absyn_Exp*_tmp956;struct Cyc_List_List*_tmp955;switch(*((int*)_tmp952)){case 0: _LL7A4: _LL7A5:
 goto _LL7A7;case 31: _LL7A6: _LL7A7:
 goto _LL7A9;case 32: _LL7A8: _LL7A9:
 goto _LL7AB;case 1: _LL7AA: _LL7AB:
 goto _LL7A3;case 2: _LL7AC: _tmp955=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp952)->f2;_LL7AD:
# 4734
 for(0;_tmp955 != 0;_tmp955=_tmp955->tl){
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp955->hd,te,cvtenv);}
goto _LL7A3;case 5: _LL7AE: _tmp958=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_tmp957=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp952)->f2;_tmp956=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp952)->f3;_LL7AF:
# 4738
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp958,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp957,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp956,te,cvtenv);
goto _LL7A3;case 6: _LL7B0: _tmp95A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_tmp959=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp952)->f2;_LL7B1:
 _tmp95C=_tmp95A;_tmp95B=_tmp959;goto _LL7B3;case 7: _LL7B2: _tmp95C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_tmp95B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp952)->f2;_LL7B3:
 _tmp95E=_tmp95C;_tmp95D=_tmp95B;goto _LL7B5;case 8: _LL7B4: _tmp95E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_tmp95D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp952)->f2;_LL7B5:
# 4745
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95E,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95D,te,cvtenv);
goto _LL7A3;case 13: _LL7B6: _tmp960=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_tmp95F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp952)->f2;_LL7B7:
# 4749
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp95F,te,cvtenv);
cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp960,1,0);
goto _LL7A3;case 18: _LL7B8: _tmp961=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_LL7B9:
 _tmp962=_tmp961;goto _LL7BB;case 16: _LL7BA: _tmp962=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_LL7BB:
# 4754
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp962,1,0);
goto _LL7A3;case 38: _LL7BC: _tmp963=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_LL7BD:
# 4757
 cvtenv=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp963,1,0);
goto _LL7A3;case 17: _LL7BE: _tmp964=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp952)->f1;_LL7BF:
# 4760
 cvtenv=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp964,te,cvtenv);
goto _LL7A3;default: _LL7C0: _LL7C1:
# 4763
({void*_tmp953=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp954="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp954,sizeof(char),66);}),_tag_dyneither(_tmp953,sizeof(void*),0));});}_LL7A3:;}
# 4765
return cvtenv;}
# 4768
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4774
struct Cyc_List_List*_tmp965=cvt.kind_env;
cvt=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);
# 4777
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp966=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp967=_tmp966;struct Cyc_Absyn_Tvar*_tmp968;_LL7C3: _tmp968=_tmp967.f1;_LL7C4:;
cvt.kind_env=Cyc_Tcutil_fast_add_free_tvar(_tmp965,_tmp968);}}
# 4785
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp969=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp96A=_tmp969;void*_tmp973;_LL7C6: _tmp973=_tmp96A.f1;_LL7C7:;{
void*_tmp96B=Cyc_Tcutil_compress(_tmp973);void*_tmp96C=_tmp96B;struct Cyc_Core_Opt**_tmp972;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp96C)->tag == 1){_LL7C9: _tmp972=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp96C)->f4;_LL7CA:
# 4789
 if(*_tmp972 == 0)
*_tmp972=({struct Cyc_Core_Opt*_tmp96D=_cycalloc(sizeof(*_tmp96D));_tmp96D->v=_tmp965;_tmp96D;});else{
# 4793
struct Cyc_List_List*_tmp96E=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp972))->v;
struct Cyc_List_List*_tmp96F=0;
for(0;_tmp96E != 0;_tmp96E=_tmp96E->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp965,(struct Cyc_Absyn_Tvar*)_tmp96E->hd))
_tmp96F=({struct Cyc_List_List*_tmp970=_cycalloc(sizeof(*_tmp970));_tmp970->hd=(struct Cyc_Absyn_Tvar*)_tmp96E->hd;_tmp970->tl=_tmp96F;_tmp970;});}
*_tmp972=({struct Cyc_Core_Opt*_tmp971=_cycalloc(sizeof(*_tmp971));_tmp971->v=_tmp96F;_tmp971;});}
# 4800
goto _LL7C8;}else{_LL7CB: _LL7CC:
 goto _LL7C8;}_LL7C8:;};}}
# 4804
return cvt;}
# 4810
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
# 4812
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp974=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp975=_tmp974;switch(((struct Cyc_Absyn_Kind*)_tmp975)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp975)->aliasqual){case Cyc_Absyn_Unique: _LL7CE: _LL7CF:
# 4816
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp976=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp977="can't unify evar with unique region!";_tag_dyneither(_tmp977,sizeof(char),37);}),_tag_dyneither(_tmp976,sizeof(void*),0));});
goto _LL7CD;case Cyc_Absyn_Aliasable: _LL7D0: _LL7D1:
 goto _LL7D3;default: _LL7D2: _LL7D3:
# 4821
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp978=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp979="can't unify evar with heap!";_tag_dyneither(_tmp979,sizeof(char),28);}),_tag_dyneither(_tmp978,sizeof(void*),0));});
goto _LL7CD;}case Cyc_Absyn_EffKind: _LL7D4: _LL7D5:
# 4824
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp97A=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp97B="can't unify evar with {}!";_tag_dyneither(_tmp97B,sizeof(char),26);}),_tag_dyneither(_tmp97A,sizeof(void*),0));});
goto _LL7CD;default: _LL7D6: _LL7D7:
# 4827
({struct Cyc_String_pa_PrintArg_struct _tmp97F;_tmp97F.tag=0;_tmp97F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));({struct Cyc_String_pa_PrintArg_struct _tmp97E;_tmp97E.tag=0;_tmp97E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));({void*_tmp97C[2]={& _tmp97E,& _tmp97F};Cyc_Tcutil_terr(loc,({const char*_tmp97D="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp97D,sizeof(char),52);}),_tag_dyneither(_tmp97C,sizeof(void*),2));});});});
goto _LL7CD;}_LL7CD:;}}
# 4839
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp980=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct _RegionHandle _tmp981=_new_region("temp");struct _RegionHandle*temp=& _tmp981;_push_region(temp);
{struct Cyc_Tcutil_CVTEnv _tmp982=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9A0;_tmp9A0.r=temp;_tmp9A0.kind_env=_tmp980;_tmp9A0.free_vars=0;_tmp9A0.free_evars=0;_tmp9A0.generalize_evars=generalize_evars;_tmp9A0.fn_result=0;_tmp9A0;}),expected_kind,1,t);
# 4848
struct Cyc_List_List*_tmp983=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp982.free_vars);
struct Cyc_List_List*_tmp984=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(temp,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp982.free_evars);
# 4852
if(_tmp980 != 0){
struct Cyc_List_List*_tmp985=0;
{struct Cyc_List_List*_tmp986=_tmp983;for(0;(unsigned int)_tmp986;_tmp986=_tmp986->tl){
struct Cyc_Absyn_Tvar*_tmp987=(struct Cyc_Absyn_Tvar*)_tmp986->hd;
int found=0;
{struct Cyc_List_List*_tmp988=_tmp980;for(0;(unsigned int)_tmp988;_tmp988=_tmp988->tl){
if(Cyc_Absyn_tvar_cmp(_tmp987,(struct Cyc_Absyn_Tvar*)_tmp988->hd)== 0){found=1;break;}}}
if(!found)
_tmp985=({struct Cyc_List_List*_tmp989=_region_malloc(temp,sizeof(*_tmp989));_tmp989->hd=(struct Cyc_Absyn_Tvar*)_tmp986->hd;_tmp989->tl=_tmp985;_tmp989;});}}
# 4862
_tmp983=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp985);}
# 4868
{struct Cyc_List_List*x=_tmp983;for(0;x != 0;x=x->tl){
void*_tmp98A=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp98B=_tmp98A;enum Cyc_Absyn_AliasQual _tmp999;struct Cyc_Core_Opt**_tmp998;struct Cyc_Absyn_Kind*_tmp997;struct Cyc_Core_Opt**_tmp996;struct Cyc_Core_Opt**_tmp995;struct Cyc_Core_Opt**_tmp994;struct Cyc_Core_Opt**_tmp993;struct Cyc_Core_Opt**_tmp992;struct Cyc_Core_Opt**_tmp991;switch(*((int*)_tmp98B)){case 1: _LL7D9: _tmp991=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp98B)->f1;_LL7DA:
 _tmp992=_tmp991;goto _LL7DC;case 2: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f2)->aliasqual == Cyc_Absyn_Top){_LL7DB: _tmp992=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f1;_LL7DC:
 _tmp993=_tmp992;goto _LL7DE;}else{goto _LL7E5;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f2)->aliasqual){case Cyc_Absyn_Top: _LL7DD: _tmp993=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f1;_LL7DE:
 _tmp994=_tmp993;goto _LL7E0;case Cyc_Absyn_Aliasable: _LL7DF: _tmp994=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f1;_LL7E0:
# 4874
*_tmp994=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);goto _LL7D8;default: _LL7E1: _tmp995=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f1;_LL7E2:
# 4876
*_tmp995=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);goto _LL7D8;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f2)->aliasqual == Cyc_Absyn_Top){_LL7E3: _tmp996=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f1;_LL7E4:
# 4878
*_tmp996=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);goto _LL7D8;}else{goto _LL7E5;}default: _LL7E5: _tmp998=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f1;_tmp997=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp98B)->f2;_LL7E6:
# 4880
*_tmp998=Cyc_Tcutil_kind_to_bound_opt(_tmp997);goto _LL7D8;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp98B)->f1)->kind == Cyc_Absyn_MemKind){_LL7E7: _tmp999=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp98B)->f1)->aliasqual;_LL7E8:
# 4882
({struct Cyc_String_pa_PrintArg_struct _tmp98F;_tmp98F.tag=0;_tmp98F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp990=_cycalloc_atomic(sizeof(*_tmp990));_tmp990->kind=Cyc_Absyn_MemKind;_tmp990->aliasqual=_tmp999;_tmp990;})));({struct Cyc_String_pa_PrintArg_struct _tmp98E;_tmp98E.tag=0;_tmp98E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));({void*_tmp98C[2]={& _tmp98E,& _tmp98F};Cyc_Tcutil_terr(loc,({const char*_tmp98D="type variable %s cannot have kind %s";_tag_dyneither(_tmp98D,sizeof(char),37);}),_tag_dyneither(_tmp98C,sizeof(void*),2));});});});
goto _LL7D8;}else{_LL7E9: _LL7EA:
 goto _LL7D8;}}_LL7D8:;}}
# 4889
if(_tmp983 != 0  || _tmp984 != 0){
{void*_tmp99A=Cyc_Tcutil_compress(t);void*_tmp99B=_tmp99A;struct Cyc_List_List**_tmp99C;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99B)->tag == 9){_LL7EC: _tmp99C=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp99B)->f1).tvars;_LL7ED:
# 4892
 if(*_tmp99C == 0){
# 4894
*_tmp99C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp983);
_tmp983=0;}
# 4897
goto _LL7EB;}else{_LL7EE: _LL7EF:
 goto _LL7EB;}_LL7EB:;}
# 4900
if(_tmp983 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp99F;_tmp99F.tag=0;_tmp99F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp983->hd)->name);({void*_tmp99D[1]={& _tmp99F};Cyc_Tcutil_terr(loc,({const char*_tmp99E="unbound type variable %s";_tag_dyneither(_tmp99E,sizeof(char),25);}),_tag_dyneither(_tmp99D,sizeof(void*),1));});});
Cyc_Tcutil_check_free_evars(_tmp984,t,loc);}}
# 4844
;_pop_region(temp);}
# 4911
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4914
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp9A1=Cyc_Tcutil_compress(t);void*_tmp9A2=_tmp9A1;struct Cyc_List_List*_tmp9AE;void*_tmp9AD;struct Cyc_Absyn_Tqual _tmp9AC;void*_tmp9AB;struct Cyc_List_List*_tmp9AA;struct Cyc_Absyn_Exp*_tmp9A9;struct Cyc_List_List*_tmp9A8;struct Cyc_Absyn_Exp*_tmp9A7;struct Cyc_List_List*_tmp9A6;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->tag == 9){_LL7F1: _tmp9AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).tvars;_tmp9AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).effect;_tmp9AC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).ret_tqual;_tmp9AB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).ret_typ;_tmp9AA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).args;_tmp9A9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).requires_clause;_tmp9A8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).requires_relns;_tmp9A7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).ensures_clause;_tmp9A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp9A2)->f1).ensures_relns;_LL7F2:
# 4917
 fd->tvs=_tmp9AE;
fd->effect=_tmp9AD;
{struct Cyc_List_List*_tmp9A3=fd->args;for(0;_tmp9A3 != 0;(_tmp9A3=_tmp9A3->tl,_tmp9AA=_tmp9AA->tl)){
# 4921
(*((struct _tuple10*)_tmp9A3->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp9AA))->hd)).f2;
(*((struct _tuple10*)_tmp9A3->hd)).f3=(*((struct _tuple10*)_tmp9AA->hd)).f3;}}
# 4924
fd->ret_tqual=_tmp9AC;
fd->ret_type=_tmp9AB;
(fd->ret_tqual).real_const=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp9AB);
fd->requires_clause=_tmp9A9;
fd->requires_relns=_tmp9A8;
fd->ensures_clause=_tmp9A7;
fd->ensures_relns=_tmp9A6;
goto _LL7F0;}else{_LL7F3: _LL7F4:
({void*_tmp9A4=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmp9A5="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp9A5,sizeof(char),38);}),_tag_dyneither(_tmp9A4,sizeof(void*),0));});}_LL7F0:;}{
# 4935
struct _RegionHandle _tmp9AF=_new_region("r");struct _RegionHandle*r=& _tmp9AF;_push_region(r);
Cyc_Tcutil_check_unique_vars(((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args),loc,({const char*_tmp9B0="function declaration has repeated parameter";_tag_dyneither(_tmp9B0,sizeof(char),44);}));
# 4940
fd->cached_typ=t;
# 4935
;_pop_region(r);};}
# 4945
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4951
struct _RegionHandle _tmp9B1=_new_region("r");struct _RegionHandle*r=& _tmp9B1;_push_region(r);
{struct Cyc_Tcutil_CVTEnv _tmp9B2=Cyc_Tcutil_check_valid_type(loc,te,({struct Cyc_Tcutil_CVTEnv _tmp9BA;_tmp9BA.r=r;_tmp9BA.kind_env=bound_tvars;_tmp9BA.free_vars=0;_tmp9BA.free_evars=0;_tmp9BA.generalize_evars=0;_tmp9BA.fn_result=0;_tmp9BA;}),expected_kind,allow_abs_aggr,t);
# 4955
struct Cyc_List_List*_tmp9B3=Cyc_Tcutil_remove_bound_tvars(r,((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp9B2.free_vars),bound_tvars);
# 4957
struct Cyc_List_List*_tmp9B4=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_rmap)(r,(void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp9B2.free_evars);
{struct Cyc_List_List*fs=_tmp9B3;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp9B5=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp9B9;_tmp9B9.tag=0;_tmp9B9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({struct Cyc_String_pa_PrintArg_struct _tmp9B8;_tmp9B8.tag=0;_tmp9B8.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9B5);({void*_tmp9B6[2]={& _tmp9B8,& _tmp9B9};Cyc_Tcutil_terr(loc,({const char*_tmp9B7="unbound type variable %s in type %s";_tag_dyneither(_tmp9B7,sizeof(char),36);}),_tag_dyneither(_tmp9B6,sizeof(void*),2));});});});}}
# 4962
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp9B4,t,loc);}
# 4952
;_pop_region(r);}
# 4967
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
tv->identity=Cyc_Tcutil_new_tvar_id();}
# 4972
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4978
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4981
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9BE;_tmp9BE.tag=0;_tmp9BE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));({struct Cyc_String_pa_PrintArg_struct _tmp9BD;_tmp9BD.tag=0;_tmp9BD.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp9BB[2]={& _tmp9BD,& _tmp9BE};Cyc_Tcutil_terr(loc,({const char*_tmp9BC="%s: %s";_tag_dyneither(_tmp9BC,sizeof(char),7);}),_tag_dyneither(_tmp9BB,sizeof(void*),2));});});});}}}
# 4987
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4991
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4995
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,tvs,loc,Cyc_Tcutil_tvar2string,({const char*_tmp9BF="duplicate type variable";_tag_dyneither(_tmp9BF,sizeof(char),24);}));}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 5009 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 5014
struct _RegionHandle _tmp9C0=_new_region("temp");struct _RegionHandle*temp=& _tmp9C0;_push_region(temp);
# 5018
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(Cyc_strcmp((struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name,({const char*_tmp9C1="";_tag_dyneither(_tmp9C1,sizeof(char),1);}))!= 0)
fields=({struct Cyc_List_List*_tmp9C2=_region_malloc(temp,sizeof(*_tmp9C2));_tmp9C2->hd=({struct _tuple32*_tmp9C3=_region_malloc(temp,sizeof(*_tmp9C3));_tmp9C3->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp9C3->f2=0;_tmp9C3;});_tmp9C2->tl=fields;_tmp9C2;});}}
# 5023
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 5025
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp9E9="struct";_tag_dyneither(_tmp9E9,sizeof(char),7);}):({const char*_tmp9EA="union";_tag_dyneither(_tmp9EA,sizeof(char),6);});
# 5028
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp9C4=(struct _tuple33*)des->hd;struct _tuple33*_tmp9C5=_tmp9C4;struct Cyc_List_List*_tmp9E1;void*_tmp9E0;_LL7F6: _tmp9E1=_tmp9C5->f1;_tmp9E0=_tmp9C5->f2;_LL7F7:;
if(_tmp9E1 == 0){
# 5033
struct Cyc_List_List*_tmp9C6=fields;
for(0;_tmp9C6 != 0;_tmp9C6=_tmp9C6->tl){
if(!(*((struct _tuple32*)_tmp9C6->hd)).f2){
(*((struct _tuple32*)_tmp9C6->hd)).f2=1;
(*((struct _tuple33*)des->hd)).f1=({struct Cyc_List_List*_tmp9C7=_cycalloc(sizeof(*_tmp9C7));_tmp9C7->hd=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp9C8=_cycalloc(sizeof(*_tmp9C8));_tmp9C8[0]=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp9C9;_tmp9C9.tag=1;_tmp9C9.f1=((*((struct _tuple32*)_tmp9C6->hd)).f1)->name;_tmp9C9;});_tmp9C8;});_tmp9C7->tl=0;_tmp9C7;});
ans=({struct Cyc_List_List*_tmp9CA=_region_malloc(rgn,sizeof(*_tmp9CA));_tmp9CA->hd=({struct _tuple34*_tmp9CB=_region_malloc(rgn,sizeof(*_tmp9CB));_tmp9CB->f1=(*((struct _tuple32*)_tmp9C6->hd)).f1;_tmp9CB->f2=_tmp9E0;_tmp9CB;});_tmp9CA->tl=ans;_tmp9CA;});
break;}}
# 5041
if(_tmp9C6 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9CE;_tmp9CE.tag=0;_tmp9CE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9CC[1]={& _tmp9CE};Cyc_Tcutil_terr(loc,({const char*_tmp9CD="too many arguments to %s";_tag_dyneither(_tmp9CD,sizeof(char),25);}),_tag_dyneither(_tmp9CC,sizeof(void*),1));});});}else{
if(_tmp9E1->tl != 0)
# 5045
({void*_tmp9CF=0;Cyc_Tcutil_terr(loc,({const char*_tmp9D0="multiple designators are not yet supported";_tag_dyneither(_tmp9D0,sizeof(char),43);}),_tag_dyneither(_tmp9CF,sizeof(void*),0));});else{
# 5048
void*_tmp9D1=(void*)_tmp9E1->hd;void*_tmp9D2=_tmp9D1;struct _dyneither_ptr*_tmp9DF;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp9D2)->tag == 0){_LL7F9: _LL7FA:
# 5050
({struct Cyc_String_pa_PrintArg_struct _tmp9D5;_tmp9D5.tag=0;_tmp9D5.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp9D3[1]={& _tmp9D5};Cyc_Tcutil_terr(loc,({const char*_tmp9D4="array designator used in argument to %s";_tag_dyneither(_tmp9D4,sizeof(char),40);}),_tag_dyneither(_tmp9D3,sizeof(void*),1));});});
goto _LL7F8;}else{_LL7FB: _tmp9DF=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp9D2)->f1;_LL7FC: {
# 5053
struct Cyc_List_List*_tmp9D6=fields;
for(0;_tmp9D6 != 0;_tmp9D6=_tmp9D6->tl){
if(Cyc_strptrcmp(_tmp9DF,((*((struct _tuple32*)_tmp9D6->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp9D6->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp9D9;_tmp9D9.tag=0;_tmp9D9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9DF);({void*_tmp9D7[1]={& _tmp9D9};Cyc_Tcutil_terr(loc,({const char*_tmp9D8="member %s has already been used as an argument";_tag_dyneither(_tmp9D8,sizeof(char),47);}),_tag_dyneither(_tmp9D7,sizeof(void*),1));});});
(*((struct _tuple32*)_tmp9D6->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp9DA=_region_malloc(rgn,sizeof(*_tmp9DA));_tmp9DA->hd=({struct _tuple34*_tmp9DB=_region_malloc(rgn,sizeof(*_tmp9DB));_tmp9DB->f1=(*((struct _tuple32*)_tmp9D6->hd)).f1;_tmp9DB->f2=_tmp9E0;_tmp9DB;});_tmp9DA->tl=ans;_tmp9DA;});
break;}}
# 5062
if(_tmp9D6 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp9DE;_tmp9DE.tag=0;_tmp9DE.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp9DF);({void*_tmp9DC[1]={& _tmp9DE};Cyc_Tcutil_terr(loc,({const char*_tmp9DD="bad field designator %s";_tag_dyneither(_tmp9DD,sizeof(char),24);}),_tag_dyneither(_tmp9DC,sizeof(void*),1));});});
goto _LL7F8;}}_LL7F8:;}}}
# 5067
if(aggr_kind == Cyc_Absyn_StructA)
# 5069
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp9E2=0;Cyc_Tcutil_terr(loc,({const char*_tmp9E3="too few arguments to struct";_tag_dyneither(_tmp9E3,sizeof(char),28);}),_tag_dyneither(_tmp9E2,sizeof(void*),0));});
break;}}else{
# 5076
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp9E4=0;Cyc_Tcutil_terr(loc,({const char*_tmp9E5="only one member of a union is allowed";_tag_dyneither(_tmp9E5,sizeof(char),38);}),_tag_dyneither(_tmp9E4,sizeof(void*),0));});
found=1;}}
# 5083
if(!found)({void*_tmp9E6=0;Cyc_Tcutil_terr(loc,({const char*_tmp9E7="missing member for union";_tag_dyneither(_tmp9E7,sizeof(char),25);}),_tag_dyneither(_tmp9E6,sizeof(void*),0));});}{
# 5086
struct Cyc_List_List*_tmp9E8=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0);return _tmp9E8;};};}
# 5018
;_pop_region(temp);}
# 5089
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9EB=Cyc_Tcutil_compress(t);void*_tmp9EC=_tmp9EB;void*_tmp9F0;union Cyc_Absyn_Constraint*_tmp9EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9EC)->tag == 5){_LL7FE: _tmp9F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9EC)->f1).elt_typ;_tmp9EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9EC)->f1).ptr_atts).bounds;_LL7FF: {
# 5092
void*_tmp9ED=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9EF);void*_tmp9EE=_tmp9ED;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9EE)->tag == 0){_LL803: _LL804:
# 5094
*elt_typ_dest=_tmp9F0;
return 1;}else{_LL805: _LL806:
 return 0;}_LL802:;}}else{_LL800: _LL801:
# 5098
 return 0;}_LL7FD:;}
# 5102
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9F1=Cyc_Tcutil_compress(t);void*_tmp9F2=_tmp9F1;void*_tmp9F4;union Cyc_Absyn_Constraint*_tmp9F3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F2)->tag == 5){_LL808: _tmp9F4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F2)->f1).elt_typ;_tmp9F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F2)->f1).ptr_atts).zero_term;_LL809:
# 5105
*elt_typ_dest=_tmp9F4;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9F3);}else{_LL80A: _LL80B:
 return 0;}_LL807:;}
# 5113
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5115
void*_tmp9F5=Cyc_Tcutil_compress(t);void*_tmp9F6=_tmp9F5;void*_tmp9FF;struct Cyc_Absyn_Tqual _tmp9FE;struct Cyc_Absyn_Exp*_tmp9FD;union Cyc_Absyn_Constraint*_tmp9FC;void*_tmp9FB;union Cyc_Absyn_Constraint*_tmp9FA;union Cyc_Absyn_Constraint*_tmp9F9;switch(*((int*)_tmp9F6)){case 5: _LL80D: _tmp9FB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F6)->f1).elt_typ;_tmp9FA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F6)->f1).ptr_atts).bounds;_tmp9F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9F6)->f1).ptr_atts).zero_term;_LL80E:
# 5117
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9F9)){
*ptr_type=t;
*elt_type=_tmp9FB;
{void*_tmp9F7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp9FA);void*_tmp9F8=_tmp9F7;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9F8)->tag == 0){_LL814: _LL815:
*is_dyneither=1;goto _LL813;}else{_LL816: _LL817:
*is_dyneither=0;goto _LL813;}_LL813:;}
# 5124
return 1;}else{
return 0;}case 8: _LL80F: _tmp9FF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F6)->f1).elt_type;_tmp9FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F6)->f1).tq;_tmp9FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F6)->f1).num_elts;_tmp9FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9F6)->f1).zero_term;_LL810:
# 5127
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9FC)){
*elt_type=_tmp9FF;
*is_dyneither=0;
*ptr_type=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);
return 1;}else{
return 0;}default: _LL811: _LL812:
 return 0;}_LL80C:;}
# 5140
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5142
void*_tmpA00=e1->r;void*_tmpA01=_tmpA00;struct Cyc_Absyn_Exp*_tmpA13;struct Cyc_Absyn_Exp*_tmpA12;struct Cyc_Absyn_Exp*_tmpA11;struct Cyc_Absyn_Exp*_tmpA10;struct Cyc_Absyn_Exp*_tmpA0F;struct Cyc_Absyn_Exp*_tmpA0E;switch(*((int*)_tmpA01)){case 13: _LL819: _LL81A:
# 5144
({struct Cyc_String_pa_PrintArg_struct _tmpA04;_tmpA04.tag=0;_tmpA04.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA02[1]={& _tmpA04};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA03="we have a cast in a lhs:  %s";_tag_dyneither(_tmpA03,sizeof(char),29);}),_tag_dyneither(_tmpA02,sizeof(void*),1));});});case 19: _LL81B: _tmpA0E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA01)->f1;_LL81C:
 _tmpA0F=_tmpA0E;goto _LL81E;case 22: _LL81D: _tmpA0F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA01)->f1;_LL81E:
# 5147
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA0F->topt),ptr_type,is_dyneither,elt_type);case 21: _LL81F: _tmpA10=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA01)->f1;_LL820:
 _tmpA11=_tmpA10;goto _LL822;case 20: _LL821: _tmpA11=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA01)->f1;_LL822:
# 5151
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA11->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA07;_tmpA07.tag=0;_tmpA07.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA05[1]={& _tmpA07};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA06="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmpA06,sizeof(char),51);}),_tag_dyneither(_tmpA05,sizeof(void*),1));});});
return 0;case 12: _LL823: _tmpA12=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA01)->f1;_LL824:
 _tmpA13=_tmpA12;goto _LL826;case 11: _LL825: _tmpA13=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA01)->f1;_LL826:
# 5157
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmpA13->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmpA0A;_tmpA0A.tag=0;_tmpA0A.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));({void*_tmpA08[1]={& _tmpA0A};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA09="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmpA09,sizeof(char),49);}),_tag_dyneither(_tmpA08,sizeof(void*),1));});});
return 0;case 1: _LL827: _LL828:
 return 0;default: _LL829: _LL82A:
({struct Cyc_String_pa_PrintArg_struct _tmpA0D;_tmpA0D.tag=0;_tmpA0D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));({void*_tmpA0B[1]={& _tmpA0D};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA0C="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmpA0C,sizeof(char),39);}),_tag_dyneither(_tmpA0B,sizeof(void*),1));});});}_LL818:;}
# 5166
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5177
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5180
void*_tmpA14=Cyc_Tcutil_compress(r);void*_tmpA15=_tmpA14;struct Cyc_Absyn_Tvar*_tmpA22;enum Cyc_Absyn_KindQual _tmpA21;enum Cyc_Absyn_AliasQual _tmpA20;switch(*((int*)_tmpA15)){case 22: _LL82C: _LL82D:
 return !must_be_unique;case 21: _LL82E: _LL82F:
 return 1;case 17: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA15)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA15)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA15)->f4 == 0){_LL830: _tmpA21=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA15)->f3)->kind)->v)->kind;_tmpA20=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpA15)->f3)->kind)->v)->aliasqual;_LL831:
# 5184
 return _tmpA21 == Cyc_Absyn_RgnKind  && (_tmpA20 == Cyc_Absyn_Unique  || _tmpA20 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL834;}}else{goto _LL834;}}else{goto _LL834;}case 2: _LL832: _tmpA22=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA15)->f1;_LL833: {
# 5188
struct Cyc_Absyn_Kind*_tmpA16=Cyc_Tcutil_tvar_kind(_tmpA22,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmpA17=_tmpA16;enum Cyc_Absyn_KindQual _tmpA1F;enum Cyc_Absyn_AliasQual _tmpA1E;_LL837: _tmpA1F=_tmpA17->kind;_tmpA1E=_tmpA17->aliasqual;_LL838:;
if(_tmpA1F == Cyc_Absyn_RgnKind  && (_tmpA1E == Cyc_Absyn_Unique  || _tmpA1E == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmpA18=Cyc_Absyn_compress_kb(_tmpA22->kind);void*_tmpA19=_tmpA18;struct Cyc_Core_Opt**_tmpA1D;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA19)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA19)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA19)->f2)->aliasqual == Cyc_Absyn_Top){_LL83A: _tmpA1D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA19)->f1;_LL83B:
# 5192
*_tmpA1D=({struct Cyc_Core_Opt*_tmpA1A=_cycalloc(sizeof(*_tmpA1A));_tmpA1A->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA1B=_cycalloc(sizeof(*_tmpA1B));_tmpA1B[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA1C;_tmpA1C.tag=2;_tmpA1C.f1=0;_tmpA1C.f2=& Cyc_Tcutil_rk;_tmpA1C;});_tmpA1B;});_tmpA1A;});
return 0;}else{goto _LL83C;}}else{goto _LL83C;}}else{_LL83C: _LL83D:
 return 1;}_LL839:;}
# 5197
return 0;}default: _LL834: _LL835:
 return 0;}_LL82B:;}
# 5204
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmpA23=Cyc_Tcutil_compress(t);void*_tmpA24=_tmpA23;struct Cyc_Absyn_Tvar*_tmpA33;void*_tmpA32;switch(*((int*)_tmpA24)){case 5: _LL83F: _tmpA32=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA24)->f1).ptr_atts).rgn;_LL840:
# 5207
 return Cyc_Tcutil_is_noalias_region(_tmpA32,must_be_unique);case 2: _LL841: _tmpA33=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmpA24)->f1;_LL842: {
# 5209
struct Cyc_Absyn_Kind*_tmpA25=Cyc_Tcutil_tvar_kind(_tmpA33,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmpA26=_tmpA25;enum Cyc_Absyn_KindQual _tmpA31;enum Cyc_Absyn_AliasQual _tmpA30;_LL846: _tmpA31=_tmpA26->kind;_tmpA30=_tmpA26->aliasqual;_LL847:;{
enum Cyc_Absyn_KindQual _tmpA27=_tmpA31;switch(_tmpA27){case Cyc_Absyn_BoxKind: _LL849: _LL84A:
 goto _LL84C;case Cyc_Absyn_AnyKind: _LL84B: _LL84C: goto _LL84E;case Cyc_Absyn_MemKind: _LL84D: _LL84E:
 if(_tmpA30 == Cyc_Absyn_Unique  || _tmpA30 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmpA28=Cyc_Absyn_compress_kb(_tmpA33->kind);void*_tmpA29=_tmpA28;struct Cyc_Core_Opt**_tmpA2F;enum Cyc_Absyn_KindQual _tmpA2E;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA29)->tag == 2){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA29)->f2)->aliasqual == Cyc_Absyn_Top){_LL852: _tmpA2F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA29)->f1;_tmpA2E=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmpA29)->f2)->kind;_LL853:
# 5215
*_tmpA2F=({struct Cyc_Core_Opt*_tmpA2A=_cycalloc(sizeof(*_tmpA2A));_tmpA2A->v=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmpA2B=_cycalloc(sizeof(*_tmpA2B));_tmpA2B[0]=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpA2C;_tmpA2C.tag=2;_tmpA2C.f1=0;_tmpA2C.f2=({struct Cyc_Absyn_Kind*_tmpA2D=_cycalloc_atomic(sizeof(*_tmpA2D));_tmpA2D->kind=_tmpA2E;_tmpA2D->aliasqual=Cyc_Absyn_Aliasable;_tmpA2D;});_tmpA2C;});_tmpA2B;});_tmpA2A;});
return 0;}else{goto _LL854;}}else{_LL854: _LL855:
# 5219
 return 1;}_LL851:;}
# 5222
return 0;default: _LL84F: _LL850:
 return 0;}_LL848:;};}default: _LL843: _LL844:
# 5225
 return 0;}_LL83E:;}
# 5228
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct _RegionHandle*rgn,void*t){
void*_tmpA34=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmpA34,0))return 1;{
void*_tmpA35=_tmpA34;union Cyc_Absyn_DatatypeFieldInfoU _tmpA49;struct Cyc_List_List*_tmpA48;union Cyc_Absyn_DatatypeInfoU _tmpA47;struct Cyc_List_List*_tmpA46;struct Cyc_List_List*_tmpA45;struct Cyc_Absyn_Aggrdecl**_tmpA44;struct Cyc_List_List*_tmpA43;struct Cyc_List_List*_tmpA42;switch(*((int*)_tmpA35)){case 10: _LL857: _tmpA42=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA35)->f1;_LL858:
# 5233
 while(_tmpA42 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,(*((struct _tuple12*)_tmpA42->hd)).f2))return 1;
_tmpA42=_tmpA42->tl;}
# 5237
return 0;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA35)->f1).aggr_info).KnownAggr).tag == 2){_LL859: _tmpA44=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA35)->f1).aggr_info).KnownAggr).val;_tmpA43=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA35)->f1).targs;_LL85A:
# 5239
 if((*_tmpA44)->impl == 0)return 0;else{
# 5241
struct Cyc_List_List*_tmpA36=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,(*_tmpA44)->tvs,_tmpA43);
struct Cyc_List_List*_tmpA37=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmpA44)->impl))->fields;
void*t;
while(_tmpA37 != 0){
t=_tmpA36 == 0?((struct Cyc_Absyn_Aggrfield*)_tmpA37->hd)->type: Cyc_Tcutil_rsubstitute(rgn,_tmpA36,((struct Cyc_Absyn_Aggrfield*)_tmpA37->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,t))return 1;
_tmpA37=_tmpA37->tl;}
# 5249
return 0;}}else{_LL85D: _LL85E:
# 5259
 return 0;}case 12: _LL85B: _tmpA45=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA35)->f2;_LL85C:
# 5252
 while(_tmpA45 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,((struct Cyc_Absyn_Aggrfield*)_tmpA45->hd)->type))return 1;
_tmpA45=_tmpA45->tl;}
# 5256
return 0;case 3: _LL85F: _tmpA47=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA35)->f1).datatype_info;_tmpA46=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmpA35)->f1).targs;_LL860: {
# 5261
union Cyc_Absyn_DatatypeInfoU _tmpA38=_tmpA47;struct Cyc_List_List*_tmpA3C;struct Cyc_Core_Opt*_tmpA3B;struct _tuple2*_tmpA3A;int _tmpA39;if((_tmpA38.UnknownDatatype).tag == 1){_LL866: _tmpA3A=((_tmpA38.UnknownDatatype).val).name;_tmpA39=((_tmpA38.UnknownDatatype).val).is_extensible;_LL867:
# 5264
 return 0;}else{_LL868: _tmpA3C=(*(_tmpA38.KnownDatatype).val)->tvs;_tmpA3B=(*(_tmpA38.KnownDatatype).val)->fields;_LL869:
# 5267
 return 0;}_LL865:;}case 4: _LL861: _tmpA49=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA35)->f1).field_info;_tmpA48=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmpA35)->f1).targs;_LL862: {
# 5270
union Cyc_Absyn_DatatypeFieldInfoU _tmpA3D=_tmpA49;struct Cyc_Absyn_Datatypedecl*_tmpA41;struct Cyc_Absyn_Datatypefield*_tmpA40;if((_tmpA3D.UnknownDatatypefield).tag == 1){_LL86B: _LL86C:
# 5273
 return 0;}else{_LL86D: _tmpA41=((_tmpA3D.KnownDatatypefield).val).f1;_tmpA40=((_tmpA3D.KnownDatatypefield).val).f2;_LL86E: {
# 5275
struct Cyc_List_List*_tmpA3E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA41->tvs,_tmpA48);
struct Cyc_List_List*_tmpA3F=_tmpA40->typs;
while(_tmpA3F != 0){
_tmpA34=_tmpA3E == 0?(*((struct _tuple12*)_tmpA3F->hd)).f2: Cyc_Tcutil_rsubstitute(rgn,_tmpA3E,(*((struct _tuple12*)_tmpA3F->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(rgn,_tmpA34))return 1;
_tmpA3F=_tmpA3F->tl;}
# 5282
return 0;}}_LL86A:;}default: _LL863: _LL864:
# 5284
 return 0;}_LL856:;};}
# 5288
static int Cyc_Tcutil_is_noalias_field(struct _RegionHandle*r,void*t,struct _dyneither_ptr*f){
void*_tmpA4A=Cyc_Tcutil_compress(t);void*_tmpA4B=_tmpA4A;struct Cyc_List_List*_tmpA56;struct Cyc_Absyn_Aggrdecl*_tmpA55;struct Cyc_List_List*_tmpA54;switch(*((int*)_tmpA4B)){case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4B)->f1).aggr_info).KnownAggr).tag == 2){_LL870: _tmpA55=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4B)->f1).aggr_info).KnownAggr).val;_tmpA54=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA4B)->f1).targs;_LL871:
# 5291
 _tmpA56=_tmpA55->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA55->impl))->fields;goto _LL873;}else{goto _LL874;}case 12: _LL872: _tmpA56=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA4B)->f2;_LL873: {
# 5293
struct Cyc_Absyn_Aggrfield*_tmpA4C=Cyc_Absyn_lookup_field(_tmpA56,f);
{struct Cyc_Absyn_Aggrfield*_tmpA4D=_tmpA4C;void*_tmpA50;if(_tmpA4D == 0){_LL877: _LL878:
({void*_tmpA4E=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA4F="is_noalias_field: missing field";_tag_dyneither(_tmpA4F,sizeof(char),32);}),_tag_dyneither(_tmpA4E,sizeof(void*),0));});}else{_LL879: _tmpA50=_tmpA4D->type;_LL87A:
# 5297
 return Cyc_Tcutil_is_noalias_pointer_or_aggr(r,_tmpA50);}_LL876:;}
# 5299
return 0;}default: _LL874: _LL875:
# 5301
({struct Cyc_String_pa_PrintArg_struct _tmpA53;_tmpA53.tag=0;_tmpA53.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t));({void*_tmpA51[1]={& _tmpA53};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA52="is_noalias_field: invalid type |%s|";_tag_dyneither(_tmpA52,sizeof(char),36);}),_tag_dyneither(_tmpA51,sizeof(void*),1));});});}_LL86F:;}
# 5309
int Cyc_Tcutil_is_noalias_path(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e){
void*_tmpA57=e->r;void*_tmpA58=_tmpA57;struct Cyc_Absyn_Stmt*_tmpA6E;struct Cyc_Absyn_Exp*_tmpA6D;struct Cyc_Absyn_Exp*_tmpA6C;struct Cyc_Absyn_Exp*_tmpA6B;struct Cyc_Absyn_Exp*_tmpA6A;struct Cyc_Absyn_Exp*_tmpA69;struct Cyc_Absyn_Exp*_tmpA68;struct Cyc_Absyn_Exp*_tmpA67;struct _dyneither_ptr*_tmpA66;struct Cyc_Absyn_Exp*_tmpA65;struct Cyc_Absyn_Exp*_tmpA64;switch(*((int*)_tmpA58)){case 1: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA58)->f1)->tag == 1){_LL87C: _LL87D:
 return 0;}else{goto _LL88E;}case 21: _LL87E: _tmpA64=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_LL87F:
 _tmpA65=_tmpA64;goto _LL881;case 19: _LL880: _tmpA65=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_LL881:
# 5314
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA65->topt),1) && 
Cyc_Tcutil_is_noalias_path(r,_tmpA65);case 20: _LL882: _tmpA67=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_tmpA66=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA58)->f2;_LL883:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA67);case 22: _LL884: _tmpA69=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_tmpA68=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA58)->f2;_LL885: {
# 5318
void*_tmpA59=Cyc_Tcutil_compress((void*)_check_null(_tmpA69->topt));void*_tmpA5A=_tmpA59;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA5A)->tag == 10){_LL891: _LL892:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA69);}else{_LL893: _LL894:
 return 0;}_LL890:;}case 5: _LL886: _tmpA6B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA58)->f2;_tmpA6A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA58)->f3;_LL887:
# 5323
 return Cyc_Tcutil_is_noalias_path(r,_tmpA6B) && Cyc_Tcutil_is_noalias_path(r,_tmpA6A);case 8: _LL888: _tmpA6C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA58)->f2;_LL889:
 _tmpA6D=_tmpA6C;goto _LL88B;case 13: _LL88A: _tmpA6D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA58)->f2;_LL88B:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA6D);case 36: _LL88C: _tmpA6E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpA58)->f1;_LL88D:
# 5327
 while(1){
void*_tmpA5B=_tmpA6E->r;void*_tmpA5C=_tmpA5B;struct Cyc_Absyn_Exp*_tmpA63;struct Cyc_Absyn_Decl*_tmpA62;struct Cyc_Absyn_Stmt*_tmpA61;struct Cyc_Absyn_Stmt*_tmpA60;struct Cyc_Absyn_Stmt*_tmpA5F;switch(*((int*)_tmpA5C)){case 2: _LL896: _tmpA60=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA5C)->f1;_tmpA5F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA5C)->f2;_LL897:
 _tmpA6E=_tmpA5F;goto _LL895;case 12: _LL898: _tmpA62=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA5C)->f1;_tmpA61=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA5C)->f2;_LL899:
 _tmpA6E=_tmpA61;goto _LL895;case 1: _LL89A: _tmpA63=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA5C)->f1;_LL89B:
 return Cyc_Tcutil_is_noalias_path(r,_tmpA63);default: _LL89C: _LL89D:
({void*_tmpA5D=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpA5E="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA5E,sizeof(char),40);}),_tag_dyneither(_tmpA5D,sizeof(void*),0));});}_LL895:;}default: _LL88E: _LL88F:
# 5335
 return 1;}_LL87B:;}
# 5352 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5354
struct _tuple18 bogus_ans=({struct _tuple18 _tmpAB6;_tmpAB6.f1=0;_tmpAB6.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpAB6;});
void*_tmpA6F=e->r;void*_tmpA70=_tmpA6F;struct Cyc_Absyn_Exp*_tmpAB5;struct Cyc_Absyn_Exp*_tmpAB4;struct Cyc_Absyn_Exp*_tmpAB3;struct Cyc_Absyn_Exp*_tmpAB2;struct _dyneither_ptr*_tmpAB1;int _tmpAB0;struct Cyc_Absyn_Exp*_tmpAAF;struct _dyneither_ptr*_tmpAAE;int _tmpAAD;void*_tmpAAC;switch(*((int*)_tmpA70)){case 1: _LL89F: _tmpAAC=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA70)->f1;_LL8A0: {
# 5358
void*_tmpA71=_tmpAAC;struct Cyc_Absyn_Vardecl*_tmpA7E;struct Cyc_Absyn_Vardecl*_tmpA7D;struct Cyc_Absyn_Vardecl*_tmpA7C;struct Cyc_Absyn_Vardecl*_tmpA7B;switch(*((int*)_tmpA71)){case 0: _LL8AC: _LL8AD:
 goto _LL8AF;case 2: _LL8AE: _LL8AF:
 return bogus_ans;case 1: _LL8B0: _tmpA7B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA71)->f1;_LL8B1: {
# 5362
void*_tmpA72=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA73=_tmpA72;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA73)->tag == 8){_LL8B9: _LL8BA:
# 5364
 return({struct _tuple18 _tmpA74;_tmpA74.f1=1;_tmpA74.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA74;});}else{_LL8BB: _LL8BC:
 return({struct _tuple18 _tmpA75;_tmpA75.f1=(_tmpA7B->tq).real_const;_tmpA75.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA75;});}_LL8B8:;}case 4: _LL8B2: _tmpA7C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA71)->f1;_LL8B3: {
# 5368
void*_tmpA76=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA77=_tmpA76;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA77)->tag == 8){_LL8BE: _LL8BF:
 return({struct _tuple18 _tmpA78;_tmpA78.f1=1;_tmpA78.f2=(void*)_check_null(_tmpA7C->rgn);_tmpA78;});}else{_LL8C0: _LL8C1:
# 5371
 _tmpA7C->escapes=1;
return({struct _tuple18 _tmpA79;_tmpA79.f1=(_tmpA7C->tq).real_const;_tmpA79.f2=(void*)_check_null(_tmpA7C->rgn);_tmpA79;});}_LL8BD:;}case 5: _LL8B4: _tmpA7D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA71)->f1;_LL8B5:
# 5374
 _tmpA7E=_tmpA7D;goto _LL8B7;default: _LL8B6: _tmpA7E=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA71)->f1;_LL8B7:
# 5376
 _tmpA7E->escapes=1;
return({struct _tuple18 _tmpA7A;_tmpA7A.f1=(_tmpA7E->tq).real_const;_tmpA7A.f2=(void*)_check_null(_tmpA7E->rgn);_tmpA7A;});}_LL8AB:;}case 20: _LL8A1: _tmpAAF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA70)->f1;_tmpAAE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA70)->f2;_tmpAAD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA70)->f3;_LL8A2:
# 5381
 if(_tmpAAD)return bogus_ans;{
# 5384
void*_tmpA7F=Cyc_Tcutil_compress((void*)_check_null(_tmpAAF->topt));void*_tmpA80=_tmpA7F;struct Cyc_Absyn_Aggrdecl*_tmpA8E;struct Cyc_List_List*_tmpA8D;switch(*((int*)_tmpA80)){case 12: _LL8C3: _tmpA8D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA80)->f2;_LL8C4: {
# 5386
struct Cyc_Absyn_Aggrfield*_tmpA81=Cyc_Absyn_lookup_field(_tmpA8D,_tmpAAE);
if(_tmpA81 != 0  && _tmpA81->width == 0){
struct _tuple18 _tmpA82=Cyc_Tcutil_addressof_props(te,_tmpAAF);struct _tuple18 _tmpA83=_tmpA82;int _tmpA86;void*_tmpA85;_LL8CA: _tmpA86=_tmpA83.f1;_tmpA85=_tmpA83.f2;_LL8CB:;
return({struct _tuple18 _tmpA84;_tmpA84.f1=(_tmpA81->tq).real_const  || _tmpA86;_tmpA84.f2=_tmpA85;_tmpA84;});}
# 5391
return bogus_ans;}case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA80)->f1).aggr_info).KnownAggr).tag == 2){_LL8C5: _tmpA8E=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA80)->f1).aggr_info).KnownAggr).val;_LL8C6: {
# 5393
struct Cyc_Absyn_Aggrfield*_tmpA87=Cyc_Absyn_lookup_decl_field(_tmpA8E,_tmpAAE);
if(_tmpA87 != 0  && _tmpA87->width == 0){
struct _tuple18 _tmpA88=Cyc_Tcutil_addressof_props(te,_tmpAAF);struct _tuple18 _tmpA89=_tmpA88;int _tmpA8C;void*_tmpA8B;_LL8CD: _tmpA8C=_tmpA89.f1;_tmpA8B=_tmpA89.f2;_LL8CE:;
return({struct _tuple18 _tmpA8A;_tmpA8A.f1=(_tmpA87->tq).real_const  || _tmpA8C;_tmpA8A.f2=_tmpA8B;_tmpA8A;});}
# 5398
return bogus_ans;}}else{goto _LL8C7;}default: _LL8C7: _LL8C8:
 return bogus_ans;}_LL8C2:;};case 21: _LL8A3: _tmpAB2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA70)->f1;_tmpAB1=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA70)->f2;_tmpAB0=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA70)->f3;_LL8A4:
# 5403
 if(_tmpAB0)return bogus_ans;{
# 5407
void*_tmpA8F=Cyc_Tcutil_compress((void*)_check_null(_tmpAB2->topt));void*_tmpA90=_tmpA8F;void*_tmpA97;void*_tmpA96;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA90)->tag == 5){_LL8D0: _tmpA97=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA90)->f1).elt_typ;_tmpA96=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA90)->f1).ptr_atts).rgn;_LL8D1: {
# 5409
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpA91=Cyc_Tcutil_compress(_tmpA97);void*_tmpA92=_tmpA91;struct Cyc_Absyn_Aggrdecl*_tmpA94;struct Cyc_List_List*_tmpA93;switch(*((int*)_tmpA92)){case 12: _LL8D5: _tmpA93=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA92)->f2;_LL8D6:
# 5412
 finfo=Cyc_Absyn_lookup_field(_tmpA93,_tmpAB1);goto _LL8D4;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA92)->f1).aggr_info).KnownAggr).tag == 2){_LL8D7: _tmpA94=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA92)->f1).aggr_info).KnownAggr).val;_LL8D8:
# 5414
 finfo=Cyc_Absyn_lookup_decl_field(_tmpA94,_tmpAB1);goto _LL8D4;}else{goto _LL8D9;}default: _LL8D9: _LL8DA:
 return bogus_ans;}_LL8D4:;}
# 5417
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpA95;_tmpA95.f1=(finfo->tq).real_const;_tmpA95.f2=_tmpA96;_tmpA95;});
return bogus_ans;}}else{_LL8D2: _LL8D3:
 return bogus_ans;}_LL8CF:;};case 19: _LL8A5: _tmpAB3=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA70)->f1;_LL8A6: {
# 5424
void*_tmpA98=Cyc_Tcutil_compress((void*)_check_null(_tmpAB3->topt));void*_tmpA99=_tmpA98;struct Cyc_Absyn_Tqual _tmpA9C;void*_tmpA9B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA99)->tag == 5){_LL8DC: _tmpA9C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA99)->f1).elt_tq;_tmpA9B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA99)->f1).ptr_atts).rgn;_LL8DD:
# 5426
 return({struct _tuple18 _tmpA9A;_tmpA9A.f1=_tmpA9C.real_const;_tmpA9A.f2=_tmpA9B;_tmpA9A;});}else{_LL8DE: _LL8DF:
 return bogus_ans;}_LL8DB:;}case 22: _LL8A7: _tmpAB5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA70)->f1;_tmpAB4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA70)->f2;_LL8A8: {
# 5432
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpAB5->topt));
void*_tmpA9D=t;struct Cyc_Absyn_Tqual _tmpAA9;struct Cyc_Absyn_Tqual _tmpAA8;void*_tmpAA7;struct Cyc_List_List*_tmpAA6;switch(*((int*)_tmpA9D)){case 10: _LL8E1: _tmpAA6=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA9D)->f1;_LL8E2: {
# 5436
struct _tuple14 _tmpA9E=Cyc_Evexp_eval_const_uint_exp(_tmpAB4);struct _tuple14 _tmpA9F=_tmpA9E;unsigned int _tmpAA3;int _tmpAA2;_LL8EA: _tmpAA3=_tmpA9F.f1;_tmpAA2=_tmpA9F.f2;_LL8EB:;
if(!_tmpAA2)
return bogus_ans;{
struct _tuple12*_tmpAA0=Cyc_Absyn_lookup_tuple_field(_tmpAA6,(int)_tmpAA3);
if(_tmpAA0 != 0)
return({struct _tuple18 _tmpAA1;_tmpAA1.f1=((*_tmpAA0).f1).real_const;_tmpAA1.f2=(Cyc_Tcutil_addressof_props(te,_tmpAB5)).f2;_tmpAA1;});
return bogus_ans;};}case 5: _LL8E3: _tmpAA8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA9D)->f1).elt_tq;_tmpAA7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA9D)->f1).ptr_atts).rgn;_LL8E4:
# 5444
 return({struct _tuple18 _tmpAA4;_tmpAA4.f1=_tmpAA8.real_const;_tmpAA4.f2=_tmpAA7;_tmpAA4;});case 8: _LL8E5: _tmpAA9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA9D)->f1).tq;_LL8E6:
# 5450
 return({struct _tuple18 _tmpAA5;_tmpAA5.f1=_tmpAA9.real_const;_tmpAA5.f2=(Cyc_Tcutil_addressof_props(te,_tmpAB5)).f2;_tmpAA5;});default: _LL8E7: _LL8E8:
 return bogus_ans;}_LL8E0:;}default: _LL8A9: _LL8AA:
# 5454
({void*_tmpAAA=0;Cyc_Tcutil_terr(e->loc,({const char*_tmpAAB="unary & applied to non-lvalue";_tag_dyneither(_tmpAAB,sizeof(char),30);}),_tag_dyneither(_tmpAAA,sizeof(void*),0));});
return bogus_ans;}_LL89E:;}
# 5461
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
b=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);{
void*_tmpAB7=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpAB8=_tmpAB7;struct Cyc_Absyn_Exp*_tmpAC1;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAB8)->tag == 0){_LL8ED: _LL8EE:
 return;}else{_LL8EF: _tmpAC1=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAB8)->f1;_LL8F0: {
# 5466
struct _tuple14 _tmpAB9=Cyc_Evexp_eval_const_uint_exp(_tmpAC1);struct _tuple14 _tmpABA=_tmpAB9;unsigned int _tmpAC0;int _tmpABF;_LL8F2: _tmpAC0=_tmpABA.f1;_tmpABF=_tmpABA.f2;_LL8F3:;
if(_tmpABF  && _tmpAC0 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpABE;_tmpABE.tag=1;_tmpABE.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpABD;_tmpABD.tag=1;_tmpABD.f1=(unsigned long)((int)_tmpAC0);({void*_tmpABB[2]={& _tmpABD,& _tmpABE};Cyc_Tcutil_terr(loc,({const char*_tmpABC="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpABC,sizeof(char),39);}),_tag_dyneither(_tmpABB,sizeof(void*),2));});});});
return;}}_LL8EC:;};}
# 5473
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0,b);}
# 5477
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpAC2=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpAC3=_tmpAC2;struct Cyc_Absyn_Exp*_tmpAC8;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAC3)->tag == 1){_LL8F5: _tmpAC8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpAC3)->f1;_LL8F6: {
# 5480
struct _tuple14 _tmpAC4=Cyc_Evexp_eval_const_uint_exp(_tmpAC8);struct _tuple14 _tmpAC5=_tmpAC4;unsigned int _tmpAC7;int _tmpAC6;_LL8FA: _tmpAC7=_tmpAC5.f1;_tmpAC6=_tmpAC5.f2;_LL8FB:;
return _tmpAC6  && _tmpAC7 == 1;}}else{_LL8F7: _LL8F8:
 return 0;}_LL8F4:;}
# 5486
int Cyc_Tcutil_bits_only(void*t){
void*_tmpAC9=Cyc_Tcutil_compress(t);void*_tmpACA=_tmpAC9;struct Cyc_List_List*_tmpAD4;struct Cyc_Absyn_Aggrdecl*_tmpAD3;struct Cyc_List_List*_tmpAD2;struct Cyc_List_List*_tmpAD1;void*_tmpAD0;union Cyc_Absyn_Constraint*_tmpACF;switch(*((int*)_tmpACA)){case 0: _LL8FD: _LL8FE:
 goto _LL900;case 6: _LL8FF: _LL900:
 goto _LL902;case 7: _LL901: _LL902:
 return 1;case 13: _LL903: _LL904:
 goto _LL906;case 14: _LL905: _LL906:
 return 0;case 8: _LL907: _tmpAD0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpACA)->f1).elt_type;_tmpACF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpACA)->f1).zero_term;_LL908:
# 5496
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpACF) && Cyc_Tcutil_bits_only(_tmpAD0);case 10: _LL909: _tmpAD1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpACA)->f1;_LL90A:
# 5498
 for(0;_tmpAD1 != 0;_tmpAD1=_tmpAD1->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpAD1->hd)).f2))return 0;}
return 1;case 11: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpACA)->f1).aggr_info).UnknownAggr).tag == 1){_LL90B: _LL90C:
 return 0;}else{_LL90D: _tmpAD3=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpACA)->f1).aggr_info).KnownAggr).val;_tmpAD2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpACA)->f1).targs;_LL90E:
# 5503
 if(_tmpAD3->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAD3->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpACB=_new_region("rgn");struct _RegionHandle*rgn=& _tmpACB;_push_region(rgn);
{struct Cyc_List_List*_tmpACC=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpAD3->tvs,_tmpAD2);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAD3->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpACC,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpACD=0;_npop_handler(0);return _tmpACD;}}}{
int _tmpACE=1;_npop_handler(0);return _tmpACE;};}
# 5510
;_pop_region(rgn);};};}case 12: _LL90F: _tmpAD4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpACA)->f2;_LL910:
# 5515
 for(0;_tmpAD4 != 0;_tmpAD4=_tmpAD4->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpAD4->hd)->type))return 0;}
return 1;default: _LL911: _LL912:
 return 0;}_LL8FC:;}
# 5523
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpAD5=e->r;void*_tmpAD6=_tmpAD5;struct Cyc_Absyn_Exp*_tmpAD8;struct Cyc_Absyn_Exp*_tmpAD7;switch(*((int*)_tmpAD6)){case 1: _LL914: _LL915:
 return 1;case 11: _LL916: _tmpAD7=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAD6)->f1;_LL917:
 _tmpAD8=_tmpAD7;goto _LL919;case 12: _LL918: _tmpAD8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAD6)->f1;_LL919:
 e=_tmpAD8;continue;default: _LL91A: _LL91B:
# 5530
 return 0;}_LL913:;}}
# 5540
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpAD9=e->r;void*_tmpADA=_tmpAD9;struct Cyc_List_List*_tmpAF8;struct Cyc_List_List*_tmpAF7;enum Cyc_Absyn_Primop _tmpAF6;struct Cyc_List_List*_tmpAF5;struct Cyc_List_List*_tmpAF4;struct Cyc_List_List*_tmpAF3;struct Cyc_List_List*_tmpAF2;struct Cyc_Absyn_Exp*_tmpAF1;struct Cyc_Absyn_Exp*_tmpAF0;struct Cyc_Absyn_Exp*_tmpAEF;struct Cyc_Absyn_Exp*_tmpAEE;void*_tmpAED;struct Cyc_Absyn_Exp*_tmpAEC;void*_tmpAEB;struct Cyc_Absyn_Exp*_tmpAEA;struct Cyc_Absyn_Exp*_tmpAE9;struct Cyc_Absyn_Exp*_tmpAE8;struct Cyc_Absyn_Exp*_tmpAE7;struct Cyc_Absyn_Exp*_tmpAE6;struct Cyc_Absyn_Exp*_tmpAE5;struct Cyc_Absyn_Exp*_tmpAE4;struct Cyc_Absyn_Exp*_tmpAE3;void*_tmpAE2;switch(*((int*)_tmpADA)){case 0: _LL91D: _LL91E:
 goto _LL920;case 16: _LL91F: _LL920:
 goto _LL922;case 17: _LL921: _LL922:
 goto _LL924;case 18: _LL923: _LL924:
 goto _LL926;case 31: _LL925: _LL926:
 goto _LL928;case 32: _LL927: _LL928:
 return 1;case 1: _LL929: _tmpAE2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_LL92A: {
# 5551
void*_tmpADB=_tmpAE2;struct Cyc_Absyn_Vardecl*_tmpAE1;struct Cyc_Absyn_Vardecl*_tmpAE0;switch(*((int*)_tmpADB)){case 2: _LL94C: _LL94D:
 return 1;case 1: _LL94E: _tmpAE0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpADB)->f1;_LL94F: {
# 5554
void*_tmpADC=Cyc_Tcutil_compress(_tmpAE0->type);void*_tmpADD=_tmpADC;switch(*((int*)_tmpADD)){case 8: _LL957: _LL958:
 goto _LL95A;case 9: _LL959: _LL95A:
 return 1;default: _LL95B: _LL95C:
 return var_okay;}_LL956:;}case 4: _LL950: _tmpAE1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpADB)->f1;_LL951:
# 5561
 if(_tmpAE1->sc == Cyc_Absyn_Static){
void*_tmpADE=Cyc_Tcutil_compress(_tmpAE1->type);void*_tmpADF=_tmpADE;switch(*((int*)_tmpADF)){case 8: _LL95E: _LL95F:
 goto _LL961;case 9: _LL960: _LL961:
 return 1;default: _LL962: _LL963:
 return var_okay;}_LL95D:;}else{
# 5568
return var_okay;}case 0: _LL952: _LL953:
 return 0;default: _LL954: _LL955:
 return var_okay;}_LL94B:;}case 5: _LL92B: _tmpAE5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_tmpAE4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpADA)->f2;_tmpAE3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpADA)->f3;_LL92C:
# 5574
 return(Cyc_Tcutil_cnst_exp(0,_tmpAE5) && 
Cyc_Tcutil_cnst_exp(0,_tmpAE4)) && 
Cyc_Tcutil_cnst_exp(0,_tmpAE3);case 8: _LL92D: _tmpAE7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_tmpAE6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpADA)->f2;_LL92E:
# 5578
 return Cyc_Tcutil_cnst_exp(0,_tmpAE7) && Cyc_Tcutil_cnst_exp(0,_tmpAE6);case 11: _LL92F: _tmpAE8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_LL930:
 _tmpAE9=_tmpAE8;goto _LL932;case 12: _LL931: _tmpAE9=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_LL932:
# 5581
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAE9);case 13: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpADA)->f4 == Cyc_Absyn_No_coercion){_LL933: _tmpAEB=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_tmpAEA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpADA)->f2;_LL934:
# 5583
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAEA);}else{_LL935: _tmpAED=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_tmpAEC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpADA)->f2;_LL936:
# 5586
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpAEC);}case 14: _LL937: _tmpAEE=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_LL938:
# 5588
 return Cyc_Tcutil_cnst_exp(1,_tmpAEE);case 26: _LL939: _tmpAF0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpADA)->f2;_tmpAEF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpADA)->f3;_LL93A:
# 5590
 return Cyc_Tcutil_cnst_exp(0,_tmpAF0) && Cyc_Tcutil_cnst_exp(0,_tmpAEF);case 27: _LL93B: _tmpAF1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_LL93C:
# 5592
 return Cyc_Tcutil_cnst_exp(0,_tmpAF1);case 25: _LL93D: _tmpAF2=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_LL93E:
 _tmpAF3=_tmpAF2;goto _LL940;case 29: _LL93F: _tmpAF3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpADA)->f2;_LL940:
 _tmpAF4=_tmpAF3;goto _LL942;case 28: _LL941: _tmpAF4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpADA)->f3;_LL942:
# 5596
 for(0;_tmpAF4 != 0;_tmpAF4=_tmpAF4->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpAF4->hd)).f2))
return 0;}
return 1;case 2: _LL943: _tmpAF6=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_tmpAF5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpADA)->f2;_LL944:
# 5601
 _tmpAF7=_tmpAF5;goto _LL946;case 23: _LL945: _tmpAF7=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_LL946:
 _tmpAF8=_tmpAF7;goto _LL948;case 30: _LL947: _tmpAF8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpADA)->f1;_LL948:
# 5604
 for(0;_tmpAF8 != 0;_tmpAF8=_tmpAF8->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpAF8->hd))
return 0;}
return 1;default: _LL949: _LL94A:
 return 0;}_LL91C:;}
# 5612
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5616
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5618
int Cyc_Tcutil_supports_default(void*t){
void*_tmpAF9=Cyc_Tcutil_compress(t);void*_tmpAFA=_tmpAF9;struct Cyc_List_List*_tmpB04;union Cyc_Absyn_AggrInfoU _tmpB03;struct Cyc_List_List*_tmpB02;struct Cyc_List_List*_tmpB01;void*_tmpB00;union Cyc_Absyn_Constraint*_tmpAFF;union Cyc_Absyn_Constraint*_tmpAFE;switch(*((int*)_tmpAFA)){case 0: _LL965: _LL966:
 goto _LL968;case 6: _LL967: _LL968:
 goto _LL96A;case 7: _LL969: _LL96A:
 return 1;case 5: _LL96B: _tmpAFF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAFA)->f1).ptr_atts).nullable;_tmpAFE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAFA)->f1).ptr_atts).bounds;_LL96C: {
# 5625
void*_tmpAFB=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAFE);void*_tmpAFC=_tmpAFB;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAFC)->tag == 0){_LL97C: _LL97D:
 return 1;}else{_LL97E: _LL97F:
# 5628
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpAFF);}_LL97B:;}case 8: _LL96D: _tmpB00=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAFA)->f1).elt_type;_LL96E:
# 5631
 return Cyc_Tcutil_supports_default(_tmpB00);case 10: _LL96F: _tmpB01=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAFA)->f1;_LL970:
# 5633
 for(0;_tmpB01 != 0;_tmpB01=_tmpB01->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpB01->hd)).f2))return 0;}
return 1;case 11: _LL971: _tmpB03=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAFA)->f1).aggr_info;_tmpB02=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAFA)->f1).targs;_LL972: {
# 5637
struct Cyc_Absyn_Aggrdecl*_tmpAFD=Cyc_Absyn_get_known_aggrdecl(_tmpB03);
if(_tmpAFD->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAFD->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpAFD->tvs,_tmpB02,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAFD->impl))->fields);}case 12: _LL973: _tmpB04=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAFA)->f2;_LL974:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpB04);case 14: _LL975: _LL976:
# 5643
 goto _LL978;case 13: _LL977: _LL978:
 return 1;default: _LL979: _LL97A:
# 5646
 return 0;}_LL964:;}
# 5651
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpB05=t;struct Cyc_Absyn_Typedefdecl*_tmpB0A;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB05)->tag == 17){_LL981: _tmpB0A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpB05)->f3;_LL982:
# 5654
 if(_tmpB0A != 0){
struct Cyc_Absyn_Tqual _tmpB06=_tmpB0A->tq;
if(((_tmpB06.print_const  || _tmpB06.q_volatile) || _tmpB06.q_restrict) || _tmpB06.real_const)
# 5659
({struct Cyc_String_pa_PrintArg_struct _tmpB09;_tmpB09.tag=0;_tmpB09.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB07[1]={& _tmpB09};Cyc_Tcutil_warn(loc,({const char*_tmpB08="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpB08,sizeof(char),44);}),_tag_dyneither(_tmpB07,sizeof(void*),1));});});}
# 5662
goto _LL980;}else{_LL983: _LL984:
 goto _LL980;}_LL980:;}
# 5667
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5669
struct _RegionHandle _tmpB0B=_new_region("rgn");struct _RegionHandle*rgn=& _tmpB0B;_push_region(rgn);
{struct Cyc_List_List*_tmpB0C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpB0D=1;_npop_handler(0);return _tmpB0D;}
t=Cyc_Tcutil_rsubstitute(rgn,_tmpB0C,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_supports_default(t)){int _tmpB0E=0;_npop_handler(0);return _tmpB0E;}}{
# 5677
int _tmpB0F=1;_npop_handler(0);return _tmpB0F;};}
# 5670
;_pop_region(rgn);}
# 5683
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpB10=Cyc_Tcutil_compress(t);void*_tmpB11=_tmpB10;union Cyc_Absyn_Constraint*_tmpB15;union Cyc_Absyn_Constraint*_tmpB14;switch(*((int*)_tmpB11)){case 6: _LL986: _LL987:
 goto _LL989;case 7: _LL988: _LL989:
 return 1;case 5: _LL98A: _tmpB15=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB11)->f1).ptr_atts).nullable;_tmpB14=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB11)->f1).ptr_atts).bounds;_LL98B: {
# 5688
void*_tmpB12=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpB14);void*_tmpB13=_tmpB12;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpB13)->tag == 0){_LL98F: _LL990:
# 5692
 return 0;}else{_LL991: _LL992:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpB15);}_LL98E:;}default: _LL98C: _LL98D:
# 5695
 return 0;}_LL985:;}
# 5699
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpB16=Cyc_Tcutil_compress(t);void*_tmpB17=_tmpB16;struct Cyc_List_List*_tmpB1B;void*_tmpB1A;switch(*((int*)_tmpB17)){case 5: _LL994: _tmpB1A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB17)->f1).elt_typ;_LL995:
 return Cyc_Tcutil_is_noreturn(_tmpB1A);case 9: _LL996: _tmpB1B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB17)->f1).attributes;_LL997:
# 5703
 for(0;_tmpB1B != 0;_tmpB1B=_tmpB1B->tl){
void*_tmpB18=(void*)_tmpB1B->hd;void*_tmpB19=_tmpB18;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpB19)->tag == 4){_LL99B: _LL99C:
 return 1;}else{_LL99D: _LL99E:
 continue;}_LL99A:;}
# 5708
goto _LL993;default: _LL998: _LL999:
 goto _LL993;}_LL993:;}
# 5711
return 0;}
# 5716
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpB1C=Cyc_Tcutil_compress(t);void*_tmpB1D=_tmpB1C;struct Cyc_List_List**_tmpB23;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB1D)->tag == 9){_LL9A0: _tmpB23=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpB1D)->f1).attributes;_LL9A1: {
# 5719
struct Cyc_List_List*_tmpB1E=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpB23,(void*)atts->hd))
*_tmpB23=({struct Cyc_List_List*_tmpB1F=_cycalloc(sizeof(*_tmpB1F));_tmpB1F->hd=(void*)atts->hd;_tmpB1F->tl=*_tmpB23;_tmpB1F;});}else{
# 5726
_tmpB1E=({struct Cyc_List_List*_tmpB20=_cycalloc(sizeof(*_tmpB20));_tmpB20->hd=(void*)atts->hd;_tmpB20->tl=_tmpB1E;_tmpB20;});}}
return _tmpB1E;}}else{_LL9A2: _LL9A3:
({void*_tmpB21=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB22="transfer_fn_type_atts";_tag_dyneither(_tmpB22,sizeof(char),22);}),_tag_dyneither(_tmpB21,sizeof(void*),0));});}_LL99F:;}
# 5733
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpB24=Cyc_Tcutil_compress(t);void*_tmpB25=_tmpB24;struct Cyc_Absyn_Exp*_tmpB2A;struct Cyc_Absyn_PtrInfo*_tmpB29;switch(*((int*)_tmpB25)){case 5: _LL9A5: _tmpB29=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB25)->f1;_LL9A6:
# 5737
{void*_tmpB26=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpB29->ptr_atts).bounds);void*_tmpB27=_tmpB26;struct Cyc_Absyn_Exp*_tmpB28;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB27)->tag == 1){_LL9AC: _tmpB28=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpB27)->f1;_LL9AD:
 bound=_tmpB28;goto _LL9AB;}else{_LL9AE: _LL9AF:
 goto _LL9AB;}_LL9AB:;}
# 5741
goto _LL9A4;case 8: _LL9A7: _tmpB2A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB25)->f1).num_elts;_LL9A8:
# 5743
 bound=_tmpB2A;
goto _LL9A4;default: _LL9A9: _LL9AA:
 goto _LL9A4;}_LL9A4:;}
# 5747
return bound;}
# 5752
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpB2B=b;struct Cyc_Absyn_Vardecl*_tmpB2F;struct Cyc_Absyn_Vardecl*_tmpB2E;struct Cyc_Absyn_Vardecl*_tmpB2D;struct Cyc_Absyn_Vardecl*_tmpB2C;switch(*((int*)_tmpB2B)){case 5: _LL9B1: _tmpB2C=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpB2B)->f1;_LL9B2:
 _tmpB2D=_tmpB2C;goto _LL9B4;case 4: _LL9B3: _tmpB2D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpB2B)->f1;_LL9B4:
 _tmpB2E=_tmpB2D;goto _LL9B6;case 3: _LL9B5: _tmpB2E=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpB2B)->f1;_LL9B6:
 _tmpB2F=_tmpB2E;goto _LL9B8;case 1: _LL9B7: _tmpB2F=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpB2B)->f1;_LL9B8:
# 5758
 if(!_tmpB2F->escapes)return _tmpB2F;
goto _LL9B0;default: _LL9B9: _LL9BA:
 goto _LL9B0;}_LL9B0:;}
# 5762
return 0;}
# 5766
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpB30=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmpB30=({struct Cyc_List_List*_tmpB31=_cycalloc(sizeof(*_tmpB31));_tmpB31->hd=*((void**)_check_null((void**)x->hd));_tmpB31->tl=_tmpB30;_tmpB31;});}}
return _tmpB30;}
# 5773
int Cyc_Tcutil_is_array(void*t){
# 5775
void*_tmpB32=Cyc_Tcutil_compress(t);void*_tmpB33=_tmpB32;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB33)->tag == 8){_LL9BC: _LL9BD:
# 5777
 return 1;}else{_LL9BE: _LL9BF:
 return 0;}_LL9BB:;}
# 5782
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
# 5784
void*_tmpB34=Cyc_Tcutil_compress(t);void*_tmpB35=_tmpB34;void*_tmpB45;struct Cyc_Absyn_Tqual _tmpB44;struct Cyc_Absyn_Exp*_tmpB43;union Cyc_Absyn_Constraint*_tmpB42;unsigned int _tmpB41;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB35)->tag == 8){_LL9C1: _tmpB45=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB35)->f1).elt_type;_tmpB44=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB35)->f1).tq;_tmpB43=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB35)->f1).num_elts;_tmpB42=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB35)->f1).zero_term;_tmpB41=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB35)->f1).zt_loc;_LL9C2: {
# 5786
void*b;
if(_tmpB43 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5790
struct Cyc_Absyn_Exp*bnd=_tmpB43;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpB36=0;((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB37="cannot convert tag without type!";_tag_dyneither(_tmpB37,sizeof(char),33);}),_tag_dyneither(_tmpB36,sizeof(void*),0));});{
void*_tmpB38=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpB39=_tmpB38;void*_tmpB3E;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB39)->tag == 19){_LL9C6: _tmpB3E=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpB39)->f1;_LL9C7:
# 5796
 b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB3A=_cycalloc(sizeof(*_tmpB3A));_tmpB3A[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB3B;_tmpB3B.tag=1;_tmpB3B.f1=Cyc_Absyn_cast_exp(Cyc_Absyn_uint_typ,Cyc_Absyn_valueof_exp(_tmpB3E,0),0,Cyc_Absyn_No_coercion,0);_tmpB3B;});_tmpB3A;});
goto _LL9C5;}else{_LL9C8: _LL9C9:
# 5799
 if(Cyc_Tcutil_is_const_exp(bnd))
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB3C=_cycalloc(sizeof(*_tmpB3C));_tmpB3C[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB3D;_tmpB3D.tag=1;_tmpB3D.f1=bnd;_tmpB3D;});_tmpB3C;});else{
# 5802
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL9C5:;};}else{
# 5806
b=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpB3F=_cycalloc(sizeof(*_tmpB3F));_tmpB3F[0]=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpB40;_tmpB40.tag=1;_tmpB40.f1=bnd;_tmpB40;});_tmpB3F;});}}
# 5808
return Cyc_Absyn_atb_typ(_tmpB45,rgn,_tmpB44,b,_tmpB42);}}else{_LL9C3: _LL9C4:
# 5811
 return t;}_LL9C0:;}
# 5816
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpB46=Cyc_Tcutil_compress(t);void*_tmpB47=_tmpB46;struct Cyc_List_List*_tmpB55;union Cyc_Absyn_AggrInfoU _tmpB54;struct Cyc_List_List*_tmpB53;struct Cyc_List_List*_tmpB52;void*_tmpB51;union Cyc_Absyn_Constraint*_tmpB50;switch(*((int*)_tmpB47)){case 0: _LL9CB: _LL9CC:
 return 1;case 1: _LL9CD: _LL9CE:
 goto _LL9D0;case 2: _LL9CF: _LL9D0:
 goto _LL9D2;case 3: _LL9D1: _LL9D2:
 goto _LL9D4;case 4: _LL9D3: _LL9D4:
 return 0;case 5: _LL9D5: _tmpB50=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpB47)->f1).ptr_atts).nullable;_LL9D6:
# 5824
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpB50);case 6: _LL9D7: _LL9D8:
 goto _LL9DA;case 7: _LL9D9: _LL9DA:
 return 1;case 8: _LL9DB: _tmpB51=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpB47)->f1).elt_type;_LL9DC:
 return Cyc_Tcutil_zeroable_type(_tmpB51);case 9: _LL9DD: _LL9DE:
 return 0;case 10: _LL9DF: _tmpB52=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpB47)->f1;_LL9E0:
# 5830
 for(0;(unsigned int)_tmpB52;_tmpB52=_tmpB52->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpB52->hd)).f2))return 0;}
return 1;case 11: _LL9E1: _tmpB54=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB47)->f1).aggr_info;_tmpB53=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpB47)->f1).targs;_LL9E2: {
# 5834
struct Cyc_Absyn_Aggrdecl*_tmpB48=Cyc_Absyn_get_known_aggrdecl(_tmpB54);
if(_tmpB48->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB48->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpB49=_new_region("r");struct _RegionHandle*r=& _tmpB49;_push_region(r);
{struct Cyc_List_List*_tmpB4A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpB48->tvs,_tmpB53);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpB48->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpB4A,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpB4B=0;_npop_handler(0);return _tmpB4B;}}}{
int _tmpB4C=1;_npop_handler(0);return _tmpB4C;};}
# 5838
;_pop_region(r);};}case 13: _LL9E3: _LL9E4:
# 5842
 return 1;case 19: _LL9E5: _LL9E6:
 return 1;case 12: _LL9E7: _tmpB55=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpB47)->f2;_LL9E8:
# 5845
 for(0;_tmpB55 != 0;_tmpB55=_tmpB55->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpB55->hd)->type))return 0;}
return 1;case 14: _LL9E9: _LL9EA:
 return 1;case 17: _LL9EB: _LL9EC:
 return 0;case 16: _LL9ED: _LL9EE:
 return 0;case 15: _LL9EF: _LL9F0:
 return 0;case 28: _LL9F1: _LL9F2:
 return 0;case 26: _LL9F3: _LL9F4:
# 5854
 goto _LL9F6;case 27: _LL9F5: _LL9F6:
 goto _LL9F8;case 18: _LL9F7: _LL9F8:
 goto _LL9FA;case 20: _LL9F9: _LL9FA:
 goto _LL9FC;case 21: _LL9FB: _LL9FC:
 goto _LL9FE;case 22: _LL9FD: _LL9FE:
 goto _LLA00;case 23: _LL9FF: _LLA00:
 goto _LLA02;case 24: _LLA01: _LLA02:
 goto _LLA04;default: _LLA03: _LLA04:
({struct Cyc_String_pa_PrintArg_struct _tmpB4F;_tmpB4F.tag=0;_tmpB4F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));({void*_tmpB4D[1]={& _tmpB4F};((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(({const char*_tmpB4E="bad type `%s' in zeroable type";_tag_dyneither(_tmpB4E,sizeof(char),31);}),_tag_dyneither(_tmpB4D,sizeof(void*),1));});});}_LL9CA:;}
