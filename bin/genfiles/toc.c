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
struct _dyneither_ptr Cyc_aprintf(struct _dyneither_ptr,struct _dyneither_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned int*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _dyneither_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _dyneither_ptr f1;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _dyneither_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _dyneither_ptr f1;};
# 167 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 170
extern struct _RegionHandle*Cyc_Core_unique_region;
# 177
void Cyc_Core_ufree(void*ptr);
# 190
struct _dyneither_ptr Cyc_Core_alias_refptr(struct _dyneither_ptr ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 237
struct Cyc_Core_NewDynamicRegion Cyc_Core_new_rckey();
# 245
void Cyc_Core_free_rckey(struct Cyc_Core_DynamicRegion*k);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _dyneither_ptr);
# 57
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*,struct _dyneither_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*f)(void*),struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 172
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 195
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 242
void*Cyc_List_nth(struct Cyc_List_List*x,int n);
# 251
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);
# 38 "string.h"
unsigned long Cyc_strlen(struct _dyneither_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _dyneither_ptr s1,struct _dyneither_ptr s2);
int Cyc_strptrcmp(struct _dyneither_ptr*s1,struct _dyneither_ptr*s2);
# 62
struct _dyneither_ptr Cyc_strconcat(struct _dyneither_ptr,struct _dyneither_ptr);
# 67
struct _dyneither_ptr Cyc_rstr_sepstr(struct _RegionHandle*,struct Cyc_List_List*,struct _dyneither_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};
# 37 "iter.h"
int Cyc_Iter_next(struct Cyc_Iter_Iter,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 68 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*cmp)(void*,void*));
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 113
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*k);
# 122 "dict.h"
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*k);struct Cyc_Xarray_Xarray{struct _dyneither_ptr elmts;int num_elmts;};
# 40 "xarray.h"
int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 57
struct Cyc_Xarray_Xarray*Cyc_Xarray_rcreate_empty(struct _RegionHandle*);
# 69
int Cyc_Xarray_add_ind(struct Cyc_Xarray_Xarray*,void*);
# 121
void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*xarr);struct Cyc_Position_Error;
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
void*Cyc_Warn_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 96 "absyn.h"
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 99
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _dyneither_ptr*f2;};
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
extern struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct Cyc_Absyn_DynEither_b_val;struct Cyc_Absyn_PtrLoc{unsigned int ptr_loc;unsigned int rgn_loc;unsigned int zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;union Cyc_Absyn_Constraint*nullable;union Cyc_Absyn_Constraint*bounds;union Cyc_Absyn_Constraint*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_typ;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_typ;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfoU_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfoU_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfoU{struct _union_DatatypeInfoU_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfoU_KnownDatatype KnownDatatype;};struct Cyc_Absyn_DatatypeInfo{union Cyc_Absyn_DatatypeInfoU datatype_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfoU_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfoU_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfoU{struct _union_DatatypeFieldInfoU_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfoU_KnownDatatypefield KnownDatatypefield;};struct Cyc_Absyn_DatatypeFieldInfo{union Cyc_Absyn_DatatypeFieldInfoU field_info;struct Cyc_List_List*targs;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfoU_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfoU_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfoU{struct _union_AggrInfoU_UnknownAggr UnknownAggr;struct _union_AggrInfoU_KnownAggr KnownAggr;};
# 347
union Cyc_Absyn_AggrInfoU Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_AggrInfo{union Cyc_Absyn_AggrInfoU aggr_info;struct Cyc_List_List*targs;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;union Cyc_Absyn_Constraint*zero_term;unsigned int zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned int loc;};struct Cyc_Absyn_VoidType_Absyn_Type_struct{int tag;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_DatatypeType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct{int tag;struct Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_IntType_Absyn_Type_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatType_Absyn_Type_struct{int tag;int f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AggrType_Absyn_Type_struct{int tag;struct Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_EnumType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnHandleType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_DynRgnType_Absyn_Type_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TagType_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_HeapRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_UniqueRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_RefCntRgn_Absyn_Type_struct{int tag;};struct Cyc_Absyn_AccessEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_JoinEff_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_RgnsEff_Absyn_Type_struct{int tag;void*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_BuiltinType_Absyn_Type_struct{int tag;struct _dyneither_ptr f1;struct Cyc_Absyn_Kind*f2;};
# 427 "absyn.h"
extern struct Cyc_Absyn_HeapRgn_Absyn_Type_struct Cyc_Absyn_HeapRgn_val;
# 430
extern struct Cyc_Absyn_VoidType_Absyn_Type_struct Cyc_Absyn_VoidType_val;struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 446
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft  = 0U,Cyc_Absyn_Scanf_ft  = 1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noconsume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;union Cyc_Absyn_Constraint*f1;unsigned int f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;union Cyc_Absyn_Constraint*f2;unsigned int f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned int f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned int f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _dyneither_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _dyneither_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _dyneither_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _dyneither_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 528
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
union Cyc_Absyn_Cnst Cyc_Absyn_Float_c(struct _dyneither_ptr,int);
# 536
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus  = 0U,Cyc_Absyn_Times  = 1U,Cyc_Absyn_Minus  = 2U,Cyc_Absyn_Div  = 3U,Cyc_Absyn_Mod  = 4U,Cyc_Absyn_Eq  = 5U,Cyc_Absyn_Neq  = 6U,Cyc_Absyn_Gt  = 7U,Cyc_Absyn_Lt  = 8U,Cyc_Absyn_Gte  = 9U,Cyc_Absyn_Lte  = 10U,Cyc_Absyn_Not  = 11U,Cyc_Absyn_Bitnot  = 12U,Cyc_Absyn_Bitand  = 13U,Cyc_Absyn_Bitor  = 14U,Cyc_Absyn_Bitxor  = 15U,Cyc_Absyn_Bitlshift  = 16U,Cyc_Absyn_Bitlrshift  = 17U,Cyc_Absyn_Bitarshift  = 18U,Cyc_Absyn_Numelts  = 19U};
# 543
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc  = 0U,Cyc_Absyn_PostInc  = 1U,Cyc_Absyn_PreDec  = 2U,Cyc_Absyn_PostDec  = 3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned int f1;};
# 561
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion  = 0U,Cyc_Absyn_No_coercion  = 1U,Cyc_Absyn_Null_to_NonNull  = 2U,Cyc_Absyn_Other_coercion  = 3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _dyneither_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _dyneither_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _dyneither_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _dyneither_ptr f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned int loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 724 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned int loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned int loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned int varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_List_List*tvs;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_Absyn_Stmt*body;void*cached_typ;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_Aggrfield{struct _dyneither_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned int loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned int loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _dyneither_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned int loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _dyneither_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 909
extern struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct Cyc_Absyn_EmptyAnnot_val;
# 916
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 926
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned int);
# 929
union Cyc_Absyn_Constraint*Cyc_Absyn_new_conref(void*x);
# 936
void*Cyc_Absyn_conref_def(void*y,union Cyc_Absyn_Constraint*x);
# 939
extern union Cyc_Absyn_Constraint*Cyc_Absyn_true_conref;
extern union Cyc_Absyn_Constraint*Cyc_Absyn_false_conref;
# 954
extern void*Cyc_Absyn_char_typ;extern void*Cyc_Absyn_uchar_typ;extern void*Cyc_Absyn_uint_typ;
# 956
extern void*Cyc_Absyn_sint_typ;
# 964
void*Cyc_Absyn_exn_typ();
# 973
extern void*Cyc_Absyn_bounds_one;
# 981
void*Cyc_Absyn_star_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 987
void*Cyc_Absyn_cstar_typ(void*t,struct Cyc_Absyn_Tqual tq);
# 989
void*Cyc_Absyn_dyneither_typ(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,union Cyc_Absyn_Constraint*zero_term);
# 992
void*Cyc_Absyn_void_star_typ();
# 994
void*Cyc_Absyn_strct(struct _dyneither_ptr*name);
void*Cyc_Absyn_strctq(struct _tuple1*name);
void*Cyc_Absyn_unionq_typ(struct _tuple1*name);
# 1000
void*Cyc_Absyn_array_typ(void*elt_type,struct Cyc_Absyn_Tqual tq,struct Cyc_Absyn_Exp*num_elts,union Cyc_Absyn_Constraint*zero_term,unsigned int ztloc);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned int);
# 1006
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned int);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_signed_int_exp(int,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned int,unsigned int);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_float_exp(struct _dyneither_ptr f,int i,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_string_exp(struct _dyneither_ptr s,unsigned int);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_var_exp(struct _tuple1*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned int);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_prim1_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_prim2_exp(enum Cyc_Absyn_Primop,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_add_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_divide_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_eq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_neq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_assign_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_pre_inc_exp(struct Cyc_Absyn_Exp*,unsigned int);
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_fncall_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned int);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftyp_exp(void*t,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned int);
# 1059
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrmember_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_aggrarrow_exp(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int);
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned int);
# 1064
struct Cyc_Absyn_Exp*Cyc_Absyn_stmt_exp(struct Cyc_Absyn_Stmt*,unsigned int);
# 1067
struct Cyc_Absyn_Exp*Cyc_Absyn_array_exp(struct Cyc_List_List*,unsigned int);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_unresolvedmem_exp(struct Cyc_Core_Opt*,struct Cyc_List_List*,unsigned int);
# 1076
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_seq_stmt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1081
struct Cyc_Absyn_Stmt*Cyc_Absyn_return_stmt(struct Cyc_Absyn_Exp*e,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_ifthenelse_stmt(struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2,unsigned int loc);
# 1089
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*d,struct Cyc_Absyn_Stmt*s,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_declare_stmt(struct _tuple1*,void*,struct Cyc_Absyn_Exp*init,struct Cyc_Absyn_Stmt*,unsigned int loc);
# 1093
struct Cyc_Absyn_Stmt*Cyc_Absyn_label_stmt(struct _dyneither_ptr*v,struct Cyc_Absyn_Stmt*s,unsigned int loc);
# 1095
struct Cyc_Absyn_Stmt*Cyc_Absyn_goto_stmt(struct _dyneither_ptr*lab,unsigned int loc);
struct Cyc_Absyn_Stmt*Cyc_Absyn_assign_stmt(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,unsigned int loc);
# 1100
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*p,unsigned int s);
# 1104
struct Cyc_Absyn_Decl*Cyc_Absyn_new_decl(void*r,unsigned int loc);
# 1109
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned int varloc,struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_static_vardecl(struct _tuple1*x,void*t,struct Cyc_Absyn_Exp*init);
# 1152
int Cyc_Absyn_is_lvalue(struct Cyc_Absyn_Exp*);
# 1155
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _dyneither_ptr*);
# 1165
struct _dyneither_ptr*Cyc_Absyn_fieldname(int);struct _tuple11{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1167
struct _tuple11 Cyc_Absyn_aggr_kinded_name(union Cyc_Absyn_AggrInfoU);
# 1169
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfoU);
# 1178
struct _dyneither_ptr*Cyc_Absyn_designatorlist_to_fieldname(struct Cyc_List_List*);
# 1183
extern int Cyc_Absyn_no_regions;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _dyneither_ptr Cyc_Absynpp_typ2string(void*);
# 67
struct _dyneither_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
struct _dyneither_ptr Cyc_Absynpp_stmt2string(struct Cyc_Absyn_Stmt*);
struct _dyneither_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);struct Cyc_Set_Set;
# 54 "set.h"
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*r,int(*cmp)(void*,void*));
# 69
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*r,struct Cyc_Set_Set*s,void*elt);
# 100
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;
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
# 84 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew  = 0U,Cyc_Tcenv_InNew  = 1U,Cyc_Tcenv_InNewAggr  = 2U};
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
# 52
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*t,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_typ_kind(void*t);
# 55
void*Cyc_Tcutil_compress(void*t);
# 65
int Cyc_Tcutil_is_arithmetic_type(void*);
# 86
int Cyc_Tcutil_is_pointer_type(void*t);
# 88
int Cyc_Tcutil_is_nullable_pointer_type(void*t);
# 90
int Cyc_Tcutil_is_boxed(void*t);
int Cyc_Tcutil_is_dyneither_ptr(void*t);
void*Cyc_Tcutil_pointer_elt_type(void*t);
# 97
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
# 105
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
# 147
int Cyc_Tcutil_unify(void*,void*);
# 150
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 152
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 154
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 241 "tcutil.h"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields);
# 247
int Cyc_Tcutil_is_tagged_pointer_typ(void*);
# 249
int Cyc_Tcutil_is_tagged_pointer_typ_elt(void*t,void**elt_typ_dest);
# 251
int Cyc_Tcutil_is_zero_pointer_typ_elt(void*t,void**elt_typ_dest);
# 261
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_dyneither,void**elt_type);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 319
void*Cyc_Tcutil_snd_tqt(struct _tuple12*);
# 348
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t);
# 352
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b);
# 355
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l);struct _tuple13{unsigned int f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 32
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct _union_RelnOp_RConst{int tag;unsigned int val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned int val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned int val;};struct _union_RelnOp_RReturn{int tag;unsigned int val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 40 "relations.h"
union Cyc_Relations_RelnOp Cyc_Relations_RConst(unsigned int);union Cyc_Relations_RelnOp Cyc_Relations_RNumelts(struct Cyc_Absyn_Vardecl*);
# 49
enum Cyc_Relations_Relation{Cyc_Relations_Req  = 0U,Cyc_Relations_Rneq  = 1U,Cyc_Relations_Rlte  = 2U,Cyc_Relations_Rlt  = 3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 75
int Cyc_Relations_exp2relnop(struct Cyc_Absyn_Exp*e,union Cyc_Relations_RelnOp*p);
# 86
struct Cyc_List_List*Cyc_Relations_add_relation(struct _RegionHandle*rgn,union Cyc_Relations_RelnOp rop1,enum Cyc_Relations_Relation r,union Cyc_Relations_RelnOp rop2,struct Cyc_List_List*relns);
# 128
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;
void Cyc_CfFlowInfo_aerr(unsigned int loc,struct _dyneither_ptr fmt,struct _dyneither_ptr ap);struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL  = 0U,Cyc_CfFlowInfo_AllIL  = 1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _dyneither_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple14{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple14 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};struct Cyc_Hashtable_Table;
# 52 "hashtable.h"
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key);
# 34 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);
# 38
void*Cyc_Toc_typ_to_c(void*);
# 40
struct _tuple1*Cyc_Toc_temp_var();
extern struct _dyneither_ptr Cyc_Toc_globals;extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 47
extern int Cyc_Toc_warn_bounds_checks;
extern int Cyc_Toc_warn_all_null_deref;
void Cyc_Toc_finish();struct Cyc_Tcpat_TcPatResult{struct _tuple0*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Tcpat_EqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_NeqNull_Tcpat_PatTest_struct{int tag;};struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr f1;int f2;};struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct{int tag;int f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct{int tag;struct _dyneither_ptr*f1;int f2;};struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Tcpat_Dummy_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_Deref_Tcpat_Access_struct{int tag;};struct Cyc_Tcpat_TupleField_Tcpat_Access_struct{int tag;unsigned int f1;};struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;unsigned int f3;};struct Cyc_Tcpat_AggrField_Tcpat_Access_struct{int tag;int f1;struct _dyneither_ptr*f2;};struct _union_PatOrWhere_pattern{int tag;struct Cyc_Absyn_Pat*val;};struct _union_PatOrWhere_where_clause{int tag;struct Cyc_Absyn_Exp*val;};union Cyc_Tcpat_PatOrWhere{struct _union_PatOrWhere_pattern pattern;struct _union_PatOrWhere_where_clause where_clause;};struct Cyc_Tcpat_PathNode{union Cyc_Tcpat_PatOrWhere orig_pat;void*access;};struct Cyc_Tcpat_Rhs{int used;unsigned int pat_loc;struct Cyc_Absyn_Stmt*rhs;};struct Cyc_Tcpat_Failure_Tcpat_Decision_struct{int tag;void*f1;};struct Cyc_Tcpat_Success_Tcpat_Decision_struct{int tag;struct Cyc_Tcpat_Rhs*f1;};struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;void*f3;};
# 61 "toc.cyc"
extern int Cyc_noexpand_r;char Cyc_Toc_Dest[5U]="Dest";
# 76 "toc.cyc"
static void*Cyc_Toc_unimp(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 78
({struct _dyneither_ptr _tmp7A1=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7A0=({const char*_tmp0="Toc (unimplemented): ";_tag_dyneither(_tmp0,sizeof(char),22U);});Cyc_strconcat(_tmp7A0,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7A1,ap);});}
# 80
static void*Cyc_Toc_toc_impos(struct _dyneither_ptr fmt,struct _dyneither_ptr ap){
# 82
({struct _dyneither_ptr _tmp7A3=(struct _dyneither_ptr)({struct _dyneither_ptr _tmp7A2=({const char*_tmp1="Toc: ";_tag_dyneither(_tmp1,sizeof(char),6U);});Cyc_strconcat(_tmp7A2,(struct _dyneither_ptr)fmt);});Cyc_Warn_vimpos(_tmp7A3,ap);});}
# 86
int Cyc_Toc_warn_bounds_checks=0;
int Cyc_Toc_warn_all_null_deref=0;
unsigned int Cyc_Toc_total_bounds_checks=0U;
unsigned int Cyc_Toc_bounds_checks_eliminated=0U;
# 91
struct _dyneither_ptr Cyc_Toc_globals={(void*)0,(void*)0,(void*)(0 + 0)};
# 94
static struct Cyc_Hashtable_Table**Cyc_Toc_gpop_tables=0;
static struct Cyc_Hashtable_Table**Cyc_Toc_fn_pop_table=0;
static int Cyc_Toc_tuple_type_counter=0;
static int Cyc_Toc_temp_var_counter=0;
static int Cyc_Toc_fresh_label_counter=0;
# 104
static struct Cyc_List_List*Cyc_Toc_result_decls=0;
# 106
static int Cyc_Toc_get_npop(struct Cyc_Absyn_Stmt*s){
return((int(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Stmt*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_fn_pop_table)),s);}struct Cyc_Toc_TocState{struct Cyc_List_List**tuple_types;struct Cyc_List_List**anon_aggr_types;struct Cyc_Dict_Dict*aggrs_so_far;struct Cyc_List_List**abs_struct_types;struct Cyc_Set_Set**datatypes_so_far;struct Cyc_Dict_Dict*xdatatypes_so_far;struct Cyc_Dict_Dict*qvar_tags;struct Cyc_Xarray_Xarray*temp_labels;};struct _tuple15{struct _tuple1*f1;struct _tuple1*f2;};
# 132
static int Cyc_Toc_qvar_tag_cmp(struct _tuple15*x,struct _tuple15*y){
struct _tuple15 _tmp2=*x;struct _tuple15 _tmp3=_tmp2;struct _tuple1*_tmp9;struct _tuple1*_tmp8;_LL1: _tmp9=_tmp3.f1;_tmp8=_tmp3.f2;_LL2:;{
struct _tuple15 _tmp4=*y;struct _tuple15 _tmp5=_tmp4;struct _tuple1*_tmp7;struct _tuple1*_tmp6;_LL4: _tmp7=_tmp5.f1;_tmp6=_tmp5.f2;_LL5:;{
int i=Cyc_Absyn_qvar_cmp(_tmp9,_tmp7);
if(i != 0)return i;
return Cyc_Absyn_qvar_cmp(_tmp8,_tmp6);};};}
# 141
static struct Cyc_Toc_TocState*Cyc_Toc_empty_toc_state(struct _RegionHandle*d){
return({struct Cyc_Toc_TocState*_tmp11=_region_malloc(d,sizeof(*_tmp11));
({struct Cyc_List_List**_tmp7AF=({struct Cyc_List_List**_tmpA=_region_malloc(d,sizeof(*_tmpA));*_tmpA=0;_tmpA;});_tmp11->tuple_types=_tmp7AF;}),({
struct Cyc_List_List**_tmp7AE=({struct Cyc_List_List**_tmpB=_region_malloc(d,sizeof(*_tmpB));*_tmpB=0;_tmpB;});_tmp11->anon_aggr_types=_tmp7AE;}),({
struct Cyc_Dict_Dict*_tmp7AD=({struct Cyc_Dict_Dict*_tmpC=_region_malloc(d,sizeof(*_tmpC));({struct Cyc_Dict_Dict _tmp7AC=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpC=_tmp7AC;});_tmpC;});_tmp11->aggrs_so_far=_tmp7AD;}),({
struct Cyc_List_List**_tmp7AB=({struct Cyc_List_List**_tmpD=_region_malloc(d,sizeof(*_tmpD));*_tmpD=0;_tmpD;});_tmp11->abs_struct_types=_tmp7AB;}),({
struct Cyc_Set_Set**_tmp7AA=({struct Cyc_Set_Set**_tmpE=_region_malloc(d,sizeof(*_tmpE));({struct Cyc_Set_Set*_tmp7A9=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Set_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpE=_tmp7A9;});_tmpE;});_tmp11->datatypes_so_far=_tmp7AA;}),({
struct Cyc_Dict_Dict*_tmp7A8=({struct Cyc_Dict_Dict*_tmpF=_region_malloc(d,sizeof(*_tmpF));({struct Cyc_Dict_Dict _tmp7A7=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple1*,struct _tuple1*)))Cyc_Dict_rempty)(d,Cyc_Absyn_qvar_cmp);*_tmpF=_tmp7A7;});_tmpF;});_tmp11->xdatatypes_so_far=_tmp7A8;}),({
struct Cyc_Dict_Dict*_tmp7A6=({struct Cyc_Dict_Dict*_tmp10=_region_malloc(d,sizeof(*_tmp10));({struct Cyc_Dict_Dict _tmp7A5=((struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*cmp)(struct _tuple15*,struct _tuple15*)))Cyc_Dict_rempty)(d,Cyc_Toc_qvar_tag_cmp);*_tmp10=_tmp7A5;});_tmp10;});_tmp11->qvar_tags=_tmp7A6;}),({
struct Cyc_Xarray_Xarray*_tmp7A4=((struct Cyc_Xarray_Xarray*(*)(struct _RegionHandle*))Cyc_Xarray_rcreate_empty)(d);_tmp11->temp_labels=_tmp7A4;});_tmp11;});}struct Cyc_Toc_TocStateWrap{struct Cyc_Core_DynamicRegion*dyn;struct Cyc_Toc_TocState*state;};
# 160
static struct Cyc_Toc_TocStateWrap*Cyc_Toc_toc_state=0;struct _tuple16{struct Cyc_Toc_TocState*f1;void*f2;};
# 166
static void*Cyc_Toc_use_toc_state(void*arg,void*(*f)(struct _RegionHandle*,struct _tuple16*)){
# 169
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp12=ts;struct Cyc_Toc_TocStateWrap*_tmp13=Cyc_Toc_toc_state;ts=_tmp13;Cyc_Toc_toc_state=_tmp12;});{
struct Cyc_Toc_TocStateWrap _tmp14=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp15=_tmp14;struct Cyc_Core_DynamicRegion*_tmp1C;struct Cyc_Toc_TocState*_tmp1B;_LL1: _tmp1C=_tmp15.dyn;_tmp1B=_tmp15.state;_LL2:;{
struct _dyneither_ptr _tmp16=((struct _dyneither_ptr(*)(struct _dyneither_ptr ptr))Cyc_Core_alias_refptr)(_tag_dyneither(_tmp1C,sizeof(struct Cyc_Core_DynamicRegion),1U));
({struct Cyc_Toc_TocStateWrap _tmp7B0=({struct Cyc_Toc_TocStateWrap _tmp76B;_tmp76B.dyn=_tmp1C,_tmp76B.state=_tmp1B;_tmp76B;});*ts=_tmp7B0;});
({struct Cyc_Toc_TocStateWrap*_tmp17=ts;struct Cyc_Toc_TocStateWrap*_tmp18=Cyc_Toc_toc_state;ts=_tmp18;Cyc_Toc_toc_state=_tmp17;});{
void*res;
{struct _RegionHandle*h=&((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)))->h;
{struct _tuple16 _tmp1A=({struct _tuple16 _tmp76C;_tmp76C.f1=_tmp1B,_tmp76C.f2=arg;_tmp76C;});
({void*_tmp7B1=f(h,& _tmp1A);res=_tmp7B1;});}
# 177
;}
# 179
Cyc_Core_free_rckey((struct Cyc_Core_DynamicRegion*)_check_null(_untag_dyneither_ptr(_tmp16,sizeof(struct Cyc_Core_DynamicRegion),1U)));
return res;};};};}struct _tuple17{struct _tuple1*f1;void*(*f2)(struct _tuple1*);};struct _tuple18{struct Cyc_Toc_TocState*f1;struct _tuple17*f2;};struct _tuple19{struct Cyc_Absyn_Aggrdecl*f1;void*f2;};
# 183
static void*Cyc_Toc_aggrdecl_type_body(struct _RegionHandle*d,struct _tuple18*env){
# 186
struct _tuple18 _tmp1D=*env;struct _tuple18 _tmp1E=_tmp1D;struct Cyc_Toc_TocState*_tmp24;struct _tuple1*_tmp23;void*(*_tmp22)(struct _tuple1*);_LL1: _tmp24=_tmp1E.f1;_tmp23=(_tmp1E.f2)->f1;_tmp22=(_tmp1E.f2)->f2;_LL2:;{
struct _tuple19**v=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp24->aggrs_so_far,_tmp23);
if(v == 0)
return _tmp22(_tmp23);else{
# 191
struct _tuple19*_tmp1F=*v;struct _tuple19*_tmp20=_tmp1F;void*_tmp21;_LL4: _tmp21=_tmp20->f2;_LL5:;
return _tmp21;}};}
# 196
static void*Cyc_Toc_aggrdecl_type(struct _tuple1*q,void*(*type_maker)(struct _tuple1*)){
struct _tuple17 env=({struct _tuple17 _tmp76D;_tmp76D.f1=q,_tmp76D.f2=type_maker;_tmp76D;});
return((void*(*)(struct _tuple17*arg,void*(*f)(struct _RegionHandle*,struct _tuple18*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_aggrdecl_type_body);}static char _tmp25[5U]="curr";
# 209 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc_curr_string={_tmp25,_tmp25,_tmp25 + 5U};static struct _dyneither_ptr*Cyc_Toc_curr_sp=& Cyc_Toc_curr_string;static char _tmp26[4U]="tag";
static struct _dyneither_ptr Cyc_Toc_tag_string={_tmp26,_tmp26,_tmp26 + 4U};static struct _dyneither_ptr*Cyc_Toc_tag_sp=& Cyc_Toc_tag_string;static char _tmp27[4U]="val";
static struct _dyneither_ptr Cyc_Toc_val_string={_tmp27,_tmp27,_tmp27 + 4U};static struct _dyneither_ptr*Cyc_Toc_val_sp=& Cyc_Toc_val_string;static char _tmp28[14U]="_handler_cons";
static struct _dyneither_ptr Cyc_Toc__handler_cons_string={_tmp28,_tmp28,_tmp28 + 14U};static struct _dyneither_ptr*Cyc_Toc__handler_cons_sp=& Cyc_Toc__handler_cons_string;static char _tmp29[8U]="handler";
static struct _dyneither_ptr Cyc_Toc_handler_string={_tmp29,_tmp29,_tmp29 + 8U};static struct _dyneither_ptr*Cyc_Toc_handler_sp=& Cyc_Toc_handler_string;static char _tmp2A[14U]="_RegionHandle";
static struct _dyneither_ptr Cyc_Toc__RegionHandle_string={_tmp2A,_tmp2A,_tmp2A + 14U};static struct _dyneither_ptr*Cyc_Toc__RegionHandle_sp=& Cyc_Toc__RegionHandle_string;static char _tmp2B[17U]="_DynRegionHandle";
static struct _dyneither_ptr Cyc_Toc__DynRegionHandle_string={_tmp2B,_tmp2B,_tmp2B + 17U};static struct _dyneither_ptr*Cyc_Toc__DynRegionHandle_sp=& Cyc_Toc__DynRegionHandle_string;static char _tmp2C[16U]="_DynRegionFrame";
static struct _dyneither_ptr Cyc_Toc__DynRegionFrame_string={_tmp2C,_tmp2C,_tmp2C + 16U};static struct _dyneither_ptr*Cyc_Toc__DynRegionFrame_sp=& Cyc_Toc__DynRegionFrame_string;static char _tmp2D[7U]="_throw";
# 230 "toc.cyc"
static struct _dyneither_ptr Cyc_Toc__throw_str={_tmp2D,_tmp2D,_tmp2D + 7U};static struct _tuple1 Cyc_Toc__throw_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_bnd={0U,& Cyc_Toc__throw_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_re={1U,(void*)& Cyc_Toc__throw_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_ev={0,(void*)& Cyc_Toc__throw_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_e=& Cyc_Toc__throw_ev;static char _tmp2E[7U]="setjmp";
static struct _dyneither_ptr Cyc_Toc_setjmp_str={_tmp2E,_tmp2E,_tmp2E + 7U};static struct _tuple1 Cyc_Toc_setjmp_pr={{.Loc_n={4,0}},& Cyc_Toc_setjmp_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc_setjmp_bnd={0U,& Cyc_Toc_setjmp_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc_setjmp_re={1U,(void*)& Cyc_Toc_setjmp_bnd};static struct Cyc_Absyn_Exp Cyc_Toc_setjmp_ev={0,(void*)& Cyc_Toc_setjmp_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc_setjmp_e=& Cyc_Toc_setjmp_ev;static char _tmp2F[14U]="_push_handler";
static struct _dyneither_ptr Cyc_Toc__push_handler_str={_tmp2F,_tmp2F,_tmp2F + 14U};static struct _tuple1 Cyc_Toc__push_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__push_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_handler_bnd={0U,& Cyc_Toc__push_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_handler_re={1U,(void*)& Cyc_Toc__push_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_handler_ev={0,(void*)& Cyc_Toc__push_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_handler_e=& Cyc_Toc__push_handler_ev;static char _tmp30[13U]="_pop_handler";
static struct _dyneither_ptr Cyc_Toc__pop_handler_str={_tmp30,_tmp30,_tmp30 + 13U};static struct _tuple1 Cyc_Toc__pop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_handler_bnd={0U,& Cyc_Toc__pop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_handler_re={1U,(void*)& Cyc_Toc__pop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_handler_ev={0,(void*)& Cyc_Toc__pop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_handler_e=& Cyc_Toc__pop_handler_ev;static char _tmp31[12U]="_exn_thrown";
static struct _dyneither_ptr Cyc_Toc__exn_thrown_str={_tmp31,_tmp31,_tmp31 + 12U};static struct _tuple1 Cyc_Toc__exn_thrown_pr={{.Loc_n={4,0}},& Cyc_Toc__exn_thrown_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__exn_thrown_bnd={0U,& Cyc_Toc__exn_thrown_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__exn_thrown_re={1U,(void*)& Cyc_Toc__exn_thrown_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__exn_thrown_ev={0,(void*)& Cyc_Toc__exn_thrown_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__exn_thrown_e=& Cyc_Toc__exn_thrown_ev;static char _tmp32[14U]="_npop_handler";
static struct _dyneither_ptr Cyc_Toc__npop_handler_str={_tmp32,_tmp32,_tmp32 + 14U};static struct _tuple1 Cyc_Toc__npop_handler_pr={{.Loc_n={4,0}},& Cyc_Toc__npop_handler_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__npop_handler_bnd={0U,& Cyc_Toc__npop_handler_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__npop_handler_re={1U,(void*)& Cyc_Toc__npop_handler_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__npop_handler_ev={0,(void*)& Cyc_Toc__npop_handler_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__npop_handler_e=& Cyc_Toc__npop_handler_ev;static char _tmp33[12U]="_check_null";
static struct _dyneither_ptr Cyc_Toc__check_null_str={_tmp33,_tmp33,_tmp33 + 12U};static struct _tuple1 Cyc_Toc__check_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_null_bnd={0U,& Cyc_Toc__check_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_null_re={1U,(void*)& Cyc_Toc__check_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_null_ev={0,(void*)& Cyc_Toc__check_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_null_e=& Cyc_Toc__check_null_ev;static char _tmp34[28U]="_check_known_subscript_null";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_null_str={_tmp34,_tmp34,_tmp34 + 28U};static struct _tuple1 Cyc_Toc__check_known_subscript_null_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_null_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_null_bnd={0U,& Cyc_Toc__check_known_subscript_null_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_null_re={1U,(void*)& Cyc_Toc__check_known_subscript_null_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_null_ev={0,(void*)& Cyc_Toc__check_known_subscript_null_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_null_e=& Cyc_Toc__check_known_subscript_null_ev;static char _tmp35[31U]="_check_known_subscript_notnull";
static struct _dyneither_ptr Cyc_Toc__check_known_subscript_notnull_str={_tmp35,_tmp35,_tmp35 + 31U};static struct _tuple1 Cyc_Toc__check_known_subscript_notnull_pr={{.Loc_n={4,0}},& Cyc_Toc__check_known_subscript_notnull_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_known_subscript_notnull_bnd={0U,& Cyc_Toc__check_known_subscript_notnull_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_known_subscript_notnull_re={1U,(void*)& Cyc_Toc__check_known_subscript_notnull_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_known_subscript_notnull_ev={0,(void*)& Cyc_Toc__check_known_subscript_notnull_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_known_subscript_notnull_e=& Cyc_Toc__check_known_subscript_notnull_ev;static char _tmp36[27U]="_check_dyneither_subscript";
static struct _dyneither_ptr Cyc_Toc__check_dyneither_subscript_str={_tmp36,_tmp36,_tmp36 + 27U};static struct _tuple1 Cyc_Toc__check_dyneither_subscript_pr={{.Loc_n={4,0}},& Cyc_Toc__check_dyneither_subscript_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_dyneither_subscript_bnd={0U,& Cyc_Toc__check_dyneither_subscript_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_dyneither_subscript_re={1U,(void*)& Cyc_Toc__check_dyneither_subscript_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_dyneither_subscript_ev={0,(void*)& Cyc_Toc__check_dyneither_subscript_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_dyneither_subscript_e=& Cyc_Toc__check_dyneither_subscript_ev;static char _tmp37[15U]="_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_str={_tmp37,_tmp37,_tmp37 + 15U};static struct _tuple1 Cyc_Toc__dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_bnd={0U,& Cyc_Toc__dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_re={1U,(void*)& Cyc_Toc__dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_ev={0,(void*)& Cyc_Toc__dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_e=& Cyc_Toc__dyneither_ptr_ev;static char _tmp38[15U]="_tag_dyneither";
static struct _dyneither_ptr Cyc_Toc__tag_dyneither_str={_tmp38,_tmp38,_tmp38 + 15U};static struct _tuple1 Cyc_Toc__tag_dyneither_pr={{.Loc_n={4,0}},& Cyc_Toc__tag_dyneither_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__tag_dyneither_bnd={0U,& Cyc_Toc__tag_dyneither_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__tag_dyneither_re={1U,(void*)& Cyc_Toc__tag_dyneither_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__tag_dyneither_ev={0,(void*)& Cyc_Toc__tag_dyneither_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__tag_dyneither_e=& Cyc_Toc__tag_dyneither_ev;static char _tmp39[20U]="_init_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__init_dyneither_ptr_str={_tmp39,_tmp39,_tmp39 + 20U};static struct _tuple1 Cyc_Toc__init_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__init_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__init_dyneither_ptr_bnd={0U,& Cyc_Toc__init_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__init_dyneither_ptr_re={1U,(void*)& Cyc_Toc__init_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__init_dyneither_ptr_ev={0,(void*)& Cyc_Toc__init_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__init_dyneither_ptr_e=& Cyc_Toc__init_dyneither_ptr_ev;static char _tmp3A[21U]="_untag_dyneither_ptr";
static struct _dyneither_ptr Cyc_Toc__untag_dyneither_ptr_str={_tmp3A,_tmp3A,_tmp3A + 21U};static struct _tuple1 Cyc_Toc__untag_dyneither_ptr_pr={{.Loc_n={4,0}},& Cyc_Toc__untag_dyneither_ptr_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__untag_dyneither_ptr_bnd={0U,& Cyc_Toc__untag_dyneither_ptr_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__untag_dyneither_ptr_re={1U,(void*)& Cyc_Toc__untag_dyneither_ptr_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__untag_dyneither_ptr_ev={0,(void*)& Cyc_Toc__untag_dyneither_ptr_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__untag_dyneither_ptr_e=& Cyc_Toc__untag_dyneither_ptr_ev;static char _tmp3B[20U]="_get_dyneither_size";
static struct _dyneither_ptr Cyc_Toc__get_dyneither_size_str={_tmp3B,_tmp3B,_tmp3B + 20U};static struct _tuple1 Cyc_Toc__get_dyneither_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_dyneither_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_dyneither_size_bnd={0U,& Cyc_Toc__get_dyneither_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_dyneither_size_re={1U,(void*)& Cyc_Toc__get_dyneither_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_dyneither_size_ev={0,(void*)& Cyc_Toc__get_dyneither_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_dyneither_size_e=& Cyc_Toc__get_dyneither_size_ev;static char _tmp3C[19U]="_get_zero_arr_size";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_str={_tmp3C,_tmp3C,_tmp3C + 19U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_bnd={0U,& Cyc_Toc__get_zero_arr_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_e=& Cyc_Toc__get_zero_arr_size_ev;static char _tmp3D[24U]="_get_zero_arr_size_char";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_char_str={_tmp3D,_tmp3D,_tmp3D + 24U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_char_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_char_bnd={0U,& Cyc_Toc__get_zero_arr_size_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_char_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_char_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_char_e=& Cyc_Toc__get_zero_arr_size_char_ev;static char _tmp3E[25U]="_get_zero_arr_size_short";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_short_str={_tmp3E,_tmp3E,_tmp3E + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_short_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_short_bnd={0U,& Cyc_Toc__get_zero_arr_size_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_short_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_short_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_short_e=& Cyc_Toc__get_zero_arr_size_short_ev;static char _tmp3F[23U]="_get_zero_arr_size_int";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_int_str={_tmp3F,_tmp3F,_tmp3F + 23U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_int_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_int_bnd={0U,& Cyc_Toc__get_zero_arr_size_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_int_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_int_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_int_e=& Cyc_Toc__get_zero_arr_size_int_ev;static char _tmp40[25U]="_get_zero_arr_size_float";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_float_str={_tmp40,_tmp40,_tmp40 + 25U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_float_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_float_bnd={0U,& Cyc_Toc__get_zero_arr_size_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_float_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_float_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_float_e=& Cyc_Toc__get_zero_arr_size_float_ev;static char _tmp41[26U]="_get_zero_arr_size_double";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_double_str={_tmp41,_tmp41,_tmp41 + 26U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_double_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_double_bnd={0U,& Cyc_Toc__get_zero_arr_size_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_double_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_double_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_double_e=& Cyc_Toc__get_zero_arr_size_double_ev;static char _tmp42[30U]="_get_zero_arr_size_longdouble";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_longdouble_str={_tmp42,_tmp42,_tmp42 + 30U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_longdouble_bnd={0U,& Cyc_Toc__get_zero_arr_size_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_longdouble_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_longdouble_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_longdouble_e=& Cyc_Toc__get_zero_arr_size_longdouble_ev;static char _tmp43[28U]="_get_zero_arr_size_voidstar";
static struct _dyneither_ptr Cyc_Toc__get_zero_arr_size_voidstar_str={_tmp43,_tmp43,_tmp43 + 28U};static struct _tuple1 Cyc_Toc__get_zero_arr_size_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__get_zero_arr_size_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__get_zero_arr_size_voidstar_bnd={0U,& Cyc_Toc__get_zero_arr_size_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__get_zero_arr_size_voidstar_re={1U,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__get_zero_arr_size_voidstar_ev={0,(void*)& Cyc_Toc__get_zero_arr_size_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__get_zero_arr_size_voidstar_e=& Cyc_Toc__get_zero_arr_size_voidstar_ev;static char _tmp44[20U]="_dyneither_ptr_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_plus_str={_tmp44,_tmp44,_tmp44 + 20U};static struct _tuple1 Cyc_Toc__dyneither_ptr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_plus_e=& Cyc_Toc__dyneither_ptr_plus_ev;static char _tmp45[15U]="_zero_arr_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_str={_tmp45,_tmp45,_tmp45 + 15U};static struct _tuple1 Cyc_Toc__zero_arr_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_bnd={0U,& Cyc_Toc__zero_arr_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_re={1U,(void*)& Cyc_Toc__zero_arr_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_ev={0,(void*)& Cyc_Toc__zero_arr_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_e=& Cyc_Toc__zero_arr_plus_ev;static char _tmp46[20U]="_zero_arr_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_char_str={_tmp46,_tmp46,_tmp46 + 20U};static struct _tuple1 Cyc_Toc__zero_arr_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_char_bnd={0U,& Cyc_Toc__zero_arr_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_char_e=& Cyc_Toc__zero_arr_plus_char_ev;static char _tmp47[21U]="_zero_arr_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_short_str={_tmp47,_tmp47,_tmp47 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_short_bnd={0U,& Cyc_Toc__zero_arr_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_short_e=& Cyc_Toc__zero_arr_plus_short_ev;static char _tmp48[19U]="_zero_arr_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_int_str={_tmp48,_tmp48,_tmp48 + 19U};static struct _tuple1 Cyc_Toc__zero_arr_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_int_bnd={0U,& Cyc_Toc__zero_arr_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_int_e=& Cyc_Toc__zero_arr_plus_int_ev;static char _tmp49[21U]="_zero_arr_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_float_str={_tmp49,_tmp49,_tmp49 + 21U};static struct _tuple1 Cyc_Toc__zero_arr_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_float_bnd={0U,& Cyc_Toc__zero_arr_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_float_e=& Cyc_Toc__zero_arr_plus_float_ev;static char _tmp4A[22U]="_zero_arr_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_double_str={_tmp4A,_tmp4A,_tmp4A + 22U};static struct _tuple1 Cyc_Toc__zero_arr_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_double_bnd={0U,& Cyc_Toc__zero_arr_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_double_e=& Cyc_Toc__zero_arr_plus_double_ev;static char _tmp4B[26U]="_zero_arr_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_longdouble_str={_tmp4B,_tmp4B,_tmp4B + 26U};static struct _tuple1 Cyc_Toc__zero_arr_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_longdouble_e=& Cyc_Toc__zero_arr_plus_longdouble_ev;static char _tmp4C[24U]="_zero_arr_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_plus_voidstar_str={_tmp4C,_tmp4C,_tmp4C + 24U};static struct _tuple1 Cyc_Toc__zero_arr_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_plus_voidstar_e=& Cyc_Toc__zero_arr_plus_voidstar_ev;static char _tmp4D[28U]="_dyneither_ptr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_str={_tmp4D,_tmp4D,_tmp4D + 28U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_e=& Cyc_Toc__dyneither_ptr_inplace_plus_ev;static char _tmp4E[23U]="_zero_arr_inplace_plus";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_str={_tmp4E,_tmp4E,_tmp4E + 23U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_e=& Cyc_Toc__zero_arr_inplace_plus_ev;static char _tmp4F[28U]="_zero_arr_inplace_plus_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_char_str={_tmp4F,_tmp4F,_tmp4F + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_char_e=& Cyc_Toc__zero_arr_inplace_plus_char_ev;static char _tmp50[29U]="_zero_arr_inplace_plus_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_short_str={_tmp50,_tmp50,_tmp50 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_short_e=& Cyc_Toc__zero_arr_inplace_plus_short_ev;static char _tmp51[27U]="_zero_arr_inplace_plus_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_int_str={_tmp51,_tmp51,_tmp51 + 27U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_int_e=& Cyc_Toc__zero_arr_inplace_plus_int_ev;static char _tmp52[29U]="_zero_arr_inplace_plus_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_float_str={_tmp52,_tmp52,_tmp52 + 29U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_float_e=& Cyc_Toc__zero_arr_inplace_plus_float_ev;static char _tmp53[30U]="_zero_arr_inplace_plus_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_double_str={_tmp53,_tmp53,_tmp53 + 30U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_double_e=& Cyc_Toc__zero_arr_inplace_plus_double_ev;static char _tmp54[34U]="_zero_arr_inplace_plus_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_longdouble_str={_tmp54,_tmp54,_tmp54 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev;static char _tmp55[32U]="_zero_arr_inplace_plus_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_voidstar_str={_tmp55,_tmp55,_tmp55 + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev;static char _tmp56[33U]="_dyneither_ptr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_inplace_plus_post_str={_tmp56,_tmp56,_tmp56 + 33U};static struct _tuple1 Cyc_Toc__dyneither_ptr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd={0U,& Cyc_Toc__dyneither_ptr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__dyneither_ptr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_inplace_plus_post_e=& Cyc_Toc__dyneither_ptr_inplace_plus_post_ev;static char _tmp57[28U]="_zero_arr_inplace_plus_post";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_str={_tmp57,_tmp57,_tmp57 + 28U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_e=& Cyc_Toc__zero_arr_inplace_plus_post_ev;static char _tmp58[33U]="_zero_arr_inplace_plus_post_char";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_char_str={_tmp58,_tmp58,_tmp58 + 33U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_char_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_char_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_char_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_char_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_char_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_char_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_char_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_char_e=& Cyc_Toc__zero_arr_inplace_plus_post_char_ev;static char _tmp59[34U]="_zero_arr_inplace_plus_post_short";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_short_str={_tmp59,_tmp59,_tmp59 + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_short_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_short_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_short_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_short_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_short_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_short_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_short_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_short_e=& Cyc_Toc__zero_arr_inplace_plus_post_short_ev;static char _tmp5A[32U]="_zero_arr_inplace_plus_post_int";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_int_str={_tmp5A,_tmp5A,_tmp5A + 32U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_int_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_int_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_int_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_int_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_int_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_int_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_int_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_int_e=& Cyc_Toc__zero_arr_inplace_plus_post_int_ev;static char _tmp5B[34U]="_zero_arr_inplace_plus_post_float";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_float_str={_tmp5B,_tmp5B,_tmp5B + 34U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_float_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_float_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_float_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_float_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_float_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_float_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_float_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_float_e=& Cyc_Toc__zero_arr_inplace_plus_post_float_ev;static char _tmp5C[35U]="_zero_arr_inplace_plus_post_double";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_double_str={_tmp5C,_tmp5C,_tmp5C + 35U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_double_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_double_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_double_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_double_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_double_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_double_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_double_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_double_e=& Cyc_Toc__zero_arr_inplace_plus_post_double_ev;static char _tmp5D[39U]="_zero_arr_inplace_plus_post_longdouble";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str={_tmp5D,_tmp5D,_tmp5D + 39U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_longdouble_e=& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev;static char _tmp5E[37U]="_zero_arr_inplace_plus_post_voidstar";
static struct _dyneither_ptr Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str={_tmp5E,_tmp5E,_tmp5E + 37U};static struct _tuple1 Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr={{.Loc_n={4,0}},& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd={0U,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re={1U,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev={0,(void*)& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__zero_arr_inplace_plus_post_voidstar_e=& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev;static char _tmp5F[10U]="_cycalloc";
static struct _dyneither_ptr Cyc_Toc__cycalloc_str={_tmp5F,_tmp5F,_tmp5F + 10U};static struct _tuple1 Cyc_Toc__cycalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_bnd={0U,& Cyc_Toc__cycalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_re={1U,(void*)& Cyc_Toc__cycalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_ev={0,(void*)& Cyc_Toc__cycalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_e=& Cyc_Toc__cycalloc_ev;static char _tmp60[11U]="_cyccalloc";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_str={_tmp60,_tmp60,_tmp60 + 11U};static struct _tuple1 Cyc_Toc__cyccalloc_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_bnd={0U,& Cyc_Toc__cyccalloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_re={1U,(void*)& Cyc_Toc__cyccalloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_ev={0,(void*)& Cyc_Toc__cyccalloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_e=& Cyc_Toc__cyccalloc_ev;static char _tmp61[17U]="_cycalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cycalloc_atomic_str={_tmp61,_tmp61,_tmp61 + 17U};static struct _tuple1 Cyc_Toc__cycalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cycalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cycalloc_atomic_bnd={0U,& Cyc_Toc__cycalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cycalloc_atomic_re={1U,(void*)& Cyc_Toc__cycalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cycalloc_atomic_ev={0,(void*)& Cyc_Toc__cycalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cycalloc_atomic_e=& Cyc_Toc__cycalloc_atomic_ev;static char _tmp62[18U]="_cyccalloc_atomic";
static struct _dyneither_ptr Cyc_Toc__cyccalloc_atomic_str={_tmp62,_tmp62,_tmp62 + 18U};static struct _tuple1 Cyc_Toc__cyccalloc_atomic_pr={{.Loc_n={4,0}},& Cyc_Toc__cyccalloc_atomic_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__cyccalloc_atomic_bnd={0U,& Cyc_Toc__cyccalloc_atomic_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__cyccalloc_atomic_re={1U,(void*)& Cyc_Toc__cyccalloc_atomic_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__cyccalloc_atomic_ev={0,(void*)& Cyc_Toc__cyccalloc_atomic_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__cyccalloc_atomic_e=& Cyc_Toc__cyccalloc_atomic_ev;static char _tmp63[15U]="_region_malloc";
static struct _dyneither_ptr Cyc_Toc__region_malloc_str={_tmp63,_tmp63,_tmp63 + 15U};static struct _tuple1 Cyc_Toc__region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_malloc_bnd={0U,& Cyc_Toc__region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_malloc_re={1U,(void*)& Cyc_Toc__region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_malloc_ev={0,(void*)& Cyc_Toc__region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_malloc_e=& Cyc_Toc__region_malloc_ev;static char _tmp64[15U]="_region_calloc";
static struct _dyneither_ptr Cyc_Toc__region_calloc_str={_tmp64,_tmp64,_tmp64 + 15U};static struct _tuple1 Cyc_Toc__region_calloc_pr={{.Loc_n={4,0}},& Cyc_Toc__region_calloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__region_calloc_bnd={0U,& Cyc_Toc__region_calloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__region_calloc_re={1U,(void*)& Cyc_Toc__region_calloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__region_calloc_ev={0,(void*)& Cyc_Toc__region_calloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__region_calloc_e=& Cyc_Toc__region_calloc_ev;static char _tmp65[13U]="_check_times";
static struct _dyneither_ptr Cyc_Toc__check_times_str={_tmp65,_tmp65,_tmp65 + 13U};static struct _tuple1 Cyc_Toc__check_times_pr={{.Loc_n={4,0}},& Cyc_Toc__check_times_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__check_times_bnd={0U,& Cyc_Toc__check_times_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__check_times_re={1U,(void*)& Cyc_Toc__check_times_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__check_times_ev={0,(void*)& Cyc_Toc__check_times_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__check_times_e=& Cyc_Toc__check_times_ev;static char _tmp66[12U]="_new_region";
static struct _dyneither_ptr Cyc_Toc__new_region_str={_tmp66,_tmp66,_tmp66 + 12U};static struct _tuple1 Cyc_Toc__new_region_pr={{.Loc_n={4,0}},& Cyc_Toc__new_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__new_region_bnd={0U,& Cyc_Toc__new_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__new_region_re={1U,(void*)& Cyc_Toc__new_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__new_region_ev={0,(void*)& Cyc_Toc__new_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__new_region_e=& Cyc_Toc__new_region_ev;static char _tmp67[13U]="_push_region";
static struct _dyneither_ptr Cyc_Toc__push_region_str={_tmp67,_tmp67,_tmp67 + 13U};static struct _tuple1 Cyc_Toc__push_region_pr={{.Loc_n={4,0}},& Cyc_Toc__push_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_region_bnd={0U,& Cyc_Toc__push_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_region_re={1U,(void*)& Cyc_Toc__push_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_region_ev={0,(void*)& Cyc_Toc__push_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_region_e=& Cyc_Toc__push_region_ev;static char _tmp68[12U]="_pop_region";
static struct _dyneither_ptr Cyc_Toc__pop_region_str={_tmp68,_tmp68,_tmp68 + 12U};static struct _tuple1 Cyc_Toc__pop_region_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_region_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_region_bnd={0U,& Cyc_Toc__pop_region_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_region_re={1U,(void*)& Cyc_Toc__pop_region_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_region_ev={0,(void*)& Cyc_Toc__pop_region_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_region_e=& Cyc_Toc__pop_region_ev;static char _tmp69[16U]="_open_dynregion";
static struct _dyneither_ptr Cyc_Toc__open_dynregion_str={_tmp69,_tmp69,_tmp69 + 16U};static struct _tuple1 Cyc_Toc__open_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__open_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__open_dynregion_bnd={0U,& Cyc_Toc__open_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__open_dynregion_re={1U,(void*)& Cyc_Toc__open_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__open_dynregion_ev={0,(void*)& Cyc_Toc__open_dynregion_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__open_dynregion_e=& Cyc_Toc__open_dynregion_ev;static char _tmp6A[16U]="_push_dynregion";
static struct _dyneither_ptr Cyc_Toc__push_dynregion_str={_tmp6A,_tmp6A,_tmp6A + 16U};static struct _tuple1 Cyc_Toc__push_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__push_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__push_dynregion_bnd={0U,& Cyc_Toc__push_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__push_dynregion_re={1U,(void*)& Cyc_Toc__push_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__push_dynregion_ev={0,(void*)& Cyc_Toc__push_dynregion_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__push_dynregion_e=& Cyc_Toc__push_dynregion_ev;static char _tmp6B[15U]="_pop_dynregion";
static struct _dyneither_ptr Cyc_Toc__pop_dynregion_str={_tmp6B,_tmp6B,_tmp6B + 15U};static struct _tuple1 Cyc_Toc__pop_dynregion_pr={{.Loc_n={4,0}},& Cyc_Toc__pop_dynregion_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__pop_dynregion_bnd={0U,& Cyc_Toc__pop_dynregion_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__pop_dynregion_re={1U,(void*)& Cyc_Toc__pop_dynregion_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__pop_dynregion_ev={0,(void*)& Cyc_Toc__pop_dynregion_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__pop_dynregion_e=& Cyc_Toc__pop_dynregion_ev;static char _tmp6C[19U]="_throw_arraybounds";
static struct _dyneither_ptr Cyc_Toc__throw_arraybounds_str={_tmp6C,_tmp6C,_tmp6C + 19U};static struct _tuple1 Cyc_Toc__throw_arraybounds_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_arraybounds_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_arraybounds_bnd={0U,& Cyc_Toc__throw_arraybounds_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_arraybounds_re={1U,(void*)& Cyc_Toc__throw_arraybounds_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_arraybounds_ev={0,(void*)& Cyc_Toc__throw_arraybounds_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_arraybounds_e=& Cyc_Toc__throw_arraybounds_ev;static char _tmp6D[29U]="_dyneither_ptr_decrease_size";
static struct _dyneither_ptr Cyc_Toc__dyneither_ptr_decrease_size_str={_tmp6D,_tmp6D,_tmp6D + 29U};static struct _tuple1 Cyc_Toc__dyneither_ptr_decrease_size_pr={{.Loc_n={4,0}},& Cyc_Toc__dyneither_ptr_decrease_size_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__dyneither_ptr_decrease_size_bnd={0U,& Cyc_Toc__dyneither_ptr_decrease_size_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__dyneither_ptr_decrease_size_re={1U,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__dyneither_ptr_decrease_size_ev={0,(void*)& Cyc_Toc__dyneither_ptr_decrease_size_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__dyneither_ptr_decrease_size_e=& Cyc_Toc__dyneither_ptr_decrease_size_ev;static char _tmp6E[13U]="_throw_match";
static struct _dyneither_ptr Cyc_Toc__throw_match_str={_tmp6E,_tmp6E,_tmp6E + 13U};static struct _tuple1 Cyc_Toc__throw_match_pr={{.Loc_n={4,0}},& Cyc_Toc__throw_match_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__throw_match_bnd={0U,& Cyc_Toc__throw_match_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__throw_match_re={1U,(void*)& Cyc_Toc__throw_match_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__throw_match_ev={0,(void*)& Cyc_Toc__throw_match_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__throw_match_e=& Cyc_Toc__throw_match_ev;static char _tmp6F[9U]="_rethrow";
static struct _dyneither_ptr Cyc_Toc__rethrow_str={_tmp6F,_tmp6F,_tmp6F + 9U};static struct _tuple1 Cyc_Toc__rethrow_pr={{.Loc_n={4,0}},& Cyc_Toc__rethrow_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__rethrow_bnd={0U,& Cyc_Toc__rethrow_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__rethrow_re={1U,(void*)& Cyc_Toc__rethrow_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__rethrow_ev={0,(void*)& Cyc_Toc__rethrow_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__rethrow_e=& Cyc_Toc__rethrow_ev;static char _tmp70[20U]="_fast_region_malloc";
static struct _dyneither_ptr Cyc_Toc__fast_region_malloc_str={_tmp70,_tmp70,_tmp70 + 20U};static struct _tuple1 Cyc_Toc__fast_region_malloc_pr={{.Loc_n={4,0}},& Cyc_Toc__fast_region_malloc_str};static struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct Cyc_Toc__fast_region_malloc_bnd={0U,& Cyc_Toc__fast_region_malloc_pr};static struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct Cyc_Toc__fast_region_malloc_re={1U,(void*)& Cyc_Toc__fast_region_malloc_bnd};static struct Cyc_Absyn_Exp Cyc_Toc__fast_region_malloc_ev={0,(void*)& Cyc_Toc__fast_region_malloc_re,0U,(void*)& Cyc_Absyn_EmptyAnnot_val};static struct Cyc_Absyn_Exp*Cyc_Toc__fast_region_malloc_e=& Cyc_Toc__fast_region_malloc_ev;
# 302
static struct Cyc_Absyn_AggrType_Absyn_Type_struct Cyc_Toc_dyneither_ptr_typ_v={11U,{{.UnknownAggr={1,{Cyc_Absyn_StructA,& Cyc_Toc__dyneither_ptr_pr,0}}},0}};
# 305
static void*Cyc_Toc_dyneither_ptr_typ=(void*)& Cyc_Toc_dyneither_ptr_typ_v;
# 307
static struct Cyc_Absyn_Tqual Cyc_Toc_mt_tq={0,0,0,0,0U};
# 309
static struct Cyc_Absyn_Stmt*Cyc_Toc_skip_stmt_dl(){
return Cyc_Absyn_skip_stmt(0U);}struct _tuple20{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 313
static struct _tuple20*Cyc_Toc_make_field(struct _dyneither_ptr*name,struct Cyc_Absyn_Exp*e){
return({struct _tuple20*_tmp73=_cycalloc(sizeof(*_tmp73));({struct Cyc_List_List*_tmp7B3=({struct Cyc_List_List*_tmp72=_cycalloc(sizeof(*_tmp72));({void*_tmp7B2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp71=_cycalloc(sizeof(*_tmp71));_tmp71->tag=1U,_tmp71->f1=name;_tmp71;});_tmp72->hd=_tmp7B2;}),_tmp72->tl=0;_tmp72;});_tmp73->f1=_tmp7B3;}),_tmp73->f2=e;_tmp73;});}
# 319
static struct Cyc_Absyn_Exp*Cyc_Toc_cast_it(void*t,struct Cyc_Absyn_Exp*e){
void*_tmp74=e->r;void*_tmp75=_tmp74;struct Cyc_Absyn_Exp*_tmp76;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp75)->tag == 14U){if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp75)->f4 == Cyc_Absyn_No_coercion){_LL1: _tmp76=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp75)->f2;_LL2:
 return Cyc_Toc_cast_it(t,_tmp76);}else{goto _LL3;}}else{_LL3: _LL4:
 return Cyc_Absyn_cast_exp(t,e,0,Cyc_Absyn_No_coercion,0U);}_LL0:;}
# 325
static void*Cyc_Toc_cast_it_r(void*t,struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp77=_cycalloc(sizeof(*_tmp77));_tmp77->tag=14U,_tmp77->f1=t,_tmp77->f2=e,_tmp77->f3=0,_tmp77->f4=Cyc_Absyn_No_coercion;_tmp77;});}
# 328
static void*Cyc_Toc_deref_exp_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_tmp78=_cycalloc(sizeof(*_tmp78));_tmp78->tag=20U,_tmp78->f1=e;_tmp78;});}
# 331
static void*Cyc_Toc_subscript_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
return(void*)({struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*_tmp79=_cycalloc(sizeof(*_tmp79));_tmp79->tag=23U,_tmp79->f1=e1,_tmp79->f2=e2;_tmp79;});}
# 334
static void*Cyc_Toc_stmt_exp_r(struct Cyc_Absyn_Stmt*s){
return(void*)({struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*_tmp7A=_cycalloc(sizeof(*_tmp7A));_tmp7A->tag=37U,_tmp7A->f1=s;_tmp7A;});}
# 337
static void*Cyc_Toc_sizeoftyp_exp_r(void*t){
return(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->tag=17U,_tmp7B->f1=t;_tmp7B;});}
# 340
static void*Cyc_Toc_fncall_exp_r(struct Cyc_Absyn_Exp*e,struct Cyc_List_List*es){
return(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp7C=_cycalloc(sizeof(*_tmp7C));_tmp7C->tag=10U,_tmp7C->f1=e,_tmp7C->f2=es,_tmp7C->f3=0,_tmp7C->f4=1;_tmp7C;});}
# 343
static void*Cyc_Toc_exp_stmt_r(struct Cyc_Absyn_Exp*e){
return(void*)({struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_tmp7D=_cycalloc(sizeof(*_tmp7D));_tmp7D->tag=1U,_tmp7D->f1=e;_tmp7D;});}
# 346
static void*Cyc_Toc_seq_stmt_r(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
return(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=2U,_tmp7E->f1=s1,_tmp7E->f2=s2;_tmp7E;});}
# 349
static void*Cyc_Toc_conditional_exp_r(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2,struct Cyc_Absyn_Exp*e3){
return(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=6U,_tmp7F->f1=e1,_tmp7F->f2=e2,_tmp7F->f3=e3;_tmp7F;});}
# 352
static void*Cyc_Toc_aggrmember_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=21U,_tmp80->f1=e,_tmp80->f2=n,_tmp80->f3=0,_tmp80->f4=0;_tmp80;});}
# 355
static void*Cyc_Toc_aggrarrow_exp_r(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*n){
return(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=22U,_tmp81->f1=e,_tmp81->f2=n,_tmp81->f3=0,_tmp81->f4=0;_tmp81;});}
# 358
static void*Cyc_Toc_unresolvedmem_exp_r(struct Cyc_Core_Opt*tdopt,struct Cyc_List_List*ds){
# 361
return(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=36U,_tmp82->f1=tdopt,_tmp82->f2=ds;_tmp82;});}
# 363
static void*Cyc_Toc_goto_stmt_r(struct _dyneither_ptr*v){
return(void*)({struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=8U,_tmp83->f1=v;_tmp83;});}
# 366
static struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct Cyc_Toc_zero_exp={0U,{.Int_c={5,{Cyc_Absyn_Signed,0}}}};
# 370
static struct Cyc_Absyn_Exp*Cyc_Toc_member_exp(struct Cyc_Absyn_Exp*e,struct _dyneither_ptr*f,unsigned int loc){
void*_tmp84=e->r;void*_tmp85=_tmp84;struct Cyc_Absyn_Exp*_tmp86;if(((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp85)->tag == 20U){_LL1: _tmp86=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp85)->f1;_LL2:
 return Cyc_Absyn_aggrarrow_exp(_tmp86,f,loc);}else{_LL3: _LL4:
 return Cyc_Absyn_aggrmember_exp(e,f,loc);}_LL0:;}struct Cyc_Toc_functionSet{struct Cyc_Absyn_Exp*fchar;struct Cyc_Absyn_Exp*fshort;struct Cyc_Absyn_Exp*fint;struct Cyc_Absyn_Exp*ffloat;struct Cyc_Absyn_Exp*fdouble;struct Cyc_Absyn_Exp*flongdouble;struct Cyc_Absyn_Exp*fvoidstar;};
# 389
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_plus_functionSet={& Cyc_Toc__zero_arr_plus_char_ev,& Cyc_Toc__zero_arr_plus_short_ev,& Cyc_Toc__zero_arr_plus_int_ev,& Cyc_Toc__zero_arr_plus_float_ev,& Cyc_Toc__zero_arr_plus_double_ev,& Cyc_Toc__zero_arr_plus_longdouble_ev,& Cyc_Toc__zero_arr_plus_voidstar_ev};
# 398
struct Cyc_Toc_functionSet Cyc_Toc__get_zero_arr_size_functionSet={& Cyc_Toc__get_zero_arr_size_char_ev,& Cyc_Toc__get_zero_arr_size_short_ev,& Cyc_Toc__get_zero_arr_size_int_ev,& Cyc_Toc__get_zero_arr_size_float_ev,& Cyc_Toc__get_zero_arr_size_double_ev,& Cyc_Toc__get_zero_arr_size_longdouble_ev,& Cyc_Toc__get_zero_arr_size_voidstar_ev};
# 407
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_functionSet={& Cyc_Toc__zero_arr_inplace_plus_char_ev,& Cyc_Toc__zero_arr_inplace_plus_short_ev,& Cyc_Toc__zero_arr_inplace_plus_int_ev,& Cyc_Toc__zero_arr_inplace_plus_float_ev,& Cyc_Toc__zero_arr_inplace_plus_double_ev,& Cyc_Toc__zero_arr_inplace_plus_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_voidstar_ev};
# 416
struct Cyc_Toc_functionSet Cyc_Toc__zero_arr_inplace_plus_post_functionSet={& Cyc_Toc__zero_arr_inplace_plus_post_char_ev,& Cyc_Toc__zero_arr_inplace_plus_post_short_ev,& Cyc_Toc__zero_arr_inplace_plus_post_int_ev,& Cyc_Toc__zero_arr_inplace_plus_post_float_ev,& Cyc_Toc__zero_arr_inplace_plus_post_double_ev,& Cyc_Toc__zero_arr_inplace_plus_post_longdouble_ev,& Cyc_Toc__zero_arr_inplace_plus_post_voidstar_ev};
# 427
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionType(struct Cyc_Toc_functionSet*fS,void*t){
struct Cyc_Absyn_Exp*function;
{void*_tmp87=Cyc_Tcutil_compress(t);void*_tmp88=_tmp87;enum Cyc_Absyn_Size_of _tmp8F;switch(*((int*)_tmp88)){case 6U: _LL1: _tmp8F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp88)->f2;_LL2:
# 431
{enum Cyc_Absyn_Size_of _tmp89=_tmp8F;switch(_tmp89){case Cyc_Absyn_Char_sz: _LLE: _LLF:
 function=fS->fchar;goto _LLD;case Cyc_Absyn_Short_sz: _LL10: _LL11:
 function=fS->fshort;goto _LLD;case Cyc_Absyn_Int_sz: _LL12: _LL13:
 function=fS->fint;goto _LLD;default: _LL14: _LL15:
({void*_tmp8A=0U;({struct _dyneither_ptr _tmp7B4=({const char*_tmp8B="impossible IntType (not char, short or int)";_tag_dyneither(_tmp8B,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B4,_tag_dyneither(_tmp8A,sizeof(void*),0U));});});}_LLD:;}
# 437
goto _LL0;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp88)->f1){case 0U: _LL3: _LL4:
 function=fS->ffloat;goto _LL0;case 1U: _LL5: _LL6:
 function=fS->fdouble;goto _LL0;default: _LL7: _LL8:
 function=fS->flongdouble;goto _LL0;}case 5U: _LL9: _LLA:
 function=fS->fvoidstar;goto _LL0;default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp8E=({struct Cyc_String_pa_PrintArg_struct _tmp76E;_tmp76E.tag=0U,({struct _dyneither_ptr _tmp7B5=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp76E.f1=_tmp7B5;});_tmp76E;});void*_tmp8C[1U];_tmp8C[0]=& _tmp8E;({struct _dyneither_ptr _tmp7B6=({const char*_tmp8D="impossible expression type %s (not int, float, double, or pointer)";_tag_dyneither(_tmp8D,sizeof(char),67U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B6,_tag_dyneither(_tmp8C,sizeof(void*),1U));});});}_LL0:;}
# 444
return function;}
# 446
static struct Cyc_Absyn_Exp*Cyc_Toc_getFunctionRemovePointer(struct Cyc_Toc_functionSet*fS,struct Cyc_Absyn_Exp*arr){
void*_tmp90=Cyc_Tcutil_compress((void*)_check_null(arr->topt));void*_tmp91=_tmp90;void*_tmp94;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->tag == 5U){_LL1: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp91)->f1).elt_typ;_LL2:
 return Cyc_Toc_getFunctionType(fS,_tmp94);}else{_LL3: _LL4:
({void*_tmp92=0U;({struct _dyneither_ptr _tmp7B7=({const char*_tmp93="impossible type (not pointer)";_tag_dyneither(_tmp93,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7B7,_tag_dyneither(_tmp92,sizeof(void*),0U));});});}_LL0:;}
# 455
static int Cyc_Toc_is_array_type(void*t){
void*_tmp95=Cyc_Tcutil_compress(t);void*_tmp96=_tmp95;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp96)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 463
static int Cyc_Toc_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp97=e->r;void*_tmp98=_tmp97;struct Cyc_List_List*_tmpA4;struct Cyc_List_List*_tmpA3;struct Cyc_List_List*_tmpA2;struct Cyc_List_List*_tmpA1;struct Cyc_List_List*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;long long _tmp9E;int _tmp9D;short _tmp9C;struct _dyneither_ptr _tmp9B;char _tmp9A;switch(*((int*)_tmp98)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp98)->f1).Null_c).tag){case 2U: _LL1: _tmp9A=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp98)->f1).Char_c).val).f2;_LL2:
 return _tmp9A == '\000';case 3U: _LL3: _tmp9B=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp98)->f1).Wchar_c).val;_LL4: {
# 467
unsigned long _tmp99=Cyc_strlen((struct _dyneither_ptr)_tmp9B);
int i=0;
if(_tmp99 >= 2  && *((const char*)_check_dyneither_subscript(_tmp9B,sizeof(char),0))== '\\'){
if(*((const char*)_check_dyneither_subscript(_tmp9B,sizeof(char),1))== '0')i=2;else{
if((((const char*)_tmp9B.curr)[1]== 'x'  && _tmp99 >= 3) && *((const char*)_check_dyneither_subscript(_tmp9B,sizeof(char),2))== '0')i=3;else{
return 0;}}
for(0;i < _tmp99;++ i){
if(*((const char*)_check_dyneither_subscript(_tmp9B,sizeof(char),i))!= '0')return 0;}
return 1;}else{
# 477
return 0;}}case 4U: _LL5: _tmp9C=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp98)->f1).Short_c).val).f2;_LL6:
 return _tmp9C == 0;case 5U: _LL7: _tmp9D=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp98)->f1).Int_c).val).f2;_LL8:
 return _tmp9D == 0;case 6U: _LL9: _tmp9E=(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp98)->f1).LongLong_c).val).f2;_LLA:
 return _tmp9E == 0;case 1U: _LLD: _LLE:
# 482
 return 1;default: goto _LL1B;}case 2U: _LLB: _LLC:
# 481
 goto _LLE;case 14U: _LLF: _tmp9F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp98)->f2;_LL10:
# 483
 return Cyc_Toc_is_zero(_tmp9F);case 24U: _LL11: _tmpA0=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_LL12:
 return((int(*)(int(*pred)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_forall)(Cyc_Toc_is_zero,_tmpA0);case 26U: _LL13: _tmpA1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp98)->f1;_LL14:
 _tmpA2=_tmpA1;goto _LL16;case 29U: _LL15: _tmpA2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp98)->f3;_LL16:
 _tmpA3=_tmpA2;goto _LL18;case 25U: _LL17: _tmpA3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp98)->f2;_LL18:
 _tmpA4=_tmpA3;goto _LL1A;case 36U: _LL19: _tmpA4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp98)->f2;_LL1A:
# 489
 for(0;_tmpA4 != 0;_tmpA4=_tmpA4->tl){
if(!Cyc_Toc_is_zero((*((struct _tuple20*)_tmpA4->hd)).f2))return 0;}
return 1;default: _LL1B: _LL1C:
 return 0;}_LL0:;}
# 498
static struct _dyneither_ptr Cyc_Toc_collapse_qvar(struct _dyneither_ptr*s,struct _tuple1*x){
struct _tuple1*_tmpA5=x;union Cyc_Absyn_Nmspace _tmpB7;struct _dyneither_ptr*_tmpB6;_LL1: _tmpB7=_tmpA5->f1;_tmpB6=_tmpA5->f2;_LL2:;{
union Cyc_Absyn_Nmspace _tmpA6=_tmpB7;struct Cyc_List_List*_tmpB5;struct Cyc_List_List*_tmpB4;struct Cyc_List_List*_tmpB3;switch((_tmpA6.Abs_n).tag){case 4U: _LL4: _LL5:
 _tmpB3=0;goto _LL7;case 1U: _LL6: _tmpB3=(_tmpA6.Rel_n).val;_LL7:
 _tmpB4=_tmpB3;goto _LL9;case 2U: _LL8: _tmpB4=(_tmpA6.Abs_n).val;_LL9:
 _tmpB5=_tmpB4;goto _LLB;default: _LLA: _tmpB5=(_tmpA6.C_n).val;_LLB:
# 507
 if(_tmpB5 == 0)
return(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpA9=({struct Cyc_String_pa_PrintArg_struct _tmp770;_tmp770.tag=0U,_tmp770.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp770;});struct Cyc_String_pa_PrintArg_struct _tmpAA=({struct Cyc_String_pa_PrintArg_struct _tmp76F;_tmp76F.tag=0U,_tmp76F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB6);_tmp76F;});void*_tmpA7[2U];_tmpA7[0]=& _tmpA9,_tmpA7[1]=& _tmpAA;({struct _dyneither_ptr _tmp7B8=({const char*_tmpA8="%s_%s_struct";_tag_dyneither(_tmpA8,sizeof(char),13U);});Cyc_aprintf(_tmp7B8,_tag_dyneither(_tmpA7,sizeof(void*),2U));});});{
struct _RegionHandle _tmpAB=_new_region("r");struct _RegionHandle*r=& _tmpAB;_push_region(r);
{struct _dyneither_ptr _tmpB2=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpAE=({struct Cyc_String_pa_PrintArg_struct _tmp773;_tmp773.tag=0U,_tmp773.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*s);_tmp773;});struct Cyc_String_pa_PrintArg_struct _tmpAF=({struct Cyc_String_pa_PrintArg_struct _tmp772;_tmp772.tag=0U,({struct _dyneither_ptr _tmp7BB=(struct _dyneither_ptr)((struct _dyneither_ptr)({struct _RegionHandle*_tmp7BA=r;struct Cyc_List_List*_tmp7B9=_tmpB5;Cyc_rstr_sepstr(_tmp7BA,_tmp7B9,({const char*_tmpB1="_";_tag_dyneither(_tmpB1,sizeof(char),2U);}));}));_tmp772.f1=_tmp7BB;});_tmp772;});struct Cyc_String_pa_PrintArg_struct _tmpB0=({struct Cyc_String_pa_PrintArg_struct _tmp771;_tmp771.tag=0U,_tmp771.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmpB6);_tmp771;});void*_tmpAC[3U];_tmpAC[0]=& _tmpAE,_tmpAC[1]=& _tmpAF,_tmpAC[2]=& _tmpB0;({struct _dyneither_ptr _tmp7BC=({const char*_tmpAD="%s_%s_%s_struct";_tag_dyneither(_tmpAD,sizeof(char),16U);});Cyc_aprintf(_tmp7BC,_tag_dyneither(_tmpAC,sizeof(void*),3U));});});_npop_handler(0U);return _tmpB2;};_pop_region(r);};}_LL3:;};}struct _tuple21{struct Cyc_Toc_TocState*f1;struct _tuple15*f2;};
# 520
static struct _tuple1*Cyc_Toc_collapse_qvars_body(struct _RegionHandle*d,struct _tuple21*env){
# 523
struct _tuple21 _tmpB8=*env;struct _tuple21 _tmpB9=_tmpB8;struct Cyc_Dict_Dict*_tmpCC;struct _tuple15*_tmpCB;_LL1: _tmpCC=(_tmpB9.f1)->qvar_tags;_tmpCB=_tmpB9.f2;_LL2:;{
struct _tuple15 _tmpBA=*_tmpCB;struct _tuple15 _tmpBB=_tmpBA;struct _tuple1*_tmpCA;struct _tuple1*_tmpC9;_LL4: _tmpCA=_tmpBB.f1;_tmpC9=_tmpBB.f2;_LL5:;{
struct _handler_cons _tmpBC;_push_handler(& _tmpBC);{int _tmpBE=0;if(setjmp(_tmpBC.handler))_tmpBE=1;if(!_tmpBE){{struct _tuple1*_tmpBF=((struct _tuple1*(*)(struct Cyc_Dict_Dict d,int(*cmp)(struct _tuple15*,struct _tuple15*),struct _tuple15*k))Cyc_Dict_lookup_other)(*_tmpCC,Cyc_Toc_qvar_tag_cmp,_tmpCB);_npop_handler(0U);return _tmpBF;};_pop_handler();}else{void*_tmpBD=(void*)_exn_thrown;void*_tmpC0=_tmpBD;void*_tmpC8;if(((struct Cyc_Dict_Absent_exn_struct*)_tmpC0)->tag == Cyc_Dict_Absent){_LL7: _LL8: {
# 527
struct _tuple15*_tmpC1=({struct _tuple15*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=_tmpCA,_tmpC7->f2=_tmpC9;_tmpC7;});
struct _tuple1*_tmpC2=_tmpCA;union Cyc_Absyn_Nmspace _tmpC6;struct _dyneither_ptr*_tmpC5;_LLC: _tmpC6=_tmpC2->f1;_tmpC5=_tmpC2->f2;_LLD:;{
struct _dyneither_ptr newvar=Cyc_Toc_collapse_qvar(_tmpC5,_tmpC9);
struct _tuple1*res=({struct _tuple1*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->f1=_tmpC6,({struct _dyneither_ptr*_tmp7BD=({struct _dyneither_ptr*_tmpC3=_cycalloc(sizeof(*_tmpC3));*_tmpC3=newvar;_tmpC3;});_tmpC4->f2=_tmp7BD;});_tmpC4;});
({struct Cyc_Dict_Dict _tmp7BE=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple15*k,struct _tuple1*v))Cyc_Dict_insert)(*_tmpCC,_tmpC1,res);*_tmpCC=_tmp7BE;});
return res;};}}else{_LL9: _tmpC8=_tmpC0;_LLA:(int)_rethrow(_tmpC8);}_LL6:;}};};};}
# 536
static struct _tuple1*Cyc_Toc_collapse_qvars(struct _tuple1*fieldname,struct _tuple1*dtname){
struct _tuple15 env=({struct _tuple15 _tmp774;_tmp774.f1=fieldname,_tmp774.f2=dtname;_tmp774;});
return((struct _tuple1*(*)(struct _tuple15*arg,struct _tuple1*(*f)(struct _RegionHandle*,struct _tuple21*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_collapse_qvars_body);}
# 543
static struct Cyc_Absyn_Aggrdecl*Cyc_Toc_make_c_struct_defn(struct _dyneither_ptr*name,struct Cyc_List_List*fs){
return({struct Cyc_Absyn_Aggrdecl*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->kind=Cyc_Absyn_StructA,_tmpCF->sc=Cyc_Absyn_Public,_tmpCF->tvs=0,_tmpCF->attributes=0,_tmpCF->expected_mem_kind=0,({
# 546
struct _tuple1*_tmp7C1=({struct _tuple1*_tmpCD=_cycalloc(sizeof(*_tmpCD));({union Cyc_Absyn_Nmspace _tmp7C0=Cyc_Absyn_Rel_n(0);_tmpCD->f1=_tmp7C0;}),_tmpCD->f2=name;_tmpCD;});_tmpCF->name=_tmp7C1;}),({
struct Cyc_Absyn_AggrdeclImpl*_tmp7BF=({struct Cyc_Absyn_AggrdeclImpl*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->exist_vars=0,_tmpCE->rgn_po=0,_tmpCE->tagged=0,_tmpCE->fields=fs;_tmpCE;});_tmpCF->impl=_tmp7BF;});_tmpCF;});}struct _tuple22{struct Cyc_Toc_TocState*f1;struct Cyc_List_List*f2;};struct _tuple23{void*f1;struct Cyc_List_List*f2;};
# 555
static void*Cyc_Toc_add_tuple_type_body(struct _RegionHandle*d,struct _tuple22*env){
# 558
struct _tuple22 _tmpD0=*env;struct _tuple22 _tmpD1=_tmpD0;struct Cyc_List_List**_tmpE8;struct Cyc_List_List*_tmpE7;_LL1: _tmpE8=(_tmpD1.f1)->tuple_types;_tmpE7=_tmpD1.f2;_LL2:;
# 560
{struct Cyc_List_List*_tmpD2=*_tmpE8;for(0;_tmpD2 != 0;_tmpD2=_tmpD2->tl){
struct _tuple23*_tmpD3=(struct _tuple23*)_tmpD2->hd;struct _tuple23*_tmpD4=_tmpD3;void*_tmpD7;struct Cyc_List_List*_tmpD6;_LL4: _tmpD7=_tmpD4->f1;_tmpD6=_tmpD4->f2;_LL5:;{
struct Cyc_List_List*_tmpD5=_tmpE7;
for(0;_tmpD5 != 0  && _tmpD6 != 0;(_tmpD5=_tmpD5->tl,_tmpD6=_tmpD6->tl)){
if(!Cyc_Tcutil_unify((*((struct _tuple12*)_tmpD5->hd)).f2,(void*)_tmpD6->hd))
break;}
if(_tmpD5 == 0  && _tmpD6 == 0)
return _tmpD7;};}}{
# 571
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpE6=_cycalloc(sizeof(*_tmpE6));({struct _dyneither_ptr _tmp7C3=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpE5=({struct Cyc_Int_pa_PrintArg_struct _tmp775;_tmp775.tag=1U,_tmp775.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp775;});void*_tmpE3[1U];_tmpE3[0]=& _tmpE5;({struct _dyneither_ptr _tmp7C2=({const char*_tmpE4="_tuple%d";_tag_dyneither(_tmpE4,sizeof(char),9U);});Cyc_aprintf(_tmp7C2,_tag_dyneither(_tmpE3,sizeof(void*),1U));});});*_tmpE6=_tmp7C3;});_tmpE6;});
struct Cyc_List_List*ts=((struct Cyc_List_List*(*)(struct _RegionHandle*,void*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_rmap)(d,Cyc_Tcutil_snd_tqt,_tmpE7);
struct Cyc_List_List*_tmpD8=0;
struct Cyc_List_List*ts2=ts;
{int i=1;for(0;ts2 != 0;(ts2=ts2->tl,i ++)){
({struct Cyc_List_List*_tmp7C6=({struct Cyc_List_List*_tmpDA=_cycalloc(sizeof(*_tmpDA));({struct Cyc_Absyn_Aggrfield*_tmp7C5=({struct Cyc_Absyn_Aggrfield*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct _dyneither_ptr*_tmp7C4=Cyc_Absyn_fieldname(i);_tmpD9->name=_tmp7C4;}),_tmpD9->tq=Cyc_Toc_mt_tq,_tmpD9->type=(void*)ts2->hd,_tmpD9->width=0,_tmpD9->attributes=0,_tmpD9->requires_clause=0;_tmpD9;});_tmpDA->hd=_tmp7C5;}),_tmpDA->tl=_tmpD8;_tmpDA;});_tmpD8=_tmp7C6;});}}
({struct Cyc_List_List*_tmp7C7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmpD8);_tmpD8=_tmp7C7;});{
struct Cyc_Absyn_Aggrdecl*_tmpDB=Cyc_Toc_make_c_struct_defn(xname,_tmpD8);
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpDC=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7C8=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpE1=_cycalloc(sizeof(*_tmpE1));*_tmpE1=_tmpDB;_tmpE1;}));(_tmpE2->f1).aggr_info=_tmp7C8;}),(_tmpE2->f1).targs=0;_tmpE2;});
({struct Cyc_List_List*_tmp7CA=({struct Cyc_List_List*_tmpDE=_cycalloc(sizeof(*_tmpDE));({struct Cyc_Absyn_Decl*_tmp7C9=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->tag=5U,_tmpDD->f1=_tmpDB;_tmpDD;}),0U);_tmpDE->hd=_tmp7C9;}),_tmpDE->tl=Cyc_Toc_result_decls;_tmpDE;});Cyc_Toc_result_decls=_tmp7CA;});
({struct Cyc_List_List*_tmp7CC=({struct Cyc_List_List*_tmpE0=_region_malloc(d,sizeof(*_tmpE0));({struct _tuple23*_tmp7CB=({struct _tuple23*_tmpDF=_region_malloc(d,sizeof(*_tmpDF));_tmpDF->f1=(void*)_tmpDC,_tmpDF->f2=ts;_tmpDF;});_tmpE0->hd=_tmp7CB;}),_tmpE0->tl=*_tmpE8;_tmpE0;});*_tmpE8=_tmp7CC;});
return(void*)_tmpDC;};};}
# 584
static void*Cyc_Toc_add_tuple_type(struct Cyc_List_List*tqs0){
return((void*(*)(struct Cyc_List_List*arg,void*(*f)(struct _RegionHandle*,struct _tuple22*)))Cyc_Toc_use_toc_state)(tqs0,Cyc_Toc_add_tuple_type_body);}struct _tuple24{enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct _tuple25{struct Cyc_Toc_TocState*f1;struct _tuple24*f2;};struct _tuple26{void*f1;enum Cyc_Absyn_AggrKind f2;struct Cyc_List_List*f3;};
# 589
static void*Cyc_Toc_add_anon_aggr_type_body(struct _RegionHandle*d,struct _tuple25*env){
# 592
struct _tuple25*_tmpE9=env;struct Cyc_List_List**_tmpFE;enum Cyc_Absyn_AggrKind _tmpFD;struct Cyc_List_List*_tmpFC;_LL1: _tmpFE=(_tmpE9->f1)->anon_aggr_types;_tmpFD=(_tmpE9->f2)->f1;_tmpFC=(_tmpE9->f2)->f2;_LL2:;
# 594
{struct Cyc_List_List*_tmpEA=*_tmpFE;for(0;_tmpEA != 0;_tmpEA=_tmpEA->tl){
struct _tuple26*_tmpEB=(struct _tuple26*)_tmpEA->hd;struct _tuple26*_tmpEC=_tmpEB;void*_tmpEF;enum Cyc_Absyn_AggrKind _tmpEE;struct Cyc_List_List*_tmpED;_LL4: _tmpEF=_tmpEC->f1;_tmpEE=_tmpEC->f2;_tmpED=_tmpEC->f3;_LL5:;
if(_tmpFD != _tmpEE)
continue;
if(!((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmpED,_tmpFC))
return _tmpEF;}}{
# 603
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmpFB=_cycalloc(sizeof(*_tmpFB));({struct _dyneither_ptr _tmp7CE=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpFA=({struct Cyc_Int_pa_PrintArg_struct _tmp776;_tmp776.tag=1U,_tmp776.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp776;});void*_tmpF8[1U];_tmpF8[0]=& _tmpFA;({struct _dyneither_ptr _tmp7CD=({const char*_tmpF9="_tuple%d";_tag_dyneither(_tmpF9,sizeof(char),9U);});Cyc_aprintf(_tmp7CD,_tag_dyneither(_tmpF8,sizeof(void*),1U));});});*_tmpFB=_tmp7CE;});_tmpFB;});
struct Cyc_Absyn_Aggrdecl*_tmpF0=Cyc_Toc_make_c_struct_defn(xname,_tmpFC);
_tmpF0->kind=_tmpFD;{
struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF1=({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmpF7=_cycalloc(sizeof(*_tmpF7));_tmpF7->tag=11U,({union Cyc_Absyn_AggrInfoU _tmp7CF=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmpF6=_cycalloc(sizeof(*_tmpF6));*_tmpF6=_tmpF0;_tmpF6;}));(_tmpF7->f1).aggr_info=_tmp7CF;}),(_tmpF7->f1).targs=0;_tmpF7;});
({struct Cyc_List_List*_tmp7D1=({struct Cyc_List_List*_tmpF3=_cycalloc(sizeof(*_tmpF3));({struct Cyc_Absyn_Decl*_tmp7D0=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->tag=5U,_tmpF2->f1=_tmpF0;_tmpF2;}),0U);_tmpF3->hd=_tmp7D0;}),_tmpF3->tl=Cyc_Toc_result_decls;_tmpF3;});Cyc_Toc_result_decls=_tmp7D1;});
({struct Cyc_List_List*_tmp7D3=({struct Cyc_List_List*_tmpF5=_region_malloc(d,sizeof(*_tmpF5));({struct _tuple26*_tmp7D2=({struct _tuple26*_tmpF4=_region_malloc(d,sizeof(*_tmpF4));_tmpF4->f1=(void*)_tmpF1,_tmpF4->f2=_tmpFD,_tmpF4->f3=_tmpFC;_tmpF4;});_tmpF5->hd=_tmp7D2;}),_tmpF5->tl=*_tmpFE;_tmpF5;});*_tmpFE=_tmp7D3;});
return(void*)_tmpF1;};};}
# 611
static void*Cyc_Toc_add_anon_aggr_type(enum Cyc_Absyn_AggrKind ak,struct Cyc_List_List*fs){
return((void*(*)(struct _tuple24*arg,void*(*f)(struct _RegionHandle*,struct _tuple25*)))Cyc_Toc_use_toc_state)(({struct _tuple24*_tmpFF=_cycalloc(sizeof(*_tmpFF));_tmpFF->f1=ak,_tmpFF->f2=fs;_tmpFF;}),Cyc_Toc_add_anon_aggr_type_body);}struct _tuple27{struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;};struct _tuple28{struct Cyc_Toc_TocState*f1;struct _tuple27*f2;};struct _tuple29{struct _tuple1*f1;struct Cyc_List_List*f2;void*f3;};
# 620
static void*Cyc_Toc_add_struct_type_body(struct _RegionHandle*d,struct _tuple28*env){
# 629
struct _tuple28 _tmp100=*env;struct _tuple28 _tmp101=_tmp100;struct Cyc_List_List**_tmp124;struct _tuple1*_tmp123;struct Cyc_List_List*_tmp122;struct Cyc_List_List*_tmp121;struct Cyc_List_List*_tmp120;_LL1: _tmp124=(_tmp101.f1)->abs_struct_types;_tmp123=(_tmp101.f2)->f1;_tmp122=(_tmp101.f2)->f2;_tmp121=(_tmp101.f2)->f3;_tmp120=(_tmp101.f2)->f4;_LL2:;
# 633
{struct Cyc_List_List*_tmp102=*_tmp124;for(0;_tmp102 != 0;_tmp102=_tmp102->tl){
struct _tuple29*_tmp103=(struct _tuple29*)_tmp102->hd;struct _tuple29*_tmp104=_tmp103;struct _tuple1*_tmp10C;struct Cyc_List_List*_tmp10B;void*_tmp10A;_LL4: _tmp10C=_tmp104->f1;_tmp10B=_tmp104->f2;_tmp10A=_tmp104->f3;_LL5:;
if(Cyc_Absyn_qvar_cmp(_tmp10C,_tmp123)== 0  && ({
int _tmp7D4=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp121);_tmp7D4 == ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp10B);})){
int okay=1;
{struct Cyc_List_List*_tmp105=_tmp121;for(0;_tmp105 != 0;(_tmp105=_tmp105->tl,_tmp10B=_tmp10B->tl)){
void*_tmp106=(void*)_tmp105->hd;
void*_tmp107=(void*)((struct Cyc_List_List*)_check_null(_tmp10B))->hd;
{struct Cyc_Absyn_Kind*_tmp108=Cyc_Tcutil_typ_kind(_tmp106);struct Cyc_Absyn_Kind*_tmp109=_tmp108;switch(((struct Cyc_Absyn_Kind*)_tmp109)->kind){case Cyc_Absyn_EffKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_RgnKind: _LL9: _LLA:
# 644
 continue;default: _LLB: _LLC:
# 647
 if(Cyc_Tcutil_unify(_tmp106,_tmp107) || ({void*_tmp7D5=Cyc_Toc_typ_to_c(_tmp106);Cyc_Tcutil_unify(_tmp7D5,Cyc_Toc_typ_to_c(_tmp107));}))
continue;
okay=0;
goto _LL6;}_LL6:;}
# 652
break;}}
# 654
if(okay)
return _tmp10A;}}}{
# 661
struct _dyneither_ptr*xname=({struct _dyneither_ptr*_tmp11F=_cycalloc(sizeof(*_tmp11F));({struct _dyneither_ptr _tmp7D7=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp11E=({struct Cyc_Int_pa_PrintArg_struct _tmp777;_tmp777.tag=1U,_tmp777.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp777;});void*_tmp11C[1U];_tmp11C[0]=& _tmp11E;({struct _dyneither_ptr _tmp7D6=({const char*_tmp11D="_tuple%d";_tag_dyneither(_tmp11D,sizeof(char),9U);});Cyc_aprintf(_tmp7D6,_tag_dyneither(_tmp11C,sizeof(void*),1U));});});*_tmp11F=_tmp7D7;});_tmp11F;});
void*x=Cyc_Absyn_strct(xname);
struct Cyc_List_List*_tmp10D=0;
# 665
({struct Cyc_List_List*_tmp7D9=({struct Cyc_List_List*_tmp10F=_region_malloc(d,sizeof(*_tmp10F));({struct _tuple29*_tmp7D8=({struct _tuple29*_tmp10E=_region_malloc(d,sizeof(*_tmp10E));_tmp10E->f1=_tmp123,_tmp10E->f2=_tmp121,_tmp10E->f3=x;_tmp10E;});_tmp10F->hd=_tmp7D8;}),_tmp10F->tl=*_tmp124;_tmp10F;});*_tmp124=_tmp7D9;});{
# 668
struct _RegionHandle _tmp110=_new_region("r");struct _RegionHandle*r=& _tmp110;_push_region(r);
{struct Cyc_List_List*_tmp111=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp122,_tmp121);
for(0;_tmp120 != 0;_tmp120=_tmp120->tl){
struct Cyc_Absyn_Aggrfield*_tmp112=(struct Cyc_Absyn_Aggrfield*)_tmp120->hd;
void*t=_tmp112->type;
struct Cyc_List_List*atts=_tmp112->attributes;
# 678
if((_tmp120->tl == 0  && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(t))) && !
Cyc_Toc_is_array_type(t))
({struct Cyc_List_List*_tmp7DB=({struct Cyc_List_List*_tmp114=_cycalloc(sizeof(*_tmp114));({void*_tmp7DA=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->tag=6U,_tmp113->f1=0;_tmp113;});_tmp114->hd=_tmp7DA;}),_tmp114->tl=atts;_tmp114;});atts=_tmp7DB;});
({void*_tmp7DC=Cyc_Toc_typ_to_c(Cyc_Tcutil_rsubstitute(r,_tmp111,t));t=_tmp7DC;});
# 685
if(Cyc_Tcutil_unify(t,(void*)& Cyc_Absyn_VoidType_val))
({void*_tmp7E1=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=8U,({void*_tmp7E0=Cyc_Absyn_void_star_typ();(_tmp115->f1).elt_type=_tmp7E0;}),({struct Cyc_Absyn_Tqual _tmp7DF=Cyc_Absyn_empty_tqual(0U);(_tmp115->f1).tq=_tmp7DF;}),({
struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Absyn_uint_exp(0U,0U);(_tmp115->f1).num_elts=_tmp7DE;}),({union Cyc_Absyn_Constraint*_tmp7DD=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp115->f1).zero_term=_tmp7DD;}),(_tmp115->f1).zt_loc=0U;_tmp115;});
# 686
t=_tmp7E1;});
# 689
({struct Cyc_List_List*_tmp7E3=({struct Cyc_List_List*_tmp117=_cycalloc(sizeof(*_tmp117));({struct Cyc_Absyn_Aggrfield*_tmp7E2=({struct Cyc_Absyn_Aggrfield*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->name=_tmp112->name,_tmp116->tq=Cyc_Toc_mt_tq,_tmp116->type=t,_tmp116->width=_tmp112->width,_tmp116->attributes=atts,_tmp116->requires_clause=0;_tmp116;});_tmp117->hd=_tmp7E2;}),_tmp117->tl=_tmp10D;_tmp117;});_tmp10D=_tmp7E3;});}
# 691
({struct Cyc_List_List*_tmp7E4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp10D);_tmp10D=_tmp7E4;});{
struct Cyc_Absyn_Aggrdecl*_tmp118=Cyc_Toc_make_c_struct_defn(xname,_tmp10D);
({struct Cyc_List_List*_tmp7E6=({struct Cyc_List_List*_tmp11A=_cycalloc(sizeof(*_tmp11A));({struct Cyc_Absyn_Decl*_tmp7E5=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=5U,_tmp119->f1=_tmp118;_tmp119;}),0U);_tmp11A->hd=_tmp7E5;}),_tmp11A->tl=Cyc_Toc_result_decls;_tmp11A;});Cyc_Toc_result_decls=_tmp7E6;});{
void*_tmp11B=x;_npop_handler(0U);return _tmp11B;};};}
# 669
;_pop_region(r);};};}
# 697
static void*Cyc_Toc_add_struct_type(struct _tuple1*struct_name,struct Cyc_List_List*type_vars,struct Cyc_List_List*type_args,struct Cyc_List_List*fields){
# 701
struct _tuple27 env=({struct _tuple27 _tmp778;_tmp778.f1=struct_name,_tmp778.f2=type_vars,_tmp778.f3=type_args,_tmp778.f4=fields;_tmp778;});
return((void*(*)(struct _tuple27*arg,void*(*f)(struct _RegionHandle*,struct _tuple28*)))Cyc_Toc_use_toc_state)(& env,Cyc_Toc_add_struct_type_body);}
# 709
struct _tuple1*Cyc_Toc_temp_var(){
return({struct _tuple1*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->f1=Cyc_Absyn_Loc_n,({struct _dyneither_ptr*_tmp7E9=({struct _dyneither_ptr*_tmp128=_cycalloc(sizeof(*_tmp128));({struct _dyneither_ptr _tmp7E8=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp127=({struct Cyc_Int_pa_PrintArg_struct _tmp779;_tmp779.tag=1U,_tmp779.f1=(unsigned int)Cyc_Toc_temp_var_counter ++;_tmp779;});void*_tmp125[1U];_tmp125[0]=& _tmp127;({struct _dyneither_ptr _tmp7E7=({const char*_tmp126="_tmp%X";_tag_dyneither(_tmp126,sizeof(char),7U);});Cyc_aprintf(_tmp7E7,_tag_dyneither(_tmp125,sizeof(void*),1U));});});*_tmp128=_tmp7E8;});_tmp128;});_tmp129->f2=_tmp7E9;});_tmp129;});}struct _tuple30{struct Cyc_Toc_TocState*f1;int f2;};
# 715
static struct _dyneither_ptr*Cyc_Toc_fresh_label_body(struct _RegionHandle*d,struct _tuple30*env){
struct _tuple30 _tmp12A=*env;struct _tuple30 _tmp12B=_tmp12A;struct Cyc_Xarray_Xarray*_tmp133;_LL1: _tmp133=(_tmp12B.f1)->temp_labels;_LL2:;{
int _tmp12C=Cyc_Toc_fresh_label_counter ++;
if(({int _tmp7EA=_tmp12C;_tmp7EA < ((int(*)(struct Cyc_Xarray_Xarray*))Cyc_Xarray_length)(_tmp133);}))
return((struct _dyneither_ptr*(*)(struct Cyc_Xarray_Xarray*,int))Cyc_Xarray_get)(_tmp133,_tmp12C);{
struct _dyneither_ptr*res=({struct _dyneither_ptr*_tmp132=_cycalloc(sizeof(*_tmp132));({struct _dyneither_ptr _tmp7EC=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp131=({struct Cyc_Int_pa_PrintArg_struct _tmp77A;_tmp77A.tag=1U,_tmp77A.f1=(unsigned int)_tmp12C;_tmp77A;});void*_tmp12F[1U];_tmp12F[0]=& _tmp131;({struct _dyneither_ptr _tmp7EB=({const char*_tmp130="_LL%X";_tag_dyneither(_tmp130,sizeof(char),6U);});Cyc_aprintf(_tmp7EB,_tag_dyneither(_tmp12F,sizeof(void*),1U));});});*_tmp132=_tmp7EC;});_tmp132;});
if(({int _tmp7ED=((int(*)(struct Cyc_Xarray_Xarray*,struct _dyneither_ptr*))Cyc_Xarray_add_ind)(_tmp133,res);_tmp7ED != _tmp12C;}))
({void*_tmp12D=0U;({struct _dyneither_ptr _tmp7EE=({const char*_tmp12E="fresh_label: add_ind returned bad index...";_tag_dyneither(_tmp12E,sizeof(char),43U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7EE,_tag_dyneither(_tmp12D,sizeof(void*),0U));});});
return res;};};}
# 726
static struct _dyneither_ptr*Cyc_Toc_fresh_label(){
return((struct _dyneither_ptr*(*)(int arg,struct _dyneither_ptr*(*f)(struct _RegionHandle*,struct _tuple30*)))Cyc_Toc_use_toc_state)(0,Cyc_Toc_fresh_label_body);}
# 732
static struct Cyc_Absyn_Exp*Cyc_Toc_datatype_tag(struct Cyc_Absyn_Datatypedecl*td,struct _tuple1*name){
int ans=0;
struct Cyc_List_List*_tmp134=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(td->fields))->v;
while(Cyc_Absyn_qvar_cmp(name,((struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp134))->hd)->name)!= 0){
++ ans;
_tmp134=_tmp134->tl;}
# 739
return Cyc_Absyn_uint_exp((unsigned int)ans,0U);}
# 745
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed);
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_result_decls);
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud);
static struct _tuple9*Cyc_Toc_arg_to_c(struct _tuple9*a){
struct _tuple9 _tmp135=*a;struct _tuple9 _tmp136=_tmp135;struct _dyneither_ptr*_tmp13A;struct Cyc_Absyn_Tqual _tmp139;void*_tmp138;_LL1: _tmp13A=_tmp136.f1;_tmp139=_tmp136.f2;_tmp138=_tmp136.f3;_LL2:;
return({struct _tuple9*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->f1=_tmp13A,_tmp137->f2=_tmp139,({void*_tmp7EF=Cyc_Toc_typ_to_c(_tmp138);_tmp137->f3=_tmp7EF;});_tmp137;});}
# 768 "toc.cyc"
static void*Cyc_Toc_typ_to_c_array(void*t){
void*_tmp13B=Cyc_Tcutil_compress(t);void*_tmp13C=_tmp13B;void*_tmp142;void*_tmp141;struct Cyc_Absyn_Tqual _tmp140;struct Cyc_Absyn_Exp*_tmp13F;union Cyc_Absyn_Constraint*_tmp13E;unsigned int _tmp13D;switch(*((int*)_tmp13C)){case 8U: _LL1: _tmp141=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13C)->f1).elt_type;_tmp140=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13C)->f1).tq;_tmp13F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13C)->f1).num_elts;_tmp13E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13C)->f1).zero_term;_tmp13D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp13C)->f1).zt_loc;_LL2:
# 771
 return({void*_tmp7F0=Cyc_Toc_typ_to_c_array(_tmp141);Cyc_Absyn_cstar_typ(_tmp7F0,_tmp140);});case 1U: _LL3: _tmp142=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp13C)->f2;if(_tmp142 != 0){_LL4:
 return Cyc_Toc_typ_to_c_array(_tmp142);}else{goto _LL5;}default: _LL5: _LL6:
 return Cyc_Toc_typ_to_c(t);}_LL0:;}
# 777
static struct Cyc_Absyn_Aggrfield*Cyc_Toc_aggrfield_to_c(struct Cyc_Absyn_Aggrfield*f,void*new_type){
# 779
struct Cyc_Absyn_Aggrfield*_tmp143=({struct Cyc_Absyn_Aggrfield*_tmp144=_cycalloc(sizeof(*_tmp144));*_tmp144=*f;_tmp144;});
_tmp143->type=new_type;
_tmp143->requires_clause=0;
_tmp143->tq=Cyc_Toc_mt_tq;
return _tmp143;}
# 786
static void Cyc_Toc_enumfields_to_c(struct Cyc_List_List*fs){
# 788
return;}
# 791
static void*Cyc_Toc_char_star_typ(){
static void**cs=0;
if(cs == 0)
({void**_tmp7F2=({void**_tmp145=_cycalloc(sizeof(*_tmp145));({void*_tmp7F1=Cyc_Absyn_star_typ(Cyc_Absyn_char_typ,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref);*_tmp145=_tmp7F1;});_tmp145;});cs=_tmp7F2;});
return*cs;}
# 797
static void*Cyc_Toc_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp7F5=({void**_tmp146=_cycalloc(sizeof(*_tmp146));({void*_tmp7F4=({void*_tmp7F3=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7F3,Cyc_Toc_mt_tq);});*_tmp146=_tmp7F4;});_tmp146;});r=_tmp7F5;});
return*r;}
# 803
static void*Cyc_Toc_dyn_rgn_typ(){
static void**r=0;
if(r == 0)
({void**_tmp7F8=({void**_tmp147=_cycalloc(sizeof(*_tmp147));({void*_tmp7F7=({void*_tmp7F6=Cyc_Absyn_strct(Cyc_Toc__DynRegionHandle_sp);Cyc_Absyn_cstar_typ(_tmp7F6,Cyc_Toc_mt_tq);});*_tmp147=_tmp7F7;});_tmp147;});r=_tmp7F8;});
return*r;}
# 809
static int Cyc_Toc_is_boxed_tvar(void*t){
void*_tmp148=Cyc_Tcutil_compress(t);void*_tmp149=_tmp148;struct Cyc_Absyn_Tvar*_tmp14A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->tag == 2U){_LL1: _tmp14A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL2:
 return Cyc_Tcutil_kind_leq(Cyc_Tcutil_typ_kind(t),& Cyc_Tcutil_tbk);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 815
static int Cyc_Toc_is_abstract_type(void*t){
struct Cyc_Absyn_Kind*_tmp14B=Cyc_Tcutil_typ_kind(t);struct Cyc_Absyn_Kind*_tmp14C=_tmp14B;if(((struct Cyc_Absyn_Kind*)_tmp14C)->kind == Cyc_Absyn_AnyKind){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 822
void*Cyc_Toc_typ_to_c(void*t){
void*_tmp14D=t;struct Cyc_Absyn_Datatypedecl*_tmp196;void**_tmp195;struct Cyc_Absyn_Enumdecl*_tmp194;struct Cyc_Absyn_Aggrdecl*_tmp193;struct Cyc_Absyn_Exp*_tmp192;struct Cyc_Absyn_Exp*_tmp191;struct _tuple1*_tmp190;struct Cyc_List_List*_tmp18F;struct Cyc_Absyn_Typedefdecl*_tmp18E;void*_tmp18D;struct Cyc_List_List*_tmp18C;struct _tuple1*_tmp18B;union Cyc_Absyn_AggrInfoU _tmp18A;struct Cyc_List_List*_tmp189;enum Cyc_Absyn_AggrKind _tmp188;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_Tqual _tmp185;void*_tmp184;struct Cyc_List_List*_tmp183;int _tmp182;struct Cyc_Absyn_VarargInfo*_tmp181;struct Cyc_List_List*_tmp180;void*_tmp17F;struct Cyc_Absyn_Tqual _tmp17E;struct Cyc_Absyn_Exp*_tmp17D;unsigned int _tmp17C;void*_tmp17B;struct Cyc_Absyn_Tqual _tmp17A;union Cyc_Absyn_Constraint*_tmp179;struct Cyc_Absyn_Datatypedecl*_tmp178;struct Cyc_Absyn_Datatypefield*_tmp177;struct Cyc_Absyn_Tvar*_tmp176;void**_tmp175;switch(*((int*)_tmp14D)){case 0U: _LL1: _LL2:
 return t;case 1U: _LL3: _tmp175=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14D)->f2;_LL4:
# 826
 if(*_tmp175 == 0){
*_tmp175=Cyc_Absyn_sint_typ;
return Cyc_Absyn_sint_typ;}
# 830
return Cyc_Toc_typ_to_c((void*)_check_null(*_tmp175));case 2U: _LL5: _tmp176=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14D)->f1;_LL6:
# 832
 if((Cyc_Tcutil_tvar_kind(_tmp176,& Cyc_Tcutil_bk))->kind == Cyc_Absyn_AnyKind)
# 834
return(void*)& Cyc_Absyn_VoidType_val;else{
# 836
return Cyc_Absyn_void_star_typ();}case 3U: _LL7: _LL8:
# 838
 return(void*)& Cyc_Absyn_VoidType_val;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp14D)->f1).field_info).KnownDatatypefield).tag == 2){_LL9: _tmp178=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp14D)->f1).field_info).KnownDatatypefield).val).f1;_tmp177=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp14D)->f1).field_info).KnownDatatypefield).val).f2;_LLA:
# 840
 return Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp177->name,_tmp178->name));}else{_LLB: _LLC:
({void*_tmp14E=0U;({struct _dyneither_ptr _tmp7F9=({const char*_tmp14F="unresolved DatatypeFieldType";_tag_dyneither(_tmp14F,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp7F9,_tag_dyneither(_tmp14E,sizeof(void*),0U));});});}case 5U: _LLD: _tmp17B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1).elt_typ;_tmp17A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1).elt_tq;_tmp179=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1).ptr_atts).bounds;_LLE:
# 845
({void*_tmp7FA=Cyc_Toc_typ_to_c(_tmp17B);_tmp17B=_tmp7FA;});{
void*_tmp150=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)((void*)& Cyc_Absyn_DynEither_b_val,_tmp179);void*_tmp151=_tmp150;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp151)->tag == 0U){_LL42: _LL43:
 return Cyc_Toc_dyneither_ptr_typ;}else{_LL44: _LL45:
 return Cyc_Absyn_star_typ(_tmp17B,(void*)& Cyc_Absyn_HeapRgn_val,_tmp17A,Cyc_Absyn_false_conref);}_LL41:;};case 6U: _LLF: _LL10:
# 850
 goto _LL12;case 7U: _LL11: _LL12:
 return t;case 8U: _LL13: _tmp17F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14D)->f1).elt_type;_tmp17E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14D)->f1).tq;_tmp17D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14D)->f1).num_elts;_tmp17C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14D)->f1).zt_loc;_LL14:
# 853
 return({void*_tmp7FE=Cyc_Toc_typ_to_c(_tmp17F);struct Cyc_Absyn_Tqual _tmp7FD=_tmp17E;struct Cyc_Absyn_Exp*_tmp7FC=_tmp17D;union Cyc_Absyn_Constraint*_tmp7FB=Cyc_Absyn_false_conref;Cyc_Absyn_array_typ(_tmp7FE,_tmp7FD,_tmp7FC,_tmp7FB,_tmp17C);});case 9U: _LL15: _tmp185=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp14D)->f1).ret_tqual;_tmp184=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp14D)->f1).ret_typ;_tmp183=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp14D)->f1).args;_tmp182=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp14D)->f1).c_varargs;_tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp14D)->f1).cyc_varargs;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp14D)->f1).attributes;_LL16: {
# 859
struct Cyc_List_List*_tmp152=0;
for(0;_tmp180 != 0;_tmp180=_tmp180->tl){
void*_tmp153=(void*)_tmp180->hd;void*_tmp154=_tmp153;switch(*((int*)_tmp154)){case 4U: _LL47: _LL48:
 goto _LL4A;case 5U: _LL49: _LL4A:
 goto _LL4C;case 19U: _LL4B: _LL4C:
 continue;case 22U: _LL4D: _LL4E:
 continue;case 21U: _LL4F: _LL50:
 continue;case 20U: _LL51: _LL52:
 continue;default: _LL53: _LL54:
({struct Cyc_List_List*_tmp7FF=({struct Cyc_List_List*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->hd=(void*)_tmp180->hd,_tmp155->tl=_tmp152;_tmp155;});_tmp152=_tmp7FF;});goto _LL46;}_LL46:;}{
# 870
struct Cyc_List_List*_tmp156=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_arg_to_c,_tmp183);
if(_tmp181 != 0){
# 873
void*_tmp157=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp181->type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_false_conref));
struct _tuple9*_tmp158=({struct _tuple9*_tmp15A=_cycalloc(sizeof(*_tmp15A));_tmp15A->f1=_tmp181->name,_tmp15A->f2=_tmp181->tq,_tmp15A->f3=_tmp157;_tmp15A;});
({struct Cyc_List_List*_tmp801=({struct Cyc_List_List*_tmp800=_tmp156;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp800,({struct Cyc_List_List*_tmp159=_cycalloc(sizeof(*_tmp159));_tmp159->hd=_tmp158,_tmp159->tl=0;_tmp159;}));});_tmp156=_tmp801;});}
# 877
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp15B=_cycalloc(sizeof(*_tmp15B));_tmp15B->tag=9U,(_tmp15B->f1).tvars=0,(_tmp15B->f1).effect=0,(_tmp15B->f1).ret_tqual=_tmp185,({void*_tmp802=Cyc_Toc_typ_to_c(_tmp184);(_tmp15B->f1).ret_typ=_tmp802;}),(_tmp15B->f1).args=_tmp156,(_tmp15B->f1).c_varargs=_tmp182,(_tmp15B->f1).cyc_varargs=0,(_tmp15B->f1).rgn_po=0,(_tmp15B->f1).attributes=_tmp152,(_tmp15B->f1).requires_clause=0,(_tmp15B->f1).requires_relns=0,(_tmp15B->f1).ensures_clause=0,(_tmp15B->f1).ensures_relns=0;_tmp15B;});};}case 10U: _LL17: _tmp186=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp14D)->f1;_LL18: {
# 882
struct Cyc_List_List*_tmp15C=0;
for(0;_tmp186 != 0;_tmp186=_tmp186->tl){
({struct Cyc_List_List*_tmp805=({struct Cyc_List_List*_tmp15E=_cycalloc(sizeof(*_tmp15E));({struct _tuple12*_tmp804=({struct _tuple12*_tmp15D=_cycalloc(sizeof(*_tmp15D));_tmp15D->f1=(*((struct _tuple12*)_tmp186->hd)).f1,({void*_tmp803=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp186->hd)).f2);_tmp15D->f2=_tmp803;});_tmp15D;});_tmp15E->hd=_tmp804;}),_tmp15E->tl=_tmp15C;_tmp15E;});_tmp15C=_tmp805;});}
return Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15C));}case 12U: _LL19: _tmp188=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14D)->f1;_tmp187=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp14D)->f2;_LL1A: {
# 891
struct Cyc_List_List*_tmp15F=0;
for(0;_tmp187 != 0;_tmp187=_tmp187->tl){
({struct Cyc_List_List*_tmp808=({struct Cyc_List_List*_tmp160=_cycalloc(sizeof(*_tmp160));({struct Cyc_Absyn_Aggrfield*_tmp807=({struct Cyc_Absyn_Aggrfield*_tmp806=(struct Cyc_Absyn_Aggrfield*)_tmp187->hd;Cyc_Toc_aggrfield_to_c(_tmp806,Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Aggrfield*)_tmp187->hd)->type));});_tmp160->hd=_tmp807;}),_tmp160->tl=_tmp15F;_tmp160;});_tmp15F=_tmp808;});}
return({enum Cyc_Absyn_AggrKind _tmp809=_tmp188;Cyc_Toc_add_anon_aggr_type(_tmp809,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp15F));});}case 11U: _LL1B: _tmp18A=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp14D)->f1).aggr_info;_tmp189=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp14D)->f1).targs;_LL1C:
# 897
{union Cyc_Absyn_AggrInfoU _tmp161=_tmp18A;if((_tmp161.UnknownAggr).tag == 1){_LL56: _LL57:
 return t;}else{_LL58: _LL59:
 goto _LL55;}_LL55:;}{
# 901
struct Cyc_Absyn_Aggrdecl*_tmp162=Cyc_Absyn_get_known_aggrdecl(_tmp18A);
if(_tmp162->expected_mem_kind){
# 904
if(_tmp162->impl == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp165=({struct Cyc_String_pa_PrintArg_struct _tmp77C;_tmp77C.tag=0U,({
struct _dyneither_ptr _tmp80A=(struct _dyneither_ptr)(_tmp162->kind == Cyc_Absyn_UnionA?({const char*_tmp167="union";_tag_dyneither(_tmp167,sizeof(char),6U);}):({const char*_tmp168="struct";_tag_dyneither(_tmp168,sizeof(char),7U);}));_tmp77C.f1=_tmp80A;});_tmp77C;});struct Cyc_String_pa_PrintArg_struct _tmp166=({struct Cyc_String_pa_PrintArg_struct _tmp77B;_tmp77B.tag=0U,({
struct _dyneither_ptr _tmp80B=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp162->name));_tmp77B.f1=_tmp80B;});_tmp77B;});void*_tmp163[2U];_tmp163[0]=& _tmp165,_tmp163[1]=& _tmp166;({struct _dyneither_ptr _tmp80C=({const char*_tmp164="%s %s was never defined.";_tag_dyneither(_tmp164,sizeof(char),25U);});Cyc_Tcutil_warn(0U,_tmp80C,_tag_dyneither(_tmp163,sizeof(void*),2U));});});}
# 912
if(_tmp162->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp162->name,Cyc_Absyn_unionq_typ);{
struct Cyc_List_List*_tmp169=_tmp162->impl == 0?0:((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp162->impl))->fields;
if(_tmp169 == 0)return Cyc_Toc_aggrdecl_type(_tmp162->name,Cyc_Absyn_strctq);
for(0;((struct Cyc_List_List*)_check_null(_tmp169))->tl != 0;_tmp169=_tmp169->tl){;}{
void*_tmp16A=((struct Cyc_Absyn_Aggrfield*)_tmp169->hd)->type;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp16A))){
if(_tmp162->expected_mem_kind)
({struct Cyc_String_pa_PrintArg_struct _tmp16D=({struct Cyc_String_pa_PrintArg_struct _tmp77D;_tmp77D.tag=0U,({struct _dyneither_ptr _tmp80D=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_qvar2string(_tmp162->name));_tmp77D.f1=_tmp80D;});_tmp77D;});void*_tmp16B[1U];_tmp16B[0]=& _tmp16D;({struct _dyneither_ptr _tmp80E=({const char*_tmp16C="struct %s ended up being abstract.";_tag_dyneither(_tmp16C,sizeof(char),35U);});Cyc_Tcutil_warn(0U,_tmp80E,_tag_dyneither(_tmp16B,sizeof(void*),1U));});});{
# 925
struct _RegionHandle _tmp16E=_new_region("r");struct _RegionHandle*r=& _tmp16E;_push_region(r);
{struct Cyc_List_List*_tmp16F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp162->tvs,_tmp189);
void*_tmp170=Cyc_Tcutil_rsubstitute(r,_tmp16F,_tmp16A);
if(Cyc_Toc_is_abstract_type(_tmp170)){void*_tmp171=Cyc_Toc_aggrdecl_type(_tmp162->name,Cyc_Absyn_strctq);_npop_handler(0U);return _tmp171;}{
void*_tmp172=Cyc_Toc_add_struct_type(_tmp162->name,_tmp162->tvs,_tmp189,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp162->impl))->fields);_npop_handler(0U);return _tmp172;};}
# 926
;_pop_region(r);};}
# 931
return Cyc_Toc_aggrdecl_type(_tmp162->name,Cyc_Absyn_strctq);};};};case 13U: _LL1D: _tmp18B=((struct Cyc_Absyn_EnumType_Absyn_Type_struct*)_tmp14D)->f1;_LL1E:
 return t;case 14U: _LL1F: _tmp18C=((struct Cyc_Absyn_AnonEnumType_Absyn_Type_struct*)_tmp14D)->f1;_LL20:
 Cyc_Toc_enumfields_to_c(_tmp18C);return t;case 17U: _LL21: _tmp190=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14D)->f1;_tmp18F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14D)->f2;_tmp18E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14D)->f3;_tmp18D=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14D)->f4;_LL22:
# 937
 if(_tmp18D == 0){
if(_tmp18F != 0)
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp173=_cycalloc(sizeof(*_tmp173));_tmp173->tag=17U,_tmp173->f1=_tmp190,_tmp173->f2=0,_tmp173->f3=_tmp18E,_tmp173->f4=0;_tmp173;});else{
return t;}}else{
# 942
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp174=_cycalloc(sizeof(*_tmp174));_tmp174->tag=17U,_tmp174->f1=_tmp190,_tmp174->f2=0,_tmp174->f3=_tmp18E,({void*_tmp80F=Cyc_Toc_typ_to_c(_tmp18D);_tmp174->f4=_tmp80F;});_tmp174;});}case 19U: _LL23: _LL24:
 return Cyc_Absyn_uint_typ;case 15U: _LL25: _LL26:
 return Cyc_Toc_rgn_typ();case 16U: _LL27: _LL28:
 return Cyc_Toc_dyn_rgn_typ();case 20U: _LL29: _LL2A:
# 948
 goto _LL2C;case 21U: _LL2B: _LL2C:
 goto _LL2E;case 22U: _LL2D: _LL2E:
 goto _LL30;case 23U: _LL2F: _LL30:
 goto _LL32;case 24U: _LL31: _LL32:
 goto _LL34;case 25U: _LL33: _LL34:
 return Cyc_Absyn_void_star_typ();case 18U: _LL35: _tmp191=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp14D)->f1;_LL36:
# 958
 return t;case 27U: _LL37: _tmp192=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp14D)->f1;_LL38:
# 960
 return t;case 28U: _LL39: _LL3A:
 return t;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14D)->f1)->r)){case 0U: _LL3B: _tmp193=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14D)->f1)->r)->f1;_LL3C:
# 963
 Cyc_Toc_aggrdecl_to_c(_tmp193,1);
if(_tmp193->kind == Cyc_Absyn_UnionA)
return Cyc_Toc_aggrdecl_type(_tmp193->name,Cyc_Absyn_unionq_typ);else{
return Cyc_Toc_aggrdecl_type(_tmp193->name,Cyc_Absyn_strctq);}case 1U: _LL3D: _tmp194=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14D)->f1)->r)->f1;_LL3E:
# 968
 Cyc_Toc_enumdecl_to_c(_tmp194);
return t;default: _LL3F: _tmp196=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14D)->f1)->r)->f1;_tmp195=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14D)->f2;_LL40:
# 971
 Cyc_Toc_datatypedecl_to_c(_tmp196);
return Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp195)));}}_LL0:;}
# 976
static struct Cyc_Absyn_Exp*Cyc_Toc_array_to_ptr_cast(void*t,struct Cyc_Absyn_Exp*e,unsigned int l){
void*_tmp197=t;void*_tmp199;struct Cyc_Absyn_Tqual _tmp198;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->tag == 8U){_LL1: _tmp199=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).elt_type;_tmp198=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp197)->f1).tq;_LL2:
# 979
 return({void*_tmp810=Cyc_Absyn_star_typ(_tmp199,(void*)& Cyc_Absyn_HeapRgn_val,_tmp198,Cyc_Absyn_false_conref);Cyc_Toc_cast_it(_tmp810,e);});}else{_LL3: _LL4:
 return Cyc_Toc_cast_it(t,e);}_LL0:;}
# 986
static int Cyc_Toc_atomic_typ(void*t){
void*_tmp19A=Cyc_Tcutil_compress(t);void*_tmp19B=_tmp19A;struct Cyc_List_List*_tmp1A7;struct Cyc_Absyn_Datatypedecl*_tmp1A6;struct Cyc_Absyn_Datatypefield*_tmp1A5;struct Cyc_List_List*_tmp1A4;union Cyc_Absyn_AggrInfoU _tmp1A3;void*_tmp1A2;switch(*((int*)_tmp19B)){case 0U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 0;case 6U: _LL5: _LL6:
 goto _LL8;case 13U: _LL7: _LL8:
 goto _LLA;case 14U: _LL9: _LLA:
 goto _LLC;case 7U: _LLB: _LLC:
 goto _LLE;case 9U: _LLD: _LLE:
 goto _LL10;case 19U: _LLF: _LL10:
 return 1;case 8U: _LL11: _tmp1A2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp19B)->f1).elt_type;_LL12:
 return Cyc_Toc_atomic_typ(_tmp1A2);case 11U: _LL13: _tmp1A3=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp19B)->f1).aggr_info;_LL14:
# 1003
{union Cyc_Absyn_AggrInfoU _tmp19C=_tmp1A3;if((_tmp19C.UnknownAggr).tag == 1){_LL26: _LL27:
 return 0;}else{_LL28: _LL29:
 goto _LL25;}_LL25:;}{
# 1007
struct Cyc_Absyn_Aggrdecl*_tmp19D=Cyc_Absyn_get_known_aggrdecl(_tmp1A3);
if(_tmp19D->impl == 0)
return 0;
{struct Cyc_List_List*_tmp19E=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp19D->impl))->fields;for(0;_tmp19E != 0;_tmp19E=_tmp19E->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp19E->hd)->type))return 0;}}
return 1;};case 12U: _LL15: _tmp1A4=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp19B)->f2;_LL16:
# 1014
 for(0;_tmp1A4 != 0;_tmp1A4=_tmp1A4->tl){
if(!Cyc_Toc_atomic_typ(((struct Cyc_Absyn_Aggrfield*)_tmp1A4->hd)->type))return 0;}
return 1;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp19B)->f1).field_info).KnownDatatypefield).tag == 2){_LL17: _tmp1A6=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp19B)->f1).field_info).KnownDatatypefield).val).f1;_tmp1A5=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp19B)->f1).field_info).KnownDatatypefield).val).f2;_LL18:
# 1018
 _tmp1A7=_tmp1A5->typs;goto _LL1A;}else{goto _LL23;}case 10U: _LL19: _tmp1A7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp19B)->f1;_LL1A:
# 1020
 for(0;_tmp1A7 != 0;_tmp1A7=_tmp1A7->tl){
if(!Cyc_Toc_atomic_typ((*((struct _tuple12*)_tmp1A7->hd)).f2))return 0;}
return 1;case 3U: _LL1B: _LL1C:
# 1025
 goto _LL1E;case 5U: _LL1D: _LL1E:
 goto _LL20;case 16U: _LL1F: _LL20:
 goto _LL22;case 15U: _LL21: _LL22:
 return 0;default: _LL23: _LL24:
({struct Cyc_String_pa_PrintArg_struct _tmp1A1=({struct Cyc_String_pa_PrintArg_struct _tmp77E;_tmp77E.tag=0U,({struct _dyneither_ptr _tmp811=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp77E.f1=_tmp811;});_tmp77E;});void*_tmp19F[1U];_tmp19F[0]=& _tmp1A1;({struct _dyneither_ptr _tmp812=({const char*_tmp1A0="atomic_typ:  bad type %s";_tag_dyneither(_tmp1A0,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp812,_tag_dyneither(_tmp19F,sizeof(void*),1U));});});}_LL0:;}
# 1033
static int Cyc_Toc_is_void_star(void*t){
void*_tmp1A8=Cyc_Tcutil_compress(t);void*_tmp1A9=_tmp1A8;void*_tmp1AC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A9)->tag == 5U){_LL1: _tmp1AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1A9)->f1).elt_typ;_LL2: {
# 1036
void*_tmp1AA=Cyc_Tcutil_compress(_tmp1AC);void*_tmp1AB=_tmp1AA;if(((struct Cyc_Absyn_VoidType_Absyn_Type_struct*)_tmp1AB)->tag == 0U){_LL6: _LL7:
 return 1;}else{_LL8: _LL9:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 1040
 return 0;}_LL0:;}
# 1044
static int Cyc_Toc_is_void_star_or_boxed_tvar(void*t){
return Cyc_Toc_is_void_star(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1048
static int Cyc_Toc_is_pointer_or_boxed_tvar(void*t){
return Cyc_Tcutil_is_pointer_type(t) || Cyc_Toc_is_boxed_tvar(t);}
# 1053
static int Cyc_Toc_is_poly_field(void*t,struct _dyneither_ptr*f){
void*_tmp1AD=Cyc_Tcutil_compress(t);void*_tmp1AE=_tmp1AD;struct Cyc_List_List*_tmp1BA;union Cyc_Absyn_AggrInfoU _tmp1B9;switch(*((int*)_tmp1AE)){case 11U: _LL1: _tmp1B9=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp1AE)->f1).aggr_info;_LL2: {
# 1056
struct Cyc_Absyn_Aggrdecl*_tmp1AF=Cyc_Absyn_get_known_aggrdecl(_tmp1B9);
if(_tmp1AF->impl == 0)
({void*_tmp1B0=0U;({struct _dyneither_ptr _tmp813=({const char*_tmp1B1="is_poly_field: type missing fields";_tag_dyneither(_tmp1B1,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp813,_tag_dyneither(_tmp1B0,sizeof(void*),0U));});});
_tmp1BA=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp1AF->impl))->fields;goto _LL4;}case 12U: _LL3: _tmp1BA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1AE)->f2;_LL4: {
# 1061
struct Cyc_Absyn_Aggrfield*_tmp1B2=Cyc_Absyn_lookup_field(_tmp1BA,f);
if(_tmp1B2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1B5=({struct Cyc_String_pa_PrintArg_struct _tmp77F;_tmp77F.tag=0U,_tmp77F.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp77F;});void*_tmp1B3[1U];_tmp1B3[0]=& _tmp1B5;({struct _dyneither_ptr _tmp814=({const char*_tmp1B4="is_poly_field: bad field %s";_tag_dyneither(_tmp1B4,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp814,_tag_dyneither(_tmp1B3,sizeof(void*),1U));});});
return Cyc_Toc_is_void_star_or_boxed_tvar(_tmp1B2->type);}default: _LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp1B8=({struct Cyc_String_pa_PrintArg_struct _tmp780;_tmp780.tag=0U,({struct _dyneither_ptr _tmp815=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp780.f1=_tmp815;});_tmp780;});void*_tmp1B6[1U];_tmp1B6[0]=& _tmp1B8;({struct _dyneither_ptr _tmp816=({const char*_tmp1B7="is_poly_field: bad type %s";_tag_dyneither(_tmp1B7,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp816,_tag_dyneither(_tmp1B6,sizeof(void*),1U));});});}_LL0:;}
# 1072
static int Cyc_Toc_is_poly_project(struct Cyc_Absyn_Exp*e){
void*_tmp1BB=e->r;void*_tmp1BC=_tmp1BB;struct Cyc_Absyn_Exp*_tmp1C6;struct _dyneither_ptr*_tmp1C5;struct Cyc_Absyn_Exp*_tmp1C4;struct _dyneither_ptr*_tmp1C3;switch(*((int*)_tmp1BC)){case 21U: _LL1: _tmp1C4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1BC)->f1;_tmp1C3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp1BC)->f2;_LL2:
# 1075
 return Cyc_Toc_is_poly_field((void*)_check_null(_tmp1C4->topt),_tmp1C3) && !
Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));case 22U: _LL3: _tmp1C6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1BC)->f1;_tmp1C5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp1BC)->f2;_LL4: {
# 1078
void*_tmp1BD=Cyc_Tcutil_compress((void*)_check_null(_tmp1C6->topt));void*_tmp1BE=_tmp1BD;void*_tmp1C2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BE)->tag == 5U){_LL8: _tmp1C2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1BE)->f1).elt_typ;_LL9:
# 1080
 return Cyc_Toc_is_poly_field(_tmp1C2,_tmp1C5) && !Cyc_Toc_is_void_star_or_boxed_tvar((void*)_check_null(e->topt));}else{_LLA: _LLB:
({struct Cyc_String_pa_PrintArg_struct _tmp1C1=({struct Cyc_String_pa_PrintArg_struct _tmp781;_tmp781.tag=0U,({struct _dyneither_ptr _tmp817=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(_tmp1C6->topt)));_tmp781.f1=_tmp817;});_tmp781;});void*_tmp1BF[1U];_tmp1BF[0]=& _tmp1C1;({struct _dyneither_ptr _tmp818=({const char*_tmp1C0="is_poly_project: bad type %s";_tag_dyneither(_tmp1C0,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp818,_tag_dyneither(_tmp1BF,sizeof(void*),1U));});});}_LL7:;}default: _LL5: _LL6:
# 1083
 return 0;}_LL0:;}
# 1088
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_ptr(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp819=Cyc_Toc__cycalloc_e;Cyc_Absyn_fncall_exp(_tmp819,({struct Cyc_List_List*_tmp1C7=_cycalloc(sizeof(*_tmp1C7));_tmp1C7->hd=s,_tmp1C7->tl=0;_tmp1C7;}),0U);});}
# 1092
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_atomic(struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp81A=Cyc_Toc__cycalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp81A,({struct Cyc_List_List*_tmp1C8=_cycalloc(sizeof(*_tmp1C8));_tmp1C8->hd=s,_tmp1C8->tl=0;_tmp1C8;}),0U);});}
# 1096
static struct Cyc_Absyn_Exp*Cyc_Toc_malloc_exp(void*t,struct Cyc_Absyn_Exp*s){
if(Cyc_Toc_atomic_typ(t))
return Cyc_Toc_malloc_atomic(s);
return Cyc_Toc_malloc_ptr(s);}
# 1102
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp81B=Cyc_Toc__region_malloc_e;Cyc_Absyn_fncall_exp(_tmp81B,({struct Cyc_Absyn_Exp*_tmp1C9[2U];_tmp1C9[0]=rgn,_tmp1C9[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1C9,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1106
static struct Cyc_Absyn_Exp*Cyc_Toc_rmalloc_inline_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s){
return({struct Cyc_Absyn_Exp*_tmp81C=Cyc_Toc__fast_region_malloc_e;Cyc_Absyn_fncall_exp(_tmp81C,({struct Cyc_Absyn_Exp*_tmp1CA[2U];_tmp1CA[0]=rgn,_tmp1CA[1]=s;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CA,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}
# 1110
static struct Cyc_Absyn_Exp*Cyc_Toc_calloc_exp(void*elt_type,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
if(Cyc_Toc_atomic_typ(elt_type))
return({struct Cyc_Absyn_Exp*_tmp81D=Cyc_Toc__cyccalloc_atomic_e;Cyc_Absyn_fncall_exp(_tmp81D,({struct Cyc_Absyn_Exp*_tmp1CB[2U];_tmp1CB[0]=s,_tmp1CB[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CB,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});else{
# 1114
return({struct Cyc_Absyn_Exp*_tmp81E=Cyc_Toc__cyccalloc_e;Cyc_Absyn_fncall_exp(_tmp81E,({struct Cyc_Absyn_Exp*_tmp1CC[2U];_tmp1CC[0]=s,_tmp1CC[1]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CC,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});}}
# 1117
static struct Cyc_Absyn_Exp*Cyc_Toc_rcalloc_exp(struct Cyc_Absyn_Exp*rgn,struct Cyc_Absyn_Exp*s,struct Cyc_Absyn_Exp*n){
return({struct Cyc_Absyn_Exp*_tmp81F=Cyc_Toc__region_calloc_e;Cyc_Absyn_fncall_exp(_tmp81F,({struct Cyc_Absyn_Exp*_tmp1CD[3U];_tmp1CD[0]=rgn,_tmp1CD[1]=s,_tmp1CD[2]=n;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1CD,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});}
# 1121
static struct Cyc_Absyn_Exp*Cyc_Toc_newthrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp820=Cyc_Toc__throw_e;Cyc_Absyn_fncall_exp(_tmp820,({struct Cyc_List_List*_tmp1CE=_cycalloc(sizeof(*_tmp1CE));_tmp1CE->hd=e,_tmp1CE->tl=0;_tmp1CE;}),0U);});}
# 1124
static struct Cyc_Absyn_Exp*Cyc_Toc_newrethrow_exp(struct Cyc_Absyn_Exp*e){
return({struct Cyc_Absyn_Exp*_tmp821=Cyc_Toc__rethrow_e;Cyc_Absyn_fncall_exp(_tmp821,({struct Cyc_List_List*_tmp1CF=_cycalloc(sizeof(*_tmp1CF));_tmp1CF->hd=e,_tmp1CF->tl=0;_tmp1CF;}),0U);});}
# 1128
static struct Cyc_Absyn_Stmt*Cyc_Toc_throw_match_stmt(){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmp822=Cyc_Toc__throw_match_e;Cyc_Absyn_fncall_exp(_tmp822,({void*_tmp1D0=0U;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D0,sizeof(struct Cyc_Absyn_Exp*),0U));}),0U);}),0U);}
# 1133
static struct Cyc_Absyn_Exp*Cyc_Toc_make_toplevel_dyn_arr(void*t,struct Cyc_Absyn_Exp*sz,struct Cyc_Absyn_Exp*e){
# 1141
int is_string=0;
{void*_tmp1D1=e->r;void*_tmp1D2=_tmp1D1;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1D2)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp1D2)->f1).Wstring_c).tag){case 8U: _LL1: _LL2:
 goto _LL4;case 9U: _LL3: _LL4:
 is_string=1;goto _LL0;default: goto _LL5;}else{_LL5: _LL6:
 goto _LL0;}_LL0:;}{
# 1147
struct Cyc_Absyn_Exp*xexp;
struct Cyc_Absyn_Exp*xplussz;
if(is_string){
struct _tuple1*x=Cyc_Toc_temp_var();
void*vd_typ=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,sz,Cyc_Absyn_false_conref,0U);
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_static_vardecl(x,vd_typ,e);
({struct Cyc_List_List*_tmp824=({struct Cyc_List_List*_tmp1D4=_cycalloc(sizeof(*_tmp1D4));({struct Cyc_Absyn_Decl*_tmp823=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp1D3=_cycalloc(sizeof(*_tmp1D3));_tmp1D3->tag=0U,_tmp1D3->f1=vd;_tmp1D3;}),0U);_tmp1D4->hd=_tmp823;}),_tmp1D4->tl=Cyc_Toc_result_decls;_tmp1D4;});Cyc_Toc_result_decls=_tmp824;});
({struct Cyc_Absyn_Exp*_tmp825=Cyc_Absyn_var_exp(x,0U);xexp=_tmp825;});
({struct Cyc_Absyn_Exp*_tmp826=Cyc_Absyn_add_exp(xexp,sz,0U);xplussz=_tmp826;});}else{
# 1157
({struct Cyc_Absyn_Exp*_tmp828=({void*_tmp827=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp827,e);});xexp=_tmp828;});
# 1159
({struct Cyc_Absyn_Exp*_tmp82A=({void*_tmp829=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp829,Cyc_Absyn_add_exp(e,sz,0U));});xplussz=_tmp82A;});}
# 1161
return Cyc_Absyn_unresolvedmem_exp(0,({struct _tuple20*_tmp1D5[3U];({struct _tuple20*_tmp82D=({struct _tuple20*_tmp1D6=_cycalloc(sizeof(*_tmp1D6));_tmp1D6->f1=0,_tmp1D6->f2=xexp;_tmp1D6;});_tmp1D5[0]=_tmp82D;}),({
struct _tuple20*_tmp82C=({struct _tuple20*_tmp1D7=_cycalloc(sizeof(*_tmp1D7));_tmp1D7->f1=0,_tmp1D7->f2=xexp;_tmp1D7;});_tmp1D5[1]=_tmp82C;}),({
struct _tuple20*_tmp82B=({struct _tuple20*_tmp1D8=_cycalloc(sizeof(*_tmp1D8));_tmp1D8->f1=0,_tmp1D8->f2=xplussz;_tmp1D8;});_tmp1D5[2]=_tmp82B;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp1D5,sizeof(struct _tuple20*),3U));}),0U);};}struct Cyc_Toc_FallthruInfo{struct _dyneither_ptr*label;struct Cyc_List_List*binders;};struct Cyc_Toc_Env{struct _dyneither_ptr**break_lab;struct _dyneither_ptr**continue_lab;struct Cyc_Toc_FallthruInfo*fallthru_info;int toplevel;int*in_lhs;int*in_sizeof;struct _RegionHandle*rgn;};
# 1198 "toc.cyc"
static int Cyc_Toc_is_toplevel(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1D9=nv;int _tmp1DA;_LL1: _tmp1DA=_tmp1D9->toplevel;_LL2:;
return _tmp1DA;}
# 1202
static int Cyc_Toc_in_lhs(struct Cyc_Toc_Env*nv){
struct Cyc_Toc_Env*_tmp1DB=nv;int*_tmp1DC;_LL1: _tmp1DC=_tmp1DB->in_lhs;_LL2:;
return*_tmp1DC;}
# 1208
static struct Cyc_Toc_Env*Cyc_Toc_empty_env(struct _RegionHandle*r){
return({struct Cyc_Toc_Env*_tmp1DF=_region_malloc(r,sizeof(*_tmp1DF));_tmp1DF->break_lab=0,_tmp1DF->continue_lab=0,_tmp1DF->fallthru_info=0,_tmp1DF->toplevel=1,({
# 1213
int*_tmp82F=({int*_tmp1DD=_region_malloc(r,sizeof(*_tmp1DD));*_tmp1DD=0;_tmp1DD;});_tmp1DF->in_lhs=_tmp82F;}),({
int*_tmp82E=({int*_tmp1DE=_region_malloc(r,sizeof(*_tmp1DE));*_tmp1DE=0;_tmp1DE;});_tmp1DF->in_sizeof=_tmp82E;}),_tmp1DF->rgn=r;_tmp1DF;});}
# 1218
static struct Cyc_Toc_Env*Cyc_Toc_share_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E0=e;struct _dyneither_ptr**_tmp1E7;struct _dyneither_ptr**_tmp1E6;struct Cyc_Toc_FallthruInfo*_tmp1E5;int _tmp1E4;int*_tmp1E3;int*_tmp1E2;_LL1: _tmp1E7=_tmp1E0->break_lab;_tmp1E6=_tmp1E0->continue_lab;_tmp1E5=_tmp1E0->fallthru_info;_tmp1E4=_tmp1E0->toplevel;_tmp1E3=_tmp1E0->in_lhs;_tmp1E2=_tmp1E0->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E1=_region_malloc(r,sizeof(*_tmp1E1));_tmp1E1->break_lab=_tmp1E7,_tmp1E1->continue_lab=_tmp1E6,_tmp1E1->fallthru_info=_tmp1E5,_tmp1E1->toplevel=_tmp1E4,_tmp1E1->in_lhs=_tmp1E3,_tmp1E1->in_sizeof=_tmp1E2,_tmp1E1->rgn=r;_tmp1E1;});}
# 1223
static struct Cyc_Toc_Env*Cyc_Toc_clear_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1E8=e;struct _dyneither_ptr**_tmp1EF;struct _dyneither_ptr**_tmp1EE;struct Cyc_Toc_FallthruInfo*_tmp1ED;int _tmp1EC;int*_tmp1EB;int*_tmp1EA;_LL1: _tmp1EF=_tmp1E8->break_lab;_tmp1EE=_tmp1E8->continue_lab;_tmp1ED=_tmp1E8->fallthru_info;_tmp1EC=_tmp1E8->toplevel;_tmp1EB=_tmp1E8->in_lhs;_tmp1EA=_tmp1E8->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1E9=_region_malloc(r,sizeof(*_tmp1E9));_tmp1E9->break_lab=_tmp1EF,_tmp1E9->continue_lab=_tmp1EE,_tmp1E9->fallthru_info=_tmp1ED,_tmp1E9->toplevel=0,_tmp1E9->in_lhs=_tmp1EB,_tmp1E9->in_sizeof=_tmp1EA,_tmp1E9->rgn=r;_tmp1E9;});}
# 1227
static struct Cyc_Toc_Env*Cyc_Toc_set_toplevel(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1F0=e;struct _dyneither_ptr**_tmp1F7;struct _dyneither_ptr**_tmp1F6;struct Cyc_Toc_FallthruInfo*_tmp1F5;int _tmp1F4;int*_tmp1F3;int*_tmp1F2;_LL1: _tmp1F7=_tmp1F0->break_lab;_tmp1F6=_tmp1F0->continue_lab;_tmp1F5=_tmp1F0->fallthru_info;_tmp1F4=_tmp1F0->toplevel;_tmp1F3=_tmp1F0->in_lhs;_tmp1F2=_tmp1F0->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp1F1=_region_malloc(r,sizeof(*_tmp1F1));_tmp1F1->break_lab=_tmp1F7,_tmp1F1->continue_lab=_tmp1F6,_tmp1F1->fallthru_info=_tmp1F5,_tmp1F1->toplevel=1,_tmp1F1->in_lhs=_tmp1F3,_tmp1F1->in_sizeof=_tmp1F2,_tmp1F1->rgn=r;_tmp1F1;});}
# 1231
static void Cyc_Toc_set_lhs(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1F8=e;int*_tmp1F9;_LL1: _tmp1F9=_tmp1F8->in_lhs;_LL2:;
*_tmp1F9=b;}
# 1235
static int Cyc_Toc_set_in_sizeof(struct Cyc_Toc_Env*e,int b){
struct Cyc_Toc_Env*_tmp1FA=e;int*_tmp1FC;_LL1: _tmp1FC=_tmp1FA->in_sizeof;_LL2:;{
int _tmp1FB=*_tmp1FC;
*_tmp1FC=b;
return _tmp1FB;};}
# 1241
static int Cyc_Toc_in_sizeof(struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FD=e;int*_tmp1FE;_LL1: _tmp1FE=_tmp1FD->in_sizeof;_LL2:;
return*_tmp1FE;}
# 1248
static struct Cyc_Toc_Env*Cyc_Toc_loop_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e){
struct Cyc_Toc_Env*_tmp1FF=e;struct _dyneither_ptr**_tmp206;struct _dyneither_ptr**_tmp205;struct Cyc_Toc_FallthruInfo*_tmp204;int _tmp203;int*_tmp202;int*_tmp201;_LL1: _tmp206=_tmp1FF->break_lab;_tmp205=_tmp1FF->continue_lab;_tmp204=_tmp1FF->fallthru_info;_tmp203=_tmp1FF->toplevel;_tmp202=_tmp1FF->in_lhs;_tmp201=_tmp1FF->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp200=_region_malloc(r,sizeof(*_tmp200));_tmp200->break_lab=0,_tmp200->continue_lab=0,_tmp200->fallthru_info=_tmp204,_tmp200->toplevel=_tmp203,_tmp200->in_lhs=_tmp202,_tmp200->in_sizeof=_tmp201,_tmp200->rgn=r;_tmp200;});}
# 1254
static struct Cyc_Toc_Env*Cyc_Toc_non_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l,struct _dyneither_ptr*fallthru_l,struct Cyc_List_List*fallthru_binders){
# 1258
struct Cyc_Toc_Env*_tmp207=e;struct _dyneither_ptr**_tmp210;struct _dyneither_ptr**_tmp20F;struct Cyc_Toc_FallthruInfo*_tmp20E;int _tmp20D;int*_tmp20C;int*_tmp20B;_LL1: _tmp210=_tmp207->break_lab;_tmp20F=_tmp207->continue_lab;_tmp20E=_tmp207->fallthru_info;_tmp20D=_tmp207->toplevel;_tmp20C=_tmp207->in_lhs;_tmp20B=_tmp207->in_sizeof;_LL2:;{
struct Cyc_Toc_FallthruInfo*fi=({struct Cyc_Toc_FallthruInfo*_tmp20A=_region_malloc(r,sizeof(*_tmp20A));
_tmp20A->label=fallthru_l,_tmp20A->binders=fallthru_binders;_tmp20A;});
return({struct Cyc_Toc_Env*_tmp209=_region_malloc(r,sizeof(*_tmp209));({struct _dyneither_ptr**_tmp830=({struct _dyneither_ptr**_tmp208=_region_malloc(r,sizeof(*_tmp208));*_tmp208=break_l;_tmp208;});_tmp209->break_lab=_tmp830;}),_tmp209->continue_lab=_tmp20F,_tmp209->fallthru_info=fi,_tmp209->toplevel=_tmp20D,_tmp209->in_lhs=_tmp20C,_tmp209->in_sizeof=_tmp20B,_tmp209->rgn=r;_tmp209;});};}
# 1264
static struct Cyc_Toc_Env*Cyc_Toc_last_switchclause_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*break_l){
# 1266
struct Cyc_Toc_Env*_tmp211=e;struct _dyneither_ptr**_tmp219;struct _dyneither_ptr**_tmp218;struct Cyc_Toc_FallthruInfo*_tmp217;int _tmp216;int*_tmp215;int*_tmp214;_LL1: _tmp219=_tmp211->break_lab;_tmp218=_tmp211->continue_lab;_tmp217=_tmp211->fallthru_info;_tmp216=_tmp211->toplevel;_tmp215=_tmp211->in_lhs;_tmp214=_tmp211->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp213=_region_malloc(r,sizeof(*_tmp213));({struct _dyneither_ptr**_tmp831=({struct _dyneither_ptr**_tmp212=_region_malloc(r,sizeof(*_tmp212));*_tmp212=break_l;_tmp212;});_tmp213->break_lab=_tmp831;}),_tmp213->continue_lab=_tmp218,_tmp213->fallthru_info=0,_tmp213->toplevel=_tmp216,_tmp213->in_lhs=_tmp215,_tmp213->in_sizeof=_tmp214,_tmp213->rgn=r;_tmp213;});}
# 1273
static struct Cyc_Toc_Env*Cyc_Toc_switch_as_switch_env(struct _RegionHandle*r,struct Cyc_Toc_Env*e,struct _dyneither_ptr*next_l){
# 1276
struct Cyc_Toc_Env*_tmp21A=e;struct _dyneither_ptr**_tmp222;struct _dyneither_ptr**_tmp221;struct Cyc_Toc_FallthruInfo*_tmp220;int _tmp21F;int*_tmp21E;int*_tmp21D;_LL1: _tmp222=_tmp21A->break_lab;_tmp221=_tmp21A->continue_lab;_tmp220=_tmp21A->fallthru_info;_tmp21F=_tmp21A->toplevel;_tmp21E=_tmp21A->in_lhs;_tmp21D=_tmp21A->in_sizeof;_LL2:;
return({struct Cyc_Toc_Env*_tmp21C=_region_malloc(r,sizeof(*_tmp21C));_tmp21C->break_lab=0,_tmp21C->continue_lab=_tmp221,({struct Cyc_Toc_FallthruInfo*_tmp832=({struct Cyc_Toc_FallthruInfo*_tmp21B=_region_malloc(r,sizeof(*_tmp21B));_tmp21B->label=next_l,_tmp21B->binders=0;_tmp21B;});_tmp21C->fallthru_info=_tmp832;}),_tmp21C->toplevel=_tmp21F,_tmp21C->in_lhs=_tmp21E,_tmp21C->in_sizeof=_tmp21D,_tmp21C->rgn=r;_tmp21C;});}
# 1289 "toc.cyc"
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e);
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s);
# 1292
static int Cyc_Toc_need_null_check(struct Cyc_Absyn_Exp*e){
# 1294
void*_tmp223=e->annot;void*_tmp224=_tmp223;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp224)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _LL2: {
# 1297
void*_tmp225=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp226=_tmp225;union Cyc_Absyn_Constraint*_tmp229;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp226)->tag == 5U){_LLC: _tmp229=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp226)->f1).ptr_atts).nullable;_LLD:
# 1300
 return((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp229);}else{_LLE: _LLF:
({void*_tmp227=0U;({struct _dyneither_ptr _tmp833=({const char*_tmp228="Toc::is_nullable";_tag_dyneither(_tmp228,sizeof(char),17U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp833,_tag_dyneither(_tmp227,sizeof(void*),0U));});});}_LLB:;}}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp224)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _LL4:
# 1305
 return 0;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp224)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1307
({void*_tmp22A=0U;({unsigned int _tmp835=e->loc;struct _dyneither_ptr _tmp834=({const char*_tmp22B="NULL pointer check will definitely fail";_tag_dyneither(_tmp22B,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp835,_tmp834,_tag_dyneither(_tmp22A,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp224)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
# 1313
 return 1;}else{_LL9: _LLA:
({void*_tmp22C=0U;({struct _dyneither_ptr _tmp836=({const char*_tmp22D="need_null_check";_tag_dyneither(_tmp22D,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp836,_tag_dyneither(_tmp22C,sizeof(void*),0U));});});}}}}_LL0:;}
# 1318
static struct Cyc_List_List*Cyc_Toc_get_relns(struct Cyc_Absyn_Exp*e){
void*_tmp22E=e->annot;void*_tmp22F=_tmp22E;struct Cyc_List_List*_tmp235;struct Cyc_List_List*_tmp234;if(((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22F)->tag == Cyc_CfFlowInfo_UnknownZ){_LL1: _tmp234=((struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct*)_tmp22F)->f1;_LL2:
 return _tmp234;}else{if(((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22F)->tag == Cyc_CfFlowInfo_NotZero){_LL3: _tmp235=((struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct*)_tmp22F)->f1;_LL4:
 return _tmp235;}else{if(((struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct*)_tmp22F)->tag == Cyc_CfFlowInfo_IsZero){_LL5: _LL6:
# 1323
({void*_tmp230=0U;({unsigned int _tmp838=e->loc;struct _dyneither_ptr _tmp837=({const char*_tmp231="dereference of NULL pointer";_tag_dyneither(_tmp231,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp838,_tmp837,_tag_dyneither(_tmp230,sizeof(void*),0U));});});
return 0;}else{if(((struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct*)_tmp22F)->tag == Cyc_Absyn_EmptyAnnot){_LL7: _LL8:
 return 0;}else{_LL9: _LLA:
({void*_tmp232=0U;({struct _dyneither_ptr _tmp839=({const char*_tmp233="get_relns";_tag_dyneither(_tmp233,sizeof(char),10U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp839,_tag_dyneither(_tmp232,sizeof(void*),0U));});});}}}}_LL0:;}static char _tmp239[8U]="*bogus*";
# 1333
static int Cyc_Toc_check_bounds(void*a_typ,struct Cyc_List_List*relns,struct Cyc_Absyn_Exp*a,struct Cyc_Absyn_Exp*i){
# 1343 "toc.cyc"
union Cyc_Relations_RelnOp rop_i=Cyc_Relations_RConst(0U);
int valid_rop_i=Cyc_Relations_exp2relnop(i,& rop_i);
# 1346
struct Cyc_Absyn_Vardecl*x;
{void*_tmp236=a->r;void*_tmp237=_tmp236;void*_tmp23A;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp237)->tag == 1U){_LL1: _tmp23A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp237)->f1;_LL2: {
# 1349
struct Cyc_Absyn_Vardecl*_tmp238=Cyc_Tcutil_nonesc_vardecl(_tmp23A);
if(_tmp238 == 0)goto _LL4;
x=_tmp238;
goto _LL0;}}else{_LL3: _LL4: {
# 1355
static struct _dyneither_ptr bogus_string={_tmp239,_tmp239,_tmp239 + 8U};
static struct _tuple1 bogus_qvar={{.Loc_n={4,0}},& bogus_string};
# 1358
static struct Cyc_Absyn_Vardecl bogus_vardecl={Cyc_Absyn_Public,& bogus_qvar,0U,{0,0,0,0,0U},(void*)& Cyc_Absyn_VoidType_val,0,0,0,0};
# 1364
x=& bogus_vardecl;
x->type=a_typ;}}_LL0:;}{
# 1367
void*_tmp23B=a_typ;
union Cyc_Relations_RelnOp rop_a=Cyc_Relations_RNumelts(x);
# 1372
struct Cyc_Absyn_Exp*bound=Cyc_Tcutil_get_type_bound(_tmp23B);
if(bound != 0){
union Cyc_Relations_RelnOp rop_bound=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(bound,& rop_bound))
({struct Cyc_List_List*_tmp83A=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_bound,Cyc_Relations_Rlte,rop_a,relns);relns=_tmp83A;});}
# 1381
if(valid_rop_i){
struct Cyc_List_List*_tmp23C=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlte,rop_i,relns);
# 1389
if(!Cyc_Relations_consistent_relations(_tmp23C))return 1;}
# 1392
inner_loop: {
void*_tmp23D=i->r;void*_tmp23E=_tmp23D;struct Cyc_Absyn_Exp*_tmp243;struct Cyc_Absyn_Exp*_tmp242;void*_tmp241;struct Cyc_Absyn_Exp*_tmp240;switch(*((int*)_tmp23E)){case 14U: _LL6: _tmp241=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23E)->f1;_tmp240=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp23E)->f2;_LL7:
 i=_tmp240;goto inner_loop;case 3U: if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23E)->f1 == Cyc_Absyn_Mod){if(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23E)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23E)->f2)->tl != 0){_LL8: _tmp243=(struct Cyc_Absyn_Exp*)(((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23E)->f2)->hd;_tmp242=(struct Cyc_Absyn_Exp*)((((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp23E)->f2)->tl)->hd;_LL9: {
# 1398
union Cyc_Relations_RelnOp rop_z=Cyc_Relations_RConst(0U);
if(Cyc_Relations_exp2relnop(_tmp242,& rop_z)){
# 1401
struct Cyc_List_List*_tmp23F=Cyc_Relations_add_relation(Cyc_Core_heap_region,rop_a,Cyc_Relations_Rlt,rop_z,relns);
# 1403
return !Cyc_Relations_consistent_relations(_tmp23F);}
# 1405
goto _LL5;}}else{goto _LLA;}}else{goto _LLA;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1408
return 0;};}
# 1411
static void*Cyc_Toc_get_cyc_typ(struct Cyc_Absyn_Exp*e){
if(e->topt == 0)({void*_tmp244=0U;({struct _dyneither_ptr _tmp83B=({const char*_tmp245="Missing type in primop ";_tag_dyneither(_tmp245,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp83B,_tag_dyneither(_tmp244,sizeof(void*),0U));});});
return(void*)_check_null(e->topt);}
# 1415
static struct _tuple12*Cyc_Toc_tup_to_c(struct Cyc_Absyn_Exp*e){
return({struct _tuple12*_tmp246=_cycalloc(sizeof(*_tmp246));_tmp246->f1=Cyc_Toc_mt_tq,({void*_tmp83C=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));_tmp246->f2=_tmp83C;});_tmp246;});}
# 1420
static struct Cyc_Absyn_Exp*Cyc_Toc_array_length_exp(struct Cyc_Absyn_Exp*e){
void*_tmp247=e->r;void*_tmp248=_tmp247;struct Cyc_Absyn_Exp*_tmp250;int _tmp24F;struct Cyc_Absyn_Exp*_tmp24E;int _tmp24D;struct Cyc_List_List*_tmp24C;switch(*((int*)_tmp248)){case 26U: _LL1: _tmp24C=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp248)->f1;_LL2:
# 1423
{struct Cyc_List_List*_tmp249=_tmp24C;for(0;_tmp249 != 0;_tmp249=_tmp249->tl){
if((*((struct _tuple20*)_tmp249->hd)).f1 != 0)
({void*_tmp24A=0U;({struct _dyneither_ptr _tmp83D=({const char*_tmp24B="array designators for abstract-field initialization";_tag_dyneither(_tmp24B,sizeof(char),52U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp83D,_tag_dyneither(_tmp24A,sizeof(void*),0U));});});}}
({struct Cyc_Absyn_Exp*_tmp83E=Cyc_Absyn_signed_int_exp(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp24C),0U);_tmp24E=_tmp83E;});_tmp24D=0;goto _LL4;case 27U: _LL3: _tmp24E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp248)->f2;_tmp24D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp248)->f4;_LL4:
 _tmp250=_tmp24E;_tmp24F=_tmp24D;goto _LL6;case 28U: _LL5: _tmp250=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp248)->f1;_tmp24F=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp248)->f3;_LL6:
# 1429
({struct Cyc_Absyn_Exp*_tmp83F=Cyc_Absyn_copy_exp(_tmp250);_tmp250=_tmp83F;});
return _tmp24F?({struct Cyc_Absyn_Exp*_tmp840=_tmp250;Cyc_Absyn_add_exp(_tmp840,Cyc_Absyn_uint_exp(1U,0U),0U);}): _tmp250;default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1438
static struct Cyc_Absyn_Exp*Cyc_Toc_get_varsizeexp(struct Cyc_Absyn_Exp*e){
# 1446
struct Cyc_List_List*dles;
struct Cyc_List_List*field_types;
{void*_tmp251=e->r;void*_tmp252=_tmp251;struct Cyc_List_List*_tmp253;if(((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp252)->tag == 29U){_LL1: _tmp253=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp252)->f3;_LL2:
 dles=_tmp253;goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1452
{void*_tmp254=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp255=_tmp254;struct Cyc_Absyn_Aggrdecl*_tmp256;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp255)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp255)->f1).aggr_info).KnownAggr).tag == 2){_LL6: _tmp256=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp255)->f1).aggr_info).KnownAggr).val;_LL7:
# 1454
 if(_tmp256->impl == 0)
return 0;
if(_tmp256->kind == Cyc_Absyn_UnionA)
return 0;
field_types=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp256->impl))->fields;
goto _LL5;}else{goto _LL8;}}else{_LL8: _LL9:
# 1462
 return 0;}_LL5:;}
# 1464
if(field_types == 0)
return 0;
for(0;((struct Cyc_List_List*)_check_null(field_types))->tl != 0;field_types=field_types->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp257=(struct Cyc_Absyn_Aggrfield*)field_types->hd;
for(0;dles != 0;dles=dles->tl){
struct _tuple20*_tmp258=(struct _tuple20*)dles->hd;struct _tuple20*_tmp259=_tmp258;struct Cyc_List_List*_tmp262;struct Cyc_Absyn_Exp*_tmp261;_LLB: _tmp262=_tmp259->f1;_tmp261=_tmp259->f2;_LLC:;{
struct _dyneither_ptr*_tmp25A=Cyc_Absyn_designatorlist_to_fieldname(_tmp262);
if(!Cyc_strptrcmp(_tmp25A,_tmp257->name)){
struct Cyc_Absyn_Exp*_tmp25B=Cyc_Toc_get_varsizeexp(_tmp261);
if(_tmp25B != 0)
return _tmp25B;{
void*_tmp25C=Cyc_Tcutil_compress(_tmp257->type);void*_tmp25D=_tmp25C;void*_tmp260;struct Cyc_Absyn_Exp*_tmp25F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25D)->tag == 8U){_LLE: _tmp260=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25D)->f1).elt_type;_tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp25D)->f1).num_elts;_LLF:
# 1479
 if(_tmp25F == 0  || !Cyc_Toc_is_zero(_tmp25F))
return 0;
# 1485
return({struct Cyc_Absyn_Exp*_tmp843=({struct Cyc_Absyn_Exp*_tmp842=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp842,({struct Cyc_Absyn_Exp*_tmp25E[2U];_tmp25E[0]=(struct Cyc_Absyn_Exp*)_check_null(Cyc_Toc_array_length_exp(_tmp261)),({
# 1487
struct Cyc_Absyn_Exp*_tmp841=Cyc_Absyn_sizeoftyp_exp(_tmp260,0U);_tmp25E[1]=_tmp841;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp25E,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 1485
Cyc_Absyn_add_exp(_tmp843,
# 1488
Cyc_Absyn_signed_int_exp((int)sizeof(double),0U),0U);});}else{_LL10: _LL11:
 return 0;}_LLD:;};}};}
# 1493
({void*_tmp263=0U;({struct _dyneither_ptr _tmp844=({const char*_tmp264="get_varsizeexp: did not find last struct field";_tag_dyneither(_tmp264,sizeof(char),47U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp844,_tag_dyneither(_tmp263,sizeof(void*),0U));});});};}
# 1496
static int Cyc_Toc_get_member_offset(struct Cyc_Absyn_Aggrdecl*ad,struct _dyneither_ptr*f){
int i=1;
{struct Cyc_List_List*_tmp265=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;_tmp265 != 0;_tmp265=_tmp265->tl){
struct Cyc_Absyn_Aggrfield*_tmp266=(struct Cyc_Absyn_Aggrfield*)_tmp265->hd;
if(Cyc_strcmp((struct _dyneither_ptr)*_tmp266->name,(struct _dyneither_ptr)*f)== 0)return i;
++ i;}}
# 1503
({void*_tmp267=0U;({struct _dyneither_ptr _tmp846=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp26A=({struct Cyc_String_pa_PrintArg_struct _tmp782;_tmp782.tag=0U,_tmp782.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp782;});void*_tmp268[1U];_tmp268[0]=& _tmp26A;({struct _dyneither_ptr _tmp845=({const char*_tmp269="get_member_offset %s failed";_tag_dyneither(_tmp269,sizeof(char),28U);});Cyc_aprintf(_tmp845,_tag_dyneither(_tmp268,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp846,_tag_dyneither(_tmp267,sizeof(void*),0U));});});}struct _tuple31{struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Exp*f2;};
# 1507
static struct Cyc_Absyn_Exp*Cyc_Toc_assignop_lvalue(struct Cyc_Absyn_Exp*el,struct _tuple31*pr){
return Cyc_Absyn_assignop_exp(el,(*pr).f1,(*pr).f2,0U);}
# 1510
static struct Cyc_Absyn_Exp*Cyc_Toc_address_lvalue(struct Cyc_Absyn_Exp*e1,int ignore){
return Cyc_Absyn_address_exp(e1,0U);}
# 1513
static struct Cyc_Absyn_Exp*Cyc_Toc_incr_lvalue(struct Cyc_Absyn_Exp*e1,enum Cyc_Absyn_Incrementor incr){
return Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_tmp26B=_cycalloc(sizeof(*_tmp26B));_tmp26B->tag=5U,_tmp26B->f1=e1,_tmp26B->f2=incr;_tmp26B;}),0U);}
# 1517
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env);
# 1526
static void Cyc_Toc_lvalue_assign(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1528
void*_tmp26C=e1->r;void*_tmp26D=_tmp26C;struct Cyc_Absyn_Exp*_tmp275;struct _dyneither_ptr*_tmp274;int _tmp273;int _tmp272;void*_tmp271;struct Cyc_Absyn_Exp*_tmp270;struct Cyc_Absyn_Stmt*_tmp26F;switch(*((int*)_tmp26D)){case 37U: _LL1: _tmp26F=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp26D)->f1;_LL2:
 Cyc_Toc_lvalue_assign_stmt(_tmp26F,fs,f,f_env);goto _LL0;case 14U: _LL3: _tmp271=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26D)->f1;_tmp270=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp26D)->f2;_LL4:
 Cyc_Toc_lvalue_assign(_tmp270,fs,f,f_env);goto _LL0;case 21U: _LL5: _tmp275=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26D)->f1;_tmp274=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26D)->f2;_tmp273=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26D)->f3;_tmp272=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp26D)->f4;_LL6:
# 1533
 e1->r=_tmp275->r;
({struct Cyc_Absyn_Exp*_tmp849=e1;struct Cyc_List_List*_tmp848=({struct Cyc_List_List*_tmp26E=_cycalloc(sizeof(*_tmp26E));_tmp26E->hd=_tmp274,_tmp26E->tl=fs;_tmp26E;});struct Cyc_Absyn_Exp*(*_tmp847)(struct Cyc_Absyn_Exp*,void*)=f;Cyc_Toc_lvalue_assign(_tmp849,_tmp848,_tmp847,f_env);});
goto _LL0;default: _LL7: _LL8: {
# 1541
struct Cyc_Absyn_Exp*e1_copy=Cyc_Absyn_copy_exp(e1);
# 1543
for(0;fs != 0;fs=fs->tl){
({struct Cyc_Absyn_Exp*_tmp84A=Cyc_Toc_member_exp(e1_copy,(struct _dyneither_ptr*)fs->hd,e1_copy->loc);e1_copy=_tmp84A;});}
({void*_tmp84B=(f(e1_copy,f_env))->r;e1->r=_tmp84B;});
goto _LL0;}}_LL0:;}
# 1549
static void Cyc_Toc_lvalue_assign_stmt(struct Cyc_Absyn_Stmt*s,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,void*),void*f_env){
# 1551
void*_tmp276=s->r;void*_tmp277=_tmp276;struct Cyc_Absyn_Stmt*_tmp27E;struct Cyc_Absyn_Decl*_tmp27D;struct Cyc_Absyn_Stmt*_tmp27C;struct Cyc_Absyn_Exp*_tmp27B;switch(*((int*)_tmp277)){case 1U: _LL1: _tmp27B=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp277)->f1;_LL2:
 Cyc_Toc_lvalue_assign(_tmp27B,fs,f,f_env);goto _LL0;case 12U: _LL3: _tmp27D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp277)->f1;_tmp27C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp277)->f2;_LL4:
# 1554
 Cyc_Toc_lvalue_assign_stmt(_tmp27C,fs,f,f_env);goto _LL0;case 2U: _LL5: _tmp27E=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp277)->f2;_LL6:
 Cyc_Toc_lvalue_assign_stmt(_tmp27E,fs,f,f_env);goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp27A=({struct Cyc_String_pa_PrintArg_struct _tmp783;_tmp783.tag=0U,({struct _dyneither_ptr _tmp84C=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp783.f1=_tmp84C;});_tmp783;});void*_tmp278[1U];_tmp278[0]=& _tmp27A;({struct _dyneither_ptr _tmp84D=({const char*_tmp279="lvalue_assign_stmt: %s";_tag_dyneither(_tmp279,sizeof(char),23U);});Cyc_Toc_toc_impos(_tmp84D,_tag_dyneither(_tmp278,sizeof(void*),1U));});});}_LL0:;}
# 1560
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s);
# 1564
static struct Cyc_Absyn_Exp*Cyc_Toc_push_address_exp(struct Cyc_Absyn_Exp*e){
void*_tmp27F=e->r;void*_tmp280=_tmp27F;struct Cyc_Absyn_Stmt*_tmp287;struct Cyc_Absyn_Exp*_tmp286;void**_tmp285;struct Cyc_Absyn_Exp**_tmp284;switch(*((int*)_tmp280)){case 14U: _LL1: _tmp285=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp280)->f1;_tmp284=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp280)->f2;_LL2:
# 1567
({struct Cyc_Absyn_Exp*_tmp84E=Cyc_Toc_push_address_exp(*_tmp284);*_tmp284=_tmp84E;});
({void*_tmp84F=Cyc_Absyn_cstar_typ(*_tmp285,Cyc_Toc_mt_tq);*_tmp285=_tmp84F;});
return e;case 20U: _LL3: _tmp286=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp280)->f1;_LL4:
# 1571
 return _tmp286;case 37U: _LL5: _tmp287=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp280)->f1;_LL6:
# 1575
 Cyc_Toc_push_address_stmt(_tmp287);
return e;default: _LL7: _LL8:
# 1578
 if(Cyc_Absyn_is_lvalue(e))return Cyc_Absyn_address_exp(e,0U);
({struct Cyc_String_pa_PrintArg_struct _tmp283=({struct Cyc_String_pa_PrintArg_struct _tmp784;_tmp784.tag=0U,({struct _dyneither_ptr _tmp850=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp784.f1=_tmp850;});_tmp784;});void*_tmp281[1U];_tmp281[0]=& _tmp283;({struct _dyneither_ptr _tmp851=({const char*_tmp282="can't take & of exp %s";_tag_dyneither(_tmp282,sizeof(char),23U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp851,_tag_dyneither(_tmp281,sizeof(void*),1U));});});}_LL0:;}
# 1583
static void Cyc_Toc_push_address_stmt(struct Cyc_Absyn_Stmt*s){
void*_tmp288=s->r;void*_tmp289=_tmp288;struct Cyc_Absyn_Exp**_tmp28F;struct Cyc_Absyn_Stmt*_tmp28E;struct Cyc_Absyn_Stmt*_tmp28D;switch(*((int*)_tmp289)){case 2U: _LL1: _tmp28D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp289)->f2;_LL2:
 _tmp28E=_tmp28D;goto _LL4;case 12U: _LL3: _tmp28E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp289)->f2;_LL4:
 Cyc_Toc_push_address_stmt(_tmp28E);goto _LL0;case 1U: _LL5: _tmp28F=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp289)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp852=Cyc_Toc_push_address_exp(*_tmp28F);*_tmp28F=_tmp852;});goto _LL0;default: _LL7: _LL8:
({struct Cyc_String_pa_PrintArg_struct _tmp28C=({struct Cyc_String_pa_PrintArg_struct _tmp785;_tmp785.tag=0U,({struct _dyneither_ptr _tmp853=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp785.f1=_tmp853;});_tmp785;});void*_tmp28A[1U];_tmp28A[0]=& _tmp28C;({struct _dyneither_ptr _tmp854=({const char*_tmp28B="can't take & of stmt %s";_tag_dyneither(_tmp28B,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp854,_tag_dyneither(_tmp28A,sizeof(void*),1U));});});}_LL0:;}
# 1592
static struct Cyc_Absyn_PtrInfo Cyc_Toc_get_ptr_type(void*t){
void*_tmp290=Cyc_Tcutil_compress(t);void*_tmp291=_tmp290;struct Cyc_Absyn_PtrInfo _tmp294;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291)->tag == 5U){_LL1: _tmp294=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp291)->f1;_LL2:
 return _tmp294;}else{_LL3: _LL4:
({void*_tmp292=0U;({struct _dyneither_ptr _tmp855=({const char*_tmp293="get_ptr_typ: not a pointer!";_tag_dyneither(_tmp293,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp855,_tag_dyneither(_tmp292,sizeof(void*),0U));});});}_LL0:;}
# 1602
static struct Cyc_Absyn_Exp*Cyc_Toc_generate_zero(void*t){
struct Cyc_Absyn_Exp*res;
{void*_tmp295=Cyc_Tcutil_compress(t);void*_tmp296=_tmp295;int _tmp2A2;enum Cyc_Absyn_Sign _tmp2A1;enum Cyc_Absyn_Sign _tmp2A0;enum Cyc_Absyn_Sign _tmp29F;enum Cyc_Absyn_Sign _tmp29E;enum Cyc_Absyn_Sign _tmp29D;switch(*((int*)_tmp296)){case 5U: _LL1: _LL2:
({struct Cyc_Absyn_Exp*_tmp856=Cyc_Absyn_null_exp(0U);res=_tmp856;});goto _LL0;case 6U: switch(((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp296)->f2){case Cyc_Absyn_Char_sz: _LL3: _tmp29D=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp296)->f1;_LL4:
({struct Cyc_Absyn_Exp*_tmp857=Cyc_Absyn_const_exp(Cyc_Absyn_Char_c(_tmp29D,'\000'),0U);res=_tmp857;});goto _LL0;case Cyc_Absyn_Short_sz: _LL5: _tmp29E=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp296)->f1;_LL6:
({struct Cyc_Absyn_Exp*_tmp858=Cyc_Absyn_const_exp(Cyc_Absyn_Short_c(_tmp29E,0),0U);res=_tmp858;});goto _LL0;case Cyc_Absyn_Int_sz: _LLB: _tmp29F=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp296)->f1;_LLC:
# 1610
 _tmp2A0=_tmp29F;goto _LLE;case Cyc_Absyn_Long_sz: _LLD: _tmp2A0=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp296)->f1;_LLE:
# 1612
({struct Cyc_Absyn_Exp*_tmp859=Cyc_Absyn_const_exp(Cyc_Absyn_Int_c(_tmp2A0,0),0U);res=_tmp859;});goto _LL0;default: _LLF: _tmp2A1=((struct Cyc_Absyn_IntType_Absyn_Type_struct*)_tmp296)->f1;_LL10:
# 1614
({struct Cyc_Absyn_Exp*_tmp85A=Cyc_Absyn_const_exp(Cyc_Absyn_LongLong_c(_tmp2A1,(long long)0),0U);res=_tmp85A;});goto _LL0;}case 13U: _LL7: _LL8:
# 1608
 goto _LLA;case 14U: _LL9: _LLA:
 _tmp29F=Cyc_Absyn_Unsigned;goto _LLC;case 7U: switch(((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp296)->f1){case 0U: _LL11: _LL12:
# 1615
({struct Cyc_Absyn_Exp*_tmp85B=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp297="0.0F";_tag_dyneither(_tmp297,sizeof(char),5U);}),0),0U);res=_tmp85B;});goto _LL0;case 1U: _LL13: _LL14:
({struct Cyc_Absyn_Exp*_tmp85C=Cyc_Absyn_const_exp(Cyc_Absyn_Float_c(({const char*_tmp298="0.0";_tag_dyneither(_tmp298,sizeof(char),4U);}),1),0U);res=_tmp85C;});goto _LL0;default: _LL15: _tmp2A2=((struct Cyc_Absyn_FloatType_Absyn_Type_struct*)_tmp296)->f1;_LL16:
({struct Cyc_Absyn_Exp*_tmp85E=Cyc_Absyn_const_exp(({struct _dyneither_ptr _tmp85D=({const char*_tmp299="0.0L";_tag_dyneither(_tmp299,sizeof(char),5U);});Cyc_Absyn_Float_c(_tmp85D,_tmp2A2);}),0U);res=_tmp85E;});goto _LL0;}default: _LL17: _LL18:
# 1619
({struct Cyc_String_pa_PrintArg_struct _tmp29C=({struct Cyc_String_pa_PrintArg_struct _tmp786;_tmp786.tag=0U,({struct _dyneither_ptr _tmp85F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(t));_tmp786.f1=_tmp85F;});_tmp786;});void*_tmp29A[1U];_tmp29A[0]=& _tmp29C;({struct _dyneither_ptr _tmp860=({const char*_tmp29B="found non-zero type %s in generate_zero";_tag_dyneither(_tmp29B,sizeof(char),40U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp860,_tag_dyneither(_tmp29A,sizeof(void*),1U));});});}_LL0:;}
# 1621
res->topt=t;
return res;}
# 1628
static void Cyc_Toc_zero_ptr_assign_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Exp*e1,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*ptr_type,int is_dyneither,void*elt_type){
# 1641 "toc.cyc"
void*fat_ptr_type=Cyc_Absyn_dyneither_typ(elt_type,(void*)& Cyc_Absyn_HeapRgn_val,Cyc_Toc_mt_tq,Cyc_Absyn_true_conref);
void*_tmp2A3=Cyc_Toc_typ_to_c(elt_type);
void*_tmp2A4=Cyc_Toc_typ_to_c(fat_ptr_type);
void*_tmp2A5=Cyc_Absyn_cstar_typ(_tmp2A3,Cyc_Toc_mt_tq);
struct Cyc_Core_Opt*_tmp2A6=({struct Cyc_Core_Opt*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->v=_tmp2A5;_tmp2D7;});
struct Cyc_Absyn_Exp*xinit;
{void*_tmp2A7=e1->r;void*_tmp2A8=_tmp2A7;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Exp*_tmp2AD;struct Cyc_Absyn_Exp*_tmp2AC;switch(*((int*)_tmp2A8)){case 20U: _LL1: _tmp2AC=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp2A8)->f1;_LL2:
# 1649
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp861=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AC,0,Cyc_Absyn_Other_coercion,0U);_tmp2AC=_tmp861;});
_tmp2AC->topt=fat_ptr_type;}
# 1653
Cyc_Toc_exp_to_c(nv,_tmp2AC);xinit=_tmp2AC;goto _LL0;case 23U: _LL3: _tmp2AE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A8)->f1;_tmp2AD=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp2A8)->f2;_LL4:
# 1655
 if(!is_dyneither){
({struct Cyc_Absyn_Exp*_tmp862=Cyc_Absyn_cast_exp(fat_ptr_type,_tmp2AE,0,Cyc_Absyn_Other_coercion,0U);_tmp2AE=_tmp862;});
_tmp2AE->topt=fat_ptr_type;}
# 1659
Cyc_Toc_exp_to_c(nv,_tmp2AE);Cyc_Toc_exp_to_c(nv,_tmp2AD);
({struct Cyc_Absyn_Exp*_tmp865=({struct Cyc_Absyn_Exp*_tmp864=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Absyn_fncall_exp(_tmp864,({struct Cyc_Absyn_Exp*_tmp2A9[3U];_tmp2A9[0]=_tmp2AE,({
struct Cyc_Absyn_Exp*_tmp863=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2A9[1]=_tmp863;}),_tmp2A9[2]=_tmp2AD;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2A9,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 1660
xinit=_tmp865;});
# 1662
goto _LL0;default: _LL5: _LL6:
({void*_tmp2AA=0U;({struct _dyneither_ptr _tmp866=({const char*_tmp2AB="found bad lhs for zero-terminated pointer assignment";_tag_dyneither(_tmp2AB,sizeof(char),53U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp866,_tag_dyneither(_tmp2AA,sizeof(void*),0U));});});}_LL0:;}{
# 1665
struct _tuple1*_tmp2AF=Cyc_Toc_temp_var();
struct _RegionHandle _tmp2B0=_new_region("rgn2");struct _RegionHandle*rgn2=& _tmp2B0;_push_region(rgn2);
{struct Cyc_Absyn_Vardecl*_tmp2B1=({struct Cyc_Absyn_Vardecl*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->sc=Cyc_Absyn_Public,_tmp2D6->name=_tmp2AF,_tmp2D6->varloc=0U,_tmp2D6->tq=Cyc_Toc_mt_tq,_tmp2D6->type=_tmp2A4,_tmp2D6->initializer=xinit,_tmp2D6->rgn=0,_tmp2D6->attributes=0,_tmp2D6->escapes=0;_tmp2D6;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B2=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D5=_cycalloc(sizeof(*_tmp2D5));_tmp2D5->tag=4U,_tmp2D5->f1=_tmp2B1;_tmp2D5;});
struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_varb_exp((void*)_tmp2B2,0U);
_tmp2B3->topt=fat_ptr_type;{
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_deref_exp(_tmp2B3,0U);
_tmp2B4->topt=elt_type;
Cyc_Toc_exp_to_c(nv,_tmp2B4);{
struct _tuple1*_tmp2B5=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2B6=({struct Cyc_Absyn_Vardecl*_tmp2D4=_cycalloc(sizeof(*_tmp2D4));_tmp2D4->sc=Cyc_Absyn_Public,_tmp2D4->name=_tmp2B5,_tmp2D4->varloc=0U,_tmp2D4->tq=Cyc_Toc_mt_tq,_tmp2D4->type=_tmp2A3,_tmp2D4->initializer=_tmp2B4,_tmp2D4->rgn=0,_tmp2D4->attributes=0,_tmp2D4->escapes=0;_tmp2D4;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2B7=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D3=_cycalloc(sizeof(*_tmp2D3));_tmp2D3->tag=4U,_tmp2D3->f1=_tmp2B6;_tmp2D3;});
struct Cyc_Absyn_Exp*z_init=e2;
if(popt != 0){
struct Cyc_Absyn_Exp*_tmp2B8=Cyc_Absyn_varb_exp((void*)_tmp2B7,0U);
_tmp2B8->topt=_tmp2B4->topt;
({struct Cyc_Absyn_Exp*_tmp867=Cyc_Absyn_prim2_exp((enum Cyc_Absyn_Primop)popt->v,_tmp2B8,e2,0U);z_init=_tmp867;});
z_init->topt=_tmp2B8->topt;}
# 1684
Cyc_Toc_exp_to_c(nv,z_init);{
struct _tuple1*_tmp2B9=Cyc_Toc_temp_var();
struct Cyc_Absyn_Vardecl*_tmp2BA=({struct Cyc_Absyn_Vardecl*_tmp2D2=_cycalloc(sizeof(*_tmp2D2));_tmp2D2->sc=Cyc_Absyn_Public,_tmp2D2->name=_tmp2B9,_tmp2D2->varloc=0U,_tmp2D2->tq=Cyc_Toc_mt_tq,_tmp2D2->type=_tmp2A3,_tmp2D2->initializer=z_init,_tmp2D2->rgn=0,_tmp2D2->attributes=0,_tmp2D2->escapes=0;_tmp2D2;});
struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2BB=({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->tag=4U,_tmp2D1->f1=_tmp2BA;_tmp2D1;});
# 1690
struct Cyc_Absyn_Exp*_tmp2BC=Cyc_Absyn_varb_exp((void*)_tmp2B7,0U);_tmp2BC->topt=_tmp2B4->topt;{
struct Cyc_Absyn_Exp*_tmp2BD=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2BE=Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp2BC,_tmp2BD,0U);
_tmp2BE->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2BE);{
# 1696
struct Cyc_Absyn_Exp*_tmp2BF=Cyc_Absyn_varb_exp((void*)_tmp2BB,0U);_tmp2BF->topt=_tmp2B4->topt;{
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Toc_generate_zero(elt_type);
struct Cyc_Absyn_Exp*_tmp2C1=Cyc_Absyn_prim2_exp(Cyc_Absyn_Neq,_tmp2BF,_tmp2C0,0U);
_tmp2C1->topt=Cyc_Absyn_sint_typ;
Cyc_Toc_exp_to_c(nv,_tmp2C1);{
# 1702
struct Cyc_List_List*_tmp2C2=({struct Cyc_Absyn_Exp*_tmp2D0[2U];({struct Cyc_Absyn_Exp*_tmp869=Cyc_Absyn_varb_exp((void*)_tmp2B2,0U);_tmp2D0[0]=_tmp869;}),({
struct Cyc_Absyn_Exp*_tmp868=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_type),0U);_tmp2D0[1]=_tmp868;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp2D0,sizeof(struct Cyc_Absyn_Exp*),2U));});
struct Cyc_Absyn_Exp*_tmp2C3=Cyc_Absyn_uint_exp(1U,0U);
struct Cyc_Absyn_Exp*xsize;
({struct Cyc_Absyn_Exp*_tmp86B=({struct Cyc_Absyn_Exp*_tmp86A=Cyc_Absyn_fncall_exp(Cyc_Toc__get_dyneither_size_e,_tmp2C2,0U);Cyc_Absyn_prim2_exp(Cyc_Absyn_Eq,_tmp86A,_tmp2C3,0U);});xsize=_tmp86B;});{
# 1709
struct Cyc_Absyn_Exp*_tmp2C4=({struct Cyc_Absyn_Exp*_tmp86C=xsize;Cyc_Absyn_and_exp(_tmp86C,Cyc_Absyn_and_exp(_tmp2BE,_tmp2C1,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C5=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U);
struct Cyc_Absyn_Exp*_tmp2C6=({struct Cyc_Absyn_Exp*_tmp86D=Cyc_Absyn_varb_exp((void*)_tmp2B2,0U);Cyc_Toc_member_exp(_tmp86D,Cyc_Toc_curr_sp,0U);});
({struct Cyc_Absyn_Exp*_tmp86E=Cyc_Toc_cast_it(_tmp2A5,_tmp2C6);_tmp2C6=_tmp86E;});{
struct Cyc_Absyn_Exp*_tmp2C7=Cyc_Absyn_deref_exp(_tmp2C6,0U);
struct Cyc_Absyn_Exp*_tmp2C8=({struct Cyc_Absyn_Exp*_tmp86F=_tmp2C7;Cyc_Absyn_assign_exp(_tmp86F,Cyc_Absyn_var_exp(_tmp2B9,0U),0U);});
struct Cyc_Absyn_Stmt*_tmp2C9=Cyc_Absyn_exp_stmt(_tmp2C8,0U);
({struct Cyc_Absyn_Stmt*_tmp873=({struct Cyc_Absyn_Stmt*_tmp872=({struct Cyc_Absyn_Exp*_tmp871=_tmp2C4;struct Cyc_Absyn_Stmt*_tmp870=_tmp2C5;Cyc_Absyn_ifthenelse_stmt(_tmp871,_tmp870,Cyc_Absyn_skip_stmt(0U),0U);});Cyc_Absyn_seq_stmt(_tmp872,_tmp2C9,0U);});_tmp2C9=_tmp873;});
({struct Cyc_Absyn_Stmt*_tmp876=({struct Cyc_Absyn_Decl*_tmp875=({struct Cyc_Absyn_Decl*_tmp2CB=_cycalloc(sizeof(*_tmp2CB));({void*_tmp874=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CA=_cycalloc(sizeof(*_tmp2CA));_tmp2CA->tag=0U,_tmp2CA->f1=_tmp2BA;_tmp2CA;});_tmp2CB->r=_tmp874;}),_tmp2CB->loc=0U;_tmp2CB;});Cyc_Absyn_decl_stmt(_tmp875,_tmp2C9,0U);});_tmp2C9=_tmp876;});
({struct Cyc_Absyn_Stmt*_tmp879=({struct Cyc_Absyn_Decl*_tmp878=({struct Cyc_Absyn_Decl*_tmp2CD=_cycalloc(sizeof(*_tmp2CD));({void*_tmp877=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CC=_cycalloc(sizeof(*_tmp2CC));_tmp2CC->tag=0U,_tmp2CC->f1=_tmp2B6;_tmp2CC;});_tmp2CD->r=_tmp877;}),_tmp2CD->loc=0U;_tmp2CD;});Cyc_Absyn_decl_stmt(_tmp878,_tmp2C9,0U);});_tmp2C9=_tmp879;});
({struct Cyc_Absyn_Stmt*_tmp87C=({struct Cyc_Absyn_Decl*_tmp87B=({struct Cyc_Absyn_Decl*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));({void*_tmp87A=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp2CE=_cycalloc(sizeof(*_tmp2CE));_tmp2CE->tag=0U,_tmp2CE->f1=_tmp2B1;_tmp2CE;});_tmp2CF->r=_tmp87A;}),_tmp2CF->loc=0U;_tmp2CF;});Cyc_Absyn_decl_stmt(_tmp87B,_tmp2C9,0U);});_tmp2C9=_tmp87C;});
({void*_tmp87D=Cyc_Toc_stmt_exp_r(_tmp2C9);e->r=_tmp87D;});};};};};};};};};};}
# 1667
;_pop_region(rgn2);};}
# 1735 "toc.cyc"
static void*Cyc_Toc_check_tagged_union(struct Cyc_Absyn_Exp*e1,void*e1_c_type,void*aggrtype,struct _dyneither_ptr*f,int in_lhs,struct Cyc_Absyn_Exp*(*aggrproj)(struct Cyc_Absyn_Exp*,struct _dyneither_ptr*,unsigned int)){
# 1739
struct Cyc_Absyn_Aggrdecl*ad;
{void*_tmp2D8=Cyc_Tcutil_compress(aggrtype);void*_tmp2D9=_tmp2D8;union Cyc_Absyn_AggrInfoU _tmp2DE;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D9)->tag == 11U){_LL1: _tmp2DE=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2D9)->f1).aggr_info;_LL2:
# 1742
({struct Cyc_Absyn_Aggrdecl*_tmp87E=Cyc_Absyn_get_known_aggrdecl(_tmp2DE);ad=_tmp87E;});goto _LL0;}else{_LL3: _LL4:
({void*_tmp2DA=0U;({struct _dyneither_ptr _tmp881=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2DD=({struct Cyc_String_pa_PrintArg_struct _tmp787;_tmp787.tag=0U,({
struct _dyneither_ptr _tmp87F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string(aggrtype));_tmp787.f1=_tmp87F;});_tmp787;});void*_tmp2DB[1U];_tmp2DB[0]=& _tmp2DD;({struct _dyneither_ptr _tmp880=({const char*_tmp2DC="expecting union but found %s in check_tagged_union";_tag_dyneither(_tmp2DC,sizeof(char),51U);});Cyc_aprintf(_tmp880,_tag_dyneither(_tmp2DB,sizeof(void*),1U));});});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp881,_tag_dyneither(_tmp2DA,sizeof(void*),0U));});});}_LL0:;}{
# 1746
struct _tuple1*_tmp2DF=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp2E0=Cyc_Absyn_var_exp(_tmp2DF,0U);
struct Cyc_Absyn_Exp*_tmp2E1=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(ad,f),0U);
if(in_lhs){
struct Cyc_Absyn_Exp*_tmp2E2=Cyc_Absyn_aggrarrow_exp(_tmp2E0,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*_tmp2E3=Cyc_Absyn_neq_exp(_tmp2E2,_tmp2E1,0U);
struct Cyc_Absyn_Exp*_tmp2E4=Cyc_Absyn_aggrarrow_exp(_tmp2E0,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Stmt*_tmp2E5=Cyc_Absyn_exp_stmt(Cyc_Absyn_address_exp(_tmp2E4,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E6=({struct Cyc_Absyn_Exp*_tmp883=_tmp2E3;struct Cyc_Absyn_Stmt*_tmp882=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp883,_tmp882,Cyc_Toc_skip_stmt_dl(),0U);});
void*_tmp2E7=Cyc_Absyn_cstar_typ(e1_c_type,Cyc_Toc_mt_tq);
struct Cyc_Absyn_Exp*_tmp2E8=Cyc_Absyn_address_exp(aggrproj(e1,f,0U),0U);
struct Cyc_Absyn_Stmt*_tmp2E9=({struct _tuple1*_tmp886=_tmp2DF;void*_tmp885=_tmp2E7;struct Cyc_Absyn_Exp*_tmp884=_tmp2E8;Cyc_Absyn_declare_stmt(_tmp886,_tmp885,_tmp884,Cyc_Absyn_seq_stmt(_tmp2E6,_tmp2E5,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2E9);}else{
# 1760
struct Cyc_Absyn_Exp*_tmp2EA=({struct Cyc_Absyn_Exp*_tmp887=aggrproj(_tmp2E0,f,0U);Cyc_Toc_member_exp(_tmp887,Cyc_Toc_tag_sp,0U);});
struct Cyc_Absyn_Exp*_tmp2EB=Cyc_Absyn_neq_exp(_tmp2EA,_tmp2E1,0U);
struct Cyc_Absyn_Exp*_tmp2EC=({struct Cyc_Absyn_Exp*_tmp888=aggrproj(_tmp2E0,f,0U);Cyc_Toc_member_exp(_tmp888,Cyc_Toc_val_sp,0U);});
struct Cyc_Absyn_Stmt*_tmp2ED=Cyc_Absyn_exp_stmt(_tmp2EC,0U);
struct Cyc_Absyn_Stmt*_tmp2EE=({struct Cyc_Absyn_Exp*_tmp88A=_tmp2EB;struct Cyc_Absyn_Stmt*_tmp889=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp88A,_tmp889,Cyc_Toc_skip_stmt_dl(),0U);});
struct Cyc_Absyn_Stmt*_tmp2EF=({struct _tuple1*_tmp88D=_tmp2DF;void*_tmp88C=e1_c_type;struct Cyc_Absyn_Exp*_tmp88B=e1;Cyc_Absyn_declare_stmt(_tmp88D,_tmp88C,_tmp88B,Cyc_Absyn_seq_stmt(_tmp2EE,_tmp2ED,0U),0U);});
return Cyc_Toc_stmt_exp_r(_tmp2EF);}};}
# 1770
static int Cyc_Toc_is_tagged_union_project_impl(void*t,struct _dyneither_ptr*f,int*f_tag,void**tagged_member_type,int clear_read,int*is_read){
# 1773
void*_tmp2F0=Cyc_Tcutil_compress(t);void*_tmp2F1=_tmp2F0;union Cyc_Absyn_AggrInfoU _tmp2F8;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F1)->tag == 11U){_LL1: _tmp2F8=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp2F1)->f1).aggr_info;_LL2: {
# 1775
struct Cyc_Absyn_Aggrdecl*_tmp2F2=Cyc_Absyn_get_known_aggrdecl(_tmp2F8);
({int _tmp88E=Cyc_Toc_get_member_offset(_tmp2F2,f);*f_tag=_tmp88E;});{
# 1778
struct _dyneither_ptr str=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2F6=({struct Cyc_String_pa_PrintArg_struct _tmp789;_tmp789.tag=0U,_tmp789.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp2F2->name).f2);_tmp789;});struct Cyc_String_pa_PrintArg_struct _tmp2F7=({struct Cyc_String_pa_PrintArg_struct _tmp788;_tmp788.tag=0U,_tmp788.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*f);_tmp788;});void*_tmp2F4[2U];_tmp2F4[0]=& _tmp2F6,_tmp2F4[1]=& _tmp2F7;({struct _dyneither_ptr _tmp88F=({const char*_tmp2F5="_union_%s_%s";_tag_dyneither(_tmp2F5,sizeof(char),13U);});Cyc_aprintf(_tmp88F,_tag_dyneither(_tmp2F4,sizeof(void*),2U));});});
({void*_tmp890=Cyc_Absyn_strct(({struct _dyneither_ptr*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));*_tmp2F3=str;_tmp2F3;}));*tagged_member_type=_tmp890;});
if(clear_read)*((int*)_check_null(is_read))=0;
return((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp2F2->impl))->tagged;};}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1789
static int Cyc_Toc_is_tagged_union_project(struct Cyc_Absyn_Exp*e,int*f_tag,void**tagged_member_type,int clear_read){
# 1792
void*_tmp2F9=e->r;void*_tmp2FA=_tmp2F9;struct Cyc_Absyn_Exp*_tmp306;struct _dyneither_ptr*_tmp305;int*_tmp304;struct Cyc_Absyn_Exp*_tmp303;struct _dyneither_ptr*_tmp302;int*_tmp301;struct Cyc_Absyn_Exp*_tmp300;switch(*((int*)_tmp2FA)){case 14U: _LL1: _tmp300=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FA)->f2;_LL2:
({void*_tmp2FB=0U;({struct _dyneither_ptr _tmp891=({const char*_tmp2FC="cast on lhs!";_tag_dyneither(_tmp2FC,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp891,_tag_dyneither(_tmp2FB,sizeof(void*),0U));});});case 21U: _LL3: _tmp303=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FA)->f1;_tmp302=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FA)->f2;_tmp301=(int*)&((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp2FA)->f4;_LL4:
# 1795
 return Cyc_Toc_is_tagged_union_project_impl((void*)_check_null(_tmp303->topt),_tmp302,f_tag,tagged_member_type,clear_read,_tmp301);case 22U: _LL5: _tmp306=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FA)->f1;_tmp305=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FA)->f2;_tmp304=(int*)&((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp2FA)->f4;_LL6: {
# 1798
void*_tmp2FD=Cyc_Tcutil_compress((void*)_check_null(_tmp306->topt));void*_tmp2FE=_tmp2FD;void*_tmp2FF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->tag == 5U){_LLA: _tmp2FF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).elt_typ;_LLB:
# 1800
 return Cyc_Toc_is_tagged_union_project_impl(_tmp2FF,_tmp305,f_tag,tagged_member_type,clear_read,_tmp304);}else{_LLC: _LLD:
# 1802
 return 0;}_LL9:;}default: _LL7: _LL8:
# 1804
 return 0;}_LL0:;}
# 1817 "toc.cyc"
static void*Cyc_Toc_tagged_union_assignop(struct Cyc_Absyn_Exp*e1,void*e1_cyc_type,struct Cyc_Core_Opt*popt,struct Cyc_Absyn_Exp*e2,void*e2_cyc_type,int tag_num,void*member_type){
# 1821
struct _tuple1*_tmp307=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*temp_exp=Cyc_Absyn_var_exp(_tmp307,0U);
struct Cyc_Absyn_Exp*temp_val=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_val_sp,0U);
struct Cyc_Absyn_Exp*temp_tag=Cyc_Absyn_aggrarrow_exp(temp_exp,Cyc_Toc_tag_sp,0U);
struct Cyc_Absyn_Exp*f_tag=Cyc_Absyn_signed_int_exp(tag_num,0U);
struct Cyc_Absyn_Stmt*s3=Cyc_Absyn_exp_stmt(Cyc_Absyn_assignop_exp(temp_val,popt,e2,0U),0U);
struct Cyc_Absyn_Stmt*s2;
if(popt == 0)
({struct Cyc_Absyn_Stmt*_tmp892=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(temp_tag,f_tag,0U),0U);s2=_tmp892;});else{
# 1831
struct Cyc_Absyn_Exp*_tmp308=Cyc_Absyn_neq_exp(temp_tag,f_tag,0U);
({struct Cyc_Absyn_Stmt*_tmp895=({struct Cyc_Absyn_Exp*_tmp894=_tmp308;struct Cyc_Absyn_Stmt*_tmp893=Cyc_Toc_throw_match_stmt();Cyc_Absyn_ifthenelse_stmt(_tmp894,_tmp893,Cyc_Toc_skip_stmt_dl(),0U);});s2=_tmp895;});}{
# 1834
struct Cyc_Absyn_Stmt*s1=({struct _tuple1*_tmp898=_tmp307;void*_tmp897=Cyc_Absyn_cstar_typ(member_type,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp896=
Cyc_Toc_push_address_exp(e1);
# 1834
Cyc_Absyn_declare_stmt(_tmp898,_tmp897,_tmp896,
# 1836
Cyc_Absyn_seq_stmt(s2,s3,0U),0U);});
return Cyc_Toc_stmt_exp_r(s1);};}struct _tuple32{struct _dyneither_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _tuple33{void*f1;void*f2;};struct _tuple34{struct Cyc_Absyn_Aggrfield*f1;struct Cyc_Absyn_Exp*f2;};
# 1840
static void Cyc_Toc_exp_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Exp*e){
void*_tmp309=e->r;
if(e->topt == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp30C=({struct Cyc_String_pa_PrintArg_struct _tmp78A;_tmp78A.tag=0U,({struct _dyneither_ptr _tmp899=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp78A.f1=_tmp899;});_tmp78A;});void*_tmp30A[1U];_tmp30A[0]=& _tmp30C;({struct _dyneither_ptr _tmp89A=({const char*_tmp30B="exp_to_c: no type for %s";_tag_dyneither(_tmp30B,sizeof(char),25U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp89A,_tag_dyneither(_tmp30A,sizeof(void*),1U));});});{
void*old_typ=(void*)_check_null(e->topt);
void*_tmp30D=_tmp309;struct Cyc_Absyn_Stmt*_tmp521;struct Cyc_Absyn_Exp*_tmp520;struct _dyneither_ptr*_tmp51F;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51D;int _tmp51C;struct Cyc_Absyn_Exp*_tmp51B;void**_tmp51A;struct Cyc_Absyn_Exp*_tmp519;int _tmp518;int _tmp517;struct Cyc_List_List*_tmp516;struct Cyc_Absyn_Datatypedecl*_tmp515;struct Cyc_Absyn_Datatypefield*_tmp514;struct _tuple1**_tmp513;struct Cyc_List_List*_tmp512;struct Cyc_List_List*_tmp511;struct Cyc_Absyn_Aggrdecl*_tmp510;void*_tmp50F;struct Cyc_List_List*_tmp50E;struct Cyc_Absyn_Exp*_tmp50D;void*_tmp50C;int _tmp50B;struct Cyc_Absyn_Vardecl*_tmp50A;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp*_tmp508;int _tmp507;struct Cyc_List_List*_tmp506;struct Cyc_List_List*_tmp505;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*_tmp502;struct _dyneither_ptr*_tmp501;int _tmp500;int _tmp4FF;struct Cyc_Absyn_Exp*_tmp4FE;struct _dyneither_ptr*_tmp4FD;int _tmp4FC;int _tmp4FB;struct Cyc_Absyn_Exp*_tmp4FA;void*_tmp4F9;struct Cyc_List_List*_tmp4F8;void*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F3;void**_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F1;int _tmp4F0;enum Cyc_Absyn_Coercion _tmp4EF;struct Cyc_Absyn_Exp*_tmp4EE;struct Cyc_List_List*_tmp4ED;struct Cyc_Absyn_Exp*_tmp4EC;struct Cyc_Absyn_Exp*_tmp4EB;int _tmp4EA;struct Cyc_Absyn_Exp*_tmp4E9;struct Cyc_List_List*_tmp4E8;int _tmp4E7;struct Cyc_List_List*_tmp4E6;struct Cyc_Absyn_VarargInfo*_tmp4E5;struct Cyc_Absyn_Exp*_tmp4E4;struct Cyc_List_List*_tmp4E3;struct Cyc_Absyn_Exp*_tmp4E2;struct Cyc_Absyn_Exp*_tmp4E1;struct Cyc_Absyn_Exp*_tmp4E0;struct Cyc_Absyn_Exp*_tmp4DF;struct Cyc_Absyn_Exp*_tmp4DE;struct Cyc_Absyn_Exp*_tmp4DD;struct Cyc_Absyn_Exp*_tmp4DC;struct Cyc_Absyn_Exp*_tmp4DB;struct Cyc_Absyn_Exp*_tmp4DA;struct Cyc_Absyn_Exp*_tmp4D9;struct Cyc_Core_Opt*_tmp4D8;struct Cyc_Absyn_Exp*_tmp4D7;struct Cyc_Absyn_Exp*_tmp4D6;enum Cyc_Absyn_Incrementor _tmp4D5;enum Cyc_Absyn_Primop _tmp4D4;struct Cyc_List_List*_tmp4D3;switch(*((int*)_tmp30D)){case 2U: _LL1: _LL2:
# 1847
 e->r=(void*)& Cyc_Toc_zero_exp;
goto _LL0;case 0U: if(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp30D)->f1).Null_c).tag == 1){_LL3: _LL4: {
# 1854
struct Cyc_Absyn_Exp*_tmp30E=Cyc_Absyn_signed_int_exp(0,0U);
if(Cyc_Tcutil_is_tagged_pointer_typ(old_typ)){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp89B=(Cyc_Toc_make_toplevel_dyn_arr(old_typ,_tmp30E,_tmp30E))->r;e->r=_tmp89B;});else{
# 1859
({void*_tmp89D=({struct Cyc_Absyn_Exp*_tmp89C=Cyc_Toc__tag_dyneither_e;Cyc_Toc_fncall_exp_r(_tmp89C,({struct Cyc_Absyn_Exp*_tmp30F[3U];_tmp30F[0]=_tmp30E,_tmp30F[1]=_tmp30E,_tmp30F[2]=_tmp30E;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp30F,sizeof(struct Cyc_Absyn_Exp*),3U));}));});e->r=_tmp89D;});}}else{
# 1861
e->r=(void*)& Cyc_Toc_zero_exp;}
# 1863
goto _LL0;}}else{_LL5: _LL6:
 goto _LL0;}case 1U: _LL7: _LL8:
 goto _LL0;case 3U: _LL9: _tmp4D4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4D3=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LLA: {
# 1868
struct Cyc_List_List*_tmp310=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_get_cyc_typ,_tmp4D3);
# 1870
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4D3);
{enum Cyc_Absyn_Primop _tmp311=_tmp4D4;switch(_tmp311){case Cyc_Absyn_Numelts: _LL58: _LL59: {
# 1873
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3))->hd;
{void*_tmp312=Cyc_Tcutil_compress((void*)_check_null(arg->topt));void*_tmp313=_tmp312;void*_tmp325;union Cyc_Absyn_Constraint*_tmp324;union Cyc_Absyn_Constraint*_tmp323;union Cyc_Absyn_Constraint*_tmp322;struct Cyc_Absyn_Exp*_tmp321;switch(*((int*)_tmp313)){case 8U: _LL6D: _tmp321=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp313)->f1).num_elts;_LL6E:
# 1877
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp321)))
({void*_tmp314=0U;({unsigned int _tmp89F=e->loc;struct _dyneither_ptr _tmp89E=({const char*_tmp315="can't calculate numelts";_tag_dyneither(_tmp315,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp89F,_tmp89E,_tag_dyneither(_tmp314,sizeof(void*),0U));});});
e->r=_tmp321->r;goto _LL6C;case 5U: _LL6F: _tmp325=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313)->f1).elt_typ;_tmp324=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313)->f1).ptr_atts).nullable;_tmp323=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313)->f1).ptr_atts).bounds;_tmp322=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp313)->f1).ptr_atts).zero_term;_LL70:
# 1881
{void*_tmp316=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp323);void*_tmp317=_tmp316;struct Cyc_Absyn_Exp*_tmp31C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp317)->tag == 0U){_LL74: _LL75:
# 1883
({void*_tmp8A2=({struct Cyc_Absyn_Exp*_tmp8A1=Cyc_Toc__get_dyneither_size_e;Cyc_Toc_fncall_exp_r(_tmp8A1,({struct Cyc_Absyn_Exp*_tmp318[2U];_tmp318[0]=(struct Cyc_Absyn_Exp*)_tmp4D3->hd,({
# 1885
struct Cyc_Absyn_Exp*_tmp8A0=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp325),0U);_tmp318[1]=_tmp8A0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp318,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 1883
e->r=_tmp8A2;});
# 1886
goto _LL73;}else{_LL76: _tmp31C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp317)->f1;_LL77:
# 1889
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp322)){
struct Cyc_Absyn_Exp*function_e=
Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,(struct Cyc_Absyn_Exp*)_tmp4D3->hd);
# 1893
({void*_tmp8A4=({struct Cyc_Absyn_Exp*_tmp8A3=function_e;Cyc_Toc_fncall_exp_r(_tmp8A3,({struct Cyc_Absyn_Exp*_tmp319[2U];_tmp319[0]=(struct Cyc_Absyn_Exp*)_tmp4D3->hd,_tmp319[1]=_tmp31C;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp319,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8A4;});}else{
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp324)){
if(!Cyc_Evexp_c_can_eval(_tmp31C))
({void*_tmp31A=0U;({unsigned int _tmp8A6=e->loc;struct _dyneither_ptr _tmp8A5=({const char*_tmp31B="can't calculate numelts";_tag_dyneither(_tmp31B,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8A6,_tmp8A5,_tag_dyneither(_tmp31A,sizeof(void*),0U));});});
# 1898
({void*_tmp8A9=({struct Cyc_Absyn_Exp*_tmp8A8=arg;struct Cyc_Absyn_Exp*_tmp8A7=_tmp31C;Cyc_Toc_conditional_exp_r(_tmp8A8,_tmp8A7,Cyc_Absyn_uint_exp(0U,0U));});e->r=_tmp8A9;});}else{
# 1900
e->r=_tmp31C->r;goto _LL73;}}
# 1902
goto _LL73;}_LL73:;}
# 1904
goto _LL6C;default: _LL71: _LL72:
# 1906
({struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp78C;_tmp78C.tag=0U,({
struct _dyneither_ptr _tmp8AA=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp78C.f1=_tmp8AA;});_tmp78C;});struct Cyc_String_pa_PrintArg_struct _tmp320=({struct Cyc_String_pa_PrintArg_struct _tmp78B;_tmp78B.tag=0U,({struct _dyneither_ptr _tmp8AB=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_typ2string((void*)_check_null(arg->topt)));_tmp78B.f1=_tmp8AB;});_tmp78B;});void*_tmp31D[2U];_tmp31D[0]=& _tmp31F,_tmp31D[1]=& _tmp320;({struct _dyneither_ptr _tmp8AC=({const char*_tmp31E="size primop applied to non-array %s (%s)";_tag_dyneither(_tmp31E,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8AC,_tag_dyneither(_tmp31D,sizeof(void*),2U));});});}_LL6C:;}
# 1909
goto _LL57;}case Cyc_Absyn_Plus: _LL5A: _LL5B:
# 1914
{void*_tmp326=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(_tmp310))->hd);void*_tmp327=_tmp326;void*_tmp32F;union Cyc_Absyn_Constraint*_tmp32E;union Cyc_Absyn_Constraint*_tmp32D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327)->tag == 5U){_LL79: _tmp32F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327)->f1).elt_typ;_tmp32E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327)->f1).ptr_atts).bounds;_tmp32D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp327)->f1).ptr_atts).zero_term;_LL7A:
# 1916
{void*_tmp328=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp32E);void*_tmp329=_tmp328;struct Cyc_Absyn_Exp*_tmp32C;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp329)->tag == 0U){_LL7E: _LL7F: {
# 1918
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3->tl))->hd;
({void*_tmp8AF=({struct Cyc_Absyn_Exp*_tmp8AE=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8AE,({struct Cyc_Absyn_Exp*_tmp32A[3U];_tmp32A[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8AD=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp32F),0U);_tmp32A[1]=_tmp8AD;}),_tmp32A[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32A,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1920
e->r=_tmp8AF;});
# 1922
goto _LL7D;}}else{_LL80: _tmp32C=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp329)->f1;_LL81:
# 1924
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp32D)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3->tl))->hd;
({void*_tmp8B1=({struct Cyc_Absyn_Exp*_tmp8B0=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,e1);Cyc_Absyn_fncall_exp(_tmp8B0,({struct Cyc_Absyn_Exp*_tmp32B[3U];_tmp32B[0]=e1,_tmp32B[1]=_tmp32C,_tmp32B[2]=e2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp32B,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);})->r;e->r=_tmp8B1;});}
# 1929
goto _LL7D;}_LL7D:;}
# 1931
goto _LL78;}else{_LL7B: _LL7C:
 goto _LL78;}_LL78:;}
# 1934
goto _LL57;case Cyc_Absyn_Minus: _LL5C: _LL5D: {
# 1939
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt((void*)((struct Cyc_List_List*)_check_null(_tmp310))->hd,& elt_typ)){
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3->tl))->hd;
if(Cyc_Tcutil_is_tagged_pointer_typ((void*)((struct Cyc_List_List*)_check_null(_tmp310->tl))->hd)){
({void*_tmp8B3=({struct Cyc_Absyn_Exp*_tmp8B2=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8B2,Cyc_Toc_curr_sp);});e1->r=_tmp8B3;});
({void*_tmp8B5=({struct Cyc_Absyn_Exp*_tmp8B4=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_aggrmember_exp_r(_tmp8B4,Cyc_Toc_curr_sp);});e2->r=_tmp8B5;});
({void*_tmp8B7=({void*_tmp8B6=Cyc_Absyn_cstar_typ(Cyc_Absyn_uchar_typ,Cyc_Toc_mt_tq);e2->topt=_tmp8B6;});e1->topt=_tmp8B7;});
({void*_tmp8B9=({struct Cyc_Absyn_Exp*_tmp8B8=Cyc_Absyn_copy_exp(e);Cyc_Absyn_divide_exp(_tmp8B8,
Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U),0U);})->r;
# 1947
e->r=_tmp8B9;});}else{
# 1950
({void*_tmp8BD=({struct Cyc_Absyn_Exp*_tmp8BC=Cyc_Toc__dyneither_ptr_plus_e;Cyc_Toc_fncall_exp_r(_tmp8BC,({struct Cyc_Absyn_Exp*_tmp330[3U];_tmp330[0]=e1,({
struct Cyc_Absyn_Exp*_tmp8BB=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp330[1]=_tmp8BB;}),({
struct Cyc_Absyn_Exp*_tmp8BA=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,e2,0U);_tmp330[2]=_tmp8BA;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp330,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 1950
e->r=_tmp8BD;});}}
# 1955
goto _LL57;}case Cyc_Absyn_Eq: _LL5E: _LL5F:
 goto _LL61;case Cyc_Absyn_Neq: _LL60: _LL61:
 goto _LL63;case Cyc_Absyn_Gt: _LL62: _LL63:
 goto _LL65;case Cyc_Absyn_Gte: _LL64: _LL65:
 goto _LL67;case Cyc_Absyn_Lt: _LL66: _LL67:
 goto _LL69;case Cyc_Absyn_Lte: _LL68: _LL69: {
# 1963
struct Cyc_Absyn_Exp*e1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3))->hd;
struct Cyc_Absyn_Exp*e2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4D3->tl))->hd;
void*t1=(void*)((struct Cyc_List_List*)_check_null(_tmp310))->hd;
void*t2=(void*)((struct Cyc_List_List*)_check_null(_tmp310->tl))->hd;
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(t1,& elt_typ)){
void*_tmp331=({void*_tmp8BE=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8BE,Cyc_Toc_mt_tq);});
({void*_tmp8C1=({void*_tmp8C0=_tmp331;Cyc_Toc_cast_it_r(_tmp8C0,({struct Cyc_Absyn_Exp*_tmp8BF=Cyc_Absyn_new_exp(e1->r,0U);Cyc_Toc_member_exp(_tmp8BF,Cyc_Toc_curr_sp,0U);}));});e1->r=_tmp8C1;});
e1->topt=_tmp331;}
# 1973
if(Cyc_Tcutil_is_tagged_pointer_typ(t2)){
void*_tmp332=({void*_tmp8C2=Cyc_Toc_typ_to_c(elt_typ);Cyc_Absyn_cstar_typ(_tmp8C2,Cyc_Toc_mt_tq);});
({void*_tmp8C5=({void*_tmp8C4=_tmp332;Cyc_Toc_cast_it_r(_tmp8C4,({struct Cyc_Absyn_Exp*_tmp8C3=Cyc_Absyn_new_exp(e2->r,0U);Cyc_Toc_member_exp(_tmp8C3,Cyc_Toc_curr_sp,0U);}));});e2->r=_tmp8C5;});
e2->topt=_tmp332;}
# 1978
goto _LL57;}default: _LL6A: _LL6B:
 goto _LL57;}_LL57:;}
# 1981
goto _LL0;}case 5U: _LLB: _tmp4D6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4D5=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LLC: {
# 1983
void*e2_cyc_typ=(void*)_check_null(_tmp4D6->topt);
# 1992 "toc.cyc"
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
struct _dyneither_ptr incr_str=({const char*_tmp34C="increment";_tag_dyneither(_tmp34C,sizeof(char),10U);});
if(_tmp4D5 == Cyc_Absyn_PreDec  || _tmp4D5 == Cyc_Absyn_PostDec)({struct _dyneither_ptr _tmp8C6=({const char*_tmp333="decrement";_tag_dyneither(_tmp333,sizeof(char),10U);});incr_str=_tmp8C6;});
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4D6,& ptr_type,& is_dyneither,& elt_type)){
({struct Cyc_String_pa_PrintArg_struct _tmp336=({struct Cyc_String_pa_PrintArg_struct _tmp78D;_tmp78D.tag=0U,_tmp78D.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp78D;});void*_tmp334[1U];_tmp334[0]=& _tmp336;({unsigned int _tmp8C8=e->loc;struct _dyneither_ptr _tmp8C7=({const char*_tmp335="in-place %s is not supported when dereferencing a zero-terminated pointer";_tag_dyneither(_tmp335,sizeof(char),74U);});Cyc_Tcutil_terr(_tmp8C8,_tmp8C7,_tag_dyneither(_tmp334,sizeof(void*),1U));});});
({void*_tmp337=0U;({struct _dyneither_ptr _tmp8C9=({const char*_tmp338="in-place inc/dec on zero-term";_tag_dyneither(_tmp338,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8C9,_tag_dyneither(_tmp337,sizeof(void*),0U));});});}{
# 2001
void*tunion_member_type=(void*)& Cyc_Absyn_VoidType_val;
int f_tag=0;
if(Cyc_Toc_is_tagged_union_project(_tmp4D6,& f_tag,& tunion_member_type,1)){
struct Cyc_Absyn_Exp*_tmp339=Cyc_Absyn_signed_int_exp(1,0U);
_tmp339->topt=Cyc_Absyn_sint_typ;{
enum Cyc_Absyn_Incrementor _tmp33A=_tmp4D5;switch(_tmp33A){case Cyc_Absyn_PreInc: _LL83: _LL84:
# 2008
({void*_tmp8CB=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33C=_cycalloc(sizeof(*_tmp33C));_tmp33C->tag=4U,_tmp33C->f1=_tmp4D6,({struct Cyc_Core_Opt*_tmp8CA=({struct Cyc_Core_Opt*_tmp33B=_cycalloc(sizeof(*_tmp33B));_tmp33B->v=(void*)Cyc_Absyn_Plus;_tmp33B;});_tmp33C->f2=_tmp8CA;}),_tmp33C->f3=_tmp339;_tmp33C;});e->r=_tmp8CB;});
Cyc_Toc_exp_to_c(nv,e);
return;case Cyc_Absyn_PreDec: _LL85: _LL86:
# 2012
({void*_tmp8CD=(void*)({struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*_tmp33E=_cycalloc(sizeof(*_tmp33E));_tmp33E->tag=4U,_tmp33E->f1=_tmp4D6,({struct Cyc_Core_Opt*_tmp8CC=({struct Cyc_Core_Opt*_tmp33D=_cycalloc(sizeof(*_tmp33D));_tmp33D->v=(void*)Cyc_Absyn_Minus;_tmp33D;});_tmp33E->f2=_tmp8CC;}),_tmp33E->f3=_tmp339;_tmp33E;});e->r=_tmp8CD;});
Cyc_Toc_exp_to_c(nv,e);
return;default: _LL87: _LL88:
# 2016
({struct Cyc_String_pa_PrintArg_struct _tmp341=({struct Cyc_String_pa_PrintArg_struct _tmp78E;_tmp78E.tag=0U,_tmp78E.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)incr_str);_tmp78E;});void*_tmp33F[1U];_tmp33F[0]=& _tmp341;({unsigned int _tmp8CF=e->loc;struct _dyneither_ptr _tmp8CE=({const char*_tmp340="in-place post%s is not supported on @tagged union members";_tag_dyneither(_tmp340,sizeof(char),58U);});Cyc_Tcutil_terr(_tmp8CF,_tmp8CE,_tag_dyneither(_tmp33F,sizeof(void*),1U));});});
# 2018
({void*_tmp342=0U;({struct _dyneither_ptr _tmp8D0=({const char*_tmp343="in-place inc/dec on @tagged union";_tag_dyneither(_tmp343,sizeof(char),34U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8D0,_tag_dyneither(_tmp342,sizeof(void*),0U));});});}_LL82:;};}{
# 2021
int need_null_ck=(Cyc_Tcutil_is_pointer_type(old_typ) && 
Cyc_Tcutil_is_nullable_pointer_type(old_typ)) && Cyc_Toc_need_null_check(_tmp4D6);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D6);
Cyc_Toc_set_lhs(nv,0);{
# 2028
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*fn_e;
int change=1;
fn_e=(_tmp4D5 == Cyc_Absyn_PostInc  || _tmp4D5 == Cyc_Absyn_PostDec)?Cyc_Toc__dyneither_ptr_inplace_plus_post_e: Cyc_Toc__dyneither_ptr_inplace_plus_e;
# 2034
if(_tmp4D5 == Cyc_Absyn_PreDec  || _tmp4D5 == Cyc_Absyn_PostDec)
change=-1;
({void*_tmp8D5=({struct Cyc_Absyn_Exp*_tmp8D4=fn_e;Cyc_Toc_fncall_exp_r(_tmp8D4,({struct Cyc_Absyn_Exp*_tmp344[3U];({struct Cyc_Absyn_Exp*_tmp8D3=Cyc_Toc_push_address_exp(_tmp4D6);_tmp344[0]=_tmp8D3;}),({
struct Cyc_Absyn_Exp*_tmp8D2=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp344[1]=_tmp8D2;}),({
struct Cyc_Absyn_Exp*_tmp8D1=Cyc_Absyn_signed_int_exp(change,0U);_tmp344[2]=_tmp8D1;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp344,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2036
e->r=_tmp8D5;});}else{
# 2039
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2045
if(_tmp4D5 != Cyc_Absyn_PostInc){
struct _tuple1*_tmp345=Cyc_Toc_temp_var();
void*_tmp346=({void*_tmp8D6=Cyc_Toc_typ_to_c(old_typ);Cyc_Absyn_cstar_typ(_tmp8D6,Cyc_Toc_mt_tq);});
struct Cyc_Absyn_Exp*_tmp347=Cyc_Toc_push_address_exp(_tmp4D6);
struct Cyc_Absyn_Exp*_tmp348=({struct Cyc_Absyn_Exp*_tmp8D7=Cyc_Absyn_deref_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp345,0U),0U),0U);Cyc_Absyn_neq_exp(_tmp8D7,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
if(need_null_ck)
({struct Cyc_Absyn_Exp*_tmp8DA=({struct Cyc_Absyn_Exp*_tmp8D9=({struct Cyc_Absyn_Exp*_tmp8D8=Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp345,0U),0U);Cyc_Absyn_neq_exp(_tmp8D8,
Cyc_Absyn_int_exp(Cyc_Absyn_None,0,0U),0U);});
# 2052
Cyc_Absyn_and_exp(_tmp8D9,_tmp348,0U);});_tmp348=_tmp8DA;});{
# 2054
struct Cyc_Absyn_Stmt*_tmp349=({struct Cyc_Absyn_Exp*_tmp8DC=_tmp348;struct Cyc_Absyn_Stmt*_tmp8DB=
Cyc_Absyn_exp_stmt(Cyc_Absyn_pre_inc_exp(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp345,0U),0U),0U),0U);
# 2054
Cyc_Absyn_ifthenelse_stmt(_tmp8DC,_tmp8DB,
# 2056
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__throw_arraybounds_e,0,0U),0U),0U);});
({struct Cyc_Absyn_Stmt*_tmp8DE=({struct Cyc_Absyn_Stmt*_tmp8DD=_tmp349;Cyc_Absyn_seq_stmt(_tmp8DD,Cyc_Absyn_exp_stmt(Cyc_Absyn_deref_exp(Cyc_Absyn_var_exp(_tmp345,0U),0U),0U),0U);});_tmp349=_tmp8DE;});
({void*_tmp8DF=(Cyc_Absyn_stmt_exp(Cyc_Absyn_declare_stmt(_tmp345,_tmp346,_tmp347,_tmp349,0U),0U))->r;e->r=_tmp8DF;});};}else{
# 2061
struct Cyc_Toc_functionSet*_tmp34A=& Cyc_Toc__zero_arr_inplace_plus_post_functionSet;
struct Cyc_Absyn_Exp*fn_e=Cyc_Toc_getFunctionRemovePointer(_tmp34A,_tmp4D6);
({void*_tmp8E3=({struct Cyc_Absyn_Exp*_tmp8E2=fn_e;Cyc_Toc_fncall_exp_r(_tmp8E2,({struct Cyc_Absyn_Exp*_tmp34B[2U];({struct Cyc_Absyn_Exp*_tmp8E1=Cyc_Toc_push_address_exp(_tmp4D6);_tmp34B[0]=_tmp8E1;}),({
struct Cyc_Absyn_Exp*_tmp8E0=Cyc_Absyn_signed_int_exp(1,0U);_tmp34B[1]=_tmp8E0;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp34B,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2063
e->r=_tmp8E3;});}}else{
# 2066
if(elt_typ == (void*)& Cyc_Absyn_VoidType_val  && !Cyc_Absyn_is_lvalue(_tmp4D6)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor),enum Cyc_Absyn_Incrementor f_env))Cyc_Toc_lvalue_assign)(_tmp4D6,0,Cyc_Toc_incr_lvalue,_tmp4D5);
e->r=_tmp4D6->r;}}}
# 2070
goto _LL0;};};};}case 4U: _LLD: _tmp4D9=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4D8=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp4D7=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_LLE: {
# 2089 "toc.cyc"
void*e1_old_typ=(void*)_check_null(_tmp4D9->topt);
void*e2_old_typ=(void*)_check_null(_tmp4D7->topt);
int f_tag=0;
void*tagged_member_struct_type=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Toc_is_tagged_union_project(_tmp4D9,& f_tag,& tagged_member_struct_type,1)){
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D9);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4D7);
({void*_tmp8E4=Cyc_Toc_tagged_union_assignop(_tmp4D9,e1_old_typ,_tmp4D8,_tmp4D7,e2_old_typ,f_tag,tagged_member_struct_type);e->r=_tmp8E4;});
# 2100
return;}{
# 2102
void*ptr_type=(void*)& Cyc_Absyn_VoidType_val;
void*elt_type=(void*)& Cyc_Absyn_VoidType_val;
int is_dyneither=0;
if(Cyc_Tcutil_is_zero_ptr_deref(_tmp4D9,& ptr_type,& is_dyneither,& elt_type)){
Cyc_Toc_zero_ptr_assign_to_c(nv,e,_tmp4D9,_tmp4D8,_tmp4D7,ptr_type,is_dyneither,elt_type);
# 2108
return;}{
# 2112
int e1_poly=Cyc_Toc_is_poly_project(_tmp4D9);
Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4D9);
Cyc_Toc_set_lhs(nv,0);
Cyc_Toc_exp_to_c(nv,_tmp4D7);{
# 2118
int done=0;
if(_tmp4D8 != 0){
void*elt_typ=(void*)& Cyc_Absyn_VoidType_val;
if(Cyc_Tcutil_is_tagged_pointer_typ_elt(old_typ,& elt_typ)){
struct Cyc_Absyn_Exp*change;
{enum Cyc_Absyn_Primop _tmp34D=(enum Cyc_Absyn_Primop)_tmp4D8->v;enum Cyc_Absyn_Primop _tmp34E=_tmp34D;switch(_tmp34E){case Cyc_Absyn_Plus: _LL8A: _LL8B:
 change=_tmp4D7;goto _LL89;case Cyc_Absyn_Minus: _LL8C: _LL8D:
# 2126
({struct Cyc_Absyn_Exp*_tmp8E5=Cyc_Absyn_prim1_exp(Cyc_Absyn_Minus,_tmp4D7,0U);change=_tmp8E5;});goto _LL89;default: _LL8E: _LL8F:
({void*_tmp34F=0U;({struct _dyneither_ptr _tmp8E6=({const char*_tmp350="bad t ? pointer arithmetic";_tag_dyneither(_tmp350,sizeof(char),27U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8E6,_tag_dyneither(_tmp34F,sizeof(void*),0U));});});}_LL89:;}
# 2129
done=1;{
# 2131
struct Cyc_Absyn_Exp*_tmp351=Cyc_Toc__dyneither_ptr_inplace_plus_e;
({void*_tmp8EA=({struct Cyc_Absyn_Exp*_tmp8E9=_tmp351;Cyc_Toc_fncall_exp_r(_tmp8E9,({struct Cyc_Absyn_Exp*_tmp352[3U];({struct Cyc_Absyn_Exp*_tmp8E8=Cyc_Toc_push_address_exp(_tmp4D9);_tmp352[0]=_tmp8E8;}),({
struct Cyc_Absyn_Exp*_tmp8E7=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(elt_typ),0U);_tmp352[1]=_tmp8E7;}),_tmp352[2]=change;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp352,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2132
e->r=_tmp8EA;});};}else{
# 2135
if(Cyc_Tcutil_is_zero_pointer_typ_elt(old_typ,& elt_typ)){
# 2138
enum Cyc_Absyn_Primop _tmp353=(enum Cyc_Absyn_Primop)_tmp4D8->v;enum Cyc_Absyn_Primop _tmp354=_tmp353;if(_tmp354 == Cyc_Absyn_Plus){_LL91: _LL92:
# 2140
 done=1;
({void*_tmp8EC=({struct Cyc_Absyn_Exp*_tmp8EB=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_inplace_plus_functionSet,_tmp4D9);Cyc_Toc_fncall_exp_r(_tmp8EB,({struct Cyc_Absyn_Exp*_tmp355[2U];_tmp355[0]=_tmp4D9,_tmp355[1]=_tmp4D7;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp355,sizeof(struct Cyc_Absyn_Exp*),2U));}));});e->r=_tmp8EC;});
goto _LL90;}else{_LL93: _LL94:
({void*_tmp356=0U;({struct _dyneither_ptr _tmp8ED=({const char*_tmp357="bad zero-terminated pointer arithmetic";_tag_dyneither(_tmp357,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8ED,_tag_dyneither(_tmp356,sizeof(void*),0U));});});}_LL90:;}}}
# 2147
if(!done){
# 2149
if(e1_poly)
({void*_tmp8EF=({void*_tmp8EE=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmp8EE,Cyc_Absyn_new_exp(_tmp4D7->r,0U));});_tmp4D7->r=_tmp8EF;});
# 2155
if(!Cyc_Absyn_is_lvalue(_tmp4D9)){
({struct Cyc_Absyn_Exp*_tmp8F0=_tmp4D9;((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,struct _tuple31*),struct _tuple31*f_env))Cyc_Toc_lvalue_assign)(_tmp8F0,0,Cyc_Toc_assignop_lvalue,({unsigned int _tmp359=1;struct _tuple31*_tmp358=_cycalloc(_check_times(_tmp359,sizeof(struct _tuple31)));(_tmp358[0]).f1=_tmp4D8,(_tmp358[0]).f2=_tmp4D7;_tmp358;}));});
e->r=_tmp4D9->r;}}
# 2160
goto _LL0;};};};}case 6U: _LLF: _tmp4DC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4DB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp4DA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_LL10:
# 2162
 Cyc_Toc_exp_to_c(nv,_tmp4DC);
Cyc_Toc_exp_to_c(nv,_tmp4DB);
Cyc_Toc_exp_to_c(nv,_tmp4DA);
goto _LL0;case 7U: _LL11: _tmp4DE=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4DD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL12:
 _tmp4E0=_tmp4DE;_tmp4DF=_tmp4DD;goto _LL14;case 8U: _LL13: _tmp4E0=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4DF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL14:
 _tmp4E2=_tmp4E0;_tmp4E1=_tmp4DF;goto _LL16;case 9U: _LL15: _tmp4E2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4E1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL16:
# 2169
 Cyc_Toc_exp_to_c(nv,_tmp4E2);
Cyc_Toc_exp_to_c(nv,_tmp4E1);
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30D)->f3 == 0){_LL17: _tmp4E4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4E3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL18:
# 2173
 Cyc_Toc_exp_to_c(nv,_tmp4E4);
((void(*)(void(*f)(struct Cyc_Toc_Env*,struct Cyc_Absyn_Exp*),struct Cyc_Toc_Env*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Toc_exp_to_c,nv,_tmp4E3);
goto _LL0;}else{_LL19: _tmp4E9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4E8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp4E7=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30D)->f3)->num_varargs;_tmp4E6=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30D)->f3)->injectors;_tmp4E5=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp30D)->f3)->vai;_LL1A: {
# 2185 "toc.cyc"
struct _RegionHandle _tmp35A=_new_region("r");struct _RegionHandle*r=& _tmp35A;_push_region(r);{
struct _tuple1*argv=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*argvexp=Cyc_Absyn_var_exp(argv,0U);
struct Cyc_Absyn_Exp*num_varargs_exp=Cyc_Absyn_uint_exp((unsigned int)_tmp4E7,0U);
void*cva_type=Cyc_Toc_typ_to_c(_tmp4E5->type);
void*arr_type=Cyc_Absyn_array_typ(cva_type,Cyc_Toc_mt_tq,num_varargs_exp,Cyc_Absyn_false_conref,0U);
# 2193
int num_args=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E8);
int num_normargs=num_args - _tmp4E7;
# 2197
struct Cyc_List_List*new_args=0;
{int i=0;for(0;i < num_normargs;(++ i,_tmp4E8=_tmp4E8->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp4E8))->hd);
({struct Cyc_List_List*_tmp8F1=({struct Cyc_List_List*_tmp35B=_cycalloc(sizeof(*_tmp35B));_tmp35B->hd=(struct Cyc_Absyn_Exp*)_tmp4E8->hd,_tmp35B->tl=new_args;_tmp35B;});new_args=_tmp8F1;});}}
# 2202
({struct Cyc_List_List*_tmp8F5=({struct Cyc_List_List*_tmp35D=_cycalloc(sizeof(*_tmp35D));({struct Cyc_Absyn_Exp*_tmp8F4=({struct Cyc_Absyn_Exp*_tmp8F3=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp8F3,({struct Cyc_Absyn_Exp*_tmp35C[3U];_tmp35C[0]=argvexp,({
# 2204
struct Cyc_Absyn_Exp*_tmp8F2=Cyc_Absyn_sizeoftyp_exp(cva_type,0U);_tmp35C[1]=_tmp8F2;}),_tmp35C[2]=num_varargs_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp35C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2202
_tmp35D->hd=_tmp8F4;}),_tmp35D->tl=new_args;_tmp35D;});new_args=_tmp8F5;});
# 2207
({struct Cyc_List_List*_tmp8F6=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_args);new_args=_tmp8F6;});
# 2209
Cyc_Toc_exp_to_c(nv,_tmp4E9);{
struct Cyc_Absyn_Stmt*s=Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(_tmp4E9,new_args,0U),0U);
# 2213
if(_tmp4E5->inject){
struct Cyc_Absyn_Datatypedecl*tud;
{void*_tmp35E=Cyc_Tcutil_compress(Cyc_Tcutil_pointer_elt_type(_tmp4E5->type));void*_tmp35F=_tmp35E;struct Cyc_Absyn_Datatypedecl*_tmp362;if(((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35F)->tag == 3U){if((((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35F)->f1).datatype_info).KnownDatatype).tag == 2){_LL96: _tmp362=*(((((struct Cyc_Absyn_DatatypeType_Absyn_Type_struct*)_tmp35F)->f1).datatype_info).KnownDatatype).val;_LL97:
 tud=_tmp362;goto _LL95;}else{goto _LL98;}}else{_LL98: _LL99:
({void*_tmp360=0U;({struct _dyneither_ptr _tmp8F7=({const char*_tmp361="toc: unknown datatype in vararg with inject";_tag_dyneither(_tmp361,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp8F7,_tag_dyneither(_tmp360,sizeof(void*),0U));});});}_LL95:;}{
# 2219
struct _dyneither_ptr vs=({unsigned int _tmp373=(unsigned int)_tmp4E7;struct _tuple1**_tmp372=({struct _RegionHandle*_tmp8F8=r;_region_malloc(_tmp8F8,_check_times(_tmp373,sizeof(struct _tuple1*)));});({{unsigned int _tmp78F=(unsigned int)_tmp4E7;unsigned int i;for(i=0;i < _tmp78F;++ i){({struct _tuple1*_tmp8F9=Cyc_Toc_temp_var();_tmp372[i]=_tmp8F9;});}}0;});_tag_dyneither(_tmp372,sizeof(struct _tuple1*),_tmp373);});
# 2221
if(_tmp4E7 != 0){
# 2225
struct Cyc_List_List*_tmp363=0;
{int i=_tmp4E7 - 1;for(0;i >= 0;-- i){
({struct Cyc_List_List*_tmp8FB=({struct Cyc_List_List*_tmp364=_cycalloc(sizeof(*_tmp364));({struct Cyc_Absyn_Exp*_tmp8FA=Cyc_Absyn_address_exp(Cyc_Absyn_var_exp(*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i)),0U),0U);_tmp364->hd=_tmp8FA;}),_tmp364->tl=_tmp363;_tmp364;});_tmp363=_tmp8FB;});}}
({struct Cyc_Absyn_Stmt*_tmp8FF=({struct _tuple1*_tmp8FE=argv;void*_tmp8FD=arr_type;struct Cyc_Absyn_Exp*_tmp8FC=Cyc_Absyn_array_exp(_tmp363,0U);Cyc_Absyn_declare_stmt(_tmp8FE,_tmp8FD,_tmp8FC,s,0U);});s=_tmp8FF;});
# 2230
({struct Cyc_List_List*_tmp900=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4E8);_tmp4E8=_tmp900;});
({struct Cyc_List_List*_tmp901=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4E6);_tmp4E6=_tmp901;});{
int i=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp4E8)- 1;
for(0;_tmp4E8 != 0;(_tmp4E8=_tmp4E8->tl,_tmp4E6=_tmp4E6->tl,-- i)){
struct Cyc_Absyn_Exp*arg=(struct Cyc_Absyn_Exp*)_tmp4E8->hd;
void*arg_type=(void*)_check_null(arg->topt);
struct _tuple1*var=*((struct _tuple1**)_check_dyneither_subscript(vs,sizeof(struct _tuple1*),i));
struct Cyc_Absyn_Exp*varexp=Cyc_Absyn_var_exp(var,0U);
struct Cyc_Absyn_Datatypefield*_tmp365=(struct Cyc_Absyn_Datatypefield*)((struct Cyc_List_List*)_check_null(_tmp4E6))->hd;struct Cyc_Absyn_Datatypefield*_tmp366=_tmp365;struct _tuple1*_tmp36C;struct Cyc_List_List*_tmp36B;_LL9B: _tmp36C=_tmp366->name;_tmp36B=_tmp366->typs;_LL9C:;{
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp36B))->hd)).f2);
Cyc_Toc_exp_to_c(nv,arg);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ))
({struct Cyc_Absyn_Exp*_tmp902=Cyc_Toc_cast_it(field_typ,arg);arg=_tmp902;});{
# 2244
struct _tuple1*_tmp367=Cyc_Toc_collapse_qvars(_tmp36C,tud->name);
struct Cyc_List_List*_tmp368=({struct _tuple20*_tmp36A[2U];({struct _tuple20*_tmp906=({struct _dyneither_ptr*_tmp905=Cyc_Toc_tag_sp;Cyc_Toc_make_field(_tmp905,Cyc_Toc_datatype_tag(tud,_tmp36C));});_tmp36A[0]=_tmp906;}),({
struct _tuple20*_tmp904=({struct _dyneither_ptr*_tmp903=Cyc_Absyn_fieldname(1);Cyc_Toc_make_field(_tmp903,arg);});_tmp36A[1]=_tmp904;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36A,sizeof(struct _tuple20*),2U));});
({struct Cyc_Absyn_Stmt*_tmp90A=({struct _tuple1*_tmp909=var;void*_tmp908=Cyc_Absyn_strctq(_tmp367);struct Cyc_Absyn_Exp*_tmp907=
Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369->tag=29U,_tmp369->f1=_tmp367,_tmp369->f2=0,_tmp369->f3=_tmp368,_tmp369->f4=0;_tmp369;}),0U);
# 2247
Cyc_Absyn_declare_stmt(_tmp909,_tmp908,_tmp907,s,0U);});s=_tmp90A;});};};}};}else{
# 2255
struct Cyc_List_List*_tmp36D=({struct _tuple32*_tmp36E[3U];({struct _tuple32*_tmp913=({struct _tuple32*_tmp36F=_cycalloc(sizeof(*_tmp36F));({struct _dyneither_ptr _tmp912=_tag_dyneither(0,0,0);_tmp36F->f1=_tmp912;}),({struct Cyc_Absyn_Exp*_tmp911=Cyc_Absyn_uint_exp(0U,0U);_tmp36F->f2=_tmp911;});_tmp36F;});_tmp36E[0]=_tmp913;}),({
struct _tuple32*_tmp910=({struct _tuple32*_tmp370=_cycalloc(sizeof(*_tmp370));({struct _dyneither_ptr _tmp90F=_tag_dyneither(0,0,0);_tmp370->f1=_tmp90F;}),({struct Cyc_Absyn_Exp*_tmp90E=Cyc_Absyn_uint_exp(0U,0U);_tmp370->f2=_tmp90E;});_tmp370;});_tmp36E[1]=_tmp910;}),({
struct _tuple32*_tmp90D=({struct _tuple32*_tmp371=_cycalloc(sizeof(*_tmp371));({struct _dyneither_ptr _tmp90C=_tag_dyneither(0,0,0);_tmp371->f1=_tmp90C;}),({struct Cyc_Absyn_Exp*_tmp90B=Cyc_Absyn_uint_exp(0U,0U);_tmp371->f2=_tmp90B;});_tmp371;});_tmp36E[2]=_tmp90D;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp36E,sizeof(struct _tuple32*),3U));});
({struct Cyc_Absyn_Stmt*_tmp917=({struct _tuple1*_tmp916=argv;void*_tmp915=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp914=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp916,_tmp915,_tmp914,s,0U);});s=_tmp917;});}};}else{
# 2263
if(_tmp4E7 != 0){
struct Cyc_List_List*array_args=0;
for(0;_tmp4E8 != 0;_tmp4E8=_tmp4E8->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp4E8->hd);
({struct Cyc_List_List*_tmp918=({struct Cyc_List_List*_tmp374=_cycalloc(sizeof(*_tmp374));_tmp374->hd=(struct Cyc_Absyn_Exp*)_tmp4E8->hd,_tmp374->tl=array_args;_tmp374;});array_args=_tmp918;});}{
# 2269
struct Cyc_Absyn_Exp*_tmp375=Cyc_Absyn_array_exp(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(array_args),0U);
({struct Cyc_Absyn_Stmt*_tmp919=Cyc_Absyn_declare_stmt(argv,arr_type,_tmp375,s,0U);s=_tmp919;});};}else{
# 2272
({struct Cyc_Absyn_Stmt*_tmp91D=({struct _tuple1*_tmp91C=argv;void*_tmp91B=Cyc_Absyn_void_star_typ();struct Cyc_Absyn_Exp*_tmp91A=Cyc_Absyn_uint_exp(0U,0U);Cyc_Absyn_declare_stmt(_tmp91C,_tmp91B,_tmp91A,s,0U);});s=_tmp91D;});}}
# 2275
({void*_tmp91E=Cyc_Toc_stmt_exp_r(s);e->r=_tmp91E;});};}
# 2277
_npop_handler(0U);goto _LL0;
# 2185
;_pop_region(r);}}case 11U: _LL1B: _tmp4EB=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4EA=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL1C:
# 2280
 Cyc_Toc_exp_to_c(nv,_tmp4EB);{
struct Cyc_Absyn_Exp*_tmp376=_tmp4EA?Cyc_Toc_newrethrow_exp(_tmp4EB): Cyc_Toc_newthrow_exp(_tmp4EB);
({void*_tmp920=({void*_tmp91F=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_array_to_ptr_cast(_tmp91F,_tmp376,0U);})->r;e->r=_tmp920;});
goto _LL0;};case 12U: _LL1D: _tmp4EC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_LL1E:
 Cyc_Toc_exp_to_c(nv,_tmp4EC);goto _LL0;case 13U: _LL1F: _tmp4EE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4ED=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL20:
# 2286
 Cyc_Toc_exp_to_c(nv,_tmp4EE);
# 2295 "toc.cyc"
for(0;_tmp4ED != 0;_tmp4ED=_tmp4ED->tl){
enum Cyc_Absyn_KindQual _tmp377=(Cyc_Tcutil_typ_kind((void*)_tmp4ED->hd))->kind;
if(_tmp377 != Cyc_Absyn_EffKind  && _tmp377 != Cyc_Absyn_RgnKind){
{void*_tmp378=Cyc_Tcutil_compress((void*)_tmp4ED->hd);void*_tmp379=_tmp378;switch(*((int*)_tmp379)){case 2U: _LL9E: _LL9F:
 goto _LLA1;case 3U: _LLA0: _LLA1:
 continue;default: _LLA2: _LLA3:
# 2302
({void*_tmp922=({void*_tmp921=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp921,_tmp4EE,0U);})->r;e->r=_tmp922;});
goto _LL9D;}_LL9D:;}
# 2305
break;}}
# 2308
goto _LL0;case 14U: _LL21: _tmp4F2=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4F1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp4F0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_tmp4EF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp30D)->f4;_LL22: {
# 2310
void*old_t2=(void*)_check_null(_tmp4F1->topt);
void*old_t2_c=Cyc_Toc_typ_to_c(old_t2);
void*new_typ=*_tmp4F2;
void*new_typ_c=Cyc_Toc_typ_to_c(new_typ);
*_tmp4F2=new_typ_c;
Cyc_Toc_exp_to_c(nv,_tmp4F1);
# 2317
{struct _tuple33 _tmp37A=({struct _tuple33 _tmp793;({void*_tmp924=Cyc_Tcutil_compress(old_t2);_tmp793.f1=_tmp924;}),({void*_tmp923=Cyc_Tcutil_compress(new_typ);_tmp793.f2=_tmp923;});_tmp793;});struct _tuple33 _tmp37B=_tmp37A;struct Cyc_Absyn_PtrInfo _tmp3B3;struct Cyc_Absyn_PtrInfo _tmp3B2;struct Cyc_Absyn_PtrInfo _tmp3B1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37B.f1)->tag == 5U)switch(*((int*)_tmp37B.f2)){case 5U: _LLA5: _tmp3B2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37B.f1)->f1;_tmp3B1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37B.f2)->f1;_LLA6: {
# 2319
int _tmp37C=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B2.ptr_atts).nullable);
int _tmp37D=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B1.ptr_atts).nullable);
void*_tmp37E=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B2.ptr_atts).bounds);
void*_tmp37F=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B1.ptr_atts).bounds);
int _tmp380=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B2.ptr_atts).zero_term);
int _tmp381=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,(_tmp3B1.ptr_atts).zero_term);
{struct _tuple33 _tmp382=({struct _tuple33 _tmp792;_tmp792.f1=_tmp37E,_tmp792.f2=_tmp37F;_tmp792;});struct _tuple33 _tmp383=_tmp382;struct Cyc_Absyn_Exp*_tmp3AE;struct Cyc_Absyn_Exp*_tmp3AD;struct Cyc_Absyn_Exp*_tmp3AC;struct Cyc_Absyn_Exp*_tmp3AB;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp383.f1)->tag == 1U){if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp383.f2)->tag == 1U){_LLAC: _tmp3AC=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp383.f1)->f1;_tmp3AB=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp383.f2)->f1;_LLAD:
# 2327
 if((!Cyc_Evexp_c_can_eval(_tmp3AC) || !Cyc_Evexp_c_can_eval(_tmp3AB)) && !
Cyc_Evexp_same_const_exp(_tmp3AC,_tmp3AB))
({void*_tmp384=0U;({unsigned int _tmp926=e->loc;struct _dyneither_ptr _tmp925=({const char*_tmp385="can't validate cast due to potential size differences";_tag_dyneither(_tmp385,sizeof(char),54U);});Cyc_Tcutil_terr(_tmp926,_tmp925,_tag_dyneither(_tmp384,sizeof(void*),0U));});});
if(_tmp37C  && !_tmp37D){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp386=0U;({struct _dyneither_ptr _tmp927=({const char*_tmp387="can't do NULL-check conversion at top-level";_tag_dyneither(_tmp387,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp927,_tag_dyneither(_tmp386,sizeof(void*),0U));});});
# 2336
if(_tmp4EF != Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp38A=({struct Cyc_String_pa_PrintArg_struct _tmp790;_tmp790.tag=0U,({struct _dyneither_ptr _tmp928=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp790.f1=_tmp928;});_tmp790;});void*_tmp388[1U];_tmp388[0]=& _tmp38A;({struct _dyneither_ptr _tmp929=({const char*_tmp389="null-check conversion mis-classified: %s";_tag_dyneither(_tmp389,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp929,_tag_dyneither(_tmp388,sizeof(void*),1U));});});{
int do_null_check=Cyc_Toc_need_null_check(_tmp4F1);
if(do_null_check){
if(!_tmp4F0)
({void*_tmp38B=0U;({unsigned int _tmp92B=e->loc;struct _dyneither_ptr _tmp92A=({const char*_tmp38C="inserted null check due to implicit cast from * to @ type";_tag_dyneither(_tmp38C,sizeof(char),58U);});Cyc_Tcutil_warn(_tmp92B,_tmp92A,_tag_dyneither(_tmp38B,sizeof(void*),0U));});});
# 2343
({void*_tmp92E=({void*_tmp92D=*_tmp4F2;Cyc_Toc_cast_it_r(_tmp92D,({struct Cyc_Absyn_Exp*_tmp92C=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp92C,({struct Cyc_List_List*_tmp38D=_cycalloc(sizeof(*_tmp38D));
_tmp38D->hd=_tmp4F1,_tmp38D->tl=0;_tmp38D;}),0U);}));});
# 2343
e->r=_tmp92E;});}else{
# 2347
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4F1->r;}};}else{
# 2352
if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4F1->r;}
# 2359
goto _LLAB;}else{_LLAE: _tmp3AD=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp383.f1)->f1;_LLAF:
# 2361
 if(!Cyc_Evexp_c_can_eval(_tmp3AD))
({void*_tmp38E=0U;({unsigned int _tmp930=e->loc;struct _dyneither_ptr _tmp92F=({const char*_tmp38F="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp38F,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp930,_tmp92F,_tag_dyneither(_tmp38E,sizeof(void*),0U));});});
# 2364
if(_tmp4EF == Cyc_Absyn_Null_to_NonNull)
({struct Cyc_String_pa_PrintArg_struct _tmp392=({struct Cyc_String_pa_PrintArg_struct _tmp791;_tmp791.tag=0U,({struct _dyneither_ptr _tmp931=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp791.f1=_tmp931;});_tmp791;});void*_tmp390[1U];_tmp390[0]=& _tmp392;({struct _dyneither_ptr _tmp932=({const char*_tmp391="conversion mis-classified as null-check: %s";_tag_dyneither(_tmp391,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp932,_tag_dyneither(_tmp390,sizeof(void*),1U));});});
if(Cyc_Toc_is_toplevel(nv)){
# 2368
if((_tmp380  && !(_tmp3B1.elt_tq).real_const) && !_tmp381)
# 2371
({struct Cyc_Absyn_Exp*_tmp934=({struct Cyc_Absyn_Exp*_tmp933=_tmp3AD;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp933,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3AD=_tmp934;});
# 2373
({void*_tmp935=(Cyc_Toc_make_toplevel_dyn_arr(old_t2,_tmp3AD,_tmp4F1))->r;e->r=_tmp935;});}else{
# 2375
struct Cyc_Absyn_Exp*_tmp393=Cyc_Toc__tag_dyneither_e;
# 2377
if(_tmp380){
# 2382
struct _tuple1*_tmp394=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp395=Cyc_Absyn_var_exp(_tmp394,0U);
struct Cyc_Absyn_Exp*arg3;
# 2387
{void*_tmp396=_tmp4F1->r;void*_tmp397=_tmp396;if(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp397)->tag == 0U)switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp397)->f1).Wstring_c).tag){case 8U: _LLB5: _LLB6:
# 2389
 arg3=_tmp3AD;goto _LLB4;case 9U: _LLB7: _LLB8:
# 2391
 arg3=_tmp3AD;goto _LLB4;default: goto _LLB9;}else{_LLB9: _LLBA:
# 2393
({struct Cyc_Absyn_Exp*_tmp939=({struct Cyc_Absyn_Exp*_tmp938=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__get_zero_arr_size_functionSet,_tmp4F1);Cyc_Absyn_fncall_exp(_tmp938,({struct Cyc_Absyn_Exp*_tmp398[2U];({
# 2395
struct Cyc_Absyn_Exp*_tmp937=({void*_tmp936=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it(_tmp936,_tmp395);});_tmp398[0]=_tmp937;}),_tmp398[1]=_tmp3AD;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp398,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2393
arg3=_tmp939;});
# 2396
goto _LLB4;}_LLB4:;}
# 2398
if(!_tmp381  && !(_tmp3B1.elt_tq).real_const)
# 2401
({struct Cyc_Absyn_Exp*_tmp93B=({struct Cyc_Absyn_Exp*_tmp93A=arg3;Cyc_Absyn_prim2_exp(Cyc_Absyn_Minus,_tmp93A,Cyc_Absyn_uint_exp(1U,0U),0U);});arg3=_tmp93B;});{
# 2403
struct Cyc_Absyn_Exp*_tmp399=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B1.elt_typ),0U);
struct Cyc_Absyn_Exp*_tmp39A=({struct Cyc_Absyn_Exp*_tmp93C=_tmp393;Cyc_Absyn_fncall_exp(_tmp93C,({struct Cyc_Absyn_Exp*_tmp39C[3U];_tmp39C[0]=_tmp395,_tmp39C[1]=_tmp399,_tmp39C[2]=arg3;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
struct Cyc_Absyn_Stmt*_tmp39B=Cyc_Absyn_exp_stmt(_tmp39A,0U);
({struct Cyc_Absyn_Stmt*_tmp940=({struct _tuple1*_tmp93F=_tmp394;void*_tmp93E=Cyc_Toc_typ_to_c(old_t2);struct Cyc_Absyn_Exp*_tmp93D=_tmp4F1;Cyc_Absyn_declare_stmt(_tmp93F,_tmp93E,_tmp93D,_tmp39B,0U);});_tmp39B=_tmp940;});
({void*_tmp941=Cyc_Toc_stmt_exp_r(_tmp39B);e->r=_tmp941;});};}else{
# 2410
({void*_tmp944=({struct Cyc_Absyn_Exp*_tmp943=_tmp393;Cyc_Toc_fncall_exp_r(_tmp943,({struct Cyc_Absyn_Exp*_tmp39D[3U];_tmp39D[0]=_tmp4F1,({
# 2412
struct Cyc_Absyn_Exp*_tmp942=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B1.elt_typ),0U);_tmp39D[1]=_tmp942;}),_tmp39D[2]=_tmp3AD;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp39D,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2410
e->r=_tmp944;});}}
# 2416
goto _LLAB;}}else{if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp383.f2)->tag == 1U){_LLB0: _tmp3AE=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp383.f2)->f1;_LLB1:
# 2418
 if(!Cyc_Evexp_c_can_eval(_tmp3AE))
({void*_tmp39E=0U;({unsigned int _tmp946=e->loc;struct _dyneither_ptr _tmp945=({const char*_tmp39F="cannot perform coercion since numelts cannot be determined statically.";_tag_dyneither(_tmp39F,sizeof(char),71U);});Cyc_Tcutil_terr(_tmp946,_tmp945,_tag_dyneither(_tmp39E,sizeof(void*),0U));});});
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3A0=0U;({struct _dyneither_ptr _tmp947=({const char*_tmp3A1="can't coerce t? to t* or t@ at the top-level";_tag_dyneither(_tmp3A1,sizeof(char),45U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp947,_tag_dyneither(_tmp3A0,sizeof(void*),0U));});});{
# 2430 "toc.cyc"
struct Cyc_Absyn_Exp*_tmp3A2=_tmp3AE;
if(_tmp380  && !_tmp381)
({struct Cyc_Absyn_Exp*_tmp949=({struct Cyc_Absyn_Exp*_tmp948=_tmp3AE;Cyc_Absyn_add_exp(_tmp948,Cyc_Absyn_uint_exp(1U,0U),0U);});_tmp3A2=_tmp949;});{
# 2437
struct Cyc_Absyn_Exp*_tmp3A3=Cyc_Toc__untag_dyneither_ptr_e;
struct Cyc_Absyn_Exp*_tmp3A4=({struct Cyc_Absyn_Exp*_tmp94B=_tmp3A3;Cyc_Absyn_fncall_exp(_tmp94B,({struct Cyc_Absyn_Exp*_tmp3A6[3U];_tmp3A6[0]=_tmp4F1,({
# 2440
struct Cyc_Absyn_Exp*_tmp94A=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B2.elt_typ),0U);_tmp3A6[1]=_tmp94A;}),_tmp3A6[2]=_tmp3A2;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3A6,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2442
if(!_tmp37D)
({void*_tmp94E=({struct Cyc_Absyn_Exp*_tmp94D=Cyc_Toc__check_null_e;Cyc_Toc_fncall_exp_r(_tmp94D,({struct Cyc_List_List*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));
({struct Cyc_Absyn_Exp*_tmp94C=Cyc_Absyn_copy_exp(_tmp3A4);_tmp3A5->hd=_tmp94C;}),_tmp3A5->tl=0;_tmp3A5;}));});
# 2443
_tmp3A4->r=_tmp94E;});
# 2445
({void*_tmp94F=Cyc_Toc_cast_it_r(*_tmp4F2,_tmp3A4);e->r=_tmp94F;});
goto _LLAB;};};}else{_LLB2: _LLB3:
# 2450
 DynCast:
 if((_tmp380  && !_tmp381) && !(_tmp3B1.elt_tq).real_const){
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp3A7=0U;({struct _dyneither_ptr _tmp950=({const char*_tmp3A8="can't coerce a ZEROTERM to a non-const NOZEROTERM pointer at toplevel";_tag_dyneither(_tmp3A8,sizeof(char),70U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmp950,_tag_dyneither(_tmp3A7,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*_tmp3A9=Cyc_Toc__dyneither_ptr_decrease_size_e;
({void*_tmp954=({struct Cyc_Absyn_Exp*_tmp953=_tmp3A9;Cyc_Toc_fncall_exp_r(_tmp953,({struct Cyc_Absyn_Exp*_tmp3AA[3U];_tmp3AA[0]=_tmp4F1,({
struct Cyc_Absyn_Exp*_tmp952=Cyc_Absyn_sizeoftyp_exp(Cyc_Toc_typ_to_c(_tmp3B2.elt_typ),0U);_tmp3AA[1]=_tmp952;}),({
struct Cyc_Absyn_Exp*_tmp951=Cyc_Absyn_uint_exp(1U,0U);_tmp3AA[2]=_tmp951;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3AA,sizeof(struct Cyc_Absyn_Exp*),3U));}));});
# 2455
e->r=_tmp954;});};}
# 2459
goto _LLAB;}}_LLAB:;}
# 2461
goto _LLA4;}case 6U: _LLA7: _tmp3B3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp37B.f1)->f1;_LLA8:
# 2463
{void*_tmp3AF=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,(_tmp3B3.ptr_atts).bounds);void*_tmp3B0=_tmp3AF;if(((struct Cyc_Absyn_DynEither_b_Absyn_Bounds_struct*)_tmp3B0)->tag == 0U){_LLBC: _LLBD:
# 2465
({void*_tmp956=({struct Cyc_Absyn_Exp*_tmp955=Cyc_Absyn_new_exp(_tmp4F1->r,_tmp4F1->loc);Cyc_Toc_aggrmember_exp_r(_tmp955,Cyc_Toc_curr_sp);});_tmp4F1->r=_tmp956;});
_tmp4F1->topt=new_typ_c;
goto _LLBB;}else{_LLBE: _LLBF:
 goto _LLBB;}_LLBB:;}
# 2470
goto _LLA4;default: goto _LLA9;}else{_LLA9: _LLAA:
# 2472
 if(Cyc_Tcutil_unify(old_t2_c,new_typ_c))
e->r=_tmp4F1->r;
goto _LLA4;}_LLA4:;}
# 2476
goto _LL0;}case 15U: _LL23: _tmp4F3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_LL24:
# 2479
 Cyc_Toc_set_lhs(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4F3);
Cyc_Toc_set_lhs(nv,0);
if(!Cyc_Absyn_is_lvalue(_tmp4F3)){
((void(*)(struct Cyc_Absyn_Exp*e1,struct Cyc_List_List*fs,struct Cyc_Absyn_Exp*(*f)(struct Cyc_Absyn_Exp*,int),int f_env))Cyc_Toc_lvalue_assign)(_tmp4F3,0,Cyc_Toc_address_lvalue,1);
# 2485
({void*_tmp958=({void*_tmp957=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp957,_tmp4F3);});e->r=_tmp958;});}else{
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind((void*)_check_null(_tmp4F3->topt))))
# 2489
({void*_tmp95A=({void*_tmp959=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_cast_it_r(_tmp959,_tmp4F3);});e->r=_tmp95A;});}
# 2491
goto _LL0;case 16U: _LL25: _tmp4F5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4F4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL26:
# 2498
 Cyc_Toc_exp_to_c(nv,_tmp4F4);{
# 2500
void*elt_typ=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4F4->topt));
struct _tuple1*_tmp3B4=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp3B5=Cyc_Absyn_var_exp(_tmp3B4,0U);
struct Cyc_Absyn_Exp*lhs;
{void*_tmp3B6=Cyc_Tcutil_compress(elt_typ);void*_tmp3B7=_tmp3B6;void*_tmp3B8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B7)->tag == 8U){_LLC1: _tmp3B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B7)->f1).elt_type;_LLC2:
# 2506
({void*_tmp95B=Cyc_Toc_typ_to_c(_tmp3B8);elt_typ=_tmp95B;});
({struct Cyc_Absyn_Exp*_tmp95C=Cyc_Absyn_copy_exp(_tmp3B5);lhs=_tmp95C;});
goto _LLC0;}else{_LLC3: _LLC4:
# 2510
({struct Cyc_Absyn_Exp*_tmp95D=Cyc_Absyn_deref_exp(Cyc_Absyn_copy_exp(_tmp3B5),0U);lhs=_tmp95D;});
goto _LLC0;}_LLC0:;}{
# 2513
struct Cyc_Absyn_Exp*array_len=Cyc_Toc_array_length_exp(_tmp4F4);
struct _tuple1*_tmp3B9=(unsigned int)array_len?Cyc_Toc_temp_var(): 0;
struct Cyc_Absyn_Exp*_tmp3BA=(unsigned int)array_len?Cyc_Absyn_var_exp((struct _tuple1*)_check_null(_tmp3B9),0U): 0;
struct Cyc_Absyn_Exp*mexp;
if((unsigned int)array_len)
({struct Cyc_Absyn_Exp*_tmp960=({struct Cyc_Absyn_Exp*_tmp95F=Cyc_Toc__check_times_e;Cyc_Absyn_fncall_exp(_tmp95F,({struct Cyc_Absyn_Exp*_tmp3BB[2U];_tmp3BB[0]=(struct Cyc_Absyn_Exp*)_check_null(_tmp3BA),({
struct Cyc_Absyn_Exp*_tmp95E=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3BB[1]=_tmp95E;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3BB,sizeof(struct Cyc_Absyn_Exp*),2U));}),0U);});
# 2518
mexp=_tmp960;});else{
# 2521
({struct Cyc_Absyn_Exp*_tmp961=Cyc_Absyn_sizeofexp_exp(Cyc_Absyn_deref_exp(_tmp3B5,0U),0U);mexp=_tmp961;});}{
# 2524
struct Cyc_Absyn_Exp*_tmp3BC=Cyc_Toc_get_varsizeexp(_tmp4F4);
if(_tmp3BC != 0)
({struct Cyc_Absyn_Exp*_tmp962=Cyc_Absyn_add_exp(mexp,_tmp3BC,0U);mexp=_tmp962;});
# 2528
if(_tmp4F5 == 0  || Cyc_Absyn_no_regions)
({struct Cyc_Absyn_Exp*_tmp963=Cyc_Toc_malloc_exp(elt_typ,mexp);mexp=_tmp963;});else{
# 2531
struct Cyc_Absyn_Exp*r=_tmp4F5;
Cyc_Toc_exp_to_c(nv,r);
({struct Cyc_Absyn_Exp*_tmp964=Cyc_Toc_rmalloc_exp(r,mexp);mexp=_tmp964;});}{
# 2535
struct Cyc_Absyn_Exp*answer;
if(Cyc_Tcutil_is_dyneither_ptr(old_typ))
({struct Cyc_Absyn_Exp*_tmp969=({struct Cyc_Absyn_Exp*_tmp968=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmp968,({struct Cyc_Absyn_Exp*_tmp3BD[3U];({
struct Cyc_Absyn_Exp*_tmp967=Cyc_Absyn_copy_exp(_tmp3B5);_tmp3BD[0]=_tmp967;}),({
struct Cyc_Absyn_Exp*_tmp966=Cyc_Absyn_sizeoftyp_exp(elt_typ,0U);_tmp3BD[1]=_tmp966;}),
(unsigned int)_tmp3BA?_tmp3BD[2]=_tmp3BA:({struct Cyc_Absyn_Exp*_tmp965=Cyc_Absyn_uint_exp(1U,0U);_tmp3BD[2]=_tmp965;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3BD,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 2537
answer=_tmp969;});else{
# 2542
({struct Cyc_Absyn_Exp*_tmp96A=Cyc_Absyn_copy_exp(_tmp3B5);answer=_tmp96A;});}
({void*_tmp96B=(void*)({struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct*_tmp3BE=_cycalloc(sizeof(*_tmp3BE));_tmp3BE->tag=Cyc_Toc_Dest,_tmp3BE->f1=_tmp3B5;_tmp3BE;});e->annot=_tmp96B;});{
struct Cyc_Absyn_Stmt*_tmp3BF=({struct _tuple1*_tmp96F=_tmp3B4;void*_tmp96E=Cyc_Absyn_cstar_typ(elt_typ,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmp96D=mexp;Cyc_Absyn_declare_stmt(_tmp96F,_tmp96E,_tmp96D,({
struct Cyc_Absyn_Stmt*_tmp96C=Cyc_Absyn_exp_stmt(Cyc_Absyn_copy_exp(e),0U);Cyc_Absyn_seq_stmt(_tmp96C,
Cyc_Absyn_exp_stmt(answer,0U),0U);}),0U);});
# 2548
if((unsigned int)array_len)
({struct Cyc_Absyn_Stmt*_tmp970=Cyc_Absyn_declare_stmt((struct _tuple1*)_check_null(_tmp3B9),Cyc_Absyn_uint_typ,array_len,_tmp3BF,0U);_tmp3BF=_tmp970;});
({void*_tmp971=Cyc_Toc_stmt_exp_r(_tmp3BF);e->r=_tmp971;});
if(_tmp3BC != 0)
({void*_tmp973=({void*_tmp972=Cyc_Toc_typ_to_c(old_typ);Cyc_Toc_cast_it_r(_tmp972,Cyc_Absyn_copy_exp(e));});e->r=_tmp973;});
goto _LL0;};};};};};case 18U: _LL27: _tmp4F6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_LL28: {
# 2557
int _tmp3C0=Cyc_Toc_set_in_sizeof(nv,1);
Cyc_Toc_exp_to_c(nv,_tmp4F6);
Cyc_Toc_set_in_sizeof(nv,_tmp3C0);
goto _LL0;}case 17U: _LL29: _tmp4F7=(void*)((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_LL2A:
({void*_tmp975=(void*)({struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*_tmp3C1=_cycalloc(sizeof(*_tmp3C1));_tmp3C1->tag=17U,({void*_tmp974=Cyc_Toc_typ_to_c(_tmp4F7);_tmp3C1->f1=_tmp974;});_tmp3C1;});e->r=_tmp975;});goto _LL0;case 19U: _LL2B: _tmp4F9=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4F8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL2C: {
# 2563
void*_tmp3C2=_tmp4F9;
struct Cyc_List_List*_tmp3C3=_tmp4F8;
for(0;_tmp3C3 != 0;_tmp3C3=_tmp3C3->tl){
void*_tmp3C4=(void*)_tmp3C3->hd;void*_tmp3C5=_tmp3C4;unsigned int _tmp3D7;struct _dyneither_ptr*_tmp3D6;if(((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3C5)->tag == 0U){_LLC6: _tmp3D6=((struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_tmp3C5)->f1;_LLC7:
 goto _LLC5;}else{_LLC8: _tmp3D7=((struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct*)_tmp3C5)->f1;_LLC9:
# 2569
{void*_tmp3C6=Cyc_Tcutil_compress(_tmp3C2);void*_tmp3C7=_tmp3C6;struct Cyc_Absyn_Datatypefield*_tmp3D5;struct Cyc_List_List*_tmp3D4;struct Cyc_List_List*_tmp3D3;union Cyc_Absyn_AggrInfoU _tmp3D2;switch(*((int*)_tmp3C7)){case 11U: _LLCB: _tmp3D2=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp3C7)->f1).aggr_info;_LLCC: {
# 2571
struct Cyc_Absyn_Aggrdecl*_tmp3C8=Cyc_Absyn_get_known_aggrdecl(_tmp3D2);
if(_tmp3C8->impl == 0)
({void*_tmp3C9=0U;({struct _dyneither_ptr _tmp976=({const char*_tmp3CA="struct fields must be known";_tag_dyneither(_tmp3CA,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp976,_tag_dyneither(_tmp3C9,sizeof(void*),0U));});});
_tmp3D3=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp3C8->impl))->fields;goto _LLCE;}case 12U: _LLCD: _tmp3D3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3C7)->f2;_LLCE: {
# 2576
struct Cyc_Absyn_Aggrfield*_tmp3CB=((struct Cyc_Absyn_Aggrfield*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D3,(int)_tmp3D7);
({void*_tmp977=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->tag=0U,_tmp3CC->f1=_tmp3CB->name;_tmp3CC;}));_tmp3C3->hd=_tmp977;});
_tmp3C2=_tmp3CB->type;
goto _LLCA;}case 10U: _LLCF: _tmp3D4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3C7)->f1;_LLD0:
# 2581
({void*_tmp979=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3CD=_cycalloc(sizeof(*_tmp3CD));_tmp3CD->tag=0U,({struct _dyneither_ptr*_tmp978=Cyc_Absyn_fieldname((int)(_tmp3D7 + 1));_tmp3CD->f1=_tmp978;});_tmp3CD;}));_tmp3C3->hd=_tmp979;});
({void*_tmp97A=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D4,(int)_tmp3D7)).f2;_tmp3C2=_tmp97A;});
goto _LLCA;case 4U: if((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3C7)->f1).field_info).KnownDatatypefield).tag == 2){_LLD1: _tmp3D5=((((((struct Cyc_Absyn_DatatypeFieldType_Absyn_Type_struct*)_tmp3C7)->f1).field_info).KnownDatatypefield).val).f2;_LLD2:
# 2585
 if(_tmp3D7 == 0)
({void*_tmp97B=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->tag=0U,_tmp3CE->f1=Cyc_Toc_tag_sp;_tmp3CE;}));_tmp3C3->hd=_tmp97B;});else{
# 2588
({void*_tmp97C=(*((struct _tuple12*(*)(struct Cyc_List_List*x,int n))Cyc_List_nth)(_tmp3D5->typs,(int)(_tmp3D7 - 1))).f2;_tmp3C2=_tmp97C;});
({void*_tmp97E=(void*)((void*)({struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_tmp3CF=_cycalloc(sizeof(*_tmp3CF));_tmp3CF->tag=0U,({struct _dyneither_ptr*_tmp97D=Cyc_Absyn_fieldname((int)_tmp3D7);_tmp3CF->f1=_tmp97D;});_tmp3CF;}));_tmp3C3->hd=_tmp97E;});}
# 2591
goto _LLCA;}else{goto _LLD3;}default: _LLD3: _LLD4:
({void*_tmp3D0=0U;({struct _dyneither_ptr _tmp97F=({const char*_tmp3D1="impossible type for offsetof tuple index";_tag_dyneither(_tmp3D1,sizeof(char),41U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp97F,_tag_dyneither(_tmp3D0,sizeof(void*),0U));});});
goto _LLCA;}_LLCA:;}
# 2595
goto _LLC5;}_LLC5:;}
# 2598
({void*_tmp981=(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->tag=19U,({void*_tmp980=Cyc_Toc_typ_to_c(_tmp4F9);_tmp3D8->f1=_tmp980;}),_tmp3D8->f2=_tmp4F8;_tmp3D8;});e->r=_tmp981;});
goto _LL0;}case 20U: _LL2D: _tmp4FA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_LL2E: {
# 2601
int _tmp3D9=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp3DA=Cyc_Tcutil_compress((void*)_check_null(_tmp4FA->topt));
{void*_tmp3DB=_tmp3DA;void*_tmp3F0;struct Cyc_Absyn_Tqual _tmp3EF;void*_tmp3EE;union Cyc_Absyn_Constraint*_tmp3ED;union Cyc_Absyn_Constraint*_tmp3EC;union Cyc_Absyn_Constraint*_tmp3EB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DB)->tag == 5U){_LLD6: _tmp3F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DB)->f1).elt_typ;_tmp3EF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DB)->f1).elt_tq;_tmp3EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DB)->f1).ptr_atts).rgn;_tmp3ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DB)->f1).ptr_atts).nullable;_tmp3EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DB)->f1).ptr_atts).bounds;_tmp3EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3DB)->f1).ptr_atts).zero_term;_LLD7:
# 2606
{void*_tmp3DC=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp3EC);void*_tmp3DD=_tmp3DC;struct Cyc_Absyn_Exp*_tmp3E8;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3DD)->tag == 1U){_LLDB: _tmp3E8=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3DD)->f1;_LLDC: {
# 2608
int do_null_check=!Cyc_Toc_in_sizeof(nv) && Cyc_Toc_need_null_check(_tmp4FA);
Cyc_Toc_exp_to_c(nv,_tmp4FA);
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3DE=0U;({unsigned int _tmp983=e->loc;struct _dyneither_ptr _tmp982=({const char*_tmp3DF="inserted null check due to dereference";_tag_dyneither(_tmp3DF,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp983,_tmp982,_tag_dyneither(_tmp3DE,sizeof(void*),0U));});});
# 2614
({void*_tmp987=({void*_tmp986=Cyc_Toc_typ_to_c(_tmp3DA);Cyc_Toc_cast_it_r(_tmp986,({
struct Cyc_Absyn_Exp*_tmp985=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp985,({struct Cyc_List_List*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));
({struct Cyc_Absyn_Exp*_tmp984=Cyc_Absyn_copy_exp(_tmp4FA);_tmp3E0->hd=_tmp984;}),_tmp3E0->tl=0;_tmp3E0;}),0U);}));});
# 2614
_tmp4FA->r=_tmp987;});}
# 2622
if(!Cyc_Toc_in_sizeof(nv) && !((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp3EB)){
struct _tuple13 _tmp3E1=Cyc_Evexp_eval_const_uint_exp(_tmp3E8);struct _tuple13 _tmp3E2=_tmp3E1;unsigned int _tmp3E6;int _tmp3E5;_LLE0: _tmp3E6=_tmp3E2.f1;_tmp3E5=_tmp3E2.f2;_LLE1:;
# 2628
if(!_tmp3E5  || _tmp3E6 <= 0)
({void*_tmp3E3=0U;({unsigned int _tmp989=e->loc;struct _dyneither_ptr _tmp988=({const char*_tmp3E4="cannot determine dereference is in bounds";_tag_dyneither(_tmp3E4,sizeof(char),42U);});Cyc_Tcutil_terr(_tmp989,_tmp988,_tag_dyneither(_tmp3E3,sizeof(void*),0U));});});}
# 2631
goto _LLDA;}}else{_LLDD: _LLDE: {
# 2634
struct Cyc_Absyn_Exp*_tmp3E7=Cyc_Absyn_uint_exp(0U,0U);
_tmp3E7->topt=Cyc_Absyn_uint_typ;
({void*_tmp98A=Cyc_Toc_subscript_exp_r(_tmp4FA,_tmp3E7);e->r=_tmp98A;});
Cyc_Toc_exp_to_c(nv,e);
goto _LLDA;}}_LLDA:;}
# 2640
goto _LLD5;}else{_LLD8: _LLD9:
({void*_tmp3E9=0U;({struct _dyneither_ptr _tmp98B=({const char*_tmp3EA="exp_to_c: Deref: non-pointer";_tag_dyneither(_tmp3EA,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp98B,_tag_dyneither(_tmp3E9,sizeof(void*),0U));});});}_LLD5:;}
# 2643
Cyc_Toc_set_lhs(nv,_tmp3D9);
goto _LL0;};}case 21U: _LL2F: _tmp4FE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp4FD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp4FC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_tmp4FB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp30D)->f4;_LL30: {
# 2646
int is_poly=Cyc_Toc_is_poly_project(e);
void*e1_cyc_type=(void*)_check_null(_tmp4FE->topt);
Cyc_Toc_exp_to_c(nv,_tmp4FE);
if(_tmp4FC  && _tmp4FB)
({void*_tmp990=({struct Cyc_Absyn_Exp*_tmp98F=_tmp4FE;void*_tmp98E=Cyc_Toc_typ_to_c(e1_cyc_type);void*_tmp98D=e1_cyc_type;struct _dyneither_ptr*_tmp98C=_tmp4FD;Cyc_Toc_check_tagged_union(_tmp98F,_tmp98E,_tmp98D,_tmp98C,
Cyc_Toc_in_lhs(nv),Cyc_Toc_member_exp);});
# 2650
e->r=_tmp990;});
# 2654
if(is_poly)
({void*_tmp992=({void*_tmp991=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp991,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp992;});
goto _LL0;}case 22U: _LL31: _tmp502=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp501=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp500=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_tmp4FF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp30D)->f4;_LL32: {
# 2658
int _tmp3F1=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*e1typ=Cyc_Tcutil_compress((void*)_check_null(_tmp502->topt));
int do_null_check=Cyc_Toc_need_null_check(_tmp502);
Cyc_Toc_exp_to_c(nv,_tmp502);{
int is_poly=Cyc_Toc_is_poly_project(e);
struct Cyc_Absyn_PtrInfo _tmp3F2=Cyc_Toc_get_ptr_type(e1typ);struct Cyc_Absyn_PtrInfo _tmp3F3=_tmp3F2;void*_tmp409;struct Cyc_Absyn_Tqual _tmp408;void*_tmp407;union Cyc_Absyn_Constraint*_tmp406;union Cyc_Absyn_Constraint*_tmp405;union Cyc_Absyn_Constraint*_tmp404;_LLE3: _tmp409=_tmp3F3.elt_typ;_tmp408=_tmp3F3.elt_tq;_tmp407=(_tmp3F3.ptr_atts).rgn;_tmp406=(_tmp3F3.ptr_atts).nullable;_tmp405=(_tmp3F3.ptr_atts).bounds;_tmp404=(_tmp3F3.ptr_atts).zero_term;_LLE4:;
{void*_tmp3F4=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp405);void*_tmp3F5=_tmp3F4;struct Cyc_Absyn_Exp*_tmp403;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F5)->tag == 1U){_LLE6: _tmp403=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp3F5)->f1;_LLE7: {
# 2669
struct _tuple13 _tmp3F6=Cyc_Evexp_eval_const_uint_exp(_tmp403);struct _tuple13 _tmp3F7=_tmp3F6;unsigned int _tmp401;int _tmp400;_LLEB: _tmp401=_tmp3F7.f1;_tmp400=_tmp3F7.f2;_LLEC:;
if(_tmp400){
if(_tmp401 < 1)
({void*_tmp3F8=0U;({struct _dyneither_ptr _tmp993=({const char*_tmp3F9="exp_to_c:  AggrArrow_e on pointer of size 0";_tag_dyneither(_tmp3F9,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp993,_tag_dyneither(_tmp3F8,sizeof(void*),0U));});});
if(do_null_check){
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp3FA=0U;({unsigned int _tmp995=e->loc;struct _dyneither_ptr _tmp994=({const char*_tmp3FB="inserted null check due to dereference";_tag_dyneither(_tmp3FB,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp995,_tmp994,_tag_dyneither(_tmp3FA,sizeof(void*),0U));});});
# 2677
({void*_tmp999=({void*_tmp998=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp998,({
struct Cyc_Absyn_Exp*_tmp997=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp997,({struct Cyc_Absyn_Exp*_tmp3FC[1U];({struct Cyc_Absyn_Exp*_tmp996=
Cyc_Absyn_new_exp(_tmp502->r,0U);_tmp3FC[0]=_tmp996;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FC,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2677
_tmp502->r=_tmp999;});}}else{
# 2682
if(!Cyc_Evexp_c_can_eval(_tmp403))
({void*_tmp3FD=0U;({unsigned int _tmp99B=e->loc;struct _dyneither_ptr _tmp99A=({const char*_tmp3FE="cannot determine pointer dereference in bounds";_tag_dyneither(_tmp3FE,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp99B,_tmp99A,_tag_dyneither(_tmp3FD,sizeof(void*),0U));});});
# 2686
({void*_tmp9A1=({void*_tmp9A0=Cyc_Toc_typ_to_c(e1typ);Cyc_Toc_cast_it_r(_tmp9A0,({
struct Cyc_Absyn_Exp*_tmp99F=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp99F,({struct Cyc_Absyn_Exp*_tmp3FF[4U];({
struct Cyc_Absyn_Exp*_tmp99E=Cyc_Absyn_new_exp(_tmp502->r,0U);_tmp3FF[0]=_tmp99E;}),_tmp3FF[1]=_tmp403,({
struct Cyc_Absyn_Exp*_tmp99D=Cyc_Absyn_sizeoftyp_exp(_tmp409,0U);_tmp3FF[2]=_tmp99D;}),({
struct Cyc_Absyn_Exp*_tmp99C=Cyc_Absyn_uint_exp(0U,0U);_tmp3FF[3]=_tmp99C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp3FF,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));});
# 2686
_tmp502->r=_tmp9A1;});}
# 2692
goto _LLE5;}}else{_LLE8: _LLE9: {
# 2695
void*ta1=Cyc_Toc_typ_to_c(_tmp409);
({void*_tmp9A7=({void*_tmp9A6=Cyc_Absyn_cstar_typ(ta1,_tmp408);Cyc_Toc_cast_it_r(_tmp9A6,({
struct Cyc_Absyn_Exp*_tmp9A5=Cyc_Toc__check_dyneither_subscript_e;Cyc_Absyn_fncall_exp(_tmp9A5,({struct Cyc_Absyn_Exp*_tmp402[3U];({
struct Cyc_Absyn_Exp*_tmp9A4=Cyc_Absyn_new_exp(_tmp502->r,0U);_tmp402[0]=_tmp9A4;}),({
struct Cyc_Absyn_Exp*_tmp9A3=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp402[1]=_tmp9A3;}),({
struct Cyc_Absyn_Exp*_tmp9A2=Cyc_Absyn_uint_exp(0U,0U);_tmp402[2]=_tmp9A2;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp402,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));});
# 2696
_tmp502->r=_tmp9A7;});
# 2703
goto _LLE5;}}_LLE5:;}
# 2705
if(_tmp500  && _tmp4FF)
({void*_tmp9AC=({struct Cyc_Absyn_Exp*_tmp9AB=_tmp502;void*_tmp9AA=Cyc_Toc_typ_to_c(e1typ);void*_tmp9A9=_tmp409;struct _dyneither_ptr*_tmp9A8=_tmp501;Cyc_Toc_check_tagged_union(_tmp9AB,_tmp9AA,_tmp9A9,_tmp9A8,Cyc_Toc_in_lhs(nv),Cyc_Absyn_aggrarrow_exp);});e->r=_tmp9AC;});
# 2708
if(is_poly  && _tmp4FF)
({void*_tmp9AE=({void*_tmp9AD=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));Cyc_Toc_array_to_ptr_cast(_tmp9AD,Cyc_Absyn_new_exp(e->r,0U),0U);})->r;e->r=_tmp9AE;});
Cyc_Toc_set_lhs(nv,_tmp3F1);
goto _LL0;};};}case 23U: _LL33: _tmp504=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp503=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL34: {
# 2713
int _tmp40A=Cyc_Toc_in_lhs(nv);
Cyc_Toc_set_lhs(nv,0);{
void*_tmp40B=Cyc_Tcutil_compress((void*)_check_null(_tmp504->topt));
# 2717
{void*_tmp40C=_tmp40B;void*_tmp436;struct Cyc_Absyn_Tqual _tmp435;void*_tmp434;union Cyc_Absyn_Constraint*_tmp433;union Cyc_Absyn_Constraint*_tmp432;union Cyc_Absyn_Constraint*_tmp431;struct Cyc_List_List*_tmp430;switch(*((int*)_tmp40C)){case 10U: _LLEE: _tmp430=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40C)->f1;_LLEF:
# 2720
 Cyc_Toc_exp_to_c(nv,_tmp504);
Cyc_Toc_exp_to_c(nv,_tmp503);{
struct _tuple13 _tmp40D=Cyc_Evexp_eval_const_uint_exp(_tmp503);struct _tuple13 _tmp40E=_tmp40D;unsigned int _tmp412;int _tmp411;_LLF5: _tmp412=_tmp40E.f1;_tmp411=_tmp40E.f2;_LLF6:;
if(!_tmp411)
({void*_tmp40F=0U;({struct _dyneither_ptr _tmp9AF=({const char*_tmp410="unknown tuple subscript in translation to C";_tag_dyneither(_tmp410,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmp9AF,_tag_dyneither(_tmp40F,sizeof(void*),0U));});});
({void*_tmp9B1=({struct Cyc_Absyn_Exp*_tmp9B0=_tmp504;Cyc_Toc_aggrmember_exp_r(_tmp9B0,Cyc_Absyn_fieldname((int)(_tmp412 + 1)));});e->r=_tmp9B1;});
goto _LLED;};case 5U: _LLF0: _tmp436=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).elt_typ;_tmp435=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).elt_tq;_tmp434=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).ptr_atts).rgn;_tmp433=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).ptr_atts).nullable;_tmp432=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).ptr_atts).bounds;_tmp431=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp40C)->f1).ptr_atts).zero_term;_LLF1: {
# 2728
struct Cyc_List_List*_tmp413=Cyc_Toc_get_relns(_tmp504);
int _tmp414=Cyc_Toc_need_null_check(_tmp504);
int _tmp415=Cyc_Toc_in_sizeof(nv);
# 2736
int in_bnds=_tmp415;
{void*_tmp416=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp432);void*_tmp417=_tmp416;_LLF8: _LLF9:
# 2739
({int _tmp9B2=in_bnds  || Cyc_Toc_check_bounds(_tmp40B,_tmp413,_tmp504,_tmp503);in_bnds=_tmp9B2;});
if(Cyc_Toc_warn_bounds_checks  && !in_bnds)
({struct Cyc_String_pa_PrintArg_struct _tmp41A=({struct Cyc_String_pa_PrintArg_struct _tmp794;_tmp794.tag=0U,({struct _dyneither_ptr _tmp9B3=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_exp2string(e));_tmp794.f1=_tmp9B3;});_tmp794;});void*_tmp418[1U];_tmp418[0]=& _tmp41A;({unsigned int _tmp9B5=e->loc;struct _dyneither_ptr _tmp9B4=({const char*_tmp419="bounds check necessary for %s";_tag_dyneither(_tmp419,sizeof(char),30U);});Cyc_Tcutil_warn(_tmp9B5,_tmp9B4,_tag_dyneither(_tmp418,sizeof(void*),1U));});});_LLF7:;}
# 2746
Cyc_Toc_exp_to_c(nv,_tmp504);
Cyc_Toc_exp_to_c(nv,_tmp503);
++ Cyc_Toc_total_bounds_checks;
{void*_tmp41B=((void*(*)(void*y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(Cyc_Absyn_bounds_one,_tmp432);void*_tmp41C=_tmp41B;struct Cyc_Absyn_Exp*_tmp42D;if(((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp41C)->tag == 1U){_LLFB: _tmp42D=((struct Cyc_Absyn_Upper_b_Absyn_Bounds_struct*)_tmp41C)->f1;_LLFC: {
# 2751
int possibly_null=((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp433) && _tmp414;
void*ta1=Cyc_Toc_typ_to_c(_tmp436);
void*ta2=Cyc_Absyn_cstar_typ(ta1,_tmp435);
if(in_bnds  && !possibly_null)
++ Cyc_Toc_bounds_checks_eliminated;else{
if(in_bnds  && possibly_null){
++ Cyc_Toc_bounds_checks_eliminated;
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp41D=0U;({unsigned int _tmp9B7=e->loc;struct _dyneither_ptr _tmp9B6=({const char*_tmp41E="inserted null check due to dereference";_tag_dyneither(_tmp41E,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9B7,_tmp9B6,_tag_dyneither(_tmp41D,sizeof(void*),0U));});});
({void*_tmp9BB=({void*_tmp9BA=ta2;Cyc_Toc_cast_it_r(_tmp9BA,({struct Cyc_Absyn_Exp*_tmp9B9=Cyc_Toc__check_null_e;Cyc_Absyn_fncall_exp(_tmp9B9,({struct Cyc_Absyn_Exp*_tmp41F[1U];({struct Cyc_Absyn_Exp*_tmp9B8=
Cyc_Absyn_copy_exp(_tmp504);_tmp41F[0]=_tmp9B8;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp41F,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}));});
# 2760
_tmp504->r=_tmp9BB;});}else{
# 2763
if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp431)){
# 2765
if(!Cyc_Evexp_c_can_eval(_tmp42D))
({void*_tmp420=0U;({unsigned int _tmp9BD=e->loc;struct _dyneither_ptr _tmp9BC=({const char*_tmp421="cannot determine subscript is in bounds";_tag_dyneither(_tmp421,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9BD,_tmp9BC,_tag_dyneither(_tmp420,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*function_e=Cyc_Toc_getFunctionRemovePointer(& Cyc_Toc__zero_arr_plus_functionSet,_tmp504);
({void*_tmp9C0=Cyc_Toc_deref_exp_r(({void*_tmp9BF=ta2;Cyc_Toc_cast_it(_tmp9BF,({
struct Cyc_Absyn_Exp*_tmp9BE=function_e;Cyc_Absyn_fncall_exp(_tmp9BE,({struct Cyc_Absyn_Exp*_tmp422[3U];_tmp422[0]=_tmp504,_tmp422[1]=_tmp42D,_tmp422[2]=_tmp503;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp422,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2768
e->r=_tmp9C0;});};}else{
# 2771
if(possibly_null){
if(!Cyc_Evexp_c_can_eval(_tmp42D))
({void*_tmp423=0U;({unsigned int _tmp9C2=e->loc;struct _dyneither_ptr _tmp9C1=({const char*_tmp424="cannot determine subscript is in bounds";_tag_dyneither(_tmp424,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9C2,_tmp9C1,_tag_dyneither(_tmp423,sizeof(void*),0U));});});
if(Cyc_Toc_warn_all_null_deref)
({void*_tmp425=0U;({unsigned int _tmp9C4=e->loc;struct _dyneither_ptr _tmp9C3=({const char*_tmp426="inserted null check due to dereference";_tag_dyneither(_tmp426,sizeof(char),39U);});Cyc_Tcutil_warn(_tmp9C4,_tmp9C3,_tag_dyneither(_tmp425,sizeof(void*),0U));});});
# 2777
({void*_tmp9C8=Cyc_Toc_deref_exp_r(({void*_tmp9C7=ta2;Cyc_Toc_cast_it(_tmp9C7,({
struct Cyc_Absyn_Exp*_tmp9C6=Cyc_Toc__check_known_subscript_null_e;Cyc_Absyn_fncall_exp(_tmp9C6,({struct Cyc_Absyn_Exp*_tmp427[4U];_tmp427[0]=_tmp504,_tmp427[1]=_tmp42D,({
# 2780
struct Cyc_Absyn_Exp*_tmp9C5=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp427[2]=_tmp9C5;}),_tmp427[3]=_tmp503;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp427,sizeof(struct Cyc_Absyn_Exp*),4U));}),0U);}));}));
# 2777
e->r=_tmp9C8;});}else{
# 2783
if(!Cyc_Evexp_c_can_eval(_tmp42D))
({void*_tmp428=0U;({unsigned int _tmp9CA=e->loc;struct _dyneither_ptr _tmp9C9=({const char*_tmp429="cannot determine subscript is in bounds";_tag_dyneither(_tmp429,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp9CA,_tmp9C9,_tag_dyneither(_tmp428,sizeof(void*),0U));});});
# 2786
({void*_tmp9CD=({struct Cyc_Absyn_Exp*_tmp9CC=Cyc_Toc__check_known_subscript_notnull_e;Cyc_Toc_fncall_exp_r(_tmp9CC,({struct Cyc_Absyn_Exp*_tmp42A[2U];_tmp42A[0]=_tmp42D,({
struct Cyc_Absyn_Exp*_tmp9CB=Cyc_Absyn_copy_exp(_tmp503);_tmp42A[1]=_tmp9CB;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42A,sizeof(struct Cyc_Absyn_Exp*),2U));}));});
# 2786
_tmp503->r=_tmp9CD;});}}}}
# 2789
goto _LLFA;}}else{_LLFD: _LLFE: {
# 2791
void*ta1=Cyc_Toc_typ_to_c(_tmp436);
if(in_bnds){
# 2795
++ Cyc_Toc_bounds_checks_eliminated;
({void*_tmp9D0=({struct Cyc_Absyn_Exp*_tmp9CF=({void*_tmp9CE=Cyc_Absyn_cstar_typ(ta1,_tmp435);Cyc_Toc_cast_it(_tmp9CE,
Cyc_Toc_member_exp(_tmp504,Cyc_Toc_curr_sp,0U));});
# 2796
Cyc_Toc_subscript_exp_r(_tmp9CF,_tmp503);});e->r=_tmp9D0;});}else{
# 2800
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Toc__check_dyneither_subscript_e;
({void*_tmp9D4=Cyc_Toc_deref_exp_r(({void*_tmp9D3=Cyc_Absyn_cstar_typ(ta1,_tmp435);Cyc_Toc_cast_it(_tmp9D3,({
struct Cyc_Absyn_Exp*_tmp9D2=_tmp42B;Cyc_Absyn_fncall_exp(_tmp9D2,({struct Cyc_Absyn_Exp*_tmp42C[3U];_tmp42C[0]=_tmp504,({
struct Cyc_Absyn_Exp*_tmp9D1=Cyc_Absyn_sizeoftyp_exp(ta1,0U);_tmp42C[1]=_tmp9D1;}),_tmp42C[2]=_tmp503;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp42C,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);}));}));
# 2801
e->r=_tmp9D4;});}
# 2806
goto _LLFA;}}_LLFA:;}
# 2808
goto _LLED;}default: _LLF2: _LLF3:
({void*_tmp42E=0U;({struct _dyneither_ptr _tmp9D5=({const char*_tmp42F="exp_to_c: Subscript on non-tuple/array/tuple ptr";_tag_dyneither(_tmp42F,sizeof(char),49U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9D5,_tag_dyneither(_tmp42E,sizeof(void*),0U));});});}_LLED:;}
# 2811
Cyc_Toc_set_lhs(nv,_tmp40A);
goto _LL0;};}case 24U: _LL35: _tmp505=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_LL36:
# 2814
 if(!Cyc_Toc_is_toplevel(nv)){
# 2816
void*_tmp437=Cyc_Toc_add_tuple_type(((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Toc_tup_to_c,_tmp505));
{void*_tmp438=_tmp437;union Cyc_Absyn_AggrInfoU _tmp43D;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->f1).targs == 0){_LL100: _tmp43D=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp438)->f1).aggr_info;_LL101: {
# 2819
struct Cyc_List_List*dles=0;
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp43D);
{int i=1;for(0;_tmp505 != 0;(_tmp505=_tmp505->tl,++ i)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp505->hd);
({struct Cyc_List_List*_tmp9D8=({struct Cyc_List_List*_tmp439=_cycalloc(sizeof(*_tmp439));({struct _tuple20*_tmp9D7=({struct _dyneither_ptr*_tmp9D6=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmp9D6,(struct Cyc_Absyn_Exp*)_tmp505->hd);});_tmp439->hd=_tmp9D7;}),_tmp439->tl=dles;_tmp439;});dles=_tmp9D8;});}}
# 2825
({void*_tmp9DA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=29U,_tmp43A->f1=sd->name,_tmp43A->f2=0,({struct Cyc_List_List*_tmp9D9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp43A->f3=_tmp9D9;}),_tmp43A->f4=sd;_tmp43A;});e->r=_tmp9DA;});
e->topt=_tmp437;
goto _LLFF;}}else{goto _LL102;}}else{_LL102: _LL103:
({void*_tmp43B=0U;({struct _dyneither_ptr _tmp9DB=({const char*_tmp43C="tuple type not an aggregate";_tag_dyneither(_tmp43C,sizeof(char),28U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9DB,_tag_dyneither(_tmp43B,sizeof(void*),0U));});});}_LLFF:;}
# 2830
goto _LL0;}else{
# 2834
struct Cyc_List_List*dles=0;
for(0;_tmp505 != 0;_tmp505=_tmp505->tl){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_tmp505->hd);
({struct Cyc_List_List*_tmp9DD=({struct Cyc_List_List*_tmp43F=_cycalloc(sizeof(*_tmp43F));({struct _tuple20*_tmp9DC=({struct _tuple20*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E->f1=0,_tmp43E->f2=(struct Cyc_Absyn_Exp*)_tmp505->hd;_tmp43E;});_tmp43F->hd=_tmp9DC;}),_tmp43F->tl=dles;_tmp43F;});dles=_tmp9DD;});}
# 2839
({void*_tmp9DE=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles));e->r=_tmp9DE;});}
# 2841
goto _LL0;case 26U: _LL37: _tmp506=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_LL38:
# 2847
{struct Cyc_List_List*_tmp440=_tmp506;for(0;_tmp440 != 0;_tmp440=_tmp440->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp440->hd)).f2);}}
if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9DF=Cyc_Toc_unresolvedmem_exp_r(0,_tmp506);e->r=_tmp9DF;});
goto _LL0;case 27U: _LL39: _tmp50A=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp509=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp508=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_tmp507=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp30D)->f4;_LL3A:
# 2855
 Cyc_Toc_exp_to_c(nv,_tmp509);
Cyc_Toc_exp_to_c(nv,_tmp508);
if(Cyc_Toc_is_toplevel(nv)){
struct _tuple13 _tmp441=Cyc_Evexp_eval_const_uint_exp(_tmp509);struct _tuple13 _tmp442=_tmp441;unsigned int _tmp44C;int _tmp44B;_LL105: _tmp44C=_tmp442.f1;_tmp44B=_tmp442.f2;_LL106:;{
void*_tmp443=Cyc_Toc_typ_to_c((void*)_check_null(_tmp508->topt));
struct Cyc_List_List*es=0;
# 2862
if(!Cyc_Toc_is_zero(_tmp508)){
if(!_tmp44B)
({void*_tmp444=0U;({unsigned int _tmp9E1=_tmp509->loc;struct _dyneither_ptr _tmp9E0=({const char*_tmp445="cannot determine value of constant";_tag_dyneither(_tmp445,sizeof(char),35U);});Cyc_Tcutil_terr(_tmp9E1,_tmp9E0,_tag_dyneither(_tmp444,sizeof(void*),0U));});});
{unsigned int i=0U;for(0;i < _tmp44C;++ i){
({struct Cyc_List_List*_tmp9E3=({struct Cyc_List_List*_tmp447=_cycalloc(sizeof(*_tmp447));({struct _tuple20*_tmp9E2=({struct _tuple20*_tmp446=_cycalloc(sizeof(*_tmp446));_tmp446->f1=0,_tmp446->f2=_tmp508;_tmp446;});_tmp447->hd=_tmp9E2;}),_tmp447->tl=es;_tmp447;});es=_tmp9E3;});}}
# 2868
if(_tmp507){
struct Cyc_Absyn_Exp*_tmp448=({void*_tmp9E4=_tmp443;Cyc_Toc_cast_it(_tmp9E4,Cyc_Absyn_uint_exp(0U,0U));});
({struct Cyc_List_List*_tmp9E7=({struct Cyc_List_List*_tmp9E6=es;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_imp_append)(_tmp9E6,({struct Cyc_List_List*_tmp44A=_cycalloc(sizeof(*_tmp44A));({struct _tuple20*_tmp9E5=({struct _tuple20*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->f1=0,_tmp449->f2=_tmp448;_tmp449;});_tmp44A->hd=_tmp9E5;}),_tmp44A->tl=0;_tmp44A;}));});es=_tmp9E7;});}}
# 2873
({void*_tmp9E8=Cyc_Toc_unresolvedmem_exp_r(0,es);e->r=_tmp9E8;});};}
# 2875
goto _LL0;case 28U: _LL3B: _tmp50D=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp50C=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp50B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_LL3C:
# 2882
 if(Cyc_Toc_is_toplevel(nv))
({void*_tmp9E9=Cyc_Toc_unresolvedmem_exp_r(0,0);e->r=_tmp9E9;});else{
# 2885
Cyc_Toc_exp_to_c(nv,_tmp50D);}
goto _LL0;case 30U: _LL3D: _tmp50F=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp50E=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL3E:
# 2889
{struct Cyc_List_List*_tmp44D=_tmp50E;for(0;_tmp44D != 0;_tmp44D=_tmp44D->tl){
Cyc_Toc_exp_to_c(nv,(*((struct _tuple20*)_tmp44D->hd)).f2);}}{
void*_tmp44E=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));
e->topt=_tmp44E;
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp44F=Cyc_Tcutil_compress(_tmp44E);void*_tmp450=_tmp44F;union Cyc_Absyn_AggrInfoU _tmp454;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp450)->tag == 11U){if((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp450)->f1).targs == 0){_LL108: _tmp454=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp450)->f1).aggr_info;_LL109: {
# 2896
struct Cyc_Absyn_Aggrdecl*sd=Cyc_Absyn_get_known_aggrdecl(_tmp454);
({void*_tmp9EA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp451=_cycalloc(sizeof(*_tmp451));_tmp451->tag=29U,_tmp451->f1=sd->name,_tmp451->f2=0,_tmp451->f3=_tmp50E,_tmp451->f4=sd;_tmp451;});e->r=_tmp9EA;});
e->topt=_tmp44E;
goto _LL107;}}else{goto _LL10A;}}else{_LL10A: _LL10B:
({void*_tmp452=0U;({struct _dyneither_ptr _tmp9EB=({const char*_tmp453="anonStruct type not an aggregate";_tag_dyneither(_tmp453,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9EB,_tag_dyneither(_tmp452,sizeof(void*),0U));});});}_LL107:;}else{
# 2903
({void*_tmp9EC=Cyc_Toc_unresolvedmem_exp_r(0,_tmp50E);e->r=_tmp9EC;});}
goto _LL0;};case 29U: _LL3F: _tmp513=(struct _tuple1**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp512=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp511=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_tmp510=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp30D)->f4;_LL40:
# 2909
 if(_tmp510 == 0  || _tmp510->impl == 0)
({void*_tmp455=0U;({struct _dyneither_ptr _tmp9ED=({const char*_tmp456="exp_to_c, Aggregate_e: missing aggrdecl pointer or fields";_tag_dyneither(_tmp456,sizeof(char),58U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9ED,_tag_dyneither(_tmp455,sizeof(void*),0U));});});{
void*_tmp457=Cyc_Toc_typ_to_c(old_typ);
{void*_tmp458=Cyc_Tcutil_compress(_tmp457);void*_tmp459=_tmp458;union Cyc_Absyn_AggrInfoU _tmp45C;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp459)->tag == 11U){_LL10D: _tmp45C=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp459)->f1).aggr_info;_LL10E:
({struct _tuple1*_tmp9EE=(Cyc_Absyn_aggr_kinded_name(_tmp45C)).f2;*_tmp513=_tmp9EE;});goto _LL10C;}else{_LL10F: _LL110:
({void*_tmp45A=0U;({struct _dyneither_ptr _tmp9EF=({const char*_tmp45B="exp_to_c, Aggregate_e: bad type translation";_tag_dyneither(_tmp45B,sizeof(char),44U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmp9EF,_tag_dyneither(_tmp45A,sizeof(void*),0U));});});}_LL10C:;}{
# 2920
struct Cyc_List_List*_tmp45D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp510->impl))->fields;
if(_tmp45D == 0)return;
for(0;((struct Cyc_List_List*)_check_null(_tmp45D))->tl != 0;_tmp45D=_tmp45D->tl){
;}{
struct Cyc_Absyn_Aggrfield*_tmp45E=(struct Cyc_Absyn_Aggrfield*)_tmp45D->hd;
struct Cyc_List_List*_tmp45F=((struct Cyc_List_List*(*)(struct _RegionHandle*rgn,unsigned int loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind,struct Cyc_List_List*fields))Cyc_Tcutil_resolve_aggregate_designators)(Cyc_Core_heap_region,e->loc,_tmp511,_tmp510->kind,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp510->impl))->fields);
# 2927
if(!Cyc_Toc_is_toplevel(nv)){
void*_tmp460=Cyc_Tcutil_compress(old_typ);void*_tmp461=_tmp460;struct Cyc_List_List*_tmp48E;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp461)->tag == 11U){_LL112: _tmp48E=(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp461)->f1).targs;_LL113:
# 2931
{struct Cyc_List_List*_tmp462=_tmp45F;for(0;_tmp462 != 0;_tmp462=_tmp462->tl){
struct _tuple34*_tmp463=(struct _tuple34*)_tmp462->hd;struct _tuple34*_tmp464=_tmp463;struct Cyc_Absyn_Aggrfield*_tmp48B;struct Cyc_Absyn_Exp*_tmp48A;_LL117: _tmp48B=_tmp464->f1;_tmp48A=_tmp464->f2;_LL118:;{
void*_tmp465=_tmp48A->topt;
Cyc_Toc_exp_to_c(nv,_tmp48A);
# 2936
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp48B->type) && !
Cyc_Toc_is_pointer_or_boxed_tvar((void*)_check_null(_tmp48A->topt)))
({void*_tmp9F1=({void*_tmp9F0=Cyc_Toc_typ_to_c(_tmp48B->type);Cyc_Toc_cast_it(_tmp9F0,
Cyc_Absyn_copy_exp(_tmp48A));})->r;
# 2938
_tmp48A->r=_tmp9F1;});
# 2941
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp510->impl))->tagged){
struct _dyneither_ptr*_tmp466=_tmp48B->name;
struct Cyc_Absyn_Exp*_tmp467=Cyc_Absyn_uint_exp((unsigned int)Cyc_Toc_get_member_offset(_tmp510,_tmp466),0U);
struct _tuple20*_tmp468=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp467);
struct _tuple20*_tmp469=Cyc_Toc_make_field(Cyc_Toc_val_sp,_tmp48A);
struct _tuple1*s=({struct _tuple1*_tmp474=_cycalloc(sizeof(*_tmp474));({union Cyc_Absyn_Nmspace _tmp9F5=Cyc_Absyn_Abs_n(0,1);_tmp474->f1=_tmp9F5;}),({
struct _dyneither_ptr*_tmp9F4=({struct _dyneither_ptr*_tmp473=_cycalloc(sizeof(*_tmp473));({struct _dyneither_ptr _tmp9F3=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp471=({struct Cyc_String_pa_PrintArg_struct _tmp796;_tmp796.tag=0U,_tmp796.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp510->name).f2);_tmp796;});struct Cyc_String_pa_PrintArg_struct _tmp472=({struct Cyc_String_pa_PrintArg_struct _tmp795;_tmp795.tag=0U,_tmp795.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp466);_tmp795;});void*_tmp46F[2U];_tmp46F[0]=& _tmp471,_tmp46F[1]=& _tmp472;({struct _dyneither_ptr _tmp9F2=({const char*_tmp470="_union_%s_%s";_tag_dyneither(_tmp470,sizeof(char),13U);});Cyc_aprintf(_tmp9F2,_tag_dyneither(_tmp46F,sizeof(void*),2U));});});*_tmp473=_tmp9F3;});_tmp473;});_tmp474->f2=_tmp9F4;});_tmp474;});
# 2949
struct _tuple20*_tmp46A=({
struct _dyneither_ptr*_tmp9F7=_tmp466;Cyc_Toc_make_field(_tmp9F7,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->tag=29U,_tmp46E->f1=s,_tmp46E->f2=0,({struct Cyc_List_List*_tmp9F6=({struct _tuple20*_tmp46D[2U];_tmp46D[0]=_tmp468,_tmp46D[1]=_tmp469;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46D,sizeof(struct _tuple20*),2U));});_tmp46E->f3=_tmp9F6;}),_tmp46E->f4=0;_tmp46E;}),0U));});
# 2952
({void*_tmp9F9=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->tag=29U,_tmp46C->f1=*_tmp513,_tmp46C->f2=0,({struct Cyc_List_List*_tmp9F8=({struct _tuple20*_tmp46B[1U];_tmp46B[0]=_tmp46A;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp46B,sizeof(struct _tuple20*),1U));});_tmp46C->f3=_tmp9F8;}),_tmp46C->f4=_tmp510;_tmp46C;});e->r=_tmp9F9;});}
# 2958
if(Cyc_Toc_is_abstract_type(old_typ) && _tmp45E == _tmp48B){
struct Cyc_List_List*_tmp475=({struct Cyc_List_List*_tmp9FA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp510->impl))->exist_vars,_tmp512);((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp9FA,
((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp510->tvs,_tmp48E));});
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp476=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp510->impl))->fields;for(0;_tmp476 != 0;_tmp476=_tmp476->tl){
struct Cyc_Absyn_Aggrfield*_tmp477=(struct Cyc_Absyn_Aggrfield*)_tmp476->hd;
void*_tmp478=Cyc_Tcutil_substitute(_tmp475,_tmp477->type);
struct Cyc_Absyn_Aggrfield*_tmp479=({struct Cyc_Absyn_Aggrfield*_tmp9FB=_tmp477;Cyc_Toc_aggrfield_to_c(_tmp9FB,
Cyc_Toc_typ_to_c(Cyc_Tcutil_substitute(_tmp475,_tmp478)));});
({struct Cyc_List_List*_tmp9FC=({struct Cyc_List_List*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->hd=_tmp479,_tmp47A->tl=new_fields;_tmp47A;});new_fields=_tmp9FC;});
# 2973
if(_tmp476->tl == 0){
{void*_tmp47B=Cyc_Tcutil_compress(_tmp479->type);void*_tmp47C=_tmp47B;struct Cyc_Absyn_ArrayInfo _tmp481;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47C)->tag == 8U){_LL11A: _tmp481=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp47C)->f1;_LL11B:
# 2976
 if(!Cyc_Evexp_c_can_eval((struct Cyc_Absyn_Exp*)_check_null(_tmp481.num_elts))){
struct Cyc_Absyn_ArrayInfo _tmp47D=_tmp481;
({struct Cyc_Absyn_Exp*_tmp9FD=Cyc_Absyn_uint_exp(0U,0U);_tmp47D.num_elts=_tmp9FD;});
({void*_tmp9FE=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->tag=8U,_tmp47E->f1=_tmp47D;_tmp47E;});_tmp479->type=_tmp9FE;});}
# 2981
goto _LL119;}else{_LL11C: _LL11D:
# 2985
 if(_tmp48A->topt == 0)
goto _LL119;
{void*_tmp47F=Cyc_Tcutil_compress((void*)_check_null(_tmp48A->topt));void*_tmp480=_tmp47F;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp480)->tag == 11U){_LL11F: _LL120:
# 2989
 _tmp479->type=(void*)_check_null(_tmp48A->topt);goto _LL11E;}else{_LL121: _LL122:
 goto _LL11E;}_LL11E:;}
# 2992
goto _LL119;}_LL119:;}
# 2996
if(!Cyc_Toc_is_array_type(_tmp477->type) && 
Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp477->type)))
({struct Cyc_List_List*_tmpA00=({struct Cyc_List_List*_tmp483=_cycalloc(sizeof(*_tmp483));({void*_tmp9FF=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp482=_cycalloc(sizeof(*_tmp482));_tmp482->tag=6U,_tmp482->f1=0;_tmp482;});_tmp483->hd=_tmp9FF;}),_tmp483->tl=_tmp479->attributes;_tmp483;});_tmp479->attributes=_tmpA00;});}}}
# 3002
({struct Cyc_Absyn_Aggrdecl*_tmpA04=({struct _dyneither_ptr*_tmpA03=({struct _dyneither_ptr*_tmp487=_cycalloc(sizeof(*_tmp487));({struct _dyneither_ptr _tmpA02=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp486=({struct Cyc_Int_pa_PrintArg_struct _tmp797;_tmp797.tag=1U,_tmp797.f1=(unsigned long)Cyc_Toc_tuple_type_counter ++;_tmp797;});void*_tmp484[1U];_tmp484[0]=& _tmp486;({struct _dyneither_ptr _tmpA01=({const char*_tmp485="_genStruct%d";_tag_dyneither(_tmp485,sizeof(char),13U);});Cyc_aprintf(_tmpA01,_tag_dyneither(_tmp484,sizeof(void*),1U));});});*_tmp487=_tmpA02;});_tmp487;});Cyc_Toc_make_c_struct_defn(_tmpA03,
# 3004
((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields));});
# 3002
_tmp510=_tmpA04;});
# 3005
*_tmp513=_tmp510->name;
Cyc_Toc_aggrdecl_to_c(_tmp510,1);
# 3008
({void*_tmpA06=(void*)({struct Cyc_Absyn_AggrType_Absyn_Type_struct*_tmp489=_cycalloc(sizeof(*_tmp489));_tmp489->tag=11U,({union Cyc_Absyn_AggrInfoU _tmpA05=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp488=_cycalloc(sizeof(*_tmp488));*_tmp488=_tmp510;_tmp488;}));(_tmp489->f1).aggr_info=_tmpA05;}),(_tmp489->f1).targs=0;_tmp489;});e->topt=_tmpA06;});}};}}
# 3011
goto _LL111;}else{_LL114: _LL115:
({void*_tmp48C=0U;({struct _dyneither_ptr _tmpA07=({const char*_tmp48D="exp_to_c, Aggregate_e: bad struct type";_tag_dyneither(_tmp48D,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA07,_tag_dyneither(_tmp48C,sizeof(void*),0U));});});}_LL111:;}else{
# 3020
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp510->impl))->tagged){
# 3022
struct _tuple34*_tmp48F=(struct _tuple34*)((struct Cyc_List_List*)_check_null(_tmp45F))->hd;struct _tuple34*_tmp490=_tmp48F;struct Cyc_Absyn_Aggrfield*_tmp499;struct Cyc_Absyn_Exp*_tmp498;_LL124: _tmp499=_tmp490->f1;_tmp498=_tmp490->f2;_LL125:;{
void*_tmp491=(void*)_check_null(_tmp498->topt);
void*_tmp492=_tmp499->type;
Cyc_Toc_exp_to_c(nv,_tmp498);
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp492) && !
Cyc_Toc_is_void_star_or_boxed_tvar(_tmp491))
({void*_tmpA09=({
void*_tmpA08=Cyc_Absyn_void_star_typ();Cyc_Toc_cast_it_r(_tmpA08,Cyc_Absyn_new_exp(_tmp498->r,0U));});
# 3028
_tmp498->r=_tmpA09;});{
# 3031
int i=Cyc_Toc_get_member_offset(_tmp510,_tmp499->name);
struct Cyc_Absyn_Exp*field_tag_exp=Cyc_Absyn_signed_int_exp(i,0U);
struct Cyc_List_List*_tmp493=({struct _tuple20*_tmp495[2U];({struct _tuple20*_tmpA0B=({struct _tuple20*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->f1=0,_tmp496->f2=field_tag_exp;_tmp496;});_tmp495[0]=_tmpA0B;}),({struct _tuple20*_tmpA0A=({struct _tuple20*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->f1=0,_tmp497->f2=_tmp498;_tmp497;});_tmp495[1]=_tmpA0A;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp495,sizeof(struct _tuple20*),2U));});
struct Cyc_Absyn_Exp*umem=Cyc_Absyn_unresolvedmem_exp(0,_tmp493,0U);
({void*_tmpA0D=Cyc_Toc_unresolvedmem_exp_r(0,({struct _tuple20*_tmp494[1U];({struct _tuple20*_tmpA0C=Cyc_Toc_make_field(_tmp499->name,umem);_tmp494[0]=_tmpA0C;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp494,sizeof(struct _tuple20*),1U));}));e->r=_tmpA0D;});};};}else{
# 3038
struct Cyc_List_List*_tmp49A=0;
struct Cyc_List_List*_tmp49B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp510->impl))->fields;
for(0;_tmp49B != 0;_tmp49B=_tmp49B->tl){
struct Cyc_List_List*_tmp49C=_tmp45F;for(0;_tmp49C != 0;_tmp49C=_tmp49C->tl){
if((*((struct _tuple34*)_tmp49C->hd)).f1 == (struct Cyc_Absyn_Aggrfield*)_tmp49B->hd){
struct _tuple34*_tmp49D=(struct _tuple34*)_tmp49C->hd;struct _tuple34*_tmp49E=_tmp49D;struct Cyc_Absyn_Aggrfield*_tmp4A4;struct Cyc_Absyn_Exp*_tmp4A3;_LL127: _tmp4A4=_tmp49E->f1;_tmp4A3=_tmp49E->f2;_LL128:;{
void*_tmp49F=Cyc_Toc_typ_to_c(_tmp4A4->type);
void*_tmp4A0=Cyc_Toc_typ_to_c((void*)_check_null(_tmp4A3->topt));
Cyc_Toc_exp_to_c(nv,_tmp4A3);
# 3048
if(!Cyc_Tcutil_unify(_tmp49F,_tmp4A0)){
# 3050
if(!Cyc_Tcutil_is_arithmetic_type(_tmp49F) || !
Cyc_Tcutil_is_arithmetic_type(_tmp4A0))
({struct Cyc_Absyn_Exp*_tmpA0F=({void*_tmpA0E=_tmp49F;Cyc_Toc_cast_it(_tmpA0E,Cyc_Absyn_copy_exp(_tmp4A3));});_tmp4A3=_tmpA0F;});}
({struct Cyc_List_List*_tmpA11=({struct Cyc_List_List*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));({struct _tuple20*_tmpA10=({struct _tuple20*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->f1=0,_tmp4A1->f2=_tmp4A3;_tmp4A1;});_tmp4A2->hd=_tmpA10;}),_tmp4A2->tl=_tmp49A;_tmp4A2;});_tmp49A=_tmpA11;});
break;};}}}
# 3057
({void*_tmpA12=Cyc_Toc_unresolvedmem_exp_r(0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp49A));e->r=_tmpA12;});}}
# 3060
goto _LL0;};};};case 31U: _LL41: _tmp516=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp515=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_tmp514=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp30D)->f3;_LL42: {
# 3064
struct Cyc_List_List*_tmp4A5=_tmp514->typs;
{struct Cyc_List_List*_tmp4A6=_tmp516;for(0;_tmp4A6 != 0;(_tmp4A6=_tmp4A6->tl,_tmp4A5=_tmp4A5->tl)){
struct Cyc_Absyn_Exp*cur_e=(struct Cyc_Absyn_Exp*)_tmp4A6->hd;
void*_tmp4A7=(void*)_check_null(cur_e->topt);
void*field_typ=Cyc_Toc_typ_to_c((*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp4A5))->hd)).f2);
Cyc_Toc_exp_to_c(nv,cur_e);
if(Cyc_Toc_is_void_star_or_boxed_tvar(field_typ) && !
Cyc_Toc_is_pointer_or_boxed_tvar(_tmp4A7))
({void*_tmpA13=(Cyc_Toc_cast_it(field_typ,cur_e))->r;cur_e->r=_tmpA13;});}}{
# 3075
struct Cyc_Absyn_Exp*_tmp4A8=_tmp515->is_extensible?Cyc_Absyn_var_exp(_tmp514->name,0U):
 Cyc_Toc_datatype_tag(_tmp515,_tmp514->name);
# 3078
if(!Cyc_Toc_is_toplevel(nv)){
# 3080
struct Cyc_List_List*dles=0;
{int i=1;for(0;_tmp516 != 0;(_tmp516=_tmp516->tl,++ i)){
({struct Cyc_List_List*_tmpA16=({struct Cyc_List_List*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));({struct _tuple20*_tmpA15=({struct _dyneither_ptr*_tmpA14=Cyc_Absyn_fieldname(i);Cyc_Toc_make_field(_tmpA14,(struct Cyc_Absyn_Exp*)_tmp516->hd);});_tmp4A9->hd=_tmpA15;}),_tmp4A9->tl=dles;_tmp4A9;});dles=_tmpA16;});}}{
# 3086
struct _tuple20*_tmp4AA=Cyc_Toc_make_field(Cyc_Toc_tag_sp,_tmp4A8);
({void*_tmpA1A=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->tag=29U,({struct _tuple1*_tmpA19=Cyc_Toc_collapse_qvars(_tmp514->name,_tmp515->name);_tmp4AC->f1=_tmpA19;}),_tmp4AC->f2=0,({
struct Cyc_List_List*_tmpA18=({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->hd=_tmp4AA,({struct Cyc_List_List*_tmpA17=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp4AB->tl=_tmpA17;});_tmp4AB;});_tmp4AC->f3=_tmpA18;}),_tmp4AC->f4=0;_tmp4AC;});
# 3087
e->r=_tmpA1A;});};}else{
# 3092
struct Cyc_List_List*_tmp4AD=0;
for(0;_tmp516 != 0;_tmp516=_tmp516->tl){
({struct Cyc_List_List*_tmpA1C=({struct Cyc_List_List*_tmp4AF=_cycalloc(sizeof(*_tmp4AF));({struct _tuple20*_tmpA1B=({struct _tuple20*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE->f1=0,_tmp4AE->f2=(struct Cyc_Absyn_Exp*)_tmp516->hd;_tmp4AE;});_tmp4AF->hd=_tmpA1B;}),_tmp4AF->tl=_tmp4AD;_tmp4AF;});_tmp4AD=_tmpA1C;});}
({void*_tmpA1F=Cyc_Toc_unresolvedmem_exp_r(0,({struct Cyc_List_List*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));({struct _tuple20*_tmpA1E=({struct _tuple20*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->f1=0,_tmp4B0->f2=_tmp4A8;_tmp4B0;});_tmp4B1->hd=_tmpA1E;}),({
struct Cyc_List_List*_tmpA1D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp4AD);_tmp4B1->tl=_tmpA1D;});_tmp4B1;}));
# 3095
e->r=_tmpA1F;});}
# 3098
goto _LL0;};}case 32U: _LL43: _LL44:
# 3100
 goto _LL46;case 33U: _LL45: _LL46:
 goto _LL0;case 34U: _LL47: _tmp51C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30D)->f1).is_calloc;_tmp51B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30D)->f1).rgn;_tmp51A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30D)->f1).elt_type;_tmp519=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30D)->f1).num_elts;_tmp518=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30D)->f1).fat_result;_tmp517=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp30D)->f1).inline_call;_LL48: {
# 3104
void*t_c=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp51A)));
Cyc_Toc_exp_to_c(nv,_tmp519);
# 3108
if(_tmp518){
struct _tuple1*_tmp4B2=Cyc_Toc_temp_var();
struct _tuple1*_tmp4B3=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*pexp;struct Cyc_Absyn_Exp*xexp;struct Cyc_Absyn_Exp*rexp;
if(_tmp51C){
xexp=_tmp519;
if(_tmp51B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp51B;
Cyc_Toc_exp_to_c(nv,rgn);
({struct Cyc_Absyn_Exp*_tmpA22=({struct Cyc_Absyn_Exp*_tmpA21=rgn;struct Cyc_Absyn_Exp*_tmpA20=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA21,_tmpA20,Cyc_Absyn_var_exp(_tmp4B2,0U));});pexp=_tmpA22;});}else{
# 3119
({struct Cyc_Absyn_Exp*_tmpA25=({void*_tmpA24=*_tmp51A;struct Cyc_Absyn_Exp*_tmpA23=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA24,_tmpA23,Cyc_Absyn_var_exp(_tmp4B2,0U));});pexp=_tmpA25;});}
# 3121
({struct Cyc_Absyn_Exp*_tmpA2A=({struct Cyc_Absyn_Exp*_tmpA29=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA29,({struct Cyc_Absyn_Exp*_tmp4B4[3U];({
struct Cyc_Absyn_Exp*_tmpA28=Cyc_Absyn_var_exp(_tmp4B3,0U);_tmp4B4[0]=_tmpA28;}),({struct Cyc_Absyn_Exp*_tmpA27=Cyc_Absyn_sizeoftyp_exp(t_c,0U);_tmp4B4[1]=_tmpA27;}),({
struct Cyc_Absyn_Exp*_tmpA26=Cyc_Absyn_var_exp(_tmp4B2,0U);_tmp4B4[2]=_tmpA26;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B4,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3121
rexp=_tmpA2A;});}else{
# 3125
if(_tmp51B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp51B;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp517)
({struct Cyc_Absyn_Exp*_tmpA2C=({struct Cyc_Absyn_Exp*_tmpA2B=rgn;Cyc_Toc_rmalloc_inline_exp(_tmpA2B,Cyc_Absyn_var_exp(_tmp4B2,0U));});pexp=_tmpA2C;});else{
# 3131
({struct Cyc_Absyn_Exp*_tmpA2E=({struct Cyc_Absyn_Exp*_tmpA2D=rgn;Cyc_Toc_rmalloc_exp(_tmpA2D,Cyc_Absyn_var_exp(_tmp4B2,0U));});pexp=_tmpA2E;});}}else{
# 3133
({struct Cyc_Absyn_Exp*_tmpA30=({void*_tmpA2F=*_tmp51A;Cyc_Toc_malloc_exp(_tmpA2F,Cyc_Absyn_var_exp(_tmp4B2,0U));});pexp=_tmpA30;});}
# 3135
({struct Cyc_Absyn_Exp*_tmpA35=({struct Cyc_Absyn_Exp*_tmpA34=Cyc_Toc__tag_dyneither_e;Cyc_Absyn_fncall_exp(_tmpA34,({struct Cyc_Absyn_Exp*_tmp4B5[3U];({struct Cyc_Absyn_Exp*_tmpA33=Cyc_Absyn_var_exp(_tmp4B3,0U);_tmp4B5[0]=_tmpA33;}),({struct Cyc_Absyn_Exp*_tmpA32=Cyc_Absyn_uint_exp(1U,0U);_tmp4B5[1]=_tmpA32;}),({
struct Cyc_Absyn_Exp*_tmpA31=Cyc_Absyn_var_exp(_tmp4B2,0U);_tmp4B5[2]=_tmpA31;});((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp4B5,sizeof(struct Cyc_Absyn_Exp*),3U));}),0U);});
# 3135
rexp=_tmpA35;});}{
# 3138
struct Cyc_Absyn_Stmt*_tmp4B6=({struct _tuple1*_tmpA3B=_tmp4B2;void*_tmpA3A=Cyc_Absyn_uint_typ;struct Cyc_Absyn_Exp*_tmpA39=_tmp519;Cyc_Absyn_declare_stmt(_tmpA3B,_tmpA3A,_tmpA39,({
struct _tuple1*_tmpA38=_tmp4B3;void*_tmpA37=Cyc_Absyn_cstar_typ(t_c,Cyc_Toc_mt_tq);struct Cyc_Absyn_Exp*_tmpA36=pexp;Cyc_Absyn_declare_stmt(_tmpA38,_tmpA37,_tmpA36,
Cyc_Absyn_exp_stmt(rexp,0U),0U);}),0U);});
({void*_tmpA3C=Cyc_Toc_stmt_exp_r(_tmp4B6);e->r=_tmpA3C;});};}else{
# 3143
if(_tmp51C){
if(_tmp51B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp51B;
Cyc_Toc_exp_to_c(nv,rgn);
({void*_tmpA3F=({struct Cyc_Absyn_Exp*_tmpA3E=rgn;struct Cyc_Absyn_Exp*_tmpA3D=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_rcalloc_exp(_tmpA3E,_tmpA3D,_tmp519);})->r;e->r=_tmpA3F;});}else{
# 3149
({void*_tmpA42=({void*_tmpA41=*_tmp51A;struct Cyc_Absyn_Exp*_tmpA40=Cyc_Absyn_sizeoftyp_exp(t_c,0U);Cyc_Toc_calloc_exp(_tmpA41,_tmpA40,_tmp519);})->r;e->r=_tmpA42;});}}else{
# 3152
if(_tmp51B != 0  && !Cyc_Absyn_no_regions){
struct Cyc_Absyn_Exp*rgn=_tmp51B;
Cyc_Toc_exp_to_c(nv,rgn);
if(_tmp517)
({void*_tmpA43=(Cyc_Toc_rmalloc_inline_exp(rgn,_tmp519))->r;e->r=_tmpA43;});else{
# 3158
({void*_tmpA44=(Cyc_Toc_rmalloc_exp(rgn,_tmp519))->r;e->r=_tmpA44;});}}else{
# 3160
({void*_tmpA45=(Cyc_Toc_malloc_exp(*_tmp51A,_tmp519))->r;e->r=_tmpA45;});}}}
# 3164
goto _LL0;}case 35U: _LL49: _tmp51E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp51D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL4A: {
# 3173
void*e1_old_typ=(void*)_check_null(_tmp51E->topt);
void*e2_old_typ=(void*)_check_null(_tmp51D->topt);
if(!Cyc_Tcutil_is_boxed(e1_old_typ) && !Cyc_Tcutil_is_pointer_type(e1_old_typ))
({void*_tmp4B7=0U;({struct _dyneither_ptr _tmpA46=({const char*_tmp4B8="Swap_e: is_pointer_or_boxed: not a pointer or boxed type";_tag_dyneither(_tmp4B8,sizeof(char),57U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA46,_tag_dyneither(_tmp4B7,sizeof(void*),0U));});});{
# 3181
unsigned int _tmp4B9=e->loc;
struct _tuple1*_tmp4BA=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4BB=Cyc_Absyn_var_exp(_tmp4BA,_tmp4B9);_tmp4BB->topt=e1_old_typ;{
struct _tuple1*_tmp4BC=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*_tmp4BD=Cyc_Absyn_var_exp(_tmp4BC,_tmp4B9);_tmp4BD->topt=e2_old_typ;{
# 3187
struct Cyc_Absyn_Exp*_tmp4BE=({struct Cyc_Absyn_Exp*_tmpA48=Cyc_Tcutil_deep_copy_exp(1,_tmp51E);struct Cyc_Absyn_Exp*_tmpA47=_tmp4BD;Cyc_Absyn_assign_exp(_tmpA48,_tmpA47,_tmp4B9);});_tmp4BE->topt=e1_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4BF=Cyc_Absyn_exp_stmt(_tmp4BE,_tmp4B9);
struct Cyc_Absyn_Exp*_tmp4C0=({struct Cyc_Absyn_Exp*_tmpA4A=Cyc_Tcutil_deep_copy_exp(1,_tmp51D);struct Cyc_Absyn_Exp*_tmpA49=_tmp4BB;Cyc_Absyn_assign_exp(_tmpA4A,_tmpA49,_tmp4B9);});_tmp4C0->topt=e2_old_typ;{
struct Cyc_Absyn_Stmt*_tmp4C1=Cyc_Absyn_exp_stmt(_tmp4C0,_tmp4B9);
# 3192
struct Cyc_Absyn_Stmt*_tmp4C2=({struct _tuple1*_tmpA52=_tmp4BA;void*_tmpA51=e1_old_typ;struct Cyc_Absyn_Exp*_tmpA50=_tmp51E;struct Cyc_Absyn_Stmt*_tmpA4F=({
struct _tuple1*_tmpA4E=_tmp4BC;void*_tmpA4D=e2_old_typ;struct Cyc_Absyn_Exp*_tmpA4C=_tmp51D;struct Cyc_Absyn_Stmt*_tmpA4B=
Cyc_Absyn_seq_stmt(_tmp4BF,_tmp4C1,_tmp4B9);
# 3193
Cyc_Absyn_declare_stmt(_tmpA4E,_tmpA4D,_tmpA4C,_tmpA4B,_tmp4B9);});
# 3192
Cyc_Absyn_declare_stmt(_tmpA52,_tmpA51,_tmpA50,_tmpA4F,_tmp4B9);});
# 3195
Cyc_Toc_stmt_to_c(nv,_tmp4C2);
({void*_tmpA53=Cyc_Toc_stmt_exp_r(_tmp4C2);e->r=_tmpA53;});
goto _LL0;};};};};};}case 38U: _LL4B: _tmp520=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_tmp51F=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmp30D)->f2;_LL4C: {
# 3200
void*_tmp4C3=Cyc_Tcutil_compress((void*)_check_null(_tmp520->topt));
Cyc_Toc_exp_to_c(nv,_tmp520);
{void*_tmp4C4=_tmp4C3;struct Cyc_Absyn_Aggrdecl*_tmp4CA;if(((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C4)->tag == 11U){if((((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C4)->f1).aggr_info).KnownAggr).tag == 2){_LL12A: _tmp4CA=*(((((struct Cyc_Absyn_AggrType_Absyn_Type_struct*)_tmp4C4)->f1).aggr_info).KnownAggr).val;_LL12B: {
# 3204
struct Cyc_Absyn_Exp*_tmp4C5=Cyc_Absyn_signed_int_exp(Cyc_Toc_get_member_offset(_tmp4CA,_tmp51F),0U);
struct Cyc_Absyn_Exp*_tmp4C6=Cyc_Toc_member_exp(_tmp520,_tmp51F,0U);
struct Cyc_Absyn_Exp*_tmp4C7=Cyc_Toc_member_exp(_tmp4C6,Cyc_Toc_tag_sp,0U);
({void*_tmpA54=(Cyc_Absyn_eq_exp(_tmp4C7,_tmp4C5,0U))->r;e->r=_tmpA54;});
goto _LL129;}}else{goto _LL12C;}}else{_LL12C: _LL12D:
({void*_tmp4C8=0U;({struct _dyneither_ptr _tmpA55=({const char*_tmp4C9="non-aggregate type in tagcheck";_tag_dyneither(_tmp4C9,sizeof(char),31U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA55,_tag_dyneither(_tmp4C8,sizeof(void*),0U));});});}_LL129:;}
# 3211
goto _LL0;}case 37U: _LL4D: _tmp521=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp30D)->f1;_LL4E:
 Cyc_Toc_stmt_to_c(nv,_tmp521);goto _LL0;case 36U: _LL4F: _LL50:
({void*_tmp4CB=0U;({struct _dyneither_ptr _tmpA56=({const char*_tmp4CC="UnresolvedMem";_tag_dyneither(_tmp4CC,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA56,_tag_dyneither(_tmp4CB,sizeof(void*),0U));});});case 25U: _LL51: _LL52:
({void*_tmp4CD=0U;({struct _dyneither_ptr _tmpA57=({const char*_tmp4CE="compoundlit";_tag_dyneither(_tmp4CE,sizeof(char),12U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_unimp)(_tmpA57,_tag_dyneither(_tmp4CD,sizeof(void*),0U));});});case 39U: _LL53: _LL54:
({void*_tmp4CF=0U;({struct _dyneither_ptr _tmpA58=({const char*_tmp4D0="valueof(-)";_tag_dyneither(_tmp4D0,sizeof(char),11U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA58,_tag_dyneither(_tmp4CF,sizeof(void*),0U));});});default: _LL55: _LL56:
({void*_tmp4D1=0U;({struct _dyneither_ptr _tmpA59=({const char*_tmp4D2="__asm__";_tag_dyneither(_tmp4D2,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA59,_tag_dyneither(_tmp4D1,sizeof(void*),0U));});});}_LL0:;};}struct _tuple35{int f1;struct _dyneither_ptr*f2;struct _dyneither_ptr*f3;struct Cyc_Absyn_Switch_clause*f4;};
# 3244 "toc.cyc"
static struct _tuple35*Cyc_Toc_gen_labels(struct _RegionHandle*r,struct Cyc_Absyn_Switch_clause*sc){
# 3246
return({struct _tuple35*_tmp522=_region_malloc(r,sizeof(*_tmp522));_tmp522->f1=0,({struct _dyneither_ptr*_tmpA5B=Cyc_Toc_fresh_label();_tmp522->f2=_tmpA5B;}),({struct _dyneither_ptr*_tmpA5A=Cyc_Toc_fresh_label();_tmp522->f3=_tmpA5A;}),_tmp522->f4=sc;_tmp522;});}
# 3251
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_path(struct Cyc_List_List*ps,struct Cyc_Absyn_Exp*v){
for(0;ps != 0;ps=((struct Cyc_List_List*)_check_null(ps))->tl){
struct Cyc_Tcpat_PathNode*_tmp523=(struct Cyc_Tcpat_PathNode*)ps->hd;
# 3257
if((int)(((_tmp523->orig_pat).pattern).tag == 1)){
void*t=(void*)_check_null(({union Cyc_Tcpat_PatOrWhere _tmp526=_tmp523->orig_pat;if((_tmp526.pattern).tag != 1)_throw_match();(_tmp526.pattern).val;})->topt);
void*_tmp524=Cyc_Tcutil_compress(Cyc_Toc_typ_to_c_array(t));
void*_tmp525=_tmp524;switch(*((int*)_tmp525)){case 0U: _LL1: _LL2:
# 3262
 goto _LL4;case 11U: _LL3: _LL4:
 goto _LL6;case 12U: _LL5: _LL6:
 goto _LL0;default: _LL7: _LL8:
({struct Cyc_Absyn_Exp*_tmpA5D=({void*_tmpA5C=Cyc_Toc_typ_to_c_array(t);Cyc_Toc_cast_it(_tmpA5C,v);});v=_tmpA5D;});goto _LL0;}_LL0:;}{
# 3268
void*_tmp527=_tmp523->access;void*_tmp528=_tmp527;struct Cyc_Absyn_Datatypedecl*_tmp533;struct Cyc_Absyn_Datatypefield*_tmp532;unsigned int _tmp531;int _tmp530;struct _dyneither_ptr*_tmp52F;unsigned int _tmp52E;switch(*((int*)_tmp528)){case 0U: _LLA: _LLB:
# 3273
 goto _LL9;case 1U: _LLC: _LLD:
# 3278
 if(ps->tl != 0){
void*_tmp529=((struct Cyc_Tcpat_PathNode*)((struct Cyc_List_List*)_check_null(ps->tl))->hd)->access;void*_tmp52A=_tmp529;struct Cyc_Absyn_Datatypedecl*_tmp52D;struct Cyc_Absyn_Datatypefield*_tmp52C;unsigned int _tmp52B;if(((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp52A)->tag == 3U){_LL15: _tmp52D=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp52A)->f1;_tmp52C=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp52A)->f2;_tmp52B=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp52A)->f3;_LL16:
# 3281
 ps=ps->tl;
({struct Cyc_Absyn_Exp*_tmpA60=({void*_tmpA5F=({void*_tmpA5E=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp52C->name,_tmp52D->name));Cyc_Absyn_cstar_typ(_tmpA5E,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA5F,v);});v=_tmpA60;});
({struct Cyc_Absyn_Exp*_tmpA62=({struct Cyc_Absyn_Exp*_tmpA61=v;Cyc_Absyn_aggrarrow_exp(_tmpA61,Cyc_Absyn_fieldname((int)(_tmp52B + 1)),0U);});v=_tmpA62;});
continue;}else{_LL17: _LL18:
# 3286
 goto _LL14;}_LL14:;}
# 3289
({struct Cyc_Absyn_Exp*_tmpA63=Cyc_Absyn_deref_exp(v,0U);v=_tmpA63;});
goto _LL9;case 2U: _LLE: _tmp52E=((struct Cyc_Tcpat_TupleField_Tcpat_Access_struct*)_tmp528)->f1;_LLF:
({struct Cyc_Absyn_Exp*_tmpA65=({struct Cyc_Absyn_Exp*_tmpA64=v;Cyc_Toc_member_exp(_tmpA64,Cyc_Absyn_fieldname((int)(_tmp52E + 1)),0U);});v=_tmpA65;});goto _LL9;case 4U: _LL10: _tmp530=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp528)->f1;_tmp52F=((struct Cyc_Tcpat_AggrField_Tcpat_Access_struct*)_tmp528)->f2;_LL11:
# 3293
({struct Cyc_Absyn_Exp*_tmpA66=Cyc_Toc_member_exp(v,_tmp52F,0U);v=_tmpA66;});
if(_tmp530)
({struct Cyc_Absyn_Exp*_tmpA67=Cyc_Toc_member_exp(v,Cyc_Toc_val_sp,0U);v=_tmpA67;});
goto _LL9;default: _LL12: _tmp533=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp528)->f1;_tmp532=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp528)->f2;_tmp531=((struct Cyc_Tcpat_DatatypeField_Tcpat_Access_struct*)_tmp528)->f3;_LL13:
# 3298
({struct Cyc_Absyn_Exp*_tmpA69=({struct Cyc_Absyn_Exp*_tmpA68=v;Cyc_Toc_member_exp(_tmpA68,Cyc_Absyn_fieldname((int)(_tmp531 + 1)),0U);});v=_tmpA69;});
goto _LL9;}_LL9:;};}
# 3302
return v;}
# 3307
static struct Cyc_Absyn_Exp*Cyc_Toc_compile_pat_test(struct Cyc_Absyn_Exp*v,void*t){
void*_tmp534=t;struct Cyc_Absyn_Datatypedecl*_tmp54D;struct Cyc_Absyn_Datatypefield*_tmp54C;struct _dyneither_ptr*_tmp54B;int _tmp54A;int _tmp549;struct Cyc_Absyn_Datatypedecl*_tmp548;struct Cyc_Absyn_Datatypefield*_tmp547;unsigned int _tmp546;struct _dyneither_ptr _tmp545;int _tmp544;void*_tmp543;struct Cyc_Absyn_Enumfield*_tmp542;struct Cyc_Absyn_Enumdecl*_tmp541;struct Cyc_Absyn_Enumfield*_tmp540;struct Cyc_Absyn_Exp*_tmp53F;switch(*((int*)_tmp534)){case 0U: _LL1: _tmp53F=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp534)->f1;_LL2:
# 3311
 if(_tmp53F == 0)return v;else{return _tmp53F;}case 1U: _LL3: _LL4:
 return({struct Cyc_Absyn_Exp*_tmpA6A=v;Cyc_Absyn_eq_exp(_tmpA6A,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 2U: _LL5: _LL6:
 return({struct Cyc_Absyn_Exp*_tmpA6B=v;Cyc_Absyn_neq_exp(_tmpA6B,Cyc_Absyn_signed_int_exp(0,0U),0U);});case 3U: _LL7: _tmp541=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp534)->f1;_tmp540=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp534)->f2;_LL8:
# 3315
 return({struct Cyc_Absyn_Exp*_tmpA6C=v;Cyc_Absyn_eq_exp(_tmpA6C,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp535=_cycalloc(sizeof(*_tmp535));_tmp535->tag=32U,_tmp535->f1=_tmp541,_tmp535->f2=_tmp540;_tmp535;}),0U),0U);});case 4U: _LL9: _tmp543=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp534)->f1;_tmp542=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp534)->f2;_LLA:
# 3317
 return({struct Cyc_Absyn_Exp*_tmpA6D=v;Cyc_Absyn_eq_exp(_tmpA6D,Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536->tag=33U,_tmp536->f1=_tmp543,_tmp536->f2=_tmp542;_tmp536;}),0U),0U);});case 5U: _LLB: _tmp545=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp534)->f1;_tmp544=((struct Cyc_Tcpat_EqFloat_Tcpat_PatTest_struct*)_tmp534)->f2;_LLC:
 return({struct Cyc_Absyn_Exp*_tmpA6E=v;Cyc_Absyn_eq_exp(_tmpA6E,Cyc_Absyn_float_exp(_tmp545,_tmp544,0U),0U);});case 6U: _LLD: _tmp546=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp534)->f1;_LLE:
 return({struct Cyc_Absyn_Exp*_tmpA6F=v;Cyc_Absyn_eq_exp(_tmpA6F,Cyc_Absyn_signed_int_exp((int)_tmp546,0U),0U);});case 7U: _LLF: _tmp549=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp534)->f1;_tmp548=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp534)->f2;_tmp547=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp534)->f3;_LL10:
# 3323
 LOOP1: {
void*_tmp537=v->r;void*_tmp538=_tmp537;struct Cyc_Absyn_Exp*_tmp53A;struct Cyc_Absyn_Exp*_tmp539;switch(*((int*)_tmp538)){case 14U: _LL16: _tmp539=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp538)->f2;_LL17:
 v=_tmp539;goto LOOP1;case 20U: _LL18: _tmp53A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp538)->f1;_LL19:
 v=_tmp53A;goto _LL15;default: _LL1A: _LL1B:
 goto _LL15;}_LL15:;}
# 3330
({struct Cyc_Absyn_Exp*_tmpA72=({void*_tmpA71=({void*_tmpA70=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp547->name,_tmp548->name));Cyc_Absyn_cstar_typ(_tmpA70,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA71,v);});v=_tmpA72;});
return({struct Cyc_Absyn_Exp*_tmpA73=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA73,Cyc_Absyn_uint_exp((unsigned int)_tmp549,0U),0U);});case 8U: _LL11: _tmp54B=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp534)->f1;_tmp54A=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp534)->f2;_LL12:
# 3335
({struct Cyc_Absyn_Exp*_tmpA75=({struct Cyc_Absyn_Exp*_tmpA74=Cyc_Toc_member_exp(v,_tmp54B,0U);Cyc_Toc_member_exp(_tmpA74,Cyc_Toc_tag_sp,0U);});v=_tmpA75;});
return({struct Cyc_Absyn_Exp*_tmpA76=v;Cyc_Absyn_eq_exp(_tmpA76,Cyc_Absyn_signed_int_exp(_tmp54A,0U),0U);});default: _LL13: _tmp54D=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp534)->f1;_tmp54C=((struct Cyc_Tcpat_EqExtensibleDatatype_Tcpat_PatTest_struct*)_tmp534)->f2;_LL14:
# 3339
 LOOP2: {
void*_tmp53B=v->r;void*_tmp53C=_tmp53B;struct Cyc_Absyn_Exp*_tmp53E;struct Cyc_Absyn_Exp*_tmp53D;switch(*((int*)_tmp53C)){case 14U: _LL1D: _tmp53D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53C)->f2;_LL1E:
 v=_tmp53D;goto LOOP2;case 20U: _LL1F: _tmp53E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp53C)->f1;_LL20:
 v=_tmp53E;goto _LL1C;default: _LL21: _LL22:
 goto _LL1C;}_LL1C:;}
# 3346
({struct Cyc_Absyn_Exp*_tmpA79=({void*_tmpA78=({void*_tmpA77=Cyc_Absyn_strctq(Cyc_Toc_collapse_qvars(_tmp54C->name,_tmp54D->name));Cyc_Absyn_cstar_typ(_tmpA77,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA78,v);});v=_tmpA79;});
return({struct Cyc_Absyn_Exp*_tmpA7A=Cyc_Absyn_aggrarrow_exp(v,Cyc_Toc_tag_sp,0U);Cyc_Absyn_eq_exp(_tmpA7A,Cyc_Absyn_var_exp(_tmp54C->name,0U),0U);});}_LL0:;}struct Cyc_Toc_OtherTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct{int tag;};struct Cyc_Toc_WhereTest_Toc_TestKind_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct{int tag;struct _dyneither_ptr*f1;};
# 3360
struct Cyc_Toc_OtherTest_Toc_TestKind_struct Cyc_Toc_OtherTest_val={0U};
struct Cyc_Toc_DatatypeTagTest_Toc_TestKind_struct Cyc_Toc_DatatypeTagTest_val={1U};struct _tuple36{int f1;void*f2;};
# 3368
static struct _tuple36 Cyc_Toc_admits_switch(struct Cyc_List_List*ss){
# 3370
int c=0;
void*k=(void*)& Cyc_Toc_OtherTest_val;
for(0;ss != 0;(ss=ss->tl,c=c + 1)){
struct _tuple33 _tmp54E=*((struct _tuple33*)ss->hd);struct _tuple33 _tmp54F=_tmp54E;void*_tmp555;_LL1: _tmp555=_tmp54F.f1;_LL2:;{
void*_tmp550=_tmp555;struct Cyc_Absyn_Exp*_tmp554;struct _dyneither_ptr*_tmp553;switch(*((int*)_tmp550)){case 3U: _LL4: _LL5:
# 3376
 goto _LL7;case 4U: _LL6: _LL7:
 goto _LL9;case 6U: _LL8: _LL9:
# 3379
 continue;case 8U: _LLA: _tmp553=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp550)->f1;_LLB:
# 3381
 if(k == (void*)& Cyc_Toc_OtherTest_val)
({void*_tmpA7B=(void*)({struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*_tmp551=_cycalloc(sizeof(*_tmp551));_tmp551->tag=3U,_tmp551->f1=_tmp553;_tmp551;});k=_tmpA7B;});
continue;case 7U: _LLC: _LLD:
# 3385
 k=(void*)& Cyc_Toc_DatatypeTagTest_val;
continue;case 0U: _LLE: _tmp554=((struct Cyc_Tcpat_WhereTest_Tcpat_PatTest_struct*)_tmp550)->f1;if(_tmp554 != 0){_LLF:
# 3389
({void*_tmpA7C=(void*)({struct Cyc_Toc_WhereTest_Toc_TestKind_struct*_tmp552=_cycalloc(sizeof(*_tmp552));_tmp552->tag=2U,_tmp552->f1=_tmp554;_tmp552;});k=_tmpA7C;});
return({struct _tuple36 _tmp798;_tmp798.f1=0,_tmp798.f2=k;_tmp798;});}else{_LL10: _LL11:
 goto _LL13;}case 1U: _LL12: _LL13:
 goto _LL15;case 2U: _LL14: _LL15:
 goto _LL17;case 5U: _LL16: _LL17:
 goto _LL19;default: _LL18: _LL19:
 return({struct _tuple36 _tmp799;_tmp799.f1=0,_tmp799.f2=k;_tmp799;});}_LL3:;};}
# 3398
return({struct _tuple36 _tmp79A;_tmp79A.f1=c,_tmp79A.f2=k;_tmp79A;});}
# 3403
static struct Cyc_Absyn_Pat*Cyc_Toc_compile_pat_test_as_case(void*p){
struct Cyc_Absyn_Exp*e;
{void*_tmp556=p;int _tmp561;int _tmp560;unsigned int _tmp55F;void*_tmp55E;struct Cyc_Absyn_Enumfield*_tmp55D;struct Cyc_Absyn_Enumdecl*_tmp55C;struct Cyc_Absyn_Enumfield*_tmp55B;switch(*((int*)_tmp556)){case 3U: _LL1: _tmp55C=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp556)->f1;_tmp55B=((struct Cyc_Tcpat_EqEnum_Tcpat_PatTest_struct*)_tmp556)->f2;_LL2:
# 3407
({struct Cyc_Absyn_Exp*_tmpA7D=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp557=_cycalloc(sizeof(*_tmp557));_tmp557->tag=32U,_tmp557->f1=_tmp55C,_tmp557->f2=_tmp55B;_tmp557;}),0U);e=_tmpA7D;});goto _LL0;case 4U: _LL3: _tmp55E=(void*)((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp556)->f1;_tmp55D=((struct Cyc_Tcpat_EqAnonEnum_Tcpat_PatTest_struct*)_tmp556)->f2;_LL4:
# 3409
({struct Cyc_Absyn_Exp*_tmpA7E=Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->tag=33U,_tmp558->f1=_tmp55E,_tmp558->f2=_tmp55D;_tmp558;}),0U);e=_tmpA7E;});goto _LL0;case 6U: _LL5: _tmp55F=((struct Cyc_Tcpat_EqConst_Tcpat_PatTest_struct*)_tmp556)->f1;_LL6:
 _tmp560=(int)_tmp55F;goto _LL8;case 7U: _LL7: _tmp560=((struct Cyc_Tcpat_EqDatatypeTag_Tcpat_PatTest_struct*)_tmp556)->f1;_LL8:
 _tmp561=_tmp560;goto _LLA;case 8U: _LL9: _tmp561=((struct Cyc_Tcpat_EqTaggedUnion_Tcpat_PatTest_struct*)_tmp556)->f2;_LLA:
# 3413
({struct Cyc_Absyn_Exp*_tmpA7F=Cyc_Absyn_uint_exp((unsigned int)_tmp561,0U);e=_tmpA7F;});
goto _LL0;default: _LLB: _LLC:
({void*_tmp559=0U;({struct _dyneither_ptr _tmpA80=({const char*_tmp55A="compile_pat_test_as_case!";_tag_dyneither(_tmp55A,sizeof(char),26U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA80,_tag_dyneither(_tmp559,sizeof(void*),0U));});});}_LL0:;}
# 3417
return Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*_tmp562=_cycalloc(sizeof(*_tmp562));_tmp562->tag=17U,_tmp562->f1=e;_tmp562;}),0U);}
# 3421
static struct Cyc_Absyn_Stmt*Cyc_Toc_seq_stmt_opt(struct Cyc_Absyn_Stmt*s1,struct Cyc_Absyn_Stmt*s2){
if(s1 == 0)return s2;
if(s2 == 0)return s1;
return Cyc_Absyn_seq_stmt(s1,s2,0U);}struct _tuple37{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 3430
static struct Cyc_Absyn_Stmt*Cyc_Toc_extract_pattern_vars(struct _RegionHandle*rgn,struct Cyc_Toc_Env**nv,struct Cyc_List_List**decls,struct Cyc_Absyn_Exp*path,struct Cyc_Absyn_Pat*p){
# 3433
void*t=(void*)_check_null(p->topt);
void*_tmp563=p->r;void*_tmp564=_tmp563;union Cyc_Absyn_AggrInfoU _tmp595;struct Cyc_List_List*_tmp594;struct Cyc_List_List*_tmp593;struct Cyc_List_List*_tmp592;struct Cyc_Absyn_Pat*_tmp591;struct Cyc_Absyn_Datatypedecl*_tmp590;struct Cyc_Absyn_Datatypefield*_tmp58F;struct Cyc_List_List*_tmp58E;struct Cyc_Absyn_Vardecl*_tmp58D;struct Cyc_Absyn_Pat*_tmp58C;struct Cyc_Absyn_Vardecl*_tmp58B;struct Cyc_Absyn_Vardecl*_tmp58A;struct Cyc_Absyn_Pat*_tmp589;struct Cyc_Absyn_Vardecl*_tmp588;switch(*((int*)_tmp564)){case 2U: _LL1: _tmp588=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp564)->f2;_LL2: {
# 3436
struct Cyc_Absyn_Pat*_tmp565=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);
_tmp565->topt=p->topt;
_tmp58A=_tmp588;_tmp589=_tmp565;goto _LL4;}case 1U: _LL3: _tmp58A=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp564)->f1;_tmp589=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp564)->f2;_LL4:
# 3441
({struct Cyc_List_List*_tmpA81=({struct Cyc_List_List*_tmp566=_region_malloc(rgn,sizeof(*_tmp566));_tmp566->hd=_tmp58A,_tmp566->tl=*decls;_tmp566;});*decls=_tmpA81;});{
struct Cyc_Absyn_Stmt*_tmp567=({struct Cyc_Absyn_Exp*_tmpA82=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->tag=4U,_tmp568->f1=_tmp58A;_tmp568;}),0U);Cyc_Absyn_assign_stmt(_tmpA82,Cyc_Absyn_copy_exp(path),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA83=_tmp567;Cyc_Toc_seq_stmt_opt(_tmpA83,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp589));});};case 4U: _LL5: _tmp58B=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp564)->f2;_LL6:
# 3446
({struct Cyc_List_List*_tmpA84=({struct Cyc_List_List*_tmp569=_region_malloc(rgn,sizeof(*_tmp569));_tmp569->hd=_tmp58B,_tmp569->tl=*decls;_tmp569;});*decls=_tmpA84;});
return({struct Cyc_Absyn_Exp*_tmpA85=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp56A=_cycalloc(sizeof(*_tmp56A));_tmp56A->tag=4U,_tmp56A->f1=_tmp58B;_tmp56A;}),0U);Cyc_Absyn_assign_stmt(_tmpA85,Cyc_Absyn_copy_exp(path),0U);});case 0U: _LL7: _LL8:
 return 0;case 3U: _LL9: _tmp58D=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp564)->f1;_tmp58C=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp564)->f2;_LLA:
# 3451
({struct Cyc_List_List*_tmpA86=({struct Cyc_List_List*_tmp56B=_region_malloc(rgn,sizeof(*_tmp56B));_tmp56B->hd=_tmp58D,_tmp56B->tl=*decls;_tmp56B;});*decls=_tmpA86;});
({void*_tmpA87=Cyc_Absyn_cstar_typ(t,Cyc_Toc_mt_tq);_tmp58D->type=_tmpA87;});{
# 3454
struct Cyc_Absyn_Stmt*_tmp56C=({struct Cyc_Absyn_Exp*_tmpA8A=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp56D=_cycalloc(sizeof(*_tmp56D));_tmp56D->tag=4U,_tmp56D->f1=_tmp58D;_tmp56D;}),0U);Cyc_Absyn_assign_stmt(_tmpA8A,({
void*_tmpA89=({void*_tmpA88=Cyc_Toc_typ_to_c_array(t);Cyc_Absyn_cstar_typ(_tmpA88,Cyc_Toc_mt_tq);});Cyc_Toc_cast_it(_tmpA89,
Cyc_Toc_push_address_exp(Cyc_Absyn_copy_exp(path)));}),0U);});
return({struct Cyc_Absyn_Stmt*_tmpA8B=_tmp56C;Cyc_Toc_seq_stmt_opt(_tmpA8B,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,path,_tmp58C));});};case 9U: _LLB: _LLC:
# 3459
 goto _LLE;case 10U: _LLD: _LLE:
 goto _LL10;case 11U: _LLF: _LL10:
 goto _LL12;case 12U: _LL11: _LL12:
 goto _LL14;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 return 0;case 6U: if(((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp564)->f1)->r)->tag == 8U){_LL17: _tmp590=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp564)->f1)->r)->f1;_tmp58F=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp564)->f1)->r)->f2;_tmp58E=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)(((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp564)->f1)->r)->f3;_LL18:
# 3468
 if(_tmp58E == 0)return 0;{
struct _tuple1*tufstrct=Cyc_Toc_collapse_qvars(_tmp58F->name,_tmp590->name);
void*_tmp56E=({void*_tmpA8C=Cyc_Absyn_strctq(tufstrct);Cyc_Absyn_cstar_typ(_tmpA8C,Cyc_Toc_mt_tq);});
({struct Cyc_Absyn_Exp*_tmpA8D=Cyc_Toc_cast_it(_tmp56E,path);path=_tmpA8D;});{
int cnt=1;
struct Cyc_List_List*_tmp56F=_tmp58F->typs;
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp58E != 0;(_tmp58E=_tmp58E->tl,_tmp56F=((struct Cyc_List_List*)_check_null(_tmp56F))->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp570=(struct Cyc_Absyn_Pat*)_tmp58E->hd;
if(_tmp570->r == (void*)& Cyc_Absyn_Wild_p_val)continue;{
void*_tmp571=(*((struct _tuple12*)((struct Cyc_List_List*)_check_null(_tmp56F))->hd)).f2;
void*_tmp572=(void*)_check_null(_tmp570->topt);
void*_tmp573=Cyc_Toc_typ_to_c_array(_tmp572);
struct Cyc_Absyn_Exp*_tmp574=({struct Cyc_Absyn_Exp*_tmpA8E=path;Cyc_Absyn_aggrarrow_exp(_tmpA8E,Cyc_Absyn_fieldname(cnt),0U);});
if(Cyc_Toc_is_void_star_or_boxed_tvar(Cyc_Toc_typ_to_c(_tmp571)))
({struct Cyc_Absyn_Exp*_tmpA8F=Cyc_Toc_cast_it(_tmp573,_tmp574);_tmp574=_tmpA8F;});
({struct Cyc_Absyn_Stmt*_tmpA91=({struct Cyc_Absyn_Stmt*_tmpA90=s;Cyc_Toc_seq_stmt_opt(_tmpA90,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp574,_tmp570));});s=_tmpA91;});};}
# 3486
return s;};};}else{_LL21: _tmp591=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp564)->f1;_LL22:
# 3531
 return({struct _RegionHandle*_tmpA95=rgn;struct Cyc_Toc_Env**_tmpA94=nv;struct Cyc_List_List**_tmpA93=decls;struct Cyc_Absyn_Exp*_tmpA92=Cyc_Absyn_deref_exp(path,0U);Cyc_Toc_extract_pattern_vars(_tmpA95,_tmpA94,_tmpA93,_tmpA92,_tmp591);});}case 8U: _LL19: _tmp592=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp564)->f3;_LL1A:
# 3488
 _tmp593=_tmp592;goto _LL1C;case 5U: _LL1B: _tmp593=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp564)->f1;_LL1C: {
# 3490
struct Cyc_Absyn_Stmt*s=0;
int cnt=1;
for(0;_tmp593 != 0;(_tmp593=_tmp593->tl,++ cnt)){
struct Cyc_Absyn_Pat*_tmp575=(struct Cyc_Absyn_Pat*)_tmp593->hd;
if(_tmp575->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
void*_tmp576=(void*)_check_null(_tmp575->topt);
struct _dyneither_ptr*_tmp577=Cyc_Absyn_fieldname(cnt);
({struct Cyc_Absyn_Stmt*_tmpA9B=({struct Cyc_Absyn_Stmt*_tmpA9A=s;Cyc_Toc_seq_stmt_opt(_tmpA9A,({struct _RegionHandle*_tmpA99=rgn;struct Cyc_Toc_Env**_tmpA98=nv;struct Cyc_List_List**_tmpA97=decls;struct Cyc_Absyn_Exp*_tmpA96=Cyc_Toc_member_exp(path,_tmp577,0U);Cyc_Toc_extract_pattern_vars(_tmpA99,_tmpA98,_tmpA97,_tmpA96,_tmp575);}));});s=_tmpA9B;});};}
# 3500
return s;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp564)->f1 == 0){_LL1D: _LL1E:
({void*_tmp578=0U;({struct _dyneither_ptr _tmpA9C=({const char*_tmp579="unresolved aggregate pattern!";_tag_dyneither(_tmp579,sizeof(char),30U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpA9C,_tag_dyneither(_tmp578,sizeof(void*),0U));});});}else{_LL1F: _tmp595=(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp564)->f1)->aggr_info;_tmp594=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp564)->f3;_LL20: {
# 3504
struct Cyc_Absyn_Aggrdecl*_tmp57A=Cyc_Absyn_get_known_aggrdecl(_tmp595);
struct Cyc_Absyn_Stmt*s=0;
for(0;_tmp594 != 0;_tmp594=_tmp594->tl){
struct _tuple37*_tmp57B=(struct _tuple37*)_tmp594->hd;
struct Cyc_Absyn_Pat*_tmp57C=(*_tmp57B).f2;
if(_tmp57C->r == (void*)& Cyc_Absyn_Wild_p_val)
continue;{
struct _dyneither_ptr*_tmp57D=Cyc_Absyn_designatorlist_to_fieldname((*_tmp57B).f1);
void*_tmp57E=(void*)_check_null(_tmp57C->topt);
void*_tmp57F=Cyc_Toc_typ_to_c_array(_tmp57E);
struct Cyc_Absyn_Exp*_tmp580=Cyc_Toc_member_exp(path,_tmp57D,0U);
# 3516
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57A->impl))->tagged)({struct Cyc_Absyn_Exp*_tmpA9D=Cyc_Toc_member_exp(_tmp580,Cyc_Toc_val_sp,0U);_tmp580=_tmpA9D;});{
void*_tmp581=((struct Cyc_Absyn_Aggrfield*)_check_null(Cyc_Absyn_lookup_field(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp57A->impl))->fields,_tmp57D)))->type;
if(Cyc_Toc_is_void_star_or_boxed_tvar(_tmp581))
({struct Cyc_Absyn_Exp*_tmpA9E=Cyc_Toc_cast_it(_tmp57F,_tmp580);_tmp580=_tmpA9E;});else{
if(!Cyc_Toc_is_array_type(_tmp581) && Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp581)))
# 3523
({struct Cyc_Absyn_Exp*_tmpAA0=Cyc_Absyn_deref_exp(({void*_tmpA9F=Cyc_Absyn_cstar_typ(_tmp57F,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpA9F,
Cyc_Absyn_address_exp(_tmp580,0U));}),0U);
# 3523
_tmp580=_tmpAA0;});}
# 3526
({struct Cyc_Absyn_Stmt*_tmpAA2=({struct Cyc_Absyn_Stmt*_tmpAA1=s;Cyc_Toc_seq_stmt_opt(_tmpAA1,Cyc_Toc_extract_pattern_vars(rgn,nv,decls,_tmp580,_tmp57C));});s=_tmpAA2;});};};}
# 3528
return s;}}case 15U: _LL23: _LL24:
# 3533
({void*_tmp582=0U;({struct _dyneither_ptr _tmpAA3=({const char*_tmp583="unknownid pat";_tag_dyneither(_tmp583,sizeof(char),14U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA3,_tag_dyneither(_tmp582,sizeof(void*),0U));});});case 16U: _LL25: _LL26:
({void*_tmp584=0U;({struct _dyneither_ptr _tmpAA4=({const char*_tmp585="unknowncall pat";_tag_dyneither(_tmp585,sizeof(char),16U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA4,_tag_dyneither(_tmp584,sizeof(void*),0U));});});default: _LL27: _LL28:
({void*_tmp586=0U;({struct _dyneither_ptr _tmpAA5=({const char*_tmp587="exp pat";_tag_dyneither(_tmp587,sizeof(char),8U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAA5,_tag_dyneither(_tmp586,sizeof(void*),0U));});});}_LL0:;}struct _tuple38{struct Cyc_Toc_Env*f1;struct _dyneither_ptr*f2;struct Cyc_Absyn_Stmt*f3;};
# 3544
static struct Cyc_Absyn_Stmt*Cyc_Toc_compile_decision_tree(struct _RegionHandle*rgn,struct Cyc_Toc_Env*nv,struct Cyc_List_List**decls,struct Cyc_List_List**bodies,void*dopt,struct Cyc_List_List*lscs,struct _tuple1*v){
# 3552
void*_tmp596=dopt;struct Cyc_List_List*_tmp5D4;struct Cyc_List_List*_tmp5D3;void*_tmp5D2;struct Cyc_Tcpat_Rhs*_tmp5D1;if(_tmp596 == 0){_LL1: _LL2:
# 3554
 return Cyc_Absyn_skip_stmt(0U);}else{switch(*((int*)_tmp596)){case 0U: _LL3: _LL4:
 return Cyc_Toc_throw_match_stmt();case 1U: _LL5: _tmp5D1=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp596)->f1;_LL6:
# 3559
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp597=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp598=_tmp597;int*_tmp59F;struct _dyneither_ptr*_tmp59E;struct _dyneither_ptr*_tmp59D;struct Cyc_Absyn_Switch_clause*_tmp59C;_LLA: _tmp59F=(int*)& _tmp598->f1;_tmp59E=_tmp598->f2;_tmp59D=_tmp598->f3;_tmp59C=_tmp598->f4;_LLB:;{
struct Cyc_Absyn_Stmt*_tmp599=_tmp59C->body;
if(_tmp599 == _tmp5D1->rhs){
# 3564
if(*_tmp59F)
return Cyc_Absyn_goto_stmt(_tmp59E,0U);
*_tmp59F=1;{
# 3568
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpAA8=rgn;struct Cyc_List_List**_tmpAA7=decls;struct Cyc_Absyn_Exp*_tmpAA6=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpAA8,& nv,_tmpAA7,_tmpAA6,_tmp59C->pattern);});
# 3571
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_label_stmt(_tmp59D,_tmp59C->body,0U);
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpAA9=Cyc_Absyn_seq_stmt(init_opt,res,0U);res=_tmpAA9;});
({struct Cyc_Absyn_Stmt*_tmpAAA=Cyc_Absyn_label_stmt(_tmp59E,res,0U);res=_tmpAAA;});
({struct Cyc_List_List*_tmpAAC=({struct Cyc_List_List*_tmp59B=_region_malloc(rgn,sizeof(*_tmp59B));({struct _tuple38*_tmpAAB=({struct _tuple38*_tmp59A=_region_malloc(rgn,sizeof(*_tmp59A));_tmp59A->f1=nv,_tmp59A->f2=_tmp59D,_tmp59A->f3=_tmp599;_tmp59A;});_tmp59B->hd=_tmpAAB;}),_tmp59B->tl=*bodies;_tmp59B;});*bodies=_tmpAAC;});
return res;};}};}
# 3579
({void*_tmp5A0=0U;({struct _dyneither_ptr _tmpAAD=({const char*_tmp5A1="couldn't find rhs!";_tag_dyneither(_tmp5A1,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAAD,_tag_dyneither(_tmp5A0,sizeof(void*),0U));});});default: _LL7: _tmp5D4=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp596)->f1;_tmp5D3=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp596)->f2;_tmp5D2=(void*)((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp596)->f3;_LL8: {
# 3582
struct Cyc_Absyn_Stmt*res=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5D2,lscs,v);
# 3584
struct Cyc_Absyn_Exp*_tmp5A2=({struct Cyc_List_List*_tmpAAE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5D4);Cyc_Toc_compile_path(_tmpAAE,Cyc_Absyn_var_exp(v,0U));});
struct Cyc_List_List*_tmp5A3=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5D3);
# 3587
struct _tuple36 _tmp5A4=Cyc_Toc_admits_switch(_tmp5A3);struct _tuple36 _tmp5A5=_tmp5A4;int _tmp5D0;void*_tmp5CF;_LLD: _tmp5D0=_tmp5A5.f1;_tmp5CF=_tmp5A5.f2;_LLE:;
if(_tmp5D0 > 1){
# 3591
struct Cyc_List_List*new_lscs=({struct Cyc_List_List*_tmp5B6=_cycalloc(sizeof(*_tmp5B6));
({struct Cyc_Absyn_Switch_clause*_tmpAB0=({struct Cyc_Absyn_Switch_clause*_tmp5B5=_cycalloc(sizeof(*_tmp5B5));({struct Cyc_Absyn_Pat*_tmpAAF=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp5B5->pattern=_tmpAAF;}),_tmp5B5->pat_vars=0,_tmp5B5->where_clause=0,_tmp5B5->body=res,_tmp5B5->loc=0U;_tmp5B5;});_tmp5B6->hd=_tmpAB0;}),_tmp5B6->tl=0;_tmp5B6;});
# 3594
for(0;_tmp5A3 != 0;_tmp5A3=_tmp5A3->tl){
struct _tuple33 _tmp5A6=*((struct _tuple33*)_tmp5A3->hd);struct _tuple33 _tmp5A7=_tmp5A6;void*_tmp5AD;void*_tmp5AC;_LL10: _tmp5AD=_tmp5A7.f1;_tmp5AC=_tmp5A7.f2;_LL11:;{
# 3597
struct Cyc_Absyn_Pat*_tmp5A8=Cyc_Toc_compile_pat_test_as_case(_tmp5AD);
# 3599
struct Cyc_Absyn_Stmt*_tmp5A9=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5AC,lscs,v);
# 3601
({struct Cyc_List_List*_tmpAB2=({struct Cyc_List_List*_tmp5AB=_cycalloc(sizeof(*_tmp5AB));({struct Cyc_Absyn_Switch_clause*_tmpAB1=({struct Cyc_Absyn_Switch_clause*_tmp5AA=_cycalloc(sizeof(*_tmp5AA));_tmp5AA->pattern=_tmp5A8,_tmp5AA->pat_vars=0,_tmp5AA->where_clause=0,_tmp5AA->body=_tmp5A9,_tmp5AA->loc=0U;_tmp5AA;});_tmp5AB->hd=_tmpAB1;}),_tmp5AB->tl=new_lscs;_tmp5AB;});new_lscs=_tmpAB2;});};}
# 3603
{void*_tmp5AE=_tmp5CF;struct _dyneither_ptr*_tmp5B3;switch(*((int*)_tmp5AE)){case 1U: _LL13: _LL14:
# 3606
 LOOP1: {
void*_tmp5AF=_tmp5A2->r;void*_tmp5B0=_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B1;switch(*((int*)_tmp5B0)){case 14U: _LL1C: _tmp5B1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5B0)->f2;_LL1D:
 _tmp5A2=_tmp5B1;goto LOOP1;case 20U: _LL1E: _tmp5B2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp5B0)->f1;_LL1F:
 _tmp5A2=_tmp5B2;goto _LL1B;default: _LL20: _LL21:
 goto _LL1B;}_LL1B:;}
# 3612
({struct Cyc_Absyn_Exp*_tmpAB4=Cyc_Absyn_deref_exp(({void*_tmpAB3=Cyc_Absyn_cstar_typ(Cyc_Absyn_sint_typ,Cyc_Toc_mt_tq);Cyc_Toc_cast_it(_tmpAB3,_tmp5A2);}),0U);_tmp5A2=_tmpAB4;});goto _LL12;case 3U: _LL15: _tmp5B3=((struct Cyc_Toc_TaggedUnionTest_Toc_TestKind_struct*)_tmp5AE)->f1;_LL16:
# 3615
({struct Cyc_Absyn_Exp*_tmpAB6=({struct Cyc_Absyn_Exp*_tmpAB5=Cyc_Toc_member_exp(_tmp5A2,_tmp5B3,0U);Cyc_Toc_member_exp(_tmpAB5,Cyc_Toc_tag_sp,0U);});_tmp5A2=_tmpAB6;});goto _LL12;case 2U: _LL17: _LL18:
# 3617
 goto _LL12;default: _LL19: _LL1A:
 goto _LL12;}_LL12:;}
# 3620
({struct Cyc_Absyn_Stmt*_tmpAB7=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp5B4=_cycalloc(sizeof(*_tmp5B4));_tmp5B4->tag=10U,_tmp5B4->f1=_tmp5A2,_tmp5B4->f2=new_lscs,_tmp5B4->f3=0;_tmp5B4;}),0U);res=_tmpAB7;});}else{
# 3624
void*_tmp5B7=_tmp5CF;struct Cyc_Absyn_Exp*_tmp5CE;if(((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5B7)->tag == 2U){_LL23: _tmp5CE=((struct Cyc_Toc_WhereTest_Toc_TestKind_struct*)_tmp5B7)->f1;_LL24: {
# 3626
struct Cyc_List_List*_tmp5B8=_tmp5A3;void*_tmp5C7;struct Cyc_Tcpat_Rhs*_tmp5C6;if(_tmp5B8 != 0){if(((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)((struct Cyc_List_List*)_tmp5B8)->hd)->f2)->tag == 1U){if(((struct Cyc_List_List*)_tmp5B8)->tl == 0){_LL28: _tmp5C7=((struct _tuple33*)_tmp5B8->hd)->f1;_tmp5C6=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)((struct _tuple33*)_tmp5B8->hd)->f2)->f1;_LL29:
# 3632
 for(0;lscs != 0;lscs=lscs->tl){
struct _tuple35*_tmp5B9=(struct _tuple35*)lscs->hd;struct _tuple35*_tmp5BA=_tmp5B9;int*_tmp5C1;struct _dyneither_ptr*_tmp5C0;struct _dyneither_ptr*_tmp5BF;struct Cyc_Absyn_Switch_clause*_tmp5BE;_LL2D: _tmp5C1=(int*)& _tmp5BA->f1;_tmp5C0=_tmp5BA->f2;_tmp5BF=_tmp5BA->f3;_tmp5BE=_tmp5BA->f4;_LL2E:;{
struct Cyc_Absyn_Stmt*_tmp5BB=_tmp5BE->body;
if(_tmp5BB == _tmp5C6->rhs){
# 3637
if(*_tmp5C1)
return Cyc_Absyn_goto_stmt(_tmp5C0,0U);
*_tmp5C1=1;{
# 3641
struct Cyc_Absyn_Stmt*init_opt=({struct _RegionHandle*_tmpABA=rgn;struct Cyc_List_List**_tmpAB9=decls;struct Cyc_Absyn_Exp*_tmpAB8=Cyc_Absyn_var_exp(v,0U);Cyc_Toc_extract_pattern_vars(_tmpABA,& nv,_tmpAB9,_tmpAB8,_tmp5BE->pattern);});
# 3644
Cyc_Toc_exp_to_c(nv,_tmp5CE);{
# 3647
struct Cyc_Absyn_Stmt*r=Cyc_Absyn_label_stmt(_tmp5BF,_tmp5BE->body,0U);
({struct Cyc_Absyn_Stmt*_tmpABB=Cyc_Absyn_ifthenelse_stmt(_tmp5CE,r,res,0U);r=_tmpABB;});
if(init_opt != 0)
({struct Cyc_Absyn_Stmt*_tmpABC=Cyc_Absyn_seq_stmt(init_opt,r,0U);r=_tmpABC;});
({struct Cyc_Absyn_Stmt*_tmpABD=Cyc_Absyn_label_stmt(_tmp5C0,r,0U);r=_tmpABD;});
({struct Cyc_List_List*_tmpABF=({struct Cyc_List_List*_tmp5BD=_region_malloc(rgn,sizeof(*_tmp5BD));({struct _tuple38*_tmpABE=({struct _tuple38*_tmp5BC=_region_malloc(rgn,sizeof(*_tmp5BC));_tmp5BC->f1=nv,_tmp5BC->f2=_tmp5BF,_tmp5BC->f3=_tmp5BB;_tmp5BC;});_tmp5BD->hd=_tmpABE;}),_tmp5BD->tl=*bodies;_tmp5BD;});*bodies=_tmpABF;});
return r;};};}};}
# 3656
({void*_tmp5C2=0U;({struct _dyneither_ptr _tmpAC0=({const char*_tmp5C3="couldn't find rhs!";_tag_dyneither(_tmp5C3,sizeof(char),19U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAC0,_tag_dyneither(_tmp5C2,sizeof(void*),0U));});});}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
({void*_tmp5C4=0U;({struct _dyneither_ptr _tmpAC1=({const char*_tmp5C5="bad where clause in match compiler";_tag_dyneither(_tmp5C5,sizeof(char),35U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAC1,_tag_dyneither(_tmp5C4,sizeof(void*),0U));});});}_LL27:;}}else{_LL25: _LL26:
# 3661
 for(0;_tmp5A3 != 0;_tmp5A3=_tmp5A3->tl){
struct _tuple33 _tmp5C8=*((struct _tuple33*)_tmp5A3->hd);struct _tuple33 _tmp5C9=_tmp5C8;void*_tmp5CD;void*_tmp5CC;_LL30: _tmp5CD=_tmp5C9.f1;_tmp5CC=_tmp5C9.f2;_LL31:;{
struct Cyc_Absyn_Exp*_tmp5CA=Cyc_Toc_compile_pat_test(_tmp5A2,_tmp5CD);
struct Cyc_Absyn_Stmt*_tmp5CB=Cyc_Toc_compile_decision_tree(rgn,nv,decls,bodies,_tmp5CC,lscs,v);
({struct Cyc_Absyn_Stmt*_tmpAC2=Cyc_Absyn_ifthenelse_stmt(_tmp5CA,_tmp5CB,res,0U);res=_tmpAC2;});};}}_LL22:;}
# 3669
return res;}}}_LL0:;}
# 3679
static struct Cyc_Toc_Env**Cyc_Toc_find_case_env(struct Cyc_List_List*bodies,struct Cyc_Absyn_Stmt*s){
# 3681
for(0;bodies != 0;bodies=bodies->tl){
struct _tuple38*_tmp5D5=(struct _tuple38*)bodies->hd;struct _tuple38*_tmp5D6=_tmp5D5;struct Cyc_Toc_Env**_tmp5D8;struct Cyc_Absyn_Stmt*_tmp5D7;_LL1: _tmp5D8=(struct Cyc_Toc_Env**)& _tmp5D6->f1;_tmp5D7=_tmp5D6->f3;_LL2:;
if(_tmp5D7 == s)return _tmp5D8;}
# 3687
return 0;}
# 3691
static void Cyc_Toc_xlate_switch(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*whole_s,struct Cyc_Absyn_Exp*e,struct Cyc_List_List*scs,void*dopt){
# 3694
void*_tmp5D9=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(nv,e);{
# 3697
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
struct _RegionHandle _tmp5DA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5DA;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp5DB=Cyc_Toc_share_env(rgn,nv);
# 3704
struct Cyc_List_List*lscs=
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Toc_gen_labels,rgn,scs);
# 3709
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp5DB,& mydecls,& mybodies,dopt,lscs,v);
# 3717
{struct Cyc_List_List*_tmp5DC=lscs;for(0;_tmp5DC != 0;_tmp5DC=_tmp5DC->tl){
struct _tuple35*_tmp5DD=(struct _tuple35*)_tmp5DC->hd;struct _tuple35*_tmp5DE=_tmp5DD;struct _dyneither_ptr*_tmp5E6;struct Cyc_Absyn_Switch_clause*_tmp5E5;_LL1: _tmp5E6=_tmp5DE->f3;_tmp5E5=_tmp5DE->f4;_LL2:;{
struct Cyc_Absyn_Stmt*s=_tmp5E5->body;
# 3721
struct Cyc_Toc_Env**envp=Cyc_Toc_find_case_env(mybodies,s);
# 3724
if(envp == 0)continue;{
struct Cyc_Toc_Env*_tmp5DF=*envp;
# 3727
if(_tmp5DC->tl != 0){
struct _tuple35*_tmp5E0=(struct _tuple35*)((struct Cyc_List_List*)_check_null(_tmp5DC->tl))->hd;struct _tuple35*_tmp5E1=_tmp5E0;struct _dyneither_ptr*_tmp5E4;struct Cyc_Absyn_Switch_clause*_tmp5E3;_LL4: _tmp5E4=_tmp5E1->f3;_tmp5E3=_tmp5E1->f4;_LL5:;{
# 3731
struct Cyc_Toc_Env**_tmp5E2=Cyc_Toc_find_case_env(mybodies,_tmp5E3->body);
# 3736
if(_tmp5E2 == 0)
({struct Cyc_Toc_Env*_tmpAC3=Cyc_Toc_last_switchclause_env(rgn,_tmp5DF,end_l);Cyc_Toc_stmt_to_c(_tmpAC3,s);});else{
# 3740
struct Cyc_List_List*vs=0;
if(_tmp5E3->pat_vars != 0){
({struct Cyc_List_List*_tmpAC4=((struct Cyc_List_List*(*)(struct Cyc_List_List*l))Cyc_Tcutil_filter_nulls)((((struct _tuple0(*)(struct Cyc_List_List*x))Cyc_List_split)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5E3->pat_vars))->v)).f1);vs=_tmpAC4;});
({struct Cyc_List_List*_tmpAC5=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vs);vs=_tmpAC5;});}
# 3745
({struct Cyc_Toc_Env*_tmpAC6=Cyc_Toc_non_last_switchclause_env(rgn,_tmp5DF,end_l,_tmp5E4,vs);Cyc_Toc_stmt_to_c(_tmpAC6,s);});}};}else{
# 3749
({struct Cyc_Toc_Env*_tmpAC7=Cyc_Toc_last_switchclause_env(rgn,_tmp5DF,end_l);Cyc_Toc_stmt_to_c(_tmpAC7,s);});}};};}}{
# 3753
struct Cyc_Absyn_Stmt*res=({struct Cyc_Absyn_Stmt*_tmpAC9=test_tree;Cyc_Absyn_seq_stmt(_tmpAC9,({struct _dyneither_ptr*_tmpAC8=end_l;Cyc_Absyn_label_stmt(_tmpAC8,Cyc_Toc_skip_stmt_dl(),0U);}),0U);});
# 3755
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp5E7=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpACA=Cyc_Toc_temp_var();_tmp5E7->name=_tmpACA;});
({void*_tmpACB=Cyc_Toc_typ_to_c_array(_tmp5E7->type);_tmp5E7->type=_tmpACB;});
({struct Cyc_Absyn_Stmt*_tmpACD=({struct Cyc_Absyn_Decl*_tmpACC=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->tag=0U,_tmp5E8->f1=_tmp5E7;_tmp5E8;}),0U);Cyc_Absyn_decl_stmt(_tmpACC,res,0U);});res=_tmpACD;});}
# 3762
({void*_tmpAD1=({struct _tuple1*_tmpAD0=v;void*_tmpACF=Cyc_Toc_typ_to_c((void*)_check_null(e->topt));struct Cyc_Absyn_Exp*_tmpACE=e;Cyc_Absyn_declare_stmt(_tmpAD0,_tmpACF,_tmpACE,res,0U);})->r;whole_s->r=_tmpAD1;});
_npop_handler(0U);return;};}
# 3701
;_pop_region(rgn);};}
# 3768
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s);
# 3770
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s);
# 3775
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude);
# 3777
struct Cyc_Absyn_Stmt*Cyc_Toc_make_npop_handler(int n){
return Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAD3=Cyc_Toc__npop_handler_e;Cyc_Absyn_fncall_exp(_tmpAD3,({struct Cyc_List_List*_tmp5E9=_cycalloc(sizeof(*_tmp5E9));
({struct Cyc_Absyn_Exp*_tmpAD2=Cyc_Absyn_uint_exp((unsigned int)(n - 1),0U);_tmp5E9->hd=_tmpAD2;}),_tmp5E9->tl=0;_tmp5E9;}),0U);}),0U);}
# 3781
void Cyc_Toc_do_npop_before(int n,struct Cyc_Absyn_Stmt*s){
if(n > 0)
({void*_tmpAD5=({struct Cyc_Absyn_Stmt*_tmpAD4=Cyc_Toc_make_npop_handler(n);Cyc_Toc_seq_stmt_r(_tmpAD4,Cyc_Absyn_new_stmt(s->r,0U));});s->r=_tmpAD5;});}
# 3786
static void Cyc_Toc_stmt_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Stmt*s){
# 3788
while(1){
void*_tmp5EA=s->r;void*_tmp5EB=_tmp5EA;struct Cyc_Absyn_Stmt*_tmp642;struct Cyc_List_List*_tmp641;void*_tmp640;struct Cyc_Absyn_Stmt*_tmp63F;struct Cyc_Absyn_Exp*_tmp63E;struct _dyneither_ptr*_tmp63D;struct Cyc_Absyn_Stmt*_tmp63C;struct Cyc_Absyn_Decl*_tmp63B;struct Cyc_Absyn_Stmt*_tmp63A;struct Cyc_List_List*_tmp639;struct Cyc_Absyn_Switch_clause**_tmp638;struct Cyc_Absyn_Exp*_tmp637;struct Cyc_List_List*_tmp636;void*_tmp635;struct Cyc_Absyn_Exp*_tmp634;struct Cyc_Absyn_Exp*_tmp633;struct Cyc_Absyn_Exp*_tmp632;struct Cyc_Absyn_Stmt*_tmp631;struct Cyc_Absyn_Exp*_tmp630;struct Cyc_Absyn_Stmt*_tmp62F;struct Cyc_Absyn_Exp*_tmp62E;struct Cyc_Absyn_Stmt*_tmp62D;struct Cyc_Absyn_Stmt*_tmp62C;struct Cyc_Absyn_Exp*_tmp62B;struct Cyc_Absyn_Stmt*_tmp62A;struct Cyc_Absyn_Stmt*_tmp629;struct Cyc_Absyn_Exp*_tmp628;switch(*((int*)_tmp5EB)){case 0U: _LL1: _LL2:
# 3791
 return;case 1U: _LL3: _tmp628=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_LL4:
# 3793
 Cyc_Toc_exp_to_c(nv,_tmp628);
return;case 2U: _LL5: _tmp62A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp629=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LL6:
# 3796
 Cyc_Toc_stmt_to_c(nv,_tmp62A);
s=_tmp629;
continue;case 3U: _LL7: _tmp62B=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_LL8: {
# 3800
void*topt=0;
if(_tmp62B != 0){
({void*_tmpAD6=Cyc_Toc_typ_to_c((void*)_check_null(_tmp62B->topt));topt=_tmpAD6;});
Cyc_Toc_exp_to_c(nv,_tmp62B);}{
# 3806
int _tmp5EC=Cyc_Toc_get_npop(s);
if(_tmp5EC > 0){
if(topt != 0){
struct _tuple1*_tmp5ED=Cyc_Toc_temp_var();
struct Cyc_Absyn_Stmt*_tmp5EE=Cyc_Absyn_return_stmt(Cyc_Absyn_var_exp(_tmp5ED,0U),0U);
({void*_tmpADB=({struct _tuple1*_tmpADA=_tmp5ED;void*_tmpAD9=topt;struct Cyc_Absyn_Exp*_tmpAD8=_tmp62B;Cyc_Absyn_declare_stmt(_tmpADA,_tmpAD9,_tmpAD8,({
struct Cyc_Absyn_Stmt*_tmpAD7=Cyc_Toc_make_npop_handler(_tmp5EC);Cyc_Absyn_seq_stmt(_tmpAD7,_tmp5EE,0U);}),0U);})->r;
# 3811
s->r=_tmpADB;});}else{
# 3815
Cyc_Toc_do_npop_before(_tmp5EC,s);}}
# 3817
return;};}case 4U: _LL9: _tmp62E=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp62D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_tmp62C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f3;_LLA:
# 3819
 Cyc_Toc_exp_to_c(nv,_tmp62E);
Cyc_Toc_stmt_to_c(nv,_tmp62D);
s=_tmp62C;
continue;case 5U: _LLB: _tmp630=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1).f1;_tmp62F=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LLC:
# 3824
 Cyc_Toc_exp_to_c(nv,_tmp630);{
struct _RegionHandle _tmp5EF=_new_region("temp");struct _RegionHandle*temp=& _tmp5EF;_push_region(temp);
({struct Cyc_Toc_Env*_tmpADC=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpADC,_tmp62F);});
# 3828
_npop_handler(0U);return;
# 3825
;_pop_region(temp);};case 6U: _LLD: _LLE: {
# 3830
struct Cyc_Toc_Env*_tmp5F0=nv;struct _dyneither_ptr**_tmp5F1;_LL22: _tmp5F1=_tmp5F0->break_lab;_LL23:;
if(_tmp5F1 != 0)
({void*_tmpADD=Cyc_Toc_goto_stmt_r(*_tmp5F1);s->r=_tmpADD;});
# 3834
({int _tmpADE=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpADE,s);});
return;}case 7U: _LLF: _LL10: {
# 3837
struct Cyc_Toc_Env*_tmp5F2=nv;struct _dyneither_ptr**_tmp5F3;_LL25: _tmp5F3=_tmp5F2->continue_lab;_LL26:;
if(_tmp5F3 != 0)
({void*_tmpADF=Cyc_Toc_goto_stmt_r(*_tmp5F3);s->r=_tmpADF;});
goto _LL12;}case 8U: _LL11: _LL12:
# 3843
({int _tmpAE0=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAE0,s);});
return;case 9U: _LL13: _tmp634=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp633=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2).f1;_tmp632=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f3).f1;_tmp631=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f4;_LL14:
# 3847
 Cyc_Toc_exp_to_c(nv,_tmp634);Cyc_Toc_exp_to_c(nv,_tmp633);Cyc_Toc_exp_to_c(nv,_tmp632);{
struct _RegionHandle _tmp5F4=_new_region("temp");struct _RegionHandle*temp=& _tmp5F4;_push_region(temp);
({struct Cyc_Toc_Env*_tmpAE1=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpAE1,_tmp631);});
# 3851
_npop_handler(0U);return;
# 3848
;_pop_region(temp);};case 10U: _LL15: _tmp637=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp636=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_tmp635=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f3;_LL16:
# 3853
 Cyc_Toc_xlate_switch(nv,s,_tmp637,_tmp636,_tmp635);
return;case 11U: _LL17: _tmp639=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp638=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LL18: {
# 3856
struct Cyc_Toc_Env*_tmp5F5=nv;struct Cyc_Toc_FallthruInfo*_tmp5FF;_LL28: _tmp5FF=_tmp5F5->fallthru_info;_LL29:;
if(_tmp5FF == 0)
({void*_tmp5F6=0U;({struct _dyneither_ptr _tmpAE2=({const char*_tmp5F7="fallthru in unexpected place";_tag_dyneither(_tmp5F7,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpAE2,_tag_dyneither(_tmp5F6,sizeof(void*),0U));});});{
struct Cyc_Toc_FallthruInfo _tmp5F8=*_tmp5FF;struct Cyc_Toc_FallthruInfo _tmp5F9=_tmp5F8;struct _dyneither_ptr*_tmp5FE;struct Cyc_List_List*_tmp5FD;_LL2B: _tmp5FE=_tmp5F9.label;_tmp5FD=_tmp5F9.binders;_LL2C:;{
struct Cyc_Absyn_Stmt*s2=Cyc_Absyn_goto_stmt(_tmp5FE,0U);
# 3862
({int _tmpAE3=Cyc_Toc_get_npop(s);Cyc_Toc_do_npop_before(_tmpAE3,s2);});{
struct Cyc_List_List*_tmp5FA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp5FD);
struct Cyc_List_List*_tmp5FB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp639);
for(0;_tmp5FA != 0;(_tmp5FA=_tmp5FA->tl,_tmp5FB=_tmp5FB->tl)){
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp5FB))->hd);
({struct Cyc_Absyn_Stmt*_tmpAE6=({struct Cyc_Absyn_Stmt*_tmpAE5=({struct Cyc_Absyn_Exp*_tmpAE4=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->tag=5U,_tmp5FC->f1=(struct Cyc_Absyn_Vardecl*)_tmp5FA->hd;_tmp5FC;}),0U);Cyc_Absyn_assign_stmt(_tmpAE4,(struct Cyc_Absyn_Exp*)_tmp5FB->hd,0U);});Cyc_Absyn_seq_stmt(_tmpAE5,s2,0U);});s2=_tmpAE6;});}
# 3870
s->r=s2->r;
return;};};};}case 12U: _LL19: _tmp63B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp63A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LL1A:
# 3876
{void*_tmp600=_tmp63B->r;void*_tmp601=_tmp600;struct Cyc_Absyn_Tvar*_tmp61C;struct Cyc_Absyn_Vardecl*_tmp61B;struct Cyc_Absyn_Exp*_tmp61A;struct Cyc_Absyn_Fndecl*_tmp619;struct Cyc_List_List*_tmp618;struct Cyc_Absyn_Pat*_tmp617;struct Cyc_Absyn_Exp*_tmp616;void*_tmp615;struct Cyc_Absyn_Vardecl*_tmp614;switch(*((int*)_tmp601)){case 0U: _LL2E: _tmp614=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp601)->f1;_LL2F:
 Cyc_Toc_local_decl_to_c(nv,_tmp614,_tmp63A);goto _LL2D;case 2U: _LL30: _tmp617=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp601)->f1;_tmp616=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp601)->f3;_tmp615=(void*)((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp601)->f4;_LL31:
# 3884
{void*_tmp602=_tmp617->r;void*_tmp603=_tmp602;struct Cyc_Absyn_Vardecl*_tmp605;if(((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp603)->tag == 1U){if(((struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)((struct Cyc_Absyn_Pat*)((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp603)->f2)->r)->tag == 0U){_LL3B: _tmp605=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp603)->f1;_LL3C:
# 3886
({struct _tuple1*_tmpAE7=Cyc_Toc_temp_var();_tmp605->name=_tmpAE7;});
_tmp605->initializer=_tmp616;
({void*_tmpAE8=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->tag=0U,_tmp604->f1=_tmp605;_tmp604;});_tmp63B->r=_tmpAE8;});
Cyc_Toc_local_decl_to_c(nv,_tmp605,_tmp63A);
goto _LL3A;}else{goto _LL3D;}}else{_LL3D: _LL3E:
# 3895
({void*_tmpAE9=(Cyc_Toc_letdecl_to_c(nv,_tmp617,_tmp615,(void*)_check_null(_tmp616->topt),_tmp616,_tmp63A))->r;s->r=_tmpAE9;});
goto _LL3A;}_LL3A:;}
# 3898
goto _LL2D;case 3U: _LL32: _tmp618=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp601)->f1;_LL33: {
# 3902
struct Cyc_List_List*_tmp606=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(_tmp618);
if(_tmp606 == 0)
({void*_tmp607=0U;({struct _dyneither_ptr _tmpAEA=({const char*_tmp608="empty Letv_d";_tag_dyneither(_tmp608,sizeof(char),13U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpAEA,_tag_dyneither(_tmp607,sizeof(void*),0U));});});
({void*_tmpAEB=(void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->tag=0U,_tmp609->f1=(struct Cyc_Absyn_Vardecl*)_tmp606->hd;_tmp609;});_tmp63B->r=_tmpAEB;});
_tmp606=_tmp606->tl;
for(0;_tmp606 != 0;_tmp606=_tmp606->tl){
struct Cyc_Absyn_Decl*_tmp60A=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp60B=_cycalloc(sizeof(*_tmp60B));_tmp60B->tag=0U,_tmp60B->f1=(struct Cyc_Absyn_Vardecl*)_tmp606->hd;_tmp60B;}),0U);
({void*_tmpAED=({struct Cyc_Absyn_Decl*_tmpAEC=_tmp60A;Cyc_Absyn_decl_stmt(_tmpAEC,Cyc_Absyn_new_stmt(s->r,0U),0U);})->r;s->r=_tmpAED;});}
# 3911
Cyc_Toc_stmt_to_c(nv,s);
goto _LL2D;}case 1U: _LL34: _tmp619=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp601)->f1;_LL35:
# 3914
 Cyc_Toc_fndecl_to_c(nv,_tmp619,0);
Cyc_Toc_stmt_to_c(nv,_tmp63A);
goto _LL2D;case 4U: _LL36: _tmp61C=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp601)->f1;_tmp61B=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp601)->f2;_tmp61A=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp601)->f3;_LL37: {
# 3918
struct Cyc_Absyn_Stmt*_tmp60C=_tmp63A;
# 3926
void*rh_struct_typ=Cyc_Absyn_strct(Cyc_Toc__RegionHandle_sp);
void*rh_struct_ptr_typ=Cyc_Absyn_cstar_typ(rh_struct_typ,Cyc_Toc_mt_tq);
struct _tuple1*rh_var=Cyc_Toc_temp_var();
struct _tuple1*x_var=_tmp61B->name;
struct Cyc_Absyn_Exp*rh_exp=Cyc_Absyn_var_exp(rh_var,0U);
struct Cyc_Absyn_Exp*x_exp=Cyc_Absyn_var_exp(x_var,0U);
# 3933
Cyc_Toc_stmt_to_c(nv,_tmp60C);
if(Cyc_Absyn_no_regions)
({void*_tmpAF1=({struct _tuple1*_tmpAF0=x_var;void*_tmpAEF=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAEE=
Cyc_Absyn_uint_exp(0U,0U);
# 3935
Cyc_Absyn_declare_stmt(_tmpAF0,_tmpAEF,_tmpAEE,_tmp60C,0U);})->r;s->r=_tmpAF1;});else{
# 3937
if((unsigned int)_tmp61A){
Cyc_Toc_exp_to_c(nv,_tmp61A);{
struct Cyc_Absyn_Exp*arg=Cyc_Absyn_address_exp(({struct Cyc_Absyn_Exp*_tmpAF3=_tmp61A;Cyc_Absyn_aggrarrow_exp(_tmpAF3,({struct _dyneither_ptr*_tmp60E=_cycalloc(sizeof(*_tmp60E));({struct _dyneither_ptr _tmpAF2=({const char*_tmp60D="h";_tag_dyneither(_tmp60D,sizeof(char),2U);});*_tmp60E=_tmpAF2;});_tmp60E;}),0U);}),0U);
({void*_tmpAF4=(Cyc_Absyn_declare_stmt(x_var,rh_struct_ptr_typ,arg,_tmp60C,0U))->r;s->r=_tmpAF4;});};}else{
# 3942
({void*_tmpB01=({
struct _tuple1*_tmpB00=rh_var;void*_tmpAFF=rh_struct_typ;struct Cyc_Absyn_Exp*_tmpAFE=({
struct Cyc_Absyn_Exp*_tmpAF6=Cyc_Toc__new_region_e;Cyc_Absyn_fncall_exp(_tmpAF6,({struct Cyc_List_List*_tmp60F=_cycalloc(sizeof(*_tmp60F));
({struct Cyc_Absyn_Exp*_tmpAF5=Cyc_Absyn_string_exp(Cyc_Absynpp_qvar2string(x_var),0U);_tmp60F->hd=_tmpAF5;}),_tmp60F->tl=0;_tmp60F;}),0U);});
# 3943
Cyc_Absyn_declare_stmt(_tmpB00,_tmpAFF,_tmpAFE,({
# 3947
struct _tuple1*_tmpAFD=x_var;void*_tmpAFC=rh_struct_ptr_typ;struct Cyc_Absyn_Exp*_tmpAFB=Cyc_Absyn_address_exp(rh_exp,0U);Cyc_Absyn_declare_stmt(_tmpAFD,_tmpAFC,_tmpAFB,({
struct Cyc_Absyn_Stmt*_tmpAFA=Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAF7=Cyc_Toc__push_region_e;Cyc_Absyn_fncall_exp(_tmpAF7,({struct Cyc_Absyn_Exp*_tmp610[1U];_tmp610[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp610,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U);Cyc_Absyn_seq_stmt(_tmpAFA,({
struct Cyc_Absyn_Stmt*_tmpAF9=_tmp60C;Cyc_Absyn_seq_stmt(_tmpAF9,
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpAF8=Cyc_Toc__pop_region_e;Cyc_Absyn_fncall_exp(_tmpAF8,({struct Cyc_Absyn_Exp*_tmp611[1U];_tmp611[0]=x_exp;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp611,sizeof(struct Cyc_Absyn_Exp*),1U));}),0U);}),0U),0U);}),0U);}),0U);}),0U);})->r;
# 3942
s->r=_tmpB01;});}}
# 3953
return;}default: _LL38: _LL39:
({void*_tmp612=0U;({struct _dyneither_ptr _tmpB02=({const char*_tmp613="bad nested declaration within function";_tag_dyneither(_tmp613,sizeof(char),39U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB02,_tag_dyneither(_tmp612,sizeof(void*),0U));});});}_LL2D:;}
# 3956
return;case 13U: _LL1B: _tmp63D=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp63C=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_LL1C:
# 3958
 s=_tmp63C;continue;case 14U: _LL1D: _tmp63F=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp63E=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2).f1;_LL1E: {
# 3960
struct _RegionHandle _tmp61D=_new_region("temp");struct _RegionHandle*temp=& _tmp61D;_push_region(temp);
({struct Cyc_Toc_Env*_tmpB03=Cyc_Toc_loop_env(temp,nv);Cyc_Toc_stmt_to_c(_tmpB03,_tmp63F);});
Cyc_Toc_exp_to_c(nv,_tmp63E);
# 3964
_npop_handler(0U);return;
# 3960
;_pop_region(temp);}default: _LL1F: _tmp642=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f1;_tmp641=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f2;_tmp640=(void*)((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp5EB)->f3;_LL20: {
# 3978 "toc.cyc"
struct _tuple1*h_var=Cyc_Toc_temp_var();
struct _tuple1*e_var=Cyc_Toc_temp_var();
struct _tuple1*was_thrown_var=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*h_exp=Cyc_Absyn_var_exp(h_var,0U);
struct Cyc_Absyn_Exp*e_exp=Cyc_Absyn_var_exp(e_var,0U);
struct Cyc_Absyn_Exp*was_thrown_exp=Cyc_Absyn_var_exp(was_thrown_var,0U);
void*h_typ=Cyc_Absyn_strct(Cyc_Toc__handler_cons_sp);
void*e_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_exn_typ());
void*was_thrown_typ=Cyc_Toc_typ_to_c(Cyc_Absyn_sint_typ);
# 3988
e_exp->topt=e_typ;{
struct _RegionHandle _tmp61E=_new_region("temp");struct _RegionHandle*temp=& _tmp61E;_push_region(temp);
# 3991
Cyc_Toc_stmt_to_c(nv,_tmp642);{
struct Cyc_Absyn_Stmt*_tmp61F=({struct Cyc_Absyn_Stmt*_tmpB04=_tmp642;Cyc_Absyn_seq_stmt(_tmpB04,
Cyc_Absyn_exp_stmt(Cyc_Absyn_fncall_exp(Cyc_Toc__pop_handler_e,0,0U),0U),0U);});
# 3996
struct Cyc_Absyn_Stmt*_tmp620=Cyc_Absyn_new_stmt((void*)({struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_tmp627=_cycalloc(sizeof(*_tmp627));_tmp627->tag=10U,_tmp627->f1=e_exp,_tmp627->f2=_tmp641,_tmp627->f3=_tmp640;_tmp627;}),0U);
# 3998
Cyc_Toc_stmt_to_c(nv,_tmp620);{
# 4001
struct Cyc_Absyn_Exp*_tmp621=({
struct Cyc_Absyn_Exp*_tmpB06=Cyc_Toc_setjmp_e;Cyc_Absyn_fncall_exp(_tmpB06,({struct Cyc_List_List*_tmp626=_cycalloc(sizeof(*_tmp626));
({struct Cyc_Absyn_Exp*_tmpB05=Cyc_Toc_member_exp(h_exp,Cyc_Toc_handler_sp,0U);_tmp626->hd=_tmpB05;}),_tmp626->tl=0;_tmp626;}),0U);});
# 4005
struct Cyc_Absyn_Stmt*_tmp622=
Cyc_Absyn_exp_stmt(({struct Cyc_Absyn_Exp*_tmpB08=Cyc_Toc__push_handler_e;Cyc_Absyn_fncall_exp(_tmpB08,({struct Cyc_List_List*_tmp625=_cycalloc(sizeof(*_tmp625));
({struct Cyc_Absyn_Exp*_tmpB07=Cyc_Absyn_address_exp(h_exp,0U);_tmp625->hd=_tmpB07;}),_tmp625->tl=0;_tmp625;}),0U);}),0U);
# 4009
struct Cyc_Absyn_Exp*_tmp623=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,0,0U);
struct Cyc_Absyn_Exp*_tmp624=Cyc_Absyn_int_exp(Cyc_Absyn_Signed,1,0U);
({void*_tmpB17=({
struct _tuple1*_tmpB16=h_var;void*_tmpB15=h_typ;Cyc_Absyn_declare_stmt(_tmpB16,_tmpB15,0,({
struct Cyc_Absyn_Stmt*_tmpB14=_tmp622;Cyc_Absyn_seq_stmt(_tmpB14,({
struct _tuple1*_tmpB13=was_thrown_var;void*_tmpB12=was_thrown_typ;struct Cyc_Absyn_Exp*_tmpB11=_tmp623;Cyc_Absyn_declare_stmt(_tmpB13,_tmpB12,_tmpB11,({
struct Cyc_Absyn_Stmt*_tmpB10=({struct Cyc_Absyn_Exp*_tmpB0A=_tmp621;struct Cyc_Absyn_Stmt*_tmpB09=
Cyc_Absyn_assign_stmt(was_thrown_exp,_tmp624,0U);
# 4015
Cyc_Absyn_ifthenelse_stmt(_tmpB0A,_tmpB09,
# 4017
Cyc_Toc_skip_stmt_dl(),0U);});
# 4015
Cyc_Absyn_seq_stmt(_tmpB10,({
# 4018
struct Cyc_Absyn_Exp*_tmpB0F=Cyc_Absyn_prim1_exp(Cyc_Absyn_Not,was_thrown_exp,0U);struct Cyc_Absyn_Stmt*_tmpB0E=_tmp61F;Cyc_Absyn_ifthenelse_stmt(_tmpB0F,_tmpB0E,({
# 4020
struct _tuple1*_tmpB0D=e_var;void*_tmpB0C=e_typ;struct Cyc_Absyn_Exp*_tmpB0B=
Cyc_Toc_cast_it(e_typ,Cyc_Toc__exn_thrown_e);
# 4020
Cyc_Absyn_declare_stmt(_tmpB0D,_tmpB0C,_tmpB0B,_tmp620,0U);}),0U);}),0U);}),0U);}),0U);}),0U);})->r;
# 4011
s->r=_tmpB17;});};};
# 4024
_npop_handler(0U);return;
# 3989
;_pop_region(temp);};}}_LL0:;}}
# 4033
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s);
static void Cyc_Toc_fndecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Fndecl*f,int cinclude){
f->tvs=0;
f->effect=0;
f->rgn_po=0;
f->requires_clause=0;
f->ensures_clause=0;
({void*_tmpB18=Cyc_Toc_typ_to_c(f->ret_type);f->ret_type=_tmpB18;});{
struct _RegionHandle _tmp643=_new_region("frgn");struct _RegionHandle*frgn=& _tmp643;_push_region(frgn);
{struct Cyc_Toc_Env*_tmp644=Cyc_Toc_share_env(frgn,nv);
{struct Cyc_List_List*_tmp645=f->args;for(0;_tmp645 != 0;_tmp645=_tmp645->tl){
struct _tuple1*_tmp646=({struct _tuple1*_tmp647=_cycalloc(sizeof(*_tmp647));_tmp647->f1=Cyc_Absyn_Loc_n,_tmp647->f2=(*((struct _tuple9*)_tmp645->hd)).f1;_tmp647;});
({void*_tmpB19=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp645->hd)).f3);(*((struct _tuple9*)_tmp645->hd)).f3=_tmpB19;});}}
# 4049
if(cinclude){
Cyc_Toc_stmttypes_to_c(f->body);
_npop_handler(0U);return;}
# 4053
({struct Cyc_Hashtable_Table**_tmpB1B=({struct Cyc_Hashtable_Table**_tmp648=_cycalloc(sizeof(*_tmp648));({struct Cyc_Hashtable_Table*_tmpB1A=((struct Cyc_Hashtable_Table*(*)(struct Cyc_Hashtable_Table*t,struct Cyc_Absyn_Fndecl*key))Cyc_Hashtable_lookup)(*((struct Cyc_Hashtable_Table**)_check_null(Cyc_Toc_gpop_tables)),f);*_tmp648=_tmpB1A;});_tmp648;});Cyc_Toc_fn_pop_table=_tmpB1B;});
if((unsigned int)f->cyc_varargs  && ((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs))->name != 0){
struct Cyc_Absyn_VarargInfo _tmp649=*((struct Cyc_Absyn_VarargInfo*)_check_null(f->cyc_varargs));struct Cyc_Absyn_VarargInfo _tmp64A=_tmp649;struct _dyneither_ptr*_tmp653;struct Cyc_Absyn_Tqual _tmp652;void*_tmp651;int _tmp650;_LL1: _tmp653=_tmp64A.name;_tmp652=_tmp64A.tq;_tmp651=_tmp64A.type;_tmp650=_tmp64A.inject;_LL2:;{
void*_tmp64B=Cyc_Toc_typ_to_c(Cyc_Absyn_dyneither_typ(_tmp651,(void*)& Cyc_Absyn_HeapRgn_val,_tmp652,Cyc_Absyn_false_conref));
struct _tuple1*_tmp64C=({struct _tuple1*_tmp64F=_cycalloc(sizeof(*_tmp64F));_tmp64F->f1=Cyc_Absyn_Loc_n,_tmp64F->f2=(struct _dyneither_ptr*)_check_null(_tmp653);_tmp64F;});
({struct Cyc_List_List*_tmpB1E=({struct Cyc_List_List*_tmpB1D=f->args;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmpB1D,({struct Cyc_List_List*_tmp64E=_cycalloc(sizeof(*_tmp64E));({struct _tuple9*_tmpB1C=({struct _tuple9*_tmp64D=_cycalloc(sizeof(*_tmp64D));_tmp64D->f1=_tmp653,_tmp64D->f2=_tmp652,_tmp64D->f3=_tmp64B;_tmp64D;});_tmp64E->hd=_tmpB1C;}),_tmp64E->tl=0;_tmp64E;}));});f->args=_tmpB1E;});
f->cyc_varargs=0;};}
# 4062
{struct Cyc_List_List*_tmp654=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(f->param_vardecls))->v;for(0;_tmp654 != 0;_tmp654=_tmp654->tl){
({void*_tmpB1F=Cyc_Toc_typ_to_c(((struct Cyc_Absyn_Vardecl*)_tmp654->hd)->type);((struct Cyc_Absyn_Vardecl*)_tmp654->hd)->type=_tmpB1F;});}}
# 4065
({struct Cyc_Toc_Env*_tmpB20=Cyc_Toc_clear_toplevel(frgn,_tmp644);Cyc_Toc_stmt_to_c(_tmpB20,f->body);});}
# 4042
;_pop_region(frgn);};}
# 4069
static enum Cyc_Absyn_Scope Cyc_Toc_scope_to_c(enum Cyc_Absyn_Scope s){
enum Cyc_Absyn_Scope _tmp655=s;switch(_tmp655){case Cyc_Absyn_Abstract: _LL1: _LL2:
 return Cyc_Absyn_Public;case Cyc_Absyn_ExternC: _LL3: _LL4:
 return Cyc_Absyn_Extern;default: _LL5: _LL6:
 return s;}_LL0:;}struct _tuple39{struct Cyc_Absyn_Aggrdecl*f1;int f2;};struct _tuple40{struct Cyc_Toc_TocState*f1;struct _tuple39*f2;};
# 4086 "toc.cyc"
static int Cyc_Toc_aggrdecl_to_c_body(struct _RegionHandle*d,struct _tuple40*env){
# 4089
struct _tuple40 _tmp656=*env;struct _tuple40 _tmp657=_tmp656;struct Cyc_Toc_TocState*_tmp68C;struct Cyc_Absyn_Aggrdecl*_tmp68B;int _tmp68A;_LL1: _tmp68C=_tmp657.f1;_tmp68B=(_tmp657.f2)->f1;_tmp68A=(_tmp657.f2)->f2;_LL2:;{
struct _tuple1*_tmp658=_tmp68B->name;
struct Cyc_Toc_TocState _tmp659=*_tmp68C;struct Cyc_Toc_TocState _tmp65A=_tmp659;struct Cyc_Dict_Dict*_tmp689;_LL4: _tmp689=_tmp65A.aggrs_so_far;_LL5:;{
int seen_defn_before;
struct _tuple19**_tmp65B=((struct _tuple19**(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp689,_tmp658);
if(_tmp65B == 0){
seen_defn_before=0;{
struct _tuple19*v;
if(_tmp68B->kind == Cyc_Absyn_StructA)
({struct _tuple19*_tmpB22=({struct _tuple19*_tmp65C=_region_malloc(d,sizeof(*_tmp65C));_tmp65C->f1=_tmp68B,({void*_tmpB21=Cyc_Absyn_strctq(_tmp658);_tmp65C->f2=_tmpB21;});_tmp65C;});v=_tmpB22;});else{
# 4100
({struct _tuple19*_tmpB24=({struct _tuple19*_tmp65D=_region_malloc(d,sizeof(*_tmp65D));_tmp65D->f1=_tmp68B,({void*_tmpB23=Cyc_Absyn_unionq_typ(_tmp658);_tmp65D->f2=_tmpB23;});_tmp65D;});v=_tmpB24;});}
({struct Cyc_Dict_Dict _tmpB25=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(*_tmp689,_tmp658,v);*_tmp689=_tmpB25;});};}else{
# 4103
struct _tuple19*_tmp65E=*_tmp65B;struct _tuple19*_tmp65F=_tmp65E;struct Cyc_Absyn_Aggrdecl*_tmp662;void*_tmp661;_LL7: _tmp662=_tmp65F->f1;_tmp661=_tmp65F->f2;_LL8:;
if(_tmp662->impl == 0){
({struct Cyc_Dict_Dict _tmpB28=({struct Cyc_Dict_Dict _tmpB27=*_tmp689;struct _tuple1*_tmpB26=_tmp658;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,struct _tuple19*v))Cyc_Dict_insert)(_tmpB27,_tmpB26,({struct _tuple19*_tmp660=_region_malloc(d,sizeof(*_tmp660));_tmp660->f1=_tmp68B,_tmp660->f2=_tmp661;_tmp660;}));});*_tmp689=_tmpB28;});
seen_defn_before=0;}else{
# 4108
seen_defn_before=1;}}{
# 4110
struct Cyc_Absyn_Aggrdecl*new_ad=({struct Cyc_Absyn_Aggrdecl*_tmp688=_cycalloc(sizeof(*_tmp688));_tmp688->kind=_tmp68B->kind,_tmp688->sc=Cyc_Absyn_Public,_tmp688->name=_tmp68B->name,_tmp688->tvs=0,_tmp688->impl=0,_tmp688->expected_mem_kind=0,_tmp688->attributes=_tmp68B->attributes;_tmp688;});
# 4117
if(_tmp68B->impl != 0  && !seen_defn_before){
({struct Cyc_Absyn_AggrdeclImpl*_tmpB29=({struct Cyc_Absyn_AggrdeclImpl*_tmp663=_cycalloc(sizeof(*_tmp663));_tmp663->exist_vars=0,_tmp663->rgn_po=0,_tmp663->fields=0,_tmp663->tagged=0;_tmp663;});new_ad->impl=_tmpB29;});{
# 4122
struct Cyc_List_List*new_fields=0;
{struct Cyc_List_List*_tmp664=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68B->impl))->fields;for(0;_tmp664 != 0;_tmp664=_tmp664->tl){
# 4126
struct Cyc_Absyn_Aggrfield*_tmp665=(struct Cyc_Absyn_Aggrfield*)_tmp664->hd;
void*_tmp666=_tmp665->type;
struct Cyc_List_List*_tmp667=_tmp665->attributes;
if(Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_typ_kind(_tmp666)) && (
_tmp68B->kind == Cyc_Absyn_StructA  && _tmp664->tl == 0  || _tmp68B->kind == Cyc_Absyn_UnionA)){
# 4140 "toc.cyc"
void*_tmp668=Cyc_Tcutil_compress(_tmp666);void*_tmp669=_tmp668;void*_tmp671;struct Cyc_Absyn_Tqual _tmp670;union Cyc_Absyn_Constraint*_tmp66F;unsigned int _tmp66E;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp669)->tag == 8U){_LLA: _tmp671=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp669)->f1).elt_type;_tmp670=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp669)->f1).tq;_tmp66F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp669)->f1).zero_term;_tmp66E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp669)->f1).zt_loc;_LLB:
# 4143
({void*_tmpB2B=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp66A=_cycalloc(sizeof(*_tmp66A));_tmp66A->tag=8U,(_tmp66A->f1).elt_type=_tmp671,(_tmp66A->f1).tq=_tmp670,({struct Cyc_Absyn_Exp*_tmpB2A=Cyc_Absyn_uint_exp(0U,0U);(_tmp66A->f1).num_elts=_tmpB2A;}),(_tmp66A->f1).zero_term=_tmp66F,(_tmp66A->f1).zt_loc=_tmp66E;_tmp66A;});_tmp666=_tmpB2B;});
goto _LL9;}else{_LLC: _LLD:
# 4146
({struct Cyc_List_List*_tmpB2D=({struct Cyc_List_List*_tmp66C=_cycalloc(sizeof(*_tmp66C));({void*_tmpB2C=(void*)({struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct*_tmp66B=_cycalloc(sizeof(*_tmp66B));_tmp66B->tag=6U,_tmp66B->f1=0;_tmp66B;});_tmp66C->hd=_tmpB2C;}),_tmp66C->tl=_tmp667;_tmp66C;});_tmp667=_tmpB2D;});
({void*_tmpB32=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp66D=_cycalloc(sizeof(*_tmp66D));_tmp66D->tag=8U,({void*_tmpB31=Cyc_Absyn_void_star_typ();(_tmp66D->f1).elt_type=_tmpB31;}),({
struct Cyc_Absyn_Tqual _tmpB30=Cyc_Absyn_empty_tqual(0U);(_tmp66D->f1).tq=_tmpB30;}),({
struct Cyc_Absyn_Exp*_tmpB2F=Cyc_Absyn_uint_exp(0U,0U);(_tmp66D->f1).num_elts=_tmpB2F;}),({
union Cyc_Absyn_Constraint*_tmpB2E=((union Cyc_Absyn_Constraint*(*)(int x))Cyc_Absyn_new_conref)(0);(_tmp66D->f1).zero_term=_tmpB2E;}),(_tmp66D->f1).zt_loc=0U;_tmp66D;});
# 4147
_tmp666=_tmpB32;});}_LL9:;}{
# 4153
struct Cyc_Absyn_Aggrfield*_tmp672=({struct Cyc_Absyn_Aggrfield*_tmp684=_cycalloc(sizeof(*_tmp684));_tmp684->name=_tmp665->name,_tmp684->tq=Cyc_Toc_mt_tq,({
# 4155
void*_tmpB33=Cyc_Toc_typ_to_c(_tmp666);_tmp684->type=_tmpB33;}),_tmp684->width=_tmp665->width,_tmp684->attributes=_tmp667,_tmp684->requires_clause=0;_tmp684;});
# 4163
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp68B->impl))->tagged){
void*_tmp673=_tmp672->type;
struct _dyneither_ptr*_tmp674=_tmp672->name;
struct _dyneither_ptr s=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp681=({struct Cyc_String_pa_PrintArg_struct _tmp79C;_tmp79C.tag=0U,_tmp79C.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*(*_tmp68B->name).f2);_tmp79C;});struct Cyc_String_pa_PrintArg_struct _tmp682=({struct Cyc_String_pa_PrintArg_struct _tmp79B;_tmp79B.tag=0U,_tmp79B.f1=(struct _dyneither_ptr)((struct _dyneither_ptr)*_tmp674);_tmp79B;});void*_tmp67F[2U];_tmp67F[0]=& _tmp681,_tmp67F[1]=& _tmp682;({struct _dyneither_ptr _tmpB34=({const char*_tmp680="_union_%s_%s";_tag_dyneither(_tmp680,sizeof(char),13U);});Cyc_aprintf(_tmpB34,_tag_dyneither(_tmp67F,sizeof(void*),2U));});});
struct _dyneither_ptr*str=({struct _dyneither_ptr*_tmp67E=_cycalloc(sizeof(*_tmp67E));*_tmp67E=s;_tmp67E;});
struct Cyc_Absyn_Aggrfield*_tmp675=({struct Cyc_Absyn_Aggrfield*_tmp67D=_cycalloc(sizeof(*_tmp67D));_tmp67D->name=Cyc_Toc_val_sp,_tmp67D->tq=Cyc_Toc_mt_tq,_tmp67D->type=_tmp673,_tmp67D->width=0,_tmp67D->attributes=0,_tmp67D->requires_clause=0;_tmp67D;});
struct Cyc_Absyn_Aggrfield*_tmp676=({struct Cyc_Absyn_Aggrfield*_tmp67C=_cycalloc(sizeof(*_tmp67C));_tmp67C->name=Cyc_Toc_tag_sp,_tmp67C->tq=Cyc_Toc_mt_tq,_tmp67C->type=Cyc_Absyn_sint_typ,_tmp67C->width=0,_tmp67C->attributes=0,_tmp67C->requires_clause=0;_tmp67C;});
struct Cyc_List_List*_tmp677=({struct Cyc_Absyn_Aggrfield*_tmp67B[2U];_tmp67B[0]=_tmp676,_tmp67B[1]=_tmp675;((struct Cyc_List_List*(*)(struct _dyneither_ptr))Cyc_List_list)(_tag_dyneither(_tmp67B,sizeof(struct Cyc_Absyn_Aggrfield*),2U));});
struct Cyc_Absyn_Aggrdecl*_tmp678=Cyc_Toc_make_c_struct_defn(str,_tmp677);
({struct Cyc_List_List*_tmpB36=({struct Cyc_List_List*_tmp67A=_cycalloc(sizeof(*_tmp67A));({struct Cyc_Absyn_Decl*_tmpB35=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp679=_cycalloc(sizeof(*_tmp679));_tmp679->tag=5U,_tmp679->f1=_tmp678;_tmp679;}),0U);_tmp67A->hd=_tmpB35;}),_tmp67A->tl=Cyc_Toc_result_decls;_tmp67A;});Cyc_Toc_result_decls=_tmpB36;});
({void*_tmpB37=Cyc_Absyn_strct(str);_tmp672->type=_tmpB37;});}
# 4175
({struct Cyc_List_List*_tmpB38=({struct Cyc_List_List*_tmp683=_cycalloc(sizeof(*_tmp683));_tmp683->hd=_tmp672,_tmp683->tl=new_fields;_tmp683;});new_fields=_tmpB38;});};}}
# 4177
({struct Cyc_List_List*_tmpB39=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_fields);(new_ad->impl)->fields=_tmpB39;});};}
# 4179
if(_tmp68A)
({struct Cyc_List_List*_tmpB3C=({struct Cyc_List_List*_tmp687=_cycalloc(sizeof(*_tmp687));({struct Cyc_Absyn_Decl*_tmpB3B=({struct Cyc_Absyn_Decl*_tmp686=_cycalloc(sizeof(*_tmp686));({void*_tmpB3A=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp685=_cycalloc(sizeof(*_tmp685));_tmp685->tag=5U,_tmp685->f1=new_ad;_tmp685;});_tmp686->r=_tmpB3A;}),_tmp686->loc=0U;_tmp686;});_tmp687->hd=_tmpB3B;}),_tmp687->tl=Cyc_Toc_result_decls;_tmp687;});Cyc_Toc_result_decls=_tmpB3C;});
return 0;};};};}
# 4184
static void Cyc_Toc_aggrdecl_to_c(struct Cyc_Absyn_Aggrdecl*ad,int add_to_decls){
struct _tuple39 p=({struct _tuple39 _tmp79D;_tmp79D.f1=ad,_tmp79D.f2=add_to_decls;_tmp79D;});
((int(*)(struct _tuple39*arg,int(*f)(struct _RegionHandle*,struct _tuple40*)))Cyc_Toc_use_toc_state)(& p,Cyc_Toc_aggrdecl_to_c_body);}struct _tuple41{struct Cyc_Toc_TocState*f1;struct Cyc_Absyn_Datatypedecl*f2;};
# 4213 "toc.cyc"
static int Cyc_Toc_datatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4216
struct _tuple41 _tmp68D=*env;struct _tuple41 _tmp68E=_tmp68D;struct Cyc_Set_Set**_tmp69F;struct Cyc_Absyn_Datatypedecl*_tmp69E;_LL1: _tmp69F=(_tmp68E.f1)->datatypes_so_far;_tmp69E=_tmp68E.f2;_LL2:;{
struct _tuple1*_tmp68F=_tmp69E->name;
if(_tmp69E->fields == 0  || ((int(*)(struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_member)(*_tmp69F,_tmp68F))
return 0;
({struct Cyc_Set_Set*_tmpB3D=((struct Cyc_Set_Set*(*)(struct _RegionHandle*r,struct Cyc_Set_Set*s,struct _tuple1*elt))Cyc_Set_rinsert)(d,*_tmp69F,_tmp68F);*_tmp69F=_tmpB3D;});
{struct Cyc_List_List*_tmp690=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp69E->fields))->v;for(0;_tmp690 != 0;_tmp690=_tmp690->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp690->hd;
# 4224
struct Cyc_List_List*_tmp691=0;
int i=1;
{struct Cyc_List_List*_tmp692=f->typs;for(0;_tmp692 != 0;(_tmp692=_tmp692->tl,i ++)){
struct _dyneither_ptr*_tmp693=Cyc_Absyn_fieldname(i);
struct Cyc_Absyn_Aggrfield*_tmp694=({struct Cyc_Absyn_Aggrfield*_tmp696=_cycalloc(sizeof(*_tmp696));_tmp696->name=_tmp693,_tmp696->tq=(*((struct _tuple12*)_tmp692->hd)).f1,({
void*_tmpB3E=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp692->hd)).f2);_tmp696->type=_tmpB3E;}),_tmp696->width=0,_tmp696->attributes=0,_tmp696->requires_clause=0;_tmp696;});
({struct Cyc_List_List*_tmpB3F=({struct Cyc_List_List*_tmp695=_cycalloc(sizeof(*_tmp695));_tmp695->hd=_tmp694,_tmp695->tl=_tmp691;_tmp695;});_tmp691=_tmpB3F;});}}
# 4232
({struct Cyc_List_List*_tmpB42=({struct Cyc_List_List*_tmp698=_cycalloc(sizeof(*_tmp698));({struct Cyc_Absyn_Aggrfield*_tmpB41=({struct Cyc_Absyn_Aggrfield*_tmp697=_cycalloc(sizeof(*_tmp697));_tmp697->name=Cyc_Toc_tag_sp,_tmp697->tq=Cyc_Toc_mt_tq,_tmp697->type=Cyc_Absyn_sint_typ,_tmp697->width=0,_tmp697->attributes=0,_tmp697->requires_clause=0;_tmp697;});_tmp698->hd=_tmpB41;}),({
struct Cyc_List_List*_tmpB40=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp691);_tmp698->tl=_tmpB40;});_tmp698;});
# 4232
_tmp691=_tmpB42;});{
# 4234
struct Cyc_Absyn_Aggrdecl*_tmp699=({struct _dyneither_ptr*_tmpB44=({struct _dyneither_ptr*_tmp69D=_cycalloc(sizeof(*_tmp69D));({struct _dyneither_ptr _tmpB43=({const char*_tmp69C="";_tag_dyneither(_tmp69C,sizeof(char),1U);});*_tmp69D=_tmpB43;});_tmp69D;});Cyc_Toc_make_c_struct_defn(_tmpB44,_tmp691);});
({struct _tuple1*_tmpB45=Cyc_Toc_collapse_qvars(f->name,_tmp69E->name);_tmp699->name=_tmpB45;});
({struct Cyc_List_List*_tmpB47=({struct Cyc_List_List*_tmp69B=_cycalloc(sizeof(*_tmp69B));({struct Cyc_Absyn_Decl*_tmpB46=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp69A=_cycalloc(sizeof(*_tmp69A));_tmp69A->tag=5U,_tmp69A->f1=_tmp699;_tmp69A;}),0U);_tmp69B->hd=_tmpB46;}),_tmp69B->tl=Cyc_Toc_result_decls;_tmp69B;});Cyc_Toc_result_decls=_tmpB47;});};}}
# 4238
return 0;};}
# 4241
static void Cyc_Toc_datatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*tud){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(tud,Cyc_Toc_datatypedecl_to_c_body);}
# 4260 "toc.cyc"
static int Cyc_Toc_xdatatypedecl_to_c_body(struct _RegionHandle*d,struct _tuple41*env){
# 4263
struct _tuple41 _tmp6A0=*env;struct _tuple41 _tmp6A1=_tmp6A0;struct Cyc_Toc_TocState*_tmp6C3;struct Cyc_Absyn_Datatypedecl*_tmp6C2;_LL1: _tmp6C3=_tmp6A1.f1;_tmp6C2=_tmp6A1.f2;_LL2:;
if(_tmp6C2->fields == 0)
return 0;{
struct Cyc_Toc_TocState _tmp6A2=*_tmp6C3;struct Cyc_Toc_TocState _tmp6A3=_tmp6A2;struct Cyc_Dict_Dict*_tmp6C1;_LL4: _tmp6C1=_tmp6A3.xdatatypes_so_far;_LL5:;{
struct _tuple1*_tmp6A4=_tmp6C2->name;
{struct Cyc_List_List*_tmp6A5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6C2->fields))->v;for(0;_tmp6A5 != 0;_tmp6A5=_tmp6A5->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_tmp6A5->hd;
struct _dyneither_ptr*fn=(*f->name).f2;
struct Cyc_Absyn_Exp*_tmp6A6=Cyc_Absyn_uint_exp(_get_dyneither_size(*fn,sizeof(char)),0U);
void*_tmp6A7=Cyc_Absyn_array_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq,_tmp6A6,Cyc_Absyn_false_conref,0U);
# 4274
int*_tmp6A8=((int*(*)(struct Cyc_Dict_Dict d,struct _tuple1*k))Cyc_Dict_lookup_opt)(*_tmp6C1,f->name);int*_tmp6A9=_tmp6A8;if(_tmp6A9 == 0){_LL7: _LL8: {
# 4276
struct Cyc_Absyn_Exp*initopt=0;
if(f->sc != Cyc_Absyn_Extern)
({struct Cyc_Absyn_Exp*_tmpB48=Cyc_Absyn_string_exp(*fn,0U);initopt=_tmpB48;});{
# 4280
struct Cyc_Absyn_Vardecl*_tmp6AA=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6A7,initopt);
_tmp6AA->sc=f->sc;
({struct Cyc_List_List*_tmpB4A=({struct Cyc_List_List*_tmp6AC=_cycalloc(sizeof(*_tmp6AC));({struct Cyc_Absyn_Decl*_tmpB49=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6AB=_cycalloc(sizeof(*_tmp6AB));_tmp6AB->tag=0U,_tmp6AB->f1=_tmp6AA;_tmp6AB;}),0U);_tmp6AC->hd=_tmpB49;}),_tmp6AC->tl=Cyc_Toc_result_decls;_tmp6AC;});Cyc_Toc_result_decls=_tmpB4A;});
({struct Cyc_Dict_Dict _tmpB4B=
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6C1,f->name,f->sc != Cyc_Absyn_Extern);
# 4283
*_tmp6C1=_tmpB4B;});{
# 4285
struct Cyc_List_List*fields=0;
int i=1;
{struct Cyc_List_List*_tmp6AD=f->typs;for(0;_tmp6AD != 0;(_tmp6AD=_tmp6AD->tl,i ++)){
struct _dyneither_ptr*_tmp6AE=({struct _dyneither_ptr*_tmp6B5=_cycalloc(sizeof(*_tmp6B5));({struct _dyneither_ptr _tmpB4D=(struct _dyneither_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp6B4=({struct Cyc_Int_pa_PrintArg_struct _tmp79E;_tmp79E.tag=1U,_tmp79E.f1=(unsigned long)i;_tmp79E;});void*_tmp6B2[1U];_tmp6B2[0]=& _tmp6B4;({struct _dyneither_ptr _tmpB4C=({const char*_tmp6B3="f%d";_tag_dyneither(_tmp6B3,sizeof(char),4U);});Cyc_aprintf(_tmpB4C,_tag_dyneither(_tmp6B2,sizeof(void*),1U));});});*_tmp6B5=_tmpB4D;});_tmp6B5;});
struct Cyc_Absyn_Aggrfield*_tmp6AF=({struct Cyc_Absyn_Aggrfield*_tmp6B1=_cycalloc(sizeof(*_tmp6B1));_tmp6B1->name=_tmp6AE,_tmp6B1->tq=(*((struct _tuple12*)_tmp6AD->hd)).f1,({
void*_tmpB4E=Cyc_Toc_typ_to_c((*((struct _tuple12*)_tmp6AD->hd)).f2);_tmp6B1->type=_tmpB4E;}),_tmp6B1->width=0,_tmp6B1->attributes=0,_tmp6B1->requires_clause=0;_tmp6B1;});
({struct Cyc_List_List*_tmpB4F=({struct Cyc_List_List*_tmp6B0=_cycalloc(sizeof(*_tmp6B0));_tmp6B0->hd=_tmp6AF,_tmp6B0->tl=fields;_tmp6B0;});fields=_tmpB4F;});}}
# 4293
({struct Cyc_List_List*_tmpB53=({struct Cyc_List_List*_tmp6B7=_cycalloc(sizeof(*_tmp6B7));({struct Cyc_Absyn_Aggrfield*_tmpB52=({struct Cyc_Absyn_Aggrfield*_tmp6B6=_cycalloc(sizeof(*_tmp6B6));_tmp6B6->name=Cyc_Toc_tag_sp,_tmp6B6->tq=Cyc_Toc_mt_tq,({
void*_tmpB51=Cyc_Absyn_cstar_typ(Cyc_Absyn_char_typ,Cyc_Toc_mt_tq);_tmp6B6->type=_tmpB51;}),_tmp6B6->width=0,_tmp6B6->attributes=0,_tmp6B6->requires_clause=0;_tmp6B6;});
# 4293
_tmp6B7->hd=_tmpB52;}),({
# 4295
struct Cyc_List_List*_tmpB50=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);_tmp6B7->tl=_tmpB50;});_tmp6B7;});
# 4293
fields=_tmpB53;});{
# 4296
struct Cyc_Absyn_Aggrdecl*_tmp6B8=({struct _dyneither_ptr*_tmpB55=({struct _dyneither_ptr*_tmp6BC=_cycalloc(sizeof(*_tmp6BC));({struct _dyneither_ptr _tmpB54=({const char*_tmp6BB="";_tag_dyneither(_tmp6BB,sizeof(char),1U);});*_tmp6BC=_tmpB54;});_tmp6BC;});Cyc_Toc_make_c_struct_defn(_tmpB55,fields);});
({struct _tuple1*_tmpB56=Cyc_Toc_collapse_qvars(f->name,_tmp6C2->name);_tmp6B8->name=_tmpB56;});
({struct Cyc_List_List*_tmpB58=({struct Cyc_List_List*_tmp6BA=_cycalloc(sizeof(*_tmp6BA));({struct Cyc_Absyn_Decl*_tmpB57=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp6B9=_cycalloc(sizeof(*_tmp6B9));_tmp6B9->tag=5U,_tmp6B9->f1=_tmp6B8;_tmp6B9;}),0U);_tmp6BA->hd=_tmpB57;}),_tmp6BA->tl=Cyc_Toc_result_decls;_tmp6BA;});Cyc_Toc_result_decls=_tmpB58;});
goto _LL6;};};};}}else{if(*((int*)_tmp6A9)== 0){_LL9: _LLA:
# 4301
 if(f->sc != Cyc_Absyn_Extern){
struct Cyc_Absyn_Exp*_tmp6BD=Cyc_Absyn_string_exp(*fn,0U);
struct Cyc_Absyn_Vardecl*_tmp6BE=Cyc_Absyn_new_vardecl(0U,f->name,_tmp6A7,_tmp6BD);
_tmp6BE->sc=f->sc;
({struct Cyc_List_List*_tmpB5A=({struct Cyc_List_List*_tmp6C0=_cycalloc(sizeof(*_tmp6C0));({struct Cyc_Absyn_Decl*_tmpB59=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6BF=_cycalloc(sizeof(*_tmp6BF));_tmp6BF->tag=0U,_tmp6BF->f1=_tmp6BE;_tmp6BF;}),0U);_tmp6C0->hd=_tmpB59;}),_tmp6C0->tl=Cyc_Toc_result_decls;_tmp6C0;});Cyc_Toc_result_decls=_tmpB5A;});
({struct Cyc_Dict_Dict _tmpB5B=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _tuple1*k,int v))Cyc_Dict_insert)(*_tmp6C1,f->name,1);*_tmp6C1=_tmpB5B;});}
# 4308
goto _LL6;}else{_LLB: _LLC:
 goto _LL6;}}_LL6:;}}
# 4312
return 0;};};}
# 4315
static void Cyc_Toc_xdatatypedecl_to_c(struct Cyc_Absyn_Datatypedecl*xd){
((int(*)(struct Cyc_Absyn_Datatypedecl*arg,int(*f)(struct _RegionHandle*,struct _tuple41*)))Cyc_Toc_use_toc_state)(xd,Cyc_Toc_xdatatypedecl_to_c_body);}
# 4319
static void Cyc_Toc_enumdecl_to_c(struct Cyc_Absyn_Enumdecl*ed){
ed->sc=Cyc_Absyn_Public;
if(ed->fields != 0)
Cyc_Toc_enumfields_to_c((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v);}
# 4325
static void Cyc_Toc_local_decl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Vardecl*vd,struct Cyc_Absyn_Stmt*s){
void*old_typ=vd->type;
({void*_tmpB5C=Cyc_Toc_typ_to_c(old_typ);vd->type=_tmpB5C;});
# 4329
if(vd->sc == Cyc_Absyn_Register  && Cyc_Tcutil_is_tagged_pointer_typ(old_typ))
vd->sc=Cyc_Absyn_Public;
Cyc_Toc_stmt_to_c(nv,s);
if(vd->initializer != 0){
struct Cyc_Absyn_Exp*init=(struct Cyc_Absyn_Exp*)_check_null(vd->initializer);
if(vd->sc == Cyc_Absyn_Static){
# 4338
struct _RegionHandle _tmp6C4=_new_region("temp");struct _RegionHandle*temp=& _tmp6C4;_push_region(temp);
{struct Cyc_Toc_Env*_tmp6C5=Cyc_Toc_set_toplevel(temp,nv);
Cyc_Toc_exp_to_c(_tmp6C5,init);}
# 4339
;_pop_region(temp);}else{
# 4343
Cyc_Toc_exp_to_c(nv,init);}}else{
# 4346
void*_tmp6C6=Cyc_Tcutil_compress(old_typ);void*_tmp6C7=_tmp6C6;void*_tmp6CE;struct Cyc_Absyn_Exp*_tmp6CD;union Cyc_Absyn_Constraint*_tmp6CC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C7)->tag == 8U){_LL1: _tmp6CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C7)->f1).elt_type;_tmp6CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C7)->f1).num_elts;_tmp6CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6C7)->f1).zero_term;_LL2:
# 4348
 if(((int(*)(int y,union Cyc_Absyn_Constraint*x))Cyc_Absyn_conref_def)(0,_tmp6CC)){
if(_tmp6CD == 0)
({void*_tmp6C8=0U;({struct _dyneither_ptr _tmpB5D=({const char*_tmp6C9="can't initialize zero-terminated array -- size unknown";_tag_dyneither(_tmp6C9,sizeof(char),55U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB5D,_tag_dyneither(_tmp6C8,sizeof(void*),0U));});});{
struct Cyc_Absyn_Exp*num_elts=_tmp6CD;
struct Cyc_Absyn_Exp*_tmp6CA=({struct Cyc_Absyn_Exp*_tmpB5F=Cyc_Absyn_var_exp(vd->name,0U);Cyc_Absyn_subscript_exp(_tmpB5F,({
struct Cyc_Absyn_Exp*_tmpB5E=num_elts;Cyc_Absyn_add_exp(_tmpB5E,Cyc_Absyn_signed_int_exp(- 1,0U),0U);}),0U);});
# 4355
struct Cyc_Absyn_Exp*_tmp6CB=Cyc_Absyn_signed_int_exp(0,0U);
({void*_tmpB61=({struct Cyc_Absyn_Stmt*_tmpB60=Cyc_Absyn_exp_stmt(Cyc_Absyn_assign_exp(_tmp6CA,_tmp6CB,0U),0U);Cyc_Toc_seq_stmt_r(_tmpB60,
Cyc_Absyn_new_stmt(s->r,0U));});
# 4356
s->r=_tmpB61;});};}
# 4359
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}}
# 4367
static void*Cyc_Toc_rewrite_decision(void*d,struct Cyc_Absyn_Stmt*success){
void*_tmp6CF=d;struct Cyc_List_List*_tmp6D6;struct Cyc_List_List*_tmp6D5;void**_tmp6D4;struct Cyc_Tcpat_Rhs*_tmp6D3;switch(*((int*)_tmp6CF)){case 0U: _LL1: _LL2:
 return d;case 1U: _LL3: _tmp6D3=((struct Cyc_Tcpat_Success_Tcpat_Decision_struct*)_tmp6CF)->f1;_LL4:
 _tmp6D3->rhs=success;return d;default: _LL5: _tmp6D6=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CF)->f1;_tmp6D5=((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CF)->f2;_tmp6D4=(void**)&((struct Cyc_Tcpat_SwitchDec_Tcpat_Decision_struct*)_tmp6CF)->f3;_LL6:
# 4372
({void*_tmpB62=Cyc_Toc_rewrite_decision(*_tmp6D4,success);*_tmp6D4=_tmpB62;});
for(0;_tmp6D5 != 0;_tmp6D5=_tmp6D5->tl){
struct _tuple33*_tmp6D0=(struct _tuple33*)_tmp6D5->hd;struct _tuple33*_tmp6D1=_tmp6D0;void**_tmp6D2;_LL8: _tmp6D2=(void**)& _tmp6D1->f2;_LL9:;
({void*_tmpB63=Cyc_Toc_rewrite_decision(*_tmp6D2,success);*_tmp6D2=_tmpB63;});}
# 4377
return d;}_LL0:;}
# 4388 "toc.cyc"
static struct Cyc_Absyn_Stmt*Cyc_Toc_letdecl_to_c(struct Cyc_Toc_Env*nv,struct Cyc_Absyn_Pat*p,void*dopt,void*t,struct Cyc_Absyn_Exp*e,struct Cyc_Absyn_Stmt*s){
# 4390
struct _RegionHandle _tmp6D7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6D7;_push_region(rgn);
{struct Cyc_Toc_Env*_tmp6D8=Cyc_Toc_share_env(rgn,nv);
void*_tmp6D9=(void*)_check_null(e->topt);
Cyc_Toc_exp_to_c(_tmp6D8,e);{
struct _tuple1*v=Cyc_Toc_temp_var();
struct Cyc_Absyn_Exp*path=Cyc_Absyn_var_exp(v,0U);
struct _dyneither_ptr*end_l=Cyc_Toc_fresh_label();
# 4400
struct Cyc_Absyn_Stmt*_tmp6DA=Cyc_Toc_skip_stmt_dl();
if(dopt != 0)
# 4404
({void*_tmpB64=Cyc_Toc_rewrite_decision(dopt,_tmp6DA);dopt=_tmpB64;});{
# 4406
struct Cyc_Absyn_Switch_clause*_tmp6DB=({struct Cyc_Absyn_Switch_clause*_tmp6E7=_cycalloc(sizeof(*_tmp6E7));_tmp6E7->pattern=p,_tmp6E7->pat_vars=0,_tmp6E7->where_clause=0,_tmp6E7->body=_tmp6DA,_tmp6E7->loc=0U;_tmp6E7;});
struct Cyc_List_List*_tmp6DC=({struct _RegionHandle*_tmpB67=rgn;struct _RegionHandle*_tmpB66=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple35*(*f)(struct _RegionHandle*,struct Cyc_Absyn_Switch_clause*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(_tmpB67,Cyc_Toc_gen_labels,_tmpB66,({struct Cyc_Absyn_Switch_clause*_tmp6E6[1U];_tmp6E6[0]=_tmp6DB;({struct _RegionHandle*_tmpB65=rgn;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _dyneither_ptr))Cyc_List_rlist)(_tmpB65,_tag_dyneither(_tmp6E6,sizeof(struct Cyc_Absyn_Switch_clause*),1U));});}));});
# 4409
struct Cyc_List_List*mydecls=0;
struct Cyc_List_List*mybodies=0;
# 4413
struct Cyc_Absyn_Stmt*test_tree=Cyc_Toc_compile_decision_tree(rgn,_tmp6D8,& mydecls,& mybodies,dopt,_tmp6DC,v);
# 4416
struct Cyc_Toc_Env*senv;
for(0;mybodies != 0;mybodies=((struct Cyc_List_List*)_check_null(mybodies))->tl){
struct _tuple38*_tmp6DD=(struct _tuple38*)((struct Cyc_List_List*)_check_null(mybodies))->hd;struct _tuple38*_tmp6DE=_tmp6DD;struct Cyc_Toc_Env*_tmp6E0;struct Cyc_Absyn_Stmt*_tmp6DF;_LL1: _tmp6E0=_tmp6DE->f1;_tmp6DF=_tmp6DE->f3;_LL2:;
if(_tmp6DF == _tmp6DA){senv=_tmp6E0;goto FOUND_ENV;}}
# 4421
({void*_tmp6E1=0U;({struct _dyneither_ptr _tmpB68=({const char*_tmp6E2="letdecl_to_c: couldn't find env!";_tag_dyneither(_tmp6E2,sizeof(char),33U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB68,_tag_dyneither(_tmp6E1,sizeof(void*),0U));});});
FOUND_ENV:
# 4425
 Cyc_Toc_stmt_to_c(senv,s);{
# 4427
struct Cyc_Absyn_Stmt*res=Cyc_Absyn_seq_stmt(test_tree,s,0U);
# 4429
for(0;mydecls != 0;mydecls=((struct Cyc_List_List*)_check_null(mydecls))->tl){
struct Cyc_Absyn_Vardecl*_tmp6E3=(struct Cyc_Absyn_Vardecl*)((struct Cyc_List_List*)_check_null(mydecls))->hd;
({struct _tuple1*_tmpB69=Cyc_Toc_temp_var();_tmp6E3->name=_tmpB69;});
({void*_tmpB6A=Cyc_Toc_typ_to_c_array(_tmp6E3->type);_tmp6E3->type=_tmpB6A;});
({struct Cyc_Absyn_Stmt*_tmpB6C=({struct Cyc_Absyn_Decl*_tmpB6B=Cyc_Absyn_new_decl((void*)({struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_tmp6E4=_cycalloc(sizeof(*_tmp6E4));_tmp6E4->tag=0U,_tmp6E4->f1=_tmp6E3;_tmp6E4;}),0U);Cyc_Absyn_decl_stmt(_tmpB6B,res,0U);});res=_tmpB6C;});}
# 4436
({struct Cyc_Absyn_Stmt*_tmpB70=({struct _tuple1*_tmpB6F=v;void*_tmpB6E=Cyc_Toc_typ_to_c(_tmp6D9);struct Cyc_Absyn_Exp*_tmpB6D=e;Cyc_Absyn_declare_stmt(_tmpB6F,_tmpB6E,_tmpB6D,res,0U);});res=_tmpB70;});{
struct Cyc_Absyn_Stmt*_tmp6E5=res;_npop_handler(0U);return _tmp6E5;};};};};}
# 4391
;_pop_region(rgn);}
# 4444
static void Cyc_Toc_exptypes_to_c(struct Cyc_Absyn_Exp*e){
void*_tmp6E8=e->r;void*_tmp6E9=_tmp6E8;struct Cyc_Absyn_MallocInfo*_tmp713;struct Cyc_Absyn_Stmt*_tmp712;void**_tmp711;void**_tmp710;struct Cyc_List_List*_tmp70F;struct Cyc_List_List*_tmp70E;void**_tmp70D;struct Cyc_List_List*_tmp70C;void**_tmp70B;struct Cyc_Absyn_Exp*_tmp70A;struct Cyc_Absyn_Exp*_tmp709;struct Cyc_List_List*_tmp708;struct Cyc_Absyn_Exp*_tmp707;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_Absyn_Exp*_tmp705;struct Cyc_Absyn_Exp*_tmp704;struct Cyc_Absyn_Exp*_tmp703;struct Cyc_Absyn_Exp*_tmp702;struct Cyc_Absyn_Exp*_tmp701;struct Cyc_Absyn_Exp*_tmp700;struct Cyc_Absyn_Exp*_tmp6FF;struct Cyc_Absyn_Exp*_tmp6FE;struct Cyc_Absyn_Exp*_tmp6FD;struct Cyc_Absyn_Exp*_tmp6FC;struct Cyc_Absyn_Exp*_tmp6FB;struct Cyc_Absyn_Exp*_tmp6FA;struct Cyc_Absyn_Exp*_tmp6F9;struct Cyc_List_List*_tmp6F8;struct Cyc_Absyn_Exp*_tmp6F7;struct Cyc_Absyn_Exp*_tmp6F6;struct Cyc_Absyn_Exp*_tmp6F5;struct Cyc_Absyn_Exp*_tmp6F4;struct Cyc_Absyn_Exp*_tmp6F3;struct Cyc_Absyn_Exp*_tmp6F2;struct Cyc_Absyn_Exp*_tmp6F1;struct Cyc_Absyn_Exp*_tmp6F0;switch(*((int*)_tmp6E9)){case 20U: _LL1: _tmp6F0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL2:
 _tmp6F1=_tmp6F0;goto _LL4;case 21U: _LL3: _tmp6F1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL4:
 _tmp6F2=_tmp6F1;goto _LL6;case 22U: _LL5: _tmp6F2=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL6:
 _tmp6F3=_tmp6F2;goto _LL8;case 15U: _LL7: _tmp6F3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL8:
 _tmp6F4=_tmp6F3;goto _LLA;case 11U: _LL9: _tmp6F4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LLA:
 _tmp6F5=_tmp6F4;goto _LLC;case 12U: _LLB: _tmp6F5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LLC:
 _tmp6F6=_tmp6F5;goto _LLE;case 18U: _LLD: _tmp6F6=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LLE:
 _tmp6F7=_tmp6F6;goto _LL10;case 5U: _LLF: _tmp6F7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL10:
 Cyc_Toc_exptypes_to_c(_tmp6F7);goto _LL0;case 3U: _LL11: _tmp6F8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL12:
((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp6F8);goto _LL0;case 7U: _LL13: _tmp6FA=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp6F9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL14:
 _tmp6FC=_tmp6FA;_tmp6FB=_tmp6F9;goto _LL16;case 8U: _LL15: _tmp6FC=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp6FB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL16:
 _tmp6FE=_tmp6FC;_tmp6FD=_tmp6FB;goto _LL18;case 9U: _LL17: _tmp6FE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp6FD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL18:
 _tmp700=_tmp6FE;_tmp6FF=_tmp6FD;goto _LL1A;case 23U: _LL19: _tmp700=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp6FF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL1A:
 _tmp702=_tmp700;_tmp701=_tmp6FF;goto _LL1C;case 35U: _LL1B: _tmp702=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp701=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL1C:
 _tmp704=_tmp702;_tmp703=_tmp701;goto _LL1E;case 4U: _LL1D: _tmp704=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp703=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmp6E9)->f3;_LL1E:
 Cyc_Toc_exptypes_to_c(_tmp704);Cyc_Toc_exptypes_to_c(_tmp703);goto _LL0;case 6U: _LL1F: _tmp707=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp706=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_tmp705=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6E9)->f3;_LL20:
# 4462
 Cyc_Toc_exptypes_to_c(_tmp707);Cyc_Toc_exptypes_to_c(_tmp706);Cyc_Toc_exptypes_to_c(_tmp705);goto _LL0;case 10U: _LL21: _tmp709=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp708=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL22:
# 4464
 Cyc_Toc_exptypes_to_c(_tmp709);((void(*)(void(*f)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Toc_exptypes_to_c,_tmp708);goto _LL0;case 14U: _LL23: _tmp70B=(void**)&((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_tmp70A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL24:
({void*_tmpB71=Cyc_Toc_typ_to_c(*_tmp70B);*_tmp70B=_tmpB71;});Cyc_Toc_exptypes_to_c(_tmp70A);goto _LL0;case 25U: _LL25: _tmp70D=(void**)&(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1)->f3;_tmp70C=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL26:
# 4467
({void*_tmpB72=Cyc_Toc_typ_to_c(*_tmp70D);*_tmp70D=_tmpB72;});
_tmp70E=_tmp70C;goto _LL28;case 36U: _LL27: _tmp70E=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmp6E9)->f2;_LL28:
 _tmp70F=_tmp70E;goto _LL2A;case 26U: _LL29: _tmp70F=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL2A:
# 4471
 for(0;_tmp70F != 0;_tmp70F=_tmp70F->tl){
struct _tuple20 _tmp6EA=*((struct _tuple20*)_tmp70F->hd);struct _tuple20 _tmp6EB=_tmp6EA;struct Cyc_Absyn_Exp*_tmp6EC;_LL54: _tmp6EC=_tmp6EB.f2;_LL55:;
Cyc_Toc_exptypes_to_c(_tmp6EC);}
# 4475
goto _LL0;case 19U: _LL2B: _tmp710=(void**)&((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL2C:
 _tmp711=_tmp710;goto _LL2E;case 17U: _LL2D: _tmp711=(void**)&((struct Cyc_Absyn_Sizeoftyp_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL2E:
({void*_tmpB73=Cyc_Toc_typ_to_c(*_tmp711);*_tmp711=_tmpB73;});goto _LL0;case 37U: _LL2F: _tmp712=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL30:
 Cyc_Toc_stmttypes_to_c(_tmp712);goto _LL0;case 34U: _LL31: _tmp713=(struct Cyc_Absyn_MallocInfo*)&((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmp6E9)->f1;_LL32:
# 4480
 if(_tmp713->elt_type != 0)
({void**_tmpB75=({void**_tmp6ED=_cycalloc(sizeof(*_tmp6ED));({void*_tmpB74=Cyc_Toc_typ_to_c(*((void**)_check_null(_tmp713->elt_type)));*_tmp6ED=_tmpB74;});_tmp6ED;});_tmp713->elt_type=_tmpB75;});
Cyc_Toc_exptypes_to_c(_tmp713->num_elts);
goto _LL0;case 0U: _LL33: _LL34:
 goto _LL36;case 1U: _LL35: _LL36:
 goto _LL38;case 32U: _LL37: _LL38:
 goto _LL3A;case 40U: _LL39: _LL3A:
 goto _LL3C;case 33U: _LL3B: _LL3C:
 goto _LL0;case 2U: _LL3D: _LL3E:
# 4490
 goto _LL40;case 30U: _LL3F: _LL40:
 goto _LL42;case 31U: _LL41: _LL42:
 goto _LL44;case 29U: _LL43: _LL44:
 goto _LL46;case 27U: _LL45: _LL46:
 goto _LL48;case 28U: _LL47: _LL48:
 goto _LL4A;case 24U: _LL49: _LL4A:
 goto _LL4C;case 13U: _LL4B: _LL4C:
 goto _LL4E;case 16U: _LL4D: _LL4E:
 goto _LL50;case 39U: _LL4F: _LL50:
 goto _LL52;default: _LL51: _LL52:
({void*_tmp6EE=0U;({unsigned int _tmpB77=e->loc;struct _dyneither_ptr _tmpB76=({const char*_tmp6EF="Cyclone expression within C code";_tag_dyneither(_tmp6EF,sizeof(char),33U);});Cyc_Tcutil_terr(_tmpB77,_tmpB76,_tag_dyneither(_tmp6EE,sizeof(void*),0U));});});goto _LL0;}_LL0:;}
# 4504
static void Cyc_Toc_decltypes_to_c(struct Cyc_Absyn_Decl*d){
void*_tmp714=d->r;void*_tmp715=_tmp714;struct Cyc_Absyn_Typedefdecl*_tmp71F;struct Cyc_Absyn_Enumdecl*_tmp71E;struct Cyc_Absyn_Aggrdecl*_tmp71D;struct Cyc_Absyn_Fndecl*_tmp71C;struct Cyc_Absyn_Vardecl*_tmp71B;switch(*((int*)_tmp715)){case 0U: _LL1: _tmp71B=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp715)->f1;_LL2:
# 4507
({void*_tmpB78=Cyc_Toc_typ_to_c(_tmp71B->type);_tmp71B->type=_tmpB78;});
if(_tmp71B->initializer != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp71B->initializer));
goto _LL0;case 1U: _LL3: _tmp71C=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp715)->f1;_LL4:
# 4511
({void*_tmpB79=Cyc_Toc_typ_to_c(_tmp71C->ret_type);_tmp71C->ret_type=_tmpB79;});
{struct Cyc_List_List*_tmp716=_tmp71C->args;for(0;_tmp716 != 0;_tmp716=_tmp716->tl){
({void*_tmpB7A=Cyc_Toc_typ_to_c((*((struct _tuple9*)_tmp716->hd)).f3);(*((struct _tuple9*)_tmp716->hd)).f3=_tmpB7A;});}}
# 4515
goto _LL0;case 5U: _LL5: _tmp71D=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp715)->f1;_LL6:
 Cyc_Toc_aggrdecl_to_c(_tmp71D,1);goto _LL0;case 7U: _LL7: _tmp71E=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp715)->f1;_LL8:
# 4518
 if(_tmp71E->fields != 0){
struct Cyc_List_List*_tmp717=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp71E->fields))->v;for(0;_tmp717 != 0;_tmp717=_tmp717->tl){
struct Cyc_Absyn_Enumfield*_tmp718=(struct Cyc_Absyn_Enumfield*)_tmp717->hd;
if(_tmp718->tag != 0)Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp718->tag));}}
# 4523
goto _LL0;case 8U: _LL9: _tmp71F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp715)->f1;_LLA:
({void*_tmpB7B=Cyc_Toc_typ_to_c((void*)_check_null(_tmp71F->defn));_tmp71F->defn=_tmpB7B;});goto _LL0;case 2U: _LLB: _LLC:
 goto _LLE;case 3U: _LLD: _LLE:
 goto _LL10;case 6U: _LLF: _LL10:
 goto _LL12;case 9U: _LL11: _LL12:
 goto _LL14;case 10U: _LL13: _LL14:
 goto _LL16;case 11U: _LL15: _LL16:
 goto _LL18;case 12U: _LL17: _LL18:
 goto _LL1A;case 4U: _LL19: _LL1A:
# 4533
({void*_tmp719=0U;({unsigned int _tmpB7D=d->loc;struct _dyneither_ptr _tmpB7C=({const char*_tmp71A="Cyclone declaration within C code";_tag_dyneither(_tmp71A,sizeof(char),34U);});Cyc_Tcutil_terr(_tmpB7D,_tmpB7C,_tag_dyneither(_tmp719,sizeof(void*),0U));});});
goto _LL0;case 13U: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E:
# 4537
 goto _LL0;}_LL0:;}
# 4541
static void Cyc_Toc_stmttypes_to_c(struct Cyc_Absyn_Stmt*s){
void*_tmp720=s->r;void*_tmp721=_tmp720;struct Cyc_Absyn_Stmt*_tmp73B;struct Cyc_Absyn_Stmt*_tmp73A;struct Cyc_Absyn_Exp*_tmp739;struct Cyc_Absyn_Decl*_tmp738;struct Cyc_Absyn_Stmt*_tmp737;struct Cyc_Absyn_Exp*_tmp736;struct Cyc_List_List*_tmp735;void*_tmp734;struct Cyc_Absyn_Exp*_tmp733;struct Cyc_Absyn_Exp*_tmp732;struct Cyc_Absyn_Exp*_tmp731;struct Cyc_Absyn_Stmt*_tmp730;struct Cyc_Absyn_Exp*_tmp72F;struct Cyc_Absyn_Stmt*_tmp72E;struct Cyc_Absyn_Exp*_tmp72D;struct Cyc_Absyn_Stmt*_tmp72C;struct Cyc_Absyn_Stmt*_tmp72B;struct Cyc_Absyn_Exp*_tmp72A;struct Cyc_Absyn_Stmt*_tmp729;struct Cyc_Absyn_Stmt*_tmp728;struct Cyc_Absyn_Exp*_tmp727;switch(*((int*)_tmp721)){case 1U: _LL1: _tmp727=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp721)->f1;_LL2:
 Cyc_Toc_exptypes_to_c(_tmp727);goto _LL0;case 2U: _LL3: _tmp729=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp721)->f1;_tmp728=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp721)->f2;_LL4:
 Cyc_Toc_stmttypes_to_c(_tmp729);Cyc_Toc_stmttypes_to_c(_tmp728);goto _LL0;case 3U: _LL5: _tmp72A=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp721)->f1;_LL6:
 if(_tmp72A != 0)Cyc_Toc_exptypes_to_c(_tmp72A);goto _LL0;case 4U: _LL7: _tmp72D=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp721)->f1;_tmp72C=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp721)->f2;_tmp72B=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp721)->f3;_LL8:
# 4547
 Cyc_Toc_exptypes_to_c(_tmp72D);Cyc_Toc_stmttypes_to_c(_tmp72C);Cyc_Toc_stmttypes_to_c(_tmp72B);goto _LL0;case 5U: _LL9: _tmp72F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp721)->f1).f1;_tmp72E=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp721)->f2;_LLA:
# 4549
 Cyc_Toc_exptypes_to_c(_tmp72F);Cyc_Toc_stmttypes_to_c(_tmp72E);goto _LL0;case 9U: _LLB: _tmp733=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp721)->f1;_tmp732=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp721)->f2).f1;_tmp731=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp721)->f3).f1;_tmp730=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp721)->f4;_LLC:
# 4551
 Cyc_Toc_exptypes_to_c(_tmp733);Cyc_Toc_exptypes_to_c(_tmp732);Cyc_Toc_exptypes_to_c(_tmp731);
Cyc_Toc_stmttypes_to_c(_tmp730);goto _LL0;case 10U: _LLD: _tmp736=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp721)->f1;_tmp735=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp721)->f2;_tmp734=(void*)((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp721)->f3;_LLE:
# 4554
 Cyc_Toc_exptypes_to_c(_tmp736);
for(0;_tmp735 != 0;_tmp735=_tmp735->tl){Cyc_Toc_stmttypes_to_c(((struct Cyc_Absyn_Switch_clause*)_tmp735->hd)->body);}
goto _LL0;case 12U: _LLF: _tmp738=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp721)->f1;_tmp737=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp721)->f2;_LL10:
 Cyc_Toc_decltypes_to_c(_tmp738);Cyc_Toc_stmttypes_to_c(_tmp737);goto _LL0;case 14U: _LL11: _tmp73A=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp721)->f1;_tmp739=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp721)->f2).f1;_LL12:
 Cyc_Toc_stmttypes_to_c(_tmp73A);Cyc_Toc_exptypes_to_c(_tmp739);goto _LL0;case 13U: _LL13: _tmp73B=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp721)->f2;_LL14:
 Cyc_Toc_stmttypes_to_c(_tmp73B);goto _LL0;case 0U: _LL15: _LL16:
 goto _LL18;case 6U: _LL17: _LL18:
 goto _LL1A;case 7U: _LL19: _LL1A:
 goto _LL1C;case 8U: _LL1B: _LL1C:
# 4564
 goto _LL0;case 11U: _LL1D: _LL1E:
# 4567
({void*_tmpB7E=(void*)({struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct*_tmp722=_cycalloc(sizeof(*_tmp722));_tmp722->tag=0U;_tmp722;});s->r=_tmpB7E;});
goto _LL0;default: _LL1F: _LL20:
# 4570
({void*_tmp723=0U;({unsigned int _tmpB82=s->loc;struct _dyneither_ptr _tmpB81=(struct _dyneither_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp726=({struct Cyc_String_pa_PrintArg_struct _tmp79F;_tmp79F.tag=0U,({struct _dyneither_ptr _tmpB7F=(struct _dyneither_ptr)((struct _dyneither_ptr)Cyc_Absynpp_stmt2string(s));_tmp79F.f1=_tmpB7F;});_tmp79F;});void*_tmp724[1U];_tmp724[0]=& _tmp726;({struct _dyneither_ptr _tmpB80=({const char*_tmp725="Cyclone statement in C code: %s";_tag_dyneither(_tmp725,sizeof(char),32U);});Cyc_aprintf(_tmpB80,_tag_dyneither(_tmp724,sizeof(void*),1U));});});Cyc_Tcutil_terr(_tmpB82,_tmpB81,_tag_dyneither(_tmp723,sizeof(void*),0U));});});
goto _LL0;}_LL0:;}
# 4578
static struct Cyc_Toc_Env*Cyc_Toc_decls_to_c(struct _RegionHandle*r,struct Cyc_Toc_Env*nv,struct Cyc_List_List*ds,int top,int cinclude){
for(0;ds != 0;ds=ds->tl){
if(!Cyc_Toc_is_toplevel(nv))
({void*_tmp73C=0U;({struct _dyneither_ptr _tmpB83=({const char*_tmp73D="decls_to_c: not at toplevel!";_tag_dyneither(_tmp73D,sizeof(char),29U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Tcutil_impos)(_tmpB83,_tag_dyneither(_tmp73C,sizeof(void*),0U));});});
Cyc_Toc_fresh_label_counter=0;{
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
void*_tmp73E=d->r;void*_tmp73F=_tmp73E;struct Cyc_List_List*_tmp757;struct Cyc_List_List*_tmp756;struct Cyc_List_List*_tmp755;struct Cyc_List_List*_tmp754;struct Cyc_Absyn_Typedefdecl*_tmp753;struct Cyc_Absyn_Enumdecl*_tmp752;struct Cyc_Absyn_Datatypedecl*_tmp751;struct Cyc_Absyn_Aggrdecl*_tmp750;struct Cyc_Absyn_Fndecl*_tmp74F;struct Cyc_Absyn_Vardecl*_tmp74E;switch(*((int*)_tmp73F)){case 0U: _LL1: _tmp74E=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp73F)->f1;_LL2: {
# 4586
struct _tuple1*_tmp740=_tmp74E->name;
# 4588
if(_tmp74E->sc == Cyc_Absyn_ExternC)
({struct _tuple1*_tmpB85=({struct _tuple1*_tmp741=_cycalloc(sizeof(*_tmp741));({union Cyc_Absyn_Nmspace _tmpB84=Cyc_Absyn_Abs_n(0,1);_tmp741->f1=_tmpB84;}),_tmp741->f2=(*_tmp740).f2;_tmp741;});_tmp740=_tmpB85;});
if(_tmp74E->initializer != 0){
if(_tmp74E->sc == Cyc_Absyn_ExternC)_tmp74E->sc=Cyc_Absyn_Public;
if(cinclude)
Cyc_Toc_exptypes_to_c((struct Cyc_Absyn_Exp*)_check_null(_tmp74E->initializer));else{
# 4595
Cyc_Toc_exp_to_c(nv,(struct Cyc_Absyn_Exp*)_check_null(_tmp74E->initializer));}}
# 4597
_tmp74E->name=_tmp740;
({enum Cyc_Absyn_Scope _tmpB86=Cyc_Toc_scope_to_c(_tmp74E->sc);_tmp74E->sc=_tmpB86;});
({void*_tmpB87=Cyc_Toc_typ_to_c(_tmp74E->type);_tmp74E->type=_tmpB87;});
({struct Cyc_List_List*_tmpB88=({struct Cyc_List_List*_tmp742=_cycalloc(sizeof(*_tmp742));_tmp742->hd=d,_tmp742->tl=Cyc_Toc_result_decls;_tmp742;});Cyc_Toc_result_decls=_tmpB88;});
goto _LL0;}case 1U: _LL3: _tmp74F=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp73F)->f1;_LL4: {
# 4603
struct _tuple1*_tmp743=_tmp74F->name;
# 4605
if(_tmp74F->sc == Cyc_Absyn_ExternC){
({struct _tuple1*_tmpB8A=({struct _tuple1*_tmp744=_cycalloc(sizeof(*_tmp744));({union Cyc_Absyn_Nmspace _tmpB89=Cyc_Absyn_Abs_n(0,1);_tmp744->f1=_tmpB89;}),_tmp744->f2=(*_tmp743).f2;_tmp744;});_tmp743=_tmpB8A;});
_tmp74F->sc=Cyc_Absyn_Public;}
# 4609
_tmp74F->name=_tmp743;
Cyc_Toc_fndecl_to_c(nv,_tmp74F,cinclude);
({struct Cyc_List_List*_tmpB8B=({struct Cyc_List_List*_tmp745=_cycalloc(sizeof(*_tmp745));_tmp745->hd=d,_tmp745->tl=Cyc_Toc_result_decls;_tmp745;});Cyc_Toc_result_decls=_tmpB8B;});
goto _LL0;}case 2U: _LL5: _LL6:
 goto _LL8;case 3U: _LL7: _LL8:
({void*_tmp746=0U;({struct _dyneither_ptr _tmpB8C=({const char*_tmp747="letdecl at toplevel";_tag_dyneither(_tmp747,sizeof(char),20U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB8C,_tag_dyneither(_tmp746,sizeof(void*),0U));});});case 4U: _LL9: _LLA:
({void*_tmp748=0U;({struct _dyneither_ptr _tmpB8D=({const char*_tmp749="region decl at toplevel";_tag_dyneither(_tmp749,sizeof(char),24U);});((int(*)(struct _dyneither_ptr fmt,struct _dyneither_ptr ap))Cyc_Toc_toc_impos)(_tmpB8D,_tag_dyneither(_tmp748,sizeof(void*),0U));});});case 5U: _LLB: _tmp750=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp73F)->f1;_LLC:
# 4617
 Cyc_Toc_aggrdecl_to_c(_tmp750,1);
goto _LL0;case 6U: _LLD: _tmp751=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp73F)->f1;_LLE:
# 4620
 if(_tmp751->is_extensible)
Cyc_Toc_xdatatypedecl_to_c(_tmp751);else{
# 4623
Cyc_Toc_datatypedecl_to_c(_tmp751);}
goto _LL0;case 7U: _LLF: _tmp752=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp73F)->f1;_LL10:
# 4626
 Cyc_Toc_enumdecl_to_c(_tmp752);
({struct Cyc_List_List*_tmpB8E=({struct Cyc_List_List*_tmp74A=_cycalloc(sizeof(*_tmp74A));_tmp74A->hd=d,_tmp74A->tl=Cyc_Toc_result_decls;_tmp74A;});Cyc_Toc_result_decls=_tmpB8E;});
goto _LL0;case 8U: _LL11: _tmp753=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp73F)->f1;_LL12:
# 4630
 _tmp753->tvs=0;
if(_tmp753->defn != 0)
({void*_tmpB8F=Cyc_Toc_typ_to_c((void*)_check_null(_tmp753->defn));_tmp753->defn=_tmpB8F;});else{
# 4634
enum Cyc_Absyn_KindQual _tmp74B=((struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp753->kind))->v)->kind;enum Cyc_Absyn_KindQual _tmp74C=_tmp74B;if(_tmp74C == Cyc_Absyn_BoxKind){_LL20: _LL21:
({void*_tmpB90=Cyc_Absyn_void_star_typ();_tmp753->defn=_tmpB90;});goto _LL1F;}else{_LL22: _LL23:
 _tmp753->defn=(void*)& Cyc_Absyn_VoidType_val;goto _LL1F;}_LL1F:;}
# 4642
if(Cyc_noexpand_r)
({struct Cyc_List_List*_tmpB91=({struct Cyc_List_List*_tmp74D=_cycalloc(sizeof(*_tmp74D));_tmp74D->hd=d,_tmp74D->tl=Cyc_Toc_result_decls;_tmp74D;});Cyc_Toc_result_decls=_tmpB91;});
goto _LL0;case 13U: _LL13: _LL14:
 goto _LL16;case 14U: _LL15: _LL16:
 goto _LL0;case 9U: _LL17: _tmp754=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp73F)->f2;_LL18:
 _tmp755=_tmp754;goto _LL1A;case 10U: _LL19: _tmp755=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp73F)->f2;_LL1A:
 _tmp756=_tmp755;goto _LL1C;case 11U: _LL1B: _tmp756=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp73F)->f1;_LL1C:
({struct Cyc_Toc_Env*_tmpB92=Cyc_Toc_decls_to_c(r,nv,_tmp756,top,cinclude);nv=_tmpB92;});goto _LL0;default: _LL1D: _tmp757=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp73F)->f1;_LL1E:
({struct Cyc_Toc_Env*_tmpB93=Cyc_Toc_decls_to_c(r,nv,_tmp757,top,1);nv=_tmpB93;});goto _LL0;}_LL0:;};}
# 4653
return nv;}
# 4657
static void Cyc_Toc_init(){
struct Cyc_Core_NewDynamicRegion _tmp758=Cyc_Core_new_rckey();struct Cyc_Core_NewDynamicRegion _tmp759=_tmp758;struct Cyc_Core_DynamicRegion*_tmp75E;_LL1: _tmp75E=_tmp759.key;_LL2:;{
struct Cyc_Toc_TocState*ts;
{struct _RegionHandle*h=& _tmp75E->h;
({struct Cyc_Toc_TocState*_tmpB94=Cyc_Toc_empty_toc_state(h);ts=_tmpB94;});;}
({struct Cyc_Toc_TocStateWrap*_tmpB95=({struct Cyc_Toc_TocStateWrap*_tmp75B=_region_malloc(Cyc_Core_unique_region,sizeof(*_tmp75B));_tmp75B->dyn=_tmp75E,_tmp75B->state=ts;_tmp75B;});Cyc_Toc_toc_state=_tmpB95;});
Cyc_Toc_result_decls=0;
Cyc_Toc_tuple_type_counter=0;
Cyc_Toc_temp_var_counter=0;
Cyc_Toc_fresh_label_counter=0;
Cyc_Toc_total_bounds_checks=0U;
Cyc_Toc_bounds_checks_eliminated=0U;
({struct _dyneither_ptr _tmpB96=_tag_dyneither(({unsigned int _tmp75D=39;struct _dyneither_ptr**_tmp75C=_cycalloc(_check_times(_tmp75D,sizeof(struct _dyneither_ptr*)));_tmp75C[0]=& Cyc_Toc__throw_str,_tmp75C[1]=& Cyc_Toc_setjmp_str,_tmp75C[2]=& Cyc_Toc__push_handler_str,_tmp75C[3]=& Cyc_Toc__pop_handler_str,_tmp75C[4]=& Cyc_Toc__exn_thrown_str,_tmp75C[5]=& Cyc_Toc__npop_handler_str,_tmp75C[6]=& Cyc_Toc__check_null_str,_tmp75C[7]=& Cyc_Toc__check_known_subscript_null_str,_tmp75C[8]=& Cyc_Toc__check_known_subscript_notnull_str,_tmp75C[9]=& Cyc_Toc__check_dyneither_subscript_str,_tmp75C[10]=& Cyc_Toc__dyneither_ptr_str,_tmp75C[11]=& Cyc_Toc__tag_dyneither_str,_tmp75C[12]=& Cyc_Toc__init_dyneither_ptr_str,_tmp75C[13]=& Cyc_Toc__untag_dyneither_ptr_str,_tmp75C[14]=& Cyc_Toc__get_dyneither_size_str,_tmp75C[15]=& Cyc_Toc__get_zero_arr_size_str,_tmp75C[16]=& Cyc_Toc__dyneither_ptr_plus_str,_tmp75C[17]=& Cyc_Toc__zero_arr_plus_str,_tmp75C[18]=& Cyc_Toc__dyneither_ptr_inplace_plus_str,_tmp75C[19]=& Cyc_Toc__zero_arr_inplace_plus_str,_tmp75C[20]=& Cyc_Toc__dyneither_ptr_inplace_plus_post_str,_tmp75C[21]=& Cyc_Toc__zero_arr_inplace_plus_post_str,_tmp75C[22]=& Cyc_Toc__cycalloc_str,_tmp75C[23]=& Cyc_Toc__cyccalloc_str,_tmp75C[24]=& Cyc_Toc__cycalloc_atomic_str,_tmp75C[25]=& Cyc_Toc__cyccalloc_atomic_str,_tmp75C[26]=& Cyc_Toc__region_malloc_str,_tmp75C[27]=& Cyc_Toc__region_calloc_str,_tmp75C[28]=& Cyc_Toc__check_times_str,_tmp75C[29]=& Cyc_Toc__new_region_str,_tmp75C[30]=& Cyc_Toc__push_region_str,_tmp75C[31]=& Cyc_Toc__pop_region_str,_tmp75C[32]=& Cyc_Toc__open_dynregion_str,_tmp75C[33]=& Cyc_Toc__push_dynregion_str,_tmp75C[34]=& Cyc_Toc__pop_dynregion_str,_tmp75C[35]=& Cyc_Toc__throw_arraybounds_str,_tmp75C[36]=& Cyc_Toc__dyneither_ptr_decrease_size_str,_tmp75C[37]=& Cyc_Toc__throw_match_str,_tmp75C[38]=& Cyc_Toc__fast_region_malloc_str;_tmp75C;}),sizeof(struct _dyneither_ptr*),39U);Cyc_Toc_globals=_tmpB96;});};}
# 4712
void Cyc_Toc_finish(){
struct Cyc_Toc_TocStateWrap*ts=0;
({struct Cyc_Toc_TocStateWrap*_tmp75F=ts;struct Cyc_Toc_TocStateWrap*_tmp760=Cyc_Toc_toc_state;ts=_tmp760;Cyc_Toc_toc_state=_tmp75F;});{
struct Cyc_Toc_TocStateWrap _tmp761=*((struct Cyc_Toc_TocStateWrap*)_check_null(ts));struct Cyc_Toc_TocStateWrap _tmp762=_tmp761;struct Cyc_Core_DynamicRegion*_tmp768;struct Cyc_Toc_TocState*_tmp767;_LL1: _tmp768=_tmp762.dyn;_tmp767=_tmp762.state;_LL2:;
# 4717
{struct _RegionHandle*h=& _tmp768->h;
{struct Cyc_Toc_TocState _tmp764=*_tmp767;struct Cyc_Toc_TocState _tmp765=_tmp764;struct Cyc_Xarray_Xarray*_tmp766;_LL4: _tmp766=_tmp765.temp_labels;_LL5:;
((void(*)(struct Cyc_Xarray_Xarray*xarr))Cyc_Xarray_reuse)(_tmp766);}
# 4718
;}
# 4721
Cyc_Core_free_rckey(_tmp768);
((void(*)(struct Cyc_Toc_TocStateWrap*ptr))Cyc_Core_ufree)(ts);
# 4724
Cyc_Toc_gpop_tables=0;
Cyc_Toc_fn_pop_table=0;};}
# 4730
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds){
# 4732
({struct Cyc_Hashtable_Table**_tmpB97=({struct Cyc_Hashtable_Table**_tmp769=_cycalloc(sizeof(*_tmp769));*_tmp769=pop_tables;_tmp769;});Cyc_Toc_gpop_tables=_tmpB97;});
Cyc_Toc_init();
{struct _RegionHandle _tmp76A=_new_region("start");struct _RegionHandle*start=& _tmp76A;_push_region(start);
({struct _RegionHandle*_tmpB99=start;struct Cyc_Toc_Env*_tmpB98=Cyc_Toc_empty_env(start);Cyc_Toc_decls_to_c(_tmpB99,_tmpB98,ds,1,0);});;_pop_region(start);}
# 4741
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(Cyc_Toc_result_decls);}
