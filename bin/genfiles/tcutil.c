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
if(({struct _dyneither_ptr _tmpAFC=({const char*_tmp1="(qualifiers don't match)";_tag_dyneither(_tmp1,sizeof(char),25U);});Cyc_strcmp(_tmpAFC,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp4;_tmp4.tag=0U;_tmp4.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp2[1U]={& _tmp4};({struct Cyc___cycFILE*_tmpAFE=Cyc_stderr;struct _dyneither_ptr _tmpAFD=({const char*_tmp3="  %s\n";_tag_dyneither(_tmp3,sizeof(char),6U);});Cyc_fprintf(_tmpAFE,_tmpAFD,_tag_dyneither(_tmp2,sizeof(void*),1U));});});});
return;}
# 68
if(({struct _dyneither_ptr _tmpAFF=({const char*_tmp5="(function effects do not match)";_tag_dyneither(_tmp5,sizeof(char),32U);});Cyc_strcmp(_tmpAFF,(struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp6=Cyc_Absynpp_tc_params_r;
_tmp6.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp6);}{
# 73
struct _dyneither_ptr s1=Cyc_Absynpp_typ2string(Cyc_Tcutil_t1_failure);
struct _dyneither_ptr s2=Cyc_Absynpp_typ2string(Cyc_Tcutil_t2_failure);
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp9;_tmp9.tag=0U;_tmp9.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s1);({void*_tmp7[1U]={& _tmp9};({struct Cyc___cycFILE*_tmpB01=Cyc_stderr;struct _dyneither_ptr _tmpB00=({const char*_tmp8="  %s";_tag_dyneither(_tmp8,sizeof(char),5U);});Cyc_fprintf(_tmpB01,_tmpB00,_tag_dyneither(_tmp7,sizeof(void*),1U));});});});
pos +=_get_dyneither_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmpA=0U;({struct Cyc___cycFILE*_tmpB03=Cyc_stderr;struct _dyneither_ptr _tmpB02=({const char*_tmpB="\n\t";_tag_dyneither(_tmpB,sizeof(char),3U);});Cyc_fprintf(_tmpB03,_tmpB02,_tag_dyneither(_tmpA,sizeof(void*),0U));});});
pos=8;}else{
# 82
({void*_tmpC=0U;({struct Cyc___cycFILE*_tmpB05=Cyc_stderr;struct _dyneither_ptr _tmpB04=({const char*_tmpD=" ";_tag_dyneither(_tmpD,sizeof(char),2U);});Cyc_fprintf(_tmpB05,_tmpB04,_tag_dyneither(_tmpC,sizeof(void*),0U));});});
++ pos;}
# 85
({void*_tmpE=0U;({struct Cyc___cycFILE*_tmpB07=Cyc_stderr;struct _dyneither_ptr _tmpB06=({const char*_tmpF="and ";_tag_dyneither(_tmpF,sizeof(char),5U);});Cyc_fprintf(_tmpB07,_tmpB06,_tag_dyneither(_tmpE,sizeof(void*),0U));});});
pos +=4;
if(pos + _get_dyneither_size(s2,sizeof(char))>= 80){
({void*_tmp10=0U;({struct Cyc___cycFILE*_tmpB09=Cyc_stderr;struct _dyneither_ptr _tmpB08=({const char*_tmp11="\n\t";_tag_dyneither(_tmp11,sizeof(char),3U);});Cyc_fprintf(_tmpB09,_tmpB08,_tag_dyneither(_tmp10,sizeof(void*),0U));});});
pos=8;}
# 91
({struct Cyc_String_pa_PrintArg_struct _tmp14;_tmp14.tag=0U;_tmp14.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)s2);({void*_tmp12[1U]={& _tmp14};({struct Cyc___cycFILE*_tmpB0B=Cyc_stderr;struct _dyneither_ptr _tmpB0A=({const char*_tmp13="%s ";_tag_dyneither(_tmp13,sizeof(char),4U);});Cyc_fprintf(_tmpB0B,_tmpB0A,_tag_dyneither(_tmp12,sizeof(void*),1U));});});});
pos +=_get_dyneither_size(s2,sizeof(char))+ 1;
if(pos + 17 >= 80){
({void*_tmp15=0U;({struct Cyc___cycFILE*_tmpB0D=Cyc_stderr;struct _dyneither_ptr _tmpB0C=({const char*_tmp16="\n\t";_tag_dyneither(_tmp16,sizeof(char),3U);});Cyc_fprintf(_tmpB0D,_tmpB0C,_tag_dyneither(_tmp15,sizeof(void*),0U));});});
pos=8;}
# 97
({void*_tmp17=0U;({struct Cyc___cycFILE*_tmpB0F=Cyc_stderr;struct _dyneither_ptr _tmpB0E=({const char*_tmp18="are not compatible. ";_tag_dyneither(_tmp18,sizeof(char),21U);});Cyc_fprintf(_tmpB0F,_tmpB0E,_tag_dyneither(_tmp17,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmpB10=(char*)Cyc_Tcutil_failure_reason.curr;_tmpB10 != (char*)(_tag_dyneither(0,0,0)).curr;})){
if(({int _tmpB11=pos;_tmpB11 + Cyc_strlen((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);})>= 80)
({void*_tmp19=0U;({struct Cyc___cycFILE*_tmpB13=Cyc_stderr;struct _dyneither_ptr _tmpB12=({const char*_tmp1A="\n\t";_tag_dyneither(_tmp1A,sizeof(char),3U);});Cyc_fprintf(_tmpB13,_tmpB12,_tag_dyneither(_tmp19,sizeof(void*),0U));});});
# 103
({struct Cyc_String_pa_PrintArg_struct _tmp1D;_tmp1D.tag=0U;_tmp1D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Tcutil_failure_reason);({void*_tmp1B[1U]={& _tmp1D};({struct Cyc___cycFILE*_tmpB15=Cyc_stderr;struct _dyneither_ptr _tmpB14=({const char*_tmp1C="%s";_tag_dyneither(_tmp1C,sizeof(char),3U);});Cyc_fprintf(_tmpB15,_tmpB14,_tag_dyneither(_tmp1B,sizeof(void*),1U));});});});}
# 105
({void*_tmp1E=0U;({struct Cyc___cycFILE*_tmpB17=Cyc_stderr;struct _dyneither_ptr _tmpB16=({const char*_tmp1F="\n";_tag_dyneither(_tmp1F,sizeof(char),2U);});Cyc_fprintf(_tmpB17,_tmpB16,_tag_dyneither(_tmp1E,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 109
void Cyc_Tcutil_terr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 111
Cyc_Position_post_error(({unsigned int _tmpB18=loc;Cyc_Position_mk_err(_tmpB18,(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap));}));}
# 113
void*Cyc_Tcutil_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 116
struct _dyneither_ptr msg=(struct _dyneither_ptr)Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);
({struct Cyc_String_pa_PrintArg_struct _tmp22;_tmp22.tag=0U;_tmp22.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp20[1U]={& _tmp22};({struct Cyc___cycFILE*_tmpB1A=Cyc_stderr;struct _dyneither_ptr _tmpB19=({const char*_tmp21="Compiler Error (Tcutil::impos): %s\n";_tag_dyneither(_tmp21,sizeof(char),36U);});Cyc_fprintf(_tmpB1A,_tmpB19,_tag_dyneither(_tmp20,sizeof(void*),1U));});});});
Cyc_fflush(Cyc_stderr);
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp23=_cycalloc(sizeof(*_tmp23));({struct Cyc_Core_Impossible_exn_struct _tmpB1B=({struct Cyc_Core_Impossible_exn_struct _tmp24;_tmp24.tag=Cyc_Core_Impossible;_tmp24.f1=msg;_tmp24;});_tmp23[0]=_tmpB1B;});_tmp23;}));}
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
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp34=_cycalloc(sizeof(*_tmp34));({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmpB1C=({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct _tmp35;_tmp35.tag=1U;_tmp35.f1=0;_tmp35;});_tmp34[0]=_tmpB1C;});_tmp34;});case 2U: _LL3: _tmp38=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp33)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp36=_cycalloc(sizeof(*_tmp36));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpB1D=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp37;_tmp37.tag=2U;_tmp37.f1=0;_tmp37.f2=_tmp38;_tmp37;});_tmp36[0]=_tmpB1D;});_tmp36;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 185
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 187
return({struct Cyc_Absyn_Tvar*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->name=tv->name;_tmp39->identity=- 1;({void*_tmpB1E=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp39->kind=_tmpB1E;});_tmp39;});}
# 189
static struct _tuple10*Cyc_Tcutil_copy_arg(struct _tuple10*arg){
# 191
struct _tuple10*_tmp3A=arg;struct _dyneither_ptr*_tmp3E;struct Cyc_Absyn_Tqual _tmp3D;void*_tmp3C;_LL1: _tmp3E=_tmp3A->f1;_tmp3D=_tmp3A->f2;_tmp3C=_tmp3A->f3;_LL2:;
return({struct _tuple10*_tmp3B=_cycalloc(sizeof(*_tmp3B));_tmp3B->f1=_tmp3E;_tmp3B->f2=_tmp3D;({void*_tmpB1F=Cyc_Tcutil_copy_type(_tmp3C);_tmp3B->f3=_tmpB1F;});_tmp3B;});}
# 194
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
struct _tuple12*_tmp3F=arg;struct Cyc_Absyn_Tqual _tmp42;void*_tmp41;_LL1: _tmp42=_tmp3F->f1;_tmp41=_tmp3F->f2;_LL2:;
return({struct _tuple12*_tmp40=_cycalloc(sizeof(*_tmp40));_tmp40->f1=_tmp42;({void*_tmpB20=Cyc_Tcutil_copy_type(_tmp41);_tmp40->f2=_tmpB20;});_tmp40;});}
# 198
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*);
# 200
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp43=_cycalloc(sizeof(*_tmp43));_tmp43->name=f->name;_tmp43->tq=f->tq;({void*_tmpB21=Cyc_Tcutil_copy_type(f->type);_tmp43->type=_tmpB21;});_tmp43->width=f->width;_tmp43->attributes=f->attributes;({struct Cyc_Absyn_Exp*_tmpB22=
Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp43->requires_clause=_tmpB22;});_tmp43;});}
# 204
static struct _tuple0*Cyc_Tcutil_copy_rgncmp(struct _tuple0*x){
struct _tuple0*_tmp44=x;void*_tmp47;void*_tmp46;_LL1: _tmp47=_tmp44->f1;_tmp46=_tmp44->f2;_LL2:;
return({struct _tuple0*_tmp45=_cycalloc(sizeof(*_tmp45));({void*_tmpB23=Cyc_Tcutil_copy_type(_tmp47);_tmp45->f1=_tmpB23;});({void*_tmpB24=Cyc_Tcutil_copy_type(_tmp46);_tmp45->f2=_tmpB24;});_tmp45;});}
# 208
static struct Cyc_Absyn_Enumfield*Cyc_Tcutil_copy_enumfield(struct Cyc_Absyn_Enumfield*f){
return({struct Cyc_Absyn_Enumfield*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->name=f->name;_tmp48->tag=f->tag;_tmp48->loc=f->loc;_tmp48;});}
# 211
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp49=_cycalloc(sizeof(*_tmp49));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpB26=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4A;_tmp4A.tag=2U;({struct Cyc_Absyn_Tvar*_tmpB25=Cyc_Tcutil_copy_tvar(t);_tmp4A.f1=_tmpB25;});_tmp4A;});_tmp49[0]=_tmpB26;});_tmp49;});}
# 215
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_Absyn_Datatypedecl*_tmpD2;struct Cyc_Absyn_Enumdecl*_tmpD1;struct Cyc_Absyn_Aggrdecl*_tmpD0;void*_tmpCF;struct Cyc_List_List*_tmpCE;void*_tmpCD;struct _tuple2*_tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_Absyn_Typedefdecl*_tmpCA;void*_tmpC9;void*_tmpC8;void*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;void*_tmpC4;struct Cyc_List_List*_tmpC3;struct _tuple2*_tmpC2;struct Cyc_Absyn_Enumdecl*_tmpC1;enum Cyc_Absyn_AggrKind _tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Aggrdecl**_tmpBE;struct Cyc_List_List*_tmpBD;enum Cyc_Absyn_AggrKind _tmpBC;struct _tuple2*_tmpBB;struct Cyc_Core_Opt*_tmpBA;struct Cyc_List_List*_tmpB9;struct Cyc_List_List*_tmpB8;struct Cyc_List_List*_tmpB7;void*_tmpB6;struct Cyc_Absyn_Tqual _tmpB5;void*_tmpB4;struct Cyc_List_List*_tmpB3;int _tmpB2;struct Cyc_Absyn_VarargInfo*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Exp*_tmpAC;struct Cyc_List_List*_tmpAB;void*_tmpAA;struct Cyc_Absyn_Tqual _tmpA9;struct Cyc_Absyn_Exp*_tmpA8;union Cyc_Absyn_Constraint*_tmpA7;unsigned int _tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;union Cyc_Absyn_Constraint*_tmpA2;union Cyc_Absyn_Constraint*_tmpA1;union Cyc_Absyn_Constraint*_tmpA0;struct Cyc_Absyn_PtrLoc*_tmp9F;union Cyc_Absyn_DatatypeFieldInfoU _tmp9E;struct Cyc_List_List*_tmp9D;union Cyc_Absyn_DatatypeInfoU _tmp9C;struct Cyc_List_List*_tmp9B;struct Cyc_Absyn_Tvar*_tmp9A;switch(*((int*)_tmp4C)){case 0U: _LL1: _LL2:
 goto _LL4;case 1U: _LL3: _LL4:
 return t;case 2U: _LL5: _tmp9A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp4C)->f1;_LL6:
 return(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D=_cycalloc(sizeof(*_tmp4D));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpB28=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4E;_tmp4E.tag=2U;({struct Cyc_Absyn_Tvar*_tmpB27=Cyc_Tcutil_copy_tvar(_tmp9A);_tmp4E.f1=_tmpB27;});_tmp4E;});_tmp4D[0]=_tmpB28;});_tmp4D;});case 3U: _LL7: _tmp9C=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).datatype_info;_tmp9B=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL8:
# 221
 return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB2B=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp50;_tmp50.tag=3U;({struct Cyc_Absyn_DatatypeInfo _tmpB2A=({struct Cyc_Absyn_DatatypeInfo _tmp51;_tmp51.datatype_info=_tmp9C;({struct Cyc_List_List*_tmpB29=Cyc_Tcutil_copy_types(_tmp9B);_tmp51.targs=_tmpB29;});_tmp51;});_tmp50.f1=_tmpB2A;});_tmp50;});_tmp4F[0]=_tmpB2B;});_tmp4F;});case 4U: _LL9: _tmp9E=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).field_info;_tmp9D=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LLA:
# 223
 return(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp52=_cycalloc(sizeof(*_tmp52));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpB2E=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp53;_tmp53.tag=4U;({struct Cyc_Absyn_DatatypeFieldInfo _tmpB2D=({struct Cyc_Absyn_DatatypeFieldInfo _tmp54;_tmp54.field_info=_tmp9E;({struct Cyc_List_List*_tmpB2C=Cyc_Tcutil_copy_types(_tmp9D);_tmp54.targs=_tmpB2C;});_tmp54;});_tmp53.f1=_tmpB2D;});_tmp53;});_tmp52[0]=_tmpB2E;});_tmp52;});case 5U: _LLB: _tmpA5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_typ;_tmpA4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_tq;_tmpA3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).rgn;_tmpA2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).nullable;_tmpA1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).bounds;_tmpA0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).zero_term;_tmp9F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).ptr_atts).ptrloc;_LLC: {
# 225
void*_tmp55=Cyc_Tcutil_copy_type(_tmpA5);
void*_tmp56=Cyc_Tcutil_copy_type(_tmpA3);
union Cyc_Absyn_Constraint*_tmp57=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA2);
struct Cyc_Absyn_Tqual _tmp58=_tmpA4;
# 230
union Cyc_Absyn_Constraint*_tmp59=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA1);
union Cyc_Absyn_Constraint*_tmp5A=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA0);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp5B=_cycalloc(sizeof(*_tmp5B));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpB31=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp5C;_tmp5C.tag=5U;({struct Cyc_Absyn_PtrInfo _tmpB30=({struct Cyc_Absyn_PtrInfo _tmp5D;_tmp5D.elt_typ=_tmp55;_tmp5D.elt_tq=_tmp58;({struct Cyc_Absyn_PtrAtts _tmpB2F=({(_tmp5D.ptr_atts).rgn=_tmp56;(_tmp5D.ptr_atts).nullable=_tmp57;(_tmp5D.ptr_atts).bounds=_tmp59;(_tmp5D.ptr_atts).zero_term=_tmp5A;(_tmp5D.ptr_atts).ptrloc=_tmp9F;_tmp5D.ptr_atts;});_tmp5D.ptr_atts=_tmpB2F;});_tmp5D;});_tmp5C.f1=_tmpB30;});_tmp5C;});_tmp5B[0]=_tmpB31;});_tmp5B;});}case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 return t;case 8U: _LL11: _tmpAA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_tmpA9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).tq;_tmpA8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).num_elts;_tmpA7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zero_term;_tmpA6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4C)->f1).zt_loc;_LL12: {
# 236
struct Cyc_Absyn_Exp*eopt2=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpA8);
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpB35=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp5F;_tmp5F.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpB34=({struct Cyc_Absyn_ArrayInfo _tmp60;({void*_tmpB32=Cyc_Tcutil_copy_type(_tmpAA);_tmp60.elt_type=_tmpB32;});_tmp60.tq=_tmpA9;_tmp60.num_elts=eopt2;({union Cyc_Absyn_Constraint*_tmpB33=
((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*cptr))Cyc_Tcutil_copy_conref)(_tmpA7);_tmp60.zero_term=_tmpB33;});_tmp60.zt_loc=_tmpA6;_tmp60;});_tmp5F.f1=_tmpB34;});_tmp5F;});_tmp5E[0]=_tmpB35;});_tmp5E;});}case 9U: _LL13: _tmpB7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).tvars;_tmpB6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).effect;_tmpB5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_tqual;_tmpB4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ret_typ;_tmpB3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).args;_tmpB2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).c_varargs;_tmpB1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).cyc_varargs;_tmpB0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).rgn_po;_tmpAF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_tmpAE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_clause;_tmpAD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).requires_relns;_tmpAC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_clause;_tmpAB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).ensures_relns;_LL14: {
# 240
struct Cyc_List_List*_tmp61=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpB7);
void*effopt2=_tmpB6 == 0?0: Cyc_Tcutil_copy_type(_tmpB6);
void*_tmp62=Cyc_Tcutil_copy_type(_tmpB4);
struct Cyc_List_List*_tmp63=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpB3);
int _tmp64=_tmpB2;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpB1 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpB1;
({struct Cyc_Absyn_VarargInfo*_tmpB37=({struct Cyc_Absyn_VarargInfo*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->name=cv->name;_tmp65->tq=cv->tq;({void*_tmpB36=Cyc_Tcutil_copy_type(cv->type);_tmp65->type=_tmpB36;});_tmp65->inject=cv->inject;_tmp65;});cyc_varargs2=_tmpB37;});}{
# 251
struct Cyc_List_List*_tmp66=((struct Cyc_List_List*(*)(struct _tuple0*(*f)(struct _tuple0*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpB0);
struct Cyc_List_List*_tmp67=_tmpAF;
struct Cyc_Absyn_Exp*_tmp68=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAE);
struct Cyc_List_List*_tmp69=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAD);
struct Cyc_Absyn_Exp*_tmp6A=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpAC);
struct Cyc_List_List*_tmp6B=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpAB);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp6C=_cycalloc(sizeof(*_tmp6C));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpB39=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp6D;_tmp6D.tag=9U;({struct Cyc_Absyn_FnInfo _tmpB38=({struct Cyc_Absyn_FnInfo _tmp6E;_tmp6E.tvars=_tmp61;_tmp6E.effect=effopt2;_tmp6E.ret_tqual=_tmpB5;_tmp6E.ret_typ=_tmp62;_tmp6E.args=_tmp63;_tmp6E.c_varargs=_tmp64;_tmp6E.cyc_varargs=cyc_varargs2;_tmp6E.rgn_po=_tmp66;_tmp6E.attributes=_tmp67;_tmp6E.requires_clause=_tmp68;_tmp6E.requires_relns=_tmp69;_tmp6E.ensures_clause=_tmp6A;_tmp6E.ensures_relns=_tmp6B;_tmp6E;});_tmp6D.f1=_tmpB38;});_tmp6D;});_tmp6C[0]=_tmpB39;});_tmp6C;});};}case 10U: _LL15: _tmpB8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp4C)->f1;_LL16:
# 260
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6F=_cycalloc(sizeof(*_tmp6F));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpB3B=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp70;_tmp70.tag=10U;({struct Cyc_List_List*_tmpB3A=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpB8);_tmp70.f1=_tmpB3A;});_tmp70;});_tmp6F[0]=_tmpB3B;});_tmp6F;});case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).tag == 1){_LL17: _tmpBC=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f1;_tmpBB=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f2;_tmpBA=((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).UnknownAggr).val).f3;_tmpB9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL18:
# 262
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp71=_cycalloc(sizeof(*_tmp71));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB3F=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp72;_tmp72.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpB3E=({struct Cyc_Absyn_AggrInfo _tmp73;({union Cyc_Absyn_AggrInfoU _tmpB3C=Cyc_Absyn_UnknownAggr(_tmpBC,_tmpBB,_tmpBA);_tmp73.aggr_info=_tmpB3C;});({struct Cyc_List_List*_tmpB3D=Cyc_Tcutil_copy_types(_tmpB9);_tmp73.targs=_tmpB3D;});_tmp73;});_tmp72.f1=_tmpB3E;});_tmp72;});_tmp71[0]=_tmpB3F;});_tmp71;});}else{_LL19: _tmpBE=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).aggr_info).KnownAggr).val;_tmpBD=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C)->f1).targs;_LL1A:
# 264
 return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp74=_cycalloc(sizeof(*_tmp74));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB43=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp75;_tmp75.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpB42=({struct Cyc_Absyn_AggrInfo _tmp76;({union Cyc_Absyn_AggrInfoU _tmpB40=Cyc_Absyn_KnownAggr(_tmpBE);_tmp76.aggr_info=_tmpB40;});({struct Cyc_List_List*_tmpB41=Cyc_Tcutil_copy_types(_tmpBD);_tmp76.targs=_tmpB41;});_tmp76;});_tmp75.f1=_tmpB42;});_tmp75;});_tmp74[0]=_tmpB43;});_tmp74;});}case 12U: _LL1B: _tmpC0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f1;_tmpBF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp4C)->f2;_LL1C:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp77=_cycalloc(sizeof(*_tmp77));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpB45=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp78;_tmp78.tag=12U;_tmp78.f1=_tmpC0;({struct Cyc_List_List*_tmpB44=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpBF);_tmp78.f2=_tmpB44;});_tmp78;});_tmp77[0]=_tmpB45;});_tmp77;});case 13U: _LL1D: _tmpC2=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC1=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp4C)->f2;_LL1E:
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp79=_cycalloc(sizeof(*_tmp79));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB46=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp7A;_tmp7A.tag=13U;_tmp7A.f1=_tmpC2;_tmp7A.f2=_tmpC1;_tmp7A;});_tmp79[0]=_tmpB46;});_tmp79;});case 14U: _LL1F: _tmpC3=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp4C)->f1;_LL20:
 return(void*)({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmpB48=({struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct _tmp7C;_tmp7C.tag=14U;({struct Cyc_List_List*_tmpB47=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Enumfield*(*f)(struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_enumfield,_tmpC3);_tmp7C.f1=_tmpB47;});_tmp7C;});_tmp7B[0]=_tmpB48;});_tmp7B;});case 19U: _LL21: _tmpC4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp4C)->f1;_LL22:
 return(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpB4A=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp7E;_tmp7E.tag=19U;({void*_tmpB49=Cyc_Tcutil_copy_type(_tmpC4);_tmp7E.f1=_tmpB49;});_tmp7E;});_tmp7D[0]=_tmpB4A;});_tmp7D;});case 18U: _LL23: _tmpC5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4C)->f1;_LL24:
# 271
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpB4B=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp80;_tmp80.tag=18U;_tmp80.f1=_tmpC5;_tmp80;});_tmp7F[0]=_tmpB4B;});_tmp7F;});case 27U: _LL25: _tmpC6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp4C)->f1;_LL26:
# 274
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp81=_cycalloc(sizeof(*_tmp81));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpB4C=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp82;_tmp82.tag=27U;_tmp82.f1=_tmpC6;_tmp82;});_tmp81[0]=_tmpB4C;});_tmp81;});case 15U: _LL27: _tmpC7=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp4C)->f1;_LL28:
 return(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpB4E=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp84;_tmp84.tag=15U;({void*_tmpB4D=Cyc_Tcutil_copy_type(_tmpC7);_tmp84.f1=_tmpB4D;});_tmp84;});_tmp83[0]=_tmpB4E;});_tmp83;});case 16U: _LL29: _tmpC9=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f1;_tmpC8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp4C)->f2;_LL2A:
 return(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpB51=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp86;_tmp86.tag=16U;({void*_tmpB4F=Cyc_Tcutil_copy_type(_tmpC9);_tmp86.f1=_tmpB4F;});({void*_tmpB50=Cyc_Tcutil_copy_type(_tmpC8);_tmp86.f2=_tmpB50;});_tmp86;});_tmp85[0]=_tmpB51;});_tmp85;});case 17U: _LL2B: _tmpCC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f1;_tmpCB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f2;_tmpCA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f3;_LL2C:
# 278
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpB53=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp88;_tmp88.tag=17U;_tmp88.f1=_tmpCC;({struct Cyc_List_List*_tmpB52=Cyc_Tcutil_copy_types(_tmpCB);_tmp88.f2=_tmpB52;});_tmp88.f3=_tmpCA;_tmp88.f4=0;_tmp88;});_tmp87[0]=_tmpB53;});_tmp87;});case 21U: _LL2D: _LL2E:
 goto _LL30;case 22U: _LL2F: _LL30:
 goto _LL32;case 28U: _LL31: _LL32:
 goto _LL34;case 20U: _LL33: _LL34:
 return t;case 23U: _LL35: _tmpCD=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp4C)->f1;_LL36:
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp89=_cycalloc(sizeof(*_tmp89));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpB55=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp8A;_tmp8A.tag=23U;({void*_tmpB54=Cyc_Tcutil_copy_type(_tmpCD);_tmp8A.f1=_tmpB54;});_tmp8A;});_tmp89[0]=_tmpB55;});_tmp89;});case 24U: _LL37: _tmpCE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp4C)->f1;_LL38:
 return(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpB57=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp8C;_tmp8C.tag=24U;({struct Cyc_List_List*_tmpB56=Cyc_Tcutil_copy_types(_tmpCE);_tmp8C.f1=_tmpB56;});_tmp8C;});_tmp8B[0]=_tmpB57;});_tmp8B;});case 25U: _LL39: _tmpCF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp4C)->f1;_LL3A:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpB59=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp8E;_tmp8E.tag=25U;({void*_tmpB58=Cyc_Tcutil_copy_type(_tmpCF);_tmp8E.f1=_tmpB58;});_tmp8E;});_tmp8D[0]=_tmpB59;});_tmp8D;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)){case 0U: _LL3B: _tmpD0=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL3C: {
# 288
struct Cyc_List_List*_tmp8F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD0->tvs);
return(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp90=_cycalloc(sizeof(*_tmp90));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpB5C=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp91;_tmp91.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpB5B=({struct Cyc_Absyn_AggrInfo _tmp92;({union Cyc_Absyn_AggrInfoU _tmpB5A=Cyc_Absyn_UnknownAggr(_tmpD0->kind,_tmpD0->name,0);_tmp92.aggr_info=_tmpB5A;});_tmp92.targs=_tmp8F;_tmp92;});_tmp91.f1=_tmpB5B;});_tmp91;});_tmp90[0]=_tmpB5C;});_tmp90;});}case 1U: _LL3D: _tmpD1=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL3E:
# 291
 return(void*)({struct Cyc_Absyn_EnumType_Absyn_Type_struct*_tmp93=_cycalloc(sizeof(*_tmp93));({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmpB5D=({struct Cyc_Absyn_EnumType_Absyn_Type_struct _tmp94;_tmp94.tag=13U;_tmp94.f1=_tmpD1->name;_tmp94.f2=0;_tmp94;});_tmp93[0]=_tmpB5D;});_tmp93;});default: _LL3F: _tmpD2=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f1)->r)->f1;_LL40: {
# 293
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD2->tvs);
return(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpB60=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp97;_tmp97.tag=3U;({struct Cyc_Absyn_DatatypeInfo _tmpB5F=({struct Cyc_Absyn_DatatypeInfo _tmp98;({union Cyc_Absyn_DatatypeInfoU _tmpB5E=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp99;_tmp99.name=_tmpD2->name;_tmp99.is_extensible=0;_tmp99;}));_tmp98.datatype_info=_tmpB5E;});_tmp98.targs=_tmp95;_tmp98;});_tmp97.f1=_tmpB5F;});_tmp97;});_tmp96[0]=_tmpB60;});_tmp96;});}}}_LL0:;}
# 308 "tcutil.cyc"
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD3=d;struct _dyneither_ptr*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->tag == 0U){_LL1: _tmpD6=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD3)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD4=_cycalloc(sizeof(*_tmpD4));({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpB62=({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct _tmpD5;_tmpD5.tag=0U;({struct Cyc_Absyn_Exp*_tmpB61=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpD6);_tmpD5.f1=_tmpB61;});_tmpD5;});_tmpD4[0]=_tmpB62;});_tmpD4;});}else{_LL3: _tmpD7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD3)->f1;_LL4:
 return d;}_LL0:;}struct _tuple19{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 314
static struct _tuple19*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple19*e){
# 316
return({struct _tuple19*_tmpD8=_cycalloc(sizeof(*_tmpD8));({struct Cyc_List_List*_tmpB63=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpD8->f1=_tmpB63;});({struct Cyc_Absyn_Exp*_tmpB64=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpD8->f2=_tmpB64;});_tmpD8;});}
# 319
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpD9=preserve_types;
{void*_tmpDA=e->r;void*_tmpDB=_tmpDA;int _tmp168;struct _dyneither_ptr _tmp167;void*_tmp166;struct Cyc_Absyn_Exp*_tmp165;struct _dyneither_ptr*_tmp164;struct Cyc_Core_Opt*_tmp163;struct Cyc_List_List*_tmp162;struct Cyc_Absyn_Exp*_tmp161;struct Cyc_Absyn_Exp*_tmp160;int _tmp15F;struct Cyc_Absyn_Exp*_tmp15E;void**_tmp15D;struct Cyc_Absyn_Exp*_tmp15C;int _tmp15B;int _tmp15A;void*_tmp159;struct Cyc_Absyn_Enumfield*_tmp158;struct Cyc_Absyn_Enumdecl*_tmp157;struct Cyc_Absyn_Enumfield*_tmp156;struct Cyc_List_List*_tmp155;struct Cyc_Absyn_Datatypedecl*_tmp154;struct Cyc_Absyn_Datatypefield*_tmp153;void*_tmp152;struct Cyc_List_List*_tmp151;struct _tuple2*_tmp150;struct Cyc_List_List*_tmp14F;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_Aggrdecl*_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;void*_tmp14B;int _tmp14A;struct Cyc_Absyn_Vardecl*_tmp149;struct Cyc_Absyn_Exp*_tmp148;struct Cyc_Absyn_Exp*_tmp147;int _tmp146;struct Cyc_List_List*_tmp145;struct _dyneither_ptr*_tmp144;struct Cyc_Absyn_Tqual _tmp143;void*_tmp142;struct Cyc_List_List*_tmp141;struct Cyc_List_List*_tmp140;struct Cyc_Absyn_Exp*_tmp13F;struct Cyc_Absyn_Exp*_tmp13E;struct Cyc_Absyn_Exp*_tmp13D;struct _dyneither_ptr*_tmp13C;int _tmp13B;int _tmp13A;struct Cyc_Absyn_Exp*_tmp139;struct _dyneither_ptr*_tmp138;int _tmp137;int _tmp136;struct Cyc_Absyn_Exp*_tmp135;void*_tmp134;struct Cyc_List_List*_tmp133;struct Cyc_Absyn_Exp*_tmp132;void*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;void*_tmp12D;struct Cyc_Absyn_Exp*_tmp12C;int _tmp12B;enum Cyc_Absyn_Coercion _tmp12A;struct Cyc_Absyn_Exp*_tmp129;struct Cyc_List_List*_tmp128;struct Cyc_Absyn_Exp*_tmp127;struct Cyc_Absyn_Exp*_tmp126;int _tmp125;struct Cyc_Absyn_Exp*_tmp124;struct Cyc_List_List*_tmp123;struct Cyc_Absyn_VarargCallInfo*_tmp122;int _tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;enum Cyc_Absyn_Incrementor _tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Core_Opt*_tmp114;struct Cyc_Absyn_Exp*_tmp113;enum Cyc_Absyn_Primop _tmp112;struct Cyc_List_List*_tmp111;struct _dyneither_ptr _tmp110;void*_tmp10F;union Cyc_Absyn_Cnst _tmp10E;switch(*((int*)_tmpDB)){case 0U: _LL1: _tmp10E=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL2:
({struct Cyc_Absyn_Exp*_tmpB65=Cyc_Absyn_const_exp(_tmp10E,e->loc);new_e=_tmpB65;});goto _LL0;case 1U: _LL3: _tmp10F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmpB66=Cyc_Absyn_varb_exp(_tmp10F,e->loc);new_e=_tmpB66;});goto _LL0;case 2U: _LL5: _tmp110=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmpB67=Cyc_Absyn_pragma_exp(_tmp110,e->loc);new_e=_tmpB67;});goto _LL0;case 3U: _LL7: _tmp112=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp111=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL8:
({struct Cyc_Absyn_Exp*_tmpB6A=({enum Cyc_Absyn_Primop _tmpB69=_tmp112;struct Cyc_List_List*_tmpB68=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp111);Cyc_Absyn_primop_exp(_tmpB69,_tmpB68,e->loc);});new_e=_tmpB6A;});goto _LL0;case 4U: _LL9: _tmp115=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp114=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp113=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLA:
# 328
({struct Cyc_Absyn_Exp*_tmpB6E=({struct Cyc_Absyn_Exp*_tmpB6D=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp115);struct Cyc_Core_Opt*_tmpB6C=(unsigned int)_tmp114?({struct Cyc_Core_Opt*_tmpDC=_cycalloc_atomic(sizeof(*_tmpDC));_tmpDC->v=(void*)_tmp114->v;_tmpDC;}): 0;struct Cyc_Absyn_Exp*_tmpB6B=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp113);Cyc_Absyn_assignop_exp(_tmpB6D,_tmpB6C,_tmpB6B,e->loc);});new_e=_tmpB6E;});
goto _LL0;case 5U: _LLB: _tmp117=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp116=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LLC:
({struct Cyc_Absyn_Exp*_tmpB71=({struct Cyc_Absyn_Exp*_tmpB70=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp117);enum Cyc_Absyn_Incrementor _tmpB6F=_tmp116;Cyc_Absyn_increment_exp(_tmpB70,_tmpB6F,e->loc);});new_e=_tmpB71;});goto _LL0;case 6U: _LLD: _tmp11A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp118=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LLE:
# 332
({struct Cyc_Absyn_Exp*_tmpB75=({struct Cyc_Absyn_Exp*_tmpB74=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11A);struct Cyc_Absyn_Exp*_tmpB73=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp119);struct Cyc_Absyn_Exp*_tmpB72=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp118);Cyc_Absyn_conditional_exp(_tmpB74,_tmpB73,_tmpB72,e->loc);});new_e=_tmpB75;});goto _LL0;case 7U: _LLF: _tmp11C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL10:
({struct Cyc_Absyn_Exp*_tmpB78=({struct Cyc_Absyn_Exp*_tmpB77=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11C);struct Cyc_Absyn_Exp*_tmpB76=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11B);Cyc_Absyn_and_exp(_tmpB77,_tmpB76,e->loc);});new_e=_tmpB78;});goto _LL0;case 8U: _LL11: _tmp11E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL12:
({struct Cyc_Absyn_Exp*_tmpB7B=({struct Cyc_Absyn_Exp*_tmpB7A=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11E);struct Cyc_Absyn_Exp*_tmpB79=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11D);Cyc_Absyn_or_exp(_tmpB7A,_tmpB79,e->loc);});new_e=_tmpB7B;});goto _LL0;case 9U: _LL13: _tmp120=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp11F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL14:
({struct Cyc_Absyn_Exp*_tmpB7E=({struct Cyc_Absyn_Exp*_tmpB7D=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp120);struct Cyc_Absyn_Exp*_tmpB7C=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp11F);Cyc_Absyn_seq_exp(_tmpB7D,_tmpB7C,e->loc);});new_e=_tmpB7E;});goto _LL0;case 10U: _LL15: _tmp124=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp123=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp122=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp121=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL16:
# 337
{struct Cyc_Absyn_VarargCallInfo*_tmpDD=_tmp122;int _tmpEB;struct Cyc_List_List*_tmpEA;struct Cyc_Absyn_VarargInfo*_tmpE9;if(_tmpDD != 0){_LL54: _tmpEB=_tmpDD->num_varargs;_tmpEA=_tmpDD->injectors;_tmpE9=_tmpDD->vai;_LL55: {
# 339
struct Cyc_Absyn_VarargInfo*_tmpDE=_tmpE9;struct _dyneither_ptr*_tmpE6;struct Cyc_Absyn_Tqual _tmpE5;void*_tmpE4;int _tmpE3;_LL59: _tmpE6=_tmpDE->name;_tmpE5=_tmpDE->tq;_tmpE4=_tmpDE->type;_tmpE3=_tmpDE->inject;_LL5A:;
({struct Cyc_Absyn_Exp*_tmpB86=({void*_tmpB85=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpDF=_cycalloc(sizeof(*_tmpDF));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpB84=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE0;_tmpE0.tag=10U;({struct Cyc_Absyn_Exp*_tmpB7F=
Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp124);_tmpE0.f1=_tmpB7F;});({struct Cyc_List_List*_tmpB80=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp123);_tmpE0.f2=_tmpB80;});({struct Cyc_Absyn_VarargCallInfo*_tmpB83=({struct Cyc_Absyn_VarargCallInfo*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->num_varargs=_tmpEB;_tmpE1->injectors=_tmpEA;({struct Cyc_Absyn_VarargInfo*_tmpB82=({struct Cyc_Absyn_VarargInfo*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->name=_tmpE6;_tmpE2->tq=_tmpE5;({void*_tmpB81=
# 343
Cyc_Tcutil_copy_type(_tmpE4);_tmpE2->type=_tmpB81;});_tmpE2->inject=_tmpE3;_tmpE2;});_tmpE1->vai=_tmpB82;});_tmpE1;});_tmpE0.f3=_tmpB83;});_tmpE0.f4=_tmp121;_tmpE0;});_tmpDF[0]=_tmpB84;});_tmpDF;});
# 340
Cyc_Absyn_new_exp(_tmpB85,e->loc);});new_e=_tmpB86;});
# 345
goto _LL53;}}else{_LL56: _LL57:
# 347
({struct Cyc_Absyn_Exp*_tmpB8B=({void*_tmpB8A=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpB89=({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct _tmpE8;_tmpE8.tag=10U;({struct Cyc_Absyn_Exp*_tmpB87=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp124);_tmpE8.f1=_tmpB87;});({struct Cyc_List_List*_tmpB88=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp123);_tmpE8.f2=_tmpB88;});_tmpE8.f3=_tmp122;_tmpE8.f4=_tmp121;_tmpE8;});_tmpE7[0]=_tmpB89;});_tmpE7;});Cyc_Absyn_new_exp(_tmpB8A,e->loc);});new_e=_tmpB8B;});
goto _LL53;}_LL53:;}
# 350
goto _LL0;case 11U: _LL17: _tmp126=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp125=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL18:
# 352
({struct Cyc_Absyn_Exp*_tmpB8E=_tmp125?({struct Cyc_Absyn_Exp*_tmpB8D=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp126);Cyc_Absyn_rethrow_exp(_tmpB8D,e->loc);}):({struct Cyc_Absyn_Exp*_tmpB8C=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp126);Cyc_Absyn_throw_exp(_tmpB8C,e->loc);});new_e=_tmpB8E;});
goto _LL0;case 12U: _LL19: _tmp127=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL1A:
({struct Cyc_Absyn_Exp*_tmpB90=({struct Cyc_Absyn_Exp*_tmpB8F=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp127);Cyc_Absyn_noinstantiate_exp(_tmpB8F,e->loc);});new_e=_tmpB90;});
goto _LL0;case 13U: _LL1B: _tmp129=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp128=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL1C:
# 357
({struct Cyc_Absyn_Exp*_tmpB93=({struct Cyc_Absyn_Exp*_tmpB92=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp129);struct Cyc_List_List*_tmpB91=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp128);Cyc_Absyn_instantiate_exp(_tmpB92,_tmpB91,e->loc);});new_e=_tmpB93;});
goto _LL0;case 14U: _LL1D: _tmp12D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp12B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp12A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL1E:
# 360
({struct Cyc_Absyn_Exp*_tmpB98=({void*_tmpB97=Cyc_Tcutil_copy_type(_tmp12D);struct Cyc_Absyn_Exp*_tmpB96=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12C);int _tmpB95=_tmp12B;enum Cyc_Absyn_Coercion _tmpB94=_tmp12A;Cyc_Absyn_cast_exp(_tmpB97,_tmpB96,_tmpB95,_tmpB94,e->loc);});new_e=_tmpB98;});goto _LL0;case 15U: _LL1F: _tmp12E=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL20:
({struct Cyc_Absyn_Exp*_tmpB9A=({struct Cyc_Absyn_Exp*_tmpB99=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12E);Cyc_Absyn_address_exp(_tmpB99,e->loc);});new_e=_tmpB9A;});goto _LL0;case 16U: _LL21: _tmp130=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp12F=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL22: {
# 363
struct Cyc_Absyn_Exp*eo1=_tmp130;if(_tmp130 != 0)({struct Cyc_Absyn_Exp*_tmpB9B=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp130);eo1=_tmpB9B;});
({struct Cyc_Absyn_Exp*_tmpB9E=({struct Cyc_Absyn_Exp*_tmpB9D=eo1;struct Cyc_Absyn_Exp*_tmpB9C=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp12F);Cyc_Absyn_New_exp(_tmpB9D,_tmpB9C,e->loc);});new_e=_tmpB9E;});
goto _LL0;}case 17U: _LL23: _tmp131=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL24:
({struct Cyc_Absyn_Exp*_tmpBA0=({void*_tmpB9F=Cyc_Tcutil_copy_type(_tmp131);Cyc_Absyn_sizeoftyp_exp(_tmpB9F,e->loc);});new_e=_tmpBA0;});
goto _LL0;case 18U: _LL25: _tmp132=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL26:
({struct Cyc_Absyn_Exp*_tmpBA2=({struct Cyc_Absyn_Exp*_tmpBA1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp132);Cyc_Absyn_sizeofexp_exp(_tmpBA1,e->loc);});new_e=_tmpBA2;});goto _LL0;case 19U: _LL27: _tmp134=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp133=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL28:
# 370
({struct Cyc_Absyn_Exp*_tmpBA5=({void*_tmpBA4=Cyc_Tcutil_copy_type(_tmp134);struct Cyc_List_List*_tmpBA3=_tmp133;Cyc_Absyn_offsetof_exp(_tmpBA4,_tmpBA3,e->loc);});new_e=_tmpBA5;});goto _LL0;case 20U: _LL29: _tmp135=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL2A:
({struct Cyc_Absyn_Exp*_tmpBA7=({struct Cyc_Absyn_Exp*_tmpBA6=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp135);Cyc_Absyn_deref_exp(_tmpBA6,e->loc);});new_e=_tmpBA7;});goto _LL0;case 21U: _LL2B: _tmp139=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp138=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp137=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp136=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL2C:
# 373
({struct Cyc_Absyn_Exp*_tmpBAB=({void*_tmpBAA=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpBA9=({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct _tmpED;_tmpED.tag=21U;({struct Cyc_Absyn_Exp*_tmpBA8=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp139);_tmpED.f1=_tmpBA8;});_tmpED.f2=_tmp138;_tmpED.f3=_tmp137;_tmpED.f4=_tmp136;_tmpED;});_tmpEC[0]=_tmpBA9;});_tmpEC;});Cyc_Absyn_new_exp(_tmpBAA,e->loc);});new_e=_tmpBAB;});goto _LL0;case 22U: _LL2D: _tmp13D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp13B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp13A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL2E:
# 375
({struct Cyc_Absyn_Exp*_tmpBAF=({void*_tmpBAE=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpBAD=({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct _tmpEF;_tmpEF.tag=22U;({struct Cyc_Absyn_Exp*_tmpBAC=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13D);_tmpEF.f1=_tmpBAC;});_tmpEF.f2=_tmp13C;_tmpEF.f3=_tmp13B;_tmpEF.f4=_tmp13A;_tmpEF;});_tmpEE[0]=_tmpBAD;});_tmpEE;});Cyc_Absyn_new_exp(_tmpBAE,e->loc);});new_e=_tmpBAF;});goto _LL0;case 23U: _LL2F: _tmp13F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp13E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL30:
({struct Cyc_Absyn_Exp*_tmpBB2=({struct Cyc_Absyn_Exp*_tmpBB1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13F);struct Cyc_Absyn_Exp*_tmpBB0=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp13E);Cyc_Absyn_subscript_exp(_tmpBB1,_tmpBB0,e->loc);});new_e=_tmpBB2;});
goto _LL0;case 24U: _LL31: _tmp140=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL32:
({struct Cyc_Absyn_Exp*_tmpBB4=({struct Cyc_List_List*_tmpBB3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp140);Cyc_Absyn_tuple_exp(_tmpBB3,e->loc);});new_e=_tmpBB4;});goto _LL0;case 25U: _LL33: _tmp144=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f1;_tmp143=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f2;_tmp142=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1)->f3;_tmp141=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL34: {
# 380
struct _dyneither_ptr*vopt1=_tmp144;
if(_tmp144 != 0)vopt1=_tmp144;
({struct Cyc_Absyn_Exp*_tmpBBA=({void*_tmpBB9=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpBB8=({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct _tmpF1;_tmpF1.tag=25U;({struct _tuple10*_tmpBB6=({struct _tuple10*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->f1=vopt1;_tmpF2->f2=_tmp143;({void*_tmpBB5=Cyc_Tcutil_copy_type(_tmp142);_tmpF2->f3=_tmpBB5;});_tmpF2;});_tmpF1.f1=_tmpBB6;});({struct Cyc_List_List*_tmpBB7=
((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp141);_tmpF1.f2=_tmpBB7;});_tmpF1;});_tmpF0[0]=_tmpBB8;});_tmpF0;});
# 382
Cyc_Absyn_new_exp(_tmpBB9,e->loc);});new_e=_tmpBBA;});
# 384
goto _LL0;}case 26U: _LL35: _tmp145=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL36:
({struct Cyc_Absyn_Exp*_tmpBBE=({void*_tmpBBD=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpBBC=({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct _tmpF4;_tmpF4.tag=26U;({struct Cyc_List_List*_tmpBBB=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp145);_tmpF4.f1=_tmpBBB;});_tmpF4;});_tmpF3[0]=_tmpBBC;});_tmpF3;});Cyc_Absyn_new_exp(_tmpBBD,e->loc);});new_e=_tmpBBE;});
goto _LL0;case 27U: _LL37: _tmp149=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp148=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp147=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp146=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL38:
# 388
({struct Cyc_Absyn_Exp*_tmpBC3=({void*_tmpBC2=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpF5=_cycalloc(sizeof(*_tmpF5));({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpBC1=({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct _tmpF6;_tmpF6.tag=27U;_tmpF6.f1=_tmp149;({struct Cyc_Absyn_Exp*_tmpBBF=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp148);_tmpF6.f2=_tmpBBF;});({struct Cyc_Absyn_Exp*_tmpBC0=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp147);_tmpF6.f3=_tmpBC0;});_tmpF6.f4=_tmp146;_tmpF6;});_tmpF5[0]=_tmpBC1;});_tmpF5;});Cyc_Absyn_new_exp(_tmpBC2,e->loc);});new_e=_tmpBC3;});
goto _LL0;case 28U: _LL39: _tmp14C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14B=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp14A=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL3A:
# 391
({struct Cyc_Absyn_Exp*_tmpBC8=({void*_tmpBC7=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpBC6=({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct _tmpF8;_tmpF8.tag=28U;({struct Cyc_Absyn_Exp*_tmpBC4=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp14C);_tmpF8.f1=_tmpBC4;});({void*_tmpBC5=Cyc_Tcutil_copy_type(_tmp14B);_tmpF8.f2=_tmpBC5;});_tmpF8.f3=_tmp14A;_tmpF8;});_tmpF7[0]=_tmpBC6;});_tmpF7;});Cyc_Absyn_new_exp(_tmpBC7,_tmp14C->loc);});new_e=_tmpBC8;});
# 393
goto _LL0;case 29U: _LL3B: _tmp150=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp14F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp14E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_tmp14D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDB)->f4;_LL3C:
# 395
({struct Cyc_Absyn_Exp*_tmpBCD=({void*_tmpBCC=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpF9=_cycalloc(sizeof(*_tmpF9));({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpBCB=({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct _tmpFA;_tmpFA.tag=29U;_tmpFA.f1=_tmp150;({struct Cyc_List_List*_tmpBC9=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp14F);_tmpFA.f2=_tmpBC9;});({struct Cyc_List_List*_tmpBCA=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp14E);_tmpFA.f3=_tmpBCA;});_tmpFA.f4=_tmp14D;_tmpFA;});_tmpF9[0]=_tmpBCB;});_tmpF9;});Cyc_Absyn_new_exp(_tmpBCC,e->loc);});new_e=_tmpBCD;});
# 397
goto _LL0;case 30U: _LL3D: _tmp152=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp151=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL3E:
# 399
({struct Cyc_Absyn_Exp*_tmpBD2=({void*_tmpBD1=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpFB=_cycalloc(sizeof(*_tmpFB));({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpBD0=({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct _tmpFC;_tmpFC.tag=30U;({void*_tmpBCE=Cyc_Tcutil_copy_type(_tmp152);_tmpFC.f1=_tmpBCE;});({struct Cyc_List_List*_tmpBCF=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp151);_tmpFC.f2=_tmpBCF;});_tmpFC;});_tmpFB[0]=_tmpBD0;});_tmpFB;});Cyc_Absyn_new_exp(_tmpBD1,e->loc);});new_e=_tmpBD2;});
goto _LL0;case 31U: _LL3F: _tmp155=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp154=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_tmp153=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDB)->f3;_LL40:
# 402
({struct Cyc_Absyn_Exp*_tmpBD6=({void*_tmpBD5=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpFD=_cycalloc(sizeof(*_tmpFD));({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpBD4=({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct _tmpFE;_tmpFE.tag=31U;({struct Cyc_List_List*_tmpBD3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpD9,_tmp155);_tmpFE.f1=_tmpBD3;});_tmpFE.f2=_tmp154;_tmpFE.f3=_tmp153;_tmpFE;});_tmpFD[0]=_tmpBD4;});_tmpFD;});Cyc_Absyn_new_exp(_tmpBD5,e->loc);});new_e=_tmpBD6;});
goto _LL0;case 32U: _LL41: _tmp157=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp156=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL42:
 new_e=e;goto _LL0;case 33U: _LL43: _tmp159=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp158=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL44:
# 406
({struct Cyc_Absyn_Exp*_tmpBDA=({void*_tmpBD9=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpFF=_cycalloc(sizeof(*_tmpFF));({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmpBD8=({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct _tmp100;_tmp100.tag=33U;({void*_tmpBD7=Cyc_Tcutil_copy_type(_tmp159);_tmp100.f1=_tmpBD7;});_tmp100.f2=_tmp158;_tmp100;});_tmpFF[0]=_tmpBD8;});_tmpFF;});Cyc_Absyn_new_exp(_tmpBD9,e->loc);});new_e=_tmpBDA;});
goto _LL0;case 34U: _LL45: _tmp15F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).is_calloc;_tmp15E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).rgn;_tmp15D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).elt_type;_tmp15C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).num_elts;_tmp15B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).fat_result;_tmp15A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDB)->f1).inline_call;_LL46: {
# 409
struct Cyc_Absyn_Exp*_tmp101=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp15E;if(_tmp15E != 0)({struct Cyc_Absyn_Exp*_tmpBDB=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp15E);r1=_tmpBDB;});{
void**t1=_tmp15D;if(_tmp15D != 0)({void**_tmpBDD=({void**_tmp102=_cycalloc(sizeof(*_tmp102));({void*_tmpBDC=Cyc_Tcutil_copy_type(*_tmp15D);_tmp102[0]=_tmpBDC;});_tmp102;});t1=_tmpBDD;});
({void*_tmpBE0=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp103=_cycalloc(sizeof(*_tmp103));({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmpBDF=({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct _tmp104;_tmp104.tag=34U;({struct Cyc_Absyn_MallocInfo _tmpBDE=({struct Cyc_Absyn_MallocInfo _tmp105;_tmp105.is_calloc=_tmp15F;_tmp105.rgn=r1;_tmp105.elt_type=t1;_tmp105.num_elts=_tmp15C;_tmp105.fat_result=_tmp15B;_tmp105.inline_call=_tmp15A;_tmp105;});_tmp104.f1=_tmpBDE;});_tmp104;});_tmp103[0]=_tmpBDF;});_tmp103;});_tmp101->r=_tmpBE0;});
new_e=_tmp101;
goto _LL0;};}case 35U: _LL47: _tmp161=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp160=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL48:
({struct Cyc_Absyn_Exp*_tmpBE3=({struct Cyc_Absyn_Exp*_tmpBE2=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp161);struct Cyc_Absyn_Exp*_tmpBE1=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp160);Cyc_Absyn_swap_exp(_tmpBE2,_tmpBE1,e->loc);});new_e=_tmpBE3;});goto _LL0;case 36U: _LL49: _tmp163=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp162=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL4A: {
# 417
struct Cyc_Core_Opt*nopt1=_tmp163;
if(_tmp163 != 0)({struct Cyc_Core_Opt*_tmpBE4=({struct Cyc_Core_Opt*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->v=(struct _tuple2*)_tmp163->v;_tmp106;});nopt1=_tmpBE4;});
({struct Cyc_Absyn_Exp*_tmpBE8=({void*_tmpBE7=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp107=_cycalloc(sizeof(*_tmp107));({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmpBE6=({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct _tmp108;_tmp108.tag=36U;_tmp108.f1=nopt1;({struct Cyc_List_List*_tmpBE5=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(int,struct _tuple19*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpD9,_tmp162);_tmp108.f2=_tmpBE5;});_tmp108;});_tmp107[0]=_tmpBE6;});_tmp107;});Cyc_Absyn_new_exp(_tmpBE7,e->loc);});new_e=_tmpBE8;});
goto _LL0;}case 37U: _LL4B: _LL4C:
# 422
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp109=_cycalloc(sizeof(*_tmp109));({struct Cyc_Core_Failure_exn_struct _tmpBEA=({struct Cyc_Core_Failure_exn_struct _tmp10A;_tmp10A.tag=Cyc_Core_Failure;({struct _dyneither_ptr _tmpBE9=({const char*_tmp10B="deep_copy: statement expressions unsupported";_tag_dyneither(_tmp10B,sizeof(char),45U);});_tmp10A.f1=_tmpBE9;});_tmp10A;});_tmp109[0]=_tmpBEA;});_tmp109;}));case 38U: _LL4D: _tmp165=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp164=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL4E:
({struct Cyc_Absyn_Exp*_tmpBEE=({void*_tmpBED=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp10C=_cycalloc(sizeof(*_tmp10C));({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmpBEC=({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct _tmp10D;_tmp10D.tag=38U;({struct Cyc_Absyn_Exp*_tmpBEB=Cyc_Tcutil_deep_copy_exp(_tmpD9,_tmp165);_tmp10D.f1=_tmpBEB;});_tmp10D.f2=_tmp164;_tmp10D;});_tmp10C[0]=_tmpBEC;});_tmp10C;});Cyc_Absyn_new_exp(_tmpBED,e->loc);});new_e=_tmpBEE;});
goto _LL0;case 39U: _LL4F: _tmp166=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_LL50:
({struct Cyc_Absyn_Exp*_tmpBF0=({void*_tmpBEF=Cyc_Tcutil_copy_type(_tmp166);Cyc_Absyn_valueof_exp(_tmpBEF,e->loc);});new_e=_tmpBF0;});
goto _LL0;default: _LL51: _tmp168=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f1;_tmp167=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDB)->f2;_LL52:
({struct Cyc_Absyn_Exp*_tmpBF1=Cyc_Absyn_asm_exp(_tmp168,_tmp167,e->loc);new_e=_tmpBF1;});goto _LL0;}_LL0:;}
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
({void*_tmpBF3=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp177=_cycalloc(sizeof(*_tmp177));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpBF2=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp178;_tmp178.tag=2U;_tmp178.f1=0;_tmp178.f2=def;_tmp178;});_tmp177[0]=_tmpBF2;});_tmp177;});tv->kind=_tmpBF3;});
return def;}_LL0:;}
# 483
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple0 _tmp17B=({struct _tuple0 _tmp192;({void*_tmpBF4=Cyc_Absyn_compress_kb(kb1);_tmp192.f1=_tmpBF4;});({void*_tmpBF5=Cyc_Absyn_compress_kb(kb2);_tmp192.f2=_tmpBF5;});_tmp192;});struct _tuple0 _tmp17C=_tmp17B;struct Cyc_Core_Opt**_tmp191;void*_tmp190;void*_tmp18F;struct Cyc_Core_Opt**_tmp18E;struct Cyc_Core_Opt**_tmp18D;struct Cyc_Absyn_Kind*_tmp18C;struct Cyc_Core_Opt**_tmp18B;struct Cyc_Absyn_Kind*_tmp18A;struct Cyc_Core_Opt**_tmp189;struct Cyc_Absyn_Kind*_tmp188;struct Cyc_Absyn_Kind*_tmp187;struct Cyc_Absyn_Kind*_tmp186;struct Cyc_Core_Opt**_tmp185;struct Cyc_Absyn_Kind*_tmp184;struct Cyc_Absyn_Kind*_tmp183;struct Cyc_Absyn_Kind*_tmp182;switch(*((int*)_tmp17C.f1)){case 0U: switch(*((int*)_tmp17C.f2)){case 0U: _LL1: _tmp183=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp182=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_LL2:
 return _tmp183 == _tmp182;case 2U: _LL5: _tmp186=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp185=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_tmp184=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f2;_LL6:
# 492
 if(Cyc_Tcutil_kind_leq(_tmp186,_tmp184)){
({struct Cyc_Core_Opt*_tmpBF6=({struct Cyc_Core_Opt*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->v=kb1;_tmp17E;});*_tmp185=_tmpBF6;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp17C.f2)){case 0U: _LL3: _tmp189=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp188=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f2;_tmp187=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_LL4:
# 487
 if(Cyc_Tcutil_kind_leq(_tmp187,_tmp188)){
({struct Cyc_Core_Opt*_tmpBF7=({struct Cyc_Core_Opt*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->v=kb2;_tmp17D;});*_tmp189=_tmpBF7;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp18D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f1;_tmp18C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f1)->f2;_tmp18B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_tmp18A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f2;_LL8:
# 497
 if(Cyc_Tcutil_kind_leq(_tmp18C,_tmp18A)){
({struct Cyc_Core_Opt*_tmpBF8=({struct Cyc_Core_Opt*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->v=kb1;_tmp17F;});*_tmp18B=_tmpBF8;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp18A,_tmp18C)){
({struct Cyc_Core_Opt*_tmpBF9=({struct Cyc_Core_Opt*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->v=kb2;_tmp180;});*_tmp18D=_tmpBF9;});
return 1;}else{
return 0;}}default: _LLB: _tmp18F=_tmp17C.f1;_tmp18E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp17C.f2)->f1;_LLC:
# 506
({struct Cyc_Core_Opt*_tmpBFA=({struct Cyc_Core_Opt*_tmp181=_cycalloc(sizeof(*_tmp181));_tmp181->v=_tmp18F;_tmp181;});*_tmp18E=_tmpBFA;});
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
({struct Cyc_String_pa_PrintArg_struct _tmp199;_tmp199.tag=0U;({struct _dyneither_ptr _tmpBFB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp199.f1=_tmpBFB;});({void*_tmp197[1U]={& _tmp199};({struct _dyneither_ptr _tmpBFC=({const char*_tmp198="swap_kind: cannot update the kind of %s";_tag_dyneither(_tmp198,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpBFC,_tag_dyneither(_tmp197,sizeof(void*),1U));});});});}_LL0:;}
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
({struct Cyc_List_List*_tmpBFE=({struct Cyc_List_List*_tmp1A1=_region_malloc(temp,sizeof(*_tmp1A1));({struct _tuple16*_tmpBFD=({struct _tuple16*_tmp1A2=_region_malloc(temp,sizeof(*_tmp1A2));_tmp1A2->f1=_tmp19D;_tmp1A2->f2=_tmp19E;_tmp1A2;});_tmp1A1->hd=_tmpBFD;});_tmp1A1->tl=_tmp19C;_tmp1A1;});_tmp19C=_tmpBFE;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 543
if(_tmp19C != 0){
({void*_tmpC01=({struct _RegionHandle*_tmpC00=temp;struct Cyc_List_List*_tmpBFF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp19C);Cyc_Tcutil_rsubstitute(_tmpC00,_tmpBFF,field_typ);});field_typ=_tmpC01;});
({struct Cyc_Absyn_Kind*_tmpC02=Cyc_Tcutil_typ_kind(field_typ);k=_tmpC02;});}}
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
({void*_tmp1A5=0U;({struct _dyneither_ptr _tmpC03=({const char*_tmp1A6="typ_kind: Unresolved DatatypeFieldType";_tag_dyneither(_tmp1A6,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC03,_tag_dyneither(_tmp1A5,sizeof(void*),0U));});});}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1A4)->f1).aggr_info).UnknownAggr).tag == 1){_LL21: _LL22:
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
({struct Cyc_String_pa_PrintArg_struct _tmp1B4;_tmp1B4.tag=0U;({struct _dyneither_ptr _tmpC04=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp1B4.f1=_tmpC04;});({void*_tmp1B2[1U]={& _tmp1B4};({struct _dyneither_ptr _tmpC05=({const char*_tmp1B3="typ_kind: typedef found: %s";_tag_dyneither(_tmp1B3,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC05,_tag_dyneither(_tmp1B2,sizeof(void*),1U));});});});
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
({void*_tmpC06=Cyc_Tcutil_compress(t);t=_tmpC06;});{
void*_tmp1C9=t;struct Cyc_List_List*_tmp1EF;void*_tmp1EE;void*_tmp1ED;void*_tmp1EC;void*_tmp1EB;struct Cyc_List_List*_tmp1EA;struct Cyc_List_List*_tmp1E9;struct Cyc_List_List*_tmp1E8;struct Cyc_List_List*_tmp1E7;struct Cyc_List_List*_tmp1E6;struct Cyc_List_List*_tmp1E5;struct Cyc_List_List*_tmp1E4;void*_tmp1E3;struct Cyc_Absyn_Tqual _tmp1E2;void*_tmp1E1;struct Cyc_List_List*_tmp1E0;int _tmp1DF;struct Cyc_Absyn_VarargInfo*_tmp1DE;struct Cyc_List_List*_tmp1DD;struct Cyc_List_List*_tmp1DC;struct Cyc_Absyn_Exp*_tmp1DB;struct Cyc_List_List*_tmp1DA;struct Cyc_Absyn_Exp*_tmp1D9;struct Cyc_List_List*_tmp1D8;void*_tmp1D7;struct Cyc_Absyn_PtrInfo _tmp1D6;void*_tmp1D5;struct Cyc_Core_Opt**_tmp1D4;struct Cyc_Absyn_Tvar*_tmp1D3;switch(*((int*)_tmp1C9)){case 2U: _LL1: _tmp1D3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C9)->f1;_LL2:
# 666
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1D3)){
({struct _dyneither_ptr _tmpC07=({const char*_tmp1CA="(type variable would escape scope)";_tag_dyneither(_tmp1CA,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpC07;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 670
goto _LL0;case 1U: _LL3: _tmp1D5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C9)->f2;_tmp1D4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C9)->f4;_LL4:
# 672
 if(t == evar){
({struct _dyneither_ptr _tmpC08=({const char*_tmp1CB="(occurs check)";_tag_dyneither(_tmp1CB,sizeof(char),15U);});Cyc_Tcutil_failure_reason=_tmpC08;});
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
({struct Cyc_List_List*_tmpC09=({struct Cyc_List_List*_tmp1CD=_cycalloc(sizeof(*_tmp1CD));_tmp1CD->hd=(struct Cyc_Absyn_Tvar*)s->hd;_tmp1CD->tl=_tmp1CC;_tmp1CD;});_tmp1CC=_tmpC09;});}}
# 692
({struct Cyc_Core_Opt*_tmpC0A=({struct Cyc_Core_Opt*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->v=_tmp1CC;_tmp1CE;});*_tmp1D4=_tmpC0A;});}}}
# 695
goto _LL0;case 5U: _LL5: _tmp1D6=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C9)->f1;_LL6:
# 698
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D6.elt_typ);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1D6.ptr_atts).rgn);
goto _LL0;case 8U: _LL7: _tmp1D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1C9)->f1).elt_type;_LL8:
# 702
 Cyc_Tcutil_occurs(evar,r,env,_tmp1D7);goto _LL0;case 9U: _LL9: _tmp1E4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).tvars;_tmp1E3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).effect;_tmp1E2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ret_tqual;_tmp1E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ret_typ;_tmp1E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).args;_tmp1DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).c_varargs;_tmp1DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).cyc_varargs;_tmp1DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).rgn_po;_tmp1DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).attributes;_tmp1DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).requires_clause;_tmp1DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).requires_relns;_tmp1D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ensures_clause;_tmp1D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1C9)->f1).ensures_relns;_LLA:
# 705
({struct Cyc_List_List*_tmpC0B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1E4,env);env=_tmpC0B;});
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
({void*_tmp1F0=0U;({struct _dyneither_ptr _tmpC0C=({const char*_tmp1F1="tq1 real_const not set.";_tag_dyneither(_tmp1F1,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC0C,_tag_dyneither(_tmp1F0,sizeof(void*),0U));});});
if(tq2.print_const  && !tq2.real_const)
({void*_tmp1F2=0U;({struct _dyneither_ptr _tmpC0D=({const char*_tmp1F3="tq2 real_const not set.";_tag_dyneither(_tmp1F3,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC0D,_tag_dyneither(_tmp1F2,sizeof(void*),0U));});});
# 763
if((tq1.real_const != tq2.real_const  || tq1.q_volatile != tq2.q_volatile) || tq1.q_restrict != tq2.q_restrict){
# 766
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
({struct _dyneither_ptr _tmpC0E=({const char*_tmp1F4="(qualifiers don't match)";_tag_dyneither(_tmp1F4,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC0E;});
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
({union Cyc_Absyn_Constraint*_tmpC0F=Cyc_Absyn_compress_conref(x);x=_tmpC0F;});
({union Cyc_Absyn_Constraint*_tmpC10=Cyc_Absyn_compress_conref(y);y=_tmpC10;});
if(x == y)return;{
union Cyc_Absyn_Constraint*_tmp1F5=x;void*_tmp1FD;switch((((union Cyc_Absyn_Constraint*)_tmp1F5)->Eq_constr).tag){case 3U: _LL1: _LL2:
# 792
({union Cyc_Absyn_Constraint _tmpC11=({union Cyc_Absyn_Constraint _tmp1F6;(_tmp1F6.Forward_constr).val=y;(_tmp1F6.Forward_constr).tag=2;_tmp1F6;});*x=_tmpC11;});return;case 1U: _LL3: _tmp1FD=(void*)(_tmp1F5->Eq_constr).val;_LL4: {
# 794
union Cyc_Absyn_Constraint*_tmp1F7=y;void*_tmp1FA;switch((((union Cyc_Absyn_Constraint*)_tmp1F7)->Eq_constr).tag){case 3U: _LL8: _LL9:
*y=*x;return;case 1U: _LLA: _tmp1FA=(void*)(_tmp1F7->Eq_constr).val;_LLB:
# 797
 if(cmp(_tmp1FD,_tmp1FA)!= 0){
Cyc_Tcutil_failure_reason=reason;
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 801
return;default: _LLC: _LLD:
({void*_tmp1F8=0U;({struct _dyneither_ptr _tmpC12=({const char*_tmp1F9="unify_conref: forward after compress(2)";_tag_dyneither(_tmp1F9,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpC12,_tag_dyneither(_tmp1F8,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 804
({void*_tmp1FB=0U;({struct _dyneither_ptr _tmpC13=({const char*_tmp1FC="unify_conref: forward after compress";_tag_dyneither(_tmp1FC,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpC13,_tag_dyneither(_tmp1FB,sizeof(void*),0U));});});}_LL0:;};}
# 808
static int Cyc_Tcutil_unify_conrefs(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
struct _handler_cons _tmp1FE;_push_handler(& _tmp1FE);{int _tmp200=0;if(setjmp(_tmp1FE.handler))_tmp200=1;if(!_tmp200){
({int(*_tmpC16)(void*,void*)=cmp;union Cyc_Absyn_Constraint*_tmpC15=x;union Cyc_Absyn_Constraint*_tmpC14=y;Cyc_Tcutil_unify_it_conrefs(_tmpC16,_tmpC15,_tmpC14,_tag_dyneither(0,0,0));});{
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
({void*_tmpC18=_tmp215;Cyc_Tcutil_unify_it(_tmpC18,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp20D=_cycalloc(sizeof(*_tmp20D));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC17=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp20E;_tmp20E.tag=18U;_tmp20E.f1=e2;_tmp20E;});_tmp20D[0]=_tmpC17;});_tmp20D;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20A.f2)->tag == 39U){_LL7: _tmp216=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp20A.f2)->f1;_LL8: {
# 837
void*_tmp20F=Cyc_Tcutil_compress(_tmp216);void*_tmp210=_tmp20F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp210)->tag == 1U){_LL11: _LL12:
({void*_tmpC1A=_tmp216;Cyc_Tcutil_unify_it(_tmpC1A,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp211=_cycalloc(sizeof(*_tmp211));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC19=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp212;_tmp212.tag=18U;_tmp212.f1=e1;_tmp212;});_tmp211[0]=_tmpC19;});_tmp211;}));});return;}else{_LL13: _LL14:
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
 return({int _tmpC1B=Cyc_Tcutil_attribute_case_number(att1);Cyc_Core_intcmp(_tmpC1B,Cyc_Tcutil_attribute_case_number(att2));});}_LL0:;}
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
({void*_tmpC1E=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp239=_cycalloc(sizeof(*_tmp239));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpC1D=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp23A;_tmp23A.tag=18U;({struct Cyc_Absyn_Exp*_tmpC1C=Cyc_Absyn_uint_exp(0U,0U);_tmp23A.f1=_tmpC1C;});_tmp23A;});_tmp239[0]=_tmpC1D;});_tmp239;});t=_tmpC1E;});goto _LL0;default: _LL9: _LLA:
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
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp240=_cycalloc(sizeof(*_tmp240));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC1F=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp241;_tmp241.tag=23U;_tmp241.f1=t;_tmp241;});_tmp240[0]=_tmpC1F;});_tmp240;});case Cyc_Absyn_EffKind: _LL3E: _LL3F:
 return t;case Cyc_Absyn_IntKind: _LL40: _LL41:
 return Cyc_Absyn_empty_effect;default: _LL42: _LL43:
 return(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp242=_cycalloc(sizeof(*_tmp242));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC20=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp243;_tmp243.tag=25U;_tmp243.f1=t;_tmp243;});_tmp242[0]=_tmpC20;});_tmp242;});}_LL3B:;}case 15U: _LL15: _tmp25A=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp23D)->f1;_LL16:
# 964
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp244=_cycalloc(sizeof(*_tmp244));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC21=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp245;_tmp245.tag=23U;_tmp245.f1=_tmp25A;_tmp245;});_tmp244[0]=_tmpC21;});_tmp244;});case 16U: _LL17: _tmp25C=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23D)->f1;_tmp25B=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp23D)->f2;_LL18:
# 968
 return(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp246=_cycalloc(sizeof(*_tmp246));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC22=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp247;_tmp247.tag=23U;_tmp247.f1=_tmp25B;_tmp247;});_tmp246[0]=_tmpC22;});_tmp246;});case 3U: _LL19: _tmp25D=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp23D)->f1).targs;_LL1A: {
# 970
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp25D);
return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp248=_cycalloc(sizeof(*_tmp248));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC23=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp249;_tmp249.tag=24U;_tmp249.f1=ts;_tmp249;});_tmp248[0]=_tmpC23;});_tmp248;}));}case 5U: _LL1B: _tmp25F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23D)->f1).elt_typ;_tmp25E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp23D)->f1).ptr_atts).rgn;_LL1C:
# 974
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp24A=_cycalloc(sizeof(*_tmp24A));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC28=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp24B;_tmp24B.tag=24U;({struct Cyc_List_List*_tmpC27=({void*_tmp24C[2U];({void*_tmpC24=Cyc_Tcutil_rgns_of(_tmp25F);_tmp24C[1U]=_tmpC24;});({void*_tmpC26=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp24D=_cycalloc(sizeof(*_tmp24D));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC25=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp24E;_tmp24E.tag=23U;_tmp24E.f1=_tmp25E;_tmp24E;});_tmp24D[0]=_tmpC25;});_tmp24D;});_tmp24C[0U]=_tmpC26;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp24C,sizeof(void*),2U));});_tmp24B.f1=_tmpC27;});_tmp24B;});_tmp24A[0]=_tmpC28;});_tmp24A;}));case 8U: _LL1D: _tmp260=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp23D)->f1).elt_type;_LL1E:
# 976
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp260));case 10U: _LL1F: _tmp261=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp23D)->f1;_LL20: {
# 978
struct Cyc_List_List*_tmp24F=0;
for(0;_tmp261 != 0;_tmp261=_tmp261->tl){
({struct Cyc_List_List*_tmpC29=({struct Cyc_List_List*_tmp250=_cycalloc(sizeof(*_tmp250));_tmp250->hd=(*((struct _tuple12*)_tmp261->hd)).f2;_tmp250->tl=_tmp24F;_tmp250;});_tmp24F=_tmpC29;});}
_tmp262=_tmp24F;goto _LL22;}case 4U: _LL21: _tmp262=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp23D)->f1).targs;_LL22:
 _tmp263=_tmp262;goto _LL24;case 11U: _LL23: _tmp263=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp23D)->f1).targs;_LL24:
# 984
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp251=_cycalloc(sizeof(*_tmp251));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC2B=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp252;_tmp252.tag=24U;({struct Cyc_List_List*_tmpC2A=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp263);_tmp252.f1=_tmpC2A;});_tmp252;});_tmp251[0]=_tmpC2B;});_tmp251;}));case 12U: _LL25: _tmp264=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp23D)->f2;_LL26:
# 986
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp253=_cycalloc(sizeof(*_tmp253));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC2D=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp254;_tmp254.tag=24U;({struct Cyc_List_List*_tmpC2C=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp264);_tmp254.f1=_tmpC2C;});_tmp254;});_tmp253[0]=_tmpC2D;});_tmp253;}));case 19U: _LL27: _LL28:
 return Cyc_Absyn_empty_effect;case 9U: _LL29: _tmp26B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).tvars;_tmp26A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).effect;_tmp269=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).ret_tqual;_tmp268=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).ret_typ;_tmp267=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).args;_tmp266=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).cyc_varargs;_tmp265=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp23D)->f1).rgn_po;_LL2A: {
# 996
void*_tmp255=({struct Cyc_List_List*_tmpC2E=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp26B);Cyc_Tcutil_substitute(_tmpC2E,(void*)_check_null(_tmp26A));});
return Cyc_Tcutil_normalize_effect(_tmp255);}case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 return Cyc_Absyn_empty_effect;case 23U: _LL31: _LL32:
 goto _LL34;case 24U: _LL33: _LL34:
 return t;case 25U: _LL35: _tmp26C=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp23D)->f1;_LL36:
 return Cyc_Tcutil_rgns_of(_tmp26C);case 17U: _LL37: _tmp26D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp23D)->f2;_LL38:
# 1005
 return Cyc_Tcutil_normalize_effect((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp256=_cycalloc(sizeof(*_tmp256));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC30=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp257;_tmp257.tag=24U;({struct Cyc_List_List*_tmpC2F=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp26D);_tmp257.f1=_tmpC2F;});_tmp257;});_tmp256[0]=_tmpC30;});_tmp256;}));default: _LL39: _LL3A:
({void*_tmp258=0U;({struct _dyneither_ptr _tmpC31=({const char*_tmp259="typedecl in rgns_of";_tag_dyneither(_tmp259,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC31,_tag_dyneither(_tmp258,sizeof(void*),0U));});});}_LL0:;}
# 1013
void*Cyc_Tcutil_normalize_effect(void*e){
({void*_tmpC32=Cyc_Tcutil_compress(e);e=_tmpC32;});{
void*_tmp26E=e;void*_tmp27A;struct Cyc_List_List**_tmp279;switch(*((int*)_tmp26E)){case 24U: _LL1: _tmp279=(struct Cyc_List_List**)&((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp26E)->f1;_LL2: {
# 1017
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp279;for(0;effs != 0;effs=effs->tl){
void*_tmp26F=(void*)effs->hd;
({void*_tmpC33=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp26F));effs->hd=_tmpC33;});{
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
({struct Cyc_List_List*_tmpC34=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp275,effects);effects=_tmpC34;});
goto _LL12;case 23U: switch(*((int*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp273)->f1)){case 20U: _LL15: _LL16:
 goto _LL18;case 22U: _LL17: _LL18:
 goto _LL1A;case 21U: _LL19: _LL1A:
 goto _LL12;default: goto _LL1B;}default: _LL1B: _tmp276=_tmp273;_LL1C:
({struct Cyc_List_List*_tmpC35=({struct Cyc_List_List*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->hd=_tmp276;_tmp274->tl=effects;_tmp274;});effects=_tmpC35;});goto _LL12;}_LL12:;}}
# 1042
({struct Cyc_List_List*_tmpC36=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp279=_tmpC36;});
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
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27B=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp27C=_cycalloc(sizeof(*_tmp27C));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC39=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp27D;_tmp27D.tag=9U;({struct Cyc_Absyn_FnInfo _tmpC38=({struct Cyc_Absyn_FnInfo _tmp27E;_tmp27E.tvars=0;_tmp27E.effect=eff;({struct Cyc_Absyn_Tqual _tmpC37=
Cyc_Absyn_empty_tqual(0U);_tmp27E.ret_tqual=_tmpC37;});_tmp27E.ret_typ=(void*)& Cyc_Absyn_VoidType_val;_tmp27E.args=0;_tmp27E.c_varargs=0;_tmp27E.cyc_varargs=0;_tmp27E.rgn_po=0;_tmp27E.attributes=0;_tmp27E.requires_clause=0;_tmp27E.requires_relns=0;_tmp27E.ensures_clause=0;_tmp27E.ensures_relns=0;_tmp27E;});_tmp27D.f1=_tmpC38;});_tmp27D;});_tmp27C[0]=_tmpC39;});_tmp27C;});
# 1065
return({void*_tmpC3C=(void*)_tmp27B;struct Cyc_Absyn_Tqual _tmpC3B=Cyc_Absyn_empty_tqual(0U);void*_tmpC3A=Cyc_Absyn_bounds_one;Cyc_Absyn_atb_typ(_tmpC3C,(void*)& Cyc_Absyn_HeapRgn_val,_tmpC3B,_tmpC3A,Cyc_Absyn_false_conref);});}
# 1072
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
({void*_tmpC3D=Cyc_Tcutil_compress(r);r=_tmpC3D;});
if((r == (void*)& Cyc_Absyn_HeapRgn_val  || r == (void*)& Cyc_Absyn_UniqueRgn_val) || r == (void*)& Cyc_Absyn_RefCntRgn_val)
return 1;{
void*_tmp27F=Cyc_Tcutil_compress(e);void*_tmp280=_tmp27F;struct Cyc_Core_Opt*_tmp2A5;void**_tmp2A4;struct Cyc_Core_Opt*_tmp2A3;void*_tmp2A2;struct Cyc_List_List*_tmp2A1;void*_tmp2A0;switch(*((int*)_tmp280)){case 23U: _LL1: _tmp2A0=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp280)->f1;_LL2:
# 1079
 if(constrain)return Cyc_Tcutil_unify(r,_tmp2A0);
({void*_tmpC3E=Cyc_Tcutil_compress(_tmp2A0);_tmp2A0=_tmpC3E;});
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
void*_tmp28B=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp28C=_cycalloc(sizeof(*_tmp28C));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC42=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp28D;_tmp28D.tag=24U;({struct Cyc_List_List*_tmpC41=({void*_tmp28E[2U];({void*_tmpC40=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp28F=_cycalloc(sizeof(*_tmp28F));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC3F=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp290;_tmp290.tag=23U;_tmp290.f1=r;_tmp290;});_tmp28F[0]=_tmpC3F;});_tmp28F;});_tmp28E[1U]=_tmpC40;});_tmp28E[0U]=_tmp28A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp28E,sizeof(void*),2U));});_tmp28D.f1=_tmpC41;});_tmp28D;});_tmp28C[0]=_tmpC42;});_tmp28C;}));
*_tmp292=_tmp28B;
return 1;};}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{_LL13: _tmp295=_tmp287;_LL14:
# 1107
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp295);}_LL10:;}case 1U: _LL7: _tmp2A5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->f1;_tmp2A4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->f2;_tmp2A3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp280)->f4;_LL8:
# 1110
 if(_tmp2A5 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2A5->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp296=0U;({struct _dyneither_ptr _tmpC43=({const char*_tmp297="effect evar has wrong kind";_tag_dyneither(_tmp297,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC43,_tag_dyneither(_tmp296,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1115
void*_tmp298=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2A3);
# 1118
Cyc_Tcutil_occurs(_tmp298,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2A3))->v,r);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp299=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp29A=_cycalloc(sizeof(*_tmp29A));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC48=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp29B;_tmp29B.tag=24U;({struct Cyc_List_List*_tmpC47=({struct Cyc_List_List*_tmp29C=_cycalloc(sizeof(*_tmp29C));_tmp29C->hd=_tmp298;({struct Cyc_List_List*_tmpC46=({struct Cyc_List_List*_tmp29D=_cycalloc(sizeof(*_tmp29D));({void*_tmpC45=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp29E=_cycalloc(sizeof(*_tmp29E));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpC44=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp29F;_tmp29F.tag=23U;_tmp29F.f1=r;_tmp29F;});_tmp29E[0]=_tmpC44;});_tmp29E;});_tmp29D->hd=_tmpC45;});_tmp29D->tl=0;_tmp29D;});_tmp29C->tl=_tmpC46;});_tmp29C;});_tmp29B.f1=_tmpC47;});_tmp29B;});_tmp29A[0]=_tmpC48;});_tmp29A;});
*_tmp2A4=(void*)_tmp299;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1129
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
({void*_tmpC49=Cyc_Tcutil_compress(t);t=_tmpC49;});{
void*_tmp2A6=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp2A7=_tmp2A6;struct Cyc_Core_Opt*_tmp2BF;void**_tmp2BE;struct Cyc_Core_Opt*_tmp2BD;void*_tmp2BC;struct Cyc_List_List*_tmp2BB;switch(*((int*)_tmp2A7)){case 23U: _LL1: _LL2:
 return 0;case 24U: _LL3: _tmp2BB=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp2A7)->f1;_LL4:
# 1134
 for(0;_tmp2BB != 0;_tmp2BB=_tmp2BB->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp2BB->hd))
return 1;}
return 0;case 25U: _LL5: _tmp2BC=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A7)->f1;_LL6:
# 1139
({void*_tmpC4A=Cyc_Tcutil_compress(_tmp2BC);_tmp2BC=_tmpC4A;});
if(t == _tmp2BC)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp2BC);{
void*_tmp2A8=Cyc_Tcutil_rgns_of(t);void*_tmp2A9=_tmp2A8;void*_tmp2B0;void*_tmp2AF;if(((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A9)->tag == 25U){_LLC: _tmp2AF=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp2A9)->f1;_LLD: {
# 1144
struct _tuple0 _tmp2AA=({struct _tuple0 _tmp2AE;({void*_tmpC4B=Cyc_Tcutil_compress(_tmp2AF);_tmp2AE.f1=_tmpC4B;});_tmp2AE.f2=_tmp2BC;_tmp2AE;});struct _tuple0 _tmp2AB=_tmp2AA;struct Cyc_Absyn_Tvar*_tmp2AD;struct Cyc_Absyn_Tvar*_tmp2AC;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AB.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AB.f2)->tag == 2U){_LL11: _tmp2AD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AB.f1)->f1;_tmp2AC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2AB.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp2BC);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp2AF == _tmp2BC;}_LL10:;}}else{_LLE: _tmp2B0=_tmp2A9;_LLF:
# 1148
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp2B0);}_LLB:;};case 1U: _LL7: _tmp2BF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A7)->f1;_tmp2BE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A7)->f2;_tmp2BD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2A7)->f4;_LL8:
# 1151
 if(_tmp2BF == 0  || ((struct Cyc_Absyn_Kind*)_tmp2BF->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2B1=0U;({struct _dyneither_ptr _tmpC4C=({const char*_tmp2B2="effect evar has wrong kind";_tag_dyneither(_tmp2B2,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC4C,_tag_dyneither(_tmp2B1,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1156
void*_tmp2B3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2BD);
# 1159
Cyc_Tcutil_occurs(_tmp2B3,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2BD))->v,t);{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B4=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2B5=_cycalloc(sizeof(*_tmp2B5));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC51=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2B6;_tmp2B6.tag=24U;({struct Cyc_List_List*_tmpC50=({struct Cyc_List_List*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->hd=_tmp2B3;({struct Cyc_List_List*_tmpC4F=({struct Cyc_List_List*_tmp2B8=_cycalloc(sizeof(*_tmp2B8));({void*_tmpC4E=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp2B9=_cycalloc(sizeof(*_tmp2B9));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpC4D=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp2BA;_tmp2BA.tag=25U;_tmp2BA.f1=t;_tmp2BA;});_tmp2B9[0]=_tmpC4D;});_tmp2B9;});_tmp2B8->hd=_tmpC4E;});_tmp2B8->tl=0;_tmp2B8;});_tmp2B7->tl=_tmpC4F;});_tmp2B7;});_tmp2B6.f1=_tmpC50;});_tmp2B6;});_tmp2B5[0]=_tmpC51;});_tmp2B5;});
*_tmp2BE=(void*)_tmp2B4;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1170
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
({void*_tmpC52=Cyc_Tcutil_compress(e);e=_tmpC52;});{
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
void*_tmp2C6=Cyc_Tcutil_dummy_fntype((void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC56=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2C8;_tmp2C8.tag=24U;({struct Cyc_List_List*_tmpC55=({void*_tmp2C9[2U];({void*_tmpC54=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC53=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2CB;_tmp2CB.tag=2U;_tmp2CB.f1=v;_tmp2CB;});_tmp2CA[0]=_tmpC53;});_tmp2CA;});_tmp2C9[1U]=_tmpC54;});_tmp2C9[0U]=_tmp2C5;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2C9,sizeof(void*),2U));});_tmp2C8.f1=_tmpC55;});_tmp2C8;});_tmp2C7[0]=_tmpC56;});_tmp2C7;}));
*_tmp2CD=_tmp2C6;
return 1;};}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{_LLE: _tmp2D0=_tmp2C2;_LLF:
# 1196
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp2D0);}_LLB:;}case 1U: _LL7: _tmp2E0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C0)->f1;_tmp2DF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C0)->f2;_tmp2DE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2C0)->f4;_LL8:
# 1199
 if(_tmp2E0 == 0  || ((struct Cyc_Absyn_Kind*)_tmp2E0->v)->kind != Cyc_Absyn_EffKind)
({void*_tmp2D1=0U;({struct _dyneither_ptr _tmpC57=({const char*_tmp2D2="effect evar has wrong kind";_tag_dyneither(_tmp2D2,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC57,_tag_dyneither(_tmp2D1,sizeof(void*),0U));});});{
# 1203
void*_tmp2D3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp2DE);
# 1205
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2DE))->v,v))
return 0;{
struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D4=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpC5C=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp2D6;_tmp2D6.tag=24U;({struct Cyc_List_List*_tmpC5B=({struct Cyc_List_List*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->hd=_tmp2D3;({struct Cyc_List_List*_tmpC5A=({struct Cyc_List_List*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));({void*_tmpC59=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp2D9=_cycalloc(sizeof(*_tmp2D9));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC58=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp2DA;_tmp2DA.tag=2U;_tmp2DA.f1=v;_tmp2DA;});_tmp2D9[0]=_tmpC58;});_tmp2D9;});_tmp2D8->hd=_tmpC59;});_tmp2D8->tl=0;_tmp2D8;});_tmp2D7->tl=_tmpC5A;});_tmp2D7;});_tmp2D6.f1=_tmpC5B;});_tmp2D6;});_tmp2D5[0]=_tmpC5C;});_tmp2D5;});
*_tmp2DF=(void*)_tmp2D4;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1215
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
({void*_tmpC5D=Cyc_Tcutil_compress(e);e=_tmpC5D;});{
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
({struct Cyc_String_pa_PrintArg_struct _tmp2F0;_tmp2F0.tag=0U;({struct _dyneither_ptr _tmpC5E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e1));_tmp2F0.f1=_tmpC5E;});({void*_tmp2EE[1U]={& _tmp2F0};({struct _dyneither_ptr _tmpC5F=({const char*_tmp2EF="subset_effect: bad effect: %s";_tag_dyneither(_tmp2EF,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpC5F,_tag_dyneither(_tmp2EE,sizeof(void*),1U));});});});}_LL0:;}
# 1299 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
({void*_tmpC60=Cyc_Tcutil_normalize_effect(e1);e1=_tmpC60;});
({void*_tmpC61=Cyc_Tcutil_normalize_effect(e2);e2=_tmpC61;});
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
({struct _dyneither_ptr _tmpC62=_tag_dyneither(0,0,0);Cyc_Tcutil_failure_reason=_tmpC62;});
({void*_tmpC63=Cyc_Tcutil_compress(t1);t1=_tmpC63;});
({void*_tmpC64=Cyc_Tcutil_compress(t2);t2=_tmpC64;});
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
({struct _dyneither_ptr _tmpC65=({const char*_tmp306="(type variable would escape scope)";_tag_dyneither(_tmp306,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpC65;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1389
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp311->v,_tmp303)){
*_tmp30C=t1;return;}
# 1392
({struct _dyneither_ptr _tmpC66=({const char*_tmp307="(kinds are incompatible)";_tag_dyneither(_tmp307,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC66;});
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
({struct _dyneither_ptr _tmpC67=({const char*_tmp30E="(kinds are incompatible)";_tag_dyneither(_tmp30E,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC67;});
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
({struct _dyneither_ptr _tmpC68=({const char*_tmp318="(variable types are not the same)";_tag_dyneither(_tmp318,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpC68;});
goto _LL11;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 11U: switch(*((int*)_tmp313.f2)){case 11U: _LL18: _tmp380=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313.f1)->f1).aggr_info;_tmp37F=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313.f1)->f1).targs;_tmp37E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313.f2)->f1).aggr_info;_tmp37D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp313.f2)->f1).targs;_LL19: {
# 1437
struct _tuple13 _tmp319=Cyc_Absyn_aggr_kinded_name(_tmp37E);struct _tuple13 _tmp31A=_tmp319;enum Cyc_Absyn_AggrKind _tmp322;struct _tuple2*_tmp321;_LL4F: _tmp322=_tmp31A.f1;_tmp321=_tmp31A.f2;_LL50:;{
struct _tuple13 _tmp31B=Cyc_Absyn_aggr_kinded_name(_tmp380);struct _tuple13 _tmp31C=_tmp31B;enum Cyc_Absyn_AggrKind _tmp320;struct _tuple2*_tmp31F;_LL52: _tmp320=_tmp31C.f1;_tmp31F=_tmp31C.f2;_LL53:;
if(_tmp322 != _tmp320){({struct _dyneither_ptr _tmpC69=({const char*_tmp31D="(struct and union type)";_tag_dyneither(_tmp31D,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpC69;});goto _LL11;}
if(Cyc_Absyn_qvar_cmp(_tmp321,_tmp31F)!= 0){({struct _dyneither_ptr _tmpC6A=({const char*_tmp31E="(different type name)";_tag_dyneither(_tmp31E,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpC6A;});goto _LL11;}
Cyc_Tcutil_unify_list(_tmp37D,_tmp37F);
return;};}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 13U: switch(*((int*)_tmp313.f2)){case 13U: _LL1A: _tmp382=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp381=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL1B:
# 1446
 if(Cyc_Absyn_qvar_cmp(_tmp382,_tmp381)== 0)return;
({struct _dyneither_ptr _tmpC6B=({const char*_tmp323="(different enum types)";_tag_dyneither(_tmp323,sizeof(char),23U);});Cyc_Tcutil_failure_reason=_tmpC6B;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 14U: switch(*((int*)_tmp313.f2)){case 14U: _LL1C: _tmp384=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp383=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp313.f2)->f1;_LL1D: {
# 1451
int bad=0;
for(0;_tmp384 != 0  && _tmp383 != 0;(_tmp384=_tmp384->tl,_tmp383=_tmp383->tl)){
struct Cyc_Absyn_Enumfield*_tmp324=(struct Cyc_Absyn_Enumfield*)_tmp384->hd;
struct Cyc_Absyn_Enumfield*_tmp325=(struct Cyc_Absyn_Enumfield*)_tmp383->hd;
if(Cyc_Absyn_qvar_cmp(_tmp324->name,_tmp325->name)!= 0){
({struct _dyneither_ptr _tmpC6C=({const char*_tmp326="(different names for enum fields)";_tag_dyneither(_tmp326,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpC6C;});
bad=1;
break;}
# 1460
if(_tmp324->tag == _tmp325->tag)continue;
if(_tmp324->tag == 0  || _tmp325->tag == 0){
({struct _dyneither_ptr _tmpC6D=({const char*_tmp327="(different tag values for enum fields)";_tag_dyneither(_tmp327,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC6D;});
bad=1;
break;}
# 1466
if(!({struct Cyc_Absyn_Exp*_tmpC6E=(struct Cyc_Absyn_Exp*)_check_null(_tmp324->tag);Cyc_Evexp_same_const_exp(_tmpC6E,(struct Cyc_Absyn_Exp*)_check_null(_tmp325->tag));})){
({struct _dyneither_ptr _tmpC6F=({const char*_tmp328="(different tag values for enum fields)";_tag_dyneither(_tmp328,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC6F;});
bad=1;
break;}}
# 1472
if(bad)goto _LL11;
if(_tmp384 == 0  && _tmp383 == 0)return;
({struct _dyneither_ptr _tmpC70=({const char*_tmp329="(different number of fields for enums)";_tag_dyneither(_tmp329,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC70;});
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
({struct _dyneither_ptr _tmpC71=({const char*_tmp32A="(different datatype field types)";_tag_dyneither(_tmp32A,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpC71;});
goto _LL11;}else{goto _LL4C;}case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}else{switch(*((int*)_tmp313.f2)){case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}}case 5U: switch(*((int*)_tmp313.f2)){case 5U: _LL22: _tmp39A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).elt_typ;_tmp399=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).elt_tq;_tmp398=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).ptr_atts).rgn;_tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).ptr_atts).nullable;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).ptr_atts).bounds;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f1)->f1).ptr_atts).zero_term;_tmp394=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).elt_typ;_tmp393=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).elt_tq;_tmp392=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).ptr_atts).rgn;_tmp391=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).ptr_atts).nullable;_tmp390=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).ptr_atts).bounds;_tmp38F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313.f2)->f1).ptr_atts).zero_term;_LL23:
# 1497
 Cyc_Tcutil_unify_it(_tmp394,_tmp39A);
Cyc_Tcutil_unify_it(_tmp398,_tmp392);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpC73=_tmp38F;union Cyc_Absyn_Constraint*_tmpC72=_tmp395;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC73,_tmpC72,({const char*_tmp32B="(not both zero terminated)";_tag_dyneither(_tmp32B,sizeof(char),27U);}));});
Cyc_Tcutil_unify_tqual(_tmp393,_tmp394,_tmp399,_tmp39A);
({union Cyc_Absyn_Constraint*_tmpC75=_tmp390;union Cyc_Absyn_Constraint*_tmpC74=_tmp396;((void(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmpC75,_tmpC74,({const char*_tmp32C="(different pointer bounds)";_tag_dyneither(_tmp32C,sizeof(char),27U);}));});
# 1505
{void*_tmp32D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp390);void*_tmp32E=_tmp32D;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp32E)->tag == 0U){_LL55: _LL56:
 return;}else{_LL57: _LL58:
 goto _LL54;}_LL54:;}
# 1509
({union Cyc_Absyn_Constraint*_tmpC77=_tmp391;union Cyc_Absyn_Constraint*_tmpC76=_tmp397;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC77,_tmpC76,({const char*_tmp32F="(incompatible pointer types)";_tag_dyneither(_tmp32F,sizeof(char),29U);}));});
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 6U: switch(*((int*)_tmp313.f2)){case 6U: _LL24: _tmp39E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp39D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313.f1)->f2;_tmp39C=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313.f2)->f1;_tmp39B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp313.f2)->f2;_LL25:
# 1514
 if(_tmp39C == _tmp39E  && ((_tmp39B == _tmp39D  || _tmp39B == Cyc_Absyn_Int_sz  && _tmp39D == Cyc_Absyn_Long_sz) || 
_tmp39B == Cyc_Absyn_Long_sz  && _tmp39D == Cyc_Absyn_Int_sz))return;
({struct _dyneither_ptr _tmpC78=({const char*_tmp330="(different integral types)";_tag_dyneither(_tmp330,sizeof(char),27U);});Cyc_Tcutil_failure_reason=_tmpC78;});
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
({struct _dyneither_ptr _tmpC79=({const char*_tmp331="(cannot prove expressions are the same)";_tag_dyneither(_tmp331,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpC79;});
goto _LL11;}
# 1533
return;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 8U: switch(*((int*)_tmp313.f2)){case 8U: _LL2C: _tmp3AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f1)->f1).elt_type;_tmp3AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f1)->f1).tq;_tmp3AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f1)->f1).num_elts;_tmp3A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f1)->f1).zero_term;_tmp3A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f2)->f1).elt_type;_tmp3A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f2)->f1).tq;_tmp3A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f2)->f1).num_elts;_tmp3A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313.f2)->f1).zero_term;_LL2D:
# 1537
 Cyc_Tcutil_unify_it(_tmp3A8,_tmp3AC);
Cyc_Tcutil_unify_tqual(_tmp3A7,_tmp3A8,_tmp3AB,_tmp3AC);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({union Cyc_Absyn_Constraint*_tmpC7B=_tmp3A9;union Cyc_Absyn_Constraint*_tmpC7A=_tmp3A5;((void(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y,struct _dyneither_ptr reason))Cyc_Tcutil_unify_it_conrefs)(Cyc_Core_intcmp,_tmpC7B,_tmpC7A,({const char*_tmp332="(not both zero terminated)";_tag_dyneither(_tmp332,sizeof(char),27U);}));});
if(_tmp3AA == _tmp3A6)return;
if(_tmp3AA == 0  || _tmp3A6 == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp3AA,_tmp3A6))
return;
({struct _dyneither_ptr _tmpC7C=({const char*_tmp333="(different array sizes)";_tag_dyneither(_tmp333,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpC7C;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 9U: switch(*((int*)_tmp313.f2)){case 9U: _LL2E: _tmp3C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).tvars;_tmp3C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).effect;_tmp3C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).ret_tqual;_tmp3C3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).ret_typ;_tmp3C2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).args;_tmp3C1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).c_varargs;_tmp3C0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).cyc_varargs;_tmp3BF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).rgn_po;_tmp3BE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).attributes;_tmp3BD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).requires_clause;_tmp3BC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).requires_relns;_tmp3BB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).ensures_clause;_tmp3BA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f1)->f1).ensures_relns;_tmp3B9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).tvars;_tmp3B8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).effect;_tmp3B7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).ret_tqual;_tmp3B6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).ret_typ;_tmp3B5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).args;_tmp3B4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).c_varargs;_tmp3B3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).cyc_varargs;_tmp3B2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).rgn_po;_tmp3B1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).attributes;_tmp3B0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).requires_clause;_tmp3AF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).requires_relns;_tmp3AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).ensures_clause;_tmp3AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp313.f2)->f1).ensures_relns;_LL2F: {
# 1551
int done=0;
{struct _RegionHandle _tmp334=_new_region("rgn");struct _RegionHandle*rgn=& _tmp334;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp3B9 != 0){
if(_tmp3C6 == 0){
({struct _dyneither_ptr _tmpC7D=({const char*_tmp335="(second function type has too few type variables)";_tag_dyneither(_tmp335,sizeof(char),50U);});Cyc_Tcutil_failure_reason=_tmpC7D;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1559
void*_tmp336=((struct Cyc_Absyn_Tvar*)_tmp3B9->hd)->kind;
void*_tmp337=((struct Cyc_Absyn_Tvar*)_tmp3C6->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp336,_tmp337)){
({struct _dyneither_ptr _tmpC82=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp33C;_tmp33C.tag=0U;({struct _dyneither_ptr _tmpC7E=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1565
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3C6->hd,& Cyc_Tcutil_bk)));_tmp33C.f1=_tmpC7E;});({struct Cyc_String_pa_PrintArg_struct _tmp33B;_tmp33B.tag=0U;({struct _dyneither_ptr _tmpC7F=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1564
Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp3B9->hd,& Cyc_Tcutil_bk)));_tmp33B.f1=_tmpC7F;});({struct Cyc_String_pa_PrintArg_struct _tmp33A;_tmp33A.tag=0U;({struct _dyneither_ptr _tmpC80=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 1563
Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp3B9->hd));_tmp33A.f1=_tmpC80;});({void*_tmp338[3U]={& _tmp33A,& _tmp33B,& _tmp33C};({struct _dyneither_ptr _tmpC81=({const char*_tmp339="(type var %s has different kinds %s and %s)";_tag_dyneither(_tmp339,sizeof(char),44U);});Cyc_aprintf(_tmpC81,_tag_dyneither(_tmp338,sizeof(void*),3U));});});});});});
# 1562
Cyc_Tcutil_failure_reason=_tmpC82;});
# 1566
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1568
({struct Cyc_List_List*_tmpC86=({struct Cyc_List_List*_tmp33D=_region_malloc(rgn,sizeof(*_tmp33D));({struct _tuple16*_tmpC85=({struct _tuple16*_tmp33E=_region_malloc(rgn,sizeof(*_tmp33E));_tmp33E->f1=(struct Cyc_Absyn_Tvar*)_tmp3C6->hd;({void*_tmpC84=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp33F=_cycalloc(sizeof(*_tmp33F));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpC83=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp340;_tmp340.tag=2U;_tmp340.f1=(struct Cyc_Absyn_Tvar*)_tmp3B9->hd;_tmp340;});_tmp33F[0]=_tmpC83;});_tmp33F;});_tmp33E->f2=_tmpC84;});_tmp33E;});_tmp33D->hd=_tmpC85;});_tmp33D->tl=inst;_tmp33D;});inst=_tmpC86;});
_tmp3B9=_tmp3B9->tl;
_tmp3C6=_tmp3C6->tl;};}
# 1572
if(_tmp3C6 != 0){
({struct _dyneither_ptr _tmpC87=({const char*_tmp341="(second function type has too many type variables)";_tag_dyneither(_tmp341,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpC87;});
_npop_handler(0U);goto _LL11;}
# 1576
if(inst != 0){
({void*_tmpC8E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp342=_cycalloc(sizeof(*_tmp342));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC89=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp343;_tmp343.tag=9U;({struct Cyc_Absyn_FnInfo _tmpC88=({struct Cyc_Absyn_FnInfo _tmp344;_tmp344.tvars=0;_tmp344.effect=_tmp3B8;_tmp344.ret_tqual=_tmp3B7;_tmp344.ret_typ=_tmp3B6;_tmp344.args=_tmp3B5;_tmp344.c_varargs=_tmp3B4;_tmp344.cyc_varargs=_tmp3B3;_tmp344.rgn_po=_tmp3B2;_tmp344.attributes=_tmp3B1;_tmp344.requires_clause=_tmp3BD;_tmp344.requires_relns=_tmp3BC;_tmp344.ensures_clause=_tmp3BB;_tmp344.ensures_relns=_tmp3BA;_tmp344;});_tmp343.f1=_tmpC88;});_tmp343;});_tmp342[0]=_tmpC89;});_tmp342;});Cyc_Tcutil_unify_it(_tmpC8E,({
# 1580
struct _RegionHandle*_tmpC8D=rgn;struct Cyc_List_List*_tmpC8C=inst;Cyc_Tcutil_rsubstitute(_tmpC8D,_tmpC8C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp345=_cycalloc(sizeof(*_tmp345));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpC8B=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp346;_tmp346.tag=9U;({struct Cyc_Absyn_FnInfo _tmpC8A=({struct Cyc_Absyn_FnInfo _tmp347;_tmp347.tvars=0;_tmp347.effect=_tmp3C5;_tmp347.ret_tqual=_tmp3C4;_tmp347.ret_typ=_tmp3C3;_tmp347.args=_tmp3C2;_tmp347.c_varargs=_tmp3C1;_tmp347.cyc_varargs=_tmp3C0;_tmp347.rgn_po=_tmp3BF;_tmp347.attributes=_tmp3BE;_tmp347.requires_clause=_tmp3B0;_tmp347.requires_relns=_tmp3AF;_tmp347.ensures_clause=_tmp3AE;_tmp347.ensures_relns=_tmp3AD;_tmp347;});_tmp346.f1=_tmpC8A;});_tmp346;});_tmp345[0]=_tmpC8B;});_tmp345;}));}));});
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
({struct _dyneither_ptr _tmpC8F=({const char*_tmp350="(function types have different number of arguments)";_tag_dyneither(_tmp350,sizeof(char),52U);});Cyc_Tcutil_failure_reason=_tmpC8F;});
goto _LL11;}
# 1604
if(_tmp3B4 != _tmp3C1){
({struct _dyneither_ptr _tmpC90=({const char*_tmp351="(only one function type takes C varargs)";_tag_dyneither(_tmp351,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpC90;});
goto _LL11;}{
# 1609
int bad_cyc_vararg=0;
{struct _tuple22 _tmp352=({struct _tuple22 _tmp35E;_tmp35E.f1=_tmp3B3;_tmp35E.f2=_tmp3C0;_tmp35E;});struct _tuple22 _tmp353=_tmp352;struct _dyneither_ptr*_tmp35D;struct Cyc_Absyn_Tqual _tmp35C;void*_tmp35B;int _tmp35A;struct _dyneither_ptr*_tmp359;struct Cyc_Absyn_Tqual _tmp358;void*_tmp357;int _tmp356;if(_tmp353.f1 == 0){if(_tmp353.f2 == 0){_LL60: _LL61:
 goto _LL5F;}else{_LL62: _LL63:
 goto _LL65;}}else{if(_tmp353.f2 == 0){_LL64: _LL65:
# 1614
 bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC91=({const char*_tmp354="(only one function type takes varargs)";_tag_dyneither(_tmp354,sizeof(char),39U);});Cyc_Tcutil_failure_reason=_tmpC91;});
goto _LL5F;}else{_LL66: _tmp35D=(_tmp353.f1)->name;_tmp35C=(_tmp353.f1)->tq;_tmp35B=(_tmp353.f1)->type;_tmp35A=(_tmp353.f1)->inject;_tmp359=(_tmp353.f2)->name;_tmp358=(_tmp353.f2)->tq;_tmp357=(_tmp353.f2)->type;_tmp356=(_tmp353.f2)->inject;_LL67:
# 1618
 Cyc_Tcutil_unify_it(_tmp35B,_tmp357);
Cyc_Tcutil_unify_tqual(_tmp35C,_tmp35B,_tmp358,_tmp357);
if(_tmp35A != _tmp356){
bad_cyc_vararg=1;
({struct _dyneither_ptr _tmpC92=({const char*_tmp355="(only one function type injects varargs)";_tag_dyneither(_tmp355,sizeof(char),41U);});Cyc_Tcutil_failure_reason=_tmpC92;});}
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
({int _tmpC94=!({void*_tmpC93=(void*)_check_null(_tmp3B8);Cyc_Tcutil_unify_effect(_tmpC93,(void*)_check_null(_tmp3C5));});bad_effect=_tmpC94;});goto _LL68;}}_LL68:;}
# 1636
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
({struct _dyneither_ptr _tmpC95=({const char*_tmp362="(function effects do not match)";_tag_dyneither(_tmp362,sizeof(char),32U);});Cyc_Tcutil_failure_reason=_tmpC95;});
goto _LL11;}
# 1642
if(!Cyc_Tcutil_same_atts(_tmp3BE,_tmp3B1)){
({struct _dyneither_ptr _tmpC96=({const char*_tmp363="(function types have different attributes)";_tag_dyneither(_tmp363,sizeof(char),43U);});Cyc_Tcutil_failure_reason=_tmpC96;});
goto _LL11;}
# 1646
if(!Cyc_Tcutil_same_rgn_po(_tmp3BF,_tmp3B2)){
({struct _dyneither_ptr _tmpC97=({const char*_tmp364="(function types have different region lifetime orderings)";_tag_dyneither(_tmp364,sizeof(char),58U);});Cyc_Tcutil_failure_reason=_tmpC97;});
goto _LL11;}
# 1650
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3BC,_tmp3AF)){
({struct _dyneither_ptr _tmpC98=({const char*_tmp365="(@requires clauses not equivalent)";_tag_dyneither(_tmp365,sizeof(char),35U);});Cyc_Tcutil_failure_reason=_tmpC98;});
goto _LL11;}
# 1654
if(!Cyc_Tcutil_check_logical_equivalence(_tmp3BA,_tmp3AD)){
({struct _dyneither_ptr _tmpC99=({const char*_tmp366="(@ensures clauses not equivalent)";_tag_dyneither(_tmp366,sizeof(char),34U);});Cyc_Tcutil_failure_reason=_tmpC99;});
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
({struct _dyneither_ptr _tmpC9A=({const char*_tmp36F="(tuple types have different numbers of components)";_tag_dyneither(_tmp36F,sizeof(char),51U);});Cyc_Tcutil_failure_reason=_tmpC9A;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 12U: switch(*((int*)_tmp313.f2)){case 12U: _LL32: _tmp3CC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313.f1)->f1;_tmp3CB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313.f1)->f2;_tmp3CA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313.f2)->f1;_tmp3C9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp313.f2)->f2;_LL33:
# 1674
 if(_tmp3CA != _tmp3CC){({struct _dyneither_ptr _tmpC9B=({const char*_tmp370="(struct and union type)";_tag_dyneither(_tmp370,sizeof(char),24U);});Cyc_Tcutil_failure_reason=_tmpC9B;});goto _LL11;}
for(0;_tmp3C9 != 0  && _tmp3CB != 0;(_tmp3C9=_tmp3C9->tl,_tmp3CB=_tmp3CB->tl)){
struct Cyc_Absyn_Aggrfield*_tmp371=(struct Cyc_Absyn_Aggrfield*)_tmp3C9->hd;
struct Cyc_Absyn_Aggrfield*_tmp372=(struct Cyc_Absyn_Aggrfield*)_tmp3CB->hd;
if(Cyc_strptrcmp(_tmp371->name,_tmp372->name)!= 0){
({struct _dyneither_ptr _tmpC9C=({const char*_tmp373="(different member names)";_tag_dyneither(_tmp373,sizeof(char),25U);});Cyc_Tcutil_failure_reason=_tmpC9C;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1682
Cyc_Tcutil_unify_it(_tmp371->type,_tmp372->type);
Cyc_Tcutil_unify_tqual(_tmp371->tq,_tmp371->type,_tmp372->tq,_tmp372->type);
if(!Cyc_Tcutil_same_atts(_tmp371->attributes,_tmp372->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC9D=({const char*_tmp374="(different attributes on member)";_tag_dyneither(_tmp374,sizeof(char),33U);});Cyc_Tcutil_failure_reason=_tmpC9D;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1690
if((_tmp371->width != 0  && _tmp372->width == 0  || 
_tmp372->width != 0  && _tmp371->width == 0) || 
(_tmp371->width != 0  && _tmp372->width != 0) && !({
struct Cyc_Absyn_Exp*_tmpC9E=(struct Cyc_Absyn_Exp*)_check_null(_tmp371->width);Cyc_Evexp_same_const_exp(_tmpC9E,(struct Cyc_Absyn_Exp*)_check_null(_tmp372->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpC9F=({const char*_tmp375="(different bitfield widths on member)";_tag_dyneither(_tmp375,sizeof(char),38U);});Cyc_Tcutil_failure_reason=_tmpC9F;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1699
if((_tmp371->requires_clause != 0  && _tmp372->requires_clause == 0  || 
_tmp371->requires_clause == 0  && _tmp372->requires_clause != 0) || 
(_tmp371->requires_clause == 0  && _tmp372->requires_clause != 0) && !({
struct Cyc_Absyn_Exp*_tmpCA0=(struct Cyc_Absyn_Exp*)_check_null(_tmp371->requires_clause);Cyc_Evexp_same_const_exp(_tmpCA0,(struct Cyc_Absyn_Exp*)_check_null(_tmp372->requires_clause));})){
# 1704
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpCA1=({const char*_tmp376="(different @requires clauses on member)";_tag_dyneither(_tmp376,sizeof(char),40U);});Cyc_Tcutil_failure_reason=_tmpCA1;});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1710
if(_tmp3C9 == 0  && _tmp3CB == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
({struct _dyneither_ptr _tmpCA2=({const char*_tmp377="(different number of members)";_tag_dyneither(_tmp377,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpCA2;});
goto _LL11;case 24U: goto _LL42;case 25U: goto _LL48;case 23U: goto _LL4A;default: goto _LL4C;}case 17U: switch(*((int*)_tmp313.f2)){case 17U: _LL34: _tmp3D0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313.f1)->f2;_tmp3CF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313.f1)->f3;_tmp3CE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313.f2)->f2;_tmp3CD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp313.f2)->f3;_LL35:
# 1716
 if(_tmp3CF != _tmp3CD){
({struct _dyneither_ptr _tmpCA3=({const char*_tmp378="(different abstract typedefs)";_tag_dyneither(_tmp378,sizeof(char),30U);});Cyc_Tcutil_failure_reason=_tmpCA3;});
goto _LL11;}
# 1720
({struct _dyneither_ptr _tmpCA4=({const char*_tmp379="(type parameters to typedef differ)";_tag_dyneither(_tmp379,sizeof(char),36U);});Cyc_Tcutil_failure_reason=_tmpCA4;});
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
({struct _dyneither_ptr _tmpCA5=({const char*_tmp37A="(effects don't unify)";_tag_dyneither(_tmp37A,sizeof(char),22U);});Cyc_Tcutil_failure_reason=_tmpCA5;});
goto _LL11;default: _LL4C: _LL4D:
 goto _LL11;}}}}_LL11:;}
# 1744
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1747
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0  && a2 != 0)return - 1;
if(a1 != 0  && a2 == 0)return 1;
return({int(*_tmpCA7)(void*,void*)=cmp;void*_tmpCA6=(void*)_check_null(a1);_tmpCA7(_tmpCA6,(void*)_check_null(a2));});}
# 1754
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp3D8=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp3D9=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp3D8,_tmp3D9);}
# 1761
static int Cyc_Tcutil_conrefs_cmp(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y){
({union Cyc_Absyn_Constraint*_tmpCA8=Cyc_Absyn_compress_conref(x);x=_tmpCA8;});
({union Cyc_Absyn_Constraint*_tmpCA9=Cyc_Absyn_compress_conref(y);y=_tmpCA9;});
if(x == y)return 0;{
union Cyc_Absyn_Constraint*_tmp3DA=x;void*_tmp3E1;switch((((union Cyc_Absyn_Constraint*)_tmp3DA)->Eq_constr).tag){case 3U: _LL1: _LL2:
 return - 1;case 1U: _LL3: _tmp3E1=(void*)(_tmp3DA->Eq_constr).val;_LL4: {
# 1768
union Cyc_Absyn_Constraint*_tmp3DB=y;void*_tmp3DE;switch((((union Cyc_Absyn_Constraint*)_tmp3DB)->Eq_constr).tag){case 3U: _LL8: _LL9:
 return 1;case 1U: _LLA: _tmp3DE=(void*)(_tmp3DB->Eq_constr).val;_LLB:
 return cmp(_tmp3E1,_tmp3DE);default: _LLC: _LLD:
({void*_tmp3DC=0U;({struct _dyneither_ptr _tmpCAA=({const char*_tmp3DD="unify_conref: forward after compress(2)";_tag_dyneither(_tmp3DD,sizeof(char),40U);});Cyc_Tcutil_impos(_tmpCAA,_tag_dyneither(_tmp3DC,sizeof(void*),0U));});});}_LL7:;}default: _LL5: _LL6:
# 1773
({void*_tmp3DF=0U;({struct _dyneither_ptr _tmpCAB=({const char*_tmp3E0="unify_conref: forward after compress";_tag_dyneither(_tmp3E0,sizeof(char),37U);});Cyc_Tcutil_impos(_tmpCAB,_tag_dyneither(_tmp3DF,sizeof(void*),0U));});});}_LL0:;};}
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
({int _tmpCAC=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);_tmp3EC=_tmpCAC;});
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
({void*_tmpCAD=Cyc_Tcutil_compress(t1);t1=_tmpCAD;});
({void*_tmpCAE=Cyc_Tcutil_compress(t2);t2=_tmpCAE;});
if(t1 == t2)return 0;{
int _tmp3EF=({int _tmpCAF=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmpCAF,Cyc_Tcutil_type_case_number(t2));});
if(_tmp3EF != 0)
return _tmp3EF;{
# 1851
struct _tuple0 _tmp3F0=({struct _tuple0 _tmp453;_tmp453.f1=t2;_tmp453.f2=t1;_tmp453;});struct _tuple0 _tmp3F1=_tmp3F0;struct _dyneither_ptr _tmp452;struct _dyneither_ptr _tmp451;struct Cyc_Absyn_Exp*_tmp450;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_Absyn_Exp*_tmp44D;void*_tmp44C;void*_tmp44B;void*_tmp44A;void*_tmp449;void*_tmp448;void*_tmp447;void*_tmp446;void*_tmp445;enum Cyc_Absyn_AggrKind _tmp444;struct Cyc_List_List*_tmp443;enum Cyc_Absyn_AggrKind _tmp442;struct Cyc_List_List*_tmp441;struct Cyc_List_List*_tmp440;struct Cyc_List_List*_tmp43F;void*_tmp43E;struct Cyc_Absyn_Tqual _tmp43D;struct Cyc_Absyn_Exp*_tmp43C;union Cyc_Absyn_Constraint*_tmp43B;void*_tmp43A;struct Cyc_Absyn_Tqual _tmp439;struct Cyc_Absyn_Exp*_tmp438;union Cyc_Absyn_Constraint*_tmp437;int _tmp436;int _tmp435;enum Cyc_Absyn_Sign _tmp434;enum Cyc_Absyn_Size_of _tmp433;enum Cyc_Absyn_Sign _tmp432;enum Cyc_Absyn_Size_of _tmp431;void*_tmp430;struct Cyc_Absyn_Tqual _tmp42F;void*_tmp42E;union Cyc_Absyn_Constraint*_tmp42D;union Cyc_Absyn_Constraint*_tmp42C;union Cyc_Absyn_Constraint*_tmp42B;void*_tmp42A;struct Cyc_Absyn_Tqual _tmp429;void*_tmp428;union Cyc_Absyn_Constraint*_tmp427;union Cyc_Absyn_Constraint*_tmp426;union Cyc_Absyn_Constraint*_tmp425;struct Cyc_Absyn_Datatypedecl*_tmp424;struct Cyc_Absyn_Datatypefield*_tmp423;struct Cyc_List_List*_tmp422;struct Cyc_Absyn_Datatypedecl*_tmp421;struct Cyc_Absyn_Datatypefield*_tmp420;struct Cyc_List_List*_tmp41F;struct Cyc_Absyn_Datatypedecl*_tmp41E;struct Cyc_List_List*_tmp41D;struct Cyc_Absyn_Datatypedecl*_tmp41C;struct Cyc_List_List*_tmp41B;struct Cyc_List_List*_tmp41A;struct Cyc_List_List*_tmp419;struct _tuple2*_tmp418;struct _tuple2*_tmp417;union Cyc_Absyn_AggrInfoU _tmp416;struct Cyc_List_List*_tmp415;union Cyc_Absyn_AggrInfoU _tmp414;struct Cyc_List_List*_tmp413;struct Cyc_Absyn_Tvar*_tmp412;struct Cyc_Absyn_Tvar*_tmp411;switch(*((int*)_tmp3F1.f1)){case 1U: switch(*((int*)_tmp3F1.f2)){case 1U: _LL1: _LL2:
# 1853
({void*_tmp3F2=0U;({struct _dyneither_ptr _tmpCB0=({const char*_tmp3F3="typecmp: can only compare closed types";_tag_dyneither(_tmp3F3,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB0,_tag_dyneither(_tmp3F2,sizeof(void*),0U));});});case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 2U: switch(*((int*)_tmp3F1.f2)){case 2U: _LL3: _tmp412=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp411=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL4:
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
({void*_tmp408=0U;({struct _dyneither_ptr _tmpCB1=({const char*_tmp409="missing expression in array index";_tag_dyneither(_tmp409,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB1,_tag_dyneither(_tmp408,sizeof(void*),0U));});});
# 1930
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp43C,_tmp438);};};}case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 9U: switch(*((int*)_tmp3F1.f2)){case 9U: _LL17: _LL18:
# 1933
({void*_tmp40A=0U;({struct _dyneither_ptr _tmpCB2=({const char*_tmp40B="typecmp: function types not handled";_tag_dyneither(_tmp40B,sizeof(char),36U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB2,_tag_dyneither(_tmp40A,sizeof(void*),0U));});});case 24U: goto _LL2B;case 25U: goto _LL31;case 23U: goto _LL33;default: goto _LL35;}case 10U: switch(*((int*)_tmp3F1.f2)){case 10U: _LL19: _tmp440=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F1.f1)->f1;_tmp43F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F1.f2)->f1;_LL1A:
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
({void*_tmp40D=0U;({struct _dyneither_ptr _tmpCB3=({const char*_tmp40E="typecmp: effects not handled";_tag_dyneither(_tmp40E,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB3,_tag_dyneither(_tmp40D,sizeof(void*),0U));});});default: _LL35: _LL36:
({void*_tmp40F=0U;({struct _dyneither_ptr _tmpCB4=({const char*_tmp410="Unmatched case in typecmp";_tag_dyneither(_tmp410,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCB4,_tag_dyneither(_tmp40F,sizeof(void*),0U));});});}}}}_LL0:;};};}
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
({void*_tmpCB5=Cyc_Tcutil_compress(t1);t1=_tmpCB5;});
({void*_tmpCB6=Cyc_Tcutil_compress(t2);t2=_tmpCB6;});{
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
({struct Cyc_Core_Opt*_tmpCB7=({struct Cyc_Core_Opt*_tmp45B=_cycalloc(sizeof(*_tmp45B));_tmp45B->v=t1;_tmp45B;});max_arith_type=_tmpCB7;});}}}
# 2020
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2024
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp45F;_tmp45F.tag=0U;({struct _dyneither_ptr _tmpCB8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp45F.f1=_tmpCB8;});({struct Cyc_String_pa_PrintArg_struct _tmp45E;_tmp45E.tag=0U;({struct _dyneither_ptr _tmpCB9=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp45E.f1=_tmpCB9;});({void*_tmp45C[2U]={& _tmp45E,& _tmp45F};({unsigned int _tmpCBB=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _dyneither_ptr _tmpCBA=({const char*_tmp45D="type mismatch: expecting %s but found %s";_tag_dyneither(_tmp45D,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpCBB,_tmpCBA,_tag_dyneither(_tmp45C,sizeof(void*),2U));});});});});
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
({void*_tmp464=0U;({unsigned int _tmpCBD=e->loc;struct _dyneither_ptr _tmpCBC=({const char*_tmp465="integral size mismatch; conversion supplied";_tag_dyneither(_tmp465,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpCBD,_tmpCBC,_tag_dyneither(_tmp464,sizeof(void*),0U));});});
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
({void*_tmp466=0U;({unsigned int _tmpCBF=e->loc;struct _dyneither_ptr _tmpCBE=({const char*_tmp467="integral size mismatch; conversion supplied";_tag_dyneither(_tmp467,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpCBF,_tmpCBE,_tag_dyneither(_tmp466,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_typ,Cyc_Absyn_No_coercion);
return 1;}
# 2079
return 0;}
# 2084
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 2092
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
# 2094
({void*_tmpCC0=Cyc_Tcutil_compress(t1);t1=_tmpCC0;});
({void*_tmpCC1=Cyc_Tcutil_compress(t2);t2=_tmpCC1;});{
struct _tuple0 _tmp468=({struct _tuple0 _tmp481;_tmp481.f1=t1;_tmp481.f2=t2;_tmp481;});struct _tuple0 _tmp469=_tmp468;void*_tmp480;struct Cyc_Absyn_Tqual _tmp47F;struct Cyc_Absyn_Exp*_tmp47E;union Cyc_Absyn_Constraint*_tmp47D;void*_tmp47C;struct Cyc_Absyn_Tqual _tmp47B;struct Cyc_Absyn_Exp*_tmp47A;union Cyc_Absyn_Constraint*_tmp479;struct Cyc_Absyn_PtrInfo _tmp478;struct Cyc_Absyn_PtrInfo _tmp477;switch(*((int*)_tmp469.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f2)->tag == 5U){_LL1: _tmp478=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f1)->f1;_tmp477=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp469.f2)->f1;_LL2: {
# 2098
int okay=1;
# 2100
if(!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp478.ptr_atts).nullable,(_tmp477.ptr_atts).nullable))
# 2103
({int _tmpCC2=!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp478.ptr_atts).nullable);okay=_tmpCC2;});
# 2105
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp478.ptr_atts).bounds,(_tmp477.ptr_atts).bounds)){
# 2108
struct _tuple0 _tmp46A=({struct _tuple0 _tmp471;({void*_tmpCC3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp478.ptr_atts).bounds);_tmp471.f1=_tmpCC3;});({void*_tmpCC4=
((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,(_tmp477.ptr_atts).bounds);_tmp471.f2=_tmpCC4;});_tmp471;});
# 2108
struct _tuple0 _tmp46B=_tmp46A;struct Cyc_Absyn_Exp*_tmp470;struct Cyc_Absyn_Exp*_tmp46F;struct Cyc_Absyn_Exp*_tmp46E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f1)->tag == 1U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp46B.f2)->tag == 0U){_LLA: _LLB:
# 2110
 goto _LLD;}else{_LLE: _tmp46F=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f1)->f1;_tmp46E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp46B.f2)->f1;_LLF:
# 2113
({int _tmpCC5=okay  && Cyc_Evexp_lte_const_exp(_tmp46E,_tmp46F);okay=_tmpCC5;});
# 2117
if(!((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp477.ptr_atts).zero_term))
({void*_tmp46C=0U;({unsigned int _tmpCC7=loc;struct _dyneither_ptr _tmpCC6=({const char*_tmp46D="implicit cast to shorter array";_tag_dyneither(_tmp46D,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpCC7,_tmpCC6,_tag_dyneither(_tmp46C,sizeof(void*),0U));});});
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
({struct Cyc_String_pa_PrintArg_struct _tmp475;_tmp475.tag=0U;({struct _dyneither_ptr _tmpCC8=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2139
Cyc_Absynpp_typ2string((_tmp477.ptr_atts).rgn));_tmp475.f1=_tmpCC8;});({struct Cyc_String_pa_PrintArg_struct _tmp474;_tmp474.tag=0U;({struct _dyneither_ptr _tmpCC9=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 2138
Cyc_Absynpp_typ2string((_tmp478.ptr_atts).rgn));_tmp474.f1=_tmpCC9;});({void*_tmp472[2U]={& _tmp474,& _tmp475};({unsigned int _tmpCCB=loc;struct _dyneither_ptr _tmpCCA=({const char*_tmp473="implicit cast from region %s to region %s";_tag_dyneither(_tmp473,sizeof(char),42U);});Cyc_Tcutil_warn(_tmpCCB,_tmpCCA,_tag_dyneither(_tmp472,sizeof(void*),2U));});});});});}else{
# 2140
okay=0;}}
# 2144
({int _tmpCCC=okay  && (((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,(_tmp478.ptr_atts).zero_term,(_tmp477.ptr_atts).zero_term) || 
# 2147
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,(_tmp478.ptr_atts).zero_term) && (_tmp477.elt_tq).real_const);
# 2144
okay=_tmpCCC;});{
# 2155
int _tmp476=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,(_tmp477.ptr_atts).bounds,Cyc_Absyn_bounds_one_conref) && !
# 2158
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,(_tmp477.ptr_atts).zero_term);
# 2162
({int _tmpCCD=okay  && (Cyc_Tcutil_unify(_tmp478.elt_typ,_tmp477.elt_typ) || 
(_tmp476  && ((_tmp477.elt_tq).real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp477.elt_typ)))) && Cyc_Tcutil_ptrsubtype(te,0,_tmp478.elt_typ,_tmp477.elt_typ));
# 2162
okay=_tmpCCD;});
# 2165
return okay;};}}else{goto _LL7;}case 8U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->tag == 8U){_LL3: _tmp480=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).elt_type;_tmp47F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).tq;_tmp47E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).num_elts;_tmp47D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f1)->f1).zero_term;_tmp47C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).elt_type;_tmp47B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).tq;_tmp47A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).num_elts;_tmp479=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp469.f2)->f1).zero_term;_LL4: {
# 2169
int okay;
# 2171
({int _tmpCCE=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp47D,_tmp479) && (
(_tmp47E != 0  && _tmp47A != 0) && Cyc_Evexp_same_const_exp(_tmp47E,_tmp47A));
# 2171
okay=_tmpCCE;});
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
# 2188
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp484=Cyc_Tcutil_compress(t);void*_tmp485=_tmp484;void*_tmp488;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp485)->tag == 5U){_LL1: _tmp488=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp485)->f1).elt_typ;_LL2:
 return _tmp488;}else{_LL3: _LL4:
({void*_tmp486=0U;({struct _dyneither_ptr _tmpCCF=({const char*_tmp487="pointer_elt_type";_tag_dyneither(_tmp487,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCCF,_tag_dyneither(_tmp486,sizeof(void*),0U));});});}_LL0:;}
# 2194
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp489=Cyc_Tcutil_compress(t);void*_tmp48A=_tmp489;struct Cyc_Absyn_PtrAtts*_tmp48D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48A)->tag == 5U){_LL1: _tmp48D=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48A)->f1).ptr_atts;_LL2:
 return _tmp48D->rgn;}else{_LL3: _LL4:
({void*_tmp48B=0U;({struct _dyneither_ptr _tmpCD0=({const char*_tmp48C="pointer_elt_type";_tag_dyneither(_tmp48C,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD0,_tag_dyneither(_tmp48B,sizeof(void*),0U));});});}_LL0:;}
# 2201
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp48E=Cyc_Tcutil_compress(t);void*_tmp48F=_tmp48E;void*_tmp490;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48F)->tag == 5U){_LL1: _tmp490=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp48F)->f1).ptr_atts).rgn;_LL2:
# 2204
*rgn=_tmp490;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2211
int Cyc_Tcutil_is_boxed(void*t){
return(Cyc_Tcutil_typ_kind(t))->kind == Cyc_Absyn_BoxKind;}
# 2215
int Cyc_Tcutil_is_dyneither_ptr(void*t){
void*_tmp491=Cyc_Tcutil_compress(t);void*_tmp492=_tmp491;union Cyc_Absyn_Constraint*_tmp493;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp492)->tag == 5U){_LL1: _tmp493=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp492)->f1).ptr_atts).bounds;_LL2:
# 2218
 return((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp493)== (void*)& Cyc_Absyn_DynEither_b_val;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2224
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp494=e->r;void*_tmp495=_tmp494;void*_tmp499;struct Cyc_Absyn_Exp*_tmp498;struct _dyneither_ptr _tmp497;switch(*((int*)_tmp495)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp497=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp495)->f1).Wchar_c).val;_LLC: {
# 2233
unsigned long _tmp496=Cyc_strlen((struct _dyneither_ptr)_tmp497);
int i=0;
if(_tmp496 >= 2  && *((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp497.curr)[1]== 'x'  && _tmp496 >= 3) && *((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp496;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp497,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 2243
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2231
 return 1;case 14U: _LLD: _tmp499=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f1;_tmp498=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp495)->f2;_LLE:
# 2244
 return Cyc_Tcutil_is_zero(_tmp498) && Cyc_Tcutil_admits_zero(_tmp499);default: _LLF: _LL10:
 return 0;}_LL0:;}
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
struct Cyc_Absyn_Kind*_tmp49A=ka;enum Cyc_Absyn_KindQual _tmp4A3;enum Cyc_Absyn_AliasQual _tmp4A2;_LL1: _tmp4A3=_tmp49A->kind;_tmp4A2=_tmp49A->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp49B=_tmp4A2;switch(_tmp49B){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2287
enum Cyc_Absyn_KindQual _tmp49C=_tmp4A3;switch(_tmp49C){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& Cyc_Tcutil_eko;default: _LL15: _LL16:
 return& Cyc_Tcutil_iko;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2296
{enum Cyc_Absyn_KindQual _tmp49D=_tmp4A3;switch(_tmp49D){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& Cyc_Tcutil_urko;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2303
goto _LL3;default: _LL8: _LL9:
# 2305
{enum Cyc_Absyn_KindQual _tmp49E=_tmp4A3;switch(_tmp49E){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& Cyc_Tcutil_trko;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2312
goto _LL3;}_LL3:;}
# 2314
({struct Cyc_String_pa_PrintArg_struct _tmp4A1;_tmp4A1.tag=0U;({struct _dyneither_ptr _tmpCD1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp4A1.f1=_tmpCD1;});({void*_tmp49F[1U]={& _tmp4A1};({struct _dyneither_ptr _tmpCD2=({const char*_tmp4A0="kind_to_opt: bad kind %s\n";_tag_dyneither(_tmp4A0,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD2,_tag_dyneither(_tmp49F,sizeof(void*),1U));});});});}
# 2317
static void**Cyc_Tcutil_kind_to_b(struct Cyc_Absyn_Kind*ka){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ab_v={0U,& Cyc_Tcutil_ak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct mb_v={0U,& Cyc_Tcutil_mk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct bb_v={0U,& Cyc_Tcutil_bk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rb_v={0U,& Cyc_Tcutil_rk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct eb_v={0U,& Cyc_Tcutil_ek};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ib_v={0U,& Cyc_Tcutil_ik};
# 2325
static void*ab=(void*)& ab_v;
static void*mb=(void*)& mb_v;
static void*bb=(void*)& bb_v;
static void*rb=(void*)& rb_v;
static void*eb=(void*)& eb_v;
static void*ib=(void*)& ib_v;
# 2332
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tab_v={0U,& Cyc_Tcutil_tak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tmb_v={0U,& Cyc_Tcutil_tmk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct tbb_v={0U,& Cyc_Tcutil_tbk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct trb_v={0U,& Cyc_Tcutil_trk};
# 2337
static void*tab=(void*)& tab_v;
static void*tmb=(void*)& tmb_v;
static void*tbb=(void*)& tbb_v;
static void*trb=(void*)& trb_v;
# 2342
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct uab_v={0U,& Cyc_Tcutil_uak};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct umb_v={0U,& Cyc_Tcutil_umk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct ubb_v={0U,& Cyc_Tcutil_ubk};
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct urb_v={0U,& Cyc_Tcutil_urk};
# 2347
static void*uab=(void*)& uab_v;
static void*umb=(void*)& umb_v;
static void*ubb=(void*)& ubb_v;
static void*urb=(void*)& urb_v;
# 2352
struct Cyc_Absyn_Kind*_tmp4A4=ka;enum Cyc_Absyn_KindQual _tmp4AD;enum Cyc_Absyn_AliasQual _tmp4AC;_LL1: _tmp4AD=_tmp4A4->kind;_tmp4AC=_tmp4A4->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp4A5=_tmp4AC;switch(_tmp4A5){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2355
enum Cyc_Absyn_KindQual _tmp4A6=_tmp4AD;switch(_tmp4A6){case Cyc_Absyn_AnyKind: _LLB: _LLC:
 return& ab;case Cyc_Absyn_MemKind: _LLD: _LLE:
 return& mb;case Cyc_Absyn_BoxKind: _LLF: _LL10:
 return& bb;case Cyc_Absyn_RgnKind: _LL11: _LL12:
 return& rb;case Cyc_Absyn_EffKind: _LL13: _LL14:
 return& eb;default: _LL15: _LL16:
 return& ib;}_LLA:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2364
{enum Cyc_Absyn_KindQual _tmp4A7=_tmp4AD;switch(_tmp4A7){case Cyc_Absyn_AnyKind: _LL18: _LL19:
 return& uab;case Cyc_Absyn_MemKind: _LL1A: _LL1B:
 return& umb;case Cyc_Absyn_BoxKind: _LL1C: _LL1D:
 return& ubb;case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return& urb;default: _LL20: _LL21:
 goto _LL17;}_LL17:;}
# 2371
goto _LL3;default: _LL8: _LL9:
# 2373
{enum Cyc_Absyn_KindQual _tmp4A8=_tmp4AD;switch(_tmp4A8){case Cyc_Absyn_AnyKind: _LL23: _LL24:
 return& tab;case Cyc_Absyn_MemKind: _LL25: _LL26:
 return& tmb;case Cyc_Absyn_BoxKind: _LL27: _LL28:
 return& tbb;case Cyc_Absyn_RgnKind: _LL29: _LL2A:
 return& trb;default: _LL2B: _LL2C:
 goto _LL22;}_LL22:;}
# 2380
goto _LL3;}_LL3:;}
# 2382
({struct Cyc_String_pa_PrintArg_struct _tmp4AB;_tmp4AB.tag=0U;({struct _dyneither_ptr _tmpCD3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(ka));_tmp4AB.f1=_tmpCD3;});({void*_tmp4A9[1U]={& _tmp4AB};({struct _dyneither_ptr _tmpCD4=({const char*_tmp4AA="kind_to_b: bad kind %s\n";_tag_dyneither(_tmp4AA,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCD4,_tag_dyneither(_tmp4A9,sizeof(void*),1U));});});});}
# 2385
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return*Cyc_Tcutil_kind_to_b(k);}
# 2388
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
# 2390
return({struct Cyc_Core_Opt*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));({void*_tmpCD5=Cyc_Tcutil_kind_to_bound(k);_tmp4BC->v=_tmpCD5;});_tmp4BC;});}
# 2395
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2) && Cyc_Tcutil_is_zero(e1)){
({void*_tmpCD7=(void*)({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmpCD6=({struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct _tmp4BE;_tmp4BE.tag=0U;_tmp4BE.f1=Cyc_Absyn_Null_c;_tmp4BE;});_tmp4BD[0]=_tmpCD6;});_tmp4BD;});e1->r=_tmpCD7;});{
struct Cyc_Core_Opt*_tmp4BF=Cyc_Tcenv_lookup_opt_type_vars(te);
struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C0=({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpCDF=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp4C6;_tmp4C6.tag=5U;({struct Cyc_Absyn_PtrInfo _tmpCDE=({struct Cyc_Absyn_PtrInfo _tmp4C7;({void*_tmpCD8=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp4BF);_tmp4C7.elt_typ=_tmpCD8;});({struct Cyc_Absyn_Tqual _tmpCD9=
Cyc_Absyn_empty_tqual(0U);_tmp4C7.elt_tq=_tmpCD9;});({struct Cyc_Absyn_PtrAtts _tmpCDD=({({void*_tmpCDA=
Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp4BF);(_tmp4C7.ptr_atts).rgn=_tmpCDA;});(_tmp4C7.ptr_atts).nullable=Cyc_Absyn_true_conref;({union Cyc_Absyn_Constraint*_tmpCDB=
# 2403
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4C7.ptr_atts).bounds=_tmpCDB;});({union Cyc_Absyn_Constraint*_tmpCDC=
((union Cyc_Absyn_Constraint*(*)())Cyc_Absyn_empty_conref)();(_tmp4C7.ptr_atts).zero_term=_tmpCDC;});(_tmp4C7.ptr_atts).ptrloc=0;_tmp4C7.ptr_atts;});_tmp4C7.ptr_atts=_tmpCDD;});_tmp4C7;});_tmp4C6.f1=_tmpCDE;});_tmp4C6;});_tmp4C5[0]=_tmpCDF;});_tmp4C5;});
e1->topt=(void*)_tmp4C0;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4C4;_tmp4C4.tag=0U;({struct _dyneither_ptr _tmpCE0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Position_string_of_segment(e1->loc));_tmp4C4.f1=_tmpCE0;});({struct Cyc_String_pa_PrintArg_struct _tmp4C3;_tmp4C3.tag=0U;({struct _dyneither_ptr _tmpCE1=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp4C3.f1=_tmpCE1;});({void*_tmp4C1[2U]={& _tmp4C3,& _tmp4C4};({struct _dyneither_ptr _tmpCE2=({const char*_tmp4C2="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_dyneither(_tmp4C2,sizeof(char),56U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpCE2,_tag_dyneither(_tmp4C1,sizeof(void*),2U));});});});});
return retv;};};}
# 2413
return 0;}
# 2416
struct _dyneither_ptr Cyc_Tcutil_coercion2string(enum Cyc_Absyn_Coercion c){
enum Cyc_Absyn_Coercion _tmp4C8=c;switch(_tmp4C8){case Cyc_Absyn_Unknown_coercion: _LL1: _LL2:
 return({const char*_tmp4C9="unknown";_tag_dyneither(_tmp4C9,sizeof(char),8U);});case Cyc_Absyn_No_coercion: _LL3: _LL4:
 return({const char*_tmp4CA="no coercion";_tag_dyneither(_tmp4CA,sizeof(char),12U);});case Cyc_Absyn_Null_to_NonNull: _LL5: _LL6:
 return({const char*_tmp4CB="null check";_tag_dyneither(_tmp4CB,sizeof(char),11U);});default: _LL7: _LL8:
 return({const char*_tmp4CC="other coercion";_tag_dyneither(_tmp4CC,sizeof(char),15U);});}_LL0:;}
# 2425
int Cyc_Tcutil_warn_alias_coerce=0;
# 2431
struct _tuple15 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_typ){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2435
static int counter=0;
struct _tuple2*v=({struct _tuple2*_tmp4E0=_cycalloc(sizeof(*_tmp4E0));_tmp4E0->f1=Cyc_Absyn_Loc_n;({struct _dyneither_ptr*_tmpCE5=({struct _dyneither_ptr*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));({struct _dyneither_ptr _tmpCE4=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4E4;_tmp4E4.tag=1U;_tmp4E4.f1=(unsigned long)counter ++;({void*_tmp4E2[1U]={& _tmp4E4};({struct _dyneither_ptr _tmpCE3=({const char*_tmp4E3="__aliasvar%d";_tag_dyneither(_tmp4E3,sizeof(char),13U);});Cyc_aprintf(_tmpCE3,_tag_dyneither(_tmp4E2,sizeof(void*),1U));});});});_tmp4E1[0]=_tmpCE4;});_tmp4E1;});_tmp4E0->f2=_tmpCE5;});_tmp4E0;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_typ,e);
struct Cyc_Absyn_Exp*ve=({void*_tmpCE7=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp4DE=_cycalloc(sizeof(*_tmp4DE));({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmpCE6=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct _tmp4DF;_tmp4DF.tag=4U;_tmp4DF.f1=vd;_tmp4DF;});_tmp4DE[0]=_tmpCE6;});_tmp4DE;});Cyc_Absyn_varb_exp(_tmpCE7,e->loc);});
# 2444
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2446
{void*_tmp4CD=Cyc_Tcutil_compress(e_typ);void*_tmp4CE=_tmp4CD;void*_tmp4DC;struct Cyc_Absyn_Tqual _tmp4DB;void*_tmp4DA;union Cyc_Absyn_Constraint*_tmp4D9;union Cyc_Absyn_Constraint*_tmp4D8;union Cyc_Absyn_Constraint*_tmp4D7;struct Cyc_Absyn_PtrLoc*_tmp4D6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->tag == 5U){_LL1: _tmp4DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).elt_typ;_tmp4DB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).elt_tq;_tmp4DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).rgn;_tmp4D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).nullable;_tmp4D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).bounds;_tmp4D7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).zero_term;_tmp4D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4CE)->f1).ptr_atts).ptrloc;_LL2:
# 2448
{void*_tmp4CF=Cyc_Tcutil_compress(_tmp4DA);void*_tmp4D0=_tmp4CF;void**_tmp4D5;struct Cyc_Core_Opt*_tmp4D4;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D0)->tag == 1U){_LL6: _tmp4D5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D0)->f2;_tmp4D4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4D0)->f4;_LL7: {
# 2450
struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D1=({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpCE8=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp4D3;_tmp4D3.tag=2U;_tmp4D3.f1=tv;_tmp4D3;});_tmp4D2[0]=_tmpCE8;});_tmp4D2;});
*_tmp4D5=(void*)_tmp4D1;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2455
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
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
if((Cyc_Tcutil_is_noalias_path(e) && 
Cyc_Tcutil_is_noalias_pointer(e_typ,0)) && 
Cyc_Tcutil_is_pointer_type(e_typ)){
# 2478
void*_tmp4E6=Cyc_Tcutil_compress(wants_typ);void*_tmp4E7=_tmp4E6;void*_tmp4EB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E7)->tag == 5U){_LL1: _tmp4EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E7)->f1).ptr_atts).rgn;_LL2: {
# 2480
void*_tmp4E8=Cyc_Tcutil_compress(_tmp4EB);void*_tmp4E9=_tmp4E8;if(((struct Cyc_Absyn_HeapRgn_Absyn_Type_struct*)_tmp4E9)->tag == 20U){_LL6: _LL7:
 return 0;}else{_LL8: _LL9: {
# 2483
struct Cyc_Absyn_Kind*_tmp4EA=Cyc_Tcutil_typ_kind(_tmp4EB);
return _tmp4EA->kind == Cyc_Absyn_RgnKind  && _tmp4EA->aliasqual == Cyc_Absyn_Aliasable;}}_LL5:;}}else{_LL3: _LL4:
# 2486
 return 0;}_LL0:;}
# 2489
return 0;}
# 2493
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2498
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2500
if(Cyc_Tcutil_is_arithmetic_type(t2) && Cyc_Tcutil_is_arithmetic_type(t1)){
# 2502
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp4EF;_tmp4EF.tag=0U;({struct _dyneither_ptr _tmpCE9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4EF.f1=_tmpCE9;});({struct Cyc_String_pa_PrintArg_struct _tmp4EE;_tmp4EE.tag=0U;({struct _dyneither_ptr _tmpCEA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4EE.f1=_tmpCEA;});({void*_tmp4EC[2U]={& _tmp4EE,& _tmp4EF};({unsigned int _tmpCEC=e->loc;struct _dyneither_ptr _tmpCEB=({const char*_tmp4ED="integral size mismatch; %s -> %s conversion supplied";_tag_dyneither(_tmp4ED,sizeof(char),53U);});Cyc_Tcutil_warn(_tmpCEC,_tmpCEB,_tag_dyneither(_tmp4EC,sizeof(void*),2U));});});});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2510
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp4F4;_tmp4F4.tag=0U;({struct _dyneither_ptr _tmpCED=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4F4.f1=_tmpCED;});({struct Cyc_String_pa_PrintArg_struct _tmp4F3;_tmp4F3.tag=0U;({struct _dyneither_ptr _tmpCEE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F3.f1=_tmpCEE;});({struct Cyc_String_pa_PrintArg_struct _tmp4F2;_tmp4F2.tag=0U;({struct _dyneither_ptr _tmpCEF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp4F2.f1=_tmpCEF;});({void*_tmp4F0[3U]={& _tmp4F2,& _tmp4F3,& _tmp4F4};({unsigned int _tmpCF1=e->loc;struct _dyneither_ptr _tmpCF0=({const char*_tmp4F1="implicit alias coercion for %s:%s to %s";_tag_dyneither(_tmp4F1,sizeof(char),40U);});Cyc_Tcutil_warn(_tmpCF1,_tmpCF0,_tag_dyneither(_tmp4F0,sizeof(void*),3U));});});});});});
*alias_coercion=1;}
# 2517
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if(({enum Cyc_Absyn_Coercion _tmpCF2=Cyc_Tcutil_castable(te,e->loc,t1,t2);c=_tmpCF2;})!= Cyc_Absyn_Unknown_coercion){
# 2525
if(c != Cyc_Absyn_No_coercion)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if(c != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp4F8;_tmp4F8.tag=0U;({struct _dyneither_ptr _tmpCF3=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(t2));_tmp4F8.f1=_tmpCF3;});({struct Cyc_String_pa_PrintArg_struct _tmp4F7;_tmp4F7.tag=0U;({struct _dyneither_ptr _tmpCF4=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t1));_tmp4F7.f1=_tmpCF4;});({void*_tmp4F5[2U]={& _tmp4F7,& _tmp4F8};({unsigned int _tmpCF6=e->loc;struct _dyneither_ptr _tmpCF5=({const char*_tmp4F6="implicit cast from %s to %s";_tag_dyneither(_tmp4F6,sizeof(char),28U);});Cyc_Tcutil_warn(_tmpCF6,_tmpCF5,_tag_dyneither(_tmp4F5,sizeof(void*),2U));});});});});
return 1;}else{
# 2531
return 0;}}}}}
# 2538
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2541
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2545
int Cyc_Tcutil_coerceable(void*t){
void*_tmp4F9=Cyc_Tcutil_compress(t);void*_tmp4FA=_tmp4F9;switch(*((int*)_tmp4FA)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 2563 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2567
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 2570
struct _tuple23 _tmp4FB=*env;struct _tuple23 _tmp4FC=_tmp4FB;struct Cyc_List_List*_tmp504;struct _RegionHandle*_tmp503;struct Cyc_Tcenv_Tenv*_tmp502;int _tmp501;_LL1: _tmp504=_tmp4FC.f1;_tmp503=_tmp4FC.f2;_tmp502=_tmp4FC.f3;_tmp501=_tmp4FC.f4;_LL2:;{
# 2572
void*_tmp4FD=_tmp504 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp503,_tmp504,x->type);
struct Cyc_List_List*_tmp4FE=Cyc_Tcutil_flatten_typ(_tmp503,_tmp501,_tmp502,_tmp4FD);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4FE)== 1)
return({struct Cyc_List_List*_tmp4FF=_region_malloc(_tmp503,sizeof(*_tmp4FF));({struct _tuple12*_tmpCF7=({struct _tuple12*_tmp500=_region_malloc(_tmp503,sizeof(*_tmp500));_tmp500->f1=x->tq;_tmp500->f2=_tmp4FD;_tmp500;});_tmp4FF->hd=_tmpCF7;});_tmp4FF->tl=0;_tmp4FF;});else{
return _tmp4FE;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2578
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple12*x){
# 2580
struct _tuple24 _tmp505=*env;struct _tuple24 _tmp506=_tmp505;struct _RegionHandle*_tmp510;struct Cyc_Tcenv_Tenv*_tmp50F;int _tmp50E;_LL1: _tmp510=_tmp506.f1;_tmp50F=_tmp506.f2;_tmp50E=_tmp506.f3;_LL2:;{
struct _tuple12 _tmp507=*x;struct _tuple12 _tmp508=_tmp507;struct Cyc_Absyn_Tqual _tmp50D;void*_tmp50C;_LL4: _tmp50D=_tmp508.f1;_tmp50C=_tmp508.f2;_LL5:;{
struct Cyc_List_List*_tmp509=Cyc_Tcutil_flatten_typ(_tmp510,_tmp50E,_tmp50F,_tmp50C);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp509)== 1)
return({struct Cyc_List_List*_tmp50A=_region_malloc(_tmp510,sizeof(*_tmp50A));({struct _tuple12*_tmpCF8=({struct _tuple12*_tmp50B=_region_malloc(_tmp510,sizeof(*_tmp50B));_tmp50B->f1=_tmp50D;_tmp50B->f2=_tmp50C;_tmp50B;});_tmp50A->hd=_tmpCF8;});_tmp50A->tl=0;_tmp50A;});else{
return _tmp509;}};};}
# 2587
static struct Cyc_List_List*Cyc_Tcutil_flatten_typ(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2591
if(flatten){
({void*_tmpCF9=Cyc_Tcutil_compress(t1);t1=_tmpCF9;});{
void*_tmp511=t1;struct Cyc_List_List*_tmp533;struct Cyc_Absyn_Aggrdecl*_tmp532;struct Cyc_List_List*_tmp531;struct Cyc_List_List*_tmp530;switch(*((int*)_tmp511)){case 0U: _LL1: _LL2:
 return 0;case 10U: _LL3: _tmp530=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp511)->f1;_LL4: {
# 2596
struct _tuple24 _tmp512=({struct _tuple24 _tmp51B;_tmp51B.f1=r;_tmp51B.f2=te;_tmp51B.f3=flatten;_tmp51B;});
# 2598
struct Cyc_List_List*_tmp513=_tmp530;struct _tuple12*_tmp51A;struct Cyc_List_List*_tmp519;if(_tmp513 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp51A=(struct _tuple12*)_tmp513->hd;_tmp519=_tmp513->tl;_LLF: {
# 2601
struct Cyc_List_List*_tmp514=Cyc_Tcutil_rcopy_tqt(& _tmp512,_tmp51A);
_tmp512.f3=0;{
struct Cyc_List_List*_tmp515=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple12*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp512,_tmp530);
struct Cyc_List_List*_tmp516=({struct Cyc_List_List*_tmp518=_region_malloc(r,sizeof(*_tmp518));_tmp518->hd=_tmp514;_tmp518->tl=_tmp515;_tmp518;});
return({struct _RegionHandle*_tmpCFA=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmpCFA,({struct Cyc_List_List*_tmp517=_region_malloc(r,sizeof(*_tmp517));_tmp517->hd=_tmp514;_tmp517->tl=_tmp515;_tmp517;}));});};}}_LLB:;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp511)->f1).aggr_info).KnownAggr).tag == 2){_LL5: _tmp532=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp511)->f1).aggr_info).KnownAggr).val;_tmp531=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp511)->f1).targs;_LL6:
# 2609
 if(((_tmp532->kind == Cyc_Absyn_UnionA  || _tmp532->impl == 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp532->impl))->exist_vars != 0) || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp532->impl))->rgn_po != 0)
# 2611
return({struct Cyc_List_List*_tmp51C=_region_malloc(r,sizeof(*_tmp51C));({struct _tuple12*_tmpCFC=({struct _tuple12*_tmp51D=_region_malloc(r,sizeof(*_tmp51D));({struct Cyc_Absyn_Tqual _tmpCFB=Cyc_Absyn_empty_tqual(0U);_tmp51D->f1=_tmpCFB;});_tmp51D->f2=t1;_tmp51D;});_tmp51C->hd=_tmpCFC;});_tmp51C->tl=0;_tmp51C;});{
struct Cyc_List_List*_tmp51E=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp532->tvs,_tmp531);
struct _tuple23 env=({struct _tuple23 _tmp527;_tmp527.f1=_tmp51E;_tmp527.f2=r;_tmp527.f3=te;_tmp527.f4=flatten;_tmp527;});
struct Cyc_List_List*_tmp51F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp532->impl))->fields;struct Cyc_List_List*_tmp520=_tmp51F;struct Cyc_Absyn_Aggrfield*_tmp526;struct Cyc_List_List*_tmp525;if(_tmp520 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp526=(struct Cyc_Absyn_Aggrfield*)_tmp520->hd;_tmp525=_tmp520->tl;_LL14: {
# 2617
struct Cyc_List_List*_tmp521=Cyc_Tcutil_flatten_typ_f(& env,_tmp526);
env.f4=0;{
struct Cyc_List_List*_tmp522=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp525);
struct Cyc_List_List*_tmp523=({struct Cyc_List_List*_tmp524=_region_malloc(r,sizeof(*_tmp524));_tmp524->hd=_tmp521;_tmp524->tl=_tmp522;_tmp524;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp523);};}}_LL10:;};}else{goto _LL9;}case 12U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp511)->f1 == Cyc_Absyn_StructA){_LL7: _tmp533=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp511)->f2;_LL8: {
# 2624
struct _tuple23 env=({struct _tuple23 _tmp52F;_tmp52F.f1=0;_tmp52F.f2=r;_tmp52F.f3=te;_tmp52F.f4=flatten;_tmp52F;});
struct Cyc_List_List*_tmp528=_tmp533;struct Cyc_Absyn_Aggrfield*_tmp52E;struct Cyc_List_List*_tmp52D;if(_tmp528 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp52E=(struct Cyc_Absyn_Aggrfield*)_tmp528->hd;_tmp52D=_tmp528->tl;_LL19: {
# 2628
struct Cyc_List_List*_tmp529=Cyc_Tcutil_flatten_typ_f(& env,_tmp52E);
env.f4=0;{
struct Cyc_List_List*_tmp52A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_typ_f,& env,_tmp52D);
struct Cyc_List_List*_tmp52B=({struct Cyc_List_List*_tmp52C=_region_malloc(r,sizeof(*_tmp52C));_tmp52C->hd=_tmp529;_tmp52C->tl=_tmp52A;_tmp52C;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp52B);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2634
 goto _LL0;}_LL0:;};}
# 2637
return({struct Cyc_List_List*_tmp534=_region_malloc(r,sizeof(*_tmp534));({struct _tuple12*_tmpCFE=({struct _tuple12*_tmp535=_region_malloc(r,sizeof(*_tmp535));({struct Cyc_Absyn_Tqual _tmpCFD=Cyc_Absyn_empty_tqual(0U);_tmp535->f1=_tmpCFD;});_tmp535->f2=t1;_tmp535;});_tmp534->hd=_tmpCFE;});_tmp534->tl=0;_tmp534;});}
# 2641
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp536=(void*)t->hd;void*_tmp537=_tmp536;switch(*((int*)_tmp537)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
# 2647
 continue;default: _LL7: _LL8:
# 2649
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2652
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Tcutil_equal_att,(void*)a2->hd,a1))return 0;}
# 2655
return 1;}
# 2658
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple0 _tmp538=({struct _tuple0 _tmp53C;({void*_tmpCFF=Cyc_Tcutil_compress(t1);_tmp53C.f1=_tmpCFF;});({void*_tmpD00=Cyc_Tcutil_compress(t2);_tmp53C.f2=_tmpD00;});_tmp53C;});struct _tuple0 _tmp539=_tmp538;enum Cyc_Absyn_Size_of _tmp53B;enum Cyc_Absyn_Size_of _tmp53A;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp539.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp539.f2)->tag == 6U){_LL1: _tmp53B=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp539.f1)->f2;_tmp53A=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp539.f2)->f2;_LL2:
# 2661
 return(_tmp53B == _tmp53A  || _tmp53B == Cyc_Absyn_Int_sz  && _tmp53A == Cyc_Absyn_Long_sz) || 
_tmp53B == Cyc_Absyn_Long_sz  && _tmp53A == Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2669
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2672
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple0*)a->hd)).f1) && Cyc_Tcutil_unify(t2,(*((struct _tuple0*)a->hd)).f2))
return 1;}}
# 2677
({void*_tmpD01=Cyc_Tcutil_compress(t1);t1=_tmpD01;});
({void*_tmpD02=Cyc_Tcutil_compress(t2);t2=_tmpD02;});{
struct _tuple0 _tmp53D=({struct _tuple0 _tmp570;_tmp570.f1=t1;_tmp570.f2=t2;_tmp570;});struct _tuple0 _tmp53E=_tmp53D;struct Cyc_Absyn_FnInfo _tmp56F;struct Cyc_Absyn_FnInfo _tmp56E;struct Cyc_Absyn_Datatypedecl*_tmp56D;struct Cyc_Absyn_Datatypefield*_tmp56C;struct Cyc_List_List*_tmp56B;struct Cyc_Absyn_Datatypedecl*_tmp56A;struct Cyc_List_List*_tmp569;void*_tmp568;struct Cyc_Absyn_Tqual _tmp567;void*_tmp566;union Cyc_Absyn_Constraint*_tmp565;union Cyc_Absyn_Constraint*_tmp564;union Cyc_Absyn_Constraint*_tmp563;void*_tmp562;struct Cyc_Absyn_Tqual _tmp561;void*_tmp560;union Cyc_Absyn_Constraint*_tmp55F;union Cyc_Absyn_Constraint*_tmp55E;union Cyc_Absyn_Constraint*_tmp55D;switch(*((int*)_tmp53E.f1)){case 5U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f2)->tag == 5U){_LL1: _tmp568=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f1)->f1).elt_typ;_tmp567=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f1)->f1).elt_tq;_tmp566=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f1)->f1).ptr_atts).rgn;_tmp565=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f1)->f1).ptr_atts).nullable;_tmp564=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f1)->f1).ptr_atts).bounds;_tmp563=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f1)->f1).ptr_atts).zero_term;_tmp562=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f2)->f1).elt_typ;_tmp561=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f2)->f1).elt_tq;_tmp560=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f2)->f1).ptr_atts).rgn;_tmp55F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f2)->f1).ptr_atts).nullable;_tmp55E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f2)->f1).ptr_atts).bounds;_tmp55D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E.f2)->f1).ptr_atts).zero_term;_LL2:
# 2685
 if(_tmp567.real_const  && !_tmp561.real_const)
return 0;
# 2688
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp565,_tmp55F) && 
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp565)) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp55F))
return 0;
# 2692
if((!((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp563,_tmp55D) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp563)) && ((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp55D))
return 0;
# 2696
if((!Cyc_Tcutil_unify(_tmp566,_tmp560) && !Cyc_Tcenv_region_outlives(te,_tmp566,_tmp560)) && !
Cyc_Tcutil_subtype(te,assume,_tmp566,_tmp560))
return 0;
# 2700
if(!((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp564,_tmp55E)){
struct _tuple0 _tmp53F=({struct _tuple0 _tmp543;({void*_tmpD03=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp564);_tmp543.f1=_tmpD03;});({void*_tmpD04=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp55E);_tmp543.f2=_tmpD04;});_tmp543;});struct _tuple0 _tmp540=_tmp53F;struct Cyc_Absyn_Exp*_tmp542;struct Cyc_Absyn_Exp*_tmp541;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp540.f1)->tag == 0U){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp540.f2)->tag == 0U){_LLA: _LLB:
 goto _LL9;}else{goto _LLE;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp540.f2)->tag == 1U){_LLC: _tmp542=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp540.f1)->f1;_tmp541=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp540.f2)->f1;_LLD:
# 2705
 if(!Cyc_Evexp_lte_const_exp(_tmp541,_tmp542))
return 0;
goto _LL9;}else{_LLE: _LLF:
 return 0;}}_LL9:;}
# 2713
if(!_tmp561.real_const  && _tmp567.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp562)))
return 0;}{
# 2719
int _tmp544=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp55E,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp55D);
# 2725
return(_tmp544  && ({struct Cyc_Tcenv_Tenv*_tmpD08=te;struct Cyc_List_List*_tmpD07=({struct Cyc_List_List*_tmp545=_cycalloc(sizeof(*_tmp545));({struct _tuple0*_tmpD05=({struct _tuple0*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->f1=t1;_tmp546->f2=t2;_tmp546;});_tmp545->hd=_tmpD05;});_tmp545->tl=assume;_tmp545;});void*_tmpD06=_tmp568;Cyc_Tcutil_ptrsubtype(_tmpD08,_tmpD07,_tmpD06,_tmp562);}) || Cyc_Tcutil_unify(_tmp568,_tmp562)) || Cyc_Tcutil_isomorphic(_tmp568,_tmp562);};}else{goto _LL7;}case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp53E.f1)->f1).field_info).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp53E.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp53E.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LL3: _tmp56D=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp53E.f1)->f1).field_info).KnownDatatypefield).val).f1;_tmp56C=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp53E.f1)->f1).field_info).KnownDatatypefield).val).f2;_tmp56B=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp53E.f1)->f1).targs;_tmp56A=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp53E.f2)->f1).datatype_info).KnownDatatype).val;_tmp569=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp53E.f2)->f1).targs;_LL4:
# 2731
 if(_tmp56D != _tmp56A  && Cyc_Absyn_qvar_cmp(_tmp56D->name,_tmp56A->name)!= 0)return 0;
# 2733
if(({int _tmpD09=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp56B);_tmpD09 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp569);}))return 0;
for(0;_tmp56B != 0;(_tmp56B=_tmp56B->tl,_tmp569=_tmp569->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp56B->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp569))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 9U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53E.f2)->tag == 9U){_LL5: _tmp56F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53E.f1)->f1;_tmp56E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp53E.f2)->f1;_LL6:
# 2740
 if(_tmp56F.tvars != 0  || _tmp56E.tvars != 0){
struct Cyc_List_List*_tmp547=_tmp56F.tvars;
struct Cyc_List_List*_tmp548=_tmp56E.tvars;
if(({int _tmpD0A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp547);_tmpD0A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp548);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp547 != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp547->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp548))->hd)->kind))return 0;
({struct Cyc_List_List*_tmpD0E=({struct Cyc_List_List*_tmp549=_cycalloc(sizeof(*_tmp549));({struct _tuple16*_tmpD0D=({struct _tuple16*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->f1=(struct Cyc_Absyn_Tvar*)_tmp548->hd;({void*_tmpD0C=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD0B=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp54C;_tmp54C.tag=2U;_tmp54C.f1=(struct Cyc_Absyn_Tvar*)_tmp547->hd;_tmp54C;});_tmp54B[0]=_tmpD0B;});_tmp54B;});_tmp54A->f2=_tmpD0C;});_tmp54A;});_tmp549->hd=_tmpD0D;});_tmp549->tl=inst;_tmp549;});inst=_tmpD0E;});
_tmp547=_tmp547->tl;
_tmp548=_tmp548->tl;}
# 2751
if(inst != 0){
_tmp56F.tvars=0;
_tmp56E.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmpD13=te;struct Cyc_List_List*_tmpD12=assume;void*_tmpD11=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54D=_cycalloc(sizeof(*_tmp54D));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD0F=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp54E;_tmp54E.tag=9U;_tmp54E.f1=_tmp56F;_tmp54E;});_tmp54D[0]=_tmpD0F;});_tmp54D;});Cyc_Tcutil_subtype(_tmpD13,_tmpD12,_tmpD11,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp54F=_cycalloc(sizeof(*_tmp54F));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD10=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp550;_tmp550.tag=9U;_tmp550.f1=_tmp56E;_tmp550;});_tmp54F[0]=_tmpD10;});_tmp54F;}));});}};}
# 2758
if(!Cyc_Tcutil_subtype(te,assume,_tmp56F.ret_typ,_tmp56E.ret_typ))return 0;{
struct Cyc_List_List*_tmp551=_tmp56F.args;
struct Cyc_List_List*_tmp552=_tmp56E.args;
# 2763
if(({int _tmpD14=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp551);_tmpD14 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp552);}))return 0;
# 2765
for(0;_tmp551 != 0;(_tmp551=_tmp551->tl,_tmp552=_tmp552->tl)){
struct _tuple10 _tmp553=*((struct _tuple10*)_tmp551->hd);struct _tuple10 _tmp554=_tmp553;struct Cyc_Absyn_Tqual _tmp55A;void*_tmp559;_LL11: _tmp55A=_tmp554.f2;_tmp559=_tmp554.f3;_LL12:;{
struct _tuple10 _tmp555=*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp552))->hd);struct _tuple10 _tmp556=_tmp555;struct Cyc_Absyn_Tqual _tmp558;void*_tmp557;_LL14: _tmp558=_tmp556.f2;_tmp557=_tmp556.f3;_LL15:;
# 2769
if(_tmp558.real_const  && !_tmp55A.real_const  || !Cyc_Tcutil_subtype(te,assume,_tmp557,_tmp559))
return 0;};}
# 2773
if(_tmp56F.c_varargs != _tmp56E.c_varargs)return 0;
if(_tmp56F.cyc_varargs != 0  && _tmp56E.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp55B=*_tmp56F.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp55C=*_tmp56E.cyc_varargs;
# 2778
if((_tmp55C.tq).real_const  && !(_tmp55B.tq).real_const  || !
Cyc_Tcutil_subtype(te,assume,_tmp55C.type,_tmp55B.type))
return 0;}else{
if(_tmp56F.cyc_varargs != 0  || _tmp56E.cyc_varargs != 0)return 0;}
# 2783
if(!({void*_tmpD15=(void*)_check_null(_tmp56F.effect);Cyc_Tcutil_subset_effect(1,_tmpD15,(void*)_check_null(_tmp56E.effect));}))return 0;
# 2785
if(!Cyc_Tcutil_sub_rgnpo(_tmp56F.rgn_po,_tmp56E.rgn_po))return 0;
# 2787
if(!Cyc_Tcutil_sub_attributes(_tmp56F.attributes,_tmp56E.attributes))return 0;
# 2789
if(!Cyc_Tcutil_check_logical_implication(_tmp56E.requires_relns,_tmp56F.requires_relns))
return 0;
# 2792
if(!Cyc_Tcutil_check_logical_implication(_tmp56F.ensures_relns,_tmp56E.ensures_relns))
return 0;
# 2795
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2807 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2809
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_typ(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp571=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp572=_tmp571;struct Cyc_Absyn_Tqual _tmp578;void*_tmp577;_LL1: _tmp578=_tmp572->f1;_tmp577=_tmp572->f2;_LL2:;{
struct _tuple12*_tmp573=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp574=_tmp573;struct Cyc_Absyn_Tqual _tmp576;void*_tmp575;_LL4: _tmp576=_tmp574->f1;_tmp575=_tmp574->f2;_LL5:;
# 2816
if(_tmp578.real_const  && !_tmp576.real_const)return 0;
# 2818
if((_tmp576.real_const  || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp575))) && 
Cyc_Tcutil_subtype(te,assume,_tmp577,_tmp575))
# 2821
continue;
# 2823
if(Cyc_Tcutil_unify(_tmp577,_tmp575))
# 2825
continue;
# 2827
if(Cyc_Tcutil_isomorphic(_tmp577,_tmp575))
# 2829
continue;
# 2832
return 0;};};}
# 2834
return 1;}
# 2837
static int Cyc_Tcutil_is_char_type(void*t){
void*_tmp579=Cyc_Tcutil_compress(t);void*_tmp57A=_tmp579;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp57A)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp57A)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2846
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned int loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
({void*_tmpD16=Cyc_Tcutil_compress(t1);t1=_tmpD16;});
({void*_tmpD17=Cyc_Tcutil_compress(t2);t2=_tmpD17;});
# 2852
if(t2 == (void*)& Cyc_Absyn_VoidType_val)
return Cyc_Absyn_No_coercion;
{void*_tmp57B=t2;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp57B)->tag == 6U)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp57B)->f2){case Cyc_Absyn_Int_sz: _LL1: _LL2:
# 2856
 goto _LL4;case Cyc_Absyn_Long_sz: _LL3: _LL4:
# 2858
 if((Cyc_Tcutil_typ_kind(t1))->kind == Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 2862
void*_tmp57C=t1;void*_tmp5A4;struct Cyc_Absyn_Enumdecl*_tmp5A3;void*_tmp5A2;struct Cyc_Absyn_Tqual _tmp5A1;struct Cyc_Absyn_Exp*_tmp5A0;union Cyc_Absyn_Constraint*_tmp59F;void*_tmp59E;struct Cyc_Absyn_Tqual _tmp59D;void*_tmp59C;union Cyc_Absyn_Constraint*_tmp59B;union Cyc_Absyn_Constraint*_tmp59A;union Cyc_Absyn_Constraint*_tmp599;switch(*((int*)_tmp57C)){case 5U: _LL8: _tmp59E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).elt_typ;_tmp59D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).elt_tq;_tmp59C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).rgn;_tmp59B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).nullable;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).bounds;_tmp599=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).zero_term;_LL9:
# 2871
{void*_tmp57D=t2;void*_tmp58F;struct Cyc_Absyn_Tqual _tmp58E;void*_tmp58D;union Cyc_Absyn_Constraint*_tmp58C;union Cyc_Absyn_Constraint*_tmp58B;union Cyc_Absyn_Constraint*_tmp58A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->tag == 5U){_LL17: _tmp58F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).elt_typ;_tmp58E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).elt_tq;_tmp58D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).ptr_atts).rgn;_tmp58C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).ptr_atts).nullable;_tmp58B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).ptr_atts).bounds;_tmp58A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57D)->f1).ptr_atts).zero_term;_LL18: {
# 2875
enum Cyc_Absyn_Coercion coercion=Cyc_Absyn_Other_coercion;
struct Cyc_List_List*_tmp57E=({struct Cyc_List_List*_tmp588=_cycalloc(sizeof(*_tmp588));({struct _tuple0*_tmpD18=({struct _tuple0*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->f1=t1;_tmp589->f2=t2;_tmp589;});_tmp588->hd=_tmpD18;});_tmp588->tl=0;_tmp588;});
int _tmp57F=_tmp58E.real_const  || !_tmp59D.real_const;
# 2889 "tcutil.cyc"
int _tmp580=
((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp58B,Cyc_Absyn_bounds_one_conref) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp58A);
# 2893
int _tmp581=_tmp57F  && (
((_tmp580  && Cyc_Tcutil_ptrsubtype(te,_tmp57E,_tmp59E,_tmp58F) || 
Cyc_Tcutil_unify(_tmp59E,_tmp58F)) || Cyc_Tcutil_isomorphic(_tmp59E,_tmp58F)) || Cyc_Tcutil_unify(_tmp58F,(void*)& Cyc_Absyn_VoidType_val));
# 2897
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp599,_tmp58A) || !((int(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp58A);
# 2901
int _tmp582=_tmp581?0:((Cyc_Tcutil_bits_only(_tmp59E) && Cyc_Tcutil_is_char_type(_tmp58F)) && !
((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp58A)) && (
_tmp58E.real_const  || !_tmp59D.real_const);
int bounds_ok=((int(*)(int(*cmp)(void*,void*),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Tcutil_unify_it_bounds,_tmp59A,_tmp58B);
if(!bounds_ok  && !_tmp582){
struct _tuple0 _tmp583=({struct _tuple0 _tmp587;({void*_tmpD19=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp59A);_tmp587.f1=_tmpD19;});({void*_tmpD1A=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)(_tmp58B);_tmp587.f2=_tmpD1A;});_tmp587;});struct _tuple0 _tmp584=_tmp583;struct Cyc_Absyn_Exp*_tmp586;struct Cyc_Absyn_Exp*_tmp585;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp584.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp584.f2)->tag == 1U){_LL1C: _tmp586=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp584.f1)->f1;_tmp585=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp584.f2)->f1;_LL1D:
# 2911
 if(Cyc_Evexp_lte_const_exp(_tmp585,_tmp586))
bounds_ok=1;
goto _LL1B;}else{goto _LL1E;}}else{_LL1E: _LL1F:
# 2916
 bounds_ok=1;goto _LL1B;}_LL1B:;}{
# 2918
int t1_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp59B);
int t2_nullable=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp58C);
if(t1_nullable  && !t2_nullable)
coercion=Cyc_Absyn_Null_to_NonNull;
# 2925
if(((bounds_ok  && zeroterm_ok) && (_tmp581  || _tmp582)) && (
Cyc_Tcutil_unify(_tmp59C,_tmp58D) || Cyc_Tcenv_region_outlives(te,_tmp59C,_tmp58D)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL19: _LL1A:
 goto _LL16;}_LL16:;}
# 2931
return Cyc_Absyn_Unknown_coercion;case 8U: _LLA: _tmp5A2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57C)->f1).elt_type;_tmp5A1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57C)->f1).tq;_tmp5A0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57C)->f1).num_elts;_tmp59F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57C)->f1).zero_term;_LLB:
# 2933
{void*_tmp590=t2;void*_tmp594;struct Cyc_Absyn_Tqual _tmp593;struct Cyc_Absyn_Exp*_tmp592;union Cyc_Absyn_Constraint*_tmp591;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->tag == 8U){_LL21: _tmp594=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->f1).elt_type;_tmp593=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->f1).tq;_tmp592=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->f1).num_elts;_tmp591=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->f1).zero_term;_LL22: {
# 2935
int okay;
({int _tmpD1B=
(((_tmp5A0 != 0  && _tmp592 != 0) && ((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp59F,_tmp591)) && 
Cyc_Evexp_lte_const_exp(_tmp592,_tmp5A0)) && 
Cyc_Evexp_lte_const_exp(_tmp5A0,_tmp592);
# 2936
okay=_tmpD1B;});
# 2940
return
# 2942
(okay  && Cyc_Tcutil_unify(_tmp5A2,_tmp594)) && (!_tmp5A1.real_const  || _tmp593.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL23: _LL24:
# 2944
 return Cyc_Absyn_Unknown_coercion;}_LL20:;}
# 2946
return Cyc_Absyn_Unknown_coercion;case 13U: _LLC: _tmp5A3=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp57C)->f2;_LLD:
# 2950
{void*_tmp595=t2;struct Cyc_Absyn_Enumdecl*_tmp596;if(((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp595)->tag == 13U){_LL26: _tmp596=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp595)->f2;_LL27:
# 2952
 if((_tmp5A3->fields != 0  && _tmp596->fields != 0) && ({
int _tmpD1C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5A3->fields))->v);_tmpD1C >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp596->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL25;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}
# 2958
goto _LLF;case 6U: _LLE: _LLF:
 goto _LL11;case 7U: _LL10: _LL11:
 return Cyc_Tcutil_coerceable(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 15U: _LL12: _tmp5A4=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp57C)->f1;_LL13:
# 2963
{void*_tmp597=t2;void*_tmp598;if(((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp597)->tag == 15U){_LL2B: _tmp598=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp597)->f1;_LL2C:
# 2965
 if(Cyc_Tcenv_region_outlives(te,_tmp5A4,_tmp598))return Cyc_Absyn_No_coercion;
goto _LL2A;}else{_LL2D: _LL2E:
 goto _LL2A;}_LL2A:;}
# 2969
return Cyc_Absyn_Unknown_coercion;default: _LL14: _LL15:
 return Cyc_Absyn_Unknown_coercion;}_LL7:;};}
# 2975
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp5A5=Cyc_Absyn_copy_exp(e);
({void*_tmpD1E=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp5A6=_cycalloc(sizeof(*_tmp5A6));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpD1D=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp5A7;_tmp5A7.tag=14U;_tmp5A7.f1=t;_tmp5A7.f2=_tmp5A5;_tmp5A7.f3=0;_tmp5A7.f4=c;_tmp5A7;});_tmp5A6[0]=_tmpD1D;});_tmp5A6;});e->r=_tmpD1E;});
e->topt=t;}}
# 2983
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp5A8=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5A9=_tmp5A8;switch(*((int*)_tmp5A9)){case 6U: _LL1: _LL2:
 goto _LL4;case 13U: _LL3: _LL4:
 goto _LL6;case 14U: _LL5: _LL6:
 goto _LL8;case 19U: _LL7: _LL8:
 return 1;case 1U: _LL9: _LLA:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_typ);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 2994
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp5AA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp5AB=_tmp5AA;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AB)->tag == 7U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 3003
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp5AC=Cyc_Tcutil_compress(t);void*_tmp5AD=_tmp5AC;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5AD)->tag == 9U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 3010
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
struct _tuple0 _tmp5AE=({struct _tuple0 _tmp5B2;_tmp5B2.f1=t1;_tmp5B2.f2=t2;_tmp5B2;});struct _tuple0 _tmp5AF=_tmp5AE;int _tmp5B1;int _tmp5B0;if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AF.f1)->tag == 7U){if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AF.f2)->tag == 7U){_LL1: _tmp5B1=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AF.f1)->f1;_tmp5B0=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AF.f2)->f1;_LL2:
# 3013
 if(_tmp5B1 != 0  && _tmp5B1 != 1)return t1;else{
if(_tmp5B0 != 0  && _tmp5B0 != 1)return t2;else{
if(_tmp5B1 >= _tmp5B0)return t1;else{
return t2;}}}}else{_LL3: _LL4:
 return t1;}}else{if(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp5AF.f2)->tag == 7U){_LL5: _LL6:
 return t2;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL7: _LL8:
 goto _LLA;}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2 == Cyc_Absyn_Long_sz)goto _LLF;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL11;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL13;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL15;else{goto _LL1B;}}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2){case Cyc_Absyn_Long_sz: goto _LLF;case Cyc_Absyn_Int_sz: goto _LL13;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2){case Cyc_Absyn_Long_sz: _LLF: _LL10:
# 3023
 goto _LL12;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 3026
 goto _LL16;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL9;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL11;case Cyc_Absyn_Int_sz: goto _LL15;default: if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{goto _LL1B;}}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LLB;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLD;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL17;else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL19;else{goto _LL1B;}}}}}}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f1)->f2){case Cyc_Absyn_LongLong_sz: _LLB: _LLC:
# 3021
 goto _LLE;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 3028
 goto _LL1A;default: goto _LL1B;}}}}else{if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2){case Cyc_Absyn_LongLong_sz: _LL9: _LLA:
# 3020
 return Cyc_Absyn_ulonglong_typ;case Cyc_Absyn_Long_sz: _LL11: _LL12:
# 3024
 return Cyc_Absyn_ulong_typ;case Cyc_Absyn_Int_sz: _LL15: _LL16:
# 3027
 return Cyc_Absyn_uint_typ;default: goto _LL1B;}else{switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp5AF.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLD: _LLE:
# 3022
 return Cyc_Absyn_slonglong_typ;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 3029
 return Cyc_Absyn_slong_typ;default: goto _LL1B;}}}else{_LL1B: _LL1C:
 return Cyc_Absyn_sint_typ;}}}}_LL0:;}
# 3041 "tcutil.cyc"
static int Cyc_Tcutil_constrain_kinds(void*c1,void*c2){
({void*_tmpD1F=Cyc_Absyn_compress_kb(c1);c1=_tmpD1F;});
({void*_tmpD20=Cyc_Absyn_compress_kb(c2);c2=_tmpD20;});
if(c1 == c2)return 1;{
struct _tuple0 _tmp5B3=({struct _tuple0 _tmp5C9;_tmp5C9.f1=c1;_tmp5C9.f2=c2;_tmp5C9;});struct _tuple0 _tmp5B4=_tmp5B3;struct Cyc_Core_Opt**_tmp5C8;struct Cyc_Absyn_Kind*_tmp5C7;struct Cyc_Core_Opt**_tmp5C6;struct Cyc_Absyn_Kind*_tmp5C5;struct Cyc_Core_Opt**_tmp5C4;struct Cyc_Absyn_Kind*_tmp5C3;struct Cyc_Absyn_Kind*_tmp5C2;struct Cyc_Core_Opt**_tmp5C1;struct Cyc_Core_Opt**_tmp5C0;struct Cyc_Absyn_Kind*_tmp5BF;struct Cyc_Core_Opt**_tmp5BE;struct Cyc_Absyn_Kind*_tmp5BD;struct Cyc_Absyn_Kind*_tmp5BC;struct Cyc_Absyn_Kind*_tmp5BB;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->tag == 0U)switch(*((int*)_tmp5B4.f2)){case 0U: _LL1: _tmp5BC=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->f1;_tmp5BB=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->f1;_LL2:
 return _tmp5BC == _tmp5BB;case 1U: goto _LL3;default: _LL9: _tmp5BF=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->f1;_tmp5BE=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->f1;_tmp5BD=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->f2;_LLA:
# 3054
 if(Cyc_Tcutil_kind_leq(_tmp5BF,_tmp5BD)){
({struct Cyc_Core_Opt*_tmpD21=({struct Cyc_Core_Opt*_tmp5B8=_cycalloc(sizeof(*_tmp5B8));_tmp5B8->v=c1;_tmp5B8;});*_tmp5BE=_tmpD21;});return 1;}else{
return 0;}}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->tag == 1U){_LL3: _tmp5C0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->f1;_LL4:
# 3047
({struct Cyc_Core_Opt*_tmpD22=({struct Cyc_Core_Opt*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));_tmp5B5->v=c1;_tmp5B5;});*_tmp5C0=_tmpD22;});return 1;}else{if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->tag == 1U){_LL5: _tmp5C1=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->f1;_LL6:
({struct Cyc_Core_Opt*_tmpD23=({struct Cyc_Core_Opt*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));_tmp5B6->v=c2;_tmp5B6;});*_tmp5C1=_tmpD23;});return 1;}else{if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->tag == 0U){_LL7: _tmp5C4=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->f1;_tmp5C3=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->f2;_tmp5C2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->f1;_LL8:
# 3050
 if(Cyc_Tcutil_kind_leq(_tmp5C2,_tmp5C3)){
({struct Cyc_Core_Opt*_tmpD24=({struct Cyc_Core_Opt*_tmp5B7=_cycalloc(sizeof(*_tmp5B7));_tmp5B7->v=c2;_tmp5B7;});*_tmp5C4=_tmpD24;});return 1;}else{
return 0;}}else{_LLB: _tmp5C8=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->f1;_tmp5C7=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B4.f1)->f2;_tmp5C6=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->f1;_tmp5C5=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5B4.f2)->f2;_LLC:
# 3058
 if(Cyc_Tcutil_kind_leq(_tmp5C7,_tmp5C5)){
({struct Cyc_Core_Opt*_tmpD25=({struct Cyc_Core_Opt*_tmp5B9=_cycalloc(sizeof(*_tmp5B9));_tmp5B9->v=c1;_tmp5B9;});*_tmp5C6=_tmpD25;});return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp5C5,_tmp5C7)){
({struct Cyc_Core_Opt*_tmpD26=({struct Cyc_Core_Opt*_tmp5BA=_cycalloc(sizeof(*_tmp5BA));_tmp5BA->v=c2;_tmp5BA;});*_tmp5C8=_tmpD26;});return 1;}else{
return 0;}}}}}}_LL0:;};}
# 3067
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 3072
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp5CE;_tmp5CE.tag=1U;_tmp5CE.f1=(unsigned long)i;({void*_tmp5CC[1U]={& _tmp5CE};({struct _dyneither_ptr _tmpD27=({const char*_tmp5CD="#%d";_tag_dyneither(_tmp5CD,sizeof(char),4U);});Cyc_aprintf(_tmpD27,_tag_dyneither(_tmp5CC,sizeof(void*),1U));});});});
return({struct Cyc_Absyn_Tvar*_tmp5CA=_cycalloc(sizeof(*_tmp5CA));({struct _dyneither_ptr*_tmpD28=({struct _dyneither_ptr*_tmp5CB=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5CB[0]=(struct _dyneither_ptr)s;_tmp5CB;});_tmp5CA->name=_tmpD28;});_tmp5CA->identity=- 1;_tmp5CA->kind=k;_tmp5CA;});}
# 3079
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _dyneither_ptr _tmp5CF=*t->name;
return*((const char*)_check_dyneither_subscript(_tmp5CF,sizeof(char),0))== '#';}
# 3084
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp5D2;_tmp5D2.tag=0U;_tmp5D2.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*t->name);({void*_tmp5D0[1U]={& _tmp5D2};({struct _dyneither_ptr _tmpD29=({const char*_tmp5D1="%s";_tag_dyneither(_tmp5D1,sizeof(char),3U);});Cyc_printf(_tmpD29,_tag_dyneither(_tmp5D0,sizeof(void*),1U));});});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _dyneither_ptr _tmp5D3=({struct _dyneither_ptr _tmpD2A=({const char*_tmp5D8="`";_tag_dyneither(_tmp5D8,sizeof(char),2U);});Cyc_strconcat(_tmpD2A,(struct _dyneither_ptr)*t->name);});
({struct _dyneither_ptr _tmp5D4=_dyneither_ptr_plus(_tmp5D3,sizeof(char),1);char _tmp5D5=*((char*)_check_dyneither_subscript(_tmp5D4,sizeof(char),0U));char _tmp5D6='t';if(_get_dyneither_size(_tmp5D4,sizeof(char))== 1U  && (_tmp5D5 == '\000'  && _tmp5D6 != '\000'))_throw_arraybounds();*((char*)_tmp5D4.curr)=_tmp5D6;});
({struct _dyneither_ptr*_tmpD2B=({struct _dyneither_ptr*_tmp5D7=_cycalloc(sizeof(struct _dyneither_ptr)* 1);_tmp5D7[0]=(struct _dyneither_ptr)_tmp5D3;_tmp5D7;});t->name=_tmpD2B;});};}
# 3093
static struct _tuple10*Cyc_Tcutil_fndecl2typ_f(struct _tuple10*x){
# 3095
return({struct _tuple10*_tmp5D9=_cycalloc(sizeof(*_tmp5D9));_tmp5D9->f1=(*x).f1;_tmp5D9->f2=(*x).f2;_tmp5D9->f3=(*x).f3;_tmp5D9;});}
# 3098
void*Cyc_Tcutil_fndecl2typ(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_typ == 0){
# 3105
struct Cyc_List_List*_tmp5DA=0;
{struct Cyc_List_List*atts=fd->attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
({struct Cyc_List_List*_tmpD2C=({struct Cyc_List_List*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->hd=(void*)atts->hd;_tmp5DB->tl=_tmp5DA;_tmp5DB;});_tmp5DA=_tmpD2C;});}}
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp5DC=_cycalloc(sizeof(*_tmp5DC));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD2F=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp5DD;_tmp5DD.tag=9U;({struct Cyc_Absyn_FnInfo _tmpD2E=({struct Cyc_Absyn_FnInfo _tmp5DE;_tmp5DE.tvars=fd->tvs;_tmp5DE.effect=fd->effect;_tmp5DE.ret_tqual=fd->ret_tqual;_tmp5DE.ret_typ=fd->ret_type;({struct Cyc_List_List*_tmpD2D=
((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_fndecl2typ_f,fd->args);_tmp5DE.args=_tmpD2D;});_tmp5DE.c_varargs=fd->c_varargs;_tmp5DE.cyc_varargs=fd->cyc_varargs;_tmp5DE.rgn_po=fd->rgn_po;_tmp5DE.attributes=_tmp5DA;_tmp5DE.requires_clause=fd->requires_clause;_tmp5DE.requires_relns=fd->requires_relns;_tmp5DE.ensures_clause=fd->ensures_clause;_tmp5DE.ensures_relns=fd->ensures_relns;_tmp5DE;});_tmp5DD.f1=_tmpD2E;});_tmp5DD;});_tmp5DC[0]=_tmpD2F;});_tmp5DC;});}
# 3118
return(void*)_check_null(fd->cached_typ);}
# 3124
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3126
union Cyc_Relations_RelnOp _tmp5DF=*rop;union Cyc_Relations_RelnOp _tmp5E0=_tmp5DF;struct Cyc_Absyn_Vardecl*_tmp5F3;struct Cyc_Absyn_Vardecl*_tmp5F2;switch((_tmp5E0.RNumelts).tag){case 2U: _LL1: _tmp5F2=(_tmp5E0.RVar).val;_LL2: {
# 3128
struct _tuple2 _tmp5E1=*_tmp5F2->name;struct _tuple2 _tmp5E2=_tmp5E1;union Cyc_Absyn_Nmspace _tmp5E9;struct _dyneither_ptr*_tmp5E8;_LL8: _tmp5E9=_tmp5E2.f1;_tmp5E8=_tmp5E2.f2;_LL9:;
if(!((int)((_tmp5E9.Loc_n).tag == 4)))goto _LL0;
if(({struct _dyneither_ptr _tmpD30=(struct _dyneither_ptr)*_tmp5E8;Cyc_strcmp(_tmpD30,({const char*_tmp5E3="return_value";_tag_dyneither(_tmp5E3,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmpD31=Cyc_Relations_RReturn();*rop=_tmpD31;});
goto _LL0;}{
# 3134
unsigned int c=0U;
{struct Cyc_List_List*_tmp5E4=args;for(0;_tmp5E4 != 0;(_tmp5E4=_tmp5E4->tl,c ++)){
struct _tuple10*_tmp5E5=(struct _tuple10*)_tmp5E4->hd;struct _tuple10*_tmp5E6=_tmp5E5;struct _dyneither_ptr*_tmp5E7;_LLB: _tmp5E7=_tmp5E6->f1;_LLC:;
if(_tmp5E7 != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5E8,(struct _dyneither_ptr)*_tmp5E7)== 0){
({union Cyc_Relations_RelnOp _tmpD32=Cyc_Relations_RParam(c);*rop=_tmpD32;});
break;}}}}
# 3144
goto _LL0;};}case 3U: _LL3: _tmp5F3=(_tmp5E0.RNumelts).val;_LL4: {
# 3146
struct _tuple2 _tmp5EA=*_tmp5F3->name;struct _tuple2 _tmp5EB=_tmp5EA;union Cyc_Absyn_Nmspace _tmp5F1;struct _dyneither_ptr*_tmp5F0;_LLE: _tmp5F1=_tmp5EB.f1;_tmp5F0=_tmp5EB.f2;_LLF:;
if(!((int)((_tmp5F1.Loc_n).tag == 4)))goto _LL0;{
unsigned int c=0U;
{struct Cyc_List_List*_tmp5EC=args;for(0;_tmp5EC != 0;(_tmp5EC=_tmp5EC->tl,c ++)){
struct _tuple10*_tmp5ED=(struct _tuple10*)_tmp5EC->hd;struct _tuple10*_tmp5EE=_tmp5ED;struct _dyneither_ptr*_tmp5EF;_LL11: _tmp5EF=_tmp5EE->f1;_LL12:;
if(_tmp5EF != 0){
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp5F0,(struct _dyneither_ptr)*_tmp5EF)== 0){
({union Cyc_Relations_RelnOp _tmpD33=Cyc_Relations_RParamNumelts(c);*rop=_tmpD33;});
break;}}}}
# 3158
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3163
static void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3165
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3169
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3171
if(e == 0)return 0;{
struct Cyc_List_List*_tmp5F4=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp5F4);
return _tmp5F4;};}struct _tuple25{void*f1;struct Cyc_Absyn_Tqual f2;void*f3;};
# 3178
static void*Cyc_Tcutil_fst_fdarg(struct _tuple25*t){return(*t).f1;}
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp5F5=pr;struct Cyc_Absyn_Tqual _tmp5F8;void*_tmp5F7;_LL1: _tmp5F8=_tmp5F5->f1;_tmp5F7=_tmp5F5->f2;_LL2:;
if(_tmp5F7 == t)return pr;else{
return({struct _tuple12*_tmp5F6=_cycalloc(sizeof(*_tmp5F6));_tmp5F6->f1=_tmp5F8;_tmp5F6->f2=t;_tmp5F6;});}}struct _tuple26{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3185
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple10*y){
# 3187
return({struct _tuple27*_tmp5F9=_region_malloc(rgn,sizeof(*_tmp5F9));({struct _tuple26*_tmpD34=({struct _tuple26*_tmp5FA=_region_malloc(rgn,sizeof(*_tmp5FA));_tmp5FA->f1=(*y).f1;_tmp5FA->f2=(*y).f2;_tmp5FA;});_tmp5F9->f1=_tmpD34;});_tmp5F9->f2=(*y).f3;_tmp5F9;});}
# 3189
static struct _tuple10*Cyc_Tcutil_substitute_f2(struct _tuple10*orig_arg,void*t){
# 3191
struct _tuple10 _tmp5FB=*orig_arg;struct _tuple10 _tmp5FC=_tmp5FB;struct _dyneither_ptr*_tmp600;struct Cyc_Absyn_Tqual _tmp5FF;void*_tmp5FE;_LL1: _tmp600=_tmp5FC.f1;_tmp5FF=_tmp5FC.f2;_tmp5FE=_tmp5FC.f3;_LL2:;
if(t == _tmp5FE)return orig_arg;
return({struct _tuple10*_tmp5FD=_cycalloc(sizeof(*_tmp5FD));_tmp5FD->f1=_tmp600;_tmp5FD->f2=_tmp5FF;_tmp5FD->f3=t;_tmp5FD;});}
# 3195
static void*Cyc_Tcutil_field_type(struct Cyc_Absyn_Aggrfield*f){
return f->type;}
# 3198
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3203
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3205
return({struct Cyc_Absyn_Exp*_tmp601=_cycalloc(sizeof(*_tmp601));_tmp601->topt=old->topt;_tmp601->r=r;_tmp601->loc=old->loc;_tmp601->annot=old->annot;_tmp601;});}
# 3210
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp602=e->r;void*_tmp603=_tmp602;void*_tmp64B;void*_tmp64A;struct Cyc_List_List*_tmp649;struct Cyc_Absyn_Exp*_tmp648;void*_tmp647;void*_tmp646;struct Cyc_Absyn_Exp*_tmp645;int _tmp644;enum Cyc_Absyn_Coercion _tmp643;struct Cyc_Absyn_Exp*_tmp642;struct Cyc_Absyn_Exp*_tmp641;struct Cyc_Absyn_Exp*_tmp640;struct Cyc_Absyn_Exp*_tmp63F;struct Cyc_Absyn_Exp*_tmp63E;struct Cyc_Absyn_Exp*_tmp63D;struct Cyc_Absyn_Exp*_tmp63C;struct Cyc_Absyn_Exp*_tmp63B;struct Cyc_Absyn_Exp*_tmp63A;enum Cyc_Absyn_Primop _tmp639;struct Cyc_List_List*_tmp638;switch(*((int*)_tmp603)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp639=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_tmp638=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp603)->f2;_LLC:
# 3219
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp638)== 1){
struct Cyc_Absyn_Exp*_tmp604=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp638))->hd;
struct Cyc_Absyn_Exp*_tmp605=Cyc_Tcutil_rsubsexp(r,inst,_tmp604);
if(_tmp605 == _tmp604)return e;
return({struct Cyc_Absyn_Exp*_tmpD37=e;Cyc_Tcutil_copye(_tmpD37,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp606=_cycalloc(sizeof(*_tmp606));({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpD36=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp607;_tmp607.tag=3U;_tmp607.f1=_tmp639;({struct Cyc_List_List*_tmpD35=({struct Cyc_Absyn_Exp*_tmp608[1U];_tmp608[0U]=_tmp605;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp608,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp607.f2=_tmpD35;});_tmp607;});_tmp606[0]=_tmpD36;});_tmp606;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp638)== 2){
struct Cyc_Absyn_Exp*_tmp609=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp638))->hd;
struct Cyc_Absyn_Exp*_tmp60A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp638->tl))->hd;
struct Cyc_Absyn_Exp*_tmp60B=Cyc_Tcutil_rsubsexp(r,inst,_tmp609);
struct Cyc_Absyn_Exp*_tmp60C=Cyc_Tcutil_rsubsexp(r,inst,_tmp60A);
if(_tmp60B == _tmp609  && _tmp60C == _tmp60A)return e;
return({struct Cyc_Absyn_Exp*_tmpD3A=e;Cyc_Tcutil_copye(_tmpD3A,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp60D=_cycalloc(sizeof(*_tmp60D));({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmpD39=({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct _tmp60E;_tmp60E.tag=3U;_tmp60E.f1=_tmp639;({struct Cyc_List_List*_tmpD38=({struct Cyc_Absyn_Exp*_tmp60F[2U];_tmp60F[1U]=_tmp60C;_tmp60F[0U]=_tmp60B;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp60F,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp60E.f2=_tmpD38;});_tmp60E;});_tmp60D[0]=_tmpD39;});_tmp60D;}));});}else{
return({void*_tmp610=0U;({struct _dyneither_ptr _tmpD3B=({const char*_tmp611="primop does not have 1 or 2 args!";_tag_dyneither(_tmp611,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD3B,_tag_dyneither(_tmp610,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp63C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_tmp63B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp603)->f2;_tmp63A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp603)->f3;_LLE: {
# 3233
struct Cyc_Absyn_Exp*_tmp612=Cyc_Tcutil_rsubsexp(r,inst,_tmp63C);
struct Cyc_Absyn_Exp*_tmp613=Cyc_Tcutil_rsubsexp(r,inst,_tmp63B);
struct Cyc_Absyn_Exp*_tmp614=Cyc_Tcutil_rsubsexp(r,inst,_tmp63A);
if((_tmp612 == _tmp63C  && _tmp613 == _tmp63B) && _tmp614 == _tmp63A)return e;
return({struct Cyc_Absyn_Exp*_tmpD3D=e;Cyc_Tcutil_copye(_tmpD3D,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp615=_cycalloc(sizeof(*_tmp615));({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmpD3C=({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct _tmp616;_tmp616.tag=6U;_tmp616.f1=_tmp612;_tmp616.f2=_tmp613;_tmp616.f3=_tmp614;_tmp616;});_tmp615[0]=_tmpD3C;});_tmp615;}));});}case 7U: _LLF: _tmp63E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_tmp63D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp603)->f2;_LL10: {
# 3239
struct Cyc_Absyn_Exp*_tmp617=Cyc_Tcutil_rsubsexp(r,inst,_tmp63E);
struct Cyc_Absyn_Exp*_tmp618=Cyc_Tcutil_rsubsexp(r,inst,_tmp63D);
if(_tmp617 == _tmp63E  && _tmp618 == _tmp63D)return e;
return({struct Cyc_Absyn_Exp*_tmpD3F=e;Cyc_Tcutil_copye(_tmpD3F,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp619=_cycalloc(sizeof(*_tmp619));({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmpD3E=({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct _tmp61A;_tmp61A.tag=7U;_tmp61A.f1=_tmp617;_tmp61A.f2=_tmp618;_tmp61A;});_tmp619[0]=_tmpD3E;});_tmp619;}));});}case 8U: _LL11: _tmp640=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_tmp63F=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp603)->f2;_LL12: {
# 3244
struct Cyc_Absyn_Exp*_tmp61B=Cyc_Tcutil_rsubsexp(r,inst,_tmp640);
struct Cyc_Absyn_Exp*_tmp61C=Cyc_Tcutil_rsubsexp(r,inst,_tmp63F);
if(_tmp61B == _tmp640  && _tmp61C == _tmp63F)return e;
return({struct Cyc_Absyn_Exp*_tmpD41=e;Cyc_Tcutil_copye(_tmpD41,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp61D=_cycalloc(sizeof(*_tmp61D));({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmpD40=({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct _tmp61E;_tmp61E.tag=8U;_tmp61E.f1=_tmp61B;_tmp61E.f2=_tmp61C;_tmp61E;});_tmp61D[0]=_tmpD40;});_tmp61D;}));});}case 9U: _LL13: _tmp642=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_tmp641=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp603)->f2;_LL14: {
# 3249
struct Cyc_Absyn_Exp*_tmp61F=Cyc_Tcutil_rsubsexp(r,inst,_tmp642);
struct Cyc_Absyn_Exp*_tmp620=Cyc_Tcutil_rsubsexp(r,inst,_tmp641);
if(_tmp61F == _tmp642  && _tmp620 == _tmp641)return e;
return({struct Cyc_Absyn_Exp*_tmpD43=e;Cyc_Tcutil_copye(_tmpD43,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp621=_cycalloc(sizeof(*_tmp621));({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmpD42=({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct _tmp622;_tmp622.tag=9U;_tmp622.f1=_tmp61F;_tmp622.f2=_tmp620;_tmp622;});_tmp621[0]=_tmpD42;});_tmp621;}));});}case 14U: _LL15: _tmp646=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_tmp645=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp603)->f2;_tmp644=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp603)->f3;_tmp643=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp603)->f4;_LL16: {
# 3254
struct Cyc_Absyn_Exp*_tmp623=Cyc_Tcutil_rsubsexp(r,inst,_tmp645);
void*_tmp624=Cyc_Tcutil_rsubstitute(r,inst,_tmp646);
if(_tmp623 == _tmp645  && _tmp624 == _tmp646)return e;
return({struct Cyc_Absyn_Exp*_tmpD45=e;Cyc_Tcutil_copye(_tmpD45,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp625=_cycalloc(sizeof(*_tmp625));({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmpD44=({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct _tmp626;_tmp626.tag=14U;_tmp626.f1=_tmp624;_tmp626.f2=_tmp623;_tmp626.f3=_tmp644;_tmp626.f4=_tmp643;_tmp626;});_tmp625[0]=_tmpD44;});_tmp625;}));});}case 17U: _LL17: _tmp647=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_LL18: {
# 3259
void*_tmp627=Cyc_Tcutil_rsubstitute(r,inst,_tmp647);
if(_tmp627 == _tmp647)return e;
return({struct Cyc_Absyn_Exp*_tmpD47=e;Cyc_Tcutil_copye(_tmpD47,(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp628=_cycalloc(sizeof(*_tmp628));({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmpD46=({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct _tmp629;_tmp629.tag=17U;_tmp629.f1=_tmp627;_tmp629;});_tmp628[0]=_tmpD46;});_tmp628;}));});}case 18U: _LL19: _tmp648=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_LL1A: {
# 3263
struct Cyc_Absyn_Exp*_tmp62A=Cyc_Tcutil_rsubsexp(r,inst,_tmp648);
if(_tmp62A == _tmp648)return e;
return({struct Cyc_Absyn_Exp*_tmpD49=e;Cyc_Tcutil_copye(_tmpD49,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp62B=_cycalloc(sizeof(*_tmp62B));({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmpD48=({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct _tmp62C;_tmp62C.tag=18U;_tmp62C.f1=_tmp62A;_tmp62C;});_tmp62B[0]=_tmpD48;});_tmp62B;}));});}case 19U: _LL1B: _tmp64A=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_tmp649=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp603)->f2;_LL1C: {
# 3267
void*_tmp62D=Cyc_Tcutil_rsubstitute(r,inst,_tmp64A);
if(_tmp62D == _tmp64A)return e;
return({struct Cyc_Absyn_Exp*_tmpD4B=e;Cyc_Tcutil_copye(_tmpD4B,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp62E=_cycalloc(sizeof(*_tmp62E));({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmpD4A=({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct _tmp62F;_tmp62F.tag=19U;_tmp62F.f1=_tmp62D;_tmp62F.f2=_tmp649;_tmp62F;});_tmp62E[0]=_tmpD4A;});_tmp62E;}));});}case 39U: _LL1D: _tmp64B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp603)->f1;_LL1E: {
# 3271
void*_tmp630=Cyc_Tcutil_rsubstitute(r,inst,_tmp64B);
if(_tmp630 == _tmp64B)return e;{
# 3274
void*_tmp631=Cyc_Tcutil_compress(_tmp630);void*_tmp632=_tmp631;struct Cyc_Absyn_Exp*_tmp635;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp632)->tag == 18U){_LL22: _tmp635=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp632)->f1;_LL23:
 return _tmp635;}else{_LL24: _LL25:
# 3277
 return({struct Cyc_Absyn_Exp*_tmpD4D=e;Cyc_Tcutil_copye(_tmpD4D,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp633=_cycalloc(sizeof(*_tmp633));({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmpD4C=({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct _tmp634;_tmp634.tag=39U;_tmp634.f1=_tmp630;_tmp634;});_tmp633[0]=_tmpD4C;});_tmp633;}));});}_LL21:;};}default: _LL1F: _LL20:
# 3280
 return({void*_tmp636=0U;({struct _dyneither_ptr _tmpD4E=({const char*_tmp637="non-type-level-expression in Tcutil::rsubsexp";_tag_dyneither(_tmp637,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD4E,_tag_dyneither(_tmp636,sizeof(void*),0U));});});}_LL0:;}
# 3284
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3287
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3291
struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3294
void*_tmp64C=f->type;
struct Cyc_Absyn_Exp*_tmp64D=f->requires_clause;
void*_tmp64E=Cyc_Tcutil_rsubstitute(r,inst,_tmp64C);
struct Cyc_Absyn_Exp*_tmp64F=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp64D);
if(_tmp64C == _tmp64E  && _tmp64D == _tmp64F)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp650=_cycalloc(sizeof(*_tmp650));_tmp650->name=f->name;_tmp650->tq=f->tq;_tmp650->type=_tmp64E;_tmp650->width=f->width;_tmp650->attributes=f->attributes;_tmp650->requires_clause=_tmp64F;_tmp650;});}}
# 3304
struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3307
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp651=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp652=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp653=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp651);
struct Cyc_List_List*_tmp654=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp652);
if(_tmp653 == _tmp651  && _tmp654 == _tmp652)return fs;
# 3314
return({struct Cyc_List_List*_tmp655=_cycalloc(sizeof(*_tmp655));_tmp655->hd=_tmp653;_tmp655->tl=_tmp654;_tmp655;});};}
# 3317
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3320
struct _tuple1 _tmp656=((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple1 _tmp657=_tmp656;struct Cyc_List_List*_tmp65B;struct Cyc_List_List*_tmp65A;_LL1: _tmp65B=_tmp657.f1;_tmp65A=_tmp657.f2;_LL2:;{
struct Cyc_List_List*_tmp658=Cyc_Tcutil_substs(rgn,inst,_tmp65B);
struct Cyc_List_List*_tmp659=Cyc_Tcutil_substs(rgn,inst,_tmp65A);
if(_tmp658 == _tmp65B  && _tmp659 == _tmp65A)
return rgn_po;else{
# 3326
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp658,_tmp659);}};}
# 3329
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3332
void*_tmp65C=Cyc_Tcutil_compress(t);void*_tmp65D=_tmp65C;struct Cyc_List_List*_tmp6EE;void*_tmp6ED;void*_tmp6EC;struct Cyc_Absyn_Exp*_tmp6EB;struct Cyc_Absyn_Exp*_tmp6EA;void*_tmp6E9;void*_tmp6E8;void*_tmp6E7;void*_tmp6E6;void*_tmp6E5;enum Cyc_Absyn_AggrKind _tmp6E4;struct Cyc_List_List*_tmp6E3;struct Cyc_List_List*_tmp6E2;struct Cyc_List_List*_tmp6E1;void*_tmp6E0;struct Cyc_Absyn_Tqual _tmp6DF;void*_tmp6DE;struct Cyc_List_List*_tmp6DD;int _tmp6DC;struct Cyc_Absyn_VarargInfo*_tmp6DB;struct Cyc_List_List*_tmp6DA;struct Cyc_List_List*_tmp6D9;struct Cyc_Absyn_Exp*_tmp6D8;struct Cyc_Absyn_Exp*_tmp6D7;void*_tmp6D6;struct Cyc_Absyn_Tqual _tmp6D5;void*_tmp6D4;union Cyc_Absyn_Constraint*_tmp6D3;union Cyc_Absyn_Constraint*_tmp6D2;union Cyc_Absyn_Constraint*_tmp6D1;void*_tmp6D0;struct Cyc_Absyn_Tqual _tmp6CF;struct Cyc_Absyn_Exp*_tmp6CE;union Cyc_Absyn_Constraint*_tmp6CD;unsigned int _tmp6CC;struct _tuple2*_tmp6CB;struct Cyc_List_List*_tmp6CA;struct Cyc_Absyn_Typedefdecl*_tmp6C9;void*_tmp6C8;union Cyc_Absyn_DatatypeFieldInfoU _tmp6C7;struct Cyc_List_List*_tmp6C6;union Cyc_Absyn_DatatypeInfoU _tmp6C5;struct Cyc_List_List*_tmp6C4;union Cyc_Absyn_AggrInfoU _tmp6C3;struct Cyc_List_List*_tmp6C2;struct Cyc_Absyn_Tvar*_tmp6C1;switch(*((int*)_tmp65D)){case 2U: _LL1: _tmp6C1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp65D)->f1;_LL2: {
# 3335
struct _handler_cons _tmp65E;_push_handler(& _tmp65E);{int _tmp660=0;if(setjmp(_tmp65E.handler))_tmp660=1;if(!_tmp660){{void*_tmp661=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp6C1);_npop_handler(0U);return _tmp661;};_pop_handler();}else{void*_tmp65F=(void*)_exn_thrown;void*_tmp662=_tmp65F;void*_tmp663;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp662)->tag == Cyc_Core_Not_found){_LL3C: _LL3D:
 return t;}else{_LL3E: _tmp663=_tmp662;_LL3F:(int)_rethrow(_tmp663);}_LL3B:;}};}case 11U: _LL3: _tmp6C3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65D)->f1).aggr_info;_tmp6C2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp65D)->f1).targs;_LL4: {
# 3338
struct Cyc_List_List*_tmp664=Cyc_Tcutil_substs(rgn,inst,_tmp6C2);
return _tmp664 == _tmp6C2?t:(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp665=_cycalloc(sizeof(*_tmp665));({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmpD50=({struct Cyc_Absyn_AggrType_Absyn_Type_struct _tmp666;_tmp666.tag=11U;({struct Cyc_Absyn_AggrInfo _tmpD4F=({struct Cyc_Absyn_AggrInfo _tmp667;_tmp667.aggr_info=_tmp6C3;_tmp667.targs=_tmp664;_tmp667;});_tmp666.f1=_tmpD4F;});_tmp666;});_tmp665[0]=_tmpD50;});_tmp665;});}case 3U: _LL5: _tmp6C5=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp65D)->f1).datatype_info;_tmp6C4=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp65D)->f1).targs;_LL6: {
# 3341
struct Cyc_List_List*_tmp668=Cyc_Tcutil_substs(rgn,inst,_tmp6C4);
return _tmp668 == _tmp6C4?t:(void*)({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*_tmp669=_cycalloc(sizeof(*_tmp669));({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmpD52=({struct Cyc_Absyn_DatatypeType_Absyn_Type_struct _tmp66A;_tmp66A.tag=3U;({struct Cyc_Absyn_DatatypeInfo _tmpD51=({struct Cyc_Absyn_DatatypeInfo _tmp66B;_tmp66B.datatype_info=_tmp6C5;_tmp66B.targs=_tmp668;_tmp66B;});_tmp66A.f1=_tmpD51;});_tmp66A;});_tmp669[0]=_tmpD52;});_tmp669;});}case 4U: _LL7: _tmp6C7=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp65D)->f1).field_info;_tmp6C6=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp65D)->f1).targs;_LL8: {
# 3345
struct Cyc_List_List*_tmp66C=Cyc_Tcutil_substs(rgn,inst,_tmp6C6);
return _tmp66C == _tmp6C6?t:(void*)({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*_tmp66D=_cycalloc(sizeof(*_tmp66D));({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmpD54=({struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct _tmp66E;_tmp66E.tag=4U;({struct Cyc_Absyn_DatatypeFieldInfo _tmpD53=({struct Cyc_Absyn_DatatypeFieldInfo _tmp66F;_tmp66F.field_info=_tmp6C7;_tmp66F.targs=_tmp66C;_tmp66F;});_tmp66E.f1=_tmpD53;});_tmp66E;});_tmp66D[0]=_tmpD54;});_tmp66D;});}case 17U: _LL9: _tmp6CB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65D)->f1;_tmp6CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65D)->f2;_tmp6C9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65D)->f3;_tmp6C8=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp65D)->f4;_LLA: {
# 3349
struct Cyc_List_List*_tmp670=Cyc_Tcutil_substs(rgn,inst,_tmp6CA);
return _tmp670 == _tmp6CA?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp671=_cycalloc(sizeof(*_tmp671));({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmpD55=({struct Cyc_Absyn_TypedefType_Absyn_Type_struct _tmp672;_tmp672.tag=17U;_tmp672.f1=_tmp6CB;_tmp672.f2=_tmp670;_tmp672.f3=_tmp6C9;_tmp672.f4=_tmp6C8;_tmp672;});_tmp671[0]=_tmpD55;});_tmp671;});}case 8U: _LLB: _tmp6D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65D)->f1).elt_type;_tmp6CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65D)->f1).tq;_tmp6CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65D)->f1).num_elts;_tmp6CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65D)->f1).zero_term;_tmp6CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp65D)->f1).zt_loc;_LLC: {
# 3352
void*_tmp673=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D0);
struct Cyc_Absyn_Exp*_tmp674=_tmp6CE == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6CE);
return _tmp673 == _tmp6D0  && _tmp674 == _tmp6CE?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp675=_cycalloc(sizeof(*_tmp675));({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmpD57=({struct Cyc_Absyn_ArrayType_Absyn_Type_struct _tmp676;_tmp676.tag=8U;({struct Cyc_Absyn_ArrayInfo _tmpD56=({struct Cyc_Absyn_ArrayInfo _tmp677;_tmp677.elt_type=_tmp673;_tmp677.tq=_tmp6CF;_tmp677.num_elts=_tmp674;_tmp677.zero_term=_tmp6CD;_tmp677.zt_loc=_tmp6CC;_tmp677;});_tmp676.f1=_tmpD56;});_tmp676;});_tmp675[0]=_tmpD57;});_tmp675;});}case 5U: _LLD: _tmp6D6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65D)->f1).elt_typ;_tmp6D5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65D)->f1).elt_tq;_tmp6D4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65D)->f1).ptr_atts).rgn;_tmp6D3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65D)->f1).ptr_atts).nullable;_tmp6D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65D)->f1).ptr_atts).bounds;_tmp6D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp65D)->f1).ptr_atts).zero_term;_LLE: {
# 3357
void*_tmp678=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D6);
void*_tmp679=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6D4);
union Cyc_Absyn_Constraint*_tmp67A=_tmp6D2;
{void*_tmp67B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp6D2);void*_tmp67C=_tmp67B;struct Cyc_Absyn_Exp*_tmp680;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp67C)->tag == 1U){_LL41: _tmp680=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp67C)->f1;_LL42: {
# 3362
struct Cyc_Absyn_Exp*_tmp67D=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp680);
if(_tmp67D != _tmp680)
({union Cyc_Absyn_Constraint*_tmpD59=((union Cyc_Absyn_Constraint*(*)(void*x))Cyc_Absyn_new_conref)((void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmp67E=_cycalloc(sizeof(*_tmp67E));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpD58=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmp67F;_tmp67F.tag=1U;_tmp67F.f1=_tmp67D;_tmp67F;});_tmp67E[0]=_tmpD58;});_tmp67E;}));_tmp67A=_tmpD59;});
goto _LL40;}}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3368
if((_tmp678 == _tmp6D6  && _tmp679 == _tmp6D4) && _tmp67A == _tmp6D2)
return t;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp681=_cycalloc(sizeof(*_tmp681));({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmpD5C=({struct Cyc_Absyn_PointerType_Absyn_Type_struct _tmp682;_tmp682.tag=5U;({struct Cyc_Absyn_PtrInfo _tmpD5B=({struct Cyc_Absyn_PtrInfo _tmp683;_tmp683.elt_typ=_tmp678;_tmp683.elt_tq=_tmp6D5;({struct Cyc_Absyn_PtrAtts _tmpD5A=({(_tmp683.ptr_atts).rgn=_tmp679;(_tmp683.ptr_atts).nullable=_tmp6D3;(_tmp683.ptr_atts).bounds=_tmp67A;(_tmp683.ptr_atts).zero_term=_tmp6D1;(_tmp683.ptr_atts).ptrloc=0;_tmp683.ptr_atts;});_tmp683.ptr_atts=_tmpD5A;});_tmp683;});_tmp682.f1=_tmpD5B;});_tmp682;});_tmp681[0]=_tmpD5C;});_tmp681;});}case 9U: _LLF: _tmp6E1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).tvars;_tmp6E0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).effect;_tmp6DF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).ret_tqual;_tmp6DE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).ret_typ;_tmp6DD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).args;_tmp6DC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).c_varargs;_tmp6DB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).cyc_varargs;_tmp6DA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).rgn_po;_tmp6D9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).attributes;_tmp6D8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).requires_clause;_tmp6D7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp65D)->f1).ensures_clause;_LL10:
# 3374
{struct Cyc_List_List*_tmp684=_tmp6E1;for(0;_tmp684 != 0;_tmp684=_tmp684->tl){
({struct Cyc_List_List*_tmpD60=({struct Cyc_List_List*_tmp685=_region_malloc(rgn,sizeof(*_tmp685));({struct _tuple16*_tmpD5F=({struct _tuple16*_tmp686=_region_malloc(rgn,sizeof(*_tmp686));_tmp686->f1=(struct Cyc_Absyn_Tvar*)_tmp684->hd;({void*_tmpD5E=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp687=_cycalloc(sizeof(*_tmp687));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpD5D=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp688;_tmp688.tag=2U;_tmp688.f1=(struct Cyc_Absyn_Tvar*)_tmp684->hd;_tmp688;});_tmp687[0]=_tmpD5D;});_tmp687;});_tmp686->f2=_tmpD5E;});_tmp686;});_tmp685->hd=_tmpD5F;});_tmp685->tl=inst;_tmp685;});inst=_tmpD60;});}}{
struct _tuple1 _tmp689=({struct _RegionHandle*_tmpD62=rgn;struct _RegionHandle*_tmpD61=rgn;((struct _tuple1(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmpD62,_tmpD61,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple10*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp6DD));});
# 3376
struct _tuple1 _tmp68A=_tmp689;struct Cyc_List_List*_tmp69B;struct Cyc_List_List*_tmp69A;_LL46: _tmp69B=_tmp68A.f1;_tmp69A=_tmp68A.f2;_LL47:;{
# 3378
struct Cyc_List_List*_tmp68B=_tmp6DD;
struct Cyc_List_List*_tmp68C=Cyc_Tcutil_substs(rgn,inst,_tmp69A);
if(_tmp68C != _tmp69A)
({struct Cyc_List_List*_tmpD63=((struct Cyc_List_List*(*)(struct _tuple10*(*f)(struct _tuple10*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp6DD,_tmp68C);_tmp68B=_tmpD63;});{
void*eff2;
if(_tmp6E0 == 0)
eff2=0;else{
# 3386
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E0);
if(new_eff == _tmp6E0)
eff2=_tmp6E0;else{
# 3390
eff2=new_eff;}}{
# 3392
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp6DB == 0)
cyc_varargs2=0;else{
# 3396
struct Cyc_Absyn_VarargInfo _tmp68D=*_tmp6DB;struct Cyc_Absyn_VarargInfo _tmp68E=_tmp68D;struct _dyneither_ptr*_tmp694;struct Cyc_Absyn_Tqual _tmp693;void*_tmp692;int _tmp691;_LL49: _tmp694=_tmp68E.name;_tmp693=_tmp68E.tq;_tmp692=_tmp68E.type;_tmp691=_tmp68E.inject;_LL4A:;{
void*_tmp68F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp692);
if(_tmp68F == _tmp692)cyc_varargs2=_tmp6DB;else{
# 3400
({struct Cyc_Absyn_VarargInfo*_tmpD64=({struct Cyc_Absyn_VarargInfo*_tmp690=_cycalloc(sizeof(*_tmp690));_tmp690->name=_tmp694;_tmp690->tq=_tmp693;_tmp690->type=_tmp68F;_tmp690->inject=_tmp691;_tmp690;});cyc_varargs2=_tmpD64;});}};}{
# 3402
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp6DA);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6D8);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp6D7);
struct Cyc_List_List*_tmp695=Cyc_Tcutil_extract_relns(_tmp68B,req2);
struct Cyc_List_List*_tmp696=Cyc_Tcutil_extract_relns(_tmp68B,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp697=_cycalloc(sizeof(*_tmp697));({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmpD67=({struct Cyc_Absyn_FnType_Absyn_Type_struct _tmp698;_tmp698.tag=9U;({struct Cyc_Absyn_FnInfo _tmpD66=({struct Cyc_Absyn_FnInfo _tmp699;_tmp699.tvars=_tmp6E1;_tmp699.effect=eff2;_tmp699.ret_tqual=_tmp6DF;({void*_tmpD65=
Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6DE);_tmp699.ret_typ=_tmpD65;});_tmp699.args=_tmp68B;_tmp699.c_varargs=_tmp6DC;_tmp699.cyc_varargs=cyc_varargs2;_tmp699.rgn_po=rgn_po2;_tmp699.attributes=_tmp6D9;_tmp699.requires_clause=req2;_tmp699.requires_relns=_tmp695;_tmp699.ensures_clause=ens2;_tmp699.ensures_relns=_tmp696;_tmp699;});_tmp698.f1=_tmpD66;});_tmp698;});_tmp697[0]=_tmpD67;});_tmp697;});};};};};};case 10U: _LL11: _tmp6E2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp65D)->f1;_LL12: {
# 3412
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp69C=_tmp6E2;for(0;_tmp69C != 0;_tmp69C=_tmp69C->tl){
void*_tmp69D=(*((struct _tuple12*)_tmp69C->hd)).f2;
void*_tmp69E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp69D);
if(_tmp69D != _tmp69E)
change=1;
# 3420
({struct Cyc_List_List*_tmpD68=({struct Cyc_List_List*_tmp69F=_region_malloc(rgn,sizeof(*_tmp69F));_tmp69F->hd=_tmp69E;_tmp69F->tl=ts2;_tmp69F;});ts2=_tmpD68;});}}
# 3422
if(!change)
return t;{
struct Cyc_List_List*_tmp6A0=({struct Cyc_List_List*_tmpD69=_tmp6E2;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmpD69,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp6A1=_cycalloc(sizeof(*_tmp6A1));({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmpD6A=({struct Cyc_Absyn_TupleType_Absyn_Type_struct _tmp6A2;_tmp6A2.tag=10U;_tmp6A2.f1=_tmp6A0;_tmp6A2;});_tmp6A1[0]=_tmpD6A;});_tmp6A1;});};}case 12U: _LL13: _tmp6E4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp65D)->f1;_tmp6E3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp65D)->f2;_LL14: {
# 3427
struct Cyc_List_List*_tmp6A3=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp6E3);
if(_tmp6E3 == _tmp6A3)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp6A4=_cycalloc(sizeof(*_tmp6A4));({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmpD6B=({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct _tmp6A5;_tmp6A5.tag=12U;_tmp6A5.f1=_tmp6E4;_tmp6A5.f2=_tmp6A3;_tmp6A5;});_tmp6A4[0]=_tmpD6B;});_tmp6A4;});}case 1U: _LL15: _tmp6E5=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp65D)->f2;_LL16:
# 3431
 if(_tmp6E5 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E5);else{
return t;}case 15U: _LL17: _tmp6E6=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp65D)->f1;_LL18: {
# 3434
void*_tmp6A6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E6);
return _tmp6A6 == _tmp6E6?t:(void*)({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*_tmp6A7=_cycalloc(sizeof(*_tmp6A7));({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmpD6C=({struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct _tmp6A8;_tmp6A8.tag=15U;_tmp6A8.f1=_tmp6A6;_tmp6A8;});_tmp6A7[0]=_tmpD6C;});_tmp6A7;});}case 16U: _LL19: _tmp6E8=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp65D)->f1;_tmp6E7=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp65D)->f2;_LL1A: {
# 3437
void*_tmp6A9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E8);
void*_tmp6AA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E7);
return _tmp6A9 == _tmp6E8  && _tmp6AA == _tmp6E7?t:(void*)({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmpD6D=({struct Cyc_Absyn_DynRgnType_Absyn_Type_struct _tmp6AC;_tmp6AC.tag=16U;_tmp6AC.f1=_tmp6A9;_tmp6AC.f2=_tmp6AA;_tmp6AC;});_tmp6AB[0]=_tmpD6D;});_tmp6AB;});}case 19U: _LL1B: _tmp6E9=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp65D)->f1;_LL1C: {
# 3441
void*_tmp6AD=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6E9);
return _tmp6AD == _tmp6E9?t:(void*)({struct Cyc_Absyn_TagType_Absyn_Type_struct*_tmp6AE=_cycalloc(sizeof(*_tmp6AE));({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmpD6E=({struct Cyc_Absyn_TagType_Absyn_Type_struct _tmp6AF;_tmp6AF.tag=19U;_tmp6AF.f1=_tmp6AD;_tmp6AF;});_tmp6AE[0]=_tmpD6E;});_tmp6AE;});}case 18U: _LL1D: _tmp6EA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp65D)->f1;_LL1E: {
# 3444
struct Cyc_Absyn_Exp*_tmp6B0=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6EA);
return _tmp6B0 == _tmp6EA?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmpD6F=({struct Cyc_Absyn_ValueofType_Absyn_Type_struct _tmp6B2;_tmp6B2.tag=18U;_tmp6B2.f1=_tmp6B0;_tmp6B2;});_tmp6B1[0]=_tmpD6F;});_tmp6B1;});}case 27U: _LL1F: _tmp6EB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp65D)->f1;_LL20: {
# 3447
struct Cyc_Absyn_Exp*_tmp6B3=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp6EB);
return _tmp6B3 == _tmp6EB?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp6B4=_cycalloc(sizeof(*_tmp6B4));({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmpD70=({struct Cyc_Absyn_TypeofType_Absyn_Type_struct _tmp6B5;_tmp6B5.tag=27U;_tmp6B5.f1=_tmp6B3;_tmp6B5;});_tmp6B4[0]=_tmpD70;});_tmp6B4;});}case 13U: _LL21: _LL22:
 goto _LL24;case 14U: _LL23: _LL24:
 goto _LL26;case 0U: _LL25: _LL26:
 goto _LL28;case 6U: _LL27: _LL28:
 goto _LL2A;case 7U: _LL29: _LL2A:
 goto _LL2C;case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 28U: _LL2F: _LL30:
 goto _LL32;case 20U: _LL31: _LL32:
 return t;case 25U: _LL33: _tmp6EC=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp65D)->f1;_LL34: {
# 3459
void*_tmp6B6=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EC);
return _tmp6B6 == _tmp6EC?t:(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpD71=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp6B8;_tmp6B8.tag=25U;_tmp6B8.f1=_tmp6B6;_tmp6B8;});_tmp6B7[0]=_tmpD71;});_tmp6B7;});}case 23U: _LL35: _tmp6ED=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp65D)->f1;_LL36: {
# 3462
void*_tmp6B9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6ED);
return _tmp6B9 == _tmp6ED?t:(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpD72=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp6BB;_tmp6BB.tag=23U;_tmp6BB.f1=_tmp6B9;_tmp6BB;});_tmp6BA[0]=_tmpD72;});_tmp6BA;});}case 24U: _LL37: _tmp6EE=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp65D)->f1;_LL38: {
# 3465
struct Cyc_List_List*_tmp6BC=Cyc_Tcutil_substs(rgn,inst,_tmp6EE);
return _tmp6BC == _tmp6EE?t:(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp6BD=_cycalloc(sizeof(*_tmp6BD));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpD73=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp6BE;_tmp6BE.tag=24U;_tmp6BE.f1=_tmp6BC;_tmp6BE;});_tmp6BD[0]=_tmpD73;});_tmp6BD;});}default: _LL39: _LL3A:
({void*_tmp6BF=0U;({struct _dyneither_ptr _tmpD74=({const char*_tmp6C0="found typedecltype in rsubs";_tag_dyneither(_tmp6C0,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD74,_tag_dyneither(_tmp6BF,sizeof(void*),0U));});});}_LL0:;}
# 3471
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3474
if(ts == 0)
return 0;{
void*_tmp6EF=(void*)ts->hd;
struct Cyc_List_List*_tmp6F0=ts->tl;
void*_tmp6F1=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp6EF);
struct Cyc_List_List*_tmp6F2=Cyc_Tcutil_substs(rgn,inst,_tmp6F0);
if(_tmp6EF == _tmp6F1  && _tmp6F0 == _tmp6F2)
return ts;
return({struct Cyc_List_List*_tmp6F3=_cycalloc(sizeof(*_tmp6F3));_tmp6F3->hd=_tmp6F1;_tmp6F3->tl=_tmp6F2;_tmp6F3;});};}
# 3485
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3492
struct _tuple16*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp6F4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp6F5=_cycalloc(sizeof(*_tmp6F5));_tmp6F5->f1=tv;({void*_tmpD76=({struct Cyc_Core_Opt*_tmpD75=_tmp6F4;Cyc_Absyn_new_evar(_tmpD75,({struct Cyc_Core_Opt*_tmp6F6=_cycalloc(sizeof(*_tmp6F6));_tmp6F6->v=s;_tmp6F6;}));});_tmp6F5->f2=_tmpD76;});_tmp6F5;});}
# 3497
struct _tuple16*Cyc_Tcutil_r_make_inst_var(struct _tuple17*env,struct Cyc_Absyn_Tvar*tv){
# 3499
struct _tuple17*_tmp6F7=env;struct Cyc_List_List*_tmp6FC;struct _RegionHandle*_tmp6FB;_LL1: _tmp6FC=_tmp6F7->f1;_tmp6FB=_tmp6F7->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp6F8=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple16*_tmp6F9=_region_malloc(_tmp6FB,sizeof(*_tmp6F9));_tmp6F9->f1=tv;({void*_tmpD78=({struct Cyc_Core_Opt*_tmpD77=_tmp6F8;Cyc_Absyn_new_evar(_tmpD77,({struct Cyc_Core_Opt*_tmp6FA=_cycalloc(sizeof(*_tmp6FA));_tmp6FA->v=_tmp6FC;_tmp6FA;}));});_tmp6F9->f2=_tmpD78;});_tmp6F9;});};}
# 3509
static struct Cyc_List_List*Cyc_Tcutil_add_free_tvar(unsigned int loc,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3513
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
if(Cyc_strptrcmp(((struct Cyc_Absyn_Tvar*)tvs2->hd)->name,tv->name)== 0){
void*k1=((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind;
void*k2=tv->kind;
if(!Cyc_Tcutil_constrain_kinds(k1,k2))
({struct Cyc_String_pa_PrintArg_struct _tmp701;_tmp701.tag=0U;({struct _dyneither_ptr _tmpD79=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kindbound2string(k2));_tmp701.f1=_tmpD79;});({struct Cyc_String_pa_PrintArg_struct _tmp700;_tmp700.tag=0U;({struct _dyneither_ptr _tmpD7A=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kindbound2string(k1));_tmp700.f1=_tmpD7A;});({struct Cyc_String_pa_PrintArg_struct _tmp6FF;_tmp6FF.tag=0U;_tmp6FF.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*tv->name);({void*_tmp6FD[3U]={& _tmp6FF,& _tmp700,& _tmp701};({unsigned int _tmpD7C=loc;struct _dyneither_ptr _tmpD7B=({const char*_tmp6FE="type variable %s is used with inconsistent kinds %s and %s";_tag_dyneither(_tmp6FE,sizeof(char),59U);});Cyc_Tcutil_terr(_tmpD7C,_tmpD7B,_tag_dyneither(_tmp6FD,sizeof(void*),3U));});});});});});
if(tv->identity == - 1)
tv->identity=((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity;else{
if(tv->identity != ((struct Cyc_Absyn_Tvar*)tvs2->hd)->identity)
({void*_tmp702=0U;({struct _dyneither_ptr _tmpD7D=({const char*_tmp703="same type variable has different identity!";_tag_dyneither(_tmp703,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD7D,_tag_dyneither(_tmp702,sizeof(void*),0U));});});}
return tvs;}}}
# 3527
({int _tmpD7E=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpD7E;});
return({struct Cyc_List_List*_tmp704=_cycalloc(sizeof(*_tmp704));_tmp704->hd=tv;_tmp704->tl=tvs;_tmp704;});}
# 3533
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3535
if(tv->identity == - 1)
({void*_tmp705=0U;({struct _dyneither_ptr _tmpD7F=({const char*_tmp706="fast_add_free_tvar: bad identity in tv";_tag_dyneither(_tmp706,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD7F,_tag_dyneither(_tmp705,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3539
struct Cyc_Absyn_Tvar*_tmp707=(struct Cyc_Absyn_Tvar*)tvs2->hd;
if(_tmp707->identity == - 1)
({void*_tmp708=0U;({struct _dyneither_ptr _tmpD80=({const char*_tmp709="fast_add_free_tvar: bad identity in tvs2";_tag_dyneither(_tmp709,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD80,_tag_dyneither(_tmp708,sizeof(void*),0U));});});
if(_tmp707->identity == tv->identity)
return tvs;}}
# 3546
return({struct Cyc_List_List*_tmp70A=_cycalloc(sizeof(*_tmp70A));_tmp70A->hd=tv;_tmp70A->tl=tvs;_tmp70A;});}struct _tuple28{struct Cyc_Absyn_Tvar*f1;int f2;};
# 3552
static struct Cyc_List_List*Cyc_Tcutil_fast_add_free_tvar_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv,int b){
# 3557
if(tv->identity == - 1)
({void*_tmp70B=0U;({struct _dyneither_ptr _tmpD81=({const char*_tmp70C="fast_add_free_tvar_bool: bad identity in tv";_tag_dyneither(_tmp70C,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD81,_tag_dyneither(_tmp70B,sizeof(void*),0U));});});
{struct Cyc_List_List*tvs2=tvs;for(0;tvs2 != 0;tvs2=tvs2->tl){
# 3561
struct _tuple28*_tmp70D=(struct _tuple28*)tvs2->hd;struct _tuple28*_tmp70E=_tmp70D;struct Cyc_Absyn_Tvar*_tmp712;int*_tmp711;_LL1: _tmp712=_tmp70E->f1;_tmp711=(int*)& _tmp70E->f2;_LL2:;
if(_tmp712->identity == - 1)
({void*_tmp70F=0U;({struct _dyneither_ptr _tmpD82=({const char*_tmp710="fast_add_free_tvar_bool: bad identity in tvs2";_tag_dyneither(_tmp710,sizeof(char),46U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD82,_tag_dyneither(_tmp70F,sizeof(void*),0U));});});
if(_tmp712->identity == tv->identity){
*_tmp711=*_tmp711  || b;
return tvs;}}}
# 3569
return({struct Cyc_List_List*_tmp713=_region_malloc(r,sizeof(*_tmp713));({struct _tuple28*_tmpD83=({struct _tuple28*_tmp714=_region_malloc(r,sizeof(*_tmp714));_tmp714->f1=tv;_tmp714->f2=b;_tmp714;});_tmp713->hd=_tmpD83;});_tmp713->tl=tvs;_tmp713;});}
# 3573
static struct Cyc_List_List*Cyc_Tcutil_add_bound_tvar(struct Cyc_List_List*tvs,struct Cyc_Absyn_Tvar*tv){
# 3575
if(tv->identity == - 1)
({struct Cyc_String_pa_PrintArg_struct _tmp717;_tmp717.tag=0U;({struct _dyneither_ptr _tmpD84=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string(tv));_tmp717.f1=_tmpD84;});({void*_tmp715[1U]={& _tmp717};({struct _dyneither_ptr _tmpD85=({const char*_tmp716="bound tvar id for %s is NULL";_tag_dyneither(_tmp716,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpD85,_tag_dyneither(_tmp715,sizeof(void*),1U));});});});
return({struct Cyc_List_List*_tmp718=_cycalloc(sizeof(*_tmp718));_tmp718->hd=tv;_tmp718->tl=tvs;_tmp718;});}struct _tuple29{void*f1;int f2;};
# 3584
static struct Cyc_List_List*Cyc_Tcutil_add_free_evar(struct _RegionHandle*r,struct Cyc_List_List*es,void*e,int b){
# 3587
void*_tmp719=Cyc_Tcutil_compress(e);void*_tmp71A=_tmp719;int _tmp724;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71A)->tag == 1U){_LL1: _tmp724=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71A)->f3;_LL2:
# 3589
{struct Cyc_List_List*es2=es;for(0;es2 != 0;es2=es2->tl){
struct _tuple29*_tmp71B=(struct _tuple29*)es2->hd;struct _tuple29*_tmp71C=_tmp71B;void*_tmp721;int*_tmp720;_LL6: _tmp721=_tmp71C->f1;_tmp720=(int*)& _tmp71C->f2;_LL7:;{
void*_tmp71D=Cyc_Tcutil_compress(_tmp721);void*_tmp71E=_tmp71D;int _tmp71F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71E)->tag == 1U){_LL9: _tmp71F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp71E)->f3;_LLA:
# 3593
 if(_tmp724 == _tmp71F){
if(b != *_tmp720)*_tmp720=1;
return es;}
# 3597
goto _LL8;}else{_LLB: _LLC:
 goto _LL8;}_LL8:;};}}
# 3601
return({struct Cyc_List_List*_tmp722=_region_malloc(r,sizeof(*_tmp722));({struct _tuple29*_tmpD86=({struct _tuple29*_tmp723=_region_malloc(r,sizeof(*_tmp723));_tmp723->f1=e;_tmp723->f2=b;_tmp723;});_tmp722->hd=_tmpD86;});_tmp722->tl=es;_tmp722;});}else{_LL3: _LL4:
 return es;}_LL0:;}
# 3606
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars(struct _RegionHandle*rgn,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3609
struct Cyc_List_List*r=0;
for(0;tvs != 0;tvs=tvs->tl){
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(((struct Cyc_Absyn_Tvar*)tvs->hd)->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3618
if(!present)({struct Cyc_List_List*_tmpD87=({struct Cyc_List_List*_tmp725=_region_malloc(rgn,sizeof(*_tmp725));_tmp725->hd=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp725->tl=r;_tmp725;});r=_tmpD87;});}
# 3620
({struct Cyc_List_List*_tmpD88=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(r);r=_tmpD88;});
return r;}
# 3625
static struct Cyc_List_List*Cyc_Tcutil_remove_bound_tvars_bool(struct _RegionHandle*r,struct Cyc_List_List*tvs,struct Cyc_List_List*btvs){
# 3629
struct Cyc_List_List*res=0;
for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp726=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp727=_tmp726;struct Cyc_Absyn_Tvar*_tmp72A;int _tmp729;_LL1: _tmp72A=_tmp727.f1;_tmp729=_tmp727.f2;_LL2:;{
int present=0;
{struct Cyc_List_List*b=btvs;for(0;b != 0;b=b->tl){
if(_tmp72A->identity == ((struct Cyc_Absyn_Tvar*)b->hd)->identity){
present=1;
break;}}}
# 3639
if(!present)({struct Cyc_List_List*_tmpD89=({struct Cyc_List_List*_tmp728=_region_malloc(r,sizeof(*_tmp728));_tmp728->hd=(struct _tuple28*)tvs->hd;_tmp728->tl=res;_tmp728;});res=_tmpD89;});};}
# 3641
({struct Cyc_List_List*_tmpD8A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(res);res=_tmpD8A;});
return res;}
# 3645
void Cyc_Tcutil_check_bitfield(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*field_typ,struct Cyc_Absyn_Exp*width,struct _dyneither_ptr*fn){
# 3647
if(width != 0){
unsigned int w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp72D;_tmp72D.tag=0U;_tmp72D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp72B[1U]={& _tmp72D};({unsigned int _tmpD8C=loc;struct _dyneither_ptr _tmpD8B=({const char*_tmp72C="bitfield %s does not have constant width";_tag_dyneither(_tmp72C,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpD8C,_tmpD8B,_tag_dyneither(_tmp72B,sizeof(void*),1U));});});});else{
# 3652
struct _tuple14 _tmp72E=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple14 _tmp72F=_tmp72E;unsigned int _tmp733;int _tmp732;_LL1: _tmp733=_tmp72F.f1;_tmp732=_tmp72F.f2;_LL2:;
if(!_tmp732)
({void*_tmp730=0U;({unsigned int _tmpD8E=loc;struct _dyneither_ptr _tmpD8D=({const char*_tmp731="cannot evaluate bitfield width at compile time";_tag_dyneither(_tmp731,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpD8E,_tmpD8D,_tag_dyneither(_tmp730,sizeof(void*),0U));});});
w=_tmp733;}{
# 3657
void*_tmp734=Cyc_Tcutil_compress(field_typ);void*_tmp735=_tmp734;enum Cyc_Absyn_Size_of _tmp743;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp735)->tag == 6U){_LL4: _tmp743=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp735)->f2;_LL5:
# 3660
{enum Cyc_Absyn_Size_of _tmp736=_tmp743;switch(_tmp736){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > 8)({void*_tmp737=0U;({unsigned int _tmpD90=loc;struct _dyneither_ptr _tmpD8F=({const char*_tmp738="bitfield larger than type";_tag_dyneither(_tmp738,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpD90,_tmpD8F,_tag_dyneither(_tmp737,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > 16)({void*_tmp739=0U;({unsigned int _tmpD92=loc;struct _dyneither_ptr _tmpD91=({const char*_tmp73A="bitfield larger than type";_tag_dyneither(_tmp73A,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpD92,_tmpD91,_tag_dyneither(_tmp739,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 3665
 if(w > 32)({void*_tmp73B=0U;({unsigned int _tmpD94=loc;struct _dyneither_ptr _tmpD93=({const char*_tmp73C="bitfield larger than type";_tag_dyneither(_tmp73C,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpD94,_tmpD93,_tag_dyneither(_tmp73B,sizeof(void*),0U));});});goto _LL8;default: _LL11: _LL12:
# 3667
 if(w > 64)({void*_tmp73D=0U;({unsigned int _tmpD96=loc;struct _dyneither_ptr _tmpD95=({const char*_tmp73E="bitfield larger than type";_tag_dyneither(_tmp73E,sizeof(char),26U);});Cyc_Tcutil_warn(_tmpD96,_tmpD95,_tag_dyneither(_tmp73D,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3669
goto _LL3;}else{_LL6: _LL7:
# 3671
({struct Cyc_String_pa_PrintArg_struct _tmp742;_tmp742.tag=0U;({struct _dyneither_ptr _tmpD97=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(field_typ));_tmp742.f1=_tmpD97;});({struct Cyc_String_pa_PrintArg_struct _tmp741;_tmp741.tag=0U;_tmp741.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({void*_tmp73F[2U]={& _tmp741,& _tmp742};({unsigned int _tmpD99=loc;struct _dyneither_ptr _tmpD98=({const char*_tmp740="bitfield %s must have integral type but has type %s";_tag_dyneither(_tmp740,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpD99,_tmpD98,_tag_dyneither(_tmp73F,sizeof(void*),2U));});});});});
goto _LL3;}_LL3:;};}}
# 3678
static void Cyc_Tcutil_check_field_atts(unsigned int loc,struct _dyneither_ptr*fn,struct Cyc_List_List*atts){
for(0;atts != 0;atts=atts->tl){
void*_tmp744=(void*)atts->hd;void*_tmp745=_tmp744;switch(*((int*)_tmp745)){case 7U: _LL1: _LL2:
 continue;case 6U: _LL3: _LL4:
 continue;default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp749;_tmp749.tag=0U;_tmp749.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*fn);({struct Cyc_String_pa_PrintArg_struct _tmp748;_tmp748.tag=0U;({struct _dyneither_ptr _tmpD9A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absyn_attribute2string((void*)atts->hd));_tmp748.f1=_tmpD9A;});({void*_tmp746[2U]={& _tmp748,& _tmp749};({unsigned int _tmpD9C=loc;struct _dyneither_ptr _tmpD9B=({const char*_tmp747="bad attribute %s on member %s";_tag_dyneither(_tmp747,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpD9C,_tmpD9B,_tag_dyneither(_tmp746,sizeof(void*),2U));});});});});}_LL0:;}}struct Cyc_Tcutil_CVTEnv{struct _RegionHandle*r;struct Cyc_List_List*kind_env;struct Cyc_List_List*free_vars;struct Cyc_List_List*free_evars;int generalize_evars;int fn_result;};
# 3706
int Cyc_Tcutil_extract_const_from_typedef(unsigned int loc,int declared_const,void*t){
void*_tmp74A=t;struct Cyc_Absyn_Typedefdecl*_tmp74E;void*_tmp74D;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74A)->tag == 17U){_LL1: _tmp74E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74A)->f3;_tmp74D=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74A)->f4;_LL2:
# 3709
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp74E))->tq).real_const  || (_tmp74E->tq).print_const){
if(declared_const)({void*_tmp74B=0U;({unsigned int _tmpD9E=loc;struct _dyneither_ptr _tmpD9D=({const char*_tmp74C="extra const";_tag_dyneither(_tmp74C,sizeof(char),12U);});Cyc_Tcutil_warn(_tmpD9E,_tmpD9D,_tag_dyneither(_tmp74B,sizeof(void*),0U));});});
return 1;}
# 3714
if((unsigned int)_tmp74D)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp74D);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3721
static int Cyc_Tcutil_typedef_tvar_is_ptr_rgn(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Typedefdecl*td){
if(td != 0){
if(td->defn != 0){
void*_tmp74F=Cyc_Tcutil_compress((void*)_check_null(td->defn));void*_tmp750=_tmp74F;void*_tmp754;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp750)->tag == 5U){_LL1: _tmp754=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp750)->f1).ptr_atts).rgn;_LL2:
# 3726
{void*_tmp751=Cyc_Tcutil_compress(_tmp754);void*_tmp752=_tmp751;struct Cyc_Absyn_Tvar*_tmp753;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp752)->tag == 2U){_LL6: _tmp753=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp752)->f1;_LL7:
# 3728
 return Cyc_Absyn_tvar_cmp(tvar,_tmp753)== 0;}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 3731
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}else{
# 3737
return 1;}
return 0;}
# 3741
static struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_inst_kind(struct Cyc_Absyn_Tvar*tvar,struct Cyc_Absyn_Kind*def_kind,struct Cyc_Absyn_Kind*expected_kind,struct Cyc_Absyn_Typedefdecl*td){
# 3744
void*_tmp755=Cyc_Absyn_compress_kb(tvar->kind);void*_tmp756=_tmp755;switch(*((int*)_tmp756)){case 2U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp756)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp756)->f2)->aliasqual == Cyc_Absyn_Top){_LL1: _LL2:
 goto _LL4;}else{goto _LL5;}}else{goto _LL5;}case 0U: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp756)->f1)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp756)->f1)->aliasqual == Cyc_Absyn_Top){_LL3: _LL4:
# 3753
 if(((expected_kind->kind == Cyc_Absyn_BoxKind  || expected_kind->kind == Cyc_Absyn_MemKind) || expected_kind->kind == Cyc_Absyn_AnyKind) && 
# 3756
Cyc_Tcutil_typedef_tvar_is_ptr_rgn(tvar,td)){
if(expected_kind->aliasqual == Cyc_Absyn_Aliasable)
return& Cyc_Tcutil_rk;else{
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
return& Cyc_Tcutil_urk;}}
# 3762
return& Cyc_Tcutil_trk;}else{goto _LL5;}}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Tcutil_tvar_kind(tvar,def_kind);}_LL0:;}
# 3768
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv);struct _tuple30{struct Cyc_Tcutil_CVTEnv f1;struct Cyc_List_List*f2;};
# 3772
static struct _tuple30 Cyc_Tcutil_check_clause(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct _dyneither_ptr clause_name,struct Cyc_Absyn_Exp*clause){
# 3775
struct Cyc_List_List*relns=0;
if(clause != 0){
Cyc_Tcexp_tcExp(te,0,clause);
if(!Cyc_Tcutil_is_integral(clause))
({struct Cyc_String_pa_PrintArg_struct _tmp75A;_tmp75A.tag=0U;({struct _dyneither_ptr _tmpD9F=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(clause->topt)));_tmp75A.f1=_tmpD9F;});({struct Cyc_String_pa_PrintArg_struct _tmp759;_tmp759.tag=0U;_tmp759.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp757[2U]={& _tmp759,& _tmp75A};({unsigned int _tmpDA1=loc;struct _dyneither_ptr _tmpDA0=({const char*_tmp758="%s clause has type %s instead of integral type";_tag_dyneither(_tmp758,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpDA1,_tmpDA0,_tag_dyneither(_tmp757,sizeof(void*),2U));});});});});
({struct Cyc_Tcutil_CVTEnv _tmpDA2=Cyc_Tcutil_i_check_valid_type_level_exp(clause,te,cvtenv);cvtenv=_tmpDA2;});
({struct Cyc_List_List*_tmpDA3=Cyc_Relations_exp2relns(Cyc_Core_heap_region,clause);relns=_tmpDA3;});
if(!Cyc_Relations_consistent_relations(relns))
({struct Cyc_String_pa_PrintArg_struct _tmp75E;_tmp75E.tag=0U;({struct _dyneither_ptr _tmpDA4=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(clause));_tmp75E.f1=_tmpDA4;});({struct Cyc_String_pa_PrintArg_struct _tmp75D;_tmp75D.tag=0U;_tmp75D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)clause_name);({void*_tmp75B[2U]={& _tmp75D,& _tmp75E};({unsigned int _tmpDA6=clause->loc;struct _dyneither_ptr _tmpDA5=({const char*_tmp75C="%s clause '%s' may be unsatisfiable";_tag_dyneither(_tmp75C,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpDA6,_tmpDA5,_tag_dyneither(_tmp75B,sizeof(void*),2U));});});});});}
# 3787
return({struct _tuple30 _tmp75F;_tmp75F.f1=cvtenv;_tmp75F.f2=relns;_tmp75F;});}struct _tuple31{enum Cyc_Absyn_Format_Type f1;void*f2;};
# 3818 "tcutil.cyc"
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv,struct Cyc_Absyn_Kind*expected_kind,void*t,int put_in_effect,int allow_abs_aggr){
# 3826
{void*_tmp760=Cyc_Tcutil_compress(t);void*_tmp761=_tmp760;struct Cyc_List_List*_tmp901;void*_tmp900;void*_tmp8FF;void*_tmp8FE;void*_tmp8FD;void*_tmp8FC;struct _tuple2*_tmp8FB;struct Cyc_List_List**_tmp8FA;struct Cyc_Absyn_Typedefdecl**_tmp8F9;void**_tmp8F8;union Cyc_Absyn_AggrInfoU*_tmp8F7;struct Cyc_List_List**_tmp8F6;enum Cyc_Absyn_AggrKind _tmp8F5;struct Cyc_List_List*_tmp8F4;struct Cyc_List_List*_tmp8F3;struct Cyc_List_List**_tmp8F2;void**_tmp8F1;struct Cyc_Absyn_Tqual*_tmp8F0;void*_tmp8EF;struct Cyc_List_List*_tmp8EE;int _tmp8ED;struct Cyc_Absyn_VarargInfo*_tmp8EC;struct Cyc_List_List*_tmp8EB;struct Cyc_List_List*_tmp8EA;struct Cyc_Absyn_Exp*_tmp8E9;struct Cyc_List_List**_tmp8E8;struct Cyc_Absyn_Exp*_tmp8E7;struct Cyc_List_List**_tmp8E6;void*_tmp8E5;struct Cyc_Absyn_Tqual*_tmp8E4;struct Cyc_Absyn_Exp**_tmp8E3;union Cyc_Absyn_Constraint*_tmp8E2;unsigned int _tmp8E1;struct Cyc_Absyn_Exp*_tmp8E0;struct Cyc_Absyn_Exp*_tmp8DF;void*_tmp8DE;void*_tmp8DD;struct Cyc_Absyn_Tqual*_tmp8DC;void*_tmp8DB;union Cyc_Absyn_Constraint*_tmp8DA;union Cyc_Absyn_Constraint*_tmp8D9;union Cyc_Absyn_Constraint*_tmp8D8;union Cyc_Absyn_DatatypeFieldInfoU*_tmp8D7;struct Cyc_List_List*_tmp8D6;union Cyc_Absyn_DatatypeInfoU*_tmp8D5;struct Cyc_List_List**_tmp8D4;struct _tuple2*_tmp8D3;struct Cyc_Absyn_Enumdecl**_tmp8D2;struct Cyc_List_List*_tmp8D1;void*_tmp8D0;void***_tmp8CF;struct Cyc_Absyn_Tvar*_tmp8CE;struct Cyc_Core_Opt**_tmp8CD;void**_tmp8CC;switch(*((int*)_tmp761)){case 0U: _LL1: _LL2:
 goto _LL0;case 1U: _LL3: _tmp8CD=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp761)->f1;_tmp8CC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp761)->f2;_LL4:
# 3830
 if(*_tmp8CD == 0  || 
Cyc_Tcutil_kind_leq(expected_kind,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp8CD))->v) && !Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(*_tmp8CD))->v,expected_kind))
({struct Cyc_Core_Opt*_tmpDA7=Cyc_Tcutil_kind_to_opt(expected_kind);*_tmp8CD=_tmpDA7;});
if((cvtenv.fn_result  && cvtenv.generalize_evars) && expected_kind->kind == Cyc_Absyn_RgnKind){
# 3835
if(expected_kind->aliasqual == Cyc_Absyn_Unique)
*_tmp8CC=(void*)& Cyc_Absyn_UniqueRgn_val;else{
# 3838
*_tmp8CC=(void*)& Cyc_Absyn_HeapRgn_val;}}else{
if(cvtenv.generalize_evars){
struct Cyc_Absyn_Tvar*_tmp762=Cyc_Tcutil_new_tvar((void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp765=_cycalloc(sizeof(*_tmp765));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDA8=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp766;_tmp766.tag=2U;_tmp766.f1=0;_tmp766.f2=expected_kind;_tmp766;});_tmp765[0]=_tmpDA8;});_tmp765;}));
({void*_tmpDAA=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp763=_cycalloc(sizeof(*_tmp763));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpDA9=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp764;_tmp764.tag=2U;_tmp764.f1=_tmp762;_tmp764;});_tmp763[0]=_tmpDA9;});_tmp763;});*_tmp8CC=_tmpDAA;});
_tmp8CE=_tmp762;goto _LL6;}else{
# 3844
({struct Cyc_List_List*_tmpDAB=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,t,put_in_effect);cvtenv.free_evars=_tmpDAB;});}}
# 3846
goto _LL0;case 2U: _LL5: _tmp8CE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp761)->f1;_LL6:
# 3848
{void*_tmp767=Cyc_Absyn_compress_kb(_tmp8CE->kind);void*_tmp768=_tmp767;struct Cyc_Core_Opt**_tmp76C;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp768)->tag == 1U){_LL3C: _tmp76C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp768)->f1;_LL3D:
# 3850
({struct Cyc_Core_Opt*_tmpDAE=({struct Cyc_Core_Opt*_tmp769=_cycalloc(sizeof(*_tmp769));({void*_tmpDAD=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp76A=_cycalloc(sizeof(*_tmp76A));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDAC=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp76B;_tmp76B.tag=2U;_tmp76B.f1=0;_tmp76B.f2=expected_kind;_tmp76B;});_tmp76A[0]=_tmpDAC;});_tmp76A;});_tmp769->v=_tmpDAD;});_tmp769;});*_tmp76C=_tmpDAE;});goto _LL3B;}else{_LL3E: _LL3F:
 goto _LL3B;}_LL3B:;}
# 3855
({struct Cyc_List_List*_tmpDAF=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8CE);cvtenv.kind_env=_tmpDAF;});
# 3858
({struct Cyc_List_List*_tmpDB0=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8CE,put_in_effect);cvtenv.free_vars=_tmpDB0;});
# 3860
{void*_tmp76D=Cyc_Absyn_compress_kb(_tmp8CE->kind);void*_tmp76E=_tmp76D;struct Cyc_Core_Opt**_tmp773;struct Cyc_Absyn_Kind*_tmp772;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp76E)->tag == 2U){_LL41: _tmp773=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp76E)->f1;_tmp772=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp76E)->f2;_LL42:
# 3862
 if(Cyc_Tcutil_kind_leq(expected_kind,_tmp772))
({struct Cyc_Core_Opt*_tmpDB3=({struct Cyc_Core_Opt*_tmp76F=_cycalloc(sizeof(*_tmp76F));({void*_tmpDB2=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp770=_cycalloc(sizeof(*_tmp770));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpDB1=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp771;_tmp771.tag=2U;_tmp771.f1=0;_tmp771.f2=expected_kind;_tmp771;});_tmp770[0]=_tmpDB1;});_tmp770;});_tmp76F->v=_tmpDB2;});_tmp76F;});*_tmp773=_tmpDB3;});
goto _LL40;}else{_LL43: _LL44:
 goto _LL40;}_LL40:;}
# 3867
goto _LL0;case 26U: _LL7: _tmp8D0=(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp761)->f1)->r;_tmp8CF=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp761)->f2;_LL8: {
# 3873
void*new_t=Cyc_Tcutil_copy_type(Cyc_Tcutil_compress(t));
{void*_tmp774=_tmp8D0;struct Cyc_Absyn_Datatypedecl*_tmp777;struct Cyc_Absyn_Enumdecl*_tmp776;struct Cyc_Absyn_Aggrdecl*_tmp775;switch(*((int*)_tmp774)){case 0U: _LL46: _tmp775=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp774)->f1;_LL47:
# 3876
 if(te->in_extern_c_include)
_tmp775->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcAggrdecl(te,loc,_tmp775);goto _LL45;case 1U: _LL48: _tmp776=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp774)->f1;_LL49:
# 3880
 if(te->in_extern_c_include)
_tmp776->sc=Cyc_Absyn_ExternC;
Cyc_Tc_tcEnumdecl(te,loc,_tmp776);goto _LL45;default: _LL4A: _tmp777=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp774)->f1;_LL4B:
# 3884
 Cyc_Tc_tcDatatypedecl(te,loc,_tmp777);goto _LL45;}_LL45:;}
# 3886
({void**_tmpDB4=({void**_tmp778=_cycalloc(sizeof(*_tmp778));_tmp778[0]=new_t;_tmp778;});*_tmp8CF=_tmpDB4;});
return Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,expected_kind,new_t,put_in_effect,allow_abs_aggr);}case 14U: _LL9: _tmp8D1=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp761)->f1;_LLA: {
# 3892
struct Cyc_List_List*prev_fields=0;
unsigned int tag_count=0U;
for(0;_tmp8D1 != 0;_tmp8D1=_tmp8D1->tl){
struct Cyc_Absyn_Enumfield*_tmp779=(struct Cyc_Absyn_Enumfield*)_tmp8D1->hd;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,(*_tmp779->name).f2))
({struct Cyc_String_pa_PrintArg_struct _tmp77C;_tmp77C.tag=0U;_tmp77C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp779->name).f2);({void*_tmp77A[1U]={& _tmp77C};({unsigned int _tmpDB6=_tmp779->loc;struct _dyneither_ptr _tmpDB5=({const char*_tmp77B="duplicate enum field name %s";_tag_dyneither(_tmp77B,sizeof(char),29U);});Cyc_Tcutil_terr(_tmpDB6,_tmpDB5,_tag_dyneither(_tmp77A,sizeof(void*),1U));});});});else{
# 3899
({struct Cyc_List_List*_tmpDB7=({struct Cyc_List_List*_tmp77D=_cycalloc(sizeof(*_tmp77D));_tmp77D->hd=(*_tmp779->name).f2;_tmp77D->tl=prev_fields;_tmp77D;});prev_fields=_tmpDB7;});}
# 3901
if(_tmp779->tag == 0)
({struct Cyc_Absyn_Exp*_tmpDB8=Cyc_Absyn_uint_exp(tag_count,_tmp779->loc);_tmp779->tag=_tmpDB8;});else{
if(!Cyc_Tcutil_is_const_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp779->tag)))
({struct Cyc_String_pa_PrintArg_struct _tmp780;_tmp780.tag=0U;_tmp780.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp779->name).f2);({void*_tmp77E[1U]={& _tmp780};({unsigned int _tmpDBA=loc;struct _dyneither_ptr _tmpDB9=({const char*_tmp77F="enum field %s: expression is not constant";_tag_dyneither(_tmp77F,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpDBA,_tmpDB9,_tag_dyneither(_tmp77E,sizeof(void*),1U));});});});}{
# 3906
unsigned int t1=(Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(_tmp779->tag))).f1;
tag_count=t1 + 1;};}
# 3909
goto _LL0;}case 13U: _LLB: _tmp8D3=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp761)->f1;_tmp8D2=(struct Cyc_Absyn_Enumdecl**)&((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp761)->f2;_LLC:
# 3911
 if(*_tmp8D2 == 0  || ((struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp8D2))->fields == 0){
struct _handler_cons _tmp781;_push_handler(& _tmp781);{int _tmp783=0;if(setjmp(_tmp781.handler))_tmp783=1;if(!_tmp783){
{struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp8D3);
*_tmp8D2=*ed;}
# 3913
;_pop_handler();}else{void*_tmp782=(void*)_exn_thrown;void*_tmp784=_tmp782;void*_tmp787;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp784)->tag == Cyc_Dict_Absent){_LL4D: _LL4E: {
# 3917
struct Cyc_Absyn_Enumdecl*_tmp785=({struct Cyc_Absyn_Enumdecl*_tmp786=_cycalloc(sizeof(*_tmp786));_tmp786->sc=Cyc_Absyn_Extern;_tmp786->name=_tmp8D3;_tmp786->fields=0;_tmp786;});
Cyc_Tc_tcEnumdecl(te,loc,_tmp785);{
struct Cyc_Absyn_Enumdecl**ed=Cyc_Tcenv_lookup_enumdecl(te,loc,_tmp8D3);
*_tmp8D2=*ed;
goto _LL4C;};}}else{_LL4F: _tmp787=_tmp784;_LL50:(int)_rethrow(_tmp787);}_LL4C:;}};}{
# 3923
struct Cyc_Absyn_Enumdecl*ed=(struct Cyc_Absyn_Enumdecl*)_check_null(*_tmp8D2);
goto _LL0;};case 3U: _LLD: _tmp8D5=(union Cyc_Absyn_DatatypeInfoU*)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp761)->f1).datatype_info;_tmp8D4=(struct Cyc_List_List**)&(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp761)->f1).targs;_LLE: {
# 3926
struct Cyc_List_List*_tmp788=*_tmp8D4;
{union Cyc_Absyn_DatatypeInfoU _tmp789=*_tmp8D5;union Cyc_Absyn_DatatypeInfoU _tmp78A=_tmp789;struct Cyc_Absyn_Datatypedecl*_tmp7A2;struct _tuple2*_tmp7A1;int _tmp7A0;if((_tmp78A.UnknownDatatype).tag == 1){_LL52: _tmp7A1=((_tmp78A.UnknownDatatype).val).name;_tmp7A0=((_tmp78A.UnknownDatatype).val).is_extensible;_LL53: {
# 3929
struct Cyc_Absyn_Datatypedecl**tudp;
{struct _handler_cons _tmp78B;_push_handler(& _tmp78B);{int _tmp78D=0;if(setjmp(_tmp78B.handler))_tmp78D=1;if(!_tmp78D){({struct Cyc_Absyn_Datatypedecl**_tmpDBB=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7A1);tudp=_tmpDBB;});;_pop_handler();}else{void*_tmp78C=(void*)_exn_thrown;void*_tmp78E=_tmp78C;void*_tmp794;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp78E)->tag == Cyc_Dict_Absent){_LL57: _LL58: {
# 3933
struct Cyc_Absyn_Datatypedecl*_tmp78F=({struct Cyc_Absyn_Datatypedecl*_tmp793=_cycalloc(sizeof(*_tmp793));_tmp793->sc=Cyc_Absyn_Extern;_tmp793->name=_tmp7A1;_tmp793->tvs=0;_tmp793->fields=0;_tmp793->is_extensible=_tmp7A0;_tmp793;});
Cyc_Tc_tcDatatypedecl(te,loc,_tmp78F);
({struct Cyc_Absyn_Datatypedecl**_tmpDBC=Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7A1);tudp=_tmpDBC;});
# 3937
if(_tmp788 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp792;_tmp792.tag=0U;({struct _dyneither_ptr _tmpDBD=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A1));_tmp792.f1=_tmpDBD;});({void*_tmp790[1U]={& _tmp792};({unsigned int _tmpDBF=loc;struct _dyneither_ptr _tmpDBE=({const char*_tmp791="declare parameterized datatype %s before using";_tag_dyneither(_tmp791,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpDBF,_tmpDBE,_tag_dyneither(_tmp790,sizeof(void*),1U));});});});
return cvtenv;}
# 3942
goto _LL56;}}else{_LL59: _tmp794=_tmp78E;_LL5A:(int)_rethrow(_tmp794);}_LL56:;}};}
# 3946
if(_tmp7A0  && !(*tudp)->is_extensible)
({struct Cyc_String_pa_PrintArg_struct _tmp797;_tmp797.tag=0U;({struct _dyneither_ptr _tmpDC0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A1));_tmp797.f1=_tmpDC0;});({void*_tmp795[1U]={& _tmp797};({unsigned int _tmpDC2=loc;struct _dyneither_ptr _tmpDC1=({const char*_tmp796="datatype %s was not declared @extensible";_tag_dyneither(_tmp796,sizeof(char),41U);});Cyc_Tcutil_terr(_tmpDC2,_tmpDC1,_tag_dyneither(_tmp795,sizeof(void*),1U));});});});
({union Cyc_Absyn_DatatypeInfoU _tmpDC3=Cyc_Absyn_KnownDatatype(tudp);*_tmp8D5=_tmpDC3;});
_tmp7A2=*tudp;goto _LL55;}}else{_LL54: _tmp7A2=*(_tmp78A.KnownDatatype).val;_LL55: {
# 3953
struct Cyc_List_List*tvs=_tmp7A2->tvs;
for(0;_tmp788 != 0  && tvs != 0;(_tmp788=_tmp788->tl,tvs=tvs->tl)){
void*t=(void*)_tmp788->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 3959
{struct _tuple0 _tmp798=({struct _tuple0 _tmp79B;({void*_tmpDC4=Cyc_Absyn_compress_kb(tv->kind);_tmp79B.f1=_tmpDC4;});_tmp79B.f2=t;_tmp79B;});struct _tuple0 _tmp799=_tmp798;struct Cyc_Absyn_Tvar*_tmp79A;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp799.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp799.f2)->tag == 2U){_LL5C: _tmp79A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp799.f2)->f1;_LL5D:
# 3961
({struct Cyc_List_List*_tmpDC5=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp79A);cvtenv.kind_env=_tmpDC5;});
({struct Cyc_List_List*_tmpDC6=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp79A,1);cvtenv.free_vars=_tmpDC6;});
continue;}else{goto _LL5E;}}else{_LL5E: _LL5F:
 goto _LL5B;}_LL5B:;}{
# 3966
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDC7=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpDC7;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 3970
if(_tmp788 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp79E;_tmp79E.tag=0U;({struct _dyneither_ptr _tmpDC8=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7A2->name));_tmp79E.f1=_tmpDC8;});({void*_tmp79C[1U]={& _tmp79E};({unsigned int _tmpDCA=loc;struct _dyneither_ptr _tmpDC9=({const char*_tmp79D="too many type arguments for datatype %s";_tag_dyneither(_tmp79D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpDCA,_tmpDC9,_tag_dyneither(_tmp79C,sizeof(void*),1U));});});});
if(tvs != 0){
# 3975
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k1=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpDCB=({struct Cyc_List_List*_tmp79F=_cycalloc(sizeof(*_tmp79F));_tmp79F->hd=e;_tmp79F->tl=hidden_ts;_tmp79F;});hidden_ts=_tmpDCB;});
({struct Cyc_Tcutil_CVTEnv _tmpDCC=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k1,e,1,allow_abs_aggr);cvtenv=_tmpDCC;});}
# 3982
({struct Cyc_List_List*_tmpDCE=({struct Cyc_List_List*_tmpDCD=*_tmp8D4;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpDCD,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp8D4=_tmpDCE;});}
# 3984
goto _LL51;}}_LL51:;}
# 3986
goto _LL0;}case 4U: _LLF: _tmp8D7=(union Cyc_Absyn_DatatypeFieldInfoU*)&(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp761)->f1).field_info;_tmp8D6=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp761)->f1).targs;_LL10:
# 3989
{union Cyc_Absyn_DatatypeFieldInfoU _tmp7A3=*_tmp8D7;union Cyc_Absyn_DatatypeFieldInfoU _tmp7A4=_tmp7A3;struct Cyc_Absyn_Datatypedecl*_tmp7B8;struct Cyc_Absyn_Datatypefield*_tmp7B7;struct _tuple2*_tmp7B6;struct _tuple2*_tmp7B5;int _tmp7B4;if((_tmp7A4.UnknownDatatypefield).tag == 1){_LL61: _tmp7B6=((_tmp7A4.UnknownDatatypefield).val).datatype_name;_tmp7B5=((_tmp7A4.UnknownDatatypefield).val).field_name;_tmp7B4=((_tmp7A4.UnknownDatatypefield).val).is_extensible;_LL62: {
# 3992
struct Cyc_Absyn_Datatypedecl*tud=*Cyc_Tcenv_lookup_datatypedecl(te,loc,_tmp7B6);
struct Cyc_Absyn_Datatypefield*tuf;
# 3997
{struct Cyc_List_List*_tmp7A5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;for(0;1;_tmp7A5=_tmp7A5->tl){
if(_tmp7A5 == 0)
({void*_tmp7A6=0U;({struct _dyneither_ptr _tmpDCF=({const char*_tmp7A7="Tcutil found a bad datatypefield";_tag_dyneither(_tmp7A7,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpDCF,_tag_dyneither(_tmp7A6,sizeof(void*),0U));});});
if(Cyc_Absyn_qvar_cmp(((struct Cyc_Absyn_Datatypefield*)_tmp7A5->hd)->name,_tmp7B5)== 0){
tuf=(struct Cyc_Absyn_Datatypefield*)_tmp7A5->hd;
break;}}}
# 4005
({union Cyc_Absyn_DatatypeFieldInfoU _tmpDD0=Cyc_Absyn_KnownDatatypefield(tud,tuf);*_tmp8D7=_tmpDD0;});
_tmp7B8=tud;_tmp7B7=tuf;goto _LL64;}}else{_LL63: _tmp7B8=((_tmp7A4.KnownDatatypefield).val).f1;_tmp7B7=((_tmp7A4.KnownDatatypefield).val).f2;_LL64: {
# 4009
struct Cyc_List_List*tvs=_tmp7B8->tvs;
for(0;_tmp8D6 != 0  && tvs != 0;(_tmp8D6=_tmp8D6->tl,tvs=tvs->tl)){
void*t=(void*)_tmp8D6->hd;
struct Cyc_Absyn_Tvar*tv=(struct Cyc_Absyn_Tvar*)tvs->hd;
# 4015
{struct _tuple0 _tmp7A8=({struct _tuple0 _tmp7AB;({void*_tmpDD1=Cyc_Absyn_compress_kb(tv->kind);_tmp7AB.f1=_tmpDD1;});_tmp7AB.f2=t;_tmp7AB;});struct _tuple0 _tmp7A9=_tmp7A8;struct Cyc_Absyn_Tvar*_tmp7AA;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp7A9.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7A9.f2)->tag == 2U){_LL66: _tmp7AA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp7A9.f2)->f1;_LL67:
# 4017
({struct Cyc_List_List*_tmpDD2=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp7AA);cvtenv.kind_env=_tmpDD2;});
({struct Cyc_List_List*_tmpDD3=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp7AA,1);cvtenv.free_vars=_tmpDD3;});
continue;}else{goto _LL68;}}else{_LL68: _LL69:
 goto _LL65;}_LL65:;}{
# 4022
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpDD4=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,t,1,allow_abs_aggr);cvtenv=_tmpDD4;});
Cyc_Tcutil_check_no_qual(loc,t);};}
# 4026
if(_tmp8D6 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7AF;_tmp7AF.tag=0U;({struct _dyneither_ptr _tmpDD5=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B7->name));_tmp7AF.f1=_tmpDD5;});({struct Cyc_String_pa_PrintArg_struct _tmp7AE;_tmp7AE.tag=0U;({struct _dyneither_ptr _tmpDD6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7B8->name));_tmp7AE.f1=_tmpDD6;});({void*_tmp7AC[2U]={& _tmp7AE,& _tmp7AF};({unsigned int _tmpDD8=loc;struct _dyneither_ptr _tmpDD7=({const char*_tmp7AD="too many type arguments for datatype %s.%s";_tag_dyneither(_tmp7AD,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpDD8,_tmpDD7,_tag_dyneither(_tmp7AC,sizeof(void*),2U));});});});});
if(tvs != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp7B3;_tmp7B3.tag=0U;({struct _dyneither_ptr _tmpDD9=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp7B7->name));_tmp7B3.f1=_tmpDD9;});({struct Cyc_String_pa_PrintArg_struct _tmp7B2;_tmp7B2.tag=0U;({struct _dyneither_ptr _tmpDDA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp7B8->name));_tmp7B2.f1=_tmpDDA;});({void*_tmp7B0[2U]={& _tmp7B2,& _tmp7B3};({unsigned int _tmpDDC=loc;struct _dyneither_ptr _tmpDDB=({const char*_tmp7B1="too few type arguments for datatype %s.%s";_tag_dyneither(_tmp7B1,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpDDC,_tmpDDB,_tag_dyneither(_tmp7B0,sizeof(void*),2U));});});});});
goto _LL60;}}_LL60:;}
# 4034
goto _LL0;case 5U: _LL11: _tmp8DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).elt_typ;_tmp8DC=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).elt_tq;_tmp8DB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).ptr_atts).rgn;_tmp8DA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).ptr_atts).nullable;_tmp8D9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).ptr_atts).bounds;_tmp8D8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp761)->f1).ptr_atts).zero_term;_LL12: {
# 4037
int is_zero_terminated;
# 4039
({struct Cyc_Tcutil_CVTEnv _tmpDDD=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp8DD,1,1);cvtenv=_tmpDDD;});
({int _tmpDDE=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8DC->print_const,_tmp8DD);_tmp8DC->real_const=_tmpDDE;});{
struct Cyc_Absyn_Kind*k;
{enum Cyc_Absyn_AliasQual _tmp7B9=expected_kind->aliasqual;enum Cyc_Absyn_AliasQual _tmp7BA=_tmp7B9;switch(_tmp7BA){case Cyc_Absyn_Aliasable: _LL6B: _LL6C:
 k=& Cyc_Tcutil_rk;goto _LL6A;case Cyc_Absyn_Unique: _LL6D: _LL6E:
 k=& Cyc_Tcutil_urk;goto _LL6A;default: _LL6F: _LL70:
 k=& Cyc_Tcutil_trk;goto _LL6A;}_LL6A:;}
# 4047
({struct Cyc_Tcutil_CVTEnv _tmpDDF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,_tmp8DB,1,1);cvtenv=_tmpDDF;});
{union Cyc_Absyn_Constraint*_tmp7BB=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp8D8);union Cyc_Absyn_Constraint*_tmp7BC=_tmp7BB;int _tmp7C2;switch((((union Cyc_Absyn_Constraint*)_tmp7BC)->Eq_constr).tag){case 3U: _LL72: _LL73:
# 4052
{void*_tmp7BD=Cyc_Tcutil_compress(_tmp8DD);void*_tmp7BE=_tmp7BD;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7BE)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp7BE)->f2 == Cyc_Absyn_Char_sz){_LL79: _LL7A:
# 4054
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D8,Cyc_Absyn_true_conref);
is_zero_terminated=1;
goto _LL78;}else{goto _LL7B;}}else{_LL7B: _LL7C:
# 4058
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8D8,Cyc_Absyn_false_conref);
is_zero_terminated=0;
goto _LL78;}_LL78:;}
# 4062
goto _LL71;case 1U: _LL74: _tmp7C2=(int)(_tmp7BC->Eq_constr).val;if(_tmp7C2){_LL75:
# 4065
 if(!Cyc_Tcutil_admits_zero(_tmp8DD))
({struct Cyc_String_pa_PrintArg_struct _tmp7C1;_tmp7C1.tag=0U;({struct _dyneither_ptr _tmpDE0=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8DD));_tmp7C1.f1=_tmpDE0;});({void*_tmp7BF[1U]={& _tmp7C1};({unsigned int _tmpDE2=loc;struct _dyneither_ptr _tmpDE1=({const char*_tmp7C0="cannot have a pointer to zero-terminated %s elements";_tag_dyneither(_tmp7C0,sizeof(char),53U);});Cyc_Tcutil_terr(_tmpDE2,_tmpDE1,_tag_dyneither(_tmp7BF,sizeof(void*),1U));});});});
is_zero_terminated=1;
goto _LL71;}else{goto _LL76;}default: _LL76: _LL77:
# 4071
 is_zero_terminated=0;
goto _LL71;}_LL71:;}
# 4075
{void*_tmp7C3=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp8D9);void*_tmp7C4=_tmp7C3;struct Cyc_Absyn_Exp*_tmp7CD;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp7C4)->tag == 0U){_LL7E: _LL7F:
 goto _LL7D;}else{_LL80: _tmp7CD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp7C4)->f1;_LL81:
# 4078
({struct Cyc_Tcenv_Tenv*_tmpDE3=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDE3,0,_tmp7CD);});
({struct Cyc_Tcutil_CVTEnv _tmpDE4=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7CD,te,cvtenv);cvtenv=_tmpDE4;});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7CD))
({void*_tmp7C5=0U;({unsigned int _tmpDE6=loc;struct _dyneither_ptr _tmpDE5=({const char*_tmp7C6="pointer bounds expression is not an unsigned int";_tag_dyneither(_tmp7C6,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpDE6,_tmpDE5,_tag_dyneither(_tmp7C5,sizeof(void*),0U));});});{
struct _tuple14 _tmp7C7=Cyc_Evexp_eval_const_uint_exp(_tmp7CD);struct _tuple14 _tmp7C8=_tmp7C7;unsigned int _tmp7CC;int _tmp7CB;_LL83: _tmp7CC=_tmp7C8.f1;_tmp7CB=_tmp7C8.f2;_LL84:;
if(is_zero_terminated  && (!_tmp7CB  || _tmp7CC < 1))
({void*_tmp7C9=0U;({unsigned int _tmpDE8=loc;struct _dyneither_ptr _tmpDE7=({const char*_tmp7CA="zero-terminated pointer cannot point to empty sequence";_tag_dyneither(_tmp7CA,sizeof(char),55U);});Cyc_Tcutil_terr(_tmpDE8,_tmpDE7,_tag_dyneither(_tmp7C9,sizeof(void*),0U));});});
goto _LL7D;};}_LL7D:;}
# 4087
goto _LL0;};}case 19U: _LL13: _tmp8DE=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmp761)->f1;_LL14:
# 4089
({struct Cyc_Tcutil_CVTEnv _tmpDE9=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp8DE,1,1);cvtenv=_tmpDE9;});goto _LL0;case 28U: _LL15: _LL16:
 goto _LL0;case 18U: _LL17: _tmp8DF=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp761)->f1;_LL18:
# 4095
({struct Cyc_Tcenv_Tenv*_tmpDEA=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDEA,0,_tmp8DF);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp8DF))
({void*_tmp7CE=0U;({unsigned int _tmpDEC=loc;struct _dyneither_ptr _tmpDEB=({const char*_tmp7CF="valueof_t requires an int expression";_tag_dyneither(_tmp7CF,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpDEC,_tmpDEB,_tag_dyneither(_tmp7CE,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpDED=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp8DF,te,cvtenv);cvtenv=_tmpDED;});
goto _LL0;case 27U: _LL19: _tmp8E0=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp761)->f1;_LL1A:
# 4104
({struct Cyc_Tcenv_Tenv*_tmpDEE=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDEE,0,_tmp8E0);});
goto _LL0;case 6U: _LL1B: _LL1C:
 goto _LL1E;case 7U: _LL1D: _LL1E:
 goto _LL0;case 8U: _LL1F: _tmp8E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp761)->f1).elt_type;_tmp8E4=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp761)->f1).tq;_tmp8E3=(struct Cyc_Absyn_Exp**)&(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp761)->f1).num_elts;_tmp8E2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp761)->f1).zero_term;_tmp8E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp761)->f1).zt_loc;_LL20: {
# 4111
struct Cyc_Absyn_Exp*_tmp7D0=*_tmp8E3;
({struct Cyc_Tcutil_CVTEnv _tmpDEF=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp8E5,1,allow_abs_aggr);cvtenv=_tmpDEF;});
({int _tmpDF0=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8E4->print_const,_tmp8E5);_tmp8E4->real_const=_tmpDF0;});{
int is_zero_terminated;
{union Cyc_Absyn_Constraint*_tmp7D1=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(_tmp8E2);union Cyc_Absyn_Constraint*_tmp7D2=_tmp7D1;int _tmp7D6;switch((((union Cyc_Absyn_Constraint*)_tmp7D2)->Eq_constr).tag){case 3U: _LL86: _LL87:
# 4118
((int(*)(int(*cmp)(int,int),union Cyc_Absyn_Constraint*x,union Cyc_Absyn_Constraint*y))Cyc_Tcutil_unify_conrefs)(Cyc_Core_intcmp,_tmp8E2,Cyc_Absyn_false_conref);
is_zero_terminated=0;
# 4133 "tcutil.cyc"
goto _LL85;case 1U: _LL88: _tmp7D6=(int)(_tmp7D2->Eq_constr).val;if(_tmp7D6){_LL89:
# 4136
 if(!Cyc_Tcutil_admits_zero(_tmp8E5))
({struct Cyc_String_pa_PrintArg_struct _tmp7D5;_tmp7D5.tag=0U;({struct _dyneither_ptr _tmpDF1=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(_tmp8E5));_tmp7D5.f1=_tmpDF1;});({void*_tmp7D3[1U]={& _tmp7D5};({unsigned int _tmpDF3=loc;struct _dyneither_ptr _tmpDF2=({const char*_tmp7D4="cannot have a zero-terminated array of %s elements";_tag_dyneither(_tmp7D4,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpDF3,_tmpDF2,_tag_dyneither(_tmp7D3,sizeof(void*),1U));});});});
is_zero_terminated=1;
goto _LL85;}else{goto _LL8A;}default: _LL8A: _LL8B:
# 4142
 is_zero_terminated=0;
goto _LL85;}_LL85:;}
# 4147
if(_tmp7D0 == 0){
# 4149
if(is_zero_terminated)
# 4151
({struct Cyc_Absyn_Exp*_tmpDF5=({struct Cyc_Absyn_Exp*_tmpDF4=Cyc_Absyn_uint_exp(1U,0U);_tmp7D0=_tmpDF4;});*_tmp8E3=_tmpDF5;});else{
# 4154
({void*_tmp7D7=0U;({unsigned int _tmpDF7=loc;struct _dyneither_ptr _tmpDF6=({const char*_tmp7D8="array bound defaults to 1 here";_tag_dyneither(_tmp7D8,sizeof(char),31U);});Cyc_Tcutil_warn(_tmpDF7,_tmpDF6,_tag_dyneither(_tmp7D7,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpDF9=({struct Cyc_Absyn_Exp*_tmpDF8=Cyc_Absyn_uint_exp(1U,0U);_tmp7D0=_tmpDF8;});*_tmp8E3=_tmpDF9;});}}
# 4158
({struct Cyc_Tcenv_Tenv*_tmpDFA=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpDFA,0,_tmp7D0);});
if(!Cyc_Tcutil_coerce_uint_typ(te,_tmp7D0))
({void*_tmp7D9=0U;({unsigned int _tmpDFC=loc;struct _dyneither_ptr _tmpDFB=({const char*_tmp7DA="array bounds expression is not an unsigned int";_tag_dyneither(_tmp7DA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpDFC,_tmpDFB,_tag_dyneither(_tmp7D9,sizeof(void*),0U));});});
({struct Cyc_Tcutil_CVTEnv _tmpDFD=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp7D0,te,cvtenv);cvtenv=_tmpDFD;});{
# 4166
struct _tuple14 _tmp7DB=Cyc_Evexp_eval_const_uint_exp(_tmp7D0);struct _tuple14 _tmp7DC=_tmp7DB;unsigned int _tmp7E2;int _tmp7E1;_LL8D: _tmp7E2=_tmp7DC.f1;_tmp7E1=_tmp7DC.f2;_LL8E:;
# 4168
if((is_zero_terminated  && _tmp7E1) && _tmp7E2 < 1)
({void*_tmp7DD=0U;({unsigned int _tmpDFF=loc;struct _dyneither_ptr _tmpDFE=({const char*_tmp7DE="zero terminated array cannot have zero elements";_tag_dyneither(_tmp7DE,sizeof(char),48U);});Cyc_Tcutil_warn(_tmpDFF,_tmpDFE,_tag_dyneither(_tmp7DD,sizeof(void*),0U));});});
# 4171
if((_tmp7E1  && _tmp7E2 < 1) && Cyc_Cyclone_tovc_r){
({void*_tmp7DF=0U;({unsigned int _tmpE01=loc;struct _dyneither_ptr _tmpE00=({const char*_tmp7E0="arrays with 0 elements are not supported except with gcc -- changing to 1.";_tag_dyneither(_tmp7E0,sizeof(char),75U);});Cyc_Tcutil_warn(_tmpE01,_tmpE00,_tag_dyneither(_tmp7DF,sizeof(void*),0U));});});
({struct Cyc_Absyn_Exp*_tmpE02=Cyc_Absyn_uint_exp(1U,0U);*_tmp8E3=_tmpE02;});}
# 4175
goto _LL0;};};}case 9U: _LL21: _tmp8F2=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).tvars;_tmp8F1=(void**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).effect;_tmp8F0=(struct Cyc_Absyn_Tqual*)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).ret_tqual;_tmp8EF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).ret_typ;_tmp8EE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).args;_tmp8ED=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).c_varargs;_tmp8EC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).cyc_varargs;_tmp8EB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).rgn_po;_tmp8EA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).attributes;_tmp8E9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).requires_clause;_tmp8E8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).requires_relns;_tmp8E7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).ensures_clause;_tmp8E6=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp761)->f1).ensures_relns;_LL22: {
# 4182
int num_convs=0;
int seen_cdecl=0;
int seen_stdcall=0;
int seen_fastcall=0;
int seen_format=0;
enum Cyc_Absyn_Format_Type ft=Cyc_Absyn_Printf_ft;
int fmt_desc_arg=-1;
int fmt_arg_start=-1;
for(0;_tmp8EA != 0;_tmp8EA=_tmp8EA->tl){
if(!Cyc_Absyn_fntype_att((void*)_tmp8EA->hd))
({struct Cyc_String_pa_PrintArg_struct _tmp7E5;_tmp7E5.tag=0U;({struct _dyneither_ptr _tmpE03=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absyn_attribute2string((void*)_tmp8EA->hd));_tmp7E5.f1=_tmpE03;});({void*_tmp7E3[1U]={& _tmp7E5};({unsigned int _tmpE05=loc;struct _dyneither_ptr _tmpE04=({const char*_tmp7E4="bad function type attribute %s";_tag_dyneither(_tmp7E4,sizeof(char),31U);});Cyc_Tcutil_terr(_tmpE05,_tmpE04,_tag_dyneither(_tmp7E3,sizeof(void*),1U));});});});{
void*_tmp7E6=(void*)_tmp8EA->hd;void*_tmp7E7=_tmp7E6;enum Cyc_Absyn_Format_Type _tmp7EC;int _tmp7EB;int _tmp7EA;switch(*((int*)_tmp7E7)){case 1U: _LL90: _LL91:
# 4195
 if(!seen_stdcall){seen_stdcall=1;++ num_convs;}goto _LL8F;case 2U: _LL92: _LL93:
# 4197
 if(!seen_cdecl){seen_cdecl=1;++ num_convs;}goto _LL8F;case 3U: _LL94: _LL95:
# 4199
 if(!seen_fastcall){seen_fastcall=1;++ num_convs;}goto _LL8F;case 19U: _LL96: _tmp7EC=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7E7)->f1;_tmp7EB=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7E7)->f2;_tmp7EA=((struct Cyc_Absyn_Format_att_Absyn_Attribute_struct*)_tmp7E7)->f3;_LL97:
# 4201
 if(!seen_format){
seen_format=1;
ft=_tmp7EC;
fmt_desc_arg=_tmp7EB;
fmt_arg_start=_tmp7EA;}else{
# 4207
({void*_tmp7E8=0U;({unsigned int _tmpE07=loc;struct _dyneither_ptr _tmpE06=({const char*_tmp7E9="function can't have multiple format attributes";_tag_dyneither(_tmp7E9,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpE07,_tmpE06,_tag_dyneither(_tmp7E8,sizeof(void*),0U));});});}
goto _LL8F;default: _LL98: _LL99:
 goto _LL8F;}_LL8F:;};}
# 4212
if(num_convs > 1)
({void*_tmp7ED=0U;({unsigned int _tmpE09=loc;struct _dyneither_ptr _tmpE08=({const char*_tmp7EE="function can't have multiple calling conventions";_tag_dyneither(_tmp7EE,sizeof(char),49U);});Cyc_Tcutil_terr(_tmpE09,_tmpE08,_tag_dyneither(_tmp7ED,sizeof(void*),0U));});});
# 4217
Cyc_Tcutil_check_unique_tvars(loc,*_tmp8F2);
{struct Cyc_List_List*b=*_tmp8F2;for(0;b != 0;b=b->tl){
({int _tmpE0A=Cyc_Tcutil_new_tvar_id();((struct Cyc_Absyn_Tvar*)b->hd)->identity=_tmpE0A;});
({struct Cyc_List_List*_tmpE0B=Cyc_Tcutil_add_bound_tvar(cvtenv.kind_env,(struct Cyc_Absyn_Tvar*)b->hd);cvtenv.kind_env=_tmpE0B;});{
void*_tmp7EF=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)b->hd)->kind);void*_tmp7F0=_tmp7EF;if(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F0)->tag == 0U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp7F0)->f1)->kind == Cyc_Absyn_MemKind){_LL9B: _LL9C:
# 4223
({struct Cyc_String_pa_PrintArg_struct _tmp7F3;_tmp7F3.tag=0U;_tmp7F3.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)b->hd)->name);({void*_tmp7F1[1U]={& _tmp7F3};({unsigned int _tmpE0D=loc;struct _dyneither_ptr _tmpE0C=({const char*_tmp7F2="function attempts to abstract Mem type variable %s";_tag_dyneither(_tmp7F2,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpE0D,_tmpE0C,_tag_dyneither(_tmp7F1,sizeof(void*),1U));});});});
goto _LL9A;}else{goto _LL9D;}}else{_LL9D: _LL9E:
 goto _LL9A;}_LL9A:;};}}{
# 4231
struct Cyc_Tcutil_CVTEnv _tmp7F4=({struct Cyc_Tcutil_CVTEnv _tmp882;_tmp882.r=Cyc_Core_heap_region;_tmp882.kind_env=cvtenv.kind_env;_tmp882.free_vars=0;_tmp882.free_evars=0;_tmp882.generalize_evars=cvtenv.generalize_evars;_tmp882.fn_result=1;_tmp882;});
# 4236
({struct Cyc_Tcutil_CVTEnv _tmpE0E=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F4,& Cyc_Tcutil_tmk,_tmp8EF,1,1);_tmp7F4=_tmpE0E;});
({int _tmpE0F=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp8F0->print_const,_tmp8EF);_tmp8F0->real_const=_tmpE0F;});
_tmp7F4.fn_result=0;
{struct Cyc_List_List*a=_tmp8EE;for(0;a != 0;a=a->tl){
struct _tuple10*_tmp7F5=(struct _tuple10*)a->hd;
void*_tmp7F6=(*_tmp7F5).f3;
({struct Cyc_Tcutil_CVTEnv _tmpE10=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F4,& Cyc_Tcutil_tmk,_tmp7F6,1,1);_tmp7F4=_tmpE10;});{
int _tmp7F7=Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp7F5).f2).print_const,_tmp7F6);
((*_tmp7F5).f2).real_const=_tmp7F7;
# 4247
if(Cyc_Tcutil_is_array(_tmp7F6)){
# 4249
void*_tmp7F8=Cyc_Absyn_new_evar(0,0);
({struct Cyc_Tcutil_CVTEnv _tmpE11=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F4,& Cyc_Tcutil_rk,_tmp7F8,1,1);_tmp7F4=_tmpE11;});
({void*_tmpE12=Cyc_Tcutil_promote_array(_tmp7F6,_tmp7F8,0);(*_tmp7F5).f3=_tmpE12;});}};}}
# 4256
if(_tmp8EC != 0){
if(_tmp8ED)({void*_tmp7F9=0U;({struct _dyneither_ptr _tmpE13=({const char*_tmp7FA="both c_vararg and cyc_vararg";_tag_dyneither(_tmp7FA,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpE13,_tag_dyneither(_tmp7F9,sizeof(void*),0U));});});{
struct Cyc_Absyn_VarargInfo _tmp7FB=*_tmp8EC;struct Cyc_Absyn_VarargInfo _tmp7FC=_tmp7FB;void*_tmp80F;int _tmp80E;_LLA0: _tmp80F=_tmp7FC.type;_tmp80E=_tmp7FC.inject;_LLA1:;
({struct Cyc_Tcutil_CVTEnv _tmpE14=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F4,& Cyc_Tcutil_tmk,_tmp80F,1,1);_tmp7F4=_tmpE14;});
({int _tmpE15=Cyc_Tcutil_extract_const_from_typedef(loc,(_tmp8EC->tq).print_const,_tmp80F);(_tmp8EC->tq).real_const=_tmpE15;});
# 4262
if(_tmp80E){
void*_tmp7FD=Cyc_Tcutil_compress(_tmp80F);void*_tmp7FE=_tmp7FD;void*_tmp80D;union Cyc_Absyn_Constraint*_tmp80C;union Cyc_Absyn_Constraint*_tmp80B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FE)->tag == 5U){_LLA3: _tmp80D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FE)->f1).elt_typ;_tmp80C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FE)->f1).ptr_atts).bounds;_tmp80B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7FE)->f1).ptr_atts).zero_term;_LLA4:
# 4265
{void*_tmp7FF=Cyc_Tcutil_compress(_tmp80D);void*_tmp800=_tmp7FF;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp800)->tag == 3U){_LLA8: _LLA9:
# 4267
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(0,_tmp80B))
({void*_tmp801=0U;({unsigned int _tmpE17=loc;struct _dyneither_ptr _tmpE16=({const char*_tmp802="can't inject into a zeroterm pointer";_tag_dyneither(_tmp802,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpE17,_tmpE16,_tag_dyneither(_tmp801,sizeof(void*),0U));});});
{void*_tmp803=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,_tmp80C);void*_tmp804=_tmp803;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp804)->tag == 0U){_LLAD: _LLAE:
# 4271
({void*_tmp805=0U;({unsigned int _tmpE19=loc;struct _dyneither_ptr _tmpE18=({const char*_tmp806="can't inject into a fat pointer to datatype";_tag_dyneither(_tmp806,sizeof(char),44U);});Cyc_Tcutil_terr(_tmpE19,_tmpE18,_tag_dyneither(_tmp805,sizeof(void*),0U));});});
goto _LLAC;}else{_LLAF: _LLB0:
 goto _LLAC;}_LLAC:;}
# 4275
goto _LLA7;}else{_LLAA: _LLAB:
({void*_tmp807=0U;({unsigned int _tmpE1B=loc;struct _dyneither_ptr _tmpE1A=({const char*_tmp808="can't inject a non-datatype type";_tag_dyneither(_tmp808,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpE1B,_tmpE1A,_tag_dyneither(_tmp807,sizeof(void*),0U));});});goto _LLA7;}_LLA7:;}
# 4278
goto _LLA2;}else{_LLA5: _LLA6:
({void*_tmp809=0U;({unsigned int _tmpE1D=loc;struct _dyneither_ptr _tmpE1C=({const char*_tmp80A="expecting a datatype pointer type";_tag_dyneither(_tmp80A,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpE1D,_tmpE1C,_tag_dyneither(_tmp809,sizeof(void*),0U));});});goto _LLA2;}_LLA2:;}};}
# 4284
if(seen_format){
int _tmp810=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp8EE);
if((((fmt_desc_arg < 0  || fmt_desc_arg > _tmp810) || fmt_arg_start < 0) || 
# 4288
(_tmp8EC == 0  && !_tmp8ED) && fmt_arg_start != 0) || 
(_tmp8EC != 0  || _tmp8ED) && fmt_arg_start != _tmp810 + 1)
# 4291
({void*_tmp811=0U;({unsigned int _tmpE1F=loc;struct _dyneither_ptr _tmpE1E=({const char*_tmp812="bad format descriptor";_tag_dyneither(_tmp812,sizeof(char),22U);});Cyc_Tcutil_terr(_tmpE1F,_tmpE1E,_tag_dyneither(_tmp811,sizeof(void*),0U));});});else{
# 4294
struct _tuple10 _tmp813=*((struct _tuple10*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp8EE,fmt_desc_arg - 1);struct _tuple10 _tmp814=_tmp813;void*_tmp82C;_LLB2: _tmp82C=_tmp814.f3;_LLB3:;
# 4296
{void*_tmp815=Cyc_Tcutil_compress(_tmp82C);void*_tmp816=_tmp815;void*_tmp824;union Cyc_Absyn_Constraint*_tmp823;union Cyc_Absyn_Constraint*_tmp822;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp816)->tag == 5U){_LLB5: _tmp824=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp816)->f1).elt_typ;_tmp823=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp816)->f1).ptr_atts).bounds;_tmp822=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp816)->f1).ptr_atts).zero_term;_LLB6:
# 4299
{struct _tuple0 _tmp817=({struct _tuple0 _tmp81F;({void*_tmpE20=Cyc_Tcutil_compress(_tmp824);_tmp81F.f1=_tmpE20;});({void*_tmpE21=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp823);_tmp81F.f2=_tmpE21;});_tmp81F;});struct _tuple0 _tmp818=_tmp817;if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp818.f1)->tag == 6U){if(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp818.f1)->f2 == Cyc_Absyn_Char_sz){if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp818.f2)->tag == 0U){_LLBA: _LLBB:
# 4301
 if(_tmp8ED)
({void*_tmp819=0U;({unsigned int _tmpE23=loc;struct _dyneither_ptr _tmpE22=({const char*_tmp81A="format descriptor is not a char * type";_tag_dyneither(_tmp81A,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE23,_tmpE22,_tag_dyneither(_tmp819,sizeof(void*),0U));});});
goto _LLB9;}else{_LLBC: _LLBD:
# 4305
 if(!_tmp8ED)
({void*_tmp81B=0U;({unsigned int _tmpE25=loc;struct _dyneither_ptr _tmpE24=({const char*_tmp81C="format descriptor is not a char ? type";_tag_dyneither(_tmp81C,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE25,_tmpE24,_tag_dyneither(_tmp81B,sizeof(void*),0U));});});
goto _LLB9;}}else{goto _LLBE;}}else{_LLBE: _LLBF:
# 4309
({void*_tmp81D=0U;({unsigned int _tmpE27=loc;struct _dyneither_ptr _tmpE26=({const char*_tmp81E="format descriptor is not a string type";_tag_dyneither(_tmp81E,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE27,_tmpE26,_tag_dyneither(_tmp81D,sizeof(void*),0U));});});
goto _LLB9;}_LLB9:;}
# 4312
goto _LLB4;}else{_LLB7: _LLB8:
({void*_tmp820=0U;({unsigned int _tmpE29=loc;struct _dyneither_ptr _tmpE28=({const char*_tmp821="format descriptor is not a string type";_tag_dyneither(_tmp821,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpE29,_tmpE28,_tag_dyneither(_tmp820,sizeof(void*),0U));});});goto _LLB4;}_LLB4:;}
# 4315
if(fmt_arg_start != 0  && !_tmp8ED){
# 4319
int problem;
{struct _tuple31 _tmp825=({struct _tuple31 _tmp829;_tmp829.f1=ft;({void*_tmpE2A=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp8EC))->type));_tmp829.f2=_tmpE2A;});_tmp829;});struct _tuple31 _tmp826=_tmp825;struct Cyc_Absyn_Datatypedecl*_tmp828;struct Cyc_Absyn_Datatypedecl*_tmp827;if(_tmp826.f1 == Cyc_Absyn_Printf_ft){if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp826.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp826.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC1: _tmp827=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp826.f2)->f1).datatype_info).KnownDatatype).val;_LLC2:
# 4322
({int _tmpE2B=Cyc_Absyn_qvar_cmp(_tmp827->name,Cyc_Absyn_datatype_print_arg_qvar)!= 0;problem=_tmpE2B;});goto _LLC0;}else{goto _LLC5;}}else{goto _LLC5;}}else{if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp826.f2)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp826.f2)->f1).datatype_info).KnownDatatype).tag == 2){_LLC3: _tmp828=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp826.f2)->f1).datatype_info).KnownDatatype).val;_LLC4:
# 4324
({int _tmpE2C=Cyc_Absyn_qvar_cmp(_tmp828->name,Cyc_Absyn_datatype_scanf_arg_qvar)!= 0;problem=_tmpE2C;});goto _LLC0;}else{goto _LLC5;}}else{_LLC5: _LLC6:
# 4326
 problem=1;goto _LLC0;}}_LLC0:;}
# 4328
if(problem)
({void*_tmp82A=0U;({unsigned int _tmpE2E=loc;struct _dyneither_ptr _tmpE2D=({const char*_tmp82B="format attribute and vararg types don't match";_tag_dyneither(_tmp82B,sizeof(char),46U);});Cyc_Tcutil_terr(_tmpE2E,_tmpE2D,_tag_dyneither(_tmp82A,sizeof(void*),0U));});});}}}
# 4336
{struct Cyc_List_List*rpo=_tmp8EB;for(0;rpo != 0;rpo=rpo->tl){
struct _tuple0*_tmp82D=(struct _tuple0*)rpo->hd;struct _tuple0*_tmp82E=_tmp82D;void*_tmp830;void*_tmp82F;_LLC8: _tmp830=_tmp82E->f1;_tmp82F=_tmp82E->f2;_LLC9:;
({struct Cyc_Tcutil_CVTEnv _tmpE2F=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F4,& Cyc_Tcutil_ek,_tmp830,1,1);_tmp7F4=_tmpE2F;});
({struct Cyc_Tcutil_CVTEnv _tmpE30=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F4,& Cyc_Tcutil_trk,_tmp82F,1,1);_tmp7F4=_tmpE30;});}}{
# 4344
struct Cyc_Tcenv_Fenv*_tmp831=Cyc_Tcenv_bogus_fenv(_tmp8EF,_tmp8EE);
struct Cyc_Tcenv_Tenv*_tmp832=({struct Cyc_Tcenv_Tenv*_tmp881=_cycalloc(sizeof(*_tmp881));_tmp881->ns=te->ns;_tmp881->ae=te->ae;_tmp881->le=_tmp831;_tmp881->allow_valueof=1;_tmp881->in_extern_c_include=te->in_extern_c_include;_tmp881;});
struct _tuple30 _tmp833=({unsigned int _tmpE34=loc;struct Cyc_Tcenv_Tenv*_tmpE33=_tmp832;struct Cyc_Tcutil_CVTEnv _tmpE32=_tmp7F4;struct _dyneither_ptr _tmpE31=({const char*_tmp880="@requires";_tag_dyneither(_tmp880,sizeof(char),10U);});Cyc_Tcutil_check_clause(_tmpE34,_tmpE33,_tmpE32,_tmpE31,_tmp8E9);});struct _tuple30 _tmp834=_tmp833;struct Cyc_Tcutil_CVTEnv _tmp87F;struct Cyc_List_List*_tmp87E;_LLCB: _tmp87F=_tmp834.f1;_tmp87E=_tmp834.f2;_LLCC:;
_tmp7F4=_tmp87F;
*_tmp8E8=_tmp87E;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp8EE,_tmp87E);{
# 4357
struct _tuple30 _tmp835=({unsigned int _tmpE38=loc;struct Cyc_Tcenv_Tenv*_tmpE37=_tmp832;struct Cyc_Tcutil_CVTEnv _tmpE36=_tmp7F4;struct _dyneither_ptr _tmpE35=({const char*_tmp87D="@ensures";_tag_dyneither(_tmp87D,sizeof(char),9U);});Cyc_Tcutil_check_clause(_tmpE38,_tmpE37,_tmpE36,_tmpE35,_tmp8E7);});struct _tuple30 _tmp836=_tmp835;struct Cyc_Tcutil_CVTEnv _tmp87C;struct Cyc_List_List*_tmp87B;_LLCE: _tmp87C=_tmp836.f1;_tmp87B=_tmp836.f2;_LLCF:;
_tmp7F4=_tmp87C;
*_tmp8E6=_tmp87B;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,_tmp8EE,_tmp87B);
# 4362
if(*_tmp8F1 != 0)
({struct Cyc_Tcutil_CVTEnv _tmpE39=Cyc_Tcutil_i_check_valid_type(loc,te,_tmp7F4,& Cyc_Tcutil_ek,(void*)_check_null(*_tmp8F1),1,1);_tmp7F4=_tmpE39;});else{
# 4365
struct Cyc_List_List*effect=0;
# 4370
{struct Cyc_List_List*tvs=_tmp7F4.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp837=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp838=_tmp837;struct Cyc_Absyn_Tvar*_tmp851;int _tmp850;_LLD1: _tmp851=_tmp838.f1;_tmp850=_tmp838.f2;_LLD2:;
if(!_tmp850)continue;{
void*_tmp839=Cyc_Absyn_compress_kb(_tmp851->kind);void*_tmp83A=_tmp839;struct Cyc_Core_Opt**_tmp84F;struct Cyc_Absyn_Kind*_tmp84E;struct Cyc_Core_Opt**_tmp84D;struct Cyc_Core_Opt**_tmp84C;struct Cyc_Absyn_Kind*_tmp84B;switch(*((int*)_tmp83A)){case 2U: _LLD4: _tmp84C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp83A)->f1;_tmp84B=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp83A)->f2;if(_tmp84B->kind == Cyc_Absyn_RgnKind){_LLD5:
# 4376
 if(_tmp84B->aliasqual == Cyc_Absyn_Top){
({struct Cyc_Core_Opt*_tmpE3A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp84C=_tmpE3A;});_tmp84E=_tmp84B;goto _LLD7;}
# 4379
({struct Cyc_Core_Opt*_tmpE3B=Cyc_Tcutil_kind_to_bound_opt(_tmp84B);*_tmp84C=_tmpE3B;});_tmp84E=_tmp84B;goto _LLD7;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp83A)->f2)->kind){case Cyc_Absyn_IntKind: _LLD8: _LLD9:
# 4382
 goto _LLDB;case Cyc_Absyn_EffKind: _LLDC: _tmp84D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp83A)->f1;_LLDD:
# 4385
({struct Cyc_Core_Opt*_tmpE3C=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ek);*_tmp84D=_tmpE3C;});goto _LLDF;default: goto _LLE2;}}case 0U: _LLD6: _tmp84E=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp83A)->f1;if(_tmp84E->kind == Cyc_Absyn_RgnKind){_LLD7:
# 4381
({struct Cyc_List_List*_tmpE41=({struct Cyc_List_List*_tmp83B=_cycalloc(sizeof(*_tmp83B));({void*_tmpE40=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp83C=_cycalloc(sizeof(*_tmp83C));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE3F=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp83D;_tmp83D.tag=23U;({void*_tmpE3E=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp83E=_cycalloc(sizeof(*_tmp83E));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE3D=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp83F;_tmp83F.tag=2U;_tmp83F.f1=_tmp851;_tmp83F;});_tmp83E[0]=_tmpE3D;});_tmp83E;});_tmp83D.f1=_tmpE3E;});_tmp83D;});_tmp83C[0]=_tmpE3F;});_tmp83C;});_tmp83B->hd=_tmpE40;});_tmp83B->tl=effect;_tmp83B;});effect=_tmpE41;});goto _LLD3;}else{switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp83A)->f1)->kind){case Cyc_Absyn_IntKind: _LLDA: _LLDB:
# 4383
 goto _LLD3;case Cyc_Absyn_EffKind: _LLDE: _LLDF:
# 4387
({struct Cyc_List_List*_tmpE44=({struct Cyc_List_List*_tmp840=_cycalloc(sizeof(*_tmp840));({void*_tmpE43=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp841=_cycalloc(sizeof(*_tmp841));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE42=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp842;_tmp842.tag=2U;_tmp842.f1=_tmp851;_tmp842;});_tmp841[0]=_tmpE42;});_tmp841;});_tmp840->hd=_tmpE43;});_tmp840->tl=effect;_tmp840;});effect=_tmpE44;});goto _LLD3;default: _LLE2: _LLE3:
# 4392
({struct Cyc_List_List*_tmpE49=({struct Cyc_List_List*_tmp846=_cycalloc(sizeof(*_tmp846));({void*_tmpE48=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp847=_cycalloc(sizeof(*_tmp847));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE47=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp848;_tmp848.tag=25U;({void*_tmpE46=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_tmp849=_cycalloc(sizeof(*_tmp849));({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmpE45=({struct Cyc_Absyn_VarType_Absyn_Type_struct _tmp84A;_tmp84A.tag=2U;_tmp84A.f1=_tmp851;_tmp84A;});_tmp849[0]=_tmpE45;});_tmp849;});_tmp848.f1=_tmpE46;});_tmp848;});_tmp847[0]=_tmpE47;});_tmp847;});_tmp846->hd=_tmpE48;});_tmp846->tl=effect;_tmp846;});effect=_tmpE49;});goto _LLD3;}}default: _LLE0: _tmp84F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp83A)->f1;_LLE1:
# 4389
({struct Cyc_Core_Opt*_tmpE4C=({struct Cyc_Core_Opt*_tmp843=_cycalloc(sizeof(*_tmp843));({void*_tmpE4B=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp844=_cycalloc(sizeof(*_tmp844));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpE4A=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp845;_tmp845.tag=2U;_tmp845.f1=0;_tmp845.f2=& Cyc_Tcutil_ak;_tmp845;});_tmp844[0]=_tmpE4A;});_tmp844;});_tmp843->v=_tmpE4B;});_tmp843;});*_tmp84F=_tmpE4C;});goto _LLE3;}_LLD3:;};}}
# 4396
{struct Cyc_List_List*ts=_tmp7F4.free_evars;for(0;ts != 0;ts=ts->tl){
struct _tuple29 _tmp852=*((struct _tuple29*)ts->hd);struct _tuple29 _tmp853=_tmp852;void*_tmp85E;int _tmp85D;_LLE5: _tmp85E=_tmp853.f1;_tmp85D=_tmp853.f2;_LLE6:;
if(!_tmp85D)continue;{
struct Cyc_Absyn_Kind*_tmp854=Cyc_Tcutil_typ_kind(_tmp85E);struct Cyc_Absyn_Kind*_tmp855=_tmp854;switch(((struct Cyc_Absyn_Kind*)_tmp855)->kind){case Cyc_Absyn_RgnKind: _LLE8: _LLE9:
# 4401
({struct Cyc_List_List*_tmpE4F=({struct Cyc_List_List*_tmp856=_cycalloc(sizeof(*_tmp856));({void*_tmpE4E=(void*)({struct Cyc_Absyn_AccessEff_Absyn_Type_struct*_tmp857=_cycalloc(sizeof(*_tmp857));({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmpE4D=({struct Cyc_Absyn_AccessEff_Absyn_Type_struct _tmp858;_tmp858.tag=23U;_tmp858.f1=_tmp85E;_tmp858;});_tmp857[0]=_tmpE4D;});_tmp857;});_tmp856->hd=_tmpE4E;});_tmp856->tl=effect;_tmp856;});effect=_tmpE4F;});goto _LLE7;case Cyc_Absyn_EffKind: _LLEA: _LLEB:
# 4403
({struct Cyc_List_List*_tmpE50=({struct Cyc_List_List*_tmp859=_cycalloc(sizeof(*_tmp859));_tmp859->hd=_tmp85E;_tmp859->tl=effect;_tmp859;});effect=_tmpE50;});goto _LLE7;case Cyc_Absyn_IntKind: _LLEC: _LLED:
 goto _LLE7;default: _LLEE: _LLEF:
# 4406
({struct Cyc_List_List*_tmpE53=({struct Cyc_List_List*_tmp85A=_cycalloc(sizeof(*_tmp85A));({void*_tmpE52=(void*)({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*_tmp85B=_cycalloc(sizeof(*_tmp85B));({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmpE51=({struct Cyc_Absyn_RgnsEff_Absyn_Type_struct _tmp85C;_tmp85C.tag=25U;_tmp85C.f1=_tmp85E;_tmp85C;});_tmp85B[0]=_tmpE51;});_tmp85B;});_tmp85A->hd=_tmpE52;});_tmp85A->tl=effect;_tmp85A;});effect=_tmpE53;});goto _LLE7;}_LLE7:;};}}
# 4409
({void*_tmpE55=(void*)({struct Cyc_Absyn_JoinEff_Absyn_Type_struct*_tmp85F=_cycalloc(sizeof(*_tmp85F));({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmpE54=({struct Cyc_Absyn_JoinEff_Absyn_Type_struct _tmp860;_tmp860.tag=24U;_tmp860.f1=effect;_tmp860;});_tmp85F[0]=_tmpE54;});_tmp85F;});*_tmp8F1=_tmpE55;});}
# 4415
if(*_tmp8F2 != 0){
struct Cyc_List_List*bs=*_tmp8F2;for(0;bs != 0;bs=bs->tl){
void*_tmp861=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)bs->hd)->kind);void*_tmp862=_tmp861;struct Cyc_Core_Opt**_tmp872;struct Cyc_Absyn_Kind*_tmp871;struct Cyc_Core_Opt**_tmp870;struct Cyc_Core_Opt**_tmp86F;struct Cyc_Core_Opt**_tmp86E;struct Cyc_Core_Opt**_tmp86D;struct Cyc_Core_Opt**_tmp86C;struct Cyc_Core_Opt**_tmp86B;struct Cyc_Core_Opt**_tmp86A;struct Cyc_Core_Opt**_tmp869;struct Cyc_Core_Opt**_tmp868;switch(*((int*)_tmp862)){case 1U: _LLF1: _tmp868=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LLF2:
# 4419
({struct Cyc_String_pa_PrintArg_struct _tmp865;_tmp865.tag=0U;_tmp865.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)bs->hd)->name);({void*_tmp863[1U]={& _tmp865};({unsigned int _tmpE57=loc;struct _dyneither_ptr _tmpE56=({const char*_tmp864="Type variable %s unconstrained, assuming boxed";_tag_dyneither(_tmp864,sizeof(char),47U);});Cyc_Tcutil_warn(_tmpE57,_tmpE56,_tag_dyneither(_tmp863,sizeof(void*),1U));});});});
# 4421
_tmp869=_tmp868;goto _LLF4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f2)->aliasqual == Cyc_Absyn_Top){_LLF3: _tmp869=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LLF4:
 _tmp86A=_tmp869;goto _LLF6;}else{goto _LL103;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF5: _tmp86A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LLF6:
 _tmp86B=_tmp86A;goto _LLF8;case Cyc_Absyn_Aliasable: _LLF7: _tmp86B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LLF8:
 _tmp86D=_tmp86B;goto _LLFA;default: _LLFD: _tmp86C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LLFE:
# 4428
 _tmp86F=_tmp86C;goto _LL100;}case Cyc_Absyn_AnyKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f2)->aliasqual){case Cyc_Absyn_Top: _LLF9: _tmp86D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LLFA:
# 4425
 _tmp86E=_tmp86D;goto _LLFC;case Cyc_Absyn_Aliasable: _LLFB: _tmp86E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LLFC:
# 4427
({struct Cyc_Core_Opt*_tmpE58=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp86E=_tmpE58;});goto _LLF0;default: _LLFF: _tmp86F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LL100:
# 4430
({struct Cyc_Core_Opt*_tmpE59=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp86F=_tmpE59;});goto _LLF0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f2)->aliasqual == Cyc_Absyn_Top){_LL101: _tmp870=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_LL102:
# 4432
({struct Cyc_Core_Opt*_tmpE5A=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp870=_tmpE5A;});goto _LLF0;}else{goto _LL103;}default: _LL103: _tmp872=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f1;_tmp871=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp862)->f2;_LL104:
# 4434
({struct Cyc_Core_Opt*_tmpE5B=Cyc_Tcutil_kind_to_bound_opt(_tmp871);*_tmp872=_tmpE5B;});goto _LLF0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp862)->f1)->kind == Cyc_Absyn_MemKind){_LL105: _LL106:
# 4436
({void*_tmp866=0U;({unsigned int _tmpE5D=loc;struct _dyneither_ptr _tmpE5C=({const char*_tmp867="functions can't abstract M types";_tag_dyneither(_tmp867,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpE5D,_tmpE5C,_tag_dyneither(_tmp866,sizeof(void*),0U));});});goto _LLF0;}else{_LL107: _LL108:
 goto _LLF0;}}_LLF0:;}}
# 4441
({struct Cyc_List_List*_tmpE5E=Cyc_Tcutil_remove_bound_tvars(Cyc_Core_heap_region,_tmp7F4.kind_env,*_tmp8F2);cvtenv.kind_env=_tmpE5E;});
({struct Cyc_List_List*_tmpE5F=Cyc_Tcutil_remove_bound_tvars_bool(_tmp7F4.r,_tmp7F4.free_vars,*_tmp8F2);_tmp7F4.free_vars=_tmpE5F;});
# 4444
{struct Cyc_List_List*tvs=_tmp7F4.free_vars;for(0;tvs != 0;tvs=tvs->tl){
struct _tuple28 _tmp873=*((struct _tuple28*)tvs->hd);struct _tuple28 _tmp874=_tmp873;struct Cyc_Absyn_Tvar*_tmp876;int _tmp875;_LL10A: _tmp876=_tmp874.f1;_tmp875=_tmp874.f2;_LL10B:;
({struct Cyc_List_List*_tmpE60=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp876,_tmp875);cvtenv.free_vars=_tmpE60;});}}
# 4449
{struct Cyc_List_List*evs=_tmp7F4.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp877=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp878=_tmp877;void*_tmp87A;int _tmp879;_LL10D: _tmp87A=_tmp878.f1;_tmp879=_tmp878.f2;_LL10E:;
({struct Cyc_List_List*_tmpE61=Cyc_Tcutil_add_free_evar(cvtenv.r,cvtenv.free_evars,_tmp87A,_tmp879);cvtenv.free_evars=_tmpE61;});}}
# 4453
goto _LL0;};};};}case 10U: _LL23: _tmp8F3=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp761)->f1;_LL24:
# 4456
 for(0;_tmp8F3 != 0;_tmp8F3=_tmp8F3->tl){
struct _tuple12*_tmp883=(struct _tuple12*)_tmp8F3->hd;
({struct Cyc_Tcutil_CVTEnv _tmpE62=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,(*_tmp883).f2,1,0);cvtenv=_tmpE62;});
({int _tmpE63=
Cyc_Tcutil_extract_const_from_typedef(loc,((*_tmp883).f1).print_const,(*_tmp883).f2);
# 4459
((*_tmp883).f1).real_const=_tmpE63;});}
# 4462
goto _LL0;case 12U: _LL25: _tmp8F5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp761)->f1;_tmp8F4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp761)->f2;_LL26: {
# 4466
struct Cyc_List_List*prev_fields=0;
for(0;_tmp8F4 != 0;_tmp8F4=_tmp8F4->tl){
struct Cyc_Absyn_Aggrfield*_tmp884=(struct Cyc_Absyn_Aggrfield*)_tmp8F4->hd;struct Cyc_Absyn_Aggrfield*_tmp885=_tmp884;struct _dyneither_ptr*_tmp895;struct Cyc_Absyn_Tqual*_tmp894;void*_tmp893;struct Cyc_Absyn_Exp*_tmp892;struct Cyc_List_List*_tmp891;struct Cyc_Absyn_Exp*_tmp890;_LL110: _tmp895=_tmp885->name;_tmp894=(struct Cyc_Absyn_Tqual*)& _tmp885->tq;_tmp893=_tmp885->type;_tmp892=_tmp885->width;_tmp891=_tmp885->attributes;_tmp890=_tmp885->requires_clause;_LL111:;
if(((int(*)(int(*compare)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*l,struct _dyneither_ptr*x))Cyc_List_mem)(Cyc_strptrcmp,prev_fields,_tmp895))
({struct Cyc_String_pa_PrintArg_struct _tmp888;_tmp888.tag=0U;_tmp888.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp895);({void*_tmp886[1U]={& _tmp888};({unsigned int _tmpE65=loc;struct _dyneither_ptr _tmpE64=({const char*_tmp887="duplicate field %s";_tag_dyneither(_tmp887,sizeof(char),19U);});Cyc_Tcutil_terr(_tmpE65,_tmpE64,_tag_dyneither(_tmp886,sizeof(void*),1U));});});});
if(({struct _dyneither_ptr _tmpE66=(struct _dyneither_ptr)*_tmp895;Cyc_strcmp(_tmpE66,({const char*_tmp889="";_tag_dyneither(_tmp889,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpE67=({struct Cyc_List_List*_tmp88A=_cycalloc(sizeof(*_tmp88A));_tmp88A->hd=_tmp895;_tmp88A->tl=prev_fields;_tmp88A;});prev_fields=_tmpE67;});
({struct Cyc_Tcutil_CVTEnv _tmpE68=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tmk,_tmp893,1,0);cvtenv=_tmpE68;});
({int _tmpE69=Cyc_Tcutil_extract_const_from_typedef(loc,_tmp894->print_const,_tmp893);_tmp894->real_const=_tmpE69;});
# 4479
Cyc_Tcutil_check_bitfield(loc,te,_tmp893,_tmp892,_tmp895);
Cyc_Tcutil_check_field_atts(loc,_tmp895,_tmp891);
if(_tmp890 != 0){
# 4483
if(_tmp8F5 != Cyc_Absyn_UnionA)
({void*_tmp88B=0U;({unsigned int _tmpE6B=loc;struct _dyneither_ptr _tmpE6A=({const char*_tmp88C="@requires clause is only allowed on union members";_tag_dyneither(_tmp88C,sizeof(char),50U);});Cyc_Tcutil_terr(_tmpE6B,_tmpE6A,_tag_dyneither(_tmp88B,sizeof(void*),0U));});});
({struct Cyc_Tcenv_Tenv*_tmpE6C=Cyc_Tcenv_allow_valueof(te);Cyc_Tcexp_tcExp(_tmpE6C,0,_tmp890);});
if(!Cyc_Tcutil_is_integral(_tmp890))
({struct Cyc_String_pa_PrintArg_struct _tmp88F;_tmp88F.tag=0U;({struct _dyneither_ptr _tmpE6D=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string((void*)_check_null(_tmp890->topt)));_tmp88F.f1=_tmpE6D;});({void*_tmp88D[1U]={& _tmp88F};({unsigned int _tmpE6F=loc;struct _dyneither_ptr _tmpE6E=({const char*_tmp88E="@requires clause has type %s instead of integral type";_tag_dyneither(_tmp88E,sizeof(char),54U);});Cyc_Tcutil_terr(_tmpE6F,_tmpE6E,_tag_dyneither(_tmp88D,sizeof(void*),1U));});});});
({struct Cyc_Tcutil_CVTEnv _tmpE70=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp890,te,cvtenv);cvtenv=_tmpE70;});}}
# 4492
goto _LL0;}case 11U: _LL27: _tmp8F7=(union Cyc_Absyn_AggrInfoU*)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp761)->f1).aggr_info;_tmp8F6=(struct Cyc_List_List**)&(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp761)->f1).targs;_LL28:
# 4495
{union Cyc_Absyn_AggrInfoU _tmp896=*_tmp8F7;union Cyc_Absyn_AggrInfoU _tmp897=_tmp896;struct Cyc_Absyn_Aggrdecl*_tmp8BB;enum Cyc_Absyn_AggrKind _tmp8BA;struct _tuple2*_tmp8B9;struct Cyc_Core_Opt*_tmp8B8;if((_tmp897.UnknownAggr).tag == 1){_LL113: _tmp8BA=((_tmp897.UnknownAggr).val).f1;_tmp8B9=((_tmp897.UnknownAggr).val).f2;_tmp8B8=((_tmp897.UnknownAggr).val).f3;_LL114: {
# 4497
struct Cyc_Absyn_Aggrdecl**adp;
{struct _handler_cons _tmp898;_push_handler(& _tmp898);{int _tmp89A=0;if(setjmp(_tmp898.handler))_tmp89A=1;if(!_tmp89A){
({struct Cyc_Absyn_Aggrdecl**_tmpE71=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8B9);adp=_tmpE71;});{
struct Cyc_Absyn_Aggrdecl*_tmp89B=*adp;
if(_tmp89B->kind != _tmp8BA){
if(_tmp89B->kind == Cyc_Absyn_StructA)
({struct Cyc_String_pa_PrintArg_struct _tmp89F;_tmp89F.tag=0U;({struct _dyneither_ptr _tmpE72=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8B9));_tmp89F.f1=_tmpE72;});({struct Cyc_String_pa_PrintArg_struct _tmp89E;_tmp89E.tag=0U;({struct _dyneither_ptr _tmpE73=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4503
Cyc_Absynpp_qvar2string(_tmp8B9));_tmp89E.f1=_tmpE73;});({void*_tmp89C[2U]={& _tmp89E,& _tmp89F};({unsigned int _tmpE75=loc;struct _dyneither_ptr _tmpE74=({const char*_tmp89D="expecting struct %s instead of union %s";_tag_dyneither(_tmp89D,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpE75,_tmpE74,_tag_dyneither(_tmp89C,sizeof(void*),2U));});});});});else{
# 4506
({struct Cyc_String_pa_PrintArg_struct _tmp8A3;_tmp8A3.tag=0U;({struct _dyneither_ptr _tmpE76=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8B9));_tmp8A3.f1=_tmpE76;});({struct Cyc_String_pa_PrintArg_struct _tmp8A2;_tmp8A2.tag=0U;({struct _dyneither_ptr _tmpE77=(struct _dyneither_ptr)((struct _dyneither_ptr)
# 4506
Cyc_Absynpp_qvar2string(_tmp8B9));_tmp8A2.f1=_tmpE77;});({void*_tmp8A0[2U]={& _tmp8A2,& _tmp8A3};({unsigned int _tmpE79=loc;struct _dyneither_ptr _tmpE78=({const char*_tmp8A1="expecting union %s instead of struct %s";_tag_dyneither(_tmp8A1,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpE79,_tmpE78,_tag_dyneither(_tmp8A0,sizeof(void*),2U));});});});});}}
# 4509
if((unsigned int)_tmp8B8  && (int)_tmp8B8->v){
if(!((unsigned int)_tmp89B->impl) || !((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp89B->impl))->tagged)
({struct Cyc_String_pa_PrintArg_struct _tmp8A6;_tmp8A6.tag=0U;({struct _dyneither_ptr _tmpE7A=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8B9));_tmp8A6.f1=_tmpE7A;});({void*_tmp8A4[1U]={& _tmp8A6};({unsigned int _tmpE7C=loc;struct _dyneither_ptr _tmpE7B=({const char*_tmp8A5="@tagged qualfiers don't agree on union %s";_tag_dyneither(_tmp8A5,sizeof(char),42U);});Cyc_Tcutil_terr(_tmpE7C,_tmpE7B,_tag_dyneither(_tmp8A4,sizeof(void*),1U));});});});}
# 4515
({union Cyc_Absyn_AggrInfoU _tmpE7D=Cyc_Absyn_KnownAggr(adp);*_tmp8F7=_tmpE7D;});};
# 4499
;_pop_handler();}else{void*_tmp899=(void*)_exn_thrown;void*_tmp8A7=_tmp899;void*_tmp8AD;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8A7)->tag == Cyc_Dict_Absent){_LL118: _LL119: {
# 4519
struct Cyc_Absyn_Aggrdecl*_tmp8A8=({struct Cyc_Absyn_Aggrdecl*_tmp8AC=_cycalloc(sizeof(*_tmp8AC));_tmp8AC->kind=_tmp8BA;_tmp8AC->sc=Cyc_Absyn_Extern;_tmp8AC->name=_tmp8B9;_tmp8AC->tvs=0;_tmp8AC->impl=0;_tmp8AC->attributes=0;_tmp8AC->expected_mem_kind=0;_tmp8AC;});
Cyc_Tc_tcAggrdecl(te,loc,_tmp8A8);
({struct Cyc_Absyn_Aggrdecl**_tmpE7E=Cyc_Tcenv_lookup_aggrdecl(te,loc,_tmp8B9);adp=_tmpE7E;});
({union Cyc_Absyn_AggrInfoU _tmpE7F=Cyc_Absyn_KnownAggr(adp);*_tmp8F7=_tmpE7F;});
# 4524
if(*_tmp8F6 != 0){
({struct Cyc_String_pa_PrintArg_struct _tmp8AB;_tmp8AB.tag=0U;({struct _dyneither_ptr _tmpE80=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_qvar2string(_tmp8B9));_tmp8AB.f1=_tmpE80;});({void*_tmp8A9[1U]={& _tmp8AB};({unsigned int _tmpE82=loc;struct _dyneither_ptr _tmpE81=({const char*_tmp8AA="declare parameterized type %s before using";_tag_dyneither(_tmp8AA,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpE82,_tmpE81,_tag_dyneither(_tmp8A9,sizeof(void*),1U));});});});
return cvtenv;}
# 4529
goto _LL117;}}else{_LL11A: _tmp8AD=_tmp8A7;_LL11B:(int)_rethrow(_tmp8AD);}_LL117:;}};}
# 4531
_tmp8BB=*adp;goto _LL116;}}else{_LL115: _tmp8BB=*(_tmp897.KnownAggr).val;_LL116: {
# 4533
struct Cyc_List_List*tvs=_tmp8BB->tvs;
struct Cyc_List_List*ts=*_tmp8F6;
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Tvar*_tmp8AE=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp8AF=(void*)ts->hd;
# 4541
{struct _tuple0 _tmp8B0=({struct _tuple0 _tmp8B3;({void*_tmpE83=Cyc_Absyn_compress_kb(_tmp8AE->kind);_tmp8B3.f1=_tmpE83;});_tmp8B3.f2=_tmp8AF;_tmp8B3;});struct _tuple0 _tmp8B1=_tmp8B0;struct Cyc_Absyn_Tvar*_tmp8B2;if(((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp8B1.f1)->tag == 1U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8B1.f2)->tag == 2U){_LL11D: _tmp8B2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp8B1.f2)->f1;_LL11E:
# 4543
({struct Cyc_List_List*_tmpE84=Cyc_Tcutil_add_free_tvar(loc,cvtenv.kind_env,_tmp8B2);cvtenv.kind_env=_tmpE84;});
({struct Cyc_List_List*_tmpE85=Cyc_Tcutil_fast_add_free_tvar_bool(cvtenv.r,cvtenv.free_vars,_tmp8B2,1);cvtenv.free_vars=_tmpE85;});
continue;}else{goto _LL11F;}}else{_LL11F: _LL120:
 goto _LL11C;}_LL11C:;}{
# 4548
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk);
({struct Cyc_Tcutil_CVTEnv _tmpE86=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpE86;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);};}
# 4552
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8B6;_tmp8B6.tag=0U;({struct _dyneither_ptr _tmpE87=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8BB->name));_tmp8B6.f1=_tmpE87;});({void*_tmp8B4[1U]={& _tmp8B6};({unsigned int _tmpE89=loc;struct _dyneither_ptr _tmpE88=({const char*_tmp8B5="too many parameters for type %s";_tag_dyneither(_tmp8B5,sizeof(char),32U);});Cyc_Tcutil_terr(_tmpE89,_tmpE88,_tag_dyneither(_tmp8B4,sizeof(void*),1U));});});});
if(tvs != 0){
# 4556
struct Cyc_List_List*hidden_ts=0;
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,0);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpE8A=({struct Cyc_List_List*_tmp8B7=_cycalloc(sizeof(*_tmp8B7));_tmp8B7->hd=e;_tmp8B7->tl=hidden_ts;_tmp8B7;});hidden_ts=_tmpE8A;});
({struct Cyc_Tcutil_CVTEnv _tmpE8B=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpE8B;});}
# 4563
({struct Cyc_List_List*_tmpE8D=({struct Cyc_List_List*_tmpE8C=*_tmp8F6;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE8C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp8F6=_tmpE8D;});}
# 4565
if((allow_abs_aggr  && _tmp8BB->impl == 0) && !
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,expected_kind))
# 4570
_tmp8BB->expected_mem_kind=1;}}_LL112:;}
# 4573
goto _LL0;case 17U: _LL29: _tmp8FB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp761)->f1;_tmp8FA=(struct Cyc_List_List**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp761)->f2;_tmp8F9=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp761)->f3;_tmp8F8=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp761)->f4;_LL2A: {
# 4576
struct Cyc_List_List*targs=*_tmp8FA;
# 4578
struct Cyc_Absyn_Typedefdecl*td;
{struct _handler_cons _tmp8BC;_push_handler(& _tmp8BC);{int _tmp8BE=0;if(setjmp(_tmp8BC.handler))_tmp8BE=1;if(!_tmp8BE){({struct Cyc_Absyn_Typedefdecl*_tmpE8E=Cyc_Tcenv_lookup_typedefdecl(te,loc,_tmp8FB);td=_tmpE8E;});;_pop_handler();}else{void*_tmp8BD=(void*)_exn_thrown;void*_tmp8BF=_tmp8BD;void*_tmp8C3;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp8BF)->tag == Cyc_Dict_Absent){_LL122: _LL123:
# 4581
({struct Cyc_String_pa_PrintArg_struct _tmp8C2;_tmp8C2.tag=0U;({struct _dyneither_ptr _tmpE8F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8FB));_tmp8C2.f1=_tmpE8F;});({void*_tmp8C0[1U]={& _tmp8C2};({unsigned int _tmpE91=loc;struct _dyneither_ptr _tmpE90=({const char*_tmp8C1="unbound typedef name %s";_tag_dyneither(_tmp8C1,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpE91,_tmpE90,_tag_dyneither(_tmp8C0,sizeof(void*),1U));});});});
return cvtenv;}else{_LL124: _tmp8C3=_tmp8BF;_LL125:(int)_rethrow(_tmp8C3);}_LL121:;}};}
# 4584
*_tmp8F9=td;{
struct Cyc_List_List*tvs=td->tvs;
struct Cyc_List_List*ts=targs;
struct Cyc_List_List*inst=0;
# 4589
for(0;ts != 0  && tvs != 0;(ts=ts->tl,tvs=tvs->tl)){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_tbk,expected_kind,td);
# 4594
({struct Cyc_Tcutil_CVTEnv _tmpE92=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,(void*)ts->hd,1,allow_abs_aggr);cvtenv=_tmpE92;});
Cyc_Tcutil_check_no_qual(loc,(void*)ts->hd);
({struct Cyc_List_List*_tmpE94=({struct Cyc_List_List*_tmp8C4=_cycalloc(sizeof(*_tmp8C4));({struct _tuple16*_tmpE93=({struct _tuple16*_tmp8C5=_cycalloc(sizeof(*_tmp8C5));_tmp8C5->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8C5->f2=(void*)ts->hd;_tmp8C5;});_tmp8C4->hd=_tmpE93;});_tmp8C4->tl=inst;_tmp8C4;});inst=_tmpE94;});}
# 4598
if(ts != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp8C8;_tmp8C8.tag=0U;({struct _dyneither_ptr _tmpE95=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp8FB));_tmp8C8.f1=_tmpE95;});({void*_tmp8C6[1U]={& _tmp8C8};({unsigned int _tmpE97=loc;struct _dyneither_ptr _tmpE96=({const char*_tmp8C7="too many parameters for typedef %s";_tag_dyneither(_tmp8C7,sizeof(char),35U);});Cyc_Tcutil_terr(_tmpE97,_tmpE96,_tag_dyneither(_tmp8C6,sizeof(void*),1U));});});});
if(tvs != 0){
struct Cyc_List_List*hidden_ts=0;
# 4603
for(0;tvs != 0;tvs=tvs->tl){
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_tvar_inst_kind((struct Cyc_Absyn_Tvar*)tvs->hd,& Cyc_Tcutil_bk,expected_kind,td);
void*e=Cyc_Absyn_new_evar(0,0);
({struct Cyc_List_List*_tmpE98=({struct Cyc_List_List*_tmp8C9=_cycalloc(sizeof(*_tmp8C9));_tmp8C9->hd=e;_tmp8C9->tl=hidden_ts;_tmp8C9;});hidden_ts=_tmpE98;});
({struct Cyc_Tcutil_CVTEnv _tmpE99=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,k,e,1,allow_abs_aggr);cvtenv=_tmpE99;});
({struct Cyc_List_List*_tmpE9B=({struct Cyc_List_List*_tmp8CA=_cycalloc(sizeof(*_tmp8CA));({struct _tuple16*_tmpE9A=({struct _tuple16*_tmp8CB=_cycalloc(sizeof(*_tmp8CB));_tmp8CB->f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_tmp8CB->f2=e;_tmp8CB;});_tmp8CA->hd=_tmpE9A;});_tmp8CA->tl=inst;_tmp8CA;});inst=_tmpE9B;});}
# 4611
({struct Cyc_List_List*_tmpE9D=({struct Cyc_List_List*_tmpE9C=targs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmpE9C,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(hidden_ts));});*_tmp8FA=_tmpE9D;});}
# 4613
if(td->defn != 0){
void*new_typ=
inst == 0?(void*)_check_null(td->defn):
 Cyc_Tcutil_substitute(inst,(void*)_check_null(td->defn));
*_tmp8F8=new_typ;}
# 4619
goto _LL0;};}case 22U: _LL2B: _LL2C:
 goto _LL2E;case 21U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL0;case 15U: _LL31: _tmp8FC=(void*)((struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct*)_tmp761)->f1;_LL32:
# 4624
({struct Cyc_Tcutil_CVTEnv _tmpE9E=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp8FC,1,allow_abs_aggr);cvtenv=_tmpE9E;});goto _LL0;case 16U: _LL33: _tmp8FE=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp761)->f1;_tmp8FD=(void*)((struct Cyc_Absyn_DynRgnType_Absyn_Type_struct*)_tmp761)->f2;_LL34:
# 4627
({struct Cyc_Tcutil_CVTEnv _tmpE9F=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp8FE,0,0);cvtenv=_tmpE9F;});
({struct Cyc_Tcutil_CVTEnv _tmpEA0=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_rk,_tmp8FD,1,0);cvtenv=_tmpEA0;});
goto _LL0;case 23U: _LL35: _tmp8FF=(void*)((struct Cyc_Absyn_AccessEff_Absyn_Type_struct*)_tmp761)->f1;_LL36:
# 4631
({struct Cyc_Tcutil_CVTEnv _tmpEA1=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_trk,_tmp8FF,1,1);cvtenv=_tmpEA1;});goto _LL0;case 25U: _LL37: _tmp900=(void*)((struct Cyc_Absyn_RgnsEff_Absyn_Type_struct*)_tmp761)->f1;_LL38:
# 4633
({struct Cyc_Tcutil_CVTEnv _tmpEA2=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp900,1,1);cvtenv=_tmpEA2;});goto _LL0;default: _LL39: _tmp901=((struct Cyc_Absyn_JoinEff_Absyn_Type_struct*)_tmp761)->f1;_LL3A:
# 4635
 for(0;_tmp901 != 0;_tmp901=_tmp901->tl){
({struct Cyc_Tcutil_CVTEnv _tmpEA3=Cyc_Tcutil_i_check_valid_type(loc,te,cvtenv,& Cyc_Tcutil_ek,(void*)_tmp901->hd,1,1);cvtenv=_tmpEA3;});}
goto _LL0;}_LL0:;}
# 4639
if(!({struct Cyc_Absyn_Kind*_tmpEA4=Cyc_Tcutil_typ_kind(t);Cyc_Tcutil_kind_leq(_tmpEA4,expected_kind);}))
({struct Cyc_String_pa_PrintArg_struct _tmp906;_tmp906.tag=0U;({struct _dyneither_ptr _tmpEA5=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(expected_kind));_tmp906.f1=_tmpEA5;});({struct Cyc_String_pa_PrintArg_struct _tmp905;_tmp905.tag=0U;({struct _dyneither_ptr _tmpEA6=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_typ_kind(t)));_tmp905.f1=_tmpEA6;});({struct Cyc_String_pa_PrintArg_struct _tmp904;_tmp904.tag=0U;({struct _dyneither_ptr _tmpEA7=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp904.f1=_tmpEA7;});({void*_tmp902[3U]={& _tmp904,& _tmp905,& _tmp906};({unsigned int _tmpEA9=loc;struct _dyneither_ptr _tmpEA8=({const char*_tmp903="type %s has kind %s but as used here needs kind %s";_tag_dyneither(_tmp903,sizeof(char),51U);});Cyc_Tcutil_terr(_tmpEA9,_tmpEA8,_tag_dyneither(_tmp902,sizeof(void*),3U));});});});});});
# 4643
return cvtenv;}
# 4651
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_i_check_valid_type_level_exp(struct Cyc_Absyn_Exp*e,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvtenv){
# 4653
{void*_tmp907=e->r;void*_tmp908=_tmp907;struct Cyc_Absyn_Exp*_tmp91A;void*_tmp919;void*_tmp918;void*_tmp917;void*_tmp916;struct Cyc_Absyn_Exp*_tmp915;struct Cyc_Absyn_Exp*_tmp914;struct Cyc_Absyn_Exp*_tmp913;struct Cyc_Absyn_Exp*_tmp912;struct Cyc_Absyn_Exp*_tmp911;struct Cyc_Absyn_Exp*_tmp910;struct Cyc_Absyn_Exp*_tmp90F;struct Cyc_Absyn_Exp*_tmp90E;struct Cyc_Absyn_Exp*_tmp90D;struct Cyc_Absyn_Exp*_tmp90C;struct Cyc_List_List*_tmp90B;switch(*((int*)_tmp908)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 goto _LL0;case 3U: _LLB: _tmp90B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp908)->f2;_LLC:
# 4660
 for(0;_tmp90B != 0;_tmp90B=_tmp90B->tl){
({struct Cyc_Tcutil_CVTEnv _tmpEAA=Cyc_Tcutil_i_check_valid_type_level_exp((struct Cyc_Absyn_Exp*)_tmp90B->hd,te,cvtenv);cvtenv=_tmpEAA;});}
goto _LL0;case 6U: _LLD: _tmp90E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_tmp90D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp908)->f2;_tmp90C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp908)->f3;_LLE:
# 4664
({struct Cyc_Tcutil_CVTEnv _tmpEAB=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp90E,te,cvtenv);cvtenv=_tmpEAB;});
({struct Cyc_Tcutil_CVTEnv _tmpEAC=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp90D,te,cvtenv);cvtenv=_tmpEAC;});
({struct Cyc_Tcutil_CVTEnv _tmpEAD=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp90C,te,cvtenv);cvtenv=_tmpEAD;});
goto _LL0;case 7U: _LLF: _tmp910=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_tmp90F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp908)->f2;_LL10:
 _tmp912=_tmp910;_tmp911=_tmp90F;goto _LL12;case 8U: _LL11: _tmp912=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_tmp911=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp908)->f2;_LL12:
 _tmp914=_tmp912;_tmp913=_tmp911;goto _LL14;case 9U: _LL13: _tmp914=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_tmp913=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp908)->f2;_LL14:
# 4671
({struct Cyc_Tcutil_CVTEnv _tmpEAE=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp914,te,cvtenv);cvtenv=_tmpEAE;});
({struct Cyc_Tcutil_CVTEnv _tmpEAF=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp913,te,cvtenv);cvtenv=_tmpEAF;});
goto _LL0;case 14U: _LL15: _tmp916=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_tmp915=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp908)->f2;_LL16:
# 4675
({struct Cyc_Tcutil_CVTEnv _tmpEB0=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp915,te,cvtenv);cvtenv=_tmpEB0;});
({struct Cyc_Tcutil_CVTEnv _tmpEB1=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp916,1,0);cvtenv=_tmpEB1;});
goto _LL0;case 19U: _LL17: _tmp917=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_LL18:
 _tmp918=_tmp917;goto _LL1A;case 17U: _LL19: _tmp918=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_LL1A:
# 4680
({struct Cyc_Tcutil_CVTEnv _tmpEB2=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_tak,_tmp918,1,0);cvtenv=_tmpEB2;});
goto _LL0;case 39U: _LL1B: _tmp919=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_LL1C:
# 4683
({struct Cyc_Tcutil_CVTEnv _tmpEB3=Cyc_Tcutil_i_check_valid_type(e->loc,te,cvtenv,& Cyc_Tcutil_ik,_tmp919,1,0);cvtenv=_tmpEB3;});
goto _LL0;case 18U: _LL1D: _tmp91A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp908)->f1;_LL1E:
# 4686
({struct Cyc_Tcutil_CVTEnv _tmpEB4=Cyc_Tcutil_i_check_valid_type_level_exp(_tmp91A,te,cvtenv);cvtenv=_tmpEB4;});
goto _LL0;default: _LL1F: _LL20:
# 4689
({void*_tmp909=0U;({struct _dyneither_ptr _tmpEB5=({const char*_tmp90A="non-type-level-expression in Tcutil::i_check_valid_type_level_exp";_tag_dyneither(_tmp90A,sizeof(char),66U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEB5,_tag_dyneither(_tmp909,sizeof(void*),0U));});});}_LL0:;}
# 4691
return cvtenv;}
# 4694
static struct Cyc_Tcutil_CVTEnv Cyc_Tcutil_check_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcutil_CVTEnv cvt,struct Cyc_Absyn_Kind*expected_kind,int allow_abs_aggr,void*t){
# 4699
struct Cyc_List_List*_tmp91B=cvt.kind_env;
({struct Cyc_Tcutil_CVTEnv _tmpEB6=Cyc_Tcutil_i_check_valid_type(loc,te,cvt,expected_kind,t,1,allow_abs_aggr);cvt=_tmpEB6;});
# 4702
{struct Cyc_List_List*vs=cvt.free_vars;for(0;vs != 0;vs=vs->tl){
struct _tuple28 _tmp91C=*((struct _tuple28*)vs->hd);struct _tuple28 _tmp91D=_tmp91C;struct Cyc_Absyn_Tvar*_tmp91E;_LL1: _tmp91E=_tmp91D.f1;_LL2:;
({struct Cyc_List_List*_tmpEB7=Cyc_Tcutil_fast_add_free_tvar(_tmp91B,_tmp91E);cvt.kind_env=_tmpEB7;});}}
# 4710
{struct Cyc_List_List*evs=cvt.free_evars;for(0;evs != 0;evs=evs->tl){
struct _tuple29 _tmp91F=*((struct _tuple29*)evs->hd);struct _tuple29 _tmp920=_tmp91F;void*_tmp929;_LL4: _tmp929=_tmp920.f1;_LL5:;{
void*_tmp921=Cyc_Tcutil_compress(_tmp929);void*_tmp922=_tmp921;struct Cyc_Core_Opt**_tmp928;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp922)->tag == 1U){_LL7: _tmp928=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp922)->f4;_LL8:
# 4714
 if(*_tmp928 == 0)
({struct Cyc_Core_Opt*_tmpEB8=({struct Cyc_Core_Opt*_tmp923=_cycalloc(sizeof(*_tmp923));_tmp923->v=_tmp91B;_tmp923;});*_tmp928=_tmpEB8;});else{
# 4718
struct Cyc_List_List*_tmp924=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp928))->v;
struct Cyc_List_List*_tmp925=0;
for(0;_tmp924 != 0;_tmp924=_tmp924->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp91B,(struct Cyc_Absyn_Tvar*)_tmp924->hd))
({struct Cyc_List_List*_tmpEB9=({struct Cyc_List_List*_tmp926=_cycalloc(sizeof(*_tmp926));_tmp926->hd=(struct Cyc_Absyn_Tvar*)_tmp924->hd;_tmp926->tl=_tmp925;_tmp926;});_tmp925=_tmpEB9;});}
({struct Cyc_Core_Opt*_tmpEBA=({struct Cyc_Core_Opt*_tmp927=_cycalloc(sizeof(*_tmp927));_tmp927->v=_tmp925;_tmp927;});*_tmp928=_tmpEBA;});}
# 4725
goto _LL6;}else{_LL9: _LLA:
 goto _LL6;}_LL6:;};}}
# 4729
return cvt;}
# 4735
void Cyc_Tcutil_check_free_evars(struct Cyc_List_List*free_evars,void*in_t,unsigned int loc){
for(0;free_evars != 0;free_evars=free_evars->tl){
void*e=(void*)free_evars->hd;
struct Cyc_Absyn_Kind*_tmp92A=Cyc_Tcutil_typ_kind(e);struct Cyc_Absyn_Kind*_tmp92B=_tmp92A;switch(((struct Cyc_Absyn_Kind*)_tmp92B)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp92B)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
# 4740
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_UniqueRgn_val))
({void*_tmp92C=0U;({struct _dyneither_ptr _tmpEBB=({const char*_tmp92D="can't unify evar with unique region!";_tag_dyneither(_tmp92D,sizeof(char),37U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEBB,_tag_dyneither(_tmp92C,sizeof(void*),0U));});});
goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 goto _LL6;default: _LL5: _LL6:
# 4745
 if(!Cyc_Tcutil_unify(e,(void*)& Cyc_Absyn_HeapRgn_val))({void*_tmp92E=0U;({struct _dyneither_ptr _tmpEBC=({const char*_tmp92F="can't unify evar with heap!";_tag_dyneither(_tmp92F,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEBC,_tag_dyneither(_tmp92E,sizeof(void*),0U));});});
goto _LL0;}case Cyc_Absyn_EffKind: _LL7: _LL8:
# 4748
 if(!Cyc_Tcutil_unify(e,Cyc_Absyn_empty_effect))({void*_tmp930=0U;({struct _dyneither_ptr _tmpEBD=({const char*_tmp931="can't unify evar with {}!";_tag_dyneither(_tmp931,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpEBD,_tag_dyneither(_tmp930,sizeof(void*),0U));});});
goto _LL0;default: _LL9: _LLA:
# 4751
({struct Cyc_String_pa_PrintArg_struct _tmp935;_tmp935.tag=0U;({struct _dyneither_ptr _tmpEBE=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_typ2string(in_t));_tmp935.f1=_tmpEBE;});({struct Cyc_String_pa_PrintArg_struct _tmp934;_tmp934.tag=0U;({struct _dyneither_ptr _tmpEBF=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(e));_tmp934.f1=_tmpEBF;});({void*_tmp932[2U]={& _tmp934,& _tmp935};({unsigned int _tmpEC1=loc;struct _dyneither_ptr _tmpEC0=({const char*_tmp933="hidden type variable %s isn't abstracted in type %s";_tag_dyneither(_tmp933,sizeof(char),52U);});Cyc_Tcutil_terr(_tmpEC1,_tmpEC0,_tag_dyneither(_tmp932,sizeof(void*),2U));});});});});
goto _LL0;}_LL0:;}}
# 4763
void Cyc_Tcutil_check_valid_toplevel_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,void*t){
int generalize_evars=Cyc_Tcutil_is_function_type(t);
struct Cyc_List_List*_tmp936=Cyc_Tcenv_lookup_type_vars(te);
struct Cyc_Absyn_Kind*expected_kind=generalize_evars?& Cyc_Tcutil_tak:& Cyc_Tcutil_tmk;
struct Cyc_Tcutil_CVTEnv _tmp937=({unsigned int _tmpEC5=loc;struct Cyc_Tcenv_Tenv*_tmpEC4=te;struct Cyc_Tcutil_CVTEnv _tmpEC3=({struct Cyc_Tcutil_CVTEnv _tmp955;_tmp955.r=Cyc_Core_heap_region;_tmp955.kind_env=_tmp936;_tmp955.free_vars=0;_tmp955.free_evars=0;_tmp955.generalize_evars=generalize_evars;_tmp955.fn_result=0;_tmp955;});struct Cyc_Absyn_Kind*_tmpEC2=expected_kind;Cyc_Tcutil_check_valid_type(_tmpEC5,_tmpEC4,_tmpEC3,_tmpEC2,1,t);});
# 4771
struct Cyc_List_List*_tmp938=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp937.free_vars);
struct Cyc_List_List*_tmp939=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp937.free_evars);
# 4775
if(_tmp936 != 0){
struct Cyc_List_List*_tmp93A=0;
{struct Cyc_List_List*_tmp93B=_tmp938;for(0;(unsigned int)_tmp93B;_tmp93B=_tmp93B->tl){
struct Cyc_Absyn_Tvar*_tmp93C=(struct Cyc_Absyn_Tvar*)_tmp93B->hd;
int found=0;
{struct Cyc_List_List*_tmp93D=_tmp936;for(0;(unsigned int)_tmp93D;_tmp93D=_tmp93D->tl){
if(Cyc_Absyn_tvar_cmp(_tmp93C,(struct Cyc_Absyn_Tvar*)_tmp93D->hd)== 0){found=1;break;}}}
if(!found)
({struct Cyc_List_List*_tmpEC6=({struct Cyc_List_List*_tmp93E=_cycalloc(sizeof(*_tmp93E));_tmp93E->hd=(struct Cyc_Absyn_Tvar*)_tmp93B->hd;_tmp93E->tl=_tmp93A;_tmp93E;});_tmp93A=_tmpEC6;});}}
# 4785
({struct Cyc_List_List*_tmpEC7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp93A);_tmp938=_tmpEC7;});}
# 4791
{struct Cyc_List_List*x=_tmp938;for(0;x != 0;x=x->tl){
void*_tmp93F=Cyc_Absyn_compress_kb(((struct Cyc_Absyn_Tvar*)x->hd)->kind);void*_tmp940=_tmp93F;enum Cyc_Absyn_AliasQual _tmp94E;struct Cyc_Core_Opt**_tmp94D;struct Cyc_Absyn_Kind*_tmp94C;struct Cyc_Core_Opt**_tmp94B;struct Cyc_Core_Opt**_tmp94A;struct Cyc_Core_Opt**_tmp949;struct Cyc_Core_Opt**_tmp948;struct Cyc_Core_Opt**_tmp947;struct Cyc_Core_Opt**_tmp946;switch(*((int*)_tmp940)){case 1U: _LL1: _tmp946=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp940)->f1;_LL2:
 _tmp947=_tmp946;goto _LL4;case 2U: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f2)->kind){case Cyc_Absyn_BoxKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f2)->aliasqual == Cyc_Absyn_Top){_LL3: _tmp947=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f1;_LL4:
 _tmp948=_tmp947;goto _LL6;}else{goto _LLD;}case Cyc_Absyn_MemKind: switch(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f2)->aliasqual){case Cyc_Absyn_Top: _LL5: _tmp948=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f1;_LL6:
 _tmp949=_tmp948;goto _LL8;case Cyc_Absyn_Aliasable: _LL7: _tmp949=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f1;_LL8:
# 4797
({struct Cyc_Core_Opt*_tmpEC8=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_bk);*_tmp949=_tmpEC8;});goto _LL0;default: _LL9: _tmp94A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f1;_LLA:
# 4799
({struct Cyc_Core_Opt*_tmpEC9=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_ubk);*_tmp94A=_tmpEC9;});goto _LL0;}case Cyc_Absyn_RgnKind: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f2)->aliasqual == Cyc_Absyn_Top){_LLB: _tmp94B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f1;_LLC:
# 4801
({struct Cyc_Core_Opt*_tmpECA=Cyc_Tcutil_kind_to_bound_opt(& Cyc_Tcutil_rk);*_tmp94B=_tmpECA;});goto _LL0;}else{goto _LLD;}default: _LLD: _tmp94D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f1;_tmp94C=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp940)->f2;_LLE:
# 4803
({struct Cyc_Core_Opt*_tmpECB=Cyc_Tcutil_kind_to_bound_opt(_tmp94C);*_tmp94D=_tmpECB;});goto _LL0;}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp940)->f1)->kind == Cyc_Absyn_MemKind){_LLF: _tmp94E=(((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp940)->f1)->aliasqual;_LL10:
# 4805
({struct Cyc_String_pa_PrintArg_struct _tmp944;_tmp944.tag=0U;({struct _dyneither_ptr _tmpECC=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_kind2string(({struct Cyc_Absyn_Kind*_tmp945=_cycalloc_atomic(sizeof(*_tmp945));_tmp945->kind=Cyc_Absyn_MemKind;_tmp945->aliasqual=_tmp94E;_tmp945;})));_tmp944.f1=_tmpECC;});({struct Cyc_String_pa_PrintArg_struct _tmp943;_tmp943.tag=0U;({struct _dyneither_ptr _tmpECD=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)x->hd));_tmp943.f1=_tmpECD;});({void*_tmp941[2U]={& _tmp943,& _tmp944};({unsigned int _tmpECF=loc;struct _dyneither_ptr _tmpECE=({const char*_tmp942="type variable %s cannot have kind %s";_tag_dyneither(_tmp942,sizeof(char),37U);});Cyc_Tcutil_terr(_tmpECF,_tmpECE,_tag_dyneither(_tmp941,sizeof(void*),2U));});});});});
goto _LL0;}else{_LL11: _LL12:
 goto _LL0;}}_LL0:;}}
# 4812
if(_tmp938 != 0  || _tmp939 != 0){
{void*_tmp94F=Cyc_Tcutil_compress(t);void*_tmp950=_tmp94F;struct Cyc_List_List**_tmp951;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp950)->tag == 9U){_LL14: _tmp951=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp950)->f1).tvars;_LL15:
# 4815
 if(*_tmp951 == 0){
# 4817
({struct Cyc_List_List*_tmpED0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_copy)(_tmp938);*_tmp951=_tmpED0;});
_tmp938=0;}
# 4820
goto _LL13;}else{_LL16: _LL17:
 goto _LL13;}_LL13:;}
# 4823
if(_tmp938 != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp954;_tmp954.tag=0U;_tmp954.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*((struct Cyc_Absyn_Tvar*)_tmp938->hd)->name);({void*_tmp952[1U]={& _tmp954};({unsigned int _tmpED2=loc;struct _dyneither_ptr _tmpED1=({const char*_tmp953="unbound type variable %s";_tag_dyneither(_tmp953,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpED2,_tmpED1,_tag_dyneither(_tmp952,sizeof(void*),1U));});});});
Cyc_Tcutil_check_free_evars(_tmp939,t,loc);}}
# 4833
void Cyc_Tcutil_check_fndecl_valid_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Fndecl*fd){
void*t=Cyc_Tcutil_fndecl2typ(fd);
# 4836
Cyc_Tcutil_check_valid_toplevel_type(loc,te,t);
{void*_tmp956=Cyc_Tcutil_compress(t);void*_tmp957=_tmp956;struct Cyc_List_List*_tmp963;void*_tmp962;struct Cyc_Absyn_Tqual _tmp961;void*_tmp960;struct Cyc_List_List*_tmp95F;struct Cyc_Absyn_Exp*_tmp95E;struct Cyc_List_List*_tmp95D;struct Cyc_Absyn_Exp*_tmp95C;struct Cyc_List_List*_tmp95B;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->tag == 9U){_LL1: _tmp963=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).tvars;_tmp962=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).effect;_tmp961=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).ret_tqual;_tmp960=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).ret_typ;_tmp95F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).args;_tmp95E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).requires_clause;_tmp95D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).requires_relns;_tmp95C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).ensures_clause;_tmp95B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp957)->f1).ensures_relns;_LL2:
# 4839
 fd->tvs=_tmp963;
fd->effect=_tmp962;
{struct Cyc_List_List*_tmp958=fd->args;for(0;_tmp958 != 0;(_tmp958=_tmp958->tl,_tmp95F=_tmp95F->tl)){
# 4843
(*((struct _tuple10*)_tmp958->hd)).f2=(*((struct _tuple10*)((struct Cyc_List_List*)_check_null(_tmp95F))->hd)).f2;
(*((struct _tuple10*)_tmp958->hd)).f3=(*((struct _tuple10*)_tmp95F->hd)).f3;}}
# 4846
fd->ret_tqual=_tmp961;
fd->ret_type=_tmp960;
({int _tmpED3=Cyc_Tcutil_extract_const_from_typedef(loc,(fd->ret_tqual).print_const,_tmp960);(fd->ret_tqual).real_const=_tmpED3;});
fd->requires_clause=_tmp95E;
fd->requires_relns=_tmp95D;
fd->ensures_clause=_tmp95C;
fd->ensures_relns=_tmp95B;
goto _LL0;}else{_LL3: _LL4:
({void*_tmp959=0U;({struct _dyneither_ptr _tmpED4=({const char*_tmp95A="check_fndecl_valid_type: not a FnType";_tag_dyneither(_tmp95A,sizeof(char),38U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpED4,_tag_dyneither(_tmp959,sizeof(void*),0U));});});}_LL0:;}
# 4856
({struct Cyc_List_List*_tmpED6=((struct Cyc_List_List*(*)(struct _dyneither_ptr*(*f)(struct _tuple10*),struct Cyc_List_List*x))Cyc_List_map)((struct _dyneither_ptr*(*)(struct _tuple10*t))Cyc_Tcutil_fst_fdarg,fd->args);unsigned int _tmpED5=loc;Cyc_Tcutil_check_unique_vars(_tmpED6,_tmpED5,({const char*_tmp964="function declaration has repeated parameter";_tag_dyneither(_tmp964,sizeof(char),44U);}));});
# 4859
fd->cached_typ=t;}
# 4864
void Cyc_Tcutil_check_type(unsigned int loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*bound_tvars,struct Cyc_Absyn_Kind*expected_kind,int allow_evars,int allow_abs_aggr,void*t){
# 4870
struct Cyc_Tcutil_CVTEnv _tmp965=({unsigned int _tmpEDB=loc;struct Cyc_Tcenv_Tenv*_tmpEDA=te;struct Cyc_Tcutil_CVTEnv _tmpED9=({struct Cyc_Tcutil_CVTEnv _tmp96D;_tmp96D.r=Cyc_Core_heap_region;_tmp96D.kind_env=bound_tvars;_tmp96D.free_vars=0;_tmp96D.free_evars=0;_tmp96D.generalize_evars=0;_tmp96D.fn_result=0;_tmp96D;});struct Cyc_Absyn_Kind*_tmpED8=expected_kind;int _tmpED7=allow_abs_aggr;Cyc_Tcutil_check_valid_type(_tmpEDB,_tmpEDA,_tmpED9,_tmpED8,_tmpED7,t);});
# 4874
struct Cyc_List_List*_tmp966=({struct _RegionHandle*_tmpEDD=Cyc_Core_heap_region;struct Cyc_List_List*_tmpEDC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct _tuple28*),struct Cyc_List_List*x))Cyc_List_map)((struct Cyc_Absyn_Tvar*(*)(struct _tuple28*))Cyc_Core_fst,_tmp965.free_vars);Cyc_Tcutil_remove_bound_tvars(_tmpEDD,_tmpEDC,bound_tvars);});
# 4876
struct Cyc_List_List*_tmp967=((struct Cyc_List_List*(*)(void*(*f)(struct _tuple29*),struct Cyc_List_List*x))Cyc_List_map)((void*(*)(struct _tuple29*))Cyc_Core_fst,_tmp965.free_evars);
{struct Cyc_List_List*fs=_tmp966;for(0;fs != 0;fs=fs->tl){
struct _dyneither_ptr*_tmp968=((struct Cyc_Absyn_Tvar*)fs->hd)->name;
({struct Cyc_String_pa_PrintArg_struct _tmp96C;_tmp96C.tag=0U;({struct _dyneither_ptr _tmpEDE=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp96C.f1=_tmpEDE;});({struct Cyc_String_pa_PrintArg_struct _tmp96B;_tmp96B.tag=0U;_tmp96B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp968);({void*_tmp969[2U]={& _tmp96B,& _tmp96C};({unsigned int _tmpEE0=loc;struct _dyneither_ptr _tmpEDF=({const char*_tmp96A="unbound type variable %s in type %s";_tag_dyneither(_tmp96A,sizeof(char),36U);});Cyc_Tcutil_terr(_tmpEE0,_tmpEDF,_tag_dyneither(_tmp969,sizeof(void*),2U));});});});});}}
# 4881
if(!allow_evars)
Cyc_Tcutil_check_free_evars(_tmp967,t,loc);}
# 4885
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmpEE1=Cyc_Tcutil_new_tvar_id();tv->identity=_tmpEE1;});}
# 4890
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 4896
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(void*),struct _dyneither_ptr msg){
# 4899
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp971;_tmp971.tag=0U;({struct _dyneither_ptr _tmpEE2=(struct _dyneither_ptr)((struct _dyneither_ptr)a2string(vs->hd));_tmp971.f1=_tmpEE2;});({struct Cyc_String_pa_PrintArg_struct _tmp970;_tmp970.tag=0U;_tmp970.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)msg);({void*_tmp96E[2U]={& _tmp970,& _tmp971};({unsigned int _tmpEE4=loc;struct _dyneither_ptr _tmpEE3=({const char*_tmp96F="%s: %s";_tag_dyneither(_tmp96F,sizeof(char),7U);});Cyc_Tcutil_terr(_tmpEE4,_tmpEE3,_tag_dyneither(_tmp96E,sizeof(void*),2U));});});});});}}}
# 4905
static struct _dyneither_ptr Cyc_Tcutil_strptr2string(struct _dyneither_ptr*s){
return*s;}
# 4909
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr msg){
((void(*)(int(*cmp)(struct _dyneither_ptr*,struct _dyneither_ptr*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct _dyneither_ptr*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 4913
void Cyc_Tcutil_check_unique_tvars(unsigned int loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmpEE6=tvs;unsigned int _tmpEE5=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned int loc,struct _dyneither_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _dyneither_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmpEE6,_tmpEE5,Cyc_Absynpp_tvar2string,({const char*_tmp972="duplicate type variable";_tag_dyneither(_tmp972,sizeof(char),24U);}));});}struct _tuple32{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple33{struct Cyc_List_List*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 4927 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 4932
struct _RegionHandle _tmp973=_new_region("temp");struct _RegionHandle*temp=& _tmp973;_push_region(temp);
# 4936
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _dyneither_ptr _tmpEE7=(struct _dyneither_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmpEE7,({const char*_tmp974="";_tag_dyneither(_tmp974,sizeof(char),1U);}));})!= 0)
({struct Cyc_List_List*_tmpEE9=({struct Cyc_List_List*_tmp975=_region_malloc(temp,sizeof(*_tmp975));({struct _tuple32*_tmpEE8=({struct _tuple32*_tmp976=_region_malloc(temp,sizeof(*_tmp976));_tmp976->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd;_tmp976->f2=0;_tmp976;});_tmp975->hd=_tmpEE8;});_tmp975->tl=fields;_tmp975;});fields=_tmpEE9;});}}
# 4941
({struct Cyc_List_List*_tmpEEA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);fields=_tmpEEA;});{
# 4943
struct _dyneither_ptr aggr_str=aggr_kind == Cyc_Absyn_StructA?({const char*_tmp99C="struct";_tag_dyneither(_tmp99C,sizeof(char),7U);}):({const char*_tmp99D="union";_tag_dyneither(_tmp99D,sizeof(char),6U);});
# 4946
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple33*_tmp977=(struct _tuple33*)des->hd;struct _tuple33*_tmp978=_tmp977;struct Cyc_List_List*_tmp994;void*_tmp993;_LL1: _tmp994=_tmp978->f1;_tmp993=_tmp978->f2;_LL2:;
if(_tmp994 == 0){
# 4951
struct Cyc_List_List*_tmp979=fields;
for(0;_tmp979 != 0;_tmp979=_tmp979->tl){
if(!(*((struct _tuple32*)_tmp979->hd)).f2){
(*((struct _tuple32*)_tmp979->hd)).f2=1;
({struct Cyc_List_List*_tmpEED=({struct Cyc_List_List*_tmp97A=_cycalloc(sizeof(*_tmp97A));({void*_tmpEEC=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp97B=_cycalloc(sizeof(*_tmp97B));({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmpEEB=({struct Cyc_Absyn_FieldName_Absyn_Designator_struct _tmp97C;_tmp97C.tag=1U;_tmp97C.f1=((*((struct _tuple32*)_tmp979->hd)).f1)->name;_tmp97C;});_tmp97B[0]=_tmpEEB;});_tmp97B;});_tmp97A->hd=_tmpEEC;});_tmp97A->tl=0;_tmp97A;});(*((struct _tuple33*)des->hd)).f1=_tmpEED;});
({struct Cyc_List_List*_tmpEEF=({struct Cyc_List_List*_tmp97D=_region_malloc(rgn,sizeof(*_tmp97D));({struct _tuple34*_tmpEEE=({struct _tuple34*_tmp97E=_region_malloc(rgn,sizeof(*_tmp97E));_tmp97E->f1=(*((struct _tuple32*)_tmp979->hd)).f1;_tmp97E->f2=_tmp993;_tmp97E;});_tmp97D->hd=_tmpEEE;});_tmp97D->tl=ans;_tmp97D;});ans=_tmpEEF;});
break;}}
# 4959
if(_tmp979 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp981;_tmp981.tag=0U;_tmp981.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp97F[1U]={& _tmp981};({unsigned int _tmpEF1=loc;struct _dyneither_ptr _tmpEF0=({const char*_tmp980="too many arguments to %s";_tag_dyneither(_tmp980,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpEF1,_tmpEF0,_tag_dyneither(_tmp97F,sizeof(void*),1U));});});});}else{
if(_tmp994->tl != 0)
# 4963
({void*_tmp982=0U;({unsigned int _tmpEF3=loc;struct _dyneither_ptr _tmpEF2=({const char*_tmp983="multiple designators are not yet supported";_tag_dyneither(_tmp983,sizeof(char),43U);});Cyc_Tcutil_terr(_tmpEF3,_tmpEF2,_tag_dyneither(_tmp982,sizeof(void*),0U));});});else{
# 4966
void*_tmp984=(void*)_tmp994->hd;void*_tmp985=_tmp984;struct _dyneither_ptr*_tmp992;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp985)->tag == 0U){_LL4: _LL5:
# 4968
({struct Cyc_String_pa_PrintArg_struct _tmp988;_tmp988.tag=0U;_tmp988.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)aggr_str);({void*_tmp986[1U]={& _tmp988};({unsigned int _tmpEF5=loc;struct _dyneither_ptr _tmpEF4=({const char*_tmp987="array designator used in argument to %s";_tag_dyneither(_tmp987,sizeof(char),40U);});Cyc_Tcutil_terr(_tmpEF5,_tmpEF4,_tag_dyneither(_tmp986,sizeof(void*),1U));});});});
goto _LL3;}else{_LL6: _tmp992=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp985)->f1;_LL7: {
# 4971
struct Cyc_List_List*_tmp989=fields;
for(0;_tmp989 != 0;_tmp989=_tmp989->tl){
if(Cyc_strptrcmp(_tmp992,((*((struct _tuple32*)_tmp989->hd)).f1)->name)== 0){
if((*((struct _tuple32*)_tmp989->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp98C;_tmp98C.tag=0U;_tmp98C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp992);({void*_tmp98A[1U]={& _tmp98C};({unsigned int _tmpEF7=loc;struct _dyneither_ptr _tmpEF6=({const char*_tmp98B="member %s has already been used as an argument";_tag_dyneither(_tmp98B,sizeof(char),47U);});Cyc_Tcutil_terr(_tmpEF7,_tmpEF6,_tag_dyneither(_tmp98A,sizeof(void*),1U));});});});
(*((struct _tuple32*)_tmp989->hd)).f2=1;
({struct Cyc_List_List*_tmpEF9=({struct Cyc_List_List*_tmp98D=_region_malloc(rgn,sizeof(*_tmp98D));({struct _tuple34*_tmpEF8=({struct _tuple34*_tmp98E=_region_malloc(rgn,sizeof(*_tmp98E));_tmp98E->f1=(*((struct _tuple32*)_tmp989->hd)).f1;_tmp98E->f2=_tmp993;_tmp98E;});_tmp98D->hd=_tmpEF8;});_tmp98D->tl=ans;_tmp98D;});ans=_tmpEF9;});
break;}}
# 4980
if(_tmp989 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp991;_tmp991.tag=0U;_tmp991.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp992);({void*_tmp98F[1U]={& _tmp991};({unsigned int _tmpEFB=loc;struct _dyneither_ptr _tmpEFA=({const char*_tmp990="bad field designator %s";_tag_dyneither(_tmp990,sizeof(char),24U);});Cyc_Tcutil_terr(_tmpEFB,_tmpEFA,_tag_dyneither(_tmp98F,sizeof(void*),1U));});});});
goto _LL3;}}_LL3:;}}}
# 4985
if(aggr_kind == Cyc_Absyn_StructA)
# 4987
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple32*)fields->hd)).f2){
({void*_tmp995=0U;({unsigned int _tmpEFD=loc;struct _dyneither_ptr _tmpEFC=({const char*_tmp996="too few arguments to struct";_tag_dyneither(_tmp996,sizeof(char),28U);});Cyc_Tcutil_terr(_tmpEFD,_tmpEFC,_tag_dyneither(_tmp995,sizeof(void*),0U));});});
break;}}else{
# 4994
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple32*)fields->hd)).f2){
if(found)({void*_tmp997=0U;({unsigned int _tmpEFF=loc;struct _dyneither_ptr _tmpEFE=({const char*_tmp998="only one member of a union is allowed";_tag_dyneither(_tmp998,sizeof(char),38U);});Cyc_Tcutil_terr(_tmpEFF,_tmpEFE,_tag_dyneither(_tmp997,sizeof(void*),0U));});});
found=1;}}
# 5001
if(!found)({void*_tmp999=0U;({unsigned int _tmpF01=loc;struct _dyneither_ptr _tmpF00=({const char*_tmp99A="missing member for union";_tag_dyneither(_tmp99A,sizeof(char),25U);});Cyc_Tcutil_terr(_tmpF01,_tmpF00,_tag_dyneither(_tmp999,sizeof(void*),0U));});});}{
# 5004
struct Cyc_List_List*_tmp99B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp99B;};};}
# 4936
;_pop_region(temp);}
# 5007
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp99E=Cyc_Tcutil_compress(t);void*_tmp99F=_tmp99E;void*_tmp9A3;union Cyc_Absyn_Constraint*_tmp9A2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99F)->tag == 5U){_LL1: _tmp9A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99F)->f1).elt_typ;_tmp9A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp99F)->f1).ptr_atts).bounds;_LL2: {
# 5010
void*_tmp9A0=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)((void*)& Cyc_Absyn_DynEither_b_val,_tmp9A2);void*_tmp9A1=_tmp9A0;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9A1)->tag == 0U){_LL6: _LL7:
# 5012
*elt_typ_dest=_tmp9A3;
return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 5016
 return 0;}_LL0:;}
# 5020
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest){
void*_tmp9A4=Cyc_Tcutil_compress(t);void*_tmp9A5=_tmp9A4;void*_tmp9A7;union Cyc_Absyn_Constraint*_tmp9A6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A5)->tag == 5U){_LL1: _tmp9A7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A5)->f1).elt_typ;_tmp9A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A5)->f1).ptr_atts).zero_term;_LL2:
# 5023
*elt_typ_dest=_tmp9A7;
return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9A6);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5031
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_dyneither,void**elt_type){
# 5033
void*_tmp9A8=Cyc_Tcutil_compress(t);void*_tmp9A9=_tmp9A8;void*_tmp9B2;struct Cyc_Absyn_Tqual _tmp9B1;struct Cyc_Absyn_Exp*_tmp9B0;union Cyc_Absyn_Constraint*_tmp9AF;void*_tmp9AE;union Cyc_Absyn_Constraint*_tmp9AD;union Cyc_Absyn_Constraint*_tmp9AC;switch(*((int*)_tmp9A9)){case 5U: _LL1: _tmp9AE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A9)->f1).elt_typ;_tmp9AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A9)->f1).ptr_atts).bounds;_tmp9AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9A9)->f1).ptr_atts).zero_term;_LL2:
# 5035
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9AC)){
*ptr_type=t;
*elt_type=_tmp9AE;
{void*_tmp9AA=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp9AD);void*_tmp9AB=_tmp9AA;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp9AB)->tag == 0U){_LL8: _LL9:
*is_dyneither=1;goto _LL7;}else{_LLA: _LLB:
*is_dyneither=0;goto _LL7;}_LL7:;}
# 5042
return 1;}else{
return 0;}case 8U: _LL3: _tmp9B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A9)->f1).elt_type;_tmp9B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A9)->f1).tq;_tmp9B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A9)->f1).num_elts;_tmp9AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp9A9)->f1).zero_term;_LL4:
# 5045
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp9AF)){
*elt_type=_tmp9B2;
*is_dyneither=0;
({void*_tmpF02=Cyc_Tcutil_promote_array(t,(void*)& Cyc_Absyn_HeapRgn_val,0);*ptr_type=_tmpF02;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 5058
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type){
# 5060
void*_tmp9B3=e1->r;void*_tmp9B4=_tmp9B3;struct Cyc_Absyn_Exp*_tmp9C6;struct Cyc_Absyn_Exp*_tmp9C5;struct Cyc_Absyn_Exp*_tmp9C4;struct Cyc_Absyn_Exp*_tmp9C3;struct Cyc_Absyn_Exp*_tmp9C2;struct Cyc_Absyn_Exp*_tmp9C1;switch(*((int*)_tmp9B4)){case 14U: _LL1: _LL2:
# 5062
({struct Cyc_String_pa_PrintArg_struct _tmp9B7;_tmp9B7.tag=0U;({struct _dyneither_ptr _tmpF03=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9B7.f1=_tmpF03;});({void*_tmp9B5[1U]={& _tmp9B7};({struct _dyneither_ptr _tmpF04=({const char*_tmp9B6="we have a cast in a lhs:  %s";_tag_dyneither(_tmp9B6,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF04,_tag_dyneither(_tmp9B5,sizeof(void*),1U));});});});case 20U: _LL3: _tmp9C1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9B4)->f1;_LL4:
 _tmp9C2=_tmp9C1;goto _LL6;case 23U: _LL5: _tmp9C2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9B4)->f1;_LL6:
# 5065
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9C2->topt),ptr_type,is_dyneither,elt_type);case 22U: _LL7: _tmp9C3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9B4)->f1;_LL8:
 _tmp9C4=_tmp9C3;goto _LLA;case 21U: _LL9: _tmp9C4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9B4)->f1;_LLA:
# 5069
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9C4->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9BA;_tmp9BA.tag=0U;({struct _dyneither_ptr _tmpF05=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));_tmp9BA.f1=_tmpF05;});({void*_tmp9B8[1U]={& _tmp9BA};({struct _dyneither_ptr _tmpF06=({const char*_tmp9B9="found zero pointer aggregate member assignment: %s";_tag_dyneither(_tmp9B9,sizeof(char),51U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF06,_tag_dyneither(_tmp9B8,sizeof(void*),1U));});});});
return 0;case 13U: _LLB: _tmp9C5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp9B4)->f1;_LLC:
 _tmp9C6=_tmp9C5;goto _LLE;case 12U: _LLD: _tmp9C6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp9B4)->f1;_LLE:
# 5075
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp9C6->topt),ptr_type,is_dyneither,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp9BD;_tmp9BD.tag=0U;({struct _dyneither_ptr _tmpF07=(struct _dyneither_ptr)((struct _dyneither_ptr)
Cyc_Absynpp_exp2string(e1));_tmp9BD.f1=_tmpF07;});({void*_tmp9BB[1U]={& _tmp9BD};({struct _dyneither_ptr _tmpF08=({const char*_tmp9BC="found zero pointer instantiate/noinstantiate: %s";_tag_dyneither(_tmp9BC,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF08,_tag_dyneither(_tmp9BB,sizeof(void*),1U));});});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp9C0;_tmp9C0.tag=0U;({struct _dyneither_ptr _tmpF09=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e1));_tmp9C0.f1=_tmpF09;});({void*_tmp9BE[1U]={& _tmp9C0};({struct _dyneither_ptr _tmpF0A=({const char*_tmp9BF="found bad lhs in is_zero_ptr_deref: %s";_tag_dyneither(_tmp9BF,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF0A,_tag_dyneither(_tmp9BE,sizeof(void*),1U));});});});}_LL0:;}
# 5084
int Cyc_Tcutil_is_tagged_pointer_typ(void*t){
void*ignore=(void*)& Cyc_Absyn_VoidType_val;
return Cyc_Tcutil_is_tagged_pointer_typ_elt(t,& ignore);}
# 5095
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 5098
void*_tmp9C7=Cyc_Tcutil_compress(r);void*_tmp9C8=_tmp9C7;struct Cyc_Absyn_Tvar*_tmp9D5;enum Cyc_Absyn_KindQual _tmp9D4;enum Cyc_Absyn_AliasQual _tmp9D3;switch(*((int*)_tmp9C8)){case 22U: _LL1: _LL2:
 return !must_be_unique;case 21U: _LL3: _LL4:
 return 1;case 17U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C8)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C8)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C8)->f4 == 0){_LL5: _tmp9D4=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C8)->f3)->kind)->v)->kind;_tmp9D3=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp9C8)->f3)->kind)->v)->aliasqual;_LL6:
# 5102
 return _tmp9D4 == Cyc_Absyn_RgnKind  && (_tmp9D3 == Cyc_Absyn_Unique  || _tmp9D3 == Cyc_Absyn_Top  && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp9D5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9C8)->f1;_LL8: {
# 5106
struct Cyc_Absyn_Kind*_tmp9C9=Cyc_Tcutil_tvar_kind(_tmp9D5,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp9CA=_tmp9C9;enum Cyc_Absyn_KindQual _tmp9D2;enum Cyc_Absyn_AliasQual _tmp9D1;_LLC: _tmp9D2=_tmp9CA->kind;_tmp9D1=_tmp9CA->aliasqual;_LLD:;
if(_tmp9D2 == Cyc_Absyn_RgnKind  && (_tmp9D1 == Cyc_Absyn_Unique  || _tmp9D1 == Cyc_Absyn_Top  && !must_be_unique)){
void*_tmp9CB=Cyc_Absyn_compress_kb(_tmp9D5->kind);void*_tmp9CC=_tmp9CB;struct Cyc_Core_Opt**_tmp9D0;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9CC)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9CC)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9CC)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp9D0=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9CC)->f1;_LL10:
# 5110
({struct Cyc_Core_Opt*_tmpF0D=({struct Cyc_Core_Opt*_tmp9CD=_cycalloc(sizeof(*_tmp9CD));({void*_tmpF0C=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9CE=_cycalloc(sizeof(*_tmp9CE));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF0B=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9CF;_tmp9CF.tag=2U;_tmp9CF.f1=0;_tmp9CF.f2=& Cyc_Tcutil_rk;_tmp9CF;});_tmp9CE[0]=_tmpF0B;});_tmp9CE;});_tmp9CD->v=_tmpF0C;});_tmp9CD;});*_tmp9D0=_tmpF0D;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 5115
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 5122
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp9D6=Cyc_Tcutil_compress(t);void*_tmp9D7=_tmp9D6;struct Cyc_Absyn_Tvar*_tmp9E6;void*_tmp9E5;switch(*((int*)_tmp9D7)){case 5U: _LL1: _tmp9E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp9D7)->f1).ptr_atts).rgn;_LL2:
# 5125
 return Cyc_Tcutil_is_noalias_region(_tmp9E5,must_be_unique);case 2U: _LL3: _tmp9E6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp9D7)->f1;_LL4: {
# 5127
struct Cyc_Absyn_Kind*_tmp9D8=Cyc_Tcutil_tvar_kind(_tmp9E6,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp9D9=_tmp9D8;enum Cyc_Absyn_KindQual _tmp9E4;enum Cyc_Absyn_AliasQual _tmp9E3;_LL8: _tmp9E4=_tmp9D9->kind;_tmp9E3=_tmp9D9->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp9DA=_tmp9E4;switch(_tmp9DA){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if(_tmp9E3 == Cyc_Absyn_Unique  || _tmp9E3 == Cyc_Absyn_Top  && !must_be_unique){
void*_tmp9DB=Cyc_Absyn_compress_kb(_tmp9E6->kind);void*_tmp9DC=_tmp9DB;struct Cyc_Core_Opt**_tmp9E2;enum Cyc_Absyn_KindQual _tmp9E1;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DC)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DC)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp9E2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DC)->f1;_tmp9E1=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp9DC)->f2)->kind;_LL15:
# 5133
({struct Cyc_Core_Opt*_tmpF11=({struct Cyc_Core_Opt*_tmp9DD=_cycalloc(sizeof(*_tmp9DD));({void*_tmpF10=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp9DE=_cycalloc(sizeof(*_tmp9DE));({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmpF0F=({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct _tmp9DF;_tmp9DF.tag=2U;_tmp9DF.f1=0;({struct Cyc_Absyn_Kind*_tmpF0E=({struct Cyc_Absyn_Kind*_tmp9E0=_cycalloc_atomic(sizeof(*_tmp9E0));_tmp9E0->kind=_tmp9E1;_tmp9E0->aliasqual=Cyc_Absyn_Aliasable;_tmp9E0;});_tmp9DF.f2=_tmpF0E;});_tmp9DF;});_tmp9DE[0]=_tmpF0F;});_tmp9DE;});_tmp9DD->v=_tmpF10;});_tmp9DD;});*_tmp9E2=_tmpF11;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 5137
 return 1;}_LL13:;}
# 5140
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 5143
 return 0;}_LL0:;}
# 5146
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp9E7=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp9E7,0))return 1;{
void*_tmp9E8=_tmp9E7;union Cyc_Absyn_DatatypeFieldInfoU _tmp9FC;struct Cyc_List_List*_tmp9FB;union Cyc_Absyn_DatatypeInfoU _tmp9FA;struct Cyc_List_List*_tmp9F9;struct Cyc_List_List*_tmp9F8;struct Cyc_Absyn_Aggrdecl**_tmp9F7;struct Cyc_List_List*_tmp9F6;struct Cyc_List_List*_tmp9F5;switch(*((int*)_tmp9E8)){case 10U: _LL1: _tmp9F5=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp9E8)->f1;_LL2:
# 5151
 while(_tmp9F5 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp9F5->hd)).f2))return 1;
_tmp9F5=_tmp9F5->tl;}
# 5155
return 0;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E8)->f1).aggr_info).KnownAggr).tag == 2){_LL3: _tmp9F7=(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E8)->f1).aggr_info).KnownAggr).val;_tmp9F6=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp9E8)->f1).targs;_LL4:
# 5157
 if((*_tmp9F7)->impl == 0)return 0;else{
# 5159
struct Cyc_List_List*_tmp9E9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp9F7)->tvs,_tmp9F6);
struct Cyc_List_List*_tmp9EA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp9F7)->impl))->fields;
void*t;
while(_tmp9EA != 0){
({void*_tmpF12=_tmp9E9 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp9EA->hd)->type: Cyc_Tcutil_substitute(_tmp9E9,((struct Cyc_Absyn_Aggrfield*)_tmp9EA->hd)->type);t=_tmpF12;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp9EA=_tmp9EA->tl;}
# 5167
return 0;}}else{_LL7: _LL8:
# 5177
 return 0;}case 12U: _LL5: _tmp9F8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp9E8)->f2;_LL6:
# 5170
 while(_tmp9F8 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp9F8->hd)->type))return 1;
_tmp9F8=_tmp9F8->tl;}
# 5174
return 0;case 3U: _LL9: _tmp9FA=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9E8)->f1).datatype_info;_tmp9F9=(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp9E8)->f1).targs;_LLA: {
# 5179
union Cyc_Absyn_DatatypeInfoU _tmp9EB=_tmp9FA;struct Cyc_List_List*_tmp9EF;struct Cyc_Core_Opt*_tmp9EE;struct _tuple2*_tmp9ED;int _tmp9EC;if((_tmp9EB.UnknownDatatype).tag == 1){_LL10: _tmp9ED=((_tmp9EB.UnknownDatatype).val).name;_tmp9EC=((_tmp9EB.UnknownDatatype).val).is_extensible;_LL11:
# 5182
 return 0;}else{_LL12: _tmp9EF=(*(_tmp9EB.KnownDatatype).val)->tvs;_tmp9EE=(*(_tmp9EB.KnownDatatype).val)->fields;_LL13:
# 5185
 return 0;}_LLF:;}case 4U: _LLB: _tmp9FC=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9E8)->f1).field_info;_tmp9FB=(((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp9E8)->f1).targs;_LLC: {
# 5188
union Cyc_Absyn_DatatypeFieldInfoU _tmp9F0=_tmp9FC;struct Cyc_Absyn_Datatypedecl*_tmp9F4;struct Cyc_Absyn_Datatypefield*_tmp9F3;if((_tmp9F0.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 5191
 return 0;}else{_LL17: _tmp9F4=((_tmp9F0.KnownDatatypefield).val).f1;_tmp9F3=((_tmp9F0.KnownDatatypefield).val).f2;_LL18: {
# 5193
struct Cyc_List_List*_tmp9F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp9F4->tvs,_tmp9FB);
struct Cyc_List_List*_tmp9F2=_tmp9F3->typs;
while(_tmp9F2 != 0){
({void*_tmpF13=_tmp9F1 == 0?(*((struct _tuple12*)_tmp9F2->hd)).f2: Cyc_Tcutil_substitute(_tmp9F1,(*((struct _tuple12*)_tmp9F2->hd)).f2);_tmp9E7=_tmpF13;});
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp9E7))return 1;
_tmp9F2=_tmp9F2->tl;}
# 5200
return 0;}}_LL14:;}default: _LLD: _LLE:
# 5202
 return 0;}_LL0:;};}
# 5209
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp9FD=e->r;void*_tmp9FE=_tmp9FD;struct Cyc_Absyn_Stmt*_tmpA14;struct Cyc_Absyn_Exp*_tmpA13;struct Cyc_Absyn_Exp*_tmpA12;struct Cyc_Absyn_Exp*_tmpA11;struct Cyc_Absyn_Exp*_tmpA10;struct Cyc_Absyn_Exp*_tmpA0F;struct Cyc_Absyn_Exp*_tmpA0E;struct Cyc_Absyn_Exp*_tmpA0D;struct _dyneither_ptr*_tmpA0C;struct Cyc_Absyn_Exp*_tmpA0B;struct Cyc_Absyn_Exp*_tmpA0A;switch(*((int*)_tmp9FE)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp9FE)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmpA0A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp9FE)->f1;_LL4:
 _tmpA0B=_tmpA0A;goto _LL6;case 20U: _LL5: _tmpA0B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp9FE)->f1;_LL6:
# 5214
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmpA0B->topt),1) && Cyc_Tcutil_is_noalias_path(_tmpA0B);case 21U: _LL7: _tmpA0D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9FE)->f1;_tmpA0C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp9FE)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmpA0D);case 23U: _LL9: _tmpA0F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9FE)->f1;_tmpA0E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp9FE)->f2;_LLA: {
# 5217
void*_tmp9FF=Cyc_Tcutil_compress((void*)_check_null(_tmpA0F->topt));void*_tmpA00=_tmp9FF;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA00)->tag == 10U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmpA0F);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmpA11=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9FE)->f2;_tmpA10=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp9FE)->f3;_LLC:
# 5222
 return Cyc_Tcutil_is_noalias_path(_tmpA11) && Cyc_Tcutil_is_noalias_path(_tmpA10);case 9U: _LLD: _tmpA12=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp9FE)->f2;_LLE:
 _tmpA13=_tmpA12;goto _LL10;case 14U: _LLF: _tmpA13=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp9FE)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmpA13);case 37U: _LL11: _tmpA14=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp9FE)->f1;_LL12:
# 5226
 while(1){
void*_tmpA01=_tmpA14->r;void*_tmpA02=_tmpA01;struct Cyc_Absyn_Exp*_tmpA09;struct Cyc_Absyn_Decl*_tmpA08;struct Cyc_Absyn_Stmt*_tmpA07;struct Cyc_Absyn_Stmt*_tmpA06;struct Cyc_Absyn_Stmt*_tmpA05;switch(*((int*)_tmpA02)){case 2U: _LL1B: _tmpA06=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA02)->f1;_tmpA05=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmpA02)->f2;_LL1C:
 _tmpA14=_tmpA05;goto _LL1A;case 12U: _LL1D: _tmpA08=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA02)->f1;_tmpA07=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmpA02)->f2;_LL1E:
 _tmpA14=_tmpA07;goto _LL1A;case 1U: _LL1F: _tmpA09=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmpA02)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmpA09);default: _LL21: _LL22:
({void*_tmpA03=0U;({struct _dyneither_ptr _tmpF14=({const char*_tmpA04="is_noalias_stmt_exp: ill-formed StmtExp";_tag_dyneither(_tmpA04,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF14,_tag_dyneither(_tmpA03,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 5234
 return 1;}_LL0:;}
# 5251 "tcutil.cyc"
struct _tuple18 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 5253
struct _tuple18 bogus_ans=({struct _tuple18 _tmpA5C;_tmpA5C.f1=0;_tmpA5C.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA5C;});
void*_tmpA15=e->r;void*_tmpA16=_tmpA15;struct Cyc_Absyn_Exp*_tmpA5B;struct Cyc_Absyn_Exp*_tmpA5A;struct Cyc_Absyn_Exp*_tmpA59;struct Cyc_Absyn_Exp*_tmpA58;struct _dyneither_ptr*_tmpA57;int _tmpA56;struct Cyc_Absyn_Exp*_tmpA55;struct _dyneither_ptr*_tmpA54;int _tmpA53;void*_tmpA52;switch(*((int*)_tmpA16)){case 1U: _LL1: _tmpA52=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA16)->f1;_LL2: {
# 5257
void*_tmpA17=_tmpA52;struct Cyc_Absyn_Vardecl*_tmpA24;struct Cyc_Absyn_Vardecl*_tmpA23;struct Cyc_Absyn_Vardecl*_tmpA22;struct Cyc_Absyn_Vardecl*_tmpA21;switch(*((int*)_tmpA17)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmpA21=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA17)->f1;_LL13: {
# 5261
void*_tmpA18=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA19=_tmpA18;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA19)->tag == 8U){_LL1B: _LL1C:
# 5263
 return({struct _tuple18 _tmpA1A;_tmpA1A.f1=1;_tmpA1A.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA1A;});}else{_LL1D: _LL1E:
 return({struct _tuple18 _tmpA1B;_tmpA1B.f1=(_tmpA21->tq).real_const;_tmpA1B.f2=(void*)& Cyc_Absyn_HeapRgn_val;_tmpA1B;});}_LL1A:;}case 4U: _LL14: _tmpA22=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA17)->f1;_LL15: {
# 5267
void*_tmpA1C=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmpA1D=_tmpA1C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA1D)->tag == 8U){_LL20: _LL21:
 return({struct _tuple18 _tmpA1E;_tmpA1E.f1=1;_tmpA1E.f2=(void*)_check_null(_tmpA22->rgn);_tmpA1E;});}else{_LL22: _LL23:
# 5270
 _tmpA22->escapes=1;
return({struct _tuple18 _tmpA1F;_tmpA1F.f1=(_tmpA22->tq).real_const;_tmpA1F.f2=(void*)_check_null(_tmpA22->rgn);_tmpA1F;});}_LL1F:;}case 5U: _LL16: _tmpA23=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpA17)->f1;_LL17:
# 5273
 _tmpA24=_tmpA23;goto _LL19;default: _LL18: _tmpA24=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpA17)->f1;_LL19:
# 5275
 _tmpA24->escapes=1;
return({struct _tuple18 _tmpA20;_tmpA20.f1=(_tmpA24->tq).real_const;_tmpA20.f2=(void*)_check_null(_tmpA24->rgn);_tmpA20;});}_LLD:;}case 21U: _LL3: _tmpA55=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA16)->f1;_tmpA54=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA16)->f2;_tmpA53=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA16)->f3;_LL4:
# 5280
 if(_tmpA53)return bogus_ans;{
# 5283
void*_tmpA25=Cyc_Tcutil_compress((void*)_check_null(_tmpA55->topt));void*_tmpA26=_tmpA25;struct Cyc_Absyn_Aggrdecl*_tmpA34;struct Cyc_List_List*_tmpA33;switch(*((int*)_tmpA26)){case 12U: _LL25: _tmpA33=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA26)->f2;_LL26: {
# 5285
struct Cyc_Absyn_Aggrfield*_tmpA27=Cyc_Absyn_lookup_field(_tmpA33,_tmpA54);
if(_tmpA27 != 0  && _tmpA27->width == 0){
struct _tuple18 _tmpA28=Cyc_Tcutil_addressof_props(te,_tmpA55);struct _tuple18 _tmpA29=_tmpA28;int _tmpA2C;void*_tmpA2B;_LL2C: _tmpA2C=_tmpA29.f1;_tmpA2B=_tmpA29.f2;_LL2D:;
return({struct _tuple18 _tmpA2A;_tmpA2A.f1=(_tmpA27->tq).real_const  || _tmpA2C;_tmpA2A.f2=_tmpA2B;_tmpA2A;});}
# 5290
return bogus_ans;}case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA26)->f1).aggr_info).KnownAggr).tag == 2){_LL27: _tmpA34=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA26)->f1).aggr_info).KnownAggr).val;_LL28: {
# 5292
struct Cyc_Absyn_Aggrfield*_tmpA2D=Cyc_Absyn_lookup_decl_field(_tmpA34,_tmpA54);
if(_tmpA2D != 0  && _tmpA2D->width == 0){
struct _tuple18 _tmpA2E=Cyc_Tcutil_addressof_props(te,_tmpA55);struct _tuple18 _tmpA2F=_tmpA2E;int _tmpA32;void*_tmpA31;_LL2F: _tmpA32=_tmpA2F.f1;_tmpA31=_tmpA2F.f2;_LL30:;
return({struct _tuple18 _tmpA30;_tmpA30.f1=(_tmpA2D->tq).real_const  || _tmpA32;_tmpA30.f2=_tmpA31;_tmpA30;});}
# 5297
return bogus_ans;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmpA58=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA16)->f1;_tmpA57=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA16)->f2;_tmpA56=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA16)->f3;_LL6:
# 5302
 if(_tmpA56)return bogus_ans;{
# 5305
void*_tmpA35=Cyc_Tcutil_compress((void*)_check_null(_tmpA58->topt));void*_tmpA36=_tmpA35;void*_tmpA3D;void*_tmpA3C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA36)->tag == 5U){_LL32: _tmpA3D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA36)->f1).elt_typ;_tmpA3C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA36)->f1).ptr_atts).rgn;_LL33: {
# 5307
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmpA37=Cyc_Tcutil_compress(_tmpA3D);void*_tmpA38=_tmpA37;struct Cyc_Absyn_Aggrdecl*_tmpA3A;struct Cyc_List_List*_tmpA39;switch(*((int*)_tmpA38)){case 12U: _LL37: _tmpA39=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA38)->f2;_LL38:
# 5310
({struct Cyc_Absyn_Aggrfield*_tmpF15=Cyc_Absyn_lookup_field(_tmpA39,_tmpA57);finfo=_tmpF15;});goto _LL36;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA38)->f1).aggr_info).KnownAggr).tag == 2){_LL39: _tmpA3A=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA38)->f1).aggr_info).KnownAggr).val;_LL3A:
# 5312
({struct Cyc_Absyn_Aggrfield*_tmpF16=Cyc_Absyn_lookup_decl_field(_tmpA3A,_tmpA57);finfo=_tmpF16;});goto _LL36;}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 5315
if(finfo != 0  && finfo->width == 0)
return({struct _tuple18 _tmpA3B;_tmpA3B.f1=(finfo->tq).real_const;_tmpA3B.f2=_tmpA3C;_tmpA3B;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmpA59=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA16)->f1;_LL8: {
# 5322
void*_tmpA3E=Cyc_Tcutil_compress((void*)_check_null(_tmpA59->topt));void*_tmpA3F=_tmpA3E;struct Cyc_Absyn_Tqual _tmpA42;void*_tmpA41;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3F)->tag == 5U){_LL3E: _tmpA42=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3F)->f1).elt_tq;_tmpA41=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA3F)->f1).ptr_atts).rgn;_LL3F:
# 5324
 return({struct _tuple18 _tmpA40;_tmpA40.f1=_tmpA42.real_const;_tmpA40.f2=_tmpA41;_tmpA40;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmpA5B=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA16)->f1;_tmpA5A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA16)->f2;_LLA: {
# 5330
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmpA5B->topt));
void*_tmpA43=t;struct Cyc_Absyn_Tqual _tmpA4F;struct Cyc_Absyn_Tqual _tmpA4E;void*_tmpA4D;struct Cyc_List_List*_tmpA4C;switch(*((int*)_tmpA43)){case 10U: _LL43: _tmpA4C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA43)->f1;_LL44: {
# 5334
struct _tuple14 _tmpA44=Cyc_Evexp_eval_const_uint_exp(_tmpA5A);struct _tuple14 _tmpA45=_tmpA44;unsigned int _tmpA49;int _tmpA48;_LL4C: _tmpA49=_tmpA45.f1;_tmpA48=_tmpA45.f2;_LL4D:;
if(!_tmpA48)
return bogus_ans;{
struct _tuple12*_tmpA46=Cyc_Absyn_lookup_tuple_field(_tmpA4C,(int)_tmpA49);
if(_tmpA46 != 0)
return({struct _tuple18 _tmpA47;_tmpA47.f1=((*_tmpA46).f1).real_const;({void*_tmpF17=(Cyc_Tcutil_addressof_props(te,_tmpA5B)).f2;_tmpA47.f2=_tmpF17;});_tmpA47;});
return bogus_ans;};}case 5U: _LL45: _tmpA4E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA43)->f1).elt_tq;_tmpA4D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpA43)->f1).ptr_atts).rgn;_LL46:
# 5342
 return({struct _tuple18 _tmpA4A;_tmpA4A.f1=_tmpA4E.real_const;_tmpA4A.f2=_tmpA4D;_tmpA4A;});case 8U: _LL47: _tmpA4F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA43)->f1).tq;_LL48:
# 5348
 return({struct _tuple18 _tmpA4B;_tmpA4B.f1=_tmpA4F.real_const;({void*_tmpF18=(Cyc_Tcutil_addressof_props(te,_tmpA5B)).f2;_tmpA4B.f2=_tmpF18;});_tmpA4B;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 5352
({void*_tmpA50=0U;({unsigned int _tmpF1A=e->loc;struct _dyneither_ptr _tmpF19=({const char*_tmpA51="unary & applied to non-lvalue";_tag_dyneither(_tmpA51,sizeof(char),30U);});Cyc_Tcutil_terr(_tmpF1A,_tmpF19,_tag_dyneither(_tmpA50,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 5359
void Cyc_Tcutil_check_bound(unsigned int loc,unsigned int i,union Cyc_Absyn_Constraint*b){
({union Cyc_Absyn_Constraint*_tmpF1B=((union Cyc_Absyn_Constraint*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_compress_conref)(b);b=_tmpF1B;});{
void*_tmpA5D=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(Cyc_Absyn_bounds_one,b);void*_tmpA5E=_tmpA5D;struct Cyc_Absyn_Exp*_tmpA67;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpA5E)->tag == 0U){_LL1: _LL2:
 return;}else{_LL3: _tmpA67=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA5E)->f1;_LL4: {
# 5364
struct _tuple14 _tmpA5F=Cyc_Evexp_eval_const_uint_exp(_tmpA67);struct _tuple14 _tmpA60=_tmpA5F;unsigned int _tmpA66;int _tmpA65;_LL6: _tmpA66=_tmpA60.f1;_tmpA65=_tmpA60.f2;_LL7:;
if(_tmpA65  && _tmpA66 <= i)
({struct Cyc_Int_pa_PrintArg_struct _tmpA64;_tmpA64.tag=1U;_tmpA64.f1=(unsigned long)((int)i);({struct Cyc_Int_pa_PrintArg_struct _tmpA63;_tmpA63.tag=1U;_tmpA63.f1=(unsigned long)((int)_tmpA66);({void*_tmpA61[2U]={& _tmpA63,& _tmpA64};({unsigned int _tmpF1D=loc;struct _dyneither_ptr _tmpF1C=({const char*_tmpA62="dereference is out of bounds: %d <= %d";_tag_dyneither(_tmpA62,sizeof(char),39U);});Cyc_Tcutil_terr(_tmpF1D,_tmpF1C,_tag_dyneither(_tmpA61,sizeof(void*),2U));});});});});
return;}}_LL0:;};}
# 5371
void Cyc_Tcutil_check_nonzero_bound(unsigned int loc,union Cyc_Absyn_Constraint*b){
Cyc_Tcutil_check_bound(loc,0U,b);}
# 5375
int Cyc_Tcutil_is_bound_one(union Cyc_Absyn_Constraint*b){
void*_tmpA68=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,b);void*_tmpA69=_tmpA68;struct Cyc_Absyn_Exp*_tmpA6E;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA69)->tag == 1U){_LL1: _tmpA6E=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpA69)->f1;_LL2: {
# 5378
struct _tuple14 _tmpA6A=Cyc_Evexp_eval_const_uint_exp(_tmpA6E);struct _tuple14 _tmpA6B=_tmpA6A;unsigned int _tmpA6D;int _tmpA6C;_LL6: _tmpA6D=_tmpA6B.f1;_tmpA6C=_tmpA6B.f2;_LL7:;
return _tmpA6C  && _tmpA6D == 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5384
int Cyc_Tcutil_bits_only(void*t){
void*_tmpA6F=Cyc_Tcutil_compress(t);void*_tmpA70=_tmpA6F;struct Cyc_List_List*_tmpA7A;struct Cyc_Absyn_Aggrdecl*_tmpA79;struct Cyc_List_List*_tmpA78;struct Cyc_List_List*_tmpA77;void*_tmpA76;union Cyc_Absyn_Constraint*_tmpA75;switch(*((int*)_tmpA70)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 return 0;case 8U: _LLB: _tmpA76=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA70)->f1).elt_type;_tmpA75=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpA70)->f1).zero_term;_LLC:
# 5394
 return !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpA75) && Cyc_Tcutil_bits_only(_tmpA76);case 10U: _LLD: _tmpA77=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpA70)->f1;_LLE:
# 5396
 for(0;_tmpA77 != 0;_tmpA77=_tmpA77->tl){
if(!Cyc_Tcutil_bits_only((*((struct _tuple12*)_tmpA77->hd)).f2))return 0;}
return 1;case 11U: if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA70)->f1).aggr_info).UnknownAggr).tag == 1){_LLF: _LL10:
 return 0;}else{_LL11: _tmpA79=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA70)->f1).aggr_info).KnownAggr).val;_tmpA78=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpA70)->f1).targs;_LL12:
# 5401
 if(_tmpA79->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA79->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmpA71=_new_region("rgn");struct _RegionHandle*rgn=& _tmpA71;_push_region(rgn);
{struct Cyc_List_List*_tmpA72=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmpA79->tvs,_tmpA78);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpA79->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_bits_only(Cyc_Tcutil_rsubstitute(rgn,_tmpA72,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpA73=0;_npop_handler(0U);return _tmpA73;}}}{
int _tmpA74=1;_npop_handler(0U);return _tmpA74;};}
# 5408
;_pop_region(rgn);};};}case 12U: _LL13: _tmpA7A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpA70)->f2;_LL14:
# 5413
 for(0;_tmpA7A != 0;_tmpA7A=_tmpA7A->tl){
if(!Cyc_Tcutil_bits_only(((struct Cyc_Absyn_Aggrfield*)_tmpA7A->hd)->type))return 0;}
return 1;default: _LL15: _LL16:
 return 0;}_LL0:;}
# 5421
int Cyc_Tcutil_is_var_exp(struct Cyc_Absyn_Exp*e){
while(1){
void*_tmpA7B=e->r;void*_tmpA7C=_tmpA7B;struct Cyc_Absyn_Exp*_tmpA7E;struct Cyc_Absyn_Exp*_tmpA7D;switch(*((int*)_tmpA7C)){case 1U: _LL1: _LL2:
 return 1;case 12U: _LL3: _tmpA7D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA7C)->f1;_LL4:
 _tmpA7E=_tmpA7D;goto _LL6;case 13U: _LL5: _tmpA7E=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA7C)->f1;_LL6:
 e=_tmpA7E;continue;default: _LL7: _LL8:
# 5428
 return 0;}_LL0:;}}
# 5438
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmpA7F=e->r;void*_tmpA80=_tmpA7F;struct Cyc_List_List*_tmpA9E;struct Cyc_List_List*_tmpA9D;enum Cyc_Absyn_Primop _tmpA9C;struct Cyc_List_List*_tmpA9B;struct Cyc_List_List*_tmpA9A;struct Cyc_List_List*_tmpA99;struct Cyc_List_List*_tmpA98;struct Cyc_Absyn_Exp*_tmpA97;struct Cyc_Absyn_Exp*_tmpA96;struct Cyc_Absyn_Exp*_tmpA95;struct Cyc_Absyn_Exp*_tmpA94;void*_tmpA93;struct Cyc_Absyn_Exp*_tmpA92;void*_tmpA91;struct Cyc_Absyn_Exp*_tmpA90;struct Cyc_Absyn_Exp*_tmpA8F;struct Cyc_Absyn_Exp*_tmpA8E;struct Cyc_Absyn_Exp*_tmpA8D;struct Cyc_Absyn_Exp*_tmpA8C;struct Cyc_Absyn_Exp*_tmpA8B;struct Cyc_Absyn_Exp*_tmpA8A;struct Cyc_Absyn_Exp*_tmpA89;void*_tmpA88;switch(*((int*)_tmpA80)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmpA88=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_LL10: {
# 5450
void*_tmpA81=_tmpA88;struct Cyc_Absyn_Vardecl*_tmpA87;struct Cyc_Absyn_Vardecl*_tmpA86;switch(*((int*)_tmpA81)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmpA86=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpA81)->f1;_LL35: {
# 5453
void*_tmpA82=Cyc_Tcutil_compress(_tmpA86->type);void*_tmpA83=_tmpA82;switch(*((int*)_tmpA83)){case 8U: _LL3D: _LL3E:
 goto _LL40;case 9U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmpA87=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpA81)->f1;_LL37:
# 5460
 if(_tmpA87->sc == Cyc_Absyn_Static){
void*_tmpA84=Cyc_Tcutil_compress(_tmpA87->type);void*_tmpA85=_tmpA84;switch(*((int*)_tmpA85)){case 8U: _LL44: _LL45:
 goto _LL47;case 9U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}else{
# 5467
return var_okay;}case 0U: _LL38: _LL39:
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmpA8B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_tmpA8A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA80)->f2;_tmpA89=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA80)->f3;_LL12:
# 5473
 return(Cyc_Tcutil_cnst_exp(0,_tmpA8B) && 
Cyc_Tcutil_cnst_exp(0,_tmpA8A)) && 
Cyc_Tcutil_cnst_exp(0,_tmpA89);case 9U: _LL13: _tmpA8D=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_tmpA8C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA80)->f2;_LL14:
# 5477
 return Cyc_Tcutil_cnst_exp(0,_tmpA8D) && Cyc_Tcutil_cnst_exp(0,_tmpA8C);case 12U: _LL15: _tmpA8E=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_LL16:
 _tmpA8F=_tmpA8E;goto _LL18;case 13U: _LL17: _tmpA8F=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_LL18:
# 5480
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA8F);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA80)->f4 == Cyc_Absyn_No_coercion){_LL19: _tmpA91=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_tmpA90=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA80)->f2;_LL1A:
# 5482
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA90);}else{_LL1B: _tmpA93=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_tmpA92=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA80)->f2;_LL1C:
# 5485
 return Cyc_Tcutil_cnst_exp(var_okay,_tmpA92);}case 15U: _LL1D: _tmpA94=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_LL1E:
# 5487
 return Cyc_Tcutil_cnst_exp(1,_tmpA94);case 27U: _LL1F: _tmpA96=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA80)->f2;_tmpA95=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA80)->f3;_LL20:
# 5489
 return Cyc_Tcutil_cnst_exp(0,_tmpA96) && Cyc_Tcutil_cnst_exp(0,_tmpA95);case 28U: _LL21: _tmpA97=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_LL22:
# 5491
 return Cyc_Tcutil_cnst_exp(0,_tmpA97);case 26U: _LL23: _tmpA98=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_LL24:
 _tmpA99=_tmpA98;goto _LL26;case 30U: _LL25: _tmpA99=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA80)->f2;_LL26:
 _tmpA9A=_tmpA99;goto _LL28;case 29U: _LL27: _tmpA9A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA80)->f3;_LL28:
# 5495
 for(0;_tmpA9A != 0;_tmpA9A=_tmpA9A->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple19*)_tmpA9A->hd)).f2))
return 0;}
return 1;case 3U: _LL29: _tmpA9C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_tmpA9B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA80)->f2;_LL2A:
# 5500
 _tmpA9D=_tmpA9B;goto _LL2C;case 24U: _LL2B: _tmpA9D=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_LL2C:
 _tmpA9E=_tmpA9D;goto _LL2E;case 31U: _LL2D: _tmpA9E=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA80)->f1;_LL2E:
# 5503
 for(0;_tmpA9E != 0;_tmpA9E=_tmpA9E->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmpA9E->hd))
return 0;}
return 1;default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 5511
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 5515
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs);
# 5517
int Cyc_Tcutil_supports_default(void*t){
void*_tmpA9F=Cyc_Tcutil_compress(t);void*_tmpAA0=_tmpA9F;struct Cyc_List_List*_tmpAAA;union Cyc_Absyn_AggrInfoU _tmpAA9;struct Cyc_List_List*_tmpAA8;struct Cyc_List_List*_tmpAA7;void*_tmpAA6;union Cyc_Absyn_Constraint*_tmpAA5;union Cyc_Absyn_Constraint*_tmpAA4;switch(*((int*)_tmpAA0)){case 0U: _LL1: _LL2:
 goto _LL4;case 6U: _LL3: _LL4:
 goto _LL6;case 7U: _LL5: _LL6:
 return 1;case 5U: _LL7: _tmpAA5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA0)->f1).ptr_atts).nullable;_tmpAA4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAA0)->f1).ptr_atts).bounds;_LL8: {
# 5524
void*_tmpAA1=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpAA4);void*_tmpAA2=_tmpAA1;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAA2)->tag == 0U){_LL18: _LL19:
 return 1;}else{_LL1A: _LL1B:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_constr)(1,_tmpAA5);}_LL17:;}case 8U: _LL9: _tmpAA6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAA0)->f1).elt_type;_LLA:
# 5529
 return Cyc_Tcutil_supports_default(_tmpAA6);case 10U: _LLB: _tmpAA7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAA0)->f1;_LLC:
# 5531
 for(0;_tmpAA7 != 0;_tmpAA7=_tmpAA7->tl){
if(!Cyc_Tcutil_supports_default((*((struct _tuple12*)_tmpAA7->hd)).f2))return 0;}
return 1;case 11U: _LLD: _tmpAA9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA0)->f1).aggr_info;_tmpAA8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAA0)->f1).targs;_LLE: {
# 5535
struct Cyc_Absyn_Aggrdecl*_tmpAA3=Cyc_Absyn_get_known_aggrdecl(_tmpAA9);
if(_tmpAA3->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAA3->impl))->exist_vars != 0)return 0;
return Cyc_Tcutil_fields_support_default(_tmpAA3->tvs,_tmpAA8,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAA3->impl))->fields);}case 12U: _LLF: _tmpAAA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAA0)->f2;_LL10:
 return Cyc_Tcutil_fields_support_default(0,0,_tmpAAA);case 14U: _LL11: _LL12:
# 5541
 goto _LL14;case 13U: _LL13: _LL14:
 return 1;default: _LL15: _LL16:
# 5544
 return 0;}_LL0:;}
# 5549
void Cyc_Tcutil_check_no_qual(unsigned int loc,void*t){
void*_tmpAAB=t;struct Cyc_Absyn_Typedefdecl*_tmpAB0;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAAB)->tag == 17U){_LL1: _tmpAB0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmpAAB)->f3;_LL2:
# 5552
 if(_tmpAB0 != 0){
struct Cyc_Absyn_Tqual _tmpAAC=_tmpAB0->tq;
if(((_tmpAAC.print_const  || _tmpAAC.q_volatile) || _tmpAAC.q_restrict) || _tmpAAC.real_const)
# 5557
({struct Cyc_String_pa_PrintArg_struct _tmpAAF;_tmpAAF.tag=0U;({struct _dyneither_ptr _tmpF1E=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAAF.f1=_tmpF1E;});({void*_tmpAAD[1U]={& _tmpAAF};({unsigned int _tmpF20=loc;struct _dyneither_ptr _tmpF1F=({const char*_tmpAAE="qualifier within typedef type %s is ignored";_tag_dyneither(_tmpAAE,sizeof(char),44U);});Cyc_Tcutil_warn(_tmpF20,_tmpF1F,_tag_dyneither(_tmpAAD,sizeof(void*),1U));});});});}
# 5560
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 5565
static int Cyc_Tcutil_fields_support_default(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 5567
struct _RegionHandle _tmpAB1=_new_region("rgn");struct _RegionHandle*rgn=& _tmpAB1;_push_region(rgn);
{struct Cyc_List_List*_tmpAB2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_supports_default(t)){int _tmpAB3=1;_npop_handler(0U);return _tmpAB3;}
({void*_tmpF21=Cyc_Tcutil_rsubstitute(rgn,_tmpAB2,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);t=_tmpF21;});
if(!Cyc_Tcutil_supports_default(t)){int _tmpAB4=0;_npop_handler(0U);return _tmpAB4;}}{
# 5575
int _tmpAB5=1;_npop_handler(0U);return _tmpAB5;};}
# 5568
;_pop_region(rgn);}
# 5581
int Cyc_Tcutil_admits_zero(void*t){
void*_tmpAB6=Cyc_Tcutil_compress(t);void*_tmpAB7=_tmpAB6;union Cyc_Absyn_Constraint*_tmpABB;union Cyc_Absyn_Constraint*_tmpABA;switch(*((int*)_tmpAB7)){case 6U: _LL1: _LL2:
 goto _LL4;case 7U: _LL3: _LL4:
 return 1;case 5U: _LL5: _tmpABB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB7)->f1).ptr_atts).nullable;_tmpABA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAB7)->f1).ptr_atts).bounds;_LL6: {
# 5586
void*_tmpAB8=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmpABA);void*_tmpAB9=_tmpAB8;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmpAB9)->tag == 0U){_LLA: _LLB:
# 5590
 return 0;}else{_LLC: _LLD:
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmpABB);}_LL9:;}default: _LL7: _LL8:
# 5593
 return 0;}_LL0:;}
# 5597
int Cyc_Tcutil_is_noreturn(void*t){
{void*_tmpABC=Cyc_Tcutil_compress(t);void*_tmpABD=_tmpABC;struct Cyc_List_List*_tmpAC1;void*_tmpAC0;switch(*((int*)_tmpABD)){case 5U: _LL1: _tmpAC0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpABD)->f1).elt_typ;_LL2:
 return Cyc_Tcutil_is_noreturn(_tmpAC0);case 9U: _LL3: _tmpAC1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpABD)->f1).attributes;_LL4:
# 5601
 for(0;_tmpAC1 != 0;_tmpAC1=_tmpAC1->tl){
void*_tmpABE=(void*)_tmpAC1->hd;void*_tmpABF=_tmpABE;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmpABF)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 5606
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5609
return 0;}
# 5614
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmpAC2=Cyc_Tcutil_compress(t);void*_tmpAC3=_tmpAC2;struct Cyc_List_List**_tmpAC9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAC3)->tag == 9U){_LL1: _tmpAC9=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmpAC3)->f1).attributes;_LL2: {
# 5617
struct Cyc_List_List*_tmpAC4=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Tcutil_attribute_cmp,*_tmpAC9,(void*)atts->hd))
({struct Cyc_List_List*_tmpF22=({struct Cyc_List_List*_tmpAC5=_cycalloc(sizeof(*_tmpAC5));_tmpAC5->hd=(void*)atts->hd;_tmpAC5->tl=*_tmpAC9;_tmpAC5;});*_tmpAC9=_tmpF22;});}else{
# 5624
({struct Cyc_List_List*_tmpF23=({struct Cyc_List_List*_tmpAC6=_cycalloc(sizeof(*_tmpAC6));_tmpAC6->hd=(void*)atts->hd;_tmpAC6->tl=_tmpAC4;_tmpAC6;});_tmpAC4=_tmpF23;});}}
return _tmpAC4;}}else{_LL3: _LL4:
({void*_tmpAC7=0U;({struct _dyneither_ptr _tmpF24=({const char*_tmpAC8="transfer_fn_type_atts";_tag_dyneither(_tmpAC8,sizeof(char),22U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF24,_tag_dyneither(_tmpAC7,sizeof(void*),0U));});});}_LL0:;}
# 5631
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
struct Cyc_Absyn_Exp*bound=0;
{void*_tmpACA=Cyc_Tcutil_compress(t);void*_tmpACB=_tmpACA;struct Cyc_Absyn_Exp*_tmpAD0;struct Cyc_Absyn_PtrInfo*_tmpACF;switch(*((int*)_tmpACB)){case 5U: _LL1: _tmpACF=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpACB)->f1;_LL2:
# 5635
{void*_tmpACC=((void*(*)(union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_val)((_tmpACF->ptr_atts).bounds);void*_tmpACD=_tmpACC;struct Cyc_Absyn_Exp*_tmpACE;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpACD)->tag == 1U){_LL8: _tmpACE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmpACD)->f1;_LL9:
 bound=_tmpACE;goto _LL7;}else{_LLA: _LLB:
 goto _LL7;}_LL7:;}
# 5639
goto _LL0;case 8U: _LL3: _tmpAD0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpACB)->f1).num_elts;_LL4:
# 5641
 bound=_tmpAD0;
goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 5645
return bound;}
# 5650
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
{void*_tmpAD1=b;struct Cyc_Absyn_Vardecl*_tmpAD5;struct Cyc_Absyn_Vardecl*_tmpAD4;struct Cyc_Absyn_Vardecl*_tmpAD3;struct Cyc_Absyn_Vardecl*_tmpAD2;switch(*((int*)_tmpAD1)){case 5U: _LL1: _tmpAD2=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmpAD1)->f1;_LL2:
 _tmpAD3=_tmpAD2;goto _LL4;case 4U: _LL3: _tmpAD3=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmpAD1)->f1;_LL4:
 _tmpAD4=_tmpAD3;goto _LL6;case 3U: _LL5: _tmpAD4=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmpAD1)->f1;_LL6:
 _tmpAD5=_tmpAD4;goto _LL8;case 1U: _LL7: _tmpAD5=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmpAD1)->f1;_LL8:
# 5656
 if(!_tmpAD5->escapes)return _tmpAD5;
goto _LL0;default: _LL9: _LLA:
 goto _LL0;}_LL0:;}
# 5660
return 0;}
# 5664
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmpAD6=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)({struct Cyc_List_List*_tmpF25=({struct Cyc_List_List*_tmpAD7=_cycalloc(sizeof(*_tmpAD7));_tmpAD7->hd=*((void**)_check_null((void**)x->hd));_tmpAD7->tl=_tmpAD6;_tmpAD7;});_tmpAD6=_tmpF25;});}}
return _tmpAD6;}
# 5671
int Cyc_Tcutil_is_array(void*t){
void*_tmpAD8=Cyc_Tcutil_compress(t);void*_tmpAD9=_tmpAD8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAD9)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 5678
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmpADA=Cyc_Tcutil_compress(t);void*_tmpADB=_tmpADA;void*_tmpAEB;struct Cyc_Absyn_Tqual _tmpAEA;struct Cyc_Absyn_Exp*_tmpAE9;union Cyc_Absyn_Constraint*_tmpAE8;unsigned int _tmpAE7;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADB)->tag == 8U){_LL1: _tmpAEB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADB)->f1).elt_type;_tmpAEA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADB)->f1).tq;_tmpAE9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADB)->f1).num_elts;_tmpAE8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADB)->f1).zero_term;_tmpAE7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpADB)->f1).zt_loc;_LL2: {
# 5681
void*b;
if(_tmpAE9 == 0)
b=(void*)& Cyc_Absyn_DynEither_b_val;else{
# 5685
struct Cyc_Absyn_Exp*bnd=_tmpAE9;
if(convert_tag){
if(bnd->topt == 0)
({void*_tmpADC=0U;({struct _dyneither_ptr _tmpF26=({const char*_tmpADD="cannot convert tag without type!";_tag_dyneither(_tmpADD,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF26,_tag_dyneither(_tmpADC,sizeof(void*),0U));});});{
void*_tmpADE=Cyc_Tcutil_compress((void*)_check_null(bnd->topt));void*_tmpADF=_tmpADE;void*_tmpAE4;if(((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpADF)->tag == 19U){_LL6: _tmpAE4=(void*)((struct Cyc_Absyn_TagType_Absyn_Type_struct*)_tmpADF)->f1;_LL7:
# 5691
({void*_tmpF2A=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAE0=_cycalloc(sizeof(*_tmpAE0));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF29=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAE1;_tmpAE1.tag=1U;({struct Cyc_Absyn_Exp*_tmpF28=({void*_tmpF27=Cyc_Absyn_uint_typ;Cyc_Absyn_cast_exp(_tmpF27,Cyc_Absyn_valueof_exp(_tmpAE4,0U),0,Cyc_Absyn_No_coercion,0U);});_tmpAE1.f1=_tmpF28;});_tmpAE1;});_tmpAE0[0]=_tmpF29;});_tmpAE0;});b=_tmpF2A;});
goto _LL5;}else{_LL8: _LL9:
# 5694
 if(Cyc_Tcutil_is_const_exp(bnd))
({void*_tmpF2C=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAE2=_cycalloc(sizeof(*_tmpAE2));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF2B=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAE3;_tmpAE3.tag=1U;_tmpAE3.f1=bnd;_tmpAE3;});_tmpAE2[0]=_tmpF2B;});_tmpAE2;});b=_tmpF2C;});else{
# 5697
b=(void*)& Cyc_Absyn_DynEither_b_val;}}_LL5:;};}else{
# 5701
({void*_tmpF2E=(void*)({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*_tmpAE5=_cycalloc(sizeof(*_tmpAE5));({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpF2D=({struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct _tmpAE6;_tmpAE6.tag=1U;_tmpAE6.f1=bnd;_tmpAE6;});_tmpAE5[0]=_tmpF2D;});_tmpAE5;});b=_tmpF2E;});}}
# 5703
return Cyc_Absyn_atb_typ(_tmpAEB,rgn,_tmpAEA,b,_tmpAE8);}}else{_LL3: _LL4:
# 5706
 return t;}_LL0:;}
# 5711
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmpAEC=Cyc_Tcutil_compress(t);void*_tmpAED=_tmpAEC;struct Cyc_List_List*_tmpAFB;union Cyc_Absyn_AggrInfoU _tmpAFA;struct Cyc_List_List*_tmpAF9;struct Cyc_List_List*_tmpAF8;void*_tmpAF7;union Cyc_Absyn_Constraint*_tmpAF6;switch(*((int*)_tmpAED)){case 0U: _LL1: _LL2:
 return 1;case 1U: _LL3: _LL4:
 goto _LL6;case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
 goto _LLA;case 4U: _LL9: _LLA:
 return 0;case 5U: _LLB: _tmpAF6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmpAED)->f1).ptr_atts).nullable;_LLC:
# 5719
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(1,_tmpAF6);case 6U: _LLD: _LLE:
 goto _LL10;case 7U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmpAF7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmpAED)->f1).elt_type;_LL12:
 return Cyc_Tcutil_zeroable_type(_tmpAF7);case 9U: _LL13: _LL14:
 return 0;case 10U: _LL15: _tmpAF8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmpAED)->f1;_LL16:
# 5725
 for(0;(unsigned int)_tmpAF8;_tmpAF8=_tmpAF8->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmpAF8->hd)).f2))return 0;}
return 1;case 11U: _LL17: _tmpAFA=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAED)->f1).aggr_info;_tmpAF9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmpAED)->f1).targs;_LL18: {
# 5729
struct Cyc_Absyn_Aggrdecl*_tmpAEE=Cyc_Absyn_get_known_aggrdecl(_tmpAFA);
if(_tmpAEE->impl == 0  || ((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAEE->impl))->exist_vars != 0)
return 0;{
struct _RegionHandle _tmpAEF=_new_region("r");struct _RegionHandle*r=& _tmpAEF;_push_region(r);
{struct Cyc_List_List*_tmpAF0=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmpAEE->tvs,_tmpAF9);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmpAEE->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_zeroable_type(Cyc_Tcutil_rsubstitute(r,_tmpAF0,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmpAF1=0;_npop_handler(0U);return _tmpAF1;}}}{
int _tmpAF2=1;_npop_handler(0U);return _tmpAF2;};}
# 5733
;_pop_region(r);};}case 13U: _LL19: _LL1A:
# 5737
 goto _LL1C;case 19U: _LL1B: _LL1C:
 return 1;case 12U: _LL1D: _tmpAFB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmpAED)->f2;_LL1E:
# 5740
 for(0;_tmpAFB != 0;_tmpAFB=_tmpAFB->tl){
if(!Cyc_Tcutil_zeroable_type(((struct Cyc_Absyn_Aggrfield*)_tmpAFB->hd)->type))return 0;}
return 1;case 14U: _LL1F: _LL20:
 return 1;case 17U: _LL21: _LL22:
 goto _LL24;case 16U: _LL23: _LL24:
 goto _LL26;case 15U: _LL25: _LL26:
 goto _LL28;case 28U: _LL27: _LL28:
 return 0;case 26U: _LL29: _LL2A:
# 5749
 goto _LL2C;case 27U: _LL2B: _LL2C:
 goto _LL2E;case 18U: _LL2D: _LL2E:
 goto _LL30;case 20U: _LL2F: _LL30:
 goto _LL32;case 21U: _LL31: _LL32:
 goto _LL34;case 22U: _LL33: _LL34:
 goto _LL36;case 23U: _LL35: _LL36:
 goto _LL38;case 24U: _LL37: _LL38:
 goto _LL3A;default: _LL39: _LL3A:
({struct Cyc_String_pa_PrintArg_struct _tmpAF5;_tmpAF5.tag=0U;({struct _dyneither_ptr _tmpF2F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmpAF5.f1=_tmpF2F;});({void*_tmpAF3[1U]={& _tmpAF5};({struct _dyneither_ptr _tmpF30=({const char*_tmpAF4="bad type `%s' in zeroable type";_tag_dyneither(_tmpAF4,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpF30,_tag_dyneither(_tmpAF3,sizeof(void*),1U));});});});}_LL0:;}
